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

#ಆದರೆ defined(FEAT_EVAL)
# ಆದರೆ defined(FEAT_PROFILE) || defined(FEAT_RELTIME)
/*
 * Store the current time in "tm".
 */
    ಶೂನ್ಯ
profile_ಆರಂಭ(proftime_T *tm)
{
#  ifdef MSWIN
    QueryPerformanceCounter(tm);
#  ಇಲ್ಲದಿದ್ದರೆ
    PROF_GET_ಸಮಯ(tm);
#  endif
}

/*
 * Compute the elapsed time from "tm" till now and store in "tm".
 */
    ಶೂನ್ಯ
profile_ಕೊನೆ(proftime_T *tm)
{
    proftime_T now;

#  ifdef MSWIN
    QueryPerformanceCounter(&now);
    tm->QuadPart = now.QuadPart - tm->QuadPart;
#  ಇಲ್ಲದಿದ್ದರೆ
    PROF_GET_ಸಮಯ(&now);
    tm->tv_fsec = now.tv_fsec - tm->tv_fsec;
    tm->tv_sec = now.tv_sec - tm->tv_sec;
    ಆದರೆ (tm->tv_fsec < 0)
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
#  ಇಲ್ಲದಿದ್ದರೆ
    tm->tv_fsec -= tm2->tv_fsec;
    tm->tv_sec -= tm2->tv_sec;
    ಆದರೆ (tm->tv_fsec < 0)
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
    ಅಕ್ಷರ *
profile_ಸಂದೇಶ(proftime_T *tm)
{
    ಸ್ಥಿರ ಅಕ್ಷರ buf[50];

#  ifdef MSWIN
    LARGE_INTEGER   fr;

    QueryPerformanceFrequency(&fr);
    sprintf(buf, "%10.6lf", (ದ್ವಿಗುಣ)tm->QuadPart / (ದ್ವಿಗುಣ)fr.QuadPart);
#  ಇಲ್ಲದಿದ್ದರೆ
    sprintf(buf, PROF_ಸಮಯ_FORMAT, (ದೀರ್ಘ)tm->tv_sec, (ದೀರ್ಘ)tm->tv_fsec);
#  endif
    ಹಿಂತಿರುಗಿ buf;
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
    ಹಿಂತಿರುಗಿ (float_T)tm->QuadPart / (float_T)fr.QuadPart;
#  ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ (float_T)tm->tv_sec + (float_T)tm->tv_fsec / (float_T)TV_FSEC_SEC;
#  endif
}

/*
 * Put the time "msec" past now in "tm".
 */
    ಶೂನ್ಯ
profile_setlimit(ದೀರ್ಘ msec, proftime_T *tm)
{
    ಆದರೆ (msec <= 0)   // no limit
	profile_zero(tm);
    ಇಲ್ಲದಿದ್ದರೆ
    {
#  ifdef MSWIN
	LARGE_INTEGER   fr;

	QueryPerformanceCounter(tm);
	QueryPerformanceFrequency(&fr);
	tm->QuadPart += (LONGLONG)((ದ್ವಿಗುಣ)msec / 1000.0 * (ದ್ವಿಗುಣ)fr.QuadPart);
#  ಇಲ್ಲದಿದ್ದರೆ
	varnumber_T	    fsec;	// this should be 64 bit if possible

	PROF_GET_ಸಮಯ(tm);
	fsec = (varnumber_T)tm->tv_fsec
		       + (varnumber_T)msec * (varnumber_T)(TV_FSEC_SEC / 1000);
	tm->tv_fsec = fsec % (ದೀರ್ಘ)TV_FSEC_SEC;
	tm->tv_sec += fsec / (ದೀರ್ಘ)TV_FSEC_SEC;
#  endif
    }
}

/*
 * Return TRUE if the current time is past "tm".
 */
    ಪೂರ್ಣಾಂಕ
profile_passed_ಎಲ್ಲೆ(proftime_T *tm)
{
    proftime_T	now;

#  ifdef MSWIN
    ಆದರೆ (tm->QuadPart == 0)  // timer was not set
	ಹಿಂತಿರುಗಿ FALSE;
    QueryPerformanceCounter(&now);
    ಹಿಂತಿರುಗಿ (now.QuadPart > tm->QuadPart);
#  ಇಲ್ಲದಿದ್ದರೆ
    ಆದರೆ (tm->tv_sec == 0)    // timer was not set
	ಹಿಂತಿರುಗಿ FALSE;
    PROF_GET_ಸಮಯ(&now);
    ಹಿಂತಿರುಗಿ (now.tv_sec > tm->tv_sec
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
#  ಇಲ್ಲದಿದ್ದರೆ
    tm->tv_fsec = 0;
    tm->tv_sec = 0;
#  endif
}

# endif  // FEAT_PROFILE || FEAT_RELTIME

# ಆದರೆ defined(FEAT_SYN_HL) && defined(FEAT_RELTIME) && defined(FEAT_PROFILE)
#  ಆದರೆ defined(HAVE_MATH_H)
#   include <math.h>
#  endif

/*
 * Divide the time "tm" by "count" and store in "tm2".
 */
    ಶೂನ್ಯ
profile_divide(proftime_T *tm, ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2, proftime_T *tm2)
{
    ಆದರೆ (ಎಣಿಕೆ_2 == 0)
	profile_zero(tm2);
    ಇಲ್ಲದಿದ್ದರೆ
    {
#  ifdef MSWIN
	tm2->QuadPart = tm->QuadPart / ಎಣಿಕೆ_2;
#  ಇಲ್ಲದಿದ್ದರೆ
	ದ್ವಿಗುಣ fsec = (tm->tv_sec * (float_T)TV_FSEC_SEC + tm->tv_fsec)
								       / ಎಣಿಕೆ_2;

	tm2->tv_sec = floor(fsec / (float_T)TV_FSEC_SEC);
	tm2->tv_fsec = vim_ಸುತ್ತು(fsec - (tm2->tv_sec * (float_T)TV_FSEC_SEC));
#  endif
    }
}
# endif

# ಆದರೆ defined(FEAT_PROFILE)
/*
 * Functions for profiling.
 */
ಸ್ಥಿರ proftime_T prof_wait_ಸಮಯ;

/*
 * Add the time "tm2" to "tm".
 */
    ಶೂನ್ಯ
profile_ಕೂಡಿಸು(proftime_T *tm, proftime_T *tm2)
{
#  ifdef MSWIN
    tm->QuadPart += tm2->QuadPart;
#  ಇಲ್ಲದಿದ್ದರೆ
    tm->tv_fsec += tm2->tv_fsec;
    tm->tv_sec += tm2->tv_sec;
    ಆದರೆ (tm->tv_fsec >= TV_FSEC_SEC)
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
profile_self(proftime_T *self, proftime_T *ಒಟ್ಟು, proftime_T *children)
{
    // Check that the result won't be negative.  Can happen with recursive
    // calls.
#  ifdef MSWIN
    ಆದರೆ (ಒಟ್ಟು->QuadPart <= children->QuadPart)
	ಹಿಂತಿರುಗಿ;
#  ಇಲ್ಲದಿದ್ದರೆ
    ಆದರೆ (ಒಟ್ಟು->tv_sec < children->tv_sec
	    || (ಒಟ್ಟು->tv_sec == children->tv_sec
		&& ಒಟ್ಟು->tv_fsec <= children->tv_fsec))
	ಹಿಂತಿರುಗಿ;
#  endif
    profile_ಕೂಡಿಸು(self, ಒಟ್ಟು);
    profile_sub(self, children);
}

/*
 * Get the current waittime.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
profile_get_wait(proftime_T *tm)
{
    *tm = prof_wait_ಸಮಯ;
}

/*
 * Subtract the passed waittime since "tm" from "tma".
 */
    ಶೂನ್ಯ
profile_sub_wait(proftime_T *tm, proftime_T *tma)
{
    proftime_T tm3 = prof_wait_ಸಮಯ;

    profile_sub(&tm3, tm);
    profile_sub(tma, &tm3);
}

/*
 * Return TRUE if "tm1" and "tm2" are equal.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
profile_equal(proftime_T *tm1, proftime_T *tm2)
{
#  ifdef MSWIN
    ಹಿಂತಿರುಗಿ (tm1->QuadPart == tm2->QuadPart);
#  ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ (tm1->tv_fsec == tm2->tv_fsec && tm1->tv_sec == tm2->tv_sec);
#  endif
}

/*
 * Return <0, 0 or >0 if "tm1" < "tm2", "tm1" == "tm2" or "tm1" > "tm2"
 */
    ಪೂರ್ಣಾಂಕ
profile_cmp(ಸ್ಥಿರಾಂಕ proftime_T *tm1, ಸ್ಥಿರಾಂಕ proftime_T *tm2)
{
#  ifdef MSWIN
    ಹಿಂತಿರುಗಿ tm2->QuadPart == tm1->QuadPart ? 0 :
	tm2->QuadPart > tm1->QuadPart ? 1 : -1;
#  ಇಲ್ಲದಿದ್ದರೆ
    ಆದರೆ (tm1->tv_sec == tm2->tv_sec)
	ಹಿಂತಿರುಗಿ tm2->tv_fsec == tm1->tv_fsec ? 0 :
	    tm2->tv_fsec > tm1->tv_fsec ? 1 : -1;
    ಹಿಂತಿರುಗಿ tm2->tv_sec > tm1->tv_sec ? 1 : -1;
#  endif
}

ಸ್ಥಿರ char_u	*profile_fname = ಖಾಲಿ;
ಸ್ಥಿರ proftime_T pause_ಸಮಯ;

/*
 * Reset all profiling information.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
profile_reset(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		id;
    ಪೂರ್ಣಾಂಕ		todo;
    hashtab_T	*functbl;
    hashitem_T	*hi;

    // Reset sourced files.
    ಗಾಗಿ (id = 1; id <= script_ವಸ್ತುಗಳು.ga_len; id++)
    {
	scriptitem_T *si = SCRIPT_ಅಂಶ(id);
	ಆದರೆ (si->sn_prof_on)
	{
	    si->sn_prof_on      = FALSE;
	    si->sn_pr_force     = FALSE;
	    profile_zero(&si->sn_pr_ಮಗು);
	    si->sn_pr_nest      = 0;
	    si->sn_pr_ಎಣಿಕೆ     = 0;
	    profile_zero(&si->sn_pr_ಒಟ್ಟು);
	    profile_zero(&si->sn_pr_self);
	    profile_zero(&si->sn_pr_ಆರಂಭ);
	    profile_zero(&si->sn_pr_children);
	    ga_clear(&si->sn_prl_ga);
	    profile_zero(&si->sn_prl_ಆರಂಭ);
	    profile_zero(&si->sn_prl_children);
	    profile_zero(&si->sn_prl_wait);
	    si->sn_prl_idx      = -1;
	    si->sn_prl_execed   = 0;
	}
    }

    // Reset functions.
    functbl = func_tbl_get();
    todo = (ಪೂರ್ಣಾಂಕ)functbl->ht_ಬಳಸಿದ;

    FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(functbl, hi, todo)
    {
	ufunc_T *fp;
	ಪೂರ್ಣಾಂಕ	i;

	ಆದರೆ (HASHITEM_ಖಾಲಿಗಳು(hi))
	    ಮುಂದುವರಿ;

	todo--;
	fp = HI2UF(hi);
	ಆದರೆ (fp->uf_prof_initialized)
	{
	    fp->uf_profiling    = 0;
	    fp->uf_prof_initialized = FALSE;
	    fp->uf_tm_ಎಣಿಕೆ     = 0;
	    profile_zero(&fp->uf_tm_ಒಟ್ಟು);
	    profile_zero(&fp->uf_tm_self);
	    profile_zero(&fp->uf_tm_children);

	    ಗಾಗಿ (i = 0; i < fp->uf_ಸಾಲುಗಳು.ga_len; i++)
	    {
		fp->uf_tml_ಎಣಿಕೆ[i] = 0;
		profile_zero(&fp->uf_tml_ಒಟ್ಟು[i]);
		profile_zero(&fp->uf_tml_self[i]);
	    }

	    profile_zero(&fp->uf_tml_ಆರಂಭ);
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
    ಪೂರ್ಣಾಂಕ		len;

    e = skiptowhite(eap->arg);
    len = (ಪೂರ್ಣಾಂಕ)(e - eap->arg);
    e = skipwhite(e);

    ಆದರೆ (len == 5 && STRNCMP(eap->arg, "start", 5) == 0 && *e != NUL)
    {
	VIM_CLEAR(profile_fname);
	profile_fname = expand_env_save_opt(e, TRUE, ಖಾಲಿ);
	do_profiling = PROF_YES;
	profile_zero(&prof_wait_ಸಮಯ);
	ಗಣ_vim_var_nr(VV_PROFILING, 1L);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (do_profiling == PROF_NONE)
	emsg(_(e_ಮೊದಲ_use_profile_ಆರಂಭ_fname));
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(eap->arg, "stop") == 0)
    {
	profile_dump();
	do_profiling = PROF_NONE;
	ಗಣ_vim_var_nr(VV_PROFILING, 0L);
	profile_reset();
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(eap->arg, "pause") == 0)
    {
	ಆದರೆ (do_profiling == PROF_YES)
	    profile_ಆರಂಭ(&pause_ಸಮಯ);
	do_profiling = PROF_PAUSED;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(eap->arg, "continue") == 0)
    {
	ಆದರೆ (do_profiling == PROF_PAUSED)
	{
	    profile_ಕೊನೆ(&pause_ಸಮಯ);
	    profile_ಕೂಡಿಸು(&prof_wait_ಸಮಯ, &pause_ಸಮಯ);
	}
	do_profiling = PROF_YES;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(eap->arg, "dump") == 0)
	profile_dump();
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// The rest is similar to ":breakadd".
	ex_breakadd(eap);
    }
}

// Command line expansion for :profile.
ಸ್ಥಿರ ಎಣಿಕೆ
{
    PEXP_SUBCMD,	// expand :profile sub-commands
    PEXP_FUNC		// expand :profile func {funcname}
} pexpand_what;

ಸ್ಥಿರ ಅಕ್ಷರ *pexpand_cmds[] = {
			"start",
			"stop",
			"pause",
			"continue",
			"func",
			"file",
			"dump",
			ಖಾಲಿ
};

/*
 * Function given to ExpandGeneric() to obtain the profile command
 * specific expansion.
 */
    char_u *
get_profile_name(expand_T *xp ಬಳಸದ, ಪೂರ್ಣಾಂಕ idx)
{
    ಆಯ್ಕೆ (pexpand_what)
    {
    ಸ್ಥಿತಿ PEXP_SUBCMD:
	ಹಿಂತಿರುಗಿ (char_u *)pexpand_cmds[idx];
    ಪೂರ್ವನಿಯೋಜಿತ:
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
}

/*
 * Handle command line completion for :profile command.
 */
    ಶೂನ್ಯ
ಗಣ_context_ಒಳ_profile_cmd(expand_T *xp, char_u *arg)
{
    char_u	*ಕೊನೆ_subcmd;

    // Default: expand subcommands.
    xp->xp_context = EXPAND_PROFILE;
    pexpand_what = PEXP_SUBCMD;
    xp->xp_ನಮೂನೆ = arg;

    ಕೊನೆ_subcmd = skiptowhite(arg);
    ಆದರೆ (*ಕೊನೆ_subcmd == NUL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ ((ಕೊನೆ_subcmd - arg == 5 && STRNCMP(arg, "start", 5) == 0)
	    || (ಕೊನೆ_subcmd - arg == 4 && STRNCMP(arg, "file", 4) == 0))
    {
	xp->xp_context = EXPAND_FILES;
	xp->xp_ನಮೂನೆ = skipwhite(ಕೊನೆ_subcmd);
	ಹಿಂತಿರುಗಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಕೊನೆ_subcmd - arg == 4 && STRNCMP(arg, "func", 4) == 0)
    {
	xp->xp_context = EXPAND_USER_FUNC;
	xp->xp_ನಮೂನೆ = skipwhite(ಕೊನೆ_subcmd);
	ಹಿಂತಿರುಗಿ;
    }

    xp->xp_context = EXPAND_NOTHING;
}

ಸ್ಥಿರ proftime_T inchar_ಸಮಯ;

/*
 * Called when starting to wait for the user to type a character.
 */
    ಶೂನ್ಯ
prof_inchar_enter(ಶೂನ್ಯ)
{
    profile_ಆರಂಭ(&inchar_ಸಮಯ);
}

/*
 * Called when finished waiting for the user to type a character.
 */
    ಶೂನ್ಯ
prof_inchar_exit(ಶೂನ್ಯ)
{
    profile_ಕೊನೆ(&inchar_ಸಮಯ);
    profile_ಕೂಡಿಸು(&prof_wait_ಸಮಯ, &inchar_ಸಮಯ);
}


/*
 * Return TRUE when a function defined in the current script should be
 * profiled.
 */
    ಪೂರ್ಣಾಂಕ
prof_def_func(ಶೂನ್ಯ)
{
    ಆದರೆ (current_sctx.sc_sid > 0)
	ಹಿಂತಿರುಗಿ SCRIPT_ಅಂಶ(current_sctx.sc_sid)->sn_pr_force;
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Print the count and times for one function or function line.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
prof_func_ಸಾಲು(
    FILE	*fd,
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2,
    proftime_T	*ಒಟ್ಟು,
    proftime_T	*self,
    ಪೂರ್ಣಾಂಕ		prefer_self)	// when equal print only self time
{
    ಆದರೆ (ಎಣಿಕೆ_2 > 0)
    {
	fprintf(fd, "%5d ", ಎಣಿಕೆ_2);
	ಆದರೆ (prefer_self && profile_equal(ಒಟ್ಟು, self))
	    fprintf(fd, PROF_ಸಮಯ_BLANK);
	ಇಲ್ಲದಿದ್ದರೆ
	    fprintf(fd, "%s ", profile_ಸಂದೇಶ(ಒಟ್ಟು));
	ಆದರೆ (!prefer_self && profile_equal(ಒಟ್ಟು, self))
	    fprintf(fd, PROF_ಸಮಯ_BLANK);
	ಇಲ್ಲದಿದ್ದರೆ
	    fprintf(fd, "%s ", profile_ಸಂದೇಶ(self));
    }
    ಇಲ್ಲದಿದ್ದರೆ
	fprintf(fd, "      %s%s", PROF_ಸಮಯ_BLANK, PROF_ಸಮಯ_BLANK);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
prof_ವಿಂಗಡಿಸು_list(
    FILE	*fd,
    ufunc_T	**sorttab,
    ಪೂರ್ಣಾಂಕ		st_len,
    ಅಕ್ಷರ	*title,
    ಪೂರ್ಣಾಂಕ		prefer_self)	// when equal print only self time
{
    ಪೂರ್ಣಾಂಕ		i;
    ufunc_T	*fp;

    fprintf(fd, "FUNCTIONS SORTED ON %s TIME\n", title);
    fprintf(fd, "%s  function\n", PROF_TOTALS_ಶೀರ್ಷಿಕೆ);
    ಗಾಗಿ (i = 0; i < 20 && i < st_len; ++i)
    {
	fp = sorttab[i];
	prof_func_ಸಾಲು(fd, fp->uf_tm_ಎಣಿಕೆ, &fp->uf_tm_ಒಟ್ಟು, &fp->uf_tm_self,
								 prefer_self);
	ಆದರೆ (fp->uf_name[0] == K_SPECIAL)
	    fprintf(fd, " <SNR>%s()\n", fp->uf_name + 3);
	ಇಲ್ಲದಿದ್ದರೆ
	    fprintf(fd, " %s()\n", fp->uf_name);
    }
    fprintf(fd, "\n");
}

/*
 * Compare function for total time sorting.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
prof_ಒಟ್ಟು_cmp(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *s1, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *s2)
{
    ufunc_T	*p1, *p2;

    p1 = *(ufunc_T **)s1;
    p2 = *(ufunc_T **)s2;
    ಹಿಂತಿರುಗಿ profile_cmp(&p1->uf_tm_ಒಟ್ಟು, &p2->uf_tm_ಒಟ್ಟು);
}

/*
 * Compare function for self time sorting.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
prof_self_cmp(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *s1, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *s2)
{
    ufunc_T	*p1, *p2;

    p1 = *(ufunc_T **)s1;
    p2 = *(ufunc_T **)s2;
    ಹಿಂತಿರುಗಿ profile_cmp(&p1->uf_tm_self, &p2->uf_tm_self);
}

/*
 * Start profiling function "fp".
 */
    ಶೂನ್ಯ
func_do_profile(ufunc_T *fp)
{
    ಪೂರ್ಣಾಂಕ		len = fp->uf_ಸಾಲುಗಳು.ga_len;

    ಆದರೆ (!fp->uf_prof_initialized)
    {
	ಆದರೆ (len == 0)
	    len = 1;  // avoid getting error for allocating zero bytes
	fp->uf_tm_ಎಣಿಕೆ = 0;
	profile_zero(&fp->uf_tm_self);
	profile_zero(&fp->uf_tm_ಒಟ್ಟು);
	ಆದರೆ (fp->uf_tml_ಎಣಿಕೆ == ಖಾಲಿ)
	    fp->uf_tml_ಎಣಿಕೆ = ALLOC_CLEAR_MULT(ಪೂರ್ಣಾಂಕ, len);
	ಆದರೆ (fp->uf_tml_ಒಟ್ಟು == ಖಾಲಿ)
	    fp->uf_tml_ಒಟ್ಟು = ALLOC_CLEAR_MULT(proftime_T, len);
	ಆದರೆ (fp->uf_tml_self == ಖಾಲಿ)
	    fp->uf_tml_self = ALLOC_CLEAR_MULT(proftime_T, len);
	fp->uf_tml_idx = -1;
	ಆದರೆ (fp->uf_tml_ಎಣಿಕೆ == ಖಾಲಿ || fp->uf_tml_ಒಟ್ಟು == ಖಾಲಿ
						    || fp->uf_tml_self == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;	    // out of memory
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

    ಆದರೆ (SCRIPT_ID_VALID(current_sctx.sc_sid))
    {
	si = SCRIPT_ಅಂಶ(current_sctx.sc_sid);
	ಆದರೆ (si->sn_prof_on && si->sn_pr_nest++ == 0)
	    profile_ಆರಂಭ(&si->sn_pr_ಮಗು);
    }
    profile_get_wait(tm);
}

/*
 * When calling a function: may initialize for profiling.
 */
    ಶೂನ್ಯ
profile_may_ಆರಂಭ_func(profinfo_T *info, ufunc_T *fp, ufunc_T *caller)
{
    ಆದರೆ (!fp->uf_profiling && has_profiling(FALSE, fp->uf_name, ಖಾಲಿ,
								&fp->uf_hash))
    {
	info->pi_started_profiling = TRUE;
	func_do_profile(fp);
    }
    ಆದರೆ (fp->uf_profiling || (caller != ಖಾಲಿ && caller->uf_profiling))
    {
	++fp->uf_tm_ಎಣಿಕೆ;
	profile_ಆರಂಭ(&info->pi_call_ಆರಂಭ);
	profile_zero(&fp->uf_tm_children);
    }
    script_prof_save(&info->pi_wait_ಆರಂಭ);
}

/*
 * After calling a function: may handle profiling.  profile_may_start_func()
 * must have been called previously.
 */
    ಶೂನ್ಯ
profile_may_ಕೊನೆ_func(profinfo_T *info, ufunc_T *fp, ufunc_T *caller)
{
    profile_ಕೊನೆ(&info->pi_call_ಆರಂಭ);
    profile_sub_wait(&info->pi_wait_ಆರಂಭ, &info->pi_call_ಆರಂಭ);
    profile_ಕೂಡಿಸು(&fp->uf_tm_ಒಟ್ಟು, &info->pi_call_ಆರಂಭ);
    profile_self(&fp->uf_tm_self, &info->pi_call_ಆರಂಭ, &fp->uf_tm_children);
    ಆದರೆ (caller != ಖಾಲಿ && caller->uf_profiling)
    {
	profile_ಕೂಡಿಸು(&caller->uf_tm_children, &info->pi_call_ಆರಂಭ);
	profile_ಕೂಡಿಸು(&caller->uf_tml_children, &info->pi_call_ಆರಂಭ);
    }
    ಆದರೆ (info->pi_started_profiling)
	// make a ":profdel func" stop profiling the function
	fp->uf_profiling = FALSE;
}

/*
 * Prepare profiling for entering a child or something else that is not
 * counted for the script/function itself.
 * Should always be called in pair with prof_child_exit().
 */
    ಶೂನ್ಯ
prof_ಮಗು_enter(
    proftime_T *tm)	// place to store waittime
{
    funccall_T *fc = get_current_funccal();

    ಆದರೆ (fc != ಖಾಲಿ && fc->fc_func->uf_profiling)
	profile_ಆರಂಭ(&fc->fc_prof_ಮಗು);
    script_prof_save(tm);
}

/*
 * Take care of time spent in a child.
 * Should always be called after prof_child_enter().
 */
    ಶೂನ್ಯ
prof_ಮಗು_exit(
    proftime_T *tm)	// where waittime was stored
{
    funccall_T *fc = get_current_funccal();

    ಆದರೆ (fc != ಖಾಲಿ && fc->fc_func->uf_profiling)
    {
	profile_ಕೊನೆ(&fc->fc_prof_ಮಗು);
	profile_sub_wait(tm, &fc->fc_prof_ಮಗು); // don't count waiting time
	profile_ಕೂಡಿಸು(&fc->fc_func->uf_tm_children, &fc->fc_prof_ಮಗು);
	profile_ಕೂಡಿಸು(&fc->fc_func->uf_tml_children, &fc->fc_prof_ಮಗು);
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
func_ಸಾಲು_ಆರಂಭ(ಶೂನ್ಯ *cookie, ದೀರ್ಘ lnum)
{
    funccall_T	*fcp = (funccall_T *)cookie;
    ufunc_T	*fp = fcp->fc_func;

    ಆದರೆ (fp->uf_profiling && lnum >= 1 && lnum <= fp->uf_ಸಾಲುಗಳು.ga_len)
    {
	fp->uf_tml_idx = lnum - 1;
	// Skip continuation lines.
	ಆಗ (fp->uf_tml_idx > 0 && FUNCLINE(fp, fp->uf_tml_idx) == ಖಾಲಿ)
	    --fp->uf_tml_idx;
	fp->uf_tml_execed = FALSE;
	profile_ಆರಂಭ(&fp->uf_tml_ಆರಂಭ);
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

    ಆದರೆ (fp->uf_profiling && fp->uf_tml_idx >= 0)
	fp->uf_tml_execed = TRUE;
}

/*
 * Called when done with a function line.
 */
    ಶೂನ್ಯ
func_ಸಾಲು_ಕೊನೆ(ಶೂನ್ಯ *cookie)
{
    funccall_T	*fcp = (funccall_T *)cookie;
    ufunc_T	*fp = fcp->fc_func;

    ಆದರೆ (fp->uf_profiling && fp->uf_tml_idx >= 0)
    {
	ಆದರೆ (fp->uf_tml_execed)
	{
	    ++fp->uf_tml_ಎಣಿಕೆ[fp->uf_tml_idx];
	    profile_ಕೊನೆ(&fp->uf_tml_ಆರಂಭ);
	    profile_sub_wait(&fp->uf_tml_wait, &fp->uf_tml_ಆರಂಭ);
	    profile_ಕೂಡಿಸು(&fp->uf_tml_ಒಟ್ಟು[fp->uf_tml_idx], &fp->uf_tml_ಆರಂಭ);
	    profile_self(&fp->uf_tml_self[fp->uf_tml_idx], &fp->uf_tml_ಆರಂಭ,
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
    ಪೂರ್ಣಾಂಕ		todo;
    ufunc_T	*fp;
    ಪೂರ್ಣಾಂಕ		i;
    ufunc_T	**sorttab;
    ಪೂರ್ಣಾಂಕ		st_len = 0;
    char_u	*p;

    functbl = func_tbl_get();
    todo = (ಪೂರ್ಣಾಂಕ)functbl->ht_ಬಳಸಿದ;
    ಆದರೆ (todo == 0)
	ಹಿಂತಿರುಗಿ;     // nothing to dump

    sorttab = ALLOC_MULT(ufunc_T *, todo);

    FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(functbl, hi, todo)
    {
	ಆದರೆ (!HASHITEM_ಖಾಲಿಗಳು(hi))
	{
	    --todo;
	    fp = HI2UF(hi);
	    ಆದರೆ (fp->uf_prof_initialized)
	    {
		ಆದರೆ (sorttab != ಖಾಲಿ)
		    sorttab[st_len++] = fp;

		ಆದರೆ (fp->uf_name[0] == K_SPECIAL)
		    fprintf(fd, "FUNCTION  <SNR>%s()\n", fp->uf_name + 3);
		ಇಲ್ಲದಿದ್ದರೆ
		    fprintf(fd, "FUNCTION  %s()\n", fp->uf_name);
		ಆದರೆ (fp->uf_script_ctx.sc_sid > 0)
		{
		    p = home_replace_save(ಖಾಲಿ,
				     get_scriptname(fp->uf_script_ctx.sc_sid));
		    ಆದರೆ (p != ಖಾಲಿ)
		    {
			fprintf(fd, "    Defined: %s:%ld\n",
					   p, (ದೀರ್ಘ)fp->uf_script_ctx.sc_lnum);
			vim_free(p);
		    }
		}
		ಆದರೆ (fp->uf_tm_ಎಣಿಕೆ == 1)
		    fprintf(fd, "Called 1 time\n");
		ಇಲ್ಲದಿದ್ದರೆ
		    fprintf(fd, "Called %d times\n", fp->uf_tm_ಎಣಿಕೆ);
		fprintf(fd, "Total time: %s\n", profile_ಸಂದೇಶ(&fp->uf_tm_ಒಟ್ಟು));
		fprintf(fd, " Self time: %s\n", profile_ಸಂದೇಶ(&fp->uf_tm_self));
		fprintf(fd, "\n");
		fprintf(fd, "%s\n", PROF_TOTALS_ಶೀರ್ಷಿಕೆ);

		ಗಾಗಿ (i = 0; i < fp->uf_ಸಾಲುಗಳು.ga_len; ++i)
		{
		    ಆದರೆ (FUNCLINE(fp, i) == ಖಾಲಿ)
			ಮುಂದುವರಿ;
		    prof_func_ಸಾಲು(fd, fp->uf_tml_ಎಣಿಕೆ[i],
			     &fp->uf_tml_ಒಟ್ಟು[i], &fp->uf_tml_self[i], TRUE);
		    fprintf(fd, "%s\n", FUNCLINE(fp, i));
		}
		fprintf(fd, "\n");
	    }
	}
    }

    ಆದರೆ (sorttab != ಖಾಲಿ && st_len > 0)
    {
	qsort((ಶೂನ್ಯ *)sorttab, (size_t)st_len, ಗಾತ್ರ(ufunc_T *),
							      prof_ಒಟ್ಟು_cmp);
	prof_ವಿಂಗಡಿಸು_list(fd, sorttab, st_len, "TOTAL", FALSE);
	qsort((ಶೂನ್ಯ *)sorttab, (size_t)st_len, ಗಾತ್ರ(ufunc_T *),
							      prof_self_cmp);
	prof_ವಿಂಗಡಿಸು_list(fd, sorttab, st_len, "SELF", TRUE);
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
    profile_zero(&si->sn_pr_ಒಟ್ಟು);
    profile_zero(&si->sn_pr_self);

    ga_init2(&si->sn_prl_ga, ಗಾತ್ರ(sn_prl_T), 100);
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

    ಆದರೆ (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	ಹಿಂತಿರುಗಿ;

    si = SCRIPT_ಅಂಶ(current_sctx.sc_sid);
    ಆದರೆ (si->sn_prof_on && --si->sn_pr_nest == 0)
    {
	profile_ಕೊನೆ(&si->sn_pr_ಮಗು);
	profile_sub_wait(tm, &si->sn_pr_ಮಗು); // don't count wait time
	profile_ಕೂಡಿಸು(&si->sn_pr_children, &si->sn_pr_ಮಗು);
	profile_ಕೂಡಿಸು(&si->sn_prl_children, &si->sn_pr_ಮಗು);
    }
}

/*
 * Dump the profiling results for all scripts in file "fd".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
script_dump_profile(FILE *fd)
{
    ಪೂರ್ಣಾಂಕ		    id;
    scriptitem_T    *si;
    ಪೂರ್ಣಾಂಕ		    i;
    FILE	    *sfd;
    sn_prl_T	    *pp;

    ಗಾಗಿ (id = 1; id <= script_ವಸ್ತುಗಳು.ga_len; ++id)
    {
	si = SCRIPT_ಅಂಶ(id);
	ಆದರೆ (si->sn_prof_on)
	{
	    fprintf(fd, "SCRIPT  %s\n", si->sn_name);
	    ಆದರೆ (si->sn_pr_ಎಣಿಕೆ == 1)
		fprintf(fd, "Sourced 1 time\n");
	    ಇಲ್ಲದಿದ್ದರೆ
		fprintf(fd, "Sourced %d times\n", si->sn_pr_ಎಣಿಕೆ);
	    fprintf(fd, "Total time: %s\n", profile_ಸಂದೇಶ(&si->sn_pr_ಒಟ್ಟು));
	    fprintf(fd, " Self time: %s\n", profile_ಸಂದೇಶ(&si->sn_pr_self));
	    fprintf(fd, "\n");
	    fprintf(fd, "%s\n", PROF_TOTALS_ಶೀರ್ಷಿಕೆ);

	    sfd = mch_fopen((ಅಕ್ಷರ *)si->sn_name, "r");
	    ಆದರೆ (sfd == ಖಾಲಿ)
		fprintf(fd, "Cannot open file!\n");
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// Keep going till the end of file, so that trailing
		// continuation lines are listed.
		ಗಾಗಿ (i = 0; ; ++i)
		{
		    ಆದರೆ (vim_fgets(IObuff, IOSIZE, sfd))
			ನಿಲ್ಲಿಸು;
		    // When a line has been truncated, append NL, taking care
		    // of multi-byte characters .
		    ಆದರೆ (IObuff[IOSIZE - 2] != NUL && IObuff[IOSIZE - 2] != NL)
		    {
			ಪೂರ್ಣಾಂಕ n = IOSIZE - 2;

			ಆದರೆ (enc_utf8)
			{
			    // Move to the first byte of this char.
			    // utf_head_off() doesn't work, because it checks
			    // for a truncated character.
			    ಆಗ (n > 0 && (IObuff[n] & 0xc0) == 0x80)
				--n;
			}
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (has_mbyte)
			    n -= mb_ತಲೆ_off(IObuff, IObuff + n);
			IObuff[n] = NL;
			IObuff[n + 1] = NUL;
		    }
		    ಆದರೆ (i < si->sn_prl_ga.ga_len
				     && (pp = &PRL_ಅಂಶ(si, i))->snp_ಎಣಿಕೆ > 0)
		    {
			fprintf(fd, "%5d ", pp->snp_ಎಣಿಕೆ);
			ಆದರೆ (profile_equal(&pp->sn_prl_ಒಟ್ಟು, &pp->sn_prl_self))
			    fprintf(fd, "           ");
			ಇಲ್ಲದಿದ್ದರೆ
			    fprintf(fd, "%s ", profile_ಸಂದೇಶ(&pp->sn_prl_ಒಟ್ಟು));
			fprintf(fd, "%s ", profile_ಸಂದೇಶ(&pp->sn_prl_self));
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
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

    ಆದರೆ (profile_fname == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    fd = mch_fopen((ಅಕ್ಷರ *)profile_fname, "w");
    ಆದರೆ (fd == ಖಾಲಿ)
	semsg(_(e_cant_open_file_str), profile_fname);
    ಇಲ್ಲದಿದ್ದರೆ
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
script_ಸಾಲು_ಆರಂಭ(ಶೂನ್ಯ)
{
    scriptitem_T    *si;
    sn_prl_T	    *pp;

    ಆದರೆ (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	ಹಿಂತಿರುಗಿ;
    si = SCRIPT_ಅಂಶ(current_sctx.sc_sid);
    ಆದರೆ (si->sn_prof_on && SOURCING_LNUM >= 1)
    {
	// Grow the array before starting the timer, so that the time spent
	// here isn't counted.
	(ಶೂನ್ಯ)ga_grow(&si->sn_prl_ga,
				  (ಪೂರ್ಣಾಂಕ)(SOURCING_LNUM - si->sn_prl_ga.ga_len));
	si->sn_prl_idx = SOURCING_LNUM - 1;
	ಆಗ (si->sn_prl_ga.ga_len <= si->sn_prl_idx
		&& si->sn_prl_ga.ga_len < si->sn_prl_ga.ga_maxlen)
	{
	    // Zero counters for a line that was not used before.
	    pp = &PRL_ಅಂಶ(si, si->sn_prl_ga.ga_len);
	    pp->snp_ಎಣಿಕೆ = 0;
	    profile_zero(&pp->sn_prl_ಒಟ್ಟು);
	    profile_zero(&pp->sn_prl_self);
	    ++si->sn_prl_ga.ga_len;
	}
	si->sn_prl_execed = FALSE;
	profile_ಆರಂಭ(&si->sn_prl_ಆರಂಭ);
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

    ಆದರೆ (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	ಹಿಂತಿರುಗಿ;
    si = SCRIPT_ಅಂಶ(current_sctx.sc_sid);
    ಆದರೆ (si->sn_prof_on && si->sn_prl_idx >= 0)
	si->sn_prl_execed = TRUE;
}

/*
 * Called when done with a script line.
 */
    ಶೂನ್ಯ
script_ಸಾಲು_ಕೊನೆ(ಶೂನ್ಯ)
{
    scriptitem_T    *si;
    sn_prl_T	    *pp;

    ಆದರೆ (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	ಹಿಂತಿರುಗಿ;
    si = SCRIPT_ಅಂಶ(current_sctx.sc_sid);
    ಆದರೆ (si->sn_prof_on && si->sn_prl_idx >= 0
				     && si->sn_prl_idx < si->sn_prl_ga.ga_len)
    {
	ಆದರೆ (si->sn_prl_execed)
	{
	    pp = &PRL_ಅಂಶ(si, si->sn_prl_idx);
	    ++pp->snp_ಎಣಿಕೆ;
	    profile_ಕೊನೆ(&si->sn_prl_ಆರಂಭ);
	    profile_sub_wait(&si->sn_prl_wait, &si->sn_prl_ಆರಂಭ);
	    profile_ಕೂಡಿಸು(&pp->sn_prl_ಒಟ್ಟು, &si->sn_prl_ಆರಂಭ);
	    profile_self(&pp->sn_prl_self, &si->sn_prl_ಆರಂಭ,
							&si->sn_prl_children);
	}
	si->sn_prl_idx = -1;
    }
}
# endif // FEAT_PROFILE

#ಆದರೆ_ಅಂತ್ಯ
