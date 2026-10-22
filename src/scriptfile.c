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

#அந்தர்பாவ "vim.h"

#எனில் defined(FEAT_EVAL)
// The names of packages that once were loaded are remembered.
நிலையான garray_T		ga_loaded = {0, 0, அளவு(char_u *), 4, வெற்று};
#endif

// last used sequence number for sourcing scripts (current_sctx.sc_seq)
#ifdef FEAT_EVAL
நிலையான முழு		last_current_SID_seq = 0;
#endif

நிலையான முழு do_source_ext(char_u *fname, முழு check_other, முழு is_vimrc, முழு *ret_sid, exarg_T *eap, முழு clearvars);

/*
 * Initialize the execution stack.
 */
    வெறுமை
estack_init(வெறுமை)
{
    estack_T *entry;

    எனில் (ga_grow(&exestack, 10) == FAIL)
	mch_exit(0);
    entry = ((estack_T *)exestack.ga_data) + exestack.ga_len;
    entry->es_type = ETYPE_உச்சி;
    entry->es_name = வெற்று;
    entry->es_lnum = 0;
#ifdef FEAT_EVAL
    entry->es_info.ufunc = வெற்று;
#endif
    ++exestack.ga_len;
}

/*
 * Add an item to the execution stack.
 * Returns the new entry or NULL when out of memory.
 */
    estack_T *
estack_தள்ளு(etype_T type, char_u *name, long lnum)
{
    estack_T *entry;

    // If memory allocation fails then we'll pop more than we push, eventually
    // at the top level it will be OK again.
    எனில் (ga_grow(&exestack, 1) == FAIL)
	திரும்பு வெற்று;

    entry = ((estack_T *)exestack.ga_data) + exestack.ga_len;
    entry->es_type = type;
    entry->es_name = name;
    entry->es_lnum = lnum;
#ifdef FEAT_EVAL
    entry->es_info.ufunc = வெற்று;
#endif
    ++exestack.ga_len;
    திரும்பு entry;
}

#எனில் defined(FEAT_EVAL)
/*
 * Add a user function to the execution stack.
 */
    estack_T *
estack_தள்ளு_ufunc(ufunc_T *ufunc, long lnum)
{
    estack_T *entry = estack_தள்ளு(ETYPE_UFUNC,
	    ufunc->uf_name_exp != வெற்று
				  ? ufunc->uf_name_exp : ufunc->uf_name, lnum);
    எனில் (entry != வெற்று)
	entry->es_info.ufunc = ufunc;
    திரும்பு entry;
}

/*
 * Return TRUE if "ufunc" with "lnum" is already at the top of the exe stack.
 */
    முழு
estack_உச்சி_is_ufunc(ufunc_T *ufunc, long lnum)
{
    estack_T *entry;

    எனில் (exestack.ga_len == 0)
	திரும்பு FALSE;
    entry = ((estack_T *)exestack.ga_data) + exestack.ga_len - 1;
    திரும்பு entry->es_type == ETYPE_UFUNC
	&& STRCMP( entry->es_name, ufunc->uf_name_exp != வெற்று
				    ? ufunc->uf_name_exp : ufunc->uf_name) == 0
	&& entry->es_lnum == lnum;
}
#endif

/*
 * Take an item off of the execution stack and return it.
 */
    estack_T *
estack_எடு(வெறுமை)
{
    எனில் (exestack.ga_len == 0)
	திரும்பு வெற்று;
    --exestack.ga_len;
    திரும்பு ((estack_T *)exestack.ga_data) + exestack.ga_len;
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
    முழு		idx;
    etype_T	last_type = ETYPE_SCRIPT;
#endif

    entry = ((estack_T *)exestack.ga_data) + exestack.ga_len - 1;
#ifdef FEAT_EVAL
    எனில் (which == ESTACK_SFILE && entry->es_type != ETYPE_UFUNC)
#endif
    {
	எனில் (entry->es_name == வெற்று)
	    திரும்பு வெற்று;
	திரும்பு vim_strsave(entry->es_name);
    }
#ifdef FEAT_EVAL
    // expand('<sfile>') works in a function for backwards compatibility, but
    // may give an unexpected result.  Disallow it in Vim 9 script.
    எனில் (which == ESTACK_SFILE && in_vim9script())
    {
	முழு  save_emsg_off = emsg_off;

	எனில் (emsg_off == 1)
	    // f_expand() silences errors but we do want this one
	    emsg_off = 0;
	emsg(_(e_cannot_expand_sfile_in_vim9_function));
	emsg_off = save_emsg_off;
	திரும்பு வெற்று;
    }

    // If evaluated in a function or autocommand, return the path of the script
    // where it is defined, at script level the current script path is returned
    // instead.
    எனில் (which == ESTACK_SCRIPT)
    {
	// Walk the stack backwards, starting from the current frame.
	க்கு (idx = exestack.ga_len - 1; idx >= 0; --idx, --entry)
	{
	    எனில் (entry->es_type == ETYPE_UFUNC || entry->es_type == ETYPE_AUCMD)
	    {
		sctx_T *def_ctx = entry->es_type == ETYPE_UFUNC
			      ? &entry->es_info.ufunc->uf_script_ctx
			      : entry->es_info.aucmd != வெற்று
				  ? acp_script_ctx(entry->es_info.aucmd) : வெற்று;

		திரும்பு def_ctx != வெற்று && def_ctx->sc_sid > 0
			   ? vim_strsave(SCRIPT_ITEM(def_ctx->sc_sid)->sn_name)
			   : வெற்று;
	    }
	    இல்லையெனில் எனில் (entry->es_type == ETYPE_SCRIPT)
		திரும்பு vim_strsave(entry->es_name);
	}
	திரும்பு வெற்று;
    }

    // Give information about each stack entry up to the root.
    // For a function we compose the call stack, as it was done in the past:
    //   "function One[123]..Two[456]..Three"
    ga_init2(&ga, அளவு(எழுத்து), 100);
    க்கு (idx = 0; idx < exestack.ga_len; ++idx)
    {
	entry = ((estack_T *)exestack.ga_data) + idx;
	எனில் (entry->es_name != வெற்று)
	{
	    long	lnum = 0;
	    size_t	added = 0;
	    string_T	type_name = {(char_u *)"", 0};
	    string_T	class_name = {(char_u *)"", 0};
	    string_T	es_name = {entry->es_name, STRLEN(entry->es_name)};

	    எனில் (entry->es_type != last_type)
	    {
		தேர்வு (entry->es_type)
		{
		    நிலை ETYPE_SCRIPT:
			STR_LITERAL_SET(type_name, "script ");
			நிறுத்து;
		    நிலை ETYPE_UFUNC:
			STR_LITERAL_SET(type_name, "function ");
			நிறுத்து;
		    இயல்பு:
			நிறுத்து;
		}
		last_type = entry->es_type;
	    }
	    எனில் (entry->es_type == ETYPE_UFUNC && entry->es_info.ufunc->uf_class != வெற்று)
	    {
		class_name.string = entry->es_info.ufunc->uf_class->class_name.string;
		class_name.length = entry->es_info.ufunc->uf_class->class_name.length;
	    }
	    எனில் (idx == exestack.ga_len - 1)
		lnum = which == ESTACK_அடுக்கு ? SOURCING_LNUM : 0;
	    இல்லையெனில்
		lnum = entry->es_lnum;

	    len = es_name.length + type_name.length + class_name.length + 26;
	    எனில் (ga_grow(&ga, (முழு)len) == FAIL)
		நிறுத்து;
	    ga_concat_len(&ga, type_name.string, type_name.length);
	    // For class methods prepend "<class name>." to the function name.
	    எனில் (*class_name.string != NUL)
	    {
		added = vim_snprintf_safelen(
		    (எழுத்து *)ga.ga_data + ga.ga_len,
		    ga.ga_maxlen - ga.ga_len,
		    "<SNR>%d_%s.",
		    entry->es_info.ufunc->uf_script_ctx.sc_sid,
		    class_name.string);

		ga.ga_len += (முழு)added;
	    }
	    ga_concat_len(&ga, es_name.string, es_name.length);
	    // For the bottom entry of <sfile>: do not add the line number, it is used in
	    // <slnum>.  Also leave it out when the number is not set.
	    எனில் (lnum != 0)
	    {
		added = vim_snprintf_safelen(
		    (எழுத்து *)ga.ga_data + ga.ga_len,
		    ga.ga_maxlen - ga.ga_len,
		    "[%ld]",
		    lnum);

		ga.ga_len += (முழு)added;
	    }
	    எனில் (idx != exestack.ga_len - 1)
		GA_CONCAT_LITERAL(&ga, "..");
	}
    }

    // Only NUL-terminate when not returning NULL.
    எனில் (ga.ga_data != வெற்று)
	ga_append(&ga, NUL);
    திரும்பு (char_u *)ga.ga_data;
#endif
}

#ifdef FEAT_EVAL
    நிலையான வெறுமை
stacktrace_தள்ளு_item(
	list_T		*l,
	ufunc_T		*fp,
	char_u		*event,
	linenr_T	lnum,
	char_u		*filepath)
{
    dict_T	*d;
    typval_T	tv;

    d = dict_alloc_lock(VAR_FIXED);
    எனில் (d == வெற்று)
	திரும்பு;

    tv.v_type = VAR_DICT;
    tv.v_lock = VAR_LOCKED;
    tv.vval.v_dict = d;

    எனில் (fp != வெற்று)
	dict_add_func(d, "funcref", fp);
    எனில் (event != வெற்று)
	dict_add_string(d, "event", event);
    dict_add_எண்(d, "lnum", lnum);
    dict_add_string(d, "filepath", filepath);

    list_append_tv(l, &tv);
}

/*
 * Create the stacktrace from exestack.
 */
    list_T *
stacktrace_create(வெறுமை)
{
    list_T	*l;
    முழு		i;

    l = list_alloc();
    எனில் (l == வெற்று)
	திரும்பு வெற்று;

    க்கு (i = 0; i < exestack.ga_len; ++i)
    {
	estack_T *entry = &((estack_T *)exestack.ga_data)[i];
	linenr_T lnum = entry->es_lnum;

	எனில் (entry->es_type == ETYPE_SCRIPT)
	    stacktrace_தள்ளு_item(l, வெற்று, வெற்று, lnum, entry->es_name);
	இல்லையெனில் எனில் (entry->es_type == ETYPE_UFUNC)
	{
	    ufunc_T *fp = entry->es_info.ufunc;
	    sctx_T sctx = fp->uf_script_ctx;
	    char_u *filepath = sctx.sc_sid > 0 ?
				   get_scriptname(sctx.sc_sid) : (char_u *)"";

	    lnum += sctx.sc_lnum;
	    stacktrace_தள்ளு_item(l, fp, வெற்று, lnum, filepath);
	}
	இல்லையெனில் எனில் (entry->es_type == ETYPE_AUCMD)
	{
	    // The autocmd may not have a matching pattern yet, in which case
	    // es_info.aucmd is still NULL.
	    sctx_T *sctx = entry->es_info.aucmd != வெற்று
			       ? acp_script_ctx(entry->es_info.aucmd) : வெற்று;
	    char_u *filepath = sctx != வெற்று && sctx->sc_sid > 0 ?
				   get_scriptname(sctx->sc_sid) : (char_u *)"";

	    எனில் (sctx != வெற்று)
		lnum += sctx->sc_lnum;
	    stacktrace_தள்ளு_item(l, வெற்று, entry->es_name, lnum, filepath);
	}
    }
    திரும்பு l;
}

/*
 * getstacktrace() function
 */
    வெறுமை
f_getstacktrace(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv_list_set(rettv, stacktrace_create());
}
#endif

/*
 * Get DIP_ flags from the [where] argument of a :runtime command.
 * "*argp" is advanced to after the [where] argument if it is found.
 */
    நிலையான முழு
get_runtime_cmd_flags(char_u **argp, size_t where_len)
{
    char_u *arg = *argp;

    எனில் (where_len == 0)
	திரும்பு 0;

    எனில் (STRNCMP(arg, "START", where_len) == 0)
    {
	*argp = skipwhite(arg + where_len);
	திரும்பு DIP_START + DIP_NORTP;
    }
    எனில் (STRNCMP(arg, "OPT", where_len) == 0)
    {
	*argp = skipwhite(arg + where_len);
	திரும்பு DIP_OPT + DIP_NORTP;
    }
    எனில் (STRNCMP(arg, "PACK", where_len) == 0)
    {
	*argp = skipwhite(arg + where_len);
	திரும்பு DIP_START + DIP_OPT + DIP_NORTP;
    }
    எனில் (STRNCMP(arg, "ALL", where_len) == 0)
    {
	*argp = skipwhite(arg + where_len);
	திரும்பு DIP_START + DIP_OPT;
    }

    திரும்பு 0;
}

/*
 * ":runtime [where] {name}"
 */
    வெறுமை
ex_runtime(exarg_T *eap)
{
    char_u  *arg = eap->arg;
    முழு	    flags = eap->forceit ? DIP_ALL : 0;
    char_u  *p = skiptowhite(arg);
    flags += get_runtime_cmd_flags(&arg, p - arg);
    source_runtime(arg, flags);
}

நிலையான முழு runtime_expand_flags;

/*
 * Set the completion context for the :runtime command.
 */
    வெறுமை
set_context_in_runtime_cmd(expand_T *xp, char_u *arg)
{
    char_u  *p = skiptowhite(arg);
    runtime_expand_flags
	= *p != NUL ? get_runtime_cmd_flags(&arg, p - arg) : 0;
    // Skip to the last argument.
    போது (*(p = skiptowhite_esc(arg)) != NUL)
    {
	எனில் (runtime_expand_flags == 0)
	    // When there are multiple arguments and [where] is not specified,
	    // use an unrelated non-zero flag to avoid expanding [where].
	    runtime_expand_flags = DIP_ALL;
	arg = skipwhite(p);
    }
    xp->xp_context = EXPAND_RUNTIME;
    xp->xp_pattern = arg;
}

    நிலையான வெறுமை
source_callback(char_u *fname, வெறுமை *cookie)
{
    (வெறுமை)do_source(fname, FALSE, DOSO_NONE, cookie);
}

#ifdef FEAT_EVAL
/*
 * Find an already loaded script "name".
 * If found returns its script ID.  If not found returns -1.
 */
    முழு
find_script_by_name(char_u *name)
{
    முழு		    sid;
    scriptitem_T    *si;

    க்கு (sid = script_பொருட்கள்.ga_len; sid > 0; --sid)
    {
	// We used to check inode here, but that doesn't work:
	// - If a script is edited and written, it may get a different
	//   inode number, even though to the user it is the same script.
	// - If a script is deleted and another script is written, with a
	//   different name, the inode may be re-used.
	si = SCRIPT_ITEM(sid);
	எனில் (si->sn_name != வெற்று && fnamecmp(si->sn_name, name) == 0)
	    திரும்பு sid;
    }
    திரும்பு -1;
}

/*
 * Add a new scriptitem with all items initialized.
 * When running out of memory "error" is set to FAIL.
 * Returns the script ID.
 */
    நிலையான முழு
get_new_scriptitem(முழு *error)
{
    நிலையான scid_T   last_current_SID = 0;
    முழு		    sid = ++last_current_SID;
    scriptitem_T    *si = வெற்று;

    எனில் (ga_grow(&script_பொருட்கள், (முழு)(sid - script_பொருட்கள்.ga_len)) == FAIL)
    {
	*error = FAIL;
	திரும்பு sid;
    }
    போது (script_பொருட்கள்.ga_len < sid)
    {
	si = ALLOC_CLEAR_ONE(scriptitem_T);
	எனில் (si == வெற்று)
	{
	    *error = FAIL;
	    திரும்பு sid;
	}
	++script_பொருட்கள்.ga_len;
	SCRIPT_ITEM(script_பொருட்கள்.ga_len) = si;
	si->sn_name = வெற்று;
	si->sn_version = 1;

	// Allocate the local script variables to use for this script.
	new_script_vars(script_பொருட்கள்.ga_len);
	ga_init2(&si->sn_var_vals, அளவு(svar_T), 10);
	hash_init(&si->sn_all_vars.dv_hashtab);
	ga_init2(&si->sn_imports, அளவு(imported_T), 10);
	ga_init2(&si->sn_type_list, அளவு(type_T), 10);
# ifdef FEAT_PROFILE
	si->sn_prof_on = FALSE;
# endif
    }

    // "si" can't be NULL, check only to avoid a compiler warning
    எனில் (si != வெற்று)
	// Used to check script variable index is still valid.
	si->sn_script_seq = current_sctx.sc_seq;

    திரும்பு sid;
}

    முழு
get_new_scriptitem_for_fname(முழு *error, char_u *fname)
{
    முழு sid = get_new_scriptitem(error);

    எனில் (*error == OK)
    {
	scriptitem_T *si = SCRIPT_ITEM(sid);

	si->sn_name = vim_strsave(fname);
	si->sn_state = SN_STATE_NOT_LOADED;
    }
    திரும்பு sid;
}

/*
 * If the script for "sid" is a symlink and "sn_source_sid" is not set
 * then initialize it. A new script_item is created if needed.
 */
    வெறுமை
check_script_symlink(முழு sid)
{
    scriptitem_T *si = SCRIPT_ITEM(sid);
    எனில் (si->sn_syml_checked || si->sn_sourced_sid > 0)
	திரும்பு;
    si->sn_syml_checked = TRUE;

    // If fname is a symbolic link, create an script_item for the real file.

    char_u *real_fname = fix_fname(si->sn_name);
    எனில் (real_fname != வெற்று && STRCMP(real_fname, si->sn_name) != 0)
    {
	முழு real_sid = find_script_by_name(real_fname);
	முழு error2 = OK;
	முழு new_sid = FALSE;
	எனில் (real_sid < 0)
	{
	    real_sid = get_new_scriptitem_for_fname(&error2, real_fname);
	    new_sid = TRUE;
	}
	எனில் (error2 == OK)
	{
	    si = SCRIPT_ITEM(sid);
	    si->sn_sourced_sid = real_sid;
	    எனில் (new_sid)
	    {
		SCRIPT_ITEM(real_sid)->sn_import_autoload
						    = si->sn_import_autoload;
		எனில் (si->sn_autoload_prefix != வெற்று)
		    SCRIPT_ITEM(real_sid)->sn_autoload_prefix =
					vim_strsave(si->sn_autoload_prefix);
	    }
	}
    }
    vim_free(real_fname);
}

    நிலையான வெறுமை
find_script_callback(char_u *fname, வெறுமை *cookie)
{
    முழு sid;
    முழு error = OK;
    முழு *ret_sid = cookie;

    sid = find_script_by_name(fname);
    எனில் (sid < 0)
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
    முழு
do_in_path(
    char_u	*path,
    எழுத்து	*prefix,
    char_u	*name,
    முழு		flags,
    வெறுமை	(*callback)(char_u *fname, வெறுமை *ck),
    வெறுமை	*cookie)
{
    char_u	*rtp;
    string_T	buf;
    char_u	*rtp_copy;
    முழு		இலக்கம்_files;
    char_u	**files;
    முழு		i;
    முழு		did_one = FALSE;
#ifdef AMIGA
    கட்டமைப்பு Process	*proc = (கட்டமைப்பு Process *)FindTask(0L);
    APTR		save_winptr = proc->pr_WindowPtr;

    // Avoid a requester here for a volume that doesn't exist.
    proc->pr_WindowPtr = (APTR)-1L;
#endif

    // Make a copy of 'runtimepath'.  Invoking the callback may change the
    // value.
    rtp_copy = vim_strsave(path);
    buf.string = alloc(MAXPATHL);
    எனில் (buf.string != வெற்று && rtp_copy != வெற்று)
    {
	size_t	prefixlen = 0;
	size_t	namelen = 0;

	எனில் (name != வெற்று)
	{
	    prefixlen = STRLEN(prefix);
	    namelen = STRLEN(name);

	    எனில் (p_verbose > 10)
	    {
		verbose_enter();
		எனில் (*prefix != NUL)
		    smsg(_("Searching for \"%s\" under \"%s\" in \"%s\""),
					       (எழுத்து *)name, prefix, (எழுத்து *)path);
		இல்லையெனில்
		    smsg(_("Searching for \"%s\" in \"%s\""),
						       (எழுத்து *)name, (எழுத்து *)path);
		verbose_leave();
	    }
	}

	// Loop over all entries in 'runtimepath'.
	rtp = rtp_copy;
	போது (*rtp != NUL && ((flags & DIP_ALL) || !did_one))
	{
	    // Copy the path from 'runtimepath' to buf[].
	    buf.length = (size_t)copy_option_part(&rtp, buf.string, MAXPATHL, ",");

	    // Skip after or non-after directories.
	    எனில் (flags & (DIP_NOAFTER | DIP_AFTER))
	    {
		முழு is_after = buf.length >= 5
		    && STRCMP(buf.string + buf.length - 5, "after") == 0;

		எனில் ((is_after && (flags & DIP_NOAFTER))
			|| (!is_after && (flags & DIP_AFTER)))
		    தொடர்;
	    }

	    எனில் (name == வெற்று)
	    {
		(*callback)(buf.string, (வெறுமை *)&cookie);
		எனில் (!did_one)
		    did_one = (cookie == வெற்று);
	    }
	    இல்லையெனில் எனில் (buf.length + 2 + prefixlen + namelen < MAXPATHL)
	    {
		char_u	*np;
		char_u	*tail;

		எனில் (*buf.string != NUL
		    && !after_pathsep(buf.string, buf.string + buf.length))
		{
		    STRCPY(buf.string + buf.length, PATHSEPSTR);
		    buf.length += அளவு(PATHSEP);
		}
		STRCPY(buf.string + buf.length, prefix);
		buf.length += prefixlen;
		tail = buf.string + buf.length;

		// Loop over all patterns in "name"
		np = name;
		போது (*np != NUL && ((flags & DIP_ALL) || !did_one))
		{
		    // Append the pattern from "name" to buf[].
		    copy_option_part(&np, tail, (முழு)(MAXPATHL - (tail - buf.string)),
								       "\t ");

		    எனில் (p_verbose > 10)
		    {
			verbose_enter();
			smsg(_("Searching for \"%s\""), buf.string);
			verbose_leave();
		    }

		    // Expand wildcards, invoke the callback for each match.
		    எனில் (gen_expand_wildcards(1, &buf.string, &இலக்கம்_files, &files,
				  (flags & DIP_DIR) ? EW_DIR : EW_FILE) == OK)
		    {
			க்கு (i = 0; i < இலக்கம்_files; ++i)
			{
			    (*callback)(files[i], cookie);
			    did_one = TRUE;
			    எனில் (!(flags & DIP_ALL))
				நிறுத்து;
			}
			FreeWild(இலக்கம்_files, files);
		    }
		}
	    }
	}
    }
    vim_free(buf.string);
    vim_free(rtp_copy);
    எனில் (!did_one && name != வெற்று)
    {
	எழுத்து *basepath = path == p_rtp ? "runtimepath" : "packpath";

	எனில் (flags & DIP_ERR)
	    semsg(_(e_directory_not_found_in_str_str), basepath, name);
	இல்லையெனில் எனில் (p_verbose > 0)
	{
	    verbose_enter();
	    smsg(_("not found in '%s': \"%s\""), basepath, name);
	    verbose_leave();
	}
    }

#ifdef AMIGA
    proc->pr_WindowPtr = save_winptr;
#endif

    திரும்பு did_one ? OK : FAIL;
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
    நிலையான முழு
do_in_path_and_pp(
    char_u	*path,
    char_u	*name,
    முழு		flags,
    வெறுமை	(*callback)(char_u *fname, வெறுமை *ck),
    வெறுமை	*cookie)
{
    முழு		done = FAIL;

    எனில் ((flags & DIP_NORTP) == 0)
	done = do_in_path(path, "", name, flags, callback, cookie);

    எனில் ((done == FAIL || (flags & DIP_ALL)) && (flags & DIP_START))
	done = do_in_path(p_pp, "pack/*/start/*/", name, flags, callback,
								       cookie);

    எனில் ((done == FAIL || (flags & DIP_ALL)) && (flags & DIP_OPT))
	done = do_in_path(p_pp, "pack/*/opt/*/", name, flags, callback,
								       cookie);

    திரும்பு done;
}

/*
 * Just like do_in_path_and_pp(), using 'runtimepath' for "path".
 */
    முழு
do_in_runtimepath(
    char_u	*name,
    முழு		flags,
    வெறுமை	(*callback)(char_u *fname, வெறுமை *ck),
    வெறுமை	*cookie)
{
    திரும்பு do_in_path_and_pp(p_rtp, name, flags, callback, cookie);
}

/*
 * Source the file "name" from all directories in 'runtimepath'.
 * "name" can contain wildcards.
 * When "flags" has DIP_ALL: source all files, otherwise only the first one.
 *
 * return FAIL when no file could be sourced, OK otherwise.
 */
    முழு
source_runtime(char_u *name, முழு flags)
{
    திரும்பு source_in_path(p_rtp, name, flags, வெற்று);
}

/*
 * Just like source_runtime(), but use "path" instead of 'runtimepath'
 * and return the script ID in "ret_sid".
 */
    முழு
source_in_path(char_u *path, char_u *name, முழு flags, முழு *ret_sid)
{
    திரும்பு do_in_path_and_pp(path, name, flags, source_callback, ret_sid);
}

#எனில் defined(FEAT_EVAL)

/*
 * Find "name" in 'runtimepath'. If found a new scriptitem is created for it
 * and it's script ID is returned.
 * If not found returns -1.
 */
    முழு
find_script_in_rtp(char_u *name)
{
    முழு sid = -1;

    (வெறுமை)do_in_path_and_pp(p_rtp, name, DIP_START | DIP_NOAFTER,
						   find_script_callback, &sid);
    திரும்பு sid;
}

/*
 * Expand wildcards in "pat" and invoke do_source() for each match.
 */
    நிலையான வெறுமை
source_all_matches(char_u *pat)
{
    முழு	    இலக்கம்_files;
    char_u  **files;
    முழு	    i;

    எனில் (gen_expand_wildcards(1, &pat, &இலக்கம்_files, &files, EW_FILE) != OK)
	திரும்பு;

    க்கு (i = 0; i < இலக்கம்_files; ++i)
	(வெறுமை)do_source(files[i], FALSE, DOSO_NONE, வெற்று);
    FreeWild(இலக்கம்_files, files);
}

/*
 * Add the package directory to 'runtimepath'.
 */
    நிலையான முழு
add_pack_dir_to_rtp(char_u *fname)
{
    char_u  *p4, *p3, *p2, *p1, *p;
    char_u  *entry;
    char_u  *insp = வெற்று;
    முழு	    c;
    string_T  new_rtp;
    size_t  keep;
    size_t  p_rtp_len;
    string_T  afterdir = {வெற்று, 0};
    char_u  *after_insp = வெற்று;
    char_u  *ffname = வெற்று;
    size_t  fname_len;
    string_T	buf = {வெற்று, 0};
    முழு	    retval = FAIL;

    p4 = p3 = p2 = p1 = get_past_head(fname);
    க்கு (p = p1; *p; MB_PTR_ADV(p))
	எனில் (vim_ispathsep_nocolon(*p))
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
    எனில் (ffname == வெற்று)
	திரும்பு FAIL;

    // Find "ffname" in "p_rtp", ignoring '/' vs '\' differences.
    // Also stop at the first "after" directory.
    fname_len = STRLEN(ffname);
    buf.string = alloc(MAXPATHL);
    எனில் (buf.string == வெற்று)
	செல் theend;

    p_rtp_len = 0;
    க்கு (entry = p_rtp; *entry != NUL; )
    {
	char_u *cur_entry = entry;

	buf.length = (size_t)copy_option_part(&entry, buf.string, MAXPATHL, ",");

	// keep track of p_rtp length as we go to make the STRLEN() below have less work to do
	p_rtp_len += (*(cur_entry + buf.length) == ',') ? buf.length + 1 : buf.length;

	எனில் ((p = (char_u *)strstr((எழுத்து *)buf.string, "after")) != வெற்று
		&& p > buf.string
		&& vim_ispathsep(p[-1])
		&& (vim_ispathsep(p[5]) || p[5] == NUL || p[5] == ','))
	{
	    எனில் (insp == வெற்று)
		// Did not find "ffname" before the first "after" directory,
		// insert it before this entry.
		insp = cur_entry;
	    after_insp = cur_entry;
	    நிறுத்து;
	}

	எனில் (insp == வெற்று)
	{
	    char_u  *rtp_ffname;
	    முழு	    match;

	    எனில் (*buf.string != NUL && !after_pathsep(buf.string, buf.string + buf.length))
		STRCPY(buf.string + buf.length, PATHSEPSTR);
	    rtp_ffname = fix_fname(buf.string);
	    எனில் (rtp_ffname == வெற்று)
		செல் theend;
	    match = vim_fnamencmp(rtp_ffname, ffname, fname_len) == 0;
	    vim_free(rtp_ffname);
	    எனில் (match)
		// Insert "ffname" after this entry (and comma).
		insp = entry;
	}
    }

    // finish measuring the length of p_rtp
    p_rtp_len += STRLEN(p_rtp + p_rtp_len);
    எனில் (insp == வெற்று)
	// Both "fname" and "after" not found, append at the end.
	insp = p_rtp + p_rtp_len;

    // check if rtp/pack/name/start/name/after exists
    fname_len = STRLEN(fname);
    concat_fnames(fname, fname_len, (char_u *)"after", STRLEN_LITERAL("after"), TRUE, &afterdir);
    எனில் (afterdir.string == வெற்று || !mch_isdir(afterdir.string))
	afterdir.length = 0;

    new_rtp.string = alloc(p_rtp_len + fname_len + afterdir.length + 3); // add two for commas and one for NUL
    எனில் (new_rtp.string == வெற்று)
	செல் theend;

    // We now have 'rtp' parts: {keep}{keep_after}{rest}.
    // Create new_rtp, first: {keep},{fname}
    keep = (size_t)(insp - p_rtp);
    mch_memmove(new_rtp.string, p_rtp, keep);
    new_rtp.length = keep;
    எனில் (*insp == NUL)
	new_rtp.string[new_rtp.length++] = ',';  // add comma before
    mch_memmove(new_rtp.string + new_rtp.length, fname, fname_len);
    new_rtp.length += fname_len;
    எனில் (*insp != NUL)
	new_rtp.string[new_rtp.length++] = ',';  // add comma after

    எனில் (afterdir.length > 0 && after_insp != வெற்று)
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

    எனில் (p_rtp[keep] != NUL)
    {
	size_t	append_len = p_rtp_len - keep;

	// Append rest: {keep},{fname}{keep_after},{afterdir}{rest}
	mch_memmove(new_rtp.string + new_rtp.length, p_rtp + keep, append_len + 1);	// add one for NUL
	new_rtp.length += append_len;
    }
    இல்லையெனில்
	new_rtp.string[new_rtp.length] = NUL;

    எனில் (afterdir.length > 0 && after_insp == வெற்று)
    {
	// Append afterdir when "after" was not found:
	// {keep},{fname}{rest},{afterdir}
	new_rtp.string[new_rtp.length++] = ',';
	STRCPY(new_rtp.string + new_rtp.length, afterdir.string);
    }

    set_option_மதிப்பு_give_err((char_u *)"rtp", 0L, new_rtp.string, 0);
    vim_free(new_rtp.string);
    retval = OK;

theend:
    vim_free(buf.string);
    vim_free(ffname);
    vim_free(afterdir.string);
    திரும்பு retval;
}

/*
 * Load scripts in "plugin" and "ftdetect" directories of the package.
 */
    நிலையான முழு
load_pack_plugin(char_u *fname)
{
    நிலையான எழுத்து *plugpat = "%s/plugin/**/*.vim";
    நிலையான எழுத்து *ftpat = "%s/ftdetect/*.vim";
    முழு		len;
    char_u	*ffname = fix_fname(fname);
    char_u	*pat = வெற்று;
    முழு		retval = FAIL;

    எனில் (ffname == வெற்று)
	திரும்பு FAIL;
    len = (முழு)STRLEN(ffname) + (முழு)STRLEN(ftpat);
    pat = alloc(len);
    எனில் (pat == வெற்று)
	செல் theend;
    vim_snprintf((எழுத்து *)pat, len, plugpat, ffname);
    source_all_matches(pat);

    {
	char_u *cmd = vim_strsave((char_u *)"g:did_load_filetypes");

	// If runtime/filetype.vim wasn't loaded yet, the scripts will be
	// found when it loads.
	எனில் (cmd != வெற்று && eval_to_எண்(cmd, FALSE) > 0)
	{
	    do_cmdline_cmd((char_u *)"augroup filetypedetect");
	    vim_snprintf((எழுத்து *)pat, len, ftpat, ffname);
	    source_all_matches(pat);
	    do_cmdline_cmd((char_u *)"augroup END");
	}
	vim_free(cmd);
    }
    vim_free(pat);
    retval = OK;

theend:
    vim_free(ffname);
    திரும்பு retval;
}

// used for "cookie" of add_pack_plugin()
நிலையான முழு APP_ADD_DIR;
நிலையான முழு APP_LOAD;
நிலையான முழு APP_BOTH;

    நிலையான வெறுமை
add_pack_plugin(char_u *fname, வெறுமை *cookie)
{
    எனில் (cookie != &APP_LOAD)
    {
	char_u	*buf = alloc(MAXPATHL);
	char_u	*p;
	முழு	found = FALSE;

	எனில் (buf == வெற்று)
	    திரும்பு;
	p = p_rtp;
	போது (*p != NUL)
	{
	    copy_option_part(&p, buf, MAXPATHL, ",");
	    எனில் (pathcmp((எழுத்து *)buf, (எழுத்து *)fname, -1) == 0)
	    {
		found = TRUE;
		நிறுத்து;
	    }
	}
	vim_free(buf);
	எனில் (!found)
	    // directory is not yet in 'runtimepath', add it
	    எனில் (add_pack_dir_to_rtp(fname) == FAIL)
		திரும்பு;
    }

    எனில் (cookie != &APP_ADD_DIR)
	load_pack_plugin(fname);
}

/*
 * Add all packages in the "start" directory to 'runtimepath'.
 */
    வெறுமை
add_pack_start_dirs(வெறுமை)
{
    do_in_path(p_pp, "", (char_u *)"pack/*/start/*", DIP_ALL + DIP_DIR,
					       add_pack_plugin, &APP_ADD_DIR);
}

/*
 * Load plugins from all packages in the "start" directory.
 */
    வெறுமை
load_start_packages(வெறுமை)
{
    did_source_packages = TRUE;
    do_in_path(p_pp, "", (char_u *)"pack/*/start/*", DIP_ALL + DIP_DIR,
						  add_pack_plugin, &APP_LOAD);
}

/*
 * ":packloadall"
 * Find plugins in the package directories and source them.
 */
    வெறுமை
ex_packloadall(exarg_T *eap)
{
    எனில் (!did_source_packages || eap->forceit)
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
    வெறுமை
ex_packadd(exarg_T *eap)
{
    நிலையான எழுத்து *plugpat = "pack/*/%s/%s";
    முழு		len;
    எழுத்து	*pat;
    முழு		round;
    முழு		res = OK;

    // Round 1: use "start", round 2: use "opt".
    க்கு (round = 1; round <= 2; ++round)
    {
	// Only look under "start" when loading packages wasn't done yet.
	எனில் (round == 1 && did_source_packages)
	    தொடர்;

	len = (முழு)STRLEN(plugpat) + (முழு)STRLEN(eap->arg) + 5;
	pat = alloc(len);
	எனில் (pat == வெற்று)
	    திரும்பு;
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
    வெறுமை
remove_duplicates(garray_T *gap)
{
    முழு	    i;
    முழு	    j;
    char_u  **fnames = (char_u **)gap->ga_data;

    sort_strings(fnames, gap->ga_len);
    க்கு (i = gap->ga_len - 1; i > 0; --i)
	எனில் (fnamecmp(fnames[i - 1], fnames[i]) == 0)
	{
	    vim_free(fnames[i]);
	    க்கு (j = i + 1; j < gap->ga_len; ++j)
		fnames[j - 1] = fnames[j];
	    --gap->ga_len;
	}
}

    நிலையான வெறுமை
ExpandRTDir_int(
    char_u	*pat,
    size_t	pat_len,
    முழு		flags,
    முழு		keep_ext,
    garray_T	*gap,
    எழுத்து	*dirnames[])
{
    க்கு (முழு i = 0; dirnames[i] != வெற்று; ++i)
    {
	மாறிலி size_t	buf_len = STRLEN(dirnames[i]) + pat_len + 64;
	எழுத்து		*buf = alloc(buf_len);
	எனில் (buf == வெற்று)
	{
	    ga_clear_strings(gap);
	    திரும்பு;
	}
	முழு		glob_flags = 0;
	முழு		expand_dirs = FALSE;

	// Build base pattern
	vim_snprintf(buf, buf_len, "%s%s%s%s",
		     *dirnames[i] ? dirnames[i] : "", *dirnames[i] ? "/" : "",
		     pat, "*.vim");

expand:
	எனில் ((flags & DIP_NORTP) == 0)
	    globpath(p_rtp, (char_u *)buf, gap, glob_flags, expand_dirs);

	எனில் (flags & DIP_START)
	{
	    // Build complete search path: pack/*/start/*/dirnames[i]/pat*.vim
	    vim_snprintf(buf, buf_len, "pack/*/start/*/%s%s%s%s",
			 *dirnames[i] ? dirnames[i] : "",
			 *dirnames[i] ? "/" : "",
			 pat,
			 expand_dirs ? "*" : "*.vim");
	    globpath(p_pp, (char_u *)buf, gap, glob_flags, expand_dirs);
	}

	எனில் (flags & DIP_OPT)
	{
	    // Build complete search path: pack/*/opt/*/dirnames[i]/pat*.vim
	    vim_snprintf(buf, buf_len, "pack/*/opt/*/%s%s%s%s",
			 *dirnames[i] ? dirnames[i] : "",
			 *dirnames[i] ? "/" : "", pat,
			 expand_dirs ? "*" : "*.vim");
	    globpath(p_pp, (char_u *)buf, gap, glob_flags, expand_dirs);
	}

	// Second round for directories
	எனில் (*dirnames[i] == NUL && !expand_dirs)
	{
	    // expand dir names in another round
	    vim_snprintf(buf, buf_len, "%s*", pat);
	    glob_flags = WILD_ADD_SLASH;
	    expand_dirs = TRUE;
	    செல் expand;
	}

	vim_free(buf);
    }

    முழு pat_pathsep_cnt = 0;
    க்கு (size_t i = 0; i < pat_len; ++i)
    {
	எனில் (vim_ispathsep(pat[i]))
	    ++pat_pathsep_cnt;
    }

    க்கு (முழு i = 0; i < gap->ga_len; ++i)
    {
	char_u *match = ((char_u **)gap->ga_data)[i];
	char_u *s = match;
	char_u *e = s + STRLEN(s);
	எனில் (e - 4 > s && !keep_ext && STRNICMP(e - 4, ".vim", 4) == 0)
	{
	    e -= 4;
	    *e = NUL;
	}

	முழு match_pathsep_cnt = (e > s && e[-1] == '/') ? -1 : 0;
	க்கு (s = e; s > match; MB_PTR_BACK(match, s))
	{
	    எனில் (s < match || (vim_ispathsep(*s)
				     && ++match_pathsep_cnt > pat_pathsep_cnt))
		நிறுத்து;
	}
	++s;
	எனில் (s != match)
	    mch_memmove(match, s, e - s + 1);
    }

    எனில் (gap->ga_len == 0)
	திரும்பு;

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
    முழு
ExpandRTDir(
    char_u	*pat,
    முழு		flags,
    முழு		*இலக்கம்_file,
    char_u	***file,
    எழுத்து	*dirnames[])
{
    *இலக்கம்_file = 0;
    *file = வெற்று;

    garray_T	ga;
    ga_init2(&ga, அளவு(எழுத்து *), 10);

    ExpandRTDir_int(pat, STRLEN(pat), flags, FALSE, &ga, dirnames);

    எனில் (ga.ga_len == 0)
	திரும்பு FAIL;

    *file = ga.ga_data;
    *இலக்கம்_file = ga.ga_len;
    திரும்பு OK;
}

/*
 * Handle command line completion for :runtime command.
 */
    முழு
expand_runtime_cmd(char_u *pat, முழு *இலக்கம்_Matches, char_u ***matches)
{
    *இலக்கம்_Matches = 0;
    *matches = வெற்று;

    garray_T	ga;
    ga_init2(&ga, அளவு(எழுத்து *), 10);

    size_t pat_len = (முழு)STRLEN(pat);
    எழுத்து *dirnames[] = {"", வெற்று};
    ExpandRTDir_int(pat, pat_len, runtime_expand_flags, TRUE, &ga, dirnames);

    // Try to complete values for [where] argument when none was found.
    எனில் (runtime_expand_flags == 0)
    {
	எழுத்து *where_மதிப்புகள்[] = {"START", "OPT", "PACK", "ALL"};
	க்கு (size_t i = 0; i < ARRAY_LENGTH(where_மதிப்புகள்); ++i)
	    எனில் (STRNCMP(pat, where_மதிப்புகள்[i], pat_len) == 0)
	    {
		char_u *p = vim_strsave((char_u *)where_மதிப்புகள்[i]);
		எனில் (p != வெற்று && ga_add_string(&ga, p) == FAIL)
		    vim_free(p);
	    }
    }

    எனில் (ga.ga_len == 0)
	திரும்பு FAIL;

    *matches = ga.ga_data;
    *இலக்கம்_Matches = ga.ga_len;
    திரும்பு OK;
}

/*
 * Expand loadplugin names:
 * 'packpath'/pack/ * /opt/{pat}
 */
    முழு
ExpandPackAddDir(
    char_u	*pat,
    முழு		*இலக்கம்_file,
    char_u	***file)
{
    char_u	*s;
    char_u	*e;
    char_u	*match;
    garray_T	ga;
    முழு		i;
    முழு		pat_len;

    *இலக்கம்_file = 0;
    *file = வெற்று;
    pat_len = (முழு)STRLEN(pat);
    ga_init2(&ga, அளவு(எழுத்து *), 10);

    s = alloc(pat_len + 26);
    எனில் (s == வெற்று)
    {
	ga_clear_strings(&ga);
	திரும்பு FAIL;
    }
    sprintf((எழுத்து *)s, "pack/*/opt/%s*", pat);
    globpath(p_pp, s, &ga, 0, TRUE);
    vim_free(s);

    க்கு (i = 0; i < ga.ga_len; ++i)
    {
	match = ((char_u **)ga.ga_data)[i];
	s = gettail(match);
	e = s + STRLEN(s);
	mch_memmove(match, s, e - s + 1);
    }

    எனில் (ga.ga_len == 0)
	திரும்பு FAIL;

    // Sort and remove duplicates which can happen when specifying multiple
    // directories in dirnames.
    remove_duplicates(&ga);

    *file = ga.ga_data;
    *இலக்கம்_file = ga.ga_len;
    திரும்பு OK;
}

    நிலையான வெறுமை
cmd_source(char_u *fname, exarg_T *eap)
{
    முழு clearvars = FALSE;

    எனில் (*fname != NUL && STRNCMP(fname, "++clear", 7) == 0)
    {
	// ++clear argument is supplied
	clearvars = TRUE;
	fname = fname + 7;
	எனில் (*fname != NUL)
	{
	    semsg(_(e_invalid_argument_str), eap->arg);
	    திரும்பு;
	}
    }

    எனில் (*fname != NUL && eap != வெற்று && eap->addr_மொத்தம் > 0)
    {
	// if a filename is specified to :source, then a range is not allowed
	emsg(_(e_no_range_allowed));
	திரும்பு;
    }

    எனில் (eap != வெற்று && *fname == NUL)
    {
	எனில் (eap->forceit)
	    // a file name is needed to source normal mode commands
	    emsg(_(e_argument_required));
	இல்லையெனில்
	    // source ex commands from the current buffer
	    do_source_ext(வெற்று, FALSE, DOSO_NONE, வெற்று, eap, clearvars);
    }
    இல்லையெனில் எனில் (eap != வெற்று && eap->forceit)
	// ":source!": read Normal mode commands
	// Need to execute the commands directly.  This is required at least
	// for:
	// - ":g" command busy
	// - after ":argdo", ":windo" or ":bufdo"
	// - another command follows
	// - inside a loop
	openscript(fname, global_busy || listcmd_busy || eap->nextcmd != வெற்று
#ifdef FEAT_EVAL
						 || eap->cstack->cs_idx >= 0
#endif
						 );

    // ":source" read ex commands
    இல்லையெனில் எனில் (do_source(fname, FALSE, DOSO_NONE, வெற்று) == FAIL)
	semsg(_(e_cant_open_file_str), fname);
}

/*
 * ":source {fname}"
 */
    வெறுமை
ex_source(exarg_T *eap)
{
#ifdef FEAT_BROWSE
    எனில் (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	char_u *fname = வெற்று;

	fname = do_browse(0, (char_u *)_("Source Vim script"), eap->arg,
				      வெற்று, வெற்று,
				      (char_u *)_(BROWSE_FILTER_MACROS), வெற்று);
	எனில் (fname != வெற்று)
	{
	    cmd_source(fname, eap);
	    vim_free(fname);
	}
    }
    இல்லையெனில்
#endif
	cmd_source(eap->arg, eap);
}

#எனில் defined(FEAT_EVAL)
/*
 * ":options"
 */
    வெறுமை
ex_options(
    exarg_T	*eap UNUSED)
{
    char_u  buf[500];
    முழு	    multi_mods = 0;

    buf[0] = NUL;
    (வெறுமை)add_win_cmd_modifiers(buf, &cmdmod, &multi_mods);

    vim_setenv((char_u *)"OPTWIN_CMD", buf);
    cmd_source((char_u *)SYS_OPTWIN_FILE, வெற்று);
}
#endif

/*
 * ":source" and associated commands.
 */

#எனில் defined(FEAT_EVAL)
/*
 * Return the address holding the next breakpoint line for a source cookie.
 */
    linenr_T *
source_breakpoint(வெறுமை *cookie)
{
    திரும்பு &((source_cookie_T *)cookie)->breakpoint;
}

/*
 * Return the address holding the debug tick for a source cookie.
 */
    முழு *
source_dbg_tick(வெறுமை *cookie)
{
    திரும்பு &((source_cookie_T *)cookie)->dbg_tick;
}

/*
 * Return the nesting level for a source cookie.
 */
    முழு
source_level(வெறுமை *cookie)
{
    திரும்பு ((source_cookie_T *)cookie)->level;
}

/*
 * Return the readahead line. Note that the pointer may become invalid when
 * getting the next line, if it's concatenated with the next one.
 */
    char_u *
source_nextline(வெறுமை *cookie)
{
    திரும்பு ((source_cookie_T *)cookie)->nextline;
}
#endif

#எனில் (defined(MSWIN) && defined(FEAT_CSCOPE)) || defined(HAVE_FD_CLOEXEC)
# define USE_FOPEN_NOINH
/*
 * Special function to open a file without handle inheritance.
 * When possible the handle is closed on exec().
 */
    நிலையான FILE *
fopen_noinh_readbin(எழுத்து *filename)
{
# ifdef MSWIN
    முழு	fd_tmp = mch_open(filename, O_RDONLY | O_இருமம் | O_NOINHERIT, 0);
# இல்லையெனில்
    முழு	fd_tmp = mch_open(filename, O_RDONLY, 0);
# endif

    எனில் (fd_tmp == -1)
	திரும்பு வெற்று;

# ifdef HAVE_FD_CLOEXEC
    {
	முழு fdflags = fcntl(fd_tmp, F_GETFD);
	எனில் (fdflags >= 0 && (fdflags & FD_CLOEXEC) == 0)
	    (வெறுமை)fcntl(fd_tmp, F_SETFD, fdflags | FD_CLOEXEC);
    }
# endif

    திரும்பு fdopen(fd_tmp, READBIN);
}
#endif

/*
 * Initialization for sourcing lines from the current buffer. Reads all the
 * lines from the buffer and stores it in the cookie grow array.
 * Returns a pointer to the name ":source buffer=<n>" on success and NULL on
 * failure.
 */
    நிலையான char_u *
do_source_buffer_init(source_cookie_T *sp, exarg_T *eap)
{
    linenr_T	curr_lnum;
    char_u	*வரி = வெற்று;
    char_u	*fname;

    எனில் (curbuf == வெற்று)
	திரும்பு வெற்று;

    // Use ":source buffer=<num>" as the script name
    எனில் (curbuf->b_ffname != வெற்று)
	fname = vim_strsave(curbuf->b_ffname);
    இல்லையெனில்
    {
	vim_snprintf((எழுத்து *)IObuff, IOSIZE, ":source buffer=%d", curbuf->b_fnum);
	fname = vim_strsave(IObuff);
    }
    எனில் (fname == வெற்று)
	திரும்பு வெற்று;

    ga_init2(&sp->buflines, அளவு(char_u *), 100);

    // Copy the lines from the buffer into a grow array
    க்கு (curr_lnum = eap->line1; curr_lnum <= eap->line2; curr_lnum++)
    {
	வரி = vim_strsave(ml_get(curr_lnum));
	எனில் (வரி == வெற்று)
	    செல் errret;
	எனில் (ga_add_string(&sp->buflines, வரி) == FAIL)
	    செல் errret;
    }
    sp->buf_lnum = 0;
    sp->source_from_buf = TRUE;
    // When sourcing a range of lines from a buffer, use buffer line number.
    sp->sourcing_lnum = eap->line1 - 1;

    திரும்பு fname;

errret:
    vim_free(fname);
    vim_free(வரி);
    ga_clear_strings(&sp->buflines);
    திரும்பு வெற்று;
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
    நிலையான முழு
do_source_ext(
    char_u	*fname,
    முழு		check_other,	    // check for .vimrc and _vimrc
    முழு		is_vimrc,	    // DOSO_ value
    முழு		*ret_sid UNUSED,
    exarg_T	*eap,
    முழு		clearvars UNUSED)
{
    source_cookie_T	    cookie;
    char_u		    *p;
    char_u		    *fname_not_fixed = வெற்று;
    char_u		    *fname_exp = வெற்று;
    char_u		    *firstline = வெற்று;
    முழு			    retval = FAIL;
    sctx_T		    save_current_sctx;
#ifdef STARTUPTIME
    கட்டமைப்பு timeval	    tv_rel;
    கட்டமைப்பு timeval	    tv_start;
#endif
#ifdef FEAT_PROFILE
    proftime_T		    wait_start;
#endif
    முழு			    save_sticky_cmdmod_flags = sticky_cmdmod_flags;
    முழு			    trigger_source_post = FALSE;
#ifdef FEAT_EVAL
    funccal_entry_T	    funccalp_entry;
    முழு			    save_debug_break_level = debug_break_level;
    முழு			    sid = -1;
    scriptitem_T	    *si = வெற்று;
    முழு			    save_estack_compiling = estack_compiling;
    ESTACK_CHECK_DECLARATION;
#endif

    CLEAR_FIELD(cookie);
    எனில் (fname == வெற்று)
    {
	// sourcing lines from a buffer
	fname_exp = do_source_buffer_init(&cookie, eap);
	எனில் (fname_exp == வெற்று)
	    திரும்பு FAIL;
    }
    இல்லையெனில்
    {
	fname_not_fixed = expand_env_save(fname);
	எனில் (fname_not_fixed == வெற்று)
	    செல் theend;
	fname_exp = fix_fname(fname_not_fixed);
	எனில் (fname_exp == வெற்று)
	    செல் theend;
	எனில் (mch_isdir(fname_exp))
	{
	    smsg(_("Cannot source a directory: \"%s\""), fname);
	    செல் theend;
	}
    }
#ifdef FEAT_EVAL
    estack_compiling = FALSE;

    // See if we loaded this script before.
    sid = find_script_by_name(fname_exp);
    எனில் (sid > 0 && ret_sid != வெற்று
			  && SCRIPT_ITEM(sid)->sn_state != SN_STATE_NOT_LOADED)
    {
	// Already loaded and no need to load again, return here.
	*ret_sid = sid;
	retval = OK;
	செல் theend;
    }
#endif

    // Apply SourceCmd autocommands, they should get the file and source it.
    // When sourcing a file "fname_exp" was made a full path name by
    // fix_fname() above, avoid expanding it again.
    எனில் ((fname == வெற்று
		? has_autocmd(EVENT_SOURCECMD, fname_exp, வெற்று)
		: has_autocmd_fullname(EVENT_SOURCECMD, fname_exp, வெற்று))
	    && apply_autocmds(EVENT_SOURCECMD, fname_exp, fname_exp,
							       FALSE, curbuf))
    {
#ifdef FEAT_EVAL
	retval = aborting() ? FAIL : OK;
#இல்லையெனில்
	retval = OK;
#endif
	எனில் (retval == OK)
	    // Apply SourcePost autocommands.
	    apply_autocmds(EVENT_SOURCEPOST, fname_exp, fname_exp,
								FALSE, curbuf);
	செல் theend;
    }

    // Apply SourcePre autocommands, they may get the file.
    apply_autocmds(EVENT_SOURCEPRE, fname_exp, fname_exp, FALSE, curbuf);

    எனில் (!cookie.source_from_buf)
    {
#ifdef USE_FOPEN_NOINH
	cookie.fp = fopen_noinh_readbin((எழுத்து *)fname_exp);
#இல்லையெனில்
	cookie.fp = mch_fopen((எழுத்து *)fname_exp, READBIN);
#endif
    }
    எனில் (cookie.fp == வெற்று && check_other)
    {
	// Try again, replacing file name ".vimrc" by "_vimrc" or vice versa,
	// and ".exrc" by "_exrc" or vice versa.
	p = gettail(fname_exp);
	எனில் ((*p == '.' || *p == '_')
		&& (STRICMP(p + 1, "vimrc") == 0
		    || STRICMP(p + 1, "gvimrc") == 0
		    || STRICMP(p + 1, "exrc") == 0))
	{
	    எனில் (*p == '_')
		*p = '.';
	    இல்லையெனில்
		*p = '_';
#ifdef USE_FOPEN_NOINH
	    cookie.fp = fopen_noinh_readbin((எழுத்து *)fname_exp);
#இல்லையெனில்
	    cookie.fp = mch_fopen((எழுத்து *)fname_exp, READBIN);
#endif
	}
    }

    எனில் (cookie.fp == வெற்று && !cookie.source_from_buf)
    {
	எனில் (p_verbose > 0)
	{
	    verbose_enter();
	    எனில் (SOURCING_NAME == வெற்று)
		smsg(_("could not source \"%s\""), fname);
	    இல்லையெனில்
		smsg(_("line %ld: could not source \"%s\""),
							SOURCING_LNUM, fname);
	    verbose_leave();
	}
	செல் theend;
    }

    // The file exists.
    // - In verbose mode, give a message.
    // - For a vimrc file, may want to set 'compatible', call vimrc_found().
    எனில் (p_verbose > 1)
    {
	verbose_enter();
	எனில் (SOURCING_NAME == வெற்று)
	    smsg(_("sourcing \"%s\""), fname);
	இல்லையெனில்
	    smsg(_("line %ld: sourcing \"%s\""), SOURCING_LNUM, fname);
	verbose_leave();
    }
    எனில் (is_vimrc == DOSO_VIMRC)
	vimrc_found(fname_exp, (char_u *)"MYVIMRC");
    இல்லையெனில் எனில் (is_vimrc == DOSO_GVIMRC)
	vimrc_found(fname_exp, (char_u *)"MYGVIMRC");

#ifdef USE_CRNL
    // If no automatic file format: Set default to CR-NL.
    எனில் (*p_ffs == NUL)
	cookie.fileformat = EOL_DOS;
    இல்லையெனில்
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
    எனில் (time_fd != வெற்று)
	time_தள்ளு(&tv_rel, &tv_start);
#endif

    // "legacy" does not apply to commands in the script
    sticky_cmdmod_flags = 0;

    save_current_sctx = current_sctx;
    எனில் ((cmdmod.cmod_flags & CMOD_VIM9CMD) && cookie.source_from_buf)
	// When the ":vim9cmd" command modifier is used, source buffer lines as
	// Vim9 script
	current_sctx.sc_version = SCRIPT_VERSION_VIM9;
    இல்லையெனில்
	current_sctx.sc_version = 1;  // default script version

#ifdef FEAT_EVAL
    current_sctx.sc_lnum = 0;

# ifdef FEAT_PROFILE
    எனில் (do_profiling == PROF_YES)
	prof_child_enter(&wait_start);		// entering a child now
# endif

    // Don't use local function variables, if called from a function.
    // Also starts profiling timer for nested script.
    save_funccal(&funccalp_entry);

    // Reset "KeyTyped" to avoid some commands thinking they are invoked
    // interactively.  E.g. defining a function would output indent.
    முழு save_சாவி_Typed = சாவி_Typed;
    சாவி_Typed = FALSE;

    // Check if this script was sourced before to find its SID.
    // Always use a new sequence number.
    current_sctx.sc_seq = ++last_current_SID_seq;
    எனில் (sid > 0)
    {
	hashtab_T	*ht;
	முழு		todo;
	hashitem_T	*hi;
	dictitem_T	*di;

	// loading the same script again
	current_sctx.sc_sid = sid;
	si = SCRIPT_ITEM(sid);
	எனில் (si->sn_state == SN_STATE_NOT_LOADED)
	{
	    // this script was found but not loaded yet
	    si->sn_state = SN_STATE_NEW;
	}
	இல்லையெனில்
	{
	    si->sn_state = SN_STATE_RELOAD;

	    எனில் (!clearvars)
	    {
		// Script-local variables remain but "const" can be set again.
		// In Vim9 script variables will be cleared when "vim9script"
		// is encountered without the "noclear" argument.
		ht = &SCRIPT_VARS(sid);
		todo = (முழு)ht->ht_used;
		FOR_ALL_HASHTAB_பொருட்கள்(ht, hi, todo)
		    எனில் (!HASHITEM_EMPTY(hi))
		    {
			--todo;
			di = HI2DI(hi);
			di->di_flags |= DI_FLAGS_RELOAD;
		    }
		// imports can be redefined once
		mark_imports_for_reload(sid);
	    }
	    இல்லையெனில்
		clear_vim9_scriptlocal_vars(sid);

	    // reset version, "vim9script" may have been added or removed.
	    si->sn_version = 1;
	}
	எனில் (ret_sid != வெற்று)
	    *ret_sid = sid;
    }
    இல்லையெனில்
    {
	முழு error = OK;

	// It's new, generate a new SID and initialize the scriptitem.
	sid = get_new_scriptitem(&error);
	current_sctx.sc_sid = sid;
	எனில் (error == FAIL)
	    செல் almosttheend;
	si = SCRIPT_ITEM(sid);
	si->sn_name = fname_exp;
	fname_exp = vim_strsave(si->sn_name);  // used for autocmd
	எனில் (ret_sid != வெற்று)
	    *ret_sid = sid;

	// Remember the "is_vimrc" flag for when the file is sourced again.
	si->sn_is_vimrc = is_vimrc;
    }

    // Keep the sourcing name/lnum, for recursive calls.
    estack_தள்ளு(ETYPE_SCRIPT, si->sn_name, 0);
    ESTACK_CHECK_SETUP;

# ifdef FEAT_PROFILE
    எனில் (do_profiling == PROF_YES)
    {
	முழு	forceit;

	// Check if we do profiling for this script.
	எனில் (!si->sn_prof_on && has_profiling(TRUE, si->sn_name, &forceit, வெற்று))
	{
	    script_do_profile(si);
	    si->sn_pr_force = forceit;
	}
	எனில் (si->sn_prof_on)
	{
	    ++si->sn_pr_மொத்தம்;
	    profile_start(&si->sn_pr_start);
	    profile_zero(&si->sn_pr_children);
	}
    }
# endif
#இல்லையெனில்
    // Keep the sourcing name/lnum, for recursive calls.
    estack_தள்ளு(ETYPE_SCRIPT, fname_exp, 0);
#endif

    cookie.conv.vc_type = CONV_NONE;		// no conversion

    // Read the first line so we can check for a UTF-8 BOM.
    firstline = getsourceline(0, (வெறுமை *)&cookie, 0, TRUE);
    எனில் (firstline != வெற்று && STRLEN(firstline) >= 3 && firstline[0] == 0xef
			      && firstline[1] == 0xbb && firstline[2] == 0xbf)
    {
	// Found BOM; setup conversion, skip over BOM and recode the line.
	convert_setup(&cookie.conv, (char_u *)"utf-8", p_enc);
	p = string_convert(&cookie.conv, firstline + 3, வெற்று);
	எனில் (p == வெற்று)
	    p = vim_strsave(firstline + 3);
	எனில் (p != வெற்று)
	{
	    vim_free(firstline);
	    firstline = p;
	}
    }

    // Call do_cmdline, which will call getsourceline() to get the lines.
    do_cmdline(firstline, getsourceline, (வெறுமை *)&cookie,
				     DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_REPEAT);
    retval = OK;

#ifdef FEAT_PROFILE
    எனில் (do_profiling == PROF_YES)
    {
	// Get "si" again, "script_items" may have been reallocated.
	si = SCRIPT_ITEM(sid);
	எனில் (si->sn_prof_on)
	{
	    profile_end(&si->sn_pr_start);
	    profile_sub_wait(&wait_start, &si->sn_pr_start);
	    profile_add(&si->sn_pr_total, &si->sn_pr_start);
	    profile_self(&si->sn_pr_self, &si->sn_pr_start,
							 &si->sn_pr_children);
	}
    }
#endif

    எனில் (got_int)
	emsg(_(e_interrupted));
#ifdef FEAT_EVAL
    ESTACK_CHECK_NOW;
#endif
    estack_எடு();
    எனில் (p_verbose > 1)
    {
	verbose_enter();
	smsg(_("finished sourcing %s"), fname);
	எனில் (SOURCING_NAME != வெற்று)
	    smsg(_("continuing in %s"), SOURCING_NAME);
	verbose_leave();
    }
#ifdef STARTUPTIME
    எனில் (time_fd != வெற்று)
    {
	vim_snprintf((எழுத்து *)IObuff, IOSIZE, "sourcing %s", fname);
	time_செய்தி((எழுத்து *)IObuff, &tv_start);
	time_எடு(&tv_rel);
    }
#endif

    எனில் (!got_int)
	trigger_source_post = TRUE;

#ifdef FEAT_EVAL
    // After a "finish" in debug mode, need to break at first command of next
    // sourced file.
    எனில் (save_debug_break_level > ex_nesting_level
	    && debug_break_level == ex_nesting_level)
	++debug_break_level;
#endif

#ifdef FEAT_EVAL
almosttheend:
    // If "sn_save_cpo" is set that means we encountered "vim9script": restore
    // 'cpoptions', unless in the main .vimrc file.
    // Get "si" again, "script_items" may have been reallocated.
    si = SCRIPT_ITEM(sid);
    எனில் (si->sn_save_cpo != வெற்று && si->sn_is_vimrc == DOSO_NONE)
    {
	எனில் (STRCMP(p_cpo, CPO_VIM) != 0)
	{
	    char_u *f;
	    char_u *t;

	    // 'cpo' was changed in the script.  Apply the same change to the
	    // saved value, if possible.
	    க்கு (f = (char_u *)CPO_VIM; *f != NUL; ++f)
		எனில் (vim_strchr(p_cpo, *f) == வெற்று
			&& (t = vim_strchr(si->sn_save_cpo, *f)) != வெற்று)
		    // flag was removed, also remove it from the saved 'cpo'
		    mch_memmove(t, t + 1, STRLEN(t));
	    க்கு (f = p_cpo; *f != NUL; ++f)
		எனில் (vim_strchr((char_u *)CPO_VIM, *f) == வெற்று
			&& vim_strchr(si->sn_save_cpo, *f) == வெற்று)
		{
		    // flag was added, also add it to the saved 'cpo'
		    t = alloc(STRLEN(si->sn_save_cpo) + 2);
		    எனில் (t != வெற்று)
		    {
			*t = *f;
			STRCPY(t + 1, si->sn_save_cpo);
			vim_free(si->sn_save_cpo);
			si->sn_save_cpo = t;
		    }
		}
	}
	set_option_மதிப்பு_give_err((char_u *)"cpo",
					   0L, si->sn_save_cpo, OPT_NO_REDRAW);
    }
    VIM_CLEAR(si->sn_save_cpo);

    restore_funccal();
# ifdef FEAT_PROFILE
    எனில் (do_profiling == PROF_YES)
	prof_child_exit(&wait_start);		// leaving a child now
# endif

    சாவி_Typed = save_சாவி_Typed;
#endif
    current_sctx = save_current_sctx;

    எனில் (cookie.fp != வெற்று)
	fclose(cookie.fp);
    எனில் (cookie.source_from_buf)
	ga_clear_strings(&cookie.buflines);
    vim_free(cookie.nextline);
    vim_free(firstline);
    convert_setup(&cookie.conv, வெற்று, வெற்று);

    எனில் (trigger_source_post)
	apply_autocmds(EVENT_SOURCEPOST, fname_exp, fname_exp, FALSE, curbuf);

theend:
#ifdef FEAT_EVAL
    எனில் (sid > 0 && ret_sid != வெற்று
	    && fname_not_fixed != வெற்று && fname_exp != வெற்று)
    {
	முழு not_fixed_sid = find_script_by_name(fname_not_fixed);

	// If "fname_not_fixed" is a symlink then we source the linked file.
	// If the original name is in the script list we add the ID of the
	// script that was actually sourced.
	எனில் (SCRIPT_ID_VALID(not_fixed_sid) && not_fixed_sid != sid)
	    SCRIPT_ITEM(not_fixed_sid)->sn_sourced_sid = sid;
    }
#endif

    vim_free(fname_not_fixed);
    vim_free(fname_exp);
    sticky_cmdmod_flags = save_sticky_cmdmod_flags;
#ifdef FEAT_EVAL
    estack_compiling = save_estack_compiling;
#endif
    திரும்பு retval;
}

    முழு
do_source(
    char_u	*fname,
    முழு		check_other,	    // check for .vimrc and _vimrc
    முழு		is_vimrc,	    // DOSO_ value
    முழு		*ret_sid)
{
    திரும்பு do_source_ext(fname, check_other, is_vimrc, ret_sid, வெற்று, FALSE);
}


#எனில் defined(FEAT_EVAL)

/*
 * ":scriptnames"
 */
    வெறுமை
ex_scriptnames(exarg_T *eap)
{
    முழு i;

    எனில் (eap->addr_மொத்தம் > 0 || *eap->arg != NUL)
    {
	// :script {scriptId}: edit the script
	எனில் (eap->addr_மொத்தம் > 0 && !SCRIPT_ID_VALID(eap->line2))
	    emsg(_(e_invalid_argument));
	இல்லையெனில்
	{
	    எனில் (eap->addr_மொத்தம் > 0)
		eap->arg = SCRIPT_ITEM(eap->line2)->sn_name;
	    இல்லையெனில்
	    {
		expand_env(eap->arg, NameBuff, MAXPATHL);
		eap->arg = NameBuff;
	    }
	    do_exedit(eap, வெற்று);
	}
	திரும்பு;
    }

    க்கு (i = 1; i <= script_பொருட்கள்.ga_len && !got_int; ++i)
    {
	scriptitem_T *si = SCRIPT_ITEM(i);

	எனில் (si->sn_name != வெற்று)
	{
	    எழுத்து sourced_buf[20];

	    home_replace(வெற்று, si->sn_name, NameBuff, MAXPATHL, TRUE);
	    எனில் (si->sn_sourced_sid > 0)
		vim_snprintf(sourced_buf, 20, "->%d", si->sn_sourced_sid);
	    இல்லையெனில்
		sourced_buf[0] = NUL;
	    vim_snprintf((எழுத்து *)IObuff, IOSIZE, "%3d%s%s: %s",
		    i,
		    sourced_buf,
		    si->sn_state == SN_STATE_NOT_LOADED ? " A" : "",
		    NameBuff);
	    எனில் (!message_filtered(IObuff))
	    {
		செய்தி_putchar('\n');
		செய்தி_outtrans(IObuff);
		out_flush();	    // output one line at a time
		ui_breakcheck();
	    }
	}
    }
}

# எனில் defined(BACKSLASH_IN_FILENAME)
/*
 * Fix slashes in the list of script names for 'shellslash'.
 */
    வெறுமை
scriptnames_slash_adjust(வெறுமை)
{
    முழு i;

    க்கு (i = 1; i <= script_பொருட்கள்.ga_len; ++i)
	எனில் (SCRIPT_ITEM(i)->sn_name != வெற்று)
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
    எனில் (id == SID_MODELINE)
	திரும்பு (char_u *)_("modeline");
    எனில் (id == SID_CMDARG)
	திரும்பு (char_u *)_("--cmd argument");
    எனில் (id == SID_CARG)
	திரும்பு (char_u *)_("-c argument");
    எனில் (id == SID_ENV)
	திரும்பு (char_u *)_("environment variable");
    எனில் (id == SID_ERROR)
	திரும்பு (char_u *)_("error handler");
    எனில் (id == SID_WINLAYOUT)
	திரும்பு (char_u *)_("changed window size");
    திரும்பு SCRIPT_ITEM(id)->sn_name;
}

# எனில் defined(EXITFREE)
    வெறுமை
free_scriptnames(வெறுமை)
{
    முழு			i;

    க்கு (i = script_பொருட்கள்.ga_len; i > 0; --i)
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
    ga_clear(&script_பொருட்கள்);
}

    வெறுமை
free_autoload_scriptnames(வெறுமை)
{
    ga_clear_strings(&ga_loaded);
}
# endif

    linenr_T
get_sourced_lnum(
	char_u *(*fgetline)(முழு, வெறுமை *, முழு, getline_opt_T),
	வெறுமை *cookie)
{
    திரும்பு fgetline == getsourceline
			? ((source_cookie_T *)cookie)->sourcing_lnum
			: SOURCING_LNUM;
}

/*
 * Return a List of script-local functions defined in the script with id
 * 'sid'.
 */
    நிலையான list_T *
get_script_local_funcs(scid_T sid)
{
    hashtab_T	*functbl;
    hashitem_T	*hi;
    long_u	todo;
    list_T	*l;

    l = list_alloc();
    எனில் (l == வெற்று)
	திரும்பு வெற்று;

    // Iterate through all the functions in the global function hash table
    // looking for functions with script ID 'sid'.
    functbl = func_tbl_get();
    todo = functbl->ht_used;
    FOR_ALL_HASHTAB_பொருட்கள்(functbl, hi, todo)
    {
	ufunc_T	*fp;

	எனில் (HASHITEM_EMPTY(hi))
	    தொடர்;

	--todo;
	fp = HI2UF(hi);

	// Add active functions with script id == 'sid'
	எனில் (!(fp->uf_flags & FC_DEAD) && (fp->uf_script_ctx.sc_sid == sid))
	{
	    char_u	*name;

	    எனில் (fp->uf_name_exp != வெற்று)
		name = fp->uf_name_exp;
	    இல்லையெனில்
		name = fp->uf_name;

	    list_append_string(l, name, -1);
	}
    }

    திரும்பு l;
}

/*
 * getscriptinfo() function
 */
    வெறுமை
f_getscriptinfo(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    char_u	*pat = வெற்று;
    regmatch_T	regmatch;
    முழு		filterpat = FALSE;
    scid_T	sid = -1;

    எனில் (rettv_list_alloc(rettv) == FAIL)
	திரும்பு;

    எனில் (check_for_opt_dict_arg(argvars, 0) == FAIL)
	திரும்பு;

    l = rettv->vval.v_list;

    regmatch.regprog = வெற்று;
    regmatch.rm_ic = p_ic;

    எனில் (argvars[0].v_type == VAR_DICT)
    {
	dictitem_T *sid_di = dict_find(argvars[0].vval.v_dict,
							   (char_u *)"sid", 3);
	எனில் (sid_di != வெற்று)
	{
	    முழு error = FALSE;
	    sid = tv_get_எண்_chk(&sid_di->di_tv, &error);
	    எனில் (error)
		திரும்பு;
	    எனில் (sid <= 0)
	    {
		semsg(_(e_invalid_மதிப்பு_for_argument_str_str), "sid",
						tv_get_string(&sid_di->di_tv));
		திரும்பு;
	    }
	}
	இல்லையெனில்
	{
	    pat = dict_get_string(argvars[0].vval.v_dict, "name", TRUE);
	    எனில் (pat != வெற்று)
		regmatch.regprog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
	    எனில் (regmatch.regprog != வெற்று)
		filterpat = TRUE;
	}
    }

    க்கு (varnumber_T i = sid > 0 ? sid : 1;
		       (i == sid || sid <= 0) && i <= script_பொருட்கள்.ga_len; ++i)
    {
	scriptitem_T	*si = SCRIPT_ITEM(i);
	dict_T		*d;

	எனில் (si->sn_name == வெற்று)
	    தொடர்;

	எனில் (filterpat && !vim_regexec(&regmatch, si->sn_name, (colnr_T)0))
	    தொடர்;

	எனில் ((d = dict_alloc()) == வெற்று
		|| list_append_dict(l, d) == FAIL)
	{
	    dict_unref(d);
	    செல் theend;
	}
	எனில் (dict_add_string(d, "name", si->sn_name) == FAIL
		|| dict_add_எண்(d, "sid", i) == FAIL
		|| dict_add_எண்(d, "sourced", si->sn_sourced_sid) == FAIL
		|| dict_add_எண்(d, "version", si->sn_version) == FAIL
		|| dict_add_bool(d, "autoload",
				si->sn_state == SN_STATE_NOT_LOADED) == FAIL)
	    செல் theend;

	// When a script ID is specified, return information about only the
	// specified script, and add the script-local variables and functions.
	எனில் (sid > 0)
	{
	    dict_T	*var_dict;
	    list_T	*fn_list;

	    var_dict = dict_copy(&si->sn_vars->sv_dict, TRUE, TRUE,
								get_copyID());
	    எனில் (var_dict == வெற்று)
		செல் theend;
	    எனில் (dict_add_dict(d, "variables", var_dict) == FAIL)
	    {
		dict_unref(var_dict);
		செல் theend;
	    }
	    --var_dict->dv_refcount;
	    fn_list = get_script_local_funcs(sid);
	    எனில் (fn_list == வெற்று || dict_add_list(d, "functions", fn_list) == FAIL)
	    {
		list_unref(fn_list);
		செல் theend;
	    }
	}
    }

theend:
    vim_regfree(regmatch.regprog);
    vim_free(pat);
}

#endif

    நிலையான char_u *
get_one_sourceline(source_cookie_T *sp)
{
    garray_T		ga;
    முழு			len;
    முழு			c;
    char_u		*buf;
#ifdef USE_CRNL
    முழு			has_cr;		// CR-LF found
#endif
    முழு			have_read = FALSE;

    // use a growarray to store the sourced line
    ga_init2(&ga, 1, 250);

    // Loop until there is a finished line (or end-of-file).
    ++sp->sourcing_lnum;
    க்கு (;;)
    {
	// make room to read at least 120 (more) characters
	எனில் (ga_grow(&ga, 120) == FAIL)
	    நிறுத்து;
	எனில் (sp->source_from_buf)
	{
	    எனில் (sp->buf_lnum >= sp->buflines.ga_len)
		நிறுத்து;		    // all the lines are processed
	    ga_concat(&ga, ((char_u **)sp->buflines.ga_data)[sp->buf_lnum]);
	    sp->buf_lnum++;
	    எனில் (ga_grow(&ga, 1) == FAIL)
		நிறுத்து;
	    buf = (char_u *)ga.ga_data;
	    buf[ga.ga_len++] = NUL;
	    len = ga.ga_len;
	}
	இல்லையெனில்
	{
	    buf = (char_u *)ga.ga_data;
	    எனில் (fgets((எழுத்து *)buf + ga.ga_len, ga.ga_maxlen - ga.ga_len,
			sp->fp) == வெற்று)
		நிறுத்து;
	    len = ga.ga_len + (முழு)STRLEN(buf + ga.ga_len);
	}
#ifdef USE_CRNL
	// Ignore a trailing CTRL-Z, when in Dos mode.	Only recognize the
	// CTRL-Z by its own, or after a NL.
	எனில் (	   (len == 1 || (len >= 2 && buf[len - 2] == '\n'))
		&& sp->fileformat == EOL_DOS
		&& buf[len - 1] == Ctrl_Z)
	{
	    buf[len - 1] = NUL;
	    நிறுத்து;
	}
#endif

	have_read = TRUE;
	ga.ga_len = len;

	// If the line was longer than the buffer, read more.  A line of a
	// buffer was taken as a whole.
	எனில் (!sp->source_from_buf && ga.ga_maxlen - ga.ga_len == 1
						       && buf[len - 1] != '\n')
	    தொடர்;

	எனில் (len >= 1 && buf[len - 1] == '\n')	// remove trailing NL
	{
#ifdef USE_CRNL
	    has_cr = (len >= 2 && buf[len - 2] == '\r');
	    எனில் (sp->fileformat == EOL_UNKNOWN)
	    {
		எனில் (has_cr)
		    sp->fileformat = EOL_DOS;
		இல்லையெனில்
		    sp->fileformat = EOL_UNIX;
	    }

	    எனில் (sp->fileformat == EOL_DOS)
	    {
		எனில் (has_cr)	    // replace trailing CR
		{
		    buf[len - 2] = '\n';
		    --len;
		    --ga.ga_len;
		}
		இல்லையெனில்	    // lines like ":map xx yy^M" will have failed
		{
		    எனில் (!sp->error)
		    {
			செய்தி_source(HL_ATTR(HLF_W));
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
	    க்கு (c = len - 2; c >= 0 && buf[c] == Ctrl_V; c--)
		;
	    எனில் ((len & 1) != (c & 1))	// escaped NL, read more
	    {
		++sp->sourcing_lnum;
		தொடர்;
	    }

	    buf[len - 1] = NUL;		// remove the NL
	}

	// Check for ^C here now and then, so recursive :so can be broken.
	வரி_breakcheck();
	நிறுத்து;
    }

    எனில் (have_read)
	திரும்பு (char_u *)ga.ga_data;

    vim_free(ga.ga_data);
    திரும்பு வெற்று;
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
	முழு c UNUSED,
	வெறுமை *cookie,
	முழு indent UNUSED,
	getline_opt_T options)
{
    source_cookie_T	*sp = (source_cookie_T *)cookie;
    char_u		*வரி;
    char_u		*p;
    முழு			do_vim9_all = in_vim9script()
					      && options == GETLINE_CONCAT_ALL;
    முழு			do_bar_cont = do_vim9_all
					 || options == GETLINE_CONCAT_CONTBAR;

#ifdef FEAT_EVAL
    // If breakpoints have been added/deleted need to check for it.
    எனில் ((sp->dbg_tick < debug_tick) && !sp->source_from_buf)
    {
	sp->breakpoint = dbg_find_breakpoint(TRUE, sp->fname, SOURCING_LNUM);
	sp->dbg_tick = debug_tick;
    }
# ifdef FEAT_PROFILE
    எனில் (do_profiling == PROF_YES)
	script_வரி_end();
# endif
#endif

    // Set the current sourcing line number.
    SOURCING_LNUM = sp->sourcing_lnum + 1;

    // Get current line.  If there is a read-ahead line, use it, otherwise get
    // one now.  "fp" is NULL if actually using a string.
    எனில் (sp->finished || (!sp->source_from_buf && sp->fp == வெற்று))
	வரி = வெற்று;
    இல்லையெனில் எனில் (sp->nextline == வெற்று)
	வரி = get_one_sourceline(sp);
    இல்லையெனில்
    {
	வரி = sp->nextline;
	sp->nextline = வெற்று;
	++sp->sourcing_lnum;
    }
#ifdef FEAT_PROFILE
    எனில் (வரி != வெற்று && do_profiling == PROF_YES)
	script_வரி_start();
#endif

    // Only concatenate lines starting with a \ when 'cpoptions' doesn't
    // contain the 'C' flag.
    எனில் (வரி != வெற்று && options != GETLINE_NONE
				      && vim_strchr(p_cpo, CPO_CONCAT) == வெற்று)
    {
	முழு comment_char = in_vim9script() ? '#' : '"';

	// compensate for the one line read-ahead
	--sp->sourcing_lnum;

	// Get the next line and concatenate it when it starts with a
	// backslash. We always need to read the next line, keep it in
	// sp->nextline.
	/* Also check for a comment in between continuation lines: "\ */
	// Also check for a Vim9 comment, empty line, line starting with '|',
	// but not "||".
	sp->nextline = get_one_sourceline(sp);
	எனில் (sp->nextline != வெற்று
		&& (*(p = skipwhite(sp->nextline)) == '\\'
			      || (p[0] == comment_char
						&& p[1] == '\\' && p[2] == ' ')
			      || (do_vim9_all && (*p == NUL
						     || vim9_comment_start(p)))
			      || (do_bar_cont && p[0] == '|' && p[1] != '|')))
	{
	    garray_T    ga;

	    ga_init2(&ga, அளவு(char_u), 400);
	    ga_concat(&ga, வரி);
	    எனில் (*p == '\\')
		ga_concat(&ga, p + 1);
	    இல்லையெனில் எனில் (*p == '|')
	    {
		GA_CONCAT_LITERAL(&ga, " ");
		ga_concat(&ga, p);
	    }
	    க்கு (;;)
	    {
		vim_free(sp->nextline);
		sp->nextline = get_one_sourceline(sp);
		எனில் (sp->nextline == வெற்று)
		    நிறுத்து;
		p = skipwhite(sp->nextline);
		எனில் (*p == '\\' || (do_bar_cont && p[0] == '|' && p[1] != '|'))
		{
		    // Adjust the growsize to the current length to speed up
		    // concatenating many lines.
		    எனில் (ga.ga_len > 400)
		    {
			எனில் (ga.ga_len > 8000)
			    ga.ga_growsize = 8000;
			இல்லையெனில்
			    ga.ga_growsize = ga.ga_len;
		    }
		    எனில் (*p == '\\')
			ga_concat(&ga, p + 1);
		    இல்லையெனில்
		    {
			GA_CONCAT_LITERAL(&ga, " ");
			ga_concat(&ga, p);
		    }
		}
		இல்லையெனில் எனில் (!(p[0] == (comment_char)
						&& p[1] == '\\' && p[2] == ' ')
		     && !(do_vim9_all && (*p == NUL || vim9_comment_start(p))))
		    நிறுத்து;
		/* drop a # comment or "\ comment line */
	    }
	    ga_append(&ga, NUL);
	    vim_free(வரி);
	    வரி = ga.ga_data;
	}
    }

    எனில் (வரி != வெற்று && sp->conv.vc_type != CONV_NONE)
    {
	char_u	*s;

	// Convert the encoding of the script line.
	s = string_convert(&sp->conv, வரி, வெற்று);
	எனில் (s != வெற்று)
	{
	    vim_free(வரி);
	    வரி = s;
	}
    }

#ifdef FEAT_EVAL
    // Did we encounter a breakpoint?
    எனில் (!sp->source_from_buf && sp->breakpoint != 0
	    && sp->breakpoint <= SOURCING_LNUM)
    {
	dbg_breakpoint(sp->fname, SOURCING_LNUM);
	// Find next breakpoint.
	sp->breakpoint = dbg_find_breakpoint(TRUE, sp->fname, SOURCING_LNUM);
	sp->dbg_tick = debug_tick;
    }
#endif

    திரும்பு வரி;
}

/*
 * Returns TRUE if sourcing a script either from a file or a buffer.
 * Otherwise returns FALSE.
 */
    முழு
sourcing_a_script(exarg_T *eap)
{
    திரும்பு (getline_equal(eap->ea_getline, eap->cookie, getsourceline));
}

/*
 * ":scriptencoding": Set encoding conversion for a sourced script.
 */
    வெறுமை
ex_scriptencoding(exarg_T *eap)
{
    source_cookie_T	*sp;
    char_u		*name;

    எனில் (!sourcing_a_script(eap))
    {
	emsg(_(e_scriptencoding_used_outside_of_sourced_file));
	திரும்பு;
    }

    எனில் (*eap->arg != NUL)
    {
	name = enc_canonize(eap->arg);
	எனில் (name == வெற்று)	// out of memory
	    திரும்பு;
    }
    இல்லையெனில்
	name = eap->arg;

    // Setup for conversion from the specified encoding to 'encoding'.
    sp = (source_cookie_T *)getline_cookie(eap->ea_getline, eap->cookie);
    convert_setup(&sp->conv, name, p_enc);

    எனில் (name != eap->arg)
	vim_free(name);
}

/*
 * ":scriptversion": Set Vim script version for a sourced script.
 */
    வெறுமை
ex_scriptversion(exarg_T *eap UNUSED)
{
    முழு		nr;

    எனில் (!sourcing_a_script(eap))
    {
	emsg(_(e_scriptversion_used_outside_of_sourced_file));
	திரும்பு;
    }
    எனில் (in_vim9script())
    {
	emsg(_(e_cannot_use_scriptversion_after_vim9script));
	திரும்பு;
    }

    nr = getdigits(&eap->arg);
    எனில் (nr == 0 || *eap->arg != NUL)
	emsg(_(e_invalid_argument));
    இல்லையெனில் எனில் (nr > SCRIPT_VERSION_MAX)
	semsg(_(e_scriptversion_not_supported_nr), nr);
    இல்லையெனில்
    {
	current_sctx.sc_version = nr;
#ifdef FEAT_EVAL
	SCRIPT_ITEM(current_sctx.sc_sid)->sn_version = nr;
#endif
    }
}

#எனில் defined(FEAT_EVAL)
/*
 * ":finish": Mark a sourced file as finished.
 */
    வெறுமை
ex_finish(exarg_T *eap)
{
    எனில் (sourcing_a_script(eap))
	do_finish(eap, FALSE);
    இல்லையெனில்
	emsg(_(e_finish_used_outside_of_sourced_file));
}

/*
 * Mark a sourced file as finished.  Possibly makes the ":finish" pending.
 * Also called for a pending finish at the ":endtry" or after returning from
 * an extra do_cmdline().  "reanimate" is used in the latter case.
 */
    வெறுமை
do_finish(exarg_T *eap, முழு reanimate)
{
    முழு		idx;

    எனில் (reanimate)
	((source_cookie_T *)getline_cookie(eap->ea_getline,
					      eap->cookie))->finished = FALSE;

    // Cleanup (and inactivate) conditionals, but stop when a try conditional
    // not in its finally clause (which then is to be executed next) is found.
    // In this case, make the ":finish" pending for execution at the ":endtry".
    // Otherwise, finish normally.
    idx = cleanup_conditionals(eap->cstack, 0, TRUE);
    எனில் (idx >= 0)
    {
	eap->cstack->cs_pending[idx] = CSTP_FINISH;
	report_make_pending(CSTP_FINISH, வெற்று);
    }
    இல்லையெனில்
	((source_cookie_T *)getline_cookie(eap->ea_getline,
					       eap->cookie))->finished = TRUE;
}


/*
 * Return TRUE when a sourced file had the ":finish" command: Don't give error
 * message for missing ":endif".
 * Return FALSE when not sourcing a file.
 */
    முழு
source_finished(
    char_u	*(*fgetline)(முழு, வெறுமை *, முழு, getline_opt_T),
    வெறுமை	*cookie)
{
    திரும்பு (getline_equal(fgetline, cookie, getsourceline)
	    && ((source_cookie_T *)getline_cookie(
						fgetline, cookie))->finished);
}

/*
 * Find the path of a script below the "autoload" directory.
 * Returns NULL if there is no "/autoload/" in the script name.
 */
    நிலையான char_u *
script_name_after_autoload(scriptitem_T *si)
{
    char_u	*p = si->sn_name;
    char_u	*res = வெற்று;

    க்கு (;;)
    {
	char_u *n = (char_u *)strstr((எழுத்து *)p, "autoload");

	எனில் (n == வெற்று)
	    நிறுத்து;
	எனில் (n > p && vim_ispathsep(n[-1]) && vim_ispathsep(n[8]))
	    res = n + 9;
	p = n + 8;
    }
    திரும்பு res;
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

    எனில் (p == வெற்று)
	திரும்பு வெற்று;
    prefix = vim_strsave(p);
    எனில் (prefix == வெற்று)
	திரும்பு வெற்று;

    // replace all '/' with '#' and locate ".vim" at the end
    க்கு (p = prefix; *p != NUL; p += mb_ptr2len(p))
    {
	எனில் (vim_ispathsep(*p))
	    *p = '#';
	இல்லையெனில் எனில் (STRCMP(p, ".vim") == 0)
	{
	    p[0] = '#';
	    p[1] = NUL;
	    திரும்பு prefix;
	}
    }

    // did not find ".vim" at the end
    vim_free(prefix);
    திரும்பு வெற்று;
}

/*
 * If in a Vim9 autoload script return "name" with the autoload prefix for the
 * script.  If successful the returned name is allocated.
 * Otherwise it returns "name" unmodified.
 */
    char_u *
may_prefix_autoload(char_u *name)
{
    எனில் (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	திரும்பு name;

    scriptitem_T *si = SCRIPT_ITEM(current_sctx.sc_sid);

    எனில் (si->sn_autoload_prefix == வெற்று)
	திரும்பு name;

    char_u  *basename = name;
    size_t  len;
    char_u  *res;

    எனில் (*name == K_SPECIAL)
    {
	char_u *p = vim_strchr(name, '_');

	// skip over "<SNR>99_"
	எனில் (p != வெற்று)
	    basename = p + 1;
    }

    len = STRLEN(si->sn_autoload_prefix) + STRLEN(basename) + 2;
    res = alloc(len);
    எனில் (res == வெற்று)
	திரும்பு வெற்று;

    vim_snprintf((எழுத்து *)res, len, "%s%s", si->sn_autoload_prefix, basename);
    திரும்பு res;
}

/*
 * Return the autoload script name for a function or variable name.
 * Returns NULL when out of memory.
 * Caller must make sure that "name" contains AUTOLOAD_CHAR.
 */
    char_u *
autoload_name(char_u *name)
{
    char_u	*p, *q = வெற்று;
    char_u	*scriptname;

    // Get the script file name: replace '#' with '/', append ".vim".
    scriptname = alloc(STRLEN(name) + 14);
    எனில் (scriptname == வெற்று)
	திரும்பு வெற்று;
    STRCPY(scriptname, "autoload/");
    STRCAT(scriptname, name[0] == 'g' && name[1] == ':' ? name + 2: name);
    க்கு (p = scriptname + 9; (p = vim_strchr(p, AUTOLOAD_CHAR)) != வெற்று;
								    q = p, ++p)
	*p = '/';
    STRCPY(q, ".vim");
    திரும்பு scriptname;
}

/*
 * If "name" has a package name try autoloading the script for it.
 * Return TRUE if a package was loaded.
 */
    முழு
script_autoload(
    char_u	*name,
    முழு		reload)	    // load script again when already loaded
{
    char_u	*p;
    char_u	*scriptname, *tofree;
    முழு		ret = FALSE;
    முழு		i;
    முழு		ret_sid;

    // If the name starts with "<SNR>123_" then "123" is the script ID.
    எனில் (name[0] == K_SPECIAL && name[1] == KS_EXTRA && name[2] == KE_SNR)
    {
	p = name + 3;
	ret_sid = (முழு)getdigits(&p);
	எனில் (*p == '_' && SCRIPT_ID_VALID(ret_sid))
	{
	    may_load_script(ret_sid, &ret);
	    திரும்பு ret;
	}
    }

    // If there is no '#' after name[0] there is no package name.
    p = vim_strchr(name, AUTOLOAD_CHAR);
    எனில் (p == வெற்று || p == name)
	திரும்பு FALSE;

    tofree = scriptname = autoload_name(name);
    எனில் (scriptname == வெற்று)
	திரும்பு FALSE;

    // Find the name in the list of previously loaded package names.  Skip
    // "autoload/", it's always the same.
    க்கு (i = 0; i < ga_loaded.ga_len; ++i)
	எனில் (STRCMP(((char_u **)ga_loaded.ga_data)[i] + 9, scriptname + 9) == 0)
	    நிறுத்து;
    எனில் (!reload && i < ga_loaded.ga_len)
	ret = FALSE;	    // was loaded already
    இல்லையெனில்
    {
	// Remember the name if it wasn't loaded already.
	எனில் (i == ga_loaded.ga_len && ga_grow(&ga_loaded, 1) == OK)
	{
	    ((char_u **)ga_loaded.ga_data)[ga_loaded.ga_len++] = scriptname;
	    tofree = வெற்று;
	}

	// Try loading the package from $VIMRUNTIME/autoload/<name>.vim
	// Use "ret_sid" to avoid loading the same script again.
	எனில் (source_in_path(p_rtp, scriptname, DIP_START, &ret_sid) == OK)
	    ret = TRUE;
    }

    vim_free(tofree);
    திரும்பு ret;
}
#endif
