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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ defined(FEAT_EVAL)
# ਜੇ defined(FEAT_PROFILE) || defined(FEAT_RELTIME)
/*
 * Store the current time in "tm".
 */
    ਖਾਲੀ
profile_ਸ਼ੁਰੂ(proftime_T *tm)
{
#  ifdef MSWIN
    QueryPerformanceCounter(tm);
#  ਨਹੀਂਤਾਂ
    PROF_GET_ਸਮਾਂ(tm);
#  endif
}

/*
 * Compute the elapsed time from "tm" till now and store in "tm".
 */
    ਖਾਲੀ
profile_ਸਿਰਾ(proftime_T *tm)
{
    proftime_T now;

#  ifdef MSWIN
    QueryPerformanceCounter(&now);
    tm->QuadPart = now.QuadPart - tm->QuadPart;
#  ਨਹੀਂਤਾਂ
    PROF_GET_ਸਮਾਂ(&now);
    tm->tv_fsec = now.tv_fsec - tm->tv_fsec;
    tm->tv_sec = now.tv_sec - tm->tv_sec;
    ਜੇ (tm->tv_fsec < 0)
    {
	tm->tv_fsec += TV_FSEC_SEC;
	--tm->tv_sec;
    }
#  endif
}

/*
 * Subtract the time "tm2" from "tm".
 */
    ਖਾਲੀ
profile_sub(proftime_T *tm, proftime_T *tm2)
{
#  ifdef MSWIN
    tm->QuadPart -= tm2->QuadPart;
#  ਨਹੀਂਤਾਂ
    tm->tv_fsec -= tm2->tv_fsec;
    tm->tv_sec -= tm2->tv_sec;
    ਜੇ (tm->tv_fsec < 0)
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
    ਅੱਖਰ *
profile_ਸੁਨੇਹਾ(proftime_T *tm)
{
    ਸਥਿਰ ਅੱਖਰ buf[50];

#  ifdef MSWIN
    LARGE_INTEGER   fr;

    QueryPerformanceFrequency(&fr);
    sprintf(buf, "%10.6lf", (ਦੁੱਗਣਾ)tm->QuadPart / (ਦੁੱਗਣਾ)fr.QuadPart);
#  ਨਹੀਂਤਾਂ
    sprintf(buf, PROF_ਸਮਾਂ_FORMAT, (ਲੰਮਾ)tm->tv_sec, (ਲੰਮਾ)tm->tv_fsec);
#  endif
    ਵਾਪਸ buf;
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
    ਵਾਪਸ (float_T)tm->QuadPart / (float_T)fr.QuadPart;
#  ਨਹੀਂਤਾਂ
    ਵਾਪਸ (float_T)tm->tv_sec + (float_T)tm->tv_fsec / (float_T)TV_FSEC_SEC;
#  endif
}

/*
 * Put the time "msec" past now in "tm".
 */
    ਖਾਲੀ
profile_setlimit(ਲੰਮਾ msec, proftime_T *tm)
{
    ਜੇ (msec <= 0)   // no limit
	profile_zero(tm);
    ਨਹੀਂਤਾਂ
    {
#  ifdef MSWIN
	LARGE_INTEGER   fr;

	QueryPerformanceCounter(tm);
	QueryPerformanceFrequency(&fr);
	tm->QuadPart += (LONGLONG)((ਦੁੱਗਣਾ)msec / 1000.0 * (ਦੁੱਗਣਾ)fr.QuadPart);
#  ਨਹੀਂਤਾਂ
	varnumber_T	    fsec;	// this should be 64 bit if possible

	PROF_GET_ਸਮਾਂ(tm);
	fsec = (varnumber_T)tm->tv_fsec
		       + (varnumber_T)msec * (varnumber_T)(TV_FSEC_SEC / 1000);
	tm->tv_fsec = fsec % (ਲੰਮਾ)TV_FSEC_SEC;
	tm->tv_sec += fsec / (ਲੰਮਾ)TV_FSEC_SEC;
#  endif
    }
}

/*
 * Return TRUE if the current time is past "tm".
 */
    ਪੂਰਨ
profile_passed_ਹੱਦਬੰਦੀ(proftime_T *tm)
{
    proftime_T	now;

#  ifdef MSWIN
    ਜੇ (tm->QuadPart == 0)  // timer was not set
	ਵਾਪਸ FALSE;
    QueryPerformanceCounter(&now);
    ਵਾਪਸ (now.QuadPart > tm->QuadPart);
#  ਨਹੀਂਤਾਂ
    ਜੇ (tm->tv_sec == 0)    // timer was not set
	ਵਾਪਸ FALSE;
    PROF_GET_ਸਮਾਂ(&now);
    ਵਾਪਸ (now.tv_sec > tm->tv_sec
	    || (now.tv_sec == tm->tv_sec && now.tv_fsec > tm->tv_fsec));
#  endif
}

/*
 * Set the time in "tm" to zero.
 */
    ਖਾਲੀ
profile_zero(proftime_T *tm)
{
#  ifdef MSWIN
    tm->QuadPart = 0;
#  ਨਹੀਂਤਾਂ
    tm->tv_fsec = 0;
    tm->tv_sec = 0;
#  endif
}

# endif  // FEAT_PROFILE || FEAT_RELTIME

# ਜੇ defined(FEAT_SYN_HL) && defined(FEAT_RELTIME) && defined(FEAT_PROFILE)
#  ਜੇ defined(HAVE_MATH_H)
#   include <math.h>
#  endif

/*
 * Divide the time "tm" by "count" and store in "tm2".
 */
    ਖਾਲੀ
profile_divide(proftime_T *tm, ਪੂਰਨ ਗਿਣਤੀ_2, proftime_T *tm2)
{
    ਜੇ (ਗਿਣਤੀ_2 == 0)
	profile_zero(tm2);
    ਨਹੀਂਤਾਂ
    {
#  ifdef MSWIN
	tm2->QuadPart = tm->QuadPart / ਗਿਣਤੀ_2;
#  ਨਹੀਂਤਾਂ
	ਦੁੱਗਣਾ fsec = (tm->tv_sec * (float_T)TV_FSEC_SEC + tm->tv_fsec)
								       / ਗਿਣਤੀ_2;

	tm2->tv_sec = floor(fsec / (float_T)TV_FSEC_SEC);
	tm2->tv_fsec = vim_ਗੇੜਾ(fsec - (tm2->tv_sec * (float_T)TV_FSEC_SEC));
#  endif
    }
}
# endif

# ਜੇ defined(FEAT_PROFILE)
/*
 * Functions for profiling.
 */
ਸਥਿਰ proftime_T prof_wait_ਸਮਾਂ;

/*
 * Add the time "tm2" to "tm".
 */
    ਖਾਲੀ
profile_ਜੋੜੋ(proftime_T *tm, proftime_T *tm2)
{
#  ifdef MSWIN
    tm->QuadPart += tm2->QuadPart;
#  ਨਹੀਂਤਾਂ
    tm->tv_fsec += tm2->tv_fsec;
    tm->tv_sec += tm2->tv_sec;
    ਜੇ (tm->tv_fsec >= TV_FSEC_SEC)
    {
	tm->tv_fsec -= TV_FSEC_SEC;
	++tm->tv_sec;
    }
#  endif
}

/*
 * Add the "self" time from the total time and the children's time.
 */
    ਖਾਲੀ
profile_self(proftime_T *self, proftime_T *ਕੁੱਲ, proftime_T *children)
{
    // Check that the result won't be negative.  Can happen with recursive
    // calls.
#  ifdef MSWIN
    ਜੇ (ਕੁੱਲ->QuadPart <= children->QuadPart)
	ਵਾਪਸ;
#  ਨਹੀਂਤਾਂ
    ਜੇ (ਕੁੱਲ->tv_sec < children->tv_sec
	    || (ਕੁੱਲ->tv_sec == children->tv_sec
		&& ਕੁੱਲ->tv_fsec <= children->tv_fsec))
	ਵਾਪਸ;
#  endif
    profile_ਜੋੜੋ(self, ਕੁੱਲ);
    profile_sub(self, children);
}

/*
 * Get the current waittime.
 */
    ਸਥਿਰ ਖਾਲੀ
profile_get_wait(proftime_T *tm)
{
    *tm = prof_wait_ਸਮਾਂ;
}

/*
 * Subtract the passed waittime since "tm" from "tma".
 */
    ਖਾਲੀ
profile_sub_wait(proftime_T *tm, proftime_T *tma)
{
    proftime_T tm3 = prof_wait_ਸਮਾਂ;

    profile_sub(&tm3, tm);
    profile_sub(tma, &tm3);
}

/*
 * Return TRUE if "tm1" and "tm2" are equal.
 */
    ਸਥਿਰ ਪੂਰਨ
profile_equal(proftime_T *tm1, proftime_T *tm2)
{
#  ifdef MSWIN
    ਵਾਪਸ (tm1->QuadPart == tm2->QuadPart);
#  ਨਹੀਂਤਾਂ
    ਵਾਪਸ (tm1->tv_fsec == tm2->tv_fsec && tm1->tv_sec == tm2->tv_sec);
#  endif
}

/*
 * Return <0, 0 or >0 if "tm1" < "tm2", "tm1" == "tm2" or "tm1" > "tm2"
 */
    ਪੂਰਨ
profile_cmp(ਅਚਲ proftime_T *tm1, ਅਚਲ proftime_T *tm2)
{
#  ifdef MSWIN
    ਵਾਪਸ tm2->QuadPart == tm1->QuadPart ? 0 :
	tm2->QuadPart > tm1->QuadPart ? 1 : -1;
#  ਨਹੀਂਤਾਂ
    ਜੇ (tm1->tv_sec == tm2->tv_sec)
	ਵਾਪਸ tm2->tv_fsec == tm1->tv_fsec ? 0 :
	    tm2->tv_fsec > tm1->tv_fsec ? 1 : -1;
    ਵਾਪਸ tm2->tv_sec > tm1->tv_sec ? 1 : -1;
#  endif
}

ਸਥਿਰ char_u	*profile_fname = ਨਿਰਾ;
ਸਥਿਰ proftime_T pause_ਸਮਾਂ;

/*
 * Reset all profiling information.
 */
    ਸਥਿਰ ਖਾਲੀ
profile_reset(ਖਾਲੀ)
{
    ਪੂਰਨ		id;
    ਪੂਰਨ		todo;
    hashtab_T	*functbl;
    hashitem_T	*hi;

    // Reset sourced files.
    ਲਈ (id = 1; id <= script_ਚੀਜ਼ਾਂ.ga_len; id++)
    {
	scriptitem_T *si = SCRIPT_ਵਸਤੂ(id);
	ਜੇ (si->sn_prof_on)
	{
	    si->sn_prof_on      = FALSE;
	    si->sn_pr_force     = FALSE;
	    profile_zero(&si->sn_pr_ਬੱਚਾ);
	    si->sn_pr_nest      = 0;
	    si->sn_pr_ਗਿਣਤੀ     = 0;
	    profile_zero(&si->sn_pr_ਕੁੱਲ);
	    profile_zero(&si->sn_pr_self);
	    profile_zero(&si->sn_pr_ਸ਼ੁਰੂ);
	    profile_zero(&si->sn_pr_children);
	    ga_clear(&si->sn_prl_ga);
	    profile_zero(&si->sn_prl_ਸ਼ੁਰੂ);
	    profile_zero(&si->sn_prl_children);
	    profile_zero(&si->sn_prl_wait);
	    si->sn_prl_idx      = -1;
	    si->sn_prl_execed   = 0;
	}
    }

    // Reset functions.
    functbl = func_tbl_get();
    todo = (ਪੂਰਨ)functbl->ht_ਵਰਤਿਆ;

    FOR_ALL_HASHTAB_ਚੀਜ਼ਾਂ(functbl, hi, todo)
    {
	ufunc_T *fp;
	ਪੂਰਨ	i;

	ਜੇ (HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	    ਜਾਰੀ;

	todo--;
	fp = HI2UF(hi);
	ਜੇ (fp->uf_prof_initialized)
	{
	    fp->uf_profiling    = 0;
	    fp->uf_prof_initialized = FALSE;
	    fp->uf_tm_ਗਿਣਤੀ     = 0;
	    profile_zero(&fp->uf_tm_ਕੁੱਲ);
	    profile_zero(&fp->uf_tm_self);
	    profile_zero(&fp->uf_tm_children);

	    ਲਈ (i = 0; i < fp->uf_ਸਤਰਾਂ.ga_len; i++)
	    {
		fp->uf_tml_ਗਿਣਤੀ[i] = 0;
		profile_zero(&fp->uf_tml_ਕੁੱਲ[i]);
		profile_zero(&fp->uf_tml_self[i]);
	    }

	    profile_zero(&fp->uf_tml_ਸ਼ੁਰੂ);
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
    ਖਾਲੀ
ex_profile(exarg_T *eap)
{
    char_u	*e;
    ਪੂਰਨ		len;

    e = skiptowhite(eap->arg);
    len = (ਪੂਰਨ)(e - eap->arg);
    e = skipwhite(e);

    ਜੇ (len == 5 && STRNCMP(eap->arg, "start", 5) == 0 && *e != NUL)
    {
	VIM_CLEAR(profile_fname);
	profile_fname = expand_env_save_opt(e, TRUE, ਨਿਰਾ);
	do_profiling = PROF_YES;
	profile_zero(&prof_wait_ਸਮਾਂ);
	ਸੈੱਟ_vim_var_nr(VV_PROFILING, 1L);
    }
    ਨਹੀਂਤਾਂ ਜੇ (do_profiling == PROF_NONE)
	emsg(_(e_ਪਹਿਲਾ_use_profile_ਸ਼ੁਰੂ_fname));
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(eap->arg, "stop") == 0)
    {
	profile_dump();
	do_profiling = PROF_NONE;
	ਸੈੱਟ_vim_var_nr(VV_PROFILING, 0L);
	profile_reset();
    }
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(eap->arg, "pause") == 0)
    {
	ਜੇ (do_profiling == PROF_YES)
	    profile_ਸ਼ੁਰੂ(&pause_ਸਮਾਂ);
	do_profiling = PROF_PAUSED;
    }
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(eap->arg, "continue") == 0)
    {
	ਜੇ (do_profiling == PROF_PAUSED)
	{
	    profile_ਸਿਰਾ(&pause_ਸਮਾਂ);
	    profile_ਜੋੜੋ(&prof_wait_ਸਮਾਂ, &pause_ਸਮਾਂ);
	}
	do_profiling = PROF_YES;
    }
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(eap->arg, "dump") == 0)
	profile_dump();
    ਨਹੀਂਤਾਂ
    {
	// The rest is similar to ":breakadd".
	ex_breakadd(eap);
    }
}

// Command line expansion for :profile.
ਸਥਿਰ ਗਿਣਤੀ
{
    PEXP_SUBCMD,	// expand :profile sub-commands
    PEXP_FUNC		// expand :profile func {funcname}
} pexpand_what;

ਸਥਿਰ ਅੱਖਰ *pexpand_cmds[] = {
			"start",
			"stop",
			"pause",
			"continue",
			"func",
			"file",
			"dump",
			ਨਿਰਾ
};

/*
 * Function given to ExpandGeneric() to obtain the profile command
 * specific expansion.
 */
    char_u *
get_profile_name(expand_T *xp ਅਣਵਰਤਿਆ, ਪੂਰਨ idx)
{
    ਚੋਣ (pexpand_what)
    {
    ਹਾਲਤ PEXP_SUBCMD:
	ਵਾਪਸ (char_u *)pexpand_cmds[idx];
    ਮੂਲ:
	ਵਾਪਸ ਨਿਰਾ;
    }
}

/*
 * Handle command line completion for :profile command.
 */
    ਖਾਲੀ
ਸੈੱਟ_context_ਵਿੱਚ_profile_cmd(expand_T *xp, char_u *arg)
{
    char_u	*ਸਿਰਾ_subcmd;

    // Default: expand subcommands.
    xp->xp_context = EXPAND_PROFILE;
    pexpand_what = PEXP_SUBCMD;
    xp->xp_ਨਮੂਨਾ = arg;

    ਸਿਰਾ_subcmd = skiptowhite(arg);
    ਜੇ (*ਸਿਰਾ_subcmd == NUL)
	ਵਾਪਸ;

    ਜੇ ((ਸਿਰਾ_subcmd - arg == 5 && STRNCMP(arg, "start", 5) == 0)
	    || (ਸਿਰਾ_subcmd - arg == 4 && STRNCMP(arg, "file", 4) == 0))
    {
	xp->xp_context = EXPAND_FILES;
	xp->xp_ਨਮੂਨਾ = skipwhite(ਸਿਰਾ_subcmd);
	ਵਾਪਸ;
    }
    ਨਹੀਂਤਾਂ ਜੇ (ਸਿਰਾ_subcmd - arg == 4 && STRNCMP(arg, "func", 4) == 0)
    {
	xp->xp_context = EXPAND_USER_FUNC;
	xp->xp_ਨਮੂਨਾ = skipwhite(ਸਿਰਾ_subcmd);
	ਵਾਪਸ;
    }

    xp->xp_context = EXPAND_NOTHING;
}

ਸਥਿਰ proftime_T inchar_ਸਮਾਂ;

/*
 * Called when starting to wait for the user to type a character.
 */
    ਖਾਲੀ
prof_inchar_enter(ਖਾਲੀ)
{
    profile_ਸ਼ੁਰੂ(&inchar_ਸਮਾਂ);
}

/*
 * Called when finished waiting for the user to type a character.
 */
    ਖਾਲੀ
prof_inchar_exit(ਖਾਲੀ)
{
    profile_ਸਿਰਾ(&inchar_ਸਮਾਂ);
    profile_ਜੋੜੋ(&prof_wait_ਸਮਾਂ, &inchar_ਸਮਾਂ);
}


/*
 * Return TRUE when a function defined in the current script should be
 * profiled.
 */
    ਪੂਰਨ
prof_def_func(ਖਾਲੀ)
{
    ਜੇ (current_sctx.sc_sid > 0)
	ਵਾਪਸ SCRIPT_ਵਸਤੂ(current_sctx.sc_sid)->sn_pr_force;
    ਵਾਪਸ FALSE;
}

/*
 * Print the count and times for one function or function line.
 */
    ਸਥਿਰ ਖਾਲੀ
prof_func_ਸਤਰ(
    FILE	*fd,
    ਪੂਰਨ		ਗਿਣਤੀ_2,
    proftime_T	*ਕੁੱਲ,
    proftime_T	*self,
    ਪੂਰਨ		prefer_self)	// when equal print only self time
{
    ਜੇ (ਗਿਣਤੀ_2 > 0)
    {
	fprintf(fd, "%5d ", ਗਿਣਤੀ_2);
	ਜੇ (prefer_self && profile_equal(ਕੁੱਲ, self))
	    fprintf(fd, PROF_ਸਮਾਂ_BLANK);
	ਨਹੀਂਤਾਂ
	    fprintf(fd, "%s ", profile_ਸੁਨੇਹਾ(ਕੁੱਲ));
	ਜੇ (!prefer_self && profile_equal(ਕੁੱਲ, self))
	    fprintf(fd, PROF_ਸਮਾਂ_BLANK);
	ਨਹੀਂਤਾਂ
	    fprintf(fd, "%s ", profile_ਸੁਨੇਹਾ(self));
    }
    ਨਹੀਂਤਾਂ
	fprintf(fd, "      %s%s", PROF_ਸਮਾਂ_BLANK, PROF_ਸਮਾਂ_BLANK);
}

    ਸਥਿਰ ਖਾਲੀ
prof_ਛਾਂਟੋ_list(
    FILE	*fd,
    ufunc_T	**sorttab,
    ਪੂਰਨ		st_len,
    ਅੱਖਰ	*title,
    ਪੂਰਨ		prefer_self)	// when equal print only self time
{
    ਪੂਰਨ		i;
    ufunc_T	*fp;

    fprintf(fd, "FUNCTIONS SORTED ON %s TIME\n", title);
    fprintf(fd, "%s  function\n", PROF_TOTALS_ਸਿਰਲੇਖ);
    ਲਈ (i = 0; i < 20 && i < st_len; ++i)
    {
	fp = sorttab[i];
	prof_func_ਸਤਰ(fd, fp->uf_tm_ਗਿਣਤੀ, &fp->uf_tm_ਕੁੱਲ, &fp->uf_tm_self,
								 prefer_self);
	ਜੇ (fp->uf_name[0] == K_SPECIAL)
	    fprintf(fd, " <SNR>%s()\n", fp->uf_name + 3);
	ਨਹੀਂਤਾਂ
	    fprintf(fd, " %s()\n", fp->uf_name);
    }
    fprintf(fd, "\n");
}

/*
 * Compare function for total time sorting.
 */
    ਸਥਿਰ ਪੂਰਨ
prof_ਕੁੱਲ_cmp(ਅਚਲ ਖਾਲੀ *s1, ਅਚਲ ਖਾਲੀ *s2)
{
    ufunc_T	*p1, *p2;

    p1 = *(ufunc_T **)s1;
    p2 = *(ufunc_T **)s2;
    ਵਾਪਸ profile_cmp(&p1->uf_tm_ਕੁੱਲ, &p2->uf_tm_ਕੁੱਲ);
}

/*
 * Compare function for self time sorting.
 */
    ਸਥਿਰ ਪੂਰਨ
prof_self_cmp(ਅਚਲ ਖਾਲੀ *s1, ਅਚਲ ਖਾਲੀ *s2)
{
    ufunc_T	*p1, *p2;

    p1 = *(ufunc_T **)s1;
    p2 = *(ufunc_T **)s2;
    ਵਾਪਸ profile_cmp(&p1->uf_tm_self, &p2->uf_tm_self);
}

/*
 * Start profiling function "fp".
 */
    ਖਾਲੀ
func_do_profile(ufunc_T *fp)
{
    ਪੂਰਨ		len = fp->uf_ਸਤਰਾਂ.ga_len;

    ਜੇ (!fp->uf_prof_initialized)
    {
	ਜੇ (len == 0)
	    len = 1;  // avoid getting error for allocating zero bytes
	fp->uf_tm_ਗਿਣਤੀ = 0;
	profile_zero(&fp->uf_tm_self);
	profile_zero(&fp->uf_tm_ਕੁੱਲ);
	ਜੇ (fp->uf_tml_ਗਿਣਤੀ == ਨਿਰਾ)
	    fp->uf_tml_ਗਿਣਤੀ = ALLOC_CLEAR_MULT(ਪੂਰਨ, len);
	ਜੇ (fp->uf_tml_ਕੁੱਲ == ਨਿਰਾ)
	    fp->uf_tml_ਕੁੱਲ = ALLOC_CLEAR_MULT(proftime_T, len);
	ਜੇ (fp->uf_tml_self == ਨਿਰਾ)
	    fp->uf_tml_self = ALLOC_CLEAR_MULT(proftime_T, len);
	fp->uf_tml_idx = -1;
	ਜੇ (fp->uf_tml_ਗਿਣਤੀ == ਨਿਰਾ || fp->uf_tml_ਕੁੱਲ == ਨਿਰਾ
						    || fp->uf_tml_self == ਨਿਰਾ)
	    ਵਾਪਸ;	    // out of memory
	fp->uf_prof_initialized = TRUE;
    }

    fp->uf_profiling = TRUE;
}

/*
 * Save time when starting to invoke another script or function.
 */
    ਸਥਿਰ ਖਾਲੀ
script_prof_save(
    proftime_T	*tm)	    // place to store wait time
{
    scriptitem_T    *si;

    ਜੇ (SCRIPT_ID_VALID(current_sctx.sc_sid))
    {
	si = SCRIPT_ਵਸਤੂ(current_sctx.sc_sid);
	ਜੇ (si->sn_prof_on && si->sn_pr_nest++ == 0)
	    profile_ਸ਼ੁਰੂ(&si->sn_pr_ਬੱਚਾ);
    }
    profile_get_wait(tm);
}

/*
 * When calling a function: may initialize for profiling.
 */
    ਖਾਲੀ
profile_may_ਸ਼ੁਰੂ_func(profinfo_T *info, ufunc_T *fp, ufunc_T *caller)
{
    ਜੇ (!fp->uf_profiling && has_profiling(FALSE, fp->uf_name, ਨਿਰਾ,
								&fp->uf_hash))
    {
	info->pi_started_profiling = TRUE;
	func_do_profile(fp);
    }
    ਜੇ (fp->uf_profiling || (caller != ਨਿਰਾ && caller->uf_profiling))
    {
	++fp->uf_tm_ਗਿਣਤੀ;
	profile_ਸ਼ੁਰੂ(&info->pi_call_ਸ਼ੁਰੂ);
	profile_zero(&fp->uf_tm_children);
    }
    script_prof_save(&info->pi_wait_ਸ਼ੁਰੂ);
}

/*
 * After calling a function: may handle profiling.  profile_may_start_func()
 * must have been called previously.
 */
    ਖਾਲੀ
profile_may_ਸਿਰਾ_func(profinfo_T *info, ufunc_T *fp, ufunc_T *caller)
{
    profile_ਸਿਰਾ(&info->pi_call_ਸ਼ੁਰੂ);
    profile_sub_wait(&info->pi_wait_ਸ਼ੁਰੂ, &info->pi_call_ਸ਼ੁਰੂ);
    profile_ਜੋੜੋ(&fp->uf_tm_ਕੁੱਲ, &info->pi_call_ਸ਼ੁਰੂ);
    profile_self(&fp->uf_tm_self, &info->pi_call_ਸ਼ੁਰੂ, &fp->uf_tm_children);
    ਜੇ (caller != ਨਿਰਾ && caller->uf_profiling)
    {
	profile_ਜੋੜੋ(&caller->uf_tm_children, &info->pi_call_ਸ਼ੁਰੂ);
	profile_ਜੋੜੋ(&caller->uf_tml_children, &info->pi_call_ਸ਼ੁਰੂ);
    }
    ਜੇ (info->pi_started_profiling)
	// make a ":profdel func" stop profiling the function
	fp->uf_profiling = FALSE;
}

/*
 * Prepare profiling for entering a child or something else that is not
 * counted for the script/function itself.
 * Should always be called in pair with prof_child_exit().
 */
    ਖਾਲੀ
prof_ਬੱਚਾ_enter(
    proftime_T *tm)	// place to store waittime
{
    funccall_T *fc = get_current_funccal();

    ਜੇ (fc != ਨਿਰਾ && fc->fc_func->uf_profiling)
	profile_ਸ਼ੁਰੂ(&fc->fc_prof_ਬੱਚਾ);
    script_prof_save(tm);
}

/*
 * Take care of time spent in a child.
 * Should always be called after prof_child_enter().
 */
    ਖਾਲੀ
prof_ਬੱਚਾ_exit(
    proftime_T *tm)	// where waittime was stored
{
    funccall_T *fc = get_current_funccal();

    ਜੇ (fc != ਨਿਰਾ && fc->fc_func->uf_profiling)
    {
	profile_ਸਿਰਾ(&fc->fc_prof_ਬੱਚਾ);
	profile_sub_wait(tm, &fc->fc_prof_ਬੱਚਾ); // don't count waiting time
	profile_ਜੋੜੋ(&fc->fc_func->uf_tm_children, &fc->fc_prof_ਬੱਚਾ);
	profile_ਜੋੜੋ(&fc->fc_func->uf_tml_children, &fc->fc_prof_ਬੱਚਾ);
    }
    script_prof_restore(tm);
}

/*
 * Called when starting to read a function line.
 * "sourcing_lnum" must be correct!
 * When skipping lines it may not actually be executed, but we won't find out
 * until later and we need to store the time now.
 */
    ਖਾਲੀ
func_ਸਤਰ_ਸ਼ੁਰੂ(ਖਾਲੀ *cookie, ਲੰਮਾ lnum)
{
    funccall_T	*fcp = (funccall_T *)cookie;
    ufunc_T	*fp = fcp->fc_func;

    ਜੇ (fp->uf_profiling && lnum >= 1 && lnum <= fp->uf_ਸਤਰਾਂ.ga_len)
    {
	fp->uf_tml_idx = lnum - 1;
	// Skip continuation lines.
	ਜਦੋਂ (fp->uf_tml_idx > 0 && FUNCLINE(fp, fp->uf_tml_idx) == ਨਿਰਾ)
	    --fp->uf_tml_idx;
	fp->uf_tml_execed = FALSE;
	profile_ਸ਼ੁਰੂ(&fp->uf_tml_ਸ਼ੁਰੂ);
	profile_zero(&fp->uf_tml_children);
	profile_get_wait(&fp->uf_tml_wait);
    }
}

/*
 * Called when actually executing a function line.
 */
    ਖਾਲੀ
func_ਸਤਰ_exec(ਖਾਲੀ *cookie)
{
    funccall_T	*fcp = (funccall_T *)cookie;
    ufunc_T	*fp = fcp->fc_func;

    ਜੇ (fp->uf_profiling && fp->uf_tml_idx >= 0)
	fp->uf_tml_execed = TRUE;
}

/*
 * Called when done with a function line.
 */
    ਖਾਲੀ
func_ਸਤਰ_ਸਿਰਾ(ਖਾਲੀ *cookie)
{
    funccall_T	*fcp = (funccall_T *)cookie;
    ufunc_T	*fp = fcp->fc_func;

    ਜੇ (fp->uf_profiling && fp->uf_tml_idx >= 0)
    {
	ਜੇ (fp->uf_tml_execed)
	{
	    ++fp->uf_tml_ਗਿਣਤੀ[fp->uf_tml_idx];
	    profile_ਸਿਰਾ(&fp->uf_tml_ਸ਼ੁਰੂ);
	    profile_sub_wait(&fp->uf_tml_wait, &fp->uf_tml_ਸ਼ੁਰੂ);
	    profile_ਜੋੜੋ(&fp->uf_tml_ਕੁੱਲ[fp->uf_tml_idx], &fp->uf_tml_ਸ਼ੁਰੂ);
	    profile_self(&fp->uf_tml_self[fp->uf_tml_idx], &fp->uf_tml_ਸ਼ੁਰੂ,
							&fp->uf_tml_children);
	}
	fp->uf_tml_idx = -1;
    }
}

/*
 * Dump the profiling results for all functions in file "fd".
 */
    ਸਥਿਰ ਖਾਲੀ
func_dump_profile(FILE *fd)
{
    hashtab_T	*functbl;
    hashitem_T	*hi;
    ਪੂਰਨ		todo;
    ufunc_T	*fp;
    ਪੂਰਨ		i;
    ufunc_T	**sorttab;
    ਪੂਰਨ		st_len = 0;
    char_u	*p;

    functbl = func_tbl_get();
    todo = (ਪੂਰਨ)functbl->ht_ਵਰਤਿਆ;
    ਜੇ (todo == 0)
	ਵਾਪਸ;     // nothing to dump

    sorttab = ALLOC_MULT(ufunc_T *, todo);

    FOR_ALL_HASHTAB_ਚੀਜ਼ਾਂ(functbl, hi, todo)
    {
	ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	{
	    --todo;
	    fp = HI2UF(hi);
	    ਜੇ (fp->uf_prof_initialized)
	    {
		ਜੇ (sorttab != ਨਿਰਾ)
		    sorttab[st_len++] = fp;

		ਜੇ (fp->uf_name[0] == K_SPECIAL)
		    fprintf(fd, "FUNCTION  <SNR>%s()\n", fp->uf_name + 3);
		ਨਹੀਂਤਾਂ
		    fprintf(fd, "FUNCTION  %s()\n", fp->uf_name);
		ਜੇ (fp->uf_script_ctx.sc_sid > 0)
		{
		    p = home_replace_save(ਨਿਰਾ,
				     get_scriptname(fp->uf_script_ctx.sc_sid));
		    ਜੇ (p != ਨਿਰਾ)
		    {
			fprintf(fd, "    Defined: %s:%ld\n",
					   p, (ਲੰਮਾ)fp->uf_script_ctx.sc_lnum);
			vim_free(p);
		    }
		}
		ਜੇ (fp->uf_tm_ਗਿਣਤੀ == 1)
		    fprintf(fd, "Called 1 time\n");
		ਨਹੀਂਤਾਂ
		    fprintf(fd, "Called %d times\n", fp->uf_tm_ਗਿਣਤੀ);
		fprintf(fd, "Total time: %s\n", profile_ਸੁਨੇਹਾ(&fp->uf_tm_ਕੁੱਲ));
		fprintf(fd, " Self time: %s\n", profile_ਸੁਨੇਹਾ(&fp->uf_tm_self));
		fprintf(fd, "\n");
		fprintf(fd, "%s\n", PROF_TOTALS_ਸਿਰਲੇਖ);

		ਲਈ (i = 0; i < fp->uf_ਸਤਰਾਂ.ga_len; ++i)
		{
		    ਜੇ (FUNCLINE(fp, i) == ਨਿਰਾ)
			ਜਾਰੀ;
		    prof_func_ਸਤਰ(fd, fp->uf_tml_ਗਿਣਤੀ[i],
			     &fp->uf_tml_ਕੁੱਲ[i], &fp->uf_tml_self[i], TRUE);
		    fprintf(fd, "%s\n", FUNCLINE(fp, i));
		}
		fprintf(fd, "\n");
	    }
	}
    }

    ਜੇ (sorttab != ਨਿਰਾ && st_len > 0)
    {
	qsort((ਖਾਲੀ *)sorttab, (size_t)st_len, ਆਕਾਰ(ufunc_T *),
							      prof_ਕੁੱਲ_cmp);
	prof_ਛਾਂਟੋ_list(fd, sorttab, st_len, "TOTAL", FALSE);
	qsort((ਖਾਲੀ *)sorttab, (size_t)st_len, ਆਕਾਰ(ufunc_T *),
							      prof_self_cmp);
	prof_ਛਾਂਟੋ_list(fd, sorttab, st_len, "SELF", TRUE);
    }

    vim_free(sorttab);
}

/*
 * Start profiling script "fp".
 */
    ਖਾਲੀ
script_do_profile(scriptitem_T *si)
{
    si->sn_pr_ਗਿਣਤੀ = 0;
    profile_zero(&si->sn_pr_ਕੁੱਲ);
    profile_zero(&si->sn_pr_self);

    ga_init2(&si->sn_prl_ga, ਆਕਾਰ(sn_prl_T), 100);
    si->sn_prl_idx = -1;
    si->sn_prof_on = TRUE;
    si->sn_pr_nest = 0;
}

/*
 * Count time spent in children after invoking another script or function.
 */
    ਖਾਲੀ
script_prof_restore(proftime_T *tm)
{
    scriptitem_T    *si;

    ਜੇ (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	ਵਾਪਸ;

    si = SCRIPT_ਵਸਤੂ(current_sctx.sc_sid);
    ਜੇ (si->sn_prof_on && --si->sn_pr_nest == 0)
    {
	profile_ਸਿਰਾ(&si->sn_pr_ਬੱਚਾ);
	profile_sub_wait(tm, &si->sn_pr_ਬੱਚਾ); // don't count wait time
	profile_ਜੋੜੋ(&si->sn_pr_children, &si->sn_pr_ਬੱਚਾ);
	profile_ਜੋੜੋ(&si->sn_prl_children, &si->sn_pr_ਬੱਚਾ);
    }
}

/*
 * Dump the profiling results for all scripts in file "fd".
 */
    ਸਥਿਰ ਖਾਲੀ
script_dump_profile(FILE *fd)
{
    ਪੂਰਨ		    id;
    scriptitem_T    *si;
    ਪੂਰਨ		    i;
    FILE	    *sfd;
    sn_prl_T	    *pp;

    ਲਈ (id = 1; id <= script_ਚੀਜ਼ਾਂ.ga_len; ++id)
    {
	si = SCRIPT_ਵਸਤੂ(id);
	ਜੇ (si->sn_prof_on)
	{
	    fprintf(fd, "SCRIPT  %s\n", si->sn_name);
	    ਜੇ (si->sn_pr_ਗਿਣਤੀ == 1)
		fprintf(fd, "Sourced 1 time\n");
	    ਨਹੀਂਤਾਂ
		fprintf(fd, "Sourced %d times\n", si->sn_pr_ਗਿਣਤੀ);
	    fprintf(fd, "Total time: %s\n", profile_ਸੁਨੇਹਾ(&si->sn_pr_ਕੁੱਲ));
	    fprintf(fd, " Self time: %s\n", profile_ਸੁਨੇਹਾ(&si->sn_pr_self));
	    fprintf(fd, "\n");
	    fprintf(fd, "%s\n", PROF_TOTALS_ਸਿਰਲੇਖ);

	    sfd = mch_fopen((ਅੱਖਰ *)si->sn_name, "r");
	    ਜੇ (sfd == ਨਿਰਾ)
		fprintf(fd, "Cannot open file!\n");
	    ਨਹੀਂਤਾਂ
	    {
		// Keep going till the end of file, so that trailing
		// continuation lines are listed.
		ਲਈ (i = 0; ; ++i)
		{
		    ਜੇ (vim_fgets(IObuff, IOSIZE, sfd))
			ਰੋਕੋ;
		    // When a line has been truncated, append NL, taking care
		    // of multi-byte characters .
		    ਜੇ (IObuff[IOSIZE - 2] != NUL && IObuff[IOSIZE - 2] != NL)
		    {
			ਪੂਰਨ n = IOSIZE - 2;

			ਜੇ (enc_utf8)
			{
			    // Move to the first byte of this char.
			    // utf_head_off() doesn't work, because it checks
			    // for a truncated character.
			    ਜਦੋਂ (n > 0 && (IObuff[n] & 0xc0) == 0x80)
				--n;
			}
			ਨਹੀਂਤਾਂ ਜੇ (has_mbyte)
			    n -= mb_ਸਿਰ_off(IObuff, IObuff + n);
			IObuff[n] = NL;
			IObuff[n + 1] = NUL;
		    }
		    ਜੇ (i < si->sn_prl_ga.ga_len
				     && (pp = &PRL_ਵਸਤੂ(si, i))->snp_ਗਿਣਤੀ > 0)
		    {
			fprintf(fd, "%5d ", pp->snp_ਗਿਣਤੀ);
			ਜੇ (profile_equal(&pp->sn_prl_ਕੁੱਲ, &pp->sn_prl_self))
			    fprintf(fd, "           ");
			ਨਹੀਂਤਾਂ
			    fprintf(fd, "%s ", profile_ਸੁਨੇਹਾ(&pp->sn_prl_ਕੁੱਲ));
			fprintf(fd, "%s ", profile_ਸੁਨੇਹਾ(&pp->sn_prl_self));
		    }
		    ਨਹੀਂਤਾਂ
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
    ਖਾਲੀ
profile_dump(ਖਾਲੀ)
{
    FILE	*fd;

    ਜੇ (profile_fname == ਨਿਰਾ)
	ਵਾਪਸ;

    fd = mch_fopen((ਅੱਖਰ *)profile_fname, "w");
    ਜੇ (fd == ਨਿਰਾ)
	semsg(_(e_cant_open_file_str), profile_fname);
    ਨਹੀਂਤਾਂ
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
    ਖਾਲੀ
script_ਸਤਰ_ਸ਼ੁਰੂ(ਖਾਲੀ)
{
    scriptitem_T    *si;
    sn_prl_T	    *pp;

    ਜੇ (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	ਵਾਪਸ;
    si = SCRIPT_ਵਸਤੂ(current_sctx.sc_sid);
    ਜੇ (si->sn_prof_on && SOURCING_LNUM >= 1)
    {
	// Grow the array before starting the timer, so that the time spent
	// here isn't counted.
	(ਖਾਲੀ)ga_grow(&si->sn_prl_ga,
				  (ਪੂਰਨ)(SOURCING_LNUM - si->sn_prl_ga.ga_len));
	si->sn_prl_idx = SOURCING_LNUM - 1;
	ਜਦੋਂ (si->sn_prl_ga.ga_len <= si->sn_prl_idx
		&& si->sn_prl_ga.ga_len < si->sn_prl_ga.ga_maxlen)
	{
	    // Zero counters for a line that was not used before.
	    pp = &PRL_ਵਸਤੂ(si, si->sn_prl_ga.ga_len);
	    pp->snp_ਗਿਣਤੀ = 0;
	    profile_zero(&pp->sn_prl_ਕੁੱਲ);
	    profile_zero(&pp->sn_prl_self);
	    ++si->sn_prl_ga.ga_len;
	}
	si->sn_prl_execed = FALSE;
	profile_ਸ਼ੁਰੂ(&si->sn_prl_ਸ਼ੁਰੂ);
	profile_zero(&si->sn_prl_children);
	profile_get_wait(&si->sn_prl_wait);
    }
}

/*
 * Called when actually executing a function line.
 */
    ਖਾਲੀ
script_ਸਤਰ_exec(ਖਾਲੀ)
{
    scriptitem_T    *si;

    ਜੇ (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	ਵਾਪਸ;
    si = SCRIPT_ਵਸਤੂ(current_sctx.sc_sid);
    ਜੇ (si->sn_prof_on && si->sn_prl_idx >= 0)
	si->sn_prl_execed = TRUE;
}

/*
 * Called when done with a script line.
 */
    ਖਾਲੀ
script_ਸਤਰ_ਸਿਰਾ(ਖਾਲੀ)
{
    scriptitem_T    *si;
    sn_prl_T	    *pp;

    ਜੇ (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	ਵਾਪਸ;
    si = SCRIPT_ਵਸਤੂ(current_sctx.sc_sid);
    ਜੇ (si->sn_prof_on && si->sn_prl_idx >= 0
				     && si->sn_prl_idx < si->sn_prl_ga.ga_len)
    {
	ਜੇ (si->sn_prl_execed)
	{
	    pp = &PRL_ਵਸਤੂ(si, si->sn_prl_idx);
	    ++pp->snp_ਗਿਣਤੀ;
	    profile_ਸਿਰਾ(&si->sn_prl_ਸ਼ੁਰੂ);
	    profile_sub_wait(&si->sn_prl_wait, &si->sn_prl_ਸ਼ੁਰੂ);
	    profile_ਜੋੜੋ(&pp->sn_prl_ਕੁੱਲ, &si->sn_prl_ਸ਼ੁਰੂ);
	    profile_self(&pp->sn_prl_self, &si->sn_prl_ਸ਼ੁਰੂ,
							&si->sn_prl_children);
	}
	si->sn_prl_idx = -1;
    }
}
# endif // FEAT_PROFILE

#ਜੇ_ਅੰਤ
