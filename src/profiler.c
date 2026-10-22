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

#అన్తర్భావ "vim.h"

#ఒకవేళ defined(FEAT_EVAL)
# ఒకవేళ defined(FEAT_PROFILE) || defined(FEAT_RELTIME)
/*
 * Store the current time in "tm".
 */
    శూన్య
profile_ప్రారంభం(proftime_T *tm)
{
#  ifdef MSWIN
    QueryPerformanceCounter(tm);
#  లేకపోతే
    PROF_GET_సమయం(tm);
#  endif
}

/*
 * Compute the elapsed time from "tm" till now and store in "tm".
 */
    శూన్య
profile_చివర(proftime_T *tm)
{
    proftime_T now;

#  ifdef MSWIN
    QueryPerformanceCounter(&now);
    tm->QuadPart = now.QuadPart - tm->QuadPart;
#  లేకపోతే
    PROF_GET_సమయం(&now);
    tm->tv_fsec = now.tv_fsec - tm->tv_fsec;
    tm->tv_sec = now.tv_sec - tm->tv_sec;
    ఒకవేళ (tm->tv_fsec < 0)
    {
	tm->tv_fsec += TV_FSEC_SEC;
	--tm->tv_sec;
    }
#  endif
}

/*
 * Subtract the time "tm2" from "tm".
 */
    శూన్య
profile_sub(proftime_T *tm, proftime_T *tm2)
{
#  ifdef MSWIN
    tm->QuadPart -= tm2->QuadPart;
#  లేకపోతే
    tm->tv_fsec -= tm2->tv_fsec;
    tm->tv_sec -= tm2->tv_sec;
    ఒకవేళ (tm->tv_fsec < 0)
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
    అక్షరం *
profile_సందేశం(proftime_T *tm)
{
    స్థిర అక్షరం buf[50];

#  ifdef MSWIN
    LARGE_INTEGER   fr;

    QueryPerformanceFrequency(&fr);
    sprintf(buf, "%10.6lf", (ద్విగుణ)tm->QuadPart / (ద్విగుణ)fr.QuadPart);
#  లేకపోతే
    sprintf(buf, PROF_సమయం_FORMAT, (దీర్ఘ)tm->tv_sec, (దీర్ఘ)tm->tv_fsec);
#  endif
    తిరిగి buf;
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
    తిరిగి (float_T)tm->QuadPart / (float_T)fr.QuadPart;
#  లేకపోతే
    తిరిగి (float_T)tm->tv_sec + (float_T)tm->tv_fsec / (float_T)TV_FSEC_SEC;
#  endif
}

/*
 * Put the time "msec" past now in "tm".
 */
    శూన్య
profile_setlimit(దీర్ఘ msec, proftime_T *tm)
{
    ఒకవేళ (msec <= 0)   // no limit
	profile_zero(tm);
    లేకపోతే
    {
#  ifdef MSWIN
	LARGE_INTEGER   fr;

	QueryPerformanceCounter(tm);
	QueryPerformanceFrequency(&fr);
	tm->QuadPart += (LONGLONG)((ద్విగుణ)msec / 1000.0 * (ద్విగుణ)fr.QuadPart);
#  లేకపోతే
	varnumber_T	    fsec;	// this should be 64 bit if possible

	PROF_GET_సమయం(tm);
	fsec = (varnumber_T)tm->tv_fsec
		       + (varnumber_T)msec * (varnumber_T)(TV_FSEC_SEC / 1000);
	tm->tv_fsec = fsec % (దీర్ఘ)TV_FSEC_SEC;
	tm->tv_sec += fsec / (దీర్ఘ)TV_FSEC_SEC;
#  endif
    }
}

/*
 * Return TRUE if the current time is past "tm".
 */
    పూర్ణాంకం
profile_passed_హద్దు(proftime_T *tm)
{
    proftime_T	now;

#  ifdef MSWIN
    ఒకవేళ (tm->QuadPart == 0)  // timer was not set
	తిరిగి FALSE;
    QueryPerformanceCounter(&now);
    తిరిగి (now.QuadPart > tm->QuadPart);
#  లేకపోతే
    ఒకవేళ (tm->tv_sec == 0)    // timer was not set
	తిరిగి FALSE;
    PROF_GET_సమయం(&now);
    తిరిగి (now.tv_sec > tm->tv_sec
	    || (now.tv_sec == tm->tv_sec && now.tv_fsec > tm->tv_fsec));
#  endif
}

/*
 * Set the time in "tm" to zero.
 */
    శూన్య
profile_zero(proftime_T *tm)
{
#  ifdef MSWIN
    tm->QuadPart = 0;
#  లేకపోతే
    tm->tv_fsec = 0;
    tm->tv_sec = 0;
#  endif
}

# endif  // FEAT_PROFILE || FEAT_RELTIME

# ఒకవేళ defined(FEAT_SYN_HL) && defined(FEAT_RELTIME) && defined(FEAT_PROFILE)
#  ఒకవేళ defined(HAVE_MATH_H)
#   include <math.h>
#  endif

/*
 * Divide the time "tm" by "count" and store in "tm2".
 */
    శూన్య
profile_divide(proftime_T *tm, పూర్ణాంకం లెక్క, proftime_T *tm2)
{
    ఒకవేళ (లెక్క == 0)
	profile_zero(tm2);
    లేకపోతే
    {
#  ifdef MSWIN
	tm2->QuadPart = tm->QuadPart / లెక్క;
#  లేకపోతే
	ద్విగుణ fsec = (tm->tv_sec * (float_T)TV_FSEC_SEC + tm->tv_fsec)
								       / లెక్క;

	tm2->tv_sec = floor(fsec / (float_T)TV_FSEC_SEC);
	tm2->tv_fsec = vim_విడత(fsec - (tm2->tv_sec * (float_T)TV_FSEC_SEC));
#  endif
    }
}
# endif

# ఒకవేళ defined(FEAT_PROFILE)
/*
 * Functions for profiling.
 */
స్థిర proftime_T prof_wait_సమయం;

/*
 * Add the time "tm2" to "tm".
 */
    శూన్య
profile_కూడు(proftime_T *tm, proftime_T *tm2)
{
#  ifdef MSWIN
    tm->QuadPart += tm2->QuadPart;
#  లేకపోతే
    tm->tv_fsec += tm2->tv_fsec;
    tm->tv_sec += tm2->tv_sec;
    ఒకవేళ (tm->tv_fsec >= TV_FSEC_SEC)
    {
	tm->tv_fsec -= TV_FSEC_SEC;
	++tm->tv_sec;
    }
#  endif
}

/*
 * Add the "self" time from the total time and the children's time.
 */
    శూన్య
profile_self(proftime_T *self, proftime_T *మొత్తం, proftime_T *children)
{
    // Check that the result won't be negative.  Can happen with recursive
    // calls.
#  ifdef MSWIN
    ఒకవేళ (మొత్తం->QuadPart <= children->QuadPart)
	తిరిగి;
#  లేకపోతే
    ఒకవేళ (మొత్తం->tv_sec < children->tv_sec
	    || (మొత్తం->tv_sec == children->tv_sec
		&& మొత్తం->tv_fsec <= children->tv_fsec))
	తిరిగి;
#  endif
    profile_కూడు(self, మొత్తం);
    profile_sub(self, children);
}

/*
 * Get the current waittime.
 */
    స్థిర శూన్య
profile_get_wait(proftime_T *tm)
{
    *tm = prof_wait_సమయం;
}

/*
 * Subtract the passed waittime since "tm" from "tma".
 */
    శూన్య
profile_sub_wait(proftime_T *tm, proftime_T *tma)
{
    proftime_T tm3 = prof_wait_సమయం;

    profile_sub(&tm3, tm);
    profile_sub(tma, &tm3);
}

/*
 * Return TRUE if "tm1" and "tm2" are equal.
 */
    స్థిర పూర్ణాంకం
profile_equal(proftime_T *tm1, proftime_T *tm2)
{
#  ifdef MSWIN
    తిరిగి (tm1->QuadPart == tm2->QuadPart);
#  లేకపోతే
    తిరిగి (tm1->tv_fsec == tm2->tv_fsec && tm1->tv_sec == tm2->tv_sec);
#  endif
}

/*
 * Return <0, 0 or >0 if "tm1" < "tm2", "tm1" == "tm2" or "tm1" > "tm2"
 */
    పూర్ణాంకం
profile_cmp(స్థిరరాశి proftime_T *tm1, స్థిరరాశి proftime_T *tm2)
{
#  ifdef MSWIN
    తిరిగి tm2->QuadPart == tm1->QuadPart ? 0 :
	tm2->QuadPart > tm1->QuadPart ? 1 : -1;
#  లేకపోతే
    ఒకవేళ (tm1->tv_sec == tm2->tv_sec)
	తిరిగి tm2->tv_fsec == tm1->tv_fsec ? 0 :
	    tm2->tv_fsec > tm1->tv_fsec ? 1 : -1;
    తిరిగి tm2->tv_sec > tm1->tv_sec ? 1 : -1;
#  endif
}

స్థిర char_u	*profile_fname = ఖాళీ;
స్థిర proftime_T pause_సమయం;

/*
 * Reset all profiling information.
 */
    స్థిర శూన్య
profile_reset(శూన్య)
{
    పూర్ణాంకం		id;
    పూర్ణాంకం		todo;
    hashtab_T	*functbl;
    hashitem_T	*hi;

    // Reset sourced files.
    కొరకు (id = 1; id <= script_వస్తువులు.ga_len; id++)
    {
	scriptitem_T *si = SCRIPT_అంశం(id);
	ఒకవేళ (si->sn_prof_on)
	{
	    si->sn_prof_on      = FALSE;
	    si->sn_pr_force     = FALSE;
	    profile_zero(&si->sn_pr_బిడ్డ);
	    si->sn_pr_nest      = 0;
	    si->sn_pr_లెక్క     = 0;
	    profile_zero(&si->sn_pr_మొత్తం);
	    profile_zero(&si->sn_pr_self);
	    profile_zero(&si->sn_pr_ప్రారంభం);
	    profile_zero(&si->sn_pr_children);
	    ga_clear(&si->sn_prl_ga);
	    profile_zero(&si->sn_prl_ప్రారంభం);
	    profile_zero(&si->sn_prl_children);
	    profile_zero(&si->sn_prl_wait);
	    si->sn_prl_idx      = -1;
	    si->sn_prl_execed   = 0;
	}
    }

    // Reset functions.
    functbl = func_tbl_get();
    todo = (పూర్ణాంకం)functbl->ht_వాడిన;

    FOR_ALL_HASHTAB_వస్తువులు(functbl, hi, todo)
    {
	ufunc_T *fp;
	పూర్ణాంకం	i;

	ఒకవేళ (HASHITEM_ఖాళీలు(hi))
	    కొనసాగు;

	todo--;
	fp = HI2UF(hi);
	ఒకవేళ (fp->uf_prof_initialized)
	{
	    fp->uf_profiling    = 0;
	    fp->uf_prof_initialized = FALSE;
	    fp->uf_tm_లెక్క     = 0;
	    profile_zero(&fp->uf_tm_మొత్తం);
	    profile_zero(&fp->uf_tm_self);
	    profile_zero(&fp->uf_tm_children);

	    కొరకు (i = 0; i < fp->uf_పంక్తులు.ga_len; i++)
	    {
		fp->uf_tml_లెక్క[i] = 0;
		profile_zero(&fp->uf_tml_మొత్తం[i]);
		profile_zero(&fp->uf_tml_self[i]);
	    }

	    profile_zero(&fp->uf_tml_ప్రారంభం);
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
    శూన్య
ex_profile(exarg_T *eap)
{
    char_u	*e;
    పూర్ణాంకం		len;

    e = skiptowhite(eap->arg);
    len = (పూర్ణాంకం)(e - eap->arg);
    e = skipwhite(e);

    ఒకవేళ (len == 5 && STRNCMP(eap->arg, "start", 5) == 0 && *e != NUL)
    {
	VIM_CLEAR(profile_fname);
	profile_fname = expand_env_save_opt(e, TRUE, ఖాళీ);
	do_profiling = PROF_YES;
	profile_zero(&prof_wait_సమయం);
	సమితి_vim_var_nr(VV_PROFILING, 1L);
    }
    లేకపోతే ఒకవేళ (do_profiling == PROF_NONE)
	emsg(_(e_మొదటి_use_profile_ప్రారంభం_fname));
    లేకపోతే ఒకవేళ (STRCMP(eap->arg, "stop") == 0)
    {
	profile_dump();
	do_profiling = PROF_NONE;
	సమితి_vim_var_nr(VV_PROFILING, 0L);
	profile_reset();
    }
    లేకపోతే ఒకవేళ (STRCMP(eap->arg, "pause") == 0)
    {
	ఒకవేళ (do_profiling == PROF_YES)
	    profile_ప్రారంభం(&pause_సమయం);
	do_profiling = PROF_PAUSED;
    }
    లేకపోతే ఒకవేళ (STRCMP(eap->arg, "continue") == 0)
    {
	ఒకవేళ (do_profiling == PROF_PAUSED)
	{
	    profile_చివర(&pause_సమయం);
	    profile_కూడు(&prof_wait_సమయం, &pause_సమయం);
	}
	do_profiling = PROF_YES;
    }
    లేకపోతే ఒకవేళ (STRCMP(eap->arg, "dump") == 0)
	profile_dump();
    లేకపోతే
    {
	// The rest is similar to ":breakadd".
	ex_breakadd(eap);
    }
}

// Command line expansion for :profile.
స్థిర గణన
{
    PEXP_SUBCMD,	// expand :profile sub-commands
    PEXP_FUNC		// expand :profile func {funcname}
} pexpand_what;

స్థిర అక్షరం *pexpand_cmds[] = {
			"start",
			"stop",
			"pause",
			"continue",
			"func",
			"file",
			"dump",
			ఖాళీ
};

/*
 * Function given to ExpandGeneric() to obtain the profile command
 * specific expansion.
 */
    char_u *
get_profile_name(expand_T *xp వాడని, పూర్ణాంకం idx)
{
    ఎంపిక (pexpand_what)
    {
    స్థితి PEXP_SUBCMD:
	తిరిగి (char_u *)pexpand_cmds[idx];
    అప్రమేయ:
	తిరిగి ఖాళీ;
    }
}

/*
 * Handle command line completion for :profile command.
 */
    శూన్య
సమితి_context_లోపల_profile_cmd(expand_T *xp, char_u *arg)
{
    char_u	*చివర_subcmd;

    // Default: expand subcommands.
    xp->xp_context = EXPAND_PROFILE;
    pexpand_what = PEXP_SUBCMD;
    xp->xp_నమూనా = arg;

    చివర_subcmd = skiptowhite(arg);
    ఒకవేళ (*చివర_subcmd == NUL)
	తిరిగి;

    ఒకవేళ ((చివర_subcmd - arg == 5 && STRNCMP(arg, "start", 5) == 0)
	    || (చివర_subcmd - arg == 4 && STRNCMP(arg, "file", 4) == 0))
    {
	xp->xp_context = EXPAND_FILES;
	xp->xp_నమూనా = skipwhite(చివర_subcmd);
	తిరిగి;
    }
    లేకపోతే ఒకవేళ (చివర_subcmd - arg == 4 && STRNCMP(arg, "func", 4) == 0)
    {
	xp->xp_context = EXPAND_USER_FUNC;
	xp->xp_నమూనా = skipwhite(చివర_subcmd);
	తిరిగి;
    }

    xp->xp_context = EXPAND_NOTHING;
}

స్థిర proftime_T inchar_సమయం;

/*
 * Called when starting to wait for the user to type a character.
 */
    శూన్య
prof_inchar_enter(శూన్య)
{
    profile_ప్రారంభం(&inchar_సమయం);
}

/*
 * Called when finished waiting for the user to type a character.
 */
    శూన్య
prof_inchar_exit(శూన్య)
{
    profile_చివర(&inchar_సమయం);
    profile_కూడు(&prof_wait_సమయం, &inchar_సమయం);
}


/*
 * Return TRUE when a function defined in the current script should be
 * profiled.
 */
    పూర్ణాంకం
prof_def_func(శూన్య)
{
    ఒకవేళ (current_sctx.sc_sid > 0)
	తిరిగి SCRIPT_అంశం(current_sctx.sc_sid)->sn_pr_force;
    తిరిగి FALSE;
}

/*
 * Print the count and times for one function or function line.
 */
    స్థిర శూన్య
prof_func_పంక్తి(
    FILE	*fd,
    పూర్ణాంకం		లెక్క,
    proftime_T	*మొత్తం,
    proftime_T	*self,
    పూర్ణాంకం		prefer_self)	// when equal print only self time
{
    ఒకవేళ (లెక్క > 0)
    {
	fprintf(fd, "%5d ", లెక్క);
	ఒకవేళ (prefer_self && profile_equal(మొత్తం, self))
	    fprintf(fd, PROF_సమయం_BLANK);
	లేకపోతే
	    fprintf(fd, "%s ", profile_సందేశం(మొత్తం));
	ఒకవేళ (!prefer_self && profile_equal(మొత్తం, self))
	    fprintf(fd, PROF_సమయం_BLANK);
	లేకపోతే
	    fprintf(fd, "%s ", profile_సందేశం(self));
    }
    లేకపోతే
	fprintf(fd, "      %s%s", PROF_సమయం_BLANK, PROF_సమయం_BLANK);
}

    స్థిర శూన్య
prof_క్రమీకరించు_list(
    FILE	*fd,
    ufunc_T	**sorttab,
    పూర్ణాంకం		st_len,
    అక్షరం	*title,
    పూర్ణాంకం		prefer_self)	// when equal print only self time
{
    పూర్ణాంకం		i;
    ufunc_T	*fp;

    fprintf(fd, "FUNCTIONS SORTED ON %s TIME\n", title);
    fprintf(fd, "%s  function\n", PROF_TOTALS_శీర్షిక);
    కొరకు (i = 0; i < 20 && i < st_len; ++i)
    {
	fp = sorttab[i];
	prof_func_పంక్తి(fd, fp->uf_tm_లెక్క, &fp->uf_tm_మొత్తం, &fp->uf_tm_self,
								 prefer_self);
	ఒకవేళ (fp->uf_name[0] == K_SPECIAL)
	    fprintf(fd, " <SNR>%s()\n", fp->uf_name + 3);
	లేకపోతే
	    fprintf(fd, " %s()\n", fp->uf_name);
    }
    fprintf(fd, "\n");
}

/*
 * Compare function for total time sorting.
 */
    స్థిర పూర్ణాంకం
prof_మొత్తం_cmp(స్థిరరాశి శూన్య *s1, స్థిరరాశి శూన్య *s2)
{
    ufunc_T	*p1, *p2;

    p1 = *(ufunc_T **)s1;
    p2 = *(ufunc_T **)s2;
    తిరిగి profile_cmp(&p1->uf_tm_మొత్తం, &p2->uf_tm_మొత్తం);
}

/*
 * Compare function for self time sorting.
 */
    స్థిర పూర్ణాంకం
prof_self_cmp(స్థిరరాశి శూన్య *s1, స్థిరరాశి శూన్య *s2)
{
    ufunc_T	*p1, *p2;

    p1 = *(ufunc_T **)s1;
    p2 = *(ufunc_T **)s2;
    తిరిగి profile_cmp(&p1->uf_tm_self, &p2->uf_tm_self);
}

/*
 * Start profiling function "fp".
 */
    శూన్య
func_do_profile(ufunc_T *fp)
{
    పూర్ణాంకం		len = fp->uf_పంక్తులు.ga_len;

    ఒకవేళ (!fp->uf_prof_initialized)
    {
	ఒకవేళ (len == 0)
	    len = 1;  // avoid getting error for allocating zero bytes
	fp->uf_tm_లెక్క = 0;
	profile_zero(&fp->uf_tm_self);
	profile_zero(&fp->uf_tm_మొత్తం);
	ఒకవేళ (fp->uf_tml_లెక్క == ఖాళీ)
	    fp->uf_tml_లెక్క = ALLOC_CLEAR_MULT(పూర్ణాంకం, len);
	ఒకవేళ (fp->uf_tml_మొత్తం == ఖాళీ)
	    fp->uf_tml_మొత్తం = ALLOC_CLEAR_MULT(proftime_T, len);
	ఒకవేళ (fp->uf_tml_self == ఖాళీ)
	    fp->uf_tml_self = ALLOC_CLEAR_MULT(proftime_T, len);
	fp->uf_tml_idx = -1;
	ఒకవేళ (fp->uf_tml_లెక్క == ఖాళీ || fp->uf_tml_మొత్తం == ఖాళీ
						    || fp->uf_tml_self == ఖాళీ)
	    తిరిగి;	    // out of memory
	fp->uf_prof_initialized = TRUE;
    }

    fp->uf_profiling = TRUE;
}

/*
 * Save time when starting to invoke another script or function.
 */
    స్థిర శూన్య
script_prof_save(
    proftime_T	*tm)	    // place to store wait time
{
    scriptitem_T    *si;

    ఒకవేళ (SCRIPT_ID_VALID(current_sctx.sc_sid))
    {
	si = SCRIPT_అంశం(current_sctx.sc_sid);
	ఒకవేళ (si->sn_prof_on && si->sn_pr_nest++ == 0)
	    profile_ప్రారంభం(&si->sn_pr_బిడ్డ);
    }
    profile_get_wait(tm);
}

/*
 * When calling a function: may initialize for profiling.
 */
    శూన్య
profile_may_ప్రారంభం_func(profinfo_T *info, ufunc_T *fp, ufunc_T *caller)
{
    ఒకవేళ (!fp->uf_profiling && has_profiling(FALSE, fp->uf_name, ఖాళీ,
								&fp->uf_hash))
    {
	info->pi_started_profiling = TRUE;
	func_do_profile(fp);
    }
    ఒకవేళ (fp->uf_profiling || (caller != ఖాళీ && caller->uf_profiling))
    {
	++fp->uf_tm_లెక్క;
	profile_ప్రారంభం(&info->pi_call_ప్రారంభం);
	profile_zero(&fp->uf_tm_children);
    }
    script_prof_save(&info->pi_wait_ప్రారంభం);
}

/*
 * After calling a function: may handle profiling.  profile_may_start_func()
 * must have been called previously.
 */
    శూన్య
profile_may_చివర_func(profinfo_T *info, ufunc_T *fp, ufunc_T *caller)
{
    profile_చివర(&info->pi_call_ప్రారంభం);
    profile_sub_wait(&info->pi_wait_ప్రారంభం, &info->pi_call_ప్రారంభం);
    profile_కూడు(&fp->uf_tm_మొత్తం, &info->pi_call_ప్రారంభం);
    profile_self(&fp->uf_tm_self, &info->pi_call_ప్రారంభం, &fp->uf_tm_children);
    ఒకవేళ (caller != ఖాళీ && caller->uf_profiling)
    {
	profile_కూడు(&caller->uf_tm_children, &info->pi_call_ప్రారంభం);
	profile_కూడు(&caller->uf_tml_children, &info->pi_call_ప్రారంభం);
    }
    ఒకవేళ (info->pi_started_profiling)
	// make a ":profdel func" stop profiling the function
	fp->uf_profiling = FALSE;
}

/*
 * Prepare profiling for entering a child or something else that is not
 * counted for the script/function itself.
 * Should always be called in pair with prof_child_exit().
 */
    శూన్య
prof_బిడ్డ_enter(
    proftime_T *tm)	// place to store waittime
{
    funccall_T *fc = get_current_funccal();

    ఒకవేళ (fc != ఖాళీ && fc->fc_func->uf_profiling)
	profile_ప్రారంభం(&fc->fc_prof_బిడ్డ);
    script_prof_save(tm);
}

/*
 * Take care of time spent in a child.
 * Should always be called after prof_child_enter().
 */
    శూన్య
prof_బిడ్డ_exit(
    proftime_T *tm)	// where waittime was stored
{
    funccall_T *fc = get_current_funccal();

    ఒకవేళ (fc != ఖాళీ && fc->fc_func->uf_profiling)
    {
	profile_చివర(&fc->fc_prof_బిడ్డ);
	profile_sub_wait(tm, &fc->fc_prof_బిడ్డ); // don't count waiting time
	profile_కూడు(&fc->fc_func->uf_tm_children, &fc->fc_prof_బిడ్డ);
	profile_కూడు(&fc->fc_func->uf_tml_children, &fc->fc_prof_బిడ్డ);
    }
    script_prof_restore(tm);
}

/*
 * Called when starting to read a function line.
 * "sourcing_lnum" must be correct!
 * When skipping lines it may not actually be executed, but we won't find out
 * until later and we need to store the time now.
 */
    శూన్య
func_పంక్తి_ప్రారంభం(శూన్య *cookie, దీర్ఘ lnum)
{
    funccall_T	*fcp = (funccall_T *)cookie;
    ufunc_T	*fp = fcp->fc_func;

    ఒకవేళ (fp->uf_profiling && lnum >= 1 && lnum <= fp->uf_పంక్తులు.ga_len)
    {
	fp->uf_tml_idx = lnum - 1;
	// Skip continuation lines.
	అయితే (fp->uf_tml_idx > 0 && FUNCLINE(fp, fp->uf_tml_idx) == ఖాళీ)
	    --fp->uf_tml_idx;
	fp->uf_tml_execed = FALSE;
	profile_ప్రారంభం(&fp->uf_tml_ప్రారంభం);
	profile_zero(&fp->uf_tml_children);
	profile_get_wait(&fp->uf_tml_wait);
    }
}

/*
 * Called when actually executing a function line.
 */
    శూన్య
func_పంక్తి_exec(శూన్య *cookie)
{
    funccall_T	*fcp = (funccall_T *)cookie;
    ufunc_T	*fp = fcp->fc_func;

    ఒకవేళ (fp->uf_profiling && fp->uf_tml_idx >= 0)
	fp->uf_tml_execed = TRUE;
}

/*
 * Called when done with a function line.
 */
    శూన్య
func_పంక్తి_చివర(శూన్య *cookie)
{
    funccall_T	*fcp = (funccall_T *)cookie;
    ufunc_T	*fp = fcp->fc_func;

    ఒకవేళ (fp->uf_profiling && fp->uf_tml_idx >= 0)
    {
	ఒకవేళ (fp->uf_tml_execed)
	{
	    ++fp->uf_tml_లెక్క[fp->uf_tml_idx];
	    profile_చివర(&fp->uf_tml_ప్రారంభం);
	    profile_sub_wait(&fp->uf_tml_wait, &fp->uf_tml_ప్రారంభం);
	    profile_కూడు(&fp->uf_tml_మొత్తం[fp->uf_tml_idx], &fp->uf_tml_ప్రారంభం);
	    profile_self(&fp->uf_tml_self[fp->uf_tml_idx], &fp->uf_tml_ప్రారంభం,
							&fp->uf_tml_children);
	}
	fp->uf_tml_idx = -1;
    }
}

/*
 * Dump the profiling results for all functions in file "fd".
 */
    స్థిర శూన్య
func_dump_profile(FILE *fd)
{
    hashtab_T	*functbl;
    hashitem_T	*hi;
    పూర్ణాంకం		todo;
    ufunc_T	*fp;
    పూర్ణాంకం		i;
    ufunc_T	**sorttab;
    పూర్ణాంకం		st_len = 0;
    char_u	*p;

    functbl = func_tbl_get();
    todo = (పూర్ణాంకం)functbl->ht_వాడిన;
    ఒకవేళ (todo == 0)
	తిరిగి;     // nothing to dump

    sorttab = ALLOC_MULT(ufunc_T *, todo);

    FOR_ALL_HASHTAB_వస్తువులు(functbl, hi, todo)
    {
	ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
	{
	    --todo;
	    fp = HI2UF(hi);
	    ఒకవేళ (fp->uf_prof_initialized)
	    {
		ఒకవేళ (sorttab != ఖాళీ)
		    sorttab[st_len++] = fp;

		ఒకవేళ (fp->uf_name[0] == K_SPECIAL)
		    fprintf(fd, "FUNCTION  <SNR>%s()\n", fp->uf_name + 3);
		లేకపోతే
		    fprintf(fd, "FUNCTION  %s()\n", fp->uf_name);
		ఒకవేళ (fp->uf_script_ctx.sc_sid > 0)
		{
		    p = home_replace_save(ఖాళీ,
				     get_scriptname(fp->uf_script_ctx.sc_sid));
		    ఒకవేళ (p != ఖాళీ)
		    {
			fprintf(fd, "    Defined: %s:%ld\n",
					   p, (దీర్ఘ)fp->uf_script_ctx.sc_lnum);
			vim_free(p);
		    }
		}
		ఒకవేళ (fp->uf_tm_లెక్క == 1)
		    fprintf(fd, "Called 1 time\n");
		లేకపోతే
		    fprintf(fd, "Called %d times\n", fp->uf_tm_లెక్క);
		fprintf(fd, "Total time: %s\n", profile_సందేశం(&fp->uf_tm_మొత్తం));
		fprintf(fd, " Self time: %s\n", profile_సందేశం(&fp->uf_tm_self));
		fprintf(fd, "\n");
		fprintf(fd, "%s\n", PROF_TOTALS_శీర్షిక);

		కొరకు (i = 0; i < fp->uf_పంక్తులు.ga_len; ++i)
		{
		    ఒకవేళ (FUNCLINE(fp, i) == ఖాళీ)
			కొనసాగు;
		    prof_func_పంక్తి(fd, fp->uf_tml_లెక్క[i],
			     &fp->uf_tml_మొత్తం[i], &fp->uf_tml_self[i], TRUE);
		    fprintf(fd, "%s\n", FUNCLINE(fp, i));
		}
		fprintf(fd, "\n");
	    }
	}
    }

    ఒకవేళ (sorttab != ఖాళీ && st_len > 0)
    {
	qsort((శూన్య *)sorttab, (size_t)st_len, పరిమాణం(ufunc_T *),
							      prof_మొత్తం_cmp);
	prof_క్రమీకరించు_list(fd, sorttab, st_len, "TOTAL", FALSE);
	qsort((శూన్య *)sorttab, (size_t)st_len, పరిమాణం(ufunc_T *),
							      prof_self_cmp);
	prof_క్రమీకరించు_list(fd, sorttab, st_len, "SELF", TRUE);
    }

    vim_free(sorttab);
}

/*
 * Start profiling script "fp".
 */
    శూన్య
script_do_profile(scriptitem_T *si)
{
    si->sn_pr_లెక్క = 0;
    profile_zero(&si->sn_pr_మొత్తం);
    profile_zero(&si->sn_pr_self);

    ga_init2(&si->sn_prl_ga, పరిమాణం(sn_prl_T), 100);
    si->sn_prl_idx = -1;
    si->sn_prof_on = TRUE;
    si->sn_pr_nest = 0;
}

/*
 * Count time spent in children after invoking another script or function.
 */
    శూన్య
script_prof_restore(proftime_T *tm)
{
    scriptitem_T    *si;

    ఒకవేళ (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	తిరిగి;

    si = SCRIPT_అంశం(current_sctx.sc_sid);
    ఒకవేళ (si->sn_prof_on && --si->sn_pr_nest == 0)
    {
	profile_చివర(&si->sn_pr_బిడ్డ);
	profile_sub_wait(tm, &si->sn_pr_బిడ్డ); // don't count wait time
	profile_కూడు(&si->sn_pr_children, &si->sn_pr_బిడ్డ);
	profile_కూడు(&si->sn_prl_children, &si->sn_pr_బిడ్డ);
    }
}

/*
 * Dump the profiling results for all scripts in file "fd".
 */
    స్థిర శూన్య
script_dump_profile(FILE *fd)
{
    పూర్ణాంకం		    id;
    scriptitem_T    *si;
    పూర్ణాంకం		    i;
    FILE	    *sfd;
    sn_prl_T	    *pp;

    కొరకు (id = 1; id <= script_వస్తువులు.ga_len; ++id)
    {
	si = SCRIPT_అంశం(id);
	ఒకవేళ (si->sn_prof_on)
	{
	    fprintf(fd, "SCRIPT  %s\n", si->sn_name);
	    ఒకవేళ (si->sn_pr_లెక్క == 1)
		fprintf(fd, "Sourced 1 time\n");
	    లేకపోతే
		fprintf(fd, "Sourced %d times\n", si->sn_pr_లెక్క);
	    fprintf(fd, "Total time: %s\n", profile_సందేశం(&si->sn_pr_మొత్తం));
	    fprintf(fd, " Self time: %s\n", profile_సందేశం(&si->sn_pr_self));
	    fprintf(fd, "\n");
	    fprintf(fd, "%s\n", PROF_TOTALS_శీర్షిక);

	    sfd = mch_fopen((అక్షరం *)si->sn_name, "r");
	    ఒకవేళ (sfd == ఖాళీ)
		fprintf(fd, "Cannot open file!\n");
	    లేకపోతే
	    {
		// Keep going till the end of file, so that trailing
		// continuation lines are listed.
		కొరకు (i = 0; ; ++i)
		{
		    ఒకవేళ (vim_fgets(IObuff, IOSIZE, sfd))
			ఆపు;
		    // When a line has been truncated, append NL, taking care
		    // of multi-byte characters .
		    ఒకవేళ (IObuff[IOSIZE - 2] != NUL && IObuff[IOSIZE - 2] != NL)
		    {
			పూర్ణాంకం n = IOSIZE - 2;

			ఒకవేళ (enc_utf8)
			{
			    // Move to the first byte of this char.
			    // utf_head_off() doesn't work, because it checks
			    // for a truncated character.
			    అయితే (n > 0 && (IObuff[n] & 0xc0) == 0x80)
				--n;
			}
			లేకపోతే ఒకవేళ (has_mbyte)
			    n -= mb_తల_off(IObuff, IObuff + n);
			IObuff[n] = NL;
			IObuff[n + 1] = NUL;
		    }
		    ఒకవేళ (i < si->sn_prl_ga.ga_len
				     && (pp = &PRL_అంశం(si, i))->snp_లెక్క > 0)
		    {
			fprintf(fd, "%5d ", pp->snp_లెక్క);
			ఒకవేళ (profile_equal(&pp->sn_prl_మొత్తం, &pp->sn_prl_self))
			    fprintf(fd, "           ");
			లేకపోతే
			    fprintf(fd, "%s ", profile_సందేశం(&pp->sn_prl_మొత్తం));
			fprintf(fd, "%s ", profile_సందేశం(&pp->sn_prl_self));
		    }
		    లేకపోతే
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
    శూన్య
profile_dump(శూన్య)
{
    FILE	*fd;

    ఒకవేళ (profile_fname == ఖాళీ)
	తిరిగి;

    fd = mch_fopen((అక్షరం *)profile_fname, "w");
    ఒకవేళ (fd == ఖాళీ)
	semsg(_(e_cant_open_file_str), profile_fname);
    లేకపోతే
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
    శూన్య
script_పంక్తి_ప్రారంభం(శూన్య)
{
    scriptitem_T    *si;
    sn_prl_T	    *pp;

    ఒకవేళ (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	తిరిగి;
    si = SCRIPT_అంశం(current_sctx.sc_sid);
    ఒకవేళ (si->sn_prof_on && SOURCING_LNUM >= 1)
    {
	// Grow the array before starting the timer, so that the time spent
	// here isn't counted.
	(శూన్య)ga_grow(&si->sn_prl_ga,
				  (పూర్ణాంకం)(SOURCING_LNUM - si->sn_prl_ga.ga_len));
	si->sn_prl_idx = SOURCING_LNUM - 1;
	అయితే (si->sn_prl_ga.ga_len <= si->sn_prl_idx
		&& si->sn_prl_ga.ga_len < si->sn_prl_ga.ga_maxlen)
	{
	    // Zero counters for a line that was not used before.
	    pp = &PRL_అంశం(si, si->sn_prl_ga.ga_len);
	    pp->snp_లెక్క = 0;
	    profile_zero(&pp->sn_prl_మొత్తం);
	    profile_zero(&pp->sn_prl_self);
	    ++si->sn_prl_ga.ga_len;
	}
	si->sn_prl_execed = FALSE;
	profile_ప్రారంభం(&si->sn_prl_ప్రారంభం);
	profile_zero(&si->sn_prl_children);
	profile_get_wait(&si->sn_prl_wait);
    }
}

/*
 * Called when actually executing a function line.
 */
    శూన్య
script_పంక్తి_exec(శూన్య)
{
    scriptitem_T    *si;

    ఒకవేళ (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	తిరిగి;
    si = SCRIPT_అంశం(current_sctx.sc_sid);
    ఒకవేళ (si->sn_prof_on && si->sn_prl_idx >= 0)
	si->sn_prl_execed = TRUE;
}

/*
 * Called when done with a script line.
 */
    శూన్య
script_పంక్తి_చివర(శూన్య)
{
    scriptitem_T    *si;
    sn_prl_T	    *pp;

    ఒకవేళ (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	తిరిగి;
    si = SCRIPT_అంశం(current_sctx.sc_sid);
    ఒకవేళ (si->sn_prof_on && si->sn_prl_idx >= 0
				     && si->sn_prl_idx < si->sn_prl_ga.ga_len)
    {
	ఒకవేళ (si->sn_prl_execed)
	{
	    pp = &PRL_అంశం(si, si->sn_prl_idx);
	    ++pp->snp_లెక్క;
	    profile_చివర(&si->sn_prl_ప్రారంభం);
	    profile_sub_wait(&si->sn_prl_wait, &si->sn_prl_ప్రారంభం);
	    profile_కూడు(&pp->sn_prl_మొత్తం, &si->sn_prl_ప్రారంభం);
	    profile_self(&pp->sn_prl_self, &si->sn_prl_ప్రారంభం,
							&si->sn_prl_children);
	}
	si->sn_prl_idx = -1;
    }
}
# endif // FEAT_PROFILE

#అయితే_ముగింపు
