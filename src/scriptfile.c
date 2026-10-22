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

#ଅନ୍ତର୍ଭାଵ "vim.h"

#ଯଦି defined(FEAT_EVAL)
// The names of packages that once were loaded are remembered.
ସ୍ଥିର garray_T		ga_loaded = {0, 0, ଆକାର(char_u *), 4, ଖାଲି};
#endif

// last used sequence number for sourcing scripts (current_sctx.sc_seq)
#ifdef FEAT_EVAL
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା		last_current_SID_seq = 0;
#endif

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_source_ext(char_u *fname, ପୂର୍ଣ୍ଣସଂଖ୍ୟା check_other, ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_vimrc, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret_sid, exarg_T *eap, ପୂର୍ଣ୍ଣସଂଖ୍ୟା clearvars);

/*
 * Initialize the execution stack.
 */
    ଶୂନ୍ୟ
estack_init(ଶୂନ୍ୟ)
{
    estack_T *entry;

    ଯଦି (ga_grow(&exestack, 10) == FAIL)
	mch_exit(0);
    entry = ((estack_T *)exestack.ga_data) + exestack.ga_len;
    entry->es_type = ETYPE_ଶୀର୍ଷ;
    entry->es_name = ଖାଲି;
    entry->es_lnum = 0;
#ifdef FEAT_EVAL
    entry->es_info.ufunc = ଖାଲି;
#endif
    ++exestack.ga_len;
}

/*
 * Add an item to the execution stack.
 * Returns the new entry or NULL when out of memory.
 */
    estack_T *
estack_ଠେଲ(etype_T type, char_u *name, long lnum)
{
    estack_T *entry;

    // If memory allocation fails then we'll pop more than we push, eventually
    // at the top level it will be OK again.
    ଯଦି (ga_grow(&exestack, 1) == FAIL)
	ଫେରନ୍ତୁ ଖାଲି;

    entry = ((estack_T *)exestack.ga_data) + exestack.ga_len;
    entry->es_type = type;
    entry->es_name = name;
    entry->es_lnum = lnum;
#ifdef FEAT_EVAL
    entry->es_info.ufunc = ଖାଲି;
#endif
    ++exestack.ga_len;
    ଫେରନ୍ତୁ entry;
}

#ଯଦି defined(FEAT_EVAL)
/*
 * Add a user function to the execution stack.
 */
    estack_T *
estack_ଠେଲ_ufunc(ufunc_T *ufunc, long lnum)
{
    estack_T *entry = estack_ଠେଲ(ETYPE_UFUNC,
	    ufunc->uf_name_exp != ଖାଲି
				  ? ufunc->uf_name_exp : ufunc->uf_name, lnum);
    ଯଦି (entry != ଖାଲି)
	entry->es_info.ufunc = ufunc;
    ଫେରନ୍ତୁ entry;
}

/*
 * Return TRUE if "ufunc" with "lnum" is already at the top of the exe stack.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
estack_ଶୀର୍ଷ_is_ufunc(ufunc_T *ufunc, long lnum)
{
    estack_T *entry;

    ଯଦି (exestack.ga_len == 0)
	ଫେରନ୍ତୁ FALSE;
    entry = ((estack_T *)exestack.ga_data) + exestack.ga_len - 1;
    ଫେରନ୍ତୁ entry->es_type == ETYPE_UFUNC
	&& STRCMP( entry->es_name, ufunc->uf_name_exp != ଖାଲି
				    ? ufunc->uf_name_exp : ufunc->uf_name) == 0
	&& entry->es_lnum == lnum;
}
#endif

/*
 * Take an item off of the execution stack and return it.
 */
    estack_T *
estack_କାଢ଼(ଶୂନ୍ୟ)
{
    ଯଦି (exestack.ga_len == 0)
	ଫେରନ୍ତୁ ଖାଲି;
    --exestack.ga_len;
    ଫେରନ୍ତୁ ((estack_T *)exestack.ga_data) + exestack.ga_len;
}

/*
 * Get the current value for "which" in allocated memory.
 * "which" is ESTACK_SFILE for <sfile>, ESTACK_STACK for <stack> or
 * ESTACK_SCRIPT for <script>.
 */
    char_u *
estack_sfile(estack_arg_T which UNUSED)
{
    estack_T	*entry;
#ifdef FEAT_EVAL
    garray_T	ga;
    size_t	len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx;
    etype_T	last_type = ETYPE_SCRIPT;
#endif

    entry = ((estack_T *)exestack.ga_data) + exestack.ga_len - 1;
#ifdef FEAT_EVAL
    ଯଦି (which == ESTACK_SFILE && entry->es_type != ETYPE_UFUNC)
#endif
    {
	ଯଦି (entry->es_name == ଖାଲି)
	    ଫେରନ୍ତୁ ଖାଲି;
	ଫେରନ୍ତୁ vim_strsave(entry->es_name);
    }
#ifdef FEAT_EVAL
    // expand('<sfile>') works in a function for backwards compatibility, but
    // may give an unexpected result.  Disallow it in Vim 9 script.
    ଯଦି (which == ESTACK_SFILE && in_vim9script())
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା  save_emsg_off = emsg_off;

	ଯଦି (emsg_off == 1)
	    // f_expand() silences errors but we do want this one
	    emsg_off = 0;
	emsg(_(e_cannot_expand_sfile_in_vim9_function));
	emsg_off = save_emsg_off;
	ଫେରନ୍ତୁ ଖାଲି;
    }

    // If evaluated in a function or autocommand, return the path of the script
    // where it is defined, at script level the current script path is returned
    // instead.
    ଯଦି (which == ESTACK_SCRIPT)
    {
	// Walk the stack backwards, starting from the current frame.
	ପାଇଁ (idx = exestack.ga_len - 1; idx >= 0; --idx, --entry)
	{
	    ଯଦି (entry->es_type == ETYPE_UFUNC || entry->es_type == ETYPE_AUCMD)
	    {
		sctx_T *def_ctx = entry->es_type == ETYPE_UFUNC
			      ? &entry->es_info.ufunc->uf_script_ctx
			      : entry->es_info.aucmd != ଖାଲି
				  ? acp_script_ctx(entry->es_info.aucmd) : ଖାଲି;

		ଫେରନ୍ତୁ def_ctx != ଖାଲି && def_ctx->sc_sid > 0
			   ? vim_strsave(SCRIPT_ITEM(def_ctx->sc_sid)->sn_name)
			   : ଖାଲି;
	    }
	    ଅନ୍ୟଥା ଯଦି (entry->es_type == ETYPE_SCRIPT)
		ଫେରନ୍ତୁ vim_strsave(entry->es_name);
	}
	ଫେରନ୍ତୁ ଖାଲି;
    }

    // Give information about each stack entry up to the root.
    // For a function we compose the call stack, as it was done in the past:
    //   "function One[123]..Two[456]..Three"
    ga_init2(&ga, ଆକାର(ଅକ୍ଷର), 100);
    ପାଇଁ (idx = 0; idx < exestack.ga_len; ++idx)
    {
	entry = ((estack_T *)exestack.ga_data) + idx;
	ଯଦି (entry->es_name != ଖାଲି)
	{
	    long	lnum = 0;
	    size_t	added = 0;
	    string_T	type_name = {(char_u *)"", 0};
	    string_T	class_name = {(char_u *)"", 0};
	    string_T	es_name = {entry->es_name, STRLEN(entry->es_name)};

	    ଯଦି (entry->es_type != last_type)
	    {
		ବାଛ (entry->es_type)
		{
		    ସ୍ଥିତି ETYPE_SCRIPT:
			STR_LITERAL_SET(type_name, "script ");
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି ETYPE_UFUNC:
			STR_LITERAL_SET(type_name, "function ");
			ଭାଙ୍ଗ;
		    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
			ଭାଙ୍ଗ;
		}
		last_type = entry->es_type;
	    }
	    ଯଦି (entry->es_type == ETYPE_UFUNC && entry->es_info.ufunc->uf_class != ଖାଲି)
	    {
		class_name.string = entry->es_info.ufunc->uf_class->class_name.string;
		class_name.length = entry->es_info.ufunc->uf_class->class_name.length;
	    }
	    ଯଦି (idx == exestack.ga_len - 1)
		lnum = which == ESTACK_ଗଦା ? SOURCING_LNUM : 0;
	    ଅନ୍ୟଥା
		lnum = entry->es_lnum;

	    len = es_name.length + type_name.length + class_name.length + 26;
	    ଯଦି (ga_grow(&ga, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)len) == FAIL)
		ଭାଙ୍ଗ;
	    ga_concat_len(&ga, type_name.string, type_name.length);
	    // For class methods prepend "<class name>." to the function name.
	    ଯଦି (*class_name.string != NUL)
	    {
		added = vim_snprintf_safelen(
		    (ଅକ୍ଷର *)ga.ga_data + ga.ga_len,
		    ga.ga_maxlen - ga.ga_len,
		    "<SNR>%d_%s.",
		    entry->es_info.ufunc->uf_script_ctx.sc_sid,
		    class_name.string);

		ga.ga_len += (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)added;
	    }
	    ga_concat_len(&ga, es_name.string, es_name.length);
	    // For the bottom entry of <sfile>: do not add the line number, it is used in
	    // <slnum>.  Also leave it out when the number is not set.
	    ଯଦି (lnum != 0)
	    {
		added = vim_snprintf_safelen(
		    (ଅକ୍ଷର *)ga.ga_data + ga.ga_len,
		    ga.ga_maxlen - ga.ga_len,
		    "[%ld]",
		    lnum);

		ga.ga_len += (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)added;
	    }
	    ଯଦି (idx != exestack.ga_len - 1)
		GA_CONCAT_LITERAL(&ga, "..");
	}
    }

    // Only NUL-terminate when not returning NULL.
    ଯଦି (ga.ga_data != ଖାଲି)
	ga_append(&ga, NUL);
    ଫେରନ୍ତୁ (char_u *)ga.ga_data;
#endif
}

#ifdef FEAT_EVAL
    ସ୍ଥିର ଶୂନ୍ୟ
stacktrace_ଠେଲ_item(
	list_T		*l,
	ufunc_T		*fp,
	char_u		*event,
	linenr_T	lnum,
	char_u		*filepath)
{
    dict_T	*d;
    typval_T	tv;

    d = dict_alloc_lock(VAR_FIXED);
    ଯଦି (d == ଖାଲି)
	ଫେରନ୍ତୁ;

    tv.v_type = VAR_DICT;
    tv.v_lock = VAR_LOCKED;
    tv.vval.v_dict = d;

    ଯଦି (fp != ଖାଲି)
	dict_add_func(d, "funcref", fp);
    ଯଦି (event != ଖାଲି)
	dict_add_string(d, "event", event);
    dict_add_ସଂଖ୍ୟା(d, "lnum", lnum);
    dict_add_string(d, "filepath", filepath);

    list_append_tv(l, &tv);
}

/*
 * Create the stacktrace from exestack.
 */
    list_T *
stacktrace_create(ଶୂନ୍ୟ)
{
    list_T	*l;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    l = list_alloc();
    ଯଦି (l == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    ପାଇଁ (i = 0; i < exestack.ga_len; ++i)
    {
	estack_T *entry = &((estack_T *)exestack.ga_data)[i];
	linenr_T lnum = entry->es_lnum;

	ଯଦି (entry->es_type == ETYPE_SCRIPT)
	    stacktrace_ଠେଲ_item(l, ଖାଲି, ଖାଲି, lnum, entry->es_name);
	ଅନ୍ୟଥା ଯଦି (entry->es_type == ETYPE_UFUNC)
	{
	    ufunc_T *fp = entry->es_info.ufunc;
	    sctx_T sctx = fp->uf_script_ctx;
	    char_u *filepath = sctx.sc_sid > 0 ?
				   get_scriptname(sctx.sc_sid) : (char_u *)"";

	    lnum += sctx.sc_lnum;
	    stacktrace_ଠେଲ_item(l, fp, ଖାଲି, lnum, filepath);
	}
	ଅନ୍ୟଥା ଯଦି (entry->es_type == ETYPE_AUCMD)
	{
	    // The autocmd may not have a matching pattern yet, in which case
	    // es_info.aucmd is still NULL.
	    sctx_T *sctx = entry->es_info.aucmd != ଖାଲି
			       ? acp_script_ctx(entry->es_info.aucmd) : ଖାଲି;
	    char_u *filepath = sctx != ଖାଲି && sctx->sc_sid > 0 ?
				   get_scriptname(sctx->sc_sid) : (char_u *)"";

	    ଯଦି (sctx != ଖାଲି)
		lnum += sctx->sc_lnum;
	    stacktrace_ଠେଲ_item(l, ଖାଲି, entry->es_name, lnum, filepath);
	}
    }
    ଫେରନ୍ତୁ l;
}

/*
 * getstacktrace() function
 */
    ଶୂନ୍ୟ
f_getstacktrace(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv_list_set(rettv, stacktrace_create());
}
#endif

/*
 * Get DIP_ flags from the [where] argument of a :runtime command.
 * "*argp" is advanced to after the [where] argument if it is found.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_runtime_cmd_flags(char_u **argp, size_t where_len)
{
    char_u *arg = *argp;

    ଯଦି (where_len == 0)
	ଫେରନ୍ତୁ 0;

    ଯଦି (STRNCMP(arg, "START", where_len) == 0)
    {
	*argp = skipwhite(arg + where_len);
	ଫେରନ୍ତୁ DIP_START + DIP_NORTP;
    }
    ଯଦି (STRNCMP(arg, "OPT", where_len) == 0)
    {
	*argp = skipwhite(arg + where_len);
	ଫେରନ୍ତୁ DIP_OPT + DIP_NORTP;
    }
    ଯଦି (STRNCMP(arg, "PACK", where_len) == 0)
    {
	*argp = skipwhite(arg + where_len);
	ଫେରନ୍ତୁ DIP_START + DIP_OPT + DIP_NORTP;
    }
    ଯଦି (STRNCMP(arg, "ALL", where_len) == 0)
    {
	*argp = skipwhite(arg + where_len);
	ଫେରନ୍ତୁ DIP_START + DIP_OPT;
    }

    ଫେରନ୍ତୁ 0;
}

/*
 * ":runtime [where] {name}"
 */
    ଶୂନ୍ୟ
ex_runtime(exarg_T *eap)
{
    char_u  *arg = eap->arg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    flags = eap->forceit ? DIP_ALL : 0;
    char_u  *p = skiptowhite(arg);
    flags += get_runtime_cmd_flags(&arg, p - arg);
    source_runtime(arg, flags);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା runtime_expand_flags;

/*
 * Set the completion context for the :runtime command.
 */
    ଶୂନ୍ୟ
set_context_in_runtime_cmd(expand_T *xp, char_u *arg)
{
    char_u  *p = skiptowhite(arg);
    runtime_expand_flags
	= *p != NUL ? get_runtime_cmd_flags(&arg, p - arg) : 0;
    // Skip to the last argument.
    ଯେତେବେଳେ (*(p = skiptowhite_esc(arg)) != NUL)
    {
	ଯଦି (runtime_expand_flags == 0)
	    // When there are multiple arguments and [where] is not specified,
	    // use an unrelated non-zero flag to avoid expanding [where].
	    runtime_expand_flags = DIP_ALL;
	arg = skipwhite(p);
    }
    xp->xp_context = EXPAND_RUNTIME;
    xp->xp_pattern = arg;
}

    ସ୍ଥିର ଶୂନ୍ୟ
source_callback(char_u *fname, ଶୂନ୍ୟ *cookie)
{
    (ଶୂନ୍ୟ)do_source(fname, FALSE, DOSO_NONE, cookie);
}

#ifdef FEAT_EVAL
/*
 * Find an already loaded script "name".
 * If found returns its script ID.  If not found returns -1.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
find_script_by_name(char_u *name)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    sid;
    scriptitem_T    *si;

    ପାଇଁ (sid = script_ବସ୍ତୁଗୁଡ଼ିକ.ga_len; sid > 0; --sid)
    {
	// We used to check inode here, but that doesn't work:
	// - If a script is edited and written, it may get a different
	//   inode number, even though to the user it is the same script.
	// - If a script is deleted and another script is written, with a
	//   different name, the inode may be re-used.
	si = SCRIPT_ITEM(sid);
	ଯଦି (si->sn_name != ଖାଲି && fnamecmp(si->sn_name, name) == 0)
	    ଫେରନ୍ତୁ sid;
    }
    ଫେରନ୍ତୁ -1;
}

/*
 * Add a new scriptitem with all items initialized.
 * When running out of memory "error" is set to FAIL.
 * Returns the script ID.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_new_scriptitem(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *error)
{
    ସ୍ଥିର scid_T   last_current_SID = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    sid = ++last_current_SID;
    scriptitem_T    *si = ଖାଲି;

    ଯଦି (ga_grow(&script_ବସ୍ତୁଗୁଡ଼ିକ, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(sid - script_ବସ୍ତୁଗୁଡ଼ିକ.ga_len)) == FAIL)
    {
	*error = FAIL;
	ଫେରନ୍ତୁ sid;
    }
    ଯେତେବେଳେ (script_ବସ୍ତୁଗୁଡ଼ିକ.ga_len < sid)
    {
	si = ALLOC_CLEAR_ONE(scriptitem_T);
	ଯଦି (si == ଖାଲି)
	{
	    *error = FAIL;
	    ଫେରନ୍ତୁ sid;
	}
	++script_ବସ୍ତୁଗୁଡ଼ିକ.ga_len;
	SCRIPT_ITEM(script_ବସ୍ତୁଗୁଡ଼ିକ.ga_len) = si;
	si->sn_name = ଖାଲି;
	si->sn_version = 1;

	// Allocate the local script variables to use for this script.
	new_script_vars(script_ବସ୍ତୁଗୁଡ଼ିକ.ga_len);
	ga_init2(&si->sn_var_vals, ଆକାର(svar_T), 10);
	hash_init(&si->sn_all_vars.dv_hashtab);
	ga_init2(&si->sn_imports, ଆକାର(imported_T), 10);
	ga_init2(&si->sn_type_list, ଆକାର(type_T), 10);
# ifdef FEAT_PROFILE
	si->sn_prof_on = FALSE;
# endif
    }

    // "si" can't be NULL, check only to avoid a compiler warning
    ଯଦି (si != ଖାଲି)
	// Used to check script variable index is still valid.
	si->sn_script_seq = current_sctx.sc_seq;

    ଫେରନ୍ତୁ sid;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_new_scriptitem_for_fname(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *error, char_u *fname)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା sid = get_new_scriptitem(error);

    ଯଦି (*error == OK)
    {
	scriptitem_T *si = SCRIPT_ITEM(sid);

	si->sn_name = vim_strsave(fname);
	si->sn_state = SN_STATE_NOT_LOADED;
    }
    ଫେରନ୍ତୁ sid;
}

/*
 * If the script for "sid" is a symlink and "sn_source_sid" is not set
 * then initialize it. A new script_item is created if needed.
 */
    ଶୂନ୍ୟ
check_script_symlink(ପୂର୍ଣ୍ଣସଂଖ୍ୟା sid)
{
    scriptitem_T *si = SCRIPT_ITEM(sid);
    ଯଦି (si->sn_syml_checked || si->sn_sourced_sid > 0)
	ଫେରନ୍ତୁ;
    si->sn_syml_checked = TRUE;

    // If fname is a symbolic link, create an script_item for the real file.

    char_u *real_fname = fix_fname(si->sn_name);
    ଯଦି (real_fname != ଖାଲି && STRCMP(real_fname, si->sn_name) != 0)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା real_sid = find_script_by_name(real_fname);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା error2 = OK;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା new_sid = FALSE;
	ଯଦି (real_sid < 0)
	{
	    real_sid = get_new_scriptitem_for_fname(&error2, real_fname);
	    new_sid = TRUE;
	}
	ଯଦି (error2 == OK)
	{
	    si = SCRIPT_ITEM(sid);
	    si->sn_sourced_sid = real_sid;
	    ଯଦି (new_sid)
	    {
		SCRIPT_ITEM(real_sid)->sn_import_autoload
						    = si->sn_import_autoload;
		ଯଦି (si->sn_autoload_prefix != ଖାଲି)
		    SCRIPT_ITEM(real_sid)->sn_autoload_prefix =
					vim_strsave(si->sn_autoload_prefix);
	    }
	}
    }
    vim_free(real_fname);
}

    ସ୍ଥିର ଶୂନ୍ୟ
find_script_callback(char_u *fname, ଶୂନ୍ୟ *cookie)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା sid;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା error = OK;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret_sid = cookie;

    sid = find_script_by_name(fname);
    ଯଦି (sid < 0)
	// script does not exist yet, create a new scriptitem
	sid = get_new_scriptitem_for_fname(&error, fname);
    *ret_sid = sid;
}
#endif

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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
do_in_path(
    char_u	*path,
    ଅକ୍ଷର	*prefix,
    char_u	*name,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags,
    ଶୂନ୍ୟ	(*callback)(char_u *fname, ଶୂନ୍ୟ *ck),
    ଶୂନ୍ୟ	*cookie)
{
    char_u	*rtp;
    string_T	buf;
    char_u	*rtp_copy;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଅଙ୍କ_files;
    char_u	**files;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_one = FALSE;
#ifdef AMIGA
    ସଂରଚନା Process	*proc = (ସଂରଚନା Process *)FindTask(0L);
    APTR		save_winptr = proc->pr_WindowPtr;

    // Avoid a requester here for a volume that doesn't exist.
    proc->pr_WindowPtr = (APTR)-1L;
#endif

    // Make a copy of 'runtimepath'.  Invoking the callback may change the
    // value.
    rtp_copy = vim_strsave(path);
    buf.string = alloc(MAXPATHL);
    ଯଦି (buf.string != ଖାଲି && rtp_copy != ଖାଲି)
    {
	size_t	prefixlen = 0;
	size_t	namelen = 0;

	ଯଦି (name != ଖାଲି)
	{
	    prefixlen = STRLEN(prefix);
	    namelen = STRLEN(name);

	    ଯଦି (p_verbose > 10)
	    {
		verbose_enter();
		ଯଦି (*prefix != NUL)
		    smsg(_("Searching for \"%s\" under \"%s\" in \"%s\""),
					       (ଅକ୍ଷର *)name, prefix, (ଅକ୍ଷର *)path);
		ଅନ୍ୟଥା
		    smsg(_("Searching for \"%s\" in \"%s\""),
						       (ଅକ୍ଷର *)name, (ଅକ୍ଷର *)path);
		verbose_leave();
	    }
	}

	// Loop over all entries in 'runtimepath'.
	rtp = rtp_copy;
	ଯେତେବେଳେ (*rtp != NUL && ((flags & DIP_ALL) || !did_one))
	{
	    // Copy the path from 'runtimepath' to buf[].
	    buf.length = (size_t)copy_option_part(&rtp, buf.string, MAXPATHL, ",");

	    // Skip after or non-after directories.
	    ଯଦି (flags & (DIP_NOAFTER | DIP_AFTER))
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_after = buf.length >= 5
		    && STRCMP(buf.string + buf.length - 5, "after") == 0;

		ଯଦି ((is_after && (flags & DIP_NOAFTER))
			|| (!is_after && (flags & DIP_AFTER)))
		    ଜାରି;
	    }

	    ଯଦି (name == ଖାଲି)
	    {
		(*callback)(buf.string, (ଶୂନ୍ୟ *)&cookie);
		ଯଦି (!did_one)
		    did_one = (cookie == ଖାଲି);
	    }
	    ଅନ୍ୟଥା ଯଦି (buf.length + 2 + prefixlen + namelen < MAXPATHL)
	    {
		char_u	*np;
		char_u	*tail;

		ଯଦି (*buf.string != NUL
		    && !after_pathsep(buf.string, buf.string + buf.length))
		{
		    STRCPY(buf.string + buf.length, PATHSEPSTR);
		    buf.length += ଆକାର(PATHSEP);
		}
		STRCPY(buf.string + buf.length, prefix);
		buf.length += prefixlen;
		tail = buf.string + buf.length;

		// Loop over all patterns in "name"
		np = name;
		ଯେତେବେଳେ (*np != NUL && ((flags & DIP_ALL) || !did_one))
		{
		    // Append the pattern from "name" to buf[].
		    copy_option_part(&np, tail, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(MAXPATHL - (tail - buf.string)),
								       "\t ");

		    ଯଦି (p_verbose > 10)
		    {
			verbose_enter();
			smsg(_("Searching for \"%s\""), buf.string);
			verbose_leave();
		    }

		    // Expand wildcards, invoke the callback for each match.
		    ଯଦି (gen_expand_wildcards(1, &buf.string, &ଅଙ୍କ_files, &files,
				  (flags & DIP_DIR) ? EW_DIR : EW_FILE) == OK)
		    {
			ପାଇଁ (i = 0; i < ଅଙ୍କ_files; ++i)
			{
			    (*callback)(files[i], cookie);
			    did_one = TRUE;
			    ଯଦି (!(flags & DIP_ALL))
				ଭାଙ୍ଗ;
			}
			FreeWild(ଅଙ୍କ_files, files);
		    }
		}
	    }
	}
    }
    vim_free(buf.string);
    vim_free(rtp_copy);
    ଯଦି (!did_one && name != ଖାଲି)
    {
	ଅକ୍ଷର *basepath = path == p_rtp ? "runtimepath" : "packpath";

	ଯଦି (flags & DIP_ERR)
	    semsg(_(e_directory_not_found_in_str_str), basepath, name);
	ଅନ୍ୟଥା ଯଦି (p_verbose > 0)
	{
	    verbose_enter();
	    smsg(_("not found in '%s': \"%s\""), basepath, name);
	    verbose_leave();
	}
    }

#ifdef AMIGA
    proc->pr_WindowPtr = save_winptr;
#endif

    ଫେରନ୍ତୁ did_one ? OK : FAIL;
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
do_in_path_and_pp(
    char_u	*path,
    char_u	*name,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags,
    ଶୂନ୍ୟ	(*callback)(char_u *fname, ଶୂନ୍ୟ *ck),
    ଶୂନ୍ୟ	*cookie)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		done = FAIL;

    ଯଦି ((flags & DIP_NORTP) == 0)
	done = do_in_path(path, "", name, flags, callback, cookie);

    ଯଦି ((done == FAIL || (flags & DIP_ALL)) && (flags & DIP_START))
	done = do_in_path(p_pp, "pack/*/start/*/", name, flags, callback,
								       cookie);

    ଯଦି ((done == FAIL || (flags & DIP_ALL)) && (flags & DIP_OPT))
	done = do_in_path(p_pp, "pack/*/opt/*/", name, flags, callback,
								       cookie);

    ଫେରନ୍ତୁ done;
}

/*
 * Just like do_in_path_and_pp(), using 'runtimepath' for "path".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
do_in_runtimepath(
    char_u	*name,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags,
    ଶୂନ୍ୟ	(*callback)(char_u *fname, ଶୂନ୍ୟ *ck),
    ଶୂନ୍ୟ	*cookie)
{
    ଫେରନ୍ତୁ do_in_path_and_pp(p_rtp, name, flags, callback, cookie);
}

/*
 * Source the file "name" from all directories in 'runtimepath'.
 * "name" can contain wildcards.
 * When "flags" has DIP_ALL: source all files, otherwise only the first one.
 *
 * return FAIL when no file could be sourced, OK otherwise.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
source_runtime(char_u *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{
    ଫେରନ୍ତୁ source_in_path(p_rtp, name, flags, ଖାଲି);
}

/*
 * Just like source_runtime(), but use "path" instead of 'runtimepath'
 * and return the script ID in "ret_sid".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
source_in_path(char_u *path, char_u *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret_sid)
{
    ଫେରନ୍ତୁ do_in_path_and_pp(path, name, flags, source_callback, ret_sid);
}

#ଯଦି defined(FEAT_EVAL)

/*
 * Find "name" in 'runtimepath'. If found a new scriptitem is created for it
 * and it's script ID is returned.
 * If not found returns -1.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
find_script_in_rtp(char_u *name)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା sid = -1;

    (ଶୂନ୍ୟ)do_in_path_and_pp(p_rtp, name, DIP_START | DIP_NOAFTER,
						   find_script_callback, &sid);
    ଫେରନ୍ତୁ sid;
}

/*
 * Expand wildcards in "pat" and invoke do_source() for each match.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
source_all_matches(char_u *pat)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    ଅଙ୍କ_files;
    char_u  **files;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    i;

    ଯଦି (gen_expand_wildcards(1, &pat, &ଅଙ୍କ_files, &files, EW_FILE) != OK)
	ଫେରନ୍ତୁ;

    ପାଇଁ (i = 0; i < ଅଙ୍କ_files; ++i)
	(ଶୂନ୍ୟ)do_source(files[i], FALSE, DOSO_NONE, ଖାଲି);
    FreeWild(ଅଙ୍କ_files, files);
}

/*
 * Add the package directory to 'runtimepath'.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
add_pack_dir_to_rtp(char_u *fname)
{
    char_u  *p4, *p3, *p2, *p1, *p;
    char_u  *entry;
    char_u  *insp = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    c;
    string_T  new_rtp;
    size_t  keep;
    size_t  p_rtp_len;
    string_T  afterdir = {ଖାଲି, 0};
    char_u  *after_insp = ଖାଲି;
    char_u  *ffname = ଖାଲି;
    size_t  fname_len;
    string_T	buf = {ଖାଲି, 0};
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    retval = FAIL;

    p4 = p3 = p2 = p1 = get_past_head(fname);
    ପାଇଁ (p = p1; *p; MB_PTR_ADV(p))
	ଯଦି (vim_ispathsep_nocolon(*p))
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
    ଯଦି (ffname == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    // Find "ffname" in "p_rtp", ignoring '/' vs '\' differences.
    // Also stop at the first "after" directory.
    fname_len = STRLEN(ffname);
    buf.string = alloc(MAXPATHL);
    ଯଦି (buf.string == ଖାଲି)
	ଯାଅ theend;

    p_rtp_len = 0;
    ପାଇଁ (entry = p_rtp; *entry != NUL; )
    {
	char_u *cur_entry = entry;

	buf.length = (size_t)copy_option_part(&entry, buf.string, MAXPATHL, ",");

	// keep track of p_rtp length as we go to make the STRLEN() below have less work to do
	p_rtp_len += (*(cur_entry + buf.length) == ',') ? buf.length + 1 : buf.length;

	ଯଦି ((p = (char_u *)strstr((ଅକ୍ଷର *)buf.string, "after")) != ଖାଲି
		&& p > buf.string
		&& vim_ispathsep(p[-1])
		&& (vim_ispathsep(p[5]) || p[5] == NUL || p[5] == ','))
	{
	    ଯଦି (insp == ଖାଲି)
		// Did not find "ffname" before the first "after" directory,
		// insert it before this entry.
		insp = cur_entry;
	    after_insp = cur_entry;
	    ଭାଙ୍ଗ;
	}

	ଯଦି (insp == ଖାଲି)
	{
	    char_u  *rtp_ffname;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    match;

	    ଯଦି (*buf.string != NUL && !after_pathsep(buf.string, buf.string + buf.length))
		STRCPY(buf.string + buf.length, PATHSEPSTR);
	    rtp_ffname = fix_fname(buf.string);
	    ଯଦି (rtp_ffname == ଖାଲି)
		ଯାଅ theend;
	    match = vim_fnamencmp(rtp_ffname, ffname, fname_len) == 0;
	    vim_free(rtp_ffname);
	    ଯଦି (match)
		// Insert "ffname" after this entry (and comma).
		insp = entry;
	}
    }

    // finish measuring the length of p_rtp
    p_rtp_len += STRLEN(p_rtp + p_rtp_len);
    ଯଦି (insp == ଖାଲି)
	// Both "fname" and "after" not found, append at the end.
	insp = p_rtp + p_rtp_len;

    // check if rtp/pack/name/start/name/after exists
    fname_len = STRLEN(fname);
    concat_fnames(fname, fname_len, (char_u *)"after", STRLEN_LITERAL("after"), TRUE, &afterdir);
    ଯଦି (afterdir.string == ଖାଲି || !mch_isdir(afterdir.string))
	afterdir.length = 0;

    new_rtp.string = alloc(p_rtp_len + fname_len + afterdir.length + 3); // add two for commas and one for NUL
    ଯଦି (new_rtp.string == ଖାଲି)
	ଯାଅ theend;

    // We now have 'rtp' parts: {keep}{keep_after}{rest}.
    // Create new_rtp, first: {keep},{fname}
    keep = (size_t)(insp - p_rtp);
    mch_memmove(new_rtp.string, p_rtp, keep);
    new_rtp.length = keep;
    ଯଦି (*insp == NUL)
	new_rtp.string[new_rtp.length++] = ',';  // add comma before
    mch_memmove(new_rtp.string + new_rtp.length, fname, fname_len);
    new_rtp.length += fname_len;
    ଯଦି (*insp != NUL)
	new_rtp.string[new_rtp.length++] = ',';  // add comma after

    ଯଦି (afterdir.length > 0 && after_insp != ଖାଲି)
    {
	size_t keep_after = (size_t)(after_insp - p_rtp);
	size_t append_len = keep_after - keep;

	// Add to new_rtp: {keep},{fname}{keep_after},{afterdir}
	mch_memmove(new_rtp.string + new_rtp.length, p_rtp + keep,
							append_len);
	new_rtp.length += append_len;
	mch_memmove(new_rtp.string + new_rtp.length, afterdir.string, afterdir.length);
	new_rtp.length += afterdir.length;
	new_rtp.string[new_rtp.length++] = ',';
	keep = keep_after;
    }

    ଯଦି (p_rtp[keep] != NUL)
    {
	size_t	append_len = p_rtp_len - keep;

	// Append rest: {keep},{fname}{keep_after},{afterdir}{rest}
	mch_memmove(new_rtp.string + new_rtp.length, p_rtp + keep, append_len + 1);	// add one for NUL
	new_rtp.length += append_len;
    }
    ଅନ୍ୟଥା
	new_rtp.string[new_rtp.length] = NUL;

    ଯଦି (afterdir.length > 0 && after_insp == ଖାଲି)
    {
	// Append afterdir when "after" was not found:
	// {keep},{fname}{rest},{afterdir}
	new_rtp.string[new_rtp.length++] = ',';
	STRCPY(new_rtp.string + new_rtp.length, afterdir.string);
    }

    set_option_ମୂଲ୍ୟ_give_err((char_u *)"rtp", 0L, new_rtp.string, 0);
    vim_free(new_rtp.string);
    retval = OK;

theend:
    vim_free(buf.string);
    vim_free(ffname);
    vim_free(afterdir.string);
    ଫେରନ୍ତୁ retval;
}

/*
 * Load scripts in "plugin" and "ftdetect" directories of the package.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
load_pack_plugin(char_u *fname)
{
    ସ୍ଥିର ଅକ୍ଷର *plugpat = "%s/plugin/**/*.vim";
    ସ୍ଥିର ଅକ୍ଷର *ftpat = "%s/ftdetect/*.vim";
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    char_u	*ffname = fix_fname(fname);
    char_u	*pat = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = FAIL;

    ଯଦି (ffname == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(ffname) + (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(ftpat);
    pat = alloc(len);
    ଯଦି (pat == ଖାଲି)
	ଯାଅ theend;
    vim_snprintf((ଅକ୍ଷର *)pat, len, plugpat, ffname);
    source_all_matches(pat);

    {
	char_u *cmd = vim_strsave((char_u *)"g:did_load_filetypes");

	// If runtime/filetype.vim wasn't loaded yet, the scripts will be
	// found when it loads.
	ଯଦି (cmd != ଖାଲି && eval_to_ସଂଖ୍ୟା(cmd, FALSE) > 0)
	{
	    do_cmdline_cmd((char_u *)"augroup filetypedetect");
	    vim_snprintf((ଅକ୍ଷର *)pat, len, ftpat, ffname);
	    source_all_matches(pat);
	    do_cmdline_cmd((char_u *)"augroup END");
	}
	vim_free(cmd);
    }
    vim_free(pat);
    retval = OK;

theend:
    vim_free(ffname);
    ଫେରନ୍ତୁ retval;
}

// used for "cookie" of add_pack_plugin()
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା APP_ADD_DIR;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା APP_LOAD;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା APP_BOTH;

    ସ୍ଥିର ଶୂନ୍ୟ
add_pack_plugin(char_u *fname, ଶୂନ୍ୟ *cookie)
{
    ଯଦି (cookie != &APP_LOAD)
    {
	char_u	*buf = alloc(MAXPATHL);
	char_u	*p;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	found = FALSE;

	ଯଦି (buf == ଖାଲି)
	    ଫେରନ୍ତୁ;
	p = p_rtp;
	ଯେତେବେଳେ (*p != NUL)
	{
	    copy_option_part(&p, buf, MAXPATHL, ",");
	    ଯଦି (pathcmp((ଅକ୍ଷର *)buf, (ଅକ୍ଷର *)fname, -1) == 0)
	    {
		found = TRUE;
		ଭାଙ୍ଗ;
	    }
	}
	vim_free(buf);
	ଯଦି (!found)
	    // directory is not yet in 'runtimepath', add it
	    ଯଦି (add_pack_dir_to_rtp(fname) == FAIL)
		ଫେରନ୍ତୁ;
    }

    ଯଦି (cookie != &APP_ADD_DIR)
	load_pack_plugin(fname);
}

/*
 * Add all packages in the "start" directory to 'runtimepath'.
 */
    ଶୂନ୍ୟ
add_pack_start_dirs(ଶୂନ୍ୟ)
{
    do_in_path(p_pp, "", (char_u *)"pack/*/start/*", DIP_ALL + DIP_DIR,
					       add_pack_plugin, &APP_ADD_DIR);
}

/*
 * Load plugins from all packages in the "start" directory.
 */
    ଶୂନ୍ୟ
load_start_packages(ଶୂନ୍ୟ)
{
    did_source_packages = TRUE;
    do_in_path(p_pp, "", (char_u *)"pack/*/start/*", DIP_ALL + DIP_DIR,
						  add_pack_plugin, &APP_LOAD);
}

/*
 * ":packloadall"
 * Find plugins in the package directories and source them.
 */
    ଶୂନ୍ୟ
ex_packloadall(exarg_T *eap)
{
    ଯଦି (!did_source_packages || eap->forceit)
    {
	// First do a round to add all directories to 'runtimepath', then load
	// the plugins. This allows for plugins to use an autoload directory
	// of another plugin.
	add_pack_start_dirs();
	load_start_packages();
    }
}

/*
 * ":packadd[!] {name}"
 */
    ଶୂନ୍ୟ
ex_packadd(exarg_T *eap)
{
    ସ୍ଥିର ଅକ୍ଷର *plugpat = "pack/*/%s/%s";
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    ଅକ୍ଷର	*pat;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		round;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		res = OK;

    // Round 1: use "start", round 2: use "opt".
    ପାଇଁ (round = 1; round <= 2; ++round)
    {
	// Only look under "start" when loading packages wasn't done yet.
	ଯଦି (round == 1 && did_source_packages)
	    ଜାରି;

	len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(plugpat) + (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(eap->arg) + 5;
	pat = alloc(len);
	ଯଦି (pat == ଖାଲି)
	    ଫେରନ୍ତୁ;
	vim_snprintf(pat, len, plugpat, round == 1 ? "start" : "opt", eap->arg);
	// The first round don't give a "not found" error, in the second round
	// only when nothing was found in the first round.
	res = do_in_path(p_pp, "", (char_u *)pat,
		DIP_ALL + DIP_DIR + (round == 2 && res == FAIL ? DIP_ERR : 0),
		add_pack_plugin, eap->forceit ? &APP_ADD_DIR : &APP_BOTH);
	vim_free(pat);
    }
}
#endif

/*
 * Sort "gap" and remove duplicate entries.  "gap" is expected to contain a
 * list of file names in allocated memory.
 */
    ଶୂନ୍ୟ
remove_duplicates(garray_T *gap)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    j;
    char_u  **fnames = (char_u **)gap->ga_data;

    sort_strings(fnames, gap->ga_len);
    ପାଇଁ (i = gap->ga_len - 1; i > 0; --i)
	ଯଦି (fnamecmp(fnames[i - 1], fnames[i]) == 0)
	{
	    vim_free(fnames[i]);
	    ପାଇଁ (j = i + 1; j < gap->ga_len; ++j)
		fnames[j - 1] = fnames[j];
	    --gap->ga_len;
	}
}

    ସ୍ଥିର ଶୂନ୍ୟ
ExpandRTDir_int(
    char_u	*pat,
    size_t	pat_len,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		keep_ext,
    garray_T	*gap,
    ଅକ୍ଷର	*dirnames[])
{
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; dirnames[i] != ଖାଲି; ++i)
    {
	ଅଚଳ size_t	buf_len = STRLEN(dirnames[i]) + pat_len + 64;
	ଅକ୍ଷର		*buf = alloc(buf_len);
	ଯଦି (buf == ଖାଲି)
	{
	    ga_clear_strings(gap);
	    ଫେରନ୍ତୁ;
	}
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		glob_flags = 0;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		expand_dirs = FALSE;

	// Build base pattern
	vim_snprintf(buf, buf_len, "%s%s%s%s",
		     *dirnames[i] ? dirnames[i] : "", *dirnames[i] ? "/" : "",
		     pat, "*.vim");

expand:
	ଯଦି ((flags & DIP_NORTP) == 0)
	    globpath(p_rtp, (char_u *)buf, gap, glob_flags, expand_dirs);

	ଯଦି (flags & DIP_START)
	{
	    // Build complete search path: pack/*/start/*/dirnames[i]/pat*.vim
	    vim_snprintf(buf, buf_len, "pack/*/start/*/%s%s%s%s",
			 *dirnames[i] ? dirnames[i] : "",
			 *dirnames[i] ? "/" : "",
			 pat,
			 expand_dirs ? "*" : "*.vim");
	    globpath(p_pp, (char_u *)buf, gap, glob_flags, expand_dirs);
	}

	ଯଦି (flags & DIP_OPT)
	{
	    // Build complete search path: pack/*/opt/*/dirnames[i]/pat*.vim
	    vim_snprintf(buf, buf_len, "pack/*/opt/*/%s%s%s%s",
			 *dirnames[i] ? dirnames[i] : "",
			 *dirnames[i] ? "/" : "", pat,
			 expand_dirs ? "*" : "*.vim");
	    globpath(p_pp, (char_u *)buf, gap, glob_flags, expand_dirs);
	}

	// Second round for directories
	ଯଦି (*dirnames[i] == NUL && !expand_dirs)
	{
	    // expand dir names in another round
	    vim_snprintf(buf, buf_len, "%s*", pat);
	    glob_flags = WILD_ADD_SLASH;
	    expand_dirs = TRUE;
	    ଯାଅ expand;
	}

	vim_free(buf);
    }

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା pat_pathsep_cnt = 0;
    ପାଇଁ (size_t i = 0; i < pat_len; ++i)
    {
	ଯଦି (vim_ispathsep(pat[i]))
	    ++pat_pathsep_cnt;
    }

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < gap->ga_len; ++i)
    {
	char_u *match = ((char_u **)gap->ga_data)[i];
	char_u *s = match;
	char_u *e = s + STRLEN(s);
	ଯଦି (e - 4 > s && !keep_ext && STRNICMP(e - 4, ".vim", 4) == 0)
	{
	    e -= 4;
	    *e = NUL;
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା match_pathsep_cnt = (e > s && e[-1] == '/') ? -1 : 0;
	ପାଇଁ (s = e; s > match; MB_PTR_BACK(match, s))
	{
	    ଯଦି (s < match || (vim_ispathsep(*s)
				     && ++match_pathsep_cnt > pat_pathsep_cnt))
		ଭାଙ୍ଗ;
	}
	++s;
	ଯଦି (s != match)
	    mch_memmove(match, s, e - s + 1);
    }

    ଯଦି (gap->ga_len == 0)
	ଫେରନ୍ତୁ;

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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ExpandRTDir(
    char_u	*pat,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*ଅଙ୍କ_file,
    char_u	***file,
    ଅକ୍ଷର	*dirnames[])
{
    *ଅଙ୍କ_file = 0;
    *file = ଖାଲି;

    garray_T	ga;
    ga_init2(&ga, ଆକାର(ଅକ୍ଷର *), 10);

    ExpandRTDir_int(pat, STRLEN(pat), flags, FALSE, &ga, dirnames);

    ଯଦି (ga.ga_len == 0)
	ଫେରନ୍ତୁ FAIL;

    *file = ga.ga_data;
    *ଅଙ୍କ_file = ga.ga_len;
    ଫେରନ୍ତୁ OK;
}

/*
 * Handle command line completion for :runtime command.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
expand_runtime_cmd(char_u *pat, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_Matches, char_u ***matches)
{
    *ଅଙ୍କ_Matches = 0;
    *matches = ଖାଲି;

    garray_T	ga;
    ga_init2(&ga, ଆକାର(ଅକ୍ଷର *), 10);

    size_t pat_len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(pat);
    ଅକ୍ଷର *dirnames[] = {"", ଖାଲି};
    ExpandRTDir_int(pat, pat_len, runtime_expand_flags, TRUE, &ga, dirnames);

    // Try to complete values for [where] argument when none was found.
    ଯଦି (runtime_expand_flags == 0)
    {
	ଅକ୍ଷର *where_ମୂଲ୍ୟଗୁଡ଼ିକ[] = {"START", "OPT", "PACK", "ALL"};
	ପାଇଁ (size_t i = 0; i < ARRAY_LENGTH(where_ମୂଲ୍ୟଗୁଡ଼ିକ); ++i)
	    ଯଦି (STRNCMP(pat, where_ମୂଲ୍ୟଗୁଡ଼ିକ[i], pat_len) == 0)
	    {
		char_u *p = vim_strsave((char_u *)where_ମୂଲ୍ୟଗୁଡ଼ିକ[i]);
		ଯଦି (p != ଖାଲି && ga_add_string(&ga, p) == FAIL)
		    vim_free(p);
	    }
    }

    ଯଦି (ga.ga_len == 0)
	ଫେରନ୍ତୁ FAIL;

    *matches = ga.ga_data;
    *ଅଙ୍କ_Matches = ga.ga_len;
    ଫେରନ୍ତୁ OK;
}

/*
 * Expand loadplugin names:
 * 'packpath'/pack/ * /opt/{pat}
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ExpandPackAddDir(
    char_u	*pat,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*ଅଙ୍କ_file,
    char_u	***file)
{
    char_u	*s;
    char_u	*e;
    char_u	*match;
    garray_T	ga;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		pat_len;

    *ଅଙ୍କ_file = 0;
    *file = ଖାଲି;
    pat_len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(pat);
    ga_init2(&ga, ଆକାର(ଅକ୍ଷର *), 10);

    s = alloc(pat_len + 26);
    ଯଦି (s == ଖାଲି)
    {
	ga_clear_strings(&ga);
	ଫେରନ୍ତୁ FAIL;
    }
    sprintf((ଅକ୍ଷର *)s, "pack/*/opt/%s*", pat);
    globpath(p_pp, s, &ga, 0, TRUE);
    vim_free(s);

    ପାଇଁ (i = 0; i < ga.ga_len; ++i)
    {
	match = ((char_u **)ga.ga_data)[i];
	s = gettail(match);
	e = s + STRLEN(s);
	mch_memmove(match, s, e - s + 1);
    }

    ଯଦି (ga.ga_len == 0)
	ଫେରନ୍ତୁ FAIL;

    // Sort and remove duplicates which can happen when specifying multiple
    // directories in dirnames.
    remove_duplicates(&ga);

    *file = ga.ga_data;
    *ଅଙ୍କ_file = ga.ga_len;
    ଫେରନ୍ତୁ OK;
}

    ସ୍ଥିର ଶୂନ୍ୟ
cmd_source(char_u *fname, exarg_T *eap)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା clearvars = FALSE;

    ଯଦି (*fname != NUL && STRNCMP(fname, "++clear", 7) == 0)
    {
	// ++clear argument is supplied
	clearvars = TRUE;
	fname = fname + 7;
	ଯଦି (*fname != NUL)
	{
	    semsg(_(e_invalid_argument_str), eap->arg);
	    ଫେରନ୍ତୁ;
	}
    }

    ଯଦି (*fname != NUL && eap != ଖାଲି && eap->addr_ଗଣନା > 0)
    {
	// if a filename is specified to :source, then a range is not allowed
	emsg(_(e_no_range_allowed));
	ଫେରନ୍ତୁ;
    }

    ଯଦି (eap != ଖାଲି && *fname == NUL)
    {
	ଯଦି (eap->forceit)
	    // a file name is needed to source normal mode commands
	    emsg(_(e_argument_required));
	ଅନ୍ୟଥା
	    // source ex commands from the current buffer
	    do_source_ext(ଖାଲି, FALSE, DOSO_NONE, ଖାଲି, eap, clearvars);
    }
    ଅନ୍ୟଥା ଯଦି (eap != ଖାଲି && eap->forceit)
	// ":source!": read Normal mode commands
	// Need to execute the commands directly.  This is required at least
	// for:
	// - ":g" command busy
	// - after ":argdo", ":windo" or ":bufdo"
	// - another command follows
	// - inside a loop
	openscript(fname, global_busy || listcmd_busy || eap->nextcmd != ଖାଲି
#ifdef FEAT_EVAL
						 || eap->cstack->cs_idx >= 0
#endif
						 );

    // ":source" read ex commands
    ଅନ୍ୟଥା ଯଦି (do_source(fname, FALSE, DOSO_NONE, ଖାଲି) == FAIL)
	semsg(_(e_cant_open_file_str), fname);
}

/*
 * ":source {fname}"
 */
    ଶୂନ୍ୟ
ex_source(exarg_T *eap)
{
#ifdef FEAT_BROWSE
    ଯଦି (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	char_u *fname = ଖାଲି;

	fname = do_browse(0, (char_u *)_("Source Vim script"), eap->arg,
				      ଖାଲି, ଖାଲି,
				      (char_u *)_(BROWSE_FILTER_MACROS), ଖାଲି);
	ଯଦି (fname != ଖାଲି)
	{
	    cmd_source(fname, eap);
	    vim_free(fname);
	}
    }
    ଅନ୍ୟଥା
#endif
	cmd_source(eap->arg, eap);
}

#ଯଦି defined(FEAT_EVAL)
/*
 * ":options"
 */
    ଶୂନ୍ୟ
ex_options(
    exarg_T	*eap UNUSED)
{
    char_u  buf[500];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    multi_mods = 0;

    buf[0] = NUL;
    (ଶୂନ୍ୟ)add_win_cmd_modifiers(buf, &cmdmod, &multi_mods);

    vim_setenv((char_u *)"OPTWIN_CMD", buf);
    cmd_source((char_u *)SYS_OPTWIN_FILE, ଖାଲି);
}
#endif

/*
 * ":source" and associated commands.
 */

#ଯଦି defined(FEAT_EVAL)
/*
 * Return the address holding the next breakpoint line for a source cookie.
 */
    linenr_T *
source_breakpoint(ଶୂନ୍ୟ *cookie)
{
    ଫେରନ୍ତୁ &((source_cookie_T *)cookie)->breakpoint;
}

/*
 * Return the address holding the debug tick for a source cookie.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା *
source_dbg_tick(ଶୂନ୍ୟ *cookie)
{
    ଫେରନ୍ତୁ &((source_cookie_T *)cookie)->dbg_tick;
}

/*
 * Return the nesting level for a source cookie.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
source_level(ଶୂନ୍ୟ *cookie)
{
    ଫେରନ୍ତୁ ((source_cookie_T *)cookie)->level;
}

/*
 * Return the readahead line. Note that the pointer may become invalid when
 * getting the next line, if it's concatenated with the next one.
 */
    char_u *
source_nextline(ଶୂନ୍ୟ *cookie)
{
    ଫେରନ୍ତୁ ((source_cookie_T *)cookie)->nextline;
}
#endif

#ଯଦି (defined(MSWIN) && defined(FEAT_CSCOPE)) || defined(HAVE_FD_CLOEXEC)
# define USE_FOPEN_NOINH
/*
 * Special function to open a file without handle inheritance.
 * When possible the handle is closed on exec().
 */
    ସ୍ଥିର FILE *
fopen_noinh_readbin(ଅକ୍ଷର *filename)
{
# ifdef MSWIN
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	fd_tmp = mch_open(filename, O_RDONLY | O_ଦ୍ୱିମିକ | O_NOINHERIT, 0);
# ଅନ୍ୟଥା
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	fd_tmp = mch_open(filename, O_RDONLY, 0);
# endif

    ଯଦି (fd_tmp == -1)
	ଫେରନ୍ତୁ ଖାଲି;

# ifdef HAVE_FD_CLOEXEC
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdflags = fcntl(fd_tmp, F_GETFD);
	ଯଦି (fdflags >= 0 && (fdflags & FD_CLOEXEC) == 0)
	    (ଶୂନ୍ୟ)fcntl(fd_tmp, F_SETFD, fdflags | FD_CLOEXEC);
    }
# endif

    ଫେରନ୍ତୁ fdopen(fd_tmp, READBIN);
}
#endif

/*
 * Initialization for sourcing lines from the current buffer. Reads all the
 * lines from the buffer and stores it in the cookie grow array.
 * Returns a pointer to the name ":source buffer=<n>" on success and NULL on
 * failure.
 */
    ସ୍ଥିର char_u *
do_source_buffer_init(source_cookie_T *sp, exarg_T *eap)
{
    linenr_T	curr_lnum;
    char_u	*ଧାଡ଼ି = ଖାଲି;
    char_u	*fname;

    ଯଦି (curbuf == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    // Use ":source buffer=<num>" as the script name
    ଯଦି (curbuf->b_ffname != ଖାଲି)
	fname = vim_strsave(curbuf->b_ffname);
    ଅନ୍ୟଥା
    {
	vim_snprintf((ଅକ୍ଷର *)IObuff, IOSIZE, ":source buffer=%d", curbuf->b_fnum);
	fname = vim_strsave(IObuff);
    }
    ଯଦି (fname == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    ga_init2(&sp->buflines, ଆକାର(char_u *), 100);

    // Copy the lines from the buffer into a grow array
    ପାଇଁ (curr_lnum = eap->line1; curr_lnum <= eap->line2; curr_lnum++)
    {
	ଧାଡ଼ି = vim_strsave(ml_get(curr_lnum));
	ଯଦି (ଧାଡ଼ି == ଖାଲି)
	    ଯାଅ errret;
	ଯଦି (ga_add_string(&sp->buflines, ଧାଡ଼ି) == FAIL)
	    ଯାଅ errret;
    }
    sp->buf_lnum = 0;
    sp->source_from_buf = TRUE;
    // When sourcing a range of lines from a buffer, use buffer line number.
    sp->sourcing_lnum = eap->line1 - 1;

    ଫେରନ୍ତୁ fname;

errret:
    vim_free(fname);
    vim_free(ଧାଡ଼ି);
    ga_clear_strings(&sp->buflines);
    ଫେରନ୍ତୁ ଖାଲି;
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
do_source_ext(
    char_u	*fname,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		check_other,	    // check for .vimrc and _vimrc
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_vimrc,	    // DOSO_ value
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*ret_sid UNUSED,
    exarg_T	*eap,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		clearvars UNUSED)
{
    source_cookie_T	    cookie;
    char_u		    *p;
    char_u		    *fname_not_fixed = ଖାଲି;
    char_u		    *fname_exp = ଖାଲି;
    char_u		    *firstline = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			    retval = FAIL;
    sctx_T		    save_current_sctx;
#ifdef STARTUPTIME
    ସଂରଚନା timeval	    tv_rel;
    ସଂରଚନା timeval	    tv_start;
#endif
#ifdef FEAT_PROFILE
    proftime_T		    wait_start;
#endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			    save_sticky_cmdmod_flags = sticky_cmdmod_flags;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			    trigger_source_post = FALSE;
#ifdef FEAT_EVAL
    funccal_entry_T	    funccalp_entry;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			    save_debug_break_level = debug_break_level;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			    sid = -1;
    scriptitem_T	    *si = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			    save_estack_compiling = estack_compiling;
    ESTACK_CHECK_DECLARATION;
#endif

    CLEAR_FIELD(cookie);
    ଯଦି (fname == ଖାଲି)
    {
	// sourcing lines from a buffer
	fname_exp = do_source_buffer_init(&cookie, eap);
	ଯଦି (fname_exp == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;
    }
    ଅନ୍ୟଥା
    {
	fname_not_fixed = expand_env_save(fname);
	ଯଦି (fname_not_fixed == ଖାଲି)
	    ଯାଅ theend;
	fname_exp = fix_fname(fname_not_fixed);
	ଯଦି (fname_exp == ଖାଲି)
	    ଯାଅ theend;
	ଯଦି (mch_isdir(fname_exp))
	{
	    smsg(_("Cannot source a directory: \"%s\""), fname);
	    ଯାଅ theend;
	}
    }
#ifdef FEAT_EVAL
    estack_compiling = FALSE;

    // See if we loaded this script before.
    sid = find_script_by_name(fname_exp);
    ଯଦି (sid > 0 && ret_sid != ଖାଲି
			  && SCRIPT_ITEM(sid)->sn_state != SN_STATE_NOT_LOADED)
    {
	// Already loaded and no need to load again, return here.
	*ret_sid = sid;
	retval = OK;
	ଯାଅ theend;
    }
#endif

    // Apply SourceCmd autocommands, they should get the file and source it.
    // When sourcing a file "fname_exp" was made a full path name by
    // fix_fname() above, avoid expanding it again.
    ଯଦି ((fname == ଖାଲି
		? has_autocmd(EVENT_SOURCECMD, fname_exp, ଖାଲି)
		: has_autocmd_fullname(EVENT_SOURCECMD, fname_exp, ଖାଲି))
	    && apply_autocmds(EVENT_SOURCECMD, fname_exp, fname_exp,
							       FALSE, curbuf))
    {
#ifdef FEAT_EVAL
	retval = aborting() ? FAIL : OK;
#ଅନ୍ୟଥା
	retval = OK;
#endif
	ଯଦି (retval == OK)
	    // Apply SourcePost autocommands.
	    apply_autocmds(EVENT_SOURCEPOST, fname_exp, fname_exp,
								FALSE, curbuf);
	ଯାଅ theend;
    }

    // Apply SourcePre autocommands, they may get the file.
    apply_autocmds(EVENT_SOURCEPRE, fname_exp, fname_exp, FALSE, curbuf);

    ଯଦି (!cookie.source_from_buf)
    {
#ifdef USE_FOPEN_NOINH
	cookie.fp = fopen_noinh_readbin((ଅକ୍ଷର *)fname_exp);
#ଅନ୍ୟଥା
	cookie.fp = mch_fopen((ଅକ୍ଷର *)fname_exp, READBIN);
#endif
    }
    ଯଦି (cookie.fp == ଖାଲି && check_other)
    {
	// Try again, replacing file name ".vimrc" by "_vimrc" or vice versa,
	// and ".exrc" by "_exrc" or vice versa.
	p = gettail(fname_exp);
	ଯଦି ((*p == '.' || *p == '_')
		&& (STRICMP(p + 1, "vimrc") == 0
		    || STRICMP(p + 1, "gvimrc") == 0
		    || STRICMP(p + 1, "exrc") == 0))
	{
	    ଯଦି (*p == '_')
		*p = '.';
	    ଅନ୍ୟଥା
		*p = '_';
#ifdef USE_FOPEN_NOINH
	    cookie.fp = fopen_noinh_readbin((ଅକ୍ଷର *)fname_exp);
#ଅନ୍ୟଥା
	    cookie.fp = mch_fopen((ଅକ୍ଷର *)fname_exp, READBIN);
#endif
	}
    }

    ଯଦି (cookie.fp == ଖାଲି && !cookie.source_from_buf)
    {
	ଯଦି (p_verbose > 0)
	{
	    verbose_enter();
	    ଯଦି (SOURCING_NAME == ଖାଲି)
		smsg(_("could not source \"%s\""), fname);
	    ଅନ୍ୟଥା
		smsg(_("line %ld: could not source \"%s\""),
							SOURCING_LNUM, fname);
	    verbose_leave();
	}
	ଯାଅ theend;
    }

    // The file exists.
    // - In verbose mode, give a message.
    // - For a vimrc file, may want to set 'compatible', call vimrc_found().
    ଯଦି (p_verbose > 1)
    {
	verbose_enter();
	ଯଦି (SOURCING_NAME == ଖାଲି)
	    smsg(_("sourcing \"%s\""), fname);
	ଅନ୍ୟଥା
	    smsg(_("line %ld: sourcing \"%s\""), SOURCING_LNUM, fname);
	verbose_leave();
    }
    ଯଦି (is_vimrc == DOSO_VIMRC)
	vimrc_found(fname_exp, (char_u *)"MYVIMRC");
    ଅନ୍ୟଥା ଯଦି (is_vimrc == DOSO_GVIMRC)
	vimrc_found(fname_exp, (char_u *)"MYGVIMRC");

#ifdef USE_CRNL
    // If no automatic file format: Set default to CR-NL.
    ଯଦି (*p_ffs == NUL)
	cookie.fileformat = EOL_DOS;
    ଅନ୍ୟଥା
	cookie.fileformat = EOL_UNKNOWN;
#endif

#ifdef FEAT_EVAL
    // Check if this script has a breakpoint.
    cookie.breakpoint = dbg_find_breakpoint(TRUE, fname_exp, (linenr_T)0);
    cookie.fname = fname_exp;
    cookie.dbg_tick = debug_tick;

    cookie.level = ex_nesting_level;
#endif

#ifdef STARTUPTIME
    ଯଦି (time_fd != ଖାଲି)
	time_ଠେଲ(&tv_rel, &tv_start);
#endif

    // "legacy" does not apply to commands in the script
    sticky_cmdmod_flags = 0;

    save_current_sctx = current_sctx;
    ଯଦି ((cmdmod.cmod_flags & CMOD_VIM9CMD) && cookie.source_from_buf)
	// When the ":vim9cmd" command modifier is used, source buffer lines as
	// Vim9 script
	current_sctx.sc_version = SCRIPT_VERSION_VIM9;
    ଅନ୍ୟଥା
	current_sctx.sc_version = 1;  // default script version

#ifdef FEAT_EVAL
    current_sctx.sc_lnum = 0;

# ifdef FEAT_PROFILE
    ଯଦି (do_profiling == PROF_YES)
	prof_child_enter(&wait_start);		// entering a child now
# endif

    // Don't use local function variables, if called from a function.
    // Also starts profiling timer for nested script.
    save_funccal(&funccalp_entry);

    // Reset "KeyTyped" to avoid some commands thinking they are invoked
    // interactively.  E.g. defining a function would output indent.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_ଚାବି_Typed = ଚାବି_Typed;
    ଚାବି_Typed = FALSE;

    // Check if this script was sourced before to find its SID.
    // Always use a new sequence number.
    current_sctx.sc_seq = ++last_current_SID_seq;
    ଯଦି (sid > 0)
    {
	hashtab_T	*ht;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		todo;
	hashitem_T	*hi;
	dictitem_T	*di;

	// loading the same script again
	current_sctx.sc_sid = sid;
	si = SCRIPT_ITEM(sid);
	ଯଦି (si->sn_state == SN_STATE_NOT_LOADED)
	{
	    // this script was found but not loaded yet
	    si->sn_state = SN_STATE_NEW;
	}
	ଅନ୍ୟଥା
	{
	    si->sn_state = SN_STATE_RELOAD;

	    ଯଦି (!clearvars)
	    {
		// Script-local variables remain but "const" can be set again.
		// In Vim9 script variables will be cleared when "vim9script"
		// is encountered without the "noclear" argument.
		ht = &SCRIPT_VARS(sid);
		todo = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ht->ht_used;
		FOR_ALL_HASHTAB_ବସ୍ତୁଗୁଡ଼ିକ(ht, hi, todo)
		    ଯଦି (!HASHITEM_EMPTY(hi))
		    {
			--todo;
			di = HI2DI(hi);
			di->di_flags |= DI_FLAGS_RELOAD;
		    }
		// imports can be redefined once
		mark_imports_for_reload(sid);
	    }
	    ଅନ୍ୟଥା
		clear_vim9_scriptlocal_vars(sid);

	    // reset version, "vim9script" may have been added or removed.
	    si->sn_version = 1;
	}
	ଯଦି (ret_sid != ଖାଲି)
	    *ret_sid = sid;
    }
    ଅନ୍ୟଥା
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା error = OK;

	// It's new, generate a new SID and initialize the scriptitem.
	sid = get_new_scriptitem(&error);
	current_sctx.sc_sid = sid;
	ଯଦି (error == FAIL)
	    ଯାଅ almosttheend;
	si = SCRIPT_ITEM(sid);
	si->sn_name = fname_exp;
	fname_exp = vim_strsave(si->sn_name);  // used for autocmd
	ଯଦି (ret_sid != ଖାଲି)
	    *ret_sid = sid;

	// Remember the "is_vimrc" flag for when the file is sourced again.
	si->sn_is_vimrc = is_vimrc;
    }

    // Keep the sourcing name/lnum, for recursive calls.
    estack_ଠେଲ(ETYPE_SCRIPT, si->sn_name, 0);
    ESTACK_CHECK_SETUP;

# ifdef FEAT_PROFILE
    ଯଦି (do_profiling == PROF_YES)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	forceit;

	// Check if we do profiling for this script.
	ଯଦି (!si->sn_prof_on && has_profiling(TRUE, si->sn_name, &forceit, ଖାଲି))
	{
	    script_do_profile(si);
	    si->sn_pr_force = forceit;
	}
	ଯଦି (si->sn_prof_on)
	{
	    ++si->sn_pr_ଗଣନା;
	    profile_start(&si->sn_pr_start);
	    profile_zero(&si->sn_pr_children);
	}
    }
# endif
#ଅନ୍ୟଥା
    // Keep the sourcing name/lnum, for recursive calls.
    estack_ଠେଲ(ETYPE_SCRIPT, fname_exp, 0);
#endif

    cookie.conv.vc_type = CONV_NONE;		// no conversion

    // Read the first line so we can check for a UTF-8 BOM.
    firstline = getsourceline(0, (ଶୂନ୍ୟ *)&cookie, 0, TRUE);
    ଯଦି (firstline != ଖାଲି && STRLEN(firstline) >= 3 && firstline[0] == 0xef
			      && firstline[1] == 0xbb && firstline[2] == 0xbf)
    {
	// Found BOM; setup conversion, skip over BOM and recode the line.
	convert_setup(&cookie.conv, (char_u *)"utf-8", p_enc);
	p = string_convert(&cookie.conv, firstline + 3, ଖାଲି);
	ଯଦି (p == ଖାଲି)
	    p = vim_strsave(firstline + 3);
	ଯଦି (p != ଖାଲି)
	{
	    vim_free(firstline);
	    firstline = p;
	}
    }

    // Call do_cmdline, which will call getsourceline() to get the lines.
    do_cmdline(firstline, getsourceline, (ଶୂନ୍ୟ *)&cookie,
				     DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_REPEAT);
    retval = OK;

#ifdef FEAT_PROFILE
    ଯଦି (do_profiling == PROF_YES)
    {
	// Get "si" again, "script_items" may have been reallocated.
	si = SCRIPT_ITEM(sid);
	ଯଦି (si->sn_prof_on)
	{
	    profile_end(&si->sn_pr_start);
	    profile_sub_wait(&wait_start, &si->sn_pr_start);
	    profile_add(&si->sn_pr_total, &si->sn_pr_start);
	    profile_self(&si->sn_pr_self, &si->sn_pr_start,
							 &si->sn_pr_children);
	}
    }
#endif

    ଯଦି (got_int)
	emsg(_(e_interrupted));
#ifdef FEAT_EVAL
    ESTACK_CHECK_NOW;
#endif
    estack_କାଢ଼();
    ଯଦି (p_verbose > 1)
    {
	verbose_enter();
	smsg(_("finished sourcing %s"), fname);
	ଯଦି (SOURCING_NAME != ଖାଲି)
	    smsg(_("continuing in %s"), SOURCING_NAME);
	verbose_leave();
    }
#ifdef STARTUPTIME
    ଯଦି (time_fd != ଖାଲି)
    {
	vim_snprintf((ଅକ୍ଷର *)IObuff, IOSIZE, "sourcing %s", fname);
	time_ସନ୍ଦେଶ((ଅକ୍ଷର *)IObuff, &tv_start);
	time_କାଢ଼(&tv_rel);
    }
#endif

    ଯଦି (!got_int)
	trigger_source_post = TRUE;

#ifdef FEAT_EVAL
    // After a "finish" in debug mode, need to break at first command of next
    // sourced file.
    ଯଦି (save_debug_break_level > ex_nesting_level
	    && debug_break_level == ex_nesting_level)
	++debug_break_level;
#endif

#ifdef FEAT_EVAL
almosttheend:
    // If "sn_save_cpo" is set that means we encountered "vim9script": restore
    // 'cpoptions', unless in the main .vimrc file.
    // Get "si" again, "script_items" may have been reallocated.
    si = SCRIPT_ITEM(sid);
    ଯଦି (si->sn_save_cpo != ଖାଲି && si->sn_is_vimrc == DOSO_NONE)
    {
	ଯଦି (STRCMP(p_cpo, CPO_VIM) != 0)
	{
	    char_u *f;
	    char_u *t;

	    // 'cpo' was changed in the script.  Apply the same change to the
	    // saved value, if possible.
	    ପାଇଁ (f = (char_u *)CPO_VIM; *f != NUL; ++f)
		ଯଦି (vim_strchr(p_cpo, *f) == ଖାଲି
			&& (t = vim_strchr(si->sn_save_cpo, *f)) != ଖାଲି)
		    // flag was removed, also remove it from the saved 'cpo'
		    mch_memmove(t, t + 1, STRLEN(t));
	    ପାଇଁ (f = p_cpo; *f != NUL; ++f)
		ଯଦି (vim_strchr((char_u *)CPO_VIM, *f) == ଖାଲି
			&& vim_strchr(si->sn_save_cpo, *f) == ଖାଲି)
		{
		    // flag was added, also add it to the saved 'cpo'
		    t = alloc(STRLEN(si->sn_save_cpo) + 2);
		    ଯଦି (t != ଖାଲି)
		    {
			*t = *f;
			STRCPY(t + 1, si->sn_save_cpo);
			vim_free(si->sn_save_cpo);
			si->sn_save_cpo = t;
		    }
		}
	}
	set_option_ମୂଲ୍ୟ_give_err((char_u *)"cpo",
					   0L, si->sn_save_cpo, OPT_NO_REDRAW);
    }
    VIM_CLEAR(si->sn_save_cpo);

    restore_funccal();
# ifdef FEAT_PROFILE
    ଯଦି (do_profiling == PROF_YES)
	prof_child_exit(&wait_start);		// leaving a child now
# endif

    ଚାବି_Typed = save_ଚାବି_Typed;
#endif
    current_sctx = save_current_sctx;

    ଯଦି (cookie.fp != ଖାଲି)
	fclose(cookie.fp);
    ଯଦି (cookie.source_from_buf)
	ga_clear_strings(&cookie.buflines);
    vim_free(cookie.nextline);
    vim_free(firstline);
    convert_setup(&cookie.conv, ଖାଲି, ଖାଲି);

    ଯଦି (trigger_source_post)
	apply_autocmds(EVENT_SOURCEPOST, fname_exp, fname_exp, FALSE, curbuf);

theend:
#ifdef FEAT_EVAL
    ଯଦି (sid > 0 && ret_sid != ଖାଲି
	    && fname_not_fixed != ଖାଲି && fname_exp != ଖାଲି)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା not_fixed_sid = find_script_by_name(fname_not_fixed);

	// If "fname_not_fixed" is a symlink then we source the linked file.
	// If the original name is in the script list we add the ID of the
	// script that was actually sourced.
	ଯଦି (SCRIPT_ID_VALID(not_fixed_sid) && not_fixed_sid != sid)
	    SCRIPT_ITEM(not_fixed_sid)->sn_sourced_sid = sid;
    }
#endif

    vim_free(fname_not_fixed);
    vim_free(fname_exp);
    sticky_cmdmod_flags = save_sticky_cmdmod_flags;
#ifdef FEAT_EVAL
    estack_compiling = save_estack_compiling;
#endif
    ଫେରନ୍ତୁ retval;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
do_source(
    char_u	*fname,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		check_other,	    // check for .vimrc and _vimrc
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_vimrc,	    // DOSO_ value
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*ret_sid)
{
    ଫେରନ୍ତୁ do_source_ext(fname, check_other, is_vimrc, ret_sid, ଖାଲି, FALSE);
}


#ଯଦି defined(FEAT_EVAL)

/*
 * ":scriptnames"
 */
    ଶୂନ୍ୟ
ex_scriptnames(exarg_T *eap)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

    ଯଦି (eap->addr_ଗଣନା > 0 || *eap->arg != NUL)
    {
	// :script {scriptId}: edit the script
	ଯଦି (eap->addr_ଗଣନା > 0 && !SCRIPT_ID_VALID(eap->line2))
	    emsg(_(e_invalid_argument));
	ଅନ୍ୟଥା
	{
	    ଯଦି (eap->addr_ଗଣନା > 0)
		eap->arg = SCRIPT_ITEM(eap->line2)->sn_name;
	    ଅନ୍ୟଥା
	    {
		expand_env(eap->arg, NameBuff, MAXPATHL);
		eap->arg = NameBuff;
	    }
	    do_exedit(eap, ଖାଲି);
	}
	ଫେରନ୍ତୁ;
    }

    ପାଇଁ (i = 1; i <= script_ବସ୍ତୁଗୁଡ଼ିକ.ga_len && !got_int; ++i)
    {
	scriptitem_T *si = SCRIPT_ITEM(i);

	ଯଦି (si->sn_name != ଖାଲି)
	{
	    ଅକ୍ଷର sourced_buf[20];

	    home_replace(ଖାଲି, si->sn_name, NameBuff, MAXPATHL, TRUE);
	    ଯଦି (si->sn_sourced_sid > 0)
		vim_snprintf(sourced_buf, 20, "->%d", si->sn_sourced_sid);
	    ଅନ୍ୟଥା
		sourced_buf[0] = NUL;
	    vim_snprintf((ଅକ୍ଷର *)IObuff, IOSIZE, "%3d%s%s: %s",
		    i,
		    sourced_buf,
		    si->sn_state == SN_STATE_NOT_LOADED ? " A" : "",
		    NameBuff);
	    ଯଦି (!message_filtered(IObuff))
	    {
		ସନ୍ଦେଶ_putchar('\n');
		ସନ୍ଦେଶ_outtrans(IObuff);
		out_flush();	    // output one line at a time
		ui_breakcheck();
	    }
	}
    }
}

# ଯଦି defined(BACKSLASH_IN_FILENAME)
/*
 * Fix slashes in the list of script names for 'shellslash'.
 */
    ଶୂନ୍ୟ
scriptnames_slash_adjust(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

    ପାଇଁ (i = 1; i <= script_ବସ୍ତୁଗୁଡ଼ିକ.ga_len; ++i)
	ଯଦି (SCRIPT_ITEM(i)->sn_name != ଖାଲି)
	    slash_adjust(SCRIPT_ITEM(i)->sn_name);
}
# endif

/*
 * Get a pointer to a script name.  Used for ":verbose set".
 * Message appended to "Last set from "
 */
    char_u *
get_scriptname(scid_T id)
{
    ଯଦି (id == SID_MODELINE)
	ଫେରନ୍ତୁ (char_u *)_("modeline");
    ଯଦି (id == SID_CMDARG)
	ଫେରନ୍ତୁ (char_u *)_("--cmd argument");
    ଯଦି (id == SID_CARG)
	ଫେରନ୍ତୁ (char_u *)_("-c argument");
    ଯଦି (id == SID_ENV)
	ଫେରନ୍ତୁ (char_u *)_("environment variable");
    ଯଦି (id == SID_ERROR)
	ଫେରନ୍ତୁ (char_u *)_("error handler");
    ଯଦି (id == SID_WINLAYOUT)
	ଫେରନ୍ତୁ (char_u *)_("changed window size");
    ଫେରନ୍ତୁ SCRIPT_ITEM(id)->sn_name;
}

# ଯଦି defined(EXITFREE)
    ଶୂନ୍ୟ
free_scriptnames(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			i;

    ପାଇଁ (i = script_ବସ୍ତୁଗୁଡ଼ିକ.ga_len; i > 0; --i)
    {
	scriptitem_T *si = SCRIPT_ITEM(i);

	// the variables themselves are cleared in evalvars_clear()
	vim_free(si->sn_vars);

	vim_free(si->sn_name);
	free_imports_and_script_vars(i);
	free_string_option(si->sn_save_cpo);
#  ifdef FEAT_PROFILE
	ga_clear(&si->sn_prl_ga);
#  endif
	vim_free(si->sn_autoload_prefix);
	vim_free(si);
    }
    ga_clear(&script_ବସ୍ତୁଗୁଡ଼ିକ);
}

    ଶୂନ୍ୟ
free_autoload_scriptnames(ଶୂନ୍ୟ)
{
    ga_clear_strings(&ga_loaded);
}
# endif

    linenr_T
get_sourced_lnum(
	char_u *(*fgetline)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ଶୂନ୍ୟ *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, getline_opt_T),
	ଶୂନ୍ୟ *cookie)
{
    ଫେରନ୍ତୁ fgetline == getsourceline
			? ((source_cookie_T *)cookie)->sourcing_lnum
			: SOURCING_LNUM;
}

/*
 * Return a List of script-local functions defined in the script with id
 * 'sid'.
 */
    ସ୍ଥିର list_T *
get_script_local_funcs(scid_T sid)
{
    hashtab_T	*functbl;
    hashitem_T	*hi;
    long_u	todo;
    list_T	*l;

    l = list_alloc();
    ଯଦି (l == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    // Iterate through all the functions in the global function hash table
    // looking for functions with script ID 'sid'.
    functbl = func_tbl_get();
    todo = functbl->ht_used;
    FOR_ALL_HASHTAB_ବସ୍ତୁଗୁଡ଼ିକ(functbl, hi, todo)
    {
	ufunc_T	*fp;

	ଯଦି (HASHITEM_EMPTY(hi))
	    ଜାରି;

	--todo;
	fp = HI2UF(hi);

	// Add active functions with script id == 'sid'
	ଯଦି (!(fp->uf_flags & FC_DEAD) && (fp->uf_script_ctx.sc_sid == sid))
	{
	    char_u	*name;

	    ଯଦି (fp->uf_name_exp != ଖାଲି)
		name = fp->uf_name_exp;
	    ଅନ୍ୟଥା
		name = fp->uf_name;

	    list_append_string(l, name, -1);
	}
    }

    ଫେରନ୍ତୁ l;
}

/*
 * getscriptinfo() function
 */
    ଶୂନ୍ୟ
f_getscriptinfo(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    char_u	*pat = ଖାଲି;
    regmatch_T	regmatch;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		filterpat = FALSE;
    scid_T	sid = -1;

    ଯଦି (rettv_list_alloc(rettv) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (check_for_opt_dict_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    l = rettv->vval.v_list;

    regmatch.regprog = ଖାଲି;
    regmatch.rm_ic = p_ic;

    ଯଦି (argvars[0].v_type == VAR_DICT)
    {
	dictitem_T *sid_di = dict_find(argvars[0].vval.v_dict,
							   (char_u *)"sid", 3);
	ଯଦି (sid_di != ଖାଲି)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା error = FALSE;
	    sid = tv_get_ସଂଖ୍ୟା_chk(&sid_di->di_tv, &error);
	    ଯଦି (error)
		ଫେରନ୍ତୁ;
	    ଯଦି (sid <= 0)
	    {
		semsg(_(e_invalid_ମୂଲ୍ୟ_for_argument_str_str), "sid",
						tv_get_string(&sid_di->di_tv));
		ଫେରନ୍ତୁ;
	    }
	}
	ଅନ୍ୟଥା
	{
	    pat = dict_get_string(argvars[0].vval.v_dict, "name", TRUE);
	    ଯଦି (pat != ଖାଲି)
		regmatch.regprog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
	    ଯଦି (regmatch.regprog != ଖାଲି)
		filterpat = TRUE;
	}
    }

    ପାଇଁ (varnumber_T i = sid > 0 ? sid : 1;
		       (i == sid || sid <= 0) && i <= script_ବସ୍ତୁଗୁଡ଼ିକ.ga_len; ++i)
    {
	scriptitem_T	*si = SCRIPT_ITEM(i);
	dict_T		*d;

	ଯଦି (si->sn_name == ଖାଲି)
	    ଜାରି;

	ଯଦି (filterpat && !vim_regexec(&regmatch, si->sn_name, (colnr_T)0))
	    ଜାରି;

	ଯଦି ((d = dict_alloc()) == ଖାଲି
		|| list_append_dict(l, d) == FAIL)
	{
	    dict_unref(d);
	    ଯାଅ theend;
	}
	ଯଦି (dict_add_string(d, "name", si->sn_name) == FAIL
		|| dict_add_ସଂଖ୍ୟା(d, "sid", i) == FAIL
		|| dict_add_ସଂଖ୍ୟା(d, "sourced", si->sn_sourced_sid) == FAIL
		|| dict_add_ସଂଖ୍ୟା(d, "version", si->sn_version) == FAIL
		|| dict_add_bool(d, "autoload",
				si->sn_state == SN_STATE_NOT_LOADED) == FAIL)
	    ଯାଅ theend;

	// When a script ID is specified, return information about only the
	// specified script, and add the script-local variables and functions.
	ଯଦି (sid > 0)
	{
	    dict_T	*var_dict;
	    list_T	*fn_list;

	    var_dict = dict_copy(&si->sn_vars->sv_dict, TRUE, TRUE,
								get_copyID());
	    ଯଦି (var_dict == ଖାଲି)
		ଯାଅ theend;
	    ଯଦି (dict_add_dict(d, "variables", var_dict) == FAIL)
	    {
		dict_unref(var_dict);
		ଯାଅ theend;
	    }
	    --var_dict->dv_refcount;
	    fn_list = get_script_local_funcs(sid);
	    ଯଦି (fn_list == ଖାଲି || dict_add_list(d, "functions", fn_list) == FAIL)
	    {
		list_unref(fn_list);
		ଯାଅ theend;
	    }
	}
    }

theend:
    vim_regfree(regmatch.regprog);
    vim_free(pat);
}

#endif

    ସ୍ଥିର char_u *
get_one_sourceline(source_cookie_T *sp)
{
    garray_T		ga;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			c;
    char_u		*buf;
#ifdef USE_CRNL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			has_cr;		// CR-LF found
#endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			have_read = FALSE;

    // use a growarray to store the sourced line
    ga_init2(&ga, 1, 250);

    // Loop until there is a finished line (or end-of-file).
    ++sp->sourcing_lnum;
    ପାଇଁ (;;)
    {
	// make room to read at least 120 (more) characters
	ଯଦି (ga_grow(&ga, 120) == FAIL)
	    ଭାଙ୍ଗ;
	ଯଦି (sp->source_from_buf)
	{
	    ଯଦି (sp->buf_lnum >= sp->buflines.ga_len)
		ଭାଙ୍ଗ;		    // all the lines are processed
	    ga_concat(&ga, ((char_u **)sp->buflines.ga_data)[sp->buf_lnum]);
	    sp->buf_lnum++;
	    ଯଦି (ga_grow(&ga, 1) == FAIL)
		ଭାଙ୍ଗ;
	    buf = (char_u *)ga.ga_data;
	    buf[ga.ga_len++] = NUL;
	    len = ga.ga_len;
	}
	ଅନ୍ୟଥା
	{
	    buf = (char_u *)ga.ga_data;
	    ଯଦି (fgets((ଅକ୍ଷର *)buf + ga.ga_len, ga.ga_maxlen - ga.ga_len,
			sp->fp) == ଖାଲି)
		ଭାଙ୍ଗ;
	    len = ga.ga_len + (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(buf + ga.ga_len);
	}
#ifdef USE_CRNL
	// Ignore a trailing CTRL-Z, when in Dos mode.	Only recognize the
	// CTRL-Z by its own, or after a NL.
	ଯଦି (	   (len == 1 || (len >= 2 && buf[len - 2] == '\n'))
		&& sp->fileformat == EOL_DOS
		&& buf[len - 1] == Ctrl_Z)
	{
	    buf[len - 1] = NUL;
	    ଭାଙ୍ଗ;
	}
#endif

	have_read = TRUE;
	ga.ga_len = len;

	// If the line was longer than the buffer, read more.  A line of a
	// buffer was taken as a whole.
	ଯଦି (!sp->source_from_buf && ga.ga_maxlen - ga.ga_len == 1
						       && buf[len - 1] != '\n')
	    ଜାରି;

	ଯଦି (len >= 1 && buf[len - 1] == '\n')	// remove trailing NL
	{
#ifdef USE_CRNL
	    has_cr = (len >= 2 && buf[len - 2] == '\r');
	    ଯଦି (sp->fileformat == EOL_UNKNOWN)
	    {
		ଯଦି (has_cr)
		    sp->fileformat = EOL_DOS;
		ଅନ୍ୟଥା
		    sp->fileformat = EOL_UNIX;
	    }

	    ଯଦି (sp->fileformat == EOL_DOS)
	    {
		ଯଦି (has_cr)	    // replace trailing CR
		{
		    buf[len - 2] = '\n';
		    --len;
		    --ga.ga_len;
		}
		ଅନ୍ୟଥା	    // lines like ":map xx yy^M" will have failed
		{
		    ଯଦି (!sp->error)
		    {
			ସନ୍ଦେଶ_source(HL_ATTR(HLF_W));
			emsg(_("W15: Warning: Wrong line separator, ^M may be missing"));
		    }
		    sp->error = TRUE;
		    sp->fileformat = EOL_UNIX;
		}
	    }
#endif
	    // The '\n' is escaped if there is an odd number of ^V's just
	    // before it, first set "c" just before the 'V's and then check
	    // len&c parities (is faster than ((len-c)%2 == 0)) -- Acevedo
	    ପାଇଁ (c = len - 2; c >= 0 && buf[c] == Ctrl_V; c--)
		;
	    ଯଦି ((len & 1) != (c & 1))	// escaped NL, read more
	    {
		++sp->sourcing_lnum;
		ଜାରି;
	    }

	    buf[len - 1] = NUL;		// remove the NL
	}

	// Check for ^C here now and then, so recursive :so can be broken.
	ଧାଡ଼ି_breakcheck();
	ଭାଙ୍ଗ;
    }

    ଯଦି (have_read)
	ଫେରନ୍ତୁ (char_u *)ga.ga_data;

    vim_free(ga.ga_data);
    ଫେରନ୍ତୁ ଖାଲି;
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
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା c UNUSED,
	ଶୂନ୍ୟ *cookie,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା indent UNUSED,
	getline_opt_T options)
{
    source_cookie_T	*sp = (source_cookie_T *)cookie;
    char_u		*ଧାଡ଼ି;
    char_u		*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			do_vim9_all = in_vim9script()
					      && options == GETLINE_CONCAT_ALL;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			do_bar_cont = do_vim9_all
					 || options == GETLINE_CONCAT_CONTBAR;

#ifdef FEAT_EVAL
    // If breakpoints have been added/deleted need to check for it.
    ଯଦି ((sp->dbg_tick < debug_tick) && !sp->source_from_buf)
    {
	sp->breakpoint = dbg_find_breakpoint(TRUE, sp->fname, SOURCING_LNUM);
	sp->dbg_tick = debug_tick;
    }
# ifdef FEAT_PROFILE
    ଯଦି (do_profiling == PROF_YES)
	script_ଧାଡ଼ି_end();
# endif
#endif

    // Set the current sourcing line number.
    SOURCING_LNUM = sp->sourcing_lnum + 1;

    // Get current line.  If there is a read-ahead line, use it, otherwise get
    // one now.  "fp" is NULL if actually using a string.
    ଯଦି (sp->finished || (!sp->source_from_buf && sp->fp == ଖାଲି))
	ଧାଡ଼ି = ଖାଲି;
    ଅନ୍ୟଥା ଯଦି (sp->nextline == ଖାଲି)
	ଧାଡ଼ି = get_one_sourceline(sp);
    ଅନ୍ୟଥା
    {
	ଧାଡ଼ି = sp->nextline;
	sp->nextline = ଖାଲି;
	++sp->sourcing_lnum;
    }
#ifdef FEAT_PROFILE
    ଯଦି (ଧାଡ଼ି != ଖାଲି && do_profiling == PROF_YES)
	script_ଧାଡ଼ି_start();
#endif

    // Only concatenate lines starting with a \ when 'cpoptions' doesn't
    // contain the 'C' flag.
    ଯଦି (ଧାଡ଼ି != ଖାଲି && options != GETLINE_NONE
				      && vim_strchr(p_cpo, CPO_CONCAT) == ଖାଲି)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା comment_char = in_vim9script() ? '#' : '"';

	// compensate for the one line read-ahead
	--sp->sourcing_lnum;

	// Get the next line and concatenate it when it starts with a
	// backslash. We always need to read the next line, keep it in
	// sp->nextline.
	/* Also check for a comment in between continuation lines: "\ */
	// Also check for a Vim9 comment, empty line, line starting with '|',
	// but not "||".
	sp->nextline = get_one_sourceline(sp);
	ଯଦି (sp->nextline != ଖାଲି
		&& (*(p = skipwhite(sp->nextline)) == '\\'
			      || (p[0] == comment_char
						&& p[1] == '\\' && p[2] == ' ')
			      || (do_vim9_all && (*p == NUL
						     || vim9_comment_start(p)))
			      || (do_bar_cont && p[0] == '|' && p[1] != '|')))
	{
	    garray_T    ga;

	    ga_init2(&ga, ଆକାର(char_u), 400);
	    ga_concat(&ga, ଧାଡ଼ି);
	    ଯଦି (*p == '\\')
		ga_concat(&ga, p + 1);
	    ଅନ୍ୟଥା ଯଦି (*p == '|')
	    {
		GA_CONCAT_LITERAL(&ga, " ");
		ga_concat(&ga, p);
	    }
	    ପାଇଁ (;;)
	    {
		vim_free(sp->nextline);
		sp->nextline = get_one_sourceline(sp);
		ଯଦି (sp->nextline == ଖାଲି)
		    ଭାଙ୍ଗ;
		p = skipwhite(sp->nextline);
		ଯଦି (*p == '\\' || (do_bar_cont && p[0] == '|' && p[1] != '|'))
		{
		    // Adjust the growsize to the current length to speed up
		    // concatenating many lines.
		    ଯଦି (ga.ga_len > 400)
		    {
			ଯଦି (ga.ga_len > 8000)
			    ga.ga_growsize = 8000;
			ଅନ୍ୟଥା
			    ga.ga_growsize = ga.ga_len;
		    }
		    ଯଦି (*p == '\\')
			ga_concat(&ga, p + 1);
		    ଅନ୍ୟଥା
		    {
			GA_CONCAT_LITERAL(&ga, " ");
			ga_concat(&ga, p);
		    }
		}
		ଅନ୍ୟଥା ଯଦି (!(p[0] == (comment_char)
						&& p[1] == '\\' && p[2] == ' ')
		     && !(do_vim9_all && (*p == NUL || vim9_comment_start(p))))
		    ଭାଙ୍ଗ;
		/* drop a # comment or "\ comment line */
	    }
	    ga_append(&ga, NUL);
	    vim_free(ଧାଡ଼ି);
	    ଧାଡ଼ି = ga.ga_data;
	}
    }

    ଯଦି (ଧାଡ଼ି != ଖାଲି && sp->conv.vc_type != CONV_NONE)
    {
	char_u	*s;

	// Convert the encoding of the script line.
	s = string_convert(&sp->conv, ଧାଡ଼ି, ଖାଲି);
	ଯଦି (s != ଖାଲି)
	{
	    vim_free(ଧାଡ଼ି);
	    ଧାଡ଼ି = s;
	}
    }

#ifdef FEAT_EVAL
    // Did we encounter a breakpoint?
    ଯଦି (!sp->source_from_buf && sp->breakpoint != 0
	    && sp->breakpoint <= SOURCING_LNUM)
    {
	dbg_breakpoint(sp->fname, SOURCING_LNUM);
	// Find next breakpoint.
	sp->breakpoint = dbg_find_breakpoint(TRUE, sp->fname, SOURCING_LNUM);
	sp->dbg_tick = debug_tick;
    }
#endif

    ଫେରନ୍ତୁ ଧାଡ଼ି;
}

/*
 * Returns TRUE if sourcing a script either from a file or a buffer.
 * Otherwise returns FALSE.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
sourcing_a_script(exarg_T *eap)
{
    ଫେରନ୍ତୁ (getline_equal(eap->ea_getline, eap->cookie, getsourceline));
}

/*
 * ":scriptencoding": Set encoding conversion for a sourced script.
 */
    ଶୂନ୍ୟ
ex_scriptencoding(exarg_T *eap)
{
    source_cookie_T	*sp;
    char_u		*name;

    ଯଦି (!sourcing_a_script(eap))
    {
	emsg(_(e_scriptencoding_used_outside_of_sourced_file));
	ଫେରନ୍ତୁ;
    }

    ଯଦି (*eap->arg != NUL)
    {
	name = enc_canonize(eap->arg);
	ଯଦି (name == ଖାଲି)	// out of memory
	    ଫେରନ୍ତୁ;
    }
    ଅନ୍ୟଥା
	name = eap->arg;

    // Setup for conversion from the specified encoding to 'encoding'.
    sp = (source_cookie_T *)getline_cookie(eap->ea_getline, eap->cookie);
    convert_setup(&sp->conv, name, p_enc);

    ଯଦି (name != eap->arg)
	vim_free(name);
}

/*
 * ":scriptversion": Set Vim script version for a sourced script.
 */
    ଶୂନ୍ୟ
ex_scriptversion(exarg_T *eap UNUSED)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nr;

    ଯଦି (!sourcing_a_script(eap))
    {
	emsg(_(e_scriptversion_used_outside_of_sourced_file));
	ଫେରନ୍ତୁ;
    }
    ଯଦି (in_vim9script())
    {
	emsg(_(e_cannot_use_scriptversion_after_vim9script));
	ଫେରନ୍ତୁ;
    }

    nr = getdigits(&eap->arg);
    ଯଦି (nr == 0 || *eap->arg != NUL)
	emsg(_(e_invalid_argument));
    ଅନ୍ୟଥା ଯଦି (nr > SCRIPT_VERSION_MAX)
	semsg(_(e_scriptversion_not_supported_nr), nr);
    ଅନ୍ୟଥା
    {
	current_sctx.sc_version = nr;
#ifdef FEAT_EVAL
	SCRIPT_ITEM(current_sctx.sc_sid)->sn_version = nr;
#endif
    }
}

#ଯଦି defined(FEAT_EVAL)
/*
 * ":finish": Mark a sourced file as finished.
 */
    ଶୂନ୍ୟ
ex_finish(exarg_T *eap)
{
    ଯଦି (sourcing_a_script(eap))
	do_finish(eap, FALSE);
    ଅନ୍ୟଥା
	emsg(_(e_finish_used_outside_of_sourced_file));
}

/*
 * Mark a sourced file as finished.  Possibly makes the ":finish" pending.
 * Also called for a pending finish at the ":endtry" or after returning from
 * an extra do_cmdline().  "reanimate" is used in the latter case.
 */
    ଶୂନ୍ୟ
do_finish(exarg_T *eap, ପୂର୍ଣ୍ଣସଂଖ୍ୟା reanimate)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx;

    ଯଦି (reanimate)
	((source_cookie_T *)getline_cookie(eap->ea_getline,
					      eap->cookie))->finished = FALSE;

    // Cleanup (and inactivate) conditionals, but stop when a try conditional
    // not in its finally clause (which then is to be executed next) is found.
    // In this case, make the ":finish" pending for execution at the ":endtry".
    // Otherwise, finish normally.
    idx = cleanup_conditionals(eap->cstack, 0, TRUE);
    ଯଦି (idx >= 0)
    {
	eap->cstack->cs_pending[idx] = CSTP_FINISH;
	report_make_pending(CSTP_FINISH, ଖାଲି);
    }
    ଅନ୍ୟଥା
	((source_cookie_T *)getline_cookie(eap->ea_getline,
					       eap->cookie))->finished = TRUE;
}


/*
 * Return TRUE when a sourced file had the ":finish" command: Don't give error
 * message for missing ":endif".
 * Return FALSE when not sourcing a file.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
source_finished(
    char_u	*(*fgetline)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ଶୂନ୍ୟ *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, getline_opt_T),
    ଶୂନ୍ୟ	*cookie)
{
    ଫେରନ୍ତୁ (getline_equal(fgetline, cookie, getsourceline)
	    && ((source_cookie_T *)getline_cookie(
						fgetline, cookie))->finished);
}

/*
 * Find the path of a script below the "autoload" directory.
 * Returns NULL if there is no "/autoload/" in the script name.
 */
    ସ୍ଥିର char_u *
script_name_after_autoload(scriptitem_T *si)
{
    char_u	*p = si->sn_name;
    char_u	*res = ଖାଲି;

    ପାଇଁ (;;)
    {
	char_u *n = (char_u *)strstr((ଅକ୍ଷର *)p, "autoload");

	ଯଦି (n == ଖାଲି)
	    ଭାଙ୍ଗ;
	ଯଦି (n > p && vim_ispathsep(n[-1]) && vim_ispathsep(n[8]))
	    res = n + 9;
	p = n + 8;
    }
    ଫେରନ୍ତୁ res;
}

/*
 * For an autoload script "autoload/dir/script.vim" return the prefix
 * "dir#script#" in allocated memory.
 * Returns NULL if anything is wrong.
 */
    char_u *
get_autoload_prefix(scriptitem_T *si)
{
    char_u *p = script_name_after_autoload(si);
    char_u *prefix;

    ଯଦି (p == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    prefix = vim_strsave(p);
    ଯଦି (prefix == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    // replace all '/' with '#' and locate ".vim" at the end
    ପାଇଁ (p = prefix; *p != NUL; p += mb_ptr2len(p))
    {
	ଯଦି (vim_ispathsep(*p))
	    *p = '#';
	ଅନ୍ୟଥା ଯଦି (STRCMP(p, ".vim") == 0)
	{
	    p[0] = '#';
	    p[1] = NUL;
	    ଫେରନ୍ତୁ prefix;
	}
    }

    // did not find ".vim" at the end
    vim_free(prefix);
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * If in a Vim9 autoload script return "name" with the autoload prefix for the
 * script.  If successful the returned name is allocated.
 * Otherwise it returns "name" unmodified.
 */
    char_u *
may_prefix_autoload(char_u *name)
{
    ଯଦି (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	ଫେରନ୍ତୁ name;

    scriptitem_T *si = SCRIPT_ITEM(current_sctx.sc_sid);

    ଯଦି (si->sn_autoload_prefix == ଖାଲି)
	ଫେରନ୍ତୁ name;

    char_u  *basename = name;
    size_t  len;
    char_u  *res;

    ଯଦି (*name == K_SPECIAL)
    {
	char_u *p = vim_strchr(name, '_');

	// skip over "<SNR>99_"
	ଯଦି (p != ଖାଲି)
	    basename = p + 1;
    }

    len = STRLEN(si->sn_autoload_prefix) + STRLEN(basename) + 2;
    res = alloc(len);
    ଯଦି (res == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    vim_snprintf((ଅକ୍ଷର *)res, len, "%s%s", si->sn_autoload_prefix, basename);
    ଫେରନ୍ତୁ res;
}

/*
 * Return the autoload script name for a function or variable name.
 * Returns NULL when out of memory.
 * Caller must make sure that "name" contains AUTOLOAD_CHAR.
 */
    char_u *
autoload_name(char_u *name)
{
    char_u	*p, *q = ଖାଲି;
    char_u	*scriptname;

    // Get the script file name: replace '#' with '/', append ".vim".
    scriptname = alloc(STRLEN(name) + 14);
    ଯଦି (scriptname == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    STRCPY(scriptname, "autoload/");
    STRCAT(scriptname, name[0] == 'g' && name[1] == ':' ? name + 2: name);
    ପାଇଁ (p = scriptname + 9; (p = vim_strchr(p, AUTOLOAD_CHAR)) != ଖାଲି;
								    q = p, ++p)
	*p = '/';
    STRCPY(q, ".vim");
    ଫେରନ୍ତୁ scriptname;
}

/*
 * If "name" has a package name try autoloading the script for it.
 * Return TRUE if a package was loaded.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
script_autoload(
    char_u	*name,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		reload)	    // load script again when already loaded
{
    char_u	*p;
    char_u	*scriptname, *tofree;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret_sid;

    // If the name starts with "<SNR>123_" then "123" is the script ID.
    ଯଦି (name[0] == K_SPECIAL && name[1] == KS_EXTRA && name[2] == KE_SNR)
    {
	p = name + 3;
	ret_sid = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)getdigits(&p);
	ଯଦି (*p == '_' && SCRIPT_ID_VALID(ret_sid))
	{
	    may_load_script(ret_sid, &ret);
	    ଫେରନ୍ତୁ ret;
	}
    }

    // If there is no '#' after name[0] there is no package name.
    p = vim_strchr(name, AUTOLOAD_CHAR);
    ଯଦି (p == ଖାଲି || p == name)
	ଫେରନ୍ତୁ FALSE;

    tofree = scriptname = autoload_name(name);
    ଯଦି (scriptname == ଖାଲି)
	ଫେରନ୍ତୁ FALSE;

    // Find the name in the list of previously loaded package names.  Skip
    // "autoload/", it's always the same.
    ପାଇଁ (i = 0; i < ga_loaded.ga_len; ++i)
	ଯଦି (STRCMP(((char_u **)ga_loaded.ga_data)[i] + 9, scriptname + 9) == 0)
	    ଭାଙ୍ଗ;
    ଯଦି (!reload && i < ga_loaded.ga_len)
	ret = FALSE;	    // was loaded already
    ଅନ୍ୟଥା
    {
	// Remember the name if it wasn't loaded already.
	ଯଦି (i == ga_loaded.ga_len && ga_grow(&ga_loaded, 1) == OK)
	{
	    ((char_u **)ga_loaded.ga_data)[ga_loaded.ga_len++] = scriptname;
	    tofree = ଖାଲି;
	}

	// Try loading the package from $VIMRUNTIME/autoload/<name>.vim
	// Use "ret_sid" to avoid loading the same script again.
	ଯଦି (source_in_path(p_rtp, scriptname, DIP_START, &ret_sid) == OK)
	    ret = TRUE;
    }

    vim_free(tofree);
    ଫେରନ୍ତୁ ret;
}
#endif
