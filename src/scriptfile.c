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

#অন্তর্ভাব "vim.h"

#যদি defined(FEAT_EVAL)
// The names of packages that once were loaded are remembered.
স্থির garray_T		ga_loaded = {0, 0, পরিমাণ(char_u *), 4, রিক্ত};
#endif

// last used sequence number for sourcing scripts (current_sctx.sc_seq)
#ifdef FEAT_EVAL
স্থির পূর্ণাঙ্ক		last_current_SID_seq = 0;
#endif

স্থির পূর্ণাঙ্ক do_source_ext(char_u *fname, পূর্ণাঙ্ক check_other, পূর্ণাঙ্ক is_vimrc, পূর্ণাঙ্ক *ret_sid, exarg_T *eap, পূর্ণাঙ্ক clearvars);

/*
 * Initialize the execution stack.
 */
    শূন্য
estack_init(শূন্য)
{
    estack_T *entry;

    যদি (ga_grow(&exestack, 10) == FAIL)
	mch_exit(0);
    entry = ((estack_T *)exestack.ga_data) + exestack.ga_len;
    entry->es_type = ETYPE_শীর্ষ;
    entry->es_name = রিক্ত;
    entry->es_lnum = 0;
#ifdef FEAT_EVAL
    entry->es_info.ufunc = রিক্ত;
#endif
    ++exestack.ga_len;
}

/*
 * Add an item to the execution stack.
 * Returns the new entry or NULL when out of memory.
 */
    estack_T *
estack_ঠেলো(etype_T type, char_u *name, long lnum)
{
    estack_T *entry;

    // If memory allocation fails then we'll pop more than we push, eventually
    // at the top level it will be OK again.
    যদি (ga_grow(&exestack, 1) == FAIL)
	প্রত্যাগম রিক্ত;

    entry = ((estack_T *)exestack.ga_data) + exestack.ga_len;
    entry->es_type = type;
    entry->es_name = name;
    entry->es_lnum = lnum;
#ifdef FEAT_EVAL
    entry->es_info.ufunc = রিক্ত;
#endif
    ++exestack.ga_len;
    প্রত্যাগম entry;
}

#যদি defined(FEAT_EVAL)
/*
 * Add a user function to the execution stack.
 */
    estack_T *
estack_ঠেলো_ufunc(ufunc_T *ufunc, long lnum)
{
    estack_T *entry = estack_ঠেলো(ETYPE_UFUNC,
	    ufunc->uf_name_exp != রিক্ত
				  ? ufunc->uf_name_exp : ufunc->uf_name, lnum);
    যদি (entry != রিক্ত)
	entry->es_info.ufunc = ufunc;
    প্রত্যাগম entry;
}

/*
 * Return TRUE if "ufunc" with "lnum" is already at the top of the exe stack.
 */
    পূর্ণাঙ্ক
estack_শীর্ষ_is_ufunc(ufunc_T *ufunc, long lnum)
{
    estack_T *entry;

    যদি (exestack.ga_len == 0)
	প্রত্যাগম FALSE;
    entry = ((estack_T *)exestack.ga_data) + exestack.ga_len - 1;
    প্রত্যাগম entry->es_type == ETYPE_UFUNC
	&& STRCMP( entry->es_name, ufunc->uf_name_exp != রিক্ত
				    ? ufunc->uf_name_exp : ufunc->uf_name) == 0
	&& entry->es_lnum == lnum;
}
#endif

/*
 * Take an item off of the execution stack and return it.
 */
    estack_T *
estack_বের_করো(শূন্য)
{
    যদি (exestack.ga_len == 0)
	প্রত্যাগম রিক্ত;
    --exestack.ga_len;
    প্রত্যাগম ((estack_T *)exestack.ga_data) + exestack.ga_len;
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
    পূর্ণাঙ্ক		idx;
    etype_T	last_type = ETYPE_SCRIPT;
#endif

    entry = ((estack_T *)exestack.ga_data) + exestack.ga_len - 1;
#ifdef FEAT_EVAL
    যদি (which == ESTACK_SFILE && entry->es_type != ETYPE_UFUNC)
#endif
    {
	যদি (entry->es_name == রিক্ত)
	    প্রত্যাগম রিক্ত;
	প্রত্যাগম vim_strsave(entry->es_name);
    }
#ifdef FEAT_EVAL
    // expand('<sfile>') works in a function for backwards compatibility, but
    // may give an unexpected result.  Disallow it in Vim 9 script.
    যদি (which == ESTACK_SFILE && in_vim9script())
    {
	পূর্ণাঙ্ক  save_emsg_off = emsg_off;

	যদি (emsg_off == 1)
	    // f_expand() silences errors but we do want this one
	    emsg_off = 0;
	emsg(_(e_cannot_expand_sfile_in_vim9_function));
	emsg_off = save_emsg_off;
	প্রত্যাগম রিক্ত;
    }

    // If evaluated in a function or autocommand, return the path of the script
    // where it is defined, at script level the current script path is returned
    // instead.
    যদি (which == ESTACK_SCRIPT)
    {
	// Walk the stack backwards, starting from the current frame.
	প্রতি (idx = exestack.ga_len - 1; idx >= 0; --idx, --entry)
	{
	    যদি (entry->es_type == ETYPE_UFUNC || entry->es_type == ETYPE_AUCMD)
	    {
		sctx_T *def_ctx = entry->es_type == ETYPE_UFUNC
			      ? &entry->es_info.ufunc->uf_script_ctx
			      : entry->es_info.aucmd != রিক্ত
				  ? acp_script_ctx(entry->es_info.aucmd) : রিক্ত;

		প্রত্যাগম def_ctx != রিক্ত && def_ctx->sc_sid > 0
			   ? vim_strsave(SCRIPT_ITEM(def_ctx->sc_sid)->sn_name)
			   : রিক্ত;
	    }
	    অন্যথা যদি (entry->es_type == ETYPE_SCRIPT)
		প্রত্যাগম vim_strsave(entry->es_name);
	}
	প্রত্যাগম রিক্ত;
    }

    // Give information about each stack entry up to the root.
    // For a function we compose the call stack, as it was done in the past:
    //   "function One[123]..Two[456]..Three"
    ga_init2(&ga, পরিমাণ(বর্ণ), 100);
    প্রতি (idx = 0; idx < exestack.ga_len; ++idx)
    {
	entry = ((estack_T *)exestack.ga_data) + idx;
	যদি (entry->es_name != রিক্ত)
	{
	    long	lnum = 0;
	    size_t	added = 0;
	    string_T	type_name = {(char_u *)"", 0};
	    string_T	class_name = {(char_u *)"", 0};
	    string_T	es_name = {entry->es_name, STRLEN(entry->es_name)};

	    যদি (entry->es_type != last_type)
	    {
		বিকল্প (entry->es_type)
		{
		    পক্ষ ETYPE_SCRIPT:
			STR_LITERAL_SET(type_name, "script ");
			ভঙ্গ;
		    পক্ষ ETYPE_UFUNC:
			STR_LITERAL_SET(type_name, "function ");
			ভঙ্গ;
		    স্বভাব:
			ভঙ্গ;
		}
		last_type = entry->es_type;
	    }
	    যদি (entry->es_type == ETYPE_UFUNC && entry->es_info.ufunc->uf_class != রিক্ত)
	    {
		class_name.string = entry->es_info.ufunc->uf_class->class_name.string;
		class_name.length = entry->es_info.ufunc->uf_class->class_name.length;
	    }
	    যদি (idx == exestack.ga_len - 1)
		lnum = which == ESTACK_স্তূপ ? SOURCING_LNUM : 0;
	    অন্যথা
		lnum = entry->es_lnum;

	    len = es_name.length + type_name.length + class_name.length + 26;
	    যদি (ga_grow(&ga, (পূর্ণাঙ্ক)len) == FAIL)
		ভঙ্গ;
	    ga_concat_len(&ga, type_name.string, type_name.length);
	    // For class methods prepend "<class name>." to the function name.
	    যদি (*class_name.string != NUL)
	    {
		added = vim_snprintf_safelen(
		    (বর্ণ *)ga.ga_data + ga.ga_len,
		    ga.ga_maxlen - ga.ga_len,
		    "<SNR>%d_%s.",
		    entry->es_info.ufunc->uf_script_ctx.sc_sid,
		    class_name.string);

		ga.ga_len += (পূর্ণাঙ্ক)added;
	    }
	    ga_concat_len(&ga, es_name.string, es_name.length);
	    // For the bottom entry of <sfile>: do not add the line number, it is used in
	    // <slnum>.  Also leave it out when the number is not set.
	    যদি (lnum != 0)
	    {
		added = vim_snprintf_safelen(
		    (বর্ণ *)ga.ga_data + ga.ga_len,
		    ga.ga_maxlen - ga.ga_len,
		    "[%ld]",
		    lnum);

		ga.ga_len += (পূর্ণাঙ্ক)added;
	    }
	    যদি (idx != exestack.ga_len - 1)
		GA_CONCAT_LITERAL(&ga, "..");
	}
    }

    // Only NUL-terminate when not returning NULL.
    যদি (ga.ga_data != রিক্ত)
	ga_append(&ga, NUL);
    প্রত্যাগম (char_u *)ga.ga_data;
#endif
}

#ifdef FEAT_EVAL
    স্থির শূন্য
stacktrace_ঠেলো_item(
	list_T		*l,
	ufunc_T		*fp,
	char_u		*event,
	linenr_T	lnum,
	char_u		*filepath)
{
    dict_T	*d;
    typval_T	tv;

    d = dict_alloc_lock(VAR_FIXED);
    যদি (d == রিক্ত)
	প্রত্যাগম;

    tv.v_type = VAR_DICT;
    tv.v_lock = VAR_LOCKED;
    tv.vval.v_dict = d;

    যদি (fp != রিক্ত)
	dict_add_func(d, "funcref", fp);
    যদি (event != রিক্ত)
	dict_add_string(d, "event", event);
    dict_add_সংখ্যা(d, "lnum", lnum);
    dict_add_string(d, "filepath", filepath);

    list_append_tv(l, &tv);
}

/*
 * Create the stacktrace from exestack.
 */
    list_T *
stacktrace_create(শূন্য)
{
    list_T	*l;
    পূর্ণাঙ্ক		i;

    l = list_alloc();
    যদি (l == রিক্ত)
	প্রত্যাগম রিক্ত;

    প্রতি (i = 0; i < exestack.ga_len; ++i)
    {
	estack_T *entry = &((estack_T *)exestack.ga_data)[i];
	linenr_T lnum = entry->es_lnum;

	যদি (entry->es_type == ETYPE_SCRIPT)
	    stacktrace_ঠেলো_item(l, রিক্ত, রিক্ত, lnum, entry->es_name);
	অন্যথা যদি (entry->es_type == ETYPE_UFUNC)
	{
	    ufunc_T *fp = entry->es_info.ufunc;
	    sctx_T sctx = fp->uf_script_ctx;
	    char_u *filepath = sctx.sc_sid > 0 ?
				   get_scriptname(sctx.sc_sid) : (char_u *)"";

	    lnum += sctx.sc_lnum;
	    stacktrace_ঠেলো_item(l, fp, রিক্ত, lnum, filepath);
	}
	অন্যথা যদি (entry->es_type == ETYPE_AUCMD)
	{
	    // The autocmd may not have a matching pattern yet, in which case
	    // es_info.aucmd is still NULL.
	    sctx_T *sctx = entry->es_info.aucmd != রিক্ত
			       ? acp_script_ctx(entry->es_info.aucmd) : রিক্ত;
	    char_u *filepath = sctx != রিক্ত && sctx->sc_sid > 0 ?
				   get_scriptname(sctx->sc_sid) : (char_u *)"";

	    যদি (sctx != রিক্ত)
		lnum += sctx->sc_lnum;
	    stacktrace_ঠেলো_item(l, রিক্ত, entry->es_name, lnum, filepath);
	}
    }
    প্রত্যাগম l;
}

/*
 * getstacktrace() function
 */
    শূন্য
f_getstacktrace(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv_list_set(rettv, stacktrace_create());
}
#endif

/*
 * Get DIP_ flags from the [where] argument of a :runtime command.
 * "*argp" is advanced to after the [where] argument if it is found.
 */
    স্থির পূর্ণাঙ্ক
get_runtime_cmd_flags(char_u **argp, size_t where_len)
{
    char_u *arg = *argp;

    যদি (where_len == 0)
	প্রত্যাগম 0;

    যদি (STRNCMP(arg, "START", where_len) == 0)
    {
	*argp = skipwhite(arg + where_len);
	প্রত্যাগম DIP_START + DIP_NORTP;
    }
    যদি (STRNCMP(arg, "OPT", where_len) == 0)
    {
	*argp = skipwhite(arg + where_len);
	প্রত্যাগম DIP_OPT + DIP_NORTP;
    }
    যদি (STRNCMP(arg, "PACK", where_len) == 0)
    {
	*argp = skipwhite(arg + where_len);
	প্রত্যাগম DIP_START + DIP_OPT + DIP_NORTP;
    }
    যদি (STRNCMP(arg, "ALL", where_len) == 0)
    {
	*argp = skipwhite(arg + where_len);
	প্রত্যাগম DIP_START + DIP_OPT;
    }

    প্রত্যাগম 0;
}

/*
 * ":runtime [where] {name}"
 */
    শূন্য
ex_runtime(exarg_T *eap)
{
    char_u  *arg = eap->arg;
    পূর্ণাঙ্ক	    flags = eap->forceit ? DIP_ALL : 0;
    char_u  *p = skiptowhite(arg);
    flags += get_runtime_cmd_flags(&arg, p - arg);
    source_runtime(arg, flags);
}

স্থির পূর্ণাঙ্ক runtime_expand_flags;

/*
 * Set the completion context for the :runtime command.
 */
    শূন্য
set_context_in_runtime_cmd(expand_T *xp, char_u *arg)
{
    char_u  *p = skiptowhite(arg);
    runtime_expand_flags
	= *p != NUL ? get_runtime_cmd_flags(&arg, p - arg) : 0;
    // Skip to the last argument.
    যাবত্ (*(p = skiptowhite_esc(arg)) != NUL)
    {
	যদি (runtime_expand_flags == 0)
	    // When there are multiple arguments and [where] is not specified,
	    // use an unrelated non-zero flag to avoid expanding [where].
	    runtime_expand_flags = DIP_ALL;
	arg = skipwhite(p);
    }
    xp->xp_context = EXPAND_RUNTIME;
    xp->xp_pattern = arg;
}

    স্থির শূন্য
source_callback(char_u *fname, শূন্য *cookie)
{
    (শূন্য)do_source(fname, FALSE, DOSO_NONE, cookie);
}

#ifdef FEAT_EVAL
/*
 * Find an already loaded script "name".
 * If found returns its script ID.  If not found returns -1.
 */
    পূর্ণাঙ্ক
find_script_by_name(char_u *name)
{
    পূর্ণাঙ্ক		    sid;
    scriptitem_T    *si;

    প্রতি (sid = script_বস্তুসমূহ.ga_len; sid > 0; --sid)
    {
	// We used to check inode here, but that doesn't work:
	// - If a script is edited and written, it may get a different
	//   inode number, even though to the user it is the same script.
	// - If a script is deleted and another script is written, with a
	//   different name, the inode may be re-used.
	si = SCRIPT_ITEM(sid);
	যদি (si->sn_name != রিক্ত && fnamecmp(si->sn_name, name) == 0)
	    প্রত্যাগম sid;
    }
    প্রত্যাগম -1;
}

/*
 * Add a new scriptitem with all items initialized.
 * When running out of memory "error" is set to FAIL.
 * Returns the script ID.
 */
    স্থির পূর্ণাঙ্ক
get_new_scriptitem(পূর্ণাঙ্ক *error)
{
    স্থির scid_T   last_current_SID = 0;
    পূর্ণাঙ্ক		    sid = ++last_current_SID;
    scriptitem_T    *si = রিক্ত;

    যদি (ga_grow(&script_বস্তুসমূহ, (পূর্ণাঙ্ক)(sid - script_বস্তুসমূহ.ga_len)) == FAIL)
    {
	*error = FAIL;
	প্রত্যাগম sid;
    }
    যাবত্ (script_বস্তুসমূহ.ga_len < sid)
    {
	si = ALLOC_CLEAR_ONE(scriptitem_T);
	যদি (si == রিক্ত)
	{
	    *error = FAIL;
	    প্রত্যাগম sid;
	}
	++script_বস্তুসমূহ.ga_len;
	SCRIPT_ITEM(script_বস্তুসমূহ.ga_len) = si;
	si->sn_name = রিক্ত;
	si->sn_version = 1;

	// Allocate the local script variables to use for this script.
	new_script_vars(script_বস্তুসমূহ.ga_len);
	ga_init2(&si->sn_var_vals, পরিমাণ(svar_T), 10);
	hash_init(&si->sn_all_vars.dv_hashtab);
	ga_init2(&si->sn_imports, পরিমাণ(imported_T), 10);
	ga_init2(&si->sn_type_list, পরিমাণ(type_T), 10);
# ifdef FEAT_PROFILE
	si->sn_prof_on = FALSE;
# endif
    }

    // "si" can't be NULL, check only to avoid a compiler warning
    যদি (si != রিক্ত)
	// Used to check script variable index is still valid.
	si->sn_script_seq = current_sctx.sc_seq;

    প্রত্যাগম sid;
}

    পূর্ণাঙ্ক
get_new_scriptitem_for_fname(পূর্ণাঙ্ক *error, char_u *fname)
{
    পূর্ণাঙ্ক sid = get_new_scriptitem(error);

    যদি (*error == OK)
    {
	scriptitem_T *si = SCRIPT_ITEM(sid);

	si->sn_name = vim_strsave(fname);
	si->sn_state = SN_STATE_NOT_LOADED;
    }
    প্রত্যাগম sid;
}

/*
 * If the script for "sid" is a symlink and "sn_source_sid" is not set
 * then initialize it. A new script_item is created if needed.
 */
    শূন্য
check_script_symlink(পূর্ণাঙ্ক sid)
{
    scriptitem_T *si = SCRIPT_ITEM(sid);
    যদি (si->sn_syml_checked || si->sn_sourced_sid > 0)
	প্রত্যাগম;
    si->sn_syml_checked = TRUE;

    // If fname is a symbolic link, create an script_item for the real file.

    char_u *real_fname = fix_fname(si->sn_name);
    যদি (real_fname != রিক্ত && STRCMP(real_fname, si->sn_name) != 0)
    {
	পূর্ণাঙ্ক real_sid = find_script_by_name(real_fname);
	পূর্ণাঙ্ক error2 = OK;
	পূর্ণাঙ্ক new_sid = FALSE;
	যদি (real_sid < 0)
	{
	    real_sid = get_new_scriptitem_for_fname(&error2, real_fname);
	    new_sid = TRUE;
	}
	যদি (error2 == OK)
	{
	    si = SCRIPT_ITEM(sid);
	    si->sn_sourced_sid = real_sid;
	    যদি (new_sid)
	    {
		SCRIPT_ITEM(real_sid)->sn_import_autoload
						    = si->sn_import_autoload;
		যদি (si->sn_autoload_prefix != রিক্ত)
		    SCRIPT_ITEM(real_sid)->sn_autoload_prefix =
					vim_strsave(si->sn_autoload_prefix);
	    }
	}
    }
    vim_free(real_fname);
}

    স্থির শূন্য
find_script_callback(char_u *fname, শূন্য *cookie)
{
    পূর্ণাঙ্ক sid;
    পূর্ণাঙ্ক error = OK;
    পূর্ণাঙ্ক *ret_sid = cookie;

    sid = find_script_by_name(fname);
    যদি (sid < 0)
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
    পূর্ণাঙ্ক
do_in_path(
    char_u	*path,
    বর্ণ	*prefix,
    char_u	*name,
    পূর্ণাঙ্ক		flags,
    শূন্য	(*callback)(char_u *fname, শূন্য *ck),
    শূন্য	*cookie)
{
    char_u	*rtp;
    string_T	buf;
    char_u	*rtp_copy;
    পূর্ণাঙ্ক		অঙ্ক_files;
    char_u	**files;
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		did_one = FALSE;
#ifdef AMIGA
    সংরচনা Process	*proc = (সংরচনা Process *)FindTask(0L);
    APTR		save_winptr = proc->pr_WindowPtr;

    // Avoid a requester here for a volume that doesn't exist.
    proc->pr_WindowPtr = (APTR)-1L;
#endif

    // Make a copy of 'runtimepath'.  Invoking the callback may change the
    // value.
    rtp_copy = vim_strsave(path);
    buf.string = alloc(MAXPATHL);
    যদি (buf.string != রিক্ত && rtp_copy != রিক্ত)
    {
	size_t	prefixlen = 0;
	size_t	namelen = 0;

	যদি (name != রিক্ত)
	{
	    prefixlen = STRLEN(prefix);
	    namelen = STRLEN(name);

	    যদি (p_verbose > 10)
	    {
		verbose_enter();
		যদি (*prefix != NUL)
		    smsg(_("Searching for \"%s\" under \"%s\" in \"%s\""),
					       (বর্ণ *)name, prefix, (বর্ণ *)path);
		অন্যথা
		    smsg(_("Searching for \"%s\" in \"%s\""),
						       (বর্ণ *)name, (বর্ণ *)path);
		verbose_leave();
	    }
	}

	// Loop over all entries in 'runtimepath'.
	rtp = rtp_copy;
	যাবত্ (*rtp != NUL && ((flags & DIP_ALL) || !did_one))
	{
	    // Copy the path from 'runtimepath' to buf[].
	    buf.length = (size_t)copy_option_part(&rtp, buf.string, MAXPATHL, ",");

	    // Skip after or non-after directories.
	    যদি (flags & (DIP_NOAFTER | DIP_AFTER))
	    {
		পূর্ণাঙ্ক is_after = buf.length >= 5
		    && STRCMP(buf.string + buf.length - 5, "after") == 0;

		যদি ((is_after && (flags & DIP_NOAFTER))
			|| (!is_after && (flags & DIP_AFTER)))
		    অনুবর্ত;
	    }

	    যদি (name == রিক্ত)
	    {
		(*callback)(buf.string, (শূন্য *)&cookie);
		যদি (!did_one)
		    did_one = (cookie == রিক্ত);
	    }
	    অন্যথা যদি (buf.length + 2 + prefixlen + namelen < MAXPATHL)
	    {
		char_u	*np;
		char_u	*tail;

		যদি (*buf.string != NUL
		    && !after_pathsep(buf.string, buf.string + buf.length))
		{
		    STRCPY(buf.string + buf.length, PATHSEPSTR);
		    buf.length += পরিমাণ(PATHSEP);
		}
		STRCPY(buf.string + buf.length, prefix);
		buf.length += prefixlen;
		tail = buf.string + buf.length;

		// Loop over all patterns in "name"
		np = name;
		যাবত্ (*np != NUL && ((flags & DIP_ALL) || !did_one))
		{
		    // Append the pattern from "name" to buf[].
		    copy_option_part(&np, tail, (পূর্ণাঙ্ক)(MAXPATHL - (tail - buf.string)),
								       "\t ");

		    যদি (p_verbose > 10)
		    {
			verbose_enter();
			smsg(_("Searching for \"%s\""), buf.string);
			verbose_leave();
		    }

		    // Expand wildcards, invoke the callback for each match.
		    যদি (gen_expand_wildcards(1, &buf.string, &অঙ্ক_files, &files,
				  (flags & DIP_DIR) ? EW_DIR : EW_FILE) == OK)
		    {
			প্রতি (i = 0; i < অঙ্ক_files; ++i)
			{
			    (*callback)(files[i], cookie);
			    did_one = TRUE;
			    যদি (!(flags & DIP_ALL))
				ভঙ্গ;
			}
			FreeWild(অঙ্ক_files, files);
		    }
		}
	    }
	}
    }
    vim_free(buf.string);
    vim_free(rtp_copy);
    যদি (!did_one && name != রিক্ত)
    {
	বর্ণ *basepath = path == p_rtp ? "runtimepath" : "packpath";

	যদি (flags & DIP_ERR)
	    semsg(_(e_directory_not_found_in_str_str), basepath, name);
	অন্যথা যদি (p_verbose > 0)
	{
	    verbose_enter();
	    smsg(_("not found in '%s': \"%s\""), basepath, name);
	    verbose_leave();
	}
    }

#ifdef AMIGA
    proc->pr_WindowPtr = save_winptr;
#endif

    প্রত্যাগম did_one ? OK : FAIL;
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
    স্থির পূর্ণাঙ্ক
do_in_path_and_pp(
    char_u	*path,
    char_u	*name,
    পূর্ণাঙ্ক		flags,
    শূন্য	(*callback)(char_u *fname, শূন্য *ck),
    শূন্য	*cookie)
{
    পূর্ণাঙ্ক		done = FAIL;

    যদি ((flags & DIP_NORTP) == 0)
	done = do_in_path(path, "", name, flags, callback, cookie);

    যদি ((done == FAIL || (flags & DIP_ALL)) && (flags & DIP_START))
	done = do_in_path(p_pp, "pack/*/start/*/", name, flags, callback,
								       cookie);

    যদি ((done == FAIL || (flags & DIP_ALL)) && (flags & DIP_OPT))
	done = do_in_path(p_pp, "pack/*/opt/*/", name, flags, callback,
								       cookie);

    প্রত্যাগম done;
}

/*
 * Just like do_in_path_and_pp(), using 'runtimepath' for "path".
 */
    পূর্ণাঙ্ক
do_in_runtimepath(
    char_u	*name,
    পূর্ণাঙ্ক		flags,
    শূন্য	(*callback)(char_u *fname, শূন্য *ck),
    শূন্য	*cookie)
{
    প্রত্যাগম do_in_path_and_pp(p_rtp, name, flags, callback, cookie);
}

/*
 * Source the file "name" from all directories in 'runtimepath'.
 * "name" can contain wildcards.
 * When "flags" has DIP_ALL: source all files, otherwise only the first one.
 *
 * return FAIL when no file could be sourced, OK otherwise.
 */
    পূর্ণাঙ্ক
source_runtime(char_u *name, পূর্ণাঙ্ক flags)
{
    প্রত্যাগম source_in_path(p_rtp, name, flags, রিক্ত);
}

/*
 * Just like source_runtime(), but use "path" instead of 'runtimepath'
 * and return the script ID in "ret_sid".
 */
    পূর্ণাঙ্ক
source_in_path(char_u *path, char_u *name, পূর্ণাঙ্ক flags, পূর্ণাঙ্ক *ret_sid)
{
    প্রত্যাগম do_in_path_and_pp(path, name, flags, source_callback, ret_sid);
}

#যদি defined(FEAT_EVAL)

/*
 * Find "name" in 'runtimepath'. If found a new scriptitem is created for it
 * and it's script ID is returned.
 * If not found returns -1.
 */
    পূর্ণাঙ্ক
find_script_in_rtp(char_u *name)
{
    পূর্ণাঙ্ক sid = -1;

    (শূন্য)do_in_path_and_pp(p_rtp, name, DIP_START | DIP_NOAFTER,
						   find_script_callback, &sid);
    প্রত্যাগম sid;
}

/*
 * Expand wildcards in "pat" and invoke do_source() for each match.
 */
    স্থির শূন্য
source_all_matches(char_u *pat)
{
    পূর্ণাঙ্ক	    অঙ্ক_files;
    char_u  **files;
    পূর্ণাঙ্ক	    i;

    যদি (gen_expand_wildcards(1, &pat, &অঙ্ক_files, &files, EW_FILE) != OK)
	প্রত্যাগম;

    প্রতি (i = 0; i < অঙ্ক_files; ++i)
	(শূন্য)do_source(files[i], FALSE, DOSO_NONE, রিক্ত);
    FreeWild(অঙ্ক_files, files);
}

/*
 * Add the package directory to 'runtimepath'.
 */
    স্থির পূর্ণাঙ্ক
add_pack_dir_to_rtp(char_u *fname)
{
    char_u  *p4, *p3, *p2, *p1, *p;
    char_u  *entry;
    char_u  *insp = রিক্ত;
    পূর্ণাঙ্ক	    c;
    string_T  new_rtp;
    size_t  keep;
    size_t  p_rtp_len;
    string_T  afterdir = {রিক্ত, 0};
    char_u  *after_insp = রিক্ত;
    char_u  *ffname = রিক্ত;
    size_t  fname_len;
    string_T	buf = {রিক্ত, 0};
    পূর্ণাঙ্ক	    retval = FAIL;

    p4 = p3 = p2 = p1 = get_past_head(fname);
    প্রতি (p = p1; *p; MB_PTR_ADV(p))
	যদি (vim_ispathsep_nocolon(*p))
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
    যদি (ffname == রিক্ত)
	প্রত্যাগম FAIL;

    // Find "ffname" in "p_rtp", ignoring '/' vs '\' differences.
    // Also stop at the first "after" directory.
    fname_len = STRLEN(ffname);
    buf.string = alloc(MAXPATHL);
    যদি (buf.string == রিক্ত)
	গচ্ছ theend;

    p_rtp_len = 0;
    প্রতি (entry = p_rtp; *entry != NUL; )
    {
	char_u *cur_entry = entry;

	buf.length = (size_t)copy_option_part(&entry, buf.string, MAXPATHL, ",");

	// keep track of p_rtp length as we go to make the STRLEN() below have less work to do
	p_rtp_len += (*(cur_entry + buf.length) == ',') ? buf.length + 1 : buf.length;

	যদি ((p = (char_u *)strstr((বর্ণ *)buf.string, "after")) != রিক্ত
		&& p > buf.string
		&& vim_ispathsep(p[-1])
		&& (vim_ispathsep(p[5]) || p[5] == NUL || p[5] == ','))
	{
	    যদি (insp == রিক্ত)
		// Did not find "ffname" before the first "after" directory,
		// insert it before this entry.
		insp = cur_entry;
	    after_insp = cur_entry;
	    ভঙ্গ;
	}

	যদি (insp == রিক্ত)
	{
	    char_u  *rtp_ffname;
	    পূর্ণাঙ্ক	    match;

	    যদি (*buf.string != NUL && !after_pathsep(buf.string, buf.string + buf.length))
		STRCPY(buf.string + buf.length, PATHSEPSTR);
	    rtp_ffname = fix_fname(buf.string);
	    যদি (rtp_ffname == রিক্ত)
		গচ্ছ theend;
	    match = vim_fnamencmp(rtp_ffname, ffname, fname_len) == 0;
	    vim_free(rtp_ffname);
	    যদি (match)
		// Insert "ffname" after this entry (and comma).
		insp = entry;
	}
    }

    // finish measuring the length of p_rtp
    p_rtp_len += STRLEN(p_rtp + p_rtp_len);
    যদি (insp == রিক্ত)
	// Both "fname" and "after" not found, append at the end.
	insp = p_rtp + p_rtp_len;

    // check if rtp/pack/name/start/name/after exists
    fname_len = STRLEN(fname);
    concat_fnames(fname, fname_len, (char_u *)"after", STRLEN_LITERAL("after"), TRUE, &afterdir);
    যদি (afterdir.string == রিক্ত || !mch_isdir(afterdir.string))
	afterdir.length = 0;

    new_rtp.string = alloc(p_rtp_len + fname_len + afterdir.length + 3); // add two for commas and one for NUL
    যদি (new_rtp.string == রিক্ত)
	গচ্ছ theend;

    // We now have 'rtp' parts: {keep}{keep_after}{rest}.
    // Create new_rtp, first: {keep},{fname}
    keep = (size_t)(insp - p_rtp);
    mch_memmove(new_rtp.string, p_rtp, keep);
    new_rtp.length = keep;
    যদি (*insp == NUL)
	new_rtp.string[new_rtp.length++] = ',';  // add comma before
    mch_memmove(new_rtp.string + new_rtp.length, fname, fname_len);
    new_rtp.length += fname_len;
    যদি (*insp != NUL)
	new_rtp.string[new_rtp.length++] = ',';  // add comma after

    যদি (afterdir.length > 0 && after_insp != রিক্ত)
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

    যদি (p_rtp[keep] != NUL)
    {
	size_t	append_len = p_rtp_len - keep;

	// Append rest: {keep},{fname}{keep_after},{afterdir}{rest}
	mch_memmove(new_rtp.string + new_rtp.length, p_rtp + keep, append_len + 1);	// add one for NUL
	new_rtp.length += append_len;
    }
    অন্যথা
	new_rtp.string[new_rtp.length] = NUL;

    যদি (afterdir.length > 0 && after_insp == রিক্ত)
    {
	// Append afterdir when "after" was not found:
	// {keep},{fname}{rest},{afterdir}
	new_rtp.string[new_rtp.length++] = ',';
	STRCPY(new_rtp.string + new_rtp.length, afterdir.string);
    }

    set_option_মান_give_err((char_u *)"rtp", 0L, new_rtp.string, 0);
    vim_free(new_rtp.string);
    retval = OK;

theend:
    vim_free(buf.string);
    vim_free(ffname);
    vim_free(afterdir.string);
    প্রত্যাগম retval;
}

/*
 * Load scripts in "plugin" and "ftdetect" directories of the package.
 */
    স্থির পূর্ণাঙ্ক
load_pack_plugin(char_u *fname)
{
    স্থির বর্ণ *plugpat = "%s/plugin/**/*.vim";
    স্থির বর্ণ *ftpat = "%s/ftdetect/*.vim";
    পূর্ণাঙ্ক		len;
    char_u	*ffname = fix_fname(fname);
    char_u	*pat = রিক্ত;
    পূর্ণাঙ্ক		retval = FAIL;

    যদি (ffname == রিক্ত)
	প্রত্যাগম FAIL;
    len = (পূর্ণাঙ্ক)STRLEN(ffname) + (পূর্ণাঙ্ক)STRLEN(ftpat);
    pat = alloc(len);
    যদি (pat == রিক্ত)
	গচ্ছ theend;
    vim_snprintf((বর্ণ *)pat, len, plugpat, ffname);
    source_all_matches(pat);

    {
	char_u *cmd = vim_strsave((char_u *)"g:did_load_filetypes");

	// If runtime/filetype.vim wasn't loaded yet, the scripts will be
	// found when it loads.
	যদি (cmd != রিক্ত && eval_to_সংখ্যা(cmd, FALSE) > 0)
	{
	    do_cmdline_cmd((char_u *)"augroup filetypedetect");
	    vim_snprintf((বর্ণ *)pat, len, ftpat, ffname);
	    source_all_matches(pat);
	    do_cmdline_cmd((char_u *)"augroup END");
	}
	vim_free(cmd);
    }
    vim_free(pat);
    retval = OK;

theend:
    vim_free(ffname);
    প্রত্যাগম retval;
}

// used for "cookie" of add_pack_plugin()
স্থির পূর্ণাঙ্ক APP_ADD_DIR;
স্থির পূর্ণাঙ্ক APP_LOAD;
স্থির পূর্ণাঙ্ক APP_BOTH;

    স্থির শূন্য
add_pack_plugin(char_u *fname, শূন্য *cookie)
{
    যদি (cookie != &APP_LOAD)
    {
	char_u	*buf = alloc(MAXPATHL);
	char_u	*p;
	পূর্ণাঙ্ক	found = FALSE;

	যদি (buf == রিক্ত)
	    প্রত্যাগম;
	p = p_rtp;
	যাবত্ (*p != NUL)
	{
	    copy_option_part(&p, buf, MAXPATHL, ",");
	    যদি (pathcmp((বর্ণ *)buf, (বর্ণ *)fname, -1) == 0)
	    {
		found = TRUE;
		ভঙ্গ;
	    }
	}
	vim_free(buf);
	যদি (!found)
	    // directory is not yet in 'runtimepath', add it
	    যদি (add_pack_dir_to_rtp(fname) == FAIL)
		প্রত্যাগম;
    }

    যদি (cookie != &APP_ADD_DIR)
	load_pack_plugin(fname);
}

/*
 * Add all packages in the "start" directory to 'runtimepath'.
 */
    শূন্য
add_pack_start_dirs(শূন্য)
{
    do_in_path(p_pp, "", (char_u *)"pack/*/start/*", DIP_ALL + DIP_DIR,
					       add_pack_plugin, &APP_ADD_DIR);
}

/*
 * Load plugins from all packages in the "start" directory.
 */
    শূন্য
load_start_packages(শূন্য)
{
    did_source_packages = TRUE;
    do_in_path(p_pp, "", (char_u *)"pack/*/start/*", DIP_ALL + DIP_DIR,
						  add_pack_plugin, &APP_LOAD);
}

/*
 * ":packloadall"
 * Find plugins in the package directories and source them.
 */
    শূন্য
ex_packloadall(exarg_T *eap)
{
    যদি (!did_source_packages || eap->forceit)
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
    শূন্য
ex_packadd(exarg_T *eap)
{
    স্থির বর্ণ *plugpat = "pack/*/%s/%s";
    পূর্ণাঙ্ক		len;
    বর্ণ	*pat;
    পূর্ণাঙ্ক		round;
    পূর্ণাঙ্ক		res = OK;

    // Round 1: use "start", round 2: use "opt".
    প্রতি (round = 1; round <= 2; ++round)
    {
	// Only look under "start" when loading packages wasn't done yet.
	যদি (round == 1 && did_source_packages)
	    অনুবর্ত;

	len = (পূর্ণাঙ্ক)STRLEN(plugpat) + (পূর্ণাঙ্ক)STRLEN(eap->arg) + 5;
	pat = alloc(len);
	যদি (pat == রিক্ত)
	    প্রত্যাগম;
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
    শূন্য
remove_duplicates(garray_T *gap)
{
    পূর্ণাঙ্ক	    i;
    পূর্ণাঙ্ক	    j;
    char_u  **fnames = (char_u **)gap->ga_data;

    sort_strings(fnames, gap->ga_len);
    প্রতি (i = gap->ga_len - 1; i > 0; --i)
	যদি (fnamecmp(fnames[i - 1], fnames[i]) == 0)
	{
	    vim_free(fnames[i]);
	    প্রতি (j = i + 1; j < gap->ga_len; ++j)
		fnames[j - 1] = fnames[j];
	    --gap->ga_len;
	}
}

    স্থির শূন্য
ExpandRTDir_int(
    char_u	*pat,
    size_t	pat_len,
    পূর্ণাঙ্ক		flags,
    পূর্ণাঙ্ক		keep_ext,
    garray_T	*gap,
    বর্ণ	*dirnames[])
{
    প্রতি (পূর্ণাঙ্ক i = 0; dirnames[i] != রিক্ত; ++i)
    {
	ধ্রুব size_t	buf_len = STRLEN(dirnames[i]) + pat_len + 64;
	বর্ণ		*buf = alloc(buf_len);
	যদি (buf == রিক্ত)
	{
	    ga_clear_strings(gap);
	    প্রত্যাগম;
	}
	পূর্ণাঙ্ক		glob_flags = 0;
	পূর্ণাঙ্ক		expand_dirs = FALSE;

	// Build base pattern
	vim_snprintf(buf, buf_len, "%s%s%s%s",
		     *dirnames[i] ? dirnames[i] : "", *dirnames[i] ? "/" : "",
		     pat, "*.vim");

expand:
	যদি ((flags & DIP_NORTP) == 0)
	    globpath(p_rtp, (char_u *)buf, gap, glob_flags, expand_dirs);

	যদি (flags & DIP_START)
	{
	    // Build complete search path: pack/*/start/*/dirnames[i]/pat*.vim
	    vim_snprintf(buf, buf_len, "pack/*/start/*/%s%s%s%s",
			 *dirnames[i] ? dirnames[i] : "",
			 *dirnames[i] ? "/" : "",
			 pat,
			 expand_dirs ? "*" : "*.vim");
	    globpath(p_pp, (char_u *)buf, gap, glob_flags, expand_dirs);
	}

	যদি (flags & DIP_OPT)
	{
	    // Build complete search path: pack/*/opt/*/dirnames[i]/pat*.vim
	    vim_snprintf(buf, buf_len, "pack/*/opt/*/%s%s%s%s",
			 *dirnames[i] ? dirnames[i] : "",
			 *dirnames[i] ? "/" : "", pat,
			 expand_dirs ? "*" : "*.vim");
	    globpath(p_pp, (char_u *)buf, gap, glob_flags, expand_dirs);
	}

	// Second round for directories
	যদি (*dirnames[i] == NUL && !expand_dirs)
	{
	    // expand dir names in another round
	    vim_snprintf(buf, buf_len, "%s*", pat);
	    glob_flags = WILD_ADD_SLASH;
	    expand_dirs = TRUE;
	    গচ্ছ expand;
	}

	vim_free(buf);
    }

    পূর্ণাঙ্ক pat_pathsep_cnt = 0;
    প্রতি (size_t i = 0; i < pat_len; ++i)
    {
	যদি (vim_ispathsep(pat[i]))
	    ++pat_pathsep_cnt;
    }

    প্রতি (পূর্ণাঙ্ক i = 0; i < gap->ga_len; ++i)
    {
	char_u *match = ((char_u **)gap->ga_data)[i];
	char_u *s = match;
	char_u *e = s + STRLEN(s);
	যদি (e - 4 > s && !keep_ext && STRNICMP(e - 4, ".vim", 4) == 0)
	{
	    e -= 4;
	    *e = NUL;
	}

	পূর্ণাঙ্ক match_pathsep_cnt = (e > s && e[-1] == '/') ? -1 : 0;
	প্রতি (s = e; s > match; MB_PTR_BACK(match, s))
	{
	    যদি (s < match || (vim_ispathsep(*s)
				     && ++match_pathsep_cnt > pat_pathsep_cnt))
		ভঙ্গ;
	}
	++s;
	যদি (s != match)
	    mch_memmove(match, s, e - s + 1);
    }

    যদি (gap->ga_len == 0)
	প্রত্যাগম;

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
    পূর্ণাঙ্ক
ExpandRTDir(
    char_u	*pat,
    পূর্ণাঙ্ক		flags,
    পূর্ণাঙ্ক		*অঙ্ক_file,
    char_u	***file,
    বর্ণ	*dirnames[])
{
    *অঙ্ক_file = 0;
    *file = রিক্ত;

    garray_T	ga;
    ga_init2(&ga, পরিমাণ(বর্ণ *), 10);

    ExpandRTDir_int(pat, STRLEN(pat), flags, FALSE, &ga, dirnames);

    যদি (ga.ga_len == 0)
	প্রত্যাগম FAIL;

    *file = ga.ga_data;
    *অঙ্ক_file = ga.ga_len;
    প্রত্যাগম OK;
}

/*
 * Handle command line completion for :runtime command.
 */
    পূর্ণাঙ্ক
expand_runtime_cmd(char_u *pat, পূর্ণাঙ্ক *অঙ্ক_Matches, char_u ***matches)
{
    *অঙ্ক_Matches = 0;
    *matches = রিক্ত;

    garray_T	ga;
    ga_init2(&ga, পরিমাণ(বর্ণ *), 10);

    size_t pat_len = (পূর্ণাঙ্ক)STRLEN(pat);
    বর্ণ *dirnames[] = {"", রিক্ত};
    ExpandRTDir_int(pat, pat_len, runtime_expand_flags, TRUE, &ga, dirnames);

    // Try to complete values for [where] argument when none was found.
    যদি (runtime_expand_flags == 0)
    {
	বর্ণ *where_মানসমূহ[] = {"START", "OPT", "PACK", "ALL"};
	প্রতি (size_t i = 0; i < ARRAY_LENGTH(where_মানসমূহ); ++i)
	    যদি (STRNCMP(pat, where_মানসমূহ[i], pat_len) == 0)
	    {
		char_u *p = vim_strsave((char_u *)where_মানসমূহ[i]);
		যদি (p != রিক্ত && ga_add_string(&ga, p) == FAIL)
		    vim_free(p);
	    }
    }

    যদি (ga.ga_len == 0)
	প্রত্যাগম FAIL;

    *matches = ga.ga_data;
    *অঙ্ক_Matches = ga.ga_len;
    প্রত্যাগম OK;
}

/*
 * Expand loadplugin names:
 * 'packpath'/pack/ * /opt/{pat}
 */
    পূর্ণাঙ্ক
ExpandPackAddDir(
    char_u	*pat,
    পূর্ণাঙ্ক		*অঙ্ক_file,
    char_u	***file)
{
    char_u	*s;
    char_u	*e;
    char_u	*match;
    garray_T	ga;
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		pat_len;

    *অঙ্ক_file = 0;
    *file = রিক্ত;
    pat_len = (পূর্ণাঙ্ক)STRLEN(pat);
    ga_init2(&ga, পরিমাণ(বর্ণ *), 10);

    s = alloc(pat_len + 26);
    যদি (s == রিক্ত)
    {
	ga_clear_strings(&ga);
	প্রত্যাগম FAIL;
    }
    sprintf((বর্ণ *)s, "pack/*/opt/%s*", pat);
    globpath(p_pp, s, &ga, 0, TRUE);
    vim_free(s);

    প্রতি (i = 0; i < ga.ga_len; ++i)
    {
	match = ((char_u **)ga.ga_data)[i];
	s = gettail(match);
	e = s + STRLEN(s);
	mch_memmove(match, s, e - s + 1);
    }

    যদি (ga.ga_len == 0)
	প্রত্যাগম FAIL;

    // Sort and remove duplicates which can happen when specifying multiple
    // directories in dirnames.
    remove_duplicates(&ga);

    *file = ga.ga_data;
    *অঙ্ক_file = ga.ga_len;
    প্রত্যাগম OK;
}

    স্থির শূন্য
cmd_source(char_u *fname, exarg_T *eap)
{
    পূর্ণাঙ্ক clearvars = FALSE;

    যদি (*fname != NUL && STRNCMP(fname, "++clear", 7) == 0)
    {
	// ++clear argument is supplied
	clearvars = TRUE;
	fname = fname + 7;
	যদি (*fname != NUL)
	{
	    semsg(_(e_invalid_argument_str), eap->arg);
	    প্রত্যাগম;
	}
    }

    যদি (*fname != NUL && eap != রিক্ত && eap->addr_গুনতি > 0)
    {
	// if a filename is specified to :source, then a range is not allowed
	emsg(_(e_no_range_allowed));
	প্রত্যাগম;
    }

    যদি (eap != রিক্ত && *fname == NUL)
    {
	যদি (eap->forceit)
	    // a file name is needed to source normal mode commands
	    emsg(_(e_argument_required));
	অন্যথা
	    // source ex commands from the current buffer
	    do_source_ext(রিক্ত, FALSE, DOSO_NONE, রিক্ত, eap, clearvars);
    }
    অন্যথা যদি (eap != রিক্ত && eap->forceit)
	// ":source!": read Normal mode commands
	// Need to execute the commands directly.  This is required at least
	// for:
	// - ":g" command busy
	// - after ":argdo", ":windo" or ":bufdo"
	// - another command follows
	// - inside a loop
	openscript(fname, global_busy || listcmd_busy || eap->nextcmd != রিক্ত
#ifdef FEAT_EVAL
						 || eap->cstack->cs_idx >= 0
#endif
						 );

    // ":source" read ex commands
    অন্যথা যদি (do_source(fname, FALSE, DOSO_NONE, রিক্ত) == FAIL)
	semsg(_(e_cant_open_file_str), fname);
}

/*
 * ":source {fname}"
 */
    শূন্য
ex_source(exarg_T *eap)
{
#ifdef FEAT_BROWSE
    যদি (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	char_u *fname = রিক্ত;

	fname = do_browse(0, (char_u *)_("Source Vim script"), eap->arg,
				      রিক্ত, রিক্ত,
				      (char_u *)_(BROWSE_FILTER_MACROS), রিক্ত);
	যদি (fname != রিক্ত)
	{
	    cmd_source(fname, eap);
	    vim_free(fname);
	}
    }
    অন্যথা
#endif
	cmd_source(eap->arg, eap);
}

#যদি defined(FEAT_EVAL)
/*
 * ":options"
 */
    শূন্য
ex_options(
    exarg_T	*eap UNUSED)
{
    char_u  buf[500];
    পূর্ণাঙ্ক	    multi_mods = 0;

    buf[0] = NUL;
    (শূন্য)add_win_cmd_modifiers(buf, &cmdmod, &multi_mods);

    vim_setenv((char_u *)"OPTWIN_CMD", buf);
    cmd_source((char_u *)SYS_OPTWIN_FILE, রিক্ত);
}
#endif

/*
 * ":source" and associated commands.
 */

#যদি defined(FEAT_EVAL)
/*
 * Return the address holding the next breakpoint line for a source cookie.
 */
    linenr_T *
source_breakpoint(শূন্য *cookie)
{
    প্রত্যাগম &((source_cookie_T *)cookie)->breakpoint;
}

/*
 * Return the address holding the debug tick for a source cookie.
 */
    পূর্ণাঙ্ক *
source_dbg_tick(শূন্য *cookie)
{
    প্রত্যাগম &((source_cookie_T *)cookie)->dbg_tick;
}

/*
 * Return the nesting level for a source cookie.
 */
    পূর্ণাঙ্ক
source_level(শূন্য *cookie)
{
    প্রত্যাগম ((source_cookie_T *)cookie)->level;
}

/*
 * Return the readahead line. Note that the pointer may become invalid when
 * getting the next line, if it's concatenated with the next one.
 */
    char_u *
source_nextline(শূন্য *cookie)
{
    প্রত্যাগম ((source_cookie_T *)cookie)->nextline;
}
#endif

#যদি (defined(MSWIN) && defined(FEAT_CSCOPE)) || defined(HAVE_FD_CLOEXEC)
# define USE_FOPEN_NOINH
/*
 * Special function to open a file without handle inheritance.
 * When possible the handle is closed on exec().
 */
    স্থির FILE *
fopen_noinh_readbin(বর্ণ *filename)
{
# ifdef MSWIN
    পূর্ণাঙ্ক	fd_tmp = mch_open(filename, O_RDONLY | O_দ্বিমিক | O_NOINHERIT, 0);
# অন্যথা
    পূর্ণাঙ্ক	fd_tmp = mch_open(filename, O_RDONLY, 0);
# endif

    যদি (fd_tmp == -1)
	প্রত্যাগম রিক্ত;

# ifdef HAVE_FD_CLOEXEC
    {
	পূর্ণাঙ্ক fdflags = fcntl(fd_tmp, F_GETFD);
	যদি (fdflags >= 0 && (fdflags & FD_CLOEXEC) == 0)
	    (শূন্য)fcntl(fd_tmp, F_SETFD, fdflags | FD_CLOEXEC);
    }
# endif

    প্রত্যাগম fdopen(fd_tmp, READBIN);
}
#endif

/*
 * Initialization for sourcing lines from the current buffer. Reads all the
 * lines from the buffer and stores it in the cookie grow array.
 * Returns a pointer to the name ":source buffer=<n>" on success and NULL on
 * failure.
 */
    স্থির char_u *
do_source_buffer_init(source_cookie_T *sp, exarg_T *eap)
{
    linenr_T	curr_lnum;
    char_u	*সারি = রিক্ত;
    char_u	*fname;

    যদি (curbuf == রিক্ত)
	প্রত্যাগম রিক্ত;

    // Use ":source buffer=<num>" as the script name
    যদি (curbuf->b_ffname != রিক্ত)
	fname = vim_strsave(curbuf->b_ffname);
    অন্যথা
    {
	vim_snprintf((বর্ণ *)IObuff, IOSIZE, ":source buffer=%d", curbuf->b_fnum);
	fname = vim_strsave(IObuff);
    }
    যদি (fname == রিক্ত)
	প্রত্যাগম রিক্ত;

    ga_init2(&sp->buflines, পরিমাণ(char_u *), 100);

    // Copy the lines from the buffer into a grow array
    প্রতি (curr_lnum = eap->line1; curr_lnum <= eap->line2; curr_lnum++)
    {
	সারি = vim_strsave(ml_get(curr_lnum));
	যদি (সারি == রিক্ত)
	    গচ্ছ errret;
	যদি (ga_add_string(&sp->buflines, সারি) == FAIL)
	    গচ্ছ errret;
    }
    sp->buf_lnum = 0;
    sp->source_from_buf = TRUE;
    // When sourcing a range of lines from a buffer, use buffer line number.
    sp->sourcing_lnum = eap->line1 - 1;

    প্রত্যাগম fname;

errret:
    vim_free(fname);
    vim_free(সারি);
    ga_clear_strings(&sp->buflines);
    প্রত্যাগম রিক্ত;
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
    স্থির পূর্ণাঙ্ক
do_source_ext(
    char_u	*fname,
    পূর্ণাঙ্ক		check_other,	    // check for .vimrc and _vimrc
    পূর্ণাঙ্ক		is_vimrc,	    // DOSO_ value
    পূর্ণাঙ্ক		*ret_sid UNUSED,
    exarg_T	*eap,
    পূর্ণাঙ্ক		clearvars UNUSED)
{
    source_cookie_T	    cookie;
    char_u		    *p;
    char_u		    *fname_not_fixed = রিক্ত;
    char_u		    *fname_exp = রিক্ত;
    char_u		    *firstline = রিক্ত;
    পূর্ণাঙ্ক			    retval = FAIL;
    sctx_T		    save_current_sctx;
#ifdef STARTUPTIME
    সংরচনা timeval	    tv_rel;
    সংরচনা timeval	    tv_start;
#endif
#ifdef FEAT_PROFILE
    proftime_T		    wait_start;
#endif
    পূর্ণাঙ্ক			    save_sticky_cmdmod_flags = sticky_cmdmod_flags;
    পূর্ণাঙ্ক			    trigger_source_post = FALSE;
#ifdef FEAT_EVAL
    funccal_entry_T	    funccalp_entry;
    পূর্ণাঙ্ক			    save_debug_break_level = debug_break_level;
    পূর্ণাঙ্ক			    sid = -1;
    scriptitem_T	    *si = রিক্ত;
    পূর্ণাঙ্ক			    save_estack_compiling = estack_compiling;
    ESTACK_CHECK_DECLARATION;
#endif

    CLEAR_FIELD(cookie);
    যদি (fname == রিক্ত)
    {
	// sourcing lines from a buffer
	fname_exp = do_source_buffer_init(&cookie, eap);
	যদি (fname_exp == রিক্ত)
	    প্রত্যাগম FAIL;
    }
    অন্যথা
    {
	fname_not_fixed = expand_env_save(fname);
	যদি (fname_not_fixed == রিক্ত)
	    গচ্ছ theend;
	fname_exp = fix_fname(fname_not_fixed);
	যদি (fname_exp == রিক্ত)
	    গচ্ছ theend;
	যদি (mch_isdir(fname_exp))
	{
	    smsg(_("Cannot source a directory: \"%s\""), fname);
	    গচ্ছ theend;
	}
    }
#ifdef FEAT_EVAL
    estack_compiling = FALSE;

    // See if we loaded this script before.
    sid = find_script_by_name(fname_exp);
    যদি (sid > 0 && ret_sid != রিক্ত
			  && SCRIPT_ITEM(sid)->sn_state != SN_STATE_NOT_LOADED)
    {
	// Already loaded and no need to load again, return here.
	*ret_sid = sid;
	retval = OK;
	গচ্ছ theend;
    }
#endif

    // Apply SourceCmd autocommands, they should get the file and source it.
    // When sourcing a file "fname_exp" was made a full path name by
    // fix_fname() above, avoid expanding it again.
    যদি ((fname == রিক্ত
		? has_autocmd(EVENT_SOURCECMD, fname_exp, রিক্ত)
		: has_autocmd_fullname(EVENT_SOURCECMD, fname_exp, রিক্ত))
	    && apply_autocmds(EVENT_SOURCECMD, fname_exp, fname_exp,
							       FALSE, curbuf))
    {
#ifdef FEAT_EVAL
	retval = aborting() ? FAIL : OK;
#অন্যথা
	retval = OK;
#endif
	যদি (retval == OK)
	    // Apply SourcePost autocommands.
	    apply_autocmds(EVENT_SOURCEPOST, fname_exp, fname_exp,
								FALSE, curbuf);
	গচ্ছ theend;
    }

    // Apply SourcePre autocommands, they may get the file.
    apply_autocmds(EVENT_SOURCEPRE, fname_exp, fname_exp, FALSE, curbuf);

    যদি (!cookie.source_from_buf)
    {
#ifdef USE_FOPEN_NOINH
	cookie.fp = fopen_noinh_readbin((বর্ণ *)fname_exp);
#অন্যথা
	cookie.fp = mch_fopen((বর্ণ *)fname_exp, READBIN);
#endif
    }
    যদি (cookie.fp == রিক্ত && check_other)
    {
	// Try again, replacing file name ".vimrc" by "_vimrc" or vice versa,
	// and ".exrc" by "_exrc" or vice versa.
	p = gettail(fname_exp);
	যদি ((*p == '.' || *p == '_')
		&& (STRICMP(p + 1, "vimrc") == 0
		    || STRICMP(p + 1, "gvimrc") == 0
		    || STRICMP(p + 1, "exrc") == 0))
	{
	    যদি (*p == '_')
		*p = '.';
	    অন্যথা
		*p = '_';
#ifdef USE_FOPEN_NOINH
	    cookie.fp = fopen_noinh_readbin((বর্ণ *)fname_exp);
#অন্যথা
	    cookie.fp = mch_fopen((বর্ণ *)fname_exp, READBIN);
#endif
	}
    }

    যদি (cookie.fp == রিক্ত && !cookie.source_from_buf)
    {
	যদি (p_verbose > 0)
	{
	    verbose_enter();
	    যদি (SOURCING_NAME == রিক্ত)
		smsg(_("could not source \"%s\""), fname);
	    অন্যথা
		smsg(_("line %ld: could not source \"%s\""),
							SOURCING_LNUM, fname);
	    verbose_leave();
	}
	গচ্ছ theend;
    }

    // The file exists.
    // - In verbose mode, give a message.
    // - For a vimrc file, may want to set 'compatible', call vimrc_found().
    যদি (p_verbose > 1)
    {
	verbose_enter();
	যদি (SOURCING_NAME == রিক্ত)
	    smsg(_("sourcing \"%s\""), fname);
	অন্যথা
	    smsg(_("line %ld: sourcing \"%s\""), SOURCING_LNUM, fname);
	verbose_leave();
    }
    যদি (is_vimrc == DOSO_VIMRC)
	vimrc_found(fname_exp, (char_u *)"MYVIMRC");
    অন্যথা যদি (is_vimrc == DOSO_GVIMRC)
	vimrc_found(fname_exp, (char_u *)"MYGVIMRC");

#ifdef USE_CRNL
    // If no automatic file format: Set default to CR-NL.
    যদি (*p_ffs == NUL)
	cookie.fileformat = EOL_DOS;
    অন্যথা
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
    যদি (time_fd != রিক্ত)
	time_ঠেলো(&tv_rel, &tv_start);
#endif

    // "legacy" does not apply to commands in the script
    sticky_cmdmod_flags = 0;

    save_current_sctx = current_sctx;
    যদি ((cmdmod.cmod_flags & CMOD_VIM9CMD) && cookie.source_from_buf)
	// When the ":vim9cmd" command modifier is used, source buffer lines as
	// Vim9 script
	current_sctx.sc_version = SCRIPT_VERSION_VIM9;
    অন্যথা
	current_sctx.sc_version = 1;  // default script version

#ifdef FEAT_EVAL
    current_sctx.sc_lnum = 0;

# ifdef FEAT_PROFILE
    যদি (do_profiling == PROF_YES)
	prof_child_enter(&wait_start);		// entering a child now
# endif

    // Don't use local function variables, if called from a function.
    // Also starts profiling timer for nested script.
    save_funccal(&funccalp_entry);

    // Reset "KeyTyped" to avoid some commands thinking they are invoked
    // interactively.  E.g. defining a function would output indent.
    পূর্ণাঙ্ক save_চাবি_Typed = চাবি_Typed;
    চাবি_Typed = FALSE;

    // Check if this script was sourced before to find its SID.
    // Always use a new sequence number.
    current_sctx.sc_seq = ++last_current_SID_seq;
    যদি (sid > 0)
    {
	hashtab_T	*ht;
	পূর্ণাঙ্ক		todo;
	hashitem_T	*hi;
	dictitem_T	*di;

	// loading the same script again
	current_sctx.sc_sid = sid;
	si = SCRIPT_ITEM(sid);
	যদি (si->sn_state == SN_STATE_NOT_LOADED)
	{
	    // this script was found but not loaded yet
	    si->sn_state = SN_STATE_NEW;
	}
	অন্যথা
	{
	    si->sn_state = SN_STATE_RELOAD;

	    যদি (!clearvars)
	    {
		// Script-local variables remain but "const" can be set again.
		// In Vim9 script variables will be cleared when "vim9script"
		// is encountered without the "noclear" argument.
		ht = &SCRIPT_VARS(sid);
		todo = (পূর্ণাঙ্ক)ht->ht_used;
		FOR_ALL_HASHTAB_বস্তুসমূহ(ht, hi, todo)
		    যদি (!HASHITEM_EMPTY(hi))
		    {
			--todo;
			di = HI2DI(hi);
			di->di_flags |= DI_FLAGS_RELOAD;
		    }
		// imports can be redefined once
		mark_imports_for_reload(sid);
	    }
	    অন্যথা
		clear_vim9_scriptlocal_vars(sid);

	    // reset version, "vim9script" may have been added or removed.
	    si->sn_version = 1;
	}
	যদি (ret_sid != রিক্ত)
	    *ret_sid = sid;
    }
    অন্যথা
    {
	পূর্ণাঙ্ক error = OK;

	// It's new, generate a new SID and initialize the scriptitem.
	sid = get_new_scriptitem(&error);
	current_sctx.sc_sid = sid;
	যদি (error == FAIL)
	    গচ্ছ almosttheend;
	si = SCRIPT_ITEM(sid);
	si->sn_name = fname_exp;
	fname_exp = vim_strsave(si->sn_name);  // used for autocmd
	যদি (ret_sid != রিক্ত)
	    *ret_sid = sid;

	// Remember the "is_vimrc" flag for when the file is sourced again.
	si->sn_is_vimrc = is_vimrc;
    }

    // Keep the sourcing name/lnum, for recursive calls.
    estack_ঠেলো(ETYPE_SCRIPT, si->sn_name, 0);
    ESTACK_CHECK_SETUP;

# ifdef FEAT_PROFILE
    যদি (do_profiling == PROF_YES)
    {
	পূর্ণাঙ্ক	forceit;

	// Check if we do profiling for this script.
	যদি (!si->sn_prof_on && has_profiling(TRUE, si->sn_name, &forceit, রিক্ত))
	{
	    script_do_profile(si);
	    si->sn_pr_force = forceit;
	}
	যদি (si->sn_prof_on)
	{
	    ++si->sn_pr_গুনতি;
	    profile_start(&si->sn_pr_start);
	    profile_zero(&si->sn_pr_children);
	}
    }
# endif
#অন্যথা
    // Keep the sourcing name/lnum, for recursive calls.
    estack_ঠেলো(ETYPE_SCRIPT, fname_exp, 0);
#endif

    cookie.conv.vc_type = CONV_NONE;		// no conversion

    // Read the first line so we can check for a UTF-8 BOM.
    firstline = getsourceline(0, (শূন্য *)&cookie, 0, TRUE);
    যদি (firstline != রিক্ত && STRLEN(firstline) >= 3 && firstline[0] == 0xef
			      && firstline[1] == 0xbb && firstline[2] == 0xbf)
    {
	// Found BOM; setup conversion, skip over BOM and recode the line.
	convert_setup(&cookie.conv, (char_u *)"utf-8", p_enc);
	p = string_convert(&cookie.conv, firstline + 3, রিক্ত);
	যদি (p == রিক্ত)
	    p = vim_strsave(firstline + 3);
	যদি (p != রিক্ত)
	{
	    vim_free(firstline);
	    firstline = p;
	}
    }

    // Call do_cmdline, which will call getsourceline() to get the lines.
    do_cmdline(firstline, getsourceline, (শূন্য *)&cookie,
				     DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_REPEAT);
    retval = OK;

#ifdef FEAT_PROFILE
    যদি (do_profiling == PROF_YES)
    {
	// Get "si" again, "script_items" may have been reallocated.
	si = SCRIPT_ITEM(sid);
	যদি (si->sn_prof_on)
	{
	    profile_end(&si->sn_pr_start);
	    profile_sub_wait(&wait_start, &si->sn_pr_start);
	    profile_add(&si->sn_pr_total, &si->sn_pr_start);
	    profile_self(&si->sn_pr_self, &si->sn_pr_start,
							 &si->sn_pr_children);
	}
    }
#endif

    যদি (got_int)
	emsg(_(e_interrupted));
#ifdef FEAT_EVAL
    ESTACK_CHECK_NOW;
#endif
    estack_বের_করো();
    যদি (p_verbose > 1)
    {
	verbose_enter();
	smsg(_("finished sourcing %s"), fname);
	যদি (SOURCING_NAME != রিক্ত)
	    smsg(_("continuing in %s"), SOURCING_NAME);
	verbose_leave();
    }
#ifdef STARTUPTIME
    যদি (time_fd != রিক্ত)
    {
	vim_snprintf((বর্ণ *)IObuff, IOSIZE, "sourcing %s", fname);
	time_বার্তা((বর্ণ *)IObuff, &tv_start);
	time_বের_করো(&tv_rel);
    }
#endif

    যদি (!got_int)
	trigger_source_post = TRUE;

#ifdef FEAT_EVAL
    // After a "finish" in debug mode, need to break at first command of next
    // sourced file.
    যদি (save_debug_break_level > ex_nesting_level
	    && debug_break_level == ex_nesting_level)
	++debug_break_level;
#endif

#ifdef FEAT_EVAL
almosttheend:
    // If "sn_save_cpo" is set that means we encountered "vim9script": restore
    // 'cpoptions', unless in the main .vimrc file.
    // Get "si" again, "script_items" may have been reallocated.
    si = SCRIPT_ITEM(sid);
    যদি (si->sn_save_cpo != রিক্ত && si->sn_is_vimrc == DOSO_NONE)
    {
	যদি (STRCMP(p_cpo, CPO_VIM) != 0)
	{
	    char_u *f;
	    char_u *t;

	    // 'cpo' was changed in the script.  Apply the same change to the
	    // saved value, if possible.
	    প্রতি (f = (char_u *)CPO_VIM; *f != NUL; ++f)
		যদি (vim_strchr(p_cpo, *f) == রিক্ত
			&& (t = vim_strchr(si->sn_save_cpo, *f)) != রিক্ত)
		    // flag was removed, also remove it from the saved 'cpo'
		    mch_memmove(t, t + 1, STRLEN(t));
	    প্রতি (f = p_cpo; *f != NUL; ++f)
		যদি (vim_strchr((char_u *)CPO_VIM, *f) == রিক্ত
			&& vim_strchr(si->sn_save_cpo, *f) == রিক্ত)
		{
		    // flag was added, also add it to the saved 'cpo'
		    t = alloc(STRLEN(si->sn_save_cpo) + 2);
		    যদি (t != রিক্ত)
		    {
			*t = *f;
			STRCPY(t + 1, si->sn_save_cpo);
			vim_free(si->sn_save_cpo);
			si->sn_save_cpo = t;
		    }
		}
	}
	set_option_মান_give_err((char_u *)"cpo",
					   0L, si->sn_save_cpo, OPT_NO_REDRAW);
    }
    VIM_CLEAR(si->sn_save_cpo);

    restore_funccal();
# ifdef FEAT_PROFILE
    যদি (do_profiling == PROF_YES)
	prof_child_exit(&wait_start);		// leaving a child now
# endif

    চাবি_Typed = save_চাবি_Typed;
#endif
    current_sctx = save_current_sctx;

    যদি (cookie.fp != রিক্ত)
	fclose(cookie.fp);
    যদি (cookie.source_from_buf)
	ga_clear_strings(&cookie.buflines);
    vim_free(cookie.nextline);
    vim_free(firstline);
    convert_setup(&cookie.conv, রিক্ত, রিক্ত);

    যদি (trigger_source_post)
	apply_autocmds(EVENT_SOURCEPOST, fname_exp, fname_exp, FALSE, curbuf);

theend:
#ifdef FEAT_EVAL
    যদি (sid > 0 && ret_sid != রিক্ত
	    && fname_not_fixed != রিক্ত && fname_exp != রিক্ত)
    {
	পূর্ণাঙ্ক not_fixed_sid = find_script_by_name(fname_not_fixed);

	// If "fname_not_fixed" is a symlink then we source the linked file.
	// If the original name is in the script list we add the ID of the
	// script that was actually sourced.
	যদি (SCRIPT_ID_VALID(not_fixed_sid) && not_fixed_sid != sid)
	    SCRIPT_ITEM(not_fixed_sid)->sn_sourced_sid = sid;
    }
#endif

    vim_free(fname_not_fixed);
    vim_free(fname_exp);
    sticky_cmdmod_flags = save_sticky_cmdmod_flags;
#ifdef FEAT_EVAL
    estack_compiling = save_estack_compiling;
#endif
    প্রত্যাগম retval;
}

    পূর্ণাঙ্ক
do_source(
    char_u	*fname,
    পূর্ণাঙ্ক		check_other,	    // check for .vimrc and _vimrc
    পূর্ণাঙ্ক		is_vimrc,	    // DOSO_ value
    পূর্ণাঙ্ক		*ret_sid)
{
    প্রত্যাগম do_source_ext(fname, check_other, is_vimrc, ret_sid, রিক্ত, FALSE);
}


#যদি defined(FEAT_EVAL)

/*
 * ":scriptnames"
 */
    শূন্য
ex_scriptnames(exarg_T *eap)
{
    পূর্ণাঙ্ক i;

    যদি (eap->addr_গুনতি > 0 || *eap->arg != NUL)
    {
	// :script {scriptId}: edit the script
	যদি (eap->addr_গুনতি > 0 && !SCRIPT_ID_VALID(eap->line2))
	    emsg(_(e_invalid_argument));
	অন্যথা
	{
	    যদি (eap->addr_গুনতি > 0)
		eap->arg = SCRIPT_ITEM(eap->line2)->sn_name;
	    অন্যথা
	    {
		expand_env(eap->arg, NameBuff, MAXPATHL);
		eap->arg = NameBuff;
	    }
	    do_exedit(eap, রিক্ত);
	}
	প্রত্যাগম;
    }

    প্রতি (i = 1; i <= script_বস্তুসমূহ.ga_len && !got_int; ++i)
    {
	scriptitem_T *si = SCRIPT_ITEM(i);

	যদি (si->sn_name != রিক্ত)
	{
	    বর্ণ sourced_buf[20];

	    home_replace(রিক্ত, si->sn_name, NameBuff, MAXPATHL, TRUE);
	    যদি (si->sn_sourced_sid > 0)
		vim_snprintf(sourced_buf, 20, "->%d", si->sn_sourced_sid);
	    অন্যথা
		sourced_buf[0] = NUL;
	    vim_snprintf((বর্ণ *)IObuff, IOSIZE, "%3d%s%s: %s",
		    i,
		    sourced_buf,
		    si->sn_state == SN_STATE_NOT_LOADED ? " A" : "",
		    NameBuff);
	    যদি (!message_filtered(IObuff))
	    {
		বার্তা_putchar('\n');
		বার্তা_outtrans(IObuff);
		out_flush();	    // output one line at a time
		ui_breakcheck();
	    }
	}
    }
}

# যদি defined(BACKSLASH_IN_FILENAME)
/*
 * Fix slashes in the list of script names for 'shellslash'.
 */
    শূন্য
scriptnames_slash_adjust(শূন্য)
{
    পূর্ণাঙ্ক i;

    প্রতি (i = 1; i <= script_বস্তুসমূহ.ga_len; ++i)
	যদি (SCRIPT_ITEM(i)->sn_name != রিক্ত)
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
    যদি (id == SID_MODELINE)
	প্রত্যাগম (char_u *)_("modeline");
    যদি (id == SID_CMDARG)
	প্রত্যাগম (char_u *)_("--cmd argument");
    যদি (id == SID_CARG)
	প্রত্যাগম (char_u *)_("-c argument");
    যদি (id == SID_ENV)
	প্রত্যাগম (char_u *)_("environment variable");
    যদি (id == SID_ERROR)
	প্রত্যাগম (char_u *)_("error handler");
    যদি (id == SID_WINLAYOUT)
	প্রত্যাগম (char_u *)_("changed window size");
    প্রত্যাগম SCRIPT_ITEM(id)->sn_name;
}

# যদি defined(EXITFREE)
    শূন্য
free_scriptnames(শূন্য)
{
    পূর্ণাঙ্ক			i;

    প্রতি (i = script_বস্তুসমূহ.ga_len; i > 0; --i)
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
    ga_clear(&script_বস্তুসমূহ);
}

    শূন্য
free_autoload_scriptnames(শূন্য)
{
    ga_clear_strings(&ga_loaded);
}
# endif

    linenr_T
get_sourced_lnum(
	char_u *(*fgetline)(পূর্ণাঙ্ক, শূন্য *, পূর্ণাঙ্ক, getline_opt_T),
	শূন্য *cookie)
{
    প্রত্যাগম fgetline == getsourceline
			? ((source_cookie_T *)cookie)->sourcing_lnum
			: SOURCING_LNUM;
}

/*
 * Return a List of script-local functions defined in the script with id
 * 'sid'.
 */
    স্থির list_T *
get_script_local_funcs(scid_T sid)
{
    hashtab_T	*functbl;
    hashitem_T	*hi;
    long_u	todo;
    list_T	*l;

    l = list_alloc();
    যদি (l == রিক্ত)
	প্রত্যাগম রিক্ত;

    // Iterate through all the functions in the global function hash table
    // looking for functions with script ID 'sid'.
    functbl = func_tbl_get();
    todo = functbl->ht_used;
    FOR_ALL_HASHTAB_বস্তুসমূহ(functbl, hi, todo)
    {
	ufunc_T	*fp;

	যদি (HASHITEM_EMPTY(hi))
	    অনুবর্ত;

	--todo;
	fp = HI2UF(hi);

	// Add active functions with script id == 'sid'
	যদি (!(fp->uf_flags & FC_DEAD) && (fp->uf_script_ctx.sc_sid == sid))
	{
	    char_u	*name;

	    যদি (fp->uf_name_exp != রিক্ত)
		name = fp->uf_name_exp;
	    অন্যথা
		name = fp->uf_name;

	    list_append_string(l, name, -1);
	}
    }

    প্রত্যাগম l;
}

/*
 * getscriptinfo() function
 */
    শূন্য
f_getscriptinfo(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    char_u	*pat = রিক্ত;
    regmatch_T	regmatch;
    পূর্ণাঙ্ক		filterpat = FALSE;
    scid_T	sid = -1;

    যদি (rettv_list_alloc(rettv) == FAIL)
	প্রত্যাগম;

    যদি (check_for_opt_dict_arg(argvars, 0) == FAIL)
	প্রত্যাগম;

    l = rettv->vval.v_list;

    regmatch.regprog = রিক্ত;
    regmatch.rm_ic = p_ic;

    যদি (argvars[0].v_type == VAR_DICT)
    {
	dictitem_T *sid_di = dict_find(argvars[0].vval.v_dict,
							   (char_u *)"sid", 3);
	যদি (sid_di != রিক্ত)
	{
	    পূর্ণাঙ্ক error = FALSE;
	    sid = tv_get_সংখ্যা_chk(&sid_di->di_tv, &error);
	    যদি (error)
		প্রত্যাগম;
	    যদি (sid <= 0)
	    {
		semsg(_(e_invalid_মান_for_argument_str_str), "sid",
						tv_get_string(&sid_di->di_tv));
		প্রত্যাগম;
	    }
	}
	অন্যথা
	{
	    pat = dict_get_string(argvars[0].vval.v_dict, "name", TRUE);
	    যদি (pat != রিক্ত)
		regmatch.regprog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
	    যদি (regmatch.regprog != রিক্ত)
		filterpat = TRUE;
	}
    }

    প্রতি (varnumber_T i = sid > 0 ? sid : 1;
		       (i == sid || sid <= 0) && i <= script_বস্তুসমূহ.ga_len; ++i)
    {
	scriptitem_T	*si = SCRIPT_ITEM(i);
	dict_T		*d;

	যদি (si->sn_name == রিক্ত)
	    অনুবর্ত;

	যদি (filterpat && !vim_regexec(&regmatch, si->sn_name, (colnr_T)0))
	    অনুবর্ত;

	যদি ((d = dict_alloc()) == রিক্ত
		|| list_append_dict(l, d) == FAIL)
	{
	    dict_unref(d);
	    গচ্ছ theend;
	}
	যদি (dict_add_string(d, "name", si->sn_name) == FAIL
		|| dict_add_সংখ্যা(d, "sid", i) == FAIL
		|| dict_add_সংখ্যা(d, "sourced", si->sn_sourced_sid) == FAIL
		|| dict_add_সংখ্যা(d, "version", si->sn_version) == FAIL
		|| dict_add_bool(d, "autoload",
				si->sn_state == SN_STATE_NOT_LOADED) == FAIL)
	    গচ্ছ theend;

	// When a script ID is specified, return information about only the
	// specified script, and add the script-local variables and functions.
	যদি (sid > 0)
	{
	    dict_T	*var_dict;
	    list_T	*fn_list;

	    var_dict = dict_copy(&si->sn_vars->sv_dict, TRUE, TRUE,
								get_copyID());
	    যদি (var_dict == রিক্ত)
		গচ্ছ theend;
	    যদি (dict_add_dict(d, "variables", var_dict) == FAIL)
	    {
		dict_unref(var_dict);
		গচ্ছ theend;
	    }
	    --var_dict->dv_refcount;
	    fn_list = get_script_local_funcs(sid);
	    যদি (fn_list == রিক্ত || dict_add_list(d, "functions", fn_list) == FAIL)
	    {
		list_unref(fn_list);
		গচ্ছ theend;
	    }
	}
    }

theend:
    vim_regfree(regmatch.regprog);
    vim_free(pat);
}

#endif

    স্থির char_u *
get_one_sourceline(source_cookie_T *sp)
{
    garray_T		ga;
    পূর্ণাঙ্ক			len;
    পূর্ণাঙ্ক			c;
    char_u		*buf;
#ifdef USE_CRNL
    পূর্ণাঙ্ক			has_cr;		// CR-LF found
#endif
    পূর্ণাঙ্ক			have_read = FALSE;

    // use a growarray to store the sourced line
    ga_init2(&ga, 1, 250);

    // Loop until there is a finished line (or end-of-file).
    ++sp->sourcing_lnum;
    প্রতি (;;)
    {
	// make room to read at least 120 (more) characters
	যদি (ga_grow(&ga, 120) == FAIL)
	    ভঙ্গ;
	যদি (sp->source_from_buf)
	{
	    যদি (sp->buf_lnum >= sp->buflines.ga_len)
		ভঙ্গ;		    // all the lines are processed
	    ga_concat(&ga, ((char_u **)sp->buflines.ga_data)[sp->buf_lnum]);
	    sp->buf_lnum++;
	    যদি (ga_grow(&ga, 1) == FAIL)
		ভঙ্গ;
	    buf = (char_u *)ga.ga_data;
	    buf[ga.ga_len++] = NUL;
	    len = ga.ga_len;
	}
	অন্যথা
	{
	    buf = (char_u *)ga.ga_data;
	    যদি (fgets((বর্ণ *)buf + ga.ga_len, ga.ga_maxlen - ga.ga_len,
			sp->fp) == রিক্ত)
		ভঙ্গ;
	    len = ga.ga_len + (পূর্ণাঙ্ক)STRLEN(buf + ga.ga_len);
	}
#ifdef USE_CRNL
	// Ignore a trailing CTRL-Z, when in Dos mode.	Only recognize the
	// CTRL-Z by its own, or after a NL.
	যদি (	   (len == 1 || (len >= 2 && buf[len - 2] == '\n'))
		&& sp->fileformat == EOL_DOS
		&& buf[len - 1] == Ctrl_Z)
	{
	    buf[len - 1] = NUL;
	    ভঙ্গ;
	}
#endif

	have_read = TRUE;
	ga.ga_len = len;

	// If the line was longer than the buffer, read more.  A line of a
	// buffer was taken as a whole.
	যদি (!sp->source_from_buf && ga.ga_maxlen - ga.ga_len == 1
						       && buf[len - 1] != '\n')
	    অনুবর্ত;

	যদি (len >= 1 && buf[len - 1] == '\n')	// remove trailing NL
	{
#ifdef USE_CRNL
	    has_cr = (len >= 2 && buf[len - 2] == '\r');
	    যদি (sp->fileformat == EOL_UNKNOWN)
	    {
		যদি (has_cr)
		    sp->fileformat = EOL_DOS;
		অন্যথা
		    sp->fileformat = EOL_UNIX;
	    }

	    যদি (sp->fileformat == EOL_DOS)
	    {
		যদি (has_cr)	    // replace trailing CR
		{
		    buf[len - 2] = '\n';
		    --len;
		    --ga.ga_len;
		}
		অন্যথা	    // lines like ":map xx yy^M" will have failed
		{
		    যদি (!sp->error)
		    {
			বার্তা_source(HL_ATTR(HLF_W));
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
	    প্রতি (c = len - 2; c >= 0 && buf[c] == Ctrl_V; c--)
		;
	    যদি ((len & 1) != (c & 1))	// escaped NL, read more
	    {
		++sp->sourcing_lnum;
		অনুবর্ত;
	    }

	    buf[len - 1] = NUL;		// remove the NL
	}

	// Check for ^C here now and then, so recursive :so can be broken.
	সারি_breakcheck();
	ভঙ্গ;
    }

    যদি (have_read)
	প্রত্যাগম (char_u *)ga.ga_data;

    vim_free(ga.ga_data);
    প্রত্যাগম রিক্ত;
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
	পূর্ণাঙ্ক c UNUSED,
	শূন্য *cookie,
	পূর্ণাঙ্ক indent UNUSED,
	getline_opt_T options)
{
    source_cookie_T	*sp = (source_cookie_T *)cookie;
    char_u		*সারি;
    char_u		*p;
    পূর্ণাঙ্ক			do_vim9_all = in_vim9script()
					      && options == GETLINE_CONCAT_ALL;
    পূর্ণাঙ্ক			do_bar_cont = do_vim9_all
					 || options == GETLINE_CONCAT_CONTBAR;

#ifdef FEAT_EVAL
    // If breakpoints have been added/deleted need to check for it.
    যদি ((sp->dbg_tick < debug_tick) && !sp->source_from_buf)
    {
	sp->breakpoint = dbg_find_breakpoint(TRUE, sp->fname, SOURCING_LNUM);
	sp->dbg_tick = debug_tick;
    }
# ifdef FEAT_PROFILE
    যদি (do_profiling == PROF_YES)
	script_সারি_end();
# endif
#endif

    // Set the current sourcing line number.
    SOURCING_LNUM = sp->sourcing_lnum + 1;

    // Get current line.  If there is a read-ahead line, use it, otherwise get
    // one now.  "fp" is NULL if actually using a string.
    যদি (sp->finished || (!sp->source_from_buf && sp->fp == রিক্ত))
	সারি = রিক্ত;
    অন্যথা যদি (sp->nextline == রিক্ত)
	সারি = get_one_sourceline(sp);
    অন্যথা
    {
	সারি = sp->nextline;
	sp->nextline = রিক্ত;
	++sp->sourcing_lnum;
    }
#ifdef FEAT_PROFILE
    যদি (সারি != রিক্ত && do_profiling == PROF_YES)
	script_সারি_start();
#endif

    // Only concatenate lines starting with a \ when 'cpoptions' doesn't
    // contain the 'C' flag.
    যদি (সারি != রিক্ত && options != GETLINE_NONE
				      && vim_strchr(p_cpo, CPO_CONCAT) == রিক্ত)
    {
	পূর্ণাঙ্ক comment_char = in_vim9script() ? '#' : '"';

	// compensate for the one line read-ahead
	--sp->sourcing_lnum;

	// Get the next line and concatenate it when it starts with a
	// backslash. We always need to read the next line, keep it in
	// sp->nextline.
	/* Also check for a comment in between continuation lines: "\ */
	// Also check for a Vim9 comment, empty line, line starting with '|',
	// but not "||".
	sp->nextline = get_one_sourceline(sp);
	যদি (sp->nextline != রিক্ত
		&& (*(p = skipwhite(sp->nextline)) == '\\'
			      || (p[0] == comment_char
						&& p[1] == '\\' && p[2] == ' ')
			      || (do_vim9_all && (*p == NUL
						     || vim9_comment_start(p)))
			      || (do_bar_cont && p[0] == '|' && p[1] != '|')))
	{
	    garray_T    ga;

	    ga_init2(&ga, পরিমাণ(char_u), 400);
	    ga_concat(&ga, সারি);
	    যদি (*p == '\\')
		ga_concat(&ga, p + 1);
	    অন্যথা যদি (*p == '|')
	    {
		GA_CONCAT_LITERAL(&ga, " ");
		ga_concat(&ga, p);
	    }
	    প্রতি (;;)
	    {
		vim_free(sp->nextline);
		sp->nextline = get_one_sourceline(sp);
		যদি (sp->nextline == রিক্ত)
		    ভঙ্গ;
		p = skipwhite(sp->nextline);
		যদি (*p == '\\' || (do_bar_cont && p[0] == '|' && p[1] != '|'))
		{
		    // Adjust the growsize to the current length to speed up
		    // concatenating many lines.
		    যদি (ga.ga_len > 400)
		    {
			যদি (ga.ga_len > 8000)
			    ga.ga_growsize = 8000;
			অন্যথা
			    ga.ga_growsize = ga.ga_len;
		    }
		    যদি (*p == '\\')
			ga_concat(&ga, p + 1);
		    অন্যথা
		    {
			GA_CONCAT_LITERAL(&ga, " ");
			ga_concat(&ga, p);
		    }
		}
		অন্যথা যদি (!(p[0] == (comment_char)
						&& p[1] == '\\' && p[2] == ' ')
		     && !(do_vim9_all && (*p == NUL || vim9_comment_start(p))))
		    ভঙ্গ;
		/* drop a # comment or "\ comment line */
	    }
	    ga_append(&ga, NUL);
	    vim_free(সারি);
	    সারি = ga.ga_data;
	}
    }

    যদি (সারি != রিক্ত && sp->conv.vc_type != CONV_NONE)
    {
	char_u	*s;

	// Convert the encoding of the script line.
	s = string_convert(&sp->conv, সারি, রিক্ত);
	যদি (s != রিক্ত)
	{
	    vim_free(সারি);
	    সারি = s;
	}
    }

#ifdef FEAT_EVAL
    // Did we encounter a breakpoint?
    যদি (!sp->source_from_buf && sp->breakpoint != 0
	    && sp->breakpoint <= SOURCING_LNUM)
    {
	dbg_breakpoint(sp->fname, SOURCING_LNUM);
	// Find next breakpoint.
	sp->breakpoint = dbg_find_breakpoint(TRUE, sp->fname, SOURCING_LNUM);
	sp->dbg_tick = debug_tick;
    }
#endif

    প্রত্যাগম সারি;
}

/*
 * Returns TRUE if sourcing a script either from a file or a buffer.
 * Otherwise returns FALSE.
 */
    পূর্ণাঙ্ক
sourcing_a_script(exarg_T *eap)
{
    প্রত্যাগম (getline_equal(eap->ea_getline, eap->cookie, getsourceline));
}

/*
 * ":scriptencoding": Set encoding conversion for a sourced script.
 */
    শূন্য
ex_scriptencoding(exarg_T *eap)
{
    source_cookie_T	*sp;
    char_u		*name;

    যদি (!sourcing_a_script(eap))
    {
	emsg(_(e_scriptencoding_used_outside_of_sourced_file));
	প্রত্যাগম;
    }

    যদি (*eap->arg != NUL)
    {
	name = enc_canonize(eap->arg);
	যদি (name == রিক্ত)	// out of memory
	    প্রত্যাগম;
    }
    অন্যথা
	name = eap->arg;

    // Setup for conversion from the specified encoding to 'encoding'.
    sp = (source_cookie_T *)getline_cookie(eap->ea_getline, eap->cookie);
    convert_setup(&sp->conv, name, p_enc);

    যদি (name != eap->arg)
	vim_free(name);
}

/*
 * ":scriptversion": Set Vim script version for a sourced script.
 */
    শূন্য
ex_scriptversion(exarg_T *eap UNUSED)
{
    পূর্ণাঙ্ক		nr;

    যদি (!sourcing_a_script(eap))
    {
	emsg(_(e_scriptversion_used_outside_of_sourced_file));
	প্রত্যাগম;
    }
    যদি (in_vim9script())
    {
	emsg(_(e_cannot_use_scriptversion_after_vim9script));
	প্রত্যাগম;
    }

    nr = getdigits(&eap->arg);
    যদি (nr == 0 || *eap->arg != NUL)
	emsg(_(e_invalid_argument));
    অন্যথা যদি (nr > SCRIPT_VERSION_MAX)
	semsg(_(e_scriptversion_not_supported_nr), nr);
    অন্যথা
    {
	current_sctx.sc_version = nr;
#ifdef FEAT_EVAL
	SCRIPT_ITEM(current_sctx.sc_sid)->sn_version = nr;
#endif
    }
}

#যদি defined(FEAT_EVAL)
/*
 * ":finish": Mark a sourced file as finished.
 */
    শূন্য
ex_finish(exarg_T *eap)
{
    যদি (sourcing_a_script(eap))
	do_finish(eap, FALSE);
    অন্যথা
	emsg(_(e_finish_used_outside_of_sourced_file));
}

/*
 * Mark a sourced file as finished.  Possibly makes the ":finish" pending.
 * Also called for a pending finish at the ":endtry" or after returning from
 * an extra do_cmdline().  "reanimate" is used in the latter case.
 */
    শূন্য
do_finish(exarg_T *eap, পূর্ণাঙ্ক reanimate)
{
    পূর্ণাঙ্ক		idx;

    যদি (reanimate)
	((source_cookie_T *)getline_cookie(eap->ea_getline,
					      eap->cookie))->finished = FALSE;

    // Cleanup (and inactivate) conditionals, but stop when a try conditional
    // not in its finally clause (which then is to be executed next) is found.
    // In this case, make the ":finish" pending for execution at the ":endtry".
    // Otherwise, finish normally.
    idx = cleanup_conditionals(eap->cstack, 0, TRUE);
    যদি (idx >= 0)
    {
	eap->cstack->cs_pending[idx] = CSTP_FINISH;
	report_make_pending(CSTP_FINISH, রিক্ত);
    }
    অন্যথা
	((source_cookie_T *)getline_cookie(eap->ea_getline,
					       eap->cookie))->finished = TRUE;
}


/*
 * Return TRUE when a sourced file had the ":finish" command: Don't give error
 * message for missing ":endif".
 * Return FALSE when not sourcing a file.
 */
    পূর্ণাঙ্ক
source_finished(
    char_u	*(*fgetline)(পূর্ণাঙ্ক, শূন্য *, পূর্ণাঙ্ক, getline_opt_T),
    শূন্য	*cookie)
{
    প্রত্যাগম (getline_equal(fgetline, cookie, getsourceline)
	    && ((source_cookie_T *)getline_cookie(
						fgetline, cookie))->finished);
}

/*
 * Find the path of a script below the "autoload" directory.
 * Returns NULL if there is no "/autoload/" in the script name.
 */
    স্থির char_u *
script_name_after_autoload(scriptitem_T *si)
{
    char_u	*p = si->sn_name;
    char_u	*res = রিক্ত;

    প্রতি (;;)
    {
	char_u *n = (char_u *)strstr((বর্ণ *)p, "autoload");

	যদি (n == রিক্ত)
	    ভঙ্গ;
	যদি (n > p && vim_ispathsep(n[-1]) && vim_ispathsep(n[8]))
	    res = n + 9;
	p = n + 8;
    }
    প্রত্যাগম res;
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

    যদি (p == রিক্ত)
	প্রত্যাগম রিক্ত;
    prefix = vim_strsave(p);
    যদি (prefix == রিক্ত)
	প্রত্যাগম রিক্ত;

    // replace all '/' with '#' and locate ".vim" at the end
    প্রতি (p = prefix; *p != NUL; p += mb_ptr2len(p))
    {
	যদি (vim_ispathsep(*p))
	    *p = '#';
	অন্যথা যদি (STRCMP(p, ".vim") == 0)
	{
	    p[0] = '#';
	    p[1] = NUL;
	    প্রত্যাগম prefix;
	}
    }

    // did not find ".vim" at the end
    vim_free(prefix);
    প্রত্যাগম রিক্ত;
}

/*
 * If in a Vim9 autoload script return "name" with the autoload prefix for the
 * script.  If successful the returned name is allocated.
 * Otherwise it returns "name" unmodified.
 */
    char_u *
may_prefix_autoload(char_u *name)
{
    যদি (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	প্রত্যাগম name;

    scriptitem_T *si = SCRIPT_ITEM(current_sctx.sc_sid);

    যদি (si->sn_autoload_prefix == রিক্ত)
	প্রত্যাগম name;

    char_u  *basename = name;
    size_t  len;
    char_u  *res;

    যদি (*name == K_SPECIAL)
    {
	char_u *p = vim_strchr(name, '_');

	// skip over "<SNR>99_"
	যদি (p != রিক্ত)
	    basename = p + 1;
    }

    len = STRLEN(si->sn_autoload_prefix) + STRLEN(basename) + 2;
    res = alloc(len);
    যদি (res == রিক্ত)
	প্রত্যাগম রিক্ত;

    vim_snprintf((বর্ণ *)res, len, "%s%s", si->sn_autoload_prefix, basename);
    প্রত্যাগম res;
}

/*
 * Return the autoload script name for a function or variable name.
 * Returns NULL when out of memory.
 * Caller must make sure that "name" contains AUTOLOAD_CHAR.
 */
    char_u *
autoload_name(char_u *name)
{
    char_u	*p, *q = রিক্ত;
    char_u	*scriptname;

    // Get the script file name: replace '#' with '/', append ".vim".
    scriptname = alloc(STRLEN(name) + 14);
    যদি (scriptname == রিক্ত)
	প্রত্যাগম রিক্ত;
    STRCPY(scriptname, "autoload/");
    STRCAT(scriptname, name[0] == 'g' && name[1] == ':' ? name + 2: name);
    প্রতি (p = scriptname + 9; (p = vim_strchr(p, AUTOLOAD_CHAR)) != রিক্ত;
								    q = p, ++p)
	*p = '/';
    STRCPY(q, ".vim");
    প্রত্যাগম scriptname;
}

/*
 * If "name" has a package name try autoloading the script for it.
 * Return TRUE if a package was loaded.
 */
    পূর্ণাঙ্ক
script_autoload(
    char_u	*name,
    পূর্ণাঙ্ক		reload)	    // load script again when already loaded
{
    char_u	*p;
    char_u	*scriptname, *tofree;
    পূর্ণাঙ্ক		ret = FALSE;
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		ret_sid;

    // If the name starts with "<SNR>123_" then "123" is the script ID.
    যদি (name[0] == K_SPECIAL && name[1] == KS_EXTRA && name[2] == KE_SNR)
    {
	p = name + 3;
	ret_sid = (পূর্ণাঙ্ক)getdigits(&p);
	যদি (*p == '_' && SCRIPT_ID_VALID(ret_sid))
	{
	    may_load_script(ret_sid, &ret);
	    প্রত্যাগম ret;
	}
    }

    // If there is no '#' after name[0] there is no package name.
    p = vim_strchr(name, AUTOLOAD_CHAR);
    যদি (p == রিক্ত || p == name)
	প্রত্যাগম FALSE;

    tofree = scriptname = autoload_name(name);
    যদি (scriptname == রিক্ত)
	প্রত্যাগম FALSE;

    // Find the name in the list of previously loaded package names.  Skip
    // "autoload/", it's always the same.
    প্রতি (i = 0; i < ga_loaded.ga_len; ++i)
	যদি (STRCMP(((char_u **)ga_loaded.ga_data)[i] + 9, scriptname + 9) == 0)
	    ভঙ্গ;
    যদি (!reload && i < ga_loaded.ga_len)
	ret = FALSE;	    // was loaded already
    অন্যথা
    {
	// Remember the name if it wasn't loaded already.
	যদি (i == ga_loaded.ga_len && ga_grow(&ga_loaded, 1) == OK)
	{
	    ((char_u **)ga_loaded.ga_data)[ga_loaded.ga_len++] = scriptname;
	    tofree = রিক্ত;
	}

	// Try loading the package from $VIMRUNTIME/autoload/<name>.vim
	// Use "ret_sid" to avoid loading the same script again.
	যদি (source_in_path(p_rtp, scriptname, DIP_START, &ret_sid) == OK)
	    ret = TRUE;
    }

    vim_free(tofree);
    প্রত্যাগম ret;
}
#endif
