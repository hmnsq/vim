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

#ضمن "vim.h"

#إذا defined(FEAT_EVAL)
# إذا defined(FEAT_PROFILE) || defined(FEAT_RELTIME)
/*
 * Store the current time in "tm".
 */
    فراغ
profile_شروع(proftime_T *tm)
{
#  ifdef MSWIN
    QueryPerformanceCounter(tm);
#  وإلا
    PROF_GET_زمان(tm);
#  endif
}

/*
 * Compute the elapsed time from "tm" till now and store in "tm".
 */
    فراغ
profile_انتها(proftime_T *tm)
{
    proftime_T now;

#  ifdef MSWIN
    QueryPerformanceCounter(&now);
    tm->QuadPart = now.QuadPart - tm->QuadPart;
#  وإلا
    PROF_GET_زمان(&now);
    tm->tv_fsec = now.tv_fsec - tm->tv_fsec;
    tm->tv_sec = now.tv_sec - tm->tv_sec;
    إذا (tm->tv_fsec < 0)
    {
	tm->tv_fsec += TV_FSEC_SEC;
	--tm->tv_sec;
    }
#  endif
}

/*
 * Subtract the time "tm2" from "tm".
 */
    فراغ
profile_sub(proftime_T *tm, proftime_T *tm2)
{
#  ifdef MSWIN
    tm->QuadPart -= tm2->QuadPart;
#  وإلا
    tm->tv_fsec -= tm2->tv_fsec;
    tm->tv_sec -= tm2->tv_sec;
    إذا (tm->tv_fsec < 0)
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
    حرف *
profile_پیام(proftime_T *tm)
{
    ساكن حرف buf[50];

#  ifdef MSWIN
    LARGE_INTEGER   fr;

    QueryPerformanceFrequency(&fr);
    sprintf(buf, "%10.6lf", (مزدوج)tm->QuadPart / (مزدوج)fr.QuadPart);
#  وإلا
    sprintf(buf, PROF_زمان_FORMAT, (دراز)tm->tv_sec, (دراز)tm->tv_fsec);
#  endif
    أرجع buf;
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
    أرجع (float_T)tm->QuadPart / (float_T)fr.QuadPart;
#  وإلا
    أرجع (float_T)tm->tv_sec + (float_T)tm->tv_fsec / (float_T)TV_FSEC_SEC;
#  endif
}

/*
 * Put the time "msec" past now in "tm".
 */
    فراغ
profile_setlimit(دراز msec, proftime_T *tm)
{
    إذا (msec <= 0)   // no limit
	profile_zero(tm);
    وإلا
    {
#  ifdef MSWIN
	LARGE_INTEGER   fr;

	QueryPerformanceCounter(tm);
	QueryPerformanceFrequency(&fr);
	tm->QuadPart += (LONGLONG)((مزدوج)msec / 1000.0 * (مزدوج)fr.QuadPart);
#  وإلا
	varnumber_T	    fsec;	// this should be 64 bit if possible

	PROF_GET_زمان(tm);
	fsec = (varnumber_T)tm->tv_fsec
		       + (varnumber_T)msec * (varnumber_T)(TV_FSEC_SEC / 1000);
	tm->tv_fsec = fsec % (دراز)TV_FSEC_SEC;
	tm->tv_sec += fsec / (دراز)TV_FSEC_SEC;
#  endif
    }
}

/*
 * Return TRUE if the current time is past "tm".
 */
    صحيح
profile_passed_حد(proftime_T *tm)
{
    proftime_T	now;

#  ifdef MSWIN
    إذا (tm->QuadPart == 0)  // timer was not set
	أرجع FALSE;
    QueryPerformanceCounter(&now);
    أرجع (now.QuadPart > tm->QuadPart);
#  وإلا
    إذا (tm->tv_sec == 0)    // timer was not set
	أرجع FALSE;
    PROF_GET_زمان(&now);
    أرجع (now.tv_sec > tm->tv_sec
	    || (now.tv_sec == tm->tv_sec && now.tv_fsec > tm->tv_fsec));
#  endif
}

/*
 * Set the time in "tm" to zero.
 */
    فراغ
profile_zero(proftime_T *tm)
{
#  ifdef MSWIN
    tm->QuadPart = 0;
#  وإلا
    tm->tv_fsec = 0;
    tm->tv_sec = 0;
#  endif
}

# endif  // FEAT_PROFILE || FEAT_RELTIME

# إذا defined(FEAT_SYN_HL) && defined(FEAT_RELTIME) && defined(FEAT_PROFILE)
#  إذا defined(HAVE_MATH_H)
#   include <math.h>
#  endif

/*
 * Divide the time "tm" by "count" and store in "tm2".
 */
    فراغ
profile_divide(proftime_T *tm, صحيح شمار, proftime_T *tm2)
{
    إذا (شمار == 0)
	profile_zero(tm2);
    وإلا
    {
#  ifdef MSWIN
	tm2->QuadPart = tm->QuadPart / شمار;
#  وإلا
	مزدوج fsec = (tm->tv_sec * (float_T)TV_FSEC_SEC + tm->tv_fsec)
								       / شمار;

	tm2->tv_sec = floor(fsec / (float_T)TV_FSEC_SEC);
	tm2->tv_fsec = vim_دور(fsec - (tm2->tv_sec * (float_T)TV_FSEC_SEC));
#  endif
    }
}
# endif

# إذا defined(FEAT_PROFILE)
/*
 * Functions for profiling.
 */
ساكن proftime_T prof_wait_زمان;

/*
 * Add the time "tm2" to "tm".
 */
    فراغ
profile_جمع_کن(proftime_T *tm, proftime_T *tm2)
{
#  ifdef MSWIN
    tm->QuadPart += tm2->QuadPart;
#  وإلا
    tm->tv_fsec += tm2->tv_fsec;
    tm->tv_sec += tm2->tv_sec;
    إذا (tm->tv_fsec >= TV_FSEC_SEC)
    {
	tm->tv_fsec -= TV_FSEC_SEC;
	++tm->tv_sec;
    }
#  endif
}

/*
 * Add the "self" time from the total time and the children's time.
 */
    فراغ
profile_self(proftime_T *self, proftime_T *مجموع, proftime_T *children)
{
    // Check that the result won't be negative.  Can happen with recursive
    // calls.
#  ifdef MSWIN
    إذا (مجموع->QuadPart <= children->QuadPart)
	أرجع;
#  وإلا
    إذا (مجموع->tv_sec < children->tv_sec
	    || (مجموع->tv_sec == children->tv_sec
		&& مجموع->tv_fsec <= children->tv_fsec))
	أرجع;
#  endif
    profile_جمع_کن(self, مجموع);
    profile_sub(self, children);
}

/*
 * Get the current waittime.
 */
    ساكن فراغ
profile_get_wait(proftime_T *tm)
{
    *tm = prof_wait_زمان;
}

/*
 * Subtract the passed waittime since "tm" from "tma".
 */
    فراغ
profile_sub_wait(proftime_T *tm, proftime_T *tma)
{
    proftime_T tm3 = prof_wait_زمان;

    profile_sub(&tm3, tm);
    profile_sub(tma, &tm3);
}

/*
 * Return TRUE if "tm1" and "tm2" are equal.
 */
    ساكن صحيح
profile_equal(proftime_T *tm1, proftime_T *tm2)
{
#  ifdef MSWIN
    أرجع (tm1->QuadPart == tm2->QuadPart);
#  وإلا
    أرجع (tm1->tv_fsec == tm2->tv_fsec && tm1->tv_sec == tm2->tv_sec);
#  endif
}

/*
 * Return <0, 0 or >0 if "tm1" < "tm2", "tm1" == "tm2" or "tm1" > "tm2"
 */
    صحيح
profile_cmp(ثابت proftime_T *tm1, ثابت proftime_T *tm2)
{
#  ifdef MSWIN
    أرجع tm2->QuadPart == tm1->QuadPart ? 0 :
	tm2->QuadPart > tm1->QuadPart ? 1 : -1;
#  وإلا
    إذا (tm1->tv_sec == tm2->tv_sec)
	أرجع tm2->tv_fsec == tm1->tv_fsec ? 0 :
	    tm2->tv_fsec > tm1->tv_fsec ? 1 : -1;
    أرجع tm2->tv_sec > tm1->tv_sec ? 1 : -1;
#  endif
}

ساكن char_u	*profile_fname = عدم;
ساكن proftime_T pause_زمان;

/*
 * Reset all profiling information.
 */
    ساكن فراغ
profile_reset(فراغ)
{
    صحيح		id;
    صحيح		todo;
    hashtab_T	*functbl;
    hashitem_T	*hi;

    // Reset sourced files.
    لكل (id = 1; id <= script_اقلام.ga_len; id++)
    {
	scriptitem_T *si = SCRIPT_قلم(id);
	إذا (si->sn_prof_on)
	{
	    si->sn_prof_on      = FALSE;
	    si->sn_pr_force     = FALSE;
	    profile_zero(&si->sn_pr_فرزند);
	    si->sn_pr_nest      = 0;
	    si->sn_pr_شمار     = 0;
	    profile_zero(&si->sn_pr_مجموع);
	    profile_zero(&si->sn_pr_self);
	    profile_zero(&si->sn_pr_شروع);
	    profile_zero(&si->sn_pr_children);
	    ga_clear(&si->sn_prl_ga);
	    profile_zero(&si->sn_prl_شروع);
	    profile_zero(&si->sn_prl_children);
	    profile_zero(&si->sn_prl_wait);
	    si->sn_prl_idx      = -1;
	    si->sn_prl_execed   = 0;
	}
    }

    // Reset functions.
    functbl = func_tbl_get();
    todo = (صحيح)functbl->ht_استفاده_شده;

    FOR_ALL_HASHTAB_اقلام(functbl, hi, todo)
    {
	ufunc_T *fp;
	صحيح	i;

	إذا (HASHITEM_خالی(hi))
	    استمر;

	todo--;
	fp = HI2UF(hi);
	إذا (fp->uf_prof_initialized)
	{
	    fp->uf_profiling    = 0;
	    fp->uf_prof_initialized = FALSE;
	    fp->uf_tm_شمار     = 0;
	    profile_zero(&fp->uf_tm_مجموع);
	    profile_zero(&fp->uf_tm_self);
	    profile_zero(&fp->uf_tm_children);

	    لكل (i = 0; i < fp->uf_سطرها.ga_len; i++)
	    {
		fp->uf_tml_شمار[i] = 0;
		profile_zero(&fp->uf_tml_مجموع[i]);
		profile_zero(&fp->uf_tml_self[i]);
	    }

	    profile_zero(&fp->uf_tml_شروع);
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
    فراغ
ex_profile(exarg_T *eap)
{
    char_u	*e;
    صحيح		len;

    e = skiptowhite(eap->arg);
    len = (صحيح)(e - eap->arg);
    e = skipwhite(e);

    إذا (len == 5 && STRNCMP(eap->arg, "start", 5) == 0 && *e != NUL)
    {
	VIM_CLEAR(profile_fname);
	profile_fname = expand_env_save_opt(e, TRUE, عدم);
	do_profiling = PROF_YES;
	profile_zero(&prof_wait_زمان);
	مجموعه_vim_var_nr(VV_PROFILING, 1L);
    }
    وإلا إذا (do_profiling == PROF_NONE)
	emsg(_(e_اولی_use_profile_شروع_fname));
    وإلا إذا (STRCMP(eap->arg, "stop") == 0)
    {
	profile_dump();
	do_profiling = PROF_NONE;
	مجموعه_vim_var_nr(VV_PROFILING, 0L);
	profile_reset();
    }
    وإلا إذا (STRCMP(eap->arg, "pause") == 0)
    {
	إذا (do_profiling == PROF_YES)
	    profile_شروع(&pause_زمان);
	do_profiling = PROF_PAUSED;
    }
    وإلا إذا (STRCMP(eap->arg, "continue") == 0)
    {
	إذا (do_profiling == PROF_PAUSED)
	{
	    profile_انتها(&pause_زمان);
	    profile_جمع_کن(&prof_wait_زمان, &pause_زمان);
	}
	do_profiling = PROF_YES;
    }
    وإلا إذا (STRCMP(eap->arg, "dump") == 0)
	profile_dump();
    وإلا
    {
	// The rest is similar to ":breakadd".
	ex_breakadd(eap);
    }
}

// Command line expansion for :profile.
ساكن تعداد
{
    PEXP_SUBCMD,	// expand :profile sub-commands
    PEXP_FUNC		// expand :profile func {funcname}
} pexpand_what;

ساكن حرف *pexpand_cmds[] = {
			"start",
			"stop",
			"pause",
			"continue",
			"func",
			"file",
			"dump",
			عدم
};

/*
 * Function given to ExpandGeneric() to obtain the profile command
 * specific expansion.
 */
    char_u *
get_profile_name(expand_T *xp استفادهنشده, صحيح idx)
{
    بدل (pexpand_what)
    {
    حالة PEXP_SUBCMD:
	أرجع (char_u *)pexpand_cmds[idx];
    افتراضي:
	أرجع عدم;
    }
}

/*
 * Handle command line completion for :profile command.
 */
    فراغ
مجموعه_context_درون_profile_cmd(expand_T *xp, char_u *arg)
{
    char_u	*انتها_subcmd;

    // Default: expand subcommands.
    xp->xp_context = EXPAND_PROFILE;
    pexpand_what = PEXP_SUBCMD;
    xp->xp_الگو = arg;

    انتها_subcmd = skiptowhite(arg);
    إذا (*انتها_subcmd == NUL)
	أرجع;

    إذا ((انتها_subcmd - arg == 5 && STRNCMP(arg, "start", 5) == 0)
	    || (انتها_subcmd - arg == 4 && STRNCMP(arg, "file", 4) == 0))
    {
	xp->xp_context = EXPAND_FILES;
	xp->xp_الگو = skipwhite(انتها_subcmd);
	أرجع;
    }
    وإلا إذا (انتها_subcmd - arg == 4 && STRNCMP(arg, "func", 4) == 0)
    {
	xp->xp_context = EXPAND_USER_FUNC;
	xp->xp_الگو = skipwhite(انتها_subcmd);
	أرجع;
    }

    xp->xp_context = EXPAND_NOTHING;
}

ساكن proftime_T inchar_زمان;

/*
 * Called when starting to wait for the user to type a character.
 */
    فراغ
prof_inchar_enter(فراغ)
{
    profile_شروع(&inchar_زمان);
}

/*
 * Called when finished waiting for the user to type a character.
 */
    فراغ
prof_inchar_exit(فراغ)
{
    profile_انتها(&inchar_زمان);
    profile_جمع_کن(&prof_wait_زمان, &inchar_زمان);
}


/*
 * Return TRUE when a function defined in the current script should be
 * profiled.
 */
    صحيح
prof_def_func(فراغ)
{
    إذا (current_sctx.sc_sid > 0)
	أرجع SCRIPT_قلم(current_sctx.sc_sid)->sn_pr_force;
    أرجع FALSE;
}

/*
 * Print the count and times for one function or function line.
 */
    ساكن فراغ
prof_func_سطر(
    FILE	*fd,
    صحيح		شمار,
    proftime_T	*مجموع,
    proftime_T	*self,
    صحيح		prefer_self)	// when equal print only self time
{
    إذا (شمار > 0)
    {
	fprintf(fd, "%5d ", شمار);
	إذا (prefer_self && profile_equal(مجموع, self))
	    fprintf(fd, PROF_زمان_BLANK);
	وإلا
	    fprintf(fd, "%s ", profile_پیام(مجموع));
	إذا (!prefer_self && profile_equal(مجموع, self))
	    fprintf(fd, PROF_زمان_BLANK);
	وإلا
	    fprintf(fd, "%s ", profile_پیام(self));
    }
    وإلا
	fprintf(fd, "      %s%s", PROF_زمان_BLANK, PROF_زمان_BLANK);
}

    ساكن فراغ
prof_مرتب_سازی_list(
    FILE	*fd,
    ufunc_T	**sorttab,
    صحيح		st_len,
    حرف	*title,
    صحيح		prefer_self)	// when equal print only self time
{
    صحيح		i;
    ufunc_T	*fp;

    fprintf(fd, "FUNCTIONS SORTED ON %s TIME\n", title);
    fprintf(fd, "%s  function\n", PROF_TOTALS_سرآیند);
    لكل (i = 0; i < 20 && i < st_len; ++i)
    {
	fp = sorttab[i];
	prof_func_سطر(fd, fp->uf_tm_شمار, &fp->uf_tm_مجموع, &fp->uf_tm_self,
								 prefer_self);
	إذا (fp->uf_name[0] == K_SPECIAL)
	    fprintf(fd, " <SNR>%s()\n", fp->uf_name + 3);
	وإلا
	    fprintf(fd, " %s()\n", fp->uf_name);
    }
    fprintf(fd, "\n");
}

/*
 * Compare function for total time sorting.
 */
    ساكن صحيح
prof_مجموع_cmp(ثابت فراغ *s1, ثابت فراغ *s2)
{
    ufunc_T	*p1, *p2;

    p1 = *(ufunc_T **)s1;
    p2 = *(ufunc_T **)s2;
    أرجع profile_cmp(&p1->uf_tm_مجموع, &p2->uf_tm_مجموع);
}

/*
 * Compare function for self time sorting.
 */
    ساكن صحيح
prof_self_cmp(ثابت فراغ *s1, ثابت فراغ *s2)
{
    ufunc_T	*p1, *p2;

    p1 = *(ufunc_T **)s1;
    p2 = *(ufunc_T **)s2;
    أرجع profile_cmp(&p1->uf_tm_self, &p2->uf_tm_self);
}

/*
 * Start profiling function "fp".
 */
    فراغ
func_do_profile(ufunc_T *fp)
{
    صحيح		len = fp->uf_سطرها.ga_len;

    إذا (!fp->uf_prof_initialized)
    {
	إذا (len == 0)
	    len = 1;  // avoid getting error for allocating zero bytes
	fp->uf_tm_شمار = 0;
	profile_zero(&fp->uf_tm_self);
	profile_zero(&fp->uf_tm_مجموع);
	إذا (fp->uf_tml_شمار == عدم)
	    fp->uf_tml_شمار = ALLOC_CLEAR_MULT(صحيح, len);
	إذا (fp->uf_tml_مجموع == عدم)
	    fp->uf_tml_مجموع = ALLOC_CLEAR_MULT(proftime_T, len);
	إذا (fp->uf_tml_self == عدم)
	    fp->uf_tml_self = ALLOC_CLEAR_MULT(proftime_T, len);
	fp->uf_tml_idx = -1;
	إذا (fp->uf_tml_شمار == عدم || fp->uf_tml_مجموع == عدم
						    || fp->uf_tml_self == عدم)
	    أرجع;	    // out of memory
	fp->uf_prof_initialized = TRUE;
    }

    fp->uf_profiling = TRUE;
}

/*
 * Save time when starting to invoke another script or function.
 */
    ساكن فراغ
script_prof_save(
    proftime_T	*tm)	    // place to store wait time
{
    scriptitem_T    *si;

    إذا (SCRIPT_ID_VALID(current_sctx.sc_sid))
    {
	si = SCRIPT_قلم(current_sctx.sc_sid);
	إذا (si->sn_prof_on && si->sn_pr_nest++ == 0)
	    profile_شروع(&si->sn_pr_فرزند);
    }
    profile_get_wait(tm);
}

/*
 * When calling a function: may initialize for profiling.
 */
    فراغ
profile_may_شروع_func(profinfo_T *info, ufunc_T *fp, ufunc_T *caller)
{
    إذا (!fp->uf_profiling && has_profiling(FALSE, fp->uf_name, عدم,
								&fp->uf_hash))
    {
	info->pi_started_profiling = TRUE;
	func_do_profile(fp);
    }
    إذا (fp->uf_profiling || (caller != عدم && caller->uf_profiling))
    {
	++fp->uf_tm_شمار;
	profile_شروع(&info->pi_call_شروع);
	profile_zero(&fp->uf_tm_children);
    }
    script_prof_save(&info->pi_wait_شروع);
}

/*
 * After calling a function: may handle profiling.  profile_may_start_func()
 * must have been called previously.
 */
    فراغ
profile_may_انتها_func(profinfo_T *info, ufunc_T *fp, ufunc_T *caller)
{
    profile_انتها(&info->pi_call_شروع);
    profile_sub_wait(&info->pi_wait_شروع, &info->pi_call_شروع);
    profile_جمع_کن(&fp->uf_tm_مجموع, &info->pi_call_شروع);
    profile_self(&fp->uf_tm_self, &info->pi_call_شروع, &fp->uf_tm_children);
    إذا (caller != عدم && caller->uf_profiling)
    {
	profile_جمع_کن(&caller->uf_tm_children, &info->pi_call_شروع);
	profile_جمع_کن(&caller->uf_tml_children, &info->pi_call_شروع);
    }
    إذا (info->pi_started_profiling)
	// make a ":profdel func" stop profiling the function
	fp->uf_profiling = FALSE;
}

/*
 * Prepare profiling for entering a child or something else that is not
 * counted for the script/function itself.
 * Should always be called in pair with prof_child_exit().
 */
    فراغ
prof_فرزند_enter(
    proftime_T *tm)	// place to store waittime
{
    funccall_T *fc = get_current_funccal();

    إذا (fc != عدم && fc->fc_func->uf_profiling)
	profile_شروع(&fc->fc_prof_فرزند);
    script_prof_save(tm);
}

/*
 * Take care of time spent in a child.
 * Should always be called after prof_child_enter().
 */
    فراغ
prof_فرزند_exit(
    proftime_T *tm)	// where waittime was stored
{
    funccall_T *fc = get_current_funccal();

    إذا (fc != عدم && fc->fc_func->uf_profiling)
    {
	profile_انتها(&fc->fc_prof_فرزند);
	profile_sub_wait(tm, &fc->fc_prof_فرزند); // don't count waiting time
	profile_جمع_کن(&fc->fc_func->uf_tm_children, &fc->fc_prof_فرزند);
	profile_جمع_کن(&fc->fc_func->uf_tml_children, &fc->fc_prof_فرزند);
    }
    script_prof_restore(tm);
}

/*
 * Called when starting to read a function line.
 * "sourcing_lnum" must be correct!
 * When skipping lines it may not actually be executed, but we won't find out
 * until later and we need to store the time now.
 */
    فراغ
func_سطر_شروع(فراغ *cookie, دراز lnum)
{
    funccall_T	*fcp = (funccall_T *)cookie;
    ufunc_T	*fp = fcp->fc_func;

    إذا (fp->uf_profiling && lnum >= 1 && lnum <= fp->uf_سطرها.ga_len)
    {
	fp->uf_tml_idx = lnum - 1;
	// Skip continuation lines.
	طالما (fp->uf_tml_idx > 0 && FUNCLINE(fp, fp->uf_tml_idx) == عدم)
	    --fp->uf_tml_idx;
	fp->uf_tml_execed = FALSE;
	profile_شروع(&fp->uf_tml_شروع);
	profile_zero(&fp->uf_tml_children);
	profile_get_wait(&fp->uf_tml_wait);
    }
}

/*
 * Called when actually executing a function line.
 */
    فراغ
func_سطر_exec(فراغ *cookie)
{
    funccall_T	*fcp = (funccall_T *)cookie;
    ufunc_T	*fp = fcp->fc_func;

    إذا (fp->uf_profiling && fp->uf_tml_idx >= 0)
	fp->uf_tml_execed = TRUE;
}

/*
 * Called when done with a function line.
 */
    فراغ
func_سطر_انتها(فراغ *cookie)
{
    funccall_T	*fcp = (funccall_T *)cookie;
    ufunc_T	*fp = fcp->fc_func;

    إذا (fp->uf_profiling && fp->uf_tml_idx >= 0)
    {
	إذا (fp->uf_tml_execed)
	{
	    ++fp->uf_tml_شمار[fp->uf_tml_idx];
	    profile_انتها(&fp->uf_tml_شروع);
	    profile_sub_wait(&fp->uf_tml_wait, &fp->uf_tml_شروع);
	    profile_جمع_کن(&fp->uf_tml_مجموع[fp->uf_tml_idx], &fp->uf_tml_شروع);
	    profile_self(&fp->uf_tml_self[fp->uf_tml_idx], &fp->uf_tml_شروع,
							&fp->uf_tml_children);
	}
	fp->uf_tml_idx = -1;
    }
}

/*
 * Dump the profiling results for all functions in file "fd".
 */
    ساكن فراغ
func_dump_profile(FILE *fd)
{
    hashtab_T	*functbl;
    hashitem_T	*hi;
    صحيح		todo;
    ufunc_T	*fp;
    صحيح		i;
    ufunc_T	**sorttab;
    صحيح		st_len = 0;
    char_u	*p;

    functbl = func_tbl_get();
    todo = (صحيح)functbl->ht_استفاده_شده;
    إذا (todo == 0)
	أرجع;     // nothing to dump

    sorttab = ALLOC_MULT(ufunc_T *, todo);

    FOR_ALL_HASHTAB_اقلام(functbl, hi, todo)
    {
	إذا (!HASHITEM_خالی(hi))
	{
	    --todo;
	    fp = HI2UF(hi);
	    إذا (fp->uf_prof_initialized)
	    {
		إذا (sorttab != عدم)
		    sorttab[st_len++] = fp;

		إذا (fp->uf_name[0] == K_SPECIAL)
		    fprintf(fd, "FUNCTION  <SNR>%s()\n", fp->uf_name + 3);
		وإلا
		    fprintf(fd, "FUNCTION  %s()\n", fp->uf_name);
		إذا (fp->uf_script_ctx.sc_sid > 0)
		{
		    p = home_replace_save(عدم,
				     get_scriptname(fp->uf_script_ctx.sc_sid));
		    إذا (p != عدم)
		    {
			fprintf(fd, "    Defined: %s:%ld\n",
					   p, (دراز)fp->uf_script_ctx.sc_lnum);
			vim_free(p);
		    }
		}
		إذا (fp->uf_tm_شمار == 1)
		    fprintf(fd, "Called 1 time\n");
		وإلا
		    fprintf(fd, "Called %d times\n", fp->uf_tm_شمار);
		fprintf(fd, "Total time: %s\n", profile_پیام(&fp->uf_tm_مجموع));
		fprintf(fd, " Self time: %s\n", profile_پیام(&fp->uf_tm_self));
		fprintf(fd, "\n");
		fprintf(fd, "%s\n", PROF_TOTALS_سرآیند);

		لكل (i = 0; i < fp->uf_سطرها.ga_len; ++i)
		{
		    إذا (FUNCLINE(fp, i) == عدم)
			استمر;
		    prof_func_سطر(fd, fp->uf_tml_شمار[i],
			     &fp->uf_tml_مجموع[i], &fp->uf_tml_self[i], TRUE);
		    fprintf(fd, "%s\n", FUNCLINE(fp, i));
		}
		fprintf(fd, "\n");
	    }
	}
    }

    إذا (sorttab != عدم && st_len > 0)
    {
	qsort((فراغ *)sorttab, (size_t)st_len, حجم(ufunc_T *),
							      prof_مجموع_cmp);
	prof_مرتب_سازی_list(fd, sorttab, st_len, "TOTAL", FALSE);
	qsort((فراغ *)sorttab, (size_t)st_len, حجم(ufunc_T *),
							      prof_self_cmp);
	prof_مرتب_سازی_list(fd, sorttab, st_len, "SELF", TRUE);
    }

    vim_free(sorttab);
}

/*
 * Start profiling script "fp".
 */
    فراغ
script_do_profile(scriptitem_T *si)
{
    si->sn_pr_شمار = 0;
    profile_zero(&si->sn_pr_مجموع);
    profile_zero(&si->sn_pr_self);

    ga_init2(&si->sn_prl_ga, حجم(sn_prl_T), 100);
    si->sn_prl_idx = -1;
    si->sn_prof_on = TRUE;
    si->sn_pr_nest = 0;
}

/*
 * Count time spent in children after invoking another script or function.
 */
    فراغ
script_prof_restore(proftime_T *tm)
{
    scriptitem_T    *si;

    إذا (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	أرجع;

    si = SCRIPT_قلم(current_sctx.sc_sid);
    إذا (si->sn_prof_on && --si->sn_pr_nest == 0)
    {
	profile_انتها(&si->sn_pr_فرزند);
	profile_sub_wait(tm, &si->sn_pr_فرزند); // don't count wait time
	profile_جمع_کن(&si->sn_pr_children, &si->sn_pr_فرزند);
	profile_جمع_کن(&si->sn_prl_children, &si->sn_pr_فرزند);
    }
}

/*
 * Dump the profiling results for all scripts in file "fd".
 */
    ساكن فراغ
script_dump_profile(FILE *fd)
{
    صحيح		    id;
    scriptitem_T    *si;
    صحيح		    i;
    FILE	    *sfd;
    sn_prl_T	    *pp;

    لكل (id = 1; id <= script_اقلام.ga_len; ++id)
    {
	si = SCRIPT_قلم(id);
	إذا (si->sn_prof_on)
	{
	    fprintf(fd, "SCRIPT  %s\n", si->sn_name);
	    إذا (si->sn_pr_شمار == 1)
		fprintf(fd, "Sourced 1 time\n");
	    وإلا
		fprintf(fd, "Sourced %d times\n", si->sn_pr_شمار);
	    fprintf(fd, "Total time: %s\n", profile_پیام(&si->sn_pr_مجموع));
	    fprintf(fd, " Self time: %s\n", profile_پیام(&si->sn_pr_self));
	    fprintf(fd, "\n");
	    fprintf(fd, "%s\n", PROF_TOTALS_سرآیند);

	    sfd = mch_fopen((حرف *)si->sn_name, "r");
	    إذا (sfd == عدم)
		fprintf(fd, "Cannot open file!\n");
	    وإلا
	    {
		// Keep going till the end of file, so that trailing
		// continuation lines are listed.
		لكل (i = 0; ; ++i)
		{
		    إذا (vim_fgets(IObuff, IOSIZE, sfd))
			اكسر;
		    // When a line has been truncated, append NL, taking care
		    // of multi-byte characters .
		    إذا (IObuff[IOSIZE - 2] != NUL && IObuff[IOSIZE - 2] != NL)
		    {
			صحيح n = IOSIZE - 2;

			إذا (enc_utf8)
			{
			    // Move to the first byte of this char.
			    // utf_head_off() doesn't work, because it checks
			    // for a truncated character.
			    طالما (n > 0 && (IObuff[n] & 0xc0) == 0x80)
				--n;
			}
			وإلا إذا (has_mbyte)
			    n -= mb_سر_off(IObuff, IObuff + n);
			IObuff[n] = NL;
			IObuff[n + 1] = NUL;
		    }
		    إذا (i < si->sn_prl_ga.ga_len
				     && (pp = &PRL_قلم(si, i))->snp_شمار > 0)
		    {
			fprintf(fd, "%5d ", pp->snp_شمار);
			إذا (profile_equal(&pp->sn_prl_مجموع, &pp->sn_prl_self))
			    fprintf(fd, "           ");
			وإلا
			    fprintf(fd, "%s ", profile_پیام(&pp->sn_prl_مجموع));
			fprintf(fd, "%s ", profile_پیام(&pp->sn_prl_self));
		    }
		    وإلا
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
    فراغ
profile_dump(فراغ)
{
    FILE	*fd;

    إذا (profile_fname == عدم)
	أرجع;

    fd = mch_fopen((حرف *)profile_fname, "w");
    إذا (fd == عدم)
	semsg(_(e_cant_open_file_str), profile_fname);
    وإلا
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
    فراغ
script_سطر_شروع(فراغ)
{
    scriptitem_T    *si;
    sn_prl_T	    *pp;

    إذا (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	أرجع;
    si = SCRIPT_قلم(current_sctx.sc_sid);
    إذا (si->sn_prof_on && SOURCING_LNUM >= 1)
    {
	// Grow the array before starting the timer, so that the time spent
	// here isn't counted.
	(فراغ)ga_grow(&si->sn_prl_ga,
				  (صحيح)(SOURCING_LNUM - si->sn_prl_ga.ga_len));
	si->sn_prl_idx = SOURCING_LNUM - 1;
	طالما (si->sn_prl_ga.ga_len <= si->sn_prl_idx
		&& si->sn_prl_ga.ga_len < si->sn_prl_ga.ga_maxlen)
	{
	    // Zero counters for a line that was not used before.
	    pp = &PRL_قلم(si, si->sn_prl_ga.ga_len);
	    pp->snp_شمار = 0;
	    profile_zero(&pp->sn_prl_مجموع);
	    profile_zero(&pp->sn_prl_self);
	    ++si->sn_prl_ga.ga_len;
	}
	si->sn_prl_execed = FALSE;
	profile_شروع(&si->sn_prl_شروع);
	profile_zero(&si->sn_prl_children);
	profile_get_wait(&si->sn_prl_wait);
    }
}

/*
 * Called when actually executing a function line.
 */
    فراغ
script_سطر_exec(فراغ)
{
    scriptitem_T    *si;

    إذا (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	أرجع;
    si = SCRIPT_قلم(current_sctx.sc_sid);
    إذا (si->sn_prof_on && si->sn_prl_idx >= 0)
	si->sn_prl_execed = TRUE;
}

/*
 * Called when done with a script line.
 */
    فراغ
script_سطر_انتها(فراغ)
{
    scriptitem_T    *si;
    sn_prl_T	    *pp;

    إذا (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	أرجع;
    si = SCRIPT_قلم(current_sctx.sc_sid);
    إذا (si->sn_prof_on && si->sn_prl_idx >= 0
				     && si->sn_prl_idx < si->sn_prl_ga.ga_len)
    {
	إذا (si->sn_prl_execed)
	{
	    pp = &PRL_قلم(si, si->sn_prl_idx);
	    ++pp->snp_شمار;
	    profile_انتها(&si->sn_prl_شروع);
	    profile_sub_wait(&si->sn_prl_wait, &si->sn_prl_شروع);
	    profile_جمع_کن(&pp->sn_prl_مجموع, &si->sn_prl_شروع);
	    profile_self(&pp->sn_prl_self, &si->sn_prl_شروع,
							&si->sn_prl_children);
	}
	si->sn_prl_idx = -1;
    }
}
# endif // FEAT_PROFILE

#پایان_اگر
