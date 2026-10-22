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

#अन्तर्भाव "vim.h"

#जइ defined(FEAT_EVAL)
# जइ defined(FEAT_PROFILE) || defined(FEAT_RELTIME)
/*
 * Store the current time in "tm".
 */
    शून्य
profile_start(proftime_T *tm)
{
#  ifdef MSWIN
    QueryPerformanceCounter(tm);
#  अण्णहा
    PROF_GET_TIME(tm);
#  endif
}

/*
 * Compute the elapsed time from "tm" till now and store in "tm".
 */
    शून्य
profile_end(proftime_T *tm)
{
    proftime_T now;

#  ifdef MSWIN
    QueryPerformanceCounter(&now);
    tm->QuadPart = now.QuadPart - tm->QuadPart;
#  अण्णहा
    PROF_GET_TIME(&now);
    tm->tv_fsec = now.tv_fsec - tm->tv_fsec;
    tm->tv_sec = now.tv_sec - tm->tv_sec;
    जइ (tm->tv_fsec < 0)
    {
	tm->tv_fsec += TV_FSEC_SEC;
	--tm->tv_sec;
    }
#  endif
}

/*
 * Subtract the time "tm2" from "tm".
 */
    शून्य
profile_sub(proftime_T *tm, proftime_T *tm2)
{
#  ifdef MSWIN
    tm->QuadPart -= tm2->QuadPart;
#  अण्णहा
    tm->tv_fsec -= tm2->tv_fsec;
    tm->tv_sec -= tm2->tv_sec;
    जइ (tm->tv_fsec < 0)
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
    वर्ण *
profile_सन्देशः(proftime_T *tm)
{
    स्थिर वर्ण buf[50];

#  ifdef MSWIN
    LARGE_INTEGER   fr;

    QueryPerformanceFrequency(&fr);
    sprintf(buf, "%10.6lf", (द्विगुण)tm->QuadPart / (द्विगुण)fr.QuadPart);
#  अण्णहा
    sprintf(buf, PROF_TIME_FORMAT, (long)tm->tv_sec, (long)tm->tv_fsec);
#  endif
    प्रत्यागम buf;
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
    प्रत्यागम (float_T)tm->QuadPart / (float_T)fr.QuadPart;
#  अण्णहा
    प्रत्यागम (float_T)tm->tv_sec + (float_T)tm->tv_fsec / (float_T)TV_FSEC_SEC;
#  endif
}

/*
 * Put the time "msec" past now in "tm".
 */
    शून्य
profile_setlimit(long msec, proftime_T *tm)
{
    जइ (msec <= 0)   // no limit
	profile_zero(tm);
    अण्णहा
    {
#  ifdef MSWIN
	LARGE_INTEGER   fr;

	QueryPerformanceCounter(tm);
	QueryPerformanceFrequency(&fr);
	tm->QuadPart += (LONGLONG)((द्विगुण)msec / 1000.0 * (द्विगुण)fr.QuadPart);
#  अण्णहा
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
    पूर्णाङ्क
profile_passed_limit(proftime_T *tm)
{
    proftime_T	now;

#  ifdef MSWIN
    जइ (tm->QuadPart == 0)  // timer was not set
	प्रत्यागम FALSE;
    QueryPerformanceCounter(&now);
    प्रत्यागम (now.QuadPart > tm->QuadPart);
#  अण्णहा
    जइ (tm->tv_sec == 0)    // timer was not set
	प्रत्यागम FALSE;
    PROF_GET_TIME(&now);
    प्रत्यागम (now.tv_sec > tm->tv_sec
	    || (now.tv_sec == tm->tv_sec && now.tv_fsec > tm->tv_fsec));
#  endif
}

/*
 * Set the time in "tm" to zero.
 */
    शून्य
profile_zero(proftime_T *tm)
{
#  ifdef MSWIN
    tm->QuadPart = 0;
#  अण्णहा
    tm->tv_fsec = 0;
    tm->tv_sec = 0;
#  endif
}

# endif  // FEAT_PROFILE || FEAT_RELTIME

# जइ defined(FEAT_SYN_HL) && defined(FEAT_RELTIME) && defined(FEAT_PROFILE)
#  जइ defined(HAVE_MATH_H)
#   include <math.h>
#  endif

/*
 * Divide the time "tm" by "count" and store in "tm2".
 */
    शून्य
profile_divide(proftime_T *tm, पूर्णाङ्क गणनम्, proftime_T *tm2)
{
    जइ (गणनम् == 0)
	profile_zero(tm2);
    अण्णहा
    {
#  ifdef MSWIN
	tm2->QuadPart = tm->QuadPart / गणनम्;
#  अण्णहा
	द्विगुण fsec = (tm->tv_sec * (float_T)TV_FSEC_SEC + tm->tv_fsec)
								       / गणनम्;

	tm2->tv_sec = floor(fsec / (float_T)TV_FSEC_SEC);
	tm2->tv_fsec = vim_round(fsec - (tm2->tv_sec * (float_T)TV_FSEC_SEC));
#  endif
    }
}
# endif

# जइ defined(FEAT_PROFILE)
/*
 * Functions for profiling.
 */
स्थिर proftime_T prof_wait_time;

/*
 * Add the time "tm2" to "tm".
 */
    शून्य
profile_add(proftime_T *tm, proftime_T *tm2)
{
#  ifdef MSWIN
    tm->QuadPart += tm2->QuadPart;
#  अण्णहा
    tm->tv_fsec += tm2->tv_fsec;
    tm->tv_sec += tm2->tv_sec;
    जइ (tm->tv_fsec >= TV_FSEC_SEC)
    {
	tm->tv_fsec -= TV_FSEC_SEC;
	++tm->tv_sec;
    }
#  endif
}

/*
 * Add the "self" time from the total time and the children's time.
 */
    शून्य
profile_self(proftime_T *self, proftime_T *total, proftime_T *children)
{
    // Check that the result won't be negative.  Can happen with recursive
    // calls.
#  ifdef MSWIN
    जइ (total->QuadPart <= children->QuadPart)
	प्रत्यागम;
#  अण्णहा
    जइ (total->tv_sec < children->tv_sec
	    || (total->tv_sec == children->tv_sec
		&& total->tv_fsec <= children->tv_fsec))
	प्रत्यागम;
#  endif
    profile_add(self, total);
    profile_sub(self, children);
}

/*
 * Get the current waittime.
 */
    स्थिर शून्य
profile_get_wait(proftime_T *tm)
{
    *tm = prof_wait_time;
}

/*
 * Subtract the passed waittime since "tm" from "tma".
 */
    शून्य
profile_sub_wait(proftime_T *tm, proftime_T *tma)
{
    proftime_T tm3 = prof_wait_time;

    profile_sub(&tm3, tm);
    profile_sub(tma, &tm3);
}

/*
 * Return TRUE if "tm1" and "tm2" are equal.
 */
    स्थिर पूर्णाङ्क
profile_equal(proftime_T *tm1, proftime_T *tm2)
{
#  ifdef MSWIN
    प्रत्यागम (tm1->QuadPart == tm2->QuadPart);
#  अण्णहा
    प्रत्यागम (tm1->tv_fsec == tm2->tv_fsec && tm1->tv_sec == tm2->tv_sec);
#  endif
}

/*
 * Return <0, 0 or >0 if "tm1" < "tm2", "tm1" == "tm2" or "tm1" > "tm2"
 */
    पूर्णाङ्क
profile_cmp(ध्रुव proftime_T *tm1, ध्रुव proftime_T *tm2)
{
#  ifdef MSWIN
    प्रत्यागम tm2->QuadPart == tm1->QuadPart ? 0 :
	tm2->QuadPart > tm1->QuadPart ? 1 : -1;
#  अण्णहा
    जइ (tm1->tv_sec == tm2->tv_sec)
	प्रत्यागम tm2->tv_fsec == tm1->tv_fsec ? 0 :
	    tm2->tv_fsec > tm1->tv_fsec ? 1 : -1;
    प्रत्यागम tm2->tv_sec > tm1->tv_sec ? 1 : -1;
#  endif
}

स्थिर char_u	*profile_fname = रिक्त;
स्थिर proftime_T pause_time;

/*
 * Reset all profiling information.
 */
    स्थिर शून्य
profile_reset(शून्य)
{
    पूर्णाङ्क		id;
    पूर्णाङ्क		todo;
    hashtab_T	*functbl;
    hashitem_T	*hi;

    // Reset sourced files.
    प्रति (id = 1; id <= script_वस्तूनि.ga_len; id++)
    {
	scriptitem_T *si = SCRIPT_ITEM(id);
	जइ (si->sn_prof_on)
	{
	    si->sn_prof_on      = FALSE;
	    si->sn_pr_force     = FALSE;
	    profile_zero(&si->sn_pr_child);
	    si->sn_pr_nest      = 0;
	    si->sn_pr_गणनम्     = 0;
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
    todo = (पूर्णाङ्क)functbl->ht_used;

    FOR_ALL_HASHTAB_वस्तूनि(functbl, hi, todo)
    {
	ufunc_T *fp;
	पूर्णाङ्क	i;

	जइ (HASHITEM_EMPTY(hi))
	    अनुवर्त;

	todo--;
	fp = HI2UF(hi);
	जइ (fp->uf_prof_initialized)
	{
	    fp->uf_profiling    = 0;
	    fp->uf_prof_initialized = FALSE;
	    fp->uf_tm_गणनम्     = 0;
	    profile_zero(&fp->uf_tm_total);
	    profile_zero(&fp->uf_tm_self);
	    profile_zero(&fp->uf_tm_children);

	    प्रति (i = 0; i < fp->uf_पङ्क्तयः.ga_len; i++)
	    {
		fp->uf_tml_गणनम्[i] = 0;
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
    शून्य
ex_profile(exarg_T *eap)
{
    char_u	*e;
    पूर्णाङ्क		len;

    e = skiptowhite(eap->arg);
    len = (पूर्णाङ्क)(e - eap->arg);
    e = skipwhite(e);

    जइ (len == 5 && STRNCMP(eap->arg, "start", 5) == 0 && *e != NUL)
    {
	VIM_CLEAR(profile_fname);
	profile_fname = expand_env_save_opt(e, TRUE, रिक्त);
	do_profiling = PROF_YES;
	profile_zero(&prof_wait_time);
	set_vim_var_nr(VV_PROFILING, 1L);
    }
    अण्णहा जइ (do_profiling == PROF_NONE)
	emsg(_(e_first_use_profile_start_fname));
    अण्णहा जइ (STRCMP(eap->arg, "stop") == 0)
    {
	profile_dump();
	do_profiling = PROF_NONE;
	set_vim_var_nr(VV_PROFILING, 0L);
	profile_reset();
    }
    अण्णहा जइ (STRCMP(eap->arg, "pause") == 0)
    {
	जइ (do_profiling == PROF_YES)
	    profile_start(&pause_time);
	do_profiling = PROF_PAUSED;
    }
    अण्णहा जइ (STRCMP(eap->arg, "continue") == 0)
    {
	जइ (do_profiling == PROF_PAUSED)
	{
	    profile_end(&pause_time);
	    profile_add(&prof_wait_time, &pause_time);
	}
	do_profiling = PROF_YES;
    }
    अण्णहा जइ (STRCMP(eap->arg, "dump") == 0)
	profile_dump();
    अण्णहा
    {
	// The rest is similar to ":breakadd".
	ex_breakadd(eap);
    }
}

// Command line expansion for :profile.
स्थिर गणना
{
    PEXP_SUBCMD,	// expand :profile sub-commands
    PEXP_FUNC		// expand :profile func {funcname}
} pexpand_what;

स्थिर वर्ण *pexpand_cmds[] = {
			"start",
			"stop",
			"pause",
			"continue",
			"func",
			"file",
			"dump",
			रिक्त
};

/*
 * Function given to ExpandGeneric() to obtain the profile command
 * specific expansion.
 */
    char_u *
get_profile_name(expand_T *xp UNUSED, पूर्णाङ्क idx)
{
    विकल्प (pexpand_what)
    {
    पक्ष PEXP_SUBCMD:
	प्रत्यागम (char_u *)pexpand_cmds[idx];
    स्वभाव:
	प्रत्यागम रिक्त;
    }
}

/*
 * Handle command line completion for :profile command.
 */
    शून्य
set_context_in_profile_cmd(expand_T *xp, char_u *arg)
{
    char_u	*end_subcmd;

    // Default: expand subcommands.
    xp->xp_context = EXPAND_PROFILE;
    pexpand_what = PEXP_SUBCMD;
    xp->xp_pattern = arg;

    end_subcmd = skiptowhite(arg);
    जइ (*end_subcmd == NUL)
	प्रत्यागम;

    जइ ((end_subcmd - arg == 5 && STRNCMP(arg, "start", 5) == 0)
	    || (end_subcmd - arg == 4 && STRNCMP(arg, "file", 4) == 0))
    {
	xp->xp_context = EXPAND_FILES;
	xp->xp_pattern = skipwhite(end_subcmd);
	प्रत्यागम;
    }
    अण्णहा जइ (end_subcmd - arg == 4 && STRNCMP(arg, "func", 4) == 0)
    {
	xp->xp_context = EXPAND_USER_FUNC;
	xp->xp_pattern = skipwhite(end_subcmd);
	प्रत्यागम;
    }

    xp->xp_context = EXPAND_NOTHING;
}

स्थिर proftime_T inchar_time;

/*
 * Called when starting to wait for the user to type a character.
 */
    शून्य
prof_inchar_enter(शून्य)
{
    profile_start(&inchar_time);
}

/*
 * Called when finished waiting for the user to type a character.
 */
    शून्य
prof_inchar_exit(शून्य)
{
    profile_end(&inchar_time);
    profile_add(&prof_wait_time, &inchar_time);
}


/*
 * Return TRUE when a function defined in the current script should be
 * profiled.
 */
    पूर्णाङ्क
prof_def_func(शून्य)
{
    जइ (current_sctx.sc_sid > 0)
	प्रत्यागम SCRIPT_ITEM(current_sctx.sc_sid)->sn_pr_force;
    प्रत्यागम FALSE;
}

/*
 * Print the count and times for one function or function line.
 */
    स्थिर शून्य
prof_func_पङ्क्तिः(
    FILE	*fd,
    पूर्णाङ्क		गणनम्,
    proftime_T	*total,
    proftime_T	*self,
    पूर्णाङ्क		prefer_self)	// when equal print only self time
{
    जइ (गणनम् > 0)
    {
	fprintf(fd, "%5d ", गणनम्);
	जइ (prefer_self && profile_equal(total, self))
	    fprintf(fd, PROF_TIME_BLANK);
	अण्णहा
	    fprintf(fd, "%s ", profile_सन्देशः(total));
	जइ (!prefer_self && profile_equal(total, self))
	    fprintf(fd, PROF_TIME_BLANK);
	अण्णहा
	    fprintf(fd, "%s ", profile_सन्देशः(self));
    }
    अण्णहा
	fprintf(fd, "      %s%s", PROF_TIME_BLANK, PROF_TIME_BLANK);
}

    स्थिर शून्य
prof_sort_list(
    FILE	*fd,
    ufunc_T	**sorttab,
    पूर्णाङ्क		st_len,
    वर्ण	*title,
    पूर्णाङ्क		prefer_self)	// when equal print only self time
{
    पूर्णाङ्क		i;
    ufunc_T	*fp;

    fprintf(fd, "FUNCTIONS SORTED ON %s TIME\n", title);
    fprintf(fd, "%s  function\n", PROF_TOTALS_HEADER);
    प्रति (i = 0; i < 20 && i < st_len; ++i)
    {
	fp = sorttab[i];
	prof_func_पङ्क्तिः(fd, fp->uf_tm_गणनम्, &fp->uf_tm_total, &fp->uf_tm_self,
								 prefer_self);
	जइ (fp->uf_name[0] == K_SPECIAL)
	    fprintf(fd, " <SNR>%s()\n", fp->uf_name + 3);
	अण्णहा
	    fprintf(fd, " %s()\n", fp->uf_name);
    }
    fprintf(fd, "\n");
}

/*
 * Compare function for total time sorting.
 */
    स्थिर पूर्णाङ्क
prof_total_cmp(ध्रुव शून्य *s1, ध्रुव शून्य *s2)
{
    ufunc_T	*p1, *p2;

    p1 = *(ufunc_T **)s1;
    p2 = *(ufunc_T **)s2;
    प्रत्यागम profile_cmp(&p1->uf_tm_total, &p2->uf_tm_total);
}

/*
 * Compare function for self time sorting.
 */
    स्थिर पूर्णाङ्क
prof_self_cmp(ध्रुव शून्य *s1, ध्रुव शून्य *s2)
{
    ufunc_T	*p1, *p2;

    p1 = *(ufunc_T **)s1;
    p2 = *(ufunc_T **)s2;
    प्रत्यागम profile_cmp(&p1->uf_tm_self, &p2->uf_tm_self);
}

/*
 * Start profiling function "fp".
 */
    शून्य
func_do_profile(ufunc_T *fp)
{
    पूर्णाङ्क		len = fp->uf_पङ्क्तयः.ga_len;

    जइ (!fp->uf_prof_initialized)
    {
	जइ (len == 0)
	    len = 1;  // avoid getting error for allocating zero bytes
	fp->uf_tm_गणनम् = 0;
	profile_zero(&fp->uf_tm_self);
	profile_zero(&fp->uf_tm_total);
	जइ (fp->uf_tml_गणनम् == रिक्त)
	    fp->uf_tml_गणनम् = ALLOC_CLEAR_MULT(पूर्णाङ्क, len);
	जइ (fp->uf_tml_total == रिक्त)
	    fp->uf_tml_total = ALLOC_CLEAR_MULT(proftime_T, len);
	जइ (fp->uf_tml_self == रिक्त)
	    fp->uf_tml_self = ALLOC_CLEAR_MULT(proftime_T, len);
	fp->uf_tml_idx = -1;
	जइ (fp->uf_tml_गणनम् == रिक्त || fp->uf_tml_total == रिक्त
						    || fp->uf_tml_self == रिक्त)
	    प्रत्यागम;	    // out of memory
	fp->uf_prof_initialized = TRUE;
    }

    fp->uf_profiling = TRUE;
}

/*
 * Save time when starting to invoke another script or function.
 */
    स्थिर शून्य
script_prof_save(
    proftime_T	*tm)	    // place to store wait time
{
    scriptitem_T    *si;

    जइ (SCRIPT_ID_VALID(current_sctx.sc_sid))
    {
	si = SCRIPT_ITEM(current_sctx.sc_sid);
	जइ (si->sn_prof_on && si->sn_pr_nest++ == 0)
	    profile_start(&si->sn_pr_child);
    }
    profile_get_wait(tm);
}

/*
 * When calling a function: may initialize for profiling.
 */
    शून्य
profile_may_start_func(profinfo_T *info, ufunc_T *fp, ufunc_T *caller)
{
    जइ (!fp->uf_profiling && has_profiling(FALSE, fp->uf_name, रिक्त,
								&fp->uf_hash))
    {
	info->pi_started_profiling = TRUE;
	func_do_profile(fp);
    }
    जइ (fp->uf_profiling || (caller != रिक्त && caller->uf_profiling))
    {
	++fp->uf_tm_गणनम्;
	profile_start(&info->pi_call_start);
	profile_zero(&fp->uf_tm_children);
    }
    script_prof_save(&info->pi_wait_start);
}

/*
 * After calling a function: may handle profiling.  profile_may_start_func()
 * must have been called previously.
 */
    शून्य
profile_may_end_func(profinfo_T *info, ufunc_T *fp, ufunc_T *caller)
{
    profile_end(&info->pi_call_start);
    profile_sub_wait(&info->pi_wait_start, &info->pi_call_start);
    profile_add(&fp->uf_tm_total, &info->pi_call_start);
    profile_self(&fp->uf_tm_self, &info->pi_call_start, &fp->uf_tm_children);
    जइ (caller != रिक्त && caller->uf_profiling)
    {
	profile_add(&caller->uf_tm_children, &info->pi_call_start);
	profile_add(&caller->uf_tml_children, &info->pi_call_start);
    }
    जइ (info->pi_started_profiling)
	// make a ":profdel func" stop profiling the function
	fp->uf_profiling = FALSE;
}

/*
 * Prepare profiling for entering a child or something else that is not
 * counted for the script/function itself.
 * Should always be called in pair with prof_child_exit().
 */
    शून्य
prof_child_enter(
    proftime_T *tm)	// place to store waittime
{
    funccall_T *fc = get_current_funccal();

    जइ (fc != रिक्त && fc->fc_func->uf_profiling)
	profile_start(&fc->fc_prof_child);
    script_prof_save(tm);
}

/*
 * Take care of time spent in a child.
 * Should always be called after prof_child_enter().
 */
    शून्य
prof_child_exit(
    proftime_T *tm)	// where waittime was stored
{
    funccall_T *fc = get_current_funccal();

    जइ (fc != रिक्त && fc->fc_func->uf_profiling)
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
    शून्य
func_पङ्क्तिः_start(शून्य *cookie, long lnum)
{
    funccall_T	*fcp = (funccall_T *)cookie;
    ufunc_T	*fp = fcp->fc_func;

    जइ (fp->uf_profiling && lnum >= 1 && lnum <= fp->uf_पङ्क्तयः.ga_len)
    {
	fp->uf_tml_idx = lnum - 1;
	// Skip continuation lines.
	जाव (fp->uf_tml_idx > 0 && FUNCLINE(fp, fp->uf_tml_idx) == रिक्त)
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
    शून्य
func_पङ्क्तिः_exec(शून्य *cookie)
{
    funccall_T	*fcp = (funccall_T *)cookie;
    ufunc_T	*fp = fcp->fc_func;

    जइ (fp->uf_profiling && fp->uf_tml_idx >= 0)
	fp->uf_tml_execed = TRUE;
}

/*
 * Called when done with a function line.
 */
    शून्य
func_पङ्क्तिः_end(शून्य *cookie)
{
    funccall_T	*fcp = (funccall_T *)cookie;
    ufunc_T	*fp = fcp->fc_func;

    जइ (fp->uf_profiling && fp->uf_tml_idx >= 0)
    {
	जइ (fp->uf_tml_execed)
	{
	    ++fp->uf_tml_गणनम्[fp->uf_tml_idx];
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
    स्थिर शून्य
func_dump_profile(FILE *fd)
{
    hashtab_T	*functbl;
    hashitem_T	*hi;
    पूर्णाङ्क		todo;
    ufunc_T	*fp;
    पूर्णाङ्क		i;
    ufunc_T	**sorttab;
    पूर्णाङ्क		st_len = 0;
    char_u	*p;

    functbl = func_tbl_get();
    todo = (पूर्णाङ्क)functbl->ht_used;
    जइ (todo == 0)
	प्रत्यागम;     // nothing to dump

    sorttab = ALLOC_MULT(ufunc_T *, todo);

    FOR_ALL_HASHTAB_वस्तूनि(functbl, hi, todo)
    {
	जइ (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    fp = HI2UF(hi);
	    जइ (fp->uf_prof_initialized)
	    {
		जइ (sorttab != रिक्त)
		    sorttab[st_len++] = fp;

		जइ (fp->uf_name[0] == K_SPECIAL)
		    fprintf(fd, "FUNCTION  <SNR>%s()\n", fp->uf_name + 3);
		अण्णहा
		    fprintf(fd, "FUNCTION  %s()\n", fp->uf_name);
		जइ (fp->uf_script_ctx.sc_sid > 0)
		{
		    p = home_replace_save(रिक्त,
				     get_scriptname(fp->uf_script_ctx.sc_sid));
		    जइ (p != रिक्त)
		    {
			fprintf(fd, "    Defined: %s:%ld\n",
					   p, (long)fp->uf_script_ctx.sc_lnum);
			vim_free(p);
		    }
		}
		जइ (fp->uf_tm_गणनम् == 1)
		    fprintf(fd, "Called 1 time\n");
		अण्णहा
		    fprintf(fd, "Called %d times\n", fp->uf_tm_गणनम्);
		fprintf(fd, "Total time: %s\n", profile_सन्देशः(&fp->uf_tm_total));
		fprintf(fd, " Self time: %s\n", profile_सन्देशः(&fp->uf_tm_self));
		fprintf(fd, "\n");
		fprintf(fd, "%s\n", PROF_TOTALS_HEADER);

		प्रति (i = 0; i < fp->uf_पङ्क्तयः.ga_len; ++i)
		{
		    जइ (FUNCLINE(fp, i) == रिक्त)
			अनुवर्त;
		    prof_func_पङ्क्तिः(fd, fp->uf_tml_गणनम्[i],
			     &fp->uf_tml_total[i], &fp->uf_tml_self[i], TRUE);
		    fprintf(fd, "%s\n", FUNCLINE(fp, i));
		}
		fprintf(fd, "\n");
	    }
	}
    }

    जइ (sorttab != रिक्त && st_len > 0)
    {
	qsort((शून्य *)sorttab, (size_t)st_len, परिमाण(ufunc_T *),
							      prof_total_cmp);
	prof_sort_list(fd, sorttab, st_len, "TOTAL", FALSE);
	qsort((शून्य *)sorttab, (size_t)st_len, परिमाण(ufunc_T *),
							      prof_self_cmp);
	prof_sort_list(fd, sorttab, st_len, "SELF", TRUE);
    }

    vim_free(sorttab);
}

/*
 * Start profiling script "fp".
 */
    शून्य
script_do_profile(scriptitem_T *si)
{
    si->sn_pr_गणनम् = 0;
    profile_zero(&si->sn_pr_total);
    profile_zero(&si->sn_pr_self);

    ga_init2(&si->sn_prl_ga, परिमाण(sn_prl_T), 100);
    si->sn_prl_idx = -1;
    si->sn_prof_on = TRUE;
    si->sn_pr_nest = 0;
}

/*
 * Count time spent in children after invoking another script or function.
 */
    शून्य
script_prof_restore(proftime_T *tm)
{
    scriptitem_T    *si;

    जइ (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	प्रत्यागम;

    si = SCRIPT_ITEM(current_sctx.sc_sid);
    जइ (si->sn_prof_on && --si->sn_pr_nest == 0)
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
    स्थिर शून्य
script_dump_profile(FILE *fd)
{
    पूर्णाङ्क		    id;
    scriptitem_T    *si;
    पूर्णाङ्क		    i;
    FILE	    *sfd;
    sn_prl_T	    *pp;

    प्रति (id = 1; id <= script_वस्तूनि.ga_len; ++id)
    {
	si = SCRIPT_ITEM(id);
	जइ (si->sn_prof_on)
	{
	    fprintf(fd, "SCRIPT  %s\n", si->sn_name);
	    जइ (si->sn_pr_गणनम् == 1)
		fprintf(fd, "Sourced 1 time\n");
	    अण्णहा
		fprintf(fd, "Sourced %d times\n", si->sn_pr_गणनम्);
	    fprintf(fd, "Total time: %s\n", profile_सन्देशः(&si->sn_pr_total));
	    fprintf(fd, " Self time: %s\n", profile_सन्देशः(&si->sn_pr_self));
	    fprintf(fd, "\n");
	    fprintf(fd, "%s\n", PROF_TOTALS_HEADER);

	    sfd = mch_fopen((वर्ण *)si->sn_name, "r");
	    जइ (sfd == रिक्त)
		fprintf(fd, "Cannot open file!\n");
	    अण्णहा
	    {
		// Keep going till the end of file, so that trailing
		// continuation lines are listed.
		प्रति (i = 0; ; ++i)
		{
		    जइ (vim_fgets(IObuff, IOSIZE, sfd))
			भङ्ग;
		    // When a line has been truncated, append NL, taking care
		    // of multi-byte characters .
		    जइ (IObuff[IOSIZE - 2] != NUL && IObuff[IOSIZE - 2] != NL)
		    {
			पूर्णाङ्क n = IOSIZE - 2;

			जइ (enc_utf8)
			{
			    // Move to the first byte of this char.
			    // utf_head_off() doesn't work, because it checks
			    // for a truncated character.
			    जाव (n > 0 && (IObuff[n] & 0xc0) == 0x80)
				--n;
			}
			अण्णहा जइ (has_mbyte)
			    n -= mb_head_off(IObuff, IObuff + n);
			IObuff[n] = NL;
			IObuff[n + 1] = NUL;
		    }
		    जइ (i < si->sn_prl_ga.ga_len
				     && (pp = &PRL_ITEM(si, i))->snp_गणनम् > 0)
		    {
			fprintf(fd, "%5d ", pp->snp_गणनम्);
			जइ (profile_equal(&pp->sn_prl_total, &pp->sn_prl_self))
			    fprintf(fd, "           ");
			अण्णहा
			    fprintf(fd, "%s ", profile_सन्देशः(&pp->sn_prl_total));
			fprintf(fd, "%s ", profile_सन्देशः(&pp->sn_prl_self));
		    }
		    अण्णहा
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
    शून्य
profile_dump(शून्य)
{
    FILE	*fd;

    जइ (profile_fname == रिक्त)
	प्रत्यागम;

    fd = mch_fopen((वर्ण *)profile_fname, "w");
    जइ (fd == रिक्त)
	semsg(_(e_cant_open_file_str), profile_fname);
    अण्णहा
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
    शून्य
script_पङ्क्तिः_start(शून्य)
{
    scriptitem_T    *si;
    sn_prl_T	    *pp;

    जइ (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	प्रत्यागम;
    si = SCRIPT_ITEM(current_sctx.sc_sid);
    जइ (si->sn_prof_on && SOURCING_LNUM >= 1)
    {
	// Grow the array before starting the timer, so that the time spent
	// here isn't counted.
	(शून्य)ga_grow(&si->sn_prl_ga,
				  (पूर्णाङ्क)(SOURCING_LNUM - si->sn_prl_ga.ga_len));
	si->sn_prl_idx = SOURCING_LNUM - 1;
	जाव (si->sn_prl_ga.ga_len <= si->sn_prl_idx
		&& si->sn_prl_ga.ga_len < si->sn_prl_ga.ga_maxlen)
	{
	    // Zero counters for a line that was not used before.
	    pp = &PRL_ITEM(si, si->sn_prl_ga.ga_len);
	    pp->snp_गणनम् = 0;
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
    शून्य
script_पङ्क्तिः_exec(शून्य)
{
    scriptitem_T    *si;

    जइ (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	प्रत्यागम;
    si = SCRIPT_ITEM(current_sctx.sc_sid);
    जइ (si->sn_prof_on && si->sn_prl_idx >= 0)
	si->sn_prl_execed = TRUE;
}

/*
 * Called when done with a script line.
 */
    शून्य
script_पङ्क्तिः_end(शून्य)
{
    scriptitem_T    *si;
    sn_prl_T	    *pp;

    जइ (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	प्रत्यागम;
    si = SCRIPT_ITEM(current_sctx.sc_sid);
    जइ (si->sn_prof_on && si->sn_prl_idx >= 0
				     && si->sn_prl_idx < si->sn_prl_ga.ga_len)
    {
	जइ (si->sn_prl_execed)
	{
	    pp = &PRL_ITEM(si, si->sn_prl_idx);
	    ++pp->snp_गणनम्;
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
