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

#અન્તર્ભાવ "vim.h"

#જો defined(FEAT_EVAL)

સ્થિર dictitem_T	globvars_var;		// variable used for g:
સ્થિર dict_T		globvardict;		// Dictionary with g: variables
#define globvarht globvardict.dv_hashtab

/*
 * Old Vim variables such as "v:version" are also available without the "v:".
 * Also in functions.  We need a special hashtable for them.
 */
સ્થિર hashtab_T	compat_hashtab;

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

પ્રકારવ્યાખ્યા સંરચના vimvar vimvar_T;

સ્થિર સંરચના vimvar
{
    અક્ષર	*vv_name;	// name of variable, without v:
    dictitem16_T vv_di;		// value and name for key (max 16 chars!)
    type_T	*vv_type;	// type or NULL
    અક્ષર	vv_flags;	// VV_COMPAT, VV_RO, VV_RO_SBX
} vimvars[VV_LEN] =
{
    // The order here must match the VV_ defines in vim.h!
    // Initializing a union does not work, leave tv.vval empty to get zero's.
    {VV_NAME("count",		 VAR_સંખ્યા), ખાલી, VV_COMPAT+VV_RO},
    {VV_NAME("count1",		 VAR_સંખ્યા), ખાલી, VV_RO},
    {VV_NAME("prevcount",	 VAR_સંખ્યા), ખાલી, VV_RO},
    {VV_NAME("errmsg",		 VAR_STRING), ખાલી, VV_COMPAT},
    {VV_NAME("warningmsg",	 VAR_STRING), ખાલી, 0},
    {VV_NAME("statusmsg",	 VAR_STRING), ખાલી, 0},
    {VV_NAME("shell_error",	 VAR_સંખ્યા), ખાલી, VV_COMPAT+VV_RO},
    {VV_NAME("this_session",	 VAR_STRING), ખાલી, VV_COMPAT},
    {VV_NAME("version",		 VAR_સંખ્યા), ખાલી, VV_COMPAT+VV_RO},
    {VV_NAME("lnum",		 VAR_સંખ્યા), ખાલી, VV_RO_SBX},
    {VV_NAME("termresponse",	 VAR_STRING), ખાલી, VV_RO},
    {VV_NAME("fname",		 VAR_STRING), ખાલી, VV_RO},
    {VV_NAME("lang",		 VAR_STRING), ખાલી, VV_RO},
    {VV_NAME("lc_time",		 VAR_STRING), ખાલી, VV_RO},
    {VV_NAME("ctype",		 VAR_STRING), ખાલી, VV_RO},
    {VV_NAME("charconvert_from", VAR_STRING), ખાલી, VV_RO},
    {VV_NAME("charconvert_to",	 VAR_STRING), ખાલી, VV_RO},
    {VV_NAME("fname_in",	 VAR_STRING), ખાલી, VV_RO},
    {VV_NAME("fname_out",	 VAR_STRING), ખાલી, VV_RO},
    {VV_NAME("fname_new",	 VAR_STRING), ખાલી, VV_RO},
    {VV_NAME("fname_diff",	 VAR_STRING), ખાલી, VV_RO},
    {VV_NAME("cmdarg",		 VAR_STRING), ખાલી, VV_RO},
    {VV_NAME("foldstart",	 VAR_સંખ્યા), ખાલી, VV_RO_SBX},
    {VV_NAME("foldend",		 VAR_સંખ્યા), ખાલી, VV_RO_SBX},
    {VV_NAME("folddashes",	 VAR_STRING), ખાલી, VV_RO_SBX},
    {VV_NAME("foldlevel",	 VAR_સંખ્યા), ખાલી, VV_RO_SBX},
    {VV_NAME("progname",	 VAR_STRING), ખાલી, VV_RO},
    {VV_NAME("servername",	 VAR_STRING), ખાલી, VV_RO},
    {VV_NAME("dying",		 VAR_સંખ્યા), ખાલી, VV_RO},
    {VV_NAME("exception",	 VAR_STRING), ખાલી, VV_RO},
    {VV_NAME("throwpoint",	 VAR_STRING), ખાલી, VV_RO},
    {VV_NAME("register",	 VAR_STRING), ખાલી, VV_RO},
    {VV_NAME("cmdbang",		 VAR_સંખ્યા), ખાલી, VV_RO},
    {VV_NAME("insertmode",	 VAR_STRING), ખાલી, VV_RO},
    {VV_NAME("val",		 VAR_UNKNOWN), ખાલી, VV_RO},
    {VV_NAME("key",		 VAR_UNKNOWN), ખાલી, VV_RO},
    {VV_NAME("profiling",	 VAR_સંખ્યા), ખાલી, VV_RO},
    {VV_NAME("fcs_reason",	 VAR_STRING), ખાલી, VV_RO},
    {VV_NAME("fcs_choice",	 VAR_STRING), ખાલી, 0},
    {VV_NAME("beval_bufnr",	 VAR_સંખ્યા), ખાલી, VV_RO},
    {VV_NAME("beval_winnr",	 VAR_સંખ્યા), ખાલી, VV_RO},
    {VV_NAME("beval_winid",	 VAR_સંખ્યા), ખાલી, VV_RO},
    {VV_NAME("beval_lnum",	 VAR_સંખ્યા), ખાલી, VV_RO},
    {VV_NAME("beval_col",	 VAR_સંખ્યા), ખાલી, VV_RO},
    {VV_NAME("beval_text",	 VAR_STRING), ખાલી, VV_RO},
    {VV_NAME("scrollstart",	 VAR_STRING), ખાલી, 0},
    {VV_NAME("swapname",	 VAR_STRING), ખાલી, VV_RO},
    {VV_NAME("swapchoice",	 VAR_STRING), ખાલી, 0},
    {VV_NAME("swapcommand",	 VAR_STRING), ખાલી, VV_RO},
    {VV_NAME("char",		 VAR_STRING), ખાલી, 0},
    {VV_NAME("mouse_win",	 VAR_સંખ્યા), ખાલી, 0},
    {VV_NAME("mouse_winid",	 VAR_સંખ્યા), ખાલી, 0},
    {VV_NAME("mouse_lnum",	 VAR_સંખ્યા), ખાલી, 0},
    {VV_NAME("mouse_col",	 VAR_સંખ્યા), ખાલી, 0},
    {VV_NAME("operator",	 VAR_STRING), ખાલી, VV_RO},
    {VV_NAME("searchforward",	 VAR_સંખ્યા), ખાલી, 0},
    {VV_NAME("hlsearch",	 VAR_સંખ્યા), ખાલી, 0},
    {VV_NAME("oldfiles",	 VAR_LIST), &t_list_string, 0},
    {VV_NAME("windowid",	 VAR_સંખ્યા), ખાલી, VV_RO},
    {VV_NAME("progpath",	 VAR_STRING), ખાલી, VV_RO},
    {VV_NAME("completed_item",	 VAR_DICT), &t_dict_string, 0},
    {VV_NAME("option_new",	 VAR_STRING), ખાલી, VV_RO},
    {VV_NAME("option_old",	 VAR_STRING), ખાલી, VV_RO},
    {VV_NAME("option_oldlocal",	 VAR_STRING), ખાલી, VV_RO},
    {VV_NAME("option_oldglobal", VAR_STRING), ખાલી, VV_RO},
    {VV_NAME("option_command",	 VAR_STRING), ખાલી, VV_RO},
    {VV_NAME("option_type",	 VAR_STRING), ખાલી, VV_RO},
    {VV_NAME("errors",		 VAR_LIST), &t_list_string, 0},
    {VV_NAME("false",		 VAR_BOOL), ખાલી, VV_RO},
    {VV_NAME("true",		 VAR_BOOL), ખાલી, VV_RO},
    {VV_NAME("none",		 VAR_SPECIAL), ખાલી, VV_RO},
    {VV_NAME("null",		 VAR_SPECIAL), ખાલી, VV_RO},
    {VV_NAME("numbermax",	 VAR_સંખ્યા), ખાલી, VV_RO},
    {VV_NAME("numbermin",	 VAR_સંખ્યા), ખાલી, VV_RO},
    {VV_NAME("numbersize",	 VAR_સંખ્યા), ખાલી, VV_RO},
    {VV_NAME("vim_did_enter",	 VAR_સંખ્યા), ખાલી, VV_RO},
    {VV_NAME("testing",		 VAR_સંખ્યા), ખાલી, 0},
    {VV_NAME("t_number",	 VAR_સંખ્યા), ખાલી, VV_RO},
    {VV_NAME("t_string",	 VAR_સંખ્યા), ખાલી, VV_RO},
    {VV_NAME("t_func",		 VAR_સંખ્યા), ખાલી, VV_RO},
    {VV_NAME("t_list",		 VAR_સંખ્યા), ખાલી, VV_RO},
    {VV_NAME("t_dict",		 VAR_સંખ્યા), ખાલી, VV_RO},
    {VV_NAME("t_float",		 VAR_સંખ્યા), ખાલી, VV_RO},
    {VV_NAME("t_bool",		 VAR_સંખ્યા), ખાલી, VV_RO},
    {VV_NAME("t_none",		 VAR_સંખ્યા), ખાલી, VV_RO},
    {VV_NAME("t_job",		 VAR_સંખ્યા), ખાલી, VV_RO},
    {VV_NAME("t_channel",	 VAR_સંખ્યા), ખાલી, VV_RO},
    {VV_NAME("t_blob",		 VAR_સંખ્યા), ખાલી, VV_RO},
    {VV_NAME("t_class",		 VAR_સંખ્યા), ખાલી, VV_RO},
    {VV_NAME("t_object",	 VAR_સંખ્યા), ખાલી, VV_RO},
    {VV_NAME("termrfgresp",	 VAR_STRING), ખાલી, VV_RO},
    {VV_NAME("termrbgresp",	 VAR_STRING), ખાલી, VV_RO},
    {VV_NAME("termu7resp",	 VAR_STRING), ખાલી, VV_RO},
    {VV_NAME("termstyleresp",	 VAR_STRING), ખાલી, VV_RO},
    {VV_NAME("termblinkresp",	 VAR_STRING), ખાલી, VV_RO},
    {VV_NAME("event",		 VAR_DICT), ખાલી, VV_RO},
    {VV_NAME("versionlong",	 VAR_સંખ્યા), ખાલી, VV_RO},
    {VV_NAME("echospace",	 VAR_સંખ્યા), ખાલી, VV_RO},
    {VV_NAME("argv",		 VAR_LIST), &t_list_string, VV_RO},
    {VV_NAME("collate",		 VAR_STRING), ખાલી, VV_RO},
    {VV_NAME("exiting",		 VAR_SPECIAL), ખાલી, VV_RO},
    {VV_NAME("colornames",       VAR_DICT), &t_dict_string, VV_RO},
    {VV_NAME("sizeofint",	 VAR_સંખ્યા), ખાલી, VV_RO},
    {VV_NAME("sizeoflong",	 VAR_સંખ્યા), ખાલી, VV_RO},
    {VV_NAME("sizeofpointer",	 VAR_સંખ્યા), ખાલી, VV_RO},
    {VV_NAME("maxcol",		 VAR_સંખ્યા), ખાલી, VV_RO},
    {VV_NAME("python3_version",	 VAR_સંખ્યા), ખાલી, VV_RO},
    {VV_NAME("t_typealias",	 VAR_સંખ્યા), ખાલી, VV_RO},
    {VV_NAME("t_enum",		 VAR_સંખ્યા), ખાલી, VV_RO},
    {VV_NAME("t_enumvalue",	 VAR_સંખ્યા), ખાલી, VV_RO},
    {VV_NAME("stacktrace",	 VAR_LIST), &t_list_dict_any, VV_RO},
    {VV_NAME("t_tuple",		 VAR_સંખ્યા), ખાલી, VV_RO},
    {VV_NAME("wayland_display",  VAR_STRING), ખાલી, VV_RO},
    {VV_NAME("clipmethod",	 VAR_STRING), ખાલી, VV_RO},
    {VV_NAME("termda1",		 VAR_STRING), ખાલી, VV_RO},
    {VV_NAME("termosc",	 VAR_STRING), ખાલી, VV_RO},
    {VV_NAME("vim_did_init",	 VAR_સંખ્યા), ખાલી, VV_RO},
    {VV_NAME("clipproviders",	 VAR_DICT), ખાલી, VV_RO},
    {VV_NAME("imagebackend",	 VAR_STRING), ખાલી, VV_RO}
};

// shorthand
#define vv_tv_type	vv_di.di_tv.v_type
#define vv_nr		vv_di.di_tv.vval.v_સંખ્યા
#define vv_float	vv_di.di_tv.vval.v_float
#define vv_str		vv_di.di_tv.vval.v_string
#define vv_list		vv_di.di_tv.vval.v_list
#define vv_dict		vv_di.di_tv.vval.v_dict
#define vv_blob		vv_di.di_tv.vval.v_blob
#define vv_tv		vv_di.di_tv

સ્થિર dictitem_T	vimvars_var;		// variable used for v:
સ્થિર dict_T		vimvardict;		// Dictionary with v: variables
#define vimvarht  vimvardict.dv_hashtab

// for VIM_VERSION_ defines
#અન્તર્ભાવ "version.h"

સ્થિર શૂન્ય list_glob_vars(પૂર્ણાંક *first);
સ્થિર શૂન્ય list_buf_vars(પૂર્ણાંક *first);
સ્થિર શૂન્ય list_win_vars(પૂર્ણાંક *first);
સ્થિર શૂન્ય list_tab_vars(પૂર્ણાંક *first);
સ્થિર char_u *list_arg_vars(exarg_T *eap, char_u *arg, પૂર્ણાંક *first);
સ્થિર char_u *ex_let_one(char_u *arg, typval_T *tv, પૂર્ણાંક copy, પૂર્ણાંક flags, char_u *endchars, char_u *op, પૂર્ણાંક var_idx);
સ્થિર પૂર્ણાંક do_unlet_var(lval_T *lp, char_u *name_end, exarg_T *eap, પૂર્ણાંક deep, શૂન્ય *cookie);
સ્થિર પૂર્ણાંક do_lock_var(lval_T *lp, char_u *name_end, exarg_T *eap, પૂર્ણાંક deep, શૂન્ય *cookie);
સ્થિર શૂન્ય list_one_var(dictitem_T *v, અક્ષર *prefix, પૂર્ણાંક *first);
સ્થિર શૂન્ય list_one_var_a(અક્ષર *prefix, char_u *name, પૂર્ણાંક type, char_u *string, પૂર્ણાંક *first);

/*
 * Initialize global and vim special variables
 */
    શૂન્ય
evalvars_init(શૂન્ય)
{
    પૂર્ણાંક		    i;
    સંરચના vimvar   *p;

    init_var_dict(&globvardict, &globvars_var, VAR_DEF_SCOPE);
    init_var_dict(&vimvardict, &vimvars_var, VAR_SCOPE);
    vimvardict.dv_lock = VAR_FIXED;
    hash_init(&compat_hashtab);

    માટે (i = 0; i < VV_LEN; ++i)
    {
	p = &vimvars[i];
	જો (p->vv_flags & VV_RO)
	    p->vv_di.di_flags = DI_FLAGS_RO | DI_FLAGS_FIX;
	નહીંતર જો (p->vv_flags & VV_RO_SBX)
	    p->vv_di.di_flags = DI_FLAGS_RO_SBX | DI_FLAGS_FIX;
	નહીંતર
	    p->vv_di.di_flags = DI_FLAGS_FIX;

	// add to v: scope dict, unless the value is not always available
	જો (p->vv_tv_type != VAR_UNKNOWN)
	    hash_add(&vimvarht, p->vv_di.di_ચાવી, "initialization");
	જો (p->vv_flags & VV_COMPAT)
	    // add to compat scope dict
	    hash_add(&compat_hashtab, p->vv_di.di_ચાવી, "initialization");
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
    set_vim_var_nr(VV_NUMBERSIZE, કદ(varnumber_T) * 8);
    set_vim_var_nr(VV_SIZEOFINT, કદ(પૂર્ણાંક));
    set_vim_var_nr(VV_SIZEOFLONG, કદ(long));
    set_vim_var_nr(VV_SIZEOFPOINTER, કદ(અક્ષર *));
    set_vim_var_nr(VV_MAXCOL, MAXCOL);

    set_vim_var_nr(VV_TYPE_સંખ્યા,  VAR_TYPE_સંખ્યા);
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

#જો defined(EXITFREE)
/*
 * Free all vim variables information on exit
 */
    શૂન્ય
evalvars_clear(શૂન્ય)
{
    પૂર્ણાંક		    i;
    સંરચના vimvar   *p;

    માટે (i = 0; i < VV_LEN; ++i)
    {
	p = &vimvars[i];
	જો (p->vv_di.di_tv.v_type == VAR_STRING)
	    VIM_CLEAR(p->vv_str);
	નહીંતર જો (p->vv_di.di_tv.v_type == VAR_LIST)
	{
	    list_unref(p->vv_list);
	    p->vv_list = ખાલી;
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
    માટે (i = 1; i <= script_વસ્તુઓ.ga_len; ++i)
	vars_clear(&SCRIPT_VARS(i));
}
#endif

    પૂર્ણાંક
garbage_collect_globvars(પૂર્ણાંક copyID)
{
    પાછા set_ref_in_ht(&globvarht, copyID, ખાલી, ખાલી);
}

    પૂર્ણાંક
garbage_collect_vimvars(પૂર્ણાંક copyID)
{
    પાછા set_ref_in_ht(&vimvarht, copyID, ખાલી, ખાલી);
}

    પૂર્ણાંક
garbage_collect_scriptvars(પૂર્ણાંક copyID)
{
    પૂર્ણાંક		    i;
    પૂર્ણાંક		    idx;
    પૂર્ણાંક		    abort = FALSE;
    scriptitem_T    *si;

    માટે (i = 1; i <= script_વસ્તુઓ.ga_len; ++i)
    {
	abort = abort || set_ref_in_ht(&SCRIPT_VARS(i), copyID, ખાલી, ખાલી);

	si = SCRIPT_ITEM(i);
	માટે (idx = 0; idx < si->sn_var_vals.ga_len; ++idx)
	{
	    svar_T    *sv = ((svar_T *)si->sn_var_vals.ga_data) + idx;

	    જો (sv->sv_name != ખાલી)
		abort = abort || set_ref_in_item(sv->sv_tv, copyID, ખાલી, ખાલી, ખાલી);
	}
    }

    પાછા abort;
}

/*
 * Set an internal variable to a string value. Creates the variable if it does
 * not already exist.
 */
    શૂન્ય
set_internal_string_var(char_u *name, char_u *કિંમત)
{
    char_u	*val;
    typval_T	*tvp;

    val = vim_strsave(કિંમત);
    જો (val == ખાલી)
	પાછા;

    tvp = alloc_string_tv(val);
    જો (tvp == ખાલી)
	પાછા;

    set_var(name, tvp, FALSE);
    free_tv(tvp);
}

    પૂર્ણાંક
eval_charconvert(
    char_u	*enc_from,
    char_u	*enc_to,
    char_u	*fname_from,
    char_u	*fname_to)
{
    પૂર્ણાંક		err = FALSE;
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;

    set_vim_var_string(VV_CC_FROM, enc_from, -1);
    set_vim_var_string(VV_CC_TO, enc_to, -1);
    set_vim_var_string(VV_FNAME_IN, fname_from, -1);
    set_vim_var_string(VV_FNAME_OUT, fname_to, -1);
    ctx = get_option_sctx("charconvert");
    જો (ctx != ખાલી)
	current_sctx = *ctx;

    જો (eval_to_bool(p_ccv, &err, ખાલી, FALSE, TRUE))
	err = TRUE;

    set_vim_var_string(VV_CC_FROM, ખાલી, -1);
    set_vim_var_string(VV_CC_TO, ખાલી, -1);
    set_vim_var_string(VV_FNAME_IN, ખાલી, -1);
    set_vim_var_string(VV_FNAME_OUT, ખાલી, -1);
    current_sctx = saved_sctx;

    જો (err)
	પાછા FAIL;
    પાછા OK;
}

#જો defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
    પૂર્ણાંક
eval_printexpr(char_u *fname, char_u *args)
{
    પૂર્ણાંક		err = FALSE;
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;

    set_vim_var_string(VV_FNAME_IN, fname, -1);
    set_vim_var_string(VV_CMDARG, args, -1);
    ctx = get_option_sctx("printexpr");
    જો (ctx != ખાલી)
	current_sctx = *ctx;

    જો (eval_to_bool(p_pexpr, &err, ખાલી, FALSE, TRUE))
	err = TRUE;

    set_vim_var_string(VV_FNAME_IN, ખાલી, -1);
    set_vim_var_string(VV_CMDARG, ખાલી, -1);
    current_sctx = saved_sctx;

    જો (err)
    {
	mch_remove(fname);
	પાછા FAIL;
    }
    પાછા OK;
}
#endif

#જો defined(FEAT_DIFF)
    શૂન્ય
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
    જો (ctx != ખાલી)
	current_sctx = *ctx;

    // errors are ignored
    tv = eval_પદાવલિ_ext(p_dex, ખાલી, TRUE);
    free_tv(tv);

    set_vim_var_string(VV_FNAME_IN, ખાલી, -1);
    set_vim_var_string(VV_FNAME_NEW, ખાલી, -1);
    set_vim_var_string(VV_FNAME_OUT, ખાલી, -1);
    current_sctx = saved_sctx;
}

    શૂન્ય
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
    જો (ctx != ખાલી)
	current_sctx = *ctx;

    // errors are ignored
    tv = eval_પદાવલિ_ext(p_pex, ખાલી, TRUE);
    free_tv(tv);

    set_vim_var_string(VV_FNAME_IN, ખાલી, -1);
    set_vim_var_string(VV_FNAME_DIFF, ખાલી, -1);
    set_vim_var_string(VV_FNAME_OUT, ખાલી, -1);
    current_sctx = saved_sctx;
}
#endif

#જો defined(FEAT_SPELL)
/*
 * Evaluate an expression to a list with suggestions.
 * For the "expr:" part of 'spellsuggest'.
 * Returns NULL when there is an error.
 */
    list_T *
eval_spell_પદાવલિ(char_u *badword, char_u *પદાવલિ)
{
    typval_T	save_val;
    typval_T	rettv;
    list_T	*list = ખાલી;
    char_u	*p = skipwhite(પદાવલિ);
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;
    પૂર્ણાંક		r;

    // Set "v:val" to the bad word.
    prepare_vimvar(VV_VAL, &save_val);
    set_vim_var_string(VV_VAL, badword, -1);
    જો (p_verbose == 0)
	++emsg_off;
    ctx = get_option_sctx("spellsuggest");
    જો (ctx != ખાલી)
	current_sctx = *ctx;

    r = may_call_simple_func(p, &rettv);
    જો (r == NOTDONE)
	r = eval1(&p, &rettv, &EVALARG_EVALUATE);
    જો (r == OK)
    {
	જો (rettv.v_type != VAR_LIST)
	    clear_tv(&rettv);
	નહીંતર
	    list = rettv.vval.v_list;
    }

    જો (p_verbose == 0)
	--emsg_off;
    clear_tv(get_vim_var_tv(VV_VAL));
    restore_vimvar(VV_VAL, &save_val);
    current_sctx = saved_sctx;

    પાછા list;
}

/*
 * "list" is supposed to contain two items: a word and a number.  Return the
 * word in "pp" and the number as the return value.
 * Return -1 if anything isn't right.
 * Used to get the good word and score from the eval_spell_expr() result.
 */
    પૂર્ણાંક
get_spellword(list_T *list, char_u **pp)
{
    listitem_T	*li;

    li = list->lv_first;
    જો (li == ખાલી)
	પાછા -1;
    *pp = tv_get_string(&li->li_tv);

    li = li->li_next;
    જો (li == ખાલી)
	પાછા -1;
    પાછા (પૂર્ણાંક)tv_get_સંખ્યા(&li->li_tv);
}
#endif

/*
 * Prepare v: variable "idx" to be used.
 * Save the current typeval in "save_tv" and clear it.
 * When not used yet add the variable to the v: hashtable.
 */
    શૂન્ય
prepare_vimvar(પૂર્ણાંક idx, typval_T *save_tv)
{
    *save_tv = vimvars[idx].vv_tv;
    vimvars[idx].vv_str = ખાલી;  // don't free it now
    જો (vimvars[idx].vv_tv_type == VAR_UNKNOWN)
	hash_add(&vimvarht, vimvars[idx].vv_di.di_ચાવી, "prepare vimvar");
}

/*
 * Restore v: variable "idx" to typeval "save_tv".
 * Note that the v: variable must have been cleared already.
 * When no longer defined, remove the variable from the v: hashtable.
 */
    શૂન્ય
restore_vimvar(પૂર્ણાંક idx, typval_T *save_tv)
{
    hashitem_T	*hi;

    vimvars[idx].vv_tv = *save_tv;
    જો (vimvars[idx].vv_tv_type != VAR_UNKNOWN)
	પાછા;

    hi = hash_find(&vimvarht, vimvars[idx].vv_di.di_ચાવી);
    જો (HASHITEM_EMPTY(hi))
	internal_error("restore_vimvar()");
    નહીંતર
	hash_remove(&vimvarht, hi, "restore vimvar");
}

/*
 * List Vim variables.
 */
    સ્થિર શૂન્ય
list_vim_vars(પૂર્ણાંક *first)
{
    list_hashtable_vars(&vimvarht, "v:", FALSE, first);
}

/*
 * List script-local variables, if there is a script.
 */
    સ્થિર શૂન્ય
list_script_vars(પૂર્ણાંક *first)
{
    જો (SCRIPT_ID_VALID(current_sctx.sc_sid))
	list_hashtable_vars(&SCRIPT_VARS(current_sctx.sc_sid),
							   "s:", FALSE, first);
}

/*
 * Return TRUE if "name" starts with "g:", "w:", "t:" or "b:".
 * But only when an identifier character follows.
 */
    પૂર્ણાંક
is_scoped_variable(char_u *name)
{
    પાછા vim_strchr((char_u *)"gwbt", name[0]) != ખાલી
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
eval_one_પદાવલિ_in_str(char_u *p, garray_T *gap, પૂર્ણાંક evaluate)
{
    char_u	*block_start = skipwhite(p + 1);  // skip the opening {
    char_u	*block_end = block_start;
    char_u	*પદાવલિ_val;

    જો (*block_start == NUL)
    {
	semsg(_(e_missing_close_curly_str), p);
	પાછા ખાલી;
    }
    જો (skip_પદાવલિ(&block_end, ખાલી) == FAIL)
	પાછા ખાલી;
    block_end = skipwhite(block_end);
    જો (*block_end != '}')
    {
	semsg(_(e_missing_close_curly_str), p);
	પાછા ખાલી;
    }
    જો (evaluate)
    {
	*block_end = NUL;
	પદાવલિ_val = eval_to_string(block_start, FALSE, FALSE);
	*block_end = '}';
	જો (પદાવલિ_val == ખાલી)
	    પાછા ખાલી;
	ga_concat(gap, પદાવલિ_val);
	vim_free(પદાવલિ_val);
    }

    પાછા block_end + 1;
}

/*
 * Evaluate all the Vim expressions {expr} in "str" and return the resulting
 * string in allocated memory.  "{{" is reduced to "{" and "}}" to "}".
 * Used for a heredoc assignment.
 * Returns NULL for an error.
 */
    સ્થિર char_u *
eval_all_પદાવલિ_in_str(char_u *str)
{
    garray_T	ga;
    char_u	*p;

    ga_init2(&ga, 1, 80);
    p = str;

    જ્યારે (*p != NUL)
    {
	char_u	*lit_start;
	પૂર્ણાંક	escaped_brace = FALSE;

	// Look for a block start.
	lit_start = p;
	જ્યારે (*p != '{' && *p != '}' && *p != NUL)
	    ++p;

	જો (*p != NUL && *p == p[1])
	{
	    // Escaped brace, unescape and continue.
	    // Include the brace in the literal string.
	    ++p;
	    escaped_brace = TRUE;
	}
	નહીંતર જો (*p == '}')
	{
	    semsg(_(e_stray_closing_curly_str), str);
	    ga_clear(&ga);
	    પાછા ખાલી;
	}

	// Append the literal part.
	ga_concat_len(&ga, lit_start, (size_t)(p - lit_start));

	જો (*p == NUL)
	    તોડો;

	જો (escaped_brace)
	{
	    // Skip the second brace.
	    ++p;
	    ચાલુ;
	}

	// Evaluate the expression and append the result.
	p = eval_one_પદાવલિ_in_str(p, &ga, TRUE);
	જો (p == ખાલી)
	{
	    ga_clear(&ga);
	    પાછા ખાલી;
	}
    }
    ga_append(&ga, NUL);

    પાછા ga.ga_data;
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
heredoc_get(exarg_T *eap, char_u *cmd, પૂર્ણાંક script_get, પૂર્ણાંક vim9compile)
{
    char_u	*theline = ખાલી;
    char_u	*marker;
    list_T	*l;
    char_u	*p;
    char_u	*str;
    પૂર્ણાંક		marker_indent_len = 0;
    પૂર્ણાંક		text_indent_len = 0;
    char_u	*text_indent = ખાલી;
    char_u	dot[] = ".";
    પૂર્ણાંક		comment_char = in_vim9script() ? '#' : '"';
    પૂર્ણાંક		evalstr = FALSE;
    પૂર્ણાંક		eval_failed = FALSE;
    cctx_T	*cctx = vim9compile ? eap->cookie : ખાલી;
    પૂર્ણાંક		ગણતરી = 0;
    પૂર્ણાંક		heredoc_in_string = FALSE;
    char_u	*લીટી_arg = ખાલી;
    char_u	*nl_ptr = vim_strchr(cmd, '\n');

    જો (nl_ptr != ખાલી)
    {
	heredoc_in_string = TRUE;
	લીટી_arg = nl_ptr + 1;
	*nl_ptr = NUL;
    }
    નહીંતર જો (eap->ea_getline == ખાલી)
    {
	emsg(_(e_cannot_use_heredoc_here));
	પાછા ખાલી;
    }

    // Check for the optional 'trim' word before the marker
    cmd = skipwhite(cmd);

    જ્યારે (TRUE)
    {
	જો (STRNCMP(cmd, "trim", 4) == 0
		&& (cmd[4] == NUL || VIM_ISWHITE(cmd[4])))
	{
	    cmd = skipwhite(cmd + 4);

	    // Trim the indentation from all the lines in the here document.
	    // The amount of indentation trimmed is the same as the indentation
	    // of the first line after the :let command line.  To find the end
	    // marker the indent of the :let command line is trimmed.
	    p = *eap->cmdlinep;
	    જ્યારે (VIM_ISWHITE(*p))
	    {
		p++;
		marker_indent_len++;
	    }
	    text_indent_len = -1;

	    ચાલુ;
	}
	જો (STRNCMP(cmd, "eval", 4) == 0
		&& (cmd[4] == NUL || VIM_ISWHITE(cmd[4])))
	{
	    cmd = skipwhite(cmd + 4);
	    evalstr = TRUE;
	    ચાલુ;
	}
	તોડો;
    }

    // The marker is the next word.
    જો (*cmd != NUL && *cmd != comment_char)
    {
	marker = skipwhite(cmd);
	p = skiptowhite(marker);
	જો (*skipwhite(p) != NUL && *skipwhite(p) != comment_char)
	{
	    semsg(_(e_trailing_characters_str), p);
	    પાછા ખાલી;
	}
	*p = NUL;
	જો (!script_get && vim_islower(*marker))
	{
	    emsg(_(e_marker_cannot_start_with_lower_case_letter));
	    પાછા ખાલી;
	}
    }
    નહીંતર
    {
	// When getting lines for an embedded script, if the marker is missing,
	// accept '.' as the marker.
	જો (script_get)
	    marker = dot;
	નહીંતર
	{
	    emsg(_(e_missing_marker));
	    પાછા ખાલી;
	}
    }

    l = list_alloc();
    જો (l == ખાલી)
	પાછા ખાલી;

    માટે (;;)
    {
	પૂર્ણાંક	mi = 0;
	પૂર્ણાંક	ti = 0;

	જો (heredoc_in_string)
	{
	    char_u	*next_લીટી;

	    // heredoc in a string separated by newlines.  Get the next line
	    // from the string.

	    જો (*લીટી_arg == NUL)
	    {
		semsg(_(e_missing_end_marker_str), marker);
		તોડો;
	    }

	    theline = લીટી_arg;
	    next_લીટી = vim_strchr(theline, '\n');
	    જો (next_લીટી == ખાલી)
		લીટી_arg += STRLEN(લીટી_arg);
	    નહીંતર
	    {
		*next_લીટી = NUL;
		લીટી_arg = next_લીટી + 1;
	    }
	}
	નહીંતર
	{
	    vim_free(theline);
	    theline = eap->ea_getline(NUL, eap->cookie, 0, FALSE);
	    જો (theline == ખાલી)
	    {
		semsg(_(e_missing_end_marker_str), marker);
		તોડો;
	    }
	}

	// with "trim": skip the indent matching the :let line to find the
	// marker
	જો (marker_indent_len > 0
		&& STRNCMP(theline, *eap->cmdlinep, marker_indent_len) == 0)
	    mi = marker_indent_len;
	જો (STRCMP(marker, theline + mi) == 0)
	    તોડો;

	// If expression evaluation failed in the heredoc, then skip till the
	// end marker.
	જો (eval_failed)
	    ચાલુ;

	જો (text_indent_len == -1 && *theline != NUL)
	{
	    // set the text indent from the first line.
	    p = theline;
	    text_indent_len = 0;
	    જ્યારે (VIM_ISWHITE(*p))
	    {
		p++;
		text_indent_len++;
	    }
	    text_indent = vim_strnsave(theline, text_indent_len);
	}
	// with "trim": skip the indent matching the first line
	જો (text_indent != ખાલી)
	    માટે (ti = 0; ti < text_indent_len; ++ti)
		જો (theline[ti] != text_indent[ti])
		    તોડો;

	str = theline + ti;
	જો (vim9compile)
	{
	    જો (compile_all_પદાવલિ_in_str(str, evalstr, cctx) == FAIL)
	    {
		vim_free(theline);
		vim_free(text_indent);
		list_free(l);
		પાછા FAIL;
	    }
	    ગણતરી++;
	}
	નહીંતર
	{
	    પૂર્ણાંક	    free_str = FALSE;

	    જો (evalstr && !eap->skip)
	    {
		str = eval_all_પદાવલિ_in_str(str);
		જો (str == ખાલી)
		{
		    // expression evaluation failed
		    eval_failed = TRUE;
		    ચાલુ;
		}
		free_str = TRUE;
	    }

	    જો (list_append_string(l, str, -1) == FAIL)
	    {
		જો (free_str)
		    vim_free(str);
		તોડો;
	    }
	    જો (free_str)
		vim_free(str);
	}
    }
    જો (heredoc_in_string)
	// Next command follows the heredoc in the string.
	eap->nextcmd = લીટી_arg;
    નહીંતર
	vim_free(theline);
    vim_free(text_indent);

    જો (vim9compile && cctx->ctx_skip != SKIP_YES && !eval_failed)
	generate_NEWLIST(cctx, ગણતરી, FALSE);

    જો (eval_failed)
    {
	// expression evaluation in the heredoc failed
	list_free(l);
	પાછા ખાલી;
    }
    પાછા l;
}

/*
 * Vim9 variable declaration:
 * ":var name"
 * ":var name: type"
 * ":var name = expr"
 * ":var name: type = expr"
 * etc.
 */
    શૂન્ય
ex_var(exarg_T *eap)
{
    char_u *p = eap->cmd;
    પૂર્ણાંક	    has_var;

    જો (!in_vim9script())
    {
	semsg(_(e_str_cannot_be_used_in_legacy_vim_script), ":var");
	પાછા;
    }
    has_var = checkforcmd_noparen(&p, "var", 3);
    જો (current_sctx.sc_sid == 0 && has_var)
    {
	emsg(_(e_cannot_declare_variable_on_command_લીટી));
	પાછા;
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
    શૂન્ય
ex_let(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    char_u	*પદાવલિ = ખાલી;
    typval_T	rettv;
    પૂર્ણાંક		var_ગણતરી = 0;
    પૂર્ણાંક		semicolon = 0;
    char_u	op[4];
    char_u	*argend;
    પૂર્ણાંક		first = TRUE;
    પૂર્ણાંક		concat;
    પૂર્ણાંક		has_assign;
    પૂર્ણાંક		flags = 0;
    પૂર્ણાંક		vim9script = in_vim9script();

    જો (eap->cmdidx == CMD_final && !vim9script)
    {
	// In legacy Vim script ":final" is short for ":finally".
	ex_finally(eap);
	પાછા;
    }
    જો (eap->cmdidx == CMD_let && vim9script)
    {
	emsg(_(e_cannot_use_let_in_vim9_script));
	પાછા;
    }

    જો (eap->cmdidx == CMD_const)
	flags |= ASSIGN_CONST;
    નહીંતર જો (eap->cmdidx == CMD_final)
	flags |= ASSIGN_FINAL;

    // Vim9 assignment without ":let", ":const" or ":final"
    જો (eap->arg == eap->cmd)
	flags |= ASSIGN_NO_DECL;

    argend = skip_var_list(arg, TRUE, &var_ગણતરી, &semicolon, FALSE);
    જો (argend == ખાલી)
	પાછા;
    પદાવલિ = skipwhite(argend);
    concat = પદાવલિ[0] == '.'
	&& ((પદાવલિ[1] == '=' && in_old_script(2))
		|| (પદાવલિ[1] == '.' && પદાવલિ[2] == '='));
    has_assign =  *પદાવલિ == '=' || (vim_strchr((char_u *)"+-*/%", *પદાવલિ) != ખાલી
							    && પદાવલિ[1] == '=');
    જો (!has_assign && !concat)
    {
	// ":let" without "=": list variables
	જો (*arg == '[')
	    emsg(_(e_invalid_argument));
	નહીંતર જો (પદાવલિ[0] == '.' && પદાવલિ[1] == '=')
	    emsg(_(e_dot_equal_not_supported_with_script_version_two));
	નહીંતર જો (!ends_excmd2(eap->cmd, arg))
	{
	    જો (vim9script)
	    {
		જો (!ends_excmd2(eap->cmd, skipwhite(argend)))
		    semsg(_(e_trailing_characters_str), argend);
		નહીંતર
		    // Vim9 declaration ":var name: type"
		    arg = vim9_declare_scriptvar(eap, arg);
	    }
	    નહીંતર
	    {
		// ":let var1 var2" - list values
		arg = list_arg_vars(eap, arg, &first);
	    }
	}
	નહીંતર જો (!eap->skip)
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
	પાછા;
    }

    જો (પદાવલિ[0] == '=' && પદાવલિ[1] == '<' && પદાવલિ[2] == '<')
    {
	list_T	*l = ખાલી;
	long	cur_lnum = SOURCING_LNUM;

	// :let text =<< [trim] [eval] END
	// :var text =<< [trim] [eval] END
	જો (vim9script && !eap->skip && (!VIM_ISWHITE(પદાવલિ[-1])
						 || !IS_WHITE_OR_NUL(પદાવલિ[3])))
	    semsg(_(e_white_space_required_before_and_after_str_at_str),
								  "=<<", પદાવલિ);
	નહીંતર
	    l = heredoc_get(eap, પદાવલિ + 3, FALSE, FALSE);

	જો (l != ખાલી)
	{
	    rettv_list_set(&rettv, l);
	    જો (!eap->skip)
	    {
		// errors are for the assignment, not the end marker
		SOURCING_LNUM = cur_lnum;
		op[0] = '=';
		op[1] = NUL;
		(શૂન્ય)ex_let_vars(eap->arg, &rettv, FALSE, semicolon, var_ગણતરી,
								flags, op);
	    }
	    clear_tv(&rettv);
	}
	પાછા;
    }

    evalarg_T   evalarg;
    પૂર્ણાંક	    len = 1;

    CLEAR_FIELD(rettv);

    પૂર્ણાંક cur_lnum;

    op[0] = '=';
    op[1] = NUL;
    જો (*પદાવલિ != '=')
    {
	જો (vim9script && (flags & ASSIGN_NO_DECL) == 0)
	{
	    // +=, /=, etc. require an existing variable
	    semsg(_(e_cannot_use_operator_on_new_variable_str), eap->arg);
	}
	નહીંતર જો (vim_strchr((char_u *)"+-*/%.", *પદાવલિ) != ખાલી)
	{
	    op[0] = *પદાવલિ;   // +=, -=, *=, /=, %= or .=
	    ++len;
	    જો (પદાવલિ[0] == '.' && પદાવલિ[1] == '.') // ..=
	    {
		++પદાવલિ;
		++len;
	    }
	}
	પદાવલિ += 2;
    }
    નહીંતર
	++પદાવલિ;

    જો (vim9script && !eap->skip && (!VIM_ISWHITE(*argend)
					       || !IS_WHITE_OR_NUL(*પદાવલિ)))
    {
	vim_strncpy(op, પદાવલિ - len, len);
	semsg(_(e_white_space_required_before_and_after_str_at_str),
							       op, argend);
    }

    જો (eap->skip)
	++emsg_skip;
    fill_evalarg_from_eap(&evalarg, eap, eap->skip);
    પદાવલિ = skipwhite_and_linebreak(પદાવલિ, &evalarg);
    cur_lnum = SOURCING_LNUM;
    પૂર્ણાંક eval_res = eval0(પદાવલિ, &rettv, eap, &evalarg);
    જો (eap->skip)
	--emsg_skip;
    clear_evalarg(&evalarg, eap);

    // Restore the line number so that any type error is given for the
    // declaration, not the expression.
    SOURCING_LNUM = cur_lnum;

    જો (!eap->skip && eval_res != FAIL)
	(શૂન્ય)ex_let_vars(eap->arg, &rettv, FALSE, semicolon, var_ગણતરી,
								    flags, op);
    જો (eval_res != FAIL)
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
    પૂર્ણાંક
ex_let_vars(
    char_u	*arg_start,
    typval_T	*tv,
    પૂર્ણાંક		copy,		// copy values from "tv", don't move
    પૂર્ણાંક		semicolon,	// from skip_var_list()
    પૂર્ણાંક		var_ગણતરી,	// from skip_var_list()
    પૂર્ણાંક		flags,		// ASSIGN_FINAL, ASSIGN_CONST, etc.
    char_u	*op)
{
    char_u	*arg = arg_start;
    list_T	*l;
    tuple_T	*tuple = ખાલી;
    પૂર્ણાંક		i;
    પૂર્ણાંક		var_idx = 0;
    listitem_T	*item = ખાલી;
    typval_T	ltv;
    પૂર્ણાંક		is_list = tv->v_type == VAR_LIST;
    પૂર્ણાંક		idx;

    જો (tv->v_type == VAR_VOID)
    {
	emsg(_(e_cannot_use_void_કિંમત));
	પાછા FAIL;
    }
    જો (*arg != '[')
    {
	// ":let var = expr" or ":for var in list"
	જો (ex_let_one(arg, tv, copy, flags, op, op, var_idx) == ખાલી)
	    પાછા FAIL;
	પાછા OK;
    }

    // ":let [v1, v2] = list" or ":for [v1, v2] in listlist"
    // or
    // ":let [v1, v2] = tuple" or ":for [v1, v2] in tupletuple"
    જો (tv->v_type != VAR_LIST && tv->v_type != VAR_TUPLE)
    {
	emsg(_(e_list_or_tuple_required));
	પાછા FAIL;
    }
    જો (is_list)
    {
	l = tv->vval.v_list;
	જો (l == ખાલી)
	{
	    emsg(_(e_list_required));
	    પાછા FAIL;
	}
	i = list_len(l);
    }
    નહીંતર
    {
	tuple = tv->vval.v_tuple;
	જો (tuple == ખાલી)
	{
	    emsg(_(e_tuple_required));
	    પાછા FAIL;
	}
	i = tuple_len(tuple);
    }

    જો (semicolon == 0 && var_ગણતરી < i)
    {
	emsg(_(is_list ? e_less_targets_than_list_વસ્તુઓ
					: e_less_targets_than_tuple_વસ્તુઓ));
	પાછા FAIL;
    }
    જો (var_ગણતરી - semicolon > i)
    {
	emsg(_(is_list ? e_more_targets_than_list_વસ્તુઓ
					: e_more_targets_than_tuple_વસ્તુઓ));
	પાછા FAIL;
    }

    જો (is_list)
    {
	CHECK_LIST_MATERIALIZE(l);
	item = l->lv_first;
    }
    નહીંતર
	idx = 0;

    જ્યારે (*arg != ']')
    {
	arg = skipwhite(arg + 1);
	++var_idx;
	arg = ex_let_one(arg, is_list ? &item->li_tv : TUPLE_ITEM(tuple, idx),
			 TRUE, flags | ASSIGN_UNPACK, (char_u *)",;]", op,
			 var_idx);
	જો (is_list)
	    item = item->li_next;
	નહીંતર
	    idx++;
	જો (arg == ખાલી)
	    પાછા FAIL;

	arg = skipwhite(arg);
	જો (*arg == ';')
	{
	    // Put the rest of the list or tuple (may be empty) in the var
	    // after ';'.  Create a new list or tuple for this.
	    જો (is_list)
	    {
		// Put the rest of the list (may be empty) in the var
		// after ';'.  Create a new list for this.
		l = list_alloc();
		જો (l == ખાલી)
		    પાછા FAIL;

		// list
		જ્યારે (item != ખાલી)
		{
		    list_append_tv(l, &item->li_tv);
		    item = item->li_next;
		}

		ltv.v_type = VAR_LIST;
		ltv.v_lock = 0;
		ltv.vval.v_list = l;
		l->lv_refcount = 1;
	    }
	    નહીંતર
	    {
		tuple_T	*new_tuple = tuple_alloc();
		જો (new_tuple == ખાલી)
		    પાછા FAIL;

		// Put the rest of the tuple (may be empty) in the var
		// after ';'.  Create a new tuple for this.
		જ્યારે (idx < TUPLE_LEN(tuple))
		{
		    typval_T    new_tv;

		    copy_tv(TUPLE_ITEM(tuple, idx), &new_tv);
		    જો (tuple_append_tv(new_tuple, &new_tv) == FAIL)
		    {
			clear_tv(&new_tv);
			tuple_unref(new_tuple);
			પાછા FAIL;
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
	    જો (arg == ખાલી)
		પાછા FAIL;
	    તોડો;
	}
	નહીંતર જો (*arg != ',' && *arg != ']')
	{
	    internal_error("ex_let_vars()");
	    પાછા FAIL;
	}
    }

    પાછા OK;
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
    પૂર્ણાંક		include_type,
    પૂર્ણાંક		*var_ગણતરી,
    પૂર્ણાંક		*semicolon,
    પૂર્ણાંક		silent)
{
    char_u	*p, *s;

    જો (*arg == '[')
    {
	// "[var, var]": find the matching ']'.
	p = arg;
	માટે (;;)
	{
	    p = skipwhite(p + 1);	// skip whites after '[', ';' or ','
	    s = skip_var_one(p, include_type);
	    જો (s == p)
	    {
		જો (!silent)
		    semsg(_(e_invalid_argument_str), p);
		પાછા ખાલી;
	    }
	    ++*var_ગણતરી;

	    p = skipwhite(s);
	    જો (*p == ']')
		તોડો;
	    નહીંતર જો (*p == ';')
	    {
		જો (*semicolon == 1)
		{
		    જો (!silent)
			emsg(_(e_double_semicolon_in_list_of_variables));
		    પાછા ખાલી;
		}
		*semicolon = 1;
	    }
	    નહીંતર જો (*p != ',')
	    {
		જો (!silent)
		    semsg(_(e_invalid_argument_str), p);
		પાછા ખાલી;
	    }
	}
	પાછા p + 1;
    }

    પાછા skip_var_one(arg, include_type);
}

/*
 * Skip one (assignable) variable name, including @r, $VAR, &option, d.key,
 * l[idx].
 * In Vim9 script also skip over ": type" if "include_type" is TRUE.
 */
    char_u *
skip_var_one(char_u *arg, પૂર્ણાંક include_type)
{
    char_u	*end;
    પૂર્ણાંક		vim9 = in_vim9script();

    જો (*arg == '@' && arg[1] != NUL)
	પાછા arg + 2;

    // termcap option name may have non-alpha characters
    જો (STRNCMP(arg, "&t_", 3) == 0 && arg[3] != NUL && arg[4] != NUL)
	પાછા arg + 5;

    end = find_name_end(*arg == '$' || *arg == '&' ? arg + 1 : arg,
				   ખાલી, ખાલી, FNE_INCL_BR | FNE_CHECK_START);

    // "a: type" is declaring variable "a" with a type, not "a:".
    // Same for "s: type".
    જો (vim9 && end == arg + 2 && end[-1] == ':')
	--end;

    જો (include_type && vim9)
    {
	જો (*skipwhite(end) == ':')
	    end = skip_type(skipwhite(skipwhite(end) + 1), FALSE);
    }
    પાછા end;
}

/*
 * List variables for hashtab "ht" with prefix "prefix".
 * If "empty" is TRUE also list NULL strings as empty strings.
 */
    શૂન્ય
list_hashtable_vars(
    hashtab_T	*ht,
    અક્ષર	*prefix,
    પૂર્ણાંક		empty,
    પૂર્ણાંક		*first)
{
    hashitem_T	*hi;
    dictitem_T	*di;
    પૂર્ણાંક		todo;
    char_u	buf[IOSIZE];

    પૂર્ણાંક save_ht_flags = ht->ht_flags;
    ht->ht_flags |= HTFLAGS_FROZEN;

    todo = (પૂર્ણાંક)ht->ht_used;
    માટે (hi = ht->ht_array; todo > 0 && !got_int; ++hi)
    {
	જો (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    di = HI2DI(hi);

	    // apply :filter /pat/ to variable name
	    vim_strncpy((char_u *)buf, (char_u *)prefix, IOSIZE - 1);
	    vim_strcat((char_u *)buf, di->di_ચાવી, IOSIZE);
	    જો (message_filtered(buf))
		ચાલુ;

	    જો (empty || di->di_tv.v_type != VAR_STRING
					   || di->di_tv.vval.v_string != ખાલી)
		list_one_var(di, prefix, first);
	}
    }

    ht->ht_flags = save_ht_flags;
}

/*
 * List global variables.
 */
    સ્થિર શૂન્ય
list_glob_vars(પૂર્ણાંક *first)
{
    list_hashtable_vars(&globvarht, "", TRUE, first);
}

/*
 * List buffer variables.
 */
    સ્થિર શૂન્ય
list_buf_vars(પૂર્ણાંક *first)
{
    list_hashtable_vars(&curbuf->b_vars->dv_hashtab, "b:", TRUE, first);
}

/*
 * List window variables.
 */
    સ્થિર શૂન્ય
list_win_vars(પૂર્ણાંક *first)
{
    list_hashtable_vars(&curwin->w_vars->dv_hashtab, "w:", TRUE, first);
}

/*
 * List tab page variables.
 */
    સ્થિર શૂન્ય
list_tab_vars(પૂર્ણાંક *first)
{
    list_hashtable_vars(&curtab->tp_vars->dv_hashtab, "t:", TRUE, first);
}

/*
 * List variables in "arg".
 */
    સ્થિર char_u *
list_arg_vars(exarg_T *eap, char_u *arg, પૂર્ણાંક *first)
{
    પૂર્ણાંક		error = FALSE;
    પૂર્ણાંક		len;
    char_u	*name;
    char_u	*name_start;
    char_u	*arg_subsc;
    char_u	*tofree;
    typval_T    tv;

    જ્યારે (!ends_excmd2(eap->cmd, arg) && !got_int)
    {
	જો (error || eap->skip)
	{
	    arg = find_name_end(arg, ખાલી, ખાલી, FNE_INCL_BR | FNE_CHECK_START);
	    જો (!VIM_ISWHITE(*arg) && !ends_excmd(*arg))
	    {
		emsg_severe = TRUE;
		જો (!did_emsg)
		    semsg(_(e_trailing_characters_str), arg);
		તોડો;
	    }
	}
	નહીંતર
	{
	    // get_name_len() takes care of expanding curly braces
	    name_start = name = arg;
	    len = get_name_len(&arg, &tofree, TRUE, TRUE);
	    જો (len <= 0)
	    {
		// This is mainly to keep test 49 working: when expanding
		// curly braces fails overrule the exception error message.
		જો (len < 0 && !aborting())
		{
		    emsg_severe = TRUE;
		    semsg(_(e_invalid_argument_str), arg);
		    તોડો;
		}
		error = TRUE;
	    }
	    નહીંતર
	    {
		arg = skipwhite(arg);
		જો (tofree != ખાલી)
		    name = tofree;
		જો (eval_variable(name, len, 0, &tv, ખાલી,
						     EVAL_VAR_VERBOSE) == FAIL)
		    error = TRUE;
		નહીંતર
		{
		    // handle d.key, l[idx], f(expr)
		    arg_subsc = arg;
		    જો (handle_subscript(&arg, name_start, &tv,
					      &EVALARG_EVALUATE, TRUE) == FAIL)
			error = TRUE;
		    નહીંતર
		    {
			જો (arg == arg_subsc && len == 2 && name[1] == ':')
			{
			    પસંદગી (*name)
			    {
				સ્થિતિ 'g': list_glob_vars(first); તોડો;
				સ્થિતિ 'b': list_buf_vars(first); તોડો;
				સ્થિતિ 'w': list_win_vars(first); તોડો;
				સ્થિતિ 't': list_tab_vars(first); તોડો;
				સ્થિતિ 'v': list_vim_vars(first); તોડો;
				સ્થિતિ 's': list_script_vars(first); તોડો;
				સ્થિતિ 'l': list_func_vars(first); તોડો;
				મૂળભૂત:
					  semsg(_(e_cant_list_variables_for_str), name);
			    }
			}
			નહીંતર
			{
			    char_u	numbuf[NUMBUFLEN];
			    char_u	*tf;
			    પૂર્ણાંક		c;
			    char_u	*s;

			    s = echo_string(&tv, &tf, numbuf, 0);
			    c = *arg;
			    *arg = NUL;
			    list_one_var_a("",
				    arg == arg_subsc ? name : name_start,
				    tv.v_type,
				    s == ખાલી ? (char_u *)"" : s,
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

    પાછા arg;
}

/*
 * Set an environment variable, part of ex_let_one().
 */
    સ્થિર char_u *
ex_let_env(
    char_u	*arg,
    typval_T	*tv,
    પૂર્ણાંક		flags,
    char_u	*endchars,
    char_u	*op)
{
    char_u	*arg_end = ખાલી;
    char_u	*name;
    પૂર્ણાંક		len;

    જો ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					 && (flags & ASSIGN_FOR_LOOP) == 0)
    {
	emsg(_(e_cannot_lock_environment_variable));
	પાછા ખાલી;
    }

    // Find the end of the name.
    ++arg;
    name = arg;
    len = get_env_len(&arg);
    જો (len == 0)
	semsg(_(e_invalid_argument_str), name - 1);
    નહીંતર
    {
	જો (op != ખાલી && vim_strchr((char_u *)"+-*/%", *op) != ખાલી)
	    semsg(_(e_wrong_variable_type_for_str_equal), op);
	નહીંતર જો (endchars != ખાલી
			      && vim_strchr(endchars, *skipwhite(arg)) == ખાલી)
	    emsg(_(e_unexpected_characters_in_let));
	નહીંતર જો (!check_secure() && !check_restricted())
	{
	    char_u	*tofree = ખાલી;
	    પૂર્ણાંક		c1 = name[len];
	    char_u	*p;

	    name[len] = NUL;
	    p = tv_get_string_chk(tv);
	    જો (p != ખાલી && op != ખાલી && *op == '.')
	    {
		પૂર્ણાંક	mustfree = FALSE;
		char_u  *s = vim_getenv(name, &mustfree);

		જો (s != ખાલી)
		{
		    p = tofree = concat_str(s, p);
		    જો (mustfree)
			vim_free(s);
		}
	    }
	    જો (p != ખાલી)
	    {
		vim_setenv_ext(name, p);
		arg_end = arg;
	    }
	    name[len] = c1;
	    vim_free(tofree);
	}
    }
    પાછા arg_end;
}

/*
 * Set an option, part of ex_let_one().
 */
    સ્થિર char_u *
ex_let_option(
    char_u	*arg,
    typval_T	*tv,
    પૂર્ણાંક		flags,
    char_u	*endchars,
    char_u	*op)
{
    char_u	*p;
    પૂર્ણાંક		scope;
    char_u	*arg_end = ખાલી;

    જો ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					 && (flags & ASSIGN_FOR_LOOP) == 0)
    {
	emsg(_(e_cannot_lock_option));
	પાછા ખાલી;
    }

    // Find the end of the name.
    p = find_option_end(&arg, &scope);
    જો (p == ખાલી || (endchars != ખાલી
			       && vim_strchr(endchars, *skipwhite(p)) == ખાલી))
    {
	emsg(_(e_unexpected_characters_in_let));
	પાછા ખાલી;
    }

    પૂર્ણાંક		c1;
    long	n = 0;
    getoption_T	opt_type;
    long	numval;
    char_u	*stringval = ખાલી;
    char_u	*s = ખાલી;
    પૂર્ણાંક		failed = FALSE;
    પૂર્ણાંક		opt_p_flags;
    char_u	*tofree = ખાલી;
    char_u	numbuf[NUMBUFLEN];

    c1 = *p;
    *p = NUL;

    opt_type = get_option_કિંમત(arg, &numval, &stringval, &opt_p_flags, scope);
    જો (opt_type == gov_unknown && arg[0] != 't' && arg[1] != '_')
    {
	semsg(_(e_unknown_option_str_2), arg);
	જાઓ theend;
    }
    જો (op != ખાલી && *op != '='
	&& (((opt_type == gov_bool || opt_type == gov_સંખ્યા) && *op == '.')
	    || (opt_type == gov_string && *op != '.')))
    {
	semsg(_(e_wrong_variable_type_for_str_equal), op);
	જાઓ theend;
    }

    જો ((opt_type == gov_bool
		|| opt_type == gov_સંખ્યા
		|| opt_type == gov_hidden_bool
		|| opt_type == gov_hidden_સંખ્યા)
		     && (tv->v_type != VAR_STRING || !in_vim9script()))
    {
	જો (opt_type == gov_bool || opt_type == gov_hidden_bool)
	    // bool, possibly hidden
	    n = (long)tv_get_bool_chk(tv, &failed);
	નહીંતર
	    // number, possibly hidden
	    n = (long)tv_get_સંખ્યા_chk(tv, &failed);
	જો (failed)
	    જાઓ theend;
    }

    જો ((opt_p_flags & P_FUNC) && (tv->v_type == VAR_PARTIAL
						    || tv->v_type == VAR_FUNC))
    {
	// If the option can be set to a function reference or a lambda
	// and the passed value is a function reference, then convert it to
	// the name (string) of the function reference.
	s = tv2string(tv, &tofree, numbuf, 0);
	જો (s == ખાલી)
	    જાઓ theend;
    }
    // Avoid setting a string option to the text "v:false" or similar.
    // In Vim9 script also don't convert a number to string.
    નહીંતર જો (tv->v_type != VAR_BOOL && tv->v_type != VAR_SPECIAL
		     && (!in_vim9script() || tv->v_type != VAR_સંખ્યા))
    {
	s = tv_get_string_chk(tv);
	જો (s == ખાલી)
	    જાઓ theend;
    }
    નહીંતર જો (opt_type == gov_string || opt_type == gov_hidden_string)
    {
	emsg(_(e_string_required));
	જાઓ theend;
    }

    જો (op != ખાલી && *op != '=')
    {
	// number, in legacy script also bool
	જો (opt_type == gov_સંખ્યા
				 || (opt_type == gov_bool && !in_vim9script()))
	{
	    પસંદગી (*op)
	    {
		સ્થિતિ '+': n = numval + n; તોડો;
		સ્થિતિ '-': n = numval - n; તોડો;
		સ્થિતિ '*': n = numval * n; તોડો;
		સ્થિતિ '/': n = (long)અંક_divide(numval, n, &failed); તોડો;
		સ્થિતિ '%': n = (long)અંક_modulus(numval, n, &failed); તોડો;
	    }
	    s = ખાલી;
	    જો (failed)
		જાઓ theend;
	}
	નહીંતર જો (opt_type == gov_string && stringval != ખાલી && s != ખાલી)
	{
	    // string
	    s = concat_str(stringval, s);
	    vim_free(stringval);
	    stringval = s;
	}
    }

    અક્ષર *err = set_option_કિંમત(arg, n, s, scope);
    arg_end = p;
    જો (err != ખાલી)
	emsg(_(err));

theend:
    *p = c1;
    vim_free(stringval);
    vim_free(tofree);
    પાછા arg_end;
}

/*
 * Set a register, part of ex_let_one().
 */
    સ્થિર char_u *
ex_let_register(
    char_u	*arg,
    typval_T	*tv,
    પૂર્ણાંક		flags,
    char_u	*endchars,
    char_u	*op)
{
    char_u	*arg_end = ખાલી;

    જો ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					 && (flags & ASSIGN_FOR_LOOP) == 0)
    {
	emsg(_(e_cannot_lock_register));
	પાછા ખાલી;
    }
    ++arg;
    જો (op != ખાલી && vim_strchr((char_u *)"+-*/%", *op) != ખાલી)
	semsg(_(e_wrong_variable_type_for_str_equal), op);
    નહીંતર જો (endchars != ખાલી
			  && vim_strchr(endchars, *skipwhite(arg + 1)) == ખાલી)
	emsg(_(e_unexpected_characters_in_let));
    નહીંતર
    {
	char_u	*ptofree = ખાલી;
	char_u	*p;

	p = tv_get_string_chk(tv);
	જો (p != ખાલી && op != ખાલી && *op == '.')
	{
	    char_u  *s = get_reg_contents(*arg == '@'
						  ? '"' : *arg, GREG_પદાવલિ_SRC);

	    જો (s != ખાલી)
	    {
		p = ptofree = concat_str(s, p);
		vim_free(s);
	    }
	}
	જો (p != ખાલી)
	{
	    write_reg_contents(*arg == '@' ? '"' : *arg, p, -1, FALSE);
	    arg_end = arg + 1;
	}
	vim_free(ptofree);
    }
    પાછા arg_end;
}

/*
 * Set one item of ":let var = expr" or ":let [v1, v2] = list" to its value.
 * Returns a pointer to the char just after the var name.
 * Returns NULL if there is an error.
 */
    સ્થિર char_u *
ex_let_one(
    char_u	*arg,		// points to variable name
    typval_T	*tv,		// value to assign to variable
    પૂર્ણાંક		copy,		// copy value from "tv"
    પૂર્ણાંક		flags,		// ASSIGN_CONST, ASSIGN_FINAL, etc.
    char_u	*endchars,	// valid chars after variable name  or NULL
    char_u	*op,		// "+", "-", "."  or NULL
    પૂર્ણાંક		var_idx)	// variable index for "let [a, b] = list"
{
    char_u	*arg_end = ખાલી;

    જો (in_vim9script() && (flags & (ASSIGN_NO_DECL | ASSIGN_DECL)) == 0
			&& (flags & (ASSIGN_CONST | ASSIGN_FINAL)) == 0
				  && vim_strchr((char_u *)"$@&", *arg) != ખાલી)
    {
	vim9_declare_error(arg);
	પાછા ખાલી;
    }

    જો (check_typval_is_કિંમત(tv) == FAIL)
	પાછા ખાલી;

    // In Vim9 script an environment variable and a register only take a
    // String.  "@#" also takes a buffer number.
    જો (in_vim9script() && (*arg == '$' || *arg == '@')
	    && !(*arg == '@' && arg[1] == '#' && tv->v_type == VAR_સંખ્યા
					       && (op == ખાલી || *op == '=')))
    {
	where_T	where = WHERE_INIT;

	જો (check_typval_type(&t_string, tv, where) == FAIL)
	    પાછા ખાલી;
    }

    જો (*arg == '$')
    {
	// ":let $VAR = expr": Set environment variable.
	પાછા ex_let_env(arg, tv, flags, endchars, op);
    }
    નહીંતર જો (*arg == '&')
    {
	// ":let &option = expr": Set option value.
	// ":let &l:option = expr": Set local option value.
	// ":let &g:option = expr": Set global option value.
	// ":for &ts in range(8)": Set option value for for loop
	પાછા ex_let_option(arg, tv, flags, endchars, op);
    }
    નહીંતર જો (*arg == '@')
    {
	// ":let @r = expr": Set register contents.
	પાછા ex_let_register(arg, tv, flags, endchars, op);
    }
    નહીંતર જો (eval_isnamec1(*arg) || *arg == '{')
    {
	lval_T	lv;
	char_u	*p;
	પૂર્ણાંક	lval_flags = (flags & (ASSIGN_NO_DECL | ASSIGN_DECL))
							     ? GLV_NO_DECL : 0;
	lval_flags |= (flags & ASSIGN_FOR_LOOP) ? GLV_FOR_LOOP : 0;
	જો (op != ખાલી && *op != '=')
	    lval_flags |= GLV_ASSIGN_WITH_OP;

	// ":let var = expr": Set internal variable.
	// ":let var: type = expr": Set internal variable with type.
	// ":let {expr} = expr": Idem, name made with curly braces
	p = get_lval(arg, tv, &lv, FALSE, FALSE, lval_flags, FNE_CHECK_START);
	જો (p != ખાલી && lv.ll_name != ખાલી)
	{
	    જો (endchars != ખાલી && vim_strchr(endchars,
					   *skipwhite(lv.ll_name_end)) == ખાલી)
	    {
		emsg(_(e_unexpected_characters_in_let));
	    }
	    નહીંતર
	    {
		set_var_lval(&lv, p, tv, copy, flags, op, var_idx);
		arg_end = lv.ll_name_end;
	    }
	}
	clear_lval(&lv);
    }
    નહીંતર
	semsg(_(e_invalid_argument_str), arg);

    પાછા arg_end;
}

/*
 * ":unlet[!] var1 ... " command.
 */
    શૂન્ય
ex_unlet(exarg_T *eap)
{
    ex_unletlock(eap, eap->arg, 0, eap->forceit ? GLV_QUIET : 0, do_unlet_var, ખાલી);
}

/*
 * ":lockvar" and ":unlockvar" commands
 */
    શૂન્ય
ex_lockvar(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    પૂર્ણાંક		deep = 2;

    જો (eap->forceit)
	deep = -1;
    નહીંતર જો (vim_isdigit(*arg))
    {
	deep = getdigits(&arg);
	arg = skipwhite(arg);
    }

    ex_unletlock(eap, arg, deep, 0, do_lock_var, ખાલી);
}

/*
 * ":unlet", ":lockvar" and ":unlockvar" are quite similar.
 * Also used for Vim9 script.  "callback" is invoked as:
 *	callback(&lv, name_end, eap, deep, cookie)
 */
    શૂન્ય
ex_unletlock(
    exarg_T	*eap,
    char_u	*argstart,
    પૂર્ણાંક		deep,
    પૂર્ણાંક		glv_flags,
    પૂર્ણાંક		(*callback)(lval_T *, char_u *, exarg_T *, પૂર્ણાંક, શૂન્ય *),
    શૂન્ય	*cookie)
{
    char_u	*arg = argstart;
    char_u	*name_end;
    પૂર્ણાંક		error = FALSE;
    lval_T	lv;

    કરો
    {
	જો (*arg == '$')
	{
	    lv.ll_name = arg;
	    lv.ll_tv = ખાલી;
	    ++arg;
	    જો (get_env_len(&arg) == 0)
	    {
		semsg(_(e_invalid_argument_str), arg - 1);
		પાછા;
	    }
	    જો (!error && !eap->skip
			      && callback(&lv, arg, eap, deep, cookie) == FAIL)
		error = TRUE;
	    name_end = arg;
	}
	નહીંતર
	{
	    // Parse the name and find the end.
	    name_end = get_lval(arg, ખાલી, &lv, TRUE, eap->skip || error,
				     glv_flags | GLV_NO_DECL, FNE_CHECK_START);
	    જો (lv.ll_name == ખાલી)
		error = TRUE;	    // error but continue parsing
	    જો (name_end == ખાલી || (!VIM_ISWHITE(*name_end)
						    && !ends_excmd(*name_end)))
	    {
		જો (name_end != ખાલી)
		{
		    emsg_severe = TRUE;
		    semsg(_(e_trailing_characters_str), name_end);
		}
		જો (!(eap->skip || error))
		    clear_lval(&lv);
		તોડો;
	    }

	    જો (!error && !eap->skip
			 && callback(&lv, name_end, eap, deep, cookie) == FAIL)
		error = TRUE;

	    જો (!eap->skip)
		clear_lval(&lv);
	}

	arg = skipwhite(name_end);
    } જ્યારે (!ends_excmd2(name_end, arg));

    set_nextcmd(eap, arg);
}

    સ્થિર પૂર્ણાંક
do_unlet_var(
    lval_T	*lp,
    char_u	*name_end,
    exarg_T	*eap,
    પૂર્ણાંક		deep UNUSED,
    શૂન્ય	*cookie UNUSED)
{
    પૂર્ણાંક		forceit = eap->forceit;
    પૂર્ણાંક		ret = OK;
    પૂર્ણાંક		cc;

    જો (lp->ll_tv == ખાલી)
    {
	cc = *name_end;
	*name_end = NUL;

	// Environment variable, normal name or expanded name.
	જો (*lp->ll_name == '$')
	    vim_unsetenv_ext(lp->ll_name + 1);
	નહીંતર જો (do_unlet(lp->ll_name, forceit) == FAIL)
	    ret = FAIL;
	*name_end = cc;
    }
    નહીંતર જો ((lp->ll_list != ખાલી
		 && કિંમત_check_lock(lp->ll_list->lv_lock, lp->ll_name, FALSE))
	    || (lp->ll_dict != ખાલી
		&& કિંમત_check_lock(lp->ll_dict->dv_lock, lp->ll_name, FALSE)))
	પાછા FAIL;
    નહીંતર જો (lp->ll_range)
	list_unlet_range(lp->ll_list, lp->ll_li, lp->ll_n1,
						    !lp->ll_empty2, lp->ll_n2);
    નહીંતર જો (lp->ll_list != ખાલી)
	// unlet a List item.
	listitem_remove(lp->ll_list, lp->ll_li);
    નહીંતર જો (lp->ll_dict != ખાલી)
	// unlet a Dictionary item.
	dictitem_remove(lp->ll_dict, lp->ll_di, "unlet");
    નહીંતર
    {
	semsg(_(e_cannot_unlet_imported_item_str), lp->ll_name);
	પાછા FAIL;
    }

    પાછા ret;
}

/*
 * Unlet one item or a range of items from a list.
 * Return OK or FAIL.
 */
    શૂન્ય
list_unlet_range(
	list_T	    *l,
	listitem_T  *li_first,
	long	    n1_arg,
	પૂર્ણાંક	    has_n2,
	long	    n2)
{
    listitem_T  *li = li_first;
    પૂર્ણાંક		n1 = n1_arg;

    // Delete a range of List items.
    li = li_first;
    n1 = n1_arg;
    જ્યારે (li != ખાલી && (!has_n2 || n2 >= n1))
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
    પૂર્ણાંક
do_unlet(char_u *name, પૂર્ણાંક forceit)
{
    hashtab_T	*ht;
    hashitem_T	*hi;
    char_u	*varname = ખાલી;  // init to shut up gcc
    dict_T	*d;
    dictitem_T	*di;

    // can't :unlet a script variable in Vim9 script
    જો (in_vim9script() && check_vim9_unlet(name) == FAIL)
	પાછા FAIL;

    ht = find_var_ht(name, &varname);

    // can't :unlet a script variable in Vim9 script from a function
    જો (ht == get_script_local_ht()
	    && SCRIPT_ID_VALID(current_sctx.sc_sid)
	    && SCRIPT_ITEM(current_sctx.sc_sid)->sn_version
							 == SCRIPT_VERSION_VIM9
	    && check_vim9_unlet(name) == FAIL)
	પાછા FAIL;

    જો (ht != ખાલી && *varname != NUL)
    {
	d = get_current_funccal_dict(ht);
	જો (d == ખાલી)
	{
	    જો (ht == &globvarht)
		d = &globvardict;
	    નહીંતર જો (ht == &compat_hashtab)
		d = &vimvardict;
	    નહીંતર
	    {
		di = find_var_in_ht(ht, *name, (char_u *)"", FALSE);
		d = di == ખાલી ? ખાલી : di->di_tv.vval.v_dict;
	    }
	    જો (d == ખાલી)
	    {
		internal_error("do_unlet()");
		પાછા FAIL;
	    }
	}
	hi = hash_find(ht, varname);
	જો (HASHITEM_EMPTY(hi))
	    hi = find_hi_in_scoped_ht(name, &ht);
	જો (hi != ખાલી && !HASHITEM_EMPTY(hi))
	{
	    di = HI2DI(hi);
	    જો (var_check_fixed(di->di_flags, name, FALSE)
		    || var_check_ro(di->di_flags, name, FALSE)
		    || કિંમત_check_lock(d->dv_lock, name, FALSE)
		    || check_hashtab_frozen(ht, "unlet"))
		પાછા FAIL;

	    delete_var(ht, hi);
	    પાછા OK;
	}
    }
    જો (forceit)
	પાછા OK;
    semsg(_(e_no_such_variable_str), name);
    પાછા FAIL;
}

    સ્થિર શૂન્ય
report_lockvar_member(અક્ષર *સંદેશ, lval_T *lp)
{
    પૂર્ણાંક did_alloc = FALSE;
    char_u *vname = (char_u *)"";
    char_u *class_name = lp->ll_class != ખાલી
				    ? lp->ll_class->class_name.string : (char_u *)"";
    જો (lp->ll_name != ખાલી)
    {
	જો (lp->ll_name_end == ખાલી)
	    vname = lp->ll_name;
	નહીંતર
	{
	    vname = vim_strnsave(lp->ll_name, lp->ll_name_end - lp->ll_name);
	    જો (vname == ખાલી)
		પાછા;
	    did_alloc = TRUE;
	}
    }
    semsg(_(સંદેશ), vname, class_name);
    જો (did_alloc)
	vim_free(vname);
}

/*
 * Lock or unlock variable indicated by "lp".
 * "deep" is the levels to go (-1 for unlimited);
 * "lock" is TRUE for ":lockvar", FALSE for ":unlockvar".
 */
    સ્થિર પૂર્ણાંક
do_lock_var(
    lval_T	*lp,
    char_u	*name_end,
    exarg_T	*eap,
    પૂર્ણાંક		deep,
    શૂન્ય	*cookie UNUSED)
{
    પૂર્ણાંક		lock = eap->cmdidx == CMD_lockvar;
    પૂર્ણાંક		ret = OK;
    પૂર્ણાંક		cc;
    dictitem_T	*di;

#ifdef LOG_LOCKVAR
    ch_log(ખાલી, "LKVAR: do_lock_var(): name %s, is_root %d", lp->ll_name, lp->ll_is_મૂળ);
#endif

    જો (lp->ll_tv == ખાલી)
    {
	cc = *name_end;
	*name_end = NUL;
	જો (*lp->ll_name == '$')
	{
	    semsg(_(e_cannot_lock_or_unlock_variable_str), lp->ll_name);
	    ret = FAIL;
	}
	નહીંતર
	{
	    // Normal name or expanded name.
	    di = find_var(lp->ll_name, ખાલી, TRUE);
	    જો (di == ખાલી)
	    {
		જો (in_vim9script())
		    semsg(_(e_cannot_find_variable_to_unlock_str),
								  lp->ll_name);
		ret = FAIL;
	    }
	    નહીંતર
	    {
		જો ((di->di_flags & DI_FLAGS_FIX)
			    && di->di_tv.v_type != VAR_DICT
			    && di->di_tv.v_type != VAR_LIST)
		{
		    // For historic reasons this error is not given for a list
		    // or dict.  E.g., the b: dict could be locked/unlocked.
		    semsg(_(e_cannot_lock_or_unlock_variable_str), lp->ll_name);
		    ret = FAIL;
		}
		નહીંતર
		{
		    જો (in_vim9script())
		    {
			svar_T  *sv = find_typval_in_script(&di->di_tv,
								     0, FALSE);

			જો (sv != ખાલી && sv->sv_const != 0)
			{
			    semsg(_(e_cannot_change_readonly_variable_str),
								  lp->ll_name);
			    ret = FAIL;
			}
		    }

		    જો (ret == OK)
		    {
			જો (lock)
			    di->di_flags |= DI_FLAGS_LOCK;
			નહીંતર
			    di->di_flags &= ~DI_FLAGS_LOCK;
			જો (deep != 0)
			    item_lock(&di->di_tv, deep, lock, FALSE);
		    }
		}
	    }
	}
	*name_end = cc;
    }
    નહીંતર જો (deep == 0 && lp->ll_object == ખાલી && lp->ll_class == ખાલી)
    {
	// nothing to do
    }
    નહીંતર જો (lp->ll_is_મૂળ)
	// (un)lock the item.
	item_lock(lp->ll_tv, deep, lock, FALSE);
    નહીંતર જો (lp->ll_range)
    {
	listitem_T    *li = lp->ll_li;

	// (un)lock a range of List items.
	જ્યારે (li != ખાલી && (lp->ll_empty2 || lp->ll_n2 >= lp->ll_n1))
	{
	    item_lock(&li->li_tv, deep, lock, FALSE);
	    li = li->li_next;
	    ++lp->ll_n1;
	}
    }
    નહીંતર જો (lp->ll_list != ખાલી)
	// (un)lock a List item.
	item_lock(&lp->ll_li->li_tv, deep, lock, FALSE);
    નહીંતર જો (lp->ll_object != ખાલી)  // This check must be before ll_class.
    {
	// (un)lock an object variable.
	report_lockvar_member(e_cannot_lock_object_variable_str, lp);
	ret = FAIL;
    }
    નહીંતર જો (lp->ll_class != ખાલી)
    {
	// (un)lock a class variable.
	report_lockvar_member(e_cannot_lock_class_variable_str, lp);
	ret = FAIL;
    }
    નહીંતર
    {
	// (un)lock a Dictionary item.
	જો (lp->ll_di == ખાલી)
	{
	    emsg(_(e_dictionary_required));
	    ret = FAIL;
	}
	નહીંતર
	    item_lock(&lp->ll_di->di_tv, deep, lock, FALSE);
    }

    પાછા ret;
}

/*
 * Lock or unlock an item.  "deep" is nr of levels to go.
 * When "check_refcount" is TRUE do not lock a list or dict with a reference
 * count larger than 1.
 */
    શૂન્ય
item_lock(typval_T *tv, પૂર્ણાંક deep, પૂર્ણાંક lock, પૂર્ણાંક check_refcount)
{
    સ્થિર પૂર્ણાંક	recurse = 0;
    list_T	*l;
    listitem_T	*li;
    dict_T	*d;
    blob_T	*b;
    hashitem_T	*hi;
    પૂર્ણાંક		todo;

#ifdef LOG_LOCKVAR
    ch_log(ખાલી, "LKVAR: item_lock(): type %s", vartype_name(tv->v_type));
#endif

    જો (recurse >= DICT_MAXNEST)
    {
	emsg(_(e_variable_nested_too_deep_for_unlock));
	પાછા;
    }
    જો (deep == 0)
	પાછા;
    ++recurse;

    // lock/unlock the item itself
    જો (lock)
	tv->v_lock |= VAR_LOCKED;
    નહીંતર
	tv->v_lock &= ~VAR_LOCKED;

    પસંદગી (tv->v_type)
    {
	સ્થિતિ VAR_UNKNOWN:
	સ્થિતિ VAR_ANY:
	સ્થિતિ VAR_VOID:
	સ્થિતિ VAR_સંખ્યા:
	સ્થિતિ VAR_BOOL:
	સ્થિતિ VAR_STRING:
	સ્થિતિ VAR_FUNC:
	સ્થિતિ VAR_PARTIAL:
	સ્થિતિ VAR_FLOAT:
	સ્થિતિ VAR_SPECIAL:
	સ્થિતિ VAR_JOB:
	સ્થિતિ VAR_CHANNEL:
	સ્થિતિ VAR_INSTR:
	સ્થિતિ VAR_CLASS:
	સ્થિતિ VAR_OBJECT:
	સ્થિતિ VAR_TYPEALIAS:
	    તોડો;

	સ્થિતિ VAR_BLOB:
	    જો ((b = tv->vval.v_blob) != ખાલી
				    && !(check_refcount && b->bv_refcount > 1))
	    {
		જો (lock)
		    b->bv_lock |= VAR_LOCKED;
		નહીંતર
		    b->bv_lock &= ~VAR_LOCKED;
	    }
	    તોડો;
	સ્થિતિ VAR_LIST:
	    જો ((l = tv->vval.v_list) != ખાલી
				    && !(check_refcount && l->lv_refcount > 1))
	    {
		જો (lock)
		    l->lv_lock |= VAR_LOCKED;
		નહીંતર
		    l->lv_lock &= ~VAR_LOCKED;
		જો (deep < 0 || deep > 1)
		{
		    જો (l->lv_first == &range_list_item)
			l->lv_lock |= VAR_વસ્તુઓ_LOCKED;
		    નહીંતર
		    {
			// recursive: lock/unlock the items the List contains
			CHECK_LIST_MATERIALIZE(l);
			FOR_ALL_LIST_વસ્તુઓ(l, li) item_lock(&li->li_tv,
					       deep - 1, lock, check_refcount);
		    }
		}
	    }
	    તોડો;
	સ્થિતિ VAR_TUPLE:
	    tuple_lock(tv->vval.v_tuple, deep, lock, check_refcount);
	    તોડો;
	સ્થિતિ VAR_DICT:
	    જો ((d = tv->vval.v_dict) != ખાલી
				    && !(check_refcount && d->dv_refcount > 1))
	    {
		જો (lock)
		    d->dv_lock |= VAR_LOCKED;
		નહીંતર
		    d->dv_lock &= ~VAR_LOCKED;
		જો (deep < 0 || deep > 1)
		{
		    // recursive: lock/unlock the items the List contains
		    todo = (પૂર્ણાંક)d->dv_hashtab.ht_used;
		    FOR_ALL_HASHTAB_વસ્તુઓ(&d->dv_hashtab, hi, todo)
		    {
			જો (!HASHITEM_EMPTY(hi))
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

#જો defined(FEAT_MENU) && defined(FEAT_MULTI_LANG)
/*
 * Delete all "menutrans_" variables.
 */
    શૂન્ય
del_menutrans_vars(શૂન્ય)
{
    hashitem_T	*hi;
    પૂર્ણાંક		todo;

    hash_lock(&globvarht);
    todo = (પૂર્ણાંક)globvarht.ht_used;
    માટે (hi = globvarht.ht_array; todo > 0 && !got_int; ++hi)
    {
	જો (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    જો (STRNCMP(HI2DI(hi)->di_ચાવી, "menutrans_", 10) == 0)
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

સ્થિર char_u	*varnamebuf = ખાલી;
સ્થિર પૂર્ણાંક	varnamebuflen = 0;

/*
 * Function to concatenate a prefix and a variable name.
 */
    char_u *
cat_prefix_varname(પૂર્ણાંક prefix, char_u *name)
{
    પૂર્ણાંક		len;

    len = (પૂર્ણાંક)STRLEN(name) + 3;
    જો (len > varnamebuflen)
    {
	vim_free(varnamebuf);
	len += 10;			// some additional space
	varnamebuf = alloc(len);
	જો (varnamebuf == ખાલી)
	{
	    varnamebuflen = 0;
	    પાછા ખાલી;
	}
	varnamebuflen = len;
    }
    *varnamebuf = prefix;
    varnamebuf[1] = ':';
    STRCPY(varnamebuf + 2, name);
    પાછા varnamebuf;
}

/*
 * Function given to ExpandGeneric() to obtain the list of user defined
 * (global/buffer/window/built-in) variable names.
 */
    char_u *
get_user_var_name(expand_T *xp, પૂર્ણાંક idx)
{
    સ્થિર long_u	gdone;
    સ્થિર long_u	bdone;
    સ્થિર long_u	wdone;
    સ્થિર long_u	tdone;
    સ્થિર પૂર્ણાંક		vidx;
    સ્થિર hashitem_T	*hi;
    hashtab_T		*ht;

    જો (idx == 0)
    {
	gdone = bdone = wdone = vidx = 0;
	tdone = 0;
    }

    // Global variables
    જો (gdone < globvarht.ht_used)
    {
	જો (gdone++ == 0)
	    hi = globvarht.ht_array;
	નહીંતર
	    ++hi;
	જ્યારે (HASHITEM_EMPTY(hi))
	    ++hi;
	જો (STRNCMP("g:", xp->xp_pattern, 2) == 0)
	    પાછા cat_prefix_varname('g', hi->hi_ચાવી);
	પાછા hi->hi_ચાવી;
    }

    // b: variables
    ht = &prevwin_curwin()->w_buffer->b_vars->dv_hashtab;
    જો (bdone < ht->ht_used)
    {
	જો (bdone++ == 0)
	    hi = ht->ht_array;
	નહીંતર
	    ++hi;
	જ્યારે (HASHITEM_EMPTY(hi))
	    ++hi;
	પાછા cat_prefix_varname('b', hi->hi_ચાવી);
    }

    // w: variables
    ht = &prevwin_curwin()->w_vars->dv_hashtab;
    જો (wdone < ht->ht_used)
    {
	જો (wdone++ == 0)
	    hi = ht->ht_array;
	નહીંતર
	    ++hi;
	જ્યારે (HASHITEM_EMPTY(hi))
	    ++hi;
	પાછા cat_prefix_varname('w', hi->hi_ચાવી);
    }

    // t: variables
    ht = &curtab->tp_vars->dv_hashtab;
    જો (tdone < ht->ht_used)
    {
	જો (tdone++ == 0)
	    hi = ht->ht_array;
	નહીંતર
	    ++hi;
	જ્યારે (HASHITEM_EMPTY(hi))
	    ++hi;
	પાછા cat_prefix_varname('t', hi->hi_ચાવી);
    }

    // v: variables
    જો (vidx < VV_LEN)
	પાછા cat_prefix_varname('v', (char_u *)vimvars[vidx++].vv_name);

    VIM_CLEAR(varnamebuf);
    varnamebuflen = 0;
    પાછા ખાલી;
}

    અક્ષર *
get_var_special_name(પૂર્ણાંક nr)
{
    પસંદગી (nr)
    {
	સ્થિતિ VVAL_FALSE: પાછા in_vim9script() ? "false" : "v:false";
	સ્થિતિ VVAL_TRUE:  પાછા in_vim9script() ? "true" : "v:true";
	સ્થિતિ VVAL_NULL:  પાછા in_vim9script() ? "null" : "v:null";
	સ્થિતિ VVAL_NONE:  પાછા "v:none";
    }
    internal_error("get_var_special_name()");
    પાછા "42";
}

/*
 * Returns the global variable dictionary
 */
    dict_T *
get_globvar_dict(શૂન્ય)
{
    પાછા &globvardict;
}

/*
 * Returns the global variable hash table
 */
    hashtab_T *
get_globvar_ht(શૂન્ય)
{
    પાછા &globvarht;
}

/*
 * Returns the v: variable dictionary
 */
    dict_T *
get_vimvar_dict(શૂન્ય)
{
    પાછા &vimvardict;
}

/*
 * Returns the index of a v:variable.  Negative if not found.
 * Returns DI_ flags in "di_flags".
 */
    પૂર્ણાંક
find_vim_var(char_u *name, પૂર્ણાંક *di_flags)
{
    dictitem_T	    *di = find_var_in_ht(&vimvarht, 0, name, TRUE);
    સંરચના vimvar   *vv;

    જો (di == ખાલી)
	પાછા -1;
    *di_flags = di->di_flags;
    vv = (સંરચના vimvar *)((અક્ષર *)di - offsetof(vimvar_T, vv_di));
    પાછા (પૂર્ણાંક)(vv - vimvars);
}


/*
 * Set type of v: variable to "type".
 */
    શૂન્ય
set_vim_var_type(પૂર્ણાંક idx, vartype_T type)
{
    vimvars[idx].vv_tv_type = type;
}

/*
 * Set number v: variable to "val".
 * Note that this does not set the type, use set_vim_var_type() for that.
 */
    શૂન્ય
set_vim_var_nr(પૂર્ણાંક idx, varnumber_T val)
{
    vimvars[idx].vv_nr = val;
}

    અક્ષર *
get_vim_var_name(પૂર્ણાંક idx)
{
    પાછા vimvars[idx].vv_name;
}

/*
 * Get typval_T v: variable value.
 */
    typval_T *
get_vim_var_tv(પૂર્ણાંક idx)
{
    પાછા &vimvars[idx].vv_tv;
}

    type_T *
get_vim_var_type(પૂર્ણાંક idx, garray_T *type_list)
{
    જો (vimvars[idx].vv_type != ખાલી)
	પાછા vimvars[idx].vv_type;
    પાછા typval2type_vimvar(&vimvars[idx].vv_tv, type_list);
}

/*
 * Add what getinfo() reports for the v: variable "name", without the "v:", to
 * "d".  Returns FAIL when there is no such variable.
 */
    પૂર્ણાંક
vim_var_info(char_u *name, dict_T *d)
{
    પૂર્ણાંક		di_flags;
    પૂર્ણાંક		idx = find_vim_var(name, &di_flags);
    garray_T	type_list;
    અક્ષર	*tofree;

    જો (idx < 0)
	પાછા FAIL;
    vim_snprintf((અક્ષર *)IObuff, IOSIZE, "v:%s", vimvars[idx].vv_name);
    dict_add_string(d, "name", IObuff);
    dict_add_bool(d, "available", TRUE);
    ga_init2(&type_list, કદ(type_T *), 10);
    અક્ષર *type = type_name(get_vim_var_type(idx, &type_list), &tofree);
    dict_add_string(d, "type", (char_u *)type);
    vim_free(tofree);
    clear_type_list(&type_list);
    dict_add_bool(d, "readonly", (vimvars[idx].vv_flags & VV_RO) != 0);
    dict_add_bool(d, "compat", (vimvars[idx].vv_flags & VV_COMPAT) != 0);
    પાછા OK;
}

/*
 * Set v: variable to "tv".  Only accepts the same type.
 * Takes over the value of "tv".
 */
    પૂર્ણાંક
set_vim_var_tv(પૂર્ણાંક idx, typval_T *tv)
{
    જો (vimvars[idx].vv_tv_type != tv->v_type)
    {
	emsg(_(e_type_mismatch_for_v_variable));
	clear_tv(tv);
	પાછા FAIL;
    }
    // VV_RO is also checked when compiling, but let's check here as well.
    જો (vimvars[idx].vv_flags & VV_RO)
    {
	semsg(_(e_cannot_change_readonly_variable_str), vimvars[idx].vv_name);
	પાછા FAIL;
    }
    જો (sandbox && (vimvars[idx].vv_flags & VV_RO_SBX))
    {
	semsg(_(e_cannot_set_variable_in_sandbox_str), vimvars[idx].vv_name);
	પાછા FAIL;
    }
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_di.di_tv = *tv;
    પાછા OK;
}

/*
 * Get number v: variable value.
 */
    varnumber_T
get_vim_var_nr(પૂર્ણાંક idx)
{
    પાછા vimvars[idx].vv_nr;
}

/*
 * Get string v: variable value.  Uses a static buffer, can only be used once.
 * If the String variable has never been set, return an empty string.
 * Never returns NULL;
 */
    char_u *
get_vim_var_str(પૂર્ણાંક idx)
{
    પાછા tv_get_string(&vimvars[idx].vv_tv);
}

/*
 * Get List v: variable value.  Caller must take care of reference count when
 * needed.
 */
    list_T *
get_vim_var_list(પૂર્ણાંક idx)
{
    પાછા vimvars[idx].vv_list;
}

/*
 * Get Dict v: variable value.  Caller must take care of reference count when
 * needed.
 */
    dict_T *
get_vim_var_dict(પૂર્ણાંક idx)
{
    પાછા vimvars[idx].vv_dict;
}

/*
 * Set v:char to character "c".
 */
    શૂન્ય
set_vim_var_char(પૂર્ણાંક c)
{
    char_u  buf[MB_MAXBYTES + 1];
    size_t  buflen;

    જો (has_mbyte)
	buflen = (*mb_char2bytes)(c, buf);
    નહીંતર
    {
	buf[0] = c;
	buflen = 1;
    }
    buf[buflen] = NUL;

    set_vim_var_string(VV_CHAR, buf, (પૂર્ણાંક)buflen);
}

/*
 * Set v:count to "count" and v:count1 to "count1".
 * When "set_prevcount" is TRUE first set v:prevcount from v:count.
 */
    શૂન્ય
set_vcount(
    long	ગણતરી,
    long	count1,
    પૂર્ણાંક		set_prevcount)
{
    જો (set_prevcount)
	vimvars[VV_PREVCOUNT].vv_nr = vimvars[VV_ગણતરી].vv_nr;
    vimvars[VV_ગણતરી].vv_nr = ગણતરી;
    vimvars[VV_COUNT1].vv_nr = count1;
}

/*
 * Save variables that might be changed as a side effect.  Used when executing
 * a timer callback.
 */
    શૂન્ય
save_vimvars(vimvars_save_T *vvsave)
{
    vvsave->vv_prevcount = vimvars[VV_PREVCOUNT].vv_nr;
    vvsave->vv_ગણતરી = vimvars[VV_ગણતરી].vv_nr;
    vvsave->vv_count1 = vimvars[VV_COUNT1].vv_nr;
}

/*
 * Restore variables saved by save_vimvars().
 */
    શૂન્ય
restore_vimvars(vimvars_save_T *vvsave)
{
    vimvars[VV_PREVCOUNT].vv_nr = vvsave->vv_prevcount;
    vimvars[VV_ગણતરી].vv_nr = vvsave->vv_ગણતરી;
    vimvars[VV_COUNT1].vv_nr = vvsave->vv_count1;
}

/*
 * Set string v: variable to a copy of "val". If 'copy' is FALSE, then set the
 * value.
 */
    શૂન્ય
set_vim_var_string(
    પૂર્ણાંક		idx,
    char_u	*val,
    પૂર્ણાંક		len)	    // length of "val" to use or -1 (whole string)
{
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_tv_type = VAR_STRING;
    જો (val == ખાલી)
	vimvars[idx].vv_str = ખાલી;
    નહીંતર જો (len == -1)
	vimvars[idx].vv_str = vim_strsave(val);
    નહીંતર
	vimvars[idx].vv_str = vim_strnsave(val, len);
}

    શૂન્ય
set_vim_var_string_direct(
    પૂર્ણાંક		idx,
    char_u	*val)
{
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_tv_type = VAR_STRING;

    vimvars[idx].vv_str = val;
}

/*
 * Set List v: variable to "val".
 */
    શૂન્ય
set_vim_var_list(પૂર્ણાંક idx, list_T *val)
{
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_tv_type = VAR_LIST;
    vimvars[idx].vv_list = val;
    જો (val != ખાલી)
	++val->lv_refcount;
}

/*
 * Set Dictionary v: variable to "val".
 */
    શૂન્ય
set_vim_var_dict(પૂર્ણાંક idx, dict_T *val)
{
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_tv_type = VAR_DICT;
    vimvars[idx].vv_dict = val;
    જો (val == ખાલી)
	પાછા;

    ++val->dv_refcount;
    dict_set_વસ્તુઓ_ro(val);
}

/*
 * Set the v:argv list.
 */
    શૂન્ય
set_argv_var(અક્ષર **argv, પૂર્ણાંક argc)
{
    list_T	*l = list_alloc();
    પૂર્ણાંક		i;

    જો (l == ખાલી)
	getout(1);
    l->lv_lock = VAR_FIXED;
    માટે (i = 0; i < argc; ++i)
    {
	જો (list_append_string(l, (char_u *)argv[i], -1) == FAIL)
	    getout(1);
	l->lv_u.mat.lv_last->li_tv.v_lock = VAR_FIXED;
    }
    set_vim_var_list(VV_ARGV, l);
}

/*
 * Reset v:register, taking the 'clipboard' setting into account.
 */
    શૂન્ય
reset_reg_var(શૂન્ય)
{
    પૂર્ણાંક regname = 0;

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
    શૂન્ય
set_reg_var(પૂર્ણાંક c)
{
    char_u  regname[2];

    જો (c == 0 || c == ' ')
	regname[0] = '"';
    નહીંતર
	regname[0] = c;
    regname[1] = NUL;
    // Avoid free/alloc when the value is already right.
    જો (vimvars[VV_REG].vv_str == ખાલી || vimvars[VV_REG].vv_str[0] != c)
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
    જો (oldval == ખાલી)
	પાછા vimvars[VV_EXCEPTION].vv_str;

    vimvars[VV_EXCEPTION].vv_str = oldval;
    પાછા ખાલી;
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
    જો (oldval == ખાલી)
	પાછા vimvars[VV_THROWPOINT].vv_str;

    vimvars[VV_THROWPOINT].vv_str = oldval;
    પાછા ખાલી;
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
    જો (eap == ખાલી)
    {
	vim_free(oldval);
	vimvars[VV_CMDARG].vv_str = oldarg;
	પાછા ખાલી;
    }

    જો (eap->force_bin == FORCE_BIN)
	len = 6;
    નહીંતર જો (eap->force_bin == FORCE_NOBIN)
	len = 8;
    નહીંતર
	len = 0;

    જો (eap->read_edit)
	len += 7;

    જો (eap->force_ff != 0)
	len += 10; // " ++ff=unix"
    જો (eap->force_enc != 0)
	len += (unsigned)STRLEN(eap->cmd + eap->force_enc) + 7;
    જો (eap->bad_char != 0)
	len += 7 + 4;  // " ++bad=" + "keep" or "drop"

    newval = alloc(len + 1);
    જો (newval == ખાલી)
	પાછા ખાલી;

    જો (eap->force_bin == FORCE_BIN)
	sprintf((અક્ષર *)newval, " ++bin");
    નહીંતર જો (eap->force_bin == FORCE_NOBIN)
	sprintf((અક્ષર *)newval, " ++nobin");
    નહીંતર
	*newval = NUL;

    જો (eap->read_edit)
	STRCAT(newval, " ++edit");

    જો (eap->force_ff != 0)
	sprintf((અક્ષર *)newval + STRLEN(newval), " ++ff=%s",
						eap->force_ff == 'u' ? "unix"
						: eap->force_ff == 'd' ? "dos"
						: "mac");
    જો (eap->force_enc != 0)
	sprintf((અક્ષર *)newval + STRLEN(newval), " ++enc=%s",
					       eap->cmd + eap->force_enc);
    જો (eap->bad_char == BAD_KEEP)
	STRCPY(newval + STRLEN(newval), " ++bad=keep");
    નહીંતર જો (eap->bad_char == BAD_DROP)
	STRCPY(newval + STRLEN(newval), " ++bad=drop");
    નહીંતર જો (eap->bad_char != 0)
	sprintf((અક્ષર *)newval + STRLEN(newval), " ++bad=%c", eap->bad_char);
    vimvars[VV_CMDARG].vv_str = newval;
    પાછા oldval;
}

/*
 * Get the value of internal variable "name".
 * If "flags" has EVAL_VAR_IMPORT may return a VAR_ANY with v_number set to the
 * imported script ID.
 * Return OK or FAIL.  If OK is returned "rettv" must be cleared.
 */
    પૂર્ણાંક
eval_variable(
    char_u	*name,
    પૂર્ણાંક		len,		// length of "name" or zero
    scid_T	sid,		// script ID for imported item or zero
    typval_T	*rettv,		// NULL when only checking existence
    dictitem_T	**dip,		// non-NULL when typval's dict item is needed
    પૂર્ણાંક		flags)		// EVAL_VAR_ flags
{
    પૂર્ણાંક		ret = OK;
    typval_T	*tv = ખાલી;
    પૂર્ણાંક		found = FALSE;
    hashtab_T	*ht = ખાલી;
    પૂર્ણાંક		cc = 0;
    type_T	*type = ખાલી;

    જો (len > 0)
    {
	// truncate the name, so that we can use strcmp()
	cc = name[len];
	name[len] = NUL;
    }

    // Check for local variable when debugging.
    જો ((sid == 0) && (tv = lookup_debug_var(name)) == ખાલી)
    {
	// Check for user-defined variables.
	dictitem_T	*v = find_var(name, &ht, flags & EVAL_VAR_NOAUTOLOAD);

	જો (v != ખાલી)
	{
	    tv = &v->di_tv;
	    જો (dip != ખાલી)
		*dip = v;
	}
	નહીંતર
	    ht = ખાલી;
    }

    જો (tv == ખાલી && (in_vim9script() || STRNCMP(name, "s:", 2) == 0))
    {
	imported_T  *import = ખાલી;
	char_u	    *p = STRNCMP(name, "s:", 2) == 0 ? name + 2 : name;

	જો (sid == 0)
	    import = find_imported(p, 0, TRUE);

	// imported variable from another script
	જો (import != ખાલી || sid != 0)
	{
	    જો ((flags & EVAL_VAR_IMPORT) == 0)
	    {
		જો (SCRIPT_ID_VALID(sid))
		{
		    ht = &SCRIPT_VARS(sid);
		    જો (ht != ખાલી)
		    {
			dictitem_T *v = find_var_in_ht(ht, 0, name,
						  flags & EVAL_VAR_NOAUTOLOAD);

			જો (v == ખાલી)
			    v = find_var_autoload_prefix(name, sid, ખાલી, ખાલી);

			જો (v != ખાલી)
			{
			    tv = &v->di_tv;
			    જો (dip != ખાલી)
				*dip = v;
			}
			નહીંતર
			    ht = ખાલી;
		    }
		}
		નહીંતર
		{
		    જો (flags & EVAL_VAR_VERBOSE)
			semsg(_(e_expected_dot_after_name_str), name);
		    ret = FAIL;
		}
	    }
	    નહીંતર
	    {
		જો (rettv != ખાલી)
		{
		    // special value that is used in handle_subscript()
		    rettv->v_type = VAR_ANY;
		    rettv->vval.v_સંખ્યા = sid != 0 ? sid : import->imp_sid;
		}
		found = TRUE;
	    }
	}
	નહીંતર જો (in_vim9script() && (flags & EVAL_VAR_NO_FUNC) == 0)
	{
	    પૂર્ણાંક	    has_g_prefix = STRNCMP(name, "g:", 2) == 0;
	    ufunc_T *ufunc = find_func(name, FALSE);

	    જો (ufunc != ખાલી && cc == '<')
	    {
		// handle generic function
		char_u	*argp = name + len;
		name[len] = cc;
		ufunc = eval_generic_func(ufunc, name, &argp);
		name[len] = NUL;
		જો (ufunc == ખાલી)
		{
		    ret = FAIL;
		    જાઓ done;
		}
	    }

	    // In Vim9 script we can get a function reference by using the
	    // function name.  For a global non-autoload function "g:" is
	    // required.
	    જો (ufunc != ખાલી && (has_g_prefix
					    || !func_requires_g_prefix(ufunc)))
	    {
		found = TRUE;
		જો (rettv != ખાલી)
		{
		    rettv->v_type = VAR_FUNC;
		    જો (has_g_prefix)
		    {
			// Keep the "g:", otherwise script-local may be
			// assumed.
			જો (cc != '<')
			    rettv->vval.v_string = vim_strsave(name);
			નહીંતર
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
		    નહીંતર
			rettv->vval.v_string = vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
		    જો (rettv->vval.v_string != ખાલી)
			func_ref(ufunc->uf_name);
		}
	    }
	}
    }

    જો (!found)
    {
	જો (tv == ખાલી)
	{
	    જો (rettv != ખાલી && (flags & EVAL_VAR_VERBOSE))
		semsg(_(e_undefined_variable_str), name);
	    ret = FAIL;
	}
	નહીંતર જો (rettv != ખાલી)
	{
	    svar_T  *sv = ખાલી;
	    પૂર્ણાંક	    was_assigned = FALSE;

	    જો (ht != ખાલી && ht == get_script_local_ht()
		    && tv != &SCRIPT_SV(current_sctx.sc_sid)->sv_var.di_tv)
	    {
		sv = find_typval_in_script(tv, 0, TRUE);
		જો (sv != ખાલી)
		{
		    type = sv->sv_type;
		    was_assigned = sv->sv_flags & SVFLAG_ASSIGNED;
		}
	    }

	    જો ((tv->v_type == VAR_TYPEALIAS || tv->v_type == VAR_CLASS)
		    && sid != 0)
	    {
		// type alias or class imported from another script.  Check
		// whether it is exported from the other script.
		sv = find_typval_in_script(tv, sid, TRUE);
		જો (sv == ખાલી)
		{
		    ret = FAIL;
		    જાઓ done;
		}
		જો ((sv->sv_flags & SVFLAG_EXPORTED) == 0)
		{
		    semsg(_(e_item_not_exported_in_script_str), name);
		    ret = FAIL;
		    જાઓ done;
		}
	    }

	    // If a list or tuple or dict variable wasn't initialized and has
	    // meaningful type, do it now.  Not for global variables, they are
	    // not declared.
	    જો (ht != &globvarht)
	    {
		જો (tv->v_type == VAR_DICT && tv->vval.v_dict == ખાલી
					    && ((type != ખાલી && !was_assigned)
							  || !in_vim9script()))
		{
		    tv->vval.v_dict = dict_alloc();
		    જો (tv->vval.v_dict != ખાલી)
		    {
			++tv->vval.v_dict->dv_refcount;
			tv->vval.v_dict->dv_type = alloc_type(type);
			જો (sv != ખાલી)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
		નહીંતર જો (tv->v_type == VAR_LIST && tv->vval.v_list == ખાલી
					    && ((type != ખાલી && !was_assigned)
							  || !in_vim9script()))
		{
		    tv->vval.v_list = list_alloc();
		    જો (tv->vval.v_list != ખાલી)
		    {
			++tv->vval.v_list->lv_refcount;
			tv->vval.v_list->lv_type = alloc_type(type);
			જો (sv != ખાલી)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
		નહીંતર જો (tv->v_type == VAR_TUPLE && tv->vval.v_tuple == ખાલી
					    && ((type != ખાલી && !was_assigned)
							  || !in_vim9script()))
		{
		    tv->vval.v_tuple = tuple_alloc();
		    જો (tv->vval.v_tuple != ખાલી)
		    {
			++tv->vval.v_tuple->tv_refcount;
			tv->vval.v_tuple->tv_type = alloc_type(type);
			જો (sv != ખાલી)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
		નહીંતર જો (tv->v_type == VAR_BLOB && tv->vval.v_blob == ખાલી
					    && ((type != ખાલી && !was_assigned)
							  || !in_vim9script()))
		{
		    tv->vval.v_blob = blob_alloc();
		    જો (tv->vval.v_blob != ખાલી)
		    {
			++tv->vval.v_blob->bv_refcount;
			જો (sv != ખાલી)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
	    }
	    ret = copy_tv(tv, rettv);
	}
    }

done:
    જો (len > 0)
	name[len] = cc;

    પાછા ret;
}

/*
 * Get the value of internal variable "name", also handling "import.name".
 * Return OK or FAIL.  If OK is returned "rettv" must be cleared.
 */
    પૂર્ણાંક
eval_variable_import(
    char_u	*name,
    typval_T	*rettv)
{
    char_u  *s = name;
    જ્યારે (ASCII_ISALNUM(*s) || *s == '_')
	++s;
    પૂર્ણાંક	    len = (પૂર્ણાંક)(s - name);

    જો (eval_variable(name, len, 0, rettv, ખાલી, EVAL_VAR_IMPORT) == FAIL)
	પાછા FAIL;
    જો (rettv->v_type == VAR_ANY && *s == '.')
    {
	char_u *ns = s + 1;
	s = ns;
	જ્યારે (ASCII_ISALNUM(*s) || *s == '_')
	    ++s;
	પૂર્ણાંક sid = rettv->vval.v_સંખ્યા;
	પાછા eval_variable(ns, (પૂર્ણાંક)(s - ns), sid, rettv, ખાલી, 0);
    }
    પાછા OK;
}


/*
 * Check if variable "name[len]" is a local variable or an argument.
 * If so, "*eval_lavars_used" is set to TRUE.
 */
    શૂન્ય
check_vars(char_u *name, પૂર્ણાંક len)
{
    પૂર્ણાંક		cc;
    char_u	*varname;
    hashtab_T	*ht;

    જો (eval_lavars_used == ખાલી)
	પાછા;

    // truncate the name, so that we can use strcmp()
    cc = name[len];
    name[len] = NUL;

    ht = find_var_ht(name, &varname);
    જો (ht == get_funccal_local_ht() || ht == get_funccal_args_ht())
    {
	જો (find_var(name, ખાલી, TRUE) != ખાલી)
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
find_var(char_u *name, hashtab_T **htp, પૂર્ણાંક no_autoload)
{
    char_u	*varname;
    hashtab_T	*ht;
    dictitem_T	*ret = ખાલી;

    ht = find_var_ht(name, &varname);
    જો (htp != ખાલી)
	*htp = ht;
    જો (ht == ખાલી)
	પાછા ખાલી;
    ret = find_var_in_ht(ht, *name, varname, no_autoload);
    જો (ret != ખાલી)
	પાછા ret;

    // Search in parent scope for lambda
    ret = find_var_in_scoped_ht(name, no_autoload);
    જો (ret != ખાલી)
	પાછા ret;

    // in Vim9 script items without a scope can be script-local
    જો (in_vim9script() && name[0] != NUL && name[1] != ':')
    {
	ht = get_script_local_ht();
	જો (ht != ખાલી)
	{
	    ret = find_var_in_ht(ht, *name, varname, no_autoload);
	    જો (ret != ખાલી)
	    {
		જો (htp != ખાલી)
		    *htp = ht;
		પાછા ret;
	    }
	}
    }

    // and finally try
    પાછા find_var_autoload_prefix(name, 0, htp, ખાલી);
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
find_var_autoload_prefix(char_u *name, પૂર્ણાંક sid, hashtab_T **htp,
							    char_u **namep)
{
    hashtab_T	*ht;
    dictitem_T	*ret = ખાલી;
    // When using "vim9script autoload" script-local items are prefixed but can
    // be used with s:name.
    પૂર્ણાંક check_sid = sid > 0 ? sid : current_sctx.sc_sid;
    જો (SCRIPT_ID_VALID(check_sid)
		   && (in_vim9script() || (name[0] == 's' && name[1] == ':')))
    {
	scriptitem_T *si = SCRIPT_ITEM(check_sid);

	જો (si->sn_autoload_prefix != ખાલી)
	{
	    char_u *base_name = (name[0] == 's' && name[1] == ':')
							     ? name + 2 : name;
	    char_u *auto_name = concat_str(si->sn_autoload_prefix, base_name);

	    જો (auto_name != ખાલી)
	    {
		પૂર્ણાંક free_auto_name = TRUE;
		ht = &globvarht;
		ret = find_var_in_ht(ht, 'g', auto_name, TRUE);
		જો (ret != ખાલી)
		{
		    જો (htp != ખાલી)
			*htp = ht;
		    જો (namep != ખાલી)
		    {
			free_auto_name = FALSE;
			*namep = auto_name;
		    }
		}
		જો (free_auto_name)
		    vim_free(auto_name);
	    }
	}
    }

    પાછા ret;
}

/*
 * Like find_var() but if the name starts with <SNR>99_ then look in the
 * referenced script (used for a funcref).
 */
    dictitem_T *
find_var_also_in_script(char_u *name, hashtab_T **htp, પૂર્ણાંક no_autoload)
{
    જો (STRNCMP(name, "<SNR>", 5) == 0 && SAFE_isdigit(name[5]))
    {
	char_u	    *p = name + 5;
	પૂર્ણાંક	    sid = getdigits(&p);

	જો (SCRIPT_ID_VALID(sid) && *p == '_')
	{
	    hashtab_T	*ht = &SCRIPT_VARS(sid);

	    જો (ht != ખાલી)
	    {
		dictitem_T *di = find_var_in_ht(ht, 0, p + 1, no_autoload);

		જો (di != ખાલી)
		{
		    જો (htp != ખાલી)
			*htp = ht;
		    પાછા di;
		}
	    }
	}
    }

    પાછા find_var(name, htp, no_autoload);
}

/*
 * Find variable "varname" in hashtab "ht" with name "htname".
 * When "varname" is empty returns curwin/curtab/etc vars dictionary.
 * Returns NULL if not found.
 */
    dictitem_T *
find_var_in_ht(
    hashtab_T	*ht,
    પૂર્ણાંક		htname,
    char_u	*varname,
    પૂર્ણાંક		no_autoload)
{
    hashitem_T	*hi;

    જો (*varname == NUL)
    {
	// Must be something like "s:", otherwise "ht" would be NULL.
	પસંદગી (htname)
	{
	    સ્થિતિ 's': પાછા &SCRIPT_SV(current_sctx.sc_sid)->sv_var;
	    સ્થિતિ 'g': પાછા &globvars_var;
	    સ્થિતિ 'v': પાછા &vimvars_var;
	    સ્થિતિ 'b': પાછા &curbuf->b_bufvar;
	    સ્થિતિ 'w': પાછા &curwin->w_winvar;
	    સ્થિતિ 't': પાછા &curtab->tp_winvar;
	    સ્થિતિ 'l': પાછા get_funccal_local_var();
	    સ્થિતિ 'a': પાછા get_funccal_args_var();
	}
	પાછા ખાલી;
    }

    hi = hash_find(ht, varname);
    જો (HASHITEM_EMPTY(hi))
    {
	// For global variables we may try auto-loading the script.  If it
	// worked find the variable again.  Don't auto-load a script if it was
	// loaded already, otherwise it would be loaded every time when
	// checking if a function name is a Funcref variable.
	જો (ht == &globvarht && !no_autoload)
	{
	    // Note: script_autoload() may make "hi" invalid. It must either
	    // be obtained again or not used.
	    જો (!script_autoload(varname, FALSE) || aborting())
		પાછા ખાલી;
	    hi = hash_find(ht, varname);
	}
	જો (HASHITEM_EMPTY(hi))
	    પાછા ખાલી;
    }
    પાછા HI2DI(hi);
}

/*
 * Get the script-local hashtab.  NULL if not in a script context.
 */
    hashtab_T *
get_script_local_ht(શૂન્ય)
{
    scid_T sid = current_sctx.sc_sid;

    જો (SCRIPT_ID_VALID(sid))
	પાછા &SCRIPT_VARS(sid);
    પાછા ખાલી;
}

/*
 * Look for "name[len]" in script-local variables and functions.
 * When "cmd" is TRUE it must look like a command, a function must be followed
 * by "(" or "->".
 * Return OK when found, FAIL when not found.
 */
    પૂર્ણાંક
lookup_scriptitem(
	char_u	*name,
	size_t	len,
	પૂર્ણાંક	cmd,
	cctx_T	*dummy UNUSED)
{
    hashtab_T	*ht = get_script_local_ht();
    char_u	buffer[30];
    char_u	*p;
    પૂર્ણાંક		res;
    hashitem_T	*hi;
    પૂર્ણાંક		is_global = FALSE;
    char_u	*fname = name;

    જો (ht == ખાલી)
	પાછા FAIL;
    જો (len < કદ(buffer) - 1)
    {
	// avoid an alloc/free for short names
	vim_strncpy(buffer, name, len);
	p = buffer;
    }
    નહીંતર
    {
	p = vim_strnsave(name, len);
	જો (p == ખાલી)
	    પાછા FAIL;
    }

    hi = hash_find(ht, p);
    res = HASHITEM_EMPTY(hi) ? FAIL : OK;

    // if not script-local, then perhaps autoload-exported
    જો (res == FAIL && find_var_autoload_prefix(p, 0, ખાલી, ખાલી) != ખાલી)
	res = OK;

    // if not script-local or autoload, then perhaps imported
    જો (res == FAIL && find_imported(p, 0, FALSE) != ખાલી)
	res = OK;
    જો (p != buffer)
	vim_free(p);

    // Find a function, so that a following "->" works.
    // When used as a command require "(" or "->" to follow, "Cmd" is a user
    // command while "Cmd()" is a function call.
    જો (res != OK)
    {
	p = skipwhite(name + len);

	જો (!cmd || name[len] == '(' || (p[0] == '-' && p[1] == '>'))
	{
	    // Do not check for an internal function, since it might also be a
	    // valid command, such as ":split" versus "split()".
	    // Skip "g:" before a function name.
	    જો (name[0] == 'g' && name[1] == ':')
	    {
		is_global = TRUE;
		fname = name + 2;
	    }
	    જો (find_func(fname, is_global) != ખાલી)
		res = OK;
	}
    }

    પાછા res;
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

    જો (name[0] == NUL)
	પાછા ખાલી;
    જો (name[1] != ':')
    {
	// The name must not start with a colon or #.
	જો (name[0] == ':' || name[0] == AUTOLOAD_CHAR)
	    પાછા ખાલી;
	*varname = name;

	// "version" is "v:version" in all scopes if scriptversion < 3.
	// Same for a few other variables marked with VV_COMPAT.
	જો (in_old_script(3))
	{
	    hi = hash_find(&compat_hashtab, name);
	    જો (!HASHITEM_EMPTY(hi))
		પાછા &compat_hashtab;
	}

	ht = get_funccal_local_ht();
	જો (ht != ખાલી)
	    પાછા ht;				// local variable

	// In Vim9 script items at the script level are script-local, except
	// for autoload names.
	જો (in_vim9script() && vim_strchr(name, AUTOLOAD_CHAR) == ખાલી)
	{
	    ht = get_script_local_ht();
	    જો (ht != ખાલી)
		પાછા ht;
	}

	પાછા &globvarht;			// global variable
    }
    *varname = name + 2;
    જો (*name == 'g')				// global variable
	પાછા &globvarht;
    // There must be no ':' or '#' in the rest of the name, unless g: is used
    જો (vim_strchr(name + 2, ':') != ખાલી
			       || vim_strchr(name + 2, AUTOLOAD_CHAR) != ખાલી)
	પાછા ખાલી;
    જો (*name == 'b')				// buffer variable
	પાછા &curbuf->b_vars->dv_hashtab;
    જો (*name == 'w')				// window variable
	પાછા &curwin->w_vars->dv_hashtab;
    જો (*name == 't')				// tab page variable
	પાછા &curtab->tp_vars->dv_hashtab;
    જો (*name == 'v')				// v: variable
	પાછા &vimvarht;
    જો (get_current_funccal() != ખાલી
	       && get_current_funccal()->fc_func->uf_def_status
							    == UF_NOT_COMPILED)
    {
	// a: and l: are only used in functions defined with ":function"
	જો (*name == 'a')			// a: function argument
	    પાછા get_funccal_args_ht();
	જો (*name == 'l')			// l: local function variable
	    પાછા get_funccal_local_ht();
    }
    જો (*name == 's')				// script variable
    {
	ht = get_script_local_ht();
	જો (ht != ખાલી)
	    પાછા ht;
    }
    પાછા ખાલી;
}

/*
 * Get the string value of a (global/local) variable.
 * Note: see tv_get_string() for how long the pointer remains valid.
 * Returns NULL when it doesn't exist.
 */
    char_u *
get_var_કિંમત(char_u *name)
{
    dictitem_T	*v;

    v = find_var(name, ખાલી, FALSE);
    જો (v == ખાલી)
	પાછા ખાલી;
    પાછા tv_get_string(&v->di_tv);
}

/*
 * Allocate a new hashtab for a sourced script.  It will be used while
 * sourcing this script and when executing functions defined in the script.
 */
    શૂન્ય
new_script_vars(scid_T id)
{
    scriptvar_T *sv;

    sv = ALLOC_CLEAR_ONE(scriptvar_T);
    જો (sv == ખાલી)
	પાછા;
    init_var_dict(&sv->sv_dict, &sv->sv_var, VAR_SCOPE);
    SCRIPT_ITEM(id)->sn_vars = sv;
}

/*
 * Initialize dictionary "dict" as a scope and set variable "dict_var" to
 * point to it.
 */
    શૂન્ય
init_var_dict(dict_T *dict, dictitem_T *dict_var, પૂર્ણાંક scope)
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
    dict_var->di_ચાવી[0] = NUL;
}

/*
 * Unreference a dictionary initialized by init_var_dict().
 */
    શૂન્ય
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
    શૂન્ય
vars_clear(hashtab_T *ht)
{
    vars_clear_ext(ht, TRUE);
}

/*
 * Like vars_clear(), but only free the value if "free_val" is TRUE.
 */
    શૂન્ય
vars_clear_ext(hashtab_T *ht, પૂર્ણાંક free_val)
{
    પૂર્ણાંક		todo;
    hashitem_T	*hi;
    dictitem_T	*v;

    hash_lock(ht);
    todo = (પૂર્ણાંક)ht->ht_used;
    FOR_ALL_HASHTAB_વસ્તુઓ(ht, hi, todo)
    {
	જો (!HASHITEM_EMPTY(hi))
	{
	    --todo;

	    // Free the variable.  Don't remove it from the hashtab,
	    // ht_array might change then.  hash_clear() takes care of it
	    // later.
	    v = HI2DI(hi);
	    જો (free_val)
		clear_tv(&v->di_tv);
	    જો (v->di_flags & DI_FLAGS_ALLOC)
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
    શૂન્ય
delete_var(hashtab_T *ht, hashitem_T *hi)
{
    dictitem_T	*di = HI2DI(hi);

    જો (hash_remove(ht, hi, "delete variable") != OK)
	પાછા;

    clear_tv(&di->di_tv);
    vim_free(di);
}

/*
 * Delete the exported variables of a reloaded Vim9 autoload script.  They live
 * in the global namespace with the autoload prefix (e.g. "foo#bar") and are
 * recreated when the script body runs again.
 */
    શૂન્ય
delete_autoload_export_vars(char_u *prefix)
{
    જો (prefix == ખાલી)
	પાછા;
    size_t  prefixlen = STRLEN(prefix);

    hash_lock(&globvarht);
    પૂર્ણાંક	todo = (પૂર્ણાંક)globvarht.ht_used;

    માટે (hashitem_T *hi = globvarht.ht_array; todo > 0; ++hi)
	જો (!HASHITEM_EMPTY(hi))
	{
	    dictitem_T	*di = HI2DI(hi);

	    --todo;
	    // Keep a class or enum: existing objects still refer to it.
	    જો (di->di_tv.v_type != VAR_CLASS
		    && STRNCMP(di->di_ચાવી, prefix, prefixlen) == 0)
		delete_var(&globvarht, hi);
	}
    hash_unlock(&globvarht);
}

/*
 * List the value of one internal variable.
 */
    સ્થિર શૂન્ય
list_one_var(dictitem_T *v, અક્ષર *prefix, પૂર્ણાંક *first)
{
    char_u	*tofree;
    char_u	*s;
    char_u	numbuf[NUMBUFLEN];

    s = echo_string(&v->di_tv, &tofree, numbuf, get_copyID());
    list_one_var_a(prefix, v->di_ચાવી, v->di_tv.v_type,
					 s == ખાલી ? (char_u *)"" : s, first);
    vim_free(tofree);
}

    સ્થિર શૂન્ય
list_one_var_a(
    અક્ષર	*prefix,
    char_u	*name,
    પૂર્ણાંક		type,
    char_u	*string,
    પૂર્ણાંક		*first)  // when TRUE clear rest of screen and set to FALSE
{
    // don't use msg() or msg_attr() to avoid overwriting "v:statusmsg"
    સંદેશ_start();
    સંદેશ_puts(prefix);
    જો (name != ખાલી)	// "a:" vars don't have a name stored
	સંદેશ_puts((અક્ષર *)name);
    સંદેશ_putchar(' ');
    સંદેશ_advance(22);
    જો (type == VAR_સંખ્યા)
	સંદેશ_putchar('#');
    નહીંતર જો (type == VAR_FUNC || type == VAR_PARTIAL)
	સંદેશ_putchar('*');
    નહીંતર જો (type == VAR_LIST)
    {
	સંદેશ_putchar('[');
	જો (*string == '[')
	    ++string;
    }
    નહીંતર જો (type == VAR_DICT)
    {
	સંદેશ_putchar('{');
	જો (*string == '{')
	    ++string;
    }
    નહીંતર
	સંદેશ_putchar(' ');

    સંદેશ_outtrans(string);

    જો (type == VAR_FUNC || type == VAR_PARTIAL)
	સંદેશ_puts("()");
    જો (*first)
    {
	સંદેશ_clr_eos();
	*first = FALSE;
    }
}

/*
 * Addition handling for setting a v: variable.
 * Return TRUE if the variable should be set normally,
 *        FALSE if nothing else needs to be done.
 */
    પૂર્ણાંક
before_set_vvar(
    char_u	*varname,
    dictitem_T	*di,
    typval_T	*tv,
    પૂર્ણાંક		copy,
    પૂર્ણાંક		*type_error)
{
    જો (di->di_tv.v_type == VAR_STRING)
    {
	VIM_CLEAR(di->di_tv.vval.v_string);
	જો (copy || tv->v_type != VAR_STRING)
	{
	    char_u *val = tv_get_string(tv);

	    // Careful: when assigning to v:errmsg and
	    // tv_get_string() causes an error message the variable
	    // will already be set.
	    જો (di->di_tv.vval.v_string == ખાલી)
		di->di_tv.vval.v_string = vim_strsave(val);
	}
	નહીંતર
	{
	    // Take over the string to avoid an extra alloc/free.
	    di->di_tv.vval.v_string = tv->vval.v_string;
	    tv->vval.v_string = ખાલી;
	}
	પાછા FALSE;
    }
    નહીંતર જો (di->di_tv.v_type == VAR_સંખ્યા)
    {
	di->di_tv.vval.v_સંખ્યા = tv_get_સંખ્યા(tv);
	જો (STRCMP(varname, "searchforward") == 0)
	    set_શોધ_direction(di->di_tv.vval.v_સંખ્યા ? '/' : '?');
#ifdef FEAT_શોધ_EXTRA
	નહીંતર જો (STRCMP(varname, "hlsearch") == 0)
	{
	    no_hlsearch = !di->di_tv.vval.v_સંખ્યા;
	    redraw_all_later(UPD_SOME_VALID);
	}
#endif
	પાછા FALSE;
    }
    નહીંતર જો (di->di_tv.v_type != tv->v_type)
    {
	*type_error = TRUE;
	પાછા FALSE;
    }
    પાછા TRUE;
}

/*
 * Set variable "name" to value in "tv".
 * If the variable already exists, the value is updated.
 * Otherwise the variable is created.
 */
    શૂન્ય
set_var(
    char_u	*name,
    typval_T	*tv,
    પૂર્ણાંક		copy)	    // make copy of value in "tv"
{
    set_var_const(name, 0, ખાલી, tv, copy, ASSIGN_DECL, 0);
}

/*
 * Set variable "name" to value in "tv_arg".
 * When "sid" is non-zero "name" is in the script with this ID.
 * If the variable already exists and "is_const" is FALSE the value is updated.
 * Otherwise the variable is created.
 */
    પૂર્ણાંક
set_var_const(
    char_u	*name,
    scid_T	sid,
    type_T	*type_arg,
    typval_T	*tv_arg,
    પૂર્ણાંક		copy,	    // make copy of value in "tv"
    પૂર્ણાંક		flags_arg,  // ASSIGN_CONST, ASSIGN_FINAL, etc.
    પૂર્ણાંક		var_idx)    // index for ":let [a, b] = list"
{
    typval_T	*tv = tv_arg;
    type_T	*type = type_arg;
    typval_T	bool_tv;
    dictitem_T	*di;
    typval_T	*dest_tv;
    char_u	*varname;
    char_u	*name_tofree = ખાલી;
    hashtab_T	*ht = ખાલી;
    પૂર્ણાંક		is_script_local;
    પૂર્ણાંક		vim9script = in_vim9script();
    પૂર્ણાંક		var_in_vim9script;
    પૂર્ણાંક		var_in_autoload = FALSE;
    પૂર્ણાંક		flags = flags_arg;
    પૂર્ણાંક		free_tv_arg = !copy;  // free tv_arg if not used
    પૂર્ણાંક		rc = FAIL;

    જો (sid != 0)
    {
	varname = ખાલી;
	જો (SCRIPT_ID_VALID(sid))
	{
	    char_u	*auto_name = ખાલી;
	    જો (find_var_autoload_prefix(name, sid, &ht, &auto_name) != ખાલી)
	    {
		var_in_autoload = TRUE;
		varname = auto_name;
		name_tofree = varname;
	    }
	    નહીંતર
		ht = &SCRIPT_VARS(sid);
	}
	જો (varname == ખાલી)
	    varname = name;
    }
    નહીંતર
    {
	scriptitem_T	*si;
	char_u		*auto_name = ખાલી;

	જો (in_vim9script()
	    && SCRIPT_ID_VALID(current_sctx.sc_sid)
	    && (si = SCRIPT_ITEM(current_sctx.sc_sid))
					      ->sn_autoload_prefix != ખાલી
	    && (is_export
		|| find_var_autoload_prefix(name, 0, ખાલી, &auto_name)
								    != ખાલી))
	{
	    // In a vim9 autoload script an exported variable is put in the
	    // global namespace with the autoload prefix.
	    var_in_autoload = TRUE;
	    varname = auto_name != ખાલી ? auto_name
		      : concat_str(si->sn_autoload_prefix, name);
	    જો (varname == ખાલી)
		જાઓ failed;
	    name_tofree = varname;
	    ht = &globvarht;
	}
	નહીંતર
	    ht = find_var_ht(name, &varname);
    }
    જો (ht == ખાલી || *varname == NUL)
    {
	semsg(_(e_illegal_variable_name_str), name);
	જાઓ failed;
    }
    is_script_local = ht == get_script_local_ht() || sid != 0
							    || var_in_autoload;

    જો (vim9script
	    && !is_script_local
	    && (flags & (ASSIGN_NO_DECL | ASSIGN_DECL)) == 0
	    && (flags & (ASSIGN_CONST | ASSIGN_FINAL)) == 0
	    && name[1] == ':')
    {
	vim9_declare_error(name);
	જાઓ failed;
    }
    જો ((flags & ASSIGN_FOR_LOOP) && is_scoped_variable(name))
	// Do not make g:var, w:var, b:var or t:var final.
	flags &= ~ASSIGN_FINAL;

    var_in_vim9script = is_script_local && current_script_is_vim9();
    જો (var_in_vim9script && name[0] == '_' && name[1] == NUL)
    {
	// For "[a, _] = list" the underscore is ignored.
	જો ((flags & ASSIGN_UNPACK) == 0)
	    emsg(_(e_cannot_use_underscore_here));
	જાઓ failed;
    }

    di = find_var_in_ht(ht, 0, varname, TRUE);

    જો (di == ખાલી && var_in_vim9script)
    {
	imported_T  *import = find_imported(varname, 0, FALSE);

	જો (import != ખાલી)
	{
	    // imported name space cannot be used
	    જો ((flags & ASSIGN_NO_DECL) == 0)
	    {
		semsg(_(e_redefining_imported_item_str), name);
		જાઓ failed;
	    }
	    semsg(_(e_cannot_use_str_itself_it_is_imported), name);
	    જાઓ failed;
	}
	જો (!in_vim9script())
	{
	    semsg(_(e_cannot_create_vim9_script_variable_in_function_str),
									 name);
	    જાઓ failed;
	}
    }

    // Search in parent scope which is possible to reference from lambda
    જો (di == ખાલી)
	di = find_var_in_scoped_ht(name, TRUE);

    જો ((tv->v_type == VAR_FUNC || tv->v_type == VAR_PARTIAL)
				  && var_wrong_func_name(name, di == ખાલી))
	જાઓ failed;

    જો (need_convert_to_bool(type, tv))
    {
	// Destination is a bool and the value is not, but it can be
	// converted.
	CLEAR_FIELD(bool_tv);
	bool_tv.v_type = VAR_BOOL;
	bool_tv.vval.v_સંખ્યા = tv2bool(tv) ? VVAL_TRUE : VVAL_FALSE;
	tv = &bool_tv;
    }

    જો (di != ખાલી)
    {
	// Item already exists.  Allowed to replace when reloading.
	જો ((di->di_flags & DI_FLAGS_RELOAD) == 0)
	{
	    જો ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					 && (flags & ASSIGN_FOR_LOOP) == 0)
	    {
		emsg(_(e_cannot_modify_existing_variable));
		જાઓ failed;
	    }

	    જો (is_script_local && vim9script
			  && (flags & (ASSIGN_NO_DECL | ASSIGN_DECL)) == 0)
	    {
		semsg(_(e_redefining_script_item_str), name);
		જાઓ failed;
	    }

	    જો (check_typval_is_કિંમત(&di->di_tv) == FAIL)
		જાઓ failed;

	    // List and Blob types can be modified in-place using the "+="
	    // compound operator.  For other types, this is not allowed.
	    પૂર્ણાંક type_inplace_modifiable =
		(di->di_tv.v_type == VAR_LIST || di->di_tv.v_type == VAR_BLOB);

	    જો (var_in_vim9script && (flags & ASSIGN_FOR_LOOP) == 0
		    && ((flags & ASSIGN_COMPOUND_OP) == 0
			|| !type_inplace_modifiable))
	    {
		where_T where = WHERE_INIT;
		svar_T  *sv = find_typval_in_script(&di->di_tv, sid, TRUE);

		જો (sv != ખાલી)
		{
		    // check the type and adjust to bool if needed
		    જો (var_idx > 0)
		    {
			where.wt_index = var_idx;
			where.wt_kind = WT_VARIABLE;
		    }
		    જો (check_script_var_type(sv, tv, name, where) == FAIL)
			જાઓ failed;
		    જો (type == ખાલી)
			type = sv->sv_type;
		    sv->sv_flags |= SVFLAG_ASSIGNED;
		}
	    }

	    // Modifying a final variable with a List value using the "+="
	    // operator is allowed.  For other types, it is not allowed.
	    જો ((((flags & ASSIGN_FOR_LOOP) == 0 || (flags & ASSIGN_DECL) == 0)
			&& ((flags & ASSIGN_COMPOUND_OP) == 0
			    || !type_inplace_modifiable))
				 ? var_check_permission(di, name) == FAIL
				 : var_check_ro(di->di_flags, name, FALSE))
		જાઓ failed;
	}
	નહીંતર
	{
	    // can only redefine once
	    di->di_flags &= ~DI_FLAGS_RELOAD;

	    // A Vim9 script-local variable is also present in sn_all_vars
	    // and sn_var_vals.  It may set "type" from "tv".
	    જો (var_in_vim9script || var_in_autoload)
		update_vim9_script_var(FALSE, di,
			var_in_autoload ? name : di->di_ચાવી, flags,
			tv, &type, (flags & ASSIGN_NO_MEMBER_TYPE) == 0);
	}

	// existing variable, need to clear the value

	// In Vim9 script a String v: variable only takes a String.
	જો (ht == &vimvarht && di->di_tv.v_type == VAR_STRING
							    && in_vim9script())
	{
	    where_T	where = WHERE_INIT;

	    જો (check_typval_type(&t_string, tv, where) == FAIL)
		જાઓ failed;
	}

	// Handle setting internal v: variables separately where needed to
	// prevent changing the type.
	પૂર્ણાંક type_error = FALSE;
	જો (ht == &vimvarht
		&& !before_set_vvar(varname, di, tv, copy, &type_error))
	{
	    જો (type_error)
		semsg(_(e_setting_v_str_to_કિંમત_with_wrong_type), varname);
	    જાઓ failed;
	}

	clear_tv(&di->di_tv);

	જો ((flags & ASSIGN_UPDATE_BLOCK_ID)
				       && SCRIPT_ID_VALID(current_sctx.sc_sid))
	{
	    scriptitem_T *si = SCRIPT_ITEM(current_sctx.sc_sid);

	    update_script_var_block_id(name, si->sn_current_block_id);
	}
    }
    નહીંતર
    {
	// Item not found, check if a function already exists.
	જો (is_script_local && (flags & (ASSIGN_NO_DECL | ASSIGN_DECL)) == 0
	       && lookup_scriptitem(name, STRLEN(name), FALSE, ખાલી) == OK)
	{
	    semsg(_(e_redefining_script_item_str), name);
	    જાઓ failed;
	}

	// add a new variable
	જો (var_in_vim9script && (flags & ASSIGN_NO_DECL))
	{
	    semsg(_(e_unknown_variable_str), name);
	    જાઓ failed;
	}

	જો (check_hashtab_frozen(ht, "add variable"))
	    જાઓ failed;

	// Can't add "v:" or "a:" variable.
	જો (ht == &vimvarht || ht == get_funccal_args_ht())
	{
	    semsg(_(e_illegal_variable_name_str), name);
	    જાઓ failed;
	}

	// Make sure the variable name is valid.  In Vim9 script an
	// autoload variable must be prefixed with "g:" unless in an
	// autoload script.
	જો (!valid_varname(varname, -1, !vim9script
			|| STRNCMP(name, "g:", 2) == 0 || var_in_autoload))
	    જાઓ failed;

	di = alloc(offsetof(dictitem_T, di_ચાવી) + STRLEN(varname) + 1);
	જો (di == ખાલી)
	    જાઓ failed;
	STRCPY(di->di_ચાવી, varname);
	જો (hash_add(ht, DI2HIKEY(di), "add variable") == FAIL)
	{
	    vim_free(di);
	    જાઓ failed;
	}
	di->di_flags = DI_FLAGS_ALLOC;
	જો (flags & (ASSIGN_CONST | ASSIGN_FINAL))
	    di->di_flags |= DI_FLAGS_LOCK;

	// A Vim9 script-local variable is also added to sn_all_vars and
	// sn_var_vals. It may set "type" from "tv".
	જો (var_in_vim9script || var_in_autoload)
	    update_vim9_script_var(TRUE, di,
		    var_in_autoload ? name : di->di_ચાવી, flags,
			  tv, &type, (flags & ASSIGN_NO_MEMBER_TYPE) == 0);
    }

    dest_tv = &di->di_tv;
    જો (copy || tv->v_type == VAR_સંખ્યા || tv->v_type == VAR_FLOAT)
	copy_tv(tv, dest_tv);
    નહીંતર
    {
	*dest_tv = *tv;
	dest_tv->v_lock = 0;
	init_tv(tv);
    }
    free_tv_arg = FALSE;

    જો (vim9script && type != ખાલી)
	set_tv_type(dest_tv, type);

    // ":const var = value" locks the value
    // ":final var = value" locks "var"
    જો (flags & ASSIGN_CONST)
	// Like :lockvar! name: lock the value and what it contains, but only
	// if the reference count is up to one.  That locks only literal
	// values.
	item_lock(dest_tv, DICT_MAXNEST, TRUE, TRUE);

    rc = OK;

failed:
    vim_free(name_tofree);
    જો (free_tv_arg)
	clear_tv(tv_arg);

    પાછા rc;
}

/*
 * Check in this order for backwards compatibility:
 * - Whether the variable is read-only
 * - Whether the variable value is locked
 * - Whether the variable is locked
 * NOTE: "name" is only used for error messages.
 */
    પૂર્ણાંક
var_check_permission(dictitem_T *di, char_u *name)
{
    જો (var_check_ro(di->di_flags, name, FALSE)
		    || કિંમત_check_lock(di->di_tv.v_lock, name, FALSE)
		    || var_check_lock(di->di_flags, name, FALSE))
	પાછા FAIL;
    પાછા OK;
}

/*
 * Return TRUE if di_flags "flags" indicates variable "name" is read-only.
 * Also give an error message.
 */
    પૂર્ણાંક
var_check_ro(પૂર્ણાંક flags, char_u *name, પૂર્ણાંક use_gettext)
{
    જો (flags & DI_FLAGS_RO)
    {
	જો (name == ખાલી)
	    emsg(_(e_cannot_change_readonly_variable));
	નહીંતર
	    semsg(_(e_cannot_change_readonly_variable_str),
				       use_gettext ? (char_u *)_(name) : name);
	પાછા TRUE;
    }
    જો ((flags & DI_FLAGS_RO_SBX) && sandbox)
    {
	જો (name == ખાલી)
	    emsg(_(e_cannot_set_variable_in_sandbox));
	નહીંતર
	    semsg(_(e_cannot_set_variable_in_sandbox_str),
				       use_gettext ? (char_u *)_(name) : name);
	પાછા TRUE;
    }
    પાછા FALSE;
}

/*
 * Return TRUE if di_flags "flags" indicates variable "name" is locked.
 * Also give an error message.
 */
    પૂર્ણાંક
var_check_lock(પૂર્ણાંક flags, char_u *name, પૂર્ણાંક use_gettext)
{
    જો (flags & DI_FLAGS_LOCK)
    {
	semsg(_(e_variable_is_locked_str),
				       use_gettext ? (char_u *)_(name) : name);
	પાછા TRUE;
    }
    પાછા FALSE;
}

/*
 * Return TRUE if di_flags "flags" indicates variable "name" is fixed.
 * Also give an error message.
 */
    પૂર્ણાંક
var_check_fixed(પૂર્ણાંક flags, char_u *name, પૂર્ણાંક use_gettext)
{
    જો (flags & DI_FLAGS_FIX)
    {
	જો (name == ખાલી)
	    emsg(_(e_cannot_delete_variable));
	નહીંતર
	    semsg(_(e_cannot_delete_variable_str),
				      use_gettext ? (char_u *)_(name) : name);
	પાછા TRUE;
    }
    પાછા FALSE;
}

/*
 * Check if a funcref is assigned to a valid variable name.
 * Return TRUE and give an error if not.
 */
    પૂર્ણાંક
var_wrong_func_name(
    char_u *name,    // points to start of variable name
    પૂર્ણાંક    new_var)  // TRUE when creating the variable
{
    // Allow for w: b: s: and t:.  In Vim9 script s: is not allowed, because
    // the name can be used without the s: prefix.
    // Allow autoload variable.
    જો (!((vim_strchr((char_u *)"wbt", name[0]) != ખાલી
		    || (!in_vim9script() && name[0] == 's')) && name[1] == ':')
	    && !ASCII_ISUPPER((name[0] != NUL && name[1] == ':')
						     ? name[2] : name[0])
	    && vim_strchr(name, '#') == ખાલી)
    {
	semsg(_(e_funcref_variable_name_must_start_with_capital_str), name);
	પાછા TRUE;
    }
    // Don't allow hiding a function.  When "v" is not NULL we might be
    // assigning another function to the same var, the type is checked
    // below.
    જો (new_var && function_exists(name, FALSE))
    {
	semsg(_(e_variable_name_conflicts_with_existing_function_str),
								    name);
	પાછા TRUE;
    }
    પાછા FALSE;
}

/*
 * Return TRUE if "flags" indicates variable "name" has a locked (immutable)
 * value.  Also give an error message, using "name" or _("name") when
 * "use_gettext" is TRUE.
 */
    પૂર્ણાંક
કિંમત_check_lock(પૂર્ણાંક lock, char_u *name, પૂર્ણાંક use_gettext)
{
    જો (lock & VAR_LOCKED)
    {
	જો (name == ખાલી)
	    emsg(_(e_કિંમત_is_locked));
	નહીંતર
	    semsg(_(e_કિંમત_is_locked_str),
				       use_gettext ? (char_u *)_(name) : name);
	પાછા TRUE;
    }
    જો (lock & VAR_FIXED)
    {
	જો (name == ખાલી)
	    emsg(_(e_cannot_change_કિંમત));
	નહીંતર
	    semsg(_(e_cannot_change_કિંમત_of_str),
				       use_gettext ? (char_u *)_(name) : name);
	પાછા TRUE;
    }
    પાછા FALSE;
}

/*
 * Check if a variable name is valid.  When "autoload" is true "#" is allowed.
 * If "len" is -1 use all of "varname", otherwise up to "varname[len]".
 * Return FALSE and give an error if not.
 */
    પૂર્ણાંક
valid_varname(char_u *varname, પૂર્ણાંક len, પૂર્ણાંક autoload)
{
    char_u *p;

    માટે (p = varname; len < 0 ? *p != NUL : p < varname + len; ++p)
	જો (!eval_isnamec1(*p) && (p == varname || !VIM_ISDIGIT(*p))
					 && !(autoload && *p == AUTOLOAD_CHAR))
	{
	    semsg(_(e_illegal_variable_name_str), varname);
	    પાછા FALSE;
	}
    પાછા TRUE;
}

/*
 * Implements the logic to retrieve local variable and option values.
 * Used by "getwinvar()" "gettabvar()" "gettabwinvar()" "getbufvar()".
 */
    સ્થિર શૂન્ય
get_var_from(
    char_u	*varname,
    typval_T	*rettv,
    typval_T	*deftv,	    // Default value if not found.
    પૂર્ણાંક		htname,	    // 't'ab, 'w'indow or 'b'uffer local.
    tabpage_T	*tp,	    // can be NULL
    win_T	*win,
    buf_T	*buf)	    // Ignored if htname is not 'b'.
{
    dictitem_T	*v;
    પૂર્ણાંક		done = FALSE;
    switchwin_T	switchwin;
    પૂર્ણાંક		need_switch_win;
    પૂર્ણાંક		do_change_curbuf = buf != ખાલી && htname == 'b';

    ++emsg_off;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ખાલી;

    જો (varname != ખાલી && tp != ખાલી && win != ખાલી
	    && (htname != 'b' || buf != ખાલી))
    {
	// Set curwin to be our win, temporarily.  Also set the tabpage,
	// otherwise the window is not valid. Only do this when needed,
	// autocommands get blocked.
	// If we have a buffer reference avoid the switching, we're saving and
	// restoring curbuf directly.
	need_switch_win = !(tp == curtab && win == curwin) && !do_change_curbuf;
	જો (!need_switch_win || switch_win(&switchwin, win, tp, TRUE) == OK)
	{
	    // Handle options. There are no tab-local options.
	    જો (*varname == '&' && htname != 't')
	    {
		buf_T	*save_curbuf = curbuf;

		// Change curbuf so the option is read from the correct buffer.
		જો (do_change_curbuf)
		    curbuf = buf;

		જો (varname[1] == NUL)
		{
		    // get all window-local or buffer-local options in a dict
		    dict_T	*opts = get_winbuf_options(htname == 'b');

		    જો (opts != ખાલી)
		    {
			rettv_dict_set(rettv, opts);
			done = TRUE;
		    }
		}
		નહીંતર જો (eval_option(&varname, rettv, TRUE) == OK)
		    // Local option
		    done = TRUE;

		curbuf = save_curbuf;
	    }
	    નહીંતર જો (*varname == NUL)
	    {
		// Empty string: return a dict with all the local variables.
		જો (htname == 'b')
		    v = &buf->b_bufvar;
		નહીંતર જો (htname == 'w')
		    v = &win->w_winvar;
		નહીંતર
		    v = &tp->tp_winvar;
		copy_tv(&v->di_tv, rettv);
		done = TRUE;
	    }
	    નહીંતર
	    {
		hashtab_T	*ht;

		જો (htname == 'b')
		    ht = &buf->b_vars->dv_hashtab;
		નહીંતર જો (htname == 'w')
		    ht = &win->w_vars->dv_hashtab;
		નહીંતર
		    ht = &tp->tp_vars->dv_hashtab;

		// Look up the variable.
		v = find_var_in_ht(ht, htname, varname, FALSE);
		જો (v != ખાલી)
		{
		    copy_tv(&v->di_tv, rettv);
		    done = TRUE;
		}
	    }
	}

	જો (need_switch_win)
	    // restore previous notion of curwin
	    restore_win(&switchwin, TRUE);
    }

    જો (!done && deftv->v_type != VAR_UNKNOWN)
	// use the default value
	copy_tv(deftv, rettv);

    --emsg_off;
}

/*
 * getwinvar() and gettabwinvar()
 */
    સ્થિર શૂન્ય
getwinvar(
    typval_T	*argvars,
    typval_T	*rettv,
    પૂર્ણાંક		off)	    // 1 for gettabwinvar()
{
    char_u	*varname;
    tabpage_T	*tp;
    win_T	*win;

    જો (off == 1)
	tp = find_tabpage((પૂર્ણાંક)tv_get_સંખ્યા_chk(&argvars[0], ખાલી));
    નહીંતર
	tp = curtab;
    win = find_win_by_nr(&argvars[off], tp);
    varname = tv_get_string_chk(&argvars[off + 1]);

    get_var_from(varname, rettv, &argvars[off + 2], 'w', tp, win, ખાલી);
}

/*
 * Set option "varname" to the value of "varp" for the current buffer/window.
 */
    સ્થિર શૂન્ય
set_option_from_tv(char_u *varname, typval_T *varp)
{
    long	numval = 0;
    char_u	*strval;
    char_u	nbuf[NUMBUFLEN];
    પૂર્ણાંક		error = FALSE;

    પૂર્ણાંક		opt_idx = findoption(varname);
    જો (opt_idx < 0)
    {
	semsg(_(e_unknown_option_str_2), varname);
	પાછા;
    }
    પૂર્ણાંક		opt_p_flags = get_option_flags(opt_idx);

    જો (varp->v_type == VAR_BOOL)
    {
	જો (opt_p_flags & P_STRING)
	{
	    emsg(_(e_string_required));
	    પાછા;
	}
	numval = (long)varp->vval.v_સંખ્યા;
	strval = (char_u *)"0";  // avoid using "false"
    }
    નહીંતર
    {
	જો ((opt_p_flags & (P_અંક|P_BOOL))
		&& (!in_vim9script() || varp->v_type != VAR_STRING))
	    numval = (long)tv_get_સંખ્યા_chk(varp, &error);
	જો (!error)
	    strval = tv_get_string_buf_chk(varp, nbuf);
    }
    જો (!error && strval != ખાલી)
	set_option_કિંમત_give_err(varname, numval, strval, OPT_LOCAL);
}

/*
 * "setwinvar()" and "settabwinvar()" functions
 */
    સ્થિર શૂન્ય
setwinvar(typval_T *argvars, પૂર્ણાંક off)
{
    win_T	*win;
    switchwin_T	switchwin;
    પૂર્ણાંક		need_switch_win;
    char_u	*varname, *winvarname;
    typval_T	*varp;
    tabpage_T	*tp = ખાલી;

    જો (check_secure())
	પાછા;

    જો (off == 1)
	tp = find_tabpage((પૂર્ણાંક)tv_get_સંખ્યા_chk(&argvars[0], ખાલી));
    નહીંતર
	tp = curtab;
    win = find_win_by_nr(&argvars[off], tp);
    varname = tv_get_string_chk(&argvars[off + 1]);
    varp = &argvars[off + 2];

    જો (win == ખાલી || varname == ખાલી)
	પાછા;

    need_switch_win = !(tp == curtab && win == curwin);
    જો (!need_switch_win
	    || switch_win(&switchwin, win, tp, TRUE) == OK)
    {
	જો (*varname == '&')
	    set_option_from_tv(varname + 1, varp);
	નહીંતર
	{
	    winvarname = alloc(STRLEN(varname) + 3);
	    જો (winvarname != ખાલી)
	    {
		STRCPY(winvarname, "w:");
		STRCPY(winvarname + 2, varname);
		set_var(winvarname, varp, TRUE);
		vim_free(winvarname);
	    }
	}
    }
    જો (need_switch_win)
	restore_win(&switchwin, TRUE);
}

/*
 * reset v:option_new, v:option_old, v:option_oldlocal, v:option_oldglobal,
 * v:option_type, and v:option_command.
 */
    શૂન્ય
reset_v_option_vars(શૂન્ય)
{
    set_vim_var_string(VV_OPTION_NEW, ખાલી, -1);
    set_vim_var_string(VV_OPTION_OLD, ખાલી, -1);
    set_vim_var_string(VV_OPTION_OLDLOCAL, ખાલી, -1);
    set_vim_var_string(VV_OPTION_OLDGLOBAL, ખાલી, -1);
    set_vim_var_string(VV_OPTION_TYPE, ખાલી, -1);
    set_vim_var_string(VV_OPTION_COMMAND, ખાલી, -1);
}

/*
 * Add an assert error to v:errors.
 */
    શૂન્ય
assert_error(garray_T *gap)
{
    સંરચના vimvar   *vp = &vimvars[VV_ERRORS];

    જો (vp->vv_tv_type != VAR_LIST || vimvars[VV_ERRORS].vv_list == ખાલી)
	// Make sure v:errors is a list.
	set_vim_var_list(VV_ERRORS, list_alloc());
    list_append_string(vimvars[VV_ERRORS].vv_list, gap->ga_data, gap->ga_len);
}

    પૂર્ણાંક
var_exists(char_u *var)
{
    char_u	*arg = var;
    char_u	*name;
    char_u	*tofree;
    typval_T    tv;
    પૂર્ણાંક		len = 0;
    પૂર્ણાંક		n = FALSE;

    // get_name_len() takes care of expanding curly braces
    name = var;
    len = get_name_len(&arg, &tofree, TRUE, FALSE);
    જો (len > 0)
    {
	જો (tofree != ખાલી)
	    name = tofree;
	n = (eval_variable(name, len, 0, &tv, ખાલી,
				 EVAL_VAR_NOAUTOLOAD + EVAL_VAR_IMPORT) == OK);
	જો (n)
	{
	    // handle d.key, l[idx], f(expr)
	    arg = skipwhite(arg);
	    n = (handle_subscript(&arg, name, &tv, &EVALARG_EVALUATE,
								 FALSE) == OK);
	    જો (n)
		clear_tv(&tv);
	}
    }
    જો (*arg != NUL)
	n = FALSE;

    vim_free(tofree);
    પાછા n;
}

સ્થિર lval_T	*redir_lval = ખાલી;
#define EVALCMD_BUSY (redir_lval == (lval_T *)&redir_lval)
સ્થિર garray_T redir_ga;	// only valid when redir_lval is not NULL
સ્થિર char_u	*redir_endp = ખાલી;
સ્થિર char_u	*redir_varname = ખાલી;

    પૂર્ણાંક
alloc_redir_lval(શૂન્ય)
{
    redir_lval = ALLOC_CLEAR_ONE(lval_T);
    જો (redir_lval == ખાલી)
	પાછા FAIL;
    પાછા OK;
}

    શૂન્ય
clear_redir_lval(શૂન્ય)
{
    VIM_CLEAR(redir_lval);
}

    શૂન્ય
init_redir_ga(શૂન્ય)
{
    ga_init2(&redir_ga, કદ(અક્ષર), 500);
}

/*
 * Start recording command output to a variable
 * When "append" is TRUE append to an existing variable.
 * Returns OK if successfully completed the setup.  FAIL otherwise.
 */
    પૂર્ણાંક
var_redir_start(char_u *name, પૂર્ણાંક append)
{
    પૂર્ણાંક		called_emsg_before;
    typval_T	tv;

    // Catch a bad name early.
    જો (!eval_isnamec1(*name))
    {
	emsg(_(e_invalid_argument));
	પાછા FAIL;
    }

    // Make a copy of the name, it is used in redir_lval until redir ends.
    redir_varname = vim_strsave(name);
    જો (redir_varname == ખાલી)
	પાછા FAIL;

    જો (alloc_redir_lval() == FAIL)
    {
	var_redir_stop();
	પાછા FAIL;
    }

    // The output is stored in growarray "redir_ga" until redirection ends.
    init_redir_ga();

    // Parse the variable name (can be a dict or list entry).
    redir_endp = get_lval(redir_varname, ખાલી, redir_lval, FALSE, FALSE, 0,
							     FNE_CHECK_START);
    જો (redir_endp == ખાલી || redir_lval->ll_name == ખાલી || *redir_endp != NUL)
    {
	clear_lval(redir_lval);
	જો (redir_endp != ખાલી && *redir_endp != NUL)
	    // Trailing characters are present after the variable name
	    semsg(_(e_trailing_characters_str), redir_endp);
	નહીંતર
	    semsg(_(e_invalid_argument_str), name);
	redir_endp = ખાલી;  // don't store a value, only cleanup
	var_redir_stop();
	પાછા FAIL;
    }

    // check if we can write to the variable: set it to or append an empty
    // string
    called_emsg_before = called_emsg;
    tv.v_type = VAR_STRING;
    tv.vval.v_string = (char_u *)"";
    જો (append)
	set_var_lval(redir_lval, redir_endp, &tv, TRUE,
					     ASSIGN_NO_DECL, (char_u *)".", 0);
    નહીંતર
	set_var_lval(redir_lval, redir_endp, &tv, TRUE,
					     ASSIGN_NO_DECL, (char_u *)"=", 0);
    clear_lval(redir_lval);
    જો (called_emsg > called_emsg_before)
    {
	redir_endp = ખાલી;  // don't store a value, only cleanup
	var_redir_stop();
	પાછા FAIL;
    }

    પાછા OK;
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
    શૂન્ય
var_redir_str(char_u *કિંમત, પૂર્ણાંક કિંમત_len)
{
    પૂર્ણાંક		len;

    જો (redir_lval == ખાલી)
	પાછા;

    જો (કિંમત_len == -1)
	len = (પૂર્ણાંક)STRLEN(કિંમત);	// Append the entire string
    નહીંતર
	len = કિંમત_len;		// Append only "value_len" characters

    જો (ga_grow(&redir_ga, len) == OK)
    {
	mch_memmove((અક્ષર *)redir_ga.ga_data + redir_ga.ga_len, કિંમત, len);
	redir_ga.ga_len += len;
    }
    નહીંતર
	var_redir_stop();
}

/*
 * Stop redirecting command output to a variable.
 * Frees the allocated memory.
 */
    શૂન્ય
var_redir_stop(શૂન્ય)
{
    typval_T	tv;

    જો (EVALCMD_BUSY)
    {
	redir_lval = ખાલી;
	પાછા;
    }

    જો (redir_lval != ખાલી)
    {
	// If there was no error: assign the text to the variable.
	જો (redir_endp != ખાલી)
	{
	    ga_append(&redir_ga, NUL);  // Append the trailing NUL.
	    tv.v_type = VAR_STRING;
	    tv.vval.v_string = redir_ga.ga_data;
	    // Call get_lval() again, if it's inside a Dict or List it may
	    // have changed.
	    redir_endp = get_lval(redir_varname, ખાલી, redir_lval,
					FALSE, FALSE, 0, FNE_CHECK_START);
	    જો (redir_endp != ખાલી && redir_lval->ll_name != ખાલી)
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
get_clear_redir_ga(શૂન્ય)
{
    char_u *res;

    ga_append(&redir_ga, NUL);  // Append the trailing NUL.
    res = redir_ga.ga_data;
    redir_ga.ga_data = ખાલી;
    પાછા res;
}

/*
 * "gettabvar()" function
 */
    શૂન્ય
f_gettabvar(typval_T *argvars, typval_T *rettv)
{
    char_u	*varname;
    tabpage_T	*tp;
    win_T	*win = ખાલી;

    જો (in_vim9script()
	    && (check_for_સંખ્યા_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	પાછા;

    varname = tv_get_string_chk(&argvars[1]);
    tp = find_tabpage((પૂર્ણાંક)tv_get_સંખ્યા_chk(&argvars[0], ખાલી));
    જો (tp != ખાલી)
	win = tp == curtab || tp->tp_firstwin == ખાલી ? firstwin
		: tp->tp_firstwin;

    get_var_from(varname, rettv, &argvars[2], 't', tp, win, ખાલી);
}

/*
 * "gettabwinvar()" function
 */
    શૂન્ય
f_gettabwinvar(typval_T *argvars, typval_T *rettv)
{
    જો (in_vim9script()
	    && (check_for_સંખ્યા_arg(argvars, 0) == FAIL
		|| check_for_સંખ્યા_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL))
	પાછા;

    getwinvar(argvars, rettv, 1);
}

/*
 * "getwinvar()" function
 */
    શૂન્ય
f_getwinvar(typval_T *argvars, typval_T *rettv)
{
    જો (in_vim9script()
	    && (check_for_સંખ્યા_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	પાછા;

    getwinvar(argvars, rettv, 0);
}

/*
 * "getbufvar()" function
 */
    શૂન્ય
f_getbufvar(typval_T *argvars, typval_T *rettv)
{
    char_u	*varname;
    buf_T	*buf;

    જો (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	પાછા;

    varname = tv_get_string_chk(&argvars[1]);
    buf = tv_get_buf_from_arg(&argvars[0]);

    get_var_from(varname, rettv, &argvars[2], 'b', curtab, curwin, buf);
}

/*
 * "settabvar()" function
 */
    શૂન્ય
f_settabvar(typval_T *argvars, typval_T *rettv UNUSED)
{
    tabpage_T	*save_curtab;
    tabpage_T	*tp;
    tabpage_T	*save_lu_tp;
    char_u	*varname, *tabvarname;
    typval_T	*varp;

    જો (check_secure())
	પાછા;

    જો (in_vim9script()
	    && (check_for_સંખ્યા_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	પાછા;

    tp = find_tabpage((પૂર્ણાંક)tv_get_સંખ્યા_chk(&argvars[0], ખાલી));
    varname = tv_get_string_chk(&argvars[1]);
    varp = &argvars[2];

    જો (varname == ખાલી || tp == ખાલી)
	પાછા;

    save_curtab = curtab;
    save_lu_tp = lastused_tabpage;
    goto_tabpage_tp(tp, FALSE, FALSE);

    tabvarname = alloc(STRLEN(varname) + 3);
    જો (tabvarname != ખાલી)
    {
	STRCPY(tabvarname, "t:");
	STRCPY(tabvarname + 2, varname);
	set_var(tabvarname, varp, TRUE);
	vim_free(tabvarname);
    }

    // Restore current tabpage and last accessed tabpage.
    જો (valid_tabpage(save_curtab))
    {
	goto_tabpage_tp(save_curtab, FALSE, FALSE);
	જો (valid_tabpage(save_lu_tp))
	    lastused_tabpage = save_lu_tp;
    }
}

/*
 * "settabwinvar()" function
 */
    શૂન્ય
f_settabwinvar(typval_T *argvars, typval_T *rettv UNUSED)
{
    જો (in_vim9script()
	    && (check_for_સંખ્યા_arg(argvars, 0) == FAIL
		|| check_for_સંખ્યા_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL))
	પાછા;

    setwinvar(argvars, 1);
}

/*
 * "setwinvar()" function
 */
    શૂન્ય
f_setwinvar(typval_T *argvars, typval_T *rettv UNUSED)
{
    જો (in_vim9script()
	    && (check_for_સંખ્યા_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	પાછા;

    setwinvar(argvars, 0);
}

/*
 * "setbufvar()" function
 */
    શૂન્ય
f_setbufvar(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    char_u	*varname, *bufvarname;
    typval_T	*varp;

    જો (check_secure())
	પાછા;

    જો (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	પાછા;

    varname = tv_get_string_chk(&argvars[1]);
    buf = tv_get_buf_from_arg(&argvars[0]);
    varp = &argvars[2];

    જો (buf == ખાલી || varname == ખાલી)
	પાછા;

    જો (*varname == '&')
    {
	aco_save_T	aco;
	// safe the current window position, it could
	// change because of 'scrollbind' window-local
	// options
	linenr_T	old_topline = curwin->w_topline;

	// Set curbuf to be our buf, temporarily.
	aucmd_prepbuf(&aco, buf);
	જો (curbuf == buf)
	{
	    // Only when it worked to set "curbuf".
	    set_option_from_tv(varname + 1, varp);

	    // reset notion of buffer
	    aucmd_restbuf(&aco);
	}
	curwin->w_topline = old_topline;
    }
    નહીંતર
    {
	bufvarname = alloc(STRLEN(varname) + 3);
	જો (bufvarname != ખાલી)
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
    પૂર્ણાંક		r = OK;

    CLEAR_FIELD(res);
    જો (arg->v_type == VAR_PARTIAL && arg->vval.v_partial != ખાલી)
    {
	res.cb_partial = arg->vval.v_partial;
	++res.cb_partial->pt_refcount;
	res.cb_name = partial_name(res.cb_partial);
    }
    નહીંતર
    {
	જો (arg->v_type == VAR_STRING && arg->vval.v_string != ખાલી
					       && SAFE_isdigit(*arg->vval.v_string))
	    r = FAIL;
	નહીંતર જો (arg->v_type == VAR_FUNC || arg->v_type == VAR_STRING)
	{
	    res.cb_name = arg->vval.v_string;
	    જો (arg->v_type == VAR_STRING)
	    {
		char_u *name = get_scriptlocal_funcname(arg->vval.v_string);
		જો (name != ખાલી)
		{
		    res.cb_name = name;
		    res.cb_free_name = TRUE;
		}
	    }
	    func_ref(res.cb_name);
	}
	નહીંતર જો (arg->v_type == VAR_સંખ્યા && arg->vval.v_સંખ્યા == 0)
	    res.cb_name = (char_u *)"";
	નહીંતર
	    r = FAIL;

	જો (r == FAIL)
	{
	    emsg(_(e_invalid_callback_argument));
	    res.cb_name = ખાલી;
	}
    }
    પાછા res;
}

/*
 * Copy a callback into a typval_T.
 */
    શૂન્ય
put_callback(callback_T *cb, typval_T *tv)
{
    જો (cb->cb_partial != ખાલી)
    {
	tv->v_type = VAR_PARTIAL;
	tv->vval.v_partial = cb->cb_partial;
	++tv->vval.v_partial->pt_refcount;
    }
    નહીંતર
    {
	tv->v_type = VAR_FUNC;
	tv->vval.v_string = vim_strsave(cb->cb_name);
	func_ref(cb->cb_name);
    }
}

    સ્થિર bool
does_callback_own_cb_name(callback_T *cb)
{
    // If cb_partial != NULL then *cb->cb_name is owned by the partial.
    પાછા cb->cb_partial || cb->cb_free_name;
}

/*
 * Make a copy of "src" into "dest", allocating the function name if needed,
 * without incrementing the refcount.
 */
    શૂન્ય
set_callback(callback_T *dest, callback_T *src)
{
    *dest = *src;
    જો (!does_callback_own_cb_name(src))
    {
	dest->cb_name = vim_strsave(src->cb_name);
	dest->cb_free_name = TRUE;
    }
    *src = (callback_T){0};
}

/*
 * Copy callback from "src" to "dest", incrementing the refcounts.
 */
    શૂન્ય
copy_callback(callback_T *dest, callback_T *src)
{
    dest->cb_partial = src->cb_partial;
    જો (dest->cb_partial != ખાલી)
    {
	dest->cb_name = src->cb_name;
	dest->cb_free_name = FALSE;
	++dest->cb_partial->pt_refcount;
    }
    નહીંતર
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
    શૂન્ય
expand_autload_callback(callback_T *cb)
{
    char_u	*name;
    char_u	*p;
    imported_T	*import;

    જો (!in_vim9script() || cb->cb_name == ખાલી
	    || (!cb->cb_free_name
	       && (cb->cb_partial == ખાલી || cb->cb_partial->pt_name == ખાલી)))
	પાછા;
    જો (cb->cb_partial != ખાલી)
	name = cb->cb_partial->pt_name;
    નહીંતર
	name = cb->cb_name;
    p = vim_strchr(name, '.');
    જો (p == ખાલી)
	પાછા;

    import = find_imported(name, p - name, FALSE);
    જો (import == ખાલી || !SCRIPT_ID_VALID(import->imp_sid))
	પાછા;

    scriptitem_T *si = SCRIPT_ITEM(import->imp_sid);
    જો (si->sn_autoload_prefix == ખાલી)
	પાછા;

    char_u *newname = concat_str(si->sn_autoload_prefix, p + 1);
    જો (newname == ખાલી)
	પાછા;

    જો (cb->cb_partial != ખાલી)
    {
	જો (cb->cb_name == cb->cb_partial->pt_name)
	    cb->cb_name = newname;
	vim_free(cb->cb_partial->pt_name);
	cb->cb_partial->pt_name = newname;
    }
    નહીંતર
    {
	vim_free(cb->cb_name);
	cb->cb_name = newname;
    }
}

/*
 * Unref/free "callback" returned by get_callback() or set_callback().
 */
    શૂન્ય
free_callback(callback_T *callback)
{
    જો (callback->cb_partial != ખાલી)
    {
	partial_unref(callback->cb_partial);
	callback->cb_partial = ખાલી;
    }
    નહીંતર જો (callback->cb_name != ખાલી)
	func_unref(callback->cb_name);
    જો (callback->cb_free_name)
    {
	vim_free(callback->cb_name);
	callback->cb_free_name = FALSE;
    }
    callback->cb_name = ખાલી;
}

#endif // FEAT_EVAL
