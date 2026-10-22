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

#ضمن "vim.h"

#إذا defined(FEAT_EVAL)
// The names of packages that once were loaded are remembered.
ساكن garray_T		ga_loaded = {0, 0, حجم(char_u *), 4, عدم};
#د_که_پای

// last used sequence number for sourcing scripts (current_sctx.sc_seq)
#که_تعریف_شوی FEAT_EVAL
ساكن صحيح		وروستی_current_SID_seq = 0;
#د_که_پای

ساكن صحيح do_سرچینه_ext(char_u *fname, صحيح check_other, صحيح is_vimrc, صحيح *ret_sid, exarg_T *eap, صحيح clearvars);

/*
 * Initialize the execution stack.
 */
    فراغ
estack_init(فراغ)
{
    estack_T *ثبت;

    إذا (ga_grow(&exestack, 10) == FAIL)
	mch_exit(0);
    ثبت = ((estack_T *)exestack.ga_معلومات) + exestack.ga_len;
    ثبت->es_type = ETYPE_سر;
    ثبت->es_name = عدم;
    ثبت->es_lnum = 0;
#که_تعریف_شوی FEAT_EVAL
    ثبت->es_info.ufunc = عدم;
#د_که_پای
    ++exestack.ga_len;
}

/*
 * Add an item to the execution stack.
 * Returns the new entry or NULL when out of memory.
 */
    estack_T *
estack_ټېل(etype_T type, char_u *name, اوږد lnum)
{
    estack_T *ثبت;

    // If memory allocation fails then we'll pop more than we push, eventually
    // at the top level it will be OK again.
    إذا (ga_grow(&exestack, 1) == FAIL)
	أرجع عدم;

    ثبت = ((estack_T *)exestack.ga_معلومات) + exestack.ga_len;
    ثبت->es_type = type;
    ثبت->es_name = name;
    ثبت->es_lnum = lnum;
#که_تعریف_شوی FEAT_EVAL
    ثبت->es_info.ufunc = عدم;
#د_که_پای
    ++exestack.ga_len;
    أرجع ثبت;
}

#إذا defined(FEAT_EVAL)
/*
 * Add a user function to the execution stack.
 */
    estack_T *
estack_ټېل_ufunc(ufunc_T *ufunc, اوږد lnum)
{
    estack_T *ثبت = estack_ټېل(ETYPE_UFUNC,
	    ufunc->uf_name_exp != عدم
				  ? ufunc->uf_name_exp : ufunc->uf_name, lnum);
    إذا (ثبت != عدم)
	ثبت->es_info.ufunc = ufunc;
    أرجع ثبت;
}

/*
 * Return TRUE if "ufunc" with "lnum" is already at the top of the exe stack.
 */
    صحيح
estack_سر_is_ufunc(ufunc_T *ufunc, اوږد lnum)
{
    estack_T *ثبت;

    إذا (exestack.ga_len == 0)
	أرجع FALSE;
    ثبت = ((estack_T *)exestack.ga_معلومات) + exestack.ga_len - 1;
    أرجع ثبت->es_type == ETYPE_UFUNC
	&& STRCMP( ثبت->es_name, ufunc->uf_name_exp != عدم
				    ? ufunc->uf_name_exp : ufunc->uf_name) == 0
	&& ثبت->es_lnum == lnum;
}
#د_که_پای

/*
 * Take an item off of the execution stack and return it.
 */
    estack_T *
estack_راوباسه(فراغ)
{
    إذا (exestack.ga_len == 0)
	أرجع عدم;
    --exestack.ga_len;
    أرجع ((estack_T *)exestack.ga_معلومات) + exestack.ga_len;
}

/*
 * Get the current value for "which" in allocated memory.
 * "which" is ESTACK_SFILE for <sfile>, ESTACK_STACK for <stack> or
 * ESTACK_SCRIPT for <script>.
 */
    char_u *
estack_sfile(estack_arg_T which ناکارول_شوی)
{
    estack_T	*ثبت;
#که_تعریف_شوی FEAT_EVAL
    garray_T	ga;
    size_t	len;
    صحيح		idx;
    etype_T	وروستی_type = ETYPE_SCRIPT;
#د_که_پای

    ثبت = ((estack_T *)exestack.ga_معلومات) + exestack.ga_len - 1;
#که_تعریف_شوی FEAT_EVAL
    إذا (which == ESTACK_SFILE && ثبت->es_type != ETYPE_UFUNC)
#د_که_پای
    {
	إذا (ثبت->es_name == عدم)
	    أرجع عدم;
	أرجع vim_strsave(ثبت->es_name);
    }
#که_تعریف_شوی FEAT_EVAL
    // expand('<sfile>') works in a function for backwards compatibility, but
    // may give an unexpected result.  Disallow it in Vim 9 script.
    إذا (which == ESTACK_SFILE && دننه_vim9script())
    {
	صحيح  save_emsg_off = emsg_off;

	إذا (emsg_off == 1)
	    // f_expand() silences errors but we do want this one
	    emsg_off = 0;
	emsg(_(e_cannot_expand_sfile_دننه_vim9_function));
	emsg_off = save_emsg_off;
	أرجع عدم;
    }

    // If evaluated in a function or autocommand, return the path of the script
    // where it is defined, at script level the current script path is returned
    // instead.
    إذا (which == ESTACK_SCRIPT)
    {
	// Walk the stack backwards, starting from the current frame.
	لكل (idx = exestack.ga_len - 1; idx >= 0; --idx, --ثبت)
	{
	    إذا (ثبت->es_type == ETYPE_UFUNC || ثبت->es_type == ETYPE_AUCMD)
	    {
		sctx_T *def_ctx = ثبت->es_type == ETYPE_UFUNC
			      ? &ثبت->es_info.ufunc->uf_script_ctx
			      : ثبت->es_info.aucmd != عدم
				  ? acp_script_ctx(ثبت->es_info.aucmd) : عدم;

		أرجع def_ctx != عدم && def_ctx->sc_sid > 0
			   ? vim_strsave(SCRIPT_توکی(def_ctx->sc_sid)->sn_name)
			   : عدم;
	    }
	    وإلا إذا (ثبت->es_type == ETYPE_SCRIPT)
		أرجع vim_strsave(ثبت->es_name);
	}
	أرجع عدم;
    }

    // Give information about each stack entry up to the root.
    // For a function we compose the call stack, as it was done in the past:
    //   "function One[123]..Two[456]..Three"
    ga_init2(&ga, حجم(حرف), 100);
    لكل (idx = 0; idx < exestack.ga_len; ++idx)
    {
	ثبت = ((estack_T *)exestack.ga_معلومات) + idx;
	إذا (ثبت->es_name != عدم)
	{
	    اوږد	lnum = 0;
	    size_t	added = 0;
	    string_T	type_name = {(char_u *)"", 0};
	    string_T	class_name = {(char_u *)"", 0};
	    string_T	es_name = {ثبت->es_name, STRLEN(ثبت->es_name)};

	    إذا (ثبت->es_type != وروستی_type)
	    {
		بدل (ثبت->es_type)
		{
		    حالة ETYPE_SCRIPT:
			STR_LITERAL_سېټ(type_name, "script ");
			اكسر;
		    حالة ETYPE_UFUNC:
			STR_LITERAL_سېټ(type_name, "function ");
			اكسر;
		    افتراضي:
			اكسر;
		}
		وروستی_type = ثبت->es_type;
	    }
	    إذا (ثبت->es_type == ETYPE_UFUNC && ثبت->es_info.ufunc->uf_class != عدم)
	    {
		class_name.string = ثبت->es_info.ufunc->uf_class->class_name.string;
		class_name.اوږدتیا = ثبت->es_info.ufunc->uf_class->class_name.اوږدتیا;
	    }
	    إذا (idx == exestack.ga_len - 1)
		lnum = which == ESTACK_ډېری ? SOURCING_LNUM : 0;
	    وإلا
		lnum = ثبت->es_lnum;

	    len = es_name.اوږدتیا + type_name.اوږدتیا + class_name.اوږدتیا + 26;
	    إذا (ga_grow(&ga, (صحيح)len) == FAIL)
		اكسر;
	    ga_concat_len(&ga, type_name.string, type_name.اوږدتیا);
	    // For class methods prepend "<class name>." to the function name.
	    إذا (*class_name.string != NUL)
	    {
		added = vim_snprintf_safelen(
		    (حرف *)ga.ga_معلومات + ga.ga_len,
		    ga.ga_maxlen - ga.ga_len,
		    "<SNR>%d_%s.",
		    ثبت->es_info.ufunc->uf_script_ctx.sc_sid,
		    class_name.string);

		ga.ga_len += (صحيح)added;
	    }
	    ga_concat_len(&ga, es_name.string, es_name.اوږدتیا);
	    // For the bottom entry of <sfile>: do not add the line number, it is used in
	    // <slnum>.  Also leave it out when the number is not set.
	    إذا (lnum != 0)
	    {
		added = vim_snprintf_safelen(
		    (حرف *)ga.ga_معلومات + ga.ga_len,
		    ga.ga_maxlen - ga.ga_len,
		    "[%ld]",
		    lnum);

		ga.ga_len += (صحيح)added;
	    }
	    إذا (idx != exestack.ga_len - 1)
		GA_CONCAT_LITERAL(&ga, "..");
	}
    }

    // Only NUL-terminate when not returning NULL.
    إذا (ga.ga_معلومات != عدم)
	ga_append(&ga, NUL);
    أرجع (char_u *)ga.ga_معلومات;
#د_که_پای
}

#که_تعریف_شوی FEAT_EVAL
    ساكن فراغ
stacktrace_ټېل_توکی(
	list_T		*l,
	ufunc_T		*fp,
	char_u		*event,
	linenr_T	lnum,
	char_u		*filepath)
{
    dict_T	*d;
    typval_T	tv;

    d = dict_alloc_قلف(VAR_FIXED);
    إذا (d == عدم)
	أرجع;

    tv.v_type = VAR_DICT;
    tv.v_قلف = VAR_LOCKED;
    tv.vval.v_dict = d;

    إذا (fp != عدم)
	dict_جمع_کړه_func(d, "funcref", fp);
    إذا (event != عدم)
	dict_جمع_کړه_string(d, "event", event);
    dict_جمع_کړه_شمېره(d, "lnum", lnum);
    dict_جمع_کړه_string(d, "filepath", filepath);

    list_append_tv(l, &tv);
}

/*
 * Create the stacktrace from exestack.
 */
    list_T *
stacktrace_create(فراغ)
{
    list_T	*l;
    صحيح		i;

    l = list_alloc();
    إذا (l == عدم)
	أرجع عدم;

    لكل (i = 0; i < exestack.ga_len; ++i)
    {
	estack_T *ثبت = &((estack_T *)exestack.ga_معلومات)[i];
	linenr_T lnum = ثبت->es_lnum;

	إذا (ثبت->es_type == ETYPE_SCRIPT)
	    stacktrace_ټېل_توکی(l, عدم, عدم, lnum, ثبت->es_name);
	وإلا إذا (ثبت->es_type == ETYPE_UFUNC)
	{
	    ufunc_T *fp = ثبت->es_info.ufunc;
	    sctx_T sctx = fp->uf_script_ctx;
	    char_u *filepath = sctx.sc_sid > 0 ?
				   get_scriptname(sctx.sc_sid) : (char_u *)"";

	    lnum += sctx.sc_lnum;
	    stacktrace_ټېل_توکی(l, fp, عدم, lnum, filepath);
	}
	وإلا إذا (ثبت->es_type == ETYPE_AUCMD)
	{
	    // The autocmd may not have a matching pattern yet, in which case
	    // es_info.aucmd is still NULL.
	    sctx_T *sctx = ثبت->es_info.aucmd != عدم
			       ? acp_script_ctx(ثبت->es_info.aucmd) : عدم;
	    char_u *filepath = sctx != عدم && sctx->sc_sid > 0 ?
				   get_scriptname(sctx->sc_sid) : (char_u *)"";

	    إذا (sctx != عدم)
		lnum += sctx->sc_lnum;
	    stacktrace_ټېل_توکی(l, عدم, ثبت->es_name, lnum, filepath);
	}
    }
    أرجع l;
}

/*
 * getstacktrace() function
 */
    فراغ
f_getstacktrace(typval_T *argvars ناکارول_شوی, typval_T *rettv)
{
    rettv_list_سېټ(rettv, stacktrace_create());
}
#د_که_پای

/*
 * Get DIP_ flags from the [where] argument of a :runtime command.
 * "*argp" is advanced to after the [where] argument if it is found.
 */
    ساكن صحيح
get_runtime_cmd_flags(char_u **argp, size_t where_len)
{
    char_u *arg = *argp;

    إذا (where_len == 0)
	أرجع 0;

    إذا (STRNCMP(arg, "START", where_len) == 0)
    {
	*argp = skipwhite(arg + where_len);
	أرجع DIP_پیل + DIP_NORTP;
    }
    إذا (STRNCMP(arg, "OPT", where_len) == 0)
    {
	*argp = skipwhite(arg + where_len);
	أرجع DIP_OPT + DIP_NORTP;
    }
    إذا (STRNCMP(arg, "PACK", where_len) == 0)
    {
	*argp = skipwhite(arg + where_len);
	أرجع DIP_پیل + DIP_OPT + DIP_NORTP;
    }
    إذا (STRNCMP(arg, "ALL", where_len) == 0)
    {
	*argp = skipwhite(arg + where_len);
	أرجع DIP_پیل + DIP_OPT;
    }

    أرجع 0;
}

/*
 * ":runtime [where] {name}"
 */
    فراغ
ex_runtime(exarg_T *eap)
{
    char_u  *arg = eap->arg;
    صحيح	    flags = eap->forceit ? DIP_ALL : 0;
    char_u  *p = skiptowhite(arg);
    flags += get_runtime_cmd_flags(&arg, p - arg);
    سرچینه_runtime(arg, flags);
}

ساكن صحيح runtime_expand_flags;

/*
 * Set the completion context for the :runtime command.
 */
    فراغ
سېټ_context_دننه_runtime_cmd(expand_T *xp, char_u *arg)
{
    char_u  *p = skiptowhite(arg);
    runtime_expand_flags
	= *p != NUL ? get_runtime_cmd_flags(&arg, p - arg) : 0;
    // Skip to the last argument.
    طالما (*(p = skiptowhite_esc(arg)) != NUL)
    {
	إذا (runtime_expand_flags == 0)
	    // When there are multiple arguments and [where] is not specified,
	    // use an unrelated non-zero flag to avoid expanding [where].
	    runtime_expand_flags = DIP_ALL;
	arg = skipwhite(p);
    }
    xp->xp_context = EXPAND_RUNTIME;
    xp->xp_نمونه = arg;
}

    ساكن فراغ
سرچینه_callback(char_u *fname, فراغ *cookie)
{
    (فراغ)do_سرچینه(fname, FALSE, DOSO_NONE, cookie);
}

#که_تعریف_شوی FEAT_EVAL
/*
 * Find an already loaded script "name".
 * If found returns its script ID.  If not found returns -1.
 */
    صحيح
پیدا_کړه_script_by_name(char_u *name)
{
    صحيح		    sid;
    scriptitem_T    *si;

    لكل (sid = script_توکي.ga_len; sid > 0; --sid)
    {
	// We used to check inode here, but that doesn't work:
	// - If a script is edited and written, it may get a different
	//   inode number, even though to the user it is the same script.
	// - If a script is deleted and another script is written, with a
	//   different name, the inode may be re-used.
	si = SCRIPT_توکی(sid);
	إذا (si->sn_name != عدم && fnamecmp(si->sn_name, name) == 0)
	    أرجع sid;
    }
    أرجع -1;
}

/*
 * Add a new scriptitem with all items initialized.
 * When running out of memory "error" is set to FAIL.
 * Returns the script ID.
 */
    ساكن صحيح
get_new_scriptitem(صحيح *error)
{
    ساكن scid_T   وروستی_current_SID = 0;
    صحيح		    sid = ++وروستی_current_SID;
    scriptitem_T    *si = عدم;

    إذا (ga_grow(&script_توکي, (صحيح)(sid - script_توکي.ga_len)) == FAIL)
    {
	*error = FAIL;
	أرجع sid;
    }
    طالما (script_توکي.ga_len < sid)
    {
	si = ALLOC_CLEAR_ONE(scriptitem_T);
	إذا (si == عدم)
	{
	    *error = FAIL;
	    أرجع sid;
	}
	++script_توکي.ga_len;
	SCRIPT_توکی(script_توکي.ga_len) = si;
	si->sn_name = عدم;
	si->sn_نسخه = 1;

	// Allocate the local script variables to use for this script.
	new_script_vars(script_توکي.ga_len);
	ga_init2(&si->sn_var_vals, حجم(svar_T), 10);
	hash_init(&si->sn_all_vars.dv_hashtab);
	ga_init2(&si->sn_imports, حجم(imported_T), 10);
	ga_init2(&si->sn_type_list, حجم(type_T), 10);
# ifdef FEAT_PROFILE
	si->sn_prof_on = FALSE;
# endif
    }

    // "si" can't be NULL, check only to avoid a compiler warning
    إذا (si != عدم)
	// Used to check script variable index is still valid.
	si->sn_script_seq = current_sctx.sc_seq;

    أرجع sid;
}

    صحيح
get_new_scriptitem_for_fname(صحيح *error, char_u *fname)
{
    صحيح sid = get_new_scriptitem(error);

    إذا (*error == OK)
    {
	scriptitem_T *si = SCRIPT_توکی(sid);

	si->sn_name = vim_strsave(fname);
	si->sn_state = SN_STATE_NOT_LOADED;
    }
    أرجع sid;
}

/*
 * If the script for "sid" is a symlink and "sn_source_sid" is not set
 * then initialize it. A new script_item is created if needed.
 */
    فراغ
check_script_symlink(صحيح sid)
{
    scriptitem_T *si = SCRIPT_توکی(sid);
    إذا (si->sn_syml_checked || si->sn_sourced_sid > 0)
	أرجع;
    si->sn_syml_checked = TRUE;

    // If fname is a symbolic link, create an script_item for the real file.

    char_u *real_fname = fix_fname(si->sn_name);
    إذا (real_fname != عدم && STRCMP(real_fname, si->sn_name) != 0)
    {
	صحيح real_sid = پیدا_کړه_script_by_name(real_fname);
	صحيح error2 = OK;
	صحيح new_sid = FALSE;
	إذا (real_sid < 0)
	{
	    real_sid = get_new_scriptitem_for_fname(&error2, real_fname);
	    new_sid = TRUE;
	}
	إذا (error2 == OK)
	{
	    si = SCRIPT_توکی(sid);
	    si->sn_sourced_sid = real_sid;
	    إذا (new_sid)
	    {
		SCRIPT_توکی(real_sid)->sn_import_autoload
						    = si->sn_import_autoload;
		إذا (si->sn_autoload_مختاړی != عدم)
		    SCRIPT_توکی(real_sid)->sn_autoload_مختاړی =
					vim_strsave(si->sn_autoload_مختاړی);
	    }
	}
    }
    vim_free(real_fname);
}

    ساكن فراغ
پیدا_کړه_script_callback(char_u *fname, فراغ *cookie)
{
    صحيح sid;
    صحيح error = OK;
    صحيح *ret_sid = cookie;

    sid = پیدا_کړه_script_by_name(fname);
    إذا (sid < 0)
	// script does not exist yet, create a new scriptitem
	sid = get_new_scriptitem_for_fname(&error, fname);
    *ret_sid = sid;
}
#د_که_پای

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
    صحيح
do_دننه_لار(
    char_u	*لار,
    حرف	*مختاړی,
    char_u	*name,
    صحيح		flags,
    فراغ	(*callback)(char_u *fname, فراغ *ck),
    فراغ	*cookie)
{
    char_u	*rtp;
    string_T	buf;
    char_u	*rtp_copy;
    صحيح		عدد_files;
    char_u	**files;
    صحيح		i;
    صحيح		did_one = FALSE;
#که_تعریف_شوی AMIGA
    بنية Process	*proc = (بنية Process *)پیدا_کړه_Task(0L);
    APTR		save_winptr = proc->pr_کړکۍ_Ptr;

    // Avoid a requester here for a volume that doesn't exist.
    proc->pr_کړکۍ_Ptr = (APTR)-1L;
#د_که_پای

    // Make a copy of 'runtimepath'.  Invoking the callback may change the
    // value.
    rtp_copy = vim_strsave(لار);
    buf.string = alloc(MAXPATHL);
    إذا (buf.string != عدم && rtp_copy != عدم)
    {
	size_t	prefixlen = 0;
	size_t	namelen = 0;

	إذا (name != عدم)
	{
	    prefixlen = STRLEN(مختاړی);
	    namelen = STRLEN(name);

	    إذا (p_verbose > 10)
	    {
		verbose_enter();
		إذا (*مختاړی != NUL)
		    smsg(_("Searching for \"%s\" under \"%s\" in \"%s\""),
					       (حرف *)name, مختاړی, (حرف *)لار);
		وإلا
		    smsg(_("Searching for \"%s\" in \"%s\""),
						       (حرف *)name, (حرف *)لار);
		verbose_leave();
	    }
	}

	// Loop over all entries in 'runtimepath'.
	rtp = rtp_copy;
	طالما (*rtp != NUL && ((flags & DIP_ALL) || !did_one))
	{
	    // Copy the path from 'runtimepath' to buf[].
	    buf.اوږدتیا = (size_t)copy_option_part(&rtp, buf.string, MAXPATHL, ",");

	    // Skip after or non-after directories.
	    إذا (flags & (DIP_NOAFTER | DIP_AFTER))
	    {
		صحيح is_after = buf.اوږدتیا >= 5
		    && STRCMP(buf.string + buf.اوږدتیا - 5, "after") == 0;

		إذا ((is_after && (flags & DIP_NOAFTER))
			|| (!is_after && (flags & DIP_AFTER)))
		    استمر;
	    }

	    إذا (name == عدم)
	    {
		(*callback)(buf.string, (فراغ *)&cookie);
		إذا (!did_one)
		    did_one = (cookie == عدم);
	    }
	    وإلا إذا (buf.اوږدتیا + 2 + prefixlen + namelen < MAXPATHL)
	    {
		char_u	*np;
		char_u	*لکۍ;

		إذا (*buf.string != NUL
		    && !after_pathsep(buf.string, buf.string + buf.اوږدتیا))
		{
		    STRCPY(buf.string + buf.اوږدتیا, PATHSEPSTR);
		    buf.اوږدتیا += حجم(PATHSEP);
		}
		STRCPY(buf.string + buf.اوږدتیا, مختاړی);
		buf.اوږدتیا += prefixlen;
		لکۍ = buf.string + buf.اوږدتیا;

		// Loop over all patterns in "name"
		np = name;
		طالما (*np != NUL && ((flags & DIP_ALL) || !did_one))
		{
		    // Append the pattern from "name" to buf[].
		    copy_option_part(&np, لکۍ, (صحيح)(MAXPATHL - (لکۍ - buf.string)),
								       "\t ");

		    إذا (p_verbose > 10)
		    {
			verbose_enter();
			smsg(_("Searching for \"%s\""), buf.string);
			verbose_leave();
		    }

		    // Expand wildcards, invoke the callback for each match.
		    إذا (gen_expand_wildcards(1, &buf.string, &عدد_files, &files,
				  (flags & DIP_DIR) ? EW_DIR : EW_FILE) == OK)
		    {
			لكل (i = 0; i < عدد_files; ++i)
			{
			    (*callback)(files[i], cookie);
			    did_one = TRUE;
			    إذا (!(flags & DIP_ALL))
				اكسر;
			}
			FreeWild(عدد_files, files);
		    }
		}
	    }
	}
    }
    vim_free(buf.string);
    vim_free(rtp_copy);
    إذا (!did_one && name != عدم)
    {
	حرف *basepath = لار == p_rtp ? "runtimepath" : "packpath";

	إذا (flags & DIP_ERR)
	    semsg(_(e_directory_not_وموندل_دننه_str_str), basepath, name);
	وإلا إذا (p_verbose > 0)
	{
	    verbose_enter();
	    smsg(_("not found in '%s': \"%s\""), basepath, name);
	    verbose_leave();
	}
    }

#که_تعریف_شوی AMIGA
    proc->pr_کړکۍ_Ptr = save_winptr;
#د_که_پای

    أرجع did_one ? OK : FAIL;
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
    ساكن صحيح
do_دننه_لار_and_pp(
    char_u	*لار,
    char_u	*name,
    صحيح		flags,
    فراغ	(*callback)(char_u *fname, فراغ *ck),
    فراغ	*cookie)
{
    صحيح		بشپړ = FAIL;

    إذا ((flags & DIP_NORTP) == 0)
	بشپړ = do_دننه_لار(لار, "", name, flags, callback, cookie);

    إذا ((بشپړ == FAIL || (flags & DIP_ALL)) && (flags & DIP_پیل))
	بشپړ = do_دننه_لار(p_pp, "pack/*/start/*/", name, flags, callback,
								       cookie);

    إذا ((بشپړ == FAIL || (flags & DIP_ALL)) && (flags & DIP_OPT))
	بشپړ = do_دننه_لار(p_pp, "pack/*/opt/*/", name, flags, callback,
								       cookie);

    أرجع بشپړ;
}

/*
 * Just like do_in_path_and_pp(), using 'runtimepath' for "path".
 */
    صحيح
do_دننه_runtimepath(
    char_u	*name,
    صحيح		flags,
    فراغ	(*callback)(char_u *fname, فراغ *ck),
    فراغ	*cookie)
{
    أرجع do_دننه_لار_and_pp(p_rtp, name, flags, callback, cookie);
}

/*
 * Source the file "name" from all directories in 'runtimepath'.
 * "name" can contain wildcards.
 * When "flags" has DIP_ALL: source all files, otherwise only the first one.
 *
 * return FAIL when no file could be sourced, OK otherwise.
 */
    صحيح
سرچینه_runtime(char_u *name, صحيح flags)
{
    أرجع سرچینه_دننه_لار(p_rtp, name, flags, عدم);
}

/*
 * Just like source_runtime(), but use "path" instead of 'runtimepath'
 * and return the script ID in "ret_sid".
 */
    صحيح
سرچینه_دننه_لار(char_u *لار, char_u *name, صحيح flags, صحيح *ret_sid)
{
    أرجع do_دننه_لار_and_pp(لار, name, flags, سرچینه_callback, ret_sid);
}

#إذا defined(FEAT_EVAL)

/*
 * Find "name" in 'runtimepath'. If found a new scriptitem is created for it
 * and it's script ID is returned.
 * If not found returns -1.
 */
    صحيح
پیدا_کړه_script_دننه_rtp(char_u *name)
{
    صحيح sid = -1;

    (فراغ)do_دننه_لار_and_pp(p_rtp, name, DIP_پیل | DIP_NOAFTER,
						   پیدا_کړه_script_callback, &sid);
    أرجع sid;
}

/*
 * Expand wildcards in "pat" and invoke do_source() for each match.
 */
    ساكن فراغ
سرچینه_all_matches(char_u *pat)
{
    صحيح	    عدد_files;
    char_u  **files;
    صحيح	    i;

    إذا (gen_expand_wildcards(1, &pat, &عدد_files, &files, EW_FILE) != OK)
	أرجع;

    لكل (i = 0; i < عدد_files; ++i)
	(فراغ)do_سرچینه(files[i], FALSE, DOSO_NONE, عدم);
    FreeWild(عدد_files, files);
}

/*
 * Add the package directory to 'runtimepath'.
 */
    ساكن صحيح
جمع_کړه_pack_dir_منزل_rtp(char_u *fname)
{
    char_u  *p4, *p3, *p2, *p1, *p;
    char_u  *ثبت;
    char_u  *insp = عدم;
    صحيح	    c;
    string_T  new_rtp;
    size_t  keep;
    size_t  p_rtp_len;
    string_T  afterdir = {عدم, 0};
    char_u  *after_insp = عدم;
    char_u  *ffname = عدم;
    size_t  fname_len;
    string_T	buf = {عدم, 0};
    صحيح	    retval = FAIL;

    p4 = p3 = p2 = p1 = get_past_سر(fname);
    لكل (p = p1; *p; MB_PTR_ADV(p))
	إذا (vim_ispathsep_nocolon(*p))
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
    إذا (ffname == عدم)
	أرجع FAIL;

    // Find "ffname" in "p_rtp", ignoring '/' vs '\' differences.
    // Also stop at the first "after" directory.
    fname_len = STRLEN(ffname);
    buf.string = alloc(MAXPATHL);
    إذا (buf.string == عدم)
	اذهب theend;

    p_rtp_len = 0;
    لكل (ثبت = p_rtp; *ثبت != NUL; )
    {
	char_u *cur_ثبت = ثبت;

	buf.اوږدتیا = (size_t)copy_option_part(&ثبت, buf.string, MAXPATHL, ",");

	// keep track of p_rtp length as we go to make the STRLEN() below have less work to do
	p_rtp_len += (*(cur_ثبت + buf.اوږدتیا) == ',') ? buf.اوږدتیا + 1 : buf.اوږدتیا;

	إذا ((p = (char_u *)مزي_پلټنه((حرف *)buf.string, "after")) != عدم
		&& p > buf.string
		&& vim_ispathsep(p[-1])
		&& (vim_ispathsep(p[5]) || p[5] == NUL || p[5] == ','))
	{
	    إذا (insp == عدم)
		// Did not find "ffname" before the first "after" directory,
		// insert it before this entry.
		insp = cur_ثبت;
	    after_insp = cur_ثبت;
	    اكسر;
	}

	إذا (insp == عدم)
	{
	    char_u  *rtp_ffname;
	    صحيح	    سمون;

	    إذا (*buf.string != NUL && !after_pathsep(buf.string, buf.string + buf.اوږدتیا))
		STRCPY(buf.string + buf.اوږدتیا, PATHSEPSTR);
	    rtp_ffname = fix_fname(buf.string);
	    إذا (rtp_ffname == عدم)
		اذهب theend;
	    سمون = vim_fnamencmp(rtp_ffname, ffname, fname_len) == 0;
	    vim_free(rtp_ffname);
	    إذا (سمون)
		// Insert "ffname" after this entry (and comma).
		insp = ثبت;
	}
    }

    // finish measuring the length of p_rtp
    p_rtp_len += STRLEN(p_rtp + p_rtp_len);
    إذا (insp == عدم)
	// Both "fname" and "after" not found, append at the end.
	insp = p_rtp + p_rtp_len;

    // check if rtp/pack/name/start/name/after exists
    fname_len = STRLEN(fname);
    concat_fnames(fname, fname_len, (char_u *)"after", STRLEN_LITERAL("after"), TRUE, &afterdir);
    إذا (afterdir.string == عدم || !mch_isdir(afterdir.string))
	afterdir.اوږدتیا = 0;

    new_rtp.string = alloc(p_rtp_len + fname_len + afterdir.اوږدتیا + 3); // add two for commas and one for NUL
    إذا (new_rtp.string == عدم)
	اذهب theend;

    // We now have 'rtp' parts: {keep}{keep_after}{rest}.
    // Create new_rtp, first: {keep},{fname}
    keep = (size_t)(insp - p_rtp);
    mch_memmove(new_rtp.string, p_rtp, keep);
    new_rtp.اوږدتیا = keep;
    إذا (*insp == NUL)
	new_rtp.string[new_rtp.اوږدتیا++] = ',';  // add comma before
    mch_memmove(new_rtp.string + new_rtp.اوږدتیا, fname, fname_len);
    new_rtp.اوږدتیا += fname_len;
    إذا (*insp != NUL)
	new_rtp.string[new_rtp.اوږدتیا++] = ',';  // add comma after

    إذا (afterdir.اوږدتیا > 0 && after_insp != عدم)
    {
	size_t keep_after = (size_t)(after_insp - p_rtp);
	size_t append_len = keep_after - keep;

	// Add to new_rtp: {keep},{fname}{keep_after},{afterdir}
	mch_memmove(new_rtp.string + new_rtp.اوږدتیا, p_rtp + keep,
							append_len);
	new_rtp.اوږدتیا += append_len;
	mch_memmove(new_rtp.string + new_rtp.اوږدتیا, afterdir.string, afterdir.اوږدتیا);
	new_rtp.اوږدتیا += afterdir.اوږدتیا;
	new_rtp.string[new_rtp.اوږدتیا++] = ',';
	keep = keep_after;
    }

    إذا (p_rtp[keep] != NUL)
    {
	size_t	append_len = p_rtp_len - keep;

	// Append rest: {keep},{fname}{keep_after},{afterdir}{rest}
	mch_memmove(new_rtp.string + new_rtp.اوږدتیا, p_rtp + keep, append_len + 1);	// add one for NUL
	new_rtp.اوږدتیا += append_len;
    }
    وإلا
	new_rtp.string[new_rtp.اوږدتیا] = NUL;

    إذا (afterdir.اوږدتیا > 0 && after_insp == عدم)
    {
	// Append afterdir when "after" was not found:
	// {keep},{fname}{rest},{afterdir}
	new_rtp.string[new_rtp.اوږدتیا++] = ',';
	STRCPY(new_rtp.string + new_rtp.اوږدتیا, afterdir.string);
    }

    سېټ_option_ارزښت_give_err((char_u *)"rtp", 0L, new_rtp.string, 0);
    vim_free(new_rtp.string);
    retval = OK;

theend:
    vim_free(buf.string);
    vim_free(ffname);
    vim_free(afterdir.string);
    أرجع retval;
}

/*
 * Load scripts in "plugin" and "ftdetect" directories of the package.
 */
    ساكن صحيح
بارول_pack_plugin(char_u *fname)
{
    ساكن حرف *plugpat = "%s/plugin/**/*.vim";
    ساكن حرف *ftpat = "%s/ftdetect/*.vim";
    صحيح		len;
    char_u	*ffname = fix_fname(fname);
    char_u	*pat = عدم;
    صحيح		retval = FAIL;

    إذا (ffname == عدم)
	أرجع FAIL;
    len = (صحيح)STRLEN(ffname) + (صحيح)STRLEN(ftpat);
    pat = alloc(len);
    إذا (pat == عدم)
	اذهب theend;
    vim_snprintf((حرف *)pat, len, plugpat, ffname);
    سرچینه_all_matches(pat);

    {
	char_u *cmd = vim_strsave((char_u *)"g:did_load_filetypes");

	// If runtime/filetype.vim wasn't loaded yet, the scripts will be
	// found when it loads.
	إذا (cmd != عدم && eval_منزل_شمېره(cmd, FALSE) > 0)
	{
	    do_cmdline_cmd((char_u *)"augroup filetypedetect");
	    vim_snprintf((حرف *)pat, len, ftpat, ffname);
	    سرچینه_all_matches(pat);
	    do_cmdline_cmd((char_u *)"augroup END");
	}
	vim_free(cmd);
    }
    vim_free(pat);
    retval = OK;

theend:
    vim_free(ffname);
    أرجع retval;
}

// used for "cookie" of add_pack_plugin()
ساكن صحيح APP_جمع_کړه_DIR;
ساكن صحيح APP_بارول;
ساكن صحيح APP_BOTH;

    ساكن فراغ
جمع_کړه_pack_plugin(char_u *fname, فراغ *cookie)
{
    إذا (cookie != &APP_بارول)
    {
	char_u	*buf = alloc(MAXPATHL);
	char_u	*p;
	صحيح	وموندل = FALSE;

	إذا (buf == عدم)
	    أرجع;
	p = p_rtp;
	طالما (*p != NUL)
	{
	    copy_option_part(&p, buf, MAXPATHL, ",");
	    إذا (pathcmp((حرف *)buf, (حرف *)fname, -1) == 0)
	    {
		وموندل = TRUE;
		اكسر;
	    }
	}
	vim_free(buf);
	إذا (!وموندل)
	    // directory is not yet in 'runtimepath', add it
	    إذا (جمع_کړه_pack_dir_منزل_rtp(fname) == FAIL)
		أرجع;
    }

    إذا (cookie != &APP_جمع_کړه_DIR)
	بارول_pack_plugin(fname);
}

/*
 * Add all packages in the "start" directory to 'runtimepath'.
 */
    فراغ
جمع_کړه_pack_پیل_dirs(فراغ)
{
    do_دننه_لار(p_pp, "", (char_u *)"pack/*/start/*", DIP_ALL + DIP_DIR,
					       جمع_کړه_pack_plugin, &APP_جمع_کړه_DIR);
}

/*
 * Load plugins from all packages in the "start" directory.
 */
    فراغ
بارول_پیل_packages(فراغ)
{
    did_سرچینه_packages = TRUE;
    do_دننه_لار(p_pp, "", (char_u *)"pack/*/start/*", DIP_ALL + DIP_DIR,
						  جمع_کړه_pack_plugin, &APP_بارول);
}

/*
 * ":packloadall"
 * Find plugins in the package directories and source them.
 */
    فراغ
ex_packloadall(exarg_T *eap)
{
    إذا (!did_سرچینه_packages || eap->forceit)
    {
	// First do a round to add all directories to 'runtimepath', then load
	// the plugins. This allows for plugins to use an autoload directory
	// of another plugin.
	جمع_کړه_pack_پیل_dirs();
	بارول_پیل_packages();
    }
}

/*
 * ":packadd[!] {name}"
 */
    فراغ
ex_packadd(exarg_T *eap)
{
    ساكن حرف *plugpat = "pack/*/%s/%s";
    صحيح		len;
    حرف	*pat;
    صحيح		پړاو;
    صحيح		res = OK;

    // Round 1: use "start", round 2: use "opt".
    لكل (پړاو = 1; پړاو <= 2; ++پړاو)
    {
	// Only look under "start" when loading packages wasn't done yet.
	إذا (پړاو == 1 && did_سرچینه_packages)
	    استمر;

	len = (صحيح)STRLEN(plugpat) + (صحيح)STRLEN(eap->arg) + 5;
	pat = alloc(len);
	إذا (pat == عدم)
	    أرجع;
	vim_snprintf(pat, len, plugpat, پړاو == 1 ? "start" : "opt", eap->arg);
	// The first round don't give a "not found" error, in the second round
	// only when nothing was found in the first round.
	res = do_دننه_لار(p_pp, "", (char_u *)pat,
		DIP_ALL + DIP_DIR + (پړاو == 2 && res == FAIL ? DIP_ERR : 0),
		جمع_کړه_pack_plugin, eap->forceit ? &APP_جمع_کړه_DIR : &APP_BOTH);
	vim_free(pat);
    }
}
#د_که_پای

/*
 * Sort "gap" and remove duplicate entries.  "gap" is expected to contain a
 * list of file names in allocated memory.
 */
    فراغ
remove_duplicates(garray_T *gap)
{
    صحيح	    i;
    صحيح	    j;
    char_u  **fnames = (char_u **)gap->ga_معلومات;

    ترتیبول_مزي_ګانې(fnames, gap->ga_len);
    لكل (i = gap->ga_len - 1; i > 0; --i)
	إذا (fnamecmp(fnames[i - 1], fnames[i]) == 0)
	{
	    vim_free(fnames[i]);
	    لكل (j = i + 1; j < gap->ga_len; ++j)
		fnames[j - 1] = fnames[j];
	    --gap->ga_len;
	}
}

    ساكن فراغ
ExpandRTDir_int(
    char_u	*pat,
    size_t	pat_len,
    صحيح		flags,
    صحيح		keep_ext,
    garray_T	*gap,
    حرف	*dirnames[])
{
    لكل (صحيح i = 0; dirnames[i] != عدم; ++i)
    {
	ثابت size_t	buf_len = STRLEN(dirnames[i]) + pat_len + 64;
	حرف		*buf = alloc(buf_len);
	إذا (buf == عدم)
	{
	    ga_clear_مزي_ګانې(gap);
	    أرجع;
	}
	صحيح		glob_flags = 0;
	صحيح		expand_dirs = FALSE;

	// Build base pattern
	vim_snprintf(buf, buf_len, "%s%s%s%s",
		     *dirnames[i] ? dirnames[i] : "", *dirnames[i] ? "/" : "",
		     pat, "*.vim");

expand:
	إذا ((flags & DIP_NORTP) == 0)
	    globpath(p_rtp, (char_u *)buf, gap, glob_flags, expand_dirs);

	إذا (flags & DIP_پیل)
	{
	    // Build complete search path: pack/*/start/*/dirnames[i]/pat*.vim
	    vim_snprintf(buf, buf_len, "pack/*/start/*/%s%s%s%s",
			 *dirnames[i] ? dirnames[i] : "",
			 *dirnames[i] ? "/" : "",
			 pat,
			 expand_dirs ? "*" : "*.vim");
	    globpath(p_pp, (char_u *)buf, gap, glob_flags, expand_dirs);
	}

	إذا (flags & DIP_OPT)
	{
	    // Build complete search path: pack/*/opt/*/dirnames[i]/pat*.vim
	    vim_snprintf(buf, buf_len, "pack/*/opt/*/%s%s%s%s",
			 *dirnames[i] ? dirnames[i] : "",
			 *dirnames[i] ? "/" : "", pat,
			 expand_dirs ? "*" : "*.vim");
	    globpath(p_pp, (char_u *)buf, gap, glob_flags, expand_dirs);
	}

	// Second round for directories
	إذا (*dirnames[i] == NUL && !expand_dirs)
	{
	    // expand dir names in another round
	    vim_snprintf(buf, buf_len, "%s*", pat);
	    glob_flags = WILD_جمع_کړه_SLASH;
	    expand_dirs = TRUE;
	    اذهب expand;
	}

	vim_free(buf);
    }

    صحيح pat_pathsep_cnt = 0;
    لكل (size_t i = 0; i < pat_len; ++i)
    {
	إذا (vim_ispathsep(pat[i]))
	    ++pat_pathsep_cnt;
    }

    لكل (صحيح i = 0; i < gap->ga_len; ++i)
    {
	char_u *سمون = ((char_u **)gap->ga_معلومات)[i];
	char_u *s = سمون;
	char_u *e = s + STRLEN(s);
	إذا (e - 4 > s && !keep_ext && STRNICMP(e - 4, ".vim", 4) == 0)
	{
	    e -= 4;
	    *e = NUL;
	}

	صحيح سمون_pathsep_cnt = (e > s && e[-1] == '/') ? -1 : 0;
	لكل (s = e; s > سمون; MB_PTR_شا(سمون, s))
	{
	    إذا (s < سمون || (vim_ispathsep(*s)
				     && ++سمون_pathsep_cnt > pat_pathsep_cnt))
		اكسر;
	}
	++s;
	إذا (s != سمون)
	    mch_memmove(سمون, s, e - s + 1);
    }

    إذا (gap->ga_len == 0)
	أرجع;

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
    صحيح
ExpandRTDir(
    char_u	*pat,
    صحيح		flags,
    صحيح		*عدد_file,
    char_u	***file,
    حرف	*dirnames[])
{
    *عدد_file = 0;
    *file = عدم;

    garray_T	ga;
    ga_init2(&ga, حجم(حرف *), 10);

    ExpandRTDir_int(pat, STRLEN(pat), flags, FALSE, &ga, dirnames);

    إذا (ga.ga_len == 0)
	أرجع FAIL;

    *file = ga.ga_معلومات;
    *عدد_file = ga.ga_len;
    أرجع OK;
}

/*
 * Handle command line completion for :runtime command.
 */
    صحيح
expand_runtime_cmd(char_u *pat, صحيح *عدد_Matches, char_u ***matches)
{
    *عدد_Matches = 0;
    *matches = عدم;

    garray_T	ga;
    ga_init2(&ga, حجم(حرف *), 10);

    size_t pat_len = (صحيح)STRLEN(pat);
    حرف *dirnames[] = {"", عدم};
    ExpandRTDir_int(pat, pat_len, runtime_expand_flags, TRUE, &ga, dirnames);

    // Try to complete values for [where] argument when none was found.
    إذا (runtime_expand_flags == 0)
    {
	حرف *where_ارزښتونه[] = {"START", "OPT", "PACK", "ALL"};
	لكل (size_t i = 0; i < ARRAY_اوږدتیا(where_ارزښتونه); ++i)
	    إذا (STRNCMP(pat, where_ارزښتونه[i], pat_len) == 0)
	    {
		char_u *p = vim_strsave((char_u *)where_ارزښتونه[i]);
		إذا (p != عدم && ga_جمع_کړه_string(&ga, p) == FAIL)
		    vim_free(p);
	    }
    }

    إذا (ga.ga_len == 0)
	أرجع FAIL;

    *matches = ga.ga_معلومات;
    *عدد_Matches = ga.ga_len;
    أرجع OK;
}

/*
 * Expand loadplugin names:
 * 'packpath'/pack/ * /opt/{pat}
 */
    صحيح
Expand_Pack_جمع_کړه_Dir(
    char_u	*pat,
    صحيح		*عدد_file,
    char_u	***file)
{
    char_u	*s;
    char_u	*e;
    char_u	*سمون;
    garray_T	ga;
    صحيح		i;
    صحيح		pat_len;

    *عدد_file = 0;
    *file = عدم;
    pat_len = (صحيح)STRLEN(pat);
    ga_init2(&ga, حجم(حرف *), 10);

    s = alloc(pat_len + 26);
    إذا (s == عدم)
    {
	ga_clear_مزي_ګانې(&ga);
	أرجع FAIL;
    }
    sprintf((حرف *)s, "pack/*/opt/%s*", pat);
    globpath(p_pp, s, &ga, 0, TRUE);
    vim_free(s);

    لكل (i = 0; i < ga.ga_len; ++i)
    {
	سمون = ((char_u **)ga.ga_معلومات)[i];
	s = gettail(سمون);
	e = s + STRLEN(s);
	mch_memmove(سمون, s, e - s + 1);
    }

    إذا (ga.ga_len == 0)
	أرجع FAIL;

    // Sort and remove duplicates which can happen when specifying multiple
    // directories in dirnames.
    remove_duplicates(&ga);

    *file = ga.ga_معلومات;
    *عدد_file = ga.ga_len;
    أرجع OK;
}

    ساكن فراغ
cmd_سرچینه(char_u *fname, exarg_T *eap)
{
    صحيح clearvars = FALSE;

    إذا (*fname != NUL && STRNCMP(fname, "++clear", 7) == 0)
    {
	// ++clear argument is supplied
	clearvars = TRUE;
	fname = fname + 7;
	إذا (*fname != NUL)
	{
	    semsg(_(e_invalid_دلیل_str), eap->arg);
	    أرجع;
	}
    }

    إذا (*fname != NUL && eap != عدم && eap->addr_شمېر > 0)
    {
	// if a filename is specified to :source, then a range is not allowed
	emsg(_(e_no_range_allowed));
	أرجع;
    }

    إذا (eap != عدم && *fname == NUL)
    {
	إذا (eap->forceit)
	    // a file name is needed to source normal mode commands
	    emsg(_(e_دلیل_required));
	وإلا
	    // source ex commands from the current buffer
	    do_سرچینه_ext(عدم, FALSE, DOSO_NONE, عدم, eap, clearvars);
    }
    وإلا إذا (eap != عدم && eap->forceit)
	// ":source!": read Normal mode commands
	// Need to execute the commands directly.  This is required at least
	// for:
	// - ":g" command busy
	// - after ":argdo", ":windo" or ":bufdo"
	// - another command follows
	// - inside a loop
	openscript(fname, global_busy || listcmd_busy || eap->nextcmd != عدم
#که_تعریف_شوی FEAT_EVAL
						 || eap->cstack->cs_idx >= 0
#د_که_پای
						 );

    // ":source" read ex commands
    وإلا إذا (do_سرچینه(fname, FALSE, DOSO_NONE, عدم) == FAIL)
	semsg(_(e_cant_open_file_str), fname);
}

/*
 * ":source {fname}"
 */
    فراغ
ex_سرچینه(exarg_T *eap)
{
#که_تعریف_شوی FEAT_BROWSE
    إذا (cmdmod.cmod_flags & CMOD_BROWSE)
    {
	char_u *fname = عدم;

	fname = do_browse(0, (char_u *)_("Source Vim script"), eap->arg,
				      عدم, عدم,
				      (char_u *)_(BROWSE_FILTER_MACROS), عدم);
	إذا (fname != عدم)
	{
	    cmd_سرچینه(fname, eap);
	    vim_free(fname);
	}
    }
    وإلا
#د_که_پای
	cmd_سرچینه(eap->arg, eap);
}

#إذا defined(FEAT_EVAL)
/*
 * ":options"
 */
    فراغ
ex_options(
    exarg_T	*eap ناکارول_شوی)
{
    char_u  buf[500];
    صحيح	    multi_mods = 0;

    buf[0] = NUL;
    (فراغ)جمع_کړه_win_cmd_modifiers(buf, &cmdmod, &multi_mods);

    vim_setenv((char_u *)"OPTWIN_CMD", buf);
    cmd_سرچینه((char_u *)SYS_OPTWIN_FILE, عدم);
}
#د_که_پای

/*
 * ":source" and associated commands.
 */

#إذا defined(FEAT_EVAL)
/*
 * Return the address holding the next breakpoint line for a source cookie.
 */
    linenr_T *
سرچینه_breakpoint(فراغ *cookie)
{
    أرجع &((سرچینه_cookie_T *)cookie)->breakpoint;
}

/*
 * Return the address holding the debug tick for a source cookie.
 */
    صحيح *
سرچینه_dbg_tick(فراغ *cookie)
{
    أرجع &((سرچینه_cookie_T *)cookie)->dbg_tick;
}

/*
 * Return the nesting level for a source cookie.
 */
    صحيح
سرچینه_level(فراغ *cookie)
{
    أرجع ((سرچینه_cookie_T *)cookie)->level;
}

/*
 * Return the readahead line. Note that the pointer may become invalid when
 * getting the next line, if it's concatenated with the next one.
 */
    char_u *
سرچینه_nextline(فراغ *cookie)
{
    أرجع ((سرچینه_cookie_T *)cookie)->nextline;
}
#د_که_پای

#إذا (defined(MSWIN) && defined(FEAT_CSCOPE)) || defined(HAVE_FD_CLOEXEC)
# define USE_FOPEN_NOINH
/*
 * Special function to open a file without handle inheritance.
 * When possible the handle is closed on exec().
 */
    ساكن FILE *
fopen_noinh_readbin(حرف *filename)
{
# ifdef MSWIN
    صحيح	fd_tmp = mch_open(filename, O_RDONLY | O_دوګونی | O_NOINHERIT, 0);
# وإلا
    صحيح	fd_tmp = mch_open(filename, O_RDONLY, 0);
# endif

    إذا (fd_tmp == -1)
	أرجع عدم;

# ifdef HAVE_FD_CLOEXEC
    {
	صحيح fdflags = fcntl(fd_tmp, F_GETFD);
	إذا (fdflags >= 0 && (fdflags & FD_CLOEXEC) == 0)
	    (فراغ)fcntl(fd_tmp, F_SETFD, fdflags | FD_CLOEXEC);
    }
# endif

    أرجع fdopen(fd_tmp, READBIN);
}
#د_که_پای

/*
 * Initialization for sourcing lines from the current buffer. Reads all the
 * lines from the buffer and stores it in the cookie grow array.
 * Returns a pointer to the name ":source buffer=<n>" on success and NULL on
 * failure.
 */
    ساكن char_u *
do_سرچینه_بفر_init(سرچینه_cookie_T *sp, exarg_T *eap)
{
    linenr_T	curr_lnum;
    char_u	*کرښه = عدم;
    char_u	*fname;

    إذا (curbuf == عدم)
	أرجع عدم;

    // Use ":source buffer=<num>" as the script name
    إذا (curbuf->b_ffname != عدم)
	fname = vim_strsave(curbuf->b_ffname);
    وإلا
    {
	vim_snprintf((حرف *)IObuff, IOSIZE, ":source buffer=%d", curbuf->b_fnum);
	fname = vim_strsave(IObuff);
    }
    إذا (fname == عدم)
	أرجع عدم;

    ga_init2(&sp->buflines, حجم(char_u *), 100);

    // Copy the lines from the buffer into a grow array
    لكل (curr_lnum = eap->line1; curr_lnum <= eap->line2; curr_lnum++)
    {
	کرښه = vim_strsave(ml_get(curr_lnum));
	إذا (کرښه == عدم)
	    اذهب errret;
	إذا (ga_جمع_کړه_string(&sp->buflines, کرښه) == FAIL)
	    اذهب errret;
    }
    sp->buf_lnum = 0;
    sp->سرچینه_from_buf = TRUE;
    // When sourcing a range of lines from a buffer, use buffer line number.
    sp->sourcing_lnum = eap->line1 - 1;

    أرجع fname;

errret:
    vim_free(fname);
    vim_free(کرښه);
    ga_clear_مزي_ګانې(&sp->buflines);
    أرجع عدم;
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
    ساكن صحيح
do_سرچینه_ext(
    char_u	*fname,
    صحيح		check_other,	    // check for .vimrc and _vimrc
    صحيح		is_vimrc,	    // DOSO_ value
    صحيح		*ret_sid ناکارول_شوی,
    exarg_T	*eap,
    صحيح		clearvars ناکارول_شوی)
{
    سرچینه_cookie_T	    cookie;
    char_u		    *p;
    char_u		    *fname_not_fixed = عدم;
    char_u		    *fname_exp = عدم;
    char_u		    *firstline = عدم;
    صحيح			    retval = FAIL;
    sctx_T		    save_current_sctx;
#که_تعریف_شوی STARTUPTIME
    بنية timeval	    tv_rel;
    بنية timeval	    tv_پیل;
#د_که_پای
#که_تعریف_شوی FEAT_PROFILE
    proftime_T		    wait_پیل;
#د_که_پای
    صحيح			    save_sticky_cmdmod_flags = sticky_cmdmod_flags;
    صحيح			    trigger_سرچینه_post = FALSE;
#که_تعریف_شوی FEAT_EVAL
    funccal_ثبت_T	    funccalp_ثبت;
    صحيح			    save_debug_break_level = debug_break_level;
    صحيح			    sid = -1;
    scriptitem_T	    *si = عدم;
    صحيح			    save_estack_compiling = estack_compiling;
    ESTACK_CHECK_DECLARATION;
#د_که_پای

    CLEAR_ډګر(cookie);
    إذا (fname == عدم)
    {
	// sourcing lines from a buffer
	fname_exp = do_سرچینه_بفر_init(&cookie, eap);
	إذا (fname_exp == عدم)
	    أرجع FAIL;
    }
    وإلا
    {
	fname_not_fixed = expand_env_save(fname);
	إذا (fname_not_fixed == عدم)
	    اذهب theend;
	fname_exp = fix_fname(fname_not_fixed);
	إذا (fname_exp == عدم)
	    اذهب theend;
	إذا (mch_isdir(fname_exp))
	{
	    smsg(_("Cannot source a directory: \"%s\""), fname);
	    اذهب theend;
	}
    }
#که_تعریف_شوی FEAT_EVAL
    estack_compiling = FALSE;

    // See if we loaded this script before.
    sid = پیدا_کړه_script_by_name(fname_exp);
    إذا (sid > 0 && ret_sid != عدم
			  && SCRIPT_توکی(sid)->sn_state != SN_STATE_NOT_LOADED)
    {
	// Already loaded and no need to load again, return here.
	*ret_sid = sid;
	retval = OK;
	اذهب theend;
    }
#د_که_پای

    // Apply SourceCmd autocommands, they should get the file and source it.
    // When sourcing a file "fname_exp" was made a full path name by
    // fix_fname() above, avoid expanding it again.
    إذا ((fname == عدم
		? has_autocmd(EVENT_SOURCECMD, fname_exp, عدم)
		: has_autocmd_fullname(EVENT_SOURCECMD, fname_exp, عدم))
	    && apply_autocmds(EVENT_SOURCECMD, fname_exp, fname_exp,
							       FALSE, curbuf))
    {
#که_تعریف_شوی FEAT_EVAL
	retval = aborting() ? FAIL : OK;
#وإلا
	retval = OK;
#د_که_پای
	إذا (retval == OK)
	    // Apply SourcePost autocommands.
	    apply_autocmds(EVENT_SOURCEPOST, fname_exp, fname_exp,
								FALSE, curbuf);
	اذهب theend;
    }

    // Apply SourcePre autocommands, they may get the file.
    apply_autocmds(EVENT_SOURCEPRE, fname_exp, fname_exp, FALSE, curbuf);

    إذا (!cookie.سرچینه_from_buf)
    {
#که_تعریف_شوی USE_FOPEN_NOINH
	cookie.fp = fopen_noinh_readbin((حرف *)fname_exp);
#وإلا
	cookie.fp = mch_fopen((حرف *)fname_exp, READBIN);
#د_که_پای
    }
    إذا (cookie.fp == عدم && check_other)
    {
	// Try again, replacing file name ".vimrc" by "_vimrc" or vice versa,
	// and ".exrc" by "_exrc" or vice versa.
	p = gettail(fname_exp);
	إذا ((*p == '.' || *p == '_')
		&& (STRICMP(p + 1, "vimrc") == 0
		    || STRICMP(p + 1, "gvimrc") == 0
		    || STRICMP(p + 1, "exrc") == 0))
	{
	    إذا (*p == '_')
		*p = '.';
	    وإلا
		*p = '_';
#که_تعریف_شوی USE_FOPEN_NOINH
	    cookie.fp = fopen_noinh_readbin((حرف *)fname_exp);
#وإلا
	    cookie.fp = mch_fopen((حرف *)fname_exp, READBIN);
#د_که_پای
	}
    }

    إذا (cookie.fp == عدم && !cookie.سرچینه_from_buf)
    {
	إذا (p_verbose > 0)
	{
	    verbose_enter();
	    إذا (SOURCING_NAME == عدم)
		smsg(_("could not source \"%s\""), fname);
	    وإلا
		smsg(_("line %ld: could not source \"%s\""),
							SOURCING_LNUM, fname);
	    verbose_leave();
	}
	اذهب theend;
    }

    // The file exists.
    // - In verbose mode, give a message.
    // - For a vimrc file, may want to set 'compatible', call vimrc_found().
    إذا (p_verbose > 1)
    {
	verbose_enter();
	إذا (SOURCING_NAME == عدم)
	    smsg(_("sourcing \"%s\""), fname);
	وإلا
	    smsg(_("line %ld: sourcing \"%s\""), SOURCING_LNUM, fname);
	verbose_leave();
    }
    إذا (is_vimrc == DOSO_VIMRC)
	vimrc_وموندل(fname_exp, (char_u *)"MYVIMRC");
    وإلا إذا (is_vimrc == DOSO_GVIMRC)
	vimrc_وموندل(fname_exp, (char_u *)"MYGVIMRC");

#که_تعریف_شوی USE_CRNL
    // If no automatic file format: Set default to CR-NL.
    إذا (*p_ffs == NUL)
	cookie.fileformat = EOL_DOS;
    وإلا
	cookie.fileformat = EOL_UNKNOWN;
#د_که_پای

#که_تعریف_شوی FEAT_EVAL
    // Check if this script has a breakpoint.
    cookie.breakpoint = dbg_پیدا_کړه_breakpoint(TRUE, fname_exp, (linenr_T)0);
    cookie.fname = fname_exp;
    cookie.dbg_tick = debug_tick;

    cookie.level = ex_nesting_level;
#د_که_پای

#که_تعریف_شوی STARTUPTIME
    إذا (وخت_fd != عدم)
	وخت_ټېل(&tv_rel, &tv_پیل);
#د_که_پای

    // "legacy" does not apply to commands in the script
    sticky_cmdmod_flags = 0;

    save_current_sctx = current_sctx;
    إذا ((cmdmod.cmod_flags & CMOD_VIM9CMD) && cookie.سرچینه_from_buf)
	// When the ":vim9cmd" command modifier is used, source buffer lines as
	// Vim9 script
	current_sctx.sc_نسخه = SCRIPT_نسخه_VIM9;
    وإلا
	current_sctx.sc_نسخه = 1;  // default script version

#که_تعریف_شوی FEAT_EVAL
    current_sctx.sc_lnum = 0;

# ifdef FEAT_PROFILE
    إذا (do_profiling == PROF_YES)
	prof_ماشوم_enter(&wait_پیل);		// entering a child now
# endif

    // Don't use local function variables, if called from a function.
    // Also starts profiling timer for nested script.
    save_funccal(&funccalp_ثبت);

    // Reset "KeyTyped" to avoid some commands thinking they are invoked
    // interactively.  E.g. defining a function would output indent.
    صحيح save_کلی_Typed = کلی_Typed;
    کلی_Typed = FALSE;

    // Check if this script was sourced before to find its SID.
    // Always use a new sequence number.
    current_sctx.sc_seq = ++وروستی_current_SID_seq;
    إذا (sid > 0)
    {
	hashtab_T	*ht;
	صحيح		todo;
	hashitem_T	*hi;
	dictitem_T	*di;

	// loading the same script again
	current_sctx.sc_sid = sid;
	si = SCRIPT_توکی(sid);
	إذا (si->sn_state == SN_STATE_NOT_LOADED)
	{
	    // this script was found but not loaded yet
	    si->sn_state = SN_STATE_NEW;
	}
	وإلا
	{
	    si->sn_state = SN_STATE_RELOAD;

	    إذا (!clearvars)
	    {
		// Script-local variables remain but "const" can be set again.
		// In Vim9 script variables will be cleared when "vim9script"
		// is encountered without the "noclear" argument.
		ht = &SCRIPT_VARS(sid);
		todo = (صحيح)ht->ht_کارول_شوی;
		FOR_ALL_HASHTAB_توکي(ht, hi, todo)
		    إذا (!HASHITEM_تشه(hi))
		    {
			--todo;
			di = HI2DI(hi);
			di->di_flags |= DI_FLAGS_RELOAD;
		    }
		// imports can be redefined once
		mark_imports_for_reload(sid);
	    }
	    وإلا
		clear_vim9_scriptlocal_vars(sid);

	    // reset version, "vim9script" may have been added or removed.
	    si->sn_نسخه = 1;
	}
	إذا (ret_sid != عدم)
	    *ret_sid = sid;
    }
    وإلا
    {
	صحيح error = OK;

	// It's new, generate a new SID and initialize the scriptitem.
	sid = get_new_scriptitem(&error);
	current_sctx.sc_sid = sid;
	إذا (error == FAIL)
	    اذهب almosttheend;
	si = SCRIPT_توکی(sid);
	si->sn_name = fname_exp;
	fname_exp = vim_strsave(si->sn_name);  // used for autocmd
	إذا (ret_sid != عدم)
	    *ret_sid = sid;

	// Remember the "is_vimrc" flag for when the file is sourced again.
	si->sn_is_vimrc = is_vimrc;
    }

    // Keep the sourcing name/lnum, for recursive calls.
    estack_ټېل(ETYPE_SCRIPT, si->sn_name, 0);
    ESTACK_CHECK_تیاری;

# ifdef FEAT_PROFILE
    إذا (do_profiling == PROF_YES)
    {
	صحيح	forceit;

	// Check if we do profiling for this script.
	إذا (!si->sn_prof_on && has_profiling(TRUE, si->sn_name, &forceit, عدم))
	{
	    script_do_profile(si);
	    si->sn_pr_force = forceit;
	}
	إذا (si->sn_prof_on)
	{
	    ++si->sn_pr_شمېر;
	    profile_پیل(&si->sn_pr_پیل);
	    profile_zero(&si->sn_pr_children);
	}
    }
# endif
#وإلا
    // Keep the sourcing name/lnum, for recursive calls.
    estack_ټېل(ETYPE_SCRIPT, fname_exp, 0);
#د_که_پای

    cookie.conv.vc_type = CONV_NONE;		// no conversion

    // Read the first line so we can check for a UTF-8 BOM.
    firstline = getsourceline(0, (فراغ *)&cookie, 0, TRUE);
    إذا (firstline != عدم && STRLEN(firstline) >= 3 && firstline[0] == 0xef
			      && firstline[1] == 0xbb && firstline[2] == 0xbf)
    {
	// Found BOM; setup conversion, skip over BOM and recode the line.
	convert_تیاری(&cookie.conv, (char_u *)"utf-8", p_enc);
	p = string_convert(&cookie.conv, firstline + 3, عدم);
	إذا (p == عدم)
	    p = vim_strsave(firstline + 3);
	إذا (p != عدم)
	{
	    vim_free(firstline);
	    firstline = p;
	}
    }

    // Call do_cmdline, which will call getsourceline() to get the lines.
    do_cmdline(firstline, getsourceline, (فراغ *)&cookie,
				     DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_REPEAT);
    retval = OK;

#که_تعریف_شوی FEAT_PROFILE
    إذا (do_profiling == PROF_YES)
    {
	// Get "si" again, "script_items" may have been reallocated.
	si = SCRIPT_توکی(sid);
	إذا (si->sn_prof_on)
	{
	    profile_پای_ټکی(&si->sn_pr_پیل);
	    profile_sub_wait(&wait_پیل, &si->sn_pr_پیل);
	    profile_جمع_کړه(&si->sn_pr_ټول, &si->sn_pr_پیل);
	    profile_self(&si->sn_pr_self, &si->sn_pr_پیل,
							 &si->sn_pr_children);
	}
    }
#د_که_پای

    إذا (got_int)
	emsg(_(e_interrupted));
#که_تعریف_شوی FEAT_EVAL
    ESTACK_CHECK_NOW;
#د_که_پای
    estack_راوباسه();
    إذا (p_verbose > 1)
    {
	verbose_enter();
	smsg(_("finished sourcing %s"), fname);
	إذا (SOURCING_NAME != عدم)
	    smsg(_("continuing in %s"), SOURCING_NAME);
	verbose_leave();
    }
#که_تعریف_شوی STARTUPTIME
    إذا (وخت_fd != عدم)
    {
	vim_snprintf((حرف *)IObuff, IOSIZE, "sourcing %s", fname);
	وخت_پیغام_2((حرف *)IObuff, &tv_پیل);
	وخت_راوباسه(&tv_rel);
    }
#د_که_پای

    إذا (!got_int)
	trigger_سرچینه_post = TRUE;

#که_تعریف_شوی FEAT_EVAL
    // After a "finish" in debug mode, need to break at first command of next
    // sourced file.
    إذا (save_debug_break_level > ex_nesting_level
	    && debug_break_level == ex_nesting_level)
	++debug_break_level;
#د_که_پای

#که_تعریف_شوی FEAT_EVAL
almosttheend:
    // If "sn_save_cpo" is set that means we encountered "vim9script": restore
    // 'cpoptions', unless in the main .vimrc file.
    // Get "si" again, "script_items" may have been reallocated.
    si = SCRIPT_توکی(sid);
    إذا (si->sn_save_cpo != عدم && si->sn_is_vimrc == DOSO_NONE)
    {
	إذا (STRCMP(p_cpo, CPO_VIM) != 0)
	{
	    char_u *f;
	    char_u *t;

	    // 'cpo' was changed in the script.  Apply the same change to the
	    // saved value, if possible.
	    لكل (f = (char_u *)CPO_VIM; *f != NUL; ++f)
		إذا (vim_strchr(p_cpo, *f) == عدم
			&& (t = vim_strchr(si->sn_save_cpo, *f)) != عدم)
		    // flag was removed, also remove it from the saved 'cpo'
		    mch_memmove(t, t + 1, STRLEN(t));
	    لكل (f = p_cpo; *f != NUL; ++f)
		إذا (vim_strchr((char_u *)CPO_VIM, *f) == عدم
			&& vim_strchr(si->sn_save_cpo, *f) == عدم)
		{
		    // flag was added, also add it to the saved 'cpo'
		    t = alloc(STRLEN(si->sn_save_cpo) + 2);
		    إذا (t != عدم)
		    {
			*t = *f;
			STRCPY(t + 1, si->sn_save_cpo);
			vim_free(si->sn_save_cpo);
			si->sn_save_cpo = t;
		    }
		}
	}
	سېټ_option_ارزښت_give_err((char_u *)"cpo",
					   0L, si->sn_save_cpo, OPT_NO_REDRAW);
    }
    VIM_CLEAR(si->sn_save_cpo);

    restore_funccal();
# ifdef FEAT_PROFILE
    إذا (do_profiling == PROF_YES)
	prof_ماشوم_exit(&wait_پیل);		// leaving a child now
# endif

    کلی_Typed = save_کلی_Typed;
#د_که_پای
    current_sctx = save_current_sctx;

    إذا (cookie.fp != عدم)
	fclose(cookie.fp);
    إذا (cookie.سرچینه_from_buf)
	ga_clear_مزي_ګانې(&cookie.buflines);
    vim_free(cookie.nextline);
    vim_free(firstline);
    convert_تیاری(&cookie.conv, عدم, عدم);

    إذا (trigger_سرچینه_post)
	apply_autocmds(EVENT_SOURCEPOST, fname_exp, fname_exp, FALSE, curbuf);

theend:
#که_تعریف_شوی FEAT_EVAL
    إذا (sid > 0 && ret_sid != عدم
	    && fname_not_fixed != عدم && fname_exp != عدم)
    {
	صحيح not_fixed_sid = پیدا_کړه_script_by_name(fname_not_fixed);

	// If "fname_not_fixed" is a symlink then we source the linked file.
	// If the original name is in the script list we add the ID of the
	// script that was actually sourced.
	إذا (SCRIPT_ID_VALID(not_fixed_sid) && not_fixed_sid != sid)
	    SCRIPT_توکی(not_fixed_sid)->sn_sourced_sid = sid;
    }
#د_که_پای

    vim_free(fname_not_fixed);
    vim_free(fname_exp);
    sticky_cmdmod_flags = save_sticky_cmdmod_flags;
#که_تعریف_شوی FEAT_EVAL
    estack_compiling = save_estack_compiling;
#د_که_پای
    أرجع retval;
}

    صحيح
do_سرچینه(
    char_u	*fname,
    صحيح		check_other,	    // check for .vimrc and _vimrc
    صحيح		is_vimrc,	    // DOSO_ value
    صحيح		*ret_sid)
{
    أرجع do_سرچینه_ext(fname, check_other, is_vimrc, ret_sid, عدم, FALSE);
}


#إذا defined(FEAT_EVAL)

/*
 * ":scriptnames"
 */
    فراغ
ex_scriptnames(exarg_T *eap)
{
    صحيح i;

    إذا (eap->addr_شمېر > 0 || *eap->arg != NUL)
    {
	// :script {scriptId}: edit the script
	إذا (eap->addr_شمېر > 0 && !SCRIPT_ID_VALID(eap->line2))
	    emsg(_(e_invalid_دلیل));
	وإلا
	{
	    إذا (eap->addr_شمېر > 0)
		eap->arg = SCRIPT_توکی(eap->line2)->sn_name;
	    وإلا
	    {
		expand_env(eap->arg, NameBuff, MAXPATHL);
		eap->arg = NameBuff;
	    }
	    do_exedit(eap, عدم);
	}
	أرجع;
    }

    لكل (i = 1; i <= script_توکي.ga_len && !got_int; ++i)
    {
	scriptitem_T *si = SCRIPT_توکی(i);

	إذا (si->sn_name != عدم)
	{
	    حرف sourced_buf[20];

	    home_replace(عدم, si->sn_name, NameBuff, MAXPATHL, TRUE);
	    إذا (si->sn_sourced_sid > 0)
		vim_snprintf(sourced_buf, 20, "->%d", si->sn_sourced_sid);
	    وإلا
		sourced_buf[0] = NUL;
	    vim_snprintf((حرف *)IObuff, IOSIZE, "%3d%s%s: %s",
		    i,
		    sourced_buf,
		    si->sn_state == SN_STATE_NOT_LOADED ? " A" : "",
		    NameBuff);
	    إذا (!پیغام_filtered(IObuff))
	    {
		پیغام_putchar('\n');
		پیغام_outtrans(IObuff);
		بهر_flush();	    // output one line at a time
		ui_breakcheck();
	    }
	}
    }
}

# إذا defined(BACKSLASH_دننه_FILENAME)
/*
 * Fix slashes in the list of script names for 'shellslash'.
 */
    فراغ
scriptnames_slash_adjust(فراغ)
{
    صحيح i;

    لكل (i = 1; i <= script_توکي.ga_len; ++i)
	إذا (SCRIPT_توکی(i)->sn_name != عدم)
	    slash_adjust(SCRIPT_توکی(i)->sn_name);
}
# endif

/*
 * Get a pointer to a script name.  Used for ":verbose set".
 * Message appended to "Last set from "
 */
    char_u *
get_scriptname(scid_T id)
{
    إذا (id == SID_MODELINE)
	أرجع (char_u *)_("modeline");
    إذا (id == SID_CMDARG)
	أرجع (char_u *)_("--cmd argument");
    إذا (id == SID_CARG)
	أرجع (char_u *)_("-c argument");
    إذا (id == SID_ENV)
	أرجع (char_u *)_("environment variable");
    إذا (id == SID_ERROR)
	أرجع (char_u *)_("error handler");
    إذا (id == SID_WINLAYOUT)
	أرجع (char_u *)_("changed window size");
    أرجع SCRIPT_توکی(id)->sn_name;
}

# إذا defined(EXITFREE)
    فراغ
free_scriptnames(فراغ)
{
    صحيح			i;

    لكل (i = script_توکي.ga_len; i > 0; --i)
    {
	scriptitem_T *si = SCRIPT_توکی(i);

	// the variables themselves are cleared in evalvars_clear()
	vim_free(si->sn_vars);

	vim_free(si->sn_name);
	free_imports_and_script_vars(i);
	free_string_option(si->sn_save_cpo);
#  ifdef FEAT_PROFILE
	ga_clear(&si->sn_prl_ga);
#  endif
	vim_free(si->sn_autoload_مختاړی);
	vim_free(si);
    }
    ga_clear(&script_توکي);
}

    فراغ
free_autoload_scriptnames(فراغ)
{
    ga_clear_مزي_ګانې(&ga_loaded);
}
# endif

    linenr_T
get_sourced_lnum(
	char_u *(*fgetline)(صحيح, فراغ *, صحيح, getline_opt_T),
	فراغ *cookie)
{
    أرجع fgetline == getsourceline
			? ((سرچینه_cookie_T *)cookie)->sourcing_lnum
			: SOURCING_LNUM;
}

/*
 * Return a List of script-local functions defined in the script with id
 * 'sid'.
 */
    ساكن list_T *
get_script_local_funcs(scid_T sid)
{
    hashtab_T	*functbl;
    hashitem_T	*hi;
    long_u	todo;
    list_T	*l;

    l = list_alloc();
    إذا (l == عدم)
	أرجع عدم;

    // Iterate through all the functions in the global function hash table
    // looking for functions with script ID 'sid'.
    functbl = func_tbl_get();
    todo = functbl->ht_کارول_شوی;
    FOR_ALL_HASHTAB_توکي(functbl, hi, todo)
    {
	ufunc_T	*fp;

	إذا (HASHITEM_تشه(hi))
	    استمر;

	--todo;
	fp = HI2UF(hi);

	// Add active functions with script id == 'sid'
	إذا (!(fp->uf_flags & FC_DEAD) && (fp->uf_script_ctx.sc_sid == sid))
	{
	    char_u	*name;

	    إذا (fp->uf_name_exp != عدم)
		name = fp->uf_name_exp;
	    وإلا
		name = fp->uf_name;

	    list_append_string(l, name, -1);
	}
    }

    أرجع l;
}

/*
 * getscriptinfo() function
 */
    فراغ
f_getscriptinfo(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    char_u	*pat = عدم;
    regmatch_T	regmatch;
    صحيح		filterpat = FALSE;
    scid_T	sid = -1;

    إذا (rettv_list_alloc(rettv) == FAIL)
	أرجع;

    إذا (check_for_opt_dict_arg(argvars, 0) == FAIL)
	أرجع;

    l = rettv->vval.v_list;

    regmatch.regprog = عدم;
    regmatch.rm_ic = p_ic;

    إذا (argvars[0].v_type == VAR_DICT)
    {
	dictitem_T *sid_di = dict_پیدا_کړه(argvars[0].vval.v_dict,
							   (char_u *)"sid", 3);
	إذا (sid_di != عدم)
	{
	    صحيح error = FALSE;
	    sid = tv_get_شمېره_chk(&sid_di->di_tv, &error);
	    إذا (error)
		أرجع;
	    إذا (sid <= 0)
	    {
		semsg(_(e_invalid_ارزښت_for_دلیل_str_str), "sid",
						tv_get_string(&sid_di->di_tv));
		أرجع;
	    }
	}
	وإلا
	{
	    pat = dict_get_string(argvars[0].vval.v_dict, "name", TRUE);
	    إذا (pat != عدم)
		regmatch.regprog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
	    إذا (regmatch.regprog != عدم)
		filterpat = TRUE;
	}
    }

    لكل (varnumber_T i = sid > 0 ? sid : 1;
		       (i == sid || sid <= 0) && i <= script_توکي.ga_len; ++i)
    {
	scriptitem_T	*si = SCRIPT_توکی(i);
	dict_T		*d;

	إذا (si->sn_name == عدم)
	    استمر;

	إذا (filterpat && !vim_regexec(&regmatch, si->sn_name, (colnr_T)0))
	    استمر;

	إذا ((d = dict_alloc()) == عدم
		|| list_append_dict(l, d) == FAIL)
	{
	    dict_unref(d);
	    اذهب theend;
	}
	إذا (dict_جمع_کړه_string(d, "name", si->sn_name) == FAIL
		|| dict_جمع_کړه_شمېره(d, "sid", i) == FAIL
		|| dict_جمع_کړه_شمېره(d, "sourced", si->sn_sourced_sid) == FAIL
		|| dict_جمع_کړه_شمېره(d, "version", si->sn_نسخه) == FAIL
		|| dict_جمع_کړه_bool(d, "autoload",
				si->sn_state == SN_STATE_NOT_LOADED) == FAIL)
	    اذهب theend;

	// When a script ID is specified, return information about only the
	// specified script, and add the script-local variables and functions.
	إذا (sid > 0)
	{
	    dict_T	*var_dict;
	    list_T	*fn_list;

	    var_dict = dict_copy(&si->sn_vars->sv_dict, TRUE, TRUE,
								get_copyID());
	    إذا (var_dict == عدم)
		اذهب theend;
	    إذا (dict_جمع_کړه_dict(d, "variables", var_dict) == FAIL)
	    {
		dict_unref(var_dict);
		اذهب theend;
	    }
	    --var_dict->dv_refcount;
	    fn_list = get_script_local_funcs(sid);
	    إذا (fn_list == عدم || dict_جمع_کړه_list(d, "functions", fn_list) == FAIL)
	    {
		list_unref(fn_list);
		اذهب theend;
	    }
	}
    }

theend:
    vim_regfree(regmatch.regprog);
    vim_free(pat);
}

#د_که_پای

    ساكن char_u *
get_one_sourceline(سرچینه_cookie_T *sp)
{
    garray_T		ga;
    صحيح			len;
    صحيح			c;
    char_u		*buf;
#که_تعریف_شوی USE_CRNL
    صحيح			has_cr;		// CR-LF found
#د_که_پای
    صحيح			have_read = FALSE;

    // use a growarray to store the sourced line
    ga_init2(&ga, 1, 250);

    // Loop until there is a finished line (or end-of-file).
    ++sp->sourcing_lnum;
    لكل (;;)
    {
	// make room to read at least 120 (more) characters
	إذا (ga_grow(&ga, 120) == FAIL)
	    اكسر;
	إذا (sp->سرچینه_from_buf)
	{
	    إذا (sp->buf_lnum >= sp->buflines.ga_len)
		اكسر;		    // all the lines are processed
	    ga_concat(&ga, ((char_u **)sp->buflines.ga_معلومات)[sp->buf_lnum]);
	    sp->buf_lnum++;
	    إذا (ga_grow(&ga, 1) == FAIL)
		اكسر;
	    buf = (char_u *)ga.ga_معلومات;
	    buf[ga.ga_len++] = NUL;
	    len = ga.ga_len;
	}
	وإلا
	{
	    buf = (char_u *)ga.ga_معلومات;
	    إذا (fgets((حرف *)buf + ga.ga_len, ga.ga_maxlen - ga.ga_len,
			sp->fp) == عدم)
		اكسر;
	    len = ga.ga_len + (صحيح)STRLEN(buf + ga.ga_len);
	}
#که_تعریف_شوی USE_CRNL
	// Ignore a trailing CTRL-Z, when in Dos mode.	Only recognize the
	// CTRL-Z by its own, or after a NL.
	إذا (	   (len == 1 || (len >= 2 && buf[len - 2] == '\n'))
		&& sp->fileformat == EOL_DOS
		&& buf[len - 1] == Ctrl_Z)
	{
	    buf[len - 1] = NUL;
	    اكسر;
	}
#د_که_پای

	have_read = TRUE;
	ga.ga_len = len;

	// If the line was longer than the buffer, read more.  A line of a
	// buffer was taken as a whole.
	إذا (!sp->سرچینه_from_buf && ga.ga_maxlen - ga.ga_len == 1
						       && buf[len - 1] != '\n')
	    استمر;

	إذا (len >= 1 && buf[len - 1] == '\n')	// remove trailing NL
	{
#که_تعریف_شوی USE_CRNL
	    has_cr = (len >= 2 && buf[len - 2] == '\r');
	    إذا (sp->fileformat == EOL_UNKNOWN)
	    {
		إذا (has_cr)
		    sp->fileformat = EOL_DOS;
		وإلا
		    sp->fileformat = EOL_UNIX;
	    }

	    إذا (sp->fileformat == EOL_DOS)
	    {
		إذا (has_cr)	    // replace trailing CR
		{
		    buf[len - 2] = '\n';
		    --len;
		    --ga.ga_len;
		}
		وإلا	    // lines like ":map xx yy^M" will have failed
		{
		    إذا (!sp->error)
		    {
			پیغام_سرچینه(HL_ATTR(HLF_W));
			emsg(_("W15: Warning: Wrong line separator, ^M may be missing"));
		    }
		    sp->error = TRUE;
		    sp->fileformat = EOL_UNIX;
		}
	    }
#د_که_پای
	    // The '\n' is escaped if there is an odd number of ^V's just
	    // before it, first set "c" just before the 'V's and then check
	    // len&c parities (is faster than ((len-c)%2 == 0)) -- Acevedo
	    لكل (c = len - 2; c >= 0 && buf[c] == Ctrl_V; c--)
		;
	    إذا ((len & 1) != (c & 1))	// escaped NL, read more
	    {
		++sp->sourcing_lnum;
		استمر;
	    }

	    buf[len - 1] = NUL;		// remove the NL
	}

	// Check for ^C here now and then, so recursive :so can be broken.
	کرښه_breakcheck();
	اكسر;
    }

    إذا (have_read)
	أرجع (char_u *)ga.ga_معلومات;

    vim_free(ga.ga_معلومات);
    أرجع عدم;
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
	صحيح c ناکارول_شوی,
	فراغ *cookie,
	صحيح indent ناکارول_شوی,
	getline_opt_T options)
{
    سرچینه_cookie_T	*sp = (سرچینه_cookie_T *)cookie;
    char_u		*کرښه;
    char_u		*p;
    صحيح			do_vim9_all = دننه_vim9script()
					      && options == GETLINE_CONCAT_ALL;
    صحيح			do_bar_cont = do_vim9_all
					 || options == GETLINE_CONCAT_CONTBAR;

#که_تعریف_شوی FEAT_EVAL
    // If breakpoints have been added/deleted need to check for it.
    إذا ((sp->dbg_tick < debug_tick) && !sp->سرچینه_from_buf)
    {
	sp->breakpoint = dbg_پیدا_کړه_breakpoint(TRUE, sp->fname, SOURCING_LNUM);
	sp->dbg_tick = debug_tick;
    }
# ifdef FEAT_PROFILE
    إذا (do_profiling == PROF_YES)
	script_کرښه_پای_ټکی();
# endif
#د_که_پای

    // Set the current sourcing line number.
    SOURCING_LNUM = sp->sourcing_lnum + 1;

    // Get current line.  If there is a read-ahead line, use it, otherwise get
    // one now.  "fp" is NULL if actually using a string.
    إذا (sp->پای_ته_رسېدلی || (!sp->سرچینه_from_buf && sp->fp == عدم))
	کرښه = عدم;
    وإلا إذا (sp->nextline == عدم)
	کرښه = get_one_sourceline(sp);
    وإلا
    {
	کرښه = sp->nextline;
	sp->nextline = عدم;
	++sp->sourcing_lnum;
    }
#که_تعریف_شوی FEAT_PROFILE
    إذا (کرښه != عدم && do_profiling == PROF_YES)
	script_کرښه_پیل();
#د_که_پای

    // Only concatenate lines starting with a \ when 'cpoptions' doesn't
    // contain the 'C' flag.
    إذا (کرښه != عدم && options != GETLINE_NONE
				      && vim_strchr(p_cpo, CPO_CONCAT) == عدم)
    {
	صحيح comment_char = دننه_vim9script() ? '#' : '"';

	// compensate for the one line read-ahead
	--sp->sourcing_lnum;

	// Get the next line and concatenate it when it starts with a
	// backslash. We always need to read the next line, keep it in
	// sp->nextline.
	/* Also check for a comment in between continuation lines: "\ */
	// Also check for a Vim9 comment, empty line, line starting with '|',
	// but not "||".
	sp->nextline = get_one_sourceline(sp);
	إذا (sp->nextline != عدم
		&& (*(p = skipwhite(sp->nextline)) == '\\'
			      || (p[0] == comment_char
						&& p[1] == '\\' && p[2] == ' ')
			      || (do_vim9_all && (*p == NUL
						     || vim9_comment_پیل(p)))
			      || (do_bar_cont && p[0] == '|' && p[1] != '|')))
	{
	    garray_T    ga;

	    ga_init2(&ga, حجم(char_u), 400);
	    ga_concat(&ga, کرښه);
	    إذا (*p == '\\')
		ga_concat(&ga, p + 1);
	    وإلا إذا (*p == '|')
	    {
		GA_CONCAT_LITERAL(&ga, " ");
		ga_concat(&ga, p);
	    }
	    لكل (;;)
	    {
		vim_free(sp->nextline);
		sp->nextline = get_one_sourceline(sp);
		إذا (sp->nextline == عدم)
		    اكسر;
		p = skipwhite(sp->nextline);
		إذا (*p == '\\' || (do_bar_cont && p[0] == '|' && p[1] != '|'))
		{
		    // Adjust the growsize to the current length to speed up
		    // concatenating many lines.
		    إذا (ga.ga_len > 400)
		    {
			إذا (ga.ga_len > 8000)
			    ga.ga_growsize = 8000;
			وإلا
			    ga.ga_growsize = ga.ga_len;
		    }
		    إذا (*p == '\\')
			ga_concat(&ga, p + 1);
		    وإلا
		    {
			GA_CONCAT_LITERAL(&ga, " ");
			ga_concat(&ga, p);
		    }
		}
		وإلا إذا (!(p[0] == (comment_char)
						&& p[1] == '\\' && p[2] == ' ')
		     && !(do_vim9_all && (*p == NUL || vim9_comment_پیل(p))))
		    اكسر;
		/* drop a # comment or "\ comment line */
	    }
	    ga_append(&ga, NUL);
	    vim_free(کرښه);
	    کرښه = ga.ga_معلومات;
	}
    }

    إذا (کرښه != عدم && sp->conv.vc_type != CONV_NONE)
    {
	char_u	*s;

	// Convert the encoding of the script line.
	s = string_convert(&sp->conv, کرښه, عدم);
	إذا (s != عدم)
	{
	    vim_free(کرښه);
	    کرښه = s;
	}
    }

#که_تعریف_شوی FEAT_EVAL
    // Did we encounter a breakpoint?
    إذا (!sp->سرچینه_from_buf && sp->breakpoint != 0
	    && sp->breakpoint <= SOURCING_LNUM)
    {
	dbg_breakpoint(sp->fname, SOURCING_LNUM);
	// Find next breakpoint.
	sp->breakpoint = dbg_پیدا_کړه_breakpoint(TRUE, sp->fname, SOURCING_LNUM);
	sp->dbg_tick = debug_tick;
    }
#د_که_پای

    أرجع کرښه;
}

/*
 * Returns TRUE if sourcing a script either from a file or a buffer.
 * Otherwise returns FALSE.
 */
    صحيح
sourcing_a_script(exarg_T *eap)
{
    أرجع (getline_equal(eap->ea_getline, eap->cookie, getsourceline));
}

/*
 * ":scriptencoding": Set encoding conversion for a sourced script.
 */
    فراغ
ex_scriptencoding(exarg_T *eap)
{
    سرچینه_cookie_T	*sp;
    char_u		*name;

    إذا (!sourcing_a_script(eap))
    {
	emsg(_(e_scriptencoding_کارول_شوی_outside_د_sourced_file));
	أرجع;
    }

    إذا (*eap->arg != NUL)
    {
	name = enc_canonize(eap->arg);
	إذا (name == عدم)	// out of memory
	    أرجع;
    }
    وإلا
	name = eap->arg;

    // Setup for conversion from the specified encoding to 'encoding'.
    sp = (سرچینه_cookie_T *)getline_cookie(eap->ea_getline, eap->cookie);
    convert_تیاری(&sp->conv, name, p_enc);

    إذا (name != eap->arg)
	vim_free(name);
}

/*
 * ":scriptversion": Set Vim script version for a sourced script.
 */
    فراغ
ex_scriptversion(exarg_T *eap ناکارول_شوی)
{
    صحيح		nr;

    إذا (!sourcing_a_script(eap))
    {
	emsg(_(e_scriptversion_کارول_شوی_outside_د_sourced_file));
	أرجع;
    }
    إذا (دننه_vim9script())
    {
	emsg(_(e_cannot_use_scriptversion_after_vim9script));
	أرجع;
    }

    nr = getdigits(&eap->arg);
    إذا (nr == 0 || *eap->arg != NUL)
	emsg(_(e_invalid_دلیل));
    وإلا إذا (nr > SCRIPT_نسخه_MAX)
	semsg(_(e_scriptversion_not_supported_nr), nr);
    وإلا
    {
	current_sctx.sc_نسخه = nr;
#که_تعریف_شوی FEAT_EVAL
	SCRIPT_توکی(current_sctx.sc_sid)->sn_نسخه = nr;
#د_که_پای
    }
}

#إذا defined(FEAT_EVAL)
/*
 * ":finish": Mark a sourced file as finished.
 */
    فراغ
ex_پای(exarg_T *eap)
{
    إذا (sourcing_a_script(eap))
	do_پای(eap, FALSE);
    وإلا
	emsg(_(e_پای_کارول_شوی_outside_د_sourced_file));
}

/*
 * Mark a sourced file as finished.  Possibly makes the ":finish" pending.
 * Also called for a pending finish at the ":endtry" or after returning from
 * an extra do_cmdline().  "reanimate" is used in the latter case.
 */
    فراغ
do_پای(exarg_T *eap, صحيح reanimate)
{
    صحيح		idx;

    إذا (reanimate)
	((سرچینه_cookie_T *)getline_cookie(eap->ea_getline,
					      eap->cookie))->پای_ته_رسېدلی = FALSE;

    // Cleanup (and inactivate) conditionals, but stop when a try conditional
    // not in its finally clause (which then is to be executed next) is found.
    // In this case, make the ":finish" pending for execution at the ":endtry".
    // Otherwise, finish normally.
    idx = cleanup_conditionals(eap->cstack, 0, TRUE);
    إذا (idx >= 0)
    {
	eap->cstack->cs_pending[idx] = CSTP_پای;
	report_make_pending(CSTP_پای, عدم);
    }
    وإلا
	((سرچینه_cookie_T *)getline_cookie(eap->ea_getline,
					       eap->cookie))->پای_ته_رسېدلی = TRUE;
}


/*
 * Return TRUE when a sourced file had the ":finish" command: Don't give error
 * message for missing ":endif".
 * Return FALSE when not sourcing a file.
 */
    صحيح
سرچینه_پای_ته_رسېدلی(
    char_u	*(*fgetline)(صحيح, فراغ *, صحيح, getline_opt_T),
    فراغ	*cookie)
{
    أرجع (getline_equal(fgetline, cookie, getsourceline)
	    && ((سرچینه_cookie_T *)getline_cookie(
						fgetline, cookie))->پای_ته_رسېدلی);
}

/*
 * Find the path of a script below the "autoload" directory.
 * Returns NULL if there is no "/autoload/" in the script name.
 */
    ساكن char_u *
script_name_after_autoload(scriptitem_T *si)
{
    char_u	*p = si->sn_name;
    char_u	*res = عدم;

    لكل (;;)
    {
	char_u *n = (char_u *)مزي_پلټنه((حرف *)p, "autoload");

	إذا (n == عدم)
	    اكسر;
	إذا (n > p && vim_ispathsep(n[-1]) && vim_ispathsep(n[8]))
	    res = n + 9;
	p = n + 8;
    }
    أرجع res;
}

/*
 * For an autoload script "autoload/dir/script.vim" return the prefix
 * "dir#script#" in allocated memory.
 * Returns NULL if anything is wrong.
 */
    char_u *
get_autoload_مختاړی(scriptitem_T *si)
{
    char_u *p = script_name_after_autoload(si);
    char_u *مختاړی;

    إذا (p == عدم)
	أرجع عدم;
    مختاړی = vim_strsave(p);
    إذا (مختاړی == عدم)
	أرجع عدم;

    // replace all '/' with '#' and locate ".vim" at the end
    لكل (p = مختاړی; *p != NUL; p += mb_ptr2len(p))
    {
	إذا (vim_ispathsep(*p))
	    *p = '#';
	وإلا إذا (STRCMP(p, ".vim") == 0)
	{
	    p[0] = '#';
	    p[1] = NUL;
	    أرجع مختاړی;
	}
    }

    // did not find ".vim" at the end
    vim_free(مختاړی);
    أرجع عدم;
}

/*
 * If in a Vim9 autoload script return "name" with the autoload prefix for the
 * script.  If successful the returned name is allocated.
 * Otherwise it returns "name" unmodified.
 */
    char_u *
may_مختاړی_autoload(char_u *name)
{
    إذا (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	أرجع name;

    scriptitem_T *si = SCRIPT_توکی(current_sctx.sc_sid);

    إذا (si->sn_autoload_مختاړی == عدم)
	أرجع name;

    char_u  *basename = name;
    size_t  len;
    char_u  *res;

    إذا (*name == K_SPECIAL)
    {
	char_u *p = vim_strchr(name, '_');

	// skip over "<SNR>99_"
	إذا (p != عدم)
	    basename = p + 1;
    }

    len = STRLEN(si->sn_autoload_مختاړی) + STRLEN(basename) + 2;
    res = alloc(len);
    إذا (res == عدم)
	أرجع عدم;

    vim_snprintf((حرف *)res, len, "%s%s", si->sn_autoload_مختاړی, basename);
    أرجع res;
}

/*
 * Return the autoload script name for a function or variable name.
 * Returns NULL when out of memory.
 * Caller must make sure that "name" contains AUTOLOAD_CHAR.
 */
    char_u *
autoload_name(char_u *name)
{
    char_u	*p, *q = عدم;
    char_u	*scriptname;

    // Get the script file name: replace '#' with '/', append ".vim".
    scriptname = alloc(STRLEN(name) + 14);
    إذا (scriptname == عدم)
	أرجع عدم;
    STRCPY(scriptname, "autoload/");
    STRCAT(scriptname, name[0] == 'g' && name[1] == ':' ? name + 2: name);
    لكل (p = scriptname + 9; (p = vim_strchr(p, AUTOLOAD_CHAR)) != عدم;
								    q = p, ++p)
	*p = '/';
    STRCPY(q, ".vim");
    أرجع scriptname;
}

/*
 * If "name" has a package name try autoloading the script for it.
 * Return TRUE if a package was loaded.
 */
    صحيح
script_autoload(
    char_u	*name,
    صحيح		reload)	    // load script again when already loaded
{
    char_u	*p;
    char_u	*scriptname, *tofree;
    صحيح		ret = FALSE;
    صحيح		i;
    صحيح		ret_sid;

    // If the name starts with "<SNR>123_" then "123" is the script ID.
    إذا (name[0] == K_SPECIAL && name[1] == KS_EXTRA && name[2] == KE_SNR)
    {
	p = name + 3;
	ret_sid = (صحيح)getdigits(&p);
	إذا (*p == '_' && SCRIPT_ID_VALID(ret_sid))
	{
	    may_بارول_script(ret_sid, &ret);
	    أرجع ret;
	}
    }

    // If there is no '#' after name[0] there is no package name.
    p = vim_strchr(name, AUTOLOAD_CHAR);
    إذا (p == عدم || p == name)
	أرجع FALSE;

    tofree = scriptname = autoload_name(name);
    إذا (scriptname == عدم)
	أرجع FALSE;

    // Find the name in the list of previously loaded package names.  Skip
    // "autoload/", it's always the same.
    لكل (i = 0; i < ga_loaded.ga_len; ++i)
	إذا (STRCMP(((char_u **)ga_loaded.ga_معلومات)[i] + 9, scriptname + 9) == 0)
	    اكسر;
    إذا (!reload && i < ga_loaded.ga_len)
	ret = FALSE;	    // was loaded already
    وإلا
    {
	// Remember the name if it wasn't loaded already.
	إذا (i == ga_loaded.ga_len && ga_grow(&ga_loaded, 1) == OK)
	{
	    ((char_u **)ga_loaded.ga_معلومات)[ga_loaded.ga_len++] = scriptname;
	    tofree = عدم;
	}

	// Try loading the package from $VIMRUNTIME/autoload/<name>.vim
	// Use "ret_sid" to avoid loading the same script again.
	إذا (سرچینه_دننه_لار(p_rtp, scriptname, DIP_پیل, &ret_sid) == OK)
	    ret = TRUE;
    }

    vim_free(tofree);
    أرجع ret;
}
#د_که_پای
