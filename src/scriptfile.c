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

#अन्तर्भाव "vim.h"

#जर defined(FEAT_EVAL)
// The names of packages that once were loaded are remembered.
स्थिर garray_T		ga_loaded = {0, 0, आकार(char_u *), 4, रिक्त};
#endif

// last used sequence number for sourcing scripts (current_sctx.sc_seq)
#ifdef FEAT_EVAL
स्थिर पूर्णांक		last_current_SID_seq = 0;
#endif

स्थिर पूर्णांक do_source_ext(char_u *fname, पूर्णांक check_other, पूर्णांक is_vimrc, पूर्णांक *ret_sid, exarg_T *eap, पूर्णांक clearvars);

/*
 * Initialize the execution stack.
 */
    शून्य
estack_init(शून्य)
{
    estack_T *entry;

    जर (ga_grow(&exestack, 10) == FAIL)
	mch_exit(0);
    entry = ((estack_T *)exestack.ga_data) + exestack.ga_len;
    entry->es_type = ETYPE_शीर्ष;
    entry->es_name = रिक्त;
    entry->es_lnum = 0;
#ifdef FEAT_EVAL
    entry->es_info.ufunc = रिक्त;
#endif
    ++exestack.ga_len;
}

/*
 * Add an item to the execution stack.
 * Returns the new entry or NULL when out of memory.
 */
    estack_T *
estack_ढकला(etype_T type, char_u *name, दीर्घ lnum)
{
    estack_T *entry;

    // If memory allocation fails then we'll pop more than we push, eventually
    // at the top level it will be OK again.
    जर (ga_grow(&exestack, 1) == FAIL)
	परत रिक्त;

    entry = ((estack_T *)exestack.ga_data) + exestack.ga_len;
    entry->es_type = type;
    entry->es_name = name;
    entry->es_lnum = lnum;
#ifdef FEAT_EVAL
    entry->es_info.ufunc = रिक्त;
#endif
    ++exestack.ga_len;
    परत entry;
}

#जर defined(FEAT_EVAL)
/*
 * Add a user function to the execution stack.
 */
    estack_T *
estack_ढकला_ufunc(ufunc_T *ufunc, दीर्घ lnum)
{
    estack_T *entry = estack_ढकला(ETYPE_UFUNC,
	    ufunc->uf_name_exp != रिक्त
				  ? ufunc->uf_name_exp : ufunc->uf_name, lnum);
    जर (entry != रिक्त)
	entry->es_info.ufunc = ufunc;
    परत entry;
}

/*
 * Return TRUE if "ufunc" with "lnum" is already at the top of the exe stack.
 */
    पूर्णांक
estack_शीर्ष_is_ufunc(ufunc_T *ufunc, दीर्घ lnum)
{
    estack_T *entry;

    जर (exestack.ga_len == 0)
	परत FALSE;
    entry = ((estack_T *)exestack.ga_data) + exestack.ga_len - 1;
    परत entry->es_type == ETYPE_UFUNC
	&& STRCMP( entry->es_name, ufunc->uf_name_exp != रिक्त
				    ? ufunc->uf_name_exp : ufunc->uf_name) == 0
	&& entry->es_lnum == lnum;
}
#endif

/*
 * Take an item off of the execution stack and return it.
 */
    estack_T *
estack_काढा(शून्य)
{
    जर (exestack.ga_len == 0)
	परत रिक्त;
    --exestack.ga_len;
    परत ((estack_T *)exestack.ga_data) + exestack.ga_len;
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
    माप_प्रकार	len;
    पूर्णांक		idx;
    etype_T	last_type = ETYPE_SCRIPT;
#endif

    entry = ((estack_T *)exestack.ga_data) + exestack.ga_len - 1;
#ifdef FEAT_EVAL
    जर (which == ESTACK_SFILE && entry->es_type != ETYPE_UFUNC)
#endif
    {
	जर (entry->es_name == रिक्त)
	    परत रिक्त;
	परत vim_strsave(entry->es_name);
    }
#ifdef FEAT_EVAL
    // expand('<sfile>') works in a function for backwards compatibility, but
    // may give an unexpected result.  Disallow it in Vim 9 script.
    जर (which == ESTACK_SFILE && in_vim9script())
    {
	पूर्णांक  save_emsg_off = emsg_off;

	जर (emsg_off == 1)
	    // f_expand() silences errors but we do want this one
	    emsg_off = 0;
	emsg(_(e_cannot_expand_sfile_in_vim9_function));
	emsg_off = save_emsg_off;
	परत रिक्त;
    }

    // If evaluated in a function or autocommand, return the path of the script
    // where it is defined, at script level the current script path is returned
    // instead.
    जर (which == ESTACK_SCRIPT)
    {
	// Walk the stack backwards, starting from the current frame.
	साठी (idx = exestack.ga_len - 1; idx >= 0; --idx, --entry)
	{
	    जर (entry->es_type == ETYPE_UFUNC || entry->es_type == ETYPE_AUCMD)
	    {
		sctx_T *def_ctx = entry->es_type == ETYPE_UFUNC
			      ? &entry->es_info.ufunc->uf_script_ctx
			      : entry->es_info.aucmd != रिक्त
				  ? acp_script_ctx(entry->es_info.aucmd) : रिक्त;

		परत def_ctx != रिक्त && def_ctx->sc_sid > 0
			   ? vim_strsave(SCRIPT_ITEM(def_ctx->sc_sid)->sn_name)
			   : रिक्त;
	    }
	    अन्यथा जर (entry->es_type == ETYPE_SCRIPT)
		परत vim_strsave(entry->es_name);
	}
	परत रिक्त;
    }

    // Give information about each stack entry up to the root.
    // For a function we compose the call stack, as it was done in the past:
    //   "function One[123]..Two[456]..Three"
    ga_init2(&ga, आकार(अक्षर), 100);
    साठी (idx = 0; idx < exestack.ga_len; ++idx)
    {
	entry = ((estack_T *)exestack.ga_data) + idx;
	जर (entry->es_name != रिक्त)
	{
	    दीर्घ	lnum = 0;
	    माप_प्रकार	added = 0;
	    string_T	type_name = {(char_u *)"", 0};
	    string_T	class_name = {(char_u *)"", 0};
	    string_T	es_name = {entry->es_name, STRLEN(entry->es_name)};

	    जर (entry->es_type != last_type)
	    {
		निवड (entry->es_type)
		{
		    स्थिती ETYPE_SCRIPT:
			STR_LITERAL_SET(type_name, "script ");
			थांबा;
		    स्थिती ETYPE_UFUNC:
			STR_LITERAL_SET(type_name, "function ");
			थांबा;
		    पूर्वनिर्धारित:
			थांबा;
		}
		last_type = entry->es_type;
	    }
	    जर (entry->es_type == ETYPE_UFUNC && entry->es_info.ufunc->uf_class != रिक्त)
	    {
		class_name.string = entry->es_info.ufunc->uf_class->class_name.string;
		class_name.length = entry->es_info.ufunc->uf_class->class_name.length;
	    }
	    जर (idx == exestack.ga_len - 1)
		lnum = which == ESTACK_चळत ? SOURCING_LNUM : 0;
	    अन्यथा
		lnum = entry->es_lnum;

	    len = es_name.length + type_name.length + class_name.length + 26;
	    जर (ga_grow(&ga, (पूर्णांक)len) == FAIL)
		थांबा;
	    ga_concat_len(&ga, type_name.string, type_name.length);
	    // For class methods prepend "<class name>." to the function name.
	    जर (*class_name.string != NUL)
	    {
		added = vim_snprintf_safelen(
		    (अक्षर *)ga.ga_data + ga.ga_len,
		    ga.ga_maxlen - ga.ga_len,
		    "<SNR>%d_%s.",
		    entry->es_info.ufunc->uf_script_ctx.sc_sid,
		    class_name.string);

		ga.ga_len += (पूर्णांक)added;
	    }
	    ga_concat_len(&ga, es_name.string, es_name.length);
	    // For the bottom entry of <sfile>: do not add the line number, it is used in
	    // <slnum>.  Also leave it out when the number is not set.
	    जर (lnum != 0)
	    {
		added = vim_snprintf_safelen(
		    (अक्षर *)ga.ga_data + ga.ga_len,
		    ga.ga_maxlen - ga.ga_len,
		    "[%ld]",
		    lnum);

		ga.ga_len += (पूर्णांक)added;
	    }
	    जर (idx != exestack.ga_len - 1)
		GA_CONCAT_LITERAL(&ga, "..");
	}
    }

    // Only NUL-terminate when not returning NULL.
    जर (ga.ga_data != रिक्त)
	ga_append(&ga, NUL);
    परत (char_u *)ga.ga_data;
#endif
}

#ifdef FEAT_EVAL
    स्थिर शून्य
stacktrace_ढकला_item(
	list_T		*l,
	ufunc_T		*fp,
	char_u		*event,
	linenr_T	lnum,
	char_u		*filepath)
{
    dict_T	*d;
    typval_T	tv;

    d = dict_alloc_lock(VAR_FIXED);
    जर (d == रिक्त)
	परत;

    tv.v_type = VAR_DICT;
    tv.v_lock = VAR_LOCKED;
    tv.vval.v_dict = d;

    जर (fp != रिक्त)
	dict_add_func(d, "funcref", fp);
    जर (event != रिक्त)
	dict_add_string(d, "event", event);
    dict_add_संख्या(d, "lnum", lnum);
    dict_add_string(d, "filepath", filepath);

    list_append_tv(l, &tv);
}

/*
 * Create the stacktrace from exestack.
 */
    list_T *
stacktrace_create(शून्य)
{
    list_T	*l;
    पूर्णांक		i;

    l = list_alloc();
    जर (l == रिक्त)
	परत रिक्त;

    साठी (i = 0; i < exestack.ga_len; ++i)
    {
	estack_T *entry = &((estack_T *)exestack.ga_data)[i];
	linenr_T lnum = entry->es_lnum;

	जर (entry->es_type == ETYPE_SCRIPT)
	    stacktrace_ढकला_item(l, रिक्त, रिक्त, lnum, entry->es_name);
	अन्यथा जर (entry->es_type == ETYPE_UFUNC)
	{
	    ufunc_T *fp = entry->es_info.ufunc;
	    sctx_T sctx = fp->uf_script_ctx;
	    char_u *filepath = sctx.sc_sid > 0 ?
				   get_scriptname(sctx.sc_sid) : (char_u *)"";

	    lnum += sctx.sc_lnum;
	    stacktrace_ढकला_item(l, fp, रिक्त, lnum, filepath);
	}
	अन्यथा जर (entry->es_type == ETYPE_AUCMD)
	{
	    // The autocmd may not have a matching pattern yet, in which case
	    // es_info.aucmd is still NULL.
	    sctx_T *sctx = entry->es_info.aucmd != रिक्त
			       ? acp_script_ctx(entry->es_info.aucmd) : रिक्त;
	    char_u *filepath = sctx != रिक्त && sctx->sc_sid > 0 ?
				   get_scriptname(sctx->sc_sid) : (char_u *)"";

	    जर (sctx != रिक्त)
		lnum += sctx->sc_lnum;
	    stacktrace_ढकला_item(l, रिक्त, entry->es_name, lnum, filepath);
	}
    }
    परत l;
}

/*
 * getstacktrace() function
 */
    शून्य
f_getstacktrace(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv_list_set(rettv, stacktrace_create());
}
#endif

/*
 * Get DIP_ flags from the [where] argument of a :runtime command.
 * "*argp" is advanced to after the [where] argument if it is found.
 */
    स्थिर पूर्णांक
get_runtime_cmd_flags(char_u **argp, माप_प्रकार where_len)
{
    char_u *arg = *argp;

    जर (where_len == 0)
	परत 0;

    जर (STRNCMP(arg, "START", where_len) == 0)
    {
	*argp = skipwhite(arg + where_len);
	परत DIP_START + DIP_NORTP;
    }
    जर (STRNCMP(arg, "OPT", where_len) == 0)
    {
	*argp = skipwhite(arg + where_len);
	परत DIP_OPT + DIP_NORTP;
    }
    जर (STRNCMP(arg, "PACK", where_len) == 0)
    {
	*argp = skipwhite(arg + where_len);
	परत DIP_START + DIP_OPT + DIP_NORTP;
    }
    जर (STRNCMP(arg, "ALL", where_len) == 0)
    {
	*argp = skipwhite(arg + where_len);
	परत DIP_START + DIP_OPT;
    }

    परत 0;
}

/*
 * ":runtime [where] {name}"
 */
    शून्य
ex_runtime(exarg_T *eap)
{
    char_u  *arg = eap->arg;
    पूर्णांक	    flags = eap->forceit ? DIP_ALL : 0;
    char_u  *p = skiptowhite(arg);
    flags += get_runtime_cmd_flags(&arg, p - arg);
    source_runtime(arg, flags);
}

स्थिर पूर्णांक runtime_expand_flags;

/*
 * Set the completion context for the :runtime command.
 */
    शून्य
set_context_in_runtime_cmd(expand_T *xp, char_u *arg)
{
    char_u  *p = skiptowhite(arg);
    runtime_expand_flags
	= *p != NUL ? get_runtime_cmd_flags(&arg, p - arg) : 0;
    // Skip to the last argument.
    पर्यंत (*(p = skiptowhite_esc(arg)) != NUL)
    {
	जर (runtime_expand_flags == 0)
	    // When there are multiple arguments and [where] is not specified,
	    // use an unrelated non-zero flag to avoid expanding [where].
	    runtime_expand_flags = DIP_ALL;
	arg = skipwhite(p);
    }
    xp->xp_context = EXPAND_RUNTIME;
    xp->xp_pattern = arg;
}

    स्थिर शून्य
source_callback(char_u *fname, शून्य *cookie)
{
    (शून्य)do_source(fname, FALSE, DOSO_NONE, cookie);
}

#ifdef FEAT_EVAL
/*
 * Find an already loaded script "name".
 * If found returns its script ID.  If not found returns -1.
 */
    पूर्णांक
find_script_by_name(char_u *name)
{
    पूर्णांक		    sid;
    scriptitem_T    *si;

    साठी (sid = script_वस्तुसंच.ga_len; sid > 0; --sid)
    {
	// We used to check inode here, but that doesn't work:
	// - If a script is edited and written, it may get a different
	//   inode number, even though to the user it is the same script.
	// - If a script is deleted and another script is written, with a
	//   different name, the inode may be re-used.
	si = SCRIPT_ITEM(sid);
	जर (si->sn_name != रिक्त && fnamecmp(si->sn_name, name) == 0)
	    परत sid;
    }
    परत -1;
}

/*
 * Add a new scriptitem with all items initialized.
 * When running out of memory "error" is set to FAIL.
 * Returns the script ID.
 */
    स्थिर पूर्णांक
get_new_scriptitem(पूर्णांक *error)
{
    स्थिर scid_T   last_current_SID = 0;
    पूर्णांक		    sid = ++last_current_SID;
    scriptitem_T    *si = रिक्त;

    जर (ga_grow(&script_वस्तुसंच, (पूर्णांक)(sid - script_वस्तुसंच.ga_len)) == FAIL)
    {
	*error = FAIL;
	परत sid;
    }
    पर्यंत (script_वस्तुसंच.ga_len < sid)
    {
	si = ALLOC_CLEAR_ONE(scriptitem_T);
	जर (si == रिक्त)
	{
	    *error = FAIL;
	    परत sid;
	}
	++script_वस्तुसंच.ga_len;
	SCRIPT_ITEM(script_वस्तुसंच.ga_len) = si;
	si->sn_name = रिक्त;
	si->sn_version = 1;

	// Allocate the local script variables to use for this script.
	new_script_vars(script_वस्तुसंच.ga_len);
	ga_init2(&si->sn_var_vals, आकार(svar_T), 10);
	hash_init(&si->sn_all_vars.dv_hashtab);
	ga_init2(&si->sn_imports, आकार(imported_T), 10);
	ga_init2(&si->sn_type_list, आकार(type_T), 10);
# ifdef FEAT_PROFILE
	si->sn_prof_on = FALSE;
# endif
    }

    // "si" can't be NULL, check only to avoid a compiler warning
    जर (si != रिक्त)
	// Used to check script variable index is still valid.
	si->sn_script_seq = current_sctx.sc_seq;

    परत sid;
}

    पूर्णांक
get_new_scriptitem_for_fname(पूर्णांक *error, char_u *fname)
{
    पूर्णांक sid = get_new_scriptitem(error);

    जर (*error == OK)
    {
	scriptitem_T *si = SCRIPT_ITEM(sid);

	si->sn_name = vim_strsave(fname);
	si->sn_state = SN_STATE_NOT_LOADED;
    }
    परत sid;
}

/*
 * If the script for "sid" is a symlink and "sn_source_sid" is not set
 * then initialize it. A new script_item is created if needed.
 */
    शून्य
check_script_symlink(पूर्णांक sid)
{
    scriptitem_T *si = SCRIPT_ITEM(sid);
    जर (si->sn_syml_checked || si->sn_sourced_sid > 0)
	परत;
    si->sn_syml_checked = TRUE;

    // If fname is a symbolic link, create an script_item for the real file.

    char_u *real_fname = fix_fname(si->sn_name);
    जर (real_fname != रिक्त && STRCMP(real_fname, si->sn_name) != 0)
    {
	पूर्णांक real_sid = find_script_by_name(real_fname);
	पूर्णांक error2 = OK;
	पूर्णांक new_sid = FALSE;
	जर (real_sid < 0)
	{
	    real_sid = get_new_scriptitem_for_fname(&error2, real_fname);
	    new_sid = TRUE;
	}
	जर (error2 == OK)
	{
	    si = SCRIPT_ITEM(sid);
	    si->sn_sourced_sid = real_sid;
	    जर (new_sid)
	    {
		SCRIPT_ITEM(real_sid)->sn_import_autoload
						    = si->sn_import_autoload;
		जर (si->sn_autoload_prefix != रिक्त)
		    SCRIPT_ITEM(real_sid)->sn_autoload_prefix =
					vim_strsave(si->sn_autoload_prefix);
	    }
	}
    }
    vim_free(real_fname);
}

    स्थिर शून्य
find_script_callback(char_u *fname, शून्य *cookie)
{
    पूर्णांक sid;
    पूर्णांक error = OK;
    पूर्णांक *ret_sid = cookie;

    sid = find_script_by_name(fname);
    जर (sid < 0)
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
    पूर्णांक
do_in_path(
    char_u	*path,
    अक्षर	*prefix,
    char_u	*name,
    पूर्णांक		flags,
    शून्य	(*callback)(char_u *fname, शून्य *ck),
    शून्य	*cookie)
{
    char_u	*rtp;
    string_T	buf;
    char_u	*rtp_copy;
    पूर्णांक		अंक_files;
    char_u	**files;
    पूर्णांक		i;
    पूर्णांक		did_one = FALSE;
#ifdef AMIGA
    संरचना Process	*proc = (संरचना Process *)FindTask(0L);
    APTR		save_winptr = proc->pr_WindowPtr;

    // Avoid a requester here for a volume that doesn't exist.
    proc->pr_WindowPtr = (APTR)-1L;
#endif

    // Make a copy of 'runtimepath'.  Invoking the callback may change the
    // value.
    rtp_copy = vim_strsave(path);
    buf.string = alloc(MAXPATHL);
    जर (buf.string != रिक्त && rtp_copy != रिक्त)
    {
	माप_प्रकार	prefixlen = 0;
	माप_प्रकार	namelen = 0;

	जर (name != रिक्त)
	{
	    prefixlen = STRLEN(prefix);
	    namelen = STRLEN(name);

	    जर (p_verbose > 10)
	    {
		verbose_enter();
		जर (*prefix != NUL)
		    smsg(_("Searching for \"%s\" under \"%s\" in \"%s\""),
					       (अक्षर *)name, prefix, (अक्षर *)path);
		अन्यथा
		    smsg(_("Searching for \"%s\" in \"%s\""),
						       (अक्षर *)name, (अक्षर *)path);
		verbose_leave();
	    }
	}

	// Loop over all entries in 'runtimepath'.
	rtp = rtp_copy;
	पर्यंत (*rtp != NUL && ((flags & DIP_ALL) || !did_one))
	{
	    // Copy the path from 'runtimepath' to buf[].
	    buf.length = (माप_प्रकार)copy_option_part(&rtp, buf.string, MAXPATHL, ",");

	    // Skip after or non-after directories.
	    जर (flags & (DIP_NOAFTER | DIP_AFTER))
	    {
		पूर्णांक is_after = buf.length >= 5
		    && STRCMP(buf.string + buf.length - 5, "after") == 0;

		जर ((is_after && (flags & DIP_NOAFTER))
			|| (!is_after && (flags & DIP_AFTER)))
		    सुरूठेवा;
	    }

	    जर (name == रिक्त)
	    {
		(*callback)(buf.string, (शून्य *)&cookie);
		जर (!did_one)
		    did_one = (cookie == रिक्त);
	    }
	    अन्यथा जर (buf.length + 2 + prefixlen + namelen < MAXPATHL)
	    {
		char_u	*np;
		char_u	*tail;

		जर (*buf.string != NUL
		    && !after_pathsep(buf.string, buf.string + buf.length))
		{
		    STRCPY(buf.string + buf.length, PATHSEPSTR);
		    buf.length += आकार(PATHSEP);
		}
		STRCPY(buf.string + buf.length, prefix);
		buf.length += prefixlen;
		tail = buf.string + buf.length;

		// Loop over all patterns in "name"
		np = name;
		पर्यंत (*np != NUL && ((flags & DIP_ALL) || !did_one))
		{
		    // Append the pattern from "name" to buf[].
		    copy_option_part(&np, tail, (पूर्णांक)(MAXPATHL - (tail - buf.string)),
								       "\t ");

		    जर (p_verbose > 10)
		    {
			verbose_enter();
			smsg(_("Searching for \"%s\""), buf.string);
			verbose_leave();
		    }

		    // Expand wildcards, invoke the callback for each match.
		    जर (gen_expand_wildcards(1, &buf.string, &अंक_files, &files,
				  (flags & DIP_DIR) ? EW_DIR : EW_FILE) == OK)
		    {
			साठी (i = 0; i < अंक_files; ++i)
			{
			    (*callback)(files[i], cookie);
			    did_one = TRUE;
			    जर (!(flags & DIP_ALL))
				थांबा;
			}
			FreeWild(अंक_files, files);
		    }
		}
	    }
	}
    }
    vim_free(buf.string);
    vim_free(rtp_copy);
    जर (!did_one && name != रिक्त)
    {
	अक्षर *basepath = path == p_rtp ? "runtimepath" : "packpath";

	जर (flags & DIP_ERR)
	    semsg(_(e_directory_not_found_in_str_str), basepath, name);
	अन्यथा जर (p_verbose > 0)
	{
	    verbose_enter();
	    smsg(_("not found in '%s': \"%s\""), basepath, name);
	    verbose_leave();
	}
    }

#ifdef AMIGA
    proc->pr_WindowPtr = save_winptr;
#endif

    परत did_one ? OK : FAIL;
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
    स्थिर पूर्णांक
do_in_path_and_pp(
    char_u	*path,
    char_u	*name,
    पूर्णांक		flags,
    शून्य	(*callback)(char_u *fname, शून्य *ck),
    शून्य	*cookie)
{
    पूर्णांक		done = FAIL;

    जर ((flags & DIP_NORTP) == 0)
	done = do_in_path(path, "", name, flags, callback, cookie);

    जर ((done == FAIL || (flags & DIP_ALL)) && (flags & DIP_START))
	done = do_in_path(p_pp, "pack/*/start/*/", name, flags, callback,
								       cookie);

    जर ((done == FAIL || (flags & DIP_ALL)) && (flags & DIP_OPT))
	done = do_in_path(p_pp, "pack/*/opt/*/", name, flags, callback,
								       cookie);

    परत done;
}

/*
 * Just like do_in_path_and_pp(), using 'runtimepath' for "path".
 */
    पूर्णांक
do_in_runtimepath(
    char_u	*name,
    पूर्णांक		flags,
    शून्य	(*callback)(char_u *fname, शून्य *ck),
    शून्य	*cookie)
{
    परत do_in_path_and_pp(p_rtp, name, flags, callback, cookie);
}

/*
 * Source the file "name" from all directories in 'runtimepath'.
 * "name" can contain wildcards.
 * When "flags" has DIP_ALL: source all files, otherwise only the first one.
 *
 * return FAIL when no file could be sourced, OK otherwise.
 */
    पूर्णांक
source_runtime(char_u *name, पूर्णांक flags)
{
    परत source_in_path(p_rtp, name, flags, रिक्त);
}

/*
 * Just like source_runtime(), but use "path" instead of 'runtimepath'
 * and return the script ID in "ret_sid".
 */
    पूर्णांक
source_in_path(char_u *path, char_u *name, पूर्णांक flags, पूर्णांक *ret_sid)
{
    परत do_in_path_and_pp(path, name, flags, source_callback, ret_sid);
}

#जर defined(FEAT_EVAL)

/*
 * Find "name" in 'runtimepath'. If found a new scriptitem is created for it
 * and it's script ID is returned.
 * If not found returns -1.
 */
    पूर्णांक
find_script_in_rtp(char_u *name)
{
    पूर्णांक sid = -1;

    (शून्य)do_in_path_and_pp(p_rtp, name, DIP_START | DIP_NOAFTER,
						   find_script_callback, &sid);
    परत sid;
}

/*
 * Expand wildcards in "pat" and invoke do_source() for each match.
 */
    स्थिर शून्य
source_all_matches(char_u *pat)
{
    पूर्णांक	    अंक_files;
    char_u  **files;
    पूर्णांक	    i;

    जर (gen_expand_wildcards(1, &pat, &अंक_files, &files, EW_FILE) != OK)
	परत;

    साठी (i = 0; i < अंक_files; ++i)
	(शून्य)do_source(files[i], FALSE, DOSO_NONE, रिक्त);
    FreeWild(अंक_files, files);
}

/*
 * Add the package directory to 'runtimepath'.
 */
    स्थिर पूर्णांक
add_pack_dir_to_rtp(char_u *fname)
{
    char_u  *p4, *p3, *p2, *p1, *p;
    char_u  *entry;
    char_u  *insp = रिक्त;
    पूर्णांक	    c;
    string_T  new_rtp;
    माप_प्रकार  keep;
    माप_प्रकार  p_rtp_len;
    string_T  afterdir = {रिक्त, 0};
    char_u  *after_insp = रिक्त;
    char_u  *ffname = रिक्त;
    माप_प्रकार  fname_len;
    string_T	buf = {रिक्त, 0};
    पूर्णांक	    retval = FAIL;

    p4 = p3 = p2 = p1 = get_past_head(fname);
    साठी (p = p1; *p; MB_PTR_ADV(p))
	जर (vim_ispathsep_nocolon(*p))
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
    जर (ffname == रिक्त)
	परत FAIL;

    // Find "ffname" in "p_rtp", ignoring '/' vs '\' differences.
    // Also stop at the first "after" directory.
    fname_len = STRLEN(ffname);
    buf.string = alloc(MAXPATHL);
    जर (buf.string == रिक्त)
	जा theend;

    p_rtp_len = 0;
    साठी (entry = p_rtp; *entry != NUL; )
    {
	char_u *cur_entry = entry;

	buf.length = (माप_प्रकार)copy_option_part(&entry, buf.string, MAXPATHL, ",");

	// keep track of p_rtp length as we go to make the STRLEN() below have less work to do
	p_rtp_len += (*(cur_entry + buf.length) == ',') ? buf.length + 1 : buf.length;

	जर ((p = (char_u *)म_माला((अक्षर *)buf.string, "after")) != रिक्त
		&& p > buf.string
		&& vim_ispathsep(p[-1])
		&& (vim_ispathsep(p[5]) || p[5] == NUL || p[5] == ','))
	{
	    जर (insp == रिक्त)
		// Did not find "ffname" before the first "after" directory,
		// insert it before this entry.
		insp = cur_entry;
	    after_insp = cur_entry;
	    थांबा;
	}

	जर (insp == रिक्त)
	{
	    char_u  *rtp_ffname;
	    पूर्णांक	    match;

	    जर (*buf.string != NUL && !after_pathsep(buf.string, buf.string + buf.length))
		STRCPY(buf.string + buf.length, PATHSEPSTR);
	    rtp_ffname = fix_fname(buf.string);
	    जर (rtp_ffname == रिक्त)
		जा theend;
	    match = vim_fnamencmp(rtp_ffname, ffname, fname_len) == 0;
	    vim_free(rtp_ffname);
	    जर (match)
		// Insert "ffname" after this entry (and comma).
		insp = entry;
	}
    }

    // finish measuring the length of p_rtp
    p_rtp_len += STRLEN(p_rtp + p_rtp_len);
    जर (insp == रिक्त)
	// Both "fname" and "after" not found, append at the end.
	insp = p_rtp + p_rtp_len;

    // check if rtp/pack/name/start/name/after exists
    fname_len = STRLEN(fname);
    concat_fnames(fname, fname_len, (char_u *)"after", STRLEN_LITERAL("after"), TRUE, &afterdir);
    जर (afterdir.string == रिक्त || !mch_isdir(afterdir.string))
	afterdir.length = 0;

    new_rtp.string = alloc(p_rtp_len + fname_len + afterdir.length + 3); // add two for commas and one for NUL
    जर (new_rtp.string == रिक्त)
	जा theend;

    // We now have 'rtp' parts: {keep}{keep_after}{rest}.
    // Create new_rtp, first: {keep},{fname}
    keep = (माप_प्रकार)(insp - p_rtp);
    mch_memmove(new_rtp.string, p_rtp, keep);
    new_rtp.length = keep;
    जर (*insp == NUL)
	new_rtp.string[new_rtp.length++] = ',';  // add comma before
    mch_memmove(new_rtp.string + new_rtp.length, fname, fname_len);
    new_rtp.length += fname_len;
    जर (*insp != NUL)
	new_rtp.string[new_rtp.length++] = ',';  // add comma after

    जर (afterdir.length > 0 && after_insp != रिक्त)
    {
	माप_प्रकार keep_after = (माप_प्रकार)(after_insp - p_rtp);
	माप_प्रकार append_len = keep_after - keep;

	// Add to new_rtp: {keep},{fname}{keep_after},{afterdir}
	mch_memmove(new_rtp.string + new_rtp.length, p_rtp + keep,
							append_len);
	new_rtp.length += append_len;
	mch_memmove(new_rtp.string + new_rtp.length, afterdir.string, afterdir.length);
	new_rtp.length += afterdir.length;
	new_rtp.string[new_rtp.length++] = ',';
	keep = keep_after;
    }

    जर (p_rtp[keep] != NUL)
    {
	माप_प्रकार	append_len = p_rtp_len - keep;

	// Append rest: {keep},{fname}{keep_after},{afterdir}{rest}
	mch_memmove(new_rtp.string + new_rtp.length, p_rtp + keep, append_len + 1);	// add one for NUL
	new_rtp.length += append_len;
    }
    अन्यथा
	new_rtp.string[new_rtp.length] = NUL;

    जर (afterdir.length > 0 && after_insp == रिक्त)
    {
	// Append afterdir when "after" was not found:
	// {keep},{fname}{rest},{afterdir}
	new_rtp.string[new_rtp.length++] = ',';
	STRCPY(new_rtp.string + new_rtp.length, afterdir.string);
    }

    set_option_मूल्य_give_err((char_u *)"rtp", 0L, new_rtp.string, 0);
    vim_free(new_rtp.string);
    retval = OK;

theend:
    vim_free(buf.string);
    vim_free(ffname);
    vim_free(afterdir.string);
    परत retval;
}

/*
 * Load scripts in "plugin" and "ftdetect" directories of the package.
 */
    स्थिर पूर्णांक
load_pack_plugin(char_u *fname)
{
    स्थिर अक्षर *plugpat = "%s/plugin/**/*.vim";
    स्थिर अक्षर *ftpat = "%s/ftdetect/*.vim";
    पूर्णांक		len;
    char_u	*ffname = fix_fname(fname);
    char_u	*pat = रिक्त;
    पूर्णांक		retval = FAIL;

    जर (ffname == रिक्त)
	परत FAIL;
    len = (पूर्णांक)STRLEN(ffname) + (पूर्णांक)STRLEN(ftpat);
    pat = alloc(len);
    जर (pat == रिक्त)
	जा theend;
    vim_snprintf((अक्षर *)pat, len, plugpat, ffname);
    source_all_matches(pat);

    {
	char_u *cmd = vim_strsave((char_u *)"g:did_load_filetypes");

	// If runtime/filetype.vim wasn't loaded yet, the scripts will be
	// found when it loads.
	जर (cmd != रिक्त && eval_to_संख्या(cmd, FALSE) > 0)
	{
	    do_cmdline_cmd((char_u *)"augroup filetypedetect");
	    vim_snprintf((अक्षर *)pat, len, ftpat, ffname);
	    source_all_matches(pat);
	    do_cmdline_cmd((char_u *)"augroup END");
	}
	vim_free(cmd);
    }
    vim_free(pat);
    retval = OK;

theend:
    vim_free(ffname);
    परत retval;
}

// used for "cookie" of add_pack_plugin()
स्थिर पूर्णांक APP_ADD_DIR;
स्थिर पूर्णांक APP_LOAD;
स्थिर पूर्णांक APP_BOTH;

    स्थिर शून्य
add_pack_plugin(char_u *fname, शून्य *cookie)
{
    जर (cookie != &APP_LOAD)
    {
	char_u	*buf = alloc(MAXPATHL);
	char_u	*p;
	पूर्णांक	found = FALSE;

	जर (buf == रिक्त)
	    परत;
	p = p_rtp;
	पर्यंत (*p != NUL)
	{
	    copy_option_part(&p, buf, MAXPATHL, ",");
	    जर (pathcmp((अक्षर *)buf, (अक्षर *)fname, -1) == 0)
	    {
		found = TRUE;
		थांबा;
	    }
	}
	vim_free(buf);
	जर (!found)
	    // directory is not yet in 'runtimepath', add it
	    जर (add_pack_dir_to_rtp(fname) == FAIL)
		परत;
    }

    जर (cookie != &APP_ADD_DIR)
	load_pack_plugin(fname);
}

/*
 * Add all packages in the "start" directory to 'runtimepath'.
 */
    शून्य
add_pack_start_dirs(शून्य)
{
    do_in_path(p_pp, "", (char_u *)"pack/*/start/*", DIP_ALL + DIP_DIR,
					       add_pack_plugin, &APP_ADD_DIR);
}

/*
 * Load plugins from all packages in the "start" directory.
 */
    शून्य
load_start_packages(शून्य)
{
    did_source_packages = TRUE;
    do_in_path(p_pp, "", (char_u *)"pack/*/start/*", DIP_ALL + DIP_DIR,
						  add_pack_plugin, &APP_LOAD);
}

/*
 * ":packloadall"
 * Find plugins in the package directories and source them.
 */
    शून्य
ex_packloadall(exarg_T *eap)
{
    जर (!did_source_packages || eap->forceit)
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
    शून्य
ex_packadd(exarg_T *eap)
{
    स्थिर अक्षर *plugpat = "pack/*/%s/%s";
    पूर्णांक		len;
    अक्षर	*pat;
    पूर्णांक		round;
    पूर्णांक		res = OK;

    // Round 1: use "start", round 2: use "opt".
    साठी (round = 1; round <= 2; ++round)
    {
	// Only look under "start" when loading packages wasn't done yet.
	जर (round == 1 && did_source_packages)
	    सुरूठेवा;

	len = (पूर्णांक)STRLEN(plugpat) + (पूर्णांक)STRLEN(eap->arg) + 5;
	pat = alloc(len);
	जर (pat == रिक्त)
	    परत;
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
    शून्य
remove_duplicates(garray_T *gap)
{
    पूर्णांक	    i;
    पूर्णांक	    j;
    char_u  **fnames = (char_u **)gap->ga_data;

    sort_strings(fnames, gap->ga_len);
    साठी (i = gap->ga_len - 1; i > 0; --i)
	जर (fnamecmp(fnames[i - 1], fnames[i]) == 0)
	{
	    vim_free(fnames[i]);
	    साठी (j = i + 1; j < gap->ga_len; ++j)
		fnames[j - 1] = fnames[j];
	    --gap->ga_len;
	}
}

    स्थिर शून्य
ExpandRTDir_int(
    char_u	*pat,
    माप_प्रकार	pat_len,
    पूर्णांक		flags,
    पूर्णांक		keep_ext,
    garray_T	*gap,
    अक्षर	*dirnames[])
{
    साठी (पूर्णांक i = 0; dirnames[i] != रिक्त; ++i)
    {
	स्थिरांक माप_प्रकार	buf_len = STRLEN(dirnames[i]) + pat_len + 64;
	अक्षर		*buf = alloc(buf_len);
	जर (buf == रिक्त)
	{
	    ga_clear_strings(gap);
	    परत;
	}
	पूर्णांक		glob_flags = 0;
	पूर्णांक		expand_dirs = FALSE;

	// Build base pattern
	vim_snprintf(buf, buf_len, "%s%s%s%s",
		     *dirnames[i] ? dirnames[i] : "", *dirnames[i] ? "/" : "",
		     pat, "*.vim");

expand:
	जर ((flags & DIP_NORTP) == 0)
	    globpath(p_rtp, (char_u *)buf, gap, glob_flags, expand_dirs);

	जर (flags & DIP_START)
	{
	    // Build complete search path: pack/*/start/*/dirnames[i]/pat*.vim
	    vim_snprintf(buf, buf_len, "pack/*/start/*/%s%s%s%s",
			 *dirnames[i] ? dirnames[i] : "",
			 *dirnames[i] ? "/" : "",
			 pat,
			 expand_dirs ? "*" : "*.vim");
	    globpath(p_pp, (char_u *)buf, gap, glob_flags, expand_dirs);
	}

	जर (flags & DIP_OPT)
	{
	    // Build complete search path: pack/*/opt/*/dirnames[i]/pat*.vim
	    vim_snprintf(buf, buf_len, "pack/*/opt/*/%s%s%s%s",
			 *dirnames[i] ? dirnames[i] : "",
			 *dirnames[i] ? "/" : "", pat,
			 expand_dirs ? "*" : "*.vim");
	    globpath(p_pp, (char_u *)buf, gap, glob_flags, expand_dirs);
	}

	// Second round for directories
	जर (*dirnames[i] == NUL && !expand_dirs)
	{
	    // expand dir names in another round
	    vim_snprintf(buf, buf_len, "%s*", pat);
	    glob_flags = WILD_ADD_SLASH;
	    expand_dirs = TRUE;
	    जा expand;
	}

	vim_free(buf);
    }

    पूर्णांक pat_pathsep_cnt = 0;
    साठी (माप_प्रकार i = 0; i < pat_len; ++i)
    {
	जर (vim_ispathsep(pat[i]))
	    ++pat_pathsep_cnt;
    }

    साठी (पूर्णांक i = 0; i < gap->ga_len; ++i)
    {
	char_u *match = ((char_u **)gap->ga_data)[i];
	char_u *s = match;
	char_u *e = s + STRLEN(s);
	जर (e - 4 > s && !keep_ext && STRNICMP(e - 4, ".vim", 4) == 0)
	{
	    e -= 4;
	    *e = NUL;
	}

	पूर्णांक match_pathsep_cnt = (e > s && e[-1] == '/') ? -1 : 0;
	साठी (s = e; s > match; MB_PTR_BACK(match, s))
	{
	    जर (s < match || (vim_ispathsep(*s)
				     && ++match_pathsep_cnt > pat_pathsep_cnt))
		थांबा;
	}
	++s;
	जर (s != match)
	    mch_memmove(match, s, e - s + 1);
    }

    जर (gap->ga_len == 0)
	परत;

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
    पूर्णांक
ExpandRTDir(
    char_u	*pat,
    पूर्णांक		flags,
    पूर्णांक		*अंक_file,
    char_u	***file,
    अक्षर	*dirnames[])
{
    *अंक_file = 0;
    *file = रिक्त;

    garray_T	ga;
    ga_init2(&ga, आकार(अक्षर *), 10);

    ExpandRTDir_int(pat, STRLEN(pat), flags, FALSE, &ga, dirnames);

    जर (ga.ga_len == 0)
	परत FAIL;

    *file = ga.ga_data;
    *अंक_file = ga.ga_len;
    परत OK;
}

/*
 * Handle command line completion for :runtime command.
 */
    पूर्णांक
expand_runtime_cmd(char_u *pat, पूर्णांक *अंक_Matches, char_u ***matches)
{
    *अंक_Matches = 0;
    *matches = रिक्त;

    garray_T	ga;
    ga_init2(&ga, आकार(अक्षर *), 10);

    माप_प्रकार pat_len = (पूर्णांक)STRLEN(pat);
    अक्षर *dirnames[] = {"", रिक्त};
    ExpandRTDir_int(pat, pat_len, runtime_expand_flags, TRUE, &ga, dirnames);

    // Try to complete values for [where] argument when none was found.
    जर (runtime_expand_flags == 0)
    {
	अक्षर *where_मूल्ये[] = {"START", "OPT", "PACK", "ALL"};
	साठी (माप_प्रकार i = 0; i < ARRAY_LENGTH(where_मूल्ये); ++i)
	    जर (STRNCMP(pat, where_मूल्ये[i], pat_len) == 0)
	    {
		char_u *p = vim_strsave((char_u *)where_मूल्ये[i]);
		जर (p != रिक्त && ga_add_string(&ga, p) == FAIL)
		    vim_free(p);
	    }
    }

    जर (ga.ga_len == 0)
	परत FAIL;

    *matches = ga.ga_data;
    *अंक_Matches = ga.ga_len;
    परत OK;
}

/*
 * Expand loadplugin names:
 * 'packpath'/pack/ * /opt/{pat}
 */
    पूर्णांक
ExpandPackAddDir(
    char_u	*pat,
    पूर्णांक		*अंक_file,
    char_u	***file)
{
    char_u	*s;
    char_u	*e;
    char_u	*match;
    garray_T	ga;
    पूर्णांक		i;
    पूर्णांक		pat_len;

    *अंक_file = 0;
    *file = रिक्त;
    pat_len = (पूर्णांक)STRLEN(pat);
    ga_init2(&ga, आकार(अक्षर *), 10);

    s = alloc(pat_len + 26);
    जर (s == रिक्त)
    {
	ga_clear_strings(&ga);
	परत FAIL;
    }
    प्र_लिखो((अक्षर *)s, "pack/*/opt/%s*", pat);
    globpath(p_pp, s, &ga, 0, TRUE);
    vim_free(s);

    साठी (i = 0; i < ga.ga_len; ++i)
    {
	match = ((char_u **)ga.ga_data)[i];
	s = gettail(match);
	e = s + STRLEN(s);
	mch_memmove(match, s, e - s + 1);
    }

    जर (ga.ga_len == 0)
	परत FAIL;

    // Sort and remove duplicates which can happen when specifying multiple
    // directories in dirnames.
    remove_duplicates(&ga);

    *file = ga.ga_data;
    *अंक_file = ga.ga_len;
    परत OK;
}

    स्थिर शून्य
cmd_source(char_u *fname, exarg_T *eap)
{
    पूर्णांक clearvars = FALSE;

    जर (*fname != NUL && STRNCMP(fname, "++clear", 7) == 0)
    {
	// ++clear argument is supplied
	clearvars = TRUE;
	fname = fname + 7;
	जर (*fname != NUL)
	{
	    semsg(_(e_invalid_argument_str), eap->arg);
	    परत;
	}
    }

    जर (*fname != NUL && eap != रिक्त && eap->addr_मोजणी > 0)
    {
	// if a filename is specified to :source, then a range is not allowed
	emsg(_(e_no_range_allowed));
	परत;
    }

    जर (eap != रिक्त && *fname == NUL)
    {
	जर (eap->forceit)
	    // a file name is needed to source normal mode commands
	    emsg(_(e_argument_required));
	अन्यथा
	    // source ex commands from the current buffer
	    do_source_ext(रिक्त, FALSE, DOSO_NONE, रिक्त, eap, clearvars);
    }
    अन्यथा जर (eap != रिक्त && eap->forceit)
	// ":source!": read Normal mode commands
	// Need to execute the commands directly.  This is required at least
	// for:
	// - ":g" command busy
	// - after ":argdo", ":windo" or ":bufdo"
	// - another command follows
	// - inside a loop
	openscript(fname, global_busy || listcmd_busy || eap->nextcmd != रिक्त
#ifdef FEAT_EVAL
						 || eap->cstack->cs_idx >= 0
#endif
						 );

    // ":source" read ex commands
    अन्यथा जर (do_source(fname, FALSE, DOSO_NONE, रिक्त) == FAIL)
	semsg(_(e_cant_open_file_str), fname);
}

/*
 * ":source {fname}"
 */
    शून्य
ex_source(exarg_T *eap)
{
#ifdef FEAT_BROWSE
    जर (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	char_u *fname = रिक्त;

	fname = do_browse(0, (char_u *)_("Source Vim script"), eap->arg,
				      रिक्त, रिक्त,
				      (char_u *)_(BROWSE_FILTER_MACROS), रिक्त);
	जर (fname != रिक्त)
	{
	    cmd_source(fname, eap);
	    vim_free(fname);
	}
    }
    अन्यथा
#endif
	cmd_source(eap->arg, eap);
}

#जर defined(FEAT_EVAL)
/*
 * ":options"
 */
    शून्य
ex_options(
    exarg_T	*eap UNUSED)
{
    char_u  buf[500];
    पूर्णांक	    multi_mods = 0;

    buf[0] = NUL;
    (शून्य)add_win_cmd_modifiers(buf, &cmdmod, &multi_mods);

    vim_setenv((char_u *)"OPTWIN_CMD", buf);
    cmd_source((char_u *)SYS_OPTWIN_FILE, रिक्त);
}
#endif

/*
 * ":source" and associated commands.
 */

#जर defined(FEAT_EVAL)
/*
 * Return the address holding the next breakpoint line for a source cookie.
 */
    linenr_T *
source_breakpoint(शून्य *cookie)
{
    परत &((source_cookie_T *)cookie)->breakpoint;
}

/*
 * Return the address holding the debug tick for a source cookie.
 */
    पूर्णांक *
source_dbg_tick(शून्य *cookie)
{
    परत &((source_cookie_T *)cookie)->dbg_tick;
}

/*
 * Return the nesting level for a source cookie.
 */
    पूर्णांक
source_level(शून्य *cookie)
{
    परत ((source_cookie_T *)cookie)->level;
}

/*
 * Return the readahead line. Note that the pointer may become invalid when
 * getting the next line, if it's concatenated with the next one.
 */
    char_u *
source_nextline(शून्य *cookie)
{
    परत ((source_cookie_T *)cookie)->nextline;
}
#endif

#जर (defined(MSWIN) && defined(FEAT_CSCOPE)) || defined(HAVE_FD_CLOEXEC)
# define USE_FOPEN_NOINH
/*
 * Special function to open a file without handle inheritance.
 * When possible the handle is closed on exec().
 */
    स्थिर खाता *
fopen_noinh_readbin(अक्षर *filename)
{
# ifdef MSWIN
    पूर्णांक	fd_tmp = mch_open(filename, O_RDONLY | O_द्विमान | O_NOINHERIT, 0);
# अन्यथा
    पूर्णांक	fd_tmp = mch_open(filename, O_RDONLY, 0);
# endif

    जर (fd_tmp == -1)
	परत रिक्त;

# ifdef HAVE_FD_CLOEXEC
    {
	पूर्णांक fdflags = fcntl(fd_tmp, F_GETFD);
	जर (fdflags >= 0 && (fdflags & FD_CLOEXEC) == 0)
	    (शून्य)fcntl(fd_tmp, F_SETFD, fdflags | FD_CLOEXEC);
    }
# endif

    परत fdopen(fd_tmp, READBIN);
}
#endif

/*
 * Initialization for sourcing lines from the current buffer. Reads all the
 * lines from the buffer and stores it in the cookie grow array.
 * Returns a pointer to the name ":source buffer=<n>" on success and NULL on
 * failure.
 */
    स्थिर char_u *
do_source_buffer_init(source_cookie_T *sp, exarg_T *eap)
{
    linenr_T	curr_lnum;
    char_u	*ओळ = रिक्त;
    char_u	*fname;

    जर (curbuf == रिक्त)
	परत रिक्त;

    // Use ":source buffer=<num>" as the script name
    जर (curbuf->b_ffname != रिक्त)
	fname = vim_strsave(curbuf->b_ffname);
    अन्यथा
    {
	vim_snprintf((अक्षर *)IObuff, IOSIZE, ":source buffer=%d", curbuf->b_fnum);
	fname = vim_strsave(IObuff);
    }
    जर (fname == रिक्त)
	परत रिक्त;

    ga_init2(&sp->buflines, आकार(char_u *), 100);

    // Copy the lines from the buffer into a grow array
    साठी (curr_lnum = eap->line1; curr_lnum <= eap->line2; curr_lnum++)
    {
	ओळ = vim_strsave(ml_get(curr_lnum));
	जर (ओळ == रिक्त)
	    जा errret;
	जर (ga_add_string(&sp->buflines, ओळ) == FAIL)
	    जा errret;
    }
    sp->buf_lnum = 0;
    sp->source_from_buf = TRUE;
    // When sourcing a range of lines from a buffer, use buffer line number.
    sp->sourcing_lnum = eap->line1 - 1;

    परत fname;

errret:
    vim_free(fname);
    vim_free(ओळ);
    ga_clear_strings(&sp->buflines);
    परत रिक्त;
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
    स्थिर पूर्णांक
do_source_ext(
    char_u	*fname,
    पूर्णांक		check_other,	    // check for .vimrc and _vimrc
    पूर्णांक		is_vimrc,	    // DOSO_ value
    पूर्णांक		*ret_sid UNUSED,
    exarg_T	*eap,
    पूर्णांक		clearvars UNUSED)
{
    source_cookie_T	    cookie;
    char_u		    *p;
    char_u		    *fname_not_fixed = रिक्त;
    char_u		    *fname_exp = रिक्त;
    char_u		    *firstline = रिक्त;
    पूर्णांक			    retval = FAIL;
    sctx_T		    save_current_sctx;
#ifdef STARTUPTIME
    संरचना timeval	    tv_rel;
    संरचना timeval	    tv_start;
#endif
#ifdef FEAT_PROFILE
    proftime_T		    wait_start;
#endif
    पूर्णांक			    save_sticky_cmdmod_flags = sticky_cmdmod_flags;
    पूर्णांक			    trigger_source_post = FALSE;
#ifdef FEAT_EVAL
    funccal_entry_T	    funccalp_entry;
    पूर्णांक			    save_debug_break_level = debug_break_level;
    पूर्णांक			    sid = -1;
    scriptitem_T	    *si = रिक्त;
    पूर्णांक			    save_estack_compiling = estack_compiling;
    ESTACK_CHECK_DECLARATION;
#endif

    CLEAR_FIELD(cookie);
    जर (fname == रिक्त)
    {
	// sourcing lines from a buffer
	fname_exp = do_source_buffer_init(&cookie, eap);
	जर (fname_exp == रिक्त)
	    परत FAIL;
    }
    अन्यथा
    {
	fname_not_fixed = expand_env_save(fname);
	जर (fname_not_fixed == रिक्त)
	    जा theend;
	fname_exp = fix_fname(fname_not_fixed);
	जर (fname_exp == रिक्त)
	    जा theend;
	जर (mch_isdir(fname_exp))
	{
	    smsg(_("Cannot source a directory: \"%s\""), fname);
	    जा theend;
	}
    }
#ifdef FEAT_EVAL
    estack_compiling = FALSE;

    // See if we loaded this script before.
    sid = find_script_by_name(fname_exp);
    जर (sid > 0 && ret_sid != रिक्त
			  && SCRIPT_ITEM(sid)->sn_state != SN_STATE_NOT_LOADED)
    {
	// Already loaded and no need to load again, return here.
	*ret_sid = sid;
	retval = OK;
	जा theend;
    }
#endif

    // Apply SourceCmd autocommands, they should get the file and source it.
    // When sourcing a file "fname_exp" was made a full path name by
    // fix_fname() above, avoid expanding it again.
    जर ((fname == रिक्त
		? has_autocmd(EVENT_SOURCECMD, fname_exp, रिक्त)
		: has_autocmd_fullname(EVENT_SOURCECMD, fname_exp, रिक्त))
	    && apply_autocmds(EVENT_SOURCECMD, fname_exp, fname_exp,
							       FALSE, curbuf))
    {
#ifdef FEAT_EVAL
	retval = aborting() ? FAIL : OK;
#अन्यथा
	retval = OK;
#endif
	जर (retval == OK)
	    // Apply SourcePost autocommands.
	    apply_autocmds(EVENT_SOURCEPOST, fname_exp, fname_exp,
								FALSE, curbuf);
	जा theend;
    }

    // Apply SourcePre autocommands, they may get the file.
    apply_autocmds(EVENT_SOURCEPRE, fname_exp, fname_exp, FALSE, curbuf);

    जर (!cookie.source_from_buf)
    {
#ifdef USE_FOPEN_NOINH
	cookie.fp = fopen_noinh_readbin((अक्षर *)fname_exp);
#अन्यथा
	cookie.fp = mch_fopen((अक्षर *)fname_exp, READBIN);
#endif
    }
    जर (cookie.fp == रिक्त && check_other)
    {
	// Try again, replacing file name ".vimrc" by "_vimrc" or vice versa,
	// and ".exrc" by "_exrc" or vice versa.
	p = gettail(fname_exp);
	जर ((*p == '.' || *p == '_')
		&& (STRICMP(p + 1, "vimrc") == 0
		    || STRICMP(p + 1, "gvimrc") == 0
		    || STRICMP(p + 1, "exrc") == 0))
	{
	    जर (*p == '_')
		*p = '.';
	    अन्यथा
		*p = '_';
#ifdef USE_FOPEN_NOINH
	    cookie.fp = fopen_noinh_readbin((अक्षर *)fname_exp);
#अन्यथा
	    cookie.fp = mch_fopen((अक्षर *)fname_exp, READBIN);
#endif
	}
    }

    जर (cookie.fp == रिक्त && !cookie.source_from_buf)
    {
	जर (p_verbose > 0)
	{
	    verbose_enter();
	    जर (SOURCING_NAME == रिक्त)
		smsg(_("could not source \"%s\""), fname);
	    अन्यथा
		smsg(_("line %ld: could not source \"%s\""),
							SOURCING_LNUM, fname);
	    verbose_leave();
	}
	जा theend;
    }

    // The file exists.
    // - In verbose mode, give a message.
    // - For a vimrc file, may want to set 'compatible', call vimrc_found().
    जर (p_verbose > 1)
    {
	verbose_enter();
	जर (SOURCING_NAME == रिक्त)
	    smsg(_("sourcing \"%s\""), fname);
	अन्यथा
	    smsg(_("line %ld: sourcing \"%s\""), SOURCING_LNUM, fname);
	verbose_leave();
    }
    जर (is_vimrc == DOSO_VIMRC)
	vimrc_found(fname_exp, (char_u *)"MYVIMRC");
    अन्यथा जर (is_vimrc == DOSO_GVIMRC)
	vimrc_found(fname_exp, (char_u *)"MYGVIMRC");

#ifdef USE_CRNL
    // If no automatic file format: Set default to CR-NL.
    जर (*p_ffs == NUL)
	cookie.fileformat = EOL_DOS;
    अन्यथा
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
    जर (time_fd != रिक्त)
	time_ढकला(&tv_rel, &tv_start);
#endif

    // "legacy" does not apply to commands in the script
    sticky_cmdmod_flags = 0;

    save_current_sctx = current_sctx;
    जर ((cmdmod.cmod_flags & CMOD_VIM9CMD) && cookie.source_from_buf)
	// When the ":vim9cmd" command modifier is used, source buffer lines as
	// Vim9 script
	current_sctx.sc_version = SCRIPT_VERSION_VIM9;
    अन्यथा
	current_sctx.sc_version = 1;  // default script version

#ifdef FEAT_EVAL
    current_sctx.sc_lnum = 0;

# ifdef FEAT_PROFILE
    जर (do_profiling == PROF_YES)
	prof_child_enter(&wait_start);		// entering a child now
# endif

    // Don't use local function variables, if called from a function.
    // Also starts profiling timer for nested script.
    save_funccal(&funccalp_entry);

    // Reset "KeyTyped" to avoid some commands thinking they are invoked
    // interactively.  E.g. defining a function would output indent.
    पूर्णांक save_किल्ली_Typed = किल्ली_Typed;
    किल्ली_Typed = FALSE;

    // Check if this script was sourced before to find its SID.
    // Always use a new sequence number.
    current_sctx.sc_seq = ++last_current_SID_seq;
    जर (sid > 0)
    {
	hashtab_T	*ht;
	पूर्णांक		todo;
	hashitem_T	*hi;
	dictitem_T	*di;

	// loading the same script again
	current_sctx.sc_sid = sid;
	si = SCRIPT_ITEM(sid);
	जर (si->sn_state == SN_STATE_NOT_LOADED)
	{
	    // this script was found but not loaded yet
	    si->sn_state = SN_STATE_NEW;
	}
	अन्यथा
	{
	    si->sn_state = SN_STATE_RELOAD;

	    जर (!clearvars)
	    {
		// Script-local variables remain but "const" can be set again.
		// In Vim9 script variables will be cleared when "vim9script"
		// is encountered without the "noclear" argument.
		ht = &SCRIPT_VARS(sid);
		todo = (पूर्णांक)ht->ht_used;
		FOR_ALL_HASHTAB_वस्तुसंच(ht, hi, todo)
		    जर (!HASHITEM_EMPTY(hi))
		    {
			--todo;
			di = HI2DI(hi);
			di->di_flags |= DI_FLAGS_RELOAD;
		    }
		// imports can be redefined once
		mark_imports_for_reload(sid);
	    }
	    अन्यथा
		clear_vim9_scriptlocal_vars(sid);

	    // reset version, "vim9script" may have been added or removed.
	    si->sn_version = 1;
	}
	जर (ret_sid != रिक्त)
	    *ret_sid = sid;
    }
    अन्यथा
    {
	पूर्णांक error = OK;

	// It's new, generate a new SID and initialize the scriptitem.
	sid = get_new_scriptitem(&error);
	current_sctx.sc_sid = sid;
	जर (error == FAIL)
	    जा almosttheend;
	si = SCRIPT_ITEM(sid);
	si->sn_name = fname_exp;
	fname_exp = vim_strsave(si->sn_name);  // used for autocmd
	जर (ret_sid != रिक्त)
	    *ret_sid = sid;

	// Remember the "is_vimrc" flag for when the file is sourced again.
	si->sn_is_vimrc = is_vimrc;
    }

    // Keep the sourcing name/lnum, for recursive calls.
    estack_ढकला(ETYPE_SCRIPT, si->sn_name, 0);
    ESTACK_CHECK_SETUP;

# ifdef FEAT_PROFILE
    जर (do_profiling == PROF_YES)
    {
	पूर्णांक	forceit;

	// Check if we do profiling for this script.
	जर (!si->sn_prof_on && has_profiling(TRUE, si->sn_name, &forceit, रिक्त))
	{
	    script_do_profile(si);
	    si->sn_pr_force = forceit;
	}
	जर (si->sn_prof_on)
	{
	    ++si->sn_pr_मोजणी;
	    profile_start(&si->sn_pr_start);
	    profile_zero(&si->sn_pr_children);
	}
    }
# endif
#अन्यथा
    // Keep the sourcing name/lnum, for recursive calls.
    estack_ढकला(ETYPE_SCRIPT, fname_exp, 0);
#endif

    cookie.conv.vc_type = CONV_NONE;		// no conversion

    // Read the first line so we can check for a UTF-8 BOM.
    firstline = getsourceline(0, (शून्य *)&cookie, 0, TRUE);
    जर (firstline != रिक्त && STRLEN(firstline) >= 3 && firstline[0] == 0xef
			      && firstline[1] == 0xbb && firstline[2] == 0xbf)
    {
	// Found BOM; setup conversion, skip over BOM and recode the line.
	convert_setup(&cookie.conv, (char_u *)"utf-8", p_enc);
	p = string_convert(&cookie.conv, firstline + 3, रिक्त);
	जर (p == रिक्त)
	    p = vim_strsave(firstline + 3);
	जर (p != रिक्त)
	{
	    vim_free(firstline);
	    firstline = p;
	}
    }

    // Call do_cmdline, which will call getsourceline() to get the lines.
    do_cmdline(firstline, getsourceline, (शून्य *)&cookie,
				     DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_REPEAT);
    retval = OK;

#ifdef FEAT_PROFILE
    जर (do_profiling == PROF_YES)
    {
	// Get "si" again, "script_items" may have been reallocated.
	si = SCRIPT_ITEM(sid);
	जर (si->sn_prof_on)
	{
	    profile_end(&si->sn_pr_start);
	    profile_sub_wait(&wait_start, &si->sn_pr_start);
	    profile_add(&si->sn_pr_total, &si->sn_pr_start);
	    profile_self(&si->sn_pr_self, &si->sn_pr_start,
							 &si->sn_pr_children);
	}
    }
#endif

    जर (got_int)
	emsg(_(e_interrupted));
#ifdef FEAT_EVAL
    ESTACK_CHECK_NOW;
#endif
    estack_काढा();
    जर (p_verbose > 1)
    {
	verbose_enter();
	smsg(_("finished sourcing %s"), fname);
	जर (SOURCING_NAME != रिक्त)
	    smsg(_("continuing in %s"), SOURCING_NAME);
	verbose_leave();
    }
#ifdef STARTUPTIME
    जर (time_fd != रिक्त)
    {
	vim_snprintf((अक्षर *)IObuff, IOSIZE, "sourcing %s", fname);
	time_संदेश((अक्षर *)IObuff, &tv_start);
	time_काढा(&tv_rel);
    }
#endif

    जर (!got_int)
	trigger_source_post = TRUE;

#ifdef FEAT_EVAL
    // After a "finish" in debug mode, need to break at first command of next
    // sourced file.
    जर (save_debug_break_level > ex_nesting_level
	    && debug_break_level == ex_nesting_level)
	++debug_break_level;
#endif

#ifdef FEAT_EVAL
almosttheend:
    // If "sn_save_cpo" is set that means we encountered "vim9script": restore
    // 'cpoptions', unless in the main .vimrc file.
    // Get "si" again, "script_items" may have been reallocated.
    si = SCRIPT_ITEM(sid);
    जर (si->sn_save_cpo != रिक्त && si->sn_is_vimrc == DOSO_NONE)
    {
	जर (STRCMP(p_cpo, CPO_VIM) != 0)
	{
	    char_u *f;
	    char_u *t;

	    // 'cpo' was changed in the script.  Apply the same change to the
	    // saved value, if possible.
	    साठी (f = (char_u *)CPO_VIM; *f != NUL; ++f)
		जर (vim_strchr(p_cpo, *f) == रिक्त
			&& (t = vim_strchr(si->sn_save_cpo, *f)) != रिक्त)
		    // flag was removed, also remove it from the saved 'cpo'
		    mch_memmove(t, t + 1, STRLEN(t));
	    साठी (f = p_cpo; *f != NUL; ++f)
		जर (vim_strchr((char_u *)CPO_VIM, *f) == रिक्त
			&& vim_strchr(si->sn_save_cpo, *f) == रिक्त)
		{
		    // flag was added, also add it to the saved 'cpo'
		    t = alloc(STRLEN(si->sn_save_cpo) + 2);
		    जर (t != रिक्त)
		    {
			*t = *f;
			STRCPY(t + 1, si->sn_save_cpo);
			vim_free(si->sn_save_cpo);
			si->sn_save_cpo = t;
		    }
		}
	}
	set_option_मूल्य_give_err((char_u *)"cpo",
					   0L, si->sn_save_cpo, OPT_NO_REDRAW);
    }
    VIM_CLEAR(si->sn_save_cpo);

    restore_funccal();
# ifdef FEAT_PROFILE
    जर (do_profiling == PROF_YES)
	prof_child_exit(&wait_start);		// leaving a child now
# endif

    किल्ली_Typed = save_किल्ली_Typed;
#endif
    current_sctx = save_current_sctx;

    जर (cookie.fp != रिक्त)
	ख_बंद(cookie.fp);
    जर (cookie.source_from_buf)
	ga_clear_strings(&cookie.buflines);
    vim_free(cookie.nextline);
    vim_free(firstline);
    convert_setup(&cookie.conv, रिक्त, रिक्त);

    जर (trigger_source_post)
	apply_autocmds(EVENT_SOURCEPOST, fname_exp, fname_exp, FALSE, curbuf);

theend:
#ifdef FEAT_EVAL
    जर (sid > 0 && ret_sid != रिक्त
	    && fname_not_fixed != रिक्त && fname_exp != रिक्त)
    {
	पूर्णांक not_fixed_sid = find_script_by_name(fname_not_fixed);

	// If "fname_not_fixed" is a symlink then we source the linked file.
	// If the original name is in the script list we add the ID of the
	// script that was actually sourced.
	जर (SCRIPT_ID_VALID(not_fixed_sid) && not_fixed_sid != sid)
	    SCRIPT_ITEM(not_fixed_sid)->sn_sourced_sid = sid;
    }
#endif

    vim_free(fname_not_fixed);
    vim_free(fname_exp);
    sticky_cmdmod_flags = save_sticky_cmdmod_flags;
#ifdef FEAT_EVAL
    estack_compiling = save_estack_compiling;
#endif
    परत retval;
}

    पूर्णांक
do_source(
    char_u	*fname,
    पूर्णांक		check_other,	    // check for .vimrc and _vimrc
    पूर्णांक		is_vimrc,	    // DOSO_ value
    पूर्णांक		*ret_sid)
{
    परत do_source_ext(fname, check_other, is_vimrc, ret_sid, रिक्त, FALSE);
}


#जर defined(FEAT_EVAL)

/*
 * ":scriptnames"
 */
    शून्य
ex_scriptnames(exarg_T *eap)
{
    पूर्णांक i;

    जर (eap->addr_मोजणी > 0 || *eap->arg != NUL)
    {
	// :script {scriptId}: edit the script
	जर (eap->addr_मोजणी > 0 && !SCRIPT_ID_VALID(eap->line2))
	    emsg(_(e_invalid_argument));
	अन्यथा
	{
	    जर (eap->addr_मोजणी > 0)
		eap->arg = SCRIPT_ITEM(eap->line2)->sn_name;
	    अन्यथा
	    {
		expand_env(eap->arg, NameBuff, MAXPATHL);
		eap->arg = NameBuff;
	    }
	    do_exedit(eap, रिक्त);
	}
	परत;
    }

    साठी (i = 1; i <= script_वस्तुसंच.ga_len && !got_int; ++i)
    {
	scriptitem_T *si = SCRIPT_ITEM(i);

	जर (si->sn_name != रिक्त)
	{
	    अक्षर sourced_buf[20];

	    home_replace(रिक्त, si->sn_name, NameBuff, MAXPATHL, TRUE);
	    जर (si->sn_sourced_sid > 0)
		vim_snprintf(sourced_buf, 20, "->%d", si->sn_sourced_sid);
	    अन्यथा
		sourced_buf[0] = NUL;
	    vim_snprintf((अक्षर *)IObuff, IOSIZE, "%3d%s%s: %s",
		    i,
		    sourced_buf,
		    si->sn_state == SN_STATE_NOT_LOADED ? " A" : "",
		    NameBuff);
	    जर (!message_filtered(IObuff))
	    {
		संदेश_putchar('\n');
		संदेश_outtrans(IObuff);
		out_flush();	    // output one line at a time
		ui_breakcheck();
	    }
	}
    }
}

# जर defined(BACKSLASH_IN_FILENAME)
/*
 * Fix slashes in the list of script names for 'shellslash'.
 */
    शून्य
scriptnames_slash_adjust(शून्य)
{
    पूर्णांक i;

    साठी (i = 1; i <= script_वस्तुसंच.ga_len; ++i)
	जर (SCRIPT_ITEM(i)->sn_name != रिक्त)
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
    जर (id == SID_MODELINE)
	परत (char_u *)_("modeline");
    जर (id == SID_CMDARG)
	परत (char_u *)_("--cmd argument");
    जर (id == SID_CARG)
	परत (char_u *)_("-c argument");
    जर (id == SID_ENV)
	परत (char_u *)_("environment variable");
    जर (id == SID_ERROR)
	परत (char_u *)_("error handler");
    जर (id == SID_WINLAYOUT)
	परत (char_u *)_("changed window size");
    परत SCRIPT_ITEM(id)->sn_name;
}

# जर defined(EXITFREE)
    शून्य
free_scriptnames(शून्य)
{
    पूर्णांक			i;

    साठी (i = script_वस्तुसंच.ga_len; i > 0; --i)
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
    ga_clear(&script_वस्तुसंच);
}

    शून्य
free_autoload_scriptnames(शून्य)
{
    ga_clear_strings(&ga_loaded);
}
# endif

    linenr_T
get_sourced_lnum(
	char_u *(*fgetline)(पूर्णांक, शून्य *, पूर्णांक, getline_opt_T),
	शून्य *cookie)
{
    परत fgetline == getsourceline
			? ((source_cookie_T *)cookie)->sourcing_lnum
			: SOURCING_LNUM;
}

/*
 * Return a List of script-local functions defined in the script with id
 * 'sid'.
 */
    स्थिर list_T *
get_script_local_funcs(scid_T sid)
{
    hashtab_T	*functbl;
    hashitem_T	*hi;
    long_u	todo;
    list_T	*l;

    l = list_alloc();
    जर (l == रिक्त)
	परत रिक्त;

    // Iterate through all the functions in the global function hash table
    // looking for functions with script ID 'sid'.
    functbl = func_tbl_get();
    todo = functbl->ht_used;
    FOR_ALL_HASHTAB_वस्तुसंच(functbl, hi, todo)
    {
	ufunc_T	*fp;

	जर (HASHITEM_EMPTY(hi))
	    सुरूठेवा;

	--todo;
	fp = HI2UF(hi);

	// Add active functions with script id == 'sid'
	जर (!(fp->uf_flags & FC_DEAD) && (fp->uf_script_ctx.sc_sid == sid))
	{
	    char_u	*name;

	    जर (fp->uf_name_exp != रिक्त)
		name = fp->uf_name_exp;
	    अन्यथा
		name = fp->uf_name;

	    list_append_string(l, name, -1);
	}
    }

    परत l;
}

/*
 * getscriptinfo() function
 */
    शून्य
f_getscriptinfo(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    char_u	*pat = रिक्त;
    regmatch_T	regmatch;
    पूर्णांक		filterpat = FALSE;
    scid_T	sid = -1;

    जर (rettv_list_alloc(rettv) == FAIL)
	परत;

    जर (check_for_opt_dict_arg(argvars, 0) == FAIL)
	परत;

    l = rettv->vval.v_list;

    regmatch.regprog = रिक्त;
    regmatch.rm_ic = p_ic;

    जर (argvars[0].v_type == VAR_DICT)
    {
	dictitem_T *sid_di = dict_find(argvars[0].vval.v_dict,
							   (char_u *)"sid", 3);
	जर (sid_di != रिक्त)
	{
	    पूर्णांक error = FALSE;
	    sid = tv_get_संख्या_chk(&sid_di->di_tv, &error);
	    जर (error)
		परत;
	    जर (sid <= 0)
	    {
		semsg(_(e_invalid_मूल्य_for_argument_str_str), "sid",
						tv_get_string(&sid_di->di_tv));
		परत;
	    }
	}
	अन्यथा
	{
	    pat = dict_get_string(argvars[0].vval.v_dict, "name", TRUE);
	    जर (pat != रिक्त)
		regmatch.regprog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
	    जर (regmatch.regprog != रिक्त)
		filterpat = TRUE;
	}
    }

    साठी (varnumber_T i = sid > 0 ? sid : 1;
		       (i == sid || sid <= 0) && i <= script_वस्तुसंच.ga_len; ++i)
    {
	scriptitem_T	*si = SCRIPT_ITEM(i);
	dict_T		*d;

	जर (si->sn_name == रिक्त)
	    सुरूठेवा;

	जर (filterpat && !vim_regexec(&regmatch, si->sn_name, (colnr_T)0))
	    सुरूठेवा;

	जर ((d = dict_alloc()) == रिक्त
		|| list_append_dict(l, d) == FAIL)
	{
	    dict_unref(d);
	    जा theend;
	}
	जर (dict_add_string(d, "name", si->sn_name) == FAIL
		|| dict_add_संख्या(d, "sid", i) == FAIL
		|| dict_add_संख्या(d, "sourced", si->sn_sourced_sid) == FAIL
		|| dict_add_संख्या(d, "version", si->sn_version) == FAIL
		|| dict_add_bool(d, "autoload",
				si->sn_state == SN_STATE_NOT_LOADED) == FAIL)
	    जा theend;

	// When a script ID is specified, return information about only the
	// specified script, and add the script-local variables and functions.
	जर (sid > 0)
	{
	    dict_T	*var_dict;
	    list_T	*fn_list;

	    var_dict = dict_copy(&si->sn_vars->sv_dict, TRUE, TRUE,
								get_copyID());
	    जर (var_dict == रिक्त)
		जा theend;
	    जर (dict_add_dict(d, "variables", var_dict) == FAIL)
	    {
		dict_unref(var_dict);
		जा theend;
	    }
	    --var_dict->dv_refcount;
	    fn_list = get_script_local_funcs(sid);
	    जर (fn_list == रिक्त || dict_add_list(d, "functions", fn_list) == FAIL)
	    {
		list_unref(fn_list);
		जा theend;
	    }
	}
    }

theend:
    vim_regfree(regmatch.regprog);
    vim_free(pat);
}

#endif

    स्थिर char_u *
get_one_sourceline(source_cookie_T *sp)
{
    garray_T		ga;
    पूर्णांक			len;
    पूर्णांक			c;
    char_u		*buf;
#ifdef USE_CRNL
    पूर्णांक			has_cr;		// CR-LF found
#endif
    पूर्णांक			have_read = FALSE;

    // use a growarray to store the sourced line
    ga_init2(&ga, 1, 250);

    // Loop until there is a finished line (or end-of-file).
    ++sp->sourcing_lnum;
    साठी (;;)
    {
	// make room to read at least 120 (more) characters
	जर (ga_grow(&ga, 120) == FAIL)
	    थांबा;
	जर (sp->source_from_buf)
	{
	    जर (sp->buf_lnum >= sp->buflines.ga_len)
		थांबा;		    // all the lines are processed
	    ga_concat(&ga, ((char_u **)sp->buflines.ga_data)[sp->buf_lnum]);
	    sp->buf_lnum++;
	    जर (ga_grow(&ga, 1) == FAIL)
		थांबा;
	    buf = (char_u *)ga.ga_data;
	    buf[ga.ga_len++] = NUL;
	    len = ga.ga_len;
	}
	अन्यथा
	{
	    buf = (char_u *)ga.ga_data;
	    जर (ख_माला_लो((अक्षर *)buf + ga.ga_len, ga.ga_maxlen - ga.ga_len,
			sp->fp) == रिक्त)
		थांबा;
	    len = ga.ga_len + (पूर्णांक)STRLEN(buf + ga.ga_len);
	}
#ifdef USE_CRNL
	// Ignore a trailing CTRL-Z, when in Dos mode.	Only recognize the
	// CTRL-Z by its own, or after a NL.
	जर (	   (len == 1 || (len >= 2 && buf[len - 2] == '\n'))
		&& sp->fileformat == EOL_DOS
		&& buf[len - 1] == Ctrl_Z)
	{
	    buf[len - 1] = NUL;
	    थांबा;
	}
#endif

	have_read = TRUE;
	ga.ga_len = len;

	// If the line was longer than the buffer, read more.  A line of a
	// buffer was taken as a whole.
	जर (!sp->source_from_buf && ga.ga_maxlen - ga.ga_len == 1
						       && buf[len - 1] != '\n')
	    सुरूठेवा;

	जर (len >= 1 && buf[len - 1] == '\n')	// remove trailing NL
	{
#ifdef USE_CRNL
	    has_cr = (len >= 2 && buf[len - 2] == '\r');
	    जर (sp->fileformat == EOL_UNKNOWN)
	    {
		जर (has_cr)
		    sp->fileformat = EOL_DOS;
		अन्यथा
		    sp->fileformat = EOL_UNIX;
	    }

	    जर (sp->fileformat == EOL_DOS)
	    {
		जर (has_cr)	    // replace trailing CR
		{
		    buf[len - 2] = '\n';
		    --len;
		    --ga.ga_len;
		}
		अन्यथा	    // lines like ":map xx yy^M" will have failed
		{
		    जर (!sp->error)
		    {
			संदेश_source(HL_ATTR(HLF_W));
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
	    साठी (c = len - 2; c >= 0 && buf[c] == Ctrl_V; c--)
		;
	    जर ((len & 1) != (c & 1))	// escaped NL, read more
	    {
		++sp->sourcing_lnum;
		सुरूठेवा;
	    }

	    buf[len - 1] = NUL;		// remove the NL
	}

	// Check for ^C here now and then, so recursive :so can be broken.
	ओळ_breakcheck();
	थांबा;
    }

    जर (have_read)
	परत (char_u *)ga.ga_data;

    vim_free(ga.ga_data);
    परत रिक्त;
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
	पूर्णांक c UNUSED,
	शून्य *cookie,
	पूर्णांक indent UNUSED,
	getline_opt_T options)
{
    source_cookie_T	*sp = (source_cookie_T *)cookie;
    char_u		*ओळ;
    char_u		*p;
    पूर्णांक			do_vim9_all = in_vim9script()
					      && options == GETLINE_CONCAT_ALL;
    पूर्णांक			do_bar_cont = do_vim9_all
					 || options == GETLINE_CONCAT_CONTBAR;

#ifdef FEAT_EVAL
    // If breakpoints have been added/deleted need to check for it.
    जर ((sp->dbg_tick < debug_tick) && !sp->source_from_buf)
    {
	sp->breakpoint = dbg_find_breakpoint(TRUE, sp->fname, SOURCING_LNUM);
	sp->dbg_tick = debug_tick;
    }
# ifdef FEAT_PROFILE
    जर (do_profiling == PROF_YES)
	script_ओळ_end();
# endif
#endif

    // Set the current sourcing line number.
    SOURCING_LNUM = sp->sourcing_lnum + 1;

    // Get current line.  If there is a read-ahead line, use it, otherwise get
    // one now.  "fp" is NULL if actually using a string.
    जर (sp->finished || (!sp->source_from_buf && sp->fp == रिक्त))
	ओळ = रिक्त;
    अन्यथा जर (sp->nextline == रिक्त)
	ओळ = get_one_sourceline(sp);
    अन्यथा
    {
	ओळ = sp->nextline;
	sp->nextline = रिक्त;
	++sp->sourcing_lnum;
    }
#ifdef FEAT_PROFILE
    जर (ओळ != रिक्त && do_profiling == PROF_YES)
	script_ओळ_start();
#endif

    // Only concatenate lines starting with a \ when 'cpoptions' doesn't
    // contain the 'C' flag.
    जर (ओळ != रिक्त && options != GETLINE_NONE
				      && vim_strchr(p_cpo, CPO_CONCAT) == रिक्त)
    {
	पूर्णांक comment_char = in_vim9script() ? '#' : '"';

	// compensate for the one line read-ahead
	--sp->sourcing_lnum;

	// Get the next line and concatenate it when it starts with a
	// backslash. We always need to read the next line, keep it in
	// sp->nextline.
	/* Also check for a comment in between continuation lines: "\ */
	// Also check for a Vim9 comment, empty line, line starting with '|',
	// but not "||".
	sp->nextline = get_one_sourceline(sp);
	जर (sp->nextline != रिक्त
		&& (*(p = skipwhite(sp->nextline)) == '\\'
			      || (p[0] == comment_char
						&& p[1] == '\\' && p[2] == ' ')
			      || (do_vim9_all && (*p == NUL
						     || vim9_comment_start(p)))
			      || (do_bar_cont && p[0] == '|' && p[1] != '|')))
	{
	    garray_T    ga;

	    ga_init2(&ga, आकार(char_u), 400);
	    ga_concat(&ga, ओळ);
	    जर (*p == '\\')
		ga_concat(&ga, p + 1);
	    अन्यथा जर (*p == '|')
	    {
		GA_CONCAT_LITERAL(&ga, " ");
		ga_concat(&ga, p);
	    }
	    साठी (;;)
	    {
		vim_free(sp->nextline);
		sp->nextline = get_one_sourceline(sp);
		जर (sp->nextline == रिक्त)
		    थांबा;
		p = skipwhite(sp->nextline);
		जर (*p == '\\' || (do_bar_cont && p[0] == '|' && p[1] != '|'))
		{
		    // Adjust the growsize to the current length to speed up
		    // concatenating many lines.
		    जर (ga.ga_len > 400)
		    {
			जर (ga.ga_len > 8000)
			    ga.ga_growsize = 8000;
			अन्यथा
			    ga.ga_growsize = ga.ga_len;
		    }
		    जर (*p == '\\')
			ga_concat(&ga, p + 1);
		    अन्यथा
		    {
			GA_CONCAT_LITERAL(&ga, " ");
			ga_concat(&ga, p);
		    }
		}
		अन्यथा जर (!(p[0] == (comment_char)
						&& p[1] == '\\' && p[2] == ' ')
		     && !(do_vim9_all && (*p == NUL || vim9_comment_start(p))))
		    थांबा;
		/* drop a # comment or "\ comment line */
	    }
	    ga_append(&ga, NUL);
	    vim_free(ओळ);
	    ओळ = ga.ga_data;
	}
    }

    जर (ओळ != रिक्त && sp->conv.vc_type != CONV_NONE)
    {
	char_u	*s;

	// Convert the encoding of the script line.
	s = string_convert(&sp->conv, ओळ, रिक्त);
	जर (s != रिक्त)
	{
	    vim_free(ओळ);
	    ओळ = s;
	}
    }

#ifdef FEAT_EVAL
    // Did we encounter a breakpoint?
    जर (!sp->source_from_buf && sp->breakpoint != 0
	    && sp->breakpoint <= SOURCING_LNUM)
    {
	dbg_breakpoint(sp->fname, SOURCING_LNUM);
	// Find next breakpoint.
	sp->breakpoint = dbg_find_breakpoint(TRUE, sp->fname, SOURCING_LNUM);
	sp->dbg_tick = debug_tick;
    }
#endif

    परत ओळ;
}

/*
 * Returns TRUE if sourcing a script either from a file or a buffer.
 * Otherwise returns FALSE.
 */
    पूर्णांक
sourcing_a_script(exarg_T *eap)
{
    परत (getline_equal(eap->ea_getline, eap->cookie, getsourceline));
}

/*
 * ":scriptencoding": Set encoding conversion for a sourced script.
 */
    शून्य
ex_scriptencoding(exarg_T *eap)
{
    source_cookie_T	*sp;
    char_u		*name;

    जर (!sourcing_a_script(eap))
    {
	emsg(_(e_scriptencoding_used_outside_of_sourced_file));
	परत;
    }

    जर (*eap->arg != NUL)
    {
	name = enc_canonize(eap->arg);
	जर (name == रिक्त)	// out of memory
	    परत;
    }
    अन्यथा
	name = eap->arg;

    // Setup for conversion from the specified encoding to 'encoding'.
    sp = (source_cookie_T *)getline_cookie(eap->ea_getline, eap->cookie);
    convert_setup(&sp->conv, name, p_enc);

    जर (name != eap->arg)
	vim_free(name);
}

/*
 * ":scriptversion": Set Vim script version for a sourced script.
 */
    शून्य
ex_scriptversion(exarg_T *eap UNUSED)
{
    पूर्णांक		nr;

    जर (!sourcing_a_script(eap))
    {
	emsg(_(e_scriptversion_used_outside_of_sourced_file));
	परत;
    }
    जर (in_vim9script())
    {
	emsg(_(e_cannot_use_scriptversion_after_vim9script));
	परत;
    }

    nr = getdigits(&eap->arg);
    जर (nr == 0 || *eap->arg != NUL)
	emsg(_(e_invalid_argument));
    अन्यथा जर (nr > SCRIPT_VERSION_MAX)
	semsg(_(e_scriptversion_not_supported_nr), nr);
    अन्यथा
    {
	current_sctx.sc_version = nr;
#ifdef FEAT_EVAL
	SCRIPT_ITEM(current_sctx.sc_sid)->sn_version = nr;
#endif
    }
}

#जर defined(FEAT_EVAL)
/*
 * ":finish": Mark a sourced file as finished.
 */
    शून्य
ex_finish(exarg_T *eap)
{
    जर (sourcing_a_script(eap))
	do_finish(eap, FALSE);
    अन्यथा
	emsg(_(e_finish_used_outside_of_sourced_file));
}

/*
 * Mark a sourced file as finished.  Possibly makes the ":finish" pending.
 * Also called for a pending finish at the ":endtry" or after returning from
 * an extra do_cmdline().  "reanimate" is used in the latter case.
 */
    शून्य
do_finish(exarg_T *eap, पूर्णांक reanimate)
{
    पूर्णांक		idx;

    जर (reanimate)
	((source_cookie_T *)getline_cookie(eap->ea_getline,
					      eap->cookie))->finished = FALSE;

    // Cleanup (and inactivate) conditionals, but stop when a try conditional
    // not in its finally clause (which then is to be executed next) is found.
    // In this case, make the ":finish" pending for execution at the ":endtry".
    // Otherwise, finish normally.
    idx = cleanup_conditionals(eap->cstack, 0, TRUE);
    जर (idx >= 0)
    {
	eap->cstack->cs_pending[idx] = CSTP_FINISH;
	report_make_pending(CSTP_FINISH, रिक्त);
    }
    अन्यथा
	((source_cookie_T *)getline_cookie(eap->ea_getline,
					       eap->cookie))->finished = TRUE;
}


/*
 * Return TRUE when a sourced file had the ":finish" command: Don't give error
 * message for missing ":endif".
 * Return FALSE when not sourcing a file.
 */
    पूर्णांक
source_finished(
    char_u	*(*fgetline)(पूर्णांक, शून्य *, पूर्णांक, getline_opt_T),
    शून्य	*cookie)
{
    परत (getline_equal(fgetline, cookie, getsourceline)
	    && ((source_cookie_T *)getline_cookie(
						fgetline, cookie))->finished);
}

/*
 * Find the path of a script below the "autoload" directory.
 * Returns NULL if there is no "/autoload/" in the script name.
 */
    स्थिर char_u *
script_name_after_autoload(scriptitem_T *si)
{
    char_u	*p = si->sn_name;
    char_u	*res = रिक्त;

    साठी (;;)
    {
	char_u *n = (char_u *)म_माला((अक्षर *)p, "autoload");

	जर (n == रिक्त)
	    थांबा;
	जर (n > p && vim_ispathsep(n[-1]) && vim_ispathsep(n[8]))
	    res = n + 9;
	p = n + 8;
    }
    परत res;
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

    जर (p == रिक्त)
	परत रिक्त;
    prefix = vim_strsave(p);
    जर (prefix == रिक्त)
	परत रिक्त;

    // replace all '/' with '#' and locate ".vim" at the end
    साठी (p = prefix; *p != NUL; p += mb_ptr2len(p))
    {
	जर (vim_ispathsep(*p))
	    *p = '#';
	अन्यथा जर (STRCMP(p, ".vim") == 0)
	{
	    p[0] = '#';
	    p[1] = NUL;
	    परत prefix;
	}
    }

    // did not find ".vim" at the end
    vim_free(prefix);
    परत रिक्त;
}

/*
 * If in a Vim9 autoload script return "name" with the autoload prefix for the
 * script.  If successful the returned name is allocated.
 * Otherwise it returns "name" unmodified.
 */
    char_u *
may_prefix_autoload(char_u *name)
{
    जर (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	परत name;

    scriptitem_T *si = SCRIPT_ITEM(current_sctx.sc_sid);

    जर (si->sn_autoload_prefix == रिक्त)
	परत name;

    char_u  *basename = name;
    माप_प्रकार  len;
    char_u  *res;

    जर (*name == K_SPECIAL)
    {
	char_u *p = vim_strchr(name, '_');

	// skip over "<SNR>99_"
	जर (p != रिक्त)
	    basename = p + 1;
    }

    len = STRLEN(si->sn_autoload_prefix) + STRLEN(basename) + 2;
    res = alloc(len);
    जर (res == रिक्त)
	परत रिक्त;

    vim_snprintf((अक्षर *)res, len, "%s%s", si->sn_autoload_prefix, basename);
    परत res;
}

/*
 * Return the autoload script name for a function or variable name.
 * Returns NULL when out of memory.
 * Caller must make sure that "name" contains AUTOLOAD_CHAR.
 */
    char_u *
autoload_name(char_u *name)
{
    char_u	*p, *q = रिक्त;
    char_u	*scriptname;

    // Get the script file name: replace '#' with '/', append ".vim".
    scriptname = alloc(STRLEN(name) + 14);
    जर (scriptname == रिक्त)
	परत रिक्त;
    STRCPY(scriptname, "autoload/");
    STRCAT(scriptname, name[0] == 'g' && name[1] == ':' ? name + 2: name);
    साठी (p = scriptname + 9; (p = vim_strchr(p, AUTOLOAD_CHAR)) != रिक्त;
								    q = p, ++p)
	*p = '/';
    STRCPY(q, ".vim");
    परत scriptname;
}

/*
 * If "name" has a package name try autoloading the script for it.
 * Return TRUE if a package was loaded.
 */
    पूर्णांक
script_autoload(
    char_u	*name,
    पूर्णांक		reload)	    // load script again when already loaded
{
    char_u	*p;
    char_u	*scriptname, *tofree;
    पूर्णांक		ret = FALSE;
    पूर्णांक		i;
    पूर्णांक		ret_sid;

    // If the name starts with "<SNR>123_" then "123" is the script ID.
    जर (name[0] == K_SPECIAL && name[1] == KS_EXTRA && name[2] == KE_SNR)
    {
	p = name + 3;
	ret_sid = (पूर्णांक)getdigits(&p);
	जर (*p == '_' && SCRIPT_ID_VALID(ret_sid))
	{
	    may_load_script(ret_sid, &ret);
	    परत ret;
	}
    }

    // If there is no '#' after name[0] there is no package name.
    p = vim_strchr(name, AUTOLOAD_CHAR);
    जर (p == रिक्त || p == name)
	परत FALSE;

    tofree = scriptname = autoload_name(name);
    जर (scriptname == रिक्त)
	परत FALSE;

    // Find the name in the list of previously loaded package names.  Skip
    // "autoload/", it's always the same.
    साठी (i = 0; i < ga_loaded.ga_len; ++i)
	जर (STRCMP(((char_u **)ga_loaded.ga_data)[i] + 9, scriptname + 9) == 0)
	    थांबा;
    जर (!reload && i < ga_loaded.ga_len)
	ret = FALSE;	    // was loaded already
    अन्यथा
    {
	// Remember the name if it wasn't loaded already.
	जर (i == ga_loaded.ga_len && ga_grow(&ga_loaded, 1) == OK)
	{
	    ((char_u **)ga_loaded.ga_data)[ga_loaded.ga_len++] = scriptname;
	    tofree = रिक्त;
	}

	// Try loading the package from $VIMRUNTIME/autoload/<name>.vim
	// Use "ret_sid" to avoid loading the same script again.
	जर (source_in_path(p_rtp, scriptname, DIP_START, &ret_sid) == OK)
	    ret = TRUE;
    }

    vim_free(tofree);
    परत ret;
}
#endif
