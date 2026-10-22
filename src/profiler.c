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

#અન્તર્ભાવ "vim.h"

#જો defined(FEAT_EVAL)
# જો defined(FEAT_PROFILE) || defined(FEAT_RELTIME)
/*
 * Store the current time in "tm".
 */
    શૂન્ય
profile_start(proftime_T *tm)
{
#  ifdef MSWIN
    QueryPerformanceCounter(tm);
#  નહીંતર
    PROF_GET_TIME(tm);
#  endif
}

/*
 * Compute the elapsed time from "tm" till now and store in "tm".
 */
    શૂન્ય
profile_end(proftime_T *tm)
{
    proftime_T now;

#  ifdef MSWIN
    QueryPerformanceCounter(&now);
    tm->QuadPart = now.QuadPart - tm->QuadPart;
#  નહીંતર
    PROF_GET_TIME(&now);
    tm->tv_fsec = now.tv_fsec - tm->tv_fsec;
    tm->tv_sec = now.tv_sec - tm->tv_sec;
    જો (tm->tv_fsec < 0)
    {
	tm->tv_fsec += TV_FSEC_SEC;
	--tm->tv_sec;
    }
#  endif
}

/*
 * Subtract the time "tm2" from "tm".
 */
    શૂન્ય
profile_sub(proftime_T *tm, proftime_T *tm2)
{
#  ifdef MSWIN
    tm->QuadPart -= tm2->QuadPart;
#  નહીંતર
    tm->tv_fsec -= tm2->tv_fsec;
    tm->tv_sec -= tm2->tv_sec;
    જો (tm->tv_fsec < 0)
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
    અક્ષર *
profile_સંદેશ(proftime_T *tm)
{
    સ્થિર અક્ષર buf[50];

#  ifdef MSWIN
    LARGE_INTEGER   fr;

    QueryPerformanceFrequency(&fr);
    sprintf(buf, "%10.6lf", (દ્વિગુણ)tm->QuadPart / (દ્વિગુણ)fr.QuadPart);
#  નહીંતર
    sprintf(buf, PROF_TIME_FORMAT, (long)tm->tv_sec, (long)tm->tv_fsec);
#  endif
    પાછા buf;
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
    પાછા (float_T)tm->QuadPart / (float_T)fr.QuadPart;
#  નહીંતર
    પાછા (float_T)tm->tv_sec + (float_T)tm->tv_fsec / (float_T)TV_FSEC_SEC;
#  endif
}

/*
 * Put the time "msec" past now in "tm".
 */
    શૂન્ય
profile_setlimit(long msec, proftime_T *tm)
{
    જો (msec <= 0)   // no limit
	profile_zero(tm);
    નહીંતર
    {
#  ifdef MSWIN
	LARGE_INTEGER   fr;

	QueryPerformanceCounter(tm);
	QueryPerformanceFrequency(&fr);
	tm->QuadPart += (LONGLONG)((દ્વિગુણ)msec / 1000.0 * (દ્વિગુણ)fr.QuadPart);
#  નહીંતર
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
    પૂર્ણાંક
profile_passed_limit(proftime_T *tm)
{
    proftime_T	now;

#  ifdef MSWIN
    જો (tm->QuadPart == 0)  // timer was not set
	પાછા FALSE;
    QueryPerformanceCounter(&now);
    પાછા (now.QuadPart > tm->QuadPart);
#  નહીંતર
    જો (tm->tv_sec == 0)    // timer was not set
	પાછા FALSE;
    PROF_GET_TIME(&now);
    પાછા (now.tv_sec > tm->tv_sec
	    || (now.tv_sec == tm->tv_sec && now.tv_fsec > tm->tv_fsec));
#  endif
}

/*
 * Set the time in "tm" to zero.
 */
    શૂન્ય
profile_zero(proftime_T *tm)
{
#  ifdef MSWIN
    tm->QuadPart = 0;
#  નહીંતર
    tm->tv_fsec = 0;
    tm->tv_sec = 0;
#  endif
}

# endif  // FEAT_PROFILE || FEAT_RELTIME

# જો defined(FEAT_SYN_HL) && defined(FEAT_RELTIME) && defined(FEAT_PROFILE)
#  જો defined(HAVE_MATH_H)
#   include <math.h>
#  endif

/*
 * Divide the time "tm" by "count" and store in "tm2".
 */
    શૂન્ય
profile_divide(proftime_T *tm, પૂર્ણાંક ગણતરી, proftime_T *tm2)
{
    જો (ગણતરી == 0)
	profile_zero(tm2);
    નહીંતર
    {
#  ifdef MSWIN
	tm2->QuadPart = tm->QuadPart / ગણતરી;
#  નહીંતર
	દ્વિગુણ fsec = (tm->tv_sec * (float_T)TV_FSEC_SEC + tm->tv_fsec)
								       / ગણતરી;

	tm2->tv_sec = floor(fsec / (float_T)TV_FSEC_SEC);
	tm2->tv_fsec = vim_round(fsec - (tm2->tv_sec * (float_T)TV_FSEC_SEC));
#  endif
    }
}
# endif

# જો defined(FEAT_PROFILE)
/*
 * Functions for profiling.
 */
સ્થિર proftime_T prof_wait_time;

/*
 * Add the time "tm2" to "tm".
 */
    શૂન્ય
profile_add(proftime_T *tm, proftime_T *tm2)
{
#  ifdef MSWIN
    tm->QuadPart += tm2->QuadPart;
#  નહીંતર
    tm->tv_fsec += tm2->tv_fsec;
    tm->tv_sec += tm2->tv_sec;
    જો (tm->tv_fsec >= TV_FSEC_SEC)
    {
	tm->tv_fsec -= TV_FSEC_SEC;
	++tm->tv_sec;
    }
#  endif
}

/*
 * Add the "self" time from the total time and the children's time.
 */
    શૂન્ય
profile_self(proftime_T *self, proftime_T *total, proftime_T *children)
{
    // Check that the result won't be negative.  Can happen with recursive
    // calls.
#  ifdef MSWIN
    જો (total->QuadPart <= children->QuadPart)
	પાછા;
#  નહીંતર
    જો (total->tv_sec < children->tv_sec
	    || (total->tv_sec == children->tv_sec
		&& total->tv_fsec <= children->tv_fsec))
	પાછા;
#  endif
    profile_add(self, total);
    profile_sub(self, children);
}

/*
 * Get the current waittime.
 */
    સ્થિર શૂન્ય
profile_get_wait(proftime_T *tm)
{
    *tm = prof_wait_time;
}

/*
 * Subtract the passed waittime since "tm" from "tma".
 */
    શૂન્ય
profile_sub_wait(proftime_T *tm, proftime_T *tma)
{
    proftime_T tm3 = prof_wait_time;

    profile_sub(&tm3, tm);
    profile_sub(tma, &tm3);
}

/*
 * Return TRUE if "tm1" and "tm2" are equal.
 */
    સ્થિર પૂર્ણાંક
profile_equal(proftime_T *tm1, proftime_T *tm2)
{
#  ifdef MSWIN
    પાછા (tm1->QuadPart == tm2->QuadPart);
#  નહીંતર
    પાછા (tm1->tv_fsec == tm2->tv_fsec && tm1->tv_sec == tm2->tv_sec);
#  endif
}

/*
 * Return <0, 0 or >0 if "tm1" < "tm2", "tm1" == "tm2" or "tm1" > "tm2"
 */
    પૂર્ણાંક
profile_cmp(અચલ proftime_T *tm1, અચલ proftime_T *tm2)
{
#  ifdef MSWIN
    પાછા tm2->QuadPart == tm1->QuadPart ? 0 :
	tm2->QuadPart > tm1->QuadPart ? 1 : -1;
#  નહીંતર
    જો (tm1->tv_sec == tm2->tv_sec)
	પાછા tm2->tv_fsec == tm1->tv_fsec ? 0 :
	    tm2->tv_fsec > tm1->tv_fsec ? 1 : -1;
    પાછા tm2->tv_sec > tm1->tv_sec ? 1 : -1;
#  endif
}

સ્થિર char_u	*profile_fname = ખાલી;
સ્થિર proftime_T pause_time;

/*
 * Reset all profiling information.
 */
    સ્થિર શૂન્ય
profile_reset(શૂન્ય)
{
    પૂર્ણાંક		id;
    પૂર્ણાંક		todo;
    hashtab_T	*functbl;
    hashitem_T	*hi;

    // Reset sourced files.
    માટે (id = 1; id <= script_વસ્તુઓ.ga_len; id++)
    {
	scriptitem_T *si = SCRIPT_ITEM(id);
	જો (si->sn_prof_on)
	{
	    si->sn_prof_on      = FALSE;
	    si->sn_pr_force     = FALSE;
	    profile_zero(&si->sn_pr_child);
	    si->sn_pr_nest      = 0;
	    si->sn_pr_ગણતરી     = 0;
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
    todo = (પૂર્ણાંક)functbl->ht_used;

    FOR_ALL_HASHTAB_વસ્તુઓ(functbl, hi, todo)
    {
	ufunc_T *fp;
	પૂર્ણાંક	i;

	જો (HASHITEM_EMPTY(hi))
	    ચાલુ;

	todo--;
	fp = HI2UF(hi);
	જો (fp->uf_prof_initialized)
	{
	    fp->uf_profiling    = 0;
	    fp->uf_prof_initialized = FALSE;
	    fp->uf_tm_ગણતરી     = 0;
	    profile_zero(&fp->uf_tm_total);
	    profile_zero(&fp->uf_tm_self);
	    profile_zero(&fp->uf_tm_children);

	    માટે (i = 0; i < fp->uf_લીટીઓ.ga_len; i++)
	    {
		fp->uf_tml_ગણતરી[i] = 0;
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
    શૂન્ય
ex_profile(exarg_T *eap)
{
    char_u	*e;
    પૂર્ણાંક		len;

    e = skiptowhite(eap->arg);
    len = (પૂર્ણાંક)(e - eap->arg);
    e = skipwhite(e);

    જો (len == 5 && STRNCMP(eap->arg, "start", 5) == 0 && *e != NUL)
    {
	VIM_CLEAR(profile_fname);
	profile_fname = expand_env_save_opt(e, TRUE, ખાલી);
	do_profiling = PROF_YES;
	profile_zero(&prof_wait_time);
	set_vim_var_nr(VV_PROFILING, 1L);
    }
    નહીંતર જો (do_profiling == PROF_NONE)
	emsg(_(e_first_use_profile_start_fname));
    નહીંતર જો (STRCMP(eap->arg, "stop") == 0)
    {
	profile_dump();
	do_profiling = PROF_NONE;
	set_vim_var_nr(VV_PROFILING, 0L);
	profile_reset();
    }
    નહીંતર જો (STRCMP(eap->arg, "pause") == 0)
    {
	જો (do_profiling == PROF_YES)
	    profile_start(&pause_time);
	do_profiling = PROF_PAUSED;
    }
    નહીંતર જો (STRCMP(eap->arg, "continue") == 0)
    {
	જો (do_profiling == PROF_PAUSED)
	{
	    profile_end(&pause_time);
	    profile_add(&prof_wait_time, &pause_time);
	}
	do_profiling = PROF_YES;
    }
    નહીંતર જો (STRCMP(eap->arg, "dump") == 0)
	profile_dump();
    નહીંતર
    {
	// The rest is similar to ":breakadd".
	ex_breakadd(eap);
    }
}

// Command line expansion for :profile.
સ્થિર ગણના
{
    PEXP_SUBCMD,	// expand :profile sub-commands
    PEXP_FUNC		// expand :profile func {funcname}
} pexpand_what;

સ્થિર અક્ષર *pexpand_cmds[] = {
			"start",
			"stop",
			"pause",
			"continue",
			"func",
			"file",
			"dump",
			ખાલી
};

/*
 * Function given to ExpandGeneric() to obtain the profile command
 * specific expansion.
 */
    char_u *
get_profile_name(expand_T *xp UNUSED, પૂર્ણાંક idx)
{
    પસંદગી (pexpand_what)
    {
    સ્થિતિ PEXP_SUBCMD:
	પાછા (char_u *)pexpand_cmds[idx];
    મૂળભૂત:
	પાછા ખાલી;
    }
}

/*
 * Handle command line completion for :profile command.
 */
    શૂન્ય
set_context_in_profile_cmd(expand_T *xp, char_u *arg)
{
    char_u	*end_subcmd;

    // Default: expand subcommands.
    xp->xp_context = EXPAND_PROFILE;
    pexpand_what = PEXP_SUBCMD;
    xp->xp_pattern = arg;

    end_subcmd = skiptowhite(arg);
    જો (*end_subcmd == NUL)
	પાછા;

    જો ((end_subcmd - arg == 5 && STRNCMP(arg, "start", 5) == 0)
	    || (end_subcmd - arg == 4 && STRNCMP(arg, "file", 4) == 0))
    {
	xp->xp_context = EXPAND_FILES;
	xp->xp_pattern = skipwhite(end_subcmd);
	પાછા;
    }
    નહીંતર જો (end_subcmd - arg == 4 && STRNCMP(arg, "func", 4) == 0)
    {
	xp->xp_context = EXPAND_USER_FUNC;
	xp->xp_pattern = skipwhite(end_subcmd);
	પાછા;
    }

    xp->xp_context = EXPAND_NOTHING;
}

સ્થિર proftime_T inchar_time;

/*
 * Called when starting to wait for the user to type a character.
 */
    શૂન્ય
prof_inchar_enter(શૂન્ય)
{
    profile_start(&inchar_time);
}

/*
 * Called when finished waiting for the user to type a character.
 */
    શૂન્ય
prof_inchar_exit(શૂન્ય)
{
    profile_end(&inchar_time);
    profile_add(&prof_wait_time, &inchar_time);
}


/*
 * Return TRUE when a function defined in the current script should be
 * profiled.
 */
    પૂર્ણાંક
prof_def_func(શૂન્ય)
{
    જો (current_sctx.sc_sid > 0)
	પાછા SCRIPT_ITEM(current_sctx.sc_sid)->sn_pr_force;
    પાછા FALSE;
}

/*
 * Print the count and times for one function or function line.
 */
    સ્થિર શૂન્ય
prof_func_લીટી(
    FILE	*fd,
    પૂર્ણાંક		ગણતરી,
    proftime_T	*total,
    proftime_T	*self,
    પૂર્ણાંક		prefer_self)	// when equal print only self time
{
    જો (ગણતરી > 0)
    {
	fprintf(fd, "%5d ", ગણતરી);
	જો (prefer_self && profile_equal(total, self))
	    fprintf(fd, PROF_TIME_BLANK);
	નહીંતર
	    fprintf(fd, "%s ", profile_સંદેશ(total));
	જો (!prefer_self && profile_equal(total, self))
	    fprintf(fd, PROF_TIME_BLANK);
	નહીંતર
	    fprintf(fd, "%s ", profile_સંદેશ(self));
    }
    નહીંતર
	fprintf(fd, "      %s%s", PROF_TIME_BLANK, PROF_TIME_BLANK);
}

    સ્થિર શૂન્ય
prof_sort_list(
    FILE	*fd,
    ufunc_T	**sorttab,
    પૂર્ણાંક		st_len,
    અક્ષર	*title,
    પૂર્ણાંક		prefer_self)	// when equal print only self time
{
    પૂર્ણાંક		i;
    ufunc_T	*fp;

    fprintf(fd, "FUNCTIONS SORTED ON %s TIME\n", title);
    fprintf(fd, "%s  function\n", PROF_TOTALS_HEADER);
    માટે (i = 0; i < 20 && i < st_len; ++i)
    {
	fp = sorttab[i];
	prof_func_લીટી(fd, fp->uf_tm_ગણતરી, &fp->uf_tm_total, &fp->uf_tm_self,
								 prefer_self);
	જો (fp->uf_name[0] == K_SPECIAL)
	    fprintf(fd, " <SNR>%s()\n", fp->uf_name + 3);
	નહીંતર
	    fprintf(fd, " %s()\n", fp->uf_name);
    }
    fprintf(fd, "\n");
}

/*
 * Compare function for total time sorting.
 */
    સ્થિર પૂર્ણાંક
prof_total_cmp(અચલ શૂન્ય *s1, અચલ શૂન્ય *s2)
{
    ufunc_T	*p1, *p2;

    p1 = *(ufunc_T **)s1;
    p2 = *(ufunc_T **)s2;
    પાછા profile_cmp(&p1->uf_tm_total, &p2->uf_tm_total);
}

/*
 * Compare function for self time sorting.
 */
    સ્થિર પૂર્ણાંક
prof_self_cmp(અચલ શૂન્ય *s1, અચલ શૂન્ય *s2)
{
    ufunc_T	*p1, *p2;

    p1 = *(ufunc_T **)s1;
    p2 = *(ufunc_T **)s2;
    પાછા profile_cmp(&p1->uf_tm_self, &p2->uf_tm_self);
}

/*
 * Start profiling function "fp".
 */
    શૂન્ય
func_do_profile(ufunc_T *fp)
{
    પૂર્ણાંક		len = fp->uf_લીટીઓ.ga_len;

    જો (!fp->uf_prof_initialized)
    {
	જો (len == 0)
	    len = 1;  // avoid getting error for allocating zero bytes
	fp->uf_tm_ગણતરી = 0;
	profile_zero(&fp->uf_tm_self);
	profile_zero(&fp->uf_tm_total);
	જો (fp->uf_tml_ગણતરી == ખાલી)
	    fp->uf_tml_ગણતરી = ALLOC_CLEAR_MULT(પૂર્ણાંક, len);
	જો (fp->uf_tml_total == ખાલી)
	    fp->uf_tml_total = ALLOC_CLEAR_MULT(proftime_T, len);
	જો (fp->uf_tml_self == ખાલી)
	    fp->uf_tml_self = ALLOC_CLEAR_MULT(proftime_T, len);
	fp->uf_tml_idx = -1;
	જો (fp->uf_tml_ગણતરી == ખાલી || fp->uf_tml_total == ખાલી
						    || fp->uf_tml_self == ખાલી)
	    પાછા;	    // out of memory
	fp->uf_prof_initialized = TRUE;
    }

    fp->uf_profiling = TRUE;
}

/*
 * Save time when starting to invoke another script or function.
 */
    સ્થિર શૂન્ય
script_prof_save(
    proftime_T	*tm)	    // place to store wait time
{
    scriptitem_T    *si;

    જો (SCRIPT_ID_VALID(current_sctx.sc_sid))
    {
	si = SCRIPT_ITEM(current_sctx.sc_sid);
	જો (si->sn_prof_on && si->sn_pr_nest++ == 0)
	    profile_start(&si->sn_pr_child);
    }
    profile_get_wait(tm);
}

/*
 * When calling a function: may initialize for profiling.
 */
    શૂન્ય
profile_may_start_func(profinfo_T *info, ufunc_T *fp, ufunc_T *caller)
{
    જો (!fp->uf_profiling && has_profiling(FALSE, fp->uf_name, ખાલી,
								&fp->uf_hash))
    {
	info->pi_started_profiling = TRUE;
	func_do_profile(fp);
    }
    જો (fp->uf_profiling || (caller != ખાલી && caller->uf_profiling))
    {
	++fp->uf_tm_ગણતરી;
	profile_start(&info->pi_call_start);
	profile_zero(&fp->uf_tm_children);
    }
    script_prof_save(&info->pi_wait_start);
}

/*
 * After calling a function: may handle profiling.  profile_may_start_func()
 * must have been called previously.
 */
    શૂન્ય
profile_may_end_func(profinfo_T *info, ufunc_T *fp, ufunc_T *caller)
{
    profile_end(&info->pi_call_start);
    profile_sub_wait(&info->pi_wait_start, &info->pi_call_start);
    profile_add(&fp->uf_tm_total, &info->pi_call_start);
    profile_self(&fp->uf_tm_self, &info->pi_call_start, &fp->uf_tm_children);
    જો (caller != ખાલી && caller->uf_profiling)
    {
	profile_add(&caller->uf_tm_children, &info->pi_call_start);
	profile_add(&caller->uf_tml_children, &info->pi_call_start);
    }
    જો (info->pi_started_profiling)
	// make a ":profdel func" stop profiling the function
	fp->uf_profiling = FALSE;
}

/*
 * Prepare profiling for entering a child or something else that is not
 * counted for the script/function itself.
 * Should always be called in pair with prof_child_exit().
 */
    શૂન્ય
prof_child_enter(
    proftime_T *tm)	// place to store waittime
{
    funccall_T *fc = get_current_funccal();

    જો (fc != ખાલી && fc->fc_func->uf_profiling)
	profile_start(&fc->fc_prof_child);
    script_prof_save(tm);
}

/*
 * Take care of time spent in a child.
 * Should always be called after prof_child_enter().
 */
    શૂન્ય
prof_child_exit(
    proftime_T *tm)	// where waittime was stored
{
    funccall_T *fc = get_current_funccal();

    જો (fc != ખાલી && fc->fc_func->uf_profiling)
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
    શૂન્ય
func_લીટી_start(શૂન્ય *cookie, long lnum)
{
    funccall_T	*fcp = (funccall_T *)cookie;
    ufunc_T	*fp = fcp->fc_func;

    જો (fp->uf_profiling && lnum >= 1 && lnum <= fp->uf_લીટીઓ.ga_len)
    {
	fp->uf_tml_idx = lnum - 1;
	// Skip continuation lines.
	જ્યારે (fp->uf_tml_idx > 0 && FUNCLINE(fp, fp->uf_tml_idx) == ખાલી)
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
    શૂન્ય
func_લીટી_exec(શૂન્ય *cookie)
{
    funccall_T	*fcp = (funccall_T *)cookie;
    ufunc_T	*fp = fcp->fc_func;

    જો (fp->uf_profiling && fp->uf_tml_idx >= 0)
	fp->uf_tml_execed = TRUE;
}

/*
 * Called when done with a function line.
 */
    શૂન્ય
func_લીટી_end(શૂન્ય *cookie)
{
    funccall_T	*fcp = (funccall_T *)cookie;
    ufunc_T	*fp = fcp->fc_func;

    જો (fp->uf_profiling && fp->uf_tml_idx >= 0)
    {
	જો (fp->uf_tml_execed)
	{
	    ++fp->uf_tml_ગણતરી[fp->uf_tml_idx];
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
    સ્થિર શૂન્ય
func_dump_profile(FILE *fd)
{
    hashtab_T	*functbl;
    hashitem_T	*hi;
    પૂર્ણાંક		todo;
    ufunc_T	*fp;
    પૂર્ણાંક		i;
    ufunc_T	**sorttab;
    પૂર્ણાંક		st_len = 0;
    char_u	*p;

    functbl = func_tbl_get();
    todo = (પૂર્ણાંક)functbl->ht_used;
    જો (todo == 0)
	પાછા;     // nothing to dump

    sorttab = ALLOC_MULT(ufunc_T *, todo);

    FOR_ALL_HASHTAB_વસ્તુઓ(functbl, hi, todo)
    {
	જો (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    fp = HI2UF(hi);
	    જો (fp->uf_prof_initialized)
	    {
		જો (sorttab != ખાલી)
		    sorttab[st_len++] = fp;

		જો (fp->uf_name[0] == K_SPECIAL)
		    fprintf(fd, "FUNCTION  <SNR>%s()\n", fp->uf_name + 3);
		નહીંતર
		    fprintf(fd, "FUNCTION  %s()\n", fp->uf_name);
		જો (fp->uf_script_ctx.sc_sid > 0)
		{
		    p = home_replace_save(ખાલી,
				     get_scriptname(fp->uf_script_ctx.sc_sid));
		    જો (p != ખાલી)
		    {
			fprintf(fd, "    Defined: %s:%ld\n",
					   p, (long)fp->uf_script_ctx.sc_lnum);
			vim_free(p);
		    }
		}
		જો (fp->uf_tm_ગણતરી == 1)
		    fprintf(fd, "Called 1 time\n");
		નહીંતર
		    fprintf(fd, "Called %d times\n", fp->uf_tm_ગણતરી);
		fprintf(fd, "Total time: %s\n", profile_સંદેશ(&fp->uf_tm_total));
		fprintf(fd, " Self time: %s\n", profile_સંદેશ(&fp->uf_tm_self));
		fprintf(fd, "\n");
		fprintf(fd, "%s\n", PROF_TOTALS_HEADER);

		માટે (i = 0; i < fp->uf_લીટીઓ.ga_len; ++i)
		{
		    જો (FUNCLINE(fp, i) == ખાલી)
			ચાલુ;
		    prof_func_લીટી(fd, fp->uf_tml_ગણતરી[i],
			     &fp->uf_tml_total[i], &fp->uf_tml_self[i], TRUE);
		    fprintf(fd, "%s\n", FUNCLINE(fp, i));
		}
		fprintf(fd, "\n");
	    }
	}
    }

    જો (sorttab != ખાલી && st_len > 0)
    {
	qsort((શૂન્ય *)sorttab, (size_t)st_len, કદ(ufunc_T *),
							      prof_total_cmp);
	prof_sort_list(fd, sorttab, st_len, "TOTAL", FALSE);
	qsort((શૂન્ય *)sorttab, (size_t)st_len, કદ(ufunc_T *),
							      prof_self_cmp);
	prof_sort_list(fd, sorttab, st_len, "SELF", TRUE);
    }

    vim_free(sorttab);
}

/*
 * Start profiling script "fp".
 */
    શૂન્ય
script_do_profile(scriptitem_T *si)
{
    si->sn_pr_ગણતરી = 0;
    profile_zero(&si->sn_pr_total);
    profile_zero(&si->sn_pr_self);

    ga_init2(&si->sn_prl_ga, કદ(sn_prl_T), 100);
    si->sn_prl_idx = -1;
    si->sn_prof_on = TRUE;
    si->sn_pr_nest = 0;
}

/*
 * Count time spent in children after invoking another script or function.
 */
    શૂન્ય
script_prof_restore(proftime_T *tm)
{
    scriptitem_T    *si;

    જો (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	પાછા;

    si = SCRIPT_ITEM(current_sctx.sc_sid);
    જો (si->sn_prof_on && --si->sn_pr_nest == 0)
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
    સ્થિર શૂન્ય
script_dump_profile(FILE *fd)
{
    પૂર્ણાંક		    id;
    scriptitem_T    *si;
    પૂર્ણાંક		    i;
    FILE	    *sfd;
    sn_prl_T	    *pp;

    માટે (id = 1; id <= script_વસ્તુઓ.ga_len; ++id)
    {
	si = SCRIPT_ITEM(id);
	જો (si->sn_prof_on)
	{
	    fprintf(fd, "SCRIPT  %s\n", si->sn_name);
	    જો (si->sn_pr_ગણતરી == 1)
		fprintf(fd, "Sourced 1 time\n");
	    નહીંતર
		fprintf(fd, "Sourced %d times\n", si->sn_pr_ગણતરી);
	    fprintf(fd, "Total time: %s\n", profile_સંદેશ(&si->sn_pr_total));
	    fprintf(fd, " Self time: %s\n", profile_સંદેશ(&si->sn_pr_self));
	    fprintf(fd, "\n");
	    fprintf(fd, "%s\n", PROF_TOTALS_HEADER);

	    sfd = mch_fopen((અક્ષર *)si->sn_name, "r");
	    જો (sfd == ખાલી)
		fprintf(fd, "Cannot open file!\n");
	    નહીંતર
	    {
		// Keep going till the end of file, so that trailing
		// continuation lines are listed.
		માટે (i = 0; ; ++i)
		{
		    જો (vim_fgets(IObuff, IOSIZE, sfd))
			તોડો;
		    // When a line has been truncated, append NL, taking care
		    // of multi-byte characters .
		    જો (IObuff[IOSIZE - 2] != NUL && IObuff[IOSIZE - 2] != NL)
		    {
			પૂર્ણાંક n = IOSIZE - 2;

			જો (enc_utf8)
			{
			    // Move to the first byte of this char.
			    // utf_head_off() doesn't work, because it checks
			    // for a truncated character.
			    જ્યારે (n > 0 && (IObuff[n] & 0xc0) == 0x80)
				--n;
			}
			નહીંતર જો (has_mbyte)
			    n -= mb_head_off(IObuff, IObuff + n);
			IObuff[n] = NL;
			IObuff[n + 1] = NUL;
		    }
		    જો (i < si->sn_prl_ga.ga_len
				     && (pp = &PRL_ITEM(si, i))->snp_ગણતરી > 0)
		    {
			fprintf(fd, "%5d ", pp->snp_ગણતરી);
			જો (profile_equal(&pp->sn_prl_total, &pp->sn_prl_self))
			    fprintf(fd, "           ");
			નહીંતર
			    fprintf(fd, "%s ", profile_સંદેશ(&pp->sn_prl_total));
			fprintf(fd, "%s ", profile_સંદેશ(&pp->sn_prl_self));
		    }
		    નહીંતર
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
    શૂન્ય
profile_dump(શૂન્ય)
{
    FILE	*fd;

    જો (profile_fname == ખાલી)
	પાછા;

    fd = mch_fopen((અક્ષર *)profile_fname, "w");
    જો (fd == ખાલી)
	semsg(_(e_cant_open_file_str), profile_fname);
    નહીંતર
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
    શૂન્ય
script_લીટી_start(શૂન્ય)
{
    scriptitem_T    *si;
    sn_prl_T	    *pp;

    જો (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	પાછા;
    si = SCRIPT_ITEM(current_sctx.sc_sid);
    જો (si->sn_prof_on && SOURCING_LNUM >= 1)
    {
	// Grow the array before starting the timer, so that the time spent
	// here isn't counted.
	(શૂન્ય)ga_grow(&si->sn_prl_ga,
				  (પૂર્ણાંક)(SOURCING_LNUM - si->sn_prl_ga.ga_len));
	si->sn_prl_idx = SOURCING_LNUM - 1;
	જ્યારે (si->sn_prl_ga.ga_len <= si->sn_prl_idx
		&& si->sn_prl_ga.ga_len < si->sn_prl_ga.ga_maxlen)
	{
	    // Zero counters for a line that was not used before.
	    pp = &PRL_ITEM(si, si->sn_prl_ga.ga_len);
	    pp->snp_ગણતરી = 0;
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
    શૂન્ય
script_લીટી_exec(શૂન્ય)
{
    scriptitem_T    *si;

    જો (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	પાછા;
    si = SCRIPT_ITEM(current_sctx.sc_sid);
    જો (si->sn_prof_on && si->sn_prl_idx >= 0)
	si->sn_prl_execed = TRUE;
}

/*
 * Called when done with a script line.
 */
    શૂન્ય
script_લીટી_end(શૂન્ય)
{
    scriptitem_T    *si;
    sn_prl_T	    *pp;

    જો (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	પાછા;
    si = SCRIPT_ITEM(current_sctx.sc_sid);
    જો (si->sn_prof_on && si->sn_prl_idx >= 0
				     && si->sn_prl_idx < si->sn_prl_ga.ga_len)
    {
	જો (si->sn_prl_execed)
	{
	    pp = &PRL_ITEM(si, si->sn_prl_idx);
	    ++pp->snp_ગણતરી;
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
