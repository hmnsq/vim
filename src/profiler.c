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

#अन्तर्भाव "vim.h"

#यदि defined(FEAT_EVAL)
# यदि defined(FEAT_PROFILE) || defined(FEAT_RELTIME)
/*
 * Store the current time in "tm".
 */
    शून्य
profile_start(proftime_T *पंचांग)
{
#  ifdef MSWIN
    QueryPerformanceCounter(पंचांग);
#  अन्यथा
    PROF_GET_TIME(पंचांग);
#  endif
}

/*
 * Compute the elapsed time from "tm" till now and store in "tm".
 */
    शून्य
profile_end(proftime_T *पंचांग)
{
    proftime_T now;

#  ifdef MSWIN
    QueryPerformanceCounter(&now);
    पंचांग->QuadPart = now.QuadPart - पंचांग->QuadPart;
#  अन्यथा
    PROF_GET_TIME(&now);
    पंचांग->tv_fsec = now.tv_fsec - पंचांग->tv_fsec;
    पंचांग->tv_sec = now.tv_sec - पंचांग->tv_sec;
    यदि (पंचांग->tv_fsec < 0)
    {
	पंचांग->tv_fsec += TV_FSEC_SEC;
	--पंचांग->tv_sec;
    }
#  endif
}

/*
 * Subtract the time "tm2" from "tm".
 */
    शून्य
profile_sub(proftime_T *पंचांग, proftime_T *tm2)
{
#  ifdef MSWIN
    पंचांग->QuadPart -= tm2->QuadPart;
#  अन्यथा
    पंचांग->tv_fsec -= tm2->tv_fsec;
    पंचांग->tv_sec -= tm2->tv_sec;
    यदि (पंचांग->tv_fsec < 0)
    {
	पंचांग->tv_fsec += TV_FSEC_SEC;
	--पंचांग->tv_sec;
    }
#  endif
}

/*
 * Return a string that represents the time in "tm".
 * Uses a static buffer!
 */
    वर्ण *
profile_सन्देश(proftime_T *पंचांग)
{
    स्थिर वर्ण buf[50];

#  ifdef MSWIN
    LARGE_INTEGER   fr;

    QueryPerformanceFrequency(&fr);
    प्र_लिखो(buf, "%10.6lf", (द्विगुण)पंचांग->QuadPart / (द्विगुण)fr.QuadPart);
#  अन्यथा
    प्र_लिखो(buf, PROF_TIME_FORMAT, (दीर्घ)पंचांग->tv_sec, (दीर्घ)पंचांग->tv_fsec);
#  endif
    फर्काऊ buf;
}

/*
 * Return a float that represents the time in "tm".
 */
    float_T
profile_float(proftime_T *पंचांग)
{
#  ifdef MSWIN
    LARGE_INTEGER   fr;

    QueryPerformanceFrequency(&fr);
    फर्काऊ (float_T)पंचांग->QuadPart / (float_T)fr.QuadPart;
#  अन्यथा
    फर्काऊ (float_T)पंचांग->tv_sec + (float_T)पंचांग->tv_fsec / (float_T)TV_FSEC_SEC;
#  endif
}

/*
 * Put the time "msec" past now in "tm".
 */
    शून्य
profile_setlimit(दीर्घ msec, proftime_T *पंचांग)
{
    यदि (msec <= 0)   // no limit
	profile_zero(पंचांग);
    अन्यथा
    {
#  ifdef MSWIN
	LARGE_INTEGER   fr;

	QueryPerformanceCounter(पंचांग);
	QueryPerformanceFrequency(&fr);
	पंचांग->QuadPart += (LONGLONG)((द्विगुण)msec / 1000.0 * (द्विगुण)fr.QuadPart);
#  अन्यथा
	varnumber_T	    fsec;	// this should be 64 bit if possible

	PROF_GET_TIME(पंचांग);
	fsec = (varnumber_T)पंचांग->tv_fsec
		       + (varnumber_T)msec * (varnumber_T)(TV_FSEC_SEC / 1000);
	पंचांग->tv_fsec = fsec % (दीर्घ)TV_FSEC_SEC;
	पंचांग->tv_sec += fsec / (दीर्घ)TV_FSEC_SEC;
#  endif
    }
}

/*
 * Return TRUE if the current time is past "tm".
 */
    पूर्णाङ्क
profile_passed_limit(proftime_T *पंचांग)
{
    proftime_T	now;

#  ifdef MSWIN
    यदि (पंचांग->QuadPart == 0)  // timer was not set
	फर्काऊ FALSE;
    QueryPerformanceCounter(&now);
    फर्काऊ (now.QuadPart > पंचांग->QuadPart);
#  अन्यथा
    यदि (पंचांग->tv_sec == 0)    // timer was not set
	फर्काऊ FALSE;
    PROF_GET_TIME(&now);
    फर्काऊ (now.tv_sec > पंचांग->tv_sec
	    || (now.tv_sec == पंचांग->tv_sec && now.tv_fsec > पंचांग->tv_fsec));
#  endif
}

/*
 * Set the time in "tm" to zero.
 */
    शून्य
profile_zero(proftime_T *पंचांग)
{
#  ifdef MSWIN
    पंचांग->QuadPart = 0;
#  अन्यथा
    पंचांग->tv_fsec = 0;
    पंचांग->tv_sec = 0;
#  endif
}

# endif  // FEAT_PROFILE || FEAT_RELTIME

# यदि defined(FEAT_SYN_HL) && defined(FEAT_RELTIME) && defined(FEAT_PROFILE)
#  यदि defined(HAVE_MATH_H)
#   include <math.h>
#  endif

/*
 * Divide the time "tm" by "count" and store in "tm2".
 */
    शून्य
profile_divide(proftime_T *पंचांग, पूर्णाङ्क गन्ती, proftime_T *tm2)
{
    यदि (गन्ती == 0)
	profile_zero(tm2);
    अन्यथा
    {
#  ifdef MSWIN
	tm2->QuadPart = पंचांग->QuadPart / गन्ती;
#  अन्यथा
	द्विगुण fsec = (पंचांग->tv_sec * (float_T)TV_FSEC_SEC + पंचांग->tv_fsec)
								       / गन्ती;

	tm2->tv_sec = न्यूनमान(fsec / (float_T)TV_FSEC_SEC);
	tm2->tv_fsec = vim_round(fsec - (tm2->tv_sec * (float_T)TV_FSEC_SEC));
#  endif
    }
}
# endif

# यदि defined(FEAT_PROFILE)
/*
 * Functions for profiling.
 */
स्थिर proftime_T prof_wait_time;

/*
 * Add the time "tm2" to "tm".
 */
    शून्य
profile_add(proftime_T *पंचांग, proftime_T *tm2)
{
#  ifdef MSWIN
    पंचांग->QuadPart += tm2->QuadPart;
#  अन्यथा
    पंचांग->tv_fsec += tm2->tv_fsec;
    पंचांग->tv_sec += tm2->tv_sec;
    यदि (पंचांग->tv_fsec >= TV_FSEC_SEC)
    {
	पंचांग->tv_fsec -= TV_FSEC_SEC;
	++पंचांग->tv_sec;
    }
#  endif
}

/*
 * Add the "self" time from the total time and the children's time.
 */
    शून्य
profile_self(proftime_T *self, proftime_T *total, proftime_T *children)
{
    // Check that the result won't be negative.  Can happen with recursive
    // calls.
#  ifdef MSWIN
    यदि (total->QuadPart <= children->QuadPart)
	फर्काऊ;
#  अन्यथा
    यदि (total->tv_sec < children->tv_sec
	    || (total->tv_sec == children->tv_sec
		&& total->tv_fsec <= children->tv_fsec))
	फर्काऊ;
#  endif
    profile_add(self, total);
    profile_sub(self, children);
}

/*
 * Get the current waittime.
 */
    स्थिर शून्य
profile_get_wait(proftime_T *पंचांग)
{
    *पंचांग = prof_wait_time;
}

/*
 * Subtract the passed waittime since "tm" from "tma".
 */
    शून्य
profile_sub_wait(proftime_T *पंचांग, proftime_T *tma)
{
    proftime_T tm3 = prof_wait_time;

    profile_sub(&tm3, पंचांग);
    profile_sub(tma, &tm3);
}

/*
 * Return TRUE if "tm1" and "tm2" are equal.
 */
    स्थिर पूर्णाङ्क
profile_equal(proftime_T *tm1, proftime_T *tm2)
{
#  ifdef MSWIN
    फर्काऊ (tm1->QuadPart == tm2->QuadPart);
#  अन्यथा
    फर्काऊ (tm1->tv_fsec == tm2->tv_fsec && tm1->tv_sec == tm2->tv_sec);
#  endif
}

/*
 * Return <0, 0 or >0 if "tm1" < "tm2", "tm1" == "tm2" or "tm1" > "tm2"
 */
    पूर्णाङ्क
profile_cmp(अचर proftime_T *tm1, अचर proftime_T *tm2)
{
#  ifdef MSWIN
    फर्काऊ tm2->QuadPart == tm1->QuadPart ? 0 :
	tm2->QuadPart > tm1->QuadPart ? 1 : -1;
#  अन्यथा
    यदि (tm1->tv_sec == tm2->tv_sec)
	फर्काऊ tm2->tv_fsec == tm1->tv_fsec ? 0 :
	    tm2->tv_fsec > tm1->tv_fsec ? 1 : -1;
    फर्काऊ tm2->tv_sec > tm1->tv_sec ? 1 : -1;
#  endif
}

स्थिर char_u	*profile_fname = रिक्त;
स्थिर proftime_T pause_time;

/*
 * Reset all profiling information.
 */
    स्थिर शून्य
profile_reset(शून्य)
{
    पूर्णाङ्क		id;
    पूर्णाङ्क		todo;
    hashtab_T	*functbl;
    hashitem_T	*hi;

    // Reset sourced files.
    लागि (id = 1; id <= script_वस्तुहरू.ga_len; id++)
    {
	scriptitem_T *si = SCRIPT_ITEM(id);
	यदि (si->sn_prof_on)
	{
	    si->sn_prof_on      = FALSE;
	    si->sn_pr_force     = FALSE;
	    profile_zero(&si->sn_pr_child);
	    si->sn_pr_nest      = 0;
	    si->sn_pr_गन्ती     = 0;
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
    todo = (पूर्णाङ्क)functbl->ht_used;

    FOR_ALL_HASHTAB_वस्तुहरू(functbl, hi, todo)
    {
	ufunc_T *fp;
	पूर्णाङ्क	i;

	यदि (HASHITEM_EMPTY(hi))
	    जारी;

	todo--;
	fp = HI2UF(hi);
	यदि (fp->uf_prof_initialized)
	{
	    fp->uf_profiling    = 0;
	    fp->uf_prof_initialized = FALSE;
	    fp->uf_tm_गन्ती     = 0;
	    profile_zero(&fp->uf_tm_total);
	    profile_zero(&fp->uf_tm_self);
	    profile_zero(&fp->uf_tm_children);

	    लागि (i = 0; i < fp->uf_पङ्क्तिहरू.ga_len; i++)
	    {
		fp->uf_tml_गन्ती[i] = 0;
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
    शून्य
ex_profile(exarg_T *eap)
{
    char_u	*e;
    पूर्णाङ्क		len;

    e = skiptowhite(eap->arg);
    len = (पूर्णाङ्क)(e - eap->arg);
    e = skipwhite(e);

    यदि (len == 5 && STRNCMP(eap->arg, "start", 5) == 0 && *e != NUL)
    {
	VIM_CLEAR(profile_fname);
	profile_fname = expand_env_save_opt(e, TRUE, रिक्त);
	do_profiling = PROF_YES;
	profile_zero(&prof_wait_time);
	set_vim_var_nr(VV_PROFILING, 1L);
    }
    अन्यथा यदि (do_profiling == PROF_NONE)
	emsg(_(e_first_use_profile_start_fname));
    अन्यथा यदि (STRCMP(eap->arg, "stop") == 0)
    {
	profile_dump();
	do_profiling = PROF_NONE;
	set_vim_var_nr(VV_PROFILING, 0L);
	profile_reset();
    }
    अन्यथा यदि (STRCMP(eap->arg, "pause") == 0)
    {
	यदि (do_profiling == PROF_YES)
	    profile_start(&pause_time);
	do_profiling = PROF_PAUSED;
    }
    अन्यथा यदि (STRCMP(eap->arg, "continue") == 0)
    {
	यदि (do_profiling == PROF_PAUSED)
	{
	    profile_end(&pause_time);
	    profile_add(&prof_wait_time, &pause_time);
	}
	do_profiling = PROF_YES;
    }
    अन्यथा यदि (STRCMP(eap->arg, "dump") == 0)
	profile_dump();
    अन्यथा
    {
	// The rest is similar to ":breakadd".
	ex_breakadd(eap);
    }
}

// Command line expansion for :profile.
स्थिर गणना
{
    PEXP_SUBCMD,	// expand :profile sub-commands
    PEXP_FUNC		// expand :profile func {funcname}
} pexpand_what;

स्थिर वर्ण *pexpand_cmds[] = {
			"start",
			"stop",
			"pause",
			"continue",
			"func",
			"file",
			"dump",
			रिक्त
};

/*
 * Function given to ExpandGeneric() to obtain the profile command
 * specific expansion.
 */
    char_u *
get_profile_name(expand_T *xp UNUSED, पूर्णाङ्क idx)
{
    विकल्प (pexpand_what)
    {
    पक्ष PEXP_SUBCMD:
	फर्काऊ (char_u *)pexpand_cmds[idx];
    स्वभाव:
	फर्काऊ रिक्त;
    }
}

/*
 * Handle command line completion for :profile command.
 */
    शून्य
set_context_in_profile_cmd(expand_T *xp, char_u *arg)
{
    char_u	*end_subcmd;

    // Default: expand subcommands.
    xp->xp_context = EXPAND_PROFILE;
    pexpand_what = PEXP_SUBCMD;
    xp->xp_pattern = arg;

    end_subcmd = skiptowhite(arg);
    यदि (*end_subcmd == NUL)
	फर्काऊ;

    यदि ((end_subcmd - arg == 5 && STRNCMP(arg, "start", 5) == 0)
	    || (end_subcmd - arg == 4 && STRNCMP(arg, "file", 4) == 0))
    {
	xp->xp_context = EXPAND_FILES;
	xp->xp_pattern = skipwhite(end_subcmd);
	फर्काऊ;
    }
    अन्यथा यदि (end_subcmd - arg == 4 && STRNCMP(arg, "func", 4) == 0)
    {
	xp->xp_context = EXPAND_USER_FUNC;
	xp->xp_pattern = skipwhite(end_subcmd);
	फर्काऊ;
    }

    xp->xp_context = EXPAND_NOTHING;
}

स्थिर proftime_T inchar_time;

/*
 * Called when starting to wait for the user to type a character.
 */
    शून्य
prof_inchar_enter(शून्य)
{
    profile_start(&inchar_time);
}

/*
 * Called when finished waiting for the user to type a character.
 */
    शून्य
prof_inchar_exit(शून्य)
{
    profile_end(&inchar_time);
    profile_add(&prof_wait_time, &inchar_time);
}


/*
 * Return TRUE when a function defined in the current script should be
 * profiled.
 */
    पूर्णाङ्क
prof_def_func(शून्य)
{
    यदि (current_sctx.sc_sid > 0)
	फर्काऊ SCRIPT_ITEM(current_sctx.sc_sid)->sn_pr_force;
    फर्काऊ FALSE;
}

/*
 * Print the count and times for one function or function line.
 */
    स्थिर शून्य
prof_func_पङ्क्ति(
    खाता	*fd,
    पूर्णाङ्क		गन्ती,
    proftime_T	*total,
    proftime_T	*self,
    पूर्णाङ्क		prefer_self)	// when equal print only self time
{
    यदि (गन्ती > 0)
    {
	ख_लिखो(fd, "%5d ", गन्ती);
	यदि (prefer_self && profile_equal(total, self))
	    ख_लिखो(fd, PROF_TIME_BLANK);
	अन्यथा
	    ख_लिखो(fd, "%s ", profile_सन्देश(total));
	यदि (!prefer_self && profile_equal(total, self))
	    ख_लिखो(fd, PROF_TIME_BLANK);
	अन्यथा
	    ख_लिखो(fd, "%s ", profile_सन्देश(self));
    }
    अन्यथा
	ख_लिखो(fd, "      %s%s", PROF_TIME_BLANK, PROF_TIME_BLANK);
}

    स्थिर शून्य
prof_sort_list(
    खाता	*fd,
    ufunc_T	**sorttab,
    पूर्णाङ्क		st_len,
    वर्ण	*title,
    पूर्णाङ्क		prefer_self)	// when equal print only self time
{
    पूर्णाङ्क		i;
    ufunc_T	*fp;

    ख_लिखो(fd, "FUNCTIONS SORTED ON %s TIME\n", title);
    ख_लिखो(fd, "%s  function\n", PROF_TOTALS_HEADER);
    लागि (i = 0; i < 20 && i < st_len; ++i)
    {
	fp = sorttab[i];
	prof_func_पङ्क्ति(fd, fp->uf_tm_गन्ती, &fp->uf_tm_total, &fp->uf_tm_self,
								 prefer_self);
	यदि (fp->uf_name[0] == K_SPECIAL)
	    ख_लिखो(fd, " <SNR>%s()\n", fp->uf_name + 3);
	अन्यथा
	    ख_लिखो(fd, " %s()\n", fp->uf_name);
    }
    ख_लिखो(fd, "\n");
}

/*
 * Compare function for total time sorting.
 */
    स्थिर पूर्णाङ्क
prof_total_cmp(अचर शून्य *s1, अचर शून्य *s2)
{
    ufunc_T	*p1, *p2;

    p1 = *(ufunc_T **)s1;
    p2 = *(ufunc_T **)s2;
    फर्काऊ profile_cmp(&p1->uf_tm_total, &p2->uf_tm_total);
}

/*
 * Compare function for self time sorting.
 */
    स्थिर पूर्णाङ्क
prof_self_cmp(अचर शून्य *s1, अचर शून्य *s2)
{
    ufunc_T	*p1, *p2;

    p1 = *(ufunc_T **)s1;
    p2 = *(ufunc_T **)s2;
    फर्काऊ profile_cmp(&p1->uf_tm_self, &p2->uf_tm_self);
}

/*
 * Start profiling function "fp".
 */
    शून्य
func_do_profile(ufunc_T *fp)
{
    पूर्णाङ्क		len = fp->uf_पङ्क्तिहरू.ga_len;

    यदि (!fp->uf_prof_initialized)
    {
	यदि (len == 0)
	    len = 1;  // avoid getting error for allocating zero bytes
	fp->uf_tm_गन्ती = 0;
	profile_zero(&fp->uf_tm_self);
	profile_zero(&fp->uf_tm_total);
	यदि (fp->uf_tml_गन्ती == रिक्त)
	    fp->uf_tml_गन्ती = ALLOC_CLEAR_MULT(पूर्णाङ्क, len);
	यदि (fp->uf_tml_total == रिक्त)
	    fp->uf_tml_total = ALLOC_CLEAR_MULT(proftime_T, len);
	यदि (fp->uf_tml_self == रिक्त)
	    fp->uf_tml_self = ALLOC_CLEAR_MULT(proftime_T, len);
	fp->uf_tml_idx = -1;
	यदि (fp->uf_tml_गन्ती == रिक्त || fp->uf_tml_total == रिक्त
						    || fp->uf_tml_self == रिक्त)
	    फर्काऊ;	    // out of memory
	fp->uf_prof_initialized = TRUE;
    }

    fp->uf_profiling = TRUE;
}

/*
 * Save time when starting to invoke another script or function.
 */
    स्थिर शून्य
script_prof_save(
    proftime_T	*पंचांग)	    // place to store wait time
{
    scriptitem_T    *si;

    यदि (SCRIPT_ID_VALID(current_sctx.sc_sid))
    {
	si = SCRIPT_ITEM(current_sctx.sc_sid);
	यदि (si->sn_prof_on && si->sn_pr_nest++ == 0)
	    profile_start(&si->sn_pr_child);
    }
    profile_get_wait(पंचांग);
}

/*
 * When calling a function: may initialize for profiling.
 */
    शून्य
profile_may_start_func(profinfo_T *info, ufunc_T *fp, ufunc_T *caller)
{
    यदि (!fp->uf_profiling && has_profiling(FALSE, fp->uf_name, रिक्त,
								&fp->uf_hash))
    {
	info->pi_started_profiling = TRUE;
	func_do_profile(fp);
    }
    यदि (fp->uf_profiling || (caller != रिक्त && caller->uf_profiling))
    {
	++fp->uf_tm_गन्ती;
	profile_start(&info->pi_call_start);
	profile_zero(&fp->uf_tm_children);
    }
    script_prof_save(&info->pi_wait_start);
}

/*
 * After calling a function: may handle profiling.  profile_may_start_func()
 * must have been called previously.
 */
    शून्य
profile_may_end_func(profinfo_T *info, ufunc_T *fp, ufunc_T *caller)
{
    profile_end(&info->pi_call_start);
    profile_sub_wait(&info->pi_wait_start, &info->pi_call_start);
    profile_add(&fp->uf_tm_total, &info->pi_call_start);
    profile_self(&fp->uf_tm_self, &info->pi_call_start, &fp->uf_tm_children);
    यदि (caller != रिक्त && caller->uf_profiling)
    {
	profile_add(&caller->uf_tm_children, &info->pi_call_start);
	profile_add(&caller->uf_tml_children, &info->pi_call_start);
    }
    यदि (info->pi_started_profiling)
	// make a ":profdel func" stop profiling the function
	fp->uf_profiling = FALSE;
}

/*
 * Prepare profiling for entering a child or something else that is not
 * counted for the script/function itself.
 * Should always be called in pair with prof_child_exit().
 */
    शून्य
prof_child_enter(
    proftime_T *पंचांग)	// place to store waittime
{
    funccall_T *fc = get_current_funccal();

    यदि (fc != रिक्त && fc->fc_func->uf_profiling)
	profile_start(&fc->fc_prof_child);
    script_prof_save(पंचांग);
}

/*
 * Take care of time spent in a child.
 * Should always be called after prof_child_enter().
 */
    शून्य
prof_child_exit(
    proftime_T *पंचांग)	// where waittime was stored
{
    funccall_T *fc = get_current_funccal();

    यदि (fc != रिक्त && fc->fc_func->uf_profiling)
    {
	profile_end(&fc->fc_prof_child);
	profile_sub_wait(पंचांग, &fc->fc_prof_child); // don't count waiting time
	profile_add(&fc->fc_func->uf_tm_children, &fc->fc_prof_child);
	profile_add(&fc->fc_func->uf_tml_children, &fc->fc_prof_child);
    }
    script_prof_restore(पंचांग);
}

/*
 * Called when starting to read a function line.
 * "sourcing_lnum" must be correct!
 * When skipping lines it may not actually be executed, but we won't find out
 * until later and we need to store the time now.
 */
    शून्य
func_पङ्क्ति_start(शून्य *cookie, दीर्घ lnum)
{
    funccall_T	*fcp = (funccall_T *)cookie;
    ufunc_T	*fp = fcp->fc_func;

    यदि (fp->uf_profiling && lnum >= 1 && lnum <= fp->uf_पङ्क्तिहरू.ga_len)
    {
	fp->uf_tml_idx = lnum - 1;
	// Skip continuation lines.
	जबसम्म (fp->uf_tml_idx > 0 && FUNCLINE(fp, fp->uf_tml_idx) == रिक्त)
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
    शून्य
func_पङ्क्ति_exec(शून्य *cookie)
{
    funccall_T	*fcp = (funccall_T *)cookie;
    ufunc_T	*fp = fcp->fc_func;

    यदि (fp->uf_profiling && fp->uf_tml_idx >= 0)
	fp->uf_tml_execed = TRUE;
}

/*
 * Called when done with a function line.
 */
    शून्य
func_पङ्क्ति_end(शून्य *cookie)
{
    funccall_T	*fcp = (funccall_T *)cookie;
    ufunc_T	*fp = fcp->fc_func;

    यदि (fp->uf_profiling && fp->uf_tml_idx >= 0)
    {
	यदि (fp->uf_tml_execed)
	{
	    ++fp->uf_tml_गन्ती[fp->uf_tml_idx];
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
    स्थिर शून्य
func_dump_profile(खाता *fd)
{
    hashtab_T	*functbl;
    hashitem_T	*hi;
    पूर्णाङ्क		todo;
    ufunc_T	*fp;
    पूर्णाङ्क		i;
    ufunc_T	**sorttab;
    पूर्णाङ्क		st_len = 0;
    char_u	*p;

    functbl = func_tbl_get();
    todo = (पूर्णाङ्क)functbl->ht_used;
    यदि (todo == 0)
	फर्काऊ;     // nothing to dump

    sorttab = ALLOC_MULT(ufunc_T *, todo);

    FOR_ALL_HASHTAB_वस्तुहरू(functbl, hi, todo)
    {
	यदि (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    fp = HI2UF(hi);
	    यदि (fp->uf_prof_initialized)
	    {
		यदि (sorttab != रिक्त)
		    sorttab[st_len++] = fp;

		यदि (fp->uf_name[0] == K_SPECIAL)
		    ख_लिखो(fd, "FUNCTION  <SNR>%s()\n", fp->uf_name + 3);
		अन्यथा
		    ख_लिखो(fd, "FUNCTION  %s()\n", fp->uf_name);
		यदि (fp->uf_script_ctx.sc_sid > 0)
		{
		    p = home_replace_save(रिक्त,
				     get_scriptname(fp->uf_script_ctx.sc_sid));
		    यदि (p != रिक्त)
		    {
			ख_लिखो(fd, "    Defined: %s:%ld\n",
					   p, (दीर्घ)fp->uf_script_ctx.sc_lnum);
			vim_free(p);
		    }
		}
		यदि (fp->uf_tm_गन्ती == 1)
		    ख_लिखो(fd, "Called 1 time\n");
		अन्यथा
		    ख_लिखो(fd, "Called %d times\n", fp->uf_tm_गन्ती);
		ख_लिखो(fd, "Total time: %s\n", profile_सन्देश(&fp->uf_tm_total));
		ख_लिखो(fd, " Self time: %s\n", profile_सन्देश(&fp->uf_tm_self));
		ख_लिखो(fd, "\n");
		ख_लिखो(fd, "%s\n", PROF_TOTALS_HEADER);

		लागि (i = 0; i < fp->uf_पङ्क्तिहरू.ga_len; ++i)
		{
		    यदि (FUNCLINE(fp, i) == रिक्त)
			जारी;
		    prof_func_पङ्क्ति(fd, fp->uf_tml_गन्ती[i],
			     &fp->uf_tml_total[i], &fp->uf_tml_self[i], TRUE);
		    ख_लिखो(fd, "%s\n", FUNCLINE(fp, i));
		}
		ख_लिखो(fd, "\n");
	    }
	}
    }

    यदि (sorttab != रिक्त && st_len > 0)
    {
	क्विक((शून्य *)sorttab, (माप_प्रकार)st_len, परिमाण(ufunc_T *),
							      prof_total_cmp);
	prof_sort_list(fd, sorttab, st_len, "TOTAL", FALSE);
	क्विक((शून्य *)sorttab, (माप_प्रकार)st_len, परिमाण(ufunc_T *),
							      prof_self_cmp);
	prof_sort_list(fd, sorttab, st_len, "SELF", TRUE);
    }

    vim_free(sorttab);
}

/*
 * Start profiling script "fp".
 */
    शून्य
script_do_profile(scriptitem_T *si)
{
    si->sn_pr_गन्ती = 0;
    profile_zero(&si->sn_pr_total);
    profile_zero(&si->sn_pr_self);

    ga_init2(&si->sn_prl_ga, परिमाण(sn_prl_T), 100);
    si->sn_prl_idx = -1;
    si->sn_prof_on = TRUE;
    si->sn_pr_nest = 0;
}

/*
 * Count time spent in children after invoking another script or function.
 */
    शून्य
script_prof_restore(proftime_T *पंचांग)
{
    scriptitem_T    *si;

    यदि (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	फर्काऊ;

    si = SCRIPT_ITEM(current_sctx.sc_sid);
    यदि (si->sn_prof_on && --si->sn_pr_nest == 0)
    {
	profile_end(&si->sn_pr_child);
	profile_sub_wait(पंचांग, &si->sn_pr_child); // don't count wait time
	profile_add(&si->sn_pr_children, &si->sn_pr_child);
	profile_add(&si->sn_prl_children, &si->sn_pr_child);
    }
}

/*
 * Dump the profiling results for all scripts in file "fd".
 */
    स्थिर शून्य
script_dump_profile(खाता *fd)
{
    पूर्णाङ्क		    id;
    scriptitem_T    *si;
    पूर्णाङ्क		    i;
    खाता	    *sfd;
    sn_prl_T	    *pp;

    लागि (id = 1; id <= script_वस्तुहरू.ga_len; ++id)
    {
	si = SCRIPT_ITEM(id);
	यदि (si->sn_prof_on)
	{
	    ख_लिखो(fd, "SCRIPT  %s\n", si->sn_name);
	    यदि (si->sn_pr_गन्ती == 1)
		ख_लिखो(fd, "Sourced 1 time\n");
	    अन्यथा
		ख_लिखो(fd, "Sourced %d times\n", si->sn_pr_गन्ती);
	    ख_लिखो(fd, "Total time: %s\n", profile_सन्देश(&si->sn_pr_total));
	    ख_लिखो(fd, " Self time: %s\n", profile_सन्देश(&si->sn_pr_self));
	    ख_लिखो(fd, "\n");
	    ख_लिखो(fd, "%s\n", PROF_TOTALS_HEADER);

	    sfd = mch_fopen((वर्ण *)si->sn_name, "r");
	    यदि (sfd == रिक्त)
		ख_लिखो(fd, "Cannot open file!\n");
	    अन्यथा
	    {
		// Keep going till the end of file, so that trailing
		// continuation lines are listed.
		लागि (i = 0; ; ++i)
		{
		    यदि (vim_fgets(IObuff, IOSIZE, sfd))
			रोक;
		    // When a line has been truncated, append NL, taking care
		    // of multi-byte characters .
		    यदि (IObuff[IOSIZE - 2] != NUL && IObuff[IOSIZE - 2] != NL)
		    {
			पूर्णाङ्क n = IOSIZE - 2;

			यदि (enc_utf8)
			{
			    // Move to the first byte of this char.
			    // utf_head_off() doesn't work, because it checks
			    // for a truncated character.
			    जबसम्म (n > 0 && (IObuff[n] & 0xc0) == 0x80)
				--n;
			}
			अन्यथा यदि (has_mbyte)
			    n -= mb_head_off(IObuff, IObuff + n);
			IObuff[n] = NL;
			IObuff[n + 1] = NUL;
		    }
		    यदि (i < si->sn_prl_ga.ga_len
				     && (pp = &PRL_ITEM(si, i))->snp_गन्ती > 0)
		    {
			ख_लिखो(fd, "%5d ", pp->snp_गन्ती);
			यदि (profile_equal(&pp->sn_prl_total, &pp->sn_prl_self))
			    ख_लिखो(fd, "           ");
			अन्यथा
			    ख_लिखो(fd, "%s ", profile_सन्देश(&pp->sn_prl_total));
			ख_लिखो(fd, "%s ", profile_सन्देश(&pp->sn_prl_self));
		    }
		    अन्यथा
			ख_लिखो(fd, "                            ");
		    ख_लिखो(fd, "%s", IObuff);
		}
		ख_बंद(sfd);
	    }
	    ख_लिखो(fd, "\n");
	}
    }
}

/*
 * Dump the profiling info.
 */
    शून्य
profile_dump(शून्य)
{
    खाता	*fd;

    यदि (profile_fname == रिक्त)
	फर्काऊ;

    fd = mch_fopen((वर्ण *)profile_fname, "w");
    यदि (fd == रिक्त)
	semsg(_(e_cant_open_file_str), profile_fname);
    अन्यथा
    {
	script_dump_profile(fd);
	func_dump_profile(fd);
	ख_बंद(fd);
    }
}

/*
 * Called when starting to read a script line.
 * "sourcing_lnum" must be correct!
 * When skipping lines it may not actually be executed, but we won't find out
 * until later and we need to store the time now.
 */
    शून्य
script_पङ्क्ति_start(शून्य)
{
    scriptitem_T    *si;
    sn_prl_T	    *pp;

    यदि (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	फर्काऊ;
    si = SCRIPT_ITEM(current_sctx.sc_sid);
    यदि (si->sn_prof_on && SOURCING_LNUM >= 1)
    {
	// Grow the array before starting the timer, so that the time spent
	// here isn't counted.
	(शून्य)ga_grow(&si->sn_prl_ga,
				  (पूर्णाङ्क)(SOURCING_LNUM - si->sn_prl_ga.ga_len));
	si->sn_prl_idx = SOURCING_LNUM - 1;
	जबसम्म (si->sn_prl_ga.ga_len <= si->sn_prl_idx
		&& si->sn_prl_ga.ga_len < si->sn_prl_ga.ga_maxlen)
	{
	    // Zero counters for a line that was not used before.
	    pp = &PRL_ITEM(si, si->sn_prl_ga.ga_len);
	    pp->snp_गन्ती = 0;
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
    शून्य
script_पङ्क्ति_exec(शून्य)
{
    scriptitem_T    *si;

    यदि (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	फर्काऊ;
    si = SCRIPT_ITEM(current_sctx.sc_sid);
    यदि (si->sn_prof_on && si->sn_prl_idx >= 0)
	si->sn_prl_execed = TRUE;
}

/*
 * Called when done with a script line.
 */
    शून्य
script_पङ्क्ति_end(शून्य)
{
    scriptitem_T    *si;
    sn_prl_T	    *pp;

    यदि (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	फर्काऊ;
    si = SCRIPT_ITEM(current_sctx.sc_sid);
    यदि (si->sn_prof_on && si->sn_prl_idx >= 0
				     && si->sn_prl_idx < si->sn_prl_ga.ga_len)
    {
	यदि (si->sn_prl_execed)
	{
	    pp = &PRL_ITEM(si, si->sn_prl_idx);
	    ++pp->snp_गन्ती;
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
