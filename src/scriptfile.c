/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * scriptfile.c: functions for dealing with the runtime directories/files
 */

#అన్తర్భావ "vim.h"

#ఒకవేళ defined(FEAT_EVAL)
// The names of packages that once were loaded are remembered.
స్థిర garray_T		ga_loaded = {0, 0, పరిమాణం(char_u *), 4, ఖాళీ};
#అయితే_ముగింపు

// last used sequence number for sourcing scripts (current_sctx.sc_seq)
#నిర్వచితమైతే FEAT_EVAL
స్థిర పూర్ణాంకం		చివరి_current_SID_seq = 0;
#అయితే_ముగింపు

స్థిర పూర్ణాంకం do_ఆధారం_ext(char_u *fname, పూర్ణాంకం check_other, పూర్ణాంకం is_vimrc, పూర్ణాంకం *ret_sid, exarg_T *eap, పూర్ణాంకం clearvars);

/*
 * Initialize the execution stack.
 */
    శూన్య
estack_init(శూన్య)
{
    estack_T *నమోదు_2;

    ఒకవేళ (ga_grow(&exestack, 10) == FAIL)
	mch_exit(0);
    నమోదు_2 = ((estack_T *)exestack.ga_దత్తాంశం) + exestack.ga_len;
    నమోదు_2->es_type = ETYPE_శిఖరం;
    నమోదు_2->es_name = ఖాళీ;
    నమోదు_2->es_lnum = 0;
#నిర్వచితమైతే FEAT_EVAL
    నమోదు_2->es_info.ufunc = ఖాళీ;
#అయితే_ముగింపు
    ++exestack.ga_len;
}

/*
 * Add an item to the execution stack.
 * Returns the new entry or NULL when out of memory.
 */
    estack_T *
estack_నెట్టు(etype_T type, char_u *name, దీర్ఘ lnum)
{
    estack_T *నమోదు_2;

    // If memory allocation fails then we'll pop more than we push, eventually
    // at the top level it will be OK again.
    ఒకవేళ (ga_grow(&exestack, 1) == FAIL)
	తిరిగి ఖాళీ;

    నమోదు_2 = ((estack_T *)exestack.ga_దత్తాంశం) + exestack.ga_len;
    నమోదు_2->es_type = type;
    నమోదు_2->es_name = name;
    నమోదు_2->es_lnum = lnum;
#నిర్వచితమైతే FEAT_EVAL
    నమోదు_2->es_info.ufunc = ఖాళీ;
#అయితే_ముగింపు
    ++exestack.ga_len;
    తిరిగి నమోదు_2;
}

#ఒకవేళ defined(FEAT_EVAL)
/*
 * Add a user function to the execution stack.
 */
    estack_T *
estack_నెట్టు_ufunc(ufunc_T *ufunc, దీర్ఘ lnum)
{
    estack_T *నమోదు_2 = estack_నెట్టు(ETYPE_UFUNC,
	    ufunc->uf_name_exp != ఖాళీ
				  ? ufunc->uf_name_exp : ufunc->uf_name, lnum);
    ఒకవేళ (నమోదు_2 != ఖాళీ)
	నమోదు_2->es_info.ufunc = ufunc;
    తిరిగి నమోదు_2;
}

/*
 * Return TRUE if "ufunc" with "lnum" is already at the top of the exe stack.
 */
    పూర్ణాంకం
estack_శిఖరం_is_ufunc(ufunc_T *ufunc, దీర్ఘ lnum)
{
    estack_T *నమోదు_2;

    ఒకవేళ (exestack.ga_len == 0)
	తిరిగి FALSE;
    నమోదు_2 = ((estack_T *)exestack.ga_దత్తాంశం) + exestack.ga_len - 1;
    తిరిగి నమోదు_2->es_type == ETYPE_UFUNC
	&& STRCMP( నమోదు_2->es_name, ufunc->uf_name_exp != ఖాళీ
				    ? ufunc->uf_name_exp : ufunc->uf_name) == 0
	&& నమోదు_2->es_lnum == lnum;
}
#అయితే_ముగింపు

/*
 * Take an item off of the execution stack and return it.
 */
    estack_T *
estack_తీయి(శూన్య)
{
    ఒకవేళ (exestack.ga_len == 0)
	తిరిగి ఖాళీ;
    --exestack.ga_len;
    తిరిగి ((estack_T *)exestack.ga_దత్తాంశం) + exestack.ga_len;
}

/*
 * Get the current value for "which" in allocated memory.
 * "which" is ESTACK_SFILE for <sfile>, ESTACK_STACK for <stack> or
 * ESTACK_SCRIPT for <script>.
 */
    char_u *
estack_sfile(estack_arg_T which వాడని)
{
    estack_T	*నమోదు_2;
#నిర్వచితమైతే FEAT_EVAL
    garray_T	ga;
    size_t	len;
    పూర్ణాంకం		idx;
    etype_T	చివరి_type = ETYPE_SCRIPT;
#అయితే_ముగింపు

    నమోదు_2 = ((estack_T *)exestack.ga_దత్తాంశం) + exestack.ga_len - 1;
#నిర్వచితమైతే FEAT_EVAL
    ఒకవేళ (which == ESTACK_SFILE && నమోదు_2->es_type != ETYPE_UFUNC)
#అయితే_ముగింపు
    {
	ఒకవేళ (నమోదు_2->es_name == ఖాళీ)
	    తిరిగి ఖాళీ;
	తిరిగి vim_strsave(నమోదు_2->es_name);
    }
#నిర్వచితమైతే FEAT_EVAL
    // expand('<sfile>') works in a function for backwards compatibility, but
    // may give an unexpected result.  Disallow it in Vim 9 script.
    ఒకవేళ (which == ESTACK_SFILE && లోపల_vim9script())
    {
	పూర్ణాంకం  save_emsg_off = emsg_off;

	ఒకవేళ (emsg_off == 1)
	    // f_expand() silences errors but we do want this one
	    emsg_off = 0;
	emsg(_(e_cannot_expand_sfile_లోపల_vim9_function));
	emsg_off = save_emsg_off;
	తిరిగి ఖాళీ;
    }

    // If evaluated in a function or autocommand, return the path of the script
    // where it is defined, at script level the current script path is returned
    // instead.
    ఒకవేళ (which == ESTACK_SCRIPT)
    {
	// Walk the stack backwards, starting from the current frame.
	కొరకు (idx = exestack.ga_len - 1; idx >= 0; --idx, --నమోదు_2)
	{
	    ఒకవేళ (నమోదు_2->es_type == ETYPE_UFUNC || నమోదు_2->es_type == ETYPE_AUCMD)
	    {
		sctx_T *def_ctx = నమోదు_2->es_type == ETYPE_UFUNC
			      ? &నమోదు_2->es_info.ufunc->uf_script_ctx
			      : నమోదు_2->es_info.aucmd != ఖాళీ
				  ? acp_script_ctx(నమోదు_2->es_info.aucmd) : ఖాళీ;

		తిరిగి def_ctx != ఖాళీ && def_ctx->sc_sid > 0
			   ? vim_strsave(SCRIPT_అంశం(def_ctx->sc_sid)->sn_name)
			   : ఖాళీ;
	    }
	    లేకపోతే ఒకవేళ (నమోదు_2->es_type == ETYPE_SCRIPT)
		తిరిగి vim_strsave(నమోదు_2->es_name);
	}
	తిరిగి ఖాళీ;
    }

    // Give information about each stack entry up to the root.
    // For a function we compose the call stack, as it was done in the past:
    //   "function One[123]..Two[456]..Three"
    ga_init2(&ga, పరిమాణం(అక్షరం), 100);
    కొరకు (idx = 0; idx < exestack.ga_len; ++idx)
    {
	నమోదు_2 = ((estack_T *)exestack.ga_దత్తాంశం) + idx;
	ఒకవేళ (నమోదు_2->es_name != ఖాళీ)
	{
	    దీర్ఘ	lnum = 0;
	    size_t	added = 0;
	    string_T	type_name = {(char_u *)"", 0};
	    string_T	class_name = {(char_u *)"", 0};
	    string_T	es_name = {నమోదు_2->es_name, STRLEN(నమోదు_2->es_name)};

	    ఒకవేళ (నమోదు_2->es_type != చివరి_type)
	    {
		ఎంపిక (నమోదు_2->es_type)
		{
		    స్థితి ETYPE_SCRIPT:
			STR_LITERAL_సమితి(type_name, "script ");
			ఆపు;
		    స్థితి ETYPE_UFUNC:
			STR_LITERAL_సమితి(type_name, "function ");
			ఆపు;
		    అప్రమేయ:
			ఆపు;
		}
		చివరి_type = నమోదు_2->es_type;
	    }
	    ఒకవేళ (నమోదు_2->es_type == ETYPE_UFUNC && నమోదు_2->es_info.ufunc->uf_class != ఖాళీ)
	    {
		class_name.string = నమోదు_2->es_info.ufunc->uf_class->class_name.string;
		class_name.నిడివి = నమోదు_2->es_info.ufunc->uf_class->class_name.నిడివి;
	    }
	    ఒకవేళ (idx == exestack.ga_len - 1)
		lnum = which == ESTACK_దొంతర ? SOURCING_LNUM : 0;
	    లేకపోతే
		lnum = నమోదు_2->es_lnum;

	    len = es_name.నిడివి + type_name.నిడివి + class_name.నిడివి + 26;
	    ఒకవేళ (ga_grow(&ga, (పూర్ణాంకం)len) == FAIL)
		ఆపు;
	    ga_concat_len(&ga, type_name.string, type_name.నిడివి);
	    // For class methods prepend "<class name>." to the function name.
	    ఒకవేళ (*class_name.string != NUL)
	    {
		added = vim_snprintf_safelen(
		    (అక్షరం *)ga.ga_దత్తాంశం + ga.ga_len,
		    ga.ga_maxlen - ga.ga_len,
		    "<SNR>%d_%s.",
		    నమోదు_2->es_info.ufunc->uf_script_ctx.sc_sid,
		    class_name.string);

		ga.ga_len += (పూర్ణాంకం)added;
	    }
	    ga_concat_len(&ga, es_name.string, es_name.నిడివి);
	    // For the bottom entry of <sfile>: do not add the line number, it is used in
	    // <slnum>.  Also leave it out when the number is not set.
	    ఒకవేళ (lnum != 0)
	    {
		added = vim_snprintf_safelen(
		    (అక్షరం *)ga.ga_దత్తాంశం + ga.ga_len,
		    ga.ga_maxlen - ga.ga_len,
		    "[%ld]",
		    lnum);

		ga.ga_len += (పూర్ణాంకం)added;
	    }
	    ఒకవేళ (idx != exestack.ga_len - 1)
		GA_CONCAT_LITERAL(&ga, "..");
	}
    }

    // Only NUL-terminate when not returning NULL.
    ఒకవేళ (ga.ga_దత్తాంశం != ఖాళీ)
	ga_append(&ga, NUL);
    తిరిగి (char_u *)ga.ga_దత్తాంశం;
#అయితే_ముగింపు
}

#నిర్వచితమైతే FEAT_EVAL
    స్థిర శూన్య
stacktrace_నెట్టు_అంశం(
	list_T		*l,
	ufunc_T		*fp,
	char_u		*event,
	linenr_T	lnum,
	char_u		*filepath)
{
    dict_T	*d;
    typval_T	tv;

    d = dict_alloc_తాళం(VAR_FIXED);
    ఒకవేళ (d == ఖాళీ)
	తిరిగి;

    tv.v_type = VAR_DICT;
    tv.v_తాళం = VAR_LOCKED;
    tv.vval.v_dict = d;

    ఒకవేళ (fp != ఖాళీ)
	dict_కూడు_func(d, "funcref", fp);
    ఒకవేళ (event != ఖాళీ)
	dict_కూడు_string(d, "event", event);
    dict_కూడు_సంఖ్య(d, "lnum", lnum);
    dict_కూడు_string(d, "filepath", filepath);

    list_append_tv(l, &tv);
}

/*
 * Create the stacktrace from exestack.
 */
    list_T *
stacktrace_create(శూన్య)
{
    list_T	*l;
    పూర్ణాంకం		i;

    l = list_alloc();
    ఒకవేళ (l == ఖాళీ)
	తిరిగి ఖాళీ;

    కొరకు (i = 0; i < exestack.ga_len; ++i)
    {
	estack_T *నమోదు_2 = &((estack_T *)exestack.ga_దత్తాంశం)[i];
	linenr_T lnum = నమోదు_2->es_lnum;

	ఒకవేళ (నమోదు_2->es_type == ETYPE_SCRIPT)
	    stacktrace_నెట్టు_అంశం(l, ఖాళీ, ఖాళీ, lnum, నమోదు_2->es_name);
	లేకపోతే ఒకవేళ (నమోదు_2->es_type == ETYPE_UFUNC)
	{
	    ufunc_T *fp = నమోదు_2->es_info.ufunc;
	    sctx_T sctx = fp->uf_script_ctx;
	    char_u *filepath = sctx.sc_sid > 0 ?
				   get_scriptname(sctx.sc_sid) : (char_u *)"";

	    lnum += sctx.sc_lnum;
	    stacktrace_నెట్టు_అంశం(l, fp, ఖాళీ, lnum, filepath);
	}
	లేకపోతే ఒకవేళ (నమోదు_2->es_type == ETYPE_AUCMD)
	{
	    // The autocmd may not have a matching pattern yet, in which case
	    // es_info.aucmd is still NULL.
	    sctx_T *sctx = నమోదు_2->es_info.aucmd != ఖాళీ
			       ? acp_script_ctx(నమోదు_2->es_info.aucmd) : ఖాళీ;
	    char_u *filepath = sctx != ఖాళీ && sctx->sc_sid > 0 ?
				   get_scriptname(sctx->sc_sid) : (char_u *)"";

	    ఒకవేళ (sctx != ఖాళీ)
		lnum += sctx->sc_lnum;
	    stacktrace_నెట్టు_అంశం(l, ఖాళీ, నమోదు_2->es_name, lnum, filepath);
	}
    }
    తిరిగి l;
}

/*
 * getstacktrace() function
 */
    శూన్య
f_getstacktrace(typval_T *argvars వాడని, typval_T *rettv)
{
    rettv_list_సమితి(rettv, stacktrace_create());
}
#అయితే_ముగింపు

/*
 * Get DIP_ flags from the [where] argument of a :runtime command.
 * "*argp" is advanced to after the [where] argument if it is found.
 */
    స్థిర పూర్ణాంకం
get_runtime_cmd_flags(char_u **argp, size_t where_len)
{
    char_u *arg = *argp;

    ఒకవేళ (where_len == 0)
	తిరిగి 0;

    ఒకవేళ (STRNCMP(arg, "START", where_len) == 0)
    {
	*argp = skipwhite(arg + where_len);
	తిరిగి DIP_ప్రారంభం + DIP_NORTP;
    }
    ఒకవేళ (STRNCMP(arg, "OPT", where_len) == 0)
    {
	*argp = skipwhite(arg + where_len);
	తిరిగి DIP_OPT + DIP_NORTP;
    }
    ఒకవేళ (STRNCMP(arg, "PACK", where_len) == 0)
    {
	*argp = skipwhite(arg + where_len);
	తిరిగి DIP_ప్రారంభం + DIP_OPT + DIP_NORTP;
    }
    ఒకవేళ (STRNCMP(arg, "ALL", where_len) == 0)
    {
	*argp = skipwhite(arg + where_len);
	తిరిగి DIP_ప్రారంభం + DIP_OPT;
    }

    తిరిగి 0;
}

/*
 * ":runtime [where] {name}"
 */
    శూన్య
ex_runtime(exarg_T *eap)
{
    char_u  *arg = eap->arg;
    పూర్ణాంకం	    flags = eap->forceit ? DIP_ALL : 0;
    char_u  *p = skiptowhite(arg);
    flags += get_runtime_cmd_flags(&arg, p - arg);
    ఆధారం_runtime(arg, flags);
}

స్థిర పూర్ణాంకం runtime_expand_flags;

/*
 * Set the completion context for the :runtime command.
 */
    శూన్య
సమితి_context_లోపల_runtime_cmd(expand_T *xp, char_u *arg)
{
    char_u  *p = skiptowhite(arg);
    runtime_expand_flags
	= *p != NUL ? get_runtime_cmd_flags(&arg, p - arg) : 0;
    // Skip to the last argument.
    అయితే (*(p = skiptowhite_esc(arg)) != NUL)
    {
	ఒకవేళ (runtime_expand_flags == 0)
	    // When there are multiple arguments and [where] is not specified,
	    // use an unrelated non-zero flag to avoid expanding [where].
	    runtime_expand_flags = DIP_ALL;
	arg = skipwhite(p);
    }
    xp->xp_context = EXPAND_RUNTIME;
    xp->xp_నమూనా = arg;
}

    స్థిర శూన్య
ఆధారం_callback(char_u *fname, శూన్య *cookie)
{
    (శూన్య)do_ఆధారం(fname, FALSE, DOSO_NONE, cookie);
}

#నిర్వచితమైతే FEAT_EVAL
/*
 * Find an already loaded script "name".
 * If found returns its script ID.  If not found returns -1.
 */
    పూర్ణాంకం
కనుగొను_script_by_name(char_u *name)
{
    పూర్ణాంకం		    sid;
    scriptitem_T    *si;

    కొరకు (sid = script_వస్తువులు.ga_len; sid > 0; --sid)
    {
	// We used to check inode here, but that doesn't work:
	// - If a script is edited and written, it may get a different
	//   inode number, even though to the user it is the same script.
	// - If a script is deleted and another script is written, with a
	//   different name, the inode may be re-used.
	si = SCRIPT_అంశం(sid);
	ఒకవేళ (si->sn_name != ఖాళీ && fnamecmp(si->sn_name, name) == 0)
	    తిరిగి sid;
    }
    తిరిగి -1;
}

/*
 * Add a new scriptitem with all items initialized.
 * When running out of memory "error" is set to FAIL.
 * Returns the script ID.
 */
    స్థిర పూర్ణాంకం
get_new_scriptitem(పూర్ణాంకం *error)
{
    స్థిర scid_T   చివరి_current_SID = 0;
    పూర్ణాంకం		    sid = ++చివరి_current_SID;
    scriptitem_T    *si = ఖాళీ;

    ఒకవేళ (ga_grow(&script_వస్తువులు, (పూర్ణాంకం)(sid - script_వస్తువులు.ga_len)) == FAIL)
    {
	*error = FAIL;
	తిరిగి sid;
    }
    అయితే (script_వస్తువులు.ga_len < sid)
    {
	si = ALLOC_CLEAR_ONE(scriptitem_T);
	ఒకవేళ (si == ఖాళీ)
	{
	    *error = FAIL;
	    తిరిగి sid;
	}
	++script_వస్తువులు.ga_len;
	SCRIPT_అంశం(script_వస్తువులు.ga_len) = si;
	si->sn_name = ఖాళీ;
	si->sn_సంస్కరణ = 1;

	// Allocate the local script variables to use for this script.
	new_script_vars(script_వస్తువులు.ga_len);
	ga_init2(&si->sn_var_vals, పరిమాణం(svar_T), 10);
	hash_init(&si->sn_all_vars.dv_hashtab);
	ga_init2(&si->sn_imports, పరిమాణం(imported_T), 10);
	ga_init2(&si->sn_type_list, పరిమాణం(type_T), 10);
# ifdef FEAT_PROFILE
	si->sn_prof_on = FALSE;
# endif
    }

    // "si" can't be NULL, check only to avoid a compiler warning
    ఒకవేళ (si != ఖాళీ)
	// Used to check script variable index is still valid.
	si->sn_script_seq = current_sctx.sc_seq;

    తిరిగి sid;
}

    పూర్ణాంకం
get_new_scriptitem_for_fname(పూర్ణాంకం *error, char_u *fname)
{
    పూర్ణాంకం sid = get_new_scriptitem(error);

    ఒకవేళ (*error == OK)
    {
	scriptitem_T *si = SCRIPT_అంశం(sid);

	si->sn_name = vim_strsave(fname);
	si->sn_state = SN_STATE_NOT_LOADED;
    }
    తిరిగి sid;
}

/*
 * If the script for "sid" is a symlink and "sn_source_sid" is not set
 * then initialize it. A new script_item is created if needed.
 */
    శూన్య
check_script_symlink(పూర్ణాంకం sid)
{
    scriptitem_T *si = SCRIPT_అంశం(sid);
    ఒకవేళ (si->sn_syml_checked || si->sn_sourced_sid > 0)
	తిరిగి;
    si->sn_syml_checked = TRUE;

    // If fname is a symbolic link, create an script_item for the real file.

    char_u *real_fname = fix_fname(si->sn_name);
    ఒకవేళ (real_fname != ఖాళీ && STRCMP(real_fname, si->sn_name) != 0)
    {
	పూర్ణాంకం real_sid = కనుగొను_script_by_name(real_fname);
	పూర్ణాంకం error2 = OK;
	పూర్ణాంకం new_sid = FALSE;
	ఒకవేళ (real_sid < 0)
	{
	    real_sid = get_new_scriptitem_for_fname(&error2, real_fname);
	    new_sid = TRUE;
	}
	ఒకవేళ (error2 == OK)
	{
	    si = SCRIPT_అంశం(sid);
	    si->sn_sourced_sid = real_sid;
	    ఒకవేళ (new_sid)
	    {
		SCRIPT_అంశం(real_sid)->sn_import_autoload
						    = si->sn_import_autoload;
		ఒకవేళ (si->sn_autoload_ఉపసర్గ != ఖాళీ)
		    SCRIPT_అంశం(real_sid)->sn_autoload_ఉపసర్గ =
					vim_strsave(si->sn_autoload_ఉపసర్గ);
	    }
	}
    }
    vim_free(real_fname);
}

    స్థిర శూన్య
కనుగొను_script_callback(char_u *fname, శూన్య *cookie)
{
    పూర్ణాంకం sid;
    పూర్ణాంకం error = OK;
    పూర్ణాంకం *ret_sid = cookie;

    sid = కనుగొను_script_by_name(fname);
    ఒకవేళ (sid < 0)
	// script does not exist yet, create a new scriptitem
	sid = get_new_scriptitem_for_fname(&error, fname);
    *ret_sid = sid;
}
#అయితే_ముగింపు

/*
 * Find the patterns in "name" in all directories in "path" and invoke
 * "callback(fname, cookie)".
 * "prefix" is prepended to each pattern in "name".
 * When "flags" has DIP_ALL: source all files, otherwise only the first one.
 * When "flags" has DIP_DIR: find directories instead of files.
 * When "flags" has DIP_ERR: give an error message if there is no match.
 *
 * Return FAIL when no file could be sourced, OK otherwise.
 */
    పూర్ణాంకం
do_లోపల_మార్గం(
    char_u	*మార్గం,
    అక్షరం	*ఉపసర్గ,
    char_u	*name,
    పూర్ణాంకం		flags,
    శూన్య	(*callback)(char_u *fname, శూన్య *ck),
    శూన్య	*cookie)
{
    char_u	*rtp;
    string_T	buf;
    char_u	*rtp_copy;
    పూర్ణాంకం		అంకె_files;
    char_u	**files;
    పూర్ణాంకం		i;
    పూర్ణాంకం		did_one = FALSE;
#నిర్వచితమైతే AMIGA
    నిర్మాణం Process	*proc = (నిర్మాణం Process *)కనుగొను_Task(0L);
    APTR		save_winptr = proc->pr_కిటికీ_Ptr;

    // Avoid a requester here for a volume that doesn't exist.
    proc->pr_కిటికీ_Ptr = (APTR)-1L;
#అయితే_ముగింపు

    // Make a copy of 'runtimepath'.  Invoking the callback may change the
    // value.
    rtp_copy = vim_strsave(మార్గం);
    buf.string = alloc(MAXPATHL);
    ఒకవేళ (buf.string != ఖాళీ && rtp_copy != ఖాళీ)
    {
	size_t	prefixlen = 0;
	size_t	namelen = 0;

	ఒకవేళ (name != ఖాళీ)
	{
	    prefixlen = STRLEN(ఉపసర్గ);
	    namelen = STRLEN(name);

	    ఒకవేళ (p_verbose > 10)
	    {
		verbose_enter();
		ఒకవేళ (*ఉపసర్గ != NUL)
		    smsg(_("Searching for \"%s\" under \"%s\" in \"%s\""),
					       (అక్షరం *)name, ఉపసర్గ, (అక్షరం *)మార్గం);
		లేకపోతే
		    smsg(_("Searching for \"%s\" in \"%s\""),
						       (అక్షరం *)name, (అక్షరం *)మార్గం);
		verbose_leave();
	    }
	}

	// Loop over all entries in 'runtimepath'.
	rtp = rtp_copy;
	అయితే (*rtp != NUL && ((flags & DIP_ALL) || !did_one))
	{
	    // Copy the path from 'runtimepath' to buf[].
	    buf.నిడివి = (size_t)copy_option_part(&rtp, buf.string, MAXPATHL, ",");

	    // Skip after or non-after directories.
	    ఒకవేళ (flags & (DIP_NOAFTER | DIP_AFTER))
	    {
		పూర్ణాంకం is_after = buf.నిడివి >= 5
		    && STRCMP(buf.string + buf.నిడివి - 5, "after") == 0;

		ఒకవేళ ((is_after && (flags & DIP_NOAFTER))
			|| (!is_after && (flags & DIP_AFTER)))
		    కొనసాగు;
	    }

	    ఒకవేళ (name == ఖాళీ)
	    {
		(*callback)(buf.string, (శూన్య *)&cookie);
		ఒకవేళ (!did_one)
		    did_one = (cookie == ఖాళీ);
	    }
	    లేకపోతే ఒకవేళ (buf.నిడివి + 2 + prefixlen + namelen < MAXPATHL)
	    {
		char_u	*np;
		char_u	*తోక;

		ఒకవేళ (*buf.string != NUL
		    && !after_pathsep(buf.string, buf.string + buf.నిడివి))
		{
		    STRCPY(buf.string + buf.నిడివి, PATHSEPSTR);
		    buf.నిడివి += పరిమాణం(PATHSEP);
		}
		STRCPY(buf.string + buf.నిడివి, ఉపసర్గ);
		buf.నిడివి += prefixlen;
		తోక = buf.string + buf.నిడివి;

		// Loop over all patterns in "name"
		np = name;
		అయితే (*np != NUL && ((flags & DIP_ALL) || !did_one))
		{
		    // Append the pattern from "name" to buf[].
		    copy_option_part(&np, తోక, (పూర్ణాంకం)(MAXPATHL - (తోక - buf.string)),
								       "\t ");

		    ఒకవేళ (p_verbose > 10)
		    {
			verbose_enter();
			smsg(_("Searching for \"%s\""), buf.string);
			verbose_leave();
		    }

		    // Expand wildcards, invoke the callback for each match.
		    ఒకవేళ (gen_expand_wildcards(1, &buf.string, &అంకె_files, &files,
				  (flags & DIP_DIR) ? EW_DIR : EW_FILE) == OK)
		    {
			కొరకు (i = 0; i < అంకె_files; ++i)
			{
			    (*callback)(files[i], cookie);
			    did_one = TRUE;
			    ఒకవేళ (!(flags & DIP_ALL))
				ఆపు;
			}
			FreeWild(అంకె_files, files);
		    }
		}
	    }
	}
    }
    vim_free(buf.string);
    vim_free(rtp_copy);
    ఒకవేళ (!did_one && name != ఖాళీ)
    {
	అక్షరం *basepath = మార్గం == p_rtp ? "runtimepath" : "packpath";

	ఒకవేళ (flags & DIP_ERR)
	    semsg(_(e_directory_not_దొరికింది_లోపల_str_str), basepath, name);
	లేకపోతే ఒకవేళ (p_verbose > 0)
	{
	    verbose_enter();
	    smsg(_("not found in '%s': \"%s\""), basepath, name);
	    verbose_leave();
	}
    }

#నిర్వచితమైతే AMIGA
    proc->pr_కిటికీ_Ptr = save_winptr;
#అయితే_ముగింపు

    తిరిగి did_one ? OK : FAIL;
}

/*
 * Find "name" in "path".  When found, invoke the callback function for
 * it: callback(fname, "cookie")
 * When "flags" has DIP_ALL repeat for all matches, otherwise only the first
 * one is used.
 * Returns OK when at least one match found, FAIL otherwise.
 *
 * If "name" is NULL calls callback for each entry in "path". Cookie is
 * passed by reference in this case, setting it to NULL indicates that callback
 * has done its job.
 */
    స్థిర పూర్ణాంకం
do_లోపల_మార్గం_and_pp(
    char_u	*మార్గం,
    char_u	*name,
    పూర్ణాంకం		flags,
    శూన్య	(*callback)(char_u *fname, శూన్య *ck),
    శూన్య	*cookie)
{
    పూర్ణాంకం		పూర్తి = FAIL;

    ఒకవేళ ((flags & DIP_NORTP) == 0)
	పూర్తి = do_లోపల_మార్గం(మార్గం, "", name, flags, callback, cookie);

    ఒకవేళ ((పూర్తి == FAIL || (flags & DIP_ALL)) && (flags & DIP_ప్రారంభం))
	పూర్తి = do_లోపల_మార్గం(p_pp, "pack/*/start/*/", name, flags, callback,
								       cookie);

    ఒకవేళ ((పూర్తి == FAIL || (flags & DIP_ALL)) && (flags & DIP_OPT))
	పూర్తి = do_లోపల_మార్గం(p_pp, "pack/*/opt/*/", name, flags, callback,
								       cookie);

    తిరిగి పూర్తి;
}

/*
 * Just like do_in_path_and_pp(), using 'runtimepath' for "path".
 */
    పూర్ణాంకం
do_లోపల_runtimepath(
    char_u	*name,
    పూర్ణాంకం		flags,
    శూన్య	(*callback)(char_u *fname, శూన్య *ck),
    శూన్య	*cookie)
{
    తిరిగి do_లోపల_మార్గం_and_pp(p_rtp, name, flags, callback, cookie);
}

/*
 * Source the file "name" from all directories in 'runtimepath'.
 * "name" can contain wildcards.
 * When "flags" has DIP_ALL: source all files, otherwise only the first one.
 *
 * return FAIL when no file could be sourced, OK otherwise.
 */
    పూర్ణాంకం
ఆధారం_runtime(char_u *name, పూర్ణాంకం flags)
{
    తిరిగి ఆధారం_లోపల_మార్గం(p_rtp, name, flags, ఖాళీ);
}

/*
 * Just like source_runtime(), but use "path" instead of 'runtimepath'
 * and return the script ID in "ret_sid".
 */
    పూర్ణాంకం
ఆధారం_లోపల_మార్గం(char_u *మార్గం, char_u *name, పూర్ణాంకం flags, పూర్ణాంకం *ret_sid)
{
    తిరిగి do_లోపల_మార్గం_and_pp(మార్గం, name, flags, ఆధారం_callback, ret_sid);
}

#ఒకవేళ defined(FEAT_EVAL)

/*
 * Find "name" in 'runtimepath'. If found a new scriptitem is created for it
 * and it's script ID is returned.
 * If not found returns -1.
 */
    పూర్ణాంకం
కనుగొను_script_లోపల_rtp(char_u *name)
{
    పూర్ణాంకం sid = -1;

    (శూన్య)do_లోపల_మార్గం_and_pp(p_rtp, name, DIP_ప్రారంభం | DIP_NOAFTER,
						   కనుగొను_script_callback, &sid);
    తిరిగి sid;
}

/*
 * Expand wildcards in "pat" and invoke do_source() for each match.
 */
    స్థిర శూన్య
ఆధారం_all_matches(char_u *pat)
{
    పూర్ణాంకం	    అంకె_files;
    char_u  **files;
    పూర్ణాంకం	    i;

    ఒకవేళ (gen_expand_wildcards(1, &pat, &అంకె_files, &files, EW_FILE) != OK)
	తిరిగి;

    కొరకు (i = 0; i < అంకె_files; ++i)
	(శూన్య)do_ఆధారం(files[i], FALSE, DOSO_NONE, ఖాళీ);
    FreeWild(అంకె_files, files);
}

/*
 * Add the package directory to 'runtimepath'.
 */
    స్థిర పూర్ణాంకం
కూడు_pack_dir_గమ్యం_rtp(char_u *fname)
{
    char_u  *p4, *p3, *p2, *p1, *p;
    char_u  *నమోదు_2;
    char_u  *insp = ఖాళీ;
    పూర్ణాంకం	    c;
    string_T  new_rtp;
    size_t  keep;
    size_t  p_rtp_len;
    string_T  afterdir = {ఖాళీ, 0};
    char_u  *after_insp = ఖాళీ;
    char_u  *ffname = ఖాళీ;
    size_t  fname_len;
    string_T	buf = {ఖాళీ, 0};
    పూర్ణాంకం	    retval = FAIL;

    p4 = p3 = p2 = p1 = get_past_తల(fname);
    కొరకు (p = p1; *p; MB_PTR_ADV(p))
	ఒకవేళ (vim_ispathsep_nocolon(*p))
	{
	    p4 = p3; p3 = p2; p2 = p1; p1 = p;
	}

    // now we have:
    // rtp/pack/name/start/name
    //    p4   p3   p2    p1
    //
    // find the part up to "pack" in 'runtimepath'
    c = *++p4; // append pathsep in order to expand symlink
    *p4 = NUL;
    ffname = fix_fname(fname);
    *p4 = c;
    ఒకవేళ (ffname == ఖాళీ)
	తిరిగి FAIL;

    // Find "ffname" in "p_rtp", ignoring '/' vs '\' differences.
    // Also stop at the first "after" directory.
    fname_len = STRLEN(ffname);
    buf.string = alloc(MAXPATHL);
    ఒకవేళ (buf.string == ఖాళీ)
	వెళ్ళు theend;

    p_rtp_len = 0;
    కొరకు (నమోదు_2 = p_rtp; *నమోదు_2 != NUL; )
    {
	char_u *cur_నమోదు = నమోదు_2;

	buf.నిడివి = (size_t)copy_option_part(&నమోదు_2, buf.string, MAXPATHL, ",");

	// keep track of p_rtp length as we go to make the STRLEN() below have less work to do
	p_rtp_len += (*(cur_నమోదు + buf.నిడివి) == ',') ? buf.నిడివి + 1 : buf.నిడివి;

	ఒకవేళ ((p = (char_u *)అక్షరశ్రేణి_శోధన((అక్షరం *)buf.string, "after")) != ఖాళీ
		&& p > buf.string
		&& vim_ispathsep(p[-1])
		&& (vim_ispathsep(p[5]) || p[5] == NUL || p[5] == ','))
	{
	    ఒకవేళ (insp == ఖాళీ)
		// Did not find "ffname" before the first "after" directory,
		// insert it before this entry.
		insp = cur_నమోదు;
	    after_insp = cur_నమోదు;
	    ఆపు;
	}

	ఒకవేళ (insp == ఖాళీ)
	{
	    char_u  *rtp_ffname;
	    పూర్ణాంకం	    సరిపోలిక;

	    ఒకవేళ (*buf.string != NUL && !after_pathsep(buf.string, buf.string + buf.నిడివి))
		STRCPY(buf.string + buf.నిడివి, PATHSEPSTR);
	    rtp_ffname = fix_fname(buf.string);
	    ఒకవేళ (rtp_ffname == ఖాళీ)
		వెళ్ళు theend;
	    సరిపోలిక = vim_fnamencmp(rtp_ffname, ffname, fname_len) == 0;
	    vim_free(rtp_ffname);
	    ఒకవేళ (సరిపోలిక)
		// Insert "ffname" after this entry (and comma).
		insp = నమోదు_2;
	}
    }

    // finish measuring the length of p_rtp
    p_rtp_len += STRLEN(p_rtp + p_rtp_len);
    ఒకవేళ (insp == ఖాళీ)
	// Both "fname" and "after" not found, append at the end.
	insp = p_rtp + p_rtp_len;

    // check if rtp/pack/name/start/name/after exists
    fname_len = STRLEN(fname);
    concat_fnames(fname, fname_len, (char_u *)"after", STRLEN_LITERAL("after"), TRUE, &afterdir);
    ఒకవేళ (afterdir.string == ఖాళీ || !mch_isdir(afterdir.string))
	afterdir.నిడివి = 0;

    new_rtp.string = alloc(p_rtp_len + fname_len + afterdir.నిడివి + 3); // add two for commas and one for NUL
    ఒకవేళ (new_rtp.string == ఖాళీ)
	వెళ్ళు theend;

    // We now have 'rtp' parts: {keep}{keep_after}{rest}.
    // Create new_rtp, first: {keep},{fname}
    keep = (size_t)(insp - p_rtp);
    mch_memmove(new_rtp.string, p_rtp, keep);
    new_rtp.నిడివి = keep;
    ఒకవేళ (*insp == NUL)
	new_rtp.string[new_rtp.నిడివి++] = ',';  // add comma before
    mch_memmove(new_rtp.string + new_rtp.నిడివి, fname, fname_len);
    new_rtp.నిడివి += fname_len;
    ఒకవేళ (*insp != NUL)
	new_rtp.string[new_rtp.నిడివి++] = ',';  // add comma after

    ఒకవేళ (afterdir.నిడివి > 0 && after_insp != ఖాళీ)
    {
	size_t keep_after = (size_t)(after_insp - p_rtp);
	size_t append_len = keep_after - keep;

	// Add to new_rtp: {keep},{fname}{keep_after},{afterdir}
	mch_memmove(new_rtp.string + new_rtp.నిడివి, p_rtp + keep,
							append_len);
	new_rtp.నిడివి += append_len;
	mch_memmove(new_rtp.string + new_rtp.నిడివి, afterdir.string, afterdir.నిడివి);
	new_rtp.నిడివి += afterdir.నిడివి;
	new_rtp.string[new_rtp.నిడివి++] = ',';
	keep = keep_after;
    }

    ఒకవేళ (p_rtp[keep] != NUL)
    {
	size_t	append_len = p_rtp_len - keep;

	// Append rest: {keep},{fname}{keep_after},{afterdir}{rest}
	mch_memmove(new_rtp.string + new_rtp.నిడివి, p_rtp + keep, append_len + 1);	// add one for NUL
	new_rtp.నిడివి += append_len;
    }
    లేకపోతే
	new_rtp.string[new_rtp.నిడివి] = NUL;

    ఒకవేళ (afterdir.నిడివి > 0 && after_insp == ఖాళీ)
    {
	// Append afterdir when "after" was not found:
	// {keep},{fname}{rest},{afterdir}
	new_rtp.string[new_rtp.నిడివి++] = ',';
	STRCPY(new_rtp.string + new_rtp.నిడివి, afterdir.string);
    }

    సమితి_option_విలువ_give_err((char_u *)"rtp", 0L, new_rtp.string, 0);
    vim_free(new_rtp.string);
    retval = OK;

theend:
    vim_free(buf.string);
    vim_free(ffname);
    vim_free(afterdir.string);
    తిరిగి retval;
}

/*
 * Load scripts in "plugin" and "ftdetect" directories of the package.
 */
    స్థిర పూర్ణాంకం
ఎక్కించు_pack_plugin(char_u *fname)
{
    స్థిర అక్షరం *plugpat = "%s/plugin/**/*.vim";
    స్థిర అక్షరం *ftpat = "%s/ftdetect/*.vim";
    పూర్ణాంకం		len;
    char_u	*ffname = fix_fname(fname);
    char_u	*pat = ఖాళీ;
    పూర్ణాంకం		retval = FAIL;

    ఒకవేళ (ffname == ఖాళీ)
	తిరిగి FAIL;
    len = (పూర్ణాంకం)STRLEN(ffname) + (పూర్ణాంకం)STRLEN(ftpat);
    pat = alloc(len);
    ఒకవేళ (pat == ఖాళీ)
	వెళ్ళు theend;
    vim_snprintf((అక్షరం *)pat, len, plugpat, ffname);
    ఆధారం_all_matches(pat);

    {
	char_u *cmd = vim_strsave((char_u *)"g:did_load_filetypes");

	// If runtime/filetype.vim wasn't loaded yet, the scripts will be
	// found when it loads.
	ఒకవేళ (cmd != ఖాళీ && eval_గమ్యం_సంఖ్య(cmd, FALSE) > 0)
	{
	    do_cmdline_cmd((char_u *)"augroup filetypedetect");
	    vim_snprintf((అక్షరం *)pat, len, ftpat, ffname);
	    ఆధారం_all_matches(pat);
	    do_cmdline_cmd((char_u *)"augroup END");
	}
	vim_free(cmd);
    }
    vim_free(pat);
    retval = OK;

theend:
    vim_free(ffname);
    తిరిగి retval;
}

// used for "cookie" of add_pack_plugin()
స్థిర పూర్ణాంకం APP_కూడు_DIR;
స్థిర పూర్ణాంకం APP_ఎక్కించు;
స్థిర పూర్ణాంకం APP_BOTH;

    స్థిర శూన్య
కూడు_pack_plugin(char_u *fname, శూన్య *cookie)
{
    ఒకవేళ (cookie != &APP_ఎక్కించు)
    {
	char_u	*buf = alloc(MAXPATHL);
	char_u	*p;
	పూర్ణాంకం	దొరికింది = FALSE;

	ఒకవేళ (buf == ఖాళీ)
	    తిరిగి;
	p = p_rtp;
	అయితే (*p != NUL)
	{
	    copy_option_part(&p, buf, MAXPATHL, ",");
	    ఒకవేళ (pathcmp((అక్షరం *)buf, (అక్షరం *)fname, -1) == 0)
	    {
		దొరికింది = TRUE;
		ఆపు;
	    }
	}
	vim_free(buf);
	ఒకవేళ (!దొరికింది)
	    // directory is not yet in 'runtimepath', add it
	    ఒకవేళ (కూడు_pack_dir_గమ్యం_rtp(fname) == FAIL)
		తిరిగి;
    }

    ఒకవేళ (cookie != &APP_కూడు_DIR)
	ఎక్కించు_pack_plugin(fname);
}

/*
 * Add all packages in the "start" directory to 'runtimepath'.
 */
    శూన్య
కూడు_pack_ప్రారంభం_dirs(శూన్య)
{
    do_లోపల_మార్గం(p_pp, "", (char_u *)"pack/*/start/*", DIP_ALL + DIP_DIR,
					       కూడు_pack_plugin, &APP_కూడు_DIR);
}

/*
 * Load plugins from all packages in the "start" directory.
 */
    శూన్య
ఎక్కించు_ప్రారంభం_packages(శూన్య)
{
    did_ఆధారం_packages = TRUE;
    do_లోపల_మార్గం(p_pp, "", (char_u *)"pack/*/start/*", DIP_ALL + DIP_DIR,
						  కూడు_pack_plugin, &APP_ఎక్కించు);
}

/*
 * ":packloadall"
 * Find plugins in the package directories and source them.
 */
    శూన్య
ex_packloadall(exarg_T *eap)
{
    ఒకవేళ (!did_ఆధారం_packages || eap->forceit)
    {
	// First do a round to add all directories to 'runtimepath', then load
	// the plugins. This allows for plugins to use an autoload directory
	// of another plugin.
	కూడు_pack_ప్రారంభం_dirs();
	ఎక్కించు_ప్రారంభం_packages();
    }
}

/*
 * ":packadd[!] {name}"
 */
    శూన్య
ex_packadd(exarg_T *eap)
{
    స్థిర అక్షరం *plugpat = "pack/*/%s/%s";
    పూర్ణాంకం		len;
    అక్షరం	*pat;
    పూర్ణాంకం		విడత;
    పూర్ణాంకం		res = OK;

    // Round 1: use "start", round 2: use "opt".
    కొరకు (విడత = 1; విడత <= 2; ++విడత)
    {
	// Only look under "start" when loading packages wasn't done yet.
	ఒకవేళ (విడత == 1 && did_ఆధారం_packages)
	    కొనసాగు;

	len = (పూర్ణాంకం)STRLEN(plugpat) + (పూర్ణాంకం)STRLEN(eap->arg) + 5;
	pat = alloc(len);
	ఒకవేళ (pat == ఖాళీ)
	    తిరిగి;
	vim_snprintf(pat, len, plugpat, విడత == 1 ? "start" : "opt", eap->arg);
	// The first round don't give a "not found" error, in the second round
	// only when nothing was found in the first round.
	res = do_లోపల_మార్గం(p_pp, "", (char_u *)pat,
		DIP_ALL + DIP_DIR + (విడత == 2 && res == FAIL ? DIP_ERR : 0),
		కూడు_pack_plugin, eap->forceit ? &APP_కూడు_DIR : &APP_BOTH);
	vim_free(pat);
    }
}
#అయితే_ముగింపు

/*
 * Sort "gap" and remove duplicate entries.  "gap" is expected to contain a
 * list of file names in allocated memory.
 */
    శూన్య
remove_duplicates(garray_T *gap)
{
    పూర్ణాంకం	    i;
    పూర్ణాంకం	    j;
    char_u  **fnames = (char_u **)gap->ga_దత్తాంశం;

    క్రమీకరించు_అక్షరశ్రేణులు(fnames, gap->ga_len);
    కొరకు (i = gap->ga_len - 1; i > 0; --i)
	ఒకవేళ (fnamecmp(fnames[i - 1], fnames[i]) == 0)
	{
	    vim_free(fnames[i]);
	    కొరకు (j = i + 1; j < gap->ga_len; ++j)
		fnames[j - 1] = fnames[j];
	    --gap->ga_len;
	}
}

    స్థిర శూన్య
ExpandRTDir_int(
    char_u	*pat,
    size_t	pat_len,
    పూర్ణాంకం		flags,
    పూర్ణాంకం		keep_ext,
    garray_T	*gap,
    అక్షరం	*dirnames[])
{
    కొరకు (పూర్ణాంకం i = 0; dirnames[i] != ఖాళీ; ++i)
    {
	స్థిరరాశి size_t	buf_len = STRLEN(dirnames[i]) + pat_len + 64;
	అక్షరం		*buf = alloc(buf_len);
	ఒకవేళ (buf == ఖాళీ)
	{
	    ga_clear_అక్షరశ్రేణులు(gap);
	    తిరిగి;
	}
	పూర్ణాంకం		glob_flags = 0;
	పూర్ణాంకం		expand_dirs = FALSE;

	// Build base pattern
	vim_snprintf(buf, buf_len, "%s%s%s%s",
		     *dirnames[i] ? dirnames[i] : "", *dirnames[i] ? "/" : "",
		     pat, "*.vim");

expand:
	ఒకవేళ ((flags & DIP_NORTP) == 0)
	    globpath(p_rtp, (char_u *)buf, gap, glob_flags, expand_dirs);

	ఒకవేళ (flags & DIP_ప్రారంభం)
	{
	    // Build complete search path: pack/*/start/*/dirnames[i]/pat*.vim
	    vim_snprintf(buf, buf_len, "pack/*/start/*/%s%s%s%s",
			 *dirnames[i] ? dirnames[i] : "",
			 *dirnames[i] ? "/" : "",
			 pat,
			 expand_dirs ? "*" : "*.vim");
	    globpath(p_pp, (char_u *)buf, gap, glob_flags, expand_dirs);
	}

	ఒకవేళ (flags & DIP_OPT)
	{
	    // Build complete search path: pack/*/opt/*/dirnames[i]/pat*.vim
	    vim_snprintf(buf, buf_len, "pack/*/opt/*/%s%s%s%s",
			 *dirnames[i] ? dirnames[i] : "",
			 *dirnames[i] ? "/" : "", pat,
			 expand_dirs ? "*" : "*.vim");
	    globpath(p_pp, (char_u *)buf, gap, glob_flags, expand_dirs);
	}

	// Second round for directories
	ఒకవేళ (*dirnames[i] == NUL && !expand_dirs)
	{
	    // expand dir names in another round
	    vim_snprintf(buf, buf_len, "%s*", pat);
	    glob_flags = WILD_కూడు_SLASH;
	    expand_dirs = TRUE;
	    వెళ్ళు expand;
	}

	vim_free(buf);
    }

    పూర్ణాంకం pat_pathsep_cnt = 0;
    కొరకు (size_t i = 0; i < pat_len; ++i)
    {
	ఒకవేళ (vim_ispathsep(pat[i]))
	    ++pat_pathsep_cnt;
    }

    కొరకు (పూర్ణాంకం i = 0; i < gap->ga_len; ++i)
    {
	char_u *సరిపోలిక = ((char_u **)gap->ga_దత్తాంశం)[i];
	char_u *s = సరిపోలిక;
	char_u *e = s + STRLEN(s);
	ఒకవేళ (e - 4 > s && !keep_ext && STRNICMP(e - 4, ".vim", 4) == 0)
	{
	    e -= 4;
	    *e = NUL;
	}

	పూర్ణాంకం సరిపోలిక_pathsep_cnt = (e > s && e[-1] == '/') ? -1 : 0;
	కొరకు (s = e; s > సరిపోలిక; MB_PTR_వెనుక(సరిపోలిక, s))
	{
	    ఒకవేళ (s < సరిపోలిక || (vim_ispathsep(*s)
				     && ++సరిపోలిక_pathsep_cnt > pat_pathsep_cnt))
		ఆపు;
	}
	++s;
	ఒకవేళ (s != సరిపోలిక)
	    mch_memmove(సరిపోలిక, s, e - s + 1);
    }

    ఒకవేళ (gap->ga_len == 0)
	తిరిగి;

    // Sort and remove duplicates which can happen when specifying multiple
    // directories in dirnames.
    remove_duplicates(gap);
}

/*
 * Expand runtime file names.
 * Search from 'runtimepath':
 *   'runtimepath'/{dirnames}/{pat}.vim
 * When "flags" has DIP_START: search also from "start" of 'packpath':
 *   'packpath'/pack/ * /start/ * /{dirnames}/{pat}.vim
 * When "flags" has DIP_OPT: search also from "opt" of 'packpath':
 *   'packpath'/pack/ * /opt/ * /{dirnames}/{pat}.vim
 * "dirnames" is an array with one or more directory names.
 */
    పూర్ణాంకం
ExpandRTDir(
    char_u	*pat,
    పూర్ణాంకం		flags,
    పూర్ణాంకం		*అంకె_file,
    char_u	***file,
    అక్షరం	*dirnames[])
{
    *అంకె_file = 0;
    *file = ఖాళీ;

    garray_T	ga;
    ga_init2(&ga, పరిమాణం(అక్షరం *), 10);

    ExpandRTDir_int(pat, STRLEN(pat), flags, FALSE, &ga, dirnames);

    ఒకవేళ (ga.ga_len == 0)
	తిరిగి FAIL;

    *file = ga.ga_దత్తాంశం;
    *అంకె_file = ga.ga_len;
    తిరిగి OK;
}

/*
 * Handle command line completion for :runtime command.
 */
    పూర్ణాంకం
expand_runtime_cmd(char_u *pat, పూర్ణాంకం *అంకె_Matches, char_u ***matches)
{
    *అంకె_Matches = 0;
    *matches = ఖాళీ;

    garray_T	ga;
    ga_init2(&ga, పరిమాణం(అక్షరం *), 10);

    size_t pat_len = (పూర్ణాంకం)STRLEN(pat);
    అక్షరం *dirnames[] = {"", ఖాళీ};
    ExpandRTDir_int(pat, pat_len, runtime_expand_flags, TRUE, &ga, dirnames);

    // Try to complete values for [where] argument when none was found.
    ఒకవేళ (runtime_expand_flags == 0)
    {
	అక్షరం *where_విలువలు[] = {"START", "OPT", "PACK", "ALL"};
	కొరకు (size_t i = 0; i < ARRAY_నిడివి(where_విలువలు); ++i)
	    ఒకవేళ (STRNCMP(pat, where_విలువలు[i], pat_len) == 0)
	    {
		char_u *p = vim_strsave((char_u *)where_విలువలు[i]);
		ఒకవేళ (p != ఖాళీ && ga_కూడు_string(&ga, p) == FAIL)
		    vim_free(p);
	    }
    }

    ఒకవేళ (ga.ga_len == 0)
	తిరిగి FAIL;

    *matches = ga.ga_దత్తాంశం;
    *అంకె_Matches = ga.ga_len;
    తిరిగి OK;
}

/*
 * Expand loadplugin names:
 * 'packpath'/pack/ * /opt/{pat}
 */
    పూర్ణాంకం
Expand_Pack_కూడు_Dir(
    char_u	*pat,
    పూర్ణాంకం		*అంకె_file,
    char_u	***file)
{
    char_u	*s;
    char_u	*e;
    char_u	*సరిపోలిక;
    garray_T	ga;
    పూర్ణాంకం		i;
    పూర్ణాంకం		pat_len;

    *అంకె_file = 0;
    *file = ఖాళీ;
    pat_len = (పూర్ణాంకం)STRLEN(pat);
    ga_init2(&ga, పరిమాణం(అక్షరం *), 10);

    s = alloc(pat_len + 26);
    ఒకవేళ (s == ఖాళీ)
    {
	ga_clear_అక్షరశ్రేణులు(&ga);
	తిరిగి FAIL;
    }
    sprintf((అక్షరం *)s, "pack/*/opt/%s*", pat);
    globpath(p_pp, s, &ga, 0, TRUE);
    vim_free(s);

    కొరకు (i = 0; i < ga.ga_len; ++i)
    {
	సరిపోలిక = ((char_u **)ga.ga_దత్తాంశం)[i];
	s = gettail(సరిపోలిక);
	e = s + STRLEN(s);
	mch_memmove(సరిపోలిక, s, e - s + 1);
    }

    ఒకవేళ (ga.ga_len == 0)
	తిరిగి FAIL;

    // Sort and remove duplicates which can happen when specifying multiple
    // directories in dirnames.
    remove_duplicates(&ga);

    *file = ga.ga_దత్తాంశం;
    *అంకె_file = ga.ga_len;
    తిరిగి OK;
}

    స్థిర శూన్య
cmd_ఆధారం(char_u *fname, exarg_T *eap)
{
    పూర్ణాంకం clearvars = FALSE;

    ఒకవేళ (*fname != NUL && STRNCMP(fname, "++clear", 7) == 0)
    {
	// ++clear argument is supplied
	clearvars = TRUE;
	fname = fname + 7;
	ఒకవేళ (*fname != NUL)
	{
	    semsg(_(e_invalid_వాదం_str), eap->arg);
	    తిరిగి;
	}
    }

    ఒకవేళ (*fname != NUL && eap != ఖాళీ && eap->addr_లెక్క > 0)
    {
	// if a filename is specified to :source, then a range is not allowed
	emsg(_(e_no_range_allowed));
	తిరిగి;
    }

    ఒకవేళ (eap != ఖాళీ && *fname == NUL)
    {
	ఒకవేళ (eap->forceit)
	    // a file name is needed to source normal mode commands
	    emsg(_(e_వాదం_required));
	లేకపోతే
	    // source ex commands from the current buffer
	    do_ఆధారం_ext(ఖాళీ, FALSE, DOSO_NONE, ఖాళీ, eap, clearvars);
    }
    లేకపోతే ఒకవేళ (eap != ఖాళీ && eap->forceit)
	// ":source!": read Normal mode commands
	// Need to execute the commands directly.  This is required at least
	// for:
	// - ":g" command busy
	// - after ":argdo", ":windo" or ":bufdo"
	// - another command follows
	// - inside a loop
	openscript(fname, global_busy || listcmd_busy || eap->nextcmd != ఖాళీ
#నిర్వచితమైతే FEAT_EVAL
						 || eap->cstack->cs_idx >= 0
#అయితే_ముగింపు
						 );

    // ":source" read ex commands
    లేకపోతే ఒకవేళ (do_ఆధారం(fname, FALSE, DOSO_NONE, ఖాళీ) == FAIL)
	semsg(_(e_cant_open_file_str), fname);
}

/*
 * ":source {fname}"
 */
    శూన్య
ex_ఆధారం(exarg_T *eap)
{
#నిర్వచితమైతే FEAT_BROWSE
    ఒకవేళ (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	char_u *fname = ఖాళీ;

	fname = do_browse(0, (char_u *)_("Source Vim script"), eap->arg,
				      ఖాళీ, ఖాళీ,
				      (char_u *)_(BROWSE_FILTER_MACROS), ఖాళీ);
	ఒకవేళ (fname != ఖాళీ)
	{
	    cmd_ఆధారం(fname, eap);
	    vim_free(fname);
	}
    }
    లేకపోతే
#అయితే_ముగింపు
	cmd_ఆధారం(eap->arg, eap);
}

#ఒకవేళ defined(FEAT_EVAL)
/*
 * ":options"
 */
    శూన్య
ex_options(
    exarg_T	*eap వాడని)
{
    char_u  buf[500];
    పూర్ణాంకం	    multi_mods = 0;

    buf[0] = NUL;
    (శూన్య)కూడు_win_cmd_modifiers(buf, &cmdmod, &multi_mods);

    vim_setenv((char_u *)"OPTWIN_CMD", buf);
    cmd_ఆధారం((char_u *)SYS_OPTWIN_FILE, ఖాళీ);
}
#అయితే_ముగింపు

/*
 * ":source" and associated commands.
 */

#ఒకవేళ defined(FEAT_EVAL)
/*
 * Return the address holding the next breakpoint line for a source cookie.
 */
    linenr_T *
ఆధారం_breakpoint(శూన్య *cookie)
{
    తిరిగి &((ఆధారం_cookie_T *)cookie)->breakpoint;
}

/*
 * Return the address holding the debug tick for a source cookie.
 */
    పూర్ణాంకం *
ఆధారం_dbg_tick(శూన్య *cookie)
{
    తిరిగి &((ఆధారం_cookie_T *)cookie)->dbg_tick;
}

/*
 * Return the nesting level for a source cookie.
 */
    పూర్ణాంకం
ఆధారం_level(శూన్య *cookie)
{
    తిరిగి ((ఆధారం_cookie_T *)cookie)->level;
}

/*
 * Return the readahead line. Note that the pointer may become invalid when
 * getting the next line, if it's concatenated with the next one.
 */
    char_u *
ఆధారం_nextline(శూన్య *cookie)
{
    తిరిగి ((ఆధారం_cookie_T *)cookie)->nextline;
}
#అయితే_ముగింపు

#ఒకవేళ (defined(MSWIN) && defined(FEAT_CSCOPE)) || defined(HAVE_FD_CLOEXEC)
# define USE_FOPEN_NOINH
/*
 * Special function to open a file without handle inheritance.
 * When possible the handle is closed on exec().
 */
    స్థిర FILE *
fopen_noinh_readbin(అక్షరం *filename)
{
# ifdef MSWIN
    పూర్ణాంకం	fd_tmp = mch_open(filename, O_RDONLY | O_ద్విమాన | O_NOINHERIT, 0);
# లేకపోతే
    పూర్ణాంకం	fd_tmp = mch_open(filename, O_RDONLY, 0);
# endif

    ఒకవేళ (fd_tmp == -1)
	తిరిగి ఖాళీ;

# ifdef HAVE_FD_CLOEXEC
    {
	పూర్ణాంకం fdflags = fcntl(fd_tmp, F_GETFD);
	ఒకవేళ (fdflags >= 0 && (fdflags & FD_CLOEXEC) == 0)
	    (శూన్య)fcntl(fd_tmp, F_SETFD, fdflags | FD_CLOEXEC);
    }
# endif

    తిరిగి fdopen(fd_tmp, READBIN);
}
#అయితే_ముగింపు

/*
 * Initialization for sourcing lines from the current buffer. Reads all the
 * lines from the buffer and stores it in the cookie grow array.
 * Returns a pointer to the name ":source buffer=<n>" on success and NULL on
 * failure.
 */
    స్థిర char_u *
do_ఆధారం_బఫరు_init(ఆధారం_cookie_T *sp, exarg_T *eap)
{
    linenr_T	curr_lnum;
    char_u	*పంక్తి = ఖాళీ;
    char_u	*fname;

    ఒకవేళ (curbuf == ఖాళీ)
	తిరిగి ఖాళీ;

    // Use ":source buffer=<num>" as the script name
    ఒకవేళ (curbuf->b_ffname != ఖాళీ)
	fname = vim_strsave(curbuf->b_ffname);
    లేకపోతే
    {
	vim_snprintf((అక్షరం *)IObuff, IOSIZE, ":source buffer=%d", curbuf->b_fnum);
	fname = vim_strsave(IObuff);
    }
    ఒకవేళ (fname == ఖాళీ)
	తిరిగి ఖాళీ;

    ga_init2(&sp->buflines, పరిమాణం(char_u *), 100);

    // Copy the lines from the buffer into a grow array
    కొరకు (curr_lnum = eap->line1; curr_lnum <= eap->line2; curr_lnum++)
    {
	పంక్తి = vim_strsave(ml_get(curr_lnum));
	ఒకవేళ (పంక్తి == ఖాళీ)
	    వెళ్ళు errret;
	ఒకవేళ (ga_కూడు_string(&sp->buflines, పంక్తి) == FAIL)
	    వెళ్ళు errret;
    }
    sp->buf_lnum = 0;
    sp->ఆధారం_from_buf = TRUE;
    // When sourcing a range of lines from a buffer, use buffer line number.
    sp->sourcing_lnum = eap->line1 - 1;

    తిరిగి fname;

errret:
    vim_free(fname);
    vim_free(పంక్తి);
    ga_clear_అక్షరశ్రేణులు(&sp->buflines);
    తిరిగి ఖాళీ;
}

/*
 * Read the file "fname" and execute its lines as EX commands.
 * When "ret_sid" is not NULL and we loaded the script before, don't load it
 * again.
 *
 * The "eap" argument is used when sourcing lines from a buffer instead of a
 * file.
 *
 * If "clearvars" is TRUE, then for scripts which are loaded more than
 * once, clear all the functions and variables previously defined in that
 * script.
 *
 * This function may be called recursively!
 *
 * Return FAIL if file could not be opened, OK otherwise.
 * If a scriptitem_T was found or created "*ret_sid" is set to the SID.
 */
    స్థిర పూర్ణాంకం
do_ఆధారం_ext(
    char_u	*fname,
    పూర్ణాంకం		check_other,	    // check for .vimrc and _vimrc
    పూర్ణాంకం		is_vimrc,	    // DOSO_ value
    పూర్ణాంకం		*ret_sid వాడని,
    exarg_T	*eap,
    పూర్ణాంకం		clearvars వాడని)
{
    ఆధారం_cookie_T	    cookie;
    char_u		    *p;
    char_u		    *fname_not_fixed = ఖాళీ;
    char_u		    *fname_exp = ఖాళీ;
    char_u		    *firstline = ఖాళీ;
    పూర్ణాంకం			    retval = FAIL;
    sctx_T		    save_current_sctx;
#నిర్వచితమైతే STARTUPTIME
    నిర్మాణం timeval	    tv_rel;
    నిర్మాణం timeval	    tv_ప్రారంభం;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_PROFILE
    proftime_T		    wait_ప్రారంభం;
#అయితే_ముగింపు
    పూర్ణాంకం			    save_sticky_cmdmod_flags = sticky_cmdmod_flags;
    పూర్ణాంకం			    trigger_ఆధారం_post = FALSE;
#నిర్వచితమైతే FEAT_EVAL
    funccal_నమోదు_T	    funccalp_నమోదు;
    పూర్ణాంకం			    save_debug_break_level = debug_break_level;
    పూర్ణాంకం			    sid = -1;
    scriptitem_T	    *si = ఖాళీ;
    పూర్ణాంకం			    save_estack_compiling = estack_compiling;
    ESTACK_CHECK_DECLARATION;
#అయితే_ముగింపు

    CLEAR_క్షేత్రం(cookie);
    ఒకవేళ (fname == ఖాళీ)
    {
	// sourcing lines from a buffer
	fname_exp = do_ఆధారం_బఫరు_init(&cookie, eap);
	ఒకవేళ (fname_exp == ఖాళీ)
	    తిరిగి FAIL;
    }
    లేకపోతే
    {
	fname_not_fixed = expand_env_save(fname);
	ఒకవేళ (fname_not_fixed == ఖాళీ)
	    వెళ్ళు theend;
	fname_exp = fix_fname(fname_not_fixed);
	ఒకవేళ (fname_exp == ఖాళీ)
	    వెళ్ళు theend;
	ఒకవేళ (mch_isdir(fname_exp))
	{
	    smsg(_("Cannot source a directory: \"%s\""), fname);
	    వెళ్ళు theend;
	}
    }
#నిర్వచితమైతే FEAT_EVAL
    estack_compiling = FALSE;

    // See if we loaded this script before.
    sid = కనుగొను_script_by_name(fname_exp);
    ఒకవేళ (sid > 0 && ret_sid != ఖాళీ
			  && SCRIPT_అంశం(sid)->sn_state != SN_STATE_NOT_LOADED)
    {
	// Already loaded and no need to load again, return here.
	*ret_sid = sid;
	retval = OK;
	వెళ్ళు theend;
    }
#అయితే_ముగింపు

    // Apply SourceCmd autocommands, they should get the file and source it.
    // When sourcing a file "fname_exp" was made a full path name by
    // fix_fname() above, avoid expanding it again.
    ఒకవేళ ((fname == ఖాళీ
		? has_autocmd(EVENT_SOURCECMD, fname_exp, ఖాళీ)
		: has_autocmd_fullname(EVENT_SOURCECMD, fname_exp, ఖాళీ))
	    && apply_autocmds(EVENT_SOURCECMD, fname_exp, fname_exp,
							       FALSE, curbuf))
    {
#నిర్వచితమైతే FEAT_EVAL
	retval = aborting() ? FAIL : OK;
#లేకపోతే
	retval = OK;
#అయితే_ముగింపు
	ఒకవేళ (retval == OK)
	    // Apply SourcePost autocommands.
	    apply_autocmds(EVENT_SOURCEPOST, fname_exp, fname_exp,
								FALSE, curbuf);
	వెళ్ళు theend;
    }

    // Apply SourcePre autocommands, they may get the file.
    apply_autocmds(EVENT_SOURCEPRE, fname_exp, fname_exp, FALSE, curbuf);

    ఒకవేళ (!cookie.ఆధారం_from_buf)
    {
#నిర్వచితమైతే USE_FOPEN_NOINH
	cookie.fp = fopen_noinh_readbin((అక్షరం *)fname_exp);
#లేకపోతే
	cookie.fp = mch_fopen((అక్షరం *)fname_exp, READBIN);
#అయితే_ముగింపు
    }
    ఒకవేళ (cookie.fp == ఖాళీ && check_other)
    {
	// Try again, replacing file name ".vimrc" by "_vimrc" or vice versa,
	// and ".exrc" by "_exrc" or vice versa.
	p = gettail(fname_exp);
	ఒకవేళ ((*p == '.' || *p == '_')
		&& (STRICMP(p + 1, "vimrc") == 0
		    || STRICMP(p + 1, "gvimrc") == 0
		    || STRICMP(p + 1, "exrc") == 0))
	{
	    ఒకవేళ (*p == '_')
		*p = '.';
	    లేకపోతే
		*p = '_';
#నిర్వచితమైతే USE_FOPEN_NOINH
	    cookie.fp = fopen_noinh_readbin((అక్షరం *)fname_exp);
#లేకపోతే
	    cookie.fp = mch_fopen((అక్షరం *)fname_exp, READBIN);
#అయితే_ముగింపు
	}
    }

    ఒకవేళ (cookie.fp == ఖాళీ && !cookie.ఆధారం_from_buf)
    {
	ఒకవేళ (p_verbose > 0)
	{
	    verbose_enter();
	    ఒకవేళ (SOURCING_NAME == ఖాళీ)
		smsg(_("could not source \"%s\""), fname);
	    లేకపోతే
		smsg(_("line %ld: could not source \"%s\""),
							SOURCING_LNUM, fname);
	    verbose_leave();
	}
	వెళ్ళు theend;
    }

    // The file exists.
    // - In verbose mode, give a message.
    // - For a vimrc file, may want to set 'compatible', call vimrc_found().
    ఒకవేళ (p_verbose > 1)
    {
	verbose_enter();
	ఒకవేళ (SOURCING_NAME == ఖాళీ)
	    smsg(_("sourcing \"%s\""), fname);
	లేకపోతే
	    smsg(_("line %ld: sourcing \"%s\""), SOURCING_LNUM, fname);
	verbose_leave();
    }
    ఒకవేళ (is_vimrc == DOSO_VIMRC)
	vimrc_దొరికింది(fname_exp, (char_u *)"MYVIMRC");
    లేకపోతే ఒకవేళ (is_vimrc == DOSO_GVIMRC)
	vimrc_దొరికింది(fname_exp, (char_u *)"MYGVIMRC");

#నిర్వచితమైతే USE_CRNL
    // If no automatic file format: Set default to CR-NL.
    ఒకవేళ (*p_ffs == NUL)
	cookie.fileformat = EOL_DOS;
    లేకపోతే
	cookie.fileformat = EOL_UNKNOWN;
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_EVAL
    // Check if this script has a breakpoint.
    cookie.breakpoint = dbg_కనుగొను_breakpoint(TRUE, fname_exp, (linenr_T)0);
    cookie.fname = fname_exp;
    cookie.dbg_tick = debug_tick;

    cookie.level = ex_nesting_level;
#అయితే_ముగింపు

#నిర్వచితమైతే STARTUPTIME
    ఒకవేళ (సమయం_fd != ఖాళీ)
	సమయం_నెట్టు(&tv_rel, &tv_ప్రారంభం);
#అయితే_ముగింపు

    // "legacy" does not apply to commands in the script
    sticky_cmdmod_flags = 0;

    save_current_sctx = current_sctx;
    ఒకవేళ ((cmdmod.cmod_flags & CMOD_VIM9CMD) && cookie.ఆధారం_from_buf)
	// When the ":vim9cmd" command modifier is used, source buffer lines as
	// Vim9 script
	current_sctx.sc_సంస్కరణ = SCRIPT_సంస్కరణ_VIM9;
    లేకపోతే
	current_sctx.sc_సంస్కరణ = 1;  // default script version

#నిర్వచితమైతే FEAT_EVAL
    current_sctx.sc_lnum = 0;

# ifdef FEAT_PROFILE
    ఒకవేళ (do_profiling == PROF_YES)
	prof_బిడ్డ_enter(&wait_ప్రారంభం);		// entering a child now
# endif

    // Don't use local function variables, if called from a function.
    // Also starts profiling timer for nested script.
    save_funccal(&funccalp_నమోదు);

    // Reset "KeyTyped" to avoid some commands thinking they are invoked
    // interactively.  E.g. defining a function would output indent.
    పూర్ణాంకం save_కీలకం_Typed = కీలకం_Typed;
    కీలకం_Typed = FALSE;

    // Check if this script was sourced before to find its SID.
    // Always use a new sequence number.
    current_sctx.sc_seq = ++చివరి_current_SID_seq;
    ఒకవేళ (sid > 0)
    {
	hashtab_T	*ht;
	పూర్ణాంకం		todo;
	hashitem_T	*hi;
	dictitem_T	*di;

	// loading the same script again
	current_sctx.sc_sid = sid;
	si = SCRIPT_అంశం(sid);
	ఒకవేళ (si->sn_state == SN_STATE_NOT_LOADED)
	{
	    // this script was found but not loaded yet
	    si->sn_state = SN_STATE_NEW;
	}
	లేకపోతే
	{
	    si->sn_state = SN_STATE_RELOAD;

	    ఒకవేళ (!clearvars)
	    {
		// Script-local variables remain but "const" can be set again.
		// In Vim9 script variables will be cleared when "vim9script"
		// is encountered without the "noclear" argument.
		ht = &SCRIPT_VARS(sid);
		todo = (పూర్ణాంకం)ht->ht_వాడిన;
		FOR_ALL_HASHTAB_వస్తువులు(ht, hi, todo)
		    ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
		    {
			--todo;
			di = HI2DI(hi);
			di->di_flags |= DI_FLAGS_RELOAD;
		    }
		// imports can be redefined once
		mark_imports_for_reload(sid);
	    }
	    లేకపోతే
		clear_vim9_scriptlocal_vars(sid);

	    // reset version, "vim9script" may have been added or removed.
	    si->sn_సంస్కరణ = 1;
	}
	ఒకవేళ (ret_sid != ఖాళీ)
	    *ret_sid = sid;
    }
    లేకపోతే
    {
	పూర్ణాంకం error = OK;

	// It's new, generate a new SID and initialize the scriptitem.
	sid = get_new_scriptitem(&error);
	current_sctx.sc_sid = sid;
	ఒకవేళ (error == FAIL)
	    వెళ్ళు almosttheend;
	si = SCRIPT_అంశం(sid);
	si->sn_name = fname_exp;
	fname_exp = vim_strsave(si->sn_name);  // used for autocmd
	ఒకవేళ (ret_sid != ఖాళీ)
	    *ret_sid = sid;

	// Remember the "is_vimrc" flag for when the file is sourced again.
	si->sn_is_vimrc = is_vimrc;
    }

    // Keep the sourcing name/lnum, for recursive calls.
    estack_నెట్టు(ETYPE_SCRIPT, si->sn_name, 0);
    ESTACK_CHECK_అమరిక;

# ifdef FEAT_PROFILE
    ఒకవేళ (do_profiling == PROF_YES)
    {
	పూర్ణాంకం	forceit;

	// Check if we do profiling for this script.
	ఒకవేళ (!si->sn_prof_on && has_profiling(TRUE, si->sn_name, &forceit, ఖాళీ))
	{
	    script_do_profile(si);
	    si->sn_pr_force = forceit;
	}
	ఒకవేళ (si->sn_prof_on)
	{
	    ++si->sn_pr_లెక్క;
	    profile_ప్రారంభం(&si->sn_pr_ప్రారంభం);
	    profile_zero(&si->sn_pr_children);
	}
    }
# endif
#లేకపోతే
    // Keep the sourcing name/lnum, for recursive calls.
    estack_నెట్టు(ETYPE_SCRIPT, fname_exp, 0);
#అయితే_ముగింపు

    cookie.conv.vc_type = CONV_NONE;		// no conversion

    // Read the first line so we can check for a UTF-8 BOM.
    firstline = getsourceline(0, (శూన్య *)&cookie, 0, TRUE);
    ఒకవేళ (firstline != ఖాళీ && STRLEN(firstline) >= 3 && firstline[0] == 0xef
			      && firstline[1] == 0xbb && firstline[2] == 0xbf)
    {
	// Found BOM; setup conversion, skip over BOM and recode the line.
	convert_అమరిక(&cookie.conv, (char_u *)"utf-8", p_enc);
	p = string_convert(&cookie.conv, firstline + 3, ఖాళీ);
	ఒకవేళ (p == ఖాళీ)
	    p = vim_strsave(firstline + 3);
	ఒకవేళ (p != ఖాళీ)
	{
	    vim_free(firstline);
	    firstline = p;
	}
    }

    // Call do_cmdline, which will call getsourceline() to get the lines.
    do_cmdline(firstline, getsourceline, (శూన్య *)&cookie,
				     DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_REPEAT);
    retval = OK;

#నిర్వచితమైతే FEAT_PROFILE
    ఒకవేళ (do_profiling == PROF_YES)
    {
	// Get "si" again, "script_items" may have been reallocated.
	si = SCRIPT_అంశం(sid);
	ఒకవేళ (si->sn_prof_on)
	{
	    profile_చివర(&si->sn_pr_ప్రారంభం);
	    profile_sub_wait(&wait_ప్రారంభం, &si->sn_pr_ప్రారంభం);
	    profile_కూడు(&si->sn_pr_మొత్తం, &si->sn_pr_ప్రారంభం);
	    profile_self(&si->sn_pr_self, &si->sn_pr_ప్రారంభం,
							 &si->sn_pr_children);
	}
    }
#అయితే_ముగింపు

    ఒకవేళ (got_int)
	emsg(_(e_interrupted));
#నిర్వచితమైతే FEAT_EVAL
    ESTACK_CHECK_NOW;
#అయితే_ముగింపు
    estack_తీయి();
    ఒకవేళ (p_verbose > 1)
    {
	verbose_enter();
	smsg(_("finished sourcing %s"), fname);
	ఒకవేళ (SOURCING_NAME != ఖాళీ)
	    smsg(_("continuing in %s"), SOURCING_NAME);
	verbose_leave();
    }
#నిర్వచితమైతే STARTUPTIME
    ఒకవేళ (సమయం_fd != ఖాళీ)
    {
	vim_snprintf((అక్షరం *)IObuff, IOSIZE, "sourcing %s", fname);
	సమయం_సందేశం_2((అక్షరం *)IObuff, &tv_ప్రారంభం);
	సమయం_తీయి(&tv_rel);
    }
#అయితే_ముగింపు

    ఒకవేళ (!got_int)
	trigger_ఆధారం_post = TRUE;

#నిర్వచితమైతే FEAT_EVAL
    // After a "finish" in debug mode, need to break at first command of next
    // sourced file.
    ఒకవేళ (save_debug_break_level > ex_nesting_level
	    && debug_break_level == ex_nesting_level)
	++debug_break_level;
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_EVAL
almosttheend:
    // If "sn_save_cpo" is set that means we encountered "vim9script": restore
    // 'cpoptions', unless in the main .vimrc file.
    // Get "si" again, "script_items" may have been reallocated.
    si = SCRIPT_అంశం(sid);
    ఒకవేళ (si->sn_save_cpo != ఖాళీ && si->sn_is_vimrc == DOSO_NONE)
    {
	ఒకవేళ (STRCMP(p_cpo, CPO_VIM) != 0)
	{
	    char_u *f;
	    char_u *t;

	    // 'cpo' was changed in the script.  Apply the same change to the
	    // saved value, if possible.
	    కొరకు (f = (char_u *)CPO_VIM; *f != NUL; ++f)
		ఒకవేళ (vim_strchr(p_cpo, *f) == ఖాళీ
			&& (t = vim_strchr(si->sn_save_cpo, *f)) != ఖాళీ)
		    // flag was removed, also remove it from the saved 'cpo'
		    mch_memmove(t, t + 1, STRLEN(t));
	    కొరకు (f = p_cpo; *f != NUL; ++f)
		ఒకవేళ (vim_strchr((char_u *)CPO_VIM, *f) == ఖాళీ
			&& vim_strchr(si->sn_save_cpo, *f) == ఖాళీ)
		{
		    // flag was added, also add it to the saved 'cpo'
		    t = alloc(STRLEN(si->sn_save_cpo) + 2);
		    ఒకవేళ (t != ఖాళీ)
		    {
			*t = *f;
			STRCPY(t + 1, si->sn_save_cpo);
			vim_free(si->sn_save_cpo);
			si->sn_save_cpo = t;
		    }
		}
	}
	సమితి_option_విలువ_give_err((char_u *)"cpo",
					   0L, si->sn_save_cpo, OPT_NO_REDRAW);
    }
    VIM_CLEAR(si->sn_save_cpo);

    restore_funccal();
# ifdef FEAT_PROFILE
    ఒకవేళ (do_profiling == PROF_YES)
	prof_బిడ్డ_exit(&wait_ప్రారంభం);		// leaving a child now
# endif

    కీలకం_Typed = save_కీలకం_Typed;
#అయితే_ముగింపు
    current_sctx = save_current_sctx;

    ఒకవేళ (cookie.fp != ఖాళీ)
	fclose(cookie.fp);
    ఒకవేళ (cookie.ఆధారం_from_buf)
	ga_clear_అక్షరశ్రేణులు(&cookie.buflines);
    vim_free(cookie.nextline);
    vim_free(firstline);
    convert_అమరిక(&cookie.conv, ఖాళీ, ఖాళీ);

    ఒకవేళ (trigger_ఆధారం_post)
	apply_autocmds(EVENT_SOURCEPOST, fname_exp, fname_exp, FALSE, curbuf);

theend:
#నిర్వచితమైతే FEAT_EVAL
    ఒకవేళ (sid > 0 && ret_sid != ఖాళీ
	    && fname_not_fixed != ఖాళీ && fname_exp != ఖాళీ)
    {
	పూర్ణాంకం not_fixed_sid = కనుగొను_script_by_name(fname_not_fixed);

	// If "fname_not_fixed" is a symlink then we source the linked file.
	// If the original name is in the script list we add the ID of the
	// script that was actually sourced.
	ఒకవేళ (SCRIPT_ID_VALID(not_fixed_sid) && not_fixed_sid != sid)
	    SCRIPT_అంశం(not_fixed_sid)->sn_sourced_sid = sid;
    }
#అయితే_ముగింపు

    vim_free(fname_not_fixed);
    vim_free(fname_exp);
    sticky_cmdmod_flags = save_sticky_cmdmod_flags;
#నిర్వచితమైతే FEAT_EVAL
    estack_compiling = save_estack_compiling;
#అయితే_ముగింపు
    తిరిగి retval;
}

    పూర్ణాంకం
do_ఆధారం(
    char_u	*fname,
    పూర్ణాంకం		check_other,	    // check for .vimrc and _vimrc
    పూర్ణాంకం		is_vimrc,	    // DOSO_ value
    పూర్ణాంకం		*ret_sid)
{
    తిరిగి do_ఆధారం_ext(fname, check_other, is_vimrc, ret_sid, ఖాళీ, FALSE);
}


#ఒకవేళ defined(FEAT_EVAL)

/*
 * ":scriptnames"
 */
    శూన్య
ex_scriptnames(exarg_T *eap)
{
    పూర్ణాంకం i;

    ఒకవేళ (eap->addr_లెక్క > 0 || *eap->arg != NUL)
    {
	// :script {scriptId}: edit the script
	ఒకవేళ (eap->addr_లెక్క > 0 && !SCRIPT_ID_VALID(eap->line2))
	    emsg(_(e_invalid_వాదం));
	లేకపోతే
	{
	    ఒకవేళ (eap->addr_లెక్క > 0)
		eap->arg = SCRIPT_అంశం(eap->line2)->sn_name;
	    లేకపోతే
	    {
		expand_env(eap->arg, NameBuff, MAXPATHL);
		eap->arg = NameBuff;
	    }
	    do_exedit(eap, ఖాళీ);
	}
	తిరిగి;
    }

    కొరకు (i = 1; i <= script_వస్తువులు.ga_len && !got_int; ++i)
    {
	scriptitem_T *si = SCRIPT_అంశం(i);

	ఒకవేళ (si->sn_name != ఖాళీ)
	{
	    అక్షరం sourced_buf[20];

	    home_replace(ఖాళీ, si->sn_name, NameBuff, MAXPATHL, TRUE);
	    ఒకవేళ (si->sn_sourced_sid > 0)
		vim_snprintf(sourced_buf, 20, "->%d", si->sn_sourced_sid);
	    లేకపోతే
		sourced_buf[0] = NUL;
	    vim_snprintf((అక్షరం *)IObuff, IOSIZE, "%3d%s%s: %s",
		    i,
		    sourced_buf,
		    si->sn_state == SN_STATE_NOT_LOADED ? " A" : "",
		    NameBuff);
	    ఒకవేళ (!సందేశం_filtered(IObuff))
	    {
		సందేశం_putchar('\n');
		సందేశం_outtrans(IObuff);
		బహిర్గతం_flush();	    // output one line at a time
		ui_breakcheck();
	    }
	}
    }
}

# ఒకవేళ defined(BACKSLASH_లోపల_FILENAME)
/*
 * Fix slashes in the list of script names for 'shellslash'.
 */
    శూన్య
scriptnames_slash_adjust(శూన్య)
{
    పూర్ణాంకం i;

    కొరకు (i = 1; i <= script_వస్తువులు.ga_len; ++i)
	ఒకవేళ (SCRIPT_అంశం(i)->sn_name != ఖాళీ)
	    slash_adjust(SCRIPT_అంశం(i)->sn_name);
}
# endif

/*
 * Get a pointer to a script name.  Used for ":verbose set".
 * Message appended to "Last set from "
 */
    char_u *
get_scriptname(scid_T id)
{
    ఒకవేళ (id == SID_MODELINE)
	తిరిగి (char_u *)_("modeline");
    ఒకవేళ (id == SID_CMDARG)
	తిరిగి (char_u *)_("--cmd argument");
    ఒకవేళ (id == SID_CARG)
	తిరిగి (char_u *)_("-c argument");
    ఒకవేళ (id == SID_ENV)
	తిరిగి (char_u *)_("environment variable");
    ఒకవేళ (id == SID_ERROR)
	తిరిగి (char_u *)_("error handler");
    ఒకవేళ (id == SID_WINLAYOUT)
	తిరిగి (char_u *)_("changed window size");
    తిరిగి SCRIPT_అంశం(id)->sn_name;
}

# ఒకవేళ defined(EXITFREE)
    శూన్య
free_scriptnames(శూన్య)
{
    పూర్ణాంకం			i;

    కొరకు (i = script_వస్తువులు.ga_len; i > 0; --i)
    {
	scriptitem_T *si = SCRIPT_అంశం(i);

	// the variables themselves are cleared in evalvars_clear()
	vim_free(si->sn_vars);

	vim_free(si->sn_name);
	free_imports_and_script_vars(i);
	free_string_option(si->sn_save_cpo);
#  ifdef FEAT_PROFILE
	ga_clear(&si->sn_prl_ga);
#  endif
	vim_free(si->sn_autoload_ఉపసర్గ);
	vim_free(si);
    }
    ga_clear(&script_వస్తువులు);
}

    శూన్య
free_autoload_scriptnames(శూన్య)
{
    ga_clear_అక్షరశ్రేణులు(&ga_loaded);
}
# endif

    linenr_T
get_sourced_lnum(
	char_u *(*fgetline)(పూర్ణాంకం, శూన్య *, పూర్ణాంకం, getline_opt_T),
	శూన్య *cookie)
{
    తిరిగి fgetline == getsourceline
			? ((ఆధారం_cookie_T *)cookie)->sourcing_lnum
			: SOURCING_LNUM;
}

/*
 * Return a List of script-local functions defined in the script with id
 * 'sid'.
 */
    స్థిర list_T *
get_script_local_funcs(scid_T sid)
{
    hashtab_T	*functbl;
    hashitem_T	*hi;
    long_u	todo;
    list_T	*l;

    l = list_alloc();
    ఒకవేళ (l == ఖాళీ)
	తిరిగి ఖాళీ;

    // Iterate through all the functions in the global function hash table
    // looking for functions with script ID 'sid'.
    functbl = func_tbl_get();
    todo = functbl->ht_వాడిన;
    FOR_ALL_HASHTAB_వస్తువులు(functbl, hi, todo)
    {
	ufunc_T	*fp;

	ఒకవేళ (HASHITEM_ఖాళీలు(hi))
	    కొనసాగు;

	--todo;
	fp = HI2UF(hi);

	// Add active functions with script id == 'sid'
	ఒకవేళ (!(fp->uf_flags & FC_DEAD) && (fp->uf_script_ctx.sc_sid == sid))
	{
	    char_u	*name;

	    ఒకవేళ (fp->uf_name_exp != ఖాళీ)
		name = fp->uf_name_exp;
	    లేకపోతే
		name = fp->uf_name;

	    list_append_string(l, name, -1);
	}
    }

    తిరిగి l;
}

/*
 * getscriptinfo() function
 */
    శూన్య
f_getscriptinfo(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    char_u	*pat = ఖాళీ;
    regmatch_T	regmatch;
    పూర్ణాంకం		filterpat = FALSE;
    scid_T	sid = -1;

    ఒకవేళ (rettv_list_alloc(rettv) == FAIL)
	తిరిగి;

    ఒకవేళ (check_for_opt_dict_arg(argvars, 0) == FAIL)
	తిరిగి;

    l = rettv->vval.v_list;

    regmatch.regprog = ఖాళీ;
    regmatch.rm_ic = p_ic;

    ఒకవేళ (argvars[0].v_type == VAR_DICT)
    {
	dictitem_T *sid_di = dict_కనుగొను(argvars[0].vval.v_dict,
							   (char_u *)"sid", 3);
	ఒకవేళ (sid_di != ఖాళీ)
	{
	    పూర్ణాంకం error = FALSE;
	    sid = tv_get_సంఖ్య_chk(&sid_di->di_tv, &error);
	    ఒకవేళ (error)
		తిరిగి;
	    ఒకవేళ (sid <= 0)
	    {
		semsg(_(e_invalid_విలువ_for_వాదం_str_str), "sid",
						tv_get_string(&sid_di->di_tv));
		తిరిగి;
	    }
	}
	లేకపోతే
	{
	    pat = dict_get_string(argvars[0].vval.v_dict, "name", TRUE);
	    ఒకవేళ (pat != ఖాళీ)
		regmatch.regprog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
	    ఒకవేళ (regmatch.regprog != ఖాళీ)
		filterpat = TRUE;
	}
    }

    కొరకు (varnumber_T i = sid > 0 ? sid : 1;
		       (i == sid || sid <= 0) && i <= script_వస్తువులు.ga_len; ++i)
    {
	scriptitem_T	*si = SCRIPT_అంశం(i);
	dict_T		*d;

	ఒకవేళ (si->sn_name == ఖాళీ)
	    కొనసాగు;

	ఒకవేళ (filterpat && !vim_regexec(&regmatch, si->sn_name, (colnr_T)0))
	    కొనసాగు;

	ఒకవేళ ((d = dict_alloc()) == ఖాళీ
		|| list_append_dict(l, d) == FAIL)
	{
	    dict_unref(d);
	    వెళ్ళు theend;
	}
	ఒకవేళ (dict_కూడు_string(d, "name", si->sn_name) == FAIL
		|| dict_కూడు_సంఖ్య(d, "sid", i) == FAIL
		|| dict_కూడు_సంఖ్య(d, "sourced", si->sn_sourced_sid) == FAIL
		|| dict_కూడు_సంఖ్య(d, "version", si->sn_సంస్కరణ) == FAIL
		|| dict_కూడు_bool(d, "autoload",
				si->sn_state == SN_STATE_NOT_LOADED) == FAIL)
	    వెళ్ళు theend;

	// When a script ID is specified, return information about only the
	// specified script, and add the script-local variables and functions.
	ఒకవేళ (sid > 0)
	{
	    dict_T	*var_dict;
	    list_T	*fn_list;

	    var_dict = dict_copy(&si->sn_vars->sv_dict, TRUE, TRUE,
								get_copyID());
	    ఒకవేళ (var_dict == ఖాళీ)
		వెళ్ళు theend;
	    ఒకవేళ (dict_కూడు_dict(d, "variables", var_dict) == FAIL)
	    {
		dict_unref(var_dict);
		వెళ్ళు theend;
	    }
	    --var_dict->dv_refcount;
	    fn_list = get_script_local_funcs(sid);
	    ఒకవేళ (fn_list == ఖాళీ || dict_కూడు_list(d, "functions", fn_list) == FAIL)
	    {
		list_unref(fn_list);
		వెళ్ళు theend;
	    }
	}
    }

theend:
    vim_regfree(regmatch.regprog);
    vim_free(pat);
}

#అయితే_ముగింపు

    స్థిర char_u *
get_one_sourceline(ఆధారం_cookie_T *sp)
{
    garray_T		ga;
    పూర్ణాంకం			len;
    పూర్ణాంకం			c;
    char_u		*buf;
#నిర్వచితమైతే USE_CRNL
    పూర్ణాంకం			has_cr;		// CR-LF found
#అయితే_ముగింపు
    పూర్ణాంకం			have_read = FALSE;

    // use a growarray to store the sourced line
    ga_init2(&ga, 1, 250);

    // Loop until there is a finished line (or end-of-file).
    ++sp->sourcing_lnum;
    కొరకు (;;)
    {
	// make room to read at least 120 (more) characters
	ఒకవేళ (ga_grow(&ga, 120) == FAIL)
	    ఆపు;
	ఒకవేళ (sp->ఆధారం_from_buf)
	{
	    ఒకవేళ (sp->buf_lnum >= sp->buflines.ga_len)
		ఆపు;		    // all the lines are processed
	    ga_concat(&ga, ((char_u **)sp->buflines.ga_దత్తాంశం)[sp->buf_lnum]);
	    sp->buf_lnum++;
	    ఒకవేళ (ga_grow(&ga, 1) == FAIL)
		ఆపు;
	    buf = (char_u *)ga.ga_దత్తాంశం;
	    buf[ga.ga_len++] = NUL;
	    len = ga.ga_len;
	}
	లేకపోతే
	{
	    buf = (char_u *)ga.ga_దత్తాంశం;
	    ఒకవేళ (fgets((అక్షరం *)buf + ga.ga_len, ga.ga_maxlen - ga.ga_len,
			sp->fp) == ఖాళీ)
		ఆపు;
	    len = ga.ga_len + (పూర్ణాంకం)STRLEN(buf + ga.ga_len);
	}
#నిర్వచితమైతే USE_CRNL
	// Ignore a trailing CTRL-Z, when in Dos mode.	Only recognize the
	// CTRL-Z by its own, or after a NL.
	ఒకవేళ (	   (len == 1 || (len >= 2 && buf[len - 2] == '\n'))
		&& sp->fileformat == EOL_DOS
		&& buf[len - 1] == Ctrl_Z)
	{
	    buf[len - 1] = NUL;
	    ఆపు;
	}
#అయితే_ముగింపు

	have_read = TRUE;
	ga.ga_len = len;

	// If the line was longer than the buffer, read more.  A line of a
	// buffer was taken as a whole.
	ఒకవేళ (!sp->ఆధారం_from_buf && ga.ga_maxlen - ga.ga_len == 1
						       && buf[len - 1] != '\n')
	    కొనసాగు;

	ఒకవేళ (len >= 1 && buf[len - 1] == '\n')	// remove trailing NL
	{
#నిర్వచితమైతే USE_CRNL
	    has_cr = (len >= 2 && buf[len - 2] == '\r');
	    ఒకవేళ (sp->fileformat == EOL_UNKNOWN)
	    {
		ఒకవేళ (has_cr)
		    sp->fileformat = EOL_DOS;
		లేకపోతే
		    sp->fileformat = EOL_UNIX;
	    }

	    ఒకవేళ (sp->fileformat == EOL_DOS)
	    {
		ఒకవేళ (has_cr)	    // replace trailing CR
		{
		    buf[len - 2] = '\n';
		    --len;
		    --ga.ga_len;
		}
		లేకపోతే	    // lines like ":map xx yy^M" will have failed
		{
		    ఒకవేళ (!sp->error)
		    {
			సందేశం_ఆధారం(HL_ATTR(HLF_W));
			emsg(_("W15: Warning: Wrong line separator, ^M may be missing"));
		    }
		    sp->error = TRUE;
		    sp->fileformat = EOL_UNIX;
		}
	    }
#అయితే_ముగింపు
	    // The '\n' is escaped if there is an odd number of ^V's just
	    // before it, first set "c" just before the 'V's and then check
	    // len&c parities (is faster than ((len-c)%2 == 0)) -- Acevedo
	    కొరకు (c = len - 2; c >= 0 && buf[c] == Ctrl_V; c--)
		;
	    ఒకవేళ ((len & 1) != (c & 1))	// escaped NL, read more
	    {
		++sp->sourcing_lnum;
		కొనసాగు;
	    }

	    buf[len - 1] = NUL;		// remove the NL
	}

	// Check for ^C here now and then, so recursive :so can be broken.
	పంక్తి_breakcheck();
	ఆపు;
    }

    ఒకవేళ (have_read)
	తిరిగి (char_u *)ga.ga_దత్తాంశం;

    vim_free(ga.ga_దత్తాంశం);
    తిరిగి ఖాళీ;
}

/*
 * Get one full line from a sourced file.
 * Called by do_cmdline() when it's called from do_source().
 *
 * Return a pointer to the line in allocated memory.
 * Return NULL for end-of-file or some error.
 */
    char_u *
getsourceline(
	పూర్ణాంకం c వాడని,
	శూన్య *cookie,
	పూర్ణాంకం indent వాడని,
	getline_opt_T options)
{
    ఆధారం_cookie_T	*sp = (ఆధారం_cookie_T *)cookie;
    char_u		*పంక్తి;
    char_u		*p;
    పూర్ణాంకం			do_vim9_all = లోపల_vim9script()
					      && options == GETLINE_CONCAT_ALL;
    పూర్ణాంకం			do_bar_cont = do_vim9_all
					 || options == GETLINE_CONCAT_CONTBAR;

#నిర్వచితమైతే FEAT_EVAL
    // If breakpoints have been added/deleted need to check for it.
    ఒకవేళ ((sp->dbg_tick < debug_tick) && !sp->ఆధారం_from_buf)
    {
	sp->breakpoint = dbg_కనుగొను_breakpoint(TRUE, sp->fname, SOURCING_LNUM);
	sp->dbg_tick = debug_tick;
    }
# ifdef FEAT_PROFILE
    ఒకవేళ (do_profiling == PROF_YES)
	script_పంక్తి_చివర();
# endif
#అయితే_ముగింపు

    // Set the current sourcing line number.
    SOURCING_LNUM = sp->sourcing_lnum + 1;

    // Get current line.  If there is a read-ahead line, use it, otherwise get
    // one now.  "fp" is NULL if actually using a string.
    ఒకవేళ (sp->ముగిసిన || (!sp->ఆధారం_from_buf && sp->fp == ఖాళీ))
	పంక్తి = ఖాళీ;
    లేకపోతే ఒకవేళ (sp->nextline == ఖాళీ)
	పంక్తి = get_one_sourceline(sp);
    లేకపోతే
    {
	పంక్తి = sp->nextline;
	sp->nextline = ఖాళీ;
	++sp->sourcing_lnum;
    }
#నిర్వచితమైతే FEAT_PROFILE
    ఒకవేళ (పంక్తి != ఖాళీ && do_profiling == PROF_YES)
	script_పంక్తి_ప్రారంభం();
#అయితే_ముగింపు

    // Only concatenate lines starting with a \ when 'cpoptions' doesn't
    // contain the 'C' flag.
    ఒకవేళ (పంక్తి != ఖాళీ && options != GETLINE_NONE
				      && vim_strchr(p_cpo, CPO_CONCAT) == ఖాళీ)
    {
	పూర్ణాంకం comment_char = లోపల_vim9script() ? '#' : '"';

	// compensate for the one line read-ahead
	--sp->sourcing_lnum;

	// Get the next line and concatenate it when it starts with a
	// backslash. We always need to read the next line, keep it in
	// sp->nextline.
	/* Also check for a comment in between continuation lines: "\ */
	// Also check for a Vim9 comment, empty line, line starting with '|',
	// but not "||".
	sp->nextline = get_one_sourceline(sp);
	ఒకవేళ (sp->nextline != ఖాళీ
		&& (*(p = skipwhite(sp->nextline)) == '\\'
			      || (p[0] == comment_char
						&& p[1] == '\\' && p[2] == ' ')
			      || (do_vim9_all && (*p == NUL
						     || vim9_comment_ప్రారంభం(p)))
			      || (do_bar_cont && p[0] == '|' && p[1] != '|')))
	{
	    garray_T    ga;

	    ga_init2(&ga, పరిమాణం(char_u), 400);
	    ga_concat(&ga, పంక్తి);
	    ఒకవేళ (*p == '\\')
		ga_concat(&ga, p + 1);
	    లేకపోతే ఒకవేళ (*p == '|')
	    {
		GA_CONCAT_LITERAL(&ga, " ");
		ga_concat(&ga, p);
	    }
	    కొరకు (;;)
	    {
		vim_free(sp->nextline);
		sp->nextline = get_one_sourceline(sp);
		ఒకవేళ (sp->nextline == ఖాళీ)
		    ఆపు;
		p = skipwhite(sp->nextline);
		ఒకవేళ (*p == '\\' || (do_bar_cont && p[0] == '|' && p[1] != '|'))
		{
		    // Adjust the growsize to the current length to speed up
		    // concatenating many lines.
		    ఒకవేళ (ga.ga_len > 400)
		    {
			ఒకవేళ (ga.ga_len > 8000)
			    ga.ga_growsize = 8000;
			లేకపోతే
			    ga.ga_growsize = ga.ga_len;
		    }
		    ఒకవేళ (*p == '\\')
			ga_concat(&ga, p + 1);
		    లేకపోతే
		    {
			GA_CONCAT_LITERAL(&ga, " ");
			ga_concat(&ga, p);
		    }
		}
		లేకపోతే ఒకవేళ (!(p[0] == (comment_char)
						&& p[1] == '\\' && p[2] == ' ')
		     && !(do_vim9_all && (*p == NUL || vim9_comment_ప్రారంభం(p))))
		    ఆపు;
		/* drop a # comment or "\ comment line */
	    }
	    ga_append(&ga, NUL);
	    vim_free(పంక్తి);
	    పంక్తి = ga.ga_దత్తాంశం;
	}
    }

    ఒకవేళ (పంక్తి != ఖాళీ && sp->conv.vc_type != CONV_NONE)
    {
	char_u	*s;

	// Convert the encoding of the script line.
	s = string_convert(&sp->conv, పంక్తి, ఖాళీ);
	ఒకవేళ (s != ఖాళీ)
	{
	    vim_free(పంక్తి);
	    పంక్తి = s;
	}
    }

#నిర్వచితమైతే FEAT_EVAL
    // Did we encounter a breakpoint?
    ఒకవేళ (!sp->ఆధారం_from_buf && sp->breakpoint != 0
	    && sp->breakpoint <= SOURCING_LNUM)
    {
	dbg_breakpoint(sp->fname, SOURCING_LNUM);
	// Find next breakpoint.
	sp->breakpoint = dbg_కనుగొను_breakpoint(TRUE, sp->fname, SOURCING_LNUM);
	sp->dbg_tick = debug_tick;
    }
#అయితే_ముగింపు

    తిరిగి పంక్తి;
}

/*
 * Returns TRUE if sourcing a script either from a file or a buffer.
 * Otherwise returns FALSE.
 */
    పూర్ణాంకం
sourcing_a_script(exarg_T *eap)
{
    తిరిగి (getline_equal(eap->ea_getline, eap->cookie, getsourceline));
}

/*
 * ":scriptencoding": Set encoding conversion for a sourced script.
 */
    శూన్య
ex_scriptencoding(exarg_T *eap)
{
    ఆధారం_cookie_T	*sp;
    char_u		*name;

    ఒకవేళ (!sourcing_a_script(eap))
    {
	emsg(_(e_scriptencoding_వాడిన_outside_యొక్క_sourced_file));
	తిరిగి;
    }

    ఒకవేళ (*eap->arg != NUL)
    {
	name = enc_canonize(eap->arg);
	ఒకవేళ (name == ఖాళీ)	// out of memory
	    తిరిగి;
    }
    లేకపోతే
	name = eap->arg;

    // Setup for conversion from the specified encoding to 'encoding'.
    sp = (ఆధారం_cookie_T *)getline_cookie(eap->ea_getline, eap->cookie);
    convert_అమరిక(&sp->conv, name, p_enc);

    ఒకవేళ (name != eap->arg)
	vim_free(name);
}

/*
 * ":scriptversion": Set Vim script version for a sourced script.
 */
    శూన్య
ex_scriptversion(exarg_T *eap వాడని)
{
    పూర్ణాంకం		nr;

    ఒకవేళ (!sourcing_a_script(eap))
    {
	emsg(_(e_scriptversion_వాడిన_outside_యొక్క_sourced_file));
	తిరిగి;
    }
    ఒకవేళ (లోపల_vim9script())
    {
	emsg(_(e_cannot_use_scriptversion_after_vim9script));
	తిరిగి;
    }

    nr = getdigits(&eap->arg);
    ఒకవేళ (nr == 0 || *eap->arg != NUL)
	emsg(_(e_invalid_వాదం));
    లేకపోతే ఒకవేళ (nr > SCRIPT_సంస్కరణ_MAX)
	semsg(_(e_scriptversion_not_supported_nr), nr);
    లేకపోతే
    {
	current_sctx.sc_సంస్కరణ = nr;
#నిర్వచితమైతే FEAT_EVAL
	SCRIPT_అంశం(current_sctx.sc_sid)->sn_సంస్కరణ = nr;
#అయితే_ముగింపు
    }
}

#ఒకవేళ defined(FEAT_EVAL)
/*
 * ":finish": Mark a sourced file as finished.
 */
    శూన్య
ex_ముగింపు(exarg_T *eap)
{
    ఒకవేళ (sourcing_a_script(eap))
	do_ముగింపు(eap, FALSE);
    లేకపోతే
	emsg(_(e_ముగింపు_వాడిన_outside_యొక్క_sourced_file));
}

/*
 * Mark a sourced file as finished.  Possibly makes the ":finish" pending.
 * Also called for a pending finish at the ":endtry" or after returning from
 * an extra do_cmdline().  "reanimate" is used in the latter case.
 */
    శూన్య
do_ముగింపు(exarg_T *eap, పూర్ణాంకం reanimate)
{
    పూర్ణాంకం		idx;

    ఒకవేళ (reanimate)
	((ఆధారం_cookie_T *)getline_cookie(eap->ea_getline,
					      eap->cookie))->ముగిసిన = FALSE;

    // Cleanup (and inactivate) conditionals, but stop when a try conditional
    // not in its finally clause (which then is to be executed next) is found.
    // In this case, make the ":finish" pending for execution at the ":endtry".
    // Otherwise, finish normally.
    idx = cleanup_conditionals(eap->cstack, 0, TRUE);
    ఒకవేళ (idx >= 0)
    {
	eap->cstack->cs_pending[idx] = CSTP_ముగింపు;
	report_make_pending(CSTP_ముగింపు, ఖాళీ);
    }
    లేకపోతే
	((ఆధారం_cookie_T *)getline_cookie(eap->ea_getline,
					       eap->cookie))->ముగిసిన = TRUE;
}


/*
 * Return TRUE when a sourced file had the ":finish" command: Don't give error
 * message for missing ":endif".
 * Return FALSE when not sourcing a file.
 */
    పూర్ణాంకం
ఆధారం_ముగిసిన(
    char_u	*(*fgetline)(పూర్ణాంకం, శూన్య *, పూర్ణాంకం, getline_opt_T),
    శూన్య	*cookie)
{
    తిరిగి (getline_equal(fgetline, cookie, getsourceline)
	    && ((ఆధారం_cookie_T *)getline_cookie(
						fgetline, cookie))->ముగిసిన);
}

/*
 * Find the path of a script below the "autoload" directory.
 * Returns NULL if there is no "/autoload/" in the script name.
 */
    స్థిర char_u *
script_name_after_autoload(scriptitem_T *si)
{
    char_u	*p = si->sn_name;
    char_u	*res = ఖాళీ;

    కొరకు (;;)
    {
	char_u *n = (char_u *)అక్షరశ్రేణి_శోధన((అక్షరం *)p, "autoload");

	ఒకవేళ (n == ఖాళీ)
	    ఆపు;
	ఒకవేళ (n > p && vim_ispathsep(n[-1]) && vim_ispathsep(n[8]))
	    res = n + 9;
	p = n + 8;
    }
    తిరిగి res;
}

/*
 * For an autoload script "autoload/dir/script.vim" return the prefix
 * "dir#script#" in allocated memory.
 * Returns NULL if anything is wrong.
 */
    char_u *
get_autoload_ఉపసర్గ(scriptitem_T *si)
{
    char_u *p = script_name_after_autoload(si);
    char_u *ఉపసర్గ;

    ఒకవేళ (p == ఖాళీ)
	తిరిగి ఖాళీ;
    ఉపసర్గ = vim_strsave(p);
    ఒకవేళ (ఉపసర్గ == ఖాళీ)
	తిరిగి ఖాళీ;

    // replace all '/' with '#' and locate ".vim" at the end
    కొరకు (p = ఉపసర్గ; *p != NUL; p += mb_ptr2len(p))
    {
	ఒకవేళ (vim_ispathsep(*p))
	    *p = '#';
	లేకపోతే ఒకవేళ (STRCMP(p, ".vim") == 0)
	{
	    p[0] = '#';
	    p[1] = NUL;
	    తిరిగి ఉపసర్గ;
	}
    }

    // did not find ".vim" at the end
    vim_free(ఉపసర్గ);
    తిరిగి ఖాళీ;
}

/*
 * If in a Vim9 autoload script return "name" with the autoload prefix for the
 * script.  If successful the returned name is allocated.
 * Otherwise it returns "name" unmodified.
 */
    char_u *
may_ఉపసర్గ_autoload(char_u *name)
{
    ఒకవేళ (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	తిరిగి name;

    scriptitem_T *si = SCRIPT_అంశం(current_sctx.sc_sid);

    ఒకవేళ (si->sn_autoload_ఉపసర్గ == ఖాళీ)
	తిరిగి name;

    char_u  *basename = name;
    size_t  len;
    char_u  *res;

    ఒకవేళ (*name == K_SPECIAL)
    {
	char_u *p = vim_strchr(name, '_');

	// skip over "<SNR>99_"
	ఒకవేళ (p != ఖాళీ)
	    basename = p + 1;
    }

    len = STRLEN(si->sn_autoload_ఉపసర్గ) + STRLEN(basename) + 2;
    res = alloc(len);
    ఒకవేళ (res == ఖాళీ)
	తిరిగి ఖాళీ;

    vim_snprintf((అక్షరం *)res, len, "%s%s", si->sn_autoload_ఉపసర్గ, basename);
    తిరిగి res;
}

/*
 * Return the autoload script name for a function or variable name.
 * Returns NULL when out of memory.
 * Caller must make sure that "name" contains AUTOLOAD_CHAR.
 */
    char_u *
autoload_name(char_u *name)
{
    char_u	*p, *q = ఖాళీ;
    char_u	*scriptname;

    // Get the script file name: replace '#' with '/', append ".vim".
    scriptname = alloc(STRLEN(name) + 14);
    ఒకవేళ (scriptname == ఖాళీ)
	తిరిగి ఖాళీ;
    STRCPY(scriptname, "autoload/");
    STRCAT(scriptname, name[0] == 'g' && name[1] == ':' ? name + 2: name);
    కొరకు (p = scriptname + 9; (p = vim_strchr(p, AUTOLOAD_CHAR)) != ఖాళీ;
								    q = p, ++p)
	*p = '/';
    STRCPY(q, ".vim");
    తిరిగి scriptname;
}

/*
 * If "name" has a package name try autoloading the script for it.
 * Return TRUE if a package was loaded.
 */
    పూర్ణాంకం
script_autoload(
    char_u	*name,
    పూర్ణాంకం		reload)	    // load script again when already loaded
{
    char_u	*p;
    char_u	*scriptname, *tofree;
    పూర్ణాంకం		ret = FALSE;
    పూర్ణాంకం		i;
    పూర్ణాంకం		ret_sid;

    // If the name starts with "<SNR>123_" then "123" is the script ID.
    ఒకవేళ (name[0] == K_SPECIAL && name[1] == KS_EXTRA && name[2] == KE_SNR)
    {
	p = name + 3;
	ret_sid = (పూర్ణాంకం)getdigits(&p);
	ఒకవేళ (*p == '_' && SCRIPT_ID_VALID(ret_sid))
	{
	    may_ఎక్కించు_script(ret_sid, &ret);
	    తిరిగి ret;
	}
    }

    // If there is no '#' after name[0] there is no package name.
    p = vim_strchr(name, AUTOLOAD_CHAR);
    ఒకవేళ (p == ఖాళీ || p == name)
	తిరిగి FALSE;

    tofree = scriptname = autoload_name(name);
    ఒకవేళ (scriptname == ఖాళీ)
	తిరిగి FALSE;

    // Find the name in the list of previously loaded package names.  Skip
    // "autoload/", it's always the same.
    కొరకు (i = 0; i < ga_loaded.ga_len; ++i)
	ఒకవేళ (STRCMP(((char_u **)ga_loaded.ga_దత్తాంశం)[i] + 9, scriptname + 9) == 0)
	    ఆపు;
    ఒకవేళ (!reload && i < ga_loaded.ga_len)
	ret = FALSE;	    // was loaded already
    లేకపోతే
    {
	// Remember the name if it wasn't loaded already.
	ఒకవేళ (i == ga_loaded.ga_len && ga_grow(&ga_loaded, 1) == OK)
	{
	    ((char_u **)ga_loaded.ga_దత్తాంశం)[ga_loaded.ga_len++] = scriptname;
	    tofree = ఖాళీ;
	}

	// Try loading the package from $VIMRUNTIME/autoload/<name>.vim
	// Use "ret_sid" to avoid loading the same script again.
	ఒకవేళ (ఆధారం_లోపల_మార్గం(p_rtp, scriptname, DIP_ప్రారంభం, &ret_sid) == OK)
	    ret = TRUE;
    }

    vim_free(tofree);
    తిరిగి ret;
}
#అయితే_ముగింపు
