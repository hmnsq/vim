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

#شامل "vim.h"

#اگر defined(FEAT_EVAL)
// The names of packages that once were loaded are remembered.
ساکن garray_T		ga_loaded = {0, 0, حجم(char_u *), 4, تہی};
#اگر_ختم

// last used sequence number for sourcing scripts (current_sctx.sc_seq)
#اگر_تعریف_شدہ FEAT_EVAL
ساکن صحیح		آخری_current_SID_seq = 0;
#اگر_ختم

ساکن صحیح do_ماخذ_ext(char_u *fname, صحیح check_other, صحیح is_vimrc, صحیح *ret_sid, exarg_T *eap, صحیح clearvars);

/*
 * Initialize the execution stack.
 */
    خالی
estack_init(خالی)
{
    estack_T *اندراج_2;

    اگر (ga_grow(&exestack, 10) == FAIL)
	mch_exit(0);
    اندراج_2 = ((estack_T *)exestack.ga_ڈیٹا) + exestack.ga_len;
    اندراج_2->es_type = ETYPE_چوٹی;
    اندراج_2->es_name = تہی;
    اندراج_2->es_lnum = 0;
#اگر_تعریف_شدہ FEAT_EVAL
    اندراج_2->es_info.ufunc = تہی;
#اگر_ختم
    ++exestack.ga_len;
}

/*
 * Add an item to the execution stack.
 * Returns the new entry or NULL when out of memory.
 */
    estack_T *
estack_ڈالو(etype_T type, char_u *name, طویل lnum)
{
    estack_T *اندراج_2;

    // If memory allocation fails then we'll pop more than we push, eventually
    // at the top level it will be OK again.
    اگر (ga_grow(&exestack, 1) == FAIL)
	واپس تہی;

    اندراج_2 = ((estack_T *)exestack.ga_ڈیٹا) + exestack.ga_len;
    اندراج_2->es_type = type;
    اندراج_2->es_name = name;
    اندراج_2->es_lnum = lnum;
#اگر_تعریف_شدہ FEAT_EVAL
    اندراج_2->es_info.ufunc = تہی;
#اگر_ختم
    ++exestack.ga_len;
    واپس اندراج_2;
}

#اگر defined(FEAT_EVAL)
/*
 * Add a user function to the execution stack.
 */
    estack_T *
estack_ڈالو_ufunc(ufunc_T *ufunc, طویل lnum)
{
    estack_T *اندراج_2 = estack_ڈالو(ETYPE_UFUNC,
	    ufunc->uf_name_exp != تہی
				  ? ufunc->uf_name_exp : ufunc->uf_name, lnum);
    اگر (اندراج_2 != تہی)
	اندراج_2->es_info.ufunc = ufunc;
    واپس اندراج_2;
}

/*
 * Return TRUE if "ufunc" with "lnum" is already at the top of the exe stack.
 */
    صحیح
estack_چوٹی_is_ufunc(ufunc_T *ufunc, طویل lnum)
{
    estack_T *اندراج_2;

    اگر (exestack.ga_len == 0)
	واپس FALSE;
    اندراج_2 = ((estack_T *)exestack.ga_ڈیٹا) + exestack.ga_len - 1;
    واپس اندراج_2->es_type == ETYPE_UFUNC
	&& STRCMP( اندراج_2->es_name, ufunc->uf_name_exp != تہی
				    ? ufunc->uf_name_exp : ufunc->uf_name) == 0
	&& اندراج_2->es_lnum == lnum;
}
#اگر_ختم

/*
 * Take an item off of the execution stack and return it.
 */
    estack_T *
estack_نکالو(خالی)
{
    اگر (exestack.ga_len == 0)
	واپس تہی;
    --exestack.ga_len;
    واپس ((estack_T *)exestack.ga_ڈیٹا) + exestack.ga_len;
}

/*
 * Get the current value for "which" in allocated memory.
 * "which" is ESTACK_SFILE for <sfile>, ESTACK_STACK for <stack> or
 * ESTACK_SCRIPT for <script>.
 */
    char_u *
estack_sfile(estack_arg_T which غیر_مستعمل)
{
    estack_T	*اندراج_2;
#اگر_تعریف_شدہ FEAT_EVAL
    garray_T	ga;
    size_t	len;
    صحیح		idx;
    etype_T	آخری_type = ETYPE_SCRIPT;
#اگر_ختم

    اندراج_2 = ((estack_T *)exestack.ga_ڈیٹا) + exestack.ga_len - 1;
#اگر_تعریف_شدہ FEAT_EVAL
    اگر (which == ESTACK_SFILE && اندراج_2->es_type != ETYPE_UFUNC)
#اگر_ختم
    {
	اگر (اندراج_2->es_name == تہی)
	    واپس تہی;
	واپس vim_strsave(اندراج_2->es_name);
    }
#اگر_تعریف_شدہ FEAT_EVAL
    // expand('<sfile>') works in a function for backwards compatibility, but
    // may give an unexpected result.  Disallow it in Vim 9 script.
    اگر (which == ESTACK_SFILE && اندر_vim9script())
    {
	صحیح  save_emsg_off = emsg_off;

	اگر (emsg_off == 1)
	    // f_expand() silences errors but we do want this one
	    emsg_off = 0;
	emsg(_(e_cannot_expand_sfile_اندر_vim9_function));
	emsg_off = save_emsg_off;
	واپس تہی;
    }

    // If evaluated in a function or autocommand, return the path of the script
    // where it is defined, at script level the current script path is returned
    // instead.
    اگر (which == ESTACK_SCRIPT)
    {
	// Walk the stack backwards, starting from the current frame.
	برائے (idx = exestack.ga_len - 1; idx >= 0; --idx, --اندراج_2)
	{
	    اگر (اندراج_2->es_type == ETYPE_UFUNC || اندراج_2->es_type == ETYPE_AUCMD)
	    {
		sctx_T *def_ctx = اندراج_2->es_type == ETYPE_UFUNC
			      ? &اندراج_2->es_info.ufunc->uf_script_ctx
			      : اندراج_2->es_info.aucmd != تہی
				  ? acp_script_ctx(اندراج_2->es_info.aucmd) : تہی;

		واپس def_ctx != تہی && def_ctx->sc_sid > 0
			   ? vim_strsave(SCRIPT_شے(def_ctx->sc_sid)->sn_name)
			   : تہی;
	    }
	    ورنہ اگر (اندراج_2->es_type == ETYPE_SCRIPT)
		واپس vim_strsave(اندراج_2->es_name);
	}
	واپس تہی;
    }

    // Give information about each stack entry up to the root.
    // For a function we compose the call stack, as it was done in the past:
    //   "function One[123]..Two[456]..Three"
    ga_init2(&ga, حجم(حرف), 100);
    برائے (idx = 0; idx < exestack.ga_len; ++idx)
    {
	اندراج_2 = ((estack_T *)exestack.ga_ڈیٹا) + idx;
	اگر (اندراج_2->es_name != تہی)
	{
	    طویل	lnum = 0;
	    size_t	added = 0;
	    string_T	type_name = {(char_u *)"", 0};
	    string_T	class_name = {(char_u *)"", 0};
	    string_T	es_name = {اندراج_2->es_name, STRLEN(اندراج_2->es_name)};

	    اگر (اندراج_2->es_type != آخری_type)
	    {
		انتخاب (اندراج_2->es_type)
		{
		    حالت ETYPE_SCRIPT:
			STR_LITERAL_سیٹ(type_name, "script ");
			توڑو;
		    حالت ETYPE_UFUNC:
			STR_LITERAL_سیٹ(type_name, "function ");
			توڑو;
		    طےشدہ:
			توڑو;
		}
		آخری_type = اندراج_2->es_type;
	    }
	    اگر (اندراج_2->es_type == ETYPE_UFUNC && اندراج_2->es_info.ufunc->uf_class != تہی)
	    {
		class_name.string = اندراج_2->es_info.ufunc->uf_class->class_name.string;
		class_name.لمبائی = اندراج_2->es_info.ufunc->uf_class->class_name.لمبائی;
	    }
	    اگر (idx == exestack.ga_len - 1)
		lnum = which == ESTACK_ڈھیر ? SOURCING_LNUM : 0;
	    ورنہ
		lnum = اندراج_2->es_lnum;

	    len = es_name.لمبائی + type_name.لمبائی + class_name.لمبائی + 26;
	    اگر (ga_grow(&ga, (صحیح)len) == FAIL)
		توڑو;
	    ga_concat_len(&ga, type_name.string, type_name.لمبائی);
	    // For class methods prepend "<class name>." to the function name.
	    اگر (*class_name.string != NUL)
	    {
		added = vim_snprintf_safelen(
		    (حرف *)ga.ga_ڈیٹا + ga.ga_len,
		    ga.ga_maxlen - ga.ga_len,
		    "<SNR>%d_%s.",
		    اندراج_2->es_info.ufunc->uf_script_ctx.sc_sid,
		    class_name.string);

		ga.ga_len += (صحیح)added;
	    }
	    ga_concat_len(&ga, es_name.string, es_name.لمبائی);
	    // For the bottom entry of <sfile>: do not add the line number, it is used in
	    // <slnum>.  Also leave it out when the number is not set.
	    اگر (lnum != 0)
	    {
		added = vim_snprintf_safelen(
		    (حرف *)ga.ga_ڈیٹا + ga.ga_len,
		    ga.ga_maxlen - ga.ga_len,
		    "[%ld]",
		    lnum);

		ga.ga_len += (صحیح)added;
	    }
	    اگر (idx != exestack.ga_len - 1)
		GA_CONCAT_LITERAL(&ga, "..");
	}
    }

    // Only NUL-terminate when not returning NULL.
    اگر (ga.ga_ڈیٹا != تہی)
	ga_append(&ga, NUL);
    واپس (char_u *)ga.ga_ڈیٹا;
#اگر_ختم
}

#اگر_تعریف_شدہ FEAT_EVAL
    ساکن خالی
stacktrace_ڈالو_شے(
	list_T		*l,
	ufunc_T		*fp,
	char_u		*event,
	linenr_T	lnum,
	char_u		*filepath)
{
    dict_T	*d;
    typval_T	tv;

    d = dict_alloc_تالا(VAR_FIXED);
    اگر (d == تہی)
	واپس;

    tv.v_type = VAR_DICT;
    tv.v_تالا = VAR_LOCKED;
    tv.vval.v_dict = d;

    اگر (fp != تہی)
	dict_جوڑو_func(d, "funcref", fp);
    اگر (event != تہی)
	dict_جوڑو_string(d, "event", event);
    dict_جوڑو_نمبر(d, "lnum", lnum);
    dict_جوڑو_string(d, "filepath", filepath);

    list_append_tv(l, &tv);
}

/*
 * Create the stacktrace from exestack.
 */
    list_T *
stacktrace_create(خالی)
{
    list_T	*l;
    صحیح		i;

    l = list_alloc();
    اگر (l == تہی)
	واپس تہی;

    برائے (i = 0; i < exestack.ga_len; ++i)
    {
	estack_T *اندراج_2 = &((estack_T *)exestack.ga_ڈیٹا)[i];
	linenr_T lnum = اندراج_2->es_lnum;

	اگر (اندراج_2->es_type == ETYPE_SCRIPT)
	    stacktrace_ڈالو_شے(l, تہی, تہی, lnum, اندراج_2->es_name);
	ورنہ اگر (اندراج_2->es_type == ETYPE_UFUNC)
	{
	    ufunc_T *fp = اندراج_2->es_info.ufunc;
	    sctx_T sctx = fp->uf_script_ctx;
	    char_u *filepath = sctx.sc_sid > 0 ?
				   get_scriptname(sctx.sc_sid) : (char_u *)"";

	    lnum += sctx.sc_lnum;
	    stacktrace_ڈالو_شے(l, fp, تہی, lnum, filepath);
	}
	ورنہ اگر (اندراج_2->es_type == ETYPE_AUCMD)
	{
	    // The autocmd may not have a matching pattern yet, in which case
	    // es_info.aucmd is still NULL.
	    sctx_T *sctx = اندراج_2->es_info.aucmd != تہی
			       ? acp_script_ctx(اندراج_2->es_info.aucmd) : تہی;
	    char_u *filepath = sctx != تہی && sctx->sc_sid > 0 ?
				   get_scriptname(sctx->sc_sid) : (char_u *)"";

	    اگر (sctx != تہی)
		lnum += sctx->sc_lnum;
	    stacktrace_ڈالو_شے(l, تہی, اندراج_2->es_name, lnum, filepath);
	}
    }
    واپس l;
}

/*
 * getstacktrace() function
 */
    خالی
f_getstacktrace(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    rettv_list_سیٹ(rettv, stacktrace_create());
}
#اگر_ختم

/*
 * Get DIP_ flags from the [where] argument of a :runtime command.
 * "*argp" is advanced to after the [where] argument if it is found.
 */
    ساکن صحیح
get_runtime_cmd_flags(char_u **argp, size_t where_len)
{
    char_u *arg = *argp;

    اگر (where_len == 0)
	واپس 0;

    اگر (STRNCMP(arg, "START", where_len) == 0)
    {
	*argp = skipwhite(arg + where_len);
	واپس DIP_آغاز + DIP_NORTP;
    }
    اگر (STRNCMP(arg, "OPT", where_len) == 0)
    {
	*argp = skipwhite(arg + where_len);
	واپس DIP_OPT + DIP_NORTP;
    }
    اگر (STRNCMP(arg, "PACK", where_len) == 0)
    {
	*argp = skipwhite(arg + where_len);
	واپس DIP_آغاز + DIP_OPT + DIP_NORTP;
    }
    اگر (STRNCMP(arg, "ALL", where_len) == 0)
    {
	*argp = skipwhite(arg + where_len);
	واپس DIP_آغاز + DIP_OPT;
    }

    واپس 0;
}

/*
 * ":runtime [where] {name}"
 */
    خالی
ex_runtime(exarg_T *eap)
{
    char_u  *arg = eap->arg;
    صحیح	    flags = eap->forceit ? DIP_ALL : 0;
    char_u  *p = skiptowhite(arg);
    flags += get_runtime_cmd_flags(&arg, p - arg);
    ماخذ_runtime(arg, flags);
}

ساکن صحیح runtime_expand_flags;

/*
 * Set the completion context for the :runtime command.
 */
    خالی
سیٹ_context_اندر_runtime_cmd(expand_T *xp, char_u *arg)
{
    char_u  *p = skiptowhite(arg);
    runtime_expand_flags
	= *p != NUL ? get_runtime_cmd_flags(&arg, p - arg) : 0;
    // Skip to the last argument.
    جب (*(p = skiptowhite_esc(arg)) != NUL)
    {
	اگر (runtime_expand_flags == 0)
	    // When there are multiple arguments and [where] is not specified,
	    // use an unrelated non-zero flag to avoid expanding [where].
	    runtime_expand_flags = DIP_ALL;
	arg = skipwhite(p);
    }
    xp->xp_context = EXPAND_RUNTIME;
    xp->xp_نمونہ = arg;
}

    ساکن خالی
ماخذ_callback(char_u *fname, خالی *cookie)
{
    (خالی)do_ماخذ(fname, FALSE, DOSO_NONE, cookie);
}

#اگر_تعریف_شدہ FEAT_EVAL
/*
 * Find an already loaded script "name".
 * If found returns its script ID.  If not found returns -1.
 */
    صحیح
ڈھونڈو_script_by_name(char_u *name)
{
    صحیح		    sid;
    scriptitem_T    *si;

    برائے (sid = script_اشیاء.ga_len; sid > 0; --sid)
    {
	// We used to check inode here, but that doesn't work:
	// - If a script is edited and written, it may get a different
	//   inode number, even though to the user it is the same script.
	// - If a script is deleted and another script is written, with a
	//   different name, the inode may be re-used.
	si = SCRIPT_شے(sid);
	اگر (si->sn_name != تہی && fnamecmp(si->sn_name, name) == 0)
	    واپس sid;
    }
    واپس -1;
}

/*
 * Add a new scriptitem with all items initialized.
 * When running out of memory "error" is set to FAIL.
 * Returns the script ID.
 */
    ساکن صحیح
get_new_scriptitem(صحیح *error)
{
    ساکن scid_T   آخری_current_SID = 0;
    صحیح		    sid = ++آخری_current_SID;
    scriptitem_T    *si = تہی;

    اگر (ga_grow(&script_اشیاء, (صحیح)(sid - script_اشیاء.ga_len)) == FAIL)
    {
	*error = FAIL;
	واپس sid;
    }
    جب (script_اشیاء.ga_len < sid)
    {
	si = ALLOC_CLEAR_ONE(scriptitem_T);
	اگر (si == تہی)
	{
	    *error = FAIL;
	    واپس sid;
	}
	++script_اشیاء.ga_len;
	SCRIPT_شے(script_اشیاء.ga_len) = si;
	si->sn_name = تہی;
	si->sn_نسخہ = 1;

	// Allocate the local script variables to use for this script.
	new_script_vars(script_اشیاء.ga_len);
	ga_init2(&si->sn_var_vals, حجم(svar_T), 10);
	hash_init(&si->sn_all_vars.dv_hashtab);
	ga_init2(&si->sn_imports, حجم(imported_T), 10);
	ga_init2(&si->sn_type_list, حجم(type_T), 10);
# ifdef FEAT_PROFILE
	si->sn_prof_on = FALSE;
# endif
    }

    // "si" can't be NULL, check only to avoid a compiler warning
    اگر (si != تہی)
	// Used to check script variable index is still valid.
	si->sn_script_seq = current_sctx.sc_seq;

    واپس sid;
}

    صحیح
get_new_scriptitem_for_fname(صحیح *error, char_u *fname)
{
    صحیح sid = get_new_scriptitem(error);

    اگر (*error == OK)
    {
	scriptitem_T *si = SCRIPT_شے(sid);

	si->sn_name = vim_strsave(fname);
	si->sn_state = SN_STATE_NOT_LOADED;
    }
    واپس sid;
}

/*
 * If the script for "sid" is a symlink and "sn_source_sid" is not set
 * then initialize it. A new script_item is created if needed.
 */
    خالی
check_script_symlink(صحیح sid)
{
    scriptitem_T *si = SCRIPT_شے(sid);
    اگر (si->sn_syml_checked || si->sn_sourced_sid > 0)
	واپس;
    si->sn_syml_checked = TRUE;

    // If fname is a symbolic link, create an script_item for the real file.

    char_u *real_fname = fix_fname(si->sn_name);
    اگر (real_fname != تہی && STRCMP(real_fname, si->sn_name) != 0)
    {
	صحیح real_sid = ڈھونڈو_script_by_name(real_fname);
	صحیح error2 = OK;
	صحیح new_sid = FALSE;
	اگر (real_sid < 0)
	{
	    real_sid = get_new_scriptitem_for_fname(&error2, real_fname);
	    new_sid = TRUE;
	}
	اگر (error2 == OK)
	{
	    si = SCRIPT_شے(sid);
	    si->sn_sourced_sid = real_sid;
	    اگر (new_sid)
	    {
		SCRIPT_شے(real_sid)->sn_import_autoload
						    = si->sn_import_autoload;
		اگر (si->sn_autoload_سابقہ != تہی)
		    SCRIPT_شے(real_sid)->sn_autoload_سابقہ =
					vim_strsave(si->sn_autoload_سابقہ);
	    }
	}
    }
    vim_free(real_fname);
}

    ساکن خالی
ڈھونڈو_script_callback(char_u *fname, خالی *cookie)
{
    صحیح sid;
    صحیح error = OK;
    صحیح *ret_sid = cookie;

    sid = ڈھونڈو_script_by_name(fname);
    اگر (sid < 0)
	// script does not exist yet, create a new scriptitem
	sid = get_new_scriptitem_for_fname(&error, fname);
    *ret_sid = sid;
}
#اگر_ختم

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
    صحیح
do_اندر_راستہ(
    char_u	*راستہ,
    حرف	*سابقہ,
    char_u	*name,
    صحیح		flags,
    خالی	(*callback)(char_u *fname, خالی *ck),
    خالی	*cookie)
{
    char_u	*rtp;
    string_T	buf;
    char_u	*rtp_copy;
    صحیح		عدد_files;
    char_u	**files;
    صحیح		i;
    صحیح		did_one = FALSE;
#اگر_تعریف_شدہ AMIGA
    ساخت Process	*proc = (ساخت Process *)ڈھونڈو_Task(0L);
    APTR		save_winptr = proc->pr_کھڑکی_Ptr;

    // Avoid a requester here for a volume that doesn't exist.
    proc->pr_کھڑکی_Ptr = (APTR)-1L;
#اگر_ختم

    // Make a copy of 'runtimepath'.  Invoking the callback may change the
    // value.
    rtp_copy = vim_strsave(راستہ);
    buf.string = alloc(MAXPATHL);
    اگر (buf.string != تہی && rtp_copy != تہی)
    {
	size_t	prefixlen = 0;
	size_t	namelen = 0;

	اگر (name != تہی)
	{
	    prefixlen = STRLEN(سابقہ);
	    namelen = STRLEN(name);

	    اگر (p_verbose > 10)
	    {
		verbose_enter();
		اگر (*سابقہ != NUL)
		    smsg(_("Searching for \"%s\" under \"%s\" in \"%s\""),
					       (حرف *)name, سابقہ, (حرف *)راستہ);
		ورنہ
		    smsg(_("Searching for \"%s\" in \"%s\""),
						       (حرف *)name, (حرف *)راستہ);
		verbose_leave();
	    }
	}

	// Loop over all entries in 'runtimepath'.
	rtp = rtp_copy;
	جب (*rtp != NUL && ((flags & DIP_ALL) || !did_one))
	{
	    // Copy the path from 'runtimepath' to buf[].
	    buf.لمبائی = (size_t)copy_option_part(&rtp, buf.string, MAXPATHL, ",");

	    // Skip after or non-after directories.
	    اگر (flags & (DIP_NOAFTER | DIP_AFTER))
	    {
		صحیح is_after = buf.لمبائی >= 5
		    && STRCMP(buf.string + buf.لمبائی - 5, "after") == 0;

		اگر ((is_after && (flags & DIP_NOAFTER))
			|| (!is_after && (flags & DIP_AFTER)))
		    جاری;
	    }

	    اگر (name == تہی)
	    {
		(*callback)(buf.string, (خالی *)&cookie);
		اگر (!did_one)
		    did_one = (cookie == تہی);
	    }
	    ورنہ اگر (buf.لمبائی + 2 + prefixlen + namelen < MAXPATHL)
	    {
		char_u	*np;
		char_u	*دم;

		اگر (*buf.string != NUL
		    && !after_pathsep(buf.string, buf.string + buf.لمبائی))
		{
		    STRCPY(buf.string + buf.لمبائی, PATHSEPSTR);
		    buf.لمبائی += حجم(PATHSEP);
		}
		STRCPY(buf.string + buf.لمبائی, سابقہ);
		buf.لمبائی += prefixlen;
		دم = buf.string + buf.لمبائی;

		// Loop over all patterns in "name"
		np = name;
		جب (*np != NUL && ((flags & DIP_ALL) || !did_one))
		{
		    // Append the pattern from "name" to buf[].
		    copy_option_part(&np, دم, (صحیح)(MAXPATHL - (دم - buf.string)),
								       "\t ");

		    اگر (p_verbose > 10)
		    {
			verbose_enter();
			smsg(_("Searching for \"%s\""), buf.string);
			verbose_leave();
		    }

		    // Expand wildcards, invoke the callback for each match.
		    اگر (gen_expand_wildcards(1, &buf.string, &عدد_files, &files,
				  (flags & DIP_DIR) ? EW_DIR : EW_FILE) == OK)
		    {
			برائے (i = 0; i < عدد_files; ++i)
			{
			    (*callback)(files[i], cookie);
			    did_one = TRUE;
			    اگر (!(flags & DIP_ALL))
				توڑو;
			}
			FreeWild(عدد_files, files);
		    }
		}
	    }
	}
    }
    vim_free(buf.string);
    vim_free(rtp_copy);
    اگر (!did_one && name != تہی)
    {
	حرف *basepath = راستہ == p_rtp ? "runtimepath" : "packpath";

	اگر (flags & DIP_ERR)
	    semsg(_(e_directory_not_ملا_اندر_str_str), basepath, name);
	ورنہ اگر (p_verbose > 0)
	{
	    verbose_enter();
	    smsg(_("not found in '%s': \"%s\""), basepath, name);
	    verbose_leave();
	}
    }

#اگر_تعریف_شدہ AMIGA
    proc->pr_کھڑکی_Ptr = save_winptr;
#اگر_ختم

    واپس did_one ? OK : FAIL;
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
    ساکن صحیح
do_اندر_راستہ_and_pp(
    char_u	*راستہ,
    char_u	*name,
    صحیح		flags,
    خالی	(*callback)(char_u *fname, خالی *ck),
    خالی	*cookie)
{
    صحیح		مکمل = FAIL;

    اگر ((flags & DIP_NORTP) == 0)
	مکمل = do_اندر_راستہ(راستہ, "", name, flags, callback, cookie);

    اگر ((مکمل == FAIL || (flags & DIP_ALL)) && (flags & DIP_آغاز))
	مکمل = do_اندر_راستہ(p_pp, "pack/*/start/*/", name, flags, callback,
								       cookie);

    اگر ((مکمل == FAIL || (flags & DIP_ALL)) && (flags & DIP_OPT))
	مکمل = do_اندر_راستہ(p_pp, "pack/*/opt/*/", name, flags, callback,
								       cookie);

    واپس مکمل;
}

/*
 * Just like do_in_path_and_pp(), using 'runtimepath' for "path".
 */
    صحیح
do_اندر_runtimepath(
    char_u	*name,
    صحیح		flags,
    خالی	(*callback)(char_u *fname, خالی *ck),
    خالی	*cookie)
{
    واپس do_اندر_راستہ_and_pp(p_rtp, name, flags, callback, cookie);
}

/*
 * Source the file "name" from all directories in 'runtimepath'.
 * "name" can contain wildcards.
 * When "flags" has DIP_ALL: source all files, otherwise only the first one.
 *
 * return FAIL when no file could be sourced, OK otherwise.
 */
    صحیح
ماخذ_runtime(char_u *name, صحیح flags)
{
    واپس ماخذ_اندر_راستہ(p_rtp, name, flags, تہی);
}

/*
 * Just like source_runtime(), but use "path" instead of 'runtimepath'
 * and return the script ID in "ret_sid".
 */
    صحیح
ماخذ_اندر_راستہ(char_u *راستہ, char_u *name, صحیح flags, صحیح *ret_sid)
{
    واپس do_اندر_راستہ_and_pp(راستہ, name, flags, ماخذ_callback, ret_sid);
}

#اگر defined(FEAT_EVAL)

/*
 * Find "name" in 'runtimepath'. If found a new scriptitem is created for it
 * and it's script ID is returned.
 * If not found returns -1.
 */
    صحیح
ڈھونڈو_script_اندر_rtp(char_u *name)
{
    صحیح sid = -1;

    (خالی)do_اندر_راستہ_and_pp(p_rtp, name, DIP_آغاز | DIP_NOAFTER,
						   ڈھونڈو_script_callback, &sid);
    واپس sid;
}

/*
 * Expand wildcards in "pat" and invoke do_source() for each match.
 */
    ساکن خالی
ماخذ_all_matches(char_u *pat)
{
    صحیح	    عدد_files;
    char_u  **files;
    صحیح	    i;

    اگر (gen_expand_wildcards(1, &pat, &عدد_files, &files, EW_FILE) != OK)
	واپس;

    برائے (i = 0; i < عدد_files; ++i)
	(خالی)do_ماخذ(files[i], FALSE, DOSO_NONE, تہی);
    FreeWild(عدد_files, files);
}

/*
 * Add the package directory to 'runtimepath'.
 */
    ساکن صحیح
جوڑو_pack_dir_مقصد_rtp(char_u *fname)
{
    char_u  *p4, *p3, *p2, *p1, *p;
    char_u  *اندراج_2;
    char_u  *insp = تہی;
    صحیح	    c;
    string_T  new_rtp;
    size_t  keep;
    size_t  p_rtp_len;
    string_T  afterdir = {تہی, 0};
    char_u  *after_insp = تہی;
    char_u  *ffname = تہی;
    size_t  fname_len;
    string_T	buf = {تہی, 0};
    صحیح	    retval = FAIL;

    p4 = p3 = p2 = p1 = get_past_سر(fname);
    برائے (p = p1; *p; MB_PTR_ADV(p))
	اگر (vim_ispathsep_nocolon(*p))
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
    اگر (ffname == تہی)
	واپس FAIL;

    // Find "ffname" in "p_rtp", ignoring '/' vs '\' differences.
    // Also stop at the first "after" directory.
    fname_len = STRLEN(ffname);
    buf.string = alloc(MAXPATHL);
    اگر (buf.string == تہی)
	جاؤ theend;

    p_rtp_len = 0;
    برائے (اندراج_2 = p_rtp; *اندراج_2 != NUL; )
    {
	char_u *cur_اندراج = اندراج_2;

	buf.لمبائی = (size_t)copy_option_part(&اندراج_2, buf.string, MAXPATHL, ",");

	// keep track of p_rtp length as we go to make the STRLEN() below have less work to do
	p_rtp_len += (*(cur_اندراج + buf.لمبائی) == ',') ? buf.لمبائی + 1 : buf.لمبائی;

	اگر ((p = (char_u *)لڑی_تلاش((حرف *)buf.string, "after")) != تہی
		&& p > buf.string
		&& vim_ispathsep(p[-1])
		&& (vim_ispathsep(p[5]) || p[5] == NUL || p[5] == ','))
	{
	    اگر (insp == تہی)
		// Did not find "ffname" before the first "after" directory,
		// insert it before this entry.
		insp = cur_اندراج;
	    after_insp = cur_اندراج;
	    توڑو;
	}

	اگر (insp == تہی)
	{
	    char_u  *rtp_ffname;
	    صحیح	    مطابقت;

	    اگر (*buf.string != NUL && !after_pathsep(buf.string, buf.string + buf.لمبائی))
		STRCPY(buf.string + buf.لمبائی, PATHSEPSTR);
	    rtp_ffname = fix_fname(buf.string);
	    اگر (rtp_ffname == تہی)
		جاؤ theend;
	    مطابقت = vim_fnamencmp(rtp_ffname, ffname, fname_len) == 0;
	    vim_free(rtp_ffname);
	    اگر (مطابقت)
		// Insert "ffname" after this entry (and comma).
		insp = اندراج_2;
	}
    }

    // finish measuring the length of p_rtp
    p_rtp_len += STRLEN(p_rtp + p_rtp_len);
    اگر (insp == تہی)
	// Both "fname" and "after" not found, append at the end.
	insp = p_rtp + p_rtp_len;

    // check if rtp/pack/name/start/name/after exists
    fname_len = STRLEN(fname);
    concat_fnames(fname, fname_len, (char_u *)"after", STRLEN_LITERAL("after"), TRUE, &afterdir);
    اگر (afterdir.string == تہی || !mch_isdir(afterdir.string))
	afterdir.لمبائی = 0;

    new_rtp.string = alloc(p_rtp_len + fname_len + afterdir.لمبائی + 3); // add two for commas and one for NUL
    اگر (new_rtp.string == تہی)
	جاؤ theend;

    // We now have 'rtp' parts: {keep}{keep_after}{rest}.
    // Create new_rtp, first: {keep},{fname}
    keep = (size_t)(insp - p_rtp);
    mch_memmove(new_rtp.string, p_rtp, keep);
    new_rtp.لمبائی = keep;
    اگر (*insp == NUL)
	new_rtp.string[new_rtp.لمبائی++] = ',';  // add comma before
    mch_memmove(new_rtp.string + new_rtp.لمبائی, fname, fname_len);
    new_rtp.لمبائی += fname_len;
    اگر (*insp != NUL)
	new_rtp.string[new_rtp.لمبائی++] = ',';  // add comma after

    اگر (afterdir.لمبائی > 0 && after_insp != تہی)
    {
	size_t keep_after = (size_t)(after_insp - p_rtp);
	size_t append_len = keep_after - keep;

	// Add to new_rtp: {keep},{fname}{keep_after},{afterdir}
	mch_memmove(new_rtp.string + new_rtp.لمبائی, p_rtp + keep,
							append_len);
	new_rtp.لمبائی += append_len;
	mch_memmove(new_rtp.string + new_rtp.لمبائی, afterdir.string, afterdir.لمبائی);
	new_rtp.لمبائی += afterdir.لمبائی;
	new_rtp.string[new_rtp.لمبائی++] = ',';
	keep = keep_after;
    }

    اگر (p_rtp[keep] != NUL)
    {
	size_t	append_len = p_rtp_len - keep;

	// Append rest: {keep},{fname}{keep_after},{afterdir}{rest}
	mch_memmove(new_rtp.string + new_rtp.لمبائی, p_rtp + keep, append_len + 1);	// add one for NUL
	new_rtp.لمبائی += append_len;
    }
    ورنہ
	new_rtp.string[new_rtp.لمبائی] = NUL;

    اگر (afterdir.لمبائی > 0 && after_insp == تہی)
    {
	// Append afterdir when "after" was not found:
	// {keep},{fname}{rest},{afterdir}
	new_rtp.string[new_rtp.لمبائی++] = ',';
	STRCPY(new_rtp.string + new_rtp.لمبائی, afterdir.string);
    }

    سیٹ_option_قدر_give_err((char_u *)"rtp", 0L, new_rtp.string, 0);
    vim_free(new_rtp.string);
    retval = OK;

theend:
    vim_free(buf.string);
    vim_free(ffname);
    vim_free(afterdir.string);
    واپس retval;
}

/*
 * Load scripts in "plugin" and "ftdetect" directories of the package.
 */
    ساکن صحیح
بھرو_pack_plugin(char_u *fname)
{
    ساکن حرف *plugpat = "%s/plugin/**/*.vim";
    ساکن حرف *ftpat = "%s/ftdetect/*.vim";
    صحیح		len;
    char_u	*ffname = fix_fname(fname);
    char_u	*pat = تہی;
    صحیح		retval = FAIL;

    اگر (ffname == تہی)
	واپس FAIL;
    len = (صحیح)STRLEN(ffname) + (صحیح)STRLEN(ftpat);
    pat = alloc(len);
    اگر (pat == تہی)
	جاؤ theend;
    vim_snprintf((حرف *)pat, len, plugpat, ffname);
    ماخذ_all_matches(pat);

    {
	char_u *cmd = vim_strsave((char_u *)"g:did_load_filetypes");

	// If runtime/filetype.vim wasn't loaded yet, the scripts will be
	// found when it loads.
	اگر (cmd != تہی && eval_مقصد_نمبر(cmd, FALSE) > 0)
	{
	    do_cmdline_cmd((char_u *)"augroup filetypedetect");
	    vim_snprintf((حرف *)pat, len, ftpat, ffname);
	    ماخذ_all_matches(pat);
	    do_cmdline_cmd((char_u *)"augroup END");
	}
	vim_free(cmd);
    }
    vim_free(pat);
    retval = OK;

theend:
    vim_free(ffname);
    واپس retval;
}

// used for "cookie" of add_pack_plugin()
ساکن صحیح APP_جوڑو_DIR;
ساکن صحیح APP_بھرو;
ساکن صحیح APP_BOTH;

    ساکن خالی
جوڑو_pack_plugin(char_u *fname, خالی *cookie)
{
    اگر (cookie != &APP_بھرو)
    {
	char_u	*buf = alloc(MAXPATHL);
	char_u	*p;
	صحیح	ملا = FALSE;

	اگر (buf == تہی)
	    واپس;
	p = p_rtp;
	جب (*p != NUL)
	{
	    copy_option_part(&p, buf, MAXPATHL, ",");
	    اگر (pathcmp((حرف *)buf, (حرف *)fname, -1) == 0)
	    {
		ملا = TRUE;
		توڑو;
	    }
	}
	vim_free(buf);
	اگر (!ملا)
	    // directory is not yet in 'runtimepath', add it
	    اگر (جوڑو_pack_dir_مقصد_rtp(fname) == FAIL)
		واپس;
    }

    اگر (cookie != &APP_جوڑو_DIR)
	بھرو_pack_plugin(fname);
}

/*
 * Add all packages in the "start" directory to 'runtimepath'.
 */
    خالی
جوڑو_pack_آغاز_dirs(خالی)
{
    do_اندر_راستہ(p_pp, "", (char_u *)"pack/*/start/*", DIP_ALL + DIP_DIR,
					       جوڑو_pack_plugin, &APP_جوڑو_DIR);
}

/*
 * Load plugins from all packages in the "start" directory.
 */
    خالی
بھرو_آغاز_packages(خالی)
{
    did_ماخذ_packages = TRUE;
    do_اندر_راستہ(p_pp, "", (char_u *)"pack/*/start/*", DIP_ALL + DIP_DIR,
						  جوڑو_pack_plugin, &APP_بھرو);
}

/*
 * ":packloadall"
 * Find plugins in the package directories and source them.
 */
    خالی
ex_packloadall(exarg_T *eap)
{
    اگر (!did_ماخذ_packages || eap->forceit)
    {
	// First do a round to add all directories to 'runtimepath', then load
	// the plugins. This allows for plugins to use an autoload directory
	// of another plugin.
	جوڑو_pack_آغاز_dirs();
	بھرو_آغاز_packages();
    }
}

/*
 * ":packadd[!] {name}"
 */
    خالی
ex_packadd(exarg_T *eap)
{
    ساکن حرف *plugpat = "pack/*/%s/%s";
    صحیح		len;
    حرف	*pat;
    صحیح		باری;
    صحیح		res = OK;

    // Round 1: use "start", round 2: use "opt".
    برائے (باری = 1; باری <= 2; ++باری)
    {
	// Only look under "start" when loading packages wasn't done yet.
	اگر (باری == 1 && did_ماخذ_packages)
	    جاری;

	len = (صحیح)STRLEN(plugpat) + (صحیح)STRLEN(eap->arg) + 5;
	pat = alloc(len);
	اگر (pat == تہی)
	    واپس;
	vim_snprintf(pat, len, plugpat, باری == 1 ? "start" : "opt", eap->arg);
	// The first round don't give a "not found" error, in the second round
	// only when nothing was found in the first round.
	res = do_اندر_راستہ(p_pp, "", (char_u *)pat,
		DIP_ALL + DIP_DIR + (باری == 2 && res == FAIL ? DIP_ERR : 0),
		جوڑو_pack_plugin, eap->forceit ? &APP_جوڑو_DIR : &APP_BOTH);
	vim_free(pat);
    }
}
#اگر_ختم

/*
 * Sort "gap" and remove duplicate entries.  "gap" is expected to contain a
 * list of file names in allocated memory.
 */
    خالی
remove_duplicates(garray_T *gap)
{
    صحیح	    i;
    صحیح	    j;
    char_u  **fnames = (char_u **)gap->ga_ڈیٹا;

    ترتیب_دو_لڑیاں(fnames, gap->ga_len);
    برائے (i = gap->ga_len - 1; i > 0; --i)
	اگر (fnamecmp(fnames[i - 1], fnames[i]) == 0)
	{
	    vim_free(fnames[i]);
	    برائے (j = i + 1; j < gap->ga_len; ++j)
		fnames[j - 1] = fnames[j];
	    --gap->ga_len;
	}
}

    ساکن خالی
ExpandRTDir_int(
    char_u	*pat,
    size_t	pat_len,
    صحیح		flags,
    صحیح		keep_ext,
    garray_T	*gap,
    حرف	*dirnames[])
{
    برائے (صحیح i = 0; dirnames[i] != تہی; ++i)
    {
	ثابت size_t	buf_len = STRLEN(dirnames[i]) + pat_len + 64;
	حرف		*buf = alloc(buf_len);
	اگر (buf == تہی)
	{
	    ga_clear_لڑیاں(gap);
	    واپس;
	}
	صحیح		glob_flags = 0;
	صحیح		expand_dirs = FALSE;

	// Build base pattern
	vim_snprintf(buf, buf_len, "%s%s%s%s",
		     *dirnames[i] ? dirnames[i] : "", *dirnames[i] ? "/" : "",
		     pat, "*.vim");

expand:
	اگر ((flags & DIP_NORTP) == 0)
	    globpath(p_rtp, (char_u *)buf, gap, glob_flags, expand_dirs);

	اگر (flags & DIP_آغاز)
	{
	    // Build complete search path: pack/*/start/*/dirnames[i]/pat*.vim
	    vim_snprintf(buf, buf_len, "pack/*/start/*/%s%s%s%s",
			 *dirnames[i] ? dirnames[i] : "",
			 *dirnames[i] ? "/" : "",
			 pat,
			 expand_dirs ? "*" : "*.vim");
	    globpath(p_pp, (char_u *)buf, gap, glob_flags, expand_dirs);
	}

	اگر (flags & DIP_OPT)
	{
	    // Build complete search path: pack/*/opt/*/dirnames[i]/pat*.vim
	    vim_snprintf(buf, buf_len, "pack/*/opt/*/%s%s%s%s",
			 *dirnames[i] ? dirnames[i] : "",
			 *dirnames[i] ? "/" : "", pat,
			 expand_dirs ? "*" : "*.vim");
	    globpath(p_pp, (char_u *)buf, gap, glob_flags, expand_dirs);
	}

	// Second round for directories
	اگر (*dirnames[i] == NUL && !expand_dirs)
	{
	    // expand dir names in another round
	    vim_snprintf(buf, buf_len, "%s*", pat);
	    glob_flags = WILD_جوڑو_SLASH;
	    expand_dirs = TRUE;
	    جاؤ expand;
	}

	vim_free(buf);
    }

    صحیح pat_pathsep_cnt = 0;
    برائے (size_t i = 0; i < pat_len; ++i)
    {
	اگر (vim_ispathsep(pat[i]))
	    ++pat_pathsep_cnt;
    }

    برائے (صحیح i = 0; i < gap->ga_len; ++i)
    {
	char_u *مطابقت = ((char_u **)gap->ga_ڈیٹا)[i];
	char_u *s = مطابقت;
	char_u *e = s + STRLEN(s);
	اگر (e - 4 > s && !keep_ext && STRNICMP(e - 4, ".vim", 4) == 0)
	{
	    e -= 4;
	    *e = NUL;
	}

	صحیح مطابقت_pathsep_cnt = (e > s && e[-1] == '/') ? -1 : 0;
	برائے (s = e; s > مطابقت; MB_PTR_عقب(مطابقت, s))
	{
	    اگر (s < مطابقت || (vim_ispathsep(*s)
				     && ++مطابقت_pathsep_cnt > pat_pathsep_cnt))
		توڑو;
	}
	++s;
	اگر (s != مطابقت)
	    mch_memmove(مطابقت, s, e - s + 1);
    }

    اگر (gap->ga_len == 0)
	واپس;

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
    صحیح
ExpandRTDir(
    char_u	*pat,
    صحیح		flags,
    صحیح		*عدد_file,
    char_u	***file,
    حرف	*dirnames[])
{
    *عدد_file = 0;
    *file = تہی;

    garray_T	ga;
    ga_init2(&ga, حجم(حرف *), 10);

    ExpandRTDir_int(pat, STRLEN(pat), flags, FALSE, &ga, dirnames);

    اگر (ga.ga_len == 0)
	واپس FAIL;

    *file = ga.ga_ڈیٹا;
    *عدد_file = ga.ga_len;
    واپس OK;
}

/*
 * Handle command line completion for :runtime command.
 */
    صحیح
expand_runtime_cmd(char_u *pat, صحیح *عدد_Matches, char_u ***matches)
{
    *عدد_Matches = 0;
    *matches = تہی;

    garray_T	ga;
    ga_init2(&ga, حجم(حرف *), 10);

    size_t pat_len = (صحیح)STRLEN(pat);
    حرف *dirnames[] = {"", تہی};
    ExpandRTDir_int(pat, pat_len, runtime_expand_flags, TRUE, &ga, dirnames);

    // Try to complete values for [where] argument when none was found.
    اگر (runtime_expand_flags == 0)
    {
	حرف *where_اقدار[] = {"START", "OPT", "PACK", "ALL"};
	برائے (size_t i = 0; i < ARRAY_لمبائی(where_اقدار); ++i)
	    اگر (STRNCMP(pat, where_اقدار[i], pat_len) == 0)
	    {
		char_u *p = vim_strsave((char_u *)where_اقدار[i]);
		اگر (p != تہی && ga_جوڑو_string(&ga, p) == FAIL)
		    vim_free(p);
	    }
    }

    اگر (ga.ga_len == 0)
	واپس FAIL;

    *matches = ga.ga_ڈیٹا;
    *عدد_Matches = ga.ga_len;
    واپس OK;
}

/*
 * Expand loadplugin names:
 * 'packpath'/pack/ * /opt/{pat}
 */
    صحیح
Expand_Pack_جوڑو_Dir(
    char_u	*pat,
    صحیح		*عدد_file,
    char_u	***file)
{
    char_u	*s;
    char_u	*e;
    char_u	*مطابقت;
    garray_T	ga;
    صحیح		i;
    صحیح		pat_len;

    *عدد_file = 0;
    *file = تہی;
    pat_len = (صحیح)STRLEN(pat);
    ga_init2(&ga, حجم(حرف *), 10);

    s = alloc(pat_len + 26);
    اگر (s == تہی)
    {
	ga_clear_لڑیاں(&ga);
	واپس FAIL;
    }
    sprintf((حرف *)s, "pack/*/opt/%s*", pat);
    globpath(p_pp, s, &ga, 0, TRUE);
    vim_free(s);

    برائے (i = 0; i < ga.ga_len; ++i)
    {
	مطابقت = ((char_u **)ga.ga_ڈیٹا)[i];
	s = gettail(مطابقت);
	e = s + STRLEN(s);
	mch_memmove(مطابقت, s, e - s + 1);
    }

    اگر (ga.ga_len == 0)
	واپس FAIL;

    // Sort and remove duplicates which can happen when specifying multiple
    // directories in dirnames.
    remove_duplicates(&ga);

    *file = ga.ga_ڈیٹا;
    *عدد_file = ga.ga_len;
    واپس OK;
}

    ساکن خالی
cmd_ماخذ(char_u *fname, exarg_T *eap)
{
    صحیح clearvars = FALSE;

    اگر (*fname != NUL && STRNCMP(fname, "++clear", 7) == 0)
    {
	// ++clear argument is supplied
	clearvars = TRUE;
	fname = fname + 7;
	اگر (*fname != NUL)
	{
	    semsg(_(e_invalid_دلیل_str), eap->arg);
	    واپس;
	}
    }

    اگر (*fname != NUL && eap != تہی && eap->addr_گنتی > 0)
    {
	// if a filename is specified to :source, then a range is not allowed
	emsg(_(e_no_range_allowed));
	واپس;
    }

    اگر (eap != تہی && *fname == NUL)
    {
	اگر (eap->forceit)
	    // a file name is needed to source normal mode commands
	    emsg(_(e_دلیل_required));
	ورنہ
	    // source ex commands from the current buffer
	    do_ماخذ_ext(تہی, FALSE, DOSO_NONE, تہی, eap, clearvars);
    }
    ورنہ اگر (eap != تہی && eap->forceit)
	// ":source!": read Normal mode commands
	// Need to execute the commands directly.  This is required at least
	// for:
	// - ":g" command busy
	// - after ":argdo", ":windo" or ":bufdo"
	// - another command follows
	// - inside a loop
	openscript(fname, global_busy || listcmd_busy || eap->nextcmd != تہی
#اگر_تعریف_شدہ FEAT_EVAL
						 || eap->cstack->cs_idx >= 0
#اگر_ختم
						 );

    // ":source" read ex commands
    ورنہ اگر (do_ماخذ(fname, FALSE, DOSO_NONE, تہی) == FAIL)
	semsg(_(e_cant_open_file_str), fname);
}

/*
 * ":source {fname}"
 */
    خالی
ex_ماخذ(exarg_T *eap)
{
#اگر_تعریف_شدہ FEAT_BROWSE
    اگر (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	char_u *fname = تہی;

	fname = do_browse(0, (char_u *)_("Source Vim script"), eap->arg,
				      تہی, تہی,
				      (char_u *)_(BROWSE_FILTER_MACROS), تہی);
	اگر (fname != تہی)
	{
	    cmd_ماخذ(fname, eap);
	    vim_free(fname);
	}
    }
    ورنہ
#اگر_ختم
	cmd_ماخذ(eap->arg, eap);
}

#اگر defined(FEAT_EVAL)
/*
 * ":options"
 */
    خالی
ex_options(
    exarg_T	*eap غیر_مستعمل)
{
    char_u  buf[500];
    صحیح	    multi_mods = 0;

    buf[0] = NUL;
    (خالی)جوڑو_win_cmd_modifiers(buf, &cmdmod, &multi_mods);

    vim_setenv((char_u *)"OPTWIN_CMD", buf);
    cmd_ماخذ((char_u *)SYS_OPTWIN_FILE, تہی);
}
#اگر_ختم

/*
 * ":source" and associated commands.
 */

#اگر defined(FEAT_EVAL)
/*
 * Return the address holding the next breakpoint line for a source cookie.
 */
    linenr_T *
ماخذ_breakpoint(خالی *cookie)
{
    واپس &((ماخذ_cookie_T *)cookie)->breakpoint;
}

/*
 * Return the address holding the debug tick for a source cookie.
 */
    صحیح *
ماخذ_dbg_tick(خالی *cookie)
{
    واپس &((ماخذ_cookie_T *)cookie)->dbg_tick;
}

/*
 * Return the nesting level for a source cookie.
 */
    صحیح
ماخذ_level(خالی *cookie)
{
    واپس ((ماخذ_cookie_T *)cookie)->level;
}

/*
 * Return the readahead line. Note that the pointer may become invalid when
 * getting the next line, if it's concatenated with the next one.
 */
    char_u *
ماخذ_nextline(خالی *cookie)
{
    واپس ((ماخذ_cookie_T *)cookie)->nextline;
}
#اگر_ختم

#اگر (defined(MSWIN) && defined(FEAT_CSCOPE)) || defined(HAVE_FD_CLOEXEC)
# define USE_FOPEN_NOINH
/*
 * Special function to open a file without handle inheritance.
 * When possible the handle is closed on exec().
 */
    ساکن FILE *
fopen_noinh_readbin(حرف *filename)
{
# ifdef MSWIN
    صحیح	fd_tmp = mch_open(filename, O_RDONLY | O_ثنائی | O_NOINHERIT, 0);
# ورنہ
    صحیح	fd_tmp = mch_open(filename, O_RDONLY, 0);
# endif

    اگر (fd_tmp == -1)
	واپس تہی;

# ifdef HAVE_FD_CLOEXEC
    {
	صحیح fdflags = fcntl(fd_tmp, F_GETFD);
	اگر (fdflags >= 0 && (fdflags & FD_CLOEXEC) == 0)
	    (خالی)fcntl(fd_tmp, F_SETFD, fdflags | FD_CLOEXEC);
    }
# endif

    واپس fdopen(fd_tmp, READBIN);
}
#اگر_ختم

/*
 * Initialization for sourcing lines from the current buffer. Reads all the
 * lines from the buffer and stores it in the cookie grow array.
 * Returns a pointer to the name ":source buffer=<n>" on success and NULL on
 * failure.
 */
    ساکن char_u *
do_ماخذ_بفر_init(ماخذ_cookie_T *sp, exarg_T *eap)
{
    linenr_T	curr_lnum;
    char_u	*سطر = تہی;
    char_u	*fname;

    اگر (curbuf == تہی)
	واپس تہی;

    // Use ":source buffer=<num>" as the script name
    اگر (curbuf->b_ffname != تہی)
	fname = vim_strsave(curbuf->b_ffname);
    ورنہ
    {
	vim_snprintf((حرف *)IObuff, IOSIZE, ":source buffer=%d", curbuf->b_fnum);
	fname = vim_strsave(IObuff);
    }
    اگر (fname == تہی)
	واپس تہی;

    ga_init2(&sp->buflines, حجم(char_u *), 100);

    // Copy the lines from the buffer into a grow array
    برائے (curr_lnum = eap->line1; curr_lnum <= eap->line2; curr_lnum++)
    {
	سطر = vim_strsave(ml_get(curr_lnum));
	اگر (سطر == تہی)
	    جاؤ errret;
	اگر (ga_جوڑو_string(&sp->buflines, سطر) == FAIL)
	    جاؤ errret;
    }
    sp->buf_lnum = 0;
    sp->ماخذ_from_buf = TRUE;
    // When sourcing a range of lines from a buffer, use buffer line number.
    sp->sourcing_lnum = eap->line1 - 1;

    واپس fname;

errret:
    vim_free(fname);
    vim_free(سطر);
    ga_clear_لڑیاں(&sp->buflines);
    واپس تہی;
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
    ساکن صحیح
do_ماخذ_ext(
    char_u	*fname,
    صحیح		check_other,	    // check for .vimrc and _vimrc
    صحیح		is_vimrc,	    // DOSO_ value
    صحیح		*ret_sid غیر_مستعمل,
    exarg_T	*eap,
    صحیح		clearvars غیر_مستعمل)
{
    ماخذ_cookie_T	    cookie;
    char_u		    *p;
    char_u		    *fname_not_fixed = تہی;
    char_u		    *fname_exp = تہی;
    char_u		    *firstline = تہی;
    صحیح			    retval = FAIL;
    sctx_T		    save_current_sctx;
#اگر_تعریف_شدہ STARTUPTIME
    ساخت timeval	    tv_rel;
    ساخت timeval	    tv_آغاز;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_PROFILE
    proftime_T		    wait_آغاز;
#اگر_ختم
    صحیح			    save_sticky_cmdmod_flags = sticky_cmdmod_flags;
    صحیح			    trigger_ماخذ_post = FALSE;
#اگر_تعریف_شدہ FEAT_EVAL
    funccal_اندراج_T	    funccalp_اندراج;
    صحیح			    save_debug_break_level = debug_break_level;
    صحیح			    sid = -1;
    scriptitem_T	    *si = تہی;
    صحیح			    save_estack_compiling = estack_compiling;
    ESTACK_CHECK_DECLARATION;
#اگر_ختم

    CLEAR_میدان(cookie);
    اگر (fname == تہی)
    {
	// sourcing lines from a buffer
	fname_exp = do_ماخذ_بفر_init(&cookie, eap);
	اگر (fname_exp == تہی)
	    واپس FAIL;
    }
    ورنہ
    {
	fname_not_fixed = expand_env_save(fname);
	اگر (fname_not_fixed == تہی)
	    جاؤ theend;
	fname_exp = fix_fname(fname_not_fixed);
	اگر (fname_exp == تہی)
	    جاؤ theend;
	اگر (mch_isdir(fname_exp))
	{
	    smsg(_("Cannot source a directory: \"%s\""), fname);
	    جاؤ theend;
	}
    }
#اگر_تعریف_شدہ FEAT_EVAL
    estack_compiling = FALSE;

    // See if we loaded this script before.
    sid = ڈھونڈو_script_by_name(fname_exp);
    اگر (sid > 0 && ret_sid != تہی
			  && SCRIPT_شے(sid)->sn_state != SN_STATE_NOT_LOADED)
    {
	// Already loaded and no need to load again, return here.
	*ret_sid = sid;
	retval = OK;
	جاؤ theend;
    }
#اگر_ختم

    // Apply SourceCmd autocommands, they should get the file and source it.
    // When sourcing a file "fname_exp" was made a full path name by
    // fix_fname() above, avoid expanding it again.
    اگر ((fname == تہی
		? has_autocmd(EVENT_SOURCECMD, fname_exp, تہی)
		: has_autocmd_fullname(EVENT_SOURCECMD, fname_exp, تہی))
	    && apply_autocmds(EVENT_SOURCECMD, fname_exp, fname_exp,
							       FALSE, curbuf))
    {
#اگر_تعریف_شدہ FEAT_EVAL
	retval = aborting() ? FAIL : OK;
#ورنہ
	retval = OK;
#اگر_ختم
	اگر (retval == OK)
	    // Apply SourcePost autocommands.
	    apply_autocmds(EVENT_SOURCEPOST, fname_exp, fname_exp,
								FALSE, curbuf);
	جاؤ theend;
    }

    // Apply SourcePre autocommands, they may get the file.
    apply_autocmds(EVENT_SOURCEPRE, fname_exp, fname_exp, FALSE, curbuf);

    اگر (!cookie.ماخذ_from_buf)
    {
#اگر_تعریف_شدہ USE_FOPEN_NOINH
	cookie.fp = fopen_noinh_readbin((حرف *)fname_exp);
#ورنہ
	cookie.fp = mch_fopen((حرف *)fname_exp, READBIN);
#اگر_ختم
    }
    اگر (cookie.fp == تہی && check_other)
    {
	// Try again, replacing file name ".vimrc" by "_vimrc" or vice versa,
	// and ".exrc" by "_exrc" or vice versa.
	p = gettail(fname_exp);
	اگر ((*p == '.' || *p == '_')
		&& (STRICMP(p + 1, "vimrc") == 0
		    || STRICMP(p + 1, "gvimrc") == 0
		    || STRICMP(p + 1, "exrc") == 0))
	{
	    اگر (*p == '_')
		*p = '.';
	    ورنہ
		*p = '_';
#اگر_تعریف_شدہ USE_FOPEN_NOINH
	    cookie.fp = fopen_noinh_readbin((حرف *)fname_exp);
#ورنہ
	    cookie.fp = mch_fopen((حرف *)fname_exp, READBIN);
#اگر_ختم
	}
    }

    اگر (cookie.fp == تہی && !cookie.ماخذ_from_buf)
    {
	اگر (p_verbose > 0)
	{
	    verbose_enter();
	    اگر (SOURCING_NAME == تہی)
		smsg(_("could not source \"%s\""), fname);
	    ورنہ
		smsg(_("line %ld: could not source \"%s\""),
							SOURCING_LNUM, fname);
	    verbose_leave();
	}
	جاؤ theend;
    }

    // The file exists.
    // - In verbose mode, give a message.
    // - For a vimrc file, may want to set 'compatible', call vimrc_found().
    اگر (p_verbose > 1)
    {
	verbose_enter();
	اگر (SOURCING_NAME == تہی)
	    smsg(_("sourcing \"%s\""), fname);
	ورنہ
	    smsg(_("line %ld: sourcing \"%s\""), SOURCING_LNUM, fname);
	verbose_leave();
    }
    اگر (is_vimrc == DOSO_VIMRC)
	vimrc_ملا(fname_exp, (char_u *)"MYVIMRC");
    ورنہ اگر (is_vimrc == DOSO_GVIMRC)
	vimrc_ملا(fname_exp, (char_u *)"MYGVIMRC");

#اگر_تعریف_شدہ USE_CRNL
    // If no automatic file format: Set default to CR-NL.
    اگر (*p_ffs == NUL)
	cookie.fileformat = EOL_DOS;
    ورنہ
	cookie.fileformat = EOL_UNKNOWN;
#اگر_ختم

#اگر_تعریف_شدہ FEAT_EVAL
    // Check if this script has a breakpoint.
    cookie.breakpoint = dbg_ڈھونڈو_breakpoint(TRUE, fname_exp, (linenr_T)0);
    cookie.fname = fname_exp;
    cookie.dbg_tick = debug_tick;

    cookie.level = ex_nesting_level;
#اگر_ختم

#اگر_تعریف_شدہ STARTUPTIME
    اگر (وقت_fd != تہی)
	وقت_ڈالو(&tv_rel, &tv_آغاز);
#اگر_ختم

    // "legacy" does not apply to commands in the script
    sticky_cmdmod_flags = 0;

    save_current_sctx = current_sctx;
    اگر ((cmdmod.cmod_flags & CMOD_VIM9CMD) && cookie.ماخذ_from_buf)
	// When the ":vim9cmd" command modifier is used, source buffer lines as
	// Vim9 script
	current_sctx.sc_نسخہ = SCRIPT_نسخہ_VIM9;
    ورنہ
	current_sctx.sc_نسخہ = 1;  // default script version

#اگر_تعریف_شدہ FEAT_EVAL
    current_sctx.sc_lnum = 0;

# ifdef FEAT_PROFILE
    اگر (do_profiling == PROF_YES)
	prof_بچہ_enter(&wait_آغاز);		// entering a child now
# endif

    // Don't use local function variables, if called from a function.
    // Also starts profiling timer for nested script.
    save_funccal(&funccalp_اندراج);

    // Reset "KeyTyped" to avoid some commands thinking they are invoked
    // interactively.  E.g. defining a function would output indent.
    صحیح save_کلید_Typed = کلید_Typed;
    کلید_Typed = FALSE;

    // Check if this script was sourced before to find its SID.
    // Always use a new sequence number.
    current_sctx.sc_seq = ++آخری_current_SID_seq;
    اگر (sid > 0)
    {
	hashtab_T	*ht;
	صحیح		todo;
	hashitem_T	*hi;
	dictitem_T	*di;

	// loading the same script again
	current_sctx.sc_sid = sid;
	si = SCRIPT_شے(sid);
	اگر (si->sn_state == SN_STATE_NOT_LOADED)
	{
	    // this script was found but not loaded yet
	    si->sn_state = SN_STATE_NEW;
	}
	ورنہ
	{
	    si->sn_state = SN_STATE_RELOAD;

	    اگر (!clearvars)
	    {
		// Script-local variables remain but "const" can be set again.
		// In Vim9 script variables will be cleared when "vim9script"
		// is encountered without the "noclear" argument.
		ht = &SCRIPT_VARS(sid);
		todo = (صحیح)ht->ht_مستعمل;
		FOR_ALL_HASHTAB_اشیاء(ht, hi, todo)
		    اگر (!HASHITEM_خالی_خانے(hi))
		    {
			--todo;
			di = HI2DI(hi);
			di->di_flags |= DI_FLAGS_RELOAD;
		    }
		// imports can be redefined once
		mark_imports_for_reload(sid);
	    }
	    ورنہ
		clear_vim9_scriptlocal_vars(sid);

	    // reset version, "vim9script" may have been added or removed.
	    si->sn_نسخہ = 1;
	}
	اگر (ret_sid != تہی)
	    *ret_sid = sid;
    }
    ورنہ
    {
	صحیح error = OK;

	// It's new, generate a new SID and initialize the scriptitem.
	sid = get_new_scriptitem(&error);
	current_sctx.sc_sid = sid;
	اگر (error == FAIL)
	    جاؤ almosttheend;
	si = SCRIPT_شے(sid);
	si->sn_name = fname_exp;
	fname_exp = vim_strsave(si->sn_name);  // used for autocmd
	اگر (ret_sid != تہی)
	    *ret_sid = sid;

	// Remember the "is_vimrc" flag for when the file is sourced again.
	si->sn_is_vimrc = is_vimrc;
    }

    // Keep the sourcing name/lnum, for recursive calls.
    estack_ڈالو(ETYPE_SCRIPT, si->sn_name, 0);
    ESTACK_CHECK_تیاری;

# ifdef FEAT_PROFILE
    اگر (do_profiling == PROF_YES)
    {
	صحیح	forceit;

	// Check if we do profiling for this script.
	اگر (!si->sn_prof_on && has_profiling(TRUE, si->sn_name, &forceit, تہی))
	{
	    script_do_profile(si);
	    si->sn_pr_force = forceit;
	}
	اگر (si->sn_prof_on)
	{
	    ++si->sn_pr_گنتی;
	    profile_آغاز(&si->sn_pr_آغاز);
	    profile_zero(&si->sn_pr_children);
	}
    }
# endif
#ورنہ
    // Keep the sourcing name/lnum, for recursive calls.
    estack_ڈالو(ETYPE_SCRIPT, fname_exp, 0);
#اگر_ختم

    cookie.conv.vc_type = CONV_NONE;		// no conversion

    // Read the first line so we can check for a UTF-8 BOM.
    firstline = getsourceline(0, (خالی *)&cookie, 0, TRUE);
    اگر (firstline != تہی && STRLEN(firstline) >= 3 && firstline[0] == 0xef
			      && firstline[1] == 0xbb && firstline[2] == 0xbf)
    {
	// Found BOM; setup conversion, skip over BOM and recode the line.
	convert_تیاری(&cookie.conv, (char_u *)"utf-8", p_enc);
	p = string_convert(&cookie.conv, firstline + 3, تہی);
	اگر (p == تہی)
	    p = vim_strsave(firstline + 3);
	اگر (p != تہی)
	{
	    vim_free(firstline);
	    firstline = p;
	}
    }

    // Call do_cmdline, which will call getsourceline() to get the lines.
    do_cmdline(firstline, getsourceline, (خالی *)&cookie,
				     DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_REPEAT);
    retval = OK;

#اگر_تعریف_شدہ FEAT_PROFILE
    اگر (do_profiling == PROF_YES)
    {
	// Get "si" again, "script_items" may have been reallocated.
	si = SCRIPT_شے(sid);
	اگر (si->sn_prof_on)
	{
	    profile_انتہا(&si->sn_pr_آغاز);
	    profile_sub_wait(&wait_آغاز, &si->sn_pr_آغاز);
	    profile_جوڑو(&si->sn_pr_کل, &si->sn_pr_آغاز);
	    profile_self(&si->sn_pr_self, &si->sn_pr_آغاز,
							 &si->sn_pr_children);
	}
    }
#اگر_ختم

    اگر (got_int)
	emsg(_(e_interrupted));
#اگر_تعریف_شدہ FEAT_EVAL
    ESTACK_CHECK_NOW;
#اگر_ختم
    estack_نکالو();
    اگر (p_verbose > 1)
    {
	verbose_enter();
	smsg(_("finished sourcing %s"), fname);
	اگر (SOURCING_NAME != تہی)
	    smsg(_("continuing in %s"), SOURCING_NAME);
	verbose_leave();
    }
#اگر_تعریف_شدہ STARTUPTIME
    اگر (وقت_fd != تہی)
    {
	vim_snprintf((حرف *)IObuff, IOSIZE, "sourcing %s", fname);
	وقت_پیغام_2((حرف *)IObuff, &tv_آغاز);
	وقت_نکالو(&tv_rel);
    }
#اگر_ختم

    اگر (!got_int)
	trigger_ماخذ_post = TRUE;

#اگر_تعریف_شدہ FEAT_EVAL
    // After a "finish" in debug mode, need to break at first command of next
    // sourced file.
    اگر (save_debug_break_level > ex_nesting_level
	    && debug_break_level == ex_nesting_level)
	++debug_break_level;
#اگر_ختم

#اگر_تعریف_شدہ FEAT_EVAL
almosttheend:
    // If "sn_save_cpo" is set that means we encountered "vim9script": restore
    // 'cpoptions', unless in the main .vimrc file.
    // Get "si" again, "script_items" may have been reallocated.
    si = SCRIPT_شے(sid);
    اگر (si->sn_save_cpo != تہی && si->sn_is_vimrc == DOSO_NONE)
    {
	اگر (STRCMP(p_cpo, CPO_VIM) != 0)
	{
	    char_u *f;
	    char_u *t;

	    // 'cpo' was changed in the script.  Apply the same change to the
	    // saved value, if possible.
	    برائے (f = (char_u *)CPO_VIM; *f != NUL; ++f)
		اگر (vim_strchr(p_cpo, *f) == تہی
			&& (t = vim_strchr(si->sn_save_cpo, *f)) != تہی)
		    // flag was removed, also remove it from the saved 'cpo'
		    mch_memmove(t, t + 1, STRLEN(t));
	    برائے (f = p_cpo; *f != NUL; ++f)
		اگر (vim_strchr((char_u *)CPO_VIM, *f) == تہی
			&& vim_strchr(si->sn_save_cpo, *f) == تہی)
		{
		    // flag was added, also add it to the saved 'cpo'
		    t = alloc(STRLEN(si->sn_save_cpo) + 2);
		    اگر (t != تہی)
		    {
			*t = *f;
			STRCPY(t + 1, si->sn_save_cpo);
			vim_free(si->sn_save_cpo);
			si->sn_save_cpo = t;
		    }
		}
	}
	سیٹ_option_قدر_give_err((char_u *)"cpo",
					   0L, si->sn_save_cpo, OPT_NO_REDRAW);
    }
    VIM_CLEAR(si->sn_save_cpo);

    restore_funccal();
# ifdef FEAT_PROFILE
    اگر (do_profiling == PROF_YES)
	prof_بچہ_exit(&wait_آغاز);		// leaving a child now
# endif

    کلید_Typed = save_کلید_Typed;
#اگر_ختم
    current_sctx = save_current_sctx;

    اگر (cookie.fp != تہی)
	fclose(cookie.fp);
    اگر (cookie.ماخذ_from_buf)
	ga_clear_لڑیاں(&cookie.buflines);
    vim_free(cookie.nextline);
    vim_free(firstline);
    convert_تیاری(&cookie.conv, تہی, تہی);

    اگر (trigger_ماخذ_post)
	apply_autocmds(EVENT_SOURCEPOST, fname_exp, fname_exp, FALSE, curbuf);

theend:
#اگر_تعریف_شدہ FEAT_EVAL
    اگر (sid > 0 && ret_sid != تہی
	    && fname_not_fixed != تہی && fname_exp != تہی)
    {
	صحیح not_fixed_sid = ڈھونڈو_script_by_name(fname_not_fixed);

	// If "fname_not_fixed" is a symlink then we source the linked file.
	// If the original name is in the script list we add the ID of the
	// script that was actually sourced.
	اگر (SCRIPT_ID_VALID(not_fixed_sid) && not_fixed_sid != sid)
	    SCRIPT_شے(not_fixed_sid)->sn_sourced_sid = sid;
    }
#اگر_ختم

    vim_free(fname_not_fixed);
    vim_free(fname_exp);
    sticky_cmdmod_flags = save_sticky_cmdmod_flags;
#اگر_تعریف_شدہ FEAT_EVAL
    estack_compiling = save_estack_compiling;
#اگر_ختم
    واپس retval;
}

    صحیح
do_ماخذ(
    char_u	*fname,
    صحیح		check_other,	    // check for .vimrc and _vimrc
    صحیح		is_vimrc,	    // DOSO_ value
    صحیح		*ret_sid)
{
    واپس do_ماخذ_ext(fname, check_other, is_vimrc, ret_sid, تہی, FALSE);
}


#اگر defined(FEAT_EVAL)

/*
 * ":scriptnames"
 */
    خالی
ex_scriptnames(exarg_T *eap)
{
    صحیح i;

    اگر (eap->addr_گنتی > 0 || *eap->arg != NUL)
    {
	// :script {scriptId}: edit the script
	اگر (eap->addr_گنتی > 0 && !SCRIPT_ID_VALID(eap->line2))
	    emsg(_(e_invalid_دلیل));
	ورنہ
	{
	    اگر (eap->addr_گنتی > 0)
		eap->arg = SCRIPT_شے(eap->line2)->sn_name;
	    ورنہ
	    {
		expand_env(eap->arg, NameBuff, MAXPATHL);
		eap->arg = NameBuff;
	    }
	    do_exedit(eap, تہی);
	}
	واپس;
    }

    برائے (i = 1; i <= script_اشیاء.ga_len && !got_int; ++i)
    {
	scriptitem_T *si = SCRIPT_شے(i);

	اگر (si->sn_name != تہی)
	{
	    حرف sourced_buf[20];

	    home_replace(تہی, si->sn_name, NameBuff, MAXPATHL, TRUE);
	    اگر (si->sn_sourced_sid > 0)
		vim_snprintf(sourced_buf, 20, "->%d", si->sn_sourced_sid);
	    ورنہ
		sourced_buf[0] = NUL;
	    vim_snprintf((حرف *)IObuff, IOSIZE, "%3d%s%s: %s",
		    i,
		    sourced_buf,
		    si->sn_state == SN_STATE_NOT_LOADED ? " A" : "",
		    NameBuff);
	    اگر (!پیغام_filtered(IObuff))
	    {
		پیغام_putchar('\n');
		پیغام_outtrans(IObuff);
		برآمد_flush();	    // output one line at a time
		ui_breakcheck();
	    }
	}
    }
}

# اگر defined(BACKSLASH_اندر_FILENAME)
/*
 * Fix slashes in the list of script names for 'shellslash'.
 */
    خالی
scriptnames_slash_adjust(خالی)
{
    صحیح i;

    برائے (i = 1; i <= script_اشیاء.ga_len; ++i)
	اگر (SCRIPT_شے(i)->sn_name != تہی)
	    slash_adjust(SCRIPT_شے(i)->sn_name);
}
# endif

/*
 * Get a pointer to a script name.  Used for ":verbose set".
 * Message appended to "Last set from "
 */
    char_u *
get_scriptname(scid_T id)
{
    اگر (id == SID_MODELINE)
	واپس (char_u *)_("modeline");
    اگر (id == SID_CMDARG)
	واپس (char_u *)_("--cmd argument");
    اگر (id == SID_CARG)
	واپس (char_u *)_("-c argument");
    اگر (id == SID_ENV)
	واپس (char_u *)_("environment variable");
    اگر (id == SID_ERROR)
	واپس (char_u *)_("error handler");
    اگر (id == SID_WINLAYOUT)
	واپس (char_u *)_("changed window size");
    واپس SCRIPT_شے(id)->sn_name;
}

# اگر defined(EXITFREE)
    خالی
free_scriptnames(خالی)
{
    صحیح			i;

    برائے (i = script_اشیاء.ga_len; i > 0; --i)
    {
	scriptitem_T *si = SCRIPT_شے(i);

	// the variables themselves are cleared in evalvars_clear()
	vim_free(si->sn_vars);

	vim_free(si->sn_name);
	free_imports_and_script_vars(i);
	free_string_option(si->sn_save_cpo);
#  ifdef FEAT_PROFILE
	ga_clear(&si->sn_prl_ga);
#  endif
	vim_free(si->sn_autoload_سابقہ);
	vim_free(si);
    }
    ga_clear(&script_اشیاء);
}

    خالی
free_autoload_scriptnames(خالی)
{
    ga_clear_لڑیاں(&ga_loaded);
}
# endif

    linenr_T
get_sourced_lnum(
	char_u *(*fgetline)(صحیح, خالی *, صحیح, getline_opt_T),
	خالی *cookie)
{
    واپس fgetline == getsourceline
			? ((ماخذ_cookie_T *)cookie)->sourcing_lnum
			: SOURCING_LNUM;
}

/*
 * Return a List of script-local functions defined in the script with id
 * 'sid'.
 */
    ساکن list_T *
get_script_local_funcs(scid_T sid)
{
    hashtab_T	*functbl;
    hashitem_T	*hi;
    long_u	todo;
    list_T	*l;

    l = list_alloc();
    اگر (l == تہی)
	واپس تہی;

    // Iterate through all the functions in the global function hash table
    // looking for functions with script ID 'sid'.
    functbl = func_tbl_get();
    todo = functbl->ht_مستعمل;
    FOR_ALL_HASHTAB_اشیاء(functbl, hi, todo)
    {
	ufunc_T	*fp;

	اگر (HASHITEM_خالی_خانے(hi))
	    جاری;

	--todo;
	fp = HI2UF(hi);

	// Add active functions with script id == 'sid'
	اگر (!(fp->uf_flags & FC_DEAD) && (fp->uf_script_ctx.sc_sid == sid))
	{
	    char_u	*name;

	    اگر (fp->uf_name_exp != تہی)
		name = fp->uf_name_exp;
	    ورنہ
		name = fp->uf_name;

	    list_append_string(l, name, -1);
	}
    }

    واپس l;
}

/*
 * getscriptinfo() function
 */
    خالی
f_getscriptinfo(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    char_u	*pat = تہی;
    regmatch_T	regmatch;
    صحیح		filterpat = FALSE;
    scid_T	sid = -1;

    اگر (rettv_list_alloc(rettv) == FAIL)
	واپس;

    اگر (check_for_opt_dict_arg(argvars, 0) == FAIL)
	واپس;

    l = rettv->vval.v_list;

    regmatch.regprog = تہی;
    regmatch.rm_ic = p_ic;

    اگر (argvars[0].v_type == VAR_DICT)
    {
	dictitem_T *sid_di = dict_ڈھونڈو(argvars[0].vval.v_dict,
							   (char_u *)"sid", 3);
	اگر (sid_di != تہی)
	{
	    صحیح error = FALSE;
	    sid = tv_get_نمبر_chk(&sid_di->di_tv, &error);
	    اگر (error)
		واپس;
	    اگر (sid <= 0)
	    {
		semsg(_(e_invalid_قدر_for_دلیل_str_str), "sid",
						tv_get_string(&sid_di->di_tv));
		واپس;
	    }
	}
	ورنہ
	{
	    pat = dict_get_string(argvars[0].vval.v_dict, "name", TRUE);
	    اگر (pat != تہی)
		regmatch.regprog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
	    اگر (regmatch.regprog != تہی)
		filterpat = TRUE;
	}
    }

    برائے (varnumber_T i = sid > 0 ? sid : 1;
		       (i == sid || sid <= 0) && i <= script_اشیاء.ga_len; ++i)
    {
	scriptitem_T	*si = SCRIPT_شے(i);
	dict_T		*d;

	اگر (si->sn_name == تہی)
	    جاری;

	اگر (filterpat && !vim_regexec(&regmatch, si->sn_name, (colnr_T)0))
	    جاری;

	اگر ((d = dict_alloc()) == تہی
		|| list_append_dict(l, d) == FAIL)
	{
	    dict_unref(d);
	    جاؤ theend;
	}
	اگر (dict_جوڑو_string(d, "name", si->sn_name) == FAIL
		|| dict_جوڑو_نمبر(d, "sid", i) == FAIL
		|| dict_جوڑو_نمبر(d, "sourced", si->sn_sourced_sid) == FAIL
		|| dict_جوڑو_نمبر(d, "version", si->sn_نسخہ) == FAIL
		|| dict_جوڑو_bool(d, "autoload",
				si->sn_state == SN_STATE_NOT_LOADED) == FAIL)
	    جاؤ theend;

	// When a script ID is specified, return information about only the
	// specified script, and add the script-local variables and functions.
	اگر (sid > 0)
	{
	    dict_T	*var_dict;
	    list_T	*fn_list;

	    var_dict = dict_copy(&si->sn_vars->sv_dict, TRUE, TRUE,
								get_copyID());
	    اگر (var_dict == تہی)
		جاؤ theend;
	    اگر (dict_جوڑو_dict(d, "variables", var_dict) == FAIL)
	    {
		dict_unref(var_dict);
		جاؤ theend;
	    }
	    --var_dict->dv_refcount;
	    fn_list = get_script_local_funcs(sid);
	    اگر (fn_list == تہی || dict_جوڑو_list(d, "functions", fn_list) == FAIL)
	    {
		list_unref(fn_list);
		جاؤ theend;
	    }
	}
    }

theend:
    vim_regfree(regmatch.regprog);
    vim_free(pat);
}

#اگر_ختم

    ساکن char_u *
get_one_sourceline(ماخذ_cookie_T *sp)
{
    garray_T		ga;
    صحیح			len;
    صحیح			c;
    char_u		*buf;
#اگر_تعریف_شدہ USE_CRNL
    صحیح			has_cr;		// CR-LF found
#اگر_ختم
    صحیح			have_read = FALSE;

    // use a growarray to store the sourced line
    ga_init2(&ga, 1, 250);

    // Loop until there is a finished line (or end-of-file).
    ++sp->sourcing_lnum;
    برائے (;;)
    {
	// make room to read at least 120 (more) characters
	اگر (ga_grow(&ga, 120) == FAIL)
	    توڑو;
	اگر (sp->ماخذ_from_buf)
	{
	    اگر (sp->buf_lnum >= sp->buflines.ga_len)
		توڑو;		    // all the lines are processed
	    ga_concat(&ga, ((char_u **)sp->buflines.ga_ڈیٹا)[sp->buf_lnum]);
	    sp->buf_lnum++;
	    اگر (ga_grow(&ga, 1) == FAIL)
		توڑو;
	    buf = (char_u *)ga.ga_ڈیٹا;
	    buf[ga.ga_len++] = NUL;
	    len = ga.ga_len;
	}
	ورنہ
	{
	    buf = (char_u *)ga.ga_ڈیٹا;
	    اگر (fgets((حرف *)buf + ga.ga_len, ga.ga_maxlen - ga.ga_len,
			sp->fp) == تہی)
		توڑو;
	    len = ga.ga_len + (صحیح)STRLEN(buf + ga.ga_len);
	}
#اگر_تعریف_شدہ USE_CRNL
	// Ignore a trailing CTRL-Z, when in Dos mode.	Only recognize the
	// CTRL-Z by its own, or after a NL.
	اگر (	   (len == 1 || (len >= 2 && buf[len - 2] == '\n'))
		&& sp->fileformat == EOL_DOS
		&& buf[len - 1] == Ctrl_Z)
	{
	    buf[len - 1] = NUL;
	    توڑو;
	}
#اگر_ختم

	have_read = TRUE;
	ga.ga_len = len;

	// If the line was longer than the buffer, read more.  A line of a
	// buffer was taken as a whole.
	اگر (!sp->ماخذ_from_buf && ga.ga_maxlen - ga.ga_len == 1
						       && buf[len - 1] != '\n')
	    جاری;

	اگر (len >= 1 && buf[len - 1] == '\n')	// remove trailing NL
	{
#اگر_تعریف_شدہ USE_CRNL
	    has_cr = (len >= 2 && buf[len - 2] == '\r');
	    اگر (sp->fileformat == EOL_UNKNOWN)
	    {
		اگر (has_cr)
		    sp->fileformat = EOL_DOS;
		ورنہ
		    sp->fileformat = EOL_UNIX;
	    }

	    اگر (sp->fileformat == EOL_DOS)
	    {
		اگر (has_cr)	    // replace trailing CR
		{
		    buf[len - 2] = '\n';
		    --len;
		    --ga.ga_len;
		}
		ورنہ	    // lines like ":map xx yy^M" will have failed
		{
		    اگر (!sp->error)
		    {
			پیغام_ماخذ(HL_ATTR(HLF_W));
			emsg(_("W15: Warning: Wrong line separator, ^M may be missing"));
		    }
		    sp->error = TRUE;
		    sp->fileformat = EOL_UNIX;
		}
	    }
#اگر_ختم
	    // The '\n' is escaped if there is an odd number of ^V's just
	    // before it, first set "c" just before the 'V's and then check
	    // len&c parities (is faster than ((len-c)%2 == 0)) -- Acevedo
	    برائے (c = len - 2; c >= 0 && buf[c] == Ctrl_V; c--)
		;
	    اگر ((len & 1) != (c & 1))	// escaped NL, read more
	    {
		++sp->sourcing_lnum;
		جاری;
	    }

	    buf[len - 1] = NUL;		// remove the NL
	}

	// Check for ^C here now and then, so recursive :so can be broken.
	سطر_breakcheck();
	توڑو;
    }

    اگر (have_read)
	واپس (char_u *)ga.ga_ڈیٹا;

    vim_free(ga.ga_ڈیٹا);
    واپس تہی;
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
	صحیح c غیر_مستعمل,
	خالی *cookie,
	صحیح indent غیر_مستعمل,
	getline_opt_T options)
{
    ماخذ_cookie_T	*sp = (ماخذ_cookie_T *)cookie;
    char_u		*سطر;
    char_u		*p;
    صحیح			do_vim9_all = اندر_vim9script()
					      && options == GETLINE_CONCAT_ALL;
    صحیح			do_bar_cont = do_vim9_all
					 || options == GETLINE_CONCAT_CONTBAR;

#اگر_تعریف_شدہ FEAT_EVAL
    // If breakpoints have been added/deleted need to check for it.
    اگر ((sp->dbg_tick < debug_tick) && !sp->ماخذ_from_buf)
    {
	sp->breakpoint = dbg_ڈھونڈو_breakpoint(TRUE, sp->fname, SOURCING_LNUM);
	sp->dbg_tick = debug_tick;
    }
# ifdef FEAT_PROFILE
    اگر (do_profiling == PROF_YES)
	script_سطر_انتہا();
# endif
#اگر_ختم

    // Set the current sourcing line number.
    SOURCING_LNUM = sp->sourcing_lnum + 1;

    // Get current line.  If there is a read-ahead line, use it, otherwise get
    // one now.  "fp" is NULL if actually using a string.
    اگر (sp->ختم_شدہ || (!sp->ماخذ_from_buf && sp->fp == تہی))
	سطر = تہی;
    ورنہ اگر (sp->nextline == تہی)
	سطر = get_one_sourceline(sp);
    ورنہ
    {
	سطر = sp->nextline;
	sp->nextline = تہی;
	++sp->sourcing_lnum;
    }
#اگر_تعریف_شدہ FEAT_PROFILE
    اگر (سطر != تہی && do_profiling == PROF_YES)
	script_سطر_آغاز();
#اگر_ختم

    // Only concatenate lines starting with a \ when 'cpoptions' doesn't
    // contain the 'C' flag.
    اگر (سطر != تہی && options != GETLINE_NONE
				      && vim_strchr(p_cpo, CPO_CONCAT) == تہی)
    {
	صحیح comment_char = اندر_vim9script() ? '#' : '"';

	// compensate for the one line read-ahead
	--sp->sourcing_lnum;

	// Get the next line and concatenate it when it starts with a
	// backslash. We always need to read the next line, keep it in
	// sp->nextline.
	/* Also check for a comment in between continuation lines: "\ */
	// Also check for a Vim9 comment, empty line, line starting with '|',
	// but not "||".
	sp->nextline = get_one_sourceline(sp);
	اگر (sp->nextline != تہی
		&& (*(p = skipwhite(sp->nextline)) == '\\'
			      || (p[0] == comment_char
						&& p[1] == '\\' && p[2] == ' ')
			      || (do_vim9_all && (*p == NUL
						     || vim9_comment_آغاز(p)))
			      || (do_bar_cont && p[0] == '|' && p[1] != '|')))
	{
	    garray_T    ga;

	    ga_init2(&ga, حجم(char_u), 400);
	    ga_concat(&ga, سطر);
	    اگر (*p == '\\')
		ga_concat(&ga, p + 1);
	    ورنہ اگر (*p == '|')
	    {
		GA_CONCAT_LITERAL(&ga, " ");
		ga_concat(&ga, p);
	    }
	    برائے (;;)
	    {
		vim_free(sp->nextline);
		sp->nextline = get_one_sourceline(sp);
		اگر (sp->nextline == تہی)
		    توڑو;
		p = skipwhite(sp->nextline);
		اگر (*p == '\\' || (do_bar_cont && p[0] == '|' && p[1] != '|'))
		{
		    // Adjust the growsize to the current length to speed up
		    // concatenating many lines.
		    اگر (ga.ga_len > 400)
		    {
			اگر (ga.ga_len > 8000)
			    ga.ga_growsize = 8000;
			ورنہ
			    ga.ga_growsize = ga.ga_len;
		    }
		    اگر (*p == '\\')
			ga_concat(&ga, p + 1);
		    ورنہ
		    {
			GA_CONCAT_LITERAL(&ga, " ");
			ga_concat(&ga, p);
		    }
		}
		ورنہ اگر (!(p[0] == (comment_char)
						&& p[1] == '\\' && p[2] == ' ')
		     && !(do_vim9_all && (*p == NUL || vim9_comment_آغاز(p))))
		    توڑو;
		/* drop a # comment or "\ comment line */
	    }
	    ga_append(&ga, NUL);
	    vim_free(سطر);
	    سطر = ga.ga_ڈیٹا;
	}
    }

    اگر (سطر != تہی && sp->conv.vc_type != CONV_NONE)
    {
	char_u	*s;

	// Convert the encoding of the script line.
	s = string_convert(&sp->conv, سطر, تہی);
	اگر (s != تہی)
	{
	    vim_free(سطر);
	    سطر = s;
	}
    }

#اگر_تعریف_شدہ FEAT_EVAL
    // Did we encounter a breakpoint?
    اگر (!sp->ماخذ_from_buf && sp->breakpoint != 0
	    && sp->breakpoint <= SOURCING_LNUM)
    {
	dbg_breakpoint(sp->fname, SOURCING_LNUM);
	// Find next breakpoint.
	sp->breakpoint = dbg_ڈھونڈو_breakpoint(TRUE, sp->fname, SOURCING_LNUM);
	sp->dbg_tick = debug_tick;
    }
#اگر_ختم

    واپس سطر;
}

/*
 * Returns TRUE if sourcing a script either from a file or a buffer.
 * Otherwise returns FALSE.
 */
    صحیح
sourcing_a_script(exarg_T *eap)
{
    واپس (getline_equal(eap->ea_getline, eap->cookie, getsourceline));
}

/*
 * ":scriptencoding": Set encoding conversion for a sourced script.
 */
    خالی
ex_scriptencoding(exarg_T *eap)
{
    ماخذ_cookie_T	*sp;
    char_u		*name;

    اگر (!sourcing_a_script(eap))
    {
	emsg(_(e_scriptencoding_مستعمل_outside_کا_sourced_file));
	واپس;
    }

    اگر (*eap->arg != NUL)
    {
	name = enc_canonize(eap->arg);
	اگر (name == تہی)	// out of memory
	    واپس;
    }
    ورنہ
	name = eap->arg;

    // Setup for conversion from the specified encoding to 'encoding'.
    sp = (ماخذ_cookie_T *)getline_cookie(eap->ea_getline, eap->cookie);
    convert_تیاری(&sp->conv, name, p_enc);

    اگر (name != eap->arg)
	vim_free(name);
}

/*
 * ":scriptversion": Set Vim script version for a sourced script.
 */
    خالی
ex_scriptversion(exarg_T *eap غیر_مستعمل)
{
    صحیح		nr;

    اگر (!sourcing_a_script(eap))
    {
	emsg(_(e_scriptversion_مستعمل_outside_کا_sourced_file));
	واپس;
    }
    اگر (اندر_vim9script())
    {
	emsg(_(e_cannot_use_scriptversion_after_vim9script));
	واپس;
    }

    nr = getdigits(&eap->arg);
    اگر (nr == 0 || *eap->arg != NUL)
	emsg(_(e_invalid_دلیل));
    ورنہ اگر (nr > SCRIPT_نسخہ_MAX)
	semsg(_(e_scriptversion_not_supported_nr), nr);
    ورنہ
    {
	current_sctx.sc_نسخہ = nr;
#اگر_تعریف_شدہ FEAT_EVAL
	SCRIPT_شے(current_sctx.sc_sid)->sn_نسخہ = nr;
#اگر_ختم
    }
}

#اگر defined(FEAT_EVAL)
/*
 * ":finish": Mark a sourced file as finished.
 */
    خالی
ex_اختتام(exarg_T *eap)
{
    اگر (sourcing_a_script(eap))
	do_اختتام(eap, FALSE);
    ورنہ
	emsg(_(e_اختتام_مستعمل_outside_کا_sourced_file));
}

/*
 * Mark a sourced file as finished.  Possibly makes the ":finish" pending.
 * Also called for a pending finish at the ":endtry" or after returning from
 * an extra do_cmdline().  "reanimate" is used in the latter case.
 */
    خالی
do_اختتام(exarg_T *eap, صحیح reanimate)
{
    صحیح		idx;

    اگر (reanimate)
	((ماخذ_cookie_T *)getline_cookie(eap->ea_getline,
					      eap->cookie))->ختم_شدہ = FALSE;

    // Cleanup (and inactivate) conditionals, but stop when a try conditional
    // not in its finally clause (which then is to be executed next) is found.
    // In this case, make the ":finish" pending for execution at the ":endtry".
    // Otherwise, finish normally.
    idx = cleanup_conditionals(eap->cstack, 0, TRUE);
    اگر (idx >= 0)
    {
	eap->cstack->cs_pending[idx] = CSTP_اختتام;
	report_make_pending(CSTP_اختتام, تہی);
    }
    ورنہ
	((ماخذ_cookie_T *)getline_cookie(eap->ea_getline,
					       eap->cookie))->ختم_شدہ = TRUE;
}


/*
 * Return TRUE when a sourced file had the ":finish" command: Don't give error
 * message for missing ":endif".
 * Return FALSE when not sourcing a file.
 */
    صحیح
ماخذ_ختم_شدہ(
    char_u	*(*fgetline)(صحیح, خالی *, صحیح, getline_opt_T),
    خالی	*cookie)
{
    واپس (getline_equal(fgetline, cookie, getsourceline)
	    && ((ماخذ_cookie_T *)getline_cookie(
						fgetline, cookie))->ختم_شدہ);
}

/*
 * Find the path of a script below the "autoload" directory.
 * Returns NULL if there is no "/autoload/" in the script name.
 */
    ساکن char_u *
script_name_after_autoload(scriptitem_T *si)
{
    char_u	*p = si->sn_name;
    char_u	*res = تہی;

    برائے (;;)
    {
	char_u *n = (char_u *)لڑی_تلاش((حرف *)p, "autoload");

	اگر (n == تہی)
	    توڑو;
	اگر (n > p && vim_ispathsep(n[-1]) && vim_ispathsep(n[8]))
	    res = n + 9;
	p = n + 8;
    }
    واپس res;
}

/*
 * For an autoload script "autoload/dir/script.vim" return the prefix
 * "dir#script#" in allocated memory.
 * Returns NULL if anything is wrong.
 */
    char_u *
get_autoload_سابقہ(scriptitem_T *si)
{
    char_u *p = script_name_after_autoload(si);
    char_u *سابقہ;

    اگر (p == تہی)
	واپس تہی;
    سابقہ = vim_strsave(p);
    اگر (سابقہ == تہی)
	واپس تہی;

    // replace all '/' with '#' and locate ".vim" at the end
    برائے (p = سابقہ; *p != NUL; p += mb_ptr2len(p))
    {
	اگر (vim_ispathsep(*p))
	    *p = '#';
	ورنہ اگر (STRCMP(p, ".vim") == 0)
	{
	    p[0] = '#';
	    p[1] = NUL;
	    واپس سابقہ;
	}
    }

    // did not find ".vim" at the end
    vim_free(سابقہ);
    واپس تہی;
}

/*
 * If in a Vim9 autoload script return "name" with the autoload prefix for the
 * script.  If successful the returned name is allocated.
 * Otherwise it returns "name" unmodified.
 */
    char_u *
may_سابقہ_autoload(char_u *name)
{
    اگر (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	واپس name;

    scriptitem_T *si = SCRIPT_شے(current_sctx.sc_sid);

    اگر (si->sn_autoload_سابقہ == تہی)
	واپس name;

    char_u  *basename = name;
    size_t  len;
    char_u  *res;

    اگر (*name == K_SPECIAL)
    {
	char_u *p = vim_strchr(name, '_');

	// skip over "<SNR>99_"
	اگر (p != تہی)
	    basename = p + 1;
    }

    len = STRLEN(si->sn_autoload_سابقہ) + STRLEN(basename) + 2;
    res = alloc(len);
    اگر (res == تہی)
	واپس تہی;

    vim_snprintf((حرف *)res, len, "%s%s", si->sn_autoload_سابقہ, basename);
    واپس res;
}

/*
 * Return the autoload script name for a function or variable name.
 * Returns NULL when out of memory.
 * Caller must make sure that "name" contains AUTOLOAD_CHAR.
 */
    char_u *
autoload_name(char_u *name)
{
    char_u	*p, *q = تہی;
    char_u	*scriptname;

    // Get the script file name: replace '#' with '/', append ".vim".
    scriptname = alloc(STRLEN(name) + 14);
    اگر (scriptname == تہی)
	واپس تہی;
    STRCPY(scriptname, "autoload/");
    STRCAT(scriptname, name[0] == 'g' && name[1] == ':' ? name + 2: name);
    برائے (p = scriptname + 9; (p = vim_strchr(p, AUTOLOAD_CHAR)) != تہی;
								    q = p, ++p)
	*p = '/';
    STRCPY(q, ".vim");
    واپس scriptname;
}

/*
 * If "name" has a package name try autoloading the script for it.
 * Return TRUE if a package was loaded.
 */
    صحیح
script_autoload(
    char_u	*name,
    صحیح		reload)	    // load script again when already loaded
{
    char_u	*p;
    char_u	*scriptname, *tofree;
    صحیح		ret = FALSE;
    صحیح		i;
    صحیح		ret_sid;

    // If the name starts with "<SNR>123_" then "123" is the script ID.
    اگر (name[0] == K_SPECIAL && name[1] == KS_EXTRA && name[2] == KE_SNR)
    {
	p = name + 3;
	ret_sid = (صحیح)getdigits(&p);
	اگر (*p == '_' && SCRIPT_ID_VALID(ret_sid))
	{
	    may_بھرو_script(ret_sid, &ret);
	    واپس ret;
	}
    }

    // If there is no '#' after name[0] there is no package name.
    p = vim_strchr(name, AUTOLOAD_CHAR);
    اگر (p == تہی || p == name)
	واپس FALSE;

    tofree = scriptname = autoload_name(name);
    اگر (scriptname == تہی)
	واپس FALSE;

    // Find the name in the list of previously loaded package names.  Skip
    // "autoload/", it's always the same.
    برائے (i = 0; i < ga_loaded.ga_len; ++i)
	اگر (STRCMP(((char_u **)ga_loaded.ga_ڈیٹا)[i] + 9, scriptname + 9) == 0)
	    توڑو;
    اگر (!reload && i < ga_loaded.ga_len)
	ret = FALSE;	    // was loaded already
    ورنہ
    {
	// Remember the name if it wasn't loaded already.
	اگر (i == ga_loaded.ga_len && ga_grow(&ga_loaded, 1) == OK)
	{
	    ((char_u **)ga_loaded.ga_ڈیٹا)[ga_loaded.ga_len++] = scriptname;
	    tofree = تہی;
	}

	// Try loading the package from $VIMRUNTIME/autoload/<name>.vim
	// Use "ret_sid" to avoid loading the same script again.
	اگر (ماخذ_اندر_راستہ(p_rtp, scriptname, DIP_آغاز, &ret_sid) == OK)
	    ret = TRUE;
    }

    vim_free(tofree);
    واپس ret;
}
#اگر_ختم
