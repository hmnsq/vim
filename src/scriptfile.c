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

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಆದರೆ defined(FEAT_EVAL)
// The names of packages that once were loaded are remembered.
ಸ್ಥಿರ garray_T		ga_loaded = {0, 0, ಗಾತ್ರ(char_u *), 4, ಖಾಲಿ};
#ಆದರೆ_ಅಂತ್ಯ

// last used sequence number for sourcing scripts (current_sctx.sc_seq)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ		ಕೊನೆಯ_current_SID_seq = 0;
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ do_ಮೂಲ_ext(char_u *fname, ಪೂರ್ಣಾಂಕ check_other, ಪೂರ್ಣಾಂಕ is_vimrc, ಪೂರ್ಣಾಂಕ *ret_sid, exarg_T *eap, ಪೂರ್ಣಾಂಕ clearvars);

/*
 * Initialize the execution stack.
 */
    ಶೂನ್ಯ
estack_init(ಶೂನ್ಯ)
{
    estack_T *ನಮೂದು;

    ಆದರೆ (ga_grow(&exestack, 10) == FAIL)
	mch_exit(0);
    ನಮೂದು = ((estack_T *)exestack.ga_ದತ್ತಾಂಶ) + exestack.ga_len;
    ನಮೂದು->es_type = ETYPE_ತುದಿ;
    ನಮೂದು->es_name = ಖಾಲಿ;
    ನಮೂದು->es_lnum = 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ನಮೂದು->es_info.ufunc = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
    ++exestack.ga_len;
}

/*
 * Add an item to the execution stack.
 * Returns the new entry or NULL when out of memory.
 */
    estack_T *
estack_ತಳ್ಳು(etype_T type, char_u *name, ದೀರ್ಘ lnum)
{
    estack_T *ನಮೂದು;

    // If memory allocation fails then we'll pop more than we push, eventually
    // at the top level it will be OK again.
    ಆದರೆ (ga_grow(&exestack, 1) == FAIL)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ನಮೂದು = ((estack_T *)exestack.ga_ದತ್ತಾಂಶ) + exestack.ga_len;
    ನಮೂದು->es_type = type;
    ನಮೂದು->es_name = name;
    ನಮೂದು->es_lnum = lnum;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ನಮೂದು->es_info.ufunc = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
    ++exestack.ga_len;
    ಹಿಂತಿರುಗಿ ನಮೂದು;
}

#ಆದರೆ defined(FEAT_EVAL)
/*
 * Add a user function to the execution stack.
 */
    estack_T *
estack_ತಳ್ಳು_ufunc(ufunc_T *ufunc, ದೀರ್ಘ lnum)
{
    estack_T *ನಮೂದು = estack_ತಳ್ಳು(ETYPE_UFUNC,
	    ufunc->uf_name_exp != ಖಾಲಿ
				  ? ufunc->uf_name_exp : ufunc->uf_name, lnum);
    ಆದರೆ (ನಮೂದು != ಖಾಲಿ)
	ನಮೂದು->es_info.ufunc = ufunc;
    ಹಿಂತಿರುಗಿ ನಮೂದು;
}

/*
 * Return TRUE if "ufunc" with "lnum" is already at the top of the exe stack.
 */
    ಪೂರ್ಣಾಂಕ
estack_ತುದಿ_is_ufunc(ufunc_T *ufunc, ದೀರ್ಘ lnum)
{
    estack_T *ನಮೂದು;

    ಆದರೆ (exestack.ga_len == 0)
	ಹಿಂತಿರುಗಿ FALSE;
    ನಮೂದು = ((estack_T *)exestack.ga_ದತ್ತಾಂಶ) + exestack.ga_len - 1;
    ಹಿಂತಿರುಗಿ ನಮೂದು->es_type == ETYPE_UFUNC
	&& STRCMP( ನಮೂದು->es_name, ufunc->uf_name_exp != ಖಾಲಿ
				    ? ufunc->uf_name_exp : ufunc->uf_name) == 0
	&& ನಮೂದು->es_lnum == lnum;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Take an item off of the execution stack and return it.
 */
    estack_T *
estack_ತೆಗೆ(ಶೂನ್ಯ)
{
    ಆದರೆ (exestack.ga_len == 0)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    --exestack.ga_len;
    ಹಿಂತಿರುಗಿ ((estack_T *)exestack.ga_ದತ್ತಾಂಶ) + exestack.ga_len;
}

/*
 * Get the current value for "which" in allocated memory.
 * "which" is ESTACK_SFILE for <sfile>, ESTACK_STACK for <stack> or
 * ESTACK_SCRIPT for <script>.
 */
    char_u *
estack_sfile(estack_arg_T which ಬಳಸದ)
{
    estack_T	*ನಮೂದು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    garray_T	ga;
    size_t	len;
    ಪೂರ್ಣಾಂಕ		idx;
    etype_T	ಕೊನೆಯ_type = ETYPE_SCRIPT;
#ಆದರೆ_ಅಂತ್ಯ

    ನಮೂದು = ((estack_T *)exestack.ga_ದತ್ತಾಂಶ) + exestack.ga_len - 1;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಆದರೆ (which == ESTACK_SFILE && ನಮೂದು->es_type != ETYPE_UFUNC)
#ಆದರೆ_ಅಂತ್ಯ
    {
	ಆದರೆ (ನಮೂದು->es_name == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	ಹಿಂತಿರುಗಿ vim_strsave(ನಮೂದು->es_name);
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    // expand('<sfile>') works in a function for backwards compatibility, but
    // may give an unexpected result.  Disallow it in Vim 9 script.
    ಆದರೆ (which == ESTACK_SFILE && ಒಳ_vim9script())
    {
	ಪೂರ್ಣಾಂಕ  save_emsg_off = emsg_off;

	ಆದರೆ (emsg_off == 1)
	    // f_expand() silences errors but we do want this one
	    emsg_off = 0;
	emsg(_(e_cannot_expand_sfile_ಒಳ_vim9_function));
	emsg_off = save_emsg_off;
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    // If evaluated in a function or autocommand, return the path of the script
    // where it is defined, at script level the current script path is returned
    // instead.
    ಆದರೆ (which == ESTACK_SCRIPT)
    {
	// Walk the stack backwards, starting from the current frame.
	ಗಾಗಿ (idx = exestack.ga_len - 1; idx >= 0; --idx, --ನಮೂದು)
	{
	    ಆದರೆ (ನಮೂದು->es_type == ETYPE_UFUNC || ನಮೂದು->es_type == ETYPE_AUCMD)
	    {
		sctx_T *def_ctx = ನಮೂದು->es_type == ETYPE_UFUNC
			      ? &ನಮೂದು->es_info.ufunc->uf_script_ctx
			      : ನಮೂದು->es_info.aucmd != ಖಾಲಿ
				  ? acp_script_ctx(ನಮೂದು->es_info.aucmd) : ಖಾಲಿ;

		ಹಿಂತಿರುಗಿ def_ctx != ಖಾಲಿ && def_ctx->sc_sid > 0
			   ? vim_strsave(SCRIPT_ಅಂಶ(def_ctx->sc_sid)->sn_name)
			   : ಖಾಲಿ;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ನಮೂದು->es_type == ETYPE_SCRIPT)
		ಹಿಂತಿರುಗಿ vim_strsave(ನಮೂದು->es_name);
	}
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    // Give information about each stack entry up to the root.
    // For a function we compose the call stack, as it was done in the past:
    //   "function One[123]..Two[456]..Three"
    ga_init2(&ga, ಗಾತ್ರ(ಅಕ್ಷರ), 100);
    ಗಾಗಿ (idx = 0; idx < exestack.ga_len; ++idx)
    {
	ನಮೂದು = ((estack_T *)exestack.ga_ದತ್ತಾಂಶ) + idx;
	ಆದರೆ (ನಮೂದು->es_name != ಖಾಲಿ)
	{
	    ದೀರ್ಘ	lnum = 0;
	    size_t	added = 0;
	    string_T	type_name = {(char_u *)"", 0};
	    string_T	class_name = {(char_u *)"", 0};
	    string_T	es_name = {ನಮೂದು->es_name, STRLEN(ನಮೂದು->es_name)};

	    ಆದರೆ (ನಮೂದು->es_type != ಕೊನೆಯ_type)
	    {
		ಆಯ್ಕೆ (ನಮೂದು->es_type)
		{
		    ಸ್ಥಿತಿ ETYPE_SCRIPT:
			STR_LITERAL_ಗಣ(type_name, "script ");
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ ETYPE_UFUNC:
			STR_LITERAL_ಗಣ(type_name, "function ");
			ನಿಲ್ಲಿಸು;
		    ಪೂರ್ವನಿಯೋಜಿತ:
			ನಿಲ್ಲಿಸು;
		}
		ಕೊನೆಯ_type = ನಮೂದು->es_type;
	    }
	    ಆದರೆ (ನಮೂದು->es_type == ETYPE_UFUNC && ನಮೂದು->es_info.ufunc->uf_class != ಖಾಲಿ)
	    {
		class_name.string = ನಮೂದು->es_info.ufunc->uf_class->class_name.string;
		class_name.ಉದ್ದಳತೆ = ನಮೂದು->es_info.ufunc->uf_class->class_name.ಉದ್ದಳತೆ;
	    }
	    ಆದರೆ (idx == exestack.ga_len - 1)
		lnum = which == ESTACK_ರಾಶಿ ? SOURCING_LNUM : 0;
	    ಇಲ್ಲದಿದ್ದರೆ
		lnum = ನಮೂದು->es_lnum;

	    len = es_name.ಉದ್ದಳತೆ + type_name.ಉದ್ದಳತೆ + class_name.ಉದ್ದಳತೆ + 26;
	    ಆದರೆ (ga_grow(&ga, (ಪೂರ್ಣಾಂಕ)len) == FAIL)
		ನಿಲ್ಲಿಸು;
	    ga_concat_len(&ga, type_name.string, type_name.ಉದ್ದಳತೆ);
	    // For class methods prepend "<class name>." to the function name.
	    ಆದರೆ (*class_name.string != NUL)
	    {
		added = vim_snprintf_safelen(
		    (ಅಕ್ಷರ *)ga.ga_ದತ್ತಾಂಶ + ga.ga_len,
		    ga.ga_maxlen - ga.ga_len,
		    "<SNR>%d_%s.",
		    ನಮೂದು->es_info.ufunc->uf_script_ctx.sc_sid,
		    class_name.string);

		ga.ga_len += (ಪೂರ್ಣಾಂಕ)added;
	    }
	    ga_concat_len(&ga, es_name.string, es_name.ಉದ್ದಳತೆ);
	    // For the bottom entry of <sfile>: do not add the line number, it is used in
	    // <slnum>.  Also leave it out when the number is not set.
	    ಆದರೆ (lnum != 0)
	    {
		added = vim_snprintf_safelen(
		    (ಅಕ್ಷರ *)ga.ga_ದತ್ತಾಂಶ + ga.ga_len,
		    ga.ga_maxlen - ga.ga_len,
		    "[%ld]",
		    lnum);

		ga.ga_len += (ಪೂರ್ಣಾಂಕ)added;
	    }
	    ಆದರೆ (idx != exestack.ga_len - 1)
		GA_CONCAT_LITERAL(&ga, "..");
	}
    }

    // Only NUL-terminate when not returning NULL.
    ಆದರೆ (ga.ga_ದತ್ತಾಂಶ != ಖಾಲಿ)
	ga_append(&ga, NUL);
    ಹಿಂತಿರುಗಿ (char_u *)ga.ga_ದತ್ತಾಂಶ;
#ಆದರೆ_ಅಂತ್ಯ
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಸ್ಥಿರ ಶೂನ್ಯ
stacktrace_ತಳ್ಳು_ಅಂಶ(
	list_T		*l,
	ufunc_T		*fp,
	char_u		*event,
	linenr_T	lnum,
	char_u		*filepath)
{
    dict_T	*d;
    typval_T	tv;

    d = dict_alloc_ಬೀಗ(VAR_FIXED);
    ಆದರೆ (d == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    tv.v_type = VAR_DICT;
    tv.v_ಬೀಗ = VAR_LOCKED;
    tv.vval.v_dict = d;

    ಆದರೆ (fp != ಖಾಲಿ)
	dict_ಕೂಡಿಸು_func(d, "funcref", fp);
    ಆದರೆ (event != ಖಾಲಿ)
	dict_ಕೂಡಿಸು_string(d, "event", event);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(d, "lnum", lnum);
    dict_ಕೂಡಿಸು_string(d, "filepath", filepath);

    list_append_tv(l, &tv);
}

/*
 * Create the stacktrace from exestack.
 */
    list_T *
stacktrace_create(ಶೂನ್ಯ)
{
    list_T	*l;
    ಪೂರ್ಣಾಂಕ		i;

    l = list_alloc();
    ಆದರೆ (l == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಗಾಗಿ (i = 0; i < exestack.ga_len; ++i)
    {
	estack_T *ನಮೂದು = &((estack_T *)exestack.ga_ದತ್ತಾಂಶ)[i];
	linenr_T lnum = ನಮೂದು->es_lnum;

	ಆದರೆ (ನಮೂದು->es_type == ETYPE_SCRIPT)
	    stacktrace_ತಳ್ಳು_ಅಂಶ(l, ಖಾಲಿ, ಖಾಲಿ, lnum, ನಮೂದು->es_name);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ನಮೂದು->es_type == ETYPE_UFUNC)
	{
	    ufunc_T *fp = ನಮೂದು->es_info.ufunc;
	    sctx_T sctx = fp->uf_script_ctx;
	    char_u *filepath = sctx.sc_sid > 0 ?
				   get_scriptname(sctx.sc_sid) : (char_u *)"";

	    lnum += sctx.sc_lnum;
	    stacktrace_ತಳ್ಳು_ಅಂಶ(l, fp, ಖಾಲಿ, lnum, filepath);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ನಮೂದು->es_type == ETYPE_AUCMD)
	{
	    // The autocmd may not have a matching pattern yet, in which case
	    // es_info.aucmd is still NULL.
	    sctx_T *sctx = ನಮೂದು->es_info.aucmd != ಖಾಲಿ
			       ? acp_script_ctx(ನಮೂದು->es_info.aucmd) : ಖಾಲಿ;
	    char_u *filepath = sctx != ಖಾಲಿ && sctx->sc_sid > 0 ?
				   get_scriptname(sctx->sc_sid) : (char_u *)"";

	    ಆದರೆ (sctx != ಖಾಲಿ)
		lnum += sctx->sc_lnum;
	    stacktrace_ತಳ್ಳು_ಅಂಶ(l, ಖಾಲಿ, ನಮೂದು->es_name, lnum, filepath);
	}
    }
    ಹಿಂತಿರುಗಿ l;
}

/*
 * getstacktrace() function
 */
    ಶೂನ್ಯ
f_getstacktrace(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    rettv_list_ಗಣ(rettv, stacktrace_create());
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Get DIP_ flags from the [where] argument of a :runtime command.
 * "*argp" is advanced to after the [where] argument if it is found.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_runtime_cmd_flags(char_u **argp, size_t where_len)
{
    char_u *arg = *argp;

    ಆದರೆ (where_len == 0)
	ಹಿಂತಿರುಗಿ 0;

    ಆದರೆ (STRNCMP(arg, "START", where_len) == 0)
    {
	*argp = skipwhite(arg + where_len);
	ಹಿಂತಿರುಗಿ DIP_ಆರಂಭ + DIP_NORTP;
    }
    ಆದರೆ (STRNCMP(arg, "OPT", where_len) == 0)
    {
	*argp = skipwhite(arg + where_len);
	ಹಿಂತಿರುಗಿ DIP_OPT + DIP_NORTP;
    }
    ಆದರೆ (STRNCMP(arg, "PACK", where_len) == 0)
    {
	*argp = skipwhite(arg + where_len);
	ಹಿಂತಿರುಗಿ DIP_ಆರಂಭ + DIP_OPT + DIP_NORTP;
    }
    ಆದರೆ (STRNCMP(arg, "ALL", where_len) == 0)
    {
	*argp = skipwhite(arg + where_len);
	ಹಿಂತಿರುಗಿ DIP_ಆರಂಭ + DIP_OPT;
    }

    ಹಿಂತಿರುಗಿ 0;
}

/*
 * ":runtime [where] {name}"
 */
    ಶೂನ್ಯ
ex_runtime(exarg_T *eap)
{
    char_u  *arg = eap->arg;
    ಪೂರ್ಣಾಂಕ	    flags = eap->forceit ? DIP_ALL : 0;
    char_u  *p = skiptowhite(arg);
    flags += get_runtime_cmd_flags(&arg, p - arg);
    ಮೂಲ_runtime(arg, flags);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ runtime_expand_flags;

/*
 * Set the completion context for the :runtime command.
 */
    ಶೂನ್ಯ
ಗಣ_context_ಒಳ_runtime_cmd(expand_T *xp, char_u *arg)
{
    char_u  *p = skiptowhite(arg);
    runtime_expand_flags
	= *p != NUL ? get_runtime_cmd_flags(&arg, p - arg) : 0;
    // Skip to the last argument.
    ಆಗ (*(p = skiptowhite_esc(arg)) != NUL)
    {
	ಆದರೆ (runtime_expand_flags == 0)
	    // When there are multiple arguments and [where] is not specified,
	    // use an unrelated non-zero flag to avoid expanding [where].
	    runtime_expand_flags = DIP_ALL;
	arg = skipwhite(p);
    }
    xp->xp_context = EXPAND_RUNTIME;
    xp->xp_ನಮೂನೆ = arg;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ಮೂಲ_callback(char_u *fname, ಶೂನ್ಯ *cookie)
{
    (ಶೂನ್ಯ)do_ಮೂಲ(fname, FALSE, DOSO_NONE, cookie);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
/*
 * Find an already loaded script "name".
 * If found returns its script ID.  If not found returns -1.
 */
    ಪೂರ್ಣಾಂಕ
ಕಂಡುಹಿಡಿ_script_by_name(char_u *name)
{
    ಪೂರ್ಣಾಂಕ		    sid;
    scriptitem_T    *si;

    ಗಾಗಿ (sid = script_ವಸ್ತುಗಳು.ga_len; sid > 0; --sid)
    {
	// We used to check inode here, but that doesn't work:
	// - If a script is edited and written, it may get a different
	//   inode number, even though to the user it is the same script.
	// - If a script is deleted and another script is written, with a
	//   different name, the inode may be re-used.
	si = SCRIPT_ಅಂಶ(sid);
	ಆದರೆ (si->sn_name != ಖಾಲಿ && fnamecmp(si->sn_name, name) == 0)
	    ಹಿಂತಿರುಗಿ sid;
    }
    ಹಿಂತಿರುಗಿ -1;
}

/*
 * Add a new scriptitem with all items initialized.
 * When running out of memory "error" is set to FAIL.
 * Returns the script ID.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_new_scriptitem(ಪೂರ್ಣಾಂಕ *error)
{
    ಸ್ಥಿರ scid_T   ಕೊನೆಯ_current_SID = 0;
    ಪೂರ್ಣಾಂಕ		    sid = ++ಕೊನೆಯ_current_SID;
    scriptitem_T    *si = ಖಾಲಿ;

    ಆದರೆ (ga_grow(&script_ವಸ್ತುಗಳು, (ಪೂರ್ಣಾಂಕ)(sid - script_ವಸ್ತುಗಳು.ga_len)) == FAIL)
    {
	*error = FAIL;
	ಹಿಂತಿರುಗಿ sid;
    }
    ಆಗ (script_ವಸ್ತುಗಳು.ga_len < sid)
    {
	si = ALLOC_CLEAR_ONE(scriptitem_T);
	ಆದರೆ (si == ಖಾಲಿ)
	{
	    *error = FAIL;
	    ಹಿಂತಿರುಗಿ sid;
	}
	++script_ವಸ್ತುಗಳು.ga_len;
	SCRIPT_ಅಂಶ(script_ವಸ್ತುಗಳು.ga_len) = si;
	si->sn_name = ಖಾಲಿ;
	si->sn_ಆವೃತ್ತಿ = 1;

	// Allocate the local script variables to use for this script.
	new_script_vars(script_ವಸ್ತುಗಳು.ga_len);
	ga_init2(&si->sn_var_vals, ಗಾತ್ರ(svar_T), 10);
	hash_init(&si->sn_all_vars.dv_hashtab);
	ga_init2(&si->sn_imports, ಗಾತ್ರ(imported_T), 10);
	ga_init2(&si->sn_type_list, ಗಾತ್ರ(type_T), 10);
# ifdef FEAT_PROFILE
	si->sn_prof_on = FALSE;
# endif
    }

    // "si" can't be NULL, check only to avoid a compiler warning
    ಆದರೆ (si != ಖಾಲಿ)
	// Used to check script variable index is still valid.
	si->sn_script_seq = current_sctx.sc_seq;

    ಹಿಂತಿರುಗಿ sid;
}

    ಪೂರ್ಣಾಂಕ
get_new_scriptitem_for_fname(ಪೂರ್ಣಾಂಕ *error, char_u *fname)
{
    ಪೂರ್ಣಾಂಕ sid = get_new_scriptitem(error);

    ಆದರೆ (*error == OK)
    {
	scriptitem_T *si = SCRIPT_ಅಂಶ(sid);

	si->sn_name = vim_strsave(fname);
	si->sn_state = SN_STATE_NOT_LOADED;
    }
    ಹಿಂತಿರುಗಿ sid;
}

/*
 * If the script for "sid" is a symlink and "sn_source_sid" is not set
 * then initialize it. A new script_item is created if needed.
 */
    ಶೂನ್ಯ
check_script_symlink(ಪೂರ್ಣಾಂಕ sid)
{
    scriptitem_T *si = SCRIPT_ಅಂಶ(sid);
    ಆದರೆ (si->sn_syml_checked || si->sn_sourced_sid > 0)
	ಹಿಂತಿರುಗಿ;
    si->sn_syml_checked = TRUE;

    // If fname is a symbolic link, create an script_item for the real file.

    char_u *real_fname = fix_fname(si->sn_name);
    ಆದರೆ (real_fname != ಖಾಲಿ && STRCMP(real_fname, si->sn_name) != 0)
    {
	ಪೂರ್ಣಾಂಕ real_sid = ಕಂಡುಹಿಡಿ_script_by_name(real_fname);
	ಪೂರ್ಣಾಂಕ error2 = OK;
	ಪೂರ್ಣಾಂಕ new_sid = FALSE;
	ಆದರೆ (real_sid < 0)
	{
	    real_sid = get_new_scriptitem_for_fname(&error2, real_fname);
	    new_sid = TRUE;
	}
	ಆದರೆ (error2 == OK)
	{
	    si = SCRIPT_ಅಂಶ(sid);
	    si->sn_sourced_sid = real_sid;
	    ಆದರೆ (new_sid)
	    {
		SCRIPT_ಅಂಶ(real_sid)->sn_import_autoload
						    = si->sn_import_autoload;
		ಆದರೆ (si->sn_autoload_ಪೂರ್ವಪ್ರತ್ಯಯ != ಖಾಲಿ)
		    SCRIPT_ಅಂಶ(real_sid)->sn_autoload_ಪೂರ್ವಪ್ರತ್ಯಯ =
					vim_strsave(si->sn_autoload_ಪೂರ್ವಪ್ರತ್ಯಯ);
	    }
	}
    }
    vim_free(real_fname);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ಕಂಡುಹಿಡಿ_script_callback(char_u *fname, ಶೂನ್ಯ *cookie)
{
    ಪೂರ್ಣಾಂಕ sid;
    ಪೂರ್ಣಾಂಕ error = OK;
    ಪೂರ್ಣಾಂಕ *ret_sid = cookie;

    sid = ಕಂಡುಹಿಡಿ_script_by_name(fname);
    ಆದರೆ (sid < 0)
	// script does not exist yet, create a new scriptitem
	sid = get_new_scriptitem_for_fname(&error, fname);
    *ret_sid = sid;
}
#ಆದರೆ_ಅಂತ್ಯ

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
    ಪೂರ್ಣಾಂಕ
do_ಒಳ_ಮಾರ್ಗ(
    char_u	*ಮಾರ್ಗ,
    ಅಕ್ಷರ	*ಪೂರ್ವಪ್ರತ್ಯಯ,
    char_u	*name,
    ಪೂರ್ಣಾಂಕ		flags,
    ಶೂನ್ಯ	(*callback)(char_u *fname, ಶೂನ್ಯ *ck),
    ಶೂನ್ಯ	*cookie)
{
    char_u	*rtp;
    string_T	buf;
    char_u	*rtp_copy;
    ಪೂರ್ಣಾಂಕ		ಅಂಕೆ_files;
    char_u	**files;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		did_one = FALSE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ AMIGA
    ರಚನೆ Process	*proc = (ರಚನೆ Process *)ಕಂಡುಹಿಡಿ_Task(0L);
    APTR		save_winptr = proc->pr_ಕಿಟಕಿ_Ptr;

    // Avoid a requester here for a volume that doesn't exist.
    proc->pr_ಕಿಟಕಿ_Ptr = (APTR)-1L;
#ಆದರೆ_ಅಂತ್ಯ

    // Make a copy of 'runtimepath'.  Invoking the callback may change the
    // value.
    rtp_copy = vim_strsave(ಮಾರ್ಗ);
    buf.string = alloc(MAXPATHL);
    ಆದರೆ (buf.string != ಖಾಲಿ && rtp_copy != ಖಾಲಿ)
    {
	size_t	prefixlen = 0;
	size_t	namelen = 0;

	ಆದರೆ (name != ಖಾಲಿ)
	{
	    prefixlen = STRLEN(ಪೂರ್ವಪ್ರತ್ಯಯ);
	    namelen = STRLEN(name);

	    ಆದರೆ (p_verbose > 10)
	    {
		verbose_enter();
		ಆದರೆ (*ಪೂರ್ವಪ್ರತ್ಯಯ != NUL)
		    smsg(_("Searching for \"%s\" under \"%s\" in \"%s\""),
					       (ಅಕ್ಷರ *)name, ಪೂರ್ವಪ್ರತ್ಯಯ, (ಅಕ್ಷರ *)ಮಾರ್ಗ);
		ಇಲ್ಲದಿದ್ದರೆ
		    smsg(_("Searching for \"%s\" in \"%s\""),
						       (ಅಕ್ಷರ *)name, (ಅಕ್ಷರ *)ಮಾರ್ಗ);
		verbose_leave();
	    }
	}

	// Loop over all entries in 'runtimepath'.
	rtp = rtp_copy;
	ಆಗ (*rtp != NUL && ((flags & DIP_ALL) || !did_one))
	{
	    // Copy the path from 'runtimepath' to buf[].
	    buf.ಉದ್ದಳತೆ = (size_t)copy_option_part(&rtp, buf.string, MAXPATHL, ",");

	    // Skip after or non-after directories.
	    ಆದರೆ (flags & (DIP_NOAFTER | DIP_AFTER))
	    {
		ಪೂರ್ಣಾಂಕ is_after = buf.ಉದ್ದಳತೆ >= 5
		    && STRCMP(buf.string + buf.ಉದ್ದಳತೆ - 5, "after") == 0;

		ಆದರೆ ((is_after && (flags & DIP_NOAFTER))
			|| (!is_after && (flags & DIP_AFTER)))
		    ಮುಂದುವರಿ;
	    }

	    ಆದರೆ (name == ಖಾಲಿ)
	    {
		(*callback)(buf.string, (ಶೂನ್ಯ *)&cookie);
		ಆದರೆ (!did_one)
		    did_one = (cookie == ಖಾಲಿ);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (buf.ಉದ್ದಳತೆ + 2 + prefixlen + namelen < MAXPATHL)
	    {
		char_u	*np;
		char_u	*ಬಾಲ;

		ಆದರೆ (*buf.string != NUL
		    && !after_pathsep(buf.string, buf.string + buf.ಉದ್ದಳತೆ))
		{
		    STRCPY(buf.string + buf.ಉದ್ದಳತೆ, PATHSEPSTR);
		    buf.ಉದ್ದಳತೆ += ಗಾತ್ರ(PATHSEP);
		}
		STRCPY(buf.string + buf.ಉದ್ದಳತೆ, ಪೂರ್ವಪ್ರತ್ಯಯ);
		buf.ಉದ್ದಳತೆ += prefixlen;
		ಬಾಲ = buf.string + buf.ಉದ್ದಳತೆ;

		// Loop over all patterns in "name"
		np = name;
		ಆಗ (*np != NUL && ((flags & DIP_ALL) || !did_one))
		{
		    // Append the pattern from "name" to buf[].
		    copy_option_part(&np, ಬಾಲ, (ಪೂರ್ಣಾಂಕ)(MAXPATHL - (ಬಾಲ - buf.string)),
								       "\t ");

		    ಆದರೆ (p_verbose > 10)
		    {
			verbose_enter();
			smsg(_("Searching for \"%s\""), buf.string);
			verbose_leave();
		    }

		    // Expand wildcards, invoke the callback for each match.
		    ಆದರೆ (gen_expand_wildcards(1, &buf.string, &ಅಂಕೆ_files, &files,
				  (flags & DIP_DIR) ? EW_DIR : EW_FILE) == OK)
		    {
			ಗಾಗಿ (i = 0; i < ಅಂಕೆ_files; ++i)
			{
			    (*callback)(files[i], cookie);
			    did_one = TRUE;
			    ಆದರೆ (!(flags & DIP_ALL))
				ನಿಲ್ಲಿಸು;
			}
			FreeWild(ಅಂಕೆ_files, files);
		    }
		}
	    }
	}
    }
    vim_free(buf.string);
    vim_free(rtp_copy);
    ಆದರೆ (!did_one && name != ಖಾಲಿ)
    {
	ಅಕ್ಷರ *basepath = ಮಾರ್ಗ == p_rtp ? "runtimepath" : "packpath";

	ಆದರೆ (flags & DIP_ERR)
	    semsg(_(e_directory_not_ಸಿಕ್ಕಿತು_ಒಳ_str_str), basepath, name);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p_verbose > 0)
	{
	    verbose_enter();
	    smsg(_("not found in '%s': \"%s\""), basepath, name);
	    verbose_leave();
	}
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ AMIGA
    proc->pr_ಕಿಟಕಿ_Ptr = save_winptr;
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ did_one ? OK : FAIL;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
do_ಒಳ_ಮಾರ್ಗ_and_pp(
    char_u	*ಮಾರ್ಗ,
    char_u	*name,
    ಪೂರ್ಣಾಂಕ		flags,
    ಶೂನ್ಯ	(*callback)(char_u *fname, ಶೂನ್ಯ *ck),
    ಶೂನ್ಯ	*cookie)
{
    ಪೂರ್ಣಾಂಕ		ಮುಗಿದ = FAIL;

    ಆದರೆ ((flags & DIP_NORTP) == 0)
	ಮುಗಿದ = do_ಒಳ_ಮಾರ್ಗ(ಮಾರ್ಗ, "", name, flags, callback, cookie);

    ಆದರೆ ((ಮುಗಿದ == FAIL || (flags & DIP_ALL)) && (flags & DIP_ಆರಂಭ))
	ಮುಗಿದ = do_ಒಳ_ಮಾರ್ಗ(p_pp, "pack/*/start/*/", name, flags, callback,
								       cookie);

    ಆದರೆ ((ಮುಗಿದ == FAIL || (flags & DIP_ALL)) && (flags & DIP_OPT))
	ಮುಗಿದ = do_ಒಳ_ಮಾರ್ಗ(p_pp, "pack/*/opt/*/", name, flags, callback,
								       cookie);

    ಹಿಂತಿರುಗಿ ಮುಗಿದ;
}

/*
 * Just like do_in_path_and_pp(), using 'runtimepath' for "path".
 */
    ಪೂರ್ಣಾಂಕ
do_ಒಳ_runtimepath(
    char_u	*name,
    ಪೂರ್ಣಾಂಕ		flags,
    ಶೂನ್ಯ	(*callback)(char_u *fname, ಶೂನ್ಯ *ck),
    ಶೂನ್ಯ	*cookie)
{
    ಹಿಂತಿರುಗಿ do_ಒಳ_ಮಾರ್ಗ_and_pp(p_rtp, name, flags, callback, cookie);
}

/*
 * Source the file "name" from all directories in 'runtimepath'.
 * "name" can contain wildcards.
 * When "flags" has DIP_ALL: source all files, otherwise only the first one.
 *
 * return FAIL when no file could be sourced, OK otherwise.
 */
    ಪೂರ್ಣಾಂಕ
ಮೂಲ_runtime(char_u *name, ಪೂರ್ಣಾಂಕ flags)
{
    ಹಿಂತಿರುಗಿ ಮೂಲ_ಒಳ_ಮಾರ್ಗ(p_rtp, name, flags, ಖಾಲಿ);
}

/*
 * Just like source_runtime(), but use "path" instead of 'runtimepath'
 * and return the script ID in "ret_sid".
 */
    ಪೂರ್ಣಾಂಕ
ಮೂಲ_ಒಳ_ಮಾರ್ಗ(char_u *ಮಾರ್ಗ, char_u *name, ಪೂರ್ಣಾಂಕ flags, ಪೂರ್ಣಾಂಕ *ret_sid)
{
    ಹಿಂತಿರುಗಿ do_ಒಳ_ಮಾರ್ಗ_and_pp(ಮಾರ್ಗ, name, flags, ಮೂಲ_callback, ret_sid);
}

#ಆದರೆ defined(FEAT_EVAL)

/*
 * Find "name" in 'runtimepath'. If found a new scriptitem is created for it
 * and it's script ID is returned.
 * If not found returns -1.
 */
    ಪೂರ್ಣಾಂಕ
ಕಂಡುಹಿಡಿ_script_ಒಳ_rtp(char_u *name)
{
    ಪೂರ್ಣಾಂಕ sid = -1;

    (ಶೂನ್ಯ)do_ಒಳ_ಮಾರ್ಗ_and_pp(p_rtp, name, DIP_ಆರಂಭ | DIP_NOAFTER,
						   ಕಂಡುಹಿಡಿ_script_callback, &sid);
    ಹಿಂತಿರುಗಿ sid;
}

/*
 * Expand wildcards in "pat" and invoke do_source() for each match.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಮೂಲ_all_matches(char_u *pat)
{
    ಪೂರ್ಣಾಂಕ	    ಅಂಕೆ_files;
    char_u  **files;
    ಪೂರ್ಣಾಂಕ	    i;

    ಆದರೆ (gen_expand_wildcards(1, &pat, &ಅಂಕೆ_files, &files, EW_FILE) != OK)
	ಹಿಂತಿರುಗಿ;

    ಗಾಗಿ (i = 0; i < ಅಂಕೆ_files; ++i)
	(ಶೂನ್ಯ)do_ಮೂಲ(files[i], FALSE, DOSO_NONE, ಖಾಲಿ);
    FreeWild(ಅಂಕೆ_files, files);
}

/*
 * Add the package directory to 'runtimepath'.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಕೂಡಿಸು_pack_dir_ಗಮ್ಯ_rtp(char_u *fname)
{
    char_u  *p4, *p3, *p2, *p1, *p;
    char_u  *ನಮೂದು;
    char_u  *insp = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ	    c;
    string_T  new_rtp;
    size_t  keep;
    size_t  p_rtp_len;
    string_T  afterdir = {ಖಾಲಿ, 0};
    char_u  *after_insp = ಖಾಲಿ;
    char_u  *ffname = ಖಾಲಿ;
    size_t  fname_len;
    string_T	buf = {ಖಾಲಿ, 0};
    ಪೂರ್ಣಾಂಕ	    retval = FAIL;

    p4 = p3 = p2 = p1 = get_past_ತಲೆ(fname);
    ಗಾಗಿ (p = p1; *p; MB_PTR_ADV(p))
	ಆದರೆ (vim_ispathsep_nocolon(*p))
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
    ಆದರೆ (ffname == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    // Find "ffname" in "p_rtp", ignoring '/' vs '\' differences.
    // Also stop at the first "after" directory.
    fname_len = STRLEN(ffname);
    buf.string = alloc(MAXPATHL);
    ಆದರೆ (buf.string == ಖಾಲಿ)
	ಹೋಗು theend;

    p_rtp_len = 0;
    ಗಾಗಿ (ನಮೂದು = p_rtp; *ನಮೂದು != NUL; )
    {
	char_u *cur_ನಮೂದು = ನಮೂದು;

	buf.ಉದ್ದಳತೆ = (size_t)copy_option_part(&ನಮೂದು, buf.string, MAXPATHL, ",");

	// keep track of p_rtp length as we go to make the STRLEN() below have less work to do
	p_rtp_len += (*(cur_ನಮೂದು + buf.ಉದ್ದಳತೆ) == ',') ? buf.ಉದ್ದಳತೆ + 1 : buf.ಉದ್ದಳತೆ;

	ಆದರೆ ((p = (char_u *)ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು((ಅಕ್ಷರ *)buf.string, "after")) != ಖಾಲಿ
		&& p > buf.string
		&& vim_ispathsep(p[-1])
		&& (vim_ispathsep(p[5]) || p[5] == NUL || p[5] == ','))
	{
	    ಆದರೆ (insp == ಖಾಲಿ)
		// Did not find "ffname" before the first "after" directory,
		// insert it before this entry.
		insp = cur_ನಮೂದು;
	    after_insp = cur_ನಮೂದು;
	    ನಿಲ್ಲಿಸು;
	}

	ಆದರೆ (insp == ಖಾಲಿ)
	{
	    char_u  *rtp_ffname;
	    ಪೂರ್ಣಾಂಕ	    ಹೊಂದಿಕೆ;

	    ಆದರೆ (*buf.string != NUL && !after_pathsep(buf.string, buf.string + buf.ಉದ್ದಳತೆ))
		STRCPY(buf.string + buf.ಉದ್ದಳತೆ, PATHSEPSTR);
	    rtp_ffname = fix_fname(buf.string);
	    ಆದರೆ (rtp_ffname == ಖಾಲಿ)
		ಹೋಗು theend;
	    ಹೊಂದಿಕೆ = vim_fnamencmp(rtp_ffname, ffname, fname_len) == 0;
	    vim_free(rtp_ffname);
	    ಆದರೆ (ಹೊಂದಿಕೆ)
		// Insert "ffname" after this entry (and comma).
		insp = ನಮೂದು;
	}
    }

    // finish measuring the length of p_rtp
    p_rtp_len += STRLEN(p_rtp + p_rtp_len);
    ಆದರೆ (insp == ಖಾಲಿ)
	// Both "fname" and "after" not found, append at the end.
	insp = p_rtp + p_rtp_len;

    // check if rtp/pack/name/start/name/after exists
    fname_len = STRLEN(fname);
    concat_fnames(fname, fname_len, (char_u *)"after", STRLEN_LITERAL("after"), TRUE, &afterdir);
    ಆದರೆ (afterdir.string == ಖಾಲಿ || !mch_isdir(afterdir.string))
	afterdir.ಉದ್ದಳತೆ = 0;

    new_rtp.string = alloc(p_rtp_len + fname_len + afterdir.ಉದ್ದಳತೆ + 3); // add two for commas and one for NUL
    ಆದರೆ (new_rtp.string == ಖಾಲಿ)
	ಹೋಗು theend;

    // We now have 'rtp' parts: {keep}{keep_after}{rest}.
    // Create new_rtp, first: {keep},{fname}
    keep = (size_t)(insp - p_rtp);
    mch_memmove(new_rtp.string, p_rtp, keep);
    new_rtp.ಉದ್ದಳತೆ = keep;
    ಆದರೆ (*insp == NUL)
	new_rtp.string[new_rtp.ಉದ್ದಳತೆ++] = ',';  // add comma before
    mch_memmove(new_rtp.string + new_rtp.ಉದ್ದಳತೆ, fname, fname_len);
    new_rtp.ಉದ್ದಳತೆ += fname_len;
    ಆದರೆ (*insp != NUL)
	new_rtp.string[new_rtp.ಉದ್ದಳತೆ++] = ',';  // add comma after

    ಆದರೆ (afterdir.ಉದ್ದಳತೆ > 0 && after_insp != ಖಾಲಿ)
    {
	size_t keep_after = (size_t)(after_insp - p_rtp);
	size_t append_len = keep_after - keep;

	// Add to new_rtp: {keep},{fname}{keep_after},{afterdir}
	mch_memmove(new_rtp.string + new_rtp.ಉದ್ದಳತೆ, p_rtp + keep,
							append_len);
	new_rtp.ಉದ್ದಳತೆ += append_len;
	mch_memmove(new_rtp.string + new_rtp.ಉದ್ದಳತೆ, afterdir.string, afterdir.ಉದ್ದಳತೆ);
	new_rtp.ಉದ್ದಳತೆ += afterdir.ಉದ್ದಳತೆ;
	new_rtp.string[new_rtp.ಉದ್ದಳತೆ++] = ',';
	keep = keep_after;
    }

    ಆದರೆ (p_rtp[keep] != NUL)
    {
	size_t	append_len = p_rtp_len - keep;

	// Append rest: {keep},{fname}{keep_after},{afterdir}{rest}
	mch_memmove(new_rtp.string + new_rtp.ಉದ್ದಳತೆ, p_rtp + keep, append_len + 1);	// add one for NUL
	new_rtp.ಉದ್ದಳತೆ += append_len;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	new_rtp.string[new_rtp.ಉದ್ದಳತೆ] = NUL;

    ಆದರೆ (afterdir.ಉದ್ದಳತೆ > 0 && after_insp == ಖಾಲಿ)
    {
	// Append afterdir when "after" was not found:
	// {keep},{fname}{rest},{afterdir}
	new_rtp.string[new_rtp.ಉದ್ದಳತೆ++] = ',';
	STRCPY(new_rtp.string + new_rtp.ಉದ್ದಳತೆ, afterdir.string);
    }

    ಗಣ_option_ಮೌಲ್ಯ_give_err((char_u *)"rtp", 0L, new_rtp.string, 0);
    vim_free(new_rtp.string);
    retval = OK;

theend:
    vim_free(buf.string);
    vim_free(ffname);
    vim_free(afterdir.string);
    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Load scripts in "plugin" and "ftdetect" directories of the package.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ತುಂಬು_pack_plugin(char_u *fname)
{
    ಸ್ಥಿರ ಅಕ್ಷರ *plugpat = "%s/plugin/**/*.vim";
    ಸ್ಥಿರ ಅಕ್ಷರ *ftpat = "%s/ftdetect/*.vim";
    ಪೂರ್ಣಾಂಕ		len;
    char_u	*ffname = fix_fname(fname);
    char_u	*pat = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		retval = FAIL;

    ಆದರೆ (ffname == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    len = (ಪೂರ್ಣಾಂಕ)STRLEN(ffname) + (ಪೂರ್ಣಾಂಕ)STRLEN(ftpat);
    pat = alloc(len);
    ಆದರೆ (pat == ಖಾಲಿ)
	ಹೋಗು theend;
    vim_snprintf((ಅಕ್ಷರ *)pat, len, plugpat, ffname);
    ಮೂಲ_all_matches(pat);

    {
	char_u *cmd = vim_strsave((char_u *)"g:did_load_filetypes");

	// If runtime/filetype.vim wasn't loaded yet, the scripts will be
	// found when it loads.
	ಆದರೆ (cmd != ಖಾಲಿ && eval_ಗಮ್ಯ_ಸಂಖ್ಯೆ(cmd, FALSE) > 0)
	{
	    do_cmdline_cmd((char_u *)"augroup filetypedetect");
	    vim_snprintf((ಅಕ್ಷರ *)pat, len, ftpat, ffname);
	    ಮೂಲ_all_matches(pat);
	    do_cmdline_cmd((char_u *)"augroup END");
	}
	vim_free(cmd);
    }
    vim_free(pat);
    retval = OK;

theend:
    vim_free(ffname);
    ಹಿಂತಿರುಗಿ retval;
}

// used for "cookie" of add_pack_plugin()
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ APP_ಕೂಡಿಸು_DIR;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ APP_ತುಂಬು;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ APP_BOTH;

    ಸ್ಥಿರ ಶೂನ್ಯ
ಕೂಡಿಸು_pack_plugin(char_u *fname, ಶೂನ್ಯ *cookie)
{
    ಆದರೆ (cookie != &APP_ತುಂಬು)
    {
	char_u	*buf = alloc(MAXPATHL);
	char_u	*p;
	ಪೂರ್ಣಾಂಕ	ಸಿಕ್ಕಿತು = FALSE;

	ಆದರೆ (buf == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;
	p = p_rtp;
	ಆಗ (*p != NUL)
	{
	    copy_option_part(&p, buf, MAXPATHL, ",");
	    ಆದರೆ (pathcmp((ಅಕ್ಷರ *)buf, (ಅಕ್ಷರ *)fname, -1) == 0)
	    {
		ಸಿಕ್ಕಿತು = TRUE;
		ನಿಲ್ಲಿಸು;
	    }
	}
	vim_free(buf);
	ಆದರೆ (!ಸಿಕ್ಕಿತು)
	    // directory is not yet in 'runtimepath', add it
	    ಆದರೆ (ಕೂಡಿಸು_pack_dir_ಗಮ್ಯ_rtp(fname) == FAIL)
		ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (cookie != &APP_ಕೂಡಿಸು_DIR)
	ತುಂಬು_pack_plugin(fname);
}

/*
 * Add all packages in the "start" directory to 'runtimepath'.
 */
    ಶೂನ್ಯ
ಕೂಡಿಸು_pack_ಆರಂಭ_dirs(ಶೂನ್ಯ)
{
    do_ಒಳ_ಮಾರ್ಗ(p_pp, "", (char_u *)"pack/*/start/*", DIP_ALL + DIP_DIR,
					       ಕೂಡಿಸು_pack_plugin, &APP_ಕೂಡಿಸು_DIR);
}

/*
 * Load plugins from all packages in the "start" directory.
 */
    ಶೂನ್ಯ
ತುಂಬು_ಆರಂಭ_packages(ಶೂನ್ಯ)
{
    did_ಮೂಲ_packages = TRUE;
    do_ಒಳ_ಮಾರ್ಗ(p_pp, "", (char_u *)"pack/*/start/*", DIP_ALL + DIP_DIR,
						  ಕೂಡಿಸು_pack_plugin, &APP_ತುಂಬು);
}

/*
 * ":packloadall"
 * Find plugins in the package directories and source them.
 */
    ಶೂನ್ಯ
ex_packloadall(exarg_T *eap)
{
    ಆದರೆ (!did_ಮೂಲ_packages || eap->forceit)
    {
	// First do a round to add all directories to 'runtimepath', then load
	// the plugins. This allows for plugins to use an autoload directory
	// of another plugin.
	ಕೂಡಿಸು_pack_ಆರಂಭ_dirs();
	ತುಂಬು_ಆರಂಭ_packages();
    }
}

/*
 * ":packadd[!] {name}"
 */
    ಶೂನ್ಯ
ex_packadd(exarg_T *eap)
{
    ಸ್ಥಿರ ಅಕ್ಷರ *plugpat = "pack/*/%s/%s";
    ಪೂರ್ಣಾಂಕ		len;
    ಅಕ್ಷರ	*pat;
    ಪೂರ್ಣಾಂಕ		ಸುತ್ತು;
    ಪೂರ್ಣಾಂಕ		res = OK;

    // Round 1: use "start", round 2: use "opt".
    ಗಾಗಿ (ಸುತ್ತು = 1; ಸುತ್ತು <= 2; ++ಸುತ್ತು)
    {
	// Only look under "start" when loading packages wasn't done yet.
	ಆದರೆ (ಸುತ್ತು == 1 && did_ಮೂಲ_packages)
	    ಮುಂದುವರಿ;

	len = (ಪೂರ್ಣಾಂಕ)STRLEN(plugpat) + (ಪೂರ್ಣಾಂಕ)STRLEN(eap->arg) + 5;
	pat = alloc(len);
	ಆದರೆ (pat == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;
	vim_snprintf(pat, len, plugpat, ಸುತ್ತು == 1 ? "start" : "opt", eap->arg);
	// The first round don't give a "not found" error, in the second round
	// only when nothing was found in the first round.
	res = do_ಒಳ_ಮಾರ್ಗ(p_pp, "", (char_u *)pat,
		DIP_ALL + DIP_DIR + (ಸುತ್ತು == 2 && res == FAIL ? DIP_ERR : 0),
		ಕೂಡಿಸು_pack_plugin, eap->forceit ? &APP_ಕೂಡಿಸು_DIR : &APP_BOTH);
	vim_free(pat);
    }
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Sort "gap" and remove duplicate entries.  "gap" is expected to contain a
 * list of file names in allocated memory.
 */
    ಶೂನ್ಯ
remove_duplicates(garray_T *gap)
{
    ಪೂರ್ಣಾಂಕ	    i;
    ಪೂರ್ಣಾಂಕ	    j;
    char_u  **fnames = (char_u **)gap->ga_ದತ್ತಾಂಶ;

    ವಿಂಗಡಿಸು_ಅಕ್ಷರಸರಣಿಗಳು(fnames, gap->ga_len);
    ಗಾಗಿ (i = gap->ga_len - 1; i > 0; --i)
	ಆದರೆ (fnamecmp(fnames[i - 1], fnames[i]) == 0)
	{
	    vim_free(fnames[i]);
	    ಗಾಗಿ (j = i + 1; j < gap->ga_len; ++j)
		fnames[j - 1] = fnames[j];
	    --gap->ga_len;
	}
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ExpandRTDir_int(
    char_u	*pat,
    size_t	pat_len,
    ಪೂರ್ಣಾಂಕ		flags,
    ಪೂರ್ಣಾಂಕ		keep_ext,
    garray_T	*gap,
    ಅಕ್ಷರ	*dirnames[])
{
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; dirnames[i] != ಖಾಲಿ; ++i)
    {
	ಸ್ಥಿರಾಂಕ size_t	buf_len = STRLEN(dirnames[i]) + pat_len + 64;
	ಅಕ್ಷರ		*buf = alloc(buf_len);
	ಆದರೆ (buf == ಖಾಲಿ)
	{
	    ga_clear_ಅಕ್ಷರಸರಣಿಗಳು(gap);
	    ಹಿಂತಿರುಗಿ;
	}
	ಪೂರ್ಣಾಂಕ		glob_flags = 0;
	ಪೂರ್ಣಾಂಕ		expand_dirs = FALSE;

	// Build base pattern
	vim_snprintf(buf, buf_len, "%s%s%s%s",
		     *dirnames[i] ? dirnames[i] : "", *dirnames[i] ? "/" : "",
		     pat, "*.vim");

expand:
	ಆದರೆ ((flags & DIP_NORTP) == 0)
	    globpath(p_rtp, (char_u *)buf, gap, glob_flags, expand_dirs);

	ಆದರೆ (flags & DIP_ಆರಂಭ)
	{
	    // Build complete search path: pack/*/start/*/dirnames[i]/pat*.vim
	    vim_snprintf(buf, buf_len, "pack/*/start/*/%s%s%s%s",
			 *dirnames[i] ? dirnames[i] : "",
			 *dirnames[i] ? "/" : "",
			 pat,
			 expand_dirs ? "*" : "*.vim");
	    globpath(p_pp, (char_u *)buf, gap, glob_flags, expand_dirs);
	}

	ಆದರೆ (flags & DIP_OPT)
	{
	    // Build complete search path: pack/*/opt/*/dirnames[i]/pat*.vim
	    vim_snprintf(buf, buf_len, "pack/*/opt/*/%s%s%s%s",
			 *dirnames[i] ? dirnames[i] : "",
			 *dirnames[i] ? "/" : "", pat,
			 expand_dirs ? "*" : "*.vim");
	    globpath(p_pp, (char_u *)buf, gap, glob_flags, expand_dirs);
	}

	// Second round for directories
	ಆದರೆ (*dirnames[i] == NUL && !expand_dirs)
	{
	    // expand dir names in another round
	    vim_snprintf(buf, buf_len, "%s*", pat);
	    glob_flags = WILD_ಕೂಡಿಸು_SLASH;
	    expand_dirs = TRUE;
	    ಹೋಗು expand;
	}

	vim_free(buf);
    }

    ಪೂರ್ಣಾಂಕ pat_pathsep_cnt = 0;
    ಗಾಗಿ (size_t i = 0; i < pat_len; ++i)
    {
	ಆದರೆ (vim_ispathsep(pat[i]))
	    ++pat_pathsep_cnt;
    }

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < gap->ga_len; ++i)
    {
	char_u *ಹೊಂದಿಕೆ = ((char_u **)gap->ga_ದತ್ತಾಂಶ)[i];
	char_u *s = ಹೊಂದಿಕೆ;
	char_u *e = s + STRLEN(s);
	ಆದರೆ (e - 4 > s && !keep_ext && STRNICMP(e - 4, ".vim", 4) == 0)
	{
	    e -= 4;
	    *e = NUL;
	}

	ಪೂರ್ಣಾಂಕ ಹೊಂದಿಕೆ_pathsep_cnt = (e > s && e[-1] == '/') ? -1 : 0;
	ಗಾಗಿ (s = e; s > ಹೊಂದಿಕೆ; MB_PTR_ಹಿಂಭಾಗ(ಹೊಂದಿಕೆ, s))
	{
	    ಆದರೆ (s < ಹೊಂದಿಕೆ || (vim_ispathsep(*s)
				     && ++ಹೊಂದಿಕೆ_pathsep_cnt > pat_pathsep_cnt))
		ನಿಲ್ಲಿಸು;
	}
	++s;
	ಆದರೆ (s != ಹೊಂದಿಕೆ)
	    mch_memmove(ಹೊಂದಿಕೆ, s, e - s + 1);
    }

    ಆದರೆ (gap->ga_len == 0)
	ಹಿಂತಿರುಗಿ;

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
    ಪೂರ್ಣಾಂಕ
ExpandRTDir(
    char_u	*pat,
    ಪೂರ್ಣಾಂಕ		flags,
    ಪೂರ್ಣಾಂಕ		*ಅಂಕೆ_file,
    char_u	***file,
    ಅಕ್ಷರ	*dirnames[])
{
    *ಅಂಕೆ_file = 0;
    *file = ಖಾಲಿ;

    garray_T	ga;
    ga_init2(&ga, ಗಾತ್ರ(ಅಕ್ಷರ *), 10);

    ExpandRTDir_int(pat, STRLEN(pat), flags, FALSE, &ga, dirnames);

    ಆದರೆ (ga.ga_len == 0)
	ಹಿಂತಿರುಗಿ FAIL;

    *file = ga.ga_ದತ್ತಾಂಶ;
    *ಅಂಕೆ_file = ga.ga_len;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Handle command line completion for :runtime command.
 */
    ಪೂರ್ಣಾಂಕ
expand_runtime_cmd(char_u *pat, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_Matches, char_u ***matches)
{
    *ಅಂಕೆ_Matches = 0;
    *matches = ಖಾಲಿ;

    garray_T	ga;
    ga_init2(&ga, ಗಾತ್ರ(ಅಕ್ಷರ *), 10);

    size_t pat_len = (ಪೂರ್ಣಾಂಕ)STRLEN(pat);
    ಅಕ್ಷರ *dirnames[] = {"", ಖಾಲಿ};
    ExpandRTDir_int(pat, pat_len, runtime_expand_flags, TRUE, &ga, dirnames);

    // Try to complete values for [where] argument when none was found.
    ಆದರೆ (runtime_expand_flags == 0)
    {
	ಅಕ್ಷರ *where_ಮೌಲ್ಯಗಳು[] = {"START", "OPT", "PACK", "ALL"};
	ಗಾಗಿ (size_t i = 0; i < ARRAY_ಉದ್ದಳತೆ(where_ಮೌಲ್ಯಗಳು); ++i)
	    ಆದರೆ (STRNCMP(pat, where_ಮೌಲ್ಯಗಳು[i], pat_len) == 0)
	    {
		char_u *p = vim_strsave((char_u *)where_ಮೌಲ್ಯಗಳು[i]);
		ಆದರೆ (p != ಖಾಲಿ && ga_ಕೂಡಿಸು_string(&ga, p) == FAIL)
		    vim_free(p);
	    }
    }

    ಆದರೆ (ga.ga_len == 0)
	ಹಿಂತಿರುಗಿ FAIL;

    *matches = ga.ga_ದತ್ತಾಂಶ;
    *ಅಂಕೆ_Matches = ga.ga_len;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Expand loadplugin names:
 * 'packpath'/pack/ * /opt/{pat}
 */
    ಪೂರ್ಣಾಂಕ
Expand_Pack_ಕೂಡಿಸು_Dir(
    char_u	*pat,
    ಪೂರ್ಣಾಂಕ		*ಅಂಕೆ_file,
    char_u	***file)
{
    char_u	*s;
    char_u	*e;
    char_u	*ಹೊಂದಿಕೆ;
    garray_T	ga;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		pat_len;

    *ಅಂಕೆ_file = 0;
    *file = ಖಾಲಿ;
    pat_len = (ಪೂರ್ಣಾಂಕ)STRLEN(pat);
    ga_init2(&ga, ಗಾತ್ರ(ಅಕ್ಷರ *), 10);

    s = alloc(pat_len + 26);
    ಆದರೆ (s == ಖಾಲಿ)
    {
	ga_clear_ಅಕ್ಷರಸರಣಿಗಳು(&ga);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    sprintf((ಅಕ್ಷರ *)s, "pack/*/opt/%s*", pat);
    globpath(p_pp, s, &ga, 0, TRUE);
    vim_free(s);

    ಗಾಗಿ (i = 0; i < ga.ga_len; ++i)
    {
	ಹೊಂದಿಕೆ = ((char_u **)ga.ga_ದತ್ತಾಂಶ)[i];
	s = gettail(ಹೊಂದಿಕೆ);
	e = s + STRLEN(s);
	mch_memmove(ಹೊಂದಿಕೆ, s, e - s + 1);
    }

    ಆದರೆ (ga.ga_len == 0)
	ಹಿಂತಿರುಗಿ FAIL;

    // Sort and remove duplicates which can happen when specifying multiple
    // directories in dirnames.
    remove_duplicates(&ga);

    *file = ga.ga_ದತ್ತಾಂಶ;
    *ಅಂಕೆ_file = ga.ga_len;
    ಹಿಂತಿರುಗಿ OK;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
cmd_ಮೂಲ(char_u *fname, exarg_T *eap)
{
    ಪೂರ್ಣಾಂಕ clearvars = FALSE;

    ಆದರೆ (*fname != NUL && STRNCMP(fname, "++clear", 7) == 0)
    {
	// ++clear argument is supplied
	clearvars = TRUE;
	fname = fname + 7;
	ಆದರೆ (*fname != NUL)
	{
	    semsg(_(e_invalid_ವಾದ_str), eap->arg);
	    ಹಿಂತಿರುಗಿ;
	}
    }

    ಆದರೆ (*fname != NUL && eap != ಖಾಲಿ && eap->addr_ಎಣಿಕೆ > 0)
    {
	// if a filename is specified to :source, then a range is not allowed
	emsg(_(e_no_range_allowed));
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (eap != ಖಾಲಿ && *fname == NUL)
    {
	ಆದರೆ (eap->forceit)
	    // a file name is needed to source normal mode commands
	    emsg(_(e_ವಾದ_required));
	ಇಲ್ಲದಿದ್ದರೆ
	    // source ex commands from the current buffer
	    do_ಮೂಲ_ext(ಖಾಲಿ, FALSE, DOSO_NONE, ಖಾಲಿ, eap, clearvars);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (eap != ಖಾಲಿ && eap->forceit)
	// ":source!": read Normal mode commands
	// Need to execute the commands directly.  This is required at least
	// for:
	// - ":g" command busy
	// - after ":argdo", ":windo" or ":bufdo"
	// - another command follows
	// - inside a loop
	openscript(fname, global_busy || listcmd_busy || eap->nextcmd != ಖಾಲಿ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
						 || eap->cstack->cs_idx >= 0
#ಆದರೆ_ಅಂತ್ಯ
						 );

    // ":source" read ex commands
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (do_ಮೂಲ(fname, FALSE, DOSO_NONE, ಖಾಲಿ) == FAIL)
	semsg(_(e_cant_open_file_str), fname);
}

/*
 * ":source {fname}"
 */
    ಶೂನ್ಯ
ex_ಮೂಲ(exarg_T *eap)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BROWSE
    ಆದರೆ (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	char_u *fname = ಖಾಲಿ;

	fname = do_browse(0, (char_u *)_("Source Vim script"), eap->arg,
				      ಖಾಲಿ, ಖಾಲಿ,
				      (char_u *)_(BROWSE_FILTER_MACROS), ಖಾಲಿ);
	ಆದರೆ (fname != ಖಾಲಿ)
	{
	    cmd_ಮೂಲ(fname, eap);
	    vim_free(fname);
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	cmd_ಮೂಲ(eap->arg, eap);
}

#ಆದರೆ defined(FEAT_EVAL)
/*
 * ":options"
 */
    ಶೂನ್ಯ
ex_options(
    exarg_T	*eap ಬಳಸದ)
{
    char_u  buf[500];
    ಪೂರ್ಣಾಂಕ	    multi_mods = 0;

    buf[0] = NUL;
    (ಶೂನ್ಯ)ಕೂಡಿಸು_win_cmd_modifiers(buf, &cmdmod, &multi_mods);

    vim_setenv((char_u *)"OPTWIN_CMD", buf);
    cmd_ಮೂಲ((char_u *)SYS_OPTWIN_FILE, ಖಾಲಿ);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * ":source" and associated commands.
 */

#ಆದರೆ defined(FEAT_EVAL)
/*
 * Return the address holding the next breakpoint line for a source cookie.
 */
    linenr_T *
ಮೂಲ_breakpoint(ಶೂನ್ಯ *cookie)
{
    ಹಿಂತಿರುಗಿ &((ಮೂಲ_cookie_T *)cookie)->breakpoint;
}

/*
 * Return the address holding the debug tick for a source cookie.
 */
    ಪೂರ್ಣಾಂಕ *
ಮೂಲ_dbg_tick(ಶೂನ್ಯ *cookie)
{
    ಹಿಂತಿರುಗಿ &((ಮೂಲ_cookie_T *)cookie)->dbg_tick;
}

/*
 * Return the nesting level for a source cookie.
 */
    ಪೂರ್ಣಾಂಕ
ಮೂಲ_level(ಶೂನ್ಯ *cookie)
{
    ಹಿಂತಿರುಗಿ ((ಮೂಲ_cookie_T *)cookie)->level;
}

/*
 * Return the readahead line. Note that the pointer may become invalid when
 * getting the next line, if it's concatenated with the next one.
 */
    char_u *
ಮೂಲ_nextline(ಶೂನ್ಯ *cookie)
{
    ಹಿಂತಿರುಗಿ ((ಮೂಲ_cookie_T *)cookie)->nextline;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ (defined(MSWIN) && defined(FEAT_CSCOPE)) || defined(HAVE_FD_CLOEXEC)
# define USE_FOPEN_NOINH
/*
 * Special function to open a file without handle inheritance.
 * When possible the handle is closed on exec().
 */
    ಸ್ಥಿರ FILE *
fopen_noinh_readbin(ಅಕ್ಷರ *filename)
{
# ifdef MSWIN
    ಪೂರ್ಣಾಂಕ	fd_tmp = mch_open(filename, O_RDONLY | O_ದ್ವಿಮಾನ | O_NOINHERIT, 0);
# ಇಲ್ಲದಿದ್ದರೆ
    ಪೂರ್ಣಾಂಕ	fd_tmp = mch_open(filename, O_RDONLY, 0);
# endif

    ಆದರೆ (fd_tmp == -1)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

# ifdef HAVE_FD_CLOEXEC
    {
	ಪೂರ್ಣಾಂಕ fdflags = fcntl(fd_tmp, F_GETFD);
	ಆದರೆ (fdflags >= 0 && (fdflags & FD_CLOEXEC) == 0)
	    (ಶೂನ್ಯ)fcntl(fd_tmp, F_SETFD, fdflags | FD_CLOEXEC);
    }
# endif

    ಹಿಂತಿರುಗಿ fdopen(fd_tmp, READBIN);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Initialization for sourcing lines from the current buffer. Reads all the
 * lines from the buffer and stores it in the cookie grow array.
 * Returns a pointer to the name ":source buffer=<n>" on success and NULL on
 * failure.
 */
    ಸ್ಥಿರ char_u *
do_ಮೂಲ_ಬಫರ್_init(ಮೂಲ_cookie_T *sp, exarg_T *eap)
{
    linenr_T	curr_lnum;
    char_u	*ಸಾಲು = ಖಾಲಿ;
    char_u	*fname;

    ಆದರೆ (curbuf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // Use ":source buffer=<num>" as the script name
    ಆದರೆ (curbuf->b_ffname != ಖಾಲಿ)
	fname = vim_strsave(curbuf->b_ffname);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	vim_snprintf((ಅಕ್ಷರ *)IObuff, IOSIZE, ":source buffer=%d", curbuf->b_fnum);
	fname = vim_strsave(IObuff);
    }
    ಆದರೆ (fname == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ga_init2(&sp->buflines, ಗಾತ್ರ(char_u *), 100);

    // Copy the lines from the buffer into a grow array
    ಗಾಗಿ (curr_lnum = eap->line1; curr_lnum <= eap->line2; curr_lnum++)
    {
	ಸಾಲು = vim_strsave(ml_get(curr_lnum));
	ಆದರೆ (ಸಾಲು == ಖಾಲಿ)
	    ಹೋಗು errret;
	ಆದರೆ (ga_ಕೂಡಿಸು_string(&sp->buflines, ಸಾಲು) == FAIL)
	    ಹೋಗು errret;
    }
    sp->buf_lnum = 0;
    sp->ಮೂಲ_from_buf = TRUE;
    // When sourcing a range of lines from a buffer, use buffer line number.
    sp->sourcing_lnum = eap->line1 - 1;

    ಹಿಂತಿರುಗಿ fname;

errret:
    vim_free(fname);
    vim_free(ಸಾಲು);
    ga_clear_ಅಕ್ಷರಸರಣಿಗಳು(&sp->buflines);
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
do_ಮೂಲ_ext(
    char_u	*fname,
    ಪೂರ್ಣಾಂಕ		check_other,	    // check for .vimrc and _vimrc
    ಪೂರ್ಣಾಂಕ		is_vimrc,	    // DOSO_ value
    ಪೂರ್ಣಾಂಕ		*ret_sid ಬಳಸದ,
    exarg_T	*eap,
    ಪೂರ್ಣಾಂಕ		clearvars ಬಳಸದ)
{
    ಮೂಲ_cookie_T	    cookie;
    char_u		    *p;
    char_u		    *fname_not_fixed = ಖಾಲಿ;
    char_u		    *fname_exp = ಖಾಲಿ;
    char_u		    *firstline = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ			    retval = FAIL;
    sctx_T		    save_current_sctx;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ STARTUPTIME
    ರಚನೆ timeval	    tv_rel;
    ರಚನೆ timeval	    tv_ಆರಂಭ;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
    proftime_T		    wait_ಆರಂಭ;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ			    save_sticky_cmdmod_flags = sticky_cmdmod_flags;
    ಪೂರ್ಣಾಂಕ			    trigger_ಮೂಲ_post = FALSE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    funccal_ನಮೂದು_T	    funccalp_ನಮೂದು;
    ಪೂರ್ಣಾಂಕ			    save_debug_break_level = debug_break_level;
    ಪೂರ್ಣಾಂಕ			    sid = -1;
    scriptitem_T	    *si = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ			    save_estack_compiling = estack_compiling;
    ESTACK_CHECK_DECLARATION;
#ಆದರೆ_ಅಂತ್ಯ

    CLEAR_ಕ್ಷೇತ್ರ(cookie);
    ಆದರೆ (fname == ಖಾಲಿ)
    {
	// sourcing lines from a buffer
	fname_exp = do_ಮೂಲ_ಬಫರ್_init(&cookie, eap);
	ಆದರೆ (fname_exp == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	fname_not_fixed = expand_env_save(fname);
	ಆದರೆ (fname_not_fixed == ಖಾಲಿ)
	    ಹೋಗು theend;
	fname_exp = fix_fname(fname_not_fixed);
	ಆದರೆ (fname_exp == ಖಾಲಿ)
	    ಹೋಗು theend;
	ಆದರೆ (mch_isdir(fname_exp))
	{
	    smsg(_("Cannot source a directory: \"%s\""), fname);
	    ಹೋಗು theend;
	}
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    estack_compiling = FALSE;

    // See if we loaded this script before.
    sid = ಕಂಡುಹಿಡಿ_script_by_name(fname_exp);
    ಆದರೆ (sid > 0 && ret_sid != ಖಾಲಿ
			  && SCRIPT_ಅಂಶ(sid)->sn_state != SN_STATE_NOT_LOADED)
    {
	// Already loaded and no need to load again, return here.
	*ret_sid = sid;
	retval = OK;
	ಹೋಗು theend;
    }
#ಆದರೆ_ಅಂತ್ಯ

    // Apply SourceCmd autocommands, they should get the file and source it.
    // When sourcing a file "fname_exp" was made a full path name by
    // fix_fname() above, avoid expanding it again.
    ಆದರೆ ((fname == ಖಾಲಿ
		? has_autocmd(EVENT_SOURCECMD, fname_exp, ಖಾಲಿ)
		: has_autocmd_fullname(EVENT_SOURCECMD, fname_exp, ಖಾಲಿ))
	    && apply_autocmds(EVENT_SOURCECMD, fname_exp, fname_exp,
							       FALSE, curbuf))
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	retval = aborting() ? FAIL : OK;
#ಇಲ್ಲದಿದ್ದರೆ
	retval = OK;
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (retval == OK)
	    // Apply SourcePost autocommands.
	    apply_autocmds(EVENT_SOURCEPOST, fname_exp, fname_exp,
								FALSE, curbuf);
	ಹೋಗು theend;
    }

    // Apply SourcePre autocommands, they may get the file.
    apply_autocmds(EVENT_SOURCEPRE, fname_exp, fname_exp, FALSE, curbuf);

    ಆದರೆ (!cookie.ಮೂಲ_from_buf)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_FOPEN_NOINH
	cookie.fp = fopen_noinh_readbin((ಅಕ್ಷರ *)fname_exp);
#ಇಲ್ಲದಿದ್ದರೆ
	cookie.fp = mch_fopen((ಅಕ್ಷರ *)fname_exp, READBIN);
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಆದರೆ (cookie.fp == ಖಾಲಿ && check_other)
    {
	// Try again, replacing file name ".vimrc" by "_vimrc" or vice versa,
	// and ".exrc" by "_exrc" or vice versa.
	p = gettail(fname_exp);
	ಆದರೆ ((*p == '.' || *p == '_')
		&& (STRICMP(p + 1, "vimrc") == 0
		    || STRICMP(p + 1, "gvimrc") == 0
		    || STRICMP(p + 1, "exrc") == 0))
	{
	    ಆದರೆ (*p == '_')
		*p = '.';
	    ಇಲ್ಲದಿದ್ದರೆ
		*p = '_';
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_FOPEN_NOINH
	    cookie.fp = fopen_noinh_readbin((ಅಕ್ಷರ *)fname_exp);
#ಇಲ್ಲದಿದ್ದರೆ
	    cookie.fp = mch_fopen((ಅಕ್ಷರ *)fname_exp, READBIN);
#ಆದರೆ_ಅಂತ್ಯ
	}
    }

    ಆದರೆ (cookie.fp == ಖಾಲಿ && !cookie.ಮೂಲ_from_buf)
    {
	ಆದರೆ (p_verbose > 0)
	{
	    verbose_enter();
	    ಆದರೆ (SOURCING_NAME == ಖಾಲಿ)
		smsg(_("could not source \"%s\""), fname);
	    ಇಲ್ಲದಿದ್ದರೆ
		smsg(_("line %ld: could not source \"%s\""),
							SOURCING_LNUM, fname);
	    verbose_leave();
	}
	ಹೋಗು theend;
    }

    // The file exists.
    // - In verbose mode, give a message.
    // - For a vimrc file, may want to set 'compatible', call vimrc_found().
    ಆದರೆ (p_verbose > 1)
    {
	verbose_enter();
	ಆದರೆ (SOURCING_NAME == ಖಾಲಿ)
	    smsg(_("sourcing \"%s\""), fname);
	ಇಲ್ಲದಿದ್ದರೆ
	    smsg(_("line %ld: sourcing \"%s\""), SOURCING_LNUM, fname);
	verbose_leave();
    }
    ಆದರೆ (is_vimrc == DOSO_VIMRC)
	vimrc_ಸಿಕ್ಕಿತು(fname_exp, (char_u *)"MYVIMRC");
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (is_vimrc == DOSO_GVIMRC)
	vimrc_ಸಿಕ್ಕಿತು(fname_exp, (char_u *)"MYGVIMRC");

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_CRNL
    // If no automatic file format: Set default to CR-NL.
    ಆದರೆ (*p_ffs == NUL)
	cookie.fileformat = EOL_DOS;
    ಇಲ್ಲದಿದ್ದರೆ
	cookie.fileformat = EOL_UNKNOWN;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    // Check if this script has a breakpoint.
    cookie.breakpoint = dbg_ಕಂಡುಹಿಡಿ_breakpoint(TRUE, fname_exp, (linenr_T)0);
    cookie.fname = fname_exp;
    cookie.dbg_tick = debug_tick;

    cookie.level = ex_nesting_level;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ STARTUPTIME
    ಆದರೆ (ಸಮಯ_fd != ಖಾಲಿ)
	ಸಮಯ_ತಳ್ಳು(&tv_rel, &tv_ಆರಂಭ);
#ಆದರೆ_ಅಂತ್ಯ

    // "legacy" does not apply to commands in the script
    sticky_cmdmod_flags = 0;

    save_current_sctx = current_sctx;
    ಆದರೆ ((cmdmod.cmod_flags & CMOD_VIM9CMD) && cookie.ಮೂಲ_from_buf)
	// When the ":vim9cmd" command modifier is used, source buffer lines as
	// Vim9 script
	current_sctx.sc_ಆವೃತ್ತಿ = SCRIPT_ಆವೃತ್ತಿ_VIM9;
    ಇಲ್ಲದಿದ್ದರೆ
	current_sctx.sc_ಆವೃತ್ತಿ = 1;  // default script version

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    current_sctx.sc_lnum = 0;

# ifdef FEAT_PROFILE
    ಆದರೆ (do_profiling == PROF_YES)
	prof_ಮಗು_enter(&wait_ಆರಂಭ);		// entering a child now
# endif

    // Don't use local function variables, if called from a function.
    // Also starts profiling timer for nested script.
    save_funccal(&funccalp_ನಮೂದು);

    // Reset "KeyTyped" to avoid some commands thinking they are invoked
    // interactively.  E.g. defining a function would output indent.
    ಪೂರ್ಣಾಂಕ save_ಕೀಲಿ_Typed = ಕೀಲಿ_Typed;
    ಕೀಲಿ_Typed = FALSE;

    // Check if this script was sourced before to find its SID.
    // Always use a new sequence number.
    current_sctx.sc_seq = ++ಕೊನೆಯ_current_SID_seq;
    ಆದರೆ (sid > 0)
    {
	hashtab_T	*ht;
	ಪೂರ್ಣಾಂಕ		todo;
	hashitem_T	*hi;
	dictitem_T	*di;

	// loading the same script again
	current_sctx.sc_sid = sid;
	si = SCRIPT_ಅಂಶ(sid);
	ಆದರೆ (si->sn_state == SN_STATE_NOT_LOADED)
	{
	    // this script was found but not loaded yet
	    si->sn_state = SN_STATE_NEW;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    si->sn_state = SN_STATE_RELOAD;

	    ಆದರೆ (!clearvars)
	    {
		// Script-local variables remain but "const" can be set again.
		// In Vim9 script variables will be cleared when "vim9script"
		// is encountered without the "noclear" argument.
		ht = &SCRIPT_VARS(sid);
		todo = (ಪೂರ್ಣಾಂಕ)ht->ht_ಬಳಸಿದ;
		FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(ht, hi, todo)
		    ಆದರೆ (!HASHITEM_ಖಾಲಿಗಳು(hi))
		    {
			--todo;
			di = HI2DI(hi);
			di->di_flags |= DI_FLAGS_RELOAD;
		    }
		// imports can be redefined once
		mark_imports_for_reload(sid);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		clear_vim9_scriptlocal_vars(sid);

	    // reset version, "vim9script" may have been added or removed.
	    si->sn_ಆವೃತ್ತಿ = 1;
	}
	ಆದರೆ (ret_sid != ಖಾಲಿ)
	    *ret_sid = sid;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಪೂರ್ಣಾಂಕ error = OK;

	// It's new, generate a new SID and initialize the scriptitem.
	sid = get_new_scriptitem(&error);
	current_sctx.sc_sid = sid;
	ಆದರೆ (error == FAIL)
	    ಹೋಗು almosttheend;
	si = SCRIPT_ಅಂಶ(sid);
	si->sn_name = fname_exp;
	fname_exp = vim_strsave(si->sn_name);  // used for autocmd
	ಆದರೆ (ret_sid != ಖಾಲಿ)
	    *ret_sid = sid;

	// Remember the "is_vimrc" flag for when the file is sourced again.
	si->sn_is_vimrc = is_vimrc;
    }

    // Keep the sourcing name/lnum, for recursive calls.
    estack_ತಳ್ಳು(ETYPE_SCRIPT, si->sn_name, 0);
    ESTACK_CHECK_ಸಿದ್ಧತೆ;

# ifdef FEAT_PROFILE
    ಆದರೆ (do_profiling == PROF_YES)
    {
	ಪೂರ್ಣಾಂಕ	forceit;

	// Check if we do profiling for this script.
	ಆದರೆ (!si->sn_prof_on && has_profiling(TRUE, si->sn_name, &forceit, ಖಾಲಿ))
	{
	    script_do_profile(si);
	    si->sn_pr_force = forceit;
	}
	ಆದರೆ (si->sn_prof_on)
	{
	    ++si->sn_pr_ಎಣಿಕೆ;
	    profile_ಆರಂಭ(&si->sn_pr_ಆರಂಭ);
	    profile_zero(&si->sn_pr_children);
	}
    }
# endif
#ಇಲ್ಲದಿದ್ದರೆ
    // Keep the sourcing name/lnum, for recursive calls.
    estack_ತಳ್ಳು(ETYPE_SCRIPT, fname_exp, 0);
#ಆದರೆ_ಅಂತ್ಯ

    cookie.conv.vc_type = CONV_NONE;		// no conversion

    // Read the first line so we can check for a UTF-8 BOM.
    firstline = getsourceline(0, (ಶೂನ್ಯ *)&cookie, 0, TRUE);
    ಆದರೆ (firstline != ಖಾಲಿ && STRLEN(firstline) >= 3 && firstline[0] == 0xef
			      && firstline[1] == 0xbb && firstline[2] == 0xbf)
    {
	// Found BOM; setup conversion, skip over BOM and recode the line.
	convert_ಸಿದ್ಧತೆ(&cookie.conv, (char_u *)"utf-8", p_enc);
	p = string_convert(&cookie.conv, firstline + 3, ಖಾಲಿ);
	ಆದರೆ (p == ಖಾಲಿ)
	    p = vim_strsave(firstline + 3);
	ಆದರೆ (p != ಖಾಲಿ)
	{
	    vim_free(firstline);
	    firstline = p;
	}
    }

    // Call do_cmdline, which will call getsourceline() to get the lines.
    do_cmdline(firstline, getsourceline, (ಶೂನ್ಯ *)&cookie,
				     DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_REPEAT);
    retval = OK;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
    ಆದರೆ (do_profiling == PROF_YES)
    {
	// Get "si" again, "script_items" may have been reallocated.
	si = SCRIPT_ಅಂಶ(sid);
	ಆದರೆ (si->sn_prof_on)
	{
	    profile_ಕೊನೆ(&si->sn_pr_ಆರಂಭ);
	    profile_sub_wait(&wait_ಆರಂಭ, &si->sn_pr_ಆರಂಭ);
	    profile_ಕೂಡಿಸು(&si->sn_pr_ಒಟ್ಟು, &si->sn_pr_ಆರಂಭ);
	    profile_self(&si->sn_pr_self, &si->sn_pr_ಆರಂಭ,
							 &si->sn_pr_children);
	}
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (got_int)
	emsg(_(e_interrupted));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ESTACK_CHECK_NOW;
#ಆದರೆ_ಅಂತ್ಯ
    estack_ತೆಗೆ();
    ಆದರೆ (p_verbose > 1)
    {
	verbose_enter();
	smsg(_("finished sourcing %s"), fname);
	ಆದರೆ (SOURCING_NAME != ಖಾಲಿ)
	    smsg(_("continuing in %s"), SOURCING_NAME);
	verbose_leave();
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ STARTUPTIME
    ಆದರೆ (ಸಮಯ_fd != ಖಾಲಿ)
    {
	vim_snprintf((ಅಕ್ಷರ *)IObuff, IOSIZE, "sourcing %s", fname);
	ಸಮಯ_ಸಂದೇಶ_2((ಅಕ್ಷರ *)IObuff, &tv_ಆರಂಭ);
	ಸಮಯ_ತೆಗೆ(&tv_rel);
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (!got_int)
	trigger_ಮೂಲ_post = TRUE;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    // After a "finish" in debug mode, need to break at first command of next
    // sourced file.
    ಆದರೆ (save_debug_break_level > ex_nesting_level
	    && debug_break_level == ex_nesting_level)
	++debug_break_level;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
almosttheend:
    // If "sn_save_cpo" is set that means we encountered "vim9script": restore
    // 'cpoptions', unless in the main .vimrc file.
    // Get "si" again, "script_items" may have been reallocated.
    si = SCRIPT_ಅಂಶ(sid);
    ಆದರೆ (si->sn_save_cpo != ಖಾಲಿ && si->sn_is_vimrc == DOSO_NONE)
    {
	ಆದರೆ (STRCMP(p_cpo, CPO_VIM) != 0)
	{
	    char_u *f;
	    char_u *t;

	    // 'cpo' was changed in the script.  Apply the same change to the
	    // saved value, if possible.
	    ಗಾಗಿ (f = (char_u *)CPO_VIM; *f != NUL; ++f)
		ಆದರೆ (vim_strchr(p_cpo, *f) == ಖಾಲಿ
			&& (t = vim_strchr(si->sn_save_cpo, *f)) != ಖಾಲಿ)
		    // flag was removed, also remove it from the saved 'cpo'
		    mch_memmove(t, t + 1, STRLEN(t));
	    ಗಾಗಿ (f = p_cpo; *f != NUL; ++f)
		ಆದರೆ (vim_strchr((char_u *)CPO_VIM, *f) == ಖಾಲಿ
			&& vim_strchr(si->sn_save_cpo, *f) == ಖಾಲಿ)
		{
		    // flag was added, also add it to the saved 'cpo'
		    t = alloc(STRLEN(si->sn_save_cpo) + 2);
		    ಆದರೆ (t != ಖಾಲಿ)
		    {
			*t = *f;
			STRCPY(t + 1, si->sn_save_cpo);
			vim_free(si->sn_save_cpo);
			si->sn_save_cpo = t;
		    }
		}
	}
	ಗಣ_option_ಮೌಲ್ಯ_give_err((char_u *)"cpo",
					   0L, si->sn_save_cpo, OPT_NO_REDRAW);
    }
    VIM_CLEAR(si->sn_save_cpo);

    restore_funccal();
# ifdef FEAT_PROFILE
    ಆದರೆ (do_profiling == PROF_YES)
	prof_ಮಗು_exit(&wait_ಆರಂಭ);		// leaving a child now
# endif

    ಕೀಲಿ_Typed = save_ಕೀಲಿ_Typed;
#ಆದರೆ_ಅಂತ್ಯ
    current_sctx = save_current_sctx;

    ಆದರೆ (cookie.fp != ಖಾಲಿ)
	fclose(cookie.fp);
    ಆದರೆ (cookie.ಮೂಲ_from_buf)
	ga_clear_ಅಕ್ಷರಸರಣಿಗಳು(&cookie.buflines);
    vim_free(cookie.nextline);
    vim_free(firstline);
    convert_ಸಿದ್ಧತೆ(&cookie.conv, ಖಾಲಿ, ಖಾಲಿ);

    ಆದರೆ (trigger_ಮೂಲ_post)
	apply_autocmds(EVENT_SOURCEPOST, fname_exp, fname_exp, FALSE, curbuf);

theend:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಆದರೆ (sid > 0 && ret_sid != ಖಾಲಿ
	    && fname_not_fixed != ಖಾಲಿ && fname_exp != ಖಾಲಿ)
    {
	ಪೂರ್ಣಾಂಕ not_fixed_sid = ಕಂಡುಹಿಡಿ_script_by_name(fname_not_fixed);

	// If "fname_not_fixed" is a symlink then we source the linked file.
	// If the original name is in the script list we add the ID of the
	// script that was actually sourced.
	ಆದರೆ (SCRIPT_ID_VALID(not_fixed_sid) && not_fixed_sid != sid)
	    SCRIPT_ಅಂಶ(not_fixed_sid)->sn_sourced_sid = sid;
    }
#ಆದರೆ_ಅಂತ್ಯ

    vim_free(fname_not_fixed);
    vim_free(fname_exp);
    sticky_cmdmod_flags = save_sticky_cmdmod_flags;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    estack_compiling = save_estack_compiling;
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ retval;
}

    ಪೂರ್ಣಾಂಕ
do_ಮೂಲ(
    char_u	*fname,
    ಪೂರ್ಣಾಂಕ		check_other,	    // check for .vimrc and _vimrc
    ಪೂರ್ಣಾಂಕ		is_vimrc,	    // DOSO_ value
    ಪೂರ್ಣಾಂಕ		*ret_sid)
{
    ಹಿಂತಿರುಗಿ do_ಮೂಲ_ext(fname, check_other, is_vimrc, ret_sid, ಖಾಲಿ, FALSE);
}


#ಆದರೆ defined(FEAT_EVAL)

/*
 * ":scriptnames"
 */
    ಶೂನ್ಯ
ex_scriptnames(exarg_T *eap)
{
    ಪೂರ್ಣಾಂಕ i;

    ಆದರೆ (eap->addr_ಎಣಿಕೆ > 0 || *eap->arg != NUL)
    {
	// :script {scriptId}: edit the script
	ಆದರೆ (eap->addr_ಎಣಿಕೆ > 0 && !SCRIPT_ID_VALID(eap->line2))
	    emsg(_(e_invalid_ವಾದ));
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (eap->addr_ಎಣಿಕೆ > 0)
		eap->arg = SCRIPT_ಅಂಶ(eap->line2)->sn_name;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		expand_env(eap->arg, NameBuff, MAXPATHL);
		eap->arg = NameBuff;
	    }
	    do_exedit(eap, ಖಾಲಿ);
	}
	ಹಿಂತಿರುಗಿ;
    }

    ಗಾಗಿ (i = 1; i <= script_ವಸ್ತುಗಳು.ga_len && !got_int; ++i)
    {
	scriptitem_T *si = SCRIPT_ಅಂಶ(i);

	ಆದರೆ (si->sn_name != ಖಾಲಿ)
	{
	    ಅಕ್ಷರ sourced_buf[20];

	    home_replace(ಖಾಲಿ, si->sn_name, NameBuff, MAXPATHL, TRUE);
	    ಆದರೆ (si->sn_sourced_sid > 0)
		vim_snprintf(sourced_buf, 20, "->%d", si->sn_sourced_sid);
	    ಇಲ್ಲದಿದ್ದರೆ
		sourced_buf[0] = NUL;
	    vim_snprintf((ಅಕ್ಷರ *)IObuff, IOSIZE, "%3d%s%s: %s",
		    i,
		    sourced_buf,
		    si->sn_state == SN_STATE_NOT_LOADED ? " A" : "",
		    NameBuff);
	    ಆದರೆ (!ಸಂದೇಶ_filtered(IObuff))
	    {
		ಸಂದೇಶ_putchar('\n');
		ಸಂದೇಶ_outtrans(IObuff);
		ಹೊರಹೋಗು_flush();	    // output one line at a time
		ui_breakcheck();
	    }
	}
    }
}

# ಆದರೆ defined(BACKSLASH_ಒಳ_FILENAME)
/*
 * Fix slashes in the list of script names for 'shellslash'.
 */
    ಶೂನ್ಯ
scriptnames_slash_adjust(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ i;

    ಗಾಗಿ (i = 1; i <= script_ವಸ್ತುಗಳು.ga_len; ++i)
	ಆದರೆ (SCRIPT_ಅಂಶ(i)->sn_name != ಖಾಲಿ)
	    slash_adjust(SCRIPT_ಅಂಶ(i)->sn_name);
}
# endif

/*
 * Get a pointer to a script name.  Used for ":verbose set".
 * Message appended to "Last set from "
 */
    char_u *
get_scriptname(scid_T id)
{
    ಆದರೆ (id == SID_MODELINE)
	ಹಿಂತಿರುಗಿ (char_u *)_("modeline");
    ಆದರೆ (id == SID_CMDARG)
	ಹಿಂತಿರುಗಿ (char_u *)_("--cmd argument");
    ಆದರೆ (id == SID_CARG)
	ಹಿಂತಿರುಗಿ (char_u *)_("-c argument");
    ಆದರೆ (id == SID_ENV)
	ಹಿಂತಿರುಗಿ (char_u *)_("environment variable");
    ಆದರೆ (id == SID_ERROR)
	ಹಿಂತಿರುಗಿ (char_u *)_("error handler");
    ಆದರೆ (id == SID_WINLAYOUT)
	ಹಿಂತಿರುಗಿ (char_u *)_("changed window size");
    ಹಿಂತಿರುಗಿ SCRIPT_ಅಂಶ(id)->sn_name;
}

# ಆದರೆ defined(EXITFREE)
    ಶೂನ್ಯ
free_scriptnames(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ			i;

    ಗಾಗಿ (i = script_ವಸ್ತುಗಳು.ga_len; i > 0; --i)
    {
	scriptitem_T *si = SCRIPT_ಅಂಶ(i);

	// the variables themselves are cleared in evalvars_clear()
	vim_free(si->sn_vars);

	vim_free(si->sn_name);
	free_imports_and_script_vars(i);
	free_string_option(si->sn_save_cpo);
#  ifdef FEAT_PROFILE
	ga_clear(&si->sn_prl_ga);
#  endif
	vim_free(si->sn_autoload_ಪೂರ್ವಪ್ರತ್ಯಯ);
	vim_free(si);
    }
    ga_clear(&script_ವಸ್ತುಗಳು);
}

    ಶೂನ್ಯ
free_autoload_scriptnames(ಶೂನ್ಯ)
{
    ga_clear_ಅಕ್ಷರಸರಣಿಗಳು(&ga_loaded);
}
# endif

    linenr_T
get_sourced_lnum(
	char_u *(*fgetline)(ಪೂರ್ಣಾಂಕ, ಶೂನ್ಯ *, ಪೂರ್ಣಾಂಕ, getline_opt_T),
	ಶೂನ್ಯ *cookie)
{
    ಹಿಂತಿರುಗಿ fgetline == getsourceline
			? ((ಮೂಲ_cookie_T *)cookie)->sourcing_lnum
			: SOURCING_LNUM;
}

/*
 * Return a List of script-local functions defined in the script with id
 * 'sid'.
 */
    ಸ್ಥಿರ list_T *
get_script_local_funcs(scid_T sid)
{
    hashtab_T	*functbl;
    hashitem_T	*hi;
    long_u	todo;
    list_T	*l;

    l = list_alloc();
    ಆದರೆ (l == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // Iterate through all the functions in the global function hash table
    // looking for functions with script ID 'sid'.
    functbl = func_tbl_get();
    todo = functbl->ht_ಬಳಸಿದ;
    FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(functbl, hi, todo)
    {
	ufunc_T	*fp;

	ಆದರೆ (HASHITEM_ಖಾಲಿಗಳು(hi))
	    ಮುಂದುವರಿ;

	--todo;
	fp = HI2UF(hi);

	// Add active functions with script id == 'sid'
	ಆದರೆ (!(fp->uf_flags & FC_DEAD) && (fp->uf_script_ctx.sc_sid == sid))
	{
	    char_u	*name;

	    ಆದರೆ (fp->uf_name_exp != ಖಾಲಿ)
		name = fp->uf_name_exp;
	    ಇಲ್ಲದಿದ್ದರೆ
		name = fp->uf_name;

	    list_append_string(l, name, -1);
	}
    }

    ಹಿಂತಿರುಗಿ l;
}

/*
 * getscriptinfo() function
 */
    ಶೂನ್ಯ
f_getscriptinfo(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    char_u	*pat = ಖಾಲಿ;
    regmatch_T	regmatch;
    ಪೂರ್ಣಾಂಕ		filterpat = FALSE;
    scid_T	sid = -1;

    ಆದರೆ (rettv_list_alloc(rettv) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (check_for_opt_dict_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    l = rettv->vval.v_list;

    regmatch.regprog = ಖಾಲಿ;
    regmatch.rm_ic = p_ic;

    ಆದರೆ (argvars[0].v_type == VAR_DICT)
    {
	dictitem_T *sid_di = dict_ಕಂಡುಹಿಡಿ(argvars[0].vval.v_dict,
							   (char_u *)"sid", 3);
	ಆದರೆ (sid_di != ಖಾಲಿ)
	{
	    ಪೂರ್ಣಾಂಕ error = FALSE;
	    sid = tv_get_ಸಂಖ್ಯೆ_chk(&sid_di->di_tv, &error);
	    ಆದರೆ (error)
		ಹಿಂತಿರುಗಿ;
	    ಆದರೆ (sid <= 0)
	    {
		semsg(_(e_invalid_ಮೌಲ್ಯ_for_ವಾದ_str_str), "sid",
						tv_get_string(&sid_di->di_tv));
		ಹಿಂತಿರುಗಿ;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    pat = dict_get_string(argvars[0].vval.v_dict, "name", TRUE);
	    ಆದರೆ (pat != ಖಾಲಿ)
		regmatch.regprog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
	    ಆದರೆ (regmatch.regprog != ಖಾಲಿ)
		filterpat = TRUE;
	}
    }

    ಗಾಗಿ (varnumber_T i = sid > 0 ? sid : 1;
		       (i == sid || sid <= 0) && i <= script_ವಸ್ತುಗಳು.ga_len; ++i)
    {
	scriptitem_T	*si = SCRIPT_ಅಂಶ(i);
	dict_T		*d;

	ಆದರೆ (si->sn_name == ಖಾಲಿ)
	    ಮುಂದುವರಿ;

	ಆದರೆ (filterpat && !vim_regexec(&regmatch, si->sn_name, (colnr_T)0))
	    ಮುಂದುವರಿ;

	ಆದರೆ ((d = dict_alloc()) == ಖಾಲಿ
		|| list_append_dict(l, d) == FAIL)
	{
	    dict_unref(d);
	    ಹೋಗು theend;
	}
	ಆದರೆ (dict_ಕೂಡಿಸು_string(d, "name", si->sn_name) == FAIL
		|| dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(d, "sid", i) == FAIL
		|| dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(d, "sourced", si->sn_sourced_sid) == FAIL
		|| dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(d, "version", si->sn_ಆವೃತ್ತಿ) == FAIL
		|| dict_ಕೂಡಿಸು_bool(d, "autoload",
				si->sn_state == SN_STATE_NOT_LOADED) == FAIL)
	    ಹೋಗು theend;

	// When a script ID is specified, return information about only the
	// specified script, and add the script-local variables and functions.
	ಆದರೆ (sid > 0)
	{
	    dict_T	*var_dict;
	    list_T	*fn_list;

	    var_dict = dict_copy(&si->sn_vars->sv_dict, TRUE, TRUE,
								get_copyID());
	    ಆದರೆ (var_dict == ಖಾಲಿ)
		ಹೋಗು theend;
	    ಆದರೆ (dict_ಕೂಡಿಸು_dict(d, "variables", var_dict) == FAIL)
	    {
		dict_unref(var_dict);
		ಹೋಗು theend;
	    }
	    --var_dict->dv_refcount;
	    fn_list = get_script_local_funcs(sid);
	    ಆದರೆ (fn_list == ಖಾಲಿ || dict_ಕೂಡಿಸು_list(d, "functions", fn_list) == FAIL)
	    {
		list_unref(fn_list);
		ಹೋಗು theend;
	    }
	}
    }

theend:
    vim_regfree(regmatch.regprog);
    vim_free(pat);
}

#ಆದರೆ_ಅಂತ್ಯ

    ಸ್ಥಿರ char_u *
get_one_sourceline(ಮೂಲ_cookie_T *sp)
{
    garray_T		ga;
    ಪೂರ್ಣಾಂಕ			len;
    ಪೂರ್ಣಾಂಕ			c;
    char_u		*buf;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_CRNL
    ಪೂರ್ಣಾಂಕ			has_cr;		// CR-LF found
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ			have_read = FALSE;

    // use a growarray to store the sourced line
    ga_init2(&ga, 1, 250);

    // Loop until there is a finished line (or end-of-file).
    ++sp->sourcing_lnum;
    ಗಾಗಿ (;;)
    {
	// make room to read at least 120 (more) characters
	ಆದರೆ (ga_grow(&ga, 120) == FAIL)
	    ನಿಲ್ಲಿಸು;
	ಆದರೆ (sp->ಮೂಲ_from_buf)
	{
	    ಆದರೆ (sp->buf_lnum >= sp->buflines.ga_len)
		ನಿಲ್ಲಿಸು;		    // all the lines are processed
	    ga_concat(&ga, ((char_u **)sp->buflines.ga_ದತ್ತಾಂಶ)[sp->buf_lnum]);
	    sp->buf_lnum++;
	    ಆದರೆ (ga_grow(&ga, 1) == FAIL)
		ನಿಲ್ಲಿಸು;
	    buf = (char_u *)ga.ga_ದತ್ತಾಂಶ;
	    buf[ga.ga_len++] = NUL;
	    len = ga.ga_len;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    buf = (char_u *)ga.ga_ದತ್ತಾಂಶ;
	    ಆದರೆ (fgets((ಅಕ್ಷರ *)buf + ga.ga_len, ga.ga_maxlen - ga.ga_len,
			sp->fp) == ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;
	    len = ga.ga_len + (ಪೂರ್ಣಾಂಕ)STRLEN(buf + ga.ga_len);
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_CRNL
	// Ignore a trailing CTRL-Z, when in Dos mode.	Only recognize the
	// CTRL-Z by its own, or after a NL.
	ಆದರೆ (	   (len == 1 || (len >= 2 && buf[len - 2] == '\n'))
		&& sp->fileformat == EOL_DOS
		&& buf[len - 1] == Ctrl_Z)
	{
	    buf[len - 1] = NUL;
	    ನಿಲ್ಲಿಸು;
	}
#ಆದರೆ_ಅಂತ್ಯ

	have_read = TRUE;
	ga.ga_len = len;

	// If the line was longer than the buffer, read more.  A line of a
	// buffer was taken as a whole.
	ಆದರೆ (!sp->ಮೂಲ_from_buf && ga.ga_maxlen - ga.ga_len == 1
						       && buf[len - 1] != '\n')
	    ಮುಂದುವರಿ;

	ಆದರೆ (len >= 1 && buf[len - 1] == '\n')	// remove trailing NL
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_CRNL
	    has_cr = (len >= 2 && buf[len - 2] == '\r');
	    ಆದರೆ (sp->fileformat == EOL_UNKNOWN)
	    {
		ಆದರೆ (has_cr)
		    sp->fileformat = EOL_DOS;
		ಇಲ್ಲದಿದ್ದರೆ
		    sp->fileformat = EOL_UNIX;
	    }

	    ಆದರೆ (sp->fileformat == EOL_DOS)
	    {
		ಆದರೆ (has_cr)	    // replace trailing CR
		{
		    buf[len - 2] = '\n';
		    --len;
		    --ga.ga_len;
		}
		ಇಲ್ಲದಿದ್ದರೆ	    // lines like ":map xx yy^M" will have failed
		{
		    ಆದರೆ (!sp->error)
		    {
			ಸಂದೇಶ_ಮೂಲ(HL_ATTR(HLF_W));
			emsg(_("W15: Warning: Wrong line separator, ^M may be missing"));
		    }
		    sp->error = TRUE;
		    sp->fileformat = EOL_UNIX;
		}
	    }
#ಆದರೆ_ಅಂತ್ಯ
	    // The '\n' is escaped if there is an odd number of ^V's just
	    // before it, first set "c" just before the 'V's and then check
	    // len&c parities (is faster than ((len-c)%2 == 0)) -- Acevedo
	    ಗಾಗಿ (c = len - 2; c >= 0 && buf[c] == Ctrl_V; c--)
		;
	    ಆದರೆ ((len & 1) != (c & 1))	// escaped NL, read more
	    {
		++sp->sourcing_lnum;
		ಮುಂದುವರಿ;
	    }

	    buf[len - 1] = NUL;		// remove the NL
	}

	// Check for ^C here now and then, so recursive :so can be broken.
	ಸಾಲು_breakcheck();
	ನಿಲ್ಲಿಸು;
    }

    ಆದರೆ (have_read)
	ಹಿಂತಿರುಗಿ (char_u *)ga.ga_ದತ್ತಾಂಶ;

    vim_free(ga.ga_ದತ್ತಾಂಶ);
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
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
	ಪೂರ್ಣಾಂಕ c ಬಳಸದ,
	ಶೂನ್ಯ *cookie,
	ಪೂರ್ಣಾಂಕ indent ಬಳಸದ,
	getline_opt_T options)
{
    ಮೂಲ_cookie_T	*sp = (ಮೂಲ_cookie_T *)cookie;
    char_u		*ಸಾಲು;
    char_u		*p;
    ಪೂರ್ಣಾಂಕ			do_vim9_all = ಒಳ_vim9script()
					      && options == GETLINE_CONCAT_ALL;
    ಪೂರ್ಣಾಂಕ			do_bar_cont = do_vim9_all
					 || options == GETLINE_CONCAT_CONTBAR;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    // If breakpoints have been added/deleted need to check for it.
    ಆದರೆ ((sp->dbg_tick < debug_tick) && !sp->ಮೂಲ_from_buf)
    {
	sp->breakpoint = dbg_ಕಂಡುಹಿಡಿ_breakpoint(TRUE, sp->fname, SOURCING_LNUM);
	sp->dbg_tick = debug_tick;
    }
# ifdef FEAT_PROFILE
    ಆದರೆ (do_profiling == PROF_YES)
	script_ಸಾಲು_ಕೊನೆ();
# endif
#ಆದರೆ_ಅಂತ್ಯ

    // Set the current sourcing line number.
    SOURCING_LNUM = sp->sourcing_lnum + 1;

    // Get current line.  If there is a read-ahead line, use it, otherwise get
    // one now.  "fp" is NULL if actually using a string.
    ಆದರೆ (sp->ಪೂರ್ಣಗೊಂಡ || (!sp->ಮೂಲ_from_buf && sp->fp == ಖಾಲಿ))
	ಸಾಲು = ಖಾಲಿ;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (sp->nextline == ಖಾಲಿ)
	ಸಾಲು = get_one_sourceline(sp);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಸಾಲು = sp->nextline;
	sp->nextline = ಖಾಲಿ;
	++sp->sourcing_lnum;
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
    ಆದರೆ (ಸಾಲು != ಖಾಲಿ && do_profiling == PROF_YES)
	script_ಸಾಲು_ಆರಂಭ();
#ಆದರೆ_ಅಂತ್ಯ

    // Only concatenate lines starting with a \ when 'cpoptions' doesn't
    // contain the 'C' flag.
    ಆದರೆ (ಸಾಲು != ಖಾಲಿ && options != GETLINE_NONE
				      && vim_strchr(p_cpo, CPO_CONCAT) == ಖಾಲಿ)
    {
	ಪೂರ್ಣಾಂಕ comment_char = ಒಳ_vim9script() ? '#' : '"';

	// compensate for the one line read-ahead
	--sp->sourcing_lnum;

	// Get the next line and concatenate it when it starts with a
	// backslash. We always need to read the next line, keep it in
	// sp->nextline.
	/* Also check for a comment in between continuation lines: "\ */
	// Also check for a Vim9 comment, empty line, line starting with '|',
	// but not "||".
	sp->nextline = get_one_sourceline(sp);
	ಆದರೆ (sp->nextline != ಖಾಲಿ
		&& (*(p = skipwhite(sp->nextline)) == '\\'
			      || (p[0] == comment_char
						&& p[1] == '\\' && p[2] == ' ')
			      || (do_vim9_all && (*p == NUL
						     || vim9_comment_ಆರಂಭ(p)))
			      || (do_bar_cont && p[0] == '|' && p[1] != '|')))
	{
	    garray_T    ga;

	    ga_init2(&ga, ಗಾತ್ರ(char_u), 400);
	    ga_concat(&ga, ಸಾಲು);
	    ಆದರೆ (*p == '\\')
		ga_concat(&ga, p + 1);
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == '|')
	    {
		GA_CONCAT_LITERAL(&ga, " ");
		ga_concat(&ga, p);
	    }
	    ಗಾಗಿ (;;)
	    {
		vim_free(sp->nextline);
		sp->nextline = get_one_sourceline(sp);
		ಆದರೆ (sp->nextline == ಖಾಲಿ)
		    ನಿಲ್ಲಿಸು;
		p = skipwhite(sp->nextline);
		ಆದರೆ (*p == '\\' || (do_bar_cont && p[0] == '|' && p[1] != '|'))
		{
		    // Adjust the growsize to the current length to speed up
		    // concatenating many lines.
		    ಆದರೆ (ga.ga_len > 400)
		    {
			ಆದರೆ (ga.ga_len > 8000)
			    ga.ga_growsize = 8000;
			ಇಲ್ಲದಿದ್ದರೆ
			    ga.ga_growsize = ga.ga_len;
		    }
		    ಆದರೆ (*p == '\\')
			ga_concat(&ga, p + 1);
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			GA_CONCAT_LITERAL(&ga, " ");
			ga_concat(&ga, p);
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!(p[0] == (comment_char)
						&& p[1] == '\\' && p[2] == ' ')
		     && !(do_vim9_all && (*p == NUL || vim9_comment_ಆರಂಭ(p))))
		    ನಿಲ್ಲಿಸು;
		/* drop a # comment or "\ comment line */
	    }
	    ga_append(&ga, NUL);
	    vim_free(ಸಾಲು);
	    ಸಾಲು = ga.ga_ದತ್ತಾಂಶ;
	}
    }

    ಆದರೆ (ಸಾಲು != ಖಾಲಿ && sp->conv.vc_type != CONV_NONE)
    {
	char_u	*s;

	// Convert the encoding of the script line.
	s = string_convert(&sp->conv, ಸಾಲು, ಖಾಲಿ);
	ಆದರೆ (s != ಖಾಲಿ)
	{
	    vim_free(ಸಾಲು);
	    ಸಾಲು = s;
	}
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    // Did we encounter a breakpoint?
    ಆದರೆ (!sp->ಮೂಲ_from_buf && sp->breakpoint != 0
	    && sp->breakpoint <= SOURCING_LNUM)
    {
	dbg_breakpoint(sp->fname, SOURCING_LNUM);
	// Find next breakpoint.
	sp->breakpoint = dbg_ಕಂಡುಹಿಡಿ_breakpoint(TRUE, sp->fname, SOURCING_LNUM);
	sp->dbg_tick = debug_tick;
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ ಸಾಲು;
}

/*
 * Returns TRUE if sourcing a script either from a file or a buffer.
 * Otherwise returns FALSE.
 */
    ಪೂರ್ಣಾಂಕ
sourcing_a_script(exarg_T *eap)
{
    ಹಿಂತಿರುಗಿ (getline_equal(eap->ea_getline, eap->cookie, getsourceline));
}

/*
 * ":scriptencoding": Set encoding conversion for a sourced script.
 */
    ಶೂನ್ಯ
ex_scriptencoding(exarg_T *eap)
{
    ಮೂಲ_cookie_T	*sp;
    char_u		*name;

    ಆದರೆ (!sourcing_a_script(eap))
    {
	emsg(_(e_scriptencoding_ಬಳಸಿದ_outside_ದ_sourced_file));
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (*eap->arg != NUL)
    {
	name = enc_canonize(eap->arg);
	ಆದರೆ (name == ಖಾಲಿ)	// out of memory
	    ಹಿಂತಿರುಗಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	name = eap->arg;

    // Setup for conversion from the specified encoding to 'encoding'.
    sp = (ಮೂಲ_cookie_T *)getline_cookie(eap->ea_getline, eap->cookie);
    convert_ಸಿದ್ಧತೆ(&sp->conv, name, p_enc);

    ಆದರೆ (name != eap->arg)
	vim_free(name);
}

/*
 * ":scriptversion": Set Vim script version for a sourced script.
 */
    ಶೂನ್ಯ
ex_scriptversion(exarg_T *eap ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ		nr;

    ಆದರೆ (!sourcing_a_script(eap))
    {
	emsg(_(e_scriptversion_ಬಳಸಿದ_outside_ದ_sourced_file));
	ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (ಒಳ_vim9script())
    {
	emsg(_(e_cannot_use_scriptversion_after_vim9script));
	ಹಿಂತಿರುಗಿ;
    }

    nr = getdigits(&eap->arg);
    ಆದರೆ (nr == 0 || *eap->arg != NUL)
	emsg(_(e_invalid_ವಾದ));
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (nr > SCRIPT_ಆವೃತ್ತಿ_MAX)
	semsg(_(e_scriptversion_not_supported_nr), nr);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	current_sctx.sc_ಆವೃತ್ತಿ = nr;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	SCRIPT_ಅಂಶ(current_sctx.sc_sid)->sn_ಆವೃತ್ತಿ = nr;
#ಆದರೆ_ಅಂತ್ಯ
    }
}

#ಆದರೆ defined(FEAT_EVAL)
/*
 * ":finish": Mark a sourced file as finished.
 */
    ಶೂನ್ಯ
ex_ಮುಕ್ತಾಯ(exarg_T *eap)
{
    ಆದರೆ (sourcing_a_script(eap))
	do_ಮುಕ್ತಾಯ(eap, FALSE);
    ಇಲ್ಲದಿದ್ದರೆ
	emsg(_(e_ಮುಕ್ತಾಯ_ಬಳಸಿದ_outside_ದ_sourced_file));
}

/*
 * Mark a sourced file as finished.  Possibly makes the ":finish" pending.
 * Also called for a pending finish at the ":endtry" or after returning from
 * an extra do_cmdline().  "reanimate" is used in the latter case.
 */
    ಶೂನ್ಯ
do_ಮುಕ್ತಾಯ(exarg_T *eap, ಪೂರ್ಣಾಂಕ reanimate)
{
    ಪೂರ್ಣಾಂಕ		idx;

    ಆದರೆ (reanimate)
	((ಮೂಲ_cookie_T *)getline_cookie(eap->ea_getline,
					      eap->cookie))->ಪೂರ್ಣಗೊಂಡ = FALSE;

    // Cleanup (and inactivate) conditionals, but stop when a try conditional
    // not in its finally clause (which then is to be executed next) is found.
    // In this case, make the ":finish" pending for execution at the ":endtry".
    // Otherwise, finish normally.
    idx = cleanup_conditionals(eap->cstack, 0, TRUE);
    ಆದರೆ (idx >= 0)
    {
	eap->cstack->cs_pending[idx] = CSTP_ಮುಕ್ತಾಯ;
	report_make_pending(CSTP_ಮುಕ್ತಾಯ, ಖಾಲಿ);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	((ಮೂಲ_cookie_T *)getline_cookie(eap->ea_getline,
					       eap->cookie))->ಪೂರ್ಣಗೊಂಡ = TRUE;
}


/*
 * Return TRUE when a sourced file had the ":finish" command: Don't give error
 * message for missing ":endif".
 * Return FALSE when not sourcing a file.
 */
    ಪೂರ್ಣಾಂಕ
ಮೂಲ_ಪೂರ್ಣಗೊಂಡ(
    char_u	*(*fgetline)(ಪೂರ್ಣಾಂಕ, ಶೂನ್ಯ *, ಪೂರ್ಣಾಂಕ, getline_opt_T),
    ಶೂನ್ಯ	*cookie)
{
    ಹಿಂತಿರುಗಿ (getline_equal(fgetline, cookie, getsourceline)
	    && ((ಮೂಲ_cookie_T *)getline_cookie(
						fgetline, cookie))->ಪೂರ್ಣಗೊಂಡ);
}

/*
 * Find the path of a script below the "autoload" directory.
 * Returns NULL if there is no "/autoload/" in the script name.
 */
    ಸ್ಥಿರ char_u *
script_name_after_autoload(scriptitem_T *si)
{
    char_u	*p = si->sn_name;
    char_u	*res = ಖಾಲಿ;

    ಗಾಗಿ (;;)
    {
	char_u *n = (char_u *)ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು((ಅಕ್ಷರ *)p, "autoload");

	ಆದರೆ (n == ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;
	ಆದರೆ (n > p && vim_ispathsep(n[-1]) && vim_ispathsep(n[8]))
	    res = n + 9;
	p = n + 8;
    }
    ಹಿಂತಿರುಗಿ res;
}

/*
 * For an autoload script "autoload/dir/script.vim" return the prefix
 * "dir#script#" in allocated memory.
 * Returns NULL if anything is wrong.
 */
    char_u *
get_autoload_ಪೂರ್ವಪ್ರತ್ಯಯ(scriptitem_T *si)
{
    char_u *p = script_name_after_autoload(si);
    char_u *ಪೂರ್ವಪ್ರತ್ಯಯ;

    ಆದರೆ (p == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಪೂರ್ವಪ್ರತ್ಯಯ = vim_strsave(p);
    ಆದರೆ (ಪೂರ್ವಪ್ರತ್ಯಯ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // replace all '/' with '#' and locate ".vim" at the end
    ಗಾಗಿ (p = ಪೂರ್ವಪ್ರತ್ಯಯ; *p != NUL; p += mb_ptr2len(p))
    {
	ಆದರೆ (vim_ispathsep(*p))
	    *p = '#';
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(p, ".vim") == 0)
	{
	    p[0] = '#';
	    p[1] = NUL;
	    ಹಿಂತಿರುಗಿ ಪೂರ್ವಪ್ರತ್ಯಯ;
	}
    }

    // did not find ".vim" at the end
    vim_free(ಪೂರ್ವಪ್ರತ್ಯಯ);
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * If in a Vim9 autoload script return "name" with the autoload prefix for the
 * script.  If successful the returned name is allocated.
 * Otherwise it returns "name" unmodified.
 */
    char_u *
may_ಪೂರ್ವಪ್ರತ್ಯಯ_autoload(char_u *name)
{
    ಆದರೆ (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	ಹಿಂತಿರುಗಿ name;

    scriptitem_T *si = SCRIPT_ಅಂಶ(current_sctx.sc_sid);

    ಆದರೆ (si->sn_autoload_ಪೂರ್ವಪ್ರತ್ಯಯ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ name;

    char_u  *basename = name;
    size_t  len;
    char_u  *res;

    ಆದರೆ (*name == K_SPECIAL)
    {
	char_u *p = vim_strchr(name, '_');

	// skip over "<SNR>99_"
	ಆದರೆ (p != ಖಾಲಿ)
	    basename = p + 1;
    }

    len = STRLEN(si->sn_autoload_ಪೂರ್ವಪ್ರತ್ಯಯ) + STRLEN(basename) + 2;
    res = alloc(len);
    ಆದರೆ (res == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    vim_snprintf((ಅಕ್ಷರ *)res, len, "%s%s", si->sn_autoload_ಪೂರ್ವಪ್ರತ್ಯಯ, basename);
    ಹಿಂತಿರುಗಿ res;
}

/*
 * Return the autoload script name for a function or variable name.
 * Returns NULL when out of memory.
 * Caller must make sure that "name" contains AUTOLOAD_CHAR.
 */
    char_u *
autoload_name(char_u *name)
{
    char_u	*p, *q = ಖಾಲಿ;
    char_u	*scriptname;

    // Get the script file name: replace '#' with '/', append ".vim".
    scriptname = alloc(STRLEN(name) + 14);
    ಆದರೆ (scriptname == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    STRCPY(scriptname, "autoload/");
    STRCAT(scriptname, name[0] == 'g' && name[1] == ':' ? name + 2: name);
    ಗಾಗಿ (p = scriptname + 9; (p = vim_strchr(p, AUTOLOAD_CHAR)) != ಖಾಲಿ;
								    q = p, ++p)
	*p = '/';
    STRCPY(q, ".vim");
    ಹಿಂತಿರುಗಿ scriptname;
}

/*
 * If "name" has a package name try autoloading the script for it.
 * Return TRUE if a package was loaded.
 */
    ಪೂರ್ಣಾಂಕ
script_autoload(
    char_u	*name,
    ಪೂರ್ಣಾಂಕ		reload)	    // load script again when already loaded
{
    char_u	*p;
    char_u	*scriptname, *tofree;
    ಪೂರ್ಣಾಂಕ		ret = FALSE;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		ret_sid;

    // If the name starts with "<SNR>123_" then "123" is the script ID.
    ಆದರೆ (name[0] == K_SPECIAL && name[1] == KS_EXTRA && name[2] == KE_SNR)
    {
	p = name + 3;
	ret_sid = (ಪೂರ್ಣಾಂಕ)getdigits(&p);
	ಆದರೆ (*p == '_' && SCRIPT_ID_VALID(ret_sid))
	{
	    may_ತುಂಬು_script(ret_sid, &ret);
	    ಹಿಂತಿರುಗಿ ret;
	}
    }

    // If there is no '#' after name[0] there is no package name.
    p = vim_strchr(name, AUTOLOAD_CHAR);
    ಆದರೆ (p == ಖಾಲಿ || p == name)
	ಹಿಂತಿರುಗಿ FALSE;

    tofree = scriptname = autoload_name(name);
    ಆದರೆ (scriptname == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;

    // Find the name in the list of previously loaded package names.  Skip
    // "autoload/", it's always the same.
    ಗಾಗಿ (i = 0; i < ga_loaded.ga_len; ++i)
	ಆದರೆ (STRCMP(((char_u **)ga_loaded.ga_ದತ್ತಾಂಶ)[i] + 9, scriptname + 9) == 0)
	    ನಿಲ್ಲಿಸು;
    ಆದರೆ (!reload && i < ga_loaded.ga_len)
	ret = FALSE;	    // was loaded already
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Remember the name if it wasn't loaded already.
	ಆದರೆ (i == ga_loaded.ga_len && ga_grow(&ga_loaded, 1) == OK)
	{
	    ((char_u **)ga_loaded.ga_ದತ್ತಾಂಶ)[ga_loaded.ga_len++] = scriptname;
	    tofree = ಖಾಲಿ;
	}

	// Try loading the package from $VIMRUNTIME/autoload/<name>.vim
	// Use "ret_sid" to avoid loading the same script again.
	ಆದರೆ (ಮೂಲ_ಒಳ_ಮಾರ್ಗ(p_rtp, scriptname, DIP_ಆರಂಭ, &ret_sid) == OK)
	    ret = TRUE;
    }

    vim_free(tofree);
    ಹಿಂತಿರುಗಿ ret;
}
#ಆದರೆ_ಅಂತ್ಯ
