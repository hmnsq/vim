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

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಯದಿ defined(FEAT_EVAL)
# ಯದಿ defined(FEAT_PROFILE) || defined(FEAT_RELTIME)
/*
 * Store the current time in "tm".
 */
    ಶೂನ್ಯ
profile_start(proftime_T *tm)
{
#  ifdef MSWIN
    QueryPerformanceCounter(tm);
#  ಅನ್ಯಥಾ
    PROF_GET_TIME(tm);
#  endif
}

/*
 * Compute the elapsed time from "tm" till now and store in "tm".
 */
    ಶೂನ್ಯ
profile_end(proftime_T *tm)
{
    proftime_T now;

#  ifdef MSWIN
    QueryPerformanceCounter(&now);
    tm->QuadPart = now.QuadPart - tm->QuadPart;
#  ಅನ್ಯಥಾ
    PROF_GET_TIME(&now);
    tm->tv_fsec = now.tv_fsec - tm->tv_fsec;
    tm->tv_sec = now.tv_sec - tm->tv_sec;
    ಯದಿ (tm->tv_fsec < 0)
    {
	tm->tv_fsec += TV_FSEC_SEC;
	--tm->tv_sec;
    }
#  endif
}

/*
 * Subtract the time "tm2" from "tm".
 */
    ಶೂನ್ಯ
profile_sub(proftime_T *tm, proftime_T *tm2)
{
#  ifdef MSWIN
    tm->QuadPart -= tm2->QuadPart;
#  ಅನ್ಯಥಾ
    tm->tv_fsec -= tm2->tv_fsec;
    tm->tv_sec -= tm2->tv_sec;
    ಯದಿ (tm->tv_fsec < 0)
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
    ವರ್ಣ *
profile_ಸಂದೇಶ(proftime_T *tm)
{
    ಸ್ಥಿರ ವರ್ಣ buf[50];

#  ifdef MSWIN
    LARGE_INTEGER   fr;

    QueryPerformanceFrequency(&fr);
    sprintf(buf, "%10.6lf", (ದ್ವಿಗುಣ)tm->QuadPart / (ದ್ವಿಗುಣ)fr.QuadPart);
#  ಅನ್ಯಥಾ
    sprintf(buf, PROF_TIME_FORMAT, (long)tm->tv_sec, (long)tm->tv_fsec);
#  endif
    ಪ್ರತ್ಯಾಗಮ buf;
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
    ಪ್ರತ್ಯಾಗಮ (float_T)tm->QuadPart / (float_T)fr.QuadPart;
#  ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ (float_T)tm->tv_sec + (float_T)tm->tv_fsec / (float_T)TV_FSEC_SEC;
#  endif
}

/*
 * Put the time "msec" past now in "tm".
 */
    ಶೂನ್ಯ
profile_setlimit(long msec, proftime_T *tm)
{
    ಯದಿ (msec <= 0)   // no limit
	profile_zero(tm);
    ಅನ್ಯಥಾ
    {
#  ifdef MSWIN
	LARGE_INTEGER   fr;

	QueryPerformanceCounter(tm);
	QueryPerformanceFrequency(&fr);
	tm->QuadPart += (LONGLONG)((ದ್ವಿಗುಣ)msec / 1000.0 * (ದ್ವಿಗುಣ)fr.QuadPart);
#  ಅನ್ಯಥಾ
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
    ಪೂರ್ಣಾಙ್ಕ
profile_passed_limit(proftime_T *tm)
{
    proftime_T	now;

#  ifdef MSWIN
    ಯದಿ (tm->QuadPart == 0)  // timer was not set
	ಪ್ರತ್ಯಾಗಮ FALSE;
    QueryPerformanceCounter(&now);
    ಪ್ರತ್ಯಾಗಮ (now.QuadPart > tm->QuadPart);
#  ಅನ್ಯಥಾ
    ಯದಿ (tm->tv_sec == 0)    // timer was not set
	ಪ್ರತ್ಯಾಗಮ FALSE;
    PROF_GET_TIME(&now);
    ಪ್ರತ್ಯಾಗಮ (now.tv_sec > tm->tv_sec
	    || (now.tv_sec == tm->tv_sec && now.tv_fsec > tm->tv_fsec));
#  endif
}

/*
 * Set the time in "tm" to zero.
 */
    ಶೂನ್ಯ
profile_zero(proftime_T *tm)
{
#  ifdef MSWIN
    tm->QuadPart = 0;
#  ಅನ್ಯಥಾ
    tm->tv_fsec = 0;
    tm->tv_sec = 0;
#  endif
}

# endif  // FEAT_PROFILE || FEAT_RELTIME

# ಯದಿ defined(FEAT_SYN_HL) && defined(FEAT_RELTIME) && defined(FEAT_PROFILE)
#  ಯದಿ defined(HAVE_MATH_H)
#   include <math.h>
#  endif

/*
 * Divide the time "tm" by "count" and store in "tm2".
 */
    ಶೂನ್ಯ
profile_divide(proftime_T *tm, ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ, proftime_T *tm2)
{
    ಯದಿ (ಎಣಿಕೆ == 0)
	profile_zero(tm2);
    ಅನ್ಯಥಾ
    {
#  ifdef MSWIN
	tm2->QuadPart = tm->QuadPart / ಎಣಿಕೆ;
#  ಅನ್ಯಥಾ
	ದ್ವಿಗುಣ fsec = (tm->tv_sec * (float_T)TV_FSEC_SEC + tm->tv_fsec)
								       / ಎಣಿಕೆ;

	tm2->tv_sec = floor(fsec / (float_T)TV_FSEC_SEC);
	tm2->tv_fsec = vim_round(fsec - (tm2->tv_sec * (float_T)TV_FSEC_SEC));
#  endif
    }
}
# endif

# ಯದಿ defined(FEAT_PROFILE)
/*
 * Functions for profiling.
 */
ಸ್ಥಿರ proftime_T prof_wait_time;

/*
 * Add the time "tm2" to "tm".
 */
    ಶೂನ್ಯ
profile_add(proftime_T *tm, proftime_T *tm2)
{
#  ifdef MSWIN
    tm->QuadPart += tm2->QuadPart;
#  ಅನ್ಯಥಾ
    tm->tv_fsec += tm2->tv_fsec;
    tm->tv_sec += tm2->tv_sec;
    ಯದಿ (tm->tv_fsec >= TV_FSEC_SEC)
    {
	tm->tv_fsec -= TV_FSEC_SEC;
	++tm->tv_sec;
    }
#  endif
}

/*
 * Add the "self" time from the total time and the children's time.
 */
    ಶೂನ್ಯ
profile_self(proftime_T *self, proftime_T *total, proftime_T *children)
{
    // Check that the result won't be negative.  Can happen with recursive
    // calls.
#  ifdef MSWIN
    ಯದಿ (total->QuadPart <= children->QuadPart)
	ಪ್ರತ್ಯಾಗಮ;
#  ಅನ್ಯಥಾ
    ಯದಿ (total->tv_sec < children->tv_sec
	    || (total->tv_sec == children->tv_sec
		&& total->tv_fsec <= children->tv_fsec))
	ಪ್ರತ್ಯಾಗಮ;
#  endif
    profile_add(self, total);
    profile_sub(self, children);
}

/*
 * Get the current waittime.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
profile_get_wait(proftime_T *tm)
{
    *tm = prof_wait_time;
}

/*
 * Subtract the passed waittime since "tm" from "tma".
 */
    ಶೂನ್ಯ
profile_sub_wait(proftime_T *tm, proftime_T *tma)
{
    proftime_T tm3 = prof_wait_time;

    profile_sub(&tm3, tm);
    profile_sub(tma, &tm3);
}

/*
 * Return TRUE if "tm1" and "tm2" are equal.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
profile_equal(proftime_T *tm1, proftime_T *tm2)
{
#  ifdef MSWIN
    ಪ್ರತ್ಯಾಗಮ (tm1->QuadPart == tm2->QuadPart);
#  ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ (tm1->tv_fsec == tm2->tv_fsec && tm1->tv_sec == tm2->tv_sec);
#  endif
}

/*
 * Return <0, 0 or >0 if "tm1" < "tm2", "tm1" == "tm2" or "tm1" > "tm2"
 */
    ಪೂರ್ಣಾಙ್ಕ
profile_cmp(ಧ್ರುವ proftime_T *tm1, ಧ್ರುವ proftime_T *tm2)
{
#  ifdef MSWIN
    ಪ್ರತ್ಯಾಗಮ tm2->QuadPart == tm1->QuadPart ? 0 :
	tm2->QuadPart > tm1->QuadPart ? 1 : -1;
#  ಅನ್ಯಥಾ
    ಯದಿ (tm1->tv_sec == tm2->tv_sec)
	ಪ್ರತ್ಯಾಗಮ tm2->tv_fsec == tm1->tv_fsec ? 0 :
	    tm2->tv_fsec > tm1->tv_fsec ? 1 : -1;
    ಪ್ರತ್ಯಾಗಮ tm2->tv_sec > tm1->tv_sec ? 1 : -1;
#  endif
}

ಸ್ಥಿರ char_u	*profile_fname = ರಿಕ್ತ;
ಸ್ಥಿರ proftime_T pause_time;

/*
 * Reset all profiling information.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
profile_reset(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		id;
    ಪೂರ್ಣಾಙ್ಕ		todo;
    hashtab_T	*functbl;
    hashitem_T	*hi;

    // Reset sourced files.
    ಪ್ರತಿ (id = 1; id <= script_ವಸ್ತುಗಳು.ga_len; id++)
    {
	scriptitem_T *si = SCRIPT_ITEM(id);
	ಯದಿ (si->sn_prof_on)
	{
	    si->sn_prof_on      = FALSE;
	    si->sn_pr_force     = FALSE;
	    profile_zero(&si->sn_pr_child);
	    si->sn_pr_nest      = 0;
	    si->sn_pr_ಎಣಿಕೆ     = 0;
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
    todo = (ಪೂರ್ಣಾಙ್ಕ)functbl->ht_used;

    FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(functbl, hi, todo)
    {
	ufunc_T *fp;
	ಪೂರ್ಣಾಙ್ಕ	i;

	ಯದಿ (HASHITEM_EMPTY(hi))
	    ಅನುವರ್ತ;

	todo--;
	fp = HI2UF(hi);
	ಯದಿ (fp->uf_prof_initialized)
	{
	    fp->uf_profiling    = 0;
	    fp->uf_prof_initialized = FALSE;
	    fp->uf_tm_ಎಣಿಕೆ     = 0;
	    profile_zero(&fp->uf_tm_total);
	    profile_zero(&fp->uf_tm_self);
	    profile_zero(&fp->uf_tm_children);

	    ಪ್ರತಿ (i = 0; i < fp->uf_ಸಾಲುಗಳು.ga_len; i++)
	    {
		fp->uf_tml_ಎಣಿಕೆ[i] = 0;
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
    ಶೂನ್ಯ
ex_profile(exarg_T *eap)
{
    char_u	*e;
    ಪೂರ್ಣಾಙ್ಕ		len;

    e = skiptowhite(eap->arg);
    len = (ಪೂರ್ಣಾಙ್ಕ)(e - eap->arg);
    e = skipwhite(e);

    ಯದಿ (len == 5 && STRNCMP(eap->arg, "start", 5) == 0 && *e != NUL)
    {
	VIM_CLEAR(profile_fname);
	profile_fname = expand_env_save_opt(e, TRUE, ರಿಕ್ತ);
	do_profiling = PROF_YES;
	profile_zero(&prof_wait_time);
	set_vim_var_nr(VV_PROFILING, 1L);
    }
    ಅನ್ಯಥಾ ಯದಿ (do_profiling == PROF_NONE)
	emsg(_(e_first_use_profile_start_fname));
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(eap->arg, "stop") == 0)
    {
	profile_dump();
	do_profiling = PROF_NONE;
	set_vim_var_nr(VV_PROFILING, 0L);
	profile_reset();
    }
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(eap->arg, "pause") == 0)
    {
	ಯದಿ (do_profiling == PROF_YES)
	    profile_start(&pause_time);
	do_profiling = PROF_PAUSED;
    }
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(eap->arg, "continue") == 0)
    {
	ಯದಿ (do_profiling == PROF_PAUSED)
	{
	    profile_end(&pause_time);
	    profile_add(&prof_wait_time, &pause_time);
	}
	do_profiling = PROF_YES;
    }
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(eap->arg, "dump") == 0)
	profile_dump();
    ಅನ್ಯಥಾ
    {
	// The rest is similar to ":breakadd".
	ex_breakadd(eap);
    }
}

// Command line expansion for :profile.
ಸ್ಥಿರ ಗಣನಾ
{
    PEXP_SUBCMD,	// expand :profile sub-commands
    PEXP_FUNC		// expand :profile func {funcname}
} pexpand_what;

ಸ್ಥಿರ ವರ್ಣ *pexpand_cmds[] = {
			"start",
			"stop",
			"pause",
			"continue",
			"func",
			"file",
			"dump",
			ರಿಕ್ತ
};

/*
 * Function given to ExpandGeneric() to obtain the profile command
 * specific expansion.
 */
    char_u *
get_profile_name(expand_T *xp UNUSED, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ವಿಕಲ್ಪ (pexpand_what)
    {
    ಪಕ್ಷ PEXP_SUBCMD:
	ಪ್ರತ್ಯಾಗಮ (char_u *)pexpand_cmds[idx];
    ಸ್ವಭಾವ:
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
}

/*
 * Handle command line completion for :profile command.
 */
    ಶೂನ್ಯ
set_context_in_profile_cmd(expand_T *xp, char_u *arg)
{
    char_u	*end_subcmd;

    // Default: expand subcommands.
    xp->xp_context = EXPAND_PROFILE;
    pexpand_what = PEXP_SUBCMD;
    xp->xp_pattern = arg;

    end_subcmd = skiptowhite(arg);
    ಯದಿ (*end_subcmd == NUL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ ((end_subcmd - arg == 5 && STRNCMP(arg, "start", 5) == 0)
	    || (end_subcmd - arg == 4 && STRNCMP(arg, "file", 4) == 0))
    {
	xp->xp_context = EXPAND_FILES;
	xp->xp_pattern = skipwhite(end_subcmd);
	ಪ್ರತ್ಯಾಗಮ;
    }
    ಅನ್ಯಥಾ ಯದಿ (end_subcmd - arg == 4 && STRNCMP(arg, "func", 4) == 0)
    {
	xp->xp_context = EXPAND_USER_FUNC;
	xp->xp_pattern = skipwhite(end_subcmd);
	ಪ್ರತ್ಯಾಗಮ;
    }

    xp->xp_context = EXPAND_NOTHING;
}

ಸ್ಥಿರ proftime_T inchar_time;

/*
 * Called when starting to wait for the user to type a character.
 */
    ಶೂನ್ಯ
prof_inchar_enter(ಶೂನ್ಯ)
{
    profile_start(&inchar_time);
}

/*
 * Called when finished waiting for the user to type a character.
 */
    ಶೂನ್ಯ
prof_inchar_exit(ಶೂನ್ಯ)
{
    profile_end(&inchar_time);
    profile_add(&prof_wait_time, &inchar_time);
}


/*
 * Return TRUE when a function defined in the current script should be
 * profiled.
 */
    ಪೂರ್ಣಾಙ್ಕ
prof_def_func(ಶೂನ್ಯ)
{
    ಯದಿ (current_sctx.sc_sid > 0)
	ಪ್ರತ್ಯಾಗಮ SCRIPT_ITEM(current_sctx.sc_sid)->sn_pr_force;
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Print the count and times for one function or function line.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
prof_func_ಸಾಲು(
    FILE	*fd,
    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ,
    proftime_T	*total,
    proftime_T	*self,
    ಪೂರ್ಣಾಙ್ಕ		prefer_self)	// when equal print only self time
{
    ಯದಿ (ಎಣಿಕೆ > 0)
    {
	fprintf(fd, "%5d ", ಎಣಿಕೆ);
	ಯದಿ (prefer_self && profile_equal(total, self))
	    fprintf(fd, PROF_TIME_BLANK);
	ಅನ್ಯಥಾ
	    fprintf(fd, "%s ", profile_ಸಂದೇಶ(total));
	ಯದಿ (!prefer_self && profile_equal(total, self))
	    fprintf(fd, PROF_TIME_BLANK);
	ಅನ್ಯಥಾ
	    fprintf(fd, "%s ", profile_ಸಂದೇಶ(self));
    }
    ಅನ್ಯಥಾ
	fprintf(fd, "      %s%s", PROF_TIME_BLANK, PROF_TIME_BLANK);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
prof_sort_list(
    FILE	*fd,
    ufunc_T	**sorttab,
    ಪೂರ್ಣಾಙ್ಕ		st_len,
    ವರ್ಣ	*title,
    ಪೂರ್ಣಾಙ್ಕ		prefer_self)	// when equal print only self time
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    ufunc_T	*fp;

    fprintf(fd, "FUNCTIONS SORTED ON %s TIME\n", title);
    fprintf(fd, "%s  function\n", PROF_TOTALS_HEADER);
    ಪ್ರತಿ (i = 0; i < 20 && i < st_len; ++i)
    {
	fp = sorttab[i];
	prof_func_ಸಾಲು(fd, fp->uf_tm_ಎಣಿಕೆ, &fp->uf_tm_total, &fp->uf_tm_self,
								 prefer_self);
	ಯದಿ (fp->uf_name[0] == K_SPECIAL)
	    fprintf(fd, " <SNR>%s()\n", fp->uf_name + 3);
	ಅನ್ಯಥಾ
	    fprintf(fd, " %s()\n", fp->uf_name);
    }
    fprintf(fd, "\n");
}

/*
 * Compare function for total time sorting.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
prof_total_cmp(ಧ್ರುವ ಶೂನ್ಯ *s1, ಧ್ರುವ ಶೂನ್ಯ *s2)
{
    ufunc_T	*p1, *p2;

    p1 = *(ufunc_T **)s1;
    p2 = *(ufunc_T **)s2;
    ಪ್ರತ್ಯಾಗಮ profile_cmp(&p1->uf_tm_total, &p2->uf_tm_total);
}

/*
 * Compare function for self time sorting.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
prof_self_cmp(ಧ್ರುವ ಶೂನ್ಯ *s1, ಧ್ರುವ ಶೂನ್ಯ *s2)
{
    ufunc_T	*p1, *p2;

    p1 = *(ufunc_T **)s1;
    p2 = *(ufunc_T **)s2;
    ಪ್ರತ್ಯಾಗಮ profile_cmp(&p1->uf_tm_self, &p2->uf_tm_self);
}

/*
 * Start profiling function "fp".
 */
    ಶೂನ್ಯ
func_do_profile(ufunc_T *fp)
{
    ಪೂರ್ಣಾಙ್ಕ		len = fp->uf_ಸಾಲುಗಳು.ga_len;

    ಯದಿ (!fp->uf_prof_initialized)
    {
	ಯದಿ (len == 0)
	    len = 1;  // avoid getting error for allocating zero bytes
	fp->uf_tm_ಎಣಿಕೆ = 0;
	profile_zero(&fp->uf_tm_self);
	profile_zero(&fp->uf_tm_total);
	ಯದಿ (fp->uf_tml_ಎಣಿಕೆ == ರಿಕ್ತ)
	    fp->uf_tml_ಎಣಿಕೆ = ALLOC_CLEAR_MULT(ಪೂರ್ಣಾಙ್ಕ, len);
	ಯದಿ (fp->uf_tml_total == ರಿಕ್ತ)
	    fp->uf_tml_total = ALLOC_CLEAR_MULT(proftime_T, len);
	ಯದಿ (fp->uf_tml_self == ರಿಕ್ತ)
	    fp->uf_tml_self = ALLOC_CLEAR_MULT(proftime_T, len);
	fp->uf_tml_idx = -1;
	ಯದಿ (fp->uf_tml_ಎಣಿಕೆ == ರಿಕ್ತ || fp->uf_tml_total == ರಿಕ್ತ
						    || fp->uf_tml_self == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;	    // out of memory
	fp->uf_prof_initialized = TRUE;
    }

    fp->uf_profiling = TRUE;
}

/*
 * Save time when starting to invoke another script or function.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
script_prof_save(
    proftime_T	*tm)	    // place to store wait time
{
    scriptitem_T    *si;

    ಯದಿ (SCRIPT_ID_VALID(current_sctx.sc_sid))
    {
	si = SCRIPT_ITEM(current_sctx.sc_sid);
	ಯದಿ (si->sn_prof_on && si->sn_pr_nest++ == 0)
	    profile_start(&si->sn_pr_child);
    }
    profile_get_wait(tm);
}

/*
 * When calling a function: may initialize for profiling.
 */
    ಶೂನ್ಯ
profile_may_start_func(profinfo_T *info, ufunc_T *fp, ufunc_T *caller)
{
    ಯದಿ (!fp->uf_profiling && has_profiling(FALSE, fp->uf_name, ರಿಕ್ತ,
								&fp->uf_hash))
    {
	info->pi_started_profiling = TRUE;
	func_do_profile(fp);
    }
    ಯದಿ (fp->uf_profiling || (caller != ರಿಕ್ತ && caller->uf_profiling))
    {
	++fp->uf_tm_ಎಣಿಕೆ;
	profile_start(&info->pi_call_start);
	profile_zero(&fp->uf_tm_children);
    }
    script_prof_save(&info->pi_wait_start);
}

/*
 * After calling a function: may handle profiling.  profile_may_start_func()
 * must have been called previously.
 */
    ಶೂನ್ಯ
profile_may_end_func(profinfo_T *info, ufunc_T *fp, ufunc_T *caller)
{
    profile_end(&info->pi_call_start);
    profile_sub_wait(&info->pi_wait_start, &info->pi_call_start);
    profile_add(&fp->uf_tm_total, &info->pi_call_start);
    profile_self(&fp->uf_tm_self, &info->pi_call_start, &fp->uf_tm_children);
    ಯದಿ (caller != ರಿಕ್ತ && caller->uf_profiling)
    {
	profile_add(&caller->uf_tm_children, &info->pi_call_start);
	profile_add(&caller->uf_tml_children, &info->pi_call_start);
    }
    ಯದಿ (info->pi_started_profiling)
	// make a ":profdel func" stop profiling the function
	fp->uf_profiling = FALSE;
}

/*
 * Prepare profiling for entering a child or something else that is not
 * counted for the script/function itself.
 * Should always be called in pair with prof_child_exit().
 */
    ಶೂನ್ಯ
prof_child_enter(
    proftime_T *tm)	// place to store waittime
{
    funccall_T *fc = get_current_funccal();

    ಯದಿ (fc != ರಿಕ್ತ && fc->fc_func->uf_profiling)
	profile_start(&fc->fc_prof_child);
    script_prof_save(tm);
}

/*
 * Take care of time spent in a child.
 * Should always be called after prof_child_enter().
 */
    ಶೂನ್ಯ
prof_child_exit(
    proftime_T *tm)	// where waittime was stored
{
    funccall_T *fc = get_current_funccal();

    ಯದಿ (fc != ರಿಕ್ತ && fc->fc_func->uf_profiling)
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
    ಶೂನ್ಯ
func_ಸಾಲು_start(ಶೂನ್ಯ *cookie, long lnum)
{
    funccall_T	*fcp = (funccall_T *)cookie;
    ufunc_T	*fp = fcp->fc_func;

    ಯದಿ (fp->uf_profiling && lnum >= 1 && lnum <= fp->uf_ಸಾಲುಗಳು.ga_len)
    {
	fp->uf_tml_idx = lnum - 1;
	// Skip continuation lines.
	ಯಾವತ್ (fp->uf_tml_idx > 0 && FUNCLINE(fp, fp->uf_tml_idx) == ರಿಕ್ತ)
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
    ಶೂನ್ಯ
func_ಸಾಲು_exec(ಶೂನ್ಯ *cookie)
{
    funccall_T	*fcp = (funccall_T *)cookie;
    ufunc_T	*fp = fcp->fc_func;

    ಯದಿ (fp->uf_profiling && fp->uf_tml_idx >= 0)
	fp->uf_tml_execed = TRUE;
}

/*
 * Called when done with a function line.
 */
    ಶೂನ್ಯ
func_ಸಾಲು_end(ಶೂನ್ಯ *cookie)
{
    funccall_T	*fcp = (funccall_T *)cookie;
    ufunc_T	*fp = fcp->fc_func;

    ಯದಿ (fp->uf_profiling && fp->uf_tml_idx >= 0)
    {
	ಯದಿ (fp->uf_tml_execed)
	{
	    ++fp->uf_tml_ಎಣಿಕೆ[fp->uf_tml_idx];
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
    ಸ್ಥಿರ ಶೂನ್ಯ
func_dump_profile(FILE *fd)
{
    hashtab_T	*functbl;
    hashitem_T	*hi;
    ಪೂರ್ಣಾಙ್ಕ		todo;
    ufunc_T	*fp;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ufunc_T	**sorttab;
    ಪೂರ್ಣಾಙ್ಕ		st_len = 0;
    char_u	*p;

    functbl = func_tbl_get();
    todo = (ಪೂರ್ಣಾಙ್ಕ)functbl->ht_used;
    ಯದಿ (todo == 0)
	ಪ್ರತ್ಯಾಗಮ;     // nothing to dump

    sorttab = ALLOC_MULT(ufunc_T *, todo);

    FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(functbl, hi, todo)
    {
	ಯದಿ (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    fp = HI2UF(hi);
	    ಯದಿ (fp->uf_prof_initialized)
	    {
		ಯದಿ (sorttab != ರಿಕ್ತ)
		    sorttab[st_len++] = fp;

		ಯದಿ (fp->uf_name[0] == K_SPECIAL)
		    fprintf(fd, "FUNCTION  <SNR>%s()\n", fp->uf_name + 3);
		ಅನ್ಯಥಾ
		    fprintf(fd, "FUNCTION  %s()\n", fp->uf_name);
		ಯದಿ (fp->uf_script_ctx.sc_sid > 0)
		{
		    p = home_replace_save(ರಿಕ್ತ,
				     get_scriptname(fp->uf_script_ctx.sc_sid));
		    ಯದಿ (p != ರಿಕ್ತ)
		    {
			fprintf(fd, "    Defined: %s:%ld\n",
					   p, (long)fp->uf_script_ctx.sc_lnum);
			vim_free(p);
		    }
		}
		ಯದಿ (fp->uf_tm_ಎಣಿಕೆ == 1)
		    fprintf(fd, "Called 1 time\n");
		ಅನ್ಯಥಾ
		    fprintf(fd, "Called %d times\n", fp->uf_tm_ಎಣಿಕೆ);
		fprintf(fd, "Total time: %s\n", profile_ಸಂದೇಶ(&fp->uf_tm_total));
		fprintf(fd, " Self time: %s\n", profile_ಸಂದೇಶ(&fp->uf_tm_self));
		fprintf(fd, "\n");
		fprintf(fd, "%s\n", PROF_TOTALS_HEADER);

		ಪ್ರತಿ (i = 0; i < fp->uf_ಸಾಲುಗಳು.ga_len; ++i)
		{
		    ಯದಿ (FUNCLINE(fp, i) == ರಿಕ್ತ)
			ಅನುವರ್ತ;
		    prof_func_ಸಾಲು(fd, fp->uf_tml_ಎಣಿಕೆ[i],
			     &fp->uf_tml_total[i], &fp->uf_tml_self[i], TRUE);
		    fprintf(fd, "%s\n", FUNCLINE(fp, i));
		}
		fprintf(fd, "\n");
	    }
	}
    }

    ಯದಿ (sorttab != ರಿಕ್ತ && st_len > 0)
    {
	qsort((ಶೂನ್ಯ *)sorttab, (size_t)st_len, ಪರಿಮಾಣ(ufunc_T *),
							      prof_total_cmp);
	prof_sort_list(fd, sorttab, st_len, "TOTAL", FALSE);
	qsort((ಶೂನ್ಯ *)sorttab, (size_t)st_len, ಪರಿಮಾಣ(ufunc_T *),
							      prof_self_cmp);
	prof_sort_list(fd, sorttab, st_len, "SELF", TRUE);
    }

    vim_free(sorttab);
}

/*
 * Start profiling script "fp".
 */
    ಶೂನ್ಯ
script_do_profile(scriptitem_T *si)
{
    si->sn_pr_ಎಣಿಕೆ = 0;
    profile_zero(&si->sn_pr_total);
    profile_zero(&si->sn_pr_self);

    ga_init2(&si->sn_prl_ga, ಪರಿಮಾಣ(sn_prl_T), 100);
    si->sn_prl_idx = -1;
    si->sn_prof_on = TRUE;
    si->sn_pr_nest = 0;
}

/*
 * Count time spent in children after invoking another script or function.
 */
    ಶೂನ್ಯ
script_prof_restore(proftime_T *tm)
{
    scriptitem_T    *si;

    ಯದಿ (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	ಪ್ರತ್ಯಾಗಮ;

    si = SCRIPT_ITEM(current_sctx.sc_sid);
    ಯದಿ (si->sn_prof_on && --si->sn_pr_nest == 0)
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
    ಸ್ಥಿರ ಶೂನ್ಯ
script_dump_profile(FILE *fd)
{
    ಪೂರ್ಣಾಙ್ಕ		    id;
    scriptitem_T    *si;
    ಪೂರ್ಣಾಙ್ಕ		    i;
    FILE	    *sfd;
    sn_prl_T	    *pp;

    ಪ್ರತಿ (id = 1; id <= script_ವಸ್ತುಗಳು.ga_len; ++id)
    {
	si = SCRIPT_ITEM(id);
	ಯದಿ (si->sn_prof_on)
	{
	    fprintf(fd, "SCRIPT  %s\n", si->sn_name);
	    ಯದಿ (si->sn_pr_ಎಣಿಕೆ == 1)
		fprintf(fd, "Sourced 1 time\n");
	    ಅನ್ಯಥಾ
		fprintf(fd, "Sourced %d times\n", si->sn_pr_ಎಣಿಕೆ);
	    fprintf(fd, "Total time: %s\n", profile_ಸಂದೇಶ(&si->sn_pr_total));
	    fprintf(fd, " Self time: %s\n", profile_ಸಂದೇಶ(&si->sn_pr_self));
	    fprintf(fd, "\n");
	    fprintf(fd, "%s\n", PROF_TOTALS_HEADER);

	    sfd = mch_fopen((ವರ್ಣ *)si->sn_name, "r");
	    ಯದಿ (sfd == ರಿಕ್ತ)
		fprintf(fd, "Cannot open file!\n");
	    ಅನ್ಯಥಾ
	    {
		// Keep going till the end of file, so that trailing
		// continuation lines are listed.
		ಪ್ರತಿ (i = 0; ; ++i)
		{
		    ಯದಿ (vim_fgets(IObuff, IOSIZE, sfd))
			ಭಙ್ಗ;
		    // When a line has been truncated, append NL, taking care
		    // of multi-byte characters .
		    ಯದಿ (IObuff[IOSIZE - 2] != NUL && IObuff[IOSIZE - 2] != NL)
		    {
			ಪೂರ್ಣಾಙ್ಕ n = IOSIZE - 2;

			ಯದಿ (enc_utf8)
			{
			    // Move to the first byte of this char.
			    // utf_head_off() doesn't work, because it checks
			    // for a truncated character.
			    ಯಾವತ್ (n > 0 && (IObuff[n] & 0xc0) == 0x80)
				--n;
			}
			ಅನ್ಯಥಾ ಯದಿ (has_mbyte)
			    n -= mb_head_off(IObuff, IObuff + n);
			IObuff[n] = NL;
			IObuff[n + 1] = NUL;
		    }
		    ಯದಿ (i < si->sn_prl_ga.ga_len
				     && (pp = &PRL_ITEM(si, i))->snp_ಎಣಿಕೆ > 0)
		    {
			fprintf(fd, "%5d ", pp->snp_ಎಣಿಕೆ);
			ಯದಿ (profile_equal(&pp->sn_prl_total, &pp->sn_prl_self))
			    fprintf(fd, "           ");
			ಅನ್ಯಥಾ
			    fprintf(fd, "%s ", profile_ಸಂದೇಶ(&pp->sn_prl_total));
			fprintf(fd, "%s ", profile_ಸಂದೇಶ(&pp->sn_prl_self));
		    }
		    ಅನ್ಯಥಾ
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
    ಶೂನ್ಯ
profile_dump(ಶೂನ್ಯ)
{
    FILE	*fd;

    ಯದಿ (profile_fname == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    fd = mch_fopen((ವರ್ಣ *)profile_fname, "w");
    ಯದಿ (fd == ರಿಕ್ತ)
	semsg(_(e_cant_open_file_str), profile_fname);
    ಅನ್ಯಥಾ
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
    ಶೂನ್ಯ
script_ಸಾಲು_start(ಶೂನ್ಯ)
{
    scriptitem_T    *si;
    sn_prl_T	    *pp;

    ಯದಿ (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	ಪ್ರತ್ಯಾಗಮ;
    si = SCRIPT_ITEM(current_sctx.sc_sid);
    ಯದಿ (si->sn_prof_on && SOURCING_LNUM >= 1)
    {
	// Grow the array before starting the timer, so that the time spent
	// here isn't counted.
	(ಶೂನ್ಯ)ga_grow(&si->sn_prl_ga,
				  (ಪೂರ್ಣಾಙ್ಕ)(SOURCING_LNUM - si->sn_prl_ga.ga_len));
	si->sn_prl_idx = SOURCING_LNUM - 1;
	ಯಾವತ್ (si->sn_prl_ga.ga_len <= si->sn_prl_idx
		&& si->sn_prl_ga.ga_len < si->sn_prl_ga.ga_maxlen)
	{
	    // Zero counters for a line that was not used before.
	    pp = &PRL_ITEM(si, si->sn_prl_ga.ga_len);
	    pp->snp_ಎಣಿಕೆ = 0;
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
    ಶೂನ್ಯ
script_ಸಾಲು_exec(ಶೂನ್ಯ)
{
    scriptitem_T    *si;

    ಯದಿ (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	ಪ್ರತ್ಯಾಗಮ;
    si = SCRIPT_ITEM(current_sctx.sc_sid);
    ಯದಿ (si->sn_prof_on && si->sn_prl_idx >= 0)
	si->sn_prl_execed = TRUE;
}

/*
 * Called when done with a script line.
 */
    ಶೂನ್ಯ
script_ಸಾಲು_end(ಶೂನ್ಯ)
{
    scriptitem_T    *si;
    sn_prl_T	    *pp;

    ಯದಿ (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	ಪ್ರತ್ಯಾಗಮ;
    si = SCRIPT_ITEM(current_sctx.sc_sid);
    ಯದಿ (si->sn_prof_on && si->sn_prl_idx >= 0
				     && si->sn_prl_idx < si->sn_prl_ga.ga_len)
    {
	ಯದಿ (si->sn_prl_execed)
	{
	    pp = &PRL_ITEM(si, si->sn_prl_idx);
	    ++pp->snp_ಎಣಿಕೆ;
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
