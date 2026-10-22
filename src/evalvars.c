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

ساکن dictitem_T	globvars_var;		// variable used for g:
ساکن dict_T		globvardict;		// Dictionary with g: variables
#تعریف_کرو globvarht globvardict.dv_hashtab

/*
 * Old Vim variables such as "v:version" are also available without the "v:".
 * Also in functions.  We need a special hashtable for them.
 */
ساکن hashtab_T	compat_hashtab;

/*
 * Array to hold the value of v: variables.
 * The value is in a dictitem, so that it can also be used in the v: scope.
 * The reason to use this table anyway is for very quick access to the
 * variables with the VV_ defines.
 */

// values for vv_flags:
#تعریف_کرو VV_COMPAT	1	// compatible, also used without "v:"
#تعریف_کرو VV_RO		2	// read-only
#تعریف_کرو VV_RO_SBX	4	// read-only in the sandbox

#تعریف_کرو VV_NAME(s, t)	s, {{t, 0, {0}}, 0, {s}}

قسمتعریف ساخت vimvar vimvar_T;

ساکن ساخت vimvar
{
    حرف	*vv_name;	// name of variable, without v:
    dictitem16_T vv_di;		// value and name for key (max 16 chars!)
    type_T	*vv_type;	// type or NULL
    حرف	vv_flags;	// VV_COMPAT, VV_RO, VV_RO_SBX
} vimvars[VV_LEN] =
{
    // The order here must match the VV_ defines in vim.h!
    // Initializing a union does not work, leave tv.vval empty to get zero's.
    {VV_NAME("count",		 VAR_نمبر), تہی, VV_COMPAT+VV_RO},
    {VV_NAME("count1",		 VAR_نمبر), تہی, VV_RO},
    {VV_NAME("prevcount",	 VAR_نمبر), تہی, VV_RO},
    {VV_NAME("errmsg",		 VAR_STRING), تہی, VV_COMPAT},
    {VV_NAME("warningmsg",	 VAR_STRING), تہی, 0},
    {VV_NAME("statusmsg",	 VAR_STRING), تہی, 0},
    {VV_NAME("shell_error",	 VAR_نمبر), تہی, VV_COMPAT+VV_RO},
    {VV_NAME("this_session",	 VAR_STRING), تہی, VV_COMPAT},
    {VV_NAME("version",		 VAR_نمبر), تہی, VV_COMPAT+VV_RO},
    {VV_NAME("lnum",		 VAR_نمبر), تہی, VV_RO_SBX},
    {VV_NAME("termresponse",	 VAR_STRING), تہی, VV_RO},
    {VV_NAME("fname",		 VAR_STRING), تہی, VV_RO},
    {VV_NAME("lang",		 VAR_STRING), تہی, VV_RO},
    {VV_NAME("lc_time",		 VAR_STRING), تہی, VV_RO},
    {VV_NAME("ctype",		 VAR_STRING), تہی, VV_RO},
    {VV_NAME("charconvert_from", VAR_STRING), تہی, VV_RO},
    {VV_NAME("charconvert_to",	 VAR_STRING), تہی, VV_RO},
    {VV_NAME("fname_in",	 VAR_STRING), تہی, VV_RO},
    {VV_NAME("fname_out",	 VAR_STRING), تہی, VV_RO},
    {VV_NAME("fname_new",	 VAR_STRING), تہی, VV_RO},
    {VV_NAME("fname_diff",	 VAR_STRING), تہی, VV_RO},
    {VV_NAME("cmdarg",		 VAR_STRING), تہی, VV_RO},
    {VV_NAME("foldstart",	 VAR_نمبر), تہی, VV_RO_SBX},
    {VV_NAME("foldend",		 VAR_نمبر), تہی, VV_RO_SBX},
    {VV_NAME("folddashes",	 VAR_STRING), تہی, VV_RO_SBX},
    {VV_NAME("foldlevel",	 VAR_نمبر), تہی, VV_RO_SBX},
    {VV_NAME("progname",	 VAR_STRING), تہی, VV_RO},
    {VV_NAME("servername",	 VAR_STRING), تہی, VV_RO},
    {VV_NAME("dying",		 VAR_نمبر), تہی, VV_RO},
    {VV_NAME("exception",	 VAR_STRING), تہی, VV_RO},
    {VV_NAME("throwpoint",	 VAR_STRING), تہی, VV_RO},
    {VV_NAME("register",	 VAR_STRING), تہی, VV_RO},
    {VV_NAME("cmdbang",		 VAR_نمبر), تہی, VV_RO},
    {VV_NAME("insertmode",	 VAR_STRING), تہی, VV_RO},
    {VV_NAME("val",		 VAR_UNKNOWN), تہی, VV_RO},
    {VV_NAME("key",		 VAR_UNKNOWN), تہی, VV_RO},
    {VV_NAME("profiling",	 VAR_نمبر), تہی, VV_RO},
    {VV_NAME("fcs_reason",	 VAR_STRING), تہی, VV_RO},
    {VV_NAME("fcs_choice",	 VAR_STRING), تہی, 0},
    {VV_NAME("beval_bufnr",	 VAR_نمبر), تہی, VV_RO},
    {VV_NAME("beval_winnr",	 VAR_نمبر), تہی, VV_RO},
    {VV_NAME("beval_winid",	 VAR_نمبر), تہی, VV_RO},
    {VV_NAME("beval_lnum",	 VAR_نمبر), تہی, VV_RO},
    {VV_NAME("beval_col",	 VAR_نمبر), تہی, VV_RO},
    {VV_NAME("beval_text",	 VAR_STRING), تہی, VV_RO},
    {VV_NAME("scrollstart",	 VAR_STRING), تہی, 0},
    {VV_NAME("swapname",	 VAR_STRING), تہی, VV_RO},
    {VV_NAME("swapchoice",	 VAR_STRING), تہی, 0},
    {VV_NAME("swapcommand",	 VAR_STRING), تہی, VV_RO},
    {VV_NAME("char",		 VAR_STRING), تہی, 0},
    {VV_NAME("mouse_win",	 VAR_نمبر), تہی, 0},
    {VV_NAME("mouse_winid",	 VAR_نمبر), تہی, 0},
    {VV_NAME("mouse_lnum",	 VAR_نمبر), تہی, 0},
    {VV_NAME("mouse_col",	 VAR_نمبر), تہی, 0},
    {VV_NAME("operator",	 VAR_STRING), تہی, VV_RO},
    {VV_NAME("searchforward",	 VAR_نمبر), تہی, 0},
    {VV_NAME("hlsearch",	 VAR_نمبر), تہی, 0},
    {VV_NAME("oldfiles",	 VAR_LIST), &t_list_string, 0},
    {VV_NAME("windowid",	 VAR_نمبر), تہی, VV_RO},
    {VV_NAME("progpath",	 VAR_STRING), تہی, VV_RO},
    {VV_NAME("completed_item",	 VAR_DICT), &t_dict_string, 0},
    {VV_NAME("option_new",	 VAR_STRING), تہی, VV_RO},
    {VV_NAME("option_old",	 VAR_STRING), تہی, VV_RO},
    {VV_NAME("option_oldlocal",	 VAR_STRING), تہی, VV_RO},
    {VV_NAME("option_oldglobal", VAR_STRING), تہی, VV_RO},
    {VV_NAME("option_command",	 VAR_STRING), تہی, VV_RO},
    {VV_NAME("option_type",	 VAR_STRING), تہی, VV_RO},
    {VV_NAME("errors",		 VAR_LIST), &t_list_string, 0},
    {VV_NAME("false",		 VAR_BOOL), تہی, VV_RO},
    {VV_NAME("true",		 VAR_BOOL), تہی, VV_RO},
    {VV_NAME("none",		 VAR_SPECIAL), تہی, VV_RO},
    {VV_NAME("null",		 VAR_SPECIAL), تہی, VV_RO},
    {VV_NAME("numbermax",	 VAR_نمبر), تہی, VV_RO},
    {VV_NAME("numbermin",	 VAR_نمبر), تہی, VV_RO},
    {VV_NAME("numbersize",	 VAR_نمبر), تہی, VV_RO},
    {VV_NAME("vim_did_enter",	 VAR_نمبر), تہی, VV_RO},
    {VV_NAME("testing",		 VAR_نمبر), تہی, 0},
    {VV_NAME("t_number",	 VAR_نمبر), تہی, VV_RO},
    {VV_NAME("t_string",	 VAR_نمبر), تہی, VV_RO},
    {VV_NAME("t_func",		 VAR_نمبر), تہی, VV_RO},
    {VV_NAME("t_list",		 VAR_نمبر), تہی, VV_RO},
    {VV_NAME("t_dict",		 VAR_نمبر), تہی, VV_RO},
    {VV_NAME("t_float",		 VAR_نمبر), تہی, VV_RO},
    {VV_NAME("t_bool",		 VAR_نمبر), تہی, VV_RO},
    {VV_NAME("t_none",		 VAR_نمبر), تہی, VV_RO},
    {VV_NAME("t_job",		 VAR_نمبر), تہی, VV_RO},
    {VV_NAME("t_channel",	 VAR_نمبر), تہی, VV_RO},
    {VV_NAME("t_blob",		 VAR_نمبر), تہی, VV_RO},
    {VV_NAME("t_class",		 VAR_نمبر), تہی, VV_RO},
    {VV_NAME("t_object",	 VAR_نمبر), تہی, VV_RO},
    {VV_NAME("termrfgresp",	 VAR_STRING), تہی, VV_RO},
    {VV_NAME("termrbgresp",	 VAR_STRING), تہی, VV_RO},
    {VV_NAME("termu7resp",	 VAR_STRING), تہی, VV_RO},
    {VV_NAME("termstyleresp",	 VAR_STRING), تہی, VV_RO},
    {VV_NAME("termblinkresp",	 VAR_STRING), تہی, VV_RO},
    {VV_NAME("event",		 VAR_DICT), تہی, VV_RO},
    {VV_NAME("versionlong",	 VAR_نمبر), تہی, VV_RO},
    {VV_NAME("echospace",	 VAR_نمبر), تہی, VV_RO},
    {VV_NAME("argv",		 VAR_LIST), &t_list_string, VV_RO},
    {VV_NAME("collate",		 VAR_STRING), تہی, VV_RO},
    {VV_NAME("exiting",		 VAR_SPECIAL), تہی, VV_RO},
    {VV_NAME("colornames",       VAR_DICT), &t_dict_string, VV_RO},
    {VV_NAME("sizeofint",	 VAR_نمبر), تہی, VV_RO},
    {VV_NAME("sizeoflong",	 VAR_نمبر), تہی, VV_RO},
    {VV_NAME("sizeofpointer",	 VAR_نمبر), تہی, VV_RO},
    {VV_NAME("maxcol",		 VAR_نمبر), تہی, VV_RO},
    {VV_NAME("python3_version",	 VAR_نمبر), تہی, VV_RO},
    {VV_NAME("t_typealias",	 VAR_نمبر), تہی, VV_RO},
    {VV_NAME("t_enum",		 VAR_نمبر), تہی, VV_RO},
    {VV_NAME("t_enumvalue",	 VAR_نمبر), تہی, VV_RO},
    {VV_NAME("stacktrace",	 VAR_LIST), &t_list_dict_any, VV_RO},
    {VV_NAME("t_tuple",		 VAR_نمبر), تہی, VV_RO},
    {VV_NAME("wayland_display",  VAR_STRING), تہی, VV_RO},
    {VV_NAME("clipmethod",	 VAR_STRING), تہی, VV_RO},
    {VV_NAME("termda1",		 VAR_STRING), تہی, VV_RO},
    {VV_NAME("termosc",	 VAR_STRING), تہی, VV_RO},
    {VV_NAME("vim_did_init",	 VAR_نمبر), تہی, VV_RO},
    {VV_NAME("clipproviders",	 VAR_DICT), تہی, VV_RO},
    {VV_NAME("imagebackend",	 VAR_STRING), تہی, VV_RO}
};

// shorthand
#تعریف_کرو vv_tv_type	vv_di.di_tv.v_type
#تعریف_کرو vv_nr		vv_di.di_tv.vval.v_نمبر
#تعریف_کرو vv_float	vv_di.di_tv.vval.v_float
#تعریف_کرو vv_str		vv_di.di_tv.vval.v_string
#تعریف_کرو vv_list		vv_di.di_tv.vval.v_list
#تعریف_کرو vv_dict		vv_di.di_tv.vval.v_dict
#تعریف_کرو vv_blob		vv_di.di_tv.vval.v_blob
#تعریف_کرو vv_tv		vv_di.di_tv

ساکن dictitem_T	vimvars_var;		// variable used for v:
ساکن dict_T		vimvardict;		// Dictionary with v: variables
#تعریف_کرو vimvarht  vimvardict.dv_hashtab

// for VIM_VERSION_ defines
#شامل "version.h"

ساکن خالی list_glob_vars(صحیح *پہلا);
ساکن خالی list_buf_vars(صحیح *پہلا);
ساکن خالی list_win_vars(صحیح *پہلا);
ساکن خالی list_tab_vars(صحیح *پہلا);
ساکن char_u *list_arg_vars(exarg_T *eap, char_u *arg, صحیح *پہلا);
ساکن char_u *ex_let_one(char_u *arg, typval_T *tv, صحیح copy, صحیح flags, char_u *endchars, char_u *op, صحیح var_idx);
ساکن صحیح do_unlet_var(lval_T *lp, char_u *name_انتہا, exarg_T *eap, صحیح deep, خالی *cookie);
ساکن صحیح do_تالا_var(lval_T *lp, char_u *name_انتہا, exarg_T *eap, صحیح deep, خالی *cookie);
ساکن خالی list_one_var(dictitem_T *v, حرف *سابقہ, صحیح *پہلا);
ساکن خالی list_one_var_a(حرف *سابقہ, char_u *name, صحیح type, char_u *string, صحیح *پہلا);

/*
 * Initialize global and vim special variables
 */
    خالی
evalvars_init(خالی)
{
    صحیح		    i;
    ساخت vimvar   *p;

    init_var_dict(&globvardict, &globvars_var, VAR_DEF_SCOPE);
    init_var_dict(&vimvardict, &vimvars_var, VAR_SCOPE);
    vimvardict.dv_تالا = VAR_FIXED;
    hash_init(&compat_hashtab);

    برائے (i = 0; i < VV_LEN; ++i)
    {
	p = &vimvars[i];
	اگر (p->vv_flags & VV_RO)
	    p->vv_di.di_flags = DI_FLAGS_RO | DI_FLAGS_FIX;
	ورنہ اگر (p->vv_flags & VV_RO_SBX)
	    p->vv_di.di_flags = DI_FLAGS_RO_SBX | DI_FLAGS_FIX;
	ورنہ
	    p->vv_di.di_flags = DI_FLAGS_FIX;

	// add to v: scope dict, unless the value is not always available
	اگر (p->vv_tv_type != VAR_UNKNOWN)
	    hash_جوڑو(&vimvarht, p->vv_di.di_کلید, "initialization");
	اگر (p->vv_flags & VV_COMPAT)
	    // add to compat scope dict
	    hash_جوڑو(&compat_hashtab, p->vv_di.di_کلید, "initialization");
    }
    سیٹ_vim_var_nr(VV_نسخہ, VIM_نسخہ_100);
    سیٹ_vim_var_nr(VV_VERSIONLONG, VIM_نسخہ_100 * 10000 + highest_patch());

    سیٹ_vim_var_nr(VV_SEARCHFORWARD, 1L);
    سیٹ_vim_var_nr(VV_HLSEARCH, 1L);
    سیٹ_vim_var_nr(VV_EXITING, VVAL_NULL);
    سیٹ_vim_var_dict(VV_COMPLETED_شے, dict_alloc_تالا(VAR_FIXED));
    سیٹ_vim_var_list(VV_ERRORS, list_alloc());
    سیٹ_vim_var_dict(VV_EVENT, dict_alloc_تالا(VAR_FIXED));

    سیٹ_vim_var_nr(VV_FALSE, VVAL_FALSE);
    سیٹ_vim_var_nr(VV_TRUE, VVAL_TRUE);
    سیٹ_vim_var_nr(VV_NONE, VVAL_NONE);
    سیٹ_vim_var_nr(VV_NULL, VVAL_NULL);
    سیٹ_vim_var_nr(VV_NUMBERMAX, VARNUM_MAX);
    سیٹ_vim_var_nr(VV_NUMBERMIN, VARNUM_MIN);
    سیٹ_vim_var_nr(VV_NUMBERSIZE, حجم(varnumber_T) * 8);
    سیٹ_vim_var_nr(VV_SIZEOFINT, حجم(صحیح));
    سیٹ_vim_var_nr(VV_SIZEOFLONG, حجم(طویل));
    سیٹ_vim_var_nr(VV_SIZEOFPOINTER, حجم(حرف *));
    سیٹ_vim_var_nr(VV_MAXCOL, MAXCOL);

    سیٹ_vim_var_nr(VV_TYPE_نمبر,  VAR_TYPE_نمبر);
    سیٹ_vim_var_nr(VV_TYPE_STRING,  VAR_TYPE_STRING);
    سیٹ_vim_var_nr(VV_TYPE_FUNC,    VAR_TYPE_FUNC);
    سیٹ_vim_var_nr(VV_TYPE_LIST,    VAR_TYPE_LIST);
    سیٹ_vim_var_nr(VV_TYPE_DICT,    VAR_TYPE_DICT);
    سیٹ_vim_var_nr(VV_TYPE_FLOAT,   VAR_TYPE_FLOAT);
    سیٹ_vim_var_nr(VV_TYPE_BOOL,    VAR_TYPE_BOOL);
    سیٹ_vim_var_nr(VV_TYPE_NONE,    VAR_TYPE_NONE);
    سیٹ_vim_var_nr(VV_TYPE_کام,     VAR_TYPE_کام);
    سیٹ_vim_var_nr(VV_TYPE_CHANNEL, VAR_TYPE_CHANNEL);
    سیٹ_vim_var_nr(VV_TYPE_BLOB,    VAR_TYPE_BLOB);
    سیٹ_vim_var_nr(VV_TYPE_CLASS,   VAR_TYPE_CLASS);
    سیٹ_vim_var_nr(VV_TYPE_OBJECT,  VAR_TYPE_OBJECT);
    سیٹ_vim_var_nr(VV_TYPE_TYPEALIAS,  VAR_TYPE_TYPEALIAS);
    سیٹ_vim_var_nr(VV_TYPE_ENUM,    VAR_TYPE_ENUM);
    سیٹ_vim_var_nr(VV_TYPE_ENUMVALUE,  VAR_TYPE_ENUMVALUE);
    سیٹ_vim_var_nr(VV_TYPE_TUPLE,   VAR_TYPE_TUPLE);

    سیٹ_vim_var_nr(VV_ECHOSPACE,    sc_ستون - 1);

    سیٹ_vim_var_dict(VV_COLORNAMES, dict_alloc());
    سیٹ_vim_var_dict(VV_CLIPPROVIDERS, dict_alloc());

#اگر_تعریف_شدہ FEAT_PYTHON3
    سیٹ_vim_var_nr(VV_PYTHON3_نسخہ, python3_نسخہ());
#اگر_ختم

    // Default for v:register is not 0 but '"'.  This is adjusted once the
    // clipboard has been setup by calling reset_reg_var().
    سیٹ_reg_var(0);
}

#اگر defined(EXITFREE)
/*
 * Free all vim variables information on exit
 */
    خالی
evalvars_clear(خالی)
{
    صحیح		    i;
    ساخت vimvar   *p;

    برائے (i = 0; i < VV_LEN; ++i)
    {
	p = &vimvars[i];
	اگر (p->vv_di.di_tv.v_type == VAR_STRING)
	    VIM_CLEAR(p->vv_str);
	ورنہ اگر (p->vv_di.di_tv.v_type == VAR_LIST)
	{
	    list_unref(p->vv_list);
	    p->vv_list = تہی;
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
    برائے (i = 1; i <= script_اشیاء.ga_len; ++i)
	vars_clear(&SCRIPT_VARS(i));
}
#اگر_ختم

    صحیح
garbage_collect_globvars(صحیح copyID)
{
    واپس سیٹ_ref_اندر_ht(&globvarht, copyID, تہی, تہی);
}

    صحیح
garbage_collect_vimvars(صحیح copyID)
{
    واپس سیٹ_ref_اندر_ht(&vimvarht, copyID, تہی, تہی);
}

    صحیح
garbage_collect_scriptvars(صحیح copyID)
{
    صحیح		    i;
    صحیح		    idx;
    صحیح		    abort = FALSE;
    scriptitem_T    *si;

    برائے (i = 1; i <= script_اشیاء.ga_len; ++i)
    {
	abort = abort || سیٹ_ref_اندر_ht(&SCRIPT_VARS(i), copyID, تہی, تہی);

	si = SCRIPT_شے(i);
	برائے (idx = 0; idx < si->sn_var_vals.ga_len; ++idx)
	{
	    svar_T    *sv = ((svar_T *)si->sn_var_vals.ga_ڈیٹا) + idx;

	    اگر (sv->sv_name != تہی)
		abort = abort || سیٹ_ref_اندر_شے(sv->sv_tv, copyID, تہی, تہی, تہی);
	}
    }

    واپس abort;
}

/*
 * Set an internal variable to a string value. Creates the variable if it does
 * not already exist.
 */
    خالی
سیٹ_اندرونی_string_var(char_u *name, char_u *قدر)
{
    char_u	*val;
    typval_T	*tvp;

    val = vim_strsave(قدر);
    اگر (val == تہی)
	واپس;

    tvp = alloc_string_tv(val);
    اگر (tvp == تہی)
	واپس;

    سیٹ_var(name, tvp, FALSE);
    free_tv(tvp);
}

    صحیح
eval_charconvert(
    char_u	*enc_from,
    char_u	*enc_مقصد,
    char_u	*fname_from,
    char_u	*fname_مقصد)
{
    صحیح		err = FALSE;
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;

    سیٹ_vim_var_string(VV_CC_FROM, enc_from, -1);
    سیٹ_vim_var_string(VV_CC_مقصد, enc_مقصد, -1);
    سیٹ_vim_var_string(VV_FNAME_اندر, fname_from, -1);
    سیٹ_vim_var_string(VV_FNAME_برآمد, fname_مقصد, -1);
    ctx = get_option_sctx("charconvert");
    اگر (ctx != تہی)
	current_sctx = *ctx;

    اگر (eval_مقصد_bool(p_ccv, &err, تہی, FALSE, TRUE))
	err = TRUE;

    سیٹ_vim_var_string(VV_CC_FROM, تہی, -1);
    سیٹ_vim_var_string(VV_CC_مقصد, تہی, -1);
    سیٹ_vim_var_string(VV_FNAME_اندر, تہی, -1);
    سیٹ_vim_var_string(VV_FNAME_برآمد, تہی, -1);
    current_sctx = saved_sctx;

    اگر (err)
	واپس FAIL;
    واپس OK;
}

#اگر defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
    صحیح
eval_printexpr(char_u *fname, char_u *دلائل)
{
    صحیح		err = FALSE;
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;

    سیٹ_vim_var_string(VV_FNAME_اندر, fname, -1);
    سیٹ_vim_var_string(VV_CMDARG, دلائل, -1);
    ctx = get_option_sctx("printexpr");
    اگر (ctx != تہی)
	current_sctx = *ctx;

    اگر (eval_مقصد_bool(p_pexpr, &err, تہی, FALSE, TRUE))
	err = TRUE;

    سیٹ_vim_var_string(VV_FNAME_اندر, تہی, -1);
    سیٹ_vim_var_string(VV_CMDARG, تہی, -1);
    current_sctx = saved_sctx;

    اگر (err)
    {
	mch_remove(fname);
	واپس FAIL;
    }
    واپس OK;
}
#اگر_ختم

#اگر defined(FEAT_DIFF)
    خالی
eval_diff(
    char_u	*origfile,
    char_u	*newfile,
    char_u	*outfile)
{
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;
    typval_T	*tv;

    سیٹ_vim_var_string(VV_FNAME_اندر, origfile, -1);
    سیٹ_vim_var_string(VV_FNAME_NEW, newfile, -1);
    سیٹ_vim_var_string(VV_FNAME_برآمد, outfile, -1);

    ctx = get_option_sctx("diffexpr");
    اگر (ctx != تہی)
	current_sctx = *ctx;

    // errors are ignored
    tv = eval_اظہاریہ_ext(p_dex, تہی, TRUE);
    free_tv(tv);

    سیٹ_vim_var_string(VV_FNAME_اندر, تہی, -1);
    سیٹ_vim_var_string(VV_FNAME_NEW, تہی, -1);
    سیٹ_vim_var_string(VV_FNAME_برآمد, تہی, -1);
    current_sctx = saved_sctx;
}

    خالی
eval_patch(
    char_u	*origfile,
    char_u	*difffile,
    char_u	*outfile)
{
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;
    typval_T	*tv;

    سیٹ_vim_var_string(VV_FNAME_اندر, origfile, -1);
    سیٹ_vim_var_string(VV_FNAME_DIFF, difffile, -1);
    سیٹ_vim_var_string(VV_FNAME_برآمد, outfile, -1);

    ctx = get_option_sctx("patchexpr");
    اگر (ctx != تہی)
	current_sctx = *ctx;

    // errors are ignored
    tv = eval_اظہاریہ_ext(p_pex, تہی, TRUE);
    free_tv(tv);

    سیٹ_vim_var_string(VV_FNAME_اندر, تہی, -1);
    سیٹ_vim_var_string(VV_FNAME_DIFF, تہی, -1);
    سیٹ_vim_var_string(VV_FNAME_برآمد, تہی, -1);
    current_sctx = saved_sctx;
}
#اگر_ختم

#اگر defined(FEAT_SPELL)
/*
 * Evaluate an expression to a list with suggestions.
 * For the "expr:" part of 'spellsuggest'.
 * Returns NULL when there is an error.
 */
    list_T *
eval_spell_اظہاریہ(char_u *badword, char_u *اظہاریہ)
{
    typval_T	save_val;
    typval_T	rettv;
    list_T	*list = تہی;
    char_u	*p = skipwhite(اظہاریہ);
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;
    صحیح		r;

    // Set "v:val" to the bad word.
    prepare_vimvar(VV_VAL, &save_val);
    سیٹ_vim_var_string(VV_VAL, badword, -1);
    اگر (p_verbose == 0)
	++emsg_off;
    ctx = get_option_sctx("spellsuggest");
    اگر (ctx != تہی)
	current_sctx = *ctx;

    r = may_call_simple_func(p, &rettv);
    اگر (r == NOTDONE)
	r = eval1(&p, &rettv, &EVALARG_EVALUATE);
    اگر (r == OK)
    {
	اگر (rettv.v_type != VAR_LIST)
	    clear_tv(&rettv);
	ورنہ
	    list = rettv.vval.v_list;
    }

    اگر (p_verbose == 0)
	--emsg_off;
    clear_tv(get_vim_var_tv(VV_VAL));
    restore_vimvar(VV_VAL, &save_val);
    current_sctx = saved_sctx;

    واپس list;
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

    li = list->lv_پہلا;
    اگر (li == تہی)
	واپس -1;
    *pp = tv_get_string(&li->li_tv);

    li = li->li_اگلا;
    اگر (li == تہی)
	واپس -1;
    واپس (صحیح)tv_get_نمبر(&li->li_tv);
}
#اگر_ختم

/*
 * Prepare v: variable "idx" to be used.
 * Save the current typeval in "save_tv" and clear it.
 * When not used yet add the variable to the v: hashtable.
 */
    خالی
prepare_vimvar(صحیح idx, typval_T *save_tv)
{
    *save_tv = vimvars[idx].vv_tv;
    vimvars[idx].vv_str = تہی;  // don't free it now
    اگر (vimvars[idx].vv_tv_type == VAR_UNKNOWN)
	hash_جوڑو(&vimvarht, vimvars[idx].vv_di.di_کلید, "prepare vimvar");
}

/*
 * Restore v: variable "idx" to typeval "save_tv".
 * Note that the v: variable must have been cleared already.
 * When no longer defined, remove the variable from the v: hashtable.
 */
    خالی
restore_vimvar(صحیح idx, typval_T *save_tv)
{
    hashitem_T	*hi;

    vimvars[idx].vv_tv = *save_tv;
    اگر (vimvars[idx].vv_tv_type != VAR_UNKNOWN)
	واپس;

    hi = hash_ڈھونڈو(&vimvarht, vimvars[idx].vv_di.di_کلید);
    اگر (HASHITEM_خالی_خانے(hi))
	اندرونی_error("restore_vimvar()");
    ورنہ
	hash_remove(&vimvarht, hi, "restore vimvar");
}

/*
 * List Vim variables.
 */
    ساکن خالی
list_vim_vars(صحیح *پہلا)
{
    list_hashtable_vars(&vimvarht, "v:", FALSE, پہلا);
}

/*
 * List script-local variables, if there is a script.
 */
    ساکن خالی
list_script_vars(صحیح *پہلا)
{
    اگر (SCRIPT_ID_VALID(current_sctx.sc_sid))
	list_hashtable_vars(&SCRIPT_VARS(current_sctx.sc_sid),
							   "s:", FALSE, پہلا);
}

/*
 * Return TRUE if "name" starts with "g:", "w:", "t:" or "b:".
 * But only when an identifier character follows.
 */
    صحیح
is_scoped_variable(char_u *name)
{
    واپس vim_strchr((char_u *)"gwbt", name[0]) != تہی
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
eval_one_اظہاریہ_اندر_str(char_u *p, garray_T *gap, صحیح evaluate)
{
    char_u	*بلاک_آغاز = skipwhite(p + 1);  // skip the opening {
    char_u	*بلاک_انتہا = بلاک_آغاز;
    char_u	*اظہاریہ_val;

    اگر (*بلاک_آغاز == NUL)
    {
	semsg(_(e_missing_close_curly_str), p);
	واپس تہی;
    }
    اگر (skip_اظہاریہ(&بلاک_انتہا, تہی) == FAIL)
	واپس تہی;
    بلاک_انتہا = skipwhite(بلاک_انتہا);
    اگر (*بلاک_انتہا != '}')
    {
	semsg(_(e_missing_close_curly_str), p);
	واپس تہی;
    }
    اگر (evaluate)
    {
	*بلاک_انتہا = NUL;
	اظہاریہ_val = eval_مقصد_string(بلاک_آغاز, FALSE, FALSE);
	*بلاک_انتہا = '}';
	اگر (اظہاریہ_val == تہی)
	    واپس تہی;
	ga_concat(gap, اظہاریہ_val);
	vim_free(اظہاریہ_val);
    }

    واپس بلاک_انتہا + 1;
}

/*
 * Evaluate all the Vim expressions {expr} in "str" and return the resulting
 * string in allocated memory.  "{{" is reduced to "{" and "}}" to "}".
 * Used for a heredoc assignment.
 * Returns NULL for an error.
 */
    ساکن char_u *
eval_all_اظہاریہ_اندر_str(char_u *str)
{
    garray_T	ga;
    char_u	*p;

    ga_init2(&ga, 1, 80);
    p = str;

    جب (*p != NUL)
    {
	char_u	*lit_آغاز;
	صحیح	escaped_brace = FALSE;

	// Look for a block start.
	lit_آغاز = p;
	جب (*p != '{' && *p != '}' && *p != NUL)
	    ++p;

	اگر (*p != NUL && *p == p[1])
	{
	    // Escaped brace, unescape and continue.
	    // Include the brace in the literal string.
	    ++p;
	    escaped_brace = TRUE;
	}
	ورنہ اگر (*p == '}')
	{
	    semsg(_(e_stray_closing_curly_str), str);
	    ga_clear(&ga);
	    واپس تہی;
	}

	// Append the literal part.
	ga_concat_len(&ga, lit_آغاز, (size_t)(p - lit_آغاز));

	اگر (*p == NUL)
	    توڑو;

	اگر (escaped_brace)
	{
	    // Skip the second brace.
	    ++p;
	    جاری;
	}

	// Evaluate the expression and append the result.
	p = eval_one_اظہاریہ_اندر_str(p, &ga, TRUE);
	اگر (p == تہی)
	{
	    ga_clear(&ga);
	    واپس تہی;
	}
    }
    ga_append(&ga, NUL);

    واپس ga.ga_ڈیٹا;
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
    char_u	*theline = تہی;
    char_u	*marker;
    list_T	*l;
    char_u	*p;
    char_u	*str;
    صحیح		marker_indent_len = 0;
    صحیح		متن_indent_len = 0;
    char_u	*متن_indent = تہی;
    char_u	dot[] = ".";
    صحیح		comment_char = اندر_vim9script() ? '#' : '"';
    صحیح		evalstr = FALSE;
    صحیح		eval_failed = FALSE;
    cctx_T	*cctx = vim9compile ? eap->cookie : تہی;
    صحیح		گنتی = 0;
    صحیح		heredoc_اندر_string = FALSE;
    char_u	*سطر_arg = تہی;
    char_u	*nl_ptr = vim_strchr(cmd, '\n');

    اگر (nl_ptr != تہی)
    {
	heredoc_اندر_string = TRUE;
	سطر_arg = nl_ptr + 1;
	*nl_ptr = NUL;
    }
    ورنہ اگر (eap->ea_getline == تہی)
    {
	emsg(_(e_cannot_use_heredoc_یہاں));
	واپس تہی;
    }

    // Check for the optional 'trim' word before the marker
    cmd = skipwhite(cmd);

    جب (TRUE)
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
	    جب (VIM_ISWHITE(*p))
	    {
		p++;
		marker_indent_len++;
	    }
	    متن_indent_len = -1;

	    جاری;
	}
	اگر (STRNCMP(cmd, "eval", 4) == 0
		&& (cmd[4] == NUL || VIM_ISWHITE(cmd[4])))
	{
	    cmd = skipwhite(cmd + 4);
	    evalstr = TRUE;
	    جاری;
	}
	توڑو;
    }

    // The marker is the next word.
    اگر (*cmd != NUL && *cmd != comment_char)
    {
	marker = skipwhite(cmd);
	p = skiptowhite(marker);
	اگر (*skipwhite(p) != NUL && *skipwhite(p) != comment_char)
	{
	    semsg(_(e_trailing_characters_str), p);
	    واپس تہی;
	}
	*p = NUL;
	اگر (!script_get && vim_islower(*marker))
	{
	    emsg(_(e_marker_cannot_آغاز_with_زیریں_case_letter));
	    واپس تہی;
	}
    }
    ورنہ
    {
	// When getting lines for an embedded script, if the marker is missing,
	// accept '.' as the marker.
	اگر (script_get)
	    marker = dot;
	ورنہ
	{
	    emsg(_(e_missing_marker));
	    واپس تہی;
	}
    }

    l = list_alloc();
    اگر (l == تہی)
	واپس تہی;

    برائے (;;)
    {
	صحیح	mi = 0;
	صحیح	ti = 0;

	اگر (heredoc_اندر_string)
	{
	    char_u	*اگلا_سطر;

	    // heredoc in a string separated by newlines.  Get the next line
	    // from the string.

	    اگر (*سطر_arg == NUL)
	    {
		semsg(_(e_missing_انتہا_marker_str), marker);
		توڑو;
	    }

	    theline = سطر_arg;
	    اگلا_سطر = vim_strchr(theline, '\n');
	    اگر (اگلا_سطر == تہی)
		سطر_arg += STRLEN(سطر_arg);
	    ورنہ
	    {
		*اگلا_سطر = NUL;
		سطر_arg = اگلا_سطر + 1;
	    }
	}
	ورنہ
	{
	    vim_free(theline);
	    theline = eap->ea_getline(NUL, eap->cookie, 0, FALSE);
	    اگر (theline == تہی)
	    {
		semsg(_(e_missing_انتہا_marker_str), marker);
		توڑو;
	    }
	}

	// with "trim": skip the indent matching the :let line to find the
	// marker
	اگر (marker_indent_len > 0
		&& STRNCMP(theline, *eap->cmdlinep, marker_indent_len) == 0)
	    mi = marker_indent_len;
	اگر (STRCMP(marker, theline + mi) == 0)
	    توڑو;

	// If expression evaluation failed in the heredoc, then skip till the
	// end marker.
	اگر (eval_failed)
	    جاری;

	اگر (متن_indent_len == -1 && *theline != NUL)
	{
	    // set the text indent from the first line.
	    p = theline;
	    متن_indent_len = 0;
	    جب (VIM_ISWHITE(*p))
	    {
		p++;
		متن_indent_len++;
	    }
	    متن_indent = vim_strnsave(theline, متن_indent_len);
	}
	// with "trim": skip the indent matching the first line
	اگر (متن_indent != تہی)
	    برائے (ti = 0; ti < متن_indent_len; ++ti)
		اگر (theline[ti] != متن_indent[ti])
		    توڑو;

	str = theline + ti;
	اگر (vim9compile)
	{
	    اگر (compile_all_اظہاریہ_اندر_str(str, evalstr, cctx) == FAIL)
	    {
		vim_free(theline);
		vim_free(متن_indent);
		list_free(l);
		واپس FAIL;
	    }
	    گنتی++;
	}
	ورنہ
	{
	    صحیح	    free_str = FALSE;

	    اگر (evalstr && !eap->skip)
	    {
		str = eval_all_اظہاریہ_اندر_str(str);
		اگر (str == تہی)
		{
		    // expression evaluation failed
		    eval_failed = TRUE;
		    جاری;
		}
		free_str = TRUE;
	    }

	    اگر (list_append_string(l, str, -1) == FAIL)
	    {
		اگر (free_str)
		    vim_free(str);
		توڑو;
	    }
	    اگر (free_str)
		vim_free(str);
	}
    }
    اگر (heredoc_اندر_string)
	// Next command follows the heredoc in the string.
	eap->nextcmd = سطر_arg;
    ورنہ
	vim_free(theline);
    vim_free(متن_indent);

    اگر (vim9compile && cctx->ctx_skip != SKIP_YES && !eval_failed)
	generate_NEWLIST(cctx, گنتی, FALSE);

    اگر (eval_failed)
    {
	// expression evaluation in the heredoc failed
	list_free(l);
	واپس تہی;
    }
    واپس l;
}

/*
 * Vim9 variable declaration:
 * ":var name"
 * ":var name: type"
 * ":var name = expr"
 * ":var name: type = expr"
 * etc.
 */
    خالی
ex_var(exarg_T *eap)
{
    char_u *p = eap->cmd;
    صحیح	    has_var;

    اگر (!اندر_vim9script())
    {
	semsg(_(e_str_cannot_be_مستعمل_اندر_legacy_vim_script), ":var");
	واپس;
    }
    has_var = checkforcmd_noparen(&p, "var", 3);
    اگر (current_sctx.sc_sid == 0 && has_var)
    {
	emsg(_(e_cannot_declare_variable_on_command_سطر));
	واپس;
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
    خالی
ex_let(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    char_u	*اظہاریہ = تہی;
    typval_T	rettv;
    صحیح		var_گنتی = 0;
    صحیح		semicolon = 0;
    char_u	op[4];
    char_u	*argend;
    صحیح		پہلا = TRUE;
    صحیح		concat;
    صحیح		has_تفویض;
    صحیح		flags = 0;
    صحیح		vim9script = اندر_vim9script();

    اگر (eap->cmdidx == CMD_final && !vim9script)
    {
	// In legacy Vim script ":final" is short for ":finally".
	ex_finally(eap);
	واپس;
    }
    اگر (eap->cmdidx == CMD_let && vim9script)
    {
	emsg(_(e_cannot_use_let_اندر_vim9_script));
	واپس;
    }

    اگر (eap->cmdidx == CMD_const)
	flags |= تفویض_CONST;
    ورنہ اگر (eap->cmdidx == CMD_final)
	flags |= تفویض_FINAL;

    // Vim9 assignment without ":let", ":const" or ":final"
    اگر (eap->arg == eap->cmd)
	flags |= تفویض_NO_DECL;

    argend = skip_var_list(arg, TRUE, &var_گنتی, &semicolon, FALSE);
    اگر (argend == تہی)
	واپس;
    اظہاریہ = skipwhite(argend);
    concat = اظہاریہ[0] == '.'
	&& ((اظہاریہ[1] == '=' && اندر_old_script(2))
		|| (اظہاریہ[1] == '.' && اظہاریہ[2] == '='));
    has_تفویض =  *اظہاریہ == '=' || (vim_strchr((char_u *)"+-*/%", *اظہاریہ) != تہی
							    && اظہاریہ[1] == '=');
    اگر (!has_تفویض && !concat)
    {
	// ":let" without "=": list variables
	اگر (*arg == '[')
	    emsg(_(e_invalid_دلیل));
	ورنہ اگر (اظہاریہ[0] == '.' && اظہاریہ[1] == '=')
	    emsg(_(e_dot_equal_not_supported_with_script_نسخہ_two));
	ورنہ اگر (!سرے_excmd2(eap->cmd, arg))
	{
	    اگر (vim9script)
	    {
		اگر (!سرے_excmd2(eap->cmd, skipwhite(argend)))
		    semsg(_(e_trailing_characters_str), argend);
		ورنہ
		    // Vim9 declaration ":var name: type"
		    arg = vim9_declare_scriptvar(eap, arg);
	    }
	    ورنہ
	    {
		// ":let var1 var2" - list values
		arg = list_arg_vars(eap, arg, &پہلا);
	    }
	}
	ورنہ اگر (!eap->skip)
	{
	    // ":let"
	    list_glob_vars(&پہلا);
	    list_buf_vars(&پہلا);
	    list_win_vars(&پہلا);
	    list_tab_vars(&پہلا);
	    list_script_vars(&پہلا);
	    list_func_vars(&پہلا);
	    list_vim_vars(&پہلا);
	}
	سیٹ_nextcmd(eap, arg);
	واپس;
    }

    اگر (اظہاریہ[0] == '=' && اظہاریہ[1] == '<' && اظہاریہ[2] == '<')
    {
	list_T	*l = تہی;
	طویل	cur_lnum = SOURCING_LNUM;

	// :let text =<< [trim] [eval] END
	// :var text =<< [trim] [eval] END
	اگر (vim9script && !eap->skip && (!VIM_ISWHITE(اظہاریہ[-1])
						 || !IS_WHITE_OR_NUL(اظہاریہ[3])))
	    semsg(_(e_white_space_required_before_and_after_str_at_str),
								  "=<<", اظہاریہ);
	ورنہ
	    l = heredoc_get(eap, اظہاریہ + 3, FALSE, FALSE);

	اگر (l != تہی)
	{
	    rettv_list_سیٹ(&rettv, l);
	    اگر (!eap->skip)
	    {
		// errors are for the assignment, not the end marker
		SOURCING_LNUM = cur_lnum;
		op[0] = '=';
		op[1] = NUL;
		(خالی)ex_let_vars(eap->arg, &rettv, FALSE, semicolon, var_گنتی,
								flags, op);
	    }
	    clear_tv(&rettv);
	}
	واپس;
    }

    evalarg_T   evalarg;
    صحیح	    len = 1;

    CLEAR_میدان(rettv);

    صحیح cur_lnum;

    op[0] = '=';
    op[1] = NUL;
    اگر (*اظہاریہ != '=')
    {
	اگر (vim9script && (flags & تفویض_NO_DECL) == 0)
	{
	    // +=, /=, etc. require an existing variable
	    semsg(_(e_cannot_use_operator_on_new_variable_str), eap->arg);
	}
	ورنہ اگر (vim_strchr((char_u *)"+-*/%.", *اظہاریہ) != تہی)
	{
	    op[0] = *اظہاریہ;   // +=, -=, *=, /=, %= or .=
	    ++len;
	    اگر (اظہاریہ[0] == '.' && اظہاریہ[1] == '.') // ..=
	    {
		++اظہاریہ;
		++len;
	    }
	}
	اظہاریہ += 2;
    }
    ورنہ
	++اظہاریہ;

    اگر (vim9script && !eap->skip && (!VIM_ISWHITE(*argend)
					       || !IS_WHITE_OR_NUL(*اظہاریہ)))
    {
	vim_strncpy(op, اظہاریہ - len, len);
	semsg(_(e_white_space_required_before_and_after_str_at_str),
							       op, argend);
    }

    اگر (eap->skip)
	++emsg_skip;
    fill_evalarg_from_eap(&evalarg, eap, eap->skip);
    اظہاریہ = skipwhite_and_linebreak(اظہاریہ, &evalarg);
    cur_lnum = SOURCING_LNUM;
    صحیح eval_res = eval0(اظہاریہ, &rettv, eap, &evalarg);
    اگر (eap->skip)
	--emsg_skip;
    clear_evalarg(&evalarg, eap);

    // Restore the line number so that any type error is given for the
    // declaration, not the expression.
    SOURCING_LNUM = cur_lnum;

    اگر (!eap->skip && eval_res != FAIL)
	(خالی)ex_let_vars(eap->arg, &rettv, FALSE, semicolon, var_گنتی,
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
    char_u	*arg_آغاز,
    typval_T	*tv,
    صحیح		copy,		// copy values from "tv", don't move
    صحیح		semicolon,	// from skip_var_list()
    صحیح		var_گنتی,	// from skip_var_list()
    صحیح		flags,		// ASSIGN_FINAL, ASSIGN_CONST, etc.
    char_u	*op)
{
    char_u	*arg = arg_آغاز;
    list_T	*l;
    tuple_T	*tuple = تہی;
    صحیح		i;
    صحیح		var_idx = 0;
    listitem_T	*شے = تہی;
    typval_T	ltv;
    صحیح		is_list = tv->v_type == VAR_LIST;
    صحیح		idx;

    اگر (tv->v_type == VAR_VOID)
    {
	emsg(_(e_cannot_use_void_قدر));
	واپس FAIL;
    }
    اگر (*arg != '[')
    {
	// ":let var = expr" or ":for var in list"
	اگر (ex_let_one(arg, tv, copy, flags, op, op, var_idx) == تہی)
	    واپس FAIL;
	واپس OK;
    }

    // ":let [v1, v2] = list" or ":for [v1, v2] in listlist"
    // or
    // ":let [v1, v2] = tuple" or ":for [v1, v2] in tupletuple"
    اگر (tv->v_type != VAR_LIST && tv->v_type != VAR_TUPLE)
    {
	emsg(_(e_list_or_tuple_required));
	واپس FAIL;
    }
    اگر (is_list)
    {
	l = tv->vval.v_list;
	اگر (l == تہی)
	{
	    emsg(_(e_list_required));
	    واپس FAIL;
	}
	i = list_len(l);
    }
    ورنہ
    {
	tuple = tv->vval.v_tuple;
	اگر (tuple == تہی)
	{
	    emsg(_(e_tuple_required));
	    واپس FAIL;
	}
	i = tuple_len(tuple);
    }

    اگر (semicolon == 0 && var_گنتی < i)
    {
	emsg(_(is_list ? e_less_targets_than_list_اشیاء
					: e_less_targets_than_tuple_اشیاء));
	واپس FAIL;
    }
    اگر (var_گنتی - semicolon > i)
    {
	emsg(_(is_list ? e_مزید_targets_than_list_اشیاء
					: e_مزید_targets_than_tuple_اشیاء));
	واپس FAIL;
    }

    اگر (is_list)
    {
	CHECK_LIST_MATERIALIZE(l);
	شے = l->lv_پہلا;
    }
    ورنہ
	idx = 0;

    جب (*arg != ']')
    {
	arg = skipwhite(arg + 1);
	++var_idx;
	arg = ex_let_one(arg, is_list ? &شے->li_tv : TUPLE_شے(tuple, idx),
			 TRUE, flags | تفویض_UNPACK, (char_u *)",;]", op,
			 var_idx);
	اگر (is_list)
	    شے = شے->li_اگلا;
	ورنہ
	    idx++;
	اگر (arg == تہی)
	    واپس FAIL;

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
		اگر (l == تہی)
		    واپس FAIL;

		// list
		جب (شے != تہی)
		{
		    list_append_tv(l, &شے->li_tv);
		    شے = شے->li_اگلا;
		}

		ltv.v_type = VAR_LIST;
		ltv.v_تالا = 0;
		ltv.vval.v_list = l;
		l->lv_refcount = 1;
	    }
	    ورنہ
	    {
		tuple_T	*new_tuple = tuple_alloc();
		اگر (new_tuple == تہی)
		    واپس FAIL;

		// Put the rest of the tuple (may be empty) in the var
		// after ';'.  Create a new tuple for this.
		جب (idx < TUPLE_LEN(tuple))
		{
		    typval_T    new_tv;

		    copy_tv(TUPLE_شے(tuple, idx), &new_tv);
		    اگر (tuple_append_tv(new_tuple, &new_tv) == FAIL)
		    {
			clear_tv(&new_tv);
			tuple_unref(new_tuple);
			واپس FAIL;
		    }
		    idx++;
		}

		ltv.v_type = VAR_TUPLE;
		ltv.v_تالا = 0;
		ltv.vval.v_tuple = new_tuple;
		new_tuple->tv_refcount = 1;
	    }

	    ++var_idx;
	    arg = ex_let_one(skipwhite(arg + 1), &ltv, FALSE,
			flags | تفویض_UNPACK, (char_u *)"]", op, var_idx);
	    clear_tv(&ltv);
	    اگر (arg == تہی)
		واپس FAIL;
	    توڑو;
	}
	ورنہ اگر (*arg != ',' && *arg != ']')
	{
	    اندرونی_error("ex_let_vars()");
	    واپس FAIL;
	}
    }

    واپس OK;
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
    صحیح		*var_گنتی,
    صحیح		*semicolon,
    صحیح		silent)
{
    char_u	*p, *s;

    اگر (*arg == '[')
    {
	// "[var, var]": find the matching ']'.
	p = arg;
	برائے (;;)
	{
	    p = skipwhite(p + 1);	// skip whites after '[', ';' or ','
	    s = skip_var_one(p, include_type);
	    اگر (s == p)
	    {
		اگر (!silent)
		    semsg(_(e_invalid_دلیل_str), p);
		واپس تہی;
	    }
	    ++*var_گنتی;

	    p = skipwhite(s);
	    اگر (*p == ']')
		توڑو;
	    ورنہ اگر (*p == ';')
	    {
		اگر (*semicolon == 1)
		{
		    اگر (!silent)
			emsg(_(e_double_semicolon_اندر_list_کا_variables));
		    واپس تہی;
		}
		*semicolon = 1;
	    }
	    ورنہ اگر (*p != ',')
	    {
		اگر (!silent)
		    semsg(_(e_invalid_دلیل_str), p);
		واپس تہی;
	    }
	}
	واپس p + 1;
    }

    واپس skip_var_one(arg, include_type);
}

/*
 * Skip one (assignable) variable name, including @r, $VAR, &option, d.key,
 * l[idx].
 * In Vim9 script also skip over ": type" if "include_type" is TRUE.
 */
    char_u *
skip_var_one(char_u *arg, صحیح include_type)
{
    char_u	*انتہا;
    صحیح		vim9 = اندر_vim9script();

    اگر (*arg == '@' && arg[1] != NUL)
	واپس arg + 2;

    // termcap option name may have non-alpha characters
    اگر (STRNCMP(arg, "&t_", 3) == 0 && arg[3] != NUL && arg[4] != NUL)
	واپس arg + 5;

    انتہا = ڈھونڈو_name_انتہا(*arg == '$' || *arg == '&' ? arg + 1 : arg,
				   تہی, تہی, FNE_INCL_BR | FNE_CHECK_آغاز);

    // "a: type" is declaring variable "a" with a type, not "a:".
    // Same for "s: type".
    اگر (vim9 && انتہا == arg + 2 && انتہا[-1] == ':')
	--انتہا;

    اگر (include_type && vim9)
    {
	اگر (*skipwhite(انتہا) == ':')
	    انتہا = skip_type(skipwhite(skipwhite(انتہا) + 1), FALSE);
    }
    واپس انتہا;
}

/*
 * List variables for hashtab "ht" with prefix "prefix".
 * If "empty" is TRUE also list NULL strings as empty strings.
 */
    خالی
list_hashtable_vars(
    hashtab_T	*ht,
    حرف	*سابقہ,
    صحیح		خالی_خانے_2,
    صحیح		*پہلا)
{
    hashitem_T	*hi;
    dictitem_T	*di;
    صحیح		todo;
    char_u	buf[IOSIZE];

    صحیح save_ht_flags = ht->ht_flags;
    ht->ht_flags |= HTFLAGS_FROZEN;

    todo = (صحیح)ht->ht_مستعمل;
    برائے (hi = ht->ht_array; todo > 0 && !got_int; ++hi)
    {
	اگر (!HASHITEM_خالی_خانے(hi))
	{
	    --todo;
	    di = HI2DI(hi);

	    // apply :filter /pat/ to variable name
	    vim_strncpy((char_u *)buf, (char_u *)سابقہ, IOSIZE - 1);
	    vim_strcat((char_u *)buf, di->di_کلید, IOSIZE);
	    اگر (پیغام_filtered(buf))
		جاری;

	    اگر (خالی_خانے_2 || di->di_tv.v_type != VAR_STRING
					   || di->di_tv.vval.v_string != تہی)
		list_one_var(di, سابقہ, پہلا);
	}
    }

    ht->ht_flags = save_ht_flags;
}

/*
 * List global variables.
 */
    ساکن خالی
list_glob_vars(صحیح *پہلا)
{
    list_hashtable_vars(&globvarht, "", TRUE, پہلا);
}

/*
 * List buffer variables.
 */
    ساکن خالی
list_buf_vars(صحیح *پہلا)
{
    list_hashtable_vars(&curbuf->b_vars->dv_hashtab, "b:", TRUE, پہلا);
}

/*
 * List window variables.
 */
    ساکن خالی
list_win_vars(صحیح *پہلا)
{
    list_hashtable_vars(&curwin->w_vars->dv_hashtab, "w:", TRUE, پہلا);
}

/*
 * List tab page variables.
 */
    ساکن خالی
list_tab_vars(صحیح *پہلا)
{
    list_hashtable_vars(&curtab->tp_vars->dv_hashtab, "t:", TRUE, پہلا);
}

/*
 * List variables in "arg".
 */
    ساکن char_u *
list_arg_vars(exarg_T *eap, char_u *arg, صحیح *پہلا)
{
    صحیح		error = FALSE;
    صحیح		len;
    char_u	*name;
    char_u	*name_آغاز;
    char_u	*arg_subsc;
    char_u	*tofree;
    typval_T    tv;

    جب (!سرے_excmd2(eap->cmd, arg) && !got_int)
    {
	اگر (error || eap->skip)
	{
	    arg = ڈھونڈو_name_انتہا(arg, تہی, تہی, FNE_INCL_BR | FNE_CHECK_آغاز);
	    اگر (!VIM_ISWHITE(*arg) && !سرے_excmd(*arg))
	    {
		emsg_severe = TRUE;
		اگر (!did_emsg)
		    semsg(_(e_trailing_characters_str), arg);
		توڑو;
	    }
	}
	ورنہ
	{
	    // get_name_len() takes care of expanding curly braces
	    name_آغاز = name = arg;
	    len = get_name_len(&arg, &tofree, TRUE, TRUE);
	    اگر (len <= 0)
	    {
		// This is mainly to keep test 49 working: when expanding
		// curly braces fails overrule the exception error message.
		اگر (len < 0 && !aborting())
		{
		    emsg_severe = TRUE;
		    semsg(_(e_invalid_دلیل_str), arg);
		    توڑو;
		}
		error = TRUE;
	    }
	    ورنہ
	    {
		arg = skipwhite(arg);
		اگر (tofree != تہی)
		    name = tofree;
		اگر (eval_variable(name, len, 0, &tv, تہی,
						     EVAL_VAR_VERBOSE) == FAIL)
		    error = TRUE;
		ورنہ
		{
		    // handle d.key, l[idx], f(expr)
		    arg_subsc = arg;
		    اگر (handle_subscript(&arg, name_آغاز, &tv,
					      &EVALARG_EVALUATE, TRUE) == FAIL)
			error = TRUE;
		    ورنہ
		    {
			اگر (arg == arg_subsc && len == 2 && name[1] == ':')
			{
			    انتخاب (*name)
			    {
				حالت 'g': list_glob_vars(پہلا); توڑو;
				حالت 'b': list_buf_vars(پہلا); توڑو;
				حالت 'w': list_win_vars(پہلا); توڑو;
				حالت 't': list_tab_vars(پہلا); توڑو;
				حالت 'v': list_vim_vars(پہلا); توڑو;
				حالت 's': list_script_vars(پہلا); توڑو;
				حالت 'l': list_func_vars(پہلا); توڑو;
				طےشدہ:
					  semsg(_(e_cant_list_variables_for_str), name);
			    }
			}
			ورنہ
			{
			    char_u	numbuf[NUMBUFLEN];
			    char_u	*tf;
			    صحیح		c;
			    char_u	*s;

			    s = echo_string(&tv, &tf, numbuf, 0);
			    c = *arg;
			    *arg = NUL;
			    list_one_var_a("",
				    arg == arg_subsc ? name : name_آغاز,
				    tv.v_type,
				    s == تہی ? (char_u *)"" : s,
				    پہلا);
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

    واپس arg;
}

/*
 * Set an environment variable, part of ex_let_one().
 */
    ساکن char_u *
ex_let_env(
    char_u	*arg,
    typval_T	*tv,
    صحیح		flags,
    char_u	*endchars,
    char_u	*op)
{
    char_u	*arg_انتہا = تہی;
    char_u	*name;
    صحیح		len;

    اگر ((flags & (تفویض_CONST | تفویض_FINAL))
					 && (flags & تفویض_FOR_LOOP) == 0)
    {
	emsg(_(e_cannot_تالا_environment_variable));
	واپس تہی;
    }

    // Find the end of the name.
    ++arg;
    name = arg;
    len = get_env_len(&arg);
    اگر (len == 0)
	semsg(_(e_invalid_دلیل_str), name - 1);
    ورنہ
    {
	اگر (op != تہی && vim_strchr((char_u *)"+-*/%", *op) != تہی)
	    semsg(_(e_wrong_variable_type_for_str_equal), op);
	ورنہ اگر (endchars != تہی
			      && vim_strchr(endchars, *skipwhite(arg)) == تہی)
	    emsg(_(e_unexpected_characters_اندر_let));
	ورنہ اگر (!check_secure() && !check_restricted())
	{
	    char_u	*tofree = تہی;
	    صحیح		c1 = name[len];
	    char_u	*p;

	    name[len] = NUL;
	    p = tv_get_string_chk(tv);
	    اگر (p != تہی && op != تہی && *op == '.')
	    {
		صحیح	mustfree = FALSE;
		char_u  *s = vim_getenv(name, &mustfree);

		اگر (s != تہی)
		{
		    p = tofree = concat_str(s, p);
		    اگر (mustfree)
			vim_free(s);
		}
	    }
	    اگر (p != تہی)
	    {
		vim_setenv_ext(name, p);
		arg_انتہا = arg;
	    }
	    name[len] = c1;
	    vim_free(tofree);
	}
    }
    واپس arg_انتہا;
}

/*
 * Set an option, part of ex_let_one().
 */
    ساکن char_u *
ex_let_option(
    char_u	*arg,
    typval_T	*tv,
    صحیح		flags,
    char_u	*endchars,
    char_u	*op)
{
    char_u	*p;
    صحیح		scope;
    char_u	*arg_انتہا = تہی;

    اگر ((flags & (تفویض_CONST | تفویض_FINAL))
					 && (flags & تفویض_FOR_LOOP) == 0)
    {
	emsg(_(e_cannot_تالا_option));
	واپس تہی;
    }

    // Find the end of the name.
    p = ڈھونڈو_option_انتہا(&arg, &scope);
    اگر (p == تہی || (endchars != تہی
			       && vim_strchr(endchars, *skipwhite(p)) == تہی))
    {
	emsg(_(e_unexpected_characters_اندر_let));
	واپس تہی;
    }

    صحیح		c1;
    طویل	n = 0;
    getoption_T	opt_type;
    طویل	numval;
    char_u	*stringval = تہی;
    char_u	*s = تہی;
    صحیح		failed = FALSE;
    صحیح		opt_p_flags;
    char_u	*tofree = تہی;
    char_u	numbuf[NUMBUFLEN];

    c1 = *p;
    *p = NUL;

    opt_type = get_option_قدر(arg, &numval, &stringval, &opt_p_flags, scope);
    اگر (opt_type == gov_unknown && arg[0] != 't' && arg[1] != '_')
    {
	semsg(_(e_unknown_option_str_2), arg);
	جاؤ theend;
    }
    اگر (op != تہی && *op != '='
	&& (((opt_type == gov_bool || opt_type == gov_نمبر) && *op == '.')
	    || (opt_type == gov_string && *op != '.')))
    {
	semsg(_(e_wrong_variable_type_for_str_equal), op);
	جاؤ theend;
    }

    اگر ((opt_type == gov_bool
		|| opt_type == gov_نمبر
		|| opt_type == gov_hidden_bool
		|| opt_type == gov_hidden_نمبر)
		     && (tv->v_type != VAR_STRING || !اندر_vim9script()))
    {
	اگر (opt_type == gov_bool || opt_type == gov_hidden_bool)
	    // bool, possibly hidden
	    n = (طویل)tv_get_bool_chk(tv, &failed);
	ورنہ
	    // number, possibly hidden
	    n = (طویل)tv_get_نمبر_chk(tv, &failed);
	اگر (failed)
	    جاؤ theend;
    }

    اگر ((opt_p_flags & P_FUNC) && (tv->v_type == VAR_PARTIAL
						    || tv->v_type == VAR_FUNC))
    {
	// If the option can be set to a function reference or a lambda
	// and the passed value is a function reference, then convert it to
	// the name (string) of the function reference.
	s = tv2string(tv, &tofree, numbuf, 0);
	اگر (s == تہی)
	    جاؤ theend;
    }
    // Avoid setting a string option to the text "v:false" or similar.
    // In Vim9 script also don't convert a number to string.
    ورنہ اگر (tv->v_type != VAR_BOOL && tv->v_type != VAR_SPECIAL
		     && (!اندر_vim9script() || tv->v_type != VAR_نمبر))
    {
	s = tv_get_string_chk(tv);
	اگر (s == تہی)
	    جاؤ theend;
    }
    ورنہ اگر (opt_type == gov_string || opt_type == gov_hidden_string)
    {
	emsg(_(e_string_required));
	جاؤ theend;
    }

    اگر (op != تہی && *op != '=')
    {
	// number, in legacy script also bool
	اگر (opt_type == gov_نمبر
				 || (opt_type == gov_bool && !اندر_vim9script()))
	{
	    انتخاب (*op)
	    {
		حالت '+': n = numval + n; توڑو;
		حالت '-': n = numval - n; توڑو;
		حالت '*': n = numval * n; توڑو;
		حالت '/': n = (طویل)عدد_divide(numval, n, &failed); توڑو;
		حالت '%': n = (طویل)عدد_modulus(numval, n, &failed); توڑو;
	    }
	    s = تہی;
	    اگر (failed)
		جاؤ theend;
	}
	ورنہ اگر (opt_type == gov_string && stringval != تہی && s != تہی)
	{
	    // string
	    s = concat_str(stringval, s);
	    vim_free(stringval);
	    stringval = s;
	}
    }

    حرف *err = سیٹ_option_قدر(arg, n, s, scope);
    arg_انتہا = p;
    اگر (err != تہی)
	emsg(_(err));

theend:
    *p = c1;
    vim_free(stringval);
    vim_free(tofree);
    واپس arg_انتہا;
}

/*
 * Set a register, part of ex_let_one().
 */
    ساکن char_u *
ex_let_رجسٹر(
    char_u	*arg,
    typval_T	*tv,
    صحیح		flags,
    char_u	*endchars,
    char_u	*op)
{
    char_u	*arg_انتہا = تہی;

    اگر ((flags & (تفویض_CONST | تفویض_FINAL))
					 && (flags & تفویض_FOR_LOOP) == 0)
    {
	emsg(_(e_cannot_تالا_رجسٹر));
	واپس تہی;
    }
    ++arg;
    اگر (op != تہی && vim_strchr((char_u *)"+-*/%", *op) != تہی)
	semsg(_(e_wrong_variable_type_for_str_equal), op);
    ورنہ اگر (endchars != تہی
			  && vim_strchr(endchars, *skipwhite(arg + 1)) == تہی)
	emsg(_(e_unexpected_characters_اندر_let));
    ورنہ
    {
	char_u	*ptofree = تہی;
	char_u	*p;

	p = tv_get_string_chk(tv);
	اگر (p != تہی && op != تہی && *op == '.')
	{
	    char_u  *s = get_reg_contents(*arg == '@'
						  ? '"' : *arg, GREG_اظہاریہ_SRC);

	    اگر (s != تہی)
	    {
		p = ptofree = concat_str(s, p);
		vim_free(s);
	    }
	}
	اگر (p != تہی)
	{
	    write_reg_contents(*arg == '@' ? '"' : *arg, p, -1, FALSE);
	    arg_انتہا = arg + 1;
	}
	vim_free(ptofree);
    }
    واپس arg_انتہا;
}

/*
 * Set one item of ":let var = expr" or ":let [v1, v2] = list" to its value.
 * Returns a pointer to the char just after the var name.
 * Returns NULL if there is an error.
 */
    ساکن char_u *
ex_let_one(
    char_u	*arg,		// points to variable name
    typval_T	*tv,		// value to assign to variable
    صحیح		copy,		// copy value from "tv"
    صحیح		flags,		// ASSIGN_CONST, ASSIGN_FINAL, etc.
    char_u	*endchars,	// valid chars after variable name  or NULL
    char_u	*op,		// "+", "-", "."  or NULL
    صحیح		var_idx)	// variable index for "let [a, b] = list"
{
    char_u	*arg_انتہا = تہی;

    اگر (اندر_vim9script() && (flags & (تفویض_NO_DECL | تفویض_DECL)) == 0
			&& (flags & (تفویض_CONST | تفویض_FINAL)) == 0
				  && vim_strchr((char_u *)"$@&", *arg) != تہی)
    {
	vim9_declare_error(arg);
	واپس تہی;
    }

    اگر (check_typval_is_قدر(tv) == FAIL)
	واپس تہی;

    // In Vim9 script an environment variable and a register only take a
    // String.  "@#" also takes a buffer number.
    اگر (اندر_vim9script() && (*arg == '$' || *arg == '@')
	    && !(*arg == '@' && arg[1] == '#' && tv->v_type == VAR_نمبر
					       && (op == تہی || *op == '=')))
    {
	where_T	where = WHERE_INIT;

	اگر (check_typval_type(&t_string, tv, where) == FAIL)
	    واپس تہی;
    }

    اگر (*arg == '$')
    {
	// ":let $VAR = expr": Set environment variable.
	واپس ex_let_env(arg, tv, flags, endchars, op);
    }
    ورنہ اگر (*arg == '&')
    {
	// ":let &option = expr": Set option value.
	// ":let &l:option = expr": Set local option value.
	// ":let &g:option = expr": Set global option value.
	// ":for &ts in range(8)": Set option value for for loop
	واپس ex_let_option(arg, tv, flags, endchars, op);
    }
    ورنہ اگر (*arg == '@')
    {
	// ":let @r = expr": Set register contents.
	واپس ex_let_رجسٹر(arg, tv, flags, endchars, op);
    }
    ورنہ اگر (eval_isnamec1(*arg) || *arg == '{')
    {
	lval_T	lv;
	char_u	*p;
	صحیح	lval_flags = (flags & (تفویض_NO_DECL | تفویض_DECL))
							     ? GLV_NO_DECL : 0;
	lval_flags |= (flags & تفویض_FOR_LOOP) ? GLV_FOR_LOOP : 0;
	اگر (op != تہی && *op != '=')
	    lval_flags |= GLV_تفویض_WITH_OP;

	// ":let var = expr": Set internal variable.
	// ":let var: type = expr": Set internal variable with type.
	// ":let {expr} = expr": Idem, name made with curly braces
	p = get_lval(arg, tv, &lv, FALSE, FALSE, lval_flags, FNE_CHECK_آغاز);
	اگر (p != تہی && lv.ll_name != تہی)
	{
	    اگر (endchars != تہی && vim_strchr(endchars,
					   *skipwhite(lv.ll_name_انتہا)) == تہی)
	    {
		emsg(_(e_unexpected_characters_اندر_let));
	    }
	    ورنہ
	    {
		سیٹ_var_lval(&lv, p, tv, copy, flags, op, var_idx);
		arg_انتہا = lv.ll_name_انتہا;
	    }
	}
	clear_lval(&lv);
    }
    ورنہ
	semsg(_(e_invalid_دلیل_str), arg);

    واپس arg_انتہا;
}

/*
 * ":unlet[!] var1 ... " command.
 */
    خالی
ex_unlet(exarg_T *eap)
{
    ex_unletlock(eap, eap->arg, 0, eap->forceit ? GLV_QUIET : 0, do_unlet_var, تہی);
}

/*
 * ":lockvar" and ":unlockvar" commands
 */
    خالی
ex_lockvar(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    صحیح		deep = 2;

    اگر (eap->forceit)
	deep = -1;
    ورنہ اگر (vim_isdigit(*arg))
    {
	deep = getdigits(&arg);
	arg = skipwhite(arg);
    }

    ex_unletlock(eap, arg, deep, 0, do_تالا_var, تہی);
}

/*
 * ":unlet", ":lockvar" and ":unlockvar" are quite similar.
 * Also used for Vim9 script.  "callback" is invoked as:
 *	callback(&lv, name_end, eap, deep, cookie)
 */
    خالی
ex_unletlock(
    exarg_T	*eap,
    char_u	*argstart,
    صحیح		deep,
    صحیح		glv_flags,
    صحیح		(*callback)(lval_T *, char_u *, exarg_T *, صحیح, خالی *),
    خالی	*cookie)
{
    char_u	*arg = argstart;
    char_u	*name_انتہا;
    صحیح		error = FALSE;
    lval_T	lv;

    کرو
    {
	اگر (*arg == '$')
	{
	    lv.ll_name = arg;
	    lv.ll_tv = تہی;
	    ++arg;
	    اگر (get_env_len(&arg) == 0)
	    {
		semsg(_(e_invalid_دلیل_str), arg - 1);
		واپس;
	    }
	    اگر (!error && !eap->skip
			      && callback(&lv, arg, eap, deep, cookie) == FAIL)
		error = TRUE;
	    name_انتہا = arg;
	}
	ورنہ
	{
	    // Parse the name and find the end.
	    name_انتہا = get_lval(arg, تہی, &lv, TRUE, eap->skip || error,
				     glv_flags | GLV_NO_DECL, FNE_CHECK_آغاز);
	    اگر (lv.ll_name == تہی)
		error = TRUE;	    // error but continue parsing
	    اگر (name_انتہا == تہی || (!VIM_ISWHITE(*name_انتہا)
						    && !سرے_excmd(*name_انتہا)))
	    {
		اگر (name_انتہا != تہی)
		{
		    emsg_severe = TRUE;
		    semsg(_(e_trailing_characters_str), name_انتہا);
		}
		اگر (!(eap->skip || error))
		    clear_lval(&lv);
		توڑو;
	    }

	    اگر (!error && !eap->skip
			 && callback(&lv, name_انتہا, eap, deep, cookie) == FAIL)
		error = TRUE;

	    اگر (!eap->skip)
		clear_lval(&lv);
	}

	arg = skipwhite(name_انتہا);
    } جب (!سرے_excmd2(name_انتہا, arg));

    سیٹ_nextcmd(eap, arg);
}

    ساکن صحیح
do_unlet_var(
    lval_T	*lp,
    char_u	*name_انتہا,
    exarg_T	*eap,
    صحیح		deep غیر_مستعمل,
    خالی	*cookie غیر_مستعمل)
{
    صحیح		forceit = eap->forceit;
    صحیح		ret = OK;
    صحیح		cc;

    اگر (lp->ll_tv == تہی)
    {
	cc = *name_انتہا;
	*name_انتہا = NUL;

	// Environment variable, normal name or expanded name.
	اگر (*lp->ll_name == '$')
	    vim_unsetenv_ext(lp->ll_name + 1);
	ورنہ اگر (do_unlet(lp->ll_name, forceit) == FAIL)
	    ret = FAIL;
	*name_انتہا = cc;
    }
    ورنہ اگر ((lp->ll_list != تہی
		 && قدر_check_تالا(lp->ll_list->lv_تالا, lp->ll_name, FALSE))
	    || (lp->ll_dict != تہی
		&& قدر_check_تالا(lp->ll_dict->dv_تالا, lp->ll_name, FALSE)))
	واپس FAIL;
    ورنہ اگر (lp->ll_range)
	list_unlet_range(lp->ll_list, lp->ll_li, lp->ll_n1,
						    !lp->ll_empty2, lp->ll_n2);
    ورنہ اگر (lp->ll_list != تہی)
	// unlet a List item.
	listitem_remove(lp->ll_list, lp->ll_li);
    ورنہ اگر (lp->ll_dict != تہی)
	// unlet a Dictionary item.
	dictitem_remove(lp->ll_dict, lp->ll_di, "unlet");
    ورنہ
    {
	semsg(_(e_cannot_unlet_imported_شے_str), lp->ll_name);
	واپس FAIL;
    }

    واپس ret;
}

/*
 * Unlet one item or a range of items from a list.
 * Return OK or FAIL.
 */
    خالی
list_unlet_range(
	list_T	    *l,
	listitem_T  *li_پہلا,
	طویل	    n1_arg,
	صحیح	    has_n2,
	طویل	    n2)
{
    listitem_T  *li = li_پہلا;
    صحیح		n1 = n1_arg;

    // Delete a range of List items.
    li = li_پہلا;
    n1 = n1_arg;
    جب (li != تہی && (!has_n2 || n2 >= n1))
    {
	listitem_T *اگلا = li->li_اگلا;

	listitem_remove(l, li);
	li = اگلا;
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
    char_u	*varname = تہی;  // init to shut up gcc
    dict_T	*d;
    dictitem_T	*di;

    // can't :unlet a script variable in Vim9 script
    اگر (اندر_vim9script() && check_vim9_unlet(name) == FAIL)
	واپس FAIL;

    ht = ڈھونڈو_var_ht(name, &varname);

    // can't :unlet a script variable in Vim9 script from a function
    اگر (ht == get_script_local_ht()
	    && SCRIPT_ID_VALID(current_sctx.sc_sid)
	    && SCRIPT_شے(current_sctx.sc_sid)->sn_نسخہ
							 == SCRIPT_نسخہ_VIM9
	    && check_vim9_unlet(name) == FAIL)
	واپس FAIL;

    اگر (ht != تہی && *varname != NUL)
    {
	d = get_current_funccal_dict(ht);
	اگر (d == تہی)
	{
	    اگر (ht == &globvarht)
		d = &globvardict;
	    ورنہ اگر (ht == &compat_hashtab)
		d = &vimvardict;
	    ورنہ
	    {
		di = ڈھونڈو_var_اندر_ht(ht, *name, (char_u *)"", FALSE);
		d = di == تہی ? تہی : di->di_tv.vval.v_dict;
	    }
	    اگر (d == تہی)
	    {
		اندرونی_error("do_unlet()");
		واپس FAIL;
	    }
	}
	hi = hash_ڈھونڈو(ht, varname);
	اگر (HASHITEM_خالی_خانے(hi))
	    hi = ڈھونڈو_hi_اندر_scoped_ht(name, &ht);
	اگر (hi != تہی && !HASHITEM_خالی_خانے(hi))
	{
	    di = HI2DI(hi);
	    اگر (var_check_fixed(di->di_flags, name, FALSE)
		    || var_check_ro(di->di_flags, name, FALSE)
		    || قدر_check_تالا(d->dv_تالا, name, FALSE)
		    || check_hashtab_frozen(ht, "unlet"))
		واپس FAIL;

	    delete_var(ht, hi);
	    واپس OK;
	}
    }
    اگر (forceit)
	واپس OK;
    semsg(_(e_no_such_variable_str), name);
    واپس FAIL;
}

    ساکن خالی
report_lockvar_member(حرف *پیغام, lval_T *lp)
{
    صحیح did_alloc = FALSE;
    char_u *vname = (char_u *)"";
    char_u *class_name = lp->ll_class != تہی
				    ? lp->ll_class->class_name.string : (char_u *)"";
    اگر (lp->ll_name != تہی)
    {
	اگر (lp->ll_name_انتہا == تہی)
	    vname = lp->ll_name;
	ورنہ
	{
	    vname = vim_strnsave(lp->ll_name, lp->ll_name_انتہا - lp->ll_name);
	    اگر (vname == تہی)
		واپس;
	    did_alloc = TRUE;
	}
    }
    semsg(_(پیغام), vname, class_name);
    اگر (did_alloc)
	vim_free(vname);
}

/*
 * Lock or unlock variable indicated by "lp".
 * "deep" is the levels to go (-1 for unlimited);
 * "lock" is TRUE for ":lockvar", FALSE for ":unlockvar".
 */
    ساکن صحیح
do_تالا_var(
    lval_T	*lp,
    char_u	*name_انتہا,
    exarg_T	*eap,
    صحیح		deep,
    خالی	*cookie غیر_مستعمل)
{
    صحیح		تالا = eap->cmdidx == CMD_lockvar;
    صحیح		ret = OK;
    صحیح		cc;
    dictitem_T	*di;

#اگر_تعریف_شدہ LOG_LOCKVAR
    ch_log(تہی, "LKVAR: do_lock_var(): name %s, is_root %d", lp->ll_name, lp->ll_is_جڑ);
#اگر_ختم

    اگر (lp->ll_tv == تہی)
    {
	cc = *name_انتہا;
	*name_انتہا = NUL;
	اگر (*lp->ll_name == '$')
	{
	    semsg(_(e_cannot_تالا_or_unlock_variable_str), lp->ll_name);
	    ret = FAIL;
	}
	ورنہ
	{
	    // Normal name or expanded name.
	    di = ڈھونڈو_var(lp->ll_name, تہی, TRUE);
	    اگر (di == تہی)
	    {
		اگر (اندر_vim9script())
		    semsg(_(e_cannot_ڈھونڈو_variable_مقصد_unlock_str),
								  lp->ll_name);
		ret = FAIL;
	    }
	    ورنہ
	    {
		اگر ((di->di_flags & DI_FLAGS_FIX)
			    && di->di_tv.v_type != VAR_DICT
			    && di->di_tv.v_type != VAR_LIST)
		{
		    // For historic reasons this error is not given for a list
		    // or dict.  E.g., the b: dict could be locked/unlocked.
		    semsg(_(e_cannot_تالا_or_unlock_variable_str), lp->ll_name);
		    ret = FAIL;
		}
		ورنہ
		{
		    اگر (اندر_vim9script())
		    {
			svar_T  *sv = ڈھونڈو_typval_اندر_script(&di->di_tv,
								     0, FALSE);

			اگر (sv != تہی && sv->sv_const != 0)
			{
			    semsg(_(e_cannot_change_readonly_variable_str),
								  lp->ll_name);
			    ret = FAIL;
			}
		    }

		    اگر (ret == OK)
		    {
			اگر (تالا)
			    di->di_flags |= DI_FLAGS_تالا;
			ورنہ
			    di->di_flags &= ~DI_FLAGS_تالا;
			اگر (deep != 0)
			    شے_تالا(&di->di_tv, deep, تالا, FALSE);
		    }
		}
	    }
	}
	*name_انتہا = cc;
    }
    ورنہ اگر (deep == 0 && lp->ll_object == تہی && lp->ll_class == تہی)
    {
	// nothing to do
    }
    ورنہ اگر (lp->ll_is_جڑ)
	// (un)lock the item.
	شے_تالا(lp->ll_tv, deep, تالا, FALSE);
    ورنہ اگر (lp->ll_range)
    {
	listitem_T    *li = lp->ll_li;

	// (un)lock a range of List items.
	جب (li != تہی && (lp->ll_empty2 || lp->ll_n2 >= lp->ll_n1))
	{
	    شے_تالا(&li->li_tv, deep, تالا, FALSE);
	    li = li->li_اگلا;
	    ++lp->ll_n1;
	}
    }
    ورنہ اگر (lp->ll_list != تہی)
	// (un)lock a List item.
	شے_تالا(&lp->ll_li->li_tv, deep, تالا, FALSE);
    ورنہ اگر (lp->ll_object != تہی)  // This check must be before ll_class.
    {
	// (un)lock an object variable.
	report_lockvar_member(e_cannot_تالا_object_variable_str, lp);
	ret = FAIL;
    }
    ورنہ اگر (lp->ll_class != تہی)
    {
	// (un)lock a class variable.
	report_lockvar_member(e_cannot_تالا_class_variable_str, lp);
	ret = FAIL;
    }
    ورنہ
    {
	// (un)lock a Dictionary item.
	اگر (lp->ll_di == تہی)
	{
	    emsg(_(e_dictionary_required));
	    ret = FAIL;
	}
	ورنہ
	    شے_تالا(&lp->ll_di->di_tv, deep, تالا, FALSE);
    }

    واپس ret;
}

/*
 * Lock or unlock an item.  "deep" is nr of levels to go.
 * When "check_refcount" is TRUE do not lock a list or dict with a reference
 * count larger than 1.
 */
    خالی
شے_تالا(typval_T *tv, صحیح deep, صحیح تالا, صحیح check_refcount)
{
    ساکن صحیح	recurse = 0;
    list_T	*l;
    listitem_T	*li;
    dict_T	*d;
    blob_T	*b;
    hashitem_T	*hi;
    صحیح		todo;

#اگر_تعریف_شدہ LOG_LOCKVAR
    ch_log(تہی, "LKVAR: item_lock(): type %s", vartype_name(tv->v_type));
#اگر_ختم

    اگر (recurse >= DICT_MAXNEST)
    {
	emsg(_(e_variable_nested_too_deep_for_unlock));
	واپس;
    }
    اگر (deep == 0)
	واپس;
    ++recurse;

    // lock/unlock the item itself
    اگر (تالا)
	tv->v_تالا |= VAR_LOCKED;
    ورنہ
	tv->v_تالا &= ~VAR_LOCKED;

    انتخاب (tv->v_type)
    {
	حالت VAR_UNKNOWN:
	حالت VAR_ANY:
	حالت VAR_VOID:
	حالت VAR_نمبر:
	حالت VAR_BOOL:
	حالت VAR_STRING:
	حالت VAR_FUNC:
	حالت VAR_PARTIAL:
	حالت VAR_FLOAT:
	حالت VAR_SPECIAL:
	حالت VAR_کام:
	حالت VAR_CHANNEL:
	حالت VAR_INSTR:
	حالت VAR_CLASS:
	حالت VAR_OBJECT:
	حالت VAR_TYPEALIAS:
	    توڑو;

	حالت VAR_BLOB:
	    اگر ((b = tv->vval.v_blob) != تہی
				    && !(check_refcount && b->bv_refcount > 1))
	    {
		اگر (تالا)
		    b->bv_تالا |= VAR_LOCKED;
		ورنہ
		    b->bv_تالا &= ~VAR_LOCKED;
	    }
	    توڑو;
	حالت VAR_LIST:
	    اگر ((l = tv->vval.v_list) != تہی
				    && !(check_refcount && l->lv_refcount > 1))
	    {
		اگر (تالا)
		    l->lv_تالا |= VAR_LOCKED;
		ورنہ
		    l->lv_تالا &= ~VAR_LOCKED;
		اگر (deep < 0 || deep > 1)
		{
		    اگر (l->lv_پہلا == &range_list_شے)
			l->lv_تالا |= VAR_اشیاء_LOCKED;
		    ورنہ
		    {
			// recursive: lock/unlock the items the List contains
			CHECK_LIST_MATERIALIZE(l);
			FOR_ALL_LIST_اشیاء(l, li) شے_تالا(&li->li_tv,
					       deep - 1, تالا, check_refcount);
		    }
		}
	    }
	    توڑو;
	حالت VAR_TUPLE:
	    tuple_تالا(tv->vval.v_tuple, deep, تالا, check_refcount);
	    توڑو;
	حالت VAR_DICT:
	    اگر ((d = tv->vval.v_dict) != تہی
				    && !(check_refcount && d->dv_refcount > 1))
	    {
		اگر (تالا)
		    d->dv_تالا |= VAR_LOCKED;
		ورنہ
		    d->dv_تالا &= ~VAR_LOCKED;
		اگر (deep < 0 || deep > 1)
		{
		    // recursive: lock/unlock the items the List contains
		    todo = (صحیح)d->dv_hashtab.ht_مستعمل;
		    FOR_ALL_HASHTAB_اشیاء(&d->dv_hashtab, hi, todo)
		    {
			اگر (!HASHITEM_خالی_خانے(hi))
			{
			    --todo;
			    شے_تالا(&HI2DI(hi)->di_tv, deep - 1, تالا,
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
    خالی
del_menutrans_vars(خالی)
{
    hashitem_T	*hi;
    صحیح		todo;

    hash_تالا(&globvarht);
    todo = (صحیح)globvarht.ht_مستعمل;
    برائے (hi = globvarht.ht_array; todo > 0 && !got_int; ++hi)
    {
	اگر (!HASHITEM_خالی_خانے(hi))
	{
	    --todo;
	    اگر (STRNCMP(HI2DI(hi)->di_کلید, "menutrans_", 10) == 0)
		delete_var(&globvarht, hi);
	}
    }
    hash_unlock(&globvarht);
}
#اگر_ختم

/*
 * Local string buffer for the next two functions to store a variable name
 * with its prefix. Allocated in cat_prefix_varname(), freed later in
 * get_user_var_name().
 */

ساکن char_u	*varnamebuf = تہی;
ساکن صحیح	varnamebuflen = 0;

/*
 * Function to concatenate a prefix and a variable name.
 */
    char_u *
cat_سابقہ_varname(صحیح سابقہ, char_u *name)
{
    صحیح		len;

    len = (صحیح)STRLEN(name) + 3;
    اگر (len > varnamebuflen)
    {
	vim_free(varnamebuf);
	len += 10;			// some additional space
	varnamebuf = alloc(len);
	اگر (varnamebuf == تہی)
	{
	    varnamebuflen = 0;
	    واپس تہی;
	}
	varnamebuflen = len;
    }
    *varnamebuf = سابقہ;
    varnamebuf[1] = ':';
    STRCPY(varnamebuf + 2, name);
    واپس varnamebuf;
}

/*
 * Function given to ExpandGeneric() to obtain the list of user defined
 * (global/buffer/window/built-in) variable names.
 */
    char_u *
get_user_var_name(expand_T *xp, صحیح idx)
{
    ساکن long_u	gdone;
    ساکن long_u	bdone;
    ساکن long_u	wdone;
    ساکن long_u	tdone;
    ساکن صحیح		vidx;
    ساکن hashitem_T	*hi;
    hashtab_T		*ht;

    اگر (idx == 0)
    {
	gdone = bdone = wdone = vidx = 0;
	tdone = 0;
    }

    // Global variables
    اگر (gdone < globvarht.ht_مستعمل)
    {
	اگر (gdone++ == 0)
	    hi = globvarht.ht_array;
	ورنہ
	    ++hi;
	جب (HASHITEM_خالی_خانے(hi))
	    ++hi;
	اگر (STRNCMP("g:", xp->xp_نمونہ, 2) == 0)
	    واپس cat_سابقہ_varname('g', hi->hi_کلید);
	واپس hi->hi_کلید;
    }

    // b: variables
    ht = &prevwin_curwin()->w_بفر->b_vars->dv_hashtab;
    اگر (bdone < ht->ht_مستعمل)
    {
	اگر (bdone++ == 0)
	    hi = ht->ht_array;
	ورنہ
	    ++hi;
	جب (HASHITEM_خالی_خانے(hi))
	    ++hi;
	واپس cat_سابقہ_varname('b', hi->hi_کلید);
    }

    // w: variables
    ht = &prevwin_curwin()->w_vars->dv_hashtab;
    اگر (wdone < ht->ht_مستعمل)
    {
	اگر (wdone++ == 0)
	    hi = ht->ht_array;
	ورنہ
	    ++hi;
	جب (HASHITEM_خالی_خانے(hi))
	    ++hi;
	واپس cat_سابقہ_varname('w', hi->hi_کلید);
    }

    // t: variables
    ht = &curtab->tp_vars->dv_hashtab;
    اگر (tdone < ht->ht_مستعمل)
    {
	اگر (tdone++ == 0)
	    hi = ht->ht_array;
	ورنہ
	    ++hi;
	جب (HASHITEM_خالی_خانے(hi))
	    ++hi;
	واپس cat_سابقہ_varname('t', hi->hi_کلید);
    }

    // v: variables
    اگر (vidx < VV_LEN)
	واپس cat_سابقہ_varname('v', (char_u *)vimvars[vidx++].vv_name);

    VIM_CLEAR(varnamebuf);
    varnamebuflen = 0;
    واپس تہی;
}

    حرف *
get_var_special_name(صحیح nr)
{
    انتخاب (nr)
    {
	حالت VVAL_FALSE: واپس اندر_vim9script() ? "false" : "v:false";
	حالت VVAL_TRUE:  واپس اندر_vim9script() ? "true" : "v:true";
	حالت VVAL_NULL:  واپس اندر_vim9script() ? "null" : "v:null";
	حالت VVAL_NONE:  واپس "v:none";
    }
    اندرونی_error("get_var_special_name()");
    واپس "42";
}

/*
 * Returns the global variable dictionary
 */
    dict_T *
get_globvar_dict(خالی)
{
    واپس &globvardict;
}

/*
 * Returns the global variable hash table
 */
    hashtab_T *
get_globvar_ht(خالی)
{
    واپس &globvarht;
}

/*
 * Returns the v: variable dictionary
 */
    dict_T *
get_vimvar_dict(خالی)
{
    واپس &vimvardict;
}

/*
 * Returns the index of a v:variable.  Negative if not found.
 * Returns DI_ flags in "di_flags".
 */
    صحیح
ڈھونڈو_vim_var(char_u *name, صحیح *di_flags)
{
    dictitem_T	    *di = ڈھونڈو_var_اندر_ht(&vimvarht, 0, name, TRUE);
    ساخت vimvar   *vv;

    اگر (di == تہی)
	واپس -1;
    *di_flags = di->di_flags;
    vv = (ساخت vimvar *)((حرف *)di - offsetof(vimvar_T, vv_di));
    واپس (صحیح)(vv - vimvars);
}


/*
 * Set type of v: variable to "type".
 */
    خالی
سیٹ_vim_var_type(صحیح idx, vartype_T type)
{
    vimvars[idx].vv_tv_type = type;
}

/*
 * Set number v: variable to "val".
 * Note that this does not set the type, use set_vim_var_type() for that.
 */
    خالی
سیٹ_vim_var_nr(صحیح idx, varnumber_T val)
{
    vimvars[idx].vv_nr = val;
}

    حرف *
get_vim_var_name(صحیح idx)
{
    واپس vimvars[idx].vv_name;
}

/*
 * Get typval_T v: variable value.
 */
    typval_T *
get_vim_var_tv(صحیح idx)
{
    واپس &vimvars[idx].vv_tv;
}

    type_T *
get_vim_var_type(صحیح idx, garray_T *type_list)
{
    اگر (vimvars[idx].vv_type != تہی)
	واپس vimvars[idx].vv_type;
    واپس typval2type_vimvar(&vimvars[idx].vv_tv, type_list);
}

/*
 * Add what getinfo() reports for the v: variable "name", without the "v:", to
 * "d".  Returns FAIL when there is no such variable.
 */
    صحیح
vim_var_info(char_u *name, dict_T *d)
{
    صحیح		di_flags;
    صحیح		idx = ڈھونڈو_vim_var(name, &di_flags);
    garray_T	type_list;
    حرف	*tofree;

    اگر (idx < 0)
	واپس FAIL;
    vim_snprintf((حرف *)IObuff, IOSIZE, "v:%s", vimvars[idx].vv_name);
    dict_جوڑو_string(d, "name", IObuff);
    dict_جوڑو_bool(d, "available", TRUE);
    ga_init2(&type_list, حجم(type_T *), 10);
    حرف *type = type_name(get_vim_var_type(idx, &type_list), &tofree);
    dict_جوڑو_string(d, "type", (char_u *)type);
    vim_free(tofree);
    clear_type_list(&type_list);
    dict_جوڑو_bool(d, "readonly", (vimvars[idx].vv_flags & VV_RO) != 0);
    dict_جوڑو_bool(d, "compat", (vimvars[idx].vv_flags & VV_COMPAT) != 0);
    واپس OK;
}

/*
 * Set v: variable to "tv".  Only accepts the same type.
 * Takes over the value of "tv".
 */
    صحیح
سیٹ_vim_var_tv(صحیح idx, typval_T *tv)
{
    اگر (vimvars[idx].vv_tv_type != tv->v_type)
    {
	emsg(_(e_type_mismatch_for_v_variable));
	clear_tv(tv);
	واپس FAIL;
    }
    // VV_RO is also checked when compiling, but let's check here as well.
    اگر (vimvars[idx].vv_flags & VV_RO)
    {
	semsg(_(e_cannot_change_readonly_variable_str), vimvars[idx].vv_name);
	واپس FAIL;
    }
    اگر (sandbox && (vimvars[idx].vv_flags & VV_RO_SBX))
    {
	semsg(_(e_cannot_سیٹ_variable_اندر_sandbox_str), vimvars[idx].vv_name);
	واپس FAIL;
    }
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_di.di_tv = *tv;
    واپس OK;
}

/*
 * Get number v: variable value.
 */
    varnumber_T
get_vim_var_nr(صحیح idx)
{
    واپس vimvars[idx].vv_nr;
}

/*
 * Get string v: variable value.  Uses a static buffer, can only be used once.
 * If the String variable has never been set, return an empty string.
 * Never returns NULL;
 */
    char_u *
get_vim_var_str(صحیح idx)
{
    واپس tv_get_string(&vimvars[idx].vv_tv);
}

/*
 * Get List v: variable value.  Caller must take care of reference count when
 * needed.
 */
    list_T *
get_vim_var_list(صحیح idx)
{
    واپس vimvars[idx].vv_list;
}

/*
 * Get Dict v: variable value.  Caller must take care of reference count when
 * needed.
 */
    dict_T *
get_vim_var_dict(صحیح idx)
{
    واپس vimvars[idx].vv_dict;
}

/*
 * Set v:char to character "c".
 */
    خالی
سیٹ_vim_var_char(صحیح c)
{
    char_u  buf[MB_MAXBYTES + 1];
    size_t  buflen;

    اگر (has_mbyte)
	buflen = (*mb_char2bytes)(c, buf);
    ورنہ
    {
	buf[0] = c;
	buflen = 1;
    }
    buf[buflen] = NUL;

    سیٹ_vim_var_string(VV_CHAR, buf, (صحیح)buflen);
}

/*
 * Set v:count to "count" and v:count1 to "count1".
 * When "set_prevcount" is TRUE first set v:prevcount from v:count.
 */
    خالی
سیٹ_vcount(
    طویل	گنتی,
    طویل	count1,
    صحیح		سیٹ_prevcount)
{
    اگر (سیٹ_prevcount)
	vimvars[VV_PREVCOUNT].vv_nr = vimvars[VV_گنتی].vv_nr;
    vimvars[VV_گنتی].vv_nr = گنتی;
    vimvars[VV_COUNT1].vv_nr = count1;
}

/*
 * Save variables that might be changed as a side effect.  Used when executing
 * a timer callback.
 */
    خالی
save_vimvars(vimvars_save_T *vvsave)
{
    vvsave->vv_prevcount = vimvars[VV_PREVCOUNT].vv_nr;
    vvsave->vv_گنتی = vimvars[VV_گنتی].vv_nr;
    vvsave->vv_count1 = vimvars[VV_COUNT1].vv_nr;
}

/*
 * Restore variables saved by save_vimvars().
 */
    خالی
restore_vimvars(vimvars_save_T *vvsave)
{
    vimvars[VV_PREVCOUNT].vv_nr = vvsave->vv_prevcount;
    vimvars[VV_گنتی].vv_nr = vvsave->vv_گنتی;
    vimvars[VV_COUNT1].vv_nr = vvsave->vv_count1;
}

/*
 * Set string v: variable to a copy of "val". If 'copy' is FALSE, then set the
 * value.
 */
    خالی
سیٹ_vim_var_string(
    صحیح		idx,
    char_u	*val,
    صحیح		len)	    // length of "val" to use or -1 (whole string)
{
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_tv_type = VAR_STRING;
    اگر (val == تہی)
	vimvars[idx].vv_str = تہی;
    ورنہ اگر (len == -1)
	vimvars[idx].vv_str = vim_strsave(val);
    ورنہ
	vimvars[idx].vv_str = vim_strnsave(val, len);
}

    خالی
سیٹ_vim_var_string_براہ_راست(
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
    خالی
سیٹ_vim_var_list(صحیح idx, list_T *val)
{
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_tv_type = VAR_LIST;
    vimvars[idx].vv_list = val;
    اگر (val != تہی)
	++val->lv_refcount;
}

/*
 * Set Dictionary v: variable to "val".
 */
    خالی
سیٹ_vim_var_dict(صحیح idx, dict_T *val)
{
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_tv_type = VAR_DICT;
    vimvars[idx].vv_dict = val;
    اگر (val == تہی)
	واپس;

    ++val->dv_refcount;
    dict_سیٹ_اشیاء_ro(val);
}

/*
 * Set the v:argv list.
 */
    خالی
سیٹ_argv_var(حرف **argv, صحیح argc)
{
    list_T	*l = list_alloc();
    صحیح		i;

    اگر (l == تہی)
	getout(1);
    l->lv_تالا = VAR_FIXED;
    برائے (i = 0; i < argc; ++i)
    {
	اگر (list_append_string(l, (char_u *)argv[i], -1) == FAIL)
	    getout(1);
	l->lv_u.mat.lv_آخری->li_tv.v_تالا = VAR_FIXED;
    }
    سیٹ_vim_var_list(VV_ARGV, l);
}

/*
 * Reset v:register, taking the 'clipboard' setting into account.
 */
    خالی
reset_reg_var(خالی)
{
    صحیح regname = 0;

    // Adjust the register according to 'clipboard', so that when
    // "unnamed" is present it becomes '*' or '+' instead of '"'.
#اگر_تعریف_شدہ HAVE_CLIPMETHOD
    adjust_clip_reg(&regname);
#اگر_ختم
    سیٹ_reg_var(regname);
}

/*
 * Set v:register if needed.
 */
    خالی
سیٹ_reg_var(صحیح c)
{
    char_u  regname[2];

    اگر (c == 0 || c == ' ')
	regname[0] = '"';
    ورنہ
	regname[0] = c;
    regname[1] = NUL;
    // Avoid free/alloc when the value is already right.
    اگر (vimvars[VV_REG].vv_str == تہی || vimvars[VV_REG].vv_str[0] != c)
	سیٹ_vim_var_string(VV_REG, regname, 1);
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
    اگر (oldval == تہی)
	واپس vimvars[VV_EXCEPTION].vv_str;

    vimvars[VV_EXCEPTION].vv_str = oldval;
    واپس تہی;
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
    اگر (oldval == تہی)
	واپس vimvars[VV_THROWPOINT].vv_str;

    vimvars[VV_THROWPOINT].vv_str = oldval;
    واپس تہی;
}

/*
 * Set v:cmdarg.
 * If "eap" != NULL, use "eap" to generate the value and return the old value.
 * If "oldarg" != NULL, restore the value to "oldarg" and return NULL.
 * Must always be called in pairs!
 */
    char_u *
سیٹ_cmdarg(exarg_T *eap, char_u *oldarg)
{
    char_u	*oldval;
    char_u	*newval;
    غیرعلامتی	len;

    oldval = vimvars[VV_CMDARG].vv_str;
    اگر (eap == تہی)
    {
	vim_free(oldval);
	vimvars[VV_CMDARG].vv_str = oldarg;
	واپس تہی;
    }

    اگر (eap->force_bin == FORCE_BIN)
	len = 6;
    ورنہ اگر (eap->force_bin == FORCE_NOBIN)
	len = 8;
    ورنہ
	len = 0;

    اگر (eap->read_edit)
	len += 7;

    اگر (eap->force_ff != 0)
	len += 10; // " ++ff=unix"
    اگر (eap->force_enc != 0)
	len += (غیرعلامتی)STRLEN(eap->cmd + eap->force_enc) + 7;
    اگر (eap->bad_char != 0)
	len += 7 + 4;  // " ++bad=" + "keep" or "drop"

    newval = alloc(len + 1);
    اگر (newval == تہی)
	واپس تہی;

    اگر (eap->force_bin == FORCE_BIN)
	sprintf((حرف *)newval, " ++bin");
    ورنہ اگر (eap->force_bin == FORCE_NOBIN)
	sprintf((حرف *)newval, " ++nobin");
    ورنہ
	*newval = NUL;

    اگر (eap->read_edit)
	STRCAT(newval, " ++edit");

    اگر (eap->force_ff != 0)
	sprintf((حرف *)newval + STRLEN(newval), " ++ff=%s",
						eap->force_ff == 'u' ? "unix"
						: eap->force_ff == 'd' ? "dos"
						: "mac");
    اگر (eap->force_enc != 0)
	sprintf((حرف *)newval + STRLEN(newval), " ++enc=%s",
					       eap->cmd + eap->force_enc);
    اگر (eap->bad_char == BAD_KEEP)
	STRCPY(newval + STRLEN(newval), " ++bad=keep");
    ورنہ اگر (eap->bad_char == BAD_DROP)
	STRCPY(newval + STRLEN(newval), " ++bad=drop");
    ورنہ اگر (eap->bad_char != 0)
	sprintf((حرف *)newval + STRLEN(newval), " ++bad=%c", eap->bad_char);
    vimvars[VV_CMDARG].vv_str = newval;
    واپس oldval;
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
    typval_T	*tv = تہی;
    صحیح		ملا = FALSE;
    hashtab_T	*ht = تہی;
    صحیح		cc = 0;
    type_T	*type = تہی;

    اگر (len > 0)
    {
	// truncate the name, so that we can use strcmp()
	cc = name[len];
	name[len] = NUL;
    }

    // Check for local variable when debugging.
    اگر ((sid == 0) && (tv = lookup_debug_var(name)) == تہی)
    {
	// Check for user-defined variables.
	dictitem_T	*v = ڈھونڈو_var(name, &ht, flags & EVAL_VAR_NOAUTOLOAD);

	اگر (v != تہی)
	{
	    tv = &v->di_tv;
	    اگر (dip != تہی)
		*dip = v;
	}
	ورنہ
	    ht = تہی;
    }

    اگر (tv == تہی && (اندر_vim9script() || STRNCMP(name, "s:", 2) == 0))
    {
	imported_T  *import = تہی;
	char_u	    *p = STRNCMP(name, "s:", 2) == 0 ? name + 2 : name;

	اگر (sid == 0)
	    import = ڈھونڈو_imported(p, 0, TRUE);

	// imported variable from another script
	اگر (import != تہی || sid != 0)
	{
	    اگر ((flags & EVAL_VAR_IMPORT) == 0)
	    {
		اگر (SCRIPT_ID_VALID(sid))
		{
		    ht = &SCRIPT_VARS(sid);
		    اگر (ht != تہی)
		    {
			dictitem_T *v = ڈھونڈو_var_اندر_ht(ht, 0, name,
						  flags & EVAL_VAR_NOAUTOLOAD);

			اگر (v == تہی)
			    v = ڈھونڈو_var_autoload_سابقہ(name, sid, تہی, تہی);

			اگر (v != تہی)
			{
			    tv = &v->di_tv;
			    اگر (dip != تہی)
				*dip = v;
			}
			ورنہ
			    ht = تہی;
		    }
		}
		ورنہ
		{
		    اگر (flags & EVAL_VAR_VERBOSE)
			semsg(_(e_expected_dot_after_name_str), name);
		    ret = FAIL;
		}
	    }
	    ورنہ
	    {
		اگر (rettv != تہی)
		{
		    // special value that is used in handle_subscript()
		    rettv->v_type = VAR_ANY;
		    rettv->vval.v_نمبر = sid != 0 ? sid : import->imp_sid;
		}
		ملا = TRUE;
	    }
	}
	ورنہ اگر (اندر_vim9script() && (flags & EVAL_VAR_NO_FUNC) == 0)
	{
	    صحیح	    has_g_سابقہ = STRNCMP(name, "g:", 2) == 0;
	    ufunc_T *ufunc = ڈھونڈو_func(name, FALSE);

	    اگر (ufunc != تہی && cc == '<')
	    {
		// handle generic function
		char_u	*argp = name + len;
		name[len] = cc;
		ufunc = eval_generic_func(ufunc, name, &argp);
		name[len] = NUL;
		اگر (ufunc == تہی)
		{
		    ret = FAIL;
		    جاؤ مکمل;
		}
	    }

	    // In Vim9 script we can get a function reference by using the
	    // function name.  For a global non-autoload function "g:" is
	    // required.
	    اگر (ufunc != تہی && (has_g_سابقہ
					    || !func_requires_g_سابقہ(ufunc)))
	    {
		ملا = TRUE;
		اگر (rettv != تہی)
		{
		    rettv->v_type = VAR_FUNC;
		    اگر (has_g_سابقہ)
		    {
			// Keep the "g:", otherwise script-local may be
			// assumed.
			اگر (cc != '<')
			    rettv->vval.v_string = vim_strsave(name);
			ورنہ
			{
			    // append the generic function arguments
			    char_u	*argp = name + len;
			    name[len] = cc;
			    rettv->vval.v_string =
				append_generic_func_type_دلائل(name, len,
									&argp);
			    name[len] = NUL;
			}
		    }
		    ورنہ
			rettv->vval.v_string = vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
		    اگر (rettv->vval.v_string != تہی)
			func_ref(ufunc->uf_name);
		}
	    }
	}
    }

    اگر (!ملا)
    {
	اگر (tv == تہی)
	{
	    اگر (rettv != تہی && (flags & EVAL_VAR_VERBOSE))
		semsg(_(e_undefined_variable_str), name);
	    ret = FAIL;
	}
	ورنہ اگر (rettv != تہی)
	{
	    svar_T  *sv = تہی;
	    صحیح	    was_assigned = FALSE;

	    اگر (ht != تہی && ht == get_script_local_ht()
		    && tv != &SCRIPT_SV(current_sctx.sc_sid)->sv_var.di_tv)
	    {
		sv = ڈھونڈو_typval_اندر_script(tv, 0, TRUE);
		اگر (sv != تہی)
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
		sv = ڈھونڈو_typval_اندر_script(tv, sid, TRUE);
		اگر (sv == تہی)
		{
		    ret = FAIL;
		    جاؤ مکمل;
		}
		اگر ((sv->sv_flags & SVFLAG_EXPORTED) == 0)
		{
		    semsg(_(e_شے_not_exported_اندر_script_str), name);
		    ret = FAIL;
		    جاؤ مکمل;
		}
	    }

	    // If a list or tuple or dict variable wasn't initialized and has
	    // meaningful type, do it now.  Not for global variables, they are
	    // not declared.
	    اگر (ht != &globvarht)
	    {
		اگر (tv->v_type == VAR_DICT && tv->vval.v_dict == تہی
					    && ((type != تہی && !was_assigned)
							  || !اندر_vim9script()))
		{
		    tv->vval.v_dict = dict_alloc();
		    اگر (tv->vval.v_dict != تہی)
		    {
			++tv->vval.v_dict->dv_refcount;
			tv->vval.v_dict->dv_type = alloc_type(type);
			اگر (sv != تہی)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
		ورنہ اگر (tv->v_type == VAR_LIST && tv->vval.v_list == تہی
					    && ((type != تہی && !was_assigned)
							  || !اندر_vim9script()))
		{
		    tv->vval.v_list = list_alloc();
		    اگر (tv->vval.v_list != تہی)
		    {
			++tv->vval.v_list->lv_refcount;
			tv->vval.v_list->lv_type = alloc_type(type);
			اگر (sv != تہی)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
		ورنہ اگر (tv->v_type == VAR_TUPLE && tv->vval.v_tuple == تہی
					    && ((type != تہی && !was_assigned)
							  || !اندر_vim9script()))
		{
		    tv->vval.v_tuple = tuple_alloc();
		    اگر (tv->vval.v_tuple != تہی)
		    {
			++tv->vval.v_tuple->tv_refcount;
			tv->vval.v_tuple->tv_type = alloc_type(type);
			اگر (sv != تہی)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
		ورنہ اگر (tv->v_type == VAR_BLOB && tv->vval.v_blob == تہی
					    && ((type != تہی && !was_assigned)
							  || !اندر_vim9script()))
		{
		    tv->vval.v_blob = blob_alloc();
		    اگر (tv->vval.v_blob != تہی)
		    {
			++tv->vval.v_blob->bv_refcount;
			اگر (sv != تہی)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
	    }
	    ret = copy_tv(tv, rettv);
	}
    }

مکمل:
    اگر (len > 0)
	name[len] = cc;

    واپس ret;
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
    جب (ASCII_ISALNUM(*s) || *s == '_')
	++s;
    صحیح	    len = (صحیح)(s - name);

    اگر (eval_variable(name, len, 0, rettv, تہی, EVAL_VAR_IMPORT) == FAIL)
	واپس FAIL;
    اگر (rettv->v_type == VAR_ANY && *s == '.')
    {
	char_u *ns = s + 1;
	s = ns;
	جب (ASCII_ISALNUM(*s) || *s == '_')
	    ++s;
	صحیح sid = rettv->vval.v_نمبر;
	واپس eval_variable(ns, (صحیح)(s - ns), sid, rettv, تہی, 0);
    }
    واپس OK;
}


/*
 * Check if variable "name[len]" is a local variable or an argument.
 * If so, "*eval_lavars_used" is set to TRUE.
 */
    خالی
check_vars(char_u *name, صحیح len)
{
    صحیح		cc;
    char_u	*varname;
    hashtab_T	*ht;

    اگر (eval_lavars_مستعمل == تہی)
	واپس;

    // truncate the name, so that we can use strcmp()
    cc = name[len];
    name[len] = NUL;

    ht = ڈھونڈو_var_ht(name, &varname);
    اگر (ht == get_funccal_local_ht() || ht == get_funccal_دلائل_ht())
    {
	اگر (ڈھونڈو_var(name, تہی, TRUE) != تہی)
	    *eval_lavars_مستعمل = TRUE;
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
ڈھونڈو_var(char_u *name, hashtab_T **htp, صحیح no_autoload)
{
    char_u	*varname;
    hashtab_T	*ht;
    dictitem_T	*ret = تہی;

    ht = ڈھونڈو_var_ht(name, &varname);
    اگر (htp != تہی)
	*htp = ht;
    اگر (ht == تہی)
	واپس تہی;
    ret = ڈھونڈو_var_اندر_ht(ht, *name, varname, no_autoload);
    اگر (ret != تہی)
	واپس ret;

    // Search in parent scope for lambda
    ret = ڈھونڈو_var_اندر_scoped_ht(name, no_autoload);
    اگر (ret != تہی)
	واپس ret;

    // in Vim9 script items without a scope can be script-local
    اگر (اندر_vim9script() && name[0] != NUL && name[1] != ':')
    {
	ht = get_script_local_ht();
	اگر (ht != تہی)
	{
	    ret = ڈھونڈو_var_اندر_ht(ht, *name, varname, no_autoload);
	    اگر (ret != تہی)
	    {
		اگر (htp != تہی)
		    *htp = ht;
		واپس ret;
	    }
	}
    }

    // and finally try
    واپس ڈھونڈو_var_autoload_سابقہ(name, 0, htp, تہی);
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
ڈھونڈو_var_autoload_سابقہ(char_u *name, صحیح sid, hashtab_T **htp,
							    char_u **namep)
{
    hashtab_T	*ht;
    dictitem_T	*ret = تہی;
    // When using "vim9script autoload" script-local items are prefixed but can
    // be used with s:name.
    صحیح check_sid = sid > 0 ? sid : current_sctx.sc_sid;
    اگر (SCRIPT_ID_VALID(check_sid)
		   && (اندر_vim9script() || (name[0] == 's' && name[1] == ':')))
    {
	scriptitem_T *si = SCRIPT_شے(check_sid);

	اگر (si->sn_autoload_سابقہ != تہی)
	{
	    char_u *بنیاد_name = (name[0] == 's' && name[1] == ':')
							     ? name + 2 : name;
	    char_u *auto_name = concat_str(si->sn_autoload_سابقہ, بنیاد_name);

	    اگر (auto_name != تہی)
	    {
		صحیح free_auto_name = TRUE;
		ht = &globvarht;
		ret = ڈھونڈو_var_اندر_ht(ht, 'g', auto_name, TRUE);
		اگر (ret != تہی)
		{
		    اگر (htp != تہی)
			*htp = ht;
		    اگر (namep != تہی)
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

    واپس ret;
}

/*
 * Like find_var() but if the name starts with <SNR>99_ then look in the
 * referenced script (used for a funcref).
 */
    dictitem_T *
ڈھونڈو_var_also_اندر_script(char_u *name, hashtab_T **htp, صحیح no_autoload)
{
    اگر (STRNCMP(name, "<SNR>", 5) == 0 && محفوظ_isdigit(name[5]))
    {
	char_u	    *p = name + 5;
	صحیح	    sid = getdigits(&p);

	اگر (SCRIPT_ID_VALID(sid) && *p == '_')
	{
	    hashtab_T	*ht = &SCRIPT_VARS(sid);

	    اگر (ht != تہی)
	    {
		dictitem_T *di = ڈھونڈو_var_اندر_ht(ht, 0, p + 1, no_autoload);

		اگر (di != تہی)
		{
		    اگر (htp != تہی)
			*htp = ht;
		    واپس di;
		}
	    }
	}
    }

    واپس ڈھونڈو_var(name, htp, no_autoload);
}

/*
 * Find variable "varname" in hashtab "ht" with name "htname".
 * When "varname" is empty returns curwin/curtab/etc vars dictionary.
 * Returns NULL if not found.
 */
    dictitem_T *
ڈھونڈو_var_اندر_ht(
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
	    حالت 's': واپس &SCRIPT_SV(current_sctx.sc_sid)->sv_var;
	    حالت 'g': واپس &globvars_var;
	    حالت 'v': واپس &vimvars_var;
	    حالت 'b': واپس &curbuf->b_bufvar;
	    حالت 'w': واپس &curwin->w_winvar;
	    حالت 't': واپس &curtab->tp_winvar;
	    حالت 'l': واپس get_funccal_local_var();
	    حالت 'a': واپس get_funccal_دلائل_var();
	}
	واپس تہی;
    }

    hi = hash_ڈھونڈو(ht, varname);
    اگر (HASHITEM_خالی_خانے(hi))
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
		واپس تہی;
	    hi = hash_ڈھونڈو(ht, varname);
	}
	اگر (HASHITEM_خالی_خانے(hi))
	    واپس تہی;
    }
    واپس HI2DI(hi);
}

/*
 * Get the script-local hashtab.  NULL if not in a script context.
 */
    hashtab_T *
get_script_local_ht(خالی)
{
    scid_T sid = current_sctx.sc_sid;

    اگر (SCRIPT_ID_VALID(sid))
	واپس &SCRIPT_VARS(sid);
    واپس تہی;
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
	cctx_T	*dummy غیر_مستعمل)
{
    hashtab_T	*ht = get_script_local_ht();
    char_u	بفر[30];
    char_u	*p;
    صحیح		res;
    hashitem_T	*hi;
    صحیح		is_global = FALSE;
    char_u	*fname = name;

    اگر (ht == تہی)
	واپس FAIL;
    اگر (len < حجم(بفر) - 1)
    {
	// avoid an alloc/free for short names
	vim_strncpy(بفر, name, len);
	p = بفر;
    }
    ورنہ
    {
	p = vim_strnsave(name, len);
	اگر (p == تہی)
	    واپس FAIL;
    }

    hi = hash_ڈھونڈو(ht, p);
    res = HASHITEM_خالی_خانے(hi) ? FAIL : OK;

    // if not script-local, then perhaps autoload-exported
    اگر (res == FAIL && ڈھونڈو_var_autoload_سابقہ(p, 0, تہی, تہی) != تہی)
	res = OK;

    // if not script-local or autoload, then perhaps imported
    اگر (res == FAIL && ڈھونڈو_imported(p, 0, FALSE) != تہی)
	res = OK;
    اگر (p != بفر)
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
	    اگر (ڈھونڈو_func(fname, is_global) != تہی)
		res = OK;
	}
    }

    واپس res;
}

/*
 * Find the hashtab used for a variable name.
 * Return NULL if the name is not valid.
 * Set "varname" to the start of name without ':'.
 */
    hashtab_T *
ڈھونڈو_var_ht(char_u *name, char_u **varname)
{
    hashitem_T	*hi;
    hashtab_T	*ht;

    اگر (name[0] == NUL)
	واپس تہی;
    اگر (name[1] != ':')
    {
	// The name must not start with a colon or #.
	اگر (name[0] == ':' || name[0] == AUTOLOAD_CHAR)
	    واپس تہی;
	*varname = name;

	// "version" is "v:version" in all scopes if scriptversion < 3.
	// Same for a few other variables marked with VV_COMPAT.
	اگر (اندر_old_script(3))
	{
	    hi = hash_ڈھونڈو(&compat_hashtab, name);
	    اگر (!HASHITEM_خالی_خانے(hi))
		واپس &compat_hashtab;
	}

	ht = get_funccal_local_ht();
	اگر (ht != تہی)
	    واپس ht;				// local variable

	// In Vim9 script items at the script level are script-local, except
	// for autoload names.
	اگر (اندر_vim9script() && vim_strchr(name, AUTOLOAD_CHAR) == تہی)
	{
	    ht = get_script_local_ht();
	    اگر (ht != تہی)
		واپس ht;
	}

	واپس &globvarht;			// global variable
    }
    *varname = name + 2;
    اگر (*name == 'g')				// global variable
	واپس &globvarht;
    // There must be no ':' or '#' in the rest of the name, unless g: is used
    اگر (vim_strchr(name + 2, ':') != تہی
			       || vim_strchr(name + 2, AUTOLOAD_CHAR) != تہی)
	واپس تہی;
    اگر (*name == 'b')				// buffer variable
	واپس &curbuf->b_vars->dv_hashtab;
    اگر (*name == 'w')				// window variable
	واپس &curwin->w_vars->dv_hashtab;
    اگر (*name == 't')				// tab page variable
	واپس &curtab->tp_vars->dv_hashtab;
    اگر (*name == 'v')				// v: variable
	واپس &vimvarht;
    اگر (get_current_funccal() != تہی
	       && get_current_funccal()->fc_func->uf_def_حالت
							    == UF_NOT_COMPILED)
    {
	// a: and l: are only used in functions defined with ":function"
	اگر (*name == 'a')			// a: function argument
	    واپس get_funccal_دلائل_ht();
	اگر (*name == 'l')			// l: local function variable
	    واپس get_funccal_local_ht();
    }
    اگر (*name == 's')				// script variable
    {
	ht = get_script_local_ht();
	اگر (ht != تہی)
	    واپس ht;
    }
    واپس تہی;
}

/*
 * Get the string value of a (global/local) variable.
 * Note: see tv_get_string() for how long the pointer remains valid.
 * Returns NULL when it doesn't exist.
 */
    char_u *
get_var_قدر(char_u *name)
{
    dictitem_T	*v;

    v = ڈھونڈو_var(name, تہی, FALSE);
    اگر (v == تہی)
	واپس تہی;
    واپس tv_get_string(&v->di_tv);
}

/*
 * Allocate a new hashtab for a sourced script.  It will be used while
 * sourcing this script and when executing functions defined in the script.
 */
    خالی
new_script_vars(scid_T id)
{
    scriptvar_T *sv;

    sv = ALLOC_CLEAR_ONE(scriptvar_T);
    اگر (sv == تہی)
	واپس;
    init_var_dict(&sv->sv_dict, &sv->sv_var, VAR_SCOPE);
    SCRIPT_شے(id)->sn_vars = sv;
}

/*
 * Initialize dictionary "dict" as a scope and set variable "dict_var" to
 * point to it.
 */
    خالی
init_var_dict(dict_T *dict, dictitem_T *dict_var, صحیح scope)
{
    hash_init(&dict->dv_hashtab);
    dict->dv_تالا = 0;
    dict->dv_scope = scope;
    dict->dv_refcount = DO_NOT_FREE_CNT;
    dict->dv_copyID = 0;
    dict_var->di_tv.vval.v_dict = dict;
    dict_var->di_tv.v_type = VAR_DICT;
    dict_var->di_tv.v_تالا = VAR_FIXED;
    dict_var->di_flags = DI_FLAGS_RO | DI_FLAGS_FIX;
    dict_var->di_کلید[0] = NUL;
}

/*
 * Unreference a dictionary initialized by init_var_dict().
 */
    خالی
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
    خالی
vars_clear(hashtab_T *ht)
{
    vars_clear_ext(ht, TRUE);
}

/*
 * Like vars_clear(), but only free the value if "free_val" is TRUE.
 */
    خالی
vars_clear_ext(hashtab_T *ht, صحیح free_val)
{
    صحیح		todo;
    hashitem_T	*hi;
    dictitem_T	*v;

    hash_تالا(ht);
    todo = (صحیح)ht->ht_مستعمل;
    FOR_ALL_HASHTAB_اشیاء(ht, hi, todo)
    {
	اگر (!HASHITEM_خالی_خانے(hi))
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
    خالی
delete_var(hashtab_T *ht, hashitem_T *hi)
{
    dictitem_T	*di = HI2DI(hi);

    اگر (hash_remove(ht, hi, "delete variable") != OK)
	واپس;

    clear_tv(&di->di_tv);
    vim_free(di);
}

/*
 * Delete the exported variables of a reloaded Vim9 autoload script.  They live
 * in the global namespace with the autoload prefix (e.g. "foo#bar") and are
 * recreated when the script body runs again.
 */
    خالی
delete_autoload_export_vars(char_u *سابقہ)
{
    اگر (سابقہ == تہی)
	واپس;
    size_t  prefixlen = STRLEN(سابقہ);

    hash_تالا(&globvarht);
    صحیح	todo = (صحیح)globvarht.ht_مستعمل;

    برائے (hashitem_T *hi = globvarht.ht_array; todo > 0; ++hi)
	اگر (!HASHITEM_خالی_خانے(hi))
	{
	    dictitem_T	*di = HI2DI(hi);

	    --todo;
	    // Keep a class or enum: existing objects still refer to it.
	    اگر (di->di_tv.v_type != VAR_CLASS
		    && STRNCMP(di->di_کلید, سابقہ, prefixlen) == 0)
		delete_var(&globvarht, hi);
	}
    hash_unlock(&globvarht);
}

/*
 * List the value of one internal variable.
 */
    ساکن خالی
list_one_var(dictitem_T *v, حرف *سابقہ, صحیح *پہلا)
{
    char_u	*tofree;
    char_u	*s;
    char_u	numbuf[NUMBUFLEN];

    s = echo_string(&v->di_tv, &tofree, numbuf, get_copyID());
    list_one_var_a(سابقہ, v->di_کلید, v->di_tv.v_type,
					 s == تہی ? (char_u *)"" : s, پہلا);
    vim_free(tofree);
}

    ساکن خالی
list_one_var_a(
    حرف	*سابقہ,
    char_u	*name,
    صحیح		type,
    char_u	*string,
    صحیح		*پہلا)  // when TRUE clear rest of screen and set to FALSE
{
    // don't use msg() or msg_attr() to avoid overwriting "v:statusmsg"
    پیغام_آغاز();
    پیغام_puts(سابقہ);
    اگر (name != تہی)	// "a:" vars don't have a name stored
	پیغام_puts((حرف *)name);
    پیغام_putchar(' ');
    پیغام_advance(22);
    اگر (type == VAR_نمبر)
	پیغام_putchar('#');
    ورنہ اگر (type == VAR_FUNC || type == VAR_PARTIAL)
	پیغام_putchar('*');
    ورنہ اگر (type == VAR_LIST)
    {
	پیغام_putchar('[');
	اگر (*string == '[')
	    ++string;
    }
    ورنہ اگر (type == VAR_DICT)
    {
	پیغام_putchar('{');
	اگر (*string == '{')
	    ++string;
    }
    ورنہ
	پیغام_putchar(' ');

    پیغام_outtrans(string);

    اگر (type == VAR_FUNC || type == VAR_PARTIAL)
	پیغام_puts("()");
    اگر (*پہلا)
    {
	پیغام_clr_eos();
	*پہلا = FALSE;
    }
}

/*
 * Addition handling for setting a v: variable.
 * Return TRUE if the variable should be set normally,
 *        FALSE if nothing else needs to be done.
 */
    صحیح
before_سیٹ_vvar(
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
	    اگر (di->di_tv.vval.v_string == تہی)
		di->di_tv.vval.v_string = vim_strsave(val);
	}
	ورنہ
	{
	    // Take over the string to avoid an extra alloc/free.
	    di->di_tv.vval.v_string = tv->vval.v_string;
	    tv->vval.v_string = تہی;
	}
	واپس FALSE;
    }
    ورنہ اگر (di->di_tv.v_type == VAR_نمبر)
    {
	di->di_tv.vval.v_نمبر = tv_get_نمبر(tv);
	اگر (STRCMP(varname, "searchforward") == 0)
	    سیٹ_تلاش_direction(di->di_tv.vval.v_نمبر ? '/' : '?');
#اگر_تعریف_شدہ FEAT_تلاش_EXTRA
	ورنہ اگر (STRCMP(varname, "hlsearch") == 0)
	{
	    no_hlsearch = !di->di_tv.vval.v_نمبر;
	    redraw_all_later(UPD_SOME_VALID);
	}
#اگر_ختم
	واپس FALSE;
    }
    ورنہ اگر (di->di_tv.v_type != tv->v_type)
    {
	*type_error = TRUE;
	واپس FALSE;
    }
    واپس TRUE;
}

/*
 * Set variable "name" to value in "tv".
 * If the variable already exists, the value is updated.
 * Otherwise the variable is created.
 */
    خالی
سیٹ_var(
    char_u	*name,
    typval_T	*tv,
    صحیح		copy)	    // make copy of value in "tv"
{
    سیٹ_var_const(name, 0, تہی, tv, copy, تفویض_DECL, 0);
}

/*
 * Set variable "name" to value in "tv_arg".
 * When "sid" is non-zero "name" is in the script with this ID.
 * If the variable already exists and "is_const" is FALSE the value is updated.
 * Otherwise the variable is created.
 */
    صحیح
سیٹ_var_const(
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
    char_u	*name_tofree = تہی;
    hashtab_T	*ht = تہی;
    صحیح		is_script_local;
    صحیح		vim9script = اندر_vim9script();
    صحیح		var_اندر_vim9script;
    صحیح		var_اندر_autoload = FALSE;
    صحیح		flags = flags_arg;
    صحیح		free_tv_arg = !copy;  // free tv_arg if not used
    صحیح		rc = FAIL;

    اگر (sid != 0)
    {
	varname = تہی;
	اگر (SCRIPT_ID_VALID(sid))
	{
	    char_u	*auto_name = تہی;
	    اگر (ڈھونڈو_var_autoload_سابقہ(name, sid, &ht, &auto_name) != تہی)
	    {
		var_اندر_autoload = TRUE;
		varname = auto_name;
		name_tofree = varname;
	    }
	    ورنہ
		ht = &SCRIPT_VARS(sid);
	}
	اگر (varname == تہی)
	    varname = name;
    }
    ورنہ
    {
	scriptitem_T	*si;
	char_u		*auto_name = تہی;

	اگر (اندر_vim9script()
	    && SCRIPT_ID_VALID(current_sctx.sc_sid)
	    && (si = SCRIPT_شے(current_sctx.sc_sid))
					      ->sn_autoload_سابقہ != تہی
	    && (is_export
		|| ڈھونڈو_var_autoload_سابقہ(name, 0, تہی, &auto_name)
								    != تہی))
	{
	    // In a vim9 autoload script an exported variable is put in the
	    // global namespace with the autoload prefix.
	    var_اندر_autoload = TRUE;
	    varname = auto_name != تہی ? auto_name
		      : concat_str(si->sn_autoload_سابقہ, name);
	    اگر (varname == تہی)
		جاؤ failed;
	    name_tofree = varname;
	    ht = &globvarht;
	}
	ورنہ
	    ht = ڈھونڈو_var_ht(name, &varname);
    }
    اگر (ht == تہی || *varname == NUL)
    {
	semsg(_(e_illegal_variable_name_str), name);
	جاؤ failed;
    }
    is_script_local = ht == get_script_local_ht() || sid != 0
							    || var_اندر_autoload;

    اگر (vim9script
	    && !is_script_local
	    && (flags & (تفویض_NO_DECL | تفویض_DECL)) == 0
	    && (flags & (تفویض_CONST | تفویض_FINAL)) == 0
	    && name[1] == ':')
    {
	vim9_declare_error(name);
	جاؤ failed;
    }
    اگر ((flags & تفویض_FOR_LOOP) && is_scoped_variable(name))
	// Do not make g:var, w:var, b:var or t:var final.
	flags &= ~تفویض_FINAL;

    var_اندر_vim9script = is_script_local && current_script_is_vim9();
    اگر (var_اندر_vim9script && name[0] == '_' && name[1] == NUL)
    {
	// For "[a, _] = list" the underscore is ignored.
	اگر ((flags & تفویض_UNPACK) == 0)
	    emsg(_(e_cannot_use_underscore_یہاں));
	جاؤ failed;
    }

    di = ڈھونڈو_var_اندر_ht(ht, 0, varname, TRUE);

    اگر (di == تہی && var_اندر_vim9script)
    {
	imported_T  *import = ڈھونڈو_imported(varname, 0, FALSE);

	اگر (import != تہی)
	{
	    // imported name space cannot be used
	    اگر ((flags & تفویض_NO_DECL) == 0)
	    {
		semsg(_(e_redefining_imported_شے_str), name);
		جاؤ failed;
	    }
	    semsg(_(e_cannot_use_str_itself_it_is_imported), name);
	    جاؤ failed;
	}
	اگر (!اندر_vim9script())
	{
	    semsg(_(e_cannot_create_vim9_script_variable_اندر_function_str),
									 name);
	    جاؤ failed;
	}
    }

    // Search in parent scope which is possible to reference from lambda
    اگر (di == تہی)
	di = ڈھونڈو_var_اندر_scoped_ht(name, TRUE);

    اگر ((tv->v_type == VAR_FUNC || tv->v_type == VAR_PARTIAL)
				  && var_wrong_func_name(name, di == تہی))
	جاؤ failed;

    اگر (need_convert_مقصد_bool(type, tv))
    {
	// Destination is a bool and the value is not, but it can be
	// converted.
	CLEAR_میدان(bool_tv);
	bool_tv.v_type = VAR_BOOL;
	bool_tv.vval.v_نمبر = tv2bool(tv) ? VVAL_TRUE : VVAL_FALSE;
	tv = &bool_tv;
    }

    اگر (di != تہی)
    {
	// Item already exists.  Allowed to replace when reloading.
	اگر ((di->di_flags & DI_FLAGS_RELOAD) == 0)
	{
	    اگر ((flags & (تفویض_CONST | تفویض_FINAL))
					 && (flags & تفویض_FOR_LOOP) == 0)
	    {
		emsg(_(e_cannot_modify_existing_variable));
		جاؤ failed;
	    }

	    اگر (is_script_local && vim9script
			  && (flags & (تفویض_NO_DECL | تفویض_DECL)) == 0)
	    {
		semsg(_(e_redefining_script_شے_str), name);
		جاؤ failed;
	    }

	    اگر (check_typval_is_قدر(&di->di_tv) == FAIL)
		جاؤ failed;

	    // List and Blob types can be modified in-place using the "+="
	    // compound operator.  For other types, this is not allowed.
	    صحیح type_inplace_modifiable =
		(di->di_tv.v_type == VAR_LIST || di->di_tv.v_type == VAR_BLOB);

	    اگر (var_اندر_vim9script && (flags & تفویض_FOR_LOOP) == 0
		    && ((flags & تفویض_COMPOUND_OP) == 0
			|| !type_inplace_modifiable))
	    {
		where_T where = WHERE_INIT;
		svar_T  *sv = ڈھونڈو_typval_اندر_script(&di->di_tv, sid, TRUE);

		اگر (sv != تہی)
		{
		    // check the type and adjust to bool if needed
		    اگر (var_idx > 0)
		    {
			where.wt_اشاریہ = var_idx;
			where.wt_قسم = WT_VARIABLE;
		    }
		    اگر (check_script_var_type(sv, tv, name, where) == FAIL)
			جاؤ failed;
		    اگر (type == تہی)
			type = sv->sv_type;
		    sv->sv_flags |= SVFLAG_ASSIGNED;
		}
	    }

	    // Modifying a final variable with a List value using the "+="
	    // operator is allowed.  For other types, it is not allowed.
	    اگر ((((flags & تفویض_FOR_LOOP) == 0 || (flags & تفویض_DECL) == 0)
			&& ((flags & تفویض_COMPOUND_OP) == 0
			    || !type_inplace_modifiable))
				 ? var_check_permission(di, name) == FAIL
				 : var_check_ro(di->di_flags, name, FALSE))
		جاؤ failed;
	}
	ورنہ
	{
	    // can only redefine once
	    di->di_flags &= ~DI_FLAGS_RELOAD;

	    // A Vim9 script-local variable is also present in sn_all_vars
	    // and sn_var_vals.  It may set "type" from "tv".
	    اگر (var_اندر_vim9script || var_اندر_autoload)
		update_vim9_script_var(FALSE, di,
			var_اندر_autoload ? name : di->di_کلید, flags,
			tv, &type, (flags & تفویض_NO_MEMBER_TYPE) == 0);
	}

	// existing variable, need to clear the value

	// In Vim9 script a String v: variable only takes a String.
	اگر (ht == &vimvarht && di->di_tv.v_type == VAR_STRING
							    && اندر_vim9script())
	{
	    where_T	where = WHERE_INIT;

	    اگر (check_typval_type(&t_string, tv, where) == FAIL)
		جاؤ failed;
	}

	// Handle setting internal v: variables separately where needed to
	// prevent changing the type.
	صحیح type_error = FALSE;
	اگر (ht == &vimvarht
		&& !before_سیٹ_vvar(varname, di, tv, copy, &type_error))
	{
	    اگر (type_error)
		semsg(_(e_setting_v_str_مقصد_قدر_with_wrong_type), varname);
	    جاؤ failed;
	}

	clear_tv(&di->di_tv);

	اگر ((flags & تفویض_UPDATE_بلاک_ID)
				       && SCRIPT_ID_VALID(current_sctx.sc_sid))
	{
	    scriptitem_T *si = SCRIPT_شے(current_sctx.sc_sid);

	    update_script_var_بلاک_id(name, si->sn_current_بلاک_id);
	}
    }
    ورنہ
    {
	// Item not found, check if a function already exists.
	اگر (is_script_local && (flags & (تفویض_NO_DECL | تفویض_DECL)) == 0
	       && lookup_scriptitem(name, STRLEN(name), FALSE, تہی) == OK)
	{
	    semsg(_(e_redefining_script_شے_str), name);
	    جاؤ failed;
	}

	// add a new variable
	اگر (var_اندر_vim9script && (flags & تفویض_NO_DECL))
	{
	    semsg(_(e_unknown_variable_str), name);
	    جاؤ failed;
	}

	اگر (check_hashtab_frozen(ht, "add variable"))
	    جاؤ failed;

	// Can't add "v:" or "a:" variable.
	اگر (ht == &vimvarht || ht == get_funccal_دلائل_ht())
	{
	    semsg(_(e_illegal_variable_name_str), name);
	    جاؤ failed;
	}

	// Make sure the variable name is valid.  In Vim9 script an
	// autoload variable must be prefixed with "g:" unless in an
	// autoload script.
	اگر (!valid_varname(varname, -1, !vim9script
			|| STRNCMP(name, "g:", 2) == 0 || var_اندر_autoload))
	    جاؤ failed;

	di = alloc(offsetof(dictitem_T, di_کلید) + STRLEN(varname) + 1);
	اگر (di == تہی)
	    جاؤ failed;
	STRCPY(di->di_کلید, varname);
	اگر (hash_جوڑو(ht, DI2HIKEY(di), "add variable") == FAIL)
	{
	    vim_free(di);
	    جاؤ failed;
	}
	di->di_flags = DI_FLAGS_ALLOC;
	اگر (flags & (تفویض_CONST | تفویض_FINAL))
	    di->di_flags |= DI_FLAGS_تالا;

	// A Vim9 script-local variable is also added to sn_all_vars and
	// sn_var_vals. It may set "type" from "tv".
	اگر (var_اندر_vim9script || var_اندر_autoload)
	    update_vim9_script_var(TRUE, di,
		    var_اندر_autoload ? name : di->di_کلید, flags,
			  tv, &type, (flags & تفویض_NO_MEMBER_TYPE) == 0);
    }

    dest_tv = &di->di_tv;
    اگر (copy || tv->v_type == VAR_نمبر || tv->v_type == VAR_FLOAT)
	copy_tv(tv, dest_tv);
    ورنہ
    {
	*dest_tv = *tv;
	dest_tv->v_تالا = 0;
	init_tv(tv);
    }
    free_tv_arg = FALSE;

    اگر (vim9script && type != تہی)
	سیٹ_tv_type(dest_tv, type);

    // ":const var = value" locks the value
    // ":final var = value" locks "var"
    اگر (flags & تفویض_CONST)
	// Like :lockvar! name: lock the value and what it contains, but only
	// if the reference count is up to one.  That locks only literal
	// values.
	شے_تالا(dest_tv, DICT_MAXNEST, TRUE, TRUE);

    rc = OK;

failed:
    vim_free(name_tofree);
    اگر (free_tv_arg)
	clear_tv(tv_arg);

    واپس rc;
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
		    || قدر_check_تالا(di->di_tv.v_تالا, name, FALSE)
		    || var_check_تالا(di->di_flags, name, FALSE))
	واپس FAIL;
    واپس OK;
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
	اگر (name == تہی)
	    emsg(_(e_cannot_change_readonly_variable));
	ورنہ
	    semsg(_(e_cannot_change_readonly_variable_str),
				       use_gettext ? (char_u *)_(name) : name);
	واپس TRUE;
    }
    اگر ((flags & DI_FLAGS_RO_SBX) && sandbox)
    {
	اگر (name == تہی)
	    emsg(_(e_cannot_سیٹ_variable_اندر_sandbox));
	ورنہ
	    semsg(_(e_cannot_سیٹ_variable_اندر_sandbox_str),
				       use_gettext ? (char_u *)_(name) : name);
	واپس TRUE;
    }
    واپس FALSE;
}

/*
 * Return TRUE if di_flags "flags" indicates variable "name" is locked.
 * Also give an error message.
 */
    صحیح
var_check_تالا(صحیح flags, char_u *name, صحیح use_gettext)
{
    اگر (flags & DI_FLAGS_تالا)
    {
	semsg(_(e_variable_is_locked_str),
				       use_gettext ? (char_u *)_(name) : name);
	واپس TRUE;
    }
    واپس FALSE;
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
	اگر (name == تہی)
	    emsg(_(e_cannot_delete_variable));
	ورنہ
	    semsg(_(e_cannot_delete_variable_str),
				      use_gettext ? (char_u *)_(name) : name);
	واپس TRUE;
    }
    واپس FALSE;
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
    اگر (!((vim_strchr((char_u *)"wbt", name[0]) != تہی
		    || (!اندر_vim9script() && name[0] == 's')) && name[1] == ':')
	    && !ASCII_ISUPPER((name[0] != NUL && name[1] == ':')
						     ? name[2] : name[0])
	    && vim_strchr(name, '#') == تہی)
    {
	semsg(_(e_funcref_variable_name_must_آغاز_with_capital_str), name);
	واپس TRUE;
    }
    // Don't allow hiding a function.  When "v" is not NULL we might be
    // assigning another function to the same var, the type is checked
    // below.
    اگر (new_var && function_exists(name, FALSE))
    {
	semsg(_(e_variable_name_conflicts_with_existing_function_str),
								    name);
	واپس TRUE;
    }
    واپس FALSE;
}

/*
 * Return TRUE if "flags" indicates variable "name" has a locked (immutable)
 * value.  Also give an error message, using "name" or _("name") when
 * "use_gettext" is TRUE.
 */
    صحیح
قدر_check_تالا(صحیح تالا, char_u *name, صحیح use_gettext)
{
    اگر (تالا & VAR_LOCKED)
    {
	اگر (name == تہی)
	    emsg(_(e_قدر_is_locked));
	ورنہ
	    semsg(_(e_قدر_is_locked_str),
				       use_gettext ? (char_u *)_(name) : name);
	واپس TRUE;
    }
    اگر (تالا & VAR_FIXED)
    {
	اگر (name == تہی)
	    emsg(_(e_cannot_change_قدر));
	ورنہ
	    semsg(_(e_cannot_change_قدر_کا_str),
				       use_gettext ? (char_u *)_(name) : name);
	واپس TRUE;
    }
    واپس FALSE;
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

    برائے (p = varname; len < 0 ? *p != NUL : p < varname + len; ++p)
	اگر (!eval_isnamec1(*p) && (p == varname || !VIM_ISDIGIT(*p))
					 && !(autoload && *p == AUTOLOAD_CHAR))
	{
	    semsg(_(e_illegal_variable_name_str), varname);
	    واپس FALSE;
	}
    واپس TRUE;
}

/*
 * Implements the logic to retrieve local variable and option values.
 * Used by "getwinvar()" "gettabvar()" "gettabwinvar()" "getbufvar()".
 */
    ساکن خالی
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
    صحیح		مکمل = FALSE;
    switchwin_T	switchwin;
    صحیح		need_switch_win;
    صحیح		do_change_curbuf = buf != تہی && htname == 'b';

    ++emsg_off;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = تہی;

    اگر (varname != تہی && tp != تہی && win != تہی
	    && (htname != 'b' || buf != تہی))
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

		    اگر (opts != تہی)
		    {
			rettv_dict_سیٹ(rettv, opts);
			مکمل = TRUE;
		    }
		}
		ورنہ اگر (eval_option(&varname, rettv, TRUE) == OK)
		    // Local option
		    مکمل = TRUE;

		curbuf = save_curbuf;
	    }
	    ورنہ اگر (*varname == NUL)
	    {
		// Empty string: return a dict with all the local variables.
		اگر (htname == 'b')
		    v = &buf->b_bufvar;
		ورنہ اگر (htname == 'w')
		    v = &win->w_winvar;
		ورنہ
		    v = &tp->tp_winvar;
		copy_tv(&v->di_tv, rettv);
		مکمل = TRUE;
	    }
	    ورنہ
	    {
		hashtab_T	*ht;

		اگر (htname == 'b')
		    ht = &buf->b_vars->dv_hashtab;
		ورنہ اگر (htname == 'w')
		    ht = &win->w_vars->dv_hashtab;
		ورنہ
		    ht = &tp->tp_vars->dv_hashtab;

		// Look up the variable.
		v = ڈھونڈو_var_اندر_ht(ht, htname, varname, FALSE);
		اگر (v != تہی)
		{
		    copy_tv(&v->di_tv, rettv);
		    مکمل = TRUE;
		}
	    }
	}

	اگر (need_switch_win)
	    // restore previous notion of curwin
	    restore_win(&switchwin, TRUE);
    }

    اگر (!مکمل && deftv->v_type != VAR_UNKNOWN)
	// use the default value
	copy_tv(deftv, rettv);

    --emsg_off;
}

/*
 * getwinvar() and gettabwinvar()
 */
    ساکن خالی
getwinvar(
    typval_T	*argvars,
    typval_T	*rettv,
    صحیح		off)	    // 1 for gettabwinvar()
{
    char_u	*varname;
    tabpage_T	*tp;
    win_T	*win;

    اگر (off == 1)
	tp = ڈھونڈو_tabpage((صحیح)tv_get_نمبر_chk(&argvars[0], تہی));
    ورنہ
	tp = curtab;
    win = ڈھونڈو_win_by_nr(&argvars[off], tp);
    varname = tv_get_string_chk(&argvars[off + 1]);

    get_var_from(varname, rettv, &argvars[off + 2], 'w', tp, win, تہی);
}

/*
 * Set option "varname" to the value of "varp" for the current buffer/window.
 */
    ساکن خالی
سیٹ_option_from_tv(char_u *varname, typval_T *varp)
{
    طویل	numval = 0;
    char_u	*strval;
    char_u	nbuf[NUMBUFLEN];
    صحیح		error = FALSE;

    صحیح		opt_idx = findoption(varname);
    اگر (opt_idx < 0)
    {
	semsg(_(e_unknown_option_str_2), varname);
	واپس;
    }
    صحیح		opt_p_flags = get_option_flags(opt_idx);

    اگر (varp->v_type == VAR_BOOL)
    {
	اگر (opt_p_flags & P_STRING)
	{
	    emsg(_(e_string_required));
	    واپس;
	}
	numval = (طویل)varp->vval.v_نمبر;
	strval = (char_u *)"0";  // avoid using "false"
    }
    ورنہ
    {
	اگر ((opt_p_flags & (P_عدد|P_BOOL))
		&& (!اندر_vim9script() || varp->v_type != VAR_STRING))
	    numval = (طویل)tv_get_نمبر_chk(varp, &error);
	اگر (!error)
	    strval = tv_get_string_buf_chk(varp, nbuf);
    }
    اگر (!error && strval != تہی)
	سیٹ_option_قدر_give_err(varname, numval, strval, OPT_LOCAL);
}

/*
 * "setwinvar()" and "settabwinvar()" functions
 */
    ساکن خالی
setwinvar(typval_T *argvars, صحیح off)
{
    win_T	*win;
    switchwin_T	switchwin;
    صحیح		need_switch_win;
    char_u	*varname, *winvarname;
    typval_T	*varp;
    tabpage_T	*tp = تہی;

    اگر (check_secure())
	واپس;

    اگر (off == 1)
	tp = ڈھونڈو_tabpage((صحیح)tv_get_نمبر_chk(&argvars[0], تہی));
    ورنہ
	tp = curtab;
    win = ڈھونڈو_win_by_nr(&argvars[off], tp);
    varname = tv_get_string_chk(&argvars[off + 1]);
    varp = &argvars[off + 2];

    اگر (win == تہی || varname == تہی)
	واپس;

    need_switch_win = !(tp == curtab && win == curwin);
    اگر (!need_switch_win
	    || switch_win(&switchwin, win, tp, TRUE) == OK)
    {
	اگر (*varname == '&')
	    سیٹ_option_from_tv(varname + 1, varp);
	ورنہ
	{
	    winvarname = alloc(STRLEN(varname) + 3);
	    اگر (winvarname != تہی)
	    {
		STRCPY(winvarname, "w:");
		STRCPY(winvarname + 2, varname);
		سیٹ_var(winvarname, varp, TRUE);
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
    خالی
reset_v_option_vars(خالی)
{
    سیٹ_vim_var_string(VV_OPTION_NEW, تہی, -1);
    سیٹ_vim_var_string(VV_OPTION_OLD, تہی, -1);
    سیٹ_vim_var_string(VV_OPTION_OLDLOCAL, تہی, -1);
    سیٹ_vim_var_string(VV_OPTION_OLDGLOBAL, تہی, -1);
    سیٹ_vim_var_string(VV_OPTION_TYPE, تہی, -1);
    سیٹ_vim_var_string(VV_OPTION_COMMAND, تہی, -1);
}

/*
 * Add an assert error to v:errors.
 */
    خالی
assert_error(garray_T *gap)
{
    ساخت vimvar   *vp = &vimvars[VV_ERRORS];

    اگر (vp->vv_tv_type != VAR_LIST || vimvars[VV_ERRORS].vv_list == تہی)
	// Make sure v:errors is a list.
	سیٹ_vim_var_list(VV_ERRORS, list_alloc());
    list_append_string(vimvars[VV_ERRORS].vv_list, gap->ga_ڈیٹا, gap->ga_len);
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
	اگر (tofree != تہی)
	    name = tofree;
	n = (eval_variable(name, len, 0, &tv, تہی,
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
    واپس n;
}

ساکن lval_T	*redir_lval = تہی;
#تعریف_کرو EVALCMD_BUSY (redir_lval == (lval_T *)&redir_lval)
ساکن garray_T redir_ga;	// only valid when redir_lval is not NULL
ساکن char_u	*redir_endp = تہی;
ساکن char_u	*redir_varname = تہی;

    صحیح
alloc_redir_lval(خالی)
{
    redir_lval = ALLOC_CLEAR_ONE(lval_T);
    اگر (redir_lval == تہی)
	واپس FAIL;
    واپس OK;
}

    خالی
clear_redir_lval(خالی)
{
    VIM_CLEAR(redir_lval);
}

    خالی
init_redir_ga(خالی)
{
    ga_init2(&redir_ga, حجم(حرف), 500);
}

/*
 * Start recording command output to a variable
 * When "append" is TRUE append to an existing variable.
 * Returns OK if successfully completed the setup.  FAIL otherwise.
 */
    صحیح
var_redir_آغاز(char_u *name, صحیح append)
{
    صحیح		called_emsg_before;
    typval_T	tv;

    // Catch a bad name early.
    اگر (!eval_isnamec1(*name))
    {
	emsg(_(e_invalid_دلیل));
	واپس FAIL;
    }

    // Make a copy of the name, it is used in redir_lval until redir ends.
    redir_varname = vim_strsave(name);
    اگر (redir_varname == تہی)
	واپس FAIL;

    اگر (alloc_redir_lval() == FAIL)
    {
	var_redir_stop();
	واپس FAIL;
    }

    // The output is stored in growarray "redir_ga" until redirection ends.
    init_redir_ga();

    // Parse the variable name (can be a dict or list entry).
    redir_endp = get_lval(redir_varname, تہی, redir_lval, FALSE, FALSE, 0,
							     FNE_CHECK_آغاز);
    اگر (redir_endp == تہی || redir_lval->ll_name == تہی || *redir_endp != NUL)
    {
	clear_lval(redir_lval);
	اگر (redir_endp != تہی && *redir_endp != NUL)
	    // Trailing characters are present after the variable name
	    semsg(_(e_trailing_characters_str), redir_endp);
	ورنہ
	    semsg(_(e_invalid_دلیل_str), name);
	redir_endp = تہی;  // don't store a value, only cleanup
	var_redir_stop();
	واپس FAIL;
    }

    // check if we can write to the variable: set it to or append an empty
    // string
    called_emsg_before = called_emsg;
    tv.v_type = VAR_STRING;
    tv.vval.v_string = (char_u *)"";
    اگر (append)
	سیٹ_var_lval(redir_lval, redir_endp, &tv, TRUE,
					     تفویض_NO_DECL, (char_u *)".", 0);
    ورنہ
	سیٹ_var_lval(redir_lval, redir_endp, &tv, TRUE,
					     تفویض_NO_DECL, (char_u *)"=", 0);
    clear_lval(redir_lval);
    اگر (called_emsg > called_emsg_before)
    {
	redir_endp = تہی;  // don't store a value, only cleanup
	var_redir_stop();
	واپس FAIL;
    }

    واپس OK;
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
    خالی
var_redir_str(char_u *قدر, صحیح قدر_len)
{
    صحیح		len;

    اگر (redir_lval == تہی)
	واپس;

    اگر (قدر_len == -1)
	len = (صحیح)STRLEN(قدر);	// Append the entire string
    ورنہ
	len = قدر_len;		// Append only "value_len" characters

    اگر (ga_grow(&redir_ga, len) == OK)
    {
	mch_memmove((حرف *)redir_ga.ga_ڈیٹا + redir_ga.ga_len, قدر, len);
	redir_ga.ga_len += len;
    }
    ورنہ
	var_redir_stop();
}

/*
 * Stop redirecting command output to a variable.
 * Frees the allocated memory.
 */
    خالی
var_redir_stop(خالی)
{
    typval_T	tv;

    اگر (EVALCMD_BUSY)
    {
	redir_lval = تہی;
	واپس;
    }

    اگر (redir_lval != تہی)
    {
	// If there was no error: assign the text to the variable.
	اگر (redir_endp != تہی)
	{
	    ga_append(&redir_ga, NUL);  // Append the trailing NUL.
	    tv.v_type = VAR_STRING;
	    tv.vval.v_string = redir_ga.ga_ڈیٹا;
	    // Call get_lval() again, if it's inside a Dict or List it may
	    // have changed.
	    redir_endp = get_lval(redir_varname, تہی, redir_lval,
					FALSE, FALSE, 0, FNE_CHECK_آغاز);
	    اگر (redir_endp != تہی && redir_lval->ll_name != تہی)
		سیٹ_var_lval(redir_lval, redir_endp, &tv, FALSE, 0,
							     (char_u *)".", 0);
	    clear_lval(redir_lval);
	}

	// free the collected output
	VIM_CLEAR(redir_ga.ga_ڈیٹا);

	VIM_CLEAR(redir_lval);
    }
    VIM_CLEAR(redir_varname);
}

/*
 * Get the collected redirected text and clear redir_ga.
 */
    char_u *
get_clear_redir_ga(خالی)
{
    char_u *res;

    ga_append(&redir_ga, NUL);  // Append the trailing NUL.
    res = redir_ga.ga_ڈیٹا;
    redir_ga.ga_ڈیٹا = تہی;
    واپس res;
}

/*
 * "gettabvar()" function
 */
    خالی
f_gettabvar(typval_T *argvars, typval_T *rettv)
{
    char_u	*varname;
    tabpage_T	*tp;
    win_T	*win = تہی;

    اگر (اندر_vim9script()
	    && (check_for_نمبر_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	واپس;

    varname = tv_get_string_chk(&argvars[1]);
    tp = ڈھونڈو_tabpage((صحیح)tv_get_نمبر_chk(&argvars[0], تہی));
    اگر (tp != تہی)
	win = tp == curtab || tp->tp_firstwin == تہی ? firstwin
		: tp->tp_firstwin;

    get_var_from(varname, rettv, &argvars[2], 't', tp, win, تہی);
}

/*
 * "gettabwinvar()" function
 */
    خالی
f_gettabwinvar(typval_T *argvars, typval_T *rettv)
{
    اگر (اندر_vim9script()
	    && (check_for_نمبر_arg(argvars, 0) == FAIL
		|| check_for_نمبر_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL))
	واپس;

    getwinvar(argvars, rettv, 1);
}

/*
 * "getwinvar()" function
 */
    خالی
f_getwinvar(typval_T *argvars, typval_T *rettv)
{
    اگر (اندر_vim9script()
	    && (check_for_نمبر_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	واپس;

    getwinvar(argvars, rettv, 0);
}

/*
 * "getbufvar()" function
 */
    خالی
f_getbufvar(typval_T *argvars, typval_T *rettv)
{
    char_u	*varname;
    buf_T	*buf;

    اگر (اندر_vim9script()
	    && (check_for_بفر_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	واپس;

    varname = tv_get_string_chk(&argvars[1]);
    buf = tv_get_buf_from_arg(&argvars[0]);

    get_var_from(varname, rettv, &argvars[2], 'b', curtab, curwin, buf);
}

/*
 * "settabvar()" function
 */
    خالی
f_settabvar(typval_T *argvars, typval_T *rettv غیر_مستعمل)
{
    tabpage_T	*save_curtab;
    tabpage_T	*tp;
    tabpage_T	*save_lu_tp;
    char_u	*varname, *tabvarname;
    typval_T	*varp;

    اگر (check_secure())
	واپس;

    اگر (اندر_vim9script()
	    && (check_for_نمبر_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	واپس;

    tp = ڈھونڈو_tabpage((صحیح)tv_get_نمبر_chk(&argvars[0], تہی));
    varname = tv_get_string_chk(&argvars[1]);
    varp = &argvars[2];

    اگر (varname == تہی || tp == تہی)
	واپس;

    save_curtab = curtab;
    save_lu_tp = lastused_tabpage;
    goto_tabpage_tp(tp, FALSE, FALSE);

    tabvarname = alloc(STRLEN(varname) + 3);
    اگر (tabvarname != تہی)
    {
	STRCPY(tabvarname, "t:");
	STRCPY(tabvarname + 2, varname);
	سیٹ_var(tabvarname, varp, TRUE);
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
    خالی
f_settabwinvar(typval_T *argvars, typval_T *rettv غیر_مستعمل)
{
    اگر (اندر_vim9script()
	    && (check_for_نمبر_arg(argvars, 0) == FAIL
		|| check_for_نمبر_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL))
	واپس;

    setwinvar(argvars, 1);
}

/*
 * "setwinvar()" function
 */
    خالی
f_setwinvar(typval_T *argvars, typval_T *rettv غیر_مستعمل)
{
    اگر (اندر_vim9script()
	    && (check_for_نمبر_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	واپس;

    setwinvar(argvars, 0);
}

/*
 * "setbufvar()" function
 */
    خالی
f_setbufvar(typval_T *argvars, typval_T *rettv غیر_مستعمل)
{
    buf_T	*buf;
    char_u	*varname, *bufvarname;
    typval_T	*varp;

    اگر (check_secure())
	واپس;

    اگر (اندر_vim9script()
	    && (check_for_بفر_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	واپس;

    varname = tv_get_string_chk(&argvars[1]);
    buf = tv_get_buf_from_arg(&argvars[0]);
    varp = &argvars[2];

    اگر (buf == تہی || varname == تہی)
	واپس;

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
	    سیٹ_option_from_tv(varname + 1, varp);

	    // reset notion of buffer
	    aucmd_restbuf(&aco);
	}
	curwin->w_topline = old_topline;
    }
    ورنہ
    {
	bufvarname = alloc(STRLEN(varname) + 3);
	اگر (bufvarname != تہی)
	{
	    buf_T *save_curbuf = curbuf;

	    curbuf = buf;
	    STRCPY(bufvarname, "b:");
	    STRCPY(bufvarname + 2, varname);
	    سیٹ_var(bufvarname, varp, TRUE);
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

    CLEAR_میدان(res);
    اگر (arg->v_type == VAR_PARTIAL && arg->vval.v_partial != تہی)
    {
	res.cb_partial = arg->vval.v_partial;
	++res.cb_partial->pt_refcount;
	res.cb_name = partial_name(res.cb_partial);
    }
    ورنہ
    {
	اگر (arg->v_type == VAR_STRING && arg->vval.v_string != تہی
					       && محفوظ_isdigit(*arg->vval.v_string))
	    r = FAIL;
	ورنہ اگر (arg->v_type == VAR_FUNC || arg->v_type == VAR_STRING)
	{
	    res.cb_name = arg->vval.v_string;
	    اگر (arg->v_type == VAR_STRING)
	    {
		char_u *name = get_scriptlocal_funcname(arg->vval.v_string);
		اگر (name != تہی)
		{
		    res.cb_name = name;
		    res.cb_free_name = TRUE;
		}
	    }
	    func_ref(res.cb_name);
	}
	ورنہ اگر (arg->v_type == VAR_نمبر && arg->vval.v_نمبر == 0)
	    res.cb_name = (char_u *)"";
	ورنہ
	    r = FAIL;

	اگر (r == FAIL)
	{
	    emsg(_(e_invalid_callback_دلیل));
	    res.cb_name = تہی;
	}
    }
    واپس res;
}

/*
 * Copy a callback into a typval_T.
 */
    خالی
put_callback(callback_T *cb, typval_T *tv)
{
    اگر (cb->cb_partial != تہی)
    {
	tv->v_type = VAR_PARTIAL;
	tv->vval.v_partial = cb->cb_partial;
	++tv->vval.v_partial->pt_refcount;
    }
    ورنہ
    {
	tv->v_type = VAR_FUNC;
	tv->vval.v_string = vim_strsave(cb->cb_name);
	func_ref(cb->cb_name);
    }
}

    ساکن bool
does_callback_own_cb_name(callback_T *cb)
{
    // If cb_partial != NULL then *cb->cb_name is owned by the partial.
    واپس cb->cb_partial || cb->cb_free_name;
}

/*
 * Make a copy of "src" into "dest", allocating the function name if needed,
 * without incrementing the refcount.
 */
    خالی
سیٹ_callback(callback_T *dest, callback_T *src)
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
    خالی
copy_callback(callback_T *dest, callback_T *src)
{
    dest->cb_partial = src->cb_partial;
    اگر (dest->cb_partial != تہی)
    {
	dest->cb_name = src->cb_name;
	dest->cb_free_name = FALSE;
	++dest->cb_partial->pt_refcount;
    }
    ورنہ
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
    خالی
expand_autload_callback(callback_T *cb)
{
    char_u	*name;
    char_u	*p;
    imported_T	*import;

    اگر (!اندر_vim9script() || cb->cb_name == تہی
	    || (!cb->cb_free_name
	       && (cb->cb_partial == تہی || cb->cb_partial->pt_name == تہی)))
	واپس;
    اگر (cb->cb_partial != تہی)
	name = cb->cb_partial->pt_name;
    ورنہ
	name = cb->cb_name;
    p = vim_strchr(name, '.');
    اگر (p == تہی)
	واپس;

    import = ڈھونڈو_imported(name, p - name, FALSE);
    اگر (import == تہی || !SCRIPT_ID_VALID(import->imp_sid))
	واپس;

    scriptitem_T *si = SCRIPT_شے(import->imp_sid);
    اگر (si->sn_autoload_سابقہ == تہی)
	واپس;

    char_u *newname = concat_str(si->sn_autoload_سابقہ, p + 1);
    اگر (newname == تہی)
	واپس;

    اگر (cb->cb_partial != تہی)
    {
	اگر (cb->cb_name == cb->cb_partial->pt_name)
	    cb->cb_name = newname;
	vim_free(cb->cb_partial->pt_name);
	cb->cb_partial->pt_name = newname;
    }
    ورنہ
    {
	vim_free(cb->cb_name);
	cb->cb_name = newname;
    }
}

/*
 * Unref/free "callback" returned by get_callback() or set_callback().
 */
    خالی
free_callback(callback_T *callback)
{
    اگر (callback->cb_partial != تہی)
    {
	partial_unref(callback->cb_partial);
	callback->cb_partial = تہی;
    }
    ورنہ اگر (callback->cb_name != تہی)
	func_unref(callback->cb_name);
    اگر (callback->cb_free_name)
    {
	vim_free(callback->cb_name);
	callback->cb_free_name = FALSE;
    }
    callback->cb_name = تہی;
}

#اگر_ختم // FEAT_EVAL
