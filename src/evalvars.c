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

#ضمن "vim.h"

#אם defined(FEAT_EVAL)

ساكن dictitem_T	globvars_var;		// variable used for g:
ساكن dict_T		globvardict;		// Dictionary with g: variables
#define globvarht globvardict.dv_hashtab

/*
 * Old Vim variables such as "v:version" are also available without the "v:".
 * Also in functions.  We need a special hashtable for them.
 */
ساكن hashtab_T	compat_hashtab;

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

تعريفنوع بنية vimvar vimvar_T;

ساكن بنية vimvar
{
    حرف	*vv_name;	// name of variable, without v:
    dictitem16_T vv_di;		// value and name for key (max 16 chars!)
    type_T	*vv_type;	// type or NULL
    حرف	vv_flags;	// VV_COMPAT, VV_RO, VV_RO_SBX
} vimvars[VV_LEN] =
{
    // The order here must match the VV_ defines in vim.h!
    // Initializing a union does not work, leave tv.vval empty to get zero's.
    {VV_NAME("count",		 VAR_NUMBER), عدم, VV_COMPAT+VV_RO},
    {VV_NAME("count1",		 VAR_NUMBER), عدم, VV_RO},
    {VV_NAME("prevcount",	 VAR_NUMBER), عدم, VV_RO},
    {VV_NAME("errmsg",		 VAR_STRING), عدم, VV_COMPAT},
    {VV_NAME("warningmsg",	 VAR_STRING), عدم, 0},
    {VV_NAME("statusmsg",	 VAR_STRING), عدم, 0},
    {VV_NAME("shell_error",	 VAR_NUMBER), عدم, VV_COMPAT+VV_RO},
    {VV_NAME("this_session",	 VAR_STRING), عدم, VV_COMPAT},
    {VV_NAME("version",		 VAR_NUMBER), عدم, VV_COMPAT+VV_RO},
    {VV_NAME("lnum",		 VAR_NUMBER), عدم, VV_RO_SBX},
    {VV_NAME("termresponse",	 VAR_STRING), عدم, VV_RO},
    {VV_NAME("fname",		 VAR_STRING), عدم, VV_RO},
    {VV_NAME("lang",		 VAR_STRING), عدم, VV_RO},
    {VV_NAME("lc_time",		 VAR_STRING), عدم, VV_RO},
    {VV_NAME("ctype",		 VAR_STRING), عدم, VV_RO},
    {VV_NAME("charconvert_from", VAR_STRING), عدم, VV_RO},
    {VV_NAME("charconvert_to",	 VAR_STRING), عدم, VV_RO},
    {VV_NAME("fname_in",	 VAR_STRING), عدم, VV_RO},
    {VV_NAME("fname_out",	 VAR_STRING), عدم, VV_RO},
    {VV_NAME("fname_new",	 VAR_STRING), عدم, VV_RO},
    {VV_NAME("fname_diff",	 VAR_STRING), عدم, VV_RO},
    {VV_NAME("cmdarg",		 VAR_STRING), عدم, VV_RO},
    {VV_NAME("foldstart",	 VAR_NUMBER), عدم, VV_RO_SBX},
    {VV_NAME("foldend",		 VAR_NUMBER), عدم, VV_RO_SBX},
    {VV_NAME("folddashes",	 VAR_STRING), عدم, VV_RO_SBX},
    {VV_NAME("foldlevel",	 VAR_NUMBER), عدم, VV_RO_SBX},
    {VV_NAME("progname",	 VAR_STRING), عدم, VV_RO},
    {VV_NAME("servername",	 VAR_STRING), عدم, VV_RO},
    {VV_NAME("dying",		 VAR_NUMBER), عدم, VV_RO},
    {VV_NAME("exception",	 VAR_STRING), عدم, VV_RO},
    {VV_NAME("throwpoint",	 VAR_STRING), عدم, VV_RO},
    {VV_NAME("register",	 VAR_STRING), عدم, VV_RO},
    {VV_NAME("cmdbang",		 VAR_NUMBER), عدم, VV_RO},
    {VV_NAME("insertmode",	 VAR_STRING), عدم, VV_RO},
    {VV_NAME("val",		 VAR_UNKNOWN), عدم, VV_RO},
    {VV_NAME("key",		 VAR_UNKNOWN), عدم, VV_RO},
    {VV_NAME("profiling",	 VAR_NUMBER), عدم, VV_RO},
    {VV_NAME("fcs_reason",	 VAR_STRING), عدم, VV_RO},
    {VV_NAME("fcs_choice",	 VAR_STRING), عدم, 0},
    {VV_NAME("beval_bufnr",	 VAR_NUMBER), عدم, VV_RO},
    {VV_NAME("beval_winnr",	 VAR_NUMBER), عدم, VV_RO},
    {VV_NAME("beval_winid",	 VAR_NUMBER), عدم, VV_RO},
    {VV_NAME("beval_lnum",	 VAR_NUMBER), عدم, VV_RO},
    {VV_NAME("beval_col",	 VAR_NUMBER), عدم, VV_RO},
    {VV_NAME("beval_text",	 VAR_STRING), عدم, VV_RO},
    {VV_NAME("scrollstart",	 VAR_STRING), عدم, 0},
    {VV_NAME("swapname",	 VAR_STRING), عدم, VV_RO},
    {VV_NAME("swapchoice",	 VAR_STRING), عدم, 0},
    {VV_NAME("swapcommand",	 VAR_STRING), عدم, VV_RO},
    {VV_NAME("char",		 VAR_STRING), عدم, 0},
    {VV_NAME("mouse_win",	 VAR_NUMBER), عدم, 0},
    {VV_NAME("mouse_winid",	 VAR_NUMBER), عدم, 0},
    {VV_NAME("mouse_lnum",	 VAR_NUMBER), عدم, 0},
    {VV_NAME("mouse_col",	 VAR_NUMBER), عدم, 0},
    {VV_NAME("operator",	 VAR_STRING), عدم, VV_RO},
    {VV_NAME("searchforward",	 VAR_NUMBER), عدم, 0},
    {VV_NAME("hlsearch",	 VAR_NUMBER), عدم, 0},
    {VV_NAME("oldfiles",	 VAR_LIST), &t_list_string, 0},
    {VV_NAME("windowid",	 VAR_NUMBER), عدم, VV_RO},
    {VV_NAME("progpath",	 VAR_STRING), عدم, VV_RO},
    {VV_NAME("completed_item",	 VAR_DICT), &t_dict_string, 0},
    {VV_NAME("option_new",	 VAR_STRING), عدم, VV_RO},
    {VV_NAME("option_old",	 VAR_STRING), عدم, VV_RO},
    {VV_NAME("option_oldlocal",	 VAR_STRING), عدم, VV_RO},
    {VV_NAME("option_oldglobal", VAR_STRING), عدم, VV_RO},
    {VV_NAME("option_command",	 VAR_STRING), عدم, VV_RO},
    {VV_NAME("option_type",	 VAR_STRING), عدم, VV_RO},
    {VV_NAME("errors",		 VAR_LIST), &t_list_string, 0},
    {VV_NAME("false",		 VAR_BOOL), عدم, VV_RO},
    {VV_NAME("true",		 VAR_BOOL), عدم, VV_RO},
    {VV_NAME("none",		 VAR_SPECIAL), عدم, VV_RO},
    {VV_NAME("null",		 VAR_SPECIAL), عدم, VV_RO},
    {VV_NAME("numbermax",	 VAR_NUMBER), عدم, VV_RO},
    {VV_NAME("numbermin",	 VAR_NUMBER), عدم, VV_RO},
    {VV_NAME("numbersize",	 VAR_NUMBER), عدم, VV_RO},
    {VV_NAME("vim_did_enter",	 VAR_NUMBER), عدم, VV_RO},
    {VV_NAME("testing",		 VAR_NUMBER), عدم, 0},
    {VV_NAME("t_number",	 VAR_NUMBER), عدم, VV_RO},
    {VV_NAME("t_string",	 VAR_NUMBER), عدم, VV_RO},
    {VV_NAME("t_func",		 VAR_NUMBER), عدم, VV_RO},
    {VV_NAME("t_list",		 VAR_NUMBER), عدم, VV_RO},
    {VV_NAME("t_dict",		 VAR_NUMBER), عدم, VV_RO},
    {VV_NAME("t_float",		 VAR_NUMBER), عدم, VV_RO},
    {VV_NAME("t_bool",		 VAR_NUMBER), عدم, VV_RO},
    {VV_NAME("t_none",		 VAR_NUMBER), عدم, VV_RO},
    {VV_NAME("t_job",		 VAR_NUMBER), عدم, VV_RO},
    {VV_NAME("t_channel",	 VAR_NUMBER), عدم, VV_RO},
    {VV_NAME("t_blob",		 VAR_NUMBER), عدم, VV_RO},
    {VV_NAME("t_class",		 VAR_NUMBER), عدم, VV_RO},
    {VV_NAME("t_object",	 VAR_NUMBER), عدم, VV_RO},
    {VV_NAME("termrfgresp",	 VAR_STRING), عدم, VV_RO},
    {VV_NAME("termrbgresp",	 VAR_STRING), عدم, VV_RO},
    {VV_NAME("termu7resp",	 VAR_STRING), عدم, VV_RO},
    {VV_NAME("termstyleresp",	 VAR_STRING), عدم, VV_RO},
    {VV_NAME("termblinkresp",	 VAR_STRING), عدم, VV_RO},
    {VV_NAME("event",		 VAR_DICT), عدم, VV_RO},
    {VV_NAME("versionlong",	 VAR_NUMBER), عدم, VV_RO},
    {VV_NAME("echospace",	 VAR_NUMBER), عدم, VV_RO},
    {VV_NAME("argv",		 VAR_LIST), &t_list_string, VV_RO},
    {VV_NAME("collate",		 VAR_STRING), عدم, VV_RO},
    {VV_NAME("exiting",		 VAR_SPECIAL), عدم, VV_RO},
    {VV_NAME("colornames",       VAR_DICT), &t_dict_string, VV_RO},
    {VV_NAME("sizeofint",	 VAR_NUMBER), عدم, VV_RO},
    {VV_NAME("sizeoflong",	 VAR_NUMBER), عدم, VV_RO},
    {VV_NAME("sizeofpointer",	 VAR_NUMBER), عدم, VV_RO},
    {VV_NAME("maxcol",		 VAR_NUMBER), عدم, VV_RO},
    {VV_NAME("python3_version",	 VAR_NUMBER), عدم, VV_RO},
    {VV_NAME("t_typealias",	 VAR_NUMBER), عدم, VV_RO},
    {VV_NAME("t_enum",		 VAR_NUMBER), عدم, VV_RO},
    {VV_NAME("t_enumvalue",	 VAR_NUMBER), عدم, VV_RO},
    {VV_NAME("stacktrace",	 VAR_LIST), &t_list_dict_any, VV_RO},
    {VV_NAME("t_tuple",		 VAR_NUMBER), عدم, VV_RO},
    {VV_NAME("wayland_display",  VAR_STRING), عدم, VV_RO},
    {VV_NAME("clipmethod",	 VAR_STRING), عدم, VV_RO},
    {VV_NAME("termda1",		 VAR_STRING), عدم, VV_RO},
    {VV_NAME("termosc",	 VAR_STRING), عدم, VV_RO},
    {VV_NAME("vim_did_init",	 VAR_NUMBER), عدم, VV_RO},
    {VV_NAME("clipproviders",	 VAR_DICT), عدم, VV_RO},
    {VV_NAME("imagebackend",	 VAR_STRING), عدم, VV_RO}
};

// shorthand
#define vv_tv_type	vv_di.di_tv.v_type
#define vv_nr		vv_di.di_tv.vval.v_number
#define vv_float	vv_di.di_tv.vval.v_float
#define vv_str		vv_di.di_tv.vval.v_string
#define vv_list		vv_di.di_tv.vval.v_list
#define vv_dict		vv_di.di_tv.vval.v_dict
#define vv_blob		vv_di.di_tv.vval.v_blob
#define vv_tv		vv_di.di_tv

ساكن dictitem_T	vimvars_var;		// variable used for v:
ساكن dict_T		vimvardict;		// Dictionary with v: variables
#define vimvarht  vimvardict.dv_hashtab

// for VIM_VERSION_ defines
#ضمن "version.h"

ساكن فراغ list_glob_vars(صحيح *first);
ساكن فراغ list_buf_vars(صحيح *first);
ساكن فراغ list_win_vars(صحيح *first);
ساكن فراغ list_tab_vars(صحيح *first);
ساكن char_u *list_arg_vars(exarg_T *eap, char_u *arg, صحيح *first);
ساكن char_u *ex_let_one(char_u *arg, typval_T *tv, صحيح copy, صحيح flags, char_u *endchars, char_u *op, صحيح var_idx);
ساكن صحيح do_unlet_var(lval_T *lp, char_u *name_end, exarg_T *eap, صحيح deep, فراغ *cookie);
ساكن صحيح do_lock_var(lval_T *lp, char_u *name_end, exarg_T *eap, صحيح deep, فراغ *cookie);
ساكن فراغ list_one_var(dictitem_T *v, حرف *prefix, صحيح *first);
ساكن فراغ list_one_var_a(حرف *prefix, char_u *name, صحيح type, char_u *string, صحيح *first);

/*
 * Initialize global and vim special variables
 */
    فراغ
evalvars_init(فراغ)
{
    صحيح		    i;
    بنية vimvar   *p;

    init_var_dict(&globvardict, &globvars_var, VAR_DEF_SCOPE);
    init_var_dict(&vimvardict, &vimvars_var, VAR_SCOPE);
    vimvardict.dv_lock = VAR_FIXED;
    hash_init(&compat_hashtab);

    עבור (i = 0; i < VV_LEN; ++i)
    {
	p = &vimvars[i];
	אם (p->vv_flags & VV_RO)
	    p->vv_di.di_flags = DI_FLAGS_RO | DI_FLAGS_FIX;
	אחרת אם (p->vv_flags & VV_RO_SBX)
	    p->vv_di.di_flags = DI_FLAGS_RO_SBX | DI_FLAGS_FIX;
	אחרת
	    p->vv_di.di_flags = DI_FLAGS_FIX;

	// add to v: scope dict, unless the value is not always available
	אם (p->vv_tv_type != VAR_UNKNOWN)
	    hash_add(&vimvarht, p->vv_di.di_key, "initialization");
	אם (p->vv_flags & VV_COMPAT)
	    // add to compat scope dict
	    hash_add(&compat_hashtab, p->vv_di.di_key, "initialization");
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
    set_vim_var_nr(VV_NUMBERSIZE, حجم(varnumber_T) * 8);
    set_vim_var_nr(VV_SIZEOFINT, حجم(صحيح));
    set_vim_var_nr(VV_SIZEOFLONG, حجم(long));
    set_vim_var_nr(VV_SIZEOFPOINTER, حجم(حرف *));
    set_vim_var_nr(VV_MAXCOL, MAXCOL);

    set_vim_var_nr(VV_TYPE_NUMBER,  VAR_TYPE_NUMBER);
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

#אם defined(EXITFREE)
/*
 * Free all vim variables information on exit
 */
    فراغ
evalvars_clear(فراغ)
{
    صحيح		    i;
    بنية vimvar   *p;

    עבור (i = 0; i < VV_LEN; ++i)
    {
	p = &vimvars[i];
	אם (p->vv_di.di_tv.v_type == VAR_STRING)
	    VIM_CLEAR(p->vv_str);
	אחרת אם (p->vv_di.di_tv.v_type == VAR_LIST)
	{
	    list_unref(p->vv_list);
	    p->vv_list = عدم;
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
    עבור (i = 1; i <= script_items.ga_len; ++i)
	vars_clear(&SCRIPT_VARS(i));
}
#endif

    صحيح
garbage_collect_globvars(صحيح copyID)
{
    החזר set_ref_in_ht(&globvarht, copyID, عدم, عدم);
}

    صحيح
garbage_collect_vimvars(صحيح copyID)
{
    החזר set_ref_in_ht(&vimvarht, copyID, عدم, عدم);
}

    صحيح
garbage_collect_scriptvars(صحيح copyID)
{
    صحيح		    i;
    صحيح		    idx;
    صحيح		    abort = FALSE;
    scriptitem_T    *si;

    עבור (i = 1; i <= script_items.ga_len; ++i)
    {
	abort = abort || set_ref_in_ht(&SCRIPT_VARS(i), copyID, عدم, عدم);

	si = SCRIPT_ITEM(i);
	עבור (idx = 0; idx < si->sn_var_vals.ga_len; ++idx)
	{
	    svar_T    *sv = ((svar_T *)si->sn_var_vals.ga_data) + idx;

	    אם (sv->sv_name != عدم)
		abort = abort || set_ref_in_item(sv->sv_tv, copyID, عدم, عدم, عدم);
	}
    }

    החזר abort;
}

/*
 * Set an internal variable to a string value. Creates the variable if it does
 * not already exist.
 */
    فراغ
set_internal_string_var(char_u *name, char_u *value)
{
    char_u	*val;
    typval_T	*tvp;

    val = vim_strsave(value);
    אם (val == عدم)
	החזר;

    tvp = alloc_string_tv(val);
    אם (tvp == عدم)
	החזר;

    set_var(name, tvp, FALSE);
    free_tv(tvp);
}

    صحيح
eval_charconvert(
    char_u	*enc_from,
    char_u	*enc_to,
    char_u	*fname_from,
    char_u	*fname_to)
{
    صحيح		err = FALSE;
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;

    set_vim_var_string(VV_CC_FROM, enc_from, -1);
    set_vim_var_string(VV_CC_TO, enc_to, -1);
    set_vim_var_string(VV_FNAME_IN, fname_from, -1);
    set_vim_var_string(VV_FNAME_OUT, fname_to, -1);
    ctx = get_option_sctx("charconvert");
    אם (ctx != عدم)
	current_sctx = *ctx;

    אם (eval_to_bool(p_ccv, &err, عدم, FALSE, TRUE))
	err = TRUE;

    set_vim_var_string(VV_CC_FROM, عدم, -1);
    set_vim_var_string(VV_CC_TO, عدم, -1);
    set_vim_var_string(VV_FNAME_IN, عدم, -1);
    set_vim_var_string(VV_FNAME_OUT, عدم, -1);
    current_sctx = saved_sctx;

    אם (err)
	החזר FAIL;
    החזר OK;
}

#אם defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
    صحيح
eval_printexpr(char_u *fname, char_u *args)
{
    صحيح		err = FALSE;
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;

    set_vim_var_string(VV_FNAME_IN, fname, -1);
    set_vim_var_string(VV_CMDARG, args, -1);
    ctx = get_option_sctx("printexpr");
    אם (ctx != عدم)
	current_sctx = *ctx;

    אם (eval_to_bool(p_pexpr, &err, عدم, FALSE, TRUE))
	err = TRUE;

    set_vim_var_string(VV_FNAME_IN, عدم, -1);
    set_vim_var_string(VV_CMDARG, عدم, -1);
    current_sctx = saved_sctx;

    אם (err)
    {
	mch_remove(fname);
	החזר FAIL;
    }
    החזר OK;
}
#endif

#אם defined(FEAT_DIFF)
    فراغ
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
    אם (ctx != عدم)
	current_sctx = *ctx;

    // errors are ignored
    tv = eval_expr_ext(p_dex, عدم, TRUE);
    free_tv(tv);

    set_vim_var_string(VV_FNAME_IN, عدم, -1);
    set_vim_var_string(VV_FNAME_NEW, عدم, -1);
    set_vim_var_string(VV_FNAME_OUT, عدم, -1);
    current_sctx = saved_sctx;
}

    فراغ
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
    אם (ctx != عدم)
	current_sctx = *ctx;

    // errors are ignored
    tv = eval_expr_ext(p_pex, عدم, TRUE);
    free_tv(tv);

    set_vim_var_string(VV_FNAME_IN, عدم, -1);
    set_vim_var_string(VV_FNAME_DIFF, عدم, -1);
    set_vim_var_string(VV_FNAME_OUT, عدم, -1);
    current_sctx = saved_sctx;
}
#endif

#אם defined(FEAT_SPELL)
/*
 * Evaluate an expression to a list with suggestions.
 * For the "expr:" part of 'spellsuggest'.
 * Returns NULL when there is an error.
 */
    list_T *
eval_spell_expr(char_u *badword, char_u *expr)
{
    typval_T	save_val;
    typval_T	rettv;
    list_T	*list = عدم;
    char_u	*p = skipwhite(expr);
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;
    صحيح		r;

    // Set "v:val" to the bad word.
    prepare_vimvar(VV_VAL, &save_val);
    set_vim_var_string(VV_VAL, badword, -1);
    אם (p_verbose == 0)
	++emsg_off;
    ctx = get_option_sctx("spellsuggest");
    אם (ctx != عدم)
	current_sctx = *ctx;

    r = may_call_simple_func(p, &rettv);
    אם (r == NOTDONE)
	r = eval1(&p, &rettv, &EVALARG_EVALUATE);
    אם (r == OK)
    {
	אם (rettv.v_type != VAR_LIST)
	    clear_tv(&rettv);
	אחרת
	    list = rettv.vval.v_list;
    }

    אם (p_verbose == 0)
	--emsg_off;
    clear_tv(get_vim_var_tv(VV_VAL));
    restore_vimvar(VV_VAL, &save_val);
    current_sctx = saved_sctx;

    החזר list;
}

/*
 * "list" is supposed to contain two items: a word and a number.  Return the
 * word in "pp" and the number as the return value.
 * Return -1 if anything isn't right.
 * Used to get the good word and score from the eval_spell_expr() result.
 */
    صحيح
get_spellword(list_T *list, char_u **pp)
{
    listitem_T	*li;

    li = list->lv_first;
    אם (li == عدم)
	החזר -1;
    *pp = tv_get_string(&li->li_tv);

    li = li->li_next;
    אם (li == عدم)
	החזר -1;
    החזר (صحيح)tv_get_number(&li->li_tv);
}
#endif

/*
 * Prepare v: variable "idx" to be used.
 * Save the current typeval in "save_tv" and clear it.
 * When not used yet add the variable to the v: hashtable.
 */
    فراغ
prepare_vimvar(صحيح idx, typval_T *save_tv)
{
    *save_tv = vimvars[idx].vv_tv;
    vimvars[idx].vv_str = عدم;  // don't free it now
    אם (vimvars[idx].vv_tv_type == VAR_UNKNOWN)
	hash_add(&vimvarht, vimvars[idx].vv_di.di_key, "prepare vimvar");
}

/*
 * Restore v: variable "idx" to typeval "save_tv".
 * Note that the v: variable must have been cleared already.
 * When no longer defined, remove the variable from the v: hashtable.
 */
    فراغ
restore_vimvar(صحيح idx, typval_T *save_tv)
{
    hashitem_T	*hi;

    vimvars[idx].vv_tv = *save_tv;
    אם (vimvars[idx].vv_tv_type != VAR_UNKNOWN)
	החזר;

    hi = hash_find(&vimvarht, vimvars[idx].vv_di.di_key);
    אם (HASHITEM_EMPTY(hi))
	internal_error("restore_vimvar()");
    אחרת
	hash_remove(&vimvarht, hi, "restore vimvar");
}

/*
 * List Vim variables.
 */
    ساكن فراغ
list_vim_vars(صحيح *first)
{
    list_hashtable_vars(&vimvarht, "v:", FALSE, first);
}

/*
 * List script-local variables, if there is a script.
 */
    ساكن فراغ
list_script_vars(صحيح *first)
{
    אם (SCRIPT_ID_VALID(current_sctx.sc_sid))
	list_hashtable_vars(&SCRIPT_VARS(current_sctx.sc_sid),
							   "s:", FALSE, first);
}

/*
 * Return TRUE if "name" starts with "g:", "w:", "t:" or "b:".
 * But only when an identifier character follows.
 */
    صحيح
is_scoped_variable(char_u *name)
{
    החזר vim_strchr((char_u *)"gwbt", name[0]) != عدم
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
eval_one_expr_in_str(char_u *p, garray_T *gap, صحيح evaluate)
{
    char_u	*block_start = skipwhite(p + 1);  // skip the opening {
    char_u	*block_end = block_start;
    char_u	*expr_val;

    אם (*block_start == NUL)
    {
	semsg(_(e_missing_close_curly_str), p);
	החזר عدم;
    }
    אם (skip_expr(&block_end, عدم) == FAIL)
	החזר عدم;
    block_end = skipwhite(block_end);
    אם (*block_end != '}')
    {
	semsg(_(e_missing_close_curly_str), p);
	החזר عدم;
    }
    אם (evaluate)
    {
	*block_end = NUL;
	expr_val = eval_to_string(block_start, FALSE, FALSE);
	*block_end = '}';
	אם (expr_val == عدم)
	    החזר عدم;
	ga_concat(gap, expr_val);
	vim_free(expr_val);
    }

    החזר block_end + 1;
}

/*
 * Evaluate all the Vim expressions {expr} in "str" and return the resulting
 * string in allocated memory.  "{{" is reduced to "{" and "}}" to "}".
 * Used for a heredoc assignment.
 * Returns NULL for an error.
 */
    ساكن char_u *
eval_all_expr_in_str(char_u *str)
{
    garray_T	ga;
    char_u	*p;

    ga_init2(&ga, 1, 80);
    p = str;

    כלעוד (*p != NUL)
    {
	char_u	*lit_start;
	صحيح	escaped_brace = FALSE;

	// Look for a block start.
	lit_start = p;
	כלעוד (*p != '{' && *p != '}' && *p != NUL)
	    ++p;

	אם (*p != NUL && *p == p[1])
	{
	    // Escaped brace, unescape and continue.
	    // Include the brace in the literal string.
	    ++p;
	    escaped_brace = TRUE;
	}
	אחרת אם (*p == '}')
	{
	    semsg(_(e_stray_closing_curly_str), str);
	    ga_clear(&ga);
	    החזר عدم;
	}

	// Append the literal part.
	ga_concat_len(&ga, lit_start, (size_t)(p - lit_start));

	אם (*p == NUL)
	    اكسر;

	אם (escaped_brace)
	{
	    // Skip the second brace.
	    ++p;
	    استمر;
	}

	// Evaluate the expression and append the result.
	p = eval_one_expr_in_str(p, &ga, TRUE);
	אם (p == عدم)
	{
	    ga_clear(&ga);
	    החזר عدم;
	}
    }
    ga_append(&ga, NUL);

    החזר ga.ga_data;
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
heredoc_get(exarg_T *eap, char_u *cmd, صحيح script_get, صحيح vim9compile)
{
    char_u	*theline = عدم;
    char_u	*marker;
    list_T	*l;
    char_u	*p;
    char_u	*str;
    صحيح		marker_indent_len = 0;
    صحيح		text_indent_len = 0;
    char_u	*text_indent = عدم;
    char_u	dot[] = ".";
    صحيح		comment_char = in_vim9script() ? '#' : '"';
    صحيح		evalstr = FALSE;
    صحيح		eval_failed = FALSE;
    cctx_T	*cctx = vim9compile ? eap->cookie : عدم;
    صحيح		count = 0;
    صحيح		heredoc_in_string = FALSE;
    char_u	*line_arg = عدم;
    char_u	*nl_ptr = vim_strchr(cmd, '\n');

    אם (nl_ptr != عدم)
    {
	heredoc_in_string = TRUE;
	line_arg = nl_ptr + 1;
	*nl_ptr = NUL;
    }
    אחרת אם (eap->ea_getline == عدم)
    {
	emsg(_(e_cannot_use_heredoc_here));
	החזר عدم;
    }

    // Check for the optional 'trim' word before the marker
    cmd = skipwhite(cmd);

    כלעוד (TRUE)
    {
	אם (STRNCMP(cmd, "trim", 4) == 0
		&& (cmd[4] == NUL || VIM_ISWHITE(cmd[4])))
	{
	    cmd = skipwhite(cmd + 4);

	    // Trim the indentation from all the lines in the here document.
	    // The amount of indentation trimmed is the same as the indentation
	    // of the first line after the :let command line.  To find the end
	    // marker the indent of the :let command line is trimmed.
	    p = *eap->cmdlinep;
	    כלעוד (VIM_ISWHITE(*p))
	    {
		p++;
		marker_indent_len++;
	    }
	    text_indent_len = -1;

	    استمر;
	}
	אם (STRNCMP(cmd, "eval", 4) == 0
		&& (cmd[4] == NUL || VIM_ISWHITE(cmd[4])))
	{
	    cmd = skipwhite(cmd + 4);
	    evalstr = TRUE;
	    استمر;
	}
	اكسر;
    }

    // The marker is the next word.
    אם (*cmd != NUL && *cmd != comment_char)
    {
	marker = skipwhite(cmd);
	p = skiptowhite(marker);
	אם (*skipwhite(p) != NUL && *skipwhite(p) != comment_char)
	{
	    semsg(_(e_trailing_characters_str), p);
	    החזר عدم;
	}
	*p = NUL;
	אם (!script_get && vim_islower(*marker))
	{
	    emsg(_(e_marker_cannot_start_with_lower_case_letter));
	    החזר عدم;
	}
    }
    אחרת
    {
	// When getting lines for an embedded script, if the marker is missing,
	// accept '.' as the marker.
	אם (script_get)
	    marker = dot;
	אחרת
	{
	    emsg(_(e_missing_marker));
	    החזר عدم;
	}
    }

    l = list_alloc();
    אם (l == عدم)
	החזר عدم;

    עבור (;;)
    {
	صحيح	mi = 0;
	صحيح	ti = 0;

	אם (heredoc_in_string)
	{
	    char_u	*next_line;

	    // heredoc in a string separated by newlines.  Get the next line
	    // from the string.

	    אם (*line_arg == NUL)
	    {
		semsg(_(e_missing_end_marker_str), marker);
		اكسر;
	    }

	    theline = line_arg;
	    next_line = vim_strchr(theline, '\n');
	    אם (next_line == عدم)
		line_arg += STRLEN(line_arg);
	    אחרת
	    {
		*next_line = NUL;
		line_arg = next_line + 1;
	    }
	}
	אחרת
	{
	    vim_free(theline);
	    theline = eap->ea_getline(NUL, eap->cookie, 0, FALSE);
	    אם (theline == عدم)
	    {
		semsg(_(e_missing_end_marker_str), marker);
		اكسر;
	    }
	}

	// with "trim": skip the indent matching the :let line to find the
	// marker
	אם (marker_indent_len > 0
		&& STRNCMP(theline, *eap->cmdlinep, marker_indent_len) == 0)
	    mi = marker_indent_len;
	אם (STRCMP(marker, theline + mi) == 0)
	    اكسر;

	// If expression evaluation failed in the heredoc, then skip till the
	// end marker.
	אם (eval_failed)
	    استمر;

	אם (text_indent_len == -1 && *theline != NUL)
	{
	    // set the text indent from the first line.
	    p = theline;
	    text_indent_len = 0;
	    כלעוד (VIM_ISWHITE(*p))
	    {
		p++;
		text_indent_len++;
	    }
	    text_indent = vim_strnsave(theline, text_indent_len);
	}
	// with "trim": skip the indent matching the first line
	אם (text_indent != عدم)
	    עבור (ti = 0; ti < text_indent_len; ++ti)
		אם (theline[ti] != text_indent[ti])
		    اكسر;

	str = theline + ti;
	אם (vim9compile)
	{
	    אם (compile_all_expr_in_str(str, evalstr, cctx) == FAIL)
	    {
		vim_free(theline);
		vim_free(text_indent);
		list_free(l);
		החזר FAIL;
	    }
	    count++;
	}
	אחרת
	{
	    صحيح	    free_str = FALSE;

	    אם (evalstr && !eap->skip)
	    {
		str = eval_all_expr_in_str(str);
		אם (str == عدم)
		{
		    // expression evaluation failed
		    eval_failed = TRUE;
		    استمر;
		}
		free_str = TRUE;
	    }

	    אם (list_append_string(l, str, -1) == FAIL)
	    {
		אם (free_str)
		    vim_free(str);
		اكسر;
	    }
	    אם (free_str)
		vim_free(str);
	}
    }
    אם (heredoc_in_string)
	// Next command follows the heredoc in the string.
	eap->nextcmd = line_arg;
    אחרת
	vim_free(theline);
    vim_free(text_indent);

    אם (vim9compile && cctx->ctx_skip != SKIP_YES && !eval_failed)
	generate_NEWLIST(cctx, count, FALSE);

    אם (eval_failed)
    {
	// expression evaluation in the heredoc failed
	list_free(l);
	החזר عدم;
    }
    החזר l;
}

/*
 * Vim9 variable declaration:
 * ":var name"
 * ":var name: type"
 * ":var name = expr"
 * ":var name: type = expr"
 * etc.
 */
    فراغ
ex_var(exarg_T *eap)
{
    char_u *p = eap->cmd;
    صحيح	    has_var;

    אם (!in_vim9script())
    {
	semsg(_(e_str_cannot_be_used_in_legacy_vim_script), ":var");
	החזר;
    }
    has_var = checkforcmd_noparen(&p, "var", 3);
    אם (current_sctx.sc_sid == 0 && has_var)
    {
	emsg(_(e_cannot_declare_variable_on_command_line));
	החזר;
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
    فراغ
ex_let(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    char_u	*expr = عدم;
    typval_T	rettv;
    صحيح		var_count = 0;
    صحيح		semicolon = 0;
    char_u	op[4];
    char_u	*argend;
    صحيح		first = TRUE;
    صحيح		concat;
    صحيح		has_assign;
    صحيح		flags = 0;
    صحيح		vim9script = in_vim9script();

    אם (eap->cmdidx == CMD_final && !vim9script)
    {
	// In legacy Vim script ":final" is short for ":finally".
	ex_finally(eap);
	החזר;
    }
    אם (eap->cmdidx == CMD_let && vim9script)
    {
	emsg(_(e_cannot_use_let_in_vim9_script));
	החזר;
    }

    אם (eap->cmdidx == CMD_const)
	flags |= ASSIGN_CONST;
    אחרת אם (eap->cmdidx == CMD_final)
	flags |= ASSIGN_FINAL;

    // Vim9 assignment without ":let", ":const" or ":final"
    אם (eap->arg == eap->cmd)
	flags |= ASSIGN_NO_DECL;

    argend = skip_var_list(arg, TRUE, &var_count, &semicolon, FALSE);
    אם (argend == عدم)
	החזר;
    expr = skipwhite(argend);
    concat = expr[0] == '.'
	&& ((expr[1] == '=' && in_old_script(2))
		|| (expr[1] == '.' && expr[2] == '='));
    has_assign =  *expr == '=' || (vim_strchr((char_u *)"+-*/%", *expr) != عدم
							    && expr[1] == '=');
    אם (!has_assign && !concat)
    {
	// ":let" without "=": list variables
	אם (*arg == '[')
	    emsg(_(e_invalid_argument));
	אחרת אם (expr[0] == '.' && expr[1] == '=')
	    emsg(_(e_dot_equal_not_supported_with_script_version_two));
	אחרת אם (!ends_excmd2(eap->cmd, arg))
	{
	    אם (vim9script)
	    {
		אם (!ends_excmd2(eap->cmd, skipwhite(argend)))
		    semsg(_(e_trailing_characters_str), argend);
		אחרת
		    // Vim9 declaration ":var name: type"
		    arg = vim9_declare_scriptvar(eap, arg);
	    }
	    אחרת
	    {
		// ":let var1 var2" - list values
		arg = list_arg_vars(eap, arg, &first);
	    }
	}
	אחרת אם (!eap->skip)
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
	החזר;
    }

    אם (expr[0] == '=' && expr[1] == '<' && expr[2] == '<')
    {
	list_T	*l = عدم;
	long	cur_lnum = SOURCING_LNUM;

	// :let text =<< [trim] [eval] END
	// :var text =<< [trim] [eval] END
	אם (vim9script && !eap->skip && (!VIM_ISWHITE(expr[-1])
						 || !IS_WHITE_OR_NUL(expr[3])))
	    semsg(_(e_white_space_required_before_and_after_str_at_str),
								  "=<<", expr);
	אחרת
	    l = heredoc_get(eap, expr + 3, FALSE, FALSE);

	אם (l != عدم)
	{
	    rettv_list_set(&rettv, l);
	    אם (!eap->skip)
	    {
		// errors are for the assignment, not the end marker
		SOURCING_LNUM = cur_lnum;
		op[0] = '=';
		op[1] = NUL;
		(فراغ)ex_let_vars(eap->arg, &rettv, FALSE, semicolon, var_count,
								flags, op);
	    }
	    clear_tv(&rettv);
	}
	החזר;
    }

    evalarg_T   evalarg;
    صحيح	    len = 1;

    CLEAR_FIELD(rettv);

    صحيح cur_lnum;

    op[0] = '=';
    op[1] = NUL;
    אם (*expr != '=')
    {
	אם (vim9script && (flags & ASSIGN_NO_DECL) == 0)
	{
	    // +=, /=, etc. require an existing variable
	    semsg(_(e_cannot_use_operator_on_new_variable_str), eap->arg);
	}
	אחרת אם (vim_strchr((char_u *)"+-*/%.", *expr) != عدم)
	{
	    op[0] = *expr;   // +=, -=, *=, /=, %= or .=
	    ++len;
	    אם (expr[0] == '.' && expr[1] == '.') // ..=
	    {
		++expr;
		++len;
	    }
	}
	expr += 2;
    }
    אחרת
	++expr;

    אם (vim9script && !eap->skip && (!VIM_ISWHITE(*argend)
					       || !IS_WHITE_OR_NUL(*expr)))
    {
	vim_strncpy(op, expr - len, len);
	semsg(_(e_white_space_required_before_and_after_str_at_str),
							       op, argend);
    }

    אם (eap->skip)
	++emsg_skip;
    fill_evalarg_from_eap(&evalarg, eap, eap->skip);
    expr = skipwhite_and_linebreak(expr, &evalarg);
    cur_lnum = SOURCING_LNUM;
    صحيح eval_res = eval0(expr, &rettv, eap, &evalarg);
    אם (eap->skip)
	--emsg_skip;
    clear_evalarg(&evalarg, eap);

    // Restore the line number so that any type error is given for the
    // declaration, not the expression.
    SOURCING_LNUM = cur_lnum;

    אם (!eap->skip && eval_res != FAIL)
	(فراغ)ex_let_vars(eap->arg, &rettv, FALSE, semicolon, var_count,
								    flags, op);
    אם (eval_res != FAIL)
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
    صحيح
ex_let_vars(
    char_u	*arg_start,
    typval_T	*tv,
    صحيح		copy,		// copy values from "tv", don't move
    صحيح		semicolon,	// from skip_var_list()
    صحيح		var_count,	// from skip_var_list()
    صحيح		flags,		// ASSIGN_FINAL, ASSIGN_CONST, etc.
    char_u	*op)
{
    char_u	*arg = arg_start;
    list_T	*l;
    tuple_T	*tuple = عدم;
    صحيح		i;
    صحيح		var_idx = 0;
    listitem_T	*item = عدم;
    typval_T	ltv;
    صحيح		is_list = tv->v_type == VAR_LIST;
    صحيح		idx;

    אם (tv->v_type == VAR_VOID)
    {
	emsg(_(e_cannot_use_void_value));
	החזר FAIL;
    }
    אם (*arg != '[')
    {
	// ":let var = expr" or ":for var in list"
	אם (ex_let_one(arg, tv, copy, flags, op, op, var_idx) == عدم)
	    החזר FAIL;
	החזר OK;
    }

    // ":let [v1, v2] = list" or ":for [v1, v2] in listlist"
    // or
    // ":let [v1, v2] = tuple" or ":for [v1, v2] in tupletuple"
    אם (tv->v_type != VAR_LIST && tv->v_type != VAR_TUPLE)
    {
	emsg(_(e_list_or_tuple_required));
	החזר FAIL;
    }
    אם (is_list)
    {
	l = tv->vval.v_list;
	אם (l == عدم)
	{
	    emsg(_(e_list_required));
	    החזר FAIL;
	}
	i = list_len(l);
    }
    אחרת
    {
	tuple = tv->vval.v_tuple;
	אם (tuple == عدم)
	{
	    emsg(_(e_tuple_required));
	    החזר FAIL;
	}
	i = tuple_len(tuple);
    }

    אם (semicolon == 0 && var_count < i)
    {
	emsg(_(is_list ? e_less_targets_than_list_items
					: e_less_targets_than_tuple_items));
	החזר FAIL;
    }
    אם (var_count - semicolon > i)
    {
	emsg(_(is_list ? e_more_targets_than_list_items
					: e_more_targets_than_tuple_items));
	החזר FAIL;
    }

    אם (is_list)
    {
	CHECK_LIST_MATERIALIZE(l);
	item = l->lv_first;
    }
    אחרת
	idx = 0;

    כלעוד (*arg != ']')
    {
	arg = skipwhite(arg + 1);
	++var_idx;
	arg = ex_let_one(arg, is_list ? &item->li_tv : TUPLE_ITEM(tuple, idx),
			 TRUE, flags | ASSIGN_UNPACK, (char_u *)",;]", op,
			 var_idx);
	אם (is_list)
	    item = item->li_next;
	אחרת
	    idx++;
	אם (arg == عدم)
	    החזר FAIL;

	arg = skipwhite(arg);
	אם (*arg == ';')
	{
	    // Put the rest of the list or tuple (may be empty) in the var
	    // after ';'.  Create a new list or tuple for this.
	    אם (is_list)
	    {
		// Put the rest of the list (may be empty) in the var
		// after ';'.  Create a new list for this.
		l = list_alloc();
		אם (l == عدم)
		    החזר FAIL;

		// list
		כלעוד (item != عدم)
		{
		    list_append_tv(l, &item->li_tv);
		    item = item->li_next;
		}

		ltv.v_type = VAR_LIST;
		ltv.v_lock = 0;
		ltv.vval.v_list = l;
		l->lv_refcount = 1;
	    }
	    אחרת
	    {
		tuple_T	*new_tuple = tuple_alloc();
		אם (new_tuple == عدم)
		    החזר FAIL;

		// Put the rest of the tuple (may be empty) in the var
		// after ';'.  Create a new tuple for this.
		כלעוד (idx < TUPLE_LEN(tuple))
		{
		    typval_T    new_tv;

		    copy_tv(TUPLE_ITEM(tuple, idx), &new_tv);
		    אם (tuple_append_tv(new_tuple, &new_tv) == FAIL)
		    {
			clear_tv(&new_tv);
			tuple_unref(new_tuple);
			החזר FAIL;
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
	    אם (arg == عدم)
		החזר FAIL;
	    اكسر;
	}
	אחרת אם (*arg != ',' && *arg != ']')
	{
	    internal_error("ex_let_vars()");
	    החזר FAIL;
	}
    }

    החזר OK;
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
    صحيح		include_type,
    صحيح		*var_count,
    صحيح		*semicolon,
    صحيح		silent)
{
    char_u	*p, *s;

    אם (*arg == '[')
    {
	// "[var, var]": find the matching ']'.
	p = arg;
	עבור (;;)
	{
	    p = skipwhite(p + 1);	// skip whites after '[', ';' or ','
	    s = skip_var_one(p, include_type);
	    אם (s == p)
	    {
		אם (!silent)
		    semsg(_(e_invalid_argument_str), p);
		החזר عدم;
	    }
	    ++*var_count;

	    p = skipwhite(s);
	    אם (*p == ']')
		اكسر;
	    אחרת אם (*p == ';')
	    {
		אם (*semicolon == 1)
		{
		    אם (!silent)
			emsg(_(e_double_semicolon_in_list_of_variables));
		    החזר عدم;
		}
		*semicolon = 1;
	    }
	    אחרת אם (*p != ',')
	    {
		אם (!silent)
		    semsg(_(e_invalid_argument_str), p);
		החזר عدم;
	    }
	}
	החזר p + 1;
    }

    החזר skip_var_one(arg, include_type);
}

/*
 * Skip one (assignable) variable name, including @r, $VAR, &option, d.key,
 * l[idx].
 * In Vim9 script also skip over ": type" if "include_type" is TRUE.
 */
    char_u *
skip_var_one(char_u *arg, صحيح include_type)
{
    char_u	*end;
    صحيح		vim9 = in_vim9script();

    אם (*arg == '@' && arg[1] != NUL)
	החזר arg + 2;

    // termcap option name may have non-alpha characters
    אם (STRNCMP(arg, "&t_", 3) == 0 && arg[3] != NUL && arg[4] != NUL)
	החזר arg + 5;

    end = find_name_end(*arg == '$' || *arg == '&' ? arg + 1 : arg,
				   عدم, عدم, FNE_INCL_BR | FNE_CHECK_START);

    // "a: type" is declaring variable "a" with a type, not "a:".
    // Same for "s: type".
    אם (vim9 && end == arg + 2 && end[-1] == ':')
	--end;

    אם (include_type && vim9)
    {
	אם (*skipwhite(end) == ':')
	    end = skip_type(skipwhite(skipwhite(end) + 1), FALSE);
    }
    החזר end;
}

/*
 * List variables for hashtab "ht" with prefix "prefix".
 * If "empty" is TRUE also list NULL strings as empty strings.
 */
    فراغ
list_hashtable_vars(
    hashtab_T	*ht,
    حرف	*prefix,
    صحيح		empty,
    صحيح		*first)
{
    hashitem_T	*hi;
    dictitem_T	*di;
    صحيح		todo;
    char_u	buf[IOSIZE];

    صحيح save_ht_flags = ht->ht_flags;
    ht->ht_flags |= HTFLAGS_FROZEN;

    todo = (صحيح)ht->ht_used;
    עבור (hi = ht->ht_array; todo > 0 && !got_int; ++hi)
    {
	אם (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    di = HI2DI(hi);

	    // apply :filter /pat/ to variable name
	    vim_strncpy((char_u *)buf, (char_u *)prefix, IOSIZE - 1);
	    vim_strcat((char_u *)buf, di->di_key, IOSIZE);
	    אם (message_filtered(buf))
		استمر;

	    אם (empty || di->di_tv.v_type != VAR_STRING
					   || di->di_tv.vval.v_string != عدم)
		list_one_var(di, prefix, first);
	}
    }

    ht->ht_flags = save_ht_flags;
}

/*
 * List global variables.
 */
    ساكن فراغ
list_glob_vars(صحيح *first)
{
    list_hashtable_vars(&globvarht, "", TRUE, first);
}

/*
 * List buffer variables.
 */
    ساكن فراغ
list_buf_vars(صحيح *first)
{
    list_hashtable_vars(&curbuf->b_vars->dv_hashtab, "b:", TRUE, first);
}

/*
 * List window variables.
 */
    ساكن فراغ
list_win_vars(صحيح *first)
{
    list_hashtable_vars(&curwin->w_vars->dv_hashtab, "w:", TRUE, first);
}

/*
 * List tab page variables.
 */
    ساكن فراغ
list_tab_vars(صحيح *first)
{
    list_hashtable_vars(&curtab->tp_vars->dv_hashtab, "t:", TRUE, first);
}

/*
 * List variables in "arg".
 */
    ساكن char_u *
list_arg_vars(exarg_T *eap, char_u *arg, صحيح *first)
{
    صحيح		error = FALSE;
    صحيح		len;
    char_u	*name;
    char_u	*name_start;
    char_u	*arg_subsc;
    char_u	*tofree;
    typval_T    tv;

    כלעוד (!ends_excmd2(eap->cmd, arg) && !got_int)
    {
	אם (error || eap->skip)
	{
	    arg = find_name_end(arg, عدم, عدم, FNE_INCL_BR | FNE_CHECK_START);
	    אם (!VIM_ISWHITE(*arg) && !ends_excmd(*arg))
	    {
		emsg_severe = TRUE;
		אם (!did_emsg)
		    semsg(_(e_trailing_characters_str), arg);
		اكسر;
	    }
	}
	אחרת
	{
	    // get_name_len() takes care of expanding curly braces
	    name_start = name = arg;
	    len = get_name_len(&arg, &tofree, TRUE, TRUE);
	    אם (len <= 0)
	    {
		// This is mainly to keep test 49 working: when expanding
		// curly braces fails overrule the exception error message.
		אם (len < 0 && !aborting())
		{
		    emsg_severe = TRUE;
		    semsg(_(e_invalid_argument_str), arg);
		    اكسر;
		}
		error = TRUE;
	    }
	    אחרת
	    {
		arg = skipwhite(arg);
		אם (tofree != عدم)
		    name = tofree;
		אם (eval_variable(name, len, 0, &tv, عدم,
						     EVAL_VAR_VERBOSE) == FAIL)
		    error = TRUE;
		אחרת
		{
		    // handle d.key, l[idx], f(expr)
		    arg_subsc = arg;
		    אם (handle_subscript(&arg, name_start, &tv,
					      &EVALARG_EVALUATE, TRUE) == FAIL)
			error = TRUE;
		    אחרת
		    {
			אם (arg == arg_subsc && len == 2 && name[1] == ':')
			{
			    بدل (*name)
			    {
				حالة 'g': list_glob_vars(first); اكسر;
				حالة 'b': list_buf_vars(first); اكسر;
				حالة 'w': list_win_vars(first); اكسر;
				حالة 't': list_tab_vars(first); اكسر;
				حالة 'v': list_vim_vars(first); اكسر;
				حالة 's': list_script_vars(first); اكسر;
				حالة 'l': list_func_vars(first); اكسر;
				افتراضي:
					  semsg(_(e_cant_list_variables_for_str), name);
			    }
			}
			אחרת
			{
			    char_u	numbuf[NUMBUFLEN];
			    char_u	*tf;
			    صحيح		c;
			    char_u	*s;

			    s = echo_string(&tv, &tf, numbuf, 0);
			    c = *arg;
			    *arg = NUL;
			    list_one_var_a("",
				    arg == arg_subsc ? name : name_start,
				    tv.v_type,
				    s == عدم ? (char_u *)"" : s,
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

    החזר arg;
}

/*
 * Set an environment variable, part of ex_let_one().
 */
    ساكن char_u *
ex_let_env(
    char_u	*arg,
    typval_T	*tv,
    صحيح		flags,
    char_u	*endchars,
    char_u	*op)
{
    char_u	*arg_end = عدم;
    char_u	*name;
    صحيح		len;

    אם ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					 && (flags & ASSIGN_FOR_LOOP) == 0)
    {
	emsg(_(e_cannot_lock_environment_variable));
	החזר عدم;
    }

    // Find the end of the name.
    ++arg;
    name = arg;
    len = get_env_len(&arg);
    אם (len == 0)
	semsg(_(e_invalid_argument_str), name - 1);
    אחרת
    {
	אם (op != عدم && vim_strchr((char_u *)"+-*/%", *op) != عدم)
	    semsg(_(e_wrong_variable_type_for_str_equal), op);
	אחרת אם (endchars != عدم
			      && vim_strchr(endchars, *skipwhite(arg)) == عدم)
	    emsg(_(e_unexpected_characters_in_let));
	אחרת אם (!check_secure() && !check_restricted())
	{
	    char_u	*tofree = عدم;
	    صحيح		c1 = name[len];
	    char_u	*p;

	    name[len] = NUL;
	    p = tv_get_string_chk(tv);
	    אם (p != عدم && op != عدم && *op == '.')
	    {
		صحيح	mustfree = FALSE;
		char_u  *s = vim_getenv(name, &mustfree);

		אם (s != عدم)
		{
		    p = tofree = concat_str(s, p);
		    אם (mustfree)
			vim_free(s);
		}
	    }
	    אם (p != عدم)
	    {
		vim_setenv_ext(name, p);
		arg_end = arg;
	    }
	    name[len] = c1;
	    vim_free(tofree);
	}
    }
    החזר arg_end;
}

/*
 * Set an option, part of ex_let_one().
 */
    ساكن char_u *
ex_let_option(
    char_u	*arg,
    typval_T	*tv,
    صحيح		flags,
    char_u	*endchars,
    char_u	*op)
{
    char_u	*p;
    صحيح		scope;
    char_u	*arg_end = عدم;

    אם ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					 && (flags & ASSIGN_FOR_LOOP) == 0)
    {
	emsg(_(e_cannot_lock_option));
	החזר عدم;
    }

    // Find the end of the name.
    p = find_option_end(&arg, &scope);
    אם (p == عدم || (endchars != عدم
			       && vim_strchr(endchars, *skipwhite(p)) == عدم))
    {
	emsg(_(e_unexpected_characters_in_let));
	החזר عدم;
    }

    صحيح		c1;
    long	n = 0;
    getoption_T	opt_type;
    long	numval;
    char_u	*stringval = عدم;
    char_u	*s = عدم;
    صحيح		failed = FALSE;
    صحيح		opt_p_flags;
    char_u	*tofree = عدم;
    char_u	numbuf[NUMBUFLEN];

    c1 = *p;
    *p = NUL;

    opt_type = get_option_value(arg, &numval, &stringval, &opt_p_flags, scope);
    אם (opt_type == gov_unknown && arg[0] != 't' && arg[1] != '_')
    {
	semsg(_(e_unknown_option_str_2), arg);
	اذهب theend;
    }
    אם (op != عدم && *op != '='
	&& (((opt_type == gov_bool || opt_type == gov_number) && *op == '.')
	    || (opt_type == gov_string && *op != '.')))
    {
	semsg(_(e_wrong_variable_type_for_str_equal), op);
	اذهب theend;
    }

    אם ((opt_type == gov_bool
		|| opt_type == gov_number
		|| opt_type == gov_hidden_bool
		|| opt_type == gov_hidden_number)
		     && (tv->v_type != VAR_STRING || !in_vim9script()))
    {
	אם (opt_type == gov_bool || opt_type == gov_hidden_bool)
	    // bool, possibly hidden
	    n = (long)tv_get_bool_chk(tv, &failed);
	אחרת
	    // number, possibly hidden
	    n = (long)tv_get_number_chk(tv, &failed);
	אם (failed)
	    اذهب theend;
    }

    אם ((opt_p_flags & P_FUNC) && (tv->v_type == VAR_PARTIAL
						    || tv->v_type == VAR_FUNC))
    {
	// If the option can be set to a function reference or a lambda
	// and the passed value is a function reference, then convert it to
	// the name (string) of the function reference.
	s = tv2string(tv, &tofree, numbuf, 0);
	אם (s == عدم)
	    اذهب theend;
    }
    // Avoid setting a string option to the text "v:false" or similar.
    // In Vim9 script also don't convert a number to string.
    אחרת אם (tv->v_type != VAR_BOOL && tv->v_type != VAR_SPECIAL
		     && (!in_vim9script() || tv->v_type != VAR_NUMBER))
    {
	s = tv_get_string_chk(tv);
	אם (s == عدم)
	    اذهب theend;
    }
    אחרת אם (opt_type == gov_string || opt_type == gov_hidden_string)
    {
	emsg(_(e_string_required));
	اذهب theend;
    }

    אם (op != عدم && *op != '=')
    {
	// number, in legacy script also bool
	אם (opt_type == gov_number
				 || (opt_type == gov_bool && !in_vim9script()))
	{
	    بدل (*op)
	    {
		حالة '+': n = numval + n; اكسر;
		حالة '-': n = numval - n; اكسر;
		حالة '*': n = numval * n; اكسر;
		حالة '/': n = (long)num_divide(numval, n, &failed); اكسر;
		حالة '%': n = (long)num_modulus(numval, n, &failed); اكسر;
	    }
	    s = عدم;
	    אם (failed)
		اذهب theend;
	}
	אחרת אם (opt_type == gov_string && stringval != عدم && s != عدم)
	{
	    // string
	    s = concat_str(stringval, s);
	    vim_free(stringval);
	    stringval = s;
	}
    }

    حرف *err = set_option_value(arg, n, s, scope);
    arg_end = p;
    אם (err != عدم)
	emsg(_(err));

theend:
    *p = c1;
    vim_free(stringval);
    vim_free(tofree);
    החזר arg_end;
}

/*
 * Set a register, part of ex_let_one().
 */
    ساكن char_u *
ex_let_register(
    char_u	*arg,
    typval_T	*tv,
    صحيح		flags,
    char_u	*endchars,
    char_u	*op)
{
    char_u	*arg_end = عدم;

    אם ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					 && (flags & ASSIGN_FOR_LOOP) == 0)
    {
	emsg(_(e_cannot_lock_register));
	החזר عدم;
    }
    ++arg;
    אם (op != عدم && vim_strchr((char_u *)"+-*/%", *op) != عدم)
	semsg(_(e_wrong_variable_type_for_str_equal), op);
    אחרת אם (endchars != عدم
			  && vim_strchr(endchars, *skipwhite(arg + 1)) == عدم)
	emsg(_(e_unexpected_characters_in_let));
    אחרת
    {
	char_u	*ptofree = عدم;
	char_u	*p;

	p = tv_get_string_chk(tv);
	אם (p != عدم && op != عدم && *op == '.')
	{
	    char_u  *s = get_reg_contents(*arg == '@'
						  ? '"' : *arg, GREG_EXPR_SRC);

	    אם (s != عدم)
	    {
		p = ptofree = concat_str(s, p);
		vim_free(s);
	    }
	}
	אם (p != عدم)
	{
	    write_reg_contents(*arg == '@' ? '"' : *arg, p, -1, FALSE);
	    arg_end = arg + 1;
	}
	vim_free(ptofree);
    }
    החזר arg_end;
}

/*
 * Set one item of ":let var = expr" or ":let [v1, v2] = list" to its value.
 * Returns a pointer to the char just after the var name.
 * Returns NULL if there is an error.
 */
    ساكن char_u *
ex_let_one(
    char_u	*arg,		// points to variable name
    typval_T	*tv,		// value to assign to variable
    صحيح		copy,		// copy value from "tv"
    صحيح		flags,		// ASSIGN_CONST, ASSIGN_FINAL, etc.
    char_u	*endchars,	// valid chars after variable name  or NULL
    char_u	*op,		// "+", "-", "."  or NULL
    صحيح		var_idx)	// variable index for "let [a, b] = list"
{
    char_u	*arg_end = عدم;

    אם (in_vim9script() && (flags & (ASSIGN_NO_DECL | ASSIGN_DECL)) == 0
			&& (flags & (ASSIGN_CONST | ASSIGN_FINAL)) == 0
				  && vim_strchr((char_u *)"$@&", *arg) != عدم)
    {
	vim9_declare_error(arg);
	החזר عدم;
    }

    אם (check_typval_is_value(tv) == FAIL)
	החזר عدم;

    // In Vim9 script an environment variable and a register only take a
    // String.  "@#" also takes a buffer number.
    אם (in_vim9script() && (*arg == '$' || *arg == '@')
	    && !(*arg == '@' && arg[1] == '#' && tv->v_type == VAR_NUMBER
					       && (op == عدم || *op == '=')))
    {
	where_T	where = WHERE_INIT;

	אם (check_typval_type(&t_string, tv, where) == FAIL)
	    החזר عدم;
    }

    אם (*arg == '$')
    {
	// ":let $VAR = expr": Set environment variable.
	החזר ex_let_env(arg, tv, flags, endchars, op);
    }
    אחרת אם (*arg == '&')
    {
	// ":let &option = expr": Set option value.
	// ":let &l:option = expr": Set local option value.
	// ":let &g:option = expr": Set global option value.
	// ":for &ts in range(8)": Set option value for for loop
	החזר ex_let_option(arg, tv, flags, endchars, op);
    }
    אחרת אם (*arg == '@')
    {
	// ":let @r = expr": Set register contents.
	החזר ex_let_register(arg, tv, flags, endchars, op);
    }
    אחרת אם (eval_isnamec1(*arg) || *arg == '{')
    {
	lval_T	lv;
	char_u	*p;
	صحيح	lval_flags = (flags & (ASSIGN_NO_DECL | ASSIGN_DECL))
							     ? GLV_NO_DECL : 0;
	lval_flags |= (flags & ASSIGN_FOR_LOOP) ? GLV_FOR_LOOP : 0;
	אם (op != عدم && *op != '=')
	    lval_flags |= GLV_ASSIGN_WITH_OP;

	// ":let var = expr": Set internal variable.
	// ":let var: type = expr": Set internal variable with type.
	// ":let {expr} = expr": Idem, name made with curly braces
	p = get_lval(arg, tv, &lv, FALSE, FALSE, lval_flags, FNE_CHECK_START);
	אם (p != عدم && lv.ll_name != عدم)
	{
	    אם (endchars != عدم && vim_strchr(endchars,
					   *skipwhite(lv.ll_name_end)) == عدم)
	    {
		emsg(_(e_unexpected_characters_in_let));
	    }
	    אחרת
	    {
		set_var_lval(&lv, p, tv, copy, flags, op, var_idx);
		arg_end = lv.ll_name_end;
	    }
	}
	clear_lval(&lv);
    }
    אחרת
	semsg(_(e_invalid_argument_str), arg);

    החזר arg_end;
}

/*
 * ":unlet[!] var1 ... " command.
 */
    فراغ
ex_unlet(exarg_T *eap)
{
    ex_unletlock(eap, eap->arg, 0, eap->forceit ? GLV_QUIET : 0, do_unlet_var, عدم);
}

/*
 * ":lockvar" and ":unlockvar" commands
 */
    فراغ
ex_lockvar(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    صحيح		deep = 2;

    אם (eap->forceit)
	deep = -1;
    אחרת אם (vim_isdigit(*arg))
    {
	deep = getdigits(&arg);
	arg = skipwhite(arg);
    }

    ex_unletlock(eap, arg, deep, 0, do_lock_var, عدم);
}

/*
 * ":unlet", ":lockvar" and ":unlockvar" are quite similar.
 * Also used for Vim9 script.  "callback" is invoked as:
 *	callback(&lv, name_end, eap, deep, cookie)
 */
    فراغ
ex_unletlock(
    exarg_T	*eap,
    char_u	*argstart,
    صحيح		deep,
    صحيح		glv_flags,
    صحيح		(*callback)(lval_T *, char_u *, exarg_T *, صحيح, فراغ *),
    فراغ	*cookie)
{
    char_u	*arg = argstart;
    char_u	*name_end;
    صحيح		error = FALSE;
    lval_T	lv;

    עשה
    {
	אם (*arg == '$')
	{
	    lv.ll_name = arg;
	    lv.ll_tv = عدم;
	    ++arg;
	    אם (get_env_len(&arg) == 0)
	    {
		semsg(_(e_invalid_argument_str), arg - 1);
		החזר;
	    }
	    אם (!error && !eap->skip
			      && callback(&lv, arg, eap, deep, cookie) == FAIL)
		error = TRUE;
	    name_end = arg;
	}
	אחרת
	{
	    // Parse the name and find the end.
	    name_end = get_lval(arg, عدم, &lv, TRUE, eap->skip || error,
				     glv_flags | GLV_NO_DECL, FNE_CHECK_START);
	    אם (lv.ll_name == عدم)
		error = TRUE;	    // error but continue parsing
	    אם (name_end == عدم || (!VIM_ISWHITE(*name_end)
						    && !ends_excmd(*name_end)))
	    {
		אם (name_end != عدم)
		{
		    emsg_severe = TRUE;
		    semsg(_(e_trailing_characters_str), name_end);
		}
		אם (!(eap->skip || error))
		    clear_lval(&lv);
		اكسر;
	    }

	    אם (!error && !eap->skip
			 && callback(&lv, name_end, eap, deep, cookie) == FAIL)
		error = TRUE;

	    אם (!eap->skip)
		clear_lval(&lv);
	}

	arg = skipwhite(name_end);
    } כלעוד (!ends_excmd2(name_end, arg));

    set_nextcmd(eap, arg);
}

    ساكن صحيح
do_unlet_var(
    lval_T	*lp,
    char_u	*name_end,
    exarg_T	*eap,
    صحيح		deep UNUSED,
    فراغ	*cookie UNUSED)
{
    صحيح		forceit = eap->forceit;
    صحيح		ret = OK;
    صحيح		cc;

    אם (lp->ll_tv == عدم)
    {
	cc = *name_end;
	*name_end = NUL;

	// Environment variable, normal name or expanded name.
	אם (*lp->ll_name == '$')
	    vim_unsetenv_ext(lp->ll_name + 1);
	אחרת אם (do_unlet(lp->ll_name, forceit) == FAIL)
	    ret = FAIL;
	*name_end = cc;
    }
    אחרת אם ((lp->ll_list != عدم
		 && value_check_lock(lp->ll_list->lv_lock, lp->ll_name, FALSE))
	    || (lp->ll_dict != عدم
		&& value_check_lock(lp->ll_dict->dv_lock, lp->ll_name, FALSE)))
	החזר FAIL;
    אחרת אם (lp->ll_range)
	list_unlet_range(lp->ll_list, lp->ll_li, lp->ll_n1,
						    !lp->ll_empty2, lp->ll_n2);
    אחרת אם (lp->ll_list != عدم)
	// unlet a List item.
	listitem_remove(lp->ll_list, lp->ll_li);
    אחרת אם (lp->ll_dict != عدم)
	// unlet a Dictionary item.
	dictitem_remove(lp->ll_dict, lp->ll_di, "unlet");
    אחרת
    {
	semsg(_(e_cannot_unlet_imported_item_str), lp->ll_name);
	החזר FAIL;
    }

    החזר ret;
}

/*
 * Unlet one item or a range of items from a list.
 * Return OK or FAIL.
 */
    فراغ
list_unlet_range(
	list_T	    *l,
	listitem_T  *li_first,
	long	    n1_arg,
	صحيح	    has_n2,
	long	    n2)
{
    listitem_T  *li = li_first;
    صحيح		n1 = n1_arg;

    // Delete a range of List items.
    li = li_first;
    n1 = n1_arg;
    כלעוד (li != عدم && (!has_n2 || n2 >= n1))
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
    صحيح
do_unlet(char_u *name, صحيح forceit)
{
    hashtab_T	*ht;
    hashitem_T	*hi;
    char_u	*varname = عدم;  // init to shut up gcc
    dict_T	*d;
    dictitem_T	*di;

    // can't :unlet a script variable in Vim9 script
    אם (in_vim9script() && check_vim9_unlet(name) == FAIL)
	החזר FAIL;

    ht = find_var_ht(name, &varname);

    // can't :unlet a script variable in Vim9 script from a function
    אם (ht == get_script_local_ht()
	    && SCRIPT_ID_VALID(current_sctx.sc_sid)
	    && SCRIPT_ITEM(current_sctx.sc_sid)->sn_version
							 == SCRIPT_VERSION_VIM9
	    && check_vim9_unlet(name) == FAIL)
	החזר FAIL;

    אם (ht != عدم && *varname != NUL)
    {
	d = get_current_funccal_dict(ht);
	אם (d == عدم)
	{
	    אם (ht == &globvarht)
		d = &globvardict;
	    אחרת אם (ht == &compat_hashtab)
		d = &vimvardict;
	    אחרת
	    {
		di = find_var_in_ht(ht, *name, (char_u *)"", FALSE);
		d = di == عدم ? عدم : di->di_tv.vval.v_dict;
	    }
	    אם (d == عدم)
	    {
		internal_error("do_unlet()");
		החזר FAIL;
	    }
	}
	hi = hash_find(ht, varname);
	אם (HASHITEM_EMPTY(hi))
	    hi = find_hi_in_scoped_ht(name, &ht);
	אם (hi != عدم && !HASHITEM_EMPTY(hi))
	{
	    di = HI2DI(hi);
	    אם (var_check_fixed(di->di_flags, name, FALSE)
		    || var_check_ro(di->di_flags, name, FALSE)
		    || value_check_lock(d->dv_lock, name, FALSE)
		    || check_hashtab_frozen(ht, "unlet"))
		החזר FAIL;

	    delete_var(ht, hi);
	    החזר OK;
	}
    }
    אם (forceit)
	החזר OK;
    semsg(_(e_no_such_variable_str), name);
    החזר FAIL;
}

    ساكن فراغ
report_lockvar_member(حرف *msg, lval_T *lp)
{
    صحيح did_alloc = FALSE;
    char_u *vname = (char_u *)"";
    char_u *class_name = lp->ll_class != عدم
				    ? lp->ll_class->class_name.string : (char_u *)"";
    אם (lp->ll_name != عدم)
    {
	אם (lp->ll_name_end == عدم)
	    vname = lp->ll_name;
	אחרת
	{
	    vname = vim_strnsave(lp->ll_name, lp->ll_name_end - lp->ll_name);
	    אם (vname == عدم)
		החזר;
	    did_alloc = TRUE;
	}
    }
    semsg(_(msg), vname, class_name);
    אם (did_alloc)
	vim_free(vname);
}

/*
 * Lock or unlock variable indicated by "lp".
 * "deep" is the levels to go (-1 for unlimited);
 * "lock" is TRUE for ":lockvar", FALSE for ":unlockvar".
 */
    ساكن صحيح
do_lock_var(
    lval_T	*lp,
    char_u	*name_end,
    exarg_T	*eap,
    صحيح		deep,
    فراغ	*cookie UNUSED)
{
    صحيح		lock = eap->cmdidx == CMD_lockvar;
    صحيح		ret = OK;
    صحيح		cc;
    dictitem_T	*di;

#ifdef LOG_LOCKVAR
    ch_log(عدم, "LKVAR: do_lock_var(): name %s, is_root %d", lp->ll_name, lp->ll_is_root);
#endif

    אם (lp->ll_tv == عدم)
    {
	cc = *name_end;
	*name_end = NUL;
	אם (*lp->ll_name == '$')
	{
	    semsg(_(e_cannot_lock_or_unlock_variable_str), lp->ll_name);
	    ret = FAIL;
	}
	אחרת
	{
	    // Normal name or expanded name.
	    di = find_var(lp->ll_name, عدم, TRUE);
	    אם (di == عدم)
	    {
		אם (in_vim9script())
		    semsg(_(e_cannot_find_variable_to_unlock_str),
								  lp->ll_name);
		ret = FAIL;
	    }
	    אחרת
	    {
		אם ((di->di_flags & DI_FLAGS_FIX)
			    && di->di_tv.v_type != VAR_DICT
			    && di->di_tv.v_type != VAR_LIST)
		{
		    // For historic reasons this error is not given for a list
		    // or dict.  E.g., the b: dict could be locked/unlocked.
		    semsg(_(e_cannot_lock_or_unlock_variable_str), lp->ll_name);
		    ret = FAIL;
		}
		אחרת
		{
		    אם (in_vim9script())
		    {
			svar_T  *sv = find_typval_in_script(&di->di_tv,
								     0, FALSE);

			אם (sv != عدم && sv->sv_const != 0)
			{
			    semsg(_(e_cannot_change_readonly_variable_str),
								  lp->ll_name);
			    ret = FAIL;
			}
		    }

		    אם (ret == OK)
		    {
			אם (lock)
			    di->di_flags |= DI_FLAGS_LOCK;
			אחרת
			    di->di_flags &= ~DI_FLAGS_LOCK;
			אם (deep != 0)
			    item_lock(&di->di_tv, deep, lock, FALSE);
		    }
		}
	    }
	}
	*name_end = cc;
    }
    אחרת אם (deep == 0 && lp->ll_object == عدم && lp->ll_class == عدم)
    {
	// nothing to do
    }
    אחרת אם (lp->ll_is_root)
	// (un)lock the item.
	item_lock(lp->ll_tv, deep, lock, FALSE);
    אחרת אם (lp->ll_range)
    {
	listitem_T    *li = lp->ll_li;

	// (un)lock a range of List items.
	כלעוד (li != عدم && (lp->ll_empty2 || lp->ll_n2 >= lp->ll_n1))
	{
	    item_lock(&li->li_tv, deep, lock, FALSE);
	    li = li->li_next;
	    ++lp->ll_n1;
	}
    }
    אחרת אם (lp->ll_list != عدم)
	// (un)lock a List item.
	item_lock(&lp->ll_li->li_tv, deep, lock, FALSE);
    אחרת אם (lp->ll_object != عدم)  // This check must be before ll_class.
    {
	// (un)lock an object variable.
	report_lockvar_member(e_cannot_lock_object_variable_str, lp);
	ret = FAIL;
    }
    אחרת אם (lp->ll_class != عدم)
    {
	// (un)lock a class variable.
	report_lockvar_member(e_cannot_lock_class_variable_str, lp);
	ret = FAIL;
    }
    אחרת
    {
	// (un)lock a Dictionary item.
	אם (lp->ll_di == عدم)
	{
	    emsg(_(e_dictionary_required));
	    ret = FAIL;
	}
	אחרת
	    item_lock(&lp->ll_di->di_tv, deep, lock, FALSE);
    }

    החזר ret;
}

/*
 * Lock or unlock an item.  "deep" is nr of levels to go.
 * When "check_refcount" is TRUE do not lock a list or dict with a reference
 * count larger than 1.
 */
    فراغ
item_lock(typval_T *tv, صحيح deep, صحيح lock, صحيح check_refcount)
{
    ساكن صحيح	recurse = 0;
    list_T	*l;
    listitem_T	*li;
    dict_T	*d;
    blob_T	*b;
    hashitem_T	*hi;
    صحيح		todo;

#ifdef LOG_LOCKVAR
    ch_log(عدم, "LKVAR: item_lock(): type %s", vartype_name(tv->v_type));
#endif

    אם (recurse >= DICT_MAXNEST)
    {
	emsg(_(e_variable_nested_too_deep_for_unlock));
	החזר;
    }
    אם (deep == 0)
	החזר;
    ++recurse;

    // lock/unlock the item itself
    אם (lock)
	tv->v_lock |= VAR_LOCKED;
    אחרת
	tv->v_lock &= ~VAR_LOCKED;

    بدل (tv->v_type)
    {
	حالة VAR_UNKNOWN:
	حالة VAR_ANY:
	حالة VAR_VOID:
	حالة VAR_NUMBER:
	حالة VAR_BOOL:
	حالة VAR_STRING:
	حالة VAR_FUNC:
	حالة VAR_PARTIAL:
	حالة VAR_FLOAT:
	حالة VAR_SPECIAL:
	حالة VAR_JOB:
	حالة VAR_CHANNEL:
	حالة VAR_INSTR:
	حالة VAR_CLASS:
	حالة VAR_OBJECT:
	حالة VAR_TYPEALIAS:
	    اكسر;

	حالة VAR_BLOB:
	    אם ((b = tv->vval.v_blob) != عدم
				    && !(check_refcount && b->bv_refcount > 1))
	    {
		אם (lock)
		    b->bv_lock |= VAR_LOCKED;
		אחרת
		    b->bv_lock &= ~VAR_LOCKED;
	    }
	    اكسر;
	حالة VAR_LIST:
	    אם ((l = tv->vval.v_list) != عدم
				    && !(check_refcount && l->lv_refcount > 1))
	    {
		אם (lock)
		    l->lv_lock |= VAR_LOCKED;
		אחרת
		    l->lv_lock &= ~VAR_LOCKED;
		אם (deep < 0 || deep > 1)
		{
		    אם (l->lv_first == &range_list_item)
			l->lv_lock |= VAR_ITEMS_LOCKED;
		    אחרת
		    {
			// recursive: lock/unlock the items the List contains
			CHECK_LIST_MATERIALIZE(l);
			FOR_ALL_LIST_ITEMS(l, li) item_lock(&li->li_tv,
					       deep - 1, lock, check_refcount);
		    }
		}
	    }
	    اكسر;
	حالة VAR_TUPLE:
	    tuple_lock(tv->vval.v_tuple, deep, lock, check_refcount);
	    اكسر;
	حالة VAR_DICT:
	    אם ((d = tv->vval.v_dict) != عدم
				    && !(check_refcount && d->dv_refcount > 1))
	    {
		אם (lock)
		    d->dv_lock |= VAR_LOCKED;
		אחרת
		    d->dv_lock &= ~VAR_LOCKED;
		אם (deep < 0 || deep > 1)
		{
		    // recursive: lock/unlock the items the List contains
		    todo = (صحيح)d->dv_hashtab.ht_used;
		    FOR_ALL_HASHTAB_ITEMS(&d->dv_hashtab, hi, todo)
		    {
			אם (!HASHITEM_EMPTY(hi))
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

#אם defined(FEAT_MENU) && defined(FEAT_MULTI_LANG)
/*
 * Delete all "menutrans_" variables.
 */
    فراغ
del_menutrans_vars(فراغ)
{
    hashitem_T	*hi;
    صحيح		todo;

    hash_lock(&globvarht);
    todo = (صحيح)globvarht.ht_used;
    עבור (hi = globvarht.ht_array; todo > 0 && !got_int; ++hi)
    {
	אם (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    אם (STRNCMP(HI2DI(hi)->di_key, "menutrans_", 10) == 0)
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

ساكن char_u	*varnamebuf = عدم;
ساكن صحيح	varnamebuflen = 0;

/*
 * Function to concatenate a prefix and a variable name.
 */
    char_u *
cat_prefix_varname(صحيح prefix, char_u *name)
{
    صحيح		len;

    len = (صحيح)STRLEN(name) + 3;
    אם (len > varnamebuflen)
    {
	vim_free(varnamebuf);
	len += 10;			// some additional space
	varnamebuf = alloc(len);
	אם (varnamebuf == عدم)
	{
	    varnamebuflen = 0;
	    החזר عدم;
	}
	varnamebuflen = len;
    }
    *varnamebuf = prefix;
    varnamebuf[1] = ':';
    STRCPY(varnamebuf + 2, name);
    החזר varnamebuf;
}

/*
 * Function given to ExpandGeneric() to obtain the list of user defined
 * (global/buffer/window/built-in) variable names.
 */
    char_u *
get_user_var_name(expand_T *xp, صحيح idx)
{
    ساكن long_u	gdone;
    ساكن long_u	bdone;
    ساكن long_u	wdone;
    ساكن long_u	tdone;
    ساكن صحيح		vidx;
    ساكن hashitem_T	*hi;
    hashtab_T		*ht;

    אם (idx == 0)
    {
	gdone = bdone = wdone = vidx = 0;
	tdone = 0;
    }

    // Global variables
    אם (gdone < globvarht.ht_used)
    {
	אם (gdone++ == 0)
	    hi = globvarht.ht_array;
	אחרת
	    ++hi;
	כלעוד (HASHITEM_EMPTY(hi))
	    ++hi;
	אם (STRNCMP("g:", xp->xp_pattern, 2) == 0)
	    החזר cat_prefix_varname('g', hi->hi_key);
	החזר hi->hi_key;
    }

    // b: variables
    ht = &prevwin_curwin()->w_buffer->b_vars->dv_hashtab;
    אם (bdone < ht->ht_used)
    {
	אם (bdone++ == 0)
	    hi = ht->ht_array;
	אחרת
	    ++hi;
	כלעוד (HASHITEM_EMPTY(hi))
	    ++hi;
	החזר cat_prefix_varname('b', hi->hi_key);
    }

    // w: variables
    ht = &prevwin_curwin()->w_vars->dv_hashtab;
    אם (wdone < ht->ht_used)
    {
	אם (wdone++ == 0)
	    hi = ht->ht_array;
	אחרת
	    ++hi;
	כלעוד (HASHITEM_EMPTY(hi))
	    ++hi;
	החזר cat_prefix_varname('w', hi->hi_key);
    }

    // t: variables
    ht = &curtab->tp_vars->dv_hashtab;
    אם (tdone < ht->ht_used)
    {
	אם (tdone++ == 0)
	    hi = ht->ht_array;
	אחרת
	    ++hi;
	כלעוד (HASHITEM_EMPTY(hi))
	    ++hi;
	החזר cat_prefix_varname('t', hi->hi_key);
    }

    // v: variables
    אם (vidx < VV_LEN)
	החזר cat_prefix_varname('v', (char_u *)vimvars[vidx++].vv_name);

    VIM_CLEAR(varnamebuf);
    varnamebuflen = 0;
    החזר عدم;
}

    حرف *
get_var_special_name(صحيح nr)
{
    بدل (nr)
    {
	حالة VVAL_FALSE: החזר in_vim9script() ? "false" : "v:false";
	حالة VVAL_TRUE:  החזר in_vim9script() ? "true" : "v:true";
	حالة VVAL_NULL:  החזר in_vim9script() ? "null" : "v:null";
	حالة VVAL_NONE:  החזר "v:none";
    }
    internal_error("get_var_special_name()");
    החזר "42";
}

/*
 * Returns the global variable dictionary
 */
    dict_T *
get_globvar_dict(فراغ)
{
    החזר &globvardict;
}

/*
 * Returns the global variable hash table
 */
    hashtab_T *
get_globvar_ht(فراغ)
{
    החזר &globvarht;
}

/*
 * Returns the v: variable dictionary
 */
    dict_T *
get_vimvar_dict(فراغ)
{
    החזר &vimvardict;
}

/*
 * Returns the index of a v:variable.  Negative if not found.
 * Returns DI_ flags in "di_flags".
 */
    صحيح
find_vim_var(char_u *name, صحيح *di_flags)
{
    dictitem_T	    *di = find_var_in_ht(&vimvarht, 0, name, TRUE);
    بنية vimvar   *vv;

    אם (di == عدم)
	החזר -1;
    *di_flags = di->di_flags;
    vv = (بنية vimvar *)((حرف *)di - offsetof(vimvar_T, vv_di));
    החזר (صحيح)(vv - vimvars);
}


/*
 * Set type of v: variable to "type".
 */
    فراغ
set_vim_var_type(صحيح idx, vartype_T type)
{
    vimvars[idx].vv_tv_type = type;
}

/*
 * Set number v: variable to "val".
 * Note that this does not set the type, use set_vim_var_type() for that.
 */
    فراغ
set_vim_var_nr(صحيح idx, varnumber_T val)
{
    vimvars[idx].vv_nr = val;
}

    حرف *
get_vim_var_name(صحيح idx)
{
    החזר vimvars[idx].vv_name;
}

/*
 * Get typval_T v: variable value.
 */
    typval_T *
get_vim_var_tv(صحيح idx)
{
    החזר &vimvars[idx].vv_tv;
}

    type_T *
get_vim_var_type(صحيح idx, garray_T *type_list)
{
    אם (vimvars[idx].vv_type != عدم)
	החזר vimvars[idx].vv_type;
    החזר typval2type_vimvar(&vimvars[idx].vv_tv, type_list);
}

/*
 * Add what getinfo() reports for the v: variable "name", without the "v:", to
 * "d".  Returns FAIL when there is no such variable.
 */
    صحيح
vim_var_info(char_u *name, dict_T *d)
{
    صحيح		di_flags;
    صحيح		idx = find_vim_var(name, &di_flags);
    garray_T	type_list;
    حرف	*tofree;

    אם (idx < 0)
	החזר FAIL;
    vim_snprintf((حرف *)IObuff, IOSIZE, "v:%s", vimvars[idx].vv_name);
    dict_add_string(d, "name", IObuff);
    dict_add_bool(d, "available", TRUE);
    ga_init2(&type_list, حجم(type_T *), 10);
    حرف *type = type_name(get_vim_var_type(idx, &type_list), &tofree);
    dict_add_string(d, "type", (char_u *)type);
    vim_free(tofree);
    clear_type_list(&type_list);
    dict_add_bool(d, "readonly", (vimvars[idx].vv_flags & VV_RO) != 0);
    dict_add_bool(d, "compat", (vimvars[idx].vv_flags & VV_COMPAT) != 0);
    החזר OK;
}

/*
 * Set v: variable to "tv".  Only accepts the same type.
 * Takes over the value of "tv".
 */
    صحيح
set_vim_var_tv(صحيح idx, typval_T *tv)
{
    אם (vimvars[idx].vv_tv_type != tv->v_type)
    {
	emsg(_(e_type_mismatch_for_v_variable));
	clear_tv(tv);
	החזר FAIL;
    }
    // VV_RO is also checked when compiling, but let's check here as well.
    אם (vimvars[idx].vv_flags & VV_RO)
    {
	semsg(_(e_cannot_change_readonly_variable_str), vimvars[idx].vv_name);
	החזר FAIL;
    }
    אם (sandbox && (vimvars[idx].vv_flags & VV_RO_SBX))
    {
	semsg(_(e_cannot_set_variable_in_sandbox_str), vimvars[idx].vv_name);
	החזר FAIL;
    }
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_di.di_tv = *tv;
    החזר OK;
}

/*
 * Get number v: variable value.
 */
    varnumber_T
get_vim_var_nr(صحيح idx)
{
    החזר vimvars[idx].vv_nr;
}

/*
 * Get string v: variable value.  Uses a static buffer, can only be used once.
 * If the String variable has never been set, return an empty string.
 * Never returns NULL;
 */
    char_u *
get_vim_var_str(صحيح idx)
{
    החזר tv_get_string(&vimvars[idx].vv_tv);
}

/*
 * Get List v: variable value.  Caller must take care of reference count when
 * needed.
 */
    list_T *
get_vim_var_list(صحيح idx)
{
    החזר vimvars[idx].vv_list;
}

/*
 * Get Dict v: variable value.  Caller must take care of reference count when
 * needed.
 */
    dict_T *
get_vim_var_dict(صحيح idx)
{
    החזר vimvars[idx].vv_dict;
}

/*
 * Set v:char to character "c".
 */
    فراغ
set_vim_var_char(صحيح c)
{
    char_u  buf[MB_MAXBYTES + 1];
    size_t  buflen;

    אם (has_mbyte)
	buflen = (*mb_char2bytes)(c, buf);
    אחרת
    {
	buf[0] = c;
	buflen = 1;
    }
    buf[buflen] = NUL;

    set_vim_var_string(VV_CHAR, buf, (صحيح)buflen);
}

/*
 * Set v:count to "count" and v:count1 to "count1".
 * When "set_prevcount" is TRUE first set v:prevcount from v:count.
 */
    فراغ
set_vcount(
    long	count,
    long	count1,
    صحيح		set_prevcount)
{
    אם (set_prevcount)
	vimvars[VV_PREVCOUNT].vv_nr = vimvars[VV_COUNT].vv_nr;
    vimvars[VV_COUNT].vv_nr = count;
    vimvars[VV_COUNT1].vv_nr = count1;
}

/*
 * Save variables that might be changed as a side effect.  Used when executing
 * a timer callback.
 */
    فراغ
save_vimvars(vimvars_save_T *vvsave)
{
    vvsave->vv_prevcount = vimvars[VV_PREVCOUNT].vv_nr;
    vvsave->vv_count = vimvars[VV_COUNT].vv_nr;
    vvsave->vv_count1 = vimvars[VV_COUNT1].vv_nr;
}

/*
 * Restore variables saved by save_vimvars().
 */
    فراغ
restore_vimvars(vimvars_save_T *vvsave)
{
    vimvars[VV_PREVCOUNT].vv_nr = vvsave->vv_prevcount;
    vimvars[VV_COUNT].vv_nr = vvsave->vv_count;
    vimvars[VV_COUNT1].vv_nr = vvsave->vv_count1;
}

/*
 * Set string v: variable to a copy of "val". If 'copy' is FALSE, then set the
 * value.
 */
    فراغ
set_vim_var_string(
    صحيح		idx,
    char_u	*val,
    صحيح		len)	    // length of "val" to use or -1 (whole string)
{
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_tv_type = VAR_STRING;
    אם (val == عدم)
	vimvars[idx].vv_str = عدم;
    אחרת אם (len == -1)
	vimvars[idx].vv_str = vim_strsave(val);
    אחרת
	vimvars[idx].vv_str = vim_strnsave(val, len);
}

    فراغ
set_vim_var_string_direct(
    صحيح		idx,
    char_u	*val)
{
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_tv_type = VAR_STRING;

    vimvars[idx].vv_str = val;
}

/*
 * Set List v: variable to "val".
 */
    فراغ
set_vim_var_list(صحيح idx, list_T *val)
{
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_tv_type = VAR_LIST;
    vimvars[idx].vv_list = val;
    אם (val != عدم)
	++val->lv_refcount;
}

/*
 * Set Dictionary v: variable to "val".
 */
    فراغ
set_vim_var_dict(صحيح idx, dict_T *val)
{
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_tv_type = VAR_DICT;
    vimvars[idx].vv_dict = val;
    אם (val == عدم)
	החזר;

    ++val->dv_refcount;
    dict_set_items_ro(val);
}

/*
 * Set the v:argv list.
 */
    فراغ
set_argv_var(حرف **argv, صحيح argc)
{
    list_T	*l = list_alloc();
    صحيح		i;

    אם (l == عدم)
	getout(1);
    l->lv_lock = VAR_FIXED;
    עבור (i = 0; i < argc; ++i)
    {
	אם (list_append_string(l, (char_u *)argv[i], -1) == FAIL)
	    getout(1);
	l->lv_u.mat.lv_last->li_tv.v_lock = VAR_FIXED;
    }
    set_vim_var_list(VV_ARGV, l);
}

/*
 * Reset v:register, taking the 'clipboard' setting into account.
 */
    فراغ
reset_reg_var(فراغ)
{
    صحيح regname = 0;

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
    فراغ
set_reg_var(صحيح c)
{
    char_u  regname[2];

    אם (c == 0 || c == ' ')
	regname[0] = '"';
    אחרת
	regname[0] = c;
    regname[1] = NUL;
    // Avoid free/alloc when the value is already right.
    אם (vimvars[VV_REG].vv_str == عدم || vimvars[VV_REG].vv_str[0] != c)
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
    אם (oldval == عدم)
	החזר vimvars[VV_EXCEPTION].vv_str;

    vimvars[VV_EXCEPTION].vv_str = oldval;
    החזר عدم;
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
    אם (oldval == عدم)
	החזר vimvars[VV_THROWPOINT].vv_str;

    vimvars[VV_THROWPOINT].vv_str = oldval;
    החזר عدم;
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
    אם (eap == عدم)
    {
	vim_free(oldval);
	vimvars[VV_CMDARG].vv_str = oldarg;
	החזר عدم;
    }

    אם (eap->force_bin == FORCE_BIN)
	len = 6;
    אחרת אם (eap->force_bin == FORCE_NOBIN)
	len = 8;
    אחרת
	len = 0;

    אם (eap->read_edit)
	len += 7;

    אם (eap->force_ff != 0)
	len += 10; // " ++ff=unix"
    אם (eap->force_enc != 0)
	len += (unsigned)STRLEN(eap->cmd + eap->force_enc) + 7;
    אם (eap->bad_char != 0)
	len += 7 + 4;  // " ++bad=" + "keep" or "drop"

    newval = alloc(len + 1);
    אם (newval == عدم)
	החזר عدم;

    אם (eap->force_bin == FORCE_BIN)
	sprintf((حرف *)newval, " ++bin");
    אחרת אם (eap->force_bin == FORCE_NOBIN)
	sprintf((حرف *)newval, " ++nobin");
    אחרת
	*newval = NUL;

    אם (eap->read_edit)
	STRCAT(newval, " ++edit");

    אם (eap->force_ff != 0)
	sprintf((حرف *)newval + STRLEN(newval), " ++ff=%s",
						eap->force_ff == 'u' ? "unix"
						: eap->force_ff == 'd' ? "dos"
						: "mac");
    אם (eap->force_enc != 0)
	sprintf((حرف *)newval + STRLEN(newval), " ++enc=%s",
					       eap->cmd + eap->force_enc);
    אם (eap->bad_char == BAD_KEEP)
	STRCPY(newval + STRLEN(newval), " ++bad=keep");
    אחרת אם (eap->bad_char == BAD_DROP)
	STRCPY(newval + STRLEN(newval), " ++bad=drop");
    אחרת אם (eap->bad_char != 0)
	sprintf((حرف *)newval + STRLEN(newval), " ++bad=%c", eap->bad_char);
    vimvars[VV_CMDARG].vv_str = newval;
    החזר oldval;
}

/*
 * Get the value of internal variable "name".
 * If "flags" has EVAL_VAR_IMPORT may return a VAR_ANY with v_number set to the
 * imported script ID.
 * Return OK or FAIL.  If OK is returned "rettv" must be cleared.
 */
    صحيح
eval_variable(
    char_u	*name,
    صحيح		len,		// length of "name" or zero
    scid_T	sid,		// script ID for imported item or zero
    typval_T	*rettv,		// NULL when only checking existence
    dictitem_T	**dip,		// non-NULL when typval's dict item is needed
    صحيح		flags)		// EVAL_VAR_ flags
{
    صحيح		ret = OK;
    typval_T	*tv = عدم;
    صحيح		found = FALSE;
    hashtab_T	*ht = عدم;
    صحيح		cc = 0;
    type_T	*type = عدم;

    אם (len > 0)
    {
	// truncate the name, so that we can use strcmp()
	cc = name[len];
	name[len] = NUL;
    }

    // Check for local variable when debugging.
    אם ((sid == 0) && (tv = lookup_debug_var(name)) == عدم)
    {
	// Check for user-defined variables.
	dictitem_T	*v = find_var(name, &ht, flags & EVAL_VAR_NOAUTOLOAD);

	אם (v != عدم)
	{
	    tv = &v->di_tv;
	    אם (dip != عدم)
		*dip = v;
	}
	אחרת
	    ht = عدم;
    }

    אם (tv == عدم && (in_vim9script() || STRNCMP(name, "s:", 2) == 0))
    {
	imported_T  *import = عدم;
	char_u	    *p = STRNCMP(name, "s:", 2) == 0 ? name + 2 : name;

	אם (sid == 0)
	    import = find_imported(p, 0, TRUE);

	// imported variable from another script
	אם (import != عدم || sid != 0)
	{
	    אם ((flags & EVAL_VAR_IMPORT) == 0)
	    {
		אם (SCRIPT_ID_VALID(sid))
		{
		    ht = &SCRIPT_VARS(sid);
		    אם (ht != عدم)
		    {
			dictitem_T *v = find_var_in_ht(ht, 0, name,
						  flags & EVAL_VAR_NOAUTOLOAD);

			אם (v == عدم)
			    v = find_var_autoload_prefix(name, sid, عدم, عدم);

			אם (v != عدم)
			{
			    tv = &v->di_tv;
			    אם (dip != عدم)
				*dip = v;
			}
			אחרת
			    ht = عدم;
		    }
		}
		אחרת
		{
		    אם (flags & EVAL_VAR_VERBOSE)
			semsg(_(e_expected_dot_after_name_str), name);
		    ret = FAIL;
		}
	    }
	    אחרת
	    {
		אם (rettv != عدم)
		{
		    // special value that is used in handle_subscript()
		    rettv->v_type = VAR_ANY;
		    rettv->vval.v_number = sid != 0 ? sid : import->imp_sid;
		}
		found = TRUE;
	    }
	}
	אחרת אם (in_vim9script() && (flags & EVAL_VAR_NO_FUNC) == 0)
	{
	    صحيح	    has_g_prefix = STRNCMP(name, "g:", 2) == 0;
	    ufunc_T *ufunc = find_func(name, FALSE);

	    אם (ufunc != عدم && cc == '<')
	    {
		// handle generic function
		char_u	*argp = name + len;
		name[len] = cc;
		ufunc = eval_generic_func(ufunc, name, &argp);
		name[len] = NUL;
		אם (ufunc == عدم)
		{
		    ret = FAIL;
		    اذهب done;
		}
	    }

	    // In Vim9 script we can get a function reference by using the
	    // function name.  For a global non-autoload function "g:" is
	    // required.
	    אם (ufunc != عدم && (has_g_prefix
					    || !func_requires_g_prefix(ufunc)))
	    {
		found = TRUE;
		אם (rettv != عدم)
		{
		    rettv->v_type = VAR_FUNC;
		    אם (has_g_prefix)
		    {
			// Keep the "g:", otherwise script-local may be
			// assumed.
			אם (cc != '<')
			    rettv->vval.v_string = vim_strsave(name);
			אחרת
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
		    אחרת
			rettv->vval.v_string = vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
		    אם (rettv->vval.v_string != عدم)
			func_ref(ufunc->uf_name);
		}
	    }
	}
    }

    אם (!found)
    {
	אם (tv == عدم)
	{
	    אם (rettv != عدم && (flags & EVAL_VAR_VERBOSE))
		semsg(_(e_undefined_variable_str), name);
	    ret = FAIL;
	}
	אחרת אם (rettv != عدم)
	{
	    svar_T  *sv = عدم;
	    صحيح	    was_assigned = FALSE;

	    אם (ht != عدم && ht == get_script_local_ht()
		    && tv != &SCRIPT_SV(current_sctx.sc_sid)->sv_var.di_tv)
	    {
		sv = find_typval_in_script(tv, 0, TRUE);
		אם (sv != عدم)
		{
		    type = sv->sv_type;
		    was_assigned = sv->sv_flags & SVFLAG_ASSIGNED;
		}
	    }

	    אם ((tv->v_type == VAR_TYPEALIAS || tv->v_type == VAR_CLASS)
		    && sid != 0)
	    {
		// type alias or class imported from another script.  Check
		// whether it is exported from the other script.
		sv = find_typval_in_script(tv, sid, TRUE);
		אם (sv == عدم)
		{
		    ret = FAIL;
		    اذهب done;
		}
		אם ((sv->sv_flags & SVFLAG_EXPORTED) == 0)
		{
		    semsg(_(e_item_not_exported_in_script_str), name);
		    ret = FAIL;
		    اذهب done;
		}
	    }

	    // If a list or tuple or dict variable wasn't initialized and has
	    // meaningful type, do it now.  Not for global variables, they are
	    // not declared.
	    אם (ht != &globvarht)
	    {
		אם (tv->v_type == VAR_DICT && tv->vval.v_dict == عدم
					    && ((type != عدم && !was_assigned)
							  || !in_vim9script()))
		{
		    tv->vval.v_dict = dict_alloc();
		    אם (tv->vval.v_dict != عدم)
		    {
			++tv->vval.v_dict->dv_refcount;
			tv->vval.v_dict->dv_type = alloc_type(type);
			אם (sv != عدم)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
		אחרת אם (tv->v_type == VAR_LIST && tv->vval.v_list == عدم
					    && ((type != عدم && !was_assigned)
							  || !in_vim9script()))
		{
		    tv->vval.v_list = list_alloc();
		    אם (tv->vval.v_list != عدم)
		    {
			++tv->vval.v_list->lv_refcount;
			tv->vval.v_list->lv_type = alloc_type(type);
			אם (sv != عدم)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
		אחרת אם (tv->v_type == VAR_TUPLE && tv->vval.v_tuple == عدم
					    && ((type != عدم && !was_assigned)
							  || !in_vim9script()))
		{
		    tv->vval.v_tuple = tuple_alloc();
		    אם (tv->vval.v_tuple != عدم)
		    {
			++tv->vval.v_tuple->tv_refcount;
			tv->vval.v_tuple->tv_type = alloc_type(type);
			אם (sv != عدم)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
		אחרת אם (tv->v_type == VAR_BLOB && tv->vval.v_blob == عدم
					    && ((type != عدم && !was_assigned)
							  || !in_vim9script()))
		{
		    tv->vval.v_blob = blob_alloc();
		    אם (tv->vval.v_blob != عدم)
		    {
			++tv->vval.v_blob->bv_refcount;
			אם (sv != عدم)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
	    }
	    ret = copy_tv(tv, rettv);
	}
    }

done:
    אם (len > 0)
	name[len] = cc;

    החזר ret;
}

/*
 * Get the value of internal variable "name", also handling "import.name".
 * Return OK or FAIL.  If OK is returned "rettv" must be cleared.
 */
    صحيح
eval_variable_import(
    char_u	*name,
    typval_T	*rettv)
{
    char_u  *s = name;
    כלעוד (ASCII_ISALNUM(*s) || *s == '_')
	++s;
    صحيح	    len = (صحيح)(s - name);

    אם (eval_variable(name, len, 0, rettv, عدم, EVAL_VAR_IMPORT) == FAIL)
	החזר FAIL;
    אם (rettv->v_type == VAR_ANY && *s == '.')
    {
	char_u *ns = s + 1;
	s = ns;
	כלעוד (ASCII_ISALNUM(*s) || *s == '_')
	    ++s;
	صحيح sid = rettv->vval.v_number;
	החזר eval_variable(ns, (صحيح)(s - ns), sid, rettv, عدم, 0);
    }
    החזר OK;
}


/*
 * Check if variable "name[len]" is a local variable or an argument.
 * If so, "*eval_lavars_used" is set to TRUE.
 */
    فراغ
check_vars(char_u *name, صحيح len)
{
    صحيح		cc;
    char_u	*varname;
    hashtab_T	*ht;

    אם (eval_lavars_used == عدم)
	החזר;

    // truncate the name, so that we can use strcmp()
    cc = name[len];
    name[len] = NUL;

    ht = find_var_ht(name, &varname);
    אם (ht == get_funccal_local_ht() || ht == get_funccal_args_ht())
    {
	אם (find_var(name, عدم, TRUE) != عدم)
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
find_var(char_u *name, hashtab_T **htp, صحيح no_autoload)
{
    char_u	*varname;
    hashtab_T	*ht;
    dictitem_T	*ret = عدم;

    ht = find_var_ht(name, &varname);
    אם (htp != عدم)
	*htp = ht;
    אם (ht == عدم)
	החזר عدم;
    ret = find_var_in_ht(ht, *name, varname, no_autoload);
    אם (ret != عدم)
	החזר ret;

    // Search in parent scope for lambda
    ret = find_var_in_scoped_ht(name, no_autoload);
    אם (ret != عدم)
	החזר ret;

    // in Vim9 script items without a scope can be script-local
    אם (in_vim9script() && name[0] != NUL && name[1] != ':')
    {
	ht = get_script_local_ht();
	אם (ht != عدم)
	{
	    ret = find_var_in_ht(ht, *name, varname, no_autoload);
	    אם (ret != عدم)
	    {
		אם (htp != عدم)
		    *htp = ht;
		החזר ret;
	    }
	}
    }

    // and finally try
    החזר find_var_autoload_prefix(name, 0, htp, عدم);
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
find_var_autoload_prefix(char_u *name, صحيح sid, hashtab_T **htp,
							    char_u **namep)
{
    hashtab_T	*ht;
    dictitem_T	*ret = عدم;
    // When using "vim9script autoload" script-local items are prefixed but can
    // be used with s:name.
    صحيح check_sid = sid > 0 ? sid : current_sctx.sc_sid;
    אם (SCRIPT_ID_VALID(check_sid)
		   && (in_vim9script() || (name[0] == 's' && name[1] == ':')))
    {
	scriptitem_T *si = SCRIPT_ITEM(check_sid);

	אם (si->sn_autoload_prefix != عدم)
	{
	    char_u *base_name = (name[0] == 's' && name[1] == ':')
							     ? name + 2 : name;
	    char_u *auto_name = concat_str(si->sn_autoload_prefix, base_name);

	    אם (auto_name != عدم)
	    {
		صحيح free_auto_name = TRUE;
		ht = &globvarht;
		ret = find_var_in_ht(ht, 'g', auto_name, TRUE);
		אם (ret != عدم)
		{
		    אם (htp != عدم)
			*htp = ht;
		    אם (namep != عدم)
		    {
			free_auto_name = FALSE;
			*namep = auto_name;
		    }
		}
		אם (free_auto_name)
		    vim_free(auto_name);
	    }
	}
    }

    החזר ret;
}

/*
 * Like find_var() but if the name starts with <SNR>99_ then look in the
 * referenced script (used for a funcref).
 */
    dictitem_T *
find_var_also_in_script(char_u *name, hashtab_T **htp, صحيح no_autoload)
{
    אם (STRNCMP(name, "<SNR>", 5) == 0 && SAFE_isdigit(name[5]))
    {
	char_u	    *p = name + 5;
	صحيح	    sid = getdigits(&p);

	אם (SCRIPT_ID_VALID(sid) && *p == '_')
	{
	    hashtab_T	*ht = &SCRIPT_VARS(sid);

	    אם (ht != عدم)
	    {
		dictitem_T *di = find_var_in_ht(ht, 0, p + 1, no_autoload);

		אם (di != عدم)
		{
		    אם (htp != عدم)
			*htp = ht;
		    החזר di;
		}
	    }
	}
    }

    החזר find_var(name, htp, no_autoload);
}

/*
 * Find variable "varname" in hashtab "ht" with name "htname".
 * When "varname" is empty returns curwin/curtab/etc vars dictionary.
 * Returns NULL if not found.
 */
    dictitem_T *
find_var_in_ht(
    hashtab_T	*ht,
    صحيح		htname,
    char_u	*varname,
    صحيح		no_autoload)
{
    hashitem_T	*hi;

    אם (*varname == NUL)
    {
	// Must be something like "s:", otherwise "ht" would be NULL.
	بدل (htname)
	{
	    حالة 's': החזר &SCRIPT_SV(current_sctx.sc_sid)->sv_var;
	    حالة 'g': החזר &globvars_var;
	    حالة 'v': החזר &vimvars_var;
	    حالة 'b': החזר &curbuf->b_bufvar;
	    حالة 'w': החזר &curwin->w_winvar;
	    حالة 't': החזר &curtab->tp_winvar;
	    حالة 'l': החזר get_funccal_local_var();
	    حالة 'a': החזר get_funccal_args_var();
	}
	החזר عدم;
    }

    hi = hash_find(ht, varname);
    אם (HASHITEM_EMPTY(hi))
    {
	// For global variables we may try auto-loading the script.  If it
	// worked find the variable again.  Don't auto-load a script if it was
	// loaded already, otherwise it would be loaded every time when
	// checking if a function name is a Funcref variable.
	אם (ht == &globvarht && !no_autoload)
	{
	    // Note: script_autoload() may make "hi" invalid. It must either
	    // be obtained again or not used.
	    אם (!script_autoload(varname, FALSE) || aborting())
		החזר عدم;
	    hi = hash_find(ht, varname);
	}
	אם (HASHITEM_EMPTY(hi))
	    החזר عدم;
    }
    החזר HI2DI(hi);
}

/*
 * Get the script-local hashtab.  NULL if not in a script context.
 */
    hashtab_T *
get_script_local_ht(فراغ)
{
    scid_T sid = current_sctx.sc_sid;

    אם (SCRIPT_ID_VALID(sid))
	החזר &SCRIPT_VARS(sid);
    החזר عدم;
}

/*
 * Look for "name[len]" in script-local variables and functions.
 * When "cmd" is TRUE it must look like a command, a function must be followed
 * by "(" or "->".
 * Return OK when found, FAIL when not found.
 */
    صحيح
lookup_scriptitem(
	char_u	*name,
	size_t	len,
	صحيح	cmd,
	cctx_T	*dummy UNUSED)
{
    hashtab_T	*ht = get_script_local_ht();
    char_u	buffer[30];
    char_u	*p;
    صحيح		res;
    hashitem_T	*hi;
    صحيح		is_global = FALSE;
    char_u	*fname = name;

    אם (ht == عدم)
	החזר FAIL;
    אם (len < حجم(buffer) - 1)
    {
	// avoid an alloc/free for short names
	vim_strncpy(buffer, name, len);
	p = buffer;
    }
    אחרת
    {
	p = vim_strnsave(name, len);
	אם (p == عدم)
	    החזר FAIL;
    }

    hi = hash_find(ht, p);
    res = HASHITEM_EMPTY(hi) ? FAIL : OK;

    // if not script-local, then perhaps autoload-exported
    אם (res == FAIL && find_var_autoload_prefix(p, 0, عدم, عدم) != عدم)
	res = OK;

    // if not script-local or autoload, then perhaps imported
    אם (res == FAIL && find_imported(p, 0, FALSE) != عدم)
	res = OK;
    אם (p != buffer)
	vim_free(p);

    // Find a function, so that a following "->" works.
    // When used as a command require "(" or "->" to follow, "Cmd" is a user
    // command while "Cmd()" is a function call.
    אם (res != OK)
    {
	p = skipwhite(name + len);

	אם (!cmd || name[len] == '(' || (p[0] == '-' && p[1] == '>'))
	{
	    // Do not check for an internal function, since it might also be a
	    // valid command, such as ":split" versus "split()".
	    // Skip "g:" before a function name.
	    אם (name[0] == 'g' && name[1] == ':')
	    {
		is_global = TRUE;
		fname = name + 2;
	    }
	    אם (find_func(fname, is_global) != عدم)
		res = OK;
	}
    }

    החזר res;
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

    אם (name[0] == NUL)
	החזר عدم;
    אם (name[1] != ':')
    {
	// The name must not start with a colon or #.
	אם (name[0] == ':' || name[0] == AUTOLOAD_CHAR)
	    החזר عدم;
	*varname = name;

	// "version" is "v:version" in all scopes if scriptversion < 3.
	// Same for a few other variables marked with VV_COMPAT.
	אם (in_old_script(3))
	{
	    hi = hash_find(&compat_hashtab, name);
	    אם (!HASHITEM_EMPTY(hi))
		החזר &compat_hashtab;
	}

	ht = get_funccal_local_ht();
	אם (ht != عدم)
	    החזר ht;				// local variable

	// In Vim9 script items at the script level are script-local, except
	// for autoload names.
	אם (in_vim9script() && vim_strchr(name, AUTOLOAD_CHAR) == عدم)
	{
	    ht = get_script_local_ht();
	    אם (ht != عدم)
		החזר ht;
	}

	החזר &globvarht;			// global variable
    }
    *varname = name + 2;
    אם (*name == 'g')				// global variable
	החזר &globvarht;
    // There must be no ':' or '#' in the rest of the name, unless g: is used
    אם (vim_strchr(name + 2, ':') != عدم
			       || vim_strchr(name + 2, AUTOLOAD_CHAR) != عدم)
	החזר عدم;
    אם (*name == 'b')				// buffer variable
	החזר &curbuf->b_vars->dv_hashtab;
    אם (*name == 'w')				// window variable
	החזר &curwin->w_vars->dv_hashtab;
    אם (*name == 't')				// tab page variable
	החזר &curtab->tp_vars->dv_hashtab;
    אם (*name == 'v')				// v: variable
	החזר &vimvarht;
    אם (get_current_funccal() != عدم
	       && get_current_funccal()->fc_func->uf_def_status
							    == UF_NOT_COMPILED)
    {
	// a: and l: are only used in functions defined with ":function"
	אם (*name == 'a')			// a: function argument
	    החזר get_funccal_args_ht();
	אם (*name == 'l')			// l: local function variable
	    החזר get_funccal_local_ht();
    }
    אם (*name == 's')				// script variable
    {
	ht = get_script_local_ht();
	אם (ht != عدم)
	    החזר ht;
    }
    החזר عدم;
}

/*
 * Get the string value of a (global/local) variable.
 * Note: see tv_get_string() for how long the pointer remains valid.
 * Returns NULL when it doesn't exist.
 */
    char_u *
get_var_value(char_u *name)
{
    dictitem_T	*v;

    v = find_var(name, عدم, FALSE);
    אם (v == عدم)
	החזר عدم;
    החזר tv_get_string(&v->di_tv);
}

/*
 * Allocate a new hashtab for a sourced script.  It will be used while
 * sourcing this script and when executing functions defined in the script.
 */
    فراغ
new_script_vars(scid_T id)
{
    scriptvar_T *sv;

    sv = ALLOC_CLEAR_ONE(scriptvar_T);
    אם (sv == عدم)
	החזר;
    init_var_dict(&sv->sv_dict, &sv->sv_var, VAR_SCOPE);
    SCRIPT_ITEM(id)->sn_vars = sv;
}

/*
 * Initialize dictionary "dict" as a scope and set variable "dict_var" to
 * point to it.
 */
    فراغ
init_var_dict(dict_T *dict, dictitem_T *dict_var, صحيح scope)
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
    dict_var->di_key[0] = NUL;
}

/*
 * Unreference a dictionary initialized by init_var_dict().
 */
    فراغ
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
    فراغ
vars_clear(hashtab_T *ht)
{
    vars_clear_ext(ht, TRUE);
}

/*
 * Like vars_clear(), but only free the value if "free_val" is TRUE.
 */
    فراغ
vars_clear_ext(hashtab_T *ht, صحيح free_val)
{
    صحيح		todo;
    hashitem_T	*hi;
    dictitem_T	*v;

    hash_lock(ht);
    todo = (صحيح)ht->ht_used;
    FOR_ALL_HASHTAB_ITEMS(ht, hi, todo)
    {
	אם (!HASHITEM_EMPTY(hi))
	{
	    --todo;

	    // Free the variable.  Don't remove it from the hashtab,
	    // ht_array might change then.  hash_clear() takes care of it
	    // later.
	    v = HI2DI(hi);
	    אם (free_val)
		clear_tv(&v->di_tv);
	    אם (v->di_flags & DI_FLAGS_ALLOC)
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
    فراغ
delete_var(hashtab_T *ht, hashitem_T *hi)
{
    dictitem_T	*di = HI2DI(hi);

    אם (hash_remove(ht, hi, "delete variable") != OK)
	החזר;

    clear_tv(&di->di_tv);
    vim_free(di);
}

/*
 * Delete the exported variables of a reloaded Vim9 autoload script.  They live
 * in the global namespace with the autoload prefix (e.g. "foo#bar") and are
 * recreated when the script body runs again.
 */
    فراغ
delete_autoload_export_vars(char_u *prefix)
{
    אם (prefix == عدم)
	החזר;
    size_t  prefixlen = STRLEN(prefix);

    hash_lock(&globvarht);
    صحيح	todo = (صحيح)globvarht.ht_used;

    עבור (hashitem_T *hi = globvarht.ht_array; todo > 0; ++hi)
	אם (!HASHITEM_EMPTY(hi))
	{
	    dictitem_T	*di = HI2DI(hi);

	    --todo;
	    // Keep a class or enum: existing objects still refer to it.
	    אם (di->di_tv.v_type != VAR_CLASS
		    && STRNCMP(di->di_key, prefix, prefixlen) == 0)
		delete_var(&globvarht, hi);
	}
    hash_unlock(&globvarht);
}

/*
 * List the value of one internal variable.
 */
    ساكن فراغ
list_one_var(dictitem_T *v, حرف *prefix, صحيح *first)
{
    char_u	*tofree;
    char_u	*s;
    char_u	numbuf[NUMBUFLEN];

    s = echo_string(&v->di_tv, &tofree, numbuf, get_copyID());
    list_one_var_a(prefix, v->di_key, v->di_tv.v_type,
					 s == عدم ? (char_u *)"" : s, first);
    vim_free(tofree);
}

    ساكن فراغ
list_one_var_a(
    حرف	*prefix,
    char_u	*name,
    صحيح		type,
    char_u	*string,
    صحيح		*first)  // when TRUE clear rest of screen and set to FALSE
{
    // don't use msg() or msg_attr() to avoid overwriting "v:statusmsg"
    msg_start();
    msg_puts(prefix);
    אם (name != عدم)	// "a:" vars don't have a name stored
	msg_puts((حرف *)name);
    msg_putchar(' ');
    msg_advance(22);
    אם (type == VAR_NUMBER)
	msg_putchar('#');
    אחרת אם (type == VAR_FUNC || type == VAR_PARTIAL)
	msg_putchar('*');
    אחרת אם (type == VAR_LIST)
    {
	msg_putchar('[');
	אם (*string == '[')
	    ++string;
    }
    אחרת אם (type == VAR_DICT)
    {
	msg_putchar('{');
	אם (*string == '{')
	    ++string;
    }
    אחרת
	msg_putchar(' ');

    msg_outtrans(string);

    אם (type == VAR_FUNC || type == VAR_PARTIAL)
	msg_puts("()");
    אם (*first)
    {
	msg_clr_eos();
	*first = FALSE;
    }
}

/*
 * Addition handling for setting a v: variable.
 * Return TRUE if the variable should be set normally,
 *        FALSE if nothing else needs to be done.
 */
    صحيح
before_set_vvar(
    char_u	*varname,
    dictitem_T	*di,
    typval_T	*tv,
    صحيح		copy,
    صحيح		*type_error)
{
    אם (di->di_tv.v_type == VAR_STRING)
    {
	VIM_CLEAR(di->di_tv.vval.v_string);
	אם (copy || tv->v_type != VAR_STRING)
	{
	    char_u *val = tv_get_string(tv);

	    // Careful: when assigning to v:errmsg and
	    // tv_get_string() causes an error message the variable
	    // will already be set.
	    אם (di->di_tv.vval.v_string == عدم)
		di->di_tv.vval.v_string = vim_strsave(val);
	}
	אחרת
	{
	    // Take over the string to avoid an extra alloc/free.
	    di->di_tv.vval.v_string = tv->vval.v_string;
	    tv->vval.v_string = عدم;
	}
	החזר FALSE;
    }
    אחרת אם (di->di_tv.v_type == VAR_NUMBER)
    {
	di->di_tv.vval.v_number = tv_get_number(tv);
	אם (STRCMP(varname, "searchforward") == 0)
	    set_search_direction(di->di_tv.vval.v_number ? '/' : '?');
#ifdef FEAT_SEARCH_EXTRA
	אחרת אם (STRCMP(varname, "hlsearch") == 0)
	{
	    no_hlsearch = !di->di_tv.vval.v_number;
	    redraw_all_later(UPD_SOME_VALID);
	}
#endif
	החזר FALSE;
    }
    אחרת אם (di->di_tv.v_type != tv->v_type)
    {
	*type_error = TRUE;
	החזר FALSE;
    }
    החזר TRUE;
}

/*
 * Set variable "name" to value in "tv".
 * If the variable already exists, the value is updated.
 * Otherwise the variable is created.
 */
    فراغ
set_var(
    char_u	*name,
    typval_T	*tv,
    صحيح		copy)	    // make copy of value in "tv"
{
    set_var_const(name, 0, عدم, tv, copy, ASSIGN_DECL, 0);
}

/*
 * Set variable "name" to value in "tv_arg".
 * When "sid" is non-zero "name" is in the script with this ID.
 * If the variable already exists and "is_const" is FALSE the value is updated.
 * Otherwise the variable is created.
 */
    صحيح
set_var_const(
    char_u	*name,
    scid_T	sid,
    type_T	*type_arg,
    typval_T	*tv_arg,
    صحيح		copy,	    // make copy of value in "tv"
    صحيح		flags_arg,  // ASSIGN_CONST, ASSIGN_FINAL, etc.
    صحيح		var_idx)    // index for ":let [a, b] = list"
{
    typval_T	*tv = tv_arg;
    type_T	*type = type_arg;
    typval_T	bool_tv;
    dictitem_T	*di;
    typval_T	*dest_tv;
    char_u	*varname;
    char_u	*name_tofree = عدم;
    hashtab_T	*ht = عدم;
    صحيح		is_script_local;
    صحيح		vim9script = in_vim9script();
    صحيح		var_in_vim9script;
    صحيح		var_in_autoload = FALSE;
    صحيح		flags = flags_arg;
    صحيح		free_tv_arg = !copy;  // free tv_arg if not used
    صحيح		rc = FAIL;

    אם (sid != 0)
    {
	varname = عدم;
	אם (SCRIPT_ID_VALID(sid))
	{
	    char_u	*auto_name = عدم;
	    אם (find_var_autoload_prefix(name, sid, &ht, &auto_name) != عدم)
	    {
		var_in_autoload = TRUE;
		varname = auto_name;
		name_tofree = varname;
	    }
	    אחרת
		ht = &SCRIPT_VARS(sid);
	}
	אם (varname == عدم)
	    varname = name;
    }
    אחרת
    {
	scriptitem_T	*si;
	char_u		*auto_name = عدم;

	אם (in_vim9script()
	    && SCRIPT_ID_VALID(current_sctx.sc_sid)
	    && (si = SCRIPT_ITEM(current_sctx.sc_sid))
					      ->sn_autoload_prefix != عدم
	    && (is_export
		|| find_var_autoload_prefix(name, 0, عدم, &auto_name)
								    != عدم))
	{
	    // In a vim9 autoload script an exported variable is put in the
	    // global namespace with the autoload prefix.
	    var_in_autoload = TRUE;
	    varname = auto_name != عدم ? auto_name
		      : concat_str(si->sn_autoload_prefix, name);
	    אם (varname == عدم)
		اذهب failed;
	    name_tofree = varname;
	    ht = &globvarht;
	}
	אחרת
	    ht = find_var_ht(name, &varname);
    }
    אם (ht == عدم || *varname == NUL)
    {
	semsg(_(e_illegal_variable_name_str), name);
	اذهب failed;
    }
    is_script_local = ht == get_script_local_ht() || sid != 0
							    || var_in_autoload;

    אם (vim9script
	    && !is_script_local
	    && (flags & (ASSIGN_NO_DECL | ASSIGN_DECL)) == 0
	    && (flags & (ASSIGN_CONST | ASSIGN_FINAL)) == 0
	    && name[1] == ':')
    {
	vim9_declare_error(name);
	اذهب failed;
    }
    אם ((flags & ASSIGN_FOR_LOOP) && is_scoped_variable(name))
	// Do not make g:var, w:var, b:var or t:var final.
	flags &= ~ASSIGN_FINAL;

    var_in_vim9script = is_script_local && current_script_is_vim9();
    אם (var_in_vim9script && name[0] == '_' && name[1] == NUL)
    {
	// For "[a, _] = list" the underscore is ignored.
	אם ((flags & ASSIGN_UNPACK) == 0)
	    emsg(_(e_cannot_use_underscore_here));
	اذهب failed;
    }

    di = find_var_in_ht(ht, 0, varname, TRUE);

    אם (di == عدم && var_in_vim9script)
    {
	imported_T  *import = find_imported(varname, 0, FALSE);

	אם (import != عدم)
	{
	    // imported name space cannot be used
	    אם ((flags & ASSIGN_NO_DECL) == 0)
	    {
		semsg(_(e_redefining_imported_item_str), name);
		اذهب failed;
	    }
	    semsg(_(e_cannot_use_str_itself_it_is_imported), name);
	    اذهب failed;
	}
	אם (!in_vim9script())
	{
	    semsg(_(e_cannot_create_vim9_script_variable_in_function_str),
									 name);
	    اذهب failed;
	}
    }

    // Search in parent scope which is possible to reference from lambda
    אם (di == عدم)
	di = find_var_in_scoped_ht(name, TRUE);

    אם ((tv->v_type == VAR_FUNC || tv->v_type == VAR_PARTIAL)
				  && var_wrong_func_name(name, di == عدم))
	اذهب failed;

    אם (need_convert_to_bool(type, tv))
    {
	// Destination is a bool and the value is not, but it can be
	// converted.
	CLEAR_FIELD(bool_tv);
	bool_tv.v_type = VAR_BOOL;
	bool_tv.vval.v_number = tv2bool(tv) ? VVAL_TRUE : VVAL_FALSE;
	tv = &bool_tv;
    }

    אם (di != عدم)
    {
	// Item already exists.  Allowed to replace when reloading.
	אם ((di->di_flags & DI_FLAGS_RELOAD) == 0)
	{
	    אם ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					 && (flags & ASSIGN_FOR_LOOP) == 0)
	    {
		emsg(_(e_cannot_modify_existing_variable));
		اذهب failed;
	    }

	    אם (is_script_local && vim9script
			  && (flags & (ASSIGN_NO_DECL | ASSIGN_DECL)) == 0)
	    {
		semsg(_(e_redefining_script_item_str), name);
		اذهب failed;
	    }

	    אם (check_typval_is_value(&di->di_tv) == FAIL)
		اذهب failed;

	    // List and Blob types can be modified in-place using the "+="
	    // compound operator.  For other types, this is not allowed.
	    صحيح type_inplace_modifiable =
		(di->di_tv.v_type == VAR_LIST || di->di_tv.v_type == VAR_BLOB);

	    אם (var_in_vim9script && (flags & ASSIGN_FOR_LOOP) == 0
		    && ((flags & ASSIGN_COMPOUND_OP) == 0
			|| !type_inplace_modifiable))
	    {
		where_T where = WHERE_INIT;
		svar_T  *sv = find_typval_in_script(&di->di_tv, sid, TRUE);

		אם (sv != عدم)
		{
		    // check the type and adjust to bool if needed
		    אם (var_idx > 0)
		    {
			where.wt_index = var_idx;
			where.wt_kind = WT_VARIABLE;
		    }
		    אם (check_script_var_type(sv, tv, name, where) == FAIL)
			اذهب failed;
		    אם (type == عدم)
			type = sv->sv_type;
		    sv->sv_flags |= SVFLAG_ASSIGNED;
		}
	    }

	    // Modifying a final variable with a List value using the "+="
	    // operator is allowed.  For other types, it is not allowed.
	    אם ((((flags & ASSIGN_FOR_LOOP) == 0 || (flags & ASSIGN_DECL) == 0)
			&& ((flags & ASSIGN_COMPOUND_OP) == 0
			    || !type_inplace_modifiable))
				 ? var_check_permission(di, name) == FAIL
				 : var_check_ro(di->di_flags, name, FALSE))
		اذهب failed;
	}
	אחרת
	{
	    // can only redefine once
	    di->di_flags &= ~DI_FLAGS_RELOAD;

	    // A Vim9 script-local variable is also present in sn_all_vars
	    // and sn_var_vals.  It may set "type" from "tv".
	    אם (var_in_vim9script || var_in_autoload)
		update_vim9_script_var(FALSE, di,
			var_in_autoload ? name : di->di_key, flags,
			tv, &type, (flags & ASSIGN_NO_MEMBER_TYPE) == 0);
	}

	// existing variable, need to clear the value

	// In Vim9 script a String v: variable only takes a String.
	אם (ht == &vimvarht && di->di_tv.v_type == VAR_STRING
							    && in_vim9script())
	{
	    where_T	where = WHERE_INIT;

	    אם (check_typval_type(&t_string, tv, where) == FAIL)
		اذهب failed;
	}

	// Handle setting internal v: variables separately where needed to
	// prevent changing the type.
	صحيح type_error = FALSE;
	אם (ht == &vimvarht
		&& !before_set_vvar(varname, di, tv, copy, &type_error))
	{
	    אם (type_error)
		semsg(_(e_setting_v_str_to_value_with_wrong_type), varname);
	    اذهب failed;
	}

	clear_tv(&di->di_tv);

	אם ((flags & ASSIGN_UPDATE_BLOCK_ID)
				       && SCRIPT_ID_VALID(current_sctx.sc_sid))
	{
	    scriptitem_T *si = SCRIPT_ITEM(current_sctx.sc_sid);

	    update_script_var_block_id(name, si->sn_current_block_id);
	}
    }
    אחרת
    {
	// Item not found, check if a function already exists.
	אם (is_script_local && (flags & (ASSIGN_NO_DECL | ASSIGN_DECL)) == 0
	       && lookup_scriptitem(name, STRLEN(name), FALSE, عدم) == OK)
	{
	    semsg(_(e_redefining_script_item_str), name);
	    اذهب failed;
	}

	// add a new variable
	אם (var_in_vim9script && (flags & ASSIGN_NO_DECL))
	{
	    semsg(_(e_unknown_variable_str), name);
	    اذهب failed;
	}

	אם (check_hashtab_frozen(ht, "add variable"))
	    اذهب failed;

	// Can't add "v:" or "a:" variable.
	אם (ht == &vimvarht || ht == get_funccal_args_ht())
	{
	    semsg(_(e_illegal_variable_name_str), name);
	    اذهب failed;
	}

	// Make sure the variable name is valid.  In Vim9 script an
	// autoload variable must be prefixed with "g:" unless in an
	// autoload script.
	אם (!valid_varname(varname, -1, !vim9script
			|| STRNCMP(name, "g:", 2) == 0 || var_in_autoload))
	    اذهب failed;

	di = alloc(offsetof(dictitem_T, di_key) + STRLEN(varname) + 1);
	אם (di == عدم)
	    اذهب failed;
	STRCPY(di->di_key, varname);
	אם (hash_add(ht, DI2HIKEY(di), "add variable") == FAIL)
	{
	    vim_free(di);
	    اذهب failed;
	}
	di->di_flags = DI_FLAGS_ALLOC;
	אם (flags & (ASSIGN_CONST | ASSIGN_FINAL))
	    di->di_flags |= DI_FLAGS_LOCK;

	// A Vim9 script-local variable is also added to sn_all_vars and
	// sn_var_vals. It may set "type" from "tv".
	אם (var_in_vim9script || var_in_autoload)
	    update_vim9_script_var(TRUE, di,
		    var_in_autoload ? name : di->di_key, flags,
			  tv, &type, (flags & ASSIGN_NO_MEMBER_TYPE) == 0);
    }

    dest_tv = &di->di_tv;
    אם (copy || tv->v_type == VAR_NUMBER || tv->v_type == VAR_FLOAT)
	copy_tv(tv, dest_tv);
    אחרת
    {
	*dest_tv = *tv;
	dest_tv->v_lock = 0;
	init_tv(tv);
    }
    free_tv_arg = FALSE;

    אם (vim9script && type != عدم)
	set_tv_type(dest_tv, type);

    // ":const var = value" locks the value
    // ":final var = value" locks "var"
    אם (flags & ASSIGN_CONST)
	// Like :lockvar! name: lock the value and what it contains, but only
	// if the reference count is up to one.  That locks only literal
	// values.
	item_lock(dest_tv, DICT_MAXNEST, TRUE, TRUE);

    rc = OK;

failed:
    vim_free(name_tofree);
    אם (free_tv_arg)
	clear_tv(tv_arg);

    החזר rc;
}

/*
 * Check in this order for backwards compatibility:
 * - Whether the variable is read-only
 * - Whether the variable value is locked
 * - Whether the variable is locked
 * NOTE: "name" is only used for error messages.
 */
    صحيح
var_check_permission(dictitem_T *di, char_u *name)
{
    אם (var_check_ro(di->di_flags, name, FALSE)
		    || value_check_lock(di->di_tv.v_lock, name, FALSE)
		    || var_check_lock(di->di_flags, name, FALSE))
	החזר FAIL;
    החזר OK;
}

/*
 * Return TRUE if di_flags "flags" indicates variable "name" is read-only.
 * Also give an error message.
 */
    صحيح
var_check_ro(صحيح flags, char_u *name, صحيح use_gettext)
{
    אם (flags & DI_FLAGS_RO)
    {
	אם (name == عدم)
	    emsg(_(e_cannot_change_readonly_variable));
	אחרת
	    semsg(_(e_cannot_change_readonly_variable_str),
				       use_gettext ? (char_u *)_(name) : name);
	החזר TRUE;
    }
    אם ((flags & DI_FLAGS_RO_SBX) && sandbox)
    {
	אם (name == عدم)
	    emsg(_(e_cannot_set_variable_in_sandbox));
	אחרת
	    semsg(_(e_cannot_set_variable_in_sandbox_str),
				       use_gettext ? (char_u *)_(name) : name);
	החזר TRUE;
    }
    החזר FALSE;
}

/*
 * Return TRUE if di_flags "flags" indicates variable "name" is locked.
 * Also give an error message.
 */
    صحيح
var_check_lock(صحيح flags, char_u *name, صحيح use_gettext)
{
    אם (flags & DI_FLAGS_LOCK)
    {
	semsg(_(e_variable_is_locked_str),
				       use_gettext ? (char_u *)_(name) : name);
	החזר TRUE;
    }
    החזר FALSE;
}

/*
 * Return TRUE if di_flags "flags" indicates variable "name" is fixed.
 * Also give an error message.
 */
    صحيح
var_check_fixed(صحيح flags, char_u *name, صحيح use_gettext)
{
    אם (flags & DI_FLAGS_FIX)
    {
	אם (name == عدم)
	    emsg(_(e_cannot_delete_variable));
	אחרת
	    semsg(_(e_cannot_delete_variable_str),
				      use_gettext ? (char_u *)_(name) : name);
	החזר TRUE;
    }
    החזר FALSE;
}

/*
 * Check if a funcref is assigned to a valid variable name.
 * Return TRUE and give an error if not.
 */
    صحيح
var_wrong_func_name(
    char_u *name,    // points to start of variable name
    صحيح    new_var)  // TRUE when creating the variable
{
    // Allow for w: b: s: and t:.  In Vim9 script s: is not allowed, because
    // the name can be used without the s: prefix.
    // Allow autoload variable.
    אם (!((vim_strchr((char_u *)"wbt", name[0]) != عدم
		    || (!in_vim9script() && name[0] == 's')) && name[1] == ':')
	    && !ASCII_ISUPPER((name[0] != NUL && name[1] == ':')
						     ? name[2] : name[0])
	    && vim_strchr(name, '#') == عدم)
    {
	semsg(_(e_funcref_variable_name_must_start_with_capital_str), name);
	החזר TRUE;
    }
    // Don't allow hiding a function.  When "v" is not NULL we might be
    // assigning another function to the same var, the type is checked
    // below.
    אם (new_var && function_exists(name, FALSE))
    {
	semsg(_(e_variable_name_conflicts_with_existing_function_str),
								    name);
	החזר TRUE;
    }
    החזר FALSE;
}

/*
 * Return TRUE if "flags" indicates variable "name" has a locked (immutable)
 * value.  Also give an error message, using "name" or _("name") when
 * "use_gettext" is TRUE.
 */
    صحيح
value_check_lock(صحيح lock, char_u *name, صحيح use_gettext)
{
    אם (lock & VAR_LOCKED)
    {
	אם (name == عدم)
	    emsg(_(e_value_is_locked));
	אחרת
	    semsg(_(e_value_is_locked_str),
				       use_gettext ? (char_u *)_(name) : name);
	החזר TRUE;
    }
    אם (lock & VAR_FIXED)
    {
	אם (name == عدم)
	    emsg(_(e_cannot_change_value));
	אחרת
	    semsg(_(e_cannot_change_value_of_str),
				       use_gettext ? (char_u *)_(name) : name);
	החזר TRUE;
    }
    החזר FALSE;
}

/*
 * Check if a variable name is valid.  When "autoload" is true "#" is allowed.
 * If "len" is -1 use all of "varname", otherwise up to "varname[len]".
 * Return FALSE and give an error if not.
 */
    صحيح
valid_varname(char_u *varname, صحيح len, صحيح autoload)
{
    char_u *p;

    עבור (p = varname; len < 0 ? *p != NUL : p < varname + len; ++p)
	אם (!eval_isnamec1(*p) && (p == varname || !VIM_ISDIGIT(*p))
					 && !(autoload && *p == AUTOLOAD_CHAR))
	{
	    semsg(_(e_illegal_variable_name_str), varname);
	    החזר FALSE;
	}
    החזר TRUE;
}

/*
 * Implements the logic to retrieve local variable and option values.
 * Used by "getwinvar()" "gettabvar()" "gettabwinvar()" "getbufvar()".
 */
    ساكن فراغ
get_var_from(
    char_u	*varname,
    typval_T	*rettv,
    typval_T	*deftv,	    // Default value if not found.
    صحيح		htname,	    // 't'ab, 'w'indow or 'b'uffer local.
    tabpage_T	*tp,	    // can be NULL
    win_T	*win,
    buf_T	*buf)	    // Ignored if htname is not 'b'.
{
    dictitem_T	*v;
    صحيح		done = FALSE;
    switchwin_T	switchwin;
    صحيح		need_switch_win;
    صحيح		do_change_curbuf = buf != عدم && htname == 'b';

    ++emsg_off;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = عدم;

    אם (varname != عدم && tp != عدم && win != عدم
	    && (htname != 'b' || buf != عدم))
    {
	// Set curwin to be our win, temporarily.  Also set the tabpage,
	// otherwise the window is not valid. Only do this when needed,
	// autocommands get blocked.
	// If we have a buffer reference avoid the switching, we're saving and
	// restoring curbuf directly.
	need_switch_win = !(tp == curtab && win == curwin) && !do_change_curbuf;
	אם (!need_switch_win || switch_win(&switchwin, win, tp, TRUE) == OK)
	{
	    // Handle options. There are no tab-local options.
	    אם (*varname == '&' && htname != 't')
	    {
		buf_T	*save_curbuf = curbuf;

		// Change curbuf so the option is read from the correct buffer.
		אם (do_change_curbuf)
		    curbuf = buf;

		אם (varname[1] == NUL)
		{
		    // get all window-local or buffer-local options in a dict
		    dict_T	*opts = get_winbuf_options(htname == 'b');

		    אם (opts != عدم)
		    {
			rettv_dict_set(rettv, opts);
			done = TRUE;
		    }
		}
		אחרת אם (eval_option(&varname, rettv, TRUE) == OK)
		    // Local option
		    done = TRUE;

		curbuf = save_curbuf;
	    }
	    אחרת אם (*varname == NUL)
	    {
		// Empty string: return a dict with all the local variables.
		אם (htname == 'b')
		    v = &buf->b_bufvar;
		אחרת אם (htname == 'w')
		    v = &win->w_winvar;
		אחרת
		    v = &tp->tp_winvar;
		copy_tv(&v->di_tv, rettv);
		done = TRUE;
	    }
	    אחרת
	    {
		hashtab_T	*ht;

		אם (htname == 'b')
		    ht = &buf->b_vars->dv_hashtab;
		אחרת אם (htname == 'w')
		    ht = &win->w_vars->dv_hashtab;
		אחרת
		    ht = &tp->tp_vars->dv_hashtab;

		// Look up the variable.
		v = find_var_in_ht(ht, htname, varname, FALSE);
		אם (v != عدم)
		{
		    copy_tv(&v->di_tv, rettv);
		    done = TRUE;
		}
	    }
	}

	אם (need_switch_win)
	    // restore previous notion of curwin
	    restore_win(&switchwin, TRUE);
    }

    אם (!done && deftv->v_type != VAR_UNKNOWN)
	// use the default value
	copy_tv(deftv, rettv);

    --emsg_off;
}

/*
 * getwinvar() and gettabwinvar()
 */
    ساكن فراغ
getwinvar(
    typval_T	*argvars,
    typval_T	*rettv,
    صحيح		off)	    // 1 for gettabwinvar()
{
    char_u	*varname;
    tabpage_T	*tp;
    win_T	*win;

    אם (off == 1)
	tp = find_tabpage((صحيح)tv_get_number_chk(&argvars[0], عدم));
    אחרת
	tp = curtab;
    win = find_win_by_nr(&argvars[off], tp);
    varname = tv_get_string_chk(&argvars[off + 1]);

    get_var_from(varname, rettv, &argvars[off + 2], 'w', tp, win, عدم);
}

/*
 * Set option "varname" to the value of "varp" for the current buffer/window.
 */
    ساكن فراغ
set_option_from_tv(char_u *varname, typval_T *varp)
{
    long	numval = 0;
    char_u	*strval;
    char_u	nbuf[NUMBUFLEN];
    صحيح		error = FALSE;

    صحيح		opt_idx = findoption(varname);
    אם (opt_idx < 0)
    {
	semsg(_(e_unknown_option_str_2), varname);
	החזר;
    }
    صحيح		opt_p_flags = get_option_flags(opt_idx);

    אם (varp->v_type == VAR_BOOL)
    {
	אם (opt_p_flags & P_STRING)
	{
	    emsg(_(e_string_required));
	    החזר;
	}
	numval = (long)varp->vval.v_number;
	strval = (char_u *)"0";  // avoid using "false"
    }
    אחרת
    {
	אם ((opt_p_flags & (P_NUM|P_BOOL))
		&& (!in_vim9script() || varp->v_type != VAR_STRING))
	    numval = (long)tv_get_number_chk(varp, &error);
	אם (!error)
	    strval = tv_get_string_buf_chk(varp, nbuf);
    }
    אם (!error && strval != عدم)
	set_option_value_give_err(varname, numval, strval, OPT_LOCAL);
}

/*
 * "setwinvar()" and "settabwinvar()" functions
 */
    ساكن فراغ
setwinvar(typval_T *argvars, صحيح off)
{
    win_T	*win;
    switchwin_T	switchwin;
    صحيح		need_switch_win;
    char_u	*varname, *winvarname;
    typval_T	*varp;
    tabpage_T	*tp = عدم;

    אם (check_secure())
	החזר;

    אם (off == 1)
	tp = find_tabpage((صحيح)tv_get_number_chk(&argvars[0], عدم));
    אחרת
	tp = curtab;
    win = find_win_by_nr(&argvars[off], tp);
    varname = tv_get_string_chk(&argvars[off + 1]);
    varp = &argvars[off + 2];

    אם (win == عدم || varname == عدم)
	החזר;

    need_switch_win = !(tp == curtab && win == curwin);
    אם (!need_switch_win
	    || switch_win(&switchwin, win, tp, TRUE) == OK)
    {
	אם (*varname == '&')
	    set_option_from_tv(varname + 1, varp);
	אחרת
	{
	    winvarname = alloc(STRLEN(varname) + 3);
	    אם (winvarname != عدم)
	    {
		STRCPY(winvarname, "w:");
		STRCPY(winvarname + 2, varname);
		set_var(winvarname, varp, TRUE);
		vim_free(winvarname);
	    }
	}
    }
    אם (need_switch_win)
	restore_win(&switchwin, TRUE);
}

/*
 * reset v:option_new, v:option_old, v:option_oldlocal, v:option_oldglobal,
 * v:option_type, and v:option_command.
 */
    فراغ
reset_v_option_vars(فراغ)
{
    set_vim_var_string(VV_OPTION_NEW, عدم, -1);
    set_vim_var_string(VV_OPTION_OLD, عدم, -1);
    set_vim_var_string(VV_OPTION_OLDLOCAL, عدم, -1);
    set_vim_var_string(VV_OPTION_OLDGLOBAL, عدم, -1);
    set_vim_var_string(VV_OPTION_TYPE, عدم, -1);
    set_vim_var_string(VV_OPTION_COMMAND, عدم, -1);
}

/*
 * Add an assert error to v:errors.
 */
    فراغ
assert_error(garray_T *gap)
{
    بنية vimvar   *vp = &vimvars[VV_ERRORS];

    אם (vp->vv_tv_type != VAR_LIST || vimvars[VV_ERRORS].vv_list == عدم)
	// Make sure v:errors is a list.
	set_vim_var_list(VV_ERRORS, list_alloc());
    list_append_string(vimvars[VV_ERRORS].vv_list, gap->ga_data, gap->ga_len);
}

    صحيح
var_exists(char_u *var)
{
    char_u	*arg = var;
    char_u	*name;
    char_u	*tofree;
    typval_T    tv;
    صحيح		len = 0;
    صحيح		n = FALSE;

    // get_name_len() takes care of expanding curly braces
    name = var;
    len = get_name_len(&arg, &tofree, TRUE, FALSE);
    אם (len > 0)
    {
	אם (tofree != عدم)
	    name = tofree;
	n = (eval_variable(name, len, 0, &tv, عدم,
				 EVAL_VAR_NOAUTOLOAD + EVAL_VAR_IMPORT) == OK);
	אם (n)
	{
	    // handle d.key, l[idx], f(expr)
	    arg = skipwhite(arg);
	    n = (handle_subscript(&arg, name, &tv, &EVALARG_EVALUATE,
								 FALSE) == OK);
	    אם (n)
		clear_tv(&tv);
	}
    }
    אם (*arg != NUL)
	n = FALSE;

    vim_free(tofree);
    החזר n;
}

ساكن lval_T	*redir_lval = عدم;
#define EVALCMD_BUSY (redir_lval == (lval_T *)&redir_lval)
ساكن garray_T redir_ga;	// only valid when redir_lval is not NULL
ساكن char_u	*redir_endp = عدم;
ساكن char_u	*redir_varname = عدم;

    صحيح
alloc_redir_lval(فراغ)
{
    redir_lval = ALLOC_CLEAR_ONE(lval_T);
    אם (redir_lval == عدم)
	החזר FAIL;
    החזר OK;
}

    فراغ
clear_redir_lval(فراغ)
{
    VIM_CLEAR(redir_lval);
}

    فراغ
init_redir_ga(فراغ)
{
    ga_init2(&redir_ga, حجم(حرف), 500);
}

/*
 * Start recording command output to a variable
 * When "append" is TRUE append to an existing variable.
 * Returns OK if successfully completed the setup.  FAIL otherwise.
 */
    صحيح
var_redir_start(char_u *name, صحيح append)
{
    صحيح		called_emsg_before;
    typval_T	tv;

    // Catch a bad name early.
    אם (!eval_isnamec1(*name))
    {
	emsg(_(e_invalid_argument));
	החזר FAIL;
    }

    // Make a copy of the name, it is used in redir_lval until redir ends.
    redir_varname = vim_strsave(name);
    אם (redir_varname == عدم)
	החזר FAIL;

    אם (alloc_redir_lval() == FAIL)
    {
	var_redir_stop();
	החזר FAIL;
    }

    // The output is stored in growarray "redir_ga" until redirection ends.
    init_redir_ga();

    // Parse the variable name (can be a dict or list entry).
    redir_endp = get_lval(redir_varname, عدم, redir_lval, FALSE, FALSE, 0,
							     FNE_CHECK_START);
    אם (redir_endp == عدم || redir_lval->ll_name == عدم || *redir_endp != NUL)
    {
	clear_lval(redir_lval);
	אם (redir_endp != عدم && *redir_endp != NUL)
	    // Trailing characters are present after the variable name
	    semsg(_(e_trailing_characters_str), redir_endp);
	אחרת
	    semsg(_(e_invalid_argument_str), name);
	redir_endp = عدم;  // don't store a value, only cleanup
	var_redir_stop();
	החזר FAIL;
    }

    // check if we can write to the variable: set it to or append an empty
    // string
    called_emsg_before = called_emsg;
    tv.v_type = VAR_STRING;
    tv.vval.v_string = (char_u *)"";
    אם (append)
	set_var_lval(redir_lval, redir_endp, &tv, TRUE,
					     ASSIGN_NO_DECL, (char_u *)".", 0);
    אחרת
	set_var_lval(redir_lval, redir_endp, &tv, TRUE,
					     ASSIGN_NO_DECL, (char_u *)"=", 0);
    clear_lval(redir_lval);
    אם (called_emsg > called_emsg_before)
    {
	redir_endp = عدم;  // don't store a value, only cleanup
	var_redir_stop();
	החזר FAIL;
    }

    החזר OK;
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
    فراغ
var_redir_str(char_u *value, صحيح value_len)
{
    صحيح		len;

    אם (redir_lval == عدم)
	החזר;

    אם (value_len == -1)
	len = (صحيح)STRLEN(value);	// Append the entire string
    אחרת
	len = value_len;		// Append only "value_len" characters

    אם (ga_grow(&redir_ga, len) == OK)
    {
	mch_memmove((حرف *)redir_ga.ga_data + redir_ga.ga_len, value, len);
	redir_ga.ga_len += len;
    }
    אחרת
	var_redir_stop();
}

/*
 * Stop redirecting command output to a variable.
 * Frees the allocated memory.
 */
    فراغ
var_redir_stop(فراغ)
{
    typval_T	tv;

    אם (EVALCMD_BUSY)
    {
	redir_lval = عدم;
	החזר;
    }

    אם (redir_lval != عدم)
    {
	// If there was no error: assign the text to the variable.
	אם (redir_endp != عدم)
	{
	    ga_append(&redir_ga, NUL);  // Append the trailing NUL.
	    tv.v_type = VAR_STRING;
	    tv.vval.v_string = redir_ga.ga_data;
	    // Call get_lval() again, if it's inside a Dict or List it may
	    // have changed.
	    redir_endp = get_lval(redir_varname, عدم, redir_lval,
					FALSE, FALSE, 0, FNE_CHECK_START);
	    אם (redir_endp != عدم && redir_lval->ll_name != عدم)
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
get_clear_redir_ga(فراغ)
{
    char_u *res;

    ga_append(&redir_ga, NUL);  // Append the trailing NUL.
    res = redir_ga.ga_data;
    redir_ga.ga_data = عدم;
    החזר res;
}

/*
 * "gettabvar()" function
 */
    فراغ
f_gettabvar(typval_T *argvars, typval_T *rettv)
{
    char_u	*varname;
    tabpage_T	*tp;
    win_T	*win = عدم;

    אם (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	החזר;

    varname = tv_get_string_chk(&argvars[1]);
    tp = find_tabpage((صحيح)tv_get_number_chk(&argvars[0], عدم));
    אם (tp != عدم)
	win = tp == curtab || tp->tp_firstwin == عدم ? firstwin
		: tp->tp_firstwin;

    get_var_from(varname, rettv, &argvars[2], 't', tp, win, عدم);
}

/*
 * "gettabwinvar()" function
 */
    فراغ
f_gettabwinvar(typval_T *argvars, typval_T *rettv)
{
    אם (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL))
	החזר;

    getwinvar(argvars, rettv, 1);
}

/*
 * "getwinvar()" function
 */
    فراغ
f_getwinvar(typval_T *argvars, typval_T *rettv)
{
    אם (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	החזר;

    getwinvar(argvars, rettv, 0);
}

/*
 * "getbufvar()" function
 */
    فراغ
f_getbufvar(typval_T *argvars, typval_T *rettv)
{
    char_u	*varname;
    buf_T	*buf;

    אם (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	החזר;

    varname = tv_get_string_chk(&argvars[1]);
    buf = tv_get_buf_from_arg(&argvars[0]);

    get_var_from(varname, rettv, &argvars[2], 'b', curtab, curwin, buf);
}

/*
 * "settabvar()" function
 */
    فراغ
f_settabvar(typval_T *argvars, typval_T *rettv UNUSED)
{
    tabpage_T	*save_curtab;
    tabpage_T	*tp;
    tabpage_T	*save_lu_tp;
    char_u	*varname, *tabvarname;
    typval_T	*varp;

    אם (check_secure())
	החזר;

    אם (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	החזר;

    tp = find_tabpage((صحيح)tv_get_number_chk(&argvars[0], عدم));
    varname = tv_get_string_chk(&argvars[1]);
    varp = &argvars[2];

    אם (varname == عدم || tp == عدم)
	החזר;

    save_curtab = curtab;
    save_lu_tp = lastused_tabpage;
    goto_tabpage_tp(tp, FALSE, FALSE);

    tabvarname = alloc(STRLEN(varname) + 3);
    אם (tabvarname != عدم)
    {
	STRCPY(tabvarname, "t:");
	STRCPY(tabvarname + 2, varname);
	set_var(tabvarname, varp, TRUE);
	vim_free(tabvarname);
    }

    // Restore current tabpage and last accessed tabpage.
    אם (valid_tabpage(save_curtab))
    {
	goto_tabpage_tp(save_curtab, FALSE, FALSE);
	אם (valid_tabpage(save_lu_tp))
	    lastused_tabpage = save_lu_tp;
    }
}

/*
 * "settabwinvar()" function
 */
    فراغ
f_settabwinvar(typval_T *argvars, typval_T *rettv UNUSED)
{
    אם (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL))
	החזר;

    setwinvar(argvars, 1);
}

/*
 * "setwinvar()" function
 */
    فراغ
f_setwinvar(typval_T *argvars, typval_T *rettv UNUSED)
{
    אם (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	החזר;

    setwinvar(argvars, 0);
}

/*
 * "setbufvar()" function
 */
    فراغ
f_setbufvar(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    char_u	*varname, *bufvarname;
    typval_T	*varp;

    אם (check_secure())
	החזר;

    אם (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	החזר;

    varname = tv_get_string_chk(&argvars[1]);
    buf = tv_get_buf_from_arg(&argvars[0]);
    varp = &argvars[2];

    אם (buf == عدم || varname == عدم)
	החזר;

    אם (*varname == '&')
    {
	aco_save_T	aco;
	// safe the current window position, it could
	// change because of 'scrollbind' window-local
	// options
	linenr_T	old_topline = curwin->w_topline;

	// Set curbuf to be our buf, temporarily.
	aucmd_prepbuf(&aco, buf);
	אם (curbuf == buf)
	{
	    // Only when it worked to set "curbuf".
	    set_option_from_tv(varname + 1, varp);

	    // reset notion of buffer
	    aucmd_restbuf(&aco);
	}
	curwin->w_topline = old_topline;
    }
    אחרת
    {
	bufvarname = alloc(STRLEN(varname) + 3);
	אם (bufvarname != عدم)
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
    صحيح		r = OK;

    CLEAR_FIELD(res);
    אם (arg->v_type == VAR_PARTIAL && arg->vval.v_partial != عدم)
    {
	res.cb_partial = arg->vval.v_partial;
	++res.cb_partial->pt_refcount;
	res.cb_name = partial_name(res.cb_partial);
    }
    אחרת
    {
	אם (arg->v_type == VAR_STRING && arg->vval.v_string != عدم
					       && SAFE_isdigit(*arg->vval.v_string))
	    r = FAIL;
	אחרת אם (arg->v_type == VAR_FUNC || arg->v_type == VAR_STRING)
	{
	    res.cb_name = arg->vval.v_string;
	    אם (arg->v_type == VAR_STRING)
	    {
		char_u *name = get_scriptlocal_funcname(arg->vval.v_string);
		אם (name != عدم)
		{
		    res.cb_name = name;
		    res.cb_free_name = TRUE;
		}
	    }
	    func_ref(res.cb_name);
	}
	אחרת אם (arg->v_type == VAR_NUMBER && arg->vval.v_number == 0)
	    res.cb_name = (char_u *)"";
	אחרת
	    r = FAIL;

	אם (r == FAIL)
	{
	    emsg(_(e_invalid_callback_argument));
	    res.cb_name = عدم;
	}
    }
    החזר res;
}

/*
 * Copy a callback into a typval_T.
 */
    فراغ
put_callback(callback_T *cb, typval_T *tv)
{
    אם (cb->cb_partial != عدم)
    {
	tv->v_type = VAR_PARTIAL;
	tv->vval.v_partial = cb->cb_partial;
	++tv->vval.v_partial->pt_refcount;
    }
    אחרת
    {
	tv->v_type = VAR_FUNC;
	tv->vval.v_string = vim_strsave(cb->cb_name);
	func_ref(cb->cb_name);
    }
}

    ساكن bool
does_callback_own_cb_name(callback_T *cb)
{
    // If cb_partial != NULL then *cb->cb_name is owned by the partial.
    החזר cb->cb_partial || cb->cb_free_name;
}

/*
 * Make a copy of "src" into "dest", allocating the function name if needed,
 * without incrementing the refcount.
 */
    فراغ
set_callback(callback_T *dest, callback_T *src)
{
    *dest = *src;
    אם (!does_callback_own_cb_name(src))
    {
	dest->cb_name = vim_strsave(src->cb_name);
	dest->cb_free_name = TRUE;
    }
    *src = (callback_T){0};
}

/*
 * Copy callback from "src" to "dest", incrementing the refcounts.
 */
    فراغ
copy_callback(callback_T *dest, callback_T *src)
{
    dest->cb_partial = src->cb_partial;
    אם (dest->cb_partial != عدم)
    {
	dest->cb_name = src->cb_name;
	dest->cb_free_name = FALSE;
	++dest->cb_partial->pt_refcount;
    }
    אחרת
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
    فراغ
expand_autload_callback(callback_T *cb)
{
    char_u	*name;
    char_u	*p;
    imported_T	*import;

    אם (!in_vim9script() || cb->cb_name == عدم
	    || (!cb->cb_free_name
	       && (cb->cb_partial == عدم || cb->cb_partial->pt_name == عدم)))
	החזר;
    אם (cb->cb_partial != عدم)
	name = cb->cb_partial->pt_name;
    אחרת
	name = cb->cb_name;
    p = vim_strchr(name, '.');
    אם (p == عدم)
	החזר;

    import = find_imported(name, p - name, FALSE);
    אם (import == عدم || !SCRIPT_ID_VALID(import->imp_sid))
	החזר;

    scriptitem_T *si = SCRIPT_ITEM(import->imp_sid);
    אם (si->sn_autoload_prefix == عدم)
	החזר;

    char_u *newname = concat_str(si->sn_autoload_prefix, p + 1);
    אם (newname == عدم)
	החזר;

    אם (cb->cb_partial != عدم)
    {
	אם (cb->cb_name == cb->cb_partial->pt_name)
	    cb->cb_name = newname;
	vim_free(cb->cb_partial->pt_name);
	cb->cb_partial->pt_name = newname;
    }
    אחרת
    {
	vim_free(cb->cb_name);
	cb->cb_name = newname;
    }
}

/*
 * Unref/free "callback" returned by get_callback() or set_callback().
 */
    فراغ
free_callback(callback_T *callback)
{
    אם (callback->cb_partial != عدم)
    {
	partial_unref(callback->cb_partial);
	callback->cb_partial = عدم;
    }
    אחרת אם (callback->cb_name != عدم)
	func_unref(callback->cb_name);
    אם (callback->cb_free_name)
    {
	vim_free(callback->cb_name);
	callback->cb_free_name = FALSE;
    }
    callback->cb_name = عدم;
}

#endif // FEAT_EVAL
