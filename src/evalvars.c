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

#شامل "vim.h"

#اگر defined(FEAT_EVAL)

ایستا dictitem_T	globvars_var;		// variable used for g:
ایستا dict_T		globvardict;		// Dictionary with g: variables
#define globvarht globvardict.dv_hashtab

/*
 * Old Vim variables such as "v:version" are also available without the "v:".
 * Also in functions.  We need a special hashtable for them.
 */
ایستا hashtab_T	compat_hashtab;

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

تعریفنوع ساختار vimvar vimvar_T;

ایستا ساختار vimvar
{
    نویسه	*vv_name;	// name of variable, without v:
    dictitem16_T vv_di;		// value and name for key (max 16 chars!)
    type_T	*vv_type;	// type or NULL
    نویسه	vv_flags;	// VV_COMPAT, VV_RO, VV_RO_SBX
} vimvars[VV_LEN] =
{
    // The order here must match the VV_ defines in vim.h!
    // Initializing a union does not work, leave tv.vval empty to get zero's.
    {VV_NAME("count",		 VAR_عدد), پوچ, VV_COMPAT+VV_RO},
    {VV_NAME("count1",		 VAR_عدد), پوچ, VV_RO},
    {VV_NAME("prevcount",	 VAR_عدد), پوچ, VV_RO},
    {VV_NAME("errmsg",		 VAR_STRING), پوچ, VV_COMPAT},
    {VV_NAME("warningmsg",	 VAR_STRING), پوچ, 0},
    {VV_NAME("statusmsg",	 VAR_STRING), پوچ, 0},
    {VV_NAME("shell_error",	 VAR_عدد), پوچ, VV_COMPAT+VV_RO},
    {VV_NAME("this_session",	 VAR_STRING), پوچ, VV_COMPAT},
    {VV_NAME("version",		 VAR_عدد), پوچ, VV_COMPAT+VV_RO},
    {VV_NAME("lnum",		 VAR_عدد), پوچ, VV_RO_SBX},
    {VV_NAME("termresponse",	 VAR_STRING), پوچ, VV_RO},
    {VV_NAME("fname",		 VAR_STRING), پوچ, VV_RO},
    {VV_NAME("lang",		 VAR_STRING), پوچ, VV_RO},
    {VV_NAME("lc_time",		 VAR_STRING), پوچ, VV_RO},
    {VV_NAME("ctype",		 VAR_STRING), پوچ, VV_RO},
    {VV_NAME("charconvert_from", VAR_STRING), پوچ, VV_RO},
    {VV_NAME("charconvert_to",	 VAR_STRING), پوچ, VV_RO},
    {VV_NAME("fname_in",	 VAR_STRING), پوچ, VV_RO},
    {VV_NAME("fname_out",	 VAR_STRING), پوچ, VV_RO},
    {VV_NAME("fname_new",	 VAR_STRING), پوچ, VV_RO},
    {VV_NAME("fname_diff",	 VAR_STRING), پوچ, VV_RO},
    {VV_NAME("cmdarg",		 VAR_STRING), پوچ, VV_RO},
    {VV_NAME("foldstart",	 VAR_عدد), پوچ, VV_RO_SBX},
    {VV_NAME("foldend",		 VAR_عدد), پوچ, VV_RO_SBX},
    {VV_NAME("folddashes",	 VAR_STRING), پوچ, VV_RO_SBX},
    {VV_NAME("foldlevel",	 VAR_عدد), پوچ, VV_RO_SBX},
    {VV_NAME("progname",	 VAR_STRING), پوچ, VV_RO},
    {VV_NAME("servername",	 VAR_STRING), پوچ, VV_RO},
    {VV_NAME("dying",		 VAR_عدد), پوچ, VV_RO},
    {VV_NAME("exception",	 VAR_STRING), پوچ, VV_RO},
    {VV_NAME("throwpoint",	 VAR_STRING), پوچ, VV_RO},
    {VV_NAME("register",	 VAR_STRING), پوچ, VV_RO},
    {VV_NAME("cmdbang",		 VAR_عدد), پوچ, VV_RO},
    {VV_NAME("insertmode",	 VAR_STRING), پوچ, VV_RO},
    {VV_NAME("val",		 VAR_UNKNOWN), پوچ, VV_RO},
    {VV_NAME("key",		 VAR_UNKNOWN), پوچ, VV_RO},
    {VV_NAME("profiling",	 VAR_عدد), پوچ, VV_RO},
    {VV_NAME("fcs_reason",	 VAR_STRING), پوچ, VV_RO},
    {VV_NAME("fcs_choice",	 VAR_STRING), پوچ, 0},
    {VV_NAME("beval_bufnr",	 VAR_عدد), پوچ, VV_RO},
    {VV_NAME("beval_winnr",	 VAR_عدد), پوچ, VV_RO},
    {VV_NAME("beval_winid",	 VAR_عدد), پوچ, VV_RO},
    {VV_NAME("beval_lnum",	 VAR_عدد), پوچ, VV_RO},
    {VV_NAME("beval_col",	 VAR_عدد), پوچ, VV_RO},
    {VV_NAME("beval_text",	 VAR_STRING), پوچ, VV_RO},
    {VV_NAME("scrollstart",	 VAR_STRING), پوچ, 0},
    {VV_NAME("swapname",	 VAR_STRING), پوچ, VV_RO},
    {VV_NAME("swapchoice",	 VAR_STRING), پوچ, 0},
    {VV_NAME("swapcommand",	 VAR_STRING), پوچ, VV_RO},
    {VV_NAME("char",		 VAR_STRING), پوچ, 0},
    {VV_NAME("mouse_win",	 VAR_عدد), پوچ, 0},
    {VV_NAME("mouse_winid",	 VAR_عدد), پوچ, 0},
    {VV_NAME("mouse_lnum",	 VAR_عدد), پوچ, 0},
    {VV_NAME("mouse_col",	 VAR_عدد), پوچ, 0},
    {VV_NAME("operator",	 VAR_STRING), پوچ, VV_RO},
    {VV_NAME("searchforward",	 VAR_عدد), پوچ, 0},
    {VV_NAME("hlsearch",	 VAR_عدد), پوچ, 0},
    {VV_NAME("oldfiles",	 VAR_LIST), &t_list_string, 0},
    {VV_NAME("windowid",	 VAR_عدد), پوچ, VV_RO},
    {VV_NAME("progpath",	 VAR_STRING), پوچ, VV_RO},
    {VV_NAME("completed_item",	 VAR_DICT), &t_dict_string, 0},
    {VV_NAME("option_new",	 VAR_STRING), پوچ, VV_RO},
    {VV_NAME("option_old",	 VAR_STRING), پوچ, VV_RO},
    {VV_NAME("option_oldlocal",	 VAR_STRING), پوچ, VV_RO},
    {VV_NAME("option_oldglobal", VAR_STRING), پوچ, VV_RO},
    {VV_NAME("option_command",	 VAR_STRING), پوچ, VV_RO},
    {VV_NAME("option_type",	 VAR_STRING), پوچ, VV_RO},
    {VV_NAME("errors",		 VAR_LIST), &t_list_string, 0},
    {VV_NAME("false",		 VAR_BOOL), پوچ, VV_RO},
    {VV_NAME("true",		 VAR_BOOL), پوچ, VV_RO},
    {VV_NAME("none",		 VAR_SPECIAL), پوچ, VV_RO},
    {VV_NAME("null",		 VAR_SPECIAL), پوچ, VV_RO},
    {VV_NAME("numbermax",	 VAR_عدد), پوچ, VV_RO},
    {VV_NAME("numbermin",	 VAR_عدد), پوچ, VV_RO},
    {VV_NAME("numbersize",	 VAR_عدد), پوچ, VV_RO},
    {VV_NAME("vim_did_enter",	 VAR_عدد), پوچ, VV_RO},
    {VV_NAME("testing",		 VAR_عدد), پوچ, 0},
    {VV_NAME("t_number",	 VAR_عدد), پوچ, VV_RO},
    {VV_NAME("t_string",	 VAR_عدد), پوچ, VV_RO},
    {VV_NAME("t_func",		 VAR_عدد), پوچ, VV_RO},
    {VV_NAME("t_list",		 VAR_عدد), پوچ, VV_RO},
    {VV_NAME("t_dict",		 VAR_عدد), پوچ, VV_RO},
    {VV_NAME("t_float",		 VAR_عدد), پوچ, VV_RO},
    {VV_NAME("t_bool",		 VAR_عدد), پوچ, VV_RO},
    {VV_NAME("t_none",		 VAR_عدد), پوچ, VV_RO},
    {VV_NAME("t_job",		 VAR_عدد), پوچ, VV_RO},
    {VV_NAME("t_channel",	 VAR_عدد), پوچ, VV_RO},
    {VV_NAME("t_blob",		 VAR_عدد), پوچ, VV_RO},
    {VV_NAME("t_class",		 VAR_عدد), پوچ, VV_RO},
    {VV_NAME("t_object",	 VAR_عدد), پوچ, VV_RO},
    {VV_NAME("termrfgresp",	 VAR_STRING), پوچ, VV_RO},
    {VV_NAME("termrbgresp",	 VAR_STRING), پوچ, VV_RO},
    {VV_NAME("termu7resp",	 VAR_STRING), پوچ, VV_RO},
    {VV_NAME("termstyleresp",	 VAR_STRING), پوچ, VV_RO},
    {VV_NAME("termblinkresp",	 VAR_STRING), پوچ, VV_RO},
    {VV_NAME("event",		 VAR_DICT), پوچ, VV_RO},
    {VV_NAME("versionlong",	 VAR_عدد), پوچ, VV_RO},
    {VV_NAME("echospace",	 VAR_عدد), پوچ, VV_RO},
    {VV_NAME("argv",		 VAR_LIST), &t_list_string, VV_RO},
    {VV_NAME("collate",		 VAR_STRING), پوچ, VV_RO},
    {VV_NAME("exiting",		 VAR_SPECIAL), پوچ, VV_RO},
    {VV_NAME("colornames",       VAR_DICT), &t_dict_string, VV_RO},
    {VV_NAME("sizeofint",	 VAR_عدد), پوچ, VV_RO},
    {VV_NAME("sizeoflong",	 VAR_عدد), پوچ, VV_RO},
    {VV_NAME("sizeofpointer",	 VAR_عدد), پوچ, VV_RO},
    {VV_NAME("maxcol",		 VAR_عدد), پوچ, VV_RO},
    {VV_NAME("python3_version",	 VAR_عدد), پوچ, VV_RO},
    {VV_NAME("t_typealias",	 VAR_عدد), پوچ, VV_RO},
    {VV_NAME("t_enum",		 VAR_عدد), پوچ, VV_RO},
    {VV_NAME("t_enumvalue",	 VAR_عدد), پوچ, VV_RO},
    {VV_NAME("stacktrace",	 VAR_LIST), &t_list_dict_any, VV_RO},
    {VV_NAME("t_tuple",		 VAR_عدد), پوچ, VV_RO},
    {VV_NAME("wayland_display",  VAR_STRING), پوچ, VV_RO},
    {VV_NAME("clipmethod",	 VAR_STRING), پوچ, VV_RO},
    {VV_NAME("termda1",		 VAR_STRING), پوچ, VV_RO},
    {VV_NAME("termosc",	 VAR_STRING), پوچ, VV_RO},
    {VV_NAME("vim_did_init",	 VAR_عدد), پوچ, VV_RO},
    {VV_NAME("clipproviders",	 VAR_DICT), پوچ, VV_RO},
    {VV_NAME("imagebackend",	 VAR_STRING), پوچ, VV_RO}
};

// shorthand
#define vv_tv_type	vv_di.di_tv.v_type
#define vv_nr		vv_di.di_tv.vval.v_عدد
#define vv_float	vv_di.di_tv.vval.v_float
#define vv_str		vv_di.di_tv.vval.v_string
#define vv_list		vv_di.di_tv.vval.v_list
#define vv_dict		vv_di.di_tv.vval.v_dict
#define vv_blob		vv_di.di_tv.vval.v_blob
#define vv_tv		vv_di.di_tv

ایستا dictitem_T	vimvars_var;		// variable used for v:
ایستا dict_T		vimvardict;		// Dictionary with v: variables
#define vimvarht  vimvardict.dv_hashtab

// for VIM_VERSION_ defines
#شامل "version.h"

ایستا تهی list_glob_vars(صحیح *first);
ایستا تهی list_buf_vars(صحیح *first);
ایستا تهی list_win_vars(صحیح *first);
ایستا تهی list_tab_vars(صحیح *first);
ایستا char_u *list_arg_vars(exarg_T *eap, char_u *arg, صحیح *first);
ایستا char_u *ex_let_one(char_u *arg, typval_T *tv, صحیح copy, صحیح flags, char_u *endchars, char_u *op, صحیح var_idx);
ایستا صحیح do_unlet_var(lval_T *lp, char_u *name_end, exarg_T *eap, صحیح deep, تهی *cookie);
ایستا صحیح do_lock_var(lval_T *lp, char_u *name_end, exarg_T *eap, صحیح deep, تهی *cookie);
ایستا تهی list_one_var(dictitem_T *v, نویسه *prefix, صحیح *first);
ایستا تهی list_one_var_a(نویسه *prefix, char_u *name, صحیح type, char_u *string, صحیح *first);

/*
 * Initialize global and vim special variables
 */
    تهی
evalvars_init(تهی)
{
    صحیح		    i;
    ساختار vimvar   *p;

    init_var_dict(&globvardict, &globvars_var, VAR_DEF_SCOPE);
    init_var_dict(&vimvardict, &vimvars_var, VAR_SCOPE);
    vimvardict.dv_lock = VAR_FIXED;
    hash_init(&compat_hashtab);

    برای (i = 0; i < VV_LEN; ++i)
    {
	p = &vimvars[i];
	اگر (p->vv_flags & VV_RO)
	    p->vv_di.di_flags = DI_FLAGS_RO | DI_FLAGS_FIX;
	وگرنه اگر (p->vv_flags & VV_RO_SBX)
	    p->vv_di.di_flags = DI_FLAGS_RO_SBX | DI_FLAGS_FIX;
	وگرنه
	    p->vv_di.di_flags = DI_FLAGS_FIX;

	// add to v: scope dict, unless the value is not always available
	اگر (p->vv_tv_type != VAR_UNKNOWN)
	    hash_add(&vimvarht, p->vv_di.di_کلید, "initialization");
	اگر (p->vv_flags & VV_COMPAT)
	    // add to compat scope dict
	    hash_add(&compat_hashtab, p->vv_di.di_کلید, "initialization");
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
    set_vim_var_nr(VV_NUMBERSIZE, اندازه(varnumber_T) * 8);
    set_vim_var_nr(VV_SIZEOFINT, اندازه(صحیح));
    set_vim_var_nr(VV_SIZEOFLONG, اندازه(long));
    set_vim_var_nr(VV_SIZEOFPOINTER, اندازه(نویسه *));
    set_vim_var_nr(VV_MAXCOL, MAXCOL);

    set_vim_var_nr(VV_TYPE_عدد,  VAR_TYPE_عدد);
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

#اگر defined(EXITFREE)
/*
 * Free all vim variables information on exit
 */
    تهی
evalvars_clear(تهی)
{
    صحیح		    i;
    ساختار vimvar   *p;

    برای (i = 0; i < VV_LEN; ++i)
    {
	p = &vimvars[i];
	اگر (p->vv_di.di_tv.v_type == VAR_STRING)
	    VIM_CLEAR(p->vv_str);
	وگرنه اگر (p->vv_di.di_tv.v_type == VAR_LIST)
	{
	    list_unref(p->vv_list);
	    p->vv_list = پوچ;
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
    برای (i = 1; i <= script_اقلام.ga_len; ++i)
	vars_clear(&SCRIPT_VARS(i));
}
#endif

    صحیح
garbage_collect_globvars(صحیح copyID)
{
    بازگشت set_ref_in_ht(&globvarht, copyID, پوچ, پوچ);
}

    صحیح
garbage_collect_vimvars(صحیح copyID)
{
    بازگشت set_ref_in_ht(&vimvarht, copyID, پوچ, پوچ);
}

    صحیح
garbage_collect_scriptvars(صحیح copyID)
{
    صحیح		    i;
    صحیح		    idx;
    صحیح		    abort = FALSE;
    scriptitem_T    *si;

    برای (i = 1; i <= script_اقلام.ga_len; ++i)
    {
	abort = abort || set_ref_in_ht(&SCRIPT_VARS(i), copyID, پوچ, پوچ);

	si = SCRIPT_ITEM(i);
	برای (idx = 0; idx < si->sn_var_vals.ga_len; ++idx)
	{
	    svar_T    *sv = ((svar_T *)si->sn_var_vals.ga_data) + idx;

	    اگر (sv->sv_name != پوچ)
		abort = abort || set_ref_in_item(sv->sv_tv, copyID, پوچ, پوچ, پوچ);
	}
    }

    بازگشت abort;
}

/*
 * Set an internal variable to a string value. Creates the variable if it does
 * not already exist.
 */
    تهی
set_internal_string_var(char_u *name, char_u *مقدار)
{
    char_u	*val;
    typval_T	*tvp;

    val = vim_strsave(مقدار);
    اگر (val == پوچ)
	بازگشت;

    tvp = alloc_string_tv(val);
    اگر (tvp == پوچ)
	بازگشت;

    set_var(name, tvp, FALSE);
    free_tv(tvp);
}

    صحیح
eval_charconvert(
    char_u	*enc_from,
    char_u	*enc_to,
    char_u	*fname_from,
    char_u	*fname_to)
{
    صحیح		err = FALSE;
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;

    set_vim_var_string(VV_CC_FROM, enc_from, -1);
    set_vim_var_string(VV_CC_TO, enc_to, -1);
    set_vim_var_string(VV_FNAME_IN, fname_from, -1);
    set_vim_var_string(VV_FNAME_OUT, fname_to, -1);
    ctx = get_option_sctx("charconvert");
    اگر (ctx != پوچ)
	current_sctx = *ctx;

    اگر (eval_to_bool(p_ccv, &err, پوچ, FALSE, TRUE))
	err = TRUE;

    set_vim_var_string(VV_CC_FROM, پوچ, -1);
    set_vim_var_string(VV_CC_TO, پوچ, -1);
    set_vim_var_string(VV_FNAME_IN, پوچ, -1);
    set_vim_var_string(VV_FNAME_OUT, پوچ, -1);
    current_sctx = saved_sctx;

    اگر (err)
	بازگشت FAIL;
    بازگشت OK;
}

#اگر defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
    صحیح
eval_printexpr(char_u *fname, char_u *args)
{
    صحیح		err = FALSE;
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;

    set_vim_var_string(VV_FNAME_IN, fname, -1);
    set_vim_var_string(VV_CMDARG, args, -1);
    ctx = get_option_sctx("printexpr");
    اگر (ctx != پوچ)
	current_sctx = *ctx;

    اگر (eval_to_bool(p_pexpr, &err, پوچ, FALSE, TRUE))
	err = TRUE;

    set_vim_var_string(VV_FNAME_IN, پوچ, -1);
    set_vim_var_string(VV_CMDARG, پوچ, -1);
    current_sctx = saved_sctx;

    اگر (err)
    {
	mch_remove(fname);
	بازگشت FAIL;
    }
    بازگشت OK;
}
#endif

#اگر defined(FEAT_DIFF)
    تهی
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
    اگر (ctx != پوچ)
	current_sctx = *ctx;

    // errors are ignored
    tv = eval_عبارت_ext(p_dex, پوچ, TRUE);
    free_tv(tv);

    set_vim_var_string(VV_FNAME_IN, پوچ, -1);
    set_vim_var_string(VV_FNAME_NEW, پوچ, -1);
    set_vim_var_string(VV_FNAME_OUT, پوچ, -1);
    current_sctx = saved_sctx;
}

    تهی
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
    اگر (ctx != پوچ)
	current_sctx = *ctx;

    // errors are ignored
    tv = eval_عبارت_ext(p_pex, پوچ, TRUE);
    free_tv(tv);

    set_vim_var_string(VV_FNAME_IN, پوچ, -1);
    set_vim_var_string(VV_FNAME_DIFF, پوچ, -1);
    set_vim_var_string(VV_FNAME_OUT, پوچ, -1);
    current_sctx = saved_sctx;
}
#endif

#اگر defined(FEAT_SPELL)
/*
 * Evaluate an expression to a list with suggestions.
 * For the "expr:" part of 'spellsuggest'.
 * Returns NULL when there is an error.
 */
    list_T *
eval_spell_عبارت(char_u *badword, char_u *عبارت)
{
    typval_T	save_val;
    typval_T	rettv;
    list_T	*list = پوچ;
    char_u	*p = skipwhite(عبارت);
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;
    صحیح		r;

    // Set "v:val" to the bad word.
    prepare_vimvar(VV_VAL, &save_val);
    set_vim_var_string(VV_VAL, badword, -1);
    اگر (p_verbose == 0)
	++emsg_off;
    ctx = get_option_sctx("spellsuggest");
    اگر (ctx != پوچ)
	current_sctx = *ctx;

    r = may_call_simple_func(p, &rettv);
    اگر (r == NOTDONE)
	r = eval1(&p, &rettv, &EVALARG_EVALUATE);
    اگر (r == OK)
    {
	اگر (rettv.v_type != VAR_LIST)
	    clear_tv(&rettv);
	وگرنه
	    list = rettv.vval.v_list;
    }

    اگر (p_verbose == 0)
	--emsg_off;
    clear_tv(get_vim_var_tv(VV_VAL));
    restore_vimvar(VV_VAL, &save_val);
    current_sctx = saved_sctx;

    بازگشت list;
}

/*
 * "list" is supposed to contain two items: a word and a number.  Return the
 * word in "pp" and the number as the return value.
 * Return -1 if anything isn't right.
 * Used to get the good word and score from the eval_spell_expr() result.
 */
    صحیح
get_spellword(list_T *list, char_u **pp)
{
    listitem_T	*li;

    li = list->lv_first;
    اگر (li == پوچ)
	بازگشت -1;
    *pp = tv_get_string(&li->li_tv);

    li = li->li_next;
    اگر (li == پوچ)
	بازگشت -1;
    بازگشت (صحیح)tv_get_عدد(&li->li_tv);
}
#endif

/*
 * Prepare v: variable "idx" to be used.
 * Save the current typeval in "save_tv" and clear it.
 * When not used yet add the variable to the v: hashtable.
 */
    تهی
prepare_vimvar(صحیح idx, typval_T *save_tv)
{
    *save_tv = vimvars[idx].vv_tv;
    vimvars[idx].vv_str = پوچ;  // don't free it now
    اگر (vimvars[idx].vv_tv_type == VAR_UNKNOWN)
	hash_add(&vimvarht, vimvars[idx].vv_di.di_کلید, "prepare vimvar");
}

/*
 * Restore v: variable "idx" to typeval "save_tv".
 * Note that the v: variable must have been cleared already.
 * When no longer defined, remove the variable from the v: hashtable.
 */
    تهی
restore_vimvar(صحیح idx, typval_T *save_tv)
{
    hashitem_T	*hi;

    vimvars[idx].vv_tv = *save_tv;
    اگر (vimvars[idx].vv_tv_type != VAR_UNKNOWN)
	بازگشت;

    hi = hash_find(&vimvarht, vimvars[idx].vv_di.di_کلید);
    اگر (HASHITEM_EMPTY(hi))
	internal_error("restore_vimvar()");
    وگرنه
	hash_remove(&vimvarht, hi, "restore vimvar");
}

/*
 * List Vim variables.
 */
    ایستا تهی
list_vim_vars(صحیح *first)
{
    list_hashtable_vars(&vimvarht, "v:", FALSE, first);
}

/*
 * List script-local variables, if there is a script.
 */
    ایستا تهی
list_script_vars(صحیح *first)
{
    اگر (SCRIPT_ID_VALID(current_sctx.sc_sid))
	list_hashtable_vars(&SCRIPT_VARS(current_sctx.sc_sid),
							   "s:", FALSE, first);
}

/*
 * Return TRUE if "name" starts with "g:", "w:", "t:" or "b:".
 * But only when an identifier character follows.
 */
    صحیح
is_scoped_variable(char_u *name)
{
    بازگشت vim_strchr((char_u *)"gwbt", name[0]) != پوچ
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
eval_one_عبارت_in_str(char_u *p, garray_T *gap, صحیح evaluate)
{
    char_u	*block_start = skipwhite(p + 1);  // skip the opening {
    char_u	*block_end = block_start;
    char_u	*عبارت_val;

    اگر (*block_start == NUL)
    {
	semsg(_(e_missing_close_curly_str), p);
	بازگشت پوچ;
    }
    اگر (skip_عبارت(&block_end, پوچ) == FAIL)
	بازگشت پوچ;
    block_end = skipwhite(block_end);
    اگر (*block_end != '}')
    {
	semsg(_(e_missing_close_curly_str), p);
	بازگشت پوچ;
    }
    اگر (evaluate)
    {
	*block_end = NUL;
	عبارت_val = eval_to_string(block_start, FALSE, FALSE);
	*block_end = '}';
	اگر (عبارت_val == پوچ)
	    بازگشت پوچ;
	ga_concat(gap, عبارت_val);
	vim_free(عبارت_val);
    }

    بازگشت block_end + 1;
}

/*
 * Evaluate all the Vim expressions {expr} in "str" and return the resulting
 * string in allocated memory.  "{{" is reduced to "{" and "}}" to "}".
 * Used for a heredoc assignment.
 * Returns NULL for an error.
 */
    ایستا char_u *
eval_all_عبارت_in_str(char_u *str)
{
    garray_T	ga;
    char_u	*p;

    ga_init2(&ga, 1, 80);
    p = str;

    وقتی (*p != NUL)
    {
	char_u	*lit_start;
	صحیح	escaped_brace = FALSE;

	// Look for a block start.
	lit_start = p;
	وقتی (*p != '{' && *p != '}' && *p != NUL)
	    ++p;

	اگر (*p != NUL && *p == p[1])
	{
	    // Escaped brace, unescape and continue.
	    // Include the brace in the literal string.
	    ++p;
	    escaped_brace = TRUE;
	}
	وگرنه اگر (*p == '}')
	{
	    semsg(_(e_stray_closing_curly_str), str);
	    ga_clear(&ga);
	    بازگشت پوچ;
	}

	// Append the literal part.
	ga_concat_len(&ga, lit_start, (size_t)(p - lit_start));

	اگر (*p == NUL)
	    بشکن;

	اگر (escaped_brace)
	{
	    // Skip the second brace.
	    ++p;
	    ادامه;
	}

	// Evaluate the expression and append the result.
	p = eval_one_عبارت_in_str(p, &ga, TRUE);
	اگر (p == پوچ)
	{
	    ga_clear(&ga);
	    بازگشت پوچ;
	}
    }
    ga_append(&ga, NUL);

    بازگشت ga.ga_data;
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
heredoc_get(exarg_T *eap, char_u *cmd, صحیح script_get, صحیح vim9compile)
{
    char_u	*theline = پوچ;
    char_u	*marker;
    list_T	*l;
    char_u	*p;
    char_u	*str;
    صحیح		marker_indent_len = 0;
    صحیح		text_indent_len = 0;
    char_u	*text_indent = پوچ;
    char_u	dot[] = ".";
    صحیح		comment_char = in_vim9script() ? '#' : '"';
    صحیح		evalstr = FALSE;
    صحیح		eval_failed = FALSE;
    cctx_T	*cctx = vim9compile ? eap->cookie : پوچ;
    صحیح		شمار = 0;
    صحیح		heredoc_in_string = FALSE;
    char_u	*سطر_arg = پوچ;
    char_u	*nl_ptr = vim_strchr(cmd, '\n');

    اگر (nl_ptr != پوچ)
    {
	heredoc_in_string = TRUE;
	سطر_arg = nl_ptr + 1;
	*nl_ptr = NUL;
    }
    وگرنه اگر (eap->ea_getline == پوچ)
    {
	emsg(_(e_cannot_use_heredoc_here));
	بازگشت پوچ;
    }

    // Check for the optional 'trim' word before the marker
    cmd = skipwhite(cmd);

    وقتی (TRUE)
    {
	اگر (STRNCMP(cmd, "trim", 4) == 0
		&& (cmd[4] == NUL || VIM_ISWHITE(cmd[4])))
	{
	    cmd = skipwhite(cmd + 4);

	    // Trim the indentation from all the lines in the here document.
	    // The amount of indentation trimmed is the same as the indentation
	    // of the first line after the :let command line.  To find the end
	    // marker the indent of the :let command line is trimmed.
	    p = *eap->cmdlinep;
	    وقتی (VIM_ISWHITE(*p))
	    {
		p++;
		marker_indent_len++;
	    }
	    text_indent_len = -1;

	    ادامه;
	}
	اگر (STRNCMP(cmd, "eval", 4) == 0
		&& (cmd[4] == NUL || VIM_ISWHITE(cmd[4])))
	{
	    cmd = skipwhite(cmd + 4);
	    evalstr = TRUE;
	    ادامه;
	}
	بشکن;
    }

    // The marker is the next word.
    اگر (*cmd != NUL && *cmd != comment_char)
    {
	marker = skipwhite(cmd);
	p = skiptowhite(marker);
	اگر (*skipwhite(p) != NUL && *skipwhite(p) != comment_char)
	{
	    semsg(_(e_trailing_characters_str), p);
	    بازگشت پوچ;
	}
	*p = NUL;
	اگر (!script_get && vim_islower(*marker))
	{
	    emsg(_(e_marker_cannot_start_with_lower_case_letter));
	    بازگشت پوچ;
	}
    }
    وگرنه
    {
	// When getting lines for an embedded script, if the marker is missing,
	// accept '.' as the marker.
	اگر (script_get)
	    marker = dot;
	وگرنه
	{
	    emsg(_(e_missing_marker));
	    بازگشت پوچ;
	}
    }

    l = list_alloc();
    اگر (l == پوچ)
	بازگشت پوچ;

    برای (;;)
    {
	صحیح	mi = 0;
	صحیح	ti = 0;

	اگر (heredoc_in_string)
	{
	    char_u	*next_سطر;

	    // heredoc in a string separated by newlines.  Get the next line
	    // from the string.

	    اگر (*سطر_arg == NUL)
	    {
		semsg(_(e_missing_end_marker_str), marker);
		بشکن;
	    }

	    theline = سطر_arg;
	    next_سطر = vim_strchr(theline, '\n');
	    اگر (next_سطر == پوچ)
		سطر_arg += STRLEN(سطر_arg);
	    وگرنه
	    {
		*next_سطر = NUL;
		سطر_arg = next_سطر + 1;
	    }
	}
	وگرنه
	{
	    vim_free(theline);
	    theline = eap->ea_getline(NUL, eap->cookie, 0, FALSE);
	    اگر (theline == پوچ)
	    {
		semsg(_(e_missing_end_marker_str), marker);
		بشکن;
	    }
	}

	// with "trim": skip the indent matching the :let line to find the
	// marker
	اگر (marker_indent_len > 0
		&& STRNCMP(theline, *eap->cmdlinep, marker_indent_len) == 0)
	    mi = marker_indent_len;
	اگر (STRCMP(marker, theline + mi) == 0)
	    بشکن;

	// If expression evaluation failed in the heredoc, then skip till the
	// end marker.
	اگر (eval_failed)
	    ادامه;

	اگر (text_indent_len == -1 && *theline != NUL)
	{
	    // set the text indent from the first line.
	    p = theline;
	    text_indent_len = 0;
	    وقتی (VIM_ISWHITE(*p))
	    {
		p++;
		text_indent_len++;
	    }
	    text_indent = vim_strnsave(theline, text_indent_len);
	}
	// with "trim": skip the indent matching the first line
	اگر (text_indent != پوچ)
	    برای (ti = 0; ti < text_indent_len; ++ti)
		اگر (theline[ti] != text_indent[ti])
		    بشکن;

	str = theline + ti;
	اگر (vim9compile)
	{
	    اگر (compile_all_عبارت_in_str(str, evalstr, cctx) == FAIL)
	    {
		vim_free(theline);
		vim_free(text_indent);
		list_free(l);
		بازگشت FAIL;
	    }
	    شمار++;
	}
	وگرنه
	{
	    صحیح	    free_str = FALSE;

	    اگر (evalstr && !eap->skip)
	    {
		str = eval_all_عبارت_in_str(str);
		اگر (str == پوچ)
		{
		    // expression evaluation failed
		    eval_failed = TRUE;
		    ادامه;
		}
		free_str = TRUE;
	    }

	    اگر (list_append_string(l, str, -1) == FAIL)
	    {
		اگر (free_str)
		    vim_free(str);
		بشکن;
	    }
	    اگر (free_str)
		vim_free(str);
	}
    }
    اگر (heredoc_in_string)
	// Next command follows the heredoc in the string.
	eap->nextcmd = سطر_arg;
    وگرنه
	vim_free(theline);
    vim_free(text_indent);

    اگر (vim9compile && cctx->ctx_skip != SKIP_YES && !eval_failed)
	generate_NEWLIST(cctx, شمار, FALSE);

    اگر (eval_failed)
    {
	// expression evaluation in the heredoc failed
	list_free(l);
	بازگشت پوچ;
    }
    بازگشت l;
}

/*
 * Vim9 variable declaration:
 * ":var name"
 * ":var name: type"
 * ":var name = expr"
 * ":var name: type = expr"
 * etc.
 */
    تهی
ex_var(exarg_T *eap)
{
    char_u *p = eap->cmd;
    صحیح	    has_var;

    اگر (!in_vim9script())
    {
	semsg(_(e_str_cannot_be_used_in_legacy_vim_script), ":var");
	بازگشت;
    }
    has_var = checkforcmd_noparen(&p, "var", 3);
    اگر (current_sctx.sc_sid == 0 && has_var)
    {
	emsg(_(e_cannot_declare_variable_on_command_سطر));
	بازگشت;
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
    تهی
ex_let(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    char_u	*عبارت = پوچ;
    typval_T	rettv;
    صحیح		var_شمار = 0;
    صحیح		semicolon = 0;
    char_u	op[4];
    char_u	*argend;
    صحیح		first = TRUE;
    صحیح		concat;
    صحیح		has_assign;
    صحیح		flags = 0;
    صحیح		vim9script = in_vim9script();

    اگر (eap->cmdidx == CMD_final && !vim9script)
    {
	// In legacy Vim script ":final" is short for ":finally".
	ex_finally(eap);
	بازگشت;
    }
    اگر (eap->cmdidx == CMD_let && vim9script)
    {
	emsg(_(e_cannot_use_let_in_vim9_script));
	بازگشت;
    }

    اگر (eap->cmdidx == CMD_const)
	flags |= ASSIGN_CONST;
    وگرنه اگر (eap->cmdidx == CMD_final)
	flags |= ASSIGN_FINAL;

    // Vim9 assignment without ":let", ":const" or ":final"
    اگر (eap->arg == eap->cmd)
	flags |= ASSIGN_NO_DECL;

    argend = skip_var_list(arg, TRUE, &var_شمار, &semicolon, FALSE);
    اگر (argend == پوچ)
	بازگشت;
    عبارت = skipwhite(argend);
    concat = عبارت[0] == '.'
	&& ((عبارت[1] == '=' && in_old_script(2))
		|| (عبارت[1] == '.' && عبارت[2] == '='));
    has_assign =  *عبارت == '=' || (vim_strchr((char_u *)"+-*/%", *عبارت) != پوچ
							    && عبارت[1] == '=');
    اگر (!has_assign && !concat)
    {
	// ":let" without "=": list variables
	اگر (*arg == '[')
	    emsg(_(e_invalid_argument));
	وگرنه اگر (عبارت[0] == '.' && عبارت[1] == '=')
	    emsg(_(e_dot_equal_not_supported_with_script_version_two));
	وگرنه اگر (!ends_excmd2(eap->cmd, arg))
	{
	    اگر (vim9script)
	    {
		اگر (!ends_excmd2(eap->cmd, skipwhite(argend)))
		    semsg(_(e_trailing_characters_str), argend);
		وگرنه
		    // Vim9 declaration ":var name: type"
		    arg = vim9_declare_scriptvar(eap, arg);
	    }
	    وگرنه
	    {
		// ":let var1 var2" - list values
		arg = list_arg_vars(eap, arg, &first);
	    }
	}
	وگرنه اگر (!eap->skip)
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
	بازگشت;
    }

    اگر (عبارت[0] == '=' && عبارت[1] == '<' && عبارت[2] == '<')
    {
	list_T	*l = پوچ;
	long	cur_lnum = SOURCING_LNUM;

	// :let text =<< [trim] [eval] END
	// :var text =<< [trim] [eval] END
	اگر (vim9script && !eap->skip && (!VIM_ISWHITE(عبارت[-1])
						 || !IS_WHITE_OR_NUL(عبارت[3])))
	    semsg(_(e_white_space_required_before_and_after_str_at_str),
								  "=<<", عبارت);
	وگرنه
	    l = heredoc_get(eap, عبارت + 3, FALSE, FALSE);

	اگر (l != پوچ)
	{
	    rettv_list_set(&rettv, l);
	    اگر (!eap->skip)
	    {
		// errors are for the assignment, not the end marker
		SOURCING_LNUM = cur_lnum;
		op[0] = '=';
		op[1] = NUL;
		(تهی)ex_let_vars(eap->arg, &rettv, FALSE, semicolon, var_شمار,
								flags, op);
	    }
	    clear_tv(&rettv);
	}
	بازگشت;
    }

    evalarg_T   evalarg;
    صحیح	    len = 1;

    CLEAR_FIELD(rettv);

    صحیح cur_lnum;

    op[0] = '=';
    op[1] = NUL;
    اگر (*عبارت != '=')
    {
	اگر (vim9script && (flags & ASSIGN_NO_DECL) == 0)
	{
	    // +=, /=, etc. require an existing variable
	    semsg(_(e_cannot_use_operator_on_new_variable_str), eap->arg);
	}
	وگرنه اگر (vim_strchr((char_u *)"+-*/%.", *عبارت) != پوچ)
	{
	    op[0] = *عبارت;   // +=, -=, *=, /=, %= or .=
	    ++len;
	    اگر (عبارت[0] == '.' && عبارت[1] == '.') // ..=
	    {
		++عبارت;
		++len;
	    }
	}
	عبارت += 2;
    }
    وگرنه
	++عبارت;

    اگر (vim9script && !eap->skip && (!VIM_ISWHITE(*argend)
					       || !IS_WHITE_OR_NUL(*عبارت)))
    {
	vim_strncpy(op, عبارت - len, len);
	semsg(_(e_white_space_required_before_and_after_str_at_str),
							       op, argend);
    }

    اگر (eap->skip)
	++emsg_skip;
    fill_evalarg_from_eap(&evalarg, eap, eap->skip);
    عبارت = skipwhite_and_linebreak(عبارت, &evalarg);
    cur_lnum = SOURCING_LNUM;
    صحیح eval_res = eval0(عبارت, &rettv, eap, &evalarg);
    اگر (eap->skip)
	--emsg_skip;
    clear_evalarg(&evalarg, eap);

    // Restore the line number so that any type error is given for the
    // declaration, not the expression.
    SOURCING_LNUM = cur_lnum;

    اگر (!eap->skip && eval_res != FAIL)
	(تهی)ex_let_vars(eap->arg, &rettv, FALSE, semicolon, var_شمار,
								    flags, op);
    اگر (eval_res != FAIL)
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
    صحیح
ex_let_vars(
    char_u	*arg_start,
    typval_T	*tv,
    صحیح		copy,		// copy values from "tv", don't move
    صحیح		semicolon,	// from skip_var_list()
    صحیح		var_شمار,	// from skip_var_list()
    صحیح		flags,		// ASSIGN_FINAL, ASSIGN_CONST, etc.
    char_u	*op)
{
    char_u	*arg = arg_start;
    list_T	*l;
    tuple_T	*tuple = پوچ;
    صحیح		i;
    صحیح		var_idx = 0;
    listitem_T	*item = پوچ;
    typval_T	ltv;
    صحیح		is_list = tv->v_type == VAR_LIST;
    صحیح		idx;

    اگر (tv->v_type == VAR_VOID)
    {
	emsg(_(e_cannot_use_void_مقدار));
	بازگشت FAIL;
    }
    اگر (*arg != '[')
    {
	// ":let var = expr" or ":for var in list"
	اگر (ex_let_one(arg, tv, copy, flags, op, op, var_idx) == پوچ)
	    بازگشت FAIL;
	بازگشت OK;
    }

    // ":let [v1, v2] = list" or ":for [v1, v2] in listlist"
    // or
    // ":let [v1, v2] = tuple" or ":for [v1, v2] in tupletuple"
    اگر (tv->v_type != VAR_LIST && tv->v_type != VAR_TUPLE)
    {
	emsg(_(e_list_or_tuple_required));
	بازگشت FAIL;
    }
    اگر (is_list)
    {
	l = tv->vval.v_list;
	اگر (l == پوچ)
	{
	    emsg(_(e_list_required));
	    بازگشت FAIL;
	}
	i = list_len(l);
    }
    وگرنه
    {
	tuple = tv->vval.v_tuple;
	اگر (tuple == پوچ)
	{
	    emsg(_(e_tuple_required));
	    بازگشت FAIL;
	}
	i = tuple_len(tuple);
    }

    اگر (semicolon == 0 && var_شمار < i)
    {
	emsg(_(is_list ? e_less_targets_than_list_اقلام
					: e_less_targets_than_tuple_اقلام));
	بازگشت FAIL;
    }
    اگر (var_شمار - semicolon > i)
    {
	emsg(_(is_list ? e_more_targets_than_list_اقلام
					: e_more_targets_than_tuple_اقلام));
	بازگشت FAIL;
    }

    اگر (is_list)
    {
	CHECK_LIST_MATERIALIZE(l);
	item = l->lv_first;
    }
    وگرنه
	idx = 0;

    وقتی (*arg != ']')
    {
	arg = skipwhite(arg + 1);
	++var_idx;
	arg = ex_let_one(arg, is_list ? &item->li_tv : TUPLE_ITEM(tuple, idx),
			 TRUE, flags | ASSIGN_UNPACK, (char_u *)",;]", op,
			 var_idx);
	اگر (is_list)
	    item = item->li_next;
	وگرنه
	    idx++;
	اگر (arg == پوچ)
	    بازگشت FAIL;

	arg = skipwhite(arg);
	اگر (*arg == ';')
	{
	    // Put the rest of the list or tuple (may be empty) in the var
	    // after ';'.  Create a new list or tuple for this.
	    اگر (is_list)
	    {
		// Put the rest of the list (may be empty) in the var
		// after ';'.  Create a new list for this.
		l = list_alloc();
		اگر (l == پوچ)
		    بازگشت FAIL;

		// list
		وقتی (item != پوچ)
		{
		    list_append_tv(l, &item->li_tv);
		    item = item->li_next;
		}

		ltv.v_type = VAR_LIST;
		ltv.v_lock = 0;
		ltv.vval.v_list = l;
		l->lv_refcount = 1;
	    }
	    وگرنه
	    {
		tuple_T	*new_tuple = tuple_alloc();
		اگر (new_tuple == پوچ)
		    بازگشت FAIL;

		// Put the rest of the tuple (may be empty) in the var
		// after ';'.  Create a new tuple for this.
		وقتی (idx < TUPLE_LEN(tuple))
		{
		    typval_T    new_tv;

		    copy_tv(TUPLE_ITEM(tuple, idx), &new_tv);
		    اگر (tuple_append_tv(new_tuple, &new_tv) == FAIL)
		    {
			clear_tv(&new_tv);
			tuple_unref(new_tuple);
			بازگشت FAIL;
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
	    اگر (arg == پوچ)
		بازگشت FAIL;
	    بشکن;
	}
	وگرنه اگر (*arg != ',' && *arg != ']')
	{
	    internal_error("ex_let_vars()");
	    بازگشت FAIL;
	}
    }

    بازگشت OK;
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
    صحیح		include_type,
    صحیح		*var_شمار,
    صحیح		*semicolon,
    صحیح		silent)
{
    char_u	*p, *s;

    اگر (*arg == '[')
    {
	// "[var, var]": find the matching ']'.
	p = arg;
	برای (;;)
	{
	    p = skipwhite(p + 1);	// skip whites after '[', ';' or ','
	    s = skip_var_one(p, include_type);
	    اگر (s == p)
	    {
		اگر (!silent)
		    semsg(_(e_invalid_argument_str), p);
		بازگشت پوچ;
	    }
	    ++*var_شمار;

	    p = skipwhite(s);
	    اگر (*p == ']')
		بشکن;
	    وگرنه اگر (*p == ';')
	    {
		اگر (*semicolon == 1)
		{
		    اگر (!silent)
			emsg(_(e_double_semicolon_in_list_of_variables));
		    بازگشت پوچ;
		}
		*semicolon = 1;
	    }
	    وگرنه اگر (*p != ',')
	    {
		اگر (!silent)
		    semsg(_(e_invalid_argument_str), p);
		بازگشت پوچ;
	    }
	}
	بازگشت p + 1;
    }

    بازگشت skip_var_one(arg, include_type);
}

/*
 * Skip one (assignable) variable name, including @r, $VAR, &option, d.key,
 * l[idx].
 * In Vim9 script also skip over ": type" if "include_type" is TRUE.
 */
    char_u *
skip_var_one(char_u *arg, صحیح include_type)
{
    char_u	*end;
    صحیح		vim9 = in_vim9script();

    اگر (*arg == '@' && arg[1] != NUL)
	بازگشت arg + 2;

    // termcap option name may have non-alpha characters
    اگر (STRNCMP(arg, "&t_", 3) == 0 && arg[3] != NUL && arg[4] != NUL)
	بازگشت arg + 5;

    end = find_name_end(*arg == '$' || *arg == '&' ? arg + 1 : arg,
				   پوچ, پوچ, FNE_INCL_BR | FNE_CHECK_START);

    // "a: type" is declaring variable "a" with a type, not "a:".
    // Same for "s: type".
    اگر (vim9 && end == arg + 2 && end[-1] == ':')
	--end;

    اگر (include_type && vim9)
    {
	اگر (*skipwhite(end) == ':')
	    end = skip_type(skipwhite(skipwhite(end) + 1), FALSE);
    }
    بازگشت end;
}

/*
 * List variables for hashtab "ht" with prefix "prefix".
 * If "empty" is TRUE also list NULL strings as empty strings.
 */
    تهی
list_hashtable_vars(
    hashtab_T	*ht,
    نویسه	*prefix,
    صحیح		empty,
    صحیح		*first)
{
    hashitem_T	*hi;
    dictitem_T	*di;
    صحیح		todo;
    char_u	buf[IOSIZE];

    صحیح save_ht_flags = ht->ht_flags;
    ht->ht_flags |= HTFLAGS_FROZEN;

    todo = (صحیح)ht->ht_used;
    برای (hi = ht->ht_array; todo > 0 && !got_int; ++hi)
    {
	اگر (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    di = HI2DI(hi);

	    // apply :filter /pat/ to variable name
	    vim_strncpy((char_u *)buf, (char_u *)prefix, IOSIZE - 1);
	    vim_strcat((char_u *)buf, di->di_کلید, IOSIZE);
	    اگر (message_filtered(buf))
		ادامه;

	    اگر (empty || di->di_tv.v_type != VAR_STRING
					   || di->di_tv.vval.v_string != پوچ)
		list_one_var(di, prefix, first);
	}
    }

    ht->ht_flags = save_ht_flags;
}

/*
 * List global variables.
 */
    ایستا تهی
list_glob_vars(صحیح *first)
{
    list_hashtable_vars(&globvarht, "", TRUE, first);
}

/*
 * List buffer variables.
 */
    ایستا تهی
list_buf_vars(صحیح *first)
{
    list_hashtable_vars(&curbuf->b_vars->dv_hashtab, "b:", TRUE, first);
}

/*
 * List window variables.
 */
    ایستا تهی
list_win_vars(صحیح *first)
{
    list_hashtable_vars(&curwin->w_vars->dv_hashtab, "w:", TRUE, first);
}

/*
 * List tab page variables.
 */
    ایستا تهی
list_tab_vars(صحیح *first)
{
    list_hashtable_vars(&curtab->tp_vars->dv_hashtab, "t:", TRUE, first);
}

/*
 * List variables in "arg".
 */
    ایستا char_u *
list_arg_vars(exarg_T *eap, char_u *arg, صحیح *first)
{
    صحیح		error = FALSE;
    صحیح		len;
    char_u	*name;
    char_u	*name_start;
    char_u	*arg_subsc;
    char_u	*tofree;
    typval_T    tv;

    وقتی (!ends_excmd2(eap->cmd, arg) && !got_int)
    {
	اگر (error || eap->skip)
	{
	    arg = find_name_end(arg, پوچ, پوچ, FNE_INCL_BR | FNE_CHECK_START);
	    اگر (!VIM_ISWHITE(*arg) && !ends_excmd(*arg))
	    {
		emsg_severe = TRUE;
		اگر (!did_emsg)
		    semsg(_(e_trailing_characters_str), arg);
		بشکن;
	    }
	}
	وگرنه
	{
	    // get_name_len() takes care of expanding curly braces
	    name_start = name = arg;
	    len = get_name_len(&arg, &tofree, TRUE, TRUE);
	    اگر (len <= 0)
	    {
		// This is mainly to keep test 49 working: when expanding
		// curly braces fails overrule the exception error message.
		اگر (len < 0 && !aborting())
		{
		    emsg_severe = TRUE;
		    semsg(_(e_invalid_argument_str), arg);
		    بشکن;
		}
		error = TRUE;
	    }
	    وگرنه
	    {
		arg = skipwhite(arg);
		اگر (tofree != پوچ)
		    name = tofree;
		اگر (eval_variable(name, len, 0, &tv, پوچ,
						     EVAL_VAR_VERBOSE) == FAIL)
		    error = TRUE;
		وگرنه
		{
		    // handle d.key, l[idx], f(expr)
		    arg_subsc = arg;
		    اگر (handle_subscript(&arg, name_start, &tv,
					      &EVALARG_EVALUATE, TRUE) == FAIL)
			error = TRUE;
		    وگرنه
		    {
			اگر (arg == arg_subsc && len == 2 && name[1] == ':')
			{
			    انتخاب (*name)
			    {
				حالت 'g': list_glob_vars(first); بشکن;
				حالت 'b': list_buf_vars(first); بشکن;
				حالت 'w': list_win_vars(first); بشکن;
				حالت 't': list_tab_vars(first); بشکن;
				حالت 'v': list_vim_vars(first); بشکن;
				حالت 's': list_script_vars(first); بشکن;
				حالت 'l': list_func_vars(first); بشکن;
				پیشفرض:
					  semsg(_(e_cant_list_variables_for_str), name);
			    }
			}
			وگرنه
			{
			    char_u	numbuf[NUMBUFLEN];
			    char_u	*tf;
			    صحیح		c;
			    char_u	*s;

			    s = echo_string(&tv, &tf, numbuf, 0);
			    c = *arg;
			    *arg = NUL;
			    list_one_var_a("",
				    arg == arg_subsc ? name : name_start,
				    tv.v_type,
				    s == پوچ ? (char_u *)"" : s,
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

    بازگشت arg;
}

/*
 * Set an environment variable, part of ex_let_one().
 */
    ایستا char_u *
ex_let_env(
    char_u	*arg,
    typval_T	*tv,
    صحیح		flags,
    char_u	*endchars,
    char_u	*op)
{
    char_u	*arg_end = پوچ;
    char_u	*name;
    صحیح		len;

    اگر ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					 && (flags & ASSIGN_FOR_LOOP) == 0)
    {
	emsg(_(e_cannot_lock_environment_variable));
	بازگشت پوچ;
    }

    // Find the end of the name.
    ++arg;
    name = arg;
    len = get_env_len(&arg);
    اگر (len == 0)
	semsg(_(e_invalid_argument_str), name - 1);
    وگرنه
    {
	اگر (op != پوچ && vim_strchr((char_u *)"+-*/%", *op) != پوچ)
	    semsg(_(e_wrong_variable_type_for_str_equal), op);
	وگرنه اگر (endchars != پوچ
			      && vim_strchr(endchars, *skipwhite(arg)) == پوچ)
	    emsg(_(e_unexpected_characters_in_let));
	وگرنه اگر (!check_secure() && !check_restricted())
	{
	    char_u	*tofree = پوچ;
	    صحیح		c1 = name[len];
	    char_u	*p;

	    name[len] = NUL;
	    p = tv_get_string_chk(tv);
	    اگر (p != پوچ && op != پوچ && *op == '.')
	    {
		صحیح	mustfree = FALSE;
		char_u  *s = vim_getenv(name, &mustfree);

		اگر (s != پوچ)
		{
		    p = tofree = concat_str(s, p);
		    اگر (mustfree)
			vim_free(s);
		}
	    }
	    اگر (p != پوچ)
	    {
		vim_setenv_ext(name, p);
		arg_end = arg;
	    }
	    name[len] = c1;
	    vim_free(tofree);
	}
    }
    بازگشت arg_end;
}

/*
 * Set an option, part of ex_let_one().
 */
    ایستا char_u *
ex_let_option(
    char_u	*arg,
    typval_T	*tv,
    صحیح		flags,
    char_u	*endchars,
    char_u	*op)
{
    char_u	*p;
    صحیح		scope;
    char_u	*arg_end = پوچ;

    اگر ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					 && (flags & ASSIGN_FOR_LOOP) == 0)
    {
	emsg(_(e_cannot_lock_option));
	بازگشت پوچ;
    }

    // Find the end of the name.
    p = find_option_end(&arg, &scope);
    اگر (p == پوچ || (endchars != پوچ
			       && vim_strchr(endchars, *skipwhite(p)) == پوچ))
    {
	emsg(_(e_unexpected_characters_in_let));
	بازگشت پوچ;
    }

    صحیح		c1;
    long	n = 0;
    getoption_T	opt_type;
    long	numval;
    char_u	*stringval = پوچ;
    char_u	*s = پوچ;
    صحیح		failed = FALSE;
    صحیح		opt_p_flags;
    char_u	*tofree = پوچ;
    char_u	numbuf[NUMBUFLEN];

    c1 = *p;
    *p = NUL;

    opt_type = get_option_مقدار(arg, &numval, &stringval, &opt_p_flags, scope);
    اگر (opt_type == gov_unknown && arg[0] != 't' && arg[1] != '_')
    {
	semsg(_(e_unknown_option_str_2), arg);
	برو theend;
    }
    اگر (op != پوچ && *op != '='
	&& (((opt_type == gov_bool || opt_type == gov_عدد) && *op == '.')
	    || (opt_type == gov_string && *op != '.')))
    {
	semsg(_(e_wrong_variable_type_for_str_equal), op);
	برو theend;
    }

    اگر ((opt_type == gov_bool
		|| opt_type == gov_عدد
		|| opt_type == gov_hidden_bool
		|| opt_type == gov_hidden_عدد)
		     && (tv->v_type != VAR_STRING || !in_vim9script()))
    {
	اگر (opt_type == gov_bool || opt_type == gov_hidden_bool)
	    // bool, possibly hidden
	    n = (long)tv_get_bool_chk(tv, &failed);
	وگرنه
	    // number, possibly hidden
	    n = (long)tv_get_عدد_chk(tv, &failed);
	اگر (failed)
	    برو theend;
    }

    اگر ((opt_p_flags & P_FUNC) && (tv->v_type == VAR_PARTIAL
						    || tv->v_type == VAR_FUNC))
    {
	// If the option can be set to a function reference or a lambda
	// and the passed value is a function reference, then convert it to
	// the name (string) of the function reference.
	s = tv2string(tv, &tofree, numbuf, 0);
	اگر (s == پوچ)
	    برو theend;
    }
    // Avoid setting a string option to the text "v:false" or similar.
    // In Vim9 script also don't convert a number to string.
    وگرنه اگر (tv->v_type != VAR_BOOL && tv->v_type != VAR_SPECIAL
		     && (!in_vim9script() || tv->v_type != VAR_عدد))
    {
	s = tv_get_string_chk(tv);
	اگر (s == پوچ)
	    برو theend;
    }
    وگرنه اگر (opt_type == gov_string || opt_type == gov_hidden_string)
    {
	emsg(_(e_string_required));
	برو theend;
    }

    اگر (op != پوچ && *op != '=')
    {
	// number, in legacy script also bool
	اگر (opt_type == gov_عدد
				 || (opt_type == gov_bool && !in_vim9script()))
	{
	    انتخاب (*op)
	    {
		حالت '+': n = numval + n; بشکن;
		حالت '-': n = numval - n; بشکن;
		حالت '*': n = numval * n; بشکن;
		حالت '/': n = (long)رقم_divide(numval, n, &failed); بشکن;
		حالت '%': n = (long)رقم_modulus(numval, n, &failed); بشکن;
	    }
	    s = پوچ;
	    اگر (failed)
		برو theend;
	}
	وگرنه اگر (opt_type == gov_string && stringval != پوچ && s != پوچ)
	{
	    // string
	    s = concat_str(stringval, s);
	    vim_free(stringval);
	    stringval = s;
	}
    }

    نویسه *err = set_option_مقدار(arg, n, s, scope);
    arg_end = p;
    اگر (err != پوچ)
	emsg(_(err));

theend:
    *p = c1;
    vim_free(stringval);
    vim_free(tofree);
    بازگشت arg_end;
}

/*
 * Set a register, part of ex_let_one().
 */
    ایستا char_u *
ex_let_register(
    char_u	*arg,
    typval_T	*tv,
    صحیح		flags,
    char_u	*endchars,
    char_u	*op)
{
    char_u	*arg_end = پوچ;

    اگر ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					 && (flags & ASSIGN_FOR_LOOP) == 0)
    {
	emsg(_(e_cannot_lock_register));
	بازگشت پوچ;
    }
    ++arg;
    اگر (op != پوچ && vim_strchr((char_u *)"+-*/%", *op) != پوچ)
	semsg(_(e_wrong_variable_type_for_str_equal), op);
    وگرنه اگر (endchars != پوچ
			  && vim_strchr(endchars, *skipwhite(arg + 1)) == پوچ)
	emsg(_(e_unexpected_characters_in_let));
    وگرنه
    {
	char_u	*ptofree = پوچ;
	char_u	*p;

	p = tv_get_string_chk(tv);
	اگر (p != پوچ && op != پوچ && *op == '.')
	{
	    char_u  *s = get_reg_contents(*arg == '@'
						  ? '"' : *arg, GREG_عبارت_SRC);

	    اگر (s != پوچ)
	    {
		p = ptofree = concat_str(s, p);
		vim_free(s);
	    }
	}
	اگر (p != پوچ)
	{
	    write_reg_contents(*arg == '@' ? '"' : *arg, p, -1, FALSE);
	    arg_end = arg + 1;
	}
	vim_free(ptofree);
    }
    بازگشت arg_end;
}

/*
 * Set one item of ":let var = expr" or ":let [v1, v2] = list" to its value.
 * Returns a pointer to the char just after the var name.
 * Returns NULL if there is an error.
 */
    ایستا char_u *
ex_let_one(
    char_u	*arg,		// points to variable name
    typval_T	*tv,		// value to assign to variable
    صحیح		copy,		// copy value from "tv"
    صحیح		flags,		// ASSIGN_CONST, ASSIGN_FINAL, etc.
    char_u	*endchars,	// valid chars after variable name  or NULL
    char_u	*op,		// "+", "-", "."  or NULL
    صحیح		var_idx)	// variable index for "let [a, b] = list"
{
    char_u	*arg_end = پوچ;

    اگر (in_vim9script() && (flags & (ASSIGN_NO_DECL | ASSIGN_DECL)) == 0
			&& (flags & (ASSIGN_CONST | ASSIGN_FINAL)) == 0
				  && vim_strchr((char_u *)"$@&", *arg) != پوچ)
    {
	vim9_declare_error(arg);
	بازگشت پوچ;
    }

    اگر (check_typval_is_مقدار(tv) == FAIL)
	بازگشت پوچ;

    // In Vim9 script an environment variable and a register only take a
    // String.  "@#" also takes a buffer number.
    اگر (in_vim9script() && (*arg == '$' || *arg == '@')
	    && !(*arg == '@' && arg[1] == '#' && tv->v_type == VAR_عدد
					       && (op == پوچ || *op == '=')))
    {
	where_T	where = WHERE_INIT;

	اگر (check_typval_type(&t_string, tv, where) == FAIL)
	    بازگشت پوچ;
    }

    اگر (*arg == '$')
    {
	// ":let $VAR = expr": Set environment variable.
	بازگشت ex_let_env(arg, tv, flags, endchars, op);
    }
    وگرنه اگر (*arg == '&')
    {
	// ":let &option = expr": Set option value.
	// ":let &l:option = expr": Set local option value.
	// ":let &g:option = expr": Set global option value.
	// ":for &ts in range(8)": Set option value for for loop
	بازگشت ex_let_option(arg, tv, flags, endchars, op);
    }
    وگرنه اگر (*arg == '@')
    {
	// ":let @r = expr": Set register contents.
	بازگشت ex_let_register(arg, tv, flags, endchars, op);
    }
    وگرنه اگر (eval_isnamec1(*arg) || *arg == '{')
    {
	lval_T	lv;
	char_u	*p;
	صحیح	lval_flags = (flags & (ASSIGN_NO_DECL | ASSIGN_DECL))
							     ? GLV_NO_DECL : 0;
	lval_flags |= (flags & ASSIGN_FOR_LOOP) ? GLV_FOR_LOOP : 0;
	اگر (op != پوچ && *op != '=')
	    lval_flags |= GLV_ASSIGN_WITH_OP;

	// ":let var = expr": Set internal variable.
	// ":let var: type = expr": Set internal variable with type.
	// ":let {expr} = expr": Idem, name made with curly braces
	p = get_lval(arg, tv, &lv, FALSE, FALSE, lval_flags, FNE_CHECK_START);
	اگر (p != پوچ && lv.ll_name != پوچ)
	{
	    اگر (endchars != پوچ && vim_strchr(endchars,
					   *skipwhite(lv.ll_name_end)) == پوچ)
	    {
		emsg(_(e_unexpected_characters_in_let));
	    }
	    وگرنه
	    {
		set_var_lval(&lv, p, tv, copy, flags, op, var_idx);
		arg_end = lv.ll_name_end;
	    }
	}
	clear_lval(&lv);
    }
    وگرنه
	semsg(_(e_invalid_argument_str), arg);

    بازگشت arg_end;
}

/*
 * ":unlet[!] var1 ... " command.
 */
    تهی
ex_unlet(exarg_T *eap)
{
    ex_unletlock(eap, eap->arg, 0, eap->forceit ? GLV_QUIET : 0, do_unlet_var, پوچ);
}

/*
 * ":lockvar" and ":unlockvar" commands
 */
    تهی
ex_lockvar(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    صحیح		deep = 2;

    اگر (eap->forceit)
	deep = -1;
    وگرنه اگر (vim_isdigit(*arg))
    {
	deep = getdigits(&arg);
	arg = skipwhite(arg);
    }

    ex_unletlock(eap, arg, deep, 0, do_lock_var, پوچ);
}

/*
 * ":unlet", ":lockvar" and ":unlockvar" are quite similar.
 * Also used for Vim9 script.  "callback" is invoked as:
 *	callback(&lv, name_end, eap, deep, cookie)
 */
    تهی
ex_unletlock(
    exarg_T	*eap,
    char_u	*argstart,
    صحیح		deep,
    صحیح		glv_flags,
    صحیح		(*callback)(lval_T *, char_u *, exarg_T *, صحیح, تهی *),
    تهی	*cookie)
{
    char_u	*arg = argstart;
    char_u	*name_end;
    صحیح		error = FALSE;
    lval_T	lv;

    انجامبده
    {
	اگر (*arg == '$')
	{
	    lv.ll_name = arg;
	    lv.ll_tv = پوچ;
	    ++arg;
	    اگر (get_env_len(&arg) == 0)
	    {
		semsg(_(e_invalid_argument_str), arg - 1);
		بازگشت;
	    }
	    اگر (!error && !eap->skip
			      && callback(&lv, arg, eap, deep, cookie) == FAIL)
		error = TRUE;
	    name_end = arg;
	}
	وگرنه
	{
	    // Parse the name and find the end.
	    name_end = get_lval(arg, پوچ, &lv, TRUE, eap->skip || error,
				     glv_flags | GLV_NO_DECL, FNE_CHECK_START);
	    اگر (lv.ll_name == پوچ)
		error = TRUE;	    // error but continue parsing
	    اگر (name_end == پوچ || (!VIM_ISWHITE(*name_end)
						    && !ends_excmd(*name_end)))
	    {
		اگر (name_end != پوچ)
		{
		    emsg_severe = TRUE;
		    semsg(_(e_trailing_characters_str), name_end);
		}
		اگر (!(eap->skip || error))
		    clear_lval(&lv);
		بشکن;
	    }

	    اگر (!error && !eap->skip
			 && callback(&lv, name_end, eap, deep, cookie) == FAIL)
		error = TRUE;

	    اگر (!eap->skip)
		clear_lval(&lv);
	}

	arg = skipwhite(name_end);
    } وقتی (!ends_excmd2(name_end, arg));

    set_nextcmd(eap, arg);
}

    ایستا صحیح
do_unlet_var(
    lval_T	*lp,
    char_u	*name_end,
    exarg_T	*eap,
    صحیح		deep UNUSED,
    تهی	*cookie UNUSED)
{
    صحیح		forceit = eap->forceit;
    صحیح		ret = OK;
    صحیح		cc;

    اگر (lp->ll_tv == پوچ)
    {
	cc = *name_end;
	*name_end = NUL;

	// Environment variable, normal name or expanded name.
	اگر (*lp->ll_name == '$')
	    vim_unsetenv_ext(lp->ll_name + 1);
	وگرنه اگر (do_unlet(lp->ll_name, forceit) == FAIL)
	    ret = FAIL;
	*name_end = cc;
    }
    وگرنه اگر ((lp->ll_list != پوچ
		 && مقدار_check_lock(lp->ll_list->lv_lock, lp->ll_name, FALSE))
	    || (lp->ll_dict != پوچ
		&& مقدار_check_lock(lp->ll_dict->dv_lock, lp->ll_name, FALSE)))
	بازگشت FAIL;
    وگرنه اگر (lp->ll_range)
	list_unlet_range(lp->ll_list, lp->ll_li, lp->ll_n1,
						    !lp->ll_empty2, lp->ll_n2);
    وگرنه اگر (lp->ll_list != پوچ)
	// unlet a List item.
	listitem_remove(lp->ll_list, lp->ll_li);
    وگرنه اگر (lp->ll_dict != پوچ)
	// unlet a Dictionary item.
	dictitem_remove(lp->ll_dict, lp->ll_di, "unlet");
    وگرنه
    {
	semsg(_(e_cannot_unlet_imported_item_str), lp->ll_name);
	بازگشت FAIL;
    }

    بازگشت ret;
}

/*
 * Unlet one item or a range of items from a list.
 * Return OK or FAIL.
 */
    تهی
list_unlet_range(
	list_T	    *l,
	listitem_T  *li_first,
	long	    n1_arg,
	صحیح	    has_n2,
	long	    n2)
{
    listitem_T  *li = li_first;
    صحیح		n1 = n1_arg;

    // Delete a range of List items.
    li = li_first;
    n1 = n1_arg;
    وقتی (li != پوچ && (!has_n2 || n2 >= n1))
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
    صحیح
do_unlet(char_u *name, صحیح forceit)
{
    hashtab_T	*ht;
    hashitem_T	*hi;
    char_u	*varname = پوچ;  // init to shut up gcc
    dict_T	*d;
    dictitem_T	*di;

    // can't :unlet a script variable in Vim9 script
    اگر (in_vim9script() && check_vim9_unlet(name) == FAIL)
	بازگشت FAIL;

    ht = find_var_ht(name, &varname);

    // can't :unlet a script variable in Vim9 script from a function
    اگر (ht == get_script_local_ht()
	    && SCRIPT_ID_VALID(current_sctx.sc_sid)
	    && SCRIPT_ITEM(current_sctx.sc_sid)->sn_version
							 == SCRIPT_VERSION_VIM9
	    && check_vim9_unlet(name) == FAIL)
	بازگشت FAIL;

    اگر (ht != پوچ && *varname != NUL)
    {
	d = get_current_funccal_dict(ht);
	اگر (d == پوچ)
	{
	    اگر (ht == &globvarht)
		d = &globvardict;
	    وگرنه اگر (ht == &compat_hashtab)
		d = &vimvardict;
	    وگرنه
	    {
		di = find_var_in_ht(ht, *name, (char_u *)"", FALSE);
		d = di == پوچ ? پوچ : di->di_tv.vval.v_dict;
	    }
	    اگر (d == پوچ)
	    {
		internal_error("do_unlet()");
		بازگشت FAIL;
	    }
	}
	hi = hash_find(ht, varname);
	اگر (HASHITEM_EMPTY(hi))
	    hi = find_hi_in_scoped_ht(name, &ht);
	اگر (hi != پوچ && !HASHITEM_EMPTY(hi))
	{
	    di = HI2DI(hi);
	    اگر (var_check_fixed(di->di_flags, name, FALSE)
		    || var_check_ro(di->di_flags, name, FALSE)
		    || مقدار_check_lock(d->dv_lock, name, FALSE)
		    || check_hashtab_frozen(ht, "unlet"))
		بازگشت FAIL;

	    delete_var(ht, hi);
	    بازگشت OK;
	}
    }
    اگر (forceit)
	بازگشت OK;
    semsg(_(e_no_such_variable_str), name);
    بازگشت FAIL;
}

    ایستا تهی
report_lockvar_member(نویسه *پیام, lval_T *lp)
{
    صحیح did_alloc = FALSE;
    char_u *vname = (char_u *)"";
    char_u *class_name = lp->ll_class != پوچ
				    ? lp->ll_class->class_name.string : (char_u *)"";
    اگر (lp->ll_name != پوچ)
    {
	اگر (lp->ll_name_end == پوچ)
	    vname = lp->ll_name;
	وگرنه
	{
	    vname = vim_strnsave(lp->ll_name, lp->ll_name_end - lp->ll_name);
	    اگر (vname == پوچ)
		بازگشت;
	    did_alloc = TRUE;
	}
    }
    semsg(_(پیام), vname, class_name);
    اگر (did_alloc)
	vim_free(vname);
}

/*
 * Lock or unlock variable indicated by "lp".
 * "deep" is the levels to go (-1 for unlimited);
 * "lock" is TRUE for ":lockvar", FALSE for ":unlockvar".
 */
    ایستا صحیح
do_lock_var(
    lval_T	*lp,
    char_u	*name_end,
    exarg_T	*eap,
    صحیح		deep,
    تهی	*cookie UNUSED)
{
    صحیح		lock = eap->cmdidx == CMD_lockvar;
    صحیح		ret = OK;
    صحیح		cc;
    dictitem_T	*di;

#ifdef LOG_LOCKVAR
    ch_log(پوچ, "LKVAR: do_lock_var(): name %s, is_root %d", lp->ll_name, lp->ll_is_ریشه);
#endif

    اگر (lp->ll_tv == پوچ)
    {
	cc = *name_end;
	*name_end = NUL;
	اگر (*lp->ll_name == '$')
	{
	    semsg(_(e_cannot_lock_or_unlock_variable_str), lp->ll_name);
	    ret = FAIL;
	}
	وگرنه
	{
	    // Normal name or expanded name.
	    di = find_var(lp->ll_name, پوچ, TRUE);
	    اگر (di == پوچ)
	    {
		اگر (in_vim9script())
		    semsg(_(e_cannot_find_variable_to_unlock_str),
								  lp->ll_name);
		ret = FAIL;
	    }
	    وگرنه
	    {
		اگر ((di->di_flags & DI_FLAGS_FIX)
			    && di->di_tv.v_type != VAR_DICT
			    && di->di_tv.v_type != VAR_LIST)
		{
		    // For historic reasons this error is not given for a list
		    // or dict.  E.g., the b: dict could be locked/unlocked.
		    semsg(_(e_cannot_lock_or_unlock_variable_str), lp->ll_name);
		    ret = FAIL;
		}
		وگرنه
		{
		    اگر (in_vim9script())
		    {
			svar_T  *sv = find_typval_in_script(&di->di_tv,
								     0, FALSE);

			اگر (sv != پوچ && sv->sv_const != 0)
			{
			    semsg(_(e_cannot_change_readonly_variable_str),
								  lp->ll_name);
			    ret = FAIL;
			}
		    }

		    اگر (ret == OK)
		    {
			اگر (lock)
			    di->di_flags |= DI_FLAGS_LOCK;
			وگرنه
			    di->di_flags &= ~DI_FLAGS_LOCK;
			اگر (deep != 0)
			    item_lock(&di->di_tv, deep, lock, FALSE);
		    }
		}
	    }
	}
	*name_end = cc;
    }
    وگرنه اگر (deep == 0 && lp->ll_object == پوچ && lp->ll_class == پوچ)
    {
	// nothing to do
    }
    وگرنه اگر (lp->ll_is_ریشه)
	// (un)lock the item.
	item_lock(lp->ll_tv, deep, lock, FALSE);
    وگرنه اگر (lp->ll_range)
    {
	listitem_T    *li = lp->ll_li;

	// (un)lock a range of List items.
	وقتی (li != پوچ && (lp->ll_empty2 || lp->ll_n2 >= lp->ll_n1))
	{
	    item_lock(&li->li_tv, deep, lock, FALSE);
	    li = li->li_next;
	    ++lp->ll_n1;
	}
    }
    وگرنه اگر (lp->ll_list != پوچ)
	// (un)lock a List item.
	item_lock(&lp->ll_li->li_tv, deep, lock, FALSE);
    وگرنه اگر (lp->ll_object != پوچ)  // This check must be before ll_class.
    {
	// (un)lock an object variable.
	report_lockvar_member(e_cannot_lock_object_variable_str, lp);
	ret = FAIL;
    }
    وگرنه اگر (lp->ll_class != پوچ)
    {
	// (un)lock a class variable.
	report_lockvar_member(e_cannot_lock_class_variable_str, lp);
	ret = FAIL;
    }
    وگرنه
    {
	// (un)lock a Dictionary item.
	اگر (lp->ll_di == پوچ)
	{
	    emsg(_(e_dictionary_required));
	    ret = FAIL;
	}
	وگرنه
	    item_lock(&lp->ll_di->di_tv, deep, lock, FALSE);
    }

    بازگشت ret;
}

/*
 * Lock or unlock an item.  "deep" is nr of levels to go.
 * When "check_refcount" is TRUE do not lock a list or dict with a reference
 * count larger than 1.
 */
    تهی
item_lock(typval_T *tv, صحیح deep, صحیح lock, صحیح check_refcount)
{
    ایستا صحیح	recurse = 0;
    list_T	*l;
    listitem_T	*li;
    dict_T	*d;
    blob_T	*b;
    hashitem_T	*hi;
    صحیح		todo;

#ifdef LOG_LOCKVAR
    ch_log(پوچ, "LKVAR: item_lock(): type %s", vartype_name(tv->v_type));
#endif

    اگر (recurse >= DICT_MAXNEST)
    {
	emsg(_(e_variable_nested_too_deep_for_unlock));
	بازگشت;
    }
    اگر (deep == 0)
	بازگشت;
    ++recurse;

    // lock/unlock the item itself
    اگر (lock)
	tv->v_lock |= VAR_LOCKED;
    وگرنه
	tv->v_lock &= ~VAR_LOCKED;

    انتخاب (tv->v_type)
    {
	حالت VAR_UNKNOWN:
	حالت VAR_ANY:
	حالت VAR_VOID:
	حالت VAR_عدد:
	حالت VAR_BOOL:
	حالت VAR_STRING:
	حالت VAR_FUNC:
	حالت VAR_PARTIAL:
	حالت VAR_FLOAT:
	حالت VAR_SPECIAL:
	حالت VAR_JOB:
	حالت VAR_CHANNEL:
	حالت VAR_INSTR:
	حالت VAR_CLASS:
	حالت VAR_OBJECT:
	حالت VAR_TYPEALIAS:
	    بشکن;

	حالت VAR_BLOB:
	    اگر ((b = tv->vval.v_blob) != پوچ
				    && !(check_refcount && b->bv_refcount > 1))
	    {
		اگر (lock)
		    b->bv_lock |= VAR_LOCKED;
		وگرنه
		    b->bv_lock &= ~VAR_LOCKED;
	    }
	    بشکن;
	حالت VAR_LIST:
	    اگر ((l = tv->vval.v_list) != پوچ
				    && !(check_refcount && l->lv_refcount > 1))
	    {
		اگر (lock)
		    l->lv_lock |= VAR_LOCKED;
		وگرنه
		    l->lv_lock &= ~VAR_LOCKED;
		اگر (deep < 0 || deep > 1)
		{
		    اگر (l->lv_first == &range_list_item)
			l->lv_lock |= VAR_اقلام_LOCKED;
		    وگرنه
		    {
			// recursive: lock/unlock the items the List contains
			CHECK_LIST_MATERIALIZE(l);
			FOR_ALL_LIST_اقلام(l, li) item_lock(&li->li_tv,
					       deep - 1, lock, check_refcount);
		    }
		}
	    }
	    بشکن;
	حالت VAR_TUPLE:
	    tuple_lock(tv->vval.v_tuple, deep, lock, check_refcount);
	    بشکن;
	حالت VAR_DICT:
	    اگر ((d = tv->vval.v_dict) != پوچ
				    && !(check_refcount && d->dv_refcount > 1))
	    {
		اگر (lock)
		    d->dv_lock |= VAR_LOCKED;
		وگرنه
		    d->dv_lock &= ~VAR_LOCKED;
		اگر (deep < 0 || deep > 1)
		{
		    // recursive: lock/unlock the items the List contains
		    todo = (صحیح)d->dv_hashtab.ht_used;
		    FOR_ALL_HASHTAB_اقلام(&d->dv_hashtab, hi, todo)
		    {
			اگر (!HASHITEM_EMPTY(hi))
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

#اگر defined(FEAT_MENU) && defined(FEAT_MULTI_LANG)
/*
 * Delete all "menutrans_" variables.
 */
    تهی
del_menutrans_vars(تهی)
{
    hashitem_T	*hi;
    صحیح		todo;

    hash_lock(&globvarht);
    todo = (صحیح)globvarht.ht_used;
    برای (hi = globvarht.ht_array; todo > 0 && !got_int; ++hi)
    {
	اگر (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    اگر (STRNCMP(HI2DI(hi)->di_کلید, "menutrans_", 10) == 0)
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

ایستا char_u	*varnamebuf = پوچ;
ایستا صحیح	varnamebuflen = 0;

/*
 * Function to concatenate a prefix and a variable name.
 */
    char_u *
cat_prefix_varname(صحیح prefix, char_u *name)
{
    صحیح		len;

    len = (صحیح)STRLEN(name) + 3;
    اگر (len > varnamebuflen)
    {
	vim_free(varnamebuf);
	len += 10;			// some additional space
	varnamebuf = alloc(len);
	اگر (varnamebuf == پوچ)
	{
	    varnamebuflen = 0;
	    بازگشت پوچ;
	}
	varnamebuflen = len;
    }
    *varnamebuf = prefix;
    varnamebuf[1] = ':';
    STRCPY(varnamebuf + 2, name);
    بازگشت varnamebuf;
}

/*
 * Function given to ExpandGeneric() to obtain the list of user defined
 * (global/buffer/window/built-in) variable names.
 */
    char_u *
get_user_var_name(expand_T *xp, صحیح idx)
{
    ایستا long_u	gdone;
    ایستا long_u	bdone;
    ایستا long_u	wdone;
    ایستا long_u	tdone;
    ایستا صحیح		vidx;
    ایستا hashitem_T	*hi;
    hashtab_T		*ht;

    اگر (idx == 0)
    {
	gdone = bdone = wdone = vidx = 0;
	tdone = 0;
    }

    // Global variables
    اگر (gdone < globvarht.ht_used)
    {
	اگر (gdone++ == 0)
	    hi = globvarht.ht_array;
	وگرنه
	    ++hi;
	وقتی (HASHITEM_EMPTY(hi))
	    ++hi;
	اگر (STRNCMP("g:", xp->xp_pattern, 2) == 0)
	    بازگشت cat_prefix_varname('g', hi->hi_کلید);
	بازگشت hi->hi_کلید;
    }

    // b: variables
    ht = &prevwin_curwin()->w_buffer->b_vars->dv_hashtab;
    اگر (bdone < ht->ht_used)
    {
	اگر (bdone++ == 0)
	    hi = ht->ht_array;
	وگرنه
	    ++hi;
	وقتی (HASHITEM_EMPTY(hi))
	    ++hi;
	بازگشت cat_prefix_varname('b', hi->hi_کلید);
    }

    // w: variables
    ht = &prevwin_curwin()->w_vars->dv_hashtab;
    اگر (wdone < ht->ht_used)
    {
	اگر (wdone++ == 0)
	    hi = ht->ht_array;
	وگرنه
	    ++hi;
	وقتی (HASHITEM_EMPTY(hi))
	    ++hi;
	بازگشت cat_prefix_varname('w', hi->hi_کلید);
    }

    // t: variables
    ht = &curtab->tp_vars->dv_hashtab;
    اگر (tdone < ht->ht_used)
    {
	اگر (tdone++ == 0)
	    hi = ht->ht_array;
	وگرنه
	    ++hi;
	وقتی (HASHITEM_EMPTY(hi))
	    ++hi;
	بازگشت cat_prefix_varname('t', hi->hi_کلید);
    }

    // v: variables
    اگر (vidx < VV_LEN)
	بازگشت cat_prefix_varname('v', (char_u *)vimvars[vidx++].vv_name);

    VIM_CLEAR(varnamebuf);
    varnamebuflen = 0;
    بازگشت پوچ;
}

    نویسه *
get_var_special_name(صحیح nr)
{
    انتخاب (nr)
    {
	حالت VVAL_FALSE: بازگشت in_vim9script() ? "false" : "v:false";
	حالت VVAL_TRUE:  بازگشت in_vim9script() ? "true" : "v:true";
	حالت VVAL_NULL:  بازگشت in_vim9script() ? "null" : "v:null";
	حالت VVAL_NONE:  بازگشت "v:none";
    }
    internal_error("get_var_special_name()");
    بازگشت "42";
}

/*
 * Returns the global variable dictionary
 */
    dict_T *
get_globvar_dict(تهی)
{
    بازگشت &globvardict;
}

/*
 * Returns the global variable hash table
 */
    hashtab_T *
get_globvar_ht(تهی)
{
    بازگشت &globvarht;
}

/*
 * Returns the v: variable dictionary
 */
    dict_T *
get_vimvar_dict(تهی)
{
    بازگشت &vimvardict;
}

/*
 * Returns the index of a v:variable.  Negative if not found.
 * Returns DI_ flags in "di_flags".
 */
    صحیح
find_vim_var(char_u *name, صحیح *di_flags)
{
    dictitem_T	    *di = find_var_in_ht(&vimvarht, 0, name, TRUE);
    ساختار vimvar   *vv;

    اگر (di == پوچ)
	بازگشت -1;
    *di_flags = di->di_flags;
    vv = (ساختار vimvar *)((نویسه *)di - offsetof(vimvar_T, vv_di));
    بازگشت (صحیح)(vv - vimvars);
}


/*
 * Set type of v: variable to "type".
 */
    تهی
set_vim_var_type(صحیح idx, vartype_T type)
{
    vimvars[idx].vv_tv_type = type;
}

/*
 * Set number v: variable to "val".
 * Note that this does not set the type, use set_vim_var_type() for that.
 */
    تهی
set_vim_var_nr(صحیح idx, varnumber_T val)
{
    vimvars[idx].vv_nr = val;
}

    نویسه *
get_vim_var_name(صحیح idx)
{
    بازگشت vimvars[idx].vv_name;
}

/*
 * Get typval_T v: variable value.
 */
    typval_T *
get_vim_var_tv(صحیح idx)
{
    بازگشت &vimvars[idx].vv_tv;
}

    type_T *
get_vim_var_type(صحیح idx, garray_T *type_list)
{
    اگر (vimvars[idx].vv_type != پوچ)
	بازگشت vimvars[idx].vv_type;
    بازگشت typval2type_vimvar(&vimvars[idx].vv_tv, type_list);
}

/*
 * Add what getinfo() reports for the v: variable "name", without the "v:", to
 * "d".  Returns FAIL when there is no such variable.
 */
    صحیح
vim_var_info(char_u *name, dict_T *d)
{
    صحیح		di_flags;
    صحیح		idx = find_vim_var(name, &di_flags);
    garray_T	type_list;
    نویسه	*tofree;

    اگر (idx < 0)
	بازگشت FAIL;
    vim_snprintf((نویسه *)IObuff, IOSIZE, "v:%s", vimvars[idx].vv_name);
    dict_add_string(d, "name", IObuff);
    dict_add_bool(d, "available", TRUE);
    ga_init2(&type_list, اندازه(type_T *), 10);
    نویسه *type = type_name(get_vim_var_type(idx, &type_list), &tofree);
    dict_add_string(d, "type", (char_u *)type);
    vim_free(tofree);
    clear_type_list(&type_list);
    dict_add_bool(d, "readonly", (vimvars[idx].vv_flags & VV_RO) != 0);
    dict_add_bool(d, "compat", (vimvars[idx].vv_flags & VV_COMPAT) != 0);
    بازگشت OK;
}

/*
 * Set v: variable to "tv".  Only accepts the same type.
 * Takes over the value of "tv".
 */
    صحیح
set_vim_var_tv(صحیح idx, typval_T *tv)
{
    اگر (vimvars[idx].vv_tv_type != tv->v_type)
    {
	emsg(_(e_type_mismatch_for_v_variable));
	clear_tv(tv);
	بازگشت FAIL;
    }
    // VV_RO is also checked when compiling, but let's check here as well.
    اگر (vimvars[idx].vv_flags & VV_RO)
    {
	semsg(_(e_cannot_change_readonly_variable_str), vimvars[idx].vv_name);
	بازگشت FAIL;
    }
    اگر (sandbox && (vimvars[idx].vv_flags & VV_RO_SBX))
    {
	semsg(_(e_cannot_set_variable_in_sandbox_str), vimvars[idx].vv_name);
	بازگشت FAIL;
    }
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_di.di_tv = *tv;
    بازگشت OK;
}

/*
 * Get number v: variable value.
 */
    varnumber_T
get_vim_var_nr(صحیح idx)
{
    بازگشت vimvars[idx].vv_nr;
}

/*
 * Get string v: variable value.  Uses a static buffer, can only be used once.
 * If the String variable has never been set, return an empty string.
 * Never returns NULL;
 */
    char_u *
get_vim_var_str(صحیح idx)
{
    بازگشت tv_get_string(&vimvars[idx].vv_tv);
}

/*
 * Get List v: variable value.  Caller must take care of reference count when
 * needed.
 */
    list_T *
get_vim_var_list(صحیح idx)
{
    بازگشت vimvars[idx].vv_list;
}

/*
 * Get Dict v: variable value.  Caller must take care of reference count when
 * needed.
 */
    dict_T *
get_vim_var_dict(صحیح idx)
{
    بازگشت vimvars[idx].vv_dict;
}

/*
 * Set v:char to character "c".
 */
    تهی
set_vim_var_char(صحیح c)
{
    char_u  buf[MB_MAXBYTES + 1];
    size_t  buflen;

    اگر (has_mbyte)
	buflen = (*mb_char2bytes)(c, buf);
    وگرنه
    {
	buf[0] = c;
	buflen = 1;
    }
    buf[buflen] = NUL;

    set_vim_var_string(VV_CHAR, buf, (صحیح)buflen);
}

/*
 * Set v:count to "count" and v:count1 to "count1".
 * When "set_prevcount" is TRUE first set v:prevcount from v:count.
 */
    تهی
set_vcount(
    long	شمار,
    long	count1,
    صحیح		set_prevcount)
{
    اگر (set_prevcount)
	vimvars[VV_PREVCOUNT].vv_nr = vimvars[VV_شمار].vv_nr;
    vimvars[VV_شمار].vv_nr = شمار;
    vimvars[VV_COUNT1].vv_nr = count1;
}

/*
 * Save variables that might be changed as a side effect.  Used when executing
 * a timer callback.
 */
    تهی
save_vimvars(vimvars_save_T *vvsave)
{
    vvsave->vv_prevcount = vimvars[VV_PREVCOUNT].vv_nr;
    vvsave->vv_شمار = vimvars[VV_شمار].vv_nr;
    vvsave->vv_count1 = vimvars[VV_COUNT1].vv_nr;
}

/*
 * Restore variables saved by save_vimvars().
 */
    تهی
restore_vimvars(vimvars_save_T *vvsave)
{
    vimvars[VV_PREVCOUNT].vv_nr = vvsave->vv_prevcount;
    vimvars[VV_شمار].vv_nr = vvsave->vv_شمار;
    vimvars[VV_COUNT1].vv_nr = vvsave->vv_count1;
}

/*
 * Set string v: variable to a copy of "val". If 'copy' is FALSE, then set the
 * value.
 */
    تهی
set_vim_var_string(
    صحیح		idx,
    char_u	*val,
    صحیح		len)	    // length of "val" to use or -1 (whole string)
{
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_tv_type = VAR_STRING;
    اگر (val == پوچ)
	vimvars[idx].vv_str = پوچ;
    وگرنه اگر (len == -1)
	vimvars[idx].vv_str = vim_strsave(val);
    وگرنه
	vimvars[idx].vv_str = vim_strnsave(val, len);
}

    تهی
set_vim_var_string_direct(
    صحیح		idx,
    char_u	*val)
{
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_tv_type = VAR_STRING;

    vimvars[idx].vv_str = val;
}

/*
 * Set List v: variable to "val".
 */
    تهی
set_vim_var_list(صحیح idx, list_T *val)
{
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_tv_type = VAR_LIST;
    vimvars[idx].vv_list = val;
    اگر (val != پوچ)
	++val->lv_refcount;
}

/*
 * Set Dictionary v: variable to "val".
 */
    تهی
set_vim_var_dict(صحیح idx, dict_T *val)
{
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_tv_type = VAR_DICT;
    vimvars[idx].vv_dict = val;
    اگر (val == پوچ)
	بازگشت;

    ++val->dv_refcount;
    dict_set_اقلام_ro(val);
}

/*
 * Set the v:argv list.
 */
    تهی
set_argv_var(نویسه **argv, صحیح argc)
{
    list_T	*l = list_alloc();
    صحیح		i;

    اگر (l == پوچ)
	getout(1);
    l->lv_lock = VAR_FIXED;
    برای (i = 0; i < argc; ++i)
    {
	اگر (list_append_string(l, (char_u *)argv[i], -1) == FAIL)
	    getout(1);
	l->lv_u.mat.lv_last->li_tv.v_lock = VAR_FIXED;
    }
    set_vim_var_list(VV_ARGV, l);
}

/*
 * Reset v:register, taking the 'clipboard' setting into account.
 */
    تهی
reset_reg_var(تهی)
{
    صحیح regname = 0;

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
    تهی
set_reg_var(صحیح c)
{
    char_u  regname[2];

    اگر (c == 0 || c == ' ')
	regname[0] = '"';
    وگرنه
	regname[0] = c;
    regname[1] = NUL;
    // Avoid free/alloc when the value is already right.
    اگر (vimvars[VV_REG].vv_str == پوچ || vimvars[VV_REG].vv_str[0] != c)
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
    اگر (oldval == پوچ)
	بازگشت vimvars[VV_EXCEPTION].vv_str;

    vimvars[VV_EXCEPTION].vv_str = oldval;
    بازگشت پوچ;
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
    اگر (oldval == پوچ)
	بازگشت vimvars[VV_THROWPOINT].vv_str;

    vimvars[VV_THROWPOINT].vv_str = oldval;
    بازگشت پوچ;
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
    اگر (eap == پوچ)
    {
	vim_free(oldval);
	vimvars[VV_CMDARG].vv_str = oldarg;
	بازگشت پوچ;
    }

    اگر (eap->force_bin == FORCE_BIN)
	len = 6;
    وگرنه اگر (eap->force_bin == FORCE_NOBIN)
	len = 8;
    وگرنه
	len = 0;

    اگر (eap->read_edit)
	len += 7;

    اگر (eap->force_ff != 0)
	len += 10; // " ++ff=unix"
    اگر (eap->force_enc != 0)
	len += (unsigned)STRLEN(eap->cmd + eap->force_enc) + 7;
    اگر (eap->bad_char != 0)
	len += 7 + 4;  // " ++bad=" + "keep" or "drop"

    newval = alloc(len + 1);
    اگر (newval == پوچ)
	بازگشت پوچ;

    اگر (eap->force_bin == FORCE_BIN)
	sprintf((نویسه *)newval, " ++bin");
    وگرنه اگر (eap->force_bin == FORCE_NOBIN)
	sprintf((نویسه *)newval, " ++nobin");
    وگرنه
	*newval = NUL;

    اگر (eap->read_edit)
	STRCAT(newval, " ++edit");

    اگر (eap->force_ff != 0)
	sprintf((نویسه *)newval + STRLEN(newval), " ++ff=%s",
						eap->force_ff == 'u' ? "unix"
						: eap->force_ff == 'd' ? "dos"
						: "mac");
    اگر (eap->force_enc != 0)
	sprintf((نویسه *)newval + STRLEN(newval), " ++enc=%s",
					       eap->cmd + eap->force_enc);
    اگر (eap->bad_char == BAD_KEEP)
	STRCPY(newval + STRLEN(newval), " ++bad=keep");
    وگرنه اگر (eap->bad_char == BAD_DROP)
	STRCPY(newval + STRLEN(newval), " ++bad=drop");
    وگرنه اگر (eap->bad_char != 0)
	sprintf((نویسه *)newval + STRLEN(newval), " ++bad=%c", eap->bad_char);
    vimvars[VV_CMDARG].vv_str = newval;
    بازگشت oldval;
}

/*
 * Get the value of internal variable "name".
 * If "flags" has EVAL_VAR_IMPORT may return a VAR_ANY with v_number set to the
 * imported script ID.
 * Return OK or FAIL.  If OK is returned "rettv" must be cleared.
 */
    صحیح
eval_variable(
    char_u	*name,
    صحیح		len,		// length of "name" or zero
    scid_T	sid,		// script ID for imported item or zero
    typval_T	*rettv,		// NULL when only checking existence
    dictitem_T	**dip,		// non-NULL when typval's dict item is needed
    صحیح		flags)		// EVAL_VAR_ flags
{
    صحیح		ret = OK;
    typval_T	*tv = پوچ;
    صحیح		found = FALSE;
    hashtab_T	*ht = پوچ;
    صحیح		cc = 0;
    type_T	*type = پوچ;

    اگر (len > 0)
    {
	// truncate the name, so that we can use strcmp()
	cc = name[len];
	name[len] = NUL;
    }

    // Check for local variable when debugging.
    اگر ((sid == 0) && (tv = lookup_debug_var(name)) == پوچ)
    {
	// Check for user-defined variables.
	dictitem_T	*v = find_var(name, &ht, flags & EVAL_VAR_NOAUTOLOAD);

	اگر (v != پوچ)
	{
	    tv = &v->di_tv;
	    اگر (dip != پوچ)
		*dip = v;
	}
	وگرنه
	    ht = پوچ;
    }

    اگر (tv == پوچ && (in_vim9script() || STRNCMP(name, "s:", 2) == 0))
    {
	imported_T  *import = پوچ;
	char_u	    *p = STRNCMP(name, "s:", 2) == 0 ? name + 2 : name;

	اگر (sid == 0)
	    import = find_imported(p, 0, TRUE);

	// imported variable from another script
	اگر (import != پوچ || sid != 0)
	{
	    اگر ((flags & EVAL_VAR_IMPORT) == 0)
	    {
		اگر (SCRIPT_ID_VALID(sid))
		{
		    ht = &SCRIPT_VARS(sid);
		    اگر (ht != پوچ)
		    {
			dictitem_T *v = find_var_in_ht(ht, 0, name,
						  flags & EVAL_VAR_NOAUTOLOAD);

			اگر (v == پوچ)
			    v = find_var_autoload_prefix(name, sid, پوچ, پوچ);

			اگر (v != پوچ)
			{
			    tv = &v->di_tv;
			    اگر (dip != پوچ)
				*dip = v;
			}
			وگرنه
			    ht = پوچ;
		    }
		}
		وگرنه
		{
		    اگر (flags & EVAL_VAR_VERBOSE)
			semsg(_(e_expected_dot_after_name_str), name);
		    ret = FAIL;
		}
	    }
	    وگرنه
	    {
		اگر (rettv != پوچ)
		{
		    // special value that is used in handle_subscript()
		    rettv->v_type = VAR_ANY;
		    rettv->vval.v_عدد = sid != 0 ? sid : import->imp_sid;
		}
		found = TRUE;
	    }
	}
	وگرنه اگر (in_vim9script() && (flags & EVAL_VAR_NO_FUNC) == 0)
	{
	    صحیح	    has_g_prefix = STRNCMP(name, "g:", 2) == 0;
	    ufunc_T *ufunc = find_func(name, FALSE);

	    اگر (ufunc != پوچ && cc == '<')
	    {
		// handle generic function
		char_u	*argp = name + len;
		name[len] = cc;
		ufunc = eval_generic_func(ufunc, name, &argp);
		name[len] = NUL;
		اگر (ufunc == پوچ)
		{
		    ret = FAIL;
		    برو done;
		}
	    }

	    // In Vim9 script we can get a function reference by using the
	    // function name.  For a global non-autoload function "g:" is
	    // required.
	    اگر (ufunc != پوچ && (has_g_prefix
					    || !func_requires_g_prefix(ufunc)))
	    {
		found = TRUE;
		اگر (rettv != پوچ)
		{
		    rettv->v_type = VAR_FUNC;
		    اگر (has_g_prefix)
		    {
			// Keep the "g:", otherwise script-local may be
			// assumed.
			اگر (cc != '<')
			    rettv->vval.v_string = vim_strsave(name);
			وگرنه
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
		    وگرنه
			rettv->vval.v_string = vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
		    اگر (rettv->vval.v_string != پوچ)
			func_ref(ufunc->uf_name);
		}
	    }
	}
    }

    اگر (!found)
    {
	اگر (tv == پوچ)
	{
	    اگر (rettv != پوچ && (flags & EVAL_VAR_VERBOSE))
		semsg(_(e_undefined_variable_str), name);
	    ret = FAIL;
	}
	وگرنه اگر (rettv != پوچ)
	{
	    svar_T  *sv = پوچ;
	    صحیح	    was_assigned = FALSE;

	    اگر (ht != پوچ && ht == get_script_local_ht()
		    && tv != &SCRIPT_SV(current_sctx.sc_sid)->sv_var.di_tv)
	    {
		sv = find_typval_in_script(tv, 0, TRUE);
		اگر (sv != پوچ)
		{
		    type = sv->sv_type;
		    was_assigned = sv->sv_flags & SVFLAG_ASSIGNED;
		}
	    }

	    اگر ((tv->v_type == VAR_TYPEALIAS || tv->v_type == VAR_CLASS)
		    && sid != 0)
	    {
		// type alias or class imported from another script.  Check
		// whether it is exported from the other script.
		sv = find_typval_in_script(tv, sid, TRUE);
		اگر (sv == پوچ)
		{
		    ret = FAIL;
		    برو done;
		}
		اگر ((sv->sv_flags & SVFLAG_EXPORTED) == 0)
		{
		    semsg(_(e_item_not_exported_in_script_str), name);
		    ret = FAIL;
		    برو done;
		}
	    }

	    // If a list or tuple or dict variable wasn't initialized and has
	    // meaningful type, do it now.  Not for global variables, they are
	    // not declared.
	    اگر (ht != &globvarht)
	    {
		اگر (tv->v_type == VAR_DICT && tv->vval.v_dict == پوچ
					    && ((type != پوچ && !was_assigned)
							  || !in_vim9script()))
		{
		    tv->vval.v_dict = dict_alloc();
		    اگر (tv->vval.v_dict != پوچ)
		    {
			++tv->vval.v_dict->dv_refcount;
			tv->vval.v_dict->dv_type = alloc_type(type);
			اگر (sv != پوچ)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
		وگرنه اگر (tv->v_type == VAR_LIST && tv->vval.v_list == پوچ
					    && ((type != پوچ && !was_assigned)
							  || !in_vim9script()))
		{
		    tv->vval.v_list = list_alloc();
		    اگر (tv->vval.v_list != پوچ)
		    {
			++tv->vval.v_list->lv_refcount;
			tv->vval.v_list->lv_type = alloc_type(type);
			اگر (sv != پوچ)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
		وگرنه اگر (tv->v_type == VAR_TUPLE && tv->vval.v_tuple == پوچ
					    && ((type != پوچ && !was_assigned)
							  || !in_vim9script()))
		{
		    tv->vval.v_tuple = tuple_alloc();
		    اگر (tv->vval.v_tuple != پوچ)
		    {
			++tv->vval.v_tuple->tv_refcount;
			tv->vval.v_tuple->tv_type = alloc_type(type);
			اگر (sv != پوچ)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
		وگرنه اگر (tv->v_type == VAR_BLOB && tv->vval.v_blob == پوچ
					    && ((type != پوچ && !was_assigned)
							  || !in_vim9script()))
		{
		    tv->vval.v_blob = blob_alloc();
		    اگر (tv->vval.v_blob != پوچ)
		    {
			++tv->vval.v_blob->bv_refcount;
			اگر (sv != پوچ)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
	    }
	    ret = copy_tv(tv, rettv);
	}
    }

done:
    اگر (len > 0)
	name[len] = cc;

    بازگشت ret;
}

/*
 * Get the value of internal variable "name", also handling "import.name".
 * Return OK or FAIL.  If OK is returned "rettv" must be cleared.
 */
    صحیح
eval_variable_import(
    char_u	*name,
    typval_T	*rettv)
{
    char_u  *s = name;
    وقتی (ASCII_ISALNUM(*s) || *s == '_')
	++s;
    صحیح	    len = (صحیح)(s - name);

    اگر (eval_variable(name, len, 0, rettv, پوچ, EVAL_VAR_IMPORT) == FAIL)
	بازگشت FAIL;
    اگر (rettv->v_type == VAR_ANY && *s == '.')
    {
	char_u *ns = s + 1;
	s = ns;
	وقتی (ASCII_ISALNUM(*s) || *s == '_')
	    ++s;
	صحیح sid = rettv->vval.v_عدد;
	بازگشت eval_variable(ns, (صحیح)(s - ns), sid, rettv, پوچ, 0);
    }
    بازگشت OK;
}


/*
 * Check if variable "name[len]" is a local variable or an argument.
 * If so, "*eval_lavars_used" is set to TRUE.
 */
    تهی
check_vars(char_u *name, صحیح len)
{
    صحیح		cc;
    char_u	*varname;
    hashtab_T	*ht;

    اگر (eval_lavars_used == پوچ)
	بازگشت;

    // truncate the name, so that we can use strcmp()
    cc = name[len];
    name[len] = NUL;

    ht = find_var_ht(name, &varname);
    اگر (ht == get_funccal_local_ht() || ht == get_funccal_args_ht())
    {
	اگر (find_var(name, پوچ, TRUE) != پوچ)
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
find_var(char_u *name, hashtab_T **htp, صحیح no_autoload)
{
    char_u	*varname;
    hashtab_T	*ht;
    dictitem_T	*ret = پوچ;

    ht = find_var_ht(name, &varname);
    اگر (htp != پوچ)
	*htp = ht;
    اگر (ht == پوچ)
	بازگشت پوچ;
    ret = find_var_in_ht(ht, *name, varname, no_autoload);
    اگر (ret != پوچ)
	بازگشت ret;

    // Search in parent scope for lambda
    ret = find_var_in_scoped_ht(name, no_autoload);
    اگر (ret != پوچ)
	بازگشت ret;

    // in Vim9 script items without a scope can be script-local
    اگر (in_vim9script() && name[0] != NUL && name[1] != ':')
    {
	ht = get_script_local_ht();
	اگر (ht != پوچ)
	{
	    ret = find_var_in_ht(ht, *name, varname, no_autoload);
	    اگر (ret != پوچ)
	    {
		اگر (htp != پوچ)
		    *htp = ht;
		بازگشت ret;
	    }
	}
    }

    // and finally try
    بازگشت find_var_autoload_prefix(name, 0, htp, پوچ);
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
find_var_autoload_prefix(char_u *name, صحیح sid, hashtab_T **htp,
							    char_u **namep)
{
    hashtab_T	*ht;
    dictitem_T	*ret = پوچ;
    // When using "vim9script autoload" script-local items are prefixed but can
    // be used with s:name.
    صحیح check_sid = sid > 0 ? sid : current_sctx.sc_sid;
    اگر (SCRIPT_ID_VALID(check_sid)
		   && (in_vim9script() || (name[0] == 's' && name[1] == ':')))
    {
	scriptitem_T *si = SCRIPT_ITEM(check_sid);

	اگر (si->sn_autoload_prefix != پوچ)
	{
	    char_u *base_name = (name[0] == 's' && name[1] == ':')
							     ? name + 2 : name;
	    char_u *auto_name = concat_str(si->sn_autoload_prefix, base_name);

	    اگر (auto_name != پوچ)
	    {
		صحیح free_auto_name = TRUE;
		ht = &globvarht;
		ret = find_var_in_ht(ht, 'g', auto_name, TRUE);
		اگر (ret != پوچ)
		{
		    اگر (htp != پوچ)
			*htp = ht;
		    اگر (namep != پوچ)
		    {
			free_auto_name = FALSE;
			*namep = auto_name;
		    }
		}
		اگر (free_auto_name)
		    vim_free(auto_name);
	    }
	}
    }

    بازگشت ret;
}

/*
 * Like find_var() but if the name starts with <SNR>99_ then look in the
 * referenced script (used for a funcref).
 */
    dictitem_T *
find_var_also_in_script(char_u *name, hashtab_T **htp, صحیح no_autoload)
{
    اگر (STRNCMP(name, "<SNR>", 5) == 0 && SAFE_isdigit(name[5]))
    {
	char_u	    *p = name + 5;
	صحیح	    sid = getdigits(&p);

	اگر (SCRIPT_ID_VALID(sid) && *p == '_')
	{
	    hashtab_T	*ht = &SCRIPT_VARS(sid);

	    اگر (ht != پوچ)
	    {
		dictitem_T *di = find_var_in_ht(ht, 0, p + 1, no_autoload);

		اگر (di != پوچ)
		{
		    اگر (htp != پوچ)
			*htp = ht;
		    بازگشت di;
		}
	    }
	}
    }

    بازگشت find_var(name, htp, no_autoload);
}

/*
 * Find variable "varname" in hashtab "ht" with name "htname".
 * When "varname" is empty returns curwin/curtab/etc vars dictionary.
 * Returns NULL if not found.
 */
    dictitem_T *
find_var_in_ht(
    hashtab_T	*ht,
    صحیح		htname,
    char_u	*varname,
    صحیح		no_autoload)
{
    hashitem_T	*hi;

    اگر (*varname == NUL)
    {
	// Must be something like "s:", otherwise "ht" would be NULL.
	انتخاب (htname)
	{
	    حالت 's': بازگشت &SCRIPT_SV(current_sctx.sc_sid)->sv_var;
	    حالت 'g': بازگشت &globvars_var;
	    حالت 'v': بازگشت &vimvars_var;
	    حالت 'b': بازگشت &curbuf->b_bufvar;
	    حالت 'w': بازگشت &curwin->w_winvar;
	    حالت 't': بازگشت &curtab->tp_winvar;
	    حالت 'l': بازگشت get_funccal_local_var();
	    حالت 'a': بازگشت get_funccal_args_var();
	}
	بازگشت پوچ;
    }

    hi = hash_find(ht, varname);
    اگر (HASHITEM_EMPTY(hi))
    {
	// For global variables we may try auto-loading the script.  If it
	// worked find the variable again.  Don't auto-load a script if it was
	// loaded already, otherwise it would be loaded every time when
	// checking if a function name is a Funcref variable.
	اگر (ht == &globvarht && !no_autoload)
	{
	    // Note: script_autoload() may make "hi" invalid. It must either
	    // be obtained again or not used.
	    اگر (!script_autoload(varname, FALSE) || aborting())
		بازگشت پوچ;
	    hi = hash_find(ht, varname);
	}
	اگر (HASHITEM_EMPTY(hi))
	    بازگشت پوچ;
    }
    بازگشت HI2DI(hi);
}

/*
 * Get the script-local hashtab.  NULL if not in a script context.
 */
    hashtab_T *
get_script_local_ht(تهی)
{
    scid_T sid = current_sctx.sc_sid;

    اگر (SCRIPT_ID_VALID(sid))
	بازگشت &SCRIPT_VARS(sid);
    بازگشت پوچ;
}

/*
 * Look for "name[len]" in script-local variables and functions.
 * When "cmd" is TRUE it must look like a command, a function must be followed
 * by "(" or "->".
 * Return OK when found, FAIL when not found.
 */
    صحیح
lookup_scriptitem(
	char_u	*name,
	size_t	len,
	صحیح	cmd,
	cctx_T	*dummy UNUSED)
{
    hashtab_T	*ht = get_script_local_ht();
    char_u	buffer[30];
    char_u	*p;
    صحیح		res;
    hashitem_T	*hi;
    صحیح		is_global = FALSE;
    char_u	*fname = name;

    اگر (ht == پوچ)
	بازگشت FAIL;
    اگر (len < اندازه(buffer) - 1)
    {
	// avoid an alloc/free for short names
	vim_strncpy(buffer, name, len);
	p = buffer;
    }
    وگرنه
    {
	p = vim_strnsave(name, len);
	اگر (p == پوچ)
	    بازگشت FAIL;
    }

    hi = hash_find(ht, p);
    res = HASHITEM_EMPTY(hi) ? FAIL : OK;

    // if not script-local, then perhaps autoload-exported
    اگر (res == FAIL && find_var_autoload_prefix(p, 0, پوچ, پوچ) != پوچ)
	res = OK;

    // if not script-local or autoload, then perhaps imported
    اگر (res == FAIL && find_imported(p, 0, FALSE) != پوچ)
	res = OK;
    اگر (p != buffer)
	vim_free(p);

    // Find a function, so that a following "->" works.
    // When used as a command require "(" or "->" to follow, "Cmd" is a user
    // command while "Cmd()" is a function call.
    اگر (res != OK)
    {
	p = skipwhite(name + len);

	اگر (!cmd || name[len] == '(' || (p[0] == '-' && p[1] == '>'))
	{
	    // Do not check for an internal function, since it might also be a
	    // valid command, such as ":split" versus "split()".
	    // Skip "g:" before a function name.
	    اگر (name[0] == 'g' && name[1] == ':')
	    {
		is_global = TRUE;
		fname = name + 2;
	    }
	    اگر (find_func(fname, is_global) != پوچ)
		res = OK;
	}
    }

    بازگشت res;
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

    اگر (name[0] == NUL)
	بازگشت پوچ;
    اگر (name[1] != ':')
    {
	// The name must not start with a colon or #.
	اگر (name[0] == ':' || name[0] == AUTOLOAD_CHAR)
	    بازگشت پوچ;
	*varname = name;

	// "version" is "v:version" in all scopes if scriptversion < 3.
	// Same for a few other variables marked with VV_COMPAT.
	اگر (in_old_script(3))
	{
	    hi = hash_find(&compat_hashtab, name);
	    اگر (!HASHITEM_EMPTY(hi))
		بازگشت &compat_hashtab;
	}

	ht = get_funccal_local_ht();
	اگر (ht != پوچ)
	    بازگشت ht;				// local variable

	// In Vim9 script items at the script level are script-local, except
	// for autoload names.
	اگر (in_vim9script() && vim_strchr(name, AUTOLOAD_CHAR) == پوچ)
	{
	    ht = get_script_local_ht();
	    اگر (ht != پوچ)
		بازگشت ht;
	}

	بازگشت &globvarht;			// global variable
    }
    *varname = name + 2;
    اگر (*name == 'g')				// global variable
	بازگشت &globvarht;
    // There must be no ':' or '#' in the rest of the name, unless g: is used
    اگر (vim_strchr(name + 2, ':') != پوچ
			       || vim_strchr(name + 2, AUTOLOAD_CHAR) != پوچ)
	بازگشت پوچ;
    اگر (*name == 'b')				// buffer variable
	بازگشت &curbuf->b_vars->dv_hashtab;
    اگر (*name == 'w')				// window variable
	بازگشت &curwin->w_vars->dv_hashtab;
    اگر (*name == 't')				// tab page variable
	بازگشت &curtab->tp_vars->dv_hashtab;
    اگر (*name == 'v')				// v: variable
	بازگشت &vimvarht;
    اگر (get_current_funccal() != پوچ
	       && get_current_funccal()->fc_func->uf_def_status
							    == UF_NOT_COMPILED)
    {
	// a: and l: are only used in functions defined with ":function"
	اگر (*name == 'a')			// a: function argument
	    بازگشت get_funccal_args_ht();
	اگر (*name == 'l')			// l: local function variable
	    بازگشت get_funccal_local_ht();
    }
    اگر (*name == 's')				// script variable
    {
	ht = get_script_local_ht();
	اگر (ht != پوچ)
	    بازگشت ht;
    }
    بازگشت پوچ;
}

/*
 * Get the string value of a (global/local) variable.
 * Note: see tv_get_string() for how long the pointer remains valid.
 * Returns NULL when it doesn't exist.
 */
    char_u *
get_var_مقدار(char_u *name)
{
    dictitem_T	*v;

    v = find_var(name, پوچ, FALSE);
    اگر (v == پوچ)
	بازگشت پوچ;
    بازگشت tv_get_string(&v->di_tv);
}

/*
 * Allocate a new hashtab for a sourced script.  It will be used while
 * sourcing this script and when executing functions defined in the script.
 */
    تهی
new_script_vars(scid_T id)
{
    scriptvar_T *sv;

    sv = ALLOC_CLEAR_ONE(scriptvar_T);
    اگر (sv == پوچ)
	بازگشت;
    init_var_dict(&sv->sv_dict, &sv->sv_var, VAR_SCOPE);
    SCRIPT_ITEM(id)->sn_vars = sv;
}

/*
 * Initialize dictionary "dict" as a scope and set variable "dict_var" to
 * point to it.
 */
    تهی
init_var_dict(dict_T *dict, dictitem_T *dict_var, صحیح scope)
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
    dict_var->di_کلید[0] = NUL;
}

/*
 * Unreference a dictionary initialized by init_var_dict().
 */
    تهی
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
    تهی
vars_clear(hashtab_T *ht)
{
    vars_clear_ext(ht, TRUE);
}

/*
 * Like vars_clear(), but only free the value if "free_val" is TRUE.
 */
    تهی
vars_clear_ext(hashtab_T *ht, صحیح free_val)
{
    صحیح		todo;
    hashitem_T	*hi;
    dictitem_T	*v;

    hash_lock(ht);
    todo = (صحیح)ht->ht_used;
    FOR_ALL_HASHTAB_اقلام(ht, hi, todo)
    {
	اگر (!HASHITEM_EMPTY(hi))
	{
	    --todo;

	    // Free the variable.  Don't remove it from the hashtab,
	    // ht_array might change then.  hash_clear() takes care of it
	    // later.
	    v = HI2DI(hi);
	    اگر (free_val)
		clear_tv(&v->di_tv);
	    اگر (v->di_flags & DI_FLAGS_ALLOC)
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
    تهی
delete_var(hashtab_T *ht, hashitem_T *hi)
{
    dictitem_T	*di = HI2DI(hi);

    اگر (hash_remove(ht, hi, "delete variable") != OK)
	بازگشت;

    clear_tv(&di->di_tv);
    vim_free(di);
}

/*
 * Delete the exported variables of a reloaded Vim9 autoload script.  They live
 * in the global namespace with the autoload prefix (e.g. "foo#bar") and are
 * recreated when the script body runs again.
 */
    تهی
delete_autoload_export_vars(char_u *prefix)
{
    اگر (prefix == پوچ)
	بازگشت;
    size_t  prefixlen = STRLEN(prefix);

    hash_lock(&globvarht);
    صحیح	todo = (صحیح)globvarht.ht_used;

    برای (hashitem_T *hi = globvarht.ht_array; todo > 0; ++hi)
	اگر (!HASHITEM_EMPTY(hi))
	{
	    dictitem_T	*di = HI2DI(hi);

	    --todo;
	    // Keep a class or enum: existing objects still refer to it.
	    اگر (di->di_tv.v_type != VAR_CLASS
		    && STRNCMP(di->di_کلید, prefix, prefixlen) == 0)
		delete_var(&globvarht, hi);
	}
    hash_unlock(&globvarht);
}

/*
 * List the value of one internal variable.
 */
    ایستا تهی
list_one_var(dictitem_T *v, نویسه *prefix, صحیح *first)
{
    char_u	*tofree;
    char_u	*s;
    char_u	numbuf[NUMBUFLEN];

    s = echo_string(&v->di_tv, &tofree, numbuf, get_copyID());
    list_one_var_a(prefix, v->di_کلید, v->di_tv.v_type,
					 s == پوچ ? (char_u *)"" : s, first);
    vim_free(tofree);
}

    ایستا تهی
list_one_var_a(
    نویسه	*prefix,
    char_u	*name,
    صحیح		type,
    char_u	*string,
    صحیح		*first)  // when TRUE clear rest of screen and set to FALSE
{
    // don't use msg() or msg_attr() to avoid overwriting "v:statusmsg"
    پیام_start();
    پیام_puts(prefix);
    اگر (name != پوچ)	// "a:" vars don't have a name stored
	پیام_puts((نویسه *)name);
    پیام_putchar(' ');
    پیام_advance(22);
    اگر (type == VAR_عدد)
	پیام_putchar('#');
    وگرنه اگر (type == VAR_FUNC || type == VAR_PARTIAL)
	پیام_putchar('*');
    وگرنه اگر (type == VAR_LIST)
    {
	پیام_putchar('[');
	اگر (*string == '[')
	    ++string;
    }
    وگرنه اگر (type == VAR_DICT)
    {
	پیام_putchar('{');
	اگر (*string == '{')
	    ++string;
    }
    وگرنه
	پیام_putchar(' ');

    پیام_outtrans(string);

    اگر (type == VAR_FUNC || type == VAR_PARTIAL)
	پیام_puts("()");
    اگر (*first)
    {
	پیام_clr_eos();
	*first = FALSE;
    }
}

/*
 * Addition handling for setting a v: variable.
 * Return TRUE if the variable should be set normally,
 *        FALSE if nothing else needs to be done.
 */
    صحیح
before_set_vvar(
    char_u	*varname,
    dictitem_T	*di,
    typval_T	*tv,
    صحیح		copy,
    صحیح		*type_error)
{
    اگر (di->di_tv.v_type == VAR_STRING)
    {
	VIM_CLEAR(di->di_tv.vval.v_string);
	اگر (copy || tv->v_type != VAR_STRING)
	{
	    char_u *val = tv_get_string(tv);

	    // Careful: when assigning to v:errmsg and
	    // tv_get_string() causes an error message the variable
	    // will already be set.
	    اگر (di->di_tv.vval.v_string == پوچ)
		di->di_tv.vval.v_string = vim_strsave(val);
	}
	وگرنه
	{
	    // Take over the string to avoid an extra alloc/free.
	    di->di_tv.vval.v_string = tv->vval.v_string;
	    tv->vval.v_string = پوچ;
	}
	بازگشت FALSE;
    }
    وگرنه اگر (di->di_tv.v_type == VAR_عدد)
    {
	di->di_tv.vval.v_عدد = tv_get_عدد(tv);
	اگر (STRCMP(varname, "searchforward") == 0)
	    set_جستجو_direction(di->di_tv.vval.v_عدد ? '/' : '?');
#ifdef FEAT_جستجو_EXTRA
	وگرنه اگر (STRCMP(varname, "hlsearch") == 0)
	{
	    no_hlsearch = !di->di_tv.vval.v_عدد;
	    redraw_all_later(UPD_SOME_VALID);
	}
#endif
	بازگشت FALSE;
    }
    وگرنه اگر (di->di_tv.v_type != tv->v_type)
    {
	*type_error = TRUE;
	بازگشت FALSE;
    }
    بازگشت TRUE;
}

/*
 * Set variable "name" to value in "tv".
 * If the variable already exists, the value is updated.
 * Otherwise the variable is created.
 */
    تهی
set_var(
    char_u	*name,
    typval_T	*tv,
    صحیح		copy)	    // make copy of value in "tv"
{
    set_var_const(name, 0, پوچ, tv, copy, ASSIGN_DECL, 0);
}

/*
 * Set variable "name" to value in "tv_arg".
 * When "sid" is non-zero "name" is in the script with this ID.
 * If the variable already exists and "is_const" is FALSE the value is updated.
 * Otherwise the variable is created.
 */
    صحیح
set_var_const(
    char_u	*name,
    scid_T	sid,
    type_T	*type_arg,
    typval_T	*tv_arg,
    صحیح		copy,	    // make copy of value in "tv"
    صحیح		flags_arg,  // ASSIGN_CONST, ASSIGN_FINAL, etc.
    صحیح		var_idx)    // index for ":let [a, b] = list"
{
    typval_T	*tv = tv_arg;
    type_T	*type = type_arg;
    typval_T	bool_tv;
    dictitem_T	*di;
    typval_T	*dest_tv;
    char_u	*varname;
    char_u	*name_tofree = پوچ;
    hashtab_T	*ht = پوچ;
    صحیح		is_script_local;
    صحیح		vim9script = in_vim9script();
    صحیح		var_in_vim9script;
    صحیح		var_in_autoload = FALSE;
    صحیح		flags = flags_arg;
    صحیح		free_tv_arg = !copy;  // free tv_arg if not used
    صحیح		rc = FAIL;

    اگر (sid != 0)
    {
	varname = پوچ;
	اگر (SCRIPT_ID_VALID(sid))
	{
	    char_u	*auto_name = پوچ;
	    اگر (find_var_autoload_prefix(name, sid, &ht, &auto_name) != پوچ)
	    {
		var_in_autoload = TRUE;
		varname = auto_name;
		name_tofree = varname;
	    }
	    وگرنه
		ht = &SCRIPT_VARS(sid);
	}
	اگر (varname == پوچ)
	    varname = name;
    }
    وگرنه
    {
	scriptitem_T	*si;
	char_u		*auto_name = پوچ;

	اگر (in_vim9script()
	    && SCRIPT_ID_VALID(current_sctx.sc_sid)
	    && (si = SCRIPT_ITEM(current_sctx.sc_sid))
					      ->sn_autoload_prefix != پوچ
	    && (is_export
		|| find_var_autoload_prefix(name, 0, پوچ, &auto_name)
								    != پوچ))
	{
	    // In a vim9 autoload script an exported variable is put in the
	    // global namespace with the autoload prefix.
	    var_in_autoload = TRUE;
	    varname = auto_name != پوچ ? auto_name
		      : concat_str(si->sn_autoload_prefix, name);
	    اگر (varname == پوچ)
		برو failed;
	    name_tofree = varname;
	    ht = &globvarht;
	}
	وگرنه
	    ht = find_var_ht(name, &varname);
    }
    اگر (ht == پوچ || *varname == NUL)
    {
	semsg(_(e_illegal_variable_name_str), name);
	برو failed;
    }
    is_script_local = ht == get_script_local_ht() || sid != 0
							    || var_in_autoload;

    اگر (vim9script
	    && !is_script_local
	    && (flags & (ASSIGN_NO_DECL | ASSIGN_DECL)) == 0
	    && (flags & (ASSIGN_CONST | ASSIGN_FINAL)) == 0
	    && name[1] == ':')
    {
	vim9_declare_error(name);
	برو failed;
    }
    اگر ((flags & ASSIGN_FOR_LOOP) && is_scoped_variable(name))
	// Do not make g:var, w:var, b:var or t:var final.
	flags &= ~ASSIGN_FINAL;

    var_in_vim9script = is_script_local && current_script_is_vim9();
    اگر (var_in_vim9script && name[0] == '_' && name[1] == NUL)
    {
	// For "[a, _] = list" the underscore is ignored.
	اگر ((flags & ASSIGN_UNPACK) == 0)
	    emsg(_(e_cannot_use_underscore_here));
	برو failed;
    }

    di = find_var_in_ht(ht, 0, varname, TRUE);

    اگر (di == پوچ && var_in_vim9script)
    {
	imported_T  *import = find_imported(varname, 0, FALSE);

	اگر (import != پوچ)
	{
	    // imported name space cannot be used
	    اگر ((flags & ASSIGN_NO_DECL) == 0)
	    {
		semsg(_(e_redefining_imported_item_str), name);
		برو failed;
	    }
	    semsg(_(e_cannot_use_str_itself_it_is_imported), name);
	    برو failed;
	}
	اگر (!in_vim9script())
	{
	    semsg(_(e_cannot_create_vim9_script_variable_in_function_str),
									 name);
	    برو failed;
	}
    }

    // Search in parent scope which is possible to reference from lambda
    اگر (di == پوچ)
	di = find_var_in_scoped_ht(name, TRUE);

    اگر ((tv->v_type == VAR_FUNC || tv->v_type == VAR_PARTIAL)
				  && var_wrong_func_name(name, di == پوچ))
	برو failed;

    اگر (need_convert_to_bool(type, tv))
    {
	// Destination is a bool and the value is not, but it can be
	// converted.
	CLEAR_FIELD(bool_tv);
	bool_tv.v_type = VAR_BOOL;
	bool_tv.vval.v_عدد = tv2bool(tv) ? VVAL_TRUE : VVAL_FALSE;
	tv = &bool_tv;
    }

    اگر (di != پوچ)
    {
	// Item already exists.  Allowed to replace when reloading.
	اگر ((di->di_flags & DI_FLAGS_RELOAD) == 0)
	{
	    اگر ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					 && (flags & ASSIGN_FOR_LOOP) == 0)
	    {
		emsg(_(e_cannot_modify_existing_variable));
		برو failed;
	    }

	    اگر (is_script_local && vim9script
			  && (flags & (ASSIGN_NO_DECL | ASSIGN_DECL)) == 0)
	    {
		semsg(_(e_redefining_script_item_str), name);
		برو failed;
	    }

	    اگر (check_typval_is_مقدار(&di->di_tv) == FAIL)
		برو failed;

	    // List and Blob types can be modified in-place using the "+="
	    // compound operator.  For other types, this is not allowed.
	    صحیح type_inplace_modifiable =
		(di->di_tv.v_type == VAR_LIST || di->di_tv.v_type == VAR_BLOB);

	    اگر (var_in_vim9script && (flags & ASSIGN_FOR_LOOP) == 0
		    && ((flags & ASSIGN_COMPOUND_OP) == 0
			|| !type_inplace_modifiable))
	    {
		where_T where = WHERE_INIT;
		svar_T  *sv = find_typval_in_script(&di->di_tv, sid, TRUE);

		اگر (sv != پوچ)
		{
		    // check the type and adjust to bool if needed
		    اگر (var_idx > 0)
		    {
			where.wt_index = var_idx;
			where.wt_kind = WT_VARIABLE;
		    }
		    اگر (check_script_var_type(sv, tv, name, where) == FAIL)
			برو failed;
		    اگر (type == پوچ)
			type = sv->sv_type;
		    sv->sv_flags |= SVFLAG_ASSIGNED;
		}
	    }

	    // Modifying a final variable with a List value using the "+="
	    // operator is allowed.  For other types, it is not allowed.
	    اگر ((((flags & ASSIGN_FOR_LOOP) == 0 || (flags & ASSIGN_DECL) == 0)
			&& ((flags & ASSIGN_COMPOUND_OP) == 0
			    || !type_inplace_modifiable))
				 ? var_check_permission(di, name) == FAIL
				 : var_check_ro(di->di_flags, name, FALSE))
		برو failed;
	}
	وگرنه
	{
	    // can only redefine once
	    di->di_flags &= ~DI_FLAGS_RELOAD;

	    // A Vim9 script-local variable is also present in sn_all_vars
	    // and sn_var_vals.  It may set "type" from "tv".
	    اگر (var_in_vim9script || var_in_autoload)
		update_vim9_script_var(FALSE, di,
			var_in_autoload ? name : di->di_کلید, flags,
			tv, &type, (flags & ASSIGN_NO_MEMBER_TYPE) == 0);
	}

	// existing variable, need to clear the value

	// In Vim9 script a String v: variable only takes a String.
	اگر (ht == &vimvarht && di->di_tv.v_type == VAR_STRING
							    && in_vim9script())
	{
	    where_T	where = WHERE_INIT;

	    اگر (check_typval_type(&t_string, tv, where) == FAIL)
		برو failed;
	}

	// Handle setting internal v: variables separately where needed to
	// prevent changing the type.
	صحیح type_error = FALSE;
	اگر (ht == &vimvarht
		&& !before_set_vvar(varname, di, tv, copy, &type_error))
	{
	    اگر (type_error)
		semsg(_(e_setting_v_str_to_مقدار_with_wrong_type), varname);
	    برو failed;
	}

	clear_tv(&di->di_tv);

	اگر ((flags & ASSIGN_UPDATE_BLOCK_ID)
				       && SCRIPT_ID_VALID(current_sctx.sc_sid))
	{
	    scriptitem_T *si = SCRIPT_ITEM(current_sctx.sc_sid);

	    update_script_var_block_id(name, si->sn_current_block_id);
	}
    }
    وگرنه
    {
	// Item not found, check if a function already exists.
	اگر (is_script_local && (flags & (ASSIGN_NO_DECL | ASSIGN_DECL)) == 0
	       && lookup_scriptitem(name, STRLEN(name), FALSE, پوچ) == OK)
	{
	    semsg(_(e_redefining_script_item_str), name);
	    برو failed;
	}

	// add a new variable
	اگر (var_in_vim9script && (flags & ASSIGN_NO_DECL))
	{
	    semsg(_(e_unknown_variable_str), name);
	    برو failed;
	}

	اگر (check_hashtab_frozen(ht, "add variable"))
	    برو failed;

	// Can't add "v:" or "a:" variable.
	اگر (ht == &vimvarht || ht == get_funccal_args_ht())
	{
	    semsg(_(e_illegal_variable_name_str), name);
	    برو failed;
	}

	// Make sure the variable name is valid.  In Vim9 script an
	// autoload variable must be prefixed with "g:" unless in an
	// autoload script.
	اگر (!valid_varname(varname, -1, !vim9script
			|| STRNCMP(name, "g:", 2) == 0 || var_in_autoload))
	    برو failed;

	di = alloc(offsetof(dictitem_T, di_کلید) + STRLEN(varname) + 1);
	اگر (di == پوچ)
	    برو failed;
	STRCPY(di->di_کلید, varname);
	اگر (hash_add(ht, DI2HIKEY(di), "add variable") == FAIL)
	{
	    vim_free(di);
	    برو failed;
	}
	di->di_flags = DI_FLAGS_ALLOC;
	اگر (flags & (ASSIGN_CONST | ASSIGN_FINAL))
	    di->di_flags |= DI_FLAGS_LOCK;

	// A Vim9 script-local variable is also added to sn_all_vars and
	// sn_var_vals. It may set "type" from "tv".
	اگر (var_in_vim9script || var_in_autoload)
	    update_vim9_script_var(TRUE, di,
		    var_in_autoload ? name : di->di_کلید, flags,
			  tv, &type, (flags & ASSIGN_NO_MEMBER_TYPE) == 0);
    }

    dest_tv = &di->di_tv;
    اگر (copy || tv->v_type == VAR_عدد || tv->v_type == VAR_FLOAT)
	copy_tv(tv, dest_tv);
    وگرنه
    {
	*dest_tv = *tv;
	dest_tv->v_lock = 0;
	init_tv(tv);
    }
    free_tv_arg = FALSE;

    اگر (vim9script && type != پوچ)
	set_tv_type(dest_tv, type);

    // ":const var = value" locks the value
    // ":final var = value" locks "var"
    اگر (flags & ASSIGN_CONST)
	// Like :lockvar! name: lock the value and what it contains, but only
	// if the reference count is up to one.  That locks only literal
	// values.
	item_lock(dest_tv, DICT_MAXNEST, TRUE, TRUE);

    rc = OK;

failed:
    vim_free(name_tofree);
    اگر (free_tv_arg)
	clear_tv(tv_arg);

    بازگشت rc;
}

/*
 * Check in this order for backwards compatibility:
 * - Whether the variable is read-only
 * - Whether the variable value is locked
 * - Whether the variable is locked
 * NOTE: "name" is only used for error messages.
 */
    صحیح
var_check_permission(dictitem_T *di, char_u *name)
{
    اگر (var_check_ro(di->di_flags, name, FALSE)
		    || مقدار_check_lock(di->di_tv.v_lock, name, FALSE)
		    || var_check_lock(di->di_flags, name, FALSE))
	بازگشت FAIL;
    بازگشت OK;
}

/*
 * Return TRUE if di_flags "flags" indicates variable "name" is read-only.
 * Also give an error message.
 */
    صحیح
var_check_ro(صحیح flags, char_u *name, صحیح use_gettext)
{
    اگر (flags & DI_FLAGS_RO)
    {
	اگر (name == پوچ)
	    emsg(_(e_cannot_change_readonly_variable));
	وگرنه
	    semsg(_(e_cannot_change_readonly_variable_str),
				       use_gettext ? (char_u *)_(name) : name);
	بازگشت TRUE;
    }
    اگر ((flags & DI_FLAGS_RO_SBX) && sandbox)
    {
	اگر (name == پوچ)
	    emsg(_(e_cannot_set_variable_in_sandbox));
	وگرنه
	    semsg(_(e_cannot_set_variable_in_sandbox_str),
				       use_gettext ? (char_u *)_(name) : name);
	بازگشت TRUE;
    }
    بازگشت FALSE;
}

/*
 * Return TRUE if di_flags "flags" indicates variable "name" is locked.
 * Also give an error message.
 */
    صحیح
var_check_lock(صحیح flags, char_u *name, صحیح use_gettext)
{
    اگر (flags & DI_FLAGS_LOCK)
    {
	semsg(_(e_variable_is_locked_str),
				       use_gettext ? (char_u *)_(name) : name);
	بازگشت TRUE;
    }
    بازگشت FALSE;
}

/*
 * Return TRUE if di_flags "flags" indicates variable "name" is fixed.
 * Also give an error message.
 */
    صحیح
var_check_fixed(صحیح flags, char_u *name, صحیح use_gettext)
{
    اگر (flags & DI_FLAGS_FIX)
    {
	اگر (name == پوچ)
	    emsg(_(e_cannot_delete_variable));
	وگرنه
	    semsg(_(e_cannot_delete_variable_str),
				      use_gettext ? (char_u *)_(name) : name);
	بازگشت TRUE;
    }
    بازگشت FALSE;
}

/*
 * Check if a funcref is assigned to a valid variable name.
 * Return TRUE and give an error if not.
 */
    صحیح
var_wrong_func_name(
    char_u *name,    // points to start of variable name
    صحیح    new_var)  // TRUE when creating the variable
{
    // Allow for w: b: s: and t:.  In Vim9 script s: is not allowed, because
    // the name can be used without the s: prefix.
    // Allow autoload variable.
    اگر (!((vim_strchr((char_u *)"wbt", name[0]) != پوچ
		    || (!in_vim9script() && name[0] == 's')) && name[1] == ':')
	    && !ASCII_ISUPPER((name[0] != NUL && name[1] == ':')
						     ? name[2] : name[0])
	    && vim_strchr(name, '#') == پوچ)
    {
	semsg(_(e_funcref_variable_name_must_start_with_capital_str), name);
	بازگشت TRUE;
    }
    // Don't allow hiding a function.  When "v" is not NULL we might be
    // assigning another function to the same var, the type is checked
    // below.
    اگر (new_var && function_exists(name, FALSE))
    {
	semsg(_(e_variable_name_conflicts_with_existing_function_str),
								    name);
	بازگشت TRUE;
    }
    بازگشت FALSE;
}

/*
 * Return TRUE if "flags" indicates variable "name" has a locked (immutable)
 * value.  Also give an error message, using "name" or _("name") when
 * "use_gettext" is TRUE.
 */
    صحیح
مقدار_check_lock(صحیح lock, char_u *name, صحیح use_gettext)
{
    اگر (lock & VAR_LOCKED)
    {
	اگر (name == پوچ)
	    emsg(_(e_مقدار_is_locked));
	وگرنه
	    semsg(_(e_مقدار_is_locked_str),
				       use_gettext ? (char_u *)_(name) : name);
	بازگشت TRUE;
    }
    اگر (lock & VAR_FIXED)
    {
	اگر (name == پوچ)
	    emsg(_(e_cannot_change_مقدار));
	وگرنه
	    semsg(_(e_cannot_change_مقدار_of_str),
				       use_gettext ? (char_u *)_(name) : name);
	بازگشت TRUE;
    }
    بازگشت FALSE;
}

/*
 * Check if a variable name is valid.  When "autoload" is true "#" is allowed.
 * If "len" is -1 use all of "varname", otherwise up to "varname[len]".
 * Return FALSE and give an error if not.
 */
    صحیح
valid_varname(char_u *varname, صحیح len, صحیح autoload)
{
    char_u *p;

    برای (p = varname; len < 0 ? *p != NUL : p < varname + len; ++p)
	اگر (!eval_isnamec1(*p) && (p == varname || !VIM_ISDIGIT(*p))
					 && !(autoload && *p == AUTOLOAD_CHAR))
	{
	    semsg(_(e_illegal_variable_name_str), varname);
	    بازگشت FALSE;
	}
    بازگشت TRUE;
}

/*
 * Implements the logic to retrieve local variable and option values.
 * Used by "getwinvar()" "gettabvar()" "gettabwinvar()" "getbufvar()".
 */
    ایستا تهی
get_var_from(
    char_u	*varname,
    typval_T	*rettv,
    typval_T	*deftv,	    // Default value if not found.
    صحیح		htname,	    // 't'ab, 'w'indow or 'b'uffer local.
    tabpage_T	*tp,	    // can be NULL
    win_T	*win,
    buf_T	*buf)	    // Ignored if htname is not 'b'.
{
    dictitem_T	*v;
    صحیح		done = FALSE;
    switchwin_T	switchwin;
    صحیح		need_switch_win;
    صحیح		do_change_curbuf = buf != پوچ && htname == 'b';

    ++emsg_off;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = پوچ;

    اگر (varname != پوچ && tp != پوچ && win != پوچ
	    && (htname != 'b' || buf != پوچ))
    {
	// Set curwin to be our win, temporarily.  Also set the tabpage,
	// otherwise the window is not valid. Only do this when needed,
	// autocommands get blocked.
	// If we have a buffer reference avoid the switching, we're saving and
	// restoring curbuf directly.
	need_switch_win = !(tp == curtab && win == curwin) && !do_change_curbuf;
	اگر (!need_switch_win || switch_win(&switchwin, win, tp, TRUE) == OK)
	{
	    // Handle options. There are no tab-local options.
	    اگر (*varname == '&' && htname != 't')
	    {
		buf_T	*save_curbuf = curbuf;

		// Change curbuf so the option is read from the correct buffer.
		اگر (do_change_curbuf)
		    curbuf = buf;

		اگر (varname[1] == NUL)
		{
		    // get all window-local or buffer-local options in a dict
		    dict_T	*opts = get_winbuf_options(htname == 'b');

		    اگر (opts != پوچ)
		    {
			rettv_dict_set(rettv, opts);
			done = TRUE;
		    }
		}
		وگرنه اگر (eval_option(&varname, rettv, TRUE) == OK)
		    // Local option
		    done = TRUE;

		curbuf = save_curbuf;
	    }
	    وگرنه اگر (*varname == NUL)
	    {
		// Empty string: return a dict with all the local variables.
		اگر (htname == 'b')
		    v = &buf->b_bufvar;
		وگرنه اگر (htname == 'w')
		    v = &win->w_winvar;
		وگرنه
		    v = &tp->tp_winvar;
		copy_tv(&v->di_tv, rettv);
		done = TRUE;
	    }
	    وگرنه
	    {
		hashtab_T	*ht;

		اگر (htname == 'b')
		    ht = &buf->b_vars->dv_hashtab;
		وگرنه اگر (htname == 'w')
		    ht = &win->w_vars->dv_hashtab;
		وگرنه
		    ht = &tp->tp_vars->dv_hashtab;

		// Look up the variable.
		v = find_var_in_ht(ht, htname, varname, FALSE);
		اگر (v != پوچ)
		{
		    copy_tv(&v->di_tv, rettv);
		    done = TRUE;
		}
	    }
	}

	اگر (need_switch_win)
	    // restore previous notion of curwin
	    restore_win(&switchwin, TRUE);
    }

    اگر (!done && deftv->v_type != VAR_UNKNOWN)
	// use the default value
	copy_tv(deftv, rettv);

    --emsg_off;
}

/*
 * getwinvar() and gettabwinvar()
 */
    ایستا تهی
getwinvar(
    typval_T	*argvars,
    typval_T	*rettv,
    صحیح		off)	    // 1 for gettabwinvar()
{
    char_u	*varname;
    tabpage_T	*tp;
    win_T	*win;

    اگر (off == 1)
	tp = find_tabpage((صحیح)tv_get_عدد_chk(&argvars[0], پوچ));
    وگرنه
	tp = curtab;
    win = find_win_by_nr(&argvars[off], tp);
    varname = tv_get_string_chk(&argvars[off + 1]);

    get_var_from(varname, rettv, &argvars[off + 2], 'w', tp, win, پوچ);
}

/*
 * Set option "varname" to the value of "varp" for the current buffer/window.
 */
    ایستا تهی
set_option_from_tv(char_u *varname, typval_T *varp)
{
    long	numval = 0;
    char_u	*strval;
    char_u	nbuf[NUMBUFLEN];
    صحیح		error = FALSE;

    صحیح		opt_idx = findoption(varname);
    اگر (opt_idx < 0)
    {
	semsg(_(e_unknown_option_str_2), varname);
	بازگشت;
    }
    صحیح		opt_p_flags = get_option_flags(opt_idx);

    اگر (varp->v_type == VAR_BOOL)
    {
	اگر (opt_p_flags & P_STRING)
	{
	    emsg(_(e_string_required));
	    بازگشت;
	}
	numval = (long)varp->vval.v_عدد;
	strval = (char_u *)"0";  // avoid using "false"
    }
    وگرنه
    {
	اگر ((opt_p_flags & (P_رقم|P_BOOL))
		&& (!in_vim9script() || varp->v_type != VAR_STRING))
	    numval = (long)tv_get_عدد_chk(varp, &error);
	اگر (!error)
	    strval = tv_get_string_buf_chk(varp, nbuf);
    }
    اگر (!error && strval != پوچ)
	set_option_مقدار_give_err(varname, numval, strval, OPT_LOCAL);
}

/*
 * "setwinvar()" and "settabwinvar()" functions
 */
    ایستا تهی
setwinvar(typval_T *argvars, صحیح off)
{
    win_T	*win;
    switchwin_T	switchwin;
    صحیح		need_switch_win;
    char_u	*varname, *winvarname;
    typval_T	*varp;
    tabpage_T	*tp = پوچ;

    اگر (check_secure())
	بازگشت;

    اگر (off == 1)
	tp = find_tabpage((صحیح)tv_get_عدد_chk(&argvars[0], پوچ));
    وگرنه
	tp = curtab;
    win = find_win_by_nr(&argvars[off], tp);
    varname = tv_get_string_chk(&argvars[off + 1]);
    varp = &argvars[off + 2];

    اگر (win == پوچ || varname == پوچ)
	بازگشت;

    need_switch_win = !(tp == curtab && win == curwin);
    اگر (!need_switch_win
	    || switch_win(&switchwin, win, tp, TRUE) == OK)
    {
	اگر (*varname == '&')
	    set_option_from_tv(varname + 1, varp);
	وگرنه
	{
	    winvarname = alloc(STRLEN(varname) + 3);
	    اگر (winvarname != پوچ)
	    {
		STRCPY(winvarname, "w:");
		STRCPY(winvarname + 2, varname);
		set_var(winvarname, varp, TRUE);
		vim_free(winvarname);
	    }
	}
    }
    اگر (need_switch_win)
	restore_win(&switchwin, TRUE);
}

/*
 * reset v:option_new, v:option_old, v:option_oldlocal, v:option_oldglobal,
 * v:option_type, and v:option_command.
 */
    تهی
reset_v_option_vars(تهی)
{
    set_vim_var_string(VV_OPTION_NEW, پوچ, -1);
    set_vim_var_string(VV_OPTION_OLD, پوچ, -1);
    set_vim_var_string(VV_OPTION_OLDLOCAL, پوچ, -1);
    set_vim_var_string(VV_OPTION_OLDGLOBAL, پوچ, -1);
    set_vim_var_string(VV_OPTION_TYPE, پوچ, -1);
    set_vim_var_string(VV_OPTION_COMMAND, پوچ, -1);
}

/*
 * Add an assert error to v:errors.
 */
    تهی
assert_error(garray_T *gap)
{
    ساختار vimvar   *vp = &vimvars[VV_ERRORS];

    اگر (vp->vv_tv_type != VAR_LIST || vimvars[VV_ERRORS].vv_list == پوچ)
	// Make sure v:errors is a list.
	set_vim_var_list(VV_ERRORS, list_alloc());
    list_append_string(vimvars[VV_ERRORS].vv_list, gap->ga_data, gap->ga_len);
}

    صحیح
var_exists(char_u *var)
{
    char_u	*arg = var;
    char_u	*name;
    char_u	*tofree;
    typval_T    tv;
    صحیح		len = 0;
    صحیح		n = FALSE;

    // get_name_len() takes care of expanding curly braces
    name = var;
    len = get_name_len(&arg, &tofree, TRUE, FALSE);
    اگر (len > 0)
    {
	اگر (tofree != پوچ)
	    name = tofree;
	n = (eval_variable(name, len, 0, &tv, پوچ,
				 EVAL_VAR_NOAUTOLOAD + EVAL_VAR_IMPORT) == OK);
	اگر (n)
	{
	    // handle d.key, l[idx], f(expr)
	    arg = skipwhite(arg);
	    n = (handle_subscript(&arg, name, &tv, &EVALARG_EVALUATE,
								 FALSE) == OK);
	    اگر (n)
		clear_tv(&tv);
	}
    }
    اگر (*arg != NUL)
	n = FALSE;

    vim_free(tofree);
    بازگشت n;
}

ایستا lval_T	*redir_lval = پوچ;
#define EVALCMD_BUSY (redir_lval == (lval_T *)&redir_lval)
ایستا garray_T redir_ga;	// only valid when redir_lval is not NULL
ایستا char_u	*redir_endp = پوچ;
ایستا char_u	*redir_varname = پوچ;

    صحیح
alloc_redir_lval(تهی)
{
    redir_lval = ALLOC_CLEAR_ONE(lval_T);
    اگر (redir_lval == پوچ)
	بازگشت FAIL;
    بازگشت OK;
}

    تهی
clear_redir_lval(تهی)
{
    VIM_CLEAR(redir_lval);
}

    تهی
init_redir_ga(تهی)
{
    ga_init2(&redir_ga, اندازه(نویسه), 500);
}

/*
 * Start recording command output to a variable
 * When "append" is TRUE append to an existing variable.
 * Returns OK if successfully completed the setup.  FAIL otherwise.
 */
    صحیح
var_redir_start(char_u *name, صحیح append)
{
    صحیح		called_emsg_before;
    typval_T	tv;

    // Catch a bad name early.
    اگر (!eval_isnamec1(*name))
    {
	emsg(_(e_invalid_argument));
	بازگشت FAIL;
    }

    // Make a copy of the name, it is used in redir_lval until redir ends.
    redir_varname = vim_strsave(name);
    اگر (redir_varname == پوچ)
	بازگشت FAIL;

    اگر (alloc_redir_lval() == FAIL)
    {
	var_redir_stop();
	بازگشت FAIL;
    }

    // The output is stored in growarray "redir_ga" until redirection ends.
    init_redir_ga();

    // Parse the variable name (can be a dict or list entry).
    redir_endp = get_lval(redir_varname, پوچ, redir_lval, FALSE, FALSE, 0,
							     FNE_CHECK_START);
    اگر (redir_endp == پوچ || redir_lval->ll_name == پوچ || *redir_endp != NUL)
    {
	clear_lval(redir_lval);
	اگر (redir_endp != پوچ && *redir_endp != NUL)
	    // Trailing characters are present after the variable name
	    semsg(_(e_trailing_characters_str), redir_endp);
	وگرنه
	    semsg(_(e_invalid_argument_str), name);
	redir_endp = پوچ;  // don't store a value, only cleanup
	var_redir_stop();
	بازگشت FAIL;
    }

    // check if we can write to the variable: set it to or append an empty
    // string
    called_emsg_before = called_emsg;
    tv.v_type = VAR_STRING;
    tv.vval.v_string = (char_u *)"";
    اگر (append)
	set_var_lval(redir_lval, redir_endp, &tv, TRUE,
					     ASSIGN_NO_DECL, (char_u *)".", 0);
    وگرنه
	set_var_lval(redir_lval, redir_endp, &tv, TRUE,
					     ASSIGN_NO_DECL, (char_u *)"=", 0);
    clear_lval(redir_lval);
    اگر (called_emsg > called_emsg_before)
    {
	redir_endp = پوچ;  // don't store a value, only cleanup
	var_redir_stop();
	بازگشت FAIL;
    }

    بازگشت OK;
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
    تهی
var_redir_str(char_u *مقدار, صحیح مقدار_len)
{
    صحیح		len;

    اگر (redir_lval == پوچ)
	بازگشت;

    اگر (مقدار_len == -1)
	len = (صحیح)STRLEN(مقدار);	// Append the entire string
    وگرنه
	len = مقدار_len;		// Append only "value_len" characters

    اگر (ga_grow(&redir_ga, len) == OK)
    {
	mch_memmove((نویسه *)redir_ga.ga_data + redir_ga.ga_len, مقدار, len);
	redir_ga.ga_len += len;
    }
    وگرنه
	var_redir_stop();
}

/*
 * Stop redirecting command output to a variable.
 * Frees the allocated memory.
 */
    تهی
var_redir_stop(تهی)
{
    typval_T	tv;

    اگر (EVALCMD_BUSY)
    {
	redir_lval = پوچ;
	بازگشت;
    }

    اگر (redir_lval != پوچ)
    {
	// If there was no error: assign the text to the variable.
	اگر (redir_endp != پوچ)
	{
	    ga_append(&redir_ga, NUL);  // Append the trailing NUL.
	    tv.v_type = VAR_STRING;
	    tv.vval.v_string = redir_ga.ga_data;
	    // Call get_lval() again, if it's inside a Dict or List it may
	    // have changed.
	    redir_endp = get_lval(redir_varname, پوچ, redir_lval,
					FALSE, FALSE, 0, FNE_CHECK_START);
	    اگر (redir_endp != پوچ && redir_lval->ll_name != پوچ)
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
get_clear_redir_ga(تهی)
{
    char_u *res;

    ga_append(&redir_ga, NUL);  // Append the trailing NUL.
    res = redir_ga.ga_data;
    redir_ga.ga_data = پوچ;
    بازگشت res;
}

/*
 * "gettabvar()" function
 */
    تهی
f_gettabvar(typval_T *argvars, typval_T *rettv)
{
    char_u	*varname;
    tabpage_T	*tp;
    win_T	*win = پوچ;

    اگر (in_vim9script()
	    && (check_for_عدد_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	بازگشت;

    varname = tv_get_string_chk(&argvars[1]);
    tp = find_tabpage((صحیح)tv_get_عدد_chk(&argvars[0], پوچ));
    اگر (tp != پوچ)
	win = tp == curtab || tp->tp_firstwin == پوچ ? firstwin
		: tp->tp_firstwin;

    get_var_from(varname, rettv, &argvars[2], 't', tp, win, پوچ);
}

/*
 * "gettabwinvar()" function
 */
    تهی
f_gettabwinvar(typval_T *argvars, typval_T *rettv)
{
    اگر (in_vim9script()
	    && (check_for_عدد_arg(argvars, 0) == FAIL
		|| check_for_عدد_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL))
	بازگشت;

    getwinvar(argvars, rettv, 1);
}

/*
 * "getwinvar()" function
 */
    تهی
f_getwinvar(typval_T *argvars, typval_T *rettv)
{
    اگر (in_vim9script()
	    && (check_for_عدد_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	بازگشت;

    getwinvar(argvars, rettv, 0);
}

/*
 * "getbufvar()" function
 */
    تهی
f_getbufvar(typval_T *argvars, typval_T *rettv)
{
    char_u	*varname;
    buf_T	*buf;

    اگر (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	بازگشت;

    varname = tv_get_string_chk(&argvars[1]);
    buf = tv_get_buf_from_arg(&argvars[0]);

    get_var_from(varname, rettv, &argvars[2], 'b', curtab, curwin, buf);
}

/*
 * "settabvar()" function
 */
    تهی
f_settabvar(typval_T *argvars, typval_T *rettv UNUSED)
{
    tabpage_T	*save_curtab;
    tabpage_T	*tp;
    tabpage_T	*save_lu_tp;
    char_u	*varname, *tabvarname;
    typval_T	*varp;

    اگر (check_secure())
	بازگشت;

    اگر (in_vim9script()
	    && (check_for_عدد_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	بازگشت;

    tp = find_tabpage((صحیح)tv_get_عدد_chk(&argvars[0], پوچ));
    varname = tv_get_string_chk(&argvars[1]);
    varp = &argvars[2];

    اگر (varname == پوچ || tp == پوچ)
	بازگشت;

    save_curtab = curtab;
    save_lu_tp = lastused_tabpage;
    goto_tabpage_tp(tp, FALSE, FALSE);

    tabvarname = alloc(STRLEN(varname) + 3);
    اگر (tabvarname != پوچ)
    {
	STRCPY(tabvarname, "t:");
	STRCPY(tabvarname + 2, varname);
	set_var(tabvarname, varp, TRUE);
	vim_free(tabvarname);
    }

    // Restore current tabpage and last accessed tabpage.
    اگر (valid_tabpage(save_curtab))
    {
	goto_tabpage_tp(save_curtab, FALSE, FALSE);
	اگر (valid_tabpage(save_lu_tp))
	    lastused_tabpage = save_lu_tp;
    }
}

/*
 * "settabwinvar()" function
 */
    تهی
f_settabwinvar(typval_T *argvars, typval_T *rettv UNUSED)
{
    اگر (in_vim9script()
	    && (check_for_عدد_arg(argvars, 0) == FAIL
		|| check_for_عدد_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL))
	بازگشت;

    setwinvar(argvars, 1);
}

/*
 * "setwinvar()" function
 */
    تهی
f_setwinvar(typval_T *argvars, typval_T *rettv UNUSED)
{
    اگر (in_vim9script()
	    && (check_for_عدد_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	بازگشت;

    setwinvar(argvars, 0);
}

/*
 * "setbufvar()" function
 */
    تهی
f_setbufvar(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    char_u	*varname, *bufvarname;
    typval_T	*varp;

    اگر (check_secure())
	بازگشت;

    اگر (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	بازگشت;

    varname = tv_get_string_chk(&argvars[1]);
    buf = tv_get_buf_from_arg(&argvars[0]);
    varp = &argvars[2];

    اگر (buf == پوچ || varname == پوچ)
	بازگشت;

    اگر (*varname == '&')
    {
	aco_save_T	aco;
	// safe the current window position, it could
	// change because of 'scrollbind' window-local
	// options
	linenr_T	old_topline = curwin->w_topline;

	// Set curbuf to be our buf, temporarily.
	aucmd_prepbuf(&aco, buf);
	اگر (curbuf == buf)
	{
	    // Only when it worked to set "curbuf".
	    set_option_from_tv(varname + 1, varp);

	    // reset notion of buffer
	    aucmd_restbuf(&aco);
	}
	curwin->w_topline = old_topline;
    }
    وگرنه
    {
	bufvarname = alloc(STRLEN(varname) + 3);
	اگر (bufvarname != پوچ)
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
    صحیح		r = OK;

    CLEAR_FIELD(res);
    اگر (arg->v_type == VAR_PARTIAL && arg->vval.v_partial != پوچ)
    {
	res.cb_partial = arg->vval.v_partial;
	++res.cb_partial->pt_refcount;
	res.cb_name = partial_name(res.cb_partial);
    }
    وگرنه
    {
	اگر (arg->v_type == VAR_STRING && arg->vval.v_string != پوچ
					       && SAFE_isdigit(*arg->vval.v_string))
	    r = FAIL;
	وگرنه اگر (arg->v_type == VAR_FUNC || arg->v_type == VAR_STRING)
	{
	    res.cb_name = arg->vval.v_string;
	    اگر (arg->v_type == VAR_STRING)
	    {
		char_u *name = get_scriptlocal_funcname(arg->vval.v_string);
		اگر (name != پوچ)
		{
		    res.cb_name = name;
		    res.cb_free_name = TRUE;
		}
	    }
	    func_ref(res.cb_name);
	}
	وگرنه اگر (arg->v_type == VAR_عدد && arg->vval.v_عدد == 0)
	    res.cb_name = (char_u *)"";
	وگرنه
	    r = FAIL;

	اگر (r == FAIL)
	{
	    emsg(_(e_invalid_callback_argument));
	    res.cb_name = پوچ;
	}
    }
    بازگشت res;
}

/*
 * Copy a callback into a typval_T.
 */
    تهی
put_callback(callback_T *cb, typval_T *tv)
{
    اگر (cb->cb_partial != پوچ)
    {
	tv->v_type = VAR_PARTIAL;
	tv->vval.v_partial = cb->cb_partial;
	++tv->vval.v_partial->pt_refcount;
    }
    وگرنه
    {
	tv->v_type = VAR_FUNC;
	tv->vval.v_string = vim_strsave(cb->cb_name);
	func_ref(cb->cb_name);
    }
}

    ایستا bool
does_callback_own_cb_name(callback_T *cb)
{
    // If cb_partial != NULL then *cb->cb_name is owned by the partial.
    بازگشت cb->cb_partial || cb->cb_free_name;
}

/*
 * Make a copy of "src" into "dest", allocating the function name if needed,
 * without incrementing the refcount.
 */
    تهی
set_callback(callback_T *dest, callback_T *src)
{
    *dest = *src;
    اگر (!does_callback_own_cb_name(src))
    {
	dest->cb_name = vim_strsave(src->cb_name);
	dest->cb_free_name = TRUE;
    }
    *src = (callback_T){0};
}

/*
 * Copy callback from "src" to "dest", incrementing the refcounts.
 */
    تهی
copy_callback(callback_T *dest, callback_T *src)
{
    dest->cb_partial = src->cb_partial;
    اگر (dest->cb_partial != پوچ)
    {
	dest->cb_name = src->cb_name;
	dest->cb_free_name = FALSE;
	++dest->cb_partial->pt_refcount;
    }
    وگرنه
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
    تهی
expand_autload_callback(callback_T *cb)
{
    char_u	*name;
    char_u	*p;
    imported_T	*import;

    اگر (!in_vim9script() || cb->cb_name == پوچ
	    || (!cb->cb_free_name
	       && (cb->cb_partial == پوچ || cb->cb_partial->pt_name == پوچ)))
	بازگشت;
    اگر (cb->cb_partial != پوچ)
	name = cb->cb_partial->pt_name;
    وگرنه
	name = cb->cb_name;
    p = vim_strchr(name, '.');
    اگر (p == پوچ)
	بازگشت;

    import = find_imported(name, p - name, FALSE);
    اگر (import == پوچ || !SCRIPT_ID_VALID(import->imp_sid))
	بازگشت;

    scriptitem_T *si = SCRIPT_ITEM(import->imp_sid);
    اگر (si->sn_autoload_prefix == پوچ)
	بازگشت;

    char_u *newname = concat_str(si->sn_autoload_prefix, p + 1);
    اگر (newname == پوچ)
	بازگشت;

    اگر (cb->cb_partial != پوچ)
    {
	اگر (cb->cb_name == cb->cb_partial->pt_name)
	    cb->cb_name = newname;
	vim_free(cb->cb_partial->pt_name);
	cb->cb_partial->pt_name = newname;
    }
    وگرنه
    {
	vim_free(cb->cb_name);
	cb->cb_name = newname;
    }
}

/*
 * Unref/free "callback" returned by get_callback() or set_callback().
 */
    تهی
free_callback(callback_T *callback)
{
    اگر (callback->cb_partial != پوچ)
    {
	partial_unref(callback->cb_partial);
	callback->cb_partial = پوچ;
    }
    وگرنه اگر (callback->cb_name != پوچ)
	func_unref(callback->cb_name);
    اگر (callback->cb_free_name)
    {
	vim_free(callback->cb_name);
	callback->cb_free_name = FALSE;
    }
    callback->cb_name = پوچ;
}

#endif // FEAT_EVAL
