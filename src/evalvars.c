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

প্রকারনির্দেশ সংরচনা vimvar vimvar_T;

স্থির সংরচনা vimvar
{
    বর্ণ	*vv_name;	// name of variable, without v:
    dictitem16_T vv_di;		// value and name for key (max 16 chars!)
    type_T	*vv_type;	// type or NULL
    বর্ণ	vv_flags;	// VV_COMPAT, VV_RO, VV_RO_SBX
} vimvars[VV_LEN] =
{
    // The order here must match the VV_ defines in vim.h!
    // Initializing a union does not work, leave tv.vval empty to get zero's.
    {VV_NAME("count",		 VAR_সংখ্যা), রিক্ত, VV_COMPAT+VV_RO},
    {VV_NAME("count1",		 VAR_সংখ্যা), রিক্ত, VV_RO},
    {VV_NAME("prevcount",	 VAR_সংখ্যা), রিক্ত, VV_RO},
    {VV_NAME("errmsg",		 VAR_STRING), রিক্ত, VV_COMPAT},
    {VV_NAME("warningmsg",	 VAR_STRING), রিক্ত, 0},
    {VV_NAME("statusmsg",	 VAR_STRING), রিক্ত, 0},
    {VV_NAME("shell_error",	 VAR_সংখ্যা), রিক্ত, VV_COMPAT+VV_RO},
    {VV_NAME("this_session",	 VAR_STRING), রিক্ত, VV_COMPAT},
    {VV_NAME("version",		 VAR_সংখ্যা), রিক্ত, VV_COMPAT+VV_RO},
    {VV_NAME("lnum",		 VAR_সংখ্যা), রিক্ত, VV_RO_SBX},
    {VV_NAME("termresponse",	 VAR_STRING), রিক্ত, VV_RO},
    {VV_NAME("fname",		 VAR_STRING), রিক্ত, VV_RO},
    {VV_NAME("lang",		 VAR_STRING), রিক্ত, VV_RO},
    {VV_NAME("lc_time",		 VAR_STRING), রিক্ত, VV_RO},
    {VV_NAME("ctype",		 VAR_STRING), রিক্ত, VV_RO},
    {VV_NAME("charconvert_from", VAR_STRING), রিক্ত, VV_RO},
    {VV_NAME("charconvert_to",	 VAR_STRING), রিক্ত, VV_RO},
    {VV_NAME("fname_in",	 VAR_STRING), রিক্ত, VV_RO},
    {VV_NAME("fname_out",	 VAR_STRING), রিক্ত, VV_RO},
    {VV_NAME("fname_new",	 VAR_STRING), রিক্ত, VV_RO},
    {VV_NAME("fname_diff",	 VAR_STRING), রিক্ত, VV_RO},
    {VV_NAME("cmdarg",		 VAR_STRING), রিক্ত, VV_RO},
    {VV_NAME("foldstart",	 VAR_সংখ্যা), রিক্ত, VV_RO_SBX},
    {VV_NAME("foldend",		 VAR_সংখ্যা), রিক্ত, VV_RO_SBX},
    {VV_NAME("folddashes",	 VAR_STRING), রিক্ত, VV_RO_SBX},
    {VV_NAME("foldlevel",	 VAR_সংখ্যা), রিক্ত, VV_RO_SBX},
    {VV_NAME("progname",	 VAR_STRING), রিক্ত, VV_RO},
    {VV_NAME("servername",	 VAR_STRING), রিক্ত, VV_RO},
    {VV_NAME("dying",		 VAR_সংখ্যা), রিক্ত, VV_RO},
    {VV_NAME("exception",	 VAR_STRING), রিক্ত, VV_RO},
    {VV_NAME("throwpoint",	 VAR_STRING), রিক্ত, VV_RO},
    {VV_NAME("register",	 VAR_STRING), রিক্ত, VV_RO},
    {VV_NAME("cmdbang",		 VAR_সংখ্যা), রিক্ত, VV_RO},
    {VV_NAME("insertmode",	 VAR_STRING), রিক্ত, VV_RO},
    {VV_NAME("val",		 VAR_UNKNOWN), রিক্ত, VV_RO},
    {VV_NAME("key",		 VAR_UNKNOWN), রিক্ত, VV_RO},
    {VV_NAME("profiling",	 VAR_সংখ্যা), রিক্ত, VV_RO},
    {VV_NAME("fcs_reason",	 VAR_STRING), রিক্ত, VV_RO},
    {VV_NAME("fcs_choice",	 VAR_STRING), রিক্ত, 0},
    {VV_NAME("beval_bufnr",	 VAR_সংখ্যা), রিক্ত, VV_RO},
    {VV_NAME("beval_winnr",	 VAR_সংখ্যা), রিক্ত, VV_RO},
    {VV_NAME("beval_winid",	 VAR_সংখ্যা), রিক্ত, VV_RO},
    {VV_NAME("beval_lnum",	 VAR_সংখ্যা), রিক্ত, VV_RO},
    {VV_NAME("beval_col",	 VAR_সংখ্যা), রিক্ত, VV_RO},
    {VV_NAME("beval_text",	 VAR_STRING), রিক্ত, VV_RO},
    {VV_NAME("scrollstart",	 VAR_STRING), রিক্ত, 0},
    {VV_NAME("swapname",	 VAR_STRING), রিক্ত, VV_RO},
    {VV_NAME("swapchoice",	 VAR_STRING), রিক্ত, 0},
    {VV_NAME("swapcommand",	 VAR_STRING), রিক্ত, VV_RO},
    {VV_NAME("char",		 VAR_STRING), রিক্ত, 0},
    {VV_NAME("mouse_win",	 VAR_সংখ্যা), রিক্ত, 0},
    {VV_NAME("mouse_winid",	 VAR_সংখ্যা), রিক্ত, 0},
    {VV_NAME("mouse_lnum",	 VAR_সংখ্যা), রিক্ত, 0},
    {VV_NAME("mouse_col",	 VAR_সংখ্যা), রিক্ত, 0},
    {VV_NAME("operator",	 VAR_STRING), রিক্ত, VV_RO},
    {VV_NAME("searchforward",	 VAR_সংখ্যা), রিক্ত, 0},
    {VV_NAME("hlsearch",	 VAR_সংখ্যা), রিক্ত, 0},
    {VV_NAME("oldfiles",	 VAR_LIST), &t_list_string, 0},
    {VV_NAME("windowid",	 VAR_সংখ্যা), রিক্ত, VV_RO},
    {VV_NAME("progpath",	 VAR_STRING), রিক্ত, VV_RO},
    {VV_NAME("completed_item",	 VAR_DICT), &t_dict_string, 0},
    {VV_NAME("option_new",	 VAR_STRING), রিক্ত, VV_RO},
    {VV_NAME("option_old",	 VAR_STRING), রিক্ত, VV_RO},
    {VV_NAME("option_oldlocal",	 VAR_STRING), রিক্ত, VV_RO},
    {VV_NAME("option_oldglobal", VAR_STRING), রিক্ত, VV_RO},
    {VV_NAME("option_command",	 VAR_STRING), রিক্ত, VV_RO},
    {VV_NAME("option_type",	 VAR_STRING), রিক্ত, VV_RO},
    {VV_NAME("errors",		 VAR_LIST), &t_list_string, 0},
    {VV_NAME("false",		 VAR_BOOL), রিক্ত, VV_RO},
    {VV_NAME("true",		 VAR_BOOL), রিক্ত, VV_RO},
    {VV_NAME("none",		 VAR_SPECIAL), রিক্ত, VV_RO},
    {VV_NAME("null",		 VAR_SPECIAL), রিক্ত, VV_RO},
    {VV_NAME("numbermax",	 VAR_সংখ্যা), রিক্ত, VV_RO},
    {VV_NAME("numbermin",	 VAR_সংখ্যা), রিক্ত, VV_RO},
    {VV_NAME("numbersize",	 VAR_সংখ্যা), রিক্ত, VV_RO},
    {VV_NAME("vim_did_enter",	 VAR_সংখ্যা), রিক্ত, VV_RO},
    {VV_NAME("testing",		 VAR_সংখ্যা), রিক্ত, 0},
    {VV_NAME("t_number",	 VAR_সংখ্যা), রিক্ত, VV_RO},
    {VV_NAME("t_string",	 VAR_সংখ্যা), রিক্ত, VV_RO},
    {VV_NAME("t_func",		 VAR_সংখ্যা), রিক্ত, VV_RO},
    {VV_NAME("t_list",		 VAR_সংখ্যা), রিক্ত, VV_RO},
    {VV_NAME("t_dict",		 VAR_সংখ্যা), রিক্ত, VV_RO},
    {VV_NAME("t_float",		 VAR_সংখ্যা), রিক্ত, VV_RO},
    {VV_NAME("t_bool",		 VAR_সংখ্যা), রিক্ত, VV_RO},
    {VV_NAME("t_none",		 VAR_সংখ্যা), রিক্ত, VV_RO},
    {VV_NAME("t_job",		 VAR_সংখ্যা), রিক্ত, VV_RO},
    {VV_NAME("t_channel",	 VAR_সংখ্যা), রিক্ত, VV_RO},
    {VV_NAME("t_blob",		 VAR_সংখ্যা), রিক্ত, VV_RO},
    {VV_NAME("t_class",		 VAR_সংখ্যা), রিক্ত, VV_RO},
    {VV_NAME("t_object",	 VAR_সংখ্যা), রিক্ত, VV_RO},
    {VV_NAME("termrfgresp",	 VAR_STRING), রিক্ত, VV_RO},
    {VV_NAME("termrbgresp",	 VAR_STRING), রিক্ত, VV_RO},
    {VV_NAME("termu7resp",	 VAR_STRING), রিক্ত, VV_RO},
    {VV_NAME("termstyleresp",	 VAR_STRING), রিক্ত, VV_RO},
    {VV_NAME("termblinkresp",	 VAR_STRING), রিক্ত, VV_RO},
    {VV_NAME("event",		 VAR_DICT), রিক্ত, VV_RO},
    {VV_NAME("versionlong",	 VAR_সংখ্যা), রিক্ত, VV_RO},
    {VV_NAME("echospace",	 VAR_সংখ্যা), রিক্ত, VV_RO},
    {VV_NAME("argv",		 VAR_LIST), &t_list_string, VV_RO},
    {VV_NAME("collate",		 VAR_STRING), রিক্ত, VV_RO},
    {VV_NAME("exiting",		 VAR_SPECIAL), রিক্ত, VV_RO},
    {VV_NAME("colornames",       VAR_DICT), &t_dict_string, VV_RO},
    {VV_NAME("sizeofint",	 VAR_সংখ্যা), রিক্ত, VV_RO},
    {VV_NAME("sizeoflong",	 VAR_সংখ্যা), রিক্ত, VV_RO},
    {VV_NAME("sizeofpointer",	 VAR_সংখ্যা), রিক্ত, VV_RO},
    {VV_NAME("maxcol",		 VAR_সংখ্যা), রিক্ত, VV_RO},
    {VV_NAME("python3_version",	 VAR_সংখ্যা), রিক্ত, VV_RO},
    {VV_NAME("t_typealias",	 VAR_সংখ্যা), রিক্ত, VV_RO},
    {VV_NAME("t_enum",		 VAR_সংখ্যা), রিক্ত, VV_RO},
    {VV_NAME("t_enumvalue",	 VAR_সংখ্যা), রিক্ত, VV_RO},
    {VV_NAME("stacktrace",	 VAR_LIST), &t_list_dict_any, VV_RO},
    {VV_NAME("t_tuple",		 VAR_সংখ্যা), রিক্ত, VV_RO},
    {VV_NAME("wayland_display",  VAR_STRING), রিক্ত, VV_RO},
    {VV_NAME("clipmethod",	 VAR_STRING), রিক্ত, VV_RO},
    {VV_NAME("termda1",		 VAR_STRING), রিক্ত, VV_RO},
    {VV_NAME("termosc",	 VAR_STRING), রিক্ত, VV_RO},
    {VV_NAME("vim_did_init",	 VAR_সংখ্যা), রিক্ত, VV_RO},
    {VV_NAME("clipproviders",	 VAR_DICT), রিক্ত, VV_RO},
    {VV_NAME("imagebackend",	 VAR_STRING), রিক্ত, VV_RO}
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

স্থির শূন্য list_glob_vars(পূর্ণাঙ্ক *first);
স্থির শূন্য list_buf_vars(পূর্ণাঙ্ক *first);
স্থির শূন্য list_win_vars(পূর্ণাঙ্ক *first);
স্থির শূন্য list_tab_vars(পূর্ণাঙ্ক *first);
স্থির char_u *list_arg_vars(exarg_T *eap, char_u *arg, পূর্ণাঙ্ক *first);
স্থির char_u *ex_let_one(char_u *arg, typval_T *tv, পূর্ণাঙ্ক copy, পূর্ণাঙ্ক flags, char_u *endchars, char_u *op, পূর্ণাঙ্ক var_idx);
স্থির পূর্ণাঙ্ক do_unlet_var(lval_T *lp, char_u *name_end, exarg_T *eap, পূর্ণাঙ্ক deep, শূন্য *cookie);
স্থির পূর্ণাঙ্ক do_lock_var(lval_T *lp, char_u *name_end, exarg_T *eap, পূর্ণাঙ্ক deep, শূন্য *cookie);
স্থির শূন্য list_one_var(dictitem_T *v, বর্ণ *prefix, পূর্ণাঙ্ক *first);
স্থির শূন্য list_one_var_a(বর্ণ *prefix, char_u *name, পূর্ণাঙ্ক type, char_u *string, পূর্ণাঙ্ক *first);

/*
 * Initialize global and vim special variables
 */
    শূন্য
evalvars_init(শূন্য)
{
    পূর্ণাঙ্ক		    i;
    সংরচনা vimvar   *p;

    init_var_dict(&globvardict, &globvars_var, VAR_DEF_SCOPE);
    init_var_dict(&vimvardict, &vimvars_var, VAR_SCOPE);
    vimvardict.dv_lock = VAR_FIXED;
    hash_init(&compat_hashtab);

    প্রতি (i = 0; i < VV_LEN; ++i)
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
    set_vim_var_nr(VV_NUMBERSIZE, পরিমাণ(varnumber_T) * 8);
    set_vim_var_nr(VV_SIZEOFINT, পরিমাণ(পূর্ণাঙ্ক));
    set_vim_var_nr(VV_SIZEOFLONG, পরিমাণ(long));
    set_vim_var_nr(VV_SIZEOFPOINTER, পরিমাণ(বর্ণ *));
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
    পূর্ণাঙ্ক		    i;
    সংরচনা vimvar   *p;

    প্রতি (i = 0; i < VV_LEN; ++i)
    {
	p = &vimvars[i];
	যদি (p->vv_di.di_tv.v_type == VAR_STRING)
	    VIM_CLEAR(p->vv_str);
	অন্যথা যদি (p->vv_di.di_tv.v_type == VAR_LIST)
	{
	    list_unref(p->vv_list);
	    p->vv_list = রিক্ত;
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
    প্রতি (i = 1; i <= script_বস্তুসমূহ.ga_len; ++i)
	vars_clear(&SCRIPT_VARS(i));
}
#endif

    পূর্ণাঙ্ক
garbage_collect_globvars(পূর্ণাঙ্ক copyID)
{
    প্রত্যাগম set_ref_in_ht(&globvarht, copyID, রিক্ত, রিক্ত);
}

    পূর্ণাঙ্ক
garbage_collect_vimvars(পূর্ণাঙ্ক copyID)
{
    প্রত্যাগম set_ref_in_ht(&vimvarht, copyID, রিক্ত, রিক্ত);
}

    পূর্ণাঙ্ক
garbage_collect_scriptvars(পূর্ণাঙ্ক copyID)
{
    পূর্ণাঙ্ক		    i;
    পূর্ণাঙ্ক		    idx;
    পূর্ণাঙ্ক		    abort = FALSE;
    scriptitem_T    *si;

    প্রতি (i = 1; i <= script_বস্তুসমূহ.ga_len; ++i)
    {
	abort = abort || set_ref_in_ht(&SCRIPT_VARS(i), copyID, রিক্ত, রিক্ত);

	si = SCRIPT_ITEM(i);
	প্রতি (idx = 0; idx < si->sn_var_vals.ga_len; ++idx)
	{
	    svar_T    *sv = ((svar_T *)si->sn_var_vals.ga_data) + idx;

	    যদি (sv->sv_name != রিক্ত)
		abort = abort || set_ref_in_item(sv->sv_tv, copyID, রিক্ত, রিক্ত, রিক্ত);
	}
    }

    প্রত্যাগম abort;
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
    যদি (val == রিক্ত)
	প্রত্যাগম;

    tvp = alloc_string_tv(val);
    যদি (tvp == রিক্ত)
	প্রত্যাগম;

    set_var(name, tvp, FALSE);
    free_tv(tvp);
}

    পূর্ণাঙ্ক
eval_charconvert(
    char_u	*enc_from,
    char_u	*enc_to,
    char_u	*fname_from,
    char_u	*fname_to)
{
    পূর্ণাঙ্ক		err = FALSE;
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;

    set_vim_var_string(VV_CC_FROM, enc_from, -1);
    set_vim_var_string(VV_CC_TO, enc_to, -1);
    set_vim_var_string(VV_FNAME_IN, fname_from, -1);
    set_vim_var_string(VV_FNAME_OUT, fname_to, -1);
    ctx = get_option_sctx("charconvert");
    যদি (ctx != রিক্ত)
	current_sctx = *ctx;

    যদি (eval_to_bool(p_ccv, &err, রিক্ত, FALSE, TRUE))
	err = TRUE;

    set_vim_var_string(VV_CC_FROM, রিক্ত, -1);
    set_vim_var_string(VV_CC_TO, রিক্ত, -1);
    set_vim_var_string(VV_FNAME_IN, রিক্ত, -1);
    set_vim_var_string(VV_FNAME_OUT, রিক্ত, -1);
    current_sctx = saved_sctx;

    যদি (err)
	প্রত্যাগম FAIL;
    প্রত্যাগম OK;
}

#যদি defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
    পূর্ণাঙ্ক
eval_printexpr(char_u *fname, char_u *args)
{
    পূর্ণাঙ্ক		err = FALSE;
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;

    set_vim_var_string(VV_FNAME_IN, fname, -1);
    set_vim_var_string(VV_CMDARG, args, -1);
    ctx = get_option_sctx("printexpr");
    যদি (ctx != রিক্ত)
	current_sctx = *ctx;

    যদি (eval_to_bool(p_pexpr, &err, রিক্ত, FALSE, TRUE))
	err = TRUE;

    set_vim_var_string(VV_FNAME_IN, রিক্ত, -1);
    set_vim_var_string(VV_CMDARG, রিক্ত, -1);
    current_sctx = saved_sctx;

    যদি (err)
    {
	mch_remove(fname);
	প্রত্যাগম FAIL;
    }
    প্রত্যাগম OK;
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
    যদি (ctx != রিক্ত)
	current_sctx = *ctx;

    // errors are ignored
    tv = eval_রাশিমালা_ext(p_dex, রিক্ত, TRUE);
    free_tv(tv);

    set_vim_var_string(VV_FNAME_IN, রিক্ত, -1);
    set_vim_var_string(VV_FNAME_NEW, রিক্ত, -1);
    set_vim_var_string(VV_FNAME_OUT, রিক্ত, -1);
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
    যদি (ctx != রিক্ত)
	current_sctx = *ctx;

    // errors are ignored
    tv = eval_রাশিমালা_ext(p_pex, রিক্ত, TRUE);
    free_tv(tv);

    set_vim_var_string(VV_FNAME_IN, রিক্ত, -1);
    set_vim_var_string(VV_FNAME_DIFF, রিক্ত, -1);
    set_vim_var_string(VV_FNAME_OUT, রিক্ত, -1);
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
    list_T	*list = রিক্ত;
    char_u	*p = skipwhite(রাশিমালা);
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;
    পূর্ণাঙ্ক		r;

    // Set "v:val" to the bad word.
    prepare_vimvar(VV_VAL, &save_val);
    set_vim_var_string(VV_VAL, badword, -1);
    যদি (p_verbose == 0)
	++emsg_off;
    ctx = get_option_sctx("spellsuggest");
    যদি (ctx != রিক্ত)
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

    প্রত্যাগম list;
}

/*
 * "list" is supposed to contain two items: a word and a number.  Return the
 * word in "pp" and the number as the return value.
 * Return -1 if anything isn't right.
 * Used to get the good word and score from the eval_spell_expr() result.
 */
    পূর্ণাঙ্ক
get_spellword(list_T *list, char_u **pp)
{
    listitem_T	*li;

    li = list->lv_first;
    যদি (li == রিক্ত)
	প্রত্যাগম -1;
    *pp = tv_get_string(&li->li_tv);

    li = li->li_next;
    যদি (li == রিক্ত)
	প্রত্যাগম -1;
    প্রত্যাগম (পূর্ণাঙ্ক)tv_get_সংখ্যা(&li->li_tv);
}
#endif

/*
 * Prepare v: variable "idx" to be used.
 * Save the current typeval in "save_tv" and clear it.
 * When not used yet add the variable to the v: hashtable.
 */
    শূন্য
prepare_vimvar(পূর্ণাঙ্ক idx, typval_T *save_tv)
{
    *save_tv = vimvars[idx].vv_tv;
    vimvars[idx].vv_str = রিক্ত;  // don't free it now
    যদি (vimvars[idx].vv_tv_type == VAR_UNKNOWN)
	hash_add(&vimvarht, vimvars[idx].vv_di.di_চাবি, "prepare vimvar");
}

/*
 * Restore v: variable "idx" to typeval "save_tv".
 * Note that the v: variable must have been cleared already.
 * When no longer defined, remove the variable from the v: hashtable.
 */
    শূন্য
restore_vimvar(পূর্ণাঙ্ক idx, typval_T *save_tv)
{
    hashitem_T	*hi;

    vimvars[idx].vv_tv = *save_tv;
    যদি (vimvars[idx].vv_tv_type != VAR_UNKNOWN)
	প্রত্যাগম;

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
list_vim_vars(পূর্ণাঙ্ক *first)
{
    list_hashtable_vars(&vimvarht, "v:", FALSE, first);
}

/*
 * List script-local variables, if there is a script.
 */
    স্থির শূন্য
list_script_vars(পূর্ণাঙ্ক *first)
{
    যদি (SCRIPT_ID_VALID(current_sctx.sc_sid))
	list_hashtable_vars(&SCRIPT_VARS(current_sctx.sc_sid),
							   "s:", FALSE, first);
}

/*
 * Return TRUE if "name" starts with "g:", "w:", "t:" or "b:".
 * But only when an identifier character follows.
 */
    পূর্ণাঙ্ক
is_scoped_variable(char_u *name)
{
    প্রত্যাগম vim_strchr((char_u *)"gwbt", name[0]) != রিক্ত
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
eval_one_রাশিমালা_in_str(char_u *p, garray_T *gap, পূর্ণাঙ্ক evaluate)
{
    char_u	*block_start = skipwhite(p + 1);  // skip the opening {
    char_u	*block_end = block_start;
    char_u	*রাশিমালা_val;

    যদি (*block_start == NUL)
    {
	semsg(_(e_missing_close_curly_str), p);
	প্রত্যাগম রিক্ত;
    }
    যদি (skip_রাশিমালা(&block_end, রিক্ত) == FAIL)
	প্রত্যাগম রিক্ত;
    block_end = skipwhite(block_end);
    যদি (*block_end != '}')
    {
	semsg(_(e_missing_close_curly_str), p);
	প্রত্যাগম রিক্ত;
    }
    যদি (evaluate)
    {
	*block_end = NUL;
	রাশিমালা_val = eval_to_string(block_start, FALSE, FALSE);
	*block_end = '}';
	যদি (রাশিমালা_val == রিক্ত)
	    প্রত্যাগম রিক্ত;
	ga_concat(gap, রাশিমালা_val);
	vim_free(রাশিমালা_val);
    }

    প্রত্যাগম block_end + 1;
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

    যাবত্ (*p != NUL)
    {
	char_u	*lit_start;
	পূর্ণাঙ্ক	escaped_brace = FALSE;

	// Look for a block start.
	lit_start = p;
	যাবত্ (*p != '{' && *p != '}' && *p != NUL)
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
	    প্রত্যাগম রিক্ত;
	}

	// Append the literal part.
	ga_concat_len(&ga, lit_start, (size_t)(p - lit_start));

	যদি (*p == NUL)
	    ভঙ্গ;

	যদি (escaped_brace)
	{
	    // Skip the second brace.
	    ++p;
	    অনুবর্ত;
	}

	// Evaluate the expression and append the result.
	p = eval_one_রাশিমালা_in_str(p, &ga, TRUE);
	যদি (p == রিক্ত)
	{
	    ga_clear(&ga);
	    প্রত্যাগম রিক্ত;
	}
    }
    ga_append(&ga, NUL);

    প্রত্যাগম ga.ga_data;
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
heredoc_get(exarg_T *eap, char_u *cmd, পূর্ণাঙ্ক script_get, পূর্ণাঙ্ক vim9compile)
{
    char_u	*theline = রিক্ত;
    char_u	*marker;
    list_T	*l;
    char_u	*p;
    char_u	*str;
    পূর্ণাঙ্ক		marker_indent_len = 0;
    পূর্ণাঙ্ক		text_indent_len = 0;
    char_u	*text_indent = রিক্ত;
    char_u	dot[] = ".";
    পূর্ণাঙ্ক		comment_char = in_vim9script() ? '#' : '"';
    পূর্ণাঙ্ক		evalstr = FALSE;
    পূর্ণাঙ্ক		eval_failed = FALSE;
    cctx_T	*cctx = vim9compile ? eap->cookie : রিক্ত;
    পূর্ণাঙ্ক		গুনতি = 0;
    পূর্ণাঙ্ক		heredoc_in_string = FALSE;
    char_u	*সারি_arg = রিক্ত;
    char_u	*nl_ptr = vim_strchr(cmd, '\n');

    যদি (nl_ptr != রিক্ত)
    {
	heredoc_in_string = TRUE;
	সারি_arg = nl_ptr + 1;
	*nl_ptr = NUL;
    }
    অন্যথা যদি (eap->ea_getline == রিক্ত)
    {
	emsg(_(e_cannot_use_heredoc_here));
	প্রত্যাগম রিক্ত;
    }

    // Check for the optional 'trim' word before the marker
    cmd = skipwhite(cmd);

    যাবত্ (TRUE)
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
	    যাবত্ (VIM_ISWHITE(*p))
	    {
		p++;
		marker_indent_len++;
	    }
	    text_indent_len = -1;

	    অনুবর্ত;
	}
	যদি (STRNCMP(cmd, "eval", 4) == 0
		&& (cmd[4] == NUL || VIM_ISWHITE(cmd[4])))
	{
	    cmd = skipwhite(cmd + 4);
	    evalstr = TRUE;
	    অনুবর্ত;
	}
	ভঙ্গ;
    }

    // The marker is the next word.
    যদি (*cmd != NUL && *cmd != comment_char)
    {
	marker = skipwhite(cmd);
	p = skiptowhite(marker);
	যদি (*skipwhite(p) != NUL && *skipwhite(p) != comment_char)
	{
	    semsg(_(e_trailing_characters_str), p);
	    প্রত্যাগম রিক্ত;
	}
	*p = NUL;
	যদি (!script_get && vim_islower(*marker))
	{
	    emsg(_(e_marker_cannot_start_with_lower_case_letter));
	    প্রত্যাগম রিক্ত;
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
	    প্রত্যাগম রিক্ত;
	}
    }

    l = list_alloc();
    যদি (l == রিক্ত)
	প্রত্যাগম রিক্ত;

    প্রতি (;;)
    {
	পূর্ণাঙ্ক	mi = 0;
	পূর্ণাঙ্ক	ti = 0;

	যদি (heredoc_in_string)
	{
	    char_u	*next_সারি;

	    // heredoc in a string separated by newlines.  Get the next line
	    // from the string.

	    যদি (*সারি_arg == NUL)
	    {
		semsg(_(e_missing_end_marker_str), marker);
		ভঙ্গ;
	    }

	    theline = সারি_arg;
	    next_সারি = vim_strchr(theline, '\n');
	    যদি (next_সারি == রিক্ত)
		সারি_arg += STRLEN(সারি_arg);
	    অন্যথা
	    {
		*next_সারি = NUL;
		সারি_arg = next_সারি + 1;
	    }
	}
	অন্যথা
	{
	    vim_free(theline);
	    theline = eap->ea_getline(NUL, eap->cookie, 0, FALSE);
	    যদি (theline == রিক্ত)
	    {
		semsg(_(e_missing_end_marker_str), marker);
		ভঙ্গ;
	    }
	}

	// with "trim": skip the indent matching the :let line to find the
	// marker
	যদি (marker_indent_len > 0
		&& STRNCMP(theline, *eap->cmdlinep, marker_indent_len) == 0)
	    mi = marker_indent_len;
	যদি (STRCMP(marker, theline + mi) == 0)
	    ভঙ্গ;

	// If expression evaluation failed in the heredoc, then skip till the
	// end marker.
	যদি (eval_failed)
	    অনুবর্ত;

	যদি (text_indent_len == -1 && *theline != NUL)
	{
	    // set the text indent from the first line.
	    p = theline;
	    text_indent_len = 0;
	    যাবত্ (VIM_ISWHITE(*p))
	    {
		p++;
		text_indent_len++;
	    }
	    text_indent = vim_strnsave(theline, text_indent_len);
	}
	// with "trim": skip the indent matching the first line
	যদি (text_indent != রিক্ত)
	    প্রতি (ti = 0; ti < text_indent_len; ++ti)
		যদি (theline[ti] != text_indent[ti])
		    ভঙ্গ;

	str = theline + ti;
	যদি (vim9compile)
	{
	    যদি (compile_all_রাশিমালা_in_str(str, evalstr, cctx) == FAIL)
	    {
		vim_free(theline);
		vim_free(text_indent);
		list_free(l);
		প্রত্যাগম FAIL;
	    }
	    গুনতি++;
	}
	অন্যথা
	{
	    পূর্ণাঙ্ক	    free_str = FALSE;

	    যদি (evalstr && !eap->skip)
	    {
		str = eval_all_রাশিমালা_in_str(str);
		যদি (str == রিক্ত)
		{
		    // expression evaluation failed
		    eval_failed = TRUE;
		    অনুবর্ত;
		}
		free_str = TRUE;
	    }

	    যদি (list_append_string(l, str, -1) == FAIL)
	    {
		যদি (free_str)
		    vim_free(str);
		ভঙ্গ;
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
	প্রত্যাগম রিক্ত;
    }
    প্রত্যাগম l;
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
    পূর্ণাঙ্ক	    has_var;

    যদি (!in_vim9script())
    {
	semsg(_(e_str_cannot_be_used_in_legacy_vim_script), ":var");
	প্রত্যাগম;
    }
    has_var = checkforcmd_noparen(&p, "var", 3);
    যদি (current_sctx.sc_sid == 0 && has_var)
    {
	emsg(_(e_cannot_declare_variable_on_command_সারি));
	প্রত্যাগম;
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
    char_u	*রাশিমালা = রিক্ত;
    typval_T	rettv;
    পূর্ণাঙ্ক		var_গুনতি = 0;
    পূর্ণাঙ্ক		semicolon = 0;
    char_u	op[4];
    char_u	*argend;
    পূর্ণাঙ্ক		first = TRUE;
    পূর্ণাঙ্ক		concat;
    পূর্ণাঙ্ক		has_assign;
    পূর্ণাঙ্ক		flags = 0;
    পূর্ণাঙ্ক		vim9script = in_vim9script();

    যদি (eap->cmdidx == CMD_final && !vim9script)
    {
	// In legacy Vim script ":final" is short for ":finally".
	ex_finally(eap);
	প্রত্যাগম;
    }
    যদি (eap->cmdidx == CMD_let && vim9script)
    {
	emsg(_(e_cannot_use_let_in_vim9_script));
	প্রত্যাগম;
    }

    যদি (eap->cmdidx == CMD_const)
	flags |= ASSIGN_CONST;
    অন্যথা যদি (eap->cmdidx == CMD_final)
	flags |= ASSIGN_FINAL;

    // Vim9 assignment without ":let", ":const" or ":final"
    যদি (eap->arg == eap->cmd)
	flags |= ASSIGN_NO_DECL;

    argend = skip_var_list(arg, TRUE, &var_গুনতি, &semicolon, FALSE);
    যদি (argend == রিক্ত)
	প্রত্যাগম;
    রাশিমালা = skipwhite(argend);
    concat = রাশিমালা[0] == '.'
	&& ((রাশিমালা[1] == '=' && in_old_script(2))
		|| (রাশিমালা[1] == '.' && রাশিমালা[2] == '='));
    has_assign =  *রাশিমালা == '=' || (vim_strchr((char_u *)"+-*/%", *রাশিমালা) != রিক্ত
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
	প্রত্যাগম;
    }

    যদি (রাশিমালা[0] == '=' && রাশিমালা[1] == '<' && রাশিমালা[2] == '<')
    {
	list_T	*l = রিক্ত;
	long	cur_lnum = SOURCING_LNUM;

	// :let text =<< [trim] [eval] END
	// :var text =<< [trim] [eval] END
	যদি (vim9script && !eap->skip && (!VIM_ISWHITE(রাশিমালা[-1])
						 || !IS_WHITE_OR_NUL(রাশিমালা[3])))
	    semsg(_(e_white_space_required_before_and_after_str_at_str),
								  "=<<", রাশিমালা);
	অন্যথা
	    l = heredoc_get(eap, রাশিমালা + 3, FALSE, FALSE);

	যদি (l != রিক্ত)
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
	প্রত্যাগম;
    }

    evalarg_T   evalarg;
    পূর্ণাঙ্ক	    len = 1;

    CLEAR_FIELD(rettv);

    পূর্ণাঙ্ক cur_lnum;

    op[0] = '=';
    op[1] = NUL;
    যদি (*রাশিমালা != '=')
    {
	যদি (vim9script && (flags & ASSIGN_NO_DECL) == 0)
	{
	    // +=, /=, etc. require an existing variable
	    semsg(_(e_cannot_use_operator_on_new_variable_str), eap->arg);
	}
	অন্যথা যদি (vim_strchr((char_u *)"+-*/%.", *রাশিমালা) != রিক্ত)
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
    পূর্ণাঙ্ক eval_res = eval0(রাশিমালা, &rettv, eap, &evalarg);
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
    পূর্ণাঙ্ক
ex_let_vars(
    char_u	*arg_start,
    typval_T	*tv,
    পূর্ণাঙ্ক		copy,		// copy values from "tv", don't move
    পূর্ণাঙ্ক		semicolon,	// from skip_var_list()
    পূর্ণাঙ্ক		var_গুনতি,	// from skip_var_list()
    পূর্ণাঙ্ক		flags,		// ASSIGN_FINAL, ASSIGN_CONST, etc.
    char_u	*op)
{
    char_u	*arg = arg_start;
    list_T	*l;
    tuple_T	*tuple = রিক্ত;
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		var_idx = 0;
    listitem_T	*item = রিক্ত;
    typval_T	ltv;
    পূর্ণাঙ্ক		is_list = tv->v_type == VAR_LIST;
    পূর্ণাঙ্ক		idx;

    যদি (tv->v_type == VAR_VOID)
    {
	emsg(_(e_cannot_use_void_মান));
	প্রত্যাগম FAIL;
    }
    যদি (*arg != '[')
    {
	// ":let var = expr" or ":for var in list"
	যদি (ex_let_one(arg, tv, copy, flags, op, op, var_idx) == রিক্ত)
	    প্রত্যাগম FAIL;
	প্রত্যাগম OK;
    }

    // ":let [v1, v2] = list" or ":for [v1, v2] in listlist"
    // or
    // ":let [v1, v2] = tuple" or ":for [v1, v2] in tupletuple"
    যদি (tv->v_type != VAR_LIST && tv->v_type != VAR_TUPLE)
    {
	emsg(_(e_list_or_tuple_required));
	প্রত্যাগম FAIL;
    }
    যদি (is_list)
    {
	l = tv->vval.v_list;
	যদি (l == রিক্ত)
	{
	    emsg(_(e_list_required));
	    প্রত্যাগম FAIL;
	}
	i = list_len(l);
    }
    অন্যথা
    {
	tuple = tv->vval.v_tuple;
	যদি (tuple == রিক্ত)
	{
	    emsg(_(e_tuple_required));
	    প্রত্যাগম FAIL;
	}
	i = tuple_len(tuple);
    }

    যদি (semicolon == 0 && var_গুনতি < i)
    {
	emsg(_(is_list ? e_less_targets_than_list_বস্তুসমূহ
					: e_less_targets_than_tuple_বস্তুসমূহ));
	প্রত্যাগম FAIL;
    }
    যদি (var_গুনতি - semicolon > i)
    {
	emsg(_(is_list ? e_more_targets_than_list_বস্তুসমূহ
					: e_more_targets_than_tuple_বস্তুসমূহ));
	প্রত্যাগম FAIL;
    }

    যদি (is_list)
    {
	CHECK_LIST_MATERIALIZE(l);
	item = l->lv_first;
    }
    অন্যথা
	idx = 0;

    যাবত্ (*arg != ']')
    {
	arg = skipwhite(arg + 1);
	++var_idx;
	arg = ex_let_one(arg, is_list ? &item->li_tv : TUPLE_ITEM(tuple, idx),
			 TRUE, flags | ASSIGN_UNPACK, (char_u *)",;]", op,
			 var_idx);
	যদি (is_list)
	    item = item->li_next;
	অন্যথা
	    idx++;
	যদি (arg == রিক্ত)
	    প্রত্যাগম FAIL;

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
		যদি (l == রিক্ত)
		    প্রত্যাগম FAIL;

		// list
		যাবত্ (item != রিক্ত)
		{
		    list_append_tv(l, &item->li_tv);
		    item = item->li_next;
		}

		ltv.v_type = VAR_LIST;
		ltv.v_lock = 0;
		ltv.vval.v_list = l;
		l->lv_refcount = 1;
	    }
	    অন্যথা
	    {
		tuple_T	*new_tuple = tuple_alloc();
		যদি (new_tuple == রিক্ত)
		    প্রত্যাগম FAIL;

		// Put the rest of the tuple (may be empty) in the var
		// after ';'.  Create a new tuple for this.
		যাবত্ (idx < TUPLE_LEN(tuple))
		{
		    typval_T    new_tv;

		    copy_tv(TUPLE_ITEM(tuple, idx), &new_tv);
		    যদি (tuple_append_tv(new_tuple, &new_tv) == FAIL)
		    {
			clear_tv(&new_tv);
			tuple_unref(new_tuple);
			প্রত্যাগম FAIL;
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
	    যদি (arg == রিক্ত)
		প্রত্যাগম FAIL;
	    ভঙ্গ;
	}
	অন্যথা যদি (*arg != ',' && *arg != ']')
	{
	    internal_error("ex_let_vars()");
	    প্রত্যাগম FAIL;
	}
    }

    প্রত্যাগম OK;
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
    পূর্ণাঙ্ক		include_type,
    পূর্ণাঙ্ক		*var_গুনতি,
    পূর্ণাঙ্ক		*semicolon,
    পূর্ণাঙ্ক		silent)
{
    char_u	*p, *s;

    যদি (*arg == '[')
    {
	// "[var, var]": find the matching ']'.
	p = arg;
	প্রতি (;;)
	{
	    p = skipwhite(p + 1);	// skip whites after '[', ';' or ','
	    s = skip_var_one(p, include_type);
	    যদি (s == p)
	    {
		যদি (!silent)
		    semsg(_(e_invalid_argument_str), p);
		প্রত্যাগম রিক্ত;
	    }
	    ++*var_গুনতি;

	    p = skipwhite(s);
	    যদি (*p == ']')
		ভঙ্গ;
	    অন্যথা যদি (*p == ';')
	    {
		যদি (*semicolon == 1)
		{
		    যদি (!silent)
			emsg(_(e_double_semicolon_in_list_of_variables));
		    প্রত্যাগম রিক্ত;
		}
		*semicolon = 1;
	    }
	    অন্যথা যদি (*p != ',')
	    {
		যদি (!silent)
		    semsg(_(e_invalid_argument_str), p);
		প্রত্যাগম রিক্ত;
	    }
	}
	প্রত্যাগম p + 1;
    }

    প্রত্যাগম skip_var_one(arg, include_type);
}

/*
 * Skip one (assignable) variable name, including @r, $VAR, &option, d.key,
 * l[idx].
 * In Vim9 script also skip over ": type" if "include_type" is TRUE.
 */
    char_u *
skip_var_one(char_u *arg, পূর্ণাঙ্ক include_type)
{
    char_u	*end;
    পূর্ণাঙ্ক		vim9 = in_vim9script();

    যদি (*arg == '@' && arg[1] != NUL)
	প্রত্যাগম arg + 2;

    // termcap option name may have non-alpha characters
    যদি (STRNCMP(arg, "&t_", 3) == 0 && arg[3] != NUL && arg[4] != NUL)
	প্রত্যাগম arg + 5;

    end = find_name_end(*arg == '$' || *arg == '&' ? arg + 1 : arg,
				   রিক্ত, রিক্ত, FNE_INCL_BR | FNE_CHECK_START);

    // "a: type" is declaring variable "a" with a type, not "a:".
    // Same for "s: type".
    যদি (vim9 && end == arg + 2 && end[-1] == ':')
	--end;

    যদি (include_type && vim9)
    {
	যদি (*skipwhite(end) == ':')
	    end = skip_type(skipwhite(skipwhite(end) + 1), FALSE);
    }
    প্রত্যাগম end;
}

/*
 * List variables for hashtab "ht" with prefix "prefix".
 * If "empty" is TRUE also list NULL strings as empty strings.
 */
    শূন্য
list_hashtable_vars(
    hashtab_T	*ht,
    বর্ণ	*prefix,
    পূর্ণাঙ্ক		empty,
    পূর্ণাঙ্ক		*first)
{
    hashitem_T	*hi;
    dictitem_T	*di;
    পূর্ণাঙ্ক		todo;
    char_u	buf[IOSIZE];

    পূর্ণাঙ্ক save_ht_flags = ht->ht_flags;
    ht->ht_flags |= HTFLAGS_FROZEN;

    todo = (পূর্ণাঙ্ক)ht->ht_used;
    প্রতি (hi = ht->ht_array; todo > 0 && !got_int; ++hi)
    {
	যদি (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    di = HI2DI(hi);

	    // apply :filter /pat/ to variable name
	    vim_strncpy((char_u *)buf, (char_u *)prefix, IOSIZE - 1);
	    vim_strcat((char_u *)buf, di->di_চাবি, IOSIZE);
	    যদি (message_filtered(buf))
		অনুবর্ত;

	    যদি (empty || di->di_tv.v_type != VAR_STRING
					   || di->di_tv.vval.v_string != রিক্ত)
		list_one_var(di, prefix, first);
	}
    }

    ht->ht_flags = save_ht_flags;
}

/*
 * List global variables.
 */
    স্থির শূন্য
list_glob_vars(পূর্ণাঙ্ক *first)
{
    list_hashtable_vars(&globvarht, "", TRUE, first);
}

/*
 * List buffer variables.
 */
    স্থির শূন্য
list_buf_vars(পূর্ণাঙ্ক *first)
{
    list_hashtable_vars(&curbuf->b_vars->dv_hashtab, "b:", TRUE, first);
}

/*
 * List window variables.
 */
    স্থির শূন্য
list_win_vars(পূর্ণাঙ্ক *first)
{
    list_hashtable_vars(&curwin->w_vars->dv_hashtab, "w:", TRUE, first);
}

/*
 * List tab page variables.
 */
    স্থির শূন্য
list_tab_vars(পূর্ণাঙ্ক *first)
{
    list_hashtable_vars(&curtab->tp_vars->dv_hashtab, "t:", TRUE, first);
}

/*
 * List variables in "arg".
 */
    স্থির char_u *
list_arg_vars(exarg_T *eap, char_u *arg, পূর্ণাঙ্ক *first)
{
    পূর্ণাঙ্ক		error = FALSE;
    পূর্ণাঙ্ক		len;
    char_u	*name;
    char_u	*name_start;
    char_u	*arg_subsc;
    char_u	*tofree;
    typval_T    tv;

    যাবত্ (!ends_excmd2(eap->cmd, arg) && !got_int)
    {
	যদি (error || eap->skip)
	{
	    arg = find_name_end(arg, রিক্ত, রিক্ত, FNE_INCL_BR | FNE_CHECK_START);
	    যদি (!VIM_ISWHITE(*arg) && !ends_excmd(*arg))
	    {
		emsg_severe = TRUE;
		যদি (!did_emsg)
		    semsg(_(e_trailing_characters_str), arg);
		ভঙ্গ;
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
		    ভঙ্গ;
		}
		error = TRUE;
	    }
	    অন্যথা
	    {
		arg = skipwhite(arg);
		যদি (tofree != রিক্ত)
		    name = tofree;
		যদি (eval_variable(name, len, 0, &tv, রিক্ত,
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
			    বিকল্প (*name)
			    {
				পক্ষ 'g': list_glob_vars(first); ভঙ্গ;
				পক্ষ 'b': list_buf_vars(first); ভঙ্গ;
				পক্ষ 'w': list_win_vars(first); ভঙ্গ;
				পক্ষ 't': list_tab_vars(first); ভঙ্গ;
				পক্ষ 'v': list_vim_vars(first); ভঙ্গ;
				পক্ষ 's': list_script_vars(first); ভঙ্গ;
				পক্ষ 'l': list_func_vars(first); ভঙ্গ;
				স্বভাব:
					  semsg(_(e_cant_list_variables_for_str), name);
			    }
			}
			অন্যথা
			{
			    char_u	numbuf[NUMBUFLEN];
			    char_u	*tf;
			    পূর্ণাঙ্ক		c;
			    char_u	*s;

			    s = echo_string(&tv, &tf, numbuf, 0);
			    c = *arg;
			    *arg = NUL;
			    list_one_var_a("",
				    arg == arg_subsc ? name : name_start,
				    tv.v_type,
				    s == রিক্ত ? (char_u *)"" : s,
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

    প্রত্যাগম arg;
}

/*
 * Set an environment variable, part of ex_let_one().
 */
    স্থির char_u *
ex_let_env(
    char_u	*arg,
    typval_T	*tv,
    পূর্ণাঙ্ক		flags,
    char_u	*endchars,
    char_u	*op)
{
    char_u	*arg_end = রিক্ত;
    char_u	*name;
    পূর্ণাঙ্ক		len;

    যদি ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					 && (flags & ASSIGN_FOR_LOOP) == 0)
    {
	emsg(_(e_cannot_lock_environment_variable));
	প্রত্যাগম রিক্ত;
    }

    // Find the end of the name.
    ++arg;
    name = arg;
    len = get_env_len(&arg);
    যদি (len == 0)
	semsg(_(e_invalid_argument_str), name - 1);
    অন্যথা
    {
	যদি (op != রিক্ত && vim_strchr((char_u *)"+-*/%", *op) != রিক্ত)
	    semsg(_(e_wrong_variable_type_for_str_equal), op);
	অন্যথা যদি (endchars != রিক্ত
			      && vim_strchr(endchars, *skipwhite(arg)) == রিক্ত)
	    emsg(_(e_unexpected_characters_in_let));
	অন্যথা যদি (!check_secure() && !check_restricted())
	{
	    char_u	*tofree = রিক্ত;
	    পূর্ণাঙ্ক		c1 = name[len];
	    char_u	*p;

	    name[len] = NUL;
	    p = tv_get_string_chk(tv);
	    যদি (p != রিক্ত && op != রিক্ত && *op == '.')
	    {
		পূর্ণাঙ্ক	mustfree = FALSE;
		char_u  *s = vim_getenv(name, &mustfree);

		যদি (s != রিক্ত)
		{
		    p = tofree = concat_str(s, p);
		    যদি (mustfree)
			vim_free(s);
		}
	    }
	    যদি (p != রিক্ত)
	    {
		vim_setenv_ext(name, p);
		arg_end = arg;
	    }
	    name[len] = c1;
	    vim_free(tofree);
	}
    }
    প্রত্যাগম arg_end;
}

/*
 * Set an option, part of ex_let_one().
 */
    স্থির char_u *
ex_let_option(
    char_u	*arg,
    typval_T	*tv,
    পূর্ণাঙ্ক		flags,
    char_u	*endchars,
    char_u	*op)
{
    char_u	*p;
    পূর্ণাঙ্ক		scope;
    char_u	*arg_end = রিক্ত;

    যদি ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					 && (flags & ASSIGN_FOR_LOOP) == 0)
    {
	emsg(_(e_cannot_lock_option));
	প্রত্যাগম রিক্ত;
    }

    // Find the end of the name.
    p = find_option_end(&arg, &scope);
    যদি (p == রিক্ত || (endchars != রিক্ত
			       && vim_strchr(endchars, *skipwhite(p)) == রিক্ত))
    {
	emsg(_(e_unexpected_characters_in_let));
	প্রত্যাগম রিক্ত;
    }

    পূর্ণাঙ্ক		c1;
    long	n = 0;
    getoption_T	opt_type;
    long	numval;
    char_u	*stringval = রিক্ত;
    char_u	*s = রিক্ত;
    পূর্ণাঙ্ক		failed = FALSE;
    পূর্ণাঙ্ক		opt_p_flags;
    char_u	*tofree = রিক্ত;
    char_u	numbuf[NUMBUFLEN];

    c1 = *p;
    *p = NUL;

    opt_type = get_option_মান(arg, &numval, &stringval, &opt_p_flags, scope);
    যদি (opt_type == gov_unknown && arg[0] != 't' && arg[1] != '_')
    {
	semsg(_(e_unknown_option_str_2), arg);
	গচ্ছ theend;
    }
    যদি (op != রিক্ত && *op != '='
	&& (((opt_type == gov_bool || opt_type == gov_সংখ্যা) && *op == '.')
	    || (opt_type == gov_string && *op != '.')))
    {
	semsg(_(e_wrong_variable_type_for_str_equal), op);
	গচ্ছ theend;
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
	    গচ্ছ theend;
    }

    যদি ((opt_p_flags & P_FUNC) && (tv->v_type == VAR_PARTIAL
						    || tv->v_type == VAR_FUNC))
    {
	// If the option can be set to a function reference or a lambda
	// and the passed value is a function reference, then convert it to
	// the name (string) of the function reference.
	s = tv2string(tv, &tofree, numbuf, 0);
	যদি (s == রিক্ত)
	    গচ্ছ theend;
    }
    // Avoid setting a string option to the text "v:false" or similar.
    // In Vim9 script also don't convert a number to string.
    অন্যথা যদি (tv->v_type != VAR_BOOL && tv->v_type != VAR_SPECIAL
		     && (!in_vim9script() || tv->v_type != VAR_সংখ্যা))
    {
	s = tv_get_string_chk(tv);
	যদি (s == রিক্ত)
	    গচ্ছ theend;
    }
    অন্যথা যদি (opt_type == gov_string || opt_type == gov_hidden_string)
    {
	emsg(_(e_string_required));
	গচ্ছ theend;
    }

    যদি (op != রিক্ত && *op != '=')
    {
	// number, in legacy script also bool
	যদি (opt_type == gov_সংখ্যা
				 || (opt_type == gov_bool && !in_vim9script()))
	{
	    বিকল্প (*op)
	    {
		পক্ষ '+': n = numval + n; ভঙ্গ;
		পক্ষ '-': n = numval - n; ভঙ্গ;
		পক্ষ '*': n = numval * n; ভঙ্গ;
		পক্ষ '/': n = (long)অঙ্ক_divide(numval, n, &failed); ভঙ্গ;
		পক্ষ '%': n = (long)অঙ্ক_modulus(numval, n, &failed); ভঙ্গ;
	    }
	    s = রিক্ত;
	    যদি (failed)
		গচ্ছ theend;
	}
	অন্যথা যদি (opt_type == gov_string && stringval != রিক্ত && s != রিক্ত)
	{
	    // string
	    s = concat_str(stringval, s);
	    vim_free(stringval);
	    stringval = s;
	}
    }

    বর্ণ *err = set_option_মান(arg, n, s, scope);
    arg_end = p;
    যদি (err != রিক্ত)
	emsg(_(err));

theend:
    *p = c1;
    vim_free(stringval);
    vim_free(tofree);
    প্রত্যাগম arg_end;
}

/*
 * Set a register, part of ex_let_one().
 */
    স্থির char_u *
ex_let_register(
    char_u	*arg,
    typval_T	*tv,
    পূর্ণাঙ্ক		flags,
    char_u	*endchars,
    char_u	*op)
{
    char_u	*arg_end = রিক্ত;

    যদি ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					 && (flags & ASSIGN_FOR_LOOP) == 0)
    {
	emsg(_(e_cannot_lock_register));
	প্রত্যাগম রিক্ত;
    }
    ++arg;
    যদি (op != রিক্ত && vim_strchr((char_u *)"+-*/%", *op) != রিক্ত)
	semsg(_(e_wrong_variable_type_for_str_equal), op);
    অন্যথা যদি (endchars != রিক্ত
			  && vim_strchr(endchars, *skipwhite(arg + 1)) == রিক্ত)
	emsg(_(e_unexpected_characters_in_let));
    অন্যথা
    {
	char_u	*ptofree = রিক্ত;
	char_u	*p;

	p = tv_get_string_chk(tv);
	যদি (p != রিক্ত && op != রিক্ত && *op == '.')
	{
	    char_u  *s = get_reg_contents(*arg == '@'
						  ? '"' : *arg, GREG_রাশিমালা_SRC);

	    যদি (s != রিক্ত)
	    {
		p = ptofree = concat_str(s, p);
		vim_free(s);
	    }
	}
	যদি (p != রিক্ত)
	{
	    write_reg_contents(*arg == '@' ? '"' : *arg, p, -1, FALSE);
	    arg_end = arg + 1;
	}
	vim_free(ptofree);
    }
    প্রত্যাগম arg_end;
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
    পূর্ণাঙ্ক		copy,		// copy value from "tv"
    পূর্ণাঙ্ক		flags,		// ASSIGN_CONST, ASSIGN_FINAL, etc.
    char_u	*endchars,	// valid chars after variable name  or NULL
    char_u	*op,		// "+", "-", "."  or NULL
    পূর্ণাঙ্ক		var_idx)	// variable index for "let [a, b] = list"
{
    char_u	*arg_end = রিক্ত;

    যদি (in_vim9script() && (flags & (ASSIGN_NO_DECL | ASSIGN_DECL)) == 0
			&& (flags & (ASSIGN_CONST | ASSIGN_FINAL)) == 0
				  && vim_strchr((char_u *)"$@&", *arg) != রিক্ত)
    {
	vim9_declare_error(arg);
	প্রত্যাগম রিক্ত;
    }

    যদি (check_typval_is_মান(tv) == FAIL)
	প্রত্যাগম রিক্ত;

    // In Vim9 script an environment variable and a register only take a
    // String.  "@#" also takes a buffer number.
    যদি (in_vim9script() && (*arg == '$' || *arg == '@')
	    && !(*arg == '@' && arg[1] == '#' && tv->v_type == VAR_সংখ্যা
					       && (op == রিক্ত || *op == '=')))
    {
	where_T	where = WHERE_INIT;

	যদি (check_typval_type(&t_string, tv, where) == FAIL)
	    প্রত্যাগম রিক্ত;
    }

    যদি (*arg == '$')
    {
	// ":let $VAR = expr": Set environment variable.
	প্রত্যাগম ex_let_env(arg, tv, flags, endchars, op);
    }
    অন্যথা যদি (*arg == '&')
    {
	// ":let &option = expr": Set option value.
	// ":let &l:option = expr": Set local option value.
	// ":let &g:option = expr": Set global option value.
	// ":for &ts in range(8)": Set option value for for loop
	প্রত্যাগম ex_let_option(arg, tv, flags, endchars, op);
    }
    অন্যথা যদি (*arg == '@')
    {
	// ":let @r = expr": Set register contents.
	প্রত্যাগম ex_let_register(arg, tv, flags, endchars, op);
    }
    অন্যথা যদি (eval_isnamec1(*arg) || *arg == '{')
    {
	lval_T	lv;
	char_u	*p;
	পূর্ণাঙ্ক	lval_flags = (flags & (ASSIGN_NO_DECL | ASSIGN_DECL))
							     ? GLV_NO_DECL : 0;
	lval_flags |= (flags & ASSIGN_FOR_LOOP) ? GLV_FOR_LOOP : 0;
	যদি (op != রিক্ত && *op != '=')
	    lval_flags |= GLV_ASSIGN_WITH_OP;

	// ":let var = expr": Set internal variable.
	// ":let var: type = expr": Set internal variable with type.
	// ":let {expr} = expr": Idem, name made with curly braces
	p = get_lval(arg, tv, &lv, FALSE, FALSE, lval_flags, FNE_CHECK_START);
	যদি (p != রিক্ত && lv.ll_name != রিক্ত)
	{
	    যদি (endchars != রিক্ত && vim_strchr(endchars,
					   *skipwhite(lv.ll_name_end)) == রিক্ত)
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

    প্রত্যাগম arg_end;
}

/*
 * ":unlet[!] var1 ... " command.
 */
    শূন্য
ex_unlet(exarg_T *eap)
{
    ex_unletlock(eap, eap->arg, 0, eap->forceit ? GLV_QUIET : 0, do_unlet_var, রিক্ত);
}

/*
 * ":lockvar" and ":unlockvar" commands
 */
    শূন্য
ex_lockvar(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    পূর্ণাঙ্ক		deep = 2;

    যদি (eap->forceit)
	deep = -1;
    অন্যথা যদি (vim_isdigit(*arg))
    {
	deep = getdigits(&arg);
	arg = skipwhite(arg);
    }

    ex_unletlock(eap, arg, deep, 0, do_lock_var, রিক্ত);
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
    পূর্ণাঙ্ক		deep,
    পূর্ণাঙ্ক		glv_flags,
    পূর্ণাঙ্ক		(*callback)(lval_T *, char_u *, exarg_T *, পূর্ণাঙ্ক, শূন্য *),
    শূন্য	*cookie)
{
    char_u	*arg = argstart;
    char_u	*name_end;
    পূর্ণাঙ্ক		error = FALSE;
    lval_T	lv;

    কুরু
    {
	যদি (*arg == '$')
	{
	    lv.ll_name = arg;
	    lv.ll_tv = রিক্ত;
	    ++arg;
	    যদি (get_env_len(&arg) == 0)
	    {
		semsg(_(e_invalid_argument_str), arg - 1);
		প্রত্যাগম;
	    }
	    যদি (!error && !eap->skip
			      && callback(&lv, arg, eap, deep, cookie) == FAIL)
		error = TRUE;
	    name_end = arg;
	}
	অন্যথা
	{
	    // Parse the name and find the end.
	    name_end = get_lval(arg, রিক্ত, &lv, TRUE, eap->skip || error,
				     glv_flags | GLV_NO_DECL, FNE_CHECK_START);
	    যদি (lv.ll_name == রিক্ত)
		error = TRUE;	    // error but continue parsing
	    যদি (name_end == রিক্ত || (!VIM_ISWHITE(*name_end)
						    && !ends_excmd(*name_end)))
	    {
		যদি (name_end != রিক্ত)
		{
		    emsg_severe = TRUE;
		    semsg(_(e_trailing_characters_str), name_end);
		}
		যদি (!(eap->skip || error))
		    clear_lval(&lv);
		ভঙ্গ;
	    }

	    যদি (!error && !eap->skip
			 && callback(&lv, name_end, eap, deep, cookie) == FAIL)
		error = TRUE;

	    যদি (!eap->skip)
		clear_lval(&lv);
	}

	arg = skipwhite(name_end);
    } যাবত্ (!ends_excmd2(name_end, arg));

    set_nextcmd(eap, arg);
}

    স্থির পূর্ণাঙ্ক
do_unlet_var(
    lval_T	*lp,
    char_u	*name_end,
    exarg_T	*eap,
    পূর্ণাঙ্ক		deep UNUSED,
    শূন্য	*cookie UNUSED)
{
    পূর্ণাঙ্ক		forceit = eap->forceit;
    পূর্ণাঙ্ক		ret = OK;
    পূর্ণাঙ্ক		cc;

    যদি (lp->ll_tv == রিক্ত)
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
    অন্যথা যদি ((lp->ll_list != রিক্ত
		 && মান_check_lock(lp->ll_list->lv_lock, lp->ll_name, FALSE))
	    || (lp->ll_dict != রিক্ত
		&& মান_check_lock(lp->ll_dict->dv_lock, lp->ll_name, FALSE)))
	প্রত্যাগম FAIL;
    অন্যথা যদি (lp->ll_range)
	list_unlet_range(lp->ll_list, lp->ll_li, lp->ll_n1,
						    !lp->ll_empty2, lp->ll_n2);
    অন্যথা যদি (lp->ll_list != রিক্ত)
	// unlet a List item.
	listitem_remove(lp->ll_list, lp->ll_li);
    অন্যথা যদি (lp->ll_dict != রিক্ত)
	// unlet a Dictionary item.
	dictitem_remove(lp->ll_dict, lp->ll_di, "unlet");
    অন্যথা
    {
	semsg(_(e_cannot_unlet_imported_item_str), lp->ll_name);
	প্রত্যাগম FAIL;
    }

    প্রত্যাগম ret;
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
	পূর্ণাঙ্ক	    has_n2,
	long	    n2)
{
    listitem_T  *li = li_first;
    পূর্ণাঙ্ক		n1 = n1_arg;

    // Delete a range of List items.
    li = li_first;
    n1 = n1_arg;
    যাবত্ (li != রিক্ত && (!has_n2 || n2 >= n1))
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
    পূর্ণাঙ্ক
do_unlet(char_u *name, পূর্ণাঙ্ক forceit)
{
    hashtab_T	*ht;
    hashitem_T	*hi;
    char_u	*varname = রিক্ত;  // init to shut up gcc
    dict_T	*d;
    dictitem_T	*di;

    // can't :unlet a script variable in Vim9 script
    যদি (in_vim9script() && check_vim9_unlet(name) == FAIL)
	প্রত্যাগম FAIL;

    ht = find_var_ht(name, &varname);

    // can't :unlet a script variable in Vim9 script from a function
    যদি (ht == get_script_local_ht()
	    && SCRIPT_ID_VALID(current_sctx.sc_sid)
	    && SCRIPT_ITEM(current_sctx.sc_sid)->sn_version
							 == SCRIPT_VERSION_VIM9
	    && check_vim9_unlet(name) == FAIL)
	প্রত্যাগম FAIL;

    যদি (ht != রিক্ত && *varname != NUL)
    {
	d = get_current_funccal_dict(ht);
	যদি (d == রিক্ত)
	{
	    যদি (ht == &globvarht)
		d = &globvardict;
	    অন্যথা যদি (ht == &compat_hashtab)
		d = &vimvardict;
	    অন্যথা
	    {
		di = find_var_in_ht(ht, *name, (char_u *)"", FALSE);
		d = di == রিক্ত ? রিক্ত : di->di_tv.vval.v_dict;
	    }
	    যদি (d == রিক্ত)
	    {
		internal_error("do_unlet()");
		প্রত্যাগম FAIL;
	    }
	}
	hi = hash_find(ht, varname);
	যদি (HASHITEM_EMPTY(hi))
	    hi = find_hi_in_scoped_ht(name, &ht);
	যদি (hi != রিক্ত && !HASHITEM_EMPTY(hi))
	{
	    di = HI2DI(hi);
	    যদি (var_check_fixed(di->di_flags, name, FALSE)
		    || var_check_ro(di->di_flags, name, FALSE)
		    || মান_check_lock(d->dv_lock, name, FALSE)
		    || check_hashtab_frozen(ht, "unlet"))
		প্রত্যাগম FAIL;

	    delete_var(ht, hi);
	    প্রত্যাগম OK;
	}
    }
    যদি (forceit)
	প্রত্যাগম OK;
    semsg(_(e_no_such_variable_str), name);
    প্রত্যাগম FAIL;
}

    স্থির শূন্য
report_lockvar_member(বর্ণ *বার্তা, lval_T *lp)
{
    পূর্ণাঙ্ক did_alloc = FALSE;
    char_u *vname = (char_u *)"";
    char_u *class_name = lp->ll_class != রিক্ত
				    ? lp->ll_class->class_name.string : (char_u *)"";
    যদি (lp->ll_name != রিক্ত)
    {
	যদি (lp->ll_name_end == রিক্ত)
	    vname = lp->ll_name;
	অন্যথা
	{
	    vname = vim_strnsave(lp->ll_name, lp->ll_name_end - lp->ll_name);
	    যদি (vname == রিক্ত)
		প্রত্যাগম;
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
    স্থির পূর্ণাঙ্ক
do_lock_var(
    lval_T	*lp,
    char_u	*name_end,
    exarg_T	*eap,
    পূর্ণাঙ্ক		deep,
    শূন্য	*cookie UNUSED)
{
    পূর্ণাঙ্ক		lock = eap->cmdidx == CMD_lockvar;
    পূর্ণাঙ্ক		ret = OK;
    পূর্ণাঙ্ক		cc;
    dictitem_T	*di;

#ifdef LOG_LOCKVAR
    ch_log(রিক্ত, "LKVAR: do_lock_var(): name %s, is_root %d", lp->ll_name, lp->ll_is_মূল);
#endif

    যদি (lp->ll_tv == রিক্ত)
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
	    di = find_var(lp->ll_name, রিক্ত, TRUE);
	    যদি (di == রিক্ত)
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

			যদি (sv != রিক্ত && sv->sv_const != 0)
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
    অন্যথা যদি (deep == 0 && lp->ll_object == রিক্ত && lp->ll_class == রিক্ত)
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
	যাবত্ (li != রিক্ত && (lp->ll_empty2 || lp->ll_n2 >= lp->ll_n1))
	{
	    item_lock(&li->li_tv, deep, lock, FALSE);
	    li = li->li_next;
	    ++lp->ll_n1;
	}
    }
    অন্যথা যদি (lp->ll_list != রিক্ত)
	// (un)lock a List item.
	item_lock(&lp->ll_li->li_tv, deep, lock, FALSE);
    অন্যথা যদি (lp->ll_object != রিক্ত)  // This check must be before ll_class.
    {
	// (un)lock an object variable.
	report_lockvar_member(e_cannot_lock_object_variable_str, lp);
	ret = FAIL;
    }
    অন্যথা যদি (lp->ll_class != রিক্ত)
    {
	// (un)lock a class variable.
	report_lockvar_member(e_cannot_lock_class_variable_str, lp);
	ret = FAIL;
    }
    অন্যথা
    {
	// (un)lock a Dictionary item.
	যদি (lp->ll_di == রিক্ত)
	{
	    emsg(_(e_dictionary_required));
	    ret = FAIL;
	}
	অন্যথা
	    item_lock(&lp->ll_di->di_tv, deep, lock, FALSE);
    }

    প্রত্যাগম ret;
}

/*
 * Lock or unlock an item.  "deep" is nr of levels to go.
 * When "check_refcount" is TRUE do not lock a list or dict with a reference
 * count larger than 1.
 */
    শূন্য
item_lock(typval_T *tv, পূর্ণাঙ্ক deep, পূর্ণাঙ্ক lock, পূর্ণাঙ্ক check_refcount)
{
    স্থির পূর্ণাঙ্ক	recurse = 0;
    list_T	*l;
    listitem_T	*li;
    dict_T	*d;
    blob_T	*b;
    hashitem_T	*hi;
    পূর্ণাঙ্ক		todo;

#ifdef LOG_LOCKVAR
    ch_log(রিক্ত, "LKVAR: item_lock(): type %s", vartype_name(tv->v_type));
#endif

    যদি (recurse >= DICT_MAXNEST)
    {
	emsg(_(e_variable_nested_too_deep_for_unlock));
	প্রত্যাগম;
    }
    যদি (deep == 0)
	প্রত্যাগম;
    ++recurse;

    // lock/unlock the item itself
    যদি (lock)
	tv->v_lock |= VAR_LOCKED;
    অন্যথা
	tv->v_lock &= ~VAR_LOCKED;

    বিকল্প (tv->v_type)
    {
	পক্ষ VAR_UNKNOWN:
	পক্ষ VAR_ANY:
	পক্ষ VAR_VOID:
	পক্ষ VAR_সংখ্যা:
	পক্ষ VAR_BOOL:
	পক্ষ VAR_STRING:
	পক্ষ VAR_FUNC:
	পক্ষ VAR_PARTIAL:
	পক্ষ VAR_FLOAT:
	পক্ষ VAR_SPECIAL:
	পক্ষ VAR_JOB:
	পক্ষ VAR_CHANNEL:
	পক্ষ VAR_INSTR:
	পক্ষ VAR_CLASS:
	পক্ষ VAR_OBJECT:
	পক্ষ VAR_TYPEALIAS:
	    ভঙ্গ;

	পক্ষ VAR_BLOB:
	    যদি ((b = tv->vval.v_blob) != রিক্ত
				    && !(check_refcount && b->bv_refcount > 1))
	    {
		যদি (lock)
		    b->bv_lock |= VAR_LOCKED;
		অন্যথা
		    b->bv_lock &= ~VAR_LOCKED;
	    }
	    ভঙ্গ;
	পক্ষ VAR_LIST:
	    যদি ((l = tv->vval.v_list) != রিক্ত
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
	    ভঙ্গ;
	পক্ষ VAR_TUPLE:
	    tuple_lock(tv->vval.v_tuple, deep, lock, check_refcount);
	    ভঙ্গ;
	পক্ষ VAR_DICT:
	    যদি ((d = tv->vval.v_dict) != রিক্ত
				    && !(check_refcount && d->dv_refcount > 1))
	    {
		যদি (lock)
		    d->dv_lock |= VAR_LOCKED;
		অন্যথা
		    d->dv_lock &= ~VAR_LOCKED;
		যদি (deep < 0 || deep > 1)
		{
		    // recursive: lock/unlock the items the List contains
		    todo = (পূর্ণাঙ্ক)d->dv_hashtab.ht_used;
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
    পূর্ণাঙ্ক		todo;

    hash_lock(&globvarht);
    todo = (পূর্ণাঙ্ক)globvarht.ht_used;
    প্রতি (hi = globvarht.ht_array; todo > 0 && !got_int; ++hi)
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

স্থির char_u	*varnamebuf = রিক্ত;
স্থির পূর্ণাঙ্ক	varnamebuflen = 0;

/*
 * Function to concatenate a prefix and a variable name.
 */
    char_u *
cat_prefix_varname(পূর্ণাঙ্ক prefix, char_u *name)
{
    পূর্ণাঙ্ক		len;

    len = (পূর্ণাঙ্ক)STRLEN(name) + 3;
    যদি (len > varnamebuflen)
    {
	vim_free(varnamebuf);
	len += 10;			// some additional space
	varnamebuf = alloc(len);
	যদি (varnamebuf == রিক্ত)
	{
	    varnamebuflen = 0;
	    প্রত্যাগম রিক্ত;
	}
	varnamebuflen = len;
    }
    *varnamebuf = prefix;
    varnamebuf[1] = ':';
    STRCPY(varnamebuf + 2, name);
    প্রত্যাগম varnamebuf;
}

/*
 * Function given to ExpandGeneric() to obtain the list of user defined
 * (global/buffer/window/built-in) variable names.
 */
    char_u *
get_user_var_name(expand_T *xp, পূর্ণাঙ্ক idx)
{
    স্থির long_u	gdone;
    স্থির long_u	bdone;
    স্থির long_u	wdone;
    স্থির long_u	tdone;
    স্থির পূর্ণাঙ্ক		vidx;
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
	যাবত্ (HASHITEM_EMPTY(hi))
	    ++hi;
	যদি (STRNCMP("g:", xp->xp_pattern, 2) == 0)
	    প্রত্যাগম cat_prefix_varname('g', hi->hi_চাবি);
	প্রত্যাগম hi->hi_চাবি;
    }

    // b: variables
    ht = &prevwin_curwin()->w_buffer->b_vars->dv_hashtab;
    যদি (bdone < ht->ht_used)
    {
	যদি (bdone++ == 0)
	    hi = ht->ht_array;
	অন্যথা
	    ++hi;
	যাবত্ (HASHITEM_EMPTY(hi))
	    ++hi;
	প্রত্যাগম cat_prefix_varname('b', hi->hi_চাবি);
    }

    // w: variables
    ht = &prevwin_curwin()->w_vars->dv_hashtab;
    যদি (wdone < ht->ht_used)
    {
	যদি (wdone++ == 0)
	    hi = ht->ht_array;
	অন্যথা
	    ++hi;
	যাবত্ (HASHITEM_EMPTY(hi))
	    ++hi;
	প্রত্যাগম cat_prefix_varname('w', hi->hi_চাবি);
    }

    // t: variables
    ht = &curtab->tp_vars->dv_hashtab;
    যদি (tdone < ht->ht_used)
    {
	যদি (tdone++ == 0)
	    hi = ht->ht_array;
	অন্যথা
	    ++hi;
	যাবত্ (HASHITEM_EMPTY(hi))
	    ++hi;
	প্রত্যাগম cat_prefix_varname('t', hi->hi_চাবি);
    }

    // v: variables
    যদি (vidx < VV_LEN)
	প্রত্যাগম cat_prefix_varname('v', (char_u *)vimvars[vidx++].vv_name);

    VIM_CLEAR(varnamebuf);
    varnamebuflen = 0;
    প্রত্যাগম রিক্ত;
}

    বর্ণ *
get_var_special_name(পূর্ণাঙ্ক nr)
{
    বিকল্প (nr)
    {
	পক্ষ VVAL_FALSE: প্রত্যাগম in_vim9script() ? "false" : "v:false";
	পক্ষ VVAL_TRUE:  প্রত্যাগম in_vim9script() ? "true" : "v:true";
	পক্ষ VVAL_NULL:  প্রত্যাগম in_vim9script() ? "null" : "v:null";
	পক্ষ VVAL_NONE:  প্রত্যাগম "v:none";
    }
    internal_error("get_var_special_name()");
    প্রত্যাগম "42";
}

/*
 * Returns the global variable dictionary
 */
    dict_T *
get_globvar_dict(শূন্য)
{
    প্রত্যাগম &globvardict;
}

/*
 * Returns the global variable hash table
 */
    hashtab_T *
get_globvar_ht(শূন্য)
{
    প্রত্যাগম &globvarht;
}

/*
 * Returns the v: variable dictionary
 */
    dict_T *
get_vimvar_dict(শূন্য)
{
    প্রত্যাগম &vimvardict;
}

/*
 * Returns the index of a v:variable.  Negative if not found.
 * Returns DI_ flags in "di_flags".
 */
    পূর্ণাঙ্ক
find_vim_var(char_u *name, পূর্ণাঙ্ক *di_flags)
{
    dictitem_T	    *di = find_var_in_ht(&vimvarht, 0, name, TRUE);
    সংরচনা vimvar   *vv;

    যদি (di == রিক্ত)
	প্রত্যাগম -1;
    *di_flags = di->di_flags;
    vv = (সংরচনা vimvar *)((বর্ণ *)di - offsetof(vimvar_T, vv_di));
    প্রত্যাগম (পূর্ণাঙ্ক)(vv - vimvars);
}


/*
 * Set type of v: variable to "type".
 */
    শূন্য
set_vim_var_type(পূর্ণাঙ্ক idx, vartype_T type)
{
    vimvars[idx].vv_tv_type = type;
}

/*
 * Set number v: variable to "val".
 * Note that this does not set the type, use set_vim_var_type() for that.
 */
    শূন্য
set_vim_var_nr(পূর্ণাঙ্ক idx, varnumber_T val)
{
    vimvars[idx].vv_nr = val;
}

    বর্ণ *
get_vim_var_name(পূর্ণাঙ্ক idx)
{
    প্রত্যাগম vimvars[idx].vv_name;
}

/*
 * Get typval_T v: variable value.
 */
    typval_T *
get_vim_var_tv(পূর্ণাঙ্ক idx)
{
    প্রত্যাগম &vimvars[idx].vv_tv;
}

    type_T *
get_vim_var_type(পূর্ণাঙ্ক idx, garray_T *type_list)
{
    যদি (vimvars[idx].vv_type != রিক্ত)
	প্রত্যাগম vimvars[idx].vv_type;
    প্রত্যাগম typval2type_vimvar(&vimvars[idx].vv_tv, type_list);
}

/*
 * Add what getinfo() reports for the v: variable "name", without the "v:", to
 * "d".  Returns FAIL when there is no such variable.
 */
    পূর্ণাঙ্ক
vim_var_info(char_u *name, dict_T *d)
{
    পূর্ণাঙ্ক		di_flags;
    পূর্ণাঙ্ক		idx = find_vim_var(name, &di_flags);
    garray_T	type_list;
    বর্ণ	*tofree;

    যদি (idx < 0)
	প্রত্যাগম FAIL;
    vim_snprintf((বর্ণ *)IObuff, IOSIZE, "v:%s", vimvars[idx].vv_name);
    dict_add_string(d, "name", IObuff);
    dict_add_bool(d, "available", TRUE);
    ga_init2(&type_list, পরিমাণ(type_T *), 10);
    বর্ণ *type = type_name(get_vim_var_type(idx, &type_list), &tofree);
    dict_add_string(d, "type", (char_u *)type);
    vim_free(tofree);
    clear_type_list(&type_list);
    dict_add_bool(d, "readonly", (vimvars[idx].vv_flags & VV_RO) != 0);
    dict_add_bool(d, "compat", (vimvars[idx].vv_flags & VV_COMPAT) != 0);
    প্রত্যাগম OK;
}

/*
 * Set v: variable to "tv".  Only accepts the same type.
 * Takes over the value of "tv".
 */
    পূর্ণাঙ্ক
set_vim_var_tv(পূর্ণাঙ্ক idx, typval_T *tv)
{
    যদি (vimvars[idx].vv_tv_type != tv->v_type)
    {
	emsg(_(e_type_mismatch_for_v_variable));
	clear_tv(tv);
	প্রত্যাগম FAIL;
    }
    // VV_RO is also checked when compiling, but let's check here as well.
    যদি (vimvars[idx].vv_flags & VV_RO)
    {
	semsg(_(e_cannot_change_readonly_variable_str), vimvars[idx].vv_name);
	প্রত্যাগম FAIL;
    }
    যদি (sandbox && (vimvars[idx].vv_flags & VV_RO_SBX))
    {
	semsg(_(e_cannot_set_variable_in_sandbox_str), vimvars[idx].vv_name);
	প্রত্যাগম FAIL;
    }
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_di.di_tv = *tv;
    প্রত্যাগম OK;
}

/*
 * Get number v: variable value.
 */
    varnumber_T
get_vim_var_nr(পূর্ণাঙ্ক idx)
{
    প্রত্যাগম vimvars[idx].vv_nr;
}

/*
 * Get string v: variable value.  Uses a static buffer, can only be used once.
 * If the String variable has never been set, return an empty string.
 * Never returns NULL;
 */
    char_u *
get_vim_var_str(পূর্ণাঙ্ক idx)
{
    প্রত্যাগম tv_get_string(&vimvars[idx].vv_tv);
}

/*
 * Get List v: variable value.  Caller must take care of reference count when
 * needed.
 */
    list_T *
get_vim_var_list(পূর্ণাঙ্ক idx)
{
    প্রত্যাগম vimvars[idx].vv_list;
}

/*
 * Get Dict v: variable value.  Caller must take care of reference count when
 * needed.
 */
    dict_T *
get_vim_var_dict(পূর্ণাঙ্ক idx)
{
    প্রত্যাগম vimvars[idx].vv_dict;
}

/*
 * Set v:char to character "c".
 */
    শূন্য
set_vim_var_char(পূর্ণাঙ্ক c)
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

    set_vim_var_string(VV_CHAR, buf, (পূর্ণাঙ্ক)buflen);
}

/*
 * Set v:count to "count" and v:count1 to "count1".
 * When "set_prevcount" is TRUE first set v:prevcount from v:count.
 */
    শূন্য
set_vcount(
    long	গুনতি,
    long	count1,
    পূর্ণাঙ্ক		set_prevcount)
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
    পূর্ণাঙ্ক		idx,
    char_u	*val,
    পূর্ণাঙ্ক		len)	    // length of "val" to use or -1 (whole string)
{
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_tv_type = VAR_STRING;
    যদি (val == রিক্ত)
	vimvars[idx].vv_str = রিক্ত;
    অন্যথা যদি (len == -1)
	vimvars[idx].vv_str = vim_strsave(val);
    অন্যথা
	vimvars[idx].vv_str = vim_strnsave(val, len);
}

    শূন্য
set_vim_var_string_direct(
    পূর্ণাঙ্ক		idx,
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
set_vim_var_list(পূর্ণাঙ্ক idx, list_T *val)
{
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_tv_type = VAR_LIST;
    vimvars[idx].vv_list = val;
    যদি (val != রিক্ত)
	++val->lv_refcount;
}

/*
 * Set Dictionary v: variable to "val".
 */
    শূন্য
set_vim_var_dict(পূর্ণাঙ্ক idx, dict_T *val)
{
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_tv_type = VAR_DICT;
    vimvars[idx].vv_dict = val;
    যদি (val == রিক্ত)
	প্রত্যাগম;

    ++val->dv_refcount;
    dict_set_বস্তুসমূহ_ro(val);
}

/*
 * Set the v:argv list.
 */
    শূন্য
set_argv_var(বর্ণ **argv, পূর্ণাঙ্ক argc)
{
    list_T	*l = list_alloc();
    পূর্ণাঙ্ক		i;

    যদি (l == রিক্ত)
	getout(1);
    l->lv_lock = VAR_FIXED;
    প্রতি (i = 0; i < argc; ++i)
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
    পূর্ণাঙ্ক regname = 0;

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
set_reg_var(পূর্ণাঙ্ক c)
{
    char_u  regname[2];

    যদি (c == 0 || c == ' ')
	regname[0] = '"';
    অন্যথা
	regname[0] = c;
    regname[1] = NUL;
    // Avoid free/alloc when the value is already right.
    যদি (vimvars[VV_REG].vv_str == রিক্ত || vimvars[VV_REG].vv_str[0] != c)
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
    যদি (oldval == রিক্ত)
	প্রত্যাগম vimvars[VV_EXCEPTION].vv_str;

    vimvars[VV_EXCEPTION].vv_str = oldval;
    প্রত্যাগম রিক্ত;
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
    যদি (oldval == রিক্ত)
	প্রত্যাগম vimvars[VV_THROWPOINT].vv_str;

    vimvars[VV_THROWPOINT].vv_str = oldval;
    প্রত্যাগম রিক্ত;
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
    যদি (eap == রিক্ত)
    {
	vim_free(oldval);
	vimvars[VV_CMDARG].vv_str = oldarg;
	প্রত্যাগম রিক্ত;
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
    যদি (newval == রিক্ত)
	প্রত্যাগম রিক্ত;

    যদি (eap->force_bin == FORCE_BIN)
	sprintf((বর্ণ *)newval, " ++bin");
    অন্যথা যদি (eap->force_bin == FORCE_NOBIN)
	sprintf((বর্ণ *)newval, " ++nobin");
    অন্যথা
	*newval = NUL;

    যদি (eap->read_edit)
	STRCAT(newval, " ++edit");

    যদি (eap->force_ff != 0)
	sprintf((বর্ণ *)newval + STRLEN(newval), " ++ff=%s",
						eap->force_ff == 'u' ? "unix"
						: eap->force_ff == 'd' ? "dos"
						: "mac");
    যদি (eap->force_enc != 0)
	sprintf((বর্ণ *)newval + STRLEN(newval), " ++enc=%s",
					       eap->cmd + eap->force_enc);
    যদি (eap->bad_char == BAD_KEEP)
	STRCPY(newval + STRLEN(newval), " ++bad=keep");
    অন্যথা যদি (eap->bad_char == BAD_DROP)
	STRCPY(newval + STRLEN(newval), " ++bad=drop");
    অন্যথা যদি (eap->bad_char != 0)
	sprintf((বর্ণ *)newval + STRLEN(newval), " ++bad=%c", eap->bad_char);
    vimvars[VV_CMDARG].vv_str = newval;
    প্রত্যাগম oldval;
}

/*
 * Get the value of internal variable "name".
 * If "flags" has EVAL_VAR_IMPORT may return a VAR_ANY with v_number set to the
 * imported script ID.
 * Return OK or FAIL.  If OK is returned "rettv" must be cleared.
 */
    পূর্ণাঙ্ক
eval_variable(
    char_u	*name,
    পূর্ণাঙ্ক		len,		// length of "name" or zero
    scid_T	sid,		// script ID for imported item or zero
    typval_T	*rettv,		// NULL when only checking existence
    dictitem_T	**dip,		// non-NULL when typval's dict item is needed
    পূর্ণাঙ্ক		flags)		// EVAL_VAR_ flags
{
    পূর্ণাঙ্ক		ret = OK;
    typval_T	*tv = রিক্ত;
    পূর্ণাঙ্ক		found = FALSE;
    hashtab_T	*ht = রিক্ত;
    পূর্ণাঙ্ক		cc = 0;
    type_T	*type = রিক্ত;

    যদি (len > 0)
    {
	// truncate the name, so that we can use strcmp()
	cc = name[len];
	name[len] = NUL;
    }

    // Check for local variable when debugging.
    যদি ((sid == 0) && (tv = lookup_debug_var(name)) == রিক্ত)
    {
	// Check for user-defined variables.
	dictitem_T	*v = find_var(name, &ht, flags & EVAL_VAR_NOAUTOLOAD);

	যদি (v != রিক্ত)
	{
	    tv = &v->di_tv;
	    যদি (dip != রিক্ত)
		*dip = v;
	}
	অন্যথা
	    ht = রিক্ত;
    }

    যদি (tv == রিক্ত && (in_vim9script() || STRNCMP(name, "s:", 2) == 0))
    {
	imported_T  *import = রিক্ত;
	char_u	    *p = STRNCMP(name, "s:", 2) == 0 ? name + 2 : name;

	যদি (sid == 0)
	    import = find_imported(p, 0, TRUE);

	// imported variable from another script
	যদি (import != রিক্ত || sid != 0)
	{
	    যদি ((flags & EVAL_VAR_IMPORT) == 0)
	    {
		যদি (SCRIPT_ID_VALID(sid))
		{
		    ht = &SCRIPT_VARS(sid);
		    যদি (ht != রিক্ত)
		    {
			dictitem_T *v = find_var_in_ht(ht, 0, name,
						  flags & EVAL_VAR_NOAUTOLOAD);

			যদি (v == রিক্ত)
			    v = find_var_autoload_prefix(name, sid, রিক্ত, রিক্ত);

			যদি (v != রিক্ত)
			{
			    tv = &v->di_tv;
			    যদি (dip != রিক্ত)
				*dip = v;
			}
			অন্যথা
			    ht = রিক্ত;
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
		যদি (rettv != রিক্ত)
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
	    পূর্ণাঙ্ক	    has_g_prefix = STRNCMP(name, "g:", 2) == 0;
	    ufunc_T *ufunc = find_func(name, FALSE);

	    যদি (ufunc != রিক্ত && cc == '<')
	    {
		// handle generic function
		char_u	*argp = name + len;
		name[len] = cc;
		ufunc = eval_generic_func(ufunc, name, &argp);
		name[len] = NUL;
		যদি (ufunc == রিক্ত)
		{
		    ret = FAIL;
		    গচ্ছ done;
		}
	    }

	    // In Vim9 script we can get a function reference by using the
	    // function name.  For a global non-autoload function "g:" is
	    // required.
	    যদি (ufunc != রিক্ত && (has_g_prefix
					    || !func_requires_g_prefix(ufunc)))
	    {
		found = TRUE;
		যদি (rettv != রিক্ত)
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
		    যদি (rettv->vval.v_string != রিক্ত)
			func_ref(ufunc->uf_name);
		}
	    }
	}
    }

    যদি (!found)
    {
	যদি (tv == রিক্ত)
	{
	    যদি (rettv != রিক্ত && (flags & EVAL_VAR_VERBOSE))
		semsg(_(e_undefined_variable_str), name);
	    ret = FAIL;
	}
	অন্যথা যদি (rettv != রিক্ত)
	{
	    svar_T  *sv = রিক্ত;
	    পূর্ণাঙ্ক	    was_assigned = FALSE;

	    যদি (ht != রিক্ত && ht == get_script_local_ht()
		    && tv != &SCRIPT_SV(current_sctx.sc_sid)->sv_var.di_tv)
	    {
		sv = find_typval_in_script(tv, 0, TRUE);
		যদি (sv != রিক্ত)
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
		যদি (sv == রিক্ত)
		{
		    ret = FAIL;
		    গচ্ছ done;
		}
		যদি ((sv->sv_flags & SVFLAG_EXPORTED) == 0)
		{
		    semsg(_(e_item_not_exported_in_script_str), name);
		    ret = FAIL;
		    গচ্ছ done;
		}
	    }

	    // If a list or tuple or dict variable wasn't initialized and has
	    // meaningful type, do it now.  Not for global variables, they are
	    // not declared.
	    যদি (ht != &globvarht)
	    {
		যদি (tv->v_type == VAR_DICT && tv->vval.v_dict == রিক্ত
					    && ((type != রিক্ত && !was_assigned)
							  || !in_vim9script()))
		{
		    tv->vval.v_dict = dict_alloc();
		    যদি (tv->vval.v_dict != রিক্ত)
		    {
			++tv->vval.v_dict->dv_refcount;
			tv->vval.v_dict->dv_type = alloc_type(type);
			যদি (sv != রিক্ত)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
		অন্যথা যদি (tv->v_type == VAR_LIST && tv->vval.v_list == রিক্ত
					    && ((type != রিক্ত && !was_assigned)
							  || !in_vim9script()))
		{
		    tv->vval.v_list = list_alloc();
		    যদি (tv->vval.v_list != রিক্ত)
		    {
			++tv->vval.v_list->lv_refcount;
			tv->vval.v_list->lv_type = alloc_type(type);
			যদি (sv != রিক্ত)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
		অন্যথা যদি (tv->v_type == VAR_TUPLE && tv->vval.v_tuple == রিক্ত
					    && ((type != রিক্ত && !was_assigned)
							  || !in_vim9script()))
		{
		    tv->vval.v_tuple = tuple_alloc();
		    যদি (tv->vval.v_tuple != রিক্ত)
		    {
			++tv->vval.v_tuple->tv_refcount;
			tv->vval.v_tuple->tv_type = alloc_type(type);
			যদি (sv != রিক্ত)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
		অন্যথা যদি (tv->v_type == VAR_BLOB && tv->vval.v_blob == রিক্ত
					    && ((type != রিক্ত && !was_assigned)
							  || !in_vim9script()))
		{
		    tv->vval.v_blob = blob_alloc();
		    যদি (tv->vval.v_blob != রিক্ত)
		    {
			++tv->vval.v_blob->bv_refcount;
			যদি (sv != রিক্ত)
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

    প্রত্যাগম ret;
}

/*
 * Get the value of internal variable "name", also handling "import.name".
 * Return OK or FAIL.  If OK is returned "rettv" must be cleared.
 */
    পূর্ণাঙ্ক
eval_variable_import(
    char_u	*name,
    typval_T	*rettv)
{
    char_u  *s = name;
    যাবত্ (ASCII_ISALNUM(*s) || *s == '_')
	++s;
    পূর্ণাঙ্ক	    len = (পূর্ণাঙ্ক)(s - name);

    যদি (eval_variable(name, len, 0, rettv, রিক্ত, EVAL_VAR_IMPORT) == FAIL)
	প্রত্যাগম FAIL;
    যদি (rettv->v_type == VAR_ANY && *s == '.')
    {
	char_u *ns = s + 1;
	s = ns;
	যাবত্ (ASCII_ISALNUM(*s) || *s == '_')
	    ++s;
	পূর্ণাঙ্ক sid = rettv->vval.v_সংখ্যা;
	প্রত্যাগম eval_variable(ns, (পূর্ণাঙ্ক)(s - ns), sid, rettv, রিক্ত, 0);
    }
    প্রত্যাগম OK;
}


/*
 * Check if variable "name[len]" is a local variable or an argument.
 * If so, "*eval_lavars_used" is set to TRUE.
 */
    শূন্য
check_vars(char_u *name, পূর্ণাঙ্ক len)
{
    পূর্ণাঙ্ক		cc;
    char_u	*varname;
    hashtab_T	*ht;

    যদি (eval_lavars_used == রিক্ত)
	প্রত্যাগম;

    // truncate the name, so that we can use strcmp()
    cc = name[len];
    name[len] = NUL;

    ht = find_var_ht(name, &varname);
    যদি (ht == get_funccal_local_ht() || ht == get_funccal_args_ht())
    {
	যদি (find_var(name, রিক্ত, TRUE) != রিক্ত)
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
find_var(char_u *name, hashtab_T **htp, পূর্ণাঙ্ক no_autoload)
{
    char_u	*varname;
    hashtab_T	*ht;
    dictitem_T	*ret = রিক্ত;

    ht = find_var_ht(name, &varname);
    যদি (htp != রিক্ত)
	*htp = ht;
    যদি (ht == রিক্ত)
	প্রত্যাগম রিক্ত;
    ret = find_var_in_ht(ht, *name, varname, no_autoload);
    যদি (ret != রিক্ত)
	প্রত্যাগম ret;

    // Search in parent scope for lambda
    ret = find_var_in_scoped_ht(name, no_autoload);
    যদি (ret != রিক্ত)
	প্রত্যাগম ret;

    // in Vim9 script items without a scope can be script-local
    যদি (in_vim9script() && name[0] != NUL && name[1] != ':')
    {
	ht = get_script_local_ht();
	যদি (ht != রিক্ত)
	{
	    ret = find_var_in_ht(ht, *name, varname, no_autoload);
	    যদি (ret != রিক্ত)
	    {
		যদি (htp != রিক্ত)
		    *htp = ht;
		প্রত্যাগম ret;
	    }
	}
    }

    // and finally try
    প্রত্যাগম find_var_autoload_prefix(name, 0, htp, রিক্ত);
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
find_var_autoload_prefix(char_u *name, পূর্ণাঙ্ক sid, hashtab_T **htp,
							    char_u **namep)
{
    hashtab_T	*ht;
    dictitem_T	*ret = রিক্ত;
    // When using "vim9script autoload" script-local items are prefixed but can
    // be used with s:name.
    পূর্ণাঙ্ক check_sid = sid > 0 ? sid : current_sctx.sc_sid;
    যদি (SCRIPT_ID_VALID(check_sid)
		   && (in_vim9script() || (name[0] == 's' && name[1] == ':')))
    {
	scriptitem_T *si = SCRIPT_ITEM(check_sid);

	যদি (si->sn_autoload_prefix != রিক্ত)
	{
	    char_u *base_name = (name[0] == 's' && name[1] == ':')
							     ? name + 2 : name;
	    char_u *auto_name = concat_str(si->sn_autoload_prefix, base_name);

	    যদি (auto_name != রিক্ত)
	    {
		পূর্ণাঙ্ক free_auto_name = TRUE;
		ht = &globvarht;
		ret = find_var_in_ht(ht, 'g', auto_name, TRUE);
		যদি (ret != রিক্ত)
		{
		    যদি (htp != রিক্ত)
			*htp = ht;
		    যদি (namep != রিক্ত)
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

    প্রত্যাগম ret;
}

/*
 * Like find_var() but if the name starts with <SNR>99_ then look in the
 * referenced script (used for a funcref).
 */
    dictitem_T *
find_var_also_in_script(char_u *name, hashtab_T **htp, পূর্ণাঙ্ক no_autoload)
{
    যদি (STRNCMP(name, "<SNR>", 5) == 0 && SAFE_isdigit(name[5]))
    {
	char_u	    *p = name + 5;
	পূর্ণাঙ্ক	    sid = getdigits(&p);

	যদি (SCRIPT_ID_VALID(sid) && *p == '_')
	{
	    hashtab_T	*ht = &SCRIPT_VARS(sid);

	    যদি (ht != রিক্ত)
	    {
		dictitem_T *di = find_var_in_ht(ht, 0, p + 1, no_autoload);

		যদি (di != রিক্ত)
		{
		    যদি (htp != রিক্ত)
			*htp = ht;
		    প্রত্যাগম di;
		}
	    }
	}
    }

    প্রত্যাগম find_var(name, htp, no_autoload);
}

/*
 * Find variable "varname" in hashtab "ht" with name "htname".
 * When "varname" is empty returns curwin/curtab/etc vars dictionary.
 * Returns NULL if not found.
 */
    dictitem_T *
find_var_in_ht(
    hashtab_T	*ht,
    পূর্ণাঙ্ক		htname,
    char_u	*varname,
    পূর্ণাঙ্ক		no_autoload)
{
    hashitem_T	*hi;

    যদি (*varname == NUL)
    {
	// Must be something like "s:", otherwise "ht" would be NULL.
	বিকল্প (htname)
	{
	    পক্ষ 's': প্রত্যাগম &SCRIPT_SV(current_sctx.sc_sid)->sv_var;
	    পক্ষ 'g': প্রত্যাগম &globvars_var;
	    পক্ষ 'v': প্রত্যাগম &vimvars_var;
	    পক্ষ 'b': প্রত্যাগম &curbuf->b_bufvar;
	    পক্ষ 'w': প্রত্যাগম &curwin->w_winvar;
	    পক্ষ 't': প্রত্যাগম &curtab->tp_winvar;
	    পক্ষ 'l': প্রত্যাগম get_funccal_local_var();
	    পক্ষ 'a': প্রত্যাগম get_funccal_args_var();
	}
	প্রত্যাগম রিক্ত;
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
		প্রত্যাগম রিক্ত;
	    hi = hash_find(ht, varname);
	}
	যদি (HASHITEM_EMPTY(hi))
	    প্রত্যাগম রিক্ত;
    }
    প্রত্যাগম HI2DI(hi);
}

/*
 * Get the script-local hashtab.  NULL if not in a script context.
 */
    hashtab_T *
get_script_local_ht(শূন্য)
{
    scid_T sid = current_sctx.sc_sid;

    যদি (SCRIPT_ID_VALID(sid))
	প্রত্যাগম &SCRIPT_VARS(sid);
    প্রত্যাগম রিক্ত;
}

/*
 * Look for "name[len]" in script-local variables and functions.
 * When "cmd" is TRUE it must look like a command, a function must be followed
 * by "(" or "->".
 * Return OK when found, FAIL when not found.
 */
    পূর্ণাঙ্ক
lookup_scriptitem(
	char_u	*name,
	size_t	len,
	পূর্ণাঙ্ক	cmd,
	cctx_T	*dummy UNUSED)
{
    hashtab_T	*ht = get_script_local_ht();
    char_u	buffer[30];
    char_u	*p;
    পূর্ণাঙ্ক		res;
    hashitem_T	*hi;
    পূর্ণাঙ্ক		is_global = FALSE;
    char_u	*fname = name;

    যদি (ht == রিক্ত)
	প্রত্যাগম FAIL;
    যদি (len < পরিমাণ(buffer) - 1)
    {
	// avoid an alloc/free for short names
	vim_strncpy(buffer, name, len);
	p = buffer;
    }
    অন্যথা
    {
	p = vim_strnsave(name, len);
	যদি (p == রিক্ত)
	    প্রত্যাগম FAIL;
    }

    hi = hash_find(ht, p);
    res = HASHITEM_EMPTY(hi) ? FAIL : OK;

    // if not script-local, then perhaps autoload-exported
    যদি (res == FAIL && find_var_autoload_prefix(p, 0, রিক্ত, রিক্ত) != রিক্ত)
	res = OK;

    // if not script-local or autoload, then perhaps imported
    যদি (res == FAIL && find_imported(p, 0, FALSE) != রিক্ত)
	res = OK;
    যদি (p != buffer)
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
	    যদি (find_func(fname, is_global) != রিক্ত)
		res = OK;
	}
    }

    প্রত্যাগম res;
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
	প্রত্যাগম রিক্ত;
    যদি (name[1] != ':')
    {
	// The name must not start with a colon or #.
	যদি (name[0] == ':' || name[0] == AUTOLOAD_CHAR)
	    প্রত্যাগম রিক্ত;
	*varname = name;

	// "version" is "v:version" in all scopes if scriptversion < 3.
	// Same for a few other variables marked with VV_COMPAT.
	যদি (in_old_script(3))
	{
	    hi = hash_find(&compat_hashtab, name);
	    যদি (!HASHITEM_EMPTY(hi))
		প্রত্যাগম &compat_hashtab;
	}

	ht = get_funccal_local_ht();
	যদি (ht != রিক্ত)
	    প্রত্যাগম ht;				// local variable

	// In Vim9 script items at the script level are script-local, except
	// for autoload names.
	যদি (in_vim9script() && vim_strchr(name, AUTOLOAD_CHAR) == রিক্ত)
	{
	    ht = get_script_local_ht();
	    যদি (ht != রিক্ত)
		প্রত্যাগম ht;
	}

	প্রত্যাগম &globvarht;			// global variable
    }
    *varname = name + 2;
    যদি (*name == 'g')				// global variable
	প্রত্যাগম &globvarht;
    // There must be no ':' or '#' in the rest of the name, unless g: is used
    যদি (vim_strchr(name + 2, ':') != রিক্ত
			       || vim_strchr(name + 2, AUTOLOAD_CHAR) != রিক্ত)
	প্রত্যাগম রিক্ত;
    যদি (*name == 'b')				// buffer variable
	প্রত্যাগম &curbuf->b_vars->dv_hashtab;
    যদি (*name == 'w')				// window variable
	প্রত্যাগম &curwin->w_vars->dv_hashtab;
    যদি (*name == 't')				// tab page variable
	প্রত্যাগম &curtab->tp_vars->dv_hashtab;
    যদি (*name == 'v')				// v: variable
	প্রত্যাগম &vimvarht;
    যদি (get_current_funccal() != রিক্ত
	       && get_current_funccal()->fc_func->uf_def_status
							    == UF_NOT_COMPILED)
    {
	// a: and l: are only used in functions defined with ":function"
	যদি (*name == 'a')			// a: function argument
	    প্রত্যাগম get_funccal_args_ht();
	যদি (*name == 'l')			// l: local function variable
	    প্রত্যাগম get_funccal_local_ht();
    }
    যদি (*name == 's')				// script variable
    {
	ht = get_script_local_ht();
	যদি (ht != রিক্ত)
	    প্রত্যাগম ht;
    }
    প্রত্যাগম রিক্ত;
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

    v = find_var(name, রিক্ত, FALSE);
    যদি (v == রিক্ত)
	প্রত্যাগম রিক্ত;
    প্রত্যাগম tv_get_string(&v->di_tv);
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
    যদি (sv == রিক্ত)
	প্রত্যাগম;
    init_var_dict(&sv->sv_dict, &sv->sv_var, VAR_SCOPE);
    SCRIPT_ITEM(id)->sn_vars = sv;
}

/*
 * Initialize dictionary "dict" as a scope and set variable "dict_var" to
 * point to it.
 */
    শূন্য
init_var_dict(dict_T *dict, dictitem_T *dict_var, পূর্ণাঙ্ক scope)
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
vars_clear_ext(hashtab_T *ht, পূর্ণাঙ্ক free_val)
{
    পূর্ণাঙ্ক		todo;
    hashitem_T	*hi;
    dictitem_T	*v;

    hash_lock(ht);
    todo = (পূর্ণাঙ্ক)ht->ht_used;
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
	প্রত্যাগম;

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
    যদি (prefix == রিক্ত)
	প্রত্যাগম;
    size_t  prefixlen = STRLEN(prefix);

    hash_lock(&globvarht);
    পূর্ণাঙ্ক	todo = (পূর্ণাঙ্ক)globvarht.ht_used;

    প্রতি (hashitem_T *hi = globvarht.ht_array; todo > 0; ++hi)
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
list_one_var(dictitem_T *v, বর্ণ *prefix, পূর্ণাঙ্ক *first)
{
    char_u	*tofree;
    char_u	*s;
    char_u	numbuf[NUMBUFLEN];

    s = echo_string(&v->di_tv, &tofree, numbuf, get_copyID());
    list_one_var_a(prefix, v->di_চাবি, v->di_tv.v_type,
					 s == রিক্ত ? (char_u *)"" : s, first);
    vim_free(tofree);
}

    স্থির শূন্য
list_one_var_a(
    বর্ণ	*prefix,
    char_u	*name,
    পূর্ণাঙ্ক		type,
    char_u	*string,
    পূর্ণাঙ্ক		*first)  // when TRUE clear rest of screen and set to FALSE
{
    // don't use msg() or msg_attr() to avoid overwriting "v:statusmsg"
    বার্তা_start();
    বার্তা_puts(prefix);
    যদি (name != রিক্ত)	// "a:" vars don't have a name stored
	বার্তা_puts((বর্ণ *)name);
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
    পূর্ণাঙ্ক
before_set_vvar(
    char_u	*varname,
    dictitem_T	*di,
    typval_T	*tv,
    পূর্ণাঙ্ক		copy,
    পূর্ণাঙ্ক		*type_error)
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
	    যদি (di->di_tv.vval.v_string == রিক্ত)
		di->di_tv.vval.v_string = vim_strsave(val);
	}
	অন্যথা
	{
	    // Take over the string to avoid an extra alloc/free.
	    di->di_tv.vval.v_string = tv->vval.v_string;
	    tv->vval.v_string = রিক্ত;
	}
	প্রত্যাগম FALSE;
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
	প্রত্যাগম FALSE;
    }
    অন্যথা যদি (di->di_tv.v_type != tv->v_type)
    {
	*type_error = TRUE;
	প্রত্যাগম FALSE;
    }
    প্রত্যাগম TRUE;
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
    পূর্ণাঙ্ক		copy)	    // make copy of value in "tv"
{
    set_var_const(name, 0, রিক্ত, tv, copy, ASSIGN_DECL, 0);
}

/*
 * Set variable "name" to value in "tv_arg".
 * When "sid" is non-zero "name" is in the script with this ID.
 * If the variable already exists and "is_const" is FALSE the value is updated.
 * Otherwise the variable is created.
 */
    পূর্ণাঙ্ক
set_var_const(
    char_u	*name,
    scid_T	sid,
    type_T	*type_arg,
    typval_T	*tv_arg,
    পূর্ণাঙ্ক		copy,	    // make copy of value in "tv"
    পূর্ণাঙ্ক		flags_arg,  // ASSIGN_CONST, ASSIGN_FINAL, etc.
    পূর্ণাঙ্ক		var_idx)    // index for ":let [a, b] = list"
{
    typval_T	*tv = tv_arg;
    type_T	*type = type_arg;
    typval_T	bool_tv;
    dictitem_T	*di;
    typval_T	*dest_tv;
    char_u	*varname;
    char_u	*name_tofree = রিক্ত;
    hashtab_T	*ht = রিক্ত;
    পূর্ণাঙ্ক		is_script_local;
    পূর্ণাঙ্ক		vim9script = in_vim9script();
    পূর্ণাঙ্ক		var_in_vim9script;
    পূর্ণাঙ্ক		var_in_autoload = FALSE;
    পূর্ণাঙ্ক		flags = flags_arg;
    পূর্ণাঙ্ক		free_tv_arg = !copy;  // free tv_arg if not used
    পূর্ণাঙ্ক		rc = FAIL;

    যদি (sid != 0)
    {
	varname = রিক্ত;
	যদি (SCRIPT_ID_VALID(sid))
	{
	    char_u	*auto_name = রিক্ত;
	    যদি (find_var_autoload_prefix(name, sid, &ht, &auto_name) != রিক্ত)
	    {
		var_in_autoload = TRUE;
		varname = auto_name;
		name_tofree = varname;
	    }
	    অন্যথা
		ht = &SCRIPT_VARS(sid);
	}
	যদি (varname == রিক্ত)
	    varname = name;
    }
    অন্যথা
    {
	scriptitem_T	*si;
	char_u		*auto_name = রিক্ত;

	যদি (in_vim9script()
	    && SCRIPT_ID_VALID(current_sctx.sc_sid)
	    && (si = SCRIPT_ITEM(current_sctx.sc_sid))
					      ->sn_autoload_prefix != রিক্ত
	    && (is_export
		|| find_var_autoload_prefix(name, 0, রিক্ত, &auto_name)
								    != রিক্ত))
	{
	    // In a vim9 autoload script an exported variable is put in the
	    // global namespace with the autoload prefix.
	    var_in_autoload = TRUE;
	    varname = auto_name != রিক্ত ? auto_name
		      : concat_str(si->sn_autoload_prefix, name);
	    যদি (varname == রিক্ত)
		গচ্ছ failed;
	    name_tofree = varname;
	    ht = &globvarht;
	}
	অন্যথা
	    ht = find_var_ht(name, &varname);
    }
    যদি (ht == রিক্ত || *varname == NUL)
    {
	semsg(_(e_illegal_variable_name_str), name);
	গচ্ছ failed;
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
	গচ্ছ failed;
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
	গচ্ছ failed;
    }

    di = find_var_in_ht(ht, 0, varname, TRUE);

    যদি (di == রিক্ত && var_in_vim9script)
    {
	imported_T  *import = find_imported(varname, 0, FALSE);

	যদি (import != রিক্ত)
	{
	    // imported name space cannot be used
	    যদি ((flags & ASSIGN_NO_DECL) == 0)
	    {
		semsg(_(e_redefining_imported_item_str), name);
		গচ্ছ failed;
	    }
	    semsg(_(e_cannot_use_str_itself_it_is_imported), name);
	    গচ্ছ failed;
	}
	যদি (!in_vim9script())
	{
	    semsg(_(e_cannot_create_vim9_script_variable_in_function_str),
									 name);
	    গচ্ছ failed;
	}
    }

    // Search in parent scope which is possible to reference from lambda
    যদি (di == রিক্ত)
	di = find_var_in_scoped_ht(name, TRUE);

    যদি ((tv->v_type == VAR_FUNC || tv->v_type == VAR_PARTIAL)
				  && var_wrong_func_name(name, di == রিক্ত))
	গচ্ছ failed;

    যদি (need_convert_to_bool(type, tv))
    {
	// Destination is a bool and the value is not, but it can be
	// converted.
	CLEAR_FIELD(bool_tv);
	bool_tv.v_type = VAR_BOOL;
	bool_tv.vval.v_সংখ্যা = tv2bool(tv) ? VVAL_TRUE : VVAL_FALSE;
	tv = &bool_tv;
    }

    যদি (di != রিক্ত)
    {
	// Item already exists.  Allowed to replace when reloading.
	যদি ((di->di_flags & DI_FLAGS_RELOAD) == 0)
	{
	    যদি ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					 && (flags & ASSIGN_FOR_LOOP) == 0)
	    {
		emsg(_(e_cannot_modify_existing_variable));
		গচ্ছ failed;
	    }

	    যদি (is_script_local && vim9script
			  && (flags & (ASSIGN_NO_DECL | ASSIGN_DECL)) == 0)
	    {
		semsg(_(e_redefining_script_item_str), name);
		গচ্ছ failed;
	    }

	    যদি (check_typval_is_মান(&di->di_tv) == FAIL)
		গচ্ছ failed;

	    // List and Blob types can be modified in-place using the "+="
	    // compound operator.  For other types, this is not allowed.
	    পূর্ণাঙ্ক type_inplace_modifiable =
		(di->di_tv.v_type == VAR_LIST || di->di_tv.v_type == VAR_BLOB);

	    যদি (var_in_vim9script && (flags & ASSIGN_FOR_LOOP) == 0
		    && ((flags & ASSIGN_COMPOUND_OP) == 0
			|| !type_inplace_modifiable))
	    {
		where_T where = WHERE_INIT;
		svar_T  *sv = find_typval_in_script(&di->di_tv, sid, TRUE);

		যদি (sv != রিক্ত)
		{
		    // check the type and adjust to bool if needed
		    যদি (var_idx > 0)
		    {
			where.wt_index = var_idx;
			where.wt_kind = WT_VARIABLE;
		    }
		    যদি (check_script_var_type(sv, tv, name, where) == FAIL)
			গচ্ছ failed;
		    যদি (type == রিক্ত)
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
		গচ্ছ failed;
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
		গচ্ছ failed;
	}

	// Handle setting internal v: variables separately where needed to
	// prevent changing the type.
	পূর্ণাঙ্ক type_error = FALSE;
	যদি (ht == &vimvarht
		&& !before_set_vvar(varname, di, tv, copy, &type_error))
	{
	    যদি (type_error)
		semsg(_(e_setting_v_str_to_মান_with_wrong_type), varname);
	    গচ্ছ failed;
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
	       && lookup_scriptitem(name, STRLEN(name), FALSE, রিক্ত) == OK)
	{
	    semsg(_(e_redefining_script_item_str), name);
	    গচ্ছ failed;
	}

	// add a new variable
	যদি (var_in_vim9script && (flags & ASSIGN_NO_DECL))
	{
	    semsg(_(e_unknown_variable_str), name);
	    গচ্ছ failed;
	}

	যদি (check_hashtab_frozen(ht, "add variable"))
	    গচ্ছ failed;

	// Can't add "v:" or "a:" variable.
	যদি (ht == &vimvarht || ht == get_funccal_args_ht())
	{
	    semsg(_(e_illegal_variable_name_str), name);
	    গচ্ছ failed;
	}

	// Make sure the variable name is valid.  In Vim9 script an
	// autoload variable must be prefixed with "g:" unless in an
	// autoload script.
	যদি (!valid_varname(varname, -1, !vim9script
			|| STRNCMP(name, "g:", 2) == 0 || var_in_autoload))
	    গচ্ছ failed;

	di = alloc(offsetof(dictitem_T, di_চাবি) + STRLEN(varname) + 1);
	যদি (di == রিক্ত)
	    গচ্ছ failed;
	STRCPY(di->di_চাবি, varname);
	যদি (hash_add(ht, DI2HIKEY(di), "add variable") == FAIL)
	{
	    vim_free(di);
	    গচ্ছ failed;
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

    যদি (vim9script && type != রিক্ত)
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

    প্রত্যাগম rc;
}

/*
 * Check in this order for backwards compatibility:
 * - Whether the variable is read-only
 * - Whether the variable value is locked
 * - Whether the variable is locked
 * NOTE: "name" is only used for error messages.
 */
    পূর্ণাঙ্ক
var_check_permission(dictitem_T *di, char_u *name)
{
    যদি (var_check_ro(di->di_flags, name, FALSE)
		    || মান_check_lock(di->di_tv.v_lock, name, FALSE)
		    || var_check_lock(di->di_flags, name, FALSE))
	প্রত্যাগম FAIL;
    প্রত্যাগম OK;
}

/*
 * Return TRUE if di_flags "flags" indicates variable "name" is read-only.
 * Also give an error message.
 */
    পূর্ণাঙ্ক
var_check_ro(পূর্ণাঙ্ক flags, char_u *name, পূর্ণাঙ্ক use_gettext)
{
    যদি (flags & DI_FLAGS_RO)
    {
	যদি (name == রিক্ত)
	    emsg(_(e_cannot_change_readonly_variable));
	অন্যথা
	    semsg(_(e_cannot_change_readonly_variable_str),
				       use_gettext ? (char_u *)_(name) : name);
	প্রত্যাগম TRUE;
    }
    যদি ((flags & DI_FLAGS_RO_SBX) && sandbox)
    {
	যদি (name == রিক্ত)
	    emsg(_(e_cannot_set_variable_in_sandbox));
	অন্যথা
	    semsg(_(e_cannot_set_variable_in_sandbox_str),
				       use_gettext ? (char_u *)_(name) : name);
	প্রত্যাগম TRUE;
    }
    প্রত্যাগম FALSE;
}

/*
 * Return TRUE if di_flags "flags" indicates variable "name" is locked.
 * Also give an error message.
 */
    পূর্ণাঙ্ক
var_check_lock(পূর্ণাঙ্ক flags, char_u *name, পূর্ণাঙ্ক use_gettext)
{
    যদি (flags & DI_FLAGS_LOCK)
    {
	semsg(_(e_variable_is_locked_str),
				       use_gettext ? (char_u *)_(name) : name);
	প্রত্যাগম TRUE;
    }
    প্রত্যাগম FALSE;
}

/*
 * Return TRUE if di_flags "flags" indicates variable "name" is fixed.
 * Also give an error message.
 */
    পূর্ণাঙ্ক
var_check_fixed(পূর্ণাঙ্ক flags, char_u *name, পূর্ণাঙ্ক use_gettext)
{
    যদি (flags & DI_FLAGS_FIX)
    {
	যদি (name == রিক্ত)
	    emsg(_(e_cannot_delete_variable));
	অন্যথা
	    semsg(_(e_cannot_delete_variable_str),
				      use_gettext ? (char_u *)_(name) : name);
	প্রত্যাগম TRUE;
    }
    প্রত্যাগম FALSE;
}

/*
 * Check if a funcref is assigned to a valid variable name.
 * Return TRUE and give an error if not.
 */
    পূর্ণাঙ্ক
var_wrong_func_name(
    char_u *name,    // points to start of variable name
    পূর্ণাঙ্ক    new_var)  // TRUE when creating the variable
{
    // Allow for w: b: s: and t:.  In Vim9 script s: is not allowed, because
    // the name can be used without the s: prefix.
    // Allow autoload variable.
    যদি (!((vim_strchr((char_u *)"wbt", name[0]) != রিক্ত
		    || (!in_vim9script() && name[0] == 's')) && name[1] == ':')
	    && !ASCII_ISUPPER((name[0] != NUL && name[1] == ':')
						     ? name[2] : name[0])
	    && vim_strchr(name, '#') == রিক্ত)
    {
	semsg(_(e_funcref_variable_name_must_start_with_capital_str), name);
	প্রত্যাগম TRUE;
    }
    // Don't allow hiding a function.  When "v" is not NULL we might be
    // assigning another function to the same var, the type is checked
    // below.
    যদি (new_var && function_exists(name, FALSE))
    {
	semsg(_(e_variable_name_conflicts_with_existing_function_str),
								    name);
	প্রত্যাগম TRUE;
    }
    প্রত্যাগম FALSE;
}

/*
 * Return TRUE if "flags" indicates variable "name" has a locked (immutable)
 * value.  Also give an error message, using "name" or _("name") when
 * "use_gettext" is TRUE.
 */
    পূর্ণাঙ্ক
মান_check_lock(পূর্ণাঙ্ক lock, char_u *name, পূর্ণাঙ্ক use_gettext)
{
    যদি (lock & VAR_LOCKED)
    {
	যদি (name == রিক্ত)
	    emsg(_(e_মান_is_locked));
	অন্যথা
	    semsg(_(e_মান_is_locked_str),
				       use_gettext ? (char_u *)_(name) : name);
	প্রত্যাগম TRUE;
    }
    যদি (lock & VAR_FIXED)
    {
	যদি (name == রিক্ত)
	    emsg(_(e_cannot_change_মান));
	অন্যথা
	    semsg(_(e_cannot_change_মান_of_str),
				       use_gettext ? (char_u *)_(name) : name);
	প্রত্যাগম TRUE;
    }
    প্রত্যাগম FALSE;
}

/*
 * Check if a variable name is valid.  When "autoload" is true "#" is allowed.
 * If "len" is -1 use all of "varname", otherwise up to "varname[len]".
 * Return FALSE and give an error if not.
 */
    পূর্ণাঙ্ক
valid_varname(char_u *varname, পূর্ণাঙ্ক len, পূর্ণাঙ্ক autoload)
{
    char_u *p;

    প্রতি (p = varname; len < 0 ? *p != NUL : p < varname + len; ++p)
	যদি (!eval_isnamec1(*p) && (p == varname || !VIM_ISDIGIT(*p))
					 && !(autoload && *p == AUTOLOAD_CHAR))
	{
	    semsg(_(e_illegal_variable_name_str), varname);
	    প্রত্যাগম FALSE;
	}
    প্রত্যাগম TRUE;
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
    পূর্ণাঙ্ক		htname,	    // 't'ab, 'w'indow or 'b'uffer local.
    tabpage_T	*tp,	    // can be NULL
    win_T	*win,
    buf_T	*buf)	    // Ignored if htname is not 'b'.
{
    dictitem_T	*v;
    পূর্ণাঙ্ক		done = FALSE;
    switchwin_T	switchwin;
    পূর্ণাঙ্ক		need_switch_win;
    পূর্ণাঙ্ক		do_change_curbuf = buf != রিক্ত && htname == 'b';

    ++emsg_off;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = রিক্ত;

    যদি (varname != রিক্ত && tp != রিক্ত && win != রিক্ত
	    && (htname != 'b' || buf != রিক্ত))
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

		    যদি (opts != রিক্ত)
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
		যদি (v != রিক্ত)
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
    পূর্ণাঙ্ক		off)	    // 1 for gettabwinvar()
{
    char_u	*varname;
    tabpage_T	*tp;
    win_T	*win;

    যদি (off == 1)
	tp = find_tabpage((পূর্ণাঙ্ক)tv_get_সংখ্যা_chk(&argvars[0], রিক্ত));
    অন্যথা
	tp = curtab;
    win = find_win_by_nr(&argvars[off], tp);
    varname = tv_get_string_chk(&argvars[off + 1]);

    get_var_from(varname, rettv, &argvars[off + 2], 'w', tp, win, রিক্ত);
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
    পূর্ণাঙ্ক		error = FALSE;

    পূর্ণাঙ্ক		opt_idx = findoption(varname);
    যদি (opt_idx < 0)
    {
	semsg(_(e_unknown_option_str_2), varname);
	প্রত্যাগম;
    }
    পূর্ণাঙ্ক		opt_p_flags = get_option_flags(opt_idx);

    যদি (varp->v_type == VAR_BOOL)
    {
	যদি (opt_p_flags & P_STRING)
	{
	    emsg(_(e_string_required));
	    প্রত্যাগম;
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
    যদি (!error && strval != রিক্ত)
	set_option_মান_give_err(varname, numval, strval, OPT_LOCAL);
}

/*
 * "setwinvar()" and "settabwinvar()" functions
 */
    স্থির শূন্য
setwinvar(typval_T *argvars, পূর্ণাঙ্ক off)
{
    win_T	*win;
    switchwin_T	switchwin;
    পূর্ণাঙ্ক		need_switch_win;
    char_u	*varname, *winvarname;
    typval_T	*varp;
    tabpage_T	*tp = রিক্ত;

    যদি (check_secure())
	প্রত্যাগম;

    যদি (off == 1)
	tp = find_tabpage((পূর্ণাঙ্ক)tv_get_সংখ্যা_chk(&argvars[0], রিক্ত));
    অন্যথা
	tp = curtab;
    win = find_win_by_nr(&argvars[off], tp);
    varname = tv_get_string_chk(&argvars[off + 1]);
    varp = &argvars[off + 2];

    যদি (win == রিক্ত || varname == রিক্ত)
	প্রত্যাগম;

    need_switch_win = !(tp == curtab && win == curwin);
    যদি (!need_switch_win
	    || switch_win(&switchwin, win, tp, TRUE) == OK)
    {
	যদি (*varname == '&')
	    set_option_from_tv(varname + 1, varp);
	অন্যথা
	{
	    winvarname = alloc(STRLEN(varname) + 3);
	    যদি (winvarname != রিক্ত)
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
    set_vim_var_string(VV_OPTION_NEW, রিক্ত, -1);
    set_vim_var_string(VV_OPTION_OLD, রিক্ত, -1);
    set_vim_var_string(VV_OPTION_OLDLOCAL, রিক্ত, -1);
    set_vim_var_string(VV_OPTION_OLDGLOBAL, রিক্ত, -1);
    set_vim_var_string(VV_OPTION_TYPE, রিক্ত, -1);
    set_vim_var_string(VV_OPTION_COMMAND, রিক্ত, -1);
}

/*
 * Add an assert error to v:errors.
 */
    শূন্য
assert_error(garray_T *gap)
{
    সংরচনা vimvar   *vp = &vimvars[VV_ERRORS];

    যদি (vp->vv_tv_type != VAR_LIST || vimvars[VV_ERRORS].vv_list == রিক্ত)
	// Make sure v:errors is a list.
	set_vim_var_list(VV_ERRORS, list_alloc());
    list_append_string(vimvars[VV_ERRORS].vv_list, gap->ga_data, gap->ga_len);
}

    পূর্ণাঙ্ক
var_exists(char_u *var)
{
    char_u	*arg = var;
    char_u	*name;
    char_u	*tofree;
    typval_T    tv;
    পূর্ণাঙ্ক		len = 0;
    পূর্ণাঙ্ক		n = FALSE;

    // get_name_len() takes care of expanding curly braces
    name = var;
    len = get_name_len(&arg, &tofree, TRUE, FALSE);
    যদি (len > 0)
    {
	যদি (tofree != রিক্ত)
	    name = tofree;
	n = (eval_variable(name, len, 0, &tv, রিক্ত,
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
    প্রত্যাগম n;
}

স্থির lval_T	*redir_lval = রিক্ত;
#define EVALCMD_BUSY (redir_lval == (lval_T *)&redir_lval)
স্থির garray_T redir_ga;	// only valid when redir_lval is not NULL
স্থির char_u	*redir_endp = রিক্ত;
স্থির char_u	*redir_varname = রিক্ত;

    পূর্ণাঙ্ক
alloc_redir_lval(শূন্য)
{
    redir_lval = ALLOC_CLEAR_ONE(lval_T);
    যদি (redir_lval == রিক্ত)
	প্রত্যাগম FAIL;
    প্রত্যাগম OK;
}

    শূন্য
clear_redir_lval(শূন্য)
{
    VIM_CLEAR(redir_lval);
}

    শূন্য
init_redir_ga(শূন্য)
{
    ga_init2(&redir_ga, পরিমাণ(বর্ণ), 500);
}

/*
 * Start recording command output to a variable
 * When "append" is TRUE append to an existing variable.
 * Returns OK if successfully completed the setup.  FAIL otherwise.
 */
    পূর্ণাঙ্ক
var_redir_start(char_u *name, পূর্ণাঙ্ক append)
{
    পূর্ণাঙ্ক		called_emsg_before;
    typval_T	tv;

    // Catch a bad name early.
    যদি (!eval_isnamec1(*name))
    {
	emsg(_(e_invalid_argument));
	প্রত্যাগম FAIL;
    }

    // Make a copy of the name, it is used in redir_lval until redir ends.
    redir_varname = vim_strsave(name);
    যদি (redir_varname == রিক্ত)
	প্রত্যাগম FAIL;

    যদি (alloc_redir_lval() == FAIL)
    {
	var_redir_stop();
	প্রত্যাগম FAIL;
    }

    // The output is stored in growarray "redir_ga" until redirection ends.
    init_redir_ga();

    // Parse the variable name (can be a dict or list entry).
    redir_endp = get_lval(redir_varname, রিক্ত, redir_lval, FALSE, FALSE, 0,
							     FNE_CHECK_START);
    যদি (redir_endp == রিক্ত || redir_lval->ll_name == রিক্ত || *redir_endp != NUL)
    {
	clear_lval(redir_lval);
	যদি (redir_endp != রিক্ত && *redir_endp != NUL)
	    // Trailing characters are present after the variable name
	    semsg(_(e_trailing_characters_str), redir_endp);
	অন্যথা
	    semsg(_(e_invalid_argument_str), name);
	redir_endp = রিক্ত;  // don't store a value, only cleanup
	var_redir_stop();
	প্রত্যাগম FAIL;
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
	redir_endp = রিক্ত;  // don't store a value, only cleanup
	var_redir_stop();
	প্রত্যাগম FAIL;
    }

    প্রত্যাগম OK;
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
var_redir_str(char_u *মান, পূর্ণাঙ্ক মান_len)
{
    পূর্ণাঙ্ক		len;

    যদি (redir_lval == রিক্ত)
	প্রত্যাগম;

    যদি (মান_len == -1)
	len = (পূর্ণাঙ্ক)STRLEN(মান);	// Append the entire string
    অন্যথা
	len = মান_len;		// Append only "value_len" characters

    যদি (ga_grow(&redir_ga, len) == OK)
    {
	mch_memmove((বর্ণ *)redir_ga.ga_data + redir_ga.ga_len, মান, len);
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
	redir_lval = রিক্ত;
	প্রত্যাগম;
    }

    যদি (redir_lval != রিক্ত)
    {
	// If there was no error: assign the text to the variable.
	যদি (redir_endp != রিক্ত)
	{
	    ga_append(&redir_ga, NUL);  // Append the trailing NUL.
	    tv.v_type = VAR_STRING;
	    tv.vval.v_string = redir_ga.ga_data;
	    // Call get_lval() again, if it's inside a Dict or List it may
	    // have changed.
	    redir_endp = get_lval(redir_varname, রিক্ত, redir_lval,
					FALSE, FALSE, 0, FNE_CHECK_START);
	    যদি (redir_endp != রিক্ত && redir_lval->ll_name != রিক্ত)
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
get_clear_redir_ga(শূন্য)
{
    char_u *res;

    ga_append(&redir_ga, NUL);  // Append the trailing NUL.
    res = redir_ga.ga_data;
    redir_ga.ga_data = রিক্ত;
    প্রত্যাগম res;
}

/*
 * "gettabvar()" function
 */
    শূন্য
f_gettabvar(typval_T *argvars, typval_T *rettv)
{
    char_u	*varname;
    tabpage_T	*tp;
    win_T	*win = রিক্ত;

    যদি (in_vim9script()
	    && (check_for_সংখ্যা_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	প্রত্যাগম;

    varname = tv_get_string_chk(&argvars[1]);
    tp = find_tabpage((পূর্ণাঙ্ক)tv_get_সংখ্যা_chk(&argvars[0], রিক্ত));
    যদি (tp != রিক্ত)
	win = tp == curtab || tp->tp_firstwin == রিক্ত ? firstwin
		: tp->tp_firstwin;

    get_var_from(varname, rettv, &argvars[2], 't', tp, win, রিক্ত);
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
	প্রত্যাগম;

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
	প্রত্যাগম;

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
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	প্রত্যাগম;

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
	প্রত্যাগম;

    যদি (in_vim9script()
	    && (check_for_সংখ্যা_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	প্রত্যাগম;

    tp = find_tabpage((পূর্ণাঙ্ক)tv_get_সংখ্যা_chk(&argvars[0], রিক্ত));
    varname = tv_get_string_chk(&argvars[1]);
    varp = &argvars[2];

    যদি (varname == রিক্ত || tp == রিক্ত)
	প্রত্যাগম;

    save_curtab = curtab;
    save_lu_tp = lastused_tabpage;
    goto_tabpage_tp(tp, FALSE, FALSE);

    tabvarname = alloc(STRLEN(varname) + 3);
    যদি (tabvarname != রিক্ত)
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
	প্রত্যাগম;

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
	প্রত্যাগম;

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
	প্রত্যাগম;

    যদি (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	প্রত্যাগম;

    varname = tv_get_string_chk(&argvars[1]);
    buf = tv_get_buf_from_arg(&argvars[0]);
    varp = &argvars[2];

    যদি (buf == রিক্ত || varname == রিক্ত)
	প্রত্যাগম;

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
	যদি (bufvarname != রিক্ত)
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
    পূর্ণাঙ্ক		r = OK;

    CLEAR_FIELD(res);
    যদি (arg->v_type == VAR_PARTIAL && arg->vval.v_partial != রিক্ত)
    {
	res.cb_partial = arg->vval.v_partial;
	++res.cb_partial->pt_refcount;
	res.cb_name = partial_name(res.cb_partial);
    }
    অন্যথা
    {
	যদি (arg->v_type == VAR_STRING && arg->vval.v_string != রিক্ত
					       && SAFE_isdigit(*arg->vval.v_string))
	    r = FAIL;
	অন্যথা যদি (arg->v_type == VAR_FUNC || arg->v_type == VAR_STRING)
	{
	    res.cb_name = arg->vval.v_string;
	    যদি (arg->v_type == VAR_STRING)
	    {
		char_u *name = get_scriptlocal_funcname(arg->vval.v_string);
		যদি (name != রিক্ত)
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
	    res.cb_name = রিক্ত;
	}
    }
    প্রত্যাগম res;
}

/*
 * Copy a callback into a typval_T.
 */
    শূন্য
put_callback(callback_T *cb, typval_T *tv)
{
    যদি (cb->cb_partial != রিক্ত)
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
    প্রত্যাগম cb->cb_partial || cb->cb_free_name;
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
    যদি (dest->cb_partial != রিক্ত)
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

    যদি (!in_vim9script() || cb->cb_name == রিক্ত
	    || (!cb->cb_free_name
	       && (cb->cb_partial == রিক্ত || cb->cb_partial->pt_name == রিক্ত)))
	প্রত্যাগম;
    যদি (cb->cb_partial != রিক্ত)
	name = cb->cb_partial->pt_name;
    অন্যথা
	name = cb->cb_name;
    p = vim_strchr(name, '.');
    যদি (p == রিক্ত)
	প্রত্যাগম;

    import = find_imported(name, p - name, FALSE);
    যদি (import == রিক্ত || !SCRIPT_ID_VALID(import->imp_sid))
	প্রত্যাগম;

    scriptitem_T *si = SCRIPT_ITEM(import->imp_sid);
    যদি (si->sn_autoload_prefix == রিক্ত)
	প্রত্যাগম;

    char_u *newname = concat_str(si->sn_autoload_prefix, p + 1);
    যদি (newname == রিক্ত)
	প্রত্যাগম;

    যদি (cb->cb_partial != রিক্ত)
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
    যদি (callback->cb_partial != রিক্ত)
    {
	partial_unref(callback->cb_partial);
	callback->cb_partial = রিক্ত;
    }
    অন্যথা যদি (callback->cb_name != রিক্ত)
	func_unref(callback->cb_name);
    যদি (callback->cb_free_name)
    {
	vim_free(callback->cb_name);
	callback->cb_free_name = FALSE;
    }
    callback->cb_name = রিক্ত;
}

#endif // FEAT_EVAL
