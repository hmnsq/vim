/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * evalvars.c: functions for dealing with variables
 */

#अन्तर्भाव "vim.h"

#यदि defined(FEAT_EVAL)

स्थिर dictitem_T	globvars_var;		// variable used for g:
स्थिर dict_T		globvardict;		// Dictionary with g: variables
#define globvarht globvardict.dv_hashtab

/*
 * Old Vim variables such as "v:version" are also available without the "v:".
 * Also in functions.  We need a special hashtable for them.
 */
स्थिर hashtab_T	compat_hashtab;

/*
 * Array to hold the value of v: variables.
 * The value is in a dictitem, so that it can also be used in the v: scope.
 * The reason to use this table anyway is for very quick access to the
 * variables with the VV_ defines.
 */

// values for vv_flags:
#define VV_COMPAT	1	// compatible, also used without "v:"
#define VV_RO		2	// read-only
#define VV_RO_SBX	4	// read-only in the sandbox

#define VV_NAME(s, t)	s, {{t, 0, {0}}, 0, {s}}

प्रकारनिर्देश संरचना vimvar vimvar_T;

स्थिर संरचना vimvar
{
    वर्ण	*vv_name;	// name of variable, without v:
    dictitem16_T vv_di;		// value and name for key (max 16 chars!)
    type_T	*vv_type;	// type or NULL
    वर्ण	vv_flags;	// VV_COMPAT, VV_RO, VV_RO_SBX
} vimvars[VV_LEN] =
{
    // The order here must match the VV_ defines in vim.h!
    // Initializing a union does not work, leave tv.vval empty to get zero's.
    {VV_NAME("count",		 VAR_संख्या), रिक्त, VV_COMPAT+VV_RO},
    {VV_NAME("count1",		 VAR_संख्या), रिक्त, VV_RO},
    {VV_NAME("prevcount",	 VAR_संख्या), रिक्त, VV_RO},
    {VV_NAME("errmsg",		 VAR_STRING), रिक्त, VV_COMPAT},
    {VV_NAME("warningmsg",	 VAR_STRING), रिक्त, 0},
    {VV_NAME("statusmsg",	 VAR_STRING), रिक्त, 0},
    {VV_NAME("shell_error",	 VAR_संख्या), रिक्त, VV_COMPAT+VV_RO},
    {VV_NAME("this_session",	 VAR_STRING), रिक्त, VV_COMPAT},
    {VV_NAME("version",		 VAR_संख्या), रिक्त, VV_COMPAT+VV_RO},
    {VV_NAME("lnum",		 VAR_संख्या), रिक्त, VV_RO_SBX},
    {VV_NAME("termresponse",	 VAR_STRING), रिक्त, VV_RO},
    {VV_NAME("fname",		 VAR_STRING), रिक्त, VV_RO},
    {VV_NAME("lang",		 VAR_STRING), रिक्त, VV_RO},
    {VV_NAME("lc_time",		 VAR_STRING), रिक्त, VV_RO},
    {VV_NAME("ctype",		 VAR_STRING), रिक्त, VV_RO},
    {VV_NAME("charconvert_from", VAR_STRING), रिक्त, VV_RO},
    {VV_NAME("charconvert_to",	 VAR_STRING), रिक्त, VV_RO},
    {VV_NAME("fname_in",	 VAR_STRING), रिक्त, VV_RO},
    {VV_NAME("fname_out",	 VAR_STRING), रिक्त, VV_RO},
    {VV_NAME("fname_new",	 VAR_STRING), रिक्त, VV_RO},
    {VV_NAME("fname_diff",	 VAR_STRING), रिक्त, VV_RO},
    {VV_NAME("cmdarg",		 VAR_STRING), रिक्त, VV_RO},
    {VV_NAME("foldstart",	 VAR_संख्या), रिक्त, VV_RO_SBX},
    {VV_NAME("foldend",		 VAR_संख्या), रिक्त, VV_RO_SBX},
    {VV_NAME("folddashes",	 VAR_STRING), रिक्त, VV_RO_SBX},
    {VV_NAME("foldlevel",	 VAR_संख्या), रिक्त, VV_RO_SBX},
    {VV_NAME("progname",	 VAR_STRING), रिक्त, VV_RO},
    {VV_NAME("servername",	 VAR_STRING), रिक्त, VV_RO},
    {VV_NAME("dying",		 VAR_संख्या), रिक्त, VV_RO},
    {VV_NAME("exception",	 VAR_STRING), रिक्त, VV_RO},
    {VV_NAME("throwpoint",	 VAR_STRING), रिक्त, VV_RO},
    {VV_NAME("register",	 VAR_STRING), रिक्त, VV_RO},
    {VV_NAME("cmdbang",		 VAR_संख्या), रिक्त, VV_RO},
    {VV_NAME("insertmode",	 VAR_STRING), रिक्त, VV_RO},
    {VV_NAME("val",		 VAR_UNKNOWN), रिक्त, VV_RO},
    {VV_NAME("key",		 VAR_UNKNOWN), रिक्त, VV_RO},
    {VV_NAME("profiling",	 VAR_संख्या), रिक्त, VV_RO},
    {VV_NAME("fcs_reason",	 VAR_STRING), रिक्त, VV_RO},
    {VV_NAME("fcs_choice",	 VAR_STRING), रिक्त, 0},
    {VV_NAME("beval_bufnr",	 VAR_संख्या), रिक्त, VV_RO},
    {VV_NAME("beval_winnr",	 VAR_संख्या), रिक्त, VV_RO},
    {VV_NAME("beval_winid",	 VAR_संख्या), रिक्त, VV_RO},
    {VV_NAME("beval_lnum",	 VAR_संख्या), रिक्त, VV_RO},
    {VV_NAME("beval_col",	 VAR_संख्या), रिक्त, VV_RO},
    {VV_NAME("beval_text",	 VAR_STRING), रिक्त, VV_RO},
    {VV_NAME("scrollstart",	 VAR_STRING), रिक्त, 0},
    {VV_NAME("swapname",	 VAR_STRING), रिक्त, VV_RO},
    {VV_NAME("swapchoice",	 VAR_STRING), रिक्त, 0},
    {VV_NAME("swapcommand",	 VAR_STRING), रिक्त, VV_RO},
    {VV_NAME("char",		 VAR_STRING), रिक्त, 0},
    {VV_NAME("mouse_win",	 VAR_संख्या), रिक्त, 0},
    {VV_NAME("mouse_winid",	 VAR_संख्या), रिक्त, 0},
    {VV_NAME("mouse_lnum",	 VAR_संख्या), रिक्त, 0},
    {VV_NAME("mouse_col",	 VAR_संख्या), रिक्त, 0},
    {VV_NAME("operator",	 VAR_STRING), रिक्त, VV_RO},
    {VV_NAME("searchforward",	 VAR_संख्या), रिक्त, 0},
    {VV_NAME("hlsearch",	 VAR_संख्या), रिक्त, 0},
    {VV_NAME("oldfiles",	 VAR_LIST), &t_list_string, 0},
    {VV_NAME("windowid",	 VAR_संख्या), रिक्त, VV_RO},
    {VV_NAME("progpath",	 VAR_STRING), रिक्त, VV_RO},
    {VV_NAME("completed_item",	 VAR_DICT), &t_dict_string, 0},
    {VV_NAME("option_new",	 VAR_STRING), रिक्त, VV_RO},
    {VV_NAME("option_old",	 VAR_STRING), रिक्त, VV_RO},
    {VV_NAME("option_oldlocal",	 VAR_STRING), रिक्त, VV_RO},
    {VV_NAME("option_oldglobal", VAR_STRING), रिक्त, VV_RO},
    {VV_NAME("option_command",	 VAR_STRING), रिक्त, VV_RO},
    {VV_NAME("option_type",	 VAR_STRING), रिक्त, VV_RO},
    {VV_NAME("errors",		 VAR_LIST), &t_list_string, 0},
    {VV_NAME("false",		 VAR_BOOL), रिक्त, VV_RO},
    {VV_NAME("true",		 VAR_BOOL), रिक्त, VV_RO},
    {VV_NAME("none",		 VAR_SPECIAL), रिक्त, VV_RO},
    {VV_NAME("null",		 VAR_SPECIAL), रिक्त, VV_RO},
    {VV_NAME("numbermax",	 VAR_संख्या), रिक्त, VV_RO},
    {VV_NAME("numbermin",	 VAR_संख्या), रिक्त, VV_RO},
    {VV_NAME("numbersize",	 VAR_संख्या), रिक्त, VV_RO},
    {VV_NAME("vim_did_enter",	 VAR_संख्या), रिक्त, VV_RO},
    {VV_NAME("testing",		 VAR_संख्या), रिक्त, 0},
    {VV_NAME("t_number",	 VAR_संख्या), रिक्त, VV_RO},
    {VV_NAME("t_string",	 VAR_संख्या), रिक्त, VV_RO},
    {VV_NAME("t_func",		 VAR_संख्या), रिक्त, VV_RO},
    {VV_NAME("t_list",		 VAR_संख्या), रिक्त, VV_RO},
    {VV_NAME("t_dict",		 VAR_संख्या), रिक्त, VV_RO},
    {VV_NAME("t_float",		 VAR_संख्या), रिक्त, VV_RO},
    {VV_NAME("t_bool",		 VAR_संख्या), रिक्त, VV_RO},
    {VV_NAME("t_none",		 VAR_संख्या), रिक्त, VV_RO},
    {VV_NAME("t_job",		 VAR_संख्या), रिक्त, VV_RO},
    {VV_NAME("t_channel",	 VAR_संख्या), रिक्त, VV_RO},
    {VV_NAME("t_blob",		 VAR_संख्या), रिक्त, VV_RO},
    {VV_NAME("t_class",		 VAR_संख्या), रिक्त, VV_RO},
    {VV_NAME("t_object",	 VAR_संख्या), रिक्त, VV_RO},
    {VV_NAME("termrfgresp",	 VAR_STRING), रिक्त, VV_RO},
    {VV_NAME("termrbgresp",	 VAR_STRING), रिक्त, VV_RO},
    {VV_NAME("termu7resp",	 VAR_STRING), रिक्त, VV_RO},
    {VV_NAME("termstyleresp",	 VAR_STRING), रिक्त, VV_RO},
    {VV_NAME("termblinkresp",	 VAR_STRING), रिक्त, VV_RO},
    {VV_NAME("event",		 VAR_DICT), रिक्त, VV_RO},
    {VV_NAME("versionlong",	 VAR_संख्या), रिक्त, VV_RO},
    {VV_NAME("echospace",	 VAR_संख्या), रिक्त, VV_RO},
    {VV_NAME("argv",		 VAR_LIST), &t_list_string, VV_RO},
    {VV_NAME("collate",		 VAR_STRING), रिक्त, VV_RO},
    {VV_NAME("exiting",		 VAR_SPECIAL), रिक्त, VV_RO},
    {VV_NAME("colornames",       VAR_DICT), &t_dict_string, VV_RO},
    {VV_NAME("sizeofint",	 VAR_संख्या), रिक्त, VV_RO},
    {VV_NAME("sizeoflong",	 VAR_संख्या), रिक्त, VV_RO},
    {VV_NAME("sizeofpointer",	 VAR_संख्या), रिक्त, VV_RO},
    {VV_NAME("maxcol",		 VAR_संख्या), रिक्त, VV_RO},
    {VV_NAME("python3_version",	 VAR_संख्या), रिक्त, VV_RO},
    {VV_NAME("t_typealias",	 VAR_संख्या), रिक्त, VV_RO},
    {VV_NAME("t_enum",		 VAR_संख्या), रिक्त, VV_RO},
    {VV_NAME("t_enumvalue",	 VAR_संख्या), रिक्त, VV_RO},
    {VV_NAME("stacktrace",	 VAR_LIST), &t_list_dict_any, VV_RO},
    {VV_NAME("t_tuple",		 VAR_संख्या), रिक्त, VV_RO},
    {VV_NAME("wayland_display",  VAR_STRING), रिक्त, VV_RO},
    {VV_NAME("clipmethod",	 VAR_STRING), रिक्त, VV_RO},
    {VV_NAME("termda1",		 VAR_STRING), रिक्त, VV_RO},
    {VV_NAME("termosc",	 VAR_STRING), रिक्त, VV_RO},
    {VV_NAME("vim_did_init",	 VAR_संख्या), रिक्त, VV_RO},
    {VV_NAME("clipproviders",	 VAR_DICT), रिक्त, VV_RO},
    {VV_NAME("imagebackend",	 VAR_STRING), रिक्त, VV_RO}
};

// shorthand
#define vv_tv_type	vv_di.di_tv.v_type
#define vv_nr		vv_di.di_tv.vval.v_संख्या
#define vv_float	vv_di.di_tv.vval.v_float
#define vv_str		vv_di.di_tv.vval.v_string
#define vv_list		vv_di.di_tv.vval.v_list
#define vv_dict		vv_di.di_tv.vval.v_dict
#define vv_blob		vv_di.di_tv.vval.v_blob
#define vv_tv		vv_di.di_tv

स्थिर dictitem_T	vimvars_var;		// variable used for v:
स्थिर dict_T		vimvardict;		// Dictionary with v: variables
#define vimvarht  vimvardict.dv_hashtab

// for VIM_VERSION_ defines
#अन्तर्भाव "version.h"

स्थिर शून्य list_glob_vars(पूर्णाङ्क *first);
स्थिर शून्य list_buf_vars(पूर्णाङ्क *first);
स्थिर शून्य list_win_vars(पूर्णाङ्क *first);
स्थिर शून्य list_tab_vars(पूर्णाङ्क *first);
स्थिर char_u *list_arg_vars(exarg_T *eap, char_u *arg, पूर्णाङ्क *first);
स्थिर char_u *ex_let_one(char_u *arg, typval_T *tv, पूर्णाङ्क copy, पूर्णाङ्क flags, char_u *endchars, char_u *op, पूर्णाङ्क var_idx);
स्थिर पूर्णाङ्क do_unlet_var(lval_T *lp, char_u *name_end, exarg_T *eap, पूर्णाङ्क deep, शून्य *cookie);
स्थिर पूर्णाङ्क do_lock_var(lval_T *lp, char_u *name_end, exarg_T *eap, पूर्णाङ्क deep, शून्य *cookie);
स्थिर शून्य list_one_var(dictitem_T *v, वर्ण *prefix, पूर्णाङ्क *first);
स्थिर शून्य list_one_var_a(वर्ण *prefix, char_u *name, पूर्णाङ्क type, char_u *string, पूर्णाङ्क *first);

/*
 * Initialize global and vim special variables
 */
    शून्य
evalvars_init(शून्य)
{
    पूर्णाङ्क		    i;
    संरचना vimvar   *p;

    init_var_dict(&globvardict, &globvars_var, VAR_DEF_SCOPE);
    init_var_dict(&vimvardict, &vimvars_var, VAR_SCOPE);
    vimvardict.dv_lock = VAR_FIXED;
    hash_init(&compat_hashtab);

    प्रति (i = 0; i < VV_LEN; ++i)
    {
	p = &vimvars[i];
	यदि (p->vv_flags & VV_RO)
	    p->vv_di.di_flags = DI_FLAGS_RO | DI_FLAGS_FIX;
	अन्यथा यदि (p->vv_flags & VV_RO_SBX)
	    p->vv_di.di_flags = DI_FLAGS_RO_SBX | DI_FLAGS_FIX;
	अन्यथा
	    p->vv_di.di_flags = DI_FLAGS_FIX;

	// add to v: scope dict, unless the value is not always available
	यदि (p->vv_tv_type != VAR_UNKNOWN)
	    hash_add(&vimvarht, p->vv_di.di_किल्ली, "initialization");
	यदि (p->vv_flags & VV_COMPAT)
	    // add to compat scope dict
	    hash_add(&compat_hashtab, p->vv_di.di_किल्ली, "initialization");
    }
    set_vim_var_nr(VV_VERSION, VIM_VERSION_100);
    set_vim_var_nr(VV_VERSIONLONG, VIM_VERSION_100 * 10000 + highest_patch());

    set_vim_var_nr(VV_SEARCHFORWARD, 1L);
    set_vim_var_nr(VV_HLSEARCH, 1L);
    set_vim_var_nr(VV_EXITING, VVAL_NULL);
    set_vim_var_dict(VV_COMPLETED_ITEM, dict_alloc_lock(VAR_FIXED));
    set_vim_var_list(VV_ERRORS, list_alloc());
    set_vim_var_dict(VV_EVENT, dict_alloc_lock(VAR_FIXED));

    set_vim_var_nr(VV_FALSE, VVAL_FALSE);
    set_vim_var_nr(VV_TRUE, VVAL_TRUE);
    set_vim_var_nr(VV_NONE, VVAL_NONE);
    set_vim_var_nr(VV_NULL, VVAL_NULL);
    set_vim_var_nr(VV_NUMBERMAX, VARNUM_MAX);
    set_vim_var_nr(VV_NUMBERMIN, VARNUM_MIN);
    set_vim_var_nr(VV_NUMBERSIZE, परिमाण(varnumber_T) * 8);
    set_vim_var_nr(VV_SIZEOFINT, परिमाण(पूर्णाङ्क));
    set_vim_var_nr(VV_SIZEOFLONG, परिमाण(long));
    set_vim_var_nr(VV_SIZEOFPOINTER, परिमाण(वर्ण *));
    set_vim_var_nr(VV_MAXCOL, MAXCOL);

    set_vim_var_nr(VV_TYPE_संख्या,  VAR_TYPE_संख्या);
    set_vim_var_nr(VV_TYPE_STRING,  VAR_TYPE_STRING);
    set_vim_var_nr(VV_TYPE_FUNC,    VAR_TYPE_FUNC);
    set_vim_var_nr(VV_TYPE_LIST,    VAR_TYPE_LIST);
    set_vim_var_nr(VV_TYPE_DICT,    VAR_TYPE_DICT);
    set_vim_var_nr(VV_TYPE_FLOAT,   VAR_TYPE_FLOAT);
    set_vim_var_nr(VV_TYPE_BOOL,    VAR_TYPE_BOOL);
    set_vim_var_nr(VV_TYPE_NONE,    VAR_TYPE_NONE);
    set_vim_var_nr(VV_TYPE_JOB,     VAR_TYPE_JOB);
    set_vim_var_nr(VV_TYPE_CHANNEL, VAR_TYPE_CHANNEL);
    set_vim_var_nr(VV_TYPE_BLOB,    VAR_TYPE_BLOB);
    set_vim_var_nr(VV_TYPE_CLASS,   VAR_TYPE_CLASS);
    set_vim_var_nr(VV_TYPE_OBJECT,  VAR_TYPE_OBJECT);
    set_vim_var_nr(VV_TYPE_TYPEALIAS,  VAR_TYPE_TYPEALIAS);
    set_vim_var_nr(VV_TYPE_ENUM,    VAR_TYPE_ENUM);
    set_vim_var_nr(VV_TYPE_ENUMVALUE,  VAR_TYPE_ENUMVALUE);
    set_vim_var_nr(VV_TYPE_TUPLE,   VAR_TYPE_TUPLE);

    set_vim_var_nr(VV_ECHOSPACE,    sc_col - 1);

    set_vim_var_dict(VV_COLORNAMES, dict_alloc());
    set_vim_var_dict(VV_CLIPPROVIDERS, dict_alloc());

#ifdef FEAT_PYTHON3
    set_vim_var_nr(VV_PYTHON3_VERSION, python3_version());
#endif

    // Default for v:register is not 0 but '"'.  This is adjusted once the
    // clipboard has been setup by calling reset_reg_var().
    set_reg_var(0);
}

#यदि defined(EXITFREE)
/*
 * Free all vim variables information on exit
 */
    शून्य
evalvars_clear(शून्य)
{
    पूर्णाङ्क		    i;
    संरचना vimvar   *p;

    प्रति (i = 0; i < VV_LEN; ++i)
    {
	p = &vimvars[i];
	यदि (p->vv_di.di_tv.v_type == VAR_STRING)
	    VIM_CLEAR(p->vv_str);
	अन्यथा यदि (p->vv_di.di_tv.v_type == VAR_LIST)
	{
	    list_unref(p->vv_list);
	    p->vv_list = रिक्त;
	}
    }
    hash_clear(&vimvarht);
    hash_init(&vimvarht);  // garbage_collect() will access it
    hash_clear(&compat_hashtab);

    // global variables
    vars_clear(&globvarht);

    // Script-local variables. Clear all the variables here.
    // The scriptvar_T is cleared later in free_scriptnames(), because a
    // variable in one script might hold a reference to the whole scope of
    // another script.
    प्रति (i = 1; i <= script_वस्तुसंच.ga_len; ++i)
	vars_clear(&SCRIPT_VARS(i));
}
#endif

    पूर्णाङ्क
garbage_collect_globvars(पूर्णाङ्क copyID)
{
    प्रत्यागम set_ref_in_ht(&globvarht, copyID, रिक्त, रिक्त);
}

    पूर्णाङ्क
garbage_collect_vimvars(पूर्णाङ्क copyID)
{
    प्रत्यागम set_ref_in_ht(&vimvarht, copyID, रिक्त, रिक्त);
}

    पूर्णाङ्क
garbage_collect_scriptvars(पूर्णाङ्क copyID)
{
    पूर्णाङ्क		    i;
    पूर्णाङ्क		    idx;
    पूर्णाङ्क		    abort = FALSE;
    scriptitem_T    *si;

    प्रति (i = 1; i <= script_वस्तुसंच.ga_len; ++i)
    {
	abort = abort || set_ref_in_ht(&SCRIPT_VARS(i), copyID, रिक्त, रिक्त);

	si = SCRIPT_ITEM(i);
	प्रति (idx = 0; idx < si->sn_var_vals.ga_len; ++idx)
	{
	    svar_T    *sv = ((svar_T *)si->sn_var_vals.ga_data) + idx;

	    यदि (sv->sv_name != रिक्त)
		abort = abort || set_ref_in_item(sv->sv_tv, copyID, रिक्त, रिक्त, रिक्त);
	}
    }

    प्रत्यागम abort;
}

/*
 * Set an internal variable to a string value. Creates the variable if it does
 * not already exist.
 */
    शून्य
set_internal_string_var(char_u *name, char_u *मूल्य)
{
    char_u	*val;
    typval_T	*tvp;

    val = vim_strsave(मूल्य);
    यदि (val == रिक्त)
	प्रत्यागम;

    tvp = alloc_string_tv(val);
    यदि (tvp == रिक्त)
	प्रत्यागम;

    set_var(name, tvp, FALSE);
    free_tv(tvp);
}

    पूर्णाङ्क
eval_charconvert(
    char_u	*enc_from,
    char_u	*enc_to,
    char_u	*fname_from,
    char_u	*fname_to)
{
    पूर्णाङ्क		err = FALSE;
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;

    set_vim_var_string(VV_CC_FROM, enc_from, -1);
    set_vim_var_string(VV_CC_TO, enc_to, -1);
    set_vim_var_string(VV_FNAME_IN, fname_from, -1);
    set_vim_var_string(VV_FNAME_OUT, fname_to, -1);
    ctx = get_option_sctx("charconvert");
    यदि (ctx != रिक्त)
	current_sctx = *ctx;

    यदि (eval_to_bool(p_ccv, &err, रिक्त, FALSE, TRUE))
	err = TRUE;

    set_vim_var_string(VV_CC_FROM, रिक्त, -1);
    set_vim_var_string(VV_CC_TO, रिक्त, -1);
    set_vim_var_string(VV_FNAME_IN, रिक्त, -1);
    set_vim_var_string(VV_FNAME_OUT, रिक्त, -1);
    current_sctx = saved_sctx;

    यदि (err)
	प्रत्यागम FAIL;
    प्रत्यागम OK;
}

#यदि defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
    पूर्णाङ्क
eval_printexpr(char_u *fname, char_u *args)
{
    पूर्णाङ्क		err = FALSE;
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;

    set_vim_var_string(VV_FNAME_IN, fname, -1);
    set_vim_var_string(VV_CMDARG, args, -1);
    ctx = get_option_sctx("printexpr");
    यदि (ctx != रिक्त)
	current_sctx = *ctx;

    यदि (eval_to_bool(p_pexpr, &err, रिक्त, FALSE, TRUE))
	err = TRUE;

    set_vim_var_string(VV_FNAME_IN, रिक्त, -1);
    set_vim_var_string(VV_CMDARG, रिक्त, -1);
    current_sctx = saved_sctx;

    यदि (err)
    {
	mch_remove(fname);
	प्रत्यागम FAIL;
    }
    प्रत्यागम OK;
}
#endif

#यदि defined(FEAT_DIFF)
    शून्य
eval_diff(
    char_u	*origfile,
    char_u	*newfile,
    char_u	*outfile)
{
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;
    typval_T	*tv;

    set_vim_var_string(VV_FNAME_IN, origfile, -1);
    set_vim_var_string(VV_FNAME_NEW, newfile, -1);
    set_vim_var_string(VV_FNAME_OUT, outfile, -1);

    ctx = get_option_sctx("diffexpr");
    यदि (ctx != रिक्त)
	current_sctx = *ctx;

    // errors are ignored
    tv = eval_अभिव्यक्ती_ext(p_dex, रिक्त, TRUE);
    free_tv(tv);

    set_vim_var_string(VV_FNAME_IN, रिक्त, -1);
    set_vim_var_string(VV_FNAME_NEW, रिक्त, -1);
    set_vim_var_string(VV_FNAME_OUT, रिक्त, -1);
    current_sctx = saved_sctx;
}

    शून्य
eval_patch(
    char_u	*origfile,
    char_u	*difffile,
    char_u	*outfile)
{
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;
    typval_T	*tv;

    set_vim_var_string(VV_FNAME_IN, origfile, -1);
    set_vim_var_string(VV_FNAME_DIFF, difffile, -1);
    set_vim_var_string(VV_FNAME_OUT, outfile, -1);

    ctx = get_option_sctx("patchexpr");
    यदि (ctx != रिक्त)
	current_sctx = *ctx;

    // errors are ignored
    tv = eval_अभिव्यक्ती_ext(p_pex, रिक्त, TRUE);
    free_tv(tv);

    set_vim_var_string(VV_FNAME_IN, रिक्त, -1);
    set_vim_var_string(VV_FNAME_DIFF, रिक्त, -1);
    set_vim_var_string(VV_FNAME_OUT, रिक्त, -1);
    current_sctx = saved_sctx;
}
#endif

#यदि defined(FEAT_SPELL)
/*
 * Evaluate an expression to a list with suggestions.
 * For the "expr:" part of 'spellsuggest'.
 * Returns NULL when there is an error.
 */
    list_T *
eval_spell_अभिव्यक्ती(char_u *badword, char_u *अभिव्यक्ती)
{
    typval_T	save_val;
    typval_T	rettv;
    list_T	*list = रिक्त;
    char_u	*p = skipwhite(अभिव्यक्ती);
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;
    पूर्णाङ्क		r;

    // Set "v:val" to the bad word.
    prepare_vimvar(VV_VAL, &save_val);
    set_vim_var_string(VV_VAL, badword, -1);
    यदि (p_verbose == 0)
	++emsg_off;
    ctx = get_option_sctx("spellsuggest");
    यदि (ctx != रिक्त)
	current_sctx = *ctx;

    r = may_call_simple_func(p, &rettv);
    यदि (r == NOTDONE)
	r = eval1(&p, &rettv, &EVALARG_EVALUATE);
    यदि (r == OK)
    {
	यदि (rettv.v_type != VAR_LIST)
	    clear_tv(&rettv);
	अन्यथा
	    list = rettv.vval.v_list;
    }

    यदि (p_verbose == 0)
	--emsg_off;
    clear_tv(get_vim_var_tv(VV_VAL));
    restore_vimvar(VV_VAL, &save_val);
    current_sctx = saved_sctx;

    प्रत्यागम list;
}

/*
 * "list" is supposed to contain two items: a word and a number.  Return the
 * word in "pp" and the number as the return value.
 * Return -1 if anything isn't right.
 * Used to get the good word and score from the eval_spell_expr() result.
 */
    पूर्णाङ्क
get_spellword(list_T *list, char_u **pp)
{
    listitem_T	*li;

    li = list->lv_first;
    यदि (li == रिक्त)
	प्रत्यागम -1;
    *pp = tv_get_string(&li->li_tv);

    li = li->li_next;
    यदि (li == रिक्त)
	प्रत्यागम -1;
    प्रत्यागम (पूर्णाङ्क)tv_get_संख्या(&li->li_tv);
}
#endif

/*
 * Prepare v: variable "idx" to be used.
 * Save the current typeval in "save_tv" and clear it.
 * When not used yet add the variable to the v: hashtable.
 */
    शून्य
prepare_vimvar(पूर्णाङ्क idx, typval_T *save_tv)
{
    *save_tv = vimvars[idx].vv_tv;
    vimvars[idx].vv_str = रिक्त;  // don't free it now
    यदि (vimvars[idx].vv_tv_type == VAR_UNKNOWN)
	hash_add(&vimvarht, vimvars[idx].vv_di.di_किल्ली, "prepare vimvar");
}

/*
 * Restore v: variable "idx" to typeval "save_tv".
 * Note that the v: variable must have been cleared already.
 * When no longer defined, remove the variable from the v: hashtable.
 */
    शून्य
restore_vimvar(पूर्णाङ्क idx, typval_T *save_tv)
{
    hashitem_T	*hi;

    vimvars[idx].vv_tv = *save_tv;
    यदि (vimvars[idx].vv_tv_type != VAR_UNKNOWN)
	प्रत्यागम;

    hi = hash_find(&vimvarht, vimvars[idx].vv_di.di_किल्ली);
    यदि (HASHITEM_EMPTY(hi))
	internal_error("restore_vimvar()");
    अन्यथा
	hash_remove(&vimvarht, hi, "restore vimvar");
}

/*
 * List Vim variables.
 */
    स्थिर शून्य
list_vim_vars(पूर्णाङ्क *first)
{
    list_hashtable_vars(&vimvarht, "v:", FALSE, first);
}

/*
 * List script-local variables, if there is a script.
 */
    स्थिर शून्य
list_script_vars(पूर्णाङ्क *first)
{
    यदि (SCRIPT_ID_VALID(current_sctx.sc_sid))
	list_hashtable_vars(&SCRIPT_VARS(current_sctx.sc_sid),
							   "s:", FALSE, first);
}

/*
 * Return TRUE if "name" starts with "g:", "w:", "t:" or "b:".
 * But only when an identifier character follows.
 */
    पूर्णाङ्क
is_scoped_variable(char_u *name)
{
    प्रत्यागम vim_strchr((char_u *)"gwbt", name[0]) != रिक्त
	&& name[1] == ':'
	&& eval_isnamec(name[2]);
}

/*
 * Evaluate one Vim expression {expr} in string "p" and append the
 * resulting string to "gap".  "p" points to the opening "{".
 * When "evaluate" is FALSE only skip over the expression.
 * Return a pointer to the character after "}", NULL for an error.
 */
    char_u *
eval_one_अभिव्यक्ती_in_str(char_u *p, garray_T *gap, पूर्णाङ्क evaluate)
{
    char_u	*block_start = skipwhite(p + 1);  // skip the opening {
    char_u	*block_end = block_start;
    char_u	*अभिव्यक्ती_val;

    यदि (*block_start == NUL)
    {
	semsg(_(e_missing_close_curly_str), p);
	प्रत्यागम रिक्त;
    }
    यदि (skip_अभिव्यक्ती(&block_end, रिक्त) == FAIL)
	प्रत्यागम रिक्त;
    block_end = skipwhite(block_end);
    यदि (*block_end != '}')
    {
	semsg(_(e_missing_close_curly_str), p);
	प्रत्यागम रिक्त;
    }
    यदि (evaluate)
    {
	*block_end = NUL;
	अभिव्यक्ती_val = eval_to_string(block_start, FALSE, FALSE);
	*block_end = '}';
	यदि (अभिव्यक्ती_val == रिक्त)
	    प्रत्यागम रिक्त;
	ga_concat(gap, अभिव्यक्ती_val);
	vim_free(अभिव्यक्ती_val);
    }

    प्रत्यागम block_end + 1;
}

/*
 * Evaluate all the Vim expressions {expr} in "str" and return the resulting
 * string in allocated memory.  "{{" is reduced to "{" and "}}" to "}".
 * Used for a heredoc assignment.
 * Returns NULL for an error.
 */
    स्थिर char_u *
eval_all_अभिव्यक्ती_in_str(char_u *str)
{
    garray_T	ga;
    char_u	*p;

    ga_init2(&ga, 1, 80);
    p = str;

    यावत् (*p != NUL)
    {
	char_u	*lit_start;
	पूर्णाङ्क	escaped_brace = FALSE;

	// Look for a block start.
	lit_start = p;
	यावत् (*p != '{' && *p != '}' && *p != NUL)
	    ++p;

	यदि (*p != NUL && *p == p[1])
	{
	    // Escaped brace, unescape and continue.
	    // Include the brace in the literal string.
	    ++p;
	    escaped_brace = TRUE;
	}
	अन्यथा यदि (*p == '}')
	{
	    semsg(_(e_stray_closing_curly_str), str);
	    ga_clear(&ga);
	    प्रत्यागम रिक्त;
	}

	// Append the literal part.
	ga_concat_len(&ga, lit_start, (size_t)(p - lit_start));

	यदि (*p == NUL)
	    भङ्ग;

	यदि (escaped_brace)
	{
	    // Skip the second brace.
	    ++p;
	    अनुवर्त;
	}

	// Evaluate the expression and append the result.
	p = eval_one_अभिव्यक्ती_in_str(p, &ga, TRUE);
	यदि (p == रिक्त)
	{
	    ga_clear(&ga);
	    प्रत्यागम रिक्त;
	}
    }
    ga_append(&ga, NUL);

    प्रत्यागम ga.ga_data;
}

/*
 * Get a list of lines from a HERE document. The here document is a list of
 * lines surrounded by a marker.
 *	cmd << {marker}
 *	  {line1}
 *	  {line2}
 *	  ....
 *	{marker}
 *
 * The {marker} is a string. If the optional 'trim' word is supplied before the
 * marker, then the leading indentation before the lines (matching the
 * indentation in the "cmd" line) is stripped.
 *
 * When getting lines for an embedded script (e.g. python, lua, perl, ruby,
 * tcl, mzscheme), "script_get" is set to TRUE. In this case, if the marker is
 * missing, then '.' is accepted as a marker.
 *
 * When compiling a heredoc assignment to a variable in a Vim9 def function,
 * "vim9compile" is set to TRUE. In this case, instead of generating a list of
 * string values from the heredoc, vim9 instructions are generated.  On success
 * the returned list will be empty.
 *
 * Returns a List with {lines} or NULL on failure.
 */
    list_T *
heredoc_get(exarg_T *eap, char_u *cmd, पूर्णाङ्क script_get, पूर्णाङ्क vim9compile)
{
    char_u	*theline = रिक्त;
    char_u	*marker;
    list_T	*l;
    char_u	*p;
    char_u	*str;
    पूर्णाङ्क		marker_indent_len = 0;
    पूर्णाङ्क		text_indent_len = 0;
    char_u	*text_indent = रिक्त;
    char_u	dot[] = ".";
    पूर्णाङ्क		comment_char = in_vim9script() ? '#' : '"';
    पूर्णाङ्क		evalstr = FALSE;
    पूर्णाङ्क		eval_failed = FALSE;
    cctx_T	*cctx = vim9compile ? eap->cookie : रिक्त;
    पूर्णाङ्क		मोजणी = 0;
    पूर्णाङ्क		heredoc_in_string = FALSE;
    char_u	*ओळ_arg = रिक्त;
    char_u	*nl_ptr = vim_strchr(cmd, '\n');

    यदि (nl_ptr != रिक्त)
    {
	heredoc_in_string = TRUE;
	ओळ_arg = nl_ptr + 1;
	*nl_ptr = NUL;
    }
    अन्यथा यदि (eap->ea_getline == रिक्त)
    {
	emsg(_(e_cannot_use_heredoc_here));
	प्रत्यागम रिक्त;
    }

    // Check for the optional 'trim' word before the marker
    cmd = skipwhite(cmd);

    यावत् (TRUE)
    {
	यदि (STRNCMP(cmd, "trim", 4) == 0
		&& (cmd[4] == NUL || VIM_ISWHITE(cmd[4])))
	{
	    cmd = skipwhite(cmd + 4);

	    // Trim the indentation from all the lines in the here document.
	    // The amount of indentation trimmed is the same as the indentation
	    // of the first line after the :let command line.  To find the end
	    // marker the indent of the :let command line is trimmed.
	    p = *eap->cmdlinep;
	    यावत् (VIM_ISWHITE(*p))
	    {
		p++;
		marker_indent_len++;
	    }
	    text_indent_len = -1;

	    अनुवर्त;
	}
	यदि (STRNCMP(cmd, "eval", 4) == 0
		&& (cmd[4] == NUL || VIM_ISWHITE(cmd[4])))
	{
	    cmd = skipwhite(cmd + 4);
	    evalstr = TRUE;
	    अनुवर्त;
	}
	भङ्ग;
    }

    // The marker is the next word.
    यदि (*cmd != NUL && *cmd != comment_char)
    {
	marker = skipwhite(cmd);
	p = skiptowhite(marker);
	यदि (*skipwhite(p) != NUL && *skipwhite(p) != comment_char)
	{
	    semsg(_(e_trailing_characters_str), p);
	    प्रत्यागम रिक्त;
	}
	*p = NUL;
	यदि (!script_get && vim_islower(*marker))
	{
	    emsg(_(e_marker_cannot_start_with_lower_case_letter));
	    प्रत्यागम रिक्त;
	}
    }
    अन्यथा
    {
	// When getting lines for an embedded script, if the marker is missing,
	// accept '.' as the marker.
	यदि (script_get)
	    marker = dot;
	अन्यथा
	{
	    emsg(_(e_missing_marker));
	    प्रत्यागम रिक्त;
	}
    }

    l = list_alloc();
    यदि (l == रिक्त)
	प्रत्यागम रिक्त;

    प्रति (;;)
    {
	पूर्णाङ्क	mi = 0;
	पूर्णाङ्क	ti = 0;

	यदि (heredoc_in_string)
	{
	    char_u	*next_ओळ;

	    // heredoc in a string separated by newlines.  Get the next line
	    // from the string.

	    यदि (*ओळ_arg == NUL)
	    {
		semsg(_(e_missing_end_marker_str), marker);
		भङ्ग;
	    }

	    theline = ओळ_arg;
	    next_ओळ = vim_strchr(theline, '\n');
	    यदि (next_ओळ == रिक्त)
		ओळ_arg += STRLEN(ओळ_arg);
	    अन्यथा
	    {
		*next_ओळ = NUL;
		ओळ_arg = next_ओळ + 1;
	    }
	}
	अन्यथा
	{
	    vim_free(theline);
	    theline = eap->ea_getline(NUL, eap->cookie, 0, FALSE);
	    यदि (theline == रिक्त)
	    {
		semsg(_(e_missing_end_marker_str), marker);
		भङ्ग;
	    }
	}

	// with "trim": skip the indent matching the :let line to find the
	// marker
	यदि (marker_indent_len > 0
		&& STRNCMP(theline, *eap->cmdlinep, marker_indent_len) == 0)
	    mi = marker_indent_len;
	यदि (STRCMP(marker, theline + mi) == 0)
	    भङ्ग;

	// If expression evaluation failed in the heredoc, then skip till the
	// end marker.
	यदि (eval_failed)
	    अनुवर्त;

	यदि (text_indent_len == -1 && *theline != NUL)
	{
	    // set the text indent from the first line.
	    p = theline;
	    text_indent_len = 0;
	    यावत् (VIM_ISWHITE(*p))
	    {
		p++;
		text_indent_len++;
	    }
	    text_indent = vim_strnsave(theline, text_indent_len);
	}
	// with "trim": skip the indent matching the first line
	यदि (text_indent != रिक्त)
	    प्रति (ti = 0; ti < text_indent_len; ++ti)
		यदि (theline[ti] != text_indent[ti])
		    भङ्ग;

	str = theline + ti;
	यदि (vim9compile)
	{
	    यदि (compile_all_अभिव्यक्ती_in_str(str, evalstr, cctx) == FAIL)
	    {
		vim_free(theline);
		vim_free(text_indent);
		list_free(l);
		प्रत्यागम FAIL;
	    }
	    मोजणी++;
	}
	अन्यथा
	{
	    पूर्णाङ्क	    free_str = FALSE;

	    यदि (evalstr && !eap->skip)
	    {
		str = eval_all_अभिव्यक्ती_in_str(str);
		यदि (str == रिक्त)
		{
		    // expression evaluation failed
		    eval_failed = TRUE;
		    अनुवर्त;
		}
		free_str = TRUE;
	    }

	    यदि (list_append_string(l, str, -1) == FAIL)
	    {
		यदि (free_str)
		    vim_free(str);
		भङ्ग;
	    }
	    यदि (free_str)
		vim_free(str);
	}
    }
    यदि (heredoc_in_string)
	// Next command follows the heredoc in the string.
	eap->nextcmd = ओळ_arg;
    अन्यथा
	vim_free(theline);
    vim_free(text_indent);

    यदि (vim9compile && cctx->ctx_skip != SKIP_YES && !eval_failed)
	generate_NEWLIST(cctx, मोजणी, FALSE);

    यदि (eval_failed)
    {
	// expression evaluation in the heredoc failed
	list_free(l);
	प्रत्यागम रिक्त;
    }
    प्रत्यागम l;
}

/*
 * Vim9 variable declaration:
 * ":var name"
 * ":var name: type"
 * ":var name = expr"
 * ":var name: type = expr"
 * etc.
 */
    शून्य
ex_var(exarg_T *eap)
{
    char_u *p = eap->cmd;
    पूर्णाङ्क	    has_var;

    यदि (!in_vim9script())
    {
	semsg(_(e_str_cannot_be_used_in_legacy_vim_script), ":var");
	प्रत्यागम;
    }
    has_var = checkforcmd_noparen(&p, "var", 3);
    यदि (current_sctx.sc_sid == 0 && has_var)
    {
	emsg(_(e_cannot_declare_variable_on_command_ओळ));
	प्रत्यागम;
    }
    ex_let(eap);
}

/*
 * ":let"			list all variable values
 * ":let var1 var2"		list variable values
 * ":let var = expr"		assignment command.
 * ":let var += expr"		assignment command.
 * ":let var -= expr"		assignment command.
 * ":let var *= expr"		assignment command.
 * ":let var /= expr"		assignment command.
 * ":let var %= expr"		assignment command.
 * ":let var .= expr"		assignment command.
 * ":let var ..= expr"		assignment command.
 * ":let [var1, var2] = expr"	unpack list.
 * ":let var =<< ..."		heredoc
 * ":let var: string"		Vim9 declaration
 *
 * ":final var = expr"		assignment command.
 * ":final [var1, var2] = expr"	unpack list.
 *
 * ":const"			list all variable values
 * ":const var1 var2"		list variable values
 * ":const var = expr"		assignment command.
 * ":const [var1, var2] = expr"	unpack list.
 */
    शून्य
ex_let(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    char_u	*अभिव्यक्ती = रिक्त;
    typval_T	rettv;
    पूर्णाङ्क		var_मोजणी = 0;
    पूर्णाङ्क		semicolon = 0;
    char_u	op[4];
    char_u	*argend;
    पूर्णाङ्क		first = TRUE;
    पूर्णाङ्क		concat;
    पूर्णाङ्क		has_assign;
    पूर्णाङ्क		flags = 0;
    पूर्णाङ्क		vim9script = in_vim9script();

    यदि (eap->cmdidx == CMD_final && !vim9script)
    {
	// In legacy Vim script ":final" is short for ":finally".
	ex_finally(eap);
	प्रत्यागम;
    }
    यदि (eap->cmdidx == CMD_let && vim9script)
    {
	emsg(_(e_cannot_use_let_in_vim9_script));
	प्रत्यागम;
    }

    यदि (eap->cmdidx == CMD_const)
	flags |= ASSIGN_CONST;
    अन्यथा यदि (eap->cmdidx == CMD_final)
	flags |= ASSIGN_FINAL;

    // Vim9 assignment without ":let", ":const" or ":final"
    यदि (eap->arg == eap->cmd)
	flags |= ASSIGN_NO_DECL;

    argend = skip_var_list(arg, TRUE, &var_मोजणी, &semicolon, FALSE);
    यदि (argend == रिक्त)
	प्रत्यागम;
    अभिव्यक्ती = skipwhite(argend);
    concat = अभिव्यक्ती[0] == '.'
	&& ((अभिव्यक्ती[1] == '=' && in_old_script(2))
		|| (अभिव्यक्ती[1] == '.' && अभिव्यक्ती[2] == '='));
    has_assign =  *अभिव्यक्ती == '=' || (vim_strchr((char_u *)"+-*/%", *अभिव्यक्ती) != रिक्त
							    && अभिव्यक्ती[1] == '=');
    यदि (!has_assign && !concat)
    {
	// ":let" without "=": list variables
	यदि (*arg == '[')
	    emsg(_(e_invalid_argument));
	अन्यथा यदि (अभिव्यक्ती[0] == '.' && अभिव्यक्ती[1] == '=')
	    emsg(_(e_dot_equal_not_supported_with_script_version_two));
	अन्यथा यदि (!ends_excmd2(eap->cmd, arg))
	{
	    यदि (vim9script)
	    {
		यदि (!ends_excmd2(eap->cmd, skipwhite(argend)))
		    semsg(_(e_trailing_characters_str), argend);
		अन्यथा
		    // Vim9 declaration ":var name: type"
		    arg = vim9_declare_scriptvar(eap, arg);
	    }
	    अन्यथा
	    {
		// ":let var1 var2" - list values
		arg = list_arg_vars(eap, arg, &first);
	    }
	}
	अन्यथा यदि (!eap->skip)
	{
	    // ":let"
	    list_glob_vars(&first);
	    list_buf_vars(&first);
	    list_win_vars(&first);
	    list_tab_vars(&first);
	    list_script_vars(&first);
	    list_func_vars(&first);
	    list_vim_vars(&first);
	}
	set_nextcmd(eap, arg);
	प्रत्यागम;
    }

    यदि (अभिव्यक्ती[0] == '=' && अभिव्यक्ती[1] == '<' && अभिव्यक्ती[2] == '<')
    {
	list_T	*l = रिक्त;
	long	cur_lnum = SOURCING_LNUM;

	// :let text =<< [trim] [eval] END
	// :var text =<< [trim] [eval] END
	यदि (vim9script && !eap->skip && (!VIM_ISWHITE(अभिव्यक्ती[-1])
						 || !IS_WHITE_OR_NUL(अभिव्यक्ती[3])))
	    semsg(_(e_white_space_required_before_and_after_str_at_str),
								  "=<<", अभिव्यक्ती);
	अन्यथा
	    l = heredoc_get(eap, अभिव्यक्ती + 3, FALSE, FALSE);

	यदि (l != रिक्त)
	{
	    rettv_list_set(&rettv, l);
	    यदि (!eap->skip)
	    {
		// errors are for the assignment, not the end marker
		SOURCING_LNUM = cur_lnum;
		op[0] = '=';
		op[1] = NUL;
		(शून्य)ex_let_vars(eap->arg, &rettv, FALSE, semicolon, var_मोजणी,
								flags, op);
	    }
	    clear_tv(&rettv);
	}
	प्रत्यागम;
    }

    evalarg_T   evalarg;
    पूर्णाङ्क	    len = 1;

    CLEAR_FIELD(rettv);

    पूर्णाङ्क cur_lnum;

    op[0] = '=';
    op[1] = NUL;
    यदि (*अभिव्यक्ती != '=')
    {
	यदि (vim9script && (flags & ASSIGN_NO_DECL) == 0)
	{
	    // +=, /=, etc. require an existing variable
	    semsg(_(e_cannot_use_operator_on_new_variable_str), eap->arg);
	}
	अन्यथा यदि (vim_strchr((char_u *)"+-*/%.", *अभिव्यक्ती) != रिक्त)
	{
	    op[0] = *अभिव्यक्ती;   // +=, -=, *=, /=, %= or .=
	    ++len;
	    यदि (अभिव्यक्ती[0] == '.' && अभिव्यक्ती[1] == '.') // ..=
	    {
		++अभिव्यक्ती;
		++len;
	    }
	}
	अभिव्यक्ती += 2;
    }
    अन्यथा
	++अभिव्यक्ती;

    यदि (vim9script && !eap->skip && (!VIM_ISWHITE(*argend)
					       || !IS_WHITE_OR_NUL(*अभिव्यक्ती)))
    {
	vim_strncpy(op, अभिव्यक्ती - len, len);
	semsg(_(e_white_space_required_before_and_after_str_at_str),
							       op, argend);
    }

    यदि (eap->skip)
	++emsg_skip;
    fill_evalarg_from_eap(&evalarg, eap, eap->skip);
    अभिव्यक्ती = skipwhite_and_linebreak(अभिव्यक्ती, &evalarg);
    cur_lnum = SOURCING_LNUM;
    पूर्णाङ्क eval_res = eval0(अभिव्यक्ती, &rettv, eap, &evalarg);
    यदि (eap->skip)
	--emsg_skip;
    clear_evalarg(&evalarg, eap);

    // Restore the line number so that any type error is given for the
    // declaration, not the expression.
    SOURCING_LNUM = cur_lnum;

    यदि (!eap->skip && eval_res != FAIL)
	(शून्य)ex_let_vars(eap->arg, &rettv, FALSE, semicolon, var_मोजणी,
								    flags, op);
    यदि (eval_res != FAIL)
	clear_tv(&rettv);
}

/*
 * Assign the typeval "tv" to the variable or variables at "arg_start".
 * Handles both "var" with any type and "[var, var; var]" with a list type.
 * When "op" is not NULL it points to a string with characters that
 * must appear after the variable(s).  Use "+", "-" or "." for add, subtract
 * or concatenate.
 * Returns OK or FAIL;
 */
    पूर्णाङ्क
ex_let_vars(
    char_u	*arg_start,
    typval_T	*tv,
    पूर्णाङ्क		copy,		// copy values from "tv", don't move
    पूर्णाङ्क		semicolon,	// from skip_var_list()
    पूर्णाङ्क		var_मोजणी,	// from skip_var_list()
    पूर्णाङ्क		flags,		// ASSIGN_FINAL, ASSIGN_CONST, etc.
    char_u	*op)
{
    char_u	*arg = arg_start;
    list_T	*l;
    tuple_T	*tuple = रिक्त;
    पूर्णाङ्क		i;
    पूर्णाङ्क		var_idx = 0;
    listitem_T	*item = रिक्त;
    typval_T	ltv;
    पूर्णाङ्क		is_list = tv->v_type == VAR_LIST;
    पूर्णाङ्क		idx;

    यदि (tv->v_type == VAR_VOID)
    {
	emsg(_(e_cannot_use_void_मूल्य));
	प्रत्यागम FAIL;
    }
    यदि (*arg != '[')
    {
	// ":let var = expr" or ":for var in list"
	यदि (ex_let_one(arg, tv, copy, flags, op, op, var_idx) == रिक्त)
	    प्रत्यागम FAIL;
	प्रत्यागम OK;
    }

    // ":let [v1, v2] = list" or ":for [v1, v2] in listlist"
    // or
    // ":let [v1, v2] = tuple" or ":for [v1, v2] in tupletuple"
    यदि (tv->v_type != VAR_LIST && tv->v_type != VAR_TUPLE)
    {
	emsg(_(e_list_or_tuple_required));
	प्रत्यागम FAIL;
    }
    यदि (is_list)
    {
	l = tv->vval.v_list;
	यदि (l == रिक्त)
	{
	    emsg(_(e_list_required));
	    प्रत्यागम FAIL;
	}
	i = list_len(l);
    }
    अन्यथा
    {
	tuple = tv->vval.v_tuple;
	यदि (tuple == रिक्त)
	{
	    emsg(_(e_tuple_required));
	    प्रत्यागम FAIL;
	}
	i = tuple_len(tuple);
    }

    यदि (semicolon == 0 && var_मोजणी < i)
    {
	emsg(_(is_list ? e_less_targets_than_list_वस्तुसंच
					: e_less_targets_than_tuple_वस्तुसंच));
	प्रत्यागम FAIL;
    }
    यदि (var_मोजणी - semicolon > i)
    {
	emsg(_(is_list ? e_more_targets_than_list_वस्तुसंच
					: e_more_targets_than_tuple_वस्तुसंच));
	प्रत्यागम FAIL;
    }

    यदि (is_list)
    {
	CHECK_LIST_MATERIALIZE(l);
	item = l->lv_first;
    }
    अन्यथा
	idx = 0;

    यावत् (*arg != ']')
    {
	arg = skipwhite(arg + 1);
	++var_idx;
	arg = ex_let_one(arg, is_list ? &item->li_tv : TUPLE_ITEM(tuple, idx),
			 TRUE, flags | ASSIGN_UNPACK, (char_u *)",;]", op,
			 var_idx);
	यदि (is_list)
	    item = item->li_next;
	अन्यथा
	    idx++;
	यदि (arg == रिक्त)
	    प्रत्यागम FAIL;

	arg = skipwhite(arg);
	यदि (*arg == ';')
	{
	    // Put the rest of the list or tuple (may be empty) in the var
	    // after ';'.  Create a new list or tuple for this.
	    यदि (is_list)
	    {
		// Put the rest of the list (may be empty) in the var
		// after ';'.  Create a new list for this.
		l = list_alloc();
		यदि (l == रिक्त)
		    प्रत्यागम FAIL;

		// list
		यावत् (item != रिक्त)
		{
		    list_append_tv(l, &item->li_tv);
		    item = item->li_next;
		}

		ltv.v_type = VAR_LIST;
		ltv.v_lock = 0;
		ltv.vval.v_list = l;
		l->lv_refcount = 1;
	    }
	    अन्यथा
	    {
		tuple_T	*new_tuple = tuple_alloc();
		यदि (new_tuple == रिक्त)
		    प्रत्यागम FAIL;

		// Put the rest of the tuple (may be empty) in the var
		// after ';'.  Create a new tuple for this.
		यावत् (idx < TUPLE_LEN(tuple))
		{
		    typval_T    new_tv;

		    copy_tv(TUPLE_ITEM(tuple, idx), &new_tv);
		    यदि (tuple_append_tv(new_tuple, &new_tv) == FAIL)
		    {
			clear_tv(&new_tv);
			tuple_unref(new_tuple);
			प्रत्यागम FAIL;
		    }
		    idx++;
		}

		ltv.v_type = VAR_TUPLE;
		ltv.v_lock = 0;
		ltv.vval.v_tuple = new_tuple;
		new_tuple->tv_refcount = 1;
	    }

	    ++var_idx;
	    arg = ex_let_one(skipwhite(arg + 1), &ltv, FALSE,
			flags | ASSIGN_UNPACK, (char_u *)"]", op, var_idx);
	    clear_tv(&ltv);
	    यदि (arg == रिक्त)
		प्रत्यागम FAIL;
	    भङ्ग;
	}
	अन्यथा यदि (*arg != ',' && *arg != ']')
	{
	    internal_error("ex_let_vars()");
	    प्रत्यागम FAIL;
	}
    }

    प्रत्यागम OK;
}

/*
 * Skip over assignable variable "var" or list of variables "[var, var]".
 * Used for ":let varvar = expr" and ":for varvar in expr".
 * For "[var, var]" increment "*var_count" for each variable.
 * for "[var, var; var]" set "semicolon" to 1.
 * If "silent" is TRUE do not give an "invalid argument" error message.
 * Return NULL for an error.
 */
    char_u *
skip_var_list(
    char_u	*arg,
    पूर्णाङ्क		include_type,
    पूर्णाङ्क		*var_मोजणी,
    पूर्णाङ्क		*semicolon,
    पूर्णाङ्क		silent)
{
    char_u	*p, *s;

    यदि (*arg == '[')
    {
	// "[var, var]": find the matching ']'.
	p = arg;
	प्रति (;;)
	{
	    p = skipwhite(p + 1);	// skip whites after '[', ';' or ','
	    s = skip_var_one(p, include_type);
	    यदि (s == p)
	    {
		यदि (!silent)
		    semsg(_(e_invalid_argument_str), p);
		प्रत्यागम रिक्त;
	    }
	    ++*var_मोजणी;

	    p = skipwhite(s);
	    यदि (*p == ']')
		भङ्ग;
	    अन्यथा यदि (*p == ';')
	    {
		यदि (*semicolon == 1)
		{
		    यदि (!silent)
			emsg(_(e_double_semicolon_in_list_of_variables));
		    प्रत्यागम रिक्त;
		}
		*semicolon = 1;
	    }
	    अन्यथा यदि (*p != ',')
	    {
		यदि (!silent)
		    semsg(_(e_invalid_argument_str), p);
		प्रत्यागम रिक्त;
	    }
	}
	प्रत्यागम p + 1;
    }

    प्रत्यागम skip_var_one(arg, include_type);
}

/*
 * Skip one (assignable) variable name, including @r, $VAR, &option, d.key,
 * l[idx].
 * In Vim9 script also skip over ": type" if "include_type" is TRUE.
 */
    char_u *
skip_var_one(char_u *arg, पूर्णाङ्क include_type)
{
    char_u	*end;
    पूर्णाङ्क		vim9 = in_vim9script();

    यदि (*arg == '@' && arg[1] != NUL)
	प्रत्यागम arg + 2;

    // termcap option name may have non-alpha characters
    यदि (STRNCMP(arg, "&t_", 3) == 0 && arg[3] != NUL && arg[4] != NUL)
	प्रत्यागम arg + 5;

    end = find_name_end(*arg == '$' || *arg == '&' ? arg + 1 : arg,
				   रिक्त, रिक्त, FNE_INCL_BR | FNE_CHECK_START);

    // "a: type" is declaring variable "a" with a type, not "a:".
    // Same for "s: type".
    यदि (vim9 && end == arg + 2 && end[-1] == ':')
	--end;

    यदि (include_type && vim9)
    {
	यदि (*skipwhite(end) == ':')
	    end = skip_type(skipwhite(skipwhite(end) + 1), FALSE);
    }
    प्रत्यागम end;
}

/*
 * List variables for hashtab "ht" with prefix "prefix".
 * If "empty" is TRUE also list NULL strings as empty strings.
 */
    शून्य
list_hashtable_vars(
    hashtab_T	*ht,
    वर्ण	*prefix,
    पूर्णाङ्क		empty,
    पूर्णाङ्क		*first)
{
    hashitem_T	*hi;
    dictitem_T	*di;
    पूर्णाङ्क		todo;
    char_u	buf[IOSIZE];

    पूर्णाङ्क save_ht_flags = ht->ht_flags;
    ht->ht_flags |= HTFLAGS_FROZEN;

    todo = (पूर्णाङ्क)ht->ht_used;
    प्रति (hi = ht->ht_array; todo > 0 && !got_int; ++hi)
    {
	यदि (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    di = HI2DI(hi);

	    // apply :filter /pat/ to variable name
	    vim_strncpy((char_u *)buf, (char_u *)prefix, IOSIZE - 1);
	    vim_strcat((char_u *)buf, di->di_किल्ली, IOSIZE);
	    यदि (message_filtered(buf))
		अनुवर्त;

	    यदि (empty || di->di_tv.v_type != VAR_STRING
					   || di->di_tv.vval.v_string != रिक्त)
		list_one_var(di, prefix, first);
	}
    }

    ht->ht_flags = save_ht_flags;
}

/*
 * List global variables.
 */
    स्थिर शून्य
list_glob_vars(पूर्णाङ्क *first)
{
    list_hashtable_vars(&globvarht, "", TRUE, first);
}

/*
 * List buffer variables.
 */
    स्थिर शून्य
list_buf_vars(पूर्णाङ्क *first)
{
    list_hashtable_vars(&curbuf->b_vars->dv_hashtab, "b:", TRUE, first);
}

/*
 * List window variables.
 */
    स्थिर शून्य
list_win_vars(पूर्णाङ्क *first)
{
    list_hashtable_vars(&curwin->w_vars->dv_hashtab, "w:", TRUE, first);
}

/*
 * List tab page variables.
 */
    स्थिर शून्य
list_tab_vars(पूर्णाङ्क *first)
{
    list_hashtable_vars(&curtab->tp_vars->dv_hashtab, "t:", TRUE, first);
}

/*
 * List variables in "arg".
 */
    स्थिर char_u *
list_arg_vars(exarg_T *eap, char_u *arg, पूर्णाङ्क *first)
{
    पूर्णाङ्क		error = FALSE;
    पूर्णाङ्क		len;
    char_u	*name;
    char_u	*name_start;
    char_u	*arg_subsc;
    char_u	*tofree;
    typval_T    tv;

    यावत् (!ends_excmd2(eap->cmd, arg) && !got_int)
    {
	यदि (error || eap->skip)
	{
	    arg = find_name_end(arg, रिक्त, रिक्त, FNE_INCL_BR | FNE_CHECK_START);
	    यदि (!VIM_ISWHITE(*arg) && !ends_excmd(*arg))
	    {
		emsg_severe = TRUE;
		यदि (!did_emsg)
		    semsg(_(e_trailing_characters_str), arg);
		भङ्ग;
	    }
	}
	अन्यथा
	{
	    // get_name_len() takes care of expanding curly braces
	    name_start = name = arg;
	    len = get_name_len(&arg, &tofree, TRUE, TRUE);
	    यदि (len <= 0)
	    {
		// This is mainly to keep test 49 working: when expanding
		// curly braces fails overrule the exception error message.
		यदि (len < 0 && !aborting())
		{
		    emsg_severe = TRUE;
		    semsg(_(e_invalid_argument_str), arg);
		    भङ्ग;
		}
		error = TRUE;
	    }
	    अन्यथा
	    {
		arg = skipwhite(arg);
		यदि (tofree != रिक्त)
		    name = tofree;
		यदि (eval_variable(name, len, 0, &tv, रिक्त,
						     EVAL_VAR_VERBOSE) == FAIL)
		    error = TRUE;
		अन्यथा
		{
		    // handle d.key, l[idx], f(expr)
		    arg_subsc = arg;
		    यदि (handle_subscript(&arg, name_start, &tv,
					      &EVALARG_EVALUATE, TRUE) == FAIL)
			error = TRUE;
		    अन्यथा
		    {
			यदि (arg == arg_subsc && len == 2 && name[1] == ':')
			{
			    विकल्प (*name)
			    {
				पक्ष 'g': list_glob_vars(first); भङ्ग;
				पक्ष 'b': list_buf_vars(first); भङ्ग;
				पक्ष 'w': list_win_vars(first); भङ्ग;
				पक्ष 't': list_tab_vars(first); भङ्ग;
				पक्ष 'v': list_vim_vars(first); भङ्ग;
				पक्ष 's': list_script_vars(first); भङ्ग;
				पक्ष 'l': list_func_vars(first); भङ्ग;
				स्वभाव:
					  semsg(_(e_cant_list_variables_for_str), name);
			    }
			}
			अन्यथा
			{
			    char_u	numbuf[NUMBUFLEN];
			    char_u	*tf;
			    पूर्णाङ्क		c;
			    char_u	*s;

			    s = echo_string(&tv, &tf, numbuf, 0);
			    c = *arg;
			    *arg = NUL;
			    list_one_var_a("",
				    arg == arg_subsc ? name : name_start,
				    tv.v_type,
				    s == रिक्त ? (char_u *)"" : s,
				    first);
			    *arg = c;
			    vim_free(tf);
			}
			clear_tv(&tv);
		    }
		}
	    }

	    vim_free(tofree);
	}

	arg = skipwhite(arg);
    }

    प्रत्यागम arg;
}

/*
 * Set an environment variable, part of ex_let_one().
 */
    स्थिर char_u *
ex_let_env(
    char_u	*arg,
    typval_T	*tv,
    पूर्णाङ्क		flags,
    char_u	*endchars,
    char_u	*op)
{
    char_u	*arg_end = रिक्त;
    char_u	*name;
    पूर्णाङ्क		len;

    यदि ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					 && (flags & ASSIGN_FOR_LOOP) == 0)
    {
	emsg(_(e_cannot_lock_environment_variable));
	प्रत्यागम रिक्त;
    }

    // Find the end of the name.
    ++arg;
    name = arg;
    len = get_env_len(&arg);
    यदि (len == 0)
	semsg(_(e_invalid_argument_str), name - 1);
    अन्यथा
    {
	यदि (op != रिक्त && vim_strchr((char_u *)"+-*/%", *op) != रिक्त)
	    semsg(_(e_wrong_variable_type_for_str_equal), op);
	अन्यथा यदि (endchars != रिक्त
			      && vim_strchr(endchars, *skipwhite(arg)) == रिक्त)
	    emsg(_(e_unexpected_characters_in_let));
	अन्यथा यदि (!check_secure() && !check_restricted())
	{
	    char_u	*tofree = रिक्त;
	    पूर्णाङ्क		c1 = name[len];
	    char_u	*p;

	    name[len] = NUL;
	    p = tv_get_string_chk(tv);
	    यदि (p != रिक्त && op != रिक्त && *op == '.')
	    {
		पूर्णाङ्क	mustfree = FALSE;
		char_u  *s = vim_getenv(name, &mustfree);

		यदि (s != रिक्त)
		{
		    p = tofree = concat_str(s, p);
		    यदि (mustfree)
			vim_free(s);
		}
	    }
	    यदि (p != रिक्त)
	    {
		vim_setenv_ext(name, p);
		arg_end = arg;
	    }
	    name[len] = c1;
	    vim_free(tofree);
	}
    }
    प्रत्यागम arg_end;
}

/*
 * Set an option, part of ex_let_one().
 */
    स्थिर char_u *
ex_let_option(
    char_u	*arg,
    typval_T	*tv,
    पूर्णाङ्क		flags,
    char_u	*endchars,
    char_u	*op)
{
    char_u	*p;
    पूर्णाङ्क		scope;
    char_u	*arg_end = रिक्त;

    यदि ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					 && (flags & ASSIGN_FOR_LOOP) == 0)
    {
	emsg(_(e_cannot_lock_option));
	प्रत्यागम रिक्त;
    }

    // Find the end of the name.
    p = find_option_end(&arg, &scope);
    यदि (p == रिक्त || (endchars != रिक्त
			       && vim_strchr(endchars, *skipwhite(p)) == रिक्त))
    {
	emsg(_(e_unexpected_characters_in_let));
	प्रत्यागम रिक्त;
    }

    पूर्णाङ्क		c1;
    long	n = 0;
    getoption_T	opt_type;
    long	numval;
    char_u	*stringval = रिक्त;
    char_u	*s = रिक्त;
    पूर्णाङ्क		failed = FALSE;
    पूर्णाङ्क		opt_p_flags;
    char_u	*tofree = रिक्त;
    char_u	numbuf[NUMBUFLEN];

    c1 = *p;
    *p = NUL;

    opt_type = get_option_मूल्य(arg, &numval, &stringval, &opt_p_flags, scope);
    यदि (opt_type == gov_unknown && arg[0] != 't' && arg[1] != '_')
    {
	semsg(_(e_unknown_option_str_2), arg);
	गच्छ theend;
    }
    यदि (op != रिक्त && *op != '='
	&& (((opt_type == gov_bool || opt_type == gov_संख्या) && *op == '.')
	    || (opt_type == gov_string && *op != '.')))
    {
	semsg(_(e_wrong_variable_type_for_str_equal), op);
	गच्छ theend;
    }

    यदि ((opt_type == gov_bool
		|| opt_type == gov_संख्या
		|| opt_type == gov_hidden_bool
		|| opt_type == gov_hidden_संख्या)
		     && (tv->v_type != VAR_STRING || !in_vim9script()))
    {
	यदि (opt_type == gov_bool || opt_type == gov_hidden_bool)
	    // bool, possibly hidden
	    n = (long)tv_get_bool_chk(tv, &failed);
	अन्यथा
	    // number, possibly hidden
	    n = (long)tv_get_संख्या_chk(tv, &failed);
	यदि (failed)
	    गच्छ theend;
    }

    यदि ((opt_p_flags & P_FUNC) && (tv->v_type == VAR_PARTIAL
						    || tv->v_type == VAR_FUNC))
    {
	// If the option can be set to a function reference or a lambda
	// and the passed value is a function reference, then convert it to
	// the name (string) of the function reference.
	s = tv2string(tv, &tofree, numbuf, 0);
	यदि (s == रिक्त)
	    गच्छ theend;
    }
    // Avoid setting a string option to the text "v:false" or similar.
    // In Vim9 script also don't convert a number to string.
    अन्यथा यदि (tv->v_type != VAR_BOOL && tv->v_type != VAR_SPECIAL
		     && (!in_vim9script() || tv->v_type != VAR_संख्या))
    {
	s = tv_get_string_chk(tv);
	यदि (s == रिक्त)
	    गच्छ theend;
    }
    अन्यथा यदि (opt_type == gov_string || opt_type == gov_hidden_string)
    {
	emsg(_(e_string_required));
	गच्छ theend;
    }

    यदि (op != रिक्त && *op != '=')
    {
	// number, in legacy script also bool
	यदि (opt_type == gov_संख्या
				 || (opt_type == gov_bool && !in_vim9script()))
	{
	    विकल्प (*op)
	    {
		पक्ष '+': n = numval + n; भङ्ग;
		पक्ष '-': n = numval - n; भङ्ग;
		पक्ष '*': n = numval * n; भङ्ग;
		पक्ष '/': n = (long)अंक_divide(numval, n, &failed); भङ्ग;
		पक्ष '%': n = (long)अंक_modulus(numval, n, &failed); भङ्ग;
	    }
	    s = रिक्त;
	    यदि (failed)
		गच्छ theend;
	}
	अन्यथा यदि (opt_type == gov_string && stringval != रिक्त && s != रिक्त)
	{
	    // string
	    s = concat_str(stringval, s);
	    vim_free(stringval);
	    stringval = s;
	}
    }

    वर्ण *err = set_option_मूल्य(arg, n, s, scope);
    arg_end = p;
    यदि (err != रिक्त)
	emsg(_(err));

theend:
    *p = c1;
    vim_free(stringval);
    vim_free(tofree);
    प्रत्यागम arg_end;
}

/*
 * Set a register, part of ex_let_one().
 */
    स्थिर char_u *
ex_let_register(
    char_u	*arg,
    typval_T	*tv,
    पूर्णाङ्क		flags,
    char_u	*endchars,
    char_u	*op)
{
    char_u	*arg_end = रिक्त;

    यदि ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					 && (flags & ASSIGN_FOR_LOOP) == 0)
    {
	emsg(_(e_cannot_lock_register));
	प्रत्यागम रिक्त;
    }
    ++arg;
    यदि (op != रिक्त && vim_strchr((char_u *)"+-*/%", *op) != रिक्त)
	semsg(_(e_wrong_variable_type_for_str_equal), op);
    अन्यथा यदि (endchars != रिक्त
			  && vim_strchr(endchars, *skipwhite(arg + 1)) == रिक्त)
	emsg(_(e_unexpected_characters_in_let));
    अन्यथा
    {
	char_u	*ptofree = रिक्त;
	char_u	*p;

	p = tv_get_string_chk(tv);
	यदि (p != रिक्त && op != रिक्त && *op == '.')
	{
	    char_u  *s = get_reg_contents(*arg == '@'
						  ? '"' : *arg, GREG_अभिव्यक्ती_SRC);

	    यदि (s != रिक्त)
	    {
		p = ptofree = concat_str(s, p);
		vim_free(s);
	    }
	}
	यदि (p != रिक्त)
	{
	    write_reg_contents(*arg == '@' ? '"' : *arg, p, -1, FALSE);
	    arg_end = arg + 1;
	}
	vim_free(ptofree);
    }
    प्रत्यागम arg_end;
}

/*
 * Set one item of ":let var = expr" or ":let [v1, v2] = list" to its value.
 * Returns a pointer to the char just after the var name.
 * Returns NULL if there is an error.
 */
    स्थिर char_u *
ex_let_one(
    char_u	*arg,		// points to variable name
    typval_T	*tv,		// value to assign to variable
    पूर्णाङ्क		copy,		// copy value from "tv"
    पूर्णाङ्क		flags,		// ASSIGN_CONST, ASSIGN_FINAL, etc.
    char_u	*endchars,	// valid chars after variable name  or NULL
    char_u	*op,		// "+", "-", "."  or NULL
    पूर्णाङ्क		var_idx)	// variable index for "let [a, b] = list"
{
    char_u	*arg_end = रिक्त;

    यदि (in_vim9script() && (flags & (ASSIGN_NO_DECL | ASSIGN_DECL)) == 0
			&& (flags & (ASSIGN_CONST | ASSIGN_FINAL)) == 0
				  && vim_strchr((char_u *)"$@&", *arg) != रिक्त)
    {
	vim9_declare_error(arg);
	प्रत्यागम रिक्त;
    }

    यदि (check_typval_is_मूल्य(tv) == FAIL)
	प्रत्यागम रिक्त;

    // In Vim9 script an environment variable and a register only take a
    // String.  "@#" also takes a buffer number.
    यदि (in_vim9script() && (*arg == '$' || *arg == '@')
	    && !(*arg == '@' && arg[1] == '#' && tv->v_type == VAR_संख्या
					       && (op == रिक्त || *op == '=')))
    {
	where_T	where = WHERE_INIT;

	यदि (check_typval_type(&t_string, tv, where) == FAIL)
	    प्रत्यागम रिक्त;
    }

    यदि (*arg == '$')
    {
	// ":let $VAR = expr": Set environment variable.
	प्रत्यागम ex_let_env(arg, tv, flags, endchars, op);
    }
    अन्यथा यदि (*arg == '&')
    {
	// ":let &option = expr": Set option value.
	// ":let &l:option = expr": Set local option value.
	// ":let &g:option = expr": Set global option value.
	// ":for &ts in range(8)": Set option value for for loop
	प्रत्यागम ex_let_option(arg, tv, flags, endchars, op);
    }
    अन्यथा यदि (*arg == '@')
    {
	// ":let @r = expr": Set register contents.
	प्रत्यागम ex_let_register(arg, tv, flags, endchars, op);
    }
    अन्यथा यदि (eval_isnamec1(*arg) || *arg == '{')
    {
	lval_T	lv;
	char_u	*p;
	पूर्णाङ्क	lval_flags = (flags & (ASSIGN_NO_DECL | ASSIGN_DECL))
							     ? GLV_NO_DECL : 0;
	lval_flags |= (flags & ASSIGN_FOR_LOOP) ? GLV_FOR_LOOP : 0;
	यदि (op != रिक्त && *op != '=')
	    lval_flags |= GLV_ASSIGN_WITH_OP;

	// ":let var = expr": Set internal variable.
	// ":let var: type = expr": Set internal variable with type.
	// ":let {expr} = expr": Idem, name made with curly braces
	p = get_lval(arg, tv, &lv, FALSE, FALSE, lval_flags, FNE_CHECK_START);
	यदि (p != रिक्त && lv.ll_name != रिक्त)
	{
	    यदि (endchars != रिक्त && vim_strchr(endchars,
					   *skipwhite(lv.ll_name_end)) == रिक्त)
	    {
		emsg(_(e_unexpected_characters_in_let));
	    }
	    अन्यथा
	    {
		set_var_lval(&lv, p, tv, copy, flags, op, var_idx);
		arg_end = lv.ll_name_end;
	    }
	}
	clear_lval(&lv);
    }
    अन्यथा
	semsg(_(e_invalid_argument_str), arg);

    प्रत्यागम arg_end;
}

/*
 * ":unlet[!] var1 ... " command.
 */
    शून्य
ex_unlet(exarg_T *eap)
{
    ex_unletlock(eap, eap->arg, 0, eap->forceit ? GLV_QUIET : 0, do_unlet_var, रिक्त);
}

/*
 * ":lockvar" and ":unlockvar" commands
 */
    शून्य
ex_lockvar(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    पूर्णाङ्क		deep = 2;

    यदि (eap->forceit)
	deep = -1;
    अन्यथा यदि (vim_isdigit(*arg))
    {
	deep = getdigits(&arg);
	arg = skipwhite(arg);
    }

    ex_unletlock(eap, arg, deep, 0, do_lock_var, रिक्त);
}

/*
 * ":unlet", ":lockvar" and ":unlockvar" are quite similar.
 * Also used for Vim9 script.  "callback" is invoked as:
 *	callback(&lv, name_end, eap, deep, cookie)
 */
    शून्य
ex_unletlock(
    exarg_T	*eap,
    char_u	*argstart,
    पूर्णाङ्क		deep,
    पूर्णाङ्क		glv_flags,
    पूर्णाङ्क		(*callback)(lval_T *, char_u *, exarg_T *, पूर्णाङ्क, शून्य *),
    शून्य	*cookie)
{
    char_u	*arg = argstart;
    char_u	*name_end;
    पूर्णाङ्क		error = FALSE;
    lval_T	lv;

    कुरु
    {
	यदि (*arg == '$')
	{
	    lv.ll_name = arg;
	    lv.ll_tv = रिक्त;
	    ++arg;
	    यदि (get_env_len(&arg) == 0)
	    {
		semsg(_(e_invalid_argument_str), arg - 1);
		प्रत्यागम;
	    }
	    यदि (!error && !eap->skip
			      && callback(&lv, arg, eap, deep, cookie) == FAIL)
		error = TRUE;
	    name_end = arg;
	}
	अन्यथा
	{
	    // Parse the name and find the end.
	    name_end = get_lval(arg, रिक्त, &lv, TRUE, eap->skip || error,
				     glv_flags | GLV_NO_DECL, FNE_CHECK_START);
	    यदि (lv.ll_name == रिक्त)
		error = TRUE;	    // error but continue parsing
	    यदि (name_end == रिक्त || (!VIM_ISWHITE(*name_end)
						    && !ends_excmd(*name_end)))
	    {
		यदि (name_end != रिक्त)
		{
		    emsg_severe = TRUE;
		    semsg(_(e_trailing_characters_str), name_end);
		}
		यदि (!(eap->skip || error))
		    clear_lval(&lv);
		भङ्ग;
	    }

	    यदि (!error && !eap->skip
			 && callback(&lv, name_end, eap, deep, cookie) == FAIL)
		error = TRUE;

	    यदि (!eap->skip)
		clear_lval(&lv);
	}

	arg = skipwhite(name_end);
    } यावत् (!ends_excmd2(name_end, arg));

    set_nextcmd(eap, arg);
}

    स्थिर पूर्णाङ्क
do_unlet_var(
    lval_T	*lp,
    char_u	*name_end,
    exarg_T	*eap,
    पूर्णाङ्क		deep UNUSED,
    शून्य	*cookie UNUSED)
{
    पूर्णाङ्क		forceit = eap->forceit;
    पूर्णाङ्क		ret = OK;
    पूर्णाङ्क		cc;

    यदि (lp->ll_tv == रिक्त)
    {
	cc = *name_end;
	*name_end = NUL;

	// Environment variable, normal name or expanded name.
	यदि (*lp->ll_name == '$')
	    vim_unsetenv_ext(lp->ll_name + 1);
	अन्यथा यदि (do_unlet(lp->ll_name, forceit) == FAIL)
	    ret = FAIL;
	*name_end = cc;
    }
    अन्यथा यदि ((lp->ll_list != रिक्त
		 && मूल्य_check_lock(lp->ll_list->lv_lock, lp->ll_name, FALSE))
	    || (lp->ll_dict != रिक्त
		&& मूल्य_check_lock(lp->ll_dict->dv_lock, lp->ll_name, FALSE)))
	प्रत्यागम FAIL;
    अन्यथा यदि (lp->ll_range)
	list_unlet_range(lp->ll_list, lp->ll_li, lp->ll_n1,
						    !lp->ll_empty2, lp->ll_n2);
    अन्यथा यदि (lp->ll_list != रिक्त)
	// unlet a List item.
	listitem_remove(lp->ll_list, lp->ll_li);
    अन्यथा यदि (lp->ll_dict != रिक्त)
	// unlet a Dictionary item.
	dictitem_remove(lp->ll_dict, lp->ll_di, "unlet");
    अन्यथा
    {
	semsg(_(e_cannot_unlet_imported_item_str), lp->ll_name);
	प्रत्यागम FAIL;
    }

    प्रत्यागम ret;
}

/*
 * Unlet one item or a range of items from a list.
 * Return OK or FAIL.
 */
    शून्य
list_unlet_range(
	list_T	    *l,
	listitem_T  *li_first,
	long	    n1_arg,
	पूर्णाङ्क	    has_n2,
	long	    n2)
{
    listitem_T  *li = li_first;
    पूर्णाङ्क		n1 = n1_arg;

    // Delete a range of List items.
    li = li_first;
    n1 = n1_arg;
    यावत् (li != रिक्त && (!has_n2 || n2 >= n1))
    {
	listitem_T *next = li->li_next;

	listitem_remove(l, li);
	li = next;
	++n1;
    }
}
/*
 * "unlet" a variable.  Return OK if it existed, FAIL if not.
 * When "forceit" is TRUE don't complain if the variable doesn't exist.
 */
    पूर्णाङ्क
do_unlet(char_u *name, पूर्णाङ्क forceit)
{
    hashtab_T	*ht;
    hashitem_T	*hi;
    char_u	*varname = रिक्त;  // init to shut up gcc
    dict_T	*d;
    dictitem_T	*di;

    // can't :unlet a script variable in Vim9 script
    यदि (in_vim9script() && check_vim9_unlet(name) == FAIL)
	प्रत्यागम FAIL;

    ht = find_var_ht(name, &varname);

    // can't :unlet a script variable in Vim9 script from a function
    यदि (ht == get_script_local_ht()
	    && SCRIPT_ID_VALID(current_sctx.sc_sid)
	    && SCRIPT_ITEM(current_sctx.sc_sid)->sn_version
							 == SCRIPT_VERSION_VIM9
	    && check_vim9_unlet(name) == FAIL)
	प्रत्यागम FAIL;

    यदि (ht != रिक्त && *varname != NUL)
    {
	d = get_current_funccal_dict(ht);
	यदि (d == रिक्त)
	{
	    यदि (ht == &globvarht)
		d = &globvardict;
	    अन्यथा यदि (ht == &compat_hashtab)
		d = &vimvardict;
	    अन्यथा
	    {
		di = find_var_in_ht(ht, *name, (char_u *)"", FALSE);
		d = di == रिक्त ? रिक्त : di->di_tv.vval.v_dict;
	    }
	    यदि (d == रिक्त)
	    {
		internal_error("do_unlet()");
		प्रत्यागम FAIL;
	    }
	}
	hi = hash_find(ht, varname);
	यदि (HASHITEM_EMPTY(hi))
	    hi = find_hi_in_scoped_ht(name, &ht);
	यदि (hi != रिक्त && !HASHITEM_EMPTY(hi))
	{
	    di = HI2DI(hi);
	    यदि (var_check_fixed(di->di_flags, name, FALSE)
		    || var_check_ro(di->di_flags, name, FALSE)
		    || मूल्य_check_lock(d->dv_lock, name, FALSE)
		    || check_hashtab_frozen(ht, "unlet"))
		प्रत्यागम FAIL;

	    delete_var(ht, hi);
	    प्रत्यागम OK;
	}
    }
    यदि (forceit)
	प्रत्यागम OK;
    semsg(_(e_no_such_variable_str), name);
    प्रत्यागम FAIL;
}

    स्थिर शून्य
report_lockvar_member(वर्ण *संदेश, lval_T *lp)
{
    पूर्णाङ्क did_alloc = FALSE;
    char_u *vname = (char_u *)"";
    char_u *class_name = lp->ll_class != रिक्त
				    ? lp->ll_class->class_name.string : (char_u *)"";
    यदि (lp->ll_name != रिक्त)
    {
	यदि (lp->ll_name_end == रिक्त)
	    vname = lp->ll_name;
	अन्यथा
	{
	    vname = vim_strnsave(lp->ll_name, lp->ll_name_end - lp->ll_name);
	    यदि (vname == रिक्त)
		प्रत्यागम;
	    did_alloc = TRUE;
	}
    }
    semsg(_(संदेश), vname, class_name);
    यदि (did_alloc)
	vim_free(vname);
}

/*
 * Lock or unlock variable indicated by "lp".
 * "deep" is the levels to go (-1 for unlimited);
 * "lock" is TRUE for ":lockvar", FALSE for ":unlockvar".
 */
    स्थिर पूर्णाङ्क
do_lock_var(
    lval_T	*lp,
    char_u	*name_end,
    exarg_T	*eap,
    पूर्णाङ्क		deep,
    शून्य	*cookie UNUSED)
{
    पूर्णाङ्क		lock = eap->cmdidx == CMD_lockvar;
    पूर्णाङ्क		ret = OK;
    पूर्णाङ्क		cc;
    dictitem_T	*di;

#ifdef LOG_LOCKVAR
    ch_log(रिक्त, "LKVAR: do_lock_var(): name %s, is_root %d", lp->ll_name, lp->ll_is_मूळ);
#endif

    यदि (lp->ll_tv == रिक्त)
    {
	cc = *name_end;
	*name_end = NUL;
	यदि (*lp->ll_name == '$')
	{
	    semsg(_(e_cannot_lock_or_unlock_variable_str), lp->ll_name);
	    ret = FAIL;
	}
	अन्यथा
	{
	    // Normal name or expanded name.
	    di = find_var(lp->ll_name, रिक्त, TRUE);
	    यदि (di == रिक्त)
	    {
		यदि (in_vim9script())
		    semsg(_(e_cannot_find_variable_to_unlock_str),
								  lp->ll_name);
		ret = FAIL;
	    }
	    अन्यथा
	    {
		यदि ((di->di_flags & DI_FLAGS_FIX)
			    && di->di_tv.v_type != VAR_DICT
			    && di->di_tv.v_type != VAR_LIST)
		{
		    // For historic reasons this error is not given for a list
		    // or dict.  E.g., the b: dict could be locked/unlocked.
		    semsg(_(e_cannot_lock_or_unlock_variable_str), lp->ll_name);
		    ret = FAIL;
		}
		अन्यथा
		{
		    यदि (in_vim9script())
		    {
			svar_T  *sv = find_typval_in_script(&di->di_tv,
								     0, FALSE);

			यदि (sv != रिक्त && sv->sv_const != 0)
			{
			    semsg(_(e_cannot_change_readonly_variable_str),
								  lp->ll_name);
			    ret = FAIL;
			}
		    }

		    यदि (ret == OK)
		    {
			यदि (lock)
			    di->di_flags |= DI_FLAGS_LOCK;
			अन्यथा
			    di->di_flags &= ~DI_FLAGS_LOCK;
			यदि (deep != 0)
			    item_lock(&di->di_tv, deep, lock, FALSE);
		    }
		}
	    }
	}
	*name_end = cc;
    }
    अन्यथा यदि (deep == 0 && lp->ll_object == रिक्त && lp->ll_class == रिक्त)
    {
	// nothing to do
    }
    अन्यथा यदि (lp->ll_is_मूळ)
	// (un)lock the item.
	item_lock(lp->ll_tv, deep, lock, FALSE);
    अन्यथा यदि (lp->ll_range)
    {
	listitem_T    *li = lp->ll_li;

	// (un)lock a range of List items.
	यावत् (li != रिक्त && (lp->ll_empty2 || lp->ll_n2 >= lp->ll_n1))
	{
	    item_lock(&li->li_tv, deep, lock, FALSE);
	    li = li->li_next;
	    ++lp->ll_n1;
	}
    }
    अन्यथा यदि (lp->ll_list != रिक्त)
	// (un)lock a List item.
	item_lock(&lp->ll_li->li_tv, deep, lock, FALSE);
    अन्यथा यदि (lp->ll_object != रिक्त)  // This check must be before ll_class.
    {
	// (un)lock an object variable.
	report_lockvar_member(e_cannot_lock_object_variable_str, lp);
	ret = FAIL;
    }
    अन्यथा यदि (lp->ll_class != रिक्त)
    {
	// (un)lock a class variable.
	report_lockvar_member(e_cannot_lock_class_variable_str, lp);
	ret = FAIL;
    }
    अन्यथा
    {
	// (un)lock a Dictionary item.
	यदि (lp->ll_di == रिक्त)
	{
	    emsg(_(e_dictionary_required));
	    ret = FAIL;
	}
	अन्यथा
	    item_lock(&lp->ll_di->di_tv, deep, lock, FALSE);
    }

    प्रत्यागम ret;
}

/*
 * Lock or unlock an item.  "deep" is nr of levels to go.
 * When "check_refcount" is TRUE do not lock a list or dict with a reference
 * count larger than 1.
 */
    शून्य
item_lock(typval_T *tv, पूर्णाङ्क deep, पूर्णाङ्क lock, पूर्णाङ्क check_refcount)
{
    स्थिर पूर्णाङ्क	recurse = 0;
    list_T	*l;
    listitem_T	*li;
    dict_T	*d;
    blob_T	*b;
    hashitem_T	*hi;
    पूर्णाङ्क		todo;

#ifdef LOG_LOCKVAR
    ch_log(रिक्त, "LKVAR: item_lock(): type %s", vartype_name(tv->v_type));
#endif

    यदि (recurse >= DICT_MAXNEST)
    {
	emsg(_(e_variable_nested_too_deep_for_unlock));
	प्रत्यागम;
    }
    यदि (deep == 0)
	प्रत्यागम;
    ++recurse;

    // lock/unlock the item itself
    यदि (lock)
	tv->v_lock |= VAR_LOCKED;
    अन्यथा
	tv->v_lock &= ~VAR_LOCKED;

    विकल्प (tv->v_type)
    {
	पक्ष VAR_UNKNOWN:
	पक्ष VAR_ANY:
	पक्ष VAR_VOID:
	पक्ष VAR_संख्या:
	पक्ष VAR_BOOL:
	पक्ष VAR_STRING:
	पक्ष VAR_FUNC:
	पक्ष VAR_PARTIAL:
	पक्ष VAR_FLOAT:
	पक्ष VAR_SPECIAL:
	पक्ष VAR_JOB:
	पक्ष VAR_CHANNEL:
	पक्ष VAR_INSTR:
	पक्ष VAR_CLASS:
	पक्ष VAR_OBJECT:
	पक्ष VAR_TYPEALIAS:
	    भङ्ग;

	पक्ष VAR_BLOB:
	    यदि ((b = tv->vval.v_blob) != रिक्त
				    && !(check_refcount && b->bv_refcount > 1))
	    {
		यदि (lock)
		    b->bv_lock |= VAR_LOCKED;
		अन्यथा
		    b->bv_lock &= ~VAR_LOCKED;
	    }
	    भङ्ग;
	पक्ष VAR_LIST:
	    यदि ((l = tv->vval.v_list) != रिक्त
				    && !(check_refcount && l->lv_refcount > 1))
	    {
		यदि (lock)
		    l->lv_lock |= VAR_LOCKED;
		अन्यथा
		    l->lv_lock &= ~VAR_LOCKED;
		यदि (deep < 0 || deep > 1)
		{
		    यदि (l->lv_first == &range_list_item)
			l->lv_lock |= VAR_वस्तुसंच_LOCKED;
		    अन्यथा
		    {
			// recursive: lock/unlock the items the List contains
			CHECK_LIST_MATERIALIZE(l);
			FOR_ALL_LIST_वस्तुसंच(l, li) item_lock(&li->li_tv,
					       deep - 1, lock, check_refcount);
		    }
		}
	    }
	    भङ्ग;
	पक्ष VAR_TUPLE:
	    tuple_lock(tv->vval.v_tuple, deep, lock, check_refcount);
	    भङ्ग;
	पक्ष VAR_DICT:
	    यदि ((d = tv->vval.v_dict) != रिक्त
				    && !(check_refcount && d->dv_refcount > 1))
	    {
		यदि (lock)
		    d->dv_lock |= VAR_LOCKED;
		अन्यथा
		    d->dv_lock &= ~VAR_LOCKED;
		यदि (deep < 0 || deep > 1)
		{
		    // recursive: lock/unlock the items the List contains
		    todo = (पूर्णाङ्क)d->dv_hashtab.ht_used;
		    FOR_ALL_HASHTAB_वस्तुसंच(&d->dv_hashtab, hi, todo)
		    {
			यदि (!HASHITEM_EMPTY(hi))
			{
			    --todo;
			    item_lock(&HI2DI(hi)->di_tv, deep - 1, lock,
							       check_refcount);
			}
		    }
		}
	    }
    }
    --recurse;
}

#यदि defined(FEAT_MENU) && defined(FEAT_MULTI_LANG)
/*
 * Delete all "menutrans_" variables.
 */
    शून्य
del_menutrans_vars(शून्य)
{
    hashitem_T	*hi;
    पूर्णाङ्क		todo;

    hash_lock(&globvarht);
    todo = (पूर्णाङ्क)globvarht.ht_used;
    प्रति (hi = globvarht.ht_array; todo > 0 && !got_int; ++hi)
    {
	यदि (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    यदि (STRNCMP(HI2DI(hi)->di_किल्ली, "menutrans_", 10) == 0)
		delete_var(&globvarht, hi);
	}
    }
    hash_unlock(&globvarht);
}
#endif

/*
 * Local string buffer for the next two functions to store a variable name
 * with its prefix. Allocated in cat_prefix_varname(), freed later in
 * get_user_var_name().
 */

स्थिर char_u	*varnamebuf = रिक्त;
स्थिर पूर्णाङ्क	varnamebuflen = 0;

/*
 * Function to concatenate a prefix and a variable name.
 */
    char_u *
cat_prefix_varname(पूर्णाङ्क prefix, char_u *name)
{
    पूर्णाङ्क		len;

    len = (पूर्णाङ्क)STRLEN(name) + 3;
    यदि (len > varnamebuflen)
    {
	vim_free(varnamebuf);
	len += 10;			// some additional space
	varnamebuf = alloc(len);
	यदि (varnamebuf == रिक्त)
	{
	    varnamebuflen = 0;
	    प्रत्यागम रिक्त;
	}
	varnamebuflen = len;
    }
    *varnamebuf = prefix;
    varnamebuf[1] = ':';
    STRCPY(varnamebuf + 2, name);
    प्रत्यागम varnamebuf;
}

/*
 * Function given to ExpandGeneric() to obtain the list of user defined
 * (global/buffer/window/built-in) variable names.
 */
    char_u *
get_user_var_name(expand_T *xp, पूर्णाङ्क idx)
{
    स्थिर long_u	gdone;
    स्थिर long_u	bdone;
    स्थिर long_u	wdone;
    स्थिर long_u	tdone;
    स्थिर पूर्णाङ्क		vidx;
    स्थिर hashitem_T	*hi;
    hashtab_T		*ht;

    यदि (idx == 0)
    {
	gdone = bdone = wdone = vidx = 0;
	tdone = 0;
    }

    // Global variables
    यदि (gdone < globvarht.ht_used)
    {
	यदि (gdone++ == 0)
	    hi = globvarht.ht_array;
	अन्यथा
	    ++hi;
	यावत् (HASHITEM_EMPTY(hi))
	    ++hi;
	यदि (STRNCMP("g:", xp->xp_pattern, 2) == 0)
	    प्रत्यागम cat_prefix_varname('g', hi->hi_किल्ली);
	प्रत्यागम hi->hi_किल्ली;
    }

    // b: variables
    ht = &prevwin_curwin()->w_buffer->b_vars->dv_hashtab;
    यदि (bdone < ht->ht_used)
    {
	यदि (bdone++ == 0)
	    hi = ht->ht_array;
	अन्यथा
	    ++hi;
	यावत् (HASHITEM_EMPTY(hi))
	    ++hi;
	प्रत्यागम cat_prefix_varname('b', hi->hi_किल्ली);
    }

    // w: variables
    ht = &prevwin_curwin()->w_vars->dv_hashtab;
    यदि (wdone < ht->ht_used)
    {
	यदि (wdone++ == 0)
	    hi = ht->ht_array;
	अन्यथा
	    ++hi;
	यावत् (HASHITEM_EMPTY(hi))
	    ++hi;
	प्रत्यागम cat_prefix_varname('w', hi->hi_किल्ली);
    }

    // t: variables
    ht = &curtab->tp_vars->dv_hashtab;
    यदि (tdone < ht->ht_used)
    {
	यदि (tdone++ == 0)
	    hi = ht->ht_array;
	अन्यथा
	    ++hi;
	यावत् (HASHITEM_EMPTY(hi))
	    ++hi;
	प्रत्यागम cat_prefix_varname('t', hi->hi_किल्ली);
    }

    // v: variables
    यदि (vidx < VV_LEN)
	प्रत्यागम cat_prefix_varname('v', (char_u *)vimvars[vidx++].vv_name);

    VIM_CLEAR(varnamebuf);
    varnamebuflen = 0;
    प्रत्यागम रिक्त;
}

    वर्ण *
get_var_special_name(पूर्णाङ्क nr)
{
    विकल्प (nr)
    {
	पक्ष VVAL_FALSE: प्रत्यागम in_vim9script() ? "false" : "v:false";
	पक्ष VVAL_TRUE:  प्रत्यागम in_vim9script() ? "true" : "v:true";
	पक्ष VVAL_NULL:  प्रत्यागम in_vim9script() ? "null" : "v:null";
	पक्ष VVAL_NONE:  प्रत्यागम "v:none";
    }
    internal_error("get_var_special_name()");
    प्रत्यागम "42";
}

/*
 * Returns the global variable dictionary
 */
    dict_T *
get_globvar_dict(शून्य)
{
    प्रत्यागम &globvardict;
}

/*
 * Returns the global variable hash table
 */
    hashtab_T *
get_globvar_ht(शून्य)
{
    प्रत्यागम &globvarht;
}

/*
 * Returns the v: variable dictionary
 */
    dict_T *
get_vimvar_dict(शून्य)
{
    प्रत्यागम &vimvardict;
}

/*
 * Returns the index of a v:variable.  Negative if not found.
 * Returns DI_ flags in "di_flags".
 */
    पूर्णाङ्क
find_vim_var(char_u *name, पूर्णाङ्क *di_flags)
{
    dictitem_T	    *di = find_var_in_ht(&vimvarht, 0, name, TRUE);
    संरचना vimvar   *vv;

    यदि (di == रिक्त)
	प्रत्यागम -1;
    *di_flags = di->di_flags;
    vv = (संरचना vimvar *)((वर्ण *)di - offsetof(vimvar_T, vv_di));
    प्रत्यागम (पूर्णाङ्क)(vv - vimvars);
}


/*
 * Set type of v: variable to "type".
 */
    शून्य
set_vim_var_type(पूर्णाङ्क idx, vartype_T type)
{
    vimvars[idx].vv_tv_type = type;
}

/*
 * Set number v: variable to "val".
 * Note that this does not set the type, use set_vim_var_type() for that.
 */
    शून्य
set_vim_var_nr(पूर्णाङ्क idx, varnumber_T val)
{
    vimvars[idx].vv_nr = val;
}

    वर्ण *
get_vim_var_name(पूर्णाङ्क idx)
{
    प्रत्यागम vimvars[idx].vv_name;
}

/*
 * Get typval_T v: variable value.
 */
    typval_T *
get_vim_var_tv(पूर्णाङ्क idx)
{
    प्रत्यागम &vimvars[idx].vv_tv;
}

    type_T *
get_vim_var_type(पूर्णाङ्क idx, garray_T *type_list)
{
    यदि (vimvars[idx].vv_type != रिक्त)
	प्रत्यागम vimvars[idx].vv_type;
    प्रत्यागम typval2type_vimvar(&vimvars[idx].vv_tv, type_list);
}

/*
 * Add what getinfo() reports for the v: variable "name", without the "v:", to
 * "d".  Returns FAIL when there is no such variable.
 */
    पूर्णाङ्क
vim_var_info(char_u *name, dict_T *d)
{
    पूर्णाङ्क		di_flags;
    पूर्णाङ्क		idx = find_vim_var(name, &di_flags);
    garray_T	type_list;
    वर्ण	*tofree;

    यदि (idx < 0)
	प्रत्यागम FAIL;
    vim_snprintf((वर्ण *)IObuff, IOSIZE, "v:%s", vimvars[idx].vv_name);
    dict_add_string(d, "name", IObuff);
    dict_add_bool(d, "available", TRUE);
    ga_init2(&type_list, परिमाण(type_T *), 10);
    वर्ण *type = type_name(get_vim_var_type(idx, &type_list), &tofree);
    dict_add_string(d, "type", (char_u *)type);
    vim_free(tofree);
    clear_type_list(&type_list);
    dict_add_bool(d, "readonly", (vimvars[idx].vv_flags & VV_RO) != 0);
    dict_add_bool(d, "compat", (vimvars[idx].vv_flags & VV_COMPAT) != 0);
    प्रत्यागम OK;
}

/*
 * Set v: variable to "tv".  Only accepts the same type.
 * Takes over the value of "tv".
 */
    पूर्णाङ्क
set_vim_var_tv(पूर्णाङ्क idx, typval_T *tv)
{
    यदि (vimvars[idx].vv_tv_type != tv->v_type)
    {
	emsg(_(e_type_mismatch_for_v_variable));
	clear_tv(tv);
	प्रत्यागम FAIL;
    }
    // VV_RO is also checked when compiling, but let's check here as well.
    यदि (vimvars[idx].vv_flags & VV_RO)
    {
	semsg(_(e_cannot_change_readonly_variable_str), vimvars[idx].vv_name);
	प्रत्यागम FAIL;
    }
    यदि (sandbox && (vimvars[idx].vv_flags & VV_RO_SBX))
    {
	semsg(_(e_cannot_set_variable_in_sandbox_str), vimvars[idx].vv_name);
	प्रत्यागम FAIL;
    }
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_di.di_tv = *tv;
    प्रत्यागम OK;
}

/*
 * Get number v: variable value.
 */
    varnumber_T
get_vim_var_nr(पूर्णाङ्क idx)
{
    प्रत्यागम vimvars[idx].vv_nr;
}

/*
 * Get string v: variable value.  Uses a static buffer, can only be used once.
 * If the String variable has never been set, return an empty string.
 * Never returns NULL;
 */
    char_u *
get_vim_var_str(पूर्णाङ्क idx)
{
    प्रत्यागम tv_get_string(&vimvars[idx].vv_tv);
}

/*
 * Get List v: variable value.  Caller must take care of reference count when
 * needed.
 */
    list_T *
get_vim_var_list(पूर्णाङ्क idx)
{
    प्रत्यागम vimvars[idx].vv_list;
}

/*
 * Get Dict v: variable value.  Caller must take care of reference count when
 * needed.
 */
    dict_T *
get_vim_var_dict(पूर्णाङ्क idx)
{
    प्रत्यागम vimvars[idx].vv_dict;
}

/*
 * Set v:char to character "c".
 */
    शून्य
set_vim_var_char(पूर्णाङ्क c)
{
    char_u  buf[MB_MAXBYTES + 1];
    size_t  buflen;

    यदि (has_mbyte)
	buflen = (*mb_char2bytes)(c, buf);
    अन्यथा
    {
	buf[0] = c;
	buflen = 1;
    }
    buf[buflen] = NUL;

    set_vim_var_string(VV_CHAR, buf, (पूर्णाङ्क)buflen);
}

/*
 * Set v:count to "count" and v:count1 to "count1".
 * When "set_prevcount" is TRUE first set v:prevcount from v:count.
 */
    शून्य
set_vcount(
    long	मोजणी,
    long	count1,
    पूर्णाङ्क		set_prevcount)
{
    यदि (set_prevcount)
	vimvars[VV_PREVCOUNT].vv_nr = vimvars[VV_मोजणी].vv_nr;
    vimvars[VV_मोजणी].vv_nr = मोजणी;
    vimvars[VV_COUNT1].vv_nr = count1;
}

/*
 * Save variables that might be changed as a side effect.  Used when executing
 * a timer callback.
 */
    शून्य
save_vimvars(vimvars_save_T *vvsave)
{
    vvsave->vv_prevcount = vimvars[VV_PREVCOUNT].vv_nr;
    vvsave->vv_मोजणी = vimvars[VV_मोजणी].vv_nr;
    vvsave->vv_count1 = vimvars[VV_COUNT1].vv_nr;
}

/*
 * Restore variables saved by save_vimvars().
 */
    शून्य
restore_vimvars(vimvars_save_T *vvsave)
{
    vimvars[VV_PREVCOUNT].vv_nr = vvsave->vv_prevcount;
    vimvars[VV_मोजणी].vv_nr = vvsave->vv_मोजणी;
    vimvars[VV_COUNT1].vv_nr = vvsave->vv_count1;
}

/*
 * Set string v: variable to a copy of "val". If 'copy' is FALSE, then set the
 * value.
 */
    शून्य
set_vim_var_string(
    पूर्णाङ्क		idx,
    char_u	*val,
    पूर्णाङ्क		len)	    // length of "val" to use or -1 (whole string)
{
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_tv_type = VAR_STRING;
    यदि (val == रिक्त)
	vimvars[idx].vv_str = रिक्त;
    अन्यथा यदि (len == -1)
	vimvars[idx].vv_str = vim_strsave(val);
    अन्यथा
	vimvars[idx].vv_str = vim_strnsave(val, len);
}

    शून्य
set_vim_var_string_direct(
    पूर्णाङ्क		idx,
    char_u	*val)
{
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_tv_type = VAR_STRING;

    vimvars[idx].vv_str = val;
}

/*
 * Set List v: variable to "val".
 */
    शून्य
set_vim_var_list(पूर्णाङ्क idx, list_T *val)
{
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_tv_type = VAR_LIST;
    vimvars[idx].vv_list = val;
    यदि (val != रिक्त)
	++val->lv_refcount;
}

/*
 * Set Dictionary v: variable to "val".
 */
    शून्य
set_vim_var_dict(पूर्णाङ्क idx, dict_T *val)
{
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_tv_type = VAR_DICT;
    vimvars[idx].vv_dict = val;
    यदि (val == रिक्त)
	प्रत्यागम;

    ++val->dv_refcount;
    dict_set_वस्तुसंच_ro(val);
}

/*
 * Set the v:argv list.
 */
    शून्य
set_argv_var(वर्ण **argv, पूर्णाङ्क argc)
{
    list_T	*l = list_alloc();
    पूर्णाङ्क		i;

    यदि (l == रिक्त)
	getout(1);
    l->lv_lock = VAR_FIXED;
    प्रति (i = 0; i < argc; ++i)
    {
	यदि (list_append_string(l, (char_u *)argv[i], -1) == FAIL)
	    getout(1);
	l->lv_u.mat.lv_last->li_tv.v_lock = VAR_FIXED;
    }
    set_vim_var_list(VV_ARGV, l);
}

/*
 * Reset v:register, taking the 'clipboard' setting into account.
 */
    शून्य
reset_reg_var(शून्य)
{
    पूर्णाङ्क regname = 0;

    // Adjust the register according to 'clipboard', so that when
    // "unnamed" is present it becomes '*' or '+' instead of '"'.
#ifdef HAVE_CLIPMETHOD
    adjust_clip_reg(&regname);
#endif
    set_reg_var(regname);
}

/*
 * Set v:register if needed.
 */
    शून्य
set_reg_var(पूर्णाङ्क c)
{
    char_u  regname[2];

    यदि (c == 0 || c == ' ')
	regname[0] = '"';
    अन्यथा
	regname[0] = c;
    regname[1] = NUL;
    // Avoid free/alloc when the value is already right.
    यदि (vimvars[VV_REG].vv_str == रिक्त || vimvars[VV_REG].vv_str[0] != c)
	set_vim_var_string(VV_REG, regname, 1);
}

/*
 * Get or set v:exception.  If "oldval" == NULL, return the current value.
 * Otherwise, restore the value to "oldval" and return NULL.
 * Must always be called in pairs to save and restore v:exception!  Does not
 * take care of memory allocations.
 */
    char_u *
v_exception(char_u *oldval)
{
    यदि (oldval == रिक्त)
	प्रत्यागम vimvars[VV_EXCEPTION].vv_str;

    vimvars[VV_EXCEPTION].vv_str = oldval;
    प्रत्यागम रिक्त;
}

/*
 * Get or set v:throwpoint.  If "oldval" == NULL, return the current value.
 * Otherwise, restore the value to "oldval" and return NULL.
 * Must always be called in pairs to save and restore v:throwpoint!  Does not
 * take care of memory allocations.
 */
    char_u *
v_throwpoint(char_u *oldval)
{
    यदि (oldval == रिक्त)
	प्रत्यागम vimvars[VV_THROWPOINT].vv_str;

    vimvars[VV_THROWPOINT].vv_str = oldval;
    प्रत्यागम रिक्त;
}

/*
 * Set v:cmdarg.
 * If "eap" != NULL, use "eap" to generate the value and return the old value.
 * If "oldarg" != NULL, restore the value to "oldarg" and return NULL.
 * Must always be called in pairs!
 */
    char_u *
set_cmdarg(exarg_T *eap, char_u *oldarg)
{
    char_u	*oldval;
    char_u	*newval;
    unsigned	len;

    oldval = vimvars[VV_CMDARG].vv_str;
    यदि (eap == रिक्त)
    {
	vim_free(oldval);
	vimvars[VV_CMDARG].vv_str = oldarg;
	प्रत्यागम रिक्त;
    }

    यदि (eap->force_bin == FORCE_BIN)
	len = 6;
    अन्यथा यदि (eap->force_bin == FORCE_NOBIN)
	len = 8;
    अन्यथा
	len = 0;

    यदि (eap->read_edit)
	len += 7;

    यदि (eap->force_ff != 0)
	len += 10; // " ++ff=unix"
    यदि (eap->force_enc != 0)
	len += (unsigned)STRLEN(eap->cmd + eap->force_enc) + 7;
    यदि (eap->bad_char != 0)
	len += 7 + 4;  // " ++bad=" + "keep" or "drop"

    newval = alloc(len + 1);
    यदि (newval == रिक्त)
	प्रत्यागम रिक्त;

    यदि (eap->force_bin == FORCE_BIN)
	sprintf((वर्ण *)newval, " ++bin");
    अन्यथा यदि (eap->force_bin == FORCE_NOBIN)
	sprintf((वर्ण *)newval, " ++nobin");
    अन्यथा
	*newval = NUL;

    यदि (eap->read_edit)
	STRCAT(newval, " ++edit");

    यदि (eap->force_ff != 0)
	sprintf((वर्ण *)newval + STRLEN(newval), " ++ff=%s",
						eap->force_ff == 'u' ? "unix"
						: eap->force_ff == 'd' ? "dos"
						: "mac");
    यदि (eap->force_enc != 0)
	sprintf((वर्ण *)newval + STRLEN(newval), " ++enc=%s",
					       eap->cmd + eap->force_enc);
    यदि (eap->bad_char == BAD_KEEP)
	STRCPY(newval + STRLEN(newval), " ++bad=keep");
    अन्यथा यदि (eap->bad_char == BAD_DROP)
	STRCPY(newval + STRLEN(newval), " ++bad=drop");
    अन्यथा यदि (eap->bad_char != 0)
	sprintf((वर्ण *)newval + STRLEN(newval), " ++bad=%c", eap->bad_char);
    vimvars[VV_CMDARG].vv_str = newval;
    प्रत्यागम oldval;
}

/*
 * Get the value of internal variable "name".
 * If "flags" has EVAL_VAR_IMPORT may return a VAR_ANY with v_number set to the
 * imported script ID.
 * Return OK or FAIL.  If OK is returned "rettv" must be cleared.
 */
    पूर्णाङ्क
eval_variable(
    char_u	*name,
    पूर्णाङ्क		len,		// length of "name" or zero
    scid_T	sid,		// script ID for imported item or zero
    typval_T	*rettv,		// NULL when only checking existence
    dictitem_T	**dip,		// non-NULL when typval's dict item is needed
    पूर्णाङ्क		flags)		// EVAL_VAR_ flags
{
    पूर्णाङ्क		ret = OK;
    typval_T	*tv = रिक्त;
    पूर्णाङ्क		found = FALSE;
    hashtab_T	*ht = रिक्त;
    पूर्णाङ्क		cc = 0;
    type_T	*type = रिक्त;

    यदि (len > 0)
    {
	// truncate the name, so that we can use strcmp()
	cc = name[len];
	name[len] = NUL;
    }

    // Check for local variable when debugging.
    यदि ((sid == 0) && (tv = lookup_debug_var(name)) == रिक्त)
    {
	// Check for user-defined variables.
	dictitem_T	*v = find_var(name, &ht, flags & EVAL_VAR_NOAUTOLOAD);

	यदि (v != रिक्त)
	{
	    tv = &v->di_tv;
	    यदि (dip != रिक्त)
		*dip = v;
	}
	अन्यथा
	    ht = रिक्त;
    }

    यदि (tv == रिक्त && (in_vim9script() || STRNCMP(name, "s:", 2) == 0))
    {
	imported_T  *import = रिक्त;
	char_u	    *p = STRNCMP(name, "s:", 2) == 0 ? name + 2 : name;

	यदि (sid == 0)
	    import = find_imported(p, 0, TRUE);

	// imported variable from another script
	यदि (import != रिक्त || sid != 0)
	{
	    यदि ((flags & EVAL_VAR_IMPORT) == 0)
	    {
		यदि (SCRIPT_ID_VALID(sid))
		{
		    ht = &SCRIPT_VARS(sid);
		    यदि (ht != रिक्त)
		    {
			dictitem_T *v = find_var_in_ht(ht, 0, name,
						  flags & EVAL_VAR_NOAUTOLOAD);

			यदि (v == रिक्त)
			    v = find_var_autoload_prefix(name, sid, रिक्त, रिक्त);

			यदि (v != रिक्त)
			{
			    tv = &v->di_tv;
			    यदि (dip != रिक्त)
				*dip = v;
			}
			अन्यथा
			    ht = रिक्त;
		    }
		}
		अन्यथा
		{
		    यदि (flags & EVAL_VAR_VERBOSE)
			semsg(_(e_expected_dot_after_name_str), name);
		    ret = FAIL;
		}
	    }
	    अन्यथा
	    {
		यदि (rettv != रिक्त)
		{
		    // special value that is used in handle_subscript()
		    rettv->v_type = VAR_ANY;
		    rettv->vval.v_संख्या = sid != 0 ? sid : import->imp_sid;
		}
		found = TRUE;
	    }
	}
	अन्यथा यदि (in_vim9script() && (flags & EVAL_VAR_NO_FUNC) == 0)
	{
	    पूर्णाङ्क	    has_g_prefix = STRNCMP(name, "g:", 2) == 0;
	    ufunc_T *ufunc = find_func(name, FALSE);

	    यदि (ufunc != रिक्त && cc == '<')
	    {
		// handle generic function
		char_u	*argp = name + len;
		name[len] = cc;
		ufunc = eval_generic_func(ufunc, name, &argp);
		name[len] = NUL;
		यदि (ufunc == रिक्त)
		{
		    ret = FAIL;
		    गच्छ done;
		}
	    }

	    // In Vim9 script we can get a function reference by using the
	    // function name.  For a global non-autoload function "g:" is
	    // required.
	    यदि (ufunc != रिक्त && (has_g_prefix
					    || !func_requires_g_prefix(ufunc)))
	    {
		found = TRUE;
		यदि (rettv != रिक्त)
		{
		    rettv->v_type = VAR_FUNC;
		    यदि (has_g_prefix)
		    {
			// Keep the "g:", otherwise script-local may be
			// assumed.
			यदि (cc != '<')
			    rettv->vval.v_string = vim_strsave(name);
			अन्यथा
			{
			    // append the generic function arguments
			    char_u	*argp = name + len;
			    name[len] = cc;
			    rettv->vval.v_string =
				append_generic_func_type_args(name, len,
									&argp);
			    name[len] = NUL;
			}
		    }
		    अन्यथा
			rettv->vval.v_string = vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
		    यदि (rettv->vval.v_string != रिक्त)
			func_ref(ufunc->uf_name);
		}
	    }
	}
    }

    यदि (!found)
    {
	यदि (tv == रिक्त)
	{
	    यदि (rettv != रिक्त && (flags & EVAL_VAR_VERBOSE))
		semsg(_(e_undefined_variable_str), name);
	    ret = FAIL;
	}
	अन्यथा यदि (rettv != रिक्त)
	{
	    svar_T  *sv = रिक्त;
	    पूर्णाङ्क	    was_assigned = FALSE;

	    यदि (ht != रिक्त && ht == get_script_local_ht()
		    && tv != &SCRIPT_SV(current_sctx.sc_sid)->sv_var.di_tv)
	    {
		sv = find_typval_in_script(tv, 0, TRUE);
		यदि (sv != रिक्त)
		{
		    type = sv->sv_type;
		    was_assigned = sv->sv_flags & SVFLAG_ASSIGNED;
		}
	    }

	    यदि ((tv->v_type == VAR_TYPEALIAS || tv->v_type == VAR_CLASS)
		    && sid != 0)
	    {
		// type alias or class imported from another script.  Check
		// whether it is exported from the other script.
		sv = find_typval_in_script(tv, sid, TRUE);
		यदि (sv == रिक्त)
		{
		    ret = FAIL;
		    गच्छ done;
		}
		यदि ((sv->sv_flags & SVFLAG_EXPORTED) == 0)
		{
		    semsg(_(e_item_not_exported_in_script_str), name);
		    ret = FAIL;
		    गच्छ done;
		}
	    }

	    // If a list or tuple or dict variable wasn't initialized and has
	    // meaningful type, do it now.  Not for global variables, they are
	    // not declared.
	    यदि (ht != &globvarht)
	    {
		यदि (tv->v_type == VAR_DICT && tv->vval.v_dict == रिक्त
					    && ((type != रिक्त && !was_assigned)
							  || !in_vim9script()))
		{
		    tv->vval.v_dict = dict_alloc();
		    यदि (tv->vval.v_dict != रिक्त)
		    {
			++tv->vval.v_dict->dv_refcount;
			tv->vval.v_dict->dv_type = alloc_type(type);
			यदि (sv != रिक्त)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
		अन्यथा यदि (tv->v_type == VAR_LIST && tv->vval.v_list == रिक्त
					    && ((type != रिक्त && !was_assigned)
							  || !in_vim9script()))
		{
		    tv->vval.v_list = list_alloc();
		    यदि (tv->vval.v_list != रिक्त)
		    {
			++tv->vval.v_list->lv_refcount;
			tv->vval.v_list->lv_type = alloc_type(type);
			यदि (sv != रिक्त)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
		अन्यथा यदि (tv->v_type == VAR_TUPLE && tv->vval.v_tuple == रिक्त
					    && ((type != रिक्त && !was_assigned)
							  || !in_vim9script()))
		{
		    tv->vval.v_tuple = tuple_alloc();
		    यदि (tv->vval.v_tuple != रिक्त)
		    {
			++tv->vval.v_tuple->tv_refcount;
			tv->vval.v_tuple->tv_type = alloc_type(type);
			यदि (sv != रिक्त)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
		अन्यथा यदि (tv->v_type == VAR_BLOB && tv->vval.v_blob == रिक्त
					    && ((type != रिक्त && !was_assigned)
							  || !in_vim9script()))
		{
		    tv->vval.v_blob = blob_alloc();
		    यदि (tv->vval.v_blob != रिक्त)
		    {
			++tv->vval.v_blob->bv_refcount;
			यदि (sv != रिक्त)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
	    }
	    ret = copy_tv(tv, rettv);
	}
    }

done:
    यदि (len > 0)
	name[len] = cc;

    प्रत्यागम ret;
}

/*
 * Get the value of internal variable "name", also handling "import.name".
 * Return OK or FAIL.  If OK is returned "rettv" must be cleared.
 */
    पूर्णाङ्क
eval_variable_import(
    char_u	*name,
    typval_T	*rettv)
{
    char_u  *s = name;
    यावत् (ASCII_ISALNUM(*s) || *s == '_')
	++s;
    पूर्णाङ्क	    len = (पूर्णाङ्क)(s - name);

    यदि (eval_variable(name, len, 0, rettv, रिक्त, EVAL_VAR_IMPORT) == FAIL)
	प्रत्यागम FAIL;
    यदि (rettv->v_type == VAR_ANY && *s == '.')
    {
	char_u *ns = s + 1;
	s = ns;
	यावत् (ASCII_ISALNUM(*s) || *s == '_')
	    ++s;
	पूर्णाङ्क sid = rettv->vval.v_संख्या;
	प्रत्यागम eval_variable(ns, (पूर्णाङ्क)(s - ns), sid, rettv, रिक्त, 0);
    }
    प्रत्यागम OK;
}


/*
 * Check if variable "name[len]" is a local variable or an argument.
 * If so, "*eval_lavars_used" is set to TRUE.
 */
    शून्य
check_vars(char_u *name, पूर्णाङ्क len)
{
    पूर्णाङ्क		cc;
    char_u	*varname;
    hashtab_T	*ht;

    यदि (eval_lavars_used == रिक्त)
	प्रत्यागम;

    // truncate the name, so that we can use strcmp()
    cc = name[len];
    name[len] = NUL;

    ht = find_var_ht(name, &varname);
    यदि (ht == get_funccal_local_ht() || ht == get_funccal_args_ht())
    {
	यदि (find_var(name, रिक्त, TRUE) != रिक्त)
	    *eval_lavars_used = TRUE;
    }

    name[len] = cc;
}

/*
 * Find variable "name" in the list of variables.
 * Return a pointer to it if found, NULL if not found.
 * Careful: "a:0" variables don't have a name.
 * When "htp" is not NULL  set "htp" to the hashtab_T used.
 */
    dictitem_T *
find_var(char_u *name, hashtab_T **htp, पूर्णाङ्क no_autoload)
{
    char_u	*varname;
    hashtab_T	*ht;
    dictitem_T	*ret = रिक्त;

    ht = find_var_ht(name, &varname);
    यदि (htp != रिक्त)
	*htp = ht;
    यदि (ht == रिक्त)
	प्रत्यागम रिक्त;
    ret = find_var_in_ht(ht, *name, varname, no_autoload);
    यदि (ret != रिक्त)
	प्रत्यागम ret;

    // Search in parent scope for lambda
    ret = find_var_in_scoped_ht(name, no_autoload);
    यदि (ret != रिक्त)
	प्रत्यागम ret;

    // in Vim9 script items without a scope can be script-local
    यदि (in_vim9script() && name[0] != NUL && name[1] != ':')
    {
	ht = get_script_local_ht();
	यदि (ht != रिक्त)
	{
	    ret = find_var_in_ht(ht, *name, varname, no_autoload);
	    यदि (ret != रिक्त)
	    {
		यदि (htp != रिक्त)
		    *htp = ht;
		प्रत्यागम ret;
	    }
	}
    }

    // and finally try
    प्रत्यागम find_var_autoload_prefix(name, 0, htp, रिक्त);
}

/*
 * Find variable "name" with sn_autoload_prefix.
 * Return a pointer to it if found, NULL if not found.
 * When "sid" > 0, use it otherwise use "current_sctx.sc_sid".
 * When "htp" is not NULL  set "htp" to the hashtab_T used.
 * When "namep" is not NULL set "namep" to the generated name, and
 * then the caller gets ownership and is responsible for freeing the name.
 */
    dictitem_T *
find_var_autoload_prefix(char_u *name, पूर्णाङ्क sid, hashtab_T **htp,
							    char_u **namep)
{
    hashtab_T	*ht;
    dictitem_T	*ret = रिक्त;
    // When using "vim9script autoload" script-local items are prefixed but can
    // be used with s:name.
    पूर्णाङ्क check_sid = sid > 0 ? sid : current_sctx.sc_sid;
    यदि (SCRIPT_ID_VALID(check_sid)
		   && (in_vim9script() || (name[0] == 's' && name[1] == ':')))
    {
	scriptitem_T *si = SCRIPT_ITEM(check_sid);

	यदि (si->sn_autoload_prefix != रिक्त)
	{
	    char_u *base_name = (name[0] == 's' && name[1] == ':')
							     ? name + 2 : name;
	    char_u *auto_name = concat_str(si->sn_autoload_prefix, base_name);

	    यदि (auto_name != रिक्त)
	    {
		पूर्णाङ्क free_auto_name = TRUE;
		ht = &globvarht;
		ret = find_var_in_ht(ht, 'g', auto_name, TRUE);
		यदि (ret != रिक्त)
		{
		    यदि (htp != रिक्त)
			*htp = ht;
		    यदि (namep != रिक्त)
		    {
			free_auto_name = FALSE;
			*namep = auto_name;
		    }
		}
		यदि (free_auto_name)
		    vim_free(auto_name);
	    }
	}
    }

    प्रत्यागम ret;
}

/*
 * Like find_var() but if the name starts with <SNR>99_ then look in the
 * referenced script (used for a funcref).
 */
    dictitem_T *
find_var_also_in_script(char_u *name, hashtab_T **htp, पूर्णाङ्क no_autoload)
{
    यदि (STRNCMP(name, "<SNR>", 5) == 0 && SAFE_isdigit(name[5]))
    {
	char_u	    *p = name + 5;
	पूर्णाङ्क	    sid = getdigits(&p);

	यदि (SCRIPT_ID_VALID(sid) && *p == '_')
	{
	    hashtab_T	*ht = &SCRIPT_VARS(sid);

	    यदि (ht != रिक्त)
	    {
		dictitem_T *di = find_var_in_ht(ht, 0, p + 1, no_autoload);

		यदि (di != रिक्त)
		{
		    यदि (htp != रिक्त)
			*htp = ht;
		    प्रत्यागम di;
		}
	    }
	}
    }

    प्रत्यागम find_var(name, htp, no_autoload);
}

/*
 * Find variable "varname" in hashtab "ht" with name "htname".
 * When "varname" is empty returns curwin/curtab/etc vars dictionary.
 * Returns NULL if not found.
 */
    dictitem_T *
find_var_in_ht(
    hashtab_T	*ht,
    पूर्णाङ्क		htname,
    char_u	*varname,
    पूर्णाङ्क		no_autoload)
{
    hashitem_T	*hi;

    यदि (*varname == NUL)
    {
	// Must be something like "s:", otherwise "ht" would be NULL.
	विकल्प (htname)
	{
	    पक्ष 's': प्रत्यागम &SCRIPT_SV(current_sctx.sc_sid)->sv_var;
	    पक्ष 'g': प्रत्यागम &globvars_var;
	    पक्ष 'v': प्रत्यागम &vimvars_var;
	    पक्ष 'b': प्रत्यागम &curbuf->b_bufvar;
	    पक्ष 'w': प्रत्यागम &curwin->w_winvar;
	    पक्ष 't': प्रत्यागम &curtab->tp_winvar;
	    पक्ष 'l': प्रत्यागम get_funccal_local_var();
	    पक्ष 'a': प्रत्यागम get_funccal_args_var();
	}
	प्रत्यागम रिक्त;
    }

    hi = hash_find(ht, varname);
    यदि (HASHITEM_EMPTY(hi))
    {
	// For global variables we may try auto-loading the script.  If it
	// worked find the variable again.  Don't auto-load a script if it was
	// loaded already, otherwise it would be loaded every time when
	// checking if a function name is a Funcref variable.
	यदि (ht == &globvarht && !no_autoload)
	{
	    // Note: script_autoload() may make "hi" invalid. It must either
	    // be obtained again or not used.
	    यदि (!script_autoload(varname, FALSE) || aborting())
		प्रत्यागम रिक्त;
	    hi = hash_find(ht, varname);
	}
	यदि (HASHITEM_EMPTY(hi))
	    प्रत्यागम रिक्त;
    }
    प्रत्यागम HI2DI(hi);
}

/*
 * Get the script-local hashtab.  NULL if not in a script context.
 */
    hashtab_T *
get_script_local_ht(शून्य)
{
    scid_T sid = current_sctx.sc_sid;

    यदि (SCRIPT_ID_VALID(sid))
	प्रत्यागम &SCRIPT_VARS(sid);
    प्रत्यागम रिक्त;
}

/*
 * Look for "name[len]" in script-local variables and functions.
 * When "cmd" is TRUE it must look like a command, a function must be followed
 * by "(" or "->".
 * Return OK when found, FAIL when not found.
 */
    पूर्णाङ्क
lookup_scriptitem(
	char_u	*name,
	size_t	len,
	पूर्णाङ्क	cmd,
	cctx_T	*dummy UNUSED)
{
    hashtab_T	*ht = get_script_local_ht();
    char_u	buffer[30];
    char_u	*p;
    पूर्णाङ्क		res;
    hashitem_T	*hi;
    पूर्णाङ्क		is_global = FALSE;
    char_u	*fname = name;

    यदि (ht == रिक्त)
	प्रत्यागम FAIL;
    यदि (len < परिमाण(buffer) - 1)
    {
	// avoid an alloc/free for short names
	vim_strncpy(buffer, name, len);
	p = buffer;
    }
    अन्यथा
    {
	p = vim_strnsave(name, len);
	यदि (p == रिक्त)
	    प्रत्यागम FAIL;
    }

    hi = hash_find(ht, p);
    res = HASHITEM_EMPTY(hi) ? FAIL : OK;

    // if not script-local, then perhaps autoload-exported
    यदि (res == FAIL && find_var_autoload_prefix(p, 0, रिक्त, रिक्त) != रिक्त)
	res = OK;

    // if not script-local or autoload, then perhaps imported
    यदि (res == FAIL && find_imported(p, 0, FALSE) != रिक्त)
	res = OK;
    यदि (p != buffer)
	vim_free(p);

    // Find a function, so that a following "->" works.
    // When used as a command require "(" or "->" to follow, "Cmd" is a user
    // command while "Cmd()" is a function call.
    यदि (res != OK)
    {
	p = skipwhite(name + len);

	यदि (!cmd || name[len] == '(' || (p[0] == '-' && p[1] == '>'))
	{
	    // Do not check for an internal function, since it might also be a
	    // valid command, such as ":split" versus "split()".
	    // Skip "g:" before a function name.
	    यदि (name[0] == 'g' && name[1] == ':')
	    {
		is_global = TRUE;
		fname = name + 2;
	    }
	    यदि (find_func(fname, is_global) != रिक्त)
		res = OK;
	}
    }

    प्रत्यागम res;
}

/*
 * Find the hashtab used for a variable name.
 * Return NULL if the name is not valid.
 * Set "varname" to the start of name without ':'.
 */
    hashtab_T *
find_var_ht(char_u *name, char_u **varname)
{
    hashitem_T	*hi;
    hashtab_T	*ht;

    यदि (name[0] == NUL)
	प्रत्यागम रिक्त;
    यदि (name[1] != ':')
    {
	// The name must not start with a colon or #.
	यदि (name[0] == ':' || name[0] == AUTOLOAD_CHAR)
	    प्रत्यागम रिक्त;
	*varname = name;

	// "version" is "v:version" in all scopes if scriptversion < 3.
	// Same for a few other variables marked with VV_COMPAT.
	यदि (in_old_script(3))
	{
	    hi = hash_find(&compat_hashtab, name);
	    यदि (!HASHITEM_EMPTY(hi))
		प्रत्यागम &compat_hashtab;
	}

	ht = get_funccal_local_ht();
	यदि (ht != रिक्त)
	    प्रत्यागम ht;				// local variable

	// In Vim9 script items at the script level are script-local, except
	// for autoload names.
	यदि (in_vim9script() && vim_strchr(name, AUTOLOAD_CHAR) == रिक्त)
	{
	    ht = get_script_local_ht();
	    यदि (ht != रिक्त)
		प्रत्यागम ht;
	}

	प्रत्यागम &globvarht;			// global variable
    }
    *varname = name + 2;
    यदि (*name == 'g')				// global variable
	प्रत्यागम &globvarht;
    // There must be no ':' or '#' in the rest of the name, unless g: is used
    यदि (vim_strchr(name + 2, ':') != रिक्त
			       || vim_strchr(name + 2, AUTOLOAD_CHAR) != रिक्त)
	प्रत्यागम रिक्त;
    यदि (*name == 'b')				// buffer variable
	प्रत्यागम &curbuf->b_vars->dv_hashtab;
    यदि (*name == 'w')				// window variable
	प्रत्यागम &curwin->w_vars->dv_hashtab;
    यदि (*name == 't')				// tab page variable
	प्रत्यागम &curtab->tp_vars->dv_hashtab;
    यदि (*name == 'v')				// v: variable
	प्रत्यागम &vimvarht;
    यदि (get_current_funccal() != रिक्त
	       && get_current_funccal()->fc_func->uf_def_status
							    == UF_NOT_COMPILED)
    {
	// a: and l: are only used in functions defined with ":function"
	यदि (*name == 'a')			// a: function argument
	    प्रत्यागम get_funccal_args_ht();
	यदि (*name == 'l')			// l: local function variable
	    प्रत्यागम get_funccal_local_ht();
    }
    यदि (*name == 's')				// script variable
    {
	ht = get_script_local_ht();
	यदि (ht != रिक्त)
	    प्रत्यागम ht;
    }
    प्रत्यागम रिक्त;
}

/*
 * Get the string value of a (global/local) variable.
 * Note: see tv_get_string() for how long the pointer remains valid.
 * Returns NULL when it doesn't exist.
 */
    char_u *
get_var_मूल्य(char_u *name)
{
    dictitem_T	*v;

    v = find_var(name, रिक्त, FALSE);
    यदि (v == रिक्त)
	प्रत्यागम रिक्त;
    प्रत्यागम tv_get_string(&v->di_tv);
}

/*
 * Allocate a new hashtab for a sourced script.  It will be used while
 * sourcing this script and when executing functions defined in the script.
 */
    शून्य
new_script_vars(scid_T id)
{
    scriptvar_T *sv;

    sv = ALLOC_CLEAR_ONE(scriptvar_T);
    यदि (sv == रिक्त)
	प्रत्यागम;
    init_var_dict(&sv->sv_dict, &sv->sv_var, VAR_SCOPE);
    SCRIPT_ITEM(id)->sn_vars = sv;
}

/*
 * Initialize dictionary "dict" as a scope and set variable "dict_var" to
 * point to it.
 */
    शून्य
init_var_dict(dict_T *dict, dictitem_T *dict_var, पूर्णाङ्क scope)
{
    hash_init(&dict->dv_hashtab);
    dict->dv_lock = 0;
    dict->dv_scope = scope;
    dict->dv_refcount = DO_NOT_FREE_CNT;
    dict->dv_copyID = 0;
    dict_var->di_tv.vval.v_dict = dict;
    dict_var->di_tv.v_type = VAR_DICT;
    dict_var->di_tv.v_lock = VAR_FIXED;
    dict_var->di_flags = DI_FLAGS_RO | DI_FLAGS_FIX;
    dict_var->di_किल्ली[0] = NUL;
}

/*
 * Unreference a dictionary initialized by init_var_dict().
 */
    शून्य
unref_var_dict(dict_T *dict)
{
    // Now the dict needs to be freed if no one else is using it, go back to
    // normal reference counting.
    dict->dv_refcount -= DO_NOT_FREE_CNT - 1;
    dict_unref(dict);
}

/*
 * Clean up a list of internal variables.
 * Frees all allocated variables and the value they contain.
 * Clears hashtab "ht", does not free it.
 */
    शून्य
vars_clear(hashtab_T *ht)
{
    vars_clear_ext(ht, TRUE);
}

/*
 * Like vars_clear(), but only free the value if "free_val" is TRUE.
 */
    शून्य
vars_clear_ext(hashtab_T *ht, पूर्णाङ्क free_val)
{
    पूर्णाङ्क		todo;
    hashitem_T	*hi;
    dictitem_T	*v;

    hash_lock(ht);
    todo = (पूर्णाङ्क)ht->ht_used;
    FOR_ALL_HASHTAB_वस्तुसंच(ht, hi, todo)
    {
	यदि (!HASHITEM_EMPTY(hi))
	{
	    --todo;

	    // Free the variable.  Don't remove it from the hashtab,
	    // ht_array might change then.  hash_clear() takes care of it
	    // later.
	    v = HI2DI(hi);
	    यदि (free_val)
		clear_tv(&v->di_tv);
	    यदि (v->di_flags & DI_FLAGS_ALLOC)
		vim_free(v);
	}
    }
    hash_clear(ht);
    hash_init(ht);
}

/*
 * Delete a variable from hashtab "ht" at item "hi".
 * Clear the variable value and free the dictitem.
 */
    शून्य
delete_var(hashtab_T *ht, hashitem_T *hi)
{
    dictitem_T	*di = HI2DI(hi);

    यदि (hash_remove(ht, hi, "delete variable") != OK)
	प्रत्यागम;

    clear_tv(&di->di_tv);
    vim_free(di);
}

/*
 * Delete the exported variables of a reloaded Vim9 autoload script.  They live
 * in the global namespace with the autoload prefix (e.g. "foo#bar") and are
 * recreated when the script body runs again.
 */
    शून्य
delete_autoload_export_vars(char_u *prefix)
{
    यदि (prefix == रिक्त)
	प्रत्यागम;
    size_t  prefixlen = STRLEN(prefix);

    hash_lock(&globvarht);
    पूर्णाङ्क	todo = (पूर्णाङ्क)globvarht.ht_used;

    प्रति (hashitem_T *hi = globvarht.ht_array; todo > 0; ++hi)
	यदि (!HASHITEM_EMPTY(hi))
	{
	    dictitem_T	*di = HI2DI(hi);

	    --todo;
	    // Keep a class or enum: existing objects still refer to it.
	    यदि (di->di_tv.v_type != VAR_CLASS
		    && STRNCMP(di->di_किल्ली, prefix, prefixlen) == 0)
		delete_var(&globvarht, hi);
	}
    hash_unlock(&globvarht);
}

/*
 * List the value of one internal variable.
 */
    स्थिर शून्य
list_one_var(dictitem_T *v, वर्ण *prefix, पूर्णाङ्क *first)
{
    char_u	*tofree;
    char_u	*s;
    char_u	numbuf[NUMBUFLEN];

    s = echo_string(&v->di_tv, &tofree, numbuf, get_copyID());
    list_one_var_a(prefix, v->di_किल्ली, v->di_tv.v_type,
					 s == रिक्त ? (char_u *)"" : s, first);
    vim_free(tofree);
}

    स्थिर शून्य
list_one_var_a(
    वर्ण	*prefix,
    char_u	*name,
    पूर्णाङ्क		type,
    char_u	*string,
    पूर्णाङ्क		*first)  // when TRUE clear rest of screen and set to FALSE
{
    // don't use msg() or msg_attr() to avoid overwriting "v:statusmsg"
    संदेश_start();
    संदेश_puts(prefix);
    यदि (name != रिक्त)	// "a:" vars don't have a name stored
	संदेश_puts((वर्ण *)name);
    संदेश_putchar(' ');
    संदेश_advance(22);
    यदि (type == VAR_संख्या)
	संदेश_putchar('#');
    अन्यथा यदि (type == VAR_FUNC || type == VAR_PARTIAL)
	संदेश_putchar('*');
    अन्यथा यदि (type == VAR_LIST)
    {
	संदेश_putchar('[');
	यदि (*string == '[')
	    ++string;
    }
    अन्यथा यदि (type == VAR_DICT)
    {
	संदेश_putchar('{');
	यदि (*string == '{')
	    ++string;
    }
    अन्यथा
	संदेश_putchar(' ');

    संदेश_outtrans(string);

    यदि (type == VAR_FUNC || type == VAR_PARTIAL)
	संदेश_puts("()");
    यदि (*first)
    {
	संदेश_clr_eos();
	*first = FALSE;
    }
}

/*
 * Addition handling for setting a v: variable.
 * Return TRUE if the variable should be set normally,
 *        FALSE if nothing else needs to be done.
 */
    पूर्णाङ्क
before_set_vvar(
    char_u	*varname,
    dictitem_T	*di,
    typval_T	*tv,
    पूर्णाङ्क		copy,
    पूर्णाङ्क		*type_error)
{
    यदि (di->di_tv.v_type == VAR_STRING)
    {
	VIM_CLEAR(di->di_tv.vval.v_string);
	यदि (copy || tv->v_type != VAR_STRING)
	{
	    char_u *val = tv_get_string(tv);

	    // Careful: when assigning to v:errmsg and
	    // tv_get_string() causes an error message the variable
	    // will already be set.
	    यदि (di->di_tv.vval.v_string == रिक्त)
		di->di_tv.vval.v_string = vim_strsave(val);
	}
	अन्यथा
	{
	    // Take over the string to avoid an extra alloc/free.
	    di->di_tv.vval.v_string = tv->vval.v_string;
	    tv->vval.v_string = रिक्त;
	}
	प्रत्यागम FALSE;
    }
    अन्यथा यदि (di->di_tv.v_type == VAR_संख्या)
    {
	di->di_tv.vval.v_संख्या = tv_get_संख्या(tv);
	यदि (STRCMP(varname, "searchforward") == 0)
	    set_शोध_direction(di->di_tv.vval.v_संख्या ? '/' : '?');
#ifdef FEAT_शोध_EXTRA
	अन्यथा यदि (STRCMP(varname, "hlsearch") == 0)
	{
	    no_hlsearch = !di->di_tv.vval.v_संख्या;
	    redraw_all_later(UPD_SOME_VALID);
	}
#endif
	प्रत्यागम FALSE;
    }
    अन्यथा यदि (di->di_tv.v_type != tv->v_type)
    {
	*type_error = TRUE;
	प्रत्यागम FALSE;
    }
    प्रत्यागम TRUE;
}

/*
 * Set variable "name" to value in "tv".
 * If the variable already exists, the value is updated.
 * Otherwise the variable is created.
 */
    शून्य
set_var(
    char_u	*name,
    typval_T	*tv,
    पूर्णाङ्क		copy)	    // make copy of value in "tv"
{
    set_var_const(name, 0, रिक्त, tv, copy, ASSIGN_DECL, 0);
}

/*
 * Set variable "name" to value in "tv_arg".
 * When "sid" is non-zero "name" is in the script with this ID.
 * If the variable already exists and "is_const" is FALSE the value is updated.
 * Otherwise the variable is created.
 */
    पूर्णाङ्क
set_var_const(
    char_u	*name,
    scid_T	sid,
    type_T	*type_arg,
    typval_T	*tv_arg,
    पूर्णाङ्क		copy,	    // make copy of value in "tv"
    पूर्णाङ्क		flags_arg,  // ASSIGN_CONST, ASSIGN_FINAL, etc.
    पूर्णाङ्क		var_idx)    // index for ":let [a, b] = list"
{
    typval_T	*tv = tv_arg;
    type_T	*type = type_arg;
    typval_T	bool_tv;
    dictitem_T	*di;
    typval_T	*dest_tv;
    char_u	*varname;
    char_u	*name_tofree = रिक्त;
    hashtab_T	*ht = रिक्त;
    पूर्णाङ्क		is_script_local;
    पूर्णाङ्क		vim9script = in_vim9script();
    पूर्णाङ्क		var_in_vim9script;
    पूर्णाङ्क		var_in_autoload = FALSE;
    पूर्णाङ्क		flags = flags_arg;
    पूर्णाङ्क		free_tv_arg = !copy;  // free tv_arg if not used
    पूर्णाङ्क		rc = FAIL;

    यदि (sid != 0)
    {
	varname = रिक्त;
	यदि (SCRIPT_ID_VALID(sid))
	{
	    char_u	*auto_name = रिक्त;
	    यदि (find_var_autoload_prefix(name, sid, &ht, &auto_name) != रिक्त)
	    {
		var_in_autoload = TRUE;
		varname = auto_name;
		name_tofree = varname;
	    }
	    अन्यथा
		ht = &SCRIPT_VARS(sid);
	}
	यदि (varname == रिक्त)
	    varname = name;
    }
    अन्यथा
    {
	scriptitem_T	*si;
	char_u		*auto_name = रिक्त;

	यदि (in_vim9script()
	    && SCRIPT_ID_VALID(current_sctx.sc_sid)
	    && (si = SCRIPT_ITEM(current_sctx.sc_sid))
					      ->sn_autoload_prefix != रिक्त
	    && (is_export
		|| find_var_autoload_prefix(name, 0, रिक्त, &auto_name)
								    != रिक्त))
	{
	    // In a vim9 autoload script an exported variable is put in the
	    // global namespace with the autoload prefix.
	    var_in_autoload = TRUE;
	    varname = auto_name != रिक्त ? auto_name
		      : concat_str(si->sn_autoload_prefix, name);
	    यदि (varname == रिक्त)
		गच्छ failed;
	    name_tofree = varname;
	    ht = &globvarht;
	}
	अन्यथा
	    ht = find_var_ht(name, &varname);
    }
    यदि (ht == रिक्त || *varname == NUL)
    {
	semsg(_(e_illegal_variable_name_str), name);
	गच्छ failed;
    }
    is_script_local = ht == get_script_local_ht() || sid != 0
							    || var_in_autoload;

    यदि (vim9script
	    && !is_script_local
	    && (flags & (ASSIGN_NO_DECL | ASSIGN_DECL)) == 0
	    && (flags & (ASSIGN_CONST | ASSIGN_FINAL)) == 0
	    && name[1] == ':')
    {
	vim9_declare_error(name);
	गच्छ failed;
    }
    यदि ((flags & ASSIGN_FOR_LOOP) && is_scoped_variable(name))
	// Do not make g:var, w:var, b:var or t:var final.
	flags &= ~ASSIGN_FINAL;

    var_in_vim9script = is_script_local && current_script_is_vim9();
    यदि (var_in_vim9script && name[0] == '_' && name[1] == NUL)
    {
	// For "[a, _] = list" the underscore is ignored.
	यदि ((flags & ASSIGN_UNPACK) == 0)
	    emsg(_(e_cannot_use_underscore_here));
	गच्छ failed;
    }

    di = find_var_in_ht(ht, 0, varname, TRUE);

    यदि (di == रिक्त && var_in_vim9script)
    {
	imported_T  *import = find_imported(varname, 0, FALSE);

	यदि (import != रिक्त)
	{
	    // imported name space cannot be used
	    यदि ((flags & ASSIGN_NO_DECL) == 0)
	    {
		semsg(_(e_redefining_imported_item_str), name);
		गच्छ failed;
	    }
	    semsg(_(e_cannot_use_str_itself_it_is_imported), name);
	    गच्छ failed;
	}
	यदि (!in_vim9script())
	{
	    semsg(_(e_cannot_create_vim9_script_variable_in_function_str),
									 name);
	    गच्छ failed;
	}
    }

    // Search in parent scope which is possible to reference from lambda
    यदि (di == रिक्त)
	di = find_var_in_scoped_ht(name, TRUE);

    यदि ((tv->v_type == VAR_FUNC || tv->v_type == VAR_PARTIAL)
				  && var_wrong_func_name(name, di == रिक्त))
	गच्छ failed;

    यदि (need_convert_to_bool(type, tv))
    {
	// Destination is a bool and the value is not, but it can be
	// converted.
	CLEAR_FIELD(bool_tv);
	bool_tv.v_type = VAR_BOOL;
	bool_tv.vval.v_संख्या = tv2bool(tv) ? VVAL_TRUE : VVAL_FALSE;
	tv = &bool_tv;
    }

    यदि (di != रिक्त)
    {
	// Item already exists.  Allowed to replace when reloading.
	यदि ((di->di_flags & DI_FLAGS_RELOAD) == 0)
	{
	    यदि ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					 && (flags & ASSIGN_FOR_LOOP) == 0)
	    {
		emsg(_(e_cannot_modify_existing_variable));
		गच्छ failed;
	    }

	    यदि (is_script_local && vim9script
			  && (flags & (ASSIGN_NO_DECL | ASSIGN_DECL)) == 0)
	    {
		semsg(_(e_redefining_script_item_str), name);
		गच्छ failed;
	    }

	    यदि (check_typval_is_मूल्य(&di->di_tv) == FAIL)
		गच्छ failed;

	    // List and Blob types can be modified in-place using the "+="
	    // compound operator.  For other types, this is not allowed.
	    पूर्णाङ्क type_inplace_modifiable =
		(di->di_tv.v_type == VAR_LIST || di->di_tv.v_type == VAR_BLOB);

	    यदि (var_in_vim9script && (flags & ASSIGN_FOR_LOOP) == 0
		    && ((flags & ASSIGN_COMPOUND_OP) == 0
			|| !type_inplace_modifiable))
	    {
		where_T where = WHERE_INIT;
		svar_T  *sv = find_typval_in_script(&di->di_tv, sid, TRUE);

		यदि (sv != रिक्त)
		{
		    // check the type and adjust to bool if needed
		    यदि (var_idx > 0)
		    {
			where.wt_index = var_idx;
			where.wt_kind = WT_VARIABLE;
		    }
		    यदि (check_script_var_type(sv, tv, name, where) == FAIL)
			गच्छ failed;
		    यदि (type == रिक्त)
			type = sv->sv_type;
		    sv->sv_flags |= SVFLAG_ASSIGNED;
		}
	    }

	    // Modifying a final variable with a List value using the "+="
	    // operator is allowed.  For other types, it is not allowed.
	    यदि ((((flags & ASSIGN_FOR_LOOP) == 0 || (flags & ASSIGN_DECL) == 0)
			&& ((flags & ASSIGN_COMPOUND_OP) == 0
			    || !type_inplace_modifiable))
				 ? var_check_permission(di, name) == FAIL
				 : var_check_ro(di->di_flags, name, FALSE))
		गच्छ failed;
	}
	अन्यथा
	{
	    // can only redefine once
	    di->di_flags &= ~DI_FLAGS_RELOAD;

	    // A Vim9 script-local variable is also present in sn_all_vars
	    // and sn_var_vals.  It may set "type" from "tv".
	    यदि (var_in_vim9script || var_in_autoload)
		update_vim9_script_var(FALSE, di,
			var_in_autoload ? name : di->di_किल्ली, flags,
			tv, &type, (flags & ASSIGN_NO_MEMBER_TYPE) == 0);
	}

	// existing variable, need to clear the value

	// In Vim9 script a String v: variable only takes a String.
	यदि (ht == &vimvarht && di->di_tv.v_type == VAR_STRING
							    && in_vim9script())
	{
	    where_T	where = WHERE_INIT;

	    यदि (check_typval_type(&t_string, tv, where) == FAIL)
		गच्छ failed;
	}

	// Handle setting internal v: variables separately where needed to
	// prevent changing the type.
	पूर्णाङ्क type_error = FALSE;
	यदि (ht == &vimvarht
		&& !before_set_vvar(varname, di, tv, copy, &type_error))
	{
	    यदि (type_error)
		semsg(_(e_setting_v_str_to_मूल्य_with_wrong_type), varname);
	    गच्छ failed;
	}

	clear_tv(&di->di_tv);

	यदि ((flags & ASSIGN_UPDATE_BLOCK_ID)
				       && SCRIPT_ID_VALID(current_sctx.sc_sid))
	{
	    scriptitem_T *si = SCRIPT_ITEM(current_sctx.sc_sid);

	    update_script_var_block_id(name, si->sn_current_block_id);
	}
    }
    अन्यथा
    {
	// Item not found, check if a function already exists.
	यदि (is_script_local && (flags & (ASSIGN_NO_DECL | ASSIGN_DECL)) == 0
	       && lookup_scriptitem(name, STRLEN(name), FALSE, रिक्त) == OK)
	{
	    semsg(_(e_redefining_script_item_str), name);
	    गच्छ failed;
	}

	// add a new variable
	यदि (var_in_vim9script && (flags & ASSIGN_NO_DECL))
	{
	    semsg(_(e_unknown_variable_str), name);
	    गच्छ failed;
	}

	यदि (check_hashtab_frozen(ht, "add variable"))
	    गच्छ failed;

	// Can't add "v:" or "a:" variable.
	यदि (ht == &vimvarht || ht == get_funccal_args_ht())
	{
	    semsg(_(e_illegal_variable_name_str), name);
	    गच्छ failed;
	}

	// Make sure the variable name is valid.  In Vim9 script an
	// autoload variable must be prefixed with "g:" unless in an
	// autoload script.
	यदि (!valid_varname(varname, -1, !vim9script
			|| STRNCMP(name, "g:", 2) == 0 || var_in_autoload))
	    गच्छ failed;

	di = alloc(offsetof(dictitem_T, di_किल्ली) + STRLEN(varname) + 1);
	यदि (di == रिक्त)
	    गच्छ failed;
	STRCPY(di->di_किल्ली, varname);
	यदि (hash_add(ht, DI2HIKEY(di), "add variable") == FAIL)
	{
	    vim_free(di);
	    गच्छ failed;
	}
	di->di_flags = DI_FLAGS_ALLOC;
	यदि (flags & (ASSIGN_CONST | ASSIGN_FINAL))
	    di->di_flags |= DI_FLAGS_LOCK;

	// A Vim9 script-local variable is also added to sn_all_vars and
	// sn_var_vals. It may set "type" from "tv".
	यदि (var_in_vim9script || var_in_autoload)
	    update_vim9_script_var(TRUE, di,
		    var_in_autoload ? name : di->di_किल्ली, flags,
			  tv, &type, (flags & ASSIGN_NO_MEMBER_TYPE) == 0);
    }

    dest_tv = &di->di_tv;
    यदि (copy || tv->v_type == VAR_संख्या || tv->v_type == VAR_FLOAT)
	copy_tv(tv, dest_tv);
    अन्यथा
    {
	*dest_tv = *tv;
	dest_tv->v_lock = 0;
	init_tv(tv);
    }
    free_tv_arg = FALSE;

    यदि (vim9script && type != रिक्त)
	set_tv_type(dest_tv, type);

    // ":const var = value" locks the value
    // ":final var = value" locks "var"
    यदि (flags & ASSIGN_CONST)
	// Like :lockvar! name: lock the value and what it contains, but only
	// if the reference count is up to one.  That locks only literal
	// values.
	item_lock(dest_tv, DICT_MAXNEST, TRUE, TRUE);

    rc = OK;

failed:
    vim_free(name_tofree);
    यदि (free_tv_arg)
	clear_tv(tv_arg);

    प्रत्यागम rc;
}

/*
 * Check in this order for backwards compatibility:
 * - Whether the variable is read-only
 * - Whether the variable value is locked
 * - Whether the variable is locked
 * NOTE: "name" is only used for error messages.
 */
    पूर्णाङ्क
var_check_permission(dictitem_T *di, char_u *name)
{
    यदि (var_check_ro(di->di_flags, name, FALSE)
		    || मूल्य_check_lock(di->di_tv.v_lock, name, FALSE)
		    || var_check_lock(di->di_flags, name, FALSE))
	प्रत्यागम FAIL;
    प्रत्यागम OK;
}

/*
 * Return TRUE if di_flags "flags" indicates variable "name" is read-only.
 * Also give an error message.
 */
    पूर्णाङ्क
var_check_ro(पूर्णाङ्क flags, char_u *name, पूर्णाङ्क use_gettext)
{
    यदि (flags & DI_FLAGS_RO)
    {
	यदि (name == रिक्त)
	    emsg(_(e_cannot_change_readonly_variable));
	अन्यथा
	    semsg(_(e_cannot_change_readonly_variable_str),
				       use_gettext ? (char_u *)_(name) : name);
	प्रत्यागम TRUE;
    }
    यदि ((flags & DI_FLAGS_RO_SBX) && sandbox)
    {
	यदि (name == रिक्त)
	    emsg(_(e_cannot_set_variable_in_sandbox));
	अन्यथा
	    semsg(_(e_cannot_set_variable_in_sandbox_str),
				       use_gettext ? (char_u *)_(name) : name);
	प्रत्यागम TRUE;
    }
    प्रत्यागम FALSE;
}

/*
 * Return TRUE if di_flags "flags" indicates variable "name" is locked.
 * Also give an error message.
 */
    पूर्णाङ्क
var_check_lock(पूर्णाङ्क flags, char_u *name, पूर्णाङ्क use_gettext)
{
    यदि (flags & DI_FLAGS_LOCK)
    {
	semsg(_(e_variable_is_locked_str),
				       use_gettext ? (char_u *)_(name) : name);
	प्रत्यागम TRUE;
    }
    प्रत्यागम FALSE;
}

/*
 * Return TRUE if di_flags "flags" indicates variable "name" is fixed.
 * Also give an error message.
 */
    पूर्णाङ्क
var_check_fixed(पूर्णाङ्क flags, char_u *name, पूर्णाङ्क use_gettext)
{
    यदि (flags & DI_FLAGS_FIX)
    {
	यदि (name == रिक्त)
	    emsg(_(e_cannot_delete_variable));
	अन्यथा
	    semsg(_(e_cannot_delete_variable_str),
				      use_gettext ? (char_u *)_(name) : name);
	प्रत्यागम TRUE;
    }
    प्रत्यागम FALSE;
}

/*
 * Check if a funcref is assigned to a valid variable name.
 * Return TRUE and give an error if not.
 */
    पूर्णाङ्क
var_wrong_func_name(
    char_u *name,    // points to start of variable name
    पूर्णाङ्क    new_var)  // TRUE when creating the variable
{
    // Allow for w: b: s: and t:.  In Vim9 script s: is not allowed, because
    // the name can be used without the s: prefix.
    // Allow autoload variable.
    यदि (!((vim_strchr((char_u *)"wbt", name[0]) != रिक्त
		    || (!in_vim9script() && name[0] == 's')) && name[1] == ':')
	    && !ASCII_ISUPPER((name[0] != NUL && name[1] == ':')
						     ? name[2] : name[0])
	    && vim_strchr(name, '#') == रिक्त)
    {
	semsg(_(e_funcref_variable_name_must_start_with_capital_str), name);
	प्रत्यागम TRUE;
    }
    // Don't allow hiding a function.  When "v" is not NULL we might be
    // assigning another function to the same var, the type is checked
    // below.
    यदि (new_var && function_exists(name, FALSE))
    {
	semsg(_(e_variable_name_conflicts_with_existing_function_str),
								    name);
	प्रत्यागम TRUE;
    }
    प्रत्यागम FALSE;
}

/*
 * Return TRUE if "flags" indicates variable "name" has a locked (immutable)
 * value.  Also give an error message, using "name" or _("name") when
 * "use_gettext" is TRUE.
 */
    पूर्णाङ्क
मूल्य_check_lock(पूर्णाङ्क lock, char_u *name, पूर्णाङ्क use_gettext)
{
    यदि (lock & VAR_LOCKED)
    {
	यदि (name == रिक्त)
	    emsg(_(e_मूल्य_is_locked));
	अन्यथा
	    semsg(_(e_मूल्य_is_locked_str),
				       use_gettext ? (char_u *)_(name) : name);
	प्रत्यागम TRUE;
    }
    यदि (lock & VAR_FIXED)
    {
	यदि (name == रिक्त)
	    emsg(_(e_cannot_change_मूल्य));
	अन्यथा
	    semsg(_(e_cannot_change_मूल्य_of_str),
				       use_gettext ? (char_u *)_(name) : name);
	प्रत्यागम TRUE;
    }
    प्रत्यागम FALSE;
}

/*
 * Check if a variable name is valid.  When "autoload" is true "#" is allowed.
 * If "len" is -1 use all of "varname", otherwise up to "varname[len]".
 * Return FALSE and give an error if not.
 */
    पूर्णाङ्क
valid_varname(char_u *varname, पूर्णाङ्क len, पूर्णाङ्क autoload)
{
    char_u *p;

    प्रति (p = varname; len < 0 ? *p != NUL : p < varname + len; ++p)
	यदि (!eval_isnamec1(*p) && (p == varname || !VIM_ISDIGIT(*p))
					 && !(autoload && *p == AUTOLOAD_CHAR))
	{
	    semsg(_(e_illegal_variable_name_str), varname);
	    प्रत्यागम FALSE;
	}
    प्रत्यागम TRUE;
}

/*
 * Implements the logic to retrieve local variable and option values.
 * Used by "getwinvar()" "gettabvar()" "gettabwinvar()" "getbufvar()".
 */
    स्थिर शून्य
get_var_from(
    char_u	*varname,
    typval_T	*rettv,
    typval_T	*deftv,	    // Default value if not found.
    पूर्णाङ्क		htname,	    // 't'ab, 'w'indow or 'b'uffer local.
    tabpage_T	*tp,	    // can be NULL
    win_T	*win,
    buf_T	*buf)	    // Ignored if htname is not 'b'.
{
    dictitem_T	*v;
    पूर्णाङ्क		done = FALSE;
    switchwin_T	switchwin;
    पूर्णाङ्क		need_switch_win;
    पूर्णाङ्क		do_change_curbuf = buf != रिक्त && htname == 'b';

    ++emsg_off;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = रिक्त;

    यदि (varname != रिक्त && tp != रिक्त && win != रिक्त
	    && (htname != 'b' || buf != रिक्त))
    {
	// Set curwin to be our win, temporarily.  Also set the tabpage,
	// otherwise the window is not valid. Only do this when needed,
	// autocommands get blocked.
	// If we have a buffer reference avoid the switching, we're saving and
	// restoring curbuf directly.
	need_switch_win = !(tp == curtab && win == curwin) && !do_change_curbuf;
	यदि (!need_switch_win || switch_win(&switchwin, win, tp, TRUE) == OK)
	{
	    // Handle options. There are no tab-local options.
	    यदि (*varname == '&' && htname != 't')
	    {
		buf_T	*save_curbuf = curbuf;

		// Change curbuf so the option is read from the correct buffer.
		यदि (do_change_curbuf)
		    curbuf = buf;

		यदि (varname[1] == NUL)
		{
		    // get all window-local or buffer-local options in a dict
		    dict_T	*opts = get_winbuf_options(htname == 'b');

		    यदि (opts != रिक्त)
		    {
			rettv_dict_set(rettv, opts);
			done = TRUE;
		    }
		}
		अन्यथा यदि (eval_option(&varname, rettv, TRUE) == OK)
		    // Local option
		    done = TRUE;

		curbuf = save_curbuf;
	    }
	    अन्यथा यदि (*varname == NUL)
	    {
		// Empty string: return a dict with all the local variables.
		यदि (htname == 'b')
		    v = &buf->b_bufvar;
		अन्यथा यदि (htname == 'w')
		    v = &win->w_winvar;
		अन्यथा
		    v = &tp->tp_winvar;
		copy_tv(&v->di_tv, rettv);
		done = TRUE;
	    }
	    अन्यथा
	    {
		hashtab_T	*ht;

		यदि (htname == 'b')
		    ht = &buf->b_vars->dv_hashtab;
		अन्यथा यदि (htname == 'w')
		    ht = &win->w_vars->dv_hashtab;
		अन्यथा
		    ht = &tp->tp_vars->dv_hashtab;

		// Look up the variable.
		v = find_var_in_ht(ht, htname, varname, FALSE);
		यदि (v != रिक्त)
		{
		    copy_tv(&v->di_tv, rettv);
		    done = TRUE;
		}
	    }
	}

	यदि (need_switch_win)
	    // restore previous notion of curwin
	    restore_win(&switchwin, TRUE);
    }

    यदि (!done && deftv->v_type != VAR_UNKNOWN)
	// use the default value
	copy_tv(deftv, rettv);

    --emsg_off;
}

/*
 * getwinvar() and gettabwinvar()
 */
    स्थिर शून्य
getwinvar(
    typval_T	*argvars,
    typval_T	*rettv,
    पूर्णाङ्क		off)	    // 1 for gettabwinvar()
{
    char_u	*varname;
    tabpage_T	*tp;
    win_T	*win;

    यदि (off == 1)
	tp = find_tabpage((पूर्णाङ्क)tv_get_संख्या_chk(&argvars[0], रिक्त));
    अन्यथा
	tp = curtab;
    win = find_win_by_nr(&argvars[off], tp);
    varname = tv_get_string_chk(&argvars[off + 1]);

    get_var_from(varname, rettv, &argvars[off + 2], 'w', tp, win, रिक्त);
}

/*
 * Set option "varname" to the value of "varp" for the current buffer/window.
 */
    स्थिर शून्य
set_option_from_tv(char_u *varname, typval_T *varp)
{
    long	numval = 0;
    char_u	*strval;
    char_u	nbuf[NUMBUFLEN];
    पूर्णाङ्क		error = FALSE;

    पूर्णाङ्क		opt_idx = findoption(varname);
    यदि (opt_idx < 0)
    {
	semsg(_(e_unknown_option_str_2), varname);
	प्रत्यागम;
    }
    पूर्णाङ्क		opt_p_flags = get_option_flags(opt_idx);

    यदि (varp->v_type == VAR_BOOL)
    {
	यदि (opt_p_flags & P_STRING)
	{
	    emsg(_(e_string_required));
	    प्रत्यागम;
	}
	numval = (long)varp->vval.v_संख्या;
	strval = (char_u *)"0";  // avoid using "false"
    }
    अन्यथा
    {
	यदि ((opt_p_flags & (P_अंक|P_BOOL))
		&& (!in_vim9script() || varp->v_type != VAR_STRING))
	    numval = (long)tv_get_संख्या_chk(varp, &error);
	यदि (!error)
	    strval = tv_get_string_buf_chk(varp, nbuf);
    }
    यदि (!error && strval != रिक्त)
	set_option_मूल्य_give_err(varname, numval, strval, OPT_LOCAL);
}

/*
 * "setwinvar()" and "settabwinvar()" functions
 */
    स्थिर शून्य
setwinvar(typval_T *argvars, पूर्णाङ्क off)
{
    win_T	*win;
    switchwin_T	switchwin;
    पूर्णाङ्क		need_switch_win;
    char_u	*varname, *winvarname;
    typval_T	*varp;
    tabpage_T	*tp = रिक्त;

    यदि (check_secure())
	प्रत्यागम;

    यदि (off == 1)
	tp = find_tabpage((पूर्णाङ्क)tv_get_संख्या_chk(&argvars[0], रिक्त));
    अन्यथा
	tp = curtab;
    win = find_win_by_nr(&argvars[off], tp);
    varname = tv_get_string_chk(&argvars[off + 1]);
    varp = &argvars[off + 2];

    यदि (win == रिक्त || varname == रिक्त)
	प्रत्यागम;

    need_switch_win = !(tp == curtab && win == curwin);
    यदि (!need_switch_win
	    || switch_win(&switchwin, win, tp, TRUE) == OK)
    {
	यदि (*varname == '&')
	    set_option_from_tv(varname + 1, varp);
	अन्यथा
	{
	    winvarname = alloc(STRLEN(varname) + 3);
	    यदि (winvarname != रिक्त)
	    {
		STRCPY(winvarname, "w:");
		STRCPY(winvarname + 2, varname);
		set_var(winvarname, varp, TRUE);
		vim_free(winvarname);
	    }
	}
    }
    यदि (need_switch_win)
	restore_win(&switchwin, TRUE);
}

/*
 * reset v:option_new, v:option_old, v:option_oldlocal, v:option_oldglobal,
 * v:option_type, and v:option_command.
 */
    शून्य
reset_v_option_vars(शून्य)
{
    set_vim_var_string(VV_OPTION_NEW, रिक्त, -1);
    set_vim_var_string(VV_OPTION_OLD, रिक्त, -1);
    set_vim_var_string(VV_OPTION_OLDLOCAL, रिक्त, -1);
    set_vim_var_string(VV_OPTION_OLDGLOBAL, रिक्त, -1);
    set_vim_var_string(VV_OPTION_TYPE, रिक्त, -1);
    set_vim_var_string(VV_OPTION_COMMAND, रिक्त, -1);
}

/*
 * Add an assert error to v:errors.
 */
    शून्य
assert_error(garray_T *gap)
{
    संरचना vimvar   *vp = &vimvars[VV_ERRORS];

    यदि (vp->vv_tv_type != VAR_LIST || vimvars[VV_ERRORS].vv_list == रिक्त)
	// Make sure v:errors is a list.
	set_vim_var_list(VV_ERRORS, list_alloc());
    list_append_string(vimvars[VV_ERRORS].vv_list, gap->ga_data, gap->ga_len);
}

    पूर्णाङ्क
var_exists(char_u *var)
{
    char_u	*arg = var;
    char_u	*name;
    char_u	*tofree;
    typval_T    tv;
    पूर्णाङ्क		len = 0;
    पूर्णाङ्क		n = FALSE;

    // get_name_len() takes care of expanding curly braces
    name = var;
    len = get_name_len(&arg, &tofree, TRUE, FALSE);
    यदि (len > 0)
    {
	यदि (tofree != रिक्त)
	    name = tofree;
	n = (eval_variable(name, len, 0, &tv, रिक्त,
				 EVAL_VAR_NOAUTOLOAD + EVAL_VAR_IMPORT) == OK);
	यदि (n)
	{
	    // handle d.key, l[idx], f(expr)
	    arg = skipwhite(arg);
	    n = (handle_subscript(&arg, name, &tv, &EVALARG_EVALUATE,
								 FALSE) == OK);
	    यदि (n)
		clear_tv(&tv);
	}
    }
    यदि (*arg != NUL)
	n = FALSE;

    vim_free(tofree);
    प्रत्यागम n;
}

स्थिर lval_T	*redir_lval = रिक्त;
#define EVALCMD_BUSY (redir_lval == (lval_T *)&redir_lval)
स्थिर garray_T redir_ga;	// only valid when redir_lval is not NULL
स्थिर char_u	*redir_endp = रिक्त;
स्थिर char_u	*redir_varname = रिक्त;

    पूर्णाङ्क
alloc_redir_lval(शून्य)
{
    redir_lval = ALLOC_CLEAR_ONE(lval_T);
    यदि (redir_lval == रिक्त)
	प्रत्यागम FAIL;
    प्रत्यागम OK;
}

    शून्य
clear_redir_lval(शून्य)
{
    VIM_CLEAR(redir_lval);
}

    शून्य
init_redir_ga(शून्य)
{
    ga_init2(&redir_ga, परिमाण(वर्ण), 500);
}

/*
 * Start recording command output to a variable
 * When "append" is TRUE append to an existing variable.
 * Returns OK if successfully completed the setup.  FAIL otherwise.
 */
    पूर्णाङ्क
var_redir_start(char_u *name, पूर्णाङ्क append)
{
    पूर्णाङ्क		called_emsg_before;
    typval_T	tv;

    // Catch a bad name early.
    यदि (!eval_isnamec1(*name))
    {
	emsg(_(e_invalid_argument));
	प्रत्यागम FAIL;
    }

    // Make a copy of the name, it is used in redir_lval until redir ends.
    redir_varname = vim_strsave(name);
    यदि (redir_varname == रिक्त)
	प्रत्यागम FAIL;

    यदि (alloc_redir_lval() == FAIL)
    {
	var_redir_stop();
	प्रत्यागम FAIL;
    }

    // The output is stored in growarray "redir_ga" until redirection ends.
    init_redir_ga();

    // Parse the variable name (can be a dict or list entry).
    redir_endp = get_lval(redir_varname, रिक्त, redir_lval, FALSE, FALSE, 0,
							     FNE_CHECK_START);
    यदि (redir_endp == रिक्त || redir_lval->ll_name == रिक्त || *redir_endp != NUL)
    {
	clear_lval(redir_lval);
	यदि (redir_endp != रिक्त && *redir_endp != NUL)
	    // Trailing characters are present after the variable name
	    semsg(_(e_trailing_characters_str), redir_endp);
	अन्यथा
	    semsg(_(e_invalid_argument_str), name);
	redir_endp = रिक्त;  // don't store a value, only cleanup
	var_redir_stop();
	प्रत्यागम FAIL;
    }

    // check if we can write to the variable: set it to or append an empty
    // string
    called_emsg_before = called_emsg;
    tv.v_type = VAR_STRING;
    tv.vval.v_string = (char_u *)"";
    यदि (append)
	set_var_lval(redir_lval, redir_endp, &tv, TRUE,
					     ASSIGN_NO_DECL, (char_u *)".", 0);
    अन्यथा
	set_var_lval(redir_lval, redir_endp, &tv, TRUE,
					     ASSIGN_NO_DECL, (char_u *)"=", 0);
    clear_lval(redir_lval);
    यदि (called_emsg > called_emsg_before)
    {
	redir_endp = रिक्त;  // don't store a value, only cleanup
	var_redir_stop();
	प्रत्यागम FAIL;
    }

    प्रत्यागम OK;
}

/*
 * Append "value[value_len]" to the variable set by var_redir_start().
 * The actual appending is postponed until redirection ends, because the value
 * appended may in fact be the string we write to, changing it may cause freed
 * memory to be used:
 *   :redir => foo
 *   :let foo
 *   :redir END
 */
    शून्य
var_redir_str(char_u *मूल्य, पूर्णाङ्क मूल्य_len)
{
    पूर्णाङ्क		len;

    यदि (redir_lval == रिक्त)
	प्रत्यागम;

    यदि (मूल्य_len == -1)
	len = (पूर्णाङ्क)STRLEN(मूल्य);	// Append the entire string
    अन्यथा
	len = मूल्य_len;		// Append only "value_len" characters

    यदि (ga_grow(&redir_ga, len) == OK)
    {
	mch_memmove((वर्ण *)redir_ga.ga_data + redir_ga.ga_len, मूल्य, len);
	redir_ga.ga_len += len;
    }
    अन्यथा
	var_redir_stop();
}

/*
 * Stop redirecting command output to a variable.
 * Frees the allocated memory.
 */
    शून्य
var_redir_stop(शून्य)
{
    typval_T	tv;

    यदि (EVALCMD_BUSY)
    {
	redir_lval = रिक्त;
	प्रत्यागम;
    }

    यदि (redir_lval != रिक्त)
    {
	// If there was no error: assign the text to the variable.
	यदि (redir_endp != रिक्त)
	{
	    ga_append(&redir_ga, NUL);  // Append the trailing NUL.
	    tv.v_type = VAR_STRING;
	    tv.vval.v_string = redir_ga.ga_data;
	    // Call get_lval() again, if it's inside a Dict or List it may
	    // have changed.
	    redir_endp = get_lval(redir_varname, रिक्त, redir_lval,
					FALSE, FALSE, 0, FNE_CHECK_START);
	    यदि (redir_endp != रिक्त && redir_lval->ll_name != रिक्त)
		set_var_lval(redir_lval, redir_endp, &tv, FALSE, 0,
							     (char_u *)".", 0);
	    clear_lval(redir_lval);
	}

	// free the collected output
	VIM_CLEAR(redir_ga.ga_data);

	VIM_CLEAR(redir_lval);
    }
    VIM_CLEAR(redir_varname);
}

/*
 * Get the collected redirected text and clear redir_ga.
 */
    char_u *
get_clear_redir_ga(शून्य)
{
    char_u *res;

    ga_append(&redir_ga, NUL);  // Append the trailing NUL.
    res = redir_ga.ga_data;
    redir_ga.ga_data = रिक्त;
    प्रत्यागम res;
}

/*
 * "gettabvar()" function
 */
    शून्य
f_gettabvar(typval_T *argvars, typval_T *rettv)
{
    char_u	*varname;
    tabpage_T	*tp;
    win_T	*win = रिक्त;

    यदि (in_vim9script()
	    && (check_for_संख्या_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    varname = tv_get_string_chk(&argvars[1]);
    tp = find_tabpage((पूर्णाङ्क)tv_get_संख्या_chk(&argvars[0], रिक्त));
    यदि (tp != रिक्त)
	win = tp == curtab || tp->tp_firstwin == रिक्त ? firstwin
		: tp->tp_firstwin;

    get_var_from(varname, rettv, &argvars[2], 't', tp, win, रिक्त);
}

/*
 * "gettabwinvar()" function
 */
    शून्य
f_gettabwinvar(typval_T *argvars, typval_T *rettv)
{
    यदि (in_vim9script()
	    && (check_for_संख्या_arg(argvars, 0) == FAIL
		|| check_for_संख्या_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL))
	प्रत्यागम;

    getwinvar(argvars, rettv, 1);
}

/*
 * "getwinvar()" function
 */
    शून्य
f_getwinvar(typval_T *argvars, typval_T *rettv)
{
    यदि (in_vim9script()
	    && (check_for_संख्या_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    getwinvar(argvars, rettv, 0);
}

/*
 * "getbufvar()" function
 */
    शून्य
f_getbufvar(typval_T *argvars, typval_T *rettv)
{
    char_u	*varname;
    buf_T	*buf;

    यदि (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    varname = tv_get_string_chk(&argvars[1]);
    buf = tv_get_buf_from_arg(&argvars[0]);

    get_var_from(varname, rettv, &argvars[2], 'b', curtab, curwin, buf);
}

/*
 * "settabvar()" function
 */
    शून्य
f_settabvar(typval_T *argvars, typval_T *rettv UNUSED)
{
    tabpage_T	*save_curtab;
    tabpage_T	*tp;
    tabpage_T	*save_lu_tp;
    char_u	*varname, *tabvarname;
    typval_T	*varp;

    यदि (check_secure())
	प्रत्यागम;

    यदि (in_vim9script()
	    && (check_for_संख्या_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    tp = find_tabpage((पूर्णाङ्क)tv_get_संख्या_chk(&argvars[0], रिक्त));
    varname = tv_get_string_chk(&argvars[1]);
    varp = &argvars[2];

    यदि (varname == रिक्त || tp == रिक्त)
	प्रत्यागम;

    save_curtab = curtab;
    save_lu_tp = lastused_tabpage;
    goto_tabpage_tp(tp, FALSE, FALSE);

    tabvarname = alloc(STRLEN(varname) + 3);
    यदि (tabvarname != रिक्त)
    {
	STRCPY(tabvarname, "t:");
	STRCPY(tabvarname + 2, varname);
	set_var(tabvarname, varp, TRUE);
	vim_free(tabvarname);
    }

    // Restore current tabpage and last accessed tabpage.
    यदि (valid_tabpage(save_curtab))
    {
	goto_tabpage_tp(save_curtab, FALSE, FALSE);
	यदि (valid_tabpage(save_lu_tp))
	    lastused_tabpage = save_lu_tp;
    }
}

/*
 * "settabwinvar()" function
 */
    शून्य
f_settabwinvar(typval_T *argvars, typval_T *rettv UNUSED)
{
    यदि (in_vim9script()
	    && (check_for_संख्या_arg(argvars, 0) == FAIL
		|| check_for_संख्या_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL))
	प्रत्यागम;

    setwinvar(argvars, 1);
}

/*
 * "setwinvar()" function
 */
    शून्य
f_setwinvar(typval_T *argvars, typval_T *rettv UNUSED)
{
    यदि (in_vim9script()
	    && (check_for_संख्या_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    setwinvar(argvars, 0);
}

/*
 * "setbufvar()" function
 */
    शून्य
f_setbufvar(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    char_u	*varname, *bufvarname;
    typval_T	*varp;

    यदि (check_secure())
	प्रत्यागम;

    यदि (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    varname = tv_get_string_chk(&argvars[1]);
    buf = tv_get_buf_from_arg(&argvars[0]);
    varp = &argvars[2];

    यदि (buf == रिक्त || varname == रिक्त)
	प्रत्यागम;

    यदि (*varname == '&')
    {
	aco_save_T	aco;
	// safe the current window position, it could
	// change because of 'scrollbind' window-local
	// options
	linenr_T	old_topline = curwin->w_topline;

	// Set curbuf to be our buf, temporarily.
	aucmd_prepbuf(&aco, buf);
	यदि (curbuf == buf)
	{
	    // Only when it worked to set "curbuf".
	    set_option_from_tv(varname + 1, varp);

	    // reset notion of buffer
	    aucmd_restbuf(&aco);
	}
	curwin->w_topline = old_topline;
    }
    अन्यथा
    {
	bufvarname = alloc(STRLEN(varname) + 3);
	यदि (bufvarname != रिक्त)
	{
	    buf_T *save_curbuf = curbuf;

	    curbuf = buf;
	    STRCPY(bufvarname, "b:");
	    STRCPY(bufvarname + 2, varname);
	    set_var(bufvarname, varp, TRUE);
	    vim_free(bufvarname);
	    curbuf = save_curbuf;
	}
    }
}

/*
 * Get a callback from "arg".  It can be a Funcref or a function name.
 * When "arg" is zero "res.cb_name" is set to an empty string.
 * If "res.cb_name" is allocated then "res.cb_free_name" is set to TRUE.
 * "res.cb_name" is set to NULL for an invalid argument.
 */
    callback_T
get_callback(typval_T *arg)
{
    callback_T  res;
    पूर्णाङ्क		r = OK;

    CLEAR_FIELD(res);
    यदि (arg->v_type == VAR_PARTIAL && arg->vval.v_partial != रिक्त)
    {
	res.cb_partial = arg->vval.v_partial;
	++res.cb_partial->pt_refcount;
	res.cb_name = partial_name(res.cb_partial);
    }
    अन्यथा
    {
	यदि (arg->v_type == VAR_STRING && arg->vval.v_string != रिक्त
					       && SAFE_isdigit(*arg->vval.v_string))
	    r = FAIL;
	अन्यथा यदि (arg->v_type == VAR_FUNC || arg->v_type == VAR_STRING)
	{
	    res.cb_name = arg->vval.v_string;
	    यदि (arg->v_type == VAR_STRING)
	    {
		char_u *name = get_scriptlocal_funcname(arg->vval.v_string);
		यदि (name != रिक्त)
		{
		    res.cb_name = name;
		    res.cb_free_name = TRUE;
		}
	    }
	    func_ref(res.cb_name);
	}
	अन्यथा यदि (arg->v_type == VAR_संख्या && arg->vval.v_संख्या == 0)
	    res.cb_name = (char_u *)"";
	अन्यथा
	    r = FAIL;

	यदि (r == FAIL)
	{
	    emsg(_(e_invalid_callback_argument));
	    res.cb_name = रिक्त;
	}
    }
    प्रत्यागम res;
}

/*
 * Copy a callback into a typval_T.
 */
    शून्य
put_callback(callback_T *cb, typval_T *tv)
{
    यदि (cb->cb_partial != रिक्त)
    {
	tv->v_type = VAR_PARTIAL;
	tv->vval.v_partial = cb->cb_partial;
	++tv->vval.v_partial->pt_refcount;
    }
    अन्यथा
    {
	tv->v_type = VAR_FUNC;
	tv->vval.v_string = vim_strsave(cb->cb_name);
	func_ref(cb->cb_name);
    }
}

    स्थिर bool
does_callback_own_cb_name(callback_T *cb)
{
    // If cb_partial != NULL then *cb->cb_name is owned by the partial.
    प्रत्यागम cb->cb_partial || cb->cb_free_name;
}

/*
 * Make a copy of "src" into "dest", allocating the function name if needed,
 * without incrementing the refcount.
 */
    शून्य
set_callback(callback_T *dest, callback_T *src)
{
    *dest = *src;
    यदि (!does_callback_own_cb_name(src))
    {
	dest->cb_name = vim_strsave(src->cb_name);
	dest->cb_free_name = TRUE;
    }
    *src = (callback_T){0};
}

/*
 * Copy callback from "src" to "dest", incrementing the refcounts.
 */
    शून्य
copy_callback(callback_T *dest, callback_T *src)
{
    dest->cb_partial = src->cb_partial;
    यदि (dest->cb_partial != रिक्त)
    {
	dest->cb_name = src->cb_name;
	dest->cb_free_name = FALSE;
	++dest->cb_partial->pt_refcount;
    }
    अन्यथा
    {
	dest->cb_name = vim_strsave(src->cb_name);
	dest->cb_free_name = TRUE;
	func_ref(src->cb_name);
    }
}

/*
 * When a callback refers to an autoload import, change the function name to
 * the "path#name" form.  Uses the current script context.
 * Only works when the name is allocated.
 */
    शून्य
expand_autload_callback(callback_T *cb)
{
    char_u	*name;
    char_u	*p;
    imported_T	*import;

    यदि (!in_vim9script() || cb->cb_name == रिक्त
	    || (!cb->cb_free_name
	       && (cb->cb_partial == रिक्त || cb->cb_partial->pt_name == रिक्त)))
	प्रत्यागम;
    यदि (cb->cb_partial != रिक्त)
	name = cb->cb_partial->pt_name;
    अन्यथा
	name = cb->cb_name;
    p = vim_strchr(name, '.');
    यदि (p == रिक्त)
	प्रत्यागम;

    import = find_imported(name, p - name, FALSE);
    यदि (import == रिक्त || !SCRIPT_ID_VALID(import->imp_sid))
	प्रत्यागम;

    scriptitem_T *si = SCRIPT_ITEM(import->imp_sid);
    यदि (si->sn_autoload_prefix == रिक्त)
	प्रत्यागम;

    char_u *newname = concat_str(si->sn_autoload_prefix, p + 1);
    यदि (newname == रिक्त)
	प्रत्यागम;

    यदि (cb->cb_partial != रिक्त)
    {
	यदि (cb->cb_name == cb->cb_partial->pt_name)
	    cb->cb_name = newname;
	vim_free(cb->cb_partial->pt_name);
	cb->cb_partial->pt_name = newname;
    }
    अन्यथा
    {
	vim_free(cb->cb_name);
	cb->cb_name = newname;
    }
}

/*
 * Unref/free "callback" returned by get_callback() or set_callback().
 */
    शून्य
free_callback(callback_T *callback)
{
    यदि (callback->cb_partial != रिक्त)
    {
	partial_unref(callback->cb_partial);
	callback->cb_partial = रिक्त;
    }
    अन्यथा यदि (callback->cb_name != रिक्त)
	func_unref(callback->cb_name);
    यदि (callback->cb_free_name)
    {
	vim_free(callback->cb_name);
	callback->cb_free_name = FALSE;
    }
    callback->cb_name = रिक्त;
}

#endif // FEAT_EVAL
