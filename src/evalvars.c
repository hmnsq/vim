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

#அந்தர்பாவ "vim.h"

#எனில் defined(FEAT_EVAL)

நிலையான dictitem_T	globvars_var;		// variable used for g:
நிலையான dict_T		globvardict;		// Dictionary with g: variables
#define globvarht globvardict.dv_hashtab

/*
 * Old Vim variables such as "v:version" are also available without the "v:".
 * Also in functions.  We need a special hashtable for them.
 */
நிலையான hashtab_T	compat_hashtab;

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

வகைவரை கட்டமைப்பு vimvar vimvar_T;

நிலையான கட்டமைப்பு vimvar
{
    எழுத்து	*vv_name;	// name of variable, without v:
    dictitem16_T vv_di;		// value and name for key (max 16 chars!)
    type_T	*vv_type;	// type or NULL
    எழுத்து	vv_flags;	// VV_COMPAT, VV_RO, VV_RO_SBX
} vimvars[VV_LEN] =
{
    // The order here must match the VV_ defines in vim.h!
    // Initializing a union does not work, leave tv.vval empty to get zero's.
    {VV_NAME("count",		 VAR_எண்), வெற்று, VV_COMPAT+VV_RO},
    {VV_NAME("count1",		 VAR_எண்), வெற்று, VV_RO},
    {VV_NAME("prevcount",	 VAR_எண்), வெற்று, VV_RO},
    {VV_NAME("errmsg",		 VAR_STRING), வெற்று, VV_COMPAT},
    {VV_NAME("warningmsg",	 VAR_STRING), வெற்று, 0},
    {VV_NAME("statusmsg",	 VAR_STRING), வெற்று, 0},
    {VV_NAME("shell_error",	 VAR_எண்), வெற்று, VV_COMPAT+VV_RO},
    {VV_NAME("this_session",	 VAR_STRING), வெற்று, VV_COMPAT},
    {VV_NAME("version",		 VAR_எண்), வெற்று, VV_COMPAT+VV_RO},
    {VV_NAME("lnum",		 VAR_எண்), வெற்று, VV_RO_SBX},
    {VV_NAME("termresponse",	 VAR_STRING), வெற்று, VV_RO},
    {VV_NAME("fname",		 VAR_STRING), வெற்று, VV_RO},
    {VV_NAME("lang",		 VAR_STRING), வெற்று, VV_RO},
    {VV_NAME("lc_time",		 VAR_STRING), வெற்று, VV_RO},
    {VV_NAME("ctype",		 VAR_STRING), வெற்று, VV_RO},
    {VV_NAME("charconvert_from", VAR_STRING), வெற்று, VV_RO},
    {VV_NAME("charconvert_to",	 VAR_STRING), வெற்று, VV_RO},
    {VV_NAME("fname_in",	 VAR_STRING), வெற்று, VV_RO},
    {VV_NAME("fname_out",	 VAR_STRING), வெற்று, VV_RO},
    {VV_NAME("fname_new",	 VAR_STRING), வெற்று, VV_RO},
    {VV_NAME("fname_diff",	 VAR_STRING), வெற்று, VV_RO},
    {VV_NAME("cmdarg",		 VAR_STRING), வெற்று, VV_RO},
    {VV_NAME("foldstart",	 VAR_எண்), வெற்று, VV_RO_SBX},
    {VV_NAME("foldend",		 VAR_எண்), வெற்று, VV_RO_SBX},
    {VV_NAME("folddashes",	 VAR_STRING), வெற்று, VV_RO_SBX},
    {VV_NAME("foldlevel",	 VAR_எண்), வெற்று, VV_RO_SBX},
    {VV_NAME("progname",	 VAR_STRING), வெற்று, VV_RO},
    {VV_NAME("servername",	 VAR_STRING), வெற்று, VV_RO},
    {VV_NAME("dying",		 VAR_எண்), வெற்று, VV_RO},
    {VV_NAME("exception",	 VAR_STRING), வெற்று, VV_RO},
    {VV_NAME("throwpoint",	 VAR_STRING), வெற்று, VV_RO},
    {VV_NAME("register",	 VAR_STRING), வெற்று, VV_RO},
    {VV_NAME("cmdbang",		 VAR_எண்), வெற்று, VV_RO},
    {VV_NAME("insertmode",	 VAR_STRING), வெற்று, VV_RO},
    {VV_NAME("val",		 VAR_UNKNOWN), வெற்று, VV_RO},
    {VV_NAME("key",		 VAR_UNKNOWN), வெற்று, VV_RO},
    {VV_NAME("profiling",	 VAR_எண்), வெற்று, VV_RO},
    {VV_NAME("fcs_reason",	 VAR_STRING), வெற்று, VV_RO},
    {VV_NAME("fcs_choice",	 VAR_STRING), வெற்று, 0},
    {VV_NAME("beval_bufnr",	 VAR_எண்), வெற்று, VV_RO},
    {VV_NAME("beval_winnr",	 VAR_எண்), வெற்று, VV_RO},
    {VV_NAME("beval_winid",	 VAR_எண்), வெற்று, VV_RO},
    {VV_NAME("beval_lnum",	 VAR_எண்), வெற்று, VV_RO},
    {VV_NAME("beval_col",	 VAR_எண்), வெற்று, VV_RO},
    {VV_NAME("beval_text",	 VAR_STRING), வெற்று, VV_RO},
    {VV_NAME("scrollstart",	 VAR_STRING), வெற்று, 0},
    {VV_NAME("swapname",	 VAR_STRING), வெற்று, VV_RO},
    {VV_NAME("swapchoice",	 VAR_STRING), வெற்று, 0},
    {VV_NAME("swapcommand",	 VAR_STRING), வெற்று, VV_RO},
    {VV_NAME("char",		 VAR_STRING), வெற்று, 0},
    {VV_NAME("mouse_win",	 VAR_எண்), வெற்று, 0},
    {VV_NAME("mouse_winid",	 VAR_எண்), வெற்று, 0},
    {VV_NAME("mouse_lnum",	 VAR_எண்), வெற்று, 0},
    {VV_NAME("mouse_col",	 VAR_எண்), வெற்று, 0},
    {VV_NAME("operator",	 VAR_STRING), வெற்று, VV_RO},
    {VV_NAME("searchforward",	 VAR_எண்), வெற்று, 0},
    {VV_NAME("hlsearch",	 VAR_எண்), வெற்று, 0},
    {VV_NAME("oldfiles",	 VAR_LIST), &t_list_string, 0},
    {VV_NAME("windowid",	 VAR_எண்), வெற்று, VV_RO},
    {VV_NAME("progpath",	 VAR_STRING), வெற்று, VV_RO},
    {VV_NAME("completed_item",	 VAR_DICT), &t_dict_string, 0},
    {VV_NAME("option_new",	 VAR_STRING), வெற்று, VV_RO},
    {VV_NAME("option_old",	 VAR_STRING), வெற்று, VV_RO},
    {VV_NAME("option_oldlocal",	 VAR_STRING), வெற்று, VV_RO},
    {VV_NAME("option_oldglobal", VAR_STRING), வெற்று, VV_RO},
    {VV_NAME("option_command",	 VAR_STRING), வெற்று, VV_RO},
    {VV_NAME("option_type",	 VAR_STRING), வெற்று, VV_RO},
    {VV_NAME("errors",		 VAR_LIST), &t_list_string, 0},
    {VV_NAME("false",		 VAR_BOOL), வெற்று, VV_RO},
    {VV_NAME("true",		 VAR_BOOL), வெற்று, VV_RO},
    {VV_NAME("none",		 VAR_SPECIAL), வெற்று, VV_RO},
    {VV_NAME("null",		 VAR_SPECIAL), வெற்று, VV_RO},
    {VV_NAME("numbermax",	 VAR_எண்), வெற்று, VV_RO},
    {VV_NAME("numbermin",	 VAR_எண்), வெற்று, VV_RO},
    {VV_NAME("numbersize",	 VAR_எண்), வெற்று, VV_RO},
    {VV_NAME("vim_did_enter",	 VAR_எண்), வெற்று, VV_RO},
    {VV_NAME("testing",		 VAR_எண்), வெற்று, 0},
    {VV_NAME("t_number",	 VAR_எண்), வெற்று, VV_RO},
    {VV_NAME("t_string",	 VAR_எண்), வெற்று, VV_RO},
    {VV_NAME("t_func",		 VAR_எண்), வெற்று, VV_RO},
    {VV_NAME("t_list",		 VAR_எண்), வெற்று, VV_RO},
    {VV_NAME("t_dict",		 VAR_எண்), வெற்று, VV_RO},
    {VV_NAME("t_float",		 VAR_எண்), வெற்று, VV_RO},
    {VV_NAME("t_bool",		 VAR_எண்), வெற்று, VV_RO},
    {VV_NAME("t_none",		 VAR_எண்), வெற்று, VV_RO},
    {VV_NAME("t_job",		 VAR_எண்), வெற்று, VV_RO},
    {VV_NAME("t_channel",	 VAR_எண்), வெற்று, VV_RO},
    {VV_NAME("t_blob",		 VAR_எண்), வெற்று, VV_RO},
    {VV_NAME("t_class",		 VAR_எண்), வெற்று, VV_RO},
    {VV_NAME("t_object",	 VAR_எண்), வெற்று, VV_RO},
    {VV_NAME("termrfgresp",	 VAR_STRING), வெற்று, VV_RO},
    {VV_NAME("termrbgresp",	 VAR_STRING), வெற்று, VV_RO},
    {VV_NAME("termu7resp",	 VAR_STRING), வெற்று, VV_RO},
    {VV_NAME("termstyleresp",	 VAR_STRING), வெற்று, VV_RO},
    {VV_NAME("termblinkresp",	 VAR_STRING), வெற்று, VV_RO},
    {VV_NAME("event",		 VAR_DICT), வெற்று, VV_RO},
    {VV_NAME("versionlong",	 VAR_எண்), வெற்று, VV_RO},
    {VV_NAME("echospace",	 VAR_எண்), வெற்று, VV_RO},
    {VV_NAME("argv",		 VAR_LIST), &t_list_string, VV_RO},
    {VV_NAME("collate",		 VAR_STRING), வெற்று, VV_RO},
    {VV_NAME("exiting",		 VAR_SPECIAL), வெற்று, VV_RO},
    {VV_NAME("colornames",       VAR_DICT), &t_dict_string, VV_RO},
    {VV_NAME("sizeofint",	 VAR_எண்), வெற்று, VV_RO},
    {VV_NAME("sizeoflong",	 VAR_எண்), வெற்று, VV_RO},
    {VV_NAME("sizeofpointer",	 VAR_எண்), வெற்று, VV_RO},
    {VV_NAME("maxcol",		 VAR_எண்), வெற்று, VV_RO},
    {VV_NAME("python3_version",	 VAR_எண்), வெற்று, VV_RO},
    {VV_NAME("t_typealias",	 VAR_எண்), வெற்று, VV_RO},
    {VV_NAME("t_enum",		 VAR_எண்), வெற்று, VV_RO},
    {VV_NAME("t_enumvalue",	 VAR_எண்), வெற்று, VV_RO},
    {VV_NAME("stacktrace",	 VAR_LIST), &t_list_dict_any, VV_RO},
    {VV_NAME("t_tuple",		 VAR_எண்), வெற்று, VV_RO},
    {VV_NAME("wayland_display",  VAR_STRING), வெற்று, VV_RO},
    {VV_NAME("clipmethod",	 VAR_STRING), வெற்று, VV_RO},
    {VV_NAME("termda1",		 VAR_STRING), வெற்று, VV_RO},
    {VV_NAME("termosc",	 VAR_STRING), வெற்று, VV_RO},
    {VV_NAME("vim_did_init",	 VAR_எண்), வெற்று, VV_RO},
    {VV_NAME("clipproviders",	 VAR_DICT), வெற்று, VV_RO},
    {VV_NAME("imagebackend",	 VAR_STRING), வெற்று, VV_RO}
};

// shorthand
#define vv_tv_type	vv_di.di_tv.v_type
#define vv_nr		vv_di.di_tv.vval.v_எண்
#define vv_float	vv_di.di_tv.vval.v_float
#define vv_str		vv_di.di_tv.vval.v_string
#define vv_list		vv_di.di_tv.vval.v_list
#define vv_dict		vv_di.di_tv.vval.v_dict
#define vv_blob		vv_di.di_tv.vval.v_blob
#define vv_tv		vv_di.di_tv

நிலையான dictitem_T	vimvars_var;		// variable used for v:
நிலையான dict_T		vimvardict;		// Dictionary with v: variables
#define vimvarht  vimvardict.dv_hashtab

// for VIM_VERSION_ defines
#அந்தர்பாவ "version.h"

நிலையான வெறுமை list_glob_vars(முழு *first);
நிலையான வெறுமை list_buf_vars(முழு *first);
நிலையான வெறுமை list_win_vars(முழு *first);
நிலையான வெறுமை list_tab_vars(முழு *first);
நிலையான char_u *list_arg_vars(exarg_T *eap, char_u *arg, முழு *first);
நிலையான char_u *ex_let_one(char_u *arg, typval_T *tv, முழு copy, முழு flags, char_u *endchars, char_u *op, முழு var_idx);
நிலையான முழு do_unlet_var(lval_T *lp, char_u *name_end, exarg_T *eap, முழு deep, வெறுமை *cookie);
நிலையான முழு do_lock_var(lval_T *lp, char_u *name_end, exarg_T *eap, முழு deep, வெறுமை *cookie);
நிலையான வெறுமை list_one_var(dictitem_T *v, எழுத்து *prefix, முழு *first);
நிலையான வெறுமை list_one_var_a(எழுத்து *prefix, char_u *name, முழு type, char_u *string, முழு *first);

/*
 * Initialize global and vim special variables
 */
    வெறுமை
evalvars_init(வெறுமை)
{
    முழு		    i;
    கட்டமைப்பு vimvar   *p;

    init_var_dict(&globvardict, &globvars_var, VAR_DEF_SCOPE);
    init_var_dict(&vimvardict, &vimvars_var, VAR_SCOPE);
    vimvardict.dv_lock = VAR_FIXED;
    hash_init(&compat_hashtab);

    க்கு (i = 0; i < VV_LEN; ++i)
    {
	p = &vimvars[i];
	எனில் (p->vv_flags & VV_RO)
	    p->vv_di.di_flags = DI_FLAGS_RO | DI_FLAGS_FIX;
	இல்லையெனில் எனில் (p->vv_flags & VV_RO_SBX)
	    p->vv_di.di_flags = DI_FLAGS_RO_SBX | DI_FLAGS_FIX;
	இல்லையெனில்
	    p->vv_di.di_flags = DI_FLAGS_FIX;

	// add to v: scope dict, unless the value is not always available
	எனில் (p->vv_tv_type != VAR_UNKNOWN)
	    hash_add(&vimvarht, p->vv_di.di_சாவி, "initialization");
	எனில் (p->vv_flags & VV_COMPAT)
	    // add to compat scope dict
	    hash_add(&compat_hashtab, p->vv_di.di_சாவி, "initialization");
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
    set_vim_var_nr(VV_NUMBERSIZE, அளவு(varnumber_T) * 8);
    set_vim_var_nr(VV_SIZEOFINT, அளவு(முழு));
    set_vim_var_nr(VV_SIZEOFLONG, அளவு(long));
    set_vim_var_nr(VV_SIZEOFPOINTER, அளவு(எழுத்து *));
    set_vim_var_nr(VV_MAXCOL, MAXCOL);

    set_vim_var_nr(VV_TYPE_எண்,  VAR_TYPE_எண்);
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

#எனில் defined(EXITFREE)
/*
 * Free all vim variables information on exit
 */
    வெறுமை
evalvars_clear(வெறுமை)
{
    முழு		    i;
    கட்டமைப்பு vimvar   *p;

    க்கு (i = 0; i < VV_LEN; ++i)
    {
	p = &vimvars[i];
	எனில் (p->vv_di.di_tv.v_type == VAR_STRING)
	    VIM_CLEAR(p->vv_str);
	இல்லையெனில் எனில் (p->vv_di.di_tv.v_type == VAR_LIST)
	{
	    list_unref(p->vv_list);
	    p->vv_list = வெற்று;
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
    க்கு (i = 1; i <= script_பொருட்கள்.ga_len; ++i)
	vars_clear(&SCRIPT_VARS(i));
}
#endif

    முழு
garbage_collect_globvars(முழு copyID)
{
    திரும்பு set_ref_in_ht(&globvarht, copyID, வெற்று, வெற்று);
}

    முழு
garbage_collect_vimvars(முழு copyID)
{
    திரும்பு set_ref_in_ht(&vimvarht, copyID, வெற்று, வெற்று);
}

    முழு
garbage_collect_scriptvars(முழு copyID)
{
    முழு		    i;
    முழு		    idx;
    முழு		    abort = FALSE;
    scriptitem_T    *si;

    க்கு (i = 1; i <= script_பொருட்கள்.ga_len; ++i)
    {
	abort = abort || set_ref_in_ht(&SCRIPT_VARS(i), copyID, வெற்று, வெற்று);

	si = SCRIPT_ITEM(i);
	க்கு (idx = 0; idx < si->sn_var_vals.ga_len; ++idx)
	{
	    svar_T    *sv = ((svar_T *)si->sn_var_vals.ga_data) + idx;

	    எனில் (sv->sv_name != வெற்று)
		abort = abort || set_ref_in_item(sv->sv_tv, copyID, வெற்று, வெற்று, வெற்று);
	}
    }

    திரும்பு abort;
}

/*
 * Set an internal variable to a string value. Creates the variable if it does
 * not already exist.
 */
    வெறுமை
set_internal_string_var(char_u *name, char_u *மதிப்பு)
{
    char_u	*val;
    typval_T	*tvp;

    val = vim_strsave(மதிப்பு);
    எனில் (val == வெற்று)
	திரும்பு;

    tvp = alloc_string_tv(val);
    எனில் (tvp == வெற்று)
	திரும்பு;

    set_var(name, tvp, FALSE);
    free_tv(tvp);
}

    முழு
eval_charconvert(
    char_u	*enc_from,
    char_u	*enc_to,
    char_u	*fname_from,
    char_u	*fname_to)
{
    முழு		err = FALSE;
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;

    set_vim_var_string(VV_CC_FROM, enc_from, -1);
    set_vim_var_string(VV_CC_TO, enc_to, -1);
    set_vim_var_string(VV_FNAME_IN, fname_from, -1);
    set_vim_var_string(VV_FNAME_OUT, fname_to, -1);
    ctx = get_option_sctx("charconvert");
    எனில் (ctx != வெற்று)
	current_sctx = *ctx;

    எனில் (eval_to_bool(p_ccv, &err, வெற்று, FALSE, TRUE))
	err = TRUE;

    set_vim_var_string(VV_CC_FROM, வெற்று, -1);
    set_vim_var_string(VV_CC_TO, வெற்று, -1);
    set_vim_var_string(VV_FNAME_IN, வெற்று, -1);
    set_vim_var_string(VV_FNAME_OUT, வெற்று, -1);
    current_sctx = saved_sctx;

    எனில் (err)
	திரும்பு FAIL;
    திரும்பு OK;
}

#எனில் defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
    முழு
eval_printexpr(char_u *fname, char_u *args)
{
    முழு		err = FALSE;
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;

    set_vim_var_string(VV_FNAME_IN, fname, -1);
    set_vim_var_string(VV_CMDARG, args, -1);
    ctx = get_option_sctx("printexpr");
    எனில் (ctx != வெற்று)
	current_sctx = *ctx;

    எனில் (eval_to_bool(p_pexpr, &err, வெற்று, FALSE, TRUE))
	err = TRUE;

    set_vim_var_string(VV_FNAME_IN, வெற்று, -1);
    set_vim_var_string(VV_CMDARG, வெற்று, -1);
    current_sctx = saved_sctx;

    எனில் (err)
    {
	mch_remove(fname);
	திரும்பு FAIL;
    }
    திரும்பு OK;
}
#endif

#எனில் defined(FEAT_DIFF)
    வெறுமை
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
    எனில் (ctx != வெற்று)
	current_sctx = *ctx;

    // errors are ignored
    tv = eval_கோவை_ext(p_dex, வெற்று, TRUE);
    free_tv(tv);

    set_vim_var_string(VV_FNAME_IN, வெற்று, -1);
    set_vim_var_string(VV_FNAME_NEW, வெற்று, -1);
    set_vim_var_string(VV_FNAME_OUT, வெற்று, -1);
    current_sctx = saved_sctx;
}

    வெறுமை
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
    எனில் (ctx != வெற்று)
	current_sctx = *ctx;

    // errors are ignored
    tv = eval_கோவை_ext(p_pex, வெற்று, TRUE);
    free_tv(tv);

    set_vim_var_string(VV_FNAME_IN, வெற்று, -1);
    set_vim_var_string(VV_FNAME_DIFF, வெற்று, -1);
    set_vim_var_string(VV_FNAME_OUT, வெற்று, -1);
    current_sctx = saved_sctx;
}
#endif

#எனில் defined(FEAT_SPELL)
/*
 * Evaluate an expression to a list with suggestions.
 * For the "expr:" part of 'spellsuggest'.
 * Returns NULL when there is an error.
 */
    list_T *
eval_spell_கோவை(char_u *badword, char_u *கோவை)
{
    typval_T	save_val;
    typval_T	rettv;
    list_T	*list = வெற்று;
    char_u	*p = skipwhite(கோவை);
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;
    முழு		r;

    // Set "v:val" to the bad word.
    prepare_vimvar(VV_VAL, &save_val);
    set_vim_var_string(VV_VAL, badword, -1);
    எனில் (p_verbose == 0)
	++emsg_off;
    ctx = get_option_sctx("spellsuggest");
    எனில் (ctx != வெற்று)
	current_sctx = *ctx;

    r = may_call_simple_func(p, &rettv);
    எனில் (r == NOTDONE)
	r = eval1(&p, &rettv, &EVALARG_EVALUATE);
    எனில் (r == OK)
    {
	எனில் (rettv.v_type != VAR_LIST)
	    clear_tv(&rettv);
	இல்லையெனில்
	    list = rettv.vval.v_list;
    }

    எனில் (p_verbose == 0)
	--emsg_off;
    clear_tv(get_vim_var_tv(VV_VAL));
    restore_vimvar(VV_VAL, &save_val);
    current_sctx = saved_sctx;

    திரும்பு list;
}

/*
 * "list" is supposed to contain two items: a word and a number.  Return the
 * word in "pp" and the number as the return value.
 * Return -1 if anything isn't right.
 * Used to get the good word and score from the eval_spell_expr() result.
 */
    முழு
get_spellword(list_T *list, char_u **pp)
{
    listitem_T	*li;

    li = list->lv_first;
    எனில் (li == வெற்று)
	திரும்பு -1;
    *pp = tv_get_string(&li->li_tv);

    li = li->li_next;
    எனில் (li == வெற்று)
	திரும்பு -1;
    திரும்பு (முழு)tv_get_எண்(&li->li_tv);
}
#endif

/*
 * Prepare v: variable "idx" to be used.
 * Save the current typeval in "save_tv" and clear it.
 * When not used yet add the variable to the v: hashtable.
 */
    வெறுமை
prepare_vimvar(முழு idx, typval_T *save_tv)
{
    *save_tv = vimvars[idx].vv_tv;
    vimvars[idx].vv_str = வெற்று;  // don't free it now
    எனில் (vimvars[idx].vv_tv_type == VAR_UNKNOWN)
	hash_add(&vimvarht, vimvars[idx].vv_di.di_சாவி, "prepare vimvar");
}

/*
 * Restore v: variable "idx" to typeval "save_tv".
 * Note that the v: variable must have been cleared already.
 * When no longer defined, remove the variable from the v: hashtable.
 */
    வெறுமை
restore_vimvar(முழு idx, typval_T *save_tv)
{
    hashitem_T	*hi;

    vimvars[idx].vv_tv = *save_tv;
    எனில் (vimvars[idx].vv_tv_type != VAR_UNKNOWN)
	திரும்பு;

    hi = hash_find(&vimvarht, vimvars[idx].vv_di.di_சாவி);
    எனில் (HASHITEM_EMPTY(hi))
	internal_error("restore_vimvar()");
    இல்லையெனில்
	hash_remove(&vimvarht, hi, "restore vimvar");
}

/*
 * List Vim variables.
 */
    நிலையான வெறுமை
list_vim_vars(முழு *first)
{
    list_hashtable_vars(&vimvarht, "v:", FALSE, first);
}

/*
 * List script-local variables, if there is a script.
 */
    நிலையான வெறுமை
list_script_vars(முழு *first)
{
    எனில் (SCRIPT_ID_VALID(current_sctx.sc_sid))
	list_hashtable_vars(&SCRIPT_VARS(current_sctx.sc_sid),
							   "s:", FALSE, first);
}

/*
 * Return TRUE if "name" starts with "g:", "w:", "t:" or "b:".
 * But only when an identifier character follows.
 */
    முழு
is_scoped_variable(char_u *name)
{
    திரும்பு vim_strchr((char_u *)"gwbt", name[0]) != வெற்று
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
eval_one_கோவை_in_str(char_u *p, garray_T *gap, முழு evaluate)
{
    char_u	*block_start = skipwhite(p + 1);  // skip the opening {
    char_u	*block_end = block_start;
    char_u	*கோவை_val;

    எனில் (*block_start == NUL)
    {
	semsg(_(e_missing_close_curly_str), p);
	திரும்பு வெற்று;
    }
    எனில் (skip_கோவை(&block_end, வெற்று) == FAIL)
	திரும்பு வெற்று;
    block_end = skipwhite(block_end);
    எனில் (*block_end != '}')
    {
	semsg(_(e_missing_close_curly_str), p);
	திரும்பு வெற்று;
    }
    எனில் (evaluate)
    {
	*block_end = NUL;
	கோவை_val = eval_to_string(block_start, FALSE, FALSE);
	*block_end = '}';
	எனில் (கோவை_val == வெற்று)
	    திரும்பு வெற்று;
	ga_concat(gap, கோவை_val);
	vim_free(கோவை_val);
    }

    திரும்பு block_end + 1;
}

/*
 * Evaluate all the Vim expressions {expr} in "str" and return the resulting
 * string in allocated memory.  "{{" is reduced to "{" and "}}" to "}".
 * Used for a heredoc assignment.
 * Returns NULL for an error.
 */
    நிலையான char_u *
eval_all_கோவை_in_str(char_u *str)
{
    garray_T	ga;
    char_u	*p;

    ga_init2(&ga, 1, 80);
    p = str;

    போது (*p != NUL)
    {
	char_u	*lit_start;
	முழு	escaped_brace = FALSE;

	// Look for a block start.
	lit_start = p;
	போது (*p != '{' && *p != '}' && *p != NUL)
	    ++p;

	எனில் (*p != NUL && *p == p[1])
	{
	    // Escaped brace, unescape and continue.
	    // Include the brace in the literal string.
	    ++p;
	    escaped_brace = TRUE;
	}
	இல்லையெனில் எனில் (*p == '}')
	{
	    semsg(_(e_stray_closing_curly_str), str);
	    ga_clear(&ga);
	    திரும்பு வெற்று;
	}

	// Append the literal part.
	ga_concat_len(&ga, lit_start, (size_t)(p - lit_start));

	எனில் (*p == NUL)
	    நிறுத்து;

	எனில் (escaped_brace)
	{
	    // Skip the second brace.
	    ++p;
	    தொடர்;
	}

	// Evaluate the expression and append the result.
	p = eval_one_கோவை_in_str(p, &ga, TRUE);
	எனில் (p == வெற்று)
	{
	    ga_clear(&ga);
	    திரும்பு வெற்று;
	}
    }
    ga_append(&ga, NUL);

    திரும்பு ga.ga_data;
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
heredoc_get(exarg_T *eap, char_u *cmd, முழு script_get, முழு vim9compile)
{
    char_u	*theline = வெற்று;
    char_u	*marker;
    list_T	*l;
    char_u	*p;
    char_u	*str;
    முழு		marker_indent_len = 0;
    முழு		text_indent_len = 0;
    char_u	*text_indent = வெற்று;
    char_u	dot[] = ".";
    முழு		comment_char = in_vim9script() ? '#' : '"';
    முழு		evalstr = FALSE;
    முழு		eval_failed = FALSE;
    cctx_T	*cctx = vim9compile ? eap->cookie : வெற்று;
    முழு		மொத்தம் = 0;
    முழு		heredoc_in_string = FALSE;
    char_u	*வரி_arg = வெற்று;
    char_u	*nl_ptr = vim_strchr(cmd, '\n');

    எனில் (nl_ptr != வெற்று)
    {
	heredoc_in_string = TRUE;
	வரி_arg = nl_ptr + 1;
	*nl_ptr = NUL;
    }
    இல்லையெனில் எனில் (eap->ea_getline == வெற்று)
    {
	emsg(_(e_cannot_use_heredoc_here));
	திரும்பு வெற்று;
    }

    // Check for the optional 'trim' word before the marker
    cmd = skipwhite(cmd);

    போது (TRUE)
    {
	எனில் (STRNCMP(cmd, "trim", 4) == 0
		&& (cmd[4] == NUL || VIM_ISWHITE(cmd[4])))
	{
	    cmd = skipwhite(cmd + 4);

	    // Trim the indentation from all the lines in the here document.
	    // The amount of indentation trimmed is the same as the indentation
	    // of the first line after the :let command line.  To find the end
	    // marker the indent of the :let command line is trimmed.
	    p = *eap->cmdlinep;
	    போது (VIM_ISWHITE(*p))
	    {
		p++;
		marker_indent_len++;
	    }
	    text_indent_len = -1;

	    தொடர்;
	}
	எனில் (STRNCMP(cmd, "eval", 4) == 0
		&& (cmd[4] == NUL || VIM_ISWHITE(cmd[4])))
	{
	    cmd = skipwhite(cmd + 4);
	    evalstr = TRUE;
	    தொடர்;
	}
	நிறுத்து;
    }

    // The marker is the next word.
    எனில் (*cmd != NUL && *cmd != comment_char)
    {
	marker = skipwhite(cmd);
	p = skiptowhite(marker);
	எனில் (*skipwhite(p) != NUL && *skipwhite(p) != comment_char)
	{
	    semsg(_(e_trailing_characters_str), p);
	    திரும்பு வெற்று;
	}
	*p = NUL;
	எனில் (!script_get && vim_islower(*marker))
	{
	    emsg(_(e_marker_cannot_start_with_lower_case_letter));
	    திரும்பு வெற்று;
	}
    }
    இல்லையெனில்
    {
	// When getting lines for an embedded script, if the marker is missing,
	// accept '.' as the marker.
	எனில் (script_get)
	    marker = dot;
	இல்லையெனில்
	{
	    emsg(_(e_missing_marker));
	    திரும்பு வெற்று;
	}
    }

    l = list_alloc();
    எனில் (l == வெற்று)
	திரும்பு வெற்று;

    க்கு (;;)
    {
	முழு	mi = 0;
	முழு	ti = 0;

	எனில் (heredoc_in_string)
	{
	    char_u	*next_வரி;

	    // heredoc in a string separated by newlines.  Get the next line
	    // from the string.

	    எனில் (*வரி_arg == NUL)
	    {
		semsg(_(e_missing_end_marker_str), marker);
		நிறுத்து;
	    }

	    theline = வரி_arg;
	    next_வரி = vim_strchr(theline, '\n');
	    எனில் (next_வரி == வெற்று)
		வரி_arg += STRLEN(வரி_arg);
	    இல்லையெனில்
	    {
		*next_வரி = NUL;
		வரி_arg = next_வரி + 1;
	    }
	}
	இல்லையெனில்
	{
	    vim_free(theline);
	    theline = eap->ea_getline(NUL, eap->cookie, 0, FALSE);
	    எனில் (theline == வெற்று)
	    {
		semsg(_(e_missing_end_marker_str), marker);
		நிறுத்து;
	    }
	}

	// with "trim": skip the indent matching the :let line to find the
	// marker
	எனில் (marker_indent_len > 0
		&& STRNCMP(theline, *eap->cmdlinep, marker_indent_len) == 0)
	    mi = marker_indent_len;
	எனில் (STRCMP(marker, theline + mi) == 0)
	    நிறுத்து;

	// If expression evaluation failed in the heredoc, then skip till the
	// end marker.
	எனில் (eval_failed)
	    தொடர்;

	எனில் (text_indent_len == -1 && *theline != NUL)
	{
	    // set the text indent from the first line.
	    p = theline;
	    text_indent_len = 0;
	    போது (VIM_ISWHITE(*p))
	    {
		p++;
		text_indent_len++;
	    }
	    text_indent = vim_strnsave(theline, text_indent_len);
	}
	// with "trim": skip the indent matching the first line
	எனில் (text_indent != வெற்று)
	    க்கு (ti = 0; ti < text_indent_len; ++ti)
		எனில் (theline[ti] != text_indent[ti])
		    நிறுத்து;

	str = theline + ti;
	எனில் (vim9compile)
	{
	    எனில் (compile_all_கோவை_in_str(str, evalstr, cctx) == FAIL)
	    {
		vim_free(theline);
		vim_free(text_indent);
		list_free(l);
		திரும்பு FAIL;
	    }
	    மொத்தம்++;
	}
	இல்லையெனில்
	{
	    முழு	    free_str = FALSE;

	    எனில் (evalstr && !eap->skip)
	    {
		str = eval_all_கோவை_in_str(str);
		எனில் (str == வெற்று)
		{
		    // expression evaluation failed
		    eval_failed = TRUE;
		    தொடர்;
		}
		free_str = TRUE;
	    }

	    எனில் (list_append_string(l, str, -1) == FAIL)
	    {
		எனில் (free_str)
		    vim_free(str);
		நிறுத்து;
	    }
	    எனில் (free_str)
		vim_free(str);
	}
    }
    எனில் (heredoc_in_string)
	// Next command follows the heredoc in the string.
	eap->nextcmd = வரி_arg;
    இல்லையெனில்
	vim_free(theline);
    vim_free(text_indent);

    எனில் (vim9compile && cctx->ctx_skip != SKIP_YES && !eval_failed)
	generate_NEWLIST(cctx, மொத்தம், FALSE);

    எனில் (eval_failed)
    {
	// expression evaluation in the heredoc failed
	list_free(l);
	திரும்பு வெற்று;
    }
    திரும்பு l;
}

/*
 * Vim9 variable declaration:
 * ":var name"
 * ":var name: type"
 * ":var name = expr"
 * ":var name: type = expr"
 * etc.
 */
    வெறுமை
ex_var(exarg_T *eap)
{
    char_u *p = eap->cmd;
    முழு	    has_var;

    எனில் (!in_vim9script())
    {
	semsg(_(e_str_cannot_be_used_in_legacy_vim_script), ":var");
	திரும்பு;
    }
    has_var = checkforcmd_noparen(&p, "var", 3);
    எனில் (current_sctx.sc_sid == 0 && has_var)
    {
	emsg(_(e_cannot_declare_variable_on_command_வரி));
	திரும்பு;
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
    வெறுமை
ex_let(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    char_u	*கோவை = வெற்று;
    typval_T	rettv;
    முழு		var_மொத்தம் = 0;
    முழு		semicolon = 0;
    char_u	op[4];
    char_u	*argend;
    முழு		first = TRUE;
    முழு		concat;
    முழு		has_assign;
    முழு		flags = 0;
    முழு		vim9script = in_vim9script();

    எனில் (eap->cmdidx == CMD_final && !vim9script)
    {
	// In legacy Vim script ":final" is short for ":finally".
	ex_finally(eap);
	திரும்பு;
    }
    எனில் (eap->cmdidx == CMD_let && vim9script)
    {
	emsg(_(e_cannot_use_let_in_vim9_script));
	திரும்பு;
    }

    எனில் (eap->cmdidx == CMD_const)
	flags |= ASSIGN_CONST;
    இல்லையெனில் எனில் (eap->cmdidx == CMD_final)
	flags |= ASSIGN_FINAL;

    // Vim9 assignment without ":let", ":const" or ":final"
    எனில் (eap->arg == eap->cmd)
	flags |= ASSIGN_NO_DECL;

    argend = skip_var_list(arg, TRUE, &var_மொத்தம், &semicolon, FALSE);
    எனில் (argend == வெற்று)
	திரும்பு;
    கோவை = skipwhite(argend);
    concat = கோவை[0] == '.'
	&& ((கோவை[1] == '=' && in_old_script(2))
		|| (கோவை[1] == '.' && கோவை[2] == '='));
    has_assign =  *கோவை == '=' || (vim_strchr((char_u *)"+-*/%", *கோவை) != வெற்று
							    && கோவை[1] == '=');
    எனில் (!has_assign && !concat)
    {
	// ":let" without "=": list variables
	எனில் (*arg == '[')
	    emsg(_(e_invalid_argument));
	இல்லையெனில் எனில் (கோவை[0] == '.' && கோவை[1] == '=')
	    emsg(_(e_dot_equal_not_supported_with_script_version_two));
	இல்லையெனில் எனில் (!ends_excmd2(eap->cmd, arg))
	{
	    எனில் (vim9script)
	    {
		எனில் (!ends_excmd2(eap->cmd, skipwhite(argend)))
		    semsg(_(e_trailing_characters_str), argend);
		இல்லையெனில்
		    // Vim9 declaration ":var name: type"
		    arg = vim9_declare_scriptvar(eap, arg);
	    }
	    இல்லையெனில்
	    {
		// ":let var1 var2" - list values
		arg = list_arg_vars(eap, arg, &first);
	    }
	}
	இல்லையெனில் எனில் (!eap->skip)
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
	திரும்பு;
    }

    எனில் (கோவை[0] == '=' && கோவை[1] == '<' && கோவை[2] == '<')
    {
	list_T	*l = வெற்று;
	long	cur_lnum = SOURCING_LNUM;

	// :let text =<< [trim] [eval] END
	// :var text =<< [trim] [eval] END
	எனில் (vim9script && !eap->skip && (!VIM_ISWHITE(கோவை[-1])
						 || !IS_WHITE_OR_NUL(கோவை[3])))
	    semsg(_(e_white_space_required_before_and_after_str_at_str),
								  "=<<", கோவை);
	இல்லையெனில்
	    l = heredoc_get(eap, கோவை + 3, FALSE, FALSE);

	எனில் (l != வெற்று)
	{
	    rettv_list_set(&rettv, l);
	    எனில் (!eap->skip)
	    {
		// errors are for the assignment, not the end marker
		SOURCING_LNUM = cur_lnum;
		op[0] = '=';
		op[1] = NUL;
		(வெறுமை)ex_let_vars(eap->arg, &rettv, FALSE, semicolon, var_மொத்தம்,
								flags, op);
	    }
	    clear_tv(&rettv);
	}
	திரும்பு;
    }

    evalarg_T   evalarg;
    முழு	    len = 1;

    CLEAR_FIELD(rettv);

    முழு cur_lnum;

    op[0] = '=';
    op[1] = NUL;
    எனில் (*கோவை != '=')
    {
	எனில் (vim9script && (flags & ASSIGN_NO_DECL) == 0)
	{
	    // +=, /=, etc. require an existing variable
	    semsg(_(e_cannot_use_operator_on_new_variable_str), eap->arg);
	}
	இல்லையெனில் எனில் (vim_strchr((char_u *)"+-*/%.", *கோவை) != வெற்று)
	{
	    op[0] = *கோவை;   // +=, -=, *=, /=, %= or .=
	    ++len;
	    எனில் (கோவை[0] == '.' && கோவை[1] == '.') // ..=
	    {
		++கோவை;
		++len;
	    }
	}
	கோவை += 2;
    }
    இல்லையெனில்
	++கோவை;

    எனில் (vim9script && !eap->skip && (!VIM_ISWHITE(*argend)
					       || !IS_WHITE_OR_NUL(*கோவை)))
    {
	vim_strncpy(op, கோவை - len, len);
	semsg(_(e_white_space_required_before_and_after_str_at_str),
							       op, argend);
    }

    எனில் (eap->skip)
	++emsg_skip;
    fill_evalarg_from_eap(&evalarg, eap, eap->skip);
    கோவை = skipwhite_and_linebreak(கோவை, &evalarg);
    cur_lnum = SOURCING_LNUM;
    முழு eval_res = eval0(கோவை, &rettv, eap, &evalarg);
    எனில் (eap->skip)
	--emsg_skip;
    clear_evalarg(&evalarg, eap);

    // Restore the line number so that any type error is given for the
    // declaration, not the expression.
    SOURCING_LNUM = cur_lnum;

    எனில் (!eap->skip && eval_res != FAIL)
	(வெறுமை)ex_let_vars(eap->arg, &rettv, FALSE, semicolon, var_மொத்தம்,
								    flags, op);
    எனில் (eval_res != FAIL)
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
    முழு
ex_let_vars(
    char_u	*arg_start,
    typval_T	*tv,
    முழு		copy,		// copy values from "tv", don't move
    முழு		semicolon,	// from skip_var_list()
    முழு		var_மொத்தம்,	// from skip_var_list()
    முழு		flags,		// ASSIGN_FINAL, ASSIGN_CONST, etc.
    char_u	*op)
{
    char_u	*arg = arg_start;
    list_T	*l;
    tuple_T	*tuple = வெற்று;
    முழு		i;
    முழு		var_idx = 0;
    listitem_T	*item = வெற்று;
    typval_T	ltv;
    முழு		is_list = tv->v_type == VAR_LIST;
    முழு		idx;

    எனில் (tv->v_type == VAR_VOID)
    {
	emsg(_(e_cannot_use_void_மதிப்பு));
	திரும்பு FAIL;
    }
    எனில் (*arg != '[')
    {
	// ":let var = expr" or ":for var in list"
	எனில் (ex_let_one(arg, tv, copy, flags, op, op, var_idx) == வெற்று)
	    திரும்பு FAIL;
	திரும்பு OK;
    }

    // ":let [v1, v2] = list" or ":for [v1, v2] in listlist"
    // or
    // ":let [v1, v2] = tuple" or ":for [v1, v2] in tupletuple"
    எனில் (tv->v_type != VAR_LIST && tv->v_type != VAR_TUPLE)
    {
	emsg(_(e_list_or_tuple_required));
	திரும்பு FAIL;
    }
    எனில் (is_list)
    {
	l = tv->vval.v_list;
	எனில் (l == வெற்று)
	{
	    emsg(_(e_list_required));
	    திரும்பு FAIL;
	}
	i = list_len(l);
    }
    இல்லையெனில்
    {
	tuple = tv->vval.v_tuple;
	எனில் (tuple == வெற்று)
	{
	    emsg(_(e_tuple_required));
	    திரும்பு FAIL;
	}
	i = tuple_len(tuple);
    }

    எனில் (semicolon == 0 && var_மொத்தம் < i)
    {
	emsg(_(is_list ? e_less_targets_than_list_பொருட்கள்
					: e_less_targets_than_tuple_பொருட்கள்));
	திரும்பு FAIL;
    }
    எனில் (var_மொத்தம் - semicolon > i)
    {
	emsg(_(is_list ? e_more_targets_than_list_பொருட்கள்
					: e_more_targets_than_tuple_பொருட்கள்));
	திரும்பு FAIL;
    }

    எனில் (is_list)
    {
	CHECK_LIST_MATERIALIZE(l);
	item = l->lv_first;
    }
    இல்லையெனில்
	idx = 0;

    போது (*arg != ']')
    {
	arg = skipwhite(arg + 1);
	++var_idx;
	arg = ex_let_one(arg, is_list ? &item->li_tv : TUPLE_ITEM(tuple, idx),
			 TRUE, flags | ASSIGN_UNPACK, (char_u *)",;]", op,
			 var_idx);
	எனில் (is_list)
	    item = item->li_next;
	இல்லையெனில்
	    idx++;
	எனில் (arg == வெற்று)
	    திரும்பு FAIL;

	arg = skipwhite(arg);
	எனில் (*arg == ';')
	{
	    // Put the rest of the list or tuple (may be empty) in the var
	    // after ';'.  Create a new list or tuple for this.
	    எனில் (is_list)
	    {
		// Put the rest of the list (may be empty) in the var
		// after ';'.  Create a new list for this.
		l = list_alloc();
		எனில் (l == வெற்று)
		    திரும்பு FAIL;

		// list
		போது (item != வெற்று)
		{
		    list_append_tv(l, &item->li_tv);
		    item = item->li_next;
		}

		ltv.v_type = VAR_LIST;
		ltv.v_lock = 0;
		ltv.vval.v_list = l;
		l->lv_refcount = 1;
	    }
	    இல்லையெனில்
	    {
		tuple_T	*new_tuple = tuple_alloc();
		எனில் (new_tuple == வெற்று)
		    திரும்பு FAIL;

		// Put the rest of the tuple (may be empty) in the var
		// after ';'.  Create a new tuple for this.
		போது (idx < TUPLE_LEN(tuple))
		{
		    typval_T    new_tv;

		    copy_tv(TUPLE_ITEM(tuple, idx), &new_tv);
		    எனில் (tuple_append_tv(new_tuple, &new_tv) == FAIL)
		    {
			clear_tv(&new_tv);
			tuple_unref(new_tuple);
			திரும்பு FAIL;
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
	    எனில் (arg == வெற்று)
		திரும்பு FAIL;
	    நிறுத்து;
	}
	இல்லையெனில் எனில் (*arg != ',' && *arg != ']')
	{
	    internal_error("ex_let_vars()");
	    திரும்பு FAIL;
	}
    }

    திரும்பு OK;
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
    முழு		include_type,
    முழு		*var_மொத்தம்,
    முழு		*semicolon,
    முழு		silent)
{
    char_u	*p, *s;

    எனில் (*arg == '[')
    {
	// "[var, var]": find the matching ']'.
	p = arg;
	க்கு (;;)
	{
	    p = skipwhite(p + 1);	// skip whites after '[', ';' or ','
	    s = skip_var_one(p, include_type);
	    எனில் (s == p)
	    {
		எனில் (!silent)
		    semsg(_(e_invalid_argument_str), p);
		திரும்பு வெற்று;
	    }
	    ++*var_மொத்தம்;

	    p = skipwhite(s);
	    எனில் (*p == ']')
		நிறுத்து;
	    இல்லையெனில் எனில் (*p == ';')
	    {
		எனில் (*semicolon == 1)
		{
		    எனில் (!silent)
			emsg(_(e_double_semicolon_in_list_of_variables));
		    திரும்பு வெற்று;
		}
		*semicolon = 1;
	    }
	    இல்லையெனில் எனில் (*p != ',')
	    {
		எனில் (!silent)
		    semsg(_(e_invalid_argument_str), p);
		திரும்பு வெற்று;
	    }
	}
	திரும்பு p + 1;
    }

    திரும்பு skip_var_one(arg, include_type);
}

/*
 * Skip one (assignable) variable name, including @r, $VAR, &option, d.key,
 * l[idx].
 * In Vim9 script also skip over ": type" if "include_type" is TRUE.
 */
    char_u *
skip_var_one(char_u *arg, முழு include_type)
{
    char_u	*end;
    முழு		vim9 = in_vim9script();

    எனில் (*arg == '@' && arg[1] != NUL)
	திரும்பு arg + 2;

    // termcap option name may have non-alpha characters
    எனில் (STRNCMP(arg, "&t_", 3) == 0 && arg[3] != NUL && arg[4] != NUL)
	திரும்பு arg + 5;

    end = find_name_end(*arg == '$' || *arg == '&' ? arg + 1 : arg,
				   வெற்று, வெற்று, FNE_INCL_BR | FNE_CHECK_START);

    // "a: type" is declaring variable "a" with a type, not "a:".
    // Same for "s: type".
    எனில் (vim9 && end == arg + 2 && end[-1] == ':')
	--end;

    எனில் (include_type && vim9)
    {
	எனில் (*skipwhite(end) == ':')
	    end = skip_type(skipwhite(skipwhite(end) + 1), FALSE);
    }
    திரும்பு end;
}

/*
 * List variables for hashtab "ht" with prefix "prefix".
 * If "empty" is TRUE also list NULL strings as empty strings.
 */
    வெறுமை
list_hashtable_vars(
    hashtab_T	*ht,
    எழுத்து	*prefix,
    முழு		empty,
    முழு		*first)
{
    hashitem_T	*hi;
    dictitem_T	*di;
    முழு		todo;
    char_u	buf[IOSIZE];

    முழு save_ht_flags = ht->ht_flags;
    ht->ht_flags |= HTFLAGS_FROZEN;

    todo = (முழு)ht->ht_used;
    க்கு (hi = ht->ht_array; todo > 0 && !got_int; ++hi)
    {
	எனில் (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    di = HI2DI(hi);

	    // apply :filter /pat/ to variable name
	    vim_strncpy((char_u *)buf, (char_u *)prefix, IOSIZE - 1);
	    vim_strcat((char_u *)buf, di->di_சாவி, IOSIZE);
	    எனில் (message_filtered(buf))
		தொடர்;

	    எனில் (empty || di->di_tv.v_type != VAR_STRING
					   || di->di_tv.vval.v_string != வெற்று)
		list_one_var(di, prefix, first);
	}
    }

    ht->ht_flags = save_ht_flags;
}

/*
 * List global variables.
 */
    நிலையான வெறுமை
list_glob_vars(முழு *first)
{
    list_hashtable_vars(&globvarht, "", TRUE, first);
}

/*
 * List buffer variables.
 */
    நிலையான வெறுமை
list_buf_vars(முழு *first)
{
    list_hashtable_vars(&curbuf->b_vars->dv_hashtab, "b:", TRUE, first);
}

/*
 * List window variables.
 */
    நிலையான வெறுமை
list_win_vars(முழு *first)
{
    list_hashtable_vars(&curwin->w_vars->dv_hashtab, "w:", TRUE, first);
}

/*
 * List tab page variables.
 */
    நிலையான வெறுமை
list_tab_vars(முழு *first)
{
    list_hashtable_vars(&curtab->tp_vars->dv_hashtab, "t:", TRUE, first);
}

/*
 * List variables in "arg".
 */
    நிலையான char_u *
list_arg_vars(exarg_T *eap, char_u *arg, முழு *first)
{
    முழு		error = FALSE;
    முழு		len;
    char_u	*name;
    char_u	*name_start;
    char_u	*arg_subsc;
    char_u	*tofree;
    typval_T    tv;

    போது (!ends_excmd2(eap->cmd, arg) && !got_int)
    {
	எனில் (error || eap->skip)
	{
	    arg = find_name_end(arg, வெற்று, வெற்று, FNE_INCL_BR | FNE_CHECK_START);
	    எனில் (!VIM_ISWHITE(*arg) && !ends_excmd(*arg))
	    {
		emsg_severe = TRUE;
		எனில் (!did_emsg)
		    semsg(_(e_trailing_characters_str), arg);
		நிறுத்து;
	    }
	}
	இல்லையெனில்
	{
	    // get_name_len() takes care of expanding curly braces
	    name_start = name = arg;
	    len = get_name_len(&arg, &tofree, TRUE, TRUE);
	    எனில் (len <= 0)
	    {
		// This is mainly to keep test 49 working: when expanding
		// curly braces fails overrule the exception error message.
		எனில் (len < 0 && !aborting())
		{
		    emsg_severe = TRUE;
		    semsg(_(e_invalid_argument_str), arg);
		    நிறுத்து;
		}
		error = TRUE;
	    }
	    இல்லையெனில்
	    {
		arg = skipwhite(arg);
		எனில் (tofree != வெற்று)
		    name = tofree;
		எனில் (eval_variable(name, len, 0, &tv, வெற்று,
						     EVAL_VAR_VERBOSE) == FAIL)
		    error = TRUE;
		இல்லையெனில்
		{
		    // handle d.key, l[idx], f(expr)
		    arg_subsc = arg;
		    எனில் (handle_subscript(&arg, name_start, &tv,
					      &EVALARG_EVALUATE, TRUE) == FAIL)
			error = TRUE;
		    இல்லையெனில்
		    {
			எனில் (arg == arg_subsc && len == 2 && name[1] == ':')
			{
			    தேர்வு (*name)
			    {
				நிலை 'g': list_glob_vars(first); நிறுத்து;
				நிலை 'b': list_buf_vars(first); நிறுத்து;
				நிலை 'w': list_win_vars(first); நிறுத்து;
				நிலை 't': list_tab_vars(first); நிறுத்து;
				நிலை 'v': list_vim_vars(first); நிறுத்து;
				நிலை 's': list_script_vars(first); நிறுத்து;
				நிலை 'l': list_func_vars(first); நிறுத்து;
				இயல்பு:
					  semsg(_(e_cant_list_variables_for_str), name);
			    }
			}
			இல்லையெனில்
			{
			    char_u	numbuf[NUMBUFLEN];
			    char_u	*tf;
			    முழு		c;
			    char_u	*s;

			    s = echo_string(&tv, &tf, numbuf, 0);
			    c = *arg;
			    *arg = NUL;
			    list_one_var_a("",
				    arg == arg_subsc ? name : name_start,
				    tv.v_type,
				    s == வெற்று ? (char_u *)"" : s,
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

    திரும்பு arg;
}

/*
 * Set an environment variable, part of ex_let_one().
 */
    நிலையான char_u *
ex_let_env(
    char_u	*arg,
    typval_T	*tv,
    முழு		flags,
    char_u	*endchars,
    char_u	*op)
{
    char_u	*arg_end = வெற்று;
    char_u	*name;
    முழு		len;

    எனில் ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					 && (flags & ASSIGN_FOR_LOOP) == 0)
    {
	emsg(_(e_cannot_lock_environment_variable));
	திரும்பு வெற்று;
    }

    // Find the end of the name.
    ++arg;
    name = arg;
    len = get_env_len(&arg);
    எனில் (len == 0)
	semsg(_(e_invalid_argument_str), name - 1);
    இல்லையெனில்
    {
	எனில் (op != வெற்று && vim_strchr((char_u *)"+-*/%", *op) != வெற்று)
	    semsg(_(e_wrong_variable_type_for_str_equal), op);
	இல்லையெனில் எனில் (endchars != வெற்று
			      && vim_strchr(endchars, *skipwhite(arg)) == வெற்று)
	    emsg(_(e_unexpected_characters_in_let));
	இல்லையெனில் எனில் (!check_secure() && !check_restricted())
	{
	    char_u	*tofree = வெற்று;
	    முழு		c1 = name[len];
	    char_u	*p;

	    name[len] = NUL;
	    p = tv_get_string_chk(tv);
	    எனில் (p != வெற்று && op != வெற்று && *op == '.')
	    {
		முழு	mustfree = FALSE;
		char_u  *s = vim_getenv(name, &mustfree);

		எனில் (s != வெற்று)
		{
		    p = tofree = concat_str(s, p);
		    எனில் (mustfree)
			vim_free(s);
		}
	    }
	    எனில் (p != வெற்று)
	    {
		vim_setenv_ext(name, p);
		arg_end = arg;
	    }
	    name[len] = c1;
	    vim_free(tofree);
	}
    }
    திரும்பு arg_end;
}

/*
 * Set an option, part of ex_let_one().
 */
    நிலையான char_u *
ex_let_option(
    char_u	*arg,
    typval_T	*tv,
    முழு		flags,
    char_u	*endchars,
    char_u	*op)
{
    char_u	*p;
    முழு		scope;
    char_u	*arg_end = வெற்று;

    எனில் ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					 && (flags & ASSIGN_FOR_LOOP) == 0)
    {
	emsg(_(e_cannot_lock_option));
	திரும்பு வெற்று;
    }

    // Find the end of the name.
    p = find_option_end(&arg, &scope);
    எனில் (p == வெற்று || (endchars != வெற்று
			       && vim_strchr(endchars, *skipwhite(p)) == வெற்று))
    {
	emsg(_(e_unexpected_characters_in_let));
	திரும்பு வெற்று;
    }

    முழு		c1;
    long	n = 0;
    getoption_T	opt_type;
    long	numval;
    char_u	*stringval = வெற்று;
    char_u	*s = வெற்று;
    முழு		failed = FALSE;
    முழு		opt_p_flags;
    char_u	*tofree = வெற்று;
    char_u	numbuf[NUMBUFLEN];

    c1 = *p;
    *p = NUL;

    opt_type = get_option_மதிப்பு(arg, &numval, &stringval, &opt_p_flags, scope);
    எனில் (opt_type == gov_unknown && arg[0] != 't' && arg[1] != '_')
    {
	semsg(_(e_unknown_option_str_2), arg);
	செல் theend;
    }
    எனில் (op != வெற்று && *op != '='
	&& (((opt_type == gov_bool || opt_type == gov_எண்) && *op == '.')
	    || (opt_type == gov_string && *op != '.')))
    {
	semsg(_(e_wrong_variable_type_for_str_equal), op);
	செல் theend;
    }

    எனில் ((opt_type == gov_bool
		|| opt_type == gov_எண்
		|| opt_type == gov_hidden_bool
		|| opt_type == gov_hidden_எண்)
		     && (tv->v_type != VAR_STRING || !in_vim9script()))
    {
	எனில் (opt_type == gov_bool || opt_type == gov_hidden_bool)
	    // bool, possibly hidden
	    n = (long)tv_get_bool_chk(tv, &failed);
	இல்லையெனில்
	    // number, possibly hidden
	    n = (long)tv_get_எண்_chk(tv, &failed);
	எனில் (failed)
	    செல் theend;
    }

    எனில் ((opt_p_flags & P_FUNC) && (tv->v_type == VAR_PARTIAL
						    || tv->v_type == VAR_FUNC))
    {
	// If the option can be set to a function reference or a lambda
	// and the passed value is a function reference, then convert it to
	// the name (string) of the function reference.
	s = tv2string(tv, &tofree, numbuf, 0);
	எனில் (s == வெற்று)
	    செல் theend;
    }
    // Avoid setting a string option to the text "v:false" or similar.
    // In Vim9 script also don't convert a number to string.
    இல்லையெனில் எனில் (tv->v_type != VAR_BOOL && tv->v_type != VAR_SPECIAL
		     && (!in_vim9script() || tv->v_type != VAR_எண்))
    {
	s = tv_get_string_chk(tv);
	எனில் (s == வெற்று)
	    செல் theend;
    }
    இல்லையெனில் எனில் (opt_type == gov_string || opt_type == gov_hidden_string)
    {
	emsg(_(e_string_required));
	செல் theend;
    }

    எனில் (op != வெற்று && *op != '=')
    {
	// number, in legacy script also bool
	எனில் (opt_type == gov_எண்
				 || (opt_type == gov_bool && !in_vim9script()))
	{
	    தேர்வு (*op)
	    {
		நிலை '+': n = numval + n; நிறுத்து;
		நிலை '-': n = numval - n; நிறுத்து;
		நிலை '*': n = numval * n; நிறுத்து;
		நிலை '/': n = (long)இலக்கம்_divide(numval, n, &failed); நிறுத்து;
		நிலை '%': n = (long)இலக்கம்_modulus(numval, n, &failed); நிறுத்து;
	    }
	    s = வெற்று;
	    எனில் (failed)
		செல் theend;
	}
	இல்லையெனில் எனில் (opt_type == gov_string && stringval != வெற்று && s != வெற்று)
	{
	    // string
	    s = concat_str(stringval, s);
	    vim_free(stringval);
	    stringval = s;
	}
    }

    எழுத்து *err = set_option_மதிப்பு(arg, n, s, scope);
    arg_end = p;
    எனில் (err != வெற்று)
	emsg(_(err));

theend:
    *p = c1;
    vim_free(stringval);
    vim_free(tofree);
    திரும்பு arg_end;
}

/*
 * Set a register, part of ex_let_one().
 */
    நிலையான char_u *
ex_let_register(
    char_u	*arg,
    typval_T	*tv,
    முழு		flags,
    char_u	*endchars,
    char_u	*op)
{
    char_u	*arg_end = வெற்று;

    எனில் ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					 && (flags & ASSIGN_FOR_LOOP) == 0)
    {
	emsg(_(e_cannot_lock_register));
	திரும்பு வெற்று;
    }
    ++arg;
    எனில் (op != வெற்று && vim_strchr((char_u *)"+-*/%", *op) != வெற்று)
	semsg(_(e_wrong_variable_type_for_str_equal), op);
    இல்லையெனில் எனில் (endchars != வெற்று
			  && vim_strchr(endchars, *skipwhite(arg + 1)) == வெற்று)
	emsg(_(e_unexpected_characters_in_let));
    இல்லையெனில்
    {
	char_u	*ptofree = வெற்று;
	char_u	*p;

	p = tv_get_string_chk(tv);
	எனில் (p != வெற்று && op != வெற்று && *op == '.')
	{
	    char_u  *s = get_reg_contents(*arg == '@'
						  ? '"' : *arg, GREG_கோவை_SRC);

	    எனில் (s != வெற்று)
	    {
		p = ptofree = concat_str(s, p);
		vim_free(s);
	    }
	}
	எனில் (p != வெற்று)
	{
	    write_reg_contents(*arg == '@' ? '"' : *arg, p, -1, FALSE);
	    arg_end = arg + 1;
	}
	vim_free(ptofree);
    }
    திரும்பு arg_end;
}

/*
 * Set one item of ":let var = expr" or ":let [v1, v2] = list" to its value.
 * Returns a pointer to the char just after the var name.
 * Returns NULL if there is an error.
 */
    நிலையான char_u *
ex_let_one(
    char_u	*arg,		// points to variable name
    typval_T	*tv,		// value to assign to variable
    முழு		copy,		// copy value from "tv"
    முழு		flags,		// ASSIGN_CONST, ASSIGN_FINAL, etc.
    char_u	*endchars,	// valid chars after variable name  or NULL
    char_u	*op,		// "+", "-", "."  or NULL
    முழு		var_idx)	// variable index for "let [a, b] = list"
{
    char_u	*arg_end = வெற்று;

    எனில் (in_vim9script() && (flags & (ASSIGN_NO_DECL | ASSIGN_DECL)) == 0
			&& (flags & (ASSIGN_CONST | ASSIGN_FINAL)) == 0
				  && vim_strchr((char_u *)"$@&", *arg) != வெற்று)
    {
	vim9_declare_error(arg);
	திரும்பு வெற்று;
    }

    எனில் (check_typval_is_மதிப்பு(tv) == FAIL)
	திரும்பு வெற்று;

    // In Vim9 script an environment variable and a register only take a
    // String.  "@#" also takes a buffer number.
    எனில் (in_vim9script() && (*arg == '$' || *arg == '@')
	    && !(*arg == '@' && arg[1] == '#' && tv->v_type == VAR_எண்
					       && (op == வெற்று || *op == '=')))
    {
	where_T	where = WHERE_INIT;

	எனில் (check_typval_type(&t_string, tv, where) == FAIL)
	    திரும்பு வெற்று;
    }

    எனில் (*arg == '$')
    {
	// ":let $VAR = expr": Set environment variable.
	திரும்பு ex_let_env(arg, tv, flags, endchars, op);
    }
    இல்லையெனில் எனில் (*arg == '&')
    {
	// ":let &option = expr": Set option value.
	// ":let &l:option = expr": Set local option value.
	// ":let &g:option = expr": Set global option value.
	// ":for &ts in range(8)": Set option value for for loop
	திரும்பு ex_let_option(arg, tv, flags, endchars, op);
    }
    இல்லையெனில் எனில் (*arg == '@')
    {
	// ":let @r = expr": Set register contents.
	திரும்பு ex_let_register(arg, tv, flags, endchars, op);
    }
    இல்லையெனில் எனில் (eval_isnamec1(*arg) || *arg == '{')
    {
	lval_T	lv;
	char_u	*p;
	முழு	lval_flags = (flags & (ASSIGN_NO_DECL | ASSIGN_DECL))
							     ? GLV_NO_DECL : 0;
	lval_flags |= (flags & ASSIGN_FOR_LOOP) ? GLV_FOR_LOOP : 0;
	எனில் (op != வெற்று && *op != '=')
	    lval_flags |= GLV_ASSIGN_WITH_OP;

	// ":let var = expr": Set internal variable.
	// ":let var: type = expr": Set internal variable with type.
	// ":let {expr} = expr": Idem, name made with curly braces
	p = get_lval(arg, tv, &lv, FALSE, FALSE, lval_flags, FNE_CHECK_START);
	எனில் (p != வெற்று && lv.ll_name != வெற்று)
	{
	    எனில் (endchars != வெற்று && vim_strchr(endchars,
					   *skipwhite(lv.ll_name_end)) == வெற்று)
	    {
		emsg(_(e_unexpected_characters_in_let));
	    }
	    இல்லையெனில்
	    {
		set_var_lval(&lv, p, tv, copy, flags, op, var_idx);
		arg_end = lv.ll_name_end;
	    }
	}
	clear_lval(&lv);
    }
    இல்லையெனில்
	semsg(_(e_invalid_argument_str), arg);

    திரும்பு arg_end;
}

/*
 * ":unlet[!] var1 ... " command.
 */
    வெறுமை
ex_unlet(exarg_T *eap)
{
    ex_unletlock(eap, eap->arg, 0, eap->forceit ? GLV_QUIET : 0, do_unlet_var, வெற்று);
}

/*
 * ":lockvar" and ":unlockvar" commands
 */
    வெறுமை
ex_lockvar(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    முழு		deep = 2;

    எனில் (eap->forceit)
	deep = -1;
    இல்லையெனில் எனில் (vim_isdigit(*arg))
    {
	deep = getdigits(&arg);
	arg = skipwhite(arg);
    }

    ex_unletlock(eap, arg, deep, 0, do_lock_var, வெற்று);
}

/*
 * ":unlet", ":lockvar" and ":unlockvar" are quite similar.
 * Also used for Vim9 script.  "callback" is invoked as:
 *	callback(&lv, name_end, eap, deep, cookie)
 */
    வெறுமை
ex_unletlock(
    exarg_T	*eap,
    char_u	*argstart,
    முழு		deep,
    முழு		glv_flags,
    முழு		(*callback)(lval_T *, char_u *, exarg_T *, முழு, வெறுமை *),
    வெறுமை	*cookie)
{
    char_u	*arg = argstart;
    char_u	*name_end;
    முழு		error = FALSE;
    lval_T	lv;

    செய்
    {
	எனில் (*arg == '$')
	{
	    lv.ll_name = arg;
	    lv.ll_tv = வெற்று;
	    ++arg;
	    எனில் (get_env_len(&arg) == 0)
	    {
		semsg(_(e_invalid_argument_str), arg - 1);
		திரும்பு;
	    }
	    எனில் (!error && !eap->skip
			      && callback(&lv, arg, eap, deep, cookie) == FAIL)
		error = TRUE;
	    name_end = arg;
	}
	இல்லையெனில்
	{
	    // Parse the name and find the end.
	    name_end = get_lval(arg, வெற்று, &lv, TRUE, eap->skip || error,
				     glv_flags | GLV_NO_DECL, FNE_CHECK_START);
	    எனில் (lv.ll_name == வெற்று)
		error = TRUE;	    // error but continue parsing
	    எனில் (name_end == வெற்று || (!VIM_ISWHITE(*name_end)
						    && !ends_excmd(*name_end)))
	    {
		எனில் (name_end != வெற்று)
		{
		    emsg_severe = TRUE;
		    semsg(_(e_trailing_characters_str), name_end);
		}
		எனில் (!(eap->skip || error))
		    clear_lval(&lv);
		நிறுத்து;
	    }

	    எனில் (!error && !eap->skip
			 && callback(&lv, name_end, eap, deep, cookie) == FAIL)
		error = TRUE;

	    எனில் (!eap->skip)
		clear_lval(&lv);
	}

	arg = skipwhite(name_end);
    } போது (!ends_excmd2(name_end, arg));

    set_nextcmd(eap, arg);
}

    நிலையான முழு
do_unlet_var(
    lval_T	*lp,
    char_u	*name_end,
    exarg_T	*eap,
    முழு		deep UNUSED,
    வெறுமை	*cookie UNUSED)
{
    முழு		forceit = eap->forceit;
    முழு		ret = OK;
    முழு		cc;

    எனில் (lp->ll_tv == வெற்று)
    {
	cc = *name_end;
	*name_end = NUL;

	// Environment variable, normal name or expanded name.
	எனில் (*lp->ll_name == '$')
	    vim_unsetenv_ext(lp->ll_name + 1);
	இல்லையெனில் எனில் (do_unlet(lp->ll_name, forceit) == FAIL)
	    ret = FAIL;
	*name_end = cc;
    }
    இல்லையெனில் எனில் ((lp->ll_list != வெற்று
		 && மதிப்பு_check_lock(lp->ll_list->lv_lock, lp->ll_name, FALSE))
	    || (lp->ll_dict != வெற்று
		&& மதிப்பு_check_lock(lp->ll_dict->dv_lock, lp->ll_name, FALSE)))
	திரும்பு FAIL;
    இல்லையெனில் எனில் (lp->ll_range)
	list_unlet_range(lp->ll_list, lp->ll_li, lp->ll_n1,
						    !lp->ll_empty2, lp->ll_n2);
    இல்லையெனில் எனில் (lp->ll_list != வெற்று)
	// unlet a List item.
	listitem_remove(lp->ll_list, lp->ll_li);
    இல்லையெனில் எனில் (lp->ll_dict != வெற்று)
	// unlet a Dictionary item.
	dictitem_remove(lp->ll_dict, lp->ll_di, "unlet");
    இல்லையெனில்
    {
	semsg(_(e_cannot_unlet_imported_item_str), lp->ll_name);
	திரும்பு FAIL;
    }

    திரும்பு ret;
}

/*
 * Unlet one item or a range of items from a list.
 * Return OK or FAIL.
 */
    வெறுமை
list_unlet_range(
	list_T	    *l,
	listitem_T  *li_first,
	long	    n1_arg,
	முழு	    has_n2,
	long	    n2)
{
    listitem_T  *li = li_first;
    முழு		n1 = n1_arg;

    // Delete a range of List items.
    li = li_first;
    n1 = n1_arg;
    போது (li != வெற்று && (!has_n2 || n2 >= n1))
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
    முழு
do_unlet(char_u *name, முழு forceit)
{
    hashtab_T	*ht;
    hashitem_T	*hi;
    char_u	*varname = வெற்று;  // init to shut up gcc
    dict_T	*d;
    dictitem_T	*di;

    // can't :unlet a script variable in Vim9 script
    எனில் (in_vim9script() && check_vim9_unlet(name) == FAIL)
	திரும்பு FAIL;

    ht = find_var_ht(name, &varname);

    // can't :unlet a script variable in Vim9 script from a function
    எனில் (ht == get_script_local_ht()
	    && SCRIPT_ID_VALID(current_sctx.sc_sid)
	    && SCRIPT_ITEM(current_sctx.sc_sid)->sn_version
							 == SCRIPT_VERSION_VIM9
	    && check_vim9_unlet(name) == FAIL)
	திரும்பு FAIL;

    எனில் (ht != வெற்று && *varname != NUL)
    {
	d = get_current_funccal_dict(ht);
	எனில் (d == வெற்று)
	{
	    எனில் (ht == &globvarht)
		d = &globvardict;
	    இல்லையெனில் எனில் (ht == &compat_hashtab)
		d = &vimvardict;
	    இல்லையெனில்
	    {
		di = find_var_in_ht(ht, *name, (char_u *)"", FALSE);
		d = di == வெற்று ? வெற்று : di->di_tv.vval.v_dict;
	    }
	    எனில் (d == வெற்று)
	    {
		internal_error("do_unlet()");
		திரும்பு FAIL;
	    }
	}
	hi = hash_find(ht, varname);
	எனில் (HASHITEM_EMPTY(hi))
	    hi = find_hi_in_scoped_ht(name, &ht);
	எனில் (hi != வெற்று && !HASHITEM_EMPTY(hi))
	{
	    di = HI2DI(hi);
	    எனில் (var_check_fixed(di->di_flags, name, FALSE)
		    || var_check_ro(di->di_flags, name, FALSE)
		    || மதிப்பு_check_lock(d->dv_lock, name, FALSE)
		    || check_hashtab_frozen(ht, "unlet"))
		திரும்பு FAIL;

	    delete_var(ht, hi);
	    திரும்பு OK;
	}
    }
    எனில் (forceit)
	திரும்பு OK;
    semsg(_(e_no_such_variable_str), name);
    திரும்பு FAIL;
}

    நிலையான வெறுமை
report_lockvar_member(எழுத்து *செய்தி, lval_T *lp)
{
    முழு did_alloc = FALSE;
    char_u *vname = (char_u *)"";
    char_u *class_name = lp->ll_class != வெற்று
				    ? lp->ll_class->class_name.string : (char_u *)"";
    எனில் (lp->ll_name != வெற்று)
    {
	எனில் (lp->ll_name_end == வெற்று)
	    vname = lp->ll_name;
	இல்லையெனில்
	{
	    vname = vim_strnsave(lp->ll_name, lp->ll_name_end - lp->ll_name);
	    எனில் (vname == வெற்று)
		திரும்பு;
	    did_alloc = TRUE;
	}
    }
    semsg(_(செய்தி), vname, class_name);
    எனில் (did_alloc)
	vim_free(vname);
}

/*
 * Lock or unlock variable indicated by "lp".
 * "deep" is the levels to go (-1 for unlimited);
 * "lock" is TRUE for ":lockvar", FALSE for ":unlockvar".
 */
    நிலையான முழு
do_lock_var(
    lval_T	*lp,
    char_u	*name_end,
    exarg_T	*eap,
    முழு		deep,
    வெறுமை	*cookie UNUSED)
{
    முழு		lock = eap->cmdidx == CMD_lockvar;
    முழு		ret = OK;
    முழு		cc;
    dictitem_T	*di;

#ifdef LOG_LOCKVAR
    ch_log(வெற்று, "LKVAR: do_lock_var(): name %s, is_root %d", lp->ll_name, lp->ll_is_வேர்);
#endif

    எனில் (lp->ll_tv == வெற்று)
    {
	cc = *name_end;
	*name_end = NUL;
	எனில் (*lp->ll_name == '$')
	{
	    semsg(_(e_cannot_lock_or_unlock_variable_str), lp->ll_name);
	    ret = FAIL;
	}
	இல்லையெனில்
	{
	    // Normal name or expanded name.
	    di = find_var(lp->ll_name, வெற்று, TRUE);
	    எனில் (di == வெற்று)
	    {
		எனில் (in_vim9script())
		    semsg(_(e_cannot_find_variable_to_unlock_str),
								  lp->ll_name);
		ret = FAIL;
	    }
	    இல்லையெனில்
	    {
		எனில் ((di->di_flags & DI_FLAGS_FIX)
			    && di->di_tv.v_type != VAR_DICT
			    && di->di_tv.v_type != VAR_LIST)
		{
		    // For historic reasons this error is not given for a list
		    // or dict.  E.g., the b: dict could be locked/unlocked.
		    semsg(_(e_cannot_lock_or_unlock_variable_str), lp->ll_name);
		    ret = FAIL;
		}
		இல்லையெனில்
		{
		    எனில் (in_vim9script())
		    {
			svar_T  *sv = find_typval_in_script(&di->di_tv,
								     0, FALSE);

			எனில் (sv != வெற்று && sv->sv_const != 0)
			{
			    semsg(_(e_cannot_change_readonly_variable_str),
								  lp->ll_name);
			    ret = FAIL;
			}
		    }

		    எனில் (ret == OK)
		    {
			எனில் (lock)
			    di->di_flags |= DI_FLAGS_LOCK;
			இல்லையெனில்
			    di->di_flags &= ~DI_FLAGS_LOCK;
			எனில் (deep != 0)
			    item_lock(&di->di_tv, deep, lock, FALSE);
		    }
		}
	    }
	}
	*name_end = cc;
    }
    இல்லையெனில் எனில் (deep == 0 && lp->ll_object == வெற்று && lp->ll_class == வெற்று)
    {
	// nothing to do
    }
    இல்லையெனில் எனில் (lp->ll_is_வேர்)
	// (un)lock the item.
	item_lock(lp->ll_tv, deep, lock, FALSE);
    இல்லையெனில் எனில் (lp->ll_range)
    {
	listitem_T    *li = lp->ll_li;

	// (un)lock a range of List items.
	போது (li != வெற்று && (lp->ll_empty2 || lp->ll_n2 >= lp->ll_n1))
	{
	    item_lock(&li->li_tv, deep, lock, FALSE);
	    li = li->li_next;
	    ++lp->ll_n1;
	}
    }
    இல்லையெனில் எனில் (lp->ll_list != வெற்று)
	// (un)lock a List item.
	item_lock(&lp->ll_li->li_tv, deep, lock, FALSE);
    இல்லையெனில் எனில் (lp->ll_object != வெற்று)  // This check must be before ll_class.
    {
	// (un)lock an object variable.
	report_lockvar_member(e_cannot_lock_object_variable_str, lp);
	ret = FAIL;
    }
    இல்லையெனில் எனில் (lp->ll_class != வெற்று)
    {
	// (un)lock a class variable.
	report_lockvar_member(e_cannot_lock_class_variable_str, lp);
	ret = FAIL;
    }
    இல்லையெனில்
    {
	// (un)lock a Dictionary item.
	எனில் (lp->ll_di == வெற்று)
	{
	    emsg(_(e_dictionary_required));
	    ret = FAIL;
	}
	இல்லையெனில்
	    item_lock(&lp->ll_di->di_tv, deep, lock, FALSE);
    }

    திரும்பு ret;
}

/*
 * Lock or unlock an item.  "deep" is nr of levels to go.
 * When "check_refcount" is TRUE do not lock a list or dict with a reference
 * count larger than 1.
 */
    வெறுமை
item_lock(typval_T *tv, முழு deep, முழு lock, முழு check_refcount)
{
    நிலையான முழு	recurse = 0;
    list_T	*l;
    listitem_T	*li;
    dict_T	*d;
    blob_T	*b;
    hashitem_T	*hi;
    முழு		todo;

#ifdef LOG_LOCKVAR
    ch_log(வெற்று, "LKVAR: item_lock(): type %s", vartype_name(tv->v_type));
#endif

    எனில் (recurse >= DICT_MAXNEST)
    {
	emsg(_(e_variable_nested_too_deep_for_unlock));
	திரும்பு;
    }
    எனில் (deep == 0)
	திரும்பு;
    ++recurse;

    // lock/unlock the item itself
    எனில் (lock)
	tv->v_lock |= VAR_LOCKED;
    இல்லையெனில்
	tv->v_lock &= ~VAR_LOCKED;

    தேர்வு (tv->v_type)
    {
	நிலை VAR_UNKNOWN:
	நிலை VAR_ANY:
	நிலை VAR_VOID:
	நிலை VAR_எண்:
	நிலை VAR_BOOL:
	நிலை VAR_STRING:
	நிலை VAR_FUNC:
	நிலை VAR_PARTIAL:
	நிலை VAR_FLOAT:
	நிலை VAR_SPECIAL:
	நிலை VAR_JOB:
	நிலை VAR_CHANNEL:
	நிலை VAR_INSTR:
	நிலை VAR_CLASS:
	நிலை VAR_OBJECT:
	நிலை VAR_TYPEALIAS:
	    நிறுத்து;

	நிலை VAR_BLOB:
	    எனில் ((b = tv->vval.v_blob) != வெற்று
				    && !(check_refcount && b->bv_refcount > 1))
	    {
		எனில் (lock)
		    b->bv_lock |= VAR_LOCKED;
		இல்லையெனில்
		    b->bv_lock &= ~VAR_LOCKED;
	    }
	    நிறுத்து;
	நிலை VAR_LIST:
	    எனில் ((l = tv->vval.v_list) != வெற்று
				    && !(check_refcount && l->lv_refcount > 1))
	    {
		எனில் (lock)
		    l->lv_lock |= VAR_LOCKED;
		இல்லையெனில்
		    l->lv_lock &= ~VAR_LOCKED;
		எனில் (deep < 0 || deep > 1)
		{
		    எனில் (l->lv_first == &range_list_item)
			l->lv_lock |= VAR_பொருட்கள்_LOCKED;
		    இல்லையெனில்
		    {
			// recursive: lock/unlock the items the List contains
			CHECK_LIST_MATERIALIZE(l);
			FOR_ALL_LIST_பொருட்கள்(l, li) item_lock(&li->li_tv,
					       deep - 1, lock, check_refcount);
		    }
		}
	    }
	    நிறுத்து;
	நிலை VAR_TUPLE:
	    tuple_lock(tv->vval.v_tuple, deep, lock, check_refcount);
	    நிறுத்து;
	நிலை VAR_DICT:
	    எனில் ((d = tv->vval.v_dict) != வெற்று
				    && !(check_refcount && d->dv_refcount > 1))
	    {
		எனில் (lock)
		    d->dv_lock |= VAR_LOCKED;
		இல்லையெனில்
		    d->dv_lock &= ~VAR_LOCKED;
		எனில் (deep < 0 || deep > 1)
		{
		    // recursive: lock/unlock the items the List contains
		    todo = (முழு)d->dv_hashtab.ht_used;
		    FOR_ALL_HASHTAB_பொருட்கள்(&d->dv_hashtab, hi, todo)
		    {
			எனில் (!HASHITEM_EMPTY(hi))
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

#எனில் defined(FEAT_MENU) && defined(FEAT_MULTI_LANG)
/*
 * Delete all "menutrans_" variables.
 */
    வெறுமை
del_menutrans_vars(வெறுமை)
{
    hashitem_T	*hi;
    முழு		todo;

    hash_lock(&globvarht);
    todo = (முழு)globvarht.ht_used;
    க்கு (hi = globvarht.ht_array; todo > 0 && !got_int; ++hi)
    {
	எனில் (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    எனில் (STRNCMP(HI2DI(hi)->di_சாவி, "menutrans_", 10) == 0)
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

நிலையான char_u	*varnamebuf = வெற்று;
நிலையான முழு	varnamebuflen = 0;

/*
 * Function to concatenate a prefix and a variable name.
 */
    char_u *
cat_prefix_varname(முழு prefix, char_u *name)
{
    முழு		len;

    len = (முழு)STRLEN(name) + 3;
    எனில் (len > varnamebuflen)
    {
	vim_free(varnamebuf);
	len += 10;			// some additional space
	varnamebuf = alloc(len);
	எனில் (varnamebuf == வெற்று)
	{
	    varnamebuflen = 0;
	    திரும்பு வெற்று;
	}
	varnamebuflen = len;
    }
    *varnamebuf = prefix;
    varnamebuf[1] = ':';
    STRCPY(varnamebuf + 2, name);
    திரும்பு varnamebuf;
}

/*
 * Function given to ExpandGeneric() to obtain the list of user defined
 * (global/buffer/window/built-in) variable names.
 */
    char_u *
get_user_var_name(expand_T *xp, முழு idx)
{
    நிலையான long_u	gdone;
    நிலையான long_u	bdone;
    நிலையான long_u	wdone;
    நிலையான long_u	tdone;
    நிலையான முழு		vidx;
    நிலையான hashitem_T	*hi;
    hashtab_T		*ht;

    எனில் (idx == 0)
    {
	gdone = bdone = wdone = vidx = 0;
	tdone = 0;
    }

    // Global variables
    எனில் (gdone < globvarht.ht_used)
    {
	எனில் (gdone++ == 0)
	    hi = globvarht.ht_array;
	இல்லையெனில்
	    ++hi;
	போது (HASHITEM_EMPTY(hi))
	    ++hi;
	எனில் (STRNCMP("g:", xp->xp_pattern, 2) == 0)
	    திரும்பு cat_prefix_varname('g', hi->hi_சாவி);
	திரும்பு hi->hi_சாவி;
    }

    // b: variables
    ht = &prevwin_curwin()->w_buffer->b_vars->dv_hashtab;
    எனில் (bdone < ht->ht_used)
    {
	எனில் (bdone++ == 0)
	    hi = ht->ht_array;
	இல்லையெனில்
	    ++hi;
	போது (HASHITEM_EMPTY(hi))
	    ++hi;
	திரும்பு cat_prefix_varname('b', hi->hi_சாவி);
    }

    // w: variables
    ht = &prevwin_curwin()->w_vars->dv_hashtab;
    எனில் (wdone < ht->ht_used)
    {
	எனில் (wdone++ == 0)
	    hi = ht->ht_array;
	இல்லையெனில்
	    ++hi;
	போது (HASHITEM_EMPTY(hi))
	    ++hi;
	திரும்பு cat_prefix_varname('w', hi->hi_சாவி);
    }

    // t: variables
    ht = &curtab->tp_vars->dv_hashtab;
    எனில் (tdone < ht->ht_used)
    {
	எனில் (tdone++ == 0)
	    hi = ht->ht_array;
	இல்லையெனில்
	    ++hi;
	போது (HASHITEM_EMPTY(hi))
	    ++hi;
	திரும்பு cat_prefix_varname('t', hi->hi_சாவி);
    }

    // v: variables
    எனில் (vidx < VV_LEN)
	திரும்பு cat_prefix_varname('v', (char_u *)vimvars[vidx++].vv_name);

    VIM_CLEAR(varnamebuf);
    varnamebuflen = 0;
    திரும்பு வெற்று;
}

    எழுத்து *
get_var_special_name(முழு nr)
{
    தேர்வு (nr)
    {
	நிலை VVAL_FALSE: திரும்பு in_vim9script() ? "false" : "v:false";
	நிலை VVAL_TRUE:  திரும்பு in_vim9script() ? "true" : "v:true";
	நிலை VVAL_NULL:  திரும்பு in_vim9script() ? "null" : "v:null";
	நிலை VVAL_NONE:  திரும்பு "v:none";
    }
    internal_error("get_var_special_name()");
    திரும்பு "42";
}

/*
 * Returns the global variable dictionary
 */
    dict_T *
get_globvar_dict(வெறுமை)
{
    திரும்பு &globvardict;
}

/*
 * Returns the global variable hash table
 */
    hashtab_T *
get_globvar_ht(வெறுமை)
{
    திரும்பு &globvarht;
}

/*
 * Returns the v: variable dictionary
 */
    dict_T *
get_vimvar_dict(வெறுமை)
{
    திரும்பு &vimvardict;
}

/*
 * Returns the index of a v:variable.  Negative if not found.
 * Returns DI_ flags in "di_flags".
 */
    முழு
find_vim_var(char_u *name, முழு *di_flags)
{
    dictitem_T	    *di = find_var_in_ht(&vimvarht, 0, name, TRUE);
    கட்டமைப்பு vimvar   *vv;

    எனில் (di == வெற்று)
	திரும்பு -1;
    *di_flags = di->di_flags;
    vv = (கட்டமைப்பு vimvar *)((எழுத்து *)di - offsetof(vimvar_T, vv_di));
    திரும்பு (முழு)(vv - vimvars);
}


/*
 * Set type of v: variable to "type".
 */
    வெறுமை
set_vim_var_type(முழு idx, vartype_T type)
{
    vimvars[idx].vv_tv_type = type;
}

/*
 * Set number v: variable to "val".
 * Note that this does not set the type, use set_vim_var_type() for that.
 */
    வெறுமை
set_vim_var_nr(முழு idx, varnumber_T val)
{
    vimvars[idx].vv_nr = val;
}

    எழுத்து *
get_vim_var_name(முழு idx)
{
    திரும்பு vimvars[idx].vv_name;
}

/*
 * Get typval_T v: variable value.
 */
    typval_T *
get_vim_var_tv(முழு idx)
{
    திரும்பு &vimvars[idx].vv_tv;
}

    type_T *
get_vim_var_type(முழு idx, garray_T *type_list)
{
    எனில் (vimvars[idx].vv_type != வெற்று)
	திரும்பு vimvars[idx].vv_type;
    திரும்பு typval2type_vimvar(&vimvars[idx].vv_tv, type_list);
}

/*
 * Add what getinfo() reports for the v: variable "name", without the "v:", to
 * "d".  Returns FAIL when there is no such variable.
 */
    முழு
vim_var_info(char_u *name, dict_T *d)
{
    முழு		di_flags;
    முழு		idx = find_vim_var(name, &di_flags);
    garray_T	type_list;
    எழுத்து	*tofree;

    எனில் (idx < 0)
	திரும்பு FAIL;
    vim_snprintf((எழுத்து *)IObuff, IOSIZE, "v:%s", vimvars[idx].vv_name);
    dict_add_string(d, "name", IObuff);
    dict_add_bool(d, "available", TRUE);
    ga_init2(&type_list, அளவு(type_T *), 10);
    எழுத்து *type = type_name(get_vim_var_type(idx, &type_list), &tofree);
    dict_add_string(d, "type", (char_u *)type);
    vim_free(tofree);
    clear_type_list(&type_list);
    dict_add_bool(d, "readonly", (vimvars[idx].vv_flags & VV_RO) != 0);
    dict_add_bool(d, "compat", (vimvars[idx].vv_flags & VV_COMPAT) != 0);
    திரும்பு OK;
}

/*
 * Set v: variable to "tv".  Only accepts the same type.
 * Takes over the value of "tv".
 */
    முழு
set_vim_var_tv(முழு idx, typval_T *tv)
{
    எனில் (vimvars[idx].vv_tv_type != tv->v_type)
    {
	emsg(_(e_type_mismatch_for_v_variable));
	clear_tv(tv);
	திரும்பு FAIL;
    }
    // VV_RO is also checked when compiling, but let's check here as well.
    எனில் (vimvars[idx].vv_flags & VV_RO)
    {
	semsg(_(e_cannot_change_readonly_variable_str), vimvars[idx].vv_name);
	திரும்பு FAIL;
    }
    எனில் (sandbox && (vimvars[idx].vv_flags & VV_RO_SBX))
    {
	semsg(_(e_cannot_set_variable_in_sandbox_str), vimvars[idx].vv_name);
	திரும்பு FAIL;
    }
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_di.di_tv = *tv;
    திரும்பு OK;
}

/*
 * Get number v: variable value.
 */
    varnumber_T
get_vim_var_nr(முழு idx)
{
    திரும்பு vimvars[idx].vv_nr;
}

/*
 * Get string v: variable value.  Uses a static buffer, can only be used once.
 * If the String variable has never been set, return an empty string.
 * Never returns NULL;
 */
    char_u *
get_vim_var_str(முழு idx)
{
    திரும்பு tv_get_string(&vimvars[idx].vv_tv);
}

/*
 * Get List v: variable value.  Caller must take care of reference count when
 * needed.
 */
    list_T *
get_vim_var_list(முழு idx)
{
    திரும்பு vimvars[idx].vv_list;
}

/*
 * Get Dict v: variable value.  Caller must take care of reference count when
 * needed.
 */
    dict_T *
get_vim_var_dict(முழு idx)
{
    திரும்பு vimvars[idx].vv_dict;
}

/*
 * Set v:char to character "c".
 */
    வெறுமை
set_vim_var_char(முழு c)
{
    char_u  buf[MB_MAXBYTES + 1];
    size_t  buflen;

    எனில் (has_mbyte)
	buflen = (*mb_char2bytes)(c, buf);
    இல்லையெனில்
    {
	buf[0] = c;
	buflen = 1;
    }
    buf[buflen] = NUL;

    set_vim_var_string(VV_CHAR, buf, (முழு)buflen);
}

/*
 * Set v:count to "count" and v:count1 to "count1".
 * When "set_prevcount" is TRUE first set v:prevcount from v:count.
 */
    வெறுமை
set_vcount(
    long	மொத்தம்,
    long	count1,
    முழு		set_prevcount)
{
    எனில் (set_prevcount)
	vimvars[VV_PREVCOUNT].vv_nr = vimvars[VV_மொத்தம்].vv_nr;
    vimvars[VV_மொத்தம்].vv_nr = மொத்தம்;
    vimvars[VV_COUNT1].vv_nr = count1;
}

/*
 * Save variables that might be changed as a side effect.  Used when executing
 * a timer callback.
 */
    வெறுமை
save_vimvars(vimvars_save_T *vvsave)
{
    vvsave->vv_prevcount = vimvars[VV_PREVCOUNT].vv_nr;
    vvsave->vv_மொத்தம் = vimvars[VV_மொத்தம்].vv_nr;
    vvsave->vv_count1 = vimvars[VV_COUNT1].vv_nr;
}

/*
 * Restore variables saved by save_vimvars().
 */
    வெறுமை
restore_vimvars(vimvars_save_T *vvsave)
{
    vimvars[VV_PREVCOUNT].vv_nr = vvsave->vv_prevcount;
    vimvars[VV_மொத்தம்].vv_nr = vvsave->vv_மொத்தம்;
    vimvars[VV_COUNT1].vv_nr = vvsave->vv_count1;
}

/*
 * Set string v: variable to a copy of "val". If 'copy' is FALSE, then set the
 * value.
 */
    வெறுமை
set_vim_var_string(
    முழு		idx,
    char_u	*val,
    முழு		len)	    // length of "val" to use or -1 (whole string)
{
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_tv_type = VAR_STRING;
    எனில் (val == வெற்று)
	vimvars[idx].vv_str = வெற்று;
    இல்லையெனில் எனில் (len == -1)
	vimvars[idx].vv_str = vim_strsave(val);
    இல்லையெனில்
	vimvars[idx].vv_str = vim_strnsave(val, len);
}

    வெறுமை
set_vim_var_string_direct(
    முழு		idx,
    char_u	*val)
{
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_tv_type = VAR_STRING;

    vimvars[idx].vv_str = val;
}

/*
 * Set List v: variable to "val".
 */
    வெறுமை
set_vim_var_list(முழு idx, list_T *val)
{
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_tv_type = VAR_LIST;
    vimvars[idx].vv_list = val;
    எனில் (val != வெற்று)
	++val->lv_refcount;
}

/*
 * Set Dictionary v: variable to "val".
 */
    வெறுமை
set_vim_var_dict(முழு idx, dict_T *val)
{
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_tv_type = VAR_DICT;
    vimvars[idx].vv_dict = val;
    எனில் (val == வெற்று)
	திரும்பு;

    ++val->dv_refcount;
    dict_set_பொருட்கள்_ro(val);
}

/*
 * Set the v:argv list.
 */
    வெறுமை
set_argv_var(எழுத்து **argv, முழு argc)
{
    list_T	*l = list_alloc();
    முழு		i;

    எனில் (l == வெற்று)
	getout(1);
    l->lv_lock = VAR_FIXED;
    க்கு (i = 0; i < argc; ++i)
    {
	எனில் (list_append_string(l, (char_u *)argv[i], -1) == FAIL)
	    getout(1);
	l->lv_u.mat.lv_last->li_tv.v_lock = VAR_FIXED;
    }
    set_vim_var_list(VV_ARGV, l);
}

/*
 * Reset v:register, taking the 'clipboard' setting into account.
 */
    வெறுமை
reset_reg_var(வெறுமை)
{
    முழு regname = 0;

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
    வெறுமை
set_reg_var(முழு c)
{
    char_u  regname[2];

    எனில் (c == 0 || c == ' ')
	regname[0] = '"';
    இல்லையெனில்
	regname[0] = c;
    regname[1] = NUL;
    // Avoid free/alloc when the value is already right.
    எனில் (vimvars[VV_REG].vv_str == வெற்று || vimvars[VV_REG].vv_str[0] != c)
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
    எனில் (oldval == வெற்று)
	திரும்பு vimvars[VV_EXCEPTION].vv_str;

    vimvars[VV_EXCEPTION].vv_str = oldval;
    திரும்பு வெற்று;
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
    எனில் (oldval == வெற்று)
	திரும்பு vimvars[VV_THROWPOINT].vv_str;

    vimvars[VV_THROWPOINT].vv_str = oldval;
    திரும்பு வெற்று;
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
    எனில் (eap == வெற்று)
    {
	vim_free(oldval);
	vimvars[VV_CMDARG].vv_str = oldarg;
	திரும்பு வெற்று;
    }

    எனில் (eap->force_bin == FORCE_BIN)
	len = 6;
    இல்லையெனில் எனில் (eap->force_bin == FORCE_NOBIN)
	len = 8;
    இல்லையெனில்
	len = 0;

    எனில் (eap->read_edit)
	len += 7;

    எனில் (eap->force_ff != 0)
	len += 10; // " ++ff=unix"
    எனில் (eap->force_enc != 0)
	len += (unsigned)STRLEN(eap->cmd + eap->force_enc) + 7;
    எனில் (eap->bad_char != 0)
	len += 7 + 4;  // " ++bad=" + "keep" or "drop"

    newval = alloc(len + 1);
    எனில் (newval == வெற்று)
	திரும்பு வெற்று;

    எனில் (eap->force_bin == FORCE_BIN)
	sprintf((எழுத்து *)newval, " ++bin");
    இல்லையெனில் எனில் (eap->force_bin == FORCE_NOBIN)
	sprintf((எழுத்து *)newval, " ++nobin");
    இல்லையெனில்
	*newval = NUL;

    எனில் (eap->read_edit)
	STRCAT(newval, " ++edit");

    எனில் (eap->force_ff != 0)
	sprintf((எழுத்து *)newval + STRLEN(newval), " ++ff=%s",
						eap->force_ff == 'u' ? "unix"
						: eap->force_ff == 'd' ? "dos"
						: "mac");
    எனில் (eap->force_enc != 0)
	sprintf((எழுத்து *)newval + STRLEN(newval), " ++enc=%s",
					       eap->cmd + eap->force_enc);
    எனில் (eap->bad_char == BAD_KEEP)
	STRCPY(newval + STRLEN(newval), " ++bad=keep");
    இல்லையெனில் எனில் (eap->bad_char == BAD_DROP)
	STRCPY(newval + STRLEN(newval), " ++bad=drop");
    இல்லையெனில் எனில் (eap->bad_char != 0)
	sprintf((எழுத்து *)newval + STRLEN(newval), " ++bad=%c", eap->bad_char);
    vimvars[VV_CMDARG].vv_str = newval;
    திரும்பு oldval;
}

/*
 * Get the value of internal variable "name".
 * If "flags" has EVAL_VAR_IMPORT may return a VAR_ANY with v_number set to the
 * imported script ID.
 * Return OK or FAIL.  If OK is returned "rettv" must be cleared.
 */
    முழு
eval_variable(
    char_u	*name,
    முழு		len,		// length of "name" or zero
    scid_T	sid,		// script ID for imported item or zero
    typval_T	*rettv,		// NULL when only checking existence
    dictitem_T	**dip,		// non-NULL when typval's dict item is needed
    முழு		flags)		// EVAL_VAR_ flags
{
    முழு		ret = OK;
    typval_T	*tv = வெற்று;
    முழு		found = FALSE;
    hashtab_T	*ht = வெற்று;
    முழு		cc = 0;
    type_T	*type = வெற்று;

    எனில் (len > 0)
    {
	// truncate the name, so that we can use strcmp()
	cc = name[len];
	name[len] = NUL;
    }

    // Check for local variable when debugging.
    எனில் ((sid == 0) && (tv = lookup_debug_var(name)) == வெற்று)
    {
	// Check for user-defined variables.
	dictitem_T	*v = find_var(name, &ht, flags & EVAL_VAR_NOAUTOLOAD);

	எனில் (v != வெற்று)
	{
	    tv = &v->di_tv;
	    எனில் (dip != வெற்று)
		*dip = v;
	}
	இல்லையெனில்
	    ht = வெற்று;
    }

    எனில் (tv == வெற்று && (in_vim9script() || STRNCMP(name, "s:", 2) == 0))
    {
	imported_T  *import = வெற்று;
	char_u	    *p = STRNCMP(name, "s:", 2) == 0 ? name + 2 : name;

	எனில் (sid == 0)
	    import = find_imported(p, 0, TRUE);

	// imported variable from another script
	எனில் (import != வெற்று || sid != 0)
	{
	    எனில் ((flags & EVAL_VAR_IMPORT) == 0)
	    {
		எனில் (SCRIPT_ID_VALID(sid))
		{
		    ht = &SCRIPT_VARS(sid);
		    எனில் (ht != வெற்று)
		    {
			dictitem_T *v = find_var_in_ht(ht, 0, name,
						  flags & EVAL_VAR_NOAUTOLOAD);

			எனில் (v == வெற்று)
			    v = find_var_autoload_prefix(name, sid, வெற்று, வெற்று);

			எனில் (v != வெற்று)
			{
			    tv = &v->di_tv;
			    எனில் (dip != வெற்று)
				*dip = v;
			}
			இல்லையெனில்
			    ht = வெற்று;
		    }
		}
		இல்லையெனில்
		{
		    எனில் (flags & EVAL_VAR_VERBOSE)
			semsg(_(e_expected_dot_after_name_str), name);
		    ret = FAIL;
		}
	    }
	    இல்லையெனில்
	    {
		எனில் (rettv != வெற்று)
		{
		    // special value that is used in handle_subscript()
		    rettv->v_type = VAR_ANY;
		    rettv->vval.v_எண் = sid != 0 ? sid : import->imp_sid;
		}
		found = TRUE;
	    }
	}
	இல்லையெனில் எனில் (in_vim9script() && (flags & EVAL_VAR_NO_FUNC) == 0)
	{
	    முழு	    has_g_prefix = STRNCMP(name, "g:", 2) == 0;
	    ufunc_T *ufunc = find_func(name, FALSE);

	    எனில் (ufunc != வெற்று && cc == '<')
	    {
		// handle generic function
		char_u	*argp = name + len;
		name[len] = cc;
		ufunc = eval_generic_func(ufunc, name, &argp);
		name[len] = NUL;
		எனில் (ufunc == வெற்று)
		{
		    ret = FAIL;
		    செல் done;
		}
	    }

	    // In Vim9 script we can get a function reference by using the
	    // function name.  For a global non-autoload function "g:" is
	    // required.
	    எனில் (ufunc != வெற்று && (has_g_prefix
					    || !func_requires_g_prefix(ufunc)))
	    {
		found = TRUE;
		எனில் (rettv != வெற்று)
		{
		    rettv->v_type = VAR_FUNC;
		    எனில் (has_g_prefix)
		    {
			// Keep the "g:", otherwise script-local may be
			// assumed.
			எனில் (cc != '<')
			    rettv->vval.v_string = vim_strsave(name);
			இல்லையெனில்
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
		    இல்லையெனில்
			rettv->vval.v_string = vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
		    எனில் (rettv->vval.v_string != வெற்று)
			func_ref(ufunc->uf_name);
		}
	    }
	}
    }

    எனில் (!found)
    {
	எனில் (tv == வெற்று)
	{
	    எனில் (rettv != வெற்று && (flags & EVAL_VAR_VERBOSE))
		semsg(_(e_undefined_variable_str), name);
	    ret = FAIL;
	}
	இல்லையெனில் எனில் (rettv != வெற்று)
	{
	    svar_T  *sv = வெற்று;
	    முழு	    was_assigned = FALSE;

	    எனில் (ht != வெற்று && ht == get_script_local_ht()
		    && tv != &SCRIPT_SV(current_sctx.sc_sid)->sv_var.di_tv)
	    {
		sv = find_typval_in_script(tv, 0, TRUE);
		எனில் (sv != வெற்று)
		{
		    type = sv->sv_type;
		    was_assigned = sv->sv_flags & SVFLAG_ASSIGNED;
		}
	    }

	    எனில் ((tv->v_type == VAR_TYPEALIAS || tv->v_type == VAR_CLASS)
		    && sid != 0)
	    {
		// type alias or class imported from another script.  Check
		// whether it is exported from the other script.
		sv = find_typval_in_script(tv, sid, TRUE);
		எனில் (sv == வெற்று)
		{
		    ret = FAIL;
		    செல் done;
		}
		எனில் ((sv->sv_flags & SVFLAG_EXPORTED) == 0)
		{
		    semsg(_(e_item_not_exported_in_script_str), name);
		    ret = FAIL;
		    செல் done;
		}
	    }

	    // If a list or tuple or dict variable wasn't initialized and has
	    // meaningful type, do it now.  Not for global variables, they are
	    // not declared.
	    எனில் (ht != &globvarht)
	    {
		எனில் (tv->v_type == VAR_DICT && tv->vval.v_dict == வெற்று
					    && ((type != வெற்று && !was_assigned)
							  || !in_vim9script()))
		{
		    tv->vval.v_dict = dict_alloc();
		    எனில் (tv->vval.v_dict != வெற்று)
		    {
			++tv->vval.v_dict->dv_refcount;
			tv->vval.v_dict->dv_type = alloc_type(type);
			எனில் (sv != வெற்று)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
		இல்லையெனில் எனில் (tv->v_type == VAR_LIST && tv->vval.v_list == வெற்று
					    && ((type != வெற்று && !was_assigned)
							  || !in_vim9script()))
		{
		    tv->vval.v_list = list_alloc();
		    எனில் (tv->vval.v_list != வெற்று)
		    {
			++tv->vval.v_list->lv_refcount;
			tv->vval.v_list->lv_type = alloc_type(type);
			எனில் (sv != வெற்று)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
		இல்லையெனில் எனில் (tv->v_type == VAR_TUPLE && tv->vval.v_tuple == வெற்று
					    && ((type != வெற்று && !was_assigned)
							  || !in_vim9script()))
		{
		    tv->vval.v_tuple = tuple_alloc();
		    எனில் (tv->vval.v_tuple != வெற்று)
		    {
			++tv->vval.v_tuple->tv_refcount;
			tv->vval.v_tuple->tv_type = alloc_type(type);
			எனில் (sv != வெற்று)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
		இல்லையெனில் எனில் (tv->v_type == VAR_BLOB && tv->vval.v_blob == வெற்று
					    && ((type != வெற்று && !was_assigned)
							  || !in_vim9script()))
		{
		    tv->vval.v_blob = blob_alloc();
		    எனில் (tv->vval.v_blob != வெற்று)
		    {
			++tv->vval.v_blob->bv_refcount;
			எனில் (sv != வெற்று)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
	    }
	    ret = copy_tv(tv, rettv);
	}
    }

done:
    எனில் (len > 0)
	name[len] = cc;

    திரும்பு ret;
}

/*
 * Get the value of internal variable "name", also handling "import.name".
 * Return OK or FAIL.  If OK is returned "rettv" must be cleared.
 */
    முழு
eval_variable_import(
    char_u	*name,
    typval_T	*rettv)
{
    char_u  *s = name;
    போது (ASCII_ISALNUM(*s) || *s == '_')
	++s;
    முழு	    len = (முழு)(s - name);

    எனில் (eval_variable(name, len, 0, rettv, வெற்று, EVAL_VAR_IMPORT) == FAIL)
	திரும்பு FAIL;
    எனில் (rettv->v_type == VAR_ANY && *s == '.')
    {
	char_u *ns = s + 1;
	s = ns;
	போது (ASCII_ISALNUM(*s) || *s == '_')
	    ++s;
	முழு sid = rettv->vval.v_எண்;
	திரும்பு eval_variable(ns, (முழு)(s - ns), sid, rettv, வெற்று, 0);
    }
    திரும்பு OK;
}


/*
 * Check if variable "name[len]" is a local variable or an argument.
 * If so, "*eval_lavars_used" is set to TRUE.
 */
    வெறுமை
check_vars(char_u *name, முழு len)
{
    முழு		cc;
    char_u	*varname;
    hashtab_T	*ht;

    எனில் (eval_lavars_used == வெற்று)
	திரும்பு;

    // truncate the name, so that we can use strcmp()
    cc = name[len];
    name[len] = NUL;

    ht = find_var_ht(name, &varname);
    எனில் (ht == get_funccal_local_ht() || ht == get_funccal_args_ht())
    {
	எனில் (find_var(name, வெற்று, TRUE) != வெற்று)
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
find_var(char_u *name, hashtab_T **htp, முழு no_autoload)
{
    char_u	*varname;
    hashtab_T	*ht;
    dictitem_T	*ret = வெற்று;

    ht = find_var_ht(name, &varname);
    எனில் (htp != வெற்று)
	*htp = ht;
    எனில் (ht == வெற்று)
	திரும்பு வெற்று;
    ret = find_var_in_ht(ht, *name, varname, no_autoload);
    எனில் (ret != வெற்று)
	திரும்பு ret;

    // Search in parent scope for lambda
    ret = find_var_in_scoped_ht(name, no_autoload);
    எனில் (ret != வெற்று)
	திரும்பு ret;

    // in Vim9 script items without a scope can be script-local
    எனில் (in_vim9script() && name[0] != NUL && name[1] != ':')
    {
	ht = get_script_local_ht();
	எனில் (ht != வெற்று)
	{
	    ret = find_var_in_ht(ht, *name, varname, no_autoload);
	    எனில் (ret != வெற்று)
	    {
		எனில் (htp != வெற்று)
		    *htp = ht;
		திரும்பு ret;
	    }
	}
    }

    // and finally try
    திரும்பு find_var_autoload_prefix(name, 0, htp, வெற்று);
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
find_var_autoload_prefix(char_u *name, முழு sid, hashtab_T **htp,
							    char_u **namep)
{
    hashtab_T	*ht;
    dictitem_T	*ret = வெற்று;
    // When using "vim9script autoload" script-local items are prefixed but can
    // be used with s:name.
    முழு check_sid = sid > 0 ? sid : current_sctx.sc_sid;
    எனில் (SCRIPT_ID_VALID(check_sid)
		   && (in_vim9script() || (name[0] == 's' && name[1] == ':')))
    {
	scriptitem_T *si = SCRIPT_ITEM(check_sid);

	எனில் (si->sn_autoload_prefix != வெற்று)
	{
	    char_u *base_name = (name[0] == 's' && name[1] == ':')
							     ? name + 2 : name;
	    char_u *auto_name = concat_str(si->sn_autoload_prefix, base_name);

	    எனில் (auto_name != வெற்று)
	    {
		முழு free_auto_name = TRUE;
		ht = &globvarht;
		ret = find_var_in_ht(ht, 'g', auto_name, TRUE);
		எனில் (ret != வெற்று)
		{
		    எனில் (htp != வெற்று)
			*htp = ht;
		    எனில் (namep != வெற்று)
		    {
			free_auto_name = FALSE;
			*namep = auto_name;
		    }
		}
		எனில் (free_auto_name)
		    vim_free(auto_name);
	    }
	}
    }

    திரும்பு ret;
}

/*
 * Like find_var() but if the name starts with <SNR>99_ then look in the
 * referenced script (used for a funcref).
 */
    dictitem_T *
find_var_also_in_script(char_u *name, hashtab_T **htp, முழு no_autoload)
{
    எனில் (STRNCMP(name, "<SNR>", 5) == 0 && SAFE_isdigit(name[5]))
    {
	char_u	    *p = name + 5;
	முழு	    sid = getdigits(&p);

	எனில் (SCRIPT_ID_VALID(sid) && *p == '_')
	{
	    hashtab_T	*ht = &SCRIPT_VARS(sid);

	    எனில் (ht != வெற்று)
	    {
		dictitem_T *di = find_var_in_ht(ht, 0, p + 1, no_autoload);

		எனில் (di != வெற்று)
		{
		    எனில் (htp != வெற்று)
			*htp = ht;
		    திரும்பு di;
		}
	    }
	}
    }

    திரும்பு find_var(name, htp, no_autoload);
}

/*
 * Find variable "varname" in hashtab "ht" with name "htname".
 * When "varname" is empty returns curwin/curtab/etc vars dictionary.
 * Returns NULL if not found.
 */
    dictitem_T *
find_var_in_ht(
    hashtab_T	*ht,
    முழு		htname,
    char_u	*varname,
    முழு		no_autoload)
{
    hashitem_T	*hi;

    எனில் (*varname == NUL)
    {
	// Must be something like "s:", otherwise "ht" would be NULL.
	தேர்வு (htname)
	{
	    நிலை 's': திரும்பு &SCRIPT_SV(current_sctx.sc_sid)->sv_var;
	    நிலை 'g': திரும்பு &globvars_var;
	    நிலை 'v': திரும்பு &vimvars_var;
	    நிலை 'b': திரும்பு &curbuf->b_bufvar;
	    நிலை 'w': திரும்பு &curwin->w_winvar;
	    நிலை 't': திரும்பு &curtab->tp_winvar;
	    நிலை 'l': திரும்பு get_funccal_local_var();
	    நிலை 'a': திரும்பு get_funccal_args_var();
	}
	திரும்பு வெற்று;
    }

    hi = hash_find(ht, varname);
    எனில் (HASHITEM_EMPTY(hi))
    {
	// For global variables we may try auto-loading the script.  If it
	// worked find the variable again.  Don't auto-load a script if it was
	// loaded already, otherwise it would be loaded every time when
	// checking if a function name is a Funcref variable.
	எனில் (ht == &globvarht && !no_autoload)
	{
	    // Note: script_autoload() may make "hi" invalid. It must either
	    // be obtained again or not used.
	    எனில் (!script_autoload(varname, FALSE) || aborting())
		திரும்பு வெற்று;
	    hi = hash_find(ht, varname);
	}
	எனில் (HASHITEM_EMPTY(hi))
	    திரும்பு வெற்று;
    }
    திரும்பு HI2DI(hi);
}

/*
 * Get the script-local hashtab.  NULL if not in a script context.
 */
    hashtab_T *
get_script_local_ht(வெறுமை)
{
    scid_T sid = current_sctx.sc_sid;

    எனில் (SCRIPT_ID_VALID(sid))
	திரும்பு &SCRIPT_VARS(sid);
    திரும்பு வெற்று;
}

/*
 * Look for "name[len]" in script-local variables and functions.
 * When "cmd" is TRUE it must look like a command, a function must be followed
 * by "(" or "->".
 * Return OK when found, FAIL when not found.
 */
    முழு
lookup_scriptitem(
	char_u	*name,
	size_t	len,
	முழு	cmd,
	cctx_T	*dummy UNUSED)
{
    hashtab_T	*ht = get_script_local_ht();
    char_u	buffer[30];
    char_u	*p;
    முழு		res;
    hashitem_T	*hi;
    முழு		is_global = FALSE;
    char_u	*fname = name;

    எனில் (ht == வெற்று)
	திரும்பு FAIL;
    எனில் (len < அளவு(buffer) - 1)
    {
	// avoid an alloc/free for short names
	vim_strncpy(buffer, name, len);
	p = buffer;
    }
    இல்லையெனில்
    {
	p = vim_strnsave(name, len);
	எனில் (p == வெற்று)
	    திரும்பு FAIL;
    }

    hi = hash_find(ht, p);
    res = HASHITEM_EMPTY(hi) ? FAIL : OK;

    // if not script-local, then perhaps autoload-exported
    எனில் (res == FAIL && find_var_autoload_prefix(p, 0, வெற்று, வெற்று) != வெற்று)
	res = OK;

    // if not script-local or autoload, then perhaps imported
    எனில் (res == FAIL && find_imported(p, 0, FALSE) != வெற்று)
	res = OK;
    எனில் (p != buffer)
	vim_free(p);

    // Find a function, so that a following "->" works.
    // When used as a command require "(" or "->" to follow, "Cmd" is a user
    // command while "Cmd()" is a function call.
    எனில் (res != OK)
    {
	p = skipwhite(name + len);

	எனில் (!cmd || name[len] == '(' || (p[0] == '-' && p[1] == '>'))
	{
	    // Do not check for an internal function, since it might also be a
	    // valid command, such as ":split" versus "split()".
	    // Skip "g:" before a function name.
	    எனில் (name[0] == 'g' && name[1] == ':')
	    {
		is_global = TRUE;
		fname = name + 2;
	    }
	    எனில் (find_func(fname, is_global) != வெற்று)
		res = OK;
	}
    }

    திரும்பு res;
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

    எனில் (name[0] == NUL)
	திரும்பு வெற்று;
    எனில் (name[1] != ':')
    {
	// The name must not start with a colon or #.
	எனில் (name[0] == ':' || name[0] == AUTOLOAD_CHAR)
	    திரும்பு வெற்று;
	*varname = name;

	// "version" is "v:version" in all scopes if scriptversion < 3.
	// Same for a few other variables marked with VV_COMPAT.
	எனில் (in_old_script(3))
	{
	    hi = hash_find(&compat_hashtab, name);
	    எனில் (!HASHITEM_EMPTY(hi))
		திரும்பு &compat_hashtab;
	}

	ht = get_funccal_local_ht();
	எனில் (ht != வெற்று)
	    திரும்பு ht;				// local variable

	// In Vim9 script items at the script level are script-local, except
	// for autoload names.
	எனில் (in_vim9script() && vim_strchr(name, AUTOLOAD_CHAR) == வெற்று)
	{
	    ht = get_script_local_ht();
	    எனில் (ht != வெற்று)
		திரும்பு ht;
	}

	திரும்பு &globvarht;			// global variable
    }
    *varname = name + 2;
    எனில் (*name == 'g')				// global variable
	திரும்பு &globvarht;
    // There must be no ':' or '#' in the rest of the name, unless g: is used
    எனில் (vim_strchr(name + 2, ':') != வெற்று
			       || vim_strchr(name + 2, AUTOLOAD_CHAR) != வெற்று)
	திரும்பு வெற்று;
    எனில் (*name == 'b')				// buffer variable
	திரும்பு &curbuf->b_vars->dv_hashtab;
    எனில் (*name == 'w')				// window variable
	திரும்பு &curwin->w_vars->dv_hashtab;
    எனில் (*name == 't')				// tab page variable
	திரும்பு &curtab->tp_vars->dv_hashtab;
    எனில் (*name == 'v')				// v: variable
	திரும்பு &vimvarht;
    எனில் (get_current_funccal() != வெற்று
	       && get_current_funccal()->fc_func->uf_def_status
							    == UF_NOT_COMPILED)
    {
	// a: and l: are only used in functions defined with ":function"
	எனில் (*name == 'a')			// a: function argument
	    திரும்பு get_funccal_args_ht();
	எனில் (*name == 'l')			// l: local function variable
	    திரும்பு get_funccal_local_ht();
    }
    எனில் (*name == 's')				// script variable
    {
	ht = get_script_local_ht();
	எனில் (ht != வெற்று)
	    திரும்பு ht;
    }
    திரும்பு வெற்று;
}

/*
 * Get the string value of a (global/local) variable.
 * Note: see tv_get_string() for how long the pointer remains valid.
 * Returns NULL when it doesn't exist.
 */
    char_u *
get_var_மதிப்பு(char_u *name)
{
    dictitem_T	*v;

    v = find_var(name, வெற்று, FALSE);
    எனில் (v == வெற்று)
	திரும்பு வெற்று;
    திரும்பு tv_get_string(&v->di_tv);
}

/*
 * Allocate a new hashtab for a sourced script.  It will be used while
 * sourcing this script and when executing functions defined in the script.
 */
    வெறுமை
new_script_vars(scid_T id)
{
    scriptvar_T *sv;

    sv = ALLOC_CLEAR_ONE(scriptvar_T);
    எனில் (sv == வெற்று)
	திரும்பு;
    init_var_dict(&sv->sv_dict, &sv->sv_var, VAR_SCOPE);
    SCRIPT_ITEM(id)->sn_vars = sv;
}

/*
 * Initialize dictionary "dict" as a scope and set variable "dict_var" to
 * point to it.
 */
    வெறுமை
init_var_dict(dict_T *dict, dictitem_T *dict_var, முழு scope)
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
    dict_var->di_சாவி[0] = NUL;
}

/*
 * Unreference a dictionary initialized by init_var_dict().
 */
    வெறுமை
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
    வெறுமை
vars_clear(hashtab_T *ht)
{
    vars_clear_ext(ht, TRUE);
}

/*
 * Like vars_clear(), but only free the value if "free_val" is TRUE.
 */
    வெறுமை
vars_clear_ext(hashtab_T *ht, முழு free_val)
{
    முழு		todo;
    hashitem_T	*hi;
    dictitem_T	*v;

    hash_lock(ht);
    todo = (முழு)ht->ht_used;
    FOR_ALL_HASHTAB_பொருட்கள்(ht, hi, todo)
    {
	எனில் (!HASHITEM_EMPTY(hi))
	{
	    --todo;

	    // Free the variable.  Don't remove it from the hashtab,
	    // ht_array might change then.  hash_clear() takes care of it
	    // later.
	    v = HI2DI(hi);
	    எனில் (free_val)
		clear_tv(&v->di_tv);
	    எனில் (v->di_flags & DI_FLAGS_ALLOC)
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
    வெறுமை
delete_var(hashtab_T *ht, hashitem_T *hi)
{
    dictitem_T	*di = HI2DI(hi);

    எனில் (hash_remove(ht, hi, "delete variable") != OK)
	திரும்பு;

    clear_tv(&di->di_tv);
    vim_free(di);
}

/*
 * Delete the exported variables of a reloaded Vim9 autoload script.  They live
 * in the global namespace with the autoload prefix (e.g. "foo#bar") and are
 * recreated when the script body runs again.
 */
    வெறுமை
delete_autoload_export_vars(char_u *prefix)
{
    எனில் (prefix == வெற்று)
	திரும்பு;
    size_t  prefixlen = STRLEN(prefix);

    hash_lock(&globvarht);
    முழு	todo = (முழு)globvarht.ht_used;

    க்கு (hashitem_T *hi = globvarht.ht_array; todo > 0; ++hi)
	எனில் (!HASHITEM_EMPTY(hi))
	{
	    dictitem_T	*di = HI2DI(hi);

	    --todo;
	    // Keep a class or enum: existing objects still refer to it.
	    எனில் (di->di_tv.v_type != VAR_CLASS
		    && STRNCMP(di->di_சாவி, prefix, prefixlen) == 0)
		delete_var(&globvarht, hi);
	}
    hash_unlock(&globvarht);
}

/*
 * List the value of one internal variable.
 */
    நிலையான வெறுமை
list_one_var(dictitem_T *v, எழுத்து *prefix, முழு *first)
{
    char_u	*tofree;
    char_u	*s;
    char_u	numbuf[NUMBUFLEN];

    s = echo_string(&v->di_tv, &tofree, numbuf, get_copyID());
    list_one_var_a(prefix, v->di_சாவி, v->di_tv.v_type,
					 s == வெற்று ? (char_u *)"" : s, first);
    vim_free(tofree);
}

    நிலையான வெறுமை
list_one_var_a(
    எழுத்து	*prefix,
    char_u	*name,
    முழு		type,
    char_u	*string,
    முழு		*first)  // when TRUE clear rest of screen and set to FALSE
{
    // don't use msg() or msg_attr() to avoid overwriting "v:statusmsg"
    செய்தி_start();
    செய்தி_puts(prefix);
    எனில் (name != வெற்று)	// "a:" vars don't have a name stored
	செய்தி_puts((எழுத்து *)name);
    செய்தி_putchar(' ');
    செய்தி_advance(22);
    எனில் (type == VAR_எண்)
	செய்தி_putchar('#');
    இல்லையெனில் எனில் (type == VAR_FUNC || type == VAR_PARTIAL)
	செய்தி_putchar('*');
    இல்லையெனில் எனில் (type == VAR_LIST)
    {
	செய்தி_putchar('[');
	எனில் (*string == '[')
	    ++string;
    }
    இல்லையெனில் எனில் (type == VAR_DICT)
    {
	செய்தி_putchar('{');
	எனில் (*string == '{')
	    ++string;
    }
    இல்லையெனில்
	செய்தி_putchar(' ');

    செய்தி_outtrans(string);

    எனில் (type == VAR_FUNC || type == VAR_PARTIAL)
	செய்தி_puts("()");
    எனில் (*first)
    {
	செய்தி_clr_eos();
	*first = FALSE;
    }
}

/*
 * Addition handling for setting a v: variable.
 * Return TRUE if the variable should be set normally,
 *        FALSE if nothing else needs to be done.
 */
    முழு
before_set_vvar(
    char_u	*varname,
    dictitem_T	*di,
    typval_T	*tv,
    முழு		copy,
    முழு		*type_error)
{
    எனில் (di->di_tv.v_type == VAR_STRING)
    {
	VIM_CLEAR(di->di_tv.vval.v_string);
	எனில் (copy || tv->v_type != VAR_STRING)
	{
	    char_u *val = tv_get_string(tv);

	    // Careful: when assigning to v:errmsg and
	    // tv_get_string() causes an error message the variable
	    // will already be set.
	    எனில் (di->di_tv.vval.v_string == வெற்று)
		di->di_tv.vval.v_string = vim_strsave(val);
	}
	இல்லையெனில்
	{
	    // Take over the string to avoid an extra alloc/free.
	    di->di_tv.vval.v_string = tv->vval.v_string;
	    tv->vval.v_string = வெற்று;
	}
	திரும்பு FALSE;
    }
    இல்லையெனில் எனில் (di->di_tv.v_type == VAR_எண்)
    {
	di->di_tv.vval.v_எண் = tv_get_எண்(tv);
	எனில் (STRCMP(varname, "searchforward") == 0)
	    set_தேடல்_direction(di->di_tv.vval.v_எண் ? '/' : '?');
#ifdef FEAT_தேடல்_EXTRA
	இல்லையெனில் எனில் (STRCMP(varname, "hlsearch") == 0)
	{
	    no_hlsearch = !di->di_tv.vval.v_எண்;
	    redraw_all_later(UPD_SOME_VALID);
	}
#endif
	திரும்பு FALSE;
    }
    இல்லையெனில் எனில் (di->di_tv.v_type != tv->v_type)
    {
	*type_error = TRUE;
	திரும்பு FALSE;
    }
    திரும்பு TRUE;
}

/*
 * Set variable "name" to value in "tv".
 * If the variable already exists, the value is updated.
 * Otherwise the variable is created.
 */
    வெறுமை
set_var(
    char_u	*name,
    typval_T	*tv,
    முழு		copy)	    // make copy of value in "tv"
{
    set_var_const(name, 0, வெற்று, tv, copy, ASSIGN_DECL, 0);
}

/*
 * Set variable "name" to value in "tv_arg".
 * When "sid" is non-zero "name" is in the script with this ID.
 * If the variable already exists and "is_const" is FALSE the value is updated.
 * Otherwise the variable is created.
 */
    முழு
set_var_const(
    char_u	*name,
    scid_T	sid,
    type_T	*type_arg,
    typval_T	*tv_arg,
    முழு		copy,	    // make copy of value in "tv"
    முழு		flags_arg,  // ASSIGN_CONST, ASSIGN_FINAL, etc.
    முழு		var_idx)    // index for ":let [a, b] = list"
{
    typval_T	*tv = tv_arg;
    type_T	*type = type_arg;
    typval_T	bool_tv;
    dictitem_T	*di;
    typval_T	*dest_tv;
    char_u	*varname;
    char_u	*name_tofree = வெற்று;
    hashtab_T	*ht = வெற்று;
    முழு		is_script_local;
    முழு		vim9script = in_vim9script();
    முழு		var_in_vim9script;
    முழு		var_in_autoload = FALSE;
    முழு		flags = flags_arg;
    முழு		free_tv_arg = !copy;  // free tv_arg if not used
    முழு		rc = FAIL;

    எனில் (sid != 0)
    {
	varname = வெற்று;
	எனில் (SCRIPT_ID_VALID(sid))
	{
	    char_u	*auto_name = வெற்று;
	    எனில் (find_var_autoload_prefix(name, sid, &ht, &auto_name) != வெற்று)
	    {
		var_in_autoload = TRUE;
		varname = auto_name;
		name_tofree = varname;
	    }
	    இல்லையெனில்
		ht = &SCRIPT_VARS(sid);
	}
	எனில் (varname == வெற்று)
	    varname = name;
    }
    இல்லையெனில்
    {
	scriptitem_T	*si;
	char_u		*auto_name = வெற்று;

	எனில் (in_vim9script()
	    && SCRIPT_ID_VALID(current_sctx.sc_sid)
	    && (si = SCRIPT_ITEM(current_sctx.sc_sid))
					      ->sn_autoload_prefix != வெற்று
	    && (is_export
		|| find_var_autoload_prefix(name, 0, வெற்று, &auto_name)
								    != வெற்று))
	{
	    // In a vim9 autoload script an exported variable is put in the
	    // global namespace with the autoload prefix.
	    var_in_autoload = TRUE;
	    varname = auto_name != வெற்று ? auto_name
		      : concat_str(si->sn_autoload_prefix, name);
	    எனில் (varname == வெற்று)
		செல் failed;
	    name_tofree = varname;
	    ht = &globvarht;
	}
	இல்லையெனில்
	    ht = find_var_ht(name, &varname);
    }
    எனில் (ht == வெற்று || *varname == NUL)
    {
	semsg(_(e_illegal_variable_name_str), name);
	செல் failed;
    }
    is_script_local = ht == get_script_local_ht() || sid != 0
							    || var_in_autoload;

    எனில் (vim9script
	    && !is_script_local
	    && (flags & (ASSIGN_NO_DECL | ASSIGN_DECL)) == 0
	    && (flags & (ASSIGN_CONST | ASSIGN_FINAL)) == 0
	    && name[1] == ':')
    {
	vim9_declare_error(name);
	செல் failed;
    }
    எனில் ((flags & ASSIGN_FOR_LOOP) && is_scoped_variable(name))
	// Do not make g:var, w:var, b:var or t:var final.
	flags &= ~ASSIGN_FINAL;

    var_in_vim9script = is_script_local && current_script_is_vim9();
    எனில் (var_in_vim9script && name[0] == '_' && name[1] == NUL)
    {
	// For "[a, _] = list" the underscore is ignored.
	எனில் ((flags & ASSIGN_UNPACK) == 0)
	    emsg(_(e_cannot_use_underscore_here));
	செல் failed;
    }

    di = find_var_in_ht(ht, 0, varname, TRUE);

    எனில் (di == வெற்று && var_in_vim9script)
    {
	imported_T  *import = find_imported(varname, 0, FALSE);

	எனில் (import != வெற்று)
	{
	    // imported name space cannot be used
	    எனில் ((flags & ASSIGN_NO_DECL) == 0)
	    {
		semsg(_(e_redefining_imported_item_str), name);
		செல் failed;
	    }
	    semsg(_(e_cannot_use_str_itself_it_is_imported), name);
	    செல் failed;
	}
	எனில் (!in_vim9script())
	{
	    semsg(_(e_cannot_create_vim9_script_variable_in_function_str),
									 name);
	    செல் failed;
	}
    }

    // Search in parent scope which is possible to reference from lambda
    எனில் (di == வெற்று)
	di = find_var_in_scoped_ht(name, TRUE);

    எனில் ((tv->v_type == VAR_FUNC || tv->v_type == VAR_PARTIAL)
				  && var_wrong_func_name(name, di == வெற்று))
	செல் failed;

    எனில் (need_convert_to_bool(type, tv))
    {
	// Destination is a bool and the value is not, but it can be
	// converted.
	CLEAR_FIELD(bool_tv);
	bool_tv.v_type = VAR_BOOL;
	bool_tv.vval.v_எண் = tv2bool(tv) ? VVAL_TRUE : VVAL_FALSE;
	tv = &bool_tv;
    }

    எனில் (di != வெற்று)
    {
	// Item already exists.  Allowed to replace when reloading.
	எனில் ((di->di_flags & DI_FLAGS_RELOAD) == 0)
	{
	    எனில் ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					 && (flags & ASSIGN_FOR_LOOP) == 0)
	    {
		emsg(_(e_cannot_modify_existing_variable));
		செல் failed;
	    }

	    எனில் (is_script_local && vim9script
			  && (flags & (ASSIGN_NO_DECL | ASSIGN_DECL)) == 0)
	    {
		semsg(_(e_redefining_script_item_str), name);
		செல் failed;
	    }

	    எனில் (check_typval_is_மதிப்பு(&di->di_tv) == FAIL)
		செல் failed;

	    // List and Blob types can be modified in-place using the "+="
	    // compound operator.  For other types, this is not allowed.
	    முழு type_inplace_modifiable =
		(di->di_tv.v_type == VAR_LIST || di->di_tv.v_type == VAR_BLOB);

	    எனில் (var_in_vim9script && (flags & ASSIGN_FOR_LOOP) == 0
		    && ((flags & ASSIGN_COMPOUND_OP) == 0
			|| !type_inplace_modifiable))
	    {
		where_T where = WHERE_INIT;
		svar_T  *sv = find_typval_in_script(&di->di_tv, sid, TRUE);

		எனில் (sv != வெற்று)
		{
		    // check the type and adjust to bool if needed
		    எனில் (var_idx > 0)
		    {
			where.wt_index = var_idx;
			where.wt_kind = WT_VARIABLE;
		    }
		    எனில் (check_script_var_type(sv, tv, name, where) == FAIL)
			செல் failed;
		    எனில் (type == வெற்று)
			type = sv->sv_type;
		    sv->sv_flags |= SVFLAG_ASSIGNED;
		}
	    }

	    // Modifying a final variable with a List value using the "+="
	    // operator is allowed.  For other types, it is not allowed.
	    எனில் ((((flags & ASSIGN_FOR_LOOP) == 0 || (flags & ASSIGN_DECL) == 0)
			&& ((flags & ASSIGN_COMPOUND_OP) == 0
			    || !type_inplace_modifiable))
				 ? var_check_permission(di, name) == FAIL
				 : var_check_ro(di->di_flags, name, FALSE))
		செல் failed;
	}
	இல்லையெனில்
	{
	    // can only redefine once
	    di->di_flags &= ~DI_FLAGS_RELOAD;

	    // A Vim9 script-local variable is also present in sn_all_vars
	    // and sn_var_vals.  It may set "type" from "tv".
	    எனில் (var_in_vim9script || var_in_autoload)
		update_vim9_script_var(FALSE, di,
			var_in_autoload ? name : di->di_சாவி, flags,
			tv, &type, (flags & ASSIGN_NO_MEMBER_TYPE) == 0);
	}

	// existing variable, need to clear the value

	// In Vim9 script a String v: variable only takes a String.
	எனில் (ht == &vimvarht && di->di_tv.v_type == VAR_STRING
							    && in_vim9script())
	{
	    where_T	where = WHERE_INIT;

	    எனில் (check_typval_type(&t_string, tv, where) == FAIL)
		செல் failed;
	}

	// Handle setting internal v: variables separately where needed to
	// prevent changing the type.
	முழு type_error = FALSE;
	எனில் (ht == &vimvarht
		&& !before_set_vvar(varname, di, tv, copy, &type_error))
	{
	    எனில் (type_error)
		semsg(_(e_setting_v_str_to_மதிப்பு_with_wrong_type), varname);
	    செல் failed;
	}

	clear_tv(&di->di_tv);

	எனில் ((flags & ASSIGN_UPDATE_BLOCK_ID)
				       && SCRIPT_ID_VALID(current_sctx.sc_sid))
	{
	    scriptitem_T *si = SCRIPT_ITEM(current_sctx.sc_sid);

	    update_script_var_block_id(name, si->sn_current_block_id);
	}
    }
    இல்லையெனில்
    {
	// Item not found, check if a function already exists.
	எனில் (is_script_local && (flags & (ASSIGN_NO_DECL | ASSIGN_DECL)) == 0
	       && lookup_scriptitem(name, STRLEN(name), FALSE, வெற்று) == OK)
	{
	    semsg(_(e_redefining_script_item_str), name);
	    செல் failed;
	}

	// add a new variable
	எனில் (var_in_vim9script && (flags & ASSIGN_NO_DECL))
	{
	    semsg(_(e_unknown_variable_str), name);
	    செல் failed;
	}

	எனில் (check_hashtab_frozen(ht, "add variable"))
	    செல் failed;

	// Can't add "v:" or "a:" variable.
	எனில் (ht == &vimvarht || ht == get_funccal_args_ht())
	{
	    semsg(_(e_illegal_variable_name_str), name);
	    செல் failed;
	}

	// Make sure the variable name is valid.  In Vim9 script an
	// autoload variable must be prefixed with "g:" unless in an
	// autoload script.
	எனில் (!valid_varname(varname, -1, !vim9script
			|| STRNCMP(name, "g:", 2) == 0 || var_in_autoload))
	    செல் failed;

	di = alloc(offsetof(dictitem_T, di_சாவி) + STRLEN(varname) + 1);
	எனில் (di == வெற்று)
	    செல் failed;
	STRCPY(di->di_சாவி, varname);
	எனில் (hash_add(ht, DI2HIKEY(di), "add variable") == FAIL)
	{
	    vim_free(di);
	    செல் failed;
	}
	di->di_flags = DI_FLAGS_ALLOC;
	எனில் (flags & (ASSIGN_CONST | ASSIGN_FINAL))
	    di->di_flags |= DI_FLAGS_LOCK;

	// A Vim9 script-local variable is also added to sn_all_vars and
	// sn_var_vals. It may set "type" from "tv".
	எனில் (var_in_vim9script || var_in_autoload)
	    update_vim9_script_var(TRUE, di,
		    var_in_autoload ? name : di->di_சாவி, flags,
			  tv, &type, (flags & ASSIGN_NO_MEMBER_TYPE) == 0);
    }

    dest_tv = &di->di_tv;
    எனில் (copy || tv->v_type == VAR_எண் || tv->v_type == VAR_FLOAT)
	copy_tv(tv, dest_tv);
    இல்லையெனில்
    {
	*dest_tv = *tv;
	dest_tv->v_lock = 0;
	init_tv(tv);
    }
    free_tv_arg = FALSE;

    எனில் (vim9script && type != வெற்று)
	set_tv_type(dest_tv, type);

    // ":const var = value" locks the value
    // ":final var = value" locks "var"
    எனில் (flags & ASSIGN_CONST)
	// Like :lockvar! name: lock the value and what it contains, but only
	// if the reference count is up to one.  That locks only literal
	// values.
	item_lock(dest_tv, DICT_MAXNEST, TRUE, TRUE);

    rc = OK;

failed:
    vim_free(name_tofree);
    எனில் (free_tv_arg)
	clear_tv(tv_arg);

    திரும்பு rc;
}

/*
 * Check in this order for backwards compatibility:
 * - Whether the variable is read-only
 * - Whether the variable value is locked
 * - Whether the variable is locked
 * NOTE: "name" is only used for error messages.
 */
    முழு
var_check_permission(dictitem_T *di, char_u *name)
{
    எனில் (var_check_ro(di->di_flags, name, FALSE)
		    || மதிப்பு_check_lock(di->di_tv.v_lock, name, FALSE)
		    || var_check_lock(di->di_flags, name, FALSE))
	திரும்பு FAIL;
    திரும்பு OK;
}

/*
 * Return TRUE if di_flags "flags" indicates variable "name" is read-only.
 * Also give an error message.
 */
    முழு
var_check_ro(முழு flags, char_u *name, முழு use_gettext)
{
    எனில் (flags & DI_FLAGS_RO)
    {
	எனில் (name == வெற்று)
	    emsg(_(e_cannot_change_readonly_variable));
	இல்லையெனில்
	    semsg(_(e_cannot_change_readonly_variable_str),
				       use_gettext ? (char_u *)_(name) : name);
	திரும்பு TRUE;
    }
    எனில் ((flags & DI_FLAGS_RO_SBX) && sandbox)
    {
	எனில் (name == வெற்று)
	    emsg(_(e_cannot_set_variable_in_sandbox));
	இல்லையெனில்
	    semsg(_(e_cannot_set_variable_in_sandbox_str),
				       use_gettext ? (char_u *)_(name) : name);
	திரும்பு TRUE;
    }
    திரும்பு FALSE;
}

/*
 * Return TRUE if di_flags "flags" indicates variable "name" is locked.
 * Also give an error message.
 */
    முழு
var_check_lock(முழு flags, char_u *name, முழு use_gettext)
{
    எனில் (flags & DI_FLAGS_LOCK)
    {
	semsg(_(e_variable_is_locked_str),
				       use_gettext ? (char_u *)_(name) : name);
	திரும்பு TRUE;
    }
    திரும்பு FALSE;
}

/*
 * Return TRUE if di_flags "flags" indicates variable "name" is fixed.
 * Also give an error message.
 */
    முழு
var_check_fixed(முழு flags, char_u *name, முழு use_gettext)
{
    எனில் (flags & DI_FLAGS_FIX)
    {
	எனில் (name == வெற்று)
	    emsg(_(e_cannot_delete_variable));
	இல்லையெனில்
	    semsg(_(e_cannot_delete_variable_str),
				      use_gettext ? (char_u *)_(name) : name);
	திரும்பு TRUE;
    }
    திரும்பு FALSE;
}

/*
 * Check if a funcref is assigned to a valid variable name.
 * Return TRUE and give an error if not.
 */
    முழு
var_wrong_func_name(
    char_u *name,    // points to start of variable name
    முழு    new_var)  // TRUE when creating the variable
{
    // Allow for w: b: s: and t:.  In Vim9 script s: is not allowed, because
    // the name can be used without the s: prefix.
    // Allow autoload variable.
    எனில் (!((vim_strchr((char_u *)"wbt", name[0]) != வெற்று
		    || (!in_vim9script() && name[0] == 's')) && name[1] == ':')
	    && !ASCII_ISUPPER((name[0] != NUL && name[1] == ':')
						     ? name[2] : name[0])
	    && vim_strchr(name, '#') == வெற்று)
    {
	semsg(_(e_funcref_variable_name_must_start_with_capital_str), name);
	திரும்பு TRUE;
    }
    // Don't allow hiding a function.  When "v" is not NULL we might be
    // assigning another function to the same var, the type is checked
    // below.
    எனில் (new_var && function_exists(name, FALSE))
    {
	semsg(_(e_variable_name_conflicts_with_existing_function_str),
								    name);
	திரும்பு TRUE;
    }
    திரும்பு FALSE;
}

/*
 * Return TRUE if "flags" indicates variable "name" has a locked (immutable)
 * value.  Also give an error message, using "name" or _("name") when
 * "use_gettext" is TRUE.
 */
    முழு
மதிப்பு_check_lock(முழு lock, char_u *name, முழு use_gettext)
{
    எனில் (lock & VAR_LOCKED)
    {
	எனில் (name == வெற்று)
	    emsg(_(e_மதிப்பு_is_locked));
	இல்லையெனில்
	    semsg(_(e_மதிப்பு_is_locked_str),
				       use_gettext ? (char_u *)_(name) : name);
	திரும்பு TRUE;
    }
    எனில் (lock & VAR_FIXED)
    {
	எனில் (name == வெற்று)
	    emsg(_(e_cannot_change_மதிப்பு));
	இல்லையெனில்
	    semsg(_(e_cannot_change_மதிப்பு_of_str),
				       use_gettext ? (char_u *)_(name) : name);
	திரும்பு TRUE;
    }
    திரும்பு FALSE;
}

/*
 * Check if a variable name is valid.  When "autoload" is true "#" is allowed.
 * If "len" is -1 use all of "varname", otherwise up to "varname[len]".
 * Return FALSE and give an error if not.
 */
    முழு
valid_varname(char_u *varname, முழு len, முழு autoload)
{
    char_u *p;

    க்கு (p = varname; len < 0 ? *p != NUL : p < varname + len; ++p)
	எனில் (!eval_isnamec1(*p) && (p == varname || !VIM_ISDIGIT(*p))
					 && !(autoload && *p == AUTOLOAD_CHAR))
	{
	    semsg(_(e_illegal_variable_name_str), varname);
	    திரும்பு FALSE;
	}
    திரும்பு TRUE;
}

/*
 * Implements the logic to retrieve local variable and option values.
 * Used by "getwinvar()" "gettabvar()" "gettabwinvar()" "getbufvar()".
 */
    நிலையான வெறுமை
get_var_from(
    char_u	*varname,
    typval_T	*rettv,
    typval_T	*deftv,	    // Default value if not found.
    முழு		htname,	    // 't'ab, 'w'indow or 'b'uffer local.
    tabpage_T	*tp,	    // can be NULL
    win_T	*win,
    buf_T	*buf)	    // Ignored if htname is not 'b'.
{
    dictitem_T	*v;
    முழு		done = FALSE;
    switchwin_T	switchwin;
    முழு		need_switch_win;
    முழு		do_change_curbuf = buf != வெற்று && htname == 'b';

    ++emsg_off;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = வெற்று;

    எனில் (varname != வெற்று && tp != வெற்று && win != வெற்று
	    && (htname != 'b' || buf != வெற்று))
    {
	// Set curwin to be our win, temporarily.  Also set the tabpage,
	// otherwise the window is not valid. Only do this when needed,
	// autocommands get blocked.
	// If we have a buffer reference avoid the switching, we're saving and
	// restoring curbuf directly.
	need_switch_win = !(tp == curtab && win == curwin) && !do_change_curbuf;
	எனில் (!need_switch_win || switch_win(&switchwin, win, tp, TRUE) == OK)
	{
	    // Handle options. There are no tab-local options.
	    எனில் (*varname == '&' && htname != 't')
	    {
		buf_T	*save_curbuf = curbuf;

		// Change curbuf so the option is read from the correct buffer.
		எனில் (do_change_curbuf)
		    curbuf = buf;

		எனில் (varname[1] == NUL)
		{
		    // get all window-local or buffer-local options in a dict
		    dict_T	*opts = get_winbuf_options(htname == 'b');

		    எனில் (opts != வெற்று)
		    {
			rettv_dict_set(rettv, opts);
			done = TRUE;
		    }
		}
		இல்லையெனில் எனில் (eval_option(&varname, rettv, TRUE) == OK)
		    // Local option
		    done = TRUE;

		curbuf = save_curbuf;
	    }
	    இல்லையெனில் எனில் (*varname == NUL)
	    {
		// Empty string: return a dict with all the local variables.
		எனில் (htname == 'b')
		    v = &buf->b_bufvar;
		இல்லையெனில் எனில் (htname == 'w')
		    v = &win->w_winvar;
		இல்லையெனில்
		    v = &tp->tp_winvar;
		copy_tv(&v->di_tv, rettv);
		done = TRUE;
	    }
	    இல்லையெனில்
	    {
		hashtab_T	*ht;

		எனில் (htname == 'b')
		    ht = &buf->b_vars->dv_hashtab;
		இல்லையெனில் எனில் (htname == 'w')
		    ht = &win->w_vars->dv_hashtab;
		இல்லையெனில்
		    ht = &tp->tp_vars->dv_hashtab;

		// Look up the variable.
		v = find_var_in_ht(ht, htname, varname, FALSE);
		எனில் (v != வெற்று)
		{
		    copy_tv(&v->di_tv, rettv);
		    done = TRUE;
		}
	    }
	}

	எனில் (need_switch_win)
	    // restore previous notion of curwin
	    restore_win(&switchwin, TRUE);
    }

    எனில் (!done && deftv->v_type != VAR_UNKNOWN)
	// use the default value
	copy_tv(deftv, rettv);

    --emsg_off;
}

/*
 * getwinvar() and gettabwinvar()
 */
    நிலையான வெறுமை
getwinvar(
    typval_T	*argvars,
    typval_T	*rettv,
    முழு		off)	    // 1 for gettabwinvar()
{
    char_u	*varname;
    tabpage_T	*tp;
    win_T	*win;

    எனில் (off == 1)
	tp = find_tabpage((முழு)tv_get_எண்_chk(&argvars[0], வெற்று));
    இல்லையெனில்
	tp = curtab;
    win = find_win_by_nr(&argvars[off], tp);
    varname = tv_get_string_chk(&argvars[off + 1]);

    get_var_from(varname, rettv, &argvars[off + 2], 'w', tp, win, வெற்று);
}

/*
 * Set option "varname" to the value of "varp" for the current buffer/window.
 */
    நிலையான வெறுமை
set_option_from_tv(char_u *varname, typval_T *varp)
{
    long	numval = 0;
    char_u	*strval;
    char_u	nbuf[NUMBUFLEN];
    முழு		error = FALSE;

    முழு		opt_idx = findoption(varname);
    எனில் (opt_idx < 0)
    {
	semsg(_(e_unknown_option_str_2), varname);
	திரும்பு;
    }
    முழு		opt_p_flags = get_option_flags(opt_idx);

    எனில் (varp->v_type == VAR_BOOL)
    {
	எனில் (opt_p_flags & P_STRING)
	{
	    emsg(_(e_string_required));
	    திரும்பு;
	}
	numval = (long)varp->vval.v_எண்;
	strval = (char_u *)"0";  // avoid using "false"
    }
    இல்லையெனில்
    {
	எனில் ((opt_p_flags & (P_இலக்கம்|P_BOOL))
		&& (!in_vim9script() || varp->v_type != VAR_STRING))
	    numval = (long)tv_get_எண்_chk(varp, &error);
	எனில் (!error)
	    strval = tv_get_string_buf_chk(varp, nbuf);
    }
    எனில் (!error && strval != வெற்று)
	set_option_மதிப்பு_give_err(varname, numval, strval, OPT_LOCAL);
}

/*
 * "setwinvar()" and "settabwinvar()" functions
 */
    நிலையான வெறுமை
setwinvar(typval_T *argvars, முழு off)
{
    win_T	*win;
    switchwin_T	switchwin;
    முழு		need_switch_win;
    char_u	*varname, *winvarname;
    typval_T	*varp;
    tabpage_T	*tp = வெற்று;

    எனில் (check_secure())
	திரும்பு;

    எனில் (off == 1)
	tp = find_tabpage((முழு)tv_get_எண்_chk(&argvars[0], வெற்று));
    இல்லையெனில்
	tp = curtab;
    win = find_win_by_nr(&argvars[off], tp);
    varname = tv_get_string_chk(&argvars[off + 1]);
    varp = &argvars[off + 2];

    எனில் (win == வெற்று || varname == வெற்று)
	திரும்பு;

    need_switch_win = !(tp == curtab && win == curwin);
    எனில் (!need_switch_win
	    || switch_win(&switchwin, win, tp, TRUE) == OK)
    {
	எனில் (*varname == '&')
	    set_option_from_tv(varname + 1, varp);
	இல்லையெனில்
	{
	    winvarname = alloc(STRLEN(varname) + 3);
	    எனில் (winvarname != வெற்று)
	    {
		STRCPY(winvarname, "w:");
		STRCPY(winvarname + 2, varname);
		set_var(winvarname, varp, TRUE);
		vim_free(winvarname);
	    }
	}
    }
    எனில் (need_switch_win)
	restore_win(&switchwin, TRUE);
}

/*
 * reset v:option_new, v:option_old, v:option_oldlocal, v:option_oldglobal,
 * v:option_type, and v:option_command.
 */
    வெறுமை
reset_v_option_vars(வெறுமை)
{
    set_vim_var_string(VV_OPTION_NEW, வெற்று, -1);
    set_vim_var_string(VV_OPTION_OLD, வெற்று, -1);
    set_vim_var_string(VV_OPTION_OLDLOCAL, வெற்று, -1);
    set_vim_var_string(VV_OPTION_OLDGLOBAL, வெற்று, -1);
    set_vim_var_string(VV_OPTION_TYPE, வெற்று, -1);
    set_vim_var_string(VV_OPTION_COMMAND, வெற்று, -1);
}

/*
 * Add an assert error to v:errors.
 */
    வெறுமை
assert_error(garray_T *gap)
{
    கட்டமைப்பு vimvar   *vp = &vimvars[VV_ERRORS];

    எனில் (vp->vv_tv_type != VAR_LIST || vimvars[VV_ERRORS].vv_list == வெற்று)
	// Make sure v:errors is a list.
	set_vim_var_list(VV_ERRORS, list_alloc());
    list_append_string(vimvars[VV_ERRORS].vv_list, gap->ga_data, gap->ga_len);
}

    முழு
var_exists(char_u *var)
{
    char_u	*arg = var;
    char_u	*name;
    char_u	*tofree;
    typval_T    tv;
    முழு		len = 0;
    முழு		n = FALSE;

    // get_name_len() takes care of expanding curly braces
    name = var;
    len = get_name_len(&arg, &tofree, TRUE, FALSE);
    எனில் (len > 0)
    {
	எனில் (tofree != வெற்று)
	    name = tofree;
	n = (eval_variable(name, len, 0, &tv, வெற்று,
				 EVAL_VAR_NOAUTOLOAD + EVAL_VAR_IMPORT) == OK);
	எனில் (n)
	{
	    // handle d.key, l[idx], f(expr)
	    arg = skipwhite(arg);
	    n = (handle_subscript(&arg, name, &tv, &EVALARG_EVALUATE,
								 FALSE) == OK);
	    எனில் (n)
		clear_tv(&tv);
	}
    }
    எனில் (*arg != NUL)
	n = FALSE;

    vim_free(tofree);
    திரும்பு n;
}

நிலையான lval_T	*redir_lval = வெற்று;
#define EVALCMD_BUSY (redir_lval == (lval_T *)&redir_lval)
நிலையான garray_T redir_ga;	// only valid when redir_lval is not NULL
நிலையான char_u	*redir_endp = வெற்று;
நிலையான char_u	*redir_varname = வெற்று;

    முழு
alloc_redir_lval(வெறுமை)
{
    redir_lval = ALLOC_CLEAR_ONE(lval_T);
    எனில் (redir_lval == வெற்று)
	திரும்பு FAIL;
    திரும்பு OK;
}

    வெறுமை
clear_redir_lval(வெறுமை)
{
    VIM_CLEAR(redir_lval);
}

    வெறுமை
init_redir_ga(வெறுமை)
{
    ga_init2(&redir_ga, அளவு(எழுத்து), 500);
}

/*
 * Start recording command output to a variable
 * When "append" is TRUE append to an existing variable.
 * Returns OK if successfully completed the setup.  FAIL otherwise.
 */
    முழு
var_redir_start(char_u *name, முழு append)
{
    முழு		called_emsg_before;
    typval_T	tv;

    // Catch a bad name early.
    எனில் (!eval_isnamec1(*name))
    {
	emsg(_(e_invalid_argument));
	திரும்பு FAIL;
    }

    // Make a copy of the name, it is used in redir_lval until redir ends.
    redir_varname = vim_strsave(name);
    எனில் (redir_varname == வெற்று)
	திரும்பு FAIL;

    எனில் (alloc_redir_lval() == FAIL)
    {
	var_redir_stop();
	திரும்பு FAIL;
    }

    // The output is stored in growarray "redir_ga" until redirection ends.
    init_redir_ga();

    // Parse the variable name (can be a dict or list entry).
    redir_endp = get_lval(redir_varname, வெற்று, redir_lval, FALSE, FALSE, 0,
							     FNE_CHECK_START);
    எனில் (redir_endp == வெற்று || redir_lval->ll_name == வெற்று || *redir_endp != NUL)
    {
	clear_lval(redir_lval);
	எனில் (redir_endp != வெற்று && *redir_endp != NUL)
	    // Trailing characters are present after the variable name
	    semsg(_(e_trailing_characters_str), redir_endp);
	இல்லையெனில்
	    semsg(_(e_invalid_argument_str), name);
	redir_endp = வெற்று;  // don't store a value, only cleanup
	var_redir_stop();
	திரும்பு FAIL;
    }

    // check if we can write to the variable: set it to or append an empty
    // string
    called_emsg_before = called_emsg;
    tv.v_type = VAR_STRING;
    tv.vval.v_string = (char_u *)"";
    எனில் (append)
	set_var_lval(redir_lval, redir_endp, &tv, TRUE,
					     ASSIGN_NO_DECL, (char_u *)".", 0);
    இல்லையெனில்
	set_var_lval(redir_lval, redir_endp, &tv, TRUE,
					     ASSIGN_NO_DECL, (char_u *)"=", 0);
    clear_lval(redir_lval);
    எனில் (called_emsg > called_emsg_before)
    {
	redir_endp = வெற்று;  // don't store a value, only cleanup
	var_redir_stop();
	திரும்பு FAIL;
    }

    திரும்பு OK;
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
    வெறுமை
var_redir_str(char_u *மதிப்பு, முழு மதிப்பு_len)
{
    முழு		len;

    எனில் (redir_lval == வெற்று)
	திரும்பு;

    எனில் (மதிப்பு_len == -1)
	len = (முழு)STRLEN(மதிப்பு);	// Append the entire string
    இல்லையெனில்
	len = மதிப்பு_len;		// Append only "value_len" characters

    எனில் (ga_grow(&redir_ga, len) == OK)
    {
	mch_memmove((எழுத்து *)redir_ga.ga_data + redir_ga.ga_len, மதிப்பு, len);
	redir_ga.ga_len += len;
    }
    இல்லையெனில்
	var_redir_stop();
}

/*
 * Stop redirecting command output to a variable.
 * Frees the allocated memory.
 */
    வெறுமை
var_redir_stop(வெறுமை)
{
    typval_T	tv;

    எனில் (EVALCMD_BUSY)
    {
	redir_lval = வெற்று;
	திரும்பு;
    }

    எனில் (redir_lval != வெற்று)
    {
	// If there was no error: assign the text to the variable.
	எனில் (redir_endp != வெற்று)
	{
	    ga_append(&redir_ga, NUL);  // Append the trailing NUL.
	    tv.v_type = VAR_STRING;
	    tv.vval.v_string = redir_ga.ga_data;
	    // Call get_lval() again, if it's inside a Dict or List it may
	    // have changed.
	    redir_endp = get_lval(redir_varname, வெற்று, redir_lval,
					FALSE, FALSE, 0, FNE_CHECK_START);
	    எனில் (redir_endp != வெற்று && redir_lval->ll_name != வெற்று)
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
get_clear_redir_ga(வெறுமை)
{
    char_u *res;

    ga_append(&redir_ga, NUL);  // Append the trailing NUL.
    res = redir_ga.ga_data;
    redir_ga.ga_data = வெற்று;
    திரும்பு res;
}

/*
 * "gettabvar()" function
 */
    வெறுமை
f_gettabvar(typval_T *argvars, typval_T *rettv)
{
    char_u	*varname;
    tabpage_T	*tp;
    win_T	*win = வெற்று;

    எனில் (in_vim9script()
	    && (check_for_எண்_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	திரும்பு;

    varname = tv_get_string_chk(&argvars[1]);
    tp = find_tabpage((முழு)tv_get_எண்_chk(&argvars[0], வெற்று));
    எனில் (tp != வெற்று)
	win = tp == curtab || tp->tp_firstwin == வெற்று ? firstwin
		: tp->tp_firstwin;

    get_var_from(varname, rettv, &argvars[2], 't', tp, win, வெற்று);
}

/*
 * "gettabwinvar()" function
 */
    வெறுமை
f_gettabwinvar(typval_T *argvars, typval_T *rettv)
{
    எனில் (in_vim9script()
	    && (check_for_எண்_arg(argvars, 0) == FAIL
		|| check_for_எண்_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL))
	திரும்பு;

    getwinvar(argvars, rettv, 1);
}

/*
 * "getwinvar()" function
 */
    வெறுமை
f_getwinvar(typval_T *argvars, typval_T *rettv)
{
    எனில் (in_vim9script()
	    && (check_for_எண்_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	திரும்பு;

    getwinvar(argvars, rettv, 0);
}

/*
 * "getbufvar()" function
 */
    வெறுமை
f_getbufvar(typval_T *argvars, typval_T *rettv)
{
    char_u	*varname;
    buf_T	*buf;

    எனில் (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	திரும்பு;

    varname = tv_get_string_chk(&argvars[1]);
    buf = tv_get_buf_from_arg(&argvars[0]);

    get_var_from(varname, rettv, &argvars[2], 'b', curtab, curwin, buf);
}

/*
 * "settabvar()" function
 */
    வெறுமை
f_settabvar(typval_T *argvars, typval_T *rettv UNUSED)
{
    tabpage_T	*save_curtab;
    tabpage_T	*tp;
    tabpage_T	*save_lu_tp;
    char_u	*varname, *tabvarname;
    typval_T	*varp;

    எனில் (check_secure())
	திரும்பு;

    எனில் (in_vim9script()
	    && (check_for_எண்_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	திரும்பு;

    tp = find_tabpage((முழு)tv_get_எண்_chk(&argvars[0], வெற்று));
    varname = tv_get_string_chk(&argvars[1]);
    varp = &argvars[2];

    எனில் (varname == வெற்று || tp == வெற்று)
	திரும்பு;

    save_curtab = curtab;
    save_lu_tp = lastused_tabpage;
    goto_tabpage_tp(tp, FALSE, FALSE);

    tabvarname = alloc(STRLEN(varname) + 3);
    எனில் (tabvarname != வெற்று)
    {
	STRCPY(tabvarname, "t:");
	STRCPY(tabvarname + 2, varname);
	set_var(tabvarname, varp, TRUE);
	vim_free(tabvarname);
    }

    // Restore current tabpage and last accessed tabpage.
    எனில் (valid_tabpage(save_curtab))
    {
	goto_tabpage_tp(save_curtab, FALSE, FALSE);
	எனில் (valid_tabpage(save_lu_tp))
	    lastused_tabpage = save_lu_tp;
    }
}

/*
 * "settabwinvar()" function
 */
    வெறுமை
f_settabwinvar(typval_T *argvars, typval_T *rettv UNUSED)
{
    எனில் (in_vim9script()
	    && (check_for_எண்_arg(argvars, 0) == FAIL
		|| check_for_எண்_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL))
	திரும்பு;

    setwinvar(argvars, 1);
}

/*
 * "setwinvar()" function
 */
    வெறுமை
f_setwinvar(typval_T *argvars, typval_T *rettv UNUSED)
{
    எனில் (in_vim9script()
	    && (check_for_எண்_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	திரும்பு;

    setwinvar(argvars, 0);
}

/*
 * "setbufvar()" function
 */
    வெறுமை
f_setbufvar(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    char_u	*varname, *bufvarname;
    typval_T	*varp;

    எனில் (check_secure())
	திரும்பு;

    எனில் (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	திரும்பு;

    varname = tv_get_string_chk(&argvars[1]);
    buf = tv_get_buf_from_arg(&argvars[0]);
    varp = &argvars[2];

    எனில் (buf == வெற்று || varname == வெற்று)
	திரும்பு;

    எனில் (*varname == '&')
    {
	aco_save_T	aco;
	// safe the current window position, it could
	// change because of 'scrollbind' window-local
	// options
	linenr_T	old_topline = curwin->w_topline;

	// Set curbuf to be our buf, temporarily.
	aucmd_prepbuf(&aco, buf);
	எனில் (curbuf == buf)
	{
	    // Only when it worked to set "curbuf".
	    set_option_from_tv(varname + 1, varp);

	    // reset notion of buffer
	    aucmd_restbuf(&aco);
	}
	curwin->w_topline = old_topline;
    }
    இல்லையெனில்
    {
	bufvarname = alloc(STRLEN(varname) + 3);
	எனில் (bufvarname != வெற்று)
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
    முழு		r = OK;

    CLEAR_FIELD(res);
    எனில் (arg->v_type == VAR_PARTIAL && arg->vval.v_partial != வெற்று)
    {
	res.cb_partial = arg->vval.v_partial;
	++res.cb_partial->pt_refcount;
	res.cb_name = partial_name(res.cb_partial);
    }
    இல்லையெனில்
    {
	எனில் (arg->v_type == VAR_STRING && arg->vval.v_string != வெற்று
					       && SAFE_isdigit(*arg->vval.v_string))
	    r = FAIL;
	இல்லையெனில் எனில் (arg->v_type == VAR_FUNC || arg->v_type == VAR_STRING)
	{
	    res.cb_name = arg->vval.v_string;
	    எனில் (arg->v_type == VAR_STRING)
	    {
		char_u *name = get_scriptlocal_funcname(arg->vval.v_string);
		எனில் (name != வெற்று)
		{
		    res.cb_name = name;
		    res.cb_free_name = TRUE;
		}
	    }
	    func_ref(res.cb_name);
	}
	இல்லையெனில் எனில் (arg->v_type == VAR_எண் && arg->vval.v_எண் == 0)
	    res.cb_name = (char_u *)"";
	இல்லையெனில்
	    r = FAIL;

	எனில் (r == FAIL)
	{
	    emsg(_(e_invalid_callback_argument));
	    res.cb_name = வெற்று;
	}
    }
    திரும்பு res;
}

/*
 * Copy a callback into a typval_T.
 */
    வெறுமை
put_callback(callback_T *cb, typval_T *tv)
{
    எனில் (cb->cb_partial != வெற்று)
    {
	tv->v_type = VAR_PARTIAL;
	tv->vval.v_partial = cb->cb_partial;
	++tv->vval.v_partial->pt_refcount;
    }
    இல்லையெனில்
    {
	tv->v_type = VAR_FUNC;
	tv->vval.v_string = vim_strsave(cb->cb_name);
	func_ref(cb->cb_name);
    }
}

    நிலையான bool
does_callback_own_cb_name(callback_T *cb)
{
    // If cb_partial != NULL then *cb->cb_name is owned by the partial.
    திரும்பு cb->cb_partial || cb->cb_free_name;
}

/*
 * Make a copy of "src" into "dest", allocating the function name if needed,
 * without incrementing the refcount.
 */
    வெறுமை
set_callback(callback_T *dest, callback_T *src)
{
    *dest = *src;
    எனில் (!does_callback_own_cb_name(src))
    {
	dest->cb_name = vim_strsave(src->cb_name);
	dest->cb_free_name = TRUE;
    }
    *src = (callback_T){0};
}

/*
 * Copy callback from "src" to "dest", incrementing the refcounts.
 */
    வெறுமை
copy_callback(callback_T *dest, callback_T *src)
{
    dest->cb_partial = src->cb_partial;
    எனில் (dest->cb_partial != வெற்று)
    {
	dest->cb_name = src->cb_name;
	dest->cb_free_name = FALSE;
	++dest->cb_partial->pt_refcount;
    }
    இல்லையெனில்
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
    வெறுமை
expand_autload_callback(callback_T *cb)
{
    char_u	*name;
    char_u	*p;
    imported_T	*import;

    எனில் (!in_vim9script() || cb->cb_name == வெற்று
	    || (!cb->cb_free_name
	       && (cb->cb_partial == வெற்று || cb->cb_partial->pt_name == வெற்று)))
	திரும்பு;
    எனில் (cb->cb_partial != வெற்று)
	name = cb->cb_partial->pt_name;
    இல்லையெனில்
	name = cb->cb_name;
    p = vim_strchr(name, '.');
    எனில் (p == வெற்று)
	திரும்பு;

    import = find_imported(name, p - name, FALSE);
    எனில் (import == வெற்று || !SCRIPT_ID_VALID(import->imp_sid))
	திரும்பு;

    scriptitem_T *si = SCRIPT_ITEM(import->imp_sid);
    எனில் (si->sn_autoload_prefix == வெற்று)
	திரும்பு;

    char_u *newname = concat_str(si->sn_autoload_prefix, p + 1);
    எனில் (newname == வெற்று)
	திரும்பு;

    எனில் (cb->cb_partial != வெற்று)
    {
	எனில் (cb->cb_name == cb->cb_partial->pt_name)
	    cb->cb_name = newname;
	vim_free(cb->cb_partial->pt_name);
	cb->cb_partial->pt_name = newname;
    }
    இல்லையெனில்
    {
	vim_free(cb->cb_name);
	cb->cb_name = newname;
    }
}

/*
 * Unref/free "callback" returned by get_callback() or set_callback().
 */
    வெறுமை
free_callback(callback_T *callback)
{
    எனில் (callback->cb_partial != வெற்று)
    {
	partial_unref(callback->cb_partial);
	callback->cb_partial = வெற்று;
    }
    இல்லையெனில் எனில் (callback->cb_name != வெற்று)
	func_unref(callback->cb_name);
    எனில் (callback->cb_free_name)
    {
	vim_free(callback->cb_name);
	callback->cb_free_name = FALSE;
    }
    callback->cb_name = வெற்று;
}

#endif // FEAT_EVAL
