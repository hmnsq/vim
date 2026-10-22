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

#অন্তর্ভাব "vim.h"

#যদি defined(FEAT_EVAL)

স্থির dictitem_T	globvars_var;		// variable used for g:
স্থির dict_T		globvardict;		// Dictionary with g: variables
#define globvarht globvardict.dv_hashtab

/*
 * Old Vim variables such as "v:version" are also available without the "v:".
 * Also in functions.  We need a special hashtable for them.
 */
স্থির hashtab_T	compat_hashtab;

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

প্রকারলেখ কাঠামো vimvar vimvar_T;

স্থির কাঠামো vimvar
{
    অক্ষর	*vv_name;	// name of variable, without v:
    dictitem16_T vv_di;		// value and name for key (max 16 chars!)
    type_T	*vv_type;	// type or NULL
    অক্ষর	vv_flags;	// VV_COMPAT, VV_RO, VV_RO_SBX
} vimvars[VV_LEN] =
{
    // The order here must match the VV_ defines in vim.h!
    // Initializing a union does not work, leave tv.vval empty to get zero's.
    {VV_NAME("count",		 VAR_সংখ্যা), ফাঁকা, VV_COMPAT+VV_RO},
    {VV_NAME("count1",		 VAR_সংখ্যা), ফাঁকা, VV_RO},
    {VV_NAME("prevcount",	 VAR_সংখ্যা), ফাঁকা, VV_RO},
    {VV_NAME("errmsg",		 VAR_STRING), ফাঁকা, VV_COMPAT},
    {VV_NAME("warningmsg",	 VAR_STRING), ফাঁকা, 0},
    {VV_NAME("statusmsg",	 VAR_STRING), ফাঁকা, 0},
    {VV_NAME("shell_error",	 VAR_সংখ্যা), ফাঁকা, VV_COMPAT+VV_RO},
    {VV_NAME("this_session",	 VAR_STRING), ফাঁকা, VV_COMPAT},
    {VV_NAME("version",		 VAR_সংখ্যা), ফাঁকা, VV_COMPAT+VV_RO},
    {VV_NAME("lnum",		 VAR_সংখ্যা), ফাঁকা, VV_RO_SBX},
    {VV_NAME("termresponse",	 VAR_STRING), ফাঁকা, VV_RO},
    {VV_NAME("fname",		 VAR_STRING), ফাঁকা, VV_RO},
    {VV_NAME("lang",		 VAR_STRING), ফাঁকা, VV_RO},
    {VV_NAME("lc_time",		 VAR_STRING), ফাঁকা, VV_RO},
    {VV_NAME("ctype",		 VAR_STRING), ফাঁকা, VV_RO},
    {VV_NAME("charconvert_from", VAR_STRING), ফাঁকা, VV_RO},
    {VV_NAME("charconvert_to",	 VAR_STRING), ফাঁকা, VV_RO},
    {VV_NAME("fname_in",	 VAR_STRING), ফাঁকা, VV_RO},
    {VV_NAME("fname_out",	 VAR_STRING), ফাঁকা, VV_RO},
    {VV_NAME("fname_new",	 VAR_STRING), ফাঁকা, VV_RO},
    {VV_NAME("fname_diff",	 VAR_STRING), ফাঁকা, VV_RO},
    {VV_NAME("cmdarg",		 VAR_STRING), ফাঁকা, VV_RO},
    {VV_NAME("foldstart",	 VAR_সংখ্যা), ফাঁকা, VV_RO_SBX},
    {VV_NAME("foldend",		 VAR_সংখ্যা), ফাঁকা, VV_RO_SBX},
    {VV_NAME("folddashes",	 VAR_STRING), ফাঁকা, VV_RO_SBX},
    {VV_NAME("foldlevel",	 VAR_সংখ্যা), ফাঁকা, VV_RO_SBX},
    {VV_NAME("progname",	 VAR_STRING), ফাঁকা, VV_RO},
    {VV_NAME("servername",	 VAR_STRING), ফাঁকা, VV_RO},
    {VV_NAME("dying",		 VAR_সংখ্যা), ফাঁকা, VV_RO},
    {VV_NAME("exception",	 VAR_STRING), ফাঁকা, VV_RO},
    {VV_NAME("throwpoint",	 VAR_STRING), ফাঁকা, VV_RO},
    {VV_NAME("register",	 VAR_STRING), ফাঁকা, VV_RO},
    {VV_NAME("cmdbang",		 VAR_সংখ্যা), ফাঁকা, VV_RO},
    {VV_NAME("insertmode",	 VAR_STRING), ফাঁকা, VV_RO},
    {VV_NAME("val",		 VAR_UNKNOWN), ফাঁকা, VV_RO},
    {VV_NAME("key",		 VAR_UNKNOWN), ফাঁকা, VV_RO},
    {VV_NAME("profiling",	 VAR_সংখ্যা), ফাঁকা, VV_RO},
    {VV_NAME("fcs_reason",	 VAR_STRING), ফাঁকা, VV_RO},
    {VV_NAME("fcs_choice",	 VAR_STRING), ফাঁকা, 0},
    {VV_NAME("beval_bufnr",	 VAR_সংখ্যা), ফাঁকা, VV_RO},
    {VV_NAME("beval_winnr",	 VAR_সংখ্যা), ফাঁকা, VV_RO},
    {VV_NAME("beval_winid",	 VAR_সংখ্যা), ফাঁকা, VV_RO},
    {VV_NAME("beval_lnum",	 VAR_সংখ্যা), ফাঁকা, VV_RO},
    {VV_NAME("beval_col",	 VAR_সংখ্যা), ফাঁকা, VV_RO},
    {VV_NAME("beval_text",	 VAR_STRING), ফাঁকা, VV_RO},
    {VV_NAME("scrollstart",	 VAR_STRING), ফাঁকা, 0},
    {VV_NAME("swapname",	 VAR_STRING), ফাঁকা, VV_RO},
    {VV_NAME("swapchoice",	 VAR_STRING), ফাঁকা, 0},
    {VV_NAME("swapcommand",	 VAR_STRING), ফাঁকা, VV_RO},
    {VV_NAME("char",		 VAR_STRING), ফাঁকা, 0},
    {VV_NAME("mouse_win",	 VAR_সংখ্যা), ফাঁকা, 0},
    {VV_NAME("mouse_winid",	 VAR_সংখ্যা), ফাঁকা, 0},
    {VV_NAME("mouse_lnum",	 VAR_সংখ্যা), ফাঁকা, 0},
    {VV_NAME("mouse_col",	 VAR_সংখ্যা), ফাঁকা, 0},
    {VV_NAME("operator",	 VAR_STRING), ফাঁকা, VV_RO},
    {VV_NAME("searchforward",	 VAR_সংখ্যা), ফাঁকা, 0},
    {VV_NAME("hlsearch",	 VAR_সংখ্যা), ফাঁকা, 0},
    {VV_NAME("oldfiles",	 VAR_LIST), &t_list_string, 0},
    {VV_NAME("windowid",	 VAR_সংখ্যা), ফাঁকা, VV_RO},
    {VV_NAME("progpath",	 VAR_STRING), ফাঁকা, VV_RO},
    {VV_NAME("completed_item",	 VAR_DICT), &t_dict_string, 0},
    {VV_NAME("option_new",	 VAR_STRING), ফাঁকা, VV_RO},
    {VV_NAME("option_old",	 VAR_STRING), ফাঁকা, VV_RO},
    {VV_NAME("option_oldlocal",	 VAR_STRING), ফাঁকা, VV_RO},
    {VV_NAME("option_oldglobal", VAR_STRING), ফাঁকা, VV_RO},
    {VV_NAME("option_command",	 VAR_STRING), ফাঁকা, VV_RO},
    {VV_NAME("option_type",	 VAR_STRING), ফাঁকা, VV_RO},
    {VV_NAME("errors",		 VAR_LIST), &t_list_string, 0},
    {VV_NAME("false",		 VAR_BOOL), ফাঁকা, VV_RO},
    {VV_NAME("true",		 VAR_BOOL), ফাঁকা, VV_RO},
    {VV_NAME("none",		 VAR_SPECIAL), ফাঁকা, VV_RO},
    {VV_NAME("null",		 VAR_SPECIAL), ফাঁকা, VV_RO},
    {VV_NAME("numbermax",	 VAR_সংখ্যা), ফাঁকা, VV_RO},
    {VV_NAME("numbermin",	 VAR_সংখ্যা), ফাঁকা, VV_RO},
    {VV_NAME("numbersize",	 VAR_সংখ্যা), ফাঁকা, VV_RO},
    {VV_NAME("vim_did_enter",	 VAR_সংখ্যা), ফাঁকা, VV_RO},
    {VV_NAME("testing",		 VAR_সংখ্যা), ফাঁকা, 0},
    {VV_NAME("t_number",	 VAR_সংখ্যা), ফাঁকা, VV_RO},
    {VV_NAME("t_string",	 VAR_সংখ্যা), ফাঁকা, VV_RO},
    {VV_NAME("t_func",		 VAR_সংখ্যা), ফাঁকা, VV_RO},
    {VV_NAME("t_list",		 VAR_সংখ্যা), ফাঁকা, VV_RO},
    {VV_NAME("t_dict",		 VAR_সংখ্যা), ফাঁকা, VV_RO},
    {VV_NAME("t_float",		 VAR_সংখ্যা), ফাঁকা, VV_RO},
    {VV_NAME("t_bool",		 VAR_সংখ্যা), ফাঁকা, VV_RO},
    {VV_NAME("t_none",		 VAR_সংখ্যা), ফাঁকা, VV_RO},
    {VV_NAME("t_job",		 VAR_সংখ্যা), ফাঁকা, VV_RO},
    {VV_NAME("t_channel",	 VAR_সংখ্যা), ফাঁকা, VV_RO},
    {VV_NAME("t_blob",		 VAR_সংখ্যা), ফাঁকা, VV_RO},
    {VV_NAME("t_class",		 VAR_সংখ্যা), ফাঁকা, VV_RO},
    {VV_NAME("t_object",	 VAR_সংখ্যা), ফাঁকা, VV_RO},
    {VV_NAME("termrfgresp",	 VAR_STRING), ফাঁকা, VV_RO},
    {VV_NAME("termrbgresp",	 VAR_STRING), ফাঁকা, VV_RO},
    {VV_NAME("termu7resp",	 VAR_STRING), ফাঁকা, VV_RO},
    {VV_NAME("termstyleresp",	 VAR_STRING), ফাঁকা, VV_RO},
    {VV_NAME("termblinkresp",	 VAR_STRING), ফাঁকা, VV_RO},
    {VV_NAME("event",		 VAR_DICT), ফাঁকা, VV_RO},
    {VV_NAME("versionlong",	 VAR_সংখ্যা), ফাঁকা, VV_RO},
    {VV_NAME("echospace",	 VAR_সংখ্যা), ফাঁকা, VV_RO},
    {VV_NAME("argv",		 VAR_LIST), &t_list_string, VV_RO},
    {VV_NAME("collate",		 VAR_STRING), ফাঁকা, VV_RO},
    {VV_NAME("exiting",		 VAR_SPECIAL), ফাঁকা, VV_RO},
    {VV_NAME("colornames",       VAR_DICT), &t_dict_string, VV_RO},
    {VV_NAME("sizeofint",	 VAR_সংখ্যা), ফাঁকা, VV_RO},
    {VV_NAME("sizeoflong",	 VAR_সংখ্যা), ফাঁকা, VV_RO},
    {VV_NAME("sizeofpointer",	 VAR_সংখ্যা), ফাঁকা, VV_RO},
    {VV_NAME("maxcol",		 VAR_সংখ্যা), ফাঁকা, VV_RO},
    {VV_NAME("python3_version",	 VAR_সংখ্যা), ফাঁকা, VV_RO},
    {VV_NAME("t_typealias",	 VAR_সংখ্যা), ফাঁকা, VV_RO},
    {VV_NAME("t_enum",		 VAR_সংখ্যা), ফাঁকা, VV_RO},
    {VV_NAME("t_enumvalue",	 VAR_সংখ্যা), ফাঁকা, VV_RO},
    {VV_NAME("stacktrace",	 VAR_LIST), &t_list_dict_any, VV_RO},
    {VV_NAME("t_tuple",		 VAR_সংখ্যা), ফাঁকা, VV_RO},
    {VV_NAME("wayland_display",  VAR_STRING), ফাঁকা, VV_RO},
    {VV_NAME("clipmethod",	 VAR_STRING), ফাঁকা, VV_RO},
    {VV_NAME("termda1",		 VAR_STRING), ফাঁকা, VV_RO},
    {VV_NAME("termosc",	 VAR_STRING), ফাঁকা, VV_RO},
    {VV_NAME("vim_did_init",	 VAR_সংখ্যা), ফাঁকা, VV_RO},
    {VV_NAME("clipproviders",	 VAR_DICT), ফাঁকা, VV_RO},
    {VV_NAME("imagebackend",	 VAR_STRING), ফাঁকা, VV_RO}
};

// shorthand
#define vv_tv_type	vv_di.di_tv.v_type
#define vv_nr		vv_di.di_tv.vval.v_সংখ্যা
#define vv_float	vv_di.di_tv.vval.v_float
#define vv_str		vv_di.di_tv.vval.v_string
#define vv_list		vv_di.di_tv.vval.v_list
#define vv_dict		vv_di.di_tv.vval.v_dict
#define vv_blob		vv_di.di_tv.vval.v_blob
#define vv_tv		vv_di.di_tv

স্থির dictitem_T	vimvars_var;		// variable used for v:
স্থির dict_T		vimvardict;		// Dictionary with v: variables
#define vimvarht  vimvardict.dv_hashtab

// for VIM_VERSION_ defines
#অন্তর্ভাব "version.h"

স্থির শূন্য list_glob_vars(পূর্ণসংখ্যা *first);
স্থির শূন্য list_buf_vars(পূর্ণসংখ্যা *first);
স্থির শূন্য list_win_vars(পূর্ণসংখ্যা *first);
স্থির শূন্য list_tab_vars(পূর্ণসংখ্যা *first);
স্থির char_u *list_arg_vars(exarg_T *eap, char_u *arg, পূর্ণসংখ্যা *first);
স্থির char_u *ex_let_one(char_u *arg, typval_T *tv, পূর্ণসংখ্যা copy, পূর্ণসংখ্যা flags, char_u *endchars, char_u *op, পূর্ণসংখ্যা var_idx);
স্থির পূর্ণসংখ্যা do_unlet_var(lval_T *lp, char_u *name_end, exarg_T *eap, পূর্ণসংখ্যা deep, শূন্য *cookie);
স্থির পূর্ণসংখ্যা do_lock_var(lval_T *lp, char_u *name_end, exarg_T *eap, পূর্ণসংখ্যা deep, শূন্য *cookie);
স্থির শূন্য list_one_var(dictitem_T *v, অক্ষর *prefix, পূর্ণসংখ্যা *first);
স্থির শূন্য list_one_var_a(অক্ষর *prefix, char_u *name, পূর্ণসংখ্যা type, char_u *string, পূর্ণসংখ্যা *first);

/*
 * Initialize global and vim special variables
 */
    শূন্য
evalvars_init(শূন্য)
{
    পূর্ণসংখ্যা		    i;
    কাঠামো vimvar   *p;

    init_var_dict(&globvardict, &globvars_var, VAR_DEF_SCOPE);
    init_var_dict(&vimvardict, &vimvars_var, VAR_SCOPE);
    vimvardict.dv_lock = VAR_FIXED;
    hash_init(&compat_hashtab);

    জন্য (i = 0; i < VV_LEN; ++i)
    {
	p = &vimvars[i];
	যদি (p->vv_flags & VV_RO)
	    p->vv_di.di_flags = DI_FLAGS_RO | DI_FLAGS_FIX;
	অন্যথা যদি (p->vv_flags & VV_RO_SBX)
	    p->vv_di.di_flags = DI_FLAGS_RO_SBX | DI_FLAGS_FIX;
	অন্যথা
	    p->vv_di.di_flags = DI_FLAGS_FIX;

	// add to v: scope dict, unless the value is not always available
	যদি (p->vv_tv_type != VAR_UNKNOWN)
	    hash_add(&vimvarht, p->vv_di.di_চাবি, "initialization");
	যদি (p->vv_flags & VV_COMPAT)
	    // add to compat scope dict
	    hash_add(&compat_hashtab, p->vv_di.di_চাবি, "initialization");
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
    set_vim_var_nr(VV_NUMBERSIZE, আকার(varnumber_T) * 8);
    set_vim_var_nr(VV_SIZEOFINT, আকার(পূর্ণসংখ্যা));
    set_vim_var_nr(VV_SIZEOFLONG, আকার(long));
    set_vim_var_nr(VV_SIZEOFPOINTER, আকার(অক্ষর *));
    set_vim_var_nr(VV_MAXCOL, MAXCOL);

    set_vim_var_nr(VV_TYPE_সংখ্যা,  VAR_TYPE_সংখ্যা);
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

#যদি defined(EXITFREE)
/*
 * Free all vim variables information on exit
 */
    শূন্য
evalvars_clear(শূন্য)
{
    পূর্ণসংখ্যা		    i;
    কাঠামো vimvar   *p;

    জন্য (i = 0; i < VV_LEN; ++i)
    {
	p = &vimvars[i];
	যদি (p->vv_di.di_tv.v_type == VAR_STRING)
	    VIM_CLEAR(p->vv_str);
	অন্যথা যদি (p->vv_di.di_tv.v_type == VAR_LIST)
	{
	    list_unref(p->vv_list);
	    p->vv_list = ফাঁকা;
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
    জন্য (i = 1; i <= script_বস্তুসমূহ.ga_len; ++i)
	vars_clear(&SCRIPT_VARS(i));
}
#endif

    পূর্ণসংখ্যা
garbage_collect_globvars(পূর্ণসংখ্যা copyID)
{
    ফেরত set_ref_in_ht(&globvarht, copyID, ফাঁকা, ফাঁকা);
}

    পূর্ণসংখ্যা
garbage_collect_vimvars(পূর্ণসংখ্যা copyID)
{
    ফেরত set_ref_in_ht(&vimvarht, copyID, ফাঁকা, ফাঁকা);
}

    পূর্ণসংখ্যা
garbage_collect_scriptvars(পূর্ণসংখ্যা copyID)
{
    পূর্ণসংখ্যা		    i;
    পূর্ণসংখ্যা		    idx;
    পূর্ণসংখ্যা		    abort = FALSE;
    scriptitem_T    *si;

    জন্য (i = 1; i <= script_বস্তুসমূহ.ga_len; ++i)
    {
	abort = abort || set_ref_in_ht(&SCRIPT_VARS(i), copyID, ফাঁকা, ফাঁকা);

	si = SCRIPT_ITEM(i);
	জন্য (idx = 0; idx < si->sn_var_vals.ga_len; ++idx)
	{
	    svar_T    *sv = ((svar_T *)si->sn_var_vals.ga_তথ্য) + idx;

	    যদি (sv->sv_name != ফাঁকা)
		abort = abort || set_ref_in_item(sv->sv_tv, copyID, ফাঁকা, ফাঁকা, ফাঁকা);
	}
    }

    ফেরত abort;
}

/*
 * Set an internal variable to a string value. Creates the variable if it does
 * not already exist.
 */
    শূন্য
set_internal_string_var(char_u *name, char_u *মান)
{
    char_u	*val;
    typval_T	*tvp;

    val = vim_strsave(মান);
    যদি (val == ফাঁকা)
	ফেরত;

    tvp = alloc_string_tv(val);
    যদি (tvp == ফাঁকা)
	ফেরত;

    set_var(name, tvp, FALSE);
    free_tv(tvp);
}

    পূর্ণসংখ্যা
eval_charconvert(
    char_u	*enc_from,
    char_u	*enc_to,
    char_u	*fname_from,
    char_u	*fname_to)
{
    পূর্ণসংখ্যা		err = FALSE;
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;

    set_vim_var_string(VV_CC_FROM, enc_from, -1);
    set_vim_var_string(VV_CC_TO, enc_to, -1);
    set_vim_var_string(VV_FNAME_IN, fname_from, -1);
    set_vim_var_string(VV_FNAME_OUT, fname_to, -1);
    ctx = get_option_sctx("charconvert");
    যদি (ctx != ফাঁকা)
	current_sctx = *ctx;

    যদি (eval_to_bool(p_ccv, &err, ফাঁকা, FALSE, TRUE))
	err = TRUE;

    set_vim_var_string(VV_CC_FROM, ফাঁকা, -1);
    set_vim_var_string(VV_CC_TO, ফাঁকা, -1);
    set_vim_var_string(VV_FNAME_IN, ফাঁকা, -1);
    set_vim_var_string(VV_FNAME_OUT, ফাঁকা, -1);
    current_sctx = saved_sctx;

    যদি (err)
	ফেরত FAIL;
    ফেরত OK;
}

#যদি defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
    পূর্ণসংখ্যা
eval_printexpr(char_u *fname, char_u *args)
{
    পূর্ণসংখ্যা		err = FALSE;
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;

    set_vim_var_string(VV_FNAME_IN, fname, -1);
    set_vim_var_string(VV_CMDARG, args, -1);
    ctx = get_option_sctx("printexpr");
    যদি (ctx != ফাঁকা)
	current_sctx = *ctx;

    যদি (eval_to_bool(p_pexpr, &err, ফাঁকা, FALSE, TRUE))
	err = TRUE;

    set_vim_var_string(VV_FNAME_IN, ফাঁকা, -1);
    set_vim_var_string(VV_CMDARG, ফাঁকা, -1);
    current_sctx = saved_sctx;

    যদি (err)
    {
	mch_remove(fname);
	ফেরত FAIL;
    }
    ফেরত OK;
}
#endif

#যদি defined(FEAT_DIFF)
    শূন্য
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
    যদি (ctx != ফাঁকা)
	current_sctx = *ctx;

    // errors are ignored
    tv = eval_রাশিমালা_ext(p_dex, ফাঁকা, TRUE);
    free_tv(tv);

    set_vim_var_string(VV_FNAME_IN, ফাঁকা, -1);
    set_vim_var_string(VV_FNAME_NEW, ফাঁকা, -1);
    set_vim_var_string(VV_FNAME_OUT, ফাঁকা, -1);
    current_sctx = saved_sctx;
}

    শূন্য
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
    যদি (ctx != ফাঁকা)
	current_sctx = *ctx;

    // errors are ignored
    tv = eval_রাশিমালা_ext(p_pex, ফাঁকা, TRUE);
    free_tv(tv);

    set_vim_var_string(VV_FNAME_IN, ফাঁকা, -1);
    set_vim_var_string(VV_FNAME_DIFF, ফাঁকা, -1);
    set_vim_var_string(VV_FNAME_OUT, ফাঁকা, -1);
    current_sctx = saved_sctx;
}
#endif

#যদি defined(FEAT_SPELL)
/*
 * Evaluate an expression to a list with suggestions.
 * For the "expr:" part of 'spellsuggest'.
 * Returns NULL when there is an error.
 */
    list_T *
eval_spell_রাশিমালা(char_u *badword, char_u *রাশিমালা)
{
    typval_T	save_val;
    typval_T	rettv;
    list_T	*list = ফাঁকা;
    char_u	*p = skipwhite(রাশিমালা);
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;
    পূর্ণসংখ্যা		r;

    // Set "v:val" to the bad word.
    prepare_vimvar(VV_VAL, &save_val);
    set_vim_var_string(VV_VAL, badword, -1);
    যদি (p_verbose == 0)
	++emsg_off;
    ctx = get_option_sctx("spellsuggest");
    যদি (ctx != ফাঁকা)
	current_sctx = *ctx;

    r = may_call_simple_func(p, &rettv);
    যদি (r == NOTDONE)
	r = eval1(&p, &rettv, &EVALARG_EVALUATE);
    যদি (r == OK)
    {
	যদি (rettv.v_type != VAR_LIST)
	    clear_tv(&rettv);
	অন্যথা
	    list = rettv.vval.v_list;
    }

    যদি (p_verbose == 0)
	--emsg_off;
    clear_tv(get_vim_var_tv(VV_VAL));
    restore_vimvar(VV_VAL, &save_val);
    current_sctx = saved_sctx;

    ফেরত list;
}

/*
 * "list" is supposed to contain two items: a word and a number.  Return the
 * word in "pp" and the number as the return value.
 * Return -1 if anything isn't right.
 * Used to get the good word and score from the eval_spell_expr() result.
 */
    পূর্ণসংখ্যা
get_spellword(list_T *list, char_u **pp)
{
    listitem_T	*li;

    li = list->lv_first;
    যদি (li == ফাঁকা)
	ফেরত -1;
    *pp = tv_get_string(&li->li_tv);

    li = li->li_পরবর্তী;
    যদি (li == ফাঁকা)
	ফেরত -1;
    ফেরত (পূর্ণসংখ্যা)tv_get_সংখ্যা(&li->li_tv);
}
#endif

/*
 * Prepare v: variable "idx" to be used.
 * Save the current typeval in "save_tv" and clear it.
 * When not used yet add the variable to the v: hashtable.
 */
    শূন্য
prepare_vimvar(পূর্ণসংখ্যা idx, typval_T *save_tv)
{
    *save_tv = vimvars[idx].vv_tv;
    vimvars[idx].vv_str = ফাঁকা;  // don't free it now
    যদি (vimvars[idx].vv_tv_type == VAR_UNKNOWN)
	hash_add(&vimvarht, vimvars[idx].vv_di.di_চাবি, "prepare vimvar");
}

/*
 * Restore v: variable "idx" to typeval "save_tv".
 * Note that the v: variable must have been cleared already.
 * When no longer defined, remove the variable from the v: hashtable.
 */
    শূন্য
restore_vimvar(পূর্ণসংখ্যা idx, typval_T *save_tv)
{
    hashitem_T	*hi;

    vimvars[idx].vv_tv = *save_tv;
    যদি (vimvars[idx].vv_tv_type != VAR_UNKNOWN)
	ফেরত;

    hi = hash_find(&vimvarht, vimvars[idx].vv_di.di_চাবি);
    যদি (HASHITEM_EMPTY(hi))
	internal_error("restore_vimvar()");
    অন্যথা
	hash_remove(&vimvarht, hi, "restore vimvar");
}

/*
 * List Vim variables.
 */
    স্থির শূন্য
list_vim_vars(পূর্ণসংখ্যা *first)
{
    list_hashtable_vars(&vimvarht, "v:", FALSE, first);
}

/*
 * List script-local variables, if there is a script.
 */
    স্থির শূন্য
list_script_vars(পূর্ণসংখ্যা *first)
{
    যদি (SCRIPT_ID_VALID(current_sctx.sc_sid))
	list_hashtable_vars(&SCRIPT_VARS(current_sctx.sc_sid),
							   "s:", FALSE, first);
}

/*
 * Return TRUE if "name" starts with "g:", "w:", "t:" or "b:".
 * But only when an identifier character follows.
 */
    পূর্ণসংখ্যা
is_scoped_variable(char_u *name)
{
    ফেরত vim_strchr((char_u *)"gwbt", name[0]) != ফাঁকা
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
eval_one_রাশিমালা_in_str(char_u *p, garray_T *gap, পূর্ণসংখ্যা evaluate)
{
    char_u	*block_start = skipwhite(p + 1);  // skip the opening {
    char_u	*block_end = block_start;
    char_u	*রাশিমালা_val;

    যদি (*block_start == NUL)
    {
	semsg(_(e_missing_close_curly_str), p);
	ফেরত ফাঁকা;
    }
    যদি (skip_রাশিমালা(&block_end, ফাঁকা) == FAIL)
	ফেরত ফাঁকা;
    block_end = skipwhite(block_end);
    যদি (*block_end != '}')
    {
	semsg(_(e_missing_close_curly_str), p);
	ফেরত ফাঁকা;
    }
    যদি (evaluate)
    {
	*block_end = NUL;
	রাশিমালা_val = eval_to_string(block_start, FALSE, FALSE);
	*block_end = '}';
	যদি (রাশিমালা_val == ফাঁকা)
	    ফেরত ফাঁকা;
	ga_concat(gap, রাশিমালা_val);
	vim_free(রাশিমালা_val);
    }

    ফেরত block_end + 1;
}

/*
 * Evaluate all the Vim expressions {expr} in "str" and return the resulting
 * string in allocated memory.  "{{" is reduced to "{" and "}}" to "}".
 * Used for a heredoc assignment.
 * Returns NULL for an error.
 */
    স্থির char_u *
eval_all_রাশিমালা_in_str(char_u *str)
{
    garray_T	ga;
    char_u	*p;

    ga_init2(&ga, 1, 80);
    p = str;

    যখন (*p != NUL)
    {
	char_u	*lit_start;
	পূর্ণসংখ্যা	escaped_brace = FALSE;

	// Look for a block start.
	lit_start = p;
	যখন (*p != '{' && *p != '}' && *p != NUL)
	    ++p;

	যদি (*p != NUL && *p == p[1])
	{
	    // Escaped brace, unescape and continue.
	    // Include the brace in the literal string.
	    ++p;
	    escaped_brace = TRUE;
	}
	অন্যথা যদি (*p == '}')
	{
	    semsg(_(e_stray_closing_curly_str), str);
	    ga_clear(&ga);
	    ফেরত ফাঁকা;
	}

	// Append the literal part.
	ga_concat_len(&ga, lit_start, (size_t)(p - lit_start));

	যদি (*p == NUL)
	    বিরতি;

	যদি (escaped_brace)
	{
	    // Skip the second brace.
	    ++p;
	    চালু;
	}

	// Evaluate the expression and append the result.
	p = eval_one_রাশিমালা_in_str(p, &ga, TRUE);
	যদি (p == ফাঁকা)
	{
	    ga_clear(&ga);
	    ফেরত ফাঁকা;
	}
    }
    ga_append(&ga, NUL);

    ফেরত ga.ga_তথ্য;
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
heredoc_get(exarg_T *eap, char_u *cmd, পূর্ণসংখ্যা script_get, পূর্ণসংখ্যা vim9compile)
{
    char_u	*theline = ফাঁকা;
    char_u	*marker;
    list_T	*l;
    char_u	*p;
    char_u	*str;
    পূর্ণসংখ্যা		marker_indent_len = 0;
    পূর্ণসংখ্যা		text_indent_len = 0;
    char_u	*text_indent = ফাঁকা;
    char_u	dot[] = ".";
    পূর্ণসংখ্যা		comment_char = in_vim9script() ? '#' : '"';
    পূর্ণসংখ্যা		evalstr = FALSE;
    পূর্ণসংখ্যা		eval_failed = FALSE;
    cctx_T	*cctx = vim9compile ? eap->cookie : ফাঁকা;
    পূর্ণসংখ্যা		গুনতি = 0;
    পূর্ণসংখ্যা		heredoc_in_string = FALSE;
    char_u	*সারি_arg = ফাঁকা;
    char_u	*nl_ptr = vim_strchr(cmd, '\n');

    যদি (nl_ptr != ফাঁকা)
    {
	heredoc_in_string = TRUE;
	সারি_arg = nl_ptr + 1;
	*nl_ptr = NUL;
    }
    অন্যথা যদি (eap->ea_getline == ফাঁকা)
    {
	emsg(_(e_cannot_use_heredoc_here));
	ফেরত ফাঁকা;
    }

    // Check for the optional 'trim' word before the marker
    cmd = skipwhite(cmd);

    যখন (TRUE)
    {
	যদি (STRNCMP(cmd, "trim", 4) == 0
		&& (cmd[4] == NUL || VIM_ISWHITE(cmd[4])))
	{
	    cmd = skipwhite(cmd + 4);

	    // Trim the indentation from all the lines in the here document.
	    // The amount of indentation trimmed is the same as the indentation
	    // of the first line after the :let command line.  To find the end
	    // marker the indent of the :let command line is trimmed.
	    p = *eap->cmdlinep;
	    যখন (VIM_ISWHITE(*p))
	    {
		p++;
		marker_indent_len++;
	    }
	    text_indent_len = -1;

	    চালু;
	}
	যদি (STRNCMP(cmd, "eval", 4) == 0
		&& (cmd[4] == NUL || VIM_ISWHITE(cmd[4])))
	{
	    cmd = skipwhite(cmd + 4);
	    evalstr = TRUE;
	    চালু;
	}
	বিরতি;
    }

    // The marker is the next word.
    যদি (*cmd != NUL && *cmd != comment_char)
    {
	marker = skipwhite(cmd);
	p = skiptowhite(marker);
	যদি (*skipwhite(p) != NUL && *skipwhite(p) != comment_char)
	{
	    semsg(_(e_trailing_characters_str), p);
	    ফেরত ফাঁকা;
	}
	*p = NUL;
	যদি (!script_get && vim_islower(*marker))
	{
	    emsg(_(e_marker_cannot_start_with_lower_case_letter));
	    ফেরত ফাঁকা;
	}
    }
    অন্যথা
    {
	// When getting lines for an embedded script, if the marker is missing,
	// accept '.' as the marker.
	যদি (script_get)
	    marker = dot;
	অন্যথা
	{
	    emsg(_(e_missing_marker));
	    ফেরত ফাঁকা;
	}
    }

    l = list_alloc();
    যদি (l == ফাঁকা)
	ফেরত ফাঁকা;

    জন্য (;;)
    {
	পূর্ণসংখ্যা	mi = 0;
	পূর্ণসংখ্যা	ti = 0;

	যদি (heredoc_in_string)
	{
	    char_u	*পরবর্তী_সারি;

	    // heredoc in a string separated by newlines.  Get the next line
	    // from the string.

	    যদি (*সারি_arg == NUL)
	    {
		semsg(_(e_missing_end_marker_str), marker);
		বিরতি;
	    }

	    theline = সারি_arg;
	    পরবর্তী_সারি = vim_strchr(theline, '\n');
	    যদি (পরবর্তী_সারি == ফাঁকা)
		সারি_arg += STRLEN(সারি_arg);
	    অন্যথা
	    {
		*পরবর্তী_সারি = NUL;
		সারি_arg = পরবর্তী_সারি + 1;
	    }
	}
	অন্যথা
	{
	    vim_free(theline);
	    theline = eap->ea_getline(NUL, eap->cookie, 0, FALSE);
	    যদি (theline == ফাঁকা)
	    {
		semsg(_(e_missing_end_marker_str), marker);
		বিরতি;
	    }
	}

	// with "trim": skip the indent matching the :let line to find the
	// marker
	যদি (marker_indent_len > 0
		&& STRNCMP(theline, *eap->cmdlinep, marker_indent_len) == 0)
	    mi = marker_indent_len;
	যদি (STRCMP(marker, theline + mi) == 0)
	    বিরতি;

	// If expression evaluation failed in the heredoc, then skip till the
	// end marker.
	যদি (eval_failed)
	    চালু;

	যদি (text_indent_len == -1 && *theline != NUL)
	{
	    // set the text indent from the first line.
	    p = theline;
	    text_indent_len = 0;
	    যখন (VIM_ISWHITE(*p))
	    {
		p++;
		text_indent_len++;
	    }
	    text_indent = vim_strnsave(theline, text_indent_len);
	}
	// with "trim": skip the indent matching the first line
	যদি (text_indent != ফাঁকা)
	    জন্য (ti = 0; ti < text_indent_len; ++ti)
		যদি (theline[ti] != text_indent[ti])
		    বিরতি;

	str = theline + ti;
	যদি (vim9compile)
	{
	    যদি (compile_all_রাশিমালা_in_str(str, evalstr, cctx) == FAIL)
	    {
		vim_free(theline);
		vim_free(text_indent);
		list_free(l);
		ফেরত FAIL;
	    }
	    গুনতি++;
	}
	অন্যথা
	{
	    পূর্ণসংখ্যা	    free_str = FALSE;

	    যদি (evalstr && !eap->skip)
	    {
		str = eval_all_রাশিমালা_in_str(str);
		যদি (str == ফাঁকা)
		{
		    // expression evaluation failed
		    eval_failed = TRUE;
		    চালু;
		}
		free_str = TRUE;
	    }

	    যদি (list_append_string(l, str, -1) == FAIL)
	    {
		যদি (free_str)
		    vim_free(str);
		বিরতি;
	    }
	    যদি (free_str)
		vim_free(str);
	}
    }
    যদি (heredoc_in_string)
	// Next command follows the heredoc in the string.
	eap->nextcmd = সারি_arg;
    অন্যথা
	vim_free(theline);
    vim_free(text_indent);

    যদি (vim9compile && cctx->ctx_skip != SKIP_YES && !eval_failed)
	generate_NEWLIST(cctx, গুনতি, FALSE);

    যদি (eval_failed)
    {
	// expression evaluation in the heredoc failed
	list_free(l);
	ফেরত ফাঁকা;
    }
    ফেরত l;
}

/*
 * Vim9 variable declaration:
 * ":var name"
 * ":var name: type"
 * ":var name = expr"
 * ":var name: type = expr"
 * etc.
 */
    শূন্য
ex_var(exarg_T *eap)
{
    char_u *p = eap->cmd;
    পূর্ণসংখ্যা	    has_var;

    যদি (!in_vim9script())
    {
	semsg(_(e_str_cannot_be_used_in_legacy_vim_script), ":var");
	ফেরত;
    }
    has_var = checkforcmd_noparen(&p, "var", 3);
    যদি (current_sctx.sc_sid == 0 && has_var)
    {
	emsg(_(e_cannot_declare_variable_on_command_সারি));
	ফেরত;
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
    শূন্য
ex_let(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    char_u	*রাশিমালা = ফাঁকা;
    typval_T	rettv;
    পূর্ণসংখ্যা		var_গুনতি = 0;
    পূর্ণসংখ্যা		semicolon = 0;
    char_u	op[4];
    char_u	*argend;
    পূর্ণসংখ্যা		first = TRUE;
    পূর্ণসংখ্যা		concat;
    পূর্ণসংখ্যা		has_assign;
    পূর্ণসংখ্যা		flags = 0;
    পূর্ণসংখ্যা		vim9script = in_vim9script();

    যদি (eap->cmdidx == CMD_final && !vim9script)
    {
	// In legacy Vim script ":final" is short for ":finally".
	ex_finally(eap);
	ফেরত;
    }
    যদি (eap->cmdidx == CMD_let && vim9script)
    {
	emsg(_(e_cannot_use_let_in_vim9_script));
	ফেরত;
    }

    যদি (eap->cmdidx == CMD_const)
	flags |= ASSIGN_CONST;
    অন্যথা যদি (eap->cmdidx == CMD_final)
	flags |= ASSIGN_FINAL;

    // Vim9 assignment without ":let", ":const" or ":final"
    যদি (eap->arg == eap->cmd)
	flags |= ASSIGN_NO_DECL;

    argend = skip_var_list(arg, TRUE, &var_গুনতি, &semicolon, FALSE);
    যদি (argend == ফাঁকা)
	ফেরত;
    রাশিমালা = skipwhite(argend);
    concat = রাশিমালা[0] == '.'
	&& ((রাশিমালা[1] == '=' && in_old_script(2))
		|| (রাশিমালা[1] == '.' && রাশিমালা[2] == '='));
    has_assign =  *রাশিমালা == '=' || (vim_strchr((char_u *)"+-*/%", *রাশিমালা) != ফাঁকা
							    && রাশিমালা[1] == '=');
    যদি (!has_assign && !concat)
    {
	// ":let" without "=": list variables
	যদি (*arg == '[')
	    emsg(_(e_invalid_argument));
	অন্যথা যদি (রাশিমালা[0] == '.' && রাশিমালা[1] == '=')
	    emsg(_(e_dot_equal_not_supported_with_script_version_two));
	অন্যথা যদি (!ends_excmd2(eap->cmd, arg))
	{
	    যদি (vim9script)
	    {
		যদি (!ends_excmd2(eap->cmd, skipwhite(argend)))
		    semsg(_(e_trailing_characters_str), argend);
		অন্যথা
		    // Vim9 declaration ":var name: type"
		    arg = vim9_declare_scriptvar(eap, arg);
	    }
	    অন্যথা
	    {
		// ":let var1 var2" - list values
		arg = list_arg_vars(eap, arg, &first);
	    }
	}
	অন্যথা যদি (!eap->skip)
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
	ফেরত;
    }

    যদি (রাশিমালা[0] == '=' && রাশিমালা[1] == '<' && রাশিমালা[2] == '<')
    {
	list_T	*l = ফাঁকা;
	long	cur_lnum = SOURCING_LNUM;

	// :let text =<< [trim] [eval] END
	// :var text =<< [trim] [eval] END
	যদি (vim9script && !eap->skip && (!VIM_ISWHITE(রাশিমালা[-1])
						 || !IS_WHITE_OR_NUL(রাশিমালা[3])))
	    semsg(_(e_white_space_required_before_and_after_str_at_str),
								  "=<<", রাশিমালা);
	অন্যথা
	    l = heredoc_get(eap, রাশিমালা + 3, FALSE, FALSE);

	যদি (l != ফাঁকা)
	{
	    rettv_list_set(&rettv, l);
	    যদি (!eap->skip)
	    {
		// errors are for the assignment, not the end marker
		SOURCING_LNUM = cur_lnum;
		op[0] = '=';
		op[1] = NUL;
		(শূন্য)ex_let_vars(eap->arg, &rettv, FALSE, semicolon, var_গুনতি,
								flags, op);
	    }
	    clear_tv(&rettv);
	}
	ফেরত;
    }

    evalarg_T   evalarg;
    পূর্ণসংখ্যা	    len = 1;

    CLEAR_FIELD(rettv);

    পূর্ণসংখ্যা cur_lnum;

    op[0] = '=';
    op[1] = NUL;
    যদি (*রাশিমালা != '=')
    {
	যদি (vim9script && (flags & ASSIGN_NO_DECL) == 0)
	{
	    // +=, /=, etc. require an existing variable
	    semsg(_(e_cannot_use_operator_on_new_variable_str), eap->arg);
	}
	অন্যথা যদি (vim_strchr((char_u *)"+-*/%.", *রাশিমালা) != ফাঁকা)
	{
	    op[0] = *রাশিমালা;   // +=, -=, *=, /=, %= or .=
	    ++len;
	    যদি (রাশিমালা[0] == '.' && রাশিমালা[1] == '.') // ..=
	    {
		++রাশিমালা;
		++len;
	    }
	}
	রাশিমালা += 2;
    }
    অন্যথা
	++রাশিমালা;

    যদি (vim9script && !eap->skip && (!VIM_ISWHITE(*argend)
					       || !IS_WHITE_OR_NUL(*রাশিমালা)))
    {
	vim_strncpy(op, রাশিমালা - len, len);
	semsg(_(e_white_space_required_before_and_after_str_at_str),
							       op, argend);
    }

    যদি (eap->skip)
	++emsg_skip;
    fill_evalarg_from_eap(&evalarg, eap, eap->skip);
    রাশিমালা = skipwhite_and_linebreak(রাশিমালা, &evalarg);
    cur_lnum = SOURCING_LNUM;
    পূর্ণসংখ্যা eval_res = eval0(রাশিমালা, &rettv, eap, &evalarg);
    যদি (eap->skip)
	--emsg_skip;
    clear_evalarg(&evalarg, eap);

    // Restore the line number so that any type error is given for the
    // declaration, not the expression.
    SOURCING_LNUM = cur_lnum;

    যদি (!eap->skip && eval_res != FAIL)
	(শূন্য)ex_let_vars(eap->arg, &rettv, FALSE, semicolon, var_গুনতি,
								    flags, op);
    যদি (eval_res != FAIL)
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
    পূর্ণসংখ্যা
ex_let_vars(
    char_u	*arg_start,
    typval_T	*tv,
    পূর্ণসংখ্যা		copy,		// copy values from "tv", don't move
    পূর্ণসংখ্যা		semicolon,	// from skip_var_list()
    পূর্ণসংখ্যা		var_গুনতি,	// from skip_var_list()
    পূর্ণসংখ্যা		flags,		// ASSIGN_FINAL, ASSIGN_CONST, etc.
    char_u	*op)
{
    char_u	*arg = arg_start;
    list_T	*l;
    tuple_T	*tuple = ফাঁকা;
    পূর্ণসংখ্যা		i;
    পূর্ণসংখ্যা		var_idx = 0;
    listitem_T	*item = ফাঁকা;
    typval_T	ltv;
    পূর্ণসংখ্যা		is_list = tv->v_type == VAR_LIST;
    পূর্ণসংখ্যা		idx;

    যদি (tv->v_type == VAR_VOID)
    {
	emsg(_(e_cannot_use_void_মান));
	ফেরত FAIL;
    }
    যদি (*arg != '[')
    {
	// ":let var = expr" or ":for var in list"
	যদি (ex_let_one(arg, tv, copy, flags, op, op, var_idx) == ফাঁকা)
	    ফেরত FAIL;
	ফেরত OK;
    }

    // ":let [v1, v2] = list" or ":for [v1, v2] in listlist"
    // or
    // ":let [v1, v2] = tuple" or ":for [v1, v2] in tupletuple"
    যদি (tv->v_type != VAR_LIST && tv->v_type != VAR_TUPLE)
    {
	emsg(_(e_list_or_tuple_required));
	ফেরত FAIL;
    }
    যদি (is_list)
    {
	l = tv->vval.v_list;
	যদি (l == ফাঁকা)
	{
	    emsg(_(e_list_required));
	    ফেরত FAIL;
	}
	i = list_len(l);
    }
    অন্যথা
    {
	tuple = tv->vval.v_tuple;
	যদি (tuple == ফাঁকা)
	{
	    emsg(_(e_tuple_required));
	    ফেরত FAIL;
	}
	i = tuple_len(tuple);
    }

    যদি (semicolon == 0 && var_গুনতি < i)
    {
	emsg(_(is_list ? e_less_targets_than_list_বস্তুসমূহ
					: e_less_targets_than_tuple_বস্তুসমূহ));
	ফেরত FAIL;
    }
    যদি (var_গুনতি - semicolon > i)
    {
	emsg(_(is_list ? e_more_targets_than_list_বস্তুসমূহ
					: e_more_targets_than_tuple_বস্তুসমূহ));
	ফেরত FAIL;
    }

    যদি (is_list)
    {
	CHECK_LIST_MATERIALIZE(l);
	item = l->lv_first;
    }
    অন্যথা
	idx = 0;

    যখন (*arg != ']')
    {
	arg = skipwhite(arg + 1);
	++var_idx;
	arg = ex_let_one(arg, is_list ? &item->li_tv : TUPLE_ITEM(tuple, idx),
			 TRUE, flags | ASSIGN_UNPACK, (char_u *)",;]", op,
			 var_idx);
	যদি (is_list)
	    item = item->li_পরবর্তী;
	অন্যথা
	    idx++;
	যদি (arg == ফাঁকা)
	    ফেরত FAIL;

	arg = skipwhite(arg);
	যদি (*arg == ';')
	{
	    // Put the rest of the list or tuple (may be empty) in the var
	    // after ';'.  Create a new list or tuple for this.
	    যদি (is_list)
	    {
		// Put the rest of the list (may be empty) in the var
		// after ';'.  Create a new list for this.
		l = list_alloc();
		যদি (l == ফাঁকা)
		    ফেরত FAIL;

		// list
		যখন (item != ফাঁকা)
		{
		    list_append_tv(l, &item->li_tv);
		    item = item->li_পরবর্তী;
		}

		ltv.v_type = VAR_LIST;
		ltv.v_lock = 0;
		ltv.vval.v_list = l;
		l->lv_refcount = 1;
	    }
	    অন্যথা
	    {
		tuple_T	*new_tuple = tuple_alloc();
		যদি (new_tuple == ফাঁকা)
		    ফেরত FAIL;

		// Put the rest of the tuple (may be empty) in the var
		// after ';'.  Create a new tuple for this.
		যখন (idx < TUPLE_LEN(tuple))
		{
		    typval_T    new_tv;

		    copy_tv(TUPLE_ITEM(tuple, idx), &new_tv);
		    যদি (tuple_append_tv(new_tuple, &new_tv) == FAIL)
		    {
			clear_tv(&new_tv);
			tuple_unref(new_tuple);
			ফেরত FAIL;
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
	    যদি (arg == ফাঁকা)
		ফেরত FAIL;
	    বিরতি;
	}
	অন্যথা যদি (*arg != ',' && *arg != ']')
	{
	    internal_error("ex_let_vars()");
	    ফেরত FAIL;
	}
    }

    ফেরত OK;
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
    পূর্ণসংখ্যা		include_type,
    পূর্ণসংখ্যা		*var_গুনতি,
    পূর্ণসংখ্যা		*semicolon,
    পূর্ণসংখ্যা		silent)
{
    char_u	*p, *s;

    যদি (*arg == '[')
    {
	// "[var, var]": find the matching ']'.
	p = arg;
	জন্য (;;)
	{
	    p = skipwhite(p + 1);	// skip whites after '[', ';' or ','
	    s = skip_var_one(p, include_type);
	    যদি (s == p)
	    {
		যদি (!silent)
		    semsg(_(e_invalid_argument_str), p);
		ফেরত ফাঁকা;
	    }
	    ++*var_গুনতি;

	    p = skipwhite(s);
	    যদি (*p == ']')
		বিরতি;
	    অন্যথা যদি (*p == ';')
	    {
		যদি (*semicolon == 1)
		{
		    যদি (!silent)
			emsg(_(e_double_semicolon_in_list_of_variables));
		    ফেরত ফাঁকা;
		}
		*semicolon = 1;
	    }
	    অন্যথা যদি (*p != ',')
	    {
		যদি (!silent)
		    semsg(_(e_invalid_argument_str), p);
		ফেরত ফাঁকা;
	    }
	}
	ফেরত p + 1;
    }

    ফেরত skip_var_one(arg, include_type);
}

/*
 * Skip one (assignable) variable name, including @r, $VAR, &option, d.key,
 * l[idx].
 * In Vim9 script also skip over ": type" if "include_type" is TRUE.
 */
    char_u *
skip_var_one(char_u *arg, পূর্ণসংখ্যা include_type)
{
    char_u	*end;
    পূর্ণসংখ্যা		vim9 = in_vim9script();

    যদি (*arg == '@' && arg[1] != NUL)
	ফেরত arg + 2;

    // termcap option name may have non-alpha characters
    যদি (STRNCMP(arg, "&t_", 3) == 0 && arg[3] != NUL && arg[4] != NUL)
	ফেরত arg + 5;

    end = find_name_end(*arg == '$' || *arg == '&' ? arg + 1 : arg,
				   ফাঁকা, ফাঁকা, FNE_INCL_BR | FNE_CHECK_START);

    // "a: type" is declaring variable "a" with a type, not "a:".
    // Same for "s: type".
    যদি (vim9 && end == arg + 2 && end[-1] == ':')
	--end;

    যদি (include_type && vim9)
    {
	যদি (*skipwhite(end) == ':')
	    end = skip_type(skipwhite(skipwhite(end) + 1), FALSE);
    }
    ফেরত end;
}

/*
 * List variables for hashtab "ht" with prefix "prefix".
 * If "empty" is TRUE also list NULL strings as empty strings.
 */
    শূন্য
list_hashtable_vars(
    hashtab_T	*ht,
    অক্ষর	*prefix,
    পূর্ণসংখ্যা		empty,
    পূর্ণসংখ্যা		*first)
{
    hashitem_T	*hi;
    dictitem_T	*di;
    পূর্ণসংখ্যা		todo;
    char_u	buf[IOSIZE];

    পূর্ণসংখ্যা save_ht_flags = ht->ht_flags;
    ht->ht_flags |= HTFLAGS_FROZEN;

    todo = (পূর্ণসংখ্যা)ht->ht_used;
    জন্য (hi = ht->ht_array; todo > 0 && !got_int; ++hi)
    {
	যদি (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    di = HI2DI(hi);

	    // apply :filter /pat/ to variable name
	    vim_strncpy((char_u *)buf, (char_u *)prefix, IOSIZE - 1);
	    vim_strcat((char_u *)buf, di->di_চাবি, IOSIZE);
	    যদি (message_filtered(buf))
		চালু;

	    যদি (empty || di->di_tv.v_type != VAR_STRING
					   || di->di_tv.vval.v_string != ফাঁকা)
		list_one_var(di, prefix, first);
	}
    }

    ht->ht_flags = save_ht_flags;
}

/*
 * List global variables.
 */
    স্থির শূন্য
list_glob_vars(পূর্ণসংখ্যা *first)
{
    list_hashtable_vars(&globvarht, "", TRUE, first);
}

/*
 * List buffer variables.
 */
    স্থির শূন্য
list_buf_vars(পূর্ণসংখ্যা *first)
{
    list_hashtable_vars(&curbuf->b_vars->dv_hashtab, "b:", TRUE, first);
}

/*
 * List window variables.
 */
    স্থির শূন্য
list_win_vars(পূর্ণসংখ্যা *first)
{
    list_hashtable_vars(&curwin->w_vars->dv_hashtab, "w:", TRUE, first);
}

/*
 * List tab page variables.
 */
    স্থির শূন্য
list_tab_vars(পূর্ণসংখ্যা *first)
{
    list_hashtable_vars(&curtab->tp_vars->dv_hashtab, "t:", TRUE, first);
}

/*
 * List variables in "arg".
 */
    স্থির char_u *
list_arg_vars(exarg_T *eap, char_u *arg, পূর্ণসংখ্যা *first)
{
    পূর্ণসংখ্যা		error = FALSE;
    পূর্ণসংখ্যা		len;
    char_u	*name;
    char_u	*name_start;
    char_u	*arg_subsc;
    char_u	*tofree;
    typval_T    tv;

    যখন (!ends_excmd2(eap->cmd, arg) && !got_int)
    {
	যদি (error || eap->skip)
	{
	    arg = find_name_end(arg, ফাঁকা, ফাঁকা, FNE_INCL_BR | FNE_CHECK_START);
	    যদি (!VIM_ISWHITE(*arg) && !ends_excmd(*arg))
	    {
		emsg_severe = TRUE;
		যদি (!did_emsg)
		    semsg(_(e_trailing_characters_str), arg);
		বিরতি;
	    }
	}
	অন্যথা
	{
	    // get_name_len() takes care of expanding curly braces
	    name_start = name = arg;
	    len = get_name_len(&arg, &tofree, TRUE, TRUE);
	    যদি (len <= 0)
	    {
		// This is mainly to keep test 49 working: when expanding
		// curly braces fails overrule the exception error message.
		যদি (len < 0 && !aborting())
		{
		    emsg_severe = TRUE;
		    semsg(_(e_invalid_argument_str), arg);
		    বিরতি;
		}
		error = TRUE;
	    }
	    অন্যথা
	    {
		arg = skipwhite(arg);
		যদি (tofree != ফাঁকা)
		    name = tofree;
		যদি (eval_variable(name, len, 0, &tv, ফাঁকা,
						     EVAL_VAR_VERBOSE) == FAIL)
		    error = TRUE;
		অন্যথা
		{
		    // handle d.key, l[idx], f(expr)
		    arg_subsc = arg;
		    যদি (handle_subscript(&arg, name_start, &tv,
					      &EVALARG_EVALUATE, TRUE) == FAIL)
			error = TRUE;
		    অন্যথা
		    {
			যদি (arg == arg_subsc && len == 2 && name[1] == ':')
			{
			    নির্বাচন (*name)
			    {
				ক্ষেত্র 'g': list_glob_vars(first); বিরতি;
				ক্ষেত্র 'b': list_buf_vars(first); বিরতি;
				ক্ষেত্র 'w': list_win_vars(first); বিরতি;
				ক্ষেত্র 't': list_tab_vars(first); বিরতি;
				ক্ষেত্র 'v': list_vim_vars(first); বিরতি;
				ক্ষেত্র 's': list_script_vars(first); বিরতি;
				ক্ষেত্র 'l': list_func_vars(first); বিরতি;
				পূর্বনির্ধারিত:
					  semsg(_(e_cant_list_variables_for_str), name);
			    }
			}
			অন্যথা
			{
			    char_u	numbuf[NUMBUFLEN];
			    char_u	*tf;
			    পূর্ণসংখ্যা		c;
			    char_u	*s;

			    s = echo_string(&tv, &tf, numbuf, 0);
			    c = *arg;
			    *arg = NUL;
			    list_one_var_a("",
				    arg == arg_subsc ? name : name_start,
				    tv.v_type,
				    s == ফাঁকা ? (char_u *)"" : s,
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

    ফেরত arg;
}

/*
 * Set an environment variable, part of ex_let_one().
 */
    স্থির char_u *
ex_let_env(
    char_u	*arg,
    typval_T	*tv,
    পূর্ণসংখ্যা		flags,
    char_u	*endchars,
    char_u	*op)
{
    char_u	*arg_end = ফাঁকা;
    char_u	*name;
    পূর্ণসংখ্যা		len;

    যদি ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					 && (flags & ASSIGN_FOR_LOOP) == 0)
    {
	emsg(_(e_cannot_lock_environment_variable));
	ফেরত ফাঁকা;
    }

    // Find the end of the name.
    ++arg;
    name = arg;
    len = get_env_len(&arg);
    যদি (len == 0)
	semsg(_(e_invalid_argument_str), name - 1);
    অন্যথা
    {
	যদি (op != ফাঁকা && vim_strchr((char_u *)"+-*/%", *op) != ফাঁকা)
	    semsg(_(e_wrong_variable_type_for_str_equal), op);
	অন্যথা যদি (endchars != ফাঁকা
			      && vim_strchr(endchars, *skipwhite(arg)) == ফাঁকা)
	    emsg(_(e_unexpected_characters_in_let));
	অন্যথা যদি (!check_secure() && !check_restricted())
	{
	    char_u	*tofree = ফাঁকা;
	    পূর্ণসংখ্যা		c1 = name[len];
	    char_u	*p;

	    name[len] = NUL;
	    p = tv_get_string_chk(tv);
	    যদি (p != ফাঁকা && op != ফাঁকা && *op == '.')
	    {
		পূর্ণসংখ্যা	mustfree = FALSE;
		char_u  *s = vim_getenv(name, &mustfree);

		যদি (s != ফাঁকা)
		{
		    p = tofree = concat_str(s, p);
		    যদি (mustfree)
			vim_free(s);
		}
	    }
	    যদি (p != ফাঁকা)
	    {
		vim_setenv_ext(name, p);
		arg_end = arg;
	    }
	    name[len] = c1;
	    vim_free(tofree);
	}
    }
    ফেরত arg_end;
}

/*
 * Set an option, part of ex_let_one().
 */
    স্থির char_u *
ex_let_option(
    char_u	*arg,
    typval_T	*tv,
    পূর্ণসংখ্যা		flags,
    char_u	*endchars,
    char_u	*op)
{
    char_u	*p;
    পূর্ণসংখ্যা		scope;
    char_u	*arg_end = ফাঁকা;

    যদি ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					 && (flags & ASSIGN_FOR_LOOP) == 0)
    {
	emsg(_(e_cannot_lock_option));
	ফেরত ফাঁকা;
    }

    // Find the end of the name.
    p = find_option_end(&arg, &scope);
    যদি (p == ফাঁকা || (endchars != ফাঁকা
			       && vim_strchr(endchars, *skipwhite(p)) == ফাঁকা))
    {
	emsg(_(e_unexpected_characters_in_let));
	ফেরত ফাঁকা;
    }

    পূর্ণসংখ্যা		c1;
    long	n = 0;
    getoption_T	opt_type;
    long	numval;
    char_u	*stringval = ফাঁকা;
    char_u	*s = ফাঁকা;
    পূর্ণসংখ্যা		failed = FALSE;
    পূর্ণসংখ্যা		opt_p_flags;
    char_u	*tofree = ফাঁকা;
    char_u	numbuf[NUMBUFLEN];

    c1 = *p;
    *p = NUL;

    opt_type = get_option_মান(arg, &numval, &stringval, &opt_p_flags, scope);
    যদি (opt_type == gov_unknown && arg[0] != 't' && arg[1] != '_')
    {
	semsg(_(e_unknown_option_str_2), arg);
	যাও theend;
    }
    যদি (op != ফাঁকা && *op != '='
	&& (((opt_type == gov_bool || opt_type == gov_সংখ্যা) && *op == '.')
	    || (opt_type == gov_string && *op != '.')))
    {
	semsg(_(e_wrong_variable_type_for_str_equal), op);
	যাও theend;
    }

    যদি ((opt_type == gov_bool
		|| opt_type == gov_সংখ্যা
		|| opt_type == gov_hidden_bool
		|| opt_type == gov_hidden_সংখ্যা)
		     && (tv->v_type != VAR_STRING || !in_vim9script()))
    {
	যদি (opt_type == gov_bool || opt_type == gov_hidden_bool)
	    // bool, possibly hidden
	    n = (long)tv_get_bool_chk(tv, &failed);
	অন্যথা
	    // number, possibly hidden
	    n = (long)tv_get_সংখ্যা_chk(tv, &failed);
	যদি (failed)
	    যাও theend;
    }

    যদি ((opt_p_flags & P_FUNC) && (tv->v_type == VAR_PARTIAL
						    || tv->v_type == VAR_FUNC))
    {
	// If the option can be set to a function reference or a lambda
	// and the passed value is a function reference, then convert it to
	// the name (string) of the function reference.
	s = tv2string(tv, &tofree, numbuf, 0);
	যদি (s == ফাঁকা)
	    যাও theend;
    }
    // Avoid setting a string option to the text "v:false" or similar.
    // In Vim9 script also don't convert a number to string.
    অন্যথা যদি (tv->v_type != VAR_BOOL && tv->v_type != VAR_SPECIAL
		     && (!in_vim9script() || tv->v_type != VAR_সংখ্যা))
    {
	s = tv_get_string_chk(tv);
	যদি (s == ফাঁকা)
	    যাও theend;
    }
    অন্যথা যদি (opt_type == gov_string || opt_type == gov_hidden_string)
    {
	emsg(_(e_string_required));
	যাও theend;
    }

    যদি (op != ফাঁকা && *op != '=')
    {
	// number, in legacy script also bool
	যদি (opt_type == gov_সংখ্যা
				 || (opt_type == gov_bool && !in_vim9script()))
	{
	    নির্বাচন (*op)
	    {
		ক্ষেত্র '+': n = numval + n; বিরতি;
		ক্ষেত্র '-': n = numval - n; বিরতি;
		ক্ষেত্র '*': n = numval * n; বিরতি;
		ক্ষেত্র '/': n = (long)অঙ্ক_divide(numval, n, &failed); বিরতি;
		ক্ষেত্র '%': n = (long)অঙ্ক_modulus(numval, n, &failed); বিরতি;
	    }
	    s = ফাঁকা;
	    যদি (failed)
		যাও theend;
	}
	অন্যথা যদি (opt_type == gov_string && stringval != ফাঁকা && s != ফাঁকা)
	{
	    // string
	    s = concat_str(stringval, s);
	    vim_free(stringval);
	    stringval = s;
	}
    }

    অক্ষর *err = set_option_মান(arg, n, s, scope);
    arg_end = p;
    যদি (err != ফাঁকা)
	emsg(_(err));

theend:
    *p = c1;
    vim_free(stringval);
    vim_free(tofree);
    ফেরত arg_end;
}

/*
 * Set a register, part of ex_let_one().
 */
    স্থির char_u *
ex_let_register(
    char_u	*arg,
    typval_T	*tv,
    পূর্ণসংখ্যা		flags,
    char_u	*endchars,
    char_u	*op)
{
    char_u	*arg_end = ফাঁকা;

    যদি ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					 && (flags & ASSIGN_FOR_LOOP) == 0)
    {
	emsg(_(e_cannot_lock_register));
	ফেরত ফাঁকা;
    }
    ++arg;
    যদি (op != ফাঁকা && vim_strchr((char_u *)"+-*/%", *op) != ফাঁকা)
	semsg(_(e_wrong_variable_type_for_str_equal), op);
    অন্যথা যদি (endchars != ফাঁকা
			  && vim_strchr(endchars, *skipwhite(arg + 1)) == ফাঁকা)
	emsg(_(e_unexpected_characters_in_let));
    অন্যথা
    {
	char_u	*ptofree = ফাঁকা;
	char_u	*p;

	p = tv_get_string_chk(tv);
	যদি (p != ফাঁকা && op != ফাঁকা && *op == '.')
	{
	    char_u  *s = get_reg_contents(*arg == '@'
						  ? '"' : *arg, GREG_রাশিমালা_SRC);

	    যদি (s != ফাঁকা)
	    {
		p = ptofree = concat_str(s, p);
		vim_free(s);
	    }
	}
	যদি (p != ফাঁকা)
	{
	    write_reg_contents(*arg == '@' ? '"' : *arg, p, -1, FALSE);
	    arg_end = arg + 1;
	}
	vim_free(ptofree);
    }
    ফেরত arg_end;
}

/*
 * Set one item of ":let var = expr" or ":let [v1, v2] = list" to its value.
 * Returns a pointer to the char just after the var name.
 * Returns NULL if there is an error.
 */
    স্থির char_u *
ex_let_one(
    char_u	*arg,		// points to variable name
    typval_T	*tv,		// value to assign to variable
    পূর্ণসংখ্যা		copy,		// copy value from "tv"
    পূর্ণসংখ্যা		flags,		// ASSIGN_CONST, ASSIGN_FINAL, etc.
    char_u	*endchars,	// valid chars after variable name  or NULL
    char_u	*op,		// "+", "-", "."  or NULL
    পূর্ণসংখ্যা		var_idx)	// variable index for "let [a, b] = list"
{
    char_u	*arg_end = ফাঁকা;

    যদি (in_vim9script() && (flags & (ASSIGN_NO_DECL | ASSIGN_DECL)) == 0
			&& (flags & (ASSIGN_CONST | ASSIGN_FINAL)) == 0
				  && vim_strchr((char_u *)"$@&", *arg) != ফাঁকা)
    {
	vim9_declare_error(arg);
	ফেরত ফাঁকা;
    }

    যদি (check_typval_is_মান(tv) == FAIL)
	ফেরত ফাঁকা;

    // In Vim9 script an environment variable and a register only take a
    // String.  "@#" also takes a buffer number.
    যদি (in_vim9script() && (*arg == '$' || *arg == '@')
	    && !(*arg == '@' && arg[1] == '#' && tv->v_type == VAR_সংখ্যা
					       && (op == ফাঁকা || *op == '=')))
    {
	where_T	where = WHERE_INIT;

	যদি (check_typval_type(&t_string, tv, where) == FAIL)
	    ফেরত ফাঁকা;
    }

    যদি (*arg == '$')
    {
	// ":let $VAR = expr": Set environment variable.
	ফেরত ex_let_env(arg, tv, flags, endchars, op);
    }
    অন্যথা যদি (*arg == '&')
    {
	// ":let &option = expr": Set option value.
	// ":let &l:option = expr": Set local option value.
	// ":let &g:option = expr": Set global option value.
	// ":for &ts in range(8)": Set option value for for loop
	ফেরত ex_let_option(arg, tv, flags, endchars, op);
    }
    অন্যথা যদি (*arg == '@')
    {
	// ":let @r = expr": Set register contents.
	ফেরত ex_let_register(arg, tv, flags, endchars, op);
    }
    অন্যথা যদি (eval_isnamec1(*arg) || *arg == '{')
    {
	lval_T	lv;
	char_u	*p;
	পূর্ণসংখ্যা	lval_flags = (flags & (ASSIGN_NO_DECL | ASSIGN_DECL))
							     ? GLV_NO_DECL : 0;
	lval_flags |= (flags & ASSIGN_FOR_LOOP) ? GLV_FOR_LOOP : 0;
	যদি (op != ফাঁকা && *op != '=')
	    lval_flags |= GLV_ASSIGN_WITH_OP;

	// ":let var = expr": Set internal variable.
	// ":let var: type = expr": Set internal variable with type.
	// ":let {expr} = expr": Idem, name made with curly braces
	p = get_lval(arg, tv, &lv, FALSE, FALSE, lval_flags, FNE_CHECK_START);
	যদি (p != ফাঁকা && lv.ll_name != ফাঁকা)
	{
	    যদি (endchars != ফাঁকা && vim_strchr(endchars,
					   *skipwhite(lv.ll_name_end)) == ফাঁকা)
	    {
		emsg(_(e_unexpected_characters_in_let));
	    }
	    অন্যথা
	    {
		set_var_lval(&lv, p, tv, copy, flags, op, var_idx);
		arg_end = lv.ll_name_end;
	    }
	}
	clear_lval(&lv);
    }
    অন্যথা
	semsg(_(e_invalid_argument_str), arg);

    ফেরত arg_end;
}

/*
 * ":unlet[!] var1 ... " command.
 */
    শূন্য
ex_unlet(exarg_T *eap)
{
    ex_unletlock(eap, eap->arg, 0, eap->forceit ? GLV_QUIET : 0, do_unlet_var, ফাঁকা);
}

/*
 * ":lockvar" and ":unlockvar" commands
 */
    শূন্য
ex_lockvar(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    পূর্ণসংখ্যা		deep = 2;

    যদি (eap->forceit)
	deep = -1;
    অন্যথা যদি (vim_isdigit(*arg))
    {
	deep = getdigits(&arg);
	arg = skipwhite(arg);
    }

    ex_unletlock(eap, arg, deep, 0, do_lock_var, ফাঁকা);
}

/*
 * ":unlet", ":lockvar" and ":unlockvar" are quite similar.
 * Also used for Vim9 script.  "callback" is invoked as:
 *	callback(&lv, name_end, eap, deep, cookie)
 */
    শূন্য
ex_unletlock(
    exarg_T	*eap,
    char_u	*argstart,
    পূর্ণসংখ্যা		deep,
    পূর্ণসংখ্যা		glv_flags,
    পূর্ণসংখ্যা		(*callback)(lval_T *, char_u *, exarg_T *, পূর্ণসংখ্যা, শূন্য *),
    শূন্য	*cookie)
{
    char_u	*arg = argstart;
    char_u	*name_end;
    পূর্ণসংখ্যা		error = FALSE;
    lval_T	lv;

    করো
    {
	যদি (*arg == '$')
	{
	    lv.ll_name = arg;
	    lv.ll_tv = ফাঁকা;
	    ++arg;
	    যদি (get_env_len(&arg) == 0)
	    {
		semsg(_(e_invalid_argument_str), arg - 1);
		ফেরত;
	    }
	    যদি (!error && !eap->skip
			      && callback(&lv, arg, eap, deep, cookie) == FAIL)
		error = TRUE;
	    name_end = arg;
	}
	অন্যথা
	{
	    // Parse the name and find the end.
	    name_end = get_lval(arg, ফাঁকা, &lv, TRUE, eap->skip || error,
				     glv_flags | GLV_NO_DECL, FNE_CHECK_START);
	    যদি (lv.ll_name == ফাঁকা)
		error = TRUE;	    // error but continue parsing
	    যদি (name_end == ফাঁকা || (!VIM_ISWHITE(*name_end)
						    && !ends_excmd(*name_end)))
	    {
		যদি (name_end != ফাঁকা)
		{
		    emsg_severe = TRUE;
		    semsg(_(e_trailing_characters_str), name_end);
		}
		যদি (!(eap->skip || error))
		    clear_lval(&lv);
		বিরতি;
	    }

	    যদি (!error && !eap->skip
			 && callback(&lv, name_end, eap, deep, cookie) == FAIL)
		error = TRUE;

	    যদি (!eap->skip)
		clear_lval(&lv);
	}

	arg = skipwhite(name_end);
    } যখন (!ends_excmd2(name_end, arg));

    set_nextcmd(eap, arg);
}

    স্থির পূর্ণসংখ্যা
do_unlet_var(
    lval_T	*lp,
    char_u	*name_end,
    exarg_T	*eap,
    পূর্ণসংখ্যা		deep UNUSED,
    শূন্য	*cookie UNUSED)
{
    পূর্ণসংখ্যা		forceit = eap->forceit;
    পূর্ণসংখ্যা		ret = OK;
    পূর্ণসংখ্যা		cc;

    যদি (lp->ll_tv == ফাঁকা)
    {
	cc = *name_end;
	*name_end = NUL;

	// Environment variable, normal name or expanded name.
	যদি (*lp->ll_name == '$')
	    vim_unsetenv_ext(lp->ll_name + 1);
	অন্যথা যদি (do_unlet(lp->ll_name, forceit) == FAIL)
	    ret = FAIL;
	*name_end = cc;
    }
    অন্যথা যদি ((lp->ll_list != ফাঁকা
		 && মান_check_lock(lp->ll_list->lv_lock, lp->ll_name, FALSE))
	    || (lp->ll_dict != ফাঁকা
		&& মান_check_lock(lp->ll_dict->dv_lock, lp->ll_name, FALSE)))
	ফেরত FAIL;
    অন্যথা যদি (lp->ll_range)
	list_unlet_range(lp->ll_list, lp->ll_li, lp->ll_n1,
						    !lp->ll_empty2, lp->ll_n2);
    অন্যথা যদি (lp->ll_list != ফাঁকা)
	// unlet a List item.
	listitem_remove(lp->ll_list, lp->ll_li);
    অন্যথা যদি (lp->ll_dict != ফাঁকা)
	// unlet a Dictionary item.
	dictitem_remove(lp->ll_dict, lp->ll_di, "unlet");
    অন্যথা
    {
	semsg(_(e_cannot_unlet_imported_item_str), lp->ll_name);
	ফেরত FAIL;
    }

    ফেরত ret;
}

/*
 * Unlet one item or a range of items from a list.
 * Return OK or FAIL.
 */
    শূন্য
list_unlet_range(
	list_T	    *l,
	listitem_T  *li_first,
	long	    n1_arg,
	পূর্ণসংখ্যা	    has_n2,
	long	    n2)
{
    listitem_T  *li = li_first;
    পূর্ণসংখ্যা		n1 = n1_arg;

    // Delete a range of List items.
    li = li_first;
    n1 = n1_arg;
    যখন (li != ফাঁকা && (!has_n2 || n2 >= n1))
    {
	listitem_T *পরবর্তী = li->li_পরবর্তী;

	listitem_remove(l, li);
	li = পরবর্তী;
	++n1;
    }
}
/*
 * "unlet" a variable.  Return OK if it existed, FAIL if not.
 * When "forceit" is TRUE don't complain if the variable doesn't exist.
 */
    পূর্ণসংখ্যা
do_unlet(char_u *name, পূর্ণসংখ্যা forceit)
{
    hashtab_T	*ht;
    hashitem_T	*hi;
    char_u	*varname = ফাঁকা;  // init to shut up gcc
    dict_T	*d;
    dictitem_T	*di;

    // can't :unlet a script variable in Vim9 script
    যদি (in_vim9script() && check_vim9_unlet(name) == FAIL)
	ফেরত FAIL;

    ht = find_var_ht(name, &varname);

    // can't :unlet a script variable in Vim9 script from a function
    যদি (ht == get_script_local_ht()
	    && SCRIPT_ID_VALID(current_sctx.sc_sid)
	    && SCRIPT_ITEM(current_sctx.sc_sid)->sn_version
							 == SCRIPT_VERSION_VIM9
	    && check_vim9_unlet(name) == FAIL)
	ফেরত FAIL;

    যদি (ht != ফাঁকা && *varname != NUL)
    {
	d = get_current_funccal_dict(ht);
	যদি (d == ফাঁকা)
	{
	    যদি (ht == &globvarht)
		d = &globvardict;
	    অন্যথা যদি (ht == &compat_hashtab)
		d = &vimvardict;
	    অন্যথা
	    {
		di = find_var_in_ht(ht, *name, (char_u *)"", FALSE);
		d = di == ফাঁকা ? ফাঁকা : di->di_tv.vval.v_dict;
	    }
	    যদি (d == ফাঁকা)
	    {
		internal_error("do_unlet()");
		ফেরত FAIL;
	    }
	}
	hi = hash_find(ht, varname);
	যদি (HASHITEM_EMPTY(hi))
	    hi = find_hi_in_scoped_ht(name, &ht);
	যদি (hi != ফাঁকা && !HASHITEM_EMPTY(hi))
	{
	    di = HI2DI(hi);
	    যদি (var_check_fixed(di->di_flags, name, FALSE)
		    || var_check_ro(di->di_flags, name, FALSE)
		    || মান_check_lock(d->dv_lock, name, FALSE)
		    || check_hashtab_frozen(ht, "unlet"))
		ফেরত FAIL;

	    delete_var(ht, hi);
	    ফেরত OK;
	}
    }
    যদি (forceit)
	ফেরত OK;
    semsg(_(e_no_such_variable_str), name);
    ফেরত FAIL;
}

    স্থির শূন্য
report_lockvar_member(অক্ষর *বার্তা, lval_T *lp)
{
    পূর্ণসংখ্যা did_alloc = FALSE;
    char_u *vname = (char_u *)"";
    char_u *class_name = lp->ll_class != ফাঁকা
				    ? lp->ll_class->class_name.string : (char_u *)"";
    যদি (lp->ll_name != ফাঁকা)
    {
	যদি (lp->ll_name_end == ফাঁকা)
	    vname = lp->ll_name;
	অন্যথা
	{
	    vname = vim_strnsave(lp->ll_name, lp->ll_name_end - lp->ll_name);
	    যদি (vname == ফাঁকা)
		ফেরত;
	    did_alloc = TRUE;
	}
    }
    semsg(_(বার্তা), vname, class_name);
    যদি (did_alloc)
	vim_free(vname);
}

/*
 * Lock or unlock variable indicated by "lp".
 * "deep" is the levels to go (-1 for unlimited);
 * "lock" is TRUE for ":lockvar", FALSE for ":unlockvar".
 */
    স্থির পূর্ণসংখ্যা
do_lock_var(
    lval_T	*lp,
    char_u	*name_end,
    exarg_T	*eap,
    পূর্ণসংখ্যা		deep,
    শূন্য	*cookie UNUSED)
{
    পূর্ণসংখ্যা		lock = eap->cmdidx == CMD_lockvar;
    পূর্ণসংখ্যা		ret = OK;
    পূর্ণসংখ্যা		cc;
    dictitem_T	*di;

#ifdef LOG_LOCKVAR
    ch_log(ফাঁকা, "LKVAR: do_lock_var(): name %s, is_root %d", lp->ll_name, lp->ll_is_মূল);
#endif

    যদি (lp->ll_tv == ফাঁকা)
    {
	cc = *name_end;
	*name_end = NUL;
	যদি (*lp->ll_name == '$')
	{
	    semsg(_(e_cannot_lock_or_unlock_variable_str), lp->ll_name);
	    ret = FAIL;
	}
	অন্যথা
	{
	    // Normal name or expanded name.
	    di = find_var(lp->ll_name, ফাঁকা, TRUE);
	    যদি (di == ফাঁকা)
	    {
		যদি (in_vim9script())
		    semsg(_(e_cannot_find_variable_to_unlock_str),
								  lp->ll_name);
		ret = FAIL;
	    }
	    অন্যথা
	    {
		যদি ((di->di_flags & DI_FLAGS_FIX)
			    && di->di_tv.v_type != VAR_DICT
			    && di->di_tv.v_type != VAR_LIST)
		{
		    // For historic reasons this error is not given for a list
		    // or dict.  E.g., the b: dict could be locked/unlocked.
		    semsg(_(e_cannot_lock_or_unlock_variable_str), lp->ll_name);
		    ret = FAIL;
		}
		অন্যথা
		{
		    যদি (in_vim9script())
		    {
			svar_T  *sv = find_typval_in_script(&di->di_tv,
								     0, FALSE);

			যদি (sv != ফাঁকা && sv->sv_const != 0)
			{
			    semsg(_(e_cannot_change_readonly_variable_str),
								  lp->ll_name);
			    ret = FAIL;
			}
		    }

		    যদি (ret == OK)
		    {
			যদি (lock)
			    di->di_flags |= DI_FLAGS_LOCK;
			অন্যথা
			    di->di_flags &= ~DI_FLAGS_LOCK;
			যদি (deep != 0)
			    item_lock(&di->di_tv, deep, lock, FALSE);
		    }
		}
	    }
	}
	*name_end = cc;
    }
    অন্যথা যদি (deep == 0 && lp->ll_object == ফাঁকা && lp->ll_class == ফাঁকা)
    {
	// nothing to do
    }
    অন্যথা যদি (lp->ll_is_মূল)
	// (un)lock the item.
	item_lock(lp->ll_tv, deep, lock, FALSE);
    অন্যথা যদি (lp->ll_range)
    {
	listitem_T    *li = lp->ll_li;

	// (un)lock a range of List items.
	যখন (li != ফাঁকা && (lp->ll_empty2 || lp->ll_n2 >= lp->ll_n1))
	{
	    item_lock(&li->li_tv, deep, lock, FALSE);
	    li = li->li_পরবর্তী;
	    ++lp->ll_n1;
	}
    }
    অন্যথা যদি (lp->ll_list != ফাঁকা)
	// (un)lock a List item.
	item_lock(&lp->ll_li->li_tv, deep, lock, FALSE);
    অন্যথা যদি (lp->ll_object != ফাঁকা)  // This check must be before ll_class.
    {
	// (un)lock an object variable.
	report_lockvar_member(e_cannot_lock_object_variable_str, lp);
	ret = FAIL;
    }
    অন্যথা যদি (lp->ll_class != ফাঁকা)
    {
	// (un)lock a class variable.
	report_lockvar_member(e_cannot_lock_class_variable_str, lp);
	ret = FAIL;
    }
    অন্যথা
    {
	// (un)lock a Dictionary item.
	যদি (lp->ll_di == ফাঁকা)
	{
	    emsg(_(e_dictionary_required));
	    ret = FAIL;
	}
	অন্যথা
	    item_lock(&lp->ll_di->di_tv, deep, lock, FALSE);
    }

    ফেরত ret;
}

/*
 * Lock or unlock an item.  "deep" is nr of levels to go.
 * When "check_refcount" is TRUE do not lock a list or dict with a reference
 * count larger than 1.
 */
    শূন্য
item_lock(typval_T *tv, পূর্ণসংখ্যা deep, পূর্ণসংখ্যা lock, পূর্ণসংখ্যা check_refcount)
{
    স্থির পূর্ণসংখ্যা	recurse = 0;
    list_T	*l;
    listitem_T	*li;
    dict_T	*d;
    blob_T	*b;
    hashitem_T	*hi;
    পূর্ণসংখ্যা		todo;

#ifdef LOG_LOCKVAR
    ch_log(ফাঁকা, "LKVAR: item_lock(): type %s", vartype_name(tv->v_type));
#endif

    যদি (recurse >= DICT_MAXNEST)
    {
	emsg(_(e_variable_nested_too_deep_for_unlock));
	ফেরত;
    }
    যদি (deep == 0)
	ফেরত;
    ++recurse;

    // lock/unlock the item itself
    যদি (lock)
	tv->v_lock |= VAR_LOCKED;
    অন্যথা
	tv->v_lock &= ~VAR_LOCKED;

    নির্বাচন (tv->v_type)
    {
	ক্ষেত্র VAR_UNKNOWN:
	ক্ষেত্র VAR_ANY:
	ক্ষেত্র VAR_VOID:
	ক্ষেত্র VAR_সংখ্যা:
	ক্ষেত্র VAR_BOOL:
	ক্ষেত্র VAR_STRING:
	ক্ষেত্র VAR_FUNC:
	ক্ষেত্র VAR_PARTIAL:
	ক্ষেত্র VAR_FLOAT:
	ক্ষেত্র VAR_SPECIAL:
	ক্ষেত্র VAR_JOB:
	ক্ষেত্র VAR_CHANNEL:
	ক্ষেত্র VAR_INSTR:
	ক্ষেত্র VAR_CLASS:
	ক্ষেত্র VAR_OBJECT:
	ক্ষেত্র VAR_TYPEALIAS:
	    বিরতি;

	ক্ষেত্র VAR_BLOB:
	    যদি ((b = tv->vval.v_blob) != ফাঁকা
				    && !(check_refcount && b->bv_refcount > 1))
	    {
		যদি (lock)
		    b->bv_lock |= VAR_LOCKED;
		অন্যথা
		    b->bv_lock &= ~VAR_LOCKED;
	    }
	    বিরতি;
	ক্ষেত্র VAR_LIST:
	    যদি ((l = tv->vval.v_list) != ফাঁকা
				    && !(check_refcount && l->lv_refcount > 1))
	    {
		যদি (lock)
		    l->lv_lock |= VAR_LOCKED;
		অন্যথা
		    l->lv_lock &= ~VAR_LOCKED;
		যদি (deep < 0 || deep > 1)
		{
		    যদি (l->lv_first == &range_list_item)
			l->lv_lock |= VAR_বস্তুসমূহ_LOCKED;
		    অন্যথা
		    {
			// recursive: lock/unlock the items the List contains
			CHECK_LIST_MATERIALIZE(l);
			FOR_ALL_LIST_বস্তুসমূহ(l, li) item_lock(&li->li_tv,
					       deep - 1, lock, check_refcount);
		    }
		}
	    }
	    বিরতি;
	ক্ষেত্র VAR_TUPLE:
	    tuple_lock(tv->vval.v_tuple, deep, lock, check_refcount);
	    বিরতি;
	ক্ষেত্র VAR_DICT:
	    যদি ((d = tv->vval.v_dict) != ফাঁকা
				    && !(check_refcount && d->dv_refcount > 1))
	    {
		যদি (lock)
		    d->dv_lock |= VAR_LOCKED;
		অন্যথা
		    d->dv_lock &= ~VAR_LOCKED;
		যদি (deep < 0 || deep > 1)
		{
		    // recursive: lock/unlock the items the List contains
		    todo = (পূর্ণসংখ্যা)d->dv_hashtab.ht_used;
		    FOR_ALL_HASHTAB_বস্তুসমূহ(&d->dv_hashtab, hi, todo)
		    {
			যদি (!HASHITEM_EMPTY(hi))
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

#যদি defined(FEAT_MENU) && defined(FEAT_MULTI_LANG)
/*
 * Delete all "menutrans_" variables.
 */
    শূন্য
del_menutrans_vars(শূন্য)
{
    hashitem_T	*hi;
    পূর্ণসংখ্যা		todo;

    hash_lock(&globvarht);
    todo = (পূর্ণসংখ্যা)globvarht.ht_used;
    জন্য (hi = globvarht.ht_array; todo > 0 && !got_int; ++hi)
    {
	যদি (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    যদি (STRNCMP(HI2DI(hi)->di_চাবি, "menutrans_", 10) == 0)
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

স্থির char_u	*varnamebuf = ফাঁকা;
স্থির পূর্ণসংখ্যা	varnamebuflen = 0;

/*
 * Function to concatenate a prefix and a variable name.
 */
    char_u *
cat_prefix_varname(পূর্ণসংখ্যা prefix, char_u *name)
{
    পূর্ণসংখ্যা		len;

    len = (পূর্ণসংখ্যা)STRLEN(name) + 3;
    যদি (len > varnamebuflen)
    {
	vim_free(varnamebuf);
	len += 10;			// some additional space
	varnamebuf = alloc(len);
	যদি (varnamebuf == ফাঁকা)
	{
	    varnamebuflen = 0;
	    ফেরত ফাঁকা;
	}
	varnamebuflen = len;
    }
    *varnamebuf = prefix;
    varnamebuf[1] = ':';
    STRCPY(varnamebuf + 2, name);
    ফেরত varnamebuf;
}

/*
 * Function given to ExpandGeneric() to obtain the list of user defined
 * (global/buffer/window/built-in) variable names.
 */
    char_u *
get_user_var_name(expand_T *xp, পূর্ণসংখ্যা idx)
{
    স্থির long_u	gdone;
    স্থির long_u	bdone;
    স্থির long_u	wdone;
    স্থির long_u	tdone;
    স্থির পূর্ণসংখ্যা		vidx;
    স্থির hashitem_T	*hi;
    hashtab_T		*ht;

    যদি (idx == 0)
    {
	gdone = bdone = wdone = vidx = 0;
	tdone = 0;
    }

    // Global variables
    যদি (gdone < globvarht.ht_used)
    {
	যদি (gdone++ == 0)
	    hi = globvarht.ht_array;
	অন্যথা
	    ++hi;
	যখন (HASHITEM_EMPTY(hi))
	    ++hi;
	যদি (STRNCMP("g:", xp->xp_pattern, 2) == 0)
	    ফেরত cat_prefix_varname('g', hi->hi_চাবি);
	ফেরত hi->hi_চাবি;
    }

    // b: variables
    ht = &prevwin_curwin()->w_বাফার->b_vars->dv_hashtab;
    যদি (bdone < ht->ht_used)
    {
	যদি (bdone++ == 0)
	    hi = ht->ht_array;
	অন্যথা
	    ++hi;
	যখন (HASHITEM_EMPTY(hi))
	    ++hi;
	ফেরত cat_prefix_varname('b', hi->hi_চাবি);
    }

    // w: variables
    ht = &prevwin_curwin()->w_vars->dv_hashtab;
    যদি (wdone < ht->ht_used)
    {
	যদি (wdone++ == 0)
	    hi = ht->ht_array;
	অন্যথা
	    ++hi;
	যখন (HASHITEM_EMPTY(hi))
	    ++hi;
	ফেরত cat_prefix_varname('w', hi->hi_চাবি);
    }

    // t: variables
    ht = &curtab->tp_vars->dv_hashtab;
    যদি (tdone < ht->ht_used)
    {
	যদি (tdone++ == 0)
	    hi = ht->ht_array;
	অন্যথা
	    ++hi;
	যখন (HASHITEM_EMPTY(hi))
	    ++hi;
	ফেরত cat_prefix_varname('t', hi->hi_চাবি);
    }

    // v: variables
    যদি (vidx < VV_LEN)
	ফেরত cat_prefix_varname('v', (char_u *)vimvars[vidx++].vv_name);

    VIM_CLEAR(varnamebuf);
    varnamebuflen = 0;
    ফেরত ফাঁকা;
}

    অক্ষর *
get_var_special_name(পূর্ণসংখ্যা nr)
{
    নির্বাচন (nr)
    {
	ক্ষেত্র VVAL_FALSE: ফেরত in_vim9script() ? "false" : "v:false";
	ক্ষেত্র VVAL_TRUE:  ফেরত in_vim9script() ? "true" : "v:true";
	ক্ষেত্র VVAL_NULL:  ফেরত in_vim9script() ? "null" : "v:null";
	ক্ষেত্র VVAL_NONE:  ফেরত "v:none";
    }
    internal_error("get_var_special_name()");
    ফেরত "42";
}

/*
 * Returns the global variable dictionary
 */
    dict_T *
get_globvar_dict(শূন্য)
{
    ফেরত &globvardict;
}

/*
 * Returns the global variable hash table
 */
    hashtab_T *
get_globvar_ht(শূন্য)
{
    ফেরত &globvarht;
}

/*
 * Returns the v: variable dictionary
 */
    dict_T *
get_vimvar_dict(শূন্য)
{
    ফেরত &vimvardict;
}

/*
 * Returns the index of a v:variable.  Negative if not found.
 * Returns DI_ flags in "di_flags".
 */
    পূর্ণসংখ্যা
find_vim_var(char_u *name, পূর্ণসংখ্যা *di_flags)
{
    dictitem_T	    *di = find_var_in_ht(&vimvarht, 0, name, TRUE);
    কাঠামো vimvar   *vv;

    যদি (di == ফাঁকা)
	ফেরত -1;
    *di_flags = di->di_flags;
    vv = (কাঠামো vimvar *)((অক্ষর *)di - offsetof(vimvar_T, vv_di));
    ফেরত (পূর্ণসংখ্যা)(vv - vimvars);
}


/*
 * Set type of v: variable to "type".
 */
    শূন্য
set_vim_var_type(পূর্ণসংখ্যা idx, vartype_T type)
{
    vimvars[idx].vv_tv_type = type;
}

/*
 * Set number v: variable to "val".
 * Note that this does not set the type, use set_vim_var_type() for that.
 */
    শূন্য
set_vim_var_nr(পূর্ণসংখ্যা idx, varnumber_T val)
{
    vimvars[idx].vv_nr = val;
}

    অক্ষর *
get_vim_var_name(পূর্ণসংখ্যা idx)
{
    ফেরত vimvars[idx].vv_name;
}

/*
 * Get typval_T v: variable value.
 */
    typval_T *
get_vim_var_tv(পূর্ণসংখ্যা idx)
{
    ফেরত &vimvars[idx].vv_tv;
}

    type_T *
get_vim_var_type(পূর্ণসংখ্যা idx, garray_T *type_list)
{
    যদি (vimvars[idx].vv_type != ফাঁকা)
	ফেরত vimvars[idx].vv_type;
    ফেরত typval2type_vimvar(&vimvars[idx].vv_tv, type_list);
}

/*
 * Add what getinfo() reports for the v: variable "name", without the "v:", to
 * "d".  Returns FAIL when there is no such variable.
 */
    পূর্ণসংখ্যা
vim_var_info(char_u *name, dict_T *d)
{
    পূর্ণসংখ্যা		di_flags;
    পূর্ণসংখ্যা		idx = find_vim_var(name, &di_flags);
    garray_T	type_list;
    অক্ষর	*tofree;

    যদি (idx < 0)
	ফেরত FAIL;
    vim_snprintf((অক্ষর *)IObuff, IOSIZE, "v:%s", vimvars[idx].vv_name);
    dict_add_string(d, "name", IObuff);
    dict_add_bool(d, "available", TRUE);
    ga_init2(&type_list, আকার(type_T *), 10);
    অক্ষর *type = type_name(get_vim_var_type(idx, &type_list), &tofree);
    dict_add_string(d, "type", (char_u *)type);
    vim_free(tofree);
    clear_type_list(&type_list);
    dict_add_bool(d, "readonly", (vimvars[idx].vv_flags & VV_RO) != 0);
    dict_add_bool(d, "compat", (vimvars[idx].vv_flags & VV_COMPAT) != 0);
    ফেরত OK;
}

/*
 * Set v: variable to "tv".  Only accepts the same type.
 * Takes over the value of "tv".
 */
    পূর্ণসংখ্যা
set_vim_var_tv(পূর্ণসংখ্যা idx, typval_T *tv)
{
    যদি (vimvars[idx].vv_tv_type != tv->v_type)
    {
	emsg(_(e_type_mismatch_for_v_variable));
	clear_tv(tv);
	ফেরত FAIL;
    }
    // VV_RO is also checked when compiling, but let's check here as well.
    যদি (vimvars[idx].vv_flags & VV_RO)
    {
	semsg(_(e_cannot_change_readonly_variable_str), vimvars[idx].vv_name);
	ফেরত FAIL;
    }
    যদি (sandbox && (vimvars[idx].vv_flags & VV_RO_SBX))
    {
	semsg(_(e_cannot_set_variable_in_sandbox_str), vimvars[idx].vv_name);
	ফেরত FAIL;
    }
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_di.di_tv = *tv;
    ফেরত OK;
}

/*
 * Get number v: variable value.
 */
    varnumber_T
get_vim_var_nr(পূর্ণসংখ্যা idx)
{
    ফেরত vimvars[idx].vv_nr;
}

/*
 * Get string v: variable value.  Uses a static buffer, can only be used once.
 * If the String variable has never been set, return an empty string.
 * Never returns NULL;
 */
    char_u *
get_vim_var_str(পূর্ণসংখ্যা idx)
{
    ফেরত tv_get_string(&vimvars[idx].vv_tv);
}

/*
 * Get List v: variable value.  Caller must take care of reference count when
 * needed.
 */
    list_T *
get_vim_var_list(পূর্ণসংখ্যা idx)
{
    ফেরত vimvars[idx].vv_list;
}

/*
 * Get Dict v: variable value.  Caller must take care of reference count when
 * needed.
 */
    dict_T *
get_vim_var_dict(পূর্ণসংখ্যা idx)
{
    ফেরত vimvars[idx].vv_dict;
}

/*
 * Set v:char to character "c".
 */
    শূন্য
set_vim_var_char(পূর্ণসংখ্যা c)
{
    char_u  buf[MB_MAXBYTES + 1];
    size_t  buflen;

    যদি (has_mbyte)
	buflen = (*mb_char2bytes)(c, buf);
    অন্যথা
    {
	buf[0] = c;
	buflen = 1;
    }
    buf[buflen] = NUL;

    set_vim_var_string(VV_CHAR, buf, (পূর্ণসংখ্যা)buflen);
}

/*
 * Set v:count to "count" and v:count1 to "count1".
 * When "set_prevcount" is TRUE first set v:prevcount from v:count.
 */
    শূন্য
set_vcount(
    long	গুনতি,
    long	count1,
    পূর্ণসংখ্যা		set_prevcount)
{
    যদি (set_prevcount)
	vimvars[VV_PREVCOUNT].vv_nr = vimvars[VV_গুনতি].vv_nr;
    vimvars[VV_গুনতি].vv_nr = গুনতি;
    vimvars[VV_COUNT1].vv_nr = count1;
}

/*
 * Save variables that might be changed as a side effect.  Used when executing
 * a timer callback.
 */
    শূন্য
save_vimvars(vimvars_save_T *vvsave)
{
    vvsave->vv_prevcount = vimvars[VV_PREVCOUNT].vv_nr;
    vvsave->vv_গুনতি = vimvars[VV_গুনতি].vv_nr;
    vvsave->vv_count1 = vimvars[VV_COUNT1].vv_nr;
}

/*
 * Restore variables saved by save_vimvars().
 */
    শূন্য
restore_vimvars(vimvars_save_T *vvsave)
{
    vimvars[VV_PREVCOUNT].vv_nr = vvsave->vv_prevcount;
    vimvars[VV_গুনতি].vv_nr = vvsave->vv_গুনতি;
    vimvars[VV_COUNT1].vv_nr = vvsave->vv_count1;
}

/*
 * Set string v: variable to a copy of "val". If 'copy' is FALSE, then set the
 * value.
 */
    শূন্য
set_vim_var_string(
    পূর্ণসংখ্যা		idx,
    char_u	*val,
    পূর্ণসংখ্যা		len)	    // length of "val" to use or -1 (whole string)
{
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_tv_type = VAR_STRING;
    যদি (val == ফাঁকা)
	vimvars[idx].vv_str = ফাঁকা;
    অন্যথা যদি (len == -1)
	vimvars[idx].vv_str = vim_strsave(val);
    অন্যথা
	vimvars[idx].vv_str = vim_strnsave(val, len);
}

    শূন্য
set_vim_var_string_direct(
    পূর্ণসংখ্যা		idx,
    char_u	*val)
{
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_tv_type = VAR_STRING;

    vimvars[idx].vv_str = val;
}

/*
 * Set List v: variable to "val".
 */
    শূন্য
set_vim_var_list(পূর্ণসংখ্যা idx, list_T *val)
{
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_tv_type = VAR_LIST;
    vimvars[idx].vv_list = val;
    যদি (val != ফাঁকা)
	++val->lv_refcount;
}

/*
 * Set Dictionary v: variable to "val".
 */
    শূন্য
set_vim_var_dict(পূর্ণসংখ্যা idx, dict_T *val)
{
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_tv_type = VAR_DICT;
    vimvars[idx].vv_dict = val;
    যদি (val == ফাঁকা)
	ফেরত;

    ++val->dv_refcount;
    dict_set_বস্তুসমূহ_ro(val);
}

/*
 * Set the v:argv list.
 */
    শূন্য
set_argv_var(অক্ষর **argv, পূর্ণসংখ্যা argc)
{
    list_T	*l = list_alloc();
    পূর্ণসংখ্যা		i;

    যদি (l == ফাঁকা)
	getout(1);
    l->lv_lock = VAR_FIXED;
    জন্য (i = 0; i < argc; ++i)
    {
	যদি (list_append_string(l, (char_u *)argv[i], -1) == FAIL)
	    getout(1);
	l->lv_u.mat.lv_last->li_tv.v_lock = VAR_FIXED;
    }
    set_vim_var_list(VV_ARGV, l);
}

/*
 * Reset v:register, taking the 'clipboard' setting into account.
 */
    শূন্য
reset_reg_var(শূন্য)
{
    পূর্ণসংখ্যা regname = 0;

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
    শূন্য
set_reg_var(পূর্ণসংখ্যা c)
{
    char_u  regname[2];

    যদি (c == 0 || c == ' ')
	regname[0] = '"';
    অন্যথা
	regname[0] = c;
    regname[1] = NUL;
    // Avoid free/alloc when the value is already right.
    যদি (vimvars[VV_REG].vv_str == ফাঁকা || vimvars[VV_REG].vv_str[0] != c)
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
    যদি (oldval == ফাঁকা)
	ফেরত vimvars[VV_EXCEPTION].vv_str;

    vimvars[VV_EXCEPTION].vv_str = oldval;
    ফেরত ফাঁকা;
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
    যদি (oldval == ফাঁকা)
	ফেরত vimvars[VV_THROWPOINT].vv_str;

    vimvars[VV_THROWPOINT].vv_str = oldval;
    ফেরত ফাঁকা;
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
    যদি (eap == ফাঁকা)
    {
	vim_free(oldval);
	vimvars[VV_CMDARG].vv_str = oldarg;
	ফেরত ফাঁকা;
    }

    যদি (eap->force_bin == FORCE_BIN)
	len = 6;
    অন্যথা যদি (eap->force_bin == FORCE_NOBIN)
	len = 8;
    অন্যথা
	len = 0;

    যদি (eap->read_edit)
	len += 7;

    যদি (eap->force_ff != 0)
	len += 10; // " ++ff=unix"
    যদি (eap->force_enc != 0)
	len += (unsigned)STRLEN(eap->cmd + eap->force_enc) + 7;
    যদি (eap->bad_char != 0)
	len += 7 + 4;  // " ++bad=" + "keep" or "drop"

    newval = alloc(len + 1);
    যদি (newval == ফাঁকা)
	ফেরত ফাঁকা;

    যদি (eap->force_bin == FORCE_BIN)
	sprintf((অক্ষর *)newval, " ++bin");
    অন্যথা যদি (eap->force_bin == FORCE_NOBIN)
	sprintf((অক্ষর *)newval, " ++nobin");
    অন্যথা
	*newval = NUL;

    যদি (eap->read_edit)
	STRCAT(newval, " ++edit");

    যদি (eap->force_ff != 0)
	sprintf((অক্ষর *)newval + STRLEN(newval), " ++ff=%s",
						eap->force_ff == 'u' ? "unix"
						: eap->force_ff == 'd' ? "dos"
						: "mac");
    যদি (eap->force_enc != 0)
	sprintf((অক্ষর *)newval + STRLEN(newval), " ++enc=%s",
					       eap->cmd + eap->force_enc);
    যদি (eap->bad_char == BAD_KEEP)
	STRCPY(newval + STRLEN(newval), " ++bad=keep");
    অন্যথা যদি (eap->bad_char == BAD_DROP)
	STRCPY(newval + STRLEN(newval), " ++bad=drop");
    অন্যথা যদি (eap->bad_char != 0)
	sprintf((অক্ষর *)newval + STRLEN(newval), " ++bad=%c", eap->bad_char);
    vimvars[VV_CMDARG].vv_str = newval;
    ফেরত oldval;
}

/*
 * Get the value of internal variable "name".
 * If "flags" has EVAL_VAR_IMPORT may return a VAR_ANY with v_number set to the
 * imported script ID.
 * Return OK or FAIL.  If OK is returned "rettv" must be cleared.
 */
    পূর্ণসংখ্যা
eval_variable(
    char_u	*name,
    পূর্ণসংখ্যা		len,		// length of "name" or zero
    scid_T	sid,		// script ID for imported item or zero
    typval_T	*rettv,		// NULL when only checking existence
    dictitem_T	**dip,		// non-NULL when typval's dict item is needed
    পূর্ণসংখ্যা		flags)		// EVAL_VAR_ flags
{
    পূর্ণসংখ্যা		ret = OK;
    typval_T	*tv = ফাঁকা;
    পূর্ণসংখ্যা		found = FALSE;
    hashtab_T	*ht = ফাঁকা;
    পূর্ণসংখ্যা		cc = 0;
    type_T	*type = ফাঁকা;

    যদি (len > 0)
    {
	// truncate the name, so that we can use strcmp()
	cc = name[len];
	name[len] = NUL;
    }

    // Check for local variable when debugging.
    যদি ((sid == 0) && (tv = lookup_debug_var(name)) == ফাঁকা)
    {
	// Check for user-defined variables.
	dictitem_T	*v = find_var(name, &ht, flags & EVAL_VAR_NOAUTOLOAD);

	যদি (v != ফাঁকা)
	{
	    tv = &v->di_tv;
	    যদি (dip != ফাঁকা)
		*dip = v;
	}
	অন্যথা
	    ht = ফাঁকা;
    }

    যদি (tv == ফাঁকা && (in_vim9script() || STRNCMP(name, "s:", 2) == 0))
    {
	imported_T  *import = ফাঁকা;
	char_u	    *p = STRNCMP(name, "s:", 2) == 0 ? name + 2 : name;

	যদি (sid == 0)
	    import = find_imported(p, 0, TRUE);

	// imported variable from another script
	যদি (import != ফাঁকা || sid != 0)
	{
	    যদি ((flags & EVAL_VAR_IMPORT) == 0)
	    {
		যদি (SCRIPT_ID_VALID(sid))
		{
		    ht = &SCRIPT_VARS(sid);
		    যদি (ht != ফাঁকা)
		    {
			dictitem_T *v = find_var_in_ht(ht, 0, name,
						  flags & EVAL_VAR_NOAUTOLOAD);

			যদি (v == ফাঁকা)
			    v = find_var_autoload_prefix(name, sid, ফাঁকা, ফাঁকা);

			যদি (v != ফাঁকা)
			{
			    tv = &v->di_tv;
			    যদি (dip != ফাঁকা)
				*dip = v;
			}
			অন্যথা
			    ht = ফাঁকা;
		    }
		}
		অন্যথা
		{
		    যদি (flags & EVAL_VAR_VERBOSE)
			semsg(_(e_expected_dot_after_name_str), name);
		    ret = FAIL;
		}
	    }
	    অন্যথা
	    {
		যদি (rettv != ফাঁকা)
		{
		    // special value that is used in handle_subscript()
		    rettv->v_type = VAR_ANY;
		    rettv->vval.v_সংখ্যা = sid != 0 ? sid : import->imp_sid;
		}
		found = TRUE;
	    }
	}
	অন্যথা যদি (in_vim9script() && (flags & EVAL_VAR_NO_FUNC) == 0)
	{
	    পূর্ণসংখ্যা	    has_g_prefix = STRNCMP(name, "g:", 2) == 0;
	    ufunc_T *ufunc = find_func(name, FALSE);

	    যদি (ufunc != ফাঁকা && cc == '<')
	    {
		// handle generic function
		char_u	*argp = name + len;
		name[len] = cc;
		ufunc = eval_generic_func(ufunc, name, &argp);
		name[len] = NUL;
		যদি (ufunc == ফাঁকা)
		{
		    ret = FAIL;
		    যাও done;
		}
	    }

	    // In Vim9 script we can get a function reference by using the
	    // function name.  For a global non-autoload function "g:" is
	    // required.
	    যদি (ufunc != ফাঁকা && (has_g_prefix
					    || !func_requires_g_prefix(ufunc)))
	    {
		found = TRUE;
		যদি (rettv != ফাঁকা)
		{
		    rettv->v_type = VAR_FUNC;
		    যদি (has_g_prefix)
		    {
			// Keep the "g:", otherwise script-local may be
			// assumed.
			যদি (cc != '<')
			    rettv->vval.v_string = vim_strsave(name);
			অন্যথা
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
		    অন্যথা
			rettv->vval.v_string = vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
		    যদি (rettv->vval.v_string != ফাঁকা)
			func_ref(ufunc->uf_name);
		}
	    }
	}
    }

    যদি (!found)
    {
	যদি (tv == ফাঁকা)
	{
	    যদি (rettv != ফাঁকা && (flags & EVAL_VAR_VERBOSE))
		semsg(_(e_undefined_variable_str), name);
	    ret = FAIL;
	}
	অন্যথা যদি (rettv != ফাঁকা)
	{
	    svar_T  *sv = ফাঁকা;
	    পূর্ণসংখ্যা	    was_assigned = FALSE;

	    যদি (ht != ফাঁকা && ht == get_script_local_ht()
		    && tv != &SCRIPT_SV(current_sctx.sc_sid)->sv_var.di_tv)
	    {
		sv = find_typval_in_script(tv, 0, TRUE);
		যদি (sv != ফাঁকা)
		{
		    type = sv->sv_type;
		    was_assigned = sv->sv_flags & SVFLAG_ASSIGNED;
		}
	    }

	    যদি ((tv->v_type == VAR_TYPEALIAS || tv->v_type == VAR_CLASS)
		    && sid != 0)
	    {
		// type alias or class imported from another script.  Check
		// whether it is exported from the other script.
		sv = find_typval_in_script(tv, sid, TRUE);
		যদি (sv == ফাঁকা)
		{
		    ret = FAIL;
		    যাও done;
		}
		যদি ((sv->sv_flags & SVFLAG_EXPORTED) == 0)
		{
		    semsg(_(e_item_not_exported_in_script_str), name);
		    ret = FAIL;
		    যাও done;
		}
	    }

	    // If a list or tuple or dict variable wasn't initialized and has
	    // meaningful type, do it now.  Not for global variables, they are
	    // not declared.
	    যদি (ht != &globvarht)
	    {
		যদি (tv->v_type == VAR_DICT && tv->vval.v_dict == ফাঁকা
					    && ((type != ফাঁকা && !was_assigned)
							  || !in_vim9script()))
		{
		    tv->vval.v_dict = dict_alloc();
		    যদি (tv->vval.v_dict != ফাঁকা)
		    {
			++tv->vval.v_dict->dv_refcount;
			tv->vval.v_dict->dv_type = alloc_type(type);
			যদি (sv != ফাঁকা)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
		অন্যথা যদি (tv->v_type == VAR_LIST && tv->vval.v_list == ফাঁকা
					    && ((type != ফাঁকা && !was_assigned)
							  || !in_vim9script()))
		{
		    tv->vval.v_list = list_alloc();
		    যদি (tv->vval.v_list != ফাঁকা)
		    {
			++tv->vval.v_list->lv_refcount;
			tv->vval.v_list->lv_type = alloc_type(type);
			যদি (sv != ফাঁকা)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
		অন্যথা যদি (tv->v_type == VAR_TUPLE && tv->vval.v_tuple == ফাঁকা
					    && ((type != ফাঁকা && !was_assigned)
							  || !in_vim9script()))
		{
		    tv->vval.v_tuple = tuple_alloc();
		    যদি (tv->vval.v_tuple != ফাঁকা)
		    {
			++tv->vval.v_tuple->tv_refcount;
			tv->vval.v_tuple->tv_type = alloc_type(type);
			যদি (sv != ফাঁকা)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
		অন্যথা যদি (tv->v_type == VAR_BLOB && tv->vval.v_blob == ফাঁকা
					    && ((type != ফাঁকা && !was_assigned)
							  || !in_vim9script()))
		{
		    tv->vval.v_blob = blob_alloc();
		    যদি (tv->vval.v_blob != ফাঁকা)
		    {
			++tv->vval.v_blob->bv_refcount;
			যদি (sv != ফাঁকা)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
	    }
	    ret = copy_tv(tv, rettv);
	}
    }

done:
    যদি (len > 0)
	name[len] = cc;

    ফেরত ret;
}

/*
 * Get the value of internal variable "name", also handling "import.name".
 * Return OK or FAIL.  If OK is returned "rettv" must be cleared.
 */
    পূর্ণসংখ্যা
eval_variable_import(
    char_u	*name,
    typval_T	*rettv)
{
    char_u  *s = name;
    যখন (ASCII_ISALNUM(*s) || *s == '_')
	++s;
    পূর্ণসংখ্যা	    len = (পূর্ণসংখ্যা)(s - name);

    যদি (eval_variable(name, len, 0, rettv, ফাঁকা, EVAL_VAR_IMPORT) == FAIL)
	ফেরত FAIL;
    যদি (rettv->v_type == VAR_ANY && *s == '.')
    {
	char_u *ns = s + 1;
	s = ns;
	যখন (ASCII_ISALNUM(*s) || *s == '_')
	    ++s;
	পূর্ণসংখ্যা sid = rettv->vval.v_সংখ্যা;
	ফেরত eval_variable(ns, (পূর্ণসংখ্যা)(s - ns), sid, rettv, ফাঁকা, 0);
    }
    ফেরত OK;
}


/*
 * Check if variable "name[len]" is a local variable or an argument.
 * If so, "*eval_lavars_used" is set to TRUE.
 */
    শূন্য
check_vars(char_u *name, পূর্ণসংখ্যা len)
{
    পূর্ণসংখ্যা		cc;
    char_u	*varname;
    hashtab_T	*ht;

    যদি (eval_lavars_used == ফাঁকা)
	ফেরত;

    // truncate the name, so that we can use strcmp()
    cc = name[len];
    name[len] = NUL;

    ht = find_var_ht(name, &varname);
    যদি (ht == get_funccal_local_ht() || ht == get_funccal_args_ht())
    {
	যদি (find_var(name, ফাঁকা, TRUE) != ফাঁকা)
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
find_var(char_u *name, hashtab_T **htp, পূর্ণসংখ্যা no_autoload)
{
    char_u	*varname;
    hashtab_T	*ht;
    dictitem_T	*ret = ফাঁকা;

    ht = find_var_ht(name, &varname);
    যদি (htp != ফাঁকা)
	*htp = ht;
    যদি (ht == ফাঁকা)
	ফেরত ফাঁকা;
    ret = find_var_in_ht(ht, *name, varname, no_autoload);
    যদি (ret != ফাঁকা)
	ফেরত ret;

    // Search in parent scope for lambda
    ret = find_var_in_scoped_ht(name, no_autoload);
    যদি (ret != ফাঁকা)
	ফেরত ret;

    // in Vim9 script items without a scope can be script-local
    যদি (in_vim9script() && name[0] != NUL && name[1] != ':')
    {
	ht = get_script_local_ht();
	যদি (ht != ফাঁকা)
	{
	    ret = find_var_in_ht(ht, *name, varname, no_autoload);
	    যদি (ret != ফাঁকা)
	    {
		যদি (htp != ফাঁকা)
		    *htp = ht;
		ফেরত ret;
	    }
	}
    }

    // and finally try
    ফেরত find_var_autoload_prefix(name, 0, htp, ফাঁকা);
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
find_var_autoload_prefix(char_u *name, পূর্ণসংখ্যা sid, hashtab_T **htp,
							    char_u **namep)
{
    hashtab_T	*ht;
    dictitem_T	*ret = ফাঁকা;
    // When using "vim9script autoload" script-local items are prefixed but can
    // be used with s:name.
    পূর্ণসংখ্যা check_sid = sid > 0 ? sid : current_sctx.sc_sid;
    যদি (SCRIPT_ID_VALID(check_sid)
		   && (in_vim9script() || (name[0] == 's' && name[1] == ':')))
    {
	scriptitem_T *si = SCRIPT_ITEM(check_sid);

	যদি (si->sn_autoload_prefix != ফাঁকা)
	{
	    char_u *base_name = (name[0] == 's' && name[1] == ':')
							     ? name + 2 : name;
	    char_u *auto_name = concat_str(si->sn_autoload_prefix, base_name);

	    যদি (auto_name != ফাঁকা)
	    {
		পূর্ণসংখ্যা free_auto_name = TRUE;
		ht = &globvarht;
		ret = find_var_in_ht(ht, 'g', auto_name, TRUE);
		যদি (ret != ফাঁকা)
		{
		    যদি (htp != ফাঁকা)
			*htp = ht;
		    যদি (namep != ফাঁকা)
		    {
			free_auto_name = FALSE;
			*namep = auto_name;
		    }
		}
		যদি (free_auto_name)
		    vim_free(auto_name);
	    }
	}
    }

    ফেরত ret;
}

/*
 * Like find_var() but if the name starts with <SNR>99_ then look in the
 * referenced script (used for a funcref).
 */
    dictitem_T *
find_var_also_in_script(char_u *name, hashtab_T **htp, পূর্ণসংখ্যা no_autoload)
{
    যদি (STRNCMP(name, "<SNR>", 5) == 0 && SAFE_isdigit(name[5]))
    {
	char_u	    *p = name + 5;
	পূর্ণসংখ্যা	    sid = getdigits(&p);

	যদি (SCRIPT_ID_VALID(sid) && *p == '_')
	{
	    hashtab_T	*ht = &SCRIPT_VARS(sid);

	    যদি (ht != ফাঁকা)
	    {
		dictitem_T *di = find_var_in_ht(ht, 0, p + 1, no_autoload);

		যদি (di != ফাঁকা)
		{
		    যদি (htp != ফাঁকা)
			*htp = ht;
		    ফেরত di;
		}
	    }
	}
    }

    ফেরত find_var(name, htp, no_autoload);
}

/*
 * Find variable "varname" in hashtab "ht" with name "htname".
 * When "varname" is empty returns curwin/curtab/etc vars dictionary.
 * Returns NULL if not found.
 */
    dictitem_T *
find_var_in_ht(
    hashtab_T	*ht,
    পূর্ণসংখ্যা		htname,
    char_u	*varname,
    পূর্ণসংখ্যা		no_autoload)
{
    hashitem_T	*hi;

    যদি (*varname == NUL)
    {
	// Must be something like "s:", otherwise "ht" would be NULL.
	নির্বাচন (htname)
	{
	    ক্ষেত্র 's': ফেরত &SCRIPT_SV(current_sctx.sc_sid)->sv_var;
	    ক্ষেত্র 'g': ফেরত &globvars_var;
	    ক্ষেত্র 'v': ফেরত &vimvars_var;
	    ক্ষেত্র 'b': ফেরত &curbuf->b_bufvar;
	    ক্ষেত্র 'w': ফেরত &curwin->w_winvar;
	    ক্ষেত্র 't': ফেরত &curtab->tp_winvar;
	    ক্ষেত্র 'l': ফেরত get_funccal_local_var();
	    ক্ষেত্র 'a': ফেরত get_funccal_args_var();
	}
	ফেরত ফাঁকা;
    }

    hi = hash_find(ht, varname);
    যদি (HASHITEM_EMPTY(hi))
    {
	// For global variables we may try auto-loading the script.  If it
	// worked find the variable again.  Don't auto-load a script if it was
	// loaded already, otherwise it would be loaded every time when
	// checking if a function name is a Funcref variable.
	যদি (ht == &globvarht && !no_autoload)
	{
	    // Note: script_autoload() may make "hi" invalid. It must either
	    // be obtained again or not used.
	    যদি (!script_autoload(varname, FALSE) || aborting())
		ফেরত ফাঁকা;
	    hi = hash_find(ht, varname);
	}
	যদি (HASHITEM_EMPTY(hi))
	    ফেরত ফাঁকা;
    }
    ফেরত HI2DI(hi);
}

/*
 * Get the script-local hashtab.  NULL if not in a script context.
 */
    hashtab_T *
get_script_local_ht(শূন্য)
{
    scid_T sid = current_sctx.sc_sid;

    যদি (SCRIPT_ID_VALID(sid))
	ফেরত &SCRIPT_VARS(sid);
    ফেরত ফাঁকা;
}

/*
 * Look for "name[len]" in script-local variables and functions.
 * When "cmd" is TRUE it must look like a command, a function must be followed
 * by "(" or "->".
 * Return OK when found, FAIL when not found.
 */
    পূর্ণসংখ্যা
lookup_scriptitem(
	char_u	*name,
	size_t	len,
	পূর্ণসংখ্যা	cmd,
	cctx_T	*dummy UNUSED)
{
    hashtab_T	*ht = get_script_local_ht();
    char_u	বাফার[30];
    char_u	*p;
    পূর্ণসংখ্যা		res;
    hashitem_T	*hi;
    পূর্ণসংখ্যা		is_global = FALSE;
    char_u	*fname = name;

    যদি (ht == ফাঁকা)
	ফেরত FAIL;
    যদি (len < আকার(বাফার) - 1)
    {
	// avoid an alloc/free for short names
	vim_strncpy(বাফার, name, len);
	p = বাফার;
    }
    অন্যথা
    {
	p = vim_strnsave(name, len);
	যদি (p == ফাঁকা)
	    ফেরত FAIL;
    }

    hi = hash_find(ht, p);
    res = HASHITEM_EMPTY(hi) ? FAIL : OK;

    // if not script-local, then perhaps autoload-exported
    যদি (res == FAIL && find_var_autoload_prefix(p, 0, ফাঁকা, ফাঁকা) != ফাঁকা)
	res = OK;

    // if not script-local or autoload, then perhaps imported
    যদি (res == FAIL && find_imported(p, 0, FALSE) != ফাঁকা)
	res = OK;
    যদি (p != বাফার)
	vim_free(p);

    // Find a function, so that a following "->" works.
    // When used as a command require "(" or "->" to follow, "Cmd" is a user
    // command while "Cmd()" is a function call.
    যদি (res != OK)
    {
	p = skipwhite(name + len);

	যদি (!cmd || name[len] == '(' || (p[0] == '-' && p[1] == '>'))
	{
	    // Do not check for an internal function, since it might also be a
	    // valid command, such as ":split" versus "split()".
	    // Skip "g:" before a function name.
	    যদি (name[0] == 'g' && name[1] == ':')
	    {
		is_global = TRUE;
		fname = name + 2;
	    }
	    যদি (find_func(fname, is_global) != ফাঁকা)
		res = OK;
	}
    }

    ফেরত res;
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

    যদি (name[0] == NUL)
	ফেরত ফাঁকা;
    যদি (name[1] != ':')
    {
	// The name must not start with a colon or #.
	যদি (name[0] == ':' || name[0] == AUTOLOAD_CHAR)
	    ফেরত ফাঁকা;
	*varname = name;

	// "version" is "v:version" in all scopes if scriptversion < 3.
	// Same for a few other variables marked with VV_COMPAT.
	যদি (in_old_script(3))
	{
	    hi = hash_find(&compat_hashtab, name);
	    যদি (!HASHITEM_EMPTY(hi))
		ফেরত &compat_hashtab;
	}

	ht = get_funccal_local_ht();
	যদি (ht != ফাঁকা)
	    ফেরত ht;				// local variable

	// In Vim9 script items at the script level are script-local, except
	// for autoload names.
	যদি (in_vim9script() && vim_strchr(name, AUTOLOAD_CHAR) == ফাঁকা)
	{
	    ht = get_script_local_ht();
	    যদি (ht != ফাঁকা)
		ফেরত ht;
	}

	ফেরত &globvarht;			// global variable
    }
    *varname = name + 2;
    যদি (*name == 'g')				// global variable
	ফেরত &globvarht;
    // There must be no ':' or '#' in the rest of the name, unless g: is used
    যদি (vim_strchr(name + 2, ':') != ফাঁকা
			       || vim_strchr(name + 2, AUTOLOAD_CHAR) != ফাঁকা)
	ফেরত ফাঁকা;
    যদি (*name == 'b')				// buffer variable
	ফেরত &curbuf->b_vars->dv_hashtab;
    যদি (*name == 'w')				// window variable
	ফেরত &curwin->w_vars->dv_hashtab;
    যদি (*name == 't')				// tab page variable
	ফেরত &curtab->tp_vars->dv_hashtab;
    যদি (*name == 'v')				// v: variable
	ফেরত &vimvarht;
    যদি (get_current_funccal() != ফাঁকা
	       && get_current_funccal()->fc_func->uf_def_status
							    == UF_NOT_COMPILED)
    {
	// a: and l: are only used in functions defined with ":function"
	যদি (*name == 'a')			// a: function argument
	    ফেরত get_funccal_args_ht();
	যদি (*name == 'l')			// l: local function variable
	    ফেরত get_funccal_local_ht();
    }
    যদি (*name == 's')				// script variable
    {
	ht = get_script_local_ht();
	যদি (ht != ফাঁকা)
	    ফেরত ht;
    }
    ফেরত ফাঁকা;
}

/*
 * Get the string value of a (global/local) variable.
 * Note: see tv_get_string() for how long the pointer remains valid.
 * Returns NULL when it doesn't exist.
 */
    char_u *
get_var_মান(char_u *name)
{
    dictitem_T	*v;

    v = find_var(name, ফাঁকা, FALSE);
    যদি (v == ফাঁকা)
	ফেরত ফাঁকা;
    ফেরত tv_get_string(&v->di_tv);
}

/*
 * Allocate a new hashtab for a sourced script.  It will be used while
 * sourcing this script and when executing functions defined in the script.
 */
    শূন্য
new_script_vars(scid_T id)
{
    scriptvar_T *sv;

    sv = ALLOC_CLEAR_ONE(scriptvar_T);
    যদি (sv == ফাঁকা)
	ফেরত;
    init_var_dict(&sv->sv_dict, &sv->sv_var, VAR_SCOPE);
    SCRIPT_ITEM(id)->sn_vars = sv;
}

/*
 * Initialize dictionary "dict" as a scope and set variable "dict_var" to
 * point to it.
 */
    শূন্য
init_var_dict(dict_T *dict, dictitem_T *dict_var, পূর্ণসংখ্যা scope)
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
    dict_var->di_চাবি[0] = NUL;
}

/*
 * Unreference a dictionary initialized by init_var_dict().
 */
    শূন্য
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
    শূন্য
vars_clear(hashtab_T *ht)
{
    vars_clear_ext(ht, TRUE);
}

/*
 * Like vars_clear(), but only free the value if "free_val" is TRUE.
 */
    শূন্য
vars_clear_ext(hashtab_T *ht, পূর্ণসংখ্যা free_val)
{
    পূর্ণসংখ্যা		todo;
    hashitem_T	*hi;
    dictitem_T	*v;

    hash_lock(ht);
    todo = (পূর্ণসংখ্যা)ht->ht_used;
    FOR_ALL_HASHTAB_বস্তুসমূহ(ht, hi, todo)
    {
	যদি (!HASHITEM_EMPTY(hi))
	{
	    --todo;

	    // Free the variable.  Don't remove it from the hashtab,
	    // ht_array might change then.  hash_clear() takes care of it
	    // later.
	    v = HI2DI(hi);
	    যদি (free_val)
		clear_tv(&v->di_tv);
	    যদি (v->di_flags & DI_FLAGS_ALLOC)
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
    শূন্য
delete_var(hashtab_T *ht, hashitem_T *hi)
{
    dictitem_T	*di = HI2DI(hi);

    যদি (hash_remove(ht, hi, "delete variable") != OK)
	ফেরত;

    clear_tv(&di->di_tv);
    vim_free(di);
}

/*
 * Delete the exported variables of a reloaded Vim9 autoload script.  They live
 * in the global namespace with the autoload prefix (e.g. "foo#bar") and are
 * recreated when the script body runs again.
 */
    শূন্য
delete_autoload_export_vars(char_u *prefix)
{
    যদি (prefix == ফাঁকা)
	ফেরত;
    size_t  prefixlen = STRLEN(prefix);

    hash_lock(&globvarht);
    পূর্ণসংখ্যা	todo = (পূর্ণসংখ্যা)globvarht.ht_used;

    জন্য (hashitem_T *hi = globvarht.ht_array; todo > 0; ++hi)
	যদি (!HASHITEM_EMPTY(hi))
	{
	    dictitem_T	*di = HI2DI(hi);

	    --todo;
	    // Keep a class or enum: existing objects still refer to it.
	    যদি (di->di_tv.v_type != VAR_CLASS
		    && STRNCMP(di->di_চাবি, prefix, prefixlen) == 0)
		delete_var(&globvarht, hi);
	}
    hash_unlock(&globvarht);
}

/*
 * List the value of one internal variable.
 */
    স্থির শূন্য
list_one_var(dictitem_T *v, অক্ষর *prefix, পূর্ণসংখ্যা *first)
{
    char_u	*tofree;
    char_u	*s;
    char_u	numbuf[NUMBUFLEN];

    s = echo_string(&v->di_tv, &tofree, numbuf, get_copyID());
    list_one_var_a(prefix, v->di_চাবি, v->di_tv.v_type,
					 s == ফাঁকা ? (char_u *)"" : s, first);
    vim_free(tofree);
}

    স্থির শূন্য
list_one_var_a(
    অক্ষর	*prefix,
    char_u	*name,
    পূর্ণসংখ্যা		type,
    char_u	*string,
    পূর্ণসংখ্যা		*first)  // when TRUE clear rest of screen and set to FALSE
{
    // don't use msg() or msg_attr() to avoid overwriting "v:statusmsg"
    বার্তা_start();
    বার্তা_puts(prefix);
    যদি (name != ফাঁকা)	// "a:" vars don't have a name stored
	বার্তা_puts((অক্ষর *)name);
    বার্তা_putchar(' ');
    বার্তা_advance(22);
    যদি (type == VAR_সংখ্যা)
	বার্তা_putchar('#');
    অন্যথা যদি (type == VAR_FUNC || type == VAR_PARTIAL)
	বার্তা_putchar('*');
    অন্যথা যদি (type == VAR_LIST)
    {
	বার্তা_putchar('[');
	যদি (*string == '[')
	    ++string;
    }
    অন্যথা যদি (type == VAR_DICT)
    {
	বার্তা_putchar('{');
	যদি (*string == '{')
	    ++string;
    }
    অন্যথা
	বার্তা_putchar(' ');

    বার্তা_outtrans(string);

    যদি (type == VAR_FUNC || type == VAR_PARTIAL)
	বার্তা_puts("()");
    যদি (*first)
    {
	বার্তা_clr_eos();
	*first = FALSE;
    }
}

/*
 * Addition handling for setting a v: variable.
 * Return TRUE if the variable should be set normally,
 *        FALSE if nothing else needs to be done.
 */
    পূর্ণসংখ্যা
before_set_vvar(
    char_u	*varname,
    dictitem_T	*di,
    typval_T	*tv,
    পূর্ণসংখ্যা		copy,
    পূর্ণসংখ্যা		*type_error)
{
    যদি (di->di_tv.v_type == VAR_STRING)
    {
	VIM_CLEAR(di->di_tv.vval.v_string);
	যদি (copy || tv->v_type != VAR_STRING)
	{
	    char_u *val = tv_get_string(tv);

	    // Careful: when assigning to v:errmsg and
	    // tv_get_string() causes an error message the variable
	    // will already be set.
	    যদি (di->di_tv.vval.v_string == ফাঁকা)
		di->di_tv.vval.v_string = vim_strsave(val);
	}
	অন্যথা
	{
	    // Take over the string to avoid an extra alloc/free.
	    di->di_tv.vval.v_string = tv->vval.v_string;
	    tv->vval.v_string = ফাঁকা;
	}
	ফেরত FALSE;
    }
    অন্যথা যদি (di->di_tv.v_type == VAR_সংখ্যা)
    {
	di->di_tv.vval.v_সংখ্যা = tv_get_সংখ্যা(tv);
	যদি (STRCMP(varname, "searchforward") == 0)
	    set_অনুসন্ধান_direction(di->di_tv.vval.v_সংখ্যা ? '/' : '?');
#ifdef FEAT_অনুসন্ধান_EXTRA
	অন্যথা যদি (STRCMP(varname, "hlsearch") == 0)
	{
	    no_hlsearch = !di->di_tv.vval.v_সংখ্যা;
	    redraw_all_later(UPD_SOME_VALID);
	}
#endif
	ফেরত FALSE;
    }
    অন্যথা যদি (di->di_tv.v_type != tv->v_type)
    {
	*type_error = TRUE;
	ফেরত FALSE;
    }
    ফেরত TRUE;
}

/*
 * Set variable "name" to value in "tv".
 * If the variable already exists, the value is updated.
 * Otherwise the variable is created.
 */
    শূন্য
set_var(
    char_u	*name,
    typval_T	*tv,
    পূর্ণসংখ্যা		copy)	    // make copy of value in "tv"
{
    set_var_const(name, 0, ফাঁকা, tv, copy, ASSIGN_DECL, 0);
}

/*
 * Set variable "name" to value in "tv_arg".
 * When "sid" is non-zero "name" is in the script with this ID.
 * If the variable already exists and "is_const" is FALSE the value is updated.
 * Otherwise the variable is created.
 */
    পূর্ণসংখ্যা
set_var_const(
    char_u	*name,
    scid_T	sid,
    type_T	*type_arg,
    typval_T	*tv_arg,
    পূর্ণসংখ্যা		copy,	    // make copy of value in "tv"
    পূর্ণসংখ্যা		flags_arg,  // ASSIGN_CONST, ASSIGN_FINAL, etc.
    পূর্ণসংখ্যা		var_idx)    // index for ":let [a, b] = list"
{
    typval_T	*tv = tv_arg;
    type_T	*type = type_arg;
    typval_T	bool_tv;
    dictitem_T	*di;
    typval_T	*dest_tv;
    char_u	*varname;
    char_u	*name_tofree = ফাঁকা;
    hashtab_T	*ht = ফাঁকা;
    পূর্ণসংখ্যা		is_script_local;
    পূর্ণসংখ্যা		vim9script = in_vim9script();
    পূর্ণসংখ্যা		var_in_vim9script;
    পূর্ণসংখ্যা		var_in_autoload = FALSE;
    পূর্ণসংখ্যা		flags = flags_arg;
    পূর্ণসংখ্যা		free_tv_arg = !copy;  // free tv_arg if not used
    পূর্ণসংখ্যা		rc = FAIL;

    যদি (sid != 0)
    {
	varname = ফাঁকা;
	যদি (SCRIPT_ID_VALID(sid))
	{
	    char_u	*auto_name = ফাঁকা;
	    যদি (find_var_autoload_prefix(name, sid, &ht, &auto_name) != ফাঁকা)
	    {
		var_in_autoload = TRUE;
		varname = auto_name;
		name_tofree = varname;
	    }
	    অন্যথা
		ht = &SCRIPT_VARS(sid);
	}
	যদি (varname == ফাঁকা)
	    varname = name;
    }
    অন্যথা
    {
	scriptitem_T	*si;
	char_u		*auto_name = ফাঁকা;

	যদি (in_vim9script()
	    && SCRIPT_ID_VALID(current_sctx.sc_sid)
	    && (si = SCRIPT_ITEM(current_sctx.sc_sid))
					      ->sn_autoload_prefix != ফাঁকা
	    && (is_export
		|| find_var_autoload_prefix(name, 0, ফাঁকা, &auto_name)
								    != ফাঁকা))
	{
	    // In a vim9 autoload script an exported variable is put in the
	    // global namespace with the autoload prefix.
	    var_in_autoload = TRUE;
	    varname = auto_name != ফাঁকা ? auto_name
		      : concat_str(si->sn_autoload_prefix, name);
	    যদি (varname == ফাঁকা)
		যাও failed;
	    name_tofree = varname;
	    ht = &globvarht;
	}
	অন্যথা
	    ht = find_var_ht(name, &varname);
    }
    যদি (ht == ফাঁকা || *varname == NUL)
    {
	semsg(_(e_illegal_variable_name_str), name);
	যাও failed;
    }
    is_script_local = ht == get_script_local_ht() || sid != 0
							    || var_in_autoload;

    যদি (vim9script
	    && !is_script_local
	    && (flags & (ASSIGN_NO_DECL | ASSIGN_DECL)) == 0
	    && (flags & (ASSIGN_CONST | ASSIGN_FINAL)) == 0
	    && name[1] == ':')
    {
	vim9_declare_error(name);
	যাও failed;
    }
    যদি ((flags & ASSIGN_FOR_LOOP) && is_scoped_variable(name))
	// Do not make g:var, w:var, b:var or t:var final.
	flags &= ~ASSIGN_FINAL;

    var_in_vim9script = is_script_local && current_script_is_vim9();
    যদি (var_in_vim9script && name[0] == '_' && name[1] == NUL)
    {
	// For "[a, _] = list" the underscore is ignored.
	যদি ((flags & ASSIGN_UNPACK) == 0)
	    emsg(_(e_cannot_use_underscore_here));
	যাও failed;
    }

    di = find_var_in_ht(ht, 0, varname, TRUE);

    যদি (di == ফাঁকা && var_in_vim9script)
    {
	imported_T  *import = find_imported(varname, 0, FALSE);

	যদি (import != ফাঁকা)
	{
	    // imported name space cannot be used
	    যদি ((flags & ASSIGN_NO_DECL) == 0)
	    {
		semsg(_(e_redefining_imported_item_str), name);
		যাও failed;
	    }
	    semsg(_(e_cannot_use_str_itself_it_is_imported), name);
	    যাও failed;
	}
	যদি (!in_vim9script())
	{
	    semsg(_(e_cannot_create_vim9_script_variable_in_function_str),
									 name);
	    যাও failed;
	}
    }

    // Search in parent scope which is possible to reference from lambda
    যদি (di == ফাঁকা)
	di = find_var_in_scoped_ht(name, TRUE);

    যদি ((tv->v_type == VAR_FUNC || tv->v_type == VAR_PARTIAL)
				  && var_wrong_func_name(name, di == ফাঁকা))
	যাও failed;

    যদি (need_convert_to_bool(type, tv))
    {
	// Destination is a bool and the value is not, but it can be
	// converted.
	CLEAR_FIELD(bool_tv);
	bool_tv.v_type = VAR_BOOL;
	bool_tv.vval.v_সংখ্যা = tv2bool(tv) ? VVAL_TRUE : VVAL_FALSE;
	tv = &bool_tv;
    }

    যদি (di != ফাঁকা)
    {
	// Item already exists.  Allowed to replace when reloading.
	যদি ((di->di_flags & DI_FLAGS_RELOAD) == 0)
	{
	    যদি ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					 && (flags & ASSIGN_FOR_LOOP) == 0)
	    {
		emsg(_(e_cannot_modify_existing_variable));
		যাও failed;
	    }

	    যদি (is_script_local && vim9script
			  && (flags & (ASSIGN_NO_DECL | ASSIGN_DECL)) == 0)
	    {
		semsg(_(e_redefining_script_item_str), name);
		যাও failed;
	    }

	    যদি (check_typval_is_মান(&di->di_tv) == FAIL)
		যাও failed;

	    // List and Blob types can be modified in-place using the "+="
	    // compound operator.  For other types, this is not allowed.
	    পূর্ণসংখ্যা type_inplace_modifiable =
		(di->di_tv.v_type == VAR_LIST || di->di_tv.v_type == VAR_BLOB);

	    যদি (var_in_vim9script && (flags & ASSIGN_FOR_LOOP) == 0
		    && ((flags & ASSIGN_COMPOUND_OP) == 0
			|| !type_inplace_modifiable))
	    {
		where_T where = WHERE_INIT;
		svar_T  *sv = find_typval_in_script(&di->di_tv, sid, TRUE);

		যদি (sv != ফাঁকা)
		{
		    // check the type and adjust to bool if needed
		    যদি (var_idx > 0)
		    {
			where.wt_সূচক = var_idx;
			where.wt_kind = WT_VARIABLE;
		    }
		    যদি (check_script_var_type(sv, tv, name, where) == FAIL)
			যাও failed;
		    যদি (type == ফাঁকা)
			type = sv->sv_type;
		    sv->sv_flags |= SVFLAG_ASSIGNED;
		}
	    }

	    // Modifying a final variable with a List value using the "+="
	    // operator is allowed.  For other types, it is not allowed.
	    যদি ((((flags & ASSIGN_FOR_LOOP) == 0 || (flags & ASSIGN_DECL) == 0)
			&& ((flags & ASSIGN_COMPOUND_OP) == 0
			    || !type_inplace_modifiable))
				 ? var_check_permission(di, name) == FAIL
				 : var_check_ro(di->di_flags, name, FALSE))
		যাও failed;
	}
	অন্যথা
	{
	    // can only redefine once
	    di->di_flags &= ~DI_FLAGS_RELOAD;

	    // A Vim9 script-local variable is also present in sn_all_vars
	    // and sn_var_vals.  It may set "type" from "tv".
	    যদি (var_in_vim9script || var_in_autoload)
		update_vim9_script_var(FALSE, di,
			var_in_autoload ? name : di->di_চাবি, flags,
			tv, &type, (flags & ASSIGN_NO_MEMBER_TYPE) == 0);
	}

	// existing variable, need to clear the value

	// In Vim9 script a String v: variable only takes a String.
	যদি (ht == &vimvarht && di->di_tv.v_type == VAR_STRING
							    && in_vim9script())
	{
	    where_T	where = WHERE_INIT;

	    যদি (check_typval_type(&t_string, tv, where) == FAIL)
		যাও failed;
	}

	// Handle setting internal v: variables separately where needed to
	// prevent changing the type.
	পূর্ণসংখ্যা type_error = FALSE;
	যদি (ht == &vimvarht
		&& !before_set_vvar(varname, di, tv, copy, &type_error))
	{
	    যদি (type_error)
		semsg(_(e_setting_v_str_to_মান_with_wrong_type), varname);
	    যাও failed;
	}

	clear_tv(&di->di_tv);

	যদি ((flags & ASSIGN_UPDATE_BLOCK_ID)
				       && SCRIPT_ID_VALID(current_sctx.sc_sid))
	{
	    scriptitem_T *si = SCRIPT_ITEM(current_sctx.sc_sid);

	    update_script_var_block_id(name, si->sn_current_block_id);
	}
    }
    অন্যথা
    {
	// Item not found, check if a function already exists.
	যদি (is_script_local && (flags & (ASSIGN_NO_DECL | ASSIGN_DECL)) == 0
	       && lookup_scriptitem(name, STRLEN(name), FALSE, ফাঁকা) == OK)
	{
	    semsg(_(e_redefining_script_item_str), name);
	    যাও failed;
	}

	// add a new variable
	যদি (var_in_vim9script && (flags & ASSIGN_NO_DECL))
	{
	    semsg(_(e_unknown_variable_str), name);
	    যাও failed;
	}

	যদি (check_hashtab_frozen(ht, "add variable"))
	    যাও failed;

	// Can't add "v:" or "a:" variable.
	যদি (ht == &vimvarht || ht == get_funccal_args_ht())
	{
	    semsg(_(e_illegal_variable_name_str), name);
	    যাও failed;
	}

	// Make sure the variable name is valid.  In Vim9 script an
	// autoload variable must be prefixed with "g:" unless in an
	// autoload script.
	যদি (!valid_varname(varname, -1, !vim9script
			|| STRNCMP(name, "g:", 2) == 0 || var_in_autoload))
	    যাও failed;

	di = alloc(offsetof(dictitem_T, di_চাবি) + STRLEN(varname) + 1);
	যদি (di == ফাঁকা)
	    যাও failed;
	STRCPY(di->di_চাবি, varname);
	যদি (hash_add(ht, DI2HIKEY(di), "add variable") == FAIL)
	{
	    vim_free(di);
	    যাও failed;
	}
	di->di_flags = DI_FLAGS_ALLOC;
	যদি (flags & (ASSIGN_CONST | ASSIGN_FINAL))
	    di->di_flags |= DI_FLAGS_LOCK;

	// A Vim9 script-local variable is also added to sn_all_vars and
	// sn_var_vals. It may set "type" from "tv".
	যদি (var_in_vim9script || var_in_autoload)
	    update_vim9_script_var(TRUE, di,
		    var_in_autoload ? name : di->di_চাবি, flags,
			  tv, &type, (flags & ASSIGN_NO_MEMBER_TYPE) == 0);
    }

    dest_tv = &di->di_tv;
    যদি (copy || tv->v_type == VAR_সংখ্যা || tv->v_type == VAR_FLOAT)
	copy_tv(tv, dest_tv);
    অন্যথা
    {
	*dest_tv = *tv;
	dest_tv->v_lock = 0;
	init_tv(tv);
    }
    free_tv_arg = FALSE;

    যদি (vim9script && type != ফাঁকা)
	set_tv_type(dest_tv, type);

    // ":const var = value" locks the value
    // ":final var = value" locks "var"
    যদি (flags & ASSIGN_CONST)
	// Like :lockvar! name: lock the value and what it contains, but only
	// if the reference count is up to one.  That locks only literal
	// values.
	item_lock(dest_tv, DICT_MAXNEST, TRUE, TRUE);

    rc = OK;

failed:
    vim_free(name_tofree);
    যদি (free_tv_arg)
	clear_tv(tv_arg);

    ফেরত rc;
}

/*
 * Check in this order for backwards compatibility:
 * - Whether the variable is read-only
 * - Whether the variable value is locked
 * - Whether the variable is locked
 * NOTE: "name" is only used for error messages.
 */
    পূর্ণসংখ্যা
var_check_permission(dictitem_T *di, char_u *name)
{
    যদি (var_check_ro(di->di_flags, name, FALSE)
		    || মান_check_lock(di->di_tv.v_lock, name, FALSE)
		    || var_check_lock(di->di_flags, name, FALSE))
	ফেরত FAIL;
    ফেরত OK;
}

/*
 * Return TRUE if di_flags "flags" indicates variable "name" is read-only.
 * Also give an error message.
 */
    পূর্ণসংখ্যা
var_check_ro(পূর্ণসংখ্যা flags, char_u *name, পূর্ণসংখ্যা use_gettext)
{
    যদি (flags & DI_FLAGS_RO)
    {
	যদি (name == ফাঁকা)
	    emsg(_(e_cannot_change_readonly_variable));
	অন্যথা
	    semsg(_(e_cannot_change_readonly_variable_str),
				       use_gettext ? (char_u *)_(name) : name);
	ফেরত TRUE;
    }
    যদি ((flags & DI_FLAGS_RO_SBX) && sandbox)
    {
	যদি (name == ফাঁকা)
	    emsg(_(e_cannot_set_variable_in_sandbox));
	অন্যথা
	    semsg(_(e_cannot_set_variable_in_sandbox_str),
				       use_gettext ? (char_u *)_(name) : name);
	ফেরত TRUE;
    }
    ফেরত FALSE;
}

/*
 * Return TRUE if di_flags "flags" indicates variable "name" is locked.
 * Also give an error message.
 */
    পূর্ণসংখ্যা
var_check_lock(পূর্ণসংখ্যা flags, char_u *name, পূর্ণসংখ্যা use_gettext)
{
    যদি (flags & DI_FLAGS_LOCK)
    {
	semsg(_(e_variable_is_locked_str),
				       use_gettext ? (char_u *)_(name) : name);
	ফেরত TRUE;
    }
    ফেরত FALSE;
}

/*
 * Return TRUE if di_flags "flags" indicates variable "name" is fixed.
 * Also give an error message.
 */
    পূর্ণসংখ্যা
var_check_fixed(পূর্ণসংখ্যা flags, char_u *name, পূর্ণসংখ্যা use_gettext)
{
    যদি (flags & DI_FLAGS_FIX)
    {
	যদি (name == ফাঁকা)
	    emsg(_(e_cannot_delete_variable));
	অন্যথা
	    semsg(_(e_cannot_delete_variable_str),
				      use_gettext ? (char_u *)_(name) : name);
	ফেরত TRUE;
    }
    ফেরত FALSE;
}

/*
 * Check if a funcref is assigned to a valid variable name.
 * Return TRUE and give an error if not.
 */
    পূর্ণসংখ্যা
var_wrong_func_name(
    char_u *name,    // points to start of variable name
    পূর্ণসংখ্যা    new_var)  // TRUE when creating the variable
{
    // Allow for w: b: s: and t:.  In Vim9 script s: is not allowed, because
    // the name can be used without the s: prefix.
    // Allow autoload variable.
    যদি (!((vim_strchr((char_u *)"wbt", name[0]) != ফাঁকা
		    || (!in_vim9script() && name[0] == 's')) && name[1] == ':')
	    && !ASCII_ISUPPER((name[0] != NUL && name[1] == ':')
						     ? name[2] : name[0])
	    && vim_strchr(name, '#') == ফাঁকা)
    {
	semsg(_(e_funcref_variable_name_must_start_with_capital_str), name);
	ফেরত TRUE;
    }
    // Don't allow hiding a function.  When "v" is not NULL we might be
    // assigning another function to the same var, the type is checked
    // below.
    যদি (new_var && function_exists(name, FALSE))
    {
	semsg(_(e_variable_name_conflicts_with_existing_function_str),
								    name);
	ফেরত TRUE;
    }
    ফেরত FALSE;
}

/*
 * Return TRUE if "flags" indicates variable "name" has a locked (immutable)
 * value.  Also give an error message, using "name" or _("name") when
 * "use_gettext" is TRUE.
 */
    পূর্ণসংখ্যা
মান_check_lock(পূর্ণসংখ্যা lock, char_u *name, পূর্ণসংখ্যা use_gettext)
{
    যদি (lock & VAR_LOCKED)
    {
	যদি (name == ফাঁকা)
	    emsg(_(e_মান_is_locked));
	অন্যথা
	    semsg(_(e_মান_is_locked_str),
				       use_gettext ? (char_u *)_(name) : name);
	ফেরত TRUE;
    }
    যদি (lock & VAR_FIXED)
    {
	যদি (name == ফাঁকা)
	    emsg(_(e_cannot_change_মান));
	অন্যথা
	    semsg(_(e_cannot_change_মান_of_str),
				       use_gettext ? (char_u *)_(name) : name);
	ফেরত TRUE;
    }
    ফেরত FALSE;
}

/*
 * Check if a variable name is valid.  When "autoload" is true "#" is allowed.
 * If "len" is -1 use all of "varname", otherwise up to "varname[len]".
 * Return FALSE and give an error if not.
 */
    পূর্ণসংখ্যা
valid_varname(char_u *varname, পূর্ণসংখ্যা len, পূর্ণসংখ্যা autoload)
{
    char_u *p;

    জন্য (p = varname; len < 0 ? *p != NUL : p < varname + len; ++p)
	যদি (!eval_isnamec1(*p) && (p == varname || !VIM_ISDIGIT(*p))
					 && !(autoload && *p == AUTOLOAD_CHAR))
	{
	    semsg(_(e_illegal_variable_name_str), varname);
	    ফেরত FALSE;
	}
    ফেরত TRUE;
}

/*
 * Implements the logic to retrieve local variable and option values.
 * Used by "getwinvar()" "gettabvar()" "gettabwinvar()" "getbufvar()".
 */
    স্থির শূন্য
get_var_from(
    char_u	*varname,
    typval_T	*rettv,
    typval_T	*deftv,	    // Default value if not found.
    পূর্ণসংখ্যা		htname,	    // 't'ab, 'w'indow or 'b'uffer local.
    tabpage_T	*tp,	    // can be NULL
    win_T	*win,
    buf_T	*buf)	    // Ignored if htname is not 'b'.
{
    dictitem_T	*v;
    পূর্ণসংখ্যা		done = FALSE;
    switchwin_T	switchwin;
    পূর্ণসংখ্যা		need_switch_win;
    পূর্ণসংখ্যা		do_change_curbuf = buf != ফাঁকা && htname == 'b';

    ++emsg_off;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ফাঁকা;

    যদি (varname != ফাঁকা && tp != ফাঁকা && win != ফাঁকা
	    && (htname != 'b' || buf != ফাঁকা))
    {
	// Set curwin to be our win, temporarily.  Also set the tabpage,
	// otherwise the window is not valid. Only do this when needed,
	// autocommands get blocked.
	// If we have a buffer reference avoid the switching, we're saving and
	// restoring curbuf directly.
	need_switch_win = !(tp == curtab && win == curwin) && !do_change_curbuf;
	যদি (!need_switch_win || switch_win(&switchwin, win, tp, TRUE) == OK)
	{
	    // Handle options. There are no tab-local options.
	    যদি (*varname == '&' && htname != 't')
	    {
		buf_T	*save_curbuf = curbuf;

		// Change curbuf so the option is read from the correct buffer.
		যদি (do_change_curbuf)
		    curbuf = buf;

		যদি (varname[1] == NUL)
		{
		    // get all window-local or buffer-local options in a dict
		    dict_T	*opts = get_winbuf_options(htname == 'b');

		    যদি (opts != ফাঁকা)
		    {
			rettv_dict_set(rettv, opts);
			done = TRUE;
		    }
		}
		অন্যথা যদি (eval_option(&varname, rettv, TRUE) == OK)
		    // Local option
		    done = TRUE;

		curbuf = save_curbuf;
	    }
	    অন্যথা যদি (*varname == NUL)
	    {
		// Empty string: return a dict with all the local variables.
		যদি (htname == 'b')
		    v = &buf->b_bufvar;
		অন্যথা যদি (htname == 'w')
		    v = &win->w_winvar;
		অন্যথা
		    v = &tp->tp_winvar;
		copy_tv(&v->di_tv, rettv);
		done = TRUE;
	    }
	    অন্যথা
	    {
		hashtab_T	*ht;

		যদি (htname == 'b')
		    ht = &buf->b_vars->dv_hashtab;
		অন্যথা যদি (htname == 'w')
		    ht = &win->w_vars->dv_hashtab;
		অন্যথা
		    ht = &tp->tp_vars->dv_hashtab;

		// Look up the variable.
		v = find_var_in_ht(ht, htname, varname, FALSE);
		যদি (v != ফাঁকা)
		{
		    copy_tv(&v->di_tv, rettv);
		    done = TRUE;
		}
	    }
	}

	যদি (need_switch_win)
	    // restore previous notion of curwin
	    restore_win(&switchwin, TRUE);
    }

    যদি (!done && deftv->v_type != VAR_UNKNOWN)
	// use the default value
	copy_tv(deftv, rettv);

    --emsg_off;
}

/*
 * getwinvar() and gettabwinvar()
 */
    স্থির শূন্য
getwinvar(
    typval_T	*argvars,
    typval_T	*rettv,
    পূর্ণসংখ্যা		off)	    // 1 for gettabwinvar()
{
    char_u	*varname;
    tabpage_T	*tp;
    win_T	*win;

    যদি (off == 1)
	tp = find_tabpage((পূর্ণসংখ্যা)tv_get_সংখ্যা_chk(&argvars[0], ফাঁকা));
    অন্যথা
	tp = curtab;
    win = find_win_by_nr(&argvars[off], tp);
    varname = tv_get_string_chk(&argvars[off + 1]);

    get_var_from(varname, rettv, &argvars[off + 2], 'w', tp, win, ফাঁকা);
}

/*
 * Set option "varname" to the value of "varp" for the current buffer/window.
 */
    স্থির শূন্য
set_option_from_tv(char_u *varname, typval_T *varp)
{
    long	numval = 0;
    char_u	*strval;
    char_u	nbuf[NUMBUFLEN];
    পূর্ণসংখ্যা		error = FALSE;

    পূর্ণসংখ্যা		opt_idx = findoption(varname);
    যদি (opt_idx < 0)
    {
	semsg(_(e_unknown_option_str_2), varname);
	ফেরত;
    }
    পূর্ণসংখ্যা		opt_p_flags = get_option_flags(opt_idx);

    যদি (varp->v_type == VAR_BOOL)
    {
	যদি (opt_p_flags & P_STRING)
	{
	    emsg(_(e_string_required));
	    ফেরত;
	}
	numval = (long)varp->vval.v_সংখ্যা;
	strval = (char_u *)"0";  // avoid using "false"
    }
    অন্যথা
    {
	যদি ((opt_p_flags & (P_অঙ্ক|P_BOOL))
		&& (!in_vim9script() || varp->v_type != VAR_STRING))
	    numval = (long)tv_get_সংখ্যা_chk(varp, &error);
	যদি (!error)
	    strval = tv_get_string_buf_chk(varp, nbuf);
    }
    যদি (!error && strval != ফাঁকা)
	set_option_মান_give_err(varname, numval, strval, OPT_LOCAL);
}

/*
 * "setwinvar()" and "settabwinvar()" functions
 */
    স্থির শূন্য
setwinvar(typval_T *argvars, পূর্ণসংখ্যা off)
{
    win_T	*win;
    switchwin_T	switchwin;
    পূর্ণসংখ্যা		need_switch_win;
    char_u	*varname, *winvarname;
    typval_T	*varp;
    tabpage_T	*tp = ফাঁকা;

    যদি (check_secure())
	ফেরত;

    যদি (off == 1)
	tp = find_tabpage((পূর্ণসংখ্যা)tv_get_সংখ্যা_chk(&argvars[0], ফাঁকা));
    অন্যথা
	tp = curtab;
    win = find_win_by_nr(&argvars[off], tp);
    varname = tv_get_string_chk(&argvars[off + 1]);
    varp = &argvars[off + 2];

    যদি (win == ফাঁকা || varname == ফাঁকা)
	ফেরত;

    need_switch_win = !(tp == curtab && win == curwin);
    যদি (!need_switch_win
	    || switch_win(&switchwin, win, tp, TRUE) == OK)
    {
	যদি (*varname == '&')
	    set_option_from_tv(varname + 1, varp);
	অন্যথা
	{
	    winvarname = alloc(STRLEN(varname) + 3);
	    যদি (winvarname != ফাঁকা)
	    {
		STRCPY(winvarname, "w:");
		STRCPY(winvarname + 2, varname);
		set_var(winvarname, varp, TRUE);
		vim_free(winvarname);
	    }
	}
    }
    যদি (need_switch_win)
	restore_win(&switchwin, TRUE);
}

/*
 * reset v:option_new, v:option_old, v:option_oldlocal, v:option_oldglobal,
 * v:option_type, and v:option_command.
 */
    শূন্য
reset_v_option_vars(শূন্য)
{
    set_vim_var_string(VV_OPTION_NEW, ফাঁকা, -1);
    set_vim_var_string(VV_OPTION_OLD, ফাঁকা, -1);
    set_vim_var_string(VV_OPTION_OLDLOCAL, ফাঁকা, -1);
    set_vim_var_string(VV_OPTION_OLDGLOBAL, ফাঁকা, -1);
    set_vim_var_string(VV_OPTION_TYPE, ফাঁকা, -1);
    set_vim_var_string(VV_OPTION_COMMAND, ফাঁকা, -1);
}

/*
 * Add an assert error to v:errors.
 */
    শূন্য
assert_error(garray_T *gap)
{
    কাঠামো vimvar   *vp = &vimvars[VV_ERRORS];

    যদি (vp->vv_tv_type != VAR_LIST || vimvars[VV_ERRORS].vv_list == ফাঁকা)
	// Make sure v:errors is a list.
	set_vim_var_list(VV_ERRORS, list_alloc());
    list_append_string(vimvars[VV_ERRORS].vv_list, gap->ga_তথ্য, gap->ga_len);
}

    পূর্ণসংখ্যা
var_exists(char_u *var)
{
    char_u	*arg = var;
    char_u	*name;
    char_u	*tofree;
    typval_T    tv;
    পূর্ণসংখ্যা		len = 0;
    পূর্ণসংখ্যা		n = FALSE;

    // get_name_len() takes care of expanding curly braces
    name = var;
    len = get_name_len(&arg, &tofree, TRUE, FALSE);
    যদি (len > 0)
    {
	যদি (tofree != ফাঁকা)
	    name = tofree;
	n = (eval_variable(name, len, 0, &tv, ফাঁকা,
				 EVAL_VAR_NOAUTOLOAD + EVAL_VAR_IMPORT) == OK);
	যদি (n)
	{
	    // handle d.key, l[idx], f(expr)
	    arg = skipwhite(arg);
	    n = (handle_subscript(&arg, name, &tv, &EVALARG_EVALUATE,
								 FALSE) == OK);
	    যদি (n)
		clear_tv(&tv);
	}
    }
    যদি (*arg != NUL)
	n = FALSE;

    vim_free(tofree);
    ফেরত n;
}

স্থির lval_T	*redir_lval = ফাঁকা;
#define EVALCMD_BUSY (redir_lval == (lval_T *)&redir_lval)
স্থির garray_T redir_ga;	// only valid when redir_lval is not NULL
স্থির char_u	*redir_endp = ফাঁকা;
স্থির char_u	*redir_varname = ফাঁকা;

    পূর্ণসংখ্যা
alloc_redir_lval(শূন্য)
{
    redir_lval = ALLOC_CLEAR_ONE(lval_T);
    যদি (redir_lval == ফাঁকা)
	ফেরত FAIL;
    ফেরত OK;
}

    শূন্য
clear_redir_lval(শূন্য)
{
    VIM_CLEAR(redir_lval);
}

    শূন্য
init_redir_ga(শূন্য)
{
    ga_init2(&redir_ga, আকার(অক্ষর), 500);
}

/*
 * Start recording command output to a variable
 * When "append" is TRUE append to an existing variable.
 * Returns OK if successfully completed the setup.  FAIL otherwise.
 */
    পূর্ণসংখ্যা
var_redir_start(char_u *name, পূর্ণসংখ্যা append)
{
    পূর্ণসংখ্যা		called_emsg_before;
    typval_T	tv;

    // Catch a bad name early.
    যদি (!eval_isnamec1(*name))
    {
	emsg(_(e_invalid_argument));
	ফেরত FAIL;
    }

    // Make a copy of the name, it is used in redir_lval until redir ends.
    redir_varname = vim_strsave(name);
    যদি (redir_varname == ফাঁকা)
	ফেরত FAIL;

    যদি (alloc_redir_lval() == FAIL)
    {
	var_redir_stop();
	ফেরত FAIL;
    }

    // The output is stored in growarray "redir_ga" until redirection ends.
    init_redir_ga();

    // Parse the variable name (can be a dict or list entry).
    redir_endp = get_lval(redir_varname, ফাঁকা, redir_lval, FALSE, FALSE, 0,
							     FNE_CHECK_START);
    যদি (redir_endp == ফাঁকা || redir_lval->ll_name == ফাঁকা || *redir_endp != NUL)
    {
	clear_lval(redir_lval);
	যদি (redir_endp != ফাঁকা && *redir_endp != NUL)
	    // Trailing characters are present after the variable name
	    semsg(_(e_trailing_characters_str), redir_endp);
	অন্যথা
	    semsg(_(e_invalid_argument_str), name);
	redir_endp = ফাঁকা;  // don't store a value, only cleanup
	var_redir_stop();
	ফেরত FAIL;
    }

    // check if we can write to the variable: set it to or append an empty
    // string
    called_emsg_before = called_emsg;
    tv.v_type = VAR_STRING;
    tv.vval.v_string = (char_u *)"";
    যদি (append)
	set_var_lval(redir_lval, redir_endp, &tv, TRUE,
					     ASSIGN_NO_DECL, (char_u *)".", 0);
    অন্যথা
	set_var_lval(redir_lval, redir_endp, &tv, TRUE,
					     ASSIGN_NO_DECL, (char_u *)"=", 0);
    clear_lval(redir_lval);
    যদি (called_emsg > called_emsg_before)
    {
	redir_endp = ফাঁকা;  // don't store a value, only cleanup
	var_redir_stop();
	ফেরত FAIL;
    }

    ফেরত OK;
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
    শূন্য
var_redir_str(char_u *মান, পূর্ণসংখ্যা মান_len)
{
    পূর্ণসংখ্যা		len;

    যদি (redir_lval == ফাঁকা)
	ফেরত;

    যদি (মান_len == -1)
	len = (পূর্ণসংখ্যা)STRLEN(মান);	// Append the entire string
    অন্যথা
	len = মান_len;		// Append only "value_len" characters

    যদি (ga_grow(&redir_ga, len) == OK)
    {
	mch_memmove((অক্ষর *)redir_ga.ga_তথ্য + redir_ga.ga_len, মান, len);
	redir_ga.ga_len += len;
    }
    অন্যথা
	var_redir_stop();
}

/*
 * Stop redirecting command output to a variable.
 * Frees the allocated memory.
 */
    শূন্য
var_redir_stop(শূন্য)
{
    typval_T	tv;

    যদি (EVALCMD_BUSY)
    {
	redir_lval = ফাঁকা;
	ফেরত;
    }

    যদি (redir_lval != ফাঁকা)
    {
	// If there was no error: assign the text to the variable.
	যদি (redir_endp != ফাঁকা)
	{
	    ga_append(&redir_ga, NUL);  // Append the trailing NUL.
	    tv.v_type = VAR_STRING;
	    tv.vval.v_string = redir_ga.ga_তথ্য;
	    // Call get_lval() again, if it's inside a Dict or List it may
	    // have changed.
	    redir_endp = get_lval(redir_varname, ফাঁকা, redir_lval,
					FALSE, FALSE, 0, FNE_CHECK_START);
	    যদি (redir_endp != ফাঁকা && redir_lval->ll_name != ফাঁকা)
		set_var_lval(redir_lval, redir_endp, &tv, FALSE, 0,
							     (char_u *)".", 0);
	    clear_lval(redir_lval);
	}

	// free the collected output
	VIM_CLEAR(redir_ga.ga_তথ্য);

	VIM_CLEAR(redir_lval);
    }
    VIM_CLEAR(redir_varname);
}

/*
 * Get the collected redirected text and clear redir_ga.
 */
    char_u *
get_clear_redir_ga(শূন্য)
{
    char_u *res;

    ga_append(&redir_ga, NUL);  // Append the trailing NUL.
    res = redir_ga.ga_তথ্য;
    redir_ga.ga_তথ্য = ফাঁকা;
    ফেরত res;
}

/*
 * "gettabvar()" function
 */
    শূন্য
f_gettabvar(typval_T *argvars, typval_T *rettv)
{
    char_u	*varname;
    tabpage_T	*tp;
    win_T	*win = ফাঁকা;

    যদি (in_vim9script()
	    && (check_for_সংখ্যা_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ফেরত;

    varname = tv_get_string_chk(&argvars[1]);
    tp = find_tabpage((পূর্ণসংখ্যা)tv_get_সংখ্যা_chk(&argvars[0], ফাঁকা));
    যদি (tp != ফাঁকা)
	win = tp == curtab || tp->tp_firstwin == ফাঁকা ? firstwin
		: tp->tp_firstwin;

    get_var_from(varname, rettv, &argvars[2], 't', tp, win, ফাঁকা);
}

/*
 * "gettabwinvar()" function
 */
    শূন্য
f_gettabwinvar(typval_T *argvars, typval_T *rettv)
{
    যদি (in_vim9script()
	    && (check_for_সংখ্যা_arg(argvars, 0) == FAIL
		|| check_for_সংখ্যা_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL))
	ফেরত;

    getwinvar(argvars, rettv, 1);
}

/*
 * "getwinvar()" function
 */
    শূন্য
f_getwinvar(typval_T *argvars, typval_T *rettv)
{
    যদি (in_vim9script()
	    && (check_for_সংখ্যা_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ফেরত;

    getwinvar(argvars, rettv, 0);
}

/*
 * "getbufvar()" function
 */
    শূন্য
f_getbufvar(typval_T *argvars, typval_T *rettv)
{
    char_u	*varname;
    buf_T	*buf;

    যদি (in_vim9script()
	    && (check_for_বাফার_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ফেরত;

    varname = tv_get_string_chk(&argvars[1]);
    buf = tv_get_buf_from_arg(&argvars[0]);

    get_var_from(varname, rettv, &argvars[2], 'b', curtab, curwin, buf);
}

/*
 * "settabvar()" function
 */
    শূন্য
f_settabvar(typval_T *argvars, typval_T *rettv UNUSED)
{
    tabpage_T	*save_curtab;
    tabpage_T	*tp;
    tabpage_T	*save_lu_tp;
    char_u	*varname, *tabvarname;
    typval_T	*varp;

    যদি (check_secure())
	ফেরত;

    যদি (in_vim9script()
	    && (check_for_সংখ্যা_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ফেরত;

    tp = find_tabpage((পূর্ণসংখ্যা)tv_get_সংখ্যা_chk(&argvars[0], ফাঁকা));
    varname = tv_get_string_chk(&argvars[1]);
    varp = &argvars[2];

    যদি (varname == ফাঁকা || tp == ফাঁকা)
	ফেরত;

    save_curtab = curtab;
    save_lu_tp = lastused_tabpage;
    goto_tabpage_tp(tp, FALSE, FALSE);

    tabvarname = alloc(STRLEN(varname) + 3);
    যদি (tabvarname != ফাঁকা)
    {
	STRCPY(tabvarname, "t:");
	STRCPY(tabvarname + 2, varname);
	set_var(tabvarname, varp, TRUE);
	vim_free(tabvarname);
    }

    // Restore current tabpage and last accessed tabpage.
    যদি (valid_tabpage(save_curtab))
    {
	goto_tabpage_tp(save_curtab, FALSE, FALSE);
	যদি (valid_tabpage(save_lu_tp))
	    lastused_tabpage = save_lu_tp;
    }
}

/*
 * "settabwinvar()" function
 */
    শূন্য
f_settabwinvar(typval_T *argvars, typval_T *rettv UNUSED)
{
    যদি (in_vim9script()
	    && (check_for_সংখ্যা_arg(argvars, 0) == FAIL
		|| check_for_সংখ্যা_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL))
	ফেরত;

    setwinvar(argvars, 1);
}

/*
 * "setwinvar()" function
 */
    শূন্য
f_setwinvar(typval_T *argvars, typval_T *rettv UNUSED)
{
    যদি (in_vim9script()
	    && (check_for_সংখ্যা_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ফেরত;

    setwinvar(argvars, 0);
}

/*
 * "setbufvar()" function
 */
    শূন্য
f_setbufvar(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    char_u	*varname, *bufvarname;
    typval_T	*varp;

    যদি (check_secure())
	ফেরত;

    যদি (in_vim9script()
	    && (check_for_বাফার_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ফেরত;

    varname = tv_get_string_chk(&argvars[1]);
    buf = tv_get_buf_from_arg(&argvars[0]);
    varp = &argvars[2];

    যদি (buf == ফাঁকা || varname == ফাঁকা)
	ফেরত;

    যদি (*varname == '&')
    {
	aco_save_T	aco;
	// safe the current window position, it could
	// change because of 'scrollbind' window-local
	// options
	linenr_T	old_topline = curwin->w_topline;

	// Set curbuf to be our buf, temporarily.
	aucmd_prepbuf(&aco, buf);
	যদি (curbuf == buf)
	{
	    // Only when it worked to set "curbuf".
	    set_option_from_tv(varname + 1, varp);

	    // reset notion of buffer
	    aucmd_restbuf(&aco);
	}
	curwin->w_topline = old_topline;
    }
    অন্যথা
    {
	bufvarname = alloc(STRLEN(varname) + 3);
	যদি (bufvarname != ফাঁকা)
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
    পূর্ণসংখ্যা		r = OK;

    CLEAR_FIELD(res);
    যদি (arg->v_type == VAR_PARTIAL && arg->vval.v_partial != ফাঁকা)
    {
	res.cb_partial = arg->vval.v_partial;
	++res.cb_partial->pt_refcount;
	res.cb_name = partial_name(res.cb_partial);
    }
    অন্যথা
    {
	যদি (arg->v_type == VAR_STRING && arg->vval.v_string != ফাঁকা
					       && SAFE_isdigit(*arg->vval.v_string))
	    r = FAIL;
	অন্যথা যদি (arg->v_type == VAR_FUNC || arg->v_type == VAR_STRING)
	{
	    res.cb_name = arg->vval.v_string;
	    যদি (arg->v_type == VAR_STRING)
	    {
		char_u *name = get_scriptlocal_funcname(arg->vval.v_string);
		যদি (name != ফাঁকা)
		{
		    res.cb_name = name;
		    res.cb_free_name = TRUE;
		}
	    }
	    func_ref(res.cb_name);
	}
	অন্যথা যদি (arg->v_type == VAR_সংখ্যা && arg->vval.v_সংখ্যা == 0)
	    res.cb_name = (char_u *)"";
	অন্যথা
	    r = FAIL;

	যদি (r == FAIL)
	{
	    emsg(_(e_invalid_callback_argument));
	    res.cb_name = ফাঁকা;
	}
    }
    ফেরত res;
}

/*
 * Copy a callback into a typval_T.
 */
    শূন্য
put_callback(callback_T *cb, typval_T *tv)
{
    যদি (cb->cb_partial != ফাঁকা)
    {
	tv->v_type = VAR_PARTIAL;
	tv->vval.v_partial = cb->cb_partial;
	++tv->vval.v_partial->pt_refcount;
    }
    অন্যথা
    {
	tv->v_type = VAR_FUNC;
	tv->vval.v_string = vim_strsave(cb->cb_name);
	func_ref(cb->cb_name);
    }
}

    স্থির bool
does_callback_own_cb_name(callback_T *cb)
{
    // If cb_partial != NULL then *cb->cb_name is owned by the partial.
    ফেরত cb->cb_partial || cb->cb_free_name;
}

/*
 * Make a copy of "src" into "dest", allocating the function name if needed,
 * without incrementing the refcount.
 */
    শূন্য
set_callback(callback_T *dest, callback_T *src)
{
    *dest = *src;
    যদি (!does_callback_own_cb_name(src))
    {
	dest->cb_name = vim_strsave(src->cb_name);
	dest->cb_free_name = TRUE;
    }
    *src = (callback_T){0};
}

/*
 * Copy callback from "src" to "dest", incrementing the refcounts.
 */
    শূন্য
copy_callback(callback_T *dest, callback_T *src)
{
    dest->cb_partial = src->cb_partial;
    যদি (dest->cb_partial != ফাঁকা)
    {
	dest->cb_name = src->cb_name;
	dest->cb_free_name = FALSE;
	++dest->cb_partial->pt_refcount;
    }
    অন্যথা
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
    শূন্য
expand_autload_callback(callback_T *cb)
{
    char_u	*name;
    char_u	*p;
    imported_T	*import;

    যদি (!in_vim9script() || cb->cb_name == ফাঁকা
	    || (!cb->cb_free_name
	       && (cb->cb_partial == ফাঁকা || cb->cb_partial->pt_name == ফাঁকা)))
	ফেরত;
    যদি (cb->cb_partial != ফাঁকা)
	name = cb->cb_partial->pt_name;
    অন্যথা
	name = cb->cb_name;
    p = vim_strchr(name, '.');
    যদি (p == ফাঁকা)
	ফেরত;

    import = find_imported(name, p - name, FALSE);
    যদি (import == ফাঁকা || !SCRIPT_ID_VALID(import->imp_sid))
	ফেরত;

    scriptitem_T *si = SCRIPT_ITEM(import->imp_sid);
    যদি (si->sn_autoload_prefix == ফাঁকা)
	ফেরত;

    char_u *newname = concat_str(si->sn_autoload_prefix, p + 1);
    যদি (newname == ফাঁকা)
	ফেরত;

    যদি (cb->cb_partial != ফাঁকা)
    {
	যদি (cb->cb_name == cb->cb_partial->pt_name)
	    cb->cb_name = newname;
	vim_free(cb->cb_partial->pt_name);
	cb->cb_partial->pt_name = newname;
    }
    অন্যথা
    {
	vim_free(cb->cb_name);
	cb->cb_name = newname;
    }
}

/*
 * Unref/free "callback" returned by get_callback() or set_callback().
 */
    শূন্য
free_callback(callback_T *callback)
{
    যদি (callback->cb_partial != ফাঁকা)
    {
	partial_unref(callback->cb_partial);
	callback->cb_partial = ফাঁকা;
    }
    অন্যথা যদি (callback->cb_name != ফাঁকা)
	func_unref(callback->cb_name);
    যদি (callback->cb_free_name)
    {
	vim_free(callback->cb_name);
	callback->cb_free_name = FALSE;
    }
    callback->cb_name = ফাঁকা;
}

#endif // FEAT_EVAL
