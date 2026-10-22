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

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಆದರೆ defined(FEAT_EVAL)

ಸ್ಥಿರ dictitem_T	globvars_var;		// variable used for g:
ಸ್ಥಿರ dict_T		globvardict;		// Dictionary with g: variables
#ವ್ಯಾಖ್ಯಾನಿಸು globvarht globvardict.dv_hashtab

/*
 * Old Vim variables such as "v:version" are also available without the "v:".
 * Also in functions.  We need a special hashtable for them.
 */
ಸ್ಥಿರ hashtab_T	compat_hashtab;

/*
 * Array to hold the value of v: variables.
 * The value is in a dictitem, so that it can also be used in the v: scope.
 * The reason to use this table anyway is for very quick access to the
 * variables with the VV_ defines.
 */

// values for vv_flags:
#ವ್ಯಾಖ್ಯಾನಿಸು VV_COMPAT	1	// compatible, also used without "v:"
#ವ್ಯಾಖ್ಯಾನಿಸು VV_RO		2	// read-only
#ವ್ಯಾಖ್ಯಾನಿಸು VV_RO_SBX	4	// read-only in the sandbox

#ವ್ಯಾಖ್ಯಾನಿಸು VV_NAME(s, t)	s, {{t, 0, {0}}, 0, {s}}

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ vimvar vimvar_T;

ಸ್ಥಿರ ರಚನೆ vimvar
{
    ಅಕ್ಷರ	*vv_name;	// name of variable, without v:
    dictitem16_T vv_di;		// value and name for key (max 16 chars!)
    type_T	*vv_type;	// type or NULL
    ಅಕ್ಷರ	vv_flags;	// VV_COMPAT, VV_RO, VV_RO_SBX
} vimvars[VV_LEN] =
{
    // The order here must match the VV_ defines in vim.h!
    // Initializing a union does not work, leave tv.vval empty to get zero's.
    {VV_NAME("count",		 VAR_ಸಂಖ್ಯೆ), ಖಾಲಿ, VV_COMPAT+VV_RO},
    {VV_NAME("count1",		 VAR_ಸಂಖ್ಯೆ), ಖಾಲಿ, VV_RO},
    {VV_NAME("prevcount",	 VAR_ಸಂಖ್ಯೆ), ಖಾಲಿ, VV_RO},
    {VV_NAME("errmsg",		 VAR_STRING), ಖಾಲಿ, VV_COMPAT},
    {VV_NAME("warningmsg",	 VAR_STRING), ಖಾಲಿ, 0},
    {VV_NAME("statusmsg",	 VAR_STRING), ಖಾಲಿ, 0},
    {VV_NAME("shell_error",	 VAR_ಸಂಖ್ಯೆ), ಖಾಲಿ, VV_COMPAT+VV_RO},
    {VV_NAME("this_session",	 VAR_STRING), ಖಾಲಿ, VV_COMPAT},
    {VV_NAME("version",		 VAR_ಸಂಖ್ಯೆ), ಖಾಲಿ, VV_COMPAT+VV_RO},
    {VV_NAME("lnum",		 VAR_ಸಂಖ್ಯೆ), ಖಾಲಿ, VV_RO_SBX},
    {VV_NAME("termresponse",	 VAR_STRING), ಖಾಲಿ, VV_RO},
    {VV_NAME("fname",		 VAR_STRING), ಖಾಲಿ, VV_RO},
    {VV_NAME("lang",		 VAR_STRING), ಖಾಲಿ, VV_RO},
    {VV_NAME("lc_time",		 VAR_STRING), ಖಾಲಿ, VV_RO},
    {VV_NAME("ctype",		 VAR_STRING), ಖಾಲಿ, VV_RO},
    {VV_NAME("charconvert_from", VAR_STRING), ಖಾಲಿ, VV_RO},
    {VV_NAME("charconvert_to",	 VAR_STRING), ಖಾಲಿ, VV_RO},
    {VV_NAME("fname_in",	 VAR_STRING), ಖಾಲಿ, VV_RO},
    {VV_NAME("fname_out",	 VAR_STRING), ಖಾಲಿ, VV_RO},
    {VV_NAME("fname_new",	 VAR_STRING), ಖಾಲಿ, VV_RO},
    {VV_NAME("fname_diff",	 VAR_STRING), ಖಾಲಿ, VV_RO},
    {VV_NAME("cmdarg",		 VAR_STRING), ಖಾಲಿ, VV_RO},
    {VV_NAME("foldstart",	 VAR_ಸಂಖ್ಯೆ), ಖಾಲಿ, VV_RO_SBX},
    {VV_NAME("foldend",		 VAR_ಸಂಖ್ಯೆ), ಖಾಲಿ, VV_RO_SBX},
    {VV_NAME("folddashes",	 VAR_STRING), ಖಾಲಿ, VV_RO_SBX},
    {VV_NAME("foldlevel",	 VAR_ಸಂಖ್ಯೆ), ಖಾಲಿ, VV_RO_SBX},
    {VV_NAME("progname",	 VAR_STRING), ಖಾಲಿ, VV_RO},
    {VV_NAME("servername",	 VAR_STRING), ಖಾಲಿ, VV_RO},
    {VV_NAME("dying",		 VAR_ಸಂಖ್ಯೆ), ಖಾಲಿ, VV_RO},
    {VV_NAME("exception",	 VAR_STRING), ಖಾಲಿ, VV_RO},
    {VV_NAME("throwpoint",	 VAR_STRING), ಖಾಲಿ, VV_RO},
    {VV_NAME("register",	 VAR_STRING), ಖಾಲಿ, VV_RO},
    {VV_NAME("cmdbang",		 VAR_ಸಂಖ್ಯೆ), ಖಾಲಿ, VV_RO},
    {VV_NAME("insertmode",	 VAR_STRING), ಖಾಲಿ, VV_RO},
    {VV_NAME("val",		 VAR_UNKNOWN), ಖಾಲಿ, VV_RO},
    {VV_NAME("key",		 VAR_UNKNOWN), ಖಾಲಿ, VV_RO},
    {VV_NAME("profiling",	 VAR_ಸಂಖ್ಯೆ), ಖಾಲಿ, VV_RO},
    {VV_NAME("fcs_reason",	 VAR_STRING), ಖಾಲಿ, VV_RO},
    {VV_NAME("fcs_choice",	 VAR_STRING), ಖಾಲಿ, 0},
    {VV_NAME("beval_bufnr",	 VAR_ಸಂಖ್ಯೆ), ಖಾಲಿ, VV_RO},
    {VV_NAME("beval_winnr",	 VAR_ಸಂಖ್ಯೆ), ಖಾಲಿ, VV_RO},
    {VV_NAME("beval_winid",	 VAR_ಸಂಖ್ಯೆ), ಖಾಲಿ, VV_RO},
    {VV_NAME("beval_lnum",	 VAR_ಸಂಖ್ಯೆ), ಖಾಲಿ, VV_RO},
    {VV_NAME("beval_col",	 VAR_ಸಂಖ್ಯೆ), ಖಾಲಿ, VV_RO},
    {VV_NAME("beval_text",	 VAR_STRING), ಖಾಲಿ, VV_RO},
    {VV_NAME("scrollstart",	 VAR_STRING), ಖಾಲಿ, 0},
    {VV_NAME("swapname",	 VAR_STRING), ಖಾಲಿ, VV_RO},
    {VV_NAME("swapchoice",	 VAR_STRING), ಖಾಲಿ, 0},
    {VV_NAME("swapcommand",	 VAR_STRING), ಖಾಲಿ, VV_RO},
    {VV_NAME("char",		 VAR_STRING), ಖಾಲಿ, 0},
    {VV_NAME("mouse_win",	 VAR_ಸಂಖ್ಯೆ), ಖಾಲಿ, 0},
    {VV_NAME("mouse_winid",	 VAR_ಸಂಖ್ಯೆ), ಖಾಲಿ, 0},
    {VV_NAME("mouse_lnum",	 VAR_ಸಂಖ್ಯೆ), ಖಾಲಿ, 0},
    {VV_NAME("mouse_col",	 VAR_ಸಂಖ್ಯೆ), ಖಾಲಿ, 0},
    {VV_NAME("operator",	 VAR_STRING), ಖಾಲಿ, VV_RO},
    {VV_NAME("searchforward",	 VAR_ಸಂಖ್ಯೆ), ಖಾಲಿ, 0},
    {VV_NAME("hlsearch",	 VAR_ಸಂಖ್ಯೆ), ಖಾಲಿ, 0},
    {VV_NAME("oldfiles",	 VAR_LIST), &t_list_string, 0},
    {VV_NAME("windowid",	 VAR_ಸಂಖ್ಯೆ), ಖಾಲಿ, VV_RO},
    {VV_NAME("progpath",	 VAR_STRING), ಖಾಲಿ, VV_RO},
    {VV_NAME("completed_item",	 VAR_DICT), &t_dict_string, 0},
    {VV_NAME("option_new",	 VAR_STRING), ಖಾಲಿ, VV_RO},
    {VV_NAME("option_old",	 VAR_STRING), ಖಾಲಿ, VV_RO},
    {VV_NAME("option_oldlocal",	 VAR_STRING), ಖಾಲಿ, VV_RO},
    {VV_NAME("option_oldglobal", VAR_STRING), ಖಾಲಿ, VV_RO},
    {VV_NAME("option_command",	 VAR_STRING), ಖಾಲಿ, VV_RO},
    {VV_NAME("option_type",	 VAR_STRING), ಖಾಲಿ, VV_RO},
    {VV_NAME("errors",		 VAR_LIST), &t_list_string, 0},
    {VV_NAME("false",		 VAR_BOOL), ಖಾಲಿ, VV_RO},
    {VV_NAME("true",		 VAR_BOOL), ಖಾಲಿ, VV_RO},
    {VV_NAME("none",		 VAR_SPECIAL), ಖಾಲಿ, VV_RO},
    {VV_NAME("null",		 VAR_SPECIAL), ಖಾಲಿ, VV_RO},
    {VV_NAME("numbermax",	 VAR_ಸಂಖ್ಯೆ), ಖಾಲಿ, VV_RO},
    {VV_NAME("numbermin",	 VAR_ಸಂಖ್ಯೆ), ಖಾಲಿ, VV_RO},
    {VV_NAME("numbersize",	 VAR_ಸಂಖ್ಯೆ), ಖಾಲಿ, VV_RO},
    {VV_NAME("vim_did_enter",	 VAR_ಸಂಖ್ಯೆ), ಖಾಲಿ, VV_RO},
    {VV_NAME("testing",		 VAR_ಸಂಖ್ಯೆ), ಖಾಲಿ, 0},
    {VV_NAME("t_number",	 VAR_ಸಂಖ್ಯೆ), ಖಾಲಿ, VV_RO},
    {VV_NAME("t_string",	 VAR_ಸಂಖ್ಯೆ), ಖಾಲಿ, VV_RO},
    {VV_NAME("t_func",		 VAR_ಸಂಖ್ಯೆ), ಖಾಲಿ, VV_RO},
    {VV_NAME("t_list",		 VAR_ಸಂಖ್ಯೆ), ಖಾಲಿ, VV_RO},
    {VV_NAME("t_dict",		 VAR_ಸಂಖ್ಯೆ), ಖಾಲಿ, VV_RO},
    {VV_NAME("t_float",		 VAR_ಸಂಖ್ಯೆ), ಖಾಲಿ, VV_RO},
    {VV_NAME("t_bool",		 VAR_ಸಂಖ್ಯೆ), ಖಾಲಿ, VV_RO},
    {VV_NAME("t_none",		 VAR_ಸಂಖ್ಯೆ), ಖಾಲಿ, VV_RO},
    {VV_NAME("t_job",		 VAR_ಸಂಖ್ಯೆ), ಖಾಲಿ, VV_RO},
    {VV_NAME("t_channel",	 VAR_ಸಂಖ್ಯೆ), ಖಾಲಿ, VV_RO},
    {VV_NAME("t_blob",		 VAR_ಸಂಖ್ಯೆ), ಖಾಲಿ, VV_RO},
    {VV_NAME("t_class",		 VAR_ಸಂಖ್ಯೆ), ಖಾಲಿ, VV_RO},
    {VV_NAME("t_object",	 VAR_ಸಂಖ್ಯೆ), ಖಾಲಿ, VV_RO},
    {VV_NAME("termrfgresp",	 VAR_STRING), ಖಾಲಿ, VV_RO},
    {VV_NAME("termrbgresp",	 VAR_STRING), ಖಾಲಿ, VV_RO},
    {VV_NAME("termu7resp",	 VAR_STRING), ಖಾಲಿ, VV_RO},
    {VV_NAME("termstyleresp",	 VAR_STRING), ಖಾಲಿ, VV_RO},
    {VV_NAME("termblinkresp",	 VAR_STRING), ಖಾಲಿ, VV_RO},
    {VV_NAME("event",		 VAR_DICT), ಖಾಲಿ, VV_RO},
    {VV_NAME("versionlong",	 VAR_ಸಂಖ್ಯೆ), ಖಾಲಿ, VV_RO},
    {VV_NAME("echospace",	 VAR_ಸಂಖ್ಯೆ), ಖಾಲಿ, VV_RO},
    {VV_NAME("argv",		 VAR_LIST), &t_list_string, VV_RO},
    {VV_NAME("collate",		 VAR_STRING), ಖಾಲಿ, VV_RO},
    {VV_NAME("exiting",		 VAR_SPECIAL), ಖಾಲಿ, VV_RO},
    {VV_NAME("colornames",       VAR_DICT), &t_dict_string, VV_RO},
    {VV_NAME("sizeofint",	 VAR_ಸಂಖ್ಯೆ), ಖಾಲಿ, VV_RO},
    {VV_NAME("sizeoflong",	 VAR_ಸಂಖ್ಯೆ), ಖಾಲಿ, VV_RO},
    {VV_NAME("sizeofpointer",	 VAR_ಸಂಖ್ಯೆ), ಖಾಲಿ, VV_RO},
    {VV_NAME("maxcol",		 VAR_ಸಂಖ್ಯೆ), ಖಾಲಿ, VV_RO},
    {VV_NAME("python3_version",	 VAR_ಸಂಖ್ಯೆ), ಖಾಲಿ, VV_RO},
    {VV_NAME("t_typealias",	 VAR_ಸಂಖ್ಯೆ), ಖಾಲಿ, VV_RO},
    {VV_NAME("t_enum",		 VAR_ಸಂಖ್ಯೆ), ಖಾಲಿ, VV_RO},
    {VV_NAME("t_enumvalue",	 VAR_ಸಂಖ್ಯೆ), ಖಾಲಿ, VV_RO},
    {VV_NAME("stacktrace",	 VAR_LIST), &t_list_dict_any, VV_RO},
    {VV_NAME("t_tuple",		 VAR_ಸಂಖ್ಯೆ), ಖಾಲಿ, VV_RO},
    {VV_NAME("wayland_display",  VAR_STRING), ಖಾಲಿ, VV_RO},
    {VV_NAME("clipmethod",	 VAR_STRING), ಖಾಲಿ, VV_RO},
    {VV_NAME("termda1",		 VAR_STRING), ಖಾಲಿ, VV_RO},
    {VV_NAME("termosc",	 VAR_STRING), ಖಾಲಿ, VV_RO},
    {VV_NAME("vim_did_init",	 VAR_ಸಂಖ್ಯೆ), ಖಾಲಿ, VV_RO},
    {VV_NAME("clipproviders",	 VAR_DICT), ಖಾಲಿ, VV_RO},
    {VV_NAME("imagebackend",	 VAR_STRING), ಖಾಲಿ, VV_RO}
};

// shorthand
#ವ್ಯಾಖ್ಯಾನಿಸು vv_tv_type	vv_di.di_tv.v_type
#ವ್ಯಾಖ್ಯಾನಿಸು vv_nr		vv_di.di_tv.vval.v_ಸಂಖ್ಯೆ
#ವ್ಯಾಖ್ಯಾನಿಸು vv_float	vv_di.di_tv.vval.v_float
#ವ್ಯಾಖ್ಯಾನಿಸು vv_str		vv_di.di_tv.vval.v_string
#ವ್ಯಾಖ್ಯಾನಿಸು vv_list		vv_di.di_tv.vval.v_list
#ವ್ಯಾಖ್ಯಾನಿಸು vv_dict		vv_di.di_tv.vval.v_dict
#ವ್ಯಾಖ್ಯಾನಿಸು vv_blob		vv_di.di_tv.vval.v_blob
#ವ್ಯಾಖ್ಯಾನಿಸು vv_tv		vv_di.di_tv

ಸ್ಥಿರ dictitem_T	vimvars_var;		// variable used for v:
ಸ್ಥಿರ dict_T		vimvardict;		// Dictionary with v: variables
#ವ್ಯಾಖ್ಯಾನಿಸು vimvarht  vimvardict.dv_hashtab

// for VIM_VERSION_ defines
#ಅನ್ತರ್ಭಾವ "version.h"

ಸ್ಥಿರ ಶೂನ್ಯ list_glob_vars(ಪೂರ್ಣಾಂಕ *ಮೊದಲ);
ಸ್ಥಿರ ಶೂನ್ಯ list_buf_vars(ಪೂರ್ಣಾಂಕ *ಮೊದಲ);
ಸ್ಥಿರ ಶೂನ್ಯ list_win_vars(ಪೂರ್ಣಾಂಕ *ಮೊದಲ);
ಸ್ಥಿರ ಶೂನ್ಯ list_tab_vars(ಪೂರ್ಣಾಂಕ *ಮೊದಲ);
ಸ್ಥಿರ char_u *list_arg_vars(exarg_T *eap, char_u *arg, ಪೂರ್ಣಾಂಕ *ಮೊದಲ);
ಸ್ಥಿರ char_u *ex_let_one(char_u *arg, typval_T *tv, ಪೂರ್ಣಾಂಕ copy, ಪೂರ್ಣಾಂಕ flags, char_u *endchars, char_u *op, ಪೂರ್ಣಾಂಕ var_idx);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ do_unlet_var(lval_T *lp, char_u *name_ಕೊನೆ, exarg_T *eap, ಪೂರ್ಣಾಂಕ deep, ಶೂನ್ಯ *cookie);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ do_ಬೀಗ_var(lval_T *lp, char_u *name_ಕೊನೆ, exarg_T *eap, ಪೂರ್ಣಾಂಕ deep, ಶೂನ್ಯ *cookie);
ಸ್ಥಿರ ಶೂನ್ಯ list_one_var(dictitem_T *v, ಅಕ್ಷರ *ಪೂರ್ವಪ್ರತ್ಯಯ, ಪೂರ್ಣಾಂಕ *ಮೊದಲ);
ಸ್ಥಿರ ಶೂನ್ಯ list_one_var_a(ಅಕ್ಷರ *ಪೂರ್ವಪ್ರತ್ಯಯ, char_u *name, ಪೂರ್ಣಾಂಕ type, char_u *string, ಪೂರ್ಣಾಂಕ *ಮೊದಲ);

/*
 * Initialize global and vim special variables
 */
    ಶೂನ್ಯ
evalvars_init(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		    i;
    ರಚನೆ vimvar   *p;

    init_var_dict(&globvardict, &globvars_var, VAR_DEF_SCOPE);
    init_var_dict(&vimvardict, &vimvars_var, VAR_SCOPE);
    vimvardict.dv_ಬೀಗ = VAR_FIXED;
    hash_init(&compat_hashtab);

    ಗಾಗಿ (i = 0; i < VV_LEN; ++i)
    {
	p = &vimvars[i];
	ಆದರೆ (p->vv_flags & VV_RO)
	    p->vv_di.di_flags = DI_FLAGS_RO | DI_FLAGS_FIX;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p->vv_flags & VV_RO_SBX)
	    p->vv_di.di_flags = DI_FLAGS_RO_SBX | DI_FLAGS_FIX;
	ಇಲ್ಲದಿದ್ದರೆ
	    p->vv_di.di_flags = DI_FLAGS_FIX;

	// add to v: scope dict, unless the value is not always available
	ಆದರೆ (p->vv_tv_type != VAR_UNKNOWN)
	    hash_ಕೂಡಿಸು(&vimvarht, p->vv_di.di_ಕೀಲಿ, "initialization");
	ಆದರೆ (p->vv_flags & VV_COMPAT)
	    // add to compat scope dict
	    hash_ಕೂಡಿಸು(&compat_hashtab, p->vv_di.di_ಕೀಲಿ, "initialization");
    }
    ಗಣ_vim_var_nr(VV_ಆವೃತ್ತಿ, VIM_ಆವೃತ್ತಿ_100);
    ಗಣ_vim_var_nr(VV_VERSIONLONG, VIM_ಆವೃತ್ತಿ_100 * 10000 + highest_patch());

    ಗಣ_vim_var_nr(VV_SEARCHFORWARD, 1L);
    ಗಣ_vim_var_nr(VV_HLSEARCH, 1L);
    ಗಣ_vim_var_nr(VV_EXITING, VVAL_NULL);
    ಗಣ_vim_var_dict(VV_COMPLETED_ಅಂಶ, dict_alloc_ಬೀಗ(VAR_FIXED));
    ಗಣ_vim_var_list(VV_ERRORS, list_alloc());
    ಗಣ_vim_var_dict(VV_EVENT, dict_alloc_ಬೀಗ(VAR_FIXED));

    ಗಣ_vim_var_nr(VV_FALSE, VVAL_FALSE);
    ಗಣ_vim_var_nr(VV_TRUE, VVAL_TRUE);
    ಗಣ_vim_var_nr(VV_NONE, VVAL_NONE);
    ಗಣ_vim_var_nr(VV_NULL, VVAL_NULL);
    ಗಣ_vim_var_nr(VV_NUMBERMAX, VARNUM_MAX);
    ಗಣ_vim_var_nr(VV_NUMBERMIN, VARNUM_MIN);
    ಗಣ_vim_var_nr(VV_NUMBERSIZE, ಗಾತ್ರ(varnumber_T) * 8);
    ಗಣ_vim_var_nr(VV_SIZEOFINT, ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ));
    ಗಣ_vim_var_nr(VV_SIZEOFLONG, ಗಾತ್ರ(ದೀರ್ಘ));
    ಗಣ_vim_var_nr(VV_SIZEOFPOINTER, ಗಾತ್ರ(ಅಕ್ಷರ *));
    ಗಣ_vim_var_nr(VV_MAXCOL, MAXCOL);

    ಗಣ_vim_var_nr(VV_TYPE_ಸಂಖ್ಯೆ,  VAR_TYPE_ಸಂಖ್ಯೆ);
    ಗಣ_vim_var_nr(VV_TYPE_STRING,  VAR_TYPE_STRING);
    ಗಣ_vim_var_nr(VV_TYPE_FUNC,    VAR_TYPE_FUNC);
    ಗಣ_vim_var_nr(VV_TYPE_LIST,    VAR_TYPE_LIST);
    ಗಣ_vim_var_nr(VV_TYPE_DICT,    VAR_TYPE_DICT);
    ಗಣ_vim_var_nr(VV_TYPE_FLOAT,   VAR_TYPE_FLOAT);
    ಗಣ_vim_var_nr(VV_TYPE_BOOL,    VAR_TYPE_BOOL);
    ಗಣ_vim_var_nr(VV_TYPE_NONE,    VAR_TYPE_NONE);
    ಗಣ_vim_var_nr(VV_TYPE_ಕೆಲಸ,     VAR_TYPE_ಕೆಲಸ);
    ಗಣ_vim_var_nr(VV_TYPE_CHANNEL, VAR_TYPE_CHANNEL);
    ಗಣ_vim_var_nr(VV_TYPE_BLOB,    VAR_TYPE_BLOB);
    ಗಣ_vim_var_nr(VV_TYPE_CLASS,   VAR_TYPE_CLASS);
    ಗಣ_vim_var_nr(VV_TYPE_OBJECT,  VAR_TYPE_OBJECT);
    ಗಣ_vim_var_nr(VV_TYPE_TYPEALIAS,  VAR_TYPE_TYPEALIAS);
    ಗಣ_vim_var_nr(VV_TYPE_ENUM,    VAR_TYPE_ENUM);
    ಗಣ_vim_var_nr(VV_TYPE_ENUMVALUE,  VAR_TYPE_ENUMVALUE);
    ಗಣ_vim_var_nr(VV_TYPE_TUPLE,   VAR_TYPE_TUPLE);

    ಗಣ_vim_var_nr(VV_ECHOSPACE,    sc_ಲಂಬಸಾಲು - 1);

    ಗಣ_vim_var_dict(VV_COLORNAMES, dict_alloc());
    ಗಣ_vim_var_dict(VV_CLIPPROVIDERS, dict_alloc());

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PYTHON3
    ಗಣ_vim_var_nr(VV_PYTHON3_ಆವೃತ್ತಿ, python3_ಆವೃತ್ತಿ());
#ಆದರೆ_ಅಂತ್ಯ

    // Default for v:register is not 0 but '"'.  This is adjusted once the
    // clipboard has been setup by calling reset_reg_var().
    ಗಣ_reg_var(0);
}

#ಆದರೆ defined(EXITFREE)
/*
 * Free all vim variables information on exit
 */
    ಶೂನ್ಯ
evalvars_clear(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		    i;
    ರಚನೆ vimvar   *p;

    ಗಾಗಿ (i = 0; i < VV_LEN; ++i)
    {
	p = &vimvars[i];
	ಆದರೆ (p->vv_di.di_tv.v_type == VAR_STRING)
	    VIM_CLEAR(p->vv_str);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p->vv_di.di_tv.v_type == VAR_LIST)
	{
	    list_unref(p->vv_list);
	    p->vv_list = ಖಾಲಿ;
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
    ಗಾಗಿ (i = 1; i <= script_ವಸ್ತುಗಳು.ga_len; ++i)
	vars_clear(&SCRIPT_VARS(i));
}
#ಆದರೆ_ಅಂತ್ಯ

    ಪೂರ್ಣಾಂಕ
garbage_collect_globvars(ಪೂರ್ಣಾಂಕ copyID)
{
    ಹಿಂತಿರುಗಿ ಗಣ_ref_ಒಳ_ht(&globvarht, copyID, ಖಾಲಿ, ಖಾಲಿ);
}

    ಪೂರ್ಣಾಂಕ
garbage_collect_vimvars(ಪೂರ್ಣಾಂಕ copyID)
{
    ಹಿಂತಿರುಗಿ ಗಣ_ref_ಒಳ_ht(&vimvarht, copyID, ಖಾಲಿ, ಖಾಲಿ);
}

    ಪೂರ್ಣಾಂಕ
garbage_collect_scriptvars(ಪೂರ್ಣಾಂಕ copyID)
{
    ಪೂರ್ಣಾಂಕ		    i;
    ಪೂರ್ಣಾಂಕ		    idx;
    ಪೂರ್ಣಾಂಕ		    abort = FALSE;
    scriptitem_T    *si;

    ಗಾಗಿ (i = 1; i <= script_ವಸ್ತುಗಳು.ga_len; ++i)
    {
	abort = abort || ಗಣ_ref_ಒಳ_ht(&SCRIPT_VARS(i), copyID, ಖಾಲಿ, ಖಾಲಿ);

	si = SCRIPT_ಅಂಶ(i);
	ಗಾಗಿ (idx = 0; idx < si->sn_var_vals.ga_len; ++idx)
	{
	    svar_T    *sv = ((svar_T *)si->sn_var_vals.ga_ದತ್ತಾಂಶ) + idx;

	    ಆದರೆ (sv->sv_name != ಖಾಲಿ)
		abort = abort || ಗಣ_ref_ಒಳ_ಅಂಶ(sv->sv_tv, copyID, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ);
	}
    }

    ಹಿಂತಿರುಗಿ abort;
}

/*
 * Set an internal variable to a string value. Creates the variable if it does
 * not already exist.
 */
    ಶೂನ್ಯ
ಗಣ_ಆಂತರಿಕ_string_var(char_u *name, char_u *ಮೌಲ್ಯ)
{
    char_u	*val;
    typval_T	*tvp;

    val = vim_strsave(ಮೌಲ್ಯ);
    ಆದರೆ (val == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    tvp = alloc_string_tv(val);
    ಆದರೆ (tvp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಗಣ_var(name, tvp, FALSE);
    free_tv(tvp);
}

    ಪೂರ್ಣಾಂಕ
eval_charconvert(
    char_u	*enc_from,
    char_u	*enc_ಗಮ್ಯ,
    char_u	*fname_from,
    char_u	*fname_ಗಮ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		err = FALSE;
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;

    ಗಣ_vim_var_string(VV_CC_FROM, enc_from, -1);
    ಗಣ_vim_var_string(VV_CC_ಗಮ್ಯ, enc_ಗಮ್ಯ, -1);
    ಗಣ_vim_var_string(VV_FNAME_ಒಳ, fname_from, -1);
    ಗಣ_vim_var_string(VV_FNAME_ಹೊರಹೋಗು, fname_ಗಮ್ಯ, -1);
    ctx = get_option_sctx("charconvert");
    ಆದರೆ (ctx != ಖಾಲಿ)
	current_sctx = *ctx;

    ಆದರೆ (eval_ಗಮ್ಯ_bool(p_ccv, &err, ಖಾಲಿ, FALSE, TRUE))
	err = TRUE;

    ಗಣ_vim_var_string(VV_CC_FROM, ಖಾಲಿ, -1);
    ಗಣ_vim_var_string(VV_CC_ಗಮ್ಯ, ಖಾಲಿ, -1);
    ಗಣ_vim_var_string(VV_FNAME_ಒಳ, ಖಾಲಿ, -1);
    ಗಣ_vim_var_string(VV_FNAME_ಹೊರಹೋಗು, ಖಾಲಿ, -1);
    current_sctx = saved_sctx;

    ಆದರೆ (err)
	ಹಿಂತಿರುಗಿ FAIL;
    ಹಿಂತಿರುಗಿ OK;
}

#ಆದರೆ defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
    ಪೂರ್ಣಾಂಕ
eval_printexpr(char_u *fname, char_u *ವಾದಗಳು)
{
    ಪೂರ್ಣಾಂಕ		err = FALSE;
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;

    ಗಣ_vim_var_string(VV_FNAME_ಒಳ, fname, -1);
    ಗಣ_vim_var_string(VV_CMDARG, ವಾದಗಳು, -1);
    ctx = get_option_sctx("printexpr");
    ಆದರೆ (ctx != ಖಾಲಿ)
	current_sctx = *ctx;

    ಆದರೆ (eval_ಗಮ್ಯ_bool(p_pexpr, &err, ಖಾಲಿ, FALSE, TRUE))
	err = TRUE;

    ಗಣ_vim_var_string(VV_FNAME_ಒಳ, ಖಾಲಿ, -1);
    ಗಣ_vim_var_string(VV_CMDARG, ಖಾಲಿ, -1);
    current_sctx = saved_sctx;

    ಆದರೆ (err)
    {
	mch_remove(fname);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_DIFF)
    ಶೂನ್ಯ
eval_diff(
    char_u	*origfile,
    char_u	*newfile,
    char_u	*outfile)
{
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;
    typval_T	*tv;

    ಗಣ_vim_var_string(VV_FNAME_ಒಳ, origfile, -1);
    ಗಣ_vim_var_string(VV_FNAME_NEW, newfile, -1);
    ಗಣ_vim_var_string(VV_FNAME_ಹೊರಹೋಗು, outfile, -1);

    ctx = get_option_sctx("diffexpr");
    ಆದರೆ (ctx != ಖಾಲಿ)
	current_sctx = *ctx;

    // errors are ignored
    tv = eval_ಅಭಿವ್ಯಕ್ತಿ_ext(p_dex, ಖಾಲಿ, TRUE);
    free_tv(tv);

    ಗಣ_vim_var_string(VV_FNAME_ಒಳ, ಖಾಲಿ, -1);
    ಗಣ_vim_var_string(VV_FNAME_NEW, ಖಾಲಿ, -1);
    ಗಣ_vim_var_string(VV_FNAME_ಹೊರಹೋಗು, ಖಾಲಿ, -1);
    current_sctx = saved_sctx;
}

    ಶೂನ್ಯ
eval_patch(
    char_u	*origfile,
    char_u	*difffile,
    char_u	*outfile)
{
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;
    typval_T	*tv;

    ಗಣ_vim_var_string(VV_FNAME_ಒಳ, origfile, -1);
    ಗಣ_vim_var_string(VV_FNAME_DIFF, difffile, -1);
    ಗಣ_vim_var_string(VV_FNAME_ಹೊರಹೋಗು, outfile, -1);

    ctx = get_option_sctx("patchexpr");
    ಆದರೆ (ctx != ಖಾಲಿ)
	current_sctx = *ctx;

    // errors are ignored
    tv = eval_ಅಭಿವ್ಯಕ್ತಿ_ext(p_pex, ಖಾಲಿ, TRUE);
    free_tv(tv);

    ಗಣ_vim_var_string(VV_FNAME_ಒಳ, ಖಾಲಿ, -1);
    ಗಣ_vim_var_string(VV_FNAME_DIFF, ಖಾಲಿ, -1);
    ಗಣ_vim_var_string(VV_FNAME_ಹೊರಹೋಗು, ಖಾಲಿ, -1);
    current_sctx = saved_sctx;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_SPELL)
/*
 * Evaluate an expression to a list with suggestions.
 * For the "expr:" part of 'spellsuggest'.
 * Returns NULL when there is an error.
 */
    list_T *
eval_spell_ಅಭಿವ್ಯಕ್ತಿ(char_u *badword, char_u *ಅಭಿವ್ಯಕ್ತಿ)
{
    typval_T	save_val;
    typval_T	rettv;
    list_T	*list = ಖಾಲಿ;
    char_u	*p = skipwhite(ಅಭಿವ್ಯಕ್ತಿ);
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;
    ಪೂರ್ಣಾಂಕ		r;

    // Set "v:val" to the bad word.
    prepare_vimvar(VV_VAL, &save_val);
    ಗಣ_vim_var_string(VV_VAL, badword, -1);
    ಆದರೆ (p_verbose == 0)
	++emsg_off;
    ctx = get_option_sctx("spellsuggest");
    ಆದರೆ (ctx != ಖಾಲಿ)
	current_sctx = *ctx;

    r = may_call_simple_func(p, &rettv);
    ಆದರೆ (r == NOTDONE)
	r = eval1(&p, &rettv, &EVALARG_EVALUATE);
    ಆದರೆ (r == OK)
    {
	ಆದರೆ (rettv.v_type != VAR_LIST)
	    clear_tv(&rettv);
	ಇಲ್ಲದಿದ್ದರೆ
	    list = rettv.vval.v_list;
    }

    ಆದರೆ (p_verbose == 0)
	--emsg_off;
    clear_tv(get_vim_var_tv(VV_VAL));
    restore_vimvar(VV_VAL, &save_val);
    current_sctx = saved_sctx;

    ಹಿಂತಿರುಗಿ list;
}

/*
 * "list" is supposed to contain two items: a word and a number.  Return the
 * word in "pp" and the number as the return value.
 * Return -1 if anything isn't right.
 * Used to get the good word and score from the eval_spell_expr() result.
 */
    ಪೂರ್ಣಾಂಕ
get_spellword(list_T *list, char_u **pp)
{
    listitem_T	*li;

    li = list->lv_ಮೊದಲ;
    ಆದರೆ (li == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ -1;
    *pp = tv_get_string(&li->li_tv);

    li = li->li_ಮುಂದಿನ;
    ಆದರೆ (li == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ -1;
    ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ(&li->li_tv);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Prepare v: variable "idx" to be used.
 * Save the current typeval in "save_tv" and clear it.
 * When not used yet add the variable to the v: hashtable.
 */
    ಶೂನ್ಯ
prepare_vimvar(ಪೂರ್ಣಾಂಕ idx, typval_T *save_tv)
{
    *save_tv = vimvars[idx].vv_tv;
    vimvars[idx].vv_str = ಖಾಲಿ;  // don't free it now
    ಆದರೆ (vimvars[idx].vv_tv_type == VAR_UNKNOWN)
	hash_ಕೂಡಿಸು(&vimvarht, vimvars[idx].vv_di.di_ಕೀಲಿ, "prepare vimvar");
}

/*
 * Restore v: variable "idx" to typeval "save_tv".
 * Note that the v: variable must have been cleared already.
 * When no longer defined, remove the variable from the v: hashtable.
 */
    ಶೂನ್ಯ
restore_vimvar(ಪೂರ್ಣಾಂಕ idx, typval_T *save_tv)
{
    hashitem_T	*hi;

    vimvars[idx].vv_tv = *save_tv;
    ಆದರೆ (vimvars[idx].vv_tv_type != VAR_UNKNOWN)
	ಹಿಂತಿರುಗಿ;

    hi = hash_ಕಂಡುಹಿಡಿ(&vimvarht, vimvars[idx].vv_di.di_ಕೀಲಿ);
    ಆದರೆ (HASHITEM_ಖಾಲಿಗಳು(hi))
	ಆಂತರಿಕ_error("restore_vimvar()");
    ಇಲ್ಲದಿದ್ದರೆ
	hash_remove(&vimvarht, hi, "restore vimvar");
}

/*
 * List Vim variables.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
list_vim_vars(ಪೂರ್ಣಾಂಕ *ಮೊದಲ)
{
    list_hashtable_vars(&vimvarht, "v:", FALSE, ಮೊದಲ);
}

/*
 * List script-local variables, if there is a script.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
list_script_vars(ಪೂರ್ಣಾಂಕ *ಮೊದಲ)
{
    ಆದರೆ (SCRIPT_ID_VALID(current_sctx.sc_sid))
	list_hashtable_vars(&SCRIPT_VARS(current_sctx.sc_sid),
							   "s:", FALSE, ಮೊದಲ);
}

/*
 * Return TRUE if "name" starts with "g:", "w:", "t:" or "b:".
 * But only when an identifier character follows.
 */
    ಪೂರ್ಣಾಂಕ
is_scoped_variable(char_u *name)
{
    ಹಿಂತಿರುಗಿ vim_strchr((char_u *)"gwbt", name[0]) != ಖಾಲಿ
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
eval_one_ಅಭಿವ್ಯಕ್ತಿ_ಒಳ_str(char_u *p, garray_T *gap, ಪೂರ್ಣಾಂಕ evaluate)
{
    char_u	*ಖಂಡ_ಆರಂಭ = skipwhite(p + 1);  // skip the opening {
    char_u	*ಖಂಡ_ಕೊನೆ = ಖಂಡ_ಆರಂಭ;
    char_u	*ಅಭಿವ್ಯಕ್ತಿ_val;

    ಆದರೆ (*ಖಂಡ_ಆರಂಭ == NUL)
    {
	semsg(_(e_missing_close_curly_str), p);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಆದರೆ (skip_ಅಭಿವ್ಯಕ್ತಿ(&ಖಂಡ_ಕೊನೆ, ಖಾಲಿ) == FAIL)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಖಂಡ_ಕೊನೆ = skipwhite(ಖಂಡ_ಕೊನೆ);
    ಆದರೆ (*ಖಂಡ_ಕೊನೆ != '}')
    {
	semsg(_(e_missing_close_curly_str), p);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಆದರೆ (evaluate)
    {
	*ಖಂಡ_ಕೊನೆ = NUL;
	ಅಭಿವ್ಯಕ್ತಿ_val = eval_ಗಮ್ಯ_string(ಖಂಡ_ಆರಂಭ, FALSE, FALSE);
	*ಖಂಡ_ಕೊನೆ = '}';
	ಆದರೆ (ಅಭಿವ್ಯಕ್ತಿ_val == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	ga_concat(gap, ಅಭಿವ್ಯಕ್ತಿ_val);
	vim_free(ಅಭಿವ್ಯಕ್ತಿ_val);
    }

    ಹಿಂತಿರುಗಿ ಖಂಡ_ಕೊನೆ + 1;
}

/*
 * Evaluate all the Vim expressions {expr} in "str" and return the resulting
 * string in allocated memory.  "{{" is reduced to "{" and "}}" to "}".
 * Used for a heredoc assignment.
 * Returns NULL for an error.
 */
    ಸ್ಥಿರ char_u *
eval_all_ಅಭಿವ್ಯಕ್ತಿ_ಒಳ_str(char_u *str)
{
    garray_T	ga;
    char_u	*p;

    ga_init2(&ga, 1, 80);
    p = str;

    ಆಗ (*p != NUL)
    {
	char_u	*lit_ಆರಂಭ;
	ಪೂರ್ಣಾಂಕ	escaped_brace = FALSE;

	// Look for a block start.
	lit_ಆರಂಭ = p;
	ಆಗ (*p != '{' && *p != '}' && *p != NUL)
	    ++p;

	ಆದರೆ (*p != NUL && *p == p[1])
	{
	    // Escaped brace, unescape and continue.
	    // Include the brace in the literal string.
	    ++p;
	    escaped_brace = TRUE;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == '}')
	{
	    semsg(_(e_stray_closing_curly_str), str);
	    ga_clear(&ga);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}

	// Append the literal part.
	ga_concat_len(&ga, lit_ಆರಂಭ, (size_t)(p - lit_ಆರಂಭ));

	ಆದರೆ (*p == NUL)
	    ನಿಲ್ಲಿಸು;

	ಆದರೆ (escaped_brace)
	{
	    // Skip the second brace.
	    ++p;
	    ಮುಂದುವರಿ;
	}

	// Evaluate the expression and append the result.
	p = eval_one_ಅಭಿವ್ಯಕ್ತಿ_ಒಳ_str(p, &ga, TRUE);
	ಆದರೆ (p == ಖಾಲಿ)
	{
	    ga_clear(&ga);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
    }
    ga_append(&ga, NUL);

    ಹಿಂತಿರುಗಿ ga.ga_ದತ್ತಾಂಶ;
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
heredoc_get(exarg_T *eap, char_u *cmd, ಪೂರ್ಣಾಂಕ script_get, ಪೂರ್ಣಾಂಕ vim9compile)
{
    char_u	*theline = ಖಾಲಿ;
    char_u	*marker;
    list_T	*l;
    char_u	*p;
    char_u	*str;
    ಪೂರ್ಣಾಂಕ		marker_indent_len = 0;
    ಪೂರ್ಣಾಂಕ		ಪಠ್ಯ_indent_len = 0;
    char_u	*ಪಠ್ಯ_indent = ಖಾಲಿ;
    char_u	dot[] = ".";
    ಪೂರ್ಣಾಂಕ		comment_char = ಒಳ_vim9script() ? '#' : '"';
    ಪೂರ್ಣಾಂಕ		evalstr = FALSE;
    ಪೂರ್ಣಾಂಕ		eval_failed = FALSE;
    cctx_T	*cctx = vim9compile ? eap->cookie : ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2 = 0;
    ಪೂರ್ಣಾಂಕ		heredoc_ಒಳ_string = FALSE;
    char_u	*ಸಾಲು_arg = ಖಾಲಿ;
    char_u	*nl_ptr = vim_strchr(cmd, '\n');

    ಆದರೆ (nl_ptr != ಖಾಲಿ)
    {
	heredoc_ಒಳ_string = TRUE;
	ಸಾಲು_arg = nl_ptr + 1;
	*nl_ptr = NUL;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (eap->ea_getline == ಖಾಲಿ)
    {
	emsg(_(e_cannot_use_heredoc_ಇಲ್ಲಿ));
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    // Check for the optional 'trim' word before the marker
    cmd = skipwhite(cmd);

    ಆಗ (TRUE)
    {
	ಆದರೆ (STRNCMP(cmd, "trim", 4) == 0
		&& (cmd[4] == NUL || VIM_ISWHITE(cmd[4])))
	{
	    cmd = skipwhite(cmd + 4);

	    // Trim the indentation from all the lines in the here document.
	    // The amount of indentation trimmed is the same as the indentation
	    // of the first line after the :let command line.  To find the end
	    // marker the indent of the :let command line is trimmed.
	    p = *eap->cmdlinep;
	    ಆಗ (VIM_ISWHITE(*p))
	    {
		p++;
		marker_indent_len++;
	    }
	    ಪಠ್ಯ_indent_len = -1;

	    ಮುಂದುವರಿ;
	}
	ಆದರೆ (STRNCMP(cmd, "eval", 4) == 0
		&& (cmd[4] == NUL || VIM_ISWHITE(cmd[4])))
	{
	    cmd = skipwhite(cmd + 4);
	    evalstr = TRUE;
	    ಮುಂದುವರಿ;
	}
	ನಿಲ್ಲಿಸು;
    }

    // The marker is the next word.
    ಆದರೆ (*cmd != NUL && *cmd != comment_char)
    {
	marker = skipwhite(cmd);
	p = skiptowhite(marker);
	ಆದರೆ (*skipwhite(p) != NUL && *skipwhite(p) != comment_char)
	{
	    semsg(_(e_trailing_characters_str), p);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
	*p = NUL;
	ಆದರೆ (!script_get && vim_islower(*marker))
	{
	    emsg(_(e_marker_cannot_ಆರಂಭ_with_ಅಧೋ_case_letter));
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// When getting lines for an embedded script, if the marker is missing,
	// accept '.' as the marker.
	ಆದರೆ (script_get)
	    marker = dot;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    emsg(_(e_missing_marker));
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
    }

    l = list_alloc();
    ಆದರೆ (l == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಗಾಗಿ (;;)
    {
	ಪೂರ್ಣಾಂಕ	mi = 0;
	ಪೂರ್ಣಾಂಕ	ti = 0;

	ಆದರೆ (heredoc_ಒಳ_string)
	{
	    char_u	*ಮುಂದಿನ_ಸಾಲು;

	    // heredoc in a string separated by newlines.  Get the next line
	    // from the string.

	    ಆದರೆ (*ಸಾಲು_arg == NUL)
	    {
		semsg(_(e_missing_ಕೊನೆ_marker_str), marker);
		ನಿಲ್ಲಿಸು;
	    }

	    theline = ಸಾಲು_arg;
	    ಮುಂದಿನ_ಸಾಲು = vim_strchr(theline, '\n');
	    ಆದರೆ (ಮುಂದಿನ_ಸಾಲು == ಖಾಲಿ)
		ಸಾಲು_arg += STRLEN(ಸಾಲು_arg);
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		*ಮುಂದಿನ_ಸಾಲು = NUL;
		ಸಾಲು_arg = ಮುಂದಿನ_ಸಾಲು + 1;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    vim_free(theline);
	    theline = eap->ea_getline(NUL, eap->cookie, 0, FALSE);
	    ಆದರೆ (theline == ಖಾಲಿ)
	    {
		semsg(_(e_missing_ಕೊನೆ_marker_str), marker);
		ನಿಲ್ಲಿಸು;
	    }
	}

	// with "trim": skip the indent matching the :let line to find the
	// marker
	ಆದರೆ (marker_indent_len > 0
		&& STRNCMP(theline, *eap->cmdlinep, marker_indent_len) == 0)
	    mi = marker_indent_len;
	ಆದರೆ (STRCMP(marker, theline + mi) == 0)
	    ನಿಲ್ಲಿಸು;

	// If expression evaluation failed in the heredoc, then skip till the
	// end marker.
	ಆದರೆ (eval_failed)
	    ಮುಂದುವರಿ;

	ಆದರೆ (ಪಠ್ಯ_indent_len == -1 && *theline != NUL)
	{
	    // set the text indent from the first line.
	    p = theline;
	    ಪಠ್ಯ_indent_len = 0;
	    ಆಗ (VIM_ISWHITE(*p))
	    {
		p++;
		ಪಠ್ಯ_indent_len++;
	    }
	    ಪಠ್ಯ_indent = vim_strnsave(theline, ಪಠ್ಯ_indent_len);
	}
	// with "trim": skip the indent matching the first line
	ಆದರೆ (ಪಠ್ಯ_indent != ಖಾಲಿ)
	    ಗಾಗಿ (ti = 0; ti < ಪಠ್ಯ_indent_len; ++ti)
		ಆದರೆ (theline[ti] != ಪಠ್ಯ_indent[ti])
		    ನಿಲ್ಲಿಸು;

	str = theline + ti;
	ಆದರೆ (vim9compile)
	{
	    ಆದರೆ (compile_all_ಅಭಿವ್ಯಕ್ತಿ_ಒಳ_str(str, evalstr, cctx) == FAIL)
	    {
		vim_free(theline);
		vim_free(ಪಠ್ಯ_indent);
		list_free(l);
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    ಎಣಿಕೆ_2++;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಪೂರ್ಣಾಂಕ	    free_str = FALSE;

	    ಆದರೆ (evalstr && !eap->skip)
	    {
		str = eval_all_ಅಭಿವ್ಯಕ್ತಿ_ಒಳ_str(str);
		ಆದರೆ (str == ಖಾಲಿ)
		{
		    // expression evaluation failed
		    eval_failed = TRUE;
		    ಮುಂದುವರಿ;
		}
		free_str = TRUE;
	    }

	    ಆದರೆ (list_append_string(l, str, -1) == FAIL)
	    {
		ಆದರೆ (free_str)
		    vim_free(str);
		ನಿಲ್ಲಿಸು;
	    }
	    ಆದರೆ (free_str)
		vim_free(str);
	}
    }
    ಆದರೆ (heredoc_ಒಳ_string)
	// Next command follows the heredoc in the string.
	eap->nextcmd = ಸಾಲು_arg;
    ಇಲ್ಲದಿದ್ದರೆ
	vim_free(theline);
    vim_free(ಪಠ್ಯ_indent);

    ಆದರೆ (vim9compile && cctx->ctx_skip != SKIP_YES && !eval_failed)
	generate_NEWLIST(cctx, ಎಣಿಕೆ_2, FALSE);

    ಆದರೆ (eval_failed)
    {
	// expression evaluation in the heredoc failed
	list_free(l);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಹಿಂತಿರುಗಿ l;
}

/*
 * Vim9 variable declaration:
 * ":var name"
 * ":var name: type"
 * ":var name = expr"
 * ":var name: type = expr"
 * etc.
 */
    ಶೂನ್ಯ
ex_var(exarg_T *eap)
{
    char_u *p = eap->cmd;
    ಪೂರ್ಣಾಂಕ	    has_var;

    ಆದರೆ (!ಒಳ_vim9script())
    {
	semsg(_(e_str_cannot_be_ಬಳಸಿದ_ಒಳ_legacy_vim_script), ":var");
	ಹಿಂತಿರುಗಿ;
    }
    has_var = checkforcmd_noparen(&p, "var", 3);
    ಆದರೆ (current_sctx.sc_sid == 0 && has_var)
    {
	emsg(_(e_cannot_declare_variable_on_command_ಸಾಲು));
	ಹಿಂತಿರುಗಿ;
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
    ಶೂನ್ಯ
ex_let(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    char_u	*ಅಭಿವ್ಯಕ್ತಿ = ಖಾಲಿ;
    typval_T	rettv;
    ಪೂರ್ಣಾಂಕ		var_ಎಣಿಕೆ = 0;
    ಪೂರ್ಣಾಂಕ		semicolon = 0;
    char_u	op[4];
    char_u	*argend;
    ಪೂರ್ಣಾಂಕ		ಮೊದಲ = TRUE;
    ಪೂರ್ಣಾಂಕ		concat;
    ಪೂರ್ಣಾಂಕ		has_ನಿಯೋಜಿಸು;
    ಪೂರ್ಣಾಂಕ		flags = 0;
    ಪೂರ್ಣಾಂಕ		vim9script = ಒಳ_vim9script();

    ಆದರೆ (eap->cmdidx == CMD_final && !vim9script)
    {
	// In legacy Vim script ":final" is short for ":finally".
	ex_finally(eap);
	ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (eap->cmdidx == CMD_let && vim9script)
    {
	emsg(_(e_cannot_use_let_ಒಳ_vim9_script));
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (eap->cmdidx == CMD_const)
	flags |= ನಿಯೋಜಿಸು_CONST;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (eap->cmdidx == CMD_final)
	flags |= ನಿಯೋಜಿಸು_FINAL;

    // Vim9 assignment without ":let", ":const" or ":final"
    ಆದರೆ (eap->arg == eap->cmd)
	flags |= ನಿಯೋಜಿಸು_NO_DECL;

    argend = skip_var_list(arg, TRUE, &var_ಎಣಿಕೆ, &semicolon, FALSE);
    ಆದರೆ (argend == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    ಅಭಿವ್ಯಕ್ತಿ = skipwhite(argend);
    concat = ಅಭಿವ್ಯಕ್ತಿ[0] == '.'
	&& ((ಅಭಿವ್ಯಕ್ತಿ[1] == '=' && ಒಳ_old_script(2))
		|| (ಅಭಿವ್ಯಕ್ತಿ[1] == '.' && ಅಭಿವ್ಯಕ್ತಿ[2] == '='));
    has_ನಿಯೋಜಿಸು =  *ಅಭಿವ್ಯಕ್ತಿ == '=' || (vim_strchr((char_u *)"+-*/%", *ಅಭಿವ್ಯಕ್ತಿ) != ಖಾಲಿ
							    && ಅಭಿವ್ಯಕ್ತಿ[1] == '=');
    ಆದರೆ (!has_ನಿಯೋಜಿಸು && !concat)
    {
	// ":let" without "=": list variables
	ಆದರೆ (*arg == '[')
	    emsg(_(e_invalid_ವಾದ));
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಅಭಿವ್ಯಕ್ತಿ[0] == '.' && ಅಭಿವ್ಯಕ್ತಿ[1] == '=')
	    emsg(_(e_dot_equal_not_supported_with_script_ಆವೃತ್ತಿ_two));
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!ತುದಿಗಳು_excmd2(eap->cmd, arg))
	{
	    ಆದರೆ (vim9script)
	    {
		ಆದರೆ (!ತುದಿಗಳು_excmd2(eap->cmd, skipwhite(argend)))
		    semsg(_(e_trailing_characters_str), argend);
		ಇಲ್ಲದಿದ್ದರೆ
		    // Vim9 declaration ":var name: type"
		    arg = vim9_declare_scriptvar(eap, arg);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// ":let var1 var2" - list values
		arg = list_arg_vars(eap, arg, &ಮೊದಲ);
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!eap->skip)
	{
	    // ":let"
	    list_glob_vars(&ಮೊದಲ);
	    list_buf_vars(&ಮೊದಲ);
	    list_win_vars(&ಮೊದಲ);
	    list_tab_vars(&ಮೊದಲ);
	    list_script_vars(&ಮೊದಲ);
	    list_func_vars(&ಮೊದಲ);
	    list_vim_vars(&ಮೊದಲ);
	}
	ಗಣ_nextcmd(eap, arg);
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (ಅಭಿವ್ಯಕ್ತಿ[0] == '=' && ಅಭಿವ್ಯಕ್ತಿ[1] == '<' && ಅಭಿವ್ಯಕ್ತಿ[2] == '<')
    {
	list_T	*l = ಖಾಲಿ;
	ದೀರ್ಘ	cur_lnum = SOURCING_LNUM;

	// :let text =<< [trim] [eval] END
	// :var text =<< [trim] [eval] END
	ಆದರೆ (vim9script && !eap->skip && (!VIM_ISWHITE(ಅಭಿವ್ಯಕ್ತಿ[-1])
						 || !IS_WHITE_OR_NUL(ಅಭಿವ್ಯಕ್ತಿ[3])))
	    semsg(_(e_white_space_required_before_and_after_str_at_str),
								  "=<<", ಅಭಿವ್ಯಕ್ತಿ);
	ಇಲ್ಲದಿದ್ದರೆ
	    l = heredoc_get(eap, ಅಭಿವ್ಯಕ್ತಿ + 3, FALSE, FALSE);

	ಆದರೆ (l != ಖಾಲಿ)
	{
	    rettv_list_ಗಣ(&rettv, l);
	    ಆದರೆ (!eap->skip)
	    {
		// errors are for the assignment, not the end marker
		SOURCING_LNUM = cur_lnum;
		op[0] = '=';
		op[1] = NUL;
		(ಶೂನ್ಯ)ex_let_vars(eap->arg, &rettv, FALSE, semicolon, var_ಎಣಿಕೆ,
								flags, op);
	    }
	    clear_tv(&rettv);
	}
	ಹಿಂತಿರುಗಿ;
    }

    evalarg_T   evalarg;
    ಪೂರ್ಣಾಂಕ	    len = 1;

    CLEAR_ಕ್ಷೇತ್ರ(rettv);

    ಪೂರ್ಣಾಂಕ cur_lnum;

    op[0] = '=';
    op[1] = NUL;
    ಆದರೆ (*ಅಭಿವ್ಯಕ್ತಿ != '=')
    {
	ಆದರೆ (vim9script && (flags & ನಿಯೋಜಿಸು_NO_DECL) == 0)
	{
	    // +=, /=, etc. require an existing variable
	    semsg(_(e_cannot_use_operator_on_new_variable_str), eap->arg);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vim_strchr((char_u *)"+-*/%.", *ಅಭಿವ್ಯಕ್ತಿ) != ಖಾಲಿ)
	{
	    op[0] = *ಅಭಿವ್ಯಕ್ತಿ;   // +=, -=, *=, /=, %= or .=
	    ++len;
	    ಆದರೆ (ಅಭಿವ್ಯಕ್ತಿ[0] == '.' && ಅಭಿವ್ಯಕ್ತಿ[1] == '.') // ..=
	    {
		++ಅಭಿವ್ಯಕ್ತಿ;
		++len;
	    }
	}
	ಅಭಿವ್ಯಕ್ತಿ += 2;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	++ಅಭಿವ್ಯಕ್ತಿ;

    ಆದರೆ (vim9script && !eap->skip && (!VIM_ISWHITE(*argend)
					       || !IS_WHITE_OR_NUL(*ಅಭಿವ್ಯಕ್ತಿ)))
    {
	vim_strncpy(op, ಅಭಿವ್ಯಕ್ತಿ - len, len);
	semsg(_(e_white_space_required_before_and_after_str_at_str),
							       op, argend);
    }

    ಆದರೆ (eap->skip)
	++emsg_skip;
    fill_evalarg_from_eap(&evalarg, eap, eap->skip);
    ಅಭಿವ್ಯಕ್ತಿ = skipwhite_and_linebreak(ಅಭಿವ್ಯಕ್ತಿ, &evalarg);
    cur_lnum = SOURCING_LNUM;
    ಪೂರ್ಣಾಂಕ eval_res = eval0(ಅಭಿವ್ಯಕ್ತಿ, &rettv, eap, &evalarg);
    ಆದರೆ (eap->skip)
	--emsg_skip;
    clear_evalarg(&evalarg, eap);

    // Restore the line number so that any type error is given for the
    // declaration, not the expression.
    SOURCING_LNUM = cur_lnum;

    ಆದರೆ (!eap->skip && eval_res != FAIL)
	(ಶೂನ್ಯ)ex_let_vars(eap->arg, &rettv, FALSE, semicolon, var_ಎಣಿಕೆ,
								    flags, op);
    ಆದರೆ (eval_res != FAIL)
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
    ಪೂರ್ಣಾಂಕ
ex_let_vars(
    char_u	*arg_ಆರಂಭ,
    typval_T	*tv,
    ಪೂರ್ಣಾಂಕ		copy,		// copy values from "tv", don't move
    ಪೂರ್ಣಾಂಕ		semicolon,	// from skip_var_list()
    ಪೂರ್ಣಾಂಕ		var_ಎಣಿಕೆ,	// from skip_var_list()
    ಪೂರ್ಣಾಂಕ		flags,		// ASSIGN_FINAL, ASSIGN_CONST, etc.
    char_u	*op)
{
    char_u	*arg = arg_ಆರಂಭ;
    list_T	*l;
    tuple_T	*tuple = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		var_idx = 0;
    listitem_T	*ಅಂಶ = ಖಾಲಿ;
    typval_T	ltv;
    ಪೂರ್ಣಾಂಕ		is_list = tv->v_type == VAR_LIST;
    ಪೂರ್ಣಾಂಕ		idx;

    ಆದರೆ (tv->v_type == VAR_VOID)
    {
	emsg(_(e_cannot_use_void_ಮೌಲ್ಯ));
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಆದರೆ (*arg != '[')
    {
	// ":let var = expr" or ":for var in list"
	ಆದರೆ (ex_let_one(arg, tv, copy, flags, op, op, var_idx) == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;
	ಹಿಂತಿರುಗಿ OK;
    }

    // ":let [v1, v2] = list" or ":for [v1, v2] in listlist"
    // or
    // ":let [v1, v2] = tuple" or ":for [v1, v2] in tupletuple"
    ಆದರೆ (tv->v_type != VAR_LIST && tv->v_type != VAR_TUPLE)
    {
	emsg(_(e_list_or_tuple_required));
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಆದರೆ (is_list)
    {
	l = tv->vval.v_list;
	ಆದರೆ (l == ಖಾಲಿ)
	{
	    emsg(_(e_list_required));
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	i = list_len(l);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	tuple = tv->vval.v_tuple;
	ಆದರೆ (tuple == ಖಾಲಿ)
	{
	    emsg(_(e_tuple_required));
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	i = tuple_len(tuple);
    }

    ಆದರೆ (semicolon == 0 && var_ಎಣಿಕೆ < i)
    {
	emsg(_(is_list ? e_less_targets_than_list_ವಸ್ತುಗಳು
					: e_less_targets_than_tuple_ವಸ್ತುಗಳು));
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಆದರೆ (var_ಎಣಿಕೆ - semicolon > i)
    {
	emsg(_(is_list ? e_ಇನ್ನಷ್ಟು_targets_than_list_ವಸ್ತುಗಳು
					: e_ಇನ್ನಷ್ಟು_targets_than_tuple_ವಸ್ತುಗಳು));
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಆದರೆ (is_list)
    {
	CHECK_LIST_MATERIALIZE(l);
	ಅಂಶ = l->lv_ಮೊದಲ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	idx = 0;

    ಆಗ (*arg != ']')
    {
	arg = skipwhite(arg + 1);
	++var_idx;
	arg = ex_let_one(arg, is_list ? &ಅಂಶ->li_tv : TUPLE_ಅಂಶ(tuple, idx),
			 TRUE, flags | ನಿಯೋಜಿಸು_UNPACK, (char_u *)",;]", op,
			 var_idx);
	ಆದರೆ (is_list)
	    ಅಂಶ = ಅಂಶ->li_ಮುಂದಿನ;
	ಇಲ್ಲದಿದ್ದರೆ
	    idx++;
	ಆದರೆ (arg == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;

	arg = skipwhite(arg);
	ಆದರೆ (*arg == ';')
	{
	    // Put the rest of the list or tuple (may be empty) in the var
	    // after ';'.  Create a new list or tuple for this.
	    ಆದರೆ (is_list)
	    {
		// Put the rest of the list (may be empty) in the var
		// after ';'.  Create a new list for this.
		l = list_alloc();
		ಆದರೆ (l == ಖಾಲಿ)
		    ಹಿಂತಿರುಗಿ FAIL;

		// list
		ಆಗ (ಅಂಶ != ಖಾಲಿ)
		{
		    list_append_tv(l, &ಅಂಶ->li_tv);
		    ಅಂಶ = ಅಂಶ->li_ಮುಂದಿನ;
		}

		ltv.v_type = VAR_LIST;
		ltv.v_ಬೀಗ = 0;
		ltv.vval.v_list = l;
		l->lv_refcount = 1;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		tuple_T	*new_tuple = tuple_alloc();
		ಆದರೆ (new_tuple == ಖಾಲಿ)
		    ಹಿಂತಿರುಗಿ FAIL;

		// Put the rest of the tuple (may be empty) in the var
		// after ';'.  Create a new tuple for this.
		ಆಗ (idx < TUPLE_LEN(tuple))
		{
		    typval_T    new_tv;

		    copy_tv(TUPLE_ಅಂಶ(tuple, idx), &new_tv);
		    ಆದರೆ (tuple_append_tv(new_tuple, &new_tv) == FAIL)
		    {
			clear_tv(&new_tv);
			tuple_unref(new_tuple);
			ಹಿಂತಿರುಗಿ FAIL;
		    }
		    idx++;
		}

		ltv.v_type = VAR_TUPLE;
		ltv.v_ಬೀಗ = 0;
		ltv.vval.v_tuple = new_tuple;
		new_tuple->tv_refcount = 1;
	    }

	    ++var_idx;
	    arg = ex_let_one(skipwhite(arg + 1), &ltv, FALSE,
			flags | ನಿಯೋಜಿಸು_UNPACK, (char_u *)"]", op, var_idx);
	    clear_tv(&ltv);
	    ಆದರೆ (arg == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ FAIL;
	    ನಿಲ್ಲಿಸು;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*arg != ',' && *arg != ']')
	{
	    ಆಂತರಿಕ_error("ex_let_vars()");
	    ಹಿಂತಿರುಗಿ FAIL;
	}
    }

    ಹಿಂತಿರುಗಿ OK;
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
    ಪೂರ್ಣಾಂಕ		include_type,
    ಪೂರ್ಣಾಂಕ		*var_ಎಣಿಕೆ,
    ಪೂರ್ಣಾಂಕ		*semicolon,
    ಪೂರ್ಣಾಂಕ		silent)
{
    char_u	*p, *s;

    ಆದರೆ (*arg == '[')
    {
	// "[var, var]": find the matching ']'.
	p = arg;
	ಗಾಗಿ (;;)
	{
	    p = skipwhite(p + 1);	// skip whites after '[', ';' or ','
	    s = skip_var_one(p, include_type);
	    ಆದರೆ (s == p)
	    {
		ಆದರೆ (!silent)
		    semsg(_(e_invalid_ವಾದ_str), p);
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    }
	    ++*var_ಎಣಿಕೆ;

	    p = skipwhite(s);
	    ಆದರೆ (*p == ']')
		ನಿಲ್ಲಿಸು;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == ';')
	    {
		ಆದರೆ (*semicolon == 1)
		{
		    ಆದರೆ (!silent)
			emsg(_(e_double_semicolon_ಒಳ_list_ದ_variables));
		    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
		}
		*semicolon = 1;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p != ',')
	    {
		ಆದರೆ (!silent)
		    semsg(_(e_invalid_ವಾದ_str), p);
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    }
	}
	ಹಿಂತಿರುಗಿ p + 1;
    }

    ಹಿಂತಿರುಗಿ skip_var_one(arg, include_type);
}

/*
 * Skip one (assignable) variable name, including @r, $VAR, &option, d.key,
 * l[idx].
 * In Vim9 script also skip over ": type" if "include_type" is TRUE.
 */
    char_u *
skip_var_one(char_u *arg, ಪೂರ್ಣಾಂಕ include_type)
{
    char_u	*ಕೊನೆ;
    ಪೂರ್ಣಾಂಕ		vim9 = ಒಳ_vim9script();

    ಆದರೆ (*arg == '@' && arg[1] != NUL)
	ಹಿಂತಿರುಗಿ arg + 2;

    // termcap option name may have non-alpha characters
    ಆದರೆ (STRNCMP(arg, "&t_", 3) == 0 && arg[3] != NUL && arg[4] != NUL)
	ಹಿಂತಿರುಗಿ arg + 5;

    ಕೊನೆ = ಕಂಡುಹಿಡಿ_name_ಕೊನೆ(*arg == '$' || *arg == '&' ? arg + 1 : arg,
				   ಖಾಲಿ, ಖಾಲಿ, FNE_INCL_BR | FNE_CHECK_ಆರಂಭ);

    // "a: type" is declaring variable "a" with a type, not "a:".
    // Same for "s: type".
    ಆದರೆ (vim9 && ಕೊನೆ == arg + 2 && ಕೊನೆ[-1] == ':')
	--ಕೊನೆ;

    ಆದರೆ (include_type && vim9)
    {
	ಆದರೆ (*skipwhite(ಕೊನೆ) == ':')
	    ಕೊನೆ = skip_type(skipwhite(skipwhite(ಕೊನೆ) + 1), FALSE);
    }
    ಹಿಂತಿರುಗಿ ಕೊನೆ;
}

/*
 * List variables for hashtab "ht" with prefix "prefix".
 * If "empty" is TRUE also list NULL strings as empty strings.
 */
    ಶೂನ್ಯ
list_hashtable_vars(
    hashtab_T	*ht,
    ಅಕ್ಷರ	*ಪೂರ್ವಪ್ರತ್ಯಯ,
    ಪೂರ್ಣಾಂಕ		ಖಾಲಿಗಳು_2,
    ಪೂರ್ಣಾಂಕ		*ಮೊದಲ)
{
    hashitem_T	*hi;
    dictitem_T	*di;
    ಪೂರ್ಣಾಂಕ		todo;
    char_u	buf[IOSIZE];

    ಪೂರ್ಣಾಂಕ save_ht_flags = ht->ht_flags;
    ht->ht_flags |= HTFLAGS_FROZEN;

    todo = (ಪೂರ್ಣಾಂಕ)ht->ht_ಬಳಸಿದ;
    ಗಾಗಿ (hi = ht->ht_array; todo > 0 && !got_int; ++hi)
    {
	ಆದರೆ (!HASHITEM_ಖಾಲಿಗಳು(hi))
	{
	    --todo;
	    di = HI2DI(hi);

	    // apply :filter /pat/ to variable name
	    vim_strncpy((char_u *)buf, (char_u *)ಪೂರ್ವಪ್ರತ್ಯಯ, IOSIZE - 1);
	    vim_strcat((char_u *)buf, di->di_ಕೀಲಿ, IOSIZE);
	    ಆದರೆ (ಸಂದೇಶ_filtered(buf))
		ಮುಂದುವರಿ;

	    ಆದರೆ (ಖಾಲಿಗಳು_2 || di->di_tv.v_type != VAR_STRING
					   || di->di_tv.vval.v_string != ಖಾಲಿ)
		list_one_var(di, ಪೂರ್ವಪ್ರತ್ಯಯ, ಮೊದಲ);
	}
    }

    ht->ht_flags = save_ht_flags;
}

/*
 * List global variables.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
list_glob_vars(ಪೂರ್ಣಾಂಕ *ಮೊದಲ)
{
    list_hashtable_vars(&globvarht, "", TRUE, ಮೊದಲ);
}

/*
 * List buffer variables.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
list_buf_vars(ಪೂರ್ಣಾಂಕ *ಮೊದಲ)
{
    list_hashtable_vars(&curbuf->b_vars->dv_hashtab, "b:", TRUE, ಮೊದಲ);
}

/*
 * List window variables.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
list_win_vars(ಪೂರ್ಣಾಂಕ *ಮೊದಲ)
{
    list_hashtable_vars(&curwin->w_vars->dv_hashtab, "w:", TRUE, ಮೊದಲ);
}

/*
 * List tab page variables.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
list_tab_vars(ಪೂರ್ಣಾಂಕ *ಮೊದಲ)
{
    list_hashtable_vars(&curtab->tp_vars->dv_hashtab, "t:", TRUE, ಮೊದಲ);
}

/*
 * List variables in "arg".
 */
    ಸ್ಥಿರ char_u *
list_arg_vars(exarg_T *eap, char_u *arg, ಪೂರ್ಣಾಂಕ *ಮೊದಲ)
{
    ಪೂರ್ಣಾಂಕ		error = FALSE;
    ಪೂರ್ಣಾಂಕ		len;
    char_u	*name;
    char_u	*name_ಆರಂಭ;
    char_u	*arg_subsc;
    char_u	*tofree;
    typval_T    tv;

    ಆಗ (!ತುದಿಗಳು_excmd2(eap->cmd, arg) && !got_int)
    {
	ಆದರೆ (error || eap->skip)
	{
	    arg = ಕಂಡುಹಿಡಿ_name_ಕೊನೆ(arg, ಖಾಲಿ, ಖಾಲಿ, FNE_INCL_BR | FNE_CHECK_ಆರಂಭ);
	    ಆದರೆ (!VIM_ISWHITE(*arg) && !ತುದಿಗಳು_excmd(*arg))
	    {
		emsg_severe = TRUE;
		ಆದರೆ (!did_emsg)
		    semsg(_(e_trailing_characters_str), arg);
		ನಿಲ್ಲಿಸು;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // get_name_len() takes care of expanding curly braces
	    name_ಆರಂಭ = name = arg;
	    len = get_name_len(&arg, &tofree, TRUE, TRUE);
	    ಆದರೆ (len <= 0)
	    {
		// This is mainly to keep test 49 working: when expanding
		// curly braces fails overrule the exception error message.
		ಆದರೆ (len < 0 && !aborting())
		{
		    emsg_severe = TRUE;
		    semsg(_(e_invalid_ವಾದ_str), arg);
		    ನಿಲ್ಲಿಸು;
		}
		error = TRUE;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		arg = skipwhite(arg);
		ಆದರೆ (tofree != ಖಾಲಿ)
		    name = tofree;
		ಆದರೆ (eval_variable(name, len, 0, &tv, ಖಾಲಿ,
						     EVAL_VAR_VERBOSE) == FAIL)
		    error = TRUE;
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    // handle d.key, l[idx], f(expr)
		    arg_subsc = arg;
		    ಆದರೆ (handle_subscript(&arg, name_ಆರಂಭ, &tv,
					      &EVALARG_EVALUATE, TRUE) == FAIL)
			error = TRUE;
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			ಆದರೆ (arg == arg_subsc && len == 2 && name[1] == ':')
			{
			    ಆಯ್ಕೆ (*name)
			    {
				ಸ್ಥಿತಿ 'g': list_glob_vars(ಮೊದಲ); ನಿಲ್ಲಿಸು;
				ಸ್ಥಿತಿ 'b': list_buf_vars(ಮೊದಲ); ನಿಲ್ಲಿಸು;
				ಸ್ಥಿತಿ 'w': list_win_vars(ಮೊದಲ); ನಿಲ್ಲಿಸು;
				ಸ್ಥಿತಿ 't': list_tab_vars(ಮೊದಲ); ನಿಲ್ಲಿಸು;
				ಸ್ಥಿತಿ 'v': list_vim_vars(ಮೊದಲ); ನಿಲ್ಲಿಸು;
				ಸ್ಥಿತಿ 's': list_script_vars(ಮೊದಲ); ನಿಲ್ಲಿಸು;
				ಸ್ಥಿತಿ 'l': list_func_vars(ಮೊದಲ); ನಿಲ್ಲಿಸು;
				ಪೂರ್ವನಿಯೋಜಿತ:
					  semsg(_(e_cant_list_variables_for_str), name);
			    }
			}
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    char_u	numbuf[NUMBUFLEN];
			    char_u	*tf;
			    ಪೂರ್ಣಾಂಕ		c;
			    char_u	*s;

			    s = echo_string(&tv, &tf, numbuf, 0);
			    c = *arg;
			    *arg = NUL;
			    list_one_var_a("",
				    arg == arg_subsc ? name : name_ಆರಂಭ,
				    tv.v_type,
				    s == ಖಾಲಿ ? (char_u *)"" : s,
				    ಮೊದಲ);
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

    ಹಿಂತಿರುಗಿ arg;
}

/*
 * Set an environment variable, part of ex_let_one().
 */
    ಸ್ಥಿರ char_u *
ex_let_env(
    char_u	*arg,
    typval_T	*tv,
    ಪೂರ್ಣಾಂಕ		flags,
    char_u	*endchars,
    char_u	*op)
{
    char_u	*arg_ಕೊನೆ = ಖಾಲಿ;
    char_u	*name;
    ಪೂರ್ಣಾಂಕ		len;

    ಆದರೆ ((flags & (ನಿಯೋಜಿಸು_CONST | ನಿಯೋಜಿಸು_FINAL))
					 && (flags & ನಿಯೋಜಿಸು_FOR_LOOP) == 0)
    {
	emsg(_(e_cannot_ಬೀಗ_environment_variable));
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    // Find the end of the name.
    ++arg;
    name = arg;
    len = get_env_len(&arg);
    ಆದರೆ (len == 0)
	semsg(_(e_invalid_ವಾದ_str), name - 1);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (op != ಖಾಲಿ && vim_strchr((char_u *)"+-*/%", *op) != ಖಾಲಿ)
	    semsg(_(e_wrong_variable_type_for_str_equal), op);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (endchars != ಖಾಲಿ
			      && vim_strchr(endchars, *skipwhite(arg)) == ಖಾಲಿ)
	    emsg(_(e_unexpected_characters_ಒಳ_let));
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!check_secure() && !check_restricted())
	{
	    char_u	*tofree = ಖಾಲಿ;
	    ಪೂರ್ಣಾಂಕ		c1 = name[len];
	    char_u	*p;

	    name[len] = NUL;
	    p = tv_get_string_chk(tv);
	    ಆದರೆ (p != ಖಾಲಿ && op != ಖಾಲಿ && *op == '.')
	    {
		ಪೂರ್ಣಾಂಕ	mustfree = FALSE;
		char_u  *s = vim_getenv(name, &mustfree);

		ಆದರೆ (s != ಖಾಲಿ)
		{
		    p = tofree = concat_str(s, p);
		    ಆದರೆ (mustfree)
			vim_free(s);
		}
	    }
	    ಆದರೆ (p != ಖಾಲಿ)
	    {
		vim_setenv_ext(name, p);
		arg_ಕೊನೆ = arg;
	    }
	    name[len] = c1;
	    vim_free(tofree);
	}
    }
    ಹಿಂತಿರುಗಿ arg_ಕೊನೆ;
}

/*
 * Set an option, part of ex_let_one().
 */
    ಸ್ಥಿರ char_u *
ex_let_option(
    char_u	*arg,
    typval_T	*tv,
    ಪೂರ್ಣಾಂಕ		flags,
    char_u	*endchars,
    char_u	*op)
{
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		scope;
    char_u	*arg_ಕೊನೆ = ಖಾಲಿ;

    ಆದರೆ ((flags & (ನಿಯೋಜಿಸು_CONST | ನಿಯೋಜಿಸು_FINAL))
					 && (flags & ನಿಯೋಜಿಸು_FOR_LOOP) == 0)
    {
	emsg(_(e_cannot_ಬೀಗ_option));
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    // Find the end of the name.
    p = ಕಂಡುಹಿಡಿ_option_ಕೊನೆ(&arg, &scope);
    ಆದರೆ (p == ಖಾಲಿ || (endchars != ಖಾಲಿ
			       && vim_strchr(endchars, *skipwhite(p)) == ಖಾಲಿ))
    {
	emsg(_(e_unexpected_characters_ಒಳ_let));
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಪೂರ್ಣಾಂಕ		c1;
    ದೀರ್ಘ	n = 0;
    getoption_T	opt_type;
    ದೀರ್ಘ	numval;
    char_u	*stringval = ಖಾಲಿ;
    char_u	*s = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		failed = FALSE;
    ಪೂರ್ಣಾಂಕ		opt_p_flags;
    char_u	*tofree = ಖಾಲಿ;
    char_u	numbuf[NUMBUFLEN];

    c1 = *p;
    *p = NUL;

    opt_type = get_option_ಮೌಲ್ಯ(arg, &numval, &stringval, &opt_p_flags, scope);
    ಆದರೆ (opt_type == gov_unknown && arg[0] != 't' && arg[1] != '_')
    {
	semsg(_(e_unknown_option_str_2), arg);
	ಹೋಗು theend;
    }
    ಆದರೆ (op != ಖಾಲಿ && *op != '='
	&& (((opt_type == gov_bool || opt_type == gov_ಸಂಖ್ಯೆ) && *op == '.')
	    || (opt_type == gov_string && *op != '.')))
    {
	semsg(_(e_wrong_variable_type_for_str_equal), op);
	ಹೋಗು theend;
    }

    ಆದರೆ ((opt_type == gov_bool
		|| opt_type == gov_ಸಂಖ್ಯೆ
		|| opt_type == gov_hidden_bool
		|| opt_type == gov_hidden_ಸಂಖ್ಯೆ)
		     && (tv->v_type != VAR_STRING || !ಒಳ_vim9script()))
    {
	ಆದರೆ (opt_type == gov_bool || opt_type == gov_hidden_bool)
	    // bool, possibly hidden
	    n = (ದೀರ್ಘ)tv_get_bool_chk(tv, &failed);
	ಇಲ್ಲದಿದ್ದರೆ
	    // number, possibly hidden
	    n = (ದೀರ್ಘ)tv_get_ಸಂಖ್ಯೆ_chk(tv, &failed);
	ಆದರೆ (failed)
	    ಹೋಗು theend;
    }

    ಆದರೆ ((opt_p_flags & P_FUNC) && (tv->v_type == VAR_PARTIAL
						    || tv->v_type == VAR_FUNC))
    {
	// If the option can be set to a function reference or a lambda
	// and the passed value is a function reference, then convert it to
	// the name (string) of the function reference.
	s = tv2string(tv, &tofree, numbuf, 0);
	ಆದರೆ (s == ಖಾಲಿ)
	    ಹೋಗು theend;
    }
    // Avoid setting a string option to the text "v:false" or similar.
    // In Vim9 script also don't convert a number to string.
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tv->v_type != VAR_BOOL && tv->v_type != VAR_SPECIAL
		     && (!ಒಳ_vim9script() || tv->v_type != VAR_ಸಂಖ್ಯೆ))
    {
	s = tv_get_string_chk(tv);
	ಆದರೆ (s == ಖಾಲಿ)
	    ಹೋಗು theend;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (opt_type == gov_string || opt_type == gov_hidden_string)
    {
	emsg(_(e_string_required));
	ಹೋಗು theend;
    }

    ಆದರೆ (op != ಖಾಲಿ && *op != '=')
    {
	// number, in legacy script also bool
	ಆದರೆ (opt_type == gov_ಸಂಖ್ಯೆ
				 || (opt_type == gov_bool && !ಒಳ_vim9script()))
	{
	    ಆಯ್ಕೆ (*op)
	    {
		ಸ್ಥಿತಿ '+': n = numval + n; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ '-': n = numval - n; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ '*': n = numval * n; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ '/': n = (ದೀರ್ಘ)ಅಂಕೆ_divide(numval, n, &failed); ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ '%': n = (ದೀರ್ಘ)ಅಂಕೆ_modulus(numval, n, &failed); ನಿಲ್ಲಿಸು;
	    }
	    s = ಖಾಲಿ;
	    ಆದರೆ (failed)
		ಹೋಗು theend;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (opt_type == gov_string && stringval != ಖಾಲಿ && s != ಖಾಲಿ)
	{
	    // string
	    s = concat_str(stringval, s);
	    vim_free(stringval);
	    stringval = s;
	}
    }

    ಅಕ್ಷರ *err = ಗಣ_option_ಮೌಲ್ಯ(arg, n, s, scope);
    arg_ಕೊನೆ = p;
    ಆದರೆ (err != ಖಾಲಿ)
	emsg(_(err));

theend:
    *p = c1;
    vim_free(stringval);
    vim_free(tofree);
    ಹಿಂತಿರುಗಿ arg_ಕೊನೆ;
}

/*
 * Set a register, part of ex_let_one().
 */
    ಸ್ಥಿರ char_u *
ex_let_ರಿಜಿಸ್ಟರ್(
    char_u	*arg,
    typval_T	*tv,
    ಪೂರ್ಣಾಂಕ		flags,
    char_u	*endchars,
    char_u	*op)
{
    char_u	*arg_ಕೊನೆ = ಖಾಲಿ;

    ಆದರೆ ((flags & (ನಿಯೋಜಿಸು_CONST | ನಿಯೋಜಿಸು_FINAL))
					 && (flags & ನಿಯೋಜಿಸು_FOR_LOOP) == 0)
    {
	emsg(_(e_cannot_ಬೀಗ_ರಿಜಿಸ್ಟರ್));
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ++arg;
    ಆದರೆ (op != ಖಾಲಿ && vim_strchr((char_u *)"+-*/%", *op) != ಖಾಲಿ)
	semsg(_(e_wrong_variable_type_for_str_equal), op);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (endchars != ಖಾಲಿ
			  && vim_strchr(endchars, *skipwhite(arg + 1)) == ಖಾಲಿ)
	emsg(_(e_unexpected_characters_ಒಳ_let));
    ಇಲ್ಲದಿದ್ದರೆ
    {
	char_u	*ptofree = ಖಾಲಿ;
	char_u	*p;

	p = tv_get_string_chk(tv);
	ಆದರೆ (p != ಖಾಲಿ && op != ಖಾಲಿ && *op == '.')
	{
	    char_u  *s = get_reg_contents(*arg == '@'
						  ? '"' : *arg, GREG_ಅಭಿವ್ಯಕ್ತಿ_SRC);

	    ಆದರೆ (s != ಖಾಲಿ)
	    {
		p = ptofree = concat_str(s, p);
		vim_free(s);
	    }
	}
	ಆದರೆ (p != ಖಾಲಿ)
	{
	    write_reg_contents(*arg == '@' ? '"' : *arg, p, -1, FALSE);
	    arg_ಕೊನೆ = arg + 1;
	}
	vim_free(ptofree);
    }
    ಹಿಂತಿರುಗಿ arg_ಕೊನೆ;
}

/*
 * Set one item of ":let var = expr" or ":let [v1, v2] = list" to its value.
 * Returns a pointer to the char just after the var name.
 * Returns NULL if there is an error.
 */
    ಸ್ಥಿರ char_u *
ex_let_one(
    char_u	*arg,		// points to variable name
    typval_T	*tv,		// value to assign to variable
    ಪೂರ್ಣಾಂಕ		copy,		// copy value from "tv"
    ಪೂರ್ಣಾಂಕ		flags,		// ASSIGN_CONST, ASSIGN_FINAL, etc.
    char_u	*endchars,	// valid chars after variable name  or NULL
    char_u	*op,		// "+", "-", "."  or NULL
    ಪೂರ್ಣಾಂಕ		var_idx)	// variable index for "let [a, b] = list"
{
    char_u	*arg_ಕೊನೆ = ಖಾಲಿ;

    ಆದರೆ (ಒಳ_vim9script() && (flags & (ನಿಯೋಜಿಸು_NO_DECL | ನಿಯೋಜಿಸು_DECL)) == 0
			&& (flags & (ನಿಯೋಜಿಸು_CONST | ನಿಯೋಜಿಸು_FINAL)) == 0
				  && vim_strchr((char_u *)"$@&", *arg) != ಖಾಲಿ)
    {
	vim9_declare_error(arg);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಆದರೆ (check_typval_is_ಮೌಲ್ಯ(tv) == FAIL)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // In Vim9 script an environment variable and a register only take a
    // String.  "@#" also takes a buffer number.
    ಆದರೆ (ಒಳ_vim9script() && (*arg == '$' || *arg == '@')
	    && !(*arg == '@' && arg[1] == '#' && tv->v_type == VAR_ಸಂಖ್ಯೆ
					       && (op == ಖಾಲಿ || *op == '=')))
    {
	where_T	where = WHERE_INIT;

	ಆದರೆ (check_typval_type(&t_string, tv, where) == FAIL)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಆದರೆ (*arg == '$')
    {
	// ":let $VAR = expr": Set environment variable.
	ಹಿಂತಿರುಗಿ ex_let_env(arg, tv, flags, endchars, op);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*arg == '&')
    {
	// ":let &option = expr": Set option value.
	// ":let &l:option = expr": Set local option value.
	// ":let &g:option = expr": Set global option value.
	// ":for &ts in range(8)": Set option value for for loop
	ಹಿಂತಿರುಗಿ ex_let_option(arg, tv, flags, endchars, op);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*arg == '@')
    {
	// ":let @r = expr": Set register contents.
	ಹಿಂತಿರುಗಿ ex_let_ರಿಜಿಸ್ಟರ್(arg, tv, flags, endchars, op);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (eval_isnamec1(*arg) || *arg == '{')
    {
	lval_T	lv;
	char_u	*p;
	ಪೂರ್ಣಾಂಕ	lval_flags = (flags & (ನಿಯೋಜಿಸು_NO_DECL | ನಿಯೋಜಿಸು_DECL))
							     ? GLV_NO_DECL : 0;
	lval_flags |= (flags & ನಿಯೋಜಿಸು_FOR_LOOP) ? GLV_FOR_LOOP : 0;
	ಆದರೆ (op != ಖಾಲಿ && *op != '=')
	    lval_flags |= GLV_ನಿಯೋಜಿಸು_WITH_OP;

	// ":let var = expr": Set internal variable.
	// ":let var: type = expr": Set internal variable with type.
	// ":let {expr} = expr": Idem, name made with curly braces
	p = get_lval(arg, tv, &lv, FALSE, FALSE, lval_flags, FNE_CHECK_ಆರಂಭ);
	ಆದರೆ (p != ಖಾಲಿ && lv.ll_name != ಖಾಲಿ)
	{
	    ಆದರೆ (endchars != ಖಾಲಿ && vim_strchr(endchars,
					   *skipwhite(lv.ll_name_ಕೊನೆ)) == ಖಾಲಿ)
	    {
		emsg(_(e_unexpected_characters_ಒಳ_let));
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಗಣ_var_lval(&lv, p, tv, copy, flags, op, var_idx);
		arg_ಕೊನೆ = lv.ll_name_ಕೊನೆ;
	    }
	}
	clear_lval(&lv);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	semsg(_(e_invalid_ವಾದ_str), arg);

    ಹಿಂತಿರುಗಿ arg_ಕೊನೆ;
}

/*
 * ":unlet[!] var1 ... " command.
 */
    ಶೂನ್ಯ
ex_unlet(exarg_T *eap)
{
    ex_unletlock(eap, eap->arg, 0, eap->forceit ? GLV_QUIET : 0, do_unlet_var, ಖಾಲಿ);
}

/*
 * ":lockvar" and ":unlockvar" commands
 */
    ಶೂನ್ಯ
ex_lockvar(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    ಪೂರ್ಣಾಂಕ		deep = 2;

    ಆದರೆ (eap->forceit)
	deep = -1;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vim_isdigit(*arg))
    {
	deep = getdigits(&arg);
	arg = skipwhite(arg);
    }

    ex_unletlock(eap, arg, deep, 0, do_ಬೀಗ_var, ಖಾಲಿ);
}

/*
 * ":unlet", ":lockvar" and ":unlockvar" are quite similar.
 * Also used for Vim9 script.  "callback" is invoked as:
 *	callback(&lv, name_end, eap, deep, cookie)
 */
    ಶೂನ್ಯ
ex_unletlock(
    exarg_T	*eap,
    char_u	*argstart,
    ಪೂರ್ಣಾಂಕ		deep,
    ಪೂರ್ಣಾಂಕ		glv_flags,
    ಪೂರ್ಣಾಂಕ		(*callback)(lval_T *, char_u *, exarg_T *, ಪೂರ್ಣಾಂಕ, ಶೂನ್ಯ *),
    ಶೂನ್ಯ	*cookie)
{
    char_u	*arg = argstart;
    char_u	*name_ಕೊನೆ;
    ಪೂರ್ಣಾಂಕ		error = FALSE;
    lval_T	lv;

    ಮಾಡು
    {
	ಆದರೆ (*arg == '$')
	{
	    lv.ll_name = arg;
	    lv.ll_tv = ಖಾಲಿ;
	    ++arg;
	    ಆದರೆ (get_env_len(&arg) == 0)
	    {
		semsg(_(e_invalid_ವಾದ_str), arg - 1);
		ಹಿಂತಿರುಗಿ;
	    }
	    ಆದರೆ (!error && !eap->skip
			      && callback(&lv, arg, eap, deep, cookie) == FAIL)
		error = TRUE;
	    name_ಕೊನೆ = arg;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // Parse the name and find the end.
	    name_ಕೊನೆ = get_lval(arg, ಖಾಲಿ, &lv, TRUE, eap->skip || error,
				     glv_flags | GLV_NO_DECL, FNE_CHECK_ಆರಂಭ);
	    ಆದರೆ (lv.ll_name == ಖಾಲಿ)
		error = TRUE;	    // error but continue parsing
	    ಆದರೆ (name_ಕೊನೆ == ಖಾಲಿ || (!VIM_ISWHITE(*name_ಕೊನೆ)
						    && !ತುದಿಗಳು_excmd(*name_ಕೊನೆ)))
	    {
		ಆದರೆ (name_ಕೊನೆ != ಖಾಲಿ)
		{
		    emsg_severe = TRUE;
		    semsg(_(e_trailing_characters_str), name_ಕೊನೆ);
		}
		ಆದರೆ (!(eap->skip || error))
		    clear_lval(&lv);
		ನಿಲ್ಲಿಸು;
	    }

	    ಆದರೆ (!error && !eap->skip
			 && callback(&lv, name_ಕೊನೆ, eap, deep, cookie) == FAIL)
		error = TRUE;

	    ಆದರೆ (!eap->skip)
		clear_lval(&lv);
	}

	arg = skipwhite(name_ಕೊನೆ);
    } ಆಗ (!ತುದಿಗಳು_excmd2(name_ಕೊನೆ, arg));

    ಗಣ_nextcmd(eap, arg);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
do_unlet_var(
    lval_T	*lp,
    char_u	*name_ಕೊನೆ,
    exarg_T	*eap,
    ಪೂರ್ಣಾಂಕ		deep ಬಳಸದ,
    ಶೂನ್ಯ	*cookie ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ		forceit = eap->forceit;
    ಪೂರ್ಣಾಂಕ		ret = OK;
    ಪೂರ್ಣಾಂಕ		cc;

    ಆದರೆ (lp->ll_tv == ಖಾಲಿ)
    {
	cc = *name_ಕೊನೆ;
	*name_ಕೊನೆ = NUL;

	// Environment variable, normal name or expanded name.
	ಆದರೆ (*lp->ll_name == '$')
	    vim_unsetenv_ext(lp->ll_name + 1);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (do_unlet(lp->ll_name, forceit) == FAIL)
	    ret = FAIL;
	*name_ಕೊನೆ = cc;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((lp->ll_list != ಖಾಲಿ
		 && ಮೌಲ್ಯ_check_ಬೀಗ(lp->ll_list->lv_ಬೀಗ, lp->ll_name, FALSE))
	    || (lp->ll_dict != ಖಾಲಿ
		&& ಮೌಲ್ಯ_check_ಬೀಗ(lp->ll_dict->dv_ಬೀಗ, lp->ll_name, FALSE)))
	ಹಿಂತಿರುಗಿ FAIL;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lp->ll_range)
	list_unlet_range(lp->ll_list, lp->ll_li, lp->ll_n1,
						    !lp->ll_empty2, lp->ll_n2);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lp->ll_list != ಖಾಲಿ)
	// unlet a List item.
	listitem_remove(lp->ll_list, lp->ll_li);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lp->ll_dict != ಖಾಲಿ)
	// unlet a Dictionary item.
	dictitem_remove(lp->ll_dict, lp->ll_di, "unlet");
    ಇಲ್ಲದಿದ್ದರೆ
    {
	semsg(_(e_cannot_unlet_imported_ಅಂಶ_str), lp->ll_name);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Unlet one item or a range of items from a list.
 * Return OK or FAIL.
 */
    ಶೂನ್ಯ
list_unlet_range(
	list_T	    *l,
	listitem_T  *li_ಮೊದಲ,
	ದೀರ್ಘ	    n1_arg,
	ಪೂರ್ಣಾಂಕ	    has_n2,
	ದೀರ್ಘ	    n2)
{
    listitem_T  *li = li_ಮೊದಲ;
    ಪೂರ್ಣಾಂಕ		n1 = n1_arg;

    // Delete a range of List items.
    li = li_ಮೊದಲ;
    n1 = n1_arg;
    ಆಗ (li != ಖಾಲಿ && (!has_n2 || n2 >= n1))
    {
	listitem_T *ಮುಂದಿನ = li->li_ಮುಂದಿನ;

	listitem_remove(l, li);
	li = ಮುಂದಿನ;
	++n1;
    }
}
/*
 * "unlet" a variable.  Return OK if it existed, FAIL if not.
 * When "forceit" is TRUE don't complain if the variable doesn't exist.
 */
    ಪೂರ್ಣಾಂಕ
do_unlet(char_u *name, ಪೂರ್ಣಾಂಕ forceit)
{
    hashtab_T	*ht;
    hashitem_T	*hi;
    char_u	*varname = ಖಾಲಿ;  // init to shut up gcc
    dict_T	*d;
    dictitem_T	*di;

    // can't :unlet a script variable in Vim9 script
    ಆದರೆ (ಒಳ_vim9script() && check_vim9_unlet(name) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    ht = ಕಂಡುಹಿಡಿ_var_ht(name, &varname);

    // can't :unlet a script variable in Vim9 script from a function
    ಆದರೆ (ht == get_script_local_ht()
	    && SCRIPT_ID_VALID(current_sctx.sc_sid)
	    && SCRIPT_ಅಂಶ(current_sctx.sc_sid)->sn_ಆವೃತ್ತಿ
							 == SCRIPT_ಆವೃತ್ತಿ_VIM9
	    && check_vim9_unlet(name) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (ht != ಖಾಲಿ && *varname != NUL)
    {
	d = get_current_funccal_dict(ht);
	ಆದರೆ (d == ಖಾಲಿ)
	{
	    ಆದರೆ (ht == &globvarht)
		d = &globvardict;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ht == &compat_hashtab)
		d = &vimvardict;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		di = ಕಂಡುಹಿಡಿ_var_ಒಳ_ht(ht, *name, (char_u *)"", FALSE);
		d = di == ಖಾಲಿ ? ಖಾಲಿ : di->di_tv.vval.v_dict;
	    }
	    ಆದರೆ (d == ಖಾಲಿ)
	    {
		ಆಂತರಿಕ_error("do_unlet()");
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	}
	hi = hash_ಕಂಡುಹಿಡಿ(ht, varname);
	ಆದರೆ (HASHITEM_ಖಾಲಿಗಳು(hi))
	    hi = ಕಂಡುಹಿಡಿ_hi_ಒಳ_scoped_ht(name, &ht);
	ಆದರೆ (hi != ಖಾಲಿ && !HASHITEM_ಖಾಲಿಗಳು(hi))
	{
	    di = HI2DI(hi);
	    ಆದರೆ (var_check_fixed(di->di_flags, name, FALSE)
		    || var_check_ro(di->di_flags, name, FALSE)
		    || ಮೌಲ್ಯ_check_ಬೀಗ(d->dv_ಬೀಗ, name, FALSE)
		    || check_hashtab_frozen(ht, "unlet"))
		ಹಿಂತಿರುಗಿ FAIL;

	    delete_var(ht, hi);
	    ಹಿಂತಿರುಗಿ OK;
	}
    }
    ಆದರೆ (forceit)
	ಹಿಂತಿರುಗಿ OK;
    semsg(_(e_no_such_variable_str), name);
    ಹಿಂತಿರುಗಿ FAIL;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
report_lockvar_member(ಅಕ್ಷರ *ಸಂದೇಶ, lval_T *lp)
{
    ಪೂರ್ಣಾಂಕ did_alloc = FALSE;
    char_u *vname = (char_u *)"";
    char_u *class_name = lp->ll_class != ಖಾಲಿ
				    ? lp->ll_class->class_name.string : (char_u *)"";
    ಆದರೆ (lp->ll_name != ಖಾಲಿ)
    {
	ಆದರೆ (lp->ll_name_ಕೊನೆ == ಖಾಲಿ)
	    vname = lp->ll_name;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    vname = vim_strnsave(lp->ll_name, lp->ll_name_ಕೊನೆ - lp->ll_name);
	    ಆದರೆ (vname == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ;
	    did_alloc = TRUE;
	}
    }
    semsg(_(ಸಂದೇಶ), vname, class_name);
    ಆದರೆ (did_alloc)
	vim_free(vname);
}

/*
 * Lock or unlock variable indicated by "lp".
 * "deep" is the levels to go (-1 for unlimited);
 * "lock" is TRUE for ":lockvar", FALSE for ":unlockvar".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
do_ಬೀಗ_var(
    lval_T	*lp,
    char_u	*name_ಕೊನೆ,
    exarg_T	*eap,
    ಪೂರ್ಣಾಂಕ		deep,
    ಶೂನ್ಯ	*cookie ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ		ಬೀಗ = eap->cmdidx == CMD_lockvar;
    ಪೂರ್ಣಾಂಕ		ret = OK;
    ಪೂರ್ಣಾಂಕ		cc;
    dictitem_T	*di;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ LOG_LOCKVAR
    ch_log(ಖಾಲಿ, "LKVAR: do_lock_var(): name %s, is_root %d", lp->ll_name, lp->ll_is_ಬೇರು);
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (lp->ll_tv == ಖಾಲಿ)
    {
	cc = *name_ಕೊನೆ;
	*name_ಕೊನೆ = NUL;
	ಆದರೆ (*lp->ll_name == '$')
	{
	    semsg(_(e_cannot_ಬೀಗ_or_unlock_variable_str), lp->ll_name);
	    ret = FAIL;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // Normal name or expanded name.
	    di = ಕಂಡುಹಿಡಿ_var(lp->ll_name, ಖಾಲಿ, TRUE);
	    ಆದರೆ (di == ಖಾಲಿ)
	    {
		ಆದರೆ (ಒಳ_vim9script())
		    semsg(_(e_cannot_ಕಂಡುಹಿಡಿ_variable_ಗಮ್ಯ_unlock_str),
								  lp->ll_name);
		ret = FAIL;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ ((di->di_flags & DI_FLAGS_FIX)
			    && di->di_tv.v_type != VAR_DICT
			    && di->di_tv.v_type != VAR_LIST)
		{
		    // For historic reasons this error is not given for a list
		    // or dict.  E.g., the b: dict could be locked/unlocked.
		    semsg(_(e_cannot_ಬೀಗ_or_unlock_variable_str), lp->ll_name);
		    ret = FAIL;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಆದರೆ (ಒಳ_vim9script())
		    {
			svar_T  *sv = ಕಂಡುಹಿಡಿ_typval_ಒಳ_script(&di->di_tv,
								     0, FALSE);

			ಆದರೆ (sv != ಖಾಲಿ && sv->sv_const != 0)
			{
			    semsg(_(e_cannot_change_readonly_variable_str),
								  lp->ll_name);
			    ret = FAIL;
			}
		    }

		    ಆದರೆ (ret == OK)
		    {
			ಆದರೆ (ಬೀಗ)
			    di->di_flags |= DI_FLAGS_ಬೀಗ;
			ಇಲ್ಲದಿದ್ದರೆ
			    di->di_flags &= ~DI_FLAGS_ಬೀಗ;
			ಆದರೆ (deep != 0)
			    ಅಂಶ_ಬೀಗ(&di->di_tv, deep, ಬೀಗ, FALSE);
		    }
		}
	    }
	}
	*name_ಕೊನೆ = cc;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (deep == 0 && lp->ll_object == ಖಾಲಿ && lp->ll_class == ಖಾಲಿ)
    {
	// nothing to do
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lp->ll_is_ಬೇರು)
	// (un)lock the item.
	ಅಂಶ_ಬೀಗ(lp->ll_tv, deep, ಬೀಗ, FALSE);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lp->ll_range)
    {
	listitem_T    *li = lp->ll_li;

	// (un)lock a range of List items.
	ಆಗ (li != ಖಾಲಿ && (lp->ll_empty2 || lp->ll_n2 >= lp->ll_n1))
	{
	    ಅಂಶ_ಬೀಗ(&li->li_tv, deep, ಬೀಗ, FALSE);
	    li = li->li_ಮುಂದಿನ;
	    ++lp->ll_n1;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lp->ll_list != ಖಾಲಿ)
	// (un)lock a List item.
	ಅಂಶ_ಬೀಗ(&lp->ll_li->li_tv, deep, ಬೀಗ, FALSE);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lp->ll_object != ಖಾಲಿ)  // This check must be before ll_class.
    {
	// (un)lock an object variable.
	report_lockvar_member(e_cannot_ಬೀಗ_object_variable_str, lp);
	ret = FAIL;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lp->ll_class != ಖಾಲಿ)
    {
	// (un)lock a class variable.
	report_lockvar_member(e_cannot_ಬೀಗ_class_variable_str, lp);
	ret = FAIL;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// (un)lock a Dictionary item.
	ಆದರೆ (lp->ll_di == ಖಾಲಿ)
	{
	    emsg(_(e_dictionary_required));
	    ret = FAIL;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ಅಂಶ_ಬೀಗ(&lp->ll_di->di_tv, deep, ಬೀಗ, FALSE);
    }

    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Lock or unlock an item.  "deep" is nr of levels to go.
 * When "check_refcount" is TRUE do not lock a list or dict with a reference
 * count larger than 1.
 */
    ಶೂನ್ಯ
ಅಂಶ_ಬೀಗ(typval_T *tv, ಪೂರ್ಣಾಂಕ deep, ಪೂರ್ಣಾಂಕ ಬೀಗ, ಪೂರ್ಣಾಂಕ check_refcount)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	recurse = 0;
    list_T	*l;
    listitem_T	*li;
    dict_T	*d;
    blob_T	*b;
    hashitem_T	*hi;
    ಪೂರ್ಣಾಂಕ		todo;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ LOG_LOCKVAR
    ch_log(ಖಾಲಿ, "LKVAR: item_lock(): type %s", vartype_name(tv->v_type));
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (recurse >= DICT_MAXNEST)
    {
	emsg(_(e_variable_nested_too_deep_for_unlock));
	ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (deep == 0)
	ಹಿಂತಿರುಗಿ;
    ++recurse;

    // lock/unlock the item itself
    ಆದರೆ (ಬೀಗ)
	tv->v_ಬೀಗ |= VAR_LOCKED;
    ಇಲ್ಲದಿದ್ದರೆ
	tv->v_ಬೀಗ &= ~VAR_LOCKED;

    ಆಯ್ಕೆ (tv->v_type)
    {
	ಸ್ಥಿತಿ VAR_UNKNOWN:
	ಸ್ಥಿತಿ VAR_ANY:
	ಸ್ಥಿತಿ VAR_VOID:
	ಸ್ಥಿತಿ VAR_ಸಂಖ್ಯೆ:
	ಸ್ಥಿತಿ VAR_BOOL:
	ಸ್ಥಿತಿ VAR_STRING:
	ಸ್ಥಿತಿ VAR_FUNC:
	ಸ್ಥಿತಿ VAR_PARTIAL:
	ಸ್ಥಿತಿ VAR_FLOAT:
	ಸ್ಥಿತಿ VAR_SPECIAL:
	ಸ್ಥಿತಿ VAR_ಕೆಲಸ:
	ಸ್ಥಿತಿ VAR_CHANNEL:
	ಸ್ಥಿತಿ VAR_INSTR:
	ಸ್ಥಿತಿ VAR_CLASS:
	ಸ್ಥಿತಿ VAR_OBJECT:
	ಸ್ಥಿತಿ VAR_TYPEALIAS:
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ VAR_BLOB:
	    ಆದರೆ ((b = tv->vval.v_blob) != ಖಾಲಿ
				    && !(check_refcount && b->bv_refcount > 1))
	    {
		ಆದರೆ (ಬೀಗ)
		    b->bv_ಬೀಗ |= VAR_LOCKED;
		ಇಲ್ಲದಿದ್ದರೆ
		    b->bv_ಬೀಗ &= ~VAR_LOCKED;
	    }
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_LIST:
	    ಆದರೆ ((l = tv->vval.v_list) != ಖಾಲಿ
				    && !(check_refcount && l->lv_refcount > 1))
	    {
		ಆದರೆ (ಬೀಗ)
		    l->lv_ಬೀಗ |= VAR_LOCKED;
		ಇಲ್ಲದಿದ್ದರೆ
		    l->lv_ಬೀಗ &= ~VAR_LOCKED;
		ಆದರೆ (deep < 0 || deep > 1)
		{
		    ಆದರೆ (l->lv_ಮೊದಲ == &range_list_ಅಂಶ)
			l->lv_ಬೀಗ |= VAR_ವಸ್ತುಗಳು_LOCKED;
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			// recursive: lock/unlock the items the List contains
			CHECK_LIST_MATERIALIZE(l);
			FOR_ALL_LIST_ವಸ್ತುಗಳು(l, li) ಅಂಶ_ಬೀಗ(&li->li_tv,
					       deep - 1, ಬೀಗ, check_refcount);
		    }
		}
	    }
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_TUPLE:
	    tuple_ಬೀಗ(tv->vval.v_tuple, deep, ಬೀಗ, check_refcount);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_DICT:
	    ಆದರೆ ((d = tv->vval.v_dict) != ಖಾಲಿ
				    && !(check_refcount && d->dv_refcount > 1))
	    {
		ಆದರೆ (ಬೀಗ)
		    d->dv_ಬೀಗ |= VAR_LOCKED;
		ಇಲ್ಲದಿದ್ದರೆ
		    d->dv_ಬೀಗ &= ~VAR_LOCKED;
		ಆದರೆ (deep < 0 || deep > 1)
		{
		    // recursive: lock/unlock the items the List contains
		    todo = (ಪೂರ್ಣಾಂಕ)d->dv_hashtab.ht_ಬಳಸಿದ;
		    FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(&d->dv_hashtab, hi, todo)
		    {
			ಆದರೆ (!HASHITEM_ಖಾಲಿಗಳು(hi))
			{
			    --todo;
			    ಅಂಶ_ಬೀಗ(&HI2DI(hi)->di_tv, deep - 1, ಬೀಗ,
							       check_refcount);
			}
		    }
		}
	    }
    }
    --recurse;
}

#ಆದರೆ defined(FEAT_MENU) && defined(FEAT_MULTI_LANG)
/*
 * Delete all "menutrans_" variables.
 */
    ಶೂನ್ಯ
del_menutrans_vars(ಶೂನ್ಯ)
{
    hashitem_T	*hi;
    ಪೂರ್ಣಾಂಕ		todo;

    hash_ಬೀಗ(&globvarht);
    todo = (ಪೂರ್ಣಾಂಕ)globvarht.ht_ಬಳಸಿದ;
    ಗಾಗಿ (hi = globvarht.ht_array; todo > 0 && !got_int; ++hi)
    {
	ಆದರೆ (!HASHITEM_ಖಾಲಿಗಳು(hi))
	{
	    --todo;
	    ಆದರೆ (STRNCMP(HI2DI(hi)->di_ಕೀಲಿ, "menutrans_", 10) == 0)
		delete_var(&globvarht, hi);
	}
    }
    hash_unlock(&globvarht);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Local string buffer for the next two functions to store a variable name
 * with its prefix. Allocated in cat_prefix_varname(), freed later in
 * get_user_var_name().
 */

ಸ್ಥಿರ char_u	*varnamebuf = ಖಾಲಿ;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	varnamebuflen = 0;

/*
 * Function to concatenate a prefix and a variable name.
 */
    char_u *
cat_ಪೂರ್ವಪ್ರತ್ಯಯ_varname(ಪೂರ್ಣಾಂಕ ಪೂರ್ವಪ್ರತ್ಯಯ, char_u *name)
{
    ಪೂರ್ಣಾಂಕ		len;

    len = (ಪೂರ್ಣಾಂಕ)STRLEN(name) + 3;
    ಆದರೆ (len > varnamebuflen)
    {
	vim_free(varnamebuf);
	len += 10;			// some additional space
	varnamebuf = alloc(len);
	ಆದರೆ (varnamebuf == ಖಾಲಿ)
	{
	    varnamebuflen = 0;
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
	varnamebuflen = len;
    }
    *varnamebuf = ಪೂರ್ವಪ್ರತ್ಯಯ;
    varnamebuf[1] = ':';
    STRCPY(varnamebuf + 2, name);
    ಹಿಂತಿರುಗಿ varnamebuf;
}

/*
 * Function given to ExpandGeneric() to obtain the list of user defined
 * (global/buffer/window/built-in) variable names.
 */
    char_u *
get_user_var_name(expand_T *xp, ಪೂರ್ಣಾಂಕ idx)
{
    ಸ್ಥಿರ long_u	gdone;
    ಸ್ಥಿರ long_u	bdone;
    ಸ್ಥಿರ long_u	wdone;
    ಸ್ಥಿರ long_u	tdone;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ		vidx;
    ಸ್ಥಿರ hashitem_T	*hi;
    hashtab_T		*ht;

    ಆದರೆ (idx == 0)
    {
	gdone = bdone = wdone = vidx = 0;
	tdone = 0;
    }

    // Global variables
    ಆದರೆ (gdone < globvarht.ht_ಬಳಸಿದ)
    {
	ಆದರೆ (gdone++ == 0)
	    hi = globvarht.ht_array;
	ಇಲ್ಲದಿದ್ದರೆ
	    ++hi;
	ಆಗ (HASHITEM_ಖಾಲಿಗಳು(hi))
	    ++hi;
	ಆದರೆ (STRNCMP("g:", xp->xp_ನಮೂನೆ, 2) == 0)
	    ಹಿಂತಿರುಗಿ cat_ಪೂರ್ವಪ್ರತ್ಯಯ_varname('g', hi->hi_ಕೀಲಿ);
	ಹಿಂತಿರುಗಿ hi->hi_ಕೀಲಿ;
    }

    // b: variables
    ht = &prevwin_curwin()->w_ಬಫರ್->b_vars->dv_hashtab;
    ಆದರೆ (bdone < ht->ht_ಬಳಸಿದ)
    {
	ಆದರೆ (bdone++ == 0)
	    hi = ht->ht_array;
	ಇಲ್ಲದಿದ್ದರೆ
	    ++hi;
	ಆಗ (HASHITEM_ಖಾಲಿಗಳು(hi))
	    ++hi;
	ಹಿಂತಿರುಗಿ cat_ಪೂರ್ವಪ್ರತ್ಯಯ_varname('b', hi->hi_ಕೀಲಿ);
    }

    // w: variables
    ht = &prevwin_curwin()->w_vars->dv_hashtab;
    ಆದರೆ (wdone < ht->ht_ಬಳಸಿದ)
    {
	ಆದರೆ (wdone++ == 0)
	    hi = ht->ht_array;
	ಇಲ್ಲದಿದ್ದರೆ
	    ++hi;
	ಆಗ (HASHITEM_ಖಾಲಿಗಳು(hi))
	    ++hi;
	ಹಿಂತಿರುಗಿ cat_ಪೂರ್ವಪ್ರತ್ಯಯ_varname('w', hi->hi_ಕೀಲಿ);
    }

    // t: variables
    ht = &curtab->tp_vars->dv_hashtab;
    ಆದರೆ (tdone < ht->ht_ಬಳಸಿದ)
    {
	ಆದರೆ (tdone++ == 0)
	    hi = ht->ht_array;
	ಇಲ್ಲದಿದ್ದರೆ
	    ++hi;
	ಆಗ (HASHITEM_ಖಾಲಿಗಳು(hi))
	    ++hi;
	ಹಿಂತಿರುಗಿ cat_ಪೂರ್ವಪ್ರತ್ಯಯ_varname('t', hi->hi_ಕೀಲಿ);
    }

    // v: variables
    ಆದರೆ (vidx < VV_LEN)
	ಹಿಂತಿರುಗಿ cat_ಪೂರ್ವಪ್ರತ್ಯಯ_varname('v', (char_u *)vimvars[vidx++].vv_name);

    VIM_CLEAR(varnamebuf);
    varnamebuflen = 0;
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಅಕ್ಷರ *
get_var_special_name(ಪೂರ್ಣಾಂಕ nr)
{
    ಆಯ್ಕೆ (nr)
    {
	ಸ್ಥಿತಿ VVAL_FALSE: ಹಿಂತಿರುಗಿ ಒಳ_vim9script() ? "false" : "v:false";
	ಸ್ಥಿತಿ VVAL_TRUE:  ಹಿಂತಿರುಗಿ ಒಳ_vim9script() ? "true" : "v:true";
	ಸ್ಥಿತಿ VVAL_NULL:  ಹಿಂತಿರುಗಿ ಒಳ_vim9script() ? "null" : "v:null";
	ಸ್ಥಿತಿ VVAL_NONE:  ಹಿಂತಿರುಗಿ "v:none";
    }
    ಆಂತರಿಕ_error("get_var_special_name()");
    ಹಿಂತಿರುಗಿ "42";
}

/*
 * Returns the global variable dictionary
 */
    dict_T *
get_globvar_dict(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ &globvardict;
}

/*
 * Returns the global variable hash table
 */
    hashtab_T *
get_globvar_ht(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ &globvarht;
}

/*
 * Returns the v: variable dictionary
 */
    dict_T *
get_vimvar_dict(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ &vimvardict;
}

/*
 * Returns the index of a v:variable.  Negative if not found.
 * Returns DI_ flags in "di_flags".
 */
    ಪೂರ್ಣಾಂಕ
ಕಂಡುಹಿಡಿ_vim_var(char_u *name, ಪೂರ್ಣಾಂಕ *di_flags)
{
    dictitem_T	    *di = ಕಂಡುಹಿಡಿ_var_ಒಳ_ht(&vimvarht, 0, name, TRUE);
    ರಚನೆ vimvar   *vv;

    ಆದರೆ (di == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ -1;
    *di_flags = di->di_flags;
    vv = (ರಚನೆ vimvar *)((ಅಕ್ಷರ *)di - offsetof(vimvar_T, vv_di));
    ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)(vv - vimvars);
}


/*
 * Set type of v: variable to "type".
 */
    ಶೂನ್ಯ
ಗಣ_vim_var_type(ಪೂರ್ಣಾಂಕ idx, vartype_T type)
{
    vimvars[idx].vv_tv_type = type;
}

/*
 * Set number v: variable to "val".
 * Note that this does not set the type, use set_vim_var_type() for that.
 */
    ಶೂನ್ಯ
ಗಣ_vim_var_nr(ಪೂರ್ಣಾಂಕ idx, varnumber_T val)
{
    vimvars[idx].vv_nr = val;
}

    ಅಕ್ಷರ *
get_vim_var_name(ಪೂರ್ಣಾಂಕ idx)
{
    ಹಿಂತಿರುಗಿ vimvars[idx].vv_name;
}

/*
 * Get typval_T v: variable value.
 */
    typval_T *
get_vim_var_tv(ಪೂರ್ಣಾಂಕ idx)
{
    ಹಿಂತಿರುಗಿ &vimvars[idx].vv_tv;
}

    type_T *
get_vim_var_type(ಪೂರ್ಣಾಂಕ idx, garray_T *type_list)
{
    ಆದರೆ (vimvars[idx].vv_type != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ vimvars[idx].vv_type;
    ಹಿಂತಿರುಗಿ typval2type_vimvar(&vimvars[idx].vv_tv, type_list);
}

/*
 * Add what getinfo() reports for the v: variable "name", without the "v:", to
 * "d".  Returns FAIL when there is no such variable.
 */
    ಪೂರ್ಣಾಂಕ
vim_var_info(char_u *name, dict_T *d)
{
    ಪೂರ್ಣಾಂಕ		di_flags;
    ಪೂರ್ಣಾಂಕ		idx = ಕಂಡುಹಿಡಿ_vim_var(name, &di_flags);
    garray_T	type_list;
    ಅಕ್ಷರ	*tofree;

    ಆದರೆ (idx < 0)
	ಹಿಂತಿರುಗಿ FAIL;
    vim_snprintf((ಅಕ್ಷರ *)IObuff, IOSIZE, "v:%s", vimvars[idx].vv_name);
    dict_ಕೂಡಿಸು_string(d, "name", IObuff);
    dict_ಕೂಡಿಸು_bool(d, "available", TRUE);
    ga_init2(&type_list, ಗಾತ್ರ(type_T *), 10);
    ಅಕ್ಷರ *type = type_name(get_vim_var_type(idx, &type_list), &tofree);
    dict_ಕೂಡಿಸು_string(d, "type", (char_u *)type);
    vim_free(tofree);
    clear_type_list(&type_list);
    dict_ಕೂಡಿಸು_bool(d, "readonly", (vimvars[idx].vv_flags & VV_RO) != 0);
    dict_ಕೂಡಿಸು_bool(d, "compat", (vimvars[idx].vv_flags & VV_COMPAT) != 0);
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Set v: variable to "tv".  Only accepts the same type.
 * Takes over the value of "tv".
 */
    ಪೂರ್ಣಾಂಕ
ಗಣ_vim_var_tv(ಪೂರ್ಣಾಂಕ idx, typval_T *tv)
{
    ಆದರೆ (vimvars[idx].vv_tv_type != tv->v_type)
    {
	emsg(_(e_type_mismatch_for_v_variable));
	clear_tv(tv);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    // VV_RO is also checked when compiling, but let's check here as well.
    ಆದರೆ (vimvars[idx].vv_flags & VV_RO)
    {
	semsg(_(e_cannot_change_readonly_variable_str), vimvars[idx].vv_name);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಆದರೆ (sandbox && (vimvars[idx].vv_flags & VV_RO_SBX))
    {
	semsg(_(e_cannot_ಗಣ_variable_ಒಳ_sandbox_str), vimvars[idx].vv_name);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_di.di_tv = *tv;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Get number v: variable value.
 */
    varnumber_T
get_vim_var_nr(ಪೂರ್ಣಾಂಕ idx)
{
    ಹಿಂತಿರುಗಿ vimvars[idx].vv_nr;
}

/*
 * Get string v: variable value.  Uses a static buffer, can only be used once.
 * If the String variable has never been set, return an empty string.
 * Never returns NULL;
 */
    char_u *
get_vim_var_str(ಪೂರ್ಣಾಂಕ idx)
{
    ಹಿಂತಿರುಗಿ tv_get_string(&vimvars[idx].vv_tv);
}

/*
 * Get List v: variable value.  Caller must take care of reference count when
 * needed.
 */
    list_T *
get_vim_var_list(ಪೂರ್ಣಾಂಕ idx)
{
    ಹಿಂತಿರುಗಿ vimvars[idx].vv_list;
}

/*
 * Get Dict v: variable value.  Caller must take care of reference count when
 * needed.
 */
    dict_T *
get_vim_var_dict(ಪೂರ್ಣಾಂಕ idx)
{
    ಹಿಂತಿರುಗಿ vimvars[idx].vv_dict;
}

/*
 * Set v:char to character "c".
 */
    ಶೂನ್ಯ
ಗಣ_vim_var_char(ಪೂರ್ಣಾಂಕ c)
{
    char_u  buf[MB_MAXBYTES + 1];
    size_t  buflen;

    ಆದರೆ (has_mbyte)
	buflen = (*mb_char2bytes)(c, buf);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	buf[0] = c;
	buflen = 1;
    }
    buf[buflen] = NUL;

    ಗಣ_vim_var_string(VV_CHAR, buf, (ಪೂರ್ಣಾಂಕ)buflen);
}

/*
 * Set v:count to "count" and v:count1 to "count1".
 * When "set_prevcount" is TRUE first set v:prevcount from v:count.
 */
    ಶೂನ್ಯ
ಗಣ_vcount(
    ದೀರ್ಘ	ಎಣಿಕೆ_2,
    ದೀರ್ಘ	count1,
    ಪೂರ್ಣಾಂಕ		ಗಣ_prevcount)
{
    ಆದರೆ (ಗಣ_prevcount)
	vimvars[VV_PREVCOUNT].vv_nr = vimvars[VV_ಎಣಿಕೆ].vv_nr;
    vimvars[VV_ಎಣಿಕೆ].vv_nr = ಎಣಿಕೆ_2;
    vimvars[VV_COUNT1].vv_nr = count1;
}

/*
 * Save variables that might be changed as a side effect.  Used when executing
 * a timer callback.
 */
    ಶೂನ್ಯ
save_vimvars(vimvars_save_T *vvsave)
{
    vvsave->vv_prevcount = vimvars[VV_PREVCOUNT].vv_nr;
    vvsave->vv_ಎಣಿಕೆ = vimvars[VV_ಎಣಿಕೆ].vv_nr;
    vvsave->vv_count1 = vimvars[VV_COUNT1].vv_nr;
}

/*
 * Restore variables saved by save_vimvars().
 */
    ಶೂನ್ಯ
restore_vimvars(vimvars_save_T *vvsave)
{
    vimvars[VV_PREVCOUNT].vv_nr = vvsave->vv_prevcount;
    vimvars[VV_ಎಣಿಕೆ].vv_nr = vvsave->vv_ಎಣಿಕೆ;
    vimvars[VV_COUNT1].vv_nr = vvsave->vv_count1;
}

/*
 * Set string v: variable to a copy of "val". If 'copy' is FALSE, then set the
 * value.
 */
    ಶೂನ್ಯ
ಗಣ_vim_var_string(
    ಪೂರ್ಣಾಂಕ		idx,
    char_u	*val,
    ಪೂರ್ಣಾಂಕ		len)	    // length of "val" to use or -1 (whole string)
{
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_tv_type = VAR_STRING;
    ಆದರೆ (val == ಖಾಲಿ)
	vimvars[idx].vv_str = ಖಾಲಿ;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (len == -1)
	vimvars[idx].vv_str = vim_strsave(val);
    ಇಲ್ಲದಿದ್ದರೆ
	vimvars[idx].vv_str = vim_strnsave(val, len);
}

    ಶೂನ್ಯ
ಗಣ_vim_var_string_ನೇರ(
    ಪೂರ್ಣಾಂಕ		idx,
    char_u	*val)
{
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_tv_type = VAR_STRING;

    vimvars[idx].vv_str = val;
}

/*
 * Set List v: variable to "val".
 */
    ಶೂನ್ಯ
ಗಣ_vim_var_list(ಪೂರ್ಣಾಂಕ idx, list_T *val)
{
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_tv_type = VAR_LIST;
    vimvars[idx].vv_list = val;
    ಆದರೆ (val != ಖಾಲಿ)
	++val->lv_refcount;
}

/*
 * Set Dictionary v: variable to "val".
 */
    ಶೂನ್ಯ
ಗಣ_vim_var_dict(ಪೂರ್ಣಾಂಕ idx, dict_T *val)
{
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_tv_type = VAR_DICT;
    vimvars[idx].vv_dict = val;
    ಆದರೆ (val == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ++val->dv_refcount;
    dict_ಗಣ_ವಸ್ತುಗಳು_ro(val);
}

/*
 * Set the v:argv list.
 */
    ಶೂನ್ಯ
ಗಣ_argv_var(ಅಕ್ಷರ **argv, ಪೂರ್ಣಾಂಕ argc)
{
    list_T	*l = list_alloc();
    ಪೂರ್ಣಾಂಕ		i;

    ಆದರೆ (l == ಖಾಲಿ)
	getout(1);
    l->lv_ಬೀಗ = VAR_FIXED;
    ಗಾಗಿ (i = 0; i < argc; ++i)
    {
	ಆದರೆ (list_append_string(l, (char_u *)argv[i], -1) == FAIL)
	    getout(1);
	l->lv_u.mat.lv_ಕೊನೆಯ->li_tv.v_ಬೀಗ = VAR_FIXED;
    }
    ಗಣ_vim_var_list(VV_ARGV, l);
}

/*
 * Reset v:register, taking the 'clipboard' setting into account.
 */
    ಶೂನ್ಯ
reset_reg_var(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ regname = 0;

    // Adjust the register according to 'clipboard', so that when
    // "unnamed" is present it becomes '*' or '+' instead of '"'.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_CLIPMETHOD
    adjust_clip_reg(&regname);
#ಆದರೆ_ಅಂತ್ಯ
    ಗಣ_reg_var(regname);
}

/*
 * Set v:register if needed.
 */
    ಶೂನ್ಯ
ಗಣ_reg_var(ಪೂರ್ಣಾಂಕ c)
{
    char_u  regname[2];

    ಆದರೆ (c == 0 || c == ' ')
	regname[0] = '"';
    ಇಲ್ಲದಿದ್ದರೆ
	regname[0] = c;
    regname[1] = NUL;
    // Avoid free/alloc when the value is already right.
    ಆದರೆ (vimvars[VV_REG].vv_str == ಖಾಲಿ || vimvars[VV_REG].vv_str[0] != c)
	ಗಣ_vim_var_string(VV_REG, regname, 1);
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
    ಆದರೆ (oldval == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ vimvars[VV_EXCEPTION].vv_str;

    vimvars[VV_EXCEPTION].vv_str = oldval;
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
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
    ಆದರೆ (oldval == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ vimvars[VV_THROWPOINT].vv_str;

    vimvars[VV_THROWPOINT].vv_str = oldval;
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Set v:cmdarg.
 * If "eap" != NULL, use "eap" to generate the value and return the old value.
 * If "oldarg" != NULL, restore the value to "oldarg" and return NULL.
 * Must always be called in pairs!
 */
    char_u *
ಗಣ_cmdarg(exarg_T *eap, char_u *oldarg)
{
    char_u	*oldval;
    char_u	*newval;
    ಚಿಹ್ನೆರಹಿತ	len;

    oldval = vimvars[VV_CMDARG].vv_str;
    ಆದರೆ (eap == ಖಾಲಿ)
    {
	vim_free(oldval);
	vimvars[VV_CMDARG].vv_str = oldarg;
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಆದರೆ (eap->force_bin == FORCE_BIN)
	len = 6;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (eap->force_bin == FORCE_NOBIN)
	len = 8;
    ಇಲ್ಲದಿದ್ದರೆ
	len = 0;

    ಆದರೆ (eap->read_edit)
	len += 7;

    ಆದರೆ (eap->force_ff != 0)
	len += 10; // " ++ff=unix"
    ಆದರೆ (eap->force_enc != 0)
	len += (ಚಿಹ್ನೆರಹಿತ)STRLEN(eap->cmd + eap->force_enc) + 7;
    ಆದರೆ (eap->bad_char != 0)
	len += 7 + 4;  // " ++bad=" + "keep" or "drop"

    newval = alloc(len + 1);
    ಆದರೆ (newval == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (eap->force_bin == FORCE_BIN)
	sprintf((ಅಕ್ಷರ *)newval, " ++bin");
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (eap->force_bin == FORCE_NOBIN)
	sprintf((ಅಕ್ಷರ *)newval, " ++nobin");
    ಇಲ್ಲದಿದ್ದರೆ
	*newval = NUL;

    ಆದರೆ (eap->read_edit)
	STRCAT(newval, " ++edit");

    ಆದರೆ (eap->force_ff != 0)
	sprintf((ಅಕ್ಷರ *)newval + STRLEN(newval), " ++ff=%s",
						eap->force_ff == 'u' ? "unix"
						: eap->force_ff == 'd' ? "dos"
						: "mac");
    ಆದರೆ (eap->force_enc != 0)
	sprintf((ಅಕ್ಷರ *)newval + STRLEN(newval), " ++enc=%s",
					       eap->cmd + eap->force_enc);
    ಆದರೆ (eap->bad_char == BAD_KEEP)
	STRCPY(newval + STRLEN(newval), " ++bad=keep");
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (eap->bad_char == BAD_DROP)
	STRCPY(newval + STRLEN(newval), " ++bad=drop");
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (eap->bad_char != 0)
	sprintf((ಅಕ್ಷರ *)newval + STRLEN(newval), " ++bad=%c", eap->bad_char);
    vimvars[VV_CMDARG].vv_str = newval;
    ಹಿಂತಿರುಗಿ oldval;
}

/*
 * Get the value of internal variable "name".
 * If "flags" has EVAL_VAR_IMPORT may return a VAR_ANY with v_number set to the
 * imported script ID.
 * Return OK or FAIL.  If OK is returned "rettv" must be cleared.
 */
    ಪೂರ್ಣಾಂಕ
eval_variable(
    char_u	*name,
    ಪೂರ್ಣಾಂಕ		len,		// length of "name" or zero
    scid_T	sid,		// script ID for imported item or zero
    typval_T	*rettv,		// NULL when only checking existence
    dictitem_T	**dip,		// non-NULL when typval's dict item is needed
    ಪೂರ್ಣಾಂಕ		flags)		// EVAL_VAR_ flags
{
    ಪೂರ್ಣಾಂಕ		ret = OK;
    typval_T	*tv = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		ಸಿಕ್ಕಿತು = FALSE;
    hashtab_T	*ht = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		cc = 0;
    type_T	*type = ಖಾಲಿ;

    ಆದರೆ (len > 0)
    {
	// truncate the name, so that we can use strcmp()
	cc = name[len];
	name[len] = NUL;
    }

    // Check for local variable when debugging.
    ಆದರೆ ((sid == 0) && (tv = lookup_debug_var(name)) == ಖಾಲಿ)
    {
	// Check for user-defined variables.
	dictitem_T	*v = ಕಂಡುಹಿಡಿ_var(name, &ht, flags & EVAL_VAR_NOAUTOLOAD);

	ಆದರೆ (v != ಖಾಲಿ)
	{
	    tv = &v->di_tv;
	    ಆದರೆ (dip != ಖಾಲಿ)
		*dip = v;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ht = ಖಾಲಿ;
    }

    ಆದರೆ (tv == ಖಾಲಿ && (ಒಳ_vim9script() || STRNCMP(name, "s:", 2) == 0))
    {
	imported_T  *import = ಖಾಲಿ;
	char_u	    *p = STRNCMP(name, "s:", 2) == 0 ? name + 2 : name;

	ಆದರೆ (sid == 0)
	    import = ಕಂಡುಹಿಡಿ_imported(p, 0, TRUE);

	// imported variable from another script
	ಆದರೆ (import != ಖಾಲಿ || sid != 0)
	{
	    ಆದರೆ ((flags & EVAL_VAR_IMPORT) == 0)
	    {
		ಆದರೆ (SCRIPT_ID_VALID(sid))
		{
		    ht = &SCRIPT_VARS(sid);
		    ಆದರೆ (ht != ಖಾಲಿ)
		    {
			dictitem_T *v = ಕಂಡುಹಿಡಿ_var_ಒಳ_ht(ht, 0, name,
						  flags & EVAL_VAR_NOAUTOLOAD);

			ಆದರೆ (v == ಖಾಲಿ)
			    v = ಕಂಡುಹಿಡಿ_var_autoload_ಪೂರ್ವಪ್ರತ್ಯಯ(name, sid, ಖಾಲಿ, ಖಾಲಿ);

			ಆದರೆ (v != ಖಾಲಿ)
			{
			    tv = &v->di_tv;
			    ಆದರೆ (dip != ಖಾಲಿ)
				*dip = v;
			}
			ಇಲ್ಲದಿದ್ದರೆ
			    ht = ಖಾಲಿ;
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಆದರೆ (flags & EVAL_VAR_VERBOSE)
			semsg(_(e_expected_dot_after_name_str), name);
		    ret = FAIL;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ (rettv != ಖಾಲಿ)
		{
		    // special value that is used in handle_subscript()
		    rettv->v_type = VAR_ANY;
		    rettv->vval.v_ಸಂಖ್ಯೆ = sid != 0 ? sid : import->imp_sid;
		}
		ಸಿಕ್ಕಿತು = TRUE;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಒಳ_vim9script() && (flags & EVAL_VAR_NO_FUNC) == 0)
	{
	    ಪೂರ್ಣಾಂಕ	    has_g_ಪೂರ್ವಪ್ರತ್ಯಯ = STRNCMP(name, "g:", 2) == 0;
	    ufunc_T *ufunc = ಕಂಡುಹಿಡಿ_func(name, FALSE);

	    ಆದರೆ (ufunc != ಖಾಲಿ && cc == '<')
	    {
		// handle generic function
		char_u	*argp = name + len;
		name[len] = cc;
		ufunc = eval_generic_func(ufunc, name, &argp);
		name[len] = NUL;
		ಆದರೆ (ufunc == ಖಾಲಿ)
		{
		    ret = FAIL;
		    ಹೋಗು ಮುಗಿದ;
		}
	    }

	    // In Vim9 script we can get a function reference by using the
	    // function name.  For a global non-autoload function "g:" is
	    // required.
	    ಆದರೆ (ufunc != ಖಾಲಿ && (has_g_ಪೂರ್ವಪ್ರತ್ಯಯ
					    || !func_requires_g_ಪೂರ್ವಪ್ರತ್ಯಯ(ufunc)))
	    {
		ಸಿಕ್ಕಿತು = TRUE;
		ಆದರೆ (rettv != ಖಾಲಿ)
		{
		    rettv->v_type = VAR_FUNC;
		    ಆದರೆ (has_g_ಪೂರ್ವಪ್ರತ್ಯಯ)
		    {
			// Keep the "g:", otherwise script-local may be
			// assumed.
			ಆದರೆ (cc != '<')
			    rettv->vval.v_string = vim_strsave(name);
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    // append the generic function arguments
			    char_u	*argp = name + len;
			    name[len] = cc;
			    rettv->vval.v_string =
				append_generic_func_type_ವಾದಗಳು(name, len,
									&argp);
			    name[len] = NUL;
			}
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			rettv->vval.v_string = vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
		    ಆದರೆ (rettv->vval.v_string != ಖಾಲಿ)
			func_ref(ufunc->uf_name);
		}
	    }
	}
    }

    ಆದರೆ (!ಸಿಕ್ಕಿತು)
    {
	ಆದರೆ (tv == ಖಾಲಿ)
	{
	    ಆದರೆ (rettv != ಖಾಲಿ && (flags & EVAL_VAR_VERBOSE))
		semsg(_(e_undefined_variable_str), name);
	    ret = FAIL;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (rettv != ಖಾಲಿ)
	{
	    svar_T  *sv = ಖಾಲಿ;
	    ಪೂರ್ಣಾಂಕ	    was_assigned = FALSE;

	    ಆದರೆ (ht != ಖಾಲಿ && ht == get_script_local_ht()
		    && tv != &SCRIPT_SV(current_sctx.sc_sid)->sv_var.di_tv)
	    {
		sv = ಕಂಡುಹಿಡಿ_typval_ಒಳ_script(tv, 0, TRUE);
		ಆದರೆ (sv != ಖಾಲಿ)
		{
		    type = sv->sv_type;
		    was_assigned = sv->sv_flags & SVFLAG_ASSIGNED;
		}
	    }

	    ಆದರೆ ((tv->v_type == VAR_TYPEALIAS || tv->v_type == VAR_CLASS)
		    && sid != 0)
	    {
		// type alias or class imported from another script.  Check
		// whether it is exported from the other script.
		sv = ಕಂಡುಹಿಡಿ_typval_ಒಳ_script(tv, sid, TRUE);
		ಆದರೆ (sv == ಖಾಲಿ)
		{
		    ret = FAIL;
		    ಹೋಗು ಮುಗಿದ;
		}
		ಆದರೆ ((sv->sv_flags & SVFLAG_EXPORTED) == 0)
		{
		    semsg(_(e_ಅಂಶ_not_exported_ಒಳ_script_str), name);
		    ret = FAIL;
		    ಹೋಗು ಮುಗಿದ;
		}
	    }

	    // If a list or tuple or dict variable wasn't initialized and has
	    // meaningful type, do it now.  Not for global variables, they are
	    // not declared.
	    ಆದರೆ (ht != &globvarht)
	    {
		ಆದರೆ (tv->v_type == VAR_DICT && tv->vval.v_dict == ಖಾಲಿ
					    && ((type != ಖಾಲಿ && !was_assigned)
							  || !ಒಳ_vim9script()))
		{
		    tv->vval.v_dict = dict_alloc();
		    ಆದರೆ (tv->vval.v_dict != ಖಾಲಿ)
		    {
			++tv->vval.v_dict->dv_refcount;
			tv->vval.v_dict->dv_type = alloc_type(type);
			ಆದರೆ (sv != ಖಾಲಿ)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tv->v_type == VAR_LIST && tv->vval.v_list == ಖಾಲಿ
					    && ((type != ಖಾಲಿ && !was_assigned)
							  || !ಒಳ_vim9script()))
		{
		    tv->vval.v_list = list_alloc();
		    ಆದರೆ (tv->vval.v_list != ಖಾಲಿ)
		    {
			++tv->vval.v_list->lv_refcount;
			tv->vval.v_list->lv_type = alloc_type(type);
			ಆದರೆ (sv != ಖಾಲಿ)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tv->v_type == VAR_TUPLE && tv->vval.v_tuple == ಖಾಲಿ
					    && ((type != ಖಾಲಿ && !was_assigned)
							  || !ಒಳ_vim9script()))
		{
		    tv->vval.v_tuple = tuple_alloc();
		    ಆದರೆ (tv->vval.v_tuple != ಖಾಲಿ)
		    {
			++tv->vval.v_tuple->tv_refcount;
			tv->vval.v_tuple->tv_type = alloc_type(type);
			ಆದರೆ (sv != ಖಾಲಿ)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tv->v_type == VAR_BLOB && tv->vval.v_blob == ಖಾಲಿ
					    && ((type != ಖಾಲಿ && !was_assigned)
							  || !ಒಳ_vim9script()))
		{
		    tv->vval.v_blob = blob_alloc();
		    ಆದರೆ (tv->vval.v_blob != ಖಾಲಿ)
		    {
			++tv->vval.v_blob->bv_refcount;
			ಆದರೆ (sv != ಖಾಲಿ)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
	    }
	    ret = copy_tv(tv, rettv);
	}
    }

ಮುಗಿದ:
    ಆದರೆ (len > 0)
	name[len] = cc;

    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Get the value of internal variable "name", also handling "import.name".
 * Return OK or FAIL.  If OK is returned "rettv" must be cleared.
 */
    ಪೂರ್ಣಾಂಕ
eval_variable_import(
    char_u	*name,
    typval_T	*rettv)
{
    char_u  *s = name;
    ಆಗ (ASCII_ISALNUM(*s) || *s == '_')
	++s;
    ಪೂರ್ಣಾಂಕ	    len = (ಪೂರ್ಣಾಂಕ)(s - name);

    ಆದರೆ (eval_variable(name, len, 0, rettv, ಖಾಲಿ, EVAL_VAR_IMPORT) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;
    ಆದರೆ (rettv->v_type == VAR_ANY && *s == '.')
    {
	char_u *ns = s + 1;
	s = ns;
	ಆಗ (ASCII_ISALNUM(*s) || *s == '_')
	    ++s;
	ಪೂರ್ಣಾಂಕ sid = rettv->vval.v_ಸಂಖ್ಯೆ;
	ಹಿಂತಿರುಗಿ eval_variable(ns, (ಪೂರ್ಣಾಂಕ)(s - ns), sid, rettv, ಖಾಲಿ, 0);
    }
    ಹಿಂತಿರುಗಿ OK;
}


/*
 * Check if variable "name[len]" is a local variable or an argument.
 * If so, "*eval_lavars_used" is set to TRUE.
 */
    ಶೂನ್ಯ
check_vars(char_u *name, ಪೂರ್ಣಾಂಕ len)
{
    ಪೂರ್ಣಾಂಕ		cc;
    char_u	*varname;
    hashtab_T	*ht;

    ಆದರೆ (eval_lavars_ಬಳಸಿದ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    // truncate the name, so that we can use strcmp()
    cc = name[len];
    name[len] = NUL;

    ht = ಕಂಡುಹಿಡಿ_var_ht(name, &varname);
    ಆದರೆ (ht == get_funccal_local_ht() || ht == get_funccal_ವಾದಗಳು_ht())
    {
	ಆದರೆ (ಕಂಡುಹಿಡಿ_var(name, ಖಾಲಿ, TRUE) != ಖಾಲಿ)
	    *eval_lavars_ಬಳಸಿದ = TRUE;
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
ಕಂಡುಹಿಡಿ_var(char_u *name, hashtab_T **htp, ಪೂರ್ಣಾಂಕ no_autoload)
{
    char_u	*varname;
    hashtab_T	*ht;
    dictitem_T	*ret = ಖಾಲಿ;

    ht = ಕಂಡುಹಿಡಿ_var_ht(name, &varname);
    ಆದರೆ (htp != ಖಾಲಿ)
	*htp = ht;
    ಆದರೆ (ht == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ret = ಕಂಡುಹಿಡಿ_var_ಒಳ_ht(ht, *name, varname, no_autoload);
    ಆದರೆ (ret != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ret;

    // Search in parent scope for lambda
    ret = ಕಂಡುಹಿಡಿ_var_ಒಳ_scoped_ht(name, no_autoload);
    ಆದರೆ (ret != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ret;

    // in Vim9 script items without a scope can be script-local
    ಆದರೆ (ಒಳ_vim9script() && name[0] != NUL && name[1] != ':')
    {
	ht = get_script_local_ht();
	ಆದರೆ (ht != ಖಾಲಿ)
	{
	    ret = ಕಂಡುಹಿಡಿ_var_ಒಳ_ht(ht, *name, varname, no_autoload);
	    ಆದರೆ (ret != ಖಾಲಿ)
	    {
		ಆದರೆ (htp != ಖಾಲಿ)
		    *htp = ht;
		ಹಿಂತಿರುಗಿ ret;
	    }
	}
    }

    // and finally try
    ಹಿಂತಿರುಗಿ ಕಂಡುಹಿಡಿ_var_autoload_ಪೂರ್ವಪ್ರತ್ಯಯ(name, 0, htp, ಖಾಲಿ);
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
ಕಂಡುಹಿಡಿ_var_autoload_ಪೂರ್ವಪ್ರತ್ಯಯ(char_u *name, ಪೂರ್ಣಾಂಕ sid, hashtab_T **htp,
							    char_u **namep)
{
    hashtab_T	*ht;
    dictitem_T	*ret = ಖಾಲಿ;
    // When using "vim9script autoload" script-local items are prefixed but can
    // be used with s:name.
    ಪೂರ್ಣಾಂಕ check_sid = sid > 0 ? sid : current_sctx.sc_sid;
    ಆದರೆ (SCRIPT_ID_VALID(check_sid)
		   && (ಒಳ_vim9script() || (name[0] == 's' && name[1] == ':')))
    {
	scriptitem_T *si = SCRIPT_ಅಂಶ(check_sid);

	ಆದರೆ (si->sn_autoload_ಪೂರ್ವಪ್ರತ್ಯಯ != ಖಾಲಿ)
	{
	    char_u *ತಳ_name = (name[0] == 's' && name[1] == ':')
							     ? name + 2 : name;
	    char_u *auto_name = concat_str(si->sn_autoload_ಪೂರ್ವಪ್ರತ್ಯಯ, ತಳ_name);

	    ಆದರೆ (auto_name != ಖಾಲಿ)
	    {
		ಪೂರ್ಣಾಂಕ free_auto_name = TRUE;
		ht = &globvarht;
		ret = ಕಂಡುಹಿಡಿ_var_ಒಳ_ht(ht, 'g', auto_name, TRUE);
		ಆದರೆ (ret != ಖಾಲಿ)
		{
		    ಆದರೆ (htp != ಖಾಲಿ)
			*htp = ht;
		    ಆದರೆ (namep != ಖಾಲಿ)
		    {
			free_auto_name = FALSE;
			*namep = auto_name;
		    }
		}
		ಆದರೆ (free_auto_name)
		    vim_free(auto_name);
	    }
	}
    }

    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Like find_var() but if the name starts with <SNR>99_ then look in the
 * referenced script (used for a funcref).
 */
    dictitem_T *
ಕಂಡುಹಿಡಿ_var_also_ಒಳ_script(char_u *name, hashtab_T **htp, ಪೂರ್ಣಾಂಕ no_autoload)
{
    ಆದರೆ (STRNCMP(name, "<SNR>", 5) == 0 && ಸುರಕ್ಷಿತ_isdigit(name[5]))
    {
	char_u	    *p = name + 5;
	ಪೂರ್ಣಾಂಕ	    sid = getdigits(&p);

	ಆದರೆ (SCRIPT_ID_VALID(sid) && *p == '_')
	{
	    hashtab_T	*ht = &SCRIPT_VARS(sid);

	    ಆದರೆ (ht != ಖಾಲಿ)
	    {
		dictitem_T *di = ಕಂಡುಹಿಡಿ_var_ಒಳ_ht(ht, 0, p + 1, no_autoload);

		ಆದರೆ (di != ಖಾಲಿ)
		{
		    ಆದರೆ (htp != ಖಾಲಿ)
			*htp = ht;
		    ಹಿಂತಿರುಗಿ di;
		}
	    }
	}
    }

    ಹಿಂತಿರುಗಿ ಕಂಡುಹಿಡಿ_var(name, htp, no_autoload);
}

/*
 * Find variable "varname" in hashtab "ht" with name "htname".
 * When "varname" is empty returns curwin/curtab/etc vars dictionary.
 * Returns NULL if not found.
 */
    dictitem_T *
ಕಂಡುಹಿಡಿ_var_ಒಳ_ht(
    hashtab_T	*ht,
    ಪೂರ್ಣಾಂಕ		htname,
    char_u	*varname,
    ಪೂರ್ಣಾಂಕ		no_autoload)
{
    hashitem_T	*hi;

    ಆದರೆ (*varname == NUL)
    {
	// Must be something like "s:", otherwise "ht" would be NULL.
	ಆಯ್ಕೆ (htname)
	{
	    ಸ್ಥಿತಿ 's': ಹಿಂತಿರುಗಿ &SCRIPT_SV(current_sctx.sc_sid)->sv_var;
	    ಸ್ಥಿತಿ 'g': ಹಿಂತಿರುಗಿ &globvars_var;
	    ಸ್ಥಿತಿ 'v': ಹಿಂತಿರುಗಿ &vimvars_var;
	    ಸ್ಥಿತಿ 'b': ಹಿಂತಿರುಗಿ &curbuf->b_bufvar;
	    ಸ್ಥಿತಿ 'w': ಹಿಂತಿರುಗಿ &curwin->w_winvar;
	    ಸ್ಥಿತಿ 't': ಹಿಂತಿರುಗಿ &curtab->tp_winvar;
	    ಸ್ಥಿತಿ 'l': ಹಿಂತಿರುಗಿ get_funccal_local_var();
	    ಸ್ಥಿತಿ 'a': ಹಿಂತಿರುಗಿ get_funccal_ವಾದಗಳು_var();
	}
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    hi = hash_ಕಂಡುಹಿಡಿ(ht, varname);
    ಆದರೆ (HASHITEM_ಖಾಲಿಗಳು(hi))
    {
	// For global variables we may try auto-loading the script.  If it
	// worked find the variable again.  Don't auto-load a script if it was
	// loaded already, otherwise it would be loaded every time when
	// checking if a function name is a Funcref variable.
	ಆದರೆ (ht == &globvarht && !no_autoload)
	{
	    // Note: script_autoload() may make "hi" invalid. It must either
	    // be obtained again or not used.
	    ಆದರೆ (!script_autoload(varname, FALSE) || aborting())
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    hi = hash_ಕಂಡುಹಿಡಿ(ht, varname);
	}
	ಆದರೆ (HASHITEM_ಖಾಲಿಗಳು(hi))
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಹಿಂತಿರುಗಿ HI2DI(hi);
}

/*
 * Get the script-local hashtab.  NULL if not in a script context.
 */
    hashtab_T *
get_script_local_ht(ಶೂನ್ಯ)
{
    scid_T sid = current_sctx.sc_sid;

    ಆದರೆ (SCRIPT_ID_VALID(sid))
	ಹಿಂತಿರುಗಿ &SCRIPT_VARS(sid);
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Look for "name[len]" in script-local variables and functions.
 * When "cmd" is TRUE it must look like a command, a function must be followed
 * by "(" or "->".
 * Return OK when found, FAIL when not found.
 */
    ಪೂರ್ಣಾಂಕ
lookup_scriptitem(
	char_u	*name,
	size_t	len,
	ಪೂರ್ಣಾಂಕ	cmd,
	cctx_T	*dummy ಬಳಸದ)
{
    hashtab_T	*ht = get_script_local_ht();
    char_u	ಬಫರ್[30];
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		res;
    hashitem_T	*hi;
    ಪೂರ್ಣಾಂಕ		is_global = FALSE;
    char_u	*fname = name;

    ಆದರೆ (ht == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    ಆದರೆ (len < ಗಾತ್ರ(ಬಫರ್) - 1)
    {
	// avoid an alloc/free for short names
	vim_strncpy(ಬಫರ್, name, len);
	p = ಬಫರ್;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	p = vim_strnsave(name, len);
	ಆದರೆ (p == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;
    }

    hi = hash_ಕಂಡುಹಿಡಿ(ht, p);
    res = HASHITEM_ಖಾಲಿಗಳು(hi) ? FAIL : OK;

    // if not script-local, then perhaps autoload-exported
    ಆದರೆ (res == FAIL && ಕಂಡುಹಿಡಿ_var_autoload_ಪೂರ್ವಪ್ರತ್ಯಯ(p, 0, ಖಾಲಿ, ಖಾಲಿ) != ಖಾಲಿ)
	res = OK;

    // if not script-local or autoload, then perhaps imported
    ಆದರೆ (res == FAIL && ಕಂಡುಹಿಡಿ_imported(p, 0, FALSE) != ಖಾಲಿ)
	res = OK;
    ಆದರೆ (p != ಬಫರ್)
	vim_free(p);

    // Find a function, so that a following "->" works.
    // When used as a command require "(" or "->" to follow, "Cmd" is a user
    // command while "Cmd()" is a function call.
    ಆದರೆ (res != OK)
    {
	p = skipwhite(name + len);

	ಆದರೆ (!cmd || name[len] == '(' || (p[0] == '-' && p[1] == '>'))
	{
	    // Do not check for an internal function, since it might also be a
	    // valid command, such as ":split" versus "split()".
	    // Skip "g:" before a function name.
	    ಆದರೆ (name[0] == 'g' && name[1] == ':')
	    {
		is_global = TRUE;
		fname = name + 2;
	    }
	    ಆದರೆ (ಕಂಡುಹಿಡಿ_func(fname, is_global) != ಖಾಲಿ)
		res = OK;
	}
    }

    ಹಿಂತಿರುಗಿ res;
}

/*
 * Find the hashtab used for a variable name.
 * Return NULL if the name is not valid.
 * Set "varname" to the start of name without ':'.
 */
    hashtab_T *
ಕಂಡುಹಿಡಿ_var_ht(char_u *name, char_u **varname)
{
    hashitem_T	*hi;
    hashtab_T	*ht;

    ಆದರೆ (name[0] == NUL)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಆದರೆ (name[1] != ':')
    {
	// The name must not start with a colon or #.
	ಆದರೆ (name[0] == ':' || name[0] == AUTOLOAD_CHAR)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	*varname = name;

	// "version" is "v:version" in all scopes if scriptversion < 3.
	// Same for a few other variables marked with VV_COMPAT.
	ಆದರೆ (ಒಳ_old_script(3))
	{
	    hi = hash_ಕಂಡುಹಿಡಿ(&compat_hashtab, name);
	    ಆದರೆ (!HASHITEM_ಖಾಲಿಗಳು(hi))
		ಹಿಂತಿರುಗಿ &compat_hashtab;
	}

	ht = get_funccal_local_ht();
	ಆದರೆ (ht != ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ ht;				// local variable

	// In Vim9 script items at the script level are script-local, except
	// for autoload names.
	ಆದರೆ (ಒಳ_vim9script() && vim_strchr(name, AUTOLOAD_CHAR) == ಖಾಲಿ)
	{
	    ht = get_script_local_ht();
	    ಆದರೆ (ht != ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ ht;
	}

	ಹಿಂತಿರುಗಿ &globvarht;			// global variable
    }
    *varname = name + 2;
    ಆದರೆ (*name == 'g')				// global variable
	ಹಿಂತಿರುಗಿ &globvarht;
    // There must be no ':' or '#' in the rest of the name, unless g: is used
    ಆದರೆ (vim_strchr(name + 2, ':') != ಖಾಲಿ
			       || vim_strchr(name + 2, AUTOLOAD_CHAR) != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಆದರೆ (*name == 'b')				// buffer variable
	ಹಿಂತಿರುಗಿ &curbuf->b_vars->dv_hashtab;
    ಆದರೆ (*name == 'w')				// window variable
	ಹಿಂತಿರುಗಿ &curwin->w_vars->dv_hashtab;
    ಆದರೆ (*name == 't')				// tab page variable
	ಹಿಂತಿರುಗಿ &curtab->tp_vars->dv_hashtab;
    ಆದರೆ (*name == 'v')				// v: variable
	ಹಿಂತಿರುಗಿ &vimvarht;
    ಆದರೆ (get_current_funccal() != ಖಾಲಿ
	       && get_current_funccal()->fc_func->uf_def_ಸ್ಥಿತಿ
							    == UF_NOT_COMPILED)
    {
	// a: and l: are only used in functions defined with ":function"
	ಆದರೆ (*name == 'a')			// a: function argument
	    ಹಿಂತಿರುಗಿ get_funccal_ವಾದಗಳು_ht();
	ಆದರೆ (*name == 'l')			// l: local function variable
	    ಹಿಂತಿರುಗಿ get_funccal_local_ht();
    }
    ಆದರೆ (*name == 's')				// script variable
    {
	ht = get_script_local_ht();
	ಆದರೆ (ht != ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ ht;
    }
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Get the string value of a (global/local) variable.
 * Note: see tv_get_string() for how long the pointer remains valid.
 * Returns NULL when it doesn't exist.
 */
    char_u *
get_var_ಮೌಲ್ಯ(char_u *name)
{
    dictitem_T	*v;

    v = ಕಂಡುಹಿಡಿ_var(name, ಖಾಲಿ, FALSE);
    ಆದರೆ (v == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ tv_get_string(&v->di_tv);
}

/*
 * Allocate a new hashtab for a sourced script.  It will be used while
 * sourcing this script and when executing functions defined in the script.
 */
    ಶೂನ್ಯ
new_script_vars(scid_T id)
{
    scriptvar_T *sv;

    sv = ALLOC_CLEAR_ONE(scriptvar_T);
    ಆದರೆ (sv == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    init_var_dict(&sv->sv_dict, &sv->sv_var, VAR_SCOPE);
    SCRIPT_ಅಂಶ(id)->sn_vars = sv;
}

/*
 * Initialize dictionary "dict" as a scope and set variable "dict_var" to
 * point to it.
 */
    ಶೂನ್ಯ
init_var_dict(dict_T *dict, dictitem_T *dict_var, ಪೂರ್ಣಾಂಕ scope)
{
    hash_init(&dict->dv_hashtab);
    dict->dv_ಬೀಗ = 0;
    dict->dv_scope = scope;
    dict->dv_refcount = DO_NOT_FREE_CNT;
    dict->dv_copyID = 0;
    dict_var->di_tv.vval.v_dict = dict;
    dict_var->di_tv.v_type = VAR_DICT;
    dict_var->di_tv.v_ಬೀಗ = VAR_FIXED;
    dict_var->di_flags = DI_FLAGS_RO | DI_FLAGS_FIX;
    dict_var->di_ಕೀಲಿ[0] = NUL;
}

/*
 * Unreference a dictionary initialized by init_var_dict().
 */
    ಶೂನ್ಯ
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
    ಶೂನ್ಯ
vars_clear(hashtab_T *ht)
{
    vars_clear_ext(ht, TRUE);
}

/*
 * Like vars_clear(), but only free the value if "free_val" is TRUE.
 */
    ಶೂನ್ಯ
vars_clear_ext(hashtab_T *ht, ಪೂರ್ಣಾಂಕ free_val)
{
    ಪೂರ್ಣಾಂಕ		todo;
    hashitem_T	*hi;
    dictitem_T	*v;

    hash_ಬೀಗ(ht);
    todo = (ಪೂರ್ಣಾಂಕ)ht->ht_ಬಳಸಿದ;
    FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(ht, hi, todo)
    {
	ಆದರೆ (!HASHITEM_ಖಾಲಿಗಳು(hi))
	{
	    --todo;

	    // Free the variable.  Don't remove it from the hashtab,
	    // ht_array might change then.  hash_clear() takes care of it
	    // later.
	    v = HI2DI(hi);
	    ಆದರೆ (free_val)
		clear_tv(&v->di_tv);
	    ಆದರೆ (v->di_flags & DI_FLAGS_ALLOC)
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
    ಶೂನ್ಯ
delete_var(hashtab_T *ht, hashitem_T *hi)
{
    dictitem_T	*di = HI2DI(hi);

    ಆದರೆ (hash_remove(ht, hi, "delete variable") != OK)
	ಹಿಂತಿರುಗಿ;

    clear_tv(&di->di_tv);
    vim_free(di);
}

/*
 * Delete the exported variables of a reloaded Vim9 autoload script.  They live
 * in the global namespace with the autoload prefix (e.g. "foo#bar") and are
 * recreated when the script body runs again.
 */
    ಶೂನ್ಯ
delete_autoload_export_vars(char_u *ಪೂರ್ವಪ್ರತ್ಯಯ)
{
    ಆದರೆ (ಪೂರ್ವಪ್ರತ್ಯಯ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    size_t  prefixlen = STRLEN(ಪೂರ್ವಪ್ರತ್ಯಯ);

    hash_ಬೀಗ(&globvarht);
    ಪೂರ್ಣಾಂಕ	todo = (ಪೂರ್ಣಾಂಕ)globvarht.ht_ಬಳಸಿದ;

    ಗಾಗಿ (hashitem_T *hi = globvarht.ht_array; todo > 0; ++hi)
	ಆದರೆ (!HASHITEM_ಖಾಲಿಗಳು(hi))
	{
	    dictitem_T	*di = HI2DI(hi);

	    --todo;
	    // Keep a class or enum: existing objects still refer to it.
	    ಆದರೆ (di->di_tv.v_type != VAR_CLASS
		    && STRNCMP(di->di_ಕೀಲಿ, ಪೂರ್ವಪ್ರತ್ಯಯ, prefixlen) == 0)
		delete_var(&globvarht, hi);
	}
    hash_unlock(&globvarht);
}

/*
 * List the value of one internal variable.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
list_one_var(dictitem_T *v, ಅಕ್ಷರ *ಪೂರ್ವಪ್ರತ್ಯಯ, ಪೂರ್ಣಾಂಕ *ಮೊದಲ)
{
    char_u	*tofree;
    char_u	*s;
    char_u	numbuf[NUMBUFLEN];

    s = echo_string(&v->di_tv, &tofree, numbuf, get_copyID());
    list_one_var_a(ಪೂರ್ವಪ್ರತ್ಯಯ, v->di_ಕೀಲಿ, v->di_tv.v_type,
					 s == ಖಾಲಿ ? (char_u *)"" : s, ಮೊದಲ);
    vim_free(tofree);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
list_one_var_a(
    ಅಕ್ಷರ	*ಪೂರ್ವಪ್ರತ್ಯಯ,
    char_u	*name,
    ಪೂರ್ಣಾಂಕ		type,
    char_u	*string,
    ಪೂರ್ಣಾಂಕ		*ಮೊದಲ)  // when TRUE clear rest of screen and set to FALSE
{
    // don't use msg() or msg_attr() to avoid overwriting "v:statusmsg"
    ಸಂದೇಶ_ಆರಂಭ();
    ಸಂದೇಶ_puts(ಪೂರ್ವಪ್ರತ್ಯಯ);
    ಆದರೆ (name != ಖಾಲಿ)	// "a:" vars don't have a name stored
	ಸಂದೇಶ_puts((ಅಕ್ಷರ *)name);
    ಸಂದೇಶ_putchar(' ');
    ಸಂದೇಶ_advance(22);
    ಆದರೆ (type == VAR_ಸಂಖ್ಯೆ)
	ಸಂದೇಶ_putchar('#');
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type == VAR_FUNC || type == VAR_PARTIAL)
	ಸಂದೇಶ_putchar('*');
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type == VAR_LIST)
    {
	ಸಂದೇಶ_putchar('[');
	ಆದರೆ (*string == '[')
	    ++string;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type == VAR_DICT)
    {
	ಸಂದೇಶ_putchar('{');
	ಆದರೆ (*string == '{')
	    ++string;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ಸಂದೇಶ_putchar(' ');

    ಸಂದೇಶ_outtrans(string);

    ಆದರೆ (type == VAR_FUNC || type == VAR_PARTIAL)
	ಸಂದೇಶ_puts("()");
    ಆದರೆ (*ಮೊದಲ)
    {
	ಸಂದೇಶ_clr_eos();
	*ಮೊದಲ = FALSE;
    }
}

/*
 * Addition handling for setting a v: variable.
 * Return TRUE if the variable should be set normally,
 *        FALSE if nothing else needs to be done.
 */
    ಪೂರ್ಣಾಂಕ
before_ಗಣ_vvar(
    char_u	*varname,
    dictitem_T	*di,
    typval_T	*tv,
    ಪೂರ್ಣಾಂಕ		copy,
    ಪೂರ್ಣಾಂಕ		*type_error)
{
    ಆದರೆ (di->di_tv.v_type == VAR_STRING)
    {
	VIM_CLEAR(di->di_tv.vval.v_string);
	ಆದರೆ (copy || tv->v_type != VAR_STRING)
	{
	    char_u *val = tv_get_string(tv);

	    // Careful: when assigning to v:errmsg and
	    // tv_get_string() causes an error message the variable
	    // will already be set.
	    ಆದರೆ (di->di_tv.vval.v_string == ಖಾಲಿ)
		di->di_tv.vval.v_string = vim_strsave(val);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // Take over the string to avoid an extra alloc/free.
	    di->di_tv.vval.v_string = tv->vval.v_string;
	    tv->vval.v_string = ಖಾಲಿ;
	}
	ಹಿಂತಿರುಗಿ FALSE;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (di->di_tv.v_type == VAR_ಸಂಖ್ಯೆ)
    {
	di->di_tv.vval.v_ಸಂಖ್ಯೆ = tv_get_ಸಂಖ್ಯೆ(tv);
	ಆದರೆ (STRCMP(varname, "searchforward") == 0)
	    ಗಣ_ಹುಡುಕು_direction(di->di_tv.vval.v_ಸಂಖ್ಯೆ ? '/' : '?');
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಹುಡುಕು_EXTRA
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(varname, "hlsearch") == 0)
	{
	    no_hlsearch = !di->di_tv.vval.v_ಸಂಖ್ಯೆ;
	    redraw_all_later(UPD_SOME_VALID);
	}
#ಆದರೆ_ಅಂತ್ಯ
	ಹಿಂತಿರುಗಿ FALSE;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (di->di_tv.v_type != tv->v_type)
    {
	*type_error = TRUE;
	ಹಿಂತಿರುಗಿ FALSE;
    }
    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Set variable "name" to value in "tv".
 * If the variable already exists, the value is updated.
 * Otherwise the variable is created.
 */
    ಶೂನ್ಯ
ಗಣ_var(
    char_u	*name,
    typval_T	*tv,
    ಪೂರ್ಣಾಂಕ		copy)	    // make copy of value in "tv"
{
    ಗಣ_var_const(name, 0, ಖಾಲಿ, tv, copy, ನಿಯೋಜಿಸು_DECL, 0);
}

/*
 * Set variable "name" to value in "tv_arg".
 * When "sid" is non-zero "name" is in the script with this ID.
 * If the variable already exists and "is_const" is FALSE the value is updated.
 * Otherwise the variable is created.
 */
    ಪೂರ್ಣಾಂಕ
ಗಣ_var_const(
    char_u	*name,
    scid_T	sid,
    type_T	*type_arg,
    typval_T	*tv_arg,
    ಪೂರ್ಣಾಂಕ		copy,	    // make copy of value in "tv"
    ಪೂರ್ಣಾಂಕ		flags_arg,  // ASSIGN_CONST, ASSIGN_FINAL, etc.
    ಪೂರ್ಣಾಂಕ		var_idx)    // index for ":let [a, b] = list"
{
    typval_T	*tv = tv_arg;
    type_T	*type = type_arg;
    typval_T	bool_tv;
    dictitem_T	*di;
    typval_T	*dest_tv;
    char_u	*varname;
    char_u	*name_tofree = ಖಾಲಿ;
    hashtab_T	*ht = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		is_script_local;
    ಪೂರ್ಣಾಂಕ		vim9script = ಒಳ_vim9script();
    ಪೂರ್ಣಾಂಕ		var_ಒಳ_vim9script;
    ಪೂರ್ಣಾಂಕ		var_ಒಳ_autoload = FALSE;
    ಪೂರ್ಣಾಂಕ		flags = flags_arg;
    ಪೂರ್ಣಾಂಕ		free_tv_arg = !copy;  // free tv_arg if not used
    ಪೂರ್ಣಾಂಕ		rc = FAIL;

    ಆದರೆ (sid != 0)
    {
	varname = ಖಾಲಿ;
	ಆದರೆ (SCRIPT_ID_VALID(sid))
	{
	    char_u	*auto_name = ಖಾಲಿ;
	    ಆದರೆ (ಕಂಡುಹಿಡಿ_var_autoload_ಪೂರ್ವಪ್ರತ್ಯಯ(name, sid, &ht, &auto_name) != ಖಾಲಿ)
	    {
		var_ಒಳ_autoload = TRUE;
		varname = auto_name;
		name_tofree = varname;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		ht = &SCRIPT_VARS(sid);
	}
	ಆದರೆ (varname == ಖಾಲಿ)
	    varname = name;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	scriptitem_T	*si;
	char_u		*auto_name = ಖಾಲಿ;

	ಆದರೆ (ಒಳ_vim9script()
	    && SCRIPT_ID_VALID(current_sctx.sc_sid)
	    && (si = SCRIPT_ಅಂಶ(current_sctx.sc_sid))
					      ->sn_autoload_ಪೂರ್ವಪ್ರತ್ಯಯ != ಖಾಲಿ
	    && (is_export
		|| ಕಂಡುಹಿಡಿ_var_autoload_ಪೂರ್ವಪ್ರತ್ಯಯ(name, 0, ಖಾಲಿ, &auto_name)
								    != ಖಾಲಿ))
	{
	    // In a vim9 autoload script an exported variable is put in the
	    // global namespace with the autoload prefix.
	    var_ಒಳ_autoload = TRUE;
	    varname = auto_name != ಖಾಲಿ ? auto_name
		      : concat_str(si->sn_autoload_ಪೂರ್ವಪ್ರತ್ಯಯ, name);
	    ಆದರೆ (varname == ಖಾಲಿ)
		ಹೋಗು failed;
	    name_tofree = varname;
	    ht = &globvarht;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ht = ಕಂಡುಹಿಡಿ_var_ht(name, &varname);
    }
    ಆದರೆ (ht == ಖಾಲಿ || *varname == NUL)
    {
	semsg(_(e_illegal_variable_name_str), name);
	ಹೋಗು failed;
    }
    is_script_local = ht == get_script_local_ht() || sid != 0
							    || var_ಒಳ_autoload;

    ಆದರೆ (vim9script
	    && !is_script_local
	    && (flags & (ನಿಯೋಜಿಸು_NO_DECL | ನಿಯೋಜಿಸು_DECL)) == 0
	    && (flags & (ನಿಯೋಜಿಸು_CONST | ನಿಯೋಜಿಸು_FINAL)) == 0
	    && name[1] == ':')
    {
	vim9_declare_error(name);
	ಹೋಗು failed;
    }
    ಆದರೆ ((flags & ನಿಯೋಜಿಸು_FOR_LOOP) && is_scoped_variable(name))
	// Do not make g:var, w:var, b:var or t:var final.
	flags &= ~ನಿಯೋಜಿಸು_FINAL;

    var_ಒಳ_vim9script = is_script_local && current_script_is_vim9();
    ಆದರೆ (var_ಒಳ_vim9script && name[0] == '_' && name[1] == NUL)
    {
	// For "[a, _] = list" the underscore is ignored.
	ಆದರೆ ((flags & ನಿಯೋಜಿಸು_UNPACK) == 0)
	    emsg(_(e_cannot_use_underscore_ಇಲ್ಲಿ));
	ಹೋಗು failed;
    }

    di = ಕಂಡುಹಿಡಿ_var_ಒಳ_ht(ht, 0, varname, TRUE);

    ಆದರೆ (di == ಖಾಲಿ && var_ಒಳ_vim9script)
    {
	imported_T  *import = ಕಂಡುಹಿಡಿ_imported(varname, 0, FALSE);

	ಆದರೆ (import != ಖಾಲಿ)
	{
	    // imported name space cannot be used
	    ಆದರೆ ((flags & ನಿಯೋಜಿಸು_NO_DECL) == 0)
	    {
		semsg(_(e_redefining_imported_ಅಂಶ_str), name);
		ಹೋಗು failed;
	    }
	    semsg(_(e_cannot_use_str_itself_it_is_imported), name);
	    ಹೋಗು failed;
	}
	ಆದರೆ (!ಒಳ_vim9script())
	{
	    semsg(_(e_cannot_create_vim9_script_variable_ಒಳ_function_str),
									 name);
	    ಹೋಗು failed;
	}
    }

    // Search in parent scope which is possible to reference from lambda
    ಆದರೆ (di == ಖಾಲಿ)
	di = ಕಂಡುಹಿಡಿ_var_ಒಳ_scoped_ht(name, TRUE);

    ಆದರೆ ((tv->v_type == VAR_FUNC || tv->v_type == VAR_PARTIAL)
				  && var_wrong_func_name(name, di == ಖಾಲಿ))
	ಹೋಗು failed;

    ಆದರೆ (need_convert_ಗಮ್ಯ_bool(type, tv))
    {
	// Destination is a bool and the value is not, but it can be
	// converted.
	CLEAR_ಕ್ಷೇತ್ರ(bool_tv);
	bool_tv.v_type = VAR_BOOL;
	bool_tv.vval.v_ಸಂಖ್ಯೆ = tv2bool(tv) ? VVAL_TRUE : VVAL_FALSE;
	tv = &bool_tv;
    }

    ಆದರೆ (di != ಖಾಲಿ)
    {
	// Item already exists.  Allowed to replace when reloading.
	ಆದರೆ ((di->di_flags & DI_FLAGS_RELOAD) == 0)
	{
	    ಆದರೆ ((flags & (ನಿಯೋಜಿಸು_CONST | ನಿಯೋಜಿಸು_FINAL))
					 && (flags & ನಿಯೋಜಿಸು_FOR_LOOP) == 0)
	    {
		emsg(_(e_cannot_modify_existing_variable));
		ಹೋಗು failed;
	    }

	    ಆದರೆ (is_script_local && vim9script
			  && (flags & (ನಿಯೋಜಿಸು_NO_DECL | ನಿಯೋಜಿಸು_DECL)) == 0)
	    {
		semsg(_(e_redefining_script_ಅಂಶ_str), name);
		ಹೋಗು failed;
	    }

	    ಆದರೆ (check_typval_is_ಮೌಲ್ಯ(&di->di_tv) == FAIL)
		ಹೋಗು failed;

	    // List and Blob types can be modified in-place using the "+="
	    // compound operator.  For other types, this is not allowed.
	    ಪೂರ್ಣಾಂಕ type_inplace_modifiable =
		(di->di_tv.v_type == VAR_LIST || di->di_tv.v_type == VAR_BLOB);

	    ಆದರೆ (var_ಒಳ_vim9script && (flags & ನಿಯೋಜಿಸು_FOR_LOOP) == 0
		    && ((flags & ನಿಯೋಜಿಸು_COMPOUND_OP) == 0
			|| !type_inplace_modifiable))
	    {
		where_T where = WHERE_INIT;
		svar_T  *sv = ಕಂಡುಹಿಡಿ_typval_ಒಳ_script(&di->di_tv, sid, TRUE);

		ಆದರೆ (sv != ಖಾಲಿ)
		{
		    // check the type and adjust to bool if needed
		    ಆದರೆ (var_idx > 0)
		    {
			where.wt_ಸೂಚ್ಯಂಕ = var_idx;
			where.wt_ಬಗೆ = WT_VARIABLE;
		    }
		    ಆದರೆ (check_script_var_type(sv, tv, name, where) == FAIL)
			ಹೋಗು failed;
		    ಆದರೆ (type == ಖಾಲಿ)
			type = sv->sv_type;
		    sv->sv_flags |= SVFLAG_ASSIGNED;
		}
	    }

	    // Modifying a final variable with a List value using the "+="
	    // operator is allowed.  For other types, it is not allowed.
	    ಆದರೆ ((((flags & ನಿಯೋಜಿಸು_FOR_LOOP) == 0 || (flags & ನಿಯೋಜಿಸು_DECL) == 0)
			&& ((flags & ನಿಯೋಜಿಸು_COMPOUND_OP) == 0
			    || !type_inplace_modifiable))
				 ? var_check_permission(di, name) == FAIL
				 : var_check_ro(di->di_flags, name, FALSE))
		ಹೋಗು failed;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // can only redefine once
	    di->di_flags &= ~DI_FLAGS_RELOAD;

	    // A Vim9 script-local variable is also present in sn_all_vars
	    // and sn_var_vals.  It may set "type" from "tv".
	    ಆದರೆ (var_ಒಳ_vim9script || var_ಒಳ_autoload)
		update_vim9_script_var(FALSE, di,
			var_ಒಳ_autoload ? name : di->di_ಕೀಲಿ, flags,
			tv, &type, (flags & ನಿಯೋಜಿಸು_NO_MEMBER_TYPE) == 0);
	}

	// existing variable, need to clear the value

	// In Vim9 script a String v: variable only takes a String.
	ಆದರೆ (ht == &vimvarht && di->di_tv.v_type == VAR_STRING
							    && ಒಳ_vim9script())
	{
	    where_T	where = WHERE_INIT;

	    ಆದರೆ (check_typval_type(&t_string, tv, where) == FAIL)
		ಹೋಗು failed;
	}

	// Handle setting internal v: variables separately where needed to
	// prevent changing the type.
	ಪೂರ್ಣಾಂಕ type_error = FALSE;
	ಆದರೆ (ht == &vimvarht
		&& !before_ಗಣ_vvar(varname, di, tv, copy, &type_error))
	{
	    ಆದರೆ (type_error)
		semsg(_(e_setting_v_str_ಗಮ್ಯ_ಮೌಲ್ಯ_with_wrong_type), varname);
	    ಹೋಗು failed;
	}

	clear_tv(&di->di_tv);

	ಆದರೆ ((flags & ನಿಯೋಜಿಸು_UPDATE_ಖಂಡ_ID)
				       && SCRIPT_ID_VALID(current_sctx.sc_sid))
	{
	    scriptitem_T *si = SCRIPT_ಅಂಶ(current_sctx.sc_sid);

	    update_script_var_ಖಂಡ_id(name, si->sn_current_ಖಂಡ_id);
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Item not found, check if a function already exists.
	ಆದರೆ (is_script_local && (flags & (ನಿಯೋಜಿಸು_NO_DECL | ನಿಯೋಜಿಸು_DECL)) == 0
	       && lookup_scriptitem(name, STRLEN(name), FALSE, ಖಾಲಿ) == OK)
	{
	    semsg(_(e_redefining_script_ಅಂಶ_str), name);
	    ಹೋಗು failed;
	}

	// add a new variable
	ಆದರೆ (var_ಒಳ_vim9script && (flags & ನಿಯೋಜಿಸು_NO_DECL))
	{
	    semsg(_(e_unknown_variable_str), name);
	    ಹೋಗು failed;
	}

	ಆದರೆ (check_hashtab_frozen(ht, "add variable"))
	    ಹೋಗು failed;

	// Can't add "v:" or "a:" variable.
	ಆದರೆ (ht == &vimvarht || ht == get_funccal_ವಾದಗಳು_ht())
	{
	    semsg(_(e_illegal_variable_name_str), name);
	    ಹೋಗು failed;
	}

	// Make sure the variable name is valid.  In Vim9 script an
	// autoload variable must be prefixed with "g:" unless in an
	// autoload script.
	ಆದರೆ (!valid_varname(varname, -1, !vim9script
			|| STRNCMP(name, "g:", 2) == 0 || var_ಒಳ_autoload))
	    ಹೋಗು failed;

	di = alloc(offsetof(dictitem_T, di_ಕೀಲಿ) + STRLEN(varname) + 1);
	ಆದರೆ (di == ಖಾಲಿ)
	    ಹೋಗು failed;
	STRCPY(di->di_ಕೀಲಿ, varname);
	ಆದರೆ (hash_ಕೂಡಿಸು(ht, DI2HIKEY(di), "add variable") == FAIL)
	{
	    vim_free(di);
	    ಹೋಗು failed;
	}
	di->di_flags = DI_FLAGS_ALLOC;
	ಆದರೆ (flags & (ನಿಯೋಜಿಸು_CONST | ನಿಯೋಜಿಸು_FINAL))
	    di->di_flags |= DI_FLAGS_ಬೀಗ;

	// A Vim9 script-local variable is also added to sn_all_vars and
	// sn_var_vals. It may set "type" from "tv".
	ಆದರೆ (var_ಒಳ_vim9script || var_ಒಳ_autoload)
	    update_vim9_script_var(TRUE, di,
		    var_ಒಳ_autoload ? name : di->di_ಕೀಲಿ, flags,
			  tv, &type, (flags & ನಿಯೋಜಿಸು_NO_MEMBER_TYPE) == 0);
    }

    dest_tv = &di->di_tv;
    ಆದರೆ (copy || tv->v_type == VAR_ಸಂಖ್ಯೆ || tv->v_type == VAR_FLOAT)
	copy_tv(tv, dest_tv);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	*dest_tv = *tv;
	dest_tv->v_ಬೀಗ = 0;
	init_tv(tv);
    }
    free_tv_arg = FALSE;

    ಆದರೆ (vim9script && type != ಖಾಲಿ)
	ಗಣ_tv_type(dest_tv, type);

    // ":const var = value" locks the value
    // ":final var = value" locks "var"
    ಆದರೆ (flags & ನಿಯೋಜಿಸು_CONST)
	// Like :lockvar! name: lock the value and what it contains, but only
	// if the reference count is up to one.  That locks only literal
	// values.
	ಅಂಶ_ಬೀಗ(dest_tv, DICT_MAXNEST, TRUE, TRUE);

    rc = OK;

failed:
    vim_free(name_tofree);
    ಆದರೆ (free_tv_arg)
	clear_tv(tv_arg);

    ಹಿಂತಿರುಗಿ rc;
}

/*
 * Check in this order for backwards compatibility:
 * - Whether the variable is read-only
 * - Whether the variable value is locked
 * - Whether the variable is locked
 * NOTE: "name" is only used for error messages.
 */
    ಪೂರ್ಣಾಂಕ
var_check_permission(dictitem_T *di, char_u *name)
{
    ಆದರೆ (var_check_ro(di->di_flags, name, FALSE)
		    || ಮೌಲ್ಯ_check_ಬೀಗ(di->di_tv.v_ಬೀಗ, name, FALSE)
		    || var_check_ಬೀಗ(di->di_flags, name, FALSE))
	ಹಿಂತಿರುಗಿ FAIL;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Return TRUE if di_flags "flags" indicates variable "name" is read-only.
 * Also give an error message.
 */
    ಪೂರ್ಣಾಂಕ
var_check_ro(ಪೂರ್ಣಾಂಕ flags, char_u *name, ಪೂರ್ಣಾಂಕ use_gettext)
{
    ಆದರೆ (flags & DI_FLAGS_RO)
    {
	ಆದರೆ (name == ಖಾಲಿ)
	    emsg(_(e_cannot_change_readonly_variable));
	ಇಲ್ಲದಿದ್ದರೆ
	    semsg(_(e_cannot_change_readonly_variable_str),
				       use_gettext ? (char_u *)_(name) : name);
	ಹಿಂತಿರುಗಿ TRUE;
    }
    ಆದರೆ ((flags & DI_FLAGS_RO_SBX) && sandbox)
    {
	ಆದರೆ (name == ಖಾಲಿ)
	    emsg(_(e_cannot_ಗಣ_variable_ಒಳ_sandbox));
	ಇಲ್ಲದಿದ್ದರೆ
	    semsg(_(e_cannot_ಗಣ_variable_ಒಳ_sandbox_str),
				       use_gettext ? (char_u *)_(name) : name);
	ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Return TRUE if di_flags "flags" indicates variable "name" is locked.
 * Also give an error message.
 */
    ಪೂರ್ಣಾಂಕ
var_check_ಬೀಗ(ಪೂರ್ಣಾಂಕ flags, char_u *name, ಪೂರ್ಣಾಂಕ use_gettext)
{
    ಆದರೆ (flags & DI_FLAGS_ಬೀಗ)
    {
	semsg(_(e_variable_is_locked_str),
				       use_gettext ? (char_u *)_(name) : name);
	ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Return TRUE if di_flags "flags" indicates variable "name" is fixed.
 * Also give an error message.
 */
    ಪೂರ್ಣಾಂಕ
var_check_fixed(ಪೂರ್ಣಾಂಕ flags, char_u *name, ಪೂರ್ಣಾಂಕ use_gettext)
{
    ಆದರೆ (flags & DI_FLAGS_FIX)
    {
	ಆದರೆ (name == ಖಾಲಿ)
	    emsg(_(e_cannot_delete_variable));
	ಇಲ್ಲದಿದ್ದರೆ
	    semsg(_(e_cannot_delete_variable_str),
				      use_gettext ? (char_u *)_(name) : name);
	ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Check if a funcref is assigned to a valid variable name.
 * Return TRUE and give an error if not.
 */
    ಪೂರ್ಣಾಂಕ
var_wrong_func_name(
    char_u *name,    // points to start of variable name
    ಪೂರ್ಣಾಂಕ    new_var)  // TRUE when creating the variable
{
    // Allow for w: b: s: and t:.  In Vim9 script s: is not allowed, because
    // the name can be used without the s: prefix.
    // Allow autoload variable.
    ಆದರೆ (!((vim_strchr((char_u *)"wbt", name[0]) != ಖಾಲಿ
		    || (!ಒಳ_vim9script() && name[0] == 's')) && name[1] == ':')
	    && !ASCII_ISUPPER((name[0] != NUL && name[1] == ':')
						     ? name[2] : name[0])
	    && vim_strchr(name, '#') == ಖಾಲಿ)
    {
	semsg(_(e_funcref_variable_name_must_ಆರಂಭ_with_capital_str), name);
	ಹಿಂತಿರುಗಿ TRUE;
    }
    // Don't allow hiding a function.  When "v" is not NULL we might be
    // assigning another function to the same var, the type is checked
    // below.
    ಆದರೆ (new_var && function_exists(name, FALSE))
    {
	semsg(_(e_variable_name_conflicts_with_existing_function_str),
								    name);
	ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Return TRUE if "flags" indicates variable "name" has a locked (immutable)
 * value.  Also give an error message, using "name" or _("name") when
 * "use_gettext" is TRUE.
 */
    ಪೂರ್ಣಾಂಕ
ಮೌಲ್ಯ_check_ಬೀಗ(ಪೂರ್ಣಾಂಕ ಬೀಗ, char_u *name, ಪೂರ್ಣಾಂಕ use_gettext)
{
    ಆದರೆ (ಬೀಗ & VAR_LOCKED)
    {
	ಆದರೆ (name == ಖಾಲಿ)
	    emsg(_(e_ಮೌಲ್ಯ_is_locked));
	ಇಲ್ಲದಿದ್ದರೆ
	    semsg(_(e_ಮೌಲ್ಯ_is_locked_str),
				       use_gettext ? (char_u *)_(name) : name);
	ಹಿಂತಿರುಗಿ TRUE;
    }
    ಆದರೆ (ಬೀಗ & VAR_FIXED)
    {
	ಆದರೆ (name == ಖಾಲಿ)
	    emsg(_(e_cannot_change_ಮೌಲ್ಯ));
	ಇಲ್ಲದಿದ್ದರೆ
	    semsg(_(e_cannot_change_ಮೌಲ್ಯ_ದ_str),
				       use_gettext ? (char_u *)_(name) : name);
	ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Check if a variable name is valid.  When "autoload" is true "#" is allowed.
 * If "len" is -1 use all of "varname", otherwise up to "varname[len]".
 * Return FALSE and give an error if not.
 */
    ಪೂರ್ಣಾಂಕ
valid_varname(char_u *varname, ಪೂರ್ಣಾಂಕ len, ಪೂರ್ಣಾಂಕ autoload)
{
    char_u *p;

    ಗಾಗಿ (p = varname; len < 0 ? *p != NUL : p < varname + len; ++p)
	ಆದರೆ (!eval_isnamec1(*p) && (p == varname || !VIM_ISDIGIT(*p))
					 && !(autoload && *p == AUTOLOAD_CHAR))
	{
	    semsg(_(e_illegal_variable_name_str), varname);
	    ಹಿಂತಿರುಗಿ FALSE;
	}
    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Implements the logic to retrieve local variable and option values.
 * Used by "getwinvar()" "gettabvar()" "gettabwinvar()" "getbufvar()".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
get_var_from(
    char_u	*varname,
    typval_T	*rettv,
    typval_T	*deftv,	    // Default value if not found.
    ಪೂರ್ಣಾಂಕ		htname,	    // 't'ab, 'w'indow or 'b'uffer local.
    tabpage_T	*tp,	    // can be NULL
    win_T	*win,
    buf_T	*buf)	    // Ignored if htname is not 'b'.
{
    dictitem_T	*v;
    ಪೂರ್ಣಾಂಕ		ಮುಗಿದ = FALSE;
    switchwin_T	switchwin;
    ಪೂರ್ಣಾಂಕ		need_switch_win;
    ಪೂರ್ಣಾಂಕ		do_change_curbuf = buf != ಖಾಲಿ && htname == 'b';

    ++emsg_off;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ಖಾಲಿ;

    ಆದರೆ (varname != ಖಾಲಿ && tp != ಖಾಲಿ && win != ಖಾಲಿ
	    && (htname != 'b' || buf != ಖಾಲಿ))
    {
	// Set curwin to be our win, temporarily.  Also set the tabpage,
	// otherwise the window is not valid. Only do this when needed,
	// autocommands get blocked.
	// If we have a buffer reference avoid the switching, we're saving and
	// restoring curbuf directly.
	need_switch_win = !(tp == curtab && win == curwin) && !do_change_curbuf;
	ಆದರೆ (!need_switch_win || switch_win(&switchwin, win, tp, TRUE) == OK)
	{
	    // Handle options. There are no tab-local options.
	    ಆದರೆ (*varname == '&' && htname != 't')
	    {
		buf_T	*save_curbuf = curbuf;

		// Change curbuf so the option is read from the correct buffer.
		ಆದರೆ (do_change_curbuf)
		    curbuf = buf;

		ಆದರೆ (varname[1] == NUL)
		{
		    // get all window-local or buffer-local options in a dict
		    dict_T	*opts = get_winbuf_options(htname == 'b');

		    ಆದರೆ (opts != ಖಾಲಿ)
		    {
			rettv_dict_ಗಣ(rettv, opts);
			ಮುಗಿದ = TRUE;
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (eval_option(&varname, rettv, TRUE) == OK)
		    // Local option
		    ಮುಗಿದ = TRUE;

		curbuf = save_curbuf;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*varname == NUL)
	    {
		// Empty string: return a dict with all the local variables.
		ಆದರೆ (htname == 'b')
		    v = &buf->b_bufvar;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (htname == 'w')
		    v = &win->w_winvar;
		ಇಲ್ಲದಿದ್ದರೆ
		    v = &tp->tp_winvar;
		copy_tv(&v->di_tv, rettv);
		ಮುಗಿದ = TRUE;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		hashtab_T	*ht;

		ಆದರೆ (htname == 'b')
		    ht = &buf->b_vars->dv_hashtab;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (htname == 'w')
		    ht = &win->w_vars->dv_hashtab;
		ಇಲ್ಲದಿದ್ದರೆ
		    ht = &tp->tp_vars->dv_hashtab;

		// Look up the variable.
		v = ಕಂಡುಹಿಡಿ_var_ಒಳ_ht(ht, htname, varname, FALSE);
		ಆದರೆ (v != ಖಾಲಿ)
		{
		    copy_tv(&v->di_tv, rettv);
		    ಮುಗಿದ = TRUE;
		}
	    }
	}

	ಆದರೆ (need_switch_win)
	    // restore previous notion of curwin
	    restore_win(&switchwin, TRUE);
    }

    ಆದರೆ (!ಮುಗಿದ && deftv->v_type != VAR_UNKNOWN)
	// use the default value
	copy_tv(deftv, rettv);

    --emsg_off;
}

/*
 * getwinvar() and gettabwinvar()
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
getwinvar(
    typval_T	*argvars,
    typval_T	*rettv,
    ಪೂರ್ಣಾಂಕ		off)	    // 1 for gettabwinvar()
{
    char_u	*varname;
    tabpage_T	*tp;
    win_T	*win;

    ಆದರೆ (off == 1)
	tp = ಕಂಡುಹಿಡಿ_tabpage((ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[0], ಖಾಲಿ));
    ಇಲ್ಲದಿದ್ದರೆ
	tp = curtab;
    win = ಕಂಡುಹಿಡಿ_win_by_nr(&argvars[off], tp);
    varname = tv_get_string_chk(&argvars[off + 1]);

    get_var_from(varname, rettv, &argvars[off + 2], 'w', tp, win, ಖಾಲಿ);
}

/*
 * Set option "varname" to the value of "varp" for the current buffer/window.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_option_from_tv(char_u *varname, typval_T *varp)
{
    ದೀರ್ಘ	numval = 0;
    char_u	*strval;
    char_u	nbuf[NUMBUFLEN];
    ಪೂರ್ಣಾಂಕ		error = FALSE;

    ಪೂರ್ಣಾಂಕ		opt_idx = findoption(varname);
    ಆದರೆ (opt_idx < 0)
    {
	semsg(_(e_unknown_option_str_2), varname);
	ಹಿಂತಿರುಗಿ;
    }
    ಪೂರ್ಣಾಂಕ		opt_p_flags = get_option_flags(opt_idx);

    ಆದರೆ (varp->v_type == VAR_BOOL)
    {
	ಆದರೆ (opt_p_flags & P_STRING)
	{
	    emsg(_(e_string_required));
	    ಹಿಂತಿರುಗಿ;
	}
	numval = (ದೀರ್ಘ)varp->vval.v_ಸಂಖ್ಯೆ;
	strval = (char_u *)"0";  // avoid using "false"
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ ((opt_p_flags & (P_ಅಂಕೆ|P_BOOL))
		&& (!ಒಳ_vim9script() || varp->v_type != VAR_STRING))
	    numval = (ದೀರ್ಘ)tv_get_ಸಂಖ್ಯೆ_chk(varp, &error);
	ಆದರೆ (!error)
	    strval = tv_get_string_buf_chk(varp, nbuf);
    }
    ಆದರೆ (!error && strval != ಖಾಲಿ)
	ಗಣ_option_ಮೌಲ್ಯ_give_err(varname, numval, strval, OPT_LOCAL);
}

/*
 * "setwinvar()" and "settabwinvar()" functions
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
setwinvar(typval_T *argvars, ಪೂರ್ಣಾಂಕ off)
{
    win_T	*win;
    switchwin_T	switchwin;
    ಪೂರ್ಣಾಂಕ		need_switch_win;
    char_u	*varname, *winvarname;
    typval_T	*varp;
    tabpage_T	*tp = ಖಾಲಿ;

    ಆದರೆ (check_secure())
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (off == 1)
	tp = ಕಂಡುಹಿಡಿ_tabpage((ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[0], ಖಾಲಿ));
    ಇಲ್ಲದಿದ್ದರೆ
	tp = curtab;
    win = ಕಂಡುಹಿಡಿ_win_by_nr(&argvars[off], tp);
    varname = tv_get_string_chk(&argvars[off + 1]);
    varp = &argvars[off + 2];

    ಆದರೆ (win == ಖಾಲಿ || varname == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    need_switch_win = !(tp == curtab && win == curwin);
    ಆದರೆ (!need_switch_win
	    || switch_win(&switchwin, win, tp, TRUE) == OK)
    {
	ಆದರೆ (*varname == '&')
	    ಗಣ_option_from_tv(varname + 1, varp);
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    winvarname = alloc(STRLEN(varname) + 3);
	    ಆದರೆ (winvarname != ಖಾಲಿ)
	    {
		STRCPY(winvarname, "w:");
		STRCPY(winvarname + 2, varname);
		ಗಣ_var(winvarname, varp, TRUE);
		vim_free(winvarname);
	    }
	}
    }
    ಆದರೆ (need_switch_win)
	restore_win(&switchwin, TRUE);
}

/*
 * reset v:option_new, v:option_old, v:option_oldlocal, v:option_oldglobal,
 * v:option_type, and v:option_command.
 */
    ಶೂನ್ಯ
reset_v_option_vars(ಶೂನ್ಯ)
{
    ಗಣ_vim_var_string(VV_OPTION_NEW, ಖಾಲಿ, -1);
    ಗಣ_vim_var_string(VV_OPTION_OLD, ಖಾಲಿ, -1);
    ಗಣ_vim_var_string(VV_OPTION_OLDLOCAL, ಖಾಲಿ, -1);
    ಗಣ_vim_var_string(VV_OPTION_OLDGLOBAL, ಖಾಲಿ, -1);
    ಗಣ_vim_var_string(VV_OPTION_TYPE, ಖಾಲಿ, -1);
    ಗಣ_vim_var_string(VV_OPTION_COMMAND, ಖಾಲಿ, -1);
}

/*
 * Add an assert error to v:errors.
 */
    ಶೂನ್ಯ
assert_error(garray_T *gap)
{
    ರಚನೆ vimvar   *vp = &vimvars[VV_ERRORS];

    ಆದರೆ (vp->vv_tv_type != VAR_LIST || vimvars[VV_ERRORS].vv_list == ಖಾಲಿ)
	// Make sure v:errors is a list.
	ಗಣ_vim_var_list(VV_ERRORS, list_alloc());
    list_append_string(vimvars[VV_ERRORS].vv_list, gap->ga_ದತ್ತಾಂಶ, gap->ga_len);
}

    ಪೂರ್ಣಾಂಕ
var_exists(char_u *var)
{
    char_u	*arg = var;
    char_u	*name;
    char_u	*tofree;
    typval_T    tv;
    ಪೂರ್ಣಾಂಕ		len = 0;
    ಪೂರ್ಣಾಂಕ		n = FALSE;

    // get_name_len() takes care of expanding curly braces
    name = var;
    len = get_name_len(&arg, &tofree, TRUE, FALSE);
    ಆದರೆ (len > 0)
    {
	ಆದರೆ (tofree != ಖಾಲಿ)
	    name = tofree;
	n = (eval_variable(name, len, 0, &tv, ಖಾಲಿ,
				 EVAL_VAR_NOAUTOLOAD + EVAL_VAR_IMPORT) == OK);
	ಆದರೆ (n)
	{
	    // handle d.key, l[idx], f(expr)
	    arg = skipwhite(arg);
	    n = (handle_subscript(&arg, name, &tv, &EVALARG_EVALUATE,
								 FALSE) == OK);
	    ಆದರೆ (n)
		clear_tv(&tv);
	}
    }
    ಆದರೆ (*arg != NUL)
	n = FALSE;

    vim_free(tofree);
    ಹಿಂತಿರುಗಿ n;
}

ಸ್ಥಿರ lval_T	*redir_lval = ಖಾಲಿ;
#ವ್ಯಾಖ್ಯಾನಿಸು EVALCMD_BUSY (redir_lval == (lval_T *)&redir_lval)
ಸ್ಥಿರ garray_T redir_ga;	// only valid when redir_lval is not NULL
ಸ್ಥಿರ char_u	*redir_endp = ಖಾಲಿ;
ಸ್ಥಿರ char_u	*redir_varname = ಖಾಲಿ;

    ಪೂರ್ಣಾಂಕ
alloc_redir_lval(ಶೂನ್ಯ)
{
    redir_lval = ALLOC_CLEAR_ONE(lval_T);
    ಆದರೆ (redir_lval == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    ಹಿಂತಿರುಗಿ OK;
}

    ಶೂನ್ಯ
clear_redir_lval(ಶೂನ್ಯ)
{
    VIM_CLEAR(redir_lval);
}

    ಶೂನ್ಯ
init_redir_ga(ಶೂನ್ಯ)
{
    ga_init2(&redir_ga, ಗಾತ್ರ(ಅಕ್ಷರ), 500);
}

/*
 * Start recording command output to a variable
 * When "append" is TRUE append to an existing variable.
 * Returns OK if successfully completed the setup.  FAIL otherwise.
 */
    ಪೂರ್ಣಾಂಕ
var_redir_ಆರಂಭ(char_u *name, ಪೂರ್ಣಾಂಕ append)
{
    ಪೂರ್ಣಾಂಕ		called_emsg_before;
    typval_T	tv;

    // Catch a bad name early.
    ಆದರೆ (!eval_isnamec1(*name))
    {
	emsg(_(e_invalid_ವಾದ));
	ಹಿಂತಿರುಗಿ FAIL;
    }

    // Make a copy of the name, it is used in redir_lval until redir ends.
    redir_varname = vim_strsave(name);
    ಆದರೆ (redir_varname == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (alloc_redir_lval() == FAIL)
    {
	var_redir_stop();
	ಹಿಂತಿರುಗಿ FAIL;
    }

    // The output is stored in growarray "redir_ga" until redirection ends.
    init_redir_ga();

    // Parse the variable name (can be a dict or list entry).
    redir_endp = get_lval(redir_varname, ಖಾಲಿ, redir_lval, FALSE, FALSE, 0,
							     FNE_CHECK_ಆರಂಭ);
    ಆದರೆ (redir_endp == ಖಾಲಿ || redir_lval->ll_name == ಖಾಲಿ || *redir_endp != NUL)
    {
	clear_lval(redir_lval);
	ಆದರೆ (redir_endp != ಖಾಲಿ && *redir_endp != NUL)
	    // Trailing characters are present after the variable name
	    semsg(_(e_trailing_characters_str), redir_endp);
	ಇಲ್ಲದಿದ್ದರೆ
	    semsg(_(e_invalid_ವಾದ_str), name);
	redir_endp = ಖಾಲಿ;  // don't store a value, only cleanup
	var_redir_stop();
	ಹಿಂತಿರುಗಿ FAIL;
    }

    // check if we can write to the variable: set it to or append an empty
    // string
    called_emsg_before = called_emsg;
    tv.v_type = VAR_STRING;
    tv.vval.v_string = (char_u *)"";
    ಆದರೆ (append)
	ಗಣ_var_lval(redir_lval, redir_endp, &tv, TRUE,
					     ನಿಯೋಜಿಸು_NO_DECL, (char_u *)".", 0);
    ಇಲ್ಲದಿದ್ದರೆ
	ಗಣ_var_lval(redir_lval, redir_endp, &tv, TRUE,
					     ನಿಯೋಜಿಸು_NO_DECL, (char_u *)"=", 0);
    clear_lval(redir_lval);
    ಆದರೆ (called_emsg > called_emsg_before)
    {
	redir_endp = ಖಾಲಿ;  // don't store a value, only cleanup
	var_redir_stop();
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಹಿಂತಿರುಗಿ OK;
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
    ಶೂನ್ಯ
var_redir_str(char_u *ಮೌಲ್ಯ, ಪೂರ್ಣಾಂಕ ಮೌಲ್ಯ_len)
{
    ಪೂರ್ಣಾಂಕ		len;

    ಆದರೆ (redir_lval == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಮೌಲ್ಯ_len == -1)
	len = (ಪೂರ್ಣಾಂಕ)STRLEN(ಮೌಲ್ಯ);	// Append the entire string
    ಇಲ್ಲದಿದ್ದರೆ
	len = ಮೌಲ್ಯ_len;		// Append only "value_len" characters

    ಆದರೆ (ga_grow(&redir_ga, len) == OK)
    {
	mch_memmove((ಅಕ್ಷರ *)redir_ga.ga_ದತ್ತಾಂಶ + redir_ga.ga_len, ಮೌಲ್ಯ, len);
	redir_ga.ga_len += len;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	var_redir_stop();
}

/*
 * Stop redirecting command output to a variable.
 * Frees the allocated memory.
 */
    ಶೂನ್ಯ
var_redir_stop(ಶೂನ್ಯ)
{
    typval_T	tv;

    ಆದರೆ (EVALCMD_BUSY)
    {
	redir_lval = ಖಾಲಿ;
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (redir_lval != ಖಾಲಿ)
    {
	// If there was no error: assign the text to the variable.
	ಆದರೆ (redir_endp != ಖಾಲಿ)
	{
	    ga_append(&redir_ga, NUL);  // Append the trailing NUL.
	    tv.v_type = VAR_STRING;
	    tv.vval.v_string = redir_ga.ga_ದತ್ತಾಂಶ;
	    // Call get_lval() again, if it's inside a Dict or List it may
	    // have changed.
	    redir_endp = get_lval(redir_varname, ಖಾಲಿ, redir_lval,
					FALSE, FALSE, 0, FNE_CHECK_ಆರಂಭ);
	    ಆದರೆ (redir_endp != ಖಾಲಿ && redir_lval->ll_name != ಖಾಲಿ)
		ಗಣ_var_lval(redir_lval, redir_endp, &tv, FALSE, 0,
							     (char_u *)".", 0);
	    clear_lval(redir_lval);
	}

	// free the collected output
	VIM_CLEAR(redir_ga.ga_ದತ್ತಾಂಶ);

	VIM_CLEAR(redir_lval);
    }
    VIM_CLEAR(redir_varname);
}

/*
 * Get the collected redirected text and clear redir_ga.
 */
    char_u *
get_clear_redir_ga(ಶೂನ್ಯ)
{
    char_u *res;

    ga_append(&redir_ga, NUL);  // Append the trailing NUL.
    res = redir_ga.ga_ದತ್ತಾಂಶ;
    redir_ga.ga_ದತ್ತಾಂಶ = ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ res;
}

/*
 * "gettabvar()" function
 */
    ಶೂನ್ಯ
f_gettabvar(typval_T *argvars, typval_T *rettv)
{
    char_u	*varname;
    tabpage_T	*tp;
    win_T	*win = ಖಾಲಿ;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    varname = tv_get_string_chk(&argvars[1]);
    tp = ಕಂಡುಹಿಡಿ_tabpage((ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[0], ಖಾಲಿ));
    ಆದರೆ (tp != ಖಾಲಿ)
	win = tp == curtab || tp->tp_firstwin == ಖಾಲಿ ? firstwin
		: tp->tp_firstwin;

    get_var_from(varname, rettv, &argvars[2], 't', tp, win, ಖಾಲಿ);
}

/*
 * "gettabwinvar()" function
 */
    ಶೂನ್ಯ
f_gettabwinvar(typval_T *argvars, typval_T *rettv)
{
    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL))
	ಹಿಂತಿರುಗಿ;

    getwinvar(argvars, rettv, 1);
}

/*
 * "getwinvar()" function
 */
    ಶೂನ್ಯ
f_getwinvar(typval_T *argvars, typval_T *rettv)
{
    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    getwinvar(argvars, rettv, 0);
}

/*
 * "getbufvar()" function
 */
    ಶೂನ್ಯ
f_getbufvar(typval_T *argvars, typval_T *rettv)
{
    char_u	*varname;
    buf_T	*buf;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_ಬಫರ್_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    varname = tv_get_string_chk(&argvars[1]);
    buf = tv_get_buf_from_arg(&argvars[0]);

    get_var_from(varname, rettv, &argvars[2], 'b', curtab, curwin, buf);
}

/*
 * "settabvar()" function
 */
    ಶೂನ್ಯ
f_settabvar(typval_T *argvars, typval_T *rettv ಬಳಸದ)
{
    tabpage_T	*save_curtab;
    tabpage_T	*tp;
    tabpage_T	*save_lu_tp;
    char_u	*varname, *tabvarname;
    typval_T	*varp;

    ಆದರೆ (check_secure())
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    tp = ಕಂಡುಹಿಡಿ_tabpage((ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[0], ಖಾಲಿ));
    varname = tv_get_string_chk(&argvars[1]);
    varp = &argvars[2];

    ಆದರೆ (varname == ಖಾಲಿ || tp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    save_curtab = curtab;
    save_lu_tp = lastused_tabpage;
    goto_tabpage_tp(tp, FALSE, FALSE);

    tabvarname = alloc(STRLEN(varname) + 3);
    ಆದರೆ (tabvarname != ಖಾಲಿ)
    {
	STRCPY(tabvarname, "t:");
	STRCPY(tabvarname + 2, varname);
	ಗಣ_var(tabvarname, varp, TRUE);
	vim_free(tabvarname);
    }

    // Restore current tabpage and last accessed tabpage.
    ಆದರೆ (valid_tabpage(save_curtab))
    {
	goto_tabpage_tp(save_curtab, FALSE, FALSE);
	ಆದರೆ (valid_tabpage(save_lu_tp))
	    lastused_tabpage = save_lu_tp;
    }
}

/*
 * "settabwinvar()" function
 */
    ಶೂನ್ಯ
f_settabwinvar(typval_T *argvars, typval_T *rettv ಬಳಸದ)
{
    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL))
	ಹಿಂತಿರುಗಿ;

    setwinvar(argvars, 1);
}

/*
 * "setwinvar()" function
 */
    ಶೂನ್ಯ
f_setwinvar(typval_T *argvars, typval_T *rettv ಬಳಸದ)
{
    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    setwinvar(argvars, 0);
}

/*
 * "setbufvar()" function
 */
    ಶೂನ್ಯ
f_setbufvar(typval_T *argvars, typval_T *rettv ಬಳಸದ)
{
    buf_T	*buf;
    char_u	*varname, *bufvarname;
    typval_T	*varp;

    ಆದರೆ (check_secure())
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_ಬಫರ್_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    varname = tv_get_string_chk(&argvars[1]);
    buf = tv_get_buf_from_arg(&argvars[0]);
    varp = &argvars[2];

    ಆದರೆ (buf == ಖಾಲಿ || varname == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (*varname == '&')
    {
	aco_save_T	aco;
	// safe the current window position, it could
	// change because of 'scrollbind' window-local
	// options
	linenr_T	old_topline = curwin->w_topline;

	// Set curbuf to be our buf, temporarily.
	aucmd_prepbuf(&aco, buf);
	ಆದರೆ (curbuf == buf)
	{
	    // Only when it worked to set "curbuf".
	    ಗಣ_option_from_tv(varname + 1, varp);

	    // reset notion of buffer
	    aucmd_restbuf(&aco);
	}
	curwin->w_topline = old_topline;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	bufvarname = alloc(STRLEN(varname) + 3);
	ಆದರೆ (bufvarname != ಖಾಲಿ)
	{
	    buf_T *save_curbuf = curbuf;

	    curbuf = buf;
	    STRCPY(bufvarname, "b:");
	    STRCPY(bufvarname + 2, varname);
	    ಗಣ_var(bufvarname, varp, TRUE);
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
    ಪೂರ್ಣಾಂಕ		r = OK;

    CLEAR_ಕ್ಷೇತ್ರ(res);
    ಆದರೆ (arg->v_type == VAR_PARTIAL && arg->vval.v_partial != ಖಾಲಿ)
    {
	res.cb_partial = arg->vval.v_partial;
	++res.cb_partial->pt_refcount;
	res.cb_name = partial_name(res.cb_partial);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (arg->v_type == VAR_STRING && arg->vval.v_string != ಖಾಲಿ
					       && ಸುರಕ್ಷಿತ_isdigit(*arg->vval.v_string))
	    r = FAIL;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (arg->v_type == VAR_FUNC || arg->v_type == VAR_STRING)
	{
	    res.cb_name = arg->vval.v_string;
	    ಆದರೆ (arg->v_type == VAR_STRING)
	    {
		char_u *name = get_scriptlocal_funcname(arg->vval.v_string);
		ಆದರೆ (name != ಖಾಲಿ)
		{
		    res.cb_name = name;
		    res.cb_free_name = TRUE;
		}
	    }
	    func_ref(res.cb_name);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (arg->v_type == VAR_ಸಂಖ್ಯೆ && arg->vval.v_ಸಂಖ್ಯೆ == 0)
	    res.cb_name = (char_u *)"";
	ಇಲ್ಲದಿದ್ದರೆ
	    r = FAIL;

	ಆದರೆ (r == FAIL)
	{
	    emsg(_(e_invalid_callback_ವಾದ));
	    res.cb_name = ಖಾಲಿ;
	}
    }
    ಹಿಂತಿರುಗಿ res;
}

/*
 * Copy a callback into a typval_T.
 */
    ಶೂನ್ಯ
put_callback(callback_T *cb, typval_T *tv)
{
    ಆದರೆ (cb->cb_partial != ಖಾಲಿ)
    {
	tv->v_type = VAR_PARTIAL;
	tv->vval.v_partial = cb->cb_partial;
	++tv->vval.v_partial->pt_refcount;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	tv->v_type = VAR_FUNC;
	tv->vval.v_string = vim_strsave(cb->cb_name);
	func_ref(cb->cb_name);
    }
}

    ಸ್ಥಿರ bool
does_callback_own_cb_name(callback_T *cb)
{
    // If cb_partial != NULL then *cb->cb_name is owned by the partial.
    ಹಿಂತಿರುಗಿ cb->cb_partial || cb->cb_free_name;
}

/*
 * Make a copy of "src" into "dest", allocating the function name if needed,
 * without incrementing the refcount.
 */
    ಶೂನ್ಯ
ಗಣ_callback(callback_T *dest, callback_T *src)
{
    *dest = *src;
    ಆದರೆ (!does_callback_own_cb_name(src))
    {
	dest->cb_name = vim_strsave(src->cb_name);
	dest->cb_free_name = TRUE;
    }
    *src = (callback_T){0};
}

/*
 * Copy callback from "src" to "dest", incrementing the refcounts.
 */
    ಶೂನ್ಯ
copy_callback(callback_T *dest, callback_T *src)
{
    dest->cb_partial = src->cb_partial;
    ಆದರೆ (dest->cb_partial != ಖಾಲಿ)
    {
	dest->cb_name = src->cb_name;
	dest->cb_free_name = FALSE;
	++dest->cb_partial->pt_refcount;
    }
    ಇಲ್ಲದಿದ್ದರೆ
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
    ಶೂನ್ಯ
expand_autload_callback(callback_T *cb)
{
    char_u	*name;
    char_u	*p;
    imported_T	*import;

    ಆದರೆ (!ಒಳ_vim9script() || cb->cb_name == ಖಾಲಿ
	    || (!cb->cb_free_name
	       && (cb->cb_partial == ಖಾಲಿ || cb->cb_partial->pt_name == ಖಾಲಿ)))
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (cb->cb_partial != ಖಾಲಿ)
	name = cb->cb_partial->pt_name;
    ಇಲ್ಲದಿದ್ದರೆ
	name = cb->cb_name;
    p = vim_strchr(name, '.');
    ಆದರೆ (p == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    import = ಕಂಡುಹಿಡಿ_imported(name, p - name, FALSE);
    ಆದರೆ (import == ಖಾಲಿ || !SCRIPT_ID_VALID(import->imp_sid))
	ಹಿಂತಿರುಗಿ;

    scriptitem_T *si = SCRIPT_ಅಂಶ(import->imp_sid);
    ಆದರೆ (si->sn_autoload_ಪೂರ್ವಪ್ರತ್ಯಯ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    char_u *newname = concat_str(si->sn_autoload_ಪೂರ್ವಪ್ರತ್ಯಯ, p + 1);
    ಆದರೆ (newname == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (cb->cb_partial != ಖಾಲಿ)
    {
	ಆದರೆ (cb->cb_name == cb->cb_partial->pt_name)
	    cb->cb_name = newname;
	vim_free(cb->cb_partial->pt_name);
	cb->cb_partial->pt_name = newname;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	vim_free(cb->cb_name);
	cb->cb_name = newname;
    }
}

/*
 * Unref/free "callback" returned by get_callback() or set_callback().
 */
    ಶೂನ್ಯ
free_callback(callback_T *callback)
{
    ಆದರೆ (callback->cb_partial != ಖಾಲಿ)
    {
	partial_unref(callback->cb_partial);
	callback->cb_partial = ಖಾಲಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (callback->cb_name != ಖಾಲಿ)
	func_unref(callback->cb_name);
    ಆದರೆ (callback->cb_free_name)
    {
	vim_free(callback->cb_name);
	callback->cb_free_name = FALSE;
    }
    callback->cb_name = ಖಾಲಿ;
}

#ಆದರೆ_ಅಂತ್ಯ // FEAT_EVAL
