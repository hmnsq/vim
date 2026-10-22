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

#شامل "vim.h"

#اگر defined(FEAT_EVAL)
# اگر defined(FEAT_PROFILE) || defined(FEAT_RELTIME)
/*
 * Store the current time in "tm".
 */
    خالی
profile_آغاز(proftime_T *tm)
{
#  ifdef MSWIN
    Query_Performance_شمار(tm);
#  ورنہ
    PROF_GET_وقت(tm);
#  endif
}

/*
 * Compute the elapsed time from "tm" till now and store in "tm".
 */
    خالی
profile_انتہا(proftime_T *tm)
{
    proftime_T now;

#  ifdef MSWIN
    Query_Performance_شمار(&now);
    tm->QuadPart = now.QuadPart - tm->QuadPart;
#  ورنہ
    PROF_GET_وقت(&now);
    tm->tv_fsec = now.tv_fsec - tm->tv_fsec;
    tm->tv_sec = now.tv_sec - tm->tv_sec;
    اگر (tm->tv_fsec < 0)
    {
	tm->tv_fsec += TV_FSEC_SEC;
	--tm->tv_sec;
    }
#  endif
}

/*
 * Subtract the time "tm2" from "tm".
 */
    خالی
profile_sub(proftime_T *tm, proftime_T *tm2)
{
#  ifdef MSWIN
    tm->QuadPart -= tm2->QuadPart;
#  ورنہ
    tm->tv_fsec -= tm2->tv_fsec;
    tm->tv_sec -= tm2->tv_sec;
    اگر (tm->tv_fsec < 0)
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
profile_پیغام(proftime_T *tm)
{
    ساکن حرف buf[50];

#  ifdef MSWIN
    LARGE_INTEGER   fr;

    QueryPerformanceFrequency(&fr);
    sprintf(buf, "%10.6lf", (دوہرا)tm->QuadPart / (دوہرا)fr.QuadPart);
#  ورنہ
    sprintf(buf, PROF_وقت_FORMAT, (طویل)tm->tv_sec, (طویل)tm->tv_fsec);
#  endif
    واپس buf;
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
    واپس (float_T)tm->QuadPart / (float_T)fr.QuadPart;
#  ورنہ
    واپس (float_T)tm->tv_sec + (float_T)tm->tv_fsec / (float_T)TV_FSEC_SEC;
#  endif
}

/*
 * Put the time "msec" past now in "tm".
 */
    خالی
profile_setlimit(طویل msec, proftime_T *tm)
{
    اگر (msec <= 0)   // no limit
	profile_zero(tm);
    ورنہ
    {
#  ifdef MSWIN
	LARGE_INTEGER   fr;

	Query_Performance_شمار(tm);
	QueryPerformanceFrequency(&fr);
	tm->QuadPart += (LONGLONG)((دوہرا)msec / 1000.0 * (دوہرا)fr.QuadPart);
#  ورنہ
	varnumber_T	    fsec;	// this should be 64 bit if possible

	PROF_GET_وقت(tm);
	fsec = (varnumber_T)tm->tv_fsec
		       + (varnumber_T)msec * (varnumber_T)(TV_FSEC_SEC / 1000);
	tm->tv_fsec = fsec % (طویل)TV_FSEC_SEC;
	tm->tv_sec += fsec / (طویل)TV_FSEC_SEC;
#  endif
    }
}

/*
 * Return TRUE if the current time is past "tm".
 */
    صحیح
profile_passed_حد_بندی(proftime_T *tm)
{
    proftime_T	now;

#  ifdef MSWIN
    اگر (tm->QuadPart == 0)  // timer was not set
	واپس FALSE;
    Query_Performance_شمار(&now);
    واپس (now.QuadPart > tm->QuadPart);
#  ورنہ
    اگر (tm->tv_sec == 0)    // timer was not set
	واپس FALSE;
    PROF_GET_وقت(&now);
    واپس (now.tv_sec > tm->tv_sec
	    || (now.tv_sec == tm->tv_sec && now.tv_fsec > tm->tv_fsec));
#  endif
}

/*
 * Set the time in "tm" to zero.
 */
    خالی
profile_zero(proftime_T *tm)
{
#  ifdef MSWIN
    tm->QuadPart = 0;
#  ورنہ
    tm->tv_fsec = 0;
    tm->tv_sec = 0;
#  endif
}

# endif  // FEAT_PROFILE || FEAT_RELTIME

# اگر defined(FEAT_SYN_HL) && defined(FEAT_RELTIME) && defined(FEAT_PROFILE)
#  اگر defined(HAVE_MATH_H)
#   include <math.h>
#  endif

/*
 * Divide the time "tm" by "count" and store in "tm2".
 */
    خالی
profile_divide(proftime_T *tm, صحیح گنتی, proftime_T *tm2)
{
    اگر (گنتی == 0)
	profile_zero(tm2);
    ورنہ
    {
#  ifdef MSWIN
	tm2->QuadPart = tm->QuadPart / گنتی;
#  ورنہ
	دوہرا fsec = (tm->tv_sec * (float_T)TV_FSEC_SEC + tm->tv_fsec)
								       / گنتی;

	tm2->tv_sec = floor(fsec / (float_T)TV_FSEC_SEC);
	tm2->tv_fsec = vim_باری(fsec - (tm2->tv_sec * (float_T)TV_FSEC_SEC));
#  endif
    }
}
# endif

# اگر defined(FEAT_PROFILE)
/*
 * Functions for profiling.
 */
ساکن proftime_T prof_wait_وقت;

/*
 * Add the time "tm2" to "tm".
 */
    خالی
profile_جوڑو(proftime_T *tm, proftime_T *tm2)
{
#  ifdef MSWIN
    tm->QuadPart += tm2->QuadPart;
#  ورنہ
    tm->tv_fsec += tm2->tv_fsec;
    tm->tv_sec += tm2->tv_sec;
    اگر (tm->tv_fsec >= TV_FSEC_SEC)
    {
	tm->tv_fsec -= TV_FSEC_SEC;
	++tm->tv_sec;
    }
#  endif
}

/*
 * Add the "self" time from the total time and the children's time.
 */
    خالی
profile_self(proftime_T *self, proftime_T *کل, proftime_T *children)
{
    // Check that the result won't be negative.  Can happen with recursive
    // calls.
#  ifdef MSWIN
    اگر (کل->QuadPart <= children->QuadPart)
	واپس;
#  ورنہ
    اگر (کل->tv_sec < children->tv_sec
	    || (کل->tv_sec == children->tv_sec
		&& کل->tv_fsec <= children->tv_fsec))
	واپس;
#  endif
    profile_جوڑو(self, کل);
    profile_sub(self, children);
}

/*
 * Get the current waittime.
 */
    ساکن خالی
profile_get_wait(proftime_T *tm)
{
    *tm = prof_wait_وقت;
}

/*
 * Subtract the passed waittime since "tm" from "tma".
 */
    خالی
profile_sub_wait(proftime_T *tm, proftime_T *tma)
{
    proftime_T tm3 = prof_wait_وقت;

    profile_sub(&tm3, tm);
    profile_sub(tma, &tm3);
}

/*
 * Return TRUE if "tm1" and "tm2" are equal.
 */
    ساکن صحیح
profile_equal(proftime_T *tm1, proftime_T *tm2)
{
#  ifdef MSWIN
    واپس (tm1->QuadPart == tm2->QuadPart);
#  ورنہ
    واپس (tm1->tv_fsec == tm2->tv_fsec && tm1->tv_sec == tm2->tv_sec);
#  endif
}

/*
 * Return <0, 0 or >0 if "tm1" < "tm2", "tm1" == "tm2" or "tm1" > "tm2"
 */
    صحیح
profile_cmp(ثابت proftime_T *tm1, ثابت proftime_T *tm2)
{
#  ifdef MSWIN
    واپس tm2->QuadPart == tm1->QuadPart ? 0 :
	tm2->QuadPart > tm1->QuadPart ? 1 : -1;
#  ورنہ
    اگر (tm1->tv_sec == tm2->tv_sec)
	واپس tm2->tv_fsec == tm1->tv_fsec ? 0 :
	    tm2->tv_fsec > tm1->tv_fsec ? 1 : -1;
    واپس tm2->tv_sec > tm1->tv_sec ? 1 : -1;
#  endif
}

ساکن char_u	*profile_fname = تہی;
ساکن proftime_T pause_وقت;

/*
 * Reset all profiling information.
 */
    ساکن خالی
profile_reset(خالی)
{
    صحیح		id;
    صحیح		todo;
    hashtab_T	*functbl;
    hashitem_T	*hi;

    // Reset sourced files.
    برائے (id = 1; id <= script_اشیاء.ga_len; id++)
    {
	scriptitem_T *si = SCRIPT_شے(id);
	اگر (si->sn_prof_on)
	{
	    si->sn_prof_on      = FALSE;
	    si->sn_pr_force     = FALSE;
	    profile_zero(&si->sn_pr_بچہ);
	    si->sn_pr_nest      = 0;
	    si->sn_pr_گنتی     = 0;
	    profile_zero(&si->sn_pr_کل);
	    profile_zero(&si->sn_pr_self);
	    profile_zero(&si->sn_pr_آغاز);
	    profile_zero(&si->sn_pr_children);
	    ga_clear(&si->sn_prl_ga);
	    profile_zero(&si->sn_prl_آغاز);
	    profile_zero(&si->sn_prl_children);
	    profile_zero(&si->sn_prl_wait);
	    si->sn_prl_idx      = -1;
	    si->sn_prl_execed   = 0;
	}
    }

    // Reset functions.
    functbl = func_tbl_get();
    todo = (صحیح)functbl->ht_مستعمل;

    FOR_ALL_HASHTAB_اشیاء(functbl, hi, todo)
    {
	ufunc_T *fp;
	صحیح	i;

	اگر (HASHITEM_خالی_خانے(hi))
	    جاری;

	todo--;
	fp = HI2UF(hi);
	اگر (fp->uf_prof_initialized)
	{
	    fp->uf_profiling    = 0;
	    fp->uf_prof_initialized = FALSE;
	    fp->uf_tm_گنتی     = 0;
	    profile_zero(&fp->uf_tm_کل);
	    profile_zero(&fp->uf_tm_self);
	    profile_zero(&fp->uf_tm_children);

	    برائے (i = 0; i < fp->uf_سطریں.ga_len; i++)
	    {
		fp->uf_tml_گنتی[i] = 0;
		profile_zero(&fp->uf_tml_کل[i]);
		profile_zero(&fp->uf_tml_self[i]);
	    }

	    profile_zero(&fp->uf_tml_آغاز);
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
    خالی
ex_profile(exarg_T *eap)
{
    char_u	*e;
    صحیح		len;

    e = skiptowhite(eap->arg);
    len = (صحیح)(e - eap->arg);
    e = skipwhite(e);

    اگر (len == 5 && STRNCMP(eap->arg, "start", 5) == 0 && *e != NUL)
    {
	VIM_CLEAR(profile_fname);
	profile_fname = expand_env_save_opt(e, TRUE, تہی);
	do_profiling = PROF_YES;
	profile_zero(&prof_wait_وقت);
	سیٹ_vim_var_nr(VV_PROFILING, 1L);
    }
    ورنہ اگر (do_profiling == PROF_NONE)
	emsg(_(e_پہلا_use_profile_آغاز_fname));
    ورنہ اگر (STRCMP(eap->arg, "stop") == 0)
    {
	profile_dump();
	do_profiling = PROF_NONE;
	سیٹ_vim_var_nr(VV_PROFILING, 0L);
	profile_reset();
    }
    ورنہ اگر (STRCMP(eap->arg, "pause") == 0)
    {
	اگر (do_profiling == PROF_YES)
	    profile_آغاز(&pause_وقت);
	do_profiling = PROF_PAUSED;
    }
    ورنہ اگر (STRCMP(eap->arg, "continue") == 0)
    {
	اگر (do_profiling == PROF_PAUSED)
	{
	    profile_انتہا(&pause_وقت);
	    profile_جوڑو(&prof_wait_وقت, &pause_وقت);
	}
	do_profiling = PROF_YES;
    }
    ورنہ اگر (STRCMP(eap->arg, "dump") == 0)
	profile_dump();
    ورنہ
    {
	// The rest is similar to ":breakadd".
	ex_breakadd(eap);
    }
}

// Command line expansion for :profile.
ساکن شمار
{
    PEXP_SUBCMD,	// expand :profile sub-commands
    PEXP_FUNC		// expand :profile func {funcname}
} pexpand_what;

ساکن حرف *pexpand_cmds[] = {
			"start",
			"stop",
			"pause",
			"continue",
			"func",
			"file",
			"dump",
			تہی
};

/*
 * Function given to ExpandGeneric() to obtain the profile command
 * specific expansion.
 */
    char_u *
get_profile_name(expand_T *xp غیر_مستعمل, صحیح idx)
{
    انتخاب (pexpand_what)
    {
    حالت PEXP_SUBCMD:
	واپس (char_u *)pexpand_cmds[idx];
    طےشدہ:
	واپس تہی;
    }
}

/*
 * Handle command line completion for :profile command.
 */
    خالی
سیٹ_context_اندر_profile_cmd(expand_T *xp, char_u *arg)
{
    char_u	*انتہا_subcmd;

    // Default: expand subcommands.
    xp->xp_context = EXPAND_PROFILE;
    pexpand_what = PEXP_SUBCMD;
    xp->xp_نمونہ = arg;

    انتہا_subcmd = skiptowhite(arg);
    اگر (*انتہا_subcmd == NUL)
	واپس;

    اگر ((انتہا_subcmd - arg == 5 && STRNCMP(arg, "start", 5) == 0)
	    || (انتہا_subcmd - arg == 4 && STRNCMP(arg, "file", 4) == 0))
    {
	xp->xp_context = EXPAND_FILES;
	xp->xp_نمونہ = skipwhite(انتہا_subcmd);
	واپس;
    }
    ورنہ اگر (انتہا_subcmd - arg == 4 && STRNCMP(arg, "func", 4) == 0)
    {
	xp->xp_context = EXPAND_USER_FUNC;
	xp->xp_نمونہ = skipwhite(انتہا_subcmd);
	واپس;
    }

    xp->xp_context = EXPAND_NOTHING;
}

ساکن proftime_T inchar_وقت;

/*
 * Called when starting to wait for the user to type a character.
 */
    خالی
prof_inchar_enter(خالی)
{
    profile_آغاز(&inchar_وقت);
}

/*
 * Called when finished waiting for the user to type a character.
 */
    خالی
prof_inchar_exit(خالی)
{
    profile_انتہا(&inchar_وقت);
    profile_جوڑو(&prof_wait_وقت, &inchar_وقت);
}


/*
 * Return TRUE when a function defined in the current script should be
 * profiled.
 */
    صحیح
prof_def_func(خالی)
{
    اگر (current_sctx.sc_sid > 0)
	واپس SCRIPT_شے(current_sctx.sc_sid)->sn_pr_force;
    واپس FALSE;
}

/*
 * Print the count and times for one function or function line.
 */
    ساکن خالی
prof_func_سطر(
    FILE	*fd,
    صحیح		گنتی,
    proftime_T	*کل,
    proftime_T	*self,
    صحیح		prefer_self)	// when equal print only self time
{
    اگر (گنتی > 0)
    {
	fprintf(fd, "%5d ", گنتی);
	اگر (prefer_self && profile_equal(کل, self))
	    fprintf(fd, PROF_وقت_BLANK);
	ورنہ
	    fprintf(fd, "%s ", profile_پیغام(کل));
	اگر (!prefer_self && profile_equal(کل, self))
	    fprintf(fd, PROF_وقت_BLANK);
	ورنہ
	    fprintf(fd, "%s ", profile_پیغام(self));
    }
    ورنہ
	fprintf(fd, "      %s%s", PROF_وقت_BLANK, PROF_وقت_BLANK);
}

    ساکن خالی
prof_ترتیب_دو_list(
    FILE	*fd,
    ufunc_T	**sorttab,
    صحیح		st_len,
    حرف	*title,
    صحیح		prefer_self)	// when equal print only self time
{
    صحیح		i;
    ufunc_T	*fp;

    fprintf(fd, "FUNCTIONS SORTED ON %s TIME\n", title);
    fprintf(fd, "%s  function\n", PROF_TOTALS_سرخی);
    برائے (i = 0; i < 20 && i < st_len; ++i)
    {
	fp = sorttab[i];
	prof_func_سطر(fd, fp->uf_tm_گنتی, &fp->uf_tm_کل, &fp->uf_tm_self,
								 prefer_self);
	اگر (fp->uf_name[0] == K_SPECIAL)
	    fprintf(fd, " <SNR>%s()\n", fp->uf_name + 3);
	ورنہ
	    fprintf(fd, " %s()\n", fp->uf_name);
    }
    fprintf(fd, "\n");
}

/*
 * Compare function for total time sorting.
 */
    ساکن صحیح
prof_کل_cmp(ثابت خالی *s1, ثابت خالی *s2)
{
    ufunc_T	*p1, *p2;

    p1 = *(ufunc_T **)s1;
    p2 = *(ufunc_T **)s2;
    واپس profile_cmp(&p1->uf_tm_کل, &p2->uf_tm_کل);
}

/*
 * Compare function for self time sorting.
 */
    ساکن صحیح
prof_self_cmp(ثابت خالی *s1, ثابت خالی *s2)
{
    ufunc_T	*p1, *p2;

    p1 = *(ufunc_T **)s1;
    p2 = *(ufunc_T **)s2;
    واپس profile_cmp(&p1->uf_tm_self, &p2->uf_tm_self);
}

/*
 * Start profiling function "fp".
 */
    خالی
func_do_profile(ufunc_T *fp)
{
    صحیح		len = fp->uf_سطریں.ga_len;

    اگر (!fp->uf_prof_initialized)
    {
	اگر (len == 0)
	    len = 1;  // avoid getting error for allocating zero bytes
	fp->uf_tm_گنتی = 0;
	profile_zero(&fp->uf_tm_self);
	profile_zero(&fp->uf_tm_کل);
	اگر (fp->uf_tml_گنتی == تہی)
	    fp->uf_tml_گنتی = ALLOC_CLEAR_MULT(صحیح, len);
	اگر (fp->uf_tml_کل == تہی)
	    fp->uf_tml_کل = ALLOC_CLEAR_MULT(proftime_T, len);
	اگر (fp->uf_tml_self == تہی)
	    fp->uf_tml_self = ALLOC_CLEAR_MULT(proftime_T, len);
	fp->uf_tml_idx = -1;
	اگر (fp->uf_tml_گنتی == تہی || fp->uf_tml_کل == تہی
						    || fp->uf_tml_self == تہی)
	    واپس;	    // out of memory
	fp->uf_prof_initialized = TRUE;
    }

    fp->uf_profiling = TRUE;
}

/*
 * Save time when starting to invoke another script or function.
 */
    ساکن خالی
script_prof_save(
    proftime_T	*tm)	    // place to store wait time
{
    scriptitem_T    *si;

    اگر (SCRIPT_ID_VALID(current_sctx.sc_sid))
    {
	si = SCRIPT_شے(current_sctx.sc_sid);
	اگر (si->sn_prof_on && si->sn_pr_nest++ == 0)
	    profile_آغاز(&si->sn_pr_بچہ);
    }
    profile_get_wait(tm);
}

/*
 * When calling a function: may initialize for profiling.
 */
    خالی
profile_may_آغاز_func(profinfo_T *info, ufunc_T *fp, ufunc_T *caller)
{
    اگر (!fp->uf_profiling && has_profiling(FALSE, fp->uf_name, تہی,
								&fp->uf_hash))
    {
	info->pi_started_profiling = TRUE;
	func_do_profile(fp);
    }
    اگر (fp->uf_profiling || (caller != تہی && caller->uf_profiling))
    {
	++fp->uf_tm_گنتی;
	profile_آغاز(&info->pi_call_آغاز);
	profile_zero(&fp->uf_tm_children);
    }
    script_prof_save(&info->pi_wait_آغاز);
}

/*
 * After calling a function: may handle profiling.  profile_may_start_func()
 * must have been called previously.
 */
    خالی
profile_may_انتہا_func(profinfo_T *info, ufunc_T *fp, ufunc_T *caller)
{
    profile_انتہا(&info->pi_call_آغاز);
    profile_sub_wait(&info->pi_wait_آغاز, &info->pi_call_آغاز);
    profile_جوڑو(&fp->uf_tm_کل, &info->pi_call_آغاز);
    profile_self(&fp->uf_tm_self, &info->pi_call_آغاز, &fp->uf_tm_children);
    اگر (caller != تہی && caller->uf_profiling)
    {
	profile_جوڑو(&caller->uf_tm_children, &info->pi_call_آغاز);
	profile_جوڑو(&caller->uf_tml_children, &info->pi_call_آغاز);
    }
    اگر (info->pi_started_profiling)
	// make a ":profdel func" stop profiling the function
	fp->uf_profiling = FALSE;
}

/*
 * Prepare profiling for entering a child or something else that is not
 * counted for the script/function itself.
 * Should always be called in pair with prof_child_exit().
 */
    خالی
prof_بچہ_enter(
    proftime_T *tm)	// place to store waittime
{
    funccall_T *fc = get_current_funccal();

    اگر (fc != تہی && fc->fc_func->uf_profiling)
	profile_آغاز(&fc->fc_prof_بچہ);
    script_prof_save(tm);
}

/*
 * Take care of time spent in a child.
 * Should always be called after prof_child_enter().
 */
    خالی
prof_بچہ_exit(
    proftime_T *tm)	// where waittime was stored
{
    funccall_T *fc = get_current_funccal();

    اگر (fc != تہی && fc->fc_func->uf_profiling)
    {
	profile_انتہا(&fc->fc_prof_بچہ);
	profile_sub_wait(tm, &fc->fc_prof_بچہ); // don't count waiting time
	profile_جوڑو(&fc->fc_func->uf_tm_children, &fc->fc_prof_بچہ);
	profile_جوڑو(&fc->fc_func->uf_tml_children, &fc->fc_prof_بچہ);
    }
    script_prof_restore(tm);
}

/*
 * Called when starting to read a function line.
 * "sourcing_lnum" must be correct!
 * When skipping lines it may not actually be executed, but we won't find out
 * until later and we need to store the time now.
 */
    خالی
func_سطر_آغاز(خالی *cookie, طویل lnum)
{
    funccall_T	*fcp = (funccall_T *)cookie;
    ufunc_T	*fp = fcp->fc_func;

    اگر (fp->uf_profiling && lnum >= 1 && lnum <= fp->uf_سطریں.ga_len)
    {
	fp->uf_tml_idx = lnum - 1;
	// Skip continuation lines.
	جب (fp->uf_tml_idx > 0 && FUNCLINE(fp, fp->uf_tml_idx) == تہی)
	    --fp->uf_tml_idx;
	fp->uf_tml_execed = FALSE;
	profile_آغاز(&fp->uf_tml_آغاز);
	profile_zero(&fp->uf_tml_children);
	profile_get_wait(&fp->uf_tml_wait);
    }
}

/*
 * Called when actually executing a function line.
 */
    خالی
func_سطر_exec(خالی *cookie)
{
    funccall_T	*fcp = (funccall_T *)cookie;
    ufunc_T	*fp = fcp->fc_func;

    اگر (fp->uf_profiling && fp->uf_tml_idx >= 0)
	fp->uf_tml_execed = TRUE;
}

/*
 * Called when done with a function line.
 */
    خالی
func_سطر_انتہا(خالی *cookie)
{
    funccall_T	*fcp = (funccall_T *)cookie;
    ufunc_T	*fp = fcp->fc_func;

    اگر (fp->uf_profiling && fp->uf_tml_idx >= 0)
    {
	اگر (fp->uf_tml_execed)
	{
	    ++fp->uf_tml_گنتی[fp->uf_tml_idx];
	    profile_انتہا(&fp->uf_tml_آغاز);
	    profile_sub_wait(&fp->uf_tml_wait, &fp->uf_tml_آغاز);
	    profile_جوڑو(&fp->uf_tml_کل[fp->uf_tml_idx], &fp->uf_tml_آغاز);
	    profile_self(&fp->uf_tml_self[fp->uf_tml_idx], &fp->uf_tml_آغاز,
							&fp->uf_tml_children);
	}
	fp->uf_tml_idx = -1;
    }
}

/*
 * Dump the profiling results for all functions in file "fd".
 */
    ساکن خالی
func_dump_profile(FILE *fd)
{
    hashtab_T	*functbl;
    hashitem_T	*hi;
    صحیح		todo;
    ufunc_T	*fp;
    صحیح		i;
    ufunc_T	**sorttab;
    صحیح		st_len = 0;
    char_u	*p;

    functbl = func_tbl_get();
    todo = (صحیح)functbl->ht_مستعمل;
    اگر (todo == 0)
	واپس;     // nothing to dump

    sorttab = ALLOC_MULT(ufunc_T *, todo);

    FOR_ALL_HASHTAB_اشیاء(functbl, hi, todo)
    {
	اگر (!HASHITEM_خالی_خانے(hi))
	{
	    --todo;
	    fp = HI2UF(hi);
	    اگر (fp->uf_prof_initialized)
	    {
		اگر (sorttab != تہی)
		    sorttab[st_len++] = fp;

		اگر (fp->uf_name[0] == K_SPECIAL)
		    fprintf(fd, "FUNCTION  <SNR>%s()\n", fp->uf_name + 3);
		ورنہ
		    fprintf(fd, "FUNCTION  %s()\n", fp->uf_name);
		اگر (fp->uf_script_ctx.sc_sid > 0)
		{
		    p = home_replace_save(تہی,
				     get_scriptname(fp->uf_script_ctx.sc_sid));
		    اگر (p != تہی)
		    {
			fprintf(fd, "    Defined: %s:%ld\n",
					   p, (طویل)fp->uf_script_ctx.sc_lnum);
			vim_free(p);
		    }
		}
		اگر (fp->uf_tm_گنتی == 1)
		    fprintf(fd, "Called 1 time\n");
		ورنہ
		    fprintf(fd, "Called %d times\n", fp->uf_tm_گنتی);
		fprintf(fd, "Total time: %s\n", profile_پیغام(&fp->uf_tm_کل));
		fprintf(fd, " Self time: %s\n", profile_پیغام(&fp->uf_tm_self));
		fprintf(fd, "\n");
		fprintf(fd, "%s\n", PROF_TOTALS_سرخی);

		برائے (i = 0; i < fp->uf_سطریں.ga_len; ++i)
		{
		    اگر (FUNCLINE(fp, i) == تہی)
			جاری;
		    prof_func_سطر(fd, fp->uf_tml_گنتی[i],
			     &fp->uf_tml_کل[i], &fp->uf_tml_self[i], TRUE);
		    fprintf(fd, "%s\n", FUNCLINE(fp, i));
		}
		fprintf(fd, "\n");
	    }
	}
    }

    اگر (sorttab != تہی && st_len > 0)
    {
	qsort((خالی *)sorttab, (size_t)st_len, حجم(ufunc_T *),
							      prof_کل_cmp);
	prof_ترتیب_دو_list(fd, sorttab, st_len, "TOTAL", FALSE);
	qsort((خالی *)sorttab, (size_t)st_len, حجم(ufunc_T *),
							      prof_self_cmp);
	prof_ترتیب_دو_list(fd, sorttab, st_len, "SELF", TRUE);
    }

    vim_free(sorttab);
}

/*
 * Start profiling script "fp".
 */
    خالی
script_do_profile(scriptitem_T *si)
{
    si->sn_pr_گنتی = 0;
    profile_zero(&si->sn_pr_کل);
    profile_zero(&si->sn_pr_self);

    ga_init2(&si->sn_prl_ga, حجم(sn_prl_T), 100);
    si->sn_prl_idx = -1;
    si->sn_prof_on = TRUE;
    si->sn_pr_nest = 0;
}

/*
 * Count time spent in children after invoking another script or function.
 */
    خالی
script_prof_restore(proftime_T *tm)
{
    scriptitem_T    *si;

    اگر (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	واپس;

    si = SCRIPT_شے(current_sctx.sc_sid);
    اگر (si->sn_prof_on && --si->sn_pr_nest == 0)
    {
	profile_انتہا(&si->sn_pr_بچہ);
	profile_sub_wait(tm, &si->sn_pr_بچہ); // don't count wait time
	profile_جوڑو(&si->sn_pr_children, &si->sn_pr_بچہ);
	profile_جوڑو(&si->sn_prl_children, &si->sn_pr_بچہ);
    }
}

/*
 * Dump the profiling results for all scripts in file "fd".
 */
    ساکن خالی
script_dump_profile(FILE *fd)
{
    صحیح		    id;
    scriptitem_T    *si;
    صحیح		    i;
    FILE	    *sfd;
    sn_prl_T	    *pp;

    برائے (id = 1; id <= script_اشیاء.ga_len; ++id)
    {
	si = SCRIPT_شے(id);
	اگر (si->sn_prof_on)
	{
	    fprintf(fd, "SCRIPT  %s\n", si->sn_name);
	    اگر (si->sn_pr_گنتی == 1)
		fprintf(fd, "Sourced 1 time\n");
	    ورنہ
		fprintf(fd, "Sourced %d times\n", si->sn_pr_گنتی);
	    fprintf(fd, "Total time: %s\n", profile_پیغام(&si->sn_pr_کل));
	    fprintf(fd, " Self time: %s\n", profile_پیغام(&si->sn_pr_self));
	    fprintf(fd, "\n");
	    fprintf(fd, "%s\n", PROF_TOTALS_سرخی);

	    sfd = mch_fopen((حرف *)si->sn_name, "r");
	    اگر (sfd == تہی)
		fprintf(fd, "Cannot open file!\n");
	    ورنہ
	    {
		// Keep going till the end of file, so that trailing
		// continuation lines are listed.
		برائے (i = 0; ; ++i)
		{
		    اگر (vim_fgets(IObuff, IOSIZE, sfd))
			توڑو;
		    // When a line has been truncated, append NL, taking care
		    // of multi-byte characters .
		    اگر (IObuff[IOSIZE - 2] != NUL && IObuff[IOSIZE - 2] != NL)
		    {
			صحیح n = IOSIZE - 2;

			اگر (enc_utf8)
			{
			    // Move to the first byte of this char.
			    // utf_head_off() doesn't work, because it checks
			    // for a truncated character.
			    جب (n > 0 && (IObuff[n] & 0xc0) == 0x80)
				--n;
			}
			ورنہ اگر (has_mbyte)
			    n -= mb_سر_off(IObuff, IObuff + n);
			IObuff[n] = NL;
			IObuff[n + 1] = NUL;
		    }
		    اگر (i < si->sn_prl_ga.ga_len
				     && (pp = &PRL_شے(si, i))->snp_گنتی > 0)
		    {
			fprintf(fd, "%5d ", pp->snp_گنتی);
			اگر (profile_equal(&pp->sn_prl_کل, &pp->sn_prl_self))
			    fprintf(fd, "           ");
			ورنہ
			    fprintf(fd, "%s ", profile_پیغام(&pp->sn_prl_کل));
			fprintf(fd, "%s ", profile_پیغام(&pp->sn_prl_self));
		    }
		    ورنہ
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
    خالی
profile_dump(خالی)
{
    FILE	*fd;

    اگر (profile_fname == تہی)
	واپس;

    fd = mch_fopen((حرف *)profile_fname, "w");
    اگر (fd == تہی)
	semsg(_(e_cant_open_file_str), profile_fname);
    ورنہ
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
    خالی
script_سطر_آغاز(خالی)
{
    scriptitem_T    *si;
    sn_prl_T	    *pp;

    اگر (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	واپس;
    si = SCRIPT_شے(current_sctx.sc_sid);
    اگر (si->sn_prof_on && SOURCING_LNUM >= 1)
    {
	// Grow the array before starting the timer, so that the time spent
	// here isn't counted.
	(خالی)ga_grow(&si->sn_prl_ga,
				  (صحیح)(SOURCING_LNUM - si->sn_prl_ga.ga_len));
	si->sn_prl_idx = SOURCING_LNUM - 1;
	جب (si->sn_prl_ga.ga_len <= si->sn_prl_idx
		&& si->sn_prl_ga.ga_len < si->sn_prl_ga.ga_maxlen)
	{
	    // Zero counters for a line that was not used before.
	    pp = &PRL_شے(si, si->sn_prl_ga.ga_len);
	    pp->snp_گنتی = 0;
	    profile_zero(&pp->sn_prl_کل);
	    profile_zero(&pp->sn_prl_self);
	    ++si->sn_prl_ga.ga_len;
	}
	si->sn_prl_execed = FALSE;
	profile_آغاز(&si->sn_prl_آغاز);
	profile_zero(&si->sn_prl_children);
	profile_get_wait(&si->sn_prl_wait);
    }
}

/*
 * Called when actually executing a function line.
 */
    خالی
script_سطر_exec(خالی)
{
    scriptitem_T    *si;

    اگر (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	واپس;
    si = SCRIPT_شے(current_sctx.sc_sid);
    اگر (si->sn_prof_on && si->sn_prl_idx >= 0)
	si->sn_prl_execed = TRUE;
}

/*
 * Called when done with a script line.
 */
    خالی
script_سطر_انتہا(خالی)
{
    scriptitem_T    *si;
    sn_prl_T	    *pp;

    اگر (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	واپس;
    si = SCRIPT_شے(current_sctx.sc_sid);
    اگر (si->sn_prof_on && si->sn_prl_idx >= 0
				     && si->sn_prl_idx < si->sn_prl_ga.ga_len)
    {
	اگر (si->sn_prl_execed)
	{
	    pp = &PRL_شے(si, si->sn_prl_idx);
	    ++pp->snp_گنتی;
	    profile_انتہا(&si->sn_prl_آغاز);
	    profile_sub_wait(&si->sn_prl_wait, &si->sn_prl_آغاز);
	    profile_جوڑو(&pp->sn_prl_کل, &si->sn_prl_آغاز);
	    profile_self(&pp->sn_prl_self, &si->sn_prl_آغاز,
							&si->sn_prl_children);
	}
	si->sn_prl_idx = -1;
    }
}
# endif // FEAT_PROFILE

#اگر_ختم
