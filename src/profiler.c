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

#അന്തര്ഭാവ "vim.h"

#എങ്കിൽ defined(FEAT_EVAL)
# എങ്കിൽ defined(FEAT_PROFILE) || defined(FEAT_RELTIME)
/*
 * Store the current time in "tm".
 */
    ശൂന്യം
profile_start(proftime_T *tm)
{
#  ifdef MSWIN
    QueryPerformanceCounter(tm);
#  അല്ലെങ്കിൽ
    PROF_GET_TIME(tm);
#  endif
}

/*
 * Compute the elapsed time from "tm" till now and store in "tm".
 */
    ശൂന്യം
profile_end(proftime_T *tm)
{
    proftime_T now;

#  ifdef MSWIN
    QueryPerformanceCounter(&now);
    tm->QuadPart = now.QuadPart - tm->QuadPart;
#  അല്ലെങ്കിൽ
    PROF_GET_TIME(&now);
    tm->tv_fsec = now.tv_fsec - tm->tv_fsec;
    tm->tv_sec = now.tv_sec - tm->tv_sec;
    എങ്കിൽ (tm->tv_fsec < 0)
    {
	tm->tv_fsec += TV_FSEC_SEC;
	--tm->tv_sec;
    }
#  endif
}

/*
 * Subtract the time "tm2" from "tm".
 */
    ശൂന്യം
profile_sub(proftime_T *tm, proftime_T *tm2)
{
#  ifdef MSWIN
    tm->QuadPart -= tm2->QuadPart;
#  അല്ലെങ്കിൽ
    tm->tv_fsec -= tm2->tv_fsec;
    tm->tv_sec -= tm2->tv_sec;
    എങ്കിൽ (tm->tv_fsec < 0)
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
    അക്ഷരം *
profile_സന്ദേശം(proftime_T *tm)
{
    സ്ഥിതം അക്ഷരം buf[50];

#  ifdef MSWIN
    LARGE_INTEGER   fr;

    QueryPerformanceFrequency(&fr);
    sprintf(buf, "%10.6lf", (ഇരട്ട)tm->QuadPart / (ഇരട്ട)fr.QuadPart);
#  അല്ലെങ്കിൽ
    sprintf(buf, PROF_TIME_FORMAT, (long)tm->tv_sec, (long)tm->tv_fsec);
#  endif
    തിരികെ buf;
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
    തിരികെ (float_T)tm->QuadPart / (float_T)fr.QuadPart;
#  അല്ലെങ്കിൽ
    തിരികെ (float_T)tm->tv_sec + (float_T)tm->tv_fsec / (float_T)TV_FSEC_SEC;
#  endif
}

/*
 * Put the time "msec" past now in "tm".
 */
    ശൂന്യം
profile_setlimit(long msec, proftime_T *tm)
{
    എങ്കിൽ (msec <= 0)   // no limit
	profile_zero(tm);
    അല്ലെങ്കിൽ
    {
#  ifdef MSWIN
	LARGE_INTEGER   fr;

	QueryPerformanceCounter(tm);
	QueryPerformanceFrequency(&fr);
	tm->QuadPart += (LONGLONG)((ഇരട്ട)msec / 1000.0 * (ഇരട്ട)fr.QuadPart);
#  അല്ലെങ്കിൽ
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
    പൂർണ്ണസംഖ്യ
profile_passed_limit(proftime_T *tm)
{
    proftime_T	now;

#  ifdef MSWIN
    എങ്കിൽ (tm->QuadPart == 0)  // timer was not set
	തിരികെ FALSE;
    QueryPerformanceCounter(&now);
    തിരികെ (now.QuadPart > tm->QuadPart);
#  അല്ലെങ്കിൽ
    എങ്കിൽ (tm->tv_sec == 0)    // timer was not set
	തിരികെ FALSE;
    PROF_GET_TIME(&now);
    തിരികെ (now.tv_sec > tm->tv_sec
	    || (now.tv_sec == tm->tv_sec && now.tv_fsec > tm->tv_fsec));
#  endif
}

/*
 * Set the time in "tm" to zero.
 */
    ശൂന്യം
profile_zero(proftime_T *tm)
{
#  ifdef MSWIN
    tm->QuadPart = 0;
#  അല്ലെങ്കിൽ
    tm->tv_fsec = 0;
    tm->tv_sec = 0;
#  endif
}

# endif  // FEAT_PROFILE || FEAT_RELTIME

# എങ്കിൽ defined(FEAT_SYN_HL) && defined(FEAT_RELTIME) && defined(FEAT_PROFILE)
#  എങ്കിൽ defined(HAVE_MATH_H)
#   include <math.h>
#  endif

/*
 * Divide the time "tm" by "count" and store in "tm2".
 */
    ശൂന്യം
profile_divide(proftime_T *tm, പൂർണ്ണസംഖ്യ എണ്ണം, proftime_T *tm2)
{
    എങ്കിൽ (എണ്ണം == 0)
	profile_zero(tm2);
    അല്ലെങ്കിൽ
    {
#  ifdef MSWIN
	tm2->QuadPart = tm->QuadPart / എണ്ണം;
#  അല്ലെങ്കിൽ
	ഇരട്ട fsec = (tm->tv_sec * (float_T)TV_FSEC_SEC + tm->tv_fsec)
								       / എണ്ണം;

	tm2->tv_sec = floor(fsec / (float_T)TV_FSEC_SEC);
	tm2->tv_fsec = vim_round(fsec - (tm2->tv_sec * (float_T)TV_FSEC_SEC));
#  endif
    }
}
# endif

# എങ്കിൽ defined(FEAT_PROFILE)
/*
 * Functions for profiling.
 */
സ്ഥിതം proftime_T prof_wait_time;

/*
 * Add the time "tm2" to "tm".
 */
    ശൂന്യം
profile_add(proftime_T *tm, proftime_T *tm2)
{
#  ifdef MSWIN
    tm->QuadPart += tm2->QuadPart;
#  അല്ലെങ്കിൽ
    tm->tv_fsec += tm2->tv_fsec;
    tm->tv_sec += tm2->tv_sec;
    എങ്കിൽ (tm->tv_fsec >= TV_FSEC_SEC)
    {
	tm->tv_fsec -= TV_FSEC_SEC;
	++tm->tv_sec;
    }
#  endif
}

/*
 * Add the "self" time from the total time and the children's time.
 */
    ശൂന്യം
profile_self(proftime_T *self, proftime_T *total, proftime_T *children)
{
    // Check that the result won't be negative.  Can happen with recursive
    // calls.
#  ifdef MSWIN
    എങ്കിൽ (total->QuadPart <= children->QuadPart)
	തിരികെ;
#  അല്ലെങ്കിൽ
    എങ്കിൽ (total->tv_sec < children->tv_sec
	    || (total->tv_sec == children->tv_sec
		&& total->tv_fsec <= children->tv_fsec))
	തിരികെ;
#  endif
    profile_add(self, total);
    profile_sub(self, children);
}

/*
 * Get the current waittime.
 */
    സ്ഥിതം ശൂന്യം
profile_get_wait(proftime_T *tm)
{
    *tm = prof_wait_time;
}

/*
 * Subtract the passed waittime since "tm" from "tma".
 */
    ശൂന്യം
profile_sub_wait(proftime_T *tm, proftime_T *tma)
{
    proftime_T tm3 = prof_wait_time;

    profile_sub(&tm3, tm);
    profile_sub(tma, &tm3);
}

/*
 * Return TRUE if "tm1" and "tm2" are equal.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
profile_equal(proftime_T *tm1, proftime_T *tm2)
{
#  ifdef MSWIN
    തിരികെ (tm1->QuadPart == tm2->QuadPart);
#  അല്ലെങ്കിൽ
    തിരികെ (tm1->tv_fsec == tm2->tv_fsec && tm1->tv_sec == tm2->tv_sec);
#  endif
}

/*
 * Return <0, 0 or >0 if "tm1" < "tm2", "tm1" == "tm2" or "tm1" > "tm2"
 */
    പൂർണ്ണസംഖ്യ
profile_cmp(സ്ഥിരം proftime_T *tm1, സ്ഥിരം proftime_T *tm2)
{
#  ifdef MSWIN
    തിരികെ tm2->QuadPart == tm1->QuadPart ? 0 :
	tm2->QuadPart > tm1->QuadPart ? 1 : -1;
#  അല്ലെങ്കിൽ
    എങ്കിൽ (tm1->tv_sec == tm2->tv_sec)
	തിരികെ tm2->tv_fsec == tm1->tv_fsec ? 0 :
	    tm2->tv_fsec > tm1->tv_fsec ? 1 : -1;
    തിരികെ tm2->tv_sec > tm1->tv_sec ? 1 : -1;
#  endif
}

സ്ഥിതം char_u	*profile_fname = ശൂന്യ;
സ്ഥിതം proftime_T pause_time;

/*
 * Reset all profiling information.
 */
    സ്ഥിതം ശൂന്യം
profile_reset(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		id;
    പൂർണ്ണസംഖ്യ		todo;
    hashtab_T	*functbl;
    hashitem_T	*hi;

    // Reset sourced files.
    വേണ്ടി (id = 1; id <= script_ഇനങ്ങൾ.ga_len; id++)
    {
	scriptitem_T *si = SCRIPT_ITEM(id);
	എങ്കിൽ (si->sn_prof_on)
	{
	    si->sn_prof_on      = FALSE;
	    si->sn_pr_force     = FALSE;
	    profile_zero(&si->sn_pr_child);
	    si->sn_pr_nest      = 0;
	    si->sn_pr_എണ്ണം     = 0;
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
    todo = (പൂർണ്ണസംഖ്യ)functbl->ht_used;

    FOR_ALL_HASHTAB_ഇനങ്ങൾ(functbl, hi, todo)
    {
	ufunc_T *fp;
	പൂർണ്ണസംഖ്യ	i;

	എങ്കിൽ (HASHITEM_EMPTY(hi))
	    തുടരുക;

	todo--;
	fp = HI2UF(hi);
	എങ്കിൽ (fp->uf_prof_initialized)
	{
	    fp->uf_profiling    = 0;
	    fp->uf_prof_initialized = FALSE;
	    fp->uf_tm_എണ്ണം     = 0;
	    profile_zero(&fp->uf_tm_total);
	    profile_zero(&fp->uf_tm_self);
	    profile_zero(&fp->uf_tm_children);

	    വേണ്ടി (i = 0; i < fp->uf_വരികൾ.ga_len; i++)
	    {
		fp->uf_tml_എണ്ണം[i] = 0;
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
    ശൂന്യം
ex_profile(exarg_T *eap)
{
    char_u	*e;
    പൂർണ്ണസംഖ്യ		len;

    e = skiptowhite(eap->arg);
    len = (പൂർണ്ണസംഖ്യ)(e - eap->arg);
    e = skipwhite(e);

    എങ്കിൽ (len == 5 && STRNCMP(eap->arg, "start", 5) == 0 && *e != NUL)
    {
	VIM_CLEAR(profile_fname);
	profile_fname = expand_env_save_opt(e, TRUE, ശൂന്യ);
	do_profiling = PROF_YES;
	profile_zero(&prof_wait_time);
	set_vim_var_nr(VV_PROFILING, 1L);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (do_profiling == PROF_NONE)
	emsg(_(e_first_use_profile_start_fname));
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(eap->arg, "stop") == 0)
    {
	profile_dump();
	do_profiling = PROF_NONE;
	set_vim_var_nr(VV_PROFILING, 0L);
	profile_reset();
    }
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(eap->arg, "pause") == 0)
    {
	എങ്കിൽ (do_profiling == PROF_YES)
	    profile_start(&pause_time);
	do_profiling = PROF_PAUSED;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(eap->arg, "continue") == 0)
    {
	എങ്കിൽ (do_profiling == PROF_PAUSED)
	{
	    profile_end(&pause_time);
	    profile_add(&prof_wait_time, &pause_time);
	}
	do_profiling = PROF_YES;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(eap->arg, "dump") == 0)
	profile_dump();
    അല്ലെങ്കിൽ
    {
	// The rest is similar to ":breakadd".
	ex_breakadd(eap);
    }
}

// Command line expansion for :profile.
സ്ഥിതം എണ്ണൽ
{
    PEXP_SUBCMD,	// expand :profile sub-commands
    PEXP_FUNC		// expand :profile func {funcname}
} pexpand_what;

സ്ഥിതം അക്ഷരം *pexpand_cmds[] = {
			"start",
			"stop",
			"pause",
			"continue",
			"func",
			"file",
			"dump",
			ശൂന്യ
};

/*
 * Function given to ExpandGeneric() to obtain the profile command
 * specific expansion.
 */
    char_u *
get_profile_name(expand_T *xp UNUSED, പൂർണ്ണസംഖ്യ idx)
{
    തിരഞ്ഞെടുപ്പ് (pexpand_what)
    {
    അവസ്ഥ PEXP_SUBCMD:
	തിരികെ (char_u *)pexpand_cmds[idx];
    സ്ഥിരസ്ഥിതി:
	തിരികെ ശൂന്യ;
    }
}

/*
 * Handle command line completion for :profile command.
 */
    ശൂന്യം
set_context_in_profile_cmd(expand_T *xp, char_u *arg)
{
    char_u	*end_subcmd;

    // Default: expand subcommands.
    xp->xp_context = EXPAND_PROFILE;
    pexpand_what = PEXP_SUBCMD;
    xp->xp_pattern = arg;

    end_subcmd = skiptowhite(arg);
    എങ്കിൽ (*end_subcmd == NUL)
	തിരികെ;

    എങ്കിൽ ((end_subcmd - arg == 5 && STRNCMP(arg, "start", 5) == 0)
	    || (end_subcmd - arg == 4 && STRNCMP(arg, "file", 4) == 0))
    {
	xp->xp_context = EXPAND_FILES;
	xp->xp_pattern = skipwhite(end_subcmd);
	തിരികെ;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (end_subcmd - arg == 4 && STRNCMP(arg, "func", 4) == 0)
    {
	xp->xp_context = EXPAND_USER_FUNC;
	xp->xp_pattern = skipwhite(end_subcmd);
	തിരികെ;
    }

    xp->xp_context = EXPAND_NOTHING;
}

സ്ഥിതം proftime_T inchar_time;

/*
 * Called when starting to wait for the user to type a character.
 */
    ശൂന്യം
prof_inchar_enter(ശൂന്യം)
{
    profile_start(&inchar_time);
}

/*
 * Called when finished waiting for the user to type a character.
 */
    ശൂന്യം
prof_inchar_exit(ശൂന്യം)
{
    profile_end(&inchar_time);
    profile_add(&prof_wait_time, &inchar_time);
}


/*
 * Return TRUE when a function defined in the current script should be
 * profiled.
 */
    പൂർണ്ണസംഖ്യ
prof_def_func(ശൂന്യം)
{
    എങ്കിൽ (current_sctx.sc_sid > 0)
	തിരികെ SCRIPT_ITEM(current_sctx.sc_sid)->sn_pr_force;
    തിരികെ FALSE;
}

/*
 * Print the count and times for one function or function line.
 */
    സ്ഥിതം ശൂന്യം
prof_func_വരി(
    FILE	*fd,
    പൂർണ്ണസംഖ്യ		എണ്ണം,
    proftime_T	*total,
    proftime_T	*self,
    പൂർണ്ണസംഖ്യ		prefer_self)	// when equal print only self time
{
    എങ്കിൽ (എണ്ണം > 0)
    {
	fprintf(fd, "%5d ", എണ്ണം);
	എങ്കിൽ (prefer_self && profile_equal(total, self))
	    fprintf(fd, PROF_TIME_BLANK);
	അല്ലെങ്കിൽ
	    fprintf(fd, "%s ", profile_സന്ദേശം(total));
	എങ്കിൽ (!prefer_self && profile_equal(total, self))
	    fprintf(fd, PROF_TIME_BLANK);
	അല്ലെങ്കിൽ
	    fprintf(fd, "%s ", profile_സന്ദേശം(self));
    }
    അല്ലെങ്കിൽ
	fprintf(fd, "      %s%s", PROF_TIME_BLANK, PROF_TIME_BLANK);
}

    സ്ഥിതം ശൂന്യം
prof_sort_list(
    FILE	*fd,
    ufunc_T	**sorttab,
    പൂർണ്ണസംഖ്യ		st_len,
    അക്ഷരം	*title,
    പൂർണ്ണസംഖ്യ		prefer_self)	// when equal print only self time
{
    പൂർണ്ണസംഖ്യ		i;
    ufunc_T	*fp;

    fprintf(fd, "FUNCTIONS SORTED ON %s TIME\n", title);
    fprintf(fd, "%s  function\n", PROF_TOTALS_HEADER);
    വേണ്ടി (i = 0; i < 20 && i < st_len; ++i)
    {
	fp = sorttab[i];
	prof_func_വരി(fd, fp->uf_tm_എണ്ണം, &fp->uf_tm_total, &fp->uf_tm_self,
								 prefer_self);
	എങ്കിൽ (fp->uf_name[0] == K_SPECIAL)
	    fprintf(fd, " <SNR>%s()\n", fp->uf_name + 3);
	അല്ലെങ്കിൽ
	    fprintf(fd, " %s()\n", fp->uf_name);
    }
    fprintf(fd, "\n");
}

/*
 * Compare function for total time sorting.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
prof_total_cmp(സ്ഥിരം ശൂന്യം *s1, സ്ഥിരം ശൂന്യം *s2)
{
    ufunc_T	*p1, *p2;

    p1 = *(ufunc_T **)s1;
    p2 = *(ufunc_T **)s2;
    തിരികെ profile_cmp(&p1->uf_tm_total, &p2->uf_tm_total);
}

/*
 * Compare function for self time sorting.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
prof_self_cmp(സ്ഥിരം ശൂന്യം *s1, സ്ഥിരം ശൂന്യം *s2)
{
    ufunc_T	*p1, *p2;

    p1 = *(ufunc_T **)s1;
    p2 = *(ufunc_T **)s2;
    തിരികെ profile_cmp(&p1->uf_tm_self, &p2->uf_tm_self);
}

/*
 * Start profiling function "fp".
 */
    ശൂന്യം
func_do_profile(ufunc_T *fp)
{
    പൂർണ്ണസംഖ്യ		len = fp->uf_വരികൾ.ga_len;

    എങ്കിൽ (!fp->uf_prof_initialized)
    {
	എങ്കിൽ (len == 0)
	    len = 1;  // avoid getting error for allocating zero bytes
	fp->uf_tm_എണ്ണം = 0;
	profile_zero(&fp->uf_tm_self);
	profile_zero(&fp->uf_tm_total);
	എങ്കിൽ (fp->uf_tml_എണ്ണം == ശൂന്യ)
	    fp->uf_tml_എണ്ണം = ALLOC_CLEAR_MULT(പൂർണ്ണസംഖ്യ, len);
	എങ്കിൽ (fp->uf_tml_total == ശൂന്യ)
	    fp->uf_tml_total = ALLOC_CLEAR_MULT(proftime_T, len);
	എങ്കിൽ (fp->uf_tml_self == ശൂന്യ)
	    fp->uf_tml_self = ALLOC_CLEAR_MULT(proftime_T, len);
	fp->uf_tml_idx = -1;
	എങ്കിൽ (fp->uf_tml_എണ്ണം == ശൂന്യ || fp->uf_tml_total == ശൂന്യ
						    || fp->uf_tml_self == ശൂന്യ)
	    തിരികെ;	    // out of memory
	fp->uf_prof_initialized = TRUE;
    }

    fp->uf_profiling = TRUE;
}

/*
 * Save time when starting to invoke another script or function.
 */
    സ്ഥിതം ശൂന്യം
script_prof_save(
    proftime_T	*tm)	    // place to store wait time
{
    scriptitem_T    *si;

    എങ്കിൽ (SCRIPT_ID_VALID(current_sctx.sc_sid))
    {
	si = SCRIPT_ITEM(current_sctx.sc_sid);
	എങ്കിൽ (si->sn_prof_on && si->sn_pr_nest++ == 0)
	    profile_start(&si->sn_pr_child);
    }
    profile_get_wait(tm);
}

/*
 * When calling a function: may initialize for profiling.
 */
    ശൂന്യം
profile_may_start_func(profinfo_T *info, ufunc_T *fp, ufunc_T *caller)
{
    എങ്കിൽ (!fp->uf_profiling && has_profiling(FALSE, fp->uf_name, ശൂന്യ,
								&fp->uf_hash))
    {
	info->pi_started_profiling = TRUE;
	func_do_profile(fp);
    }
    എങ്കിൽ (fp->uf_profiling || (caller != ശൂന്യ && caller->uf_profiling))
    {
	++fp->uf_tm_എണ്ണം;
	profile_start(&info->pi_call_start);
	profile_zero(&fp->uf_tm_children);
    }
    script_prof_save(&info->pi_wait_start);
}

/*
 * After calling a function: may handle profiling.  profile_may_start_func()
 * must have been called previously.
 */
    ശൂന്യം
profile_may_end_func(profinfo_T *info, ufunc_T *fp, ufunc_T *caller)
{
    profile_end(&info->pi_call_start);
    profile_sub_wait(&info->pi_wait_start, &info->pi_call_start);
    profile_add(&fp->uf_tm_total, &info->pi_call_start);
    profile_self(&fp->uf_tm_self, &info->pi_call_start, &fp->uf_tm_children);
    എങ്കിൽ (caller != ശൂന്യ && caller->uf_profiling)
    {
	profile_add(&caller->uf_tm_children, &info->pi_call_start);
	profile_add(&caller->uf_tml_children, &info->pi_call_start);
    }
    എങ്കിൽ (info->pi_started_profiling)
	// make a ":profdel func" stop profiling the function
	fp->uf_profiling = FALSE;
}

/*
 * Prepare profiling for entering a child or something else that is not
 * counted for the script/function itself.
 * Should always be called in pair with prof_child_exit().
 */
    ശൂന്യം
prof_child_enter(
    proftime_T *tm)	// place to store waittime
{
    funccall_T *fc = get_current_funccal();

    എങ്കിൽ (fc != ശൂന്യ && fc->fc_func->uf_profiling)
	profile_start(&fc->fc_prof_child);
    script_prof_save(tm);
}

/*
 * Take care of time spent in a child.
 * Should always be called after prof_child_enter().
 */
    ശൂന്യം
prof_child_exit(
    proftime_T *tm)	// where waittime was stored
{
    funccall_T *fc = get_current_funccal();

    എങ്കിൽ (fc != ശൂന്യ && fc->fc_func->uf_profiling)
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
    ശൂന്യം
func_വരി_start(ശൂന്യം *cookie, long lnum)
{
    funccall_T	*fcp = (funccall_T *)cookie;
    ufunc_T	*fp = fcp->fc_func;

    എങ്കിൽ (fp->uf_profiling && lnum >= 1 && lnum <= fp->uf_വരികൾ.ga_len)
    {
	fp->uf_tml_idx = lnum - 1;
	// Skip continuation lines.
	ആകുമ്പോൾ (fp->uf_tml_idx > 0 && FUNCLINE(fp, fp->uf_tml_idx) == ശൂന്യ)
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
    ശൂന്യം
func_വരി_exec(ശൂന്യം *cookie)
{
    funccall_T	*fcp = (funccall_T *)cookie;
    ufunc_T	*fp = fcp->fc_func;

    എങ്കിൽ (fp->uf_profiling && fp->uf_tml_idx >= 0)
	fp->uf_tml_execed = TRUE;
}

/*
 * Called when done with a function line.
 */
    ശൂന്യം
func_വരി_end(ശൂന്യം *cookie)
{
    funccall_T	*fcp = (funccall_T *)cookie;
    ufunc_T	*fp = fcp->fc_func;

    എങ്കിൽ (fp->uf_profiling && fp->uf_tml_idx >= 0)
    {
	എങ്കിൽ (fp->uf_tml_execed)
	{
	    ++fp->uf_tml_എണ്ണം[fp->uf_tml_idx];
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
    സ്ഥിതം ശൂന്യം
func_dump_profile(FILE *fd)
{
    hashtab_T	*functbl;
    hashitem_T	*hi;
    പൂർണ്ണസംഖ്യ		todo;
    ufunc_T	*fp;
    പൂർണ്ണസംഖ്യ		i;
    ufunc_T	**sorttab;
    പൂർണ്ണസംഖ്യ		st_len = 0;
    char_u	*p;

    functbl = func_tbl_get();
    todo = (പൂർണ്ണസംഖ്യ)functbl->ht_used;
    എങ്കിൽ (todo == 0)
	തിരികെ;     // nothing to dump

    sorttab = ALLOC_MULT(ufunc_T *, todo);

    FOR_ALL_HASHTAB_ഇനങ്ങൾ(functbl, hi, todo)
    {
	എങ്കിൽ (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    fp = HI2UF(hi);
	    എങ്കിൽ (fp->uf_prof_initialized)
	    {
		എങ്കിൽ (sorttab != ശൂന്യ)
		    sorttab[st_len++] = fp;

		എങ്കിൽ (fp->uf_name[0] == K_SPECIAL)
		    fprintf(fd, "FUNCTION  <SNR>%s()\n", fp->uf_name + 3);
		അല്ലെങ്കിൽ
		    fprintf(fd, "FUNCTION  %s()\n", fp->uf_name);
		എങ്കിൽ (fp->uf_script_ctx.sc_sid > 0)
		{
		    p = home_replace_save(ശൂന്യ,
				     get_scriptname(fp->uf_script_ctx.sc_sid));
		    എങ്കിൽ (p != ശൂന്യ)
		    {
			fprintf(fd, "    Defined: %s:%ld\n",
					   p, (long)fp->uf_script_ctx.sc_lnum);
			vim_free(p);
		    }
		}
		എങ്കിൽ (fp->uf_tm_എണ്ണം == 1)
		    fprintf(fd, "Called 1 time\n");
		അല്ലെങ്കിൽ
		    fprintf(fd, "Called %d times\n", fp->uf_tm_എണ്ണം);
		fprintf(fd, "Total time: %s\n", profile_സന്ദേശം(&fp->uf_tm_total));
		fprintf(fd, " Self time: %s\n", profile_സന്ദേശം(&fp->uf_tm_self));
		fprintf(fd, "\n");
		fprintf(fd, "%s\n", PROF_TOTALS_HEADER);

		വേണ്ടി (i = 0; i < fp->uf_വരികൾ.ga_len; ++i)
		{
		    എങ്കിൽ (FUNCLINE(fp, i) == ശൂന്യ)
			തുടരുക;
		    prof_func_വരി(fd, fp->uf_tml_എണ്ണം[i],
			     &fp->uf_tml_total[i], &fp->uf_tml_self[i], TRUE);
		    fprintf(fd, "%s\n", FUNCLINE(fp, i));
		}
		fprintf(fd, "\n");
	    }
	}
    }

    എങ്കിൽ (sorttab != ശൂന്യ && st_len > 0)
    {
	qsort((ശൂന്യം *)sorttab, (size_t)st_len, വലുപ്പം(ufunc_T *),
							      prof_total_cmp);
	prof_sort_list(fd, sorttab, st_len, "TOTAL", FALSE);
	qsort((ശൂന്യം *)sorttab, (size_t)st_len, വലുപ്പം(ufunc_T *),
							      prof_self_cmp);
	prof_sort_list(fd, sorttab, st_len, "SELF", TRUE);
    }

    vim_free(sorttab);
}

/*
 * Start profiling script "fp".
 */
    ശൂന്യം
script_do_profile(scriptitem_T *si)
{
    si->sn_pr_എണ്ണം = 0;
    profile_zero(&si->sn_pr_total);
    profile_zero(&si->sn_pr_self);

    ga_init2(&si->sn_prl_ga, വലുപ്പം(sn_prl_T), 100);
    si->sn_prl_idx = -1;
    si->sn_prof_on = TRUE;
    si->sn_pr_nest = 0;
}

/*
 * Count time spent in children after invoking another script or function.
 */
    ശൂന്യം
script_prof_restore(proftime_T *tm)
{
    scriptitem_T    *si;

    എങ്കിൽ (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	തിരികെ;

    si = SCRIPT_ITEM(current_sctx.sc_sid);
    എങ്കിൽ (si->sn_prof_on && --si->sn_pr_nest == 0)
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
    സ്ഥിതം ശൂന്യം
script_dump_profile(FILE *fd)
{
    പൂർണ്ണസംഖ്യ		    id;
    scriptitem_T    *si;
    പൂർണ്ണസംഖ്യ		    i;
    FILE	    *sfd;
    sn_prl_T	    *pp;

    വേണ്ടി (id = 1; id <= script_ഇനങ്ങൾ.ga_len; ++id)
    {
	si = SCRIPT_ITEM(id);
	എങ്കിൽ (si->sn_prof_on)
	{
	    fprintf(fd, "SCRIPT  %s\n", si->sn_name);
	    എങ്കിൽ (si->sn_pr_എണ്ണം == 1)
		fprintf(fd, "Sourced 1 time\n");
	    അല്ലെങ്കിൽ
		fprintf(fd, "Sourced %d times\n", si->sn_pr_എണ്ണം);
	    fprintf(fd, "Total time: %s\n", profile_സന്ദേശം(&si->sn_pr_total));
	    fprintf(fd, " Self time: %s\n", profile_സന്ദേശം(&si->sn_pr_self));
	    fprintf(fd, "\n");
	    fprintf(fd, "%s\n", PROF_TOTALS_HEADER);

	    sfd = mch_fopen((അക്ഷരം *)si->sn_name, "r");
	    എങ്കിൽ (sfd == ശൂന്യ)
		fprintf(fd, "Cannot open file!\n");
	    അല്ലെങ്കിൽ
	    {
		// Keep going till the end of file, so that trailing
		// continuation lines are listed.
		വേണ്ടി (i = 0; ; ++i)
		{
		    എങ്കിൽ (vim_fgets(IObuff, IOSIZE, sfd))
			നിർത്തുക;
		    // When a line has been truncated, append NL, taking care
		    // of multi-byte characters .
		    എങ്കിൽ (IObuff[IOSIZE - 2] != NUL && IObuff[IOSIZE - 2] != NL)
		    {
			പൂർണ്ണസംഖ്യ n = IOSIZE - 2;

			എങ്കിൽ (enc_utf8)
			{
			    // Move to the first byte of this char.
			    // utf_head_off() doesn't work, because it checks
			    // for a truncated character.
			    ആകുമ്പോൾ (n > 0 && (IObuff[n] & 0xc0) == 0x80)
				--n;
			}
			അല്ലെങ്കിൽ എങ്കിൽ (has_mbyte)
			    n -= mb_head_off(IObuff, IObuff + n);
			IObuff[n] = NL;
			IObuff[n + 1] = NUL;
		    }
		    എങ്കിൽ (i < si->sn_prl_ga.ga_len
				     && (pp = &PRL_ITEM(si, i))->snp_എണ്ണം > 0)
		    {
			fprintf(fd, "%5d ", pp->snp_എണ്ണം);
			എങ്കിൽ (profile_equal(&pp->sn_prl_total, &pp->sn_prl_self))
			    fprintf(fd, "           ");
			അല്ലെങ്കിൽ
			    fprintf(fd, "%s ", profile_സന്ദേശം(&pp->sn_prl_total));
			fprintf(fd, "%s ", profile_സന്ദേശം(&pp->sn_prl_self));
		    }
		    അല്ലെങ്കിൽ
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
    ശൂന്യം
profile_dump(ശൂന്യം)
{
    FILE	*fd;

    എങ്കിൽ (profile_fname == ശൂന്യ)
	തിരികെ;

    fd = mch_fopen((അക്ഷരം *)profile_fname, "w");
    എങ്കിൽ (fd == ശൂന്യ)
	semsg(_(e_cant_open_file_str), profile_fname);
    അല്ലെങ്കിൽ
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
    ശൂന്യം
script_വരി_start(ശൂന്യം)
{
    scriptitem_T    *si;
    sn_prl_T	    *pp;

    എങ്കിൽ (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	തിരികെ;
    si = SCRIPT_ITEM(current_sctx.sc_sid);
    എങ്കിൽ (si->sn_prof_on && SOURCING_LNUM >= 1)
    {
	// Grow the array before starting the timer, so that the time spent
	// here isn't counted.
	(ശൂന്യം)ga_grow(&si->sn_prl_ga,
				  (പൂർണ്ണസംഖ്യ)(SOURCING_LNUM - si->sn_prl_ga.ga_len));
	si->sn_prl_idx = SOURCING_LNUM - 1;
	ആകുമ്പോൾ (si->sn_prl_ga.ga_len <= si->sn_prl_idx
		&& si->sn_prl_ga.ga_len < si->sn_prl_ga.ga_maxlen)
	{
	    // Zero counters for a line that was not used before.
	    pp = &PRL_ITEM(si, si->sn_prl_ga.ga_len);
	    pp->snp_എണ്ണം = 0;
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
    ശൂന്യം
script_വരി_exec(ശൂന്യം)
{
    scriptitem_T    *si;

    എങ്കിൽ (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	തിരികെ;
    si = SCRIPT_ITEM(current_sctx.sc_sid);
    എങ്കിൽ (si->sn_prof_on && si->sn_prl_idx >= 0)
	si->sn_prl_execed = TRUE;
}

/*
 * Called when done with a script line.
 */
    ശൂന്യം
script_വരി_end(ശൂന്യം)
{
    scriptitem_T    *si;
    sn_prl_T	    *pp;

    എങ്കിൽ (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	തിരികെ;
    si = SCRIPT_ITEM(current_sctx.sc_sid);
    എങ്കിൽ (si->sn_prof_on && si->sn_prl_idx >= 0
				     && si->sn_prl_idx < si->sn_prl_ga.ga_len)
    {
	എങ്കിൽ (si->sn_prl_execed)
	{
	    pp = &PRL_ITEM(si, si->sn_prl_idx);
	    ++pp->snp_എണ്ണം;
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
