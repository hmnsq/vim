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

#অন্তর্ভাব "vim.h"

#যদি defined(FEAT_EVAL)
# যদি defined(FEAT_PROFILE) || defined(FEAT_RELTIME)
/*
 * Store the current time in "tm".
 */
    শূন্য
profile_start(proftime_T *tm)
{
#  ifdef MSWIN
    Query_Performance_গণক(tm);
#  অন্যথা
    PROF_GET_TIME(tm);
#  endif
}

/*
 * Compute the elapsed time from "tm" till now and store in "tm".
 */
    শূন্য
profile_end(proftime_T *tm)
{
    proftime_T now;

#  ifdef MSWIN
    Query_Performance_গণক(&now);
    tm->QuadPart = now.QuadPart - tm->QuadPart;
#  অন্যথা
    PROF_GET_TIME(&now);
    tm->tv_fsec = now.tv_fsec - tm->tv_fsec;
    tm->tv_sec = now.tv_sec - tm->tv_sec;
    যদি (tm->tv_fsec < 0)
    {
	tm->tv_fsec += TV_FSEC_SEC;
	--tm->tv_sec;
    }
#  endif
}

/*
 * Subtract the time "tm2" from "tm".
 */
    শূন্য
profile_sub(proftime_T *tm, proftime_T *tm2)
{
#  ifdef MSWIN
    tm->QuadPart -= tm2->QuadPart;
#  অন্যথা
    tm->tv_fsec -= tm2->tv_fsec;
    tm->tv_sec -= tm2->tv_sec;
    যদি (tm->tv_fsec < 0)
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
    অক্ষর *
profile_বার্তা(proftime_T *tm)
{
    স্থির অক্ষর buf[50];

#  ifdef MSWIN
    LARGE_INTEGER   fr;

    QueryPerformanceFrequency(&fr);
    sprintf(buf, "%10.6lf", (দ্বিগুণ)tm->QuadPart / (দ্বিগুণ)fr.QuadPart);
#  অন্যথা
    sprintf(buf, PROF_TIME_FORMAT, (long)tm->tv_sec, (long)tm->tv_fsec);
#  endif
    ফেরত buf;
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
    ফেরত (float_T)tm->QuadPart / (float_T)fr.QuadPart;
#  অন্যথা
    ফেরত (float_T)tm->tv_sec + (float_T)tm->tv_fsec / (float_T)TV_FSEC_SEC;
#  endif
}

/*
 * Put the time "msec" past now in "tm".
 */
    শূন্য
profile_setlimit(long msec, proftime_T *tm)
{
    যদি (msec <= 0)   // no limit
	profile_zero(tm);
    অন্যথা
    {
#  ifdef MSWIN
	LARGE_INTEGER   fr;

	Query_Performance_গণক(tm);
	QueryPerformanceFrequency(&fr);
	tm->QuadPart += (LONGLONG)((দ্বিগুণ)msec / 1000.0 * (দ্বিগুণ)fr.QuadPart);
#  অন্যথা
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
    পূর্ণসংখ্যা
profile_passed_limit(proftime_T *tm)
{
    proftime_T	now;

#  ifdef MSWIN
    যদি (tm->QuadPart == 0)  // timer was not set
	ফেরত FALSE;
    Query_Performance_গণক(&now);
    ফেরত (now.QuadPart > tm->QuadPart);
#  অন্যথা
    যদি (tm->tv_sec == 0)    // timer was not set
	ফেরত FALSE;
    PROF_GET_TIME(&now);
    ফেরত (now.tv_sec > tm->tv_sec
	    || (now.tv_sec == tm->tv_sec && now.tv_fsec > tm->tv_fsec));
#  endif
}

/*
 * Set the time in "tm" to zero.
 */
    শূন্য
profile_zero(proftime_T *tm)
{
#  ifdef MSWIN
    tm->QuadPart = 0;
#  অন্যথা
    tm->tv_fsec = 0;
    tm->tv_sec = 0;
#  endif
}

# endif  // FEAT_PROFILE || FEAT_RELTIME

# যদি defined(FEAT_SYN_HL) && defined(FEAT_RELTIME) && defined(FEAT_PROFILE)
#  যদি defined(HAVE_MATH_H)
#   include <math.h>
#  endif

/*
 * Divide the time "tm" by "count" and store in "tm2".
 */
    শূন্য
profile_divide(proftime_T *tm, পূর্ণসংখ্যা গুনতি, proftime_T *tm2)
{
    যদি (গুনতি == 0)
	profile_zero(tm2);
    অন্যথা
    {
#  ifdef MSWIN
	tm2->QuadPart = tm->QuadPart / গুনতি;
#  অন্যথা
	দ্বিগুণ fsec = (tm->tv_sec * (float_T)TV_FSEC_SEC + tm->tv_fsec)
								       / গুনতি;

	tm2->tv_sec = floor(fsec / (float_T)TV_FSEC_SEC);
	tm2->tv_fsec = vim_round(fsec - (tm2->tv_sec * (float_T)TV_FSEC_SEC));
#  endif
    }
}
# endif

# যদি defined(FEAT_PROFILE)
/*
 * Functions for profiling.
 */
স্থির proftime_T prof_wait_time;

/*
 * Add the time "tm2" to "tm".
 */
    শূন্য
profile_add(proftime_T *tm, proftime_T *tm2)
{
#  ifdef MSWIN
    tm->QuadPart += tm2->QuadPart;
#  অন্যথা
    tm->tv_fsec += tm2->tv_fsec;
    tm->tv_sec += tm2->tv_sec;
    যদি (tm->tv_fsec >= TV_FSEC_SEC)
    {
	tm->tv_fsec -= TV_FSEC_SEC;
	++tm->tv_sec;
    }
#  endif
}

/*
 * Add the "self" time from the total time and the children's time.
 */
    শূন্য
profile_self(proftime_T *self, proftime_T *যোগ, proftime_T *children)
{
    // Check that the result won't be negative.  Can happen with recursive
    // calls.
#  ifdef MSWIN
    যদি (যোগ->QuadPart <= children->QuadPart)
	ফেরত;
#  অন্যথা
    যদি (যোগ->tv_sec < children->tv_sec
	    || (যোগ->tv_sec == children->tv_sec
		&& যোগ->tv_fsec <= children->tv_fsec))
	ফেরত;
#  endif
    profile_add(self, যোগ);
    profile_sub(self, children);
}

/*
 * Get the current waittime.
 */
    স্থির শূন্য
profile_get_wait(proftime_T *tm)
{
    *tm = prof_wait_time;
}

/*
 * Subtract the passed waittime since "tm" from "tma".
 */
    শূন্য
profile_sub_wait(proftime_T *tm, proftime_T *tma)
{
    proftime_T tm3 = prof_wait_time;

    profile_sub(&tm3, tm);
    profile_sub(tma, &tm3);
}

/*
 * Return TRUE if "tm1" and "tm2" are equal.
 */
    স্থির পূর্ণসংখ্যা
profile_equal(proftime_T *tm1, proftime_T *tm2)
{
#  ifdef MSWIN
    ফেরত (tm1->QuadPart == tm2->QuadPart);
#  অন্যথা
    ফেরত (tm1->tv_fsec == tm2->tv_fsec && tm1->tv_sec == tm2->tv_sec);
#  endif
}

/*
 * Return <0, 0 or >0 if "tm1" < "tm2", "tm1" == "tm2" or "tm1" > "tm2"
 */
    পূর্ণসংখ্যা
profile_cmp(ধ্রুবক proftime_T *tm1, ধ্রুবক proftime_T *tm2)
{
#  ifdef MSWIN
    ফেরত tm2->QuadPart == tm1->QuadPart ? 0 :
	tm2->QuadPart > tm1->QuadPart ? 1 : -1;
#  অন্যথা
    যদি (tm1->tv_sec == tm2->tv_sec)
	ফেরত tm2->tv_fsec == tm1->tv_fsec ? 0 :
	    tm2->tv_fsec > tm1->tv_fsec ? 1 : -1;
    ফেরত tm2->tv_sec > tm1->tv_sec ? 1 : -1;
#  endif
}

স্থির char_u	*profile_fname = ফাঁকা;
স্থির proftime_T pause_time;

/*
 * Reset all profiling information.
 */
    স্থির শূন্য
profile_reset(শূন্য)
{
    পূর্ণসংখ্যা		id;
    পূর্ণসংখ্যা		todo;
    hashtab_T	*functbl;
    hashitem_T	*hi;

    // Reset sourced files.
    জন্য (id = 1; id <= script_বস্তুসমূহ.ga_len; id++)
    {
	scriptitem_T *si = SCRIPT_ITEM(id);
	যদি (si->sn_prof_on)
	{
	    si->sn_prof_on      = FALSE;
	    si->sn_pr_force     = FALSE;
	    profile_zero(&si->sn_pr_child);
	    si->sn_pr_nest      = 0;
	    si->sn_pr_গুনতি     = 0;
	    profile_zero(&si->sn_pr_যোগ);
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
    todo = (পূর্ণসংখ্যা)functbl->ht_used;

    FOR_ALL_HASHTAB_বস্তুসমূহ(functbl, hi, todo)
    {
	ufunc_T *fp;
	পূর্ণসংখ্যা	i;

	যদি (HASHITEM_EMPTY(hi))
	    চালু;

	todo--;
	fp = HI2UF(hi);
	যদি (fp->uf_prof_initialized)
	{
	    fp->uf_profiling    = 0;
	    fp->uf_prof_initialized = FALSE;
	    fp->uf_tm_গুনতি     = 0;
	    profile_zero(&fp->uf_tm_যোগ);
	    profile_zero(&fp->uf_tm_self);
	    profile_zero(&fp->uf_tm_children);

	    জন্য (i = 0; i < fp->uf_সারিগুলি.ga_len; i++)
	    {
		fp->uf_tml_গুনতি[i] = 0;
		profile_zero(&fp->uf_tml_যোগ[i]);
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
    শূন্য
ex_profile(exarg_T *eap)
{
    char_u	*e;
    পূর্ণসংখ্যা		len;

    e = skiptowhite(eap->arg);
    len = (পূর্ণসংখ্যা)(e - eap->arg);
    e = skipwhite(e);

    যদি (len == 5 && STRNCMP(eap->arg, "start", 5) == 0 && *e != NUL)
    {
	VIM_CLEAR(profile_fname);
	profile_fname = expand_env_save_opt(e, TRUE, ফাঁকা);
	do_profiling = PROF_YES;
	profile_zero(&prof_wait_time);
	set_vim_var_nr(VV_PROFILING, 1L);
    }
    অন্যথা যদি (do_profiling == PROF_NONE)
	emsg(_(e_first_use_profile_start_fname));
    অন্যথা যদি (STRCMP(eap->arg, "stop") == 0)
    {
	profile_dump();
	do_profiling = PROF_NONE;
	set_vim_var_nr(VV_PROFILING, 0L);
	profile_reset();
    }
    অন্যথা যদি (STRCMP(eap->arg, "pause") == 0)
    {
	যদি (do_profiling == PROF_YES)
	    profile_start(&pause_time);
	do_profiling = PROF_PAUSED;
    }
    অন্যথা যদি (STRCMP(eap->arg, "continue") == 0)
    {
	যদি (do_profiling == PROF_PAUSED)
	{
	    profile_end(&pause_time);
	    profile_add(&prof_wait_time, &pause_time);
	}
	do_profiling = PROF_YES;
    }
    অন্যথা যদি (STRCMP(eap->arg, "dump") == 0)
	profile_dump();
    অন্যথা
    {
	// The rest is similar to ":breakadd".
	ex_breakadd(eap);
    }
}

// Command line expansion for :profile.
স্থির গণনা
{
    PEXP_SUBCMD,	// expand :profile sub-commands
    PEXP_FUNC		// expand :profile func {funcname}
} pexpand_what;

স্থির অক্ষর *pexpand_cmds[] = {
			"start",
			"stop",
			"pause",
			"continue",
			"func",
			"file",
			"dump",
			ফাঁকা
};

/*
 * Function given to ExpandGeneric() to obtain the profile command
 * specific expansion.
 */
    char_u *
get_profile_name(expand_T *xp UNUSED, পূর্ণসংখ্যা idx)
{
    নির্বাচন (pexpand_what)
    {
    ক্ষেত্র PEXP_SUBCMD:
	ফেরত (char_u *)pexpand_cmds[idx];
    পূর্বনির্ধারিত:
	ফেরত ফাঁকা;
    }
}

/*
 * Handle command line completion for :profile command.
 */
    শূন্য
set_context_in_profile_cmd(expand_T *xp, char_u *arg)
{
    char_u	*end_subcmd;

    // Default: expand subcommands.
    xp->xp_context = EXPAND_PROFILE;
    pexpand_what = PEXP_SUBCMD;
    xp->xp_pattern = arg;

    end_subcmd = skiptowhite(arg);
    যদি (*end_subcmd == NUL)
	ফেরত;

    যদি ((end_subcmd - arg == 5 && STRNCMP(arg, "start", 5) == 0)
	    || (end_subcmd - arg == 4 && STRNCMP(arg, "file", 4) == 0))
    {
	xp->xp_context = EXPAND_FILES;
	xp->xp_pattern = skipwhite(end_subcmd);
	ফেরত;
    }
    অন্যথা যদি (end_subcmd - arg == 4 && STRNCMP(arg, "func", 4) == 0)
    {
	xp->xp_context = EXPAND_USER_FUNC;
	xp->xp_pattern = skipwhite(end_subcmd);
	ফেরত;
    }

    xp->xp_context = EXPAND_NOTHING;
}

স্থির proftime_T inchar_time;

/*
 * Called when starting to wait for the user to type a character.
 */
    শূন্য
prof_inchar_enter(শূন্য)
{
    profile_start(&inchar_time);
}

/*
 * Called when finished waiting for the user to type a character.
 */
    শূন্য
prof_inchar_exit(শূন্য)
{
    profile_end(&inchar_time);
    profile_add(&prof_wait_time, &inchar_time);
}


/*
 * Return TRUE when a function defined in the current script should be
 * profiled.
 */
    পূর্ণসংখ্যা
prof_def_func(শূন্য)
{
    যদি (current_sctx.sc_sid > 0)
	ফেরত SCRIPT_ITEM(current_sctx.sc_sid)->sn_pr_force;
    ফেরত FALSE;
}

/*
 * Print the count and times for one function or function line.
 */
    স্থির শূন্য
prof_func_সারি(
    FILE	*fd,
    পূর্ণসংখ্যা		গুনতি,
    proftime_T	*যোগ,
    proftime_T	*self,
    পূর্ণসংখ্যা		prefer_self)	// when equal print only self time
{
    যদি (গুনতি > 0)
    {
	fprintf(fd, "%5d ", গুনতি);
	যদি (prefer_self && profile_equal(যোগ, self))
	    fprintf(fd, PROF_TIME_BLANK);
	অন্যথা
	    fprintf(fd, "%s ", profile_বার্তা(যোগ));
	যদি (!prefer_self && profile_equal(যোগ, self))
	    fprintf(fd, PROF_TIME_BLANK);
	অন্যথা
	    fprintf(fd, "%s ", profile_বার্তা(self));
    }
    অন্যথা
	fprintf(fd, "      %s%s", PROF_TIME_BLANK, PROF_TIME_BLANK);
}

    স্থির শূন্য
prof_sort_list(
    FILE	*fd,
    ufunc_T	**sorttab,
    পূর্ণসংখ্যা		st_len,
    অক্ষর	*title,
    পূর্ণসংখ্যা		prefer_self)	// when equal print only self time
{
    পূর্ণসংখ্যা		i;
    ufunc_T	*fp;

    fprintf(fd, "FUNCTIONS SORTED ON %s TIME\n", title);
    fprintf(fd, "%s  function\n", PROF_TOTALS_HEADER);
    জন্য (i = 0; i < 20 && i < st_len; ++i)
    {
	fp = sorttab[i];
	prof_func_সারি(fd, fp->uf_tm_গুনতি, &fp->uf_tm_যোগ, &fp->uf_tm_self,
								 prefer_self);
	যদি (fp->uf_name[0] == K_SPECIAL)
	    fprintf(fd, " <SNR>%s()\n", fp->uf_name + 3);
	অন্যথা
	    fprintf(fd, " %s()\n", fp->uf_name);
    }
    fprintf(fd, "\n");
}

/*
 * Compare function for total time sorting.
 */
    স্থির পূর্ণসংখ্যা
prof_যোগ_cmp(ধ্রুবক শূন্য *s1, ধ্রুবক শূন্য *s2)
{
    ufunc_T	*p1, *p2;

    p1 = *(ufunc_T **)s1;
    p2 = *(ufunc_T **)s2;
    ফেরত profile_cmp(&p1->uf_tm_যোগ, &p2->uf_tm_যোগ);
}

/*
 * Compare function for self time sorting.
 */
    স্থির পূর্ণসংখ্যা
prof_self_cmp(ধ্রুবক শূন্য *s1, ধ্রুবক শূন্য *s2)
{
    ufunc_T	*p1, *p2;

    p1 = *(ufunc_T **)s1;
    p2 = *(ufunc_T **)s2;
    ফেরত profile_cmp(&p1->uf_tm_self, &p2->uf_tm_self);
}

/*
 * Start profiling function "fp".
 */
    শূন্য
func_do_profile(ufunc_T *fp)
{
    পূর্ণসংখ্যা		len = fp->uf_সারিগুলি.ga_len;

    যদি (!fp->uf_prof_initialized)
    {
	যদি (len == 0)
	    len = 1;  // avoid getting error for allocating zero bytes
	fp->uf_tm_গুনতি = 0;
	profile_zero(&fp->uf_tm_self);
	profile_zero(&fp->uf_tm_যোগ);
	যদি (fp->uf_tml_গুনতি == ফাঁকা)
	    fp->uf_tml_গুনতি = ALLOC_CLEAR_MULT(পূর্ণসংখ্যা, len);
	যদি (fp->uf_tml_যোগ == ফাঁকা)
	    fp->uf_tml_যোগ = ALLOC_CLEAR_MULT(proftime_T, len);
	যদি (fp->uf_tml_self == ফাঁকা)
	    fp->uf_tml_self = ALLOC_CLEAR_MULT(proftime_T, len);
	fp->uf_tml_idx = -1;
	যদি (fp->uf_tml_গুনতি == ফাঁকা || fp->uf_tml_যোগ == ফাঁকা
						    || fp->uf_tml_self == ফাঁকা)
	    ফেরত;	    // out of memory
	fp->uf_prof_initialized = TRUE;
    }

    fp->uf_profiling = TRUE;
}

/*
 * Save time when starting to invoke another script or function.
 */
    স্থির শূন্য
script_prof_save(
    proftime_T	*tm)	    // place to store wait time
{
    scriptitem_T    *si;

    যদি (SCRIPT_ID_VALID(current_sctx.sc_sid))
    {
	si = SCRIPT_ITEM(current_sctx.sc_sid);
	যদি (si->sn_prof_on && si->sn_pr_nest++ == 0)
	    profile_start(&si->sn_pr_child);
    }
    profile_get_wait(tm);
}

/*
 * When calling a function: may initialize for profiling.
 */
    শূন্য
profile_may_start_func(profinfo_T *info, ufunc_T *fp, ufunc_T *caller)
{
    যদি (!fp->uf_profiling && has_profiling(FALSE, fp->uf_name, ফাঁকা,
								&fp->uf_hash))
    {
	info->pi_started_profiling = TRUE;
	func_do_profile(fp);
    }
    যদি (fp->uf_profiling || (caller != ফাঁকা && caller->uf_profiling))
    {
	++fp->uf_tm_গুনতি;
	profile_start(&info->pi_call_start);
	profile_zero(&fp->uf_tm_children);
    }
    script_prof_save(&info->pi_wait_start);
}

/*
 * After calling a function: may handle profiling.  profile_may_start_func()
 * must have been called previously.
 */
    শূন্য
profile_may_end_func(profinfo_T *info, ufunc_T *fp, ufunc_T *caller)
{
    profile_end(&info->pi_call_start);
    profile_sub_wait(&info->pi_wait_start, &info->pi_call_start);
    profile_add(&fp->uf_tm_যোগ, &info->pi_call_start);
    profile_self(&fp->uf_tm_self, &info->pi_call_start, &fp->uf_tm_children);
    যদি (caller != ফাঁকা && caller->uf_profiling)
    {
	profile_add(&caller->uf_tm_children, &info->pi_call_start);
	profile_add(&caller->uf_tml_children, &info->pi_call_start);
    }
    যদি (info->pi_started_profiling)
	// make a ":profdel func" stop profiling the function
	fp->uf_profiling = FALSE;
}

/*
 * Prepare profiling for entering a child or something else that is not
 * counted for the script/function itself.
 * Should always be called in pair with prof_child_exit().
 */
    শূন্য
prof_child_enter(
    proftime_T *tm)	// place to store waittime
{
    funccall_T *fc = get_current_funccal();

    যদি (fc != ফাঁকা && fc->fc_func->uf_profiling)
	profile_start(&fc->fc_prof_child);
    script_prof_save(tm);
}

/*
 * Take care of time spent in a child.
 * Should always be called after prof_child_enter().
 */
    শূন্য
prof_child_exit(
    proftime_T *tm)	// where waittime was stored
{
    funccall_T *fc = get_current_funccal();

    যদি (fc != ফাঁকা && fc->fc_func->uf_profiling)
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
    শূন্য
func_সারি_start(শূন্য *cookie, long lnum)
{
    funccall_T	*fcp = (funccall_T *)cookie;
    ufunc_T	*fp = fcp->fc_func;

    যদি (fp->uf_profiling && lnum >= 1 && lnum <= fp->uf_সারিগুলি.ga_len)
    {
	fp->uf_tml_idx = lnum - 1;
	// Skip continuation lines.
	যখন (fp->uf_tml_idx > 0 && FUNCLINE(fp, fp->uf_tml_idx) == ফাঁকা)
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
    শূন্য
func_সারি_exec(শূন্য *cookie)
{
    funccall_T	*fcp = (funccall_T *)cookie;
    ufunc_T	*fp = fcp->fc_func;

    যদি (fp->uf_profiling && fp->uf_tml_idx >= 0)
	fp->uf_tml_execed = TRUE;
}

/*
 * Called when done with a function line.
 */
    শূন্য
func_সারি_end(শূন্য *cookie)
{
    funccall_T	*fcp = (funccall_T *)cookie;
    ufunc_T	*fp = fcp->fc_func;

    যদি (fp->uf_profiling && fp->uf_tml_idx >= 0)
    {
	যদি (fp->uf_tml_execed)
	{
	    ++fp->uf_tml_গুনতি[fp->uf_tml_idx];
	    profile_end(&fp->uf_tml_start);
	    profile_sub_wait(&fp->uf_tml_wait, &fp->uf_tml_start);
	    profile_add(&fp->uf_tml_যোগ[fp->uf_tml_idx], &fp->uf_tml_start);
	    profile_self(&fp->uf_tml_self[fp->uf_tml_idx], &fp->uf_tml_start,
							&fp->uf_tml_children);
	}
	fp->uf_tml_idx = -1;
    }
}

/*
 * Dump the profiling results for all functions in file "fd".
 */
    স্থির শূন্য
func_dump_profile(FILE *fd)
{
    hashtab_T	*functbl;
    hashitem_T	*hi;
    পূর্ণসংখ্যা		todo;
    ufunc_T	*fp;
    পূর্ণসংখ্যা		i;
    ufunc_T	**sorttab;
    পূর্ণসংখ্যা		st_len = 0;
    char_u	*p;

    functbl = func_tbl_get();
    todo = (পূর্ণসংখ্যা)functbl->ht_used;
    যদি (todo == 0)
	ফেরত;     // nothing to dump

    sorttab = ALLOC_MULT(ufunc_T *, todo);

    FOR_ALL_HASHTAB_বস্তুসমূহ(functbl, hi, todo)
    {
	যদি (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    fp = HI2UF(hi);
	    যদি (fp->uf_prof_initialized)
	    {
		যদি (sorttab != ফাঁকা)
		    sorttab[st_len++] = fp;

		যদি (fp->uf_name[0] == K_SPECIAL)
		    fprintf(fd, "FUNCTION  <SNR>%s()\n", fp->uf_name + 3);
		অন্যথা
		    fprintf(fd, "FUNCTION  %s()\n", fp->uf_name);
		যদি (fp->uf_script_ctx.sc_sid > 0)
		{
		    p = home_replace_save(ফাঁকা,
				     get_scriptname(fp->uf_script_ctx.sc_sid));
		    যদি (p != ফাঁকা)
		    {
			fprintf(fd, "    Defined: %s:%ld\n",
					   p, (long)fp->uf_script_ctx.sc_lnum);
			vim_free(p);
		    }
		}
		যদি (fp->uf_tm_গুনতি == 1)
		    fprintf(fd, "Called 1 time\n");
		অন্যথা
		    fprintf(fd, "Called %d times\n", fp->uf_tm_গুনতি);
		fprintf(fd, "Total time: %s\n", profile_বার্তা(&fp->uf_tm_যোগ));
		fprintf(fd, " Self time: %s\n", profile_বার্তা(&fp->uf_tm_self));
		fprintf(fd, "\n");
		fprintf(fd, "%s\n", PROF_TOTALS_HEADER);

		জন্য (i = 0; i < fp->uf_সারিগুলি.ga_len; ++i)
		{
		    যদি (FUNCLINE(fp, i) == ফাঁকা)
			চালু;
		    prof_func_সারি(fd, fp->uf_tml_গুনতি[i],
			     &fp->uf_tml_যোগ[i], &fp->uf_tml_self[i], TRUE);
		    fprintf(fd, "%s\n", FUNCLINE(fp, i));
		}
		fprintf(fd, "\n");
	    }
	}
    }

    যদি (sorttab != ফাঁকা && st_len > 0)
    {
	qsort((শূন্য *)sorttab, (size_t)st_len, আকার(ufunc_T *),
							      prof_যোগ_cmp);
	prof_sort_list(fd, sorttab, st_len, "TOTAL", FALSE);
	qsort((শূন্য *)sorttab, (size_t)st_len, আকার(ufunc_T *),
							      prof_self_cmp);
	prof_sort_list(fd, sorttab, st_len, "SELF", TRUE);
    }

    vim_free(sorttab);
}

/*
 * Start profiling script "fp".
 */
    শূন্য
script_do_profile(scriptitem_T *si)
{
    si->sn_pr_গুনতি = 0;
    profile_zero(&si->sn_pr_যোগ);
    profile_zero(&si->sn_pr_self);

    ga_init2(&si->sn_prl_ga, আকার(sn_prl_T), 100);
    si->sn_prl_idx = -1;
    si->sn_prof_on = TRUE;
    si->sn_pr_nest = 0;
}

/*
 * Count time spent in children after invoking another script or function.
 */
    শূন্য
script_prof_restore(proftime_T *tm)
{
    scriptitem_T    *si;

    যদি (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	ফেরত;

    si = SCRIPT_ITEM(current_sctx.sc_sid);
    যদি (si->sn_prof_on && --si->sn_pr_nest == 0)
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
    স্থির শূন্য
script_dump_profile(FILE *fd)
{
    পূর্ণসংখ্যা		    id;
    scriptitem_T    *si;
    পূর্ণসংখ্যা		    i;
    FILE	    *sfd;
    sn_prl_T	    *pp;

    জন্য (id = 1; id <= script_বস্তুসমূহ.ga_len; ++id)
    {
	si = SCRIPT_ITEM(id);
	যদি (si->sn_prof_on)
	{
	    fprintf(fd, "SCRIPT  %s\n", si->sn_name);
	    যদি (si->sn_pr_গুনতি == 1)
		fprintf(fd, "Sourced 1 time\n");
	    অন্যথা
		fprintf(fd, "Sourced %d times\n", si->sn_pr_গুনতি);
	    fprintf(fd, "Total time: %s\n", profile_বার্তা(&si->sn_pr_যোগ));
	    fprintf(fd, " Self time: %s\n", profile_বার্তা(&si->sn_pr_self));
	    fprintf(fd, "\n");
	    fprintf(fd, "%s\n", PROF_TOTALS_HEADER);

	    sfd = mch_fopen((অক্ষর *)si->sn_name, "r");
	    যদি (sfd == ফাঁকা)
		fprintf(fd, "Cannot open file!\n");
	    অন্যথা
	    {
		// Keep going till the end of file, so that trailing
		// continuation lines are listed.
		জন্য (i = 0; ; ++i)
		{
		    যদি (vim_fgets(IObuff, IOSIZE, sfd))
			বিরতি;
		    // When a line has been truncated, append NL, taking care
		    // of multi-byte characters .
		    যদি (IObuff[IOSIZE - 2] != NUL && IObuff[IOSIZE - 2] != NL)
		    {
			পূর্ণসংখ্যা n = IOSIZE - 2;

			যদি (enc_utf8)
			{
			    // Move to the first byte of this char.
			    // utf_head_off() doesn't work, because it checks
			    // for a truncated character.
			    যখন (n > 0 && (IObuff[n] & 0xc0) == 0x80)
				--n;
			}
			অন্যথা যদি (has_mbyte)
			    n -= mb_শীর্ষ_off(IObuff, IObuff + n);
			IObuff[n] = NL;
			IObuff[n + 1] = NUL;
		    }
		    যদি (i < si->sn_prl_ga.ga_len
				     && (pp = &PRL_ITEM(si, i))->snp_গুনতি > 0)
		    {
			fprintf(fd, "%5d ", pp->snp_গুনতি);
			যদি (profile_equal(&pp->sn_prl_যোগ, &pp->sn_prl_self))
			    fprintf(fd, "           ");
			অন্যথা
			    fprintf(fd, "%s ", profile_বার্তা(&pp->sn_prl_যোগ));
			fprintf(fd, "%s ", profile_বার্তা(&pp->sn_prl_self));
		    }
		    অন্যথা
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
    শূন্য
profile_dump(শূন্য)
{
    FILE	*fd;

    যদি (profile_fname == ফাঁকা)
	ফেরত;

    fd = mch_fopen((অক্ষর *)profile_fname, "w");
    যদি (fd == ফাঁকা)
	semsg(_(e_cant_open_file_str), profile_fname);
    অন্যথা
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
    শূন্য
script_সারি_start(শূন্য)
{
    scriptitem_T    *si;
    sn_prl_T	    *pp;

    যদি (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	ফেরত;
    si = SCRIPT_ITEM(current_sctx.sc_sid);
    যদি (si->sn_prof_on && SOURCING_LNUM >= 1)
    {
	// Grow the array before starting the timer, so that the time spent
	// here isn't counted.
	(শূন্য)ga_grow(&si->sn_prl_ga,
				  (পূর্ণসংখ্যা)(SOURCING_LNUM - si->sn_prl_ga.ga_len));
	si->sn_prl_idx = SOURCING_LNUM - 1;
	যখন (si->sn_prl_ga.ga_len <= si->sn_prl_idx
		&& si->sn_prl_ga.ga_len < si->sn_prl_ga.ga_maxlen)
	{
	    // Zero counters for a line that was not used before.
	    pp = &PRL_ITEM(si, si->sn_prl_ga.ga_len);
	    pp->snp_গুনতি = 0;
	    profile_zero(&pp->sn_prl_যোগ);
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
    শূন্য
script_সারি_exec(শূন্য)
{
    scriptitem_T    *si;

    যদি (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	ফেরত;
    si = SCRIPT_ITEM(current_sctx.sc_sid);
    যদি (si->sn_prof_on && si->sn_prl_idx >= 0)
	si->sn_prl_execed = TRUE;
}

/*
 * Called when done with a script line.
 */
    শূন্য
script_সারি_end(শূন্য)
{
    scriptitem_T    *si;
    sn_prl_T	    *pp;

    যদি (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	ফেরত;
    si = SCRIPT_ITEM(current_sctx.sc_sid);
    যদি (si->sn_prof_on && si->sn_prl_idx >= 0
				     && si->sn_prl_idx < si->sn_prl_ga.ga_len)
    {
	যদি (si->sn_prl_execed)
	{
	    pp = &PRL_ITEM(si, si->sn_prl_idx);
	    ++pp->snp_গুনতি;
	    profile_end(&si->sn_prl_start);
	    profile_sub_wait(&si->sn_prl_wait, &si->sn_prl_start);
	    profile_add(&pp->sn_prl_যোগ, &si->sn_prl_start);
	    profile_self(&pp->sn_prl_self, &si->sn_prl_start,
							&si->sn_prl_children);
	}
	si->sn_prl_idx = -1;
    }
}
# endif // FEAT_PROFILE

#endif
