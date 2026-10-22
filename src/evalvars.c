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

#ଅନ୍ତର୍ଭାଵ "vim.h"

#ଯଦି defined(FEAT_EVAL)

ସ୍ଥିର dictitem_T	globvars_var;		// variable used for g:
ସ୍ଥିର dict_T		globvardict;		// Dictionary with g: variables
#define globvarht globvardict.dv_hashtab

/*
 * Old Vim variables such as "v:version" are also available without the "v:".
 * Also in functions.  We need a special hashtable for them.
 */
ସ୍ଥିର hashtab_T	compat_hashtab;

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

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା vimvar vimvar_T;

ସ୍ଥିର ସଂରଚନା vimvar
{
    ଅକ୍ଷର	*vv_name;	// name of variable, without v:
    dictitem16_T vv_di;		// value and name for key (max 16 chars!)
    type_T	*vv_type;	// type or NULL
    ଅକ୍ଷର	vv_flags;	// VV_COMPAT, VV_RO, VV_RO_SBX
} vimvars[VV_LEN] =
{
    // The order here must match the VV_ defines in vim.h!
    // Initializing a union does not work, leave tv.vval empty to get zero's.
    {VV_NAME("count",		 VAR_ସଂଖ୍ୟା), ଖାଲି, VV_COMPAT+VV_RO},
    {VV_NAME("count1",		 VAR_ସଂଖ୍ୟା), ଖାଲି, VV_RO},
    {VV_NAME("prevcount",	 VAR_ସଂଖ୍ୟା), ଖାଲି, VV_RO},
    {VV_NAME("errmsg",		 VAR_STRING), ଖାଲି, VV_COMPAT},
    {VV_NAME("warningmsg",	 VAR_STRING), ଖାଲି, 0},
    {VV_NAME("statusmsg",	 VAR_STRING), ଖାଲି, 0},
    {VV_NAME("shell_error",	 VAR_ସଂଖ୍ୟା), ଖାଲି, VV_COMPAT+VV_RO},
    {VV_NAME("this_session",	 VAR_STRING), ଖାଲି, VV_COMPAT},
    {VV_NAME("version",		 VAR_ସଂଖ୍ୟା), ଖାଲି, VV_COMPAT+VV_RO},
    {VV_NAME("lnum",		 VAR_ସଂଖ୍ୟା), ଖାଲି, VV_RO_SBX},
    {VV_NAME("termresponse",	 VAR_STRING), ଖାଲି, VV_RO},
    {VV_NAME("fname",		 VAR_STRING), ଖାଲି, VV_RO},
    {VV_NAME("lang",		 VAR_STRING), ଖାଲି, VV_RO},
    {VV_NAME("lc_time",		 VAR_STRING), ଖାଲି, VV_RO},
    {VV_NAME("ctype",		 VAR_STRING), ଖାଲି, VV_RO},
    {VV_NAME("charconvert_from", VAR_STRING), ଖାଲି, VV_RO},
    {VV_NAME("charconvert_to",	 VAR_STRING), ଖାଲି, VV_RO},
    {VV_NAME("fname_in",	 VAR_STRING), ଖାଲି, VV_RO},
    {VV_NAME("fname_out",	 VAR_STRING), ଖାଲି, VV_RO},
    {VV_NAME("fname_new",	 VAR_STRING), ଖାଲି, VV_RO},
    {VV_NAME("fname_diff",	 VAR_STRING), ଖାଲି, VV_RO},
    {VV_NAME("cmdarg",		 VAR_STRING), ଖାଲି, VV_RO},
    {VV_NAME("foldstart",	 VAR_ସଂଖ୍ୟା), ଖାଲି, VV_RO_SBX},
    {VV_NAME("foldend",		 VAR_ସଂଖ୍ୟା), ଖାଲି, VV_RO_SBX},
    {VV_NAME("folddashes",	 VAR_STRING), ଖାଲି, VV_RO_SBX},
    {VV_NAME("foldlevel",	 VAR_ସଂଖ୍ୟା), ଖାଲି, VV_RO_SBX},
    {VV_NAME("progname",	 VAR_STRING), ଖାଲି, VV_RO},
    {VV_NAME("servername",	 VAR_STRING), ଖାଲି, VV_RO},
    {VV_NAME("dying",		 VAR_ସଂଖ୍ୟା), ଖାଲି, VV_RO},
    {VV_NAME("exception",	 VAR_STRING), ଖାଲି, VV_RO},
    {VV_NAME("throwpoint",	 VAR_STRING), ଖାଲି, VV_RO},
    {VV_NAME("register",	 VAR_STRING), ଖାଲି, VV_RO},
    {VV_NAME("cmdbang",		 VAR_ସଂଖ୍ୟା), ଖାଲି, VV_RO},
    {VV_NAME("insertmode",	 VAR_STRING), ଖାଲି, VV_RO},
    {VV_NAME("val",		 VAR_UNKNOWN), ଖାଲି, VV_RO},
    {VV_NAME("key",		 VAR_UNKNOWN), ଖାଲି, VV_RO},
    {VV_NAME("profiling",	 VAR_ସଂଖ୍ୟା), ଖାଲି, VV_RO},
    {VV_NAME("fcs_reason",	 VAR_STRING), ଖାଲି, VV_RO},
    {VV_NAME("fcs_choice",	 VAR_STRING), ଖାଲି, 0},
    {VV_NAME("beval_bufnr",	 VAR_ସଂଖ୍ୟା), ଖାଲି, VV_RO},
    {VV_NAME("beval_winnr",	 VAR_ସଂଖ୍ୟା), ଖାଲି, VV_RO},
    {VV_NAME("beval_winid",	 VAR_ସଂଖ୍ୟା), ଖାଲି, VV_RO},
    {VV_NAME("beval_lnum",	 VAR_ସଂଖ୍ୟା), ଖାଲି, VV_RO},
    {VV_NAME("beval_col",	 VAR_ସଂଖ୍ୟା), ଖାଲି, VV_RO},
    {VV_NAME("beval_text",	 VAR_STRING), ଖାଲି, VV_RO},
    {VV_NAME("scrollstart",	 VAR_STRING), ଖାଲି, 0},
    {VV_NAME("swapname",	 VAR_STRING), ଖାଲି, VV_RO},
    {VV_NAME("swapchoice",	 VAR_STRING), ଖାଲି, 0},
    {VV_NAME("swapcommand",	 VAR_STRING), ଖାଲି, VV_RO},
    {VV_NAME("char",		 VAR_STRING), ଖାଲି, 0},
    {VV_NAME("mouse_win",	 VAR_ସଂଖ୍ୟା), ଖାଲି, 0},
    {VV_NAME("mouse_winid",	 VAR_ସଂଖ୍ୟା), ଖାଲି, 0},
    {VV_NAME("mouse_lnum",	 VAR_ସଂଖ୍ୟା), ଖାଲି, 0},
    {VV_NAME("mouse_col",	 VAR_ସଂଖ୍ୟା), ଖାଲି, 0},
    {VV_NAME("operator",	 VAR_STRING), ଖାଲି, VV_RO},
    {VV_NAME("searchforward",	 VAR_ସଂଖ୍ୟା), ଖାଲି, 0},
    {VV_NAME("hlsearch",	 VAR_ସଂଖ୍ୟା), ଖାଲି, 0},
    {VV_NAME("oldfiles",	 VAR_LIST), &t_list_string, 0},
    {VV_NAME("windowid",	 VAR_ସଂଖ୍ୟା), ଖାଲି, VV_RO},
    {VV_NAME("progpath",	 VAR_STRING), ଖାଲି, VV_RO},
    {VV_NAME("completed_item",	 VAR_DICT), &t_dict_string, 0},
    {VV_NAME("option_new",	 VAR_STRING), ଖାଲି, VV_RO},
    {VV_NAME("option_old",	 VAR_STRING), ଖାଲି, VV_RO},
    {VV_NAME("option_oldlocal",	 VAR_STRING), ଖାଲି, VV_RO},
    {VV_NAME("option_oldglobal", VAR_STRING), ଖାଲି, VV_RO},
    {VV_NAME("option_command",	 VAR_STRING), ଖାଲି, VV_RO},
    {VV_NAME("option_type",	 VAR_STRING), ଖାଲି, VV_RO},
    {VV_NAME("errors",		 VAR_LIST), &t_list_string, 0},
    {VV_NAME("false",		 VAR_BOOL), ଖାଲି, VV_RO},
    {VV_NAME("true",		 VAR_BOOL), ଖାଲି, VV_RO},
    {VV_NAME("none",		 VAR_SPECIAL), ଖାଲି, VV_RO},
    {VV_NAME("null",		 VAR_SPECIAL), ଖାଲି, VV_RO},
    {VV_NAME("numbermax",	 VAR_ସଂଖ୍ୟା), ଖାଲି, VV_RO},
    {VV_NAME("numbermin",	 VAR_ସଂଖ୍ୟା), ଖାଲି, VV_RO},
    {VV_NAME("numbersize",	 VAR_ସଂଖ୍ୟା), ଖାଲି, VV_RO},
    {VV_NAME("vim_did_enter",	 VAR_ସଂଖ୍ୟା), ଖାଲି, VV_RO},
    {VV_NAME("testing",		 VAR_ସଂଖ୍ୟା), ଖାଲି, 0},
    {VV_NAME("t_number",	 VAR_ସଂଖ୍ୟା), ଖାଲି, VV_RO},
    {VV_NAME("t_string",	 VAR_ସଂଖ୍ୟା), ଖାଲି, VV_RO},
    {VV_NAME("t_func",		 VAR_ସଂଖ୍ୟା), ଖାଲି, VV_RO},
    {VV_NAME("t_list",		 VAR_ସଂଖ୍ୟା), ଖାଲି, VV_RO},
    {VV_NAME("t_dict",		 VAR_ସଂଖ୍ୟା), ଖାଲି, VV_RO},
    {VV_NAME("t_float",		 VAR_ସଂଖ୍ୟା), ଖାଲି, VV_RO},
    {VV_NAME("t_bool",		 VAR_ସଂଖ୍ୟା), ଖାଲି, VV_RO},
    {VV_NAME("t_none",		 VAR_ସଂଖ୍ୟା), ଖାଲି, VV_RO},
    {VV_NAME("t_job",		 VAR_ସଂଖ୍ୟା), ଖାଲି, VV_RO},
    {VV_NAME("t_channel",	 VAR_ସଂଖ୍ୟା), ଖାଲି, VV_RO},
    {VV_NAME("t_blob",		 VAR_ସଂଖ୍ୟା), ଖାଲି, VV_RO},
    {VV_NAME("t_class",		 VAR_ସଂଖ୍ୟା), ଖାଲି, VV_RO},
    {VV_NAME("t_object",	 VAR_ସଂଖ୍ୟା), ଖାଲି, VV_RO},
    {VV_NAME("termrfgresp",	 VAR_STRING), ଖାଲି, VV_RO},
    {VV_NAME("termrbgresp",	 VAR_STRING), ଖାଲି, VV_RO},
    {VV_NAME("termu7resp",	 VAR_STRING), ଖାଲି, VV_RO},
    {VV_NAME("termstyleresp",	 VAR_STRING), ଖାଲି, VV_RO},
    {VV_NAME("termblinkresp",	 VAR_STRING), ଖାଲି, VV_RO},
    {VV_NAME("event",		 VAR_DICT), ଖାଲି, VV_RO},
    {VV_NAME("versionlong",	 VAR_ସଂଖ୍ୟା), ଖାଲି, VV_RO},
    {VV_NAME("echospace",	 VAR_ସଂଖ୍ୟା), ଖାଲି, VV_RO},
    {VV_NAME("argv",		 VAR_LIST), &t_list_string, VV_RO},
    {VV_NAME("collate",		 VAR_STRING), ଖାଲି, VV_RO},
    {VV_NAME("exiting",		 VAR_SPECIAL), ଖାଲି, VV_RO},
    {VV_NAME("colornames",       VAR_DICT), &t_dict_string, VV_RO},
    {VV_NAME("sizeofint",	 VAR_ସଂଖ୍ୟା), ଖାଲି, VV_RO},
    {VV_NAME("sizeoflong",	 VAR_ସଂଖ୍ୟା), ଖାଲି, VV_RO},
    {VV_NAME("sizeofpointer",	 VAR_ସଂଖ୍ୟା), ଖାଲି, VV_RO},
    {VV_NAME("maxcol",		 VAR_ସଂଖ୍ୟା), ଖାଲି, VV_RO},
    {VV_NAME("python3_version",	 VAR_ସଂଖ୍ୟା), ଖାଲି, VV_RO},
    {VV_NAME("t_typealias",	 VAR_ସଂଖ୍ୟା), ଖାଲି, VV_RO},
    {VV_NAME("t_enum",		 VAR_ସଂଖ୍ୟା), ଖାଲି, VV_RO},
    {VV_NAME("t_enumvalue",	 VAR_ସଂଖ୍ୟା), ଖାଲି, VV_RO},
    {VV_NAME("stacktrace",	 VAR_LIST), &t_list_dict_any, VV_RO},
    {VV_NAME("t_tuple",		 VAR_ସଂଖ୍ୟା), ଖାଲି, VV_RO},
    {VV_NAME("wayland_display",  VAR_STRING), ଖାଲି, VV_RO},
    {VV_NAME("clipmethod",	 VAR_STRING), ଖାଲି, VV_RO},
    {VV_NAME("termda1",		 VAR_STRING), ଖାଲି, VV_RO},
    {VV_NAME("termosc",	 VAR_STRING), ଖାଲି, VV_RO},
    {VV_NAME("vim_did_init",	 VAR_ସଂଖ୍ୟା), ଖାଲି, VV_RO},
    {VV_NAME("clipproviders",	 VAR_DICT), ଖାଲି, VV_RO},
    {VV_NAME("imagebackend",	 VAR_STRING), ଖାଲି, VV_RO}
};

// shorthand
#define vv_tv_type	vv_di.di_tv.v_type
#define vv_nr		vv_di.di_tv.vval.v_ସଂଖ୍ୟା
#define vv_float	vv_di.di_tv.vval.v_float
#define vv_str		vv_di.di_tv.vval.v_string
#define vv_list		vv_di.di_tv.vval.v_list
#define vv_dict		vv_di.di_tv.vval.v_dict
#define vv_blob		vv_di.di_tv.vval.v_blob
#define vv_tv		vv_di.di_tv

ସ୍ଥିର dictitem_T	vimvars_var;		// variable used for v:
ସ୍ଥିର dict_T		vimvardict;		// Dictionary with v: variables
#define vimvarht  vimvardict.dv_hashtab

// for VIM_VERSION_ defines
#ଅନ୍ତର୍ଭାଵ "version.h"

ସ୍ଥିର ଶୂନ୍ୟ list_glob_vars(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *first);
ସ୍ଥିର ଶୂନ୍ୟ list_buf_vars(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *first);
ସ୍ଥିର ଶୂନ୍ୟ list_win_vars(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *first);
ସ୍ଥିର ଶୂନ୍ୟ list_tab_vars(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *first);
ସ୍ଥିର char_u *list_arg_vars(exarg_T *eap, char_u *arg, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *first);
ସ୍ଥିର char_u *ex_let_one(char_u *arg, typval_T *tv, ପୂର୍ଣ୍ଣସଂଖ୍ୟା copy, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags, char_u *endchars, char_u *op, ପୂର୍ଣ୍ଣସଂଖ୍ୟା var_idx);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_unlet_var(lval_T *lp, char_u *name_end, exarg_T *eap, ପୂର୍ଣ୍ଣସଂଖ୍ୟା deep, ଶୂନ୍ୟ *cookie);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_lock_var(lval_T *lp, char_u *name_end, exarg_T *eap, ପୂର୍ଣ୍ଣସଂଖ୍ୟା deep, ଶୂନ୍ୟ *cookie);
ସ୍ଥିର ଶୂନ୍ୟ list_one_var(dictitem_T *v, ଅକ୍ଷର *prefix, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *first);
ସ୍ଥିର ଶୂନ୍ୟ list_one_var_a(ଅକ୍ଷର *prefix, char_u *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା type, char_u *string, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *first);

/*
 * Initialize global and vim special variables
 */
    ଶୂନ୍ୟ
evalvars_init(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    i;
    ସଂରଚନା vimvar   *p;

    init_var_dict(&globvardict, &globvars_var, VAR_DEF_SCOPE);
    init_var_dict(&vimvardict, &vimvars_var, VAR_SCOPE);
    vimvardict.dv_lock = VAR_FIXED;
    hash_init(&compat_hashtab);

    ପାଇଁ (i = 0; i < VV_LEN; ++i)
    {
	p = &vimvars[i];
	ଯଦି (p->vv_flags & VV_RO)
	    p->vv_di.di_flags = DI_FLAGS_RO | DI_FLAGS_FIX;
	ଅନ୍ୟଥା ଯଦି (p->vv_flags & VV_RO_SBX)
	    p->vv_di.di_flags = DI_FLAGS_RO_SBX | DI_FLAGS_FIX;
	ଅନ୍ୟଥା
	    p->vv_di.di_flags = DI_FLAGS_FIX;

	// add to v: scope dict, unless the value is not always available
	ଯଦି (p->vv_tv_type != VAR_UNKNOWN)
	    hash_add(&vimvarht, p->vv_di.di_ଚାବି, "initialization");
	ଯଦି (p->vv_flags & VV_COMPAT)
	    // add to compat scope dict
	    hash_add(&compat_hashtab, p->vv_di.di_ଚାବି, "initialization");
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
    set_vim_var_nr(VV_NUMBERSIZE, ଆକାର(varnumber_T) * 8);
    set_vim_var_nr(VV_SIZEOFINT, ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା));
    set_vim_var_nr(VV_SIZEOFLONG, ଆକାର(long));
    set_vim_var_nr(VV_SIZEOFPOINTER, ଆକାର(ଅକ୍ଷର *));
    set_vim_var_nr(VV_MAXCOL, MAXCOL);

    set_vim_var_nr(VV_TYPE_ସଂଖ୍ୟା,  VAR_TYPE_ସଂଖ୍ୟା);
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

#ଯଦି defined(EXITFREE)
/*
 * Free all vim variables information on exit
 */
    ଶୂନ୍ୟ
evalvars_clear(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    i;
    ସଂରଚନା vimvar   *p;

    ପାଇଁ (i = 0; i < VV_LEN; ++i)
    {
	p = &vimvars[i];
	ଯଦି (p->vv_di.di_tv.v_type == VAR_STRING)
	    VIM_CLEAR(p->vv_str);
	ଅନ୍ୟଥା ଯଦି (p->vv_di.di_tv.v_type == VAR_LIST)
	{
	    list_unref(p->vv_list);
	    p->vv_list = ଖାଲି;
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
    ପାଇଁ (i = 1; i <= script_ବସ୍ତୁଗୁଡ଼ିକ.ga_len; ++i)
	vars_clear(&SCRIPT_VARS(i));
}
#endif

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
garbage_collect_globvars(ପୂର୍ଣ୍ଣସଂଖ୍ୟା copyID)
{
    ଫେରନ୍ତୁ set_ref_in_ht(&globvarht, copyID, ଖାଲି, ଖାଲି);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
garbage_collect_vimvars(ପୂର୍ଣ୍ଣସଂଖ୍ୟା copyID)
{
    ଫେରନ୍ତୁ set_ref_in_ht(&vimvarht, copyID, ଖାଲି, ଖାଲି);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
garbage_collect_scriptvars(ପୂର୍ଣ୍ଣସଂଖ୍ୟା copyID)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    idx;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    abort = FALSE;
    scriptitem_T    *si;

    ପାଇଁ (i = 1; i <= script_ବସ୍ତୁଗୁଡ଼ିକ.ga_len; ++i)
    {
	abort = abort || set_ref_in_ht(&SCRIPT_VARS(i), copyID, ଖାଲି, ଖାଲି);

	si = SCRIPT_ITEM(i);
	ପାଇଁ (idx = 0; idx < si->sn_var_vals.ga_len; ++idx)
	{
	    svar_T    *sv = ((svar_T *)si->sn_var_vals.ga_data) + idx;

	    ଯଦି (sv->sv_name != ଖାଲି)
		abort = abort || set_ref_in_item(sv->sv_tv, copyID, ଖାଲି, ଖାଲି, ଖାଲି);
	}
    }

    ଫେରନ୍ତୁ abort;
}

/*
 * Set an internal variable to a string value. Creates the variable if it does
 * not already exist.
 */
    ଶୂନ୍ୟ
set_internal_string_var(char_u *name, char_u *ମୂଲ୍ୟ)
{
    char_u	*val;
    typval_T	*tvp;

    val = vim_strsave(ମୂଲ୍ୟ);
    ଯଦି (val == ଖାଲି)
	ଫେରନ୍ତୁ;

    tvp = alloc_string_tv(val);
    ଯଦି (tvp == ଖାଲି)
	ଫେରନ୍ତୁ;

    set_var(name, tvp, FALSE);
    free_tv(tvp);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
eval_charconvert(
    char_u	*enc_from,
    char_u	*enc_to,
    char_u	*fname_from,
    char_u	*fname_to)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		err = FALSE;
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;

    set_vim_var_string(VV_CC_FROM, enc_from, -1);
    set_vim_var_string(VV_CC_TO, enc_to, -1);
    set_vim_var_string(VV_FNAME_IN, fname_from, -1);
    set_vim_var_string(VV_FNAME_OUT, fname_to, -1);
    ctx = get_option_sctx("charconvert");
    ଯଦି (ctx != ଖାଲି)
	current_sctx = *ctx;

    ଯଦି (eval_to_bool(p_ccv, &err, ଖାଲି, FALSE, TRUE))
	err = TRUE;

    set_vim_var_string(VV_CC_FROM, ଖାଲି, -1);
    set_vim_var_string(VV_CC_TO, ଖାଲି, -1);
    set_vim_var_string(VV_FNAME_IN, ଖାଲି, -1);
    set_vim_var_string(VV_FNAME_OUT, ଖାଲି, -1);
    current_sctx = saved_sctx;

    ଯଦି (err)
	ଫେରନ୍ତୁ FAIL;
    ଫେରନ୍ତୁ OK;
}

#ଯଦି defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
eval_printexpr(char_u *fname, char_u *args)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		err = FALSE;
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;

    set_vim_var_string(VV_FNAME_IN, fname, -1);
    set_vim_var_string(VV_CMDARG, args, -1);
    ctx = get_option_sctx("printexpr");
    ଯଦି (ctx != ଖାଲି)
	current_sctx = *ctx;

    ଯଦି (eval_to_bool(p_pexpr, &err, ଖାଲି, FALSE, TRUE))
	err = TRUE;

    set_vim_var_string(VV_FNAME_IN, ଖାଲି, -1);
    set_vim_var_string(VV_CMDARG, ଖାଲି, -1);
    current_sctx = saved_sctx;

    ଯଦି (err)
    {
	mch_remove(fname);
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}
#endif

#ଯଦି defined(FEAT_DIFF)
    ଶୂନ୍ୟ
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
    ଯଦି (ctx != ଖାଲି)
	current_sctx = *ctx;

    // errors are ignored
    tv = eval_ପଦସମଷ୍ଟି_ext(p_dex, ଖାଲି, TRUE);
    free_tv(tv);

    set_vim_var_string(VV_FNAME_IN, ଖାଲି, -1);
    set_vim_var_string(VV_FNAME_NEW, ଖାଲି, -1);
    set_vim_var_string(VV_FNAME_OUT, ଖାଲି, -1);
    current_sctx = saved_sctx;
}

    ଶୂନ୍ୟ
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
    ଯଦି (ctx != ଖାଲି)
	current_sctx = *ctx;

    // errors are ignored
    tv = eval_ପଦସମଷ୍ଟି_ext(p_pex, ଖାଲି, TRUE);
    free_tv(tv);

    set_vim_var_string(VV_FNAME_IN, ଖାଲି, -1);
    set_vim_var_string(VV_FNAME_DIFF, ଖାଲି, -1);
    set_vim_var_string(VV_FNAME_OUT, ଖାଲି, -1);
    current_sctx = saved_sctx;
}
#endif

#ଯଦି defined(FEAT_SPELL)
/*
 * Evaluate an expression to a list with suggestions.
 * For the "expr:" part of 'spellsuggest'.
 * Returns NULL when there is an error.
 */
    list_T *
eval_spell_ପଦସମଷ୍ଟି(char_u *badword, char_u *ପଦସମଷ୍ଟି)
{
    typval_T	save_val;
    typval_T	rettv;
    list_T	*list = ଖାଲି;
    char_u	*p = skipwhite(ପଦସମଷ୍ଟି);
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		r;

    // Set "v:val" to the bad word.
    prepare_vimvar(VV_VAL, &save_val);
    set_vim_var_string(VV_VAL, badword, -1);
    ଯଦି (p_verbose == 0)
	++emsg_off;
    ctx = get_option_sctx("spellsuggest");
    ଯଦି (ctx != ଖାଲି)
	current_sctx = *ctx;

    r = may_call_simple_func(p, &rettv);
    ଯଦି (r == NOTDONE)
	r = eval1(&p, &rettv, &EVALARG_EVALUATE);
    ଯଦି (r == OK)
    {
	ଯଦି (rettv.v_type != VAR_LIST)
	    clear_tv(&rettv);
	ଅନ୍ୟଥା
	    list = rettv.vval.v_list;
    }

    ଯଦି (p_verbose == 0)
	--emsg_off;
    clear_tv(get_vim_var_tv(VV_VAL));
    restore_vimvar(VV_VAL, &save_val);
    current_sctx = saved_sctx;

    ଫେରନ୍ତୁ list;
}

/*
 * "list" is supposed to contain two items: a word and a number.  Return the
 * word in "pp" and the number as the return value.
 * Return -1 if anything isn't right.
 * Used to get the good word and score from the eval_spell_expr() result.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_spellword(list_T *list, char_u **pp)
{
    listitem_T	*li;

    li = list->lv_first;
    ଯଦି (li == ଖାଲି)
	ଫେରନ୍ତୁ -1;
    *pp = tv_get_string(&li->li_tv);

    li = li->li_next;
    ଯଦି (li == ଖାଲି)
	ଫେରନ୍ତୁ -1;
    ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା(&li->li_tv);
}
#endif

/*
 * Prepare v: variable "idx" to be used.
 * Save the current typeval in "save_tv" and clear it.
 * When not used yet add the variable to the v: hashtable.
 */
    ଶୂନ୍ୟ
prepare_vimvar(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, typval_T *save_tv)
{
    *save_tv = vimvars[idx].vv_tv;
    vimvars[idx].vv_str = ଖାଲି;  // don't free it now
    ଯଦି (vimvars[idx].vv_tv_type == VAR_UNKNOWN)
	hash_add(&vimvarht, vimvars[idx].vv_di.di_ଚାବି, "prepare vimvar");
}

/*
 * Restore v: variable "idx" to typeval "save_tv".
 * Note that the v: variable must have been cleared already.
 * When no longer defined, remove the variable from the v: hashtable.
 */
    ଶୂନ୍ୟ
restore_vimvar(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, typval_T *save_tv)
{
    hashitem_T	*hi;

    vimvars[idx].vv_tv = *save_tv;
    ଯଦି (vimvars[idx].vv_tv_type != VAR_UNKNOWN)
	ଫେରନ୍ତୁ;

    hi = hash_find(&vimvarht, vimvars[idx].vv_di.di_ଚାବି);
    ଯଦି (HASHITEM_EMPTY(hi))
	internal_error("restore_vimvar()");
    ଅନ୍ୟଥା
	hash_remove(&vimvarht, hi, "restore vimvar");
}

/*
 * List Vim variables.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
list_vim_vars(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *first)
{
    list_hashtable_vars(&vimvarht, "v:", FALSE, first);
}

/*
 * List script-local variables, if there is a script.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
list_script_vars(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *first)
{
    ଯଦି (SCRIPT_ID_VALID(current_sctx.sc_sid))
	list_hashtable_vars(&SCRIPT_VARS(current_sctx.sc_sid),
							   "s:", FALSE, first);
}

/*
 * Return TRUE if "name" starts with "g:", "w:", "t:" or "b:".
 * But only when an identifier character follows.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
is_scoped_variable(char_u *name)
{
    ଫେରନ୍ତୁ vim_strchr((char_u *)"gwbt", name[0]) != ଖାଲି
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
eval_one_ପଦସମଷ୍ଟି_in_str(char_u *p, garray_T *gap, ପୂର୍ଣ୍ଣସଂଖ୍ୟା evaluate)
{
    char_u	*block_start = skipwhite(p + 1);  // skip the opening {
    char_u	*block_end = block_start;
    char_u	*ପଦସମଷ୍ଟି_val;

    ଯଦି (*block_start == NUL)
    {
	semsg(_(e_missing_close_curly_str), p);
	ଫେରନ୍ତୁ ଖାଲି;
    }
    ଯଦି (skip_ପଦସମଷ୍ଟି(&block_end, ଖାଲି) == FAIL)
	ଫେରନ୍ତୁ ଖାଲି;
    block_end = skipwhite(block_end);
    ଯଦି (*block_end != '}')
    {
	semsg(_(e_missing_close_curly_str), p);
	ଫେରନ୍ତୁ ଖାଲି;
    }
    ଯଦି (evaluate)
    {
	*block_end = NUL;
	ପଦସମଷ୍ଟି_val = eval_to_string(block_start, FALSE, FALSE);
	*block_end = '}';
	ଯଦି (ପଦସମଷ୍ଟି_val == ଖାଲି)
	    ଫେରନ୍ତୁ ଖାଲି;
	ga_concat(gap, ପଦସମଷ୍ଟି_val);
	vim_free(ପଦସମଷ୍ଟି_val);
    }

    ଫେରନ୍ତୁ block_end + 1;
}

/*
 * Evaluate all the Vim expressions {expr} in "str" and return the resulting
 * string in allocated memory.  "{{" is reduced to "{" and "}}" to "}".
 * Used for a heredoc assignment.
 * Returns NULL for an error.
 */
    ସ୍ଥିର char_u *
eval_all_ପଦସମଷ୍ଟି_in_str(char_u *str)
{
    garray_T	ga;
    char_u	*p;

    ga_init2(&ga, 1, 80);
    p = str;

    ଯେତେବେଳେ (*p != NUL)
    {
	char_u	*lit_start;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	escaped_brace = FALSE;

	// Look for a block start.
	lit_start = p;
	ଯେତେବେଳେ (*p != '{' && *p != '}' && *p != NUL)
	    ++p;

	ଯଦି (*p != NUL && *p == p[1])
	{
	    // Escaped brace, unescape and continue.
	    // Include the brace in the literal string.
	    ++p;
	    escaped_brace = TRUE;
	}
	ଅନ୍ୟଥା ଯଦି (*p == '}')
	{
	    semsg(_(e_stray_closing_curly_str), str);
	    ga_clear(&ga);
	    ଫେରନ୍ତୁ ଖାଲି;
	}

	// Append the literal part.
	ga_concat_len(&ga, lit_start, (size_t)(p - lit_start));

	ଯଦି (*p == NUL)
	    ଭାଙ୍ଗ;

	ଯଦି (escaped_brace)
	{
	    // Skip the second brace.
	    ++p;
	    ଜାରି;
	}

	// Evaluate the expression and append the result.
	p = eval_one_ପଦସମଷ୍ଟି_in_str(p, &ga, TRUE);
	ଯଦି (p == ଖାଲି)
	{
	    ga_clear(&ga);
	    ଫେରନ୍ତୁ ଖାଲି;
	}
    }
    ga_append(&ga, NUL);

    ଫେରନ୍ତୁ ga.ga_data;
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
heredoc_get(exarg_T *eap, char_u *cmd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା script_get, ପୂର୍ଣ୍ଣସଂଖ୍ୟା vim9compile)
{
    char_u	*theline = ଖାଲି;
    char_u	*marker;
    list_T	*l;
    char_u	*p;
    char_u	*str;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		marker_indent_len = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		text_indent_len = 0;
    char_u	*text_indent = ଖାଲି;
    char_u	dot[] = ".";
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		comment_char = in_vim9script() ? '#' : '"';
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		evalstr = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		eval_failed = FALSE;
    cctx_T	*cctx = vim9compile ? eap->cookie : ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଗଣନା_2 = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		heredoc_in_string = FALSE;
    char_u	*ଧାଡ଼ି_arg = ଖାଲି;
    char_u	*nl_ptr = vim_strchr(cmd, '\n');

    ଯଦି (nl_ptr != ଖାଲି)
    {
	heredoc_in_string = TRUE;
	ଧାଡ଼ି_arg = nl_ptr + 1;
	*nl_ptr = NUL;
    }
    ଅନ୍ୟଥା ଯଦି (eap->ea_getline == ଖାଲି)
    {
	emsg(_(e_cannot_use_heredoc_here));
	ଫେରନ୍ତୁ ଖାଲି;
    }

    // Check for the optional 'trim' word before the marker
    cmd = skipwhite(cmd);

    ଯେତେବେଳେ (TRUE)
    {
	ଯଦି (STRNCMP(cmd, "trim", 4) == 0
		&& (cmd[4] == NUL || VIM_ISWHITE(cmd[4])))
	{
	    cmd = skipwhite(cmd + 4);

	    // Trim the indentation from all the lines in the here document.
	    // The amount of indentation trimmed is the same as the indentation
	    // of the first line after the :let command line.  To find the end
	    // marker the indent of the :let command line is trimmed.
	    p = *eap->cmdlinep;
	    ଯେତେବେଳେ (VIM_ISWHITE(*p))
	    {
		p++;
		marker_indent_len++;
	    }
	    text_indent_len = -1;

	    ଜାରି;
	}
	ଯଦି (STRNCMP(cmd, "eval", 4) == 0
		&& (cmd[4] == NUL || VIM_ISWHITE(cmd[4])))
	{
	    cmd = skipwhite(cmd + 4);
	    evalstr = TRUE;
	    ଜାରି;
	}
	ଭାଙ୍ଗ;
    }

    // The marker is the next word.
    ଯଦି (*cmd != NUL && *cmd != comment_char)
    {
	marker = skipwhite(cmd);
	p = skiptowhite(marker);
	ଯଦି (*skipwhite(p) != NUL && *skipwhite(p) != comment_char)
	{
	    semsg(_(e_trailing_characters_str), p);
	    ଫେରନ୍ତୁ ଖାଲି;
	}
	*p = NUL;
	ଯଦି (!script_get && vim_islower(*marker))
	{
	    emsg(_(e_marker_cannot_start_with_lower_case_letter));
	    ଫେରନ୍ତୁ ଖାଲି;
	}
    }
    ଅନ୍ୟଥା
    {
	// When getting lines for an embedded script, if the marker is missing,
	// accept '.' as the marker.
	ଯଦି (script_get)
	    marker = dot;
	ଅନ୍ୟଥା
	{
	    emsg(_(e_missing_marker));
	    ଫେରନ୍ତୁ ଖାଲି;
	}
    }

    l = list_alloc();
    ଯଦି (l == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    ପାଇଁ (;;)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	mi = 0;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	ti = 0;

	ଯଦି (heredoc_in_string)
	{
	    char_u	*next_ଧାଡ଼ି;

	    // heredoc in a string separated by newlines.  Get the next line
	    // from the string.

	    ଯଦି (*ଧାଡ଼ି_arg == NUL)
	    {
		semsg(_(e_missing_end_marker_str), marker);
		ଭାଙ୍ଗ;
	    }

	    theline = ଧାଡ଼ି_arg;
	    next_ଧାଡ଼ି = vim_strchr(theline, '\n');
	    ଯଦି (next_ଧାଡ଼ି == ଖାଲି)
		ଧାଡ଼ି_arg += STRLEN(ଧାଡ଼ି_arg);
	    ଅନ୍ୟଥା
	    {
		*next_ଧାଡ଼ି = NUL;
		ଧାଡ଼ି_arg = next_ଧାଡ଼ି + 1;
	    }
	}
	ଅନ୍ୟଥା
	{
	    vim_free(theline);
	    theline = eap->ea_getline(NUL, eap->cookie, 0, FALSE);
	    ଯଦି (theline == ଖାଲି)
	    {
		semsg(_(e_missing_end_marker_str), marker);
		ଭାଙ୍ଗ;
	    }
	}

	// with "trim": skip the indent matching the :let line to find the
	// marker
	ଯଦି (marker_indent_len > 0
		&& STRNCMP(theline, *eap->cmdlinep, marker_indent_len) == 0)
	    mi = marker_indent_len;
	ଯଦି (STRCMP(marker, theline + mi) == 0)
	    ଭାଙ୍ଗ;

	// If expression evaluation failed in the heredoc, then skip till the
	// end marker.
	ଯଦି (eval_failed)
	    ଜାରି;

	ଯଦି (text_indent_len == -1 && *theline != NUL)
	{
	    // set the text indent from the first line.
	    p = theline;
	    text_indent_len = 0;
	    ଯେତେବେଳେ (VIM_ISWHITE(*p))
	    {
		p++;
		text_indent_len++;
	    }
	    text_indent = vim_strnsave(theline, text_indent_len);
	}
	// with "trim": skip the indent matching the first line
	ଯଦି (text_indent != ଖାଲି)
	    ପାଇଁ (ti = 0; ti < text_indent_len; ++ti)
		ଯଦି (theline[ti] != text_indent[ti])
		    ଭାଙ୍ଗ;

	str = theline + ti;
	ଯଦି (vim9compile)
	{
	    ଯଦି (compile_all_ପଦସମଷ୍ଟି_in_str(str, evalstr, cctx) == FAIL)
	    {
		vim_free(theline);
		vim_free(text_indent);
		list_free(l);
		ଫେରନ୍ତୁ FAIL;
	    }
	    ଗଣନା_2++;
	}
	ଅନ୍ୟଥା
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    free_str = FALSE;

	    ଯଦି (evalstr && !eap->skip)
	    {
		str = eval_all_ପଦସମଷ୍ଟି_in_str(str);
		ଯଦି (str == ଖାଲି)
		{
		    // expression evaluation failed
		    eval_failed = TRUE;
		    ଜାରି;
		}
		free_str = TRUE;
	    }

	    ଯଦି (list_append_string(l, str, -1) == FAIL)
	    {
		ଯଦି (free_str)
		    vim_free(str);
		ଭାଙ୍ଗ;
	    }
	    ଯଦି (free_str)
		vim_free(str);
	}
    }
    ଯଦି (heredoc_in_string)
	// Next command follows the heredoc in the string.
	eap->nextcmd = ଧାଡ଼ି_arg;
    ଅନ୍ୟଥା
	vim_free(theline);
    vim_free(text_indent);

    ଯଦି (vim9compile && cctx->ctx_skip != SKIP_YES && !eval_failed)
	generate_NEWLIST(cctx, ଗଣନା_2, FALSE);

    ଯଦି (eval_failed)
    {
	// expression evaluation in the heredoc failed
	list_free(l);
	ଫେରନ୍ତୁ ଖାଲି;
    }
    ଫେରନ୍ତୁ l;
}

/*
 * Vim9 variable declaration:
 * ":var name"
 * ":var name: type"
 * ":var name = expr"
 * ":var name: type = expr"
 * etc.
 */
    ଶୂନ୍ୟ
ex_var(exarg_T *eap)
{
    char_u *p = eap->cmd;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    has_var;

    ଯଦି (!in_vim9script())
    {
	semsg(_(e_str_cannot_be_used_in_legacy_vim_script), ":var");
	ଫେରନ୍ତୁ;
    }
    has_var = checkforcmd_noparen(&p, "var", 3);
    ଯଦି (current_sctx.sc_sid == 0 && has_var)
    {
	emsg(_(e_cannot_declare_variable_on_command_ଧାଡ଼ି));
	ଫେରନ୍ତୁ;
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
    ଶୂନ୍ୟ
ex_let(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    char_u	*ପଦସମଷ୍ଟି = ଖାଲି;
    typval_T	rettv;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		var_ଗଣନା = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		semicolon = 0;
    char_u	op[4];
    char_u	*argend;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		first = TRUE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		concat;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		has_assign;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		vim9script = in_vim9script();

    ଯଦି (eap->cmdidx == CMD_final && !vim9script)
    {
	// In legacy Vim script ":final" is short for ":finally".
	ex_finally(eap);
	ଫେରନ୍ତୁ;
    }
    ଯଦି (eap->cmdidx == CMD_let && vim9script)
    {
	emsg(_(e_cannot_use_let_in_vim9_script));
	ଫେରନ୍ତୁ;
    }

    ଯଦି (eap->cmdidx == CMD_const)
	flags |= ASSIGN_CONST;
    ଅନ୍ୟଥା ଯଦି (eap->cmdidx == CMD_final)
	flags |= ASSIGN_FINAL;

    // Vim9 assignment without ":let", ":const" or ":final"
    ଯଦି (eap->arg == eap->cmd)
	flags |= ASSIGN_NO_DECL;

    argend = skip_var_list(arg, TRUE, &var_ଗଣନା, &semicolon, FALSE);
    ଯଦି (argend == ଖାଲି)
	ଫେରନ୍ତୁ;
    ପଦସମଷ୍ଟି = skipwhite(argend);
    concat = ପଦସମଷ୍ଟି[0] == '.'
	&& ((ପଦସମଷ୍ଟି[1] == '=' && in_old_script(2))
		|| (ପଦସମଷ୍ଟି[1] == '.' && ପଦସମଷ୍ଟି[2] == '='));
    has_assign =  *ପଦସମଷ୍ଟି == '=' || (vim_strchr((char_u *)"+-*/%", *ପଦସମଷ୍ଟି) != ଖାଲି
							    && ପଦସମଷ୍ଟି[1] == '=');
    ଯଦି (!has_assign && !concat)
    {
	// ":let" without "=": list variables
	ଯଦି (*arg == '[')
	    emsg(_(e_invalid_argument));
	ଅନ୍ୟଥା ଯଦି (ପଦସମଷ୍ଟି[0] == '.' && ପଦସମଷ୍ଟି[1] == '=')
	    emsg(_(e_dot_equal_not_supported_with_script_version_two));
	ଅନ୍ୟଥା ଯଦି (!ends_excmd2(eap->cmd, arg))
	{
	    ଯଦି (vim9script)
	    {
		ଯଦି (!ends_excmd2(eap->cmd, skipwhite(argend)))
		    semsg(_(e_trailing_characters_str), argend);
		ଅନ୍ୟଥା
		    // Vim9 declaration ":var name: type"
		    arg = vim9_declare_scriptvar(eap, arg);
	    }
	    ଅନ୍ୟଥା
	    {
		// ":let var1 var2" - list values
		arg = list_arg_vars(eap, arg, &first);
	    }
	}
	ଅନ୍ୟଥା ଯଦି (!eap->skip)
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
	ଫେରନ୍ତୁ;
    }

    ଯଦି (ପଦସମଷ୍ଟି[0] == '=' && ପଦସମଷ୍ଟି[1] == '<' && ପଦସମଷ୍ଟି[2] == '<')
    {
	list_T	*l = ଖାଲି;
	long	cur_lnum = SOURCING_LNUM;

	// :let text =<< [trim] [eval] END
	// :var text =<< [trim] [eval] END
	ଯଦି (vim9script && !eap->skip && (!VIM_ISWHITE(ପଦସମଷ୍ଟି[-1])
						 || !IS_WHITE_OR_NUL(ପଦସମଷ୍ଟି[3])))
	    semsg(_(e_white_space_required_before_and_after_str_at_str),
								  "=<<", ପଦସମଷ୍ଟି);
	ଅନ୍ୟଥା
	    l = heredoc_get(eap, ପଦସମଷ୍ଟି + 3, FALSE, FALSE);

	ଯଦି (l != ଖାଲି)
	{
	    rettv_list_set(&rettv, l);
	    ଯଦି (!eap->skip)
	    {
		// errors are for the assignment, not the end marker
		SOURCING_LNUM = cur_lnum;
		op[0] = '=';
		op[1] = NUL;
		(ଶୂନ୍ୟ)ex_let_vars(eap->arg, &rettv, FALSE, semicolon, var_ଗଣନା,
								flags, op);
	    }
	    clear_tv(&rettv);
	}
	ଫେରନ୍ତୁ;
    }

    evalarg_T   evalarg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    len = 1;

    CLEAR_FIELD(rettv);

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା cur_lnum;

    op[0] = '=';
    op[1] = NUL;
    ଯଦି (*ପଦସମଷ୍ଟି != '=')
    {
	ଯଦି (vim9script && (flags & ASSIGN_NO_DECL) == 0)
	{
	    // +=, /=, etc. require an existing variable
	    semsg(_(e_cannot_use_operator_on_new_variable_str), eap->arg);
	}
	ଅନ୍ୟଥା ଯଦି (vim_strchr((char_u *)"+-*/%.", *ପଦସମଷ୍ଟି) != ଖାଲି)
	{
	    op[0] = *ପଦସମଷ୍ଟି;   // +=, -=, *=, /=, %= or .=
	    ++len;
	    ଯଦି (ପଦସମଷ୍ଟି[0] == '.' && ପଦସମଷ୍ଟି[1] == '.') // ..=
	    {
		++ପଦସମଷ୍ଟି;
		++len;
	    }
	}
	ପଦସମଷ୍ଟି += 2;
    }
    ଅନ୍ୟଥା
	++ପଦସମଷ୍ଟି;

    ଯଦି (vim9script && !eap->skip && (!VIM_ISWHITE(*argend)
					       || !IS_WHITE_OR_NUL(*ପଦସମଷ୍ଟି)))
    {
	vim_strncpy(op, ପଦସମଷ୍ଟି - len, len);
	semsg(_(e_white_space_required_before_and_after_str_at_str),
							       op, argend);
    }

    ଯଦି (eap->skip)
	++emsg_skip;
    fill_evalarg_from_eap(&evalarg, eap, eap->skip);
    ପଦସମଷ୍ଟି = skipwhite_and_linebreak(ପଦସମଷ୍ଟି, &evalarg);
    cur_lnum = SOURCING_LNUM;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା eval_res = eval0(ପଦସମଷ୍ଟି, &rettv, eap, &evalarg);
    ଯଦି (eap->skip)
	--emsg_skip;
    clear_evalarg(&evalarg, eap);

    // Restore the line number so that any type error is given for the
    // declaration, not the expression.
    SOURCING_LNUM = cur_lnum;

    ଯଦି (!eap->skip && eval_res != FAIL)
	(ଶୂନ୍ୟ)ex_let_vars(eap->arg, &rettv, FALSE, semicolon, var_ଗଣନା,
								    flags, op);
    ଯଦି (eval_res != FAIL)
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ex_let_vars(
    char_u	*arg_start,
    typval_T	*tv,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		copy,		// copy values from "tv", don't move
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		semicolon,	// from skip_var_list()
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		var_ଗଣନା,	// from skip_var_list()
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags,		// ASSIGN_FINAL, ASSIGN_CONST, etc.
    char_u	*op)
{
    char_u	*arg = arg_start;
    list_T	*l;
    tuple_T	*tuple = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		var_idx = 0;
    listitem_T	*item = ଖାଲି;
    typval_T	ltv;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_list = tv->v_type == VAR_LIST;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx;

    ଯଦି (tv->v_type == VAR_VOID)
    {
	emsg(_(e_cannot_use_void_ମୂଲ୍ୟ));
	ଫେରନ୍ତୁ FAIL;
    }
    ଯଦି (*arg != '[')
    {
	// ":let var = expr" or ":for var in list"
	ଯଦି (ex_let_one(arg, tv, copy, flags, op, op, var_idx) == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;
	ଫେରନ୍ତୁ OK;
    }

    // ":let [v1, v2] = list" or ":for [v1, v2] in listlist"
    // or
    // ":let [v1, v2] = tuple" or ":for [v1, v2] in tupletuple"
    ଯଦି (tv->v_type != VAR_LIST && tv->v_type != VAR_TUPLE)
    {
	emsg(_(e_list_or_tuple_required));
	ଫେରନ୍ତୁ FAIL;
    }
    ଯଦି (is_list)
    {
	l = tv->vval.v_list;
	ଯଦି (l == ଖାଲି)
	{
	    emsg(_(e_list_required));
	    ଫେରନ୍ତୁ FAIL;
	}
	i = list_len(l);
    }
    ଅନ୍ୟଥା
    {
	tuple = tv->vval.v_tuple;
	ଯଦି (tuple == ଖାଲି)
	{
	    emsg(_(e_tuple_required));
	    ଫେରନ୍ତୁ FAIL;
	}
	i = tuple_len(tuple);
    }

    ଯଦି (semicolon == 0 && var_ଗଣନା < i)
    {
	emsg(_(is_list ? e_less_targets_than_list_ବସ୍ତୁଗୁଡ଼ିକ
					: e_less_targets_than_tuple_ବସ୍ତୁଗୁଡ଼ିକ));
	ଫେରନ୍ତୁ FAIL;
    }
    ଯଦି (var_ଗଣନା - semicolon > i)
    {
	emsg(_(is_list ? e_more_targets_than_list_ବସ୍ତୁଗୁଡ଼ିକ
					: e_more_targets_than_tuple_ବସ୍ତୁଗୁଡ଼ିକ));
	ଫେରନ୍ତୁ FAIL;
    }

    ଯଦି (is_list)
    {
	CHECK_LIST_MATERIALIZE(l);
	item = l->lv_first;
    }
    ଅନ୍ୟଥା
	idx = 0;

    ଯେତେବେଳେ (*arg != ']')
    {
	arg = skipwhite(arg + 1);
	++var_idx;
	arg = ex_let_one(arg, is_list ? &item->li_tv : TUPLE_ITEM(tuple, idx),
			 TRUE, flags | ASSIGN_UNPACK, (char_u *)",;]", op,
			 var_idx);
	ଯଦି (is_list)
	    item = item->li_next;
	ଅନ୍ୟଥା
	    idx++;
	ଯଦି (arg == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;

	arg = skipwhite(arg);
	ଯଦି (*arg == ';')
	{
	    // Put the rest of the list or tuple (may be empty) in the var
	    // after ';'.  Create a new list or tuple for this.
	    ଯଦି (is_list)
	    {
		// Put the rest of the list (may be empty) in the var
		// after ';'.  Create a new list for this.
		l = list_alloc();
		ଯଦି (l == ଖାଲି)
		    ଫେରନ୍ତୁ FAIL;

		// list
		ଯେତେବେଳେ (item != ଖାଲି)
		{
		    list_append_tv(l, &item->li_tv);
		    item = item->li_next;
		}

		ltv.v_type = VAR_LIST;
		ltv.v_lock = 0;
		ltv.vval.v_list = l;
		l->lv_refcount = 1;
	    }
	    ଅନ୍ୟଥା
	    {
		tuple_T	*new_tuple = tuple_alloc();
		ଯଦି (new_tuple == ଖାଲି)
		    ଫେରନ୍ତୁ FAIL;

		// Put the rest of the tuple (may be empty) in the var
		// after ';'.  Create a new tuple for this.
		ଯେତେବେଳେ (idx < TUPLE_LEN(tuple))
		{
		    typval_T    new_tv;

		    copy_tv(TUPLE_ITEM(tuple, idx), &new_tv);
		    ଯଦି (tuple_append_tv(new_tuple, &new_tv) == FAIL)
		    {
			clear_tv(&new_tv);
			tuple_unref(new_tuple);
			ଫେରନ୍ତୁ FAIL;
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
	    ଯଦି (arg == ଖାଲି)
		ଫେରନ୍ତୁ FAIL;
	    ଭାଙ୍ଗ;
	}
	ଅନ୍ୟଥା ଯଦି (*arg != ',' && *arg != ']')
	{
	    internal_error("ex_let_vars()");
	    ଫେରନ୍ତୁ FAIL;
	}
    }

    ଫେରନ୍ତୁ OK;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		include_type,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*var_ଗଣନା,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*semicolon,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		silent)
{
    char_u	*p, *s;

    ଯଦି (*arg == '[')
    {
	// "[var, var]": find the matching ']'.
	p = arg;
	ପାଇଁ (;;)
	{
	    p = skipwhite(p + 1);	// skip whites after '[', ';' or ','
	    s = skip_var_one(p, include_type);
	    ଯଦି (s == p)
	    {
		ଯଦି (!silent)
		    semsg(_(e_invalid_argument_str), p);
		ଫେରନ୍ତୁ ଖାଲି;
	    }
	    ++*var_ଗଣନା;

	    p = skipwhite(s);
	    ଯଦି (*p == ']')
		ଭାଙ୍ଗ;
	    ଅନ୍ୟଥା ଯଦି (*p == ';')
	    {
		ଯଦି (*semicolon == 1)
		{
		    ଯଦି (!silent)
			emsg(_(e_double_semicolon_in_list_of_variables));
		    ଫେରନ୍ତୁ ଖାଲି;
		}
		*semicolon = 1;
	    }
	    ଅନ୍ୟଥା ଯଦି (*p != ',')
	    {
		ଯଦି (!silent)
		    semsg(_(e_invalid_argument_str), p);
		ଫେରନ୍ତୁ ଖାଲି;
	    }
	}
	ଫେରନ୍ତୁ p + 1;
    }

    ଫେରନ୍ତୁ skip_var_one(arg, include_type);
}

/*
 * Skip one (assignable) variable name, including @r, $VAR, &option, d.key,
 * l[idx].
 * In Vim9 script also skip over ": type" if "include_type" is TRUE.
 */
    char_u *
skip_var_one(char_u *arg, ପୂର୍ଣ୍ଣସଂଖ୍ୟା include_type)
{
    char_u	*end;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		vim9 = in_vim9script();

    ଯଦି (*arg == '@' && arg[1] != NUL)
	ଫେରନ୍ତୁ arg + 2;

    // termcap option name may have non-alpha characters
    ଯଦି (STRNCMP(arg, "&t_", 3) == 0 && arg[3] != NUL && arg[4] != NUL)
	ଫେରନ୍ତୁ arg + 5;

    end = find_name_end(*arg == '$' || *arg == '&' ? arg + 1 : arg,
				   ଖାଲି, ଖାଲି, FNE_INCL_BR | FNE_CHECK_START);

    // "a: type" is declaring variable "a" with a type, not "a:".
    // Same for "s: type".
    ଯଦି (vim9 && end == arg + 2 && end[-1] == ':')
	--end;

    ଯଦି (include_type && vim9)
    {
	ଯଦି (*skipwhite(end) == ':')
	    end = skip_type(skipwhite(skipwhite(end) + 1), FALSE);
    }
    ଫେରନ୍ତୁ end;
}

/*
 * List variables for hashtab "ht" with prefix "prefix".
 * If "empty" is TRUE also list NULL strings as empty strings.
 */
    ଶୂନ୍ୟ
list_hashtable_vars(
    hashtab_T	*ht,
    ଅକ୍ଷର	*prefix,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		empty,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*first)
{
    hashitem_T	*hi;
    dictitem_T	*di;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		todo;
    char_u	buf[IOSIZE];

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_ht_flags = ht->ht_flags;
    ht->ht_flags |= HTFLAGS_FROZEN;

    todo = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ht->ht_used;
    ପାଇଁ (hi = ht->ht_array; todo > 0 && !got_int; ++hi)
    {
	ଯଦି (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    di = HI2DI(hi);

	    // apply :filter /pat/ to variable name
	    vim_strncpy((char_u *)buf, (char_u *)prefix, IOSIZE - 1);
	    vim_strcat((char_u *)buf, di->di_ଚାବି, IOSIZE);
	    ଯଦି (message_filtered(buf))
		ଜାରି;

	    ଯଦି (empty || di->di_tv.v_type != VAR_STRING
					   || di->di_tv.vval.v_string != ଖାଲି)
		list_one_var(di, prefix, first);
	}
    }

    ht->ht_flags = save_ht_flags;
}

/*
 * List global variables.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
list_glob_vars(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *first)
{
    list_hashtable_vars(&globvarht, "", TRUE, first);
}

/*
 * List buffer variables.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
list_buf_vars(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *first)
{
    list_hashtable_vars(&curbuf->b_vars->dv_hashtab, "b:", TRUE, first);
}

/*
 * List window variables.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
list_win_vars(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *first)
{
    list_hashtable_vars(&curwin->w_vars->dv_hashtab, "w:", TRUE, first);
}

/*
 * List tab page variables.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
list_tab_vars(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *first)
{
    list_hashtable_vars(&curtab->tp_vars->dv_hashtab, "t:", TRUE, first);
}

/*
 * List variables in "arg".
 */
    ସ୍ଥିର char_u *
list_arg_vars(exarg_T *eap, char_u *arg, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *first)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		error = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    char_u	*name;
    char_u	*name_start;
    char_u	*arg_subsc;
    char_u	*tofree;
    typval_T    tv;

    ଯେତେବେଳେ (!ends_excmd2(eap->cmd, arg) && !got_int)
    {
	ଯଦି (error || eap->skip)
	{
	    arg = find_name_end(arg, ଖାଲି, ଖାଲି, FNE_INCL_BR | FNE_CHECK_START);
	    ଯଦି (!VIM_ISWHITE(*arg) && !ends_excmd(*arg))
	    {
		emsg_severe = TRUE;
		ଯଦି (!did_emsg)
		    semsg(_(e_trailing_characters_str), arg);
		ଭାଙ୍ଗ;
	    }
	}
	ଅନ୍ୟଥା
	{
	    // get_name_len() takes care of expanding curly braces
	    name_start = name = arg;
	    len = get_name_len(&arg, &tofree, TRUE, TRUE);
	    ଯଦି (len <= 0)
	    {
		// This is mainly to keep test 49 working: when expanding
		// curly braces fails overrule the exception error message.
		ଯଦି (len < 0 && !aborting())
		{
		    emsg_severe = TRUE;
		    semsg(_(e_invalid_argument_str), arg);
		    ଭାଙ୍ଗ;
		}
		error = TRUE;
	    }
	    ଅନ୍ୟଥା
	    {
		arg = skipwhite(arg);
		ଯଦି (tofree != ଖାଲି)
		    name = tofree;
		ଯଦି (eval_variable(name, len, 0, &tv, ଖାଲି,
						     EVAL_VAR_VERBOSE) == FAIL)
		    error = TRUE;
		ଅନ୍ୟଥା
		{
		    // handle d.key, l[idx], f(expr)
		    arg_subsc = arg;
		    ଯଦି (handle_subscript(&arg, name_start, &tv,
					      &EVALARG_EVALUATE, TRUE) == FAIL)
			error = TRUE;
		    ଅନ୍ୟଥା
		    {
			ଯଦି (arg == arg_subsc && len == 2 && name[1] == ':')
			{
			    ବାଛ (*name)
			    {
				ସ୍ଥିତି 'g': list_glob_vars(first); ଭାଙ୍ଗ;
				ସ୍ଥିତି 'b': list_buf_vars(first); ଭାଙ୍ଗ;
				ସ୍ଥିତି 'w': list_win_vars(first); ଭାଙ୍ଗ;
				ସ୍ଥିତି 't': list_tab_vars(first); ଭାଙ୍ଗ;
				ସ୍ଥିତି 'v': list_vim_vars(first); ଭାଙ୍ଗ;
				ସ୍ଥିତି 's': list_script_vars(first); ଭାଙ୍ଗ;
				ସ୍ଥିତି 'l': list_func_vars(first); ଭାଙ୍ଗ;
				ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
					  semsg(_(e_cant_list_variables_for_str), name);
			    }
			}
			ଅନ୍ୟଥା
			{
			    char_u	numbuf[NUMBUFLEN];
			    char_u	*tf;
			    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
			    char_u	*s;

			    s = echo_string(&tv, &tf, numbuf, 0);
			    c = *arg;
			    *arg = NUL;
			    list_one_var_a("",
				    arg == arg_subsc ? name : name_start,
				    tv.v_type,
				    s == ଖାଲି ? (char_u *)"" : s,
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

    ଫେରନ୍ତୁ arg;
}

/*
 * Set an environment variable, part of ex_let_one().
 */
    ସ୍ଥିର char_u *
ex_let_env(
    char_u	*arg,
    typval_T	*tv,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags,
    char_u	*endchars,
    char_u	*op)
{
    char_u	*arg_end = ଖାଲି;
    char_u	*name;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;

    ଯଦି ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					 && (flags & ASSIGN_FOR_LOOP) == 0)
    {
	emsg(_(e_cannot_lock_environment_variable));
	ଫେରନ୍ତୁ ଖାଲି;
    }

    // Find the end of the name.
    ++arg;
    name = arg;
    len = get_env_len(&arg);
    ଯଦି (len == 0)
	semsg(_(e_invalid_argument_str), name - 1);
    ଅନ୍ୟଥା
    {
	ଯଦି (op != ଖାଲି && vim_strchr((char_u *)"+-*/%", *op) != ଖାଲି)
	    semsg(_(e_wrong_variable_type_for_str_equal), op);
	ଅନ୍ୟଥା ଯଦି (endchars != ଖାଲି
			      && vim_strchr(endchars, *skipwhite(arg)) == ଖାଲି)
	    emsg(_(e_unexpected_characters_in_let));
	ଅନ୍ୟଥା ଯଦି (!check_secure() && !check_restricted())
	{
	    char_u	*tofree = ଖାଲି;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c1 = name[len];
	    char_u	*p;

	    name[len] = NUL;
	    p = tv_get_string_chk(tv);
	    ଯଦି (p != ଖାଲି && op != ଖାଲି && *op == '.')
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	mustfree = FALSE;
		char_u  *s = vim_getenv(name, &mustfree);

		ଯଦି (s != ଖାଲି)
		{
		    p = tofree = concat_str(s, p);
		    ଯଦି (mustfree)
			vim_free(s);
		}
	    }
	    ଯଦି (p != ଖାଲି)
	    {
		vim_setenv_ext(name, p);
		arg_end = arg;
	    }
	    name[len] = c1;
	    vim_free(tofree);
	}
    }
    ଫେରନ୍ତୁ arg_end;
}

/*
 * Set an option, part of ex_let_one().
 */
    ସ୍ଥିର char_u *
ex_let_option(
    char_u	*arg,
    typval_T	*tv,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags,
    char_u	*endchars,
    char_u	*op)
{
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		scope;
    char_u	*arg_end = ଖାଲି;

    ଯଦି ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					 && (flags & ASSIGN_FOR_LOOP) == 0)
    {
	emsg(_(e_cannot_lock_option));
	ଫେରନ୍ତୁ ଖାଲି;
    }

    // Find the end of the name.
    p = find_option_end(&arg, &scope);
    ଯଦି (p == ଖାଲି || (endchars != ଖାଲି
			       && vim_strchr(endchars, *skipwhite(p)) == ଖାଲି))
    {
	emsg(_(e_unexpected_characters_in_let));
	ଫେରନ୍ତୁ ଖାଲି;
    }

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c1;
    long	n = 0;
    getoption_T	opt_type;
    long	numval;
    char_u	*stringval = ଖାଲି;
    char_u	*s = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		failed = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_p_flags;
    char_u	*tofree = ଖାଲି;
    char_u	numbuf[NUMBUFLEN];

    c1 = *p;
    *p = NUL;

    opt_type = get_option_ମୂଲ୍ୟ(arg, &numval, &stringval, &opt_p_flags, scope);
    ଯଦି (opt_type == gov_unknown && arg[0] != 't' && arg[1] != '_')
    {
	semsg(_(e_unknown_option_str_2), arg);
	ଯାଅ theend;
    }
    ଯଦି (op != ଖାଲି && *op != '='
	&& (((opt_type == gov_bool || opt_type == gov_ସଂଖ୍ୟା) && *op == '.')
	    || (opt_type == gov_string && *op != '.')))
    {
	semsg(_(e_wrong_variable_type_for_str_equal), op);
	ଯାଅ theend;
    }

    ଯଦି ((opt_type == gov_bool
		|| opt_type == gov_ସଂଖ୍ୟା
		|| opt_type == gov_hidden_bool
		|| opt_type == gov_hidden_ସଂଖ୍ୟା)
		     && (tv->v_type != VAR_STRING || !in_vim9script()))
    {
	ଯଦି (opt_type == gov_bool || opt_type == gov_hidden_bool)
	    // bool, possibly hidden
	    n = (long)tv_get_bool_chk(tv, &failed);
	ଅନ୍ୟଥା
	    // number, possibly hidden
	    n = (long)tv_get_ସଂଖ୍ୟା_chk(tv, &failed);
	ଯଦି (failed)
	    ଯାଅ theend;
    }

    ଯଦି ((opt_p_flags & P_FUNC) && (tv->v_type == VAR_PARTIAL
						    || tv->v_type == VAR_FUNC))
    {
	// If the option can be set to a function reference or a lambda
	// and the passed value is a function reference, then convert it to
	// the name (string) of the function reference.
	s = tv2string(tv, &tofree, numbuf, 0);
	ଯଦି (s == ଖାଲି)
	    ଯାଅ theend;
    }
    // Avoid setting a string option to the text "v:false" or similar.
    // In Vim9 script also don't convert a number to string.
    ଅନ୍ୟଥା ଯଦି (tv->v_type != VAR_BOOL && tv->v_type != VAR_SPECIAL
		     && (!in_vim9script() || tv->v_type != VAR_ସଂଖ୍ୟା))
    {
	s = tv_get_string_chk(tv);
	ଯଦି (s == ଖାଲି)
	    ଯାଅ theend;
    }
    ଅନ୍ୟଥା ଯଦି (opt_type == gov_string || opt_type == gov_hidden_string)
    {
	emsg(_(e_string_required));
	ଯାଅ theend;
    }

    ଯଦି (op != ଖାଲି && *op != '=')
    {
	// number, in legacy script also bool
	ଯଦି (opt_type == gov_ସଂଖ୍ୟା
				 || (opt_type == gov_bool && !in_vim9script()))
	{
	    ବାଛ (*op)
	    {
		ସ୍ଥିତି '+': n = numval + n; ଭାଙ୍ଗ;
		ସ୍ଥିତି '-': n = numval - n; ଭାଙ୍ଗ;
		ସ୍ଥିତି '*': n = numval * n; ଭାଙ୍ଗ;
		ସ୍ଥିତି '/': n = (long)ଅଙ୍କ_divide(numval, n, &failed); ଭାଙ୍ଗ;
		ସ୍ଥିତି '%': n = (long)ଅଙ୍କ_modulus(numval, n, &failed); ଭାଙ୍ଗ;
	    }
	    s = ଖାଲି;
	    ଯଦି (failed)
		ଯାଅ theend;
	}
	ଅନ୍ୟଥା ଯଦି (opt_type == gov_string && stringval != ଖାଲି && s != ଖାଲି)
	{
	    // string
	    s = concat_str(stringval, s);
	    vim_free(stringval);
	    stringval = s;
	}
    }

    ଅକ୍ଷର *err = set_option_ମୂଲ୍ୟ(arg, n, s, scope);
    arg_end = p;
    ଯଦି (err != ଖାଲି)
	emsg(_(err));

theend:
    *p = c1;
    vim_free(stringval);
    vim_free(tofree);
    ଫେରନ୍ତୁ arg_end;
}

/*
 * Set a register, part of ex_let_one().
 */
    ସ୍ଥିର char_u *
ex_let_register(
    char_u	*arg,
    typval_T	*tv,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags,
    char_u	*endchars,
    char_u	*op)
{
    char_u	*arg_end = ଖାଲି;

    ଯଦି ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					 && (flags & ASSIGN_FOR_LOOP) == 0)
    {
	emsg(_(e_cannot_lock_register));
	ଫେରନ୍ତୁ ଖାଲି;
    }
    ++arg;
    ଯଦି (op != ଖାଲି && vim_strchr((char_u *)"+-*/%", *op) != ଖାଲି)
	semsg(_(e_wrong_variable_type_for_str_equal), op);
    ଅନ୍ୟଥା ଯଦି (endchars != ଖାଲି
			  && vim_strchr(endchars, *skipwhite(arg + 1)) == ଖାଲି)
	emsg(_(e_unexpected_characters_in_let));
    ଅନ୍ୟଥା
    {
	char_u	*ptofree = ଖାଲି;
	char_u	*p;

	p = tv_get_string_chk(tv);
	ଯଦି (p != ଖାଲି && op != ଖାଲି && *op == '.')
	{
	    char_u  *s = get_reg_contents(*arg == '@'
						  ? '"' : *arg, GREG_ପଦସମଷ୍ଟି_SRC);

	    ଯଦି (s != ଖାଲି)
	    {
		p = ptofree = concat_str(s, p);
		vim_free(s);
	    }
	}
	ଯଦି (p != ଖାଲି)
	{
	    write_reg_contents(*arg == '@' ? '"' : *arg, p, -1, FALSE);
	    arg_end = arg + 1;
	}
	vim_free(ptofree);
    }
    ଫେରନ୍ତୁ arg_end;
}

/*
 * Set one item of ":let var = expr" or ":let [v1, v2] = list" to its value.
 * Returns a pointer to the char just after the var name.
 * Returns NULL if there is an error.
 */
    ସ୍ଥିର char_u *
ex_let_one(
    char_u	*arg,		// points to variable name
    typval_T	*tv,		// value to assign to variable
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		copy,		// copy value from "tv"
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags,		// ASSIGN_CONST, ASSIGN_FINAL, etc.
    char_u	*endchars,	// valid chars after variable name  or NULL
    char_u	*op,		// "+", "-", "."  or NULL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		var_idx)	// variable index for "let [a, b] = list"
{
    char_u	*arg_end = ଖାଲି;

    ଯଦି (in_vim9script() && (flags & (ASSIGN_NO_DECL | ASSIGN_DECL)) == 0
			&& (flags & (ASSIGN_CONST | ASSIGN_FINAL)) == 0
				  && vim_strchr((char_u *)"$@&", *arg) != ଖାଲି)
    {
	vim9_declare_error(arg);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    ଯଦି (check_typval_is_ମୂଲ୍ୟ(tv) == FAIL)
	ଫେରନ୍ତୁ ଖାଲି;

    // In Vim9 script an environment variable and a register only take a
    // String.  "@#" also takes a buffer number.
    ଯଦି (in_vim9script() && (*arg == '$' || *arg == '@')
	    && !(*arg == '@' && arg[1] == '#' && tv->v_type == VAR_ସଂଖ୍ୟା
					       && (op == ଖାଲି || *op == '=')))
    {
	where_T	where = WHERE_INIT;

	ଯଦି (check_typval_type(&t_string, tv, where) == FAIL)
	    ଫେରନ୍ତୁ ଖାଲି;
    }

    ଯଦି (*arg == '$')
    {
	// ":let $VAR = expr": Set environment variable.
	ଫେରନ୍ତୁ ex_let_env(arg, tv, flags, endchars, op);
    }
    ଅନ୍ୟଥା ଯଦି (*arg == '&')
    {
	// ":let &option = expr": Set option value.
	// ":let &l:option = expr": Set local option value.
	// ":let &g:option = expr": Set global option value.
	// ":for &ts in range(8)": Set option value for for loop
	ଫେରନ୍ତୁ ex_let_option(arg, tv, flags, endchars, op);
    }
    ଅନ୍ୟଥା ଯଦି (*arg == '@')
    {
	// ":let @r = expr": Set register contents.
	ଫେରନ୍ତୁ ex_let_register(arg, tv, flags, endchars, op);
    }
    ଅନ୍ୟଥା ଯଦି (eval_isnamec1(*arg) || *arg == '{')
    {
	lval_T	lv;
	char_u	*p;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	lval_flags = (flags & (ASSIGN_NO_DECL | ASSIGN_DECL))
							     ? GLV_NO_DECL : 0;
	lval_flags |= (flags & ASSIGN_FOR_LOOP) ? GLV_FOR_LOOP : 0;
	ଯଦି (op != ଖାଲି && *op != '=')
	    lval_flags |= GLV_ASSIGN_WITH_OP;

	// ":let var = expr": Set internal variable.
	// ":let var: type = expr": Set internal variable with type.
	// ":let {expr} = expr": Idem, name made with curly braces
	p = get_lval(arg, tv, &lv, FALSE, FALSE, lval_flags, FNE_CHECK_START);
	ଯଦି (p != ଖାଲି && lv.ll_name != ଖାଲି)
	{
	    ଯଦି (endchars != ଖାଲି && vim_strchr(endchars,
					   *skipwhite(lv.ll_name_end)) == ଖାଲି)
	    {
		emsg(_(e_unexpected_characters_in_let));
	    }
	    ଅନ୍ୟଥା
	    {
		set_var_lval(&lv, p, tv, copy, flags, op, var_idx);
		arg_end = lv.ll_name_end;
	    }
	}
	clear_lval(&lv);
    }
    ଅନ୍ୟଥା
	semsg(_(e_invalid_argument_str), arg);

    ଫେରନ୍ତୁ arg_end;
}

/*
 * ":unlet[!] var1 ... " command.
 */
    ଶୂନ୍ୟ
ex_unlet(exarg_T *eap)
{
    ex_unletlock(eap, eap->arg, 0, eap->forceit ? GLV_QUIET : 0, do_unlet_var, ଖାଲି);
}

/*
 * ":lockvar" and ":unlockvar" commands
 */
    ଶୂନ୍ୟ
ex_lockvar(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		deep = 2;

    ଯଦି (eap->forceit)
	deep = -1;
    ଅନ୍ୟଥା ଯଦି (vim_isdigit(*arg))
    {
	deep = getdigits(&arg);
	arg = skipwhite(arg);
    }

    ex_unletlock(eap, arg, deep, 0, do_lock_var, ଖାଲି);
}

/*
 * ":unlet", ":lockvar" and ":unlockvar" are quite similar.
 * Also used for Vim9 script.  "callback" is invoked as:
 *	callback(&lv, name_end, eap, deep, cookie)
 */
    ଶୂନ୍ୟ
ex_unletlock(
    exarg_T	*eap,
    char_u	*argstart,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		deep,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		glv_flags,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		(*callback)(lval_T *, char_u *, exarg_T *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ଶୂନ୍ୟ *),
    ଶୂନ୍ୟ	*cookie)
{
    char_u	*arg = argstart;
    char_u	*name_end;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		error = FALSE;
    lval_T	lv;

    କର
    {
	ଯଦି (*arg == '$')
	{
	    lv.ll_name = arg;
	    lv.ll_tv = ଖାଲି;
	    ++arg;
	    ଯଦି (get_env_len(&arg) == 0)
	    {
		semsg(_(e_invalid_argument_str), arg - 1);
		ଫେରନ୍ତୁ;
	    }
	    ଯଦି (!error && !eap->skip
			      && callback(&lv, arg, eap, deep, cookie) == FAIL)
		error = TRUE;
	    name_end = arg;
	}
	ଅନ୍ୟଥା
	{
	    // Parse the name and find the end.
	    name_end = get_lval(arg, ଖାଲି, &lv, TRUE, eap->skip || error,
				     glv_flags | GLV_NO_DECL, FNE_CHECK_START);
	    ଯଦି (lv.ll_name == ଖାଲି)
		error = TRUE;	    // error but continue parsing
	    ଯଦି (name_end == ଖାଲି || (!VIM_ISWHITE(*name_end)
						    && !ends_excmd(*name_end)))
	    {
		ଯଦି (name_end != ଖାଲି)
		{
		    emsg_severe = TRUE;
		    semsg(_(e_trailing_characters_str), name_end);
		}
		ଯଦି (!(eap->skip || error))
		    clear_lval(&lv);
		ଭାଙ୍ଗ;
	    }

	    ଯଦି (!error && !eap->skip
			 && callback(&lv, name_end, eap, deep, cookie) == FAIL)
		error = TRUE;

	    ଯଦି (!eap->skip)
		clear_lval(&lv);
	}

	arg = skipwhite(name_end);
    } ଯେତେବେଳେ (!ends_excmd2(name_end, arg));

    set_nextcmd(eap, arg);
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
do_unlet_var(
    lval_T	*lp,
    char_u	*name_end,
    exarg_T	*eap,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		deep UNUSED,
    ଶୂନ୍ୟ	*cookie UNUSED)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		forceit = eap->forceit;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret = OK;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cc;

    ଯଦି (lp->ll_tv == ଖାଲି)
    {
	cc = *name_end;
	*name_end = NUL;

	// Environment variable, normal name or expanded name.
	ଯଦି (*lp->ll_name == '$')
	    vim_unsetenv_ext(lp->ll_name + 1);
	ଅନ୍ୟଥା ଯଦି (do_unlet(lp->ll_name, forceit) == FAIL)
	    ret = FAIL;
	*name_end = cc;
    }
    ଅନ୍ୟଥା ଯଦି ((lp->ll_list != ଖାଲି
		 && ମୂଲ୍ୟ_check_lock(lp->ll_list->lv_lock, lp->ll_name, FALSE))
	    || (lp->ll_dict != ଖାଲି
		&& ମୂଲ୍ୟ_check_lock(lp->ll_dict->dv_lock, lp->ll_name, FALSE)))
	ଫେରନ୍ତୁ FAIL;
    ଅନ୍ୟଥା ଯଦି (lp->ll_range)
	list_unlet_range(lp->ll_list, lp->ll_li, lp->ll_n1,
						    !lp->ll_empty2, lp->ll_n2);
    ଅନ୍ୟଥା ଯଦି (lp->ll_list != ଖାଲି)
	// unlet a List item.
	listitem_remove(lp->ll_list, lp->ll_li);
    ଅନ୍ୟଥା ଯଦି (lp->ll_dict != ଖାଲି)
	// unlet a Dictionary item.
	dictitem_remove(lp->ll_dict, lp->ll_di, "unlet");
    ଅନ୍ୟଥା
    {
	semsg(_(e_cannot_unlet_imported_item_str), lp->ll_name);
	ଫେରନ୍ତୁ FAIL;
    }

    ଫେରନ୍ତୁ ret;
}

/*
 * Unlet one item or a range of items from a list.
 * Return OK or FAIL.
 */
    ଶୂନ୍ୟ
list_unlet_range(
	list_T	    *l,
	listitem_T  *li_first,
	long	    n1_arg,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    has_n2,
	long	    n2)
{
    listitem_T  *li = li_first;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n1 = n1_arg;

    // Delete a range of List items.
    li = li_first;
    n1 = n1_arg;
    ଯେତେବେଳେ (li != ଖାଲି && (!has_n2 || n2 >= n1))
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
do_unlet(char_u *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା forceit)
{
    hashtab_T	*ht;
    hashitem_T	*hi;
    char_u	*varname = ଖାଲି;  // init to shut up gcc
    dict_T	*d;
    dictitem_T	*di;

    // can't :unlet a script variable in Vim9 script
    ଯଦି (in_vim9script() && check_vim9_unlet(name) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    ht = find_var_ht(name, &varname);

    // can't :unlet a script variable in Vim9 script from a function
    ଯଦି (ht == get_script_local_ht()
	    && SCRIPT_ID_VALID(current_sctx.sc_sid)
	    && SCRIPT_ITEM(current_sctx.sc_sid)->sn_version
							 == SCRIPT_VERSION_VIM9
	    && check_vim9_unlet(name) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (ht != ଖାଲି && *varname != NUL)
    {
	d = get_current_funccal_dict(ht);
	ଯଦି (d == ଖାଲି)
	{
	    ଯଦି (ht == &globvarht)
		d = &globvardict;
	    ଅନ୍ୟଥା ଯଦି (ht == &compat_hashtab)
		d = &vimvardict;
	    ଅନ୍ୟଥା
	    {
		di = find_var_in_ht(ht, *name, (char_u *)"", FALSE);
		d = di == ଖାଲି ? ଖାଲି : di->di_tv.vval.v_dict;
	    }
	    ଯଦି (d == ଖାଲି)
	    {
		internal_error("do_unlet()");
		ଫେରନ୍ତୁ FAIL;
	    }
	}
	hi = hash_find(ht, varname);
	ଯଦି (HASHITEM_EMPTY(hi))
	    hi = find_hi_in_scoped_ht(name, &ht);
	ଯଦି (hi != ଖାଲି && !HASHITEM_EMPTY(hi))
	{
	    di = HI2DI(hi);
	    ଯଦି (var_check_fixed(di->di_flags, name, FALSE)
		    || var_check_ro(di->di_flags, name, FALSE)
		    || ମୂଲ୍ୟ_check_lock(d->dv_lock, name, FALSE)
		    || check_hashtab_frozen(ht, "unlet"))
		ଫେରନ୍ତୁ FAIL;

	    delete_var(ht, hi);
	    ଫେରନ୍ତୁ OK;
	}
    }
    ଯଦି (forceit)
	ଫେରନ୍ତୁ OK;
    semsg(_(e_no_such_variable_str), name);
    ଫେରନ୍ତୁ FAIL;
}

    ସ୍ଥିର ଶୂନ୍ୟ
report_lockvar_member(ଅକ୍ଷର *ସନ୍ଦେଶ, lval_T *lp)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା did_alloc = FALSE;
    char_u *vname = (char_u *)"";
    char_u *class_name = lp->ll_class != ଖାଲି
				    ? lp->ll_class->class_name.string : (char_u *)"";
    ଯଦି (lp->ll_name != ଖାଲି)
    {
	ଯଦି (lp->ll_name_end == ଖାଲି)
	    vname = lp->ll_name;
	ଅନ୍ୟଥା
	{
	    vname = vim_strnsave(lp->ll_name, lp->ll_name_end - lp->ll_name);
	    ଯଦି (vname == ଖାଲି)
		ଫେରନ୍ତୁ;
	    did_alloc = TRUE;
	}
    }
    semsg(_(ସନ୍ଦେଶ), vname, class_name);
    ଯଦି (did_alloc)
	vim_free(vname);
}

/*
 * Lock or unlock variable indicated by "lp".
 * "deep" is the levels to go (-1 for unlimited);
 * "lock" is TRUE for ":lockvar", FALSE for ":unlockvar".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
do_lock_var(
    lval_T	*lp,
    char_u	*name_end,
    exarg_T	*eap,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		deep,
    ଶୂନ୍ୟ	*cookie UNUSED)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		lock = eap->cmdidx == CMD_lockvar;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret = OK;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cc;
    dictitem_T	*di;

#ifdef LOG_LOCKVAR
    ch_log(ଖାଲି, "LKVAR: do_lock_var(): name %s, is_root %d", lp->ll_name, lp->ll_is_ମୂଳ);
#endif

    ଯଦି (lp->ll_tv == ଖାଲି)
    {
	cc = *name_end;
	*name_end = NUL;
	ଯଦି (*lp->ll_name == '$')
	{
	    semsg(_(e_cannot_lock_or_unlock_variable_str), lp->ll_name);
	    ret = FAIL;
	}
	ଅନ୍ୟଥା
	{
	    // Normal name or expanded name.
	    di = find_var(lp->ll_name, ଖାଲି, TRUE);
	    ଯଦି (di == ଖାଲି)
	    {
		ଯଦି (in_vim9script())
		    semsg(_(e_cannot_find_variable_to_unlock_str),
								  lp->ll_name);
		ret = FAIL;
	    }
	    ଅନ୍ୟଥା
	    {
		ଯଦି ((di->di_flags & DI_FLAGS_FIX)
			    && di->di_tv.v_type != VAR_DICT
			    && di->di_tv.v_type != VAR_LIST)
		{
		    // For historic reasons this error is not given for a list
		    // or dict.  E.g., the b: dict could be locked/unlocked.
		    semsg(_(e_cannot_lock_or_unlock_variable_str), lp->ll_name);
		    ret = FAIL;
		}
		ଅନ୍ୟଥା
		{
		    ଯଦି (in_vim9script())
		    {
			svar_T  *sv = find_typval_in_script(&di->di_tv,
								     0, FALSE);

			ଯଦି (sv != ଖାଲି && sv->sv_const != 0)
			{
			    semsg(_(e_cannot_change_readonly_variable_str),
								  lp->ll_name);
			    ret = FAIL;
			}
		    }

		    ଯଦି (ret == OK)
		    {
			ଯଦି (lock)
			    di->di_flags |= DI_FLAGS_LOCK;
			ଅନ୍ୟଥା
			    di->di_flags &= ~DI_FLAGS_LOCK;
			ଯଦି (deep != 0)
			    item_lock(&di->di_tv, deep, lock, FALSE);
		    }
		}
	    }
	}
	*name_end = cc;
    }
    ଅନ୍ୟଥା ଯଦି (deep == 0 && lp->ll_object == ଖାଲି && lp->ll_class == ଖାଲି)
    {
	// nothing to do
    }
    ଅନ୍ୟଥା ଯଦି (lp->ll_is_ମୂଳ)
	// (un)lock the item.
	item_lock(lp->ll_tv, deep, lock, FALSE);
    ଅନ୍ୟଥା ଯଦି (lp->ll_range)
    {
	listitem_T    *li = lp->ll_li;

	// (un)lock a range of List items.
	ଯେତେବେଳେ (li != ଖାଲି && (lp->ll_empty2 || lp->ll_n2 >= lp->ll_n1))
	{
	    item_lock(&li->li_tv, deep, lock, FALSE);
	    li = li->li_next;
	    ++lp->ll_n1;
	}
    }
    ଅନ୍ୟଥା ଯଦି (lp->ll_list != ଖାଲି)
	// (un)lock a List item.
	item_lock(&lp->ll_li->li_tv, deep, lock, FALSE);
    ଅନ୍ୟଥା ଯଦି (lp->ll_object != ଖାଲି)  // This check must be before ll_class.
    {
	// (un)lock an object variable.
	report_lockvar_member(e_cannot_lock_object_variable_str, lp);
	ret = FAIL;
    }
    ଅନ୍ୟଥା ଯଦି (lp->ll_class != ଖାଲି)
    {
	// (un)lock a class variable.
	report_lockvar_member(e_cannot_lock_class_variable_str, lp);
	ret = FAIL;
    }
    ଅନ୍ୟଥା
    {
	// (un)lock a Dictionary item.
	ଯଦି (lp->ll_di == ଖାଲି)
	{
	    emsg(_(e_dictionary_required));
	    ret = FAIL;
	}
	ଅନ୍ୟଥା
	    item_lock(&lp->ll_di->di_tv, deep, lock, FALSE);
    }

    ଫେରନ୍ତୁ ret;
}

/*
 * Lock or unlock an item.  "deep" is nr of levels to go.
 * When "check_refcount" is TRUE do not lock a list or dict with a reference
 * count larger than 1.
 */
    ଶୂନ୍ୟ
item_lock(typval_T *tv, ପୂର୍ଣ୍ଣସଂଖ୍ୟା deep, ପୂର୍ଣ୍ଣସଂଖ୍ୟା lock, ପୂର୍ଣ୍ଣସଂଖ୍ୟା check_refcount)
{
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	recurse = 0;
    list_T	*l;
    listitem_T	*li;
    dict_T	*d;
    blob_T	*b;
    hashitem_T	*hi;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		todo;

#ifdef LOG_LOCKVAR
    ch_log(ଖାଲି, "LKVAR: item_lock(): type %s", vartype_name(tv->v_type));
#endif

    ଯଦି (recurse >= DICT_MAXNEST)
    {
	emsg(_(e_variable_nested_too_deep_for_unlock));
	ଫେରନ୍ତୁ;
    }
    ଯଦି (deep == 0)
	ଫେରନ୍ତୁ;
    ++recurse;

    // lock/unlock the item itself
    ଯଦି (lock)
	tv->v_lock |= VAR_LOCKED;
    ଅନ୍ୟଥା
	tv->v_lock &= ~VAR_LOCKED;

    ବାଛ (tv->v_type)
    {
	ସ୍ଥିତି VAR_UNKNOWN:
	ସ୍ଥିତି VAR_ANY:
	ସ୍ଥିତି VAR_VOID:
	ସ୍ଥିତି VAR_ସଂଖ୍ୟା:
	ସ୍ଥିତି VAR_BOOL:
	ସ୍ଥିତି VAR_STRING:
	ସ୍ଥିତି VAR_FUNC:
	ସ୍ଥିତି VAR_PARTIAL:
	ସ୍ଥିତି VAR_FLOAT:
	ସ୍ଥିତି VAR_SPECIAL:
	ସ୍ଥିତି VAR_JOB:
	ସ୍ଥିତି VAR_CHANNEL:
	ସ୍ଥିତି VAR_INSTR:
	ସ୍ଥିତି VAR_CLASS:
	ସ୍ଥିତି VAR_OBJECT:
	ସ୍ଥିତି VAR_TYPEALIAS:
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି VAR_BLOB:
	    ଯଦି ((b = tv->vval.v_blob) != ଖାଲି
				    && !(check_refcount && b->bv_refcount > 1))
	    {
		ଯଦି (lock)
		    b->bv_lock |= VAR_LOCKED;
		ଅନ୍ୟଥା
		    b->bv_lock &= ~VAR_LOCKED;
	    }
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_LIST:
	    ଯଦି ((l = tv->vval.v_list) != ଖାଲି
				    && !(check_refcount && l->lv_refcount > 1))
	    {
		ଯଦି (lock)
		    l->lv_lock |= VAR_LOCKED;
		ଅନ୍ୟଥା
		    l->lv_lock &= ~VAR_LOCKED;
		ଯଦି (deep < 0 || deep > 1)
		{
		    ଯଦି (l->lv_first == &range_list_item)
			l->lv_lock |= VAR_ବସ୍ତୁଗୁଡ଼ିକ_LOCKED;
		    ଅନ୍ୟଥା
		    {
			// recursive: lock/unlock the items the List contains
			CHECK_LIST_MATERIALIZE(l);
			FOR_ALL_LIST_ବସ୍ତୁଗୁଡ଼ିକ(l, li) item_lock(&li->li_tv,
					       deep - 1, lock, check_refcount);
		    }
		}
	    }
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_TUPLE:
	    tuple_lock(tv->vval.v_tuple, deep, lock, check_refcount);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_DICT:
	    ଯଦି ((d = tv->vval.v_dict) != ଖାଲି
				    && !(check_refcount && d->dv_refcount > 1))
	    {
		ଯଦି (lock)
		    d->dv_lock |= VAR_LOCKED;
		ଅନ୍ୟଥା
		    d->dv_lock &= ~VAR_LOCKED;
		ଯଦି (deep < 0 || deep > 1)
		{
		    // recursive: lock/unlock the items the List contains
		    todo = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)d->dv_hashtab.ht_used;
		    FOR_ALL_HASHTAB_ବସ୍ତୁଗୁଡ଼ିକ(&d->dv_hashtab, hi, todo)
		    {
			ଯଦି (!HASHITEM_EMPTY(hi))
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

#ଯଦି defined(FEAT_MENU) && defined(FEAT_MULTI_LANG)
/*
 * Delete all "menutrans_" variables.
 */
    ଶୂନ୍ୟ
del_menutrans_vars(ଶୂନ୍ୟ)
{
    hashitem_T	*hi;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		todo;

    hash_lock(&globvarht);
    todo = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)globvarht.ht_used;
    ପାଇଁ (hi = globvarht.ht_array; todo > 0 && !got_int; ++hi)
    {
	ଯଦି (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    ଯଦି (STRNCMP(HI2DI(hi)->di_ଚାବି, "menutrans_", 10) == 0)
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

ସ୍ଥିର char_u	*varnamebuf = ଖାଲି;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	varnamebuflen = 0;

/*
 * Function to concatenate a prefix and a variable name.
 */
    char_u *
cat_prefix_varname(ପୂର୍ଣ୍ଣସଂଖ୍ୟା prefix, char_u *name)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;

    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(name) + 3;
    ଯଦି (len > varnamebuflen)
    {
	vim_free(varnamebuf);
	len += 10;			// some additional space
	varnamebuf = alloc(len);
	ଯଦି (varnamebuf == ଖାଲି)
	{
	    varnamebuflen = 0;
	    ଫେରନ୍ତୁ ଖାଲି;
	}
	varnamebuflen = len;
    }
    *varnamebuf = prefix;
    varnamebuf[1] = ':';
    STRCPY(varnamebuf + 2, name);
    ଫେରନ୍ତୁ varnamebuf;
}

/*
 * Function given to ExpandGeneric() to obtain the list of user defined
 * (global/buffer/window/built-in) variable names.
 */
    char_u *
get_user_var_name(expand_T *xp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ସ୍ଥିର long_u	gdone;
    ସ୍ଥିର long_u	bdone;
    ସ୍ଥିର long_u	wdone;
    ସ୍ଥିର long_u	tdone;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା		vidx;
    ସ୍ଥିର hashitem_T	*hi;
    hashtab_T		*ht;

    ଯଦି (idx == 0)
    {
	gdone = bdone = wdone = vidx = 0;
	tdone = 0;
    }

    // Global variables
    ଯଦି (gdone < globvarht.ht_used)
    {
	ଯଦି (gdone++ == 0)
	    hi = globvarht.ht_array;
	ଅନ୍ୟଥା
	    ++hi;
	ଯେତେବେଳେ (HASHITEM_EMPTY(hi))
	    ++hi;
	ଯଦି (STRNCMP("g:", xp->xp_pattern, 2) == 0)
	    ଫେରନ୍ତୁ cat_prefix_varname('g', hi->hi_ଚାବି);
	ଫେରନ୍ତୁ hi->hi_ଚାବି;
    }

    // b: variables
    ht = &prevwin_curwin()->w_buffer->b_vars->dv_hashtab;
    ଯଦି (bdone < ht->ht_used)
    {
	ଯଦି (bdone++ == 0)
	    hi = ht->ht_array;
	ଅନ୍ୟଥା
	    ++hi;
	ଯେତେବେଳେ (HASHITEM_EMPTY(hi))
	    ++hi;
	ଫେରନ୍ତୁ cat_prefix_varname('b', hi->hi_ଚାବି);
    }

    // w: variables
    ht = &prevwin_curwin()->w_vars->dv_hashtab;
    ଯଦି (wdone < ht->ht_used)
    {
	ଯଦି (wdone++ == 0)
	    hi = ht->ht_array;
	ଅନ୍ୟଥା
	    ++hi;
	ଯେତେବେଳେ (HASHITEM_EMPTY(hi))
	    ++hi;
	ଫେରନ୍ତୁ cat_prefix_varname('w', hi->hi_ଚାବି);
    }

    // t: variables
    ht = &curtab->tp_vars->dv_hashtab;
    ଯଦି (tdone < ht->ht_used)
    {
	ଯଦି (tdone++ == 0)
	    hi = ht->ht_array;
	ଅନ୍ୟଥା
	    ++hi;
	ଯେତେବେଳେ (HASHITEM_EMPTY(hi))
	    ++hi;
	ଫେରନ୍ତୁ cat_prefix_varname('t', hi->hi_ଚାବି);
    }

    // v: variables
    ଯଦି (vidx < VV_LEN)
	ଫେରନ୍ତୁ cat_prefix_varname('v', (char_u *)vimvars[vidx++].vv_name);

    VIM_CLEAR(varnamebuf);
    varnamebuflen = 0;
    ଫେରନ୍ତୁ ଖାଲି;
}

    ଅକ୍ଷର *
get_var_special_name(ପୂର୍ଣ୍ଣସଂଖ୍ୟା nr)
{
    ବାଛ (nr)
    {
	ସ୍ଥିତି VVAL_FALSE: ଫେରନ୍ତୁ in_vim9script() ? "false" : "v:false";
	ସ୍ଥିତି VVAL_TRUE:  ଫେରନ୍ତୁ in_vim9script() ? "true" : "v:true";
	ସ୍ଥିତି VVAL_NULL:  ଫେରନ୍ତୁ in_vim9script() ? "null" : "v:null";
	ସ୍ଥିତି VVAL_NONE:  ଫେରନ୍ତୁ "v:none";
    }
    internal_error("get_var_special_name()");
    ଫେରନ୍ତୁ "42";
}

/*
 * Returns the global variable dictionary
 */
    dict_T *
get_globvar_dict(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ &globvardict;
}

/*
 * Returns the global variable hash table
 */
    hashtab_T *
get_globvar_ht(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ &globvarht;
}

/*
 * Returns the v: variable dictionary
 */
    dict_T *
get_vimvar_dict(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ &vimvardict;
}

/*
 * Returns the index of a v:variable.  Negative if not found.
 * Returns DI_ flags in "di_flags".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
find_vim_var(char_u *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *di_flags)
{
    dictitem_T	    *di = find_var_in_ht(&vimvarht, 0, name, TRUE);
    ସଂରଚନା vimvar   *vv;

    ଯଦି (di == ଖାଲି)
	ଫେରନ୍ତୁ -1;
    *di_flags = di->di_flags;
    vv = (ସଂରଚନା vimvar *)((ଅକ୍ଷର *)di - offsetof(vimvar_T, vv_di));
    ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(vv - vimvars);
}


/*
 * Set type of v: variable to "type".
 */
    ଶୂନ୍ୟ
set_vim_var_type(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, vartype_T type)
{
    vimvars[idx].vv_tv_type = type;
}

/*
 * Set number v: variable to "val".
 * Note that this does not set the type, use set_vim_var_type() for that.
 */
    ଶୂନ୍ୟ
set_vim_var_nr(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, varnumber_T val)
{
    vimvars[idx].vv_nr = val;
}

    ଅକ୍ଷର *
get_vim_var_name(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଫେରନ୍ତୁ vimvars[idx].vv_name;
}

/*
 * Get typval_T v: variable value.
 */
    typval_T *
get_vim_var_tv(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଫେରନ୍ତୁ &vimvars[idx].vv_tv;
}

    type_T *
get_vim_var_type(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, garray_T *type_list)
{
    ଯଦି (vimvars[idx].vv_type != ଖାଲି)
	ଫେରନ୍ତୁ vimvars[idx].vv_type;
    ଫେରନ୍ତୁ typval2type_vimvar(&vimvars[idx].vv_tv, type_list);
}

/*
 * Add what getinfo() reports for the v: variable "name", without the "v:", to
 * "d".  Returns FAIL when there is no such variable.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim_var_info(char_u *name, dict_T *d)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		di_flags;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx = find_vim_var(name, &di_flags);
    garray_T	type_list;
    ଅକ୍ଷର	*tofree;

    ଯଦି (idx < 0)
	ଫେରନ୍ତୁ FAIL;
    vim_snprintf((ଅକ୍ଷର *)IObuff, IOSIZE, "v:%s", vimvars[idx].vv_name);
    dict_add_string(d, "name", IObuff);
    dict_add_bool(d, "available", TRUE);
    ga_init2(&type_list, ଆକାର(type_T *), 10);
    ଅକ୍ଷର *type = type_name(get_vim_var_type(idx, &type_list), &tofree);
    dict_add_string(d, "type", (char_u *)type);
    vim_free(tofree);
    clear_type_list(&type_list);
    dict_add_bool(d, "readonly", (vimvars[idx].vv_flags & VV_RO) != 0);
    dict_add_bool(d, "compat", (vimvars[idx].vv_flags & VV_COMPAT) != 0);
    ଫେରନ୍ତୁ OK;
}

/*
 * Set v: variable to "tv".  Only accepts the same type.
 * Takes over the value of "tv".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
set_vim_var_tv(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, typval_T *tv)
{
    ଯଦି (vimvars[idx].vv_tv_type != tv->v_type)
    {
	emsg(_(e_type_mismatch_for_v_variable));
	clear_tv(tv);
	ଫେରନ୍ତୁ FAIL;
    }
    // VV_RO is also checked when compiling, but let's check here as well.
    ଯଦି (vimvars[idx].vv_flags & VV_RO)
    {
	semsg(_(e_cannot_change_readonly_variable_str), vimvars[idx].vv_name);
	ଫେରନ୍ତୁ FAIL;
    }
    ଯଦି (sandbox && (vimvars[idx].vv_flags & VV_RO_SBX))
    {
	semsg(_(e_cannot_set_variable_in_sandbox_str), vimvars[idx].vv_name);
	ଫେରନ୍ତୁ FAIL;
    }
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_di.di_tv = *tv;
    ଫେରନ୍ତୁ OK;
}

/*
 * Get number v: variable value.
 */
    varnumber_T
get_vim_var_nr(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଫେରନ୍ତୁ vimvars[idx].vv_nr;
}

/*
 * Get string v: variable value.  Uses a static buffer, can only be used once.
 * If the String variable has never been set, return an empty string.
 * Never returns NULL;
 */
    char_u *
get_vim_var_str(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଫେରନ୍ତୁ tv_get_string(&vimvars[idx].vv_tv);
}

/*
 * Get List v: variable value.  Caller must take care of reference count when
 * needed.
 */
    list_T *
get_vim_var_list(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଫେରନ୍ତୁ vimvars[idx].vv_list;
}

/*
 * Get Dict v: variable value.  Caller must take care of reference count when
 * needed.
 */
    dict_T *
get_vim_var_dict(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଫେରନ୍ତୁ vimvars[idx].vv_dict;
}

/*
 * Set v:char to character "c".
 */
    ଶୂନ୍ୟ
set_vim_var_char(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    char_u  buf[MB_MAXBYTES + 1];
    size_t  buflen;

    ଯଦି (has_mbyte)
	buflen = (*mb_char2bytes)(c, buf);
    ଅନ୍ୟଥା
    {
	buf[0] = c;
	buflen = 1;
    }
    buf[buflen] = NUL;

    set_vim_var_string(VV_CHAR, buf, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)buflen);
}

/*
 * Set v:count to "count" and v:count1 to "count1".
 * When "set_prevcount" is TRUE first set v:prevcount from v:count.
 */
    ଶୂନ୍ୟ
set_vcount(
    long	ଗଣନା_2,
    long	count1,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		set_prevcount)
{
    ଯଦି (set_prevcount)
	vimvars[VV_PREVCOUNT].vv_nr = vimvars[VV_ଗଣନା].vv_nr;
    vimvars[VV_ଗଣନା].vv_nr = ଗଣନା_2;
    vimvars[VV_COUNT1].vv_nr = count1;
}

/*
 * Save variables that might be changed as a side effect.  Used when executing
 * a timer callback.
 */
    ଶୂନ୍ୟ
save_vimvars(vimvars_save_T *vvsave)
{
    vvsave->vv_prevcount = vimvars[VV_PREVCOUNT].vv_nr;
    vvsave->vv_ଗଣନା = vimvars[VV_ଗଣନା].vv_nr;
    vvsave->vv_count1 = vimvars[VV_COUNT1].vv_nr;
}

/*
 * Restore variables saved by save_vimvars().
 */
    ଶୂନ୍ୟ
restore_vimvars(vimvars_save_T *vvsave)
{
    vimvars[VV_PREVCOUNT].vv_nr = vvsave->vv_prevcount;
    vimvars[VV_ଗଣନା].vv_nr = vvsave->vv_ଗଣନା;
    vimvars[VV_COUNT1].vv_nr = vvsave->vv_count1;
}

/*
 * Set string v: variable to a copy of "val". If 'copy' is FALSE, then set the
 * value.
 */
    ଶୂନ୍ୟ
set_vim_var_string(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx,
    char_u	*val,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len)	    // length of "val" to use or -1 (whole string)
{
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_tv_type = VAR_STRING;
    ଯଦି (val == ଖାଲି)
	vimvars[idx].vv_str = ଖାଲି;
    ଅନ୍ୟଥା ଯଦି (len == -1)
	vimvars[idx].vv_str = vim_strsave(val);
    ଅନ୍ୟଥା
	vimvars[idx].vv_str = vim_strnsave(val, len);
}

    ଶୂନ୍ୟ
set_vim_var_string_direct(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx,
    char_u	*val)
{
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_tv_type = VAR_STRING;

    vimvars[idx].vv_str = val;
}

/*
 * Set List v: variable to "val".
 */
    ଶୂନ୍ୟ
set_vim_var_list(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, list_T *val)
{
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_tv_type = VAR_LIST;
    vimvars[idx].vv_list = val;
    ଯଦି (val != ଖାଲି)
	++val->lv_refcount;
}

/*
 * Set Dictionary v: variable to "val".
 */
    ଶୂନ୍ୟ
set_vim_var_dict(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, dict_T *val)
{
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_tv_type = VAR_DICT;
    vimvars[idx].vv_dict = val;
    ଯଦି (val == ଖାଲି)
	ଫେରନ୍ତୁ;

    ++val->dv_refcount;
    dict_set_ବସ୍ତୁଗୁଡ଼ିକ_ro(val);
}

/*
 * Set the v:argv list.
 */
    ଶୂନ୍ୟ
set_argv_var(ଅକ୍ଷର **argv, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc)
{
    list_T	*l = list_alloc();
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ଯଦି (l == ଖାଲି)
	getout(1);
    l->lv_lock = VAR_FIXED;
    ପାଇଁ (i = 0; i < argc; ++i)
    {
	ଯଦି (list_append_string(l, (char_u *)argv[i], -1) == FAIL)
	    getout(1);
	l->lv_u.mat.lv_last->li_tv.v_lock = VAR_FIXED;
    }
    set_vim_var_list(VV_ARGV, l);
}

/*
 * Reset v:register, taking the 'clipboard' setting into account.
 */
    ଶୂନ୍ୟ
reset_reg_var(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା regname = 0;

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
    ଶୂନ୍ୟ
set_reg_var(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    char_u  regname[2];

    ଯଦି (c == 0 || c == ' ')
	regname[0] = '"';
    ଅନ୍ୟଥା
	regname[0] = c;
    regname[1] = NUL;
    // Avoid free/alloc when the value is already right.
    ଯଦି (vimvars[VV_REG].vv_str == ଖାଲି || vimvars[VV_REG].vv_str[0] != c)
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
    ଯଦି (oldval == ଖାଲି)
	ଫେରନ୍ତୁ vimvars[VV_EXCEPTION].vv_str;

    vimvars[VV_EXCEPTION].vv_str = oldval;
    ଫେରନ୍ତୁ ଖାଲି;
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
    ଯଦି (oldval == ଖାଲି)
	ଫେରନ୍ତୁ vimvars[VV_THROWPOINT].vv_str;

    vimvars[VV_THROWPOINT].vv_str = oldval;
    ଫେରନ୍ତୁ ଖାଲି;
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
    ଯଦି (eap == ଖାଲି)
    {
	vim_free(oldval);
	vimvars[VV_CMDARG].vv_str = oldarg;
	ଫେରନ୍ତୁ ଖାଲି;
    }

    ଯଦି (eap->force_bin == FORCE_BIN)
	len = 6;
    ଅନ୍ୟଥା ଯଦି (eap->force_bin == FORCE_NOBIN)
	len = 8;
    ଅନ୍ୟଥା
	len = 0;

    ଯଦି (eap->read_edit)
	len += 7;

    ଯଦି (eap->force_ff != 0)
	len += 10; // " ++ff=unix"
    ଯଦି (eap->force_enc != 0)
	len += (unsigned)STRLEN(eap->cmd + eap->force_enc) + 7;
    ଯଦି (eap->bad_char != 0)
	len += 7 + 4;  // " ++bad=" + "keep" or "drop"

    newval = alloc(len + 1);
    ଯଦି (newval == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (eap->force_bin == FORCE_BIN)
	sprintf((ଅକ୍ଷର *)newval, " ++bin");
    ଅନ୍ୟଥା ଯଦି (eap->force_bin == FORCE_NOBIN)
	sprintf((ଅକ୍ଷର *)newval, " ++nobin");
    ଅନ୍ୟଥା
	*newval = NUL;

    ଯଦି (eap->read_edit)
	STRCAT(newval, " ++edit");

    ଯଦି (eap->force_ff != 0)
	sprintf((ଅକ୍ଷର *)newval + STRLEN(newval), " ++ff=%s",
						eap->force_ff == 'u' ? "unix"
						: eap->force_ff == 'd' ? "dos"
						: "mac");
    ଯଦି (eap->force_enc != 0)
	sprintf((ଅକ୍ଷର *)newval + STRLEN(newval), " ++enc=%s",
					       eap->cmd + eap->force_enc);
    ଯଦି (eap->bad_char == BAD_KEEP)
	STRCPY(newval + STRLEN(newval), " ++bad=keep");
    ଅନ୍ୟଥା ଯଦି (eap->bad_char == BAD_DROP)
	STRCPY(newval + STRLEN(newval), " ++bad=drop");
    ଅନ୍ୟଥା ଯଦି (eap->bad_char != 0)
	sprintf((ଅକ୍ଷର *)newval + STRLEN(newval), " ++bad=%c", eap->bad_char);
    vimvars[VV_CMDARG].vv_str = newval;
    ଫେରନ୍ତୁ oldval;
}

/*
 * Get the value of internal variable "name".
 * If "flags" has EVAL_VAR_IMPORT may return a VAR_ANY with v_number set to the
 * imported script ID.
 * Return OK or FAIL.  If OK is returned "rettv" must be cleared.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
eval_variable(
    char_u	*name,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len,		// length of "name" or zero
    scid_T	sid,		// script ID for imported item or zero
    typval_T	*rettv,		// NULL when only checking existence
    dictitem_T	**dip,		// non-NULL when typval's dict item is needed
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags)		// EVAL_VAR_ flags
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret = OK;
    typval_T	*tv = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		found = FALSE;
    hashtab_T	*ht = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cc = 0;
    type_T	*type = ଖାଲି;

    ଯଦି (len > 0)
    {
	// truncate the name, so that we can use strcmp()
	cc = name[len];
	name[len] = NUL;
    }

    // Check for local variable when debugging.
    ଯଦି ((sid == 0) && (tv = lookup_debug_var(name)) == ଖାଲି)
    {
	// Check for user-defined variables.
	dictitem_T	*v = find_var(name, &ht, flags & EVAL_VAR_NOAUTOLOAD);

	ଯଦି (v != ଖାଲି)
	{
	    tv = &v->di_tv;
	    ଯଦି (dip != ଖାଲି)
		*dip = v;
	}
	ଅନ୍ୟଥା
	    ht = ଖାଲି;
    }

    ଯଦି (tv == ଖାଲି && (in_vim9script() || STRNCMP(name, "s:", 2) == 0))
    {
	imported_T  *import = ଖାଲି;
	char_u	    *p = STRNCMP(name, "s:", 2) == 0 ? name + 2 : name;

	ଯଦି (sid == 0)
	    import = find_imported(p, 0, TRUE);

	// imported variable from another script
	ଯଦି (import != ଖାଲି || sid != 0)
	{
	    ଯଦି ((flags & EVAL_VAR_IMPORT) == 0)
	    {
		ଯଦି (SCRIPT_ID_VALID(sid))
		{
		    ht = &SCRIPT_VARS(sid);
		    ଯଦି (ht != ଖାଲି)
		    {
			dictitem_T *v = find_var_in_ht(ht, 0, name,
						  flags & EVAL_VAR_NOAUTOLOAD);

			ଯଦି (v == ଖାଲି)
			    v = find_var_autoload_prefix(name, sid, ଖାଲି, ଖାଲି);

			ଯଦି (v != ଖାଲି)
			{
			    tv = &v->di_tv;
			    ଯଦି (dip != ଖାଲି)
				*dip = v;
			}
			ଅନ୍ୟଥା
			    ht = ଖାଲି;
		    }
		}
		ଅନ୍ୟଥା
		{
		    ଯଦି (flags & EVAL_VAR_VERBOSE)
			semsg(_(e_expected_dot_after_name_str), name);
		    ret = FAIL;
		}
	    }
	    ଅନ୍ୟଥା
	    {
		ଯଦି (rettv != ଖାଲି)
		{
		    // special value that is used in handle_subscript()
		    rettv->v_type = VAR_ANY;
		    rettv->vval.v_ସଂଖ୍ୟା = sid != 0 ? sid : import->imp_sid;
		}
		found = TRUE;
	    }
	}
	ଅନ୍ୟଥା ଯଦି (in_vim9script() && (flags & EVAL_VAR_NO_FUNC) == 0)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    has_g_prefix = STRNCMP(name, "g:", 2) == 0;
	    ufunc_T *ufunc = find_func(name, FALSE);

	    ଯଦି (ufunc != ଖାଲି && cc == '<')
	    {
		// handle generic function
		char_u	*argp = name + len;
		name[len] = cc;
		ufunc = eval_generic_func(ufunc, name, &argp);
		name[len] = NUL;
		ଯଦି (ufunc == ଖାଲି)
		{
		    ret = FAIL;
		    ଯାଅ done;
		}
	    }

	    // In Vim9 script we can get a function reference by using the
	    // function name.  For a global non-autoload function "g:" is
	    // required.
	    ଯଦି (ufunc != ଖାଲି && (has_g_prefix
					    || !func_requires_g_prefix(ufunc)))
	    {
		found = TRUE;
		ଯଦି (rettv != ଖାଲି)
		{
		    rettv->v_type = VAR_FUNC;
		    ଯଦି (has_g_prefix)
		    {
			// Keep the "g:", otherwise script-local may be
			// assumed.
			ଯଦି (cc != '<')
			    rettv->vval.v_string = vim_strsave(name);
			ଅନ୍ୟଥା
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
		    ଅନ୍ୟଥା
			rettv->vval.v_string = vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
		    ଯଦି (rettv->vval.v_string != ଖାଲି)
			func_ref(ufunc->uf_name);
		}
	    }
	}
    }

    ଯଦି (!found)
    {
	ଯଦି (tv == ଖାଲି)
	{
	    ଯଦି (rettv != ଖାଲି && (flags & EVAL_VAR_VERBOSE))
		semsg(_(e_undefined_variable_str), name);
	    ret = FAIL;
	}
	ଅନ୍ୟଥା ଯଦି (rettv != ଖାଲି)
	{
	    svar_T  *sv = ଖାଲି;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    was_assigned = FALSE;

	    ଯଦି (ht != ଖାଲି && ht == get_script_local_ht()
		    && tv != &SCRIPT_SV(current_sctx.sc_sid)->sv_var.di_tv)
	    {
		sv = find_typval_in_script(tv, 0, TRUE);
		ଯଦି (sv != ଖାଲି)
		{
		    type = sv->sv_type;
		    was_assigned = sv->sv_flags & SVFLAG_ASSIGNED;
		}
	    }

	    ଯଦି ((tv->v_type == VAR_TYPEALIAS || tv->v_type == VAR_CLASS)
		    && sid != 0)
	    {
		// type alias or class imported from another script.  Check
		// whether it is exported from the other script.
		sv = find_typval_in_script(tv, sid, TRUE);
		ଯଦି (sv == ଖାଲି)
		{
		    ret = FAIL;
		    ଯାଅ done;
		}
		ଯଦି ((sv->sv_flags & SVFLAG_EXPORTED) == 0)
		{
		    semsg(_(e_item_not_exported_in_script_str), name);
		    ret = FAIL;
		    ଯାଅ done;
		}
	    }

	    // If a list or tuple or dict variable wasn't initialized and has
	    // meaningful type, do it now.  Not for global variables, they are
	    // not declared.
	    ଯଦି (ht != &globvarht)
	    {
		ଯଦି (tv->v_type == VAR_DICT && tv->vval.v_dict == ଖାଲି
					    && ((type != ଖାଲି && !was_assigned)
							  || !in_vim9script()))
		{
		    tv->vval.v_dict = dict_alloc();
		    ଯଦି (tv->vval.v_dict != ଖାଲି)
		    {
			++tv->vval.v_dict->dv_refcount;
			tv->vval.v_dict->dv_type = alloc_type(type);
			ଯଦି (sv != ଖାଲି)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
		ଅନ୍ୟଥା ଯଦି (tv->v_type == VAR_LIST && tv->vval.v_list == ଖାଲି
					    && ((type != ଖାଲି && !was_assigned)
							  || !in_vim9script()))
		{
		    tv->vval.v_list = list_alloc();
		    ଯଦି (tv->vval.v_list != ଖାଲି)
		    {
			++tv->vval.v_list->lv_refcount;
			tv->vval.v_list->lv_type = alloc_type(type);
			ଯଦି (sv != ଖାଲି)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
		ଅନ୍ୟଥା ଯଦି (tv->v_type == VAR_TUPLE && tv->vval.v_tuple == ଖାଲି
					    && ((type != ଖାଲି && !was_assigned)
							  || !in_vim9script()))
		{
		    tv->vval.v_tuple = tuple_alloc();
		    ଯଦି (tv->vval.v_tuple != ଖାଲି)
		    {
			++tv->vval.v_tuple->tv_refcount;
			tv->vval.v_tuple->tv_type = alloc_type(type);
			ଯଦି (sv != ଖାଲି)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
		ଅନ୍ୟଥା ଯଦି (tv->v_type == VAR_BLOB && tv->vval.v_blob == ଖାଲି
					    && ((type != ଖାଲି && !was_assigned)
							  || !in_vim9script()))
		{
		    tv->vval.v_blob = blob_alloc();
		    ଯଦି (tv->vval.v_blob != ଖାଲି)
		    {
			++tv->vval.v_blob->bv_refcount;
			ଯଦି (sv != ଖାଲି)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
	    }
	    ret = copy_tv(tv, rettv);
	}
    }

done:
    ଯଦି (len > 0)
	name[len] = cc;

    ଫେରନ୍ତୁ ret;
}

/*
 * Get the value of internal variable "name", also handling "import.name".
 * Return OK or FAIL.  If OK is returned "rettv" must be cleared.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
eval_variable_import(
    char_u	*name,
    typval_T	*rettv)
{
    char_u  *s = name;
    ଯେତେବେଳେ (ASCII_ISALNUM(*s) || *s == '_')
	++s;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(s - name);

    ଯଦି (eval_variable(name, len, 0, rettv, ଖାଲି, EVAL_VAR_IMPORT) == FAIL)
	ଫେରନ୍ତୁ FAIL;
    ଯଦି (rettv->v_type == VAR_ANY && *s == '.')
    {
	char_u *ns = s + 1;
	s = ns;
	ଯେତେବେଳେ (ASCII_ISALNUM(*s) || *s == '_')
	    ++s;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା sid = rettv->vval.v_ସଂଖ୍ୟା;
	ଫେରନ୍ତୁ eval_variable(ns, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(s - ns), sid, rettv, ଖାଲି, 0);
    }
    ଫେରନ୍ତୁ OK;
}


/*
 * Check if variable "name[len]" is a local variable or an argument.
 * If so, "*eval_lavars_used" is set to TRUE.
 */
    ଶୂନ୍ୟ
check_vars(char_u *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cc;
    char_u	*varname;
    hashtab_T	*ht;

    ଯଦି (eval_lavars_used == ଖାଲି)
	ଫେରନ୍ତୁ;

    // truncate the name, so that we can use strcmp()
    cc = name[len];
    name[len] = NUL;

    ht = find_var_ht(name, &varname);
    ଯଦି (ht == get_funccal_local_ht() || ht == get_funccal_args_ht())
    {
	ଯଦି (find_var(name, ଖାଲି, TRUE) != ଖାଲି)
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
find_var(char_u *name, hashtab_T **htp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା no_autoload)
{
    char_u	*varname;
    hashtab_T	*ht;
    dictitem_T	*ret = ଖାଲି;

    ht = find_var_ht(name, &varname);
    ଯଦି (htp != ଖାଲି)
	*htp = ht;
    ଯଦି (ht == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    ret = find_var_in_ht(ht, *name, varname, no_autoload);
    ଯଦି (ret != ଖାଲି)
	ଫେରନ୍ତୁ ret;

    // Search in parent scope for lambda
    ret = find_var_in_scoped_ht(name, no_autoload);
    ଯଦି (ret != ଖାଲି)
	ଫେରନ୍ତୁ ret;

    // in Vim9 script items without a scope can be script-local
    ଯଦି (in_vim9script() && name[0] != NUL && name[1] != ':')
    {
	ht = get_script_local_ht();
	ଯଦି (ht != ଖାଲି)
	{
	    ret = find_var_in_ht(ht, *name, varname, no_autoload);
	    ଯଦି (ret != ଖାଲି)
	    {
		ଯଦି (htp != ଖାଲି)
		    *htp = ht;
		ଫେରନ୍ତୁ ret;
	    }
	}
    }

    // and finally try
    ଫେରନ୍ତୁ find_var_autoload_prefix(name, 0, htp, ଖାଲି);
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
find_var_autoload_prefix(char_u *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା sid, hashtab_T **htp,
							    char_u **namep)
{
    hashtab_T	*ht;
    dictitem_T	*ret = ଖାଲି;
    // When using "vim9script autoload" script-local items are prefixed but can
    // be used with s:name.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା check_sid = sid > 0 ? sid : current_sctx.sc_sid;
    ଯଦି (SCRIPT_ID_VALID(check_sid)
		   && (in_vim9script() || (name[0] == 's' && name[1] == ':')))
    {
	scriptitem_T *si = SCRIPT_ITEM(check_sid);

	ଯଦି (si->sn_autoload_prefix != ଖାଲି)
	{
	    char_u *base_name = (name[0] == 's' && name[1] == ':')
							     ? name + 2 : name;
	    char_u *auto_name = concat_str(si->sn_autoload_prefix, base_name);

	    ଯଦି (auto_name != ଖାଲି)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା free_auto_name = TRUE;
		ht = &globvarht;
		ret = find_var_in_ht(ht, 'g', auto_name, TRUE);
		ଯଦି (ret != ଖାଲି)
		{
		    ଯଦି (htp != ଖାଲି)
			*htp = ht;
		    ଯଦି (namep != ଖାଲି)
		    {
			free_auto_name = FALSE;
			*namep = auto_name;
		    }
		}
		ଯଦି (free_auto_name)
		    vim_free(auto_name);
	    }
	}
    }

    ଫେରନ୍ତୁ ret;
}

/*
 * Like find_var() but if the name starts with <SNR>99_ then look in the
 * referenced script (used for a funcref).
 */
    dictitem_T *
find_var_also_in_script(char_u *name, hashtab_T **htp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା no_autoload)
{
    ଯଦି (STRNCMP(name, "<SNR>", 5) == 0 && SAFE_isdigit(name[5]))
    {
	char_u	    *p = name + 5;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    sid = getdigits(&p);

	ଯଦି (SCRIPT_ID_VALID(sid) && *p == '_')
	{
	    hashtab_T	*ht = &SCRIPT_VARS(sid);

	    ଯଦି (ht != ଖାଲି)
	    {
		dictitem_T *di = find_var_in_ht(ht, 0, p + 1, no_autoload);

		ଯଦି (di != ଖାଲି)
		{
		    ଯଦି (htp != ଖାଲି)
			*htp = ht;
		    ଫେରନ୍ତୁ di;
		}
	    }
	}
    }

    ଫେରନ୍ତୁ find_var(name, htp, no_autoload);
}

/*
 * Find variable "varname" in hashtab "ht" with name "htname".
 * When "varname" is empty returns curwin/curtab/etc vars dictionary.
 * Returns NULL if not found.
 */
    dictitem_T *
find_var_in_ht(
    hashtab_T	*ht,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		htname,
    char_u	*varname,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		no_autoload)
{
    hashitem_T	*hi;

    ଯଦି (*varname == NUL)
    {
	// Must be something like "s:", otherwise "ht" would be NULL.
	ବାଛ (htname)
	{
	    ସ୍ଥିତି 's': ଫେରନ୍ତୁ &SCRIPT_SV(current_sctx.sc_sid)->sv_var;
	    ସ୍ଥିତି 'g': ଫେରନ୍ତୁ &globvars_var;
	    ସ୍ଥିତି 'v': ଫେରନ୍ତୁ &vimvars_var;
	    ସ୍ଥିତି 'b': ଫେରନ୍ତୁ &curbuf->b_bufvar;
	    ସ୍ଥିତି 'w': ଫେରନ୍ତୁ &curwin->w_winvar;
	    ସ୍ଥିତି 't': ଫେରନ୍ତୁ &curtab->tp_winvar;
	    ସ୍ଥିତି 'l': ଫେରନ୍ତୁ get_funccal_local_var();
	    ସ୍ଥିତି 'a': ଫେରନ୍ତୁ get_funccal_args_var();
	}
	ଫେରନ୍ତୁ ଖାଲି;
    }

    hi = hash_find(ht, varname);
    ଯଦି (HASHITEM_EMPTY(hi))
    {
	// For global variables we may try auto-loading the script.  If it
	// worked find the variable again.  Don't auto-load a script if it was
	// loaded already, otherwise it would be loaded every time when
	// checking if a function name is a Funcref variable.
	ଯଦି (ht == &globvarht && !no_autoload)
	{
	    // Note: script_autoload() may make "hi" invalid. It must either
	    // be obtained again or not used.
	    ଯଦି (!script_autoload(varname, FALSE) || aborting())
		ଫେରନ୍ତୁ ଖାଲି;
	    hi = hash_find(ht, varname);
	}
	ଯଦି (HASHITEM_EMPTY(hi))
	    ଫେରନ୍ତୁ ଖାଲି;
    }
    ଫେରନ୍ତୁ HI2DI(hi);
}

/*
 * Get the script-local hashtab.  NULL if not in a script context.
 */
    hashtab_T *
get_script_local_ht(ଶୂନ୍ୟ)
{
    scid_T sid = current_sctx.sc_sid;

    ଯଦି (SCRIPT_ID_VALID(sid))
	ଫେରନ୍ତୁ &SCRIPT_VARS(sid);
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Look for "name[len]" in script-local variables and functions.
 * When "cmd" is TRUE it must look like a command, a function must be followed
 * by "(" or "->".
 * Return OK when found, FAIL when not found.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
lookup_scriptitem(
	char_u	*name,
	size_t	len,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	cmd,
	cctx_T	*dummy UNUSED)
{
    hashtab_T	*ht = get_script_local_ht();
    char_u	buffer[30];
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		res;
    hashitem_T	*hi;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_global = FALSE;
    char_u	*fname = name;

    ଯଦି (ht == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    ଯଦି (len < ଆକାର(buffer) - 1)
    {
	// avoid an alloc/free for short names
	vim_strncpy(buffer, name, len);
	p = buffer;
    }
    ଅନ୍ୟଥା
    {
	p = vim_strnsave(name, len);
	ଯଦି (p == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;
    }

    hi = hash_find(ht, p);
    res = HASHITEM_EMPTY(hi) ? FAIL : OK;

    // if not script-local, then perhaps autoload-exported
    ଯଦି (res == FAIL && find_var_autoload_prefix(p, 0, ଖାଲି, ଖାଲି) != ଖାଲି)
	res = OK;

    // if not script-local or autoload, then perhaps imported
    ଯଦି (res == FAIL && find_imported(p, 0, FALSE) != ଖାଲି)
	res = OK;
    ଯଦି (p != buffer)
	vim_free(p);

    // Find a function, so that a following "->" works.
    // When used as a command require "(" or "->" to follow, "Cmd" is a user
    // command while "Cmd()" is a function call.
    ଯଦି (res != OK)
    {
	p = skipwhite(name + len);

	ଯଦି (!cmd || name[len] == '(' || (p[0] == '-' && p[1] == '>'))
	{
	    // Do not check for an internal function, since it might also be a
	    // valid command, such as ":split" versus "split()".
	    // Skip "g:" before a function name.
	    ଯଦି (name[0] == 'g' && name[1] == ':')
	    {
		is_global = TRUE;
		fname = name + 2;
	    }
	    ଯଦି (find_func(fname, is_global) != ଖାଲି)
		res = OK;
	}
    }

    ଫେରନ୍ତୁ res;
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

    ଯଦି (name[0] == NUL)
	ଫେରନ୍ତୁ ଖାଲି;
    ଯଦି (name[1] != ':')
    {
	// The name must not start with a colon or #.
	ଯଦି (name[0] == ':' || name[0] == AUTOLOAD_CHAR)
	    ଫେରନ୍ତୁ ଖାଲି;
	*varname = name;

	// "version" is "v:version" in all scopes if scriptversion < 3.
	// Same for a few other variables marked with VV_COMPAT.
	ଯଦି (in_old_script(3))
	{
	    hi = hash_find(&compat_hashtab, name);
	    ଯଦି (!HASHITEM_EMPTY(hi))
		ଫେରନ୍ତୁ &compat_hashtab;
	}

	ht = get_funccal_local_ht();
	ଯଦି (ht != ଖାଲି)
	    ଫେରନ୍ତୁ ht;				// local variable

	// In Vim9 script items at the script level are script-local, except
	// for autoload names.
	ଯଦି (in_vim9script() && vim_strchr(name, AUTOLOAD_CHAR) == ଖାଲି)
	{
	    ht = get_script_local_ht();
	    ଯଦି (ht != ଖାଲି)
		ଫେରନ୍ତୁ ht;
	}

	ଫେରନ୍ତୁ &globvarht;			// global variable
    }
    *varname = name + 2;
    ଯଦି (*name == 'g')				// global variable
	ଫେରନ୍ତୁ &globvarht;
    // There must be no ':' or '#' in the rest of the name, unless g: is used
    ଯଦି (vim_strchr(name + 2, ':') != ଖାଲି
			       || vim_strchr(name + 2, AUTOLOAD_CHAR) != ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    ଯଦି (*name == 'b')				// buffer variable
	ଫେରନ୍ତୁ &curbuf->b_vars->dv_hashtab;
    ଯଦି (*name == 'w')				// window variable
	ଫେରନ୍ତୁ &curwin->w_vars->dv_hashtab;
    ଯଦି (*name == 't')				// tab page variable
	ଫେରନ୍ତୁ &curtab->tp_vars->dv_hashtab;
    ଯଦି (*name == 'v')				// v: variable
	ଫେରନ୍ତୁ &vimvarht;
    ଯଦି (get_current_funccal() != ଖାଲି
	       && get_current_funccal()->fc_func->uf_def_status
							    == UF_NOT_COMPILED)
    {
	// a: and l: are only used in functions defined with ":function"
	ଯଦି (*name == 'a')			// a: function argument
	    ଫେରନ୍ତୁ get_funccal_args_ht();
	ଯଦି (*name == 'l')			// l: local function variable
	    ଫେରନ୍ତୁ get_funccal_local_ht();
    }
    ଯଦି (*name == 's')				// script variable
    {
	ht = get_script_local_ht();
	ଯଦି (ht != ଖାଲି)
	    ଫେରନ୍ତୁ ht;
    }
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Get the string value of a (global/local) variable.
 * Note: see tv_get_string() for how long the pointer remains valid.
 * Returns NULL when it doesn't exist.
 */
    char_u *
get_var_ମୂଲ୍ୟ(char_u *name)
{
    dictitem_T	*v;

    v = find_var(name, ଖାଲି, FALSE);
    ଯଦି (v == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    ଫେରନ୍ତୁ tv_get_string(&v->di_tv);
}

/*
 * Allocate a new hashtab for a sourced script.  It will be used while
 * sourcing this script and when executing functions defined in the script.
 */
    ଶୂନ୍ୟ
new_script_vars(scid_T id)
{
    scriptvar_T *sv;

    sv = ALLOC_CLEAR_ONE(scriptvar_T);
    ଯଦି (sv == ଖାଲି)
	ଫେରନ୍ତୁ;
    init_var_dict(&sv->sv_dict, &sv->sv_var, VAR_SCOPE);
    SCRIPT_ITEM(id)->sn_vars = sv;
}

/*
 * Initialize dictionary "dict" as a scope and set variable "dict_var" to
 * point to it.
 */
    ଶୂନ୍ୟ
init_var_dict(dict_T *dict, dictitem_T *dict_var, ପୂର୍ଣ୍ଣସଂଖ୍ୟା scope)
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
    dict_var->di_ଚାବି[0] = NUL;
}

/*
 * Unreference a dictionary initialized by init_var_dict().
 */
    ଶୂନ୍ୟ
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
    ଶୂନ୍ୟ
vars_clear(hashtab_T *ht)
{
    vars_clear_ext(ht, TRUE);
}

/*
 * Like vars_clear(), but only free the value if "free_val" is TRUE.
 */
    ଶୂନ୍ୟ
vars_clear_ext(hashtab_T *ht, ପୂର୍ଣ୍ଣସଂଖ୍ୟା free_val)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		todo;
    hashitem_T	*hi;
    dictitem_T	*v;

    hash_lock(ht);
    todo = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ht->ht_used;
    FOR_ALL_HASHTAB_ବସ୍ତୁଗୁଡ଼ିକ(ht, hi, todo)
    {
	ଯଦି (!HASHITEM_EMPTY(hi))
	{
	    --todo;

	    // Free the variable.  Don't remove it from the hashtab,
	    // ht_array might change then.  hash_clear() takes care of it
	    // later.
	    v = HI2DI(hi);
	    ଯଦି (free_val)
		clear_tv(&v->di_tv);
	    ଯଦି (v->di_flags & DI_FLAGS_ALLOC)
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
    ଶୂନ୍ୟ
delete_var(hashtab_T *ht, hashitem_T *hi)
{
    dictitem_T	*di = HI2DI(hi);

    ଯଦି (hash_remove(ht, hi, "delete variable") != OK)
	ଫେରନ୍ତୁ;

    clear_tv(&di->di_tv);
    vim_free(di);
}

/*
 * Delete the exported variables of a reloaded Vim9 autoload script.  They live
 * in the global namespace with the autoload prefix (e.g. "foo#bar") and are
 * recreated when the script body runs again.
 */
    ଶୂନ୍ୟ
delete_autoload_export_vars(char_u *prefix)
{
    ଯଦି (prefix == ଖାଲି)
	ଫେରନ୍ତୁ;
    size_t  prefixlen = STRLEN(prefix);

    hash_lock(&globvarht);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	todo = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)globvarht.ht_used;

    ପାଇଁ (hashitem_T *hi = globvarht.ht_array; todo > 0; ++hi)
	ଯଦି (!HASHITEM_EMPTY(hi))
	{
	    dictitem_T	*di = HI2DI(hi);

	    --todo;
	    // Keep a class or enum: existing objects still refer to it.
	    ଯଦି (di->di_tv.v_type != VAR_CLASS
		    && STRNCMP(di->di_ଚାବି, prefix, prefixlen) == 0)
		delete_var(&globvarht, hi);
	}
    hash_unlock(&globvarht);
}

/*
 * List the value of one internal variable.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
list_one_var(dictitem_T *v, ଅକ୍ଷର *prefix, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *first)
{
    char_u	*tofree;
    char_u	*s;
    char_u	numbuf[NUMBUFLEN];

    s = echo_string(&v->di_tv, &tofree, numbuf, get_copyID());
    list_one_var_a(prefix, v->di_ଚାବି, v->di_tv.v_type,
					 s == ଖାଲି ? (char_u *)"" : s, first);
    vim_free(tofree);
}

    ସ୍ଥିର ଶୂନ୍ୟ
list_one_var_a(
    ଅକ୍ଷର	*prefix,
    char_u	*name,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		type,
    char_u	*string,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*first)  // when TRUE clear rest of screen and set to FALSE
{
    // don't use msg() or msg_attr() to avoid overwriting "v:statusmsg"
    ସନ୍ଦେଶ_start();
    ସନ୍ଦେଶ_puts(prefix);
    ଯଦି (name != ଖାଲି)	// "a:" vars don't have a name stored
	ସନ୍ଦେଶ_puts((ଅକ୍ଷର *)name);
    ସନ୍ଦେଶ_putchar(' ');
    ସନ୍ଦେଶ_advance(22);
    ଯଦି (type == VAR_ସଂଖ୍ୟା)
	ସନ୍ଦେଶ_putchar('#');
    ଅନ୍ୟଥା ଯଦି (type == VAR_FUNC || type == VAR_PARTIAL)
	ସନ୍ଦେଶ_putchar('*');
    ଅନ୍ୟଥା ଯଦି (type == VAR_LIST)
    {
	ସନ୍ଦେଶ_putchar('[');
	ଯଦି (*string == '[')
	    ++string;
    }
    ଅନ୍ୟଥା ଯଦି (type == VAR_DICT)
    {
	ସନ୍ଦେଶ_putchar('{');
	ଯଦି (*string == '{')
	    ++string;
    }
    ଅନ୍ୟଥା
	ସନ୍ଦେଶ_putchar(' ');

    ସନ୍ଦେଶ_outtrans(string);

    ଯଦି (type == VAR_FUNC || type == VAR_PARTIAL)
	ସନ୍ଦେଶ_puts("()");
    ଯଦି (*first)
    {
	ସନ୍ଦେଶ_clr_eos();
	*first = FALSE;
    }
}

/*
 * Addition handling for setting a v: variable.
 * Return TRUE if the variable should be set normally,
 *        FALSE if nothing else needs to be done.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
before_set_vvar(
    char_u	*varname,
    dictitem_T	*di,
    typval_T	*tv,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		copy,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*type_error)
{
    ଯଦି (di->di_tv.v_type == VAR_STRING)
    {
	VIM_CLEAR(di->di_tv.vval.v_string);
	ଯଦି (copy || tv->v_type != VAR_STRING)
	{
	    char_u *val = tv_get_string(tv);

	    // Careful: when assigning to v:errmsg and
	    // tv_get_string() causes an error message the variable
	    // will already be set.
	    ଯଦି (di->di_tv.vval.v_string == ଖାଲି)
		di->di_tv.vval.v_string = vim_strsave(val);
	}
	ଅନ୍ୟଥା
	{
	    // Take over the string to avoid an extra alloc/free.
	    di->di_tv.vval.v_string = tv->vval.v_string;
	    tv->vval.v_string = ଖାଲି;
	}
	ଫେରନ୍ତୁ FALSE;
    }
    ଅନ୍ୟଥା ଯଦି (di->di_tv.v_type == VAR_ସଂଖ୍ୟା)
    {
	di->di_tv.vval.v_ସଂଖ୍ୟା = tv_get_ସଂଖ୍ୟା(tv);
	ଯଦି (STRCMP(varname, "searchforward") == 0)
	    set_ସନ୍ଧାନ_direction(di->di_tv.vval.v_ସଂଖ୍ୟା ? '/' : '?');
#ifdef FEAT_ସନ୍ଧାନ_EXTRA
	ଅନ୍ୟଥା ଯଦି (STRCMP(varname, "hlsearch") == 0)
	{
	    no_hlsearch = !di->di_tv.vval.v_ସଂଖ୍ୟା;
	    redraw_all_later(UPD_SOME_VALID);
	}
#endif
	ଫେରନ୍ତୁ FALSE;
    }
    ଅନ୍ୟଥା ଯଦି (di->di_tv.v_type != tv->v_type)
    {
	*type_error = TRUE;
	ଫେରନ୍ତୁ FALSE;
    }
    ଫେରନ୍ତୁ TRUE;
}

/*
 * Set variable "name" to value in "tv".
 * If the variable already exists, the value is updated.
 * Otherwise the variable is created.
 */
    ଶୂନ୍ୟ
set_var(
    char_u	*name,
    typval_T	*tv,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		copy)	    // make copy of value in "tv"
{
    set_var_const(name, 0, ଖାଲି, tv, copy, ASSIGN_DECL, 0);
}

/*
 * Set variable "name" to value in "tv_arg".
 * When "sid" is non-zero "name" is in the script with this ID.
 * If the variable already exists and "is_const" is FALSE the value is updated.
 * Otherwise the variable is created.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
set_var_const(
    char_u	*name,
    scid_T	sid,
    type_T	*type_arg,
    typval_T	*tv_arg,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		copy,	    // make copy of value in "tv"
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags_arg,  // ASSIGN_CONST, ASSIGN_FINAL, etc.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		var_idx)    // index for ":let [a, b] = list"
{
    typval_T	*tv = tv_arg;
    type_T	*type = type_arg;
    typval_T	bool_tv;
    dictitem_T	*di;
    typval_T	*dest_tv;
    char_u	*varname;
    char_u	*name_tofree = ଖାଲି;
    hashtab_T	*ht = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_script_local;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		vim9script = in_vim9script();
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		var_in_vim9script;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		var_in_autoload = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags = flags_arg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		free_tv_arg = !copy;  // free tv_arg if not used
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		rc = FAIL;

    ଯଦି (sid != 0)
    {
	varname = ଖାଲି;
	ଯଦି (SCRIPT_ID_VALID(sid))
	{
	    char_u	*auto_name = ଖାଲି;
	    ଯଦି (find_var_autoload_prefix(name, sid, &ht, &auto_name) != ଖାଲି)
	    {
		var_in_autoload = TRUE;
		varname = auto_name;
		name_tofree = varname;
	    }
	    ଅନ୍ୟଥା
		ht = &SCRIPT_VARS(sid);
	}
	ଯଦି (varname == ଖାଲି)
	    varname = name;
    }
    ଅନ୍ୟଥା
    {
	scriptitem_T	*si;
	char_u		*auto_name = ଖାଲି;

	ଯଦି (in_vim9script()
	    && SCRIPT_ID_VALID(current_sctx.sc_sid)
	    && (si = SCRIPT_ITEM(current_sctx.sc_sid))
					      ->sn_autoload_prefix != ଖାଲି
	    && (is_export
		|| find_var_autoload_prefix(name, 0, ଖାଲି, &auto_name)
								    != ଖାଲି))
	{
	    // In a vim9 autoload script an exported variable is put in the
	    // global namespace with the autoload prefix.
	    var_in_autoload = TRUE;
	    varname = auto_name != ଖାଲି ? auto_name
		      : concat_str(si->sn_autoload_prefix, name);
	    ଯଦି (varname == ଖାଲି)
		ଯାଅ failed;
	    name_tofree = varname;
	    ht = &globvarht;
	}
	ଅନ୍ୟଥା
	    ht = find_var_ht(name, &varname);
    }
    ଯଦି (ht == ଖାଲି || *varname == NUL)
    {
	semsg(_(e_illegal_variable_name_str), name);
	ଯାଅ failed;
    }
    is_script_local = ht == get_script_local_ht() || sid != 0
							    || var_in_autoload;

    ଯଦି (vim9script
	    && !is_script_local
	    && (flags & (ASSIGN_NO_DECL | ASSIGN_DECL)) == 0
	    && (flags & (ASSIGN_CONST | ASSIGN_FINAL)) == 0
	    && name[1] == ':')
    {
	vim9_declare_error(name);
	ଯାଅ failed;
    }
    ଯଦି ((flags & ASSIGN_FOR_LOOP) && is_scoped_variable(name))
	// Do not make g:var, w:var, b:var or t:var final.
	flags &= ~ASSIGN_FINAL;

    var_in_vim9script = is_script_local && current_script_is_vim9();
    ଯଦି (var_in_vim9script && name[0] == '_' && name[1] == NUL)
    {
	// For "[a, _] = list" the underscore is ignored.
	ଯଦି ((flags & ASSIGN_UNPACK) == 0)
	    emsg(_(e_cannot_use_underscore_here));
	ଯାଅ failed;
    }

    di = find_var_in_ht(ht, 0, varname, TRUE);

    ଯଦି (di == ଖାଲି && var_in_vim9script)
    {
	imported_T  *import = find_imported(varname, 0, FALSE);

	ଯଦି (import != ଖାଲି)
	{
	    // imported name space cannot be used
	    ଯଦି ((flags & ASSIGN_NO_DECL) == 0)
	    {
		semsg(_(e_redefining_imported_item_str), name);
		ଯାଅ failed;
	    }
	    semsg(_(e_cannot_use_str_itself_it_is_imported), name);
	    ଯାଅ failed;
	}
	ଯଦି (!in_vim9script())
	{
	    semsg(_(e_cannot_create_vim9_script_variable_in_function_str),
									 name);
	    ଯାଅ failed;
	}
    }

    // Search in parent scope which is possible to reference from lambda
    ଯଦି (di == ଖାଲି)
	di = find_var_in_scoped_ht(name, TRUE);

    ଯଦି ((tv->v_type == VAR_FUNC || tv->v_type == VAR_PARTIAL)
				  && var_wrong_func_name(name, di == ଖାଲି))
	ଯାଅ failed;

    ଯଦି (need_convert_to_bool(type, tv))
    {
	// Destination is a bool and the value is not, but it can be
	// converted.
	CLEAR_FIELD(bool_tv);
	bool_tv.v_type = VAR_BOOL;
	bool_tv.vval.v_ସଂଖ୍ୟା = tv2bool(tv) ? VVAL_TRUE : VVAL_FALSE;
	tv = &bool_tv;
    }

    ଯଦି (di != ଖାଲି)
    {
	// Item already exists.  Allowed to replace when reloading.
	ଯଦି ((di->di_flags & DI_FLAGS_RELOAD) == 0)
	{
	    ଯଦି ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					 && (flags & ASSIGN_FOR_LOOP) == 0)
	    {
		emsg(_(e_cannot_modify_existing_variable));
		ଯାଅ failed;
	    }

	    ଯଦି (is_script_local && vim9script
			  && (flags & (ASSIGN_NO_DECL | ASSIGN_DECL)) == 0)
	    {
		semsg(_(e_redefining_script_item_str), name);
		ଯାଅ failed;
	    }

	    ଯଦି (check_typval_is_ମୂଲ୍ୟ(&di->di_tv) == FAIL)
		ଯାଅ failed;

	    // List and Blob types can be modified in-place using the "+="
	    // compound operator.  For other types, this is not allowed.
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା type_inplace_modifiable =
		(di->di_tv.v_type == VAR_LIST || di->di_tv.v_type == VAR_BLOB);

	    ଯଦି (var_in_vim9script && (flags & ASSIGN_FOR_LOOP) == 0
		    && ((flags & ASSIGN_COMPOUND_OP) == 0
			|| !type_inplace_modifiable))
	    {
		where_T where = WHERE_INIT;
		svar_T  *sv = find_typval_in_script(&di->di_tv, sid, TRUE);

		ଯଦି (sv != ଖାଲି)
		{
		    // check the type and adjust to bool if needed
		    ଯଦି (var_idx > 0)
		    {
			where.wt_index = var_idx;
			where.wt_kind = WT_VARIABLE;
		    }
		    ଯଦି (check_script_var_type(sv, tv, name, where) == FAIL)
			ଯାଅ failed;
		    ଯଦି (type == ଖାଲି)
			type = sv->sv_type;
		    sv->sv_flags |= SVFLAG_ASSIGNED;
		}
	    }

	    // Modifying a final variable with a List value using the "+="
	    // operator is allowed.  For other types, it is not allowed.
	    ଯଦି ((((flags & ASSIGN_FOR_LOOP) == 0 || (flags & ASSIGN_DECL) == 0)
			&& ((flags & ASSIGN_COMPOUND_OP) == 0
			    || !type_inplace_modifiable))
				 ? var_check_permission(di, name) == FAIL
				 : var_check_ro(di->di_flags, name, FALSE))
		ଯାଅ failed;
	}
	ଅନ୍ୟଥା
	{
	    // can only redefine once
	    di->di_flags &= ~DI_FLAGS_RELOAD;

	    // A Vim9 script-local variable is also present in sn_all_vars
	    // and sn_var_vals.  It may set "type" from "tv".
	    ଯଦି (var_in_vim9script || var_in_autoload)
		update_vim9_script_var(FALSE, di,
			var_in_autoload ? name : di->di_ଚାବି, flags,
			tv, &type, (flags & ASSIGN_NO_MEMBER_TYPE) == 0);
	}

	// existing variable, need to clear the value

	// In Vim9 script a String v: variable only takes a String.
	ଯଦି (ht == &vimvarht && di->di_tv.v_type == VAR_STRING
							    && in_vim9script())
	{
	    where_T	where = WHERE_INIT;

	    ଯଦି (check_typval_type(&t_string, tv, where) == FAIL)
		ଯାଅ failed;
	}

	// Handle setting internal v: variables separately where needed to
	// prevent changing the type.
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା type_error = FALSE;
	ଯଦି (ht == &vimvarht
		&& !before_set_vvar(varname, di, tv, copy, &type_error))
	{
	    ଯଦି (type_error)
		semsg(_(e_setting_v_str_to_ମୂଲ୍ୟ_with_wrong_type), varname);
	    ଯାଅ failed;
	}

	clear_tv(&di->di_tv);

	ଯଦି ((flags & ASSIGN_UPDATE_BLOCK_ID)
				       && SCRIPT_ID_VALID(current_sctx.sc_sid))
	{
	    scriptitem_T *si = SCRIPT_ITEM(current_sctx.sc_sid);

	    update_script_var_block_id(name, si->sn_current_block_id);
	}
    }
    ଅନ୍ୟଥା
    {
	// Item not found, check if a function already exists.
	ଯଦି (is_script_local && (flags & (ASSIGN_NO_DECL | ASSIGN_DECL)) == 0
	       && lookup_scriptitem(name, STRLEN(name), FALSE, ଖାଲି) == OK)
	{
	    semsg(_(e_redefining_script_item_str), name);
	    ଯାଅ failed;
	}

	// add a new variable
	ଯଦି (var_in_vim9script && (flags & ASSIGN_NO_DECL))
	{
	    semsg(_(e_unknown_variable_str), name);
	    ଯାଅ failed;
	}

	ଯଦି (check_hashtab_frozen(ht, "add variable"))
	    ଯାଅ failed;

	// Can't add "v:" or "a:" variable.
	ଯଦି (ht == &vimvarht || ht == get_funccal_args_ht())
	{
	    semsg(_(e_illegal_variable_name_str), name);
	    ଯାଅ failed;
	}

	// Make sure the variable name is valid.  In Vim9 script an
	// autoload variable must be prefixed with "g:" unless in an
	// autoload script.
	ଯଦି (!valid_varname(varname, -1, !vim9script
			|| STRNCMP(name, "g:", 2) == 0 || var_in_autoload))
	    ଯାଅ failed;

	di = alloc(offsetof(dictitem_T, di_ଚାବି) + STRLEN(varname) + 1);
	ଯଦି (di == ଖାଲି)
	    ଯାଅ failed;
	STRCPY(di->di_ଚାବି, varname);
	ଯଦି (hash_add(ht, DI2HIKEY(di), "add variable") == FAIL)
	{
	    vim_free(di);
	    ଯାଅ failed;
	}
	di->di_flags = DI_FLAGS_ALLOC;
	ଯଦି (flags & (ASSIGN_CONST | ASSIGN_FINAL))
	    di->di_flags |= DI_FLAGS_LOCK;

	// A Vim9 script-local variable is also added to sn_all_vars and
	// sn_var_vals. It may set "type" from "tv".
	ଯଦି (var_in_vim9script || var_in_autoload)
	    update_vim9_script_var(TRUE, di,
		    var_in_autoload ? name : di->di_ଚାବି, flags,
			  tv, &type, (flags & ASSIGN_NO_MEMBER_TYPE) == 0);
    }

    dest_tv = &di->di_tv;
    ଯଦି (copy || tv->v_type == VAR_ସଂଖ୍ୟା || tv->v_type == VAR_FLOAT)
	copy_tv(tv, dest_tv);
    ଅନ୍ୟଥା
    {
	*dest_tv = *tv;
	dest_tv->v_lock = 0;
	init_tv(tv);
    }
    free_tv_arg = FALSE;

    ଯଦି (vim9script && type != ଖାଲି)
	set_tv_type(dest_tv, type);

    // ":const var = value" locks the value
    // ":final var = value" locks "var"
    ଯଦି (flags & ASSIGN_CONST)
	// Like :lockvar! name: lock the value and what it contains, but only
	// if the reference count is up to one.  That locks only literal
	// values.
	item_lock(dest_tv, DICT_MAXNEST, TRUE, TRUE);

    rc = OK;

failed:
    vim_free(name_tofree);
    ଯଦି (free_tv_arg)
	clear_tv(tv_arg);

    ଫେରନ୍ତୁ rc;
}

/*
 * Check in this order for backwards compatibility:
 * - Whether the variable is read-only
 * - Whether the variable value is locked
 * - Whether the variable is locked
 * NOTE: "name" is only used for error messages.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
var_check_permission(dictitem_T *di, char_u *name)
{
    ଯଦି (var_check_ro(di->di_flags, name, FALSE)
		    || ମୂଲ୍ୟ_check_lock(di->di_tv.v_lock, name, FALSE)
		    || var_check_lock(di->di_flags, name, FALSE))
	ଫେରନ୍ତୁ FAIL;
    ଫେରନ୍ତୁ OK;
}

/*
 * Return TRUE if di_flags "flags" indicates variable "name" is read-only.
 * Also give an error message.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
var_check_ro(ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags, char_u *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା use_gettext)
{
    ଯଦି (flags & DI_FLAGS_RO)
    {
	ଯଦି (name == ଖାଲି)
	    emsg(_(e_cannot_change_readonly_variable));
	ଅନ୍ୟଥା
	    semsg(_(e_cannot_change_readonly_variable_str),
				       use_gettext ? (char_u *)_(name) : name);
	ଫେରନ୍ତୁ TRUE;
    }
    ଯଦି ((flags & DI_FLAGS_RO_SBX) && sandbox)
    {
	ଯଦି (name == ଖାଲି)
	    emsg(_(e_cannot_set_variable_in_sandbox));
	ଅନ୍ୟଥା
	    semsg(_(e_cannot_set_variable_in_sandbox_str),
				       use_gettext ? (char_u *)_(name) : name);
	ଫେରନ୍ତୁ TRUE;
    }
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Return TRUE if di_flags "flags" indicates variable "name" is locked.
 * Also give an error message.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
var_check_lock(ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags, char_u *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା use_gettext)
{
    ଯଦି (flags & DI_FLAGS_LOCK)
    {
	semsg(_(e_variable_is_locked_str),
				       use_gettext ? (char_u *)_(name) : name);
	ଫେରନ୍ତୁ TRUE;
    }
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Return TRUE if di_flags "flags" indicates variable "name" is fixed.
 * Also give an error message.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
var_check_fixed(ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags, char_u *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା use_gettext)
{
    ଯଦି (flags & DI_FLAGS_FIX)
    {
	ଯଦି (name == ଖାଲି)
	    emsg(_(e_cannot_delete_variable));
	ଅନ୍ୟଥା
	    semsg(_(e_cannot_delete_variable_str),
				      use_gettext ? (char_u *)_(name) : name);
	ଫେରନ୍ତୁ TRUE;
    }
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Check if a funcref is assigned to a valid variable name.
 * Return TRUE and give an error if not.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
var_wrong_func_name(
    char_u *name,    // points to start of variable name
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା    new_var)  // TRUE when creating the variable
{
    // Allow for w: b: s: and t:.  In Vim9 script s: is not allowed, because
    // the name can be used without the s: prefix.
    // Allow autoload variable.
    ଯଦି (!((vim_strchr((char_u *)"wbt", name[0]) != ଖାଲି
		    || (!in_vim9script() && name[0] == 's')) && name[1] == ':')
	    && !ASCII_ISUPPER((name[0] != NUL && name[1] == ':')
						     ? name[2] : name[0])
	    && vim_strchr(name, '#') == ଖାଲି)
    {
	semsg(_(e_funcref_variable_name_must_start_with_capital_str), name);
	ଫେରନ୍ତୁ TRUE;
    }
    // Don't allow hiding a function.  When "v" is not NULL we might be
    // assigning another function to the same var, the type is checked
    // below.
    ଯଦି (new_var && function_exists(name, FALSE))
    {
	semsg(_(e_variable_name_conflicts_with_existing_function_str),
								    name);
	ଫେରନ୍ତୁ TRUE;
    }
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Return TRUE if "flags" indicates variable "name" has a locked (immutable)
 * value.  Also give an error message, using "name" or _("name") when
 * "use_gettext" is TRUE.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ମୂଲ୍ୟ_check_lock(ପୂର୍ଣ୍ଣସଂଖ୍ୟା lock, char_u *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା use_gettext)
{
    ଯଦି (lock & VAR_LOCKED)
    {
	ଯଦି (name == ଖାଲି)
	    emsg(_(e_ମୂଲ୍ୟ_is_locked));
	ଅନ୍ୟଥା
	    semsg(_(e_ମୂଲ୍ୟ_is_locked_str),
				       use_gettext ? (char_u *)_(name) : name);
	ଫେରନ୍ତୁ TRUE;
    }
    ଯଦି (lock & VAR_FIXED)
    {
	ଯଦି (name == ଖାଲି)
	    emsg(_(e_cannot_change_ମୂଲ୍ୟ));
	ଅନ୍ୟଥା
	    semsg(_(e_cannot_change_ମୂଲ୍ୟ_of_str),
				       use_gettext ? (char_u *)_(name) : name);
	ଫେରନ୍ତୁ TRUE;
    }
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Check if a variable name is valid.  When "autoload" is true "#" is allowed.
 * If "len" is -1 use all of "varname", otherwise up to "varname[len]".
 * Return FALSE and give an error if not.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
valid_varname(char_u *varname, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len, ପୂର୍ଣ୍ଣସଂଖ୍ୟା autoload)
{
    char_u *p;

    ପାଇଁ (p = varname; len < 0 ? *p != NUL : p < varname + len; ++p)
	ଯଦି (!eval_isnamec1(*p) && (p == varname || !VIM_ISDIGIT(*p))
					 && !(autoload && *p == AUTOLOAD_CHAR))
	{
	    semsg(_(e_illegal_variable_name_str), varname);
	    ଫେରନ୍ତୁ FALSE;
	}
    ଫେରନ୍ତୁ TRUE;
}

/*
 * Implements the logic to retrieve local variable and option values.
 * Used by "getwinvar()" "gettabvar()" "gettabwinvar()" "getbufvar()".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
get_var_from(
    char_u	*varname,
    typval_T	*rettv,
    typval_T	*deftv,	    // Default value if not found.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		htname,	    // 't'ab, 'w'indow or 'b'uffer local.
    tabpage_T	*tp,	    // can be NULL
    win_T	*win,
    buf_T	*buf)	    // Ignored if htname is not 'b'.
{
    dictitem_T	*v;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		done = FALSE;
    switchwin_T	switchwin;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		need_switch_win;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		do_change_curbuf = buf != ଖାଲି && htname == 'b';

    ++emsg_off;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ଖାଲି;

    ଯଦି (varname != ଖାଲି && tp != ଖାଲି && win != ଖାଲି
	    && (htname != 'b' || buf != ଖାଲି))
    {
	// Set curwin to be our win, temporarily.  Also set the tabpage,
	// otherwise the window is not valid. Only do this when needed,
	// autocommands get blocked.
	// If we have a buffer reference avoid the switching, we're saving and
	// restoring curbuf directly.
	need_switch_win = !(tp == curtab && win == curwin) && !do_change_curbuf;
	ଯଦି (!need_switch_win || switch_win(&switchwin, win, tp, TRUE) == OK)
	{
	    // Handle options. There are no tab-local options.
	    ଯଦି (*varname == '&' && htname != 't')
	    {
		buf_T	*save_curbuf = curbuf;

		// Change curbuf so the option is read from the correct buffer.
		ଯଦି (do_change_curbuf)
		    curbuf = buf;

		ଯଦି (varname[1] == NUL)
		{
		    // get all window-local or buffer-local options in a dict
		    dict_T	*opts = get_winbuf_options(htname == 'b');

		    ଯଦି (opts != ଖାଲି)
		    {
			rettv_dict_set(rettv, opts);
			done = TRUE;
		    }
		}
		ଅନ୍ୟଥା ଯଦି (eval_option(&varname, rettv, TRUE) == OK)
		    // Local option
		    done = TRUE;

		curbuf = save_curbuf;
	    }
	    ଅନ୍ୟଥା ଯଦି (*varname == NUL)
	    {
		// Empty string: return a dict with all the local variables.
		ଯଦି (htname == 'b')
		    v = &buf->b_bufvar;
		ଅନ୍ୟଥା ଯଦି (htname == 'w')
		    v = &win->w_winvar;
		ଅନ୍ୟଥା
		    v = &tp->tp_winvar;
		copy_tv(&v->di_tv, rettv);
		done = TRUE;
	    }
	    ଅନ୍ୟଥା
	    {
		hashtab_T	*ht;

		ଯଦି (htname == 'b')
		    ht = &buf->b_vars->dv_hashtab;
		ଅନ୍ୟଥା ଯଦି (htname == 'w')
		    ht = &win->w_vars->dv_hashtab;
		ଅନ୍ୟଥା
		    ht = &tp->tp_vars->dv_hashtab;

		// Look up the variable.
		v = find_var_in_ht(ht, htname, varname, FALSE);
		ଯଦି (v != ଖାଲି)
		{
		    copy_tv(&v->di_tv, rettv);
		    done = TRUE;
		}
	    }
	}

	ଯଦି (need_switch_win)
	    // restore previous notion of curwin
	    restore_win(&switchwin, TRUE);
    }

    ଯଦି (!done && deftv->v_type != VAR_UNKNOWN)
	// use the default value
	copy_tv(deftv, rettv);

    --emsg_off;
}

/*
 * getwinvar() and gettabwinvar()
 */
    ସ୍ଥିର ଶୂନ୍ୟ
getwinvar(
    typval_T	*argvars,
    typval_T	*rettv,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		off)	    // 1 for gettabwinvar()
{
    char_u	*varname;
    tabpage_T	*tp;
    win_T	*win;

    ଯଦି (off == 1)
	tp = find_tabpage((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା_chk(&argvars[0], ଖାଲି));
    ଅନ୍ୟଥା
	tp = curtab;
    win = find_win_by_nr(&argvars[off], tp);
    varname = tv_get_string_chk(&argvars[off + 1]);

    get_var_from(varname, rettv, &argvars[off + 2], 'w', tp, win, ଖାଲି);
}

/*
 * Set option "varname" to the value of "varp" for the current buffer/window.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
set_option_from_tv(char_u *varname, typval_T *varp)
{
    long	numval = 0;
    char_u	*strval;
    char_u	nbuf[NUMBUFLEN];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		error = FALSE;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_idx = findoption(varname);
    ଯଦି (opt_idx < 0)
    {
	semsg(_(e_unknown_option_str_2), varname);
	ଫେରନ୍ତୁ;
    }
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_p_flags = get_option_flags(opt_idx);

    ଯଦି (varp->v_type == VAR_BOOL)
    {
	ଯଦି (opt_p_flags & P_STRING)
	{
	    emsg(_(e_string_required));
	    ଫେରନ୍ତୁ;
	}
	numval = (long)varp->vval.v_ସଂଖ୍ୟା;
	strval = (char_u *)"0";  // avoid using "false"
    }
    ଅନ୍ୟଥା
    {
	ଯଦି ((opt_p_flags & (P_ଅଙ୍କ|P_BOOL))
		&& (!in_vim9script() || varp->v_type != VAR_STRING))
	    numval = (long)tv_get_ସଂଖ୍ୟା_chk(varp, &error);
	ଯଦି (!error)
	    strval = tv_get_string_buf_chk(varp, nbuf);
    }
    ଯଦି (!error && strval != ଖାଲି)
	set_option_ମୂଲ୍ୟ_give_err(varname, numval, strval, OPT_LOCAL);
}

/*
 * "setwinvar()" and "settabwinvar()" functions
 */
    ସ୍ଥିର ଶୂନ୍ୟ
setwinvar(typval_T *argvars, ପୂର୍ଣ୍ଣସଂଖ୍ୟା off)
{
    win_T	*win;
    switchwin_T	switchwin;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		need_switch_win;
    char_u	*varname, *winvarname;
    typval_T	*varp;
    tabpage_T	*tp = ଖାଲି;

    ଯଦି (check_secure())
	ଫେରନ୍ତୁ;

    ଯଦି (off == 1)
	tp = find_tabpage((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା_chk(&argvars[0], ଖାଲି));
    ଅନ୍ୟଥା
	tp = curtab;
    win = find_win_by_nr(&argvars[off], tp);
    varname = tv_get_string_chk(&argvars[off + 1]);
    varp = &argvars[off + 2];

    ଯଦି (win == ଖାଲି || varname == ଖାଲି)
	ଫେରନ୍ତୁ;

    need_switch_win = !(tp == curtab && win == curwin);
    ଯଦି (!need_switch_win
	    || switch_win(&switchwin, win, tp, TRUE) == OK)
    {
	ଯଦି (*varname == '&')
	    set_option_from_tv(varname + 1, varp);
	ଅନ୍ୟଥା
	{
	    winvarname = alloc(STRLEN(varname) + 3);
	    ଯଦି (winvarname != ଖାଲି)
	    {
		STRCPY(winvarname, "w:");
		STRCPY(winvarname + 2, varname);
		set_var(winvarname, varp, TRUE);
		vim_free(winvarname);
	    }
	}
    }
    ଯଦି (need_switch_win)
	restore_win(&switchwin, TRUE);
}

/*
 * reset v:option_new, v:option_old, v:option_oldlocal, v:option_oldglobal,
 * v:option_type, and v:option_command.
 */
    ଶୂନ୍ୟ
reset_v_option_vars(ଶୂନ୍ୟ)
{
    set_vim_var_string(VV_OPTION_NEW, ଖାଲି, -1);
    set_vim_var_string(VV_OPTION_OLD, ଖାଲି, -1);
    set_vim_var_string(VV_OPTION_OLDLOCAL, ଖାଲି, -1);
    set_vim_var_string(VV_OPTION_OLDGLOBAL, ଖାଲି, -1);
    set_vim_var_string(VV_OPTION_TYPE, ଖାଲି, -1);
    set_vim_var_string(VV_OPTION_COMMAND, ଖାଲି, -1);
}

/*
 * Add an assert error to v:errors.
 */
    ଶୂନ୍ୟ
assert_error(garray_T *gap)
{
    ସଂରଚନା vimvar   *vp = &vimvars[VV_ERRORS];

    ଯଦି (vp->vv_tv_type != VAR_LIST || vimvars[VV_ERRORS].vv_list == ଖାଲି)
	// Make sure v:errors is a list.
	set_vim_var_list(VV_ERRORS, list_alloc());
    list_append_string(vimvars[VV_ERRORS].vv_list, gap->ga_data, gap->ga_len);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
var_exists(char_u *var)
{
    char_u	*arg = var;
    char_u	*name;
    char_u	*tofree;
    typval_T    tv;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n = FALSE;

    // get_name_len() takes care of expanding curly braces
    name = var;
    len = get_name_len(&arg, &tofree, TRUE, FALSE);
    ଯଦି (len > 0)
    {
	ଯଦି (tofree != ଖାଲି)
	    name = tofree;
	n = (eval_variable(name, len, 0, &tv, ଖାଲି,
				 EVAL_VAR_NOAUTOLOAD + EVAL_VAR_IMPORT) == OK);
	ଯଦି (n)
	{
	    // handle d.key, l[idx], f(expr)
	    arg = skipwhite(arg);
	    n = (handle_subscript(&arg, name, &tv, &EVALARG_EVALUATE,
								 FALSE) == OK);
	    ଯଦି (n)
		clear_tv(&tv);
	}
    }
    ଯଦି (*arg != NUL)
	n = FALSE;

    vim_free(tofree);
    ଫେରନ୍ତୁ n;
}

ସ୍ଥିର lval_T	*redir_lval = ଖାଲି;
#define EVALCMD_BUSY (redir_lval == (lval_T *)&redir_lval)
ସ୍ଥିର garray_T redir_ga;	// only valid when redir_lval is not NULL
ସ୍ଥିର char_u	*redir_endp = ଖାଲି;
ସ୍ଥିର char_u	*redir_varname = ଖାଲି;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
alloc_redir_lval(ଶୂନ୍ୟ)
{
    redir_lval = ALLOC_CLEAR_ONE(lval_T);
    ଯଦି (redir_lval == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    ଫେରନ୍ତୁ OK;
}

    ଶୂନ୍ୟ
clear_redir_lval(ଶୂନ୍ୟ)
{
    VIM_CLEAR(redir_lval);
}

    ଶୂନ୍ୟ
init_redir_ga(ଶୂନ୍ୟ)
{
    ga_init2(&redir_ga, ଆକାର(ଅକ୍ଷର), 500);
}

/*
 * Start recording command output to a variable
 * When "append" is TRUE append to an existing variable.
 * Returns OK if successfully completed the setup.  FAIL otherwise.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
var_redir_start(char_u *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା append)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		called_emsg_before;
    typval_T	tv;

    // Catch a bad name early.
    ଯଦି (!eval_isnamec1(*name))
    {
	emsg(_(e_invalid_argument));
	ଫେରନ୍ତୁ FAIL;
    }

    // Make a copy of the name, it is used in redir_lval until redir ends.
    redir_varname = vim_strsave(name);
    ଯଦି (redir_varname == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (alloc_redir_lval() == FAIL)
    {
	var_redir_stop();
	ଫେରନ୍ତୁ FAIL;
    }

    // The output is stored in growarray "redir_ga" until redirection ends.
    init_redir_ga();

    // Parse the variable name (can be a dict or list entry).
    redir_endp = get_lval(redir_varname, ଖାଲି, redir_lval, FALSE, FALSE, 0,
							     FNE_CHECK_START);
    ଯଦି (redir_endp == ଖାଲି || redir_lval->ll_name == ଖାଲି || *redir_endp != NUL)
    {
	clear_lval(redir_lval);
	ଯଦି (redir_endp != ଖାଲି && *redir_endp != NUL)
	    // Trailing characters are present after the variable name
	    semsg(_(e_trailing_characters_str), redir_endp);
	ଅନ୍ୟଥା
	    semsg(_(e_invalid_argument_str), name);
	redir_endp = ଖାଲି;  // don't store a value, only cleanup
	var_redir_stop();
	ଫେରନ୍ତୁ FAIL;
    }

    // check if we can write to the variable: set it to or append an empty
    // string
    called_emsg_before = called_emsg;
    tv.v_type = VAR_STRING;
    tv.vval.v_string = (char_u *)"";
    ଯଦି (append)
	set_var_lval(redir_lval, redir_endp, &tv, TRUE,
					     ASSIGN_NO_DECL, (char_u *)".", 0);
    ଅନ୍ୟଥା
	set_var_lval(redir_lval, redir_endp, &tv, TRUE,
					     ASSIGN_NO_DECL, (char_u *)"=", 0);
    clear_lval(redir_lval);
    ଯଦି (called_emsg > called_emsg_before)
    {
	redir_endp = ଖାଲି;  // don't store a value, only cleanup
	var_redir_stop();
	ଫେରନ୍ତୁ FAIL;
    }

    ଫେରନ୍ତୁ OK;
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
    ଶୂନ୍ୟ
var_redir_str(char_u *ମୂଲ୍ୟ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଲ୍ୟ_len)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;

    ଯଦି (redir_lval == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (ମୂଲ୍ୟ_len == -1)
	len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(ମୂଲ୍ୟ);	// Append the entire string
    ଅନ୍ୟଥା
	len = ମୂଲ୍ୟ_len;		// Append only "value_len" characters

    ଯଦି (ga_grow(&redir_ga, len) == OK)
    {
	mch_memmove((ଅକ୍ଷର *)redir_ga.ga_data + redir_ga.ga_len, ମୂଲ୍ୟ, len);
	redir_ga.ga_len += len;
    }
    ଅନ୍ୟଥା
	var_redir_stop();
}

/*
 * Stop redirecting command output to a variable.
 * Frees the allocated memory.
 */
    ଶୂନ୍ୟ
var_redir_stop(ଶୂନ୍ୟ)
{
    typval_T	tv;

    ଯଦି (EVALCMD_BUSY)
    {
	redir_lval = ଖାଲି;
	ଫେରନ୍ତୁ;
    }

    ଯଦି (redir_lval != ଖାଲି)
    {
	// If there was no error: assign the text to the variable.
	ଯଦି (redir_endp != ଖାଲି)
	{
	    ga_append(&redir_ga, NUL);  // Append the trailing NUL.
	    tv.v_type = VAR_STRING;
	    tv.vval.v_string = redir_ga.ga_data;
	    // Call get_lval() again, if it's inside a Dict or List it may
	    // have changed.
	    redir_endp = get_lval(redir_varname, ଖାଲି, redir_lval,
					FALSE, FALSE, 0, FNE_CHECK_START);
	    ଯଦି (redir_endp != ଖାଲି && redir_lval->ll_name != ଖାଲି)
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
get_clear_redir_ga(ଶୂନ୍ୟ)
{
    char_u *res;

    ga_append(&redir_ga, NUL);  // Append the trailing NUL.
    res = redir_ga.ga_data;
    redir_ga.ga_data = ଖାଲି;
    ଫେରନ୍ତୁ res;
}

/*
 * "gettabvar()" function
 */
    ଶୂନ୍ୟ
f_gettabvar(typval_T *argvars, typval_T *rettv)
{
    char_u	*varname;
    tabpage_T	*tp;
    win_T	*win = ଖାଲି;

    ଯଦି (in_vim9script()
	    && (check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    varname = tv_get_string_chk(&argvars[1]);
    tp = find_tabpage((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା_chk(&argvars[0], ଖାଲି));
    ଯଦି (tp != ଖାଲି)
	win = tp == curtab || tp->tp_firstwin == ଖାଲି ? firstwin
		: tp->tp_firstwin;

    get_var_from(varname, rettv, &argvars[2], 't', tp, win, ଖାଲି);
}

/*
 * "gettabwinvar()" function
 */
    ଶୂନ୍ୟ
f_gettabwinvar(typval_T *argvars, typval_T *rettv)
{
    ଯଦି (in_vim9script()
	    && (check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL
		|| check_for_ସଂଖ୍ୟା_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL))
	ଫେରନ୍ତୁ;

    getwinvar(argvars, rettv, 1);
}

/*
 * "getwinvar()" function
 */
    ଶୂନ୍ୟ
f_getwinvar(typval_T *argvars, typval_T *rettv)
{
    ଯଦି (in_vim9script()
	    && (check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    getwinvar(argvars, rettv, 0);
}

/*
 * "getbufvar()" function
 */
    ଶୂନ୍ୟ
f_getbufvar(typval_T *argvars, typval_T *rettv)
{
    char_u	*varname;
    buf_T	*buf;

    ଯଦି (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    varname = tv_get_string_chk(&argvars[1]);
    buf = tv_get_buf_from_arg(&argvars[0]);

    get_var_from(varname, rettv, &argvars[2], 'b', curtab, curwin, buf);
}

/*
 * "settabvar()" function
 */
    ଶୂନ୍ୟ
f_settabvar(typval_T *argvars, typval_T *rettv UNUSED)
{
    tabpage_T	*save_curtab;
    tabpage_T	*tp;
    tabpage_T	*save_lu_tp;
    char_u	*varname, *tabvarname;
    typval_T	*varp;

    ଯଦି (check_secure())
	ଫେରନ୍ତୁ;

    ଯଦି (in_vim9script()
	    && (check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    tp = find_tabpage((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା_chk(&argvars[0], ଖାଲି));
    varname = tv_get_string_chk(&argvars[1]);
    varp = &argvars[2];

    ଯଦି (varname == ଖାଲି || tp == ଖାଲି)
	ଫେରନ୍ତୁ;

    save_curtab = curtab;
    save_lu_tp = lastused_tabpage;
    goto_tabpage_tp(tp, FALSE, FALSE);

    tabvarname = alloc(STRLEN(varname) + 3);
    ଯଦି (tabvarname != ଖାଲି)
    {
	STRCPY(tabvarname, "t:");
	STRCPY(tabvarname + 2, varname);
	set_var(tabvarname, varp, TRUE);
	vim_free(tabvarname);
    }

    // Restore current tabpage and last accessed tabpage.
    ଯଦି (valid_tabpage(save_curtab))
    {
	goto_tabpage_tp(save_curtab, FALSE, FALSE);
	ଯଦି (valid_tabpage(save_lu_tp))
	    lastused_tabpage = save_lu_tp;
    }
}

/*
 * "settabwinvar()" function
 */
    ଶୂନ୍ୟ
f_settabwinvar(typval_T *argvars, typval_T *rettv UNUSED)
{
    ଯଦି (in_vim9script()
	    && (check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL
		|| check_for_ସଂଖ୍ୟା_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL))
	ଫେରନ୍ତୁ;

    setwinvar(argvars, 1);
}

/*
 * "setwinvar()" function
 */
    ଶୂନ୍ୟ
f_setwinvar(typval_T *argvars, typval_T *rettv UNUSED)
{
    ଯଦି (in_vim9script()
	    && (check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    setwinvar(argvars, 0);
}

/*
 * "setbufvar()" function
 */
    ଶୂନ୍ୟ
f_setbufvar(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    char_u	*varname, *bufvarname;
    typval_T	*varp;

    ଯଦି (check_secure())
	ଫେରନ୍ତୁ;

    ଯଦି (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    varname = tv_get_string_chk(&argvars[1]);
    buf = tv_get_buf_from_arg(&argvars[0]);
    varp = &argvars[2];

    ଯଦି (buf == ଖାଲି || varname == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (*varname == '&')
    {
	aco_save_T	aco;
	// safe the current window position, it could
	// change because of 'scrollbind' window-local
	// options
	linenr_T	old_topline = curwin->w_topline;

	// Set curbuf to be our buf, temporarily.
	aucmd_prepbuf(&aco, buf);
	ଯଦି (curbuf == buf)
	{
	    // Only when it worked to set "curbuf".
	    set_option_from_tv(varname + 1, varp);

	    // reset notion of buffer
	    aucmd_restbuf(&aco);
	}
	curwin->w_topline = old_topline;
    }
    ଅନ୍ୟଥା
    {
	bufvarname = alloc(STRLEN(varname) + 3);
	ଯଦି (bufvarname != ଖାଲି)
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		r = OK;

    CLEAR_FIELD(res);
    ଯଦି (arg->v_type == VAR_PARTIAL && arg->vval.v_partial != ଖାଲି)
    {
	res.cb_partial = arg->vval.v_partial;
	++res.cb_partial->pt_refcount;
	res.cb_name = partial_name(res.cb_partial);
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (arg->v_type == VAR_STRING && arg->vval.v_string != ଖାଲି
					       && SAFE_isdigit(*arg->vval.v_string))
	    r = FAIL;
	ଅନ୍ୟଥା ଯଦି (arg->v_type == VAR_FUNC || arg->v_type == VAR_STRING)
	{
	    res.cb_name = arg->vval.v_string;
	    ଯଦି (arg->v_type == VAR_STRING)
	    {
		char_u *name = get_scriptlocal_funcname(arg->vval.v_string);
		ଯଦି (name != ଖାଲି)
		{
		    res.cb_name = name;
		    res.cb_free_name = TRUE;
		}
	    }
	    func_ref(res.cb_name);
	}
	ଅନ୍ୟଥା ଯଦି (arg->v_type == VAR_ସଂଖ୍ୟା && arg->vval.v_ସଂଖ୍ୟା == 0)
	    res.cb_name = (char_u *)"";
	ଅନ୍ୟଥା
	    r = FAIL;

	ଯଦି (r == FAIL)
	{
	    emsg(_(e_invalid_callback_argument));
	    res.cb_name = ଖାଲି;
	}
    }
    ଫେରନ୍ତୁ res;
}

/*
 * Copy a callback into a typval_T.
 */
    ଶୂନ୍ୟ
put_callback(callback_T *cb, typval_T *tv)
{
    ଯଦି (cb->cb_partial != ଖାଲି)
    {
	tv->v_type = VAR_PARTIAL;
	tv->vval.v_partial = cb->cb_partial;
	++tv->vval.v_partial->pt_refcount;
    }
    ଅନ୍ୟଥା
    {
	tv->v_type = VAR_FUNC;
	tv->vval.v_string = vim_strsave(cb->cb_name);
	func_ref(cb->cb_name);
    }
}

    ସ୍ଥିର bool
does_callback_own_cb_name(callback_T *cb)
{
    // If cb_partial != NULL then *cb->cb_name is owned by the partial.
    ଫେରନ୍ତୁ cb->cb_partial || cb->cb_free_name;
}

/*
 * Make a copy of "src" into "dest", allocating the function name if needed,
 * without incrementing the refcount.
 */
    ଶୂନ୍ୟ
set_callback(callback_T *dest, callback_T *src)
{
    *dest = *src;
    ଯଦି (!does_callback_own_cb_name(src))
    {
	dest->cb_name = vim_strsave(src->cb_name);
	dest->cb_free_name = TRUE;
    }
    *src = (callback_T){0};
}

/*
 * Copy callback from "src" to "dest", incrementing the refcounts.
 */
    ଶୂନ୍ୟ
copy_callback(callback_T *dest, callback_T *src)
{
    dest->cb_partial = src->cb_partial;
    ଯଦି (dest->cb_partial != ଖାଲି)
    {
	dest->cb_name = src->cb_name;
	dest->cb_free_name = FALSE;
	++dest->cb_partial->pt_refcount;
    }
    ଅନ୍ୟଥା
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
    ଶୂନ୍ୟ
expand_autload_callback(callback_T *cb)
{
    char_u	*name;
    char_u	*p;
    imported_T	*import;

    ଯଦି (!in_vim9script() || cb->cb_name == ଖାଲି
	    || (!cb->cb_free_name
	       && (cb->cb_partial == ଖାଲି || cb->cb_partial->pt_name == ଖାଲି)))
	ଫେରନ୍ତୁ;
    ଯଦି (cb->cb_partial != ଖାଲି)
	name = cb->cb_partial->pt_name;
    ଅନ୍ୟଥା
	name = cb->cb_name;
    p = vim_strchr(name, '.');
    ଯଦି (p == ଖାଲି)
	ଫେରନ୍ତୁ;

    import = find_imported(name, p - name, FALSE);
    ଯଦି (import == ଖାଲି || !SCRIPT_ID_VALID(import->imp_sid))
	ଫେରନ୍ତୁ;

    scriptitem_T *si = SCRIPT_ITEM(import->imp_sid);
    ଯଦି (si->sn_autoload_prefix == ଖାଲି)
	ଫେରନ୍ତୁ;

    char_u *newname = concat_str(si->sn_autoload_prefix, p + 1);
    ଯଦି (newname == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (cb->cb_partial != ଖାଲି)
    {
	ଯଦି (cb->cb_name == cb->cb_partial->pt_name)
	    cb->cb_name = newname;
	vim_free(cb->cb_partial->pt_name);
	cb->cb_partial->pt_name = newname;
    }
    ଅନ୍ୟଥା
    {
	vim_free(cb->cb_name);
	cb->cb_name = newname;
    }
}

/*
 * Unref/free "callback" returned by get_callback() or set_callback().
 */
    ଶୂନ୍ୟ
free_callback(callback_T *callback)
{
    ଯଦି (callback->cb_partial != ଖାଲି)
    {
	partial_unref(callback->cb_partial);
	callback->cb_partial = ଖାଲି;
    }
    ଅନ୍ୟଥା ଯଦି (callback->cb_name != ଖାଲି)
	func_unref(callback->cb_name);
    ଯଦି (callback->cb_free_name)
    {
	vim_free(callback->cb_name);
	callback->cb_free_name = FALSE;
    }
    callback->cb_name = ଖାଲି;
}

#endif // FEAT_EVAL
