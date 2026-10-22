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

#அந்தர்பாவ "vim.h"

#எனில் defined(FEAT_EVAL)
# எனில் defined(FEAT_PROFILE) || defined(FEAT_RELTIME)
/*
 * Store the current time in "tm".
 */
    வெறுமை
profile_start(proftime_T *tm)
{
#  ifdef MSWIN
    QueryPerformanceCounter(tm);
#  இல்லையெனில்
    PROF_GET_TIME(tm);
#  endif
}

/*
 * Compute the elapsed time from "tm" till now and store in "tm".
 */
    வெறுமை
profile_end(proftime_T *tm)
{
    proftime_T now;

#  ifdef MSWIN
    QueryPerformanceCounter(&now);
    tm->QuadPart = now.QuadPart - tm->QuadPart;
#  இல்லையெனில்
    PROF_GET_TIME(&now);
    tm->tv_fsec = now.tv_fsec - tm->tv_fsec;
    tm->tv_sec = now.tv_sec - tm->tv_sec;
    எனில் (tm->tv_fsec < 0)
    {
	tm->tv_fsec += TV_FSEC_SEC;
	--tm->tv_sec;
    }
#  endif
}

/*
 * Subtract the time "tm2" from "tm".
 */
    வெறுமை
profile_sub(proftime_T *tm, proftime_T *tm2)
{
#  ifdef MSWIN
    tm->QuadPart -= tm2->QuadPart;
#  இல்லையெனில்
    tm->tv_fsec -= tm2->tv_fsec;
    tm->tv_sec -= tm2->tv_sec;
    எனில் (tm->tv_fsec < 0)
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
    எழுத்து *
profile_செய்தி(proftime_T *tm)
{
    நிலையான எழுத்து buf[50];

#  ifdef MSWIN
    LARGE_INTEGER   fr;

    QueryPerformanceFrequency(&fr);
    sprintf(buf, "%10.6lf", (இரட்டை)tm->QuadPart / (இரட்டை)fr.QuadPart);
#  இல்லையெனில்
    sprintf(buf, PROF_TIME_FORMAT, (long)tm->tv_sec, (long)tm->tv_fsec);
#  endif
    திரும்பு buf;
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
    திரும்பு (float_T)tm->QuadPart / (float_T)fr.QuadPart;
#  இல்லையெனில்
    திரும்பு (float_T)tm->tv_sec + (float_T)tm->tv_fsec / (float_T)TV_FSEC_SEC;
#  endif
}

/*
 * Put the time "msec" past now in "tm".
 */
    வெறுமை
profile_setlimit(long msec, proftime_T *tm)
{
    எனில் (msec <= 0)   // no limit
	profile_zero(tm);
    இல்லையெனில்
    {
#  ifdef MSWIN
	LARGE_INTEGER   fr;

	QueryPerformanceCounter(tm);
	QueryPerformanceFrequency(&fr);
	tm->QuadPart += (LONGLONG)((இரட்டை)msec / 1000.0 * (இரட்டை)fr.QuadPart);
#  இல்லையெனில்
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
    முழு
profile_passed_limit(proftime_T *tm)
{
    proftime_T	now;

#  ifdef MSWIN
    எனில் (tm->QuadPart == 0)  // timer was not set
	திரும்பு FALSE;
    QueryPerformanceCounter(&now);
    திரும்பு (now.QuadPart > tm->QuadPart);
#  இல்லையெனில்
    எனில் (tm->tv_sec == 0)    // timer was not set
	திரும்பு FALSE;
    PROF_GET_TIME(&now);
    திரும்பு (now.tv_sec > tm->tv_sec
	    || (now.tv_sec == tm->tv_sec && now.tv_fsec > tm->tv_fsec));
#  endif
}

/*
 * Set the time in "tm" to zero.
 */
    வெறுமை
profile_zero(proftime_T *tm)
{
#  ifdef MSWIN
    tm->QuadPart = 0;
#  இல்லையெனில்
    tm->tv_fsec = 0;
    tm->tv_sec = 0;
#  endif
}

# endif  // FEAT_PROFILE || FEAT_RELTIME

# எனில் defined(FEAT_SYN_HL) && defined(FEAT_RELTIME) && defined(FEAT_PROFILE)
#  எனில் defined(HAVE_MATH_H)
#   include <math.h>
#  endif

/*
 * Divide the time "tm" by "count" and store in "tm2".
 */
    வெறுமை
profile_divide(proftime_T *tm, முழு மொத்தம், proftime_T *tm2)
{
    எனில் (மொத்தம் == 0)
	profile_zero(tm2);
    இல்லையெனில்
    {
#  ifdef MSWIN
	tm2->QuadPart = tm->QuadPart / மொத்தம்;
#  இல்லையெனில்
	இரட்டை fsec = (tm->tv_sec * (float_T)TV_FSEC_SEC + tm->tv_fsec)
								       / மொத்தம்;

	tm2->tv_sec = floor(fsec / (float_T)TV_FSEC_SEC);
	tm2->tv_fsec = vim_round(fsec - (tm2->tv_sec * (float_T)TV_FSEC_SEC));
#  endif
    }
}
# endif

# எனில் defined(FEAT_PROFILE)
/*
 * Functions for profiling.
 */
நிலையான proftime_T prof_wait_time;

/*
 * Add the time "tm2" to "tm".
 */
    வெறுமை
profile_add(proftime_T *tm, proftime_T *tm2)
{
#  ifdef MSWIN
    tm->QuadPart += tm2->QuadPart;
#  இல்லையெனில்
    tm->tv_fsec += tm2->tv_fsec;
    tm->tv_sec += tm2->tv_sec;
    எனில் (tm->tv_fsec >= TV_FSEC_SEC)
    {
	tm->tv_fsec -= TV_FSEC_SEC;
	++tm->tv_sec;
    }
#  endif
}

/*
 * Add the "self" time from the total time and the children's time.
 */
    வெறுமை
profile_self(proftime_T *self, proftime_T *total, proftime_T *children)
{
    // Check that the result won't be negative.  Can happen with recursive
    // calls.
#  ifdef MSWIN
    எனில் (total->QuadPart <= children->QuadPart)
	திரும்பு;
#  இல்லையெனில்
    எனில் (total->tv_sec < children->tv_sec
	    || (total->tv_sec == children->tv_sec
		&& total->tv_fsec <= children->tv_fsec))
	திரும்பு;
#  endif
    profile_add(self, total);
    profile_sub(self, children);
}

/*
 * Get the current waittime.
 */
    நிலையான வெறுமை
profile_get_wait(proftime_T *tm)
{
    *tm = prof_wait_time;
}

/*
 * Subtract the passed waittime since "tm" from "tma".
 */
    வெறுமை
profile_sub_wait(proftime_T *tm, proftime_T *tma)
{
    proftime_T tm3 = prof_wait_time;

    profile_sub(&tm3, tm);
    profile_sub(tma, &tm3);
}

/*
 * Return TRUE if "tm1" and "tm2" are equal.
 */
    நிலையான முழு
profile_equal(proftime_T *tm1, proftime_T *tm2)
{
#  ifdef MSWIN
    திரும்பு (tm1->QuadPart == tm2->QuadPart);
#  இல்லையெனில்
    திரும்பு (tm1->tv_fsec == tm2->tv_fsec && tm1->tv_sec == tm2->tv_sec);
#  endif
}

/*
 * Return <0, 0 or >0 if "tm1" < "tm2", "tm1" == "tm2" or "tm1" > "tm2"
 */
    முழு
profile_cmp(மாறிலி proftime_T *tm1, மாறிலி proftime_T *tm2)
{
#  ifdef MSWIN
    திரும்பு tm2->QuadPart == tm1->QuadPart ? 0 :
	tm2->QuadPart > tm1->QuadPart ? 1 : -1;
#  இல்லையெனில்
    எனில் (tm1->tv_sec == tm2->tv_sec)
	திரும்பு tm2->tv_fsec == tm1->tv_fsec ? 0 :
	    tm2->tv_fsec > tm1->tv_fsec ? 1 : -1;
    திரும்பு tm2->tv_sec > tm1->tv_sec ? 1 : -1;
#  endif
}

நிலையான char_u	*profile_fname = வெற்று;
நிலையான proftime_T pause_time;

/*
 * Reset all profiling information.
 */
    நிலையான வெறுமை
profile_reset(வெறுமை)
{
    முழு		id;
    முழு		todo;
    hashtab_T	*functbl;
    hashitem_T	*hi;

    // Reset sourced files.
    க்கு (id = 1; id <= script_பொருட்கள்.ga_len; id++)
    {
	scriptitem_T *si = SCRIPT_ITEM(id);
	எனில் (si->sn_prof_on)
	{
	    si->sn_prof_on      = FALSE;
	    si->sn_pr_force     = FALSE;
	    profile_zero(&si->sn_pr_child);
	    si->sn_pr_nest      = 0;
	    si->sn_pr_மொத்தம்     = 0;
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
    todo = (முழு)functbl->ht_used;

    FOR_ALL_HASHTAB_பொருட்கள்(functbl, hi, todo)
    {
	ufunc_T *fp;
	முழு	i;

	எனில் (HASHITEM_EMPTY(hi))
	    தொடர்;

	todo--;
	fp = HI2UF(hi);
	எனில் (fp->uf_prof_initialized)
	{
	    fp->uf_profiling    = 0;
	    fp->uf_prof_initialized = FALSE;
	    fp->uf_tm_மொத்தம்     = 0;
	    profile_zero(&fp->uf_tm_total);
	    profile_zero(&fp->uf_tm_self);
	    profile_zero(&fp->uf_tm_children);

	    க்கு (i = 0; i < fp->uf_வரிகள்.ga_len; i++)
	    {
		fp->uf_tml_மொத்தம்[i] = 0;
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
    வெறுமை
ex_profile(exarg_T *eap)
{
    char_u	*e;
    முழு		len;

    e = skiptowhite(eap->arg);
    len = (முழு)(e - eap->arg);
    e = skipwhite(e);

    எனில் (len == 5 && STRNCMP(eap->arg, "start", 5) == 0 && *e != NUL)
    {
	VIM_CLEAR(profile_fname);
	profile_fname = expand_env_save_opt(e, TRUE, வெற்று);
	do_profiling = PROF_YES;
	profile_zero(&prof_wait_time);
	set_vim_var_nr(VV_PROFILING, 1L);
    }
    இல்லையெனில் எனில் (do_profiling == PROF_NONE)
	emsg(_(e_first_use_profile_start_fname));
    இல்லையெனில் எனில் (STRCMP(eap->arg, "stop") == 0)
    {
	profile_dump();
	do_profiling = PROF_NONE;
	set_vim_var_nr(VV_PROFILING, 0L);
	profile_reset();
    }
    இல்லையெனில் எனில் (STRCMP(eap->arg, "pause") == 0)
    {
	எனில் (do_profiling == PROF_YES)
	    profile_start(&pause_time);
	do_profiling = PROF_PAUSED;
    }
    இல்லையெனில் எனில் (STRCMP(eap->arg, "continue") == 0)
    {
	எனில் (do_profiling == PROF_PAUSED)
	{
	    profile_end(&pause_time);
	    profile_add(&prof_wait_time, &pause_time);
	}
	do_profiling = PROF_YES;
    }
    இல்லையெனில் எனில் (STRCMP(eap->arg, "dump") == 0)
	profile_dump();
    இல்லையெனில்
    {
	// The rest is similar to ":breakadd".
	ex_breakadd(eap);
    }
}

// Command line expansion for :profile.
நிலையான எண்ணிக்கை
{
    PEXP_SUBCMD,	// expand :profile sub-commands
    PEXP_FUNC		// expand :profile func {funcname}
} pexpand_what;

நிலையான எழுத்து *pexpand_cmds[] = {
			"start",
			"stop",
			"pause",
			"continue",
			"func",
			"file",
			"dump",
			வெற்று
};

/*
 * Function given to ExpandGeneric() to obtain the profile command
 * specific expansion.
 */
    char_u *
get_profile_name(expand_T *xp UNUSED, முழு idx)
{
    தேர்வு (pexpand_what)
    {
    நிலை PEXP_SUBCMD:
	திரும்பு (char_u *)pexpand_cmds[idx];
    இயல்பு:
	திரும்பு வெற்று;
    }
}

/*
 * Handle command line completion for :profile command.
 */
    வெறுமை
set_context_in_profile_cmd(expand_T *xp, char_u *arg)
{
    char_u	*end_subcmd;

    // Default: expand subcommands.
    xp->xp_context = EXPAND_PROFILE;
    pexpand_what = PEXP_SUBCMD;
    xp->xp_pattern = arg;

    end_subcmd = skiptowhite(arg);
    எனில் (*end_subcmd == NUL)
	திரும்பு;

    எனில் ((end_subcmd - arg == 5 && STRNCMP(arg, "start", 5) == 0)
	    || (end_subcmd - arg == 4 && STRNCMP(arg, "file", 4) == 0))
    {
	xp->xp_context = EXPAND_FILES;
	xp->xp_pattern = skipwhite(end_subcmd);
	திரும்பு;
    }
    இல்லையெனில் எனில் (end_subcmd - arg == 4 && STRNCMP(arg, "func", 4) == 0)
    {
	xp->xp_context = EXPAND_USER_FUNC;
	xp->xp_pattern = skipwhite(end_subcmd);
	திரும்பு;
    }

    xp->xp_context = EXPAND_NOTHING;
}

நிலையான proftime_T inchar_time;

/*
 * Called when starting to wait for the user to type a character.
 */
    வெறுமை
prof_inchar_enter(வெறுமை)
{
    profile_start(&inchar_time);
}

/*
 * Called when finished waiting for the user to type a character.
 */
    வெறுமை
prof_inchar_exit(வெறுமை)
{
    profile_end(&inchar_time);
    profile_add(&prof_wait_time, &inchar_time);
}


/*
 * Return TRUE when a function defined in the current script should be
 * profiled.
 */
    முழு
prof_def_func(வெறுமை)
{
    எனில் (current_sctx.sc_sid > 0)
	திரும்பு SCRIPT_ITEM(current_sctx.sc_sid)->sn_pr_force;
    திரும்பு FALSE;
}

/*
 * Print the count and times for one function or function line.
 */
    நிலையான வெறுமை
prof_func_வரி(
    FILE	*fd,
    முழு		மொத்தம்,
    proftime_T	*total,
    proftime_T	*self,
    முழு		prefer_self)	// when equal print only self time
{
    எனில் (மொத்தம் > 0)
    {
	fprintf(fd, "%5d ", மொத்தம்);
	எனில் (prefer_self && profile_equal(total, self))
	    fprintf(fd, PROF_TIME_BLANK);
	இல்லையெனில்
	    fprintf(fd, "%s ", profile_செய்தி(total));
	எனில் (!prefer_self && profile_equal(total, self))
	    fprintf(fd, PROF_TIME_BLANK);
	இல்லையெனில்
	    fprintf(fd, "%s ", profile_செய்தி(self));
    }
    இல்லையெனில்
	fprintf(fd, "      %s%s", PROF_TIME_BLANK, PROF_TIME_BLANK);
}

    நிலையான வெறுமை
prof_sort_list(
    FILE	*fd,
    ufunc_T	**sorttab,
    முழு		st_len,
    எழுத்து	*title,
    முழு		prefer_self)	// when equal print only self time
{
    முழு		i;
    ufunc_T	*fp;

    fprintf(fd, "FUNCTIONS SORTED ON %s TIME\n", title);
    fprintf(fd, "%s  function\n", PROF_TOTALS_HEADER);
    க்கு (i = 0; i < 20 && i < st_len; ++i)
    {
	fp = sorttab[i];
	prof_func_வரி(fd, fp->uf_tm_மொத்தம், &fp->uf_tm_total, &fp->uf_tm_self,
								 prefer_self);
	எனில் (fp->uf_name[0] == K_SPECIAL)
	    fprintf(fd, " <SNR>%s()\n", fp->uf_name + 3);
	இல்லையெனில்
	    fprintf(fd, " %s()\n", fp->uf_name);
    }
    fprintf(fd, "\n");
}

/*
 * Compare function for total time sorting.
 */
    நிலையான முழு
prof_total_cmp(மாறிலி வெறுமை *s1, மாறிலி வெறுமை *s2)
{
    ufunc_T	*p1, *p2;

    p1 = *(ufunc_T **)s1;
    p2 = *(ufunc_T **)s2;
    திரும்பு profile_cmp(&p1->uf_tm_total, &p2->uf_tm_total);
}

/*
 * Compare function for self time sorting.
 */
    நிலையான முழு
prof_self_cmp(மாறிலி வெறுமை *s1, மாறிலி வெறுமை *s2)
{
    ufunc_T	*p1, *p2;

    p1 = *(ufunc_T **)s1;
    p2 = *(ufunc_T **)s2;
    திரும்பு profile_cmp(&p1->uf_tm_self, &p2->uf_tm_self);
}

/*
 * Start profiling function "fp".
 */
    வெறுமை
func_do_profile(ufunc_T *fp)
{
    முழு		len = fp->uf_வரிகள்.ga_len;

    எனில் (!fp->uf_prof_initialized)
    {
	எனில் (len == 0)
	    len = 1;  // avoid getting error for allocating zero bytes
	fp->uf_tm_மொத்தம் = 0;
	profile_zero(&fp->uf_tm_self);
	profile_zero(&fp->uf_tm_total);
	எனில் (fp->uf_tml_மொத்தம் == வெற்று)
	    fp->uf_tml_மொத்தம் = ALLOC_CLEAR_MULT(முழு, len);
	எனில் (fp->uf_tml_total == வெற்று)
	    fp->uf_tml_total = ALLOC_CLEAR_MULT(proftime_T, len);
	எனில் (fp->uf_tml_self == வெற்று)
	    fp->uf_tml_self = ALLOC_CLEAR_MULT(proftime_T, len);
	fp->uf_tml_idx = -1;
	எனில் (fp->uf_tml_மொத்தம் == வெற்று || fp->uf_tml_total == வெற்று
						    || fp->uf_tml_self == வெற்று)
	    திரும்பு;	    // out of memory
	fp->uf_prof_initialized = TRUE;
    }

    fp->uf_profiling = TRUE;
}

/*
 * Save time when starting to invoke another script or function.
 */
    நிலையான வெறுமை
script_prof_save(
    proftime_T	*tm)	    // place to store wait time
{
    scriptitem_T    *si;

    எனில் (SCRIPT_ID_VALID(current_sctx.sc_sid))
    {
	si = SCRIPT_ITEM(current_sctx.sc_sid);
	எனில் (si->sn_prof_on && si->sn_pr_nest++ == 0)
	    profile_start(&si->sn_pr_child);
    }
    profile_get_wait(tm);
}

/*
 * When calling a function: may initialize for profiling.
 */
    வெறுமை
profile_may_start_func(profinfo_T *info, ufunc_T *fp, ufunc_T *caller)
{
    எனில் (!fp->uf_profiling && has_profiling(FALSE, fp->uf_name, வெற்று,
								&fp->uf_hash))
    {
	info->pi_started_profiling = TRUE;
	func_do_profile(fp);
    }
    எனில் (fp->uf_profiling || (caller != வெற்று && caller->uf_profiling))
    {
	++fp->uf_tm_மொத்தம்;
	profile_start(&info->pi_call_start);
	profile_zero(&fp->uf_tm_children);
    }
    script_prof_save(&info->pi_wait_start);
}

/*
 * After calling a function: may handle profiling.  profile_may_start_func()
 * must have been called previously.
 */
    வெறுமை
profile_may_end_func(profinfo_T *info, ufunc_T *fp, ufunc_T *caller)
{
    profile_end(&info->pi_call_start);
    profile_sub_wait(&info->pi_wait_start, &info->pi_call_start);
    profile_add(&fp->uf_tm_total, &info->pi_call_start);
    profile_self(&fp->uf_tm_self, &info->pi_call_start, &fp->uf_tm_children);
    எனில் (caller != வெற்று && caller->uf_profiling)
    {
	profile_add(&caller->uf_tm_children, &info->pi_call_start);
	profile_add(&caller->uf_tml_children, &info->pi_call_start);
    }
    எனில் (info->pi_started_profiling)
	// make a ":profdel func" stop profiling the function
	fp->uf_profiling = FALSE;
}

/*
 * Prepare profiling for entering a child or something else that is not
 * counted for the script/function itself.
 * Should always be called in pair with prof_child_exit().
 */
    வெறுமை
prof_child_enter(
    proftime_T *tm)	// place to store waittime
{
    funccall_T *fc = get_current_funccal();

    எனில் (fc != வெற்று && fc->fc_func->uf_profiling)
	profile_start(&fc->fc_prof_child);
    script_prof_save(tm);
}

/*
 * Take care of time spent in a child.
 * Should always be called after prof_child_enter().
 */
    வெறுமை
prof_child_exit(
    proftime_T *tm)	// where waittime was stored
{
    funccall_T *fc = get_current_funccal();

    எனில் (fc != வெற்று && fc->fc_func->uf_profiling)
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
    வெறுமை
func_வரி_start(வெறுமை *cookie, long lnum)
{
    funccall_T	*fcp = (funccall_T *)cookie;
    ufunc_T	*fp = fcp->fc_func;

    எனில் (fp->uf_profiling && lnum >= 1 && lnum <= fp->uf_வரிகள்.ga_len)
    {
	fp->uf_tml_idx = lnum - 1;
	// Skip continuation lines.
	போது (fp->uf_tml_idx > 0 && FUNCLINE(fp, fp->uf_tml_idx) == வெற்று)
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
    வெறுமை
func_வரி_exec(வெறுமை *cookie)
{
    funccall_T	*fcp = (funccall_T *)cookie;
    ufunc_T	*fp = fcp->fc_func;

    எனில் (fp->uf_profiling && fp->uf_tml_idx >= 0)
	fp->uf_tml_execed = TRUE;
}

/*
 * Called when done with a function line.
 */
    வெறுமை
func_வரி_end(வெறுமை *cookie)
{
    funccall_T	*fcp = (funccall_T *)cookie;
    ufunc_T	*fp = fcp->fc_func;

    எனில் (fp->uf_profiling && fp->uf_tml_idx >= 0)
    {
	எனில் (fp->uf_tml_execed)
	{
	    ++fp->uf_tml_மொத்தம்[fp->uf_tml_idx];
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
    நிலையான வெறுமை
func_dump_profile(FILE *fd)
{
    hashtab_T	*functbl;
    hashitem_T	*hi;
    முழு		todo;
    ufunc_T	*fp;
    முழு		i;
    ufunc_T	**sorttab;
    முழு		st_len = 0;
    char_u	*p;

    functbl = func_tbl_get();
    todo = (முழு)functbl->ht_used;
    எனில் (todo == 0)
	திரும்பு;     // nothing to dump

    sorttab = ALLOC_MULT(ufunc_T *, todo);

    FOR_ALL_HASHTAB_பொருட்கள்(functbl, hi, todo)
    {
	எனில் (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    fp = HI2UF(hi);
	    எனில் (fp->uf_prof_initialized)
	    {
		எனில் (sorttab != வெற்று)
		    sorttab[st_len++] = fp;

		எனில் (fp->uf_name[0] == K_SPECIAL)
		    fprintf(fd, "FUNCTION  <SNR>%s()\n", fp->uf_name + 3);
		இல்லையெனில்
		    fprintf(fd, "FUNCTION  %s()\n", fp->uf_name);
		எனில் (fp->uf_script_ctx.sc_sid > 0)
		{
		    p = home_replace_save(வெற்று,
				     get_scriptname(fp->uf_script_ctx.sc_sid));
		    எனில் (p != வெற்று)
		    {
			fprintf(fd, "    Defined: %s:%ld\n",
					   p, (long)fp->uf_script_ctx.sc_lnum);
			vim_free(p);
		    }
		}
		எனில் (fp->uf_tm_மொத்தம் == 1)
		    fprintf(fd, "Called 1 time\n");
		இல்லையெனில்
		    fprintf(fd, "Called %d times\n", fp->uf_tm_மொத்தம்);
		fprintf(fd, "Total time: %s\n", profile_செய்தி(&fp->uf_tm_total));
		fprintf(fd, " Self time: %s\n", profile_செய்தி(&fp->uf_tm_self));
		fprintf(fd, "\n");
		fprintf(fd, "%s\n", PROF_TOTALS_HEADER);

		க்கு (i = 0; i < fp->uf_வரிகள்.ga_len; ++i)
		{
		    எனில் (FUNCLINE(fp, i) == வெற்று)
			தொடர்;
		    prof_func_வரி(fd, fp->uf_tml_மொத்தம்[i],
			     &fp->uf_tml_total[i], &fp->uf_tml_self[i], TRUE);
		    fprintf(fd, "%s\n", FUNCLINE(fp, i));
		}
		fprintf(fd, "\n");
	    }
	}
    }

    எனில் (sorttab != வெற்று && st_len > 0)
    {
	qsort((வெறுமை *)sorttab, (size_t)st_len, அளவு(ufunc_T *),
							      prof_total_cmp);
	prof_sort_list(fd, sorttab, st_len, "TOTAL", FALSE);
	qsort((வெறுமை *)sorttab, (size_t)st_len, அளவு(ufunc_T *),
							      prof_self_cmp);
	prof_sort_list(fd, sorttab, st_len, "SELF", TRUE);
    }

    vim_free(sorttab);
}

/*
 * Start profiling script "fp".
 */
    வெறுமை
script_do_profile(scriptitem_T *si)
{
    si->sn_pr_மொத்தம் = 0;
    profile_zero(&si->sn_pr_total);
    profile_zero(&si->sn_pr_self);

    ga_init2(&si->sn_prl_ga, அளவு(sn_prl_T), 100);
    si->sn_prl_idx = -1;
    si->sn_prof_on = TRUE;
    si->sn_pr_nest = 0;
}

/*
 * Count time spent in children after invoking another script or function.
 */
    வெறுமை
script_prof_restore(proftime_T *tm)
{
    scriptitem_T    *si;

    எனில் (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	திரும்பு;

    si = SCRIPT_ITEM(current_sctx.sc_sid);
    எனில் (si->sn_prof_on && --si->sn_pr_nest == 0)
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
    நிலையான வெறுமை
script_dump_profile(FILE *fd)
{
    முழு		    id;
    scriptitem_T    *si;
    முழு		    i;
    FILE	    *sfd;
    sn_prl_T	    *pp;

    க்கு (id = 1; id <= script_பொருட்கள்.ga_len; ++id)
    {
	si = SCRIPT_ITEM(id);
	எனில் (si->sn_prof_on)
	{
	    fprintf(fd, "SCRIPT  %s\n", si->sn_name);
	    எனில் (si->sn_pr_மொத்தம் == 1)
		fprintf(fd, "Sourced 1 time\n");
	    இல்லையெனில்
		fprintf(fd, "Sourced %d times\n", si->sn_pr_மொத்தம்);
	    fprintf(fd, "Total time: %s\n", profile_செய்தி(&si->sn_pr_total));
	    fprintf(fd, " Self time: %s\n", profile_செய்தி(&si->sn_pr_self));
	    fprintf(fd, "\n");
	    fprintf(fd, "%s\n", PROF_TOTALS_HEADER);

	    sfd = mch_fopen((எழுத்து *)si->sn_name, "r");
	    எனில் (sfd == வெற்று)
		fprintf(fd, "Cannot open file!\n");
	    இல்லையெனில்
	    {
		// Keep going till the end of file, so that trailing
		// continuation lines are listed.
		க்கு (i = 0; ; ++i)
		{
		    எனில் (vim_fgets(IObuff, IOSIZE, sfd))
			நிறுத்து;
		    // When a line has been truncated, append NL, taking care
		    // of multi-byte characters .
		    எனில் (IObuff[IOSIZE - 2] != NUL && IObuff[IOSIZE - 2] != NL)
		    {
			முழு n = IOSIZE - 2;

			எனில் (enc_utf8)
			{
			    // Move to the first byte of this char.
			    // utf_head_off() doesn't work, because it checks
			    // for a truncated character.
			    போது (n > 0 && (IObuff[n] & 0xc0) == 0x80)
				--n;
			}
			இல்லையெனில் எனில் (has_mbyte)
			    n -= mb_head_off(IObuff, IObuff + n);
			IObuff[n] = NL;
			IObuff[n + 1] = NUL;
		    }
		    எனில் (i < si->sn_prl_ga.ga_len
				     && (pp = &PRL_ITEM(si, i))->snp_மொத்தம் > 0)
		    {
			fprintf(fd, "%5d ", pp->snp_மொத்தம்);
			எனில் (profile_equal(&pp->sn_prl_total, &pp->sn_prl_self))
			    fprintf(fd, "           ");
			இல்லையெனில்
			    fprintf(fd, "%s ", profile_செய்தி(&pp->sn_prl_total));
			fprintf(fd, "%s ", profile_செய்தி(&pp->sn_prl_self));
		    }
		    இல்லையெனில்
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
    வெறுமை
profile_dump(வெறுமை)
{
    FILE	*fd;

    எனில் (profile_fname == வெற்று)
	திரும்பு;

    fd = mch_fopen((எழுத்து *)profile_fname, "w");
    எனில் (fd == வெற்று)
	semsg(_(e_cant_open_file_str), profile_fname);
    இல்லையெனில்
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
    வெறுமை
script_வரி_start(வெறுமை)
{
    scriptitem_T    *si;
    sn_prl_T	    *pp;

    எனில் (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	திரும்பு;
    si = SCRIPT_ITEM(current_sctx.sc_sid);
    எனில் (si->sn_prof_on && SOURCING_LNUM >= 1)
    {
	// Grow the array before starting the timer, so that the time spent
	// here isn't counted.
	(வெறுமை)ga_grow(&si->sn_prl_ga,
				  (முழு)(SOURCING_LNUM - si->sn_prl_ga.ga_len));
	si->sn_prl_idx = SOURCING_LNUM - 1;
	போது (si->sn_prl_ga.ga_len <= si->sn_prl_idx
		&& si->sn_prl_ga.ga_len < si->sn_prl_ga.ga_maxlen)
	{
	    // Zero counters for a line that was not used before.
	    pp = &PRL_ITEM(si, si->sn_prl_ga.ga_len);
	    pp->snp_மொத்தம் = 0;
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
    வெறுமை
script_வரி_exec(வெறுமை)
{
    scriptitem_T    *si;

    எனில் (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	திரும்பு;
    si = SCRIPT_ITEM(current_sctx.sc_sid);
    எனில் (si->sn_prof_on && si->sn_prl_idx >= 0)
	si->sn_prl_execed = TRUE;
}

/*
 * Called when done with a script line.
 */
    வெறுமை
script_வரி_end(வெறுமை)
{
    scriptitem_T    *si;
    sn_prl_T	    *pp;

    எனில் (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	திரும்பு;
    si = SCRIPT_ITEM(current_sctx.sc_sid);
    எனில் (si->sn_prof_on && si->sn_prl_idx >= 0
				     && si->sn_prl_idx < si->sn_prl_ga.ga_len)
    {
	எனில் (si->sn_prl_execed)
	{
	    pp = &PRL_ITEM(si, si->sn_prl_idx);
	    ++pp->snp_மொத்தம்;
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
