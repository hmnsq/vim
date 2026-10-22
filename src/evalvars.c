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

#ಯದಿ defined(FEAT_EVAL)

ಸ್ಥಿರ dictitem_T	globvars_var;		// variable used for g:
ಸ್ಥಿರ dict_T		globvardict;		// Dictionary with g: variables
#define globvarht globvardict.dv_hashtab

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
#define VV_COMPAT	1	// compatible, also used without "v:"
#define VV_RO		2	// read-only
#define VV_RO_SBX	4	// read-only in the sandbox

#define VV_NAME(s, t)	s, {{t, 0, {0}}, 0, {s}}

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ vimvar vimvar_T;

ಸ್ಥಿರ ಸಂರಚನಾ vimvar
{
    ವರ್ಣ	*vv_name;	// name of variable, without v:
    dictitem16_T vv_di;		// value and name for key (max 16 chars!)
    type_T	*vv_type;	// type or NULL
    ವರ್ಣ	vv_flags;	// VV_COMPAT, VV_RO, VV_RO_SBX
} vimvars[VV_LEN] =
{
    // The order here must match the VV_ defines in vim.h!
    // Initializing a union does not work, leave tv.vval empty to get zero's.
    {VV_NAME("count",		 VAR_ಸಂಖ್ಯೆ), ರಿಕ್ತ, VV_COMPAT+VV_RO},
    {VV_NAME("count1",		 VAR_ಸಂಖ್ಯೆ), ರಿಕ್ತ, VV_RO},
    {VV_NAME("prevcount",	 VAR_ಸಂಖ್ಯೆ), ರಿಕ್ತ, VV_RO},
    {VV_NAME("errmsg",		 VAR_STRING), ರಿಕ್ತ, VV_COMPAT},
    {VV_NAME("warningmsg",	 VAR_STRING), ರಿಕ್ತ, 0},
    {VV_NAME("statusmsg",	 VAR_STRING), ರಿಕ್ತ, 0},
    {VV_NAME("shell_error",	 VAR_ಸಂಖ್ಯೆ), ರಿಕ್ತ, VV_COMPAT+VV_RO},
    {VV_NAME("this_session",	 VAR_STRING), ರಿಕ್ತ, VV_COMPAT},
    {VV_NAME("version",		 VAR_ಸಂಖ್ಯೆ), ರಿಕ್ತ, VV_COMPAT+VV_RO},
    {VV_NAME("lnum",		 VAR_ಸಂಖ್ಯೆ), ರಿಕ್ತ, VV_RO_SBX},
    {VV_NAME("termresponse",	 VAR_STRING), ರಿಕ್ತ, VV_RO},
    {VV_NAME("fname",		 VAR_STRING), ರಿಕ್ತ, VV_RO},
    {VV_NAME("lang",		 VAR_STRING), ರಿಕ್ತ, VV_RO},
    {VV_NAME("lc_time",		 VAR_STRING), ರಿಕ್ತ, VV_RO},
    {VV_NAME("ctype",		 VAR_STRING), ರಿಕ್ತ, VV_RO},
    {VV_NAME("charconvert_from", VAR_STRING), ರಿಕ್ತ, VV_RO},
    {VV_NAME("charconvert_to",	 VAR_STRING), ರಿಕ್ತ, VV_RO},
    {VV_NAME("fname_in",	 VAR_STRING), ರಿಕ್ತ, VV_RO},
    {VV_NAME("fname_out",	 VAR_STRING), ರಿಕ್ತ, VV_RO},
    {VV_NAME("fname_new",	 VAR_STRING), ರಿಕ್ತ, VV_RO},
    {VV_NAME("fname_diff",	 VAR_STRING), ರಿಕ್ತ, VV_RO},
    {VV_NAME("cmdarg",		 VAR_STRING), ರಿಕ್ತ, VV_RO},
    {VV_NAME("foldstart",	 VAR_ಸಂಖ್ಯೆ), ರಿಕ್ತ, VV_RO_SBX},
    {VV_NAME("foldend",		 VAR_ಸಂಖ್ಯೆ), ರಿಕ್ತ, VV_RO_SBX},
    {VV_NAME("folddashes",	 VAR_STRING), ರಿಕ್ತ, VV_RO_SBX},
    {VV_NAME("foldlevel",	 VAR_ಸಂಖ್ಯೆ), ರಿಕ್ತ, VV_RO_SBX},
    {VV_NAME("progname",	 VAR_STRING), ರಿಕ್ತ, VV_RO},
    {VV_NAME("servername",	 VAR_STRING), ರಿಕ್ತ, VV_RO},
    {VV_NAME("dying",		 VAR_ಸಂಖ್ಯೆ), ರಿಕ್ತ, VV_RO},
    {VV_NAME("exception",	 VAR_STRING), ರಿಕ್ತ, VV_RO},
    {VV_NAME("throwpoint",	 VAR_STRING), ರಿಕ್ತ, VV_RO},
    {VV_NAME("register",	 VAR_STRING), ರಿಕ್ತ, VV_RO},
    {VV_NAME("cmdbang",		 VAR_ಸಂಖ್ಯೆ), ರಿಕ್ತ, VV_RO},
    {VV_NAME("insertmode",	 VAR_STRING), ರಿಕ್ತ, VV_RO},
    {VV_NAME("val",		 VAR_UNKNOWN), ರಿಕ್ತ, VV_RO},
    {VV_NAME("key",		 VAR_UNKNOWN), ರಿಕ್ತ, VV_RO},
    {VV_NAME("profiling",	 VAR_ಸಂಖ್ಯೆ), ರಿಕ್ತ, VV_RO},
    {VV_NAME("fcs_reason",	 VAR_STRING), ರಿಕ್ತ, VV_RO},
    {VV_NAME("fcs_choice",	 VAR_STRING), ರಿಕ್ತ, 0},
    {VV_NAME("beval_bufnr",	 VAR_ಸಂಖ್ಯೆ), ರಿಕ್ತ, VV_RO},
    {VV_NAME("beval_winnr",	 VAR_ಸಂಖ್ಯೆ), ರಿಕ್ತ, VV_RO},
    {VV_NAME("beval_winid",	 VAR_ಸಂಖ್ಯೆ), ರಿಕ್ತ, VV_RO},
    {VV_NAME("beval_lnum",	 VAR_ಸಂಖ್ಯೆ), ರಿಕ್ತ, VV_RO},
    {VV_NAME("beval_col",	 VAR_ಸಂಖ್ಯೆ), ರಿಕ್ತ, VV_RO},
    {VV_NAME("beval_text",	 VAR_STRING), ರಿಕ್ತ, VV_RO},
    {VV_NAME("scrollstart",	 VAR_STRING), ರಿಕ್ತ, 0},
    {VV_NAME("swapname",	 VAR_STRING), ರಿಕ್ತ, VV_RO},
    {VV_NAME("swapchoice",	 VAR_STRING), ರಿಕ್ತ, 0},
    {VV_NAME("swapcommand",	 VAR_STRING), ರಿಕ್ತ, VV_RO},
    {VV_NAME("char",		 VAR_STRING), ರಿಕ್ತ, 0},
    {VV_NAME("mouse_win",	 VAR_ಸಂಖ್ಯೆ), ರಿಕ್ತ, 0},
    {VV_NAME("mouse_winid",	 VAR_ಸಂಖ್ಯೆ), ರಿಕ್ತ, 0},
    {VV_NAME("mouse_lnum",	 VAR_ಸಂಖ್ಯೆ), ರಿಕ್ತ, 0},
    {VV_NAME("mouse_col",	 VAR_ಸಂಖ್ಯೆ), ರಿಕ್ತ, 0},
    {VV_NAME("operator",	 VAR_STRING), ರಿಕ್ತ, VV_RO},
    {VV_NAME("searchforward",	 VAR_ಸಂಖ್ಯೆ), ರಿಕ್ತ, 0},
    {VV_NAME("hlsearch",	 VAR_ಸಂಖ್ಯೆ), ರಿಕ್ತ, 0},
    {VV_NAME("oldfiles",	 VAR_LIST), &t_list_string, 0},
    {VV_NAME("windowid",	 VAR_ಸಂಖ್ಯೆ), ರಿಕ್ತ, VV_RO},
    {VV_NAME("progpath",	 VAR_STRING), ರಿಕ್ತ, VV_RO},
    {VV_NAME("completed_item",	 VAR_DICT), &t_dict_string, 0},
    {VV_NAME("option_new",	 VAR_STRING), ರಿಕ್ತ, VV_RO},
    {VV_NAME("option_old",	 VAR_STRING), ರಿಕ್ತ, VV_RO},
    {VV_NAME("option_oldlocal",	 VAR_STRING), ರಿಕ್ತ, VV_RO},
    {VV_NAME("option_oldglobal", VAR_STRING), ರಿಕ್ತ, VV_RO},
    {VV_NAME("option_command",	 VAR_STRING), ರಿಕ್ತ, VV_RO},
    {VV_NAME("option_type",	 VAR_STRING), ರಿಕ್ತ, VV_RO},
    {VV_NAME("errors",		 VAR_LIST), &t_list_string, 0},
    {VV_NAME("false",		 VAR_BOOL), ರಿಕ್ತ, VV_RO},
    {VV_NAME("true",		 VAR_BOOL), ರಿಕ್ತ, VV_RO},
    {VV_NAME("none",		 VAR_SPECIAL), ರಿಕ್ತ, VV_RO},
    {VV_NAME("null",		 VAR_SPECIAL), ರಿಕ್ತ, VV_RO},
    {VV_NAME("numbermax",	 VAR_ಸಂಖ್ಯೆ), ರಿಕ್ತ, VV_RO},
    {VV_NAME("numbermin",	 VAR_ಸಂಖ್ಯೆ), ರಿಕ್ತ, VV_RO},
    {VV_NAME("numbersize",	 VAR_ಸಂಖ್ಯೆ), ರಿಕ್ತ, VV_RO},
    {VV_NAME("vim_did_enter",	 VAR_ಸಂಖ್ಯೆ), ರಿಕ್ತ, VV_RO},
    {VV_NAME("testing",		 VAR_ಸಂಖ್ಯೆ), ರಿಕ್ತ, 0},
    {VV_NAME("t_number",	 VAR_ಸಂಖ್ಯೆ), ರಿಕ್ತ, VV_RO},
    {VV_NAME("t_string",	 VAR_ಸಂಖ್ಯೆ), ರಿಕ್ತ, VV_RO},
    {VV_NAME("t_func",		 VAR_ಸಂಖ್ಯೆ), ರಿಕ್ತ, VV_RO},
    {VV_NAME("t_list",		 VAR_ಸಂಖ್ಯೆ), ರಿಕ್ತ, VV_RO},
    {VV_NAME("t_dict",		 VAR_ಸಂಖ್ಯೆ), ರಿಕ್ತ, VV_RO},
    {VV_NAME("t_float",		 VAR_ಸಂಖ್ಯೆ), ರಿಕ್ತ, VV_RO},
    {VV_NAME("t_bool",		 VAR_ಸಂಖ್ಯೆ), ರಿಕ್ತ, VV_RO},
    {VV_NAME("t_none",		 VAR_ಸಂಖ್ಯೆ), ರಿಕ್ತ, VV_RO},
    {VV_NAME("t_job",		 VAR_ಸಂಖ್ಯೆ), ರಿಕ್ತ, VV_RO},
    {VV_NAME("t_channel",	 VAR_ಸಂಖ್ಯೆ), ರಿಕ್ತ, VV_RO},
    {VV_NAME("t_blob",		 VAR_ಸಂಖ್ಯೆ), ರಿಕ್ತ, VV_RO},
    {VV_NAME("t_class",		 VAR_ಸಂಖ್ಯೆ), ರಿಕ್ತ, VV_RO},
    {VV_NAME("t_object",	 VAR_ಸಂಖ್ಯೆ), ರಿಕ್ತ, VV_RO},
    {VV_NAME("termrfgresp",	 VAR_STRING), ರಿಕ್ತ, VV_RO},
    {VV_NAME("termrbgresp",	 VAR_STRING), ರಿಕ್ತ, VV_RO},
    {VV_NAME("termu7resp",	 VAR_STRING), ರಿಕ್ತ, VV_RO},
    {VV_NAME("termstyleresp",	 VAR_STRING), ರಿಕ್ತ, VV_RO},
    {VV_NAME("termblinkresp",	 VAR_STRING), ರಿಕ್ತ, VV_RO},
    {VV_NAME("event",		 VAR_DICT), ರಿಕ್ತ, VV_RO},
    {VV_NAME("versionlong",	 VAR_ಸಂಖ್ಯೆ), ರಿಕ್ತ, VV_RO},
    {VV_NAME("echospace",	 VAR_ಸಂಖ್ಯೆ), ರಿಕ್ತ, VV_RO},
    {VV_NAME("argv",		 VAR_LIST), &t_list_string, VV_RO},
    {VV_NAME("collate",		 VAR_STRING), ರಿಕ್ತ, VV_RO},
    {VV_NAME("exiting",		 VAR_SPECIAL), ರಿಕ್ತ, VV_RO},
    {VV_NAME("colornames",       VAR_DICT), &t_dict_string, VV_RO},
    {VV_NAME("sizeofint",	 VAR_ಸಂಖ್ಯೆ), ರಿಕ್ತ, VV_RO},
    {VV_NAME("sizeoflong",	 VAR_ಸಂಖ್ಯೆ), ರಿಕ್ತ, VV_RO},
    {VV_NAME("sizeofpointer",	 VAR_ಸಂಖ್ಯೆ), ರಿಕ್ತ, VV_RO},
    {VV_NAME("maxcol",		 VAR_ಸಂಖ್ಯೆ), ರಿಕ್ತ, VV_RO},
    {VV_NAME("python3_version",	 VAR_ಸಂಖ್ಯೆ), ರಿಕ್ತ, VV_RO},
    {VV_NAME("t_typealias",	 VAR_ಸಂಖ್ಯೆ), ರಿಕ್ತ, VV_RO},
    {VV_NAME("t_enum",		 VAR_ಸಂಖ್ಯೆ), ರಿಕ್ತ, VV_RO},
    {VV_NAME("t_enumvalue",	 VAR_ಸಂಖ್ಯೆ), ರಿಕ್ತ, VV_RO},
    {VV_NAME("stacktrace",	 VAR_LIST), &t_list_dict_any, VV_RO},
    {VV_NAME("t_tuple",		 VAR_ಸಂಖ್ಯೆ), ರಿಕ್ತ, VV_RO},
    {VV_NAME("wayland_display",  VAR_STRING), ರಿಕ್ತ, VV_RO},
    {VV_NAME("clipmethod",	 VAR_STRING), ರಿಕ್ತ, VV_RO},
    {VV_NAME("termda1",		 VAR_STRING), ರಿಕ್ತ, VV_RO},
    {VV_NAME("termosc",	 VAR_STRING), ರಿಕ್ತ, VV_RO},
    {VV_NAME("vim_did_init",	 VAR_ಸಂಖ್ಯೆ), ರಿಕ್ತ, VV_RO},
    {VV_NAME("clipproviders",	 VAR_DICT), ರಿಕ್ತ, VV_RO},
    {VV_NAME("imagebackend",	 VAR_STRING), ರಿಕ್ತ, VV_RO}
};

// shorthand
#define vv_tv_type	vv_di.di_tv.v_type
#define vv_nr		vv_di.di_tv.vval.v_ಸಂಖ್ಯೆ
#define vv_float	vv_di.di_tv.vval.v_float
#define vv_str		vv_di.di_tv.vval.v_string
#define vv_list		vv_di.di_tv.vval.v_list
#define vv_dict		vv_di.di_tv.vval.v_dict
#define vv_blob		vv_di.di_tv.vval.v_blob
#define vv_tv		vv_di.di_tv

ಸ್ಥಿರ dictitem_T	vimvars_var;		// variable used for v:
ಸ್ಥಿರ dict_T		vimvardict;		// Dictionary with v: variables
#define vimvarht  vimvardict.dv_hashtab

// for VIM_VERSION_ defines
#ಅನ್ತರ್ಭಾವ "version.h"

ಸ್ಥಿರ ಶೂನ್ಯ list_glob_vars(ಪೂರ್ಣಾಙ್ಕ *first);
ಸ್ಥಿರ ಶೂನ್ಯ list_buf_vars(ಪೂರ್ಣಾಙ್ಕ *first);
ಸ್ಥಿರ ಶೂನ್ಯ list_win_vars(ಪೂರ್ಣಾಙ್ಕ *first);
ಸ್ಥಿರ ಶೂನ್ಯ list_tab_vars(ಪೂರ್ಣಾಙ್ಕ *first);
ಸ್ಥಿರ char_u *list_arg_vars(exarg_T *eap, char_u *arg, ಪೂರ್ಣಾಙ್ಕ *first);
ಸ್ಥಿರ char_u *ex_let_one(char_u *arg, typval_T *tv, ಪೂರ್ಣಾಙ್ಕ copy, ಪೂರ್ಣಾಙ್ಕ flags, char_u *endchars, char_u *op, ಪೂರ್ಣಾಙ್ಕ var_idx);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ do_unlet_var(lval_T *lp, char_u *name_end, exarg_T *eap, ಪೂರ್ಣಾಙ್ಕ deep, ಶೂನ್ಯ *cookie);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ do_lock_var(lval_T *lp, char_u *name_end, exarg_T *eap, ಪೂರ್ಣಾಙ್ಕ deep, ಶೂನ್ಯ *cookie);
ಸ್ಥಿರ ಶೂನ್ಯ list_one_var(dictitem_T *v, ವರ್ಣ *prefix, ಪೂರ್ಣಾಙ್ಕ *first);
ಸ್ಥಿರ ಶೂನ್ಯ list_one_var_a(ವರ್ಣ *prefix, char_u *name, ಪೂರ್ಣಾಙ್ಕ type, char_u *string, ಪೂರ್ಣಾಙ್ಕ *first);

/*
 * Initialize global and vim special variables
 */
    ಶೂನ್ಯ
evalvars_init(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		    i;
    ಸಂರಚನಾ vimvar   *p;

    init_var_dict(&globvardict, &globvars_var, VAR_DEF_SCOPE);
    init_var_dict(&vimvardict, &vimvars_var, VAR_SCOPE);
    vimvardict.dv_lock = VAR_FIXED;
    hash_init(&compat_hashtab);

    ಪ್ರತಿ (i = 0; i < VV_LEN; ++i)
    {
	p = &vimvars[i];
	ಯದಿ (p->vv_flags & VV_RO)
	    p->vv_di.di_flags = DI_FLAGS_RO | DI_FLAGS_FIX;
	ಅನ್ಯಥಾ ಯದಿ (p->vv_flags & VV_RO_SBX)
	    p->vv_di.di_flags = DI_FLAGS_RO_SBX | DI_FLAGS_FIX;
	ಅನ್ಯಥಾ
	    p->vv_di.di_flags = DI_FLAGS_FIX;

	// add to v: scope dict, unless the value is not always available
	ಯದಿ (p->vv_tv_type != VAR_UNKNOWN)
	    hash_add(&vimvarht, p->vv_di.di_ಕೀಲಿ, "initialization");
	ಯದಿ (p->vv_flags & VV_COMPAT)
	    // add to compat scope dict
	    hash_add(&compat_hashtab, p->vv_di.di_ಕೀಲಿ, "initialization");
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
    set_vim_var_nr(VV_NUMBERSIZE, ಪರಿಮಾಣ(varnumber_T) * 8);
    set_vim_var_nr(VV_SIZEOFINT, ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ));
    set_vim_var_nr(VV_SIZEOFLONG, ಪರಿಮಾಣ(long));
    set_vim_var_nr(VV_SIZEOFPOINTER, ಪರಿಮಾಣ(ವರ್ಣ *));
    set_vim_var_nr(VV_MAXCOL, MAXCOL);

    set_vim_var_nr(VV_TYPE_ಸಂಖ್ಯೆ,  VAR_TYPE_ಸಂಖ್ಯೆ);
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

#ಯದಿ defined(EXITFREE)
/*
 * Free all vim variables information on exit
 */
    ಶೂನ್ಯ
evalvars_clear(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		    i;
    ಸಂರಚನಾ vimvar   *p;

    ಪ್ರತಿ (i = 0; i < VV_LEN; ++i)
    {
	p = &vimvars[i];
	ಯದಿ (p->vv_di.di_tv.v_type == VAR_STRING)
	    VIM_CLEAR(p->vv_str);
	ಅನ್ಯಥಾ ಯದಿ (p->vv_di.di_tv.v_type == VAR_LIST)
	{
	    list_unref(p->vv_list);
	    p->vv_list = ರಿಕ್ತ;
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
    ಪ್ರತಿ (i = 1; i <= script_ವಸ್ತುಗಳು.ga_len; ++i)
	vars_clear(&SCRIPT_VARS(i));
}
#endif

    ಪೂರ್ಣಾಙ್ಕ
garbage_collect_globvars(ಪೂರ್ಣಾಙ್ಕ copyID)
{
    ಪ್ರತ್ಯಾಗಮ set_ref_in_ht(&globvarht, copyID, ರಿಕ್ತ, ರಿಕ್ತ);
}

    ಪೂರ್ಣಾಙ್ಕ
garbage_collect_vimvars(ಪೂರ್ಣಾಙ್ಕ copyID)
{
    ಪ್ರತ್ಯಾಗಮ set_ref_in_ht(&vimvarht, copyID, ರಿಕ್ತ, ರಿಕ್ತ);
}

    ಪೂರ್ಣಾಙ್ಕ
garbage_collect_scriptvars(ಪೂರ್ಣಾಙ್ಕ copyID)
{
    ಪೂರ್ಣಾಙ್ಕ		    i;
    ಪೂರ್ಣಾಙ್ಕ		    idx;
    ಪೂರ್ಣಾಙ್ಕ		    abort = FALSE;
    scriptitem_T    *si;

    ಪ್ರತಿ (i = 1; i <= script_ವಸ್ತುಗಳು.ga_len; ++i)
    {
	abort = abort || set_ref_in_ht(&SCRIPT_VARS(i), copyID, ರಿಕ್ತ, ರಿಕ್ತ);

	si = SCRIPT_ITEM(i);
	ಪ್ರತಿ (idx = 0; idx < si->sn_var_vals.ga_len; ++idx)
	{
	    svar_T    *sv = ((svar_T *)si->sn_var_vals.ga_data) + idx;

	    ಯದಿ (sv->sv_name != ರಿಕ್ತ)
		abort = abort || set_ref_in_item(sv->sv_tv, copyID, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ);
	}
    }

    ಪ್ರತ್ಯಾಗಮ abort;
}

/*
 * Set an internal variable to a string value. Creates the variable if it does
 * not already exist.
 */
    ಶೂನ್ಯ
set_internal_string_var(char_u *name, char_u *ಮೌಲ್ಯ)
{
    char_u	*val;
    typval_T	*tvp;

    val = vim_strsave(ಮೌಲ್ಯ);
    ಯದಿ (val == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    tvp = alloc_string_tv(val);
    ಯದಿ (tvp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    set_var(name, tvp, FALSE);
    free_tv(tvp);
}

    ಪೂರ್ಣಾಙ್ಕ
eval_charconvert(
    char_u	*enc_from,
    char_u	*enc_to,
    char_u	*fname_from,
    char_u	*fname_to)
{
    ಪೂರ್ಣಾಙ್ಕ		err = FALSE;
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;

    set_vim_var_string(VV_CC_FROM, enc_from, -1);
    set_vim_var_string(VV_CC_TO, enc_to, -1);
    set_vim_var_string(VV_FNAME_IN, fname_from, -1);
    set_vim_var_string(VV_FNAME_OUT, fname_to, -1);
    ctx = get_option_sctx("charconvert");
    ಯದಿ (ctx != ರಿಕ್ತ)
	current_sctx = *ctx;

    ಯದಿ (eval_to_bool(p_ccv, &err, ರಿಕ್ತ, FALSE, TRUE))
	err = TRUE;

    set_vim_var_string(VV_CC_FROM, ರಿಕ್ತ, -1);
    set_vim_var_string(VV_CC_TO, ರಿಕ್ತ, -1);
    set_vim_var_string(VV_FNAME_IN, ರಿಕ್ತ, -1);
    set_vim_var_string(VV_FNAME_OUT, ರಿಕ್ತ, -1);
    current_sctx = saved_sctx;

    ಯದಿ (err)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಪ್ರತ್ಯಾಗಮ OK;
}

#ಯದಿ defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
    ಪೂರ್ಣಾಙ್ಕ
eval_printexpr(char_u *fname, char_u *args)
{
    ಪೂರ್ಣಾಙ್ಕ		err = FALSE;
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;

    set_vim_var_string(VV_FNAME_IN, fname, -1);
    set_vim_var_string(VV_CMDARG, args, -1);
    ctx = get_option_sctx("printexpr");
    ಯದಿ (ctx != ರಿಕ್ತ)
	current_sctx = *ctx;

    ಯದಿ (eval_to_bool(p_pexpr, &err, ರಿಕ್ತ, FALSE, TRUE))
	err = TRUE;

    set_vim_var_string(VV_FNAME_IN, ರಿಕ್ತ, -1);
    set_vim_var_string(VV_CMDARG, ರಿಕ್ತ, -1);
    current_sctx = saved_sctx;

    ಯದಿ (err)
    {
	mch_remove(fname);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}
#endif

#ಯದಿ defined(FEAT_DIFF)
    ಶೂನ್ಯ
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
    ಯದಿ (ctx != ರಿಕ್ತ)
	current_sctx = *ctx;

    // errors are ignored
    tv = eval_ಅಭಿವ್ಯಕ್ತಿ_ext(p_dex, ರಿಕ್ತ, TRUE);
    free_tv(tv);

    set_vim_var_string(VV_FNAME_IN, ರಿಕ್ತ, -1);
    set_vim_var_string(VV_FNAME_NEW, ರಿಕ್ತ, -1);
    set_vim_var_string(VV_FNAME_OUT, ರಿಕ್ತ, -1);
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

    set_vim_var_string(VV_FNAME_IN, origfile, -1);
    set_vim_var_string(VV_FNAME_DIFF, difffile, -1);
    set_vim_var_string(VV_FNAME_OUT, outfile, -1);

    ctx = get_option_sctx("patchexpr");
    ಯದಿ (ctx != ರಿಕ್ತ)
	current_sctx = *ctx;

    // errors are ignored
    tv = eval_ಅಭಿವ್ಯಕ್ತಿ_ext(p_pex, ರಿಕ್ತ, TRUE);
    free_tv(tv);

    set_vim_var_string(VV_FNAME_IN, ರಿಕ್ತ, -1);
    set_vim_var_string(VV_FNAME_DIFF, ರಿಕ್ತ, -1);
    set_vim_var_string(VV_FNAME_OUT, ರಿಕ್ತ, -1);
    current_sctx = saved_sctx;
}
#endif

#ಯದಿ defined(FEAT_SPELL)
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
    list_T	*list = ರಿಕ್ತ;
    char_u	*p = skipwhite(ಅಭಿವ್ಯಕ್ತಿ);
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;
    ಪೂರ್ಣಾಙ್ಕ		r;

    // Set "v:val" to the bad word.
    prepare_vimvar(VV_VAL, &save_val);
    set_vim_var_string(VV_VAL, badword, -1);
    ಯದಿ (p_verbose == 0)
	++emsg_off;
    ctx = get_option_sctx("spellsuggest");
    ಯದಿ (ctx != ರಿಕ್ತ)
	current_sctx = *ctx;

    r = may_call_simple_func(p, &rettv);
    ಯದಿ (r == NOTDONE)
	r = eval1(&p, &rettv, &EVALARG_EVALUATE);
    ಯದಿ (r == OK)
    {
	ಯದಿ (rettv.v_type != VAR_LIST)
	    clear_tv(&rettv);
	ಅನ್ಯಥಾ
	    list = rettv.vval.v_list;
    }

    ಯದಿ (p_verbose == 0)
	--emsg_off;
    clear_tv(get_vim_var_tv(VV_VAL));
    restore_vimvar(VV_VAL, &save_val);
    current_sctx = saved_sctx;

    ಪ್ರತ್ಯಾಗಮ list;
}

/*
 * "list" is supposed to contain two items: a word and a number.  Return the
 * word in "pp" and the number as the return value.
 * Return -1 if anything isn't right.
 * Used to get the good word and score from the eval_spell_expr() result.
 */
    ಪೂರ್ಣಾಙ್ಕ
get_spellword(list_T *list, char_u **pp)
{
    listitem_T	*li;

    li = list->lv_first;
    ಯದಿ (li == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ -1;
    *pp = tv_get_string(&li->li_tv);

    li = li->li_next;
    ಯದಿ (li == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ -1;
    ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ(&li->li_tv);
}
#endif

/*
 * Prepare v: variable "idx" to be used.
 * Save the current typeval in "save_tv" and clear it.
 * When not used yet add the variable to the v: hashtable.
 */
    ಶೂನ್ಯ
prepare_vimvar(ಪೂರ್ಣಾಙ್ಕ idx, typval_T *save_tv)
{
    *save_tv = vimvars[idx].vv_tv;
    vimvars[idx].vv_str = ರಿಕ್ತ;  // don't free it now
    ಯದಿ (vimvars[idx].vv_tv_type == VAR_UNKNOWN)
	hash_add(&vimvarht, vimvars[idx].vv_di.di_ಕೀಲಿ, "prepare vimvar");
}

/*
 * Restore v: variable "idx" to typeval "save_tv".
 * Note that the v: variable must have been cleared already.
 * When no longer defined, remove the variable from the v: hashtable.
 */
    ಶೂನ್ಯ
restore_vimvar(ಪೂರ್ಣಾಙ್ಕ idx, typval_T *save_tv)
{
    hashitem_T	*hi;

    vimvars[idx].vv_tv = *save_tv;
    ಯದಿ (vimvars[idx].vv_tv_type != VAR_UNKNOWN)
	ಪ್ರತ್ಯಾಗಮ;

    hi = hash_find(&vimvarht, vimvars[idx].vv_di.di_ಕೀಲಿ);
    ಯದಿ (HASHITEM_EMPTY(hi))
	internal_error("restore_vimvar()");
    ಅನ್ಯಥಾ
	hash_remove(&vimvarht, hi, "restore vimvar");
}

/*
 * List Vim variables.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
list_vim_vars(ಪೂರ್ಣಾಙ್ಕ *first)
{
    list_hashtable_vars(&vimvarht, "v:", FALSE, first);
}

/*
 * List script-local variables, if there is a script.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
list_script_vars(ಪೂರ್ಣಾಙ್ಕ *first)
{
    ಯದಿ (SCRIPT_ID_VALID(current_sctx.sc_sid))
	list_hashtable_vars(&SCRIPT_VARS(current_sctx.sc_sid),
							   "s:", FALSE, first);
}

/*
 * Return TRUE if "name" starts with "g:", "w:", "t:" or "b:".
 * But only when an identifier character follows.
 */
    ಪೂರ್ಣಾಙ್ಕ
is_scoped_variable(char_u *name)
{
    ಪ್ರತ್ಯಾಗಮ vim_strchr((char_u *)"gwbt", name[0]) != ರಿಕ್ತ
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
eval_one_ಅಭಿವ್ಯಕ್ತಿ_in_str(char_u *p, garray_T *gap, ಪೂರ್ಣಾಙ್ಕ evaluate)
{
    char_u	*block_start = skipwhite(p + 1);  // skip the opening {
    char_u	*block_end = block_start;
    char_u	*ಅಭಿವ್ಯಕ್ತಿ_val;

    ಯದಿ (*block_start == NUL)
    {
	semsg(_(e_missing_close_curly_str), p);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಯದಿ (skip_ಅಭಿವ್ಯಕ್ತಿ(&block_end, ರಿಕ್ತ) == FAIL)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    block_end = skipwhite(block_end);
    ಯದಿ (*block_end != '}')
    {
	semsg(_(e_missing_close_curly_str), p);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಯದಿ (evaluate)
    {
	*block_end = NUL;
	ಅಭಿವ್ಯಕ್ತಿ_val = eval_to_string(block_start, FALSE, FALSE);
	*block_end = '}';
	ಯದಿ (ಅಭಿವ್ಯಕ್ತಿ_val == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	ga_concat(gap, ಅಭಿವ್ಯಕ್ತಿ_val);
	vim_free(ಅಭಿವ್ಯಕ್ತಿ_val);
    }

    ಪ್ರತ್ಯಾಗಮ block_end + 1;
}

/*
 * Evaluate all the Vim expressions {expr} in "str" and return the resulting
 * string in allocated memory.  "{{" is reduced to "{" and "}}" to "}".
 * Used for a heredoc assignment.
 * Returns NULL for an error.
 */
    ಸ್ಥಿರ char_u *
eval_all_ಅಭಿವ್ಯಕ್ತಿ_in_str(char_u *str)
{
    garray_T	ga;
    char_u	*p;

    ga_init2(&ga, 1, 80);
    p = str;

    ಯಾವತ್ (*p != NUL)
    {
	char_u	*lit_start;
	ಪೂರ್ಣಾಙ್ಕ	escaped_brace = FALSE;

	// Look for a block start.
	lit_start = p;
	ಯಾವತ್ (*p != '{' && *p != '}' && *p != NUL)
	    ++p;

	ಯದಿ (*p != NUL && *p == p[1])
	{
	    // Escaped brace, unescape and continue.
	    // Include the brace in the literal string.
	    ++p;
	    escaped_brace = TRUE;
	}
	ಅನ್ಯಥಾ ಯದಿ (*p == '}')
	{
	    semsg(_(e_stray_closing_curly_str), str);
	    ga_clear(&ga);
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}

	// Append the literal part.
	ga_concat_len(&ga, lit_start, (size_t)(p - lit_start));

	ಯದಿ (*p == NUL)
	    ಭಙ್ಗ;

	ಯದಿ (escaped_brace)
	{
	    // Skip the second brace.
	    ++p;
	    ಅನುವರ್ತ;
	}

	// Evaluate the expression and append the result.
	p = eval_one_ಅಭಿವ್ಯಕ್ತಿ_in_str(p, &ga, TRUE);
	ಯದಿ (p == ರಿಕ್ತ)
	{
	    ga_clear(&ga);
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
    }
    ga_append(&ga, NUL);

    ಪ್ರತ್ಯಾಗಮ ga.ga_data;
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
heredoc_get(exarg_T *eap, char_u *cmd, ಪೂರ್ಣಾಙ್ಕ script_get, ಪೂರ್ಣಾಙ್ಕ vim9compile)
{
    char_u	*theline = ರಿಕ್ತ;
    char_u	*marker;
    list_T	*l;
    char_u	*p;
    char_u	*str;
    ಪೂರ್ಣಾಙ್ಕ		marker_indent_len = 0;
    ಪೂರ್ಣಾಙ್ಕ		text_indent_len = 0;
    char_u	*text_indent = ರಿಕ್ತ;
    char_u	dot[] = ".";
    ಪೂರ್ಣಾಙ್ಕ		comment_char = in_vim9script() ? '#' : '"';
    ಪೂರ್ಣಾಙ್ಕ		evalstr = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		eval_failed = FALSE;
    cctx_T	*cctx = vim9compile ? eap->cookie : ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ = 0;
    ಪೂರ್ಣಾಙ್ಕ		heredoc_in_string = FALSE;
    char_u	*ಸಾಲು_arg = ರಿಕ್ತ;
    char_u	*nl_ptr = vim_strchr(cmd, '\n');

    ಯದಿ (nl_ptr != ರಿಕ್ತ)
    {
	heredoc_in_string = TRUE;
	ಸಾಲು_arg = nl_ptr + 1;
	*nl_ptr = NUL;
    }
    ಅನ್ಯಥಾ ಯದಿ (eap->ea_getline == ರಿಕ್ತ)
    {
	emsg(_(e_cannot_use_heredoc_here));
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    // Check for the optional 'trim' word before the marker
    cmd = skipwhite(cmd);

    ಯಾವತ್ (TRUE)
    {
	ಯದಿ (STRNCMP(cmd, "trim", 4) == 0
		&& (cmd[4] == NUL || VIM_ISWHITE(cmd[4])))
	{
	    cmd = skipwhite(cmd + 4);

	    // Trim the indentation from all the lines in the here document.
	    // The amount of indentation trimmed is the same as the indentation
	    // of the first line after the :let command line.  To find the end
	    // marker the indent of the :let command line is trimmed.
	    p = *eap->cmdlinep;
	    ಯಾವತ್ (VIM_ISWHITE(*p))
	    {
		p++;
		marker_indent_len++;
	    }
	    text_indent_len = -1;

	    ಅನುವರ್ತ;
	}
	ಯದಿ (STRNCMP(cmd, "eval", 4) == 0
		&& (cmd[4] == NUL || VIM_ISWHITE(cmd[4])))
	{
	    cmd = skipwhite(cmd + 4);
	    evalstr = TRUE;
	    ಅನುವರ್ತ;
	}
	ಭಙ್ಗ;
    }

    // The marker is the next word.
    ಯದಿ (*cmd != NUL && *cmd != comment_char)
    {
	marker = skipwhite(cmd);
	p = skiptowhite(marker);
	ಯದಿ (*skipwhite(p) != NUL && *skipwhite(p) != comment_char)
	{
	    semsg(_(e_trailing_characters_str), p);
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
	*p = NUL;
	ಯದಿ (!script_get && vim_islower(*marker))
	{
	    emsg(_(e_marker_cannot_start_with_lower_case_letter));
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
    }
    ಅನ್ಯಥಾ
    {
	// When getting lines for an embedded script, if the marker is missing,
	// accept '.' as the marker.
	ಯದಿ (script_get)
	    marker = dot;
	ಅನ್ಯಥಾ
	{
	    emsg(_(e_missing_marker));
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
    }

    l = list_alloc();
    ಯದಿ (l == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಪ್ರತಿ (;;)
    {
	ಪೂರ್ಣಾಙ್ಕ	mi = 0;
	ಪೂರ್ಣಾಙ್ಕ	ti = 0;

	ಯದಿ (heredoc_in_string)
	{
	    char_u	*next_ಸಾಲು;

	    // heredoc in a string separated by newlines.  Get the next line
	    // from the string.

	    ಯದಿ (*ಸಾಲು_arg == NUL)
	    {
		semsg(_(e_missing_end_marker_str), marker);
		ಭಙ್ಗ;
	    }

	    theline = ಸಾಲು_arg;
	    next_ಸಾಲು = vim_strchr(theline, '\n');
	    ಯದಿ (next_ಸಾಲು == ರಿಕ್ತ)
		ಸಾಲು_arg += STRLEN(ಸಾಲು_arg);
	    ಅನ್ಯಥಾ
	    {
		*next_ಸಾಲು = NUL;
		ಸಾಲು_arg = next_ಸಾಲು + 1;
	    }
	}
	ಅನ್ಯಥಾ
	{
	    vim_free(theline);
	    theline = eap->ea_getline(NUL, eap->cookie, 0, FALSE);
	    ಯದಿ (theline == ರಿಕ್ತ)
	    {
		semsg(_(e_missing_end_marker_str), marker);
		ಭಙ್ಗ;
	    }
	}

	// with "trim": skip the indent matching the :let line to find the
	// marker
	ಯದಿ (marker_indent_len > 0
		&& STRNCMP(theline, *eap->cmdlinep, marker_indent_len) == 0)
	    mi = marker_indent_len;
	ಯದಿ (STRCMP(marker, theline + mi) == 0)
	    ಭಙ್ಗ;

	// If expression evaluation failed in the heredoc, then skip till the
	// end marker.
	ಯದಿ (eval_failed)
	    ಅನುವರ್ತ;

	ಯದಿ (text_indent_len == -1 && *theline != NUL)
	{
	    // set the text indent from the first line.
	    p = theline;
	    text_indent_len = 0;
	    ಯಾವತ್ (VIM_ISWHITE(*p))
	    {
		p++;
		text_indent_len++;
	    }
	    text_indent = vim_strnsave(theline, text_indent_len);
	}
	// with "trim": skip the indent matching the first line
	ಯದಿ (text_indent != ರಿಕ್ತ)
	    ಪ್ರತಿ (ti = 0; ti < text_indent_len; ++ti)
		ಯದಿ (theline[ti] != text_indent[ti])
		    ಭಙ್ಗ;

	str = theline + ti;
	ಯದಿ (vim9compile)
	{
	    ಯದಿ (compile_all_ಅಭಿವ್ಯಕ್ತಿ_in_str(str, evalstr, cctx) == FAIL)
	    {
		vim_free(theline);
		vim_free(text_indent);
		list_free(l);
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    ಎಣಿಕೆ++;
	}
	ಅನ್ಯಥಾ
	{
	    ಪೂರ್ಣಾಙ್ಕ	    free_str = FALSE;

	    ಯದಿ (evalstr && !eap->skip)
	    {
		str = eval_all_ಅಭಿವ್ಯಕ್ತಿ_in_str(str);
		ಯದಿ (str == ರಿಕ್ತ)
		{
		    // expression evaluation failed
		    eval_failed = TRUE;
		    ಅನುವರ್ತ;
		}
		free_str = TRUE;
	    }

	    ಯದಿ (list_append_string(l, str, -1) == FAIL)
	    {
		ಯದಿ (free_str)
		    vim_free(str);
		ಭಙ್ಗ;
	    }
	    ಯದಿ (free_str)
		vim_free(str);
	}
    }
    ಯದಿ (heredoc_in_string)
	// Next command follows the heredoc in the string.
	eap->nextcmd = ಸಾಲು_arg;
    ಅನ್ಯಥಾ
	vim_free(theline);
    vim_free(text_indent);

    ಯದಿ (vim9compile && cctx->ctx_skip != SKIP_YES && !eval_failed)
	generate_NEWLIST(cctx, ಎಣಿಕೆ, FALSE);

    ಯದಿ (eval_failed)
    {
	// expression evaluation in the heredoc failed
	list_free(l);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಪ್ರತ್ಯಾಗಮ l;
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
    ಪೂರ್ಣಾಙ್ಕ	    has_var;

    ಯದಿ (!in_vim9script())
    {
	semsg(_(e_str_cannot_be_used_in_legacy_vim_script), ":var");
	ಪ್ರತ್ಯಾಗಮ;
    }
    has_var = checkforcmd_noparen(&p, "var", 3);
    ಯದಿ (current_sctx.sc_sid == 0 && has_var)
    {
	emsg(_(e_cannot_declare_variable_on_command_ಸಾಲು));
	ಪ್ರತ್ಯಾಗಮ;
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
    char_u	*ಅಭಿವ್ಯಕ್ತಿ = ರಿಕ್ತ;
    typval_T	rettv;
    ಪೂರ್ಣಾಙ್ಕ		var_ಎಣಿಕೆ = 0;
    ಪೂರ್ಣಾಙ್ಕ		semicolon = 0;
    char_u	op[4];
    char_u	*argend;
    ಪೂರ್ಣಾಙ್ಕ		first = TRUE;
    ಪೂರ್ಣಾಙ್ಕ		concat;
    ಪೂರ್ಣಾಙ್ಕ		has_assign;
    ಪೂರ್ಣಾಙ್ಕ		flags = 0;
    ಪೂರ್ಣಾಙ್ಕ		vim9script = in_vim9script();

    ಯದಿ (eap->cmdidx == CMD_final && !vim9script)
    {
	// In legacy Vim script ":final" is short for ":finally".
	ex_finally(eap);
	ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (eap->cmdidx == CMD_let && vim9script)
    {
	emsg(_(e_cannot_use_let_in_vim9_script));
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (eap->cmdidx == CMD_const)
	flags |= ASSIGN_CONST;
    ಅನ್ಯಥಾ ಯದಿ (eap->cmdidx == CMD_final)
	flags |= ASSIGN_FINAL;

    // Vim9 assignment without ":let", ":const" or ":final"
    ಯದಿ (eap->arg == eap->cmd)
	flags |= ASSIGN_NO_DECL;

    argend = skip_var_list(arg, TRUE, &var_ಎಣಿಕೆ, &semicolon, FALSE);
    ಯದಿ (argend == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    ಅಭಿವ್ಯಕ್ತಿ = skipwhite(argend);
    concat = ಅಭಿವ್ಯಕ್ತಿ[0] == '.'
	&& ((ಅಭಿವ್ಯಕ್ತಿ[1] == '=' && in_old_script(2))
		|| (ಅಭಿವ್ಯಕ್ತಿ[1] == '.' && ಅಭಿವ್ಯಕ್ತಿ[2] == '='));
    has_assign =  *ಅಭಿವ್ಯಕ್ತಿ == '=' || (vim_strchr((char_u *)"+-*/%", *ಅಭಿವ್ಯಕ್ತಿ) != ರಿಕ್ತ
							    && ಅಭಿವ್ಯಕ್ತಿ[1] == '=');
    ಯದಿ (!has_assign && !concat)
    {
	// ":let" without "=": list variables
	ಯದಿ (*arg == '[')
	    emsg(_(e_invalid_argument));
	ಅನ್ಯಥಾ ಯದಿ (ಅಭಿವ್ಯಕ್ತಿ[0] == '.' && ಅಭಿವ್ಯಕ್ತಿ[1] == '=')
	    emsg(_(e_dot_equal_not_supported_with_script_version_two));
	ಅನ್ಯಥಾ ಯದಿ (!ends_excmd2(eap->cmd, arg))
	{
	    ಯದಿ (vim9script)
	    {
		ಯದಿ (!ends_excmd2(eap->cmd, skipwhite(argend)))
		    semsg(_(e_trailing_characters_str), argend);
		ಅನ್ಯಥಾ
		    // Vim9 declaration ":var name: type"
		    arg = vim9_declare_scriptvar(eap, arg);
	    }
	    ಅನ್ಯಥಾ
	    {
		// ":let var1 var2" - list values
		arg = list_arg_vars(eap, arg, &first);
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (!eap->skip)
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
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (ಅಭಿವ್ಯಕ್ತಿ[0] == '=' && ಅಭಿವ್ಯಕ್ತಿ[1] == '<' && ಅಭಿವ್ಯಕ್ತಿ[2] == '<')
    {
	list_T	*l = ರಿಕ್ತ;
	long	cur_lnum = SOURCING_LNUM;

	// :let text =<< [trim] [eval] END
	// :var text =<< [trim] [eval] END
	ಯದಿ (vim9script && !eap->skip && (!VIM_ISWHITE(ಅಭಿವ್ಯಕ್ತಿ[-1])
						 || !IS_WHITE_OR_NUL(ಅಭಿವ್ಯಕ್ತಿ[3])))
	    semsg(_(e_white_space_required_before_and_after_str_at_str),
								  "=<<", ಅಭಿವ್ಯಕ್ತಿ);
	ಅನ್ಯಥಾ
	    l = heredoc_get(eap, ಅಭಿವ್ಯಕ್ತಿ + 3, FALSE, FALSE);

	ಯದಿ (l != ರಿಕ್ತ)
	{
	    rettv_list_set(&rettv, l);
	    ಯದಿ (!eap->skip)
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
	ಪ್ರತ್ಯಾಗಮ;
    }

    evalarg_T   evalarg;
    ಪೂರ್ಣಾಙ್ಕ	    len = 1;

    CLEAR_FIELD(rettv);

    ಪೂರ್ಣಾಙ್ಕ cur_lnum;

    op[0] = '=';
    op[1] = NUL;
    ಯದಿ (*ಅಭಿವ್ಯಕ್ತಿ != '=')
    {
	ಯದಿ (vim9script && (flags & ASSIGN_NO_DECL) == 0)
	{
	    // +=, /=, etc. require an existing variable
	    semsg(_(e_cannot_use_operator_on_new_variable_str), eap->arg);
	}
	ಅನ್ಯಥಾ ಯದಿ (vim_strchr((char_u *)"+-*/%.", *ಅಭಿವ್ಯಕ್ತಿ) != ರಿಕ್ತ)
	{
	    op[0] = *ಅಭಿವ್ಯಕ್ತಿ;   // +=, -=, *=, /=, %= or .=
	    ++len;
	    ಯದಿ (ಅಭಿವ್ಯಕ್ತಿ[0] == '.' && ಅಭಿವ್ಯಕ್ತಿ[1] == '.') // ..=
	    {
		++ಅಭಿವ್ಯಕ್ತಿ;
		++len;
	    }
	}
	ಅಭಿವ್ಯಕ್ತಿ += 2;
    }
    ಅನ್ಯಥಾ
	++ಅಭಿವ್ಯಕ್ತಿ;

    ಯದಿ (vim9script && !eap->skip && (!VIM_ISWHITE(*argend)
					       || !IS_WHITE_OR_NUL(*ಅಭಿವ್ಯಕ್ತಿ)))
    {
	vim_strncpy(op, ಅಭಿವ್ಯಕ್ತಿ - len, len);
	semsg(_(e_white_space_required_before_and_after_str_at_str),
							       op, argend);
    }

    ಯದಿ (eap->skip)
	++emsg_skip;
    fill_evalarg_from_eap(&evalarg, eap, eap->skip);
    ಅಭಿವ್ಯಕ್ತಿ = skipwhite_and_linebreak(ಅಭಿವ್ಯಕ್ತಿ, &evalarg);
    cur_lnum = SOURCING_LNUM;
    ಪೂರ್ಣಾಙ್ಕ eval_res = eval0(ಅಭಿವ್ಯಕ್ತಿ, &rettv, eap, &evalarg);
    ಯದಿ (eap->skip)
	--emsg_skip;
    clear_evalarg(&evalarg, eap);

    // Restore the line number so that any type error is given for the
    // declaration, not the expression.
    SOURCING_LNUM = cur_lnum;

    ಯದಿ (!eap->skip && eval_res != FAIL)
	(ಶೂನ್ಯ)ex_let_vars(eap->arg, &rettv, FALSE, semicolon, var_ಎಣಿಕೆ,
								    flags, op);
    ಯದಿ (eval_res != FAIL)
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
    ಪೂರ್ಣಾಙ್ಕ
ex_let_vars(
    char_u	*arg_start,
    typval_T	*tv,
    ಪೂರ್ಣಾಙ್ಕ		copy,		// copy values from "tv", don't move
    ಪೂರ್ಣಾಙ್ಕ		semicolon,	// from skip_var_list()
    ಪೂರ್ಣಾಙ್ಕ		var_ಎಣಿಕೆ,	// from skip_var_list()
    ಪೂರ್ಣಾಙ್ಕ		flags,		// ASSIGN_FINAL, ASSIGN_CONST, etc.
    char_u	*op)
{
    char_u	*arg = arg_start;
    list_T	*l;
    tuple_T	*tuple = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		var_idx = 0;
    listitem_T	*item = ರಿಕ್ತ;
    typval_T	ltv;
    ಪೂರ್ಣಾಙ್ಕ		is_list = tv->v_type == VAR_LIST;
    ಪೂರ್ಣಾಙ್ಕ		idx;

    ಯದಿ (tv->v_type == VAR_VOID)
    {
	emsg(_(e_cannot_use_void_ಮೌಲ್ಯ));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಯದಿ (*arg != '[')
    {
	// ":let var = expr" or ":for var in list"
	ಯದಿ (ex_let_one(arg, tv, copy, flags, op, op, var_idx) == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	ಪ್ರತ್ಯಾಗಮ OK;
    }

    // ":let [v1, v2] = list" or ":for [v1, v2] in listlist"
    // or
    // ":let [v1, v2] = tuple" or ":for [v1, v2] in tupletuple"
    ಯದಿ (tv->v_type != VAR_LIST && tv->v_type != VAR_TUPLE)
    {
	emsg(_(e_list_or_tuple_required));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಯದಿ (is_list)
    {
	l = tv->vval.v_list;
	ಯದಿ (l == ರಿಕ್ತ)
	{
	    emsg(_(e_list_required));
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	i = list_len(l);
    }
    ಅನ್ಯಥಾ
    {
	tuple = tv->vval.v_tuple;
	ಯದಿ (tuple == ರಿಕ್ತ)
	{
	    emsg(_(e_tuple_required));
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	i = tuple_len(tuple);
    }

    ಯದಿ (semicolon == 0 && var_ಎಣಿಕೆ < i)
    {
	emsg(_(is_list ? e_less_targets_than_list_ವಸ್ತುಗಳು
					: e_less_targets_than_tuple_ವಸ್ತುಗಳು));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಯದಿ (var_ಎಣಿಕೆ - semicolon > i)
    {
	emsg(_(is_list ? e_more_targets_than_list_ವಸ್ತುಗಳು
					: e_more_targets_than_tuple_ವಸ್ತುಗಳು));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಯದಿ (is_list)
    {
	CHECK_LIST_MATERIALIZE(l);
	item = l->lv_first;
    }
    ಅನ್ಯಥಾ
	idx = 0;

    ಯಾವತ್ (*arg != ']')
    {
	arg = skipwhite(arg + 1);
	++var_idx;
	arg = ex_let_one(arg, is_list ? &item->li_tv : TUPLE_ITEM(tuple, idx),
			 TRUE, flags | ASSIGN_UNPACK, (char_u *)",;]", op,
			 var_idx);
	ಯದಿ (is_list)
	    item = item->li_next;
	ಅನ್ಯಥಾ
	    idx++;
	ಯದಿ (arg == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	arg = skipwhite(arg);
	ಯದಿ (*arg == ';')
	{
	    // Put the rest of the list or tuple (may be empty) in the var
	    // after ';'.  Create a new list or tuple for this.
	    ಯದಿ (is_list)
	    {
		// Put the rest of the list (may be empty) in the var
		// after ';'.  Create a new list for this.
		l = list_alloc();
		ಯದಿ (l == ರಿಕ್ತ)
		    ಪ್ರತ್ಯಾಗಮ FAIL;

		// list
		ಯಾವತ್ (item != ರಿಕ್ತ)
		{
		    list_append_tv(l, &item->li_tv);
		    item = item->li_next;
		}

		ltv.v_type = VAR_LIST;
		ltv.v_lock = 0;
		ltv.vval.v_list = l;
		l->lv_refcount = 1;
	    }
	    ಅನ್ಯಥಾ
	    {
		tuple_T	*new_tuple = tuple_alloc();
		ಯದಿ (new_tuple == ರಿಕ್ತ)
		    ಪ್ರತ್ಯಾಗಮ FAIL;

		// Put the rest of the tuple (may be empty) in the var
		// after ';'.  Create a new tuple for this.
		ಯಾವತ್ (idx < TUPLE_LEN(tuple))
		{
		    typval_T    new_tv;

		    copy_tv(TUPLE_ITEM(tuple, idx), &new_tv);
		    ಯದಿ (tuple_append_tv(new_tuple, &new_tv) == FAIL)
		    {
			clear_tv(&new_tv);
			tuple_unref(new_tuple);
			ಪ್ರತ್ಯಾಗಮ FAIL;
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
	    ಯದಿ (arg == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    ಭಙ್ಗ;
	}
	ಅನ್ಯಥಾ ಯದಿ (*arg != ',' && *arg != ']')
	{
	    internal_error("ex_let_vars()");
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
    }

    ಪ್ರತ್ಯಾಗಮ OK;
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
    ಪೂರ್ಣಾಙ್ಕ		include_type,
    ಪೂರ್ಣಾಙ್ಕ		*var_ಎಣಿಕೆ,
    ಪೂರ್ಣಾಙ್ಕ		*semicolon,
    ಪೂರ್ಣಾಙ್ಕ		silent)
{
    char_u	*p, *s;

    ಯದಿ (*arg == '[')
    {
	// "[var, var]": find the matching ']'.
	p = arg;
	ಪ್ರತಿ (;;)
	{
	    p = skipwhite(p + 1);	// skip whites after '[', ';' or ','
	    s = skip_var_one(p, include_type);
	    ಯದಿ (s == p)
	    {
		ಯದಿ (!silent)
		    semsg(_(e_invalid_argument_str), p);
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    }
	    ++*var_ಎಣಿಕೆ;

	    p = skipwhite(s);
	    ಯದಿ (*p == ']')
		ಭಙ್ಗ;
	    ಅನ್ಯಥಾ ಯದಿ (*p == ';')
	    {
		ಯದಿ (*semicolon == 1)
		{
		    ಯದಿ (!silent)
			emsg(_(e_double_semicolon_in_list_of_variables));
		    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
		}
		*semicolon = 1;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (*p != ',')
	    {
		ಯದಿ (!silent)
		    semsg(_(e_invalid_argument_str), p);
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    }
	}
	ಪ್ರತ್ಯಾಗಮ p + 1;
    }

    ಪ್ರತ್ಯಾಗಮ skip_var_one(arg, include_type);
}

/*
 * Skip one (assignable) variable name, including @r, $VAR, &option, d.key,
 * l[idx].
 * In Vim9 script also skip over ": type" if "include_type" is TRUE.
 */
    char_u *
skip_var_one(char_u *arg, ಪೂರ್ಣಾಙ್ಕ include_type)
{
    char_u	*end;
    ಪೂರ್ಣಾಙ್ಕ		vim9 = in_vim9script();

    ಯದಿ (*arg == '@' && arg[1] != NUL)
	ಪ್ರತ್ಯಾಗಮ arg + 2;

    // termcap option name may have non-alpha characters
    ಯದಿ (STRNCMP(arg, "&t_", 3) == 0 && arg[3] != NUL && arg[4] != NUL)
	ಪ್ರತ್ಯಾಗಮ arg + 5;

    end = find_name_end(*arg == '$' || *arg == '&' ? arg + 1 : arg,
				   ರಿಕ್ತ, ರಿಕ್ತ, FNE_INCL_BR | FNE_CHECK_START);

    // "a: type" is declaring variable "a" with a type, not "a:".
    // Same for "s: type".
    ಯದಿ (vim9 && end == arg + 2 && end[-1] == ':')
	--end;

    ಯದಿ (include_type && vim9)
    {
	ಯದಿ (*skipwhite(end) == ':')
	    end = skip_type(skipwhite(skipwhite(end) + 1), FALSE);
    }
    ಪ್ರತ್ಯಾಗಮ end;
}

/*
 * List variables for hashtab "ht" with prefix "prefix".
 * If "empty" is TRUE also list NULL strings as empty strings.
 */
    ಶೂನ್ಯ
list_hashtable_vars(
    hashtab_T	*ht,
    ವರ್ಣ	*prefix,
    ಪೂರ್ಣಾಙ್ಕ		empty,
    ಪೂರ್ಣಾಙ್ಕ		*first)
{
    hashitem_T	*hi;
    dictitem_T	*di;
    ಪೂರ್ಣಾಙ್ಕ		todo;
    char_u	buf[IOSIZE];

    ಪೂರ್ಣಾಙ್ಕ save_ht_flags = ht->ht_flags;
    ht->ht_flags |= HTFLAGS_FROZEN;

    todo = (ಪೂರ್ಣಾಙ್ಕ)ht->ht_used;
    ಪ್ರತಿ (hi = ht->ht_array; todo > 0 && !got_int; ++hi)
    {
	ಯದಿ (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    di = HI2DI(hi);

	    // apply :filter /pat/ to variable name
	    vim_strncpy((char_u *)buf, (char_u *)prefix, IOSIZE - 1);
	    vim_strcat((char_u *)buf, di->di_ಕೀಲಿ, IOSIZE);
	    ಯದಿ (message_filtered(buf))
		ಅನುವರ್ತ;

	    ಯದಿ (empty || di->di_tv.v_type != VAR_STRING
					   || di->di_tv.vval.v_string != ರಿಕ್ತ)
		list_one_var(di, prefix, first);
	}
    }

    ht->ht_flags = save_ht_flags;
}

/*
 * List global variables.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
list_glob_vars(ಪೂರ್ಣಾಙ್ಕ *first)
{
    list_hashtable_vars(&globvarht, "", TRUE, first);
}

/*
 * List buffer variables.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
list_buf_vars(ಪೂರ್ಣಾಙ್ಕ *first)
{
    list_hashtable_vars(&curbuf->b_vars->dv_hashtab, "b:", TRUE, first);
}

/*
 * List window variables.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
list_win_vars(ಪೂರ್ಣಾಙ್ಕ *first)
{
    list_hashtable_vars(&curwin->w_vars->dv_hashtab, "w:", TRUE, first);
}

/*
 * List tab page variables.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
list_tab_vars(ಪೂರ್ಣಾಙ್ಕ *first)
{
    list_hashtable_vars(&curtab->tp_vars->dv_hashtab, "t:", TRUE, first);
}

/*
 * List variables in "arg".
 */
    ಸ್ಥಿರ char_u *
list_arg_vars(exarg_T *eap, char_u *arg, ಪೂರ್ಣಾಙ್ಕ *first)
{
    ಪೂರ್ಣಾಙ್ಕ		error = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		len;
    char_u	*name;
    char_u	*name_start;
    char_u	*arg_subsc;
    char_u	*tofree;
    typval_T    tv;

    ಯಾವತ್ (!ends_excmd2(eap->cmd, arg) && !got_int)
    {
	ಯದಿ (error || eap->skip)
	{
	    arg = find_name_end(arg, ರಿಕ್ತ, ರಿಕ್ತ, FNE_INCL_BR | FNE_CHECK_START);
	    ಯದಿ (!VIM_ISWHITE(*arg) && !ends_excmd(*arg))
	    {
		emsg_severe = TRUE;
		ಯದಿ (!did_emsg)
		    semsg(_(e_trailing_characters_str), arg);
		ಭಙ್ಗ;
	    }
	}
	ಅನ್ಯಥಾ
	{
	    // get_name_len() takes care of expanding curly braces
	    name_start = name = arg;
	    len = get_name_len(&arg, &tofree, TRUE, TRUE);
	    ಯದಿ (len <= 0)
	    {
		// This is mainly to keep test 49 working: when expanding
		// curly braces fails overrule the exception error message.
		ಯದಿ (len < 0 && !aborting())
		{
		    emsg_severe = TRUE;
		    semsg(_(e_invalid_argument_str), arg);
		    ಭಙ್ಗ;
		}
		error = TRUE;
	    }
	    ಅನ್ಯಥಾ
	    {
		arg = skipwhite(arg);
		ಯದಿ (tofree != ರಿಕ್ತ)
		    name = tofree;
		ಯದಿ (eval_variable(name, len, 0, &tv, ರಿಕ್ತ,
						     EVAL_VAR_VERBOSE) == FAIL)
		    error = TRUE;
		ಅನ್ಯಥಾ
		{
		    // handle d.key, l[idx], f(expr)
		    arg_subsc = arg;
		    ಯದಿ (handle_subscript(&arg, name_start, &tv,
					      &EVALARG_EVALUATE, TRUE) == FAIL)
			error = TRUE;
		    ಅನ್ಯಥಾ
		    {
			ಯದಿ (arg == arg_subsc && len == 2 && name[1] == ':')
			{
			    ವಿಕಲ್ಪ (*name)
			    {
				ಪಕ್ಷ 'g': list_glob_vars(first); ಭಙ್ಗ;
				ಪಕ್ಷ 'b': list_buf_vars(first); ಭಙ್ಗ;
				ಪಕ್ಷ 'w': list_win_vars(first); ಭಙ್ಗ;
				ಪಕ್ಷ 't': list_tab_vars(first); ಭಙ್ಗ;
				ಪಕ್ಷ 'v': list_vim_vars(first); ಭಙ್ಗ;
				ಪಕ್ಷ 's': list_script_vars(first); ಭಙ್ಗ;
				ಪಕ್ಷ 'l': list_func_vars(first); ಭಙ್ಗ;
				ಸ್ವಭಾವ:
					  semsg(_(e_cant_list_variables_for_str), name);
			    }
			}
			ಅನ್ಯಥಾ
			{
			    char_u	numbuf[NUMBUFLEN];
			    char_u	*tf;
			    ಪೂರ್ಣಾಙ್ಕ		c;
			    char_u	*s;

			    s = echo_string(&tv, &tf, numbuf, 0);
			    c = *arg;
			    *arg = NUL;
			    list_one_var_a("",
				    arg == arg_subsc ? name : name_start,
				    tv.v_type,
				    s == ರಿಕ್ತ ? (char_u *)"" : s,
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

    ಪ್ರತ್ಯಾಗಮ arg;
}

/*
 * Set an environment variable, part of ex_let_one().
 */
    ಸ್ಥಿರ char_u *
ex_let_env(
    char_u	*arg,
    typval_T	*tv,
    ಪೂರ್ಣಾಙ್ಕ		flags,
    char_u	*endchars,
    char_u	*op)
{
    char_u	*arg_end = ರಿಕ್ತ;
    char_u	*name;
    ಪೂರ್ಣಾಙ್ಕ		len;

    ಯದಿ ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					 && (flags & ASSIGN_FOR_LOOP) == 0)
    {
	emsg(_(e_cannot_lock_environment_variable));
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    // Find the end of the name.
    ++arg;
    name = arg;
    len = get_env_len(&arg);
    ಯದಿ (len == 0)
	semsg(_(e_invalid_argument_str), name - 1);
    ಅನ್ಯಥಾ
    {
	ಯದಿ (op != ರಿಕ್ತ && vim_strchr((char_u *)"+-*/%", *op) != ರಿಕ್ತ)
	    semsg(_(e_wrong_variable_type_for_str_equal), op);
	ಅನ್ಯಥಾ ಯದಿ (endchars != ರಿಕ್ತ
			      && vim_strchr(endchars, *skipwhite(arg)) == ರಿಕ್ತ)
	    emsg(_(e_unexpected_characters_in_let));
	ಅನ್ಯಥಾ ಯದಿ (!check_secure() && !check_restricted())
	{
	    char_u	*tofree = ರಿಕ್ತ;
	    ಪೂರ್ಣಾಙ್ಕ		c1 = name[len];
	    char_u	*p;

	    name[len] = NUL;
	    p = tv_get_string_chk(tv);
	    ಯದಿ (p != ರಿಕ್ತ && op != ರಿಕ್ತ && *op == '.')
	    {
		ಪೂರ್ಣಾಙ್ಕ	mustfree = FALSE;
		char_u  *s = vim_getenv(name, &mustfree);

		ಯದಿ (s != ರಿಕ್ತ)
		{
		    p = tofree = concat_str(s, p);
		    ಯದಿ (mustfree)
			vim_free(s);
		}
	    }
	    ಯದಿ (p != ರಿಕ್ತ)
	    {
		vim_setenv_ext(name, p);
		arg_end = arg;
	    }
	    name[len] = c1;
	    vim_free(tofree);
	}
    }
    ಪ್ರತ್ಯಾಗಮ arg_end;
}

/*
 * Set an option, part of ex_let_one().
 */
    ಸ್ಥಿರ char_u *
ex_let_option(
    char_u	*arg,
    typval_T	*tv,
    ಪೂರ್ಣಾಙ್ಕ		flags,
    char_u	*endchars,
    char_u	*op)
{
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		scope;
    char_u	*arg_end = ರಿಕ್ತ;

    ಯದಿ ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					 && (flags & ASSIGN_FOR_LOOP) == 0)
    {
	emsg(_(e_cannot_lock_option));
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    // Find the end of the name.
    p = find_option_end(&arg, &scope);
    ಯದಿ (p == ರಿಕ್ತ || (endchars != ರಿಕ್ತ
			       && vim_strchr(endchars, *skipwhite(p)) == ರಿಕ್ತ))
    {
	emsg(_(e_unexpected_characters_in_let));
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಪೂರ್ಣಾಙ್ಕ		c1;
    long	n = 0;
    getoption_T	opt_type;
    long	numval;
    char_u	*stringval = ರಿಕ್ತ;
    char_u	*s = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		failed = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		opt_p_flags;
    char_u	*tofree = ರಿಕ್ತ;
    char_u	numbuf[NUMBUFLEN];

    c1 = *p;
    *p = NUL;

    opt_type = get_option_ಮೌಲ್ಯ(arg, &numval, &stringval, &opt_p_flags, scope);
    ಯದಿ (opt_type == gov_unknown && arg[0] != 't' && arg[1] != '_')
    {
	semsg(_(e_unknown_option_str_2), arg);
	ಗಚ್ಛ theend;
    }
    ಯದಿ (op != ರಿಕ್ತ && *op != '='
	&& (((opt_type == gov_bool || opt_type == gov_ಸಂಖ್ಯೆ) && *op == '.')
	    || (opt_type == gov_string && *op != '.')))
    {
	semsg(_(e_wrong_variable_type_for_str_equal), op);
	ಗಚ್ಛ theend;
    }

    ಯದಿ ((opt_type == gov_bool
		|| opt_type == gov_ಸಂಖ್ಯೆ
		|| opt_type == gov_hidden_bool
		|| opt_type == gov_hidden_ಸಂಖ್ಯೆ)
		     && (tv->v_type != VAR_STRING || !in_vim9script()))
    {
	ಯದಿ (opt_type == gov_bool || opt_type == gov_hidden_bool)
	    // bool, possibly hidden
	    n = (long)tv_get_bool_chk(tv, &failed);
	ಅನ್ಯಥಾ
	    // number, possibly hidden
	    n = (long)tv_get_ಸಂಖ್ಯೆ_chk(tv, &failed);
	ಯದಿ (failed)
	    ಗಚ್ಛ theend;
    }

    ಯದಿ ((opt_p_flags & P_FUNC) && (tv->v_type == VAR_PARTIAL
						    || tv->v_type == VAR_FUNC))
    {
	// If the option can be set to a function reference or a lambda
	// and the passed value is a function reference, then convert it to
	// the name (string) of the function reference.
	s = tv2string(tv, &tofree, numbuf, 0);
	ಯದಿ (s == ರಿಕ್ತ)
	    ಗಚ್ಛ theend;
    }
    // Avoid setting a string option to the text "v:false" or similar.
    // In Vim9 script also don't convert a number to string.
    ಅನ್ಯಥಾ ಯದಿ (tv->v_type != VAR_BOOL && tv->v_type != VAR_SPECIAL
		     && (!in_vim9script() || tv->v_type != VAR_ಸಂಖ್ಯೆ))
    {
	s = tv_get_string_chk(tv);
	ಯದಿ (s == ರಿಕ್ತ)
	    ಗಚ್ಛ theend;
    }
    ಅನ್ಯಥಾ ಯದಿ (opt_type == gov_string || opt_type == gov_hidden_string)
    {
	emsg(_(e_string_required));
	ಗಚ್ಛ theend;
    }

    ಯದಿ (op != ರಿಕ್ತ && *op != '=')
    {
	// number, in legacy script also bool
	ಯದಿ (opt_type == gov_ಸಂಖ್ಯೆ
				 || (opt_type == gov_bool && !in_vim9script()))
	{
	    ವಿಕಲ್ಪ (*op)
	    {
		ಪಕ್ಷ '+': n = numval + n; ಭಙ್ಗ;
		ಪಕ್ಷ '-': n = numval - n; ಭಙ್ಗ;
		ಪಕ್ಷ '*': n = numval * n; ಭಙ್ಗ;
		ಪಕ್ಷ '/': n = (long)ಅಂಕೆ_divide(numval, n, &failed); ಭಙ್ಗ;
		ಪಕ್ಷ '%': n = (long)ಅಂಕೆ_modulus(numval, n, &failed); ಭಙ್ಗ;
	    }
	    s = ರಿಕ್ತ;
	    ಯದಿ (failed)
		ಗಚ್ಛ theend;
	}
	ಅನ್ಯಥಾ ಯದಿ (opt_type == gov_string && stringval != ರಿಕ್ತ && s != ರಿಕ್ತ)
	{
	    // string
	    s = concat_str(stringval, s);
	    vim_free(stringval);
	    stringval = s;
	}
    }

    ವರ್ಣ *err = set_option_ಮೌಲ್ಯ(arg, n, s, scope);
    arg_end = p;
    ಯದಿ (err != ರಿಕ್ತ)
	emsg(_(err));

theend:
    *p = c1;
    vim_free(stringval);
    vim_free(tofree);
    ಪ್ರತ್ಯಾಗಮ arg_end;
}

/*
 * Set a register, part of ex_let_one().
 */
    ಸ್ಥಿರ char_u *
ex_let_register(
    char_u	*arg,
    typval_T	*tv,
    ಪೂರ್ಣಾಙ್ಕ		flags,
    char_u	*endchars,
    char_u	*op)
{
    char_u	*arg_end = ರಿಕ್ತ;

    ಯದಿ ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					 && (flags & ASSIGN_FOR_LOOP) == 0)
    {
	emsg(_(e_cannot_lock_register));
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ++arg;
    ಯದಿ (op != ರಿಕ್ತ && vim_strchr((char_u *)"+-*/%", *op) != ರಿಕ್ತ)
	semsg(_(e_wrong_variable_type_for_str_equal), op);
    ಅನ್ಯಥಾ ಯದಿ (endchars != ರಿಕ್ತ
			  && vim_strchr(endchars, *skipwhite(arg + 1)) == ರಿಕ್ತ)
	emsg(_(e_unexpected_characters_in_let));
    ಅನ್ಯಥಾ
    {
	char_u	*ptofree = ರಿಕ್ತ;
	char_u	*p;

	p = tv_get_string_chk(tv);
	ಯದಿ (p != ರಿಕ್ತ && op != ರಿಕ್ತ && *op == '.')
	{
	    char_u  *s = get_reg_contents(*arg == '@'
						  ? '"' : *arg, GREG_ಅಭಿವ್ಯಕ್ತಿ_SRC);

	    ಯದಿ (s != ರಿಕ್ತ)
	    {
		p = ptofree = concat_str(s, p);
		vim_free(s);
	    }
	}
	ಯದಿ (p != ರಿಕ್ತ)
	{
	    write_reg_contents(*arg == '@' ? '"' : *arg, p, -1, FALSE);
	    arg_end = arg + 1;
	}
	vim_free(ptofree);
    }
    ಪ್ರತ್ಯಾಗಮ arg_end;
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
    ಪೂರ್ಣಾಙ್ಕ		copy,		// copy value from "tv"
    ಪೂರ್ಣಾಙ್ಕ		flags,		// ASSIGN_CONST, ASSIGN_FINAL, etc.
    char_u	*endchars,	// valid chars after variable name  or NULL
    char_u	*op,		// "+", "-", "."  or NULL
    ಪೂರ್ಣಾಙ್ಕ		var_idx)	// variable index for "let [a, b] = list"
{
    char_u	*arg_end = ರಿಕ್ತ;

    ಯದಿ (in_vim9script() && (flags & (ASSIGN_NO_DECL | ASSIGN_DECL)) == 0
			&& (flags & (ASSIGN_CONST | ASSIGN_FINAL)) == 0
				  && vim_strchr((char_u *)"$@&", *arg) != ರಿಕ್ತ)
    {
	vim9_declare_error(arg);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಯದಿ (check_typval_is_ಮೌಲ್ಯ(tv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    // In Vim9 script an environment variable and a register only take a
    // String.  "@#" also takes a buffer number.
    ಯದಿ (in_vim9script() && (*arg == '$' || *arg == '@')
	    && !(*arg == '@' && arg[1] == '#' && tv->v_type == VAR_ಸಂಖ್ಯೆ
					       && (op == ರಿಕ್ತ || *op == '=')))
    {
	where_T	where = WHERE_INIT;

	ಯದಿ (check_typval_type(&t_string, tv, where) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಯದಿ (*arg == '$')
    {
	// ":let $VAR = expr": Set environment variable.
	ಪ್ರತ್ಯಾಗಮ ex_let_env(arg, tv, flags, endchars, op);
    }
    ಅನ್ಯಥಾ ಯದಿ (*arg == '&')
    {
	// ":let &option = expr": Set option value.
	// ":let &l:option = expr": Set local option value.
	// ":let &g:option = expr": Set global option value.
	// ":for &ts in range(8)": Set option value for for loop
	ಪ್ರತ್ಯಾಗಮ ex_let_option(arg, tv, flags, endchars, op);
    }
    ಅನ್ಯಥಾ ಯದಿ (*arg == '@')
    {
	// ":let @r = expr": Set register contents.
	ಪ್ರತ್ಯಾಗಮ ex_let_register(arg, tv, flags, endchars, op);
    }
    ಅನ್ಯಥಾ ಯದಿ (eval_isnamec1(*arg) || *arg == '{')
    {
	lval_T	lv;
	char_u	*p;
	ಪೂರ್ಣಾಙ್ಕ	lval_flags = (flags & (ASSIGN_NO_DECL | ASSIGN_DECL))
							     ? GLV_NO_DECL : 0;
	lval_flags |= (flags & ASSIGN_FOR_LOOP) ? GLV_FOR_LOOP : 0;
	ಯದಿ (op != ರಿಕ್ತ && *op != '=')
	    lval_flags |= GLV_ASSIGN_WITH_OP;

	// ":let var = expr": Set internal variable.
	// ":let var: type = expr": Set internal variable with type.
	// ":let {expr} = expr": Idem, name made with curly braces
	p = get_lval(arg, tv, &lv, FALSE, FALSE, lval_flags, FNE_CHECK_START);
	ಯದಿ (p != ರಿಕ್ತ && lv.ll_name != ರಿಕ್ತ)
	{
	    ಯದಿ (endchars != ರಿಕ್ತ && vim_strchr(endchars,
					   *skipwhite(lv.ll_name_end)) == ರಿಕ್ತ)
	    {
		emsg(_(e_unexpected_characters_in_let));
	    }
	    ಅನ್ಯಥಾ
	    {
		set_var_lval(&lv, p, tv, copy, flags, op, var_idx);
		arg_end = lv.ll_name_end;
	    }
	}
	clear_lval(&lv);
    }
    ಅನ್ಯಥಾ
	semsg(_(e_invalid_argument_str), arg);

    ಪ್ರತ್ಯಾಗಮ arg_end;
}

/*
 * ":unlet[!] var1 ... " command.
 */
    ಶೂನ್ಯ
ex_unlet(exarg_T *eap)
{
    ex_unletlock(eap, eap->arg, 0, eap->forceit ? GLV_QUIET : 0, do_unlet_var, ರಿಕ್ತ);
}

/*
 * ":lockvar" and ":unlockvar" commands
 */
    ಶೂನ್ಯ
ex_lockvar(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    ಪೂರ್ಣಾಙ್ಕ		deep = 2;

    ಯದಿ (eap->forceit)
	deep = -1;
    ಅನ್ಯಥಾ ಯದಿ (vim_isdigit(*arg))
    {
	deep = getdigits(&arg);
	arg = skipwhite(arg);
    }

    ex_unletlock(eap, arg, deep, 0, do_lock_var, ರಿಕ್ತ);
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
    ಪೂರ್ಣಾಙ್ಕ		deep,
    ಪೂರ್ಣಾಙ್ಕ		glv_flags,
    ಪೂರ್ಣಾಙ್ಕ		(*callback)(lval_T *, char_u *, exarg_T *, ಪೂರ್ಣಾಙ್ಕ, ಶೂನ್ಯ *),
    ಶೂನ್ಯ	*cookie)
{
    char_u	*arg = argstart;
    char_u	*name_end;
    ಪೂರ್ಣಾಙ್ಕ		error = FALSE;
    lval_T	lv;

    ಕುರು
    {
	ಯದಿ (*arg == '$')
	{
	    lv.ll_name = arg;
	    lv.ll_tv = ರಿಕ್ತ;
	    ++arg;
	    ಯದಿ (get_env_len(&arg) == 0)
	    {
		semsg(_(e_invalid_argument_str), arg - 1);
		ಪ್ರತ್ಯಾಗಮ;
	    }
	    ಯದಿ (!error && !eap->skip
			      && callback(&lv, arg, eap, deep, cookie) == FAIL)
		error = TRUE;
	    name_end = arg;
	}
	ಅನ್ಯಥಾ
	{
	    // Parse the name and find the end.
	    name_end = get_lval(arg, ರಿಕ್ತ, &lv, TRUE, eap->skip || error,
				     glv_flags | GLV_NO_DECL, FNE_CHECK_START);
	    ಯದಿ (lv.ll_name == ರಿಕ್ತ)
		error = TRUE;	    // error but continue parsing
	    ಯದಿ (name_end == ರಿಕ್ತ || (!VIM_ISWHITE(*name_end)
						    && !ends_excmd(*name_end)))
	    {
		ಯದಿ (name_end != ರಿಕ್ತ)
		{
		    emsg_severe = TRUE;
		    semsg(_(e_trailing_characters_str), name_end);
		}
		ಯದಿ (!(eap->skip || error))
		    clear_lval(&lv);
		ಭಙ್ಗ;
	    }

	    ಯದಿ (!error && !eap->skip
			 && callback(&lv, name_end, eap, deep, cookie) == FAIL)
		error = TRUE;

	    ಯದಿ (!eap->skip)
		clear_lval(&lv);
	}

	arg = skipwhite(name_end);
    } ಯಾವತ್ (!ends_excmd2(name_end, arg));

    set_nextcmd(eap, arg);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
do_unlet_var(
    lval_T	*lp,
    char_u	*name_end,
    exarg_T	*eap,
    ಪೂರ್ಣಾಙ್ಕ		deep UNUSED,
    ಶೂನ್ಯ	*cookie UNUSED)
{
    ಪೂರ್ಣಾಙ್ಕ		forceit = eap->forceit;
    ಪೂರ್ಣಾಙ್ಕ		ret = OK;
    ಪೂರ್ಣಾಙ್ಕ		cc;

    ಯದಿ (lp->ll_tv == ರಿಕ್ತ)
    {
	cc = *name_end;
	*name_end = NUL;

	// Environment variable, normal name or expanded name.
	ಯದಿ (*lp->ll_name == '$')
	    vim_unsetenv_ext(lp->ll_name + 1);
	ಅನ್ಯಥಾ ಯದಿ (do_unlet(lp->ll_name, forceit) == FAIL)
	    ret = FAIL;
	*name_end = cc;
    }
    ಅನ್ಯಥಾ ಯದಿ ((lp->ll_list != ರಿಕ್ತ
		 && ಮೌಲ್ಯ_check_lock(lp->ll_list->lv_lock, lp->ll_name, FALSE))
	    || (lp->ll_dict != ರಿಕ್ತ
		&& ಮೌಲ್ಯ_check_lock(lp->ll_dict->dv_lock, lp->ll_name, FALSE)))
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಅನ್ಯಥಾ ಯದಿ (lp->ll_range)
	list_unlet_range(lp->ll_list, lp->ll_li, lp->ll_n1,
						    !lp->ll_empty2, lp->ll_n2);
    ಅನ್ಯಥಾ ಯದಿ (lp->ll_list != ರಿಕ್ತ)
	// unlet a List item.
	listitem_remove(lp->ll_list, lp->ll_li);
    ಅನ್ಯಥಾ ಯದಿ (lp->ll_dict != ರಿಕ್ತ)
	// unlet a Dictionary item.
	dictitem_remove(lp->ll_dict, lp->ll_di, "unlet");
    ಅನ್ಯಥಾ
    {
	semsg(_(e_cannot_unlet_imported_item_str), lp->ll_name);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Unlet one item or a range of items from a list.
 * Return OK or FAIL.
 */
    ಶೂನ್ಯ
list_unlet_range(
	list_T	    *l,
	listitem_T  *li_first,
	long	    n1_arg,
	ಪೂರ್ಣಾಙ್ಕ	    has_n2,
	long	    n2)
{
    listitem_T  *li = li_first;
    ಪೂರ್ಣಾಙ್ಕ		n1 = n1_arg;

    // Delete a range of List items.
    li = li_first;
    n1 = n1_arg;
    ಯಾವತ್ (li != ರಿಕ್ತ && (!has_n2 || n2 >= n1))
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
    ಪೂರ್ಣಾಙ್ಕ
do_unlet(char_u *name, ಪೂರ್ಣಾಙ್ಕ forceit)
{
    hashtab_T	*ht;
    hashitem_T	*hi;
    char_u	*varname = ರಿಕ್ತ;  // init to shut up gcc
    dict_T	*d;
    dictitem_T	*di;

    // can't :unlet a script variable in Vim9 script
    ಯದಿ (in_vim9script() && check_vim9_unlet(name) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ht = find_var_ht(name, &varname);

    // can't :unlet a script variable in Vim9 script from a function
    ಯದಿ (ht == get_script_local_ht()
	    && SCRIPT_ID_VALID(current_sctx.sc_sid)
	    && SCRIPT_ITEM(current_sctx.sc_sid)->sn_version
							 == SCRIPT_VERSION_VIM9
	    && check_vim9_unlet(name) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (ht != ರಿಕ್ತ && *varname != NUL)
    {
	d = get_current_funccal_dict(ht);
	ಯದಿ (d == ರಿಕ್ತ)
	{
	    ಯದಿ (ht == &globvarht)
		d = &globvardict;
	    ಅನ್ಯಥಾ ಯದಿ (ht == &compat_hashtab)
		d = &vimvardict;
	    ಅನ್ಯಥಾ
	    {
		di = find_var_in_ht(ht, *name, (char_u *)"", FALSE);
		d = di == ರಿಕ್ತ ? ರಿಕ್ತ : di->di_tv.vval.v_dict;
	    }
	    ಯದಿ (d == ರಿಕ್ತ)
	    {
		internal_error("do_unlet()");
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	}
	hi = hash_find(ht, varname);
	ಯದಿ (HASHITEM_EMPTY(hi))
	    hi = find_hi_in_scoped_ht(name, &ht);
	ಯದಿ (hi != ರಿಕ್ತ && !HASHITEM_EMPTY(hi))
	{
	    di = HI2DI(hi);
	    ಯದಿ (var_check_fixed(di->di_flags, name, FALSE)
		    || var_check_ro(di->di_flags, name, FALSE)
		    || ಮೌಲ್ಯ_check_lock(d->dv_lock, name, FALSE)
		    || check_hashtab_frozen(ht, "unlet"))
		ಪ್ರತ್ಯಾಗಮ FAIL;

	    delete_var(ht, hi);
	    ಪ್ರತ್ಯಾಗಮ OK;
	}
    }
    ಯದಿ (forceit)
	ಪ್ರತ್ಯಾಗಮ OK;
    semsg(_(e_no_such_variable_str), name);
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
report_lockvar_member(ವರ್ಣ *ಸಂದೇಶ, lval_T *lp)
{
    ಪೂರ್ಣಾಙ್ಕ did_alloc = FALSE;
    char_u *vname = (char_u *)"";
    char_u *class_name = lp->ll_class != ರಿಕ್ತ
				    ? lp->ll_class->class_name.string : (char_u *)"";
    ಯದಿ (lp->ll_name != ರಿಕ್ತ)
    {
	ಯದಿ (lp->ll_name_end == ರಿಕ್ತ)
	    vname = lp->ll_name;
	ಅನ್ಯಥಾ
	{
	    vname = vim_strnsave(lp->ll_name, lp->ll_name_end - lp->ll_name);
	    ಯದಿ (vname == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ;
	    did_alloc = TRUE;
	}
    }
    semsg(_(ಸಂದೇಶ), vname, class_name);
    ಯದಿ (did_alloc)
	vim_free(vname);
}

/*
 * Lock or unlock variable indicated by "lp".
 * "deep" is the levels to go (-1 for unlimited);
 * "lock" is TRUE for ":lockvar", FALSE for ":unlockvar".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
do_lock_var(
    lval_T	*lp,
    char_u	*name_end,
    exarg_T	*eap,
    ಪೂರ್ಣಾಙ್ಕ		deep,
    ಶೂನ್ಯ	*cookie UNUSED)
{
    ಪೂರ್ಣಾಙ್ಕ		lock = eap->cmdidx == CMD_lockvar;
    ಪೂರ್ಣಾಙ್ಕ		ret = OK;
    ಪೂರ್ಣಾಙ್ಕ		cc;
    dictitem_T	*di;

#ifdef LOG_LOCKVAR
    ch_log(ರಿಕ್ತ, "LKVAR: do_lock_var(): name %s, is_root %d", lp->ll_name, lp->ll_is_ಬೇರು);
#endif

    ಯದಿ (lp->ll_tv == ರಿಕ್ತ)
    {
	cc = *name_end;
	*name_end = NUL;
	ಯದಿ (*lp->ll_name == '$')
	{
	    semsg(_(e_cannot_lock_or_unlock_variable_str), lp->ll_name);
	    ret = FAIL;
	}
	ಅನ್ಯಥಾ
	{
	    // Normal name or expanded name.
	    di = find_var(lp->ll_name, ರಿಕ್ತ, TRUE);
	    ಯದಿ (di == ರಿಕ್ತ)
	    {
		ಯದಿ (in_vim9script())
		    semsg(_(e_cannot_find_variable_to_unlock_str),
								  lp->ll_name);
		ret = FAIL;
	    }
	    ಅನ್ಯಥಾ
	    {
		ಯದಿ ((di->di_flags & DI_FLAGS_FIX)
			    && di->di_tv.v_type != VAR_DICT
			    && di->di_tv.v_type != VAR_LIST)
		{
		    // For historic reasons this error is not given for a list
		    // or dict.  E.g., the b: dict could be locked/unlocked.
		    semsg(_(e_cannot_lock_or_unlock_variable_str), lp->ll_name);
		    ret = FAIL;
		}
		ಅನ್ಯಥಾ
		{
		    ಯದಿ (in_vim9script())
		    {
			svar_T  *sv = find_typval_in_script(&di->di_tv,
								     0, FALSE);

			ಯದಿ (sv != ರಿಕ್ತ && sv->sv_const != 0)
			{
			    semsg(_(e_cannot_change_readonly_variable_str),
								  lp->ll_name);
			    ret = FAIL;
			}
		    }

		    ಯದಿ (ret == OK)
		    {
			ಯದಿ (lock)
			    di->di_flags |= DI_FLAGS_LOCK;
			ಅನ್ಯಥಾ
			    di->di_flags &= ~DI_FLAGS_LOCK;
			ಯದಿ (deep != 0)
			    item_lock(&di->di_tv, deep, lock, FALSE);
		    }
		}
	    }
	}
	*name_end = cc;
    }
    ಅನ್ಯಥಾ ಯದಿ (deep == 0 && lp->ll_object == ರಿಕ್ತ && lp->ll_class == ರಿಕ್ತ)
    {
	// nothing to do
    }
    ಅನ್ಯಥಾ ಯದಿ (lp->ll_is_ಬೇರು)
	// (un)lock the item.
	item_lock(lp->ll_tv, deep, lock, FALSE);
    ಅನ್ಯಥಾ ಯದಿ (lp->ll_range)
    {
	listitem_T    *li = lp->ll_li;

	// (un)lock a range of List items.
	ಯಾವತ್ (li != ರಿಕ್ತ && (lp->ll_empty2 || lp->ll_n2 >= lp->ll_n1))
	{
	    item_lock(&li->li_tv, deep, lock, FALSE);
	    li = li->li_next;
	    ++lp->ll_n1;
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (lp->ll_list != ರಿಕ್ತ)
	// (un)lock a List item.
	item_lock(&lp->ll_li->li_tv, deep, lock, FALSE);
    ಅನ್ಯಥಾ ಯದಿ (lp->ll_object != ರಿಕ್ತ)  // This check must be before ll_class.
    {
	// (un)lock an object variable.
	report_lockvar_member(e_cannot_lock_object_variable_str, lp);
	ret = FAIL;
    }
    ಅನ್ಯಥಾ ಯದಿ (lp->ll_class != ರಿಕ್ತ)
    {
	// (un)lock a class variable.
	report_lockvar_member(e_cannot_lock_class_variable_str, lp);
	ret = FAIL;
    }
    ಅನ್ಯಥಾ
    {
	// (un)lock a Dictionary item.
	ಯದಿ (lp->ll_di == ರಿಕ್ತ)
	{
	    emsg(_(e_dictionary_required));
	    ret = FAIL;
	}
	ಅನ್ಯಥಾ
	    item_lock(&lp->ll_di->di_tv, deep, lock, FALSE);
    }

    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Lock or unlock an item.  "deep" is nr of levels to go.
 * When "check_refcount" is TRUE do not lock a list or dict with a reference
 * count larger than 1.
 */
    ಶೂನ್ಯ
item_lock(typval_T *tv, ಪೂರ್ಣಾಙ್ಕ deep, ಪೂರ್ಣಾಙ್ಕ lock, ಪೂರ್ಣಾಙ್ಕ check_refcount)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	recurse = 0;
    list_T	*l;
    listitem_T	*li;
    dict_T	*d;
    blob_T	*b;
    hashitem_T	*hi;
    ಪೂರ್ಣಾಙ್ಕ		todo;

#ifdef LOG_LOCKVAR
    ch_log(ರಿಕ್ತ, "LKVAR: item_lock(): type %s", vartype_name(tv->v_type));
#endif

    ಯದಿ (recurse >= DICT_MAXNEST)
    {
	emsg(_(e_variable_nested_too_deep_for_unlock));
	ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (deep == 0)
	ಪ್ರತ್ಯಾಗಮ;
    ++recurse;

    // lock/unlock the item itself
    ಯದಿ (lock)
	tv->v_lock |= VAR_LOCKED;
    ಅನ್ಯಥಾ
	tv->v_lock &= ~VAR_LOCKED;

    ವಿಕಲ್ಪ (tv->v_type)
    {
	ಪಕ್ಷ VAR_UNKNOWN:
	ಪಕ್ಷ VAR_ANY:
	ಪಕ್ಷ VAR_VOID:
	ಪಕ್ಷ VAR_ಸಂಖ್ಯೆ:
	ಪಕ್ಷ VAR_BOOL:
	ಪಕ್ಷ VAR_STRING:
	ಪಕ್ಷ VAR_FUNC:
	ಪಕ್ಷ VAR_PARTIAL:
	ಪಕ್ಷ VAR_FLOAT:
	ಪಕ್ಷ VAR_SPECIAL:
	ಪಕ್ಷ VAR_JOB:
	ಪಕ್ಷ VAR_CHANNEL:
	ಪಕ್ಷ VAR_INSTR:
	ಪಕ್ಷ VAR_CLASS:
	ಪಕ್ಷ VAR_OBJECT:
	ಪಕ್ಷ VAR_TYPEALIAS:
	    ಭಙ್ಗ;

	ಪಕ್ಷ VAR_BLOB:
	    ಯದಿ ((b = tv->vval.v_blob) != ರಿಕ್ತ
				    && !(check_refcount && b->bv_refcount > 1))
	    {
		ಯದಿ (lock)
		    b->bv_lock |= VAR_LOCKED;
		ಅನ್ಯಥಾ
		    b->bv_lock &= ~VAR_LOCKED;
	    }
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_LIST:
	    ಯದಿ ((l = tv->vval.v_list) != ರಿಕ್ತ
				    && !(check_refcount && l->lv_refcount > 1))
	    {
		ಯದಿ (lock)
		    l->lv_lock |= VAR_LOCKED;
		ಅನ್ಯಥಾ
		    l->lv_lock &= ~VAR_LOCKED;
		ಯದಿ (deep < 0 || deep > 1)
		{
		    ಯದಿ (l->lv_first == &range_list_item)
			l->lv_lock |= VAR_ವಸ್ತುಗಳು_LOCKED;
		    ಅನ್ಯಥಾ
		    {
			// recursive: lock/unlock the items the List contains
			CHECK_LIST_MATERIALIZE(l);
			FOR_ALL_LIST_ವಸ್ತುಗಳು(l, li) item_lock(&li->li_tv,
					       deep - 1, lock, check_refcount);
		    }
		}
	    }
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_TUPLE:
	    tuple_lock(tv->vval.v_tuple, deep, lock, check_refcount);
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_DICT:
	    ಯದಿ ((d = tv->vval.v_dict) != ರಿಕ್ತ
				    && !(check_refcount && d->dv_refcount > 1))
	    {
		ಯದಿ (lock)
		    d->dv_lock |= VAR_LOCKED;
		ಅನ್ಯಥಾ
		    d->dv_lock &= ~VAR_LOCKED;
		ಯದಿ (deep < 0 || deep > 1)
		{
		    // recursive: lock/unlock the items the List contains
		    todo = (ಪೂರ್ಣಾಙ್ಕ)d->dv_hashtab.ht_used;
		    FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(&d->dv_hashtab, hi, todo)
		    {
			ಯದಿ (!HASHITEM_EMPTY(hi))
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

#ಯದಿ defined(FEAT_MENU) && defined(FEAT_MULTI_LANG)
/*
 * Delete all "menutrans_" variables.
 */
    ಶೂನ್ಯ
del_menutrans_vars(ಶೂನ್ಯ)
{
    hashitem_T	*hi;
    ಪೂರ್ಣಾಙ್ಕ		todo;

    hash_lock(&globvarht);
    todo = (ಪೂರ್ಣಾಙ್ಕ)globvarht.ht_used;
    ಪ್ರತಿ (hi = globvarht.ht_array; todo > 0 && !got_int; ++hi)
    {
	ಯದಿ (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    ಯದಿ (STRNCMP(HI2DI(hi)->di_ಕೀಲಿ, "menutrans_", 10) == 0)
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

ಸ್ಥಿರ char_u	*varnamebuf = ರಿಕ್ತ;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	varnamebuflen = 0;

/*
 * Function to concatenate a prefix and a variable name.
 */
    char_u *
cat_prefix_varname(ಪೂರ್ಣಾಙ್ಕ prefix, char_u *name)
{
    ಪೂರ್ಣಾಙ್ಕ		len;

    len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(name) + 3;
    ಯದಿ (len > varnamebuflen)
    {
	vim_free(varnamebuf);
	len += 10;			// some additional space
	varnamebuf = alloc(len);
	ಯದಿ (varnamebuf == ರಿಕ್ತ)
	{
	    varnamebuflen = 0;
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
	varnamebuflen = len;
    }
    *varnamebuf = prefix;
    varnamebuf[1] = ':';
    STRCPY(varnamebuf + 2, name);
    ಪ್ರತ್ಯಾಗಮ varnamebuf;
}

/*
 * Function given to ExpandGeneric() to obtain the list of user defined
 * (global/buffer/window/built-in) variable names.
 */
    char_u *
get_user_var_name(expand_T *xp, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಸ್ಥಿರ long_u	gdone;
    ಸ್ಥಿರ long_u	bdone;
    ಸ್ಥಿರ long_u	wdone;
    ಸ್ಥಿರ long_u	tdone;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ		vidx;
    ಸ್ಥಿರ hashitem_T	*hi;
    hashtab_T		*ht;

    ಯದಿ (idx == 0)
    {
	gdone = bdone = wdone = vidx = 0;
	tdone = 0;
    }

    // Global variables
    ಯದಿ (gdone < globvarht.ht_used)
    {
	ಯದಿ (gdone++ == 0)
	    hi = globvarht.ht_array;
	ಅನ್ಯಥಾ
	    ++hi;
	ಯಾವತ್ (HASHITEM_EMPTY(hi))
	    ++hi;
	ಯದಿ (STRNCMP("g:", xp->xp_pattern, 2) == 0)
	    ಪ್ರತ್ಯಾಗಮ cat_prefix_varname('g', hi->hi_ಕೀಲಿ);
	ಪ್ರತ್ಯಾಗಮ hi->hi_ಕೀಲಿ;
    }

    // b: variables
    ht = &prevwin_curwin()->w_buffer->b_vars->dv_hashtab;
    ಯದಿ (bdone < ht->ht_used)
    {
	ಯದಿ (bdone++ == 0)
	    hi = ht->ht_array;
	ಅನ್ಯಥಾ
	    ++hi;
	ಯಾವತ್ (HASHITEM_EMPTY(hi))
	    ++hi;
	ಪ್ರತ್ಯಾಗಮ cat_prefix_varname('b', hi->hi_ಕೀಲಿ);
    }

    // w: variables
    ht = &prevwin_curwin()->w_vars->dv_hashtab;
    ಯದಿ (wdone < ht->ht_used)
    {
	ಯದಿ (wdone++ == 0)
	    hi = ht->ht_array;
	ಅನ್ಯಥಾ
	    ++hi;
	ಯಾವತ್ (HASHITEM_EMPTY(hi))
	    ++hi;
	ಪ್ರತ್ಯಾಗಮ cat_prefix_varname('w', hi->hi_ಕೀಲಿ);
    }

    // t: variables
    ht = &curtab->tp_vars->dv_hashtab;
    ಯದಿ (tdone < ht->ht_used)
    {
	ಯದಿ (tdone++ == 0)
	    hi = ht->ht_array;
	ಅನ್ಯಥಾ
	    ++hi;
	ಯಾವತ್ (HASHITEM_EMPTY(hi))
	    ++hi;
	ಪ್ರತ್ಯಾಗಮ cat_prefix_varname('t', hi->hi_ಕೀಲಿ);
    }

    // v: variables
    ಯದಿ (vidx < VV_LEN)
	ಪ್ರತ್ಯಾಗಮ cat_prefix_varname('v', (char_u *)vimvars[vidx++].vv_name);

    VIM_CLEAR(varnamebuf);
    varnamebuflen = 0;
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    ವರ್ಣ *
get_var_special_name(ಪೂರ್ಣಾಙ್ಕ nr)
{
    ವಿಕಲ್ಪ (nr)
    {
	ಪಕ್ಷ VVAL_FALSE: ಪ್ರತ್ಯಾಗಮ in_vim9script() ? "false" : "v:false";
	ಪಕ್ಷ VVAL_TRUE:  ಪ್ರತ್ಯಾಗಮ in_vim9script() ? "true" : "v:true";
	ಪಕ್ಷ VVAL_NULL:  ಪ್ರತ್ಯಾಗಮ in_vim9script() ? "null" : "v:null";
	ಪಕ್ಷ VVAL_NONE:  ಪ್ರತ್ಯಾಗಮ "v:none";
    }
    internal_error("get_var_special_name()");
    ಪ್ರತ್ಯಾಗಮ "42";
}

/*
 * Returns the global variable dictionary
 */
    dict_T *
get_globvar_dict(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ &globvardict;
}

/*
 * Returns the global variable hash table
 */
    hashtab_T *
get_globvar_ht(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ &globvarht;
}

/*
 * Returns the v: variable dictionary
 */
    dict_T *
get_vimvar_dict(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ &vimvardict;
}

/*
 * Returns the index of a v:variable.  Negative if not found.
 * Returns DI_ flags in "di_flags".
 */
    ಪೂರ್ಣಾಙ್ಕ
find_vim_var(char_u *name, ಪೂರ್ಣಾಙ್ಕ *di_flags)
{
    dictitem_T	    *di = find_var_in_ht(&vimvarht, 0, name, TRUE);
    ಸಂರಚನಾ vimvar   *vv;

    ಯದಿ (di == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ -1;
    *di_flags = di->di_flags;
    vv = (ಸಂರಚನಾ vimvar *)((ವರ್ಣ *)di - offsetof(vimvar_T, vv_di));
    ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)(vv - vimvars);
}


/*
 * Set type of v: variable to "type".
 */
    ಶೂನ್ಯ
set_vim_var_type(ಪೂರ್ಣಾಙ್ಕ idx, vartype_T type)
{
    vimvars[idx].vv_tv_type = type;
}

/*
 * Set number v: variable to "val".
 * Note that this does not set the type, use set_vim_var_type() for that.
 */
    ಶೂನ್ಯ
set_vim_var_nr(ಪೂರ್ಣಾಙ್ಕ idx, varnumber_T val)
{
    vimvars[idx].vv_nr = val;
}

    ವರ್ಣ *
get_vim_var_name(ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಪ್ರತ್ಯಾಗಮ vimvars[idx].vv_name;
}

/*
 * Get typval_T v: variable value.
 */
    typval_T *
get_vim_var_tv(ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಪ್ರತ್ಯಾಗಮ &vimvars[idx].vv_tv;
}

    type_T *
get_vim_var_type(ಪೂರ್ಣಾಙ್ಕ idx, garray_T *type_list)
{
    ಯದಿ (vimvars[idx].vv_type != ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ vimvars[idx].vv_type;
    ಪ್ರತ್ಯಾಗಮ typval2type_vimvar(&vimvars[idx].vv_tv, type_list);
}

/*
 * Add what getinfo() reports for the v: variable "name", without the "v:", to
 * "d".  Returns FAIL when there is no such variable.
 */
    ಪೂರ್ಣಾಙ್ಕ
vim_var_info(char_u *name, dict_T *d)
{
    ಪೂರ್ಣಾಙ್ಕ		di_flags;
    ಪೂರ್ಣಾಙ್ಕ		idx = find_vim_var(name, &di_flags);
    garray_T	type_list;
    ವರ್ಣ	*tofree;

    ಯದಿ (idx < 0)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    vim_snprintf((ವರ್ಣ *)IObuff, IOSIZE, "v:%s", vimvars[idx].vv_name);
    dict_add_string(d, "name", IObuff);
    dict_add_bool(d, "available", TRUE);
    ga_init2(&type_list, ಪರಿಮಾಣ(type_T *), 10);
    ವರ್ಣ *type = type_name(get_vim_var_type(idx, &type_list), &tofree);
    dict_add_string(d, "type", (char_u *)type);
    vim_free(tofree);
    clear_type_list(&type_list);
    dict_add_bool(d, "readonly", (vimvars[idx].vv_flags & VV_RO) != 0);
    dict_add_bool(d, "compat", (vimvars[idx].vv_flags & VV_COMPAT) != 0);
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Set v: variable to "tv".  Only accepts the same type.
 * Takes over the value of "tv".
 */
    ಪೂರ್ಣಾಙ್ಕ
set_vim_var_tv(ಪೂರ್ಣಾಙ್ಕ idx, typval_T *tv)
{
    ಯದಿ (vimvars[idx].vv_tv_type != tv->v_type)
    {
	emsg(_(e_type_mismatch_for_v_variable));
	clear_tv(tv);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    // VV_RO is also checked when compiling, but let's check here as well.
    ಯದಿ (vimvars[idx].vv_flags & VV_RO)
    {
	semsg(_(e_cannot_change_readonly_variable_str), vimvars[idx].vv_name);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಯದಿ (sandbox && (vimvars[idx].vv_flags & VV_RO_SBX))
    {
	semsg(_(e_cannot_set_variable_in_sandbox_str), vimvars[idx].vv_name);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_di.di_tv = *tv;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Get number v: variable value.
 */
    varnumber_T
get_vim_var_nr(ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಪ್ರತ್ಯಾಗಮ vimvars[idx].vv_nr;
}

/*
 * Get string v: variable value.  Uses a static buffer, can only be used once.
 * If the String variable has never been set, return an empty string.
 * Never returns NULL;
 */
    char_u *
get_vim_var_str(ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಪ್ರತ್ಯಾಗಮ tv_get_string(&vimvars[idx].vv_tv);
}

/*
 * Get List v: variable value.  Caller must take care of reference count when
 * needed.
 */
    list_T *
get_vim_var_list(ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಪ್ರತ್ಯಾಗಮ vimvars[idx].vv_list;
}

/*
 * Get Dict v: variable value.  Caller must take care of reference count when
 * needed.
 */
    dict_T *
get_vim_var_dict(ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಪ್ರತ್ಯಾಗಮ vimvars[idx].vv_dict;
}

/*
 * Set v:char to character "c".
 */
    ಶೂನ್ಯ
set_vim_var_char(ಪೂರ್ಣಾಙ್ಕ c)
{
    char_u  buf[MB_MAXBYTES + 1];
    size_t  buflen;

    ಯದಿ (has_mbyte)
	buflen = (*mb_char2bytes)(c, buf);
    ಅನ್ಯಥಾ
    {
	buf[0] = c;
	buflen = 1;
    }
    buf[buflen] = NUL;

    set_vim_var_string(VV_CHAR, buf, (ಪೂರ್ಣಾಙ್ಕ)buflen);
}

/*
 * Set v:count to "count" and v:count1 to "count1".
 * When "set_prevcount" is TRUE first set v:prevcount from v:count.
 */
    ಶೂನ್ಯ
set_vcount(
    long	ಎಣಿಕೆ,
    long	count1,
    ಪೂರ್ಣಾಙ್ಕ		set_prevcount)
{
    ಯದಿ (set_prevcount)
	vimvars[VV_PREVCOUNT].vv_nr = vimvars[VV_ಎಣಿಕೆ].vv_nr;
    vimvars[VV_ಎಣಿಕೆ].vv_nr = ಎಣಿಕೆ;
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
set_vim_var_string(
    ಪೂರ್ಣಾಙ್ಕ		idx,
    char_u	*val,
    ಪೂರ್ಣಾಙ್ಕ		len)	    // length of "val" to use or -1 (whole string)
{
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_tv_type = VAR_STRING;
    ಯದಿ (val == ರಿಕ್ತ)
	vimvars[idx].vv_str = ರಿಕ್ತ;
    ಅನ್ಯಥಾ ಯದಿ (len == -1)
	vimvars[idx].vv_str = vim_strsave(val);
    ಅನ್ಯಥಾ
	vimvars[idx].vv_str = vim_strnsave(val, len);
}

    ಶೂನ್ಯ
set_vim_var_string_direct(
    ಪೂರ್ಣಾಙ್ಕ		idx,
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
set_vim_var_list(ಪೂರ್ಣಾಙ್ಕ idx, list_T *val)
{
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_tv_type = VAR_LIST;
    vimvars[idx].vv_list = val;
    ಯದಿ (val != ರಿಕ್ತ)
	++val->lv_refcount;
}

/*
 * Set Dictionary v: variable to "val".
 */
    ಶೂನ್ಯ
set_vim_var_dict(ಪೂರ್ಣಾಙ್ಕ idx, dict_T *val)
{
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_tv_type = VAR_DICT;
    vimvars[idx].vv_dict = val;
    ಯದಿ (val == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ++val->dv_refcount;
    dict_set_ವಸ್ತುಗಳು_ro(val);
}

/*
 * Set the v:argv list.
 */
    ಶೂನ್ಯ
set_argv_var(ವರ್ಣ **argv, ಪೂರ್ಣಾಙ್ಕ argc)
{
    list_T	*l = list_alloc();
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಯದಿ (l == ರಿಕ್ತ)
	getout(1);
    l->lv_lock = VAR_FIXED;
    ಪ್ರತಿ (i = 0; i < argc; ++i)
    {
	ಯದಿ (list_append_string(l, (char_u *)argv[i], -1) == FAIL)
	    getout(1);
	l->lv_u.mat.lv_last->li_tv.v_lock = VAR_FIXED;
    }
    set_vim_var_list(VV_ARGV, l);
}

/*
 * Reset v:register, taking the 'clipboard' setting into account.
 */
    ಶೂನ್ಯ
reset_reg_var(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ regname = 0;

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
    ಶೂನ್ಯ
set_reg_var(ಪೂರ್ಣಾಙ್ಕ c)
{
    char_u  regname[2];

    ಯದಿ (c == 0 || c == ' ')
	regname[0] = '"';
    ಅನ್ಯಥಾ
	regname[0] = c;
    regname[1] = NUL;
    // Avoid free/alloc when the value is already right.
    ಯದಿ (vimvars[VV_REG].vv_str == ರಿಕ್ತ || vimvars[VV_REG].vv_str[0] != c)
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
    ಯದಿ (oldval == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ vimvars[VV_EXCEPTION].vv_str;

    vimvars[VV_EXCEPTION].vv_str = oldval;
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
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
    ಯದಿ (oldval == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ vimvars[VV_THROWPOINT].vv_str;

    vimvars[VV_THROWPOINT].vv_str = oldval;
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
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
    ಯದಿ (eap == ರಿಕ್ತ)
    {
	vim_free(oldval);
	vimvars[VV_CMDARG].vv_str = oldarg;
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಯದಿ (eap->force_bin == FORCE_BIN)
	len = 6;
    ಅನ್ಯಥಾ ಯದಿ (eap->force_bin == FORCE_NOBIN)
	len = 8;
    ಅನ್ಯಥಾ
	len = 0;

    ಯದಿ (eap->read_edit)
	len += 7;

    ಯದಿ (eap->force_ff != 0)
	len += 10; // " ++ff=unix"
    ಯದಿ (eap->force_enc != 0)
	len += (unsigned)STRLEN(eap->cmd + eap->force_enc) + 7;
    ಯದಿ (eap->bad_char != 0)
	len += 7 + 4;  // " ++bad=" + "keep" or "drop"

    newval = alloc(len + 1);
    ಯದಿ (newval == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (eap->force_bin == FORCE_BIN)
	sprintf((ವರ್ಣ *)newval, " ++bin");
    ಅನ್ಯಥಾ ಯದಿ (eap->force_bin == FORCE_NOBIN)
	sprintf((ವರ್ಣ *)newval, " ++nobin");
    ಅನ್ಯಥಾ
	*newval = NUL;

    ಯದಿ (eap->read_edit)
	STRCAT(newval, " ++edit");

    ಯದಿ (eap->force_ff != 0)
	sprintf((ವರ್ಣ *)newval + STRLEN(newval), " ++ff=%s",
						eap->force_ff == 'u' ? "unix"
						: eap->force_ff == 'd' ? "dos"
						: "mac");
    ಯದಿ (eap->force_enc != 0)
	sprintf((ವರ್ಣ *)newval + STRLEN(newval), " ++enc=%s",
					       eap->cmd + eap->force_enc);
    ಯದಿ (eap->bad_char == BAD_KEEP)
	STRCPY(newval + STRLEN(newval), " ++bad=keep");
    ಅನ್ಯಥಾ ಯದಿ (eap->bad_char == BAD_DROP)
	STRCPY(newval + STRLEN(newval), " ++bad=drop");
    ಅನ್ಯಥಾ ಯದಿ (eap->bad_char != 0)
	sprintf((ವರ್ಣ *)newval + STRLEN(newval), " ++bad=%c", eap->bad_char);
    vimvars[VV_CMDARG].vv_str = newval;
    ಪ್ರತ್ಯಾಗಮ oldval;
}

/*
 * Get the value of internal variable "name".
 * If "flags" has EVAL_VAR_IMPORT may return a VAR_ANY with v_number set to the
 * imported script ID.
 * Return OK or FAIL.  If OK is returned "rettv" must be cleared.
 */
    ಪೂರ್ಣಾಙ್ಕ
eval_variable(
    char_u	*name,
    ಪೂರ್ಣಾಙ್ಕ		len,		// length of "name" or zero
    scid_T	sid,		// script ID for imported item or zero
    typval_T	*rettv,		// NULL when only checking existence
    dictitem_T	**dip,		// non-NULL when typval's dict item is needed
    ಪೂರ್ಣಾಙ್ಕ		flags)		// EVAL_VAR_ flags
{
    ಪೂರ್ಣಾಙ್ಕ		ret = OK;
    typval_T	*tv = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		found = FALSE;
    hashtab_T	*ht = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		cc = 0;
    type_T	*type = ರಿಕ್ತ;

    ಯದಿ (len > 0)
    {
	// truncate the name, so that we can use strcmp()
	cc = name[len];
	name[len] = NUL;
    }

    // Check for local variable when debugging.
    ಯದಿ ((sid == 0) && (tv = lookup_debug_var(name)) == ರಿಕ್ತ)
    {
	// Check for user-defined variables.
	dictitem_T	*v = find_var(name, &ht, flags & EVAL_VAR_NOAUTOLOAD);

	ಯದಿ (v != ರಿಕ್ತ)
	{
	    tv = &v->di_tv;
	    ಯದಿ (dip != ರಿಕ್ತ)
		*dip = v;
	}
	ಅನ್ಯಥಾ
	    ht = ರಿಕ್ತ;
    }

    ಯದಿ (tv == ರಿಕ್ತ && (in_vim9script() || STRNCMP(name, "s:", 2) == 0))
    {
	imported_T  *import = ರಿಕ್ತ;
	char_u	    *p = STRNCMP(name, "s:", 2) == 0 ? name + 2 : name;

	ಯದಿ (sid == 0)
	    import = find_imported(p, 0, TRUE);

	// imported variable from another script
	ಯದಿ (import != ರಿಕ್ತ || sid != 0)
	{
	    ಯದಿ ((flags & EVAL_VAR_IMPORT) == 0)
	    {
		ಯದಿ (SCRIPT_ID_VALID(sid))
		{
		    ht = &SCRIPT_VARS(sid);
		    ಯದಿ (ht != ರಿಕ್ತ)
		    {
			dictitem_T *v = find_var_in_ht(ht, 0, name,
						  flags & EVAL_VAR_NOAUTOLOAD);

			ಯದಿ (v == ರಿಕ್ತ)
			    v = find_var_autoload_prefix(name, sid, ರಿಕ್ತ, ರಿಕ್ತ);

			ಯದಿ (v != ರಿಕ್ತ)
			{
			    tv = &v->di_tv;
			    ಯದಿ (dip != ರಿಕ್ತ)
				*dip = v;
			}
			ಅನ್ಯಥಾ
			    ht = ರಿಕ್ತ;
		    }
		}
		ಅನ್ಯಥಾ
		{
		    ಯದಿ (flags & EVAL_VAR_VERBOSE)
			semsg(_(e_expected_dot_after_name_str), name);
		    ret = FAIL;
		}
	    }
	    ಅನ್ಯಥಾ
	    {
		ಯದಿ (rettv != ರಿಕ್ತ)
		{
		    // special value that is used in handle_subscript()
		    rettv->v_type = VAR_ANY;
		    rettv->vval.v_ಸಂಖ್ಯೆ = sid != 0 ? sid : import->imp_sid;
		}
		found = TRUE;
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (in_vim9script() && (flags & EVAL_VAR_NO_FUNC) == 0)
	{
	    ಪೂರ್ಣಾಙ್ಕ	    has_g_prefix = STRNCMP(name, "g:", 2) == 0;
	    ufunc_T *ufunc = find_func(name, FALSE);

	    ಯದಿ (ufunc != ರಿಕ್ತ && cc == '<')
	    {
		// handle generic function
		char_u	*argp = name + len;
		name[len] = cc;
		ufunc = eval_generic_func(ufunc, name, &argp);
		name[len] = NUL;
		ಯದಿ (ufunc == ರಿಕ್ತ)
		{
		    ret = FAIL;
		    ಗಚ್ಛ done;
		}
	    }

	    // In Vim9 script we can get a function reference by using the
	    // function name.  For a global non-autoload function "g:" is
	    // required.
	    ಯದಿ (ufunc != ರಿಕ್ತ && (has_g_prefix
					    || !func_requires_g_prefix(ufunc)))
	    {
		found = TRUE;
		ಯದಿ (rettv != ರಿಕ್ತ)
		{
		    rettv->v_type = VAR_FUNC;
		    ಯದಿ (has_g_prefix)
		    {
			// Keep the "g:", otherwise script-local may be
			// assumed.
			ಯದಿ (cc != '<')
			    rettv->vval.v_string = vim_strsave(name);
			ಅನ್ಯಥಾ
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
		    ಅನ್ಯಥಾ
			rettv->vval.v_string = vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
		    ಯದಿ (rettv->vval.v_string != ರಿಕ್ತ)
			func_ref(ufunc->uf_name);
		}
	    }
	}
    }

    ಯದಿ (!found)
    {
	ಯದಿ (tv == ರಿಕ್ತ)
	{
	    ಯದಿ (rettv != ರಿಕ್ತ && (flags & EVAL_VAR_VERBOSE))
		semsg(_(e_undefined_variable_str), name);
	    ret = FAIL;
	}
	ಅನ್ಯಥಾ ಯದಿ (rettv != ರಿಕ್ತ)
	{
	    svar_T  *sv = ರಿಕ್ತ;
	    ಪೂರ್ಣಾಙ್ಕ	    was_assigned = FALSE;

	    ಯದಿ (ht != ರಿಕ್ತ && ht == get_script_local_ht()
		    && tv != &SCRIPT_SV(current_sctx.sc_sid)->sv_var.di_tv)
	    {
		sv = find_typval_in_script(tv, 0, TRUE);
		ಯದಿ (sv != ರಿಕ್ತ)
		{
		    type = sv->sv_type;
		    was_assigned = sv->sv_flags & SVFLAG_ASSIGNED;
		}
	    }

	    ಯದಿ ((tv->v_type == VAR_TYPEALIAS || tv->v_type == VAR_CLASS)
		    && sid != 0)
	    {
		// type alias or class imported from another script.  Check
		// whether it is exported from the other script.
		sv = find_typval_in_script(tv, sid, TRUE);
		ಯದಿ (sv == ರಿಕ್ತ)
		{
		    ret = FAIL;
		    ಗಚ್ಛ done;
		}
		ಯದಿ ((sv->sv_flags & SVFLAG_EXPORTED) == 0)
		{
		    semsg(_(e_item_not_exported_in_script_str), name);
		    ret = FAIL;
		    ಗಚ್ಛ done;
		}
	    }

	    // If a list or tuple or dict variable wasn't initialized and has
	    // meaningful type, do it now.  Not for global variables, they are
	    // not declared.
	    ಯದಿ (ht != &globvarht)
	    {
		ಯದಿ (tv->v_type == VAR_DICT && tv->vval.v_dict == ರಿಕ್ತ
					    && ((type != ರಿಕ್ತ && !was_assigned)
							  || !in_vim9script()))
		{
		    tv->vval.v_dict = dict_alloc();
		    ಯದಿ (tv->vval.v_dict != ರಿಕ್ತ)
		    {
			++tv->vval.v_dict->dv_refcount;
			tv->vval.v_dict->dv_type = alloc_type(type);
			ಯದಿ (sv != ರಿಕ್ತ)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
		ಅನ್ಯಥಾ ಯದಿ (tv->v_type == VAR_LIST && tv->vval.v_list == ರಿಕ್ತ
					    && ((type != ರಿಕ್ತ && !was_assigned)
							  || !in_vim9script()))
		{
		    tv->vval.v_list = list_alloc();
		    ಯದಿ (tv->vval.v_list != ರಿಕ್ತ)
		    {
			++tv->vval.v_list->lv_refcount;
			tv->vval.v_list->lv_type = alloc_type(type);
			ಯದಿ (sv != ರಿಕ್ತ)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
		ಅನ್ಯಥಾ ಯದಿ (tv->v_type == VAR_TUPLE && tv->vval.v_tuple == ರಿಕ್ತ
					    && ((type != ರಿಕ್ತ && !was_assigned)
							  || !in_vim9script()))
		{
		    tv->vval.v_tuple = tuple_alloc();
		    ಯದಿ (tv->vval.v_tuple != ರಿಕ್ತ)
		    {
			++tv->vval.v_tuple->tv_refcount;
			tv->vval.v_tuple->tv_type = alloc_type(type);
			ಯದಿ (sv != ರಿಕ್ತ)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
		ಅನ್ಯಥಾ ಯದಿ (tv->v_type == VAR_BLOB && tv->vval.v_blob == ರಿಕ್ತ
					    && ((type != ರಿಕ್ತ && !was_assigned)
							  || !in_vim9script()))
		{
		    tv->vval.v_blob = blob_alloc();
		    ಯದಿ (tv->vval.v_blob != ರಿಕ್ತ)
		    {
			++tv->vval.v_blob->bv_refcount;
			ಯದಿ (sv != ರಿಕ್ತ)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
	    }
	    ret = copy_tv(tv, rettv);
	}
    }

done:
    ಯದಿ (len > 0)
	name[len] = cc;

    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Get the value of internal variable "name", also handling "import.name".
 * Return OK or FAIL.  If OK is returned "rettv" must be cleared.
 */
    ಪೂರ್ಣಾಙ್ಕ
eval_variable_import(
    char_u	*name,
    typval_T	*rettv)
{
    char_u  *s = name;
    ಯಾವತ್ (ASCII_ISALNUM(*s) || *s == '_')
	++s;
    ಪೂರ್ಣಾಙ್ಕ	    len = (ಪೂರ್ಣಾಙ್ಕ)(s - name);

    ಯದಿ (eval_variable(name, len, 0, rettv, ರಿಕ್ತ, EVAL_VAR_IMPORT) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಯದಿ (rettv->v_type == VAR_ANY && *s == '.')
    {
	char_u *ns = s + 1;
	s = ns;
	ಯಾವತ್ (ASCII_ISALNUM(*s) || *s == '_')
	    ++s;
	ಪೂರ್ಣಾಙ್ಕ sid = rettv->vval.v_ಸಂಖ್ಯೆ;
	ಪ್ರತ್ಯಾಗಮ eval_variable(ns, (ಪೂರ್ಣಾಙ್ಕ)(s - ns), sid, rettv, ರಿಕ್ತ, 0);
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}


/*
 * Check if variable "name[len]" is a local variable or an argument.
 * If so, "*eval_lavars_used" is set to TRUE.
 */
    ಶೂನ್ಯ
check_vars(char_u *name, ಪೂರ್ಣಾಙ್ಕ len)
{
    ಪೂರ್ಣಾಙ್ಕ		cc;
    char_u	*varname;
    hashtab_T	*ht;

    ಯದಿ (eval_lavars_used == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    // truncate the name, so that we can use strcmp()
    cc = name[len];
    name[len] = NUL;

    ht = find_var_ht(name, &varname);
    ಯದಿ (ht == get_funccal_local_ht() || ht == get_funccal_args_ht())
    {
	ಯದಿ (find_var(name, ರಿಕ್ತ, TRUE) != ರಿಕ್ತ)
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
find_var(char_u *name, hashtab_T **htp, ಪೂರ್ಣಾಙ್ಕ no_autoload)
{
    char_u	*varname;
    hashtab_T	*ht;
    dictitem_T	*ret = ರಿಕ್ತ;

    ht = find_var_ht(name, &varname);
    ಯದಿ (htp != ರಿಕ್ತ)
	*htp = ht;
    ಯದಿ (ht == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ret = find_var_in_ht(ht, *name, varname, no_autoload);
    ಯದಿ (ret != ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ret;

    // Search in parent scope for lambda
    ret = find_var_in_scoped_ht(name, no_autoload);
    ಯದಿ (ret != ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ret;

    // in Vim9 script items without a scope can be script-local
    ಯದಿ (in_vim9script() && name[0] != NUL && name[1] != ':')
    {
	ht = get_script_local_ht();
	ಯದಿ (ht != ರಿಕ್ತ)
	{
	    ret = find_var_in_ht(ht, *name, varname, no_autoload);
	    ಯದಿ (ret != ರಿಕ್ತ)
	    {
		ಯದಿ (htp != ರಿಕ್ತ)
		    *htp = ht;
		ಪ್ರತ್ಯಾಗಮ ret;
	    }
	}
    }

    // and finally try
    ಪ್ರತ್ಯಾಗಮ find_var_autoload_prefix(name, 0, htp, ರಿಕ್ತ);
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
find_var_autoload_prefix(char_u *name, ಪೂರ್ಣಾಙ್ಕ sid, hashtab_T **htp,
							    char_u **namep)
{
    hashtab_T	*ht;
    dictitem_T	*ret = ರಿಕ್ತ;
    // When using "vim9script autoload" script-local items are prefixed but can
    // be used with s:name.
    ಪೂರ್ಣಾಙ್ಕ check_sid = sid > 0 ? sid : current_sctx.sc_sid;
    ಯದಿ (SCRIPT_ID_VALID(check_sid)
		   && (in_vim9script() || (name[0] == 's' && name[1] == ':')))
    {
	scriptitem_T *si = SCRIPT_ITEM(check_sid);

	ಯದಿ (si->sn_autoload_prefix != ರಿಕ್ತ)
	{
	    char_u *base_name = (name[0] == 's' && name[1] == ':')
							     ? name + 2 : name;
	    char_u *auto_name = concat_str(si->sn_autoload_prefix, base_name);

	    ಯದಿ (auto_name != ರಿಕ್ತ)
	    {
		ಪೂರ್ಣಾಙ್ಕ free_auto_name = TRUE;
		ht = &globvarht;
		ret = find_var_in_ht(ht, 'g', auto_name, TRUE);
		ಯದಿ (ret != ರಿಕ್ತ)
		{
		    ಯದಿ (htp != ರಿಕ್ತ)
			*htp = ht;
		    ಯದಿ (namep != ರಿಕ್ತ)
		    {
			free_auto_name = FALSE;
			*namep = auto_name;
		    }
		}
		ಯದಿ (free_auto_name)
		    vim_free(auto_name);
	    }
	}
    }

    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Like find_var() but if the name starts with <SNR>99_ then look in the
 * referenced script (used for a funcref).
 */
    dictitem_T *
find_var_also_in_script(char_u *name, hashtab_T **htp, ಪೂರ್ಣಾಙ್ಕ no_autoload)
{
    ಯದಿ (STRNCMP(name, "<SNR>", 5) == 0 && SAFE_isdigit(name[5]))
    {
	char_u	    *p = name + 5;
	ಪೂರ್ಣಾಙ್ಕ	    sid = getdigits(&p);

	ಯದಿ (SCRIPT_ID_VALID(sid) && *p == '_')
	{
	    hashtab_T	*ht = &SCRIPT_VARS(sid);

	    ಯದಿ (ht != ರಿಕ್ತ)
	    {
		dictitem_T *di = find_var_in_ht(ht, 0, p + 1, no_autoload);

		ಯದಿ (di != ರಿಕ್ತ)
		{
		    ಯದಿ (htp != ರಿಕ್ತ)
			*htp = ht;
		    ಪ್ರತ್ಯಾಗಮ di;
		}
	    }
	}
    }

    ಪ್ರತ್ಯಾಗಮ find_var(name, htp, no_autoload);
}

/*
 * Find variable "varname" in hashtab "ht" with name "htname".
 * When "varname" is empty returns curwin/curtab/etc vars dictionary.
 * Returns NULL if not found.
 */
    dictitem_T *
find_var_in_ht(
    hashtab_T	*ht,
    ಪೂರ್ಣಾಙ್ಕ		htname,
    char_u	*varname,
    ಪೂರ್ಣಾಙ್ಕ		no_autoload)
{
    hashitem_T	*hi;

    ಯದಿ (*varname == NUL)
    {
	// Must be something like "s:", otherwise "ht" would be NULL.
	ವಿಕಲ್ಪ (htname)
	{
	    ಪಕ್ಷ 's': ಪ್ರತ್ಯಾಗಮ &SCRIPT_SV(current_sctx.sc_sid)->sv_var;
	    ಪಕ್ಷ 'g': ಪ್ರತ್ಯಾಗಮ &globvars_var;
	    ಪಕ್ಷ 'v': ಪ್ರತ್ಯಾಗಮ &vimvars_var;
	    ಪಕ್ಷ 'b': ಪ್ರತ್ಯಾಗಮ &curbuf->b_bufvar;
	    ಪಕ್ಷ 'w': ಪ್ರತ್ಯಾಗಮ &curwin->w_winvar;
	    ಪಕ್ಷ 't': ಪ್ರತ್ಯಾಗಮ &curtab->tp_winvar;
	    ಪಕ್ಷ 'l': ಪ್ರತ್ಯಾಗಮ get_funccal_local_var();
	    ಪಕ್ಷ 'a': ಪ್ರತ್ಯಾಗಮ get_funccal_args_var();
	}
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    hi = hash_find(ht, varname);
    ಯದಿ (HASHITEM_EMPTY(hi))
    {
	// For global variables we may try auto-loading the script.  If it
	// worked find the variable again.  Don't auto-load a script if it was
	// loaded already, otherwise it would be loaded every time when
	// checking if a function name is a Funcref variable.
	ಯದಿ (ht == &globvarht && !no_autoload)
	{
	    // Note: script_autoload() may make "hi" invalid. It must either
	    // be obtained again or not used.
	    ಯದಿ (!script_autoload(varname, FALSE) || aborting())
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    hi = hash_find(ht, varname);
	}
	ಯದಿ (HASHITEM_EMPTY(hi))
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಪ್ರತ್ಯಾಗಮ HI2DI(hi);
}

/*
 * Get the script-local hashtab.  NULL if not in a script context.
 */
    hashtab_T *
get_script_local_ht(ಶೂನ್ಯ)
{
    scid_T sid = current_sctx.sc_sid;

    ಯದಿ (SCRIPT_ID_VALID(sid))
	ಪ್ರತ್ಯಾಗಮ &SCRIPT_VARS(sid);
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Look for "name[len]" in script-local variables and functions.
 * When "cmd" is TRUE it must look like a command, a function must be followed
 * by "(" or "->".
 * Return OK when found, FAIL when not found.
 */
    ಪೂರ್ಣಾಙ್ಕ
lookup_scriptitem(
	char_u	*name,
	size_t	len,
	ಪೂರ್ಣಾಙ್ಕ	cmd,
	cctx_T	*dummy UNUSED)
{
    hashtab_T	*ht = get_script_local_ht();
    char_u	buffer[30];
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		res;
    hashitem_T	*hi;
    ಪೂರ್ಣಾಙ್ಕ		is_global = FALSE;
    char_u	*fname = name;

    ಯದಿ (ht == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಯದಿ (len < ಪರಿಮಾಣ(buffer) - 1)
    {
	// avoid an alloc/free for short names
	vim_strncpy(buffer, name, len);
	p = buffer;
    }
    ಅನ್ಯಥಾ
    {
	p = vim_strnsave(name, len);
	ಯದಿ (p == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    hi = hash_find(ht, p);
    res = HASHITEM_EMPTY(hi) ? FAIL : OK;

    // if not script-local, then perhaps autoload-exported
    ಯದಿ (res == FAIL && find_var_autoload_prefix(p, 0, ರಿಕ್ತ, ರಿಕ್ತ) != ರಿಕ್ತ)
	res = OK;

    // if not script-local or autoload, then perhaps imported
    ಯದಿ (res == FAIL && find_imported(p, 0, FALSE) != ರಿಕ್ತ)
	res = OK;
    ಯದಿ (p != buffer)
	vim_free(p);

    // Find a function, so that a following "->" works.
    // When used as a command require "(" or "->" to follow, "Cmd" is a user
    // command while "Cmd()" is a function call.
    ಯದಿ (res != OK)
    {
	p = skipwhite(name + len);

	ಯದಿ (!cmd || name[len] == '(' || (p[0] == '-' && p[1] == '>'))
	{
	    // Do not check for an internal function, since it might also be a
	    // valid command, such as ":split" versus "split()".
	    // Skip "g:" before a function name.
	    ಯದಿ (name[0] == 'g' && name[1] == ':')
	    {
		is_global = TRUE;
		fname = name + 2;
	    }
	    ಯದಿ (find_func(fname, is_global) != ರಿಕ್ತ)
		res = OK;
	}
    }

    ಪ್ರತ್ಯಾಗಮ res;
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

    ಯದಿ (name[0] == NUL)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಯದಿ (name[1] != ':')
    {
	// The name must not start with a colon or #.
	ಯದಿ (name[0] == ':' || name[0] == AUTOLOAD_CHAR)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	*varname = name;

	// "version" is "v:version" in all scopes if scriptversion < 3.
	// Same for a few other variables marked with VV_COMPAT.
	ಯದಿ (in_old_script(3))
	{
	    hi = hash_find(&compat_hashtab, name);
	    ಯದಿ (!HASHITEM_EMPTY(hi))
		ಪ್ರತ್ಯಾಗಮ &compat_hashtab;
	}

	ht = get_funccal_local_ht();
	ಯದಿ (ht != ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ ht;				// local variable

	// In Vim9 script items at the script level are script-local, except
	// for autoload names.
	ಯದಿ (in_vim9script() && vim_strchr(name, AUTOLOAD_CHAR) == ರಿಕ್ತ)
	{
	    ht = get_script_local_ht();
	    ಯದಿ (ht != ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ ht;
	}

	ಪ್ರತ್ಯಾಗಮ &globvarht;			// global variable
    }
    *varname = name + 2;
    ಯದಿ (*name == 'g')				// global variable
	ಪ್ರತ್ಯಾಗಮ &globvarht;
    // There must be no ':' or '#' in the rest of the name, unless g: is used
    ಯದಿ (vim_strchr(name + 2, ':') != ರಿಕ್ತ
			       || vim_strchr(name + 2, AUTOLOAD_CHAR) != ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಯದಿ (*name == 'b')				// buffer variable
	ಪ್ರತ್ಯಾಗಮ &curbuf->b_vars->dv_hashtab;
    ಯದಿ (*name == 'w')				// window variable
	ಪ್ರತ್ಯಾಗಮ &curwin->w_vars->dv_hashtab;
    ಯದಿ (*name == 't')				// tab page variable
	ಪ್ರತ್ಯಾಗಮ &curtab->tp_vars->dv_hashtab;
    ಯದಿ (*name == 'v')				// v: variable
	ಪ್ರತ್ಯಾಗಮ &vimvarht;
    ಯದಿ (get_current_funccal() != ರಿಕ್ತ
	       && get_current_funccal()->fc_func->uf_def_status
							    == UF_NOT_COMPILED)
    {
	// a: and l: are only used in functions defined with ":function"
	ಯದಿ (*name == 'a')			// a: function argument
	    ಪ್ರತ್ಯಾಗಮ get_funccal_args_ht();
	ಯದಿ (*name == 'l')			// l: local function variable
	    ಪ್ರತ್ಯಾಗಮ get_funccal_local_ht();
    }
    ಯದಿ (*name == 's')				// script variable
    {
	ht = get_script_local_ht();
	ಯದಿ (ht != ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ ht;
    }
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
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

    v = find_var(name, ರಿಕ್ತ, FALSE);
    ಯದಿ (v == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ tv_get_string(&v->di_tv);
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
    ಯದಿ (sv == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    init_var_dict(&sv->sv_dict, &sv->sv_var, VAR_SCOPE);
    SCRIPT_ITEM(id)->sn_vars = sv;
}

/*
 * Initialize dictionary "dict" as a scope and set variable "dict_var" to
 * point to it.
 */
    ಶೂನ್ಯ
init_var_dict(dict_T *dict, dictitem_T *dict_var, ಪೂರ್ಣಾಙ್ಕ scope)
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
vars_clear_ext(hashtab_T *ht, ಪೂರ್ಣಾಙ್ಕ free_val)
{
    ಪೂರ್ಣಾಙ್ಕ		todo;
    hashitem_T	*hi;
    dictitem_T	*v;

    hash_lock(ht);
    todo = (ಪೂರ್ಣಾಙ್ಕ)ht->ht_used;
    FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(ht, hi, todo)
    {
	ಯದಿ (!HASHITEM_EMPTY(hi))
	{
	    --todo;

	    // Free the variable.  Don't remove it from the hashtab,
	    // ht_array might change then.  hash_clear() takes care of it
	    // later.
	    v = HI2DI(hi);
	    ಯದಿ (free_val)
		clear_tv(&v->di_tv);
	    ಯದಿ (v->di_flags & DI_FLAGS_ALLOC)
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

    ಯದಿ (hash_remove(ht, hi, "delete variable") != OK)
	ಪ್ರತ್ಯಾಗಮ;

    clear_tv(&di->di_tv);
    vim_free(di);
}

/*
 * Delete the exported variables of a reloaded Vim9 autoload script.  They live
 * in the global namespace with the autoload prefix (e.g. "foo#bar") and are
 * recreated when the script body runs again.
 */
    ಶೂನ್ಯ
delete_autoload_export_vars(char_u *prefix)
{
    ಯದಿ (prefix == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    size_t  prefixlen = STRLEN(prefix);

    hash_lock(&globvarht);
    ಪೂರ್ಣಾಙ್ಕ	todo = (ಪೂರ್ಣಾಙ್ಕ)globvarht.ht_used;

    ಪ್ರತಿ (hashitem_T *hi = globvarht.ht_array; todo > 0; ++hi)
	ಯದಿ (!HASHITEM_EMPTY(hi))
	{
	    dictitem_T	*di = HI2DI(hi);

	    --todo;
	    // Keep a class or enum: existing objects still refer to it.
	    ಯದಿ (di->di_tv.v_type != VAR_CLASS
		    && STRNCMP(di->di_ಕೀಲಿ, prefix, prefixlen) == 0)
		delete_var(&globvarht, hi);
	}
    hash_unlock(&globvarht);
}

/*
 * List the value of one internal variable.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
list_one_var(dictitem_T *v, ವರ್ಣ *prefix, ಪೂರ್ಣಾಙ್ಕ *first)
{
    char_u	*tofree;
    char_u	*s;
    char_u	numbuf[NUMBUFLEN];

    s = echo_string(&v->di_tv, &tofree, numbuf, get_copyID());
    list_one_var_a(prefix, v->di_ಕೀಲಿ, v->di_tv.v_type,
					 s == ರಿಕ್ತ ? (char_u *)"" : s, first);
    vim_free(tofree);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
list_one_var_a(
    ವರ್ಣ	*prefix,
    char_u	*name,
    ಪೂರ್ಣಾಙ್ಕ		type,
    char_u	*string,
    ಪೂರ್ಣಾಙ್ಕ		*first)  // when TRUE clear rest of screen and set to FALSE
{
    // don't use msg() or msg_attr() to avoid overwriting "v:statusmsg"
    ಸಂದೇಶ_start();
    ಸಂದೇಶ_puts(prefix);
    ಯದಿ (name != ರಿಕ್ತ)	// "a:" vars don't have a name stored
	ಸಂದೇಶ_puts((ವರ್ಣ *)name);
    ಸಂದೇಶ_putchar(' ');
    ಸಂದೇಶ_advance(22);
    ಯದಿ (type == VAR_ಸಂಖ್ಯೆ)
	ಸಂದೇಶ_putchar('#');
    ಅನ್ಯಥಾ ಯದಿ (type == VAR_FUNC || type == VAR_PARTIAL)
	ಸಂದೇಶ_putchar('*');
    ಅನ್ಯಥಾ ಯದಿ (type == VAR_LIST)
    {
	ಸಂದೇಶ_putchar('[');
	ಯದಿ (*string == '[')
	    ++string;
    }
    ಅನ್ಯಥಾ ಯದಿ (type == VAR_DICT)
    {
	ಸಂದೇಶ_putchar('{');
	ಯದಿ (*string == '{')
	    ++string;
    }
    ಅನ್ಯಥಾ
	ಸಂದೇಶ_putchar(' ');

    ಸಂದೇಶ_outtrans(string);

    ಯದಿ (type == VAR_FUNC || type == VAR_PARTIAL)
	ಸಂದೇಶ_puts("()");
    ಯದಿ (*first)
    {
	ಸಂದೇಶ_clr_eos();
	*first = FALSE;
    }
}

/*
 * Addition handling for setting a v: variable.
 * Return TRUE if the variable should be set normally,
 *        FALSE if nothing else needs to be done.
 */
    ಪೂರ್ಣಾಙ್ಕ
before_set_vvar(
    char_u	*varname,
    dictitem_T	*di,
    typval_T	*tv,
    ಪೂರ್ಣಾಙ್ಕ		copy,
    ಪೂರ್ಣಾಙ್ಕ		*type_error)
{
    ಯದಿ (di->di_tv.v_type == VAR_STRING)
    {
	VIM_CLEAR(di->di_tv.vval.v_string);
	ಯದಿ (copy || tv->v_type != VAR_STRING)
	{
	    char_u *val = tv_get_string(tv);

	    // Careful: when assigning to v:errmsg and
	    // tv_get_string() causes an error message the variable
	    // will already be set.
	    ಯದಿ (di->di_tv.vval.v_string == ರಿಕ್ತ)
		di->di_tv.vval.v_string = vim_strsave(val);
	}
	ಅನ್ಯಥಾ
	{
	    // Take over the string to avoid an extra alloc/free.
	    di->di_tv.vval.v_string = tv->vval.v_string;
	    tv->vval.v_string = ರಿಕ್ತ;
	}
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }
    ಅನ್ಯಥಾ ಯದಿ (di->di_tv.v_type == VAR_ಸಂಖ್ಯೆ)
    {
	di->di_tv.vval.v_ಸಂಖ್ಯೆ = tv_get_ಸಂಖ್ಯೆ(tv);
	ಯದಿ (STRCMP(varname, "searchforward") == 0)
	    set_ಹುಡುಕು_direction(di->di_tv.vval.v_ಸಂಖ್ಯೆ ? '/' : '?');
#ifdef FEAT_ಹುಡುಕು_EXTRA
	ಅನ್ಯಥಾ ಯದಿ (STRCMP(varname, "hlsearch") == 0)
	{
	    no_hlsearch = !di->di_tv.vval.v_ಸಂಖ್ಯೆ;
	    redraw_all_later(UPD_SOME_VALID);
	}
#endif
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }
    ಅನ್ಯಥಾ ಯದಿ (di->di_tv.v_type != tv->v_type)
    {
	*type_error = TRUE;
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * Set variable "name" to value in "tv".
 * If the variable already exists, the value is updated.
 * Otherwise the variable is created.
 */
    ಶೂನ್ಯ
set_var(
    char_u	*name,
    typval_T	*tv,
    ಪೂರ್ಣಾಙ್ಕ		copy)	    // make copy of value in "tv"
{
    set_var_const(name, 0, ರಿಕ್ತ, tv, copy, ASSIGN_DECL, 0);
}

/*
 * Set variable "name" to value in "tv_arg".
 * When "sid" is non-zero "name" is in the script with this ID.
 * If the variable already exists and "is_const" is FALSE the value is updated.
 * Otherwise the variable is created.
 */
    ಪೂರ್ಣಾಙ್ಕ
set_var_const(
    char_u	*name,
    scid_T	sid,
    type_T	*type_arg,
    typval_T	*tv_arg,
    ಪೂರ್ಣಾಙ್ಕ		copy,	    // make copy of value in "tv"
    ಪೂರ್ಣಾಙ್ಕ		flags_arg,  // ASSIGN_CONST, ASSIGN_FINAL, etc.
    ಪೂರ್ಣಾಙ್ಕ		var_idx)    // index for ":let [a, b] = list"
{
    typval_T	*tv = tv_arg;
    type_T	*type = type_arg;
    typval_T	bool_tv;
    dictitem_T	*di;
    typval_T	*dest_tv;
    char_u	*varname;
    char_u	*name_tofree = ರಿಕ್ತ;
    hashtab_T	*ht = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		is_script_local;
    ಪೂರ್ಣಾಙ್ಕ		vim9script = in_vim9script();
    ಪೂರ್ಣಾಙ್ಕ		var_in_vim9script;
    ಪೂರ್ಣಾಙ್ಕ		var_in_autoload = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		flags = flags_arg;
    ಪೂರ್ಣಾಙ್ಕ		free_tv_arg = !copy;  // free tv_arg if not used
    ಪೂರ್ಣಾಙ್ಕ		rc = FAIL;

    ಯದಿ (sid != 0)
    {
	varname = ರಿಕ್ತ;
	ಯದಿ (SCRIPT_ID_VALID(sid))
	{
	    char_u	*auto_name = ರಿಕ್ತ;
	    ಯದಿ (find_var_autoload_prefix(name, sid, &ht, &auto_name) != ರಿಕ್ತ)
	    {
		var_in_autoload = TRUE;
		varname = auto_name;
		name_tofree = varname;
	    }
	    ಅನ್ಯಥಾ
		ht = &SCRIPT_VARS(sid);
	}
	ಯದಿ (varname == ರಿಕ್ತ)
	    varname = name;
    }
    ಅನ್ಯಥಾ
    {
	scriptitem_T	*si;
	char_u		*auto_name = ರಿಕ್ತ;

	ಯದಿ (in_vim9script()
	    && SCRIPT_ID_VALID(current_sctx.sc_sid)
	    && (si = SCRIPT_ITEM(current_sctx.sc_sid))
					      ->sn_autoload_prefix != ರಿಕ್ತ
	    && (is_export
		|| find_var_autoload_prefix(name, 0, ರಿಕ್ತ, &auto_name)
								    != ರಿಕ್ತ))
	{
	    // In a vim9 autoload script an exported variable is put in the
	    // global namespace with the autoload prefix.
	    var_in_autoload = TRUE;
	    varname = auto_name != ರಿಕ್ತ ? auto_name
		      : concat_str(si->sn_autoload_prefix, name);
	    ಯದಿ (varname == ರಿಕ್ತ)
		ಗಚ್ಛ failed;
	    name_tofree = varname;
	    ht = &globvarht;
	}
	ಅನ್ಯಥಾ
	    ht = find_var_ht(name, &varname);
    }
    ಯದಿ (ht == ರಿಕ್ತ || *varname == NUL)
    {
	semsg(_(e_illegal_variable_name_str), name);
	ಗಚ್ಛ failed;
    }
    is_script_local = ht == get_script_local_ht() || sid != 0
							    || var_in_autoload;

    ಯದಿ (vim9script
	    && !is_script_local
	    && (flags & (ASSIGN_NO_DECL | ASSIGN_DECL)) == 0
	    && (flags & (ASSIGN_CONST | ASSIGN_FINAL)) == 0
	    && name[1] == ':')
    {
	vim9_declare_error(name);
	ಗಚ್ಛ failed;
    }
    ಯದಿ ((flags & ASSIGN_FOR_LOOP) && is_scoped_variable(name))
	// Do not make g:var, w:var, b:var or t:var final.
	flags &= ~ASSIGN_FINAL;

    var_in_vim9script = is_script_local && current_script_is_vim9();
    ಯದಿ (var_in_vim9script && name[0] == '_' && name[1] == NUL)
    {
	// For "[a, _] = list" the underscore is ignored.
	ಯದಿ ((flags & ASSIGN_UNPACK) == 0)
	    emsg(_(e_cannot_use_underscore_here));
	ಗಚ್ಛ failed;
    }

    di = find_var_in_ht(ht, 0, varname, TRUE);

    ಯದಿ (di == ರಿಕ್ತ && var_in_vim9script)
    {
	imported_T  *import = find_imported(varname, 0, FALSE);

	ಯದಿ (import != ರಿಕ್ತ)
	{
	    // imported name space cannot be used
	    ಯದಿ ((flags & ASSIGN_NO_DECL) == 0)
	    {
		semsg(_(e_redefining_imported_item_str), name);
		ಗಚ್ಛ failed;
	    }
	    semsg(_(e_cannot_use_str_itself_it_is_imported), name);
	    ಗಚ್ಛ failed;
	}
	ಯದಿ (!in_vim9script())
	{
	    semsg(_(e_cannot_create_vim9_script_variable_in_function_str),
									 name);
	    ಗಚ್ಛ failed;
	}
    }

    // Search in parent scope which is possible to reference from lambda
    ಯದಿ (di == ರಿಕ್ತ)
	di = find_var_in_scoped_ht(name, TRUE);

    ಯದಿ ((tv->v_type == VAR_FUNC || tv->v_type == VAR_PARTIAL)
				  && var_wrong_func_name(name, di == ರಿಕ್ತ))
	ಗಚ್ಛ failed;

    ಯದಿ (need_convert_to_bool(type, tv))
    {
	// Destination is a bool and the value is not, but it can be
	// converted.
	CLEAR_FIELD(bool_tv);
	bool_tv.v_type = VAR_BOOL;
	bool_tv.vval.v_ಸಂಖ್ಯೆ = tv2bool(tv) ? VVAL_TRUE : VVAL_FALSE;
	tv = &bool_tv;
    }

    ಯದಿ (di != ರಿಕ್ತ)
    {
	// Item already exists.  Allowed to replace when reloading.
	ಯದಿ ((di->di_flags & DI_FLAGS_RELOAD) == 0)
	{
	    ಯದಿ ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					 && (flags & ASSIGN_FOR_LOOP) == 0)
	    {
		emsg(_(e_cannot_modify_existing_variable));
		ಗಚ್ಛ failed;
	    }

	    ಯದಿ (is_script_local && vim9script
			  && (flags & (ASSIGN_NO_DECL | ASSIGN_DECL)) == 0)
	    {
		semsg(_(e_redefining_script_item_str), name);
		ಗಚ್ಛ failed;
	    }

	    ಯದಿ (check_typval_is_ಮೌಲ್ಯ(&di->di_tv) == FAIL)
		ಗಚ್ಛ failed;

	    // List and Blob types can be modified in-place using the "+="
	    // compound operator.  For other types, this is not allowed.
	    ಪೂರ್ಣಾಙ್ಕ type_inplace_modifiable =
		(di->di_tv.v_type == VAR_LIST || di->di_tv.v_type == VAR_BLOB);

	    ಯದಿ (var_in_vim9script && (flags & ASSIGN_FOR_LOOP) == 0
		    && ((flags & ASSIGN_COMPOUND_OP) == 0
			|| !type_inplace_modifiable))
	    {
		where_T where = WHERE_INIT;
		svar_T  *sv = find_typval_in_script(&di->di_tv, sid, TRUE);

		ಯದಿ (sv != ರಿಕ್ತ)
		{
		    // check the type and adjust to bool if needed
		    ಯದಿ (var_idx > 0)
		    {
			where.wt_index = var_idx;
			where.wt_kind = WT_VARIABLE;
		    }
		    ಯದಿ (check_script_var_type(sv, tv, name, where) == FAIL)
			ಗಚ್ಛ failed;
		    ಯದಿ (type == ರಿಕ್ತ)
			type = sv->sv_type;
		    sv->sv_flags |= SVFLAG_ASSIGNED;
		}
	    }

	    // Modifying a final variable with a List value using the "+="
	    // operator is allowed.  For other types, it is not allowed.
	    ಯದಿ ((((flags & ASSIGN_FOR_LOOP) == 0 || (flags & ASSIGN_DECL) == 0)
			&& ((flags & ASSIGN_COMPOUND_OP) == 0
			    || !type_inplace_modifiable))
				 ? var_check_permission(di, name) == FAIL
				 : var_check_ro(di->di_flags, name, FALSE))
		ಗಚ್ಛ failed;
	}
	ಅನ್ಯಥಾ
	{
	    // can only redefine once
	    di->di_flags &= ~DI_FLAGS_RELOAD;

	    // A Vim9 script-local variable is also present in sn_all_vars
	    // and sn_var_vals.  It may set "type" from "tv".
	    ಯದಿ (var_in_vim9script || var_in_autoload)
		update_vim9_script_var(FALSE, di,
			var_in_autoload ? name : di->di_ಕೀಲಿ, flags,
			tv, &type, (flags & ASSIGN_NO_MEMBER_TYPE) == 0);
	}

	// existing variable, need to clear the value

	// In Vim9 script a String v: variable only takes a String.
	ಯದಿ (ht == &vimvarht && di->di_tv.v_type == VAR_STRING
							    && in_vim9script())
	{
	    where_T	where = WHERE_INIT;

	    ಯದಿ (check_typval_type(&t_string, tv, where) == FAIL)
		ಗಚ್ಛ failed;
	}

	// Handle setting internal v: variables separately where needed to
	// prevent changing the type.
	ಪೂರ್ಣಾಙ್ಕ type_error = FALSE;
	ಯದಿ (ht == &vimvarht
		&& !before_set_vvar(varname, di, tv, copy, &type_error))
	{
	    ಯದಿ (type_error)
		semsg(_(e_setting_v_str_to_ಮೌಲ್ಯ_with_wrong_type), varname);
	    ಗಚ್ಛ failed;
	}

	clear_tv(&di->di_tv);

	ಯದಿ ((flags & ASSIGN_UPDATE_BLOCK_ID)
				       && SCRIPT_ID_VALID(current_sctx.sc_sid))
	{
	    scriptitem_T *si = SCRIPT_ITEM(current_sctx.sc_sid);

	    update_script_var_block_id(name, si->sn_current_block_id);
	}
    }
    ಅನ್ಯಥಾ
    {
	// Item not found, check if a function already exists.
	ಯದಿ (is_script_local && (flags & (ASSIGN_NO_DECL | ASSIGN_DECL)) == 0
	       && lookup_scriptitem(name, STRLEN(name), FALSE, ರಿಕ್ತ) == OK)
	{
	    semsg(_(e_redefining_script_item_str), name);
	    ಗಚ್ಛ failed;
	}

	// add a new variable
	ಯದಿ (var_in_vim9script && (flags & ASSIGN_NO_DECL))
	{
	    semsg(_(e_unknown_variable_str), name);
	    ಗಚ್ಛ failed;
	}

	ಯದಿ (check_hashtab_frozen(ht, "add variable"))
	    ಗಚ್ಛ failed;

	// Can't add "v:" or "a:" variable.
	ಯದಿ (ht == &vimvarht || ht == get_funccal_args_ht())
	{
	    semsg(_(e_illegal_variable_name_str), name);
	    ಗಚ್ಛ failed;
	}

	// Make sure the variable name is valid.  In Vim9 script an
	// autoload variable must be prefixed with "g:" unless in an
	// autoload script.
	ಯದಿ (!valid_varname(varname, -1, !vim9script
			|| STRNCMP(name, "g:", 2) == 0 || var_in_autoload))
	    ಗಚ್ಛ failed;

	di = alloc(offsetof(dictitem_T, di_ಕೀಲಿ) + STRLEN(varname) + 1);
	ಯದಿ (di == ರಿಕ್ತ)
	    ಗಚ್ಛ failed;
	STRCPY(di->di_ಕೀಲಿ, varname);
	ಯದಿ (hash_add(ht, DI2HIKEY(di), "add variable") == FAIL)
	{
	    vim_free(di);
	    ಗಚ್ಛ failed;
	}
	di->di_flags = DI_FLAGS_ALLOC;
	ಯದಿ (flags & (ASSIGN_CONST | ASSIGN_FINAL))
	    di->di_flags |= DI_FLAGS_LOCK;

	// A Vim9 script-local variable is also added to sn_all_vars and
	// sn_var_vals. It may set "type" from "tv".
	ಯದಿ (var_in_vim9script || var_in_autoload)
	    update_vim9_script_var(TRUE, di,
		    var_in_autoload ? name : di->di_ಕೀಲಿ, flags,
			  tv, &type, (flags & ASSIGN_NO_MEMBER_TYPE) == 0);
    }

    dest_tv = &di->di_tv;
    ಯದಿ (copy || tv->v_type == VAR_ಸಂಖ್ಯೆ || tv->v_type == VAR_FLOAT)
	copy_tv(tv, dest_tv);
    ಅನ್ಯಥಾ
    {
	*dest_tv = *tv;
	dest_tv->v_lock = 0;
	init_tv(tv);
    }
    free_tv_arg = FALSE;

    ಯದಿ (vim9script && type != ರಿಕ್ತ)
	set_tv_type(dest_tv, type);

    // ":const var = value" locks the value
    // ":final var = value" locks "var"
    ಯದಿ (flags & ASSIGN_CONST)
	// Like :lockvar! name: lock the value and what it contains, but only
	// if the reference count is up to one.  That locks only literal
	// values.
	item_lock(dest_tv, DICT_MAXNEST, TRUE, TRUE);

    rc = OK;

failed:
    vim_free(name_tofree);
    ಯದಿ (free_tv_arg)
	clear_tv(tv_arg);

    ಪ್ರತ್ಯಾಗಮ rc;
}

/*
 * Check in this order for backwards compatibility:
 * - Whether the variable is read-only
 * - Whether the variable value is locked
 * - Whether the variable is locked
 * NOTE: "name" is only used for error messages.
 */
    ಪೂರ್ಣಾಙ್ಕ
var_check_permission(dictitem_T *di, char_u *name)
{
    ಯದಿ (var_check_ro(di->di_flags, name, FALSE)
		    || ಮೌಲ್ಯ_check_lock(di->di_tv.v_lock, name, FALSE)
		    || var_check_lock(di->di_flags, name, FALSE))
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Return TRUE if di_flags "flags" indicates variable "name" is read-only.
 * Also give an error message.
 */
    ಪೂರ್ಣಾಙ್ಕ
var_check_ro(ಪೂರ್ಣಾಙ್ಕ flags, char_u *name, ಪೂರ್ಣಾಙ್ಕ use_gettext)
{
    ಯದಿ (flags & DI_FLAGS_RO)
    {
	ಯದಿ (name == ರಿಕ್ತ)
	    emsg(_(e_cannot_change_readonly_variable));
	ಅನ್ಯಥಾ
	    semsg(_(e_cannot_change_readonly_variable_str),
				       use_gettext ? (char_u *)_(name) : name);
	ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಯದಿ ((flags & DI_FLAGS_RO_SBX) && sandbox)
    {
	ಯದಿ (name == ರಿಕ್ತ)
	    emsg(_(e_cannot_set_variable_in_sandbox));
	ಅನ್ಯಥಾ
	    semsg(_(e_cannot_set_variable_in_sandbox_str),
				       use_gettext ? (char_u *)_(name) : name);
	ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Return TRUE if di_flags "flags" indicates variable "name" is locked.
 * Also give an error message.
 */
    ಪೂರ್ಣಾಙ್ಕ
var_check_lock(ಪೂರ್ಣಾಙ್ಕ flags, char_u *name, ಪೂರ್ಣಾಙ್ಕ use_gettext)
{
    ಯದಿ (flags & DI_FLAGS_LOCK)
    {
	semsg(_(e_variable_is_locked_str),
				       use_gettext ? (char_u *)_(name) : name);
	ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Return TRUE if di_flags "flags" indicates variable "name" is fixed.
 * Also give an error message.
 */
    ಪೂರ್ಣಾಙ್ಕ
var_check_fixed(ಪೂರ್ಣಾಙ್ಕ flags, char_u *name, ಪೂರ್ಣಾಙ್ಕ use_gettext)
{
    ಯದಿ (flags & DI_FLAGS_FIX)
    {
	ಯದಿ (name == ರಿಕ್ತ)
	    emsg(_(e_cannot_delete_variable));
	ಅನ್ಯಥಾ
	    semsg(_(e_cannot_delete_variable_str),
				      use_gettext ? (char_u *)_(name) : name);
	ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Check if a funcref is assigned to a valid variable name.
 * Return TRUE and give an error if not.
 */
    ಪೂರ್ಣಾಙ್ಕ
var_wrong_func_name(
    char_u *name,    // points to start of variable name
    ಪೂರ್ಣಾಙ್ಕ    new_var)  // TRUE when creating the variable
{
    // Allow for w: b: s: and t:.  In Vim9 script s: is not allowed, because
    // the name can be used without the s: prefix.
    // Allow autoload variable.
    ಯದಿ (!((vim_strchr((char_u *)"wbt", name[0]) != ರಿಕ್ತ
		    || (!in_vim9script() && name[0] == 's')) && name[1] == ':')
	    && !ASCII_ISUPPER((name[0] != NUL && name[1] == ':')
						     ? name[2] : name[0])
	    && vim_strchr(name, '#') == ರಿಕ್ತ)
    {
	semsg(_(e_funcref_variable_name_must_start_with_capital_str), name);
	ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    // Don't allow hiding a function.  When "v" is not NULL we might be
    // assigning another function to the same var, the type is checked
    // below.
    ಯದಿ (new_var && function_exists(name, FALSE))
    {
	semsg(_(e_variable_name_conflicts_with_existing_function_str),
								    name);
	ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Return TRUE if "flags" indicates variable "name" has a locked (immutable)
 * value.  Also give an error message, using "name" or _("name") when
 * "use_gettext" is TRUE.
 */
    ಪೂರ್ಣಾಙ್ಕ
ಮೌಲ್ಯ_check_lock(ಪೂರ್ಣಾಙ್ಕ lock, char_u *name, ಪೂರ್ಣಾಙ್ಕ use_gettext)
{
    ಯದಿ (lock & VAR_LOCKED)
    {
	ಯದಿ (name == ರಿಕ್ತ)
	    emsg(_(e_ಮೌಲ್ಯ_is_locked));
	ಅನ್ಯಥಾ
	    semsg(_(e_ಮೌಲ್ಯ_is_locked_str),
				       use_gettext ? (char_u *)_(name) : name);
	ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಯದಿ (lock & VAR_FIXED)
    {
	ಯದಿ (name == ರಿಕ್ತ)
	    emsg(_(e_cannot_change_ಮೌಲ್ಯ));
	ಅನ್ಯಥಾ
	    semsg(_(e_cannot_change_ಮೌಲ್ಯ_of_str),
				       use_gettext ? (char_u *)_(name) : name);
	ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Check if a variable name is valid.  When "autoload" is true "#" is allowed.
 * If "len" is -1 use all of "varname", otherwise up to "varname[len]".
 * Return FALSE and give an error if not.
 */
    ಪೂರ್ಣಾಙ್ಕ
valid_varname(char_u *varname, ಪೂರ್ಣಾಙ್ಕ len, ಪೂರ್ಣಾಙ್ಕ autoload)
{
    char_u *p;

    ಪ್ರತಿ (p = varname; len < 0 ? *p != NUL : p < varname + len; ++p)
	ಯದಿ (!eval_isnamec1(*p) && (p == varname || !VIM_ISDIGIT(*p))
					 && !(autoload && *p == AUTOLOAD_CHAR))
	{
	    semsg(_(e_illegal_variable_name_str), varname);
	    ಪ್ರತ್ಯಾಗಮ FALSE;
	}
    ಪ್ರತ್ಯಾಗಮ TRUE;
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
    ಪೂರ್ಣಾಙ್ಕ		htname,	    // 't'ab, 'w'indow or 'b'uffer local.
    tabpage_T	*tp,	    // can be NULL
    win_T	*win,
    buf_T	*buf)	    // Ignored if htname is not 'b'.
{
    dictitem_T	*v;
    ಪೂರ್ಣಾಙ್ಕ		done = FALSE;
    switchwin_T	switchwin;
    ಪೂರ್ಣಾಙ್ಕ		need_switch_win;
    ಪೂರ್ಣಾಙ್ಕ		do_change_curbuf = buf != ರಿಕ್ತ && htname == 'b';

    ++emsg_off;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ರಿಕ್ತ;

    ಯದಿ (varname != ರಿಕ್ತ && tp != ರಿಕ್ತ && win != ರಿಕ್ತ
	    && (htname != 'b' || buf != ರಿಕ್ತ))
    {
	// Set curwin to be our win, temporarily.  Also set the tabpage,
	// otherwise the window is not valid. Only do this when needed,
	// autocommands get blocked.
	// If we have a buffer reference avoid the switching, we're saving and
	// restoring curbuf directly.
	need_switch_win = !(tp == curtab && win == curwin) && !do_change_curbuf;
	ಯದಿ (!need_switch_win || switch_win(&switchwin, win, tp, TRUE) == OK)
	{
	    // Handle options. There are no tab-local options.
	    ಯದಿ (*varname == '&' && htname != 't')
	    {
		buf_T	*save_curbuf = curbuf;

		// Change curbuf so the option is read from the correct buffer.
		ಯದಿ (do_change_curbuf)
		    curbuf = buf;

		ಯದಿ (varname[1] == NUL)
		{
		    // get all window-local or buffer-local options in a dict
		    dict_T	*opts = get_winbuf_options(htname == 'b');

		    ಯದಿ (opts != ರಿಕ್ತ)
		    {
			rettv_dict_set(rettv, opts);
			done = TRUE;
		    }
		}
		ಅನ್ಯಥಾ ಯದಿ (eval_option(&varname, rettv, TRUE) == OK)
		    // Local option
		    done = TRUE;

		curbuf = save_curbuf;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (*varname == NUL)
	    {
		// Empty string: return a dict with all the local variables.
		ಯದಿ (htname == 'b')
		    v = &buf->b_bufvar;
		ಅನ್ಯಥಾ ಯದಿ (htname == 'w')
		    v = &win->w_winvar;
		ಅನ್ಯಥಾ
		    v = &tp->tp_winvar;
		copy_tv(&v->di_tv, rettv);
		done = TRUE;
	    }
	    ಅನ್ಯಥಾ
	    {
		hashtab_T	*ht;

		ಯದಿ (htname == 'b')
		    ht = &buf->b_vars->dv_hashtab;
		ಅನ್ಯಥಾ ಯದಿ (htname == 'w')
		    ht = &win->w_vars->dv_hashtab;
		ಅನ್ಯಥಾ
		    ht = &tp->tp_vars->dv_hashtab;

		// Look up the variable.
		v = find_var_in_ht(ht, htname, varname, FALSE);
		ಯದಿ (v != ರಿಕ್ತ)
		{
		    copy_tv(&v->di_tv, rettv);
		    done = TRUE;
		}
	    }
	}

	ಯದಿ (need_switch_win)
	    // restore previous notion of curwin
	    restore_win(&switchwin, TRUE);
    }

    ಯದಿ (!done && deftv->v_type != VAR_UNKNOWN)
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
    ಪೂರ್ಣಾಙ್ಕ		off)	    // 1 for gettabwinvar()
{
    char_u	*varname;
    tabpage_T	*tp;
    win_T	*win;

    ಯದಿ (off == 1)
	tp = find_tabpage((ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[0], ರಿಕ್ತ));
    ಅನ್ಯಥಾ
	tp = curtab;
    win = find_win_by_nr(&argvars[off], tp);
    varname = tv_get_string_chk(&argvars[off + 1]);

    get_var_from(varname, rettv, &argvars[off + 2], 'w', tp, win, ರಿಕ್ತ);
}

/*
 * Set option "varname" to the value of "varp" for the current buffer/window.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
set_option_from_tv(char_u *varname, typval_T *varp)
{
    long	numval = 0;
    char_u	*strval;
    char_u	nbuf[NUMBUFLEN];
    ಪೂರ್ಣಾಙ್ಕ		error = FALSE;

    ಪೂರ್ಣಾಙ್ಕ		opt_idx = findoption(varname);
    ಯದಿ (opt_idx < 0)
    {
	semsg(_(e_unknown_option_str_2), varname);
	ಪ್ರತ್ಯಾಗಮ;
    }
    ಪೂರ್ಣಾಙ್ಕ		opt_p_flags = get_option_flags(opt_idx);

    ಯದಿ (varp->v_type == VAR_BOOL)
    {
	ಯದಿ (opt_p_flags & P_STRING)
	{
	    emsg(_(e_string_required));
	    ಪ್ರತ್ಯಾಗಮ;
	}
	numval = (long)varp->vval.v_ಸಂಖ್ಯೆ;
	strval = (char_u *)"0";  // avoid using "false"
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ ((opt_p_flags & (P_ಅಂಕೆ|P_BOOL))
		&& (!in_vim9script() || varp->v_type != VAR_STRING))
	    numval = (long)tv_get_ಸಂಖ್ಯೆ_chk(varp, &error);
	ಯದಿ (!error)
	    strval = tv_get_string_buf_chk(varp, nbuf);
    }
    ಯದಿ (!error && strval != ರಿಕ್ತ)
	set_option_ಮೌಲ್ಯ_give_err(varname, numval, strval, OPT_LOCAL);
}

/*
 * "setwinvar()" and "settabwinvar()" functions
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
setwinvar(typval_T *argvars, ಪೂರ್ಣಾಙ್ಕ off)
{
    win_T	*win;
    switchwin_T	switchwin;
    ಪೂರ್ಣಾಙ್ಕ		need_switch_win;
    char_u	*varname, *winvarname;
    typval_T	*varp;
    tabpage_T	*tp = ರಿಕ್ತ;

    ಯದಿ (check_secure())
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (off == 1)
	tp = find_tabpage((ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[0], ರಿಕ್ತ));
    ಅನ್ಯಥಾ
	tp = curtab;
    win = find_win_by_nr(&argvars[off], tp);
    varname = tv_get_string_chk(&argvars[off + 1]);
    varp = &argvars[off + 2];

    ಯದಿ (win == ರಿಕ್ತ || varname == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    need_switch_win = !(tp == curtab && win == curwin);
    ಯದಿ (!need_switch_win
	    || switch_win(&switchwin, win, tp, TRUE) == OK)
    {
	ಯದಿ (*varname == '&')
	    set_option_from_tv(varname + 1, varp);
	ಅನ್ಯಥಾ
	{
	    winvarname = alloc(STRLEN(varname) + 3);
	    ಯದಿ (winvarname != ರಿಕ್ತ)
	    {
		STRCPY(winvarname, "w:");
		STRCPY(winvarname + 2, varname);
		set_var(winvarname, varp, TRUE);
		vim_free(winvarname);
	    }
	}
    }
    ಯದಿ (need_switch_win)
	restore_win(&switchwin, TRUE);
}

/*
 * reset v:option_new, v:option_old, v:option_oldlocal, v:option_oldglobal,
 * v:option_type, and v:option_command.
 */
    ಶೂನ್ಯ
reset_v_option_vars(ಶೂನ್ಯ)
{
    set_vim_var_string(VV_OPTION_NEW, ರಿಕ್ತ, -1);
    set_vim_var_string(VV_OPTION_OLD, ರಿಕ್ತ, -1);
    set_vim_var_string(VV_OPTION_OLDLOCAL, ರಿಕ್ತ, -1);
    set_vim_var_string(VV_OPTION_OLDGLOBAL, ರಿಕ್ತ, -1);
    set_vim_var_string(VV_OPTION_TYPE, ರಿಕ್ತ, -1);
    set_vim_var_string(VV_OPTION_COMMAND, ರಿಕ್ತ, -1);
}

/*
 * Add an assert error to v:errors.
 */
    ಶೂನ್ಯ
assert_error(garray_T *gap)
{
    ಸಂರಚನಾ vimvar   *vp = &vimvars[VV_ERRORS];

    ಯದಿ (vp->vv_tv_type != VAR_LIST || vimvars[VV_ERRORS].vv_list == ರಿಕ್ತ)
	// Make sure v:errors is a list.
	set_vim_var_list(VV_ERRORS, list_alloc());
    list_append_string(vimvars[VV_ERRORS].vv_list, gap->ga_data, gap->ga_len);
}

    ಪೂರ್ಣಾಙ್ಕ
var_exists(char_u *var)
{
    char_u	*arg = var;
    char_u	*name;
    char_u	*tofree;
    typval_T    tv;
    ಪೂರ್ಣಾಙ್ಕ		len = 0;
    ಪೂರ್ಣಾಙ್ಕ		n = FALSE;

    // get_name_len() takes care of expanding curly braces
    name = var;
    len = get_name_len(&arg, &tofree, TRUE, FALSE);
    ಯದಿ (len > 0)
    {
	ಯದಿ (tofree != ರಿಕ್ತ)
	    name = tofree;
	n = (eval_variable(name, len, 0, &tv, ರಿಕ್ತ,
				 EVAL_VAR_NOAUTOLOAD + EVAL_VAR_IMPORT) == OK);
	ಯದಿ (n)
	{
	    // handle d.key, l[idx], f(expr)
	    arg = skipwhite(arg);
	    n = (handle_subscript(&arg, name, &tv, &EVALARG_EVALUATE,
								 FALSE) == OK);
	    ಯದಿ (n)
		clear_tv(&tv);
	}
    }
    ಯದಿ (*arg != NUL)
	n = FALSE;

    vim_free(tofree);
    ಪ್ರತ್ಯಾಗಮ n;
}

ಸ್ಥಿರ lval_T	*redir_lval = ರಿಕ್ತ;
#define EVALCMD_BUSY (redir_lval == (lval_T *)&redir_lval)
ಸ್ಥಿರ garray_T redir_ga;	// only valid when redir_lval is not NULL
ಸ್ಥಿರ char_u	*redir_endp = ರಿಕ್ತ;
ಸ್ಥಿರ char_u	*redir_varname = ರಿಕ್ತ;

    ಪೂರ್ಣಾಙ್ಕ
alloc_redir_lval(ಶೂನ್ಯ)
{
    redir_lval = ALLOC_CLEAR_ONE(lval_T);
    ಯದಿ (redir_lval == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಪ್ರತ್ಯಾಗಮ OK;
}

    ಶೂನ್ಯ
clear_redir_lval(ಶೂನ್ಯ)
{
    VIM_CLEAR(redir_lval);
}

    ಶೂನ್ಯ
init_redir_ga(ಶೂನ್ಯ)
{
    ga_init2(&redir_ga, ಪರಿಮಾಣ(ವರ್ಣ), 500);
}

/*
 * Start recording command output to a variable
 * When "append" is TRUE append to an existing variable.
 * Returns OK if successfully completed the setup.  FAIL otherwise.
 */
    ಪೂರ್ಣಾಙ್ಕ
var_redir_start(char_u *name, ಪೂರ್ಣಾಙ್ಕ append)
{
    ಪೂರ್ಣಾಙ್ಕ		called_emsg_before;
    typval_T	tv;

    // Catch a bad name early.
    ಯದಿ (!eval_isnamec1(*name))
    {
	emsg(_(e_invalid_argument));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    // Make a copy of the name, it is used in redir_lval until redir ends.
    redir_varname = vim_strsave(name);
    ಯದಿ (redir_varname == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (alloc_redir_lval() == FAIL)
    {
	var_redir_stop();
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    // The output is stored in growarray "redir_ga" until redirection ends.
    init_redir_ga();

    // Parse the variable name (can be a dict or list entry).
    redir_endp = get_lval(redir_varname, ರಿಕ್ತ, redir_lval, FALSE, FALSE, 0,
							     FNE_CHECK_START);
    ಯದಿ (redir_endp == ರಿಕ್ತ || redir_lval->ll_name == ರಿಕ್ತ || *redir_endp != NUL)
    {
	clear_lval(redir_lval);
	ಯದಿ (redir_endp != ರಿಕ್ತ && *redir_endp != NUL)
	    // Trailing characters are present after the variable name
	    semsg(_(e_trailing_characters_str), redir_endp);
	ಅನ್ಯಥಾ
	    semsg(_(e_invalid_argument_str), name);
	redir_endp = ರಿಕ್ತ;  // don't store a value, only cleanup
	var_redir_stop();
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    // check if we can write to the variable: set it to or append an empty
    // string
    called_emsg_before = called_emsg;
    tv.v_type = VAR_STRING;
    tv.vval.v_string = (char_u *)"";
    ಯದಿ (append)
	set_var_lval(redir_lval, redir_endp, &tv, TRUE,
					     ASSIGN_NO_DECL, (char_u *)".", 0);
    ಅನ್ಯಥಾ
	set_var_lval(redir_lval, redir_endp, &tv, TRUE,
					     ASSIGN_NO_DECL, (char_u *)"=", 0);
    clear_lval(redir_lval);
    ಯದಿ (called_emsg > called_emsg_before)
    {
	redir_endp = ರಿಕ್ತ;  // don't store a value, only cleanup
	var_redir_stop();
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಪ್ರತ್ಯಾಗಮ OK;
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
var_redir_str(char_u *ಮೌಲ್ಯ, ಪೂರ್ಣಾಙ್ಕ ಮೌಲ್ಯ_len)
{
    ಪೂರ್ಣಾಙ್ಕ		len;

    ಯದಿ (redir_lval == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (ಮೌಲ್ಯ_len == -1)
	len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(ಮೌಲ್ಯ);	// Append the entire string
    ಅನ್ಯಥಾ
	len = ಮೌಲ್ಯ_len;		// Append only "value_len" characters

    ಯದಿ (ga_grow(&redir_ga, len) == OK)
    {
	mch_memmove((ವರ್ಣ *)redir_ga.ga_data + redir_ga.ga_len, ಮೌಲ್ಯ, len);
	redir_ga.ga_len += len;
    }
    ಅನ್ಯಥಾ
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

    ಯದಿ (EVALCMD_BUSY)
    {
	redir_lval = ರಿಕ್ತ;
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (redir_lval != ರಿಕ್ತ)
    {
	// If there was no error: assign the text to the variable.
	ಯದಿ (redir_endp != ರಿಕ್ತ)
	{
	    ga_append(&redir_ga, NUL);  // Append the trailing NUL.
	    tv.v_type = VAR_STRING;
	    tv.vval.v_string = redir_ga.ga_data;
	    // Call get_lval() again, if it's inside a Dict or List it may
	    // have changed.
	    redir_endp = get_lval(redir_varname, ರಿಕ್ತ, redir_lval,
					FALSE, FALSE, 0, FNE_CHECK_START);
	    ಯದಿ (redir_endp != ರಿಕ್ತ && redir_lval->ll_name != ರಿಕ್ತ)
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
get_clear_redir_ga(ಶೂನ್ಯ)
{
    char_u *res;

    ga_append(&redir_ga, NUL);  // Append the trailing NUL.
    res = redir_ga.ga_data;
    redir_ga.ga_data = ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ res;
}

/*
 * "gettabvar()" function
 */
    ಶೂನ್ಯ
f_gettabvar(typval_T *argvars, typval_T *rettv)
{
    char_u	*varname;
    tabpage_T	*tp;
    win_T	*win = ರಿಕ್ತ;

    ಯದಿ (in_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    varname = tv_get_string_chk(&argvars[1]);
    tp = find_tabpage((ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[0], ರಿಕ್ತ));
    ಯದಿ (tp != ರಿಕ್ತ)
	win = tp == curtab || tp->tp_firstwin == ರಿಕ್ತ ? firstwin
		: tp->tp_firstwin;

    get_var_from(varname, rettv, &argvars[2], 't', tp, win, ರಿಕ್ತ);
}

/*
 * "gettabwinvar()" function
 */
    ಶೂನ್ಯ
f_gettabwinvar(typval_T *argvars, typval_T *rettv)
{
    ಯದಿ (in_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    getwinvar(argvars, rettv, 1);
}

/*
 * "getwinvar()" function
 */
    ಶೂನ್ಯ
f_getwinvar(typval_T *argvars, typval_T *rettv)
{
    ಯದಿ (in_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

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

    ಯದಿ (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    varname = tv_get_string_chk(&argvars[1]);
    buf = tv_get_buf_from_arg(&argvars[0]);

    get_var_from(varname, rettv, &argvars[2], 'b', curtab, curwin, buf);
}

/*
 * "settabvar()" function
 */
    ಶೂನ್ಯ
f_settabvar(typval_T *argvars, typval_T *rettv UNUSED)
{
    tabpage_T	*save_curtab;
    tabpage_T	*tp;
    tabpage_T	*save_lu_tp;
    char_u	*varname, *tabvarname;
    typval_T	*varp;

    ಯದಿ (check_secure())
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (in_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    tp = find_tabpage((ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[0], ರಿಕ್ತ));
    varname = tv_get_string_chk(&argvars[1]);
    varp = &argvars[2];

    ಯದಿ (varname == ರಿಕ್ತ || tp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    save_curtab = curtab;
    save_lu_tp = lastused_tabpage;
    goto_tabpage_tp(tp, FALSE, FALSE);

    tabvarname = alloc(STRLEN(varname) + 3);
    ಯದಿ (tabvarname != ರಿಕ್ತ)
    {
	STRCPY(tabvarname, "t:");
	STRCPY(tabvarname + 2, varname);
	set_var(tabvarname, varp, TRUE);
	vim_free(tabvarname);
    }

    // Restore current tabpage and last accessed tabpage.
    ಯದಿ (valid_tabpage(save_curtab))
    {
	goto_tabpage_tp(save_curtab, FALSE, FALSE);
	ಯದಿ (valid_tabpage(save_lu_tp))
	    lastused_tabpage = save_lu_tp;
    }
}

/*
 * "settabwinvar()" function
 */
    ಶೂನ್ಯ
f_settabwinvar(typval_T *argvars, typval_T *rettv UNUSED)
{
    ಯದಿ (in_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    setwinvar(argvars, 1);
}

/*
 * "setwinvar()" function
 */
    ಶೂನ್ಯ
f_setwinvar(typval_T *argvars, typval_T *rettv UNUSED)
{
    ಯದಿ (in_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    setwinvar(argvars, 0);
}

/*
 * "setbufvar()" function
 */
    ಶೂನ್ಯ
f_setbufvar(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    char_u	*varname, *bufvarname;
    typval_T	*varp;

    ಯದಿ (check_secure())
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    varname = tv_get_string_chk(&argvars[1]);
    buf = tv_get_buf_from_arg(&argvars[0]);
    varp = &argvars[2];

    ಯದಿ (buf == ರಿಕ್ತ || varname == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (*varname == '&')
    {
	aco_save_T	aco;
	// safe the current window position, it could
	// change because of 'scrollbind' window-local
	// options
	linenr_T	old_topline = curwin->w_topline;

	// Set curbuf to be our buf, temporarily.
	aucmd_prepbuf(&aco, buf);
	ಯದಿ (curbuf == buf)
	{
	    // Only when it worked to set "curbuf".
	    set_option_from_tv(varname + 1, varp);

	    // reset notion of buffer
	    aucmd_restbuf(&aco);
	}
	curwin->w_topline = old_topline;
    }
    ಅನ್ಯಥಾ
    {
	bufvarname = alloc(STRLEN(varname) + 3);
	ಯದಿ (bufvarname != ರಿಕ್ತ)
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
    ಪೂರ್ಣಾಙ್ಕ		r = OK;

    CLEAR_FIELD(res);
    ಯದಿ (arg->v_type == VAR_PARTIAL && arg->vval.v_partial != ರಿಕ್ತ)
    {
	res.cb_partial = arg->vval.v_partial;
	++res.cb_partial->pt_refcount;
	res.cb_name = partial_name(res.cb_partial);
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (arg->v_type == VAR_STRING && arg->vval.v_string != ರಿಕ್ತ
					       && SAFE_isdigit(*arg->vval.v_string))
	    r = FAIL;
	ಅನ್ಯಥಾ ಯದಿ (arg->v_type == VAR_FUNC || arg->v_type == VAR_STRING)
	{
	    res.cb_name = arg->vval.v_string;
	    ಯದಿ (arg->v_type == VAR_STRING)
	    {
		char_u *name = get_scriptlocal_funcname(arg->vval.v_string);
		ಯದಿ (name != ರಿಕ್ತ)
		{
		    res.cb_name = name;
		    res.cb_free_name = TRUE;
		}
	    }
	    func_ref(res.cb_name);
	}
	ಅನ್ಯಥಾ ಯದಿ (arg->v_type == VAR_ಸಂಖ್ಯೆ && arg->vval.v_ಸಂಖ್ಯೆ == 0)
	    res.cb_name = (char_u *)"";
	ಅನ್ಯಥಾ
	    r = FAIL;

	ಯದಿ (r == FAIL)
	{
	    emsg(_(e_invalid_callback_argument));
	    res.cb_name = ರಿಕ್ತ;
	}
    }
    ಪ್ರತ್ಯಾಗಮ res;
}

/*
 * Copy a callback into a typval_T.
 */
    ಶೂನ್ಯ
put_callback(callback_T *cb, typval_T *tv)
{
    ಯದಿ (cb->cb_partial != ರಿಕ್ತ)
    {
	tv->v_type = VAR_PARTIAL;
	tv->vval.v_partial = cb->cb_partial;
	++tv->vval.v_partial->pt_refcount;
    }
    ಅನ್ಯಥಾ
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
    ಪ್ರತ್ಯಾಗಮ cb->cb_partial || cb->cb_free_name;
}

/*
 * Make a copy of "src" into "dest", allocating the function name if needed,
 * without incrementing the refcount.
 */
    ಶೂನ್ಯ
set_callback(callback_T *dest, callback_T *src)
{
    *dest = *src;
    ಯದಿ (!does_callback_own_cb_name(src))
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
    ಯದಿ (dest->cb_partial != ರಿಕ್ತ)
    {
	dest->cb_name = src->cb_name;
	dest->cb_free_name = FALSE;
	++dest->cb_partial->pt_refcount;
    }
    ಅನ್ಯಥಾ
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

    ಯದಿ (!in_vim9script() || cb->cb_name == ರಿಕ್ತ
	    || (!cb->cb_free_name
	       && (cb->cb_partial == ರಿಕ್ತ || cb->cb_partial->pt_name == ರಿಕ್ತ)))
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (cb->cb_partial != ರಿಕ್ತ)
	name = cb->cb_partial->pt_name;
    ಅನ್ಯಥಾ
	name = cb->cb_name;
    p = vim_strchr(name, '.');
    ಯದಿ (p == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    import = find_imported(name, p - name, FALSE);
    ಯದಿ (import == ರಿಕ್ತ || !SCRIPT_ID_VALID(import->imp_sid))
	ಪ್ರತ್ಯಾಗಮ;

    scriptitem_T *si = SCRIPT_ITEM(import->imp_sid);
    ಯದಿ (si->sn_autoload_prefix == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    char_u *newname = concat_str(si->sn_autoload_prefix, p + 1);
    ಯದಿ (newname == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (cb->cb_partial != ರಿಕ್ತ)
    {
	ಯದಿ (cb->cb_name == cb->cb_partial->pt_name)
	    cb->cb_name = newname;
	vim_free(cb->cb_partial->pt_name);
	cb->cb_partial->pt_name = newname;
    }
    ಅನ್ಯಥಾ
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
    ಯದಿ (callback->cb_partial != ರಿಕ್ತ)
    {
	partial_unref(callback->cb_partial);
	callback->cb_partial = ರಿಕ್ತ;
    }
    ಅನ್ಯಥಾ ಯದಿ (callback->cb_name != ರಿಕ್ತ)
	func_unref(callback->cb_name);
    ಯದಿ (callback->cb_free_name)
    {
	vim_free(callback->cb_name);
	callback->cb_free_name = FALSE;
    }
    callback->cb_name = ರಿಕ್ತ;
}

#endif // FEAT_EVAL
