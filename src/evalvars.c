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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ defined(FEAT_EVAL)

ਸਥਿਰ dictitem_T	globvars_var;		// variable used for g:
ਸਥਿਰ dict_T		globvardict;		// Dictionary with g: variables
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ globvarht globvardict.dv_hashtab

/*
 * Old Vim variables such as "v:version" are also available without the "v:".
 * Also in functions.  We need a special hashtable for them.
 */
ਸਥਿਰ hashtab_T	compat_hashtab;

/*
 * Array to hold the value of v: variables.
 * The value is in a dictitem, so that it can also be used in the v: scope.
 * The reason to use this table anyway is for very quick access to the
 * variables with the VV_ defines.
 */

// values for vv_flags:
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_COMPAT	1	// compatible, also used without "v:"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_RO		2	// read-only
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_RO_SBX	4	// read-only in the sandbox

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VV_NAME(s, t)	s, {{t, 0, {0}}, 0, {s}}

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ vimvar vimvar_T;

ਸਥਿਰ ਬਣਤਰ vimvar
{
    ਅੱਖਰ	*vv_name;	// name of variable, without v:
    dictitem16_T vv_di;		// value and name for key (max 16 chars!)
    type_T	*vv_type;	// type or NULL
    ਅੱਖਰ	vv_flags;	// VV_COMPAT, VV_RO, VV_RO_SBX
} vimvars[VV_LEN] =
{
    // The order here must match the VV_ defines in vim.h!
    // Initializing a union does not work, leave tv.vval empty to get zero's.
    {VV_NAME("count",		 VAR_ਸੰਖਿਆ), ਨਿਰਾ, VV_COMPAT+VV_RO},
    {VV_NAME("count1",		 VAR_ਸੰਖਿਆ), ਨਿਰਾ, VV_RO},
    {VV_NAME("prevcount",	 VAR_ਸੰਖਿਆ), ਨਿਰਾ, VV_RO},
    {VV_NAME("errmsg",		 VAR_STRING), ਨਿਰਾ, VV_COMPAT},
    {VV_NAME("warningmsg",	 VAR_STRING), ਨਿਰਾ, 0},
    {VV_NAME("statusmsg",	 VAR_STRING), ਨਿਰਾ, 0},
    {VV_NAME("shell_error",	 VAR_ਸੰਖਿਆ), ਨਿਰਾ, VV_COMPAT+VV_RO},
    {VV_NAME("this_session",	 VAR_STRING), ਨਿਰਾ, VV_COMPAT},
    {VV_NAME("version",		 VAR_ਸੰਖਿਆ), ਨਿਰਾ, VV_COMPAT+VV_RO},
    {VV_NAME("lnum",		 VAR_ਸੰਖਿਆ), ਨਿਰਾ, VV_RO_SBX},
    {VV_NAME("termresponse",	 VAR_STRING), ਨਿਰਾ, VV_RO},
    {VV_NAME("fname",		 VAR_STRING), ਨਿਰਾ, VV_RO},
    {VV_NAME("lang",		 VAR_STRING), ਨਿਰਾ, VV_RO},
    {VV_NAME("lc_time",		 VAR_STRING), ਨਿਰਾ, VV_RO},
    {VV_NAME("ctype",		 VAR_STRING), ਨਿਰਾ, VV_RO},
    {VV_NAME("charconvert_from", VAR_STRING), ਨਿਰਾ, VV_RO},
    {VV_NAME("charconvert_to",	 VAR_STRING), ਨਿਰਾ, VV_RO},
    {VV_NAME("fname_in",	 VAR_STRING), ਨਿਰਾ, VV_RO},
    {VV_NAME("fname_out",	 VAR_STRING), ਨਿਰਾ, VV_RO},
    {VV_NAME("fname_new",	 VAR_STRING), ਨਿਰਾ, VV_RO},
    {VV_NAME("fname_diff",	 VAR_STRING), ਨਿਰਾ, VV_RO},
    {VV_NAME("cmdarg",		 VAR_STRING), ਨਿਰਾ, VV_RO},
    {VV_NAME("foldstart",	 VAR_ਸੰਖਿਆ), ਨਿਰਾ, VV_RO_SBX},
    {VV_NAME("foldend",		 VAR_ਸੰਖਿਆ), ਨਿਰਾ, VV_RO_SBX},
    {VV_NAME("folddashes",	 VAR_STRING), ਨਿਰਾ, VV_RO_SBX},
    {VV_NAME("foldlevel",	 VAR_ਸੰਖਿਆ), ਨਿਰਾ, VV_RO_SBX},
    {VV_NAME("progname",	 VAR_STRING), ਨਿਰਾ, VV_RO},
    {VV_NAME("servername",	 VAR_STRING), ਨਿਰਾ, VV_RO},
    {VV_NAME("dying",		 VAR_ਸੰਖਿਆ), ਨਿਰਾ, VV_RO},
    {VV_NAME("exception",	 VAR_STRING), ਨਿਰਾ, VV_RO},
    {VV_NAME("throwpoint",	 VAR_STRING), ਨਿਰਾ, VV_RO},
    {VV_NAME("register",	 VAR_STRING), ਨਿਰਾ, VV_RO},
    {VV_NAME("cmdbang",		 VAR_ਸੰਖਿਆ), ਨਿਰਾ, VV_RO},
    {VV_NAME("insertmode",	 VAR_STRING), ਨਿਰਾ, VV_RO},
    {VV_NAME("val",		 VAR_UNKNOWN), ਨਿਰਾ, VV_RO},
    {VV_NAME("key",		 VAR_UNKNOWN), ਨਿਰਾ, VV_RO},
    {VV_NAME("profiling",	 VAR_ਸੰਖਿਆ), ਨਿਰਾ, VV_RO},
    {VV_NAME("fcs_reason",	 VAR_STRING), ਨਿਰਾ, VV_RO},
    {VV_NAME("fcs_choice",	 VAR_STRING), ਨਿਰਾ, 0},
    {VV_NAME("beval_bufnr",	 VAR_ਸੰਖਿਆ), ਨਿਰਾ, VV_RO},
    {VV_NAME("beval_winnr",	 VAR_ਸੰਖਿਆ), ਨਿਰਾ, VV_RO},
    {VV_NAME("beval_winid",	 VAR_ਸੰਖਿਆ), ਨਿਰਾ, VV_RO},
    {VV_NAME("beval_lnum",	 VAR_ਸੰਖਿਆ), ਨਿਰਾ, VV_RO},
    {VV_NAME("beval_col",	 VAR_ਸੰਖਿਆ), ਨਿਰਾ, VV_RO},
    {VV_NAME("beval_text",	 VAR_STRING), ਨਿਰਾ, VV_RO},
    {VV_NAME("scrollstart",	 VAR_STRING), ਨਿਰਾ, 0},
    {VV_NAME("swapname",	 VAR_STRING), ਨਿਰਾ, VV_RO},
    {VV_NAME("swapchoice",	 VAR_STRING), ਨਿਰਾ, 0},
    {VV_NAME("swapcommand",	 VAR_STRING), ਨਿਰਾ, VV_RO},
    {VV_NAME("char",		 VAR_STRING), ਨਿਰਾ, 0},
    {VV_NAME("mouse_win",	 VAR_ਸੰਖਿਆ), ਨਿਰਾ, 0},
    {VV_NAME("mouse_winid",	 VAR_ਸੰਖਿਆ), ਨਿਰਾ, 0},
    {VV_NAME("mouse_lnum",	 VAR_ਸੰਖਿਆ), ਨਿਰਾ, 0},
    {VV_NAME("mouse_col",	 VAR_ਸੰਖਿਆ), ਨਿਰਾ, 0},
    {VV_NAME("operator",	 VAR_STRING), ਨਿਰਾ, VV_RO},
    {VV_NAME("searchforward",	 VAR_ਸੰਖਿਆ), ਨਿਰਾ, 0},
    {VV_NAME("hlsearch",	 VAR_ਸੰਖਿਆ), ਨਿਰਾ, 0},
    {VV_NAME("oldfiles",	 VAR_LIST), &t_list_string, 0},
    {VV_NAME("windowid",	 VAR_ਸੰਖਿਆ), ਨਿਰਾ, VV_RO},
    {VV_NAME("progpath",	 VAR_STRING), ਨਿਰਾ, VV_RO},
    {VV_NAME("completed_item",	 VAR_DICT), &t_dict_string, 0},
    {VV_NAME("option_new",	 VAR_STRING), ਨਿਰਾ, VV_RO},
    {VV_NAME("option_old",	 VAR_STRING), ਨਿਰਾ, VV_RO},
    {VV_NAME("option_oldlocal",	 VAR_STRING), ਨਿਰਾ, VV_RO},
    {VV_NAME("option_oldglobal", VAR_STRING), ਨਿਰਾ, VV_RO},
    {VV_NAME("option_command",	 VAR_STRING), ਨਿਰਾ, VV_RO},
    {VV_NAME("option_type",	 VAR_STRING), ਨਿਰਾ, VV_RO},
    {VV_NAME("errors",		 VAR_LIST), &t_list_string, 0},
    {VV_NAME("false",		 VAR_BOOL), ਨਿਰਾ, VV_RO},
    {VV_NAME("true",		 VAR_BOOL), ਨਿਰਾ, VV_RO},
    {VV_NAME("none",		 VAR_SPECIAL), ਨਿਰਾ, VV_RO},
    {VV_NAME("null",		 VAR_SPECIAL), ਨਿਰਾ, VV_RO},
    {VV_NAME("numbermax",	 VAR_ਸੰਖਿਆ), ਨਿਰਾ, VV_RO},
    {VV_NAME("numbermin",	 VAR_ਸੰਖਿਆ), ਨਿਰਾ, VV_RO},
    {VV_NAME("numbersize",	 VAR_ਸੰਖਿਆ), ਨਿਰਾ, VV_RO},
    {VV_NAME("vim_did_enter",	 VAR_ਸੰਖਿਆ), ਨਿਰਾ, VV_RO},
    {VV_NAME("testing",		 VAR_ਸੰਖਿਆ), ਨਿਰਾ, 0},
    {VV_NAME("t_number",	 VAR_ਸੰਖਿਆ), ਨਿਰਾ, VV_RO},
    {VV_NAME("t_string",	 VAR_ਸੰਖਿਆ), ਨਿਰਾ, VV_RO},
    {VV_NAME("t_func",		 VAR_ਸੰਖਿਆ), ਨਿਰਾ, VV_RO},
    {VV_NAME("t_list",		 VAR_ਸੰਖਿਆ), ਨਿਰਾ, VV_RO},
    {VV_NAME("t_dict",		 VAR_ਸੰਖਿਆ), ਨਿਰਾ, VV_RO},
    {VV_NAME("t_float",		 VAR_ਸੰਖਿਆ), ਨਿਰਾ, VV_RO},
    {VV_NAME("t_bool",		 VAR_ਸੰਖਿਆ), ਨਿਰਾ, VV_RO},
    {VV_NAME("t_none",		 VAR_ਸੰਖਿਆ), ਨਿਰਾ, VV_RO},
    {VV_NAME("t_job",		 VAR_ਸੰਖਿਆ), ਨਿਰਾ, VV_RO},
    {VV_NAME("t_channel",	 VAR_ਸੰਖਿਆ), ਨਿਰਾ, VV_RO},
    {VV_NAME("t_blob",		 VAR_ਸੰਖਿਆ), ਨਿਰਾ, VV_RO},
    {VV_NAME("t_class",		 VAR_ਸੰਖਿਆ), ਨਿਰਾ, VV_RO},
    {VV_NAME("t_object",	 VAR_ਸੰਖਿਆ), ਨਿਰਾ, VV_RO},
    {VV_NAME("termrfgresp",	 VAR_STRING), ਨਿਰਾ, VV_RO},
    {VV_NAME("termrbgresp",	 VAR_STRING), ਨਿਰਾ, VV_RO},
    {VV_NAME("termu7resp",	 VAR_STRING), ਨਿਰਾ, VV_RO},
    {VV_NAME("termstyleresp",	 VAR_STRING), ਨਿਰਾ, VV_RO},
    {VV_NAME("termblinkresp",	 VAR_STRING), ਨਿਰਾ, VV_RO},
    {VV_NAME("event",		 VAR_DICT), ਨਿਰਾ, VV_RO},
    {VV_NAME("versionlong",	 VAR_ਸੰਖਿਆ), ਨਿਰਾ, VV_RO},
    {VV_NAME("echospace",	 VAR_ਸੰਖਿਆ), ਨਿਰਾ, VV_RO},
    {VV_NAME("argv",		 VAR_LIST), &t_list_string, VV_RO},
    {VV_NAME("collate",		 VAR_STRING), ਨਿਰਾ, VV_RO},
    {VV_NAME("exiting",		 VAR_SPECIAL), ਨਿਰਾ, VV_RO},
    {VV_NAME("colornames",       VAR_DICT), &t_dict_string, VV_RO},
    {VV_NAME("sizeofint",	 VAR_ਸੰਖਿਆ), ਨਿਰਾ, VV_RO},
    {VV_NAME("sizeoflong",	 VAR_ਸੰਖਿਆ), ਨਿਰਾ, VV_RO},
    {VV_NAME("sizeofpointer",	 VAR_ਸੰਖਿਆ), ਨਿਰਾ, VV_RO},
    {VV_NAME("maxcol",		 VAR_ਸੰਖਿਆ), ਨਿਰਾ, VV_RO},
    {VV_NAME("python3_version",	 VAR_ਸੰਖਿਆ), ਨਿਰਾ, VV_RO},
    {VV_NAME("t_typealias",	 VAR_ਸੰਖਿਆ), ਨਿਰਾ, VV_RO},
    {VV_NAME("t_enum",		 VAR_ਸੰਖਿਆ), ਨਿਰਾ, VV_RO},
    {VV_NAME("t_enumvalue",	 VAR_ਸੰਖਿਆ), ਨਿਰਾ, VV_RO},
    {VV_NAME("stacktrace",	 VAR_LIST), &t_list_dict_any, VV_RO},
    {VV_NAME("t_tuple",		 VAR_ਸੰਖਿਆ), ਨਿਰਾ, VV_RO},
    {VV_NAME("wayland_display",  VAR_STRING), ਨਿਰਾ, VV_RO},
    {VV_NAME("clipmethod",	 VAR_STRING), ਨਿਰਾ, VV_RO},
    {VV_NAME("termda1",		 VAR_STRING), ਨਿਰਾ, VV_RO},
    {VV_NAME("termosc",	 VAR_STRING), ਨਿਰਾ, VV_RO},
    {VV_NAME("vim_did_init",	 VAR_ਸੰਖਿਆ), ਨਿਰਾ, VV_RO},
    {VV_NAME("clipproviders",	 VAR_DICT), ਨਿਰਾ, VV_RO},
    {VV_NAME("imagebackend",	 VAR_STRING), ਨਿਰਾ, VV_RO}
};

// shorthand
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ vv_tv_type	vv_di.di_tv.v_type
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ vv_nr		vv_di.di_tv.vval.v_ਸੰਖਿਆ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ vv_float	vv_di.di_tv.vval.v_float
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ vv_str		vv_di.di_tv.vval.v_string
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ vv_list		vv_di.di_tv.vval.v_list
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ vv_dict		vv_di.di_tv.vval.v_dict
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ vv_blob		vv_di.di_tv.vval.v_blob
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ vv_tv		vv_di.di_tv

ਸਥਿਰ dictitem_T	vimvars_var;		// variable used for v:
ਸਥਿਰ dict_T		vimvardict;		// Dictionary with v: variables
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ vimvarht  vimvardict.dv_hashtab

// for VIM_VERSION_ defines
#ਅਨ੍ਤਰ੍ਭਾਵ "version.h"

ਸਥਿਰ ਖਾਲੀ list_glob_vars(ਪੂਰਨ *ਪਹਿਲਾ);
ਸਥਿਰ ਖਾਲੀ list_buf_vars(ਪੂਰਨ *ਪਹਿਲਾ);
ਸਥਿਰ ਖਾਲੀ list_win_vars(ਪੂਰਨ *ਪਹਿਲਾ);
ਸਥਿਰ ਖਾਲੀ list_tab_vars(ਪੂਰਨ *ਪਹਿਲਾ);
ਸਥਿਰ char_u *list_arg_vars(exarg_T *eap, char_u *arg, ਪੂਰਨ *ਪਹਿਲਾ);
ਸਥਿਰ char_u *ex_let_one(char_u *arg, typval_T *tv, ਪੂਰਨ copy, ਪੂਰਨ flags, char_u *endchars, char_u *op, ਪੂਰਨ var_idx);
ਸਥਿਰ ਪੂਰਨ do_unlet_var(lval_T *lp, char_u *name_ਸਿਰਾ, exarg_T *eap, ਪੂਰਨ deep, ਖਾਲੀ *cookie);
ਸਥਿਰ ਪੂਰਨ do_ਤਾਲਾ_var(lval_T *lp, char_u *name_ਸਿਰਾ, exarg_T *eap, ਪੂਰਨ deep, ਖਾਲੀ *cookie);
ਸਥਿਰ ਖਾਲੀ list_one_var(dictitem_T *v, ਅੱਖਰ *ਅਗੇਤਰ, ਪੂਰਨ *ਪਹਿਲਾ);
ਸਥਿਰ ਖਾਲੀ list_one_var_a(ਅੱਖਰ *ਅਗੇਤਰ, char_u *name, ਪੂਰਨ type, char_u *string, ਪੂਰਨ *ਪਹਿਲਾ);

/*
 * Initialize global and vim special variables
 */
    ਖਾਲੀ
evalvars_init(ਖਾਲੀ)
{
    ਪੂਰਨ		    i;
    ਬਣਤਰ vimvar   *p;

    init_var_dict(&globvardict, &globvars_var, VAR_DEF_SCOPE);
    init_var_dict(&vimvardict, &vimvars_var, VAR_SCOPE);
    vimvardict.dv_ਤਾਲਾ = VAR_FIXED;
    hash_init(&compat_hashtab);

    ਲਈ (i = 0; i < VV_LEN; ++i)
    {
	p = &vimvars[i];
	ਜੇ (p->vv_flags & VV_RO)
	    p->vv_di.di_flags = DI_FLAGS_RO | DI_FLAGS_FIX;
	ਨਹੀਂਤਾਂ ਜੇ (p->vv_flags & VV_RO_SBX)
	    p->vv_di.di_flags = DI_FLAGS_RO_SBX | DI_FLAGS_FIX;
	ਨਹੀਂਤਾਂ
	    p->vv_di.di_flags = DI_FLAGS_FIX;

	// add to v: scope dict, unless the value is not always available
	ਜੇ (p->vv_tv_type != VAR_UNKNOWN)
	    hash_ਜੋੜੋ(&vimvarht, p->vv_di.di_ਕੁੰਜੀ, "initialization");
	ਜੇ (p->vv_flags & VV_COMPAT)
	    // add to compat scope dict
	    hash_ਜੋੜੋ(&compat_hashtab, p->vv_di.di_ਕੁੰਜੀ, "initialization");
    }
    ਸੈੱਟ_vim_var_nr(VV_ਸੰਸਕਰਣ, VIM_ਸੰਸਕਰਣ_100);
    ਸੈੱਟ_vim_var_nr(VV_VERSIONLONG, VIM_ਸੰਸਕਰਣ_100 * 10000 + highest_patch());

    ਸੈੱਟ_vim_var_nr(VV_SEARCHFORWARD, 1L);
    ਸੈੱਟ_vim_var_nr(VV_HLSEARCH, 1L);
    ਸੈੱਟ_vim_var_nr(VV_EXITING, VVAL_NULL);
    ਸੈੱਟ_vim_var_dict(VV_COMPLETED_ਵਸਤੂ, dict_alloc_ਤਾਲਾ(VAR_FIXED));
    ਸੈੱਟ_vim_var_list(VV_ERRORS, list_alloc());
    ਸੈੱਟ_vim_var_dict(VV_EVENT, dict_alloc_ਤਾਲਾ(VAR_FIXED));

    ਸੈੱਟ_vim_var_nr(VV_FALSE, VVAL_FALSE);
    ਸੈੱਟ_vim_var_nr(VV_TRUE, VVAL_TRUE);
    ਸੈੱਟ_vim_var_nr(VV_NONE, VVAL_NONE);
    ਸੈੱਟ_vim_var_nr(VV_NULL, VVAL_NULL);
    ਸੈੱਟ_vim_var_nr(VV_NUMBERMAX, VARNUM_MAX);
    ਸੈੱਟ_vim_var_nr(VV_NUMBERMIN, VARNUM_MIN);
    ਸੈੱਟ_vim_var_nr(VV_NUMBERSIZE, ਆਕਾਰ(varnumber_T) * 8);
    ਸੈੱਟ_vim_var_nr(VV_SIZEOFINT, ਆਕਾਰ(ਪੂਰਨ));
    ਸੈੱਟ_vim_var_nr(VV_SIZEOFLONG, ਆਕਾਰ(ਲੰਮਾ));
    ਸੈੱਟ_vim_var_nr(VV_SIZEOFPOINTER, ਆਕਾਰ(ਅੱਖਰ *));
    ਸੈੱਟ_vim_var_nr(VV_MAXCOL, MAXCOL);

    ਸੈੱਟ_vim_var_nr(VV_TYPE_ਸੰਖਿਆ,  VAR_TYPE_ਸੰਖਿਆ);
    ਸੈੱਟ_vim_var_nr(VV_TYPE_STRING,  VAR_TYPE_STRING);
    ਸੈੱਟ_vim_var_nr(VV_TYPE_FUNC,    VAR_TYPE_FUNC);
    ਸੈੱਟ_vim_var_nr(VV_TYPE_LIST,    VAR_TYPE_LIST);
    ਸੈੱਟ_vim_var_nr(VV_TYPE_DICT,    VAR_TYPE_DICT);
    ਸੈੱਟ_vim_var_nr(VV_TYPE_FLOAT,   VAR_TYPE_FLOAT);
    ਸੈੱਟ_vim_var_nr(VV_TYPE_BOOL,    VAR_TYPE_BOOL);
    ਸੈੱਟ_vim_var_nr(VV_TYPE_NONE,    VAR_TYPE_NONE);
    ਸੈੱਟ_vim_var_nr(VV_TYPE_ਕੰਮ,     VAR_TYPE_ਕੰਮ);
    ਸੈੱਟ_vim_var_nr(VV_TYPE_CHANNEL, VAR_TYPE_CHANNEL);
    ਸੈੱਟ_vim_var_nr(VV_TYPE_BLOB,    VAR_TYPE_BLOB);
    ਸੈੱਟ_vim_var_nr(VV_TYPE_CLASS,   VAR_TYPE_CLASS);
    ਸੈੱਟ_vim_var_nr(VV_TYPE_OBJECT,  VAR_TYPE_OBJECT);
    ਸੈੱਟ_vim_var_nr(VV_TYPE_TYPEALIAS,  VAR_TYPE_TYPEALIAS);
    ਸੈੱਟ_vim_var_nr(VV_TYPE_ENUM,    VAR_TYPE_ENUM);
    ਸੈੱਟ_vim_var_nr(VV_TYPE_ENUMVALUE,  VAR_TYPE_ENUMVALUE);
    ਸੈੱਟ_vim_var_nr(VV_TYPE_TUPLE,   VAR_TYPE_TUPLE);

    ਸੈੱਟ_vim_var_nr(VV_ECHOSPACE,    sc_ਖਾਨਾ - 1);

    ਸੈੱਟ_vim_var_dict(VV_COLORNAMES, dict_alloc());
    ਸੈੱਟ_vim_var_dict(VV_CLIPPROVIDERS, dict_alloc());

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PYTHON3
    ਸੈੱਟ_vim_var_nr(VV_PYTHON3_ਸੰਸਕਰਣ, python3_ਸੰਸਕਰਣ());
#ਜੇ_ਅੰਤ

    // Default for v:register is not 0 but '"'.  This is adjusted once the
    // clipboard has been setup by calling reset_reg_var().
    ਸੈੱਟ_reg_var(0);
}

#ਜੇ defined(EXITFREE)
/*
 * Free all vim variables information on exit
 */
    ਖਾਲੀ
evalvars_clear(ਖਾਲੀ)
{
    ਪੂਰਨ		    i;
    ਬਣਤਰ vimvar   *p;

    ਲਈ (i = 0; i < VV_LEN; ++i)
    {
	p = &vimvars[i];
	ਜੇ (p->vv_di.di_tv.v_type == VAR_STRING)
	    VIM_CLEAR(p->vv_str);
	ਨਹੀਂਤਾਂ ਜੇ (p->vv_di.di_tv.v_type == VAR_LIST)
	{
	    list_unref(p->vv_list);
	    p->vv_list = ਨਿਰਾ;
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
    ਲਈ (i = 1; i <= script_ਚੀਜ਼ਾਂ.ga_len; ++i)
	vars_clear(&SCRIPT_VARS(i));
}
#ਜੇ_ਅੰਤ

    ਪੂਰਨ
garbage_collect_globvars(ਪੂਰਨ copyID)
{
    ਵਾਪਸ ਸੈੱਟ_ref_ਵਿੱਚ_ht(&globvarht, copyID, ਨਿਰਾ, ਨਿਰਾ);
}

    ਪੂਰਨ
garbage_collect_vimvars(ਪੂਰਨ copyID)
{
    ਵਾਪਸ ਸੈੱਟ_ref_ਵਿੱਚ_ht(&vimvarht, copyID, ਨਿਰਾ, ਨਿਰਾ);
}

    ਪੂਰਨ
garbage_collect_scriptvars(ਪੂਰਨ copyID)
{
    ਪੂਰਨ		    i;
    ਪੂਰਨ		    idx;
    ਪੂਰਨ		    abort = FALSE;
    scriptitem_T    *si;

    ਲਈ (i = 1; i <= script_ਚੀਜ਼ਾਂ.ga_len; ++i)
    {
	abort = abort || ਸੈੱਟ_ref_ਵਿੱਚ_ht(&SCRIPT_VARS(i), copyID, ਨਿਰਾ, ਨਿਰਾ);

	si = SCRIPT_ਵਸਤੂ(i);
	ਲਈ (idx = 0; idx < si->sn_var_vals.ga_len; ++idx)
	{
	    svar_T    *sv = ((svar_T *)si->sn_var_vals.ga_ਅੰਕੜੇ) + idx;

	    ਜੇ (sv->sv_name != ਨਿਰਾ)
		abort = abort || ਸੈੱਟ_ref_ਵਿੱਚ_ਵਸਤੂ(sv->sv_tv, copyID, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ);
	}
    }

    ਵਾਪਸ abort;
}

/*
 * Set an internal variable to a string value. Creates the variable if it does
 * not already exist.
 */
    ਖਾਲੀ
ਸੈੱਟ_ਅੰਦਰੂਨੀ_string_var(char_u *name, char_u *ਮੁੱਲ)
{
    char_u	*val;
    typval_T	*tvp;

    val = vim_strsave(ਮੁੱਲ);
    ਜੇ (val == ਨਿਰਾ)
	ਵਾਪਸ;

    tvp = alloc_string_tv(val);
    ਜੇ (tvp == ਨਿਰਾ)
	ਵਾਪਸ;

    ਸੈੱਟ_var(name, tvp, FALSE);
    free_tv(tvp);
}

    ਪੂਰਨ
eval_charconvert(
    char_u	*enc_from,
    char_u	*enc_ਮੰਜ਼ਿਲ,
    char_u	*fname_from,
    char_u	*fname_ਮੰਜ਼ਿਲ)
{
    ਪੂਰਨ		err = FALSE;
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;

    ਸੈੱਟ_vim_var_string(VV_CC_FROM, enc_from, -1);
    ਸੈੱਟ_vim_var_string(VV_CC_ਮੰਜ਼ਿਲ, enc_ਮੰਜ਼ਿਲ, -1);
    ਸੈੱਟ_vim_var_string(VV_FNAME_ਵਿੱਚ, fname_from, -1);
    ਸੈੱਟ_vim_var_string(VV_FNAME_ਬਾਹਰਲਾ, fname_ਮੰਜ਼ਿਲ, -1);
    ctx = get_option_sctx("charconvert");
    ਜੇ (ctx != ਨਿਰਾ)
	current_sctx = *ctx;

    ਜੇ (eval_ਮੰਜ਼ਿਲ_bool(p_ccv, &err, ਨਿਰਾ, FALSE, TRUE))
	err = TRUE;

    ਸੈੱਟ_vim_var_string(VV_CC_FROM, ਨਿਰਾ, -1);
    ਸੈੱਟ_vim_var_string(VV_CC_ਮੰਜ਼ਿਲ, ਨਿਰਾ, -1);
    ਸੈੱਟ_vim_var_string(VV_FNAME_ਵਿੱਚ, ਨਿਰਾ, -1);
    ਸੈੱਟ_vim_var_string(VV_FNAME_ਬਾਹਰਲਾ, ਨਿਰਾ, -1);
    current_sctx = saved_sctx;

    ਜੇ (err)
	ਵਾਪਸ FAIL;
    ਵਾਪਸ OK;
}

#ਜੇ defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
    ਪੂਰਨ
eval_printexpr(char_u *fname, char_u *ਦਲੀਲਾਂ)
{
    ਪੂਰਨ		err = FALSE;
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;

    ਸੈੱਟ_vim_var_string(VV_FNAME_ਵਿੱਚ, fname, -1);
    ਸੈੱਟ_vim_var_string(VV_CMDARG, ਦਲੀਲਾਂ, -1);
    ctx = get_option_sctx("printexpr");
    ਜੇ (ctx != ਨਿਰਾ)
	current_sctx = *ctx;

    ਜੇ (eval_ਮੰਜ਼ਿਲ_bool(p_pexpr, &err, ਨਿਰਾ, FALSE, TRUE))
	err = TRUE;

    ਸੈੱਟ_vim_var_string(VV_FNAME_ਵਿੱਚ, ਨਿਰਾ, -1);
    ਸੈੱਟ_vim_var_string(VV_CMDARG, ਨਿਰਾ, -1);
    current_sctx = saved_sctx;

    ਜੇ (err)
    {
	mch_remove(fname);
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_DIFF)
    ਖਾਲੀ
eval_diff(
    char_u	*origfile,
    char_u	*newfile,
    char_u	*outfile)
{
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;
    typval_T	*tv;

    ਸੈੱਟ_vim_var_string(VV_FNAME_ਵਿੱਚ, origfile, -1);
    ਸੈੱਟ_vim_var_string(VV_FNAME_NEW, newfile, -1);
    ਸੈੱਟ_vim_var_string(VV_FNAME_ਬਾਹਰਲਾ, outfile, -1);

    ctx = get_option_sctx("diffexpr");
    ਜੇ (ctx != ਨਿਰਾ)
	current_sctx = *ctx;

    // errors are ignored
    tv = eval_ਵਿਅੰਜਕ_ext(p_dex, ਨਿਰਾ, TRUE);
    free_tv(tv);

    ਸੈੱਟ_vim_var_string(VV_FNAME_ਵਿੱਚ, ਨਿਰਾ, -1);
    ਸੈੱਟ_vim_var_string(VV_FNAME_NEW, ਨਿਰਾ, -1);
    ਸੈੱਟ_vim_var_string(VV_FNAME_ਬਾਹਰਲਾ, ਨਿਰਾ, -1);
    current_sctx = saved_sctx;
}

    ਖਾਲੀ
eval_patch(
    char_u	*origfile,
    char_u	*difffile,
    char_u	*outfile)
{
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;
    typval_T	*tv;

    ਸੈੱਟ_vim_var_string(VV_FNAME_ਵਿੱਚ, origfile, -1);
    ਸੈੱਟ_vim_var_string(VV_FNAME_DIFF, difffile, -1);
    ਸੈੱਟ_vim_var_string(VV_FNAME_ਬਾਹਰਲਾ, outfile, -1);

    ctx = get_option_sctx("patchexpr");
    ਜੇ (ctx != ਨਿਰਾ)
	current_sctx = *ctx;

    // errors are ignored
    tv = eval_ਵਿਅੰਜਕ_ext(p_pex, ਨਿਰਾ, TRUE);
    free_tv(tv);

    ਸੈੱਟ_vim_var_string(VV_FNAME_ਵਿੱਚ, ਨਿਰਾ, -1);
    ਸੈੱਟ_vim_var_string(VV_FNAME_DIFF, ਨਿਰਾ, -1);
    ਸੈੱਟ_vim_var_string(VV_FNAME_ਬਾਹਰਲਾ, ਨਿਰਾ, -1);
    current_sctx = saved_sctx;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_SPELL)
/*
 * Evaluate an expression to a list with suggestions.
 * For the "expr:" part of 'spellsuggest'.
 * Returns NULL when there is an error.
 */
    list_T *
eval_spell_ਵਿਅੰਜਕ(char_u *badword, char_u *ਵਿਅੰਜਕ)
{
    typval_T	save_val;
    typval_T	rettv;
    list_T	*list = ਨਿਰਾ;
    char_u	*p = skipwhite(ਵਿਅੰਜਕ);
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;
    ਪੂਰਨ		r;

    // Set "v:val" to the bad word.
    prepare_vimvar(VV_VAL, &save_val);
    ਸੈੱਟ_vim_var_string(VV_VAL, badword, -1);
    ਜੇ (p_verbose == 0)
	++emsg_off;
    ctx = get_option_sctx("spellsuggest");
    ਜੇ (ctx != ਨਿਰਾ)
	current_sctx = *ctx;

    r = may_call_simple_func(p, &rettv);
    ਜੇ (r == NOTDONE)
	r = eval1(&p, &rettv, &EVALARG_EVALUATE);
    ਜੇ (r == OK)
    {
	ਜੇ (rettv.v_type != VAR_LIST)
	    clear_tv(&rettv);
	ਨਹੀਂਤਾਂ
	    list = rettv.vval.v_list;
    }

    ਜੇ (p_verbose == 0)
	--emsg_off;
    clear_tv(get_vim_var_tv(VV_VAL));
    restore_vimvar(VV_VAL, &save_val);
    current_sctx = saved_sctx;

    ਵਾਪਸ list;
}

/*
 * "list" is supposed to contain two items: a word and a number.  Return the
 * word in "pp" and the number as the return value.
 * Return -1 if anything isn't right.
 * Used to get the good word and score from the eval_spell_expr() result.
 */
    ਪੂਰਨ
get_spellword(list_T *list, char_u **pp)
{
    listitem_T	*li;

    li = list->lv_ਪਹਿਲਾ;
    ਜੇ (li == ਨਿਰਾ)
	ਵਾਪਸ -1;
    *pp = tv_get_string(&li->li_tv);

    li = li->li_ਅਗਲਾ;
    ਜੇ (li == ਨਿਰਾ)
	ਵਾਪਸ -1;
    ਵਾਪਸ (ਪੂਰਨ)tv_get_ਸੰਖਿਆ(&li->li_tv);
}
#ਜੇ_ਅੰਤ

/*
 * Prepare v: variable "idx" to be used.
 * Save the current typeval in "save_tv" and clear it.
 * When not used yet add the variable to the v: hashtable.
 */
    ਖਾਲੀ
prepare_vimvar(ਪੂਰਨ idx, typval_T *save_tv)
{
    *save_tv = vimvars[idx].vv_tv;
    vimvars[idx].vv_str = ਨਿਰਾ;  // don't free it now
    ਜੇ (vimvars[idx].vv_tv_type == VAR_UNKNOWN)
	hash_ਜੋੜੋ(&vimvarht, vimvars[idx].vv_di.di_ਕੁੰਜੀ, "prepare vimvar");
}

/*
 * Restore v: variable "idx" to typeval "save_tv".
 * Note that the v: variable must have been cleared already.
 * When no longer defined, remove the variable from the v: hashtable.
 */
    ਖਾਲੀ
restore_vimvar(ਪੂਰਨ idx, typval_T *save_tv)
{
    hashitem_T	*hi;

    vimvars[idx].vv_tv = *save_tv;
    ਜੇ (vimvars[idx].vv_tv_type != VAR_UNKNOWN)
	ਵਾਪਸ;

    hi = hash_ਲੱਭੋ(&vimvarht, vimvars[idx].vv_di.di_ਕੁੰਜੀ);
    ਜੇ (HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	ਅੰਦਰੂਨੀ_error("restore_vimvar()");
    ਨਹੀਂਤਾਂ
	hash_remove(&vimvarht, hi, "restore vimvar");
}

/*
 * List Vim variables.
 */
    ਸਥਿਰ ਖਾਲੀ
list_vim_vars(ਪੂਰਨ *ਪਹਿਲਾ)
{
    list_hashtable_vars(&vimvarht, "v:", FALSE, ਪਹਿਲਾ);
}

/*
 * List script-local variables, if there is a script.
 */
    ਸਥਿਰ ਖਾਲੀ
list_script_vars(ਪੂਰਨ *ਪਹਿਲਾ)
{
    ਜੇ (SCRIPT_ID_VALID(current_sctx.sc_sid))
	list_hashtable_vars(&SCRIPT_VARS(current_sctx.sc_sid),
							   "s:", FALSE, ਪਹਿਲਾ);
}

/*
 * Return TRUE if "name" starts with "g:", "w:", "t:" or "b:".
 * But only when an identifier character follows.
 */
    ਪੂਰਨ
is_scoped_variable(char_u *name)
{
    ਵਾਪਸ vim_strchr((char_u *)"gwbt", name[0]) != ਨਿਰਾ
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
eval_one_ਵਿਅੰਜਕ_ਵਿੱਚ_str(char_u *p, garray_T *gap, ਪੂਰਨ evaluate)
{
    char_u	*ਖੰਡ_ਸ਼ੁਰੂ = skipwhite(p + 1);  // skip the opening {
    char_u	*ਖੰਡ_ਸਿਰਾ = ਖੰਡ_ਸ਼ੁਰੂ;
    char_u	*ਵਿਅੰਜਕ_val;

    ਜੇ (*ਖੰਡ_ਸ਼ੁਰੂ == NUL)
    {
	semsg(_(e_missing_close_curly_str), p);
	ਵਾਪਸ ਨਿਰਾ;
    }
    ਜੇ (skip_ਵਿਅੰਜਕ(&ਖੰਡ_ਸਿਰਾ, ਨਿਰਾ) == FAIL)
	ਵਾਪਸ ਨਿਰਾ;
    ਖੰਡ_ਸਿਰਾ = skipwhite(ਖੰਡ_ਸਿਰਾ);
    ਜੇ (*ਖੰਡ_ਸਿਰਾ != '}')
    {
	semsg(_(e_missing_close_curly_str), p);
	ਵਾਪਸ ਨਿਰਾ;
    }
    ਜੇ (evaluate)
    {
	*ਖੰਡ_ਸਿਰਾ = NUL;
	ਵਿਅੰਜਕ_val = eval_ਮੰਜ਼ਿਲ_string(ਖੰਡ_ਸ਼ੁਰੂ, FALSE, FALSE);
	*ਖੰਡ_ਸਿਰਾ = '}';
	ਜੇ (ਵਿਅੰਜਕ_val == ਨਿਰਾ)
	    ਵਾਪਸ ਨਿਰਾ;
	ga_concat(gap, ਵਿਅੰਜਕ_val);
	vim_free(ਵਿਅੰਜਕ_val);
    }

    ਵਾਪਸ ਖੰਡ_ਸਿਰਾ + 1;
}

/*
 * Evaluate all the Vim expressions {expr} in "str" and return the resulting
 * string in allocated memory.  "{{" is reduced to "{" and "}}" to "}".
 * Used for a heredoc assignment.
 * Returns NULL for an error.
 */
    ਸਥਿਰ char_u *
eval_all_ਵਿਅੰਜਕ_ਵਿੱਚ_str(char_u *str)
{
    garray_T	ga;
    char_u	*p;

    ga_init2(&ga, 1, 80);
    p = str;

    ਜਦੋਂ (*p != NUL)
    {
	char_u	*lit_ਸ਼ੁਰੂ;
	ਪੂਰਨ	escaped_brace = FALSE;

	// Look for a block start.
	lit_ਸ਼ੁਰੂ = p;
	ਜਦੋਂ (*p != '{' && *p != '}' && *p != NUL)
	    ++p;

	ਜੇ (*p != NUL && *p == p[1])
	{
	    // Escaped brace, unescape and continue.
	    // Include the brace in the literal string.
	    ++p;
	    escaped_brace = TRUE;
	}
	ਨਹੀਂਤਾਂ ਜੇ (*p == '}')
	{
	    semsg(_(e_stray_closing_curly_str), str);
	    ga_clear(&ga);
	    ਵਾਪਸ ਨਿਰਾ;
	}

	// Append the literal part.
	ga_concat_len(&ga, lit_ਸ਼ੁਰੂ, (size_t)(p - lit_ਸ਼ੁਰੂ));

	ਜੇ (*p == NUL)
	    ਰੋਕੋ;

	ਜੇ (escaped_brace)
	{
	    // Skip the second brace.
	    ++p;
	    ਜਾਰੀ;
	}

	// Evaluate the expression and append the result.
	p = eval_one_ਵਿਅੰਜਕ_ਵਿੱਚ_str(p, &ga, TRUE);
	ਜੇ (p == ਨਿਰਾ)
	{
	    ga_clear(&ga);
	    ਵਾਪਸ ਨਿਰਾ;
	}
    }
    ga_append(&ga, NUL);

    ਵਾਪਸ ga.ga_ਅੰਕੜੇ;
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
heredoc_get(exarg_T *eap, char_u *cmd, ਪੂਰਨ script_get, ਪੂਰਨ vim9compile)
{
    char_u	*theline = ਨਿਰਾ;
    char_u	*marker;
    list_T	*l;
    char_u	*p;
    char_u	*str;
    ਪੂਰਨ		marker_indent_len = 0;
    ਪੂਰਨ		ਪਾਠ_indent_len = 0;
    char_u	*ਪਾਠ_indent = ਨਿਰਾ;
    char_u	dot[] = ".";
    ਪੂਰਨ		comment_char = ਵਿੱਚ_vim9script() ? '#' : '"';
    ਪੂਰਨ		evalstr = FALSE;
    ਪੂਰਨ		eval_failed = FALSE;
    cctx_T	*cctx = vim9compile ? eap->cookie : ਨਿਰਾ;
    ਪੂਰਨ		ਗਿਣਤੀ_2 = 0;
    ਪੂਰਨ		heredoc_ਵਿੱਚ_string = FALSE;
    char_u	*ਸਤਰ_arg = ਨਿਰਾ;
    char_u	*nl_ptr = vim_strchr(cmd, '\n');

    ਜੇ (nl_ptr != ਨਿਰਾ)
    {
	heredoc_ਵਿੱਚ_string = TRUE;
	ਸਤਰ_arg = nl_ptr + 1;
	*nl_ptr = NUL;
    }
    ਨਹੀਂਤਾਂ ਜੇ (eap->ea_getline == ਨਿਰਾ)
    {
	emsg(_(e_cannot_use_heredoc_ਇੱਥੇ));
	ਵਾਪਸ ਨਿਰਾ;
    }

    // Check for the optional 'trim' word before the marker
    cmd = skipwhite(cmd);

    ਜਦੋਂ (TRUE)
    {
	ਜੇ (STRNCMP(cmd, "trim", 4) == 0
		&& (cmd[4] == NUL || VIM_ISWHITE(cmd[4])))
	{
	    cmd = skipwhite(cmd + 4);

	    // Trim the indentation from all the lines in the here document.
	    // The amount of indentation trimmed is the same as the indentation
	    // of the first line after the :let command line.  To find the end
	    // marker the indent of the :let command line is trimmed.
	    p = *eap->cmdlinep;
	    ਜਦੋਂ (VIM_ISWHITE(*p))
	    {
		p++;
		marker_indent_len++;
	    }
	    ਪਾਠ_indent_len = -1;

	    ਜਾਰੀ;
	}
	ਜੇ (STRNCMP(cmd, "eval", 4) == 0
		&& (cmd[4] == NUL || VIM_ISWHITE(cmd[4])))
	{
	    cmd = skipwhite(cmd + 4);
	    evalstr = TRUE;
	    ਜਾਰੀ;
	}
	ਰੋਕੋ;
    }

    // The marker is the next word.
    ਜੇ (*cmd != NUL && *cmd != comment_char)
    {
	marker = skipwhite(cmd);
	p = skiptowhite(marker);
	ਜੇ (*skipwhite(p) != NUL && *skipwhite(p) != comment_char)
	{
	    semsg(_(e_trailing_characters_str), p);
	    ਵਾਪਸ ਨਿਰਾ;
	}
	*p = NUL;
	ਜੇ (!script_get && vim_islower(*marker))
	{
	    emsg(_(e_marker_cannot_ਸ਼ੁਰੂ_with_ਹੇਠਲਾ_case_letter));
	    ਵਾਪਸ ਨਿਰਾ;
	}
    }
    ਨਹੀਂਤਾਂ
    {
	// When getting lines for an embedded script, if the marker is missing,
	// accept '.' as the marker.
	ਜੇ (script_get)
	    marker = dot;
	ਨਹੀਂਤਾਂ
	{
	    emsg(_(e_missing_marker));
	    ਵਾਪਸ ਨਿਰਾ;
	}
    }

    l = list_alloc();
    ਜੇ (l == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    ਲਈ (;;)
    {
	ਪੂਰਨ	mi = 0;
	ਪੂਰਨ	ti = 0;

	ਜੇ (heredoc_ਵਿੱਚ_string)
	{
	    char_u	*ਅਗਲਾ_ਸਤਰ;

	    // heredoc in a string separated by newlines.  Get the next line
	    // from the string.

	    ਜੇ (*ਸਤਰ_arg == NUL)
	    {
		semsg(_(e_missing_ਸਿਰਾ_marker_str), marker);
		ਰੋਕੋ;
	    }

	    theline = ਸਤਰ_arg;
	    ਅਗਲਾ_ਸਤਰ = vim_strchr(theline, '\n');
	    ਜੇ (ਅਗਲਾ_ਸਤਰ == ਨਿਰਾ)
		ਸਤਰ_arg += STRLEN(ਸਤਰ_arg);
	    ਨਹੀਂਤਾਂ
	    {
		*ਅਗਲਾ_ਸਤਰ = NUL;
		ਸਤਰ_arg = ਅਗਲਾ_ਸਤਰ + 1;
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    vim_free(theline);
	    theline = eap->ea_getline(NUL, eap->cookie, 0, FALSE);
	    ਜੇ (theline == ਨਿਰਾ)
	    {
		semsg(_(e_missing_ਸਿਰਾ_marker_str), marker);
		ਰੋਕੋ;
	    }
	}

	// with "trim": skip the indent matching the :let line to find the
	// marker
	ਜੇ (marker_indent_len > 0
		&& STRNCMP(theline, *eap->cmdlinep, marker_indent_len) == 0)
	    mi = marker_indent_len;
	ਜੇ (STRCMP(marker, theline + mi) == 0)
	    ਰੋਕੋ;

	// If expression evaluation failed in the heredoc, then skip till the
	// end marker.
	ਜੇ (eval_failed)
	    ਜਾਰੀ;

	ਜੇ (ਪਾਠ_indent_len == -1 && *theline != NUL)
	{
	    // set the text indent from the first line.
	    p = theline;
	    ਪਾਠ_indent_len = 0;
	    ਜਦੋਂ (VIM_ISWHITE(*p))
	    {
		p++;
		ਪਾਠ_indent_len++;
	    }
	    ਪਾਠ_indent = vim_strnsave(theline, ਪਾਠ_indent_len);
	}
	// with "trim": skip the indent matching the first line
	ਜੇ (ਪਾਠ_indent != ਨਿਰਾ)
	    ਲਈ (ti = 0; ti < ਪਾਠ_indent_len; ++ti)
		ਜੇ (theline[ti] != ਪਾਠ_indent[ti])
		    ਰੋਕੋ;

	str = theline + ti;
	ਜੇ (vim9compile)
	{
	    ਜੇ (compile_all_ਵਿਅੰਜਕ_ਵਿੱਚ_str(str, evalstr, cctx) == FAIL)
	    {
		vim_free(theline);
		vim_free(ਪਾਠ_indent);
		list_free(l);
		ਵਾਪਸ FAIL;
	    }
	    ਗਿਣਤੀ_2++;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਪੂਰਨ	    free_str = FALSE;

	    ਜੇ (evalstr && !eap->skip)
	    {
		str = eval_all_ਵਿਅੰਜਕ_ਵਿੱਚ_str(str);
		ਜੇ (str == ਨਿਰਾ)
		{
		    // expression evaluation failed
		    eval_failed = TRUE;
		    ਜਾਰੀ;
		}
		free_str = TRUE;
	    }

	    ਜੇ (list_append_string(l, str, -1) == FAIL)
	    {
		ਜੇ (free_str)
		    vim_free(str);
		ਰੋਕੋ;
	    }
	    ਜੇ (free_str)
		vim_free(str);
	}
    }
    ਜੇ (heredoc_ਵਿੱਚ_string)
	// Next command follows the heredoc in the string.
	eap->nextcmd = ਸਤਰ_arg;
    ਨਹੀਂਤਾਂ
	vim_free(theline);
    vim_free(ਪਾਠ_indent);

    ਜੇ (vim9compile && cctx->ctx_skip != SKIP_YES && !eval_failed)
	generate_NEWLIST(cctx, ਗਿਣਤੀ_2, FALSE);

    ਜੇ (eval_failed)
    {
	// expression evaluation in the heredoc failed
	list_free(l);
	ਵਾਪਸ ਨਿਰਾ;
    }
    ਵਾਪਸ l;
}

/*
 * Vim9 variable declaration:
 * ":var name"
 * ":var name: type"
 * ":var name = expr"
 * ":var name: type = expr"
 * etc.
 */
    ਖਾਲੀ
ex_var(exarg_T *eap)
{
    char_u *p = eap->cmd;
    ਪੂਰਨ	    has_var;

    ਜੇ (!ਵਿੱਚ_vim9script())
    {
	semsg(_(e_str_cannot_be_ਵਰਤਿਆ_ਵਿੱਚ_legacy_vim_script), ":var");
	ਵਾਪਸ;
    }
    has_var = checkforcmd_noparen(&p, "var", 3);
    ਜੇ (current_sctx.sc_sid == 0 && has_var)
    {
	emsg(_(e_cannot_declare_variable_on_command_ਸਤਰ));
	ਵਾਪਸ;
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
    ਖਾਲੀ
ex_let(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    char_u	*ਵਿਅੰਜਕ = ਨਿਰਾ;
    typval_T	rettv;
    ਪੂਰਨ		var_ਗਿਣਤੀ = 0;
    ਪੂਰਨ		semicolon = 0;
    char_u	op[4];
    char_u	*argend;
    ਪੂਰਨ		ਪਹਿਲਾ = TRUE;
    ਪੂਰਨ		concat;
    ਪੂਰਨ		has_ਸੌਂਪੋ;
    ਪੂਰਨ		flags = 0;
    ਪੂਰਨ		vim9script = ਵਿੱਚ_vim9script();

    ਜੇ (eap->cmdidx == CMD_final && !vim9script)
    {
	// In legacy Vim script ":final" is short for ":finally".
	ex_finally(eap);
	ਵਾਪਸ;
    }
    ਜੇ (eap->cmdidx == CMD_let && vim9script)
    {
	emsg(_(e_cannot_use_let_ਵਿੱਚ_vim9_script));
	ਵਾਪਸ;
    }

    ਜੇ (eap->cmdidx == CMD_const)
	flags |= ਸੌਂਪੋ_CONST;
    ਨਹੀਂਤਾਂ ਜੇ (eap->cmdidx == CMD_final)
	flags |= ਸੌਂਪੋ_FINAL;

    // Vim9 assignment without ":let", ":const" or ":final"
    ਜੇ (eap->arg == eap->cmd)
	flags |= ਸੌਂਪੋ_NO_DECL;

    argend = skip_var_list(arg, TRUE, &var_ਗਿਣਤੀ, &semicolon, FALSE);
    ਜੇ (argend == ਨਿਰਾ)
	ਵਾਪਸ;
    ਵਿਅੰਜਕ = skipwhite(argend);
    concat = ਵਿਅੰਜਕ[0] == '.'
	&& ((ਵਿਅੰਜਕ[1] == '=' && ਵਿੱਚ_old_script(2))
		|| (ਵਿਅੰਜਕ[1] == '.' && ਵਿਅੰਜਕ[2] == '='));
    has_ਸੌਂਪੋ =  *ਵਿਅੰਜਕ == '=' || (vim_strchr((char_u *)"+-*/%", *ਵਿਅੰਜਕ) != ਨਿਰਾ
							    && ਵਿਅੰਜਕ[1] == '=');
    ਜੇ (!has_ਸੌਂਪੋ && !concat)
    {
	// ":let" without "=": list variables
	ਜੇ (*arg == '[')
	    emsg(_(e_invalid_ਦਲੀਲ));
	ਨਹੀਂਤਾਂ ਜੇ (ਵਿਅੰਜਕ[0] == '.' && ਵਿਅੰਜਕ[1] == '=')
	    emsg(_(e_dot_equal_not_supported_with_script_ਸੰਸਕਰਣ_two));
	ਨਹੀਂਤਾਂ ਜੇ (!ਸਿਰੇ_excmd2(eap->cmd, arg))
	{
	    ਜੇ (vim9script)
	    {
		ਜੇ (!ਸਿਰੇ_excmd2(eap->cmd, skipwhite(argend)))
		    semsg(_(e_trailing_characters_str), argend);
		ਨਹੀਂਤਾਂ
		    // Vim9 declaration ":var name: type"
		    arg = vim9_declare_scriptvar(eap, arg);
	    }
	    ਨਹੀਂਤਾਂ
	    {
		// ":let var1 var2" - list values
		arg = list_arg_vars(eap, arg, &ਪਹਿਲਾ);
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (!eap->skip)
	{
	    // ":let"
	    list_glob_vars(&ਪਹਿਲਾ);
	    list_buf_vars(&ਪਹਿਲਾ);
	    list_win_vars(&ਪਹਿਲਾ);
	    list_tab_vars(&ਪਹਿਲਾ);
	    list_script_vars(&ਪਹਿਲਾ);
	    list_func_vars(&ਪਹਿਲਾ);
	    list_vim_vars(&ਪਹਿਲਾ);
	}
	ਸੈੱਟ_nextcmd(eap, arg);
	ਵਾਪਸ;
    }

    ਜੇ (ਵਿਅੰਜਕ[0] == '=' && ਵਿਅੰਜਕ[1] == '<' && ਵਿਅੰਜਕ[2] == '<')
    {
	list_T	*l = ਨਿਰਾ;
	ਲੰਮਾ	cur_lnum = SOURCING_LNUM;

	// :let text =<< [trim] [eval] END
	// :var text =<< [trim] [eval] END
	ਜੇ (vim9script && !eap->skip && (!VIM_ISWHITE(ਵਿਅੰਜਕ[-1])
						 || !IS_WHITE_OR_NUL(ਵਿਅੰਜਕ[3])))
	    semsg(_(e_white_space_required_before_and_after_str_at_str),
								  "=<<", ਵਿਅੰਜਕ);
	ਨਹੀਂਤਾਂ
	    l = heredoc_get(eap, ਵਿਅੰਜਕ + 3, FALSE, FALSE);

	ਜੇ (l != ਨਿਰਾ)
	{
	    rettv_list_ਸੈੱਟ(&rettv, l);
	    ਜੇ (!eap->skip)
	    {
		// errors are for the assignment, not the end marker
		SOURCING_LNUM = cur_lnum;
		op[0] = '=';
		op[1] = NUL;
		(ਖਾਲੀ)ex_let_vars(eap->arg, &rettv, FALSE, semicolon, var_ਗਿਣਤੀ,
								flags, op);
	    }
	    clear_tv(&rettv);
	}
	ਵਾਪਸ;
    }

    evalarg_T   evalarg;
    ਪੂਰਨ	    len = 1;

    CLEAR_ਖੇਤਰ(rettv);

    ਪੂਰਨ cur_lnum;

    op[0] = '=';
    op[1] = NUL;
    ਜੇ (*ਵਿਅੰਜਕ != '=')
    {
	ਜੇ (vim9script && (flags & ਸੌਂਪੋ_NO_DECL) == 0)
	{
	    // +=, /=, etc. require an existing variable
	    semsg(_(e_cannot_use_operator_on_new_variable_str), eap->arg);
	}
	ਨਹੀਂਤਾਂ ਜੇ (vim_strchr((char_u *)"+-*/%.", *ਵਿਅੰਜਕ) != ਨਿਰਾ)
	{
	    op[0] = *ਵਿਅੰਜਕ;   // +=, -=, *=, /=, %= or .=
	    ++len;
	    ਜੇ (ਵਿਅੰਜਕ[0] == '.' && ਵਿਅੰਜਕ[1] == '.') // ..=
	    {
		++ਵਿਅੰਜਕ;
		++len;
	    }
	}
	ਵਿਅੰਜਕ += 2;
    }
    ਨਹੀਂਤਾਂ
	++ਵਿਅੰਜਕ;

    ਜੇ (vim9script && !eap->skip && (!VIM_ISWHITE(*argend)
					       || !IS_WHITE_OR_NUL(*ਵਿਅੰਜਕ)))
    {
	vim_strncpy(op, ਵਿਅੰਜਕ - len, len);
	semsg(_(e_white_space_required_before_and_after_str_at_str),
							       op, argend);
    }

    ਜੇ (eap->skip)
	++emsg_skip;
    fill_evalarg_from_eap(&evalarg, eap, eap->skip);
    ਵਿਅੰਜਕ = skipwhite_and_linebreak(ਵਿਅੰਜਕ, &evalarg);
    cur_lnum = SOURCING_LNUM;
    ਪੂਰਨ eval_res = eval0(ਵਿਅੰਜਕ, &rettv, eap, &evalarg);
    ਜੇ (eap->skip)
	--emsg_skip;
    clear_evalarg(&evalarg, eap);

    // Restore the line number so that any type error is given for the
    // declaration, not the expression.
    SOURCING_LNUM = cur_lnum;

    ਜੇ (!eap->skip && eval_res != FAIL)
	(ਖਾਲੀ)ex_let_vars(eap->arg, &rettv, FALSE, semicolon, var_ਗਿਣਤੀ,
								    flags, op);
    ਜੇ (eval_res != FAIL)
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
    ਪੂਰਨ
ex_let_vars(
    char_u	*arg_ਸ਼ੁਰੂ,
    typval_T	*tv,
    ਪੂਰਨ		copy,		// copy values from "tv", don't move
    ਪੂਰਨ		semicolon,	// from skip_var_list()
    ਪੂਰਨ		var_ਗਿਣਤੀ,	// from skip_var_list()
    ਪੂਰਨ		flags,		// ASSIGN_FINAL, ASSIGN_CONST, etc.
    char_u	*op)
{
    char_u	*arg = arg_ਸ਼ੁਰੂ;
    list_T	*l;
    tuple_T	*tuple = ਨਿਰਾ;
    ਪੂਰਨ		i;
    ਪੂਰਨ		var_idx = 0;
    listitem_T	*ਵਸਤੂ = ਨਿਰਾ;
    typval_T	ltv;
    ਪੂਰਨ		is_list = tv->v_type == VAR_LIST;
    ਪੂਰਨ		idx;

    ਜੇ (tv->v_type == VAR_VOID)
    {
	emsg(_(e_cannot_use_void_ਮੁੱਲ));
	ਵਾਪਸ FAIL;
    }
    ਜੇ (*arg != '[')
    {
	// ":let var = expr" or ":for var in list"
	ਜੇ (ex_let_one(arg, tv, copy, flags, op, op, var_idx) == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;
	ਵਾਪਸ OK;
    }

    // ":let [v1, v2] = list" or ":for [v1, v2] in listlist"
    // or
    // ":let [v1, v2] = tuple" or ":for [v1, v2] in tupletuple"
    ਜੇ (tv->v_type != VAR_LIST && tv->v_type != VAR_TUPLE)
    {
	emsg(_(e_list_or_tuple_required));
	ਵਾਪਸ FAIL;
    }
    ਜੇ (is_list)
    {
	l = tv->vval.v_list;
	ਜੇ (l == ਨਿਰਾ)
	{
	    emsg(_(e_list_required));
	    ਵਾਪਸ FAIL;
	}
	i = list_len(l);
    }
    ਨਹੀਂਤਾਂ
    {
	tuple = tv->vval.v_tuple;
	ਜੇ (tuple == ਨਿਰਾ)
	{
	    emsg(_(e_tuple_required));
	    ਵਾਪਸ FAIL;
	}
	i = tuple_len(tuple);
    }

    ਜੇ (semicolon == 0 && var_ਗਿਣਤੀ < i)
    {
	emsg(_(is_list ? e_less_targets_than_list_ਚੀਜ਼ਾਂ
					: e_less_targets_than_tuple_ਚੀਜ਼ਾਂ));
	ਵਾਪਸ FAIL;
    }
    ਜੇ (var_ਗਿਣਤੀ - semicolon > i)
    {
	emsg(_(is_list ? e_ਹੋਰ_targets_than_list_ਚੀਜ਼ਾਂ
					: e_ਹੋਰ_targets_than_tuple_ਚੀਜ਼ਾਂ));
	ਵਾਪਸ FAIL;
    }

    ਜੇ (is_list)
    {
	CHECK_LIST_MATERIALIZE(l);
	ਵਸਤੂ = l->lv_ਪਹਿਲਾ;
    }
    ਨਹੀਂਤਾਂ
	idx = 0;

    ਜਦੋਂ (*arg != ']')
    {
	arg = skipwhite(arg + 1);
	++var_idx;
	arg = ex_let_one(arg, is_list ? &ਵਸਤੂ->li_tv : TUPLE_ਵਸਤੂ(tuple, idx),
			 TRUE, flags | ਸੌਂਪੋ_UNPACK, (char_u *)",;]", op,
			 var_idx);
	ਜੇ (is_list)
	    ਵਸਤੂ = ਵਸਤੂ->li_ਅਗਲਾ;
	ਨਹੀਂਤਾਂ
	    idx++;
	ਜੇ (arg == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;

	arg = skipwhite(arg);
	ਜੇ (*arg == ';')
	{
	    // Put the rest of the list or tuple (may be empty) in the var
	    // after ';'.  Create a new list or tuple for this.
	    ਜੇ (is_list)
	    {
		// Put the rest of the list (may be empty) in the var
		// after ';'.  Create a new list for this.
		l = list_alloc();
		ਜੇ (l == ਨਿਰਾ)
		    ਵਾਪਸ FAIL;

		// list
		ਜਦੋਂ (ਵਸਤੂ != ਨਿਰਾ)
		{
		    list_append_tv(l, &ਵਸਤੂ->li_tv);
		    ਵਸਤੂ = ਵਸਤੂ->li_ਅਗਲਾ;
		}

		ltv.v_type = VAR_LIST;
		ltv.v_ਤਾਲਾ = 0;
		ltv.vval.v_list = l;
		l->lv_refcount = 1;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		tuple_T	*new_tuple = tuple_alloc();
		ਜੇ (new_tuple == ਨਿਰਾ)
		    ਵਾਪਸ FAIL;

		// Put the rest of the tuple (may be empty) in the var
		// after ';'.  Create a new tuple for this.
		ਜਦੋਂ (idx < TUPLE_LEN(tuple))
		{
		    typval_T    new_tv;

		    copy_tv(TUPLE_ਵਸਤੂ(tuple, idx), &new_tv);
		    ਜੇ (tuple_append_tv(new_tuple, &new_tv) == FAIL)
		    {
			clear_tv(&new_tv);
			tuple_unref(new_tuple);
			ਵਾਪਸ FAIL;
		    }
		    idx++;
		}

		ltv.v_type = VAR_TUPLE;
		ltv.v_ਤਾਲਾ = 0;
		ltv.vval.v_tuple = new_tuple;
		new_tuple->tv_refcount = 1;
	    }

	    ++var_idx;
	    arg = ex_let_one(skipwhite(arg + 1), &ltv, FALSE,
			flags | ਸੌਂਪੋ_UNPACK, (char_u *)"]", op, var_idx);
	    clear_tv(&ltv);
	    ਜੇ (arg == ਨਿਰਾ)
		ਵਾਪਸ FAIL;
	    ਰੋਕੋ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (*arg != ',' && *arg != ']')
	{
	    ਅੰਦਰੂਨੀ_error("ex_let_vars()");
	    ਵਾਪਸ FAIL;
	}
    }

    ਵਾਪਸ OK;
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
    ਪੂਰਨ		include_type,
    ਪੂਰਨ		*var_ਗਿਣਤੀ,
    ਪੂਰਨ		*semicolon,
    ਪੂਰਨ		silent)
{
    char_u	*p, *s;

    ਜੇ (*arg == '[')
    {
	// "[var, var]": find the matching ']'.
	p = arg;
	ਲਈ (;;)
	{
	    p = skipwhite(p + 1);	// skip whites after '[', ';' or ','
	    s = skip_var_one(p, include_type);
	    ਜੇ (s == p)
	    {
		ਜੇ (!silent)
		    semsg(_(e_invalid_ਦਲੀਲ_str), p);
		ਵਾਪਸ ਨਿਰਾ;
	    }
	    ++*var_ਗਿਣਤੀ;

	    p = skipwhite(s);
	    ਜੇ (*p == ']')
		ਰੋਕੋ;
	    ਨਹੀਂਤਾਂ ਜੇ (*p == ';')
	    {
		ਜੇ (*semicolon == 1)
		{
		    ਜੇ (!silent)
			emsg(_(e_double_semicolon_ਵਿੱਚ_list_ਦਾ_variables));
		    ਵਾਪਸ ਨਿਰਾ;
		}
		*semicolon = 1;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (*p != ',')
	    {
		ਜੇ (!silent)
		    semsg(_(e_invalid_ਦਲੀਲ_str), p);
		ਵਾਪਸ ਨਿਰਾ;
	    }
	}
	ਵਾਪਸ p + 1;
    }

    ਵਾਪਸ skip_var_one(arg, include_type);
}

/*
 * Skip one (assignable) variable name, including @r, $VAR, &option, d.key,
 * l[idx].
 * In Vim9 script also skip over ": type" if "include_type" is TRUE.
 */
    char_u *
skip_var_one(char_u *arg, ਪੂਰਨ include_type)
{
    char_u	*ਸਿਰਾ;
    ਪੂਰਨ		vim9 = ਵਿੱਚ_vim9script();

    ਜੇ (*arg == '@' && arg[1] != NUL)
	ਵਾਪਸ arg + 2;

    // termcap option name may have non-alpha characters
    ਜੇ (STRNCMP(arg, "&t_", 3) == 0 && arg[3] != NUL && arg[4] != NUL)
	ਵਾਪਸ arg + 5;

    ਸਿਰਾ = ਲੱਭੋ_name_ਸਿਰਾ(*arg == '$' || *arg == '&' ? arg + 1 : arg,
				   ਨਿਰਾ, ਨਿਰਾ, FNE_INCL_BR | FNE_CHECK_ਸ਼ੁਰੂ);

    // "a: type" is declaring variable "a" with a type, not "a:".
    // Same for "s: type".
    ਜੇ (vim9 && ਸਿਰਾ == arg + 2 && ਸਿਰਾ[-1] == ':')
	--ਸਿਰਾ;

    ਜੇ (include_type && vim9)
    {
	ਜੇ (*skipwhite(ਸਿਰਾ) == ':')
	    ਸਿਰਾ = skip_type(skipwhite(skipwhite(ਸਿਰਾ) + 1), FALSE);
    }
    ਵਾਪਸ ਸਿਰਾ;
}

/*
 * List variables for hashtab "ht" with prefix "prefix".
 * If "empty" is TRUE also list NULL strings as empty strings.
 */
    ਖਾਲੀ
list_hashtable_vars(
    hashtab_T	*ht,
    ਅੱਖਰ	*ਅਗੇਤਰ,
    ਪੂਰਨ		ਖਾਲੀ_ਥਾਵਾਂ_2,
    ਪੂਰਨ		*ਪਹਿਲਾ)
{
    hashitem_T	*hi;
    dictitem_T	*di;
    ਪੂਰਨ		todo;
    char_u	buf[IOSIZE];

    ਪੂਰਨ save_ht_flags = ht->ht_flags;
    ht->ht_flags |= HTFLAGS_FROZEN;

    todo = (ਪੂਰਨ)ht->ht_ਵਰਤਿਆ;
    ਲਈ (hi = ht->ht_array; todo > 0 && !got_int; ++hi)
    {
	ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	{
	    --todo;
	    di = HI2DI(hi);

	    // apply :filter /pat/ to variable name
	    vim_strncpy((char_u *)buf, (char_u *)ਅਗੇਤਰ, IOSIZE - 1);
	    vim_strcat((char_u *)buf, di->di_ਕੁੰਜੀ, IOSIZE);
	    ਜੇ (ਸੁਨੇਹਾ_filtered(buf))
		ਜਾਰੀ;

	    ਜੇ (ਖਾਲੀ_ਥਾਵਾਂ_2 || di->di_tv.v_type != VAR_STRING
					   || di->di_tv.vval.v_string != ਨਿਰਾ)
		list_one_var(di, ਅਗੇਤਰ, ਪਹਿਲਾ);
	}
    }

    ht->ht_flags = save_ht_flags;
}

/*
 * List global variables.
 */
    ਸਥਿਰ ਖਾਲੀ
list_glob_vars(ਪੂਰਨ *ਪਹਿਲਾ)
{
    list_hashtable_vars(&globvarht, "", TRUE, ਪਹਿਲਾ);
}

/*
 * List buffer variables.
 */
    ਸਥਿਰ ਖਾਲੀ
list_buf_vars(ਪੂਰਨ *ਪਹਿਲਾ)
{
    list_hashtable_vars(&curbuf->b_vars->dv_hashtab, "b:", TRUE, ਪਹਿਲਾ);
}

/*
 * List window variables.
 */
    ਸਥਿਰ ਖਾਲੀ
list_win_vars(ਪੂਰਨ *ਪਹਿਲਾ)
{
    list_hashtable_vars(&curwin->w_vars->dv_hashtab, "w:", TRUE, ਪਹਿਲਾ);
}

/*
 * List tab page variables.
 */
    ਸਥਿਰ ਖਾਲੀ
list_tab_vars(ਪੂਰਨ *ਪਹਿਲਾ)
{
    list_hashtable_vars(&curtab->tp_vars->dv_hashtab, "t:", TRUE, ਪਹਿਲਾ);
}

/*
 * List variables in "arg".
 */
    ਸਥਿਰ char_u *
list_arg_vars(exarg_T *eap, char_u *arg, ਪੂਰਨ *ਪਹਿਲਾ)
{
    ਪੂਰਨ		error = FALSE;
    ਪੂਰਨ		len;
    char_u	*name;
    char_u	*name_ਸ਼ੁਰੂ;
    char_u	*arg_subsc;
    char_u	*tofree;
    typval_T    tv;

    ਜਦੋਂ (!ਸਿਰੇ_excmd2(eap->cmd, arg) && !got_int)
    {
	ਜੇ (error || eap->skip)
	{
	    arg = ਲੱਭੋ_name_ਸਿਰਾ(arg, ਨਿਰਾ, ਨਿਰਾ, FNE_INCL_BR | FNE_CHECK_ਸ਼ੁਰੂ);
	    ਜੇ (!VIM_ISWHITE(*arg) && !ਸਿਰੇ_excmd(*arg))
	    {
		emsg_severe = TRUE;
		ਜੇ (!did_emsg)
		    semsg(_(e_trailing_characters_str), arg);
		ਰੋਕੋ;
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    // get_name_len() takes care of expanding curly braces
	    name_ਸ਼ੁਰੂ = name = arg;
	    len = get_name_len(&arg, &tofree, TRUE, TRUE);
	    ਜੇ (len <= 0)
	    {
		// This is mainly to keep test 49 working: when expanding
		// curly braces fails overrule the exception error message.
		ਜੇ (len < 0 && !aborting())
		{
		    emsg_severe = TRUE;
		    semsg(_(e_invalid_ਦਲੀਲ_str), arg);
		    ਰੋਕੋ;
		}
		error = TRUE;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		arg = skipwhite(arg);
		ਜੇ (tofree != ਨਿਰਾ)
		    name = tofree;
		ਜੇ (eval_variable(name, len, 0, &tv, ਨਿਰਾ,
						     EVAL_VAR_VERBOSE) == FAIL)
		    error = TRUE;
		ਨਹੀਂਤਾਂ
		{
		    // handle d.key, l[idx], f(expr)
		    arg_subsc = arg;
		    ਜੇ (handle_subscript(&arg, name_ਸ਼ੁਰੂ, &tv,
					      &EVALARG_EVALUATE, TRUE) == FAIL)
			error = TRUE;
		    ਨਹੀਂਤਾਂ
		    {
			ਜੇ (arg == arg_subsc && len == 2 && name[1] == ':')
			{
			    ਚੋਣ (*name)
			    {
				ਹਾਲਤ 'g': list_glob_vars(ਪਹਿਲਾ); ਰੋਕੋ;
				ਹਾਲਤ 'b': list_buf_vars(ਪਹਿਲਾ); ਰੋਕੋ;
				ਹਾਲਤ 'w': list_win_vars(ਪਹਿਲਾ); ਰੋਕੋ;
				ਹਾਲਤ 't': list_tab_vars(ਪਹਿਲਾ); ਰੋਕੋ;
				ਹਾਲਤ 'v': list_vim_vars(ਪਹਿਲਾ); ਰੋਕੋ;
				ਹਾਲਤ 's': list_script_vars(ਪਹਿਲਾ); ਰੋਕੋ;
				ਹਾਲਤ 'l': list_func_vars(ਪਹਿਲਾ); ਰੋਕੋ;
				ਮੂਲ:
					  semsg(_(e_cant_list_variables_for_str), name);
			    }
			}
			ਨਹੀਂਤਾਂ
			{
			    char_u	numbuf[NUMBUFLEN];
			    char_u	*tf;
			    ਪੂਰਨ		c;
			    char_u	*s;

			    s = echo_string(&tv, &tf, numbuf, 0);
			    c = *arg;
			    *arg = NUL;
			    list_one_var_a("",
				    arg == arg_subsc ? name : name_ਸ਼ੁਰੂ,
				    tv.v_type,
				    s == ਨਿਰਾ ? (char_u *)"" : s,
				    ਪਹਿਲਾ);
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

    ਵਾਪਸ arg;
}

/*
 * Set an environment variable, part of ex_let_one().
 */
    ਸਥਿਰ char_u *
ex_let_env(
    char_u	*arg,
    typval_T	*tv,
    ਪੂਰਨ		flags,
    char_u	*endchars,
    char_u	*op)
{
    char_u	*arg_ਸਿਰਾ = ਨਿਰਾ;
    char_u	*name;
    ਪੂਰਨ		len;

    ਜੇ ((flags & (ਸੌਂਪੋ_CONST | ਸੌਂਪੋ_FINAL))
					 && (flags & ਸੌਂਪੋ_FOR_LOOP) == 0)
    {
	emsg(_(e_cannot_ਤਾਲਾ_environment_variable));
	ਵਾਪਸ ਨਿਰਾ;
    }

    // Find the end of the name.
    ++arg;
    name = arg;
    len = get_env_len(&arg);
    ਜੇ (len == 0)
	semsg(_(e_invalid_ਦਲੀਲ_str), name - 1);
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (op != ਨਿਰਾ && vim_strchr((char_u *)"+-*/%", *op) != ਨਿਰਾ)
	    semsg(_(e_wrong_variable_type_for_str_equal), op);
	ਨਹੀਂਤਾਂ ਜੇ (endchars != ਨਿਰਾ
			      && vim_strchr(endchars, *skipwhite(arg)) == ਨਿਰਾ)
	    emsg(_(e_unexpected_characters_ਵਿੱਚ_let));
	ਨਹੀਂਤਾਂ ਜੇ (!check_secure() && !check_restricted())
	{
	    char_u	*tofree = ਨਿਰਾ;
	    ਪੂਰਨ		c1 = name[len];
	    char_u	*p;

	    name[len] = NUL;
	    p = tv_get_string_chk(tv);
	    ਜੇ (p != ਨਿਰਾ && op != ਨਿਰਾ && *op == '.')
	    {
		ਪੂਰਨ	mustfree = FALSE;
		char_u  *s = vim_getenv(name, &mustfree);

		ਜੇ (s != ਨਿਰਾ)
		{
		    p = tofree = concat_str(s, p);
		    ਜੇ (mustfree)
			vim_free(s);
		}
	    }
	    ਜੇ (p != ਨਿਰਾ)
	    {
		vim_setenv_ext(name, p);
		arg_ਸਿਰਾ = arg;
	    }
	    name[len] = c1;
	    vim_free(tofree);
	}
    }
    ਵਾਪਸ arg_ਸਿਰਾ;
}

/*
 * Set an option, part of ex_let_one().
 */
    ਸਥਿਰ char_u *
ex_let_option(
    char_u	*arg,
    typval_T	*tv,
    ਪੂਰਨ		flags,
    char_u	*endchars,
    char_u	*op)
{
    char_u	*p;
    ਪੂਰਨ		scope;
    char_u	*arg_ਸਿਰਾ = ਨਿਰਾ;

    ਜੇ ((flags & (ਸੌਂਪੋ_CONST | ਸੌਂਪੋ_FINAL))
					 && (flags & ਸੌਂਪੋ_FOR_LOOP) == 0)
    {
	emsg(_(e_cannot_ਤਾਲਾ_option));
	ਵਾਪਸ ਨਿਰਾ;
    }

    // Find the end of the name.
    p = ਲੱਭੋ_option_ਸਿਰਾ(&arg, &scope);
    ਜੇ (p == ਨਿਰਾ || (endchars != ਨਿਰਾ
			       && vim_strchr(endchars, *skipwhite(p)) == ਨਿਰਾ))
    {
	emsg(_(e_unexpected_characters_ਵਿੱਚ_let));
	ਵਾਪਸ ਨਿਰਾ;
    }

    ਪੂਰਨ		c1;
    ਲੰਮਾ	n = 0;
    getoption_T	opt_type;
    ਲੰਮਾ	numval;
    char_u	*stringval = ਨਿਰਾ;
    char_u	*s = ਨਿਰਾ;
    ਪੂਰਨ		failed = FALSE;
    ਪੂਰਨ		opt_p_flags;
    char_u	*tofree = ਨਿਰਾ;
    char_u	numbuf[NUMBUFLEN];

    c1 = *p;
    *p = NUL;

    opt_type = get_option_ਮੁੱਲ(arg, &numval, &stringval, &opt_p_flags, scope);
    ਜੇ (opt_type == gov_unknown && arg[0] != 't' && arg[1] != '_')
    {
	semsg(_(e_unknown_option_str_2), arg);
	ਜਾਓ theend;
    }
    ਜੇ (op != ਨਿਰਾ && *op != '='
	&& (((opt_type == gov_bool || opt_type == gov_ਸੰਖਿਆ) && *op == '.')
	    || (opt_type == gov_string && *op != '.')))
    {
	semsg(_(e_wrong_variable_type_for_str_equal), op);
	ਜਾਓ theend;
    }

    ਜੇ ((opt_type == gov_bool
		|| opt_type == gov_ਸੰਖਿਆ
		|| opt_type == gov_hidden_bool
		|| opt_type == gov_hidden_ਸੰਖਿਆ)
		     && (tv->v_type != VAR_STRING || !ਵਿੱਚ_vim9script()))
    {
	ਜੇ (opt_type == gov_bool || opt_type == gov_hidden_bool)
	    // bool, possibly hidden
	    n = (ਲੰਮਾ)tv_get_bool_chk(tv, &failed);
	ਨਹੀਂਤਾਂ
	    // number, possibly hidden
	    n = (ਲੰਮਾ)tv_get_ਸੰਖਿਆ_chk(tv, &failed);
	ਜੇ (failed)
	    ਜਾਓ theend;
    }

    ਜੇ ((opt_p_flags & P_FUNC) && (tv->v_type == VAR_PARTIAL
						    || tv->v_type == VAR_FUNC))
    {
	// If the option can be set to a function reference or a lambda
	// and the passed value is a function reference, then convert it to
	// the name (string) of the function reference.
	s = tv2string(tv, &tofree, numbuf, 0);
	ਜੇ (s == ਨਿਰਾ)
	    ਜਾਓ theend;
    }
    // Avoid setting a string option to the text "v:false" or similar.
    // In Vim9 script also don't convert a number to string.
    ਨਹੀਂਤਾਂ ਜੇ (tv->v_type != VAR_BOOL && tv->v_type != VAR_SPECIAL
		     && (!ਵਿੱਚ_vim9script() || tv->v_type != VAR_ਸੰਖਿਆ))
    {
	s = tv_get_string_chk(tv);
	ਜੇ (s == ਨਿਰਾ)
	    ਜਾਓ theend;
    }
    ਨਹੀਂਤਾਂ ਜੇ (opt_type == gov_string || opt_type == gov_hidden_string)
    {
	emsg(_(e_string_required));
	ਜਾਓ theend;
    }

    ਜੇ (op != ਨਿਰਾ && *op != '=')
    {
	// number, in legacy script also bool
	ਜੇ (opt_type == gov_ਸੰਖਿਆ
				 || (opt_type == gov_bool && !ਵਿੱਚ_vim9script()))
	{
	    ਚੋਣ (*op)
	    {
		ਹਾਲਤ '+': n = numval + n; ਰੋਕੋ;
		ਹਾਲਤ '-': n = numval - n; ਰੋਕੋ;
		ਹਾਲਤ '*': n = numval * n; ਰੋਕੋ;
		ਹਾਲਤ '/': n = (ਲੰਮਾ)ਅੰਕ_divide(numval, n, &failed); ਰੋਕੋ;
		ਹਾਲਤ '%': n = (ਲੰਮਾ)ਅੰਕ_modulus(numval, n, &failed); ਰੋਕੋ;
	    }
	    s = ਨਿਰਾ;
	    ਜੇ (failed)
		ਜਾਓ theend;
	}
	ਨਹੀਂਤਾਂ ਜੇ (opt_type == gov_string && stringval != ਨਿਰਾ && s != ਨਿਰਾ)
	{
	    // string
	    s = concat_str(stringval, s);
	    vim_free(stringval);
	    stringval = s;
	}
    }

    ਅੱਖਰ *err = ਸੈੱਟ_option_ਮੁੱਲ(arg, n, s, scope);
    arg_ਸਿਰਾ = p;
    ਜੇ (err != ਨਿਰਾ)
	emsg(_(err));

theend:
    *p = c1;
    vim_free(stringval);
    vim_free(tofree);
    ਵਾਪਸ arg_ਸਿਰਾ;
}

/*
 * Set a register, part of ex_let_one().
 */
    ਸਥਿਰ char_u *
ex_let_ਰਜਿਸਟਰ(
    char_u	*arg,
    typval_T	*tv,
    ਪੂਰਨ		flags,
    char_u	*endchars,
    char_u	*op)
{
    char_u	*arg_ਸਿਰਾ = ਨਿਰਾ;

    ਜੇ ((flags & (ਸੌਂਪੋ_CONST | ਸੌਂਪੋ_FINAL))
					 && (flags & ਸੌਂਪੋ_FOR_LOOP) == 0)
    {
	emsg(_(e_cannot_ਤਾਲਾ_ਰਜਿਸਟਰ));
	ਵਾਪਸ ਨਿਰਾ;
    }
    ++arg;
    ਜੇ (op != ਨਿਰਾ && vim_strchr((char_u *)"+-*/%", *op) != ਨਿਰਾ)
	semsg(_(e_wrong_variable_type_for_str_equal), op);
    ਨਹੀਂਤਾਂ ਜੇ (endchars != ਨਿਰਾ
			  && vim_strchr(endchars, *skipwhite(arg + 1)) == ਨਿਰਾ)
	emsg(_(e_unexpected_characters_ਵਿੱਚ_let));
    ਨਹੀਂਤਾਂ
    {
	char_u	*ptofree = ਨਿਰਾ;
	char_u	*p;

	p = tv_get_string_chk(tv);
	ਜੇ (p != ਨਿਰਾ && op != ਨਿਰਾ && *op == '.')
	{
	    char_u  *s = get_reg_contents(*arg == '@'
						  ? '"' : *arg, GREG_ਵਿਅੰਜਕ_SRC);

	    ਜੇ (s != ਨਿਰਾ)
	    {
		p = ptofree = concat_str(s, p);
		vim_free(s);
	    }
	}
	ਜੇ (p != ਨਿਰਾ)
	{
	    write_reg_contents(*arg == '@' ? '"' : *arg, p, -1, FALSE);
	    arg_ਸਿਰਾ = arg + 1;
	}
	vim_free(ptofree);
    }
    ਵਾਪਸ arg_ਸਿਰਾ;
}

/*
 * Set one item of ":let var = expr" or ":let [v1, v2] = list" to its value.
 * Returns a pointer to the char just after the var name.
 * Returns NULL if there is an error.
 */
    ਸਥਿਰ char_u *
ex_let_one(
    char_u	*arg,		// points to variable name
    typval_T	*tv,		// value to assign to variable
    ਪੂਰਨ		copy,		// copy value from "tv"
    ਪੂਰਨ		flags,		// ASSIGN_CONST, ASSIGN_FINAL, etc.
    char_u	*endchars,	// valid chars after variable name  or NULL
    char_u	*op,		// "+", "-", "."  or NULL
    ਪੂਰਨ		var_idx)	// variable index for "let [a, b] = list"
{
    char_u	*arg_ਸਿਰਾ = ਨਿਰਾ;

    ਜੇ (ਵਿੱਚ_vim9script() && (flags & (ਸੌਂਪੋ_NO_DECL | ਸੌਂਪੋ_DECL)) == 0
			&& (flags & (ਸੌਂਪੋ_CONST | ਸੌਂਪੋ_FINAL)) == 0
				  && vim_strchr((char_u *)"$@&", *arg) != ਨਿਰਾ)
    {
	vim9_declare_error(arg);
	ਵਾਪਸ ਨਿਰਾ;
    }

    ਜੇ (check_typval_is_ਮੁੱਲ(tv) == FAIL)
	ਵਾਪਸ ਨਿਰਾ;

    // In Vim9 script an environment variable and a register only take a
    // String.  "@#" also takes a buffer number.
    ਜੇ (ਵਿੱਚ_vim9script() && (*arg == '$' || *arg == '@')
	    && !(*arg == '@' && arg[1] == '#' && tv->v_type == VAR_ਸੰਖਿਆ
					       && (op == ਨਿਰਾ || *op == '=')))
    {
	where_T	where = WHERE_INIT;

	ਜੇ (check_typval_type(&t_string, tv, where) == FAIL)
	    ਵਾਪਸ ਨਿਰਾ;
    }

    ਜੇ (*arg == '$')
    {
	// ":let $VAR = expr": Set environment variable.
	ਵਾਪਸ ex_let_env(arg, tv, flags, endchars, op);
    }
    ਨਹੀਂਤਾਂ ਜੇ (*arg == '&')
    {
	// ":let &option = expr": Set option value.
	// ":let &l:option = expr": Set local option value.
	// ":let &g:option = expr": Set global option value.
	// ":for &ts in range(8)": Set option value for for loop
	ਵਾਪਸ ex_let_option(arg, tv, flags, endchars, op);
    }
    ਨਹੀਂਤਾਂ ਜੇ (*arg == '@')
    {
	// ":let @r = expr": Set register contents.
	ਵਾਪਸ ex_let_ਰਜਿਸਟਰ(arg, tv, flags, endchars, op);
    }
    ਨਹੀਂਤਾਂ ਜੇ (eval_isnamec1(*arg) || *arg == '{')
    {
	lval_T	lv;
	char_u	*p;
	ਪੂਰਨ	lval_flags = (flags & (ਸੌਂਪੋ_NO_DECL | ਸੌਂਪੋ_DECL))
							     ? GLV_NO_DECL : 0;
	lval_flags |= (flags & ਸੌਂਪੋ_FOR_LOOP) ? GLV_FOR_LOOP : 0;
	ਜੇ (op != ਨਿਰਾ && *op != '=')
	    lval_flags |= GLV_ਸੌਂਪੋ_WITH_OP;

	// ":let var = expr": Set internal variable.
	// ":let var: type = expr": Set internal variable with type.
	// ":let {expr} = expr": Idem, name made with curly braces
	p = get_lval(arg, tv, &lv, FALSE, FALSE, lval_flags, FNE_CHECK_ਸ਼ੁਰੂ);
	ਜੇ (p != ਨਿਰਾ && lv.ll_name != ਨਿਰਾ)
	{
	    ਜੇ (endchars != ਨਿਰਾ && vim_strchr(endchars,
					   *skipwhite(lv.ll_name_ਸਿਰਾ)) == ਨਿਰਾ)
	    {
		emsg(_(e_unexpected_characters_ਵਿੱਚ_let));
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਸੈੱਟ_var_lval(&lv, p, tv, copy, flags, op, var_idx);
		arg_ਸਿਰਾ = lv.ll_name_ਸਿਰਾ;
	    }
	}
	clear_lval(&lv);
    }
    ਨਹੀਂਤਾਂ
	semsg(_(e_invalid_ਦਲੀਲ_str), arg);

    ਵਾਪਸ arg_ਸਿਰਾ;
}

/*
 * ":unlet[!] var1 ... " command.
 */
    ਖਾਲੀ
ex_unlet(exarg_T *eap)
{
    ex_unletlock(eap, eap->arg, 0, eap->forceit ? GLV_QUIET : 0, do_unlet_var, ਨਿਰਾ);
}

/*
 * ":lockvar" and ":unlockvar" commands
 */
    ਖਾਲੀ
ex_lockvar(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    ਪੂਰਨ		deep = 2;

    ਜੇ (eap->forceit)
	deep = -1;
    ਨਹੀਂਤਾਂ ਜੇ (vim_isdigit(*arg))
    {
	deep = getdigits(&arg);
	arg = skipwhite(arg);
    }

    ex_unletlock(eap, arg, deep, 0, do_ਤਾਲਾ_var, ਨਿਰਾ);
}

/*
 * ":unlet", ":lockvar" and ":unlockvar" are quite similar.
 * Also used for Vim9 script.  "callback" is invoked as:
 *	callback(&lv, name_end, eap, deep, cookie)
 */
    ਖਾਲੀ
ex_unletlock(
    exarg_T	*eap,
    char_u	*argstart,
    ਪੂਰਨ		deep,
    ਪੂਰਨ		glv_flags,
    ਪੂਰਨ		(*callback)(lval_T *, char_u *, exarg_T *, ਪੂਰਨ, ਖਾਲੀ *),
    ਖਾਲੀ	*cookie)
{
    char_u	*arg = argstart;
    char_u	*name_ਸਿਰਾ;
    ਪੂਰਨ		error = FALSE;
    lval_T	lv;

    ਕਰੋ
    {
	ਜੇ (*arg == '$')
	{
	    lv.ll_name = arg;
	    lv.ll_tv = ਨਿਰਾ;
	    ++arg;
	    ਜੇ (get_env_len(&arg) == 0)
	    {
		semsg(_(e_invalid_ਦਲੀਲ_str), arg - 1);
		ਵਾਪਸ;
	    }
	    ਜੇ (!error && !eap->skip
			      && callback(&lv, arg, eap, deep, cookie) == FAIL)
		error = TRUE;
	    name_ਸਿਰਾ = arg;
	}
	ਨਹੀਂਤਾਂ
	{
	    // Parse the name and find the end.
	    name_ਸਿਰਾ = get_lval(arg, ਨਿਰਾ, &lv, TRUE, eap->skip || error,
				     glv_flags | GLV_NO_DECL, FNE_CHECK_ਸ਼ੁਰੂ);
	    ਜੇ (lv.ll_name == ਨਿਰਾ)
		error = TRUE;	    // error but continue parsing
	    ਜੇ (name_ਸਿਰਾ == ਨਿਰਾ || (!VIM_ISWHITE(*name_ਸਿਰਾ)
						    && !ਸਿਰੇ_excmd(*name_ਸਿਰਾ)))
	    {
		ਜੇ (name_ਸਿਰਾ != ਨਿਰਾ)
		{
		    emsg_severe = TRUE;
		    semsg(_(e_trailing_characters_str), name_ਸਿਰਾ);
		}
		ਜੇ (!(eap->skip || error))
		    clear_lval(&lv);
		ਰੋਕੋ;
	    }

	    ਜੇ (!error && !eap->skip
			 && callback(&lv, name_ਸਿਰਾ, eap, deep, cookie) == FAIL)
		error = TRUE;

	    ਜੇ (!eap->skip)
		clear_lval(&lv);
	}

	arg = skipwhite(name_ਸਿਰਾ);
    } ਜਦੋਂ (!ਸਿਰੇ_excmd2(name_ਸਿਰਾ, arg));

    ਸੈੱਟ_nextcmd(eap, arg);
}

    ਸਥਿਰ ਪੂਰਨ
do_unlet_var(
    lval_T	*lp,
    char_u	*name_ਸਿਰਾ,
    exarg_T	*eap,
    ਪੂਰਨ		deep ਅਣਵਰਤਿਆ,
    ਖਾਲੀ	*cookie ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ		forceit = eap->forceit;
    ਪੂਰਨ		ret = OK;
    ਪੂਰਨ		cc;

    ਜੇ (lp->ll_tv == ਨਿਰਾ)
    {
	cc = *name_ਸਿਰਾ;
	*name_ਸਿਰਾ = NUL;

	// Environment variable, normal name or expanded name.
	ਜੇ (*lp->ll_name == '$')
	    vim_unsetenv_ext(lp->ll_name + 1);
	ਨਹੀਂਤਾਂ ਜੇ (do_unlet(lp->ll_name, forceit) == FAIL)
	    ret = FAIL;
	*name_ਸਿਰਾ = cc;
    }
    ਨਹੀਂਤਾਂ ਜੇ ((lp->ll_list != ਨਿਰਾ
		 && ਮੁੱਲ_check_ਤਾਲਾ(lp->ll_list->lv_ਤਾਲਾ, lp->ll_name, FALSE))
	    || (lp->ll_dict != ਨਿਰਾ
		&& ਮੁੱਲ_check_ਤਾਲਾ(lp->ll_dict->dv_ਤਾਲਾ, lp->ll_name, FALSE)))
	ਵਾਪਸ FAIL;
    ਨਹੀਂਤਾਂ ਜੇ (lp->ll_range)
	list_unlet_range(lp->ll_list, lp->ll_li, lp->ll_n1,
						    !lp->ll_empty2, lp->ll_n2);
    ਨਹੀਂਤਾਂ ਜੇ (lp->ll_list != ਨਿਰਾ)
	// unlet a List item.
	listitem_remove(lp->ll_list, lp->ll_li);
    ਨਹੀਂਤਾਂ ਜੇ (lp->ll_dict != ਨਿਰਾ)
	// unlet a Dictionary item.
	dictitem_remove(lp->ll_dict, lp->ll_di, "unlet");
    ਨਹੀਂਤਾਂ
    {
	semsg(_(e_cannot_unlet_imported_ਵਸਤੂ_str), lp->ll_name);
	ਵਾਪਸ FAIL;
    }

    ਵਾਪਸ ret;
}

/*
 * Unlet one item or a range of items from a list.
 * Return OK or FAIL.
 */
    ਖਾਲੀ
list_unlet_range(
	list_T	    *l,
	listitem_T  *li_ਪਹਿਲਾ,
	ਲੰਮਾ	    n1_arg,
	ਪੂਰਨ	    has_n2,
	ਲੰਮਾ	    n2)
{
    listitem_T  *li = li_ਪਹਿਲਾ;
    ਪੂਰਨ		n1 = n1_arg;

    // Delete a range of List items.
    li = li_ਪਹਿਲਾ;
    n1 = n1_arg;
    ਜਦੋਂ (li != ਨਿਰਾ && (!has_n2 || n2 >= n1))
    {
	listitem_T *ਅਗਲਾ = li->li_ਅਗਲਾ;

	listitem_remove(l, li);
	li = ਅਗਲਾ;
	++n1;
    }
}
/*
 * "unlet" a variable.  Return OK if it existed, FAIL if not.
 * When "forceit" is TRUE don't complain if the variable doesn't exist.
 */
    ਪੂਰਨ
do_unlet(char_u *name, ਪੂਰਨ forceit)
{
    hashtab_T	*ht;
    hashitem_T	*hi;
    char_u	*varname = ਨਿਰਾ;  // init to shut up gcc
    dict_T	*d;
    dictitem_T	*di;

    // can't :unlet a script variable in Vim9 script
    ਜੇ (ਵਿੱਚ_vim9script() && check_vim9_unlet(name) == FAIL)
	ਵਾਪਸ FAIL;

    ht = ਲੱਭੋ_var_ht(name, &varname);

    // can't :unlet a script variable in Vim9 script from a function
    ਜੇ (ht == get_script_local_ht()
	    && SCRIPT_ID_VALID(current_sctx.sc_sid)
	    && SCRIPT_ਵਸਤੂ(current_sctx.sc_sid)->sn_ਸੰਸਕਰਣ
							 == SCRIPT_ਸੰਸਕਰਣ_VIM9
	    && check_vim9_unlet(name) == FAIL)
	ਵਾਪਸ FAIL;

    ਜੇ (ht != ਨਿਰਾ && *varname != NUL)
    {
	d = get_current_funccal_dict(ht);
	ਜੇ (d == ਨਿਰਾ)
	{
	    ਜੇ (ht == &globvarht)
		d = &globvardict;
	    ਨਹੀਂਤਾਂ ਜੇ (ht == &compat_hashtab)
		d = &vimvardict;
	    ਨਹੀਂਤਾਂ
	    {
		di = ਲੱਭੋ_var_ਵਿੱਚ_ht(ht, *name, (char_u *)"", FALSE);
		d = di == ਨਿਰਾ ? ਨਿਰਾ : di->di_tv.vval.v_dict;
	    }
	    ਜੇ (d == ਨਿਰਾ)
	    {
		ਅੰਦਰੂਨੀ_error("do_unlet()");
		ਵਾਪਸ FAIL;
	    }
	}
	hi = hash_ਲੱਭੋ(ht, varname);
	ਜੇ (HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	    hi = ਲੱਭੋ_hi_ਵਿੱਚ_scoped_ht(name, &ht);
	ਜੇ (hi != ਨਿਰਾ && !HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	{
	    di = HI2DI(hi);
	    ਜੇ (var_check_fixed(di->di_flags, name, FALSE)
		    || var_check_ro(di->di_flags, name, FALSE)
		    || ਮੁੱਲ_check_ਤਾਲਾ(d->dv_ਤਾਲਾ, name, FALSE)
		    || check_hashtab_frozen(ht, "unlet"))
		ਵਾਪਸ FAIL;

	    delete_var(ht, hi);
	    ਵਾਪਸ OK;
	}
    }
    ਜੇ (forceit)
	ਵਾਪਸ OK;
    semsg(_(e_no_such_variable_str), name);
    ਵਾਪਸ FAIL;
}

    ਸਥਿਰ ਖਾਲੀ
report_lockvar_member(ਅੱਖਰ *ਸੁਨੇਹਾ, lval_T *lp)
{
    ਪੂਰਨ did_alloc = FALSE;
    char_u *vname = (char_u *)"";
    char_u *class_name = lp->ll_class != ਨਿਰਾ
				    ? lp->ll_class->class_name.string : (char_u *)"";
    ਜੇ (lp->ll_name != ਨਿਰਾ)
    {
	ਜੇ (lp->ll_name_ਸਿਰਾ == ਨਿਰਾ)
	    vname = lp->ll_name;
	ਨਹੀਂਤਾਂ
	{
	    vname = vim_strnsave(lp->ll_name, lp->ll_name_ਸਿਰਾ - lp->ll_name);
	    ਜੇ (vname == ਨਿਰਾ)
		ਵਾਪਸ;
	    did_alloc = TRUE;
	}
    }
    semsg(_(ਸੁਨੇਹਾ), vname, class_name);
    ਜੇ (did_alloc)
	vim_free(vname);
}

/*
 * Lock or unlock variable indicated by "lp".
 * "deep" is the levels to go (-1 for unlimited);
 * "lock" is TRUE for ":lockvar", FALSE for ":unlockvar".
 */
    ਸਥਿਰ ਪੂਰਨ
do_ਤਾਲਾ_var(
    lval_T	*lp,
    char_u	*name_ਸਿਰਾ,
    exarg_T	*eap,
    ਪੂਰਨ		deep,
    ਖਾਲੀ	*cookie ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ		ਤਾਲਾ = eap->cmdidx == CMD_lockvar;
    ਪੂਰਨ		ret = OK;
    ਪੂਰਨ		cc;
    dictitem_T	*di;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ LOG_LOCKVAR
    ch_log(ਨਿਰਾ, "LKVAR: do_lock_var(): name %s, is_root %d", lp->ll_name, lp->ll_is_ਜੜ੍ਹ);
#ਜੇ_ਅੰਤ

    ਜੇ (lp->ll_tv == ਨਿਰਾ)
    {
	cc = *name_ਸਿਰਾ;
	*name_ਸਿਰਾ = NUL;
	ਜੇ (*lp->ll_name == '$')
	{
	    semsg(_(e_cannot_ਤਾਲਾ_or_unlock_variable_str), lp->ll_name);
	    ret = FAIL;
	}
	ਨਹੀਂਤਾਂ
	{
	    // Normal name or expanded name.
	    di = ਲੱਭੋ_var(lp->ll_name, ਨਿਰਾ, TRUE);
	    ਜੇ (di == ਨਿਰਾ)
	    {
		ਜੇ (ਵਿੱਚ_vim9script())
		    semsg(_(e_cannot_ਲੱਭੋ_variable_ਮੰਜ਼ਿਲ_unlock_str),
								  lp->ll_name);
		ret = FAIL;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ ((di->di_flags & DI_FLAGS_FIX)
			    && di->di_tv.v_type != VAR_DICT
			    && di->di_tv.v_type != VAR_LIST)
		{
		    // For historic reasons this error is not given for a list
		    // or dict.  E.g., the b: dict could be locked/unlocked.
		    semsg(_(e_cannot_ਤਾਲਾ_or_unlock_variable_str), lp->ll_name);
		    ret = FAIL;
		}
		ਨਹੀਂਤਾਂ
		{
		    ਜੇ (ਵਿੱਚ_vim9script())
		    {
			svar_T  *sv = ਲੱਭੋ_typval_ਵਿੱਚ_script(&di->di_tv,
								     0, FALSE);

			ਜੇ (sv != ਨਿਰਾ && sv->sv_const != 0)
			{
			    semsg(_(e_cannot_change_readonly_variable_str),
								  lp->ll_name);
			    ret = FAIL;
			}
		    }

		    ਜੇ (ret == OK)
		    {
			ਜੇ (ਤਾਲਾ)
			    di->di_flags |= DI_FLAGS_ਤਾਲਾ;
			ਨਹੀਂਤਾਂ
			    di->di_flags &= ~DI_FLAGS_ਤਾਲਾ;
			ਜੇ (deep != 0)
			    ਵਸਤੂ_ਤਾਲਾ(&di->di_tv, deep, ਤਾਲਾ, FALSE);
		    }
		}
	    }
	}
	*name_ਸਿਰਾ = cc;
    }
    ਨਹੀਂਤਾਂ ਜੇ (deep == 0 && lp->ll_object == ਨਿਰਾ && lp->ll_class == ਨਿਰਾ)
    {
	// nothing to do
    }
    ਨਹੀਂਤਾਂ ਜੇ (lp->ll_is_ਜੜ੍ਹ)
	// (un)lock the item.
	ਵਸਤੂ_ਤਾਲਾ(lp->ll_tv, deep, ਤਾਲਾ, FALSE);
    ਨਹੀਂਤਾਂ ਜੇ (lp->ll_range)
    {
	listitem_T    *li = lp->ll_li;

	// (un)lock a range of List items.
	ਜਦੋਂ (li != ਨਿਰਾ && (lp->ll_empty2 || lp->ll_n2 >= lp->ll_n1))
	{
	    ਵਸਤੂ_ਤਾਲਾ(&li->li_tv, deep, ਤਾਲਾ, FALSE);
	    li = li->li_ਅਗਲਾ;
	    ++lp->ll_n1;
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (lp->ll_list != ਨਿਰਾ)
	// (un)lock a List item.
	ਵਸਤੂ_ਤਾਲਾ(&lp->ll_li->li_tv, deep, ਤਾਲਾ, FALSE);
    ਨਹੀਂਤਾਂ ਜੇ (lp->ll_object != ਨਿਰਾ)  // This check must be before ll_class.
    {
	// (un)lock an object variable.
	report_lockvar_member(e_cannot_ਤਾਲਾ_object_variable_str, lp);
	ret = FAIL;
    }
    ਨਹੀਂਤਾਂ ਜੇ (lp->ll_class != ਨਿਰਾ)
    {
	// (un)lock a class variable.
	report_lockvar_member(e_cannot_ਤਾਲਾ_class_variable_str, lp);
	ret = FAIL;
    }
    ਨਹੀਂਤਾਂ
    {
	// (un)lock a Dictionary item.
	ਜੇ (lp->ll_di == ਨਿਰਾ)
	{
	    emsg(_(e_dictionary_required));
	    ret = FAIL;
	}
	ਨਹੀਂਤਾਂ
	    ਵਸਤੂ_ਤਾਲਾ(&lp->ll_di->di_tv, deep, ਤਾਲਾ, FALSE);
    }

    ਵਾਪਸ ret;
}

/*
 * Lock or unlock an item.  "deep" is nr of levels to go.
 * When "check_refcount" is TRUE do not lock a list or dict with a reference
 * count larger than 1.
 */
    ਖਾਲੀ
ਵਸਤੂ_ਤਾਲਾ(typval_T *tv, ਪੂਰਨ deep, ਪੂਰਨ ਤਾਲਾ, ਪੂਰਨ check_refcount)
{
    ਸਥਿਰ ਪੂਰਨ	recurse = 0;
    list_T	*l;
    listitem_T	*li;
    dict_T	*d;
    blob_T	*b;
    hashitem_T	*hi;
    ਪੂਰਨ		todo;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ LOG_LOCKVAR
    ch_log(ਨਿਰਾ, "LKVAR: item_lock(): type %s", vartype_name(tv->v_type));
#ਜੇ_ਅੰਤ

    ਜੇ (recurse >= DICT_MAXNEST)
    {
	emsg(_(e_variable_nested_too_deep_for_unlock));
	ਵਾਪਸ;
    }
    ਜੇ (deep == 0)
	ਵਾਪਸ;
    ++recurse;

    // lock/unlock the item itself
    ਜੇ (ਤਾਲਾ)
	tv->v_ਤਾਲਾ |= VAR_LOCKED;
    ਨਹੀਂਤਾਂ
	tv->v_ਤਾਲਾ &= ~VAR_LOCKED;

    ਚੋਣ (tv->v_type)
    {
	ਹਾਲਤ VAR_UNKNOWN:
	ਹਾਲਤ VAR_ANY:
	ਹਾਲਤ VAR_VOID:
	ਹਾਲਤ VAR_ਸੰਖਿਆ:
	ਹਾਲਤ VAR_BOOL:
	ਹਾਲਤ VAR_STRING:
	ਹਾਲਤ VAR_FUNC:
	ਹਾਲਤ VAR_PARTIAL:
	ਹਾਲਤ VAR_FLOAT:
	ਹਾਲਤ VAR_SPECIAL:
	ਹਾਲਤ VAR_ਕੰਮ:
	ਹਾਲਤ VAR_CHANNEL:
	ਹਾਲਤ VAR_INSTR:
	ਹਾਲਤ VAR_CLASS:
	ਹਾਲਤ VAR_OBJECT:
	ਹਾਲਤ VAR_TYPEALIAS:
	    ਰੋਕੋ;

	ਹਾਲਤ VAR_BLOB:
	    ਜੇ ((b = tv->vval.v_blob) != ਨਿਰਾ
				    && !(check_refcount && b->bv_refcount > 1))
	    {
		ਜੇ (ਤਾਲਾ)
		    b->bv_ਤਾਲਾ |= VAR_LOCKED;
		ਨਹੀਂਤਾਂ
		    b->bv_ਤਾਲਾ &= ~VAR_LOCKED;
	    }
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_LIST:
	    ਜੇ ((l = tv->vval.v_list) != ਨਿਰਾ
				    && !(check_refcount && l->lv_refcount > 1))
	    {
		ਜੇ (ਤਾਲਾ)
		    l->lv_ਤਾਲਾ |= VAR_LOCKED;
		ਨਹੀਂਤਾਂ
		    l->lv_ਤਾਲਾ &= ~VAR_LOCKED;
		ਜੇ (deep < 0 || deep > 1)
		{
		    ਜੇ (l->lv_ਪਹਿਲਾ == &range_list_ਵਸਤੂ)
			l->lv_ਤਾਲਾ |= VAR_ਚੀਜ਼ਾਂ_LOCKED;
		    ਨਹੀਂਤਾਂ
		    {
			// recursive: lock/unlock the items the List contains
			CHECK_LIST_MATERIALIZE(l);
			FOR_ALL_LIST_ਚੀਜ਼ਾਂ(l, li) ਵਸਤੂ_ਤਾਲਾ(&li->li_tv,
					       deep - 1, ਤਾਲਾ, check_refcount);
		    }
		}
	    }
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_TUPLE:
	    tuple_ਤਾਲਾ(tv->vval.v_tuple, deep, ਤਾਲਾ, check_refcount);
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_DICT:
	    ਜੇ ((d = tv->vval.v_dict) != ਨਿਰਾ
				    && !(check_refcount && d->dv_refcount > 1))
	    {
		ਜੇ (ਤਾਲਾ)
		    d->dv_ਤਾਲਾ |= VAR_LOCKED;
		ਨਹੀਂਤਾਂ
		    d->dv_ਤਾਲਾ &= ~VAR_LOCKED;
		ਜੇ (deep < 0 || deep > 1)
		{
		    // recursive: lock/unlock the items the List contains
		    todo = (ਪੂਰਨ)d->dv_hashtab.ht_ਵਰਤਿਆ;
		    FOR_ALL_HASHTAB_ਚੀਜ਼ਾਂ(&d->dv_hashtab, hi, todo)
		    {
			ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
			{
			    --todo;
			    ਵਸਤੂ_ਤਾਲਾ(&HI2DI(hi)->di_tv, deep - 1, ਤਾਲਾ,
							       check_refcount);
			}
		    }
		}
	    }
    }
    --recurse;
}

#ਜੇ defined(FEAT_MENU) && defined(FEAT_MULTI_LANG)
/*
 * Delete all "menutrans_" variables.
 */
    ਖਾਲੀ
del_menutrans_vars(ਖਾਲੀ)
{
    hashitem_T	*hi;
    ਪੂਰਨ		todo;

    hash_ਤਾਲਾ(&globvarht);
    todo = (ਪੂਰਨ)globvarht.ht_ਵਰਤਿਆ;
    ਲਈ (hi = globvarht.ht_array; todo > 0 && !got_int; ++hi)
    {
	ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	{
	    --todo;
	    ਜੇ (STRNCMP(HI2DI(hi)->di_ਕੁੰਜੀ, "menutrans_", 10) == 0)
		delete_var(&globvarht, hi);
	}
    }
    hash_unlock(&globvarht);
}
#ਜੇ_ਅੰਤ

/*
 * Local string buffer for the next two functions to store a variable name
 * with its prefix. Allocated in cat_prefix_varname(), freed later in
 * get_user_var_name().
 */

ਸਥਿਰ char_u	*varnamebuf = ਨਿਰਾ;
ਸਥਿਰ ਪੂਰਨ	varnamebuflen = 0;

/*
 * Function to concatenate a prefix and a variable name.
 */
    char_u *
cat_ਅਗੇਤਰ_varname(ਪੂਰਨ ਅਗੇਤਰ, char_u *name)
{
    ਪੂਰਨ		len;

    len = (ਪੂਰਨ)STRLEN(name) + 3;
    ਜੇ (len > varnamebuflen)
    {
	vim_free(varnamebuf);
	len += 10;			// some additional space
	varnamebuf = alloc(len);
	ਜੇ (varnamebuf == ਨਿਰਾ)
	{
	    varnamebuflen = 0;
	    ਵਾਪਸ ਨਿਰਾ;
	}
	varnamebuflen = len;
    }
    *varnamebuf = ਅਗੇਤਰ;
    varnamebuf[1] = ':';
    STRCPY(varnamebuf + 2, name);
    ਵਾਪਸ varnamebuf;
}

/*
 * Function given to ExpandGeneric() to obtain the list of user defined
 * (global/buffer/window/built-in) variable names.
 */
    char_u *
get_user_var_name(expand_T *xp, ਪੂਰਨ idx)
{
    ਸਥਿਰ long_u	gdone;
    ਸਥਿਰ long_u	bdone;
    ਸਥਿਰ long_u	wdone;
    ਸਥਿਰ long_u	tdone;
    ਸਥਿਰ ਪੂਰਨ		vidx;
    ਸਥਿਰ hashitem_T	*hi;
    hashtab_T		*ht;

    ਜੇ (idx == 0)
    {
	gdone = bdone = wdone = vidx = 0;
	tdone = 0;
    }

    // Global variables
    ਜੇ (gdone < globvarht.ht_ਵਰਤਿਆ)
    {
	ਜੇ (gdone++ == 0)
	    hi = globvarht.ht_array;
	ਨਹੀਂਤਾਂ
	    ++hi;
	ਜਦੋਂ (HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	    ++hi;
	ਜੇ (STRNCMP("g:", xp->xp_ਨਮੂਨਾ, 2) == 0)
	    ਵਾਪਸ cat_ਅਗੇਤਰ_varname('g', hi->hi_ਕੁੰਜੀ);
	ਵਾਪਸ hi->hi_ਕੁੰਜੀ;
    }

    // b: variables
    ht = &prevwin_curwin()->w_ਬਫਰ->b_vars->dv_hashtab;
    ਜੇ (bdone < ht->ht_ਵਰਤਿਆ)
    {
	ਜੇ (bdone++ == 0)
	    hi = ht->ht_array;
	ਨਹੀਂਤਾਂ
	    ++hi;
	ਜਦੋਂ (HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	    ++hi;
	ਵਾਪਸ cat_ਅਗੇਤਰ_varname('b', hi->hi_ਕੁੰਜੀ);
    }

    // w: variables
    ht = &prevwin_curwin()->w_vars->dv_hashtab;
    ਜੇ (wdone < ht->ht_ਵਰਤਿਆ)
    {
	ਜੇ (wdone++ == 0)
	    hi = ht->ht_array;
	ਨਹੀਂਤਾਂ
	    ++hi;
	ਜਦੋਂ (HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	    ++hi;
	ਵਾਪਸ cat_ਅਗੇਤਰ_varname('w', hi->hi_ਕੁੰਜੀ);
    }

    // t: variables
    ht = &curtab->tp_vars->dv_hashtab;
    ਜੇ (tdone < ht->ht_ਵਰਤਿਆ)
    {
	ਜੇ (tdone++ == 0)
	    hi = ht->ht_array;
	ਨਹੀਂਤਾਂ
	    ++hi;
	ਜਦੋਂ (HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	    ++hi;
	ਵਾਪਸ cat_ਅਗੇਤਰ_varname('t', hi->hi_ਕੁੰਜੀ);
    }

    // v: variables
    ਜੇ (vidx < VV_LEN)
	ਵਾਪਸ cat_ਅਗੇਤਰ_varname('v', (char_u *)vimvars[vidx++].vv_name);

    VIM_CLEAR(varnamebuf);
    varnamebuflen = 0;
    ਵਾਪਸ ਨਿਰਾ;
}

    ਅੱਖਰ *
get_var_special_name(ਪੂਰਨ nr)
{
    ਚੋਣ (nr)
    {
	ਹਾਲਤ VVAL_FALSE: ਵਾਪਸ ਵਿੱਚ_vim9script() ? "false" : "v:false";
	ਹਾਲਤ VVAL_TRUE:  ਵਾਪਸ ਵਿੱਚ_vim9script() ? "true" : "v:true";
	ਹਾਲਤ VVAL_NULL:  ਵਾਪਸ ਵਿੱਚ_vim9script() ? "null" : "v:null";
	ਹਾਲਤ VVAL_NONE:  ਵਾਪਸ "v:none";
    }
    ਅੰਦਰੂਨੀ_error("get_var_special_name()");
    ਵਾਪਸ "42";
}

/*
 * Returns the global variable dictionary
 */
    dict_T *
get_globvar_dict(ਖਾਲੀ)
{
    ਵਾਪਸ &globvardict;
}

/*
 * Returns the global variable hash table
 */
    hashtab_T *
get_globvar_ht(ਖਾਲੀ)
{
    ਵਾਪਸ &globvarht;
}

/*
 * Returns the v: variable dictionary
 */
    dict_T *
get_vimvar_dict(ਖਾਲੀ)
{
    ਵਾਪਸ &vimvardict;
}

/*
 * Returns the index of a v:variable.  Negative if not found.
 * Returns DI_ flags in "di_flags".
 */
    ਪੂਰਨ
ਲੱਭੋ_vim_var(char_u *name, ਪੂਰਨ *di_flags)
{
    dictitem_T	    *di = ਲੱਭੋ_var_ਵਿੱਚ_ht(&vimvarht, 0, name, TRUE);
    ਬਣਤਰ vimvar   *vv;

    ਜੇ (di == ਨਿਰਾ)
	ਵਾਪਸ -1;
    *di_flags = di->di_flags;
    vv = (ਬਣਤਰ vimvar *)((ਅੱਖਰ *)di - offsetof(vimvar_T, vv_di));
    ਵਾਪਸ (ਪੂਰਨ)(vv - vimvars);
}


/*
 * Set type of v: variable to "type".
 */
    ਖਾਲੀ
ਸੈੱਟ_vim_var_type(ਪੂਰਨ idx, vartype_T type)
{
    vimvars[idx].vv_tv_type = type;
}

/*
 * Set number v: variable to "val".
 * Note that this does not set the type, use set_vim_var_type() for that.
 */
    ਖਾਲੀ
ਸੈੱਟ_vim_var_nr(ਪੂਰਨ idx, varnumber_T val)
{
    vimvars[idx].vv_nr = val;
}

    ਅੱਖਰ *
get_vim_var_name(ਪੂਰਨ idx)
{
    ਵਾਪਸ vimvars[idx].vv_name;
}

/*
 * Get typval_T v: variable value.
 */
    typval_T *
get_vim_var_tv(ਪੂਰਨ idx)
{
    ਵਾਪਸ &vimvars[idx].vv_tv;
}

    type_T *
get_vim_var_type(ਪੂਰਨ idx, garray_T *type_list)
{
    ਜੇ (vimvars[idx].vv_type != ਨਿਰਾ)
	ਵਾਪਸ vimvars[idx].vv_type;
    ਵਾਪਸ typval2type_vimvar(&vimvars[idx].vv_tv, type_list);
}

/*
 * Add what getinfo() reports for the v: variable "name", without the "v:", to
 * "d".  Returns FAIL when there is no such variable.
 */
    ਪੂਰਨ
vim_var_info(char_u *name, dict_T *d)
{
    ਪੂਰਨ		di_flags;
    ਪੂਰਨ		idx = ਲੱਭੋ_vim_var(name, &di_flags);
    garray_T	type_list;
    ਅੱਖਰ	*tofree;

    ਜੇ (idx < 0)
	ਵਾਪਸ FAIL;
    vim_snprintf((ਅੱਖਰ *)IObuff, IOSIZE, "v:%s", vimvars[idx].vv_name);
    dict_ਜੋੜੋ_string(d, "name", IObuff);
    dict_ਜੋੜੋ_bool(d, "available", TRUE);
    ga_init2(&type_list, ਆਕਾਰ(type_T *), 10);
    ਅੱਖਰ *type = type_name(get_vim_var_type(idx, &type_list), &tofree);
    dict_ਜੋੜੋ_string(d, "type", (char_u *)type);
    vim_free(tofree);
    clear_type_list(&type_list);
    dict_ਜੋੜੋ_bool(d, "readonly", (vimvars[idx].vv_flags & VV_RO) != 0);
    dict_ਜੋੜੋ_bool(d, "compat", (vimvars[idx].vv_flags & VV_COMPAT) != 0);
    ਵਾਪਸ OK;
}

/*
 * Set v: variable to "tv".  Only accepts the same type.
 * Takes over the value of "tv".
 */
    ਪੂਰਨ
ਸੈੱਟ_vim_var_tv(ਪੂਰਨ idx, typval_T *tv)
{
    ਜੇ (vimvars[idx].vv_tv_type != tv->v_type)
    {
	emsg(_(e_type_mismatch_for_v_variable));
	clear_tv(tv);
	ਵਾਪਸ FAIL;
    }
    // VV_RO is also checked when compiling, but let's check here as well.
    ਜੇ (vimvars[idx].vv_flags & VV_RO)
    {
	semsg(_(e_cannot_change_readonly_variable_str), vimvars[idx].vv_name);
	ਵਾਪਸ FAIL;
    }
    ਜੇ (sandbox && (vimvars[idx].vv_flags & VV_RO_SBX))
    {
	semsg(_(e_cannot_ਸੈੱਟ_variable_ਵਿੱਚ_sandbox_str), vimvars[idx].vv_name);
	ਵਾਪਸ FAIL;
    }
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_di.di_tv = *tv;
    ਵਾਪਸ OK;
}

/*
 * Get number v: variable value.
 */
    varnumber_T
get_vim_var_nr(ਪੂਰਨ idx)
{
    ਵਾਪਸ vimvars[idx].vv_nr;
}

/*
 * Get string v: variable value.  Uses a static buffer, can only be used once.
 * If the String variable has never been set, return an empty string.
 * Never returns NULL;
 */
    char_u *
get_vim_var_str(ਪੂਰਨ idx)
{
    ਵਾਪਸ tv_get_string(&vimvars[idx].vv_tv);
}

/*
 * Get List v: variable value.  Caller must take care of reference count when
 * needed.
 */
    list_T *
get_vim_var_list(ਪੂਰਨ idx)
{
    ਵਾਪਸ vimvars[idx].vv_list;
}

/*
 * Get Dict v: variable value.  Caller must take care of reference count when
 * needed.
 */
    dict_T *
get_vim_var_dict(ਪੂਰਨ idx)
{
    ਵਾਪਸ vimvars[idx].vv_dict;
}

/*
 * Set v:char to character "c".
 */
    ਖਾਲੀ
ਸੈੱਟ_vim_var_char(ਪੂਰਨ c)
{
    char_u  buf[MB_MAXBYTES + 1];
    size_t  buflen;

    ਜੇ (has_mbyte)
	buflen = (*mb_char2bytes)(c, buf);
    ਨਹੀਂਤਾਂ
    {
	buf[0] = c;
	buflen = 1;
    }
    buf[buflen] = NUL;

    ਸੈੱਟ_vim_var_string(VV_CHAR, buf, (ਪੂਰਨ)buflen);
}

/*
 * Set v:count to "count" and v:count1 to "count1".
 * When "set_prevcount" is TRUE first set v:prevcount from v:count.
 */
    ਖਾਲੀ
ਸੈੱਟ_vcount(
    ਲੰਮਾ	ਗਿਣਤੀ_2,
    ਲੰਮਾ	count1,
    ਪੂਰਨ		ਸੈੱਟ_prevcount)
{
    ਜੇ (ਸੈੱਟ_prevcount)
	vimvars[VV_PREVCOUNT].vv_nr = vimvars[VV_ਗਿਣਤੀ].vv_nr;
    vimvars[VV_ਗਿਣਤੀ].vv_nr = ਗਿਣਤੀ_2;
    vimvars[VV_COUNT1].vv_nr = count1;
}

/*
 * Save variables that might be changed as a side effect.  Used when executing
 * a timer callback.
 */
    ਖਾਲੀ
save_vimvars(vimvars_save_T *vvsave)
{
    vvsave->vv_prevcount = vimvars[VV_PREVCOUNT].vv_nr;
    vvsave->vv_ਗਿਣਤੀ = vimvars[VV_ਗਿਣਤੀ].vv_nr;
    vvsave->vv_count1 = vimvars[VV_COUNT1].vv_nr;
}

/*
 * Restore variables saved by save_vimvars().
 */
    ਖਾਲੀ
restore_vimvars(vimvars_save_T *vvsave)
{
    vimvars[VV_PREVCOUNT].vv_nr = vvsave->vv_prevcount;
    vimvars[VV_ਗਿਣਤੀ].vv_nr = vvsave->vv_ਗਿਣਤੀ;
    vimvars[VV_COUNT1].vv_nr = vvsave->vv_count1;
}

/*
 * Set string v: variable to a copy of "val". If 'copy' is FALSE, then set the
 * value.
 */
    ਖਾਲੀ
ਸੈੱਟ_vim_var_string(
    ਪੂਰਨ		idx,
    char_u	*val,
    ਪੂਰਨ		len)	    // length of "val" to use or -1 (whole string)
{
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_tv_type = VAR_STRING;
    ਜੇ (val == ਨਿਰਾ)
	vimvars[idx].vv_str = ਨਿਰਾ;
    ਨਹੀਂਤਾਂ ਜੇ (len == -1)
	vimvars[idx].vv_str = vim_strsave(val);
    ਨਹੀਂਤਾਂ
	vimvars[idx].vv_str = vim_strnsave(val, len);
}

    ਖਾਲੀ
ਸੈੱਟ_vim_var_string_ਸਿੱਧਾ(
    ਪੂਰਨ		idx,
    char_u	*val)
{
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_tv_type = VAR_STRING;

    vimvars[idx].vv_str = val;
}

/*
 * Set List v: variable to "val".
 */
    ਖਾਲੀ
ਸੈੱਟ_vim_var_list(ਪੂਰਨ idx, list_T *val)
{
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_tv_type = VAR_LIST;
    vimvars[idx].vv_list = val;
    ਜੇ (val != ਨਿਰਾ)
	++val->lv_refcount;
}

/*
 * Set Dictionary v: variable to "val".
 */
    ਖਾਲੀ
ਸੈੱਟ_vim_var_dict(ਪੂਰਨ idx, dict_T *val)
{
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_tv_type = VAR_DICT;
    vimvars[idx].vv_dict = val;
    ਜੇ (val == ਨਿਰਾ)
	ਵਾਪਸ;

    ++val->dv_refcount;
    dict_ਸੈੱਟ_ਚੀਜ਼ਾਂ_ro(val);
}

/*
 * Set the v:argv list.
 */
    ਖਾਲੀ
ਸੈੱਟ_argv_var(ਅੱਖਰ **argv, ਪੂਰਨ argc)
{
    list_T	*l = list_alloc();
    ਪੂਰਨ		i;

    ਜੇ (l == ਨਿਰਾ)
	getout(1);
    l->lv_ਤਾਲਾ = VAR_FIXED;
    ਲਈ (i = 0; i < argc; ++i)
    {
	ਜੇ (list_append_string(l, (char_u *)argv[i], -1) == FAIL)
	    getout(1);
	l->lv_u.mat.lv_ਆਖਰੀ->li_tv.v_ਤਾਲਾ = VAR_FIXED;
    }
    ਸੈੱਟ_vim_var_list(VV_ARGV, l);
}

/*
 * Reset v:register, taking the 'clipboard' setting into account.
 */
    ਖਾਲੀ
reset_reg_var(ਖਾਲੀ)
{
    ਪੂਰਨ regname = 0;

    // Adjust the register according to 'clipboard', so that when
    // "unnamed" is present it becomes '*' or '+' instead of '"'.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_CLIPMETHOD
    adjust_clip_reg(&regname);
#ਜੇ_ਅੰਤ
    ਸੈੱਟ_reg_var(regname);
}

/*
 * Set v:register if needed.
 */
    ਖਾਲੀ
ਸੈੱਟ_reg_var(ਪੂਰਨ c)
{
    char_u  regname[2];

    ਜੇ (c == 0 || c == ' ')
	regname[0] = '"';
    ਨਹੀਂਤਾਂ
	regname[0] = c;
    regname[1] = NUL;
    // Avoid free/alloc when the value is already right.
    ਜੇ (vimvars[VV_REG].vv_str == ਨਿਰਾ || vimvars[VV_REG].vv_str[0] != c)
	ਸੈੱਟ_vim_var_string(VV_REG, regname, 1);
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
    ਜੇ (oldval == ਨਿਰਾ)
	ਵਾਪਸ vimvars[VV_EXCEPTION].vv_str;

    vimvars[VV_EXCEPTION].vv_str = oldval;
    ਵਾਪਸ ਨਿਰਾ;
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
    ਜੇ (oldval == ਨਿਰਾ)
	ਵਾਪਸ vimvars[VV_THROWPOINT].vv_str;

    vimvars[VV_THROWPOINT].vv_str = oldval;
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Set v:cmdarg.
 * If "eap" != NULL, use "eap" to generate the value and return the old value.
 * If "oldarg" != NULL, restore the value to "oldarg" and return NULL.
 * Must always be called in pairs!
 */
    char_u *
ਸੈੱਟ_cmdarg(exarg_T *eap, char_u *oldarg)
{
    char_u	*oldval;
    char_u	*newval;
    ਬਿਨਾਨਿਸ਼ਾਨੀ	len;

    oldval = vimvars[VV_CMDARG].vv_str;
    ਜੇ (eap == ਨਿਰਾ)
    {
	vim_free(oldval);
	vimvars[VV_CMDARG].vv_str = oldarg;
	ਵਾਪਸ ਨਿਰਾ;
    }

    ਜੇ (eap->force_bin == FORCE_BIN)
	len = 6;
    ਨਹੀਂਤਾਂ ਜੇ (eap->force_bin == FORCE_NOBIN)
	len = 8;
    ਨਹੀਂਤਾਂ
	len = 0;

    ਜੇ (eap->read_edit)
	len += 7;

    ਜੇ (eap->force_ff != 0)
	len += 10; // " ++ff=unix"
    ਜੇ (eap->force_enc != 0)
	len += (ਬਿਨਾਨਿਸ਼ਾਨੀ)STRLEN(eap->cmd + eap->force_enc) + 7;
    ਜੇ (eap->bad_char != 0)
	len += 7 + 4;  // " ++bad=" + "keep" or "drop"

    newval = alloc(len + 1);
    ਜੇ (newval == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (eap->force_bin == FORCE_BIN)
	sprintf((ਅੱਖਰ *)newval, " ++bin");
    ਨਹੀਂਤਾਂ ਜੇ (eap->force_bin == FORCE_NOBIN)
	sprintf((ਅੱਖਰ *)newval, " ++nobin");
    ਨਹੀਂਤਾਂ
	*newval = NUL;

    ਜੇ (eap->read_edit)
	STRCAT(newval, " ++edit");

    ਜੇ (eap->force_ff != 0)
	sprintf((ਅੱਖਰ *)newval + STRLEN(newval), " ++ff=%s",
						eap->force_ff == 'u' ? "unix"
						: eap->force_ff == 'd' ? "dos"
						: "mac");
    ਜੇ (eap->force_enc != 0)
	sprintf((ਅੱਖਰ *)newval + STRLEN(newval), " ++enc=%s",
					       eap->cmd + eap->force_enc);
    ਜੇ (eap->bad_char == BAD_KEEP)
	STRCPY(newval + STRLEN(newval), " ++bad=keep");
    ਨਹੀਂਤਾਂ ਜੇ (eap->bad_char == BAD_DROP)
	STRCPY(newval + STRLEN(newval), " ++bad=drop");
    ਨਹੀਂਤਾਂ ਜੇ (eap->bad_char != 0)
	sprintf((ਅੱਖਰ *)newval + STRLEN(newval), " ++bad=%c", eap->bad_char);
    vimvars[VV_CMDARG].vv_str = newval;
    ਵਾਪਸ oldval;
}

/*
 * Get the value of internal variable "name".
 * If "flags" has EVAL_VAR_IMPORT may return a VAR_ANY with v_number set to the
 * imported script ID.
 * Return OK or FAIL.  If OK is returned "rettv" must be cleared.
 */
    ਪੂਰਨ
eval_variable(
    char_u	*name,
    ਪੂਰਨ		len,		// length of "name" or zero
    scid_T	sid,		// script ID for imported item or zero
    typval_T	*rettv,		// NULL when only checking existence
    dictitem_T	**dip,		// non-NULL when typval's dict item is needed
    ਪੂਰਨ		flags)		// EVAL_VAR_ flags
{
    ਪੂਰਨ		ret = OK;
    typval_T	*tv = ਨਿਰਾ;
    ਪੂਰਨ		ਮਿਲਿਆ = FALSE;
    hashtab_T	*ht = ਨਿਰਾ;
    ਪੂਰਨ		cc = 0;
    type_T	*type = ਨਿਰਾ;

    ਜੇ (len > 0)
    {
	// truncate the name, so that we can use strcmp()
	cc = name[len];
	name[len] = NUL;
    }

    // Check for local variable when debugging.
    ਜੇ ((sid == 0) && (tv = lookup_debug_var(name)) == ਨਿਰਾ)
    {
	// Check for user-defined variables.
	dictitem_T	*v = ਲੱਭੋ_var(name, &ht, flags & EVAL_VAR_NOAUTOLOAD);

	ਜੇ (v != ਨਿਰਾ)
	{
	    tv = &v->di_tv;
	    ਜੇ (dip != ਨਿਰਾ)
		*dip = v;
	}
	ਨਹੀਂਤਾਂ
	    ht = ਨਿਰਾ;
    }

    ਜੇ (tv == ਨਿਰਾ && (ਵਿੱਚ_vim9script() || STRNCMP(name, "s:", 2) == 0))
    {
	imported_T  *import = ਨਿਰਾ;
	char_u	    *p = STRNCMP(name, "s:", 2) == 0 ? name + 2 : name;

	ਜੇ (sid == 0)
	    import = ਲੱਭੋ_imported(p, 0, TRUE);

	// imported variable from another script
	ਜੇ (import != ਨਿਰਾ || sid != 0)
	{
	    ਜੇ ((flags & EVAL_VAR_IMPORT) == 0)
	    {
		ਜੇ (SCRIPT_ID_VALID(sid))
		{
		    ht = &SCRIPT_VARS(sid);
		    ਜੇ (ht != ਨਿਰਾ)
		    {
			dictitem_T *v = ਲੱਭੋ_var_ਵਿੱਚ_ht(ht, 0, name,
						  flags & EVAL_VAR_NOAUTOLOAD);

			ਜੇ (v == ਨਿਰਾ)
			    v = ਲੱਭੋ_var_autoload_ਅਗੇਤਰ(name, sid, ਨਿਰਾ, ਨਿਰਾ);

			ਜੇ (v != ਨਿਰਾ)
			{
			    tv = &v->di_tv;
			    ਜੇ (dip != ਨਿਰਾ)
				*dip = v;
			}
			ਨਹੀਂਤਾਂ
			    ht = ਨਿਰਾ;
		    }
		}
		ਨਹੀਂਤਾਂ
		{
		    ਜੇ (flags & EVAL_VAR_VERBOSE)
			semsg(_(e_expected_dot_after_name_str), name);
		    ret = FAIL;
		}
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ (rettv != ਨਿਰਾ)
		{
		    // special value that is used in handle_subscript()
		    rettv->v_type = VAR_ANY;
		    rettv->vval.v_ਸੰਖਿਆ = sid != 0 ? sid : import->imp_sid;
		}
		ਮਿਲਿਆ = TRUE;
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (ਵਿੱਚ_vim9script() && (flags & EVAL_VAR_NO_FUNC) == 0)
	{
	    ਪੂਰਨ	    has_g_ਅਗੇਤਰ = STRNCMP(name, "g:", 2) == 0;
	    ufunc_T *ufunc = ਲੱਭੋ_func(name, FALSE);

	    ਜੇ (ufunc != ਨਿਰਾ && cc == '<')
	    {
		// handle generic function
		char_u	*argp = name + len;
		name[len] = cc;
		ufunc = eval_generic_func(ufunc, name, &argp);
		name[len] = NUL;
		ਜੇ (ufunc == ਨਿਰਾ)
		{
		    ret = FAIL;
		    ਜਾਓ ਪੂਰਾ;
		}
	    }

	    // In Vim9 script we can get a function reference by using the
	    // function name.  For a global non-autoload function "g:" is
	    // required.
	    ਜੇ (ufunc != ਨਿਰਾ && (has_g_ਅਗੇਤਰ
					    || !func_requires_g_ਅਗੇਤਰ(ufunc)))
	    {
		ਮਿਲਿਆ = TRUE;
		ਜੇ (rettv != ਨਿਰਾ)
		{
		    rettv->v_type = VAR_FUNC;
		    ਜੇ (has_g_ਅਗੇਤਰ)
		    {
			// Keep the "g:", otherwise script-local may be
			// assumed.
			ਜੇ (cc != '<')
			    rettv->vval.v_string = vim_strsave(name);
			ਨਹੀਂਤਾਂ
			{
			    // append the generic function arguments
			    char_u	*argp = name + len;
			    name[len] = cc;
			    rettv->vval.v_string =
				append_generic_func_type_ਦਲੀਲਾਂ(name, len,
									&argp);
			    name[len] = NUL;
			}
		    }
		    ਨਹੀਂਤਾਂ
			rettv->vval.v_string = vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
		    ਜੇ (rettv->vval.v_string != ਨਿਰਾ)
			func_ref(ufunc->uf_name);
		}
	    }
	}
    }

    ਜੇ (!ਮਿਲਿਆ)
    {
	ਜੇ (tv == ਨਿਰਾ)
	{
	    ਜੇ (rettv != ਨਿਰਾ && (flags & EVAL_VAR_VERBOSE))
		semsg(_(e_undefined_variable_str), name);
	    ret = FAIL;
	}
	ਨਹੀਂਤਾਂ ਜੇ (rettv != ਨਿਰਾ)
	{
	    svar_T  *sv = ਨਿਰਾ;
	    ਪੂਰਨ	    was_assigned = FALSE;

	    ਜੇ (ht != ਨਿਰਾ && ht == get_script_local_ht()
		    && tv != &SCRIPT_SV(current_sctx.sc_sid)->sv_var.di_tv)
	    {
		sv = ਲੱਭੋ_typval_ਵਿੱਚ_script(tv, 0, TRUE);
		ਜੇ (sv != ਨਿਰਾ)
		{
		    type = sv->sv_type;
		    was_assigned = sv->sv_flags & SVFLAG_ASSIGNED;
		}
	    }

	    ਜੇ ((tv->v_type == VAR_TYPEALIAS || tv->v_type == VAR_CLASS)
		    && sid != 0)
	    {
		// type alias or class imported from another script.  Check
		// whether it is exported from the other script.
		sv = ਲੱਭੋ_typval_ਵਿੱਚ_script(tv, sid, TRUE);
		ਜੇ (sv == ਨਿਰਾ)
		{
		    ret = FAIL;
		    ਜਾਓ ਪੂਰਾ;
		}
		ਜੇ ((sv->sv_flags & SVFLAG_EXPORTED) == 0)
		{
		    semsg(_(e_ਵਸਤੂ_not_exported_ਵਿੱਚ_script_str), name);
		    ret = FAIL;
		    ਜਾਓ ਪੂਰਾ;
		}
	    }

	    // If a list or tuple or dict variable wasn't initialized and has
	    // meaningful type, do it now.  Not for global variables, they are
	    // not declared.
	    ਜੇ (ht != &globvarht)
	    {
		ਜੇ (tv->v_type == VAR_DICT && tv->vval.v_dict == ਨਿਰਾ
					    && ((type != ਨਿਰਾ && !was_assigned)
							  || !ਵਿੱਚ_vim9script()))
		{
		    tv->vval.v_dict = dict_alloc();
		    ਜੇ (tv->vval.v_dict != ਨਿਰਾ)
		    {
			++tv->vval.v_dict->dv_refcount;
			tv->vval.v_dict->dv_type = alloc_type(type);
			ਜੇ (sv != ਨਿਰਾ)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
		ਨਹੀਂਤਾਂ ਜੇ (tv->v_type == VAR_LIST && tv->vval.v_list == ਨਿਰਾ
					    && ((type != ਨਿਰਾ && !was_assigned)
							  || !ਵਿੱਚ_vim9script()))
		{
		    tv->vval.v_list = list_alloc();
		    ਜੇ (tv->vval.v_list != ਨਿਰਾ)
		    {
			++tv->vval.v_list->lv_refcount;
			tv->vval.v_list->lv_type = alloc_type(type);
			ਜੇ (sv != ਨਿਰਾ)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
		ਨਹੀਂਤਾਂ ਜੇ (tv->v_type == VAR_TUPLE && tv->vval.v_tuple == ਨਿਰਾ
					    && ((type != ਨਿਰਾ && !was_assigned)
							  || !ਵਿੱਚ_vim9script()))
		{
		    tv->vval.v_tuple = tuple_alloc();
		    ਜੇ (tv->vval.v_tuple != ਨਿਰਾ)
		    {
			++tv->vval.v_tuple->tv_refcount;
			tv->vval.v_tuple->tv_type = alloc_type(type);
			ਜੇ (sv != ਨਿਰਾ)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
		ਨਹੀਂਤਾਂ ਜੇ (tv->v_type == VAR_BLOB && tv->vval.v_blob == ਨਿਰਾ
					    && ((type != ਨਿਰਾ && !was_assigned)
							  || !ਵਿੱਚ_vim9script()))
		{
		    tv->vval.v_blob = blob_alloc();
		    ਜੇ (tv->vval.v_blob != ਨਿਰਾ)
		    {
			++tv->vval.v_blob->bv_refcount;
			ਜੇ (sv != ਨਿਰਾ)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
	    }
	    ret = copy_tv(tv, rettv);
	}
    }

ਪੂਰਾ:
    ਜੇ (len > 0)
	name[len] = cc;

    ਵਾਪਸ ret;
}

/*
 * Get the value of internal variable "name", also handling "import.name".
 * Return OK or FAIL.  If OK is returned "rettv" must be cleared.
 */
    ਪੂਰਨ
eval_variable_import(
    char_u	*name,
    typval_T	*rettv)
{
    char_u  *s = name;
    ਜਦੋਂ (ASCII_ISALNUM(*s) || *s == '_')
	++s;
    ਪੂਰਨ	    len = (ਪੂਰਨ)(s - name);

    ਜੇ (eval_variable(name, len, 0, rettv, ਨਿਰਾ, EVAL_VAR_IMPORT) == FAIL)
	ਵਾਪਸ FAIL;
    ਜੇ (rettv->v_type == VAR_ANY && *s == '.')
    {
	char_u *ns = s + 1;
	s = ns;
	ਜਦੋਂ (ASCII_ISALNUM(*s) || *s == '_')
	    ++s;
	ਪੂਰਨ sid = rettv->vval.v_ਸੰਖਿਆ;
	ਵਾਪਸ eval_variable(ns, (ਪੂਰਨ)(s - ns), sid, rettv, ਨਿਰਾ, 0);
    }
    ਵਾਪਸ OK;
}


/*
 * Check if variable "name[len]" is a local variable or an argument.
 * If so, "*eval_lavars_used" is set to TRUE.
 */
    ਖਾਲੀ
check_vars(char_u *name, ਪੂਰਨ len)
{
    ਪੂਰਨ		cc;
    char_u	*varname;
    hashtab_T	*ht;

    ਜੇ (eval_lavars_ਵਰਤਿਆ == ਨਿਰਾ)
	ਵਾਪਸ;

    // truncate the name, so that we can use strcmp()
    cc = name[len];
    name[len] = NUL;

    ht = ਲੱਭੋ_var_ht(name, &varname);
    ਜੇ (ht == get_funccal_local_ht() || ht == get_funccal_ਦਲੀਲਾਂ_ht())
    {
	ਜੇ (ਲੱਭੋ_var(name, ਨਿਰਾ, TRUE) != ਨਿਰਾ)
	    *eval_lavars_ਵਰਤਿਆ = TRUE;
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
ਲੱਭੋ_var(char_u *name, hashtab_T **htp, ਪੂਰਨ no_autoload)
{
    char_u	*varname;
    hashtab_T	*ht;
    dictitem_T	*ret = ਨਿਰਾ;

    ht = ਲੱਭੋ_var_ht(name, &varname);
    ਜੇ (htp != ਨਿਰਾ)
	*htp = ht;
    ਜੇ (ht == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    ret = ਲੱਭੋ_var_ਵਿੱਚ_ht(ht, *name, varname, no_autoload);
    ਜੇ (ret != ਨਿਰਾ)
	ਵਾਪਸ ret;

    // Search in parent scope for lambda
    ret = ਲੱਭੋ_var_ਵਿੱਚ_scoped_ht(name, no_autoload);
    ਜੇ (ret != ਨਿਰਾ)
	ਵਾਪਸ ret;

    // in Vim9 script items without a scope can be script-local
    ਜੇ (ਵਿੱਚ_vim9script() && name[0] != NUL && name[1] != ':')
    {
	ht = get_script_local_ht();
	ਜੇ (ht != ਨਿਰਾ)
	{
	    ret = ਲੱਭੋ_var_ਵਿੱਚ_ht(ht, *name, varname, no_autoload);
	    ਜੇ (ret != ਨਿਰਾ)
	    {
		ਜੇ (htp != ਨਿਰਾ)
		    *htp = ht;
		ਵਾਪਸ ret;
	    }
	}
    }

    // and finally try
    ਵਾਪਸ ਲੱਭੋ_var_autoload_ਅਗੇਤਰ(name, 0, htp, ਨਿਰਾ);
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
ਲੱਭੋ_var_autoload_ਅਗੇਤਰ(char_u *name, ਪੂਰਨ sid, hashtab_T **htp,
							    char_u **namep)
{
    hashtab_T	*ht;
    dictitem_T	*ret = ਨਿਰਾ;
    // When using "vim9script autoload" script-local items are prefixed but can
    // be used with s:name.
    ਪੂਰਨ check_sid = sid > 0 ? sid : current_sctx.sc_sid;
    ਜੇ (SCRIPT_ID_VALID(check_sid)
		   && (ਵਿੱਚ_vim9script() || (name[0] == 's' && name[1] == ':')))
    {
	scriptitem_T *si = SCRIPT_ਵਸਤੂ(check_sid);

	ਜੇ (si->sn_autoload_ਅਗੇਤਰ != ਨਿਰਾ)
	{
	    char_u *ਅਧਾਰ_name = (name[0] == 's' && name[1] == ':')
							     ? name + 2 : name;
	    char_u *auto_name = concat_str(si->sn_autoload_ਅਗੇਤਰ, ਅਧਾਰ_name);

	    ਜੇ (auto_name != ਨਿਰਾ)
	    {
		ਪੂਰਨ free_auto_name = TRUE;
		ht = &globvarht;
		ret = ਲੱਭੋ_var_ਵਿੱਚ_ht(ht, 'g', auto_name, TRUE);
		ਜੇ (ret != ਨਿਰਾ)
		{
		    ਜੇ (htp != ਨਿਰਾ)
			*htp = ht;
		    ਜੇ (namep != ਨਿਰਾ)
		    {
			free_auto_name = FALSE;
			*namep = auto_name;
		    }
		}
		ਜੇ (free_auto_name)
		    vim_free(auto_name);
	    }
	}
    }

    ਵਾਪਸ ret;
}

/*
 * Like find_var() but if the name starts with <SNR>99_ then look in the
 * referenced script (used for a funcref).
 */
    dictitem_T *
ਲੱਭੋ_var_also_ਵਿੱਚ_script(char_u *name, hashtab_T **htp, ਪੂਰਨ no_autoload)
{
    ਜੇ (STRNCMP(name, "<SNR>", 5) == 0 && ਸੁਰੱਖਿਅਤ_isdigit(name[5]))
    {
	char_u	    *p = name + 5;
	ਪੂਰਨ	    sid = getdigits(&p);

	ਜੇ (SCRIPT_ID_VALID(sid) && *p == '_')
	{
	    hashtab_T	*ht = &SCRIPT_VARS(sid);

	    ਜੇ (ht != ਨਿਰਾ)
	    {
		dictitem_T *di = ਲੱਭੋ_var_ਵਿੱਚ_ht(ht, 0, p + 1, no_autoload);

		ਜੇ (di != ਨਿਰਾ)
		{
		    ਜੇ (htp != ਨਿਰਾ)
			*htp = ht;
		    ਵਾਪਸ di;
		}
	    }
	}
    }

    ਵਾਪਸ ਲੱਭੋ_var(name, htp, no_autoload);
}

/*
 * Find variable "varname" in hashtab "ht" with name "htname".
 * When "varname" is empty returns curwin/curtab/etc vars dictionary.
 * Returns NULL if not found.
 */
    dictitem_T *
ਲੱਭੋ_var_ਵਿੱਚ_ht(
    hashtab_T	*ht,
    ਪੂਰਨ		htname,
    char_u	*varname,
    ਪੂਰਨ		no_autoload)
{
    hashitem_T	*hi;

    ਜੇ (*varname == NUL)
    {
	// Must be something like "s:", otherwise "ht" would be NULL.
	ਚੋਣ (htname)
	{
	    ਹਾਲਤ 's': ਵਾਪਸ &SCRIPT_SV(current_sctx.sc_sid)->sv_var;
	    ਹਾਲਤ 'g': ਵਾਪਸ &globvars_var;
	    ਹਾਲਤ 'v': ਵਾਪਸ &vimvars_var;
	    ਹਾਲਤ 'b': ਵਾਪਸ &curbuf->b_bufvar;
	    ਹਾਲਤ 'w': ਵਾਪਸ &curwin->w_winvar;
	    ਹਾਲਤ 't': ਵਾਪਸ &curtab->tp_winvar;
	    ਹਾਲਤ 'l': ਵਾਪਸ get_funccal_local_var();
	    ਹਾਲਤ 'a': ਵਾਪਸ get_funccal_ਦਲੀਲਾਂ_var();
	}
	ਵਾਪਸ ਨਿਰਾ;
    }

    hi = hash_ਲੱਭੋ(ht, varname);
    ਜੇ (HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
    {
	// For global variables we may try auto-loading the script.  If it
	// worked find the variable again.  Don't auto-load a script if it was
	// loaded already, otherwise it would be loaded every time when
	// checking if a function name is a Funcref variable.
	ਜੇ (ht == &globvarht && !no_autoload)
	{
	    // Note: script_autoload() may make "hi" invalid. It must either
	    // be obtained again or not used.
	    ਜੇ (!script_autoload(varname, FALSE) || aborting())
		ਵਾਪਸ ਨਿਰਾ;
	    hi = hash_ਲੱਭੋ(ht, varname);
	}
	ਜੇ (HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	    ਵਾਪਸ ਨਿਰਾ;
    }
    ਵਾਪਸ HI2DI(hi);
}

/*
 * Get the script-local hashtab.  NULL if not in a script context.
 */
    hashtab_T *
get_script_local_ht(ਖਾਲੀ)
{
    scid_T sid = current_sctx.sc_sid;

    ਜੇ (SCRIPT_ID_VALID(sid))
	ਵਾਪਸ &SCRIPT_VARS(sid);
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Look for "name[len]" in script-local variables and functions.
 * When "cmd" is TRUE it must look like a command, a function must be followed
 * by "(" or "->".
 * Return OK when found, FAIL when not found.
 */
    ਪੂਰਨ
lookup_scriptitem(
	char_u	*name,
	size_t	len,
	ਪੂਰਨ	cmd,
	cctx_T	*dummy ਅਣਵਰਤਿਆ)
{
    hashtab_T	*ht = get_script_local_ht();
    char_u	ਬਫਰ[30];
    char_u	*p;
    ਪੂਰਨ		res;
    hashitem_T	*hi;
    ਪੂਰਨ		is_global = FALSE;
    char_u	*fname = name;

    ਜੇ (ht == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    ਜੇ (len < ਆਕਾਰ(ਬਫਰ) - 1)
    {
	// avoid an alloc/free for short names
	vim_strncpy(ਬਫਰ, name, len);
	p = ਬਫਰ;
    }
    ਨਹੀਂਤਾਂ
    {
	p = vim_strnsave(name, len);
	ਜੇ (p == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;
    }

    hi = hash_ਲੱਭੋ(ht, p);
    res = HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi) ? FAIL : OK;

    // if not script-local, then perhaps autoload-exported
    ਜੇ (res == FAIL && ਲੱਭੋ_var_autoload_ਅਗੇਤਰ(p, 0, ਨਿਰਾ, ਨਿਰਾ) != ਨਿਰਾ)
	res = OK;

    // if not script-local or autoload, then perhaps imported
    ਜੇ (res == FAIL && ਲੱਭੋ_imported(p, 0, FALSE) != ਨਿਰਾ)
	res = OK;
    ਜੇ (p != ਬਫਰ)
	vim_free(p);

    // Find a function, so that a following "->" works.
    // When used as a command require "(" or "->" to follow, "Cmd" is a user
    // command while "Cmd()" is a function call.
    ਜੇ (res != OK)
    {
	p = skipwhite(name + len);

	ਜੇ (!cmd || name[len] == '(' || (p[0] == '-' && p[1] == '>'))
	{
	    // Do not check for an internal function, since it might also be a
	    // valid command, such as ":split" versus "split()".
	    // Skip "g:" before a function name.
	    ਜੇ (name[0] == 'g' && name[1] == ':')
	    {
		is_global = TRUE;
		fname = name + 2;
	    }
	    ਜੇ (ਲੱਭੋ_func(fname, is_global) != ਨਿਰਾ)
		res = OK;
	}
    }

    ਵਾਪਸ res;
}

/*
 * Find the hashtab used for a variable name.
 * Return NULL if the name is not valid.
 * Set "varname" to the start of name without ':'.
 */
    hashtab_T *
ਲੱਭੋ_var_ht(char_u *name, char_u **varname)
{
    hashitem_T	*hi;
    hashtab_T	*ht;

    ਜੇ (name[0] == NUL)
	ਵਾਪਸ ਨਿਰਾ;
    ਜੇ (name[1] != ':')
    {
	// The name must not start with a colon or #.
	ਜੇ (name[0] == ':' || name[0] == AUTOLOAD_CHAR)
	    ਵਾਪਸ ਨਿਰਾ;
	*varname = name;

	// "version" is "v:version" in all scopes if scriptversion < 3.
	// Same for a few other variables marked with VV_COMPAT.
	ਜੇ (ਵਿੱਚ_old_script(3))
	{
	    hi = hash_ਲੱਭੋ(&compat_hashtab, name);
	    ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
		ਵਾਪਸ &compat_hashtab;
	}

	ht = get_funccal_local_ht();
	ਜੇ (ht != ਨਿਰਾ)
	    ਵਾਪਸ ht;				// local variable

	// In Vim9 script items at the script level are script-local, except
	// for autoload names.
	ਜੇ (ਵਿੱਚ_vim9script() && vim_strchr(name, AUTOLOAD_CHAR) == ਨਿਰਾ)
	{
	    ht = get_script_local_ht();
	    ਜੇ (ht != ਨਿਰਾ)
		ਵਾਪਸ ht;
	}

	ਵਾਪਸ &globvarht;			// global variable
    }
    *varname = name + 2;
    ਜੇ (*name == 'g')				// global variable
	ਵਾਪਸ &globvarht;
    // There must be no ':' or '#' in the rest of the name, unless g: is used
    ਜੇ (vim_strchr(name + 2, ':') != ਨਿਰਾ
			       || vim_strchr(name + 2, AUTOLOAD_CHAR) != ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    ਜੇ (*name == 'b')				// buffer variable
	ਵਾਪਸ &curbuf->b_vars->dv_hashtab;
    ਜੇ (*name == 'w')				// window variable
	ਵਾਪਸ &curwin->w_vars->dv_hashtab;
    ਜੇ (*name == 't')				// tab page variable
	ਵਾਪਸ &curtab->tp_vars->dv_hashtab;
    ਜੇ (*name == 'v')				// v: variable
	ਵਾਪਸ &vimvarht;
    ਜੇ (get_current_funccal() != ਨਿਰਾ
	       && get_current_funccal()->fc_func->uf_def_ਸਥਿਤੀ
							    == UF_NOT_COMPILED)
    {
	// a: and l: are only used in functions defined with ":function"
	ਜੇ (*name == 'a')			// a: function argument
	    ਵਾਪਸ get_funccal_ਦਲੀਲਾਂ_ht();
	ਜੇ (*name == 'l')			// l: local function variable
	    ਵਾਪਸ get_funccal_local_ht();
    }
    ਜੇ (*name == 's')				// script variable
    {
	ht = get_script_local_ht();
	ਜੇ (ht != ਨਿਰਾ)
	    ਵਾਪਸ ht;
    }
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Get the string value of a (global/local) variable.
 * Note: see tv_get_string() for how long the pointer remains valid.
 * Returns NULL when it doesn't exist.
 */
    char_u *
get_var_ਮੁੱਲ(char_u *name)
{
    dictitem_T	*v;

    v = ਲੱਭੋ_var(name, ਨਿਰਾ, FALSE);
    ਜੇ (v == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    ਵਾਪਸ tv_get_string(&v->di_tv);
}

/*
 * Allocate a new hashtab for a sourced script.  It will be used while
 * sourcing this script and when executing functions defined in the script.
 */
    ਖਾਲੀ
new_script_vars(scid_T id)
{
    scriptvar_T *sv;

    sv = ALLOC_CLEAR_ONE(scriptvar_T);
    ਜੇ (sv == ਨਿਰਾ)
	ਵਾਪਸ;
    init_var_dict(&sv->sv_dict, &sv->sv_var, VAR_SCOPE);
    SCRIPT_ਵਸਤੂ(id)->sn_vars = sv;
}

/*
 * Initialize dictionary "dict" as a scope and set variable "dict_var" to
 * point to it.
 */
    ਖਾਲੀ
init_var_dict(dict_T *dict, dictitem_T *dict_var, ਪੂਰਨ scope)
{
    hash_init(&dict->dv_hashtab);
    dict->dv_ਤਾਲਾ = 0;
    dict->dv_scope = scope;
    dict->dv_refcount = DO_NOT_FREE_CNT;
    dict->dv_copyID = 0;
    dict_var->di_tv.vval.v_dict = dict;
    dict_var->di_tv.v_type = VAR_DICT;
    dict_var->di_tv.v_ਤਾਲਾ = VAR_FIXED;
    dict_var->di_flags = DI_FLAGS_RO | DI_FLAGS_FIX;
    dict_var->di_ਕੁੰਜੀ[0] = NUL;
}

/*
 * Unreference a dictionary initialized by init_var_dict().
 */
    ਖਾਲੀ
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
    ਖਾਲੀ
vars_clear(hashtab_T *ht)
{
    vars_clear_ext(ht, TRUE);
}

/*
 * Like vars_clear(), but only free the value if "free_val" is TRUE.
 */
    ਖਾਲੀ
vars_clear_ext(hashtab_T *ht, ਪੂਰਨ free_val)
{
    ਪੂਰਨ		todo;
    hashitem_T	*hi;
    dictitem_T	*v;

    hash_ਤਾਲਾ(ht);
    todo = (ਪੂਰਨ)ht->ht_ਵਰਤਿਆ;
    FOR_ALL_HASHTAB_ਚੀਜ਼ਾਂ(ht, hi, todo)
    {
	ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	{
	    --todo;

	    // Free the variable.  Don't remove it from the hashtab,
	    // ht_array might change then.  hash_clear() takes care of it
	    // later.
	    v = HI2DI(hi);
	    ਜੇ (free_val)
		clear_tv(&v->di_tv);
	    ਜੇ (v->di_flags & DI_FLAGS_ALLOC)
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
    ਖਾਲੀ
delete_var(hashtab_T *ht, hashitem_T *hi)
{
    dictitem_T	*di = HI2DI(hi);

    ਜੇ (hash_remove(ht, hi, "delete variable") != OK)
	ਵਾਪਸ;

    clear_tv(&di->di_tv);
    vim_free(di);
}

/*
 * Delete the exported variables of a reloaded Vim9 autoload script.  They live
 * in the global namespace with the autoload prefix (e.g. "foo#bar") and are
 * recreated when the script body runs again.
 */
    ਖਾਲੀ
delete_autoload_export_vars(char_u *ਅਗੇਤਰ)
{
    ਜੇ (ਅਗੇਤਰ == ਨਿਰਾ)
	ਵਾਪਸ;
    size_t  prefixlen = STRLEN(ਅਗੇਤਰ);

    hash_ਤਾਲਾ(&globvarht);
    ਪੂਰਨ	todo = (ਪੂਰਨ)globvarht.ht_ਵਰਤਿਆ;

    ਲਈ (hashitem_T *hi = globvarht.ht_array; todo > 0; ++hi)
	ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	{
	    dictitem_T	*di = HI2DI(hi);

	    --todo;
	    // Keep a class or enum: existing objects still refer to it.
	    ਜੇ (di->di_tv.v_type != VAR_CLASS
		    && STRNCMP(di->di_ਕੁੰਜੀ, ਅਗੇਤਰ, prefixlen) == 0)
		delete_var(&globvarht, hi);
	}
    hash_unlock(&globvarht);
}

/*
 * List the value of one internal variable.
 */
    ਸਥਿਰ ਖਾਲੀ
list_one_var(dictitem_T *v, ਅੱਖਰ *ਅਗੇਤਰ, ਪੂਰਨ *ਪਹਿਲਾ)
{
    char_u	*tofree;
    char_u	*s;
    char_u	numbuf[NUMBUFLEN];

    s = echo_string(&v->di_tv, &tofree, numbuf, get_copyID());
    list_one_var_a(ਅਗੇਤਰ, v->di_ਕੁੰਜੀ, v->di_tv.v_type,
					 s == ਨਿਰਾ ? (char_u *)"" : s, ਪਹਿਲਾ);
    vim_free(tofree);
}

    ਸਥਿਰ ਖਾਲੀ
list_one_var_a(
    ਅੱਖਰ	*ਅਗੇਤਰ,
    char_u	*name,
    ਪੂਰਨ		type,
    char_u	*string,
    ਪੂਰਨ		*ਪਹਿਲਾ)  // when TRUE clear rest of screen and set to FALSE
{
    // don't use msg() or msg_attr() to avoid overwriting "v:statusmsg"
    ਸੁਨੇਹਾ_ਸ਼ੁਰੂ();
    ਸੁਨੇਹਾ_puts(ਅਗੇਤਰ);
    ਜੇ (name != ਨਿਰਾ)	// "a:" vars don't have a name stored
	ਸੁਨੇਹਾ_puts((ਅੱਖਰ *)name);
    ਸੁਨੇਹਾ_putchar(' ');
    ਸੁਨੇਹਾ_advance(22);
    ਜੇ (type == VAR_ਸੰਖਿਆ)
	ਸੁਨੇਹਾ_putchar('#');
    ਨਹੀਂਤਾਂ ਜੇ (type == VAR_FUNC || type == VAR_PARTIAL)
	ਸੁਨੇਹਾ_putchar('*');
    ਨਹੀਂਤਾਂ ਜੇ (type == VAR_LIST)
    {
	ਸੁਨੇਹਾ_putchar('[');
	ਜੇ (*string == '[')
	    ++string;
    }
    ਨਹੀਂਤਾਂ ਜੇ (type == VAR_DICT)
    {
	ਸੁਨੇਹਾ_putchar('{');
	ਜੇ (*string == '{')
	    ++string;
    }
    ਨਹੀਂਤਾਂ
	ਸੁਨੇਹਾ_putchar(' ');

    ਸੁਨੇਹਾ_outtrans(string);

    ਜੇ (type == VAR_FUNC || type == VAR_PARTIAL)
	ਸੁਨੇਹਾ_puts("()");
    ਜੇ (*ਪਹਿਲਾ)
    {
	ਸੁਨੇਹਾ_clr_eos();
	*ਪਹਿਲਾ = FALSE;
    }
}

/*
 * Addition handling for setting a v: variable.
 * Return TRUE if the variable should be set normally,
 *        FALSE if nothing else needs to be done.
 */
    ਪੂਰਨ
before_ਸੈੱਟ_vvar(
    char_u	*varname,
    dictitem_T	*di,
    typval_T	*tv,
    ਪੂਰਨ		copy,
    ਪੂਰਨ		*type_error)
{
    ਜੇ (di->di_tv.v_type == VAR_STRING)
    {
	VIM_CLEAR(di->di_tv.vval.v_string);
	ਜੇ (copy || tv->v_type != VAR_STRING)
	{
	    char_u *val = tv_get_string(tv);

	    // Careful: when assigning to v:errmsg and
	    // tv_get_string() causes an error message the variable
	    // will already be set.
	    ਜੇ (di->di_tv.vval.v_string == ਨਿਰਾ)
		di->di_tv.vval.v_string = vim_strsave(val);
	}
	ਨਹੀਂਤਾਂ
	{
	    // Take over the string to avoid an extra alloc/free.
	    di->di_tv.vval.v_string = tv->vval.v_string;
	    tv->vval.v_string = ਨਿਰਾ;
	}
	ਵਾਪਸ FALSE;
    }
    ਨਹੀਂਤਾਂ ਜੇ (di->di_tv.v_type == VAR_ਸੰਖਿਆ)
    {
	di->di_tv.vval.v_ਸੰਖਿਆ = tv_get_ਸੰਖਿਆ(tv);
	ਜੇ (STRCMP(varname, "searchforward") == 0)
	    ਸੈੱਟ_ਖੋਜ_direction(di->di_tv.vval.v_ਸੰਖਿਆ ? '/' : '?');
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਖੋਜ_EXTRA
	ਨਹੀਂਤਾਂ ਜੇ (STRCMP(varname, "hlsearch") == 0)
	{
	    no_hlsearch = !di->di_tv.vval.v_ਸੰਖਿਆ;
	    redraw_all_later(UPD_SOME_VALID);
	}
#ਜੇ_ਅੰਤ
	ਵਾਪਸ FALSE;
    }
    ਨਹੀਂਤਾਂ ਜੇ (di->di_tv.v_type != tv->v_type)
    {
	*type_error = TRUE;
	ਵਾਪਸ FALSE;
    }
    ਵਾਪਸ TRUE;
}

/*
 * Set variable "name" to value in "tv".
 * If the variable already exists, the value is updated.
 * Otherwise the variable is created.
 */
    ਖਾਲੀ
ਸੈੱਟ_var(
    char_u	*name,
    typval_T	*tv,
    ਪੂਰਨ		copy)	    // make copy of value in "tv"
{
    ਸੈੱਟ_var_const(name, 0, ਨਿਰਾ, tv, copy, ਸੌਂਪੋ_DECL, 0);
}

/*
 * Set variable "name" to value in "tv_arg".
 * When "sid" is non-zero "name" is in the script with this ID.
 * If the variable already exists and "is_const" is FALSE the value is updated.
 * Otherwise the variable is created.
 */
    ਪੂਰਨ
ਸੈੱਟ_var_const(
    char_u	*name,
    scid_T	sid,
    type_T	*type_arg,
    typval_T	*tv_arg,
    ਪੂਰਨ		copy,	    // make copy of value in "tv"
    ਪੂਰਨ		flags_arg,  // ASSIGN_CONST, ASSIGN_FINAL, etc.
    ਪੂਰਨ		var_idx)    // index for ":let [a, b] = list"
{
    typval_T	*tv = tv_arg;
    type_T	*type = type_arg;
    typval_T	bool_tv;
    dictitem_T	*di;
    typval_T	*dest_tv;
    char_u	*varname;
    char_u	*name_tofree = ਨਿਰਾ;
    hashtab_T	*ht = ਨਿਰਾ;
    ਪੂਰਨ		is_script_local;
    ਪੂਰਨ		vim9script = ਵਿੱਚ_vim9script();
    ਪੂਰਨ		var_ਵਿੱਚ_vim9script;
    ਪੂਰਨ		var_ਵਿੱਚ_autoload = FALSE;
    ਪੂਰਨ		flags = flags_arg;
    ਪੂਰਨ		free_tv_arg = !copy;  // free tv_arg if not used
    ਪੂਰਨ		rc = FAIL;

    ਜੇ (sid != 0)
    {
	varname = ਨਿਰਾ;
	ਜੇ (SCRIPT_ID_VALID(sid))
	{
	    char_u	*auto_name = ਨਿਰਾ;
	    ਜੇ (ਲੱਭੋ_var_autoload_ਅਗੇਤਰ(name, sid, &ht, &auto_name) != ਨਿਰਾ)
	    {
		var_ਵਿੱਚ_autoload = TRUE;
		varname = auto_name;
		name_tofree = varname;
	    }
	    ਨਹੀਂਤਾਂ
		ht = &SCRIPT_VARS(sid);
	}
	ਜੇ (varname == ਨਿਰਾ)
	    varname = name;
    }
    ਨਹੀਂਤਾਂ
    {
	scriptitem_T	*si;
	char_u		*auto_name = ਨਿਰਾ;

	ਜੇ (ਵਿੱਚ_vim9script()
	    && SCRIPT_ID_VALID(current_sctx.sc_sid)
	    && (si = SCRIPT_ਵਸਤੂ(current_sctx.sc_sid))
					      ->sn_autoload_ਅਗੇਤਰ != ਨਿਰਾ
	    && (is_export
		|| ਲੱਭੋ_var_autoload_ਅਗੇਤਰ(name, 0, ਨਿਰਾ, &auto_name)
								    != ਨਿਰਾ))
	{
	    // In a vim9 autoload script an exported variable is put in the
	    // global namespace with the autoload prefix.
	    var_ਵਿੱਚ_autoload = TRUE;
	    varname = auto_name != ਨਿਰਾ ? auto_name
		      : concat_str(si->sn_autoload_ਅਗੇਤਰ, name);
	    ਜੇ (varname == ਨਿਰਾ)
		ਜਾਓ failed;
	    name_tofree = varname;
	    ht = &globvarht;
	}
	ਨਹੀਂਤਾਂ
	    ht = ਲੱਭੋ_var_ht(name, &varname);
    }
    ਜੇ (ht == ਨਿਰਾ || *varname == NUL)
    {
	semsg(_(e_illegal_variable_name_str), name);
	ਜਾਓ failed;
    }
    is_script_local = ht == get_script_local_ht() || sid != 0
							    || var_ਵਿੱਚ_autoload;

    ਜੇ (vim9script
	    && !is_script_local
	    && (flags & (ਸੌਂਪੋ_NO_DECL | ਸੌਂਪੋ_DECL)) == 0
	    && (flags & (ਸੌਂਪੋ_CONST | ਸੌਂਪੋ_FINAL)) == 0
	    && name[1] == ':')
    {
	vim9_declare_error(name);
	ਜਾਓ failed;
    }
    ਜੇ ((flags & ਸੌਂਪੋ_FOR_LOOP) && is_scoped_variable(name))
	// Do not make g:var, w:var, b:var or t:var final.
	flags &= ~ਸੌਂਪੋ_FINAL;

    var_ਵਿੱਚ_vim9script = is_script_local && current_script_is_vim9();
    ਜੇ (var_ਵਿੱਚ_vim9script && name[0] == '_' && name[1] == NUL)
    {
	// For "[a, _] = list" the underscore is ignored.
	ਜੇ ((flags & ਸੌਂਪੋ_UNPACK) == 0)
	    emsg(_(e_cannot_use_underscore_ਇੱਥੇ));
	ਜਾਓ failed;
    }

    di = ਲੱਭੋ_var_ਵਿੱਚ_ht(ht, 0, varname, TRUE);

    ਜੇ (di == ਨਿਰਾ && var_ਵਿੱਚ_vim9script)
    {
	imported_T  *import = ਲੱਭੋ_imported(varname, 0, FALSE);

	ਜੇ (import != ਨਿਰਾ)
	{
	    // imported name space cannot be used
	    ਜੇ ((flags & ਸੌਂਪੋ_NO_DECL) == 0)
	    {
		semsg(_(e_redefining_imported_ਵਸਤੂ_str), name);
		ਜਾਓ failed;
	    }
	    semsg(_(e_cannot_use_str_itself_it_is_imported), name);
	    ਜਾਓ failed;
	}
	ਜੇ (!ਵਿੱਚ_vim9script())
	{
	    semsg(_(e_cannot_create_vim9_script_variable_ਵਿੱਚ_function_str),
									 name);
	    ਜਾਓ failed;
	}
    }

    // Search in parent scope which is possible to reference from lambda
    ਜੇ (di == ਨਿਰਾ)
	di = ਲੱਭੋ_var_ਵਿੱਚ_scoped_ht(name, TRUE);

    ਜੇ ((tv->v_type == VAR_FUNC || tv->v_type == VAR_PARTIAL)
				  && var_wrong_func_name(name, di == ਨਿਰਾ))
	ਜਾਓ failed;

    ਜੇ (need_convert_ਮੰਜ਼ਿਲ_bool(type, tv))
    {
	// Destination is a bool and the value is not, but it can be
	// converted.
	CLEAR_ਖੇਤਰ(bool_tv);
	bool_tv.v_type = VAR_BOOL;
	bool_tv.vval.v_ਸੰਖਿਆ = tv2bool(tv) ? VVAL_TRUE : VVAL_FALSE;
	tv = &bool_tv;
    }

    ਜੇ (di != ਨਿਰਾ)
    {
	// Item already exists.  Allowed to replace when reloading.
	ਜੇ ((di->di_flags & DI_FLAGS_RELOAD) == 0)
	{
	    ਜੇ ((flags & (ਸੌਂਪੋ_CONST | ਸੌਂਪੋ_FINAL))
					 && (flags & ਸੌਂਪੋ_FOR_LOOP) == 0)
	    {
		emsg(_(e_cannot_modify_existing_variable));
		ਜਾਓ failed;
	    }

	    ਜੇ (is_script_local && vim9script
			  && (flags & (ਸੌਂਪੋ_NO_DECL | ਸੌਂਪੋ_DECL)) == 0)
	    {
		semsg(_(e_redefining_script_ਵਸਤੂ_str), name);
		ਜਾਓ failed;
	    }

	    ਜੇ (check_typval_is_ਮੁੱਲ(&di->di_tv) == FAIL)
		ਜਾਓ failed;

	    // List and Blob types can be modified in-place using the "+="
	    // compound operator.  For other types, this is not allowed.
	    ਪੂਰਨ type_inplace_modifiable =
		(di->di_tv.v_type == VAR_LIST || di->di_tv.v_type == VAR_BLOB);

	    ਜੇ (var_ਵਿੱਚ_vim9script && (flags & ਸੌਂਪੋ_FOR_LOOP) == 0
		    && ((flags & ਸੌਂਪੋ_COMPOUND_OP) == 0
			|| !type_inplace_modifiable))
	    {
		where_T where = WHERE_INIT;
		svar_T  *sv = ਲੱਭੋ_typval_ਵਿੱਚ_script(&di->di_tv, sid, TRUE);

		ਜੇ (sv != ਨਿਰਾ)
		{
		    // check the type and adjust to bool if needed
		    ਜੇ (var_idx > 0)
		    {
			where.wt_ਸੂਚਕਾਂਕ = var_idx;
			where.wt_ਕਿਸਮ = WT_VARIABLE;
		    }
		    ਜੇ (check_script_var_type(sv, tv, name, where) == FAIL)
			ਜਾਓ failed;
		    ਜੇ (type == ਨਿਰਾ)
			type = sv->sv_type;
		    sv->sv_flags |= SVFLAG_ASSIGNED;
		}
	    }

	    // Modifying a final variable with a List value using the "+="
	    // operator is allowed.  For other types, it is not allowed.
	    ਜੇ ((((flags & ਸੌਂਪੋ_FOR_LOOP) == 0 || (flags & ਸੌਂਪੋ_DECL) == 0)
			&& ((flags & ਸੌਂਪੋ_COMPOUND_OP) == 0
			    || !type_inplace_modifiable))
				 ? var_check_permission(di, name) == FAIL
				 : var_check_ro(di->di_flags, name, FALSE))
		ਜਾਓ failed;
	}
	ਨਹੀਂਤਾਂ
	{
	    // can only redefine once
	    di->di_flags &= ~DI_FLAGS_RELOAD;

	    // A Vim9 script-local variable is also present in sn_all_vars
	    // and sn_var_vals.  It may set "type" from "tv".
	    ਜੇ (var_ਵਿੱਚ_vim9script || var_ਵਿੱਚ_autoload)
		update_vim9_script_var(FALSE, di,
			var_ਵਿੱਚ_autoload ? name : di->di_ਕੁੰਜੀ, flags,
			tv, &type, (flags & ਸੌਂਪੋ_NO_MEMBER_TYPE) == 0);
	}

	// existing variable, need to clear the value

	// In Vim9 script a String v: variable only takes a String.
	ਜੇ (ht == &vimvarht && di->di_tv.v_type == VAR_STRING
							    && ਵਿੱਚ_vim9script())
	{
	    where_T	where = WHERE_INIT;

	    ਜੇ (check_typval_type(&t_string, tv, where) == FAIL)
		ਜਾਓ failed;
	}

	// Handle setting internal v: variables separately where needed to
	// prevent changing the type.
	ਪੂਰਨ type_error = FALSE;
	ਜੇ (ht == &vimvarht
		&& !before_ਸੈੱਟ_vvar(varname, di, tv, copy, &type_error))
	{
	    ਜੇ (type_error)
		semsg(_(e_setting_v_str_ਮੰਜ਼ਿਲ_ਮੁੱਲ_with_wrong_type), varname);
	    ਜਾਓ failed;
	}

	clear_tv(&di->di_tv);

	ਜੇ ((flags & ਸੌਂਪੋ_UPDATE_ਖੰਡ_ID)
				       && SCRIPT_ID_VALID(current_sctx.sc_sid))
	{
	    scriptitem_T *si = SCRIPT_ਵਸਤੂ(current_sctx.sc_sid);

	    update_script_var_ਖੰਡ_id(name, si->sn_current_ਖੰਡ_id);
	}
    }
    ਨਹੀਂਤਾਂ
    {
	// Item not found, check if a function already exists.
	ਜੇ (is_script_local && (flags & (ਸੌਂਪੋ_NO_DECL | ਸੌਂਪੋ_DECL)) == 0
	       && lookup_scriptitem(name, STRLEN(name), FALSE, ਨਿਰਾ) == OK)
	{
	    semsg(_(e_redefining_script_ਵਸਤੂ_str), name);
	    ਜਾਓ failed;
	}

	// add a new variable
	ਜੇ (var_ਵਿੱਚ_vim9script && (flags & ਸੌਂਪੋ_NO_DECL))
	{
	    semsg(_(e_unknown_variable_str), name);
	    ਜਾਓ failed;
	}

	ਜੇ (check_hashtab_frozen(ht, "add variable"))
	    ਜਾਓ failed;

	// Can't add "v:" or "a:" variable.
	ਜੇ (ht == &vimvarht || ht == get_funccal_ਦਲੀਲਾਂ_ht())
	{
	    semsg(_(e_illegal_variable_name_str), name);
	    ਜਾਓ failed;
	}

	// Make sure the variable name is valid.  In Vim9 script an
	// autoload variable must be prefixed with "g:" unless in an
	// autoload script.
	ਜੇ (!valid_varname(varname, -1, !vim9script
			|| STRNCMP(name, "g:", 2) == 0 || var_ਵਿੱਚ_autoload))
	    ਜਾਓ failed;

	di = alloc(offsetof(dictitem_T, di_ਕੁੰਜੀ) + STRLEN(varname) + 1);
	ਜੇ (di == ਨਿਰਾ)
	    ਜਾਓ failed;
	STRCPY(di->di_ਕੁੰਜੀ, varname);
	ਜੇ (hash_ਜੋੜੋ(ht, DI2HIKEY(di), "add variable") == FAIL)
	{
	    vim_free(di);
	    ਜਾਓ failed;
	}
	di->di_flags = DI_FLAGS_ALLOC;
	ਜੇ (flags & (ਸੌਂਪੋ_CONST | ਸੌਂਪੋ_FINAL))
	    di->di_flags |= DI_FLAGS_ਤਾਲਾ;

	// A Vim9 script-local variable is also added to sn_all_vars and
	// sn_var_vals. It may set "type" from "tv".
	ਜੇ (var_ਵਿੱਚ_vim9script || var_ਵਿੱਚ_autoload)
	    update_vim9_script_var(TRUE, di,
		    var_ਵਿੱਚ_autoload ? name : di->di_ਕੁੰਜੀ, flags,
			  tv, &type, (flags & ਸੌਂਪੋ_NO_MEMBER_TYPE) == 0);
    }

    dest_tv = &di->di_tv;
    ਜੇ (copy || tv->v_type == VAR_ਸੰਖਿਆ || tv->v_type == VAR_FLOAT)
	copy_tv(tv, dest_tv);
    ਨਹੀਂਤਾਂ
    {
	*dest_tv = *tv;
	dest_tv->v_ਤਾਲਾ = 0;
	init_tv(tv);
    }
    free_tv_arg = FALSE;

    ਜੇ (vim9script && type != ਨਿਰਾ)
	ਸੈੱਟ_tv_type(dest_tv, type);

    // ":const var = value" locks the value
    // ":final var = value" locks "var"
    ਜੇ (flags & ਸੌਂਪੋ_CONST)
	// Like :lockvar! name: lock the value and what it contains, but only
	// if the reference count is up to one.  That locks only literal
	// values.
	ਵਸਤੂ_ਤਾਲਾ(dest_tv, DICT_MAXNEST, TRUE, TRUE);

    rc = OK;

failed:
    vim_free(name_tofree);
    ਜੇ (free_tv_arg)
	clear_tv(tv_arg);

    ਵਾਪਸ rc;
}

/*
 * Check in this order for backwards compatibility:
 * - Whether the variable is read-only
 * - Whether the variable value is locked
 * - Whether the variable is locked
 * NOTE: "name" is only used for error messages.
 */
    ਪੂਰਨ
var_check_permission(dictitem_T *di, char_u *name)
{
    ਜੇ (var_check_ro(di->di_flags, name, FALSE)
		    || ਮੁੱਲ_check_ਤਾਲਾ(di->di_tv.v_ਤਾਲਾ, name, FALSE)
		    || var_check_ਤਾਲਾ(di->di_flags, name, FALSE))
	ਵਾਪਸ FAIL;
    ਵਾਪਸ OK;
}

/*
 * Return TRUE if di_flags "flags" indicates variable "name" is read-only.
 * Also give an error message.
 */
    ਪੂਰਨ
var_check_ro(ਪੂਰਨ flags, char_u *name, ਪੂਰਨ use_gettext)
{
    ਜੇ (flags & DI_FLAGS_RO)
    {
	ਜੇ (name == ਨਿਰਾ)
	    emsg(_(e_cannot_change_readonly_variable));
	ਨਹੀਂਤਾਂ
	    semsg(_(e_cannot_change_readonly_variable_str),
				       use_gettext ? (char_u *)_(name) : name);
	ਵਾਪਸ TRUE;
    }
    ਜੇ ((flags & DI_FLAGS_RO_SBX) && sandbox)
    {
	ਜੇ (name == ਨਿਰਾ)
	    emsg(_(e_cannot_ਸੈੱਟ_variable_ਵਿੱਚ_sandbox));
	ਨਹੀਂਤਾਂ
	    semsg(_(e_cannot_ਸੈੱਟ_variable_ਵਿੱਚ_sandbox_str),
				       use_gettext ? (char_u *)_(name) : name);
	ਵਾਪਸ TRUE;
    }
    ਵਾਪਸ FALSE;
}

/*
 * Return TRUE if di_flags "flags" indicates variable "name" is locked.
 * Also give an error message.
 */
    ਪੂਰਨ
var_check_ਤਾਲਾ(ਪੂਰਨ flags, char_u *name, ਪੂਰਨ use_gettext)
{
    ਜੇ (flags & DI_FLAGS_ਤਾਲਾ)
    {
	semsg(_(e_variable_is_locked_str),
				       use_gettext ? (char_u *)_(name) : name);
	ਵਾਪਸ TRUE;
    }
    ਵਾਪਸ FALSE;
}

/*
 * Return TRUE if di_flags "flags" indicates variable "name" is fixed.
 * Also give an error message.
 */
    ਪੂਰਨ
var_check_fixed(ਪੂਰਨ flags, char_u *name, ਪੂਰਨ use_gettext)
{
    ਜੇ (flags & DI_FLAGS_FIX)
    {
	ਜੇ (name == ਨਿਰਾ)
	    emsg(_(e_cannot_delete_variable));
	ਨਹੀਂਤਾਂ
	    semsg(_(e_cannot_delete_variable_str),
				      use_gettext ? (char_u *)_(name) : name);
	ਵਾਪਸ TRUE;
    }
    ਵਾਪਸ FALSE;
}

/*
 * Check if a funcref is assigned to a valid variable name.
 * Return TRUE and give an error if not.
 */
    ਪੂਰਨ
var_wrong_func_name(
    char_u *name,    // points to start of variable name
    ਪੂਰਨ    new_var)  // TRUE when creating the variable
{
    // Allow for w: b: s: and t:.  In Vim9 script s: is not allowed, because
    // the name can be used without the s: prefix.
    // Allow autoload variable.
    ਜੇ (!((vim_strchr((char_u *)"wbt", name[0]) != ਨਿਰਾ
		    || (!ਵਿੱਚ_vim9script() && name[0] == 's')) && name[1] == ':')
	    && !ASCII_ISUPPER((name[0] != NUL && name[1] == ':')
						     ? name[2] : name[0])
	    && vim_strchr(name, '#') == ਨਿਰਾ)
    {
	semsg(_(e_funcref_variable_name_must_ਸ਼ੁਰੂ_with_capital_str), name);
	ਵਾਪਸ TRUE;
    }
    // Don't allow hiding a function.  When "v" is not NULL we might be
    // assigning another function to the same var, the type is checked
    // below.
    ਜੇ (new_var && function_exists(name, FALSE))
    {
	semsg(_(e_variable_name_conflicts_with_existing_function_str),
								    name);
	ਵਾਪਸ TRUE;
    }
    ਵਾਪਸ FALSE;
}

/*
 * Return TRUE if "flags" indicates variable "name" has a locked (immutable)
 * value.  Also give an error message, using "name" or _("name") when
 * "use_gettext" is TRUE.
 */
    ਪੂਰਨ
ਮੁੱਲ_check_ਤਾਲਾ(ਪੂਰਨ ਤਾਲਾ, char_u *name, ਪੂਰਨ use_gettext)
{
    ਜੇ (ਤਾਲਾ & VAR_LOCKED)
    {
	ਜੇ (name == ਨਿਰਾ)
	    emsg(_(e_ਮੁੱਲ_is_locked));
	ਨਹੀਂਤਾਂ
	    semsg(_(e_ਮੁੱਲ_is_locked_str),
				       use_gettext ? (char_u *)_(name) : name);
	ਵਾਪਸ TRUE;
    }
    ਜੇ (ਤਾਲਾ & VAR_FIXED)
    {
	ਜੇ (name == ਨਿਰਾ)
	    emsg(_(e_cannot_change_ਮੁੱਲ));
	ਨਹੀਂਤਾਂ
	    semsg(_(e_cannot_change_ਮੁੱਲ_ਦਾ_str),
				       use_gettext ? (char_u *)_(name) : name);
	ਵਾਪਸ TRUE;
    }
    ਵਾਪਸ FALSE;
}

/*
 * Check if a variable name is valid.  When "autoload" is true "#" is allowed.
 * If "len" is -1 use all of "varname", otherwise up to "varname[len]".
 * Return FALSE and give an error if not.
 */
    ਪੂਰਨ
valid_varname(char_u *varname, ਪੂਰਨ len, ਪੂਰਨ autoload)
{
    char_u *p;

    ਲਈ (p = varname; len < 0 ? *p != NUL : p < varname + len; ++p)
	ਜੇ (!eval_isnamec1(*p) && (p == varname || !VIM_ISDIGIT(*p))
					 && !(autoload && *p == AUTOLOAD_CHAR))
	{
	    semsg(_(e_illegal_variable_name_str), varname);
	    ਵਾਪਸ FALSE;
	}
    ਵਾਪਸ TRUE;
}

/*
 * Implements the logic to retrieve local variable and option values.
 * Used by "getwinvar()" "gettabvar()" "gettabwinvar()" "getbufvar()".
 */
    ਸਥਿਰ ਖਾਲੀ
get_var_from(
    char_u	*varname,
    typval_T	*rettv,
    typval_T	*deftv,	    // Default value if not found.
    ਪੂਰਨ		htname,	    // 't'ab, 'w'indow or 'b'uffer local.
    tabpage_T	*tp,	    // can be NULL
    win_T	*win,
    buf_T	*buf)	    // Ignored if htname is not 'b'.
{
    dictitem_T	*v;
    ਪੂਰਨ		ਪੂਰਾ = FALSE;
    switchwin_T	switchwin;
    ਪੂਰਨ		need_switch_win;
    ਪੂਰਨ		do_change_curbuf = buf != ਨਿਰਾ && htname == 'b';

    ++emsg_off;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ਨਿਰਾ;

    ਜੇ (varname != ਨਿਰਾ && tp != ਨਿਰਾ && win != ਨਿਰਾ
	    && (htname != 'b' || buf != ਨਿਰਾ))
    {
	// Set curwin to be our win, temporarily.  Also set the tabpage,
	// otherwise the window is not valid. Only do this when needed,
	// autocommands get blocked.
	// If we have a buffer reference avoid the switching, we're saving and
	// restoring curbuf directly.
	need_switch_win = !(tp == curtab && win == curwin) && !do_change_curbuf;
	ਜੇ (!need_switch_win || switch_win(&switchwin, win, tp, TRUE) == OK)
	{
	    // Handle options. There are no tab-local options.
	    ਜੇ (*varname == '&' && htname != 't')
	    {
		buf_T	*save_curbuf = curbuf;

		// Change curbuf so the option is read from the correct buffer.
		ਜੇ (do_change_curbuf)
		    curbuf = buf;

		ਜੇ (varname[1] == NUL)
		{
		    // get all window-local or buffer-local options in a dict
		    dict_T	*opts = get_winbuf_options(htname == 'b');

		    ਜੇ (opts != ਨਿਰਾ)
		    {
			rettv_dict_ਸੈੱਟ(rettv, opts);
			ਪੂਰਾ = TRUE;
		    }
		}
		ਨਹੀਂਤਾਂ ਜੇ (eval_option(&varname, rettv, TRUE) == OK)
		    // Local option
		    ਪੂਰਾ = TRUE;

		curbuf = save_curbuf;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (*varname == NUL)
	    {
		// Empty string: return a dict with all the local variables.
		ਜੇ (htname == 'b')
		    v = &buf->b_bufvar;
		ਨਹੀਂਤਾਂ ਜੇ (htname == 'w')
		    v = &win->w_winvar;
		ਨਹੀਂਤਾਂ
		    v = &tp->tp_winvar;
		copy_tv(&v->di_tv, rettv);
		ਪੂਰਾ = TRUE;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		hashtab_T	*ht;

		ਜੇ (htname == 'b')
		    ht = &buf->b_vars->dv_hashtab;
		ਨਹੀਂਤਾਂ ਜੇ (htname == 'w')
		    ht = &win->w_vars->dv_hashtab;
		ਨਹੀਂਤਾਂ
		    ht = &tp->tp_vars->dv_hashtab;

		// Look up the variable.
		v = ਲੱਭੋ_var_ਵਿੱਚ_ht(ht, htname, varname, FALSE);
		ਜੇ (v != ਨਿਰਾ)
		{
		    copy_tv(&v->di_tv, rettv);
		    ਪੂਰਾ = TRUE;
		}
	    }
	}

	ਜੇ (need_switch_win)
	    // restore previous notion of curwin
	    restore_win(&switchwin, TRUE);
    }

    ਜੇ (!ਪੂਰਾ && deftv->v_type != VAR_UNKNOWN)
	// use the default value
	copy_tv(deftv, rettv);

    --emsg_off;
}

/*
 * getwinvar() and gettabwinvar()
 */
    ਸਥਿਰ ਖਾਲੀ
getwinvar(
    typval_T	*argvars,
    typval_T	*rettv,
    ਪੂਰਨ		off)	    // 1 for gettabwinvar()
{
    char_u	*varname;
    tabpage_T	*tp;
    win_T	*win;

    ਜੇ (off == 1)
	tp = ਲੱਭੋ_tabpage((ਪੂਰਨ)tv_get_ਸੰਖਿਆ_chk(&argvars[0], ਨਿਰਾ));
    ਨਹੀਂਤਾਂ
	tp = curtab;
    win = ਲੱਭੋ_win_by_nr(&argvars[off], tp);
    varname = tv_get_string_chk(&argvars[off + 1]);

    get_var_from(varname, rettv, &argvars[off + 2], 'w', tp, win, ਨਿਰਾ);
}

/*
 * Set option "varname" to the value of "varp" for the current buffer/window.
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_option_from_tv(char_u *varname, typval_T *varp)
{
    ਲੰਮਾ	numval = 0;
    char_u	*strval;
    char_u	nbuf[NUMBUFLEN];
    ਪੂਰਨ		error = FALSE;

    ਪੂਰਨ		opt_idx = findoption(varname);
    ਜੇ (opt_idx < 0)
    {
	semsg(_(e_unknown_option_str_2), varname);
	ਵਾਪਸ;
    }
    ਪੂਰਨ		opt_p_flags = get_option_flags(opt_idx);

    ਜੇ (varp->v_type == VAR_BOOL)
    {
	ਜੇ (opt_p_flags & P_STRING)
	{
	    emsg(_(e_string_required));
	    ਵਾਪਸ;
	}
	numval = (ਲੰਮਾ)varp->vval.v_ਸੰਖਿਆ;
	strval = (char_u *)"0";  // avoid using "false"
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ ((opt_p_flags & (P_ਅੰਕ|P_BOOL))
		&& (!ਵਿੱਚ_vim9script() || varp->v_type != VAR_STRING))
	    numval = (ਲੰਮਾ)tv_get_ਸੰਖਿਆ_chk(varp, &error);
	ਜੇ (!error)
	    strval = tv_get_string_buf_chk(varp, nbuf);
    }
    ਜੇ (!error && strval != ਨਿਰਾ)
	ਸੈੱਟ_option_ਮੁੱਲ_give_err(varname, numval, strval, OPT_LOCAL);
}

/*
 * "setwinvar()" and "settabwinvar()" functions
 */
    ਸਥਿਰ ਖਾਲੀ
setwinvar(typval_T *argvars, ਪੂਰਨ off)
{
    win_T	*win;
    switchwin_T	switchwin;
    ਪੂਰਨ		need_switch_win;
    char_u	*varname, *winvarname;
    typval_T	*varp;
    tabpage_T	*tp = ਨਿਰਾ;

    ਜੇ (check_secure())
	ਵਾਪਸ;

    ਜੇ (off == 1)
	tp = ਲੱਭੋ_tabpage((ਪੂਰਨ)tv_get_ਸੰਖਿਆ_chk(&argvars[0], ਨਿਰਾ));
    ਨਹੀਂਤਾਂ
	tp = curtab;
    win = ਲੱਭੋ_win_by_nr(&argvars[off], tp);
    varname = tv_get_string_chk(&argvars[off + 1]);
    varp = &argvars[off + 2];

    ਜੇ (win == ਨਿਰਾ || varname == ਨਿਰਾ)
	ਵਾਪਸ;

    need_switch_win = !(tp == curtab && win == curwin);
    ਜੇ (!need_switch_win
	    || switch_win(&switchwin, win, tp, TRUE) == OK)
    {
	ਜੇ (*varname == '&')
	    ਸੈੱਟ_option_from_tv(varname + 1, varp);
	ਨਹੀਂਤਾਂ
	{
	    winvarname = alloc(STRLEN(varname) + 3);
	    ਜੇ (winvarname != ਨਿਰਾ)
	    {
		STRCPY(winvarname, "w:");
		STRCPY(winvarname + 2, varname);
		ਸੈੱਟ_var(winvarname, varp, TRUE);
		vim_free(winvarname);
	    }
	}
    }
    ਜੇ (need_switch_win)
	restore_win(&switchwin, TRUE);
}

/*
 * reset v:option_new, v:option_old, v:option_oldlocal, v:option_oldglobal,
 * v:option_type, and v:option_command.
 */
    ਖਾਲੀ
reset_v_option_vars(ਖਾਲੀ)
{
    ਸੈੱਟ_vim_var_string(VV_OPTION_NEW, ਨਿਰਾ, -1);
    ਸੈੱਟ_vim_var_string(VV_OPTION_OLD, ਨਿਰਾ, -1);
    ਸੈੱਟ_vim_var_string(VV_OPTION_OLDLOCAL, ਨਿਰਾ, -1);
    ਸੈੱਟ_vim_var_string(VV_OPTION_OLDGLOBAL, ਨਿਰਾ, -1);
    ਸੈੱਟ_vim_var_string(VV_OPTION_TYPE, ਨਿਰਾ, -1);
    ਸੈੱਟ_vim_var_string(VV_OPTION_COMMAND, ਨਿਰਾ, -1);
}

/*
 * Add an assert error to v:errors.
 */
    ਖਾਲੀ
assert_error(garray_T *gap)
{
    ਬਣਤਰ vimvar   *vp = &vimvars[VV_ERRORS];

    ਜੇ (vp->vv_tv_type != VAR_LIST || vimvars[VV_ERRORS].vv_list == ਨਿਰਾ)
	// Make sure v:errors is a list.
	ਸੈੱਟ_vim_var_list(VV_ERRORS, list_alloc());
    list_append_string(vimvars[VV_ERRORS].vv_list, gap->ga_ਅੰਕੜੇ, gap->ga_len);
}

    ਪੂਰਨ
var_exists(char_u *var)
{
    char_u	*arg = var;
    char_u	*name;
    char_u	*tofree;
    typval_T    tv;
    ਪੂਰਨ		len = 0;
    ਪੂਰਨ		n = FALSE;

    // get_name_len() takes care of expanding curly braces
    name = var;
    len = get_name_len(&arg, &tofree, TRUE, FALSE);
    ਜੇ (len > 0)
    {
	ਜੇ (tofree != ਨਿਰਾ)
	    name = tofree;
	n = (eval_variable(name, len, 0, &tv, ਨਿਰਾ,
				 EVAL_VAR_NOAUTOLOAD + EVAL_VAR_IMPORT) == OK);
	ਜੇ (n)
	{
	    // handle d.key, l[idx], f(expr)
	    arg = skipwhite(arg);
	    n = (handle_subscript(&arg, name, &tv, &EVALARG_EVALUATE,
								 FALSE) == OK);
	    ਜੇ (n)
		clear_tv(&tv);
	}
    }
    ਜੇ (*arg != NUL)
	n = FALSE;

    vim_free(tofree);
    ਵਾਪਸ n;
}

ਸਥਿਰ lval_T	*redir_lval = ਨਿਰਾ;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EVALCMD_BUSY (redir_lval == (lval_T *)&redir_lval)
ਸਥਿਰ garray_T redir_ga;	// only valid when redir_lval is not NULL
ਸਥਿਰ char_u	*redir_endp = ਨਿਰਾ;
ਸਥਿਰ char_u	*redir_varname = ਨਿਰਾ;

    ਪੂਰਨ
alloc_redir_lval(ਖਾਲੀ)
{
    redir_lval = ALLOC_CLEAR_ONE(lval_T);
    ਜੇ (redir_lval == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    ਵਾਪਸ OK;
}

    ਖਾਲੀ
clear_redir_lval(ਖਾਲੀ)
{
    VIM_CLEAR(redir_lval);
}

    ਖਾਲੀ
init_redir_ga(ਖਾਲੀ)
{
    ga_init2(&redir_ga, ਆਕਾਰ(ਅੱਖਰ), 500);
}

/*
 * Start recording command output to a variable
 * When "append" is TRUE append to an existing variable.
 * Returns OK if successfully completed the setup.  FAIL otherwise.
 */
    ਪੂਰਨ
var_redir_ਸ਼ੁਰੂ(char_u *name, ਪੂਰਨ append)
{
    ਪੂਰਨ		called_emsg_before;
    typval_T	tv;

    // Catch a bad name early.
    ਜੇ (!eval_isnamec1(*name))
    {
	emsg(_(e_invalid_ਦਲੀਲ));
	ਵਾਪਸ FAIL;
    }

    // Make a copy of the name, it is used in redir_lval until redir ends.
    redir_varname = vim_strsave(name);
    ਜੇ (redir_varname == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    ਜੇ (alloc_redir_lval() == FAIL)
    {
	var_redir_stop();
	ਵਾਪਸ FAIL;
    }

    // The output is stored in growarray "redir_ga" until redirection ends.
    init_redir_ga();

    // Parse the variable name (can be a dict or list entry).
    redir_endp = get_lval(redir_varname, ਨਿਰਾ, redir_lval, FALSE, FALSE, 0,
							     FNE_CHECK_ਸ਼ੁਰੂ);
    ਜੇ (redir_endp == ਨਿਰਾ || redir_lval->ll_name == ਨਿਰਾ || *redir_endp != NUL)
    {
	clear_lval(redir_lval);
	ਜੇ (redir_endp != ਨਿਰਾ && *redir_endp != NUL)
	    // Trailing characters are present after the variable name
	    semsg(_(e_trailing_characters_str), redir_endp);
	ਨਹੀਂਤਾਂ
	    semsg(_(e_invalid_ਦਲੀਲ_str), name);
	redir_endp = ਨਿਰਾ;  // don't store a value, only cleanup
	var_redir_stop();
	ਵਾਪਸ FAIL;
    }

    // check if we can write to the variable: set it to or append an empty
    // string
    called_emsg_before = called_emsg;
    tv.v_type = VAR_STRING;
    tv.vval.v_string = (char_u *)"";
    ਜੇ (append)
	ਸੈੱਟ_var_lval(redir_lval, redir_endp, &tv, TRUE,
					     ਸੌਂਪੋ_NO_DECL, (char_u *)".", 0);
    ਨਹੀਂਤਾਂ
	ਸੈੱਟ_var_lval(redir_lval, redir_endp, &tv, TRUE,
					     ਸੌਂਪੋ_NO_DECL, (char_u *)"=", 0);
    clear_lval(redir_lval);
    ਜੇ (called_emsg > called_emsg_before)
    {
	redir_endp = ਨਿਰਾ;  // don't store a value, only cleanup
	var_redir_stop();
	ਵਾਪਸ FAIL;
    }

    ਵਾਪਸ OK;
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
    ਖਾਲੀ
var_redir_str(char_u *ਮੁੱਲ, ਪੂਰਨ ਮੁੱਲ_len)
{
    ਪੂਰਨ		len;

    ਜੇ (redir_lval == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (ਮੁੱਲ_len == -1)
	len = (ਪੂਰਨ)STRLEN(ਮੁੱਲ);	// Append the entire string
    ਨਹੀਂਤਾਂ
	len = ਮੁੱਲ_len;		// Append only "value_len" characters

    ਜੇ (ga_grow(&redir_ga, len) == OK)
    {
	mch_memmove((ਅੱਖਰ *)redir_ga.ga_ਅੰਕੜੇ + redir_ga.ga_len, ਮੁੱਲ, len);
	redir_ga.ga_len += len;
    }
    ਨਹੀਂਤਾਂ
	var_redir_stop();
}

/*
 * Stop redirecting command output to a variable.
 * Frees the allocated memory.
 */
    ਖਾਲੀ
var_redir_stop(ਖਾਲੀ)
{
    typval_T	tv;

    ਜੇ (EVALCMD_BUSY)
    {
	redir_lval = ਨਿਰਾ;
	ਵਾਪਸ;
    }

    ਜੇ (redir_lval != ਨਿਰਾ)
    {
	// If there was no error: assign the text to the variable.
	ਜੇ (redir_endp != ਨਿਰਾ)
	{
	    ga_append(&redir_ga, NUL);  // Append the trailing NUL.
	    tv.v_type = VAR_STRING;
	    tv.vval.v_string = redir_ga.ga_ਅੰਕੜੇ;
	    // Call get_lval() again, if it's inside a Dict or List it may
	    // have changed.
	    redir_endp = get_lval(redir_varname, ਨਿਰਾ, redir_lval,
					FALSE, FALSE, 0, FNE_CHECK_ਸ਼ੁਰੂ);
	    ਜੇ (redir_endp != ਨਿਰਾ && redir_lval->ll_name != ਨਿਰਾ)
		ਸੈੱਟ_var_lval(redir_lval, redir_endp, &tv, FALSE, 0,
							     (char_u *)".", 0);
	    clear_lval(redir_lval);
	}

	// free the collected output
	VIM_CLEAR(redir_ga.ga_ਅੰਕੜੇ);

	VIM_CLEAR(redir_lval);
    }
    VIM_CLEAR(redir_varname);
}

/*
 * Get the collected redirected text and clear redir_ga.
 */
    char_u *
get_clear_redir_ga(ਖਾਲੀ)
{
    char_u *res;

    ga_append(&redir_ga, NUL);  // Append the trailing NUL.
    res = redir_ga.ga_ਅੰਕੜੇ;
    redir_ga.ga_ਅੰਕੜੇ = ਨਿਰਾ;
    ਵਾਪਸ res;
}

/*
 * "gettabvar()" function
 */
    ਖਾਲੀ
f_gettabvar(typval_T *argvars, typval_T *rettv)
{
    char_u	*varname;
    tabpage_T	*tp;
    win_T	*win = ਨਿਰਾ;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    varname = tv_get_string_chk(&argvars[1]);
    tp = ਲੱਭੋ_tabpage((ਪੂਰਨ)tv_get_ਸੰਖਿਆ_chk(&argvars[0], ਨਿਰਾ));
    ਜੇ (tp != ਨਿਰਾ)
	win = tp == curtab || tp->tp_firstwin == ਨਿਰਾ ? firstwin
		: tp->tp_firstwin;

    get_var_from(varname, rettv, &argvars[2], 't', tp, win, ਨਿਰਾ);
}

/*
 * "gettabwinvar()" function
 */
    ਖਾਲੀ
f_gettabwinvar(typval_T *argvars, typval_T *rettv)
{
    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL
		|| check_for_ਸੰਖਿਆ_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL))
	ਵਾਪਸ;

    getwinvar(argvars, rettv, 1);
}

/*
 * "getwinvar()" function
 */
    ਖਾਲੀ
f_getwinvar(typval_T *argvars, typval_T *rettv)
{
    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    getwinvar(argvars, rettv, 0);
}

/*
 * "getbufvar()" function
 */
    ਖਾਲੀ
f_getbufvar(typval_T *argvars, typval_T *rettv)
{
    char_u	*varname;
    buf_T	*buf;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_ਬਫਰ_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    varname = tv_get_string_chk(&argvars[1]);
    buf = tv_get_buf_from_arg(&argvars[0]);

    get_var_from(varname, rettv, &argvars[2], 'b', curtab, curwin, buf);
}

/*
 * "settabvar()" function
 */
    ਖਾਲੀ
f_settabvar(typval_T *argvars, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    tabpage_T	*save_curtab;
    tabpage_T	*tp;
    tabpage_T	*save_lu_tp;
    char_u	*varname, *tabvarname;
    typval_T	*varp;

    ਜੇ (check_secure())
	ਵਾਪਸ;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    tp = ਲੱਭੋ_tabpage((ਪੂਰਨ)tv_get_ਸੰਖਿਆ_chk(&argvars[0], ਨਿਰਾ));
    varname = tv_get_string_chk(&argvars[1]);
    varp = &argvars[2];

    ਜੇ (varname == ਨਿਰਾ || tp == ਨਿਰਾ)
	ਵਾਪਸ;

    save_curtab = curtab;
    save_lu_tp = lastused_tabpage;
    goto_tabpage_tp(tp, FALSE, FALSE);

    tabvarname = alloc(STRLEN(varname) + 3);
    ਜੇ (tabvarname != ਨਿਰਾ)
    {
	STRCPY(tabvarname, "t:");
	STRCPY(tabvarname + 2, varname);
	ਸੈੱਟ_var(tabvarname, varp, TRUE);
	vim_free(tabvarname);
    }

    // Restore current tabpage and last accessed tabpage.
    ਜੇ (valid_tabpage(save_curtab))
    {
	goto_tabpage_tp(save_curtab, FALSE, FALSE);
	ਜੇ (valid_tabpage(save_lu_tp))
	    lastused_tabpage = save_lu_tp;
    }
}

/*
 * "settabwinvar()" function
 */
    ਖਾਲੀ
f_settabwinvar(typval_T *argvars, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL
		|| check_for_ਸੰਖਿਆ_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL))
	ਵਾਪਸ;

    setwinvar(argvars, 1);
}

/*
 * "setwinvar()" function
 */
    ਖਾਲੀ
f_setwinvar(typval_T *argvars, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    setwinvar(argvars, 0);
}

/*
 * "setbufvar()" function
 */
    ਖਾਲੀ
f_setbufvar(typval_T *argvars, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    buf_T	*buf;
    char_u	*varname, *bufvarname;
    typval_T	*varp;

    ਜੇ (check_secure())
	ਵਾਪਸ;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_ਬਫਰ_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    varname = tv_get_string_chk(&argvars[1]);
    buf = tv_get_buf_from_arg(&argvars[0]);
    varp = &argvars[2];

    ਜੇ (buf == ਨਿਰਾ || varname == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (*varname == '&')
    {
	aco_save_T	aco;
	// safe the current window position, it could
	// change because of 'scrollbind' window-local
	// options
	linenr_T	old_topline = curwin->w_topline;

	// Set curbuf to be our buf, temporarily.
	aucmd_prepbuf(&aco, buf);
	ਜੇ (curbuf == buf)
	{
	    // Only when it worked to set "curbuf".
	    ਸੈੱਟ_option_from_tv(varname + 1, varp);

	    // reset notion of buffer
	    aucmd_restbuf(&aco);
	}
	curwin->w_topline = old_topline;
    }
    ਨਹੀਂਤਾਂ
    {
	bufvarname = alloc(STRLEN(varname) + 3);
	ਜੇ (bufvarname != ਨਿਰਾ)
	{
	    buf_T *save_curbuf = curbuf;

	    curbuf = buf;
	    STRCPY(bufvarname, "b:");
	    STRCPY(bufvarname + 2, varname);
	    ਸੈੱਟ_var(bufvarname, varp, TRUE);
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
    ਪੂਰਨ		r = OK;

    CLEAR_ਖੇਤਰ(res);
    ਜੇ (arg->v_type == VAR_PARTIAL && arg->vval.v_partial != ਨਿਰਾ)
    {
	res.cb_partial = arg->vval.v_partial;
	++res.cb_partial->pt_refcount;
	res.cb_name = partial_name(res.cb_partial);
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (arg->v_type == VAR_STRING && arg->vval.v_string != ਨਿਰਾ
					       && ਸੁਰੱਖਿਅਤ_isdigit(*arg->vval.v_string))
	    r = FAIL;
	ਨਹੀਂਤਾਂ ਜੇ (arg->v_type == VAR_FUNC || arg->v_type == VAR_STRING)
	{
	    res.cb_name = arg->vval.v_string;
	    ਜੇ (arg->v_type == VAR_STRING)
	    {
		char_u *name = get_scriptlocal_funcname(arg->vval.v_string);
		ਜੇ (name != ਨਿਰਾ)
		{
		    res.cb_name = name;
		    res.cb_free_name = TRUE;
		}
	    }
	    func_ref(res.cb_name);
	}
	ਨਹੀਂਤਾਂ ਜੇ (arg->v_type == VAR_ਸੰਖਿਆ && arg->vval.v_ਸੰਖਿਆ == 0)
	    res.cb_name = (char_u *)"";
	ਨਹੀਂਤਾਂ
	    r = FAIL;

	ਜੇ (r == FAIL)
	{
	    emsg(_(e_invalid_callback_ਦਲੀਲ));
	    res.cb_name = ਨਿਰਾ;
	}
    }
    ਵਾਪਸ res;
}

/*
 * Copy a callback into a typval_T.
 */
    ਖਾਲੀ
put_callback(callback_T *cb, typval_T *tv)
{
    ਜੇ (cb->cb_partial != ਨਿਰਾ)
    {
	tv->v_type = VAR_PARTIAL;
	tv->vval.v_partial = cb->cb_partial;
	++tv->vval.v_partial->pt_refcount;
    }
    ਨਹੀਂਤਾਂ
    {
	tv->v_type = VAR_FUNC;
	tv->vval.v_string = vim_strsave(cb->cb_name);
	func_ref(cb->cb_name);
    }
}

    ਸਥਿਰ bool
does_callback_own_cb_name(callback_T *cb)
{
    // If cb_partial != NULL then *cb->cb_name is owned by the partial.
    ਵਾਪਸ cb->cb_partial || cb->cb_free_name;
}

/*
 * Make a copy of "src" into "dest", allocating the function name if needed,
 * without incrementing the refcount.
 */
    ਖਾਲੀ
ਸੈੱਟ_callback(callback_T *dest, callback_T *src)
{
    *dest = *src;
    ਜੇ (!does_callback_own_cb_name(src))
    {
	dest->cb_name = vim_strsave(src->cb_name);
	dest->cb_free_name = TRUE;
    }
    *src = (callback_T){0};
}

/*
 * Copy callback from "src" to "dest", incrementing the refcounts.
 */
    ਖਾਲੀ
copy_callback(callback_T *dest, callback_T *src)
{
    dest->cb_partial = src->cb_partial;
    ਜੇ (dest->cb_partial != ਨਿਰਾ)
    {
	dest->cb_name = src->cb_name;
	dest->cb_free_name = FALSE;
	++dest->cb_partial->pt_refcount;
    }
    ਨਹੀਂਤਾਂ
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
    ਖਾਲੀ
expand_autload_callback(callback_T *cb)
{
    char_u	*name;
    char_u	*p;
    imported_T	*import;

    ਜੇ (!ਵਿੱਚ_vim9script() || cb->cb_name == ਨਿਰਾ
	    || (!cb->cb_free_name
	       && (cb->cb_partial == ਨਿਰਾ || cb->cb_partial->pt_name == ਨਿਰਾ)))
	ਵਾਪਸ;
    ਜੇ (cb->cb_partial != ਨਿਰਾ)
	name = cb->cb_partial->pt_name;
    ਨਹੀਂਤਾਂ
	name = cb->cb_name;
    p = vim_strchr(name, '.');
    ਜੇ (p == ਨਿਰਾ)
	ਵਾਪਸ;

    import = ਲੱਭੋ_imported(name, p - name, FALSE);
    ਜੇ (import == ਨਿਰਾ || !SCRIPT_ID_VALID(import->imp_sid))
	ਵਾਪਸ;

    scriptitem_T *si = SCRIPT_ਵਸਤੂ(import->imp_sid);
    ਜੇ (si->sn_autoload_ਅਗੇਤਰ == ਨਿਰਾ)
	ਵਾਪਸ;

    char_u *newname = concat_str(si->sn_autoload_ਅਗੇਤਰ, p + 1);
    ਜੇ (newname == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (cb->cb_partial != ਨਿਰਾ)
    {
	ਜੇ (cb->cb_name == cb->cb_partial->pt_name)
	    cb->cb_name = newname;
	vim_free(cb->cb_partial->pt_name);
	cb->cb_partial->pt_name = newname;
    }
    ਨਹੀਂਤਾਂ
    {
	vim_free(cb->cb_name);
	cb->cb_name = newname;
    }
}

/*
 * Unref/free "callback" returned by get_callback() or set_callback().
 */
    ਖਾਲੀ
free_callback(callback_T *callback)
{
    ਜੇ (callback->cb_partial != ਨਿਰਾ)
    {
	partial_unref(callback->cb_partial);
	callback->cb_partial = ਨਿਰਾ;
    }
    ਨਹੀਂਤਾਂ ਜੇ (callback->cb_name != ਨਿਰਾ)
	func_unref(callback->cb_name);
    ਜੇ (callback->cb_free_name)
    {
	vim_free(callback->cb_name);
	callback->cb_free_name = FALSE;
    }
    callback->cb_name = ਨਿਰਾ;
}

#ਜੇ_ਅੰਤ // FEAT_EVAL
