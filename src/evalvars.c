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

#അന്തര്ഭാവ "vim.h"

#എങ്കിൽ defined(FEAT_EVAL)

സ്ഥിതം dictitem_T	globvars_var;		// variable used for g:
സ്ഥിതം dict_T		globvardict;		// Dictionary with g: variables
#define globvarht globvardict.dv_hashtab

/*
 * Old Vim variables such as "v:version" are also available without the "v:".
 * Also in functions.  We need a special hashtable for them.
 */
സ്ഥിതം hashtab_T	compat_hashtab;

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

തരനിർവചനം ഘടന vimvar vimvar_T;

സ്ഥിതം ഘടന vimvar
{
    അക്ഷരം	*vv_name;	// name of variable, without v:
    dictitem16_T vv_di;		// value and name for key (max 16 chars!)
    type_T	*vv_type;	// type or NULL
    അക്ഷരം	vv_flags;	// VV_COMPAT, VV_RO, VV_RO_SBX
} vimvars[VV_LEN] =
{
    // The order here must match the VV_ defines in vim.h!
    // Initializing a union does not work, leave tv.vval empty to get zero's.
    {VV_NAME("count",		 VAR_സംഖ്യ), ശൂന്യ, VV_COMPAT+VV_RO},
    {VV_NAME("count1",		 VAR_സംഖ്യ), ശൂന്യ, VV_RO},
    {VV_NAME("prevcount",	 VAR_സംഖ്യ), ശൂന്യ, VV_RO},
    {VV_NAME("errmsg",		 VAR_STRING), ശൂന്യ, VV_COMPAT},
    {VV_NAME("warningmsg",	 VAR_STRING), ശൂന്യ, 0},
    {VV_NAME("statusmsg",	 VAR_STRING), ശൂന്യ, 0},
    {VV_NAME("shell_error",	 VAR_സംഖ്യ), ശൂന്യ, VV_COMPAT+VV_RO},
    {VV_NAME("this_session",	 VAR_STRING), ശൂന്യ, VV_COMPAT},
    {VV_NAME("version",		 VAR_സംഖ്യ), ശൂന്യ, VV_COMPAT+VV_RO},
    {VV_NAME("lnum",		 VAR_സംഖ്യ), ശൂന്യ, VV_RO_SBX},
    {VV_NAME("termresponse",	 VAR_STRING), ശൂന്യ, VV_RO},
    {VV_NAME("fname",		 VAR_STRING), ശൂന്യ, VV_RO},
    {VV_NAME("lang",		 VAR_STRING), ശൂന്യ, VV_RO},
    {VV_NAME("lc_time",		 VAR_STRING), ശൂന്യ, VV_RO},
    {VV_NAME("ctype",		 VAR_STRING), ശൂന്യ, VV_RO},
    {VV_NAME("charconvert_from", VAR_STRING), ശൂന്യ, VV_RO},
    {VV_NAME("charconvert_to",	 VAR_STRING), ശൂന്യ, VV_RO},
    {VV_NAME("fname_in",	 VAR_STRING), ശൂന്യ, VV_RO},
    {VV_NAME("fname_out",	 VAR_STRING), ശൂന്യ, VV_RO},
    {VV_NAME("fname_new",	 VAR_STRING), ശൂന്യ, VV_RO},
    {VV_NAME("fname_diff",	 VAR_STRING), ശൂന്യ, VV_RO},
    {VV_NAME("cmdarg",		 VAR_STRING), ശൂന്യ, VV_RO},
    {VV_NAME("foldstart",	 VAR_സംഖ്യ), ശൂന്യ, VV_RO_SBX},
    {VV_NAME("foldend",		 VAR_സംഖ്യ), ശൂന്യ, VV_RO_SBX},
    {VV_NAME("folddashes",	 VAR_STRING), ശൂന്യ, VV_RO_SBX},
    {VV_NAME("foldlevel",	 VAR_സംഖ്യ), ശൂന്യ, VV_RO_SBX},
    {VV_NAME("progname",	 VAR_STRING), ശൂന്യ, VV_RO},
    {VV_NAME("servername",	 VAR_STRING), ശൂന്യ, VV_RO},
    {VV_NAME("dying",		 VAR_സംഖ്യ), ശൂന്യ, VV_RO},
    {VV_NAME("exception",	 VAR_STRING), ശൂന്യ, VV_RO},
    {VV_NAME("throwpoint",	 VAR_STRING), ശൂന്യ, VV_RO},
    {VV_NAME("register",	 VAR_STRING), ശൂന്യ, VV_RO},
    {VV_NAME("cmdbang",		 VAR_സംഖ്യ), ശൂന്യ, VV_RO},
    {VV_NAME("insertmode",	 VAR_STRING), ശൂന്യ, VV_RO},
    {VV_NAME("val",		 VAR_UNKNOWN), ശൂന്യ, VV_RO},
    {VV_NAME("key",		 VAR_UNKNOWN), ശൂന്യ, VV_RO},
    {VV_NAME("profiling",	 VAR_സംഖ്യ), ശൂന്യ, VV_RO},
    {VV_NAME("fcs_reason",	 VAR_STRING), ശൂന്യ, VV_RO},
    {VV_NAME("fcs_choice",	 VAR_STRING), ശൂന്യ, 0},
    {VV_NAME("beval_bufnr",	 VAR_സംഖ്യ), ശൂന്യ, VV_RO},
    {VV_NAME("beval_winnr",	 VAR_സംഖ്യ), ശൂന്യ, VV_RO},
    {VV_NAME("beval_winid",	 VAR_സംഖ്യ), ശൂന്യ, VV_RO},
    {VV_NAME("beval_lnum",	 VAR_സംഖ്യ), ശൂന്യ, VV_RO},
    {VV_NAME("beval_col",	 VAR_സംഖ്യ), ശൂന്യ, VV_RO},
    {VV_NAME("beval_text",	 VAR_STRING), ശൂന്യ, VV_RO},
    {VV_NAME("scrollstart",	 VAR_STRING), ശൂന്യ, 0},
    {VV_NAME("swapname",	 VAR_STRING), ശൂന്യ, VV_RO},
    {VV_NAME("swapchoice",	 VAR_STRING), ശൂന്യ, 0},
    {VV_NAME("swapcommand",	 VAR_STRING), ശൂന്യ, VV_RO},
    {VV_NAME("char",		 VAR_STRING), ശൂന്യ, 0},
    {VV_NAME("mouse_win",	 VAR_സംഖ്യ), ശൂന്യ, 0},
    {VV_NAME("mouse_winid",	 VAR_സംഖ്യ), ശൂന്യ, 0},
    {VV_NAME("mouse_lnum",	 VAR_സംഖ്യ), ശൂന്യ, 0},
    {VV_NAME("mouse_col",	 VAR_സംഖ്യ), ശൂന്യ, 0},
    {VV_NAME("operator",	 VAR_STRING), ശൂന്യ, VV_RO},
    {VV_NAME("searchforward",	 VAR_സംഖ്യ), ശൂന്യ, 0},
    {VV_NAME("hlsearch",	 VAR_സംഖ്യ), ശൂന്യ, 0},
    {VV_NAME("oldfiles",	 VAR_LIST), &t_list_string, 0},
    {VV_NAME("windowid",	 VAR_സംഖ്യ), ശൂന്യ, VV_RO},
    {VV_NAME("progpath",	 VAR_STRING), ശൂന്യ, VV_RO},
    {VV_NAME("completed_item",	 VAR_DICT), &t_dict_string, 0},
    {VV_NAME("option_new",	 VAR_STRING), ശൂന്യ, VV_RO},
    {VV_NAME("option_old",	 VAR_STRING), ശൂന്യ, VV_RO},
    {VV_NAME("option_oldlocal",	 VAR_STRING), ശൂന്യ, VV_RO},
    {VV_NAME("option_oldglobal", VAR_STRING), ശൂന്യ, VV_RO},
    {VV_NAME("option_command",	 VAR_STRING), ശൂന്യ, VV_RO},
    {VV_NAME("option_type",	 VAR_STRING), ശൂന്യ, VV_RO},
    {VV_NAME("errors",		 VAR_LIST), &t_list_string, 0},
    {VV_NAME("false",		 VAR_BOOL), ശൂന്യ, VV_RO},
    {VV_NAME("true",		 VAR_BOOL), ശൂന്യ, VV_RO},
    {VV_NAME("none",		 VAR_SPECIAL), ശൂന്യ, VV_RO},
    {VV_NAME("null",		 VAR_SPECIAL), ശൂന്യ, VV_RO},
    {VV_NAME("numbermax",	 VAR_സംഖ്യ), ശൂന്യ, VV_RO},
    {VV_NAME("numbermin",	 VAR_സംഖ്യ), ശൂന്യ, VV_RO},
    {VV_NAME("numbersize",	 VAR_സംഖ്യ), ശൂന്യ, VV_RO},
    {VV_NAME("vim_did_enter",	 VAR_സംഖ്യ), ശൂന്യ, VV_RO},
    {VV_NAME("testing",		 VAR_സംഖ്യ), ശൂന്യ, 0},
    {VV_NAME("t_number",	 VAR_സംഖ്യ), ശൂന്യ, VV_RO},
    {VV_NAME("t_string",	 VAR_സംഖ്യ), ശൂന്യ, VV_RO},
    {VV_NAME("t_func",		 VAR_സംഖ്യ), ശൂന്യ, VV_RO},
    {VV_NAME("t_list",		 VAR_സംഖ്യ), ശൂന്യ, VV_RO},
    {VV_NAME("t_dict",		 VAR_സംഖ്യ), ശൂന്യ, VV_RO},
    {VV_NAME("t_float",		 VAR_സംഖ്യ), ശൂന്യ, VV_RO},
    {VV_NAME("t_bool",		 VAR_സംഖ്യ), ശൂന്യ, VV_RO},
    {VV_NAME("t_none",		 VAR_സംഖ്യ), ശൂന്യ, VV_RO},
    {VV_NAME("t_job",		 VAR_സംഖ്യ), ശൂന്യ, VV_RO},
    {VV_NAME("t_channel",	 VAR_സംഖ്യ), ശൂന്യ, VV_RO},
    {VV_NAME("t_blob",		 VAR_സംഖ്യ), ശൂന്യ, VV_RO},
    {VV_NAME("t_class",		 VAR_സംഖ്യ), ശൂന്യ, VV_RO},
    {VV_NAME("t_object",	 VAR_സംഖ്യ), ശൂന്യ, VV_RO},
    {VV_NAME("termrfgresp",	 VAR_STRING), ശൂന്യ, VV_RO},
    {VV_NAME("termrbgresp",	 VAR_STRING), ശൂന്യ, VV_RO},
    {VV_NAME("termu7resp",	 VAR_STRING), ശൂന്യ, VV_RO},
    {VV_NAME("termstyleresp",	 VAR_STRING), ശൂന്യ, VV_RO},
    {VV_NAME("termblinkresp",	 VAR_STRING), ശൂന്യ, VV_RO},
    {VV_NAME("event",		 VAR_DICT), ശൂന്യ, VV_RO},
    {VV_NAME("versionlong",	 VAR_സംഖ്യ), ശൂന്യ, VV_RO},
    {VV_NAME("echospace",	 VAR_സംഖ്യ), ശൂന്യ, VV_RO},
    {VV_NAME("argv",		 VAR_LIST), &t_list_string, VV_RO},
    {VV_NAME("collate",		 VAR_STRING), ശൂന്യ, VV_RO},
    {VV_NAME("exiting",		 VAR_SPECIAL), ശൂന്യ, VV_RO},
    {VV_NAME("colornames",       VAR_DICT), &t_dict_string, VV_RO},
    {VV_NAME("sizeofint",	 VAR_സംഖ്യ), ശൂന്യ, VV_RO},
    {VV_NAME("sizeoflong",	 VAR_സംഖ്യ), ശൂന്യ, VV_RO},
    {VV_NAME("sizeofpointer",	 VAR_സംഖ്യ), ശൂന്യ, VV_RO},
    {VV_NAME("maxcol",		 VAR_സംഖ്യ), ശൂന്യ, VV_RO},
    {VV_NAME("python3_version",	 VAR_സംഖ്യ), ശൂന്യ, VV_RO},
    {VV_NAME("t_typealias",	 VAR_സംഖ്യ), ശൂന്യ, VV_RO},
    {VV_NAME("t_enum",		 VAR_സംഖ്യ), ശൂന്യ, VV_RO},
    {VV_NAME("t_enumvalue",	 VAR_സംഖ്യ), ശൂന്യ, VV_RO},
    {VV_NAME("stacktrace",	 VAR_LIST), &t_list_dict_any, VV_RO},
    {VV_NAME("t_tuple",		 VAR_സംഖ്യ), ശൂന്യ, VV_RO},
    {VV_NAME("wayland_display",  VAR_STRING), ശൂന്യ, VV_RO},
    {VV_NAME("clipmethod",	 VAR_STRING), ശൂന്യ, VV_RO},
    {VV_NAME("termda1",		 VAR_STRING), ശൂന്യ, VV_RO},
    {VV_NAME("termosc",	 VAR_STRING), ശൂന്യ, VV_RO},
    {VV_NAME("vim_did_init",	 VAR_സംഖ്യ), ശൂന്യ, VV_RO},
    {VV_NAME("clipproviders",	 VAR_DICT), ശൂന്യ, VV_RO},
    {VV_NAME("imagebackend",	 VAR_STRING), ശൂന്യ, VV_RO}
};

// shorthand
#define vv_tv_type	vv_di.di_tv.v_type
#define vv_nr		vv_di.di_tv.vval.v_സംഖ്യ
#define vv_float	vv_di.di_tv.vval.v_float
#define vv_str		vv_di.di_tv.vval.v_string
#define vv_list		vv_di.di_tv.vval.v_list
#define vv_dict		vv_di.di_tv.vval.v_dict
#define vv_blob		vv_di.di_tv.vval.v_blob
#define vv_tv		vv_di.di_tv

സ്ഥിതം dictitem_T	vimvars_var;		// variable used for v:
സ്ഥിതം dict_T		vimvardict;		// Dictionary with v: variables
#define vimvarht  vimvardict.dv_hashtab

// for VIM_VERSION_ defines
#അന്തര്ഭാവ "version.h"

സ്ഥിതം ശൂന്യം list_glob_vars(പൂർണ്ണസംഖ്യ *first);
സ്ഥിതം ശൂന്യം list_buf_vars(പൂർണ്ണസംഖ്യ *first);
സ്ഥിതം ശൂന്യം list_win_vars(പൂർണ്ണസംഖ്യ *first);
സ്ഥിതം ശൂന്യം list_tab_vars(പൂർണ്ണസംഖ്യ *first);
സ്ഥിതം char_u *list_arg_vars(exarg_T *eap, char_u *arg, പൂർണ്ണസംഖ്യ *first);
സ്ഥിതം char_u *ex_let_one(char_u *arg, typval_T *tv, പൂർണ്ണസംഖ്യ copy, പൂർണ്ണസംഖ്യ flags, char_u *endchars, char_u *op, പൂർണ്ണസംഖ്യ var_idx);
സ്ഥിതം പൂർണ്ണസംഖ്യ do_unlet_var(lval_T *lp, char_u *name_end, exarg_T *eap, പൂർണ്ണസംഖ്യ deep, ശൂന്യം *cookie);
സ്ഥിതം പൂർണ്ണസംഖ്യ do_lock_var(lval_T *lp, char_u *name_end, exarg_T *eap, പൂർണ്ണസംഖ്യ deep, ശൂന്യം *cookie);
സ്ഥിതം ശൂന്യം list_one_var(dictitem_T *v, അക്ഷരം *prefix, പൂർണ്ണസംഖ്യ *first);
സ്ഥിതം ശൂന്യം list_one_var_a(അക്ഷരം *prefix, char_u *name, പൂർണ്ണസംഖ്യ type, char_u *string, പൂർണ്ണസംഖ്യ *first);

/*
 * Initialize global and vim special variables
 */
    ശൂന്യം
evalvars_init(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		    i;
    ഘടന vimvar   *p;

    init_var_dict(&globvardict, &globvars_var, VAR_DEF_SCOPE);
    init_var_dict(&vimvardict, &vimvars_var, VAR_SCOPE);
    vimvardict.dv_lock = VAR_FIXED;
    hash_init(&compat_hashtab);

    വേണ്ടി (i = 0; i < VV_LEN; ++i)
    {
	p = &vimvars[i];
	എങ്കിൽ (p->vv_flags & VV_RO)
	    p->vv_di.di_flags = DI_FLAGS_RO | DI_FLAGS_FIX;
	അല്ലെങ്കിൽ എങ്കിൽ (p->vv_flags & VV_RO_SBX)
	    p->vv_di.di_flags = DI_FLAGS_RO_SBX | DI_FLAGS_FIX;
	അല്ലെങ്കിൽ
	    p->vv_di.di_flags = DI_FLAGS_FIX;

	// add to v: scope dict, unless the value is not always available
	എങ്കിൽ (p->vv_tv_type != VAR_UNKNOWN)
	    hash_add(&vimvarht, p->vv_di.di_താക്കോൽ, "initialization");
	എങ്കിൽ (p->vv_flags & VV_COMPAT)
	    // add to compat scope dict
	    hash_add(&compat_hashtab, p->vv_di.di_താക്കോൽ, "initialization");
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
    set_vim_var_nr(VV_NUMBERSIZE, വലുപ്പം(varnumber_T) * 8);
    set_vim_var_nr(VV_SIZEOFINT, വലുപ്പം(പൂർണ്ണസംഖ്യ));
    set_vim_var_nr(VV_SIZEOFLONG, വലുപ്പം(long));
    set_vim_var_nr(VV_SIZEOFPOINTER, വലുപ്പം(അക്ഷരം *));
    set_vim_var_nr(VV_MAXCOL, MAXCOL);

    set_vim_var_nr(VV_TYPE_സംഖ്യ,  VAR_TYPE_സംഖ്യ);
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

#എങ്കിൽ defined(EXITFREE)
/*
 * Free all vim variables information on exit
 */
    ശൂന്യം
evalvars_clear(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		    i;
    ഘടന vimvar   *p;

    വേണ്ടി (i = 0; i < VV_LEN; ++i)
    {
	p = &vimvars[i];
	എങ്കിൽ (p->vv_di.di_tv.v_type == VAR_STRING)
	    VIM_CLEAR(p->vv_str);
	അല്ലെങ്കിൽ എങ്കിൽ (p->vv_di.di_tv.v_type == VAR_LIST)
	{
	    list_unref(p->vv_list);
	    p->vv_list = ശൂന്യ;
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
    വേണ്ടി (i = 1; i <= script_ഇനങ്ങൾ.ga_len; ++i)
	vars_clear(&SCRIPT_VARS(i));
}
#endif

    പൂർണ്ണസംഖ്യ
garbage_collect_globvars(പൂർണ്ണസംഖ്യ copyID)
{
    തിരികെ set_ref_in_ht(&globvarht, copyID, ശൂന്യ, ശൂന്യ);
}

    പൂർണ്ണസംഖ്യ
garbage_collect_vimvars(പൂർണ്ണസംഖ്യ copyID)
{
    തിരികെ set_ref_in_ht(&vimvarht, copyID, ശൂന്യ, ശൂന്യ);
}

    പൂർണ്ണസംഖ്യ
garbage_collect_scriptvars(പൂർണ്ണസംഖ്യ copyID)
{
    പൂർണ്ണസംഖ്യ		    i;
    പൂർണ്ണസംഖ്യ		    idx;
    പൂർണ്ണസംഖ്യ		    abort = FALSE;
    scriptitem_T    *si;

    വേണ്ടി (i = 1; i <= script_ഇനങ്ങൾ.ga_len; ++i)
    {
	abort = abort || set_ref_in_ht(&SCRIPT_VARS(i), copyID, ശൂന്യ, ശൂന്യ);

	si = SCRIPT_ITEM(i);
	വേണ്ടി (idx = 0; idx < si->sn_var_vals.ga_len; ++idx)
	{
	    svar_T    *sv = ((svar_T *)si->sn_var_vals.ga_data) + idx;

	    എങ്കിൽ (sv->sv_name != ശൂന്യ)
		abort = abort || set_ref_in_item(sv->sv_tv, copyID, ശൂന്യ, ശൂന്യ, ശൂന്യ);
	}
    }

    തിരികെ abort;
}

/*
 * Set an internal variable to a string value. Creates the variable if it does
 * not already exist.
 */
    ശൂന്യം
set_internal_string_var(char_u *name, char_u *മൂല്യം)
{
    char_u	*val;
    typval_T	*tvp;

    val = vim_strsave(മൂല്യം);
    എങ്കിൽ (val == ശൂന്യ)
	തിരികെ;

    tvp = alloc_string_tv(val);
    എങ്കിൽ (tvp == ശൂന്യ)
	തിരികെ;

    set_var(name, tvp, FALSE);
    free_tv(tvp);
}

    പൂർണ്ണസംഖ്യ
eval_charconvert(
    char_u	*enc_from,
    char_u	*enc_to,
    char_u	*fname_from,
    char_u	*fname_to)
{
    പൂർണ്ണസംഖ്യ		err = FALSE;
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;

    set_vim_var_string(VV_CC_FROM, enc_from, -1);
    set_vim_var_string(VV_CC_TO, enc_to, -1);
    set_vim_var_string(VV_FNAME_IN, fname_from, -1);
    set_vim_var_string(VV_FNAME_OUT, fname_to, -1);
    ctx = get_option_sctx("charconvert");
    എങ്കിൽ (ctx != ശൂന്യ)
	current_sctx = *ctx;

    എങ്കിൽ (eval_to_bool(p_ccv, &err, ശൂന്യ, FALSE, TRUE))
	err = TRUE;

    set_vim_var_string(VV_CC_FROM, ശൂന്യ, -1);
    set_vim_var_string(VV_CC_TO, ശൂന്യ, -1);
    set_vim_var_string(VV_FNAME_IN, ശൂന്യ, -1);
    set_vim_var_string(VV_FNAME_OUT, ശൂന്യ, -1);
    current_sctx = saved_sctx;

    എങ്കിൽ (err)
	തിരികെ FAIL;
    തിരികെ OK;
}

#എങ്കിൽ defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
    പൂർണ്ണസംഖ്യ
eval_printexpr(char_u *fname, char_u *args)
{
    പൂർണ്ണസംഖ്യ		err = FALSE;
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;

    set_vim_var_string(VV_FNAME_IN, fname, -1);
    set_vim_var_string(VV_CMDARG, args, -1);
    ctx = get_option_sctx("printexpr");
    എങ്കിൽ (ctx != ശൂന്യ)
	current_sctx = *ctx;

    എങ്കിൽ (eval_to_bool(p_pexpr, &err, ശൂന്യ, FALSE, TRUE))
	err = TRUE;

    set_vim_var_string(VV_FNAME_IN, ശൂന്യ, -1);
    set_vim_var_string(VV_CMDARG, ശൂന്യ, -1);
    current_sctx = saved_sctx;

    എങ്കിൽ (err)
    {
	mch_remove(fname);
	തിരികെ FAIL;
    }
    തിരികെ OK;
}
#endif

#എങ്കിൽ defined(FEAT_DIFF)
    ശൂന്യം
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
    എങ്കിൽ (ctx != ശൂന്യ)
	current_sctx = *ctx;

    // errors are ignored
    tv = eval_വ്യഞ്ജകം_ext(p_dex, ശൂന്യ, TRUE);
    free_tv(tv);

    set_vim_var_string(VV_FNAME_IN, ശൂന്യ, -1);
    set_vim_var_string(VV_FNAME_NEW, ശൂന്യ, -1);
    set_vim_var_string(VV_FNAME_OUT, ശൂന്യ, -1);
    current_sctx = saved_sctx;
}

    ശൂന്യം
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
    എങ്കിൽ (ctx != ശൂന്യ)
	current_sctx = *ctx;

    // errors are ignored
    tv = eval_വ്യഞ്ജകം_ext(p_pex, ശൂന്യ, TRUE);
    free_tv(tv);

    set_vim_var_string(VV_FNAME_IN, ശൂന്യ, -1);
    set_vim_var_string(VV_FNAME_DIFF, ശൂന്യ, -1);
    set_vim_var_string(VV_FNAME_OUT, ശൂന്യ, -1);
    current_sctx = saved_sctx;
}
#endif

#എങ്കിൽ defined(FEAT_SPELL)
/*
 * Evaluate an expression to a list with suggestions.
 * For the "expr:" part of 'spellsuggest'.
 * Returns NULL when there is an error.
 */
    list_T *
eval_spell_വ്യഞ്ജകം(char_u *badword, char_u *വ്യഞ്ജകം)
{
    typval_T	save_val;
    typval_T	rettv;
    list_T	*list = ശൂന്യ;
    char_u	*p = skipwhite(വ്യഞ്ജകം);
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;
    പൂർണ്ണസംഖ്യ		r;

    // Set "v:val" to the bad word.
    prepare_vimvar(VV_VAL, &save_val);
    set_vim_var_string(VV_VAL, badword, -1);
    എങ്കിൽ (p_verbose == 0)
	++emsg_off;
    ctx = get_option_sctx("spellsuggest");
    എങ്കിൽ (ctx != ശൂന്യ)
	current_sctx = *ctx;

    r = may_call_simple_func(p, &rettv);
    എങ്കിൽ (r == NOTDONE)
	r = eval1(&p, &rettv, &EVALARG_EVALUATE);
    എങ്കിൽ (r == OK)
    {
	എങ്കിൽ (rettv.v_type != VAR_LIST)
	    clear_tv(&rettv);
	അല്ലെങ്കിൽ
	    list = rettv.vval.v_list;
    }

    എങ്കിൽ (p_verbose == 0)
	--emsg_off;
    clear_tv(get_vim_var_tv(VV_VAL));
    restore_vimvar(VV_VAL, &save_val);
    current_sctx = saved_sctx;

    തിരികെ list;
}

/*
 * "list" is supposed to contain two items: a word and a number.  Return the
 * word in "pp" and the number as the return value.
 * Return -1 if anything isn't right.
 * Used to get the good word and score from the eval_spell_expr() result.
 */
    പൂർണ്ണസംഖ്യ
get_spellword(list_T *list, char_u **pp)
{
    listitem_T	*li;

    li = list->lv_first;
    എങ്കിൽ (li == ശൂന്യ)
	തിരികെ -1;
    *pp = tv_get_string(&li->li_tv);

    li = li->li_next;
    എങ്കിൽ (li == ശൂന്യ)
	തിരികെ -1;
    തിരികെ (പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ(&li->li_tv);
}
#endif

/*
 * Prepare v: variable "idx" to be used.
 * Save the current typeval in "save_tv" and clear it.
 * When not used yet add the variable to the v: hashtable.
 */
    ശൂന്യം
prepare_vimvar(പൂർണ്ണസംഖ്യ idx, typval_T *save_tv)
{
    *save_tv = vimvars[idx].vv_tv;
    vimvars[idx].vv_str = ശൂന്യ;  // don't free it now
    എങ്കിൽ (vimvars[idx].vv_tv_type == VAR_UNKNOWN)
	hash_add(&vimvarht, vimvars[idx].vv_di.di_താക്കോൽ, "prepare vimvar");
}

/*
 * Restore v: variable "idx" to typeval "save_tv".
 * Note that the v: variable must have been cleared already.
 * When no longer defined, remove the variable from the v: hashtable.
 */
    ശൂന്യം
restore_vimvar(പൂർണ്ണസംഖ്യ idx, typval_T *save_tv)
{
    hashitem_T	*hi;

    vimvars[idx].vv_tv = *save_tv;
    എങ്കിൽ (vimvars[idx].vv_tv_type != VAR_UNKNOWN)
	തിരികെ;

    hi = hash_find(&vimvarht, vimvars[idx].vv_di.di_താക്കോൽ);
    എങ്കിൽ (HASHITEM_EMPTY(hi))
	internal_error("restore_vimvar()");
    അല്ലെങ്കിൽ
	hash_remove(&vimvarht, hi, "restore vimvar");
}

/*
 * List Vim variables.
 */
    സ്ഥിതം ശൂന്യം
list_vim_vars(പൂർണ്ണസംഖ്യ *first)
{
    list_hashtable_vars(&vimvarht, "v:", FALSE, first);
}

/*
 * List script-local variables, if there is a script.
 */
    സ്ഥിതം ശൂന്യം
list_script_vars(പൂർണ്ണസംഖ്യ *first)
{
    എങ്കിൽ (SCRIPT_ID_VALID(current_sctx.sc_sid))
	list_hashtable_vars(&SCRIPT_VARS(current_sctx.sc_sid),
							   "s:", FALSE, first);
}

/*
 * Return TRUE if "name" starts with "g:", "w:", "t:" or "b:".
 * But only when an identifier character follows.
 */
    പൂർണ്ണസംഖ്യ
is_scoped_variable(char_u *name)
{
    തിരികെ vim_strchr((char_u *)"gwbt", name[0]) != ശൂന്യ
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
eval_one_വ്യഞ്ജകം_in_str(char_u *p, garray_T *gap, പൂർണ്ണസംഖ്യ evaluate)
{
    char_u	*block_start = skipwhite(p + 1);  // skip the opening {
    char_u	*block_end = block_start;
    char_u	*വ്യഞ്ജകം_val;

    എങ്കിൽ (*block_start == NUL)
    {
	semsg(_(e_missing_close_curly_str), p);
	തിരികെ ശൂന്യ;
    }
    എങ്കിൽ (skip_വ്യഞ്ജകം(&block_end, ശൂന്യ) == FAIL)
	തിരികെ ശൂന്യ;
    block_end = skipwhite(block_end);
    എങ്കിൽ (*block_end != '}')
    {
	semsg(_(e_missing_close_curly_str), p);
	തിരികെ ശൂന്യ;
    }
    എങ്കിൽ (evaluate)
    {
	*block_end = NUL;
	വ്യഞ്ജകം_val = eval_to_string(block_start, FALSE, FALSE);
	*block_end = '}';
	എങ്കിൽ (വ്യഞ്ജകം_val == ശൂന്യ)
	    തിരികെ ശൂന്യ;
	ga_concat(gap, വ്യഞ്ജകം_val);
	vim_free(വ്യഞ്ജകം_val);
    }

    തിരികെ block_end + 1;
}

/*
 * Evaluate all the Vim expressions {expr} in "str" and return the resulting
 * string in allocated memory.  "{{" is reduced to "{" and "}}" to "}".
 * Used for a heredoc assignment.
 * Returns NULL for an error.
 */
    സ്ഥിതം char_u *
eval_all_വ്യഞ്ജകം_in_str(char_u *str)
{
    garray_T	ga;
    char_u	*p;

    ga_init2(&ga, 1, 80);
    p = str;

    ആകുമ്പോൾ (*p != NUL)
    {
	char_u	*lit_start;
	പൂർണ്ണസംഖ്യ	escaped_brace = FALSE;

	// Look for a block start.
	lit_start = p;
	ആകുമ്പോൾ (*p != '{' && *p != '}' && *p != NUL)
	    ++p;

	എങ്കിൽ (*p != NUL && *p == p[1])
	{
	    // Escaped brace, unescape and continue.
	    // Include the brace in the literal string.
	    ++p;
	    escaped_brace = TRUE;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (*p == '}')
	{
	    semsg(_(e_stray_closing_curly_str), str);
	    ga_clear(&ga);
	    തിരികെ ശൂന്യ;
	}

	// Append the literal part.
	ga_concat_len(&ga, lit_start, (size_t)(p - lit_start));

	എങ്കിൽ (*p == NUL)
	    നിർത്തുക;

	എങ്കിൽ (escaped_brace)
	{
	    // Skip the second brace.
	    ++p;
	    തുടരുക;
	}

	// Evaluate the expression and append the result.
	p = eval_one_വ്യഞ്ജകം_in_str(p, &ga, TRUE);
	എങ്കിൽ (p == ശൂന്യ)
	{
	    ga_clear(&ga);
	    തിരികെ ശൂന്യ;
	}
    }
    ga_append(&ga, NUL);

    തിരികെ ga.ga_data;
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
heredoc_get(exarg_T *eap, char_u *cmd, പൂർണ്ണസംഖ്യ script_get, പൂർണ്ണസംഖ്യ vim9compile)
{
    char_u	*theline = ശൂന്യ;
    char_u	*marker;
    list_T	*l;
    char_u	*p;
    char_u	*str;
    പൂർണ്ണസംഖ്യ		marker_indent_len = 0;
    പൂർണ്ണസംഖ്യ		text_indent_len = 0;
    char_u	*text_indent = ശൂന്യ;
    char_u	dot[] = ".";
    പൂർണ്ണസംഖ്യ		comment_char = in_vim9script() ? '#' : '"';
    പൂർണ്ണസംഖ്യ		evalstr = FALSE;
    പൂർണ്ണസംഖ്യ		eval_failed = FALSE;
    cctx_T	*cctx = vim9compile ? eap->cookie : ശൂന്യ;
    പൂർണ്ണസംഖ്യ		എണ്ണം = 0;
    പൂർണ്ണസംഖ്യ		heredoc_in_string = FALSE;
    char_u	*വരി_arg = ശൂന്യ;
    char_u	*nl_ptr = vim_strchr(cmd, '\n');

    എങ്കിൽ (nl_ptr != ശൂന്യ)
    {
	heredoc_in_string = TRUE;
	വരി_arg = nl_ptr + 1;
	*nl_ptr = NUL;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (eap->ea_getline == ശൂന്യ)
    {
	emsg(_(e_cannot_use_heredoc_here));
	തിരികെ ശൂന്യ;
    }

    // Check for the optional 'trim' word before the marker
    cmd = skipwhite(cmd);

    ആകുമ്പോൾ (TRUE)
    {
	എങ്കിൽ (STRNCMP(cmd, "trim", 4) == 0
		&& (cmd[4] == NUL || VIM_ISWHITE(cmd[4])))
	{
	    cmd = skipwhite(cmd + 4);

	    // Trim the indentation from all the lines in the here document.
	    // The amount of indentation trimmed is the same as the indentation
	    // of the first line after the :let command line.  To find the end
	    // marker the indent of the :let command line is trimmed.
	    p = *eap->cmdlinep;
	    ആകുമ്പോൾ (VIM_ISWHITE(*p))
	    {
		p++;
		marker_indent_len++;
	    }
	    text_indent_len = -1;

	    തുടരുക;
	}
	എങ്കിൽ (STRNCMP(cmd, "eval", 4) == 0
		&& (cmd[4] == NUL || VIM_ISWHITE(cmd[4])))
	{
	    cmd = skipwhite(cmd + 4);
	    evalstr = TRUE;
	    തുടരുക;
	}
	നിർത്തുക;
    }

    // The marker is the next word.
    എങ്കിൽ (*cmd != NUL && *cmd != comment_char)
    {
	marker = skipwhite(cmd);
	p = skiptowhite(marker);
	എങ്കിൽ (*skipwhite(p) != NUL && *skipwhite(p) != comment_char)
	{
	    semsg(_(e_trailing_characters_str), p);
	    തിരികെ ശൂന്യ;
	}
	*p = NUL;
	എങ്കിൽ (!script_get && vim_islower(*marker))
	{
	    emsg(_(e_marker_cannot_start_with_lower_case_letter));
	    തിരികെ ശൂന്യ;
	}
    }
    അല്ലെങ്കിൽ
    {
	// When getting lines for an embedded script, if the marker is missing,
	// accept '.' as the marker.
	എങ്കിൽ (script_get)
	    marker = dot;
	അല്ലെങ്കിൽ
	{
	    emsg(_(e_missing_marker));
	    തിരികെ ശൂന്യ;
	}
    }

    l = list_alloc();
    എങ്കിൽ (l == ശൂന്യ)
	തിരികെ ശൂന്യ;

    വേണ്ടി (;;)
    {
	പൂർണ്ണസംഖ്യ	mi = 0;
	പൂർണ്ണസംഖ്യ	ti = 0;

	എങ്കിൽ (heredoc_in_string)
	{
	    char_u	*next_വരി;

	    // heredoc in a string separated by newlines.  Get the next line
	    // from the string.

	    എങ്കിൽ (*വരി_arg == NUL)
	    {
		semsg(_(e_missing_end_marker_str), marker);
		നിർത്തുക;
	    }

	    theline = വരി_arg;
	    next_വരി = vim_strchr(theline, '\n');
	    എങ്കിൽ (next_വരി == ശൂന്യ)
		വരി_arg += STRLEN(വരി_arg);
	    അല്ലെങ്കിൽ
	    {
		*next_വരി = NUL;
		വരി_arg = next_വരി + 1;
	    }
	}
	അല്ലെങ്കിൽ
	{
	    vim_free(theline);
	    theline = eap->ea_getline(NUL, eap->cookie, 0, FALSE);
	    എങ്കിൽ (theline == ശൂന്യ)
	    {
		semsg(_(e_missing_end_marker_str), marker);
		നിർത്തുക;
	    }
	}

	// with "trim": skip the indent matching the :let line to find the
	// marker
	എങ്കിൽ (marker_indent_len > 0
		&& STRNCMP(theline, *eap->cmdlinep, marker_indent_len) == 0)
	    mi = marker_indent_len;
	എങ്കിൽ (STRCMP(marker, theline + mi) == 0)
	    നിർത്തുക;

	// If expression evaluation failed in the heredoc, then skip till the
	// end marker.
	എങ്കിൽ (eval_failed)
	    തുടരുക;

	എങ്കിൽ (text_indent_len == -1 && *theline != NUL)
	{
	    // set the text indent from the first line.
	    p = theline;
	    text_indent_len = 0;
	    ആകുമ്പോൾ (VIM_ISWHITE(*p))
	    {
		p++;
		text_indent_len++;
	    }
	    text_indent = vim_strnsave(theline, text_indent_len);
	}
	// with "trim": skip the indent matching the first line
	എങ്കിൽ (text_indent != ശൂന്യ)
	    വേണ്ടി (ti = 0; ti < text_indent_len; ++ti)
		എങ്കിൽ (theline[ti] != text_indent[ti])
		    നിർത്തുക;

	str = theline + ti;
	എങ്കിൽ (vim9compile)
	{
	    എങ്കിൽ (compile_all_വ്യഞ്ജകം_in_str(str, evalstr, cctx) == FAIL)
	    {
		vim_free(theline);
		vim_free(text_indent);
		list_free(l);
		തിരികെ FAIL;
	    }
	    എണ്ണം++;
	}
	അല്ലെങ്കിൽ
	{
	    പൂർണ്ണസംഖ്യ	    free_str = FALSE;

	    എങ്കിൽ (evalstr && !eap->skip)
	    {
		str = eval_all_വ്യഞ്ജകം_in_str(str);
		എങ്കിൽ (str == ശൂന്യ)
		{
		    // expression evaluation failed
		    eval_failed = TRUE;
		    തുടരുക;
		}
		free_str = TRUE;
	    }

	    എങ്കിൽ (list_append_string(l, str, -1) == FAIL)
	    {
		എങ്കിൽ (free_str)
		    vim_free(str);
		നിർത്തുക;
	    }
	    എങ്കിൽ (free_str)
		vim_free(str);
	}
    }
    എങ്കിൽ (heredoc_in_string)
	// Next command follows the heredoc in the string.
	eap->nextcmd = വരി_arg;
    അല്ലെങ്കിൽ
	vim_free(theline);
    vim_free(text_indent);

    എങ്കിൽ (vim9compile && cctx->ctx_skip != SKIP_YES && !eval_failed)
	generate_NEWLIST(cctx, എണ്ണം, FALSE);

    എങ്കിൽ (eval_failed)
    {
	// expression evaluation in the heredoc failed
	list_free(l);
	തിരികെ ശൂന്യ;
    }
    തിരികെ l;
}

/*
 * Vim9 variable declaration:
 * ":var name"
 * ":var name: type"
 * ":var name = expr"
 * ":var name: type = expr"
 * etc.
 */
    ശൂന്യം
ex_var(exarg_T *eap)
{
    char_u *p = eap->cmd;
    പൂർണ്ണസംഖ്യ	    has_var;

    എങ്കിൽ (!in_vim9script())
    {
	semsg(_(e_str_cannot_be_used_in_legacy_vim_script), ":var");
	തിരികെ;
    }
    has_var = checkforcmd_noparen(&p, "var", 3);
    എങ്കിൽ (current_sctx.sc_sid == 0 && has_var)
    {
	emsg(_(e_cannot_declare_variable_on_command_വരി));
	തിരികെ;
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
    ശൂന്യം
ex_let(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    char_u	*വ്യഞ്ജകം = ശൂന്യ;
    typval_T	rettv;
    പൂർണ്ണസംഖ്യ		var_എണ്ണം = 0;
    പൂർണ്ണസംഖ്യ		semicolon = 0;
    char_u	op[4];
    char_u	*argend;
    പൂർണ്ണസംഖ്യ		first = TRUE;
    പൂർണ്ണസംഖ്യ		concat;
    പൂർണ്ണസംഖ്യ		has_assign;
    പൂർണ്ണസംഖ്യ		flags = 0;
    പൂർണ്ണസംഖ്യ		vim9script = in_vim9script();

    എങ്കിൽ (eap->cmdidx == CMD_final && !vim9script)
    {
	// In legacy Vim script ":final" is short for ":finally".
	ex_finally(eap);
	തിരികെ;
    }
    എങ്കിൽ (eap->cmdidx == CMD_let && vim9script)
    {
	emsg(_(e_cannot_use_let_in_vim9_script));
	തിരികെ;
    }

    എങ്കിൽ (eap->cmdidx == CMD_const)
	flags |= ASSIGN_CONST;
    അല്ലെങ്കിൽ എങ്കിൽ (eap->cmdidx == CMD_final)
	flags |= ASSIGN_FINAL;

    // Vim9 assignment without ":let", ":const" or ":final"
    എങ്കിൽ (eap->arg == eap->cmd)
	flags |= ASSIGN_NO_DECL;

    argend = skip_var_list(arg, TRUE, &var_എണ്ണം, &semicolon, FALSE);
    എങ്കിൽ (argend == ശൂന്യ)
	തിരികെ;
    വ്യഞ്ജകം = skipwhite(argend);
    concat = വ്യഞ്ജകം[0] == '.'
	&& ((വ്യഞ്ജകം[1] == '=' && in_old_script(2))
		|| (വ്യഞ്ജകം[1] == '.' && വ്യഞ്ജകം[2] == '='));
    has_assign =  *വ്യഞ്ജകം == '=' || (vim_strchr((char_u *)"+-*/%", *വ്യഞ്ജകം) != ശൂന്യ
							    && വ്യഞ്ജകം[1] == '=');
    എങ്കിൽ (!has_assign && !concat)
    {
	// ":let" without "=": list variables
	എങ്കിൽ (*arg == '[')
	    emsg(_(e_invalid_argument));
	അല്ലെങ്കിൽ എങ്കിൽ (വ്യഞ്ജകം[0] == '.' && വ്യഞ്ജകം[1] == '=')
	    emsg(_(e_dot_equal_not_supported_with_script_version_two));
	അല്ലെങ്കിൽ എങ്കിൽ (!ends_excmd2(eap->cmd, arg))
	{
	    എങ്കിൽ (vim9script)
	    {
		എങ്കിൽ (!ends_excmd2(eap->cmd, skipwhite(argend)))
		    semsg(_(e_trailing_characters_str), argend);
		അല്ലെങ്കിൽ
		    // Vim9 declaration ":var name: type"
		    arg = vim9_declare_scriptvar(eap, arg);
	    }
	    അല്ലെങ്കിൽ
	    {
		// ":let var1 var2" - list values
		arg = list_arg_vars(eap, arg, &first);
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (!eap->skip)
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
	തിരികെ;
    }

    എങ്കിൽ (വ്യഞ്ജകം[0] == '=' && വ്യഞ്ജകം[1] == '<' && വ്യഞ്ജകം[2] == '<')
    {
	list_T	*l = ശൂന്യ;
	long	cur_lnum = SOURCING_LNUM;

	// :let text =<< [trim] [eval] END
	// :var text =<< [trim] [eval] END
	എങ്കിൽ (vim9script && !eap->skip && (!VIM_ISWHITE(വ്യഞ്ജകം[-1])
						 || !IS_WHITE_OR_NUL(വ്യഞ്ജകം[3])))
	    semsg(_(e_white_space_required_before_and_after_str_at_str),
								  "=<<", വ്യഞ്ജകം);
	അല്ലെങ്കിൽ
	    l = heredoc_get(eap, വ്യഞ്ജകം + 3, FALSE, FALSE);

	എങ്കിൽ (l != ശൂന്യ)
	{
	    rettv_list_set(&rettv, l);
	    എങ്കിൽ (!eap->skip)
	    {
		// errors are for the assignment, not the end marker
		SOURCING_LNUM = cur_lnum;
		op[0] = '=';
		op[1] = NUL;
		(ശൂന്യം)ex_let_vars(eap->arg, &rettv, FALSE, semicolon, var_എണ്ണം,
								flags, op);
	    }
	    clear_tv(&rettv);
	}
	തിരികെ;
    }

    evalarg_T   evalarg;
    പൂർണ്ണസംഖ്യ	    len = 1;

    CLEAR_FIELD(rettv);

    പൂർണ്ണസംഖ്യ cur_lnum;

    op[0] = '=';
    op[1] = NUL;
    എങ്കിൽ (*വ്യഞ്ജകം != '=')
    {
	എങ്കിൽ (vim9script && (flags & ASSIGN_NO_DECL) == 0)
	{
	    // +=, /=, etc. require an existing variable
	    semsg(_(e_cannot_use_operator_on_new_variable_str), eap->arg);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (vim_strchr((char_u *)"+-*/%.", *വ്യഞ്ജകം) != ശൂന്യ)
	{
	    op[0] = *വ്യഞ്ജകം;   // +=, -=, *=, /=, %= or .=
	    ++len;
	    എങ്കിൽ (വ്യഞ്ജകം[0] == '.' && വ്യഞ്ജകം[1] == '.') // ..=
	    {
		++വ്യഞ്ജകം;
		++len;
	    }
	}
	വ്യഞ്ജകം += 2;
    }
    അല്ലെങ്കിൽ
	++വ്യഞ്ജകം;

    എങ്കിൽ (vim9script && !eap->skip && (!VIM_ISWHITE(*argend)
					       || !IS_WHITE_OR_NUL(*വ്യഞ്ജകം)))
    {
	vim_strncpy(op, വ്യഞ്ജകം - len, len);
	semsg(_(e_white_space_required_before_and_after_str_at_str),
							       op, argend);
    }

    എങ്കിൽ (eap->skip)
	++emsg_skip;
    fill_evalarg_from_eap(&evalarg, eap, eap->skip);
    വ്യഞ്ജകം = skipwhite_and_linebreak(വ്യഞ്ജകം, &evalarg);
    cur_lnum = SOURCING_LNUM;
    പൂർണ്ണസംഖ്യ eval_res = eval0(വ്യഞ്ജകം, &rettv, eap, &evalarg);
    എങ്കിൽ (eap->skip)
	--emsg_skip;
    clear_evalarg(&evalarg, eap);

    // Restore the line number so that any type error is given for the
    // declaration, not the expression.
    SOURCING_LNUM = cur_lnum;

    എങ്കിൽ (!eap->skip && eval_res != FAIL)
	(ശൂന്യം)ex_let_vars(eap->arg, &rettv, FALSE, semicolon, var_എണ്ണം,
								    flags, op);
    എങ്കിൽ (eval_res != FAIL)
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
    പൂർണ്ണസംഖ്യ
ex_let_vars(
    char_u	*arg_start,
    typval_T	*tv,
    പൂർണ്ണസംഖ്യ		copy,		// copy values from "tv", don't move
    പൂർണ്ണസംഖ്യ		semicolon,	// from skip_var_list()
    പൂർണ്ണസംഖ്യ		var_എണ്ണം,	// from skip_var_list()
    പൂർണ്ണസംഖ്യ		flags,		// ASSIGN_FINAL, ASSIGN_CONST, etc.
    char_u	*op)
{
    char_u	*arg = arg_start;
    list_T	*l;
    tuple_T	*tuple = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		var_idx = 0;
    listitem_T	*item = ശൂന്യ;
    typval_T	ltv;
    പൂർണ്ണസംഖ്യ		is_list = tv->v_type == VAR_LIST;
    പൂർണ്ണസംഖ്യ		idx;

    എങ്കിൽ (tv->v_type == VAR_VOID)
    {
	emsg(_(e_cannot_use_void_മൂല്യം));
	തിരികെ FAIL;
    }
    എങ്കിൽ (*arg != '[')
    {
	// ":let var = expr" or ":for var in list"
	എങ്കിൽ (ex_let_one(arg, tv, copy, flags, op, op, var_idx) == ശൂന്യ)
	    തിരികെ FAIL;
	തിരികെ OK;
    }

    // ":let [v1, v2] = list" or ":for [v1, v2] in listlist"
    // or
    // ":let [v1, v2] = tuple" or ":for [v1, v2] in tupletuple"
    എങ്കിൽ (tv->v_type != VAR_LIST && tv->v_type != VAR_TUPLE)
    {
	emsg(_(e_list_or_tuple_required));
	തിരികെ FAIL;
    }
    എങ്കിൽ (is_list)
    {
	l = tv->vval.v_list;
	എങ്കിൽ (l == ശൂന്യ)
	{
	    emsg(_(e_list_required));
	    തിരികെ FAIL;
	}
	i = list_len(l);
    }
    അല്ലെങ്കിൽ
    {
	tuple = tv->vval.v_tuple;
	എങ്കിൽ (tuple == ശൂന്യ)
	{
	    emsg(_(e_tuple_required));
	    തിരികെ FAIL;
	}
	i = tuple_len(tuple);
    }

    എങ്കിൽ (semicolon == 0 && var_എണ്ണം < i)
    {
	emsg(_(is_list ? e_less_targets_than_list_ഇനങ്ങൾ
					: e_less_targets_than_tuple_ഇനങ്ങൾ));
	തിരികെ FAIL;
    }
    എങ്കിൽ (var_എണ്ണം - semicolon > i)
    {
	emsg(_(is_list ? e_more_targets_than_list_ഇനങ്ങൾ
					: e_more_targets_than_tuple_ഇനങ്ങൾ));
	തിരികെ FAIL;
    }

    എങ്കിൽ (is_list)
    {
	CHECK_LIST_MATERIALIZE(l);
	item = l->lv_first;
    }
    അല്ലെങ്കിൽ
	idx = 0;

    ആകുമ്പോൾ (*arg != ']')
    {
	arg = skipwhite(arg + 1);
	++var_idx;
	arg = ex_let_one(arg, is_list ? &item->li_tv : TUPLE_ITEM(tuple, idx),
			 TRUE, flags | ASSIGN_UNPACK, (char_u *)",;]", op,
			 var_idx);
	എങ്കിൽ (is_list)
	    item = item->li_next;
	അല്ലെങ്കിൽ
	    idx++;
	എങ്കിൽ (arg == ശൂന്യ)
	    തിരികെ FAIL;

	arg = skipwhite(arg);
	എങ്കിൽ (*arg == ';')
	{
	    // Put the rest of the list or tuple (may be empty) in the var
	    // after ';'.  Create a new list or tuple for this.
	    എങ്കിൽ (is_list)
	    {
		// Put the rest of the list (may be empty) in the var
		// after ';'.  Create a new list for this.
		l = list_alloc();
		എങ്കിൽ (l == ശൂന്യ)
		    തിരികെ FAIL;

		// list
		ആകുമ്പോൾ (item != ശൂന്യ)
		{
		    list_append_tv(l, &item->li_tv);
		    item = item->li_next;
		}

		ltv.v_type = VAR_LIST;
		ltv.v_lock = 0;
		ltv.vval.v_list = l;
		l->lv_refcount = 1;
	    }
	    അല്ലെങ്കിൽ
	    {
		tuple_T	*new_tuple = tuple_alloc();
		എങ്കിൽ (new_tuple == ശൂന്യ)
		    തിരികെ FAIL;

		// Put the rest of the tuple (may be empty) in the var
		// after ';'.  Create a new tuple for this.
		ആകുമ്പോൾ (idx < TUPLE_LEN(tuple))
		{
		    typval_T    new_tv;

		    copy_tv(TUPLE_ITEM(tuple, idx), &new_tv);
		    എങ്കിൽ (tuple_append_tv(new_tuple, &new_tv) == FAIL)
		    {
			clear_tv(&new_tv);
			tuple_unref(new_tuple);
			തിരികെ FAIL;
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
	    എങ്കിൽ (arg == ശൂന്യ)
		തിരികെ FAIL;
	    നിർത്തുക;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (*arg != ',' && *arg != ']')
	{
	    internal_error("ex_let_vars()");
	    തിരികെ FAIL;
	}
    }

    തിരികെ OK;
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
    പൂർണ്ണസംഖ്യ		include_type,
    പൂർണ്ണസംഖ്യ		*var_എണ്ണം,
    പൂർണ്ണസംഖ്യ		*semicolon,
    പൂർണ്ണസംഖ്യ		silent)
{
    char_u	*p, *s;

    എങ്കിൽ (*arg == '[')
    {
	// "[var, var]": find the matching ']'.
	p = arg;
	വേണ്ടി (;;)
	{
	    p = skipwhite(p + 1);	// skip whites after '[', ';' or ','
	    s = skip_var_one(p, include_type);
	    എങ്കിൽ (s == p)
	    {
		എങ്കിൽ (!silent)
		    semsg(_(e_invalid_argument_str), p);
		തിരികെ ശൂന്യ;
	    }
	    ++*var_എണ്ണം;

	    p = skipwhite(s);
	    എങ്കിൽ (*p == ']')
		നിർത്തുക;
	    അല്ലെങ്കിൽ എങ്കിൽ (*p == ';')
	    {
		എങ്കിൽ (*semicolon == 1)
		{
		    എങ്കിൽ (!silent)
			emsg(_(e_double_semicolon_in_list_of_variables));
		    തിരികെ ശൂന്യ;
		}
		*semicolon = 1;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (*p != ',')
	    {
		എങ്കിൽ (!silent)
		    semsg(_(e_invalid_argument_str), p);
		തിരികെ ശൂന്യ;
	    }
	}
	തിരികെ p + 1;
    }

    തിരികെ skip_var_one(arg, include_type);
}

/*
 * Skip one (assignable) variable name, including @r, $VAR, &option, d.key,
 * l[idx].
 * In Vim9 script also skip over ": type" if "include_type" is TRUE.
 */
    char_u *
skip_var_one(char_u *arg, പൂർണ്ണസംഖ്യ include_type)
{
    char_u	*end;
    പൂർണ്ണസംഖ്യ		vim9 = in_vim9script();

    എങ്കിൽ (*arg == '@' && arg[1] != NUL)
	തിരികെ arg + 2;

    // termcap option name may have non-alpha characters
    എങ്കിൽ (STRNCMP(arg, "&t_", 3) == 0 && arg[3] != NUL && arg[4] != NUL)
	തിരികെ arg + 5;

    end = find_name_end(*arg == '$' || *arg == '&' ? arg + 1 : arg,
				   ശൂന്യ, ശൂന്യ, FNE_INCL_BR | FNE_CHECK_START);

    // "a: type" is declaring variable "a" with a type, not "a:".
    // Same for "s: type".
    എങ്കിൽ (vim9 && end == arg + 2 && end[-1] == ':')
	--end;

    എങ്കിൽ (include_type && vim9)
    {
	എങ്കിൽ (*skipwhite(end) == ':')
	    end = skip_type(skipwhite(skipwhite(end) + 1), FALSE);
    }
    തിരികെ end;
}

/*
 * List variables for hashtab "ht" with prefix "prefix".
 * If "empty" is TRUE also list NULL strings as empty strings.
 */
    ശൂന്യം
list_hashtable_vars(
    hashtab_T	*ht,
    അക്ഷരം	*prefix,
    പൂർണ്ണസംഖ്യ		empty,
    പൂർണ്ണസംഖ്യ		*first)
{
    hashitem_T	*hi;
    dictitem_T	*di;
    പൂർണ്ണസംഖ്യ		todo;
    char_u	buf[IOSIZE];

    പൂർണ്ണസംഖ്യ save_ht_flags = ht->ht_flags;
    ht->ht_flags |= HTFLAGS_FROZEN;

    todo = (പൂർണ്ണസംഖ്യ)ht->ht_used;
    വേണ്ടി (hi = ht->ht_array; todo > 0 && !got_int; ++hi)
    {
	എങ്കിൽ (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    di = HI2DI(hi);

	    // apply :filter /pat/ to variable name
	    vim_strncpy((char_u *)buf, (char_u *)prefix, IOSIZE - 1);
	    vim_strcat((char_u *)buf, di->di_താക്കോൽ, IOSIZE);
	    എങ്കിൽ (message_filtered(buf))
		തുടരുക;

	    എങ്കിൽ (empty || di->di_tv.v_type != VAR_STRING
					   || di->di_tv.vval.v_string != ശൂന്യ)
		list_one_var(di, prefix, first);
	}
    }

    ht->ht_flags = save_ht_flags;
}

/*
 * List global variables.
 */
    സ്ഥിതം ശൂന്യം
list_glob_vars(പൂർണ്ണസംഖ്യ *first)
{
    list_hashtable_vars(&globvarht, "", TRUE, first);
}

/*
 * List buffer variables.
 */
    സ്ഥിതം ശൂന്യം
list_buf_vars(പൂർണ്ണസംഖ്യ *first)
{
    list_hashtable_vars(&curbuf->b_vars->dv_hashtab, "b:", TRUE, first);
}

/*
 * List window variables.
 */
    സ്ഥിതം ശൂന്യം
list_win_vars(പൂർണ്ണസംഖ്യ *first)
{
    list_hashtable_vars(&curwin->w_vars->dv_hashtab, "w:", TRUE, first);
}

/*
 * List tab page variables.
 */
    സ്ഥിതം ശൂന്യം
list_tab_vars(പൂർണ്ണസംഖ്യ *first)
{
    list_hashtable_vars(&curtab->tp_vars->dv_hashtab, "t:", TRUE, first);
}

/*
 * List variables in "arg".
 */
    സ്ഥിതം char_u *
list_arg_vars(exarg_T *eap, char_u *arg, പൂർണ്ണസംഖ്യ *first)
{
    പൂർണ്ണസംഖ്യ		error = FALSE;
    പൂർണ്ണസംഖ്യ		len;
    char_u	*name;
    char_u	*name_start;
    char_u	*arg_subsc;
    char_u	*tofree;
    typval_T    tv;

    ആകുമ്പോൾ (!ends_excmd2(eap->cmd, arg) && !got_int)
    {
	എങ്കിൽ (error || eap->skip)
	{
	    arg = find_name_end(arg, ശൂന്യ, ശൂന്യ, FNE_INCL_BR | FNE_CHECK_START);
	    എങ്കിൽ (!VIM_ISWHITE(*arg) && !ends_excmd(*arg))
	    {
		emsg_severe = TRUE;
		എങ്കിൽ (!did_emsg)
		    semsg(_(e_trailing_characters_str), arg);
		നിർത്തുക;
	    }
	}
	അല്ലെങ്കിൽ
	{
	    // get_name_len() takes care of expanding curly braces
	    name_start = name = arg;
	    len = get_name_len(&arg, &tofree, TRUE, TRUE);
	    എങ്കിൽ (len <= 0)
	    {
		// This is mainly to keep test 49 working: when expanding
		// curly braces fails overrule the exception error message.
		എങ്കിൽ (len < 0 && !aborting())
		{
		    emsg_severe = TRUE;
		    semsg(_(e_invalid_argument_str), arg);
		    നിർത്തുക;
		}
		error = TRUE;
	    }
	    അല്ലെങ്കിൽ
	    {
		arg = skipwhite(arg);
		എങ്കിൽ (tofree != ശൂന്യ)
		    name = tofree;
		എങ്കിൽ (eval_variable(name, len, 0, &tv, ശൂന്യ,
						     EVAL_VAR_VERBOSE) == FAIL)
		    error = TRUE;
		അല്ലെങ്കിൽ
		{
		    // handle d.key, l[idx], f(expr)
		    arg_subsc = arg;
		    എങ്കിൽ (handle_subscript(&arg, name_start, &tv,
					      &EVALARG_EVALUATE, TRUE) == FAIL)
			error = TRUE;
		    അല്ലെങ്കിൽ
		    {
			എങ്കിൽ (arg == arg_subsc && len == 2 && name[1] == ':')
			{
			    തിരഞ്ഞെടുപ്പ് (*name)
			    {
				അവസ്ഥ 'g': list_glob_vars(first); നിർത്തുക;
				അവസ്ഥ 'b': list_buf_vars(first); നിർത്തുക;
				അവസ്ഥ 'w': list_win_vars(first); നിർത്തുക;
				അവസ്ഥ 't': list_tab_vars(first); നിർത്തുക;
				അവസ്ഥ 'v': list_vim_vars(first); നിർത്തുക;
				അവസ്ഥ 's': list_script_vars(first); നിർത്തുക;
				അവസ്ഥ 'l': list_func_vars(first); നിർത്തുക;
				സ്ഥിരസ്ഥിതി:
					  semsg(_(e_cant_list_variables_for_str), name);
			    }
			}
			അല്ലെങ്കിൽ
			{
			    char_u	numbuf[NUMBUFLEN];
			    char_u	*tf;
			    പൂർണ്ണസംഖ്യ		c;
			    char_u	*s;

			    s = echo_string(&tv, &tf, numbuf, 0);
			    c = *arg;
			    *arg = NUL;
			    list_one_var_a("",
				    arg == arg_subsc ? name : name_start,
				    tv.v_type,
				    s == ശൂന്യ ? (char_u *)"" : s,
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

    തിരികെ arg;
}

/*
 * Set an environment variable, part of ex_let_one().
 */
    സ്ഥിതം char_u *
ex_let_env(
    char_u	*arg,
    typval_T	*tv,
    പൂർണ്ണസംഖ്യ		flags,
    char_u	*endchars,
    char_u	*op)
{
    char_u	*arg_end = ശൂന്യ;
    char_u	*name;
    പൂർണ്ണസംഖ്യ		len;

    എങ്കിൽ ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					 && (flags & ASSIGN_FOR_LOOP) == 0)
    {
	emsg(_(e_cannot_lock_environment_variable));
	തിരികെ ശൂന്യ;
    }

    // Find the end of the name.
    ++arg;
    name = arg;
    len = get_env_len(&arg);
    എങ്കിൽ (len == 0)
	semsg(_(e_invalid_argument_str), name - 1);
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (op != ശൂന്യ && vim_strchr((char_u *)"+-*/%", *op) != ശൂന്യ)
	    semsg(_(e_wrong_variable_type_for_str_equal), op);
	അല്ലെങ്കിൽ എങ്കിൽ (endchars != ശൂന്യ
			      && vim_strchr(endchars, *skipwhite(arg)) == ശൂന്യ)
	    emsg(_(e_unexpected_characters_in_let));
	അല്ലെങ്കിൽ എങ്കിൽ (!check_secure() && !check_restricted())
	{
	    char_u	*tofree = ശൂന്യ;
	    പൂർണ്ണസംഖ്യ		c1 = name[len];
	    char_u	*p;

	    name[len] = NUL;
	    p = tv_get_string_chk(tv);
	    എങ്കിൽ (p != ശൂന്യ && op != ശൂന്യ && *op == '.')
	    {
		പൂർണ്ണസംഖ്യ	mustfree = FALSE;
		char_u  *s = vim_getenv(name, &mustfree);

		എങ്കിൽ (s != ശൂന്യ)
		{
		    p = tofree = concat_str(s, p);
		    എങ്കിൽ (mustfree)
			vim_free(s);
		}
	    }
	    എങ്കിൽ (p != ശൂന്യ)
	    {
		vim_setenv_ext(name, p);
		arg_end = arg;
	    }
	    name[len] = c1;
	    vim_free(tofree);
	}
    }
    തിരികെ arg_end;
}

/*
 * Set an option, part of ex_let_one().
 */
    സ്ഥിതം char_u *
ex_let_option(
    char_u	*arg,
    typval_T	*tv,
    പൂർണ്ണസംഖ്യ		flags,
    char_u	*endchars,
    char_u	*op)
{
    char_u	*p;
    പൂർണ്ണസംഖ്യ		scope;
    char_u	*arg_end = ശൂന്യ;

    എങ്കിൽ ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					 && (flags & ASSIGN_FOR_LOOP) == 0)
    {
	emsg(_(e_cannot_lock_option));
	തിരികെ ശൂന്യ;
    }

    // Find the end of the name.
    p = find_option_end(&arg, &scope);
    എങ്കിൽ (p == ശൂന്യ || (endchars != ശൂന്യ
			       && vim_strchr(endchars, *skipwhite(p)) == ശൂന്യ))
    {
	emsg(_(e_unexpected_characters_in_let));
	തിരികെ ശൂന്യ;
    }

    പൂർണ്ണസംഖ്യ		c1;
    long	n = 0;
    getoption_T	opt_type;
    long	numval;
    char_u	*stringval = ശൂന്യ;
    char_u	*s = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		failed = FALSE;
    പൂർണ്ണസംഖ്യ		opt_p_flags;
    char_u	*tofree = ശൂന്യ;
    char_u	numbuf[NUMBUFLEN];

    c1 = *p;
    *p = NUL;

    opt_type = get_option_മൂല്യം(arg, &numval, &stringval, &opt_p_flags, scope);
    എങ്കിൽ (opt_type == gov_unknown && arg[0] != 't' && arg[1] != '_')
    {
	semsg(_(e_unknown_option_str_2), arg);
	പോകുക theend;
    }
    എങ്കിൽ (op != ശൂന്യ && *op != '='
	&& (((opt_type == gov_bool || opt_type == gov_സംഖ്യ) && *op == '.')
	    || (opt_type == gov_string && *op != '.')))
    {
	semsg(_(e_wrong_variable_type_for_str_equal), op);
	പോകുക theend;
    }

    എങ്കിൽ ((opt_type == gov_bool
		|| opt_type == gov_സംഖ്യ
		|| opt_type == gov_hidden_bool
		|| opt_type == gov_hidden_സംഖ്യ)
		     && (tv->v_type != VAR_STRING || !in_vim9script()))
    {
	എങ്കിൽ (opt_type == gov_bool || opt_type == gov_hidden_bool)
	    // bool, possibly hidden
	    n = (long)tv_get_bool_chk(tv, &failed);
	അല്ലെങ്കിൽ
	    // number, possibly hidden
	    n = (long)tv_get_സംഖ്യ_chk(tv, &failed);
	എങ്കിൽ (failed)
	    പോകുക theend;
    }

    എങ്കിൽ ((opt_p_flags & P_FUNC) && (tv->v_type == VAR_PARTIAL
						    || tv->v_type == VAR_FUNC))
    {
	// If the option can be set to a function reference or a lambda
	// and the passed value is a function reference, then convert it to
	// the name (string) of the function reference.
	s = tv2string(tv, &tofree, numbuf, 0);
	എങ്കിൽ (s == ശൂന്യ)
	    പോകുക theend;
    }
    // Avoid setting a string option to the text "v:false" or similar.
    // In Vim9 script also don't convert a number to string.
    അല്ലെങ്കിൽ എങ്കിൽ (tv->v_type != VAR_BOOL && tv->v_type != VAR_SPECIAL
		     && (!in_vim9script() || tv->v_type != VAR_സംഖ്യ))
    {
	s = tv_get_string_chk(tv);
	എങ്കിൽ (s == ശൂന്യ)
	    പോകുക theend;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (opt_type == gov_string || opt_type == gov_hidden_string)
    {
	emsg(_(e_string_required));
	പോകുക theend;
    }

    എങ്കിൽ (op != ശൂന്യ && *op != '=')
    {
	// number, in legacy script also bool
	എങ്കിൽ (opt_type == gov_സംഖ്യ
				 || (opt_type == gov_bool && !in_vim9script()))
	{
	    തിരഞ്ഞെടുപ്പ് (*op)
	    {
		അവസ്ഥ '+': n = numval + n; നിർത്തുക;
		അവസ്ഥ '-': n = numval - n; നിർത്തുക;
		അവസ്ഥ '*': n = numval * n; നിർത്തുക;
		അവസ്ഥ '/': n = (long)അക്കം_divide(numval, n, &failed); നിർത്തുക;
		അവസ്ഥ '%': n = (long)അക്കം_modulus(numval, n, &failed); നിർത്തുക;
	    }
	    s = ശൂന്യ;
	    എങ്കിൽ (failed)
		പോകുക theend;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (opt_type == gov_string && stringval != ശൂന്യ && s != ശൂന്യ)
	{
	    // string
	    s = concat_str(stringval, s);
	    vim_free(stringval);
	    stringval = s;
	}
    }

    അക്ഷരം *err = set_option_മൂല്യം(arg, n, s, scope);
    arg_end = p;
    എങ്കിൽ (err != ശൂന്യ)
	emsg(_(err));

theend:
    *p = c1;
    vim_free(stringval);
    vim_free(tofree);
    തിരികെ arg_end;
}

/*
 * Set a register, part of ex_let_one().
 */
    സ്ഥിതം char_u *
ex_let_register(
    char_u	*arg,
    typval_T	*tv,
    പൂർണ്ണസംഖ്യ		flags,
    char_u	*endchars,
    char_u	*op)
{
    char_u	*arg_end = ശൂന്യ;

    എങ്കിൽ ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					 && (flags & ASSIGN_FOR_LOOP) == 0)
    {
	emsg(_(e_cannot_lock_register));
	തിരികെ ശൂന്യ;
    }
    ++arg;
    എങ്കിൽ (op != ശൂന്യ && vim_strchr((char_u *)"+-*/%", *op) != ശൂന്യ)
	semsg(_(e_wrong_variable_type_for_str_equal), op);
    അല്ലെങ്കിൽ എങ്കിൽ (endchars != ശൂന്യ
			  && vim_strchr(endchars, *skipwhite(arg + 1)) == ശൂന്യ)
	emsg(_(e_unexpected_characters_in_let));
    അല്ലെങ്കിൽ
    {
	char_u	*ptofree = ശൂന്യ;
	char_u	*p;

	p = tv_get_string_chk(tv);
	എങ്കിൽ (p != ശൂന്യ && op != ശൂന്യ && *op == '.')
	{
	    char_u  *s = get_reg_contents(*arg == '@'
						  ? '"' : *arg, GREG_വ്യഞ്ജകം_SRC);

	    എങ്കിൽ (s != ശൂന്യ)
	    {
		p = ptofree = concat_str(s, p);
		vim_free(s);
	    }
	}
	എങ്കിൽ (p != ശൂന്യ)
	{
	    write_reg_contents(*arg == '@' ? '"' : *arg, p, -1, FALSE);
	    arg_end = arg + 1;
	}
	vim_free(ptofree);
    }
    തിരികെ arg_end;
}

/*
 * Set one item of ":let var = expr" or ":let [v1, v2] = list" to its value.
 * Returns a pointer to the char just after the var name.
 * Returns NULL if there is an error.
 */
    സ്ഥിതം char_u *
ex_let_one(
    char_u	*arg,		// points to variable name
    typval_T	*tv,		// value to assign to variable
    പൂർണ്ണസംഖ്യ		copy,		// copy value from "tv"
    പൂർണ്ണസംഖ്യ		flags,		// ASSIGN_CONST, ASSIGN_FINAL, etc.
    char_u	*endchars,	// valid chars after variable name  or NULL
    char_u	*op,		// "+", "-", "."  or NULL
    പൂർണ്ണസംഖ്യ		var_idx)	// variable index for "let [a, b] = list"
{
    char_u	*arg_end = ശൂന്യ;

    എങ്കിൽ (in_vim9script() && (flags & (ASSIGN_NO_DECL | ASSIGN_DECL)) == 0
			&& (flags & (ASSIGN_CONST | ASSIGN_FINAL)) == 0
				  && vim_strchr((char_u *)"$@&", *arg) != ശൂന്യ)
    {
	vim9_declare_error(arg);
	തിരികെ ശൂന്യ;
    }

    എങ്കിൽ (check_typval_is_മൂല്യം(tv) == FAIL)
	തിരികെ ശൂന്യ;

    // In Vim9 script an environment variable and a register only take a
    // String.  "@#" also takes a buffer number.
    എങ്കിൽ (in_vim9script() && (*arg == '$' || *arg == '@')
	    && !(*arg == '@' && arg[1] == '#' && tv->v_type == VAR_സംഖ്യ
					       && (op == ശൂന്യ || *op == '=')))
    {
	where_T	where = WHERE_INIT;

	എങ്കിൽ (check_typval_type(&t_string, tv, where) == FAIL)
	    തിരികെ ശൂന്യ;
    }

    എങ്കിൽ (*arg == '$')
    {
	// ":let $VAR = expr": Set environment variable.
	തിരികെ ex_let_env(arg, tv, flags, endchars, op);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (*arg == '&')
    {
	// ":let &option = expr": Set option value.
	// ":let &l:option = expr": Set local option value.
	// ":let &g:option = expr": Set global option value.
	// ":for &ts in range(8)": Set option value for for loop
	തിരികെ ex_let_option(arg, tv, flags, endchars, op);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (*arg == '@')
    {
	// ":let @r = expr": Set register contents.
	തിരികെ ex_let_register(arg, tv, flags, endchars, op);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (eval_isnamec1(*arg) || *arg == '{')
    {
	lval_T	lv;
	char_u	*p;
	പൂർണ്ണസംഖ്യ	lval_flags = (flags & (ASSIGN_NO_DECL | ASSIGN_DECL))
							     ? GLV_NO_DECL : 0;
	lval_flags |= (flags & ASSIGN_FOR_LOOP) ? GLV_FOR_LOOP : 0;
	എങ്കിൽ (op != ശൂന്യ && *op != '=')
	    lval_flags |= GLV_ASSIGN_WITH_OP;

	// ":let var = expr": Set internal variable.
	// ":let var: type = expr": Set internal variable with type.
	// ":let {expr} = expr": Idem, name made with curly braces
	p = get_lval(arg, tv, &lv, FALSE, FALSE, lval_flags, FNE_CHECK_START);
	എങ്കിൽ (p != ശൂന്യ && lv.ll_name != ശൂന്യ)
	{
	    എങ്കിൽ (endchars != ശൂന്യ && vim_strchr(endchars,
					   *skipwhite(lv.ll_name_end)) == ശൂന്യ)
	    {
		emsg(_(e_unexpected_characters_in_let));
	    }
	    അല്ലെങ്കിൽ
	    {
		set_var_lval(&lv, p, tv, copy, flags, op, var_idx);
		arg_end = lv.ll_name_end;
	    }
	}
	clear_lval(&lv);
    }
    അല്ലെങ്കിൽ
	semsg(_(e_invalid_argument_str), arg);

    തിരികെ arg_end;
}

/*
 * ":unlet[!] var1 ... " command.
 */
    ശൂന്യം
ex_unlet(exarg_T *eap)
{
    ex_unletlock(eap, eap->arg, 0, eap->forceit ? GLV_QUIET : 0, do_unlet_var, ശൂന്യ);
}

/*
 * ":lockvar" and ":unlockvar" commands
 */
    ശൂന്യം
ex_lockvar(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    പൂർണ്ണസംഖ്യ		deep = 2;

    എങ്കിൽ (eap->forceit)
	deep = -1;
    അല്ലെങ്കിൽ എങ്കിൽ (vim_isdigit(*arg))
    {
	deep = getdigits(&arg);
	arg = skipwhite(arg);
    }

    ex_unletlock(eap, arg, deep, 0, do_lock_var, ശൂന്യ);
}

/*
 * ":unlet", ":lockvar" and ":unlockvar" are quite similar.
 * Also used for Vim9 script.  "callback" is invoked as:
 *	callback(&lv, name_end, eap, deep, cookie)
 */
    ശൂന്യം
ex_unletlock(
    exarg_T	*eap,
    char_u	*argstart,
    പൂർണ്ണസംഖ്യ		deep,
    പൂർണ്ണസംഖ്യ		glv_flags,
    പൂർണ്ണസംഖ്യ		(*callback)(lval_T *, char_u *, exarg_T *, പൂർണ്ണസംഖ്യ, ശൂന്യം *),
    ശൂന്യം	*cookie)
{
    char_u	*arg = argstart;
    char_u	*name_end;
    പൂർണ്ണസംഖ്യ		error = FALSE;
    lval_T	lv;

    ചെയ്യുക
    {
	എങ്കിൽ (*arg == '$')
	{
	    lv.ll_name = arg;
	    lv.ll_tv = ശൂന്യ;
	    ++arg;
	    എങ്കിൽ (get_env_len(&arg) == 0)
	    {
		semsg(_(e_invalid_argument_str), arg - 1);
		തിരികെ;
	    }
	    എങ്കിൽ (!error && !eap->skip
			      && callback(&lv, arg, eap, deep, cookie) == FAIL)
		error = TRUE;
	    name_end = arg;
	}
	അല്ലെങ്കിൽ
	{
	    // Parse the name and find the end.
	    name_end = get_lval(arg, ശൂന്യ, &lv, TRUE, eap->skip || error,
				     glv_flags | GLV_NO_DECL, FNE_CHECK_START);
	    എങ്കിൽ (lv.ll_name == ശൂന്യ)
		error = TRUE;	    // error but continue parsing
	    എങ്കിൽ (name_end == ശൂന്യ || (!VIM_ISWHITE(*name_end)
						    && !ends_excmd(*name_end)))
	    {
		എങ്കിൽ (name_end != ശൂന്യ)
		{
		    emsg_severe = TRUE;
		    semsg(_(e_trailing_characters_str), name_end);
		}
		എങ്കിൽ (!(eap->skip || error))
		    clear_lval(&lv);
		നിർത്തുക;
	    }

	    എങ്കിൽ (!error && !eap->skip
			 && callback(&lv, name_end, eap, deep, cookie) == FAIL)
		error = TRUE;

	    എങ്കിൽ (!eap->skip)
		clear_lval(&lv);
	}

	arg = skipwhite(name_end);
    } ആകുമ്പോൾ (!ends_excmd2(name_end, arg));

    set_nextcmd(eap, arg);
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
do_unlet_var(
    lval_T	*lp,
    char_u	*name_end,
    exarg_T	*eap,
    പൂർണ്ണസംഖ്യ		deep UNUSED,
    ശൂന്യം	*cookie UNUSED)
{
    പൂർണ്ണസംഖ്യ		forceit = eap->forceit;
    പൂർണ്ണസംഖ്യ		ret = OK;
    പൂർണ്ണസംഖ്യ		cc;

    എങ്കിൽ (lp->ll_tv == ശൂന്യ)
    {
	cc = *name_end;
	*name_end = NUL;

	// Environment variable, normal name or expanded name.
	എങ്കിൽ (*lp->ll_name == '$')
	    vim_unsetenv_ext(lp->ll_name + 1);
	അല്ലെങ്കിൽ എങ്കിൽ (do_unlet(lp->ll_name, forceit) == FAIL)
	    ret = FAIL;
	*name_end = cc;
    }
    അല്ലെങ്കിൽ എങ്കിൽ ((lp->ll_list != ശൂന്യ
		 && മൂല്യം_check_lock(lp->ll_list->lv_lock, lp->ll_name, FALSE))
	    || (lp->ll_dict != ശൂന്യ
		&& മൂല്യം_check_lock(lp->ll_dict->dv_lock, lp->ll_name, FALSE)))
	തിരികെ FAIL;
    അല്ലെങ്കിൽ എങ്കിൽ (lp->ll_range)
	list_unlet_range(lp->ll_list, lp->ll_li, lp->ll_n1,
						    !lp->ll_empty2, lp->ll_n2);
    അല്ലെങ്കിൽ എങ്കിൽ (lp->ll_list != ശൂന്യ)
	// unlet a List item.
	listitem_remove(lp->ll_list, lp->ll_li);
    അല്ലെങ്കിൽ എങ്കിൽ (lp->ll_dict != ശൂന്യ)
	// unlet a Dictionary item.
	dictitem_remove(lp->ll_dict, lp->ll_di, "unlet");
    അല്ലെങ്കിൽ
    {
	semsg(_(e_cannot_unlet_imported_item_str), lp->ll_name);
	തിരികെ FAIL;
    }

    തിരികെ ret;
}

/*
 * Unlet one item or a range of items from a list.
 * Return OK or FAIL.
 */
    ശൂന്യം
list_unlet_range(
	list_T	    *l,
	listitem_T  *li_first,
	long	    n1_arg,
	പൂർണ്ണസംഖ്യ	    has_n2,
	long	    n2)
{
    listitem_T  *li = li_first;
    പൂർണ്ണസംഖ്യ		n1 = n1_arg;

    // Delete a range of List items.
    li = li_first;
    n1 = n1_arg;
    ആകുമ്പോൾ (li != ശൂന്യ && (!has_n2 || n2 >= n1))
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
    പൂർണ്ണസംഖ്യ
do_unlet(char_u *name, പൂർണ്ണസംഖ്യ forceit)
{
    hashtab_T	*ht;
    hashitem_T	*hi;
    char_u	*varname = ശൂന്യ;  // init to shut up gcc
    dict_T	*d;
    dictitem_T	*di;

    // can't :unlet a script variable in Vim9 script
    എങ്കിൽ (in_vim9script() && check_vim9_unlet(name) == FAIL)
	തിരികെ FAIL;

    ht = find_var_ht(name, &varname);

    // can't :unlet a script variable in Vim9 script from a function
    എങ്കിൽ (ht == get_script_local_ht()
	    && SCRIPT_ID_VALID(current_sctx.sc_sid)
	    && SCRIPT_ITEM(current_sctx.sc_sid)->sn_version
							 == SCRIPT_VERSION_VIM9
	    && check_vim9_unlet(name) == FAIL)
	തിരികെ FAIL;

    എങ്കിൽ (ht != ശൂന്യ && *varname != NUL)
    {
	d = get_current_funccal_dict(ht);
	എങ്കിൽ (d == ശൂന്യ)
	{
	    എങ്കിൽ (ht == &globvarht)
		d = &globvardict;
	    അല്ലെങ്കിൽ എങ്കിൽ (ht == &compat_hashtab)
		d = &vimvardict;
	    അല്ലെങ്കിൽ
	    {
		di = find_var_in_ht(ht, *name, (char_u *)"", FALSE);
		d = di == ശൂന്യ ? ശൂന്യ : di->di_tv.vval.v_dict;
	    }
	    എങ്കിൽ (d == ശൂന്യ)
	    {
		internal_error("do_unlet()");
		തിരികെ FAIL;
	    }
	}
	hi = hash_find(ht, varname);
	എങ്കിൽ (HASHITEM_EMPTY(hi))
	    hi = find_hi_in_scoped_ht(name, &ht);
	എങ്കിൽ (hi != ശൂന്യ && !HASHITEM_EMPTY(hi))
	{
	    di = HI2DI(hi);
	    എങ്കിൽ (var_check_fixed(di->di_flags, name, FALSE)
		    || var_check_ro(di->di_flags, name, FALSE)
		    || മൂല്യം_check_lock(d->dv_lock, name, FALSE)
		    || check_hashtab_frozen(ht, "unlet"))
		തിരികെ FAIL;

	    delete_var(ht, hi);
	    തിരികെ OK;
	}
    }
    എങ്കിൽ (forceit)
	തിരികെ OK;
    semsg(_(e_no_such_variable_str), name);
    തിരികെ FAIL;
}

    സ്ഥിതം ശൂന്യം
report_lockvar_member(അക്ഷരം *സന്ദേശം, lval_T *lp)
{
    പൂർണ്ണസംഖ്യ did_alloc = FALSE;
    char_u *vname = (char_u *)"";
    char_u *class_name = lp->ll_class != ശൂന്യ
				    ? lp->ll_class->class_name.string : (char_u *)"";
    എങ്കിൽ (lp->ll_name != ശൂന്യ)
    {
	എങ്കിൽ (lp->ll_name_end == ശൂന്യ)
	    vname = lp->ll_name;
	അല്ലെങ്കിൽ
	{
	    vname = vim_strnsave(lp->ll_name, lp->ll_name_end - lp->ll_name);
	    എങ്കിൽ (vname == ശൂന്യ)
		തിരികെ;
	    did_alloc = TRUE;
	}
    }
    semsg(_(സന്ദേശം), vname, class_name);
    എങ്കിൽ (did_alloc)
	vim_free(vname);
}

/*
 * Lock or unlock variable indicated by "lp".
 * "deep" is the levels to go (-1 for unlimited);
 * "lock" is TRUE for ":lockvar", FALSE for ":unlockvar".
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
do_lock_var(
    lval_T	*lp,
    char_u	*name_end,
    exarg_T	*eap,
    പൂർണ്ണസംഖ്യ		deep,
    ശൂന്യം	*cookie UNUSED)
{
    പൂർണ്ണസംഖ്യ		lock = eap->cmdidx == CMD_lockvar;
    പൂർണ്ണസംഖ്യ		ret = OK;
    പൂർണ്ണസംഖ്യ		cc;
    dictitem_T	*di;

#ifdef LOG_LOCKVAR
    ch_log(ശൂന്യ, "LKVAR: do_lock_var(): name %s, is_root %d", lp->ll_name, lp->ll_is_വേര്);
#endif

    എങ്കിൽ (lp->ll_tv == ശൂന്യ)
    {
	cc = *name_end;
	*name_end = NUL;
	എങ്കിൽ (*lp->ll_name == '$')
	{
	    semsg(_(e_cannot_lock_or_unlock_variable_str), lp->ll_name);
	    ret = FAIL;
	}
	അല്ലെങ്കിൽ
	{
	    // Normal name or expanded name.
	    di = find_var(lp->ll_name, ശൂന്യ, TRUE);
	    എങ്കിൽ (di == ശൂന്യ)
	    {
		എങ്കിൽ (in_vim9script())
		    semsg(_(e_cannot_find_variable_to_unlock_str),
								  lp->ll_name);
		ret = FAIL;
	    }
	    അല്ലെങ്കിൽ
	    {
		എങ്കിൽ ((di->di_flags & DI_FLAGS_FIX)
			    && di->di_tv.v_type != VAR_DICT
			    && di->di_tv.v_type != VAR_LIST)
		{
		    // For historic reasons this error is not given for a list
		    // or dict.  E.g., the b: dict could be locked/unlocked.
		    semsg(_(e_cannot_lock_or_unlock_variable_str), lp->ll_name);
		    ret = FAIL;
		}
		അല്ലെങ്കിൽ
		{
		    എങ്കിൽ (in_vim9script())
		    {
			svar_T  *sv = find_typval_in_script(&di->di_tv,
								     0, FALSE);

			എങ്കിൽ (sv != ശൂന്യ && sv->sv_const != 0)
			{
			    semsg(_(e_cannot_change_readonly_variable_str),
								  lp->ll_name);
			    ret = FAIL;
			}
		    }

		    എങ്കിൽ (ret == OK)
		    {
			എങ്കിൽ (lock)
			    di->di_flags |= DI_FLAGS_LOCK;
			അല്ലെങ്കിൽ
			    di->di_flags &= ~DI_FLAGS_LOCK;
			എങ്കിൽ (deep != 0)
			    item_lock(&di->di_tv, deep, lock, FALSE);
		    }
		}
	    }
	}
	*name_end = cc;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (deep == 0 && lp->ll_object == ശൂന്യ && lp->ll_class == ശൂന്യ)
    {
	// nothing to do
    }
    അല്ലെങ്കിൽ എങ്കിൽ (lp->ll_is_വേര്)
	// (un)lock the item.
	item_lock(lp->ll_tv, deep, lock, FALSE);
    അല്ലെങ്കിൽ എങ്കിൽ (lp->ll_range)
    {
	listitem_T    *li = lp->ll_li;

	// (un)lock a range of List items.
	ആകുമ്പോൾ (li != ശൂന്യ && (lp->ll_empty2 || lp->ll_n2 >= lp->ll_n1))
	{
	    item_lock(&li->li_tv, deep, lock, FALSE);
	    li = li->li_next;
	    ++lp->ll_n1;
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (lp->ll_list != ശൂന്യ)
	// (un)lock a List item.
	item_lock(&lp->ll_li->li_tv, deep, lock, FALSE);
    അല്ലെങ്കിൽ എങ്കിൽ (lp->ll_object != ശൂന്യ)  // This check must be before ll_class.
    {
	// (un)lock an object variable.
	report_lockvar_member(e_cannot_lock_object_variable_str, lp);
	ret = FAIL;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (lp->ll_class != ശൂന്യ)
    {
	// (un)lock a class variable.
	report_lockvar_member(e_cannot_lock_class_variable_str, lp);
	ret = FAIL;
    }
    അല്ലെങ്കിൽ
    {
	// (un)lock a Dictionary item.
	എങ്കിൽ (lp->ll_di == ശൂന്യ)
	{
	    emsg(_(e_dictionary_required));
	    ret = FAIL;
	}
	അല്ലെങ്കിൽ
	    item_lock(&lp->ll_di->di_tv, deep, lock, FALSE);
    }

    തിരികെ ret;
}

/*
 * Lock or unlock an item.  "deep" is nr of levels to go.
 * When "check_refcount" is TRUE do not lock a list or dict with a reference
 * count larger than 1.
 */
    ശൂന്യം
item_lock(typval_T *tv, പൂർണ്ണസംഖ്യ deep, പൂർണ്ണസംഖ്യ lock, പൂർണ്ണസംഖ്യ check_refcount)
{
    സ്ഥിതം പൂർണ്ണസംഖ്യ	recurse = 0;
    list_T	*l;
    listitem_T	*li;
    dict_T	*d;
    blob_T	*b;
    hashitem_T	*hi;
    പൂർണ്ണസംഖ്യ		todo;

#ifdef LOG_LOCKVAR
    ch_log(ശൂന്യ, "LKVAR: item_lock(): type %s", vartype_name(tv->v_type));
#endif

    എങ്കിൽ (recurse >= DICT_MAXNEST)
    {
	emsg(_(e_variable_nested_too_deep_for_unlock));
	തിരികെ;
    }
    എങ്കിൽ (deep == 0)
	തിരികെ;
    ++recurse;

    // lock/unlock the item itself
    എങ്കിൽ (lock)
	tv->v_lock |= VAR_LOCKED;
    അല്ലെങ്കിൽ
	tv->v_lock &= ~VAR_LOCKED;

    തിരഞ്ഞെടുപ്പ് (tv->v_type)
    {
	അവസ്ഥ VAR_UNKNOWN:
	അവസ്ഥ VAR_ANY:
	അവസ്ഥ VAR_VOID:
	അവസ്ഥ VAR_സംഖ്യ:
	അവസ്ഥ VAR_BOOL:
	അവസ്ഥ VAR_STRING:
	അവസ്ഥ VAR_FUNC:
	അവസ്ഥ VAR_PARTIAL:
	അവസ്ഥ VAR_FLOAT:
	അവസ്ഥ VAR_SPECIAL:
	അവസ്ഥ VAR_JOB:
	അവസ്ഥ VAR_CHANNEL:
	അവസ്ഥ VAR_INSTR:
	അവസ്ഥ VAR_CLASS:
	അവസ്ഥ VAR_OBJECT:
	അവസ്ഥ VAR_TYPEALIAS:
	    നിർത്തുക;

	അവസ്ഥ VAR_BLOB:
	    എങ്കിൽ ((b = tv->vval.v_blob) != ശൂന്യ
				    && !(check_refcount && b->bv_refcount > 1))
	    {
		എങ്കിൽ (lock)
		    b->bv_lock |= VAR_LOCKED;
		അല്ലെങ്കിൽ
		    b->bv_lock &= ~VAR_LOCKED;
	    }
	    നിർത്തുക;
	അവസ്ഥ VAR_LIST:
	    എങ്കിൽ ((l = tv->vval.v_list) != ശൂന്യ
				    && !(check_refcount && l->lv_refcount > 1))
	    {
		എങ്കിൽ (lock)
		    l->lv_lock |= VAR_LOCKED;
		അല്ലെങ്കിൽ
		    l->lv_lock &= ~VAR_LOCKED;
		എങ്കിൽ (deep < 0 || deep > 1)
		{
		    എങ്കിൽ (l->lv_first == &range_list_item)
			l->lv_lock |= VAR_ഇനങ്ങൾ_LOCKED;
		    അല്ലെങ്കിൽ
		    {
			// recursive: lock/unlock the items the List contains
			CHECK_LIST_MATERIALIZE(l);
			FOR_ALL_LIST_ഇനങ്ങൾ(l, li) item_lock(&li->li_tv,
					       deep - 1, lock, check_refcount);
		    }
		}
	    }
	    നിർത്തുക;
	അവസ്ഥ VAR_TUPLE:
	    tuple_lock(tv->vval.v_tuple, deep, lock, check_refcount);
	    നിർത്തുക;
	അവസ്ഥ VAR_DICT:
	    എങ്കിൽ ((d = tv->vval.v_dict) != ശൂന്യ
				    && !(check_refcount && d->dv_refcount > 1))
	    {
		എങ്കിൽ (lock)
		    d->dv_lock |= VAR_LOCKED;
		അല്ലെങ്കിൽ
		    d->dv_lock &= ~VAR_LOCKED;
		എങ്കിൽ (deep < 0 || deep > 1)
		{
		    // recursive: lock/unlock the items the List contains
		    todo = (പൂർണ്ണസംഖ്യ)d->dv_hashtab.ht_used;
		    FOR_ALL_HASHTAB_ഇനങ്ങൾ(&d->dv_hashtab, hi, todo)
		    {
			എങ്കിൽ (!HASHITEM_EMPTY(hi))
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

#എങ്കിൽ defined(FEAT_MENU) && defined(FEAT_MULTI_LANG)
/*
 * Delete all "menutrans_" variables.
 */
    ശൂന്യം
del_menutrans_vars(ശൂന്യം)
{
    hashitem_T	*hi;
    പൂർണ്ണസംഖ്യ		todo;

    hash_lock(&globvarht);
    todo = (പൂർണ്ണസംഖ്യ)globvarht.ht_used;
    വേണ്ടി (hi = globvarht.ht_array; todo > 0 && !got_int; ++hi)
    {
	എങ്കിൽ (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    എങ്കിൽ (STRNCMP(HI2DI(hi)->di_താക്കോൽ, "menutrans_", 10) == 0)
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

സ്ഥിതം char_u	*varnamebuf = ശൂന്യ;
സ്ഥിതം പൂർണ്ണസംഖ്യ	varnamebuflen = 0;

/*
 * Function to concatenate a prefix and a variable name.
 */
    char_u *
cat_prefix_varname(പൂർണ്ണസംഖ്യ prefix, char_u *name)
{
    പൂർണ്ണസംഖ്യ		len;

    len = (പൂർണ്ണസംഖ്യ)STRLEN(name) + 3;
    എങ്കിൽ (len > varnamebuflen)
    {
	vim_free(varnamebuf);
	len += 10;			// some additional space
	varnamebuf = alloc(len);
	എങ്കിൽ (varnamebuf == ശൂന്യ)
	{
	    varnamebuflen = 0;
	    തിരികെ ശൂന്യ;
	}
	varnamebuflen = len;
    }
    *varnamebuf = prefix;
    varnamebuf[1] = ':';
    STRCPY(varnamebuf + 2, name);
    തിരികെ varnamebuf;
}

/*
 * Function given to ExpandGeneric() to obtain the list of user defined
 * (global/buffer/window/built-in) variable names.
 */
    char_u *
get_user_var_name(expand_T *xp, പൂർണ്ണസംഖ്യ idx)
{
    സ്ഥിതം long_u	gdone;
    സ്ഥിതം long_u	bdone;
    സ്ഥിതം long_u	wdone;
    സ്ഥിതം long_u	tdone;
    സ്ഥിതം പൂർണ്ണസംഖ്യ		vidx;
    സ്ഥിതം hashitem_T	*hi;
    hashtab_T		*ht;

    എങ്കിൽ (idx == 0)
    {
	gdone = bdone = wdone = vidx = 0;
	tdone = 0;
    }

    // Global variables
    എങ്കിൽ (gdone < globvarht.ht_used)
    {
	എങ്കിൽ (gdone++ == 0)
	    hi = globvarht.ht_array;
	അല്ലെങ്കിൽ
	    ++hi;
	ആകുമ്പോൾ (HASHITEM_EMPTY(hi))
	    ++hi;
	എങ്കിൽ (STRNCMP("g:", xp->xp_pattern, 2) == 0)
	    തിരികെ cat_prefix_varname('g', hi->hi_താക്കോൽ);
	തിരികെ hi->hi_താക്കോൽ;
    }

    // b: variables
    ht = &prevwin_curwin()->w_buffer->b_vars->dv_hashtab;
    എങ്കിൽ (bdone < ht->ht_used)
    {
	എങ്കിൽ (bdone++ == 0)
	    hi = ht->ht_array;
	അല്ലെങ്കിൽ
	    ++hi;
	ആകുമ്പോൾ (HASHITEM_EMPTY(hi))
	    ++hi;
	തിരികെ cat_prefix_varname('b', hi->hi_താക്കോൽ);
    }

    // w: variables
    ht = &prevwin_curwin()->w_vars->dv_hashtab;
    എങ്കിൽ (wdone < ht->ht_used)
    {
	എങ്കിൽ (wdone++ == 0)
	    hi = ht->ht_array;
	അല്ലെങ്കിൽ
	    ++hi;
	ആകുമ്പോൾ (HASHITEM_EMPTY(hi))
	    ++hi;
	തിരികെ cat_prefix_varname('w', hi->hi_താക്കോൽ);
    }

    // t: variables
    ht = &curtab->tp_vars->dv_hashtab;
    എങ്കിൽ (tdone < ht->ht_used)
    {
	എങ്കിൽ (tdone++ == 0)
	    hi = ht->ht_array;
	അല്ലെങ്കിൽ
	    ++hi;
	ആകുമ്പോൾ (HASHITEM_EMPTY(hi))
	    ++hi;
	തിരികെ cat_prefix_varname('t', hi->hi_താക്കോൽ);
    }

    // v: variables
    എങ്കിൽ (vidx < VV_LEN)
	തിരികെ cat_prefix_varname('v', (char_u *)vimvars[vidx++].vv_name);

    VIM_CLEAR(varnamebuf);
    varnamebuflen = 0;
    തിരികെ ശൂന്യ;
}

    അക്ഷരം *
get_var_special_name(പൂർണ്ണസംഖ്യ nr)
{
    തിരഞ്ഞെടുപ്പ് (nr)
    {
	അവസ്ഥ VVAL_FALSE: തിരികെ in_vim9script() ? "false" : "v:false";
	അവസ്ഥ VVAL_TRUE:  തിരികെ in_vim9script() ? "true" : "v:true";
	അവസ്ഥ VVAL_NULL:  തിരികെ in_vim9script() ? "null" : "v:null";
	അവസ്ഥ VVAL_NONE:  തിരികെ "v:none";
    }
    internal_error("get_var_special_name()");
    തിരികെ "42";
}

/*
 * Returns the global variable dictionary
 */
    dict_T *
get_globvar_dict(ശൂന്യം)
{
    തിരികെ &globvardict;
}

/*
 * Returns the global variable hash table
 */
    hashtab_T *
get_globvar_ht(ശൂന്യം)
{
    തിരികെ &globvarht;
}

/*
 * Returns the v: variable dictionary
 */
    dict_T *
get_vimvar_dict(ശൂന്യം)
{
    തിരികെ &vimvardict;
}

/*
 * Returns the index of a v:variable.  Negative if not found.
 * Returns DI_ flags in "di_flags".
 */
    പൂർണ്ണസംഖ്യ
find_vim_var(char_u *name, പൂർണ്ണസംഖ്യ *di_flags)
{
    dictitem_T	    *di = find_var_in_ht(&vimvarht, 0, name, TRUE);
    ഘടന vimvar   *vv;

    എങ്കിൽ (di == ശൂന്യ)
	തിരികെ -1;
    *di_flags = di->di_flags;
    vv = (ഘടന vimvar *)((അക്ഷരം *)di - offsetof(vimvar_T, vv_di));
    തിരികെ (പൂർണ്ണസംഖ്യ)(vv - vimvars);
}


/*
 * Set type of v: variable to "type".
 */
    ശൂന്യം
set_vim_var_type(പൂർണ്ണസംഖ്യ idx, vartype_T type)
{
    vimvars[idx].vv_tv_type = type;
}

/*
 * Set number v: variable to "val".
 * Note that this does not set the type, use set_vim_var_type() for that.
 */
    ശൂന്യം
set_vim_var_nr(പൂർണ്ണസംഖ്യ idx, varnumber_T val)
{
    vimvars[idx].vv_nr = val;
}

    അക്ഷരം *
get_vim_var_name(പൂർണ്ണസംഖ്യ idx)
{
    തിരികെ vimvars[idx].vv_name;
}

/*
 * Get typval_T v: variable value.
 */
    typval_T *
get_vim_var_tv(പൂർണ്ണസംഖ്യ idx)
{
    തിരികെ &vimvars[idx].vv_tv;
}

    type_T *
get_vim_var_type(പൂർണ്ണസംഖ്യ idx, garray_T *type_list)
{
    എങ്കിൽ (vimvars[idx].vv_type != ശൂന്യ)
	തിരികെ vimvars[idx].vv_type;
    തിരികെ typval2type_vimvar(&vimvars[idx].vv_tv, type_list);
}

/*
 * Add what getinfo() reports for the v: variable "name", without the "v:", to
 * "d".  Returns FAIL when there is no such variable.
 */
    പൂർണ്ണസംഖ്യ
vim_var_info(char_u *name, dict_T *d)
{
    പൂർണ്ണസംഖ്യ		di_flags;
    പൂർണ്ണസംഖ്യ		idx = find_vim_var(name, &di_flags);
    garray_T	type_list;
    അക്ഷരം	*tofree;

    എങ്കിൽ (idx < 0)
	തിരികെ FAIL;
    vim_snprintf((അക്ഷരം *)IObuff, IOSIZE, "v:%s", vimvars[idx].vv_name);
    dict_add_string(d, "name", IObuff);
    dict_add_bool(d, "available", TRUE);
    ga_init2(&type_list, വലുപ്പം(type_T *), 10);
    അക്ഷരം *type = type_name(get_vim_var_type(idx, &type_list), &tofree);
    dict_add_string(d, "type", (char_u *)type);
    vim_free(tofree);
    clear_type_list(&type_list);
    dict_add_bool(d, "readonly", (vimvars[idx].vv_flags & VV_RO) != 0);
    dict_add_bool(d, "compat", (vimvars[idx].vv_flags & VV_COMPAT) != 0);
    തിരികെ OK;
}

/*
 * Set v: variable to "tv".  Only accepts the same type.
 * Takes over the value of "tv".
 */
    പൂർണ്ണസംഖ്യ
set_vim_var_tv(പൂർണ്ണസംഖ്യ idx, typval_T *tv)
{
    എങ്കിൽ (vimvars[idx].vv_tv_type != tv->v_type)
    {
	emsg(_(e_type_mismatch_for_v_variable));
	clear_tv(tv);
	തിരികെ FAIL;
    }
    // VV_RO is also checked when compiling, but let's check here as well.
    എങ്കിൽ (vimvars[idx].vv_flags & VV_RO)
    {
	semsg(_(e_cannot_change_readonly_variable_str), vimvars[idx].vv_name);
	തിരികെ FAIL;
    }
    എങ്കിൽ (sandbox && (vimvars[idx].vv_flags & VV_RO_SBX))
    {
	semsg(_(e_cannot_set_variable_in_sandbox_str), vimvars[idx].vv_name);
	തിരികെ FAIL;
    }
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_di.di_tv = *tv;
    തിരികെ OK;
}

/*
 * Get number v: variable value.
 */
    varnumber_T
get_vim_var_nr(പൂർണ്ണസംഖ്യ idx)
{
    തിരികെ vimvars[idx].vv_nr;
}

/*
 * Get string v: variable value.  Uses a static buffer, can only be used once.
 * If the String variable has never been set, return an empty string.
 * Never returns NULL;
 */
    char_u *
get_vim_var_str(പൂർണ്ണസംഖ്യ idx)
{
    തിരികെ tv_get_string(&vimvars[idx].vv_tv);
}

/*
 * Get List v: variable value.  Caller must take care of reference count when
 * needed.
 */
    list_T *
get_vim_var_list(പൂർണ്ണസംഖ്യ idx)
{
    തിരികെ vimvars[idx].vv_list;
}

/*
 * Get Dict v: variable value.  Caller must take care of reference count when
 * needed.
 */
    dict_T *
get_vim_var_dict(പൂർണ്ണസംഖ്യ idx)
{
    തിരികെ vimvars[idx].vv_dict;
}

/*
 * Set v:char to character "c".
 */
    ശൂന്യം
set_vim_var_char(പൂർണ്ണസംഖ്യ c)
{
    char_u  buf[MB_MAXBYTES + 1];
    size_t  buflen;

    എങ്കിൽ (has_mbyte)
	buflen = (*mb_char2bytes)(c, buf);
    അല്ലെങ്കിൽ
    {
	buf[0] = c;
	buflen = 1;
    }
    buf[buflen] = NUL;

    set_vim_var_string(VV_CHAR, buf, (പൂർണ്ണസംഖ്യ)buflen);
}

/*
 * Set v:count to "count" and v:count1 to "count1".
 * When "set_prevcount" is TRUE first set v:prevcount from v:count.
 */
    ശൂന്യം
set_vcount(
    long	എണ്ണം,
    long	count1,
    പൂർണ്ണസംഖ്യ		set_prevcount)
{
    എങ്കിൽ (set_prevcount)
	vimvars[VV_PREVCOUNT].vv_nr = vimvars[VV_എണ്ണം].vv_nr;
    vimvars[VV_എണ്ണം].vv_nr = എണ്ണം;
    vimvars[VV_COUNT1].vv_nr = count1;
}

/*
 * Save variables that might be changed as a side effect.  Used when executing
 * a timer callback.
 */
    ശൂന്യം
save_vimvars(vimvars_save_T *vvsave)
{
    vvsave->vv_prevcount = vimvars[VV_PREVCOUNT].vv_nr;
    vvsave->vv_എണ്ണം = vimvars[VV_എണ്ണം].vv_nr;
    vvsave->vv_count1 = vimvars[VV_COUNT1].vv_nr;
}

/*
 * Restore variables saved by save_vimvars().
 */
    ശൂന്യം
restore_vimvars(vimvars_save_T *vvsave)
{
    vimvars[VV_PREVCOUNT].vv_nr = vvsave->vv_prevcount;
    vimvars[VV_എണ്ണം].vv_nr = vvsave->vv_എണ്ണം;
    vimvars[VV_COUNT1].vv_nr = vvsave->vv_count1;
}

/*
 * Set string v: variable to a copy of "val". If 'copy' is FALSE, then set the
 * value.
 */
    ശൂന്യം
set_vim_var_string(
    പൂർണ്ണസംഖ്യ		idx,
    char_u	*val,
    പൂർണ്ണസംഖ്യ		len)	    // length of "val" to use or -1 (whole string)
{
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_tv_type = VAR_STRING;
    എങ്കിൽ (val == ശൂന്യ)
	vimvars[idx].vv_str = ശൂന്യ;
    അല്ലെങ്കിൽ എങ്കിൽ (len == -1)
	vimvars[idx].vv_str = vim_strsave(val);
    അല്ലെങ്കിൽ
	vimvars[idx].vv_str = vim_strnsave(val, len);
}

    ശൂന്യം
set_vim_var_string_direct(
    പൂർണ്ണസംഖ്യ		idx,
    char_u	*val)
{
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_tv_type = VAR_STRING;

    vimvars[idx].vv_str = val;
}

/*
 * Set List v: variable to "val".
 */
    ശൂന്യം
set_vim_var_list(പൂർണ്ണസംഖ്യ idx, list_T *val)
{
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_tv_type = VAR_LIST;
    vimvars[idx].vv_list = val;
    എങ്കിൽ (val != ശൂന്യ)
	++val->lv_refcount;
}

/*
 * Set Dictionary v: variable to "val".
 */
    ശൂന്യം
set_vim_var_dict(പൂർണ്ണസംഖ്യ idx, dict_T *val)
{
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_tv_type = VAR_DICT;
    vimvars[idx].vv_dict = val;
    എങ്കിൽ (val == ശൂന്യ)
	തിരികെ;

    ++val->dv_refcount;
    dict_set_ഇനങ്ങൾ_ro(val);
}

/*
 * Set the v:argv list.
 */
    ശൂന്യം
set_argv_var(അക്ഷരം **argv, പൂർണ്ണസംഖ്യ argc)
{
    list_T	*l = list_alloc();
    പൂർണ്ണസംഖ്യ		i;

    എങ്കിൽ (l == ശൂന്യ)
	getout(1);
    l->lv_lock = VAR_FIXED;
    വേണ്ടി (i = 0; i < argc; ++i)
    {
	എങ്കിൽ (list_append_string(l, (char_u *)argv[i], -1) == FAIL)
	    getout(1);
	l->lv_u.mat.lv_last->li_tv.v_lock = VAR_FIXED;
    }
    set_vim_var_list(VV_ARGV, l);
}

/*
 * Reset v:register, taking the 'clipboard' setting into account.
 */
    ശൂന്യം
reset_reg_var(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ regname = 0;

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
    ശൂന്യം
set_reg_var(പൂർണ്ണസംഖ്യ c)
{
    char_u  regname[2];

    എങ്കിൽ (c == 0 || c == ' ')
	regname[0] = '"';
    അല്ലെങ്കിൽ
	regname[0] = c;
    regname[1] = NUL;
    // Avoid free/alloc when the value is already right.
    എങ്കിൽ (vimvars[VV_REG].vv_str == ശൂന്യ || vimvars[VV_REG].vv_str[0] != c)
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
    എങ്കിൽ (oldval == ശൂന്യ)
	തിരികെ vimvars[VV_EXCEPTION].vv_str;

    vimvars[VV_EXCEPTION].vv_str = oldval;
    തിരികെ ശൂന്യ;
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
    എങ്കിൽ (oldval == ശൂന്യ)
	തിരികെ vimvars[VV_THROWPOINT].vv_str;

    vimvars[VV_THROWPOINT].vv_str = oldval;
    തിരികെ ശൂന്യ;
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
    എങ്കിൽ (eap == ശൂന്യ)
    {
	vim_free(oldval);
	vimvars[VV_CMDARG].vv_str = oldarg;
	തിരികെ ശൂന്യ;
    }

    എങ്കിൽ (eap->force_bin == FORCE_BIN)
	len = 6;
    അല്ലെങ്കിൽ എങ്കിൽ (eap->force_bin == FORCE_NOBIN)
	len = 8;
    അല്ലെങ്കിൽ
	len = 0;

    എങ്കിൽ (eap->read_edit)
	len += 7;

    എങ്കിൽ (eap->force_ff != 0)
	len += 10; // " ++ff=unix"
    എങ്കിൽ (eap->force_enc != 0)
	len += (unsigned)STRLEN(eap->cmd + eap->force_enc) + 7;
    എങ്കിൽ (eap->bad_char != 0)
	len += 7 + 4;  // " ++bad=" + "keep" or "drop"

    newval = alloc(len + 1);
    എങ്കിൽ (newval == ശൂന്യ)
	തിരികെ ശൂന്യ;

    എങ്കിൽ (eap->force_bin == FORCE_BIN)
	sprintf((അക്ഷരം *)newval, " ++bin");
    അല്ലെങ്കിൽ എങ്കിൽ (eap->force_bin == FORCE_NOBIN)
	sprintf((അക്ഷരം *)newval, " ++nobin");
    അല്ലെങ്കിൽ
	*newval = NUL;

    എങ്കിൽ (eap->read_edit)
	STRCAT(newval, " ++edit");

    എങ്കിൽ (eap->force_ff != 0)
	sprintf((അക്ഷരം *)newval + STRLEN(newval), " ++ff=%s",
						eap->force_ff == 'u' ? "unix"
						: eap->force_ff == 'd' ? "dos"
						: "mac");
    എങ്കിൽ (eap->force_enc != 0)
	sprintf((അക്ഷരം *)newval + STRLEN(newval), " ++enc=%s",
					       eap->cmd + eap->force_enc);
    എങ്കിൽ (eap->bad_char == BAD_KEEP)
	STRCPY(newval + STRLEN(newval), " ++bad=keep");
    അല്ലെങ്കിൽ എങ്കിൽ (eap->bad_char == BAD_DROP)
	STRCPY(newval + STRLEN(newval), " ++bad=drop");
    അല്ലെങ്കിൽ എങ്കിൽ (eap->bad_char != 0)
	sprintf((അക്ഷരം *)newval + STRLEN(newval), " ++bad=%c", eap->bad_char);
    vimvars[VV_CMDARG].vv_str = newval;
    തിരികെ oldval;
}

/*
 * Get the value of internal variable "name".
 * If "flags" has EVAL_VAR_IMPORT may return a VAR_ANY with v_number set to the
 * imported script ID.
 * Return OK or FAIL.  If OK is returned "rettv" must be cleared.
 */
    പൂർണ്ണസംഖ്യ
eval_variable(
    char_u	*name,
    പൂർണ്ണസംഖ്യ		len,		// length of "name" or zero
    scid_T	sid,		// script ID for imported item or zero
    typval_T	*rettv,		// NULL when only checking existence
    dictitem_T	**dip,		// non-NULL when typval's dict item is needed
    പൂർണ്ണസംഖ്യ		flags)		// EVAL_VAR_ flags
{
    പൂർണ്ണസംഖ്യ		ret = OK;
    typval_T	*tv = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		found = FALSE;
    hashtab_T	*ht = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		cc = 0;
    type_T	*type = ശൂന്യ;

    എങ്കിൽ (len > 0)
    {
	// truncate the name, so that we can use strcmp()
	cc = name[len];
	name[len] = NUL;
    }

    // Check for local variable when debugging.
    എങ്കിൽ ((sid == 0) && (tv = lookup_debug_var(name)) == ശൂന്യ)
    {
	// Check for user-defined variables.
	dictitem_T	*v = find_var(name, &ht, flags & EVAL_VAR_NOAUTOLOAD);

	എങ്കിൽ (v != ശൂന്യ)
	{
	    tv = &v->di_tv;
	    എങ്കിൽ (dip != ശൂന്യ)
		*dip = v;
	}
	അല്ലെങ്കിൽ
	    ht = ശൂന്യ;
    }

    എങ്കിൽ (tv == ശൂന്യ && (in_vim9script() || STRNCMP(name, "s:", 2) == 0))
    {
	imported_T  *import = ശൂന്യ;
	char_u	    *p = STRNCMP(name, "s:", 2) == 0 ? name + 2 : name;

	എങ്കിൽ (sid == 0)
	    import = find_imported(p, 0, TRUE);

	// imported variable from another script
	എങ്കിൽ (import != ശൂന്യ || sid != 0)
	{
	    എങ്കിൽ ((flags & EVAL_VAR_IMPORT) == 0)
	    {
		എങ്കിൽ (SCRIPT_ID_VALID(sid))
		{
		    ht = &SCRIPT_VARS(sid);
		    എങ്കിൽ (ht != ശൂന്യ)
		    {
			dictitem_T *v = find_var_in_ht(ht, 0, name,
						  flags & EVAL_VAR_NOAUTOLOAD);

			എങ്കിൽ (v == ശൂന്യ)
			    v = find_var_autoload_prefix(name, sid, ശൂന്യ, ശൂന്യ);

			എങ്കിൽ (v != ശൂന്യ)
			{
			    tv = &v->di_tv;
			    എങ്കിൽ (dip != ശൂന്യ)
				*dip = v;
			}
			അല്ലെങ്കിൽ
			    ht = ശൂന്യ;
		    }
		}
		അല്ലെങ്കിൽ
		{
		    എങ്കിൽ (flags & EVAL_VAR_VERBOSE)
			semsg(_(e_expected_dot_after_name_str), name);
		    ret = FAIL;
		}
	    }
	    അല്ലെങ്കിൽ
	    {
		എങ്കിൽ (rettv != ശൂന്യ)
		{
		    // special value that is used in handle_subscript()
		    rettv->v_type = VAR_ANY;
		    rettv->vval.v_സംഖ്യ = sid != 0 ? sid : import->imp_sid;
		}
		found = TRUE;
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (in_vim9script() && (flags & EVAL_VAR_NO_FUNC) == 0)
	{
	    പൂർണ്ണസംഖ്യ	    has_g_prefix = STRNCMP(name, "g:", 2) == 0;
	    ufunc_T *ufunc = find_func(name, FALSE);

	    എങ്കിൽ (ufunc != ശൂന്യ && cc == '<')
	    {
		// handle generic function
		char_u	*argp = name + len;
		name[len] = cc;
		ufunc = eval_generic_func(ufunc, name, &argp);
		name[len] = NUL;
		എങ്കിൽ (ufunc == ശൂന്യ)
		{
		    ret = FAIL;
		    പോകുക done;
		}
	    }

	    // In Vim9 script we can get a function reference by using the
	    // function name.  For a global non-autoload function "g:" is
	    // required.
	    എങ്കിൽ (ufunc != ശൂന്യ && (has_g_prefix
					    || !func_requires_g_prefix(ufunc)))
	    {
		found = TRUE;
		എങ്കിൽ (rettv != ശൂന്യ)
		{
		    rettv->v_type = VAR_FUNC;
		    എങ്കിൽ (has_g_prefix)
		    {
			// Keep the "g:", otherwise script-local may be
			// assumed.
			എങ്കിൽ (cc != '<')
			    rettv->vval.v_string = vim_strsave(name);
			അല്ലെങ്കിൽ
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
		    അല്ലെങ്കിൽ
			rettv->vval.v_string = vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
		    എങ്കിൽ (rettv->vval.v_string != ശൂന്യ)
			func_ref(ufunc->uf_name);
		}
	    }
	}
    }

    എങ്കിൽ (!found)
    {
	എങ്കിൽ (tv == ശൂന്യ)
	{
	    എങ്കിൽ (rettv != ശൂന്യ && (flags & EVAL_VAR_VERBOSE))
		semsg(_(e_undefined_variable_str), name);
	    ret = FAIL;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (rettv != ശൂന്യ)
	{
	    svar_T  *sv = ശൂന്യ;
	    പൂർണ്ണസംഖ്യ	    was_assigned = FALSE;

	    എങ്കിൽ (ht != ശൂന്യ && ht == get_script_local_ht()
		    && tv != &SCRIPT_SV(current_sctx.sc_sid)->sv_var.di_tv)
	    {
		sv = find_typval_in_script(tv, 0, TRUE);
		എങ്കിൽ (sv != ശൂന്യ)
		{
		    type = sv->sv_type;
		    was_assigned = sv->sv_flags & SVFLAG_ASSIGNED;
		}
	    }

	    എങ്കിൽ ((tv->v_type == VAR_TYPEALIAS || tv->v_type == VAR_CLASS)
		    && sid != 0)
	    {
		// type alias or class imported from another script.  Check
		// whether it is exported from the other script.
		sv = find_typval_in_script(tv, sid, TRUE);
		എങ്കിൽ (sv == ശൂന്യ)
		{
		    ret = FAIL;
		    പോകുക done;
		}
		എങ്കിൽ ((sv->sv_flags & SVFLAG_EXPORTED) == 0)
		{
		    semsg(_(e_item_not_exported_in_script_str), name);
		    ret = FAIL;
		    പോകുക done;
		}
	    }

	    // If a list or tuple or dict variable wasn't initialized and has
	    // meaningful type, do it now.  Not for global variables, they are
	    // not declared.
	    എങ്കിൽ (ht != &globvarht)
	    {
		എങ്കിൽ (tv->v_type == VAR_DICT && tv->vval.v_dict == ശൂന്യ
					    && ((type != ശൂന്യ && !was_assigned)
							  || !in_vim9script()))
		{
		    tv->vval.v_dict = dict_alloc();
		    എങ്കിൽ (tv->vval.v_dict != ശൂന്യ)
		    {
			++tv->vval.v_dict->dv_refcount;
			tv->vval.v_dict->dv_type = alloc_type(type);
			എങ്കിൽ (sv != ശൂന്യ)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
		അല്ലെങ്കിൽ എങ്കിൽ (tv->v_type == VAR_LIST && tv->vval.v_list == ശൂന്യ
					    && ((type != ശൂന്യ && !was_assigned)
							  || !in_vim9script()))
		{
		    tv->vval.v_list = list_alloc();
		    എങ്കിൽ (tv->vval.v_list != ശൂന്യ)
		    {
			++tv->vval.v_list->lv_refcount;
			tv->vval.v_list->lv_type = alloc_type(type);
			എങ്കിൽ (sv != ശൂന്യ)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
		അല്ലെങ്കിൽ എങ്കിൽ (tv->v_type == VAR_TUPLE && tv->vval.v_tuple == ശൂന്യ
					    && ((type != ശൂന്യ && !was_assigned)
							  || !in_vim9script()))
		{
		    tv->vval.v_tuple = tuple_alloc();
		    എങ്കിൽ (tv->vval.v_tuple != ശൂന്യ)
		    {
			++tv->vval.v_tuple->tv_refcount;
			tv->vval.v_tuple->tv_type = alloc_type(type);
			എങ്കിൽ (sv != ശൂന്യ)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
		അല്ലെങ്കിൽ എങ്കിൽ (tv->v_type == VAR_BLOB && tv->vval.v_blob == ശൂന്യ
					    && ((type != ശൂന്യ && !was_assigned)
							  || !in_vim9script()))
		{
		    tv->vval.v_blob = blob_alloc();
		    എങ്കിൽ (tv->vval.v_blob != ശൂന്യ)
		    {
			++tv->vval.v_blob->bv_refcount;
			എങ്കിൽ (sv != ശൂന്യ)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
	    }
	    ret = copy_tv(tv, rettv);
	}
    }

done:
    എങ്കിൽ (len > 0)
	name[len] = cc;

    തിരികെ ret;
}

/*
 * Get the value of internal variable "name", also handling "import.name".
 * Return OK or FAIL.  If OK is returned "rettv" must be cleared.
 */
    പൂർണ്ണസംഖ്യ
eval_variable_import(
    char_u	*name,
    typval_T	*rettv)
{
    char_u  *s = name;
    ആകുമ്പോൾ (ASCII_ISALNUM(*s) || *s == '_')
	++s;
    പൂർണ്ണസംഖ്യ	    len = (പൂർണ്ണസംഖ്യ)(s - name);

    എങ്കിൽ (eval_variable(name, len, 0, rettv, ശൂന്യ, EVAL_VAR_IMPORT) == FAIL)
	തിരികെ FAIL;
    എങ്കിൽ (rettv->v_type == VAR_ANY && *s == '.')
    {
	char_u *ns = s + 1;
	s = ns;
	ആകുമ്പോൾ (ASCII_ISALNUM(*s) || *s == '_')
	    ++s;
	പൂർണ്ണസംഖ്യ sid = rettv->vval.v_സംഖ്യ;
	തിരികെ eval_variable(ns, (പൂർണ്ണസംഖ്യ)(s - ns), sid, rettv, ശൂന്യ, 0);
    }
    തിരികെ OK;
}


/*
 * Check if variable "name[len]" is a local variable or an argument.
 * If so, "*eval_lavars_used" is set to TRUE.
 */
    ശൂന്യം
check_vars(char_u *name, പൂർണ്ണസംഖ്യ len)
{
    പൂർണ്ണസംഖ്യ		cc;
    char_u	*varname;
    hashtab_T	*ht;

    എങ്കിൽ (eval_lavars_used == ശൂന്യ)
	തിരികെ;

    // truncate the name, so that we can use strcmp()
    cc = name[len];
    name[len] = NUL;

    ht = find_var_ht(name, &varname);
    എങ്കിൽ (ht == get_funccal_local_ht() || ht == get_funccal_args_ht())
    {
	എങ്കിൽ (find_var(name, ശൂന്യ, TRUE) != ശൂന്യ)
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
find_var(char_u *name, hashtab_T **htp, പൂർണ്ണസംഖ്യ no_autoload)
{
    char_u	*varname;
    hashtab_T	*ht;
    dictitem_T	*ret = ശൂന്യ;

    ht = find_var_ht(name, &varname);
    എങ്കിൽ (htp != ശൂന്യ)
	*htp = ht;
    എങ്കിൽ (ht == ശൂന്യ)
	തിരികെ ശൂന്യ;
    ret = find_var_in_ht(ht, *name, varname, no_autoload);
    എങ്കിൽ (ret != ശൂന്യ)
	തിരികെ ret;

    // Search in parent scope for lambda
    ret = find_var_in_scoped_ht(name, no_autoload);
    എങ്കിൽ (ret != ശൂന്യ)
	തിരികെ ret;

    // in Vim9 script items without a scope can be script-local
    എങ്കിൽ (in_vim9script() && name[0] != NUL && name[1] != ':')
    {
	ht = get_script_local_ht();
	എങ്കിൽ (ht != ശൂന്യ)
	{
	    ret = find_var_in_ht(ht, *name, varname, no_autoload);
	    എങ്കിൽ (ret != ശൂന്യ)
	    {
		എങ്കിൽ (htp != ശൂന്യ)
		    *htp = ht;
		തിരികെ ret;
	    }
	}
    }

    // and finally try
    തിരികെ find_var_autoload_prefix(name, 0, htp, ശൂന്യ);
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
find_var_autoload_prefix(char_u *name, പൂർണ്ണസംഖ്യ sid, hashtab_T **htp,
							    char_u **namep)
{
    hashtab_T	*ht;
    dictitem_T	*ret = ശൂന്യ;
    // When using "vim9script autoload" script-local items are prefixed but can
    // be used with s:name.
    പൂർണ്ണസംഖ്യ check_sid = sid > 0 ? sid : current_sctx.sc_sid;
    എങ്കിൽ (SCRIPT_ID_VALID(check_sid)
		   && (in_vim9script() || (name[0] == 's' && name[1] == ':')))
    {
	scriptitem_T *si = SCRIPT_ITEM(check_sid);

	എങ്കിൽ (si->sn_autoload_prefix != ശൂന്യ)
	{
	    char_u *base_name = (name[0] == 's' && name[1] == ':')
							     ? name + 2 : name;
	    char_u *auto_name = concat_str(si->sn_autoload_prefix, base_name);

	    എങ്കിൽ (auto_name != ശൂന്യ)
	    {
		പൂർണ്ണസംഖ്യ free_auto_name = TRUE;
		ht = &globvarht;
		ret = find_var_in_ht(ht, 'g', auto_name, TRUE);
		എങ്കിൽ (ret != ശൂന്യ)
		{
		    എങ്കിൽ (htp != ശൂന്യ)
			*htp = ht;
		    എങ്കിൽ (namep != ശൂന്യ)
		    {
			free_auto_name = FALSE;
			*namep = auto_name;
		    }
		}
		എങ്കിൽ (free_auto_name)
		    vim_free(auto_name);
	    }
	}
    }

    തിരികെ ret;
}

/*
 * Like find_var() but if the name starts with <SNR>99_ then look in the
 * referenced script (used for a funcref).
 */
    dictitem_T *
find_var_also_in_script(char_u *name, hashtab_T **htp, പൂർണ്ണസംഖ്യ no_autoload)
{
    എങ്കിൽ (STRNCMP(name, "<SNR>", 5) == 0 && SAFE_isdigit(name[5]))
    {
	char_u	    *p = name + 5;
	പൂർണ്ണസംഖ്യ	    sid = getdigits(&p);

	എങ്കിൽ (SCRIPT_ID_VALID(sid) && *p == '_')
	{
	    hashtab_T	*ht = &SCRIPT_VARS(sid);

	    എങ്കിൽ (ht != ശൂന്യ)
	    {
		dictitem_T *di = find_var_in_ht(ht, 0, p + 1, no_autoload);

		എങ്കിൽ (di != ശൂന്യ)
		{
		    എങ്കിൽ (htp != ശൂന്യ)
			*htp = ht;
		    തിരികെ di;
		}
	    }
	}
    }

    തിരികെ find_var(name, htp, no_autoload);
}

/*
 * Find variable "varname" in hashtab "ht" with name "htname".
 * When "varname" is empty returns curwin/curtab/etc vars dictionary.
 * Returns NULL if not found.
 */
    dictitem_T *
find_var_in_ht(
    hashtab_T	*ht,
    പൂർണ്ണസംഖ്യ		htname,
    char_u	*varname,
    പൂർണ്ണസംഖ്യ		no_autoload)
{
    hashitem_T	*hi;

    എങ്കിൽ (*varname == NUL)
    {
	// Must be something like "s:", otherwise "ht" would be NULL.
	തിരഞ്ഞെടുപ്പ് (htname)
	{
	    അവസ്ഥ 's': തിരികെ &SCRIPT_SV(current_sctx.sc_sid)->sv_var;
	    അവസ്ഥ 'g': തിരികെ &globvars_var;
	    അവസ്ഥ 'v': തിരികെ &vimvars_var;
	    അവസ്ഥ 'b': തിരികെ &curbuf->b_bufvar;
	    അവസ്ഥ 'w': തിരികെ &curwin->w_winvar;
	    അവസ്ഥ 't': തിരികെ &curtab->tp_winvar;
	    അവസ്ഥ 'l': തിരികെ get_funccal_local_var();
	    അവസ്ഥ 'a': തിരികെ get_funccal_args_var();
	}
	തിരികെ ശൂന്യ;
    }

    hi = hash_find(ht, varname);
    എങ്കിൽ (HASHITEM_EMPTY(hi))
    {
	// For global variables we may try auto-loading the script.  If it
	// worked find the variable again.  Don't auto-load a script if it was
	// loaded already, otherwise it would be loaded every time when
	// checking if a function name is a Funcref variable.
	എങ്കിൽ (ht == &globvarht && !no_autoload)
	{
	    // Note: script_autoload() may make "hi" invalid. It must either
	    // be obtained again or not used.
	    എങ്കിൽ (!script_autoload(varname, FALSE) || aborting())
		തിരികെ ശൂന്യ;
	    hi = hash_find(ht, varname);
	}
	എങ്കിൽ (HASHITEM_EMPTY(hi))
	    തിരികെ ശൂന്യ;
    }
    തിരികെ HI2DI(hi);
}

/*
 * Get the script-local hashtab.  NULL if not in a script context.
 */
    hashtab_T *
get_script_local_ht(ശൂന്യം)
{
    scid_T sid = current_sctx.sc_sid;

    എങ്കിൽ (SCRIPT_ID_VALID(sid))
	തിരികെ &SCRIPT_VARS(sid);
    തിരികെ ശൂന്യ;
}

/*
 * Look for "name[len]" in script-local variables and functions.
 * When "cmd" is TRUE it must look like a command, a function must be followed
 * by "(" or "->".
 * Return OK when found, FAIL when not found.
 */
    പൂർണ്ണസംഖ്യ
lookup_scriptitem(
	char_u	*name,
	size_t	len,
	പൂർണ്ണസംഖ്യ	cmd,
	cctx_T	*dummy UNUSED)
{
    hashtab_T	*ht = get_script_local_ht();
    char_u	buffer[30];
    char_u	*p;
    പൂർണ്ണസംഖ്യ		res;
    hashitem_T	*hi;
    പൂർണ്ണസംഖ്യ		is_global = FALSE;
    char_u	*fname = name;

    എങ്കിൽ (ht == ശൂന്യ)
	തിരികെ FAIL;
    എങ്കിൽ (len < വലുപ്പം(buffer) - 1)
    {
	// avoid an alloc/free for short names
	vim_strncpy(buffer, name, len);
	p = buffer;
    }
    അല്ലെങ്കിൽ
    {
	p = vim_strnsave(name, len);
	എങ്കിൽ (p == ശൂന്യ)
	    തിരികെ FAIL;
    }

    hi = hash_find(ht, p);
    res = HASHITEM_EMPTY(hi) ? FAIL : OK;

    // if not script-local, then perhaps autoload-exported
    എങ്കിൽ (res == FAIL && find_var_autoload_prefix(p, 0, ശൂന്യ, ശൂന്യ) != ശൂന്യ)
	res = OK;

    // if not script-local or autoload, then perhaps imported
    എങ്കിൽ (res == FAIL && find_imported(p, 0, FALSE) != ശൂന്യ)
	res = OK;
    എങ്കിൽ (p != buffer)
	vim_free(p);

    // Find a function, so that a following "->" works.
    // When used as a command require "(" or "->" to follow, "Cmd" is a user
    // command while "Cmd()" is a function call.
    എങ്കിൽ (res != OK)
    {
	p = skipwhite(name + len);

	എങ്കിൽ (!cmd || name[len] == '(' || (p[0] == '-' && p[1] == '>'))
	{
	    // Do not check for an internal function, since it might also be a
	    // valid command, such as ":split" versus "split()".
	    // Skip "g:" before a function name.
	    എങ്കിൽ (name[0] == 'g' && name[1] == ':')
	    {
		is_global = TRUE;
		fname = name + 2;
	    }
	    എങ്കിൽ (find_func(fname, is_global) != ശൂന്യ)
		res = OK;
	}
    }

    തിരികെ res;
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

    എങ്കിൽ (name[0] == NUL)
	തിരികെ ശൂന്യ;
    എങ്കിൽ (name[1] != ':')
    {
	// The name must not start with a colon or #.
	എങ്കിൽ (name[0] == ':' || name[0] == AUTOLOAD_CHAR)
	    തിരികെ ശൂന്യ;
	*varname = name;

	// "version" is "v:version" in all scopes if scriptversion < 3.
	// Same for a few other variables marked with VV_COMPAT.
	എങ്കിൽ (in_old_script(3))
	{
	    hi = hash_find(&compat_hashtab, name);
	    എങ്കിൽ (!HASHITEM_EMPTY(hi))
		തിരികെ &compat_hashtab;
	}

	ht = get_funccal_local_ht();
	എങ്കിൽ (ht != ശൂന്യ)
	    തിരികെ ht;				// local variable

	// In Vim9 script items at the script level are script-local, except
	// for autoload names.
	എങ്കിൽ (in_vim9script() && vim_strchr(name, AUTOLOAD_CHAR) == ശൂന്യ)
	{
	    ht = get_script_local_ht();
	    എങ്കിൽ (ht != ശൂന്യ)
		തിരികെ ht;
	}

	തിരികെ &globvarht;			// global variable
    }
    *varname = name + 2;
    എങ്കിൽ (*name == 'g')				// global variable
	തിരികെ &globvarht;
    // There must be no ':' or '#' in the rest of the name, unless g: is used
    എങ്കിൽ (vim_strchr(name + 2, ':') != ശൂന്യ
			       || vim_strchr(name + 2, AUTOLOAD_CHAR) != ശൂന്യ)
	തിരികെ ശൂന്യ;
    എങ്കിൽ (*name == 'b')				// buffer variable
	തിരികെ &curbuf->b_vars->dv_hashtab;
    എങ്കിൽ (*name == 'w')				// window variable
	തിരികെ &curwin->w_vars->dv_hashtab;
    എങ്കിൽ (*name == 't')				// tab page variable
	തിരികെ &curtab->tp_vars->dv_hashtab;
    എങ്കിൽ (*name == 'v')				// v: variable
	തിരികെ &vimvarht;
    എങ്കിൽ (get_current_funccal() != ശൂന്യ
	       && get_current_funccal()->fc_func->uf_def_status
							    == UF_NOT_COMPILED)
    {
	// a: and l: are only used in functions defined with ":function"
	എങ്കിൽ (*name == 'a')			// a: function argument
	    തിരികെ get_funccal_args_ht();
	എങ്കിൽ (*name == 'l')			// l: local function variable
	    തിരികെ get_funccal_local_ht();
    }
    എങ്കിൽ (*name == 's')				// script variable
    {
	ht = get_script_local_ht();
	എങ്കിൽ (ht != ശൂന്യ)
	    തിരികെ ht;
    }
    തിരികെ ശൂന്യ;
}

/*
 * Get the string value of a (global/local) variable.
 * Note: see tv_get_string() for how long the pointer remains valid.
 * Returns NULL when it doesn't exist.
 */
    char_u *
get_var_മൂല്യം(char_u *name)
{
    dictitem_T	*v;

    v = find_var(name, ശൂന്യ, FALSE);
    എങ്കിൽ (v == ശൂന്യ)
	തിരികെ ശൂന്യ;
    തിരികെ tv_get_string(&v->di_tv);
}

/*
 * Allocate a new hashtab for a sourced script.  It will be used while
 * sourcing this script and when executing functions defined in the script.
 */
    ശൂന്യം
new_script_vars(scid_T id)
{
    scriptvar_T *sv;

    sv = ALLOC_CLEAR_ONE(scriptvar_T);
    എങ്കിൽ (sv == ശൂന്യ)
	തിരികെ;
    init_var_dict(&sv->sv_dict, &sv->sv_var, VAR_SCOPE);
    SCRIPT_ITEM(id)->sn_vars = sv;
}

/*
 * Initialize dictionary "dict" as a scope and set variable "dict_var" to
 * point to it.
 */
    ശൂന്യം
init_var_dict(dict_T *dict, dictitem_T *dict_var, പൂർണ്ണസംഖ്യ scope)
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
    dict_var->di_താക്കോൽ[0] = NUL;
}

/*
 * Unreference a dictionary initialized by init_var_dict().
 */
    ശൂന്യം
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
    ശൂന്യം
vars_clear(hashtab_T *ht)
{
    vars_clear_ext(ht, TRUE);
}

/*
 * Like vars_clear(), but only free the value if "free_val" is TRUE.
 */
    ശൂന്യം
vars_clear_ext(hashtab_T *ht, പൂർണ്ണസംഖ്യ free_val)
{
    പൂർണ്ണസംഖ്യ		todo;
    hashitem_T	*hi;
    dictitem_T	*v;

    hash_lock(ht);
    todo = (പൂർണ്ണസംഖ്യ)ht->ht_used;
    FOR_ALL_HASHTAB_ഇനങ്ങൾ(ht, hi, todo)
    {
	എങ്കിൽ (!HASHITEM_EMPTY(hi))
	{
	    --todo;

	    // Free the variable.  Don't remove it from the hashtab,
	    // ht_array might change then.  hash_clear() takes care of it
	    // later.
	    v = HI2DI(hi);
	    എങ്കിൽ (free_val)
		clear_tv(&v->di_tv);
	    എങ്കിൽ (v->di_flags & DI_FLAGS_ALLOC)
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
    ശൂന്യം
delete_var(hashtab_T *ht, hashitem_T *hi)
{
    dictitem_T	*di = HI2DI(hi);

    എങ്കിൽ (hash_remove(ht, hi, "delete variable") != OK)
	തിരികെ;

    clear_tv(&di->di_tv);
    vim_free(di);
}

/*
 * Delete the exported variables of a reloaded Vim9 autoload script.  They live
 * in the global namespace with the autoload prefix (e.g. "foo#bar") and are
 * recreated when the script body runs again.
 */
    ശൂന്യം
delete_autoload_export_vars(char_u *prefix)
{
    എങ്കിൽ (prefix == ശൂന്യ)
	തിരികെ;
    size_t  prefixlen = STRLEN(prefix);

    hash_lock(&globvarht);
    പൂർണ്ണസംഖ്യ	todo = (പൂർണ്ണസംഖ്യ)globvarht.ht_used;

    വേണ്ടി (hashitem_T *hi = globvarht.ht_array; todo > 0; ++hi)
	എങ്കിൽ (!HASHITEM_EMPTY(hi))
	{
	    dictitem_T	*di = HI2DI(hi);

	    --todo;
	    // Keep a class or enum: existing objects still refer to it.
	    എങ്കിൽ (di->di_tv.v_type != VAR_CLASS
		    && STRNCMP(di->di_താക്കോൽ, prefix, prefixlen) == 0)
		delete_var(&globvarht, hi);
	}
    hash_unlock(&globvarht);
}

/*
 * List the value of one internal variable.
 */
    സ്ഥിതം ശൂന്യം
list_one_var(dictitem_T *v, അക്ഷരം *prefix, പൂർണ്ണസംഖ്യ *first)
{
    char_u	*tofree;
    char_u	*s;
    char_u	numbuf[NUMBUFLEN];

    s = echo_string(&v->di_tv, &tofree, numbuf, get_copyID());
    list_one_var_a(prefix, v->di_താക്കോൽ, v->di_tv.v_type,
					 s == ശൂന്യ ? (char_u *)"" : s, first);
    vim_free(tofree);
}

    സ്ഥിതം ശൂന്യം
list_one_var_a(
    അക്ഷരം	*prefix,
    char_u	*name,
    പൂർണ്ണസംഖ്യ		type,
    char_u	*string,
    പൂർണ്ണസംഖ്യ		*first)  // when TRUE clear rest of screen and set to FALSE
{
    // don't use msg() or msg_attr() to avoid overwriting "v:statusmsg"
    സന്ദേശം_start();
    സന്ദേശം_puts(prefix);
    എങ്കിൽ (name != ശൂന്യ)	// "a:" vars don't have a name stored
	സന്ദേശം_puts((അക്ഷരം *)name);
    സന്ദേശം_putchar(' ');
    സന്ദേശം_advance(22);
    എങ്കിൽ (type == VAR_സംഖ്യ)
	സന്ദേശം_putchar('#');
    അല്ലെങ്കിൽ എങ്കിൽ (type == VAR_FUNC || type == VAR_PARTIAL)
	സന്ദേശം_putchar('*');
    അല്ലെങ്കിൽ എങ്കിൽ (type == VAR_LIST)
    {
	സന്ദേശം_putchar('[');
	എങ്കിൽ (*string == '[')
	    ++string;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (type == VAR_DICT)
    {
	സന്ദേശം_putchar('{');
	എങ്കിൽ (*string == '{')
	    ++string;
    }
    അല്ലെങ്കിൽ
	സന്ദേശം_putchar(' ');

    സന്ദേശം_outtrans(string);

    എങ്കിൽ (type == VAR_FUNC || type == VAR_PARTIAL)
	സന്ദേശം_puts("()");
    എങ്കിൽ (*first)
    {
	സന്ദേശം_clr_eos();
	*first = FALSE;
    }
}

/*
 * Addition handling for setting a v: variable.
 * Return TRUE if the variable should be set normally,
 *        FALSE if nothing else needs to be done.
 */
    പൂർണ്ണസംഖ്യ
before_set_vvar(
    char_u	*varname,
    dictitem_T	*di,
    typval_T	*tv,
    പൂർണ്ണസംഖ്യ		copy,
    പൂർണ്ണസംഖ്യ		*type_error)
{
    എങ്കിൽ (di->di_tv.v_type == VAR_STRING)
    {
	VIM_CLEAR(di->di_tv.vval.v_string);
	എങ്കിൽ (copy || tv->v_type != VAR_STRING)
	{
	    char_u *val = tv_get_string(tv);

	    // Careful: when assigning to v:errmsg and
	    // tv_get_string() causes an error message the variable
	    // will already be set.
	    എങ്കിൽ (di->di_tv.vval.v_string == ശൂന്യ)
		di->di_tv.vval.v_string = vim_strsave(val);
	}
	അല്ലെങ്കിൽ
	{
	    // Take over the string to avoid an extra alloc/free.
	    di->di_tv.vval.v_string = tv->vval.v_string;
	    tv->vval.v_string = ശൂന്യ;
	}
	തിരികെ FALSE;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (di->di_tv.v_type == VAR_സംഖ്യ)
    {
	di->di_tv.vval.v_സംഖ്യ = tv_get_സംഖ്യ(tv);
	എങ്കിൽ (STRCMP(varname, "searchforward") == 0)
	    set_തിരയൽ_direction(di->di_tv.vval.v_സംഖ്യ ? '/' : '?');
#ifdef FEAT_തിരയൽ_EXTRA
	അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(varname, "hlsearch") == 0)
	{
	    no_hlsearch = !di->di_tv.vval.v_സംഖ്യ;
	    redraw_all_later(UPD_SOME_VALID);
	}
#endif
	തിരികെ FALSE;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (di->di_tv.v_type != tv->v_type)
    {
	*type_error = TRUE;
	തിരികെ FALSE;
    }
    തിരികെ TRUE;
}

/*
 * Set variable "name" to value in "tv".
 * If the variable already exists, the value is updated.
 * Otherwise the variable is created.
 */
    ശൂന്യം
set_var(
    char_u	*name,
    typval_T	*tv,
    പൂർണ്ണസംഖ്യ		copy)	    // make copy of value in "tv"
{
    set_var_const(name, 0, ശൂന്യ, tv, copy, ASSIGN_DECL, 0);
}

/*
 * Set variable "name" to value in "tv_arg".
 * When "sid" is non-zero "name" is in the script with this ID.
 * If the variable already exists and "is_const" is FALSE the value is updated.
 * Otherwise the variable is created.
 */
    പൂർണ്ണസംഖ്യ
set_var_const(
    char_u	*name,
    scid_T	sid,
    type_T	*type_arg,
    typval_T	*tv_arg,
    പൂർണ്ണസംഖ്യ		copy,	    // make copy of value in "tv"
    പൂർണ്ണസംഖ്യ		flags_arg,  // ASSIGN_CONST, ASSIGN_FINAL, etc.
    പൂർണ്ണസംഖ്യ		var_idx)    // index for ":let [a, b] = list"
{
    typval_T	*tv = tv_arg;
    type_T	*type = type_arg;
    typval_T	bool_tv;
    dictitem_T	*di;
    typval_T	*dest_tv;
    char_u	*varname;
    char_u	*name_tofree = ശൂന്യ;
    hashtab_T	*ht = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		is_script_local;
    പൂർണ്ണസംഖ്യ		vim9script = in_vim9script();
    പൂർണ്ണസംഖ്യ		var_in_vim9script;
    പൂർണ്ണസംഖ്യ		var_in_autoload = FALSE;
    പൂർണ്ണസംഖ്യ		flags = flags_arg;
    പൂർണ്ണസംഖ്യ		free_tv_arg = !copy;  // free tv_arg if not used
    പൂർണ്ണസംഖ്യ		rc = FAIL;

    എങ്കിൽ (sid != 0)
    {
	varname = ശൂന്യ;
	എങ്കിൽ (SCRIPT_ID_VALID(sid))
	{
	    char_u	*auto_name = ശൂന്യ;
	    എങ്കിൽ (find_var_autoload_prefix(name, sid, &ht, &auto_name) != ശൂന്യ)
	    {
		var_in_autoload = TRUE;
		varname = auto_name;
		name_tofree = varname;
	    }
	    അല്ലെങ്കിൽ
		ht = &SCRIPT_VARS(sid);
	}
	എങ്കിൽ (varname == ശൂന്യ)
	    varname = name;
    }
    അല്ലെങ്കിൽ
    {
	scriptitem_T	*si;
	char_u		*auto_name = ശൂന്യ;

	എങ്കിൽ (in_vim9script()
	    && SCRIPT_ID_VALID(current_sctx.sc_sid)
	    && (si = SCRIPT_ITEM(current_sctx.sc_sid))
					      ->sn_autoload_prefix != ശൂന്യ
	    && (is_export
		|| find_var_autoload_prefix(name, 0, ശൂന്യ, &auto_name)
								    != ശൂന്യ))
	{
	    // In a vim9 autoload script an exported variable is put in the
	    // global namespace with the autoload prefix.
	    var_in_autoload = TRUE;
	    varname = auto_name != ശൂന്യ ? auto_name
		      : concat_str(si->sn_autoload_prefix, name);
	    എങ്കിൽ (varname == ശൂന്യ)
		പോകുക failed;
	    name_tofree = varname;
	    ht = &globvarht;
	}
	അല്ലെങ്കിൽ
	    ht = find_var_ht(name, &varname);
    }
    എങ്കിൽ (ht == ശൂന്യ || *varname == NUL)
    {
	semsg(_(e_illegal_variable_name_str), name);
	പോകുക failed;
    }
    is_script_local = ht == get_script_local_ht() || sid != 0
							    || var_in_autoload;

    എങ്കിൽ (vim9script
	    && !is_script_local
	    && (flags & (ASSIGN_NO_DECL | ASSIGN_DECL)) == 0
	    && (flags & (ASSIGN_CONST | ASSIGN_FINAL)) == 0
	    && name[1] == ':')
    {
	vim9_declare_error(name);
	പോകുക failed;
    }
    എങ്കിൽ ((flags & ASSIGN_FOR_LOOP) && is_scoped_variable(name))
	// Do not make g:var, w:var, b:var or t:var final.
	flags &= ~ASSIGN_FINAL;

    var_in_vim9script = is_script_local && current_script_is_vim9();
    എങ്കിൽ (var_in_vim9script && name[0] == '_' && name[1] == NUL)
    {
	// For "[a, _] = list" the underscore is ignored.
	എങ്കിൽ ((flags & ASSIGN_UNPACK) == 0)
	    emsg(_(e_cannot_use_underscore_here));
	പോകുക failed;
    }

    di = find_var_in_ht(ht, 0, varname, TRUE);

    എങ്കിൽ (di == ശൂന്യ && var_in_vim9script)
    {
	imported_T  *import = find_imported(varname, 0, FALSE);

	എങ്കിൽ (import != ശൂന്യ)
	{
	    // imported name space cannot be used
	    എങ്കിൽ ((flags & ASSIGN_NO_DECL) == 0)
	    {
		semsg(_(e_redefining_imported_item_str), name);
		പോകുക failed;
	    }
	    semsg(_(e_cannot_use_str_itself_it_is_imported), name);
	    പോകുക failed;
	}
	എങ്കിൽ (!in_vim9script())
	{
	    semsg(_(e_cannot_create_vim9_script_variable_in_function_str),
									 name);
	    പോകുക failed;
	}
    }

    // Search in parent scope which is possible to reference from lambda
    എങ്കിൽ (di == ശൂന്യ)
	di = find_var_in_scoped_ht(name, TRUE);

    എങ്കിൽ ((tv->v_type == VAR_FUNC || tv->v_type == VAR_PARTIAL)
				  && var_wrong_func_name(name, di == ശൂന്യ))
	പോകുക failed;

    എങ്കിൽ (need_convert_to_bool(type, tv))
    {
	// Destination is a bool and the value is not, but it can be
	// converted.
	CLEAR_FIELD(bool_tv);
	bool_tv.v_type = VAR_BOOL;
	bool_tv.vval.v_സംഖ്യ = tv2bool(tv) ? VVAL_TRUE : VVAL_FALSE;
	tv = &bool_tv;
    }

    എങ്കിൽ (di != ശൂന്യ)
    {
	// Item already exists.  Allowed to replace when reloading.
	എങ്കിൽ ((di->di_flags & DI_FLAGS_RELOAD) == 0)
	{
	    എങ്കിൽ ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					 && (flags & ASSIGN_FOR_LOOP) == 0)
	    {
		emsg(_(e_cannot_modify_existing_variable));
		പോകുക failed;
	    }

	    എങ്കിൽ (is_script_local && vim9script
			  && (flags & (ASSIGN_NO_DECL | ASSIGN_DECL)) == 0)
	    {
		semsg(_(e_redefining_script_item_str), name);
		പോകുക failed;
	    }

	    എങ്കിൽ (check_typval_is_മൂല്യം(&di->di_tv) == FAIL)
		പോകുക failed;

	    // List and Blob types can be modified in-place using the "+="
	    // compound operator.  For other types, this is not allowed.
	    പൂർണ്ണസംഖ്യ type_inplace_modifiable =
		(di->di_tv.v_type == VAR_LIST || di->di_tv.v_type == VAR_BLOB);

	    എങ്കിൽ (var_in_vim9script && (flags & ASSIGN_FOR_LOOP) == 0
		    && ((flags & ASSIGN_COMPOUND_OP) == 0
			|| !type_inplace_modifiable))
	    {
		where_T where = WHERE_INIT;
		svar_T  *sv = find_typval_in_script(&di->di_tv, sid, TRUE);

		എങ്കിൽ (sv != ശൂന്യ)
		{
		    // check the type and adjust to bool if needed
		    എങ്കിൽ (var_idx > 0)
		    {
			where.wt_index = var_idx;
			where.wt_kind = WT_VARIABLE;
		    }
		    എങ്കിൽ (check_script_var_type(sv, tv, name, where) == FAIL)
			പോകുക failed;
		    എങ്കിൽ (type == ശൂന്യ)
			type = sv->sv_type;
		    sv->sv_flags |= SVFLAG_ASSIGNED;
		}
	    }

	    // Modifying a final variable with a List value using the "+="
	    // operator is allowed.  For other types, it is not allowed.
	    എങ്കിൽ ((((flags & ASSIGN_FOR_LOOP) == 0 || (flags & ASSIGN_DECL) == 0)
			&& ((flags & ASSIGN_COMPOUND_OP) == 0
			    || !type_inplace_modifiable))
				 ? var_check_permission(di, name) == FAIL
				 : var_check_ro(di->di_flags, name, FALSE))
		പോകുക failed;
	}
	അല്ലെങ്കിൽ
	{
	    // can only redefine once
	    di->di_flags &= ~DI_FLAGS_RELOAD;

	    // A Vim9 script-local variable is also present in sn_all_vars
	    // and sn_var_vals.  It may set "type" from "tv".
	    എങ്കിൽ (var_in_vim9script || var_in_autoload)
		update_vim9_script_var(FALSE, di,
			var_in_autoload ? name : di->di_താക്കോൽ, flags,
			tv, &type, (flags & ASSIGN_NO_MEMBER_TYPE) == 0);
	}

	// existing variable, need to clear the value

	// In Vim9 script a String v: variable only takes a String.
	എങ്കിൽ (ht == &vimvarht && di->di_tv.v_type == VAR_STRING
							    && in_vim9script())
	{
	    where_T	where = WHERE_INIT;

	    എങ്കിൽ (check_typval_type(&t_string, tv, where) == FAIL)
		പോകുക failed;
	}

	// Handle setting internal v: variables separately where needed to
	// prevent changing the type.
	പൂർണ്ണസംഖ്യ type_error = FALSE;
	എങ്കിൽ (ht == &vimvarht
		&& !before_set_vvar(varname, di, tv, copy, &type_error))
	{
	    എങ്കിൽ (type_error)
		semsg(_(e_setting_v_str_to_മൂല്യം_with_wrong_type), varname);
	    പോകുക failed;
	}

	clear_tv(&di->di_tv);

	എങ്കിൽ ((flags & ASSIGN_UPDATE_BLOCK_ID)
				       && SCRIPT_ID_VALID(current_sctx.sc_sid))
	{
	    scriptitem_T *si = SCRIPT_ITEM(current_sctx.sc_sid);

	    update_script_var_block_id(name, si->sn_current_block_id);
	}
    }
    അല്ലെങ്കിൽ
    {
	// Item not found, check if a function already exists.
	എങ്കിൽ (is_script_local && (flags & (ASSIGN_NO_DECL | ASSIGN_DECL)) == 0
	       && lookup_scriptitem(name, STRLEN(name), FALSE, ശൂന്യ) == OK)
	{
	    semsg(_(e_redefining_script_item_str), name);
	    പോകുക failed;
	}

	// add a new variable
	എങ്കിൽ (var_in_vim9script && (flags & ASSIGN_NO_DECL))
	{
	    semsg(_(e_unknown_variable_str), name);
	    പോകുക failed;
	}

	എങ്കിൽ (check_hashtab_frozen(ht, "add variable"))
	    പോകുക failed;

	// Can't add "v:" or "a:" variable.
	എങ്കിൽ (ht == &vimvarht || ht == get_funccal_args_ht())
	{
	    semsg(_(e_illegal_variable_name_str), name);
	    പോകുക failed;
	}

	// Make sure the variable name is valid.  In Vim9 script an
	// autoload variable must be prefixed with "g:" unless in an
	// autoload script.
	എങ്കിൽ (!valid_varname(varname, -1, !vim9script
			|| STRNCMP(name, "g:", 2) == 0 || var_in_autoload))
	    പോകുക failed;

	di = alloc(offsetof(dictitem_T, di_താക്കോൽ) + STRLEN(varname) + 1);
	എങ്കിൽ (di == ശൂന്യ)
	    പോകുക failed;
	STRCPY(di->di_താക്കോൽ, varname);
	എങ്കിൽ (hash_add(ht, DI2HIKEY(di), "add variable") == FAIL)
	{
	    vim_free(di);
	    പോകുക failed;
	}
	di->di_flags = DI_FLAGS_ALLOC;
	എങ്കിൽ (flags & (ASSIGN_CONST | ASSIGN_FINAL))
	    di->di_flags |= DI_FLAGS_LOCK;

	// A Vim9 script-local variable is also added to sn_all_vars and
	// sn_var_vals. It may set "type" from "tv".
	എങ്കിൽ (var_in_vim9script || var_in_autoload)
	    update_vim9_script_var(TRUE, di,
		    var_in_autoload ? name : di->di_താക്കോൽ, flags,
			  tv, &type, (flags & ASSIGN_NO_MEMBER_TYPE) == 0);
    }

    dest_tv = &di->di_tv;
    എങ്കിൽ (copy || tv->v_type == VAR_സംഖ്യ || tv->v_type == VAR_FLOAT)
	copy_tv(tv, dest_tv);
    അല്ലെങ്കിൽ
    {
	*dest_tv = *tv;
	dest_tv->v_lock = 0;
	init_tv(tv);
    }
    free_tv_arg = FALSE;

    എങ്കിൽ (vim9script && type != ശൂന്യ)
	set_tv_type(dest_tv, type);

    // ":const var = value" locks the value
    // ":final var = value" locks "var"
    എങ്കിൽ (flags & ASSIGN_CONST)
	// Like :lockvar! name: lock the value and what it contains, but only
	// if the reference count is up to one.  That locks only literal
	// values.
	item_lock(dest_tv, DICT_MAXNEST, TRUE, TRUE);

    rc = OK;

failed:
    vim_free(name_tofree);
    എങ്കിൽ (free_tv_arg)
	clear_tv(tv_arg);

    തിരികെ rc;
}

/*
 * Check in this order for backwards compatibility:
 * - Whether the variable is read-only
 * - Whether the variable value is locked
 * - Whether the variable is locked
 * NOTE: "name" is only used for error messages.
 */
    പൂർണ്ണസംഖ്യ
var_check_permission(dictitem_T *di, char_u *name)
{
    എങ്കിൽ (var_check_ro(di->di_flags, name, FALSE)
		    || മൂല്യം_check_lock(di->di_tv.v_lock, name, FALSE)
		    || var_check_lock(di->di_flags, name, FALSE))
	തിരികെ FAIL;
    തിരികെ OK;
}

/*
 * Return TRUE if di_flags "flags" indicates variable "name" is read-only.
 * Also give an error message.
 */
    പൂർണ്ണസംഖ്യ
var_check_ro(പൂർണ്ണസംഖ്യ flags, char_u *name, പൂർണ്ണസംഖ്യ use_gettext)
{
    എങ്കിൽ (flags & DI_FLAGS_RO)
    {
	എങ്കിൽ (name == ശൂന്യ)
	    emsg(_(e_cannot_change_readonly_variable));
	അല്ലെങ്കിൽ
	    semsg(_(e_cannot_change_readonly_variable_str),
				       use_gettext ? (char_u *)_(name) : name);
	തിരികെ TRUE;
    }
    എങ്കിൽ ((flags & DI_FLAGS_RO_SBX) && sandbox)
    {
	എങ്കിൽ (name == ശൂന്യ)
	    emsg(_(e_cannot_set_variable_in_sandbox));
	അല്ലെങ്കിൽ
	    semsg(_(e_cannot_set_variable_in_sandbox_str),
				       use_gettext ? (char_u *)_(name) : name);
	തിരികെ TRUE;
    }
    തിരികെ FALSE;
}

/*
 * Return TRUE if di_flags "flags" indicates variable "name" is locked.
 * Also give an error message.
 */
    പൂർണ്ണസംഖ്യ
var_check_lock(പൂർണ്ണസംഖ്യ flags, char_u *name, പൂർണ്ണസംഖ്യ use_gettext)
{
    എങ്കിൽ (flags & DI_FLAGS_LOCK)
    {
	semsg(_(e_variable_is_locked_str),
				       use_gettext ? (char_u *)_(name) : name);
	തിരികെ TRUE;
    }
    തിരികെ FALSE;
}

/*
 * Return TRUE if di_flags "flags" indicates variable "name" is fixed.
 * Also give an error message.
 */
    പൂർണ്ണസംഖ്യ
var_check_fixed(പൂർണ്ണസംഖ്യ flags, char_u *name, പൂർണ്ണസംഖ്യ use_gettext)
{
    എങ്കിൽ (flags & DI_FLAGS_FIX)
    {
	എങ്കിൽ (name == ശൂന്യ)
	    emsg(_(e_cannot_delete_variable));
	അല്ലെങ്കിൽ
	    semsg(_(e_cannot_delete_variable_str),
				      use_gettext ? (char_u *)_(name) : name);
	തിരികെ TRUE;
    }
    തിരികെ FALSE;
}

/*
 * Check if a funcref is assigned to a valid variable name.
 * Return TRUE and give an error if not.
 */
    പൂർണ്ണസംഖ്യ
var_wrong_func_name(
    char_u *name,    // points to start of variable name
    പൂർണ്ണസംഖ്യ    new_var)  // TRUE when creating the variable
{
    // Allow for w: b: s: and t:.  In Vim9 script s: is not allowed, because
    // the name can be used without the s: prefix.
    // Allow autoload variable.
    എങ്കിൽ (!((vim_strchr((char_u *)"wbt", name[0]) != ശൂന്യ
		    || (!in_vim9script() && name[0] == 's')) && name[1] == ':')
	    && !ASCII_ISUPPER((name[0] != NUL && name[1] == ':')
						     ? name[2] : name[0])
	    && vim_strchr(name, '#') == ശൂന്യ)
    {
	semsg(_(e_funcref_variable_name_must_start_with_capital_str), name);
	തിരികെ TRUE;
    }
    // Don't allow hiding a function.  When "v" is not NULL we might be
    // assigning another function to the same var, the type is checked
    // below.
    എങ്കിൽ (new_var && function_exists(name, FALSE))
    {
	semsg(_(e_variable_name_conflicts_with_existing_function_str),
								    name);
	തിരികെ TRUE;
    }
    തിരികെ FALSE;
}

/*
 * Return TRUE if "flags" indicates variable "name" has a locked (immutable)
 * value.  Also give an error message, using "name" or _("name") when
 * "use_gettext" is TRUE.
 */
    പൂർണ്ണസംഖ്യ
മൂല്യം_check_lock(പൂർണ്ണസംഖ്യ lock, char_u *name, പൂർണ്ണസംഖ്യ use_gettext)
{
    എങ്കിൽ (lock & VAR_LOCKED)
    {
	എങ്കിൽ (name == ശൂന്യ)
	    emsg(_(e_മൂല്യം_is_locked));
	അല്ലെങ്കിൽ
	    semsg(_(e_മൂല്യം_is_locked_str),
				       use_gettext ? (char_u *)_(name) : name);
	തിരികെ TRUE;
    }
    എങ്കിൽ (lock & VAR_FIXED)
    {
	എങ്കിൽ (name == ശൂന്യ)
	    emsg(_(e_cannot_change_മൂല്യം));
	അല്ലെങ്കിൽ
	    semsg(_(e_cannot_change_മൂല്യം_of_str),
				       use_gettext ? (char_u *)_(name) : name);
	തിരികെ TRUE;
    }
    തിരികെ FALSE;
}

/*
 * Check if a variable name is valid.  When "autoload" is true "#" is allowed.
 * If "len" is -1 use all of "varname", otherwise up to "varname[len]".
 * Return FALSE and give an error if not.
 */
    പൂർണ്ണസംഖ്യ
valid_varname(char_u *varname, പൂർണ്ണസംഖ്യ len, പൂർണ്ണസംഖ്യ autoload)
{
    char_u *p;

    വേണ്ടി (p = varname; len < 0 ? *p != NUL : p < varname + len; ++p)
	എങ്കിൽ (!eval_isnamec1(*p) && (p == varname || !VIM_ISDIGIT(*p))
					 && !(autoload && *p == AUTOLOAD_CHAR))
	{
	    semsg(_(e_illegal_variable_name_str), varname);
	    തിരികെ FALSE;
	}
    തിരികെ TRUE;
}

/*
 * Implements the logic to retrieve local variable and option values.
 * Used by "getwinvar()" "gettabvar()" "gettabwinvar()" "getbufvar()".
 */
    സ്ഥിതം ശൂന്യം
get_var_from(
    char_u	*varname,
    typval_T	*rettv,
    typval_T	*deftv,	    // Default value if not found.
    പൂർണ്ണസംഖ്യ		htname,	    // 't'ab, 'w'indow or 'b'uffer local.
    tabpage_T	*tp,	    // can be NULL
    win_T	*win,
    buf_T	*buf)	    // Ignored if htname is not 'b'.
{
    dictitem_T	*v;
    പൂർണ്ണസംഖ്യ		done = FALSE;
    switchwin_T	switchwin;
    പൂർണ്ണസംഖ്യ		need_switch_win;
    പൂർണ്ണസംഖ്യ		do_change_curbuf = buf != ശൂന്യ && htname == 'b';

    ++emsg_off;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ശൂന്യ;

    എങ്കിൽ (varname != ശൂന്യ && tp != ശൂന്യ && win != ശൂന്യ
	    && (htname != 'b' || buf != ശൂന്യ))
    {
	// Set curwin to be our win, temporarily.  Also set the tabpage,
	// otherwise the window is not valid. Only do this when needed,
	// autocommands get blocked.
	// If we have a buffer reference avoid the switching, we're saving and
	// restoring curbuf directly.
	need_switch_win = !(tp == curtab && win == curwin) && !do_change_curbuf;
	എങ്കിൽ (!need_switch_win || switch_win(&switchwin, win, tp, TRUE) == OK)
	{
	    // Handle options. There are no tab-local options.
	    എങ്കിൽ (*varname == '&' && htname != 't')
	    {
		buf_T	*save_curbuf = curbuf;

		// Change curbuf so the option is read from the correct buffer.
		എങ്കിൽ (do_change_curbuf)
		    curbuf = buf;

		എങ്കിൽ (varname[1] == NUL)
		{
		    // get all window-local or buffer-local options in a dict
		    dict_T	*opts = get_winbuf_options(htname == 'b');

		    എങ്കിൽ (opts != ശൂന്യ)
		    {
			rettv_dict_set(rettv, opts);
			done = TRUE;
		    }
		}
		അല്ലെങ്കിൽ എങ്കിൽ (eval_option(&varname, rettv, TRUE) == OK)
		    // Local option
		    done = TRUE;

		curbuf = save_curbuf;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (*varname == NUL)
	    {
		// Empty string: return a dict with all the local variables.
		എങ്കിൽ (htname == 'b')
		    v = &buf->b_bufvar;
		അല്ലെങ്കിൽ എങ്കിൽ (htname == 'w')
		    v = &win->w_winvar;
		അല്ലെങ്കിൽ
		    v = &tp->tp_winvar;
		copy_tv(&v->di_tv, rettv);
		done = TRUE;
	    }
	    അല്ലെങ്കിൽ
	    {
		hashtab_T	*ht;

		എങ്കിൽ (htname == 'b')
		    ht = &buf->b_vars->dv_hashtab;
		അല്ലെങ്കിൽ എങ്കിൽ (htname == 'w')
		    ht = &win->w_vars->dv_hashtab;
		അല്ലെങ്കിൽ
		    ht = &tp->tp_vars->dv_hashtab;

		// Look up the variable.
		v = find_var_in_ht(ht, htname, varname, FALSE);
		എങ്കിൽ (v != ശൂന്യ)
		{
		    copy_tv(&v->di_tv, rettv);
		    done = TRUE;
		}
	    }
	}

	എങ്കിൽ (need_switch_win)
	    // restore previous notion of curwin
	    restore_win(&switchwin, TRUE);
    }

    എങ്കിൽ (!done && deftv->v_type != VAR_UNKNOWN)
	// use the default value
	copy_tv(deftv, rettv);

    --emsg_off;
}

/*
 * getwinvar() and gettabwinvar()
 */
    സ്ഥിതം ശൂന്യം
getwinvar(
    typval_T	*argvars,
    typval_T	*rettv,
    പൂർണ്ണസംഖ്യ		off)	    // 1 for gettabwinvar()
{
    char_u	*varname;
    tabpage_T	*tp;
    win_T	*win;

    എങ്കിൽ (off == 1)
	tp = find_tabpage((പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ_chk(&argvars[0], ശൂന്യ));
    അല്ലെങ്കിൽ
	tp = curtab;
    win = find_win_by_nr(&argvars[off], tp);
    varname = tv_get_string_chk(&argvars[off + 1]);

    get_var_from(varname, rettv, &argvars[off + 2], 'w', tp, win, ശൂന്യ);
}

/*
 * Set option "varname" to the value of "varp" for the current buffer/window.
 */
    സ്ഥിതം ശൂന്യം
set_option_from_tv(char_u *varname, typval_T *varp)
{
    long	numval = 0;
    char_u	*strval;
    char_u	nbuf[NUMBUFLEN];
    പൂർണ്ണസംഖ്യ		error = FALSE;

    പൂർണ്ണസംഖ്യ		opt_idx = findoption(varname);
    എങ്കിൽ (opt_idx < 0)
    {
	semsg(_(e_unknown_option_str_2), varname);
	തിരികെ;
    }
    പൂർണ്ണസംഖ്യ		opt_p_flags = get_option_flags(opt_idx);

    എങ്കിൽ (varp->v_type == VAR_BOOL)
    {
	എങ്കിൽ (opt_p_flags & P_STRING)
	{
	    emsg(_(e_string_required));
	    തിരികെ;
	}
	numval = (long)varp->vval.v_സംഖ്യ;
	strval = (char_u *)"0";  // avoid using "false"
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ ((opt_p_flags & (P_അക്കം|P_BOOL))
		&& (!in_vim9script() || varp->v_type != VAR_STRING))
	    numval = (long)tv_get_സംഖ്യ_chk(varp, &error);
	എങ്കിൽ (!error)
	    strval = tv_get_string_buf_chk(varp, nbuf);
    }
    എങ്കിൽ (!error && strval != ശൂന്യ)
	set_option_മൂല്യം_give_err(varname, numval, strval, OPT_LOCAL);
}

/*
 * "setwinvar()" and "settabwinvar()" functions
 */
    സ്ഥിതം ശൂന്യം
setwinvar(typval_T *argvars, പൂർണ്ണസംഖ്യ off)
{
    win_T	*win;
    switchwin_T	switchwin;
    പൂർണ്ണസംഖ്യ		need_switch_win;
    char_u	*varname, *winvarname;
    typval_T	*varp;
    tabpage_T	*tp = ശൂന്യ;

    എങ്കിൽ (check_secure())
	തിരികെ;

    എങ്കിൽ (off == 1)
	tp = find_tabpage((പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ_chk(&argvars[0], ശൂന്യ));
    അല്ലെങ്കിൽ
	tp = curtab;
    win = find_win_by_nr(&argvars[off], tp);
    varname = tv_get_string_chk(&argvars[off + 1]);
    varp = &argvars[off + 2];

    എങ്കിൽ (win == ശൂന്യ || varname == ശൂന്യ)
	തിരികെ;

    need_switch_win = !(tp == curtab && win == curwin);
    എങ്കിൽ (!need_switch_win
	    || switch_win(&switchwin, win, tp, TRUE) == OK)
    {
	എങ്കിൽ (*varname == '&')
	    set_option_from_tv(varname + 1, varp);
	അല്ലെങ്കിൽ
	{
	    winvarname = alloc(STRLEN(varname) + 3);
	    എങ്കിൽ (winvarname != ശൂന്യ)
	    {
		STRCPY(winvarname, "w:");
		STRCPY(winvarname + 2, varname);
		set_var(winvarname, varp, TRUE);
		vim_free(winvarname);
	    }
	}
    }
    എങ്കിൽ (need_switch_win)
	restore_win(&switchwin, TRUE);
}

/*
 * reset v:option_new, v:option_old, v:option_oldlocal, v:option_oldglobal,
 * v:option_type, and v:option_command.
 */
    ശൂന്യം
reset_v_option_vars(ശൂന്യം)
{
    set_vim_var_string(VV_OPTION_NEW, ശൂന്യ, -1);
    set_vim_var_string(VV_OPTION_OLD, ശൂന്യ, -1);
    set_vim_var_string(VV_OPTION_OLDLOCAL, ശൂന്യ, -1);
    set_vim_var_string(VV_OPTION_OLDGLOBAL, ശൂന്യ, -1);
    set_vim_var_string(VV_OPTION_TYPE, ശൂന്യ, -1);
    set_vim_var_string(VV_OPTION_COMMAND, ശൂന്യ, -1);
}

/*
 * Add an assert error to v:errors.
 */
    ശൂന്യം
assert_error(garray_T *gap)
{
    ഘടന vimvar   *vp = &vimvars[VV_ERRORS];

    എങ്കിൽ (vp->vv_tv_type != VAR_LIST || vimvars[VV_ERRORS].vv_list == ശൂന്യ)
	// Make sure v:errors is a list.
	set_vim_var_list(VV_ERRORS, list_alloc());
    list_append_string(vimvars[VV_ERRORS].vv_list, gap->ga_data, gap->ga_len);
}

    പൂർണ്ണസംഖ്യ
var_exists(char_u *var)
{
    char_u	*arg = var;
    char_u	*name;
    char_u	*tofree;
    typval_T    tv;
    പൂർണ്ണസംഖ്യ		len = 0;
    പൂർണ്ണസംഖ്യ		n = FALSE;

    // get_name_len() takes care of expanding curly braces
    name = var;
    len = get_name_len(&arg, &tofree, TRUE, FALSE);
    എങ്കിൽ (len > 0)
    {
	എങ്കിൽ (tofree != ശൂന്യ)
	    name = tofree;
	n = (eval_variable(name, len, 0, &tv, ശൂന്യ,
				 EVAL_VAR_NOAUTOLOAD + EVAL_VAR_IMPORT) == OK);
	എങ്കിൽ (n)
	{
	    // handle d.key, l[idx], f(expr)
	    arg = skipwhite(arg);
	    n = (handle_subscript(&arg, name, &tv, &EVALARG_EVALUATE,
								 FALSE) == OK);
	    എങ്കിൽ (n)
		clear_tv(&tv);
	}
    }
    എങ്കിൽ (*arg != NUL)
	n = FALSE;

    vim_free(tofree);
    തിരികെ n;
}

സ്ഥിതം lval_T	*redir_lval = ശൂന്യ;
#define EVALCMD_BUSY (redir_lval == (lval_T *)&redir_lval)
സ്ഥിതം garray_T redir_ga;	// only valid when redir_lval is not NULL
സ്ഥിതം char_u	*redir_endp = ശൂന്യ;
സ്ഥിതം char_u	*redir_varname = ശൂന്യ;

    പൂർണ്ണസംഖ്യ
alloc_redir_lval(ശൂന്യം)
{
    redir_lval = ALLOC_CLEAR_ONE(lval_T);
    എങ്കിൽ (redir_lval == ശൂന്യ)
	തിരികെ FAIL;
    തിരികെ OK;
}

    ശൂന്യം
clear_redir_lval(ശൂന്യം)
{
    VIM_CLEAR(redir_lval);
}

    ശൂന്യം
init_redir_ga(ശൂന്യം)
{
    ga_init2(&redir_ga, വലുപ്പം(അക്ഷരം), 500);
}

/*
 * Start recording command output to a variable
 * When "append" is TRUE append to an existing variable.
 * Returns OK if successfully completed the setup.  FAIL otherwise.
 */
    പൂർണ്ണസംഖ്യ
var_redir_start(char_u *name, പൂർണ്ണസംഖ്യ append)
{
    പൂർണ്ണസംഖ്യ		called_emsg_before;
    typval_T	tv;

    // Catch a bad name early.
    എങ്കിൽ (!eval_isnamec1(*name))
    {
	emsg(_(e_invalid_argument));
	തിരികെ FAIL;
    }

    // Make a copy of the name, it is used in redir_lval until redir ends.
    redir_varname = vim_strsave(name);
    എങ്കിൽ (redir_varname == ശൂന്യ)
	തിരികെ FAIL;

    എങ്കിൽ (alloc_redir_lval() == FAIL)
    {
	var_redir_stop();
	തിരികെ FAIL;
    }

    // The output is stored in growarray "redir_ga" until redirection ends.
    init_redir_ga();

    // Parse the variable name (can be a dict or list entry).
    redir_endp = get_lval(redir_varname, ശൂന്യ, redir_lval, FALSE, FALSE, 0,
							     FNE_CHECK_START);
    എങ്കിൽ (redir_endp == ശൂന്യ || redir_lval->ll_name == ശൂന്യ || *redir_endp != NUL)
    {
	clear_lval(redir_lval);
	എങ്കിൽ (redir_endp != ശൂന്യ && *redir_endp != NUL)
	    // Trailing characters are present after the variable name
	    semsg(_(e_trailing_characters_str), redir_endp);
	അല്ലെങ്കിൽ
	    semsg(_(e_invalid_argument_str), name);
	redir_endp = ശൂന്യ;  // don't store a value, only cleanup
	var_redir_stop();
	തിരികെ FAIL;
    }

    // check if we can write to the variable: set it to or append an empty
    // string
    called_emsg_before = called_emsg;
    tv.v_type = VAR_STRING;
    tv.vval.v_string = (char_u *)"";
    എങ്കിൽ (append)
	set_var_lval(redir_lval, redir_endp, &tv, TRUE,
					     ASSIGN_NO_DECL, (char_u *)".", 0);
    അല്ലെങ്കിൽ
	set_var_lval(redir_lval, redir_endp, &tv, TRUE,
					     ASSIGN_NO_DECL, (char_u *)"=", 0);
    clear_lval(redir_lval);
    എങ്കിൽ (called_emsg > called_emsg_before)
    {
	redir_endp = ശൂന്യ;  // don't store a value, only cleanup
	var_redir_stop();
	തിരികെ FAIL;
    }

    തിരികെ OK;
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
    ശൂന്യം
var_redir_str(char_u *മൂല്യം, പൂർണ്ണസംഖ്യ മൂല്യം_len)
{
    പൂർണ്ണസംഖ്യ		len;

    എങ്കിൽ (redir_lval == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ (മൂല്യം_len == -1)
	len = (പൂർണ്ണസംഖ്യ)STRLEN(മൂല്യം);	// Append the entire string
    അല്ലെങ്കിൽ
	len = മൂല്യം_len;		// Append only "value_len" characters

    എങ്കിൽ (ga_grow(&redir_ga, len) == OK)
    {
	mch_memmove((അക്ഷരം *)redir_ga.ga_data + redir_ga.ga_len, മൂല്യം, len);
	redir_ga.ga_len += len;
    }
    അല്ലെങ്കിൽ
	var_redir_stop();
}

/*
 * Stop redirecting command output to a variable.
 * Frees the allocated memory.
 */
    ശൂന്യം
var_redir_stop(ശൂന്യം)
{
    typval_T	tv;

    എങ്കിൽ (EVALCMD_BUSY)
    {
	redir_lval = ശൂന്യ;
	തിരികെ;
    }

    എങ്കിൽ (redir_lval != ശൂന്യ)
    {
	// If there was no error: assign the text to the variable.
	എങ്കിൽ (redir_endp != ശൂന്യ)
	{
	    ga_append(&redir_ga, NUL);  // Append the trailing NUL.
	    tv.v_type = VAR_STRING;
	    tv.vval.v_string = redir_ga.ga_data;
	    // Call get_lval() again, if it's inside a Dict or List it may
	    // have changed.
	    redir_endp = get_lval(redir_varname, ശൂന്യ, redir_lval,
					FALSE, FALSE, 0, FNE_CHECK_START);
	    എങ്കിൽ (redir_endp != ശൂന്യ && redir_lval->ll_name != ശൂന്യ)
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
get_clear_redir_ga(ശൂന്യം)
{
    char_u *res;

    ga_append(&redir_ga, NUL);  // Append the trailing NUL.
    res = redir_ga.ga_data;
    redir_ga.ga_data = ശൂന്യ;
    തിരികെ res;
}

/*
 * "gettabvar()" function
 */
    ശൂന്യം
f_gettabvar(typval_T *argvars, typval_T *rettv)
{
    char_u	*varname;
    tabpage_T	*tp;
    win_T	*win = ശൂന്യ;

    എങ്കിൽ (in_vim9script()
	    && (check_for_സംഖ്യ_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	തിരികെ;

    varname = tv_get_string_chk(&argvars[1]);
    tp = find_tabpage((പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ_chk(&argvars[0], ശൂന്യ));
    എങ്കിൽ (tp != ശൂന്യ)
	win = tp == curtab || tp->tp_firstwin == ശൂന്യ ? firstwin
		: tp->tp_firstwin;

    get_var_from(varname, rettv, &argvars[2], 't', tp, win, ശൂന്യ);
}

/*
 * "gettabwinvar()" function
 */
    ശൂന്യം
f_gettabwinvar(typval_T *argvars, typval_T *rettv)
{
    എങ്കിൽ (in_vim9script()
	    && (check_for_സംഖ്യ_arg(argvars, 0) == FAIL
		|| check_for_സംഖ്യ_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL))
	തിരികെ;

    getwinvar(argvars, rettv, 1);
}

/*
 * "getwinvar()" function
 */
    ശൂന്യം
f_getwinvar(typval_T *argvars, typval_T *rettv)
{
    എങ്കിൽ (in_vim9script()
	    && (check_for_സംഖ്യ_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	തിരികെ;

    getwinvar(argvars, rettv, 0);
}

/*
 * "getbufvar()" function
 */
    ശൂന്യം
f_getbufvar(typval_T *argvars, typval_T *rettv)
{
    char_u	*varname;
    buf_T	*buf;

    എങ്കിൽ (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	തിരികെ;

    varname = tv_get_string_chk(&argvars[1]);
    buf = tv_get_buf_from_arg(&argvars[0]);

    get_var_from(varname, rettv, &argvars[2], 'b', curtab, curwin, buf);
}

/*
 * "settabvar()" function
 */
    ശൂന്യം
f_settabvar(typval_T *argvars, typval_T *rettv UNUSED)
{
    tabpage_T	*save_curtab;
    tabpage_T	*tp;
    tabpage_T	*save_lu_tp;
    char_u	*varname, *tabvarname;
    typval_T	*varp;

    എങ്കിൽ (check_secure())
	തിരികെ;

    എങ്കിൽ (in_vim9script()
	    && (check_for_സംഖ്യ_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	തിരികെ;

    tp = find_tabpage((പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ_chk(&argvars[0], ശൂന്യ));
    varname = tv_get_string_chk(&argvars[1]);
    varp = &argvars[2];

    എങ്കിൽ (varname == ശൂന്യ || tp == ശൂന്യ)
	തിരികെ;

    save_curtab = curtab;
    save_lu_tp = lastused_tabpage;
    goto_tabpage_tp(tp, FALSE, FALSE);

    tabvarname = alloc(STRLEN(varname) + 3);
    എങ്കിൽ (tabvarname != ശൂന്യ)
    {
	STRCPY(tabvarname, "t:");
	STRCPY(tabvarname + 2, varname);
	set_var(tabvarname, varp, TRUE);
	vim_free(tabvarname);
    }

    // Restore current tabpage and last accessed tabpage.
    എങ്കിൽ (valid_tabpage(save_curtab))
    {
	goto_tabpage_tp(save_curtab, FALSE, FALSE);
	എങ്കിൽ (valid_tabpage(save_lu_tp))
	    lastused_tabpage = save_lu_tp;
    }
}

/*
 * "settabwinvar()" function
 */
    ശൂന്യം
f_settabwinvar(typval_T *argvars, typval_T *rettv UNUSED)
{
    എങ്കിൽ (in_vim9script()
	    && (check_for_സംഖ്യ_arg(argvars, 0) == FAIL
		|| check_for_സംഖ്യ_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL))
	തിരികെ;

    setwinvar(argvars, 1);
}

/*
 * "setwinvar()" function
 */
    ശൂന്യം
f_setwinvar(typval_T *argvars, typval_T *rettv UNUSED)
{
    എങ്കിൽ (in_vim9script()
	    && (check_for_സംഖ്യ_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	തിരികെ;

    setwinvar(argvars, 0);
}

/*
 * "setbufvar()" function
 */
    ശൂന്യം
f_setbufvar(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    char_u	*varname, *bufvarname;
    typval_T	*varp;

    എങ്കിൽ (check_secure())
	തിരികെ;

    എങ്കിൽ (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	തിരികെ;

    varname = tv_get_string_chk(&argvars[1]);
    buf = tv_get_buf_from_arg(&argvars[0]);
    varp = &argvars[2];

    എങ്കിൽ (buf == ശൂന്യ || varname == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ (*varname == '&')
    {
	aco_save_T	aco;
	// safe the current window position, it could
	// change because of 'scrollbind' window-local
	// options
	linenr_T	old_topline = curwin->w_topline;

	// Set curbuf to be our buf, temporarily.
	aucmd_prepbuf(&aco, buf);
	എങ്കിൽ (curbuf == buf)
	{
	    // Only when it worked to set "curbuf".
	    set_option_from_tv(varname + 1, varp);

	    // reset notion of buffer
	    aucmd_restbuf(&aco);
	}
	curwin->w_topline = old_topline;
    }
    അല്ലെങ്കിൽ
    {
	bufvarname = alloc(STRLEN(varname) + 3);
	എങ്കിൽ (bufvarname != ശൂന്യ)
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
    പൂർണ്ണസംഖ്യ		r = OK;

    CLEAR_FIELD(res);
    എങ്കിൽ (arg->v_type == VAR_PARTIAL && arg->vval.v_partial != ശൂന്യ)
    {
	res.cb_partial = arg->vval.v_partial;
	++res.cb_partial->pt_refcount;
	res.cb_name = partial_name(res.cb_partial);
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (arg->v_type == VAR_STRING && arg->vval.v_string != ശൂന്യ
					       && SAFE_isdigit(*arg->vval.v_string))
	    r = FAIL;
	അല്ലെങ്കിൽ എങ്കിൽ (arg->v_type == VAR_FUNC || arg->v_type == VAR_STRING)
	{
	    res.cb_name = arg->vval.v_string;
	    എങ്കിൽ (arg->v_type == VAR_STRING)
	    {
		char_u *name = get_scriptlocal_funcname(arg->vval.v_string);
		എങ്കിൽ (name != ശൂന്യ)
		{
		    res.cb_name = name;
		    res.cb_free_name = TRUE;
		}
	    }
	    func_ref(res.cb_name);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (arg->v_type == VAR_സംഖ്യ && arg->vval.v_സംഖ്യ == 0)
	    res.cb_name = (char_u *)"";
	അല്ലെങ്കിൽ
	    r = FAIL;

	എങ്കിൽ (r == FAIL)
	{
	    emsg(_(e_invalid_callback_argument));
	    res.cb_name = ശൂന്യ;
	}
    }
    തിരികെ res;
}

/*
 * Copy a callback into a typval_T.
 */
    ശൂന്യം
put_callback(callback_T *cb, typval_T *tv)
{
    എങ്കിൽ (cb->cb_partial != ശൂന്യ)
    {
	tv->v_type = VAR_PARTIAL;
	tv->vval.v_partial = cb->cb_partial;
	++tv->vval.v_partial->pt_refcount;
    }
    അല്ലെങ്കിൽ
    {
	tv->v_type = VAR_FUNC;
	tv->vval.v_string = vim_strsave(cb->cb_name);
	func_ref(cb->cb_name);
    }
}

    സ്ഥിതം bool
does_callback_own_cb_name(callback_T *cb)
{
    // If cb_partial != NULL then *cb->cb_name is owned by the partial.
    തിരികെ cb->cb_partial || cb->cb_free_name;
}

/*
 * Make a copy of "src" into "dest", allocating the function name if needed,
 * without incrementing the refcount.
 */
    ശൂന്യം
set_callback(callback_T *dest, callback_T *src)
{
    *dest = *src;
    എങ്കിൽ (!does_callback_own_cb_name(src))
    {
	dest->cb_name = vim_strsave(src->cb_name);
	dest->cb_free_name = TRUE;
    }
    *src = (callback_T){0};
}

/*
 * Copy callback from "src" to "dest", incrementing the refcounts.
 */
    ശൂന്യം
copy_callback(callback_T *dest, callback_T *src)
{
    dest->cb_partial = src->cb_partial;
    എങ്കിൽ (dest->cb_partial != ശൂന്യ)
    {
	dest->cb_name = src->cb_name;
	dest->cb_free_name = FALSE;
	++dest->cb_partial->pt_refcount;
    }
    അല്ലെങ്കിൽ
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
    ശൂന്യം
expand_autload_callback(callback_T *cb)
{
    char_u	*name;
    char_u	*p;
    imported_T	*import;

    എങ്കിൽ (!in_vim9script() || cb->cb_name == ശൂന്യ
	    || (!cb->cb_free_name
	       && (cb->cb_partial == ശൂന്യ || cb->cb_partial->pt_name == ശൂന്യ)))
	തിരികെ;
    എങ്കിൽ (cb->cb_partial != ശൂന്യ)
	name = cb->cb_partial->pt_name;
    അല്ലെങ്കിൽ
	name = cb->cb_name;
    p = vim_strchr(name, '.');
    എങ്കിൽ (p == ശൂന്യ)
	തിരികെ;

    import = find_imported(name, p - name, FALSE);
    എങ്കിൽ (import == ശൂന്യ || !SCRIPT_ID_VALID(import->imp_sid))
	തിരികെ;

    scriptitem_T *si = SCRIPT_ITEM(import->imp_sid);
    എങ്കിൽ (si->sn_autoload_prefix == ശൂന്യ)
	തിരികെ;

    char_u *newname = concat_str(si->sn_autoload_prefix, p + 1);
    എങ്കിൽ (newname == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ (cb->cb_partial != ശൂന്യ)
    {
	എങ്കിൽ (cb->cb_name == cb->cb_partial->pt_name)
	    cb->cb_name = newname;
	vim_free(cb->cb_partial->pt_name);
	cb->cb_partial->pt_name = newname;
    }
    അല്ലെങ്കിൽ
    {
	vim_free(cb->cb_name);
	cb->cb_name = newname;
    }
}

/*
 * Unref/free "callback" returned by get_callback() or set_callback().
 */
    ശൂന്യം
free_callback(callback_T *callback)
{
    എങ്കിൽ (callback->cb_partial != ശൂന്യ)
    {
	partial_unref(callback->cb_partial);
	callback->cb_partial = ശൂന്യ;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (callback->cb_name != ശൂന്യ)
	func_unref(callback->cb_name);
    എങ്കിൽ (callback->cb_free_name)
    {
	vim_free(callback->cb_name);
	callback->cb_free_name = FALSE;
    }
    callback->cb_name = ശൂന്യ;
}

#endif // FEAT_EVAL
