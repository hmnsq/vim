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

#include "vim.h"

#もし defined(FEAT_EVAL)
// The names of packages that once were loaded are remembered.
static garray_T		ga_loaded = {0, 0, sizeof(char_u *), 4, NULL};
#endif

// last used sequence number for sourcing scripts (current_sctx.sc_seq)
#ifdef FEAT_EVAL
static 整数		last_current_SID_seq = 0;
#endif

static 整数 do_source_ext(char_u *fname, 整数 check_other, 整数 is_vimrc, 整数 *ret_sid, exarg_T *eap, 整数 clearvars);

/*
 * Initialize the execution stack.
 */
    void
estack_init(void)
{
    estack_T *entry;

    もし (ga_grow(&exestack, 10) == FAIL)
	mch_exit(0);
    entry = ((estack_T *)exestack.ga_data) + exestack.ga_len;
    entry->es_type = ETYPE_TOP;
    entry->es_name = NULL;
    entry->es_lnum = 0;
#ifdef FEAT_EVAL
    entry->es_info.ufunc = NULL;
#endif
    ++exestack.ga_len;
}

/*
 * Add an item to the execution stack.
 * Returns the new entry or NULL when out of memory.
 */
    estack_T *
estack_push(etype_T type, char_u *name, long lnum)
{
    estack_T *entry;

    // If memory allocation fails then we'll pop more than we push, eventually
    // at the top level it will be OK again.
    もし (ga_grow(&exestack, 1) == FAIL)
	戻る NULL;

    entry = ((estack_T *)exestack.ga_data) + exestack.ga_len;
    entry->es_type = type;
    entry->es_name = name;
    entry->es_lnum = lnum;
#ifdef FEAT_EVAL
    entry->es_info.ufunc = NULL;
#endif
    ++exestack.ga_len;
    戻る entry;
}

#もし defined(FEAT_EVAL)
/*
 * Add a user function to the execution stack.
 */
    estack_T *
estack_push_ufunc(ufunc_T *ufunc, long lnum)
{
    estack_T *entry = estack_push(ETYPE_UFUNC,
	    ufunc->uf_name_exp != NULL
				  ? ufunc->uf_name_exp : ufunc->uf_name, lnum);
    もし (entry != NULL)
	entry->es_info.ufunc = ufunc;
    戻る entry;
}

/*
 * Return TRUE if "ufunc" with "lnum" is already at the top of the exe stack.
 */
    整数
estack_top_is_ufunc(ufunc_T *ufunc, long lnum)
{
    estack_T *entry;

    もし (exestack.ga_len == 0)
	戻る FALSE;
    entry = ((estack_T *)exestack.ga_data) + exestack.ga_len - 1;
    戻る entry->es_type == ETYPE_UFUNC
	&& STRCMP( entry->es_name, ufunc->uf_name_exp != NULL
				    ? ufunc->uf_name_exp : ufunc->uf_name) == 0
	&& entry->es_lnum == lnum;
}
#endif

/*
 * Take an item off of the execution stack and return it.
 */
    estack_T *
estack_pop(void)
{
    もし (exestack.ga_len == 0)
	戻る NULL;
    --exestack.ga_len;
    戻る ((estack_T *)exestack.ga_data) + exestack.ga_len;
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
    整数		idx;
    etype_T	last_type = ETYPE_SCRIPT;
#endif

    entry = ((estack_T *)exestack.ga_data) + exestack.ga_len - 1;
#ifdef FEAT_EVAL
    もし (which == ESTACK_SFILE && entry->es_type != ETYPE_UFUNC)
#endif
    {
	もし (entry->es_name == NULL)
	    戻る NULL;
	戻る vim_strsave(entry->es_name);
    }
#ifdef FEAT_EVAL
    // expand('<sfile>') works in a function for backwards compatibility, but
    // may give an unexpected result.  Disallow it in Vim 9 script.
    もし (which == ESTACK_SFILE && in_vim9script())
    {
	整数  save_emsg_off = emsg_off;

	もし (emsg_off == 1)
	    // f_expand() silences errors but we do want this one
	    emsg_off = 0;
	emsg(_(e_cannot_expand_sfile_in_vim9_function));
	emsg_off = save_emsg_off;
	戻る NULL;
    }

    // If evaluated in a function or autocommand, return the path of the script
    // where it is defined, at script level the current script path is returned
    // instead.
    もし (which == ESTACK_SCRIPT)
    {
	// Walk the stack backwards, starting from the current frame.
	ために (idx = exestack.ga_len - 1; idx >= 0; --idx, --entry)
	{
	    もし (entry->es_type == ETYPE_UFUNC || entry->es_type == ETYPE_AUCMD)
	    {
		sctx_T *def_ctx = entry->es_type == ETYPE_UFUNC
			      ? &entry->es_info.ufunc->uf_script_ctx
			      : entry->es_info.aucmd != NULL
				  ? acp_script_ctx(entry->es_info.aucmd) : NULL;

		戻る def_ctx != NULL && def_ctx->sc_sid > 0
			   ? vim_strsave(SCRIPT_ITEM(def_ctx->sc_sid)->sn_name)
			   : NULL;
	    }
	    否则 もし (entry->es_type == ETYPE_SCRIPT)
		戻る vim_strsave(entry->es_name);
	}
	戻る NULL;
    }

    // Give information about each stack entry up to the root.
    // For a function we compose the call stack, as it was done in the past:
    //   "function One[123]..Two[456]..Three"
    ga_init2(&ga, sizeof(char), 100);
    ために (idx = 0; idx < exestack.ga_len; ++idx)
    {
	entry = ((estack_T *)exestack.ga_data) + idx;
	もし (entry->es_name != NULL)
	{
	    long	lnum = 0;
	    size_t	added = 0;
	    string_T	type_name = {(char_u *)"", 0};
	    string_T	class_name = {(char_u *)"", 0};
	    string_T	es_name = {entry->es_name, STRLEN(entry->es_name)};

	    もし (entry->es_type != last_type)
	    {
		switch (entry->es_type)
		{
		    case ETYPE_SCRIPT:
			STR_LITERAL_SET(type_name, "script ");
			break;
		    case ETYPE_UFUNC:
			STR_LITERAL_SET(type_name, "function ");
			break;
		    default:
			break;
		}
		last_type = entry->es_type;
	    }
	    もし (entry->es_type == ETYPE_UFUNC && entry->es_info.ufunc->uf_class != NULL)
	    {
		class_name.string = entry->es_info.ufunc->uf_class->class_name.string;
		class_name.length = entry->es_info.ufunc->uf_class->class_name.length;
	    }
	    もし (idx == exestack.ga_len - 1)
		lnum = which == ESTACK_STACK ? SOURCING_LNUM : 0;
	    否则
		lnum = entry->es_lnum;

	    len = es_name.length + type_name.length + class_name.length + 26;
	    もし (ga_grow(&ga, (整数)len) == FAIL)
		break;
	    ga_concat_len(&ga, type_name.string, type_name.length);
	    // For class methods prepend "<class name>." to the function name.
	    もし (*class_name.string != NUL)
	    {
		added = vim_snprintf_safelen(
		    (char *)ga.ga_data + ga.ga_len,
		    ga.ga_maxlen - ga.ga_len,
		    "<SNR>%d_%s.",
		    entry->es_info.ufunc->uf_script_ctx.sc_sid,
		    class_name.string);

		ga.ga_len += (整数)added;
	    }
	    ga_concat_len(&ga, es_name.string, es_name.length);
	    // For the bottom entry of <sfile>: do not add the line number, it is used in
	    // <slnum>.  Also leave it out when the number is not set.
	    もし (lnum != 0)
	    {
		added = vim_snprintf_safelen(
		    (char *)ga.ga_data + ga.ga_len,
		    ga.ga_maxlen - ga.ga_len,
		    "[%ld]",
		    lnum);

		ga.ga_len += (整数)added;
	    }
	    もし (idx != exestack.ga_len - 1)
		GA_CONCAT_LITERAL(&ga, "..");
	}
    }

    // Only NUL-terminate when not returning NULL.
    もし (ga.ga_data != NULL)
	ga_append(&ga, NUL);
    戻る (char_u *)ga.ga_data;
#endif
}

#ifdef FEAT_EVAL
    static void
stacktrace_push_item(
	list_T		*l,
	ufunc_T		*fp,
	char_u		*event,
	linenr_T	lnum,
	char_u		*filepath)
{
    dict_T	*d;
    typval_T	tv;

    d = dict_alloc_lock(VAR_FIXED);
    もし (d == NULL)
	戻る;

    tv.v_type = VAR_DICT;
    tv.v_lock = VAR_LOCKED;
    tv.vval.v_dict = d;

    もし (fp != NULL)
	dict_add_func(d, "funcref", fp);
    もし (event != NULL)
	dict_add_string(d, "event", event);
    dict_add_number(d, "lnum", lnum);
    dict_add_string(d, "filepath", filepath);

    list_append_tv(l, &tv);
}

/*
 * Create the stacktrace from exestack.
 */
    list_T *
stacktrace_create(void)
{
    list_T	*l;
    整数		i;

    l = list_alloc();
    もし (l == NULL)
	戻る NULL;

    ために (i = 0; i < exestack.ga_len; ++i)
    {
	estack_T *entry = &((estack_T *)exestack.ga_data)[i];
	linenr_T lnum = entry->es_lnum;

	もし (entry->es_type == ETYPE_SCRIPT)
	    stacktrace_push_item(l, NULL, NULL, lnum, entry->es_name);
	否则 もし (entry->es_type == ETYPE_UFUNC)
	{
	    ufunc_T *fp = entry->es_info.ufunc;
	    sctx_T sctx = fp->uf_script_ctx;
	    char_u *filepath = sctx.sc_sid > 0 ?
				   get_scriptname(sctx.sc_sid) : (char_u *)"";

	    lnum += sctx.sc_lnum;
	    stacktrace_push_item(l, fp, NULL, lnum, filepath);
	}
	否则 もし (entry->es_type == ETYPE_AUCMD)
	{
	    // The autocmd may not have a matching pattern yet, in which case
	    // es_info.aucmd is still NULL.
	    sctx_T *sctx = entry->es_info.aucmd != NULL
			       ? acp_script_ctx(entry->es_info.aucmd) : NULL;
	    char_u *filepath = sctx != NULL && sctx->sc_sid > 0 ?
				   get_scriptname(sctx->sc_sid) : (char_u *)"";

	    もし (sctx != NULL)
		lnum += sctx->sc_lnum;
	    stacktrace_push_item(l, NULL, entry->es_name, lnum, filepath);
	}
    }
    戻る l;
}

/*
 * getstacktrace() function
 */
    void
f_getstacktrace(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv_list_set(rettv, stacktrace_create());
}
#endif

/*
 * Get DIP_ flags from the [where] argument of a :runtime command.
 * "*argp" is advanced to after the [where] argument if it is found.
 */
    static 整数
get_runtime_cmd_flags(char_u **argp, size_t where_len)
{
    char_u *arg = *argp;

    もし (where_len == 0)
	戻る 0;

    もし (STRNCMP(arg, "START", where_len) == 0)
    {
	*argp = skipwhite(arg + where_len);
	戻る DIP_START + DIP_NORTP;
    }
    もし (STRNCMP(arg, "OPT", where_len) == 0)
    {
	*argp = skipwhite(arg + where_len);
	戻る DIP_OPT + DIP_NORTP;
    }
    もし (STRNCMP(arg, "PACK", where_len) == 0)
    {
	*argp = skipwhite(arg + where_len);
	戻る DIP_START + DIP_OPT + DIP_NORTP;
    }
    もし (STRNCMP(arg, "ALL", where_len) == 0)
    {
	*argp = skipwhite(arg + where_len);
	戻る DIP_START + DIP_OPT;
    }

    戻る 0;
}

/*
 * ":runtime [where] {name}"
 */
    void
ex_runtime(exarg_T *eap)
{
    char_u  *arg = eap->arg;
    整数	    flags = eap->forceit ? DIP_ALL : 0;
    char_u  *p = skiptowhite(arg);
    flags += get_runtime_cmd_flags(&arg, p - arg);
    source_runtime(arg, flags);
}

static 整数 runtime_expand_flags;

/*
 * Set the completion context for the :runtime command.
 */
    void
set_context_in_runtime_cmd(expand_T *xp, char_u *arg)
{
    char_u  *p = skiptowhite(arg);
    runtime_expand_flags
	= *p != NUL ? get_runtime_cmd_flags(&arg, p - arg) : 0;
    // Skip to the last argument.
    ながら (*(p = skiptowhite_esc(arg)) != NUL)
    {
	もし (runtime_expand_flags == 0)
	    // When there are multiple arguments and [where] is not specified,
	    // use an unrelated non-zero flag to avoid expanding [where].
	    runtime_expand_flags = DIP_ALL;
	arg = skipwhite(p);
    }
    xp->xp_context = EXPAND_RUNTIME;
    xp->xp_pattern = arg;
}

    static void
source_callback(char_u *fname, void *cookie)
{
    (void)do_source(fname, FALSE, DOSO_NONE, cookie);
}

#ifdef FEAT_EVAL
/*
 * Find an already loaded script "name".
 * If found returns its script ID.  If not found returns -1.
 */
    整数
find_script_by_name(char_u *name)
{
    整数		    sid;
    scriptitem_T    *si;

    ために (sid = script_items.ga_len; sid > 0; --sid)
    {
	// We used to check inode here, but that doesn't work:
	// - If a script is edited and written, it may get a different
	//   inode number, even though to the user it is the same script.
	// - If a script is deleted and another script is written, with a
	//   different name, the inode may be re-used.
	si = SCRIPT_ITEM(sid);
	もし (si->sn_name != NULL && fnamecmp(si->sn_name, name) == 0)
	    戻る sid;
    }
    戻る -1;
}

/*
 * Add a new scriptitem with all items initialized.
 * When running out of memory "error" is set to FAIL.
 * Returns the script ID.
 */
    static 整数
get_new_scriptitem(整数 *error)
{
    static scid_T   last_current_SID = 0;
    整数		    sid = ++last_current_SID;
    scriptitem_T    *si = NULL;

    もし (ga_grow(&script_items, (整数)(sid - script_items.ga_len)) == FAIL)
    {
	*error = FAIL;
	戻る sid;
    }
    ながら (script_items.ga_len < sid)
    {
	si = ALLOC_CLEAR_ONE(scriptitem_T);
	もし (si == NULL)
	{
	    *error = FAIL;
	    戻る sid;
	}
	++script_items.ga_len;
	SCRIPT_ITEM(script_items.ga_len) = si;
	si->sn_name = NULL;
	si->sn_version = 1;

	// Allocate the local script variables to use for this script.
	new_script_vars(script_items.ga_len);
	ga_init2(&si->sn_var_vals, sizeof(svar_T), 10);
	hash_init(&si->sn_all_vars.dv_hashtab);
	ga_init2(&si->sn_imports, sizeof(imported_T), 10);
	ga_init2(&si->sn_type_list, sizeof(type_T), 10);
# ifdef FEAT_PROFILE
	si->sn_prof_on = FALSE;
# endif
    }

    // "si" can't be NULL, check only to avoid a compiler warning
    もし (si != NULL)
	// Used to check script variable index is still valid.
	si->sn_script_seq = current_sctx.sc_seq;

    戻る sid;
}

    整数
get_new_scriptitem_for_fname(整数 *error, char_u *fname)
{
    整数 sid = get_new_scriptitem(error);

    もし (*error == OK)
    {
	scriptitem_T *si = SCRIPT_ITEM(sid);

	si->sn_name = vim_strsave(fname);
	si->sn_state = SN_STATE_NOT_LOADED;
    }
    戻る sid;
}

/*
 * If the script for "sid" is a symlink and "sn_source_sid" is not set
 * then initialize it. A new script_item is created if needed.
 */
    void
check_script_symlink(整数 sid)
{
    scriptitem_T *si = SCRIPT_ITEM(sid);
    もし (si->sn_syml_checked || si->sn_sourced_sid > 0)
	戻る;
    si->sn_syml_checked = TRUE;

    // If fname is a symbolic link, create an script_item for the real file.

    char_u *real_fname = fix_fname(si->sn_name);
    もし (real_fname != NULL && STRCMP(real_fname, si->sn_name) != 0)
    {
	整数 real_sid = find_script_by_name(real_fname);
	整数 error2 = OK;
	整数 new_sid = FALSE;
	もし (real_sid < 0)
	{
	    real_sid = get_new_scriptitem_for_fname(&error2, real_fname);
	    new_sid = TRUE;
	}
	もし (error2 == OK)
	{
	    si = SCRIPT_ITEM(sid);
	    si->sn_sourced_sid = real_sid;
	    もし (new_sid)
	    {
		SCRIPT_ITEM(real_sid)->sn_import_autoload
						    = si->sn_import_autoload;
		もし (si->sn_autoload_prefix != NULL)
		    SCRIPT_ITEM(real_sid)->sn_autoload_prefix =
					vim_strsave(si->sn_autoload_prefix);
	    }
	}
    }
    vim_free(real_fname);
}

    static void
find_script_callback(char_u *fname, void *cookie)
{
    整数 sid;
    整数 error = OK;
    整数 *ret_sid = cookie;

    sid = find_script_by_name(fname);
    もし (sid < 0)
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
    整数
do_in_path(
    char_u	*path,
    char	*prefix,
    char_u	*name,
    整数		flags,
    void	(*callback)(char_u *fname, void *ck),
    void	*cookie)
{
    char_u	*rtp;
    string_T	buf;
    char_u	*rtp_copy;
    整数		num_files;
    char_u	**files;
    整数		i;
    整数		did_one = FALSE;
#ifdef AMIGA
    struct Process	*proc = (struct Process *)FindTask(0L);
    APTR		save_winptr = proc->pr_WindowPtr;

    // Avoid a requester here for a volume that doesn't exist.
    proc->pr_WindowPtr = (APTR)-1L;
#endif

    // Make a copy of 'runtimepath'.  Invoking the callback may change the
    // value.
    rtp_copy = vim_strsave(path);
    buf.string = alloc(MAXPATHL);
    もし (buf.string != NULL && rtp_copy != NULL)
    {
	size_t	prefixlen = 0;
	size_t	namelen = 0;

	もし (name != NULL)
	{
	    prefixlen = STRLEN(prefix);
	    namelen = STRLEN(name);

	    もし (p_verbose > 10)
	    {
		verbose_enter();
		もし (*prefix != NUL)
		    smsg(_("Searching for \"%s\" under \"%s\" in \"%s\""),
					       (char *)name, prefix, (char *)path);
		否则
		    smsg(_("Searching for \"%s\" in \"%s\""),
						       (char *)name, (char *)path);
		verbose_leave();
	    }
	}

	// Loop over all entries in 'runtimepath'.
	rtp = rtp_copy;
	ながら (*rtp != NUL && ((flags & DIP_ALL) || !did_one))
	{
	    // Copy the path from 'runtimepath' to buf[].
	    buf.length = (size_t)copy_option_part(&rtp, buf.string, MAXPATHL, ",");

	    // Skip after or non-after directories.
	    もし (flags & (DIP_NOAFTER | DIP_AFTER))
	    {
		整数 is_after = buf.length >= 5
		    && STRCMP(buf.string + buf.length - 5, "after") == 0;

		もし ((is_after && (flags & DIP_NOAFTER))
			|| (!is_after && (flags & DIP_AFTER)))
		    continue;
	    }

	    もし (name == NULL)
	    {
		(*callback)(buf.string, (void *)&cookie);
		もし (!did_one)
		    did_one = (cookie == NULL);
	    }
	    否则 もし (buf.length + 2 + prefixlen + namelen < MAXPATHL)
	    {
		char_u	*np;
		char_u	*tail;

		もし (*buf.string != NUL
		    && !after_pathsep(buf.string, buf.string + buf.length))
		{
		    STRCPY(buf.string + buf.length, PATHSEPSTR);
		    buf.length += sizeof(PATHSEP);
		}
		STRCPY(buf.string + buf.length, prefix);
		buf.length += prefixlen;
		tail = buf.string + buf.length;

		// Loop over all patterns in "name"
		np = name;
		ながら (*np != NUL && ((flags & DIP_ALL) || !did_one))
		{
		    // Append the pattern from "name" to buf[].
		    copy_option_part(&np, tail, (整数)(MAXPATHL - (tail - buf.string)),
								       "\t ");

		    もし (p_verbose > 10)
		    {
			verbose_enter();
			smsg(_("Searching for \"%s\""), buf.string);
			verbose_leave();
		    }

		    // Expand wildcards, invoke the callback for each match.
		    もし (gen_expand_wildcards(1, &buf.string, &num_files, &files,
				  (flags & DIP_DIR) ? EW_DIR : EW_FILE) == OK)
		    {
			ために (i = 0; i < num_files; ++i)
			{
			    (*callback)(files[i], cookie);
			    did_one = TRUE;
			    もし (!(flags & DIP_ALL))
				break;
			}
			FreeWild(num_files, files);
		    }
		}
	    }
	}
    }
    vim_free(buf.string);
    vim_free(rtp_copy);
    もし (!did_one && name != NULL)
    {
	char *basepath = path == p_rtp ? "runtimepath" : "packpath";

	もし (flags & DIP_ERR)
	    semsg(_(e_directory_not_found_in_str_str), basepath, name);
	否则 もし (p_verbose > 0)
	{
	    verbose_enter();
	    smsg(_("not found in '%s': \"%s\""), basepath, name);
	    verbose_leave();
	}
    }

#ifdef AMIGA
    proc->pr_WindowPtr = save_winptr;
#endif

    戻る did_one ? OK : FAIL;
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
    static 整数
do_in_path_and_pp(
    char_u	*path,
    char_u	*name,
    整数		flags,
    void	(*callback)(char_u *fname, void *ck),
    void	*cookie)
{
    整数		done = FAIL;

    もし ((flags & DIP_NORTP) == 0)
	done = do_in_path(path, "", name, flags, callback, cookie);

    もし ((done == FAIL || (flags & DIP_ALL)) && (flags & DIP_START))
	done = do_in_path(p_pp, "pack/*/start/*/", name, flags, callback,
								       cookie);

    もし ((done == FAIL || (flags & DIP_ALL)) && (flags & DIP_OPT))
	done = do_in_path(p_pp, "pack/*/opt/*/", name, flags, callback,
								       cookie);

    戻る done;
}

/*
 * Just like do_in_path_and_pp(), using 'runtimepath' for "path".
 */
    整数
do_in_runtimepath(
    char_u	*name,
    整数		flags,
    void	(*callback)(char_u *fname, void *ck),
    void	*cookie)
{
    戻る do_in_path_and_pp(p_rtp, name, flags, callback, cookie);
}

/*
 * Source the file "name" from all directories in 'runtimepath'.
 * "name" can contain wildcards.
 * When "flags" has DIP_ALL: source all files, otherwise only the first one.
 *
 * return FAIL when no file could be sourced, OK otherwise.
 */
    整数
source_runtime(char_u *name, 整数 flags)
{
    戻る source_in_path(p_rtp, name, flags, NULL);
}

/*
 * Just like source_runtime(), but use "path" instead of 'runtimepath'
 * and return the script ID in "ret_sid".
 */
    整数
source_in_path(char_u *path, char_u *name, 整数 flags, 整数 *ret_sid)
{
    戻る do_in_path_and_pp(path, name, flags, source_callback, ret_sid);
}

#もし defined(FEAT_EVAL)

/*
 * Find "name" in 'runtimepath'. If found a new scriptitem is created for it
 * and it's script ID is returned.
 * If not found returns -1.
 */
    整数
find_script_in_rtp(char_u *name)
{
    整数 sid = -1;

    (void)do_in_path_and_pp(p_rtp, name, DIP_START | DIP_NOAFTER,
						   find_script_callback, &sid);
    戻る sid;
}

/*
 * Expand wildcards in "pat" and invoke do_source() for each match.
 */
    static void
source_all_matches(char_u *pat)
{
    整数	    num_files;
    char_u  **files;
    整数	    i;

    もし (gen_expand_wildcards(1, &pat, &num_files, &files, EW_FILE) != OK)
	戻る;

    ために (i = 0; i < num_files; ++i)
	(void)do_source(files[i], FALSE, DOSO_NONE, NULL);
    FreeWild(num_files, files);
}

/*
 * Add the package directory to 'runtimepath'.
 */
    static 整数
add_pack_dir_to_rtp(char_u *fname)
{
    char_u  *p4, *p3, *p2, *p1, *p;
    char_u  *entry;
    char_u  *insp = NULL;
    整数	    c;
    string_T  new_rtp;
    size_t  keep;
    size_t  p_rtp_len;
    string_T  afterdir = {NULL, 0};
    char_u  *after_insp = NULL;
    char_u  *ffname = NULL;
    size_t  fname_len;
    string_T	buf = {NULL, 0};
    整数	    retval = FAIL;

    p4 = p3 = p2 = p1 = get_past_head(fname);
    ために (p = p1; *p; MB_PTR_ADV(p))
	もし (vim_ispathsep_nocolon(*p))
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
    もし (ffname == NULL)
	戻る FAIL;

    // Find "ffname" in "p_rtp", ignoring '/' vs '\' differences.
    // Also stop at the first "after" directory.
    fname_len = STRLEN(ffname);
    buf.string = alloc(MAXPATHL);
    もし (buf.string == NULL)
	goto theend;

    p_rtp_len = 0;
    ために (entry = p_rtp; *entry != NUL; )
    {
	char_u *cur_entry = entry;

	buf.length = (size_t)copy_option_part(&entry, buf.string, MAXPATHL, ",");

	// keep track of p_rtp length as we go to make the STRLEN() below have less work to do
	p_rtp_len += (*(cur_entry + buf.length) == ',') ? buf.length + 1 : buf.length;

	もし ((p = (char_u *)strstr((char *)buf.string, "after")) != NULL
		&& p > buf.string
		&& vim_ispathsep(p[-1])
		&& (vim_ispathsep(p[5]) || p[5] == NUL || p[5] == ','))
	{
	    もし (insp == NULL)
		// Did not find "ffname" before the first "after" directory,
		// insert it before this entry.
		insp = cur_entry;
	    after_insp = cur_entry;
	    break;
	}

	もし (insp == NULL)
	{
	    char_u  *rtp_ffname;
	    整数	    match;

	    もし (*buf.string != NUL && !after_pathsep(buf.string, buf.string + buf.length))
		STRCPY(buf.string + buf.length, PATHSEPSTR);
	    rtp_ffname = fix_fname(buf.string);
	    もし (rtp_ffname == NULL)
		goto theend;
	    match = vim_fnamencmp(rtp_ffname, ffname, fname_len) == 0;
	    vim_free(rtp_ffname);
	    もし (match)
		// Insert "ffname" after this entry (and comma).
		insp = entry;
	}
    }

    // finish measuring the length of p_rtp
    p_rtp_len += STRLEN(p_rtp + p_rtp_len);
    もし (insp == NULL)
	// Both "fname" and "after" not found, append at the end.
	insp = p_rtp + p_rtp_len;

    // check if rtp/pack/name/start/name/after exists
    fname_len = STRLEN(fname);
    concat_fnames(fname, fname_len, (char_u *)"after", STRLEN_LITERAL("after"), TRUE, &afterdir);
    もし (afterdir.string == NULL || !mch_isdir(afterdir.string))
	afterdir.length = 0;

    new_rtp.string = alloc(p_rtp_len + fname_len + afterdir.length + 3); // add two for commas and one for NUL
    もし (new_rtp.string == NULL)
	goto theend;

    // We now have 'rtp' parts: {keep}{keep_after}{rest}.
    // Create new_rtp, first: {keep},{fname}
    keep = (size_t)(insp - p_rtp);
    mch_memmove(new_rtp.string, p_rtp, keep);
    new_rtp.length = keep;
    もし (*insp == NUL)
	new_rtp.string[new_rtp.length++] = ',';  // add comma before
    mch_memmove(new_rtp.string + new_rtp.length, fname, fname_len);
    new_rtp.length += fname_len;
    もし (*insp != NUL)
	new_rtp.string[new_rtp.length++] = ',';  // add comma after

    もし (afterdir.length > 0 && after_insp != NULL)
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

    もし (p_rtp[keep] != NUL)
    {
	size_t	append_len = p_rtp_len - keep;

	// Append rest: {keep},{fname}{keep_after},{afterdir}{rest}
	mch_memmove(new_rtp.string + new_rtp.length, p_rtp + keep, append_len + 1);	// add one for NUL
	new_rtp.length += append_len;
    }
    否则
	new_rtp.string[new_rtp.length] = NUL;

    もし (afterdir.length > 0 && after_insp == NULL)
    {
	// Append afterdir when "after" was not found:
	// {keep},{fname}{rest},{afterdir}
	new_rtp.string[new_rtp.length++] = ',';
	STRCPY(new_rtp.string + new_rtp.length, afterdir.string);
    }

    set_option_value_give_err((char_u *)"rtp", 0L, new_rtp.string, 0);
    vim_free(new_rtp.string);
    retval = OK;

theend:
    vim_free(buf.string);
    vim_free(ffname);
    vim_free(afterdir.string);
    戻る retval;
}

/*
 * Load scripts in "plugin" and "ftdetect" directories of the package.
 */
    static 整数
load_pack_plugin(char_u *fname)
{
    static char *plugpat = "%s/plugin/**/*.vim";
    static char *ftpat = "%s/ftdetect/*.vim";
    整数		len;
    char_u	*ffname = fix_fname(fname);
    char_u	*pat = NULL;
    整数		retval = FAIL;

    もし (ffname == NULL)
	戻る FAIL;
    len = (整数)STRLEN(ffname) + (整数)STRLEN(ftpat);
    pat = alloc(len);
    もし (pat == NULL)
	goto theend;
    vim_snprintf((char *)pat, len, plugpat, ffname);
    source_all_matches(pat);

    {
	char_u *cmd = vim_strsave((char_u *)"g:did_load_filetypes");

	// If runtime/filetype.vim wasn't loaded yet, the scripts will be
	// found when it loads.
	もし (cmd != NULL && eval_to_number(cmd, FALSE) > 0)
	{
	    do_cmdline_cmd((char_u *)"augroup filetypedetect");
	    vim_snprintf((char *)pat, len, ftpat, ffname);
	    source_all_matches(pat);
	    do_cmdline_cmd((char_u *)"augroup END");
	}
	vim_free(cmd);
    }
    vim_free(pat);
    retval = OK;

theend:
    vim_free(ffname);
    戻る retval;
}

// used for "cookie" of add_pack_plugin()
static 整数 APP_ADD_DIR;
static 整数 APP_LOAD;
static 整数 APP_BOTH;

    static void
add_pack_plugin(char_u *fname, void *cookie)
{
    もし (cookie != &APP_LOAD)
    {
	char_u	*buf = alloc(MAXPATHL);
	char_u	*p;
	整数	found = FALSE;

	もし (buf == NULL)
	    戻る;
	p = p_rtp;
	ながら (*p != NUL)
	{
	    copy_option_part(&p, buf, MAXPATHL, ",");
	    もし (pathcmp((char *)buf, (char *)fname, -1) == 0)
	    {
		found = TRUE;
		break;
	    }
	}
	vim_free(buf);
	もし (!found)
	    // directory is not yet in 'runtimepath', add it
	    もし (add_pack_dir_to_rtp(fname) == FAIL)
		戻る;
    }

    もし (cookie != &APP_ADD_DIR)
	load_pack_plugin(fname);
}

/*
 * Add all packages in the "start" directory to 'runtimepath'.
 */
    void
add_pack_start_dirs(void)
{
    do_in_path(p_pp, "", (char_u *)"pack/*/start/*", DIP_ALL + DIP_DIR,
					       add_pack_plugin, &APP_ADD_DIR);
}

/*
 * Load plugins from all packages in the "start" directory.
 */
    void
load_start_packages(void)
{
    did_source_packages = TRUE;
    do_in_path(p_pp, "", (char_u *)"pack/*/start/*", DIP_ALL + DIP_DIR,
						  add_pack_plugin, &APP_LOAD);
}

/*
 * ":packloadall"
 * Find plugins in the package directories and source them.
 */
    void
ex_packloadall(exarg_T *eap)
{
    もし (!did_source_packages || eap->forceit)
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
    void
ex_packadd(exarg_T *eap)
{
    static char *plugpat = "pack/*/%s/%s";
    整数		len;
    char	*pat;
    整数		round;
    整数		res = OK;

    // Round 1: use "start", round 2: use "opt".
    ために (round = 1; round <= 2; ++round)
    {
	// Only look under "start" when loading packages wasn't done yet.
	もし (round == 1 && did_source_packages)
	    continue;

	len = (整数)STRLEN(plugpat) + (整数)STRLEN(eap->arg) + 5;
	pat = alloc(len);
	もし (pat == NULL)
	    戻る;
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
    void
remove_duplicates(garray_T *gap)
{
    整数	    i;
    整数	    j;
    char_u  **fnames = (char_u **)gap->ga_data;

    sort_strings(fnames, gap->ga_len);
    ために (i = gap->ga_len - 1; i > 0; --i)
	もし (fnamecmp(fnames[i - 1], fnames[i]) == 0)
	{
	    vim_free(fnames[i]);
	    ために (j = i + 1; j < gap->ga_len; ++j)
		fnames[j - 1] = fnames[j];
	    --gap->ga_len;
	}
}

    static void
ExpandRTDir_int(
    char_u	*pat,
    size_t	pat_len,
    整数		flags,
    整数		keep_ext,
    garray_T	*gap,
    char	*dirnames[])
{
    ために (整数 i = 0; dirnames[i] != NULL; ++i)
    {
	const size_t	buf_len = STRLEN(dirnames[i]) + pat_len + 64;
	char		*buf = alloc(buf_len);
	もし (buf == NULL)
	{
	    ga_clear_strings(gap);
	    戻る;
	}
	整数		glob_flags = 0;
	整数		expand_dirs = FALSE;

	// Build base pattern
	vim_snprintf(buf, buf_len, "%s%s%s%s",
		     *dirnames[i] ? dirnames[i] : "", *dirnames[i] ? "/" : "",
		     pat, "*.vim");

expand:
	もし ((flags & DIP_NORTP) == 0)
	    globpath(p_rtp, (char_u *)buf, gap, glob_flags, expand_dirs);

	もし (flags & DIP_START)
	{
	    // Build complete search path: pack/*/start/*/dirnames[i]/pat*.vim
	    vim_snprintf(buf, buf_len, "pack/*/start/*/%s%s%s%s",
			 *dirnames[i] ? dirnames[i] : "",
			 *dirnames[i] ? "/" : "",
			 pat,
			 expand_dirs ? "*" : "*.vim");
	    globpath(p_pp, (char_u *)buf, gap, glob_flags, expand_dirs);
	}

	もし (flags & DIP_OPT)
	{
	    // Build complete search path: pack/*/opt/*/dirnames[i]/pat*.vim
	    vim_snprintf(buf, buf_len, "pack/*/opt/*/%s%s%s%s",
			 *dirnames[i] ? dirnames[i] : "",
			 *dirnames[i] ? "/" : "", pat,
			 expand_dirs ? "*" : "*.vim");
	    globpath(p_pp, (char_u *)buf, gap, glob_flags, expand_dirs);
	}

	// Second round for directories
	もし (*dirnames[i] == NUL && !expand_dirs)
	{
	    // expand dir names in another round
	    vim_snprintf(buf, buf_len, "%s*", pat);
	    glob_flags = WILD_ADD_SLASH;
	    expand_dirs = TRUE;
	    goto expand;
	}

	vim_free(buf);
    }

    整数 pat_pathsep_cnt = 0;
    ために (size_t i = 0; i < pat_len; ++i)
    {
	もし (vim_ispathsep(pat[i]))
	    ++pat_pathsep_cnt;
    }

    ために (整数 i = 0; i < gap->ga_len; ++i)
    {
	char_u *match = ((char_u **)gap->ga_data)[i];
	char_u *s = match;
	char_u *e = s + STRLEN(s);
	もし (e - 4 > s && !keep_ext && STRNICMP(e - 4, ".vim", 4) == 0)
	{
	    e -= 4;
	    *e = NUL;
	}

	整数 match_pathsep_cnt = (e > s && e[-1] == '/') ? -1 : 0;
	ために (s = e; s > match; MB_PTR_BACK(match, s))
	{
	    もし (s < match || (vim_ispathsep(*s)
				     && ++match_pathsep_cnt > pat_pathsep_cnt))
		break;
	}
	++s;
	もし (s != match)
	    mch_memmove(match, s, e - s + 1);
    }

    もし (gap->ga_len == 0)
	戻る;

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
    整数
ExpandRTDir(
    char_u	*pat,
    整数		flags,
    整数		*num_file,
    char_u	***file,
    char	*dirnames[])
{
    *num_file = 0;
    *file = NULL;

    garray_T	ga;
    ga_init2(&ga, sizeof(char *), 10);

    ExpandRTDir_int(pat, STRLEN(pat), flags, FALSE, &ga, dirnames);

    もし (ga.ga_len == 0)
	戻る FAIL;

    *file = ga.ga_data;
    *num_file = ga.ga_len;
    戻る OK;
}

/*
 * Handle command line completion for :runtime command.
 */
    整数
expand_runtime_cmd(char_u *pat, 整数 *numMatches, char_u ***matches)
{
    *numMatches = 0;
    *matches = NULL;

    garray_T	ga;
    ga_init2(&ga, sizeof(char *), 10);

    size_t pat_len = (整数)STRLEN(pat);
    char *dirnames[] = {"", NULL};
    ExpandRTDir_int(pat, pat_len, runtime_expand_flags, TRUE, &ga, dirnames);

    // Try to complete values for [where] argument when none was found.
    もし (runtime_expand_flags == 0)
    {
	char *where_values[] = {"START", "OPT", "PACK", "ALL"};
	ために (size_t i = 0; i < ARRAY_LENGTH(where_values); ++i)
	    もし (STRNCMP(pat, where_values[i], pat_len) == 0)
	    {
		char_u *p = vim_strsave((char_u *)where_values[i]);
		もし (p != NULL && ga_add_string(&ga, p) == FAIL)
		    vim_free(p);
	    }
    }

    もし (ga.ga_len == 0)
	戻る FAIL;

    *matches = ga.ga_data;
    *numMatches = ga.ga_len;
    戻る OK;
}

/*
 * Expand loadplugin names:
 * 'packpath'/pack/ * /opt/{pat}
 */
    整数
ExpandPackAddDir(
    char_u	*pat,
    整数		*num_file,
    char_u	***file)
{
    char_u	*s;
    char_u	*e;
    char_u	*match;
    garray_T	ga;
    整数		i;
    整数		pat_len;

    *num_file = 0;
    *file = NULL;
    pat_len = (整数)STRLEN(pat);
    ga_init2(&ga, sizeof(char *), 10);

    s = alloc(pat_len + 26);
    もし (s == NULL)
    {
	ga_clear_strings(&ga);
	戻る FAIL;
    }
    sprintf((char *)s, "pack/*/opt/%s*", pat);
    globpath(p_pp, s, &ga, 0, TRUE);
    vim_free(s);

    ために (i = 0; i < ga.ga_len; ++i)
    {
	match = ((char_u **)ga.ga_data)[i];
	s = gettail(match);
	e = s + STRLEN(s);
	mch_memmove(match, s, e - s + 1);
    }

    もし (ga.ga_len == 0)
	戻る FAIL;

    // Sort and remove duplicates which can happen when specifying multiple
    // directories in dirnames.
    remove_duplicates(&ga);

    *file = ga.ga_data;
    *num_file = ga.ga_len;
    戻る OK;
}

    static void
cmd_source(char_u *fname, exarg_T *eap)
{
    整数 clearvars = FALSE;

    もし (*fname != NUL && STRNCMP(fname, "++clear", 7) == 0)
    {
	// ++clear argument is supplied
	clearvars = TRUE;
	fname = fname + 7;
	もし (*fname != NUL)
	{
	    semsg(_(e_invalid_argument_str), eap->arg);
	    戻る;
	}
    }

    もし (*fname != NUL && eap != NULL && eap->addr_count > 0)
    {
	// if a filename is specified to :source, then a range is not allowed
	emsg(_(e_no_range_allowed));
	戻る;
    }

    もし (eap != NULL && *fname == NUL)
    {
	もし (eap->forceit)
	    // a file name is needed to source normal mode commands
	    emsg(_(e_argument_required));
	否则
	    // source ex commands from the current buffer
	    do_source_ext(NULL, FALSE, DOSO_NONE, NULL, eap, clearvars);
    }
    否则 もし (eap != NULL && eap->forceit)
	// ":source!": read Normal mode commands
	// Need to execute the commands directly.  This is required at least
	// for:
	// - ":g" command busy
	// - after ":argdo", ":windo" or ":bufdo"
	// - another command follows
	// - inside a loop
	openscript(fname, global_busy || listcmd_busy || eap->nextcmd != NULL
#ifdef FEAT_EVAL
						 || eap->cstack->cs_idx >= 0
#endif
						 );

    // ":source" read ex commands
    否则 もし (do_source(fname, FALSE, DOSO_NONE, NULL) == FAIL)
	semsg(_(e_cant_open_file_str), fname);
}

/*
 * ":source {fname}"
 */
    void
ex_source(exarg_T *eap)
{
#ifdef FEAT_BROWSE
    もし (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	char_u *fname = NULL;

	fname = do_browse(0, (char_u *)_("Source Vim script"), eap->arg,
				      NULL, NULL,
				      (char_u *)_(BROWSE_FILTER_MACROS), NULL);
	もし (fname != NULL)
	{
	    cmd_source(fname, eap);
	    vim_free(fname);
	}
    }
    否则
#endif
	cmd_source(eap->arg, eap);
}

#もし defined(FEAT_EVAL)
/*
 * ":options"
 */
    void
ex_options(
    exarg_T	*eap UNUSED)
{
    char_u  buf[500];
    整数	    multi_mods = 0;

    buf[0] = NUL;
    (void)add_win_cmd_modifiers(buf, &cmdmod, &multi_mods);

    vim_setenv((char_u *)"OPTWIN_CMD", buf);
    cmd_source((char_u *)SYS_OPTWIN_FILE, NULL);
}
#endif

/*
 * ":source" and associated commands.
 */

#もし defined(FEAT_EVAL)
/*
 * Return the address holding the next breakpoint line for a source cookie.
 */
    linenr_T *
source_breakpoint(void *cookie)
{
    戻る &((source_cookie_T *)cookie)->breakpoint;
}

/*
 * Return the address holding the debug tick for a source cookie.
 */
    整数 *
source_dbg_tick(void *cookie)
{
    戻る &((source_cookie_T *)cookie)->dbg_tick;
}

/*
 * Return the nesting level for a source cookie.
 */
    整数
source_level(void *cookie)
{
    戻る ((source_cookie_T *)cookie)->level;
}

/*
 * Return the readahead line. Note that the pointer may become invalid when
 * getting the next line, if it's concatenated with the next one.
 */
    char_u *
source_nextline(void *cookie)
{
    戻る ((source_cookie_T *)cookie)->nextline;
}
#endif

#もし (defined(MSWIN) && defined(FEAT_CSCOPE)) || defined(HAVE_FD_CLOEXEC)
# define USE_FOPEN_NOINH
/*
 * Special function to open a file without handle inheritance.
 * When possible the handle is closed on exec().
 */
    static FILE *
fopen_noinh_readbin(char *filename)
{
# ifdef MSWIN
    整数	fd_tmp = mch_open(filename, O_RDONLY | O_BINARY | O_NOINHERIT, 0);
# 否则
    整数	fd_tmp = mch_open(filename, O_RDONLY, 0);
# endif

    もし (fd_tmp == -1)
	戻る NULL;

# ifdef HAVE_FD_CLOEXEC
    {
	整数 fdflags = fcntl(fd_tmp, F_GETFD);
	もし (fdflags >= 0 && (fdflags & FD_CLOEXEC) == 0)
	    (void)fcntl(fd_tmp, F_SETFD, fdflags | FD_CLOEXEC);
    }
# endif

    戻る fdopen(fd_tmp, READBIN);
}
#endif

/*
 * Initialization for sourcing lines from the current buffer. Reads all the
 * lines from the buffer and stores it in the cookie grow array.
 * Returns a pointer to the name ":source buffer=<n>" on success and NULL on
 * failure.
 */
    static char_u *
do_source_buffer_init(source_cookie_T *sp, exarg_T *eap)
{
    linenr_T	curr_lnum;
    char_u	*line = NULL;
    char_u	*fname;

    もし (curbuf == NULL)
	戻る NULL;

    // Use ":source buffer=<num>" as the script name
    もし (curbuf->b_ffname != NULL)
	fname = vim_strsave(curbuf->b_ffname);
    否则
    {
	vim_snprintf((char *)IObuff, IOSIZE, ":source buffer=%d", curbuf->b_fnum);
	fname = vim_strsave(IObuff);
    }
    もし (fname == NULL)
	戻る NULL;

    ga_init2(&sp->buflines, sizeof(char_u *), 100);

    // Copy the lines from the buffer into a grow array
    ために (curr_lnum = eap->line1; curr_lnum <= eap->line2; curr_lnum++)
    {
	line = vim_strsave(ml_get(curr_lnum));
	もし (line == NULL)
	    goto errret;
	もし (ga_add_string(&sp->buflines, line) == FAIL)
	    goto errret;
    }
    sp->buf_lnum = 0;
    sp->source_from_buf = TRUE;
    // When sourcing a range of lines from a buffer, use buffer line number.
    sp->sourcing_lnum = eap->line1 - 1;

    戻る fname;

errret:
    vim_free(fname);
    vim_free(line);
    ga_clear_strings(&sp->buflines);
    戻る NULL;
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
    static 整数
do_source_ext(
    char_u	*fname,
    整数		check_other,	    // check for .vimrc and _vimrc
    整数		is_vimrc,	    // DOSO_ value
    整数		*ret_sid UNUSED,
    exarg_T	*eap,
    整数		clearvars UNUSED)
{
    source_cookie_T	    cookie;
    char_u		    *p;
    char_u		    *fname_not_fixed = NULL;
    char_u		    *fname_exp = NULL;
    char_u		    *firstline = NULL;
    整数			    retval = FAIL;
    sctx_T		    save_current_sctx;
#ifdef STARTUPTIME
    struct timeval	    tv_rel;
    struct timeval	    tv_start;
#endif
#ifdef FEAT_PROFILE
    proftime_T		    wait_start;
#endif
    整数			    save_sticky_cmdmod_flags = sticky_cmdmod_flags;
    整数			    trigger_source_post = FALSE;
#ifdef FEAT_EVAL
    funccal_entry_T	    funccalp_entry;
    整数			    save_debug_break_level = debug_break_level;
    整数			    sid = -1;
    scriptitem_T	    *si = NULL;
    整数			    save_estack_compiling = estack_compiling;
    ESTACK_CHECK_DECLARATION;
#endif

    CLEAR_FIELD(cookie);
    もし (fname == NULL)
    {
	// sourcing lines from a buffer
	fname_exp = do_source_buffer_init(&cookie, eap);
	もし (fname_exp == NULL)
	    戻る FAIL;
    }
    否则
    {
	fname_not_fixed = expand_env_save(fname);
	もし (fname_not_fixed == NULL)
	    goto theend;
	fname_exp = fix_fname(fname_not_fixed);
	もし (fname_exp == NULL)
	    goto theend;
	もし (mch_isdir(fname_exp))
	{
	    smsg(_("Cannot source a directory: \"%s\""), fname);
	    goto theend;
	}
    }
#ifdef FEAT_EVAL
    estack_compiling = FALSE;

    // See if we loaded this script before.
    sid = find_script_by_name(fname_exp);
    もし (sid > 0 && ret_sid != NULL
			  && SCRIPT_ITEM(sid)->sn_state != SN_STATE_NOT_LOADED)
    {
	// Already loaded and no need to load again, return here.
	*ret_sid = sid;
	retval = OK;
	goto theend;
    }
#endif

    // Apply SourceCmd autocommands, they should get the file and source it.
    // When sourcing a file "fname_exp" was made a full path name by
    // fix_fname() above, avoid expanding it again.
    もし ((fname == NULL
		? has_autocmd(EVENT_SOURCECMD, fname_exp, NULL)
		: has_autocmd_fullname(EVENT_SOURCECMD, fname_exp, NULL))
	    && apply_autocmds(EVENT_SOURCECMD, fname_exp, fname_exp,
							       FALSE, curbuf))
    {
#ifdef FEAT_EVAL
	retval = aborting() ? FAIL : OK;
#否则
	retval = OK;
#endif
	もし (retval == OK)
	    // Apply SourcePost autocommands.
	    apply_autocmds(EVENT_SOURCEPOST, fname_exp, fname_exp,
								FALSE, curbuf);
	goto theend;
    }

    // Apply SourcePre autocommands, they may get the file.
    apply_autocmds(EVENT_SOURCEPRE, fname_exp, fname_exp, FALSE, curbuf);

    もし (!cookie.source_from_buf)
    {
#ifdef USE_FOPEN_NOINH
	cookie.fp = fopen_noinh_readbin((char *)fname_exp);
#否则
	cookie.fp = mch_fopen((char *)fname_exp, READBIN);
#endif
    }
    もし (cookie.fp == NULL && check_other)
    {
	// Try again, replacing file name ".vimrc" by "_vimrc" or vice versa,
	// and ".exrc" by "_exrc" or vice versa.
	p = gettail(fname_exp);
	もし ((*p == '.' || *p == '_')
		&& (STRICMP(p + 1, "vimrc") == 0
		    || STRICMP(p + 1, "gvimrc") == 0
		    || STRICMP(p + 1, "exrc") == 0))
	{
	    もし (*p == '_')
		*p = '.';
	    否则
		*p = '_';
#ifdef USE_FOPEN_NOINH
	    cookie.fp = fopen_noinh_readbin((char *)fname_exp);
#否则
	    cookie.fp = mch_fopen((char *)fname_exp, READBIN);
#endif
	}
    }

    もし (cookie.fp == NULL && !cookie.source_from_buf)
    {
	もし (p_verbose > 0)
	{
	    verbose_enter();
	    もし (SOURCING_NAME == NULL)
		smsg(_("could not source \"%s\""), fname);
	    否则
		smsg(_("line %ld: could not source \"%s\""),
							SOURCING_LNUM, fname);
	    verbose_leave();
	}
	goto theend;
    }

    // The file exists.
    // - In verbose mode, give a message.
    // - For a vimrc file, may want to set 'compatible', call vimrc_found().
    もし (p_verbose > 1)
    {
	verbose_enter();
	もし (SOURCING_NAME == NULL)
	    smsg(_("sourcing \"%s\""), fname);
	否则
	    smsg(_("line %ld: sourcing \"%s\""), SOURCING_LNUM, fname);
	verbose_leave();
    }
    もし (is_vimrc == DOSO_VIMRC)
	vimrc_found(fname_exp, (char_u *)"MYVIMRC");
    否则 もし (is_vimrc == DOSO_GVIMRC)
	vimrc_found(fname_exp, (char_u *)"MYGVIMRC");

#ifdef USE_CRNL
    // If no automatic file format: Set default to CR-NL.
    もし (*p_ffs == NUL)
	cookie.fileformat = EOL_DOS;
    否则
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
    もし (time_fd != NULL)
	time_push(&tv_rel, &tv_start);
#endif

    // "legacy" does not apply to commands in the script
    sticky_cmdmod_flags = 0;

    save_current_sctx = current_sctx;
    もし ((cmdmod.cmod_flags & CMOD_VIM9CMD) && cookie.source_from_buf)
	// When the ":vim9cmd" command modifier is used, source buffer lines as
	// Vim9 script
	current_sctx.sc_version = SCRIPT_VERSION_VIM9;
    否则
	current_sctx.sc_version = 1;  // default script version

#ifdef FEAT_EVAL
    current_sctx.sc_lnum = 0;

# ifdef FEAT_PROFILE
    もし (do_profiling == PROF_YES)
	prof_child_enter(&wait_start);		// entering a child now
# endif

    // Don't use local function variables, if called from a function.
    // Also starts profiling timer for nested script.
    save_funccal(&funccalp_entry);

    // Reset "KeyTyped" to avoid some commands thinking they are invoked
    // interactively.  E.g. defining a function would output indent.
    整数 save_KeyTyped = KeyTyped;
    KeyTyped = FALSE;

    // Check if this script was sourced before to find its SID.
    // Always use a new sequence number.
    current_sctx.sc_seq = ++last_current_SID_seq;
    もし (sid > 0)
    {
	hashtab_T	*ht;
	整数		todo;
	hashitem_T	*hi;
	dictitem_T	*di;

	// loading the same script again
	current_sctx.sc_sid = sid;
	si = SCRIPT_ITEM(sid);
	もし (si->sn_state == SN_STATE_NOT_LOADED)
	{
	    // this script was found but not loaded yet
	    si->sn_state = SN_STATE_NEW;
	}
	否则
	{
	    si->sn_state = SN_STATE_RELOAD;

	    もし (!clearvars)
	    {
		// Script-local variables remain but "const" can be set again.
		// In Vim9 script variables will be cleared when "vim9script"
		// is encountered without the "noclear" argument.
		ht = &SCRIPT_VARS(sid);
		todo = (整数)ht->ht_used;
		FOR_ALL_HASHTAB_ITEMS(ht, hi, todo)
		    もし (!HASHITEM_EMPTY(hi))
		    {
			--todo;
			di = HI2DI(hi);
			di->di_flags |= DI_FLAGS_RELOAD;
		    }
		// imports can be redefined once
		mark_imports_for_reload(sid);
	    }
	    否则
		clear_vim9_scriptlocal_vars(sid);

	    // reset version, "vim9script" may have been added or removed.
	    si->sn_version = 1;
	}
	もし (ret_sid != NULL)
	    *ret_sid = sid;
    }
    否则
    {
	整数 error = OK;

	// It's new, generate a new SID and initialize the scriptitem.
	sid = get_new_scriptitem(&error);
	current_sctx.sc_sid = sid;
	もし (error == FAIL)
	    goto almosttheend;
	si = SCRIPT_ITEM(sid);
	si->sn_name = fname_exp;
	fname_exp = vim_strsave(si->sn_name);  // used for autocmd
	もし (ret_sid != NULL)
	    *ret_sid = sid;

	// Remember the "is_vimrc" flag for when the file is sourced again.
	si->sn_is_vimrc = is_vimrc;
    }

    // Keep the sourcing name/lnum, for recursive calls.
    estack_push(ETYPE_SCRIPT, si->sn_name, 0);
    ESTACK_CHECK_SETUP;

# ifdef FEAT_PROFILE
    もし (do_profiling == PROF_YES)
    {
	整数	forceit;

	// Check if we do profiling for this script.
	もし (!si->sn_prof_on && has_profiling(TRUE, si->sn_name, &forceit, NULL))
	{
	    script_do_profile(si);
	    si->sn_pr_force = forceit;
	}
	もし (si->sn_prof_on)
	{
	    ++si->sn_pr_count;
	    profile_start(&si->sn_pr_start);
	    profile_zero(&si->sn_pr_children);
	}
    }
# endif
#否则
    // Keep the sourcing name/lnum, for recursive calls.
    estack_push(ETYPE_SCRIPT, fname_exp, 0);
#endif

    cookie.conv.vc_type = CONV_NONE;		// no conversion

    // Read the first line so we can check for a UTF-8 BOM.
    firstline = getsourceline(0, (void *)&cookie, 0, TRUE);
    もし (firstline != NULL && STRLEN(firstline) >= 3 && firstline[0] == 0xef
			      && firstline[1] == 0xbb && firstline[2] == 0xbf)
    {
	// Found BOM; setup conversion, skip over BOM and recode the line.
	convert_setup(&cookie.conv, (char_u *)"utf-8", p_enc);
	p = string_convert(&cookie.conv, firstline + 3, NULL);
	もし (p == NULL)
	    p = vim_strsave(firstline + 3);
	もし (p != NULL)
	{
	    vim_free(firstline);
	    firstline = p;
	}
    }

    // Call do_cmdline, which will call getsourceline() to get the lines.
    do_cmdline(firstline, getsourceline, (void *)&cookie,
				     DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_REPEAT);
    retval = OK;

#ifdef FEAT_PROFILE
    もし (do_profiling == PROF_YES)
    {
	// Get "si" again, "script_items" may have been reallocated.
	si = SCRIPT_ITEM(sid);
	もし (si->sn_prof_on)
	{
	    profile_end(&si->sn_pr_start);
	    profile_sub_wait(&wait_start, &si->sn_pr_start);
	    profile_add(&si->sn_pr_total, &si->sn_pr_start);
	    profile_self(&si->sn_pr_self, &si->sn_pr_start,
							 &si->sn_pr_children);
	}
    }
#endif

    もし (got_int)
	emsg(_(e_interrupted));
#ifdef FEAT_EVAL
    ESTACK_CHECK_NOW;
#endif
    estack_pop();
    もし (p_verbose > 1)
    {
	verbose_enter();
	smsg(_("finished sourcing %s"), fname);
	もし (SOURCING_NAME != NULL)
	    smsg(_("continuing in %s"), SOURCING_NAME);
	verbose_leave();
    }
#ifdef STARTUPTIME
    もし (time_fd != NULL)
    {
	vim_snprintf((char *)IObuff, IOSIZE, "sourcing %s", fname);
	time_msg((char *)IObuff, &tv_start);
	time_pop(&tv_rel);
    }
#endif

    もし (!got_int)
	trigger_source_post = TRUE;

#ifdef FEAT_EVAL
    // After a "finish" in debug mode, need to break at first command of next
    // sourced file.
    もし (save_debug_break_level > ex_nesting_level
	    && debug_break_level == ex_nesting_level)
	++debug_break_level;
#endif

#ifdef FEAT_EVAL
almosttheend:
    // If "sn_save_cpo" is set that means we encountered "vim9script": restore
    // 'cpoptions', unless in the main .vimrc file.
    // Get "si" again, "script_items" may have been reallocated.
    si = SCRIPT_ITEM(sid);
    もし (si->sn_save_cpo != NULL && si->sn_is_vimrc == DOSO_NONE)
    {
	もし (STRCMP(p_cpo, CPO_VIM) != 0)
	{
	    char_u *f;
	    char_u *t;

	    // 'cpo' was changed in the script.  Apply the same change to the
	    // saved value, if possible.
	    ために (f = (char_u *)CPO_VIM; *f != NUL; ++f)
		もし (vim_strchr(p_cpo, *f) == NULL
			&& (t = vim_strchr(si->sn_save_cpo, *f)) != NULL)
		    // flag was removed, also remove it from the saved 'cpo'
		    mch_memmove(t, t + 1, STRLEN(t));
	    ために (f = p_cpo; *f != NUL; ++f)
		もし (vim_strchr((char_u *)CPO_VIM, *f) == NULL
			&& vim_strchr(si->sn_save_cpo, *f) == NULL)
		{
		    // flag was added, also add it to the saved 'cpo'
		    t = alloc(STRLEN(si->sn_save_cpo) + 2);
		    もし (t != NULL)
		    {
			*t = *f;
			STRCPY(t + 1, si->sn_save_cpo);
			vim_free(si->sn_save_cpo);
			si->sn_save_cpo = t;
		    }
		}
	}
	set_option_value_give_err((char_u *)"cpo",
					   0L, si->sn_save_cpo, OPT_NO_REDRAW);
    }
    VIM_CLEAR(si->sn_save_cpo);

    restore_funccal();
# ifdef FEAT_PROFILE
    もし (do_profiling == PROF_YES)
	prof_child_exit(&wait_start);		// leaving a child now
# endif

    KeyTyped = save_KeyTyped;
#endif
    current_sctx = save_current_sctx;

    もし (cookie.fp != NULL)
	fclose(cookie.fp);
    もし (cookie.source_from_buf)
	ga_clear_strings(&cookie.buflines);
    vim_free(cookie.nextline);
    vim_free(firstline);
    convert_setup(&cookie.conv, NULL, NULL);

    もし (trigger_source_post)
	apply_autocmds(EVENT_SOURCEPOST, fname_exp, fname_exp, FALSE, curbuf);

theend:
#ifdef FEAT_EVAL
    もし (sid > 0 && ret_sid != NULL
	    && fname_not_fixed != NULL && fname_exp != NULL)
    {
	整数 not_fixed_sid = find_script_by_name(fname_not_fixed);

	// If "fname_not_fixed" is a symlink then we source the linked file.
	// If the original name is in the script list we add the ID of the
	// script that was actually sourced.
	もし (SCRIPT_ID_VALID(not_fixed_sid) && not_fixed_sid != sid)
	    SCRIPT_ITEM(not_fixed_sid)->sn_sourced_sid = sid;
    }
#endif

    vim_free(fname_not_fixed);
    vim_free(fname_exp);
    sticky_cmdmod_flags = save_sticky_cmdmod_flags;
#ifdef FEAT_EVAL
    estack_compiling = save_estack_compiling;
#endif
    戻る retval;
}

    整数
do_source(
    char_u	*fname,
    整数		check_other,	    // check for .vimrc and _vimrc
    整数		is_vimrc,	    // DOSO_ value
    整数		*ret_sid)
{
    戻る do_source_ext(fname, check_other, is_vimrc, ret_sid, NULL, FALSE);
}


#もし defined(FEAT_EVAL)

/*
 * ":scriptnames"
 */
    void
ex_scriptnames(exarg_T *eap)
{
    整数 i;

    もし (eap->addr_count > 0 || *eap->arg != NUL)
    {
	// :script {scriptId}: edit the script
	もし (eap->addr_count > 0 && !SCRIPT_ID_VALID(eap->line2))
	    emsg(_(e_invalid_argument));
	否则
	{
	    もし (eap->addr_count > 0)
		eap->arg = SCRIPT_ITEM(eap->line2)->sn_name;
	    否则
	    {
		expand_env(eap->arg, NameBuff, MAXPATHL);
		eap->arg = NameBuff;
	    }
	    do_exedit(eap, NULL);
	}
	戻る;
    }

    ために (i = 1; i <= script_items.ga_len && !got_int; ++i)
    {
	scriptitem_T *si = SCRIPT_ITEM(i);

	もし (si->sn_name != NULL)
	{
	    char sourced_buf[20];

	    home_replace(NULL, si->sn_name, NameBuff, MAXPATHL, TRUE);
	    もし (si->sn_sourced_sid > 0)
		vim_snprintf(sourced_buf, 20, "->%d", si->sn_sourced_sid);
	    否则
		sourced_buf[0] = NUL;
	    vim_snprintf((char *)IObuff, IOSIZE, "%3d%s%s: %s",
		    i,
		    sourced_buf,
		    si->sn_state == SN_STATE_NOT_LOADED ? " A" : "",
		    NameBuff);
	    もし (!message_filtered(IObuff))
	    {
		msg_putchar('\n');
		msg_outtrans(IObuff);
		out_flush();	    // output one line at a time
		ui_breakcheck();
	    }
	}
    }
}

# もし defined(BACKSLASH_IN_FILENAME)
/*
 * Fix slashes in the list of script names for 'shellslash'.
 */
    void
scriptnames_slash_adjust(void)
{
    整数 i;

    ために (i = 1; i <= script_items.ga_len; ++i)
	もし (SCRIPT_ITEM(i)->sn_name != NULL)
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
    もし (id == SID_MODELINE)
	戻る (char_u *)_("modeline");
    もし (id == SID_CMDARG)
	戻る (char_u *)_("--cmd argument");
    もし (id == SID_CARG)
	戻る (char_u *)_("-c argument");
    もし (id == SID_ENV)
	戻る (char_u *)_("environment variable");
    もし (id == SID_ERROR)
	戻る (char_u *)_("error handler");
    もし (id == SID_WINLAYOUT)
	戻る (char_u *)_("changed window size");
    戻る SCRIPT_ITEM(id)->sn_name;
}

# もし defined(EXITFREE)
    void
free_scriptnames(void)
{
    整数			i;

    ために (i = script_items.ga_len; i > 0; --i)
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
    ga_clear(&script_items);
}

    void
free_autoload_scriptnames(void)
{
    ga_clear_strings(&ga_loaded);
}
# endif

    linenr_T
get_sourced_lnum(
	char_u *(*fgetline)(整数, void *, 整数, getline_opt_T),
	void *cookie)
{
    戻る fgetline == getsourceline
			? ((source_cookie_T *)cookie)->sourcing_lnum
			: SOURCING_LNUM;
}

/*
 * Return a List of script-local functions defined in the script with id
 * 'sid'.
 */
    static list_T *
get_script_local_funcs(scid_T sid)
{
    hashtab_T	*functbl;
    hashitem_T	*hi;
    long_u	todo;
    list_T	*l;

    l = list_alloc();
    もし (l == NULL)
	戻る NULL;

    // Iterate through all the functions in the global function hash table
    // looking for functions with script ID 'sid'.
    functbl = func_tbl_get();
    todo = functbl->ht_used;
    FOR_ALL_HASHTAB_ITEMS(functbl, hi, todo)
    {
	ufunc_T	*fp;

	もし (HASHITEM_EMPTY(hi))
	    continue;

	--todo;
	fp = HI2UF(hi);

	// Add active functions with script id == 'sid'
	もし (!(fp->uf_flags & FC_DEAD) && (fp->uf_script_ctx.sc_sid == sid))
	{
	    char_u	*name;

	    もし (fp->uf_name_exp != NULL)
		name = fp->uf_name_exp;
	    否则
		name = fp->uf_name;

	    list_append_string(l, name, -1);
	}
    }

    戻る l;
}

/*
 * getscriptinfo() function
 */
    void
f_getscriptinfo(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    char_u	*pat = NULL;
    regmatch_T	regmatch;
    整数		filterpat = FALSE;
    scid_T	sid = -1;

    もし (rettv_list_alloc(rettv) == FAIL)
	戻る;

    もし (check_for_opt_dict_arg(argvars, 0) == FAIL)
	戻る;

    l = rettv->vval.v_list;

    regmatch.regprog = NULL;
    regmatch.rm_ic = p_ic;

    もし (argvars[0].v_type == VAR_DICT)
    {
	dictitem_T *sid_di = dict_find(argvars[0].vval.v_dict,
							   (char_u *)"sid", 3);
	もし (sid_di != NULL)
	{
	    整数 error = FALSE;
	    sid = tv_get_number_chk(&sid_di->di_tv, &error);
	    もし (error)
		戻る;
	    もし (sid <= 0)
	    {
		semsg(_(e_invalid_value_for_argument_str_str), "sid",
						tv_get_string(&sid_di->di_tv));
		戻る;
	    }
	}
	否则
	{
	    pat = dict_get_string(argvars[0].vval.v_dict, "name", TRUE);
	    もし (pat != NULL)
		regmatch.regprog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
	    もし (regmatch.regprog != NULL)
		filterpat = TRUE;
	}
    }

    ために (varnumber_T i = sid > 0 ? sid : 1;
		       (i == sid || sid <= 0) && i <= script_items.ga_len; ++i)
    {
	scriptitem_T	*si = SCRIPT_ITEM(i);
	dict_T		*d;

	もし (si->sn_name == NULL)
	    continue;

	もし (filterpat && !vim_regexec(&regmatch, si->sn_name, (colnr_T)0))
	    continue;

	もし ((d = dict_alloc()) == NULL
		|| list_append_dict(l, d) == FAIL)
	{
	    dict_unref(d);
	    goto theend;
	}
	もし (dict_add_string(d, "name", si->sn_name) == FAIL
		|| dict_add_number(d, "sid", i) == FAIL
		|| dict_add_number(d, "sourced", si->sn_sourced_sid) == FAIL
		|| dict_add_number(d, "version", si->sn_version) == FAIL
		|| dict_add_bool(d, "autoload",
				si->sn_state == SN_STATE_NOT_LOADED) == FAIL)
	    goto theend;

	// When a script ID is specified, return information about only the
	// specified script, and add the script-local variables and functions.
	もし (sid > 0)
	{
	    dict_T	*var_dict;
	    list_T	*fn_list;

	    var_dict = dict_copy(&si->sn_vars->sv_dict, TRUE, TRUE,
								get_copyID());
	    もし (var_dict == NULL)
		goto theend;
	    もし (dict_add_dict(d, "variables", var_dict) == FAIL)
	    {
		dict_unref(var_dict);
		goto theend;
	    }
	    --var_dict->dv_refcount;
	    fn_list = get_script_local_funcs(sid);
	    もし (fn_list == NULL || dict_add_list(d, "functions", fn_list) == FAIL)
	    {
		list_unref(fn_list);
		goto theend;
	    }
	}
    }

theend:
    vim_regfree(regmatch.regprog);
    vim_free(pat);
}

#endif

    static char_u *
get_one_sourceline(source_cookie_T *sp)
{
    garray_T		ga;
    整数			len;
    整数			c;
    char_u		*buf;
#ifdef USE_CRNL
    整数			has_cr;		// CR-LF found
#endif
    整数			have_read = FALSE;

    // use a growarray to store the sourced line
    ga_init2(&ga, 1, 250);

    // Loop until there is a finished line (or end-of-file).
    ++sp->sourcing_lnum;
    ために (;;)
    {
	// make room to read at least 120 (more) characters
	もし (ga_grow(&ga, 120) == FAIL)
	    break;
	もし (sp->source_from_buf)
	{
	    もし (sp->buf_lnum >= sp->buflines.ga_len)
		break;		    // all the lines are processed
	    ga_concat(&ga, ((char_u **)sp->buflines.ga_data)[sp->buf_lnum]);
	    sp->buf_lnum++;
	    もし (ga_grow(&ga, 1) == FAIL)
		break;
	    buf = (char_u *)ga.ga_data;
	    buf[ga.ga_len++] = NUL;
	    len = ga.ga_len;
	}
	否则
	{
	    buf = (char_u *)ga.ga_data;
	    もし (fgets((char *)buf + ga.ga_len, ga.ga_maxlen - ga.ga_len,
			sp->fp) == NULL)
		break;
	    len = ga.ga_len + (整数)STRLEN(buf + ga.ga_len);
	}
#ifdef USE_CRNL
	// Ignore a trailing CTRL-Z, when in Dos mode.	Only recognize the
	// CTRL-Z by its own, or after a NL.
	もし (	   (len == 1 || (len >= 2 && buf[len - 2] == '\n'))
		&& sp->fileformat == EOL_DOS
		&& buf[len - 1] == Ctrl_Z)
	{
	    buf[len - 1] = NUL;
	    break;
	}
#endif

	have_read = TRUE;
	ga.ga_len = len;

	// If the line was longer than the buffer, read more.  A line of a
	// buffer was taken as a whole.
	もし (!sp->source_from_buf && ga.ga_maxlen - ga.ga_len == 1
						       && buf[len - 1] != '\n')
	    continue;

	もし (len >= 1 && buf[len - 1] == '\n')	// remove trailing NL
	{
#ifdef USE_CRNL
	    has_cr = (len >= 2 && buf[len - 2] == '\r');
	    もし (sp->fileformat == EOL_UNKNOWN)
	    {
		もし (has_cr)
		    sp->fileformat = EOL_DOS;
		否则
		    sp->fileformat = EOL_UNIX;
	    }

	    もし (sp->fileformat == EOL_DOS)
	    {
		もし (has_cr)	    // replace trailing CR
		{
		    buf[len - 2] = '\n';
		    --len;
		    --ga.ga_len;
		}
		否则	    // lines like ":map xx yy^M" will have failed
		{
		    もし (!sp->error)
		    {
			msg_source(HL_ATTR(HLF_W));
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
	    ために (c = len - 2; c >= 0 && buf[c] == Ctrl_V; c--)
		;
	    もし ((len & 1) != (c & 1))	// escaped NL, read more
	    {
		++sp->sourcing_lnum;
		continue;
	    }

	    buf[len - 1] = NUL;		// remove the NL
	}

	// Check for ^C here now and then, so recursive :so can be broken.
	line_breakcheck();
	break;
    }

    もし (have_read)
	戻る (char_u *)ga.ga_data;

    vim_free(ga.ga_data);
    戻る NULL;
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
	整数 c UNUSED,
	void *cookie,
	整数 indent UNUSED,
	getline_opt_T options)
{
    source_cookie_T	*sp = (source_cookie_T *)cookie;
    char_u		*line;
    char_u		*p;
    整数			do_vim9_all = in_vim9script()
					      && options == GETLINE_CONCAT_ALL;
    整数			do_bar_cont = do_vim9_all
					 || options == GETLINE_CONCAT_CONTBAR;

#ifdef FEAT_EVAL
    // If breakpoints have been added/deleted need to check for it.
    もし ((sp->dbg_tick < debug_tick) && !sp->source_from_buf)
    {
	sp->breakpoint = dbg_find_breakpoint(TRUE, sp->fname, SOURCING_LNUM);
	sp->dbg_tick = debug_tick;
    }
# ifdef FEAT_PROFILE
    もし (do_profiling == PROF_YES)
	script_line_end();
# endif
#endif

    // Set the current sourcing line number.
    SOURCING_LNUM = sp->sourcing_lnum + 1;

    // Get current line.  If there is a read-ahead line, use it, otherwise get
    // one now.  "fp" is NULL if actually using a string.
    もし (sp->finished || (!sp->source_from_buf && sp->fp == NULL))
	line = NULL;
    否则 もし (sp->nextline == NULL)
	line = get_one_sourceline(sp);
    否则
    {
	line = sp->nextline;
	sp->nextline = NULL;
	++sp->sourcing_lnum;
    }
#ifdef FEAT_PROFILE
    もし (line != NULL && do_profiling == PROF_YES)
	script_line_start();
#endif

    // Only concatenate lines starting with a \ when 'cpoptions' doesn't
    // contain the 'C' flag.
    もし (line != NULL && options != GETLINE_NONE
				      && vim_strchr(p_cpo, CPO_CONCAT) == NULL)
    {
	整数 comment_char = in_vim9script() ? '#' : '"';

	// compensate for the one line read-ahead
	--sp->sourcing_lnum;

	// Get the next line and concatenate it when it starts with a
	// backslash. We always need to read the next line, keep it in
	// sp->nextline.
	/* Also check for a comment in between continuation lines: "\ */
	// Also check for a Vim9 comment, empty line, line starting with '|',
	// but not "||".
	sp->nextline = get_one_sourceline(sp);
	もし (sp->nextline != NULL
		&& (*(p = skipwhite(sp->nextline)) == '\\'
			      || (p[0] == comment_char
						&& p[1] == '\\' && p[2] == ' ')
			      || (do_vim9_all && (*p == NUL
						     || vim9_comment_start(p)))
			      || (do_bar_cont && p[0] == '|' && p[1] != '|')))
	{
	    garray_T    ga;

	    ga_init2(&ga, sizeof(char_u), 400);
	    ga_concat(&ga, line);
	    もし (*p == '\\')
		ga_concat(&ga, p + 1);
	    否则 もし (*p == '|')
	    {
		GA_CONCAT_LITERAL(&ga, " ");
		ga_concat(&ga, p);
	    }
	    ために (;;)
	    {
		vim_free(sp->nextline);
		sp->nextline = get_one_sourceline(sp);
		もし (sp->nextline == NULL)
		    break;
		p = skipwhite(sp->nextline);
		もし (*p == '\\' || (do_bar_cont && p[0] == '|' && p[1] != '|'))
		{
		    // Adjust the growsize to the current length to speed up
		    // concatenating many lines.
		    もし (ga.ga_len > 400)
		    {
			もし (ga.ga_len > 8000)
			    ga.ga_growsize = 8000;
			否则
			    ga.ga_growsize = ga.ga_len;
		    }
		    もし (*p == '\\')
			ga_concat(&ga, p + 1);
		    否则
		    {
			GA_CONCAT_LITERAL(&ga, " ");
			ga_concat(&ga, p);
		    }
		}
		否则 もし (!(p[0] == (comment_char)
						&& p[1] == '\\' && p[2] == ' ')
		     && !(do_vim9_all && (*p == NUL || vim9_comment_start(p))))
		    break;
		/* drop a # comment or "\ comment line */
	    }
	    ga_append(&ga, NUL);
	    vim_free(line);
	    line = ga.ga_data;
	}
    }

    もし (line != NULL && sp->conv.vc_type != CONV_NONE)
    {
	char_u	*s;

	// Convert the encoding of the script line.
	s = string_convert(&sp->conv, line, NULL);
	もし (s != NULL)
	{
	    vim_free(line);
	    line = s;
	}
    }

#ifdef FEAT_EVAL
    // Did we encounter a breakpoint?
    もし (!sp->source_from_buf && sp->breakpoint != 0
	    && sp->breakpoint <= SOURCING_LNUM)
    {
	dbg_breakpoint(sp->fname, SOURCING_LNUM);
	// Find next breakpoint.
	sp->breakpoint = dbg_find_breakpoint(TRUE, sp->fname, SOURCING_LNUM);
	sp->dbg_tick = debug_tick;
    }
#endif

    戻る line;
}

/*
 * Returns TRUE if sourcing a script either from a file or a buffer.
 * Otherwise returns FALSE.
 */
    整数
sourcing_a_script(exarg_T *eap)
{
    戻る (getline_equal(eap->ea_getline, eap->cookie, getsourceline));
}

/*
 * ":scriptencoding": Set encoding conversion for a sourced script.
 */
    void
ex_scriptencoding(exarg_T *eap)
{
    source_cookie_T	*sp;
    char_u		*name;

    もし (!sourcing_a_script(eap))
    {
	emsg(_(e_scriptencoding_used_outside_of_sourced_file));
	戻る;
    }

    もし (*eap->arg != NUL)
    {
	name = enc_canonize(eap->arg);
	もし (name == NULL)	// out of memory
	    戻る;
    }
    否则
	name = eap->arg;

    // Setup for conversion from the specified encoding to 'encoding'.
    sp = (source_cookie_T *)getline_cookie(eap->ea_getline, eap->cookie);
    convert_setup(&sp->conv, name, p_enc);

    もし (name != eap->arg)
	vim_free(name);
}

/*
 * ":scriptversion": Set Vim script version for a sourced script.
 */
    void
ex_scriptversion(exarg_T *eap UNUSED)
{
    整数		nr;

    もし (!sourcing_a_script(eap))
    {
	emsg(_(e_scriptversion_used_outside_of_sourced_file));
	戻る;
    }
    もし (in_vim9script())
    {
	emsg(_(e_cannot_use_scriptversion_after_vim9script));
	戻る;
    }

    nr = getdigits(&eap->arg);
    もし (nr == 0 || *eap->arg != NUL)
	emsg(_(e_invalid_argument));
    否则 もし (nr > SCRIPT_VERSION_MAX)
	semsg(_(e_scriptversion_not_supported_nr), nr);
    否则
    {
	current_sctx.sc_version = nr;
#ifdef FEAT_EVAL
	SCRIPT_ITEM(current_sctx.sc_sid)->sn_version = nr;
#endif
    }
}

#もし defined(FEAT_EVAL)
/*
 * ":finish": Mark a sourced file as finished.
 */
    void
ex_finish(exarg_T *eap)
{
    もし (sourcing_a_script(eap))
	do_finish(eap, FALSE);
    否则
	emsg(_(e_finish_used_outside_of_sourced_file));
}

/*
 * Mark a sourced file as finished.  Possibly makes the ":finish" pending.
 * Also called for a pending finish at the ":endtry" or after returning from
 * an extra do_cmdline().  "reanimate" is used in the latter case.
 */
    void
do_finish(exarg_T *eap, 整数 reanimate)
{
    整数		idx;

    もし (reanimate)
	((source_cookie_T *)getline_cookie(eap->ea_getline,
					      eap->cookie))->finished = FALSE;

    // Cleanup (and inactivate) conditionals, but stop when a try conditional
    // not in its finally clause (which then is to be executed next) is found.
    // In this case, make the ":finish" pending for execution at the ":endtry".
    // Otherwise, finish normally.
    idx = cleanup_conditionals(eap->cstack, 0, TRUE);
    もし (idx >= 0)
    {
	eap->cstack->cs_pending[idx] = CSTP_FINISH;
	report_make_pending(CSTP_FINISH, NULL);
    }
    否则
	((source_cookie_T *)getline_cookie(eap->ea_getline,
					       eap->cookie))->finished = TRUE;
}


/*
 * Return TRUE when a sourced file had the ":finish" command: Don't give error
 * message for missing ":endif".
 * Return FALSE when not sourcing a file.
 */
    整数
source_finished(
    char_u	*(*fgetline)(整数, void *, 整数, getline_opt_T),
    void	*cookie)
{
    戻る (getline_equal(fgetline, cookie, getsourceline)
	    && ((source_cookie_T *)getline_cookie(
						fgetline, cookie))->finished);
}

/*
 * Find the path of a script below the "autoload" directory.
 * Returns NULL if there is no "/autoload/" in the script name.
 */
    static char_u *
script_name_after_autoload(scriptitem_T *si)
{
    char_u	*p = si->sn_name;
    char_u	*res = NULL;

    ために (;;)
    {
	char_u *n = (char_u *)strstr((char *)p, "autoload");

	もし (n == NULL)
	    break;
	もし (n > p && vim_ispathsep(n[-1]) && vim_ispathsep(n[8]))
	    res = n + 9;
	p = n + 8;
    }
    戻る res;
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

    もし (p == NULL)
	戻る NULL;
    prefix = vim_strsave(p);
    もし (prefix == NULL)
	戻る NULL;

    // replace all '/' with '#' and locate ".vim" at the end
    ために (p = prefix; *p != NUL; p += mb_ptr2len(p))
    {
	もし (vim_ispathsep(*p))
	    *p = '#';
	否则 もし (STRCMP(p, ".vim") == 0)
	{
	    p[0] = '#';
	    p[1] = NUL;
	    戻る prefix;
	}
    }

    // did not find ".vim" at the end
    vim_free(prefix);
    戻る NULL;
}

/*
 * If in a Vim9 autoload script return "name" with the autoload prefix for the
 * script.  If successful the returned name is allocated.
 * Otherwise it returns "name" unmodified.
 */
    char_u *
may_prefix_autoload(char_u *name)
{
    もし (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	戻る name;

    scriptitem_T *si = SCRIPT_ITEM(current_sctx.sc_sid);

    もし (si->sn_autoload_prefix == NULL)
	戻る name;

    char_u  *basename = name;
    size_t  len;
    char_u  *res;

    もし (*name == K_SPECIAL)
    {
	char_u *p = vim_strchr(name, '_');

	// skip over "<SNR>99_"
	もし (p != NULL)
	    basename = p + 1;
    }

    len = STRLEN(si->sn_autoload_prefix) + STRLEN(basename) + 2;
    res = alloc(len);
    もし (res == NULL)
	戻る NULL;

    vim_snprintf((char *)res, len, "%s%s", si->sn_autoload_prefix, basename);
    戻る res;
}

/*
 * Return the autoload script name for a function or variable name.
 * Returns NULL when out of memory.
 * Caller must make sure that "name" contains AUTOLOAD_CHAR.
 */
    char_u *
autoload_name(char_u *name)
{
    char_u	*p, *q = NULL;
    char_u	*scriptname;

    // Get the script file name: replace '#' with '/', append ".vim".
    scriptname = alloc(STRLEN(name) + 14);
    もし (scriptname == NULL)
	戻る NULL;
    STRCPY(scriptname, "autoload/");
    STRCAT(scriptname, name[0] == 'g' && name[1] == ':' ? name + 2: name);
    ために (p = scriptname + 9; (p = vim_strchr(p, AUTOLOAD_CHAR)) != NULL;
								    q = p, ++p)
	*p = '/';
    STRCPY(q, ".vim");
    戻る scriptname;
}

/*
 * If "name" has a package name try autoloading the script for it.
 * Return TRUE if a package was loaded.
 */
    整数
script_autoload(
    char_u	*name,
    整数		reload)	    // load script again when already loaded
{
    char_u	*p;
    char_u	*scriptname, *tofree;
    整数		ret = FALSE;
    整数		i;
    整数		ret_sid;

    // If the name starts with "<SNR>123_" then "123" is the script ID.
    もし (name[0] == K_SPECIAL && name[1] == KS_EXTRA && name[2] == KE_SNR)
    {
	p = name + 3;
	ret_sid = (整数)getdigits(&p);
	もし (*p == '_' && SCRIPT_ID_VALID(ret_sid))
	{
	    may_load_script(ret_sid, &ret);
	    戻る ret;
	}
    }

    // If there is no '#' after name[0] there is no package name.
    p = vim_strchr(name, AUTOLOAD_CHAR);
    もし (p == NULL || p == name)
	戻る FALSE;

    tofree = scriptname = autoload_name(name);
    もし (scriptname == NULL)
	戻る FALSE;

    // Find the name in the list of previously loaded package names.  Skip
    // "autoload/", it's always the same.
    ために (i = 0; i < ga_loaded.ga_len; ++i)
	もし (STRCMP(((char_u **)ga_loaded.ga_data)[i] + 9, scriptname + 9) == 0)
	    break;
    もし (!reload && i < ga_loaded.ga_len)
	ret = FALSE;	    // was loaded already
    否则
    {
	// Remember the name if it wasn't loaded already.
	もし (i == ga_loaded.ga_len && ga_grow(&ga_loaded, 1) == OK)
	{
	    ((char_u **)ga_loaded.ga_data)[ga_loaded.ga_len++] = scriptname;
	    tofree = NULL;
	}

	// Try loading the package from $VIMRUNTIME/autoload/<name>.vim
	// Use "ret_sid" to avoid loading the same script again.
	もし (source_in_path(p_rtp, scriptname, DIP_START, &ret_sid) == OK)
	    ret = TRUE;
    }

    vim_free(tofree);
    戻る ret;
}
#endif
