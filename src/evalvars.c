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

#सचे defined(FEAT_EVAL)

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
    {VV_NAME("count",		 VAR_सङ्ख्या), रिक्त, VV_COMPAT+VV_RO},
    {VV_NAME("count1",		 VAR_सङ्ख्या), रिक्त, VV_RO},
    {VV_NAME("prevcount",	 VAR_सङ्ख्या), रिक्त, VV_RO},
    {VV_NAME("errmsg",		 VAR_STRING), रिक्त, VV_COMPAT},
    {VV_NAME("warningmsg",	 VAR_STRING), रिक्त, 0},
    {VV_NAME("statusmsg",	 VAR_STRING), रिक्त, 0},
    {VV_NAME("shell_error",	 VAR_सङ्ख्या), रिक्त, VV_COMPAT+VV_RO},
    {VV_NAME("this_session",	 VAR_STRING), रिक्त, VV_COMPAT},
    {VV_NAME("version",		 VAR_सङ्ख्या), रिक्त, VV_COMPAT+VV_RO},
    {VV_NAME("lnum",		 VAR_सङ्ख्या), रिक्त, VV_RO_SBX},
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
    {VV_NAME("foldstart",	 VAR_सङ्ख्या), रिक्त, VV_RO_SBX},
    {VV_NAME("foldend",		 VAR_सङ्ख्या), रिक्त, VV_RO_SBX},
    {VV_NAME("folddashes",	 VAR_STRING), रिक्त, VV_RO_SBX},
    {VV_NAME("foldlevel",	 VAR_सङ्ख्या), रिक्त, VV_RO_SBX},
    {VV_NAME("progname",	 VAR_STRING), रिक्त, VV_RO},
    {VV_NAME("servername",	 VAR_STRING), रिक्त, VV_RO},
    {VV_NAME("dying",		 VAR_सङ्ख्या), रिक्त, VV_RO},
    {VV_NAME("exception",	 VAR_STRING), रिक्त, VV_RO},
    {VV_NAME("throwpoint",	 VAR_STRING), रिक्त, VV_RO},
    {VV_NAME("register",	 VAR_STRING), रिक्त, VV_RO},
    {VV_NAME("cmdbang",		 VAR_सङ्ख्या), रिक्त, VV_RO},
    {VV_NAME("insertmode",	 VAR_STRING), रिक्त, VV_RO},
    {VV_NAME("val",		 VAR_UNKNOWN), रिक्त, VV_RO},
    {VV_NAME("key",		 VAR_UNKNOWN), रिक्त, VV_RO},
    {VV_NAME("profiling",	 VAR_सङ्ख्या), रिक्त, VV_RO},
    {VV_NAME("fcs_reason",	 VAR_STRING), रिक्त, VV_RO},
    {VV_NAME("fcs_choice",	 VAR_STRING), रिक्त, 0},
    {VV_NAME("beval_bufnr",	 VAR_सङ्ख्या), रिक्त, VV_RO},
    {VV_NAME("beval_winnr",	 VAR_सङ्ख्या), रिक्त, VV_RO},
    {VV_NAME("beval_winid",	 VAR_सङ्ख्या), रिक्त, VV_RO},
    {VV_NAME("beval_lnum",	 VAR_सङ्ख्या), रिक्त, VV_RO},
    {VV_NAME("beval_col",	 VAR_सङ्ख्या), रिक्त, VV_RO},
    {VV_NAME("beval_text",	 VAR_STRING), रिक्त, VV_RO},
    {VV_NAME("scrollstart",	 VAR_STRING), रिक्त, 0},
    {VV_NAME("swapname",	 VAR_STRING), रिक्त, VV_RO},
    {VV_NAME("swapchoice",	 VAR_STRING), रिक्त, 0},
    {VV_NAME("swapcommand",	 VAR_STRING), रिक्त, VV_RO},
    {VV_NAME("char",		 VAR_STRING), रिक्त, 0},
    {VV_NAME("mouse_win",	 VAR_सङ्ख्या), रिक्त, 0},
    {VV_NAME("mouse_winid",	 VAR_सङ्ख्या), रिक्त, 0},
    {VV_NAME("mouse_lnum",	 VAR_सङ्ख्या), रिक्त, 0},
    {VV_NAME("mouse_col",	 VAR_सङ्ख्या), रिक्त, 0},
    {VV_NAME("operator",	 VAR_STRING), रिक्त, VV_RO},
    {VV_NAME("searchforward",	 VAR_सङ्ख्या), रिक्त, 0},
    {VV_NAME("hlsearch",	 VAR_सङ्ख्या), रिक्त, 0},
    {VV_NAME("oldfiles",	 VAR_LIST), &t_list_string, 0},
    {VV_NAME("windowid",	 VAR_सङ्ख्या), रिक्त, VV_RO},
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
    {VV_NAME("numbermax",	 VAR_सङ्ख्या), रिक्त, VV_RO},
    {VV_NAME("numbermin",	 VAR_सङ्ख्या), रिक्त, VV_RO},
    {VV_NAME("numbersize",	 VAR_सङ्ख्या), रिक्त, VV_RO},
    {VV_NAME("vim_did_enter",	 VAR_सङ्ख्या), रिक्त, VV_RO},
    {VV_NAME("testing",		 VAR_सङ्ख्या), रिक्त, 0},
    {VV_NAME("t_number",	 VAR_सङ्ख्या), रिक्त, VV_RO},
    {VV_NAME("t_string",	 VAR_सङ्ख्या), रिक्त, VV_RO},
    {VV_NAME("t_func",		 VAR_सङ्ख्या), रिक्त, VV_RO},
    {VV_NAME("t_list",		 VAR_सङ्ख्या), रिक्त, VV_RO},
    {VV_NAME("t_dict",		 VAR_सङ्ख्या), रिक्त, VV_RO},
    {VV_NAME("t_float",		 VAR_सङ्ख्या), रिक्त, VV_RO},
    {VV_NAME("t_bool",		 VAR_सङ्ख्या), रिक्त, VV_RO},
    {VV_NAME("t_none",		 VAR_सङ्ख्या), रिक्त, VV_RO},
    {VV_NAME("t_job",		 VAR_सङ्ख्या), रिक्त, VV_RO},
    {VV_NAME("t_channel",	 VAR_सङ्ख्या), रिक्त, VV_RO},
    {VV_NAME("t_blob",		 VAR_सङ्ख्या), रिक्त, VV_RO},
    {VV_NAME("t_class",		 VAR_सङ्ख्या), रिक्त, VV_RO},
    {VV_NAME("t_object",	 VAR_सङ्ख्या), रिक्त, VV_RO},
    {VV_NAME("termrfgresp",	 VAR_STRING), रिक्त, VV_RO},
    {VV_NAME("termrbgresp",	 VAR_STRING), रिक्त, VV_RO},
    {VV_NAME("termu7resp",	 VAR_STRING), रिक्त, VV_RO},
    {VV_NAME("termstyleresp",	 VAR_STRING), रिक्त, VV_RO},
    {VV_NAME("termblinkresp",	 VAR_STRING), रिक्त, VV_RO},
    {VV_NAME("event",		 VAR_DICT), रिक्त, VV_RO},
    {VV_NAME("versionlong",	 VAR_सङ्ख्या), रिक्त, VV_RO},
    {VV_NAME("echospace",	 VAR_सङ्ख्या), रिक्त, VV_RO},
    {VV_NAME("argv",		 VAR_LIST), &t_list_string, VV_RO},
    {VV_NAME("collate",		 VAR_STRING), रिक्त, VV_RO},
    {VV_NAME("exiting",		 VAR_SPECIAL), रिक्त, VV_RO},
    {VV_NAME("colornames",       VAR_DICT), &t_dict_string, VV_RO},
    {VV_NAME("sizeofint",	 VAR_सङ्ख्या), रिक्त, VV_RO},
    {VV_NAME("sizeoflong",	 VAR_सङ्ख्या), रिक्त, VV_RO},
    {VV_NAME("sizeofpointer",	 VAR_सङ्ख्या), रिक्त, VV_RO},
    {VV_NAME("maxcol",		 VAR_सङ्ख्या), रिक्त, VV_RO},
    {VV_NAME("python3_version",	 VAR_सङ्ख्या), रिक्त, VV_RO},
    {VV_NAME("t_typealias",	 VAR_सङ्ख्या), रिक्त, VV_RO},
    {VV_NAME("t_enum",		 VAR_सङ्ख्या), रिक्त, VV_RO},
    {VV_NAME("t_enumvalue",	 VAR_सङ्ख्या), रिक्त, VV_RO},
    {VV_NAME("stacktrace",	 VAR_LIST), &t_list_dict_any, VV_RO},
    {VV_NAME("t_tuple",		 VAR_सङ्ख्या), रिक्त, VV_RO},
    {VV_NAME("wayland_display",  VAR_STRING), रिक्त, VV_RO},
    {VV_NAME("clipmethod",	 VAR_STRING), रिक्त, VV_RO},
    {VV_NAME("termda1",		 VAR_STRING), रिक्त, VV_RO},
    {VV_NAME("termosc",	 VAR_STRING), रिक्त, VV_RO},
    {VV_NAME("vim_did_init",	 VAR_सङ्ख्या), रिक्त, VV_RO},
    {VV_NAME("clipproviders",	 VAR_DICT), रिक्त, VV_RO},
    {VV_NAME("imagebackend",	 VAR_STRING), रिक्त, VV_RO}
};

// shorthand
#define vv_tv_type	vv_di.di_tv.v_type
#define vv_nr		vv_di.di_tv.vval.v_सङ्ख्या
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

    अत्थाय (i = 0; i < VV_LEN; ++i)
    {
	p = &vimvars[i];
	सचे (p->vv_flags & VV_RO)
	    p->vv_di.di_flags = DI_FLAGS_RO | DI_FLAGS_FIX;
	अञ्ञथा सचे (p->vv_flags & VV_RO_SBX)
	    p->vv_di.di_flags = DI_FLAGS_RO_SBX | DI_FLAGS_FIX;
	अञ्ञथा
	    p->vv_di.di_flags = DI_FLAGS_FIX;

	// add to v: scope dict, unless the value is not always available
	सचे (p->vv_tv_type != VAR_UNKNOWN)
	    hash_add(&vimvarht, p->vv_di.di_कुञ्चिका, "initialization");
	सचे (p->vv_flags & VV_COMPAT)
	    // add to compat scope dict
	    hash_add(&compat_hashtab, p->vv_di.di_कुञ्चिका, "initialization");
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

    set_vim_var_nr(VV_TYPE_सङ्ख्या,  VAR_TYPE_सङ्ख्या);
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

#सचे defined(EXITFREE)
/*
 * Free all vim variables information on exit
 */
    शून्य
evalvars_clear(शून्य)
{
    पूर्णाङ्क		    i;
    संरचना vimvar   *p;

    अत्थाय (i = 0; i < VV_LEN; ++i)
    {
	p = &vimvars[i];
	सचे (p->vv_di.di_tv.v_type == VAR_STRING)
	    VIM_CLEAR(p->vv_str);
	अञ्ञथा सचे (p->vv_di.di_tv.v_type == VAR_LIST)
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
    अत्थाय (i = 1; i <= script_वस्तूनि.ga_len; ++i)
	vars_clear(&SCRIPT_VARS(i));
}
#endif

    पूर्णाङ्क
garbage_collect_globvars(पूर्णाङ्क copyID)
{
    पटिनिवत्त set_ref_in_ht(&globvarht, copyID, रिक्त, रिक्त);
}

    पूर्णाङ्क
garbage_collect_vimvars(पूर्णाङ्क copyID)
{
    पटिनिवत्त set_ref_in_ht(&vimvarht, copyID, रिक्त, रिक्त);
}

    पूर्णाङ्क
garbage_collect_scriptvars(पूर्णाङ्क copyID)
{
    पूर्णाङ्क		    i;
    पूर्णाङ्क		    idx;
    पूर्णाङ्क		    abort = FALSE;
    scriptitem_T    *si;

    अत्थाय (i = 1; i <= script_वस्तूनि.ga_len; ++i)
    {
	abort = abort || set_ref_in_ht(&SCRIPT_VARS(i), copyID, रिक्त, रिक्त);

	si = SCRIPT_ITEM(i);
	अत्थाय (idx = 0; idx < si->sn_var_vals.ga_len; ++idx)
	{
	    svar_T    *sv = ((svar_T *)si->sn_var_vals.ga_data) + idx;

	    सचे (sv->sv_name != रिक्त)
		abort = abort || set_ref_in_item(sv->sv_tv, copyID, रिक्त, रिक्त, रिक्त);
	}
    }

    पटिनिवत्त abort;
}

/*
 * Set an internal variable to a string value. Creates the variable if it does
 * not already exist.
 */
    शून्य
set_internal_string_var(char_u *name, char_u *मानम्)
{
    char_u	*val;
    typval_T	*tvp;

    val = vim_strsave(मानम्);
    सचे (val == रिक्त)
	पटिनिवत्त;

    tvp = alloc_string_tv(val);
    सचे (tvp == रिक्त)
	पटिनिवत्त;

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
    सचे (ctx != रिक्त)
	current_sctx = *ctx;

    सचे (eval_to_bool(p_ccv, &err, रिक्त, FALSE, TRUE))
	err = TRUE;

    set_vim_var_string(VV_CC_FROM, रिक्त, -1);
    set_vim_var_string(VV_CC_TO, रिक्त, -1);
    set_vim_var_string(VV_FNAME_IN, रिक्त, -1);
    set_vim_var_string(VV_FNAME_OUT, रिक्त, -1);
    current_sctx = saved_sctx;

    सचे (err)
	पटिनिवत्त FAIL;
    पटिनिवत्त OK;
}

#सचे defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
    पूर्णाङ्क
eval_printexpr(char_u *fname, char_u *args)
{
    पूर्णाङ्क		err = FALSE;
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;

    set_vim_var_string(VV_FNAME_IN, fname, -1);
    set_vim_var_string(VV_CMDARG, args, -1);
    ctx = get_option_sctx("printexpr");
    सचे (ctx != रिक्त)
	current_sctx = *ctx;

    सचे (eval_to_bool(p_pexpr, &err, रिक्त, FALSE, TRUE))
	err = TRUE;

    set_vim_var_string(VV_FNAME_IN, रिक्त, -1);
    set_vim_var_string(VV_CMDARG, रिक्त, -1);
    current_sctx = saved_sctx;

    सचे (err)
    {
	mch_remove(fname);
	पटिनिवत्त FAIL;
    }
    पटिनिवत्त OK;
}
#endif

#सचे defined(FEAT_DIFF)
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
    सचे (ctx != रिक्त)
	current_sctx = *ctx;

    // errors are ignored
    tv = eval_व्यञ्जकम्_ext(p_dex, रिक्त, TRUE);
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
    सचे (ctx != रिक्त)
	current_sctx = *ctx;

    // errors are ignored
    tv = eval_व्यञ्जकम्_ext(p_pex, रिक्त, TRUE);
    free_tv(tv);

    set_vim_var_string(VV_FNAME_IN, रिक्त, -1);
    set_vim_var_string(VV_FNAME_DIFF, रिक्त, -1);
    set_vim_var_string(VV_FNAME_OUT, रिक्त, -1);
    current_sctx = saved_sctx;
}
#endif

#सचे defined(FEAT_SPELL)
/*
 * Evaluate an expression to a list with suggestions.
 * For the "expr:" part of 'spellsuggest'.
 * Returns NULL when there is an error.
 */
    list_T *
eval_spell_व्यञ्जकम्(char_u *badword, char_u *व्यञ्जकम्)
{
    typval_T	save_val;
    typval_T	rettv;
    list_T	*list = रिक्त;
    char_u	*p = skipwhite(व्यञ्जकम्);
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;
    पूर्णाङ्क		r;

    // Set "v:val" to the bad word.
    prepare_vimvar(VV_VAL, &save_val);
    set_vim_var_string(VV_VAL, badword, -1);
    सचे (p_verbose == 0)
	++emsg_off;
    ctx = get_option_sctx("spellsuggest");
    सचे (ctx != रिक्त)
	current_sctx = *ctx;

    r = may_call_simple_func(p, &rettv);
    सचे (r == NOTDONE)
	r = eval1(&p, &rettv, &EVALARG_EVALUATE);
    सचे (r == OK)
    {
	सचे (rettv.v_type != VAR_LIST)
	    clear_tv(&rettv);
	अञ्ञथा
	    list = rettv.vval.v_list;
    }

    सचे (p_verbose == 0)
	--emsg_off;
    clear_tv(get_vim_var_tv(VV_VAL));
    restore_vimvar(VV_VAL, &save_val);
    current_sctx = saved_sctx;

    पटिनिवत्त list;
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
    सचे (li == रिक्त)
	पटिनिवत्त -1;
    *pp = tv_get_string(&li->li_tv);

    li = li->li_next;
    सचे (li == रिक्त)
	पटिनिवत्त -1;
    पटिनिवत्त (पूर्णाङ्क)tv_get_सङ्ख्या(&li->li_tv);
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
    सचे (vimvars[idx].vv_tv_type == VAR_UNKNOWN)
	hash_add(&vimvarht, vimvars[idx].vv_di.di_कुञ्चिका, "prepare vimvar");
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
    सचे (vimvars[idx].vv_tv_type != VAR_UNKNOWN)
	पटिनिवत्त;

    hi = hash_find(&vimvarht, vimvars[idx].vv_di.di_कुञ्चिका);
    सचे (HASHITEM_EMPTY(hi))
	internal_error("restore_vimvar()");
    अञ्ञथा
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
    सचे (SCRIPT_ID_VALID(current_sctx.sc_sid))
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
    पटिनिवत्त vim_strchr((char_u *)"gwbt", name[0]) != रिक्त
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
eval_one_व्यञ्जकम्_in_str(char_u *p, garray_T *gap, पूर्णाङ्क evaluate)
{
    char_u	*block_start = skipwhite(p + 1);  // skip the opening {
    char_u	*block_end = block_start;
    char_u	*व्यञ्जकम्_val;

    सचे (*block_start == NUL)
    {
	semsg(_(e_missing_close_curly_str), p);
	पटिनिवत्त रिक्त;
    }
    सचे (skip_व्यञ्जकम्(&block_end, रिक्त) == FAIL)
	पटिनिवत्त रिक्त;
    block_end = skipwhite(block_end);
    सचे (*block_end != '}')
    {
	semsg(_(e_missing_close_curly_str), p);
	पटिनिवत्त रिक्त;
    }
    सचे (evaluate)
    {
	*block_end = NUL;
	व्यञ्जकम्_val = eval_to_string(block_start, FALSE, FALSE);
	*block_end = '}';
	सचे (व्यञ्जकम्_val == रिक्त)
	    पटिनिवत्त रिक्त;
	ga_concat(gap, व्यञ्जकम्_val);
	vim_free(व्यञ्जकम्_val);
    }

    पटिनिवत्त block_end + 1;
}

/*
 * Evaluate all the Vim expressions {expr} in "str" and return the resulting
 * string in allocated memory.  "{{" is reduced to "{" and "}}" to "}".
 * Used for a heredoc assignment.
 * Returns NULL for an error.
 */
    स्थिर char_u *
eval_all_व्यञ्जकम्_in_str(char_u *str)
{
    garray_T	ga;
    char_u	*p;

    ga_init2(&ga, 1, 80);
    p = str;

    याव (*p != NUL)
    {
	char_u	*lit_start;
	पूर्णाङ्क	escaped_brace = FALSE;

	// Look for a block start.
	lit_start = p;
	याव (*p != '{' && *p != '}' && *p != NUL)
	    ++p;

	सचे (*p != NUL && *p == p[1])
	{
	    // Escaped brace, unescape and continue.
	    // Include the brace in the literal string.
	    ++p;
	    escaped_brace = TRUE;
	}
	अञ्ञथा सचे (*p == '}')
	{
	    semsg(_(e_stray_closing_curly_str), str);
	    ga_clear(&ga);
	    पटिनिवत्त रिक्त;
	}

	// Append the literal part.
	ga_concat_len(&ga, lit_start, (size_t)(p - lit_start));

	सचे (*p == NUL)
	    भङ्ग;

	सचे (escaped_brace)
	{
	    // Skip the second brace.
	    ++p;
	    अनुवर्त;
	}

	// Evaluate the expression and append the result.
	p = eval_one_व्यञ्जकम्_in_str(p, &ga, TRUE);
	सचे (p == रिक्त)
	{
	    ga_clear(&ga);
	    पटिनिवत्त रिक्त;
	}
    }
    ga_append(&ga, NUL);

    पटिनिवत्त ga.ga_data;
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
    पूर्णाङ्क		गणनम् = 0;
    पूर्णाङ्क		heredoc_in_string = FALSE;
    char_u	*पङ्क्तिः_arg = रिक्त;
    char_u	*nl_ptr = vim_strchr(cmd, '\n');

    सचे (nl_ptr != रिक्त)
    {
	heredoc_in_string = TRUE;
	पङ्क्तिः_arg = nl_ptr + 1;
	*nl_ptr = NUL;
    }
    अञ्ञथा सचे (eap->ea_getline == रिक्त)
    {
	emsg(_(e_cannot_use_heredoc_here));
	पटिनिवत्त रिक्त;
    }

    // Check for the optional 'trim' word before the marker
    cmd = skipwhite(cmd);

    याव (TRUE)
    {
	सचे (STRNCMP(cmd, "trim", 4) == 0
		&& (cmd[4] == NUL || VIM_ISWHITE(cmd[4])))
	{
	    cmd = skipwhite(cmd + 4);

	    // Trim the indentation from all the lines in the here document.
	    // The amount of indentation trimmed is the same as the indentation
	    // of the first line after the :let command line.  To find the end
	    // marker the indent of the :let command line is trimmed.
	    p = *eap->cmdlinep;
	    याव (VIM_ISWHITE(*p))
	    {
		p++;
		marker_indent_len++;
	    }
	    text_indent_len = -1;

	    अनुवर्त;
	}
	सचे (STRNCMP(cmd, "eval", 4) == 0
		&& (cmd[4] == NUL || VIM_ISWHITE(cmd[4])))
	{
	    cmd = skipwhite(cmd + 4);
	    evalstr = TRUE;
	    अनुवर्त;
	}
	भङ्ग;
    }

    // The marker is the next word.
    सचे (*cmd != NUL && *cmd != comment_char)
    {
	marker = skipwhite(cmd);
	p = skiptowhite(marker);
	सचे (*skipwhite(p) != NUL && *skipwhite(p) != comment_char)
	{
	    semsg(_(e_trailing_characters_str), p);
	    पटिनिवत्त रिक्त;
	}
	*p = NUL;
	सचे (!script_get && vim_islower(*marker))
	{
	    emsg(_(e_marker_cannot_start_with_lower_case_letter));
	    पटिनिवत्त रिक्त;
	}
    }
    अञ्ञथा
    {
	// When getting lines for an embedded script, if the marker is missing,
	// accept '.' as the marker.
	सचे (script_get)
	    marker = dot;
	अञ्ञथा
	{
	    emsg(_(e_missing_marker));
	    पटिनिवत्त रिक्त;
	}
    }

    l = list_alloc();
    सचे (l == रिक्त)
	पटिनिवत्त रिक्त;

    अत्थाय (;;)
    {
	पूर्णाङ्क	mi = 0;
	पूर्णाङ्क	ti = 0;

	सचे (heredoc_in_string)
	{
	    char_u	*next_पङ्क्तिः;

	    // heredoc in a string separated by newlines.  Get the next line
	    // from the string.

	    सचे (*पङ्क्तिः_arg == NUL)
	    {
		semsg(_(e_missing_end_marker_str), marker);
		भङ्ग;
	    }

	    theline = पङ्क्तिः_arg;
	    next_पङ्क्तिः = vim_strchr(theline, '\n');
	    सचे (next_पङ्क्तिः == रिक्त)
		पङ्क्तिः_arg += STRLEN(पङ्क्तिः_arg);
	    अञ्ञथा
	    {
		*next_पङ्क्तिः = NUL;
		पङ्क्तिः_arg = next_पङ्क्तिः + 1;
	    }
	}
	अञ्ञथा
	{
	    vim_free(theline);
	    theline = eap->ea_getline(NUL, eap->cookie, 0, FALSE);
	    सचे (theline == रिक्त)
	    {
		semsg(_(e_missing_end_marker_str), marker);
		भङ्ग;
	    }
	}

	// with "trim": skip the indent matching the :let line to find the
	// marker
	सचे (marker_indent_len > 0
		&& STRNCMP(theline, *eap->cmdlinep, marker_indent_len) == 0)
	    mi = marker_indent_len;
	सचे (STRCMP(marker, theline + mi) == 0)
	    भङ्ग;

	// If expression evaluation failed in the heredoc, then skip till the
	// end marker.
	सचे (eval_failed)
	    अनुवर्त;

	सचे (text_indent_len == -1 && *theline != NUL)
	{
	    // set the text indent from the first line.
	    p = theline;
	    text_indent_len = 0;
	    याव (VIM_ISWHITE(*p))
	    {
		p++;
		text_indent_len++;
	    }
	    text_indent = vim_strnsave(theline, text_indent_len);
	}
	// with "trim": skip the indent matching the first line
	सचे (text_indent != रिक्त)
	    अत्थाय (ti = 0; ti < text_indent_len; ++ti)
		सचे (theline[ti] != text_indent[ti])
		    भङ्ग;

	str = theline + ti;
	सचे (vim9compile)
	{
	    सचे (compile_all_व्यञ्जकम्_in_str(str, evalstr, cctx) == FAIL)
	    {
		vim_free(theline);
		vim_free(text_indent);
		list_free(l);
		पटिनिवत्त FAIL;
	    }
	    गणनम्++;
	}
	अञ्ञथा
	{
	    पूर्णाङ्क	    free_str = FALSE;

	    सचे (evalstr && !eap->skip)
	    {
		str = eval_all_व्यञ्जकम्_in_str(str);
		सचे (str == रिक्त)
		{
		    // expression evaluation failed
		    eval_failed = TRUE;
		    अनुवर्त;
		}
		free_str = TRUE;
	    }

	    सचे (list_append_string(l, str, -1) == FAIL)
	    {
		सचे (free_str)
		    vim_free(str);
		भङ्ग;
	    }
	    सचे (free_str)
		vim_free(str);
	}
    }
    सचे (heredoc_in_string)
	// Next command follows the heredoc in the string.
	eap->nextcmd = पङ्क्तिः_arg;
    अञ्ञथा
	vim_free(theline);
    vim_free(text_indent);

    सचे (vim9compile && cctx->ctx_skip != SKIP_YES && !eval_failed)
	generate_NEWLIST(cctx, गणनम्, FALSE);

    सचे (eval_failed)
    {
	// expression evaluation in the heredoc failed
	list_free(l);
	पटिनिवत्त रिक्त;
    }
    पटिनिवत्त l;
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

    सचे (!in_vim9script())
    {
	semsg(_(e_str_cannot_be_used_in_legacy_vim_script), ":var");
	पटिनिवत्त;
    }
    has_var = checkforcmd_noparen(&p, "var", 3);
    सचे (current_sctx.sc_sid == 0 && has_var)
    {
	emsg(_(e_cannot_declare_variable_on_command_पङ्क्तिः));
	पटिनिवत्त;
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
    char_u	*व्यञ्जकम् = रिक्त;
    typval_T	rettv;
    पूर्णाङ्क		var_गणनम् = 0;
    पूर्णाङ्क		semicolon = 0;
    char_u	op[4];
    char_u	*argend;
    पूर्णाङ्क		first = TRUE;
    पूर्णाङ्क		concat;
    पूर्णाङ्क		has_assign;
    पूर्णाङ्क		flags = 0;
    पूर्णाङ्क		vim9script = in_vim9script();

    सचे (eap->cmdidx == CMD_final && !vim9script)
    {
	// In legacy Vim script ":final" is short for ":finally".
	ex_finally(eap);
	पटिनिवत्त;
    }
    सचे (eap->cmdidx == CMD_let && vim9script)
    {
	emsg(_(e_cannot_use_let_in_vim9_script));
	पटिनिवत्त;
    }

    सचे (eap->cmdidx == CMD_const)
	flags |= ASSIGN_CONST;
    अञ्ञथा सचे (eap->cmdidx == CMD_final)
	flags |= ASSIGN_FINAL;

    // Vim9 assignment without ":let", ":const" or ":final"
    सचे (eap->arg == eap->cmd)
	flags |= ASSIGN_NO_DECL;

    argend = skip_var_list(arg, TRUE, &var_गणनम्, &semicolon, FALSE);
    सचे (argend == रिक्त)
	पटिनिवत्त;
    व्यञ्जकम् = skipwhite(argend);
    concat = व्यञ्जकम्[0] == '.'
	&& ((व्यञ्जकम्[1] == '=' && in_old_script(2))
		|| (व्यञ्जकम्[1] == '.' && व्यञ्जकम्[2] == '='));
    has_assign =  *व्यञ्जकम् == '=' || (vim_strchr((char_u *)"+-*/%", *व्यञ्जकम्) != रिक्त
							    && व्यञ्जकम्[1] == '=');
    सचे (!has_assign && !concat)
    {
	// ":let" without "=": list variables
	सचे (*arg == '[')
	    emsg(_(e_invalid_argument));
	अञ्ञथा सचे (व्यञ्जकम्[0] == '.' && व्यञ्जकम्[1] == '=')
	    emsg(_(e_dot_equal_not_supported_with_script_version_two));
	अञ्ञथा सचे (!ends_excmd2(eap->cmd, arg))
	{
	    सचे (vim9script)
	    {
		सचे (!ends_excmd2(eap->cmd, skipwhite(argend)))
		    semsg(_(e_trailing_characters_str), argend);
		अञ्ञथा
		    // Vim9 declaration ":var name: type"
		    arg = vim9_declare_scriptvar(eap, arg);
	    }
	    अञ्ञथा
	    {
		// ":let var1 var2" - list values
		arg = list_arg_vars(eap, arg, &first);
	    }
	}
	अञ्ञथा सचे (!eap->skip)
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
	पटिनिवत्त;
    }

    सचे (व्यञ्जकम्[0] == '=' && व्यञ्जकम्[1] == '<' && व्यञ्जकम्[2] == '<')
    {
	list_T	*l = रिक्त;
	long	cur_lnum = SOURCING_LNUM;

	// :let text =<< [trim] [eval] END
	// :var text =<< [trim] [eval] END
	सचे (vim9script && !eap->skip && (!VIM_ISWHITE(व्यञ्जकम्[-1])
						 || !IS_WHITE_OR_NUL(व्यञ्जकम्[3])))
	    semsg(_(e_white_space_required_before_and_after_str_at_str),
								  "=<<", व्यञ्जकम्);
	अञ्ञथा
	    l = heredoc_get(eap, व्यञ्जकम् + 3, FALSE, FALSE);

	सचे (l != रिक्त)
	{
	    rettv_list_set(&rettv, l);
	    सचे (!eap->skip)
	    {
		// errors are for the assignment, not the end marker
		SOURCING_LNUM = cur_lnum;
		op[0] = '=';
		op[1] = NUL;
		(शून्य)ex_let_vars(eap->arg, &rettv, FALSE, semicolon, var_गणनम्,
								flags, op);
	    }
	    clear_tv(&rettv);
	}
	पटिनिवत्त;
    }

    evalarg_T   evalarg;
    पूर्णाङ्क	    len = 1;

    CLEAR_FIELD(rettv);

    पूर्णाङ्क cur_lnum;

    op[0] = '=';
    op[1] = NUL;
    सचे (*व्यञ्जकम् != '=')
    {
	सचे (vim9script && (flags & ASSIGN_NO_DECL) == 0)
	{
	    // +=, /=, etc. require an existing variable
	    semsg(_(e_cannot_use_operator_on_new_variable_str), eap->arg);
	}
	अञ्ञथा सचे (vim_strchr((char_u *)"+-*/%.", *व्यञ्जकम्) != रिक्त)
	{
	    op[0] = *व्यञ्जकम्;   // +=, -=, *=, /=, %= or .=
	    ++len;
	    सचे (व्यञ्जकम्[0] == '.' && व्यञ्जकम्[1] == '.') // ..=
	    {
		++व्यञ्जकम्;
		++len;
	    }
	}
	व्यञ्जकम् += 2;
    }
    अञ्ञथा
	++व्यञ्जकम्;

    सचे (vim9script && !eap->skip && (!VIM_ISWHITE(*argend)
					       || !IS_WHITE_OR_NUL(*व्यञ्जकम्)))
    {
	vim_strncpy(op, व्यञ्जकम् - len, len);
	semsg(_(e_white_space_required_before_and_after_str_at_str),
							       op, argend);
    }

    सचे (eap->skip)
	++emsg_skip;
    fill_evalarg_from_eap(&evalarg, eap, eap->skip);
    व्यञ्जकम् = skipwhite_and_linebreak(व्यञ्जकम्, &evalarg);
    cur_lnum = SOURCING_LNUM;
    पूर्णाङ्क eval_res = eval0(व्यञ्जकम्, &rettv, eap, &evalarg);
    सचे (eap->skip)
	--emsg_skip;
    clear_evalarg(&evalarg, eap);

    // Restore the line number so that any type error is given for the
    // declaration, not the expression.
    SOURCING_LNUM = cur_lnum;

    सचे (!eap->skip && eval_res != FAIL)
	(शून्य)ex_let_vars(eap->arg, &rettv, FALSE, semicolon, var_गणनम्,
								    flags, op);
    सचे (eval_res != FAIL)
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
    पूर्णाङ्क		var_गणनम्,	// from skip_var_list()
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

    सचे (tv->v_type == VAR_VOID)
    {
	emsg(_(e_cannot_use_void_मानम्));
	पटिनिवत्त FAIL;
    }
    सचे (*arg != '[')
    {
	// ":let var = expr" or ":for var in list"
	सचे (ex_let_one(arg, tv, copy, flags, op, op, var_idx) == रिक्त)
	    पटिनिवत्त FAIL;
	पटिनिवत्त OK;
    }

    // ":let [v1, v2] = list" or ":for [v1, v2] in listlist"
    // or
    // ":let [v1, v2] = tuple" or ":for [v1, v2] in tupletuple"
    सचे (tv->v_type != VAR_LIST && tv->v_type != VAR_TUPLE)
    {
	emsg(_(e_list_or_tuple_required));
	पटिनिवत्त FAIL;
    }
    सचे (is_list)
    {
	l = tv->vval.v_list;
	सचे (l == रिक्त)
	{
	    emsg(_(e_list_required));
	    पटिनिवत्त FAIL;
	}
	i = list_len(l);
    }
    अञ्ञथा
    {
	tuple = tv->vval.v_tuple;
	सचे (tuple == रिक्त)
	{
	    emsg(_(e_tuple_required));
	    पटिनिवत्त FAIL;
	}
	i = tuple_len(tuple);
    }

    सचे (semicolon == 0 && var_गणनम् < i)
    {
	emsg(_(is_list ? e_less_targets_than_list_वस्तूनि
					: e_less_targets_than_tuple_वस्तूनि));
	पटिनिवत्त FAIL;
    }
    सचे (var_गणनम् - semicolon > i)
    {
	emsg(_(is_list ? e_more_targets_than_list_वस्तूनि
					: e_more_targets_than_tuple_वस्तूनि));
	पटिनिवत्त FAIL;
    }

    सचे (is_list)
    {
	CHECK_LIST_MATERIALIZE(l);
	item = l->lv_first;
    }
    अञ्ञथा
	idx = 0;

    याव (*arg != ']')
    {
	arg = skipwhite(arg + 1);
	++var_idx;
	arg = ex_let_one(arg, is_list ? &item->li_tv : TUPLE_ITEM(tuple, idx),
			 TRUE, flags | ASSIGN_UNPACK, (char_u *)",;]", op,
			 var_idx);
	सचे (is_list)
	    item = item->li_next;
	अञ्ञथा
	    idx++;
	सचे (arg == रिक्त)
	    पटिनिवत्त FAIL;

	arg = skipwhite(arg);
	सचे (*arg == ';')
	{
	    // Put the rest of the list or tuple (may be empty) in the var
	    // after ';'.  Create a new list or tuple for this.
	    सचे (is_list)
	    {
		// Put the rest of the list (may be empty) in the var
		// after ';'.  Create a new list for this.
		l = list_alloc();
		सचे (l == रिक्त)
		    पटिनिवत्त FAIL;

		// list
		याव (item != रिक्त)
		{
		    list_append_tv(l, &item->li_tv);
		    item = item->li_next;
		}

		ltv.v_type = VAR_LIST;
		ltv.v_lock = 0;
		ltv.vval.v_list = l;
		l->lv_refcount = 1;
	    }
	    अञ्ञथा
	    {
		tuple_T	*new_tuple = tuple_alloc();
		सचे (new_tuple == रिक्त)
		    पटिनिवत्त FAIL;

		// Put the rest of the tuple (may be empty) in the var
		// after ';'.  Create a new tuple for this.
		याव (idx < TUPLE_LEN(tuple))
		{
		    typval_T    new_tv;

		    copy_tv(TUPLE_ITEM(tuple, idx), &new_tv);
		    सचे (tuple_append_tv(new_tuple, &new_tv) == FAIL)
		    {
			clear_tv(&new_tv);
			tuple_unref(new_tuple);
			पटिनिवत्त FAIL;
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
	    सचे (arg == रिक्त)
		पटिनिवत्त FAIL;
	    भङ्ग;
	}
	अञ्ञथा सचे (*arg != ',' && *arg != ']')
	{
	    internal_error("ex_let_vars()");
	    पटिनिवत्त FAIL;
	}
    }

    पटिनिवत्त OK;
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
    पूर्णाङ्क		*var_गणनम्,
    पूर्णाङ्क		*semicolon,
    पूर्णाङ्क		silent)
{
    char_u	*p, *s;

    सचे (*arg == '[')
    {
	// "[var, var]": find the matching ']'.
	p = arg;
	अत्थाय (;;)
	{
	    p = skipwhite(p + 1);	// skip whites after '[', ';' or ','
	    s = skip_var_one(p, include_type);
	    सचे (s == p)
	    {
		सचे (!silent)
		    semsg(_(e_invalid_argument_str), p);
		पटिनिवत्त रिक्त;
	    }
	    ++*var_गणनम्;

	    p = skipwhite(s);
	    सचे (*p == ']')
		भङ्ग;
	    अञ्ञथा सचे (*p == ';')
	    {
		सचे (*semicolon == 1)
		{
		    सचे (!silent)
			emsg(_(e_double_semicolon_in_list_of_variables));
		    पटिनिवत्त रिक्त;
		}
		*semicolon = 1;
	    }
	    अञ्ञथा सचे (*p != ',')
	    {
		सचे (!silent)
		    semsg(_(e_invalid_argument_str), p);
		पटिनिवत्त रिक्त;
	    }
	}
	पटिनिवत्त p + 1;
    }

    पटिनिवत्त skip_var_one(arg, include_type);
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

    सचे (*arg == '@' && arg[1] != NUL)
	पटिनिवत्त arg + 2;

    // termcap option name may have non-alpha characters
    सचे (STRNCMP(arg, "&t_", 3) == 0 && arg[3] != NUL && arg[4] != NUL)
	पटिनिवत्त arg + 5;

    end = find_name_end(*arg == '$' || *arg == '&' ? arg + 1 : arg,
				   रिक्त, रिक्त, FNE_INCL_BR | FNE_CHECK_START);

    // "a: type" is declaring variable "a" with a type, not "a:".
    // Same for "s: type".
    सचे (vim9 && end == arg + 2 && end[-1] == ':')
	--end;

    सचे (include_type && vim9)
    {
	सचे (*skipwhite(end) == ':')
	    end = skip_type(skipwhite(skipwhite(end) + 1), FALSE);
    }
    पटिनिवत्त end;
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
    अत्थाय (hi = ht->ht_array; todo > 0 && !got_int; ++hi)
    {
	सचे (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    di = HI2DI(hi);

	    // apply :filter /pat/ to variable name
	    vim_strncpy((char_u *)buf, (char_u *)prefix, IOSIZE - 1);
	    vim_strcat((char_u *)buf, di->di_कुञ्चिका, IOSIZE);
	    सचे (message_filtered(buf))
		अनुवर्त;

	    सचे (empty || di->di_tv.v_type != VAR_STRING
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

    याव (!ends_excmd2(eap->cmd, arg) && !got_int)
    {
	सचे (error || eap->skip)
	{
	    arg = find_name_end(arg, रिक्त, रिक्त, FNE_INCL_BR | FNE_CHECK_START);
	    सचे (!VIM_ISWHITE(*arg) && !ends_excmd(*arg))
	    {
		emsg_severe = TRUE;
		सचे (!did_emsg)
		    semsg(_(e_trailing_characters_str), arg);
		भङ्ग;
	    }
	}
	अञ्ञथा
	{
	    // get_name_len() takes care of expanding curly braces
	    name_start = name = arg;
	    len = get_name_len(&arg, &tofree, TRUE, TRUE);
	    सचे (len <= 0)
	    {
		// This is mainly to keep test 49 working: when expanding
		// curly braces fails overrule the exception error message.
		सचे (len < 0 && !aborting())
		{
		    emsg_severe = TRUE;
		    semsg(_(e_invalid_argument_str), arg);
		    भङ्ग;
		}
		error = TRUE;
	    }
	    अञ्ञथा
	    {
		arg = skipwhite(arg);
		सचे (tofree != रिक्त)
		    name = tofree;
		सचे (eval_variable(name, len, 0, &tv, रिक्त,
						     EVAL_VAR_VERBOSE) == FAIL)
		    error = TRUE;
		अञ्ञथा
		{
		    // handle d.key, l[idx], f(expr)
		    arg_subsc = arg;
		    सचे (handle_subscript(&arg, name_start, &tv,
					      &EVALARG_EVALUATE, TRUE) == FAIL)
			error = TRUE;
		    अञ्ञथा
		    {
			सचे (arg == arg_subsc && len == 2 && name[1] == ':')
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
			अञ्ञथा
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

    पटिनिवत्त arg;
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

    सचे ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					 && (flags & ASSIGN_FOR_LOOP) == 0)
    {
	emsg(_(e_cannot_lock_environment_variable));
	पटिनिवत्त रिक्त;
    }

    // Find the end of the name.
    ++arg;
    name = arg;
    len = get_env_len(&arg);
    सचे (len == 0)
	semsg(_(e_invalid_argument_str), name - 1);
    अञ्ञथा
    {
	सचे (op != रिक्त && vim_strchr((char_u *)"+-*/%", *op) != रिक्त)
	    semsg(_(e_wrong_variable_type_for_str_equal), op);
	अञ्ञथा सचे (endchars != रिक्त
			      && vim_strchr(endchars, *skipwhite(arg)) == रिक्त)
	    emsg(_(e_unexpected_characters_in_let));
	अञ्ञथा सचे (!check_secure() && !check_restricted())
	{
	    char_u	*tofree = रिक्त;
	    पूर्णाङ्क		c1 = name[len];
	    char_u	*p;

	    name[len] = NUL;
	    p = tv_get_string_chk(tv);
	    सचे (p != रिक्त && op != रिक्त && *op == '.')
	    {
		पूर्णाङ्क	mustfree = FALSE;
		char_u  *s = vim_getenv(name, &mustfree);

		सचे (s != रिक्त)
		{
		    p = tofree = concat_str(s, p);
		    सचे (mustfree)
			vim_free(s);
		}
	    }
	    सचे (p != रिक्त)
	    {
		vim_setenv_ext(name, p);
		arg_end = arg;
	    }
	    name[len] = c1;
	    vim_free(tofree);
	}
    }
    पटिनिवत्त arg_end;
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

    सचे ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					 && (flags & ASSIGN_FOR_LOOP) == 0)
    {
	emsg(_(e_cannot_lock_option));
	पटिनिवत्त रिक्त;
    }

    // Find the end of the name.
    p = find_option_end(&arg, &scope);
    सचे (p == रिक्त || (endchars != रिक्त
			       && vim_strchr(endchars, *skipwhite(p)) == रिक्त))
    {
	emsg(_(e_unexpected_characters_in_let));
	पटिनिवत्त रिक्त;
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

    opt_type = get_option_मानम्(arg, &numval, &stringval, &opt_p_flags, scope);
    सचे (opt_type == gov_unknown && arg[0] != 't' && arg[1] != '_')
    {
	semsg(_(e_unknown_option_str_2), arg);
	गच्छ theend;
    }
    सचे (op != रिक्त && *op != '='
	&& (((opt_type == gov_bool || opt_type == gov_सङ्ख्या) && *op == '.')
	    || (opt_type == gov_string && *op != '.')))
    {
	semsg(_(e_wrong_variable_type_for_str_equal), op);
	गच्छ theend;
    }

    सचे ((opt_type == gov_bool
		|| opt_type == gov_सङ्ख्या
		|| opt_type == gov_hidden_bool
		|| opt_type == gov_hidden_सङ्ख्या)
		     && (tv->v_type != VAR_STRING || !in_vim9script()))
    {
	सचे (opt_type == gov_bool || opt_type == gov_hidden_bool)
	    // bool, possibly hidden
	    n = (long)tv_get_bool_chk(tv, &failed);
	अञ्ञथा
	    // number, possibly hidden
	    n = (long)tv_get_सङ्ख्या_chk(tv, &failed);
	सचे (failed)
	    गच्छ theend;
    }

    सचे ((opt_p_flags & P_FUNC) && (tv->v_type == VAR_PARTIAL
						    || tv->v_type == VAR_FUNC))
    {
	// If the option can be set to a function reference or a lambda
	// and the passed value is a function reference, then convert it to
	// the name (string) of the function reference.
	s = tv2string(tv, &tofree, numbuf, 0);
	सचे (s == रिक्त)
	    गच्छ theend;
    }
    // Avoid setting a string option to the text "v:false" or similar.
    // In Vim9 script also don't convert a number to string.
    अञ्ञथा सचे (tv->v_type != VAR_BOOL && tv->v_type != VAR_SPECIAL
		     && (!in_vim9script() || tv->v_type != VAR_सङ्ख्या))
    {
	s = tv_get_string_chk(tv);
	सचे (s == रिक्त)
	    गच्छ theend;
    }
    अञ्ञथा सचे (opt_type == gov_string || opt_type == gov_hidden_string)
    {
	emsg(_(e_string_required));
	गच्छ theend;
    }

    सचे (op != रिक्त && *op != '=')
    {
	// number, in legacy script also bool
	सचे (opt_type == gov_सङ्ख्या
				 || (opt_type == gov_bool && !in_vim9script()))
	{
	    विकल्प (*op)
	    {
		पक्ष '+': n = numval + n; भङ्ग;
		पक्ष '-': n = numval - n; भङ्ग;
		पक्ष '*': n = numval * n; भङ्ग;
		पक्ष '/': n = (long)अङ्कः_divide(numval, n, &failed); भङ्ग;
		पक्ष '%': n = (long)अङ्कः_modulus(numval, n, &failed); भङ्ग;
	    }
	    s = रिक्त;
	    सचे (failed)
		गच्छ theend;
	}
	अञ्ञथा सचे (opt_type == gov_string && stringval != रिक्त && s != रिक्त)
	{
	    // string
	    s = concat_str(stringval, s);
	    vim_free(stringval);
	    stringval = s;
	}
    }

    वर्ण *err = set_option_मानम्(arg, n, s, scope);
    arg_end = p;
    सचे (err != रिक्त)
	emsg(_(err));

theend:
    *p = c1;
    vim_free(stringval);
    vim_free(tofree);
    पटिनिवत्त arg_end;
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

    सचे ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					 && (flags & ASSIGN_FOR_LOOP) == 0)
    {
	emsg(_(e_cannot_lock_register));
	पटिनिवत्त रिक्त;
    }
    ++arg;
    सचे (op != रिक्त && vim_strchr((char_u *)"+-*/%", *op) != रिक्त)
	semsg(_(e_wrong_variable_type_for_str_equal), op);
    अञ्ञथा सचे (endchars != रिक्त
			  && vim_strchr(endchars, *skipwhite(arg + 1)) == रिक्त)
	emsg(_(e_unexpected_characters_in_let));
    अञ्ञथा
    {
	char_u	*ptofree = रिक्त;
	char_u	*p;

	p = tv_get_string_chk(tv);
	सचे (p != रिक्त && op != रिक्त && *op == '.')
	{
	    char_u  *s = get_reg_contents(*arg == '@'
						  ? '"' : *arg, GREG_व्यञ्जकम्_SRC);

	    सचे (s != रिक्त)
	    {
		p = ptofree = concat_str(s, p);
		vim_free(s);
	    }
	}
	सचे (p != रिक्त)
	{
	    write_reg_contents(*arg == '@' ? '"' : *arg, p, -1, FALSE);
	    arg_end = arg + 1;
	}
	vim_free(ptofree);
    }
    पटिनिवत्त arg_end;
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

    सचे (in_vim9script() && (flags & (ASSIGN_NO_DECL | ASSIGN_DECL)) == 0
			&& (flags & (ASSIGN_CONST | ASSIGN_FINAL)) == 0
				  && vim_strchr((char_u *)"$@&", *arg) != रिक्त)
    {
	vim9_declare_error(arg);
	पटिनिवत्त रिक्त;
    }

    सचे (check_typval_is_मानम्(tv) == FAIL)
	पटिनिवत्त रिक्त;

    // In Vim9 script an environment variable and a register only take a
    // String.  "@#" also takes a buffer number.
    सचे (in_vim9script() && (*arg == '$' || *arg == '@')
	    && !(*arg == '@' && arg[1] == '#' && tv->v_type == VAR_सङ्ख्या
					       && (op == रिक्त || *op == '=')))
    {
	where_T	where = WHERE_INIT;

	सचे (check_typval_type(&t_string, tv, where) == FAIL)
	    पटिनिवत्त रिक्त;
    }

    सचे (*arg == '$')
    {
	// ":let $VAR = expr": Set environment variable.
	पटिनिवत्त ex_let_env(arg, tv, flags, endchars, op);
    }
    अञ्ञथा सचे (*arg == '&')
    {
	// ":let &option = expr": Set option value.
	// ":let &l:option = expr": Set local option value.
	// ":let &g:option = expr": Set global option value.
	// ":for &ts in range(8)": Set option value for for loop
	पटिनिवत्त ex_let_option(arg, tv, flags, endchars, op);
    }
    अञ्ञथा सचे (*arg == '@')
    {
	// ":let @r = expr": Set register contents.
	पटिनिवत्त ex_let_register(arg, tv, flags, endchars, op);
    }
    अञ्ञथा सचे (eval_isnamec1(*arg) || *arg == '{')
    {
	lval_T	lv;
	char_u	*p;
	पूर्णाङ्क	lval_flags = (flags & (ASSIGN_NO_DECL | ASSIGN_DECL))
							     ? GLV_NO_DECL : 0;
	lval_flags |= (flags & ASSIGN_FOR_LOOP) ? GLV_FOR_LOOP : 0;
	सचे (op != रिक्त && *op != '=')
	    lval_flags |= GLV_ASSIGN_WITH_OP;

	// ":let var = expr": Set internal variable.
	// ":let var: type = expr": Set internal variable with type.
	// ":let {expr} = expr": Idem, name made with curly braces
	p = get_lval(arg, tv, &lv, FALSE, FALSE, lval_flags, FNE_CHECK_START);
	सचे (p != रिक्त && lv.ll_name != रिक्त)
	{
	    सचे (endchars != रिक्त && vim_strchr(endchars,
					   *skipwhite(lv.ll_name_end)) == रिक्त)
	    {
		emsg(_(e_unexpected_characters_in_let));
	    }
	    अञ्ञथा
	    {
		set_var_lval(&lv, p, tv, copy, flags, op, var_idx);
		arg_end = lv.ll_name_end;
	    }
	}
	clear_lval(&lv);
    }
    अञ्ञथा
	semsg(_(e_invalid_argument_str), arg);

    पटिनिवत्त arg_end;
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

    सचे (eap->forceit)
	deep = -1;
    अञ्ञथा सचे (vim_isdigit(*arg))
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

    करोहि
    {
	सचे (*arg == '$')
	{
	    lv.ll_name = arg;
	    lv.ll_tv = रिक्त;
	    ++arg;
	    सचे (get_env_len(&arg) == 0)
	    {
		semsg(_(e_invalid_argument_str), arg - 1);
		पटिनिवत्त;
	    }
	    सचे (!error && !eap->skip
			      && callback(&lv, arg, eap, deep, cookie) == FAIL)
		error = TRUE;
	    name_end = arg;
	}
	अञ्ञथा
	{
	    // Parse the name and find the end.
	    name_end = get_lval(arg, रिक्त, &lv, TRUE, eap->skip || error,
				     glv_flags | GLV_NO_DECL, FNE_CHECK_START);
	    सचे (lv.ll_name == रिक्त)
		error = TRUE;	    // error but continue parsing
	    सचे (name_end == रिक्त || (!VIM_ISWHITE(*name_end)
						    && !ends_excmd(*name_end)))
	    {
		सचे (name_end != रिक्त)
		{
		    emsg_severe = TRUE;
		    semsg(_(e_trailing_characters_str), name_end);
		}
		सचे (!(eap->skip || error))
		    clear_lval(&lv);
		भङ्ग;
	    }

	    सचे (!error && !eap->skip
			 && callback(&lv, name_end, eap, deep, cookie) == FAIL)
		error = TRUE;

	    सचे (!eap->skip)
		clear_lval(&lv);
	}

	arg = skipwhite(name_end);
    } याव (!ends_excmd2(name_end, arg));

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

    सचे (lp->ll_tv == रिक्त)
    {
	cc = *name_end;
	*name_end = NUL;

	// Environment variable, normal name or expanded name.
	सचे (*lp->ll_name == '$')
	    vim_unsetenv_ext(lp->ll_name + 1);
	अञ्ञथा सचे (do_unlet(lp->ll_name, forceit) == FAIL)
	    ret = FAIL;
	*name_end = cc;
    }
    अञ्ञथा सचे ((lp->ll_list != रिक्त
		 && मानम्_check_lock(lp->ll_list->lv_lock, lp->ll_name, FALSE))
	    || (lp->ll_dict != रिक्त
		&& मानम्_check_lock(lp->ll_dict->dv_lock, lp->ll_name, FALSE)))
	पटिनिवत्त FAIL;
    अञ्ञथा सचे (lp->ll_range)
	list_unlet_range(lp->ll_list, lp->ll_li, lp->ll_n1,
						    !lp->ll_empty2, lp->ll_n2);
    अञ्ञथा सचे (lp->ll_list != रिक्त)
	// unlet a List item.
	listitem_remove(lp->ll_list, lp->ll_li);
    अञ्ञथा सचे (lp->ll_dict != रिक्त)
	// unlet a Dictionary item.
	dictitem_remove(lp->ll_dict, lp->ll_di, "unlet");
    अञ्ञथा
    {
	semsg(_(e_cannot_unlet_imported_item_str), lp->ll_name);
	पटिनिवत्त FAIL;
    }

    पटिनिवत्त ret;
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
    याव (li != रिक्त && (!has_n2 || n2 >= n1))
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
    सचे (in_vim9script() && check_vim9_unlet(name) == FAIL)
	पटिनिवत्त FAIL;

    ht = find_var_ht(name, &varname);

    // can't :unlet a script variable in Vim9 script from a function
    सचे (ht == get_script_local_ht()
	    && SCRIPT_ID_VALID(current_sctx.sc_sid)
	    && SCRIPT_ITEM(current_sctx.sc_sid)->sn_version
							 == SCRIPT_VERSION_VIM9
	    && check_vim9_unlet(name) == FAIL)
	पटिनिवत्त FAIL;

    सचे (ht != रिक्त && *varname != NUL)
    {
	d = get_current_funccal_dict(ht);
	सचे (d == रिक्त)
	{
	    सचे (ht == &globvarht)
		d = &globvardict;
	    अञ्ञथा सचे (ht == &compat_hashtab)
		d = &vimvardict;
	    अञ्ञथा
	    {
		di = find_var_in_ht(ht, *name, (char_u *)"", FALSE);
		d = di == रिक्त ? रिक्त : di->di_tv.vval.v_dict;
	    }
	    सचे (d == रिक्त)
	    {
		internal_error("do_unlet()");
		पटिनिवत्त FAIL;
	    }
	}
	hi = hash_find(ht, varname);
	सचे (HASHITEM_EMPTY(hi))
	    hi = find_hi_in_scoped_ht(name, &ht);
	सचे (hi != रिक्त && !HASHITEM_EMPTY(hi))
	{
	    di = HI2DI(hi);
	    सचे (var_check_fixed(di->di_flags, name, FALSE)
		    || var_check_ro(di->di_flags, name, FALSE)
		    || मानम्_check_lock(d->dv_lock, name, FALSE)
		    || check_hashtab_frozen(ht, "unlet"))
		पटिनिवत्त FAIL;

	    delete_var(ht, hi);
	    पटिनिवत्त OK;
	}
    }
    सचे (forceit)
	पटिनिवत्त OK;
    semsg(_(e_no_such_variable_str), name);
    पटिनिवत्त FAIL;
}

    स्थिर शून्य
report_lockvar_member(वर्ण *सन्देशः, lval_T *lp)
{
    पूर्णाङ्क did_alloc = FALSE;
    char_u *vname = (char_u *)"";
    char_u *class_name = lp->ll_class != रिक्त
				    ? lp->ll_class->class_name.string : (char_u *)"";
    सचे (lp->ll_name != रिक्त)
    {
	सचे (lp->ll_name_end == रिक्त)
	    vname = lp->ll_name;
	अञ्ञथा
	{
	    vname = vim_strnsave(lp->ll_name, lp->ll_name_end - lp->ll_name);
	    सचे (vname == रिक्त)
		पटिनिवत्त;
	    did_alloc = TRUE;
	}
    }
    semsg(_(सन्देशः), vname, class_name);
    सचे (did_alloc)
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
    ch_log(रिक्त, "LKVAR: do_lock_var(): name %s, is_root %d", lp->ll_name, lp->ll_is_मूलम्);
#endif

    सचे (lp->ll_tv == रिक्त)
    {
	cc = *name_end;
	*name_end = NUL;
	सचे (*lp->ll_name == '$')
	{
	    semsg(_(e_cannot_lock_or_unlock_variable_str), lp->ll_name);
	    ret = FAIL;
	}
	अञ्ञथा
	{
	    // Normal name or expanded name.
	    di = find_var(lp->ll_name, रिक्त, TRUE);
	    सचे (di == रिक्त)
	    {
		सचे (in_vim9script())
		    semsg(_(e_cannot_find_variable_to_unlock_str),
								  lp->ll_name);
		ret = FAIL;
	    }
	    अञ्ञथा
	    {
		सचे ((di->di_flags & DI_FLAGS_FIX)
			    && di->di_tv.v_type != VAR_DICT
			    && di->di_tv.v_type != VAR_LIST)
		{
		    // For historic reasons this error is not given for a list
		    // or dict.  E.g., the b: dict could be locked/unlocked.
		    semsg(_(e_cannot_lock_or_unlock_variable_str), lp->ll_name);
		    ret = FAIL;
		}
		अञ्ञथा
		{
		    सचे (in_vim9script())
		    {
			svar_T  *sv = find_typval_in_script(&di->di_tv,
								     0, FALSE);

			सचे (sv != रिक्त && sv->sv_const != 0)
			{
			    semsg(_(e_cannot_change_readonly_variable_str),
								  lp->ll_name);
			    ret = FAIL;
			}
		    }

		    सचे (ret == OK)
		    {
			सचे (lock)
			    di->di_flags |= DI_FLAGS_LOCK;
			अञ्ञथा
			    di->di_flags &= ~DI_FLAGS_LOCK;
			सचे (deep != 0)
			    item_lock(&di->di_tv, deep, lock, FALSE);
		    }
		}
	    }
	}
	*name_end = cc;
    }
    अञ्ञथा सचे (deep == 0 && lp->ll_object == रिक्त && lp->ll_class == रिक्त)
    {
	// nothing to do
    }
    अञ्ञथा सचे (lp->ll_is_मूलम्)
	// (un)lock the item.
	item_lock(lp->ll_tv, deep, lock, FALSE);
    अञ्ञथा सचे (lp->ll_range)
    {
	listitem_T    *li = lp->ll_li;

	// (un)lock a range of List items.
	याव (li != रिक्त && (lp->ll_empty2 || lp->ll_n2 >= lp->ll_n1))
	{
	    item_lock(&li->li_tv, deep, lock, FALSE);
	    li = li->li_next;
	    ++lp->ll_n1;
	}
    }
    अञ्ञथा सचे (lp->ll_list != रिक्त)
	// (un)lock a List item.
	item_lock(&lp->ll_li->li_tv, deep, lock, FALSE);
    अञ्ञथा सचे (lp->ll_object != रिक्त)  // This check must be before ll_class.
    {
	// (un)lock an object variable.
	report_lockvar_member(e_cannot_lock_object_variable_str, lp);
	ret = FAIL;
    }
    अञ्ञथा सचे (lp->ll_class != रिक्त)
    {
	// (un)lock a class variable.
	report_lockvar_member(e_cannot_lock_class_variable_str, lp);
	ret = FAIL;
    }
    अञ्ञथा
    {
	// (un)lock a Dictionary item.
	सचे (lp->ll_di == रिक्त)
	{
	    emsg(_(e_dictionary_required));
	    ret = FAIL;
	}
	अञ्ञथा
	    item_lock(&lp->ll_di->di_tv, deep, lock, FALSE);
    }

    पटिनिवत्त ret;
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

    सचे (recurse >= DICT_MAXNEST)
    {
	emsg(_(e_variable_nested_too_deep_for_unlock));
	पटिनिवत्त;
    }
    सचे (deep == 0)
	पटिनिवत्त;
    ++recurse;

    // lock/unlock the item itself
    सचे (lock)
	tv->v_lock |= VAR_LOCKED;
    अञ्ञथा
	tv->v_lock &= ~VAR_LOCKED;

    विकल्प (tv->v_type)
    {
	पक्ष VAR_UNKNOWN:
	पक्ष VAR_ANY:
	पक्ष VAR_VOID:
	पक्ष VAR_सङ्ख्या:
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
	    सचे ((b = tv->vval.v_blob) != रिक्त
				    && !(check_refcount && b->bv_refcount > 1))
	    {
		सचे (lock)
		    b->bv_lock |= VAR_LOCKED;
		अञ्ञथा
		    b->bv_lock &= ~VAR_LOCKED;
	    }
	    भङ्ग;
	पक्ष VAR_LIST:
	    सचे ((l = tv->vval.v_list) != रिक्त
				    && !(check_refcount && l->lv_refcount > 1))
	    {
		सचे (lock)
		    l->lv_lock |= VAR_LOCKED;
		अञ्ञथा
		    l->lv_lock &= ~VAR_LOCKED;
		सचे (deep < 0 || deep > 1)
		{
		    सचे (l->lv_first == &range_list_item)
			l->lv_lock |= VAR_वस्तूनि_LOCKED;
		    अञ्ञथा
		    {
			// recursive: lock/unlock the items the List contains
			CHECK_LIST_MATERIALIZE(l);
			FOR_ALL_LIST_वस्तूनि(l, li) item_lock(&li->li_tv,
					       deep - 1, lock, check_refcount);
		    }
		}
	    }
	    भङ्ग;
	पक्ष VAR_TUPLE:
	    tuple_lock(tv->vval.v_tuple, deep, lock, check_refcount);
	    भङ्ग;
	पक्ष VAR_DICT:
	    सचे ((d = tv->vval.v_dict) != रिक्त
				    && !(check_refcount && d->dv_refcount > 1))
	    {
		सचे (lock)
		    d->dv_lock |= VAR_LOCKED;
		अञ्ञथा
		    d->dv_lock &= ~VAR_LOCKED;
		सचे (deep < 0 || deep > 1)
		{
		    // recursive: lock/unlock the items the List contains
		    todo = (पूर्णाङ्क)d->dv_hashtab.ht_used;
		    FOR_ALL_HASHTAB_वस्तूनि(&d->dv_hashtab, hi, todo)
		    {
			सचे (!HASHITEM_EMPTY(hi))
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

#सचे defined(FEAT_MENU) && defined(FEAT_MULTI_LANG)
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
    अत्थाय (hi = globvarht.ht_array; todo > 0 && !got_int; ++hi)
    {
	सचे (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    सचे (STRNCMP(HI2DI(hi)->di_कुञ्चिका, "menutrans_", 10) == 0)
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
    सचे (len > varnamebuflen)
    {
	vim_free(varnamebuf);
	len += 10;			// some additional space
	varnamebuf = alloc(len);
	सचे (varnamebuf == रिक्त)
	{
	    varnamebuflen = 0;
	    पटिनिवत्त रिक्त;
	}
	varnamebuflen = len;
    }
    *varnamebuf = prefix;
    varnamebuf[1] = ':';
    STRCPY(varnamebuf + 2, name);
    पटिनिवत्त varnamebuf;
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

    सचे (idx == 0)
    {
	gdone = bdone = wdone = vidx = 0;
	tdone = 0;
    }

    // Global variables
    सचे (gdone < globvarht.ht_used)
    {
	सचे (gdone++ == 0)
	    hi = globvarht.ht_array;
	अञ्ञथा
	    ++hi;
	याव (HASHITEM_EMPTY(hi))
	    ++hi;
	सचे (STRNCMP("g:", xp->xp_pattern, 2) == 0)
	    पटिनिवत्त cat_prefix_varname('g', hi->hi_कुञ्चिका);
	पटिनिवत्त hi->hi_कुञ्चिका;
    }

    // b: variables
    ht = &prevwin_curwin()->w_buffer->b_vars->dv_hashtab;
    सचे (bdone < ht->ht_used)
    {
	सचे (bdone++ == 0)
	    hi = ht->ht_array;
	अञ्ञथा
	    ++hi;
	याव (HASHITEM_EMPTY(hi))
	    ++hi;
	पटिनिवत्त cat_prefix_varname('b', hi->hi_कुञ्चिका);
    }

    // w: variables
    ht = &prevwin_curwin()->w_vars->dv_hashtab;
    सचे (wdone < ht->ht_used)
    {
	सचे (wdone++ == 0)
	    hi = ht->ht_array;
	अञ्ञथा
	    ++hi;
	याव (HASHITEM_EMPTY(hi))
	    ++hi;
	पटिनिवत्त cat_prefix_varname('w', hi->hi_कुञ्चिका);
    }

    // t: variables
    ht = &curtab->tp_vars->dv_hashtab;
    सचे (tdone < ht->ht_used)
    {
	सचे (tdone++ == 0)
	    hi = ht->ht_array;
	अञ्ञथा
	    ++hi;
	याव (HASHITEM_EMPTY(hi))
	    ++hi;
	पटिनिवत्त cat_prefix_varname('t', hi->hi_कुञ्चिका);
    }

    // v: variables
    सचे (vidx < VV_LEN)
	पटिनिवत्त cat_prefix_varname('v', (char_u *)vimvars[vidx++].vv_name);

    VIM_CLEAR(varnamebuf);
    varnamebuflen = 0;
    पटिनिवत्त रिक्त;
}

    वर्ण *
get_var_special_name(पूर्णाङ्क nr)
{
    विकल्प (nr)
    {
	पक्ष VVAL_FALSE: पटिनिवत्त in_vim9script() ? "false" : "v:false";
	पक्ष VVAL_TRUE:  पटिनिवत्त in_vim9script() ? "true" : "v:true";
	पक्ष VVAL_NULL:  पटिनिवत्त in_vim9script() ? "null" : "v:null";
	पक्ष VVAL_NONE:  पटिनिवत्त "v:none";
    }
    internal_error("get_var_special_name()");
    पटिनिवत्त "42";
}

/*
 * Returns the global variable dictionary
 */
    dict_T *
get_globvar_dict(शून्य)
{
    पटिनिवत्त &globvardict;
}

/*
 * Returns the global variable hash table
 */
    hashtab_T *
get_globvar_ht(शून्य)
{
    पटिनिवत्त &globvarht;
}

/*
 * Returns the v: variable dictionary
 */
    dict_T *
get_vimvar_dict(शून्य)
{
    पटिनिवत्त &vimvardict;
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

    सचे (di == रिक्त)
	पटिनिवत्त -1;
    *di_flags = di->di_flags;
    vv = (संरचना vimvar *)((वर्ण *)di - offsetof(vimvar_T, vv_di));
    पटिनिवत्त (पूर्णाङ्क)(vv - vimvars);
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
    पटिनिवत्त vimvars[idx].vv_name;
}

/*
 * Get typval_T v: variable value.
 */
    typval_T *
get_vim_var_tv(पूर्णाङ्क idx)
{
    पटिनिवत्त &vimvars[idx].vv_tv;
}

    type_T *
get_vim_var_type(पूर्णाङ्क idx, garray_T *type_list)
{
    सचे (vimvars[idx].vv_type != रिक्त)
	पटिनिवत्त vimvars[idx].vv_type;
    पटिनिवत्त typval2type_vimvar(&vimvars[idx].vv_tv, type_list);
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

    सचे (idx < 0)
	पटिनिवत्त FAIL;
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
    पटिनिवत्त OK;
}

/*
 * Set v: variable to "tv".  Only accepts the same type.
 * Takes over the value of "tv".
 */
    पूर्णाङ्क
set_vim_var_tv(पूर्णाङ्क idx, typval_T *tv)
{
    सचे (vimvars[idx].vv_tv_type != tv->v_type)
    {
	emsg(_(e_type_mismatch_for_v_variable));
	clear_tv(tv);
	पटिनिवत्त FAIL;
    }
    // VV_RO is also checked when compiling, but let's check here as well.
    सचे (vimvars[idx].vv_flags & VV_RO)
    {
	semsg(_(e_cannot_change_readonly_variable_str), vimvars[idx].vv_name);
	पटिनिवत्त FAIL;
    }
    सचे (sandbox && (vimvars[idx].vv_flags & VV_RO_SBX))
    {
	semsg(_(e_cannot_set_variable_in_sandbox_str), vimvars[idx].vv_name);
	पटिनिवत्त FAIL;
    }
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_di.di_tv = *tv;
    पटिनिवत्त OK;
}

/*
 * Get number v: variable value.
 */
    varnumber_T
get_vim_var_nr(पूर्णाङ्क idx)
{
    पटिनिवत्त vimvars[idx].vv_nr;
}

/*
 * Get string v: variable value.  Uses a static buffer, can only be used once.
 * If the String variable has never been set, return an empty string.
 * Never returns NULL;
 */
    char_u *
get_vim_var_str(पूर्णाङ्क idx)
{
    पटिनिवत्त tv_get_string(&vimvars[idx].vv_tv);
}

/*
 * Get List v: variable value.  Caller must take care of reference count when
 * needed.
 */
    list_T *
get_vim_var_list(पूर्णाङ्क idx)
{
    पटिनिवत्त vimvars[idx].vv_list;
}

/*
 * Get Dict v: variable value.  Caller must take care of reference count when
 * needed.
 */
    dict_T *
get_vim_var_dict(पूर्णाङ्क idx)
{
    पटिनिवत्त vimvars[idx].vv_dict;
}

/*
 * Set v:char to character "c".
 */
    शून्य
set_vim_var_char(पूर्णाङ्क c)
{
    char_u  buf[MB_MAXBYTES + 1];
    size_t  buflen;

    सचे (has_mbyte)
	buflen = (*mb_char2bytes)(c, buf);
    अञ्ञथा
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
    long	गणनम्,
    long	count1,
    पूर्णाङ्क		set_prevcount)
{
    सचे (set_prevcount)
	vimvars[VV_PREVCOUNT].vv_nr = vimvars[VV_गणनम्].vv_nr;
    vimvars[VV_गणनम्].vv_nr = गणनम्;
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
    vvsave->vv_गणनम् = vimvars[VV_गणनम्].vv_nr;
    vvsave->vv_count1 = vimvars[VV_COUNT1].vv_nr;
}

/*
 * Restore variables saved by save_vimvars().
 */
    शून्य
restore_vimvars(vimvars_save_T *vvsave)
{
    vimvars[VV_PREVCOUNT].vv_nr = vvsave->vv_prevcount;
    vimvars[VV_गणनम्].vv_nr = vvsave->vv_गणनम्;
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
    सचे (val == रिक्त)
	vimvars[idx].vv_str = रिक्त;
    अञ्ञथा सचे (len == -1)
	vimvars[idx].vv_str = vim_strsave(val);
    अञ्ञथा
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
    सचे (val != रिक्त)
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
    सचे (val == रिक्त)
	पटिनिवत्त;

    ++val->dv_refcount;
    dict_set_वस्तूनि_ro(val);
}

/*
 * Set the v:argv list.
 */
    शून्य
set_argv_var(वर्ण **argv, पूर्णाङ्क argc)
{
    list_T	*l = list_alloc();
    पूर्णाङ्क		i;

    सचे (l == रिक्त)
	getout(1);
    l->lv_lock = VAR_FIXED;
    अत्थाय (i = 0; i < argc; ++i)
    {
	सचे (list_append_string(l, (char_u *)argv[i], -1) == FAIL)
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

    सचे (c == 0 || c == ' ')
	regname[0] = '"';
    अञ्ञथा
	regname[0] = c;
    regname[1] = NUL;
    // Avoid free/alloc when the value is already right.
    सचे (vimvars[VV_REG].vv_str == रिक्त || vimvars[VV_REG].vv_str[0] != c)
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
    सचे (oldval == रिक्त)
	पटिनिवत्त vimvars[VV_EXCEPTION].vv_str;

    vimvars[VV_EXCEPTION].vv_str = oldval;
    पटिनिवत्त रिक्त;
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
    सचे (oldval == रिक्त)
	पटिनिवत्त vimvars[VV_THROWPOINT].vv_str;

    vimvars[VV_THROWPOINT].vv_str = oldval;
    पटिनिवत्त रिक्त;
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
    सचे (eap == रिक्त)
    {
	vim_free(oldval);
	vimvars[VV_CMDARG].vv_str = oldarg;
	पटिनिवत्त रिक्त;
    }

    सचे (eap->force_bin == FORCE_BIN)
	len = 6;
    अञ्ञथा सचे (eap->force_bin == FORCE_NOBIN)
	len = 8;
    अञ्ञथा
	len = 0;

    सचे (eap->read_edit)
	len += 7;

    सचे (eap->force_ff != 0)
	len += 10; // " ++ff=unix"
    सचे (eap->force_enc != 0)
	len += (unsigned)STRLEN(eap->cmd + eap->force_enc) + 7;
    सचे (eap->bad_char != 0)
	len += 7 + 4;  // " ++bad=" + "keep" or "drop"

    newval = alloc(len + 1);
    सचे (newval == रिक्त)
	पटिनिवत्त रिक्त;

    सचे (eap->force_bin == FORCE_BIN)
	sprintf((वर्ण *)newval, " ++bin");
    अञ्ञथा सचे (eap->force_bin == FORCE_NOBIN)
	sprintf((वर्ण *)newval, " ++nobin");
    अञ्ञथा
	*newval = NUL;

    सचे (eap->read_edit)
	STRCAT(newval, " ++edit");

    सचे (eap->force_ff != 0)
	sprintf((वर्ण *)newval + STRLEN(newval), " ++ff=%s",
						eap->force_ff == 'u' ? "unix"
						: eap->force_ff == 'd' ? "dos"
						: "mac");
    सचे (eap->force_enc != 0)
	sprintf((वर्ण *)newval + STRLEN(newval), " ++enc=%s",
					       eap->cmd + eap->force_enc);
    सचे (eap->bad_char == BAD_KEEP)
	STRCPY(newval + STRLEN(newval), " ++bad=keep");
    अञ्ञथा सचे (eap->bad_char == BAD_DROP)
	STRCPY(newval + STRLEN(newval), " ++bad=drop");
    अञ्ञथा सचे (eap->bad_char != 0)
	sprintf((वर्ण *)newval + STRLEN(newval), " ++bad=%c", eap->bad_char);
    vimvars[VV_CMDARG].vv_str = newval;
    पटिनिवत्त oldval;
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

    सचे (len > 0)
    {
	// truncate the name, so that we can use strcmp()
	cc = name[len];
	name[len] = NUL;
    }

    // Check for local variable when debugging.
    सचे ((sid == 0) && (tv = lookup_debug_var(name)) == रिक्त)
    {
	// Check for user-defined variables.
	dictitem_T	*v = find_var(name, &ht, flags & EVAL_VAR_NOAUTOLOAD);

	सचे (v != रिक्त)
	{
	    tv = &v->di_tv;
	    सचे (dip != रिक्त)
		*dip = v;
	}
	अञ्ञथा
	    ht = रिक्त;
    }

    सचे (tv == रिक्त && (in_vim9script() || STRNCMP(name, "s:", 2) == 0))
    {
	imported_T  *import = रिक्त;
	char_u	    *p = STRNCMP(name, "s:", 2) == 0 ? name + 2 : name;

	सचे (sid == 0)
	    import = find_imported(p, 0, TRUE);

	// imported variable from another script
	सचे (import != रिक्त || sid != 0)
	{
	    सचे ((flags & EVAL_VAR_IMPORT) == 0)
	    {
		सचे (SCRIPT_ID_VALID(sid))
		{
		    ht = &SCRIPT_VARS(sid);
		    सचे (ht != रिक्त)
		    {
			dictitem_T *v = find_var_in_ht(ht, 0, name,
						  flags & EVAL_VAR_NOAUTOLOAD);

			सचे (v == रिक्त)
			    v = find_var_autoload_prefix(name, sid, रिक्त, रिक्त);

			सचे (v != रिक्त)
			{
			    tv = &v->di_tv;
			    सचे (dip != रिक्त)
				*dip = v;
			}
			अञ्ञथा
			    ht = रिक्त;
		    }
		}
		अञ्ञथा
		{
		    सचे (flags & EVAL_VAR_VERBOSE)
			semsg(_(e_expected_dot_after_name_str), name);
		    ret = FAIL;
		}
	    }
	    अञ्ञथा
	    {
		सचे (rettv != रिक्त)
		{
		    // special value that is used in handle_subscript()
		    rettv->v_type = VAR_ANY;
		    rettv->vval.v_सङ्ख्या = sid != 0 ? sid : import->imp_sid;
		}
		found = TRUE;
	    }
	}
	अञ्ञथा सचे (in_vim9script() && (flags & EVAL_VAR_NO_FUNC) == 0)
	{
	    पूर्णाङ्क	    has_g_prefix = STRNCMP(name, "g:", 2) == 0;
	    ufunc_T *ufunc = find_func(name, FALSE);

	    सचे (ufunc != रिक्त && cc == '<')
	    {
		// handle generic function
		char_u	*argp = name + len;
		name[len] = cc;
		ufunc = eval_generic_func(ufunc, name, &argp);
		name[len] = NUL;
		सचे (ufunc == रिक्त)
		{
		    ret = FAIL;
		    गच्छ done;
		}
	    }

	    // In Vim9 script we can get a function reference by using the
	    // function name.  For a global non-autoload function "g:" is
	    // required.
	    सचे (ufunc != रिक्त && (has_g_prefix
					    || !func_requires_g_prefix(ufunc)))
	    {
		found = TRUE;
		सचे (rettv != रिक्त)
		{
		    rettv->v_type = VAR_FUNC;
		    सचे (has_g_prefix)
		    {
			// Keep the "g:", otherwise script-local may be
			// assumed.
			सचे (cc != '<')
			    rettv->vval.v_string = vim_strsave(name);
			अञ्ञथा
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
		    अञ्ञथा
			rettv->vval.v_string = vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
		    सचे (rettv->vval.v_string != रिक्त)
			func_ref(ufunc->uf_name);
		}
	    }
	}
    }

    सचे (!found)
    {
	सचे (tv == रिक्त)
	{
	    सचे (rettv != रिक्त && (flags & EVAL_VAR_VERBOSE))
		semsg(_(e_undefined_variable_str), name);
	    ret = FAIL;
	}
	अञ्ञथा सचे (rettv != रिक्त)
	{
	    svar_T  *sv = रिक्त;
	    पूर्णाङ्क	    was_assigned = FALSE;

	    सचे (ht != रिक्त && ht == get_script_local_ht()
		    && tv != &SCRIPT_SV(current_sctx.sc_sid)->sv_var.di_tv)
	    {
		sv = find_typval_in_script(tv, 0, TRUE);
		सचे (sv != रिक्त)
		{
		    type = sv->sv_type;
		    was_assigned = sv->sv_flags & SVFLAG_ASSIGNED;
		}
	    }

	    सचे ((tv->v_type == VAR_TYPEALIAS || tv->v_type == VAR_CLASS)
		    && sid != 0)
	    {
		// type alias or class imported from another script.  Check
		// whether it is exported from the other script.
		sv = find_typval_in_script(tv, sid, TRUE);
		सचे (sv == रिक्त)
		{
		    ret = FAIL;
		    गच्छ done;
		}
		सचे ((sv->sv_flags & SVFLAG_EXPORTED) == 0)
		{
		    semsg(_(e_item_not_exported_in_script_str), name);
		    ret = FAIL;
		    गच्छ done;
		}
	    }

	    // If a list or tuple or dict variable wasn't initialized and has
	    // meaningful type, do it now.  Not for global variables, they are
	    // not declared.
	    सचे (ht != &globvarht)
	    {
		सचे (tv->v_type == VAR_DICT && tv->vval.v_dict == रिक्त
					    && ((type != रिक्त && !was_assigned)
							  || !in_vim9script()))
		{
		    tv->vval.v_dict = dict_alloc();
		    सचे (tv->vval.v_dict != रिक्त)
		    {
			++tv->vval.v_dict->dv_refcount;
			tv->vval.v_dict->dv_type = alloc_type(type);
			सचे (sv != रिक्त)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
		अञ्ञथा सचे (tv->v_type == VAR_LIST && tv->vval.v_list == रिक्त
					    && ((type != रिक्त && !was_assigned)
							  || !in_vim9script()))
		{
		    tv->vval.v_list = list_alloc();
		    सचे (tv->vval.v_list != रिक्त)
		    {
			++tv->vval.v_list->lv_refcount;
			tv->vval.v_list->lv_type = alloc_type(type);
			सचे (sv != रिक्त)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
		अञ्ञथा सचे (tv->v_type == VAR_TUPLE && tv->vval.v_tuple == रिक्त
					    && ((type != रिक्त && !was_assigned)
							  || !in_vim9script()))
		{
		    tv->vval.v_tuple = tuple_alloc();
		    सचे (tv->vval.v_tuple != रिक्त)
		    {
			++tv->vval.v_tuple->tv_refcount;
			tv->vval.v_tuple->tv_type = alloc_type(type);
			सचे (sv != रिक्त)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
		अञ्ञथा सचे (tv->v_type == VAR_BLOB && tv->vval.v_blob == रिक्त
					    && ((type != रिक्त && !was_assigned)
							  || !in_vim9script()))
		{
		    tv->vval.v_blob = blob_alloc();
		    सचे (tv->vval.v_blob != रिक्त)
		    {
			++tv->vval.v_blob->bv_refcount;
			सचे (sv != रिक्त)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
	    }
	    ret = copy_tv(tv, rettv);
	}
    }

done:
    सचे (len > 0)
	name[len] = cc;

    पटिनिवत्त ret;
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
    याव (ASCII_ISALNUM(*s) || *s == '_')
	++s;
    पूर्णाङ्क	    len = (पूर्णाङ्क)(s - name);

    सचे (eval_variable(name, len, 0, rettv, रिक्त, EVAL_VAR_IMPORT) == FAIL)
	पटिनिवत्त FAIL;
    सचे (rettv->v_type == VAR_ANY && *s == '.')
    {
	char_u *ns = s + 1;
	s = ns;
	याव (ASCII_ISALNUM(*s) || *s == '_')
	    ++s;
	पूर्णाङ्क sid = rettv->vval.v_सङ्ख्या;
	पटिनिवत्त eval_variable(ns, (पूर्णाङ्क)(s - ns), sid, rettv, रिक्त, 0);
    }
    पटिनिवत्त OK;
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

    सचे (eval_lavars_used == रिक्त)
	पटिनिवत्त;

    // truncate the name, so that we can use strcmp()
    cc = name[len];
    name[len] = NUL;

    ht = find_var_ht(name, &varname);
    सचे (ht == get_funccal_local_ht() || ht == get_funccal_args_ht())
    {
	सचे (find_var(name, रिक्त, TRUE) != रिक्त)
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
    सचे (htp != रिक्त)
	*htp = ht;
    सचे (ht == रिक्त)
	पटिनिवत्त रिक्त;
    ret = find_var_in_ht(ht, *name, varname, no_autoload);
    सचे (ret != रिक्त)
	पटिनिवत्त ret;

    // Search in parent scope for lambda
    ret = find_var_in_scoped_ht(name, no_autoload);
    सचे (ret != रिक्त)
	पटिनिवत्त ret;

    // in Vim9 script items without a scope can be script-local
    सचे (in_vim9script() && name[0] != NUL && name[1] != ':')
    {
	ht = get_script_local_ht();
	सचे (ht != रिक्त)
	{
	    ret = find_var_in_ht(ht, *name, varname, no_autoload);
	    सचे (ret != रिक्त)
	    {
		सचे (htp != रिक्त)
		    *htp = ht;
		पटिनिवत्त ret;
	    }
	}
    }

    // and finally try
    पटिनिवत्त find_var_autoload_prefix(name, 0, htp, रिक्त);
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
    सचे (SCRIPT_ID_VALID(check_sid)
		   && (in_vim9script() || (name[0] == 's' && name[1] == ':')))
    {
	scriptitem_T *si = SCRIPT_ITEM(check_sid);

	सचे (si->sn_autoload_prefix != रिक्त)
	{
	    char_u *base_name = (name[0] == 's' && name[1] == ':')
							     ? name + 2 : name;
	    char_u *auto_name = concat_str(si->sn_autoload_prefix, base_name);

	    सचे (auto_name != रिक्त)
	    {
		पूर्णाङ्क free_auto_name = TRUE;
		ht = &globvarht;
		ret = find_var_in_ht(ht, 'g', auto_name, TRUE);
		सचे (ret != रिक्त)
		{
		    सचे (htp != रिक्त)
			*htp = ht;
		    सचे (namep != रिक्त)
		    {
			free_auto_name = FALSE;
			*namep = auto_name;
		    }
		}
		सचे (free_auto_name)
		    vim_free(auto_name);
	    }
	}
    }

    पटिनिवत्त ret;
}

/*
 * Like find_var() but if the name starts with <SNR>99_ then look in the
 * referenced script (used for a funcref).
 */
    dictitem_T *
find_var_also_in_script(char_u *name, hashtab_T **htp, पूर्णाङ्क no_autoload)
{
    सचे (STRNCMP(name, "<SNR>", 5) == 0 && SAFE_isdigit(name[5]))
    {
	char_u	    *p = name + 5;
	पूर्णाङ्क	    sid = getdigits(&p);

	सचे (SCRIPT_ID_VALID(sid) && *p == '_')
	{
	    hashtab_T	*ht = &SCRIPT_VARS(sid);

	    सचे (ht != रिक्त)
	    {
		dictitem_T *di = find_var_in_ht(ht, 0, p + 1, no_autoload);

		सचे (di != रिक्त)
		{
		    सचे (htp != रिक्त)
			*htp = ht;
		    पटिनिवत्त di;
		}
	    }
	}
    }

    पटिनिवत्त find_var(name, htp, no_autoload);
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

    सचे (*varname == NUL)
    {
	// Must be something like "s:", otherwise "ht" would be NULL.
	विकल्प (htname)
	{
	    पक्ष 's': पटिनिवत्त &SCRIPT_SV(current_sctx.sc_sid)->sv_var;
	    पक्ष 'g': पटिनिवत्त &globvars_var;
	    पक्ष 'v': पटिनिवत्त &vimvars_var;
	    पक्ष 'b': पटिनिवत्त &curbuf->b_bufvar;
	    पक्ष 'w': पटिनिवत्त &curwin->w_winvar;
	    पक्ष 't': पटिनिवत्त &curtab->tp_winvar;
	    पक्ष 'l': पटिनिवत्त get_funccal_local_var();
	    पक्ष 'a': पटिनिवत्त get_funccal_args_var();
	}
	पटिनिवत्त रिक्त;
    }

    hi = hash_find(ht, varname);
    सचे (HASHITEM_EMPTY(hi))
    {
	// For global variables we may try auto-loading the script.  If it
	// worked find the variable again.  Don't auto-load a script if it was
	// loaded already, otherwise it would be loaded every time when
	// checking if a function name is a Funcref variable.
	सचे (ht == &globvarht && !no_autoload)
	{
	    // Note: script_autoload() may make "hi" invalid. It must either
	    // be obtained again or not used.
	    सचे (!script_autoload(varname, FALSE) || aborting())
		पटिनिवत्त रिक्त;
	    hi = hash_find(ht, varname);
	}
	सचे (HASHITEM_EMPTY(hi))
	    पटिनिवत्त रिक्त;
    }
    पटिनिवत्त HI2DI(hi);
}

/*
 * Get the script-local hashtab.  NULL if not in a script context.
 */
    hashtab_T *
get_script_local_ht(शून्य)
{
    scid_T sid = current_sctx.sc_sid;

    सचे (SCRIPT_ID_VALID(sid))
	पटिनिवत्त &SCRIPT_VARS(sid);
    पटिनिवत्त रिक्त;
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

    सचे (ht == रिक्त)
	पटिनिवत्त FAIL;
    सचे (len < परिमाण(buffer) - 1)
    {
	// avoid an alloc/free for short names
	vim_strncpy(buffer, name, len);
	p = buffer;
    }
    अञ्ञथा
    {
	p = vim_strnsave(name, len);
	सचे (p == रिक्त)
	    पटिनिवत्त FAIL;
    }

    hi = hash_find(ht, p);
    res = HASHITEM_EMPTY(hi) ? FAIL : OK;

    // if not script-local, then perhaps autoload-exported
    सचे (res == FAIL && find_var_autoload_prefix(p, 0, रिक्त, रिक्त) != रिक्त)
	res = OK;

    // if not script-local or autoload, then perhaps imported
    सचे (res == FAIL && find_imported(p, 0, FALSE) != रिक्त)
	res = OK;
    सचे (p != buffer)
	vim_free(p);

    // Find a function, so that a following "->" works.
    // When used as a command require "(" or "->" to follow, "Cmd" is a user
    // command while "Cmd()" is a function call.
    सचे (res != OK)
    {
	p = skipwhite(name + len);

	सचे (!cmd || name[len] == '(' || (p[0] == '-' && p[1] == '>'))
	{
	    // Do not check for an internal function, since it might also be a
	    // valid command, such as ":split" versus "split()".
	    // Skip "g:" before a function name.
	    सचे (name[0] == 'g' && name[1] == ':')
	    {
		is_global = TRUE;
		fname = name + 2;
	    }
	    सचे (find_func(fname, is_global) != रिक्त)
		res = OK;
	}
    }

    पटिनिवत्त res;
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

    सचे (name[0] == NUL)
	पटिनिवत्त रिक्त;
    सचे (name[1] != ':')
    {
	// The name must not start with a colon or #.
	सचे (name[0] == ':' || name[0] == AUTOLOAD_CHAR)
	    पटिनिवत्त रिक्त;
	*varname = name;

	// "version" is "v:version" in all scopes if scriptversion < 3.
	// Same for a few other variables marked with VV_COMPAT.
	सचे (in_old_script(3))
	{
	    hi = hash_find(&compat_hashtab, name);
	    सचे (!HASHITEM_EMPTY(hi))
		पटिनिवत्त &compat_hashtab;
	}

	ht = get_funccal_local_ht();
	सचे (ht != रिक्त)
	    पटिनिवत्त ht;				// local variable

	// In Vim9 script items at the script level are script-local, except
	// for autoload names.
	सचे (in_vim9script() && vim_strchr(name, AUTOLOAD_CHAR) == रिक्त)
	{
	    ht = get_script_local_ht();
	    सचे (ht != रिक्त)
		पटिनिवत्त ht;
	}

	पटिनिवत्त &globvarht;			// global variable
    }
    *varname = name + 2;
    सचे (*name == 'g')				// global variable
	पटिनिवत्त &globvarht;
    // There must be no ':' or '#' in the rest of the name, unless g: is used
    सचे (vim_strchr(name + 2, ':') != रिक्त
			       || vim_strchr(name + 2, AUTOLOAD_CHAR) != रिक्त)
	पटिनिवत्त रिक्त;
    सचे (*name == 'b')				// buffer variable
	पटिनिवत्त &curbuf->b_vars->dv_hashtab;
    सचे (*name == 'w')				// window variable
	पटिनिवत्त &curwin->w_vars->dv_hashtab;
    सचे (*name == 't')				// tab page variable
	पटिनिवत्त &curtab->tp_vars->dv_hashtab;
    सचे (*name == 'v')				// v: variable
	पटिनिवत्त &vimvarht;
    सचे (get_current_funccal() != रिक्त
	       && get_current_funccal()->fc_func->uf_def_status
							    == UF_NOT_COMPILED)
    {
	// a: and l: are only used in functions defined with ":function"
	सचे (*name == 'a')			// a: function argument
	    पटिनिवत्त get_funccal_args_ht();
	सचे (*name == 'l')			// l: local function variable
	    पटिनिवत्त get_funccal_local_ht();
    }
    सचे (*name == 's')				// script variable
    {
	ht = get_script_local_ht();
	सचे (ht != रिक्त)
	    पटिनिवत्त ht;
    }
    पटिनिवत्त रिक्त;
}

/*
 * Get the string value of a (global/local) variable.
 * Note: see tv_get_string() for how long the pointer remains valid.
 * Returns NULL when it doesn't exist.
 */
    char_u *
get_var_मानम्(char_u *name)
{
    dictitem_T	*v;

    v = find_var(name, रिक्त, FALSE);
    सचे (v == रिक्त)
	पटिनिवत्त रिक्त;
    पटिनिवत्त tv_get_string(&v->di_tv);
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
    सचे (sv == रिक्त)
	पटिनिवत्त;
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
    dict_var->di_कुञ्चिका[0] = NUL;
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
    FOR_ALL_HASHTAB_वस्तूनि(ht, hi, todo)
    {
	सचे (!HASHITEM_EMPTY(hi))
	{
	    --todo;

	    // Free the variable.  Don't remove it from the hashtab,
	    // ht_array might change then.  hash_clear() takes care of it
	    // later.
	    v = HI2DI(hi);
	    सचे (free_val)
		clear_tv(&v->di_tv);
	    सचे (v->di_flags & DI_FLAGS_ALLOC)
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

    सचे (hash_remove(ht, hi, "delete variable") != OK)
	पटिनिवत्त;

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
    सचे (prefix == रिक्त)
	पटिनिवत्त;
    size_t  prefixlen = STRLEN(prefix);

    hash_lock(&globvarht);
    पूर्णाङ्क	todo = (पूर्णाङ्क)globvarht.ht_used;

    अत्थाय (hashitem_T *hi = globvarht.ht_array; todo > 0; ++hi)
	सचे (!HASHITEM_EMPTY(hi))
	{
	    dictitem_T	*di = HI2DI(hi);

	    --todo;
	    // Keep a class or enum: existing objects still refer to it.
	    सचे (di->di_tv.v_type != VAR_CLASS
		    && STRNCMP(di->di_कुञ्चिका, prefix, prefixlen) == 0)
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
    list_one_var_a(prefix, v->di_कुञ्चिका, v->di_tv.v_type,
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
    सन्देशः_start();
    सन्देशः_puts(prefix);
    सचे (name != रिक्त)	// "a:" vars don't have a name stored
	सन्देशः_puts((वर्ण *)name);
    सन्देशः_putchar(' ');
    सन्देशः_advance(22);
    सचे (type == VAR_सङ्ख्या)
	सन्देशः_putchar('#');
    अञ्ञथा सचे (type == VAR_FUNC || type == VAR_PARTIAL)
	सन्देशः_putchar('*');
    अञ्ञथा सचे (type == VAR_LIST)
    {
	सन्देशः_putchar('[');
	सचे (*string == '[')
	    ++string;
    }
    अञ्ञथा सचे (type == VAR_DICT)
    {
	सन्देशः_putchar('{');
	सचे (*string == '{')
	    ++string;
    }
    अञ्ञथा
	सन्देशः_putchar(' ');

    सन्देशः_outtrans(string);

    सचे (type == VAR_FUNC || type == VAR_PARTIAL)
	सन्देशः_puts("()");
    सचे (*first)
    {
	सन्देशः_clr_eos();
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
    सचे (di->di_tv.v_type == VAR_STRING)
    {
	VIM_CLEAR(di->di_tv.vval.v_string);
	सचे (copy || tv->v_type != VAR_STRING)
	{
	    char_u *val = tv_get_string(tv);

	    // Careful: when assigning to v:errmsg and
	    // tv_get_string() causes an error message the variable
	    // will already be set.
	    सचे (di->di_tv.vval.v_string == रिक्त)
		di->di_tv.vval.v_string = vim_strsave(val);
	}
	अञ्ञथा
	{
	    // Take over the string to avoid an extra alloc/free.
	    di->di_tv.vval.v_string = tv->vval.v_string;
	    tv->vval.v_string = रिक्त;
	}
	पटिनिवत्त FALSE;
    }
    अञ्ञथा सचे (di->di_tv.v_type == VAR_सङ्ख्या)
    {
	di->di_tv.vval.v_सङ्ख्या = tv_get_सङ्ख्या(tv);
	सचे (STRCMP(varname, "searchforward") == 0)
	    set_अन्वेषणम्_direction(di->di_tv.vval.v_सङ्ख्या ? '/' : '?');
#ifdef FEAT_अन्वेषणम्_EXTRA
	अञ्ञथा सचे (STRCMP(varname, "hlsearch") == 0)
	{
	    no_hlsearch = !di->di_tv.vval.v_सङ्ख्या;
	    redraw_all_later(UPD_SOME_VALID);
	}
#endif
	पटिनिवत्त FALSE;
    }
    अञ्ञथा सचे (di->di_tv.v_type != tv->v_type)
    {
	*type_error = TRUE;
	पटिनिवत्त FALSE;
    }
    पटिनिवत्त TRUE;
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

    सचे (sid != 0)
    {
	varname = रिक्त;
	सचे (SCRIPT_ID_VALID(sid))
	{
	    char_u	*auto_name = रिक्त;
	    सचे (find_var_autoload_prefix(name, sid, &ht, &auto_name) != रिक्त)
	    {
		var_in_autoload = TRUE;
		varname = auto_name;
		name_tofree = varname;
	    }
	    अञ्ञथा
		ht = &SCRIPT_VARS(sid);
	}
	सचे (varname == रिक्त)
	    varname = name;
    }
    अञ्ञथा
    {
	scriptitem_T	*si;
	char_u		*auto_name = रिक्त;

	सचे (in_vim9script()
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
	    सचे (varname == रिक्त)
		गच्छ failed;
	    name_tofree = varname;
	    ht = &globvarht;
	}
	अञ्ञथा
	    ht = find_var_ht(name, &varname);
    }
    सचे (ht == रिक्त || *varname == NUL)
    {
	semsg(_(e_illegal_variable_name_str), name);
	गच्छ failed;
    }
    is_script_local = ht == get_script_local_ht() || sid != 0
							    || var_in_autoload;

    सचे (vim9script
	    && !is_script_local
	    && (flags & (ASSIGN_NO_DECL | ASSIGN_DECL)) == 0
	    && (flags & (ASSIGN_CONST | ASSIGN_FINAL)) == 0
	    && name[1] == ':')
    {
	vim9_declare_error(name);
	गच्छ failed;
    }
    सचे ((flags & ASSIGN_FOR_LOOP) && is_scoped_variable(name))
	// Do not make g:var, w:var, b:var or t:var final.
	flags &= ~ASSIGN_FINAL;

    var_in_vim9script = is_script_local && current_script_is_vim9();
    सचे (var_in_vim9script && name[0] == '_' && name[1] == NUL)
    {
	// For "[a, _] = list" the underscore is ignored.
	सचे ((flags & ASSIGN_UNPACK) == 0)
	    emsg(_(e_cannot_use_underscore_here));
	गच्छ failed;
    }

    di = find_var_in_ht(ht, 0, varname, TRUE);

    सचे (di == रिक्त && var_in_vim9script)
    {
	imported_T  *import = find_imported(varname, 0, FALSE);

	सचे (import != रिक्त)
	{
	    // imported name space cannot be used
	    सचे ((flags & ASSIGN_NO_DECL) == 0)
	    {
		semsg(_(e_redefining_imported_item_str), name);
		गच्छ failed;
	    }
	    semsg(_(e_cannot_use_str_itself_it_is_imported), name);
	    गच्छ failed;
	}
	सचे (!in_vim9script())
	{
	    semsg(_(e_cannot_create_vim9_script_variable_in_function_str),
									 name);
	    गच्छ failed;
	}
    }

    // Search in parent scope which is possible to reference from lambda
    सचे (di == रिक्त)
	di = find_var_in_scoped_ht(name, TRUE);

    सचे ((tv->v_type == VAR_FUNC || tv->v_type == VAR_PARTIAL)
				  && var_wrong_func_name(name, di == रिक्त))
	गच्छ failed;

    सचे (need_convert_to_bool(type, tv))
    {
	// Destination is a bool and the value is not, but it can be
	// converted.
	CLEAR_FIELD(bool_tv);
	bool_tv.v_type = VAR_BOOL;
	bool_tv.vval.v_सङ्ख्या = tv2bool(tv) ? VVAL_TRUE : VVAL_FALSE;
	tv = &bool_tv;
    }

    सचे (di != रिक्त)
    {
	// Item already exists.  Allowed to replace when reloading.
	सचे ((di->di_flags & DI_FLAGS_RELOAD) == 0)
	{
	    सचे ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					 && (flags & ASSIGN_FOR_LOOP) == 0)
	    {
		emsg(_(e_cannot_modify_existing_variable));
		गच्छ failed;
	    }

	    सचे (is_script_local && vim9script
			  && (flags & (ASSIGN_NO_DECL | ASSIGN_DECL)) == 0)
	    {
		semsg(_(e_redefining_script_item_str), name);
		गच्छ failed;
	    }

	    सचे (check_typval_is_मानम्(&di->di_tv) == FAIL)
		गच्छ failed;

	    // List and Blob types can be modified in-place using the "+="
	    // compound operator.  For other types, this is not allowed.
	    पूर्णाङ्क type_inplace_modifiable =
		(di->di_tv.v_type == VAR_LIST || di->di_tv.v_type == VAR_BLOB);

	    सचे (var_in_vim9script && (flags & ASSIGN_FOR_LOOP) == 0
		    && ((flags & ASSIGN_COMPOUND_OP) == 0
			|| !type_inplace_modifiable))
	    {
		where_T where = WHERE_INIT;
		svar_T  *sv = find_typval_in_script(&di->di_tv, sid, TRUE);

		सचे (sv != रिक्त)
		{
		    // check the type and adjust to bool if needed
		    सचे (var_idx > 0)
		    {
			where.wt_index = var_idx;
			where.wt_kind = WT_VARIABLE;
		    }
		    सचे (check_script_var_type(sv, tv, name, where) == FAIL)
			गच्छ failed;
		    सचे (type == रिक्त)
			type = sv->sv_type;
		    sv->sv_flags |= SVFLAG_ASSIGNED;
		}
	    }

	    // Modifying a final variable with a List value using the "+="
	    // operator is allowed.  For other types, it is not allowed.
	    सचे ((((flags & ASSIGN_FOR_LOOP) == 0 || (flags & ASSIGN_DECL) == 0)
			&& ((flags & ASSIGN_COMPOUND_OP) == 0
			    || !type_inplace_modifiable))
				 ? var_check_permission(di, name) == FAIL
				 : var_check_ro(di->di_flags, name, FALSE))
		गच्छ failed;
	}
	अञ्ञथा
	{
	    // can only redefine once
	    di->di_flags &= ~DI_FLAGS_RELOAD;

	    // A Vim9 script-local variable is also present in sn_all_vars
	    // and sn_var_vals.  It may set "type" from "tv".
	    सचे (var_in_vim9script || var_in_autoload)
		update_vim9_script_var(FALSE, di,
			var_in_autoload ? name : di->di_कुञ्चिका, flags,
			tv, &type, (flags & ASSIGN_NO_MEMBER_TYPE) == 0);
	}

	// existing variable, need to clear the value

	// In Vim9 script a String v: variable only takes a String.
	सचे (ht == &vimvarht && di->di_tv.v_type == VAR_STRING
							    && in_vim9script())
	{
	    where_T	where = WHERE_INIT;

	    सचे (check_typval_type(&t_string, tv, where) == FAIL)
		गच्छ failed;
	}

	// Handle setting internal v: variables separately where needed to
	// prevent changing the type.
	पूर्णाङ्क type_error = FALSE;
	सचे (ht == &vimvarht
		&& !before_set_vvar(varname, di, tv, copy, &type_error))
	{
	    सचे (type_error)
		semsg(_(e_setting_v_str_to_मानम्_with_wrong_type), varname);
	    गच्छ failed;
	}

	clear_tv(&di->di_tv);

	सचे ((flags & ASSIGN_UPDATE_BLOCK_ID)
				       && SCRIPT_ID_VALID(current_sctx.sc_sid))
	{
	    scriptitem_T *si = SCRIPT_ITEM(current_sctx.sc_sid);

	    update_script_var_block_id(name, si->sn_current_block_id);
	}
    }
    अञ्ञथा
    {
	// Item not found, check if a function already exists.
	सचे (is_script_local && (flags & (ASSIGN_NO_DECL | ASSIGN_DECL)) == 0
	       && lookup_scriptitem(name, STRLEN(name), FALSE, रिक्त) == OK)
	{
	    semsg(_(e_redefining_script_item_str), name);
	    गच्छ failed;
	}

	// add a new variable
	सचे (var_in_vim9script && (flags & ASSIGN_NO_DECL))
	{
	    semsg(_(e_unknown_variable_str), name);
	    गच्छ failed;
	}

	सचे (check_hashtab_frozen(ht, "add variable"))
	    गच्छ failed;

	// Can't add "v:" or "a:" variable.
	सचे (ht == &vimvarht || ht == get_funccal_args_ht())
	{
	    semsg(_(e_illegal_variable_name_str), name);
	    गच्छ failed;
	}

	// Make sure the variable name is valid.  In Vim9 script an
	// autoload variable must be prefixed with "g:" unless in an
	// autoload script.
	सचे (!valid_varname(varname, -1, !vim9script
			|| STRNCMP(name, "g:", 2) == 0 || var_in_autoload))
	    गच्छ failed;

	di = alloc(offsetof(dictitem_T, di_कुञ्चिका) + STRLEN(varname) + 1);
	सचे (di == रिक्त)
	    गच्छ failed;
	STRCPY(di->di_कुञ्चिका, varname);
	सचे (hash_add(ht, DI2HIKEY(di), "add variable") == FAIL)
	{
	    vim_free(di);
	    गच्छ failed;
	}
	di->di_flags = DI_FLAGS_ALLOC;
	सचे (flags & (ASSIGN_CONST | ASSIGN_FINAL))
	    di->di_flags |= DI_FLAGS_LOCK;

	// A Vim9 script-local variable is also added to sn_all_vars and
	// sn_var_vals. It may set "type" from "tv".
	सचे (var_in_vim9script || var_in_autoload)
	    update_vim9_script_var(TRUE, di,
		    var_in_autoload ? name : di->di_कुञ्चिका, flags,
			  tv, &type, (flags & ASSIGN_NO_MEMBER_TYPE) == 0);
    }

    dest_tv = &di->di_tv;
    सचे (copy || tv->v_type == VAR_सङ्ख्या || tv->v_type == VAR_FLOAT)
	copy_tv(tv, dest_tv);
    अञ्ञथा
    {
	*dest_tv = *tv;
	dest_tv->v_lock = 0;
	init_tv(tv);
    }
    free_tv_arg = FALSE;

    सचे (vim9script && type != रिक्त)
	set_tv_type(dest_tv, type);

    // ":const var = value" locks the value
    // ":final var = value" locks "var"
    सचे (flags & ASSIGN_CONST)
	// Like :lockvar! name: lock the value and what it contains, but only
	// if the reference count is up to one.  That locks only literal
	// values.
	item_lock(dest_tv, DICT_MAXNEST, TRUE, TRUE);

    rc = OK;

failed:
    vim_free(name_tofree);
    सचे (free_tv_arg)
	clear_tv(tv_arg);

    पटिनिवत्त rc;
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
    सचे (var_check_ro(di->di_flags, name, FALSE)
		    || मानम्_check_lock(di->di_tv.v_lock, name, FALSE)
		    || var_check_lock(di->di_flags, name, FALSE))
	पटिनिवत्त FAIL;
    पटिनिवत्त OK;
}

/*
 * Return TRUE if di_flags "flags" indicates variable "name" is read-only.
 * Also give an error message.
 */
    पूर्णाङ्क
var_check_ro(पूर्णाङ्क flags, char_u *name, पूर्णाङ्क use_gettext)
{
    सचे (flags & DI_FLAGS_RO)
    {
	सचे (name == रिक्त)
	    emsg(_(e_cannot_change_readonly_variable));
	अञ्ञथा
	    semsg(_(e_cannot_change_readonly_variable_str),
				       use_gettext ? (char_u *)_(name) : name);
	पटिनिवत्त TRUE;
    }
    सचे ((flags & DI_FLAGS_RO_SBX) && sandbox)
    {
	सचे (name == रिक्त)
	    emsg(_(e_cannot_set_variable_in_sandbox));
	अञ्ञथा
	    semsg(_(e_cannot_set_variable_in_sandbox_str),
				       use_gettext ? (char_u *)_(name) : name);
	पटिनिवत्त TRUE;
    }
    पटिनिवत्त FALSE;
}

/*
 * Return TRUE if di_flags "flags" indicates variable "name" is locked.
 * Also give an error message.
 */
    पूर्णाङ्क
var_check_lock(पूर्णाङ्क flags, char_u *name, पूर्णाङ्क use_gettext)
{
    सचे (flags & DI_FLAGS_LOCK)
    {
	semsg(_(e_variable_is_locked_str),
				       use_gettext ? (char_u *)_(name) : name);
	पटिनिवत्त TRUE;
    }
    पटिनिवत्त FALSE;
}

/*
 * Return TRUE if di_flags "flags" indicates variable "name" is fixed.
 * Also give an error message.
 */
    पूर्णाङ्क
var_check_fixed(पूर्णाङ्क flags, char_u *name, पूर्णाङ्क use_gettext)
{
    सचे (flags & DI_FLAGS_FIX)
    {
	सचे (name == रिक्त)
	    emsg(_(e_cannot_delete_variable));
	अञ्ञथा
	    semsg(_(e_cannot_delete_variable_str),
				      use_gettext ? (char_u *)_(name) : name);
	पटिनिवत्त TRUE;
    }
    पटिनिवत्त FALSE;
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
    सचे (!((vim_strchr((char_u *)"wbt", name[0]) != रिक्त
		    || (!in_vim9script() && name[0] == 's')) && name[1] == ':')
	    && !ASCII_ISUPPER((name[0] != NUL && name[1] == ':')
						     ? name[2] : name[0])
	    && vim_strchr(name, '#') == रिक्त)
    {
	semsg(_(e_funcref_variable_name_must_start_with_capital_str), name);
	पटिनिवत्त TRUE;
    }
    // Don't allow hiding a function.  When "v" is not NULL we might be
    // assigning another function to the same var, the type is checked
    // below.
    सचे (new_var && function_exists(name, FALSE))
    {
	semsg(_(e_variable_name_conflicts_with_existing_function_str),
								    name);
	पटिनिवत्त TRUE;
    }
    पटिनिवत्त FALSE;
}

/*
 * Return TRUE if "flags" indicates variable "name" has a locked (immutable)
 * value.  Also give an error message, using "name" or _("name") when
 * "use_gettext" is TRUE.
 */
    पूर्णाङ्क
मानम्_check_lock(पूर्णाङ्क lock, char_u *name, पूर्णाङ्क use_gettext)
{
    सचे (lock & VAR_LOCKED)
    {
	सचे (name == रिक्त)
	    emsg(_(e_मानम्_is_locked));
	अञ्ञथा
	    semsg(_(e_मानम्_is_locked_str),
				       use_gettext ? (char_u *)_(name) : name);
	पटिनिवत्त TRUE;
    }
    सचे (lock & VAR_FIXED)
    {
	सचे (name == रिक्त)
	    emsg(_(e_cannot_change_मानम्));
	अञ्ञथा
	    semsg(_(e_cannot_change_मानम्_of_str),
				       use_gettext ? (char_u *)_(name) : name);
	पटिनिवत्त TRUE;
    }
    पटिनिवत्त FALSE;
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

    अत्थाय (p = varname; len < 0 ? *p != NUL : p < varname + len; ++p)
	सचे (!eval_isnamec1(*p) && (p == varname || !VIM_ISDIGIT(*p))
					 && !(autoload && *p == AUTOLOAD_CHAR))
	{
	    semsg(_(e_illegal_variable_name_str), varname);
	    पटिनिवत्त FALSE;
	}
    पटिनिवत्त TRUE;
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

    सचे (varname != रिक्त && tp != रिक्त && win != रिक्त
	    && (htname != 'b' || buf != रिक्त))
    {
	// Set curwin to be our win, temporarily.  Also set the tabpage,
	// otherwise the window is not valid. Only do this when needed,
	// autocommands get blocked.
	// If we have a buffer reference avoid the switching, we're saving and
	// restoring curbuf directly.
	need_switch_win = !(tp == curtab && win == curwin) && !do_change_curbuf;
	सचे (!need_switch_win || switch_win(&switchwin, win, tp, TRUE) == OK)
	{
	    // Handle options. There are no tab-local options.
	    सचे (*varname == '&' && htname != 't')
	    {
		buf_T	*save_curbuf = curbuf;

		// Change curbuf so the option is read from the correct buffer.
		सचे (do_change_curbuf)
		    curbuf = buf;

		सचे (varname[1] == NUL)
		{
		    // get all window-local or buffer-local options in a dict
		    dict_T	*opts = get_winbuf_options(htname == 'b');

		    सचे (opts != रिक्त)
		    {
			rettv_dict_set(rettv, opts);
			done = TRUE;
		    }
		}
		अञ्ञथा सचे (eval_option(&varname, rettv, TRUE) == OK)
		    // Local option
		    done = TRUE;

		curbuf = save_curbuf;
	    }
	    अञ्ञथा सचे (*varname == NUL)
	    {
		// Empty string: return a dict with all the local variables.
		सचे (htname == 'b')
		    v = &buf->b_bufvar;
		अञ्ञथा सचे (htname == 'w')
		    v = &win->w_winvar;
		अञ्ञथा
		    v = &tp->tp_winvar;
		copy_tv(&v->di_tv, rettv);
		done = TRUE;
	    }
	    अञ्ञथा
	    {
		hashtab_T	*ht;

		सचे (htname == 'b')
		    ht = &buf->b_vars->dv_hashtab;
		अञ्ञथा सचे (htname == 'w')
		    ht = &win->w_vars->dv_hashtab;
		अञ्ञथा
		    ht = &tp->tp_vars->dv_hashtab;

		// Look up the variable.
		v = find_var_in_ht(ht, htname, varname, FALSE);
		सचे (v != रिक्त)
		{
		    copy_tv(&v->di_tv, rettv);
		    done = TRUE;
		}
	    }
	}

	सचे (need_switch_win)
	    // restore previous notion of curwin
	    restore_win(&switchwin, TRUE);
    }

    सचे (!done && deftv->v_type != VAR_UNKNOWN)
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

    सचे (off == 1)
	tp = find_tabpage((पूर्णाङ्क)tv_get_सङ्ख्या_chk(&argvars[0], रिक्त));
    अञ्ञथा
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
    सचे (opt_idx < 0)
    {
	semsg(_(e_unknown_option_str_2), varname);
	पटिनिवत्त;
    }
    पूर्णाङ्क		opt_p_flags = get_option_flags(opt_idx);

    सचे (varp->v_type == VAR_BOOL)
    {
	सचे (opt_p_flags & P_STRING)
	{
	    emsg(_(e_string_required));
	    पटिनिवत्त;
	}
	numval = (long)varp->vval.v_सङ्ख्या;
	strval = (char_u *)"0";  // avoid using "false"
    }
    अञ्ञथा
    {
	सचे ((opt_p_flags & (P_अङ्कः|P_BOOL))
		&& (!in_vim9script() || varp->v_type != VAR_STRING))
	    numval = (long)tv_get_सङ्ख्या_chk(varp, &error);
	सचे (!error)
	    strval = tv_get_string_buf_chk(varp, nbuf);
    }
    सचे (!error && strval != रिक्त)
	set_option_मानम्_give_err(varname, numval, strval, OPT_LOCAL);
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

    सचे (check_secure())
	पटिनिवत्त;

    सचे (off == 1)
	tp = find_tabpage((पूर्णाङ्क)tv_get_सङ्ख्या_chk(&argvars[0], रिक्त));
    अञ्ञथा
	tp = curtab;
    win = find_win_by_nr(&argvars[off], tp);
    varname = tv_get_string_chk(&argvars[off + 1]);
    varp = &argvars[off + 2];

    सचे (win == रिक्त || varname == रिक्त)
	पटिनिवत्त;

    need_switch_win = !(tp == curtab && win == curwin);
    सचे (!need_switch_win
	    || switch_win(&switchwin, win, tp, TRUE) == OK)
    {
	सचे (*varname == '&')
	    set_option_from_tv(varname + 1, varp);
	अञ्ञथा
	{
	    winvarname = alloc(STRLEN(varname) + 3);
	    सचे (winvarname != रिक्त)
	    {
		STRCPY(winvarname, "w:");
		STRCPY(winvarname + 2, varname);
		set_var(winvarname, varp, TRUE);
		vim_free(winvarname);
	    }
	}
    }
    सचे (need_switch_win)
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

    सचे (vp->vv_tv_type != VAR_LIST || vimvars[VV_ERRORS].vv_list == रिक्त)
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
    सचे (len > 0)
    {
	सचे (tofree != रिक्त)
	    name = tofree;
	n = (eval_variable(name, len, 0, &tv, रिक्त,
				 EVAL_VAR_NOAUTOLOAD + EVAL_VAR_IMPORT) == OK);
	सचे (n)
	{
	    // handle d.key, l[idx], f(expr)
	    arg = skipwhite(arg);
	    n = (handle_subscript(&arg, name, &tv, &EVALARG_EVALUATE,
								 FALSE) == OK);
	    सचे (n)
		clear_tv(&tv);
	}
    }
    सचे (*arg != NUL)
	n = FALSE;

    vim_free(tofree);
    पटिनिवत्त n;
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
    सचे (redir_lval == रिक्त)
	पटिनिवत्त FAIL;
    पटिनिवत्त OK;
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
    सचे (!eval_isnamec1(*name))
    {
	emsg(_(e_invalid_argument));
	पटिनिवत्त FAIL;
    }

    // Make a copy of the name, it is used in redir_lval until redir ends.
    redir_varname = vim_strsave(name);
    सचे (redir_varname == रिक्त)
	पटिनिवत्त FAIL;

    सचे (alloc_redir_lval() == FAIL)
    {
	var_redir_stop();
	पटिनिवत्त FAIL;
    }

    // The output is stored in growarray "redir_ga" until redirection ends.
    init_redir_ga();

    // Parse the variable name (can be a dict or list entry).
    redir_endp = get_lval(redir_varname, रिक्त, redir_lval, FALSE, FALSE, 0,
							     FNE_CHECK_START);
    सचे (redir_endp == रिक्त || redir_lval->ll_name == रिक्त || *redir_endp != NUL)
    {
	clear_lval(redir_lval);
	सचे (redir_endp != रिक्त && *redir_endp != NUL)
	    // Trailing characters are present after the variable name
	    semsg(_(e_trailing_characters_str), redir_endp);
	अञ्ञथा
	    semsg(_(e_invalid_argument_str), name);
	redir_endp = रिक्त;  // don't store a value, only cleanup
	var_redir_stop();
	पटिनिवत्त FAIL;
    }

    // check if we can write to the variable: set it to or append an empty
    // string
    called_emsg_before = called_emsg;
    tv.v_type = VAR_STRING;
    tv.vval.v_string = (char_u *)"";
    सचे (append)
	set_var_lval(redir_lval, redir_endp, &tv, TRUE,
					     ASSIGN_NO_DECL, (char_u *)".", 0);
    अञ्ञथा
	set_var_lval(redir_lval, redir_endp, &tv, TRUE,
					     ASSIGN_NO_DECL, (char_u *)"=", 0);
    clear_lval(redir_lval);
    सचे (called_emsg > called_emsg_before)
    {
	redir_endp = रिक्त;  // don't store a value, only cleanup
	var_redir_stop();
	पटिनिवत्त FAIL;
    }

    पटिनिवत्त OK;
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
var_redir_str(char_u *मानम्, पूर्णाङ्क मानम्_len)
{
    पूर्णाङ्क		len;

    सचे (redir_lval == रिक्त)
	पटिनिवत्त;

    सचे (मानम्_len == -1)
	len = (पूर्णाङ्क)STRLEN(मानम्);	// Append the entire string
    अञ्ञथा
	len = मानम्_len;		// Append only "value_len" characters

    सचे (ga_grow(&redir_ga, len) == OK)
    {
	mch_memmove((वर्ण *)redir_ga.ga_data + redir_ga.ga_len, मानम्, len);
	redir_ga.ga_len += len;
    }
    अञ्ञथा
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

    सचे (EVALCMD_BUSY)
    {
	redir_lval = रिक्त;
	पटिनिवत्त;
    }

    सचे (redir_lval != रिक्त)
    {
	// If there was no error: assign the text to the variable.
	सचे (redir_endp != रिक्त)
	{
	    ga_append(&redir_ga, NUL);  // Append the trailing NUL.
	    tv.v_type = VAR_STRING;
	    tv.vval.v_string = redir_ga.ga_data;
	    // Call get_lval() again, if it's inside a Dict or List it may
	    // have changed.
	    redir_endp = get_lval(redir_varname, रिक्त, redir_lval,
					FALSE, FALSE, 0, FNE_CHECK_START);
	    सचे (redir_endp != रिक्त && redir_lval->ll_name != रिक्त)
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
    पटिनिवत्त res;
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

    सचे (in_vim9script()
	    && (check_for_सङ्ख्या_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	पटिनिवत्त;

    varname = tv_get_string_chk(&argvars[1]);
    tp = find_tabpage((पूर्णाङ्क)tv_get_सङ्ख्या_chk(&argvars[0], रिक्त));
    सचे (tp != रिक्त)
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
    सचे (in_vim9script()
	    && (check_for_सङ्ख्या_arg(argvars, 0) == FAIL
		|| check_for_सङ्ख्या_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL))
	पटिनिवत्त;

    getwinvar(argvars, rettv, 1);
}

/*
 * "getwinvar()" function
 */
    शून्य
f_getwinvar(typval_T *argvars, typval_T *rettv)
{
    सचे (in_vim9script()
	    && (check_for_सङ्ख्या_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	पटिनिवत्त;

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

    सचे (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	पटिनिवत्त;

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

    सचे (check_secure())
	पटिनिवत्त;

    सचे (in_vim9script()
	    && (check_for_सङ्ख्या_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	पटिनिवत्त;

    tp = find_tabpage((पूर्णाङ्क)tv_get_सङ्ख्या_chk(&argvars[0], रिक्त));
    varname = tv_get_string_chk(&argvars[1]);
    varp = &argvars[2];

    सचे (varname == रिक्त || tp == रिक्त)
	पटिनिवत्त;

    save_curtab = curtab;
    save_lu_tp = lastused_tabpage;
    goto_tabpage_tp(tp, FALSE, FALSE);

    tabvarname = alloc(STRLEN(varname) + 3);
    सचे (tabvarname != रिक्त)
    {
	STRCPY(tabvarname, "t:");
	STRCPY(tabvarname + 2, varname);
	set_var(tabvarname, varp, TRUE);
	vim_free(tabvarname);
    }

    // Restore current tabpage and last accessed tabpage.
    सचे (valid_tabpage(save_curtab))
    {
	goto_tabpage_tp(save_curtab, FALSE, FALSE);
	सचे (valid_tabpage(save_lu_tp))
	    lastused_tabpage = save_lu_tp;
    }
}

/*
 * "settabwinvar()" function
 */
    शून्य
f_settabwinvar(typval_T *argvars, typval_T *rettv UNUSED)
{
    सचे (in_vim9script()
	    && (check_for_सङ्ख्या_arg(argvars, 0) == FAIL
		|| check_for_सङ्ख्या_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL))
	पटिनिवत्त;

    setwinvar(argvars, 1);
}

/*
 * "setwinvar()" function
 */
    शून्य
f_setwinvar(typval_T *argvars, typval_T *rettv UNUSED)
{
    सचे (in_vim9script()
	    && (check_for_सङ्ख्या_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	पटिनिवत्त;

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

    सचे (check_secure())
	पटिनिवत्त;

    सचे (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	पटिनिवत्त;

    varname = tv_get_string_chk(&argvars[1]);
    buf = tv_get_buf_from_arg(&argvars[0]);
    varp = &argvars[2];

    सचे (buf == रिक्त || varname == रिक्त)
	पटिनिवत्त;

    सचे (*varname == '&')
    {
	aco_save_T	aco;
	// safe the current window position, it could
	// change because of 'scrollbind' window-local
	// options
	linenr_T	old_topline = curwin->w_topline;

	// Set curbuf to be our buf, temporarily.
	aucmd_prepbuf(&aco, buf);
	सचे (curbuf == buf)
	{
	    // Only when it worked to set "curbuf".
	    set_option_from_tv(varname + 1, varp);

	    // reset notion of buffer
	    aucmd_restbuf(&aco);
	}
	curwin->w_topline = old_topline;
    }
    अञ्ञथा
    {
	bufvarname = alloc(STRLEN(varname) + 3);
	सचे (bufvarname != रिक्त)
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
    सचे (arg->v_type == VAR_PARTIAL && arg->vval.v_partial != रिक्त)
    {
	res.cb_partial = arg->vval.v_partial;
	++res.cb_partial->pt_refcount;
	res.cb_name = partial_name(res.cb_partial);
    }
    अञ्ञथा
    {
	सचे (arg->v_type == VAR_STRING && arg->vval.v_string != रिक्त
					       && SAFE_isdigit(*arg->vval.v_string))
	    r = FAIL;
	अञ्ञथा सचे (arg->v_type == VAR_FUNC || arg->v_type == VAR_STRING)
	{
	    res.cb_name = arg->vval.v_string;
	    सचे (arg->v_type == VAR_STRING)
	    {
		char_u *name = get_scriptlocal_funcname(arg->vval.v_string);
		सचे (name != रिक्त)
		{
		    res.cb_name = name;
		    res.cb_free_name = TRUE;
		}
	    }
	    func_ref(res.cb_name);
	}
	अञ्ञथा सचे (arg->v_type == VAR_सङ्ख्या && arg->vval.v_सङ्ख्या == 0)
	    res.cb_name = (char_u *)"";
	अञ्ञथा
	    r = FAIL;

	सचे (r == FAIL)
	{
	    emsg(_(e_invalid_callback_argument));
	    res.cb_name = रिक्त;
	}
    }
    पटिनिवत्त res;
}

/*
 * Copy a callback into a typval_T.
 */
    शून्य
put_callback(callback_T *cb, typval_T *tv)
{
    सचे (cb->cb_partial != रिक्त)
    {
	tv->v_type = VAR_PARTIAL;
	tv->vval.v_partial = cb->cb_partial;
	++tv->vval.v_partial->pt_refcount;
    }
    अञ्ञथा
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
    पटिनिवत्त cb->cb_partial || cb->cb_free_name;
}

/*
 * Make a copy of "src" into "dest", allocating the function name if needed,
 * without incrementing the refcount.
 */
    शून्य
set_callback(callback_T *dest, callback_T *src)
{
    *dest = *src;
    सचे (!does_callback_own_cb_name(src))
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
    सचे (dest->cb_partial != रिक्त)
    {
	dest->cb_name = src->cb_name;
	dest->cb_free_name = FALSE;
	++dest->cb_partial->pt_refcount;
    }
    अञ्ञथा
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

    सचे (!in_vim9script() || cb->cb_name == रिक्त
	    || (!cb->cb_free_name
	       && (cb->cb_partial == रिक्त || cb->cb_partial->pt_name == रिक्त)))
	पटिनिवत्त;
    सचे (cb->cb_partial != रिक्त)
	name = cb->cb_partial->pt_name;
    अञ्ञथा
	name = cb->cb_name;
    p = vim_strchr(name, '.');
    सचे (p == रिक्त)
	पटिनिवत्त;

    import = find_imported(name, p - name, FALSE);
    सचे (import == रिक्त || !SCRIPT_ID_VALID(import->imp_sid))
	पटिनिवत्त;

    scriptitem_T *si = SCRIPT_ITEM(import->imp_sid);
    सचे (si->sn_autoload_prefix == रिक्त)
	पटिनिवत्त;

    char_u *newname = concat_str(si->sn_autoload_prefix, p + 1);
    सचे (newname == रिक्त)
	पटिनिवत्त;

    सचे (cb->cb_partial != रिक्त)
    {
	सचे (cb->cb_name == cb->cb_partial->pt_name)
	    cb->cb_name = newname;
	vim_free(cb->cb_partial->pt_name);
	cb->cb_partial->pt_name = newname;
    }
    अञ्ञथा
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
    सचे (callback->cb_partial != रिक्त)
    {
	partial_unref(callback->cb_partial);
	callback->cb_partial = रिक्त;
    }
    अञ्ञथा सचे (callback->cb_name != रिक्त)
	func_unref(callback->cb_name);
    सचे (callback->cb_free_name)
    {
	vim_free(callback->cb_name);
	callback->cb_free_name = FALSE;
    }
    callback->cb_name = रिक्त;
}

#endif // FEAT_EVAL
