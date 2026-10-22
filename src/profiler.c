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

#ଅନ୍ତର୍ଭାଵ "vim.h"

#ଯଦି defined(FEAT_EVAL)
# ଯଦି defined(FEAT_PROFILE) || defined(FEAT_RELTIME)
/*
 * Store the current time in "tm".
 */
    ଶୂନ୍ୟ
profile_start(proftime_T *tm)
{
#  ifdef MSWIN
    QueryPerformanceCounter(tm);
#  ଅନ୍ୟଥା
    PROF_GET_TIME(tm);
#  endif
}

/*
 * Compute the elapsed time from "tm" till now and store in "tm".
 */
    ଶୂନ୍ୟ
profile_end(proftime_T *tm)
{
    proftime_T now;

#  ifdef MSWIN
    QueryPerformanceCounter(&now);
    tm->QuadPart = now.QuadPart - tm->QuadPart;
#  ଅନ୍ୟଥା
    PROF_GET_TIME(&now);
    tm->tv_fsec = now.tv_fsec - tm->tv_fsec;
    tm->tv_sec = now.tv_sec - tm->tv_sec;
    ଯଦି (tm->tv_fsec < 0)
    {
	tm->tv_fsec += TV_FSEC_SEC;
	--tm->tv_sec;
    }
#  endif
}

/*
 * Subtract the time "tm2" from "tm".
 */
    ଶୂନ୍ୟ
profile_sub(proftime_T *tm, proftime_T *tm2)
{
#  ifdef MSWIN
    tm->QuadPart -= tm2->QuadPart;
#  ଅନ୍ୟଥା
    tm->tv_fsec -= tm2->tv_fsec;
    tm->tv_sec -= tm2->tv_sec;
    ଯଦି (tm->tv_fsec < 0)
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
    ଅକ୍ଷର *
profile_ସନ୍ଦେଶ(proftime_T *tm)
{
    ସ୍ଥିର ଅକ୍ଷର buf[50];

#  ifdef MSWIN
    LARGE_INTEGER   fr;

    QueryPerformanceFrequency(&fr);
    sprintf(buf, "%10.6lf", (ଦ୍ୱିଗୁଣ)tm->QuadPart / (ଦ୍ୱିଗୁଣ)fr.QuadPart);
#  ଅନ୍ୟଥା
    sprintf(buf, PROF_TIME_FORMAT, (long)tm->tv_sec, (long)tm->tv_fsec);
#  endif
    ଫେରନ୍ତୁ buf;
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
    ଫେରନ୍ତୁ (float_T)tm->QuadPart / (float_T)fr.QuadPart;
#  ଅନ୍ୟଥା
    ଫେରନ୍ତୁ (float_T)tm->tv_sec + (float_T)tm->tv_fsec / (float_T)TV_FSEC_SEC;
#  endif
}

/*
 * Put the time "msec" past now in "tm".
 */
    ଶୂନ୍ୟ
profile_setlimit(long msec, proftime_T *tm)
{
    ଯଦି (msec <= 0)   // no limit
	profile_zero(tm);
    ଅନ୍ୟଥା
    {
#  ifdef MSWIN
	LARGE_INTEGER   fr;

	QueryPerformanceCounter(tm);
	QueryPerformanceFrequency(&fr);
	tm->QuadPart += (LONGLONG)((ଦ୍ୱିଗୁଣ)msec / 1000.0 * (ଦ୍ୱିଗୁଣ)fr.QuadPart);
#  ଅନ୍ୟଥା
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
profile_passed_limit(proftime_T *tm)
{
    proftime_T	now;

#  ifdef MSWIN
    ଯଦି (tm->QuadPart == 0)  // timer was not set
	ଫେରନ୍ତୁ FALSE;
    QueryPerformanceCounter(&now);
    ଫେରନ୍ତୁ (now.QuadPart > tm->QuadPart);
#  ଅନ୍ୟଥା
    ଯଦି (tm->tv_sec == 0)    // timer was not set
	ଫେରନ୍ତୁ FALSE;
    PROF_GET_TIME(&now);
    ଫେରନ୍ତୁ (now.tv_sec > tm->tv_sec
	    || (now.tv_sec == tm->tv_sec && now.tv_fsec > tm->tv_fsec));
#  endif
}

/*
 * Set the time in "tm" to zero.
 */
    ଶୂନ୍ୟ
profile_zero(proftime_T *tm)
{
#  ifdef MSWIN
    tm->QuadPart = 0;
#  ଅନ୍ୟଥା
    tm->tv_fsec = 0;
    tm->tv_sec = 0;
#  endif
}

# endif  // FEAT_PROFILE || FEAT_RELTIME

# ଯଦି defined(FEAT_SYN_HL) && defined(FEAT_RELTIME) && defined(FEAT_PROFILE)
#  ଯଦି defined(HAVE_MATH_H)
#   include <math.h>
#  endif

/*
 * Divide the time "tm" by "count" and store in "tm2".
 */
    ଶୂନ୍ୟ
profile_divide(proftime_T *tm, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2, proftime_T *tm2)
{
    ଯଦି (ଗଣନା_2 == 0)
	profile_zero(tm2);
    ଅନ୍ୟଥା
    {
#  ifdef MSWIN
	tm2->QuadPart = tm->QuadPart / ଗଣନା_2;
#  ଅନ୍ୟଥା
	ଦ୍ୱିଗୁଣ fsec = (tm->tv_sec * (float_T)TV_FSEC_SEC + tm->tv_fsec)
								       / ଗଣନା_2;

	tm2->tv_sec = floor(fsec / (float_T)TV_FSEC_SEC);
	tm2->tv_fsec = vim_round(fsec - (tm2->tv_sec * (float_T)TV_FSEC_SEC));
#  endif
    }
}
# endif

# ଯଦି defined(FEAT_PROFILE)
/*
 * Functions for profiling.
 */
ସ୍ଥିର proftime_T prof_wait_time;

/*
 * Add the time "tm2" to "tm".
 */
    ଶୂନ୍ୟ
profile_add(proftime_T *tm, proftime_T *tm2)
{
#  ifdef MSWIN
    tm->QuadPart += tm2->QuadPart;
#  ଅନ୍ୟଥା
    tm->tv_fsec += tm2->tv_fsec;
    tm->tv_sec += tm2->tv_sec;
    ଯଦି (tm->tv_fsec >= TV_FSEC_SEC)
    {
	tm->tv_fsec -= TV_FSEC_SEC;
	++tm->tv_sec;
    }
#  endif
}

/*
 * Add the "self" time from the total time and the children's time.
 */
    ଶୂନ୍ୟ
profile_self(proftime_T *self, proftime_T *total, proftime_T *children)
{
    // Check that the result won't be negative.  Can happen with recursive
    // calls.
#  ifdef MSWIN
    ଯଦି (total->QuadPart <= children->QuadPart)
	ଫେରନ୍ତୁ;
#  ଅନ୍ୟଥା
    ଯଦି (total->tv_sec < children->tv_sec
	    || (total->tv_sec == children->tv_sec
		&& total->tv_fsec <= children->tv_fsec))
	ଫେରନ୍ତୁ;
#  endif
    profile_add(self, total);
    profile_sub(self, children);
}

/*
 * Get the current waittime.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
profile_get_wait(proftime_T *tm)
{
    *tm = prof_wait_time;
}

/*
 * Subtract the passed waittime since "tm" from "tma".
 */
    ଶୂନ୍ୟ
profile_sub_wait(proftime_T *tm, proftime_T *tma)
{
    proftime_T tm3 = prof_wait_time;

    profile_sub(&tm3, tm);
    profile_sub(tma, &tm3);
}

/*
 * Return TRUE if "tm1" and "tm2" are equal.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
profile_equal(proftime_T *tm1, proftime_T *tm2)
{
#  ifdef MSWIN
    ଫେରନ୍ତୁ (tm1->QuadPart == tm2->QuadPart);
#  ଅନ୍ୟଥା
    ଫେରନ୍ତୁ (tm1->tv_fsec == tm2->tv_fsec && tm1->tv_sec == tm2->tv_sec);
#  endif
}

/*
 * Return <0, 0 or >0 if "tm1" < "tm2", "tm1" == "tm2" or "tm1" > "tm2"
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
profile_cmp(ଅଚଳ proftime_T *tm1, ଅଚଳ proftime_T *tm2)
{
#  ifdef MSWIN
    ଫେରନ୍ତୁ tm2->QuadPart == tm1->QuadPart ? 0 :
	tm2->QuadPart > tm1->QuadPart ? 1 : -1;
#  ଅନ୍ୟଥା
    ଯଦି (tm1->tv_sec == tm2->tv_sec)
	ଫେରନ୍ତୁ tm2->tv_fsec == tm1->tv_fsec ? 0 :
	    tm2->tv_fsec > tm1->tv_fsec ? 1 : -1;
    ଫେରନ୍ତୁ tm2->tv_sec > tm1->tv_sec ? 1 : -1;
#  endif
}

ସ୍ଥିର char_u	*profile_fname = ଖାଲି;
ସ୍ଥିର proftime_T pause_time;

/*
 * Reset all profiling information.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
profile_reset(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		id;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		todo;
    hashtab_T	*functbl;
    hashitem_T	*hi;

    // Reset sourced files.
    ପାଇଁ (id = 1; id <= script_ବସ୍ତୁଗୁଡ଼ିକ.ga_len; id++)
    {
	scriptitem_T *si = SCRIPT_ITEM(id);
	ଯଦି (si->sn_prof_on)
	{
	    si->sn_prof_on      = FALSE;
	    si->sn_pr_force     = FALSE;
	    profile_zero(&si->sn_pr_child);
	    si->sn_pr_nest      = 0;
	    si->sn_pr_ଗଣନା     = 0;
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
    todo = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)functbl->ht_used;

    FOR_ALL_HASHTAB_ବସ୍ତୁଗୁଡ଼ିକ(functbl, hi, todo)
    {
	ufunc_T *fp;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	i;

	ଯଦି (HASHITEM_EMPTY(hi))
	    ଜାରି;

	todo--;
	fp = HI2UF(hi);
	ଯଦି (fp->uf_prof_initialized)
	{
	    fp->uf_profiling    = 0;
	    fp->uf_prof_initialized = FALSE;
	    fp->uf_tm_ଗଣନା     = 0;
	    profile_zero(&fp->uf_tm_total);
	    profile_zero(&fp->uf_tm_self);
	    profile_zero(&fp->uf_tm_children);

	    ପାଇଁ (i = 0; i < fp->uf_ଧାଡ଼ିଗୁଡ଼ିକ.ga_len; i++)
	    {
		fp->uf_tml_ଗଣନା[i] = 0;
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
    ଶୂନ୍ୟ
ex_profile(exarg_T *eap)
{
    char_u	*e;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;

    e = skiptowhite(eap->arg);
    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(e - eap->arg);
    e = skipwhite(e);

    ଯଦି (len == 5 && STRNCMP(eap->arg, "start", 5) == 0 && *e != NUL)
    {
	VIM_CLEAR(profile_fname);
	profile_fname = expand_env_save_opt(e, TRUE, ଖାଲି);
	do_profiling = PROF_YES;
	profile_zero(&prof_wait_time);
	set_vim_var_nr(VV_PROFILING, 1L);
    }
    ଅନ୍ୟଥା ଯଦି (do_profiling == PROF_NONE)
	emsg(_(e_first_use_profile_start_fname));
    ଅନ୍ୟଥା ଯଦି (STRCMP(eap->arg, "stop") == 0)
    {
	profile_dump();
	do_profiling = PROF_NONE;
	set_vim_var_nr(VV_PROFILING, 0L);
	profile_reset();
    }
    ଅନ୍ୟଥା ଯଦି (STRCMP(eap->arg, "pause") == 0)
    {
	ଯଦି (do_profiling == PROF_YES)
	    profile_start(&pause_time);
	do_profiling = PROF_PAUSED;
    }
    ଅନ୍ୟଥା ଯଦି (STRCMP(eap->arg, "continue") == 0)
    {
	ଯଦି (do_profiling == PROF_PAUSED)
	{
	    profile_end(&pause_time);
	    profile_add(&prof_wait_time, &pause_time);
	}
	do_profiling = PROF_YES;
    }
    ଅନ୍ୟଥା ଯଦି (STRCMP(eap->arg, "dump") == 0)
	profile_dump();
    ଅନ୍ୟଥା
    {
	// The rest is similar to ":breakadd".
	ex_breakadd(eap);
    }
}

// Command line expansion for :profile.
ସ୍ଥିର ଗଣନା
{
    PEXP_SUBCMD,	// expand :profile sub-commands
    PEXP_FUNC		// expand :profile func {funcname}
} pexpand_what;

ସ୍ଥିର ଅକ୍ଷର *pexpand_cmds[] = {
			"start",
			"stop",
			"pause",
			"continue",
			"func",
			"file",
			"dump",
			ଖାଲି
};

/*
 * Function given to ExpandGeneric() to obtain the profile command
 * specific expansion.
 */
    char_u *
get_profile_name(expand_T *xp UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ବାଛ (pexpand_what)
    {
    ସ୍ଥିତି PEXP_SUBCMD:
	ଫେରନ୍ତୁ (char_u *)pexpand_cmds[idx];
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	ଫେରନ୍ତୁ ଖାଲି;
    }
}

/*
 * Handle command line completion for :profile command.
 */
    ଶୂନ୍ୟ
set_context_in_profile_cmd(expand_T *xp, char_u *arg)
{
    char_u	*end_subcmd;

    // Default: expand subcommands.
    xp->xp_context = EXPAND_PROFILE;
    pexpand_what = PEXP_SUBCMD;
    xp->xp_pattern = arg;

    end_subcmd = skiptowhite(arg);
    ଯଦି (*end_subcmd == NUL)
	ଫେରନ୍ତୁ;

    ଯଦି ((end_subcmd - arg == 5 && STRNCMP(arg, "start", 5) == 0)
	    || (end_subcmd - arg == 4 && STRNCMP(arg, "file", 4) == 0))
    {
	xp->xp_context = EXPAND_FILES;
	xp->xp_pattern = skipwhite(end_subcmd);
	ଫେରନ୍ତୁ;
    }
    ଅନ୍ୟଥା ଯଦି (end_subcmd - arg == 4 && STRNCMP(arg, "func", 4) == 0)
    {
	xp->xp_context = EXPAND_USER_FUNC;
	xp->xp_pattern = skipwhite(end_subcmd);
	ଫେରନ୍ତୁ;
    }

    xp->xp_context = EXPAND_NOTHING;
}

ସ୍ଥିର proftime_T inchar_time;

/*
 * Called when starting to wait for the user to type a character.
 */
    ଶୂନ୍ୟ
prof_inchar_enter(ଶୂନ୍ୟ)
{
    profile_start(&inchar_time);
}

/*
 * Called when finished waiting for the user to type a character.
 */
    ଶୂନ୍ୟ
prof_inchar_exit(ଶୂନ୍ୟ)
{
    profile_end(&inchar_time);
    profile_add(&prof_wait_time, &inchar_time);
}


/*
 * Return TRUE when a function defined in the current script should be
 * profiled.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
prof_def_func(ଶୂନ୍ୟ)
{
    ଯଦି (current_sctx.sc_sid > 0)
	ଫେରନ୍ତୁ SCRIPT_ITEM(current_sctx.sc_sid)->sn_pr_force;
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Print the count and times for one function or function line.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
prof_func_ଧାଡ଼ି(
    FILE	*fd,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଗଣନା_2,
    proftime_T	*total,
    proftime_T	*self,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prefer_self)	// when equal print only self time
{
    ଯଦି (ଗଣନା_2 > 0)
    {
	fprintf(fd, "%5d ", ଗଣନା_2);
	ଯଦି (prefer_self && profile_equal(total, self))
	    fprintf(fd, PROF_TIME_BLANK);
	ଅନ୍ୟଥା
	    fprintf(fd, "%s ", profile_ସନ୍ଦେଶ(total));
	ଯଦି (!prefer_self && profile_equal(total, self))
	    fprintf(fd, PROF_TIME_BLANK);
	ଅନ୍ୟଥା
	    fprintf(fd, "%s ", profile_ସନ୍ଦେଶ(self));
    }
    ଅନ୍ୟଥା
	fprintf(fd, "      %s%s", PROF_TIME_BLANK, PROF_TIME_BLANK);
}

    ସ୍ଥିର ଶୂନ୍ୟ
prof_sort_list(
    FILE	*fd,
    ufunc_T	**sorttab,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		st_len,
    ଅକ୍ଷର	*title,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prefer_self)	// when equal print only self time
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ufunc_T	*fp;

    fprintf(fd, "FUNCTIONS SORTED ON %s TIME\n", title);
    fprintf(fd, "%s  function\n", PROF_TOTALS_HEADER);
    ପାଇଁ (i = 0; i < 20 && i < st_len; ++i)
    {
	fp = sorttab[i];
	prof_func_ଧାଡ଼ି(fd, fp->uf_tm_ଗଣନା, &fp->uf_tm_total, &fp->uf_tm_self,
								 prefer_self);
	ଯଦି (fp->uf_name[0] == K_SPECIAL)
	    fprintf(fd, " <SNR>%s()\n", fp->uf_name + 3);
	ଅନ୍ୟଥା
	    fprintf(fd, " %s()\n", fp->uf_name);
    }
    fprintf(fd, "\n");
}

/*
 * Compare function for total time sorting.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
prof_total_cmp(ଅଚଳ ଶୂନ୍ୟ *s1, ଅଚଳ ଶୂନ୍ୟ *s2)
{
    ufunc_T	*p1, *p2;

    p1 = *(ufunc_T **)s1;
    p2 = *(ufunc_T **)s2;
    ଫେରନ୍ତୁ profile_cmp(&p1->uf_tm_total, &p2->uf_tm_total);
}

/*
 * Compare function for self time sorting.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
prof_self_cmp(ଅଚଳ ଶୂନ୍ୟ *s1, ଅଚଳ ଶୂନ୍ୟ *s2)
{
    ufunc_T	*p1, *p2;

    p1 = *(ufunc_T **)s1;
    p2 = *(ufunc_T **)s2;
    ଫେରନ୍ତୁ profile_cmp(&p1->uf_tm_self, &p2->uf_tm_self);
}

/*
 * Start profiling function "fp".
 */
    ଶୂନ୍ୟ
func_do_profile(ufunc_T *fp)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len = fp->uf_ଧାଡ଼ିଗୁଡ଼ିକ.ga_len;

    ଯଦି (!fp->uf_prof_initialized)
    {
	ଯଦି (len == 0)
	    len = 1;  // avoid getting error for allocating zero bytes
	fp->uf_tm_ଗଣନା = 0;
	profile_zero(&fp->uf_tm_self);
	profile_zero(&fp->uf_tm_total);
	ଯଦି (fp->uf_tml_ଗଣନା == ଖାଲି)
	    fp->uf_tml_ଗଣନା = ALLOC_CLEAR_MULT(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, len);
	ଯଦି (fp->uf_tml_total == ଖାଲି)
	    fp->uf_tml_total = ALLOC_CLEAR_MULT(proftime_T, len);
	ଯଦି (fp->uf_tml_self == ଖାଲି)
	    fp->uf_tml_self = ALLOC_CLEAR_MULT(proftime_T, len);
	fp->uf_tml_idx = -1;
	ଯଦି (fp->uf_tml_ଗଣନା == ଖାଲି || fp->uf_tml_total == ଖାଲି
						    || fp->uf_tml_self == ଖାଲି)
	    ଫେରନ୍ତୁ;	    // out of memory
	fp->uf_prof_initialized = TRUE;
    }

    fp->uf_profiling = TRUE;
}

/*
 * Save time when starting to invoke another script or function.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
script_prof_save(
    proftime_T	*tm)	    // place to store wait time
{
    scriptitem_T    *si;

    ଯଦି (SCRIPT_ID_VALID(current_sctx.sc_sid))
    {
	si = SCRIPT_ITEM(current_sctx.sc_sid);
	ଯଦି (si->sn_prof_on && si->sn_pr_nest++ == 0)
	    profile_start(&si->sn_pr_child);
    }
    profile_get_wait(tm);
}

/*
 * When calling a function: may initialize for profiling.
 */
    ଶୂନ୍ୟ
profile_may_start_func(profinfo_T *info, ufunc_T *fp, ufunc_T *caller)
{
    ଯଦି (!fp->uf_profiling && has_profiling(FALSE, fp->uf_name, ଖାଲି,
								&fp->uf_hash))
    {
	info->pi_started_profiling = TRUE;
	func_do_profile(fp);
    }
    ଯଦି (fp->uf_profiling || (caller != ଖାଲି && caller->uf_profiling))
    {
	++fp->uf_tm_ଗଣନା;
	profile_start(&info->pi_call_start);
	profile_zero(&fp->uf_tm_children);
    }
    script_prof_save(&info->pi_wait_start);
}

/*
 * After calling a function: may handle profiling.  profile_may_start_func()
 * must have been called previously.
 */
    ଶୂନ୍ୟ
profile_may_end_func(profinfo_T *info, ufunc_T *fp, ufunc_T *caller)
{
    profile_end(&info->pi_call_start);
    profile_sub_wait(&info->pi_wait_start, &info->pi_call_start);
    profile_add(&fp->uf_tm_total, &info->pi_call_start);
    profile_self(&fp->uf_tm_self, &info->pi_call_start, &fp->uf_tm_children);
    ଯଦି (caller != ଖାଲି && caller->uf_profiling)
    {
	profile_add(&caller->uf_tm_children, &info->pi_call_start);
	profile_add(&caller->uf_tml_children, &info->pi_call_start);
    }
    ଯଦି (info->pi_started_profiling)
	// make a ":profdel func" stop profiling the function
	fp->uf_profiling = FALSE;
}

/*
 * Prepare profiling for entering a child or something else that is not
 * counted for the script/function itself.
 * Should always be called in pair with prof_child_exit().
 */
    ଶୂନ୍ୟ
prof_child_enter(
    proftime_T *tm)	// place to store waittime
{
    funccall_T *fc = get_current_funccal();

    ଯଦି (fc != ଖାଲି && fc->fc_func->uf_profiling)
	profile_start(&fc->fc_prof_child);
    script_prof_save(tm);
}

/*
 * Take care of time spent in a child.
 * Should always be called after prof_child_enter().
 */
    ଶୂନ୍ୟ
prof_child_exit(
    proftime_T *tm)	// where waittime was stored
{
    funccall_T *fc = get_current_funccal();

    ଯଦି (fc != ଖାଲି && fc->fc_func->uf_profiling)
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
    ଶୂନ୍ୟ
func_ଧାଡ଼ି_start(ଶୂନ୍ୟ *cookie, long lnum)
{
    funccall_T	*fcp = (funccall_T *)cookie;
    ufunc_T	*fp = fcp->fc_func;

    ଯଦି (fp->uf_profiling && lnum >= 1 && lnum <= fp->uf_ଧାଡ଼ିଗୁଡ଼ିକ.ga_len)
    {
	fp->uf_tml_idx = lnum - 1;
	// Skip continuation lines.
	ଯେତେବେଳେ (fp->uf_tml_idx > 0 && FUNCLINE(fp, fp->uf_tml_idx) == ଖାଲି)
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
    ଶୂନ୍ୟ
func_ଧାଡ଼ି_exec(ଶୂନ୍ୟ *cookie)
{
    funccall_T	*fcp = (funccall_T *)cookie;
    ufunc_T	*fp = fcp->fc_func;

    ଯଦି (fp->uf_profiling && fp->uf_tml_idx >= 0)
	fp->uf_tml_execed = TRUE;
}

/*
 * Called when done with a function line.
 */
    ଶୂନ୍ୟ
func_ଧାଡ଼ି_end(ଶୂନ୍ୟ *cookie)
{
    funccall_T	*fcp = (funccall_T *)cookie;
    ufunc_T	*fp = fcp->fc_func;

    ଯଦି (fp->uf_profiling && fp->uf_tml_idx >= 0)
    {
	ଯଦି (fp->uf_tml_execed)
	{
	    ++fp->uf_tml_ଗଣନା[fp->uf_tml_idx];
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
    ସ୍ଥିର ଶୂନ୍ୟ
func_dump_profile(FILE *fd)
{
    hashtab_T	*functbl;
    hashitem_T	*hi;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		todo;
    ufunc_T	*fp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ufunc_T	**sorttab;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		st_len = 0;
    char_u	*p;

    functbl = func_tbl_get();
    todo = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)functbl->ht_used;
    ଯଦି (todo == 0)
	ଫେରନ୍ତୁ;     // nothing to dump

    sorttab = ALLOC_MULT(ufunc_T *, todo);

    FOR_ALL_HASHTAB_ବସ୍ତୁଗୁଡ଼ିକ(functbl, hi, todo)
    {
	ଯଦି (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    fp = HI2UF(hi);
	    ଯଦି (fp->uf_prof_initialized)
	    {
		ଯଦି (sorttab != ଖାଲି)
		    sorttab[st_len++] = fp;

		ଯଦି (fp->uf_name[0] == K_SPECIAL)
		    fprintf(fd, "FUNCTION  <SNR>%s()\n", fp->uf_name + 3);
		ଅନ୍ୟଥା
		    fprintf(fd, "FUNCTION  %s()\n", fp->uf_name);
		ଯଦି (fp->uf_script_ctx.sc_sid > 0)
		{
		    p = home_replace_save(ଖାଲି,
				     get_scriptname(fp->uf_script_ctx.sc_sid));
		    ଯଦି (p != ଖାଲି)
		    {
			fprintf(fd, "    Defined: %s:%ld\n",
					   p, (long)fp->uf_script_ctx.sc_lnum);
			vim_free(p);
		    }
		}
		ଯଦି (fp->uf_tm_ଗଣନା == 1)
		    fprintf(fd, "Called 1 time\n");
		ଅନ୍ୟଥା
		    fprintf(fd, "Called %d times\n", fp->uf_tm_ଗଣନା);
		fprintf(fd, "Total time: %s\n", profile_ସନ୍ଦେଶ(&fp->uf_tm_total));
		fprintf(fd, " Self time: %s\n", profile_ସନ୍ଦେଶ(&fp->uf_tm_self));
		fprintf(fd, "\n");
		fprintf(fd, "%s\n", PROF_TOTALS_HEADER);

		ପାଇଁ (i = 0; i < fp->uf_ଧାଡ଼ିଗୁଡ଼ିକ.ga_len; ++i)
		{
		    ଯଦି (FUNCLINE(fp, i) == ଖାଲି)
			ଜାରି;
		    prof_func_ଧାଡ଼ି(fd, fp->uf_tml_ଗଣନା[i],
			     &fp->uf_tml_total[i], &fp->uf_tml_self[i], TRUE);
		    fprintf(fd, "%s\n", FUNCLINE(fp, i));
		}
		fprintf(fd, "\n");
	    }
	}
    }

    ଯଦି (sorttab != ଖାଲି && st_len > 0)
    {
	qsort((ଶୂନ୍ୟ *)sorttab, (size_t)st_len, ଆକାର(ufunc_T *),
							      prof_total_cmp);
	prof_sort_list(fd, sorttab, st_len, "TOTAL", FALSE);
	qsort((ଶୂନ୍ୟ *)sorttab, (size_t)st_len, ଆକାର(ufunc_T *),
							      prof_self_cmp);
	prof_sort_list(fd, sorttab, st_len, "SELF", TRUE);
    }

    vim_free(sorttab);
}

/*
 * Start profiling script "fp".
 */
    ଶୂନ୍ୟ
script_do_profile(scriptitem_T *si)
{
    si->sn_pr_ଗଣନା = 0;
    profile_zero(&si->sn_pr_total);
    profile_zero(&si->sn_pr_self);

    ga_init2(&si->sn_prl_ga, ଆକାର(sn_prl_T), 100);
    si->sn_prl_idx = -1;
    si->sn_prof_on = TRUE;
    si->sn_pr_nest = 0;
}

/*
 * Count time spent in children after invoking another script or function.
 */
    ଶୂନ୍ୟ
script_prof_restore(proftime_T *tm)
{
    scriptitem_T    *si;

    ଯଦି (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	ଫେରନ୍ତୁ;

    si = SCRIPT_ITEM(current_sctx.sc_sid);
    ଯଦି (si->sn_prof_on && --si->sn_pr_nest == 0)
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
    ସ୍ଥିର ଶୂନ୍ୟ
script_dump_profile(FILE *fd)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    id;
    scriptitem_T    *si;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    i;
    FILE	    *sfd;
    sn_prl_T	    *pp;

    ପାଇଁ (id = 1; id <= script_ବସ୍ତୁଗୁଡ଼ିକ.ga_len; ++id)
    {
	si = SCRIPT_ITEM(id);
	ଯଦି (si->sn_prof_on)
	{
	    fprintf(fd, "SCRIPT  %s\n", si->sn_name);
	    ଯଦି (si->sn_pr_ଗଣନା == 1)
		fprintf(fd, "Sourced 1 time\n");
	    ଅନ୍ୟଥା
		fprintf(fd, "Sourced %d times\n", si->sn_pr_ଗଣନା);
	    fprintf(fd, "Total time: %s\n", profile_ସନ୍ଦେଶ(&si->sn_pr_total));
	    fprintf(fd, " Self time: %s\n", profile_ସନ୍ଦେଶ(&si->sn_pr_self));
	    fprintf(fd, "\n");
	    fprintf(fd, "%s\n", PROF_TOTALS_HEADER);

	    sfd = mch_fopen((ଅକ୍ଷର *)si->sn_name, "r");
	    ଯଦି (sfd == ଖାଲି)
		fprintf(fd, "Cannot open file!\n");
	    ଅନ୍ୟଥା
	    {
		// Keep going till the end of file, so that trailing
		// continuation lines are listed.
		ପାଇଁ (i = 0; ; ++i)
		{
		    ଯଦି (vim_fgets(IObuff, IOSIZE, sfd))
			ଭାଙ୍ଗ;
		    // When a line has been truncated, append NL, taking care
		    // of multi-byte characters .
		    ଯଦି (IObuff[IOSIZE - 2] != NUL && IObuff[IOSIZE - 2] != NL)
		    {
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା n = IOSIZE - 2;

			ଯଦି (enc_utf8)
			{
			    // Move to the first byte of this char.
			    // utf_head_off() doesn't work, because it checks
			    // for a truncated character.
			    ଯେତେବେଳେ (n > 0 && (IObuff[n] & 0xc0) == 0x80)
				--n;
			}
			ଅନ୍ୟଥା ଯଦି (has_mbyte)
			    n -= mb_head_off(IObuff, IObuff + n);
			IObuff[n] = NL;
			IObuff[n + 1] = NUL;
		    }
		    ଯଦି (i < si->sn_prl_ga.ga_len
				     && (pp = &PRL_ITEM(si, i))->snp_ଗଣନା > 0)
		    {
			fprintf(fd, "%5d ", pp->snp_ଗଣନା);
			ଯଦି (profile_equal(&pp->sn_prl_total, &pp->sn_prl_self))
			    fprintf(fd, "           ");
			ଅନ୍ୟଥା
			    fprintf(fd, "%s ", profile_ସନ୍ଦେଶ(&pp->sn_prl_total));
			fprintf(fd, "%s ", profile_ସନ୍ଦେଶ(&pp->sn_prl_self));
		    }
		    ଅନ୍ୟଥା
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
    ଶୂନ୍ୟ
profile_dump(ଶୂନ୍ୟ)
{
    FILE	*fd;

    ଯଦି (profile_fname == ଖାଲି)
	ଫେରନ୍ତୁ;

    fd = mch_fopen((ଅକ୍ଷର *)profile_fname, "w");
    ଯଦି (fd == ଖାଲି)
	semsg(_(e_cant_open_file_str), profile_fname);
    ଅନ୍ୟଥା
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
    ଶୂନ୍ୟ
script_ଧାଡ଼ି_start(ଶୂନ୍ୟ)
{
    scriptitem_T    *si;
    sn_prl_T	    *pp;

    ଯଦି (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	ଫେରନ୍ତୁ;
    si = SCRIPT_ITEM(current_sctx.sc_sid);
    ଯଦି (si->sn_prof_on && SOURCING_LNUM >= 1)
    {
	// Grow the array before starting the timer, so that the time spent
	// here isn't counted.
	(ଶୂନ୍ୟ)ga_grow(&si->sn_prl_ga,
				  (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(SOURCING_LNUM - si->sn_prl_ga.ga_len));
	si->sn_prl_idx = SOURCING_LNUM - 1;
	ଯେତେବେଳେ (si->sn_prl_ga.ga_len <= si->sn_prl_idx
		&& si->sn_prl_ga.ga_len < si->sn_prl_ga.ga_maxlen)
	{
	    // Zero counters for a line that was not used before.
	    pp = &PRL_ITEM(si, si->sn_prl_ga.ga_len);
	    pp->snp_ଗଣନା = 0;
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
    ଶୂନ୍ୟ
script_ଧାଡ଼ି_exec(ଶୂନ୍ୟ)
{
    scriptitem_T    *si;

    ଯଦି (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	ଫେରନ୍ତୁ;
    si = SCRIPT_ITEM(current_sctx.sc_sid);
    ଯଦି (si->sn_prof_on && si->sn_prl_idx >= 0)
	si->sn_prl_execed = TRUE;
}

/*
 * Called when done with a script line.
 */
    ଶୂନ୍ୟ
script_ଧାଡ଼ି_end(ଶୂନ୍ୟ)
{
    scriptitem_T    *si;
    sn_prl_T	    *pp;

    ଯଦି (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	ଫେରନ୍ତୁ;
    si = SCRIPT_ITEM(current_sctx.sc_sid);
    ଯଦି (si->sn_prof_on && si->sn_prl_idx >= 0
				     && si->sn_prl_idx < si->sn_prl_ga.ga_len)
    {
	ଯଦି (si->sn_prl_execed)
	{
	    pp = &PRL_ITEM(si, si->sn_prl_idx);
	    ++pp->snp_ଗଣନା;
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
