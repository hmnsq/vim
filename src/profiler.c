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

#شامل "vim.h"

#اگر defined(FEAT_EVAL)
# اگر defined(FEAT_PROFILE) || defined(FEAT_RELTIME)
/*
 * Store the current time in "tm".
 */
    تهی
profile_start(proftime_T *tm)
{
#  ifdef MSWIN
    QueryPerformanceCounter(tm);
#  وگرنه
    PROF_GET_TIME(tm);
#  endif
}

/*
 * Compute the elapsed time from "tm" till now and store in "tm".
 */
    تهی
profile_end(proftime_T *tm)
{
    proftime_T now;

#  ifdef MSWIN
    QueryPerformanceCounter(&now);
    tm->QuadPart = now.QuadPart - tm->QuadPart;
#  وگرنه
    PROF_GET_TIME(&now);
    tm->tv_fsec = now.tv_fsec - tm->tv_fsec;
    tm->tv_sec = now.tv_sec - tm->tv_sec;
    اگر (tm->tv_fsec < 0)
    {
	tm->tv_fsec += TV_FSEC_SEC;
	--tm->tv_sec;
    }
#  endif
}

/*
 * Subtract the time "tm2" from "tm".
 */
    تهی
profile_sub(proftime_T *tm, proftime_T *tm2)
{
#  ifdef MSWIN
    tm->QuadPart -= tm2->QuadPart;
#  وگرنه
    tm->tv_fsec -= tm2->tv_fsec;
    tm->tv_sec -= tm2->tv_sec;
    اگر (tm->tv_fsec < 0)
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
    نویسه *
profile_پیام(proftime_T *tm)
{
    ایستا نویسه buf[50];

#  ifdef MSWIN
    LARGE_INTEGER   fr;

    QueryPerformanceFrequency(&fr);
    sprintf(buf, "%10.6lf", (دوگانه)tm->QuadPart / (دوگانه)fr.QuadPart);
#  وگرنه
    sprintf(buf, PROF_TIME_FORMAT, (long)tm->tv_sec, (long)tm->tv_fsec);
#  endif
    بازگشت buf;
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
    بازگشت (float_T)tm->QuadPart / (float_T)fr.QuadPart;
#  وگرنه
    بازگشت (float_T)tm->tv_sec + (float_T)tm->tv_fsec / (float_T)TV_FSEC_SEC;
#  endif
}

/*
 * Put the time "msec" past now in "tm".
 */
    تهی
profile_setlimit(long msec, proftime_T *tm)
{
    اگر (msec <= 0)   // no limit
	profile_zero(tm);
    وگرنه
    {
#  ifdef MSWIN
	LARGE_INTEGER   fr;

	QueryPerformanceCounter(tm);
	QueryPerformanceFrequency(&fr);
	tm->QuadPart += (LONGLONG)((دوگانه)msec / 1000.0 * (دوگانه)fr.QuadPart);
#  وگرنه
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
    صحیح
profile_passed_limit(proftime_T *tm)
{
    proftime_T	now;

#  ifdef MSWIN
    اگر (tm->QuadPart == 0)  // timer was not set
	بازگشت FALSE;
    QueryPerformanceCounter(&now);
    بازگشت (now.QuadPart > tm->QuadPart);
#  وگرنه
    اگر (tm->tv_sec == 0)    // timer was not set
	بازگشت FALSE;
    PROF_GET_TIME(&now);
    بازگشت (now.tv_sec > tm->tv_sec
	    || (now.tv_sec == tm->tv_sec && now.tv_fsec > tm->tv_fsec));
#  endif
}

/*
 * Set the time in "tm" to zero.
 */
    تهی
profile_zero(proftime_T *tm)
{
#  ifdef MSWIN
    tm->QuadPart = 0;
#  وگرنه
    tm->tv_fsec = 0;
    tm->tv_sec = 0;
#  endif
}

# endif  // FEAT_PROFILE || FEAT_RELTIME

# اگر defined(FEAT_SYN_HL) && defined(FEAT_RELTIME) && defined(FEAT_PROFILE)
#  اگر defined(HAVE_MATH_H)
#   include <math.h>
#  endif

/*
 * Divide the time "tm" by "count" and store in "tm2".
 */
    تهی
profile_divide(proftime_T *tm, صحیح شمار, proftime_T *tm2)
{
    اگر (شمار == 0)
	profile_zero(tm2);
    وگرنه
    {
#  ifdef MSWIN
	tm2->QuadPart = tm->QuadPart / شمار;
#  وگرنه
	دوگانه fsec = (tm->tv_sec * (float_T)TV_FSEC_SEC + tm->tv_fsec)
								       / شمار;

	tm2->tv_sec = floor(fsec / (float_T)TV_FSEC_SEC);
	tm2->tv_fsec = vim_round(fsec - (tm2->tv_sec * (float_T)TV_FSEC_SEC));
#  endif
    }
}
# endif

# اگر defined(FEAT_PROFILE)
/*
 * Functions for profiling.
 */
ایستا proftime_T prof_wait_time;

/*
 * Add the time "tm2" to "tm".
 */
    تهی
profile_add(proftime_T *tm, proftime_T *tm2)
{
#  ifdef MSWIN
    tm->QuadPart += tm2->QuadPart;
#  وگرنه
    tm->tv_fsec += tm2->tv_fsec;
    tm->tv_sec += tm2->tv_sec;
    اگر (tm->tv_fsec >= TV_FSEC_SEC)
    {
	tm->tv_fsec -= TV_FSEC_SEC;
	++tm->tv_sec;
    }
#  endif
}

/*
 * Add the "self" time from the total time and the children's time.
 */
    تهی
profile_self(proftime_T *self, proftime_T *total, proftime_T *children)
{
    // Check that the result won't be negative.  Can happen with recursive
    // calls.
#  ifdef MSWIN
    اگر (total->QuadPart <= children->QuadPart)
	بازگشت;
#  وگرنه
    اگر (total->tv_sec < children->tv_sec
	    || (total->tv_sec == children->tv_sec
		&& total->tv_fsec <= children->tv_fsec))
	بازگشت;
#  endif
    profile_add(self, total);
    profile_sub(self, children);
}

/*
 * Get the current waittime.
 */
    ایستا تهی
profile_get_wait(proftime_T *tm)
{
    *tm = prof_wait_time;
}

/*
 * Subtract the passed waittime since "tm" from "tma".
 */
    تهی
profile_sub_wait(proftime_T *tm, proftime_T *tma)
{
    proftime_T tm3 = prof_wait_time;

    profile_sub(&tm3, tm);
    profile_sub(tma, &tm3);
}

/*
 * Return TRUE if "tm1" and "tm2" are equal.
 */
    ایستا صحیح
profile_equal(proftime_T *tm1, proftime_T *tm2)
{
#  ifdef MSWIN
    بازگشت (tm1->QuadPart == tm2->QuadPart);
#  وگرنه
    بازگشت (tm1->tv_fsec == tm2->tv_fsec && tm1->tv_sec == tm2->tv_sec);
#  endif
}

/*
 * Return <0, 0 or >0 if "tm1" < "tm2", "tm1" == "tm2" or "tm1" > "tm2"
 */
    صحیح
profile_cmp(ثابت proftime_T *tm1, ثابت proftime_T *tm2)
{
#  ifdef MSWIN
    بازگشت tm2->QuadPart == tm1->QuadPart ? 0 :
	tm2->QuadPart > tm1->QuadPart ? 1 : -1;
#  وگرنه
    اگر (tm1->tv_sec == tm2->tv_sec)
	بازگشت tm2->tv_fsec == tm1->tv_fsec ? 0 :
	    tm2->tv_fsec > tm1->tv_fsec ? 1 : -1;
    بازگشت tm2->tv_sec > tm1->tv_sec ? 1 : -1;
#  endif
}

ایستا char_u	*profile_fname = پوچ;
ایستا proftime_T pause_time;

/*
 * Reset all profiling information.
 */
    ایستا تهی
profile_reset(تهی)
{
    صحیح		id;
    صحیح		todo;
    hashtab_T	*functbl;
    hashitem_T	*hi;

    // Reset sourced files.
    برای (id = 1; id <= script_اقلام.ga_len; id++)
    {
	scriptitem_T *si = SCRIPT_ITEM(id);
	اگر (si->sn_prof_on)
	{
	    si->sn_prof_on      = FALSE;
	    si->sn_pr_force     = FALSE;
	    profile_zero(&si->sn_pr_child);
	    si->sn_pr_nest      = 0;
	    si->sn_pr_شمار     = 0;
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
    todo = (صحیح)functbl->ht_used;

    FOR_ALL_HASHTAB_اقلام(functbl, hi, todo)
    {
	ufunc_T *fp;
	صحیح	i;

	اگر (HASHITEM_EMPTY(hi))
	    ادامه;

	todo--;
	fp = HI2UF(hi);
	اگر (fp->uf_prof_initialized)
	{
	    fp->uf_profiling    = 0;
	    fp->uf_prof_initialized = FALSE;
	    fp->uf_tm_شمار     = 0;
	    profile_zero(&fp->uf_tm_total);
	    profile_zero(&fp->uf_tm_self);
	    profile_zero(&fp->uf_tm_children);

	    برای (i = 0; i < fp->uf_سطرها.ga_len; i++)
	    {
		fp->uf_tml_شمار[i] = 0;
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
    تهی
ex_profile(exarg_T *eap)
{
    char_u	*e;
    صحیح		len;

    e = skiptowhite(eap->arg);
    len = (صحیح)(e - eap->arg);
    e = skipwhite(e);

    اگر (len == 5 && STRNCMP(eap->arg, "start", 5) == 0 && *e != NUL)
    {
	VIM_CLEAR(profile_fname);
	profile_fname = expand_env_save_opt(e, TRUE, پوچ);
	do_profiling = PROF_YES;
	profile_zero(&prof_wait_time);
	set_vim_var_nr(VV_PROFILING, 1L);
    }
    وگرنه اگر (do_profiling == PROF_NONE)
	emsg(_(e_first_use_profile_start_fname));
    وگرنه اگر (STRCMP(eap->arg, "stop") == 0)
    {
	profile_dump();
	do_profiling = PROF_NONE;
	set_vim_var_nr(VV_PROFILING, 0L);
	profile_reset();
    }
    وگرنه اگر (STRCMP(eap->arg, "pause") == 0)
    {
	اگر (do_profiling == PROF_YES)
	    profile_start(&pause_time);
	do_profiling = PROF_PAUSED;
    }
    وگرنه اگر (STRCMP(eap->arg, "continue") == 0)
    {
	اگر (do_profiling == PROF_PAUSED)
	{
	    profile_end(&pause_time);
	    profile_add(&prof_wait_time, &pause_time);
	}
	do_profiling = PROF_YES;
    }
    وگرنه اگر (STRCMP(eap->arg, "dump") == 0)
	profile_dump();
    وگرنه
    {
	// The rest is similar to ":breakadd".
	ex_breakadd(eap);
    }
}

// Command line expansion for :profile.
ایستا شمارش
{
    PEXP_SUBCMD,	// expand :profile sub-commands
    PEXP_FUNC		// expand :profile func {funcname}
} pexpand_what;

ایستا نویسه *pexpand_cmds[] = {
			"start",
			"stop",
			"pause",
			"continue",
			"func",
			"file",
			"dump",
			پوچ
};

/*
 * Function given to ExpandGeneric() to obtain the profile command
 * specific expansion.
 */
    char_u *
get_profile_name(expand_T *xp UNUSED, صحیح idx)
{
    انتخاب (pexpand_what)
    {
    حالت PEXP_SUBCMD:
	بازگشت (char_u *)pexpand_cmds[idx];
    پیشفرض:
	بازگشت پوچ;
    }
}

/*
 * Handle command line completion for :profile command.
 */
    تهی
set_context_in_profile_cmd(expand_T *xp, char_u *arg)
{
    char_u	*end_subcmd;

    // Default: expand subcommands.
    xp->xp_context = EXPAND_PROFILE;
    pexpand_what = PEXP_SUBCMD;
    xp->xp_pattern = arg;

    end_subcmd = skiptowhite(arg);
    اگر (*end_subcmd == NUL)
	بازگشت;

    اگر ((end_subcmd - arg == 5 && STRNCMP(arg, "start", 5) == 0)
	    || (end_subcmd - arg == 4 && STRNCMP(arg, "file", 4) == 0))
    {
	xp->xp_context = EXPAND_FILES;
	xp->xp_pattern = skipwhite(end_subcmd);
	بازگشت;
    }
    وگرنه اگر (end_subcmd - arg == 4 && STRNCMP(arg, "func", 4) == 0)
    {
	xp->xp_context = EXPAND_USER_FUNC;
	xp->xp_pattern = skipwhite(end_subcmd);
	بازگشت;
    }

    xp->xp_context = EXPAND_NOTHING;
}

ایستا proftime_T inchar_time;

/*
 * Called when starting to wait for the user to type a character.
 */
    تهی
prof_inchar_enter(تهی)
{
    profile_start(&inchar_time);
}

/*
 * Called when finished waiting for the user to type a character.
 */
    تهی
prof_inchar_exit(تهی)
{
    profile_end(&inchar_time);
    profile_add(&prof_wait_time, &inchar_time);
}


/*
 * Return TRUE when a function defined in the current script should be
 * profiled.
 */
    صحیح
prof_def_func(تهی)
{
    اگر (current_sctx.sc_sid > 0)
	بازگشت SCRIPT_ITEM(current_sctx.sc_sid)->sn_pr_force;
    بازگشت FALSE;
}

/*
 * Print the count and times for one function or function line.
 */
    ایستا تهی
prof_func_سطر(
    FILE	*fd,
    صحیح		شمار,
    proftime_T	*total,
    proftime_T	*self,
    صحیح		prefer_self)	// when equal print only self time
{
    اگر (شمار > 0)
    {
	fprintf(fd, "%5d ", شمار);
	اگر (prefer_self && profile_equal(total, self))
	    fprintf(fd, PROF_TIME_BLANK);
	وگرنه
	    fprintf(fd, "%s ", profile_پیام(total));
	اگر (!prefer_self && profile_equal(total, self))
	    fprintf(fd, PROF_TIME_BLANK);
	وگرنه
	    fprintf(fd, "%s ", profile_پیام(self));
    }
    وگرنه
	fprintf(fd, "      %s%s", PROF_TIME_BLANK, PROF_TIME_BLANK);
}

    ایستا تهی
prof_sort_list(
    FILE	*fd,
    ufunc_T	**sorttab,
    صحیح		st_len,
    نویسه	*title,
    صحیح		prefer_self)	// when equal print only self time
{
    صحیح		i;
    ufunc_T	*fp;

    fprintf(fd, "FUNCTIONS SORTED ON %s TIME\n", title);
    fprintf(fd, "%s  function\n", PROF_TOTALS_HEADER);
    برای (i = 0; i < 20 && i < st_len; ++i)
    {
	fp = sorttab[i];
	prof_func_سطر(fd, fp->uf_tm_شمار, &fp->uf_tm_total, &fp->uf_tm_self,
								 prefer_self);
	اگر (fp->uf_name[0] == K_SPECIAL)
	    fprintf(fd, " <SNR>%s()\n", fp->uf_name + 3);
	وگرنه
	    fprintf(fd, " %s()\n", fp->uf_name);
    }
    fprintf(fd, "\n");
}

/*
 * Compare function for total time sorting.
 */
    ایستا صحیح
prof_total_cmp(ثابت تهی *s1, ثابت تهی *s2)
{
    ufunc_T	*p1, *p2;

    p1 = *(ufunc_T **)s1;
    p2 = *(ufunc_T **)s2;
    بازگشت profile_cmp(&p1->uf_tm_total, &p2->uf_tm_total);
}

/*
 * Compare function for self time sorting.
 */
    ایستا صحیح
prof_self_cmp(ثابت تهی *s1, ثابت تهی *s2)
{
    ufunc_T	*p1, *p2;

    p1 = *(ufunc_T **)s1;
    p2 = *(ufunc_T **)s2;
    بازگشت profile_cmp(&p1->uf_tm_self, &p2->uf_tm_self);
}

/*
 * Start profiling function "fp".
 */
    تهی
func_do_profile(ufunc_T *fp)
{
    صحیح		len = fp->uf_سطرها.ga_len;

    اگر (!fp->uf_prof_initialized)
    {
	اگر (len == 0)
	    len = 1;  // avoid getting error for allocating zero bytes
	fp->uf_tm_شمار = 0;
	profile_zero(&fp->uf_tm_self);
	profile_zero(&fp->uf_tm_total);
	اگر (fp->uf_tml_شمار == پوچ)
	    fp->uf_tml_شمار = ALLOC_CLEAR_MULT(صحیح, len);
	اگر (fp->uf_tml_total == پوچ)
	    fp->uf_tml_total = ALLOC_CLEAR_MULT(proftime_T, len);
	اگر (fp->uf_tml_self == پوچ)
	    fp->uf_tml_self = ALLOC_CLEAR_MULT(proftime_T, len);
	fp->uf_tml_idx = -1;
	اگر (fp->uf_tml_شمار == پوچ || fp->uf_tml_total == پوچ
						    || fp->uf_tml_self == پوچ)
	    بازگشت;	    // out of memory
	fp->uf_prof_initialized = TRUE;
    }

    fp->uf_profiling = TRUE;
}

/*
 * Save time when starting to invoke another script or function.
 */
    ایستا تهی
script_prof_save(
    proftime_T	*tm)	    // place to store wait time
{
    scriptitem_T    *si;

    اگر (SCRIPT_ID_VALID(current_sctx.sc_sid))
    {
	si = SCRIPT_ITEM(current_sctx.sc_sid);
	اگر (si->sn_prof_on && si->sn_pr_nest++ == 0)
	    profile_start(&si->sn_pr_child);
    }
    profile_get_wait(tm);
}

/*
 * When calling a function: may initialize for profiling.
 */
    تهی
profile_may_start_func(profinfo_T *info, ufunc_T *fp, ufunc_T *caller)
{
    اگر (!fp->uf_profiling && has_profiling(FALSE, fp->uf_name, پوچ,
								&fp->uf_hash))
    {
	info->pi_started_profiling = TRUE;
	func_do_profile(fp);
    }
    اگر (fp->uf_profiling || (caller != پوچ && caller->uf_profiling))
    {
	++fp->uf_tm_شمار;
	profile_start(&info->pi_call_start);
	profile_zero(&fp->uf_tm_children);
    }
    script_prof_save(&info->pi_wait_start);
}

/*
 * After calling a function: may handle profiling.  profile_may_start_func()
 * must have been called previously.
 */
    تهی
profile_may_end_func(profinfo_T *info, ufunc_T *fp, ufunc_T *caller)
{
    profile_end(&info->pi_call_start);
    profile_sub_wait(&info->pi_wait_start, &info->pi_call_start);
    profile_add(&fp->uf_tm_total, &info->pi_call_start);
    profile_self(&fp->uf_tm_self, &info->pi_call_start, &fp->uf_tm_children);
    اگر (caller != پوچ && caller->uf_profiling)
    {
	profile_add(&caller->uf_tm_children, &info->pi_call_start);
	profile_add(&caller->uf_tml_children, &info->pi_call_start);
    }
    اگر (info->pi_started_profiling)
	// make a ":profdel func" stop profiling the function
	fp->uf_profiling = FALSE;
}

/*
 * Prepare profiling for entering a child or something else that is not
 * counted for the script/function itself.
 * Should always be called in pair with prof_child_exit().
 */
    تهی
prof_child_enter(
    proftime_T *tm)	// place to store waittime
{
    funccall_T *fc = get_current_funccal();

    اگر (fc != پوچ && fc->fc_func->uf_profiling)
	profile_start(&fc->fc_prof_child);
    script_prof_save(tm);
}

/*
 * Take care of time spent in a child.
 * Should always be called after prof_child_enter().
 */
    تهی
prof_child_exit(
    proftime_T *tm)	// where waittime was stored
{
    funccall_T *fc = get_current_funccal();

    اگر (fc != پوچ && fc->fc_func->uf_profiling)
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
    تهی
func_سطر_start(تهی *cookie, long lnum)
{
    funccall_T	*fcp = (funccall_T *)cookie;
    ufunc_T	*fp = fcp->fc_func;

    اگر (fp->uf_profiling && lnum >= 1 && lnum <= fp->uf_سطرها.ga_len)
    {
	fp->uf_tml_idx = lnum - 1;
	// Skip continuation lines.
	وقتی (fp->uf_tml_idx > 0 && FUNCLINE(fp, fp->uf_tml_idx) == پوچ)
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
    تهی
func_سطر_exec(تهی *cookie)
{
    funccall_T	*fcp = (funccall_T *)cookie;
    ufunc_T	*fp = fcp->fc_func;

    اگر (fp->uf_profiling && fp->uf_tml_idx >= 0)
	fp->uf_tml_execed = TRUE;
}

/*
 * Called when done with a function line.
 */
    تهی
func_سطر_end(تهی *cookie)
{
    funccall_T	*fcp = (funccall_T *)cookie;
    ufunc_T	*fp = fcp->fc_func;

    اگر (fp->uf_profiling && fp->uf_tml_idx >= 0)
    {
	اگر (fp->uf_tml_execed)
	{
	    ++fp->uf_tml_شمار[fp->uf_tml_idx];
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
    ایستا تهی
func_dump_profile(FILE *fd)
{
    hashtab_T	*functbl;
    hashitem_T	*hi;
    صحیح		todo;
    ufunc_T	*fp;
    صحیح		i;
    ufunc_T	**sorttab;
    صحیح		st_len = 0;
    char_u	*p;

    functbl = func_tbl_get();
    todo = (صحیح)functbl->ht_used;
    اگر (todo == 0)
	بازگشت;     // nothing to dump

    sorttab = ALLOC_MULT(ufunc_T *, todo);

    FOR_ALL_HASHTAB_اقلام(functbl, hi, todo)
    {
	اگر (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    fp = HI2UF(hi);
	    اگر (fp->uf_prof_initialized)
	    {
		اگر (sorttab != پوچ)
		    sorttab[st_len++] = fp;

		اگر (fp->uf_name[0] == K_SPECIAL)
		    fprintf(fd, "FUNCTION  <SNR>%s()\n", fp->uf_name + 3);
		وگرنه
		    fprintf(fd, "FUNCTION  %s()\n", fp->uf_name);
		اگر (fp->uf_script_ctx.sc_sid > 0)
		{
		    p = home_replace_save(پوچ,
				     get_scriptname(fp->uf_script_ctx.sc_sid));
		    اگر (p != پوچ)
		    {
			fprintf(fd, "    Defined: %s:%ld\n",
					   p, (long)fp->uf_script_ctx.sc_lnum);
			vim_free(p);
		    }
		}
		اگر (fp->uf_tm_شمار == 1)
		    fprintf(fd, "Called 1 time\n");
		وگرنه
		    fprintf(fd, "Called %d times\n", fp->uf_tm_شمار);
		fprintf(fd, "Total time: %s\n", profile_پیام(&fp->uf_tm_total));
		fprintf(fd, " Self time: %s\n", profile_پیام(&fp->uf_tm_self));
		fprintf(fd, "\n");
		fprintf(fd, "%s\n", PROF_TOTALS_HEADER);

		برای (i = 0; i < fp->uf_سطرها.ga_len; ++i)
		{
		    اگر (FUNCLINE(fp, i) == پوچ)
			ادامه;
		    prof_func_سطر(fd, fp->uf_tml_شمار[i],
			     &fp->uf_tml_total[i], &fp->uf_tml_self[i], TRUE);
		    fprintf(fd, "%s\n", FUNCLINE(fp, i));
		}
		fprintf(fd, "\n");
	    }
	}
    }

    اگر (sorttab != پوچ && st_len > 0)
    {
	qsort((تهی *)sorttab, (size_t)st_len, اندازه(ufunc_T *),
							      prof_total_cmp);
	prof_sort_list(fd, sorttab, st_len, "TOTAL", FALSE);
	qsort((تهی *)sorttab, (size_t)st_len, اندازه(ufunc_T *),
							      prof_self_cmp);
	prof_sort_list(fd, sorttab, st_len, "SELF", TRUE);
    }

    vim_free(sorttab);
}

/*
 * Start profiling script "fp".
 */
    تهی
script_do_profile(scriptitem_T *si)
{
    si->sn_pr_شمار = 0;
    profile_zero(&si->sn_pr_total);
    profile_zero(&si->sn_pr_self);

    ga_init2(&si->sn_prl_ga, اندازه(sn_prl_T), 100);
    si->sn_prl_idx = -1;
    si->sn_prof_on = TRUE;
    si->sn_pr_nest = 0;
}

/*
 * Count time spent in children after invoking another script or function.
 */
    تهی
script_prof_restore(proftime_T *tm)
{
    scriptitem_T    *si;

    اگر (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	بازگشت;

    si = SCRIPT_ITEM(current_sctx.sc_sid);
    اگر (si->sn_prof_on && --si->sn_pr_nest == 0)
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
    ایستا تهی
script_dump_profile(FILE *fd)
{
    صحیح		    id;
    scriptitem_T    *si;
    صحیح		    i;
    FILE	    *sfd;
    sn_prl_T	    *pp;

    برای (id = 1; id <= script_اقلام.ga_len; ++id)
    {
	si = SCRIPT_ITEM(id);
	اگر (si->sn_prof_on)
	{
	    fprintf(fd, "SCRIPT  %s\n", si->sn_name);
	    اگر (si->sn_pr_شمار == 1)
		fprintf(fd, "Sourced 1 time\n");
	    وگرنه
		fprintf(fd, "Sourced %d times\n", si->sn_pr_شمار);
	    fprintf(fd, "Total time: %s\n", profile_پیام(&si->sn_pr_total));
	    fprintf(fd, " Self time: %s\n", profile_پیام(&si->sn_pr_self));
	    fprintf(fd, "\n");
	    fprintf(fd, "%s\n", PROF_TOTALS_HEADER);

	    sfd = mch_fopen((نویسه *)si->sn_name, "r");
	    اگر (sfd == پوچ)
		fprintf(fd, "Cannot open file!\n");
	    وگرنه
	    {
		// Keep going till the end of file, so that trailing
		// continuation lines are listed.
		برای (i = 0; ; ++i)
		{
		    اگر (vim_fgets(IObuff, IOSIZE, sfd))
			بشکن;
		    // When a line has been truncated, append NL, taking care
		    // of multi-byte characters .
		    اگر (IObuff[IOSIZE - 2] != NUL && IObuff[IOSIZE - 2] != NL)
		    {
			صحیح n = IOSIZE - 2;

			اگر (enc_utf8)
			{
			    // Move to the first byte of this char.
			    // utf_head_off() doesn't work, because it checks
			    // for a truncated character.
			    وقتی (n > 0 && (IObuff[n] & 0xc0) == 0x80)
				--n;
			}
			وگرنه اگر (has_mbyte)
			    n -= mb_head_off(IObuff, IObuff + n);
			IObuff[n] = NL;
			IObuff[n + 1] = NUL;
		    }
		    اگر (i < si->sn_prl_ga.ga_len
				     && (pp = &PRL_ITEM(si, i))->snp_شمار > 0)
		    {
			fprintf(fd, "%5d ", pp->snp_شمار);
			اگر (profile_equal(&pp->sn_prl_total, &pp->sn_prl_self))
			    fprintf(fd, "           ");
			وگرنه
			    fprintf(fd, "%s ", profile_پیام(&pp->sn_prl_total));
			fprintf(fd, "%s ", profile_پیام(&pp->sn_prl_self));
		    }
		    وگرنه
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
    تهی
profile_dump(تهی)
{
    FILE	*fd;

    اگر (profile_fname == پوچ)
	بازگشت;

    fd = mch_fopen((نویسه *)profile_fname, "w");
    اگر (fd == پوچ)
	semsg(_(e_cant_open_file_str), profile_fname);
    وگرنه
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
    تهی
script_سطر_start(تهی)
{
    scriptitem_T    *si;
    sn_prl_T	    *pp;

    اگر (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	بازگشت;
    si = SCRIPT_ITEM(current_sctx.sc_sid);
    اگر (si->sn_prof_on && SOURCING_LNUM >= 1)
    {
	// Grow the array before starting the timer, so that the time spent
	// here isn't counted.
	(تهی)ga_grow(&si->sn_prl_ga,
				  (صحیح)(SOURCING_LNUM - si->sn_prl_ga.ga_len));
	si->sn_prl_idx = SOURCING_LNUM - 1;
	وقتی (si->sn_prl_ga.ga_len <= si->sn_prl_idx
		&& si->sn_prl_ga.ga_len < si->sn_prl_ga.ga_maxlen)
	{
	    // Zero counters for a line that was not used before.
	    pp = &PRL_ITEM(si, si->sn_prl_ga.ga_len);
	    pp->snp_شمار = 0;
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
    تهی
script_سطر_exec(تهی)
{
    scriptitem_T    *si;

    اگر (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	بازگشت;
    si = SCRIPT_ITEM(current_sctx.sc_sid);
    اگر (si->sn_prof_on && si->sn_prl_idx >= 0)
	si->sn_prl_execed = TRUE;
}

/*
 * Called when done with a script line.
 */
    تهی
script_سطر_end(تهی)
{
    scriptitem_T    *si;
    sn_prl_T	    *pp;

    اگر (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	بازگشت;
    si = SCRIPT_ITEM(current_sctx.sc_sid);
    اگر (si->sn_prof_on && si->sn_prl_idx >= 0
				     && si->sn_prl_idx < si->sn_prl_ga.ga_len)
    {
	اگر (si->sn_prl_execed)
	{
	    pp = &PRL_ITEM(si, si->sn_prl_idx);
	    ++pp->snp_شمار;
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
