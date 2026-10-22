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

#ако defined(FEAT_EVAL)
// The names of packages that once were loaded are remembered.
static garray_T		ga_loaded = {0, 0, sizeof(char_u *), 4, NULL};
#endif

// last used sequence number for sourcing scripts (current_sctx.sc_seq)
#ifdef FEAT_EVAL
static цяло		last_current_SID_seq = 0;
#endif

static цяло do_source_ext(char_u *fname, цяло check_other, цяло is_vimrc, цяло *ret_sid, exarg_T *eap, цяло clearvars);

/*
 * Initialize the execution stack.
 */
    void
estack_init(void)
{
    estack_T *entry;

    ако (ga_grow(&exestack, 10) == FAIL)
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
    ако (ga_grow(&exestack, 1) == FAIL)
	върни NULL;

    entry = ((estack_T *)exestack.ga_data) + exestack.ga_len;
    entry->es_type = type;
    entry->es_name = name;
    entry->es_lnum = lnum;
#ifdef FEAT_EVAL
    entry->es_info.ufunc = NULL;
#endif
    ++exestack.ga_len;
    върни entry;
}

#ако defined(FEAT_EVAL)
/*
 * Add a user function to the execution stack.
 */
    estack_T *
estack_push_ufunc(ufunc_T *ufunc, long lnum)
{
    estack_T *entry = estack_push(ETYPE_UFUNC,
	    ufunc->uf_name_exp != NULL
				  ? ufunc->uf_name_exp : ufunc->uf_name, lnum);
    ако (entry != NULL)
	entry->es_info.ufunc = ufunc;
    върни entry;
}

/*
 * Return TRUE if "ufunc" with "lnum" is already at the top of the exe stack.
 */
    цяло
estack_top_is_ufunc(ufunc_T *ufunc, long lnum)
{
    estack_T *entry;

    ако (exestack.ga_len == 0)
	върни FALSE;
    entry = ((estack_T *)exestack.ga_data) + exestack.ga_len - 1;
    върни entry->es_type == ETYPE_UFUNC
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
    ако (exestack.ga_len == 0)
	върни NULL;
    --exestack.ga_len;
    върни ((estack_T *)exestack.ga_data) + exestack.ga_len;
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
    цяло		idx;
    etype_T	last_type = ETYPE_SCRIPT;
#endif

    entry = ((estack_T *)exestack.ga_data) + exestack.ga_len - 1;
#ifdef FEAT_EVAL
    ако (which == ESTACK_SFILE && entry->es_type != ETYPE_UFUNC)
#endif
    {
	ако (entry->es_name == NULL)
	    върни NULL;
	върни vim_strsave(entry->es_name);
    }
#ifdef FEAT_EVAL
    // expand('<sfile>') works in a function for backwards compatibility, but
    // may give an unexpected result.  Disallow it in Vim 9 script.
    ако (which == ESTACK_SFILE && in_vim9script())
    {
	цяло  save_emsg_off = emsg_off;

	ако (emsg_off == 1)
	    // f_expand() silences errors but we do want this one
	    emsg_off = 0;
	emsg(_(e_cannot_expand_sfile_in_vim9_function));
	emsg_off = save_emsg_off;
	върни NULL;
    }

    // If evaluated in a function or autocommand, return the path of the script
    // where it is defined, at script level the current script path is returned
    // instead.
    ако (which == ESTACK_SCRIPT)
    {
	// Walk the stack backwards, starting from the current frame.
	за (idx = exestack.ga_len - 1; idx >= 0; --idx, --entry)
	{
	    ако (entry->es_type == ETYPE_UFUNC || entry->es_type == ETYPE_AUCMD)
	    {
		sctx_T *def_ctx = entry->es_type == ETYPE_UFUNC
			      ? &entry->es_info.ufunc->uf_script_ctx
			      : entry->es_info.aucmd != NULL
				  ? acp_script_ctx(entry->es_info.aucmd) : NULL;

		върни def_ctx != NULL && def_ctx->sc_sid > 0
			   ? vim_strsave(SCRIPT_ITEM(def_ctx->sc_sid)->sn_name)
			   : NULL;
	    }
	    иначе ако (entry->es_type == ETYPE_SCRIPT)
		върни vim_strsave(entry->es_name);
	}
	върни NULL;
    }

    // Give information about each stack entry up to the root.
    // For a function we compose the call stack, as it was done in the past:
    //   "function One[123]..Two[456]..Three"
    ga_init2(&ga, sizeof(char), 100);
    за (idx = 0; idx < exestack.ga_len; ++idx)
    {
	entry = ((estack_T *)exestack.ga_data) + idx;
	ако (entry->es_name != NULL)
	{
	    long	lnum = 0;
	    size_t	added = 0;
	    string_T	type_name = {(char_u *)"", 0};
	    string_T	class_name = {(char_u *)"", 0};
	    string_T	es_name = {entry->es_name, STRLEN(entry->es_name)};

	    ако (entry->es_type != last_type)
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
	    ако (entry->es_type == ETYPE_UFUNC && entry->es_info.ufunc->uf_class != NULL)
	    {
		class_name.string = entry->es_info.ufunc->uf_class->class_name.string;
		class_name.length = entry->es_info.ufunc->uf_class->class_name.length;
	    }
	    ако (idx == exestack.ga_len - 1)
		lnum = which == ESTACK_STACK ? SOURCING_LNUM : 0;
	    иначе
		lnum = entry->es_lnum;

	    len = es_name.length + type_name.length + class_name.length + 26;
	    ако (ga_grow(&ga, (цяло)len) == FAIL)
		break;
	    ga_concat_len(&ga, type_name.string, type_name.length);
	    // For class methods prepend "<class name>." to the function name.
	    ако (*class_name.string != NUL)
	    {
		added = vim_snprintf_safelen(
		    (char *)ga.ga_data + ga.ga_len,
		    ga.ga_maxlen - ga.ga_len,
		    "<SNR>%d_%s.",
		    entry->es_info.ufunc->uf_script_ctx.sc_sid,
		    class_name.string);

		ga.ga_len += (цяло)added;
	    }
	    ga_concat_len(&ga, es_name.string, es_name.length);
	    // For the bottom entry of <sfile>: do not add the line number, it is used in
	    // <slnum>.  Also leave it out when the number is not set.
	    ако (lnum != 0)
	    {
		added = vim_snprintf_safelen(
		    (char *)ga.ga_data + ga.ga_len,
		    ga.ga_maxlen - ga.ga_len,
		    "[%ld]",
		    lnum);

		ga.ga_len += (цяло)added;
	    }
	    ако (idx != exestack.ga_len - 1)
		GA_CONCAT_LITERAL(&ga, "..");
	}
    }

    // Only NUL-terminate when not returning NULL.
    ако (ga.ga_data != NULL)
	ga_append(&ga, NUL);
    върни (char_u *)ga.ga_data;
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
    ако (d == NULL)
	върни;

    tv.v_type = VAR_DICT;
    tv.v_lock = VAR_LOCKED;
    tv.vval.v_dict = d;

    ако (fp != NULL)
	dict_add_func(d, "funcref", fp);
    ако (event != NULL)
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
    цяло		i;

    l = list_alloc();
    ако (l == NULL)
	върни NULL;

    за (i = 0; i < exestack.ga_len; ++i)
    {
	estack_T *entry = &((estack_T *)exestack.ga_data)[i];
	linenr_T lnum = entry->es_lnum;

	ако (entry->es_type == ETYPE_SCRIPT)
	    stacktrace_push_item(l, NULL, NULL, lnum, entry->es_name);
	иначе ако (entry->es_type == ETYPE_UFUNC)
	{
	    ufunc_T *fp = entry->es_info.ufunc;
	    sctx_T sctx = fp->uf_script_ctx;
	    char_u *filepath = sctx.sc_sid > 0 ?
				   get_scriptname(sctx.sc_sid) : (char_u *)"";

	    lnum += sctx.sc_lnum;
	    stacktrace_push_item(l, fp, NULL, lnum, filepath);
	}
	иначе ако (entry->es_type == ETYPE_AUCMD)
	{
	    // The autocmd may not have a matching pattern yet, in which case
	    // es_info.aucmd is still NULL.
	    sctx_T *sctx = entry->es_info.aucmd != NULL
			       ? acp_script_ctx(entry->es_info.aucmd) : NULL;
	    char_u *filepath = sctx != NULL && sctx->sc_sid > 0 ?
				   get_scriptname(sctx->sc_sid) : (char_u *)"";

	    ако (sctx != NULL)
		lnum += sctx->sc_lnum;
	    stacktrace_push_item(l, NULL, entry->es_name, lnum, filepath);
	}
    }
    върни l;
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
    static цяло
get_runtime_cmd_flags(char_u **argp, size_t where_len)
{
    char_u *arg = *argp;

    ако (where_len == 0)
	върни 0;

    ако (STRNCMP(arg, "START", where_len) == 0)
    {
	*argp = skipwhite(arg + where_len);
	върни DIP_START + DIP_NORTP;
    }
    ако (STRNCMP(arg, "OPT", where_len) == 0)
    {
	*argp = skipwhite(arg + where_len);
	върни DIP_OPT + DIP_NORTP;
    }
    ако (STRNCMP(arg, "PACK", where_len) == 0)
    {
	*argp = skipwhite(arg + where_len);
	върни DIP_START + DIP_OPT + DIP_NORTP;
    }
    ако (STRNCMP(arg, "ALL", where_len) == 0)
    {
	*argp = skipwhite(arg + where_len);
	върни DIP_START + DIP_OPT;
    }

    върни 0;
}

/*
 * ":runtime [where] {name}"
 */
    void
ex_runtime(exarg_T *eap)
{
    char_u  *arg = eap->arg;
    цяло	    flags = eap->forceit ? DIP_ALL : 0;
    char_u  *p = skiptowhite(arg);
    flags += get_runtime_cmd_flags(&arg, p - arg);
    source_runtime(arg, flags);
}

static цяло runtime_expand_flags;

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
    докато (*(p = skiptowhite_esc(arg)) != NUL)
    {
	ако (runtime_expand_flags == 0)
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
    цяло
find_script_by_name(char_u *name)
{
    цяло		    sid;
    scriptitem_T    *si;

    за (sid = script_items.ga_len; sid > 0; --sid)
    {
	// We used to check inode here, but that doesn't work:
	// - If a script is edited and written, it may get a different
	//   inode number, even though to the user it is the same script.
	// - If a script is deleted and another script is written, with a
	//   different name, the inode may be re-used.
	si = SCRIPT_ITEM(sid);
	ако (si->sn_name != NULL && fnamecmp(si->sn_name, name) == 0)
	    върни sid;
    }
    върни -1;
}

/*
 * Add a new scriptitem with all items initialized.
 * When running out of memory "error" is set to FAIL.
 * Returns the script ID.
 */
    static цяло
get_new_scriptitem(цяло *error)
{
    static scid_T   last_current_SID = 0;
    цяло		    sid = ++last_current_SID;
    scriptitem_T    *si = NULL;

    ако (ga_grow(&script_items, (цяло)(sid - script_items.ga_len)) == FAIL)
    {
	*error = FAIL;
	върни sid;
    }
    докато (script_items.ga_len < sid)
    {
	si = ALLOC_CLEAR_ONE(scriptitem_T);
	ако (si == NULL)
	{
	    *error = FAIL;
	    върни sid;
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
    ако (si != NULL)
	// Used to check script variable index is still valid.
	si->sn_script_seq = current_sctx.sc_seq;

    върни sid;
}

    цяло
get_new_scriptitem_for_fname(цяло *error, char_u *fname)
{
    цяло sid = get_new_scriptitem(error);

    ако (*error == OK)
    {
	scriptitem_T *si = SCRIPT_ITEM(sid);

	si->sn_name = vim_strsave(fname);
	si->sn_state = SN_STATE_NOT_LOADED;
    }
    върни sid;
}

/*
 * If the script for "sid" is a symlink and "sn_source_sid" is not set
 * then initialize it. A new script_item is created if needed.
 */
    void
check_script_symlink(цяло sid)
{
    scriptitem_T *si = SCRIPT_ITEM(sid);
    ако (si->sn_syml_checked || si->sn_sourced_sid > 0)
	върни;
    si->sn_syml_checked = TRUE;

    // If fname is a symbolic link, create an script_item for the real file.

    char_u *real_fname = fix_fname(si->sn_name);
    ако (real_fname != NULL && STRCMP(real_fname, si->sn_name) != 0)
    {
	цяло real_sid = find_script_by_name(real_fname);
	цяло error2 = OK;
	цяло new_sid = FALSE;
	ако (real_sid < 0)
	{
	    real_sid = get_new_scriptitem_for_fname(&error2, real_fname);
	    new_sid = TRUE;
	}
	ако (error2 == OK)
	{
	    si = SCRIPT_ITEM(sid);
	    si->sn_sourced_sid = real_sid;
	    ако (new_sid)
	    {
		SCRIPT_ITEM(real_sid)->sn_import_autoload
						    = si->sn_import_autoload;
		ако (si->sn_autoload_prefix != NULL)
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
    цяло sid;
    цяло error = OK;
    цяло *ret_sid = cookie;

    sid = find_script_by_name(fname);
    ако (sid < 0)
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
    цяло
do_in_path(
    char_u	*path,
    char	*prefix,
    char_u	*name,
    цяло		flags,
    void	(*callback)(char_u *fname, void *ck),
    void	*cookie)
{
    char_u	*rtp;
    string_T	buf;
    char_u	*rtp_copy;
    цяло		num_files;
    char_u	**files;
    цяло		i;
    цяло		did_one = FALSE;
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
    ако (buf.string != NULL && rtp_copy != NULL)
    {
	size_t	prefixlen = 0;
	size_t	namelen = 0;

	ако (name != NULL)
	{
	    prefixlen = STRLEN(prefix);
	    namelen = STRLEN(name);

	    ако (p_verbose > 10)
	    {
		verbose_enter();
		ако (*prefix != NUL)
		    smsg(_("Searching for \"%s\" under \"%s\" in \"%s\""),
					       (char *)name, prefix, (char *)path);
		иначе
		    smsg(_("Searching for \"%s\" in \"%s\""),
						       (char *)name, (char *)path);
		verbose_leave();
	    }
	}

	// Loop over all entries in 'runtimepath'.
	rtp = rtp_copy;
	докато (*rtp != NUL && ((flags & DIP_ALL) || !did_one))
	{
	    // Copy the path from 'runtimepath' to buf[].
	    buf.length = (size_t)copy_option_part(&rtp, buf.string, MAXPATHL, ",");

	    // Skip after or non-after directories.
	    ако (flags & (DIP_NOAFTER | DIP_AFTER))
	    {
		цяло is_after = buf.length >= 5
		    && STRCMP(buf.string + buf.length - 5, "after") == 0;

		ако ((is_after && (flags & DIP_NOAFTER))
			|| (!is_after && (flags & DIP_AFTER)))
		    continue;
	    }

	    ако (name == NULL)
	    {
		(*callback)(buf.string, (void *)&cookie);
		ако (!did_one)
		    did_one = (cookie == NULL);
	    }
	    иначе ако (buf.length + 2 + prefixlen + namelen < MAXPATHL)
	    {
		char_u	*np;
		char_u	*tail;

		ако (*buf.string != NUL
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
		докато (*np != NUL && ((flags & DIP_ALL) || !did_one))
		{
		    // Append the pattern from "name" to buf[].
		    copy_option_part(&np, tail, (цяло)(MAXPATHL - (tail - buf.string)),
								       "\t ");

		    ако (p_verbose > 10)
		    {
			verbose_enter();
			smsg(_("Searching for \"%s\""), buf.string);
			verbose_leave();
		    }

		    // Expand wildcards, invoke the callback for each match.
		    ако (gen_expand_wildcards(1, &buf.string, &num_files, &files,
				  (flags & DIP_DIR) ? EW_DIR : EW_FILE) == OK)
		    {
			за (i = 0; i < num_files; ++i)
			{
			    (*callback)(files[i], cookie);
			    did_one = TRUE;
			    ако (!(flags & DIP_ALL))
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
    ако (!did_one && name != NULL)
    {
	char *basepath = path == p_rtp ? "runtimepath" : "packpath";

	ако (flags & DIP_ERR)
	    semsg(_(e_directory_not_found_in_str_str), basepath, name);
	иначе ако (p_verbose > 0)
	{
	    verbose_enter();
	    smsg(_("not found in '%s': \"%s\""), basepath, name);
	    verbose_leave();
	}
    }

#ifdef AMIGA
    proc->pr_WindowPtr = save_winptr;
#endif

    върни did_one ? OK : FAIL;
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
    static цяло
do_in_path_and_pp(
    char_u	*path,
    char_u	*name,
    цяло		flags,
    void	(*callback)(char_u *fname, void *ck),
    void	*cookie)
{
    цяло		done = FAIL;

    ако ((flags & DIP_NORTP) == 0)
	done = do_in_path(path, "", name, flags, callback, cookie);

    ако ((done == FAIL || (flags & DIP_ALL)) && (flags & DIP_START))
	done = do_in_path(p_pp, "pack/*/start/*/", name, flags, callback,
								       cookie);

    ако ((done == FAIL || (flags & DIP_ALL)) && (flags & DIP_OPT))
	done = do_in_path(p_pp, "pack/*/opt/*/", name, flags, callback,
								       cookie);

    върни done;
}

/*
 * Just like do_in_path_and_pp(), using 'runtimepath' for "path".
 */
    цяло
do_in_runtimepath(
    char_u	*name,
    цяло		flags,
    void	(*callback)(char_u *fname, void *ck),
    void	*cookie)
{
    върни do_in_path_and_pp(p_rtp, name, flags, callback, cookie);
}

/*
 * Source the file "name" from all directories in 'runtimepath'.
 * "name" can contain wildcards.
 * When "flags" has DIP_ALL: source all files, otherwise only the first one.
 *
 * return FAIL when no file could be sourced, OK otherwise.
 */
    цяло
source_runtime(char_u *name, цяло flags)
{
    върни source_in_path(p_rtp, name, flags, NULL);
}

/*
 * Just like source_runtime(), but use "path" instead of 'runtimepath'
 * and return the script ID in "ret_sid".
 */
    цяло
source_in_path(char_u *path, char_u *name, цяло flags, цяло *ret_sid)
{
    върни do_in_path_and_pp(path, name, flags, source_callback, ret_sid);
}

#ако defined(FEAT_EVAL)

/*
 * Find "name" in 'runtimepath'. If found a new scriptitem is created for it
 * and it's script ID is returned.
 * If not found returns -1.
 */
    цяло
find_script_in_rtp(char_u *name)
{
    цяло sid = -1;

    (void)do_in_path_and_pp(p_rtp, name, DIP_START | DIP_NOAFTER,
						   find_script_callback, &sid);
    върни sid;
}

/*
 * Expand wildcards in "pat" and invoke do_source() for each match.
 */
    static void
source_all_matches(char_u *pat)
{
    цяло	    num_files;
    char_u  **files;
    цяло	    i;

    ако (gen_expand_wildcards(1, &pat, &num_files, &files, EW_FILE) != OK)
	върни;

    за (i = 0; i < num_files; ++i)
	(void)do_source(files[i], FALSE, DOSO_NONE, NULL);
    FreeWild(num_files, files);
}

/*
 * Add the package directory to 'runtimepath'.
 */
    static цяло
add_pack_dir_to_rtp(char_u *fname)
{
    char_u  *p4, *p3, *p2, *p1, *p;
    char_u  *entry;
    char_u  *insp = NULL;
    цяло	    c;
    string_T  new_rtp;
    size_t  keep;
    size_t  p_rtp_len;
    string_T  afterdir = {NULL, 0};
    char_u  *after_insp = NULL;
    char_u  *ffname = NULL;
    size_t  fname_len;
    string_T	buf = {NULL, 0};
    цяло	    retval = FAIL;

    p4 = p3 = p2 = p1 = get_past_head(fname);
    за (p = p1; *p; MB_PTR_ADV(p))
	ако (vim_ispathsep_nocolon(*p))
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
    ако (ffname == NULL)
	върни FAIL;

    // Find "ffname" in "p_rtp", ignoring '/' vs '\' differences.
    // Also stop at the first "after" directory.
    fname_len = STRLEN(ffname);
    buf.string = alloc(MAXPATHL);
    ако (buf.string == NULL)
	goto theend;

    p_rtp_len = 0;
    за (entry = p_rtp; *entry != NUL; )
    {
	char_u *cur_entry = entry;

	buf.length = (size_t)copy_option_part(&entry, buf.string, MAXPATHL, ",");

	// keep track of p_rtp length as we go to make the STRLEN() below have less work to do
	p_rtp_len += (*(cur_entry + buf.length) == ',') ? buf.length + 1 : buf.length;

	ако ((p = (char_u *)strstr((char *)buf.string, "after")) != NULL
		&& p > buf.string
		&& vim_ispathsep(p[-1])
		&& (vim_ispathsep(p[5]) || p[5] == NUL || p[5] == ','))
	{
	    ако (insp == NULL)
		// Did not find "ffname" before the first "after" directory,
		// insert it before this entry.
		insp = cur_entry;
	    after_insp = cur_entry;
	    break;
	}

	ако (insp == NULL)
	{
	    char_u  *rtp_ffname;
	    цяло	    match;

	    ако (*buf.string != NUL && !after_pathsep(buf.string, buf.string + buf.length))
		STRCPY(buf.string + buf.length, PATHSEPSTR);
	    rtp_ffname = fix_fname(buf.string);
	    ако (rtp_ffname == NULL)
		goto theend;
	    match = vim_fnamencmp(rtp_ffname, ffname, fname_len) == 0;
	    vim_free(rtp_ffname);
	    ако (match)
		// Insert "ffname" after this entry (and comma).
		insp = entry;
	}
    }

    // finish measuring the length of p_rtp
    p_rtp_len += STRLEN(p_rtp + p_rtp_len);
    ако (insp == NULL)
	// Both "fname" and "after" not found, append at the end.
	insp = p_rtp + p_rtp_len;

    // check if rtp/pack/name/start/name/after exists
    fname_len = STRLEN(fname);
    concat_fnames(fname, fname_len, (char_u *)"after", STRLEN_LITERAL("after"), TRUE, &afterdir);
    ако (afterdir.string == NULL || !mch_isdir(afterdir.string))
	afterdir.length = 0;

    new_rtp.string = alloc(p_rtp_len + fname_len + afterdir.length + 3); // add two for commas and one for NUL
    ако (new_rtp.string == NULL)
	goto theend;

    // We now have 'rtp' parts: {keep}{keep_after}{rest}.
    // Create new_rtp, first: {keep},{fname}
    keep = (size_t)(insp - p_rtp);
    mch_memmove(new_rtp.string, p_rtp, keep);
    new_rtp.length = keep;
    ако (*insp == NUL)
	new_rtp.string[new_rtp.length++] = ',';  // add comma before
    mch_memmove(new_rtp.string + new_rtp.length, fname, fname_len);
    new_rtp.length += fname_len;
    ако (*insp != NUL)
	new_rtp.string[new_rtp.length++] = ',';  // add comma after

    ако (afterdir.length > 0 && after_insp != NULL)
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

    ако (p_rtp[keep] != NUL)
    {
	size_t	append_len = p_rtp_len - keep;

	// Append rest: {keep},{fname}{keep_after},{afterdir}{rest}
	mch_memmove(new_rtp.string + new_rtp.length, p_rtp + keep, append_len + 1);	// add one for NUL
	new_rtp.length += append_len;
    }
    иначе
	new_rtp.string[new_rtp.length] = NUL;

    ако (afterdir.length > 0 && after_insp == NULL)
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
    върни retval;
}

/*
 * Load scripts in "plugin" and "ftdetect" directories of the package.
 */
    static цяло
load_pack_plugin(char_u *fname)
{
    static char *plugpat = "%s/plugin/**/*.vim";
    static char *ftpat = "%s/ftdetect/*.vim";
    цяло		len;
    char_u	*ffname = fix_fname(fname);
    char_u	*pat = NULL;
    цяло		retval = FAIL;

    ако (ffname == NULL)
	върни FAIL;
    len = (цяло)STRLEN(ffname) + (цяло)STRLEN(ftpat);
    pat = alloc(len);
    ако (pat == NULL)
	goto theend;
    vim_snprintf((char *)pat, len, plugpat, ffname);
    source_all_matches(pat);

    {
	char_u *cmd = vim_strsave((char_u *)"g:did_load_filetypes");

	// If runtime/filetype.vim wasn't loaded yet, the scripts will be
	// found when it loads.
	ако (cmd != NULL && eval_to_number(cmd, FALSE) > 0)
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
    върни retval;
}

// used for "cookie" of add_pack_plugin()
static цяло APP_ADD_DIR;
static цяло APP_LOAD;
static цяло APP_BOTH;

    static void
add_pack_plugin(char_u *fname, void *cookie)
{
    ако (cookie != &APP_LOAD)
    {
	char_u	*buf = alloc(MAXPATHL);
	char_u	*p;
	цяло	found = FALSE;

	ако (buf == NULL)
	    върни;
	p = p_rtp;
	докато (*p != NUL)
	{
	    copy_option_part(&p, buf, MAXPATHL, ",");
	    ако (pathcmp((char *)buf, (char *)fname, -1) == 0)
	    {
		found = TRUE;
		break;
	    }
	}
	vim_free(buf);
	ако (!found)
	    // directory is not yet in 'runtimepath', add it
	    ако (add_pack_dir_to_rtp(fname) == FAIL)
		върни;
    }

    ако (cookie != &APP_ADD_DIR)
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
    ако (!did_source_packages || eap->forceit)
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
    цяло		len;
    char	*pat;
    цяло		round;
    цяло		res = OK;

    // Round 1: use "start", round 2: use "opt".
    за (round = 1; round <= 2; ++round)
    {
	// Only look under "start" when loading packages wasn't done yet.
	ако (round == 1 && did_source_packages)
	    continue;

	len = (цяло)STRLEN(plugpat) + (цяло)STRLEN(eap->arg) + 5;
	pat = alloc(len);
	ако (pat == NULL)
	    върни;
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
    цяло	    i;
    цяло	    j;
    char_u  **fnames = (char_u **)gap->ga_data;

    sort_strings(fnames, gap->ga_len);
    за (i = gap->ga_len - 1; i > 0; --i)
	ако (fnamecmp(fnames[i - 1], fnames[i]) == 0)
	{
	    vim_free(fnames[i]);
	    за (j = i + 1; j < gap->ga_len; ++j)
		fnames[j - 1] = fnames[j];
	    --gap->ga_len;
	}
}

    static void
ExpandRTDir_int(
    char_u	*pat,
    size_t	pat_len,
    цяло		flags,
    цяло		keep_ext,
    garray_T	*gap,
    char	*dirnames[])
{
    за (цяло i = 0; dirnames[i] != NULL; ++i)
    {
	const size_t	buf_len = STRLEN(dirnames[i]) + pat_len + 64;
	char		*buf = alloc(buf_len);
	ако (buf == NULL)
	{
	    ga_clear_strings(gap);
	    върни;
	}
	цяло		glob_flags = 0;
	цяло		expand_dirs = FALSE;

	// Build base pattern
	vim_snprintf(buf, buf_len, "%s%s%s%s",
		     *dirnames[i] ? dirnames[i] : "", *dirnames[i] ? "/" : "",
		     pat, "*.vim");

expand:
	ако ((flags & DIP_NORTP) == 0)
	    globpath(p_rtp, (char_u *)buf, gap, glob_flags, expand_dirs);

	ако (flags & DIP_START)
	{
	    // Build complete search path: pack/*/start/*/dirnames[i]/pat*.vim
	    vim_snprintf(buf, buf_len, "pack/*/start/*/%s%s%s%s",
			 *dirnames[i] ? dirnames[i] : "",
			 *dirnames[i] ? "/" : "",
			 pat,
			 expand_dirs ? "*" : "*.vim");
	    globpath(p_pp, (char_u *)buf, gap, glob_flags, expand_dirs);
	}

	ако (flags & DIP_OPT)
	{
	    // Build complete search path: pack/*/opt/*/dirnames[i]/pat*.vim
	    vim_snprintf(buf, buf_len, "pack/*/opt/*/%s%s%s%s",
			 *dirnames[i] ? dirnames[i] : "",
			 *dirnames[i] ? "/" : "", pat,
			 expand_dirs ? "*" : "*.vim");
	    globpath(p_pp, (char_u *)buf, gap, glob_flags, expand_dirs);
	}

	// Second round for directories
	ако (*dirnames[i] == NUL && !expand_dirs)
	{
	    // expand dir names in another round
	    vim_snprintf(buf, buf_len, "%s*", pat);
	    glob_flags = WILD_ADD_SLASH;
	    expand_dirs = TRUE;
	    goto expand;
	}

	vim_free(buf);
    }

    цяло pat_pathsep_cnt = 0;
    за (size_t i = 0; i < pat_len; ++i)
    {
	ако (vim_ispathsep(pat[i]))
	    ++pat_pathsep_cnt;
    }

    за (цяло i = 0; i < gap->ga_len; ++i)
    {
	char_u *match = ((char_u **)gap->ga_data)[i];
	char_u *s = match;
	char_u *e = s + STRLEN(s);
	ако (e - 4 > s && !keep_ext && STRNICMP(e - 4, ".vim", 4) == 0)
	{
	    e -= 4;
	    *e = NUL;
	}

	цяло match_pathsep_cnt = (e > s && e[-1] == '/') ? -1 : 0;
	за (s = e; s > match; MB_PTR_BACK(match, s))
	{
	    ако (s < match || (vim_ispathsep(*s)
				     && ++match_pathsep_cnt > pat_pathsep_cnt))
		break;
	}
	++s;
	ако (s != match)
	    mch_memmove(match, s, e - s + 1);
    }

    ако (gap->ga_len == 0)
	върни;

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
    цяло
ExpandRTDir(
    char_u	*pat,
    цяло		flags,
    цяло		*num_file,
    char_u	***file,
    char	*dirnames[])
{
    *num_file = 0;
    *file = NULL;

    garray_T	ga;
    ga_init2(&ga, sizeof(char *), 10);

    ExpandRTDir_int(pat, STRLEN(pat), flags, FALSE, &ga, dirnames);

    ако (ga.ga_len == 0)
	върни FAIL;

    *file = ga.ga_data;
    *num_file = ga.ga_len;
    върни OK;
}

/*
 * Handle command line completion for :runtime command.
 */
    цяло
expand_runtime_cmd(char_u *pat, цяло *numMatches, char_u ***matches)
{
    *numMatches = 0;
    *matches = NULL;

    garray_T	ga;
    ga_init2(&ga, sizeof(char *), 10);

    size_t pat_len = (цяло)STRLEN(pat);
    char *dirnames[] = {"", NULL};
    ExpandRTDir_int(pat, pat_len, runtime_expand_flags, TRUE, &ga, dirnames);

    // Try to complete values for [where] argument when none was found.
    ако (runtime_expand_flags == 0)
    {
	char *where_values[] = {"START", "OPT", "PACK", "ALL"};
	за (size_t i = 0; i < ARRAY_LENGTH(where_values); ++i)
	    ако (STRNCMP(pat, where_values[i], pat_len) == 0)
	    {
		char_u *p = vim_strsave((char_u *)where_values[i]);
		ако (p != NULL && ga_add_string(&ga, p) == FAIL)
		    vim_free(p);
	    }
    }

    ако (ga.ga_len == 0)
	върни FAIL;

    *matches = ga.ga_data;
    *numMatches = ga.ga_len;
    върни OK;
}

/*
 * Expand loadplugin names:
 * 'packpath'/pack/ * /opt/{pat}
 */
    цяло
ExpandPackAddDir(
    char_u	*pat,
    цяло		*num_file,
    char_u	***file)
{
    char_u	*s;
    char_u	*e;
    char_u	*match;
    garray_T	ga;
    цяло		i;
    цяло		pat_len;

    *num_file = 0;
    *file = NULL;
    pat_len = (цяло)STRLEN(pat);
    ga_init2(&ga, sizeof(char *), 10);

    s = alloc(pat_len + 26);
    ако (s == NULL)
    {
	ga_clear_strings(&ga);
	върни FAIL;
    }
    sprintf((char *)s, "pack/*/opt/%s*", pat);
    globpath(p_pp, s, &ga, 0, TRUE);
    vim_free(s);

    за (i = 0; i < ga.ga_len; ++i)
    {
	match = ((char_u **)ga.ga_data)[i];
	s = gettail(match);
	e = s + STRLEN(s);
	mch_memmove(match, s, e - s + 1);
    }

    ако (ga.ga_len == 0)
	върни FAIL;

    // Sort and remove duplicates which can happen when specifying multiple
    // directories in dirnames.
    remove_duplicates(&ga);

    *file = ga.ga_data;
    *num_file = ga.ga_len;
    върни OK;
}

    static void
cmd_source(char_u *fname, exarg_T *eap)
{
    цяло clearvars = FALSE;

    ако (*fname != NUL && STRNCMP(fname, "++clear", 7) == 0)
    {
	// ++clear argument is supplied
	clearvars = TRUE;
	fname = fname + 7;
	ако (*fname != NUL)
	{
	    semsg(_(e_invalid_argument_str), eap->arg);
	    върни;
	}
    }

    ако (*fname != NUL && eap != NULL && eap->addr_count > 0)
    {
	// if a filename is specified to :source, then a range is not allowed
	emsg(_(e_no_range_allowed));
	върни;
    }

    ако (eap != NULL && *fname == NUL)
    {
	ако (eap->forceit)
	    // a file name is needed to source normal mode commands
	    emsg(_(e_argument_required));
	иначе
	    // source ex commands from the current buffer
	    do_source_ext(NULL, FALSE, DOSO_NONE, NULL, eap, clearvars);
    }
    иначе ако (eap != NULL && eap->forceit)
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
    иначе ако (do_source(fname, FALSE, DOSO_NONE, NULL) == FAIL)
	semsg(_(e_cant_open_file_str), fname);
}

/*
 * ":source {fname}"
 */
    void
ex_source(exarg_T *eap)
{
#ifdef FEAT_BROWSE
    ако (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	char_u *fname = NULL;

	fname = do_browse(0, (char_u *)_("Source Vim script"), eap->arg,
				      NULL, NULL,
				      (char_u *)_(BROWSE_FILTER_MACROS), NULL);
	ако (fname != NULL)
	{
	    cmd_source(fname, eap);
	    vim_free(fname);
	}
    }
    иначе
#endif
	cmd_source(eap->arg, eap);
}

#ако defined(FEAT_EVAL)
/*
 * ":options"
 */
    void
ex_options(
    exarg_T	*eap UNUSED)
{
    char_u  buf[500];
    цяло	    multi_mods = 0;

    buf[0] = NUL;
    (void)add_win_cmd_modifiers(buf, &cmdmod, &multi_mods);

    vim_setenv((char_u *)"OPTWIN_CMD", buf);
    cmd_source((char_u *)SYS_OPTWIN_FILE, NULL);
}
#endif

/*
 * ":source" and associated commands.
 */

#ако defined(FEAT_EVAL)
/*
 * Return the address holding the next breakpoint line for a source cookie.
 */
    linenr_T *
source_breakpoint(void *cookie)
{
    върни &((source_cookie_T *)cookie)->breakpoint;
}

/*
 * Return the address holding the debug tick for a source cookie.
 */
    цяло *
source_dbg_tick(void *cookie)
{
    върни &((source_cookie_T *)cookie)->dbg_tick;
}

/*
 * Return the nesting level for a source cookie.
 */
    цяло
source_level(void *cookie)
{
    върни ((source_cookie_T *)cookie)->level;
}

/*
 * Return the readahead line. Note that the pointer may become invalid when
 * getting the next line, if it's concatenated with the next one.
 */
    char_u *
source_nextline(void *cookie)
{
    върни ((source_cookie_T *)cookie)->nextline;
}
#endif

#ако (defined(MSWIN) && defined(FEAT_CSCOPE)) || defined(HAVE_FD_CLOEXEC)
# define USE_FOPEN_NOINH
/*
 * Special function to open a file without handle inheritance.
 * When possible the handle is closed on exec().
 */
    static FILE *
fopen_noinh_readbin(char *filename)
{
# ifdef MSWIN
    цяло	fd_tmp = mch_open(filename, O_RDONLY | O_BINARY | O_NOINHERIT, 0);
# иначе
    цяло	fd_tmp = mch_open(filename, O_RDONLY, 0);
# endif

    ако (fd_tmp == -1)
	върни NULL;

# ifdef HAVE_FD_CLOEXEC
    {
	цяло fdflags = fcntl(fd_tmp, F_GETFD);
	ако (fdflags >= 0 && (fdflags & FD_CLOEXEC) == 0)
	    (void)fcntl(fd_tmp, F_SETFD, fdflags | FD_CLOEXEC);
    }
# endif

    върни fdopen(fd_tmp, READBIN);
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

    ако (curbuf == NULL)
	върни NULL;

    // Use ":source buffer=<num>" as the script name
    ако (curbuf->b_ffname != NULL)
	fname = vim_strsave(curbuf->b_ffname);
    иначе
    {
	vim_snprintf((char *)IObuff, IOSIZE, ":source buffer=%d", curbuf->b_fnum);
	fname = vim_strsave(IObuff);
    }
    ако (fname == NULL)
	върни NULL;

    ga_init2(&sp->buflines, sizeof(char_u *), 100);

    // Copy the lines from the buffer into a grow array
    за (curr_lnum = eap->line1; curr_lnum <= eap->line2; curr_lnum++)
    {
	line = vim_strsave(ml_get(curr_lnum));
	ако (line == NULL)
	    goto errret;
	ако (ga_add_string(&sp->buflines, line) == FAIL)
	    goto errret;
    }
    sp->buf_lnum = 0;
    sp->source_from_buf = TRUE;
    // When sourcing a range of lines from a buffer, use buffer line number.
    sp->sourcing_lnum = eap->line1 - 1;

    върни fname;

errret:
    vim_free(fname);
    vim_free(line);
    ga_clear_strings(&sp->buflines);
    върни NULL;
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
    static цяло
do_source_ext(
    char_u	*fname,
    цяло		check_other,	    // check for .vimrc and _vimrc
    цяло		is_vimrc,	    // DOSO_ value
    цяло		*ret_sid UNUSED,
    exarg_T	*eap,
    цяло		clearvars UNUSED)
{
    source_cookie_T	    cookie;
    char_u		    *p;
    char_u		    *fname_not_fixed = NULL;
    char_u		    *fname_exp = NULL;
    char_u		    *firstline = NULL;
    цяло			    retval = FAIL;
    sctx_T		    save_current_sctx;
#ifdef STARTUPTIME
    struct timeval	    tv_rel;
    struct timeval	    tv_start;
#endif
#ifdef FEAT_PROFILE
    proftime_T		    wait_start;
#endif
    цяло			    save_sticky_cmdmod_flags = sticky_cmdmod_flags;
    цяло			    trigger_source_post = FALSE;
#ifdef FEAT_EVAL
    funccal_entry_T	    funccalp_entry;
    цяло			    save_debug_break_level = debug_break_level;
    цяло			    sid = -1;
    scriptitem_T	    *si = NULL;
    цяло			    save_estack_compiling = estack_compiling;
    ESTACK_CHECK_DECLARATION;
#endif

    CLEAR_FIELD(cookie);
    ако (fname == NULL)
    {
	// sourcing lines from a buffer
	fname_exp = do_source_buffer_init(&cookie, eap);
	ако (fname_exp == NULL)
	    върни FAIL;
    }
    иначе
    {
	fname_not_fixed = expand_env_save(fname);
	ако (fname_not_fixed == NULL)
	    goto theend;
	fname_exp = fix_fname(fname_not_fixed);
	ако (fname_exp == NULL)
	    goto theend;
	ако (mch_isdir(fname_exp))
	{
	    smsg(_("Cannot source a directory: \"%s\""), fname);
	    goto theend;
	}
    }
#ifdef FEAT_EVAL
    estack_compiling = FALSE;

    // See if we loaded this script before.
    sid = find_script_by_name(fname_exp);
    ако (sid > 0 && ret_sid != NULL
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
    ако ((fname == NULL
		? has_autocmd(EVENT_SOURCECMD, fname_exp, NULL)
		: has_autocmd_fullname(EVENT_SOURCECMD, fname_exp, NULL))
	    && apply_autocmds(EVENT_SOURCECMD, fname_exp, fname_exp,
							       FALSE, curbuf))
    {
#ifdef FEAT_EVAL
	retval = aborting() ? FAIL : OK;
#иначе
	retval = OK;
#endif
	ако (retval == OK)
	    // Apply SourcePost autocommands.
	    apply_autocmds(EVENT_SOURCEPOST, fname_exp, fname_exp,
								FALSE, curbuf);
	goto theend;
    }

    // Apply SourcePre autocommands, they may get the file.
    apply_autocmds(EVENT_SOURCEPRE, fname_exp, fname_exp, FALSE, curbuf);

    ако (!cookie.source_from_buf)
    {
#ifdef USE_FOPEN_NOINH
	cookie.fp = fopen_noinh_readbin((char *)fname_exp);
#иначе
	cookie.fp = mch_fopen((char *)fname_exp, READBIN);
#endif
    }
    ако (cookie.fp == NULL && check_other)
    {
	// Try again, replacing file name ".vimrc" by "_vimrc" or vice versa,
	// and ".exrc" by "_exrc" or vice versa.
	p = gettail(fname_exp);
	ако ((*p == '.' || *p == '_')
		&& (STRICMP(p + 1, "vimrc") == 0
		    || STRICMP(p + 1, "gvimrc") == 0
		    || STRICMP(p + 1, "exrc") == 0))
	{
	    ако (*p == '_')
		*p = '.';
	    иначе
		*p = '_';
#ifdef USE_FOPEN_NOINH
	    cookie.fp = fopen_noinh_readbin((char *)fname_exp);
#иначе
	    cookie.fp = mch_fopen((char *)fname_exp, READBIN);
#endif
	}
    }

    ако (cookie.fp == NULL && !cookie.source_from_buf)
    {
	ако (p_verbose > 0)
	{
	    verbose_enter();
	    ако (SOURCING_NAME == NULL)
		smsg(_("could not source \"%s\""), fname);
	    иначе
		smsg(_("line %ld: could not source \"%s\""),
							SOURCING_LNUM, fname);
	    verbose_leave();
	}
	goto theend;
    }

    // The file exists.
    // - In verbose mode, give a message.
    // - For a vimrc file, may want to set 'compatible', call vimrc_found().
    ако (p_verbose > 1)
    {
	verbose_enter();
	ако (SOURCING_NAME == NULL)
	    smsg(_("sourcing \"%s\""), fname);
	иначе
	    smsg(_("line %ld: sourcing \"%s\""), SOURCING_LNUM, fname);
	verbose_leave();
    }
    ако (is_vimrc == DOSO_VIMRC)
	vimrc_found(fname_exp, (char_u *)"MYVIMRC");
    иначе ако (is_vimrc == DOSO_GVIMRC)
	vimrc_found(fname_exp, (char_u *)"MYGVIMRC");

#ifdef USE_CRNL
    // If no automatic file format: Set default to CR-NL.
    ако (*p_ffs == NUL)
	cookie.fileformat = EOL_DOS;
    иначе
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
    ако (time_fd != NULL)
	time_push(&tv_rel, &tv_start);
#endif

    // "legacy" does not apply to commands in the script
    sticky_cmdmod_flags = 0;

    save_current_sctx = current_sctx;
    ако ((cmdmod.cmod_flags & CMOD_VIM9CMD) && cookie.source_from_buf)
	// When the ":vim9cmd" command modifier is used, source buffer lines as
	// Vim9 script
	current_sctx.sc_version = SCRIPT_VERSION_VIM9;
    иначе
	current_sctx.sc_version = 1;  // default script version

#ifdef FEAT_EVAL
    current_sctx.sc_lnum = 0;

# ifdef FEAT_PROFILE
    ако (do_profiling == PROF_YES)
	prof_child_enter(&wait_start);		// entering a child now
# endif

    // Don't use local function variables, if called from a function.
    // Also starts profiling timer for nested script.
    save_funccal(&funccalp_entry);

    // Reset "KeyTyped" to avoid some commands thinking they are invoked
    // interactively.  E.g. defining a function would output indent.
    цяло save_KeyTyped = KeyTyped;
    KeyTyped = FALSE;

    // Check if this script was sourced before to find its SID.
    // Always use a new sequence number.
    current_sctx.sc_seq = ++last_current_SID_seq;
    ако (sid > 0)
    {
	hashtab_T	*ht;
	цяло		todo;
	hashitem_T	*hi;
	dictitem_T	*di;

	// loading the same script again
	current_sctx.sc_sid = sid;
	si = SCRIPT_ITEM(sid);
	ако (si->sn_state == SN_STATE_NOT_LOADED)
	{
	    // this script was found but not loaded yet
	    si->sn_state = SN_STATE_NEW;
	}
	иначе
	{
	    si->sn_state = SN_STATE_RELOAD;

	    ако (!clearvars)
	    {
		// Script-local variables remain but "const" can be set again.
		// In Vim9 script variables will be cleared when "vim9script"
		// is encountered without the "noclear" argument.
		ht = &SCRIPT_VARS(sid);
		todo = (цяло)ht->ht_used;
		FOR_ALL_HASHTAB_ITEMS(ht, hi, todo)
		    ако (!HASHITEM_EMPTY(hi))
		    {
			--todo;
			di = HI2DI(hi);
			di->di_flags |= DI_FLAGS_RELOAD;
		    }
		// imports can be redefined once
		mark_imports_for_reload(sid);
	    }
	    иначе
		clear_vim9_scriptlocal_vars(sid);

	    // reset version, "vim9script" may have been added or removed.
	    si->sn_version = 1;
	}
	ако (ret_sid != NULL)
	    *ret_sid = sid;
    }
    иначе
    {
	цяло error = OK;

	// It's new, generate a new SID and initialize the scriptitem.
	sid = get_new_scriptitem(&error);
	current_sctx.sc_sid = sid;
	ако (error == FAIL)
	    goto almosttheend;
	si = SCRIPT_ITEM(sid);
	si->sn_name = fname_exp;
	fname_exp = vim_strsave(si->sn_name);  // used for autocmd
	ако (ret_sid != NULL)
	    *ret_sid = sid;

	// Remember the "is_vimrc" flag for when the file is sourced again.
	si->sn_is_vimrc = is_vimrc;
    }

    // Keep the sourcing name/lnum, for recursive calls.
    estack_push(ETYPE_SCRIPT, si->sn_name, 0);
    ESTACK_CHECK_SETUP;

# ifdef FEAT_PROFILE
    ако (do_profiling == PROF_YES)
    {
	цяло	forceit;

	// Check if we do profiling for this script.
	ако (!si->sn_prof_on && has_profiling(TRUE, si->sn_name, &forceit, NULL))
	{
	    script_do_profile(si);
	    si->sn_pr_force = forceit;
	}
	ако (si->sn_prof_on)
	{
	    ++si->sn_pr_count;
	    profile_start(&si->sn_pr_start);
	    profile_zero(&si->sn_pr_children);
	}
    }
# endif
#иначе
    // Keep the sourcing name/lnum, for recursive calls.
    estack_push(ETYPE_SCRIPT, fname_exp, 0);
#endif

    cookie.conv.vc_type = CONV_NONE;		// no conversion

    // Read the first line so we can check for a UTF-8 BOM.
    firstline = getsourceline(0, (void *)&cookie, 0, TRUE);
    ако (firstline != NULL && STRLEN(firstline) >= 3 && firstline[0] == 0xef
			      && firstline[1] == 0xbb && firstline[2] == 0xbf)
    {
	// Found BOM; setup conversion, skip over BOM and recode the line.
	convert_setup(&cookie.conv, (char_u *)"utf-8", p_enc);
	p = string_convert(&cookie.conv, firstline + 3, NULL);
	ако (p == NULL)
	    p = vim_strsave(firstline + 3);
	ако (p != NULL)
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
    ако (do_profiling == PROF_YES)
    {
	// Get "si" again, "script_items" may have been reallocated.
	si = SCRIPT_ITEM(sid);
	ако (si->sn_prof_on)
	{
	    profile_end(&si->sn_pr_start);
	    profile_sub_wait(&wait_start, &si->sn_pr_start);
	    profile_add(&si->sn_pr_total, &si->sn_pr_start);
	    profile_self(&si->sn_pr_self, &si->sn_pr_start,
							 &si->sn_pr_children);
	}
    }
#endif

    ако (got_int)
	emsg(_(e_interrupted));
#ifdef FEAT_EVAL
    ESTACK_CHECK_NOW;
#endif
    estack_pop();
    ако (p_verbose > 1)
    {
	verbose_enter();
	smsg(_("finished sourcing %s"), fname);
	ако (SOURCING_NAME != NULL)
	    smsg(_("continuing in %s"), SOURCING_NAME);
	verbose_leave();
    }
#ifdef STARTUPTIME
    ако (time_fd != NULL)
    {
	vim_snprintf((char *)IObuff, IOSIZE, "sourcing %s", fname);
	time_msg((char *)IObuff, &tv_start);
	time_pop(&tv_rel);
    }
#endif

    ако (!got_int)
	trigger_source_post = TRUE;

#ifdef FEAT_EVAL
    // After a "finish" in debug mode, need to break at first command of next
    // sourced file.
    ако (save_debug_break_level > ex_nesting_level
	    && debug_break_level == ex_nesting_level)
	++debug_break_level;
#endif

#ifdef FEAT_EVAL
almosttheend:
    // If "sn_save_cpo" is set that means we encountered "vim9script": restore
    // 'cpoptions', unless in the main .vimrc file.
    // Get "si" again, "script_items" may have been reallocated.
    si = SCRIPT_ITEM(sid);
    ако (si->sn_save_cpo != NULL && si->sn_is_vimrc == DOSO_NONE)
    {
	ако (STRCMP(p_cpo, CPO_VIM) != 0)
	{
	    char_u *f;
	    char_u *t;

	    // 'cpo' was changed in the script.  Apply the same change to the
	    // saved value, if possible.
	    за (f = (char_u *)CPO_VIM; *f != NUL; ++f)
		ако (vim_strchr(p_cpo, *f) == NULL
			&& (t = vim_strchr(si->sn_save_cpo, *f)) != NULL)
		    // flag was removed, also remove it from the saved 'cpo'
		    mch_memmove(t, t + 1, STRLEN(t));
	    за (f = p_cpo; *f != NUL; ++f)
		ако (vim_strchr((char_u *)CPO_VIM, *f) == NULL
			&& vim_strchr(si->sn_save_cpo, *f) == NULL)
		{
		    // flag was added, also add it to the saved 'cpo'
		    t = alloc(STRLEN(si->sn_save_cpo) + 2);
		    ако (t != NULL)
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
    ако (do_profiling == PROF_YES)
	prof_child_exit(&wait_start);		// leaving a child now
# endif

    KeyTyped = save_KeyTyped;
#endif
    current_sctx = save_current_sctx;

    ако (cookie.fp != NULL)
	fclose(cookie.fp);
    ако (cookie.source_from_buf)
	ga_clear_strings(&cookie.buflines);
    vim_free(cookie.nextline);
    vim_free(firstline);
    convert_setup(&cookie.conv, NULL, NULL);

    ако (trigger_source_post)
	apply_autocmds(EVENT_SOURCEPOST, fname_exp, fname_exp, FALSE, curbuf);

theend:
#ifdef FEAT_EVAL
    ако (sid > 0 && ret_sid != NULL
	    && fname_not_fixed != NULL && fname_exp != NULL)
    {
	цяло not_fixed_sid = find_script_by_name(fname_not_fixed);

	// If "fname_not_fixed" is a symlink then we source the linked file.
	// If the original name is in the script list we add the ID of the
	// script that was actually sourced.
	ако (SCRIPT_ID_VALID(not_fixed_sid) && not_fixed_sid != sid)
	    SCRIPT_ITEM(not_fixed_sid)->sn_sourced_sid = sid;
    }
#endif

    vim_free(fname_not_fixed);
    vim_free(fname_exp);
    sticky_cmdmod_flags = save_sticky_cmdmod_flags;
#ifdef FEAT_EVAL
    estack_compiling = save_estack_compiling;
#endif
    върни retval;
}

    цяло
do_source(
    char_u	*fname,
    цяло		check_other,	    // check for .vimrc and _vimrc
    цяло		is_vimrc,	    // DOSO_ value
    цяло		*ret_sid)
{
    върни do_source_ext(fname, check_other, is_vimrc, ret_sid, NULL, FALSE);
}


#ако defined(FEAT_EVAL)

/*
 * ":scriptnames"
 */
    void
ex_scriptnames(exarg_T *eap)
{
    цяло i;

    ако (eap->addr_count > 0 || *eap->arg != NUL)
    {
	// :script {scriptId}: edit the script
	ако (eap->addr_count > 0 && !SCRIPT_ID_VALID(eap->line2))
	    emsg(_(e_invalid_argument));
	иначе
	{
	    ако (eap->addr_count > 0)
		eap->arg = SCRIPT_ITEM(eap->line2)->sn_name;
	    иначе
	    {
		expand_env(eap->arg, NameBuff, MAXPATHL);
		eap->arg = NameBuff;
	    }
	    do_exedit(eap, NULL);
	}
	върни;
    }

    за (i = 1; i <= script_items.ga_len && !got_int; ++i)
    {
	scriptitem_T *si = SCRIPT_ITEM(i);

	ако (si->sn_name != NULL)
	{
	    char sourced_buf[20];

	    home_replace(NULL, si->sn_name, NameBuff, MAXPATHL, TRUE);
	    ако (si->sn_sourced_sid > 0)
		vim_snprintf(sourced_buf, 20, "->%d", si->sn_sourced_sid);
	    иначе
		sourced_buf[0] = NUL;
	    vim_snprintf((char *)IObuff, IOSIZE, "%3d%s%s: %s",
		    i,
		    sourced_buf,
		    si->sn_state == SN_STATE_NOT_LOADED ? " A" : "",
		    NameBuff);
	    ако (!message_filtered(IObuff))
	    {
		msg_putchar('\n');
		msg_outtrans(IObuff);
		out_flush();	    // output one line at a time
		ui_breakcheck();
	    }
	}
    }
}

# ако defined(BACKSLASH_IN_FILENAME)
/*
 * Fix slashes in the list of script names for 'shellslash'.
 */
    void
scriptnames_slash_adjust(void)
{
    цяло i;

    за (i = 1; i <= script_items.ga_len; ++i)
	ако (SCRIPT_ITEM(i)->sn_name != NULL)
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
    ако (id == SID_MODELINE)
	върни (char_u *)_("modeline");
    ако (id == SID_CMDARG)
	върни (char_u *)_("--cmd argument");
    ако (id == SID_CARG)
	върни (char_u *)_("-c argument");
    ако (id == SID_ENV)
	върни (char_u *)_("environment variable");
    ако (id == SID_ERROR)
	върни (char_u *)_("error handler");
    ако (id == SID_WINLAYOUT)
	върни (char_u *)_("changed window size");
    върни SCRIPT_ITEM(id)->sn_name;
}

# ако defined(EXITFREE)
    void
free_scriptnames(void)
{
    цяло			i;

    за (i = script_items.ga_len; i > 0; --i)
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
	char_u *(*fgetline)(цяло, void *, цяло, getline_opt_T),
	void *cookie)
{
    върни fgetline == getsourceline
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
    ако (l == NULL)
	върни NULL;

    // Iterate through all the functions in the global function hash table
    // looking for functions with script ID 'sid'.
    functbl = func_tbl_get();
    todo = functbl->ht_used;
    FOR_ALL_HASHTAB_ITEMS(functbl, hi, todo)
    {
	ufunc_T	*fp;

	ако (HASHITEM_EMPTY(hi))
	    continue;

	--todo;
	fp = HI2UF(hi);

	// Add active functions with script id == 'sid'
	ако (!(fp->uf_flags & FC_DEAD) && (fp->uf_script_ctx.sc_sid == sid))
	{
	    char_u	*name;

	    ако (fp->uf_name_exp != NULL)
		name = fp->uf_name_exp;
	    иначе
		name = fp->uf_name;

	    list_append_string(l, name, -1);
	}
    }

    върни l;
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
    цяло		filterpat = FALSE;
    scid_T	sid = -1;

    ако (rettv_list_alloc(rettv) == FAIL)
	върни;

    ако (check_for_opt_dict_arg(argvars, 0) == FAIL)
	върни;

    l = rettv->vval.v_list;

    regmatch.regprog = NULL;
    regmatch.rm_ic = p_ic;

    ако (argvars[0].v_type == VAR_DICT)
    {
	dictitem_T *sid_di = dict_find(argvars[0].vval.v_dict,
							   (char_u *)"sid", 3);
	ако (sid_di != NULL)
	{
	    цяло error = FALSE;
	    sid = tv_get_number_chk(&sid_di->di_tv, &error);
	    ако (error)
		върни;
	    ако (sid <= 0)
	    {
		semsg(_(e_invalid_value_for_argument_str_str), "sid",
						tv_get_string(&sid_di->di_tv));
		върни;
	    }
	}
	иначе
	{
	    pat = dict_get_string(argvars[0].vval.v_dict, "name", TRUE);
	    ако (pat != NULL)
		regmatch.regprog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
	    ако (regmatch.regprog != NULL)
		filterpat = TRUE;
	}
    }

    за (varnumber_T i = sid > 0 ? sid : 1;
		       (i == sid || sid <= 0) && i <= script_items.ga_len; ++i)
    {
	scriptitem_T	*si = SCRIPT_ITEM(i);
	dict_T		*d;

	ако (si->sn_name == NULL)
	    continue;

	ако (filterpat && !vim_regexec(&regmatch, si->sn_name, (colnr_T)0))
	    continue;

	ако ((d = dict_alloc()) == NULL
		|| list_append_dict(l, d) == FAIL)
	{
	    dict_unref(d);
	    goto theend;
	}
	ако (dict_add_string(d, "name", si->sn_name) == FAIL
		|| dict_add_number(d, "sid", i) == FAIL
		|| dict_add_number(d, "sourced", si->sn_sourced_sid) == FAIL
		|| dict_add_number(d, "version", si->sn_version) == FAIL
		|| dict_add_bool(d, "autoload",
				si->sn_state == SN_STATE_NOT_LOADED) == FAIL)
	    goto theend;

	// When a script ID is specified, return information about only the
	// specified script, and add the script-local variables and functions.
	ако (sid > 0)
	{
	    dict_T	*var_dict;
	    list_T	*fn_list;

	    var_dict = dict_copy(&si->sn_vars->sv_dict, TRUE, TRUE,
								get_copyID());
	    ако (var_dict == NULL)
		goto theend;
	    ако (dict_add_dict(d, "variables", var_dict) == FAIL)
	    {
		dict_unref(var_dict);
		goto theend;
	    }
	    --var_dict->dv_refcount;
	    fn_list = get_script_local_funcs(sid);
	    ако (fn_list == NULL || dict_add_list(d, "functions", fn_list) == FAIL)
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
    цяло			len;
    цяло			c;
    char_u		*buf;
#ifdef USE_CRNL
    цяло			has_cr;		// CR-LF found
#endif
    цяло			have_read = FALSE;

    // use a growarray to store the sourced line
    ga_init2(&ga, 1, 250);

    // Loop until there is a finished line (or end-of-file).
    ++sp->sourcing_lnum;
    за (;;)
    {
	// make room to read at least 120 (more) characters
	ако (ga_grow(&ga, 120) == FAIL)
	    break;
	ако (sp->source_from_buf)
	{
	    ако (sp->buf_lnum >= sp->buflines.ga_len)
		break;		    // all the lines are processed
	    ga_concat(&ga, ((char_u **)sp->buflines.ga_data)[sp->buf_lnum]);
	    sp->buf_lnum++;
	    ако (ga_grow(&ga, 1) == FAIL)
		break;
	    buf = (char_u *)ga.ga_data;
	    buf[ga.ga_len++] = NUL;
	    len = ga.ga_len;
	}
	иначе
	{
	    buf = (char_u *)ga.ga_data;
	    ако (fgets((char *)buf + ga.ga_len, ga.ga_maxlen - ga.ga_len,
			sp->fp) == NULL)
		break;
	    len = ga.ga_len + (цяло)STRLEN(buf + ga.ga_len);
	}
#ifdef USE_CRNL
	// Ignore a trailing CTRL-Z, when in Dos mode.	Only recognize the
	// CTRL-Z by its own, or after a NL.
	ако (	   (len == 1 || (len >= 2 && buf[len - 2] == '\n'))
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
	ако (!sp->source_from_buf && ga.ga_maxlen - ga.ga_len == 1
						       && buf[len - 1] != '\n')
	    continue;

	ако (len >= 1 && buf[len - 1] == '\n')	// remove trailing NL
	{
#ifdef USE_CRNL
	    has_cr = (len >= 2 && buf[len - 2] == '\r');
	    ако (sp->fileformat == EOL_UNKNOWN)
	    {
		ако (has_cr)
		    sp->fileformat = EOL_DOS;
		иначе
		    sp->fileformat = EOL_UNIX;
	    }

	    ако (sp->fileformat == EOL_DOS)
	    {
		ако (has_cr)	    // replace trailing CR
		{
		    buf[len - 2] = '\n';
		    --len;
		    --ga.ga_len;
		}
		иначе	    // lines like ":map xx yy^M" will have failed
		{
		    ако (!sp->error)
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
	    за (c = len - 2; c >= 0 && buf[c] == Ctrl_V; c--)
		;
	    ако ((len & 1) != (c & 1))	// escaped NL, read more
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

    ако (have_read)
	върни (char_u *)ga.ga_data;

    vim_free(ga.ga_data);
    върни NULL;
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
	цяло c UNUSED,
	void *cookie,
	цяло indent UNUSED,
	getline_opt_T options)
{
    source_cookie_T	*sp = (source_cookie_T *)cookie;
    char_u		*line;
    char_u		*p;
    цяло			do_vim9_all = in_vim9script()
					      && options == GETLINE_CONCAT_ALL;
    цяло			do_bar_cont = do_vim9_all
					 || options == GETLINE_CONCAT_CONTBAR;

#ifdef FEAT_EVAL
    // If breakpoints have been added/deleted need to check for it.
    ако ((sp->dbg_tick < debug_tick) && !sp->source_from_buf)
    {
	sp->breakpoint = dbg_find_breakpoint(TRUE, sp->fname, SOURCING_LNUM);
	sp->dbg_tick = debug_tick;
    }
# ifdef FEAT_PROFILE
    ако (do_profiling == PROF_YES)
	script_line_end();
# endif
#endif

    // Set the current sourcing line number.
    SOURCING_LNUM = sp->sourcing_lnum + 1;

    // Get current line.  If there is a read-ahead line, use it, otherwise get
    // one now.  "fp" is NULL if actually using a string.
    ако (sp->finished || (!sp->source_from_buf && sp->fp == NULL))
	line = NULL;
    иначе ако (sp->nextline == NULL)
	line = get_one_sourceline(sp);
    иначе
    {
	line = sp->nextline;
	sp->nextline = NULL;
	++sp->sourcing_lnum;
    }
#ifdef FEAT_PROFILE
    ако (line != NULL && do_profiling == PROF_YES)
	script_line_start();
#endif

    // Only concatenate lines starting with a \ when 'cpoptions' doesn't
    // contain the 'C' flag.
    ако (line != NULL && options != GETLINE_NONE
				      && vim_strchr(p_cpo, CPO_CONCAT) == NULL)
    {
	цяло comment_char = in_vim9script() ? '#' : '"';

	// compensate for the one line read-ahead
	--sp->sourcing_lnum;

	// Get the next line and concatenate it when it starts with a
	// backslash. We always need to read the next line, keep it in
	// sp->nextline.
	/* Also check for a comment in between continuation lines: "\ */
	// Also check for a Vim9 comment, empty line, line starting with '|',
	// but not "||".
	sp->nextline = get_one_sourceline(sp);
	ако (sp->nextline != NULL
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
	    ако (*p == '\\')
		ga_concat(&ga, p + 1);
	    иначе ако (*p == '|')
	    {
		GA_CONCAT_LITERAL(&ga, " ");
		ga_concat(&ga, p);
	    }
	    за (;;)
	    {
		vim_free(sp->nextline);
		sp->nextline = get_one_sourceline(sp);
		ако (sp->nextline == NULL)
		    break;
		p = skipwhite(sp->nextline);
		ако (*p == '\\' || (do_bar_cont && p[0] == '|' && p[1] != '|'))
		{
		    // Adjust the growsize to the current length to speed up
		    // concatenating many lines.
		    ако (ga.ga_len > 400)
		    {
			ако (ga.ga_len > 8000)
			    ga.ga_growsize = 8000;
			иначе
			    ga.ga_growsize = ga.ga_len;
		    }
		    ако (*p == '\\')
			ga_concat(&ga, p + 1);
		    иначе
		    {
			GA_CONCAT_LITERAL(&ga, " ");
			ga_concat(&ga, p);
		    }
		}
		иначе ако (!(p[0] == (comment_char)
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

    ако (line != NULL && sp->conv.vc_type != CONV_NONE)
    {
	char_u	*s;

	// Convert the encoding of the script line.
	s = string_convert(&sp->conv, line, NULL);
	ако (s != NULL)
	{
	    vim_free(line);
	    line = s;
	}
    }

#ifdef FEAT_EVAL
    // Did we encounter a breakpoint?
    ако (!sp->source_from_buf && sp->breakpoint != 0
	    && sp->breakpoint <= SOURCING_LNUM)
    {
	dbg_breakpoint(sp->fname, SOURCING_LNUM);
	// Find next breakpoint.
	sp->breakpoint = dbg_find_breakpoint(TRUE, sp->fname, SOURCING_LNUM);
	sp->dbg_tick = debug_tick;
    }
#endif

    върни line;
}

/*
 * Returns TRUE if sourcing a script either from a file or a buffer.
 * Otherwise returns FALSE.
 */
    цяло
sourcing_a_script(exarg_T *eap)
{
    върни (getline_equal(eap->ea_getline, eap->cookie, getsourceline));
}

/*
 * ":scriptencoding": Set encoding conversion for a sourced script.
 */
    void
ex_scriptencoding(exarg_T *eap)
{
    source_cookie_T	*sp;
    char_u		*name;

    ако (!sourcing_a_script(eap))
    {
	emsg(_(e_scriptencoding_used_outside_of_sourced_file));
	върни;
    }

    ако (*eap->arg != NUL)
    {
	name = enc_canonize(eap->arg);
	ако (name == NULL)	// out of memory
	    върни;
    }
    иначе
	name = eap->arg;

    // Setup for conversion from the specified encoding to 'encoding'.
    sp = (source_cookie_T *)getline_cookie(eap->ea_getline, eap->cookie);
    convert_setup(&sp->conv, name, p_enc);

    ако (name != eap->arg)
	vim_free(name);
}

/*
 * ":scriptversion": Set Vim script version for a sourced script.
 */
    void
ex_scriptversion(exarg_T *eap UNUSED)
{
    цяло		nr;

    ако (!sourcing_a_script(eap))
    {
	emsg(_(e_scriptversion_used_outside_of_sourced_file));
	върни;
    }
    ако (in_vim9script())
    {
	emsg(_(e_cannot_use_scriptversion_after_vim9script));
	върни;
    }

    nr = getdigits(&eap->arg);
    ако (nr == 0 || *eap->arg != NUL)
	emsg(_(e_invalid_argument));
    иначе ако (nr > SCRIPT_VERSION_MAX)
	semsg(_(e_scriptversion_not_supported_nr), nr);
    иначе
    {
	current_sctx.sc_version = nr;
#ifdef FEAT_EVAL
	SCRIPT_ITEM(current_sctx.sc_sid)->sn_version = nr;
#endif
    }
}

#ако defined(FEAT_EVAL)
/*
 * ":finish": Mark a sourced file as finished.
 */
    void
ex_finish(exarg_T *eap)
{
    ако (sourcing_a_script(eap))
	do_finish(eap, FALSE);
    иначе
	emsg(_(e_finish_used_outside_of_sourced_file));
}

/*
 * Mark a sourced file as finished.  Possibly makes the ":finish" pending.
 * Also called for a pending finish at the ":endtry" or after returning from
 * an extra do_cmdline().  "reanimate" is used in the latter case.
 */
    void
do_finish(exarg_T *eap, цяло reanimate)
{
    цяло		idx;

    ако (reanimate)
	((source_cookie_T *)getline_cookie(eap->ea_getline,
					      eap->cookie))->finished = FALSE;

    // Cleanup (and inactivate) conditionals, but stop when a try conditional
    // not in its finally clause (which then is to be executed next) is found.
    // In this case, make the ":finish" pending for execution at the ":endtry".
    // Otherwise, finish normally.
    idx = cleanup_conditionals(eap->cstack, 0, TRUE);
    ако (idx >= 0)
    {
	eap->cstack->cs_pending[idx] = CSTP_FINISH;
	report_make_pending(CSTP_FINISH, NULL);
    }
    иначе
	((source_cookie_T *)getline_cookie(eap->ea_getline,
					       eap->cookie))->finished = TRUE;
}


/*
 * Return TRUE when a sourced file had the ":finish" command: Don't give error
 * message for missing ":endif".
 * Return FALSE when not sourcing a file.
 */
    цяло
source_finished(
    char_u	*(*fgetline)(цяло, void *, цяло, getline_opt_T),
    void	*cookie)
{
    върни (getline_equal(fgetline, cookie, getsourceline)
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

    за (;;)
    {
	char_u *n = (char_u *)strstr((char *)p, "autoload");

	ако (n == NULL)
	    break;
	ако (n > p && vim_ispathsep(n[-1]) && vim_ispathsep(n[8]))
	    res = n + 9;
	p = n + 8;
    }
    върни res;
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

    ако (p == NULL)
	върни NULL;
    prefix = vim_strsave(p);
    ако (prefix == NULL)
	върни NULL;

    // replace all '/' with '#' and locate ".vim" at the end
    за (p = prefix; *p != NUL; p += mb_ptr2len(p))
    {
	ако (vim_ispathsep(*p))
	    *p = '#';
	иначе ако (STRCMP(p, ".vim") == 0)
	{
	    p[0] = '#';
	    p[1] = NUL;
	    върни prefix;
	}
    }

    // did not find ".vim" at the end
    vim_free(prefix);
    върни NULL;
}

/*
 * If in a Vim9 autoload script return "name" with the autoload prefix for the
 * script.  If successful the returned name is allocated.
 * Otherwise it returns "name" unmodified.
 */
    char_u *
may_prefix_autoload(char_u *name)
{
    ако (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	върни name;

    scriptitem_T *si = SCRIPT_ITEM(current_sctx.sc_sid);

    ако (si->sn_autoload_prefix == NULL)
	върни name;

    char_u  *basename = name;
    size_t  len;
    char_u  *res;

    ако (*name == K_SPECIAL)
    {
	char_u *p = vim_strchr(name, '_');

	// skip over "<SNR>99_"
	ако (p != NULL)
	    basename = p + 1;
    }

    len = STRLEN(si->sn_autoload_prefix) + STRLEN(basename) + 2;
    res = alloc(len);
    ако (res == NULL)
	върни NULL;

    vim_snprintf((char *)res, len, "%s%s", si->sn_autoload_prefix, basename);
    върни res;
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
    ако (scriptname == NULL)
	върни NULL;
    STRCPY(scriptname, "autoload/");
    STRCAT(scriptname, name[0] == 'g' && name[1] == ':' ? name + 2: name);
    за (p = scriptname + 9; (p = vim_strchr(p, AUTOLOAD_CHAR)) != NULL;
								    q = p, ++p)
	*p = '/';
    STRCPY(q, ".vim");
    върни scriptname;
}

/*
 * If "name" has a package name try autoloading the script for it.
 * Return TRUE if a package was loaded.
 */
    цяло
script_autoload(
    char_u	*name,
    цяло		reload)	    // load script again when already loaded
{
    char_u	*p;
    char_u	*scriptname, *tofree;
    цяло		ret = FALSE;
    цяло		i;
    цяло		ret_sid;

    // If the name starts with "<SNR>123_" then "123" is the script ID.
    ако (name[0] == K_SPECIAL && name[1] == KS_EXTRA && name[2] == KE_SNR)
    {
	p = name + 3;
	ret_sid = (цяло)getdigits(&p);
	ако (*p == '_' && SCRIPT_ID_VALID(ret_sid))
	{
	    may_load_script(ret_sid, &ret);
	    върни ret;
	}
    }

    // If there is no '#' after name[0] there is no package name.
    p = vim_strchr(name, AUTOLOAD_CHAR);
    ако (p == NULL || p == name)
	върни FALSE;

    tofree = scriptname = autoload_name(name);
    ако (scriptname == NULL)
	върни FALSE;

    // Find the name in the list of previously loaded package names.  Skip
    // "autoload/", it's always the same.
    за (i = 0; i < ga_loaded.ga_len; ++i)
	ако (STRCMP(((char_u **)ga_loaded.ga_data)[i] + 9, scriptname + 9) == 0)
	    break;
    ако (!reload && i < ga_loaded.ga_len)
	ret = FALSE;	    // was loaded already
    иначе
    {
	// Remember the name if it wasn't loaded already.
	ако (i == ga_loaded.ga_len && ga_grow(&ga_loaded, 1) == OK)
	{
	    ((char_u **)ga_loaded.ga_data)[ga_loaded.ga_len++] = scriptname;
	    tofree = NULL;
	}

	// Try loading the package from $VIMRUNTIME/autoload/<name>.vim
	// Use "ret_sid" to avoid loading the same script again.
	ако (source_in_path(p_rtp, scriptname, DIP_START, &ret_sid) == OK)
	    ret = TRUE;
    }

    vim_free(tofree);
    върни ret;
}
#endif
