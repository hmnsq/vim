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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ defined(FEAT_EVAL)
// The names of packages that once were loaded are remembered.
ਸਥਿਰ garray_T		ga_loaded = {0, 0, ਆਕਾਰ(char_u *), 4, ਨਿਰਾ};
#ਜੇ_ਅੰਤ

// last used sequence number for sourcing scripts (current_sctx.sc_seq)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
ਸਥਿਰ ਪੂਰਨ		ਆਖਰੀ_current_SID_seq = 0;
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਪੂਰਨ do_ਸਰੋਤ_ext(char_u *fname, ਪੂਰਨ check_other, ਪੂਰਨ is_vimrc, ਪੂਰਨ *ret_sid, exarg_T *eap, ਪੂਰਨ clearvars);

/*
 * Initialize the execution stack.
 */
    ਖਾਲੀ
estack_init(ਖਾਲੀ)
{
    estack_T *ਇੰਦਰਾਜ;

    ਜੇ (ga_grow(&exestack, 10) == FAIL)
	mch_exit(0);
    ਇੰਦਰਾਜ = ((estack_T *)exestack.ga_ਅੰਕੜੇ) + exestack.ga_len;
    ਇੰਦਰਾਜ->es_type = ETYPE_ਸਿਖਰ;
    ਇੰਦਰਾਜ->es_name = ਨਿਰਾ;
    ਇੰਦਰਾਜ->es_lnum = 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਇੰਦਰਾਜ->es_info.ufunc = ਨਿਰਾ;
#ਜੇ_ਅੰਤ
    ++exestack.ga_len;
}

/*
 * Add an item to the execution stack.
 * Returns the new entry or NULL when out of memory.
 */
    estack_T *
estack_ਧੱਕੋ(etype_T type, char_u *name, ਲੰਮਾ lnum)
{
    estack_T *ਇੰਦਰਾਜ;

    // If memory allocation fails then we'll pop more than we push, eventually
    // at the top level it will be OK again.
    ਜੇ (ga_grow(&exestack, 1) == FAIL)
	ਵਾਪਸ ਨਿਰਾ;

    ਇੰਦਰਾਜ = ((estack_T *)exestack.ga_ਅੰਕੜੇ) + exestack.ga_len;
    ਇੰਦਰਾਜ->es_type = type;
    ਇੰਦਰਾਜ->es_name = name;
    ਇੰਦਰਾਜ->es_lnum = lnum;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਇੰਦਰਾਜ->es_info.ufunc = ਨਿਰਾ;
#ਜੇ_ਅੰਤ
    ++exestack.ga_len;
    ਵਾਪਸ ਇੰਦਰਾਜ;
}

#ਜੇ defined(FEAT_EVAL)
/*
 * Add a user function to the execution stack.
 */
    estack_T *
estack_ਧੱਕੋ_ufunc(ufunc_T *ufunc, ਲੰਮਾ lnum)
{
    estack_T *ਇੰਦਰਾਜ = estack_ਧੱਕੋ(ETYPE_UFUNC,
	    ufunc->uf_name_exp != ਨਿਰਾ
				  ? ufunc->uf_name_exp : ufunc->uf_name, lnum);
    ਜੇ (ਇੰਦਰਾਜ != ਨਿਰਾ)
	ਇੰਦਰਾਜ->es_info.ufunc = ufunc;
    ਵਾਪਸ ਇੰਦਰਾਜ;
}

/*
 * Return TRUE if "ufunc" with "lnum" is already at the top of the exe stack.
 */
    ਪੂਰਨ
estack_ਸਿਖਰ_is_ufunc(ufunc_T *ufunc, ਲੰਮਾ lnum)
{
    estack_T *ਇੰਦਰਾਜ;

    ਜੇ (exestack.ga_len == 0)
	ਵਾਪਸ FALSE;
    ਇੰਦਰਾਜ = ((estack_T *)exestack.ga_ਅੰਕੜੇ) + exestack.ga_len - 1;
    ਵਾਪਸ ਇੰਦਰਾਜ->es_type == ETYPE_UFUNC
	&& STRCMP( ਇੰਦਰਾਜ->es_name, ufunc->uf_name_exp != ਨਿਰਾ
				    ? ufunc->uf_name_exp : ufunc->uf_name) == 0
	&& ਇੰਦਰਾਜ->es_lnum == lnum;
}
#ਜੇ_ਅੰਤ

/*
 * Take an item off of the execution stack and return it.
 */
    estack_T *
estack_ਕੱਢੋ(ਖਾਲੀ)
{
    ਜੇ (exestack.ga_len == 0)
	ਵਾਪਸ ਨਿਰਾ;
    --exestack.ga_len;
    ਵਾਪਸ ((estack_T *)exestack.ga_ਅੰਕੜੇ) + exestack.ga_len;
}

/*
 * Get the current value for "which" in allocated memory.
 * "which" is ESTACK_SFILE for <sfile>, ESTACK_STACK for <stack> or
 * ESTACK_SCRIPT for <script>.
 */
    char_u *
estack_sfile(estack_arg_T which ਅਣਵਰਤਿਆ)
{
    estack_T	*ਇੰਦਰਾਜ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    garray_T	ga;
    size_t	len;
    ਪੂਰਨ		idx;
    etype_T	ਆਖਰੀ_type = ETYPE_SCRIPT;
#ਜੇ_ਅੰਤ

    ਇੰਦਰਾਜ = ((estack_T *)exestack.ga_ਅੰਕੜੇ) + exestack.ga_len - 1;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਜੇ (which == ESTACK_SFILE && ਇੰਦਰਾਜ->es_type != ETYPE_UFUNC)
#ਜੇ_ਅੰਤ
    {
	ਜੇ (ਇੰਦਰਾਜ->es_name == ਨਿਰਾ)
	    ਵਾਪਸ ਨਿਰਾ;
	ਵਾਪਸ vim_strsave(ਇੰਦਰਾਜ->es_name);
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    // expand('<sfile>') works in a function for backwards compatibility, but
    // may give an unexpected result.  Disallow it in Vim 9 script.
    ਜੇ (which == ESTACK_SFILE && ਵਿੱਚ_vim9script())
    {
	ਪੂਰਨ  save_emsg_off = emsg_off;

	ਜੇ (emsg_off == 1)
	    // f_expand() silences errors but we do want this one
	    emsg_off = 0;
	emsg(_(e_cannot_expand_sfile_ਵਿੱਚ_vim9_function));
	emsg_off = save_emsg_off;
	ਵਾਪਸ ਨਿਰਾ;
    }

    // If evaluated in a function or autocommand, return the path of the script
    // where it is defined, at script level the current script path is returned
    // instead.
    ਜੇ (which == ESTACK_SCRIPT)
    {
	// Walk the stack backwards, starting from the current frame.
	ਲਈ (idx = exestack.ga_len - 1; idx >= 0; --idx, --ਇੰਦਰਾਜ)
	{
	    ਜੇ (ਇੰਦਰਾਜ->es_type == ETYPE_UFUNC || ਇੰਦਰਾਜ->es_type == ETYPE_AUCMD)
	    {
		sctx_T *def_ctx = ਇੰਦਰਾਜ->es_type == ETYPE_UFUNC
			      ? &ਇੰਦਰਾਜ->es_info.ufunc->uf_script_ctx
			      : ਇੰਦਰਾਜ->es_info.aucmd != ਨਿਰਾ
				  ? acp_script_ctx(ਇੰਦਰਾਜ->es_info.aucmd) : ਨਿਰਾ;

		ਵਾਪਸ def_ctx != ਨਿਰਾ && def_ctx->sc_sid > 0
			   ? vim_strsave(SCRIPT_ਵਸਤੂ(def_ctx->sc_sid)->sn_name)
			   : ਨਿਰਾ;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (ਇੰਦਰਾਜ->es_type == ETYPE_SCRIPT)
		ਵਾਪਸ vim_strsave(ਇੰਦਰਾਜ->es_name);
	}
	ਵਾਪਸ ਨਿਰਾ;
    }

    // Give information about each stack entry up to the root.
    // For a function we compose the call stack, as it was done in the past:
    //   "function One[123]..Two[456]..Three"
    ga_init2(&ga, ਆਕਾਰ(ਅੱਖਰ), 100);
    ਲਈ (idx = 0; idx < exestack.ga_len; ++idx)
    {
	ਇੰਦਰਾਜ = ((estack_T *)exestack.ga_ਅੰਕੜੇ) + idx;
	ਜੇ (ਇੰਦਰਾਜ->es_name != ਨਿਰਾ)
	{
	    ਲੰਮਾ	lnum = 0;
	    size_t	added = 0;
	    string_T	type_name = {(char_u *)"", 0};
	    string_T	class_name = {(char_u *)"", 0};
	    string_T	es_name = {ਇੰਦਰਾਜ->es_name, STRLEN(ਇੰਦਰਾਜ->es_name)};

	    ਜੇ (ਇੰਦਰਾਜ->es_type != ਆਖਰੀ_type)
	    {
		ਚੋਣ (ਇੰਦਰਾਜ->es_type)
		{
		    ਹਾਲਤ ETYPE_SCRIPT:
			STR_LITERAL_ਸੈੱਟ(type_name, "script ");
			ਰੋਕੋ;
		    ਹਾਲਤ ETYPE_UFUNC:
			STR_LITERAL_ਸੈੱਟ(type_name, "function ");
			ਰੋਕੋ;
		    ਮੂਲ:
			ਰੋਕੋ;
		}
		ਆਖਰੀ_type = ਇੰਦਰਾਜ->es_type;
	    }
	    ਜੇ (ਇੰਦਰਾਜ->es_type == ETYPE_UFUNC && ਇੰਦਰਾਜ->es_info.ufunc->uf_class != ਨਿਰਾ)
	    {
		class_name.string = ਇੰਦਰਾਜ->es_info.ufunc->uf_class->class_name.string;
		class_name.ਲੰਮਾਈ = ਇੰਦਰਾਜ->es_info.ufunc->uf_class->class_name.ਲੰਮਾਈ;
	    }
	    ਜੇ (idx == exestack.ga_len - 1)
		lnum = which == ESTACK_ਢੇਰ ? SOURCING_LNUM : 0;
	    ਨਹੀਂਤਾਂ
		lnum = ਇੰਦਰਾਜ->es_lnum;

	    len = es_name.ਲੰਮਾਈ + type_name.ਲੰਮਾਈ + class_name.ਲੰਮਾਈ + 26;
	    ਜੇ (ga_grow(&ga, (ਪੂਰਨ)len) == FAIL)
		ਰੋਕੋ;
	    ga_concat_len(&ga, type_name.string, type_name.ਲੰਮਾਈ);
	    // For class methods prepend "<class name>." to the function name.
	    ਜੇ (*class_name.string != NUL)
	    {
		added = vim_snprintf_safelen(
		    (ਅੱਖਰ *)ga.ga_ਅੰਕੜੇ + ga.ga_len,
		    ga.ga_maxlen - ga.ga_len,
		    "<SNR>%d_%s.",
		    ਇੰਦਰਾਜ->es_info.ufunc->uf_script_ctx.sc_sid,
		    class_name.string);

		ga.ga_len += (ਪੂਰਨ)added;
	    }
	    ga_concat_len(&ga, es_name.string, es_name.ਲੰਮਾਈ);
	    // For the bottom entry of <sfile>: do not add the line number, it is used in
	    // <slnum>.  Also leave it out when the number is not set.
	    ਜੇ (lnum != 0)
	    {
		added = vim_snprintf_safelen(
		    (ਅੱਖਰ *)ga.ga_ਅੰਕੜੇ + ga.ga_len,
		    ga.ga_maxlen - ga.ga_len,
		    "[%ld]",
		    lnum);

		ga.ga_len += (ਪੂਰਨ)added;
	    }
	    ਜੇ (idx != exestack.ga_len - 1)
		GA_CONCAT_LITERAL(&ga, "..");
	}
    }

    // Only NUL-terminate when not returning NULL.
    ਜੇ (ga.ga_ਅੰਕੜੇ != ਨਿਰਾ)
	ga_append(&ga, NUL);
    ਵਾਪਸ (char_u *)ga.ga_ਅੰਕੜੇ;
#ਜੇ_ਅੰਤ
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਸਥਿਰ ਖਾਲੀ
stacktrace_ਧੱਕੋ_ਵਸਤੂ(
	list_T		*l,
	ufunc_T		*fp,
	char_u		*event,
	linenr_T	lnum,
	char_u		*filepath)
{
    dict_T	*d;
    typval_T	tv;

    d = dict_alloc_ਤਾਲਾ(VAR_FIXED);
    ਜੇ (d == ਨਿਰਾ)
	ਵਾਪਸ;

    tv.v_type = VAR_DICT;
    tv.v_ਤਾਲਾ = VAR_LOCKED;
    tv.vval.v_dict = d;

    ਜੇ (fp != ਨਿਰਾ)
	dict_ਜੋੜੋ_func(d, "funcref", fp);
    ਜੇ (event != ਨਿਰਾ)
	dict_ਜੋੜੋ_string(d, "event", event);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(d, "lnum", lnum);
    dict_ਜੋੜੋ_string(d, "filepath", filepath);

    list_append_tv(l, &tv);
}

/*
 * Create the stacktrace from exestack.
 */
    list_T *
stacktrace_create(ਖਾਲੀ)
{
    list_T	*l;
    ਪੂਰਨ		i;

    l = list_alloc();
    ਜੇ (l == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    ਲਈ (i = 0; i < exestack.ga_len; ++i)
    {
	estack_T *ਇੰਦਰਾਜ = &((estack_T *)exestack.ga_ਅੰਕੜੇ)[i];
	linenr_T lnum = ਇੰਦਰਾਜ->es_lnum;

	ਜੇ (ਇੰਦਰਾਜ->es_type == ETYPE_SCRIPT)
	    stacktrace_ਧੱਕੋ_ਵਸਤੂ(l, ਨਿਰਾ, ਨਿਰਾ, lnum, ਇੰਦਰਾਜ->es_name);
	ਨਹੀਂਤਾਂ ਜੇ (ਇੰਦਰਾਜ->es_type == ETYPE_UFUNC)
	{
	    ufunc_T *fp = ਇੰਦਰਾਜ->es_info.ufunc;
	    sctx_T sctx = fp->uf_script_ctx;
	    char_u *filepath = sctx.sc_sid > 0 ?
				   get_scriptname(sctx.sc_sid) : (char_u *)"";

	    lnum += sctx.sc_lnum;
	    stacktrace_ਧੱਕੋ_ਵਸਤੂ(l, fp, ਨਿਰਾ, lnum, filepath);
	}
	ਨਹੀਂਤਾਂ ਜੇ (ਇੰਦਰਾਜ->es_type == ETYPE_AUCMD)
	{
	    // The autocmd may not have a matching pattern yet, in which case
	    // es_info.aucmd is still NULL.
	    sctx_T *sctx = ਇੰਦਰਾਜ->es_info.aucmd != ਨਿਰਾ
			       ? acp_script_ctx(ਇੰਦਰਾਜ->es_info.aucmd) : ਨਿਰਾ;
	    char_u *filepath = sctx != ਨਿਰਾ && sctx->sc_sid > 0 ?
				   get_scriptname(sctx->sc_sid) : (char_u *)"";

	    ਜੇ (sctx != ਨਿਰਾ)
		lnum += sctx->sc_lnum;
	    stacktrace_ਧੱਕੋ_ਵਸਤੂ(l, ਨਿਰਾ, ਇੰਦਰਾਜ->es_name, lnum, filepath);
	}
    }
    ਵਾਪਸ l;
}

/*
 * getstacktrace() function
 */
    ਖਾਲੀ
f_getstacktrace(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    rettv_list_ਸੈੱਟ(rettv, stacktrace_create());
}
#ਜੇ_ਅੰਤ

/*
 * Get DIP_ flags from the [where] argument of a :runtime command.
 * "*argp" is advanced to after the [where] argument if it is found.
 */
    ਸਥਿਰ ਪੂਰਨ
get_runtime_cmd_flags(char_u **argp, size_t where_len)
{
    char_u *arg = *argp;

    ਜੇ (where_len == 0)
	ਵਾਪਸ 0;

    ਜੇ (STRNCMP(arg, "START", where_len) == 0)
    {
	*argp = skipwhite(arg + where_len);
	ਵਾਪਸ DIP_ਸ਼ੁਰੂ + DIP_NORTP;
    }
    ਜੇ (STRNCMP(arg, "OPT", where_len) == 0)
    {
	*argp = skipwhite(arg + where_len);
	ਵਾਪਸ DIP_OPT + DIP_NORTP;
    }
    ਜੇ (STRNCMP(arg, "PACK", where_len) == 0)
    {
	*argp = skipwhite(arg + where_len);
	ਵਾਪਸ DIP_ਸ਼ੁਰੂ + DIP_OPT + DIP_NORTP;
    }
    ਜੇ (STRNCMP(arg, "ALL", where_len) == 0)
    {
	*argp = skipwhite(arg + where_len);
	ਵਾਪਸ DIP_ਸ਼ੁਰੂ + DIP_OPT;
    }

    ਵਾਪਸ 0;
}

/*
 * ":runtime [where] {name}"
 */
    ਖਾਲੀ
ex_runtime(exarg_T *eap)
{
    char_u  *arg = eap->arg;
    ਪੂਰਨ	    flags = eap->forceit ? DIP_ALL : 0;
    char_u  *p = skiptowhite(arg);
    flags += get_runtime_cmd_flags(&arg, p - arg);
    ਸਰੋਤ_runtime(arg, flags);
}

ਸਥਿਰ ਪੂਰਨ runtime_expand_flags;

/*
 * Set the completion context for the :runtime command.
 */
    ਖਾਲੀ
ਸੈੱਟ_context_ਵਿੱਚ_runtime_cmd(expand_T *xp, char_u *arg)
{
    char_u  *p = skiptowhite(arg);
    runtime_expand_flags
	= *p != NUL ? get_runtime_cmd_flags(&arg, p - arg) : 0;
    // Skip to the last argument.
    ਜਦੋਂ (*(p = skiptowhite_esc(arg)) != NUL)
    {
	ਜੇ (runtime_expand_flags == 0)
	    // When there are multiple arguments and [where] is not specified,
	    // use an unrelated non-zero flag to avoid expanding [where].
	    runtime_expand_flags = DIP_ALL;
	arg = skipwhite(p);
    }
    xp->xp_context = EXPAND_RUNTIME;
    xp->xp_ਨਮੂਨਾ = arg;
}

    ਸਥਿਰ ਖਾਲੀ
ਸਰੋਤ_callback(char_u *fname, ਖਾਲੀ *cookie)
{
    (ਖਾਲੀ)do_ਸਰੋਤ(fname, FALSE, DOSO_NONE, cookie);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
/*
 * Find an already loaded script "name".
 * If found returns its script ID.  If not found returns -1.
 */
    ਪੂਰਨ
ਲੱਭੋ_script_by_name(char_u *name)
{
    ਪੂਰਨ		    sid;
    scriptitem_T    *si;

    ਲਈ (sid = script_ਚੀਜ਼ਾਂ.ga_len; sid > 0; --sid)
    {
	// We used to check inode here, but that doesn't work:
	// - If a script is edited and written, it may get a different
	//   inode number, even though to the user it is the same script.
	// - If a script is deleted and another script is written, with a
	//   different name, the inode may be re-used.
	si = SCRIPT_ਵਸਤੂ(sid);
	ਜੇ (si->sn_name != ਨਿਰਾ && fnamecmp(si->sn_name, name) == 0)
	    ਵਾਪਸ sid;
    }
    ਵਾਪਸ -1;
}

/*
 * Add a new scriptitem with all items initialized.
 * When running out of memory "error" is set to FAIL.
 * Returns the script ID.
 */
    ਸਥਿਰ ਪੂਰਨ
get_new_scriptitem(ਪੂਰਨ *error)
{
    ਸਥਿਰ scid_T   ਆਖਰੀ_current_SID = 0;
    ਪੂਰਨ		    sid = ++ਆਖਰੀ_current_SID;
    scriptitem_T    *si = ਨਿਰਾ;

    ਜੇ (ga_grow(&script_ਚੀਜ਼ਾਂ, (ਪੂਰਨ)(sid - script_ਚੀਜ਼ਾਂ.ga_len)) == FAIL)
    {
	*error = FAIL;
	ਵਾਪਸ sid;
    }
    ਜਦੋਂ (script_ਚੀਜ਼ਾਂ.ga_len < sid)
    {
	si = ALLOC_CLEAR_ONE(scriptitem_T);
	ਜੇ (si == ਨਿਰਾ)
	{
	    *error = FAIL;
	    ਵਾਪਸ sid;
	}
	++script_ਚੀਜ਼ਾਂ.ga_len;
	SCRIPT_ਵਸਤੂ(script_ਚੀਜ਼ਾਂ.ga_len) = si;
	si->sn_name = ਨਿਰਾ;
	si->sn_ਸੰਸਕਰਣ = 1;

	// Allocate the local script variables to use for this script.
	new_script_vars(script_ਚੀਜ਼ਾਂ.ga_len);
	ga_init2(&si->sn_var_vals, ਆਕਾਰ(svar_T), 10);
	hash_init(&si->sn_all_vars.dv_hashtab);
	ga_init2(&si->sn_imports, ਆਕਾਰ(imported_T), 10);
	ga_init2(&si->sn_type_list, ਆਕਾਰ(type_T), 10);
# ifdef FEAT_PROFILE
	si->sn_prof_on = FALSE;
# endif
    }

    // "si" can't be NULL, check only to avoid a compiler warning
    ਜੇ (si != ਨਿਰਾ)
	// Used to check script variable index is still valid.
	si->sn_script_seq = current_sctx.sc_seq;

    ਵਾਪਸ sid;
}

    ਪੂਰਨ
get_new_scriptitem_for_fname(ਪੂਰਨ *error, char_u *fname)
{
    ਪੂਰਨ sid = get_new_scriptitem(error);

    ਜੇ (*error == OK)
    {
	scriptitem_T *si = SCRIPT_ਵਸਤੂ(sid);

	si->sn_name = vim_strsave(fname);
	si->sn_state = SN_STATE_NOT_LOADED;
    }
    ਵਾਪਸ sid;
}

/*
 * If the script for "sid" is a symlink and "sn_source_sid" is not set
 * then initialize it. A new script_item is created if needed.
 */
    ਖਾਲੀ
check_script_symlink(ਪੂਰਨ sid)
{
    scriptitem_T *si = SCRIPT_ਵਸਤੂ(sid);
    ਜੇ (si->sn_syml_checked || si->sn_sourced_sid > 0)
	ਵਾਪਸ;
    si->sn_syml_checked = TRUE;

    // If fname is a symbolic link, create an script_item for the real file.

    char_u *real_fname = fix_fname(si->sn_name);
    ਜੇ (real_fname != ਨਿਰਾ && STRCMP(real_fname, si->sn_name) != 0)
    {
	ਪੂਰਨ real_sid = ਲੱਭੋ_script_by_name(real_fname);
	ਪੂਰਨ error2 = OK;
	ਪੂਰਨ new_sid = FALSE;
	ਜੇ (real_sid < 0)
	{
	    real_sid = get_new_scriptitem_for_fname(&error2, real_fname);
	    new_sid = TRUE;
	}
	ਜੇ (error2 == OK)
	{
	    si = SCRIPT_ਵਸਤੂ(sid);
	    si->sn_sourced_sid = real_sid;
	    ਜੇ (new_sid)
	    {
		SCRIPT_ਵਸਤੂ(real_sid)->sn_import_autoload
						    = si->sn_import_autoload;
		ਜੇ (si->sn_autoload_ਅਗੇਤਰ != ਨਿਰਾ)
		    SCRIPT_ਵਸਤੂ(real_sid)->sn_autoload_ਅਗੇਤਰ =
					vim_strsave(si->sn_autoload_ਅਗੇਤਰ);
	    }
	}
    }
    vim_free(real_fname);
}

    ਸਥਿਰ ਖਾਲੀ
ਲੱਭੋ_script_callback(char_u *fname, ਖਾਲੀ *cookie)
{
    ਪੂਰਨ sid;
    ਪੂਰਨ error = OK;
    ਪੂਰਨ *ret_sid = cookie;

    sid = ਲੱਭੋ_script_by_name(fname);
    ਜੇ (sid < 0)
	// script does not exist yet, create a new scriptitem
	sid = get_new_scriptitem_for_fname(&error, fname);
    *ret_sid = sid;
}
#ਜੇ_ਅੰਤ

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
    ਪੂਰਨ
do_ਵਿੱਚ_ਰਸਤਾ(
    char_u	*ਰਸਤਾ,
    ਅੱਖਰ	*ਅਗੇਤਰ,
    char_u	*name,
    ਪੂਰਨ		flags,
    ਖਾਲੀ	(*callback)(char_u *fname, ਖਾਲੀ *ck),
    ਖਾਲੀ	*cookie)
{
    char_u	*rtp;
    string_T	buf;
    char_u	*rtp_copy;
    ਪੂਰਨ		ਅੰਕ_files;
    char_u	**files;
    ਪੂਰਨ		i;
    ਪੂਰਨ		did_one = FALSE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ AMIGA
    ਬਣਤਰ Process	*proc = (ਬਣਤਰ Process *)ਲੱਭੋ_Task(0L);
    APTR		save_winptr = proc->pr_ਖਿੜਕੀ_Ptr;

    // Avoid a requester here for a volume that doesn't exist.
    proc->pr_ਖਿੜਕੀ_Ptr = (APTR)-1L;
#ਜੇ_ਅੰਤ

    // Make a copy of 'runtimepath'.  Invoking the callback may change the
    // value.
    rtp_copy = vim_strsave(ਰਸਤਾ);
    buf.string = alloc(MAXPATHL);
    ਜੇ (buf.string != ਨਿਰਾ && rtp_copy != ਨਿਰਾ)
    {
	size_t	prefixlen = 0;
	size_t	namelen = 0;

	ਜੇ (name != ਨਿਰਾ)
	{
	    prefixlen = STRLEN(ਅਗੇਤਰ);
	    namelen = STRLEN(name);

	    ਜੇ (p_verbose > 10)
	    {
		verbose_enter();
		ਜੇ (*ਅਗੇਤਰ != NUL)
		    smsg(_("Searching for \"%s\" under \"%s\" in \"%s\""),
					       (ਅੱਖਰ *)name, ਅਗੇਤਰ, (ਅੱਖਰ *)ਰਸਤਾ);
		ਨਹੀਂਤਾਂ
		    smsg(_("Searching for \"%s\" in \"%s\""),
						       (ਅੱਖਰ *)name, (ਅੱਖਰ *)ਰਸਤਾ);
		verbose_leave();
	    }
	}

	// Loop over all entries in 'runtimepath'.
	rtp = rtp_copy;
	ਜਦੋਂ (*rtp != NUL && ((flags & DIP_ALL) || !did_one))
	{
	    // Copy the path from 'runtimepath' to buf[].
	    buf.ਲੰਮਾਈ = (size_t)copy_option_part(&rtp, buf.string, MAXPATHL, ",");

	    // Skip after or non-after directories.
	    ਜੇ (flags & (DIP_NOAFTER | DIP_AFTER))
	    {
		ਪੂਰਨ is_after = buf.ਲੰਮਾਈ >= 5
		    && STRCMP(buf.string + buf.ਲੰਮਾਈ - 5, "after") == 0;

		ਜੇ ((is_after && (flags & DIP_NOAFTER))
			|| (!is_after && (flags & DIP_AFTER)))
		    ਜਾਰੀ;
	    }

	    ਜੇ (name == ਨਿਰਾ)
	    {
		(*callback)(buf.string, (ਖਾਲੀ *)&cookie);
		ਜੇ (!did_one)
		    did_one = (cookie == ਨਿਰਾ);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (buf.ਲੰਮਾਈ + 2 + prefixlen + namelen < MAXPATHL)
	    {
		char_u	*np;
		char_u	*ਪੂਛ;

		ਜੇ (*buf.string != NUL
		    && !after_pathsep(buf.string, buf.string + buf.ਲੰਮਾਈ))
		{
		    STRCPY(buf.string + buf.ਲੰਮਾਈ, PATHSEPSTR);
		    buf.ਲੰਮਾਈ += ਆਕਾਰ(PATHSEP);
		}
		STRCPY(buf.string + buf.ਲੰਮਾਈ, ਅਗੇਤਰ);
		buf.ਲੰਮਾਈ += prefixlen;
		ਪੂਛ = buf.string + buf.ਲੰਮਾਈ;

		// Loop over all patterns in "name"
		np = name;
		ਜਦੋਂ (*np != NUL && ((flags & DIP_ALL) || !did_one))
		{
		    // Append the pattern from "name" to buf[].
		    copy_option_part(&np, ਪੂਛ, (ਪੂਰਨ)(MAXPATHL - (ਪੂਛ - buf.string)),
								       "\t ");

		    ਜੇ (p_verbose > 10)
		    {
			verbose_enter();
			smsg(_("Searching for \"%s\""), buf.string);
			verbose_leave();
		    }

		    // Expand wildcards, invoke the callback for each match.
		    ਜੇ (gen_expand_wildcards(1, &buf.string, &ਅੰਕ_files, &files,
				  (flags & DIP_DIR) ? EW_DIR : EW_FILE) == OK)
		    {
			ਲਈ (i = 0; i < ਅੰਕ_files; ++i)
			{
			    (*callback)(files[i], cookie);
			    did_one = TRUE;
			    ਜੇ (!(flags & DIP_ALL))
				ਰੋਕੋ;
			}
			FreeWild(ਅੰਕ_files, files);
		    }
		}
	    }
	}
    }
    vim_free(buf.string);
    vim_free(rtp_copy);
    ਜੇ (!did_one && name != ਨਿਰਾ)
    {
	ਅੱਖਰ *basepath = ਰਸਤਾ == p_rtp ? "runtimepath" : "packpath";

	ਜੇ (flags & DIP_ERR)
	    semsg(_(e_directory_not_ਮਿਲਿਆ_ਵਿੱਚ_str_str), basepath, name);
	ਨਹੀਂਤਾਂ ਜੇ (p_verbose > 0)
	{
	    verbose_enter();
	    smsg(_("not found in '%s': \"%s\""), basepath, name);
	    verbose_leave();
	}
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ AMIGA
    proc->pr_ਖਿੜਕੀ_Ptr = save_winptr;
#ਜੇ_ਅੰਤ

    ਵਾਪਸ did_one ? OK : FAIL;
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
    ਸਥਿਰ ਪੂਰਨ
do_ਵਿੱਚ_ਰਸਤਾ_and_pp(
    char_u	*ਰਸਤਾ,
    char_u	*name,
    ਪੂਰਨ		flags,
    ਖਾਲੀ	(*callback)(char_u *fname, ਖਾਲੀ *ck),
    ਖਾਲੀ	*cookie)
{
    ਪੂਰਨ		ਪੂਰਾ = FAIL;

    ਜੇ ((flags & DIP_NORTP) == 0)
	ਪੂਰਾ = do_ਵਿੱਚ_ਰਸਤਾ(ਰਸਤਾ, "", name, flags, callback, cookie);

    ਜੇ ((ਪੂਰਾ == FAIL || (flags & DIP_ALL)) && (flags & DIP_ਸ਼ੁਰੂ))
	ਪੂਰਾ = do_ਵਿੱਚ_ਰਸਤਾ(p_pp, "pack/*/start/*/", name, flags, callback,
								       cookie);

    ਜੇ ((ਪੂਰਾ == FAIL || (flags & DIP_ALL)) && (flags & DIP_OPT))
	ਪੂਰਾ = do_ਵਿੱਚ_ਰਸਤਾ(p_pp, "pack/*/opt/*/", name, flags, callback,
								       cookie);

    ਵਾਪਸ ਪੂਰਾ;
}

/*
 * Just like do_in_path_and_pp(), using 'runtimepath' for "path".
 */
    ਪੂਰਨ
do_ਵਿੱਚ_runtimepath(
    char_u	*name,
    ਪੂਰਨ		flags,
    ਖਾਲੀ	(*callback)(char_u *fname, ਖਾਲੀ *ck),
    ਖਾਲੀ	*cookie)
{
    ਵਾਪਸ do_ਵਿੱਚ_ਰਸਤਾ_and_pp(p_rtp, name, flags, callback, cookie);
}

/*
 * Source the file "name" from all directories in 'runtimepath'.
 * "name" can contain wildcards.
 * When "flags" has DIP_ALL: source all files, otherwise only the first one.
 *
 * return FAIL when no file could be sourced, OK otherwise.
 */
    ਪੂਰਨ
ਸਰੋਤ_runtime(char_u *name, ਪੂਰਨ flags)
{
    ਵਾਪਸ ਸਰੋਤ_ਵਿੱਚ_ਰਸਤਾ(p_rtp, name, flags, ਨਿਰਾ);
}

/*
 * Just like source_runtime(), but use "path" instead of 'runtimepath'
 * and return the script ID in "ret_sid".
 */
    ਪੂਰਨ
ਸਰੋਤ_ਵਿੱਚ_ਰਸਤਾ(char_u *ਰਸਤਾ, char_u *name, ਪੂਰਨ flags, ਪੂਰਨ *ret_sid)
{
    ਵਾਪਸ do_ਵਿੱਚ_ਰਸਤਾ_and_pp(ਰਸਤਾ, name, flags, ਸਰੋਤ_callback, ret_sid);
}

#ਜੇ defined(FEAT_EVAL)

/*
 * Find "name" in 'runtimepath'. If found a new scriptitem is created for it
 * and it's script ID is returned.
 * If not found returns -1.
 */
    ਪੂਰਨ
ਲੱਭੋ_script_ਵਿੱਚ_rtp(char_u *name)
{
    ਪੂਰਨ sid = -1;

    (ਖਾਲੀ)do_ਵਿੱਚ_ਰਸਤਾ_and_pp(p_rtp, name, DIP_ਸ਼ੁਰੂ | DIP_NOAFTER,
						   ਲੱਭੋ_script_callback, &sid);
    ਵਾਪਸ sid;
}

/*
 * Expand wildcards in "pat" and invoke do_source() for each match.
 */
    ਸਥਿਰ ਖਾਲੀ
ਸਰੋਤ_all_matches(char_u *pat)
{
    ਪੂਰਨ	    ਅੰਕ_files;
    char_u  **files;
    ਪੂਰਨ	    i;

    ਜੇ (gen_expand_wildcards(1, &pat, &ਅੰਕ_files, &files, EW_FILE) != OK)
	ਵਾਪਸ;

    ਲਈ (i = 0; i < ਅੰਕ_files; ++i)
	(ਖਾਲੀ)do_ਸਰੋਤ(files[i], FALSE, DOSO_NONE, ਨਿਰਾ);
    FreeWild(ਅੰਕ_files, files);
}

/*
 * Add the package directory to 'runtimepath'.
 */
    ਸਥਿਰ ਪੂਰਨ
ਜੋੜੋ_pack_dir_ਮੰਜ਼ਿਲ_rtp(char_u *fname)
{
    char_u  *p4, *p3, *p2, *p1, *p;
    char_u  *ਇੰਦਰਾਜ;
    char_u  *insp = ਨਿਰਾ;
    ਪੂਰਨ	    c;
    string_T  new_rtp;
    size_t  keep;
    size_t  p_rtp_len;
    string_T  afterdir = {ਨਿਰਾ, 0};
    char_u  *after_insp = ਨਿਰਾ;
    char_u  *ffname = ਨਿਰਾ;
    size_t  fname_len;
    string_T	buf = {ਨਿਰਾ, 0};
    ਪੂਰਨ	    retval = FAIL;

    p4 = p3 = p2 = p1 = get_past_ਸਿਰ(fname);
    ਲਈ (p = p1; *p; MB_PTR_ADV(p))
	ਜੇ (vim_ispathsep_nocolon(*p))
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
    ਜੇ (ffname == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    // Find "ffname" in "p_rtp", ignoring '/' vs '\' differences.
    // Also stop at the first "after" directory.
    fname_len = STRLEN(ffname);
    buf.string = alloc(MAXPATHL);
    ਜੇ (buf.string == ਨਿਰਾ)
	ਜਾਓ theend;

    p_rtp_len = 0;
    ਲਈ (ਇੰਦਰਾਜ = p_rtp; *ਇੰਦਰਾਜ != NUL; )
    {
	char_u *cur_ਇੰਦਰਾਜ = ਇੰਦਰਾਜ;

	buf.ਲੰਮਾਈ = (size_t)copy_option_part(&ਇੰਦਰਾਜ, buf.string, MAXPATHL, ",");

	// keep track of p_rtp length as we go to make the STRLEN() below have less work to do
	p_rtp_len += (*(cur_ਇੰਦਰਾਜ + buf.ਲੰਮਾਈ) == ',') ? buf.ਲੰਮਾਈ + 1 : buf.ਲੰਮਾਈ;

	ਜੇ ((p = (char_u *)ਲੜੀ_ਖੋਜ((ਅੱਖਰ *)buf.string, "after")) != ਨਿਰਾ
		&& p > buf.string
		&& vim_ispathsep(p[-1])
		&& (vim_ispathsep(p[5]) || p[5] == NUL || p[5] == ','))
	{
	    ਜੇ (insp == ਨਿਰਾ)
		// Did not find "ffname" before the first "after" directory,
		// insert it before this entry.
		insp = cur_ਇੰਦਰਾਜ;
	    after_insp = cur_ਇੰਦਰਾਜ;
	    ਰੋਕੋ;
	}

	ਜੇ (insp == ਨਿਰਾ)
	{
	    char_u  *rtp_ffname;
	    ਪੂਰਨ	    ਮੇਲ;

	    ਜੇ (*buf.string != NUL && !after_pathsep(buf.string, buf.string + buf.ਲੰਮਾਈ))
		STRCPY(buf.string + buf.ਲੰਮਾਈ, PATHSEPSTR);
	    rtp_ffname = fix_fname(buf.string);
	    ਜੇ (rtp_ffname == ਨਿਰਾ)
		ਜਾਓ theend;
	    ਮੇਲ = vim_fnamencmp(rtp_ffname, ffname, fname_len) == 0;
	    vim_free(rtp_ffname);
	    ਜੇ (ਮੇਲ)
		// Insert "ffname" after this entry (and comma).
		insp = ਇੰਦਰਾਜ;
	}
    }

    // finish measuring the length of p_rtp
    p_rtp_len += STRLEN(p_rtp + p_rtp_len);
    ਜੇ (insp == ਨਿਰਾ)
	// Both "fname" and "after" not found, append at the end.
	insp = p_rtp + p_rtp_len;

    // check if rtp/pack/name/start/name/after exists
    fname_len = STRLEN(fname);
    concat_fnames(fname, fname_len, (char_u *)"after", STRLEN_LITERAL("after"), TRUE, &afterdir);
    ਜੇ (afterdir.string == ਨਿਰਾ || !mch_isdir(afterdir.string))
	afterdir.ਲੰਮਾਈ = 0;

    new_rtp.string = alloc(p_rtp_len + fname_len + afterdir.ਲੰਮਾਈ + 3); // add two for commas and one for NUL
    ਜੇ (new_rtp.string == ਨਿਰਾ)
	ਜਾਓ theend;

    // We now have 'rtp' parts: {keep}{keep_after}{rest}.
    // Create new_rtp, first: {keep},{fname}
    keep = (size_t)(insp - p_rtp);
    mch_memmove(new_rtp.string, p_rtp, keep);
    new_rtp.ਲੰਮਾਈ = keep;
    ਜੇ (*insp == NUL)
	new_rtp.string[new_rtp.ਲੰਮਾਈ++] = ',';  // add comma before
    mch_memmove(new_rtp.string + new_rtp.ਲੰਮਾਈ, fname, fname_len);
    new_rtp.ਲੰਮਾਈ += fname_len;
    ਜੇ (*insp != NUL)
	new_rtp.string[new_rtp.ਲੰਮਾਈ++] = ',';  // add comma after

    ਜੇ (afterdir.ਲੰਮਾਈ > 0 && after_insp != ਨਿਰਾ)
    {
	size_t keep_after = (size_t)(after_insp - p_rtp);
	size_t append_len = keep_after - keep;

	// Add to new_rtp: {keep},{fname}{keep_after},{afterdir}
	mch_memmove(new_rtp.string + new_rtp.ਲੰਮਾਈ, p_rtp + keep,
							append_len);
	new_rtp.ਲੰਮਾਈ += append_len;
	mch_memmove(new_rtp.string + new_rtp.ਲੰਮਾਈ, afterdir.string, afterdir.ਲੰਮਾਈ);
	new_rtp.ਲੰਮਾਈ += afterdir.ਲੰਮਾਈ;
	new_rtp.string[new_rtp.ਲੰਮਾਈ++] = ',';
	keep = keep_after;
    }

    ਜੇ (p_rtp[keep] != NUL)
    {
	size_t	append_len = p_rtp_len - keep;

	// Append rest: {keep},{fname}{keep_after},{afterdir}{rest}
	mch_memmove(new_rtp.string + new_rtp.ਲੰਮਾਈ, p_rtp + keep, append_len + 1);	// add one for NUL
	new_rtp.ਲੰਮਾਈ += append_len;
    }
    ਨਹੀਂਤਾਂ
	new_rtp.string[new_rtp.ਲੰਮਾਈ] = NUL;

    ਜੇ (afterdir.ਲੰਮਾਈ > 0 && after_insp == ਨਿਰਾ)
    {
	// Append afterdir when "after" was not found:
	// {keep},{fname}{rest},{afterdir}
	new_rtp.string[new_rtp.ਲੰਮਾਈ++] = ',';
	STRCPY(new_rtp.string + new_rtp.ਲੰਮਾਈ, afterdir.string);
    }

    ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u *)"rtp", 0L, new_rtp.string, 0);
    vim_free(new_rtp.string);
    retval = OK;

theend:
    vim_free(buf.string);
    vim_free(ffname);
    vim_free(afterdir.string);
    ਵਾਪਸ retval;
}

/*
 * Load scripts in "plugin" and "ftdetect" directories of the package.
 */
    ਸਥਿਰ ਪੂਰਨ
ਲੱਦੋ_pack_plugin(char_u *fname)
{
    ਸਥਿਰ ਅੱਖਰ *plugpat = "%s/plugin/**/*.vim";
    ਸਥਿਰ ਅੱਖਰ *ftpat = "%s/ftdetect/*.vim";
    ਪੂਰਨ		len;
    char_u	*ffname = fix_fname(fname);
    char_u	*pat = ਨਿਰਾ;
    ਪੂਰਨ		retval = FAIL;

    ਜੇ (ffname == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    len = (ਪੂਰਨ)STRLEN(ffname) + (ਪੂਰਨ)STRLEN(ftpat);
    pat = alloc(len);
    ਜੇ (pat == ਨਿਰਾ)
	ਜਾਓ theend;
    vim_snprintf((ਅੱਖਰ *)pat, len, plugpat, ffname);
    ਸਰੋਤ_all_matches(pat);

    {
	char_u *cmd = vim_strsave((char_u *)"g:did_load_filetypes");

	// If runtime/filetype.vim wasn't loaded yet, the scripts will be
	// found when it loads.
	ਜੇ (cmd != ਨਿਰਾ && eval_ਮੰਜ਼ਿਲ_ਸੰਖਿਆ(cmd, FALSE) > 0)
	{
	    do_cmdline_cmd((char_u *)"augroup filetypedetect");
	    vim_snprintf((ਅੱਖਰ *)pat, len, ftpat, ffname);
	    ਸਰੋਤ_all_matches(pat);
	    do_cmdline_cmd((char_u *)"augroup END");
	}
	vim_free(cmd);
    }
    vim_free(pat);
    retval = OK;

theend:
    vim_free(ffname);
    ਵਾਪਸ retval;
}

// used for "cookie" of add_pack_plugin()
ਸਥਿਰ ਪੂਰਨ APP_ਜੋੜੋ_DIR;
ਸਥਿਰ ਪੂਰਨ APP_ਲੱਦੋ;
ਸਥਿਰ ਪੂਰਨ APP_BOTH;

    ਸਥਿਰ ਖਾਲੀ
ਜੋੜੋ_pack_plugin(char_u *fname, ਖਾਲੀ *cookie)
{
    ਜੇ (cookie != &APP_ਲੱਦੋ)
    {
	char_u	*buf = alloc(MAXPATHL);
	char_u	*p;
	ਪੂਰਨ	ਮਿਲਿਆ = FALSE;

	ਜੇ (buf == ਨਿਰਾ)
	    ਵਾਪਸ;
	p = p_rtp;
	ਜਦੋਂ (*p != NUL)
	{
	    copy_option_part(&p, buf, MAXPATHL, ",");
	    ਜੇ (pathcmp((ਅੱਖਰ *)buf, (ਅੱਖਰ *)fname, -1) == 0)
	    {
		ਮਿਲਿਆ = TRUE;
		ਰੋਕੋ;
	    }
	}
	vim_free(buf);
	ਜੇ (!ਮਿਲਿਆ)
	    // directory is not yet in 'runtimepath', add it
	    ਜੇ (ਜੋੜੋ_pack_dir_ਮੰਜ਼ਿਲ_rtp(fname) == FAIL)
		ਵਾਪਸ;
    }

    ਜੇ (cookie != &APP_ਜੋੜੋ_DIR)
	ਲੱਦੋ_pack_plugin(fname);
}

/*
 * Add all packages in the "start" directory to 'runtimepath'.
 */
    ਖਾਲੀ
ਜੋੜੋ_pack_ਸ਼ੁਰੂ_dirs(ਖਾਲੀ)
{
    do_ਵਿੱਚ_ਰਸਤਾ(p_pp, "", (char_u *)"pack/*/start/*", DIP_ALL + DIP_DIR,
					       ਜੋੜੋ_pack_plugin, &APP_ਜੋੜੋ_DIR);
}

/*
 * Load plugins from all packages in the "start" directory.
 */
    ਖਾਲੀ
ਲੱਦੋ_ਸ਼ੁਰੂ_packages(ਖਾਲੀ)
{
    did_ਸਰੋਤ_packages = TRUE;
    do_ਵਿੱਚ_ਰਸਤਾ(p_pp, "", (char_u *)"pack/*/start/*", DIP_ALL + DIP_DIR,
						  ਜੋੜੋ_pack_plugin, &APP_ਲੱਦੋ);
}

/*
 * ":packloadall"
 * Find plugins in the package directories and source them.
 */
    ਖਾਲੀ
ex_packloadall(exarg_T *eap)
{
    ਜੇ (!did_ਸਰੋਤ_packages || eap->forceit)
    {
	// First do a round to add all directories to 'runtimepath', then load
	// the plugins. This allows for plugins to use an autoload directory
	// of another plugin.
	ਜੋੜੋ_pack_ਸ਼ੁਰੂ_dirs();
	ਲੱਦੋ_ਸ਼ੁਰੂ_packages();
    }
}

/*
 * ":packadd[!] {name}"
 */
    ਖਾਲੀ
ex_packadd(exarg_T *eap)
{
    ਸਥਿਰ ਅੱਖਰ *plugpat = "pack/*/%s/%s";
    ਪੂਰਨ		len;
    ਅੱਖਰ	*pat;
    ਪੂਰਨ		ਗੇੜਾ;
    ਪੂਰਨ		res = OK;

    // Round 1: use "start", round 2: use "opt".
    ਲਈ (ਗੇੜਾ = 1; ਗੇੜਾ <= 2; ++ਗੇੜਾ)
    {
	// Only look under "start" when loading packages wasn't done yet.
	ਜੇ (ਗੇੜਾ == 1 && did_ਸਰੋਤ_packages)
	    ਜਾਰੀ;

	len = (ਪੂਰਨ)STRLEN(plugpat) + (ਪੂਰਨ)STRLEN(eap->arg) + 5;
	pat = alloc(len);
	ਜੇ (pat == ਨਿਰਾ)
	    ਵਾਪਸ;
	vim_snprintf(pat, len, plugpat, ਗੇੜਾ == 1 ? "start" : "opt", eap->arg);
	// The first round don't give a "not found" error, in the second round
	// only when nothing was found in the first round.
	res = do_ਵਿੱਚ_ਰਸਤਾ(p_pp, "", (char_u *)pat,
		DIP_ALL + DIP_DIR + (ਗੇੜਾ == 2 && res == FAIL ? DIP_ERR : 0),
		ਜੋੜੋ_pack_plugin, eap->forceit ? &APP_ਜੋੜੋ_DIR : &APP_BOTH);
	vim_free(pat);
    }
}
#ਜੇ_ਅੰਤ

/*
 * Sort "gap" and remove duplicate entries.  "gap" is expected to contain a
 * list of file names in allocated memory.
 */
    ਖਾਲੀ
remove_duplicates(garray_T *gap)
{
    ਪੂਰਨ	    i;
    ਪੂਰਨ	    j;
    char_u  **fnames = (char_u **)gap->ga_ਅੰਕੜੇ;

    ਛਾਂਟੋ_ਲੜੀਆਂ(fnames, gap->ga_len);
    ਲਈ (i = gap->ga_len - 1; i > 0; --i)
	ਜੇ (fnamecmp(fnames[i - 1], fnames[i]) == 0)
	{
	    vim_free(fnames[i]);
	    ਲਈ (j = i + 1; j < gap->ga_len; ++j)
		fnames[j - 1] = fnames[j];
	    --gap->ga_len;
	}
}

    ਸਥਿਰ ਖਾਲੀ
ExpandRTDir_int(
    char_u	*pat,
    size_t	pat_len,
    ਪੂਰਨ		flags,
    ਪੂਰਨ		keep_ext,
    garray_T	*gap,
    ਅੱਖਰ	*dirnames[])
{
    ਲਈ (ਪੂਰਨ i = 0; dirnames[i] != ਨਿਰਾ; ++i)
    {
	ਅਚਲ size_t	buf_len = STRLEN(dirnames[i]) + pat_len + 64;
	ਅੱਖਰ		*buf = alloc(buf_len);
	ਜੇ (buf == ਨਿਰਾ)
	{
	    ga_clear_ਲੜੀਆਂ(gap);
	    ਵਾਪਸ;
	}
	ਪੂਰਨ		glob_flags = 0;
	ਪੂਰਨ		expand_dirs = FALSE;

	// Build base pattern
	vim_snprintf(buf, buf_len, "%s%s%s%s",
		     *dirnames[i] ? dirnames[i] : "", *dirnames[i] ? "/" : "",
		     pat, "*.vim");

expand:
	ਜੇ ((flags & DIP_NORTP) == 0)
	    globpath(p_rtp, (char_u *)buf, gap, glob_flags, expand_dirs);

	ਜੇ (flags & DIP_ਸ਼ੁਰੂ)
	{
	    // Build complete search path: pack/*/start/*/dirnames[i]/pat*.vim
	    vim_snprintf(buf, buf_len, "pack/*/start/*/%s%s%s%s",
			 *dirnames[i] ? dirnames[i] : "",
			 *dirnames[i] ? "/" : "",
			 pat,
			 expand_dirs ? "*" : "*.vim");
	    globpath(p_pp, (char_u *)buf, gap, glob_flags, expand_dirs);
	}

	ਜੇ (flags & DIP_OPT)
	{
	    // Build complete search path: pack/*/opt/*/dirnames[i]/pat*.vim
	    vim_snprintf(buf, buf_len, "pack/*/opt/*/%s%s%s%s",
			 *dirnames[i] ? dirnames[i] : "",
			 *dirnames[i] ? "/" : "", pat,
			 expand_dirs ? "*" : "*.vim");
	    globpath(p_pp, (char_u *)buf, gap, glob_flags, expand_dirs);
	}

	// Second round for directories
	ਜੇ (*dirnames[i] == NUL && !expand_dirs)
	{
	    // expand dir names in another round
	    vim_snprintf(buf, buf_len, "%s*", pat);
	    glob_flags = WILD_ਜੋੜੋ_SLASH;
	    expand_dirs = TRUE;
	    ਜਾਓ expand;
	}

	vim_free(buf);
    }

    ਪੂਰਨ pat_pathsep_cnt = 0;
    ਲਈ (size_t i = 0; i < pat_len; ++i)
    {
	ਜੇ (vim_ispathsep(pat[i]))
	    ++pat_pathsep_cnt;
    }

    ਲਈ (ਪੂਰਨ i = 0; i < gap->ga_len; ++i)
    {
	char_u *ਮੇਲ = ((char_u **)gap->ga_ਅੰਕੜੇ)[i];
	char_u *s = ਮੇਲ;
	char_u *e = s + STRLEN(s);
	ਜੇ (e - 4 > s && !keep_ext && STRNICMP(e - 4, ".vim", 4) == 0)
	{
	    e -= 4;
	    *e = NUL;
	}

	ਪੂਰਨ ਮੇਲ_pathsep_cnt = (e > s && e[-1] == '/') ? -1 : 0;
	ਲਈ (s = e; s > ਮੇਲ; MB_PTR_ਪਿੱਠ(ਮੇਲ, s))
	{
	    ਜੇ (s < ਮੇਲ || (vim_ispathsep(*s)
				     && ++ਮੇਲ_pathsep_cnt > pat_pathsep_cnt))
		ਰੋਕੋ;
	}
	++s;
	ਜੇ (s != ਮੇਲ)
	    mch_memmove(ਮੇਲ, s, e - s + 1);
    }

    ਜੇ (gap->ga_len == 0)
	ਵਾਪਸ;

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
    ਪੂਰਨ
ExpandRTDir(
    char_u	*pat,
    ਪੂਰਨ		flags,
    ਪੂਰਨ		*ਅੰਕ_file,
    char_u	***file,
    ਅੱਖਰ	*dirnames[])
{
    *ਅੰਕ_file = 0;
    *file = ਨਿਰਾ;

    garray_T	ga;
    ga_init2(&ga, ਆਕਾਰ(ਅੱਖਰ *), 10);

    ExpandRTDir_int(pat, STRLEN(pat), flags, FALSE, &ga, dirnames);

    ਜੇ (ga.ga_len == 0)
	ਵਾਪਸ FAIL;

    *file = ga.ga_ਅੰਕੜੇ;
    *ਅੰਕ_file = ga.ga_len;
    ਵਾਪਸ OK;
}

/*
 * Handle command line completion for :runtime command.
 */
    ਪੂਰਨ
expand_runtime_cmd(char_u *pat, ਪੂਰਨ *ਅੰਕ_Matches, char_u ***matches)
{
    *ਅੰਕ_Matches = 0;
    *matches = ਨਿਰਾ;

    garray_T	ga;
    ga_init2(&ga, ਆਕਾਰ(ਅੱਖਰ *), 10);

    size_t pat_len = (ਪੂਰਨ)STRLEN(pat);
    ਅੱਖਰ *dirnames[] = {"", ਨਿਰਾ};
    ExpandRTDir_int(pat, pat_len, runtime_expand_flags, TRUE, &ga, dirnames);

    // Try to complete values for [where] argument when none was found.
    ਜੇ (runtime_expand_flags == 0)
    {
	ਅੱਖਰ *where_ਮੁੱਲਾਂ[] = {"START", "OPT", "PACK", "ALL"};
	ਲਈ (size_t i = 0; i < ARRAY_ਲੰਮਾਈ(where_ਮੁੱਲਾਂ); ++i)
	    ਜੇ (STRNCMP(pat, where_ਮੁੱਲਾਂ[i], pat_len) == 0)
	    {
		char_u *p = vim_strsave((char_u *)where_ਮੁੱਲਾਂ[i]);
		ਜੇ (p != ਨਿਰਾ && ga_ਜੋੜੋ_string(&ga, p) == FAIL)
		    vim_free(p);
	    }
    }

    ਜੇ (ga.ga_len == 0)
	ਵਾਪਸ FAIL;

    *matches = ga.ga_ਅੰਕੜੇ;
    *ਅੰਕ_Matches = ga.ga_len;
    ਵਾਪਸ OK;
}

/*
 * Expand loadplugin names:
 * 'packpath'/pack/ * /opt/{pat}
 */
    ਪੂਰਨ
Expand_Pack_ਜੋੜੋ_Dir(
    char_u	*pat,
    ਪੂਰਨ		*ਅੰਕ_file,
    char_u	***file)
{
    char_u	*s;
    char_u	*e;
    char_u	*ਮੇਲ;
    garray_T	ga;
    ਪੂਰਨ		i;
    ਪੂਰਨ		pat_len;

    *ਅੰਕ_file = 0;
    *file = ਨਿਰਾ;
    pat_len = (ਪੂਰਨ)STRLEN(pat);
    ga_init2(&ga, ਆਕਾਰ(ਅੱਖਰ *), 10);

    s = alloc(pat_len + 26);
    ਜੇ (s == ਨਿਰਾ)
    {
	ga_clear_ਲੜੀਆਂ(&ga);
	ਵਾਪਸ FAIL;
    }
    sprintf((ਅੱਖਰ *)s, "pack/*/opt/%s*", pat);
    globpath(p_pp, s, &ga, 0, TRUE);
    vim_free(s);

    ਲਈ (i = 0; i < ga.ga_len; ++i)
    {
	ਮੇਲ = ((char_u **)ga.ga_ਅੰਕੜੇ)[i];
	s = gettail(ਮੇਲ);
	e = s + STRLEN(s);
	mch_memmove(ਮੇਲ, s, e - s + 1);
    }

    ਜੇ (ga.ga_len == 0)
	ਵਾਪਸ FAIL;

    // Sort and remove duplicates which can happen when specifying multiple
    // directories in dirnames.
    remove_duplicates(&ga);

    *file = ga.ga_ਅੰਕੜੇ;
    *ਅੰਕ_file = ga.ga_len;
    ਵਾਪਸ OK;
}

    ਸਥਿਰ ਖਾਲੀ
cmd_ਸਰੋਤ(char_u *fname, exarg_T *eap)
{
    ਪੂਰਨ clearvars = FALSE;

    ਜੇ (*fname != NUL && STRNCMP(fname, "++clear", 7) == 0)
    {
	// ++clear argument is supplied
	clearvars = TRUE;
	fname = fname + 7;
	ਜੇ (*fname != NUL)
	{
	    semsg(_(e_invalid_ਦਲੀਲ_str), eap->arg);
	    ਵਾਪਸ;
	}
    }

    ਜੇ (*fname != NUL && eap != ਨਿਰਾ && eap->addr_ਗਿਣਤੀ > 0)
    {
	// if a filename is specified to :source, then a range is not allowed
	emsg(_(e_no_range_allowed));
	ਵਾਪਸ;
    }

    ਜੇ (eap != ਨਿਰਾ && *fname == NUL)
    {
	ਜੇ (eap->forceit)
	    // a file name is needed to source normal mode commands
	    emsg(_(e_ਦਲੀਲ_required));
	ਨਹੀਂਤਾਂ
	    // source ex commands from the current buffer
	    do_ਸਰੋਤ_ext(ਨਿਰਾ, FALSE, DOSO_NONE, ਨਿਰਾ, eap, clearvars);
    }
    ਨਹੀਂਤਾਂ ਜੇ (eap != ਨਿਰਾ && eap->forceit)
	// ":source!": read Normal mode commands
	// Need to execute the commands directly.  This is required at least
	// for:
	// - ":g" command busy
	// - after ":argdo", ":windo" or ":bufdo"
	// - another command follows
	// - inside a loop
	openscript(fname, global_busy || listcmd_busy || eap->nextcmd != ਨਿਰਾ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
						 || eap->cstack->cs_idx >= 0
#ਜੇ_ਅੰਤ
						 );

    // ":source" read ex commands
    ਨਹੀਂਤਾਂ ਜੇ (do_ਸਰੋਤ(fname, FALSE, DOSO_NONE, ਨਿਰਾ) == FAIL)
	semsg(_(e_cant_open_file_str), fname);
}

/*
 * ":source {fname}"
 */
    ਖਾਲੀ
ex_ਸਰੋਤ(exarg_T *eap)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BROWSE
    ਜੇ (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	char_u *fname = ਨਿਰਾ;

	fname = do_browse(0, (char_u *)_("Source Vim script"), eap->arg,
				      ਨਿਰਾ, ਨਿਰਾ,
				      (char_u *)_(BROWSE_FILTER_MACROS), ਨਿਰਾ);
	ਜੇ (fname != ਨਿਰਾ)
	{
	    cmd_ਸਰੋਤ(fname, eap);
	    vim_free(fname);
	}
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	cmd_ਸਰੋਤ(eap->arg, eap);
}

#ਜੇ defined(FEAT_EVAL)
/*
 * ":options"
 */
    ਖਾਲੀ
ex_options(
    exarg_T	*eap ਅਣਵਰਤਿਆ)
{
    char_u  buf[500];
    ਪੂਰਨ	    multi_mods = 0;

    buf[0] = NUL;
    (ਖਾਲੀ)ਜੋੜੋ_win_cmd_modifiers(buf, &cmdmod, &multi_mods);

    vim_setenv((char_u *)"OPTWIN_CMD", buf);
    cmd_ਸਰੋਤ((char_u *)SYS_OPTWIN_FILE, ਨਿਰਾ);
}
#ਜੇ_ਅੰਤ

/*
 * ":source" and associated commands.
 */

#ਜੇ defined(FEAT_EVAL)
/*
 * Return the address holding the next breakpoint line for a source cookie.
 */
    linenr_T *
ਸਰੋਤ_breakpoint(ਖਾਲੀ *cookie)
{
    ਵਾਪਸ &((ਸਰੋਤ_cookie_T *)cookie)->breakpoint;
}

/*
 * Return the address holding the debug tick for a source cookie.
 */
    ਪੂਰਨ *
ਸਰੋਤ_dbg_tick(ਖਾਲੀ *cookie)
{
    ਵਾਪਸ &((ਸਰੋਤ_cookie_T *)cookie)->dbg_tick;
}

/*
 * Return the nesting level for a source cookie.
 */
    ਪੂਰਨ
ਸਰੋਤ_level(ਖਾਲੀ *cookie)
{
    ਵਾਪਸ ((ਸਰੋਤ_cookie_T *)cookie)->level;
}

/*
 * Return the readahead line. Note that the pointer may become invalid when
 * getting the next line, if it's concatenated with the next one.
 */
    char_u *
ਸਰੋਤ_nextline(ਖਾਲੀ *cookie)
{
    ਵਾਪਸ ((ਸਰੋਤ_cookie_T *)cookie)->nextline;
}
#ਜੇ_ਅੰਤ

#ਜੇ (defined(MSWIN) && defined(FEAT_CSCOPE)) || defined(HAVE_FD_CLOEXEC)
# define USE_FOPEN_NOINH
/*
 * Special function to open a file without handle inheritance.
 * When possible the handle is closed on exec().
 */
    ਸਥਿਰ FILE *
fopen_noinh_readbin(ਅੱਖਰ *filename)
{
# ifdef MSWIN
    ਪੂਰਨ	fd_tmp = mch_open(filename, O_RDONLY | O_ਦੁਆਧਾਰੀ | O_NOINHERIT, 0);
# ਨਹੀਂਤਾਂ
    ਪੂਰਨ	fd_tmp = mch_open(filename, O_RDONLY, 0);
# endif

    ਜੇ (fd_tmp == -1)
	ਵਾਪਸ ਨਿਰਾ;

# ifdef HAVE_FD_CLOEXEC
    {
	ਪੂਰਨ fdflags = fcntl(fd_tmp, F_GETFD);
	ਜੇ (fdflags >= 0 && (fdflags & FD_CLOEXEC) == 0)
	    (ਖਾਲੀ)fcntl(fd_tmp, F_SETFD, fdflags | FD_CLOEXEC);
    }
# endif

    ਵਾਪਸ fdopen(fd_tmp, READBIN);
}
#ਜੇ_ਅੰਤ

/*
 * Initialization for sourcing lines from the current buffer. Reads all the
 * lines from the buffer and stores it in the cookie grow array.
 * Returns a pointer to the name ":source buffer=<n>" on success and NULL on
 * failure.
 */
    ਸਥਿਰ char_u *
do_ਸਰੋਤ_ਬਫਰ_init(ਸਰੋਤ_cookie_T *sp, exarg_T *eap)
{
    linenr_T	curr_lnum;
    char_u	*ਸਤਰ = ਨਿਰਾ;
    char_u	*fname;

    ਜੇ (curbuf == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    // Use ":source buffer=<num>" as the script name
    ਜੇ (curbuf->b_ffname != ਨਿਰਾ)
	fname = vim_strsave(curbuf->b_ffname);
    ਨਹੀਂਤਾਂ
    {
	vim_snprintf((ਅੱਖਰ *)IObuff, IOSIZE, ":source buffer=%d", curbuf->b_fnum);
	fname = vim_strsave(IObuff);
    }
    ਜੇ (fname == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    ga_init2(&sp->buflines, ਆਕਾਰ(char_u *), 100);

    // Copy the lines from the buffer into a grow array
    ਲਈ (curr_lnum = eap->line1; curr_lnum <= eap->line2; curr_lnum++)
    {
	ਸਤਰ = vim_strsave(ml_get(curr_lnum));
	ਜੇ (ਸਤਰ == ਨਿਰਾ)
	    ਜਾਓ errret;
	ਜੇ (ga_ਜੋੜੋ_string(&sp->buflines, ਸਤਰ) == FAIL)
	    ਜਾਓ errret;
    }
    sp->buf_lnum = 0;
    sp->ਸਰੋਤ_from_buf = TRUE;
    // When sourcing a range of lines from a buffer, use buffer line number.
    sp->sourcing_lnum = eap->line1 - 1;

    ਵਾਪਸ fname;

errret:
    vim_free(fname);
    vim_free(ਸਤਰ);
    ga_clear_ਲੜੀਆਂ(&sp->buflines);
    ਵਾਪਸ ਨਿਰਾ;
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
    ਸਥਿਰ ਪੂਰਨ
do_ਸਰੋਤ_ext(
    char_u	*fname,
    ਪੂਰਨ		check_other,	    // check for .vimrc and _vimrc
    ਪੂਰਨ		is_vimrc,	    // DOSO_ value
    ਪੂਰਨ		*ret_sid ਅਣਵਰਤਿਆ,
    exarg_T	*eap,
    ਪੂਰਨ		clearvars ਅਣਵਰਤਿਆ)
{
    ਸਰੋਤ_cookie_T	    cookie;
    char_u		    *p;
    char_u		    *fname_not_fixed = ਨਿਰਾ;
    char_u		    *fname_exp = ਨਿਰਾ;
    char_u		    *firstline = ਨਿਰਾ;
    ਪੂਰਨ			    retval = FAIL;
    sctx_T		    save_current_sctx;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ STARTUPTIME
    ਬਣਤਰ timeval	    tv_rel;
    ਬਣਤਰ timeval	    tv_ਸ਼ੁਰੂ;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROFILE
    proftime_T		    wait_ਸ਼ੁਰੂ;
#ਜੇ_ਅੰਤ
    ਪੂਰਨ			    save_sticky_cmdmod_flags = sticky_cmdmod_flags;
    ਪੂਰਨ			    trigger_ਸਰੋਤ_post = FALSE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    funccal_ਇੰਦਰਾਜ_T	    funccalp_ਇੰਦਰਾਜ;
    ਪੂਰਨ			    save_debug_break_level = debug_break_level;
    ਪੂਰਨ			    sid = -1;
    scriptitem_T	    *si = ਨਿਰਾ;
    ਪੂਰਨ			    save_estack_compiling = estack_compiling;
    ESTACK_CHECK_DECLARATION;
#ਜੇ_ਅੰਤ

    CLEAR_ਖੇਤਰ(cookie);
    ਜੇ (fname == ਨਿਰਾ)
    {
	// sourcing lines from a buffer
	fname_exp = do_ਸਰੋਤ_ਬਫਰ_init(&cookie, eap);
	ਜੇ (fname_exp == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;
    }
    ਨਹੀਂਤਾਂ
    {
	fname_not_fixed = expand_env_save(fname);
	ਜੇ (fname_not_fixed == ਨਿਰਾ)
	    ਜਾਓ theend;
	fname_exp = fix_fname(fname_not_fixed);
	ਜੇ (fname_exp == ਨਿਰਾ)
	    ਜਾਓ theend;
	ਜੇ (mch_isdir(fname_exp))
	{
	    smsg(_("Cannot source a directory: \"%s\""), fname);
	    ਜਾਓ theend;
	}
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    estack_compiling = FALSE;

    // See if we loaded this script before.
    sid = ਲੱਭੋ_script_by_name(fname_exp);
    ਜੇ (sid > 0 && ret_sid != ਨਿਰਾ
			  && SCRIPT_ਵਸਤੂ(sid)->sn_state != SN_STATE_NOT_LOADED)
    {
	// Already loaded and no need to load again, return here.
	*ret_sid = sid;
	retval = OK;
	ਜਾਓ theend;
    }
#ਜੇ_ਅੰਤ

    // Apply SourceCmd autocommands, they should get the file and source it.
    // When sourcing a file "fname_exp" was made a full path name by
    // fix_fname() above, avoid expanding it again.
    ਜੇ ((fname == ਨਿਰਾ
		? has_autocmd(EVENT_SOURCECMD, fname_exp, ਨਿਰਾ)
		: has_autocmd_fullname(EVENT_SOURCECMD, fname_exp, ਨਿਰਾ))
	    && apply_autocmds(EVENT_SOURCECMD, fname_exp, fname_exp,
							       FALSE, curbuf))
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	retval = aborting() ? FAIL : OK;
#ਨਹੀਂਤਾਂ
	retval = OK;
#ਜੇ_ਅੰਤ
	ਜੇ (retval == OK)
	    // Apply SourcePost autocommands.
	    apply_autocmds(EVENT_SOURCEPOST, fname_exp, fname_exp,
								FALSE, curbuf);
	ਜਾਓ theend;
    }

    // Apply SourcePre autocommands, they may get the file.
    apply_autocmds(EVENT_SOURCEPRE, fname_exp, fname_exp, FALSE, curbuf);

    ਜੇ (!cookie.ਸਰੋਤ_from_buf)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_FOPEN_NOINH
	cookie.fp = fopen_noinh_readbin((ਅੱਖਰ *)fname_exp);
#ਨਹੀਂਤਾਂ
	cookie.fp = mch_fopen((ਅੱਖਰ *)fname_exp, READBIN);
#ਜੇ_ਅੰਤ
    }
    ਜੇ (cookie.fp == ਨਿਰਾ && check_other)
    {
	// Try again, replacing file name ".vimrc" by "_vimrc" or vice versa,
	// and ".exrc" by "_exrc" or vice versa.
	p = gettail(fname_exp);
	ਜੇ ((*p == '.' || *p == '_')
		&& (STRICMP(p + 1, "vimrc") == 0
		    || STRICMP(p + 1, "gvimrc") == 0
		    || STRICMP(p + 1, "exrc") == 0))
	{
	    ਜੇ (*p == '_')
		*p = '.';
	    ਨਹੀਂਤਾਂ
		*p = '_';
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_FOPEN_NOINH
	    cookie.fp = fopen_noinh_readbin((ਅੱਖਰ *)fname_exp);
#ਨਹੀਂਤਾਂ
	    cookie.fp = mch_fopen((ਅੱਖਰ *)fname_exp, READBIN);
#ਜੇ_ਅੰਤ
	}
    }

    ਜੇ (cookie.fp == ਨਿਰਾ && !cookie.ਸਰੋਤ_from_buf)
    {
	ਜੇ (p_verbose > 0)
	{
	    verbose_enter();
	    ਜੇ (SOURCING_NAME == ਨਿਰਾ)
		smsg(_("could not source \"%s\""), fname);
	    ਨਹੀਂਤਾਂ
		smsg(_("line %ld: could not source \"%s\""),
							SOURCING_LNUM, fname);
	    verbose_leave();
	}
	ਜਾਓ theend;
    }

    // The file exists.
    // - In verbose mode, give a message.
    // - For a vimrc file, may want to set 'compatible', call vimrc_found().
    ਜੇ (p_verbose > 1)
    {
	verbose_enter();
	ਜੇ (SOURCING_NAME == ਨਿਰਾ)
	    smsg(_("sourcing \"%s\""), fname);
	ਨਹੀਂਤਾਂ
	    smsg(_("line %ld: sourcing \"%s\""), SOURCING_LNUM, fname);
	verbose_leave();
    }
    ਜੇ (is_vimrc == DOSO_VIMRC)
	vimrc_ਮਿਲਿਆ(fname_exp, (char_u *)"MYVIMRC");
    ਨਹੀਂਤਾਂ ਜੇ (is_vimrc == DOSO_GVIMRC)
	vimrc_ਮਿਲਿਆ(fname_exp, (char_u *)"MYGVIMRC");

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_CRNL
    // If no automatic file format: Set default to CR-NL.
    ਜੇ (*p_ffs == NUL)
	cookie.fileformat = EOL_DOS;
    ਨਹੀਂਤਾਂ
	cookie.fileformat = EOL_UNKNOWN;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    // Check if this script has a breakpoint.
    cookie.breakpoint = dbg_ਲੱਭੋ_breakpoint(TRUE, fname_exp, (linenr_T)0);
    cookie.fname = fname_exp;
    cookie.dbg_tick = debug_tick;

    cookie.level = ex_nesting_level;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ STARTUPTIME
    ਜੇ (ਸਮਾਂ_fd != ਨਿਰਾ)
	ਸਮਾਂ_ਧੱਕੋ(&tv_rel, &tv_ਸ਼ੁਰੂ);
#ਜੇ_ਅੰਤ

    // "legacy" does not apply to commands in the script
    sticky_cmdmod_flags = 0;

    save_current_sctx = current_sctx;
    ਜੇ ((cmdmod.cmod_flags & CMOD_VIM9CMD) && cookie.ਸਰੋਤ_from_buf)
	// When the ":vim9cmd" command modifier is used, source buffer lines as
	// Vim9 script
	current_sctx.sc_ਸੰਸਕਰਣ = SCRIPT_ਸੰਸਕਰਣ_VIM9;
    ਨਹੀਂਤਾਂ
	current_sctx.sc_ਸੰਸਕਰਣ = 1;  // default script version

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    current_sctx.sc_lnum = 0;

# ifdef FEAT_PROFILE
    ਜੇ (do_profiling == PROF_YES)
	prof_ਬੱਚਾ_enter(&wait_ਸ਼ੁਰੂ);		// entering a child now
# endif

    // Don't use local function variables, if called from a function.
    // Also starts profiling timer for nested script.
    save_funccal(&funccalp_ਇੰਦਰਾਜ);

    // Reset "KeyTyped" to avoid some commands thinking they are invoked
    // interactively.  E.g. defining a function would output indent.
    ਪੂਰਨ save_ਕੁੰਜੀ_Typed = ਕੁੰਜੀ_Typed;
    ਕੁੰਜੀ_Typed = FALSE;

    // Check if this script was sourced before to find its SID.
    // Always use a new sequence number.
    current_sctx.sc_seq = ++ਆਖਰੀ_current_SID_seq;
    ਜੇ (sid > 0)
    {
	hashtab_T	*ht;
	ਪੂਰਨ		todo;
	hashitem_T	*hi;
	dictitem_T	*di;

	// loading the same script again
	current_sctx.sc_sid = sid;
	si = SCRIPT_ਵਸਤੂ(sid);
	ਜੇ (si->sn_state == SN_STATE_NOT_LOADED)
	{
	    // this script was found but not loaded yet
	    si->sn_state = SN_STATE_NEW;
	}
	ਨਹੀਂਤਾਂ
	{
	    si->sn_state = SN_STATE_RELOAD;

	    ਜੇ (!clearvars)
	    {
		// Script-local variables remain but "const" can be set again.
		// In Vim9 script variables will be cleared when "vim9script"
		// is encountered without the "noclear" argument.
		ht = &SCRIPT_VARS(sid);
		todo = (ਪੂਰਨ)ht->ht_ਵਰਤਿਆ;
		FOR_ALL_HASHTAB_ਚੀਜ਼ਾਂ(ht, hi, todo)
		    ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
		    {
			--todo;
			di = HI2DI(hi);
			di->di_flags |= DI_FLAGS_RELOAD;
		    }
		// imports can be redefined once
		mark_imports_for_reload(sid);
	    }
	    ਨਹੀਂਤਾਂ
		clear_vim9_scriptlocal_vars(sid);

	    // reset version, "vim9script" may have been added or removed.
	    si->sn_ਸੰਸਕਰਣ = 1;
	}
	ਜੇ (ret_sid != ਨਿਰਾ)
	    *ret_sid = sid;
    }
    ਨਹੀਂਤਾਂ
    {
	ਪੂਰਨ error = OK;

	// It's new, generate a new SID and initialize the scriptitem.
	sid = get_new_scriptitem(&error);
	current_sctx.sc_sid = sid;
	ਜੇ (error == FAIL)
	    ਜਾਓ almosttheend;
	si = SCRIPT_ਵਸਤੂ(sid);
	si->sn_name = fname_exp;
	fname_exp = vim_strsave(si->sn_name);  // used for autocmd
	ਜੇ (ret_sid != ਨਿਰਾ)
	    *ret_sid = sid;

	// Remember the "is_vimrc" flag for when the file is sourced again.
	si->sn_is_vimrc = is_vimrc;
    }

    // Keep the sourcing name/lnum, for recursive calls.
    estack_ਧੱਕੋ(ETYPE_SCRIPT, si->sn_name, 0);
    ESTACK_CHECK_ਤਿਆਰੀ;

# ifdef FEAT_PROFILE
    ਜੇ (do_profiling == PROF_YES)
    {
	ਪੂਰਨ	forceit;

	// Check if we do profiling for this script.
	ਜੇ (!si->sn_prof_on && has_profiling(TRUE, si->sn_name, &forceit, ਨਿਰਾ))
	{
	    script_do_profile(si);
	    si->sn_pr_force = forceit;
	}
	ਜੇ (si->sn_prof_on)
	{
	    ++si->sn_pr_ਗਿਣਤੀ;
	    profile_ਸ਼ੁਰੂ(&si->sn_pr_ਸ਼ੁਰੂ);
	    profile_zero(&si->sn_pr_children);
	}
    }
# endif
#ਨਹੀਂਤਾਂ
    // Keep the sourcing name/lnum, for recursive calls.
    estack_ਧੱਕੋ(ETYPE_SCRIPT, fname_exp, 0);
#ਜੇ_ਅੰਤ

    cookie.conv.vc_type = CONV_NONE;		// no conversion

    // Read the first line so we can check for a UTF-8 BOM.
    firstline = getsourceline(0, (ਖਾਲੀ *)&cookie, 0, TRUE);
    ਜੇ (firstline != ਨਿਰਾ && STRLEN(firstline) >= 3 && firstline[0] == 0xef
			      && firstline[1] == 0xbb && firstline[2] == 0xbf)
    {
	// Found BOM; setup conversion, skip over BOM and recode the line.
	convert_ਤਿਆਰੀ(&cookie.conv, (char_u *)"utf-8", p_enc);
	p = string_convert(&cookie.conv, firstline + 3, ਨਿਰਾ);
	ਜੇ (p == ਨਿਰਾ)
	    p = vim_strsave(firstline + 3);
	ਜੇ (p != ਨਿਰਾ)
	{
	    vim_free(firstline);
	    firstline = p;
	}
    }

    // Call do_cmdline, which will call getsourceline() to get the lines.
    do_cmdline(firstline, getsourceline, (ਖਾਲੀ *)&cookie,
				     DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_REPEAT);
    retval = OK;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROFILE
    ਜੇ (do_profiling == PROF_YES)
    {
	// Get "si" again, "script_items" may have been reallocated.
	si = SCRIPT_ਵਸਤੂ(sid);
	ਜੇ (si->sn_prof_on)
	{
	    profile_ਸਿਰਾ(&si->sn_pr_ਸ਼ੁਰੂ);
	    profile_sub_wait(&wait_ਸ਼ੁਰੂ, &si->sn_pr_ਸ਼ੁਰੂ);
	    profile_ਜੋੜੋ(&si->sn_pr_ਕੁੱਲ, &si->sn_pr_ਸ਼ੁਰੂ);
	    profile_self(&si->sn_pr_self, &si->sn_pr_ਸ਼ੁਰੂ,
							 &si->sn_pr_children);
	}
    }
#ਜੇ_ਅੰਤ

    ਜੇ (got_int)
	emsg(_(e_interrupted));
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ESTACK_CHECK_NOW;
#ਜੇ_ਅੰਤ
    estack_ਕੱਢੋ();
    ਜੇ (p_verbose > 1)
    {
	verbose_enter();
	smsg(_("finished sourcing %s"), fname);
	ਜੇ (SOURCING_NAME != ਨਿਰਾ)
	    smsg(_("continuing in %s"), SOURCING_NAME);
	verbose_leave();
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ STARTUPTIME
    ਜੇ (ਸਮਾਂ_fd != ਨਿਰਾ)
    {
	vim_snprintf((ਅੱਖਰ *)IObuff, IOSIZE, "sourcing %s", fname);
	ਸਮਾਂ_ਸੁਨੇਹਾ_2((ਅੱਖਰ *)IObuff, &tv_ਸ਼ੁਰੂ);
	ਸਮਾਂ_ਕੱਢੋ(&tv_rel);
    }
#ਜੇ_ਅੰਤ

    ਜੇ (!got_int)
	trigger_ਸਰੋਤ_post = TRUE;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    // After a "finish" in debug mode, need to break at first command of next
    // sourced file.
    ਜੇ (save_debug_break_level > ex_nesting_level
	    && debug_break_level == ex_nesting_level)
	++debug_break_level;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
almosttheend:
    // If "sn_save_cpo" is set that means we encountered "vim9script": restore
    // 'cpoptions', unless in the main .vimrc file.
    // Get "si" again, "script_items" may have been reallocated.
    si = SCRIPT_ਵਸਤੂ(sid);
    ਜੇ (si->sn_save_cpo != ਨਿਰਾ && si->sn_is_vimrc == DOSO_NONE)
    {
	ਜੇ (STRCMP(p_cpo, CPO_VIM) != 0)
	{
	    char_u *f;
	    char_u *t;

	    // 'cpo' was changed in the script.  Apply the same change to the
	    // saved value, if possible.
	    ਲਈ (f = (char_u *)CPO_VIM; *f != NUL; ++f)
		ਜੇ (vim_strchr(p_cpo, *f) == ਨਿਰਾ
			&& (t = vim_strchr(si->sn_save_cpo, *f)) != ਨਿਰਾ)
		    // flag was removed, also remove it from the saved 'cpo'
		    mch_memmove(t, t + 1, STRLEN(t));
	    ਲਈ (f = p_cpo; *f != NUL; ++f)
		ਜੇ (vim_strchr((char_u *)CPO_VIM, *f) == ਨਿਰਾ
			&& vim_strchr(si->sn_save_cpo, *f) == ਨਿਰਾ)
		{
		    // flag was added, also add it to the saved 'cpo'
		    t = alloc(STRLEN(si->sn_save_cpo) + 2);
		    ਜੇ (t != ਨਿਰਾ)
		    {
			*t = *f;
			STRCPY(t + 1, si->sn_save_cpo);
			vim_free(si->sn_save_cpo);
			si->sn_save_cpo = t;
		    }
		}
	}
	ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u *)"cpo",
					   0L, si->sn_save_cpo, OPT_NO_REDRAW);
    }
    VIM_CLEAR(si->sn_save_cpo);

    restore_funccal();
# ifdef FEAT_PROFILE
    ਜੇ (do_profiling == PROF_YES)
	prof_ਬੱਚਾ_exit(&wait_ਸ਼ੁਰੂ);		// leaving a child now
# endif

    ਕੁੰਜੀ_Typed = save_ਕੁੰਜੀ_Typed;
#ਜੇ_ਅੰਤ
    current_sctx = save_current_sctx;

    ਜੇ (cookie.fp != ਨਿਰਾ)
	fclose(cookie.fp);
    ਜੇ (cookie.ਸਰੋਤ_from_buf)
	ga_clear_ਲੜੀਆਂ(&cookie.buflines);
    vim_free(cookie.nextline);
    vim_free(firstline);
    convert_ਤਿਆਰੀ(&cookie.conv, ਨਿਰਾ, ਨਿਰਾ);

    ਜੇ (trigger_ਸਰੋਤ_post)
	apply_autocmds(EVENT_SOURCEPOST, fname_exp, fname_exp, FALSE, curbuf);

theend:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਜੇ (sid > 0 && ret_sid != ਨਿਰਾ
	    && fname_not_fixed != ਨਿਰਾ && fname_exp != ਨਿਰਾ)
    {
	ਪੂਰਨ not_fixed_sid = ਲੱਭੋ_script_by_name(fname_not_fixed);

	// If "fname_not_fixed" is a symlink then we source the linked file.
	// If the original name is in the script list we add the ID of the
	// script that was actually sourced.
	ਜੇ (SCRIPT_ID_VALID(not_fixed_sid) && not_fixed_sid != sid)
	    SCRIPT_ਵਸਤੂ(not_fixed_sid)->sn_sourced_sid = sid;
    }
#ਜੇ_ਅੰਤ

    vim_free(fname_not_fixed);
    vim_free(fname_exp);
    sticky_cmdmod_flags = save_sticky_cmdmod_flags;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    estack_compiling = save_estack_compiling;
#ਜੇ_ਅੰਤ
    ਵਾਪਸ retval;
}

    ਪੂਰਨ
do_ਸਰੋਤ(
    char_u	*fname,
    ਪੂਰਨ		check_other,	    // check for .vimrc and _vimrc
    ਪੂਰਨ		is_vimrc,	    // DOSO_ value
    ਪੂਰਨ		*ret_sid)
{
    ਵਾਪਸ do_ਸਰੋਤ_ext(fname, check_other, is_vimrc, ret_sid, ਨਿਰਾ, FALSE);
}


#ਜੇ defined(FEAT_EVAL)

/*
 * ":scriptnames"
 */
    ਖਾਲੀ
ex_scriptnames(exarg_T *eap)
{
    ਪੂਰਨ i;

    ਜੇ (eap->addr_ਗਿਣਤੀ > 0 || *eap->arg != NUL)
    {
	// :script {scriptId}: edit the script
	ਜੇ (eap->addr_ਗਿਣਤੀ > 0 && !SCRIPT_ID_VALID(eap->line2))
	    emsg(_(e_invalid_ਦਲੀਲ));
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (eap->addr_ਗਿਣਤੀ > 0)
		eap->arg = SCRIPT_ਵਸਤੂ(eap->line2)->sn_name;
	    ਨਹੀਂਤਾਂ
	    {
		expand_env(eap->arg, NameBuff, MAXPATHL);
		eap->arg = NameBuff;
	    }
	    do_exedit(eap, ਨਿਰਾ);
	}
	ਵਾਪਸ;
    }

    ਲਈ (i = 1; i <= script_ਚੀਜ਼ਾਂ.ga_len && !got_int; ++i)
    {
	scriptitem_T *si = SCRIPT_ਵਸਤੂ(i);

	ਜੇ (si->sn_name != ਨਿਰਾ)
	{
	    ਅੱਖਰ sourced_buf[20];

	    home_replace(ਨਿਰਾ, si->sn_name, NameBuff, MAXPATHL, TRUE);
	    ਜੇ (si->sn_sourced_sid > 0)
		vim_snprintf(sourced_buf, 20, "->%d", si->sn_sourced_sid);
	    ਨਹੀਂਤਾਂ
		sourced_buf[0] = NUL;
	    vim_snprintf((ਅੱਖਰ *)IObuff, IOSIZE, "%3d%s%s: %s",
		    i,
		    sourced_buf,
		    si->sn_state == SN_STATE_NOT_LOADED ? " A" : "",
		    NameBuff);
	    ਜੇ (!ਸੁਨੇਹਾ_filtered(IObuff))
	    {
		ਸੁਨੇਹਾ_putchar('\n');
		ਸੁਨੇਹਾ_outtrans(IObuff);
		ਬਾਹਰਲਾ_flush();	    // output one line at a time
		ui_breakcheck();
	    }
	}
    }
}

# ਜੇ defined(BACKSLASH_ਵਿੱਚ_FILENAME)
/*
 * Fix slashes in the list of script names for 'shellslash'.
 */
    ਖਾਲੀ
scriptnames_slash_adjust(ਖਾਲੀ)
{
    ਪੂਰਨ i;

    ਲਈ (i = 1; i <= script_ਚੀਜ਼ਾਂ.ga_len; ++i)
	ਜੇ (SCRIPT_ਵਸਤੂ(i)->sn_name != ਨਿਰਾ)
	    slash_adjust(SCRIPT_ਵਸਤੂ(i)->sn_name);
}
# endif

/*
 * Get a pointer to a script name.  Used for ":verbose set".
 * Message appended to "Last set from "
 */
    char_u *
get_scriptname(scid_T id)
{
    ਜੇ (id == SID_MODELINE)
	ਵਾਪਸ (char_u *)_("modeline");
    ਜੇ (id == SID_CMDARG)
	ਵਾਪਸ (char_u *)_("--cmd argument");
    ਜੇ (id == SID_CARG)
	ਵਾਪਸ (char_u *)_("-c argument");
    ਜੇ (id == SID_ENV)
	ਵਾਪਸ (char_u *)_("environment variable");
    ਜੇ (id == SID_ERROR)
	ਵਾਪਸ (char_u *)_("error handler");
    ਜੇ (id == SID_WINLAYOUT)
	ਵਾਪਸ (char_u *)_("changed window size");
    ਵਾਪਸ SCRIPT_ਵਸਤੂ(id)->sn_name;
}

# ਜੇ defined(EXITFREE)
    ਖਾਲੀ
free_scriptnames(ਖਾਲੀ)
{
    ਪੂਰਨ			i;

    ਲਈ (i = script_ਚੀਜ਼ਾਂ.ga_len; i > 0; --i)
    {
	scriptitem_T *si = SCRIPT_ਵਸਤੂ(i);

	// the variables themselves are cleared in evalvars_clear()
	vim_free(si->sn_vars);

	vim_free(si->sn_name);
	free_imports_and_script_vars(i);
	free_string_option(si->sn_save_cpo);
#  ifdef FEAT_PROFILE
	ga_clear(&si->sn_prl_ga);
#  endif
	vim_free(si->sn_autoload_ਅਗੇਤਰ);
	vim_free(si);
    }
    ga_clear(&script_ਚੀਜ਼ਾਂ);
}

    ਖਾਲੀ
free_autoload_scriptnames(ਖਾਲੀ)
{
    ga_clear_ਲੜੀਆਂ(&ga_loaded);
}
# endif

    linenr_T
get_sourced_lnum(
	char_u *(*fgetline)(ਪੂਰਨ, ਖਾਲੀ *, ਪੂਰਨ, getline_opt_T),
	ਖਾਲੀ *cookie)
{
    ਵਾਪਸ fgetline == getsourceline
			? ((ਸਰੋਤ_cookie_T *)cookie)->sourcing_lnum
			: SOURCING_LNUM;
}

/*
 * Return a List of script-local functions defined in the script with id
 * 'sid'.
 */
    ਸਥਿਰ list_T *
get_script_local_funcs(scid_T sid)
{
    hashtab_T	*functbl;
    hashitem_T	*hi;
    long_u	todo;
    list_T	*l;

    l = list_alloc();
    ਜੇ (l == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    // Iterate through all the functions in the global function hash table
    // looking for functions with script ID 'sid'.
    functbl = func_tbl_get();
    todo = functbl->ht_ਵਰਤਿਆ;
    FOR_ALL_HASHTAB_ਚੀਜ਼ਾਂ(functbl, hi, todo)
    {
	ufunc_T	*fp;

	ਜੇ (HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	    ਜਾਰੀ;

	--todo;
	fp = HI2UF(hi);

	// Add active functions with script id == 'sid'
	ਜੇ (!(fp->uf_flags & FC_DEAD) && (fp->uf_script_ctx.sc_sid == sid))
	{
	    char_u	*name;

	    ਜੇ (fp->uf_name_exp != ਨਿਰਾ)
		name = fp->uf_name_exp;
	    ਨਹੀਂਤਾਂ
		name = fp->uf_name;

	    list_append_string(l, name, -1);
	}
    }

    ਵਾਪਸ l;
}

/*
 * getscriptinfo() function
 */
    ਖਾਲੀ
f_getscriptinfo(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    char_u	*pat = ਨਿਰਾ;
    regmatch_T	regmatch;
    ਪੂਰਨ		filterpat = FALSE;
    scid_T	sid = -1;

    ਜੇ (rettv_list_alloc(rettv) == FAIL)
	ਵਾਪਸ;

    ਜੇ (check_for_opt_dict_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    l = rettv->vval.v_list;

    regmatch.regprog = ਨਿਰਾ;
    regmatch.rm_ic = p_ic;

    ਜੇ (argvars[0].v_type == VAR_DICT)
    {
	dictitem_T *sid_di = dict_ਲੱਭੋ(argvars[0].vval.v_dict,
							   (char_u *)"sid", 3);
	ਜੇ (sid_di != ਨਿਰਾ)
	{
	    ਪੂਰਨ error = FALSE;
	    sid = tv_get_ਸੰਖਿਆ_chk(&sid_di->di_tv, &error);
	    ਜੇ (error)
		ਵਾਪਸ;
	    ਜੇ (sid <= 0)
	    {
		semsg(_(e_invalid_ਮੁੱਲ_for_ਦਲੀਲ_str_str), "sid",
						tv_get_string(&sid_di->di_tv));
		ਵਾਪਸ;
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    pat = dict_get_string(argvars[0].vval.v_dict, "name", TRUE);
	    ਜੇ (pat != ਨਿਰਾ)
		regmatch.regprog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
	    ਜੇ (regmatch.regprog != ਨਿਰਾ)
		filterpat = TRUE;
	}
    }

    ਲਈ (varnumber_T i = sid > 0 ? sid : 1;
		       (i == sid || sid <= 0) && i <= script_ਚੀਜ਼ਾਂ.ga_len; ++i)
    {
	scriptitem_T	*si = SCRIPT_ਵਸਤੂ(i);
	dict_T		*d;

	ਜੇ (si->sn_name == ਨਿਰਾ)
	    ਜਾਰੀ;

	ਜੇ (filterpat && !vim_regexec(&regmatch, si->sn_name, (colnr_T)0))
	    ਜਾਰੀ;

	ਜੇ ((d = dict_alloc()) == ਨਿਰਾ
		|| list_append_dict(l, d) == FAIL)
	{
	    dict_unref(d);
	    ਜਾਓ theend;
	}
	ਜੇ (dict_ਜੋੜੋ_string(d, "name", si->sn_name) == FAIL
		|| dict_ਜੋੜੋ_ਸੰਖਿਆ(d, "sid", i) == FAIL
		|| dict_ਜੋੜੋ_ਸੰਖਿਆ(d, "sourced", si->sn_sourced_sid) == FAIL
		|| dict_ਜੋੜੋ_ਸੰਖਿਆ(d, "version", si->sn_ਸੰਸਕਰਣ) == FAIL
		|| dict_ਜੋੜੋ_bool(d, "autoload",
				si->sn_state == SN_STATE_NOT_LOADED) == FAIL)
	    ਜਾਓ theend;

	// When a script ID is specified, return information about only the
	// specified script, and add the script-local variables and functions.
	ਜੇ (sid > 0)
	{
	    dict_T	*var_dict;
	    list_T	*fn_list;

	    var_dict = dict_copy(&si->sn_vars->sv_dict, TRUE, TRUE,
								get_copyID());
	    ਜੇ (var_dict == ਨਿਰਾ)
		ਜਾਓ theend;
	    ਜੇ (dict_ਜੋੜੋ_dict(d, "variables", var_dict) == FAIL)
	    {
		dict_unref(var_dict);
		ਜਾਓ theend;
	    }
	    --var_dict->dv_refcount;
	    fn_list = get_script_local_funcs(sid);
	    ਜੇ (fn_list == ਨਿਰਾ || dict_ਜੋੜੋ_list(d, "functions", fn_list) == FAIL)
	    {
		list_unref(fn_list);
		ਜਾਓ theend;
	    }
	}
    }

theend:
    vim_regfree(regmatch.regprog);
    vim_free(pat);
}

#ਜੇ_ਅੰਤ

    ਸਥਿਰ char_u *
get_one_sourceline(ਸਰੋਤ_cookie_T *sp)
{
    garray_T		ga;
    ਪੂਰਨ			len;
    ਪੂਰਨ			c;
    char_u		*buf;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_CRNL
    ਪੂਰਨ			has_cr;		// CR-LF found
#ਜੇ_ਅੰਤ
    ਪੂਰਨ			have_read = FALSE;

    // use a growarray to store the sourced line
    ga_init2(&ga, 1, 250);

    // Loop until there is a finished line (or end-of-file).
    ++sp->sourcing_lnum;
    ਲਈ (;;)
    {
	// make room to read at least 120 (more) characters
	ਜੇ (ga_grow(&ga, 120) == FAIL)
	    ਰੋਕੋ;
	ਜੇ (sp->ਸਰੋਤ_from_buf)
	{
	    ਜੇ (sp->buf_lnum >= sp->buflines.ga_len)
		ਰੋਕੋ;		    // all the lines are processed
	    ga_concat(&ga, ((char_u **)sp->buflines.ga_ਅੰਕੜੇ)[sp->buf_lnum]);
	    sp->buf_lnum++;
	    ਜੇ (ga_grow(&ga, 1) == FAIL)
		ਰੋਕੋ;
	    buf = (char_u *)ga.ga_ਅੰਕੜੇ;
	    buf[ga.ga_len++] = NUL;
	    len = ga.ga_len;
	}
	ਨਹੀਂਤਾਂ
	{
	    buf = (char_u *)ga.ga_ਅੰਕੜੇ;
	    ਜੇ (fgets((ਅੱਖਰ *)buf + ga.ga_len, ga.ga_maxlen - ga.ga_len,
			sp->fp) == ਨਿਰਾ)
		ਰੋਕੋ;
	    len = ga.ga_len + (ਪੂਰਨ)STRLEN(buf + ga.ga_len);
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_CRNL
	// Ignore a trailing CTRL-Z, when in Dos mode.	Only recognize the
	// CTRL-Z by its own, or after a NL.
	ਜੇ (	   (len == 1 || (len >= 2 && buf[len - 2] == '\n'))
		&& sp->fileformat == EOL_DOS
		&& buf[len - 1] == Ctrl_Z)
	{
	    buf[len - 1] = NUL;
	    ਰੋਕੋ;
	}
#ਜੇ_ਅੰਤ

	have_read = TRUE;
	ga.ga_len = len;

	// If the line was longer than the buffer, read more.  A line of a
	// buffer was taken as a whole.
	ਜੇ (!sp->ਸਰੋਤ_from_buf && ga.ga_maxlen - ga.ga_len == 1
						       && buf[len - 1] != '\n')
	    ਜਾਰੀ;

	ਜੇ (len >= 1 && buf[len - 1] == '\n')	// remove trailing NL
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_CRNL
	    has_cr = (len >= 2 && buf[len - 2] == '\r');
	    ਜੇ (sp->fileformat == EOL_UNKNOWN)
	    {
		ਜੇ (has_cr)
		    sp->fileformat = EOL_DOS;
		ਨਹੀਂਤਾਂ
		    sp->fileformat = EOL_UNIX;
	    }

	    ਜੇ (sp->fileformat == EOL_DOS)
	    {
		ਜੇ (has_cr)	    // replace trailing CR
		{
		    buf[len - 2] = '\n';
		    --len;
		    --ga.ga_len;
		}
		ਨਹੀਂਤਾਂ	    // lines like ":map xx yy^M" will have failed
		{
		    ਜੇ (!sp->error)
		    {
			ਸੁਨੇਹਾ_ਸਰੋਤ(HL_ATTR(HLF_W));
			emsg(_("W15: Warning: Wrong line separator, ^M may be missing"));
		    }
		    sp->error = TRUE;
		    sp->fileformat = EOL_UNIX;
		}
	    }
#ਜੇ_ਅੰਤ
	    // The '\n' is escaped if there is an odd number of ^V's just
	    // before it, first set "c" just before the 'V's and then check
	    // len&c parities (is faster than ((len-c)%2 == 0)) -- Acevedo
	    ਲਈ (c = len - 2; c >= 0 && buf[c] == Ctrl_V; c--)
		;
	    ਜੇ ((len & 1) != (c & 1))	// escaped NL, read more
	    {
		++sp->sourcing_lnum;
		ਜਾਰੀ;
	    }

	    buf[len - 1] = NUL;		// remove the NL
	}

	// Check for ^C here now and then, so recursive :so can be broken.
	ਸਤਰ_breakcheck();
	ਰੋਕੋ;
    }

    ਜੇ (have_read)
	ਵਾਪਸ (char_u *)ga.ga_ਅੰਕੜੇ;

    vim_free(ga.ga_ਅੰਕੜੇ);
    ਵਾਪਸ ਨਿਰਾ;
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
	ਪੂਰਨ c ਅਣਵਰਤਿਆ,
	ਖਾਲੀ *cookie,
	ਪੂਰਨ indent ਅਣਵਰਤਿਆ,
	getline_opt_T options)
{
    ਸਰੋਤ_cookie_T	*sp = (ਸਰੋਤ_cookie_T *)cookie;
    char_u		*ਸਤਰ;
    char_u		*p;
    ਪੂਰਨ			do_vim9_all = ਵਿੱਚ_vim9script()
					      && options == GETLINE_CONCAT_ALL;
    ਪੂਰਨ			do_bar_cont = do_vim9_all
					 || options == GETLINE_CONCAT_CONTBAR;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    // If breakpoints have been added/deleted need to check for it.
    ਜੇ ((sp->dbg_tick < debug_tick) && !sp->ਸਰੋਤ_from_buf)
    {
	sp->breakpoint = dbg_ਲੱਭੋ_breakpoint(TRUE, sp->fname, SOURCING_LNUM);
	sp->dbg_tick = debug_tick;
    }
# ifdef FEAT_PROFILE
    ਜੇ (do_profiling == PROF_YES)
	script_ਸਤਰ_ਸਿਰਾ();
# endif
#ਜੇ_ਅੰਤ

    // Set the current sourcing line number.
    SOURCING_LNUM = sp->sourcing_lnum + 1;

    // Get current line.  If there is a read-ahead line, use it, otherwise get
    // one now.  "fp" is NULL if actually using a string.
    ਜੇ (sp->ਮੁਕੰਮਲ || (!sp->ਸਰੋਤ_from_buf && sp->fp == ਨਿਰਾ))
	ਸਤਰ = ਨਿਰਾ;
    ਨਹੀਂਤਾਂ ਜੇ (sp->nextline == ਨਿਰਾ)
	ਸਤਰ = get_one_sourceline(sp);
    ਨਹੀਂਤਾਂ
    {
	ਸਤਰ = sp->nextline;
	sp->nextline = ਨਿਰਾ;
	++sp->sourcing_lnum;
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROFILE
    ਜੇ (ਸਤਰ != ਨਿਰਾ && do_profiling == PROF_YES)
	script_ਸਤਰ_ਸ਼ੁਰੂ();
#ਜੇ_ਅੰਤ

    // Only concatenate lines starting with a \ when 'cpoptions' doesn't
    // contain the 'C' flag.
    ਜੇ (ਸਤਰ != ਨਿਰਾ && options != GETLINE_NONE
				      && vim_strchr(p_cpo, CPO_CONCAT) == ਨਿਰਾ)
    {
	ਪੂਰਨ comment_char = ਵਿੱਚ_vim9script() ? '#' : '"';

	// compensate for the one line read-ahead
	--sp->sourcing_lnum;

	// Get the next line and concatenate it when it starts with a
	// backslash. We always need to read the next line, keep it in
	// sp->nextline.
	/* Also check for a comment in between continuation lines: "\ */
	// Also check for a Vim9 comment, empty line, line starting with '|',
	// but not "||".
	sp->nextline = get_one_sourceline(sp);
	ਜੇ (sp->nextline != ਨਿਰਾ
		&& (*(p = skipwhite(sp->nextline)) == '\\'
			      || (p[0] == comment_char
						&& p[1] == '\\' && p[2] == ' ')
			      || (do_vim9_all && (*p == NUL
						     || vim9_comment_ਸ਼ੁਰੂ(p)))
			      || (do_bar_cont && p[0] == '|' && p[1] != '|')))
	{
	    garray_T    ga;

	    ga_init2(&ga, ਆਕਾਰ(char_u), 400);
	    ga_concat(&ga, ਸਤਰ);
	    ਜੇ (*p == '\\')
		ga_concat(&ga, p + 1);
	    ਨਹੀਂਤਾਂ ਜੇ (*p == '|')
	    {
		GA_CONCAT_LITERAL(&ga, " ");
		ga_concat(&ga, p);
	    }
	    ਲਈ (;;)
	    {
		vim_free(sp->nextline);
		sp->nextline = get_one_sourceline(sp);
		ਜੇ (sp->nextline == ਨਿਰਾ)
		    ਰੋਕੋ;
		p = skipwhite(sp->nextline);
		ਜੇ (*p == '\\' || (do_bar_cont && p[0] == '|' && p[1] != '|'))
		{
		    // Adjust the growsize to the current length to speed up
		    // concatenating many lines.
		    ਜੇ (ga.ga_len > 400)
		    {
			ਜੇ (ga.ga_len > 8000)
			    ga.ga_growsize = 8000;
			ਨਹੀਂਤਾਂ
			    ga.ga_growsize = ga.ga_len;
		    }
		    ਜੇ (*p == '\\')
			ga_concat(&ga, p + 1);
		    ਨਹੀਂਤਾਂ
		    {
			GA_CONCAT_LITERAL(&ga, " ");
			ga_concat(&ga, p);
		    }
		}
		ਨਹੀਂਤਾਂ ਜੇ (!(p[0] == (comment_char)
						&& p[1] == '\\' && p[2] == ' ')
		     && !(do_vim9_all && (*p == NUL || vim9_comment_ਸ਼ੁਰੂ(p))))
		    ਰੋਕੋ;
		/* drop a # comment or "\ comment line */
	    }
	    ga_append(&ga, NUL);
	    vim_free(ਸਤਰ);
	    ਸਤਰ = ga.ga_ਅੰਕੜੇ;
	}
    }

    ਜੇ (ਸਤਰ != ਨਿਰਾ && sp->conv.vc_type != CONV_NONE)
    {
	char_u	*s;

	// Convert the encoding of the script line.
	s = string_convert(&sp->conv, ਸਤਰ, ਨਿਰਾ);
	ਜੇ (s != ਨਿਰਾ)
	{
	    vim_free(ਸਤਰ);
	    ਸਤਰ = s;
	}
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    // Did we encounter a breakpoint?
    ਜੇ (!sp->ਸਰੋਤ_from_buf && sp->breakpoint != 0
	    && sp->breakpoint <= SOURCING_LNUM)
    {
	dbg_breakpoint(sp->fname, SOURCING_LNUM);
	// Find next breakpoint.
	sp->breakpoint = dbg_ਲੱਭੋ_breakpoint(TRUE, sp->fname, SOURCING_LNUM);
	sp->dbg_tick = debug_tick;
    }
#ਜੇ_ਅੰਤ

    ਵਾਪਸ ਸਤਰ;
}

/*
 * Returns TRUE if sourcing a script either from a file or a buffer.
 * Otherwise returns FALSE.
 */
    ਪੂਰਨ
sourcing_a_script(exarg_T *eap)
{
    ਵਾਪਸ (getline_equal(eap->ea_getline, eap->cookie, getsourceline));
}

/*
 * ":scriptencoding": Set encoding conversion for a sourced script.
 */
    ਖਾਲੀ
ex_scriptencoding(exarg_T *eap)
{
    ਸਰੋਤ_cookie_T	*sp;
    char_u		*name;

    ਜੇ (!sourcing_a_script(eap))
    {
	emsg(_(e_scriptencoding_ਵਰਤਿਆ_outside_ਦਾ_sourced_file));
	ਵਾਪਸ;
    }

    ਜੇ (*eap->arg != NUL)
    {
	name = enc_canonize(eap->arg);
	ਜੇ (name == ਨਿਰਾ)	// out of memory
	    ਵਾਪਸ;
    }
    ਨਹੀਂਤਾਂ
	name = eap->arg;

    // Setup for conversion from the specified encoding to 'encoding'.
    sp = (ਸਰੋਤ_cookie_T *)getline_cookie(eap->ea_getline, eap->cookie);
    convert_ਤਿਆਰੀ(&sp->conv, name, p_enc);

    ਜੇ (name != eap->arg)
	vim_free(name);
}

/*
 * ":scriptversion": Set Vim script version for a sourced script.
 */
    ਖਾਲੀ
ex_scriptversion(exarg_T *eap ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ		nr;

    ਜੇ (!sourcing_a_script(eap))
    {
	emsg(_(e_scriptversion_ਵਰਤਿਆ_outside_ਦਾ_sourced_file));
	ਵਾਪਸ;
    }
    ਜੇ (ਵਿੱਚ_vim9script())
    {
	emsg(_(e_cannot_use_scriptversion_after_vim9script));
	ਵਾਪਸ;
    }

    nr = getdigits(&eap->arg);
    ਜੇ (nr == 0 || *eap->arg != NUL)
	emsg(_(e_invalid_ਦਲੀਲ));
    ਨਹੀਂਤਾਂ ਜੇ (nr > SCRIPT_ਸੰਸਕਰਣ_MAX)
	semsg(_(e_scriptversion_not_supported_nr), nr);
    ਨਹੀਂਤਾਂ
    {
	current_sctx.sc_ਸੰਸਕਰਣ = nr;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	SCRIPT_ਵਸਤੂ(current_sctx.sc_sid)->sn_ਸੰਸਕਰਣ = nr;
#ਜੇ_ਅੰਤ
    }
}

#ਜੇ defined(FEAT_EVAL)
/*
 * ":finish": Mark a sourced file as finished.
 */
    ਖਾਲੀ
ex_ਅੰਤ(exarg_T *eap)
{
    ਜੇ (sourcing_a_script(eap))
	do_ਅੰਤ(eap, FALSE);
    ਨਹੀਂਤਾਂ
	emsg(_(e_ਅੰਤ_ਵਰਤਿਆ_outside_ਦਾ_sourced_file));
}

/*
 * Mark a sourced file as finished.  Possibly makes the ":finish" pending.
 * Also called for a pending finish at the ":endtry" or after returning from
 * an extra do_cmdline().  "reanimate" is used in the latter case.
 */
    ਖਾਲੀ
do_ਅੰਤ(exarg_T *eap, ਪੂਰਨ reanimate)
{
    ਪੂਰਨ		idx;

    ਜੇ (reanimate)
	((ਸਰੋਤ_cookie_T *)getline_cookie(eap->ea_getline,
					      eap->cookie))->ਮੁਕੰਮਲ = FALSE;

    // Cleanup (and inactivate) conditionals, but stop when a try conditional
    // not in its finally clause (which then is to be executed next) is found.
    // In this case, make the ":finish" pending for execution at the ":endtry".
    // Otherwise, finish normally.
    idx = cleanup_conditionals(eap->cstack, 0, TRUE);
    ਜੇ (idx >= 0)
    {
	eap->cstack->cs_pending[idx] = CSTP_ਅੰਤ;
	report_make_pending(CSTP_ਅੰਤ, ਨਿਰਾ);
    }
    ਨਹੀਂਤਾਂ
	((ਸਰੋਤ_cookie_T *)getline_cookie(eap->ea_getline,
					       eap->cookie))->ਮੁਕੰਮਲ = TRUE;
}


/*
 * Return TRUE when a sourced file had the ":finish" command: Don't give error
 * message for missing ":endif".
 * Return FALSE when not sourcing a file.
 */
    ਪੂਰਨ
ਸਰੋਤ_ਮੁਕੰਮਲ(
    char_u	*(*fgetline)(ਪੂਰਨ, ਖਾਲੀ *, ਪੂਰਨ, getline_opt_T),
    ਖਾਲੀ	*cookie)
{
    ਵਾਪਸ (getline_equal(fgetline, cookie, getsourceline)
	    && ((ਸਰੋਤ_cookie_T *)getline_cookie(
						fgetline, cookie))->ਮੁਕੰਮਲ);
}

/*
 * Find the path of a script below the "autoload" directory.
 * Returns NULL if there is no "/autoload/" in the script name.
 */
    ਸਥਿਰ char_u *
script_name_after_autoload(scriptitem_T *si)
{
    char_u	*p = si->sn_name;
    char_u	*res = ਨਿਰਾ;

    ਲਈ (;;)
    {
	char_u *n = (char_u *)ਲੜੀ_ਖੋਜ((ਅੱਖਰ *)p, "autoload");

	ਜੇ (n == ਨਿਰਾ)
	    ਰੋਕੋ;
	ਜੇ (n > p && vim_ispathsep(n[-1]) && vim_ispathsep(n[8]))
	    res = n + 9;
	p = n + 8;
    }
    ਵਾਪਸ res;
}

/*
 * For an autoload script "autoload/dir/script.vim" return the prefix
 * "dir#script#" in allocated memory.
 * Returns NULL if anything is wrong.
 */
    char_u *
get_autoload_ਅਗੇਤਰ(scriptitem_T *si)
{
    char_u *p = script_name_after_autoload(si);
    char_u *ਅਗੇਤਰ;

    ਜੇ (p == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    ਅਗੇਤਰ = vim_strsave(p);
    ਜੇ (ਅਗੇਤਰ == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    // replace all '/' with '#' and locate ".vim" at the end
    ਲਈ (p = ਅਗੇਤਰ; *p != NUL; p += mb_ptr2len(p))
    {
	ਜੇ (vim_ispathsep(*p))
	    *p = '#';
	ਨਹੀਂਤਾਂ ਜੇ (STRCMP(p, ".vim") == 0)
	{
	    p[0] = '#';
	    p[1] = NUL;
	    ਵਾਪਸ ਅਗੇਤਰ;
	}
    }

    // did not find ".vim" at the end
    vim_free(ਅਗੇਤਰ);
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * If in a Vim9 autoload script return "name" with the autoload prefix for the
 * script.  If successful the returned name is allocated.
 * Otherwise it returns "name" unmodified.
 */
    char_u *
may_ਅਗੇਤਰ_autoload(char_u *name)
{
    ਜੇ (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	ਵਾਪਸ name;

    scriptitem_T *si = SCRIPT_ਵਸਤੂ(current_sctx.sc_sid);

    ਜੇ (si->sn_autoload_ਅਗੇਤਰ == ਨਿਰਾ)
	ਵਾਪਸ name;

    char_u  *basename = name;
    size_t  len;
    char_u  *res;

    ਜੇ (*name == K_SPECIAL)
    {
	char_u *p = vim_strchr(name, '_');

	// skip over "<SNR>99_"
	ਜੇ (p != ਨਿਰਾ)
	    basename = p + 1;
    }

    len = STRLEN(si->sn_autoload_ਅਗੇਤਰ) + STRLEN(basename) + 2;
    res = alloc(len);
    ਜੇ (res == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    vim_snprintf((ਅੱਖਰ *)res, len, "%s%s", si->sn_autoload_ਅਗੇਤਰ, basename);
    ਵਾਪਸ res;
}

/*
 * Return the autoload script name for a function or variable name.
 * Returns NULL when out of memory.
 * Caller must make sure that "name" contains AUTOLOAD_CHAR.
 */
    char_u *
autoload_name(char_u *name)
{
    char_u	*p, *q = ਨਿਰਾ;
    char_u	*scriptname;

    // Get the script file name: replace '#' with '/', append ".vim".
    scriptname = alloc(STRLEN(name) + 14);
    ਜੇ (scriptname == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    STRCPY(scriptname, "autoload/");
    STRCAT(scriptname, name[0] == 'g' && name[1] == ':' ? name + 2: name);
    ਲਈ (p = scriptname + 9; (p = vim_strchr(p, AUTOLOAD_CHAR)) != ਨਿਰਾ;
								    q = p, ++p)
	*p = '/';
    STRCPY(q, ".vim");
    ਵਾਪਸ scriptname;
}

/*
 * If "name" has a package name try autoloading the script for it.
 * Return TRUE if a package was loaded.
 */
    ਪੂਰਨ
script_autoload(
    char_u	*name,
    ਪੂਰਨ		reload)	    // load script again when already loaded
{
    char_u	*p;
    char_u	*scriptname, *tofree;
    ਪੂਰਨ		ret = FALSE;
    ਪੂਰਨ		i;
    ਪੂਰਨ		ret_sid;

    // If the name starts with "<SNR>123_" then "123" is the script ID.
    ਜੇ (name[0] == K_SPECIAL && name[1] == KS_EXTRA && name[2] == KE_SNR)
    {
	p = name + 3;
	ret_sid = (ਪੂਰਨ)getdigits(&p);
	ਜੇ (*p == '_' && SCRIPT_ID_VALID(ret_sid))
	{
	    may_ਲੱਦੋ_script(ret_sid, &ret);
	    ਵਾਪਸ ret;
	}
    }

    // If there is no '#' after name[0] there is no package name.
    p = vim_strchr(name, AUTOLOAD_CHAR);
    ਜੇ (p == ਨਿਰਾ || p == name)
	ਵਾਪਸ FALSE;

    tofree = scriptname = autoload_name(name);
    ਜੇ (scriptname == ਨਿਰਾ)
	ਵਾਪਸ FALSE;

    // Find the name in the list of previously loaded package names.  Skip
    // "autoload/", it's always the same.
    ਲਈ (i = 0; i < ga_loaded.ga_len; ++i)
	ਜੇ (STRCMP(((char_u **)ga_loaded.ga_ਅੰਕੜੇ)[i] + 9, scriptname + 9) == 0)
	    ਰੋਕੋ;
    ਜੇ (!reload && i < ga_loaded.ga_len)
	ret = FALSE;	    // was loaded already
    ਨਹੀਂਤਾਂ
    {
	// Remember the name if it wasn't loaded already.
	ਜੇ (i == ga_loaded.ga_len && ga_grow(&ga_loaded, 1) == OK)
	{
	    ((char_u **)ga_loaded.ga_ਅੰਕੜੇ)[ga_loaded.ga_len++] = scriptname;
	    tofree = ਨਿਰਾ;
	}

	// Try loading the package from $VIMRUNTIME/autoload/<name>.vim
	// Use "ret_sid" to avoid loading the same script again.
	ਜੇ (ਸਰੋਤ_ਵਿੱਚ_ਰਸਤਾ(p_rtp, scriptname, DIP_ਸ਼ੁਰੂ, &ret_sid) == OK)
	    ret = TRUE;
    }

    vim_free(tofree);
    ਵਾਪਸ ret;
}
#ਜੇ_ਅੰਤ
