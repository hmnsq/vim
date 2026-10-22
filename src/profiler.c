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

#include "vim.h"

#གལ་ཏེ defined(FEAT_EVAL)
# གལ་ཏེ defined(FEAT_PROFILE) || defined(FEAT_RELTIME)
/*
 * Store the current time in "tm".
 */
    void
profile_start(proftime_T *tm)
{
#  ifdef MSWIN
    QueryPerformanceCounter(tm);
#  དེ་མིན
    PROF_GET_TIME(tm);
#  endif
}

/*
 * Compute the elapsed time from "tm" till now and store in "tm".
 */
    void
profile_end(proftime_T *tm)
{
    proftime_T now;

#  ifdef MSWIN
    QueryPerformanceCounter(&now);
    tm->QuadPart = now.QuadPart - tm->QuadPart;
#  དེ་མིན
    PROF_GET_TIME(&now);
    tm->tv_fsec = now.tv_fsec - tm->tv_fsec;
    tm->tv_sec = now.tv_sec - tm->tv_sec;
    གལ་ཏེ (tm->tv_fsec < 0)
    {
	tm->tv_fsec += TV_FSEC_SEC;
	--tm->tv_sec;
    }
#  endif
}

/*
 * Subtract the time "tm2" from "tm".
 */
    void
profile_sub(proftime_T *tm, proftime_T *tm2)
{
#  ifdef MSWIN
    tm->QuadPart -= tm2->QuadPart;
#  དེ་མིན
    tm->tv_fsec -= tm2->tv_fsec;
    tm->tv_sec -= tm2->tv_sec;
    གལ་ཏེ (tm->tv_fsec < 0)
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
    var_na *
profile_msg(proftime_T *tm)
{
    static var_na buf[50];

#  ifdef MSWIN
    LARGE_INTEGER   fr;

    QueryPerformanceFrequency(&fr);
    sprintf(buf, "%10.6lf", (dviguu_na)tm->QuadPart / (dviguu_na)fr.QuadPart);
#  དེ་མིན
    sprintf(buf, PROF_TIME_FORMAT, (long)tm->tv_sec, (long)tm->tv_fsec);
#  endif
    ཕྱིར buf;
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
    ཕྱིར (float_T)tm->QuadPart / (float_T)fr.QuadPart;
#  དེ་མིན
    ཕྱིར (float_T)tm->tv_sec + (float_T)tm->tv_fsec / (float_T)TV_FSEC_SEC;
#  endif
}

/*
 * Put the time "msec" past now in "tm".
 */
    void
profile_setlimit(long msec, proftime_T *tm)
{
    གལ་ཏེ (msec <= 0)   // no limit
	profile_zero(tm);
    དེ་མིན
    {
#  ifdef MSWIN
	LARGE_INTEGER   fr;

	QueryPerformanceCounter(tm);
	QueryPerformanceFrequency(&fr);
	tm->QuadPart += (LONGLONG)((dviguu_na)msec / 1000.0 * (dviguu_na)fr.QuadPart);
#  དེ་མིན
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
    ཧྲིལ
profile_passed_limit(proftime_T *tm)
{
    proftime_T	now;

#  ifdef MSWIN
    གལ་ཏེ (tm->QuadPart == 0)  // timer was not set
	ཕྱིར FALSE;
    QueryPerformanceCounter(&now);
    ཕྱིར (now.QuadPart > tm->QuadPart);
#  དེ་མིན
    གལ་ཏེ (tm->tv_sec == 0)    // timer was not set
	ཕྱིར FALSE;
    PROF_GET_TIME(&now);
    ཕྱིར (now.tv_sec > tm->tv_sec
	    || (now.tv_sec == tm->tv_sec && now.tv_fsec > tm->tv_fsec));
#  endif
}

/*
 * Set the time in "tm" to zero.
 */
    void
profile_zero(proftime_T *tm)
{
#  ifdef MSWIN
    tm->QuadPart = 0;
#  དེ་མིན
    tm->tv_fsec = 0;
    tm->tv_sec = 0;
#  endif
}

# endif  // FEAT_PROFILE || FEAT_RELTIME

# གལ་ཏེ defined(FEAT_SYN_HL) && defined(FEAT_RELTIME) && defined(FEAT_PROFILE)
#  གལ་ཏེ defined(HAVE_MATH_H)
#   include <math.h>
#  endif

/*
 * Divide the time "tm" by "count" and store in "tm2".
 */
    void
profile_divide(proftime_T *tm, ཧྲིལ count, proftime_T *tm2)
{
    གལ་ཏེ (count == 0)
	profile_zero(tm2);
    དེ་མིན
    {
#  ifdef MSWIN
	tm2->QuadPart = tm->QuadPart / count;
#  དེ་མིན
	dviguu_na fsec = (tm->tv_sec * (float_T)TV_FSEC_SEC + tm->tv_fsec)
								       / count;

	tm2->tv_sec = floor(fsec / (float_T)TV_FSEC_SEC);
	tm2->tv_fsec = vim_round(fsec - (tm2->tv_sec * (float_T)TV_FSEC_SEC));
#  endif
    }
}
# endif

# གལ་ཏེ defined(FEAT_PROFILE)
/*
 * Functions for profiling.
 */
static proftime_T prof_wait_time;

/*
 * Add the time "tm2" to "tm".
 */
    void
profile_add(proftime_T *tm, proftime_T *tm2)
{
#  ifdef MSWIN
    tm->QuadPart += tm2->QuadPart;
#  དེ་མིན
    tm->tv_fsec += tm2->tv_fsec;
    tm->tv_sec += tm2->tv_sec;
    གལ་ཏེ (tm->tv_fsec >= TV_FSEC_SEC)
    {
	tm->tv_fsec -= TV_FSEC_SEC;
	++tm->tv_sec;
    }
#  endif
}

/*
 * Add the "self" time from the total time and the children's time.
 */
    void
profile_self(proftime_T *self, proftime_T *total, proftime_T *children)
{
    // Check that the result won't be negative.  Can happen with recursive
    // calls.
#  ifdef MSWIN
    གལ་ཏེ (total->QuadPart <= children->QuadPart)
	ཕྱིར;
#  དེ་མིན
    གལ་ཏེ (total->tv_sec < children->tv_sec
	    || (total->tv_sec == children->tv_sec
		&& total->tv_fsec <= children->tv_fsec))
	ཕྱིར;
#  endif
    profile_add(self, total);
    profile_sub(self, children);
}

/*
 * Get the current waittime.
 */
    static void
profile_get_wait(proftime_T *tm)
{
    *tm = prof_wait_time;
}

/*
 * Subtract the passed waittime since "tm" from "tma".
 */
    void
profile_sub_wait(proftime_T *tm, proftime_T *tma)
{
    proftime_T tm3 = prof_wait_time;

    profile_sub(&tm3, tm);
    profile_sub(tma, &tm3);
}

/*
 * Return TRUE if "tm1" and "tm2" are equal.
 */
    static ཧྲིལ
profile_equal(proftime_T *tm1, proftime_T *tm2)
{
#  ifdef MSWIN
    ཕྱིར (tm1->QuadPart == tm2->QuadPart);
#  དེ་མིན
    ཕྱིར (tm1->tv_fsec == tm2->tv_fsec && tm1->tv_sec == tm2->tv_sec);
#  endif
}

/*
 * Return <0, 0 or >0 if "tm1" < "tm2", "tm1" == "tm2" or "tm1" > "tm2"
 */
    ཧྲིལ
profile_cmp(const proftime_T *tm1, const proftime_T *tm2)
{
#  ifdef MSWIN
    ཕྱིར tm2->QuadPart == tm1->QuadPart ? 0 :
	tm2->QuadPart > tm1->QuadPart ? 1 : -1;
#  དེ་མིན
    གལ་ཏེ (tm1->tv_sec == tm2->tv_sec)
	ཕྱིར tm2->tv_fsec == tm1->tv_fsec ? 0 :
	    tm2->tv_fsec > tm1->tv_fsec ? 1 : -1;
    ཕྱིར tm2->tv_sec > tm1->tv_sec ? 1 : -1;
#  endif
}

static char_u	*profile_fname = NULL;
static proftime_T pause_time;

/*
 * Reset all profiling information.
 */
    static void
profile_reset(void)
{
    ཧྲིལ		id;
    ཧྲིལ		todo;
    hashtab_T	*functbl;
    hashitem_T	*hi;

    // Reset sourced files.
    ཆེད་དུ (id = 1; id <= script_items.ga_len; id++)
    {
	scriptitem_T *si = SCRIPT_ITEM(id);
	གལ་ཏེ (si->sn_prof_on)
	{
	    si->sn_prof_on      = FALSE;
	    si->sn_pr_force     = FALSE;
	    profile_zero(&si->sn_pr_child);
	    si->sn_pr_nest      = 0;
	    si->sn_pr_count     = 0;
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
    todo = (ཧྲིལ)functbl->ht_used;

    FOR_ALL_HASHTAB_ITEMS(functbl, hi, todo)
    {
	ufunc_T *fp;
	ཧྲིལ	i;

	གལ་ཏེ (HASHITEM_EMPTY(hi))
	    continue;

	todo--;
	fp = HI2UF(hi);
	གལ་ཏེ (fp->uf_prof_initialized)
	{
	    fp->uf_profiling    = 0;
	    fp->uf_prof_initialized = FALSE;
	    fp->uf_tm_count     = 0;
	    profile_zero(&fp->uf_tm_total);
	    profile_zero(&fp->uf_tm_self);
	    profile_zero(&fp->uf_tm_children);

	    ཆེད་དུ (i = 0; i < fp->uf_lines.ga_len; i++)
	    {
		fp->uf_tml_count[i] = 0;
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
    void
ex_profile(exarg_T *eap)
{
    char_u	*e;
    ཧྲིལ		len;

    e = skiptowhite(eap->arg);
    len = (ཧྲིལ)(e - eap->arg);
    e = skipwhite(e);

    གལ་ཏེ (len == 5 && STRNCMP(eap->arg, "start", 5) == 0 && *e != NUL)
    {
	VIM_CLEAR(profile_fname);
	profile_fname = expand_env_save_opt(e, TRUE, NULL);
	do_profiling = PROF_YES;
	profile_zero(&prof_wait_time);
	set_vim_var_nr(VV_PROFILING, 1L);
    }
    དེ་མིན གལ་ཏེ (do_profiling == PROF_NONE)
	emsg(_(e_first_use_profile_start_fname));
    དེ་མིན གལ་ཏེ (STRCMP(eap->arg, "stop") == 0)
    {
	profile_dump();
	do_profiling = PROF_NONE;
	set_vim_var_nr(VV_PROFILING, 0L);
	profile_reset();
    }
    དེ་མིན གལ་ཏེ (STRCMP(eap->arg, "pause") == 0)
    {
	གལ་ཏེ (do_profiling == PROF_YES)
	    profile_start(&pause_time);
	do_profiling = PROF_PAUSED;
    }
    དེ་མིན གལ་ཏེ (STRCMP(eap->arg, "continue") == 0)
    {
	གལ་ཏེ (do_profiling == PROF_PAUSED)
	{
	    profile_end(&pause_time);
	    profile_add(&prof_wait_time, &pause_time);
	}
	do_profiling = PROF_YES;
    }
    དེ་མིན གལ་ཏེ (STRCMP(eap->arg, "dump") == 0)
	profile_dump();
    དེ་མིན
    {
	// The rest is similar to ":breakadd".
	ex_breakadd(eap);
    }
}

// Command line expansion for :profile.
static ga_nanaa
{
    PEXP_SUBCMD,	// expand :profile sub-commands
    PEXP_FUNC		// expand :profile func {funcname}
} pexpand_what;

static var_na *pexpand_cmds[] = {
			"start",
			"stop",
			"pause",
			"continue",
			"func",
			"file",
			"dump",
			NULL
};

/*
 * Function given to ExpandGeneric() to obtain the profile command
 * specific expansion.
 */
    char_u *
get_profile_name(expand_T *xp UNUSED, ཧྲིལ idx)
{
    switch (pexpand_what)
    {
    pak_sa PEXP_SUBCMD:
	ཕྱིར (char_u *)pexpand_cmds[idx];
    default:
	ཕྱིར NULL;
    }
}

/*
 * Handle command line completion for :profile command.
 */
    void
set_context_in_profile_cmd(expand_T *xp, char_u *arg)
{
    char_u	*end_subcmd;

    // Default: expand subcommands.
    xp->xp_context = EXPAND_PROFILE;
    pexpand_what = PEXP_SUBCMD;
    xp->xp_pattern = arg;

    end_subcmd = skiptowhite(arg);
    གལ་ཏེ (*end_subcmd == NUL)
	ཕྱིར;

    གལ་ཏེ ((end_subcmd - arg == 5 && STRNCMP(arg, "start", 5) == 0)
	    || (end_subcmd - arg == 4 && STRNCMP(arg, "file", 4) == 0))
    {
	xp->xp_context = EXPAND_FILES;
	xp->xp_pattern = skipwhite(end_subcmd);
	ཕྱིར;
    }
    དེ་མིན གལ་ཏེ (end_subcmd - arg == 4 && STRNCMP(arg, "func", 4) == 0)
    {
	xp->xp_context = EXPAND_USER_FUNC;
	xp->xp_pattern = skipwhite(end_subcmd);
	ཕྱིར;
    }

    xp->xp_context = EXPAND_NOTHING;
}

static proftime_T inchar_time;

/*
 * Called when starting to wait for the user to type a character.
 */
    void
prof_inchar_enter(void)
{
    profile_start(&inchar_time);
}

/*
 * Called when finished waiting for the user to type a character.
 */
    void
prof_inchar_exit(void)
{
    profile_end(&inchar_time);
    profile_add(&prof_wait_time, &inchar_time);
}


/*
 * Return TRUE when a function defined in the current script should be
 * profiled.
 */
    ཧྲིལ
prof_def_func(void)
{
    གལ་ཏེ (current_sctx.sc_sid > 0)
	ཕྱིར SCRIPT_ITEM(current_sctx.sc_sid)->sn_pr_force;
    ཕྱིར FALSE;
}

/*
 * Print the count and times for one function or function line.
 */
    static void
prof_func_line(
    FILE	*fd,
    ཧྲིལ		count,
    proftime_T	*total,
    proftime_T	*self,
    ཧྲིལ		prefer_self)	// when equal print only self time
{
    གལ་ཏེ (count > 0)
    {
	fprintf(fd, "%5d ", count);
	གལ་ཏེ (prefer_self && profile_equal(total, self))
	    fprintf(fd, PROF_TIME_BLANK);
	དེ་མིན
	    fprintf(fd, "%s ", profile_msg(total));
	གལ་ཏེ (!prefer_self && profile_equal(total, self))
	    fprintf(fd, PROF_TIME_BLANK);
	དེ་མིན
	    fprintf(fd, "%s ", profile_msg(self));
    }
    དེ་མིན
	fprintf(fd, "      %s%s", PROF_TIME_BLANK, PROF_TIME_BLANK);
}

    static void
prof_sort_list(
    FILE	*fd,
    ufunc_T	**sorttab,
    ཧྲིལ		st_len,
    var_na	*title,
    ཧྲིལ		prefer_self)	// when equal print only self time
{
    ཧྲིལ		i;
    ufunc_T	*fp;

    fprintf(fd, "FUNCTIONS SORTED ON %s TIME\n", title);
    fprintf(fd, "%s  function\n", PROF_TOTALS_HEADER);
    ཆེད་དུ (i = 0; i < 20 && i < st_len; ++i)
    {
	fp = sorttab[i];
	prof_func_line(fd, fp->uf_tm_count, &fp->uf_tm_total, &fp->uf_tm_self,
								 prefer_self);
	གལ་ཏེ (fp->uf_name[0] == K_SPECIAL)
	    fprintf(fd, " <SNR>%s()\n", fp->uf_name + 3);
	དེ་མིན
	    fprintf(fd, " %s()\n", fp->uf_name);
    }
    fprintf(fd, "\n");
}

/*
 * Compare function for total time sorting.
 */
    static ཧྲིལ
prof_total_cmp(const void *s1, const void *s2)
{
    ufunc_T	*p1, *p2;

    p1 = *(ufunc_T **)s1;
    p2 = *(ufunc_T **)s2;
    ཕྱིར profile_cmp(&p1->uf_tm_total, &p2->uf_tm_total);
}

/*
 * Compare function for self time sorting.
 */
    static ཧྲིལ
prof_self_cmp(const void *s1, const void *s2)
{
    ufunc_T	*p1, *p2;

    p1 = *(ufunc_T **)s1;
    p2 = *(ufunc_T **)s2;
    ཕྱིར profile_cmp(&p1->uf_tm_self, &p2->uf_tm_self);
}

/*
 * Start profiling function "fp".
 */
    void
func_do_profile(ufunc_T *fp)
{
    ཧྲིལ		len = fp->uf_lines.ga_len;

    གལ་ཏེ (!fp->uf_prof_initialized)
    {
	གལ་ཏེ (len == 0)
	    len = 1;  // avoid getting error for allocating zero bytes
	fp->uf_tm_count = 0;
	profile_zero(&fp->uf_tm_self);
	profile_zero(&fp->uf_tm_total);
	གལ་ཏེ (fp->uf_tml_count == NULL)
	    fp->uf_tml_count = ALLOC_CLEAR_MULT(ཧྲིལ, len);
	གལ་ཏེ (fp->uf_tml_total == NULL)
	    fp->uf_tml_total = ALLOC_CLEAR_MULT(proftime_T, len);
	གལ་ཏེ (fp->uf_tml_self == NULL)
	    fp->uf_tml_self = ALLOC_CLEAR_MULT(proftime_T, len);
	fp->uf_tml_idx = -1;
	གལ་ཏེ (fp->uf_tml_count == NULL || fp->uf_tml_total == NULL
						    || fp->uf_tml_self == NULL)
	    ཕྱིར;	    // out of memory
	fp->uf_prof_initialized = TRUE;
    }

    fp->uf_profiling = TRUE;
}

/*
 * Save time when starting to invoke another script or function.
 */
    static void
script_prof_save(
    proftime_T	*tm)	    // place to store wait time
{
    scriptitem_T    *si;

    གལ་ཏེ (SCRIPT_ID_VALID(current_sctx.sc_sid))
    {
	si = SCRIPT_ITEM(current_sctx.sc_sid);
	གལ་ཏེ (si->sn_prof_on && si->sn_pr_nest++ == 0)
	    profile_start(&si->sn_pr_child);
    }
    profile_get_wait(tm);
}

/*
 * When calling a function: may initialize for profiling.
 */
    void
profile_may_start_func(profinfo_T *info, ufunc_T *fp, ufunc_T *caller)
{
    གལ་ཏེ (!fp->uf_profiling && has_profiling(FALSE, fp->uf_name, NULL,
								&fp->uf_hash))
    {
	info->pi_started_profiling = TRUE;
	func_do_profile(fp);
    }
    གལ་ཏེ (fp->uf_profiling || (caller != NULL && caller->uf_profiling))
    {
	++fp->uf_tm_count;
	profile_start(&info->pi_call_start);
	profile_zero(&fp->uf_tm_children);
    }
    script_prof_save(&info->pi_wait_start);
}

/*
 * After calling a function: may handle profiling.  profile_may_start_func()
 * must have been called previously.
 */
    void
profile_may_end_func(profinfo_T *info, ufunc_T *fp, ufunc_T *caller)
{
    profile_end(&info->pi_call_start);
    profile_sub_wait(&info->pi_wait_start, &info->pi_call_start);
    profile_add(&fp->uf_tm_total, &info->pi_call_start);
    profile_self(&fp->uf_tm_self, &info->pi_call_start, &fp->uf_tm_children);
    གལ་ཏེ (caller != NULL && caller->uf_profiling)
    {
	profile_add(&caller->uf_tm_children, &info->pi_call_start);
	profile_add(&caller->uf_tml_children, &info->pi_call_start);
    }
    གལ་ཏེ (info->pi_started_profiling)
	// make a ":profdel func" stop profiling the function
	fp->uf_profiling = FALSE;
}

/*
 * Prepare profiling for entering a child or something else that is not
 * counted for the script/function itself.
 * Should always be called in pair with prof_child_exit().
 */
    void
prof_child_enter(
    proftime_T *tm)	// place to store waittime
{
    funccall_T *fc = get_current_funccal();

    གལ་ཏེ (fc != NULL && fc->fc_func->uf_profiling)
	profile_start(&fc->fc_prof_child);
    script_prof_save(tm);
}

/*
 * Take care of time spent in a child.
 * Should always be called after prof_child_enter().
 */
    void
prof_child_exit(
    proftime_T *tm)	// where waittime was stored
{
    funccall_T *fc = get_current_funccal();

    གལ་ཏེ (fc != NULL && fc->fc_func->uf_profiling)
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
    void
func_line_start(void *cookie, long lnum)
{
    funccall_T	*fcp = (funccall_T *)cookie;
    ufunc_T	*fp = fcp->fc_func;

    གལ་ཏེ (fp->uf_profiling && lnum >= 1 && lnum <= fp->uf_lines.ga_len)
    {
	fp->uf_tml_idx = lnum - 1;
	// Skip continuation lines.
	སྐབས (fp->uf_tml_idx > 0 && FUNCLINE(fp, fp->uf_tml_idx) == NULL)
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
    void
func_line_exec(void *cookie)
{
    funccall_T	*fcp = (funccall_T *)cookie;
    ufunc_T	*fp = fcp->fc_func;

    གལ་ཏེ (fp->uf_profiling && fp->uf_tml_idx >= 0)
	fp->uf_tml_execed = TRUE;
}

/*
 * Called when done with a function line.
 */
    void
func_line_end(void *cookie)
{
    funccall_T	*fcp = (funccall_T *)cookie;
    ufunc_T	*fp = fcp->fc_func;

    གལ་ཏེ (fp->uf_profiling && fp->uf_tml_idx >= 0)
    {
	གལ་ཏེ (fp->uf_tml_execed)
	{
	    ++fp->uf_tml_count[fp->uf_tml_idx];
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
    static void
func_dump_profile(FILE *fd)
{
    hashtab_T	*functbl;
    hashitem_T	*hi;
    ཧྲིལ		todo;
    ufunc_T	*fp;
    ཧྲིལ		i;
    ufunc_T	**sorttab;
    ཧྲིལ		st_len = 0;
    char_u	*p;

    functbl = func_tbl_get();
    todo = (ཧྲིལ)functbl->ht_used;
    གལ་ཏེ (todo == 0)
	ཕྱིར;     // nothing to dump

    sorttab = ALLOC_MULT(ufunc_T *, todo);

    FOR_ALL_HASHTAB_ITEMS(functbl, hi, todo)
    {
	གལ་ཏེ (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    fp = HI2UF(hi);
	    གལ་ཏེ (fp->uf_prof_initialized)
	    {
		གལ་ཏེ (sorttab != NULL)
		    sorttab[st_len++] = fp;

		གལ་ཏེ (fp->uf_name[0] == K_SPECIAL)
		    fprintf(fd, "FUNCTION  <SNR>%s()\n", fp->uf_name + 3);
		དེ་མིན
		    fprintf(fd, "FUNCTION  %s()\n", fp->uf_name);
		གལ་ཏེ (fp->uf_script_ctx.sc_sid > 0)
		{
		    p = home_replace_save(NULL,
				     get_scriptname(fp->uf_script_ctx.sc_sid));
		    གལ་ཏེ (p != NULL)
		    {
			fprintf(fd, "    Defined: %s:%ld\n",
					   p, (long)fp->uf_script_ctx.sc_lnum);
			vim_free(p);
		    }
		}
		གལ་ཏེ (fp->uf_tm_count == 1)
		    fprintf(fd, "Called 1 time\n");
		དེ་མིན
		    fprintf(fd, "Called %d times\n", fp->uf_tm_count);
		fprintf(fd, "Total time: %s\n", profile_msg(&fp->uf_tm_total));
		fprintf(fd, " Self time: %s\n", profile_msg(&fp->uf_tm_self));
		fprintf(fd, "\n");
		fprintf(fd, "%s\n", PROF_TOTALS_HEADER);

		ཆེད་དུ (i = 0; i < fp->uf_lines.ga_len; ++i)
		{
		    གལ་ཏེ (FUNCLINE(fp, i) == NULL)
			continue;
		    prof_func_line(fd, fp->uf_tml_count[i],
			     &fp->uf_tml_total[i], &fp->uf_tml_self[i], TRUE);
		    fprintf(fd, "%s\n", FUNCLINE(fp, i));
		}
		fprintf(fd, "\n");
	    }
	}
    }

    གལ་ཏེ (sorttab != NULL && st_len > 0)
    {
	qsort((void *)sorttab, (size_t)st_len, parimaa_na(ufunc_T *),
							      prof_total_cmp);
	prof_sort_list(fd, sorttab, st_len, "TOTAL", FALSE);
	qsort((void *)sorttab, (size_t)st_len, parimaa_na(ufunc_T *),
							      prof_self_cmp);
	prof_sort_list(fd, sorttab, st_len, "SELF", TRUE);
    }

    vim_free(sorttab);
}

/*
 * Start profiling script "fp".
 */
    void
script_do_profile(scriptitem_T *si)
{
    si->sn_pr_count = 0;
    profile_zero(&si->sn_pr_total);
    profile_zero(&si->sn_pr_self);

    ga_init2(&si->sn_prl_ga, parimaa_na(sn_prl_T), 100);
    si->sn_prl_idx = -1;
    si->sn_prof_on = TRUE;
    si->sn_pr_nest = 0;
}

/*
 * Count time spent in children after invoking another script or function.
 */
    void
script_prof_restore(proftime_T *tm)
{
    scriptitem_T    *si;

    གལ་ཏེ (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	ཕྱིར;

    si = SCRIPT_ITEM(current_sctx.sc_sid);
    གལ་ཏེ (si->sn_prof_on && --si->sn_pr_nest == 0)
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
    static void
script_dump_profile(FILE *fd)
{
    ཧྲིལ		    id;
    scriptitem_T    *si;
    ཧྲིལ		    i;
    FILE	    *sfd;
    sn_prl_T	    *pp;

    ཆེད་དུ (id = 1; id <= script_items.ga_len; ++id)
    {
	si = SCRIPT_ITEM(id);
	གལ་ཏེ (si->sn_prof_on)
	{
	    fprintf(fd, "SCRIPT  %s\n", si->sn_name);
	    གལ་ཏེ (si->sn_pr_count == 1)
		fprintf(fd, "Sourced 1 time\n");
	    དེ་མིན
		fprintf(fd, "Sourced %d times\n", si->sn_pr_count);
	    fprintf(fd, "Total time: %s\n", profile_msg(&si->sn_pr_total));
	    fprintf(fd, " Self time: %s\n", profile_msg(&si->sn_pr_self));
	    fprintf(fd, "\n");
	    fprintf(fd, "%s\n", PROF_TOTALS_HEADER);

	    sfd = mch_fopen((var_na *)si->sn_name, "r");
	    གལ་ཏེ (sfd == NULL)
		fprintf(fd, "Cannot open file!\n");
	    དེ་མིན
	    {
		// Keep going till the end of file, so that trailing
		// continuation lines are listed.
		ཆེད་དུ (i = 0; ; ++i)
		{
		    གལ་ཏེ (vim_fgets(IObuff, IOSIZE, sfd))
			bha_nga;
		    // When a line has been truncated, append NL, taking care
		    // of multi-byte characters .
		    གལ་ཏེ (IObuff[IOSIZE - 2] != NUL && IObuff[IOSIZE - 2] != NL)
		    {
			ཧྲིལ n = IOSIZE - 2;

			གལ་ཏེ (enc_utf8)
			{
			    // Move to the first byte of this char.
			    // utf_head_off() doesn't work, because it checks
			    // for a truncated character.
			    སྐབས (n > 0 && (IObuff[n] & 0xc0) == 0x80)
				--n;
			}
			དེ་མིན གལ་ཏེ (has_mbyte)
			    n -= mb_head_off(IObuff, IObuff + n);
			IObuff[n] = NL;
			IObuff[n + 1] = NUL;
		    }
		    གལ་ཏེ (i < si->sn_prl_ga.ga_len
				     && (pp = &PRL_ITEM(si, i))->snp_count > 0)
		    {
			fprintf(fd, "%5d ", pp->snp_count);
			གལ་ཏེ (profile_equal(&pp->sn_prl_total, &pp->sn_prl_self))
			    fprintf(fd, "           ");
			དེ་མིན
			    fprintf(fd, "%s ", profile_msg(&pp->sn_prl_total));
			fprintf(fd, "%s ", profile_msg(&pp->sn_prl_self));
		    }
		    དེ་མིན
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
    void
profile_dump(void)
{
    FILE	*fd;

    གལ་ཏེ (profile_fname == NULL)
	ཕྱིར;

    fd = mch_fopen((var_na *)profile_fname, "w");
    གལ་ཏེ (fd == NULL)
	semsg(_(e_cant_open_file_str), profile_fname);
    དེ་མིན
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
    void
script_line_start(void)
{
    scriptitem_T    *si;
    sn_prl_T	    *pp;

    གལ་ཏེ (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	ཕྱིར;
    si = SCRIPT_ITEM(current_sctx.sc_sid);
    གལ་ཏེ (si->sn_prof_on && SOURCING_LNUM >= 1)
    {
	// Grow the array before starting the timer, so that the time spent
	// here isn't counted.
	(void)ga_grow(&si->sn_prl_ga,
				  (ཧྲིལ)(SOURCING_LNUM - si->sn_prl_ga.ga_len));
	si->sn_prl_idx = SOURCING_LNUM - 1;
	སྐབས (si->sn_prl_ga.ga_len <= si->sn_prl_idx
		&& si->sn_prl_ga.ga_len < si->sn_prl_ga.ga_maxlen)
	{
	    // Zero counters for a line that was not used before.
	    pp = &PRL_ITEM(si, si->sn_prl_ga.ga_len);
	    pp->snp_count = 0;
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
    void
script_line_exec(void)
{
    scriptitem_T    *si;

    གལ་ཏེ (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	ཕྱིར;
    si = SCRIPT_ITEM(current_sctx.sc_sid);
    གལ་ཏེ (si->sn_prof_on && si->sn_prl_idx >= 0)
	si->sn_prl_execed = TRUE;
}

/*
 * Called when done with a script line.
 */
    void
script_line_end(void)
{
    scriptitem_T    *si;
    sn_prl_T	    *pp;

    གལ་ཏེ (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	ཕྱིར;
    si = SCRIPT_ITEM(current_sctx.sc_sid);
    གལ་ཏེ (si->sn_prof_on && si->sn_prl_idx >= 0
				     && si->sn_prl_idx < si->sn_prl_ga.ga_len)
    {
	གལ་ཏེ (si->sn_prl_execed)
	{
	    pp = &PRL_ITEM(si, si->sn_prl_idx);
	    ++pp->snp_count;
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
