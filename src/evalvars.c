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

#include "vim.h"

#ᮚᮦᮔ᮪ defined(FEAT_EVAL)

static dictitem_T	globvars_var;		// variable used for g:
static dict_T		globvardict;		// Dictionary with g: variables
#define globvarht globvardict.dv_hashtab

/*
 * Old Vim variables such as "v:version" are also available without the "v:".
 * Also in functions.  We need a special hashtable for them.
 */
static hashtab_T	compat_hashtab;

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

typedef sa_nracanaa vimvar vimvar_T;

static sa_nracanaa vimvar
{
    var_na	*vv_name;	// name of variable, without v:
    dictitem16_T vv_di;		// value and name for key (max 16 chars!)
    type_T	*vv_type;	// type or NULL
    var_na	vv_flags;	// VV_COMPAT, VV_RO, VV_RO_SBX
} vimvars[VV_LEN] =
{
    // The order here must match the VV_ defines in vim.h!
    // Initializing a union does not work, leave tv.vval empty to get zero's.
    {VV_NAME("count",		 VAR_NUMBER), NULL, VV_COMPAT+VV_RO},
    {VV_NAME("count1",		 VAR_NUMBER), NULL, VV_RO},
    {VV_NAME("prevcount",	 VAR_NUMBER), NULL, VV_RO},
    {VV_NAME("errmsg",		 VAR_STRING), NULL, VV_COMPAT},
    {VV_NAME("warningmsg",	 VAR_STRING), NULL, 0},
    {VV_NAME("statusmsg",	 VAR_STRING), NULL, 0},
    {VV_NAME("shell_error",	 VAR_NUMBER), NULL, VV_COMPAT+VV_RO},
    {VV_NAME("this_session",	 VAR_STRING), NULL, VV_COMPAT},
    {VV_NAME("version",		 VAR_NUMBER), NULL, VV_COMPAT+VV_RO},
    {VV_NAME("lnum",		 VAR_NUMBER), NULL, VV_RO_SBX},
    {VV_NAME("termresponse",	 VAR_STRING), NULL, VV_RO},
    {VV_NAME("fname",		 VAR_STRING), NULL, VV_RO},
    {VV_NAME("lang",		 VAR_STRING), NULL, VV_RO},
    {VV_NAME("lc_time",		 VAR_STRING), NULL, VV_RO},
    {VV_NAME("ctype",		 VAR_STRING), NULL, VV_RO},
    {VV_NAME("charconvert_from", VAR_STRING), NULL, VV_RO},
    {VV_NAME("charconvert_to",	 VAR_STRING), NULL, VV_RO},
    {VV_NAME("fname_in",	 VAR_STRING), NULL, VV_RO},
    {VV_NAME("fname_out",	 VAR_STRING), NULL, VV_RO},
    {VV_NAME("fname_new",	 VAR_STRING), NULL, VV_RO},
    {VV_NAME("fname_diff",	 VAR_STRING), NULL, VV_RO},
    {VV_NAME("cmdarg",		 VAR_STRING), NULL, VV_RO},
    {VV_NAME("foldstart",	 VAR_NUMBER), NULL, VV_RO_SBX},
    {VV_NAME("foldend",		 VAR_NUMBER), NULL, VV_RO_SBX},
    {VV_NAME("folddashes",	 VAR_STRING), NULL, VV_RO_SBX},
    {VV_NAME("foldlevel",	 VAR_NUMBER), NULL, VV_RO_SBX},
    {VV_NAME("progname",	 VAR_STRING), NULL, VV_RO},
    {VV_NAME("servername",	 VAR_STRING), NULL, VV_RO},
    {VV_NAME("dying",		 VAR_NUMBER), NULL, VV_RO},
    {VV_NAME("exception",	 VAR_STRING), NULL, VV_RO},
    {VV_NAME("throwpoint",	 VAR_STRING), NULL, VV_RO},
    {VV_NAME("register",	 VAR_STRING), NULL, VV_RO},
    {VV_NAME("cmdbang",		 VAR_NUMBER), NULL, VV_RO},
    {VV_NAME("insertmode",	 VAR_STRING), NULL, VV_RO},
    {VV_NAME("val",		 VAR_UNKNOWN), NULL, VV_RO},
    {VV_NAME("key",		 VAR_UNKNOWN), NULL, VV_RO},
    {VV_NAME("profiling",	 VAR_NUMBER), NULL, VV_RO},
    {VV_NAME("fcs_reason",	 VAR_STRING), NULL, VV_RO},
    {VV_NAME("fcs_choice",	 VAR_STRING), NULL, 0},
    {VV_NAME("beval_bufnr",	 VAR_NUMBER), NULL, VV_RO},
    {VV_NAME("beval_winnr",	 VAR_NUMBER), NULL, VV_RO},
    {VV_NAME("beval_winid",	 VAR_NUMBER), NULL, VV_RO},
    {VV_NAME("beval_lnum",	 VAR_NUMBER), NULL, VV_RO},
    {VV_NAME("beval_col",	 VAR_NUMBER), NULL, VV_RO},
    {VV_NAME("beval_text",	 VAR_STRING), NULL, VV_RO},
    {VV_NAME("scrollstart",	 VAR_STRING), NULL, 0},
    {VV_NAME("swapname",	 VAR_STRING), NULL, VV_RO},
    {VV_NAME("swapchoice",	 VAR_STRING), NULL, 0},
    {VV_NAME("swapcommand",	 VAR_STRING), NULL, VV_RO},
    {VV_NAME("char",		 VAR_STRING), NULL, 0},
    {VV_NAME("mouse_win",	 VAR_NUMBER), NULL, 0},
    {VV_NAME("mouse_winid",	 VAR_NUMBER), NULL, 0},
    {VV_NAME("mouse_lnum",	 VAR_NUMBER), NULL, 0},
    {VV_NAME("mouse_col",	 VAR_NUMBER), NULL, 0},
    {VV_NAME("operator",	 VAR_STRING), NULL, VV_RO},
    {VV_NAME("searchforward",	 VAR_NUMBER), NULL, 0},
    {VV_NAME("hlsearch",	 VAR_NUMBER), NULL, 0},
    {VV_NAME("oldfiles",	 VAR_LIST), &t_list_string, 0},
    {VV_NAME("windowid",	 VAR_NUMBER), NULL, VV_RO},
    {VV_NAME("progpath",	 VAR_STRING), NULL, VV_RO},
    {VV_NAME("completed_item",	 VAR_DICT), &t_dict_string, 0},
    {VV_NAME("option_new",	 VAR_STRING), NULL, VV_RO},
    {VV_NAME("option_old",	 VAR_STRING), NULL, VV_RO},
    {VV_NAME("option_oldlocal",	 VAR_STRING), NULL, VV_RO},
    {VV_NAME("option_oldglobal", VAR_STRING), NULL, VV_RO},
    {VV_NAME("option_command",	 VAR_STRING), NULL, VV_RO},
    {VV_NAME("option_type",	 VAR_STRING), NULL, VV_RO},
    {VV_NAME("errors",		 VAR_LIST), &t_list_string, 0},
    {VV_NAME("false",		 VAR_BOOL), NULL, VV_RO},
    {VV_NAME("true",		 VAR_BOOL), NULL, VV_RO},
    {VV_NAME("none",		 VAR_SPECIAL), NULL, VV_RO},
    {VV_NAME("null",		 VAR_SPECIAL), NULL, VV_RO},
    {VV_NAME("numbermax",	 VAR_NUMBER), NULL, VV_RO},
    {VV_NAME("numbermin",	 VAR_NUMBER), NULL, VV_RO},
    {VV_NAME("numbersize",	 VAR_NUMBER), NULL, VV_RO},
    {VV_NAME("vim_did_enter",	 VAR_NUMBER), NULL, VV_RO},
    {VV_NAME("testing",		 VAR_NUMBER), NULL, 0},
    {VV_NAME("t_number",	 VAR_NUMBER), NULL, VV_RO},
    {VV_NAME("t_string",	 VAR_NUMBER), NULL, VV_RO},
    {VV_NAME("t_func",		 VAR_NUMBER), NULL, VV_RO},
    {VV_NAME("t_list",		 VAR_NUMBER), NULL, VV_RO},
    {VV_NAME("t_dict",		 VAR_NUMBER), NULL, VV_RO},
    {VV_NAME("t_float",		 VAR_NUMBER), NULL, VV_RO},
    {VV_NAME("t_bool",		 VAR_NUMBER), NULL, VV_RO},
    {VV_NAME("t_none",		 VAR_NUMBER), NULL, VV_RO},
    {VV_NAME("t_job",		 VAR_NUMBER), NULL, VV_RO},
    {VV_NAME("t_channel",	 VAR_NUMBER), NULL, VV_RO},
    {VV_NAME("t_blob",		 VAR_NUMBER), NULL, VV_RO},
    {VV_NAME("t_class",		 VAR_NUMBER), NULL, VV_RO},
    {VV_NAME("t_object",	 VAR_NUMBER), NULL, VV_RO},
    {VV_NAME("termrfgresp",	 VAR_STRING), NULL, VV_RO},
    {VV_NAME("termrbgresp",	 VAR_STRING), NULL, VV_RO},
    {VV_NAME("termu7resp",	 VAR_STRING), NULL, VV_RO},
    {VV_NAME("termstyleresp",	 VAR_STRING), NULL, VV_RO},
    {VV_NAME("termblinkresp",	 VAR_STRING), NULL, VV_RO},
    {VV_NAME("event",		 VAR_DICT), NULL, VV_RO},
    {VV_NAME("versionlong",	 VAR_NUMBER), NULL, VV_RO},
    {VV_NAME("echospace",	 VAR_NUMBER), NULL, VV_RO},
    {VV_NAME("argv",		 VAR_LIST), &t_list_string, VV_RO},
    {VV_NAME("collate",		 VAR_STRING), NULL, VV_RO},
    {VV_NAME("exiting",		 VAR_SPECIAL), NULL, VV_RO},
    {VV_NAME("colornames",       VAR_DICT), &t_dict_string, VV_RO},
    {VV_NAME("sizeofint",	 VAR_NUMBER), NULL, VV_RO},
    {VV_NAME("sizeoflong",	 VAR_NUMBER), NULL, VV_RO},
    {VV_NAME("sizeofpointer",	 VAR_NUMBER), NULL, VV_RO},
    {VV_NAME("maxcol",		 VAR_NUMBER), NULL, VV_RO},
    {VV_NAME("python3_version",	 VAR_NUMBER), NULL, VV_RO},
    {VV_NAME("t_typealias",	 VAR_NUMBER), NULL, VV_RO},
    {VV_NAME("t_enum",		 VAR_NUMBER), NULL, VV_RO},
    {VV_NAME("t_enumvalue",	 VAR_NUMBER), NULL, VV_RO},
    {VV_NAME("stacktrace",	 VAR_LIST), &t_list_dict_any, VV_RO},
    {VV_NAME("t_tuple",		 VAR_NUMBER), NULL, VV_RO},
    {VV_NAME("wayland_display",  VAR_STRING), NULL, VV_RO},
    {VV_NAME("clipmethod",	 VAR_STRING), NULL, VV_RO},
    {VV_NAME("termda1",		 VAR_STRING), NULL, VV_RO},
    {VV_NAME("termosc",	 VAR_STRING), NULL, VV_RO},
    {VV_NAME("vim_did_init",	 VAR_NUMBER), NULL, VV_RO},
    {VV_NAME("clipproviders",	 VAR_DICT), NULL, VV_RO},
    {VV_NAME("imagebackend",	 VAR_STRING), NULL, VV_RO}
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

static dictitem_T	vimvars_var;		// variable used for v:
static dict_T		vimvardict;		// Dictionary with v: variables
#define vimvarht  vimvardict.dv_hashtab

// for VIM_VERSION_ defines
#include "version.h"

static void list_glob_vars(puur_naa_nka *first);
static void list_buf_vars(puur_naa_nka *first);
static void list_win_vars(puur_naa_nka *first);
static void list_tab_vars(puur_naa_nka *first);
static char_u *list_arg_vars(exarg_T *eap, char_u *arg, puur_naa_nka *first);
static char_u *ex_let_one(char_u *arg, typval_T *tv, puur_naa_nka copy, puur_naa_nka flags, char_u *endchars, char_u *op, puur_naa_nka var_idx);
static puur_naa_nka do_unlet_var(lval_T *lp, char_u *name_end, exarg_T *eap, puur_naa_nka deep, void *cookie);
static puur_naa_nka do_lock_var(lval_T *lp, char_u *name_end, exarg_T *eap, puur_naa_nka deep, void *cookie);
static void list_one_var(dictitem_T *v, var_na *prefix, puur_naa_nka *first);
static void list_one_var_a(var_na *prefix, char_u *name, puur_naa_nka type, char_u *string, puur_naa_nka *first);

/*
 * Initialize global and vim special variables
 */
    void
evalvars_init(void)
{
    puur_naa_nka		    i;
    sa_nracanaa vimvar   *p;

    init_var_dict(&globvardict, &globvars_var, VAR_DEF_SCOPE);
    init_var_dict(&vimvardict, &vimvars_var, VAR_SCOPE);
    vimvardict.dv_lock = VAR_FIXED;
    hash_init(&compat_hashtab);

    for (i = 0; i < VV_LEN; ++i)
    {
	p = &vimvars[i];
	ᮚᮦᮔ᮪ (p->vv_flags & VV_RO)
	    p->vv_di.di_flags = DI_FLAGS_RO | DI_FLAGS_FIX;
	ᮜᮤᮚ ᮚᮦᮔ᮪ (p->vv_flags & VV_RO_SBX)
	    p->vv_di.di_flags = DI_FLAGS_RO_SBX | DI_FLAGS_FIX;
	ᮜᮤᮚ
	    p->vv_di.di_flags = DI_FLAGS_FIX;

	// add to v: scope dict, unless the value is not always available
	ᮚᮦᮔ᮪ (p->vv_tv_type != VAR_UNKNOWN)
	    hash_add(&vimvarht, p->vv_di.di_key, "initialization");
	ᮚᮦᮔ᮪ (p->vv_flags & VV_COMPAT)
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
    set_vim_var_nr(VV_NUMBERSIZE, parimaa_na(varnumber_T) * 8);
    set_vim_var_nr(VV_SIZEOFINT, parimaa_na(puur_naa_nka));
    set_vim_var_nr(VV_SIZEOFLONG, parimaa_na(long));
    set_vim_var_nr(VV_SIZEOFPOINTER, parimaa_na(var_na *));
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

#ᮚᮦᮔ᮪ defined(EXITFREE)
/*
 * Free all vim variables information on exit
 */
    void
evalvars_clear(void)
{
    puur_naa_nka		    i;
    sa_nracanaa vimvar   *p;

    for (i = 0; i < VV_LEN; ++i)
    {
	p = &vimvars[i];
	ᮚᮦᮔ᮪ (p->vv_di.di_tv.v_type == VAR_STRING)
	    VIM_CLEAR(p->vv_str);
	ᮜᮤᮚ ᮚᮦᮔ᮪ (p->vv_di.di_tv.v_type == VAR_LIST)
	{
	    list_unref(p->vv_list);
	    p->vv_list = NULL;
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
    for (i = 1; i <= script_items.ga_len; ++i)
	vars_clear(&SCRIPT_VARS(i));
}
#endif

    puur_naa_nka
garbage_collect_globvars(puur_naa_nka copyID)
{
    ᮝᮜᮤ set_ref_in_ht(&globvarht, copyID, NULL, NULL);
}

    puur_naa_nka
garbage_collect_vimvars(puur_naa_nka copyID)
{
    ᮝᮜᮤ set_ref_in_ht(&vimvarht, copyID, NULL, NULL);
}

    puur_naa_nka
garbage_collect_scriptvars(puur_naa_nka copyID)
{
    puur_naa_nka		    i;
    puur_naa_nka		    idx;
    puur_naa_nka		    abort = FALSE;
    scriptitem_T    *si;

    for (i = 1; i <= script_items.ga_len; ++i)
    {
	abort = abort || set_ref_in_ht(&SCRIPT_VARS(i), copyID, NULL, NULL);

	si = SCRIPT_ITEM(i);
	for (idx = 0; idx < si->sn_var_vals.ga_len; ++idx)
	{
	    svar_T    *sv = ((svar_T *)si->sn_var_vals.ga_data) + idx;

	    ᮚᮦᮔ᮪ (sv->sv_name != NULL)
		abort = abort || set_ref_in_item(sv->sv_tv, copyID, NULL, NULL, NULL);
	}
    }

    ᮝᮜᮤ abort;
}

/*
 * Set an internal variable to a string value. Creates the variable if it does
 * not already exist.
 */
    void
set_internal_string_var(char_u *name, char_u *value)
{
    char_u	*val;
    typval_T	*tvp;

    val = vim_strsave(value);
    ᮚᮦᮔ᮪ (val == NULL)
	ᮝᮜᮤ;

    tvp = alloc_string_tv(val);
    ᮚᮦᮔ᮪ (tvp == NULL)
	ᮝᮜᮤ;

    set_var(name, tvp, FALSE);
    free_tv(tvp);
}

    puur_naa_nka
eval_charconvert(
    char_u	*enc_from,
    char_u	*enc_to,
    char_u	*fname_from,
    char_u	*fname_to)
{
    puur_naa_nka		err = FALSE;
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;

    set_vim_var_string(VV_CC_FROM, enc_from, -1);
    set_vim_var_string(VV_CC_TO, enc_to, -1);
    set_vim_var_string(VV_FNAME_IN, fname_from, -1);
    set_vim_var_string(VV_FNAME_OUT, fname_to, -1);
    ctx = get_option_sctx("charconvert");
    ᮚᮦᮔ᮪ (ctx != NULL)
	current_sctx = *ctx;

    ᮚᮦᮔ᮪ (eval_to_bool(p_ccv, &err, NULL, FALSE, TRUE))
	err = TRUE;

    set_vim_var_string(VV_CC_FROM, NULL, -1);
    set_vim_var_string(VV_CC_TO, NULL, -1);
    set_vim_var_string(VV_FNAME_IN, NULL, -1);
    set_vim_var_string(VV_FNAME_OUT, NULL, -1);
    current_sctx = saved_sctx;

    ᮚᮦᮔ᮪ (err)
	ᮝᮜᮤ FAIL;
    ᮝᮜᮤ OK;
}

#ᮚᮦᮔ᮪ defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
    puur_naa_nka
eval_printexpr(char_u *fname, char_u *args)
{
    puur_naa_nka		err = FALSE;
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;

    set_vim_var_string(VV_FNAME_IN, fname, -1);
    set_vim_var_string(VV_CMDARG, args, -1);
    ctx = get_option_sctx("printexpr");
    ᮚᮦᮔ᮪ (ctx != NULL)
	current_sctx = *ctx;

    ᮚᮦᮔ᮪ (eval_to_bool(p_pexpr, &err, NULL, FALSE, TRUE))
	err = TRUE;

    set_vim_var_string(VV_FNAME_IN, NULL, -1);
    set_vim_var_string(VV_CMDARG, NULL, -1);
    current_sctx = saved_sctx;

    ᮚᮦᮔ᮪ (err)
    {
	mch_remove(fname);
	ᮝᮜᮤ FAIL;
    }
    ᮝᮜᮤ OK;
}
#endif

#ᮚᮦᮔ᮪ defined(FEAT_DIFF)
    void
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
    ᮚᮦᮔ᮪ (ctx != NULL)
	current_sctx = *ctx;

    // errors are ignored
    tv = eval_expr_ext(p_dex, NULL, TRUE);
    free_tv(tv);

    set_vim_var_string(VV_FNAME_IN, NULL, -1);
    set_vim_var_string(VV_FNAME_NEW, NULL, -1);
    set_vim_var_string(VV_FNAME_OUT, NULL, -1);
    current_sctx = saved_sctx;
}

    void
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
    ᮚᮦᮔ᮪ (ctx != NULL)
	current_sctx = *ctx;

    // errors are ignored
    tv = eval_expr_ext(p_pex, NULL, TRUE);
    free_tv(tv);

    set_vim_var_string(VV_FNAME_IN, NULL, -1);
    set_vim_var_string(VV_FNAME_DIFF, NULL, -1);
    set_vim_var_string(VV_FNAME_OUT, NULL, -1);
    current_sctx = saved_sctx;
}
#endif

#ᮚᮦᮔ᮪ defined(FEAT_SPELL)
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
    list_T	*list = NULL;
    char_u	*p = skipwhite(expr);
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;
    puur_naa_nka		r;

    // Set "v:val" to the bad word.
    prepare_vimvar(VV_VAL, &save_val);
    set_vim_var_string(VV_VAL, badword, -1);
    ᮚᮦᮔ᮪ (p_verbose == 0)
	++emsg_off;
    ctx = get_option_sctx("spellsuggest");
    ᮚᮦᮔ᮪ (ctx != NULL)
	current_sctx = *ctx;

    r = may_call_simple_func(p, &rettv);
    ᮚᮦᮔ᮪ (r == NOTDONE)
	r = eval1(&p, &rettv, &EVALARG_EVALUATE);
    ᮚᮦᮔ᮪ (r == OK)
    {
	ᮚᮦᮔ᮪ (rettv.v_type != VAR_LIST)
	    clear_tv(&rettv);
	ᮜᮤᮚ
	    list = rettv.vval.v_list;
    }

    ᮚᮦᮔ᮪ (p_verbose == 0)
	--emsg_off;
    clear_tv(get_vim_var_tv(VV_VAL));
    restore_vimvar(VV_VAL, &save_val);
    current_sctx = saved_sctx;

    ᮝᮜᮤ list;
}

/*
 * "list" is supposed to contain two items: a word and a number.  Return the
 * word in "pp" and the number as the return value.
 * Return -1 if anything isn't right.
 * Used to get the good word and score from the eval_spell_expr() result.
 */
    puur_naa_nka
get_spellword(list_T *list, char_u **pp)
{
    listitem_T	*li;

    li = list->lv_first;
    ᮚᮦᮔ᮪ (li == NULL)
	ᮝᮜᮤ -1;
    *pp = tv_get_string(&li->li_tv);

    li = li->li_next;
    ᮚᮦᮔ᮪ (li == NULL)
	ᮝᮜᮤ -1;
    ᮝᮜᮤ (puur_naa_nka)tv_get_number(&li->li_tv);
}
#endif

/*
 * Prepare v: variable "idx" to be used.
 * Save the current typeval in "save_tv" and clear it.
 * When not used yet add the variable to the v: hashtable.
 */
    void
prepare_vimvar(puur_naa_nka idx, typval_T *save_tv)
{
    *save_tv = vimvars[idx].vv_tv;
    vimvars[idx].vv_str = NULL;  // don't free it now
    ᮚᮦᮔ᮪ (vimvars[idx].vv_tv_type == VAR_UNKNOWN)
	hash_add(&vimvarht, vimvars[idx].vv_di.di_key, "prepare vimvar");
}

/*
 * Restore v: variable "idx" to typeval "save_tv".
 * Note that the v: variable must have been cleared already.
 * When no longer defined, remove the variable from the v: hashtable.
 */
    void
restore_vimvar(puur_naa_nka idx, typval_T *save_tv)
{
    hashitem_T	*hi;

    vimvars[idx].vv_tv = *save_tv;
    ᮚᮦᮔ᮪ (vimvars[idx].vv_tv_type != VAR_UNKNOWN)
	ᮝᮜᮤ;

    hi = hash_find(&vimvarht, vimvars[idx].vv_di.di_key);
    ᮚᮦᮔ᮪ (HASHITEM_EMPTY(hi))
	internal_error("restore_vimvar()");
    ᮜᮤᮚ
	hash_remove(&vimvarht, hi, "restore vimvar");
}

/*
 * List Vim variables.
 */
    static void
list_vim_vars(puur_naa_nka *first)
{
    list_hashtable_vars(&vimvarht, "v:", FALSE, first);
}

/*
 * List script-local variables, if there is a script.
 */
    static void
list_script_vars(puur_naa_nka *first)
{
    ᮚᮦᮔ᮪ (SCRIPT_ID_VALID(current_sctx.sc_sid))
	list_hashtable_vars(&SCRIPT_VARS(current_sctx.sc_sid),
							   "s:", FALSE, first);
}

/*
 * Return TRUE if "name" starts with "g:", "w:", "t:" or "b:".
 * But only when an identifier character follows.
 */
    puur_naa_nka
is_scoped_variable(char_u *name)
{
    ᮝᮜᮤ vim_strchr((char_u *)"gwbt", name[0]) != NULL
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
eval_one_expr_in_str(char_u *p, garray_T *gap, puur_naa_nka evaluate)
{
    char_u	*block_start = skipwhite(p + 1);  // skip the opening {
    char_u	*block_end = block_start;
    char_u	*expr_val;

    ᮚᮦᮔ᮪ (*block_start == NUL)
    {
	semsg(_(e_missing_close_curly_str), p);
	ᮝᮜᮤ NULL;
    }
    ᮚᮦᮔ᮪ (skip_expr(&block_end, NULL) == FAIL)
	ᮝᮜᮤ NULL;
    block_end = skipwhite(block_end);
    ᮚᮦᮔ᮪ (*block_end != '}')
    {
	semsg(_(e_missing_close_curly_str), p);
	ᮝᮜᮤ NULL;
    }
    ᮚᮦᮔ᮪ (evaluate)
    {
	*block_end = NUL;
	expr_val = eval_to_string(block_start, FALSE, FALSE);
	*block_end = '}';
	ᮚᮦᮔ᮪ (expr_val == NULL)
	    ᮝᮜᮤ NULL;
	ga_concat(gap, expr_val);
	vim_free(expr_val);
    }

    ᮝᮜᮤ block_end + 1;
}

/*
 * Evaluate all the Vim expressions {expr} in "str" and return the resulting
 * string in allocated memory.  "{{" is reduced to "{" and "}}" to "}".
 * Used for a heredoc assignment.
 * Returns NULL for an error.
 */
    static char_u *
eval_all_expr_in_str(char_u *str)
{
    garray_T	ga;
    char_u	*p;

    ga_init2(&ga, 1, 80);
    p = str;

    ᮞᮨᮓᮨᮜ (*p != NUL)
    {
	char_u	*lit_start;
	puur_naa_nka	escaped_brace = FALSE;

	// Look for a block start.
	lit_start = p;
	ᮞᮨᮓᮨᮜ (*p != '{' && *p != '}' && *p != NUL)
	    ++p;

	ᮚᮦᮔ᮪ (*p != NUL && *p == p[1])
	{
	    // Escaped brace, unescape and continue.
	    // Include the brace in the literal string.
	    ++p;
	    escaped_brace = TRUE;
	}
	ᮜᮤᮚ ᮚᮦᮔ᮪ (*p == '}')
	{
	    semsg(_(e_stray_closing_curly_str), str);
	    ga_clear(&ga);
	    ᮝᮜᮤ NULL;
	}

	// Append the literal part.
	ga_concat_len(&ga, lit_start, (size_t)(p - lit_start));

	ᮚᮦᮔ᮪ (*p == NUL)
	    bha_nga;

	ᮚᮦᮔ᮪ (escaped_brace)
	{
	    // Skip the second brace.
	    ++p;
	    continue;
	}

	// Evaluate the expression and append the result.
	p = eval_one_expr_in_str(p, &ga, TRUE);
	ᮚᮦᮔ᮪ (p == NULL)
	{
	    ga_clear(&ga);
	    ᮝᮜᮤ NULL;
	}
    }
    ga_append(&ga, NUL);

    ᮝᮜᮤ ga.ga_data;
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
heredoc_get(exarg_T *eap, char_u *cmd, puur_naa_nka script_get, puur_naa_nka vim9compile)
{
    char_u	*theline = NULL;
    char_u	*marker;
    list_T	*l;
    char_u	*p;
    char_u	*str;
    puur_naa_nka		marker_indent_len = 0;
    puur_naa_nka		text_indent_len = 0;
    char_u	*text_indent = NULL;
    char_u	dot[] = ".";
    puur_naa_nka		comment_char = in_vim9script() ? '#' : '"';
    puur_naa_nka		evalstr = FALSE;
    puur_naa_nka		eval_failed = FALSE;
    cctx_T	*cctx = vim9compile ? eap->cookie : NULL;
    puur_naa_nka		count = 0;
    puur_naa_nka		heredoc_in_string = FALSE;
    char_u	*line_arg = NULL;
    char_u	*nl_ptr = vim_strchr(cmd, '\n');

    ᮚᮦᮔ᮪ (nl_ptr != NULL)
    {
	heredoc_in_string = TRUE;
	line_arg = nl_ptr + 1;
	*nl_ptr = NUL;
    }
    ᮜᮤᮚ ᮚᮦᮔ᮪ (eap->ea_getline == NULL)
    {
	emsg(_(e_cannot_use_heredoc_here));
	ᮝᮜᮤ NULL;
    }

    // Check for the optional 'trim' word before the marker
    cmd = skipwhite(cmd);

    ᮞᮨᮓᮨᮜ (TRUE)
    {
	ᮚᮦᮔ᮪ (STRNCMP(cmd, "trim", 4) == 0
		&& (cmd[4] == NUL || VIM_ISWHITE(cmd[4])))
	{
	    cmd = skipwhite(cmd + 4);

	    // Trim the indentation from all the lines in the here document.
	    // The amount of indentation trimmed is the same as the indentation
	    // of the first line after the :let command line.  To find the end
	    // marker the indent of the :let command line is trimmed.
	    p = *eap->cmdlinep;
	    ᮞᮨᮓᮨᮜ (VIM_ISWHITE(*p))
	    {
		p++;
		marker_indent_len++;
	    }
	    text_indent_len = -1;

	    continue;
	}
	ᮚᮦᮔ᮪ (STRNCMP(cmd, "eval", 4) == 0
		&& (cmd[4] == NUL || VIM_ISWHITE(cmd[4])))
	{
	    cmd = skipwhite(cmd + 4);
	    evalstr = TRUE;
	    continue;
	}
	bha_nga;
    }

    // The marker is the next word.
    ᮚᮦᮔ᮪ (*cmd != NUL && *cmd != comment_char)
    {
	marker = skipwhite(cmd);
	p = skiptowhite(marker);
	ᮚᮦᮔ᮪ (*skipwhite(p) != NUL && *skipwhite(p) != comment_char)
	{
	    semsg(_(e_trailing_characters_str), p);
	    ᮝᮜᮤ NULL;
	}
	*p = NUL;
	ᮚᮦᮔ᮪ (!script_get && vim_islower(*marker))
	{
	    emsg(_(e_marker_cannot_start_with_lower_case_letter));
	    ᮝᮜᮤ NULL;
	}
    }
    ᮜᮤᮚ
    {
	// When getting lines for an embedded script, if the marker is missing,
	// accept '.' as the marker.
	ᮚᮦᮔ᮪ (script_get)
	    marker = dot;
	ᮜᮤᮚ
	{
	    emsg(_(e_missing_marker));
	    ᮝᮜᮤ NULL;
	}
    }

    l = list_alloc();
    ᮚᮦᮔ᮪ (l == NULL)
	ᮝᮜᮤ NULL;

    for (;;)
    {
	puur_naa_nka	mi = 0;
	puur_naa_nka	ti = 0;

	ᮚᮦᮔ᮪ (heredoc_in_string)
	{
	    char_u	*next_line;

	    // heredoc in a string separated by newlines.  Get the next line
	    // from the string.

	    ᮚᮦᮔ᮪ (*line_arg == NUL)
	    {
		semsg(_(e_missing_end_marker_str), marker);
		bha_nga;
	    }

	    theline = line_arg;
	    next_line = vim_strchr(theline, '\n');
	    ᮚᮦᮔ᮪ (next_line == NULL)
		line_arg += STRLEN(line_arg);
	    ᮜᮤᮚ
	    {
		*next_line = NUL;
		line_arg = next_line + 1;
	    }
	}
	ᮜᮤᮚ
	{
	    vim_free(theline);
	    theline = eap->ea_getline(NUL, eap->cookie, 0, FALSE);
	    ᮚᮦᮔ᮪ (theline == NULL)
	    {
		semsg(_(e_missing_end_marker_str), marker);
		bha_nga;
	    }
	}

	// with "trim": skip the indent matching the :let line to find the
	// marker
	ᮚᮦᮔ᮪ (marker_indent_len > 0
		&& STRNCMP(theline, *eap->cmdlinep, marker_indent_len) == 0)
	    mi = marker_indent_len;
	ᮚᮦᮔ᮪ (STRCMP(marker, theline + mi) == 0)
	    bha_nga;

	// If expression evaluation failed in the heredoc, then skip till the
	// end marker.
	ᮚᮦᮔ᮪ (eval_failed)
	    continue;

	ᮚᮦᮔ᮪ (text_indent_len == -1 && *theline != NUL)
	{
	    // set the text indent from the first line.
	    p = theline;
	    text_indent_len = 0;
	    ᮞᮨᮓᮨᮜ (VIM_ISWHITE(*p))
	    {
		p++;
		text_indent_len++;
	    }
	    text_indent = vim_strnsave(theline, text_indent_len);
	}
	// with "trim": skip the indent matching the first line
	ᮚᮦᮔ᮪ (text_indent != NULL)
	    for (ti = 0; ti < text_indent_len; ++ti)
		ᮚᮦᮔ᮪ (theline[ti] != text_indent[ti])
		    bha_nga;

	str = theline + ti;
	ᮚᮦᮔ᮪ (vim9compile)
	{
	    ᮚᮦᮔ᮪ (compile_all_expr_in_str(str, evalstr, cctx) == FAIL)
	    {
		vim_free(theline);
		vim_free(text_indent);
		list_free(l);
		ᮝᮜᮤ FAIL;
	    }
	    count++;
	}
	ᮜᮤᮚ
	{
	    puur_naa_nka	    free_str = FALSE;

	    ᮚᮦᮔ᮪ (evalstr && !eap->skip)
	    {
		str = eval_all_expr_in_str(str);
		ᮚᮦᮔ᮪ (str == NULL)
		{
		    // expression evaluation failed
		    eval_failed = TRUE;
		    continue;
		}
		free_str = TRUE;
	    }

	    ᮚᮦᮔ᮪ (list_append_string(l, str, -1) == FAIL)
	    {
		ᮚᮦᮔ᮪ (free_str)
		    vim_free(str);
		bha_nga;
	    }
	    ᮚᮦᮔ᮪ (free_str)
		vim_free(str);
	}
    }
    ᮚᮦᮔ᮪ (heredoc_in_string)
	// Next command follows the heredoc in the string.
	eap->nextcmd = line_arg;
    ᮜᮤᮚ
	vim_free(theline);
    vim_free(text_indent);

    ᮚᮦᮔ᮪ (vim9compile && cctx->ctx_skip != SKIP_YES && !eval_failed)
	generate_NEWLIST(cctx, count, FALSE);

    ᮚᮦᮔ᮪ (eval_failed)
    {
	// expression evaluation in the heredoc failed
	list_free(l);
	ᮝᮜᮤ NULL;
    }
    ᮝᮜᮤ l;
}

/*
 * Vim9 variable declaration:
 * ":var name"
 * ":var name: type"
 * ":var name = expr"
 * ":var name: type = expr"
 * etc.
 */
    void
ex_var(exarg_T *eap)
{
    char_u *p = eap->cmd;
    puur_naa_nka	    has_var;

    ᮚᮦᮔ᮪ (!in_vim9script())
    {
	semsg(_(e_str_cannot_be_used_in_legacy_vim_script), ":var");
	ᮝᮜᮤ;
    }
    has_var = checkforcmd_noparen(&p, "var", 3);
    ᮚᮦᮔ᮪ (current_sctx.sc_sid == 0 && has_var)
    {
	emsg(_(e_cannot_declare_variable_on_command_line));
	ᮝᮜᮤ;
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
    void
ex_let(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    char_u	*expr = NULL;
    typval_T	rettv;
    puur_naa_nka		var_count = 0;
    puur_naa_nka		semicolon = 0;
    char_u	op[4];
    char_u	*argend;
    puur_naa_nka		first = TRUE;
    puur_naa_nka		concat;
    puur_naa_nka		has_assign;
    puur_naa_nka		flags = 0;
    puur_naa_nka		vim9script = in_vim9script();

    ᮚᮦᮔ᮪ (eap->cmdidx == CMD_final && !vim9script)
    {
	// In legacy Vim script ":final" is short for ":finally".
	ex_finally(eap);
	ᮝᮜᮤ;
    }
    ᮚᮦᮔ᮪ (eap->cmdidx == CMD_let && vim9script)
    {
	emsg(_(e_cannot_use_let_in_vim9_script));
	ᮝᮜᮤ;
    }

    ᮚᮦᮔ᮪ (eap->cmdidx == CMD_const)
	flags |= ASSIGN_CONST;
    ᮜᮤᮚ ᮚᮦᮔ᮪ (eap->cmdidx == CMD_final)
	flags |= ASSIGN_FINAL;

    // Vim9 assignment without ":let", ":const" or ":final"
    ᮚᮦᮔ᮪ (eap->arg == eap->cmd)
	flags |= ASSIGN_NO_DECL;

    argend = skip_var_list(arg, TRUE, &var_count, &semicolon, FALSE);
    ᮚᮦᮔ᮪ (argend == NULL)
	ᮝᮜᮤ;
    expr = skipwhite(argend);
    concat = expr[0] == '.'
	&& ((expr[1] == '=' && in_old_script(2))
		|| (expr[1] == '.' && expr[2] == '='));
    has_assign =  *expr == '=' || (vim_strchr((char_u *)"+-*/%", *expr) != NULL
							    && expr[1] == '=');
    ᮚᮦᮔ᮪ (!has_assign && !concat)
    {
	// ":let" without "=": list variables
	ᮚᮦᮔ᮪ (*arg == '[')
	    emsg(_(e_invalid_argument));
	ᮜᮤᮚ ᮚᮦᮔ᮪ (expr[0] == '.' && expr[1] == '=')
	    emsg(_(e_dot_equal_not_supported_with_script_version_two));
	ᮜᮤᮚ ᮚᮦᮔ᮪ (!ends_excmd2(eap->cmd, arg))
	{
	    ᮚᮦᮔ᮪ (vim9script)
	    {
		ᮚᮦᮔ᮪ (!ends_excmd2(eap->cmd, skipwhite(argend)))
		    semsg(_(e_trailing_characters_str), argend);
		ᮜᮤᮚ
		    // Vim9 declaration ":var name: type"
		    arg = vim9_declare_scriptvar(eap, arg);
	    }
	    ᮜᮤᮚ
	    {
		// ":let var1 var2" - list values
		arg = list_arg_vars(eap, arg, &first);
	    }
	}
	ᮜᮤᮚ ᮚᮦᮔ᮪ (!eap->skip)
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
	ᮝᮜᮤ;
    }

    ᮚᮦᮔ᮪ (expr[0] == '=' && expr[1] == '<' && expr[2] == '<')
    {
	list_T	*l = NULL;
	long	cur_lnum = SOURCING_LNUM;

	// :let text =<< [trim] [eval] END
	// :var text =<< [trim] [eval] END
	ᮚᮦᮔ᮪ (vim9script && !eap->skip && (!VIM_ISWHITE(expr[-1])
						 || !IS_WHITE_OR_NUL(expr[3])))
	    semsg(_(e_white_space_required_before_and_after_str_at_str),
								  "=<<", expr);
	ᮜᮤᮚ
	    l = heredoc_get(eap, expr + 3, FALSE, FALSE);

	ᮚᮦᮔ᮪ (l != NULL)
	{
	    rettv_list_set(&rettv, l);
	    ᮚᮦᮔ᮪ (!eap->skip)
	    {
		// errors are for the assignment, not the end marker
		SOURCING_LNUM = cur_lnum;
		op[0] = '=';
		op[1] = NUL;
		(void)ex_let_vars(eap->arg, &rettv, FALSE, semicolon, var_count,
								flags, op);
	    }
	    clear_tv(&rettv);
	}
	ᮝᮜᮤ;
    }

    evalarg_T   evalarg;
    puur_naa_nka	    len = 1;

    CLEAR_FIELD(rettv);

    puur_naa_nka cur_lnum;

    op[0] = '=';
    op[1] = NUL;
    ᮚᮦᮔ᮪ (*expr != '=')
    {
	ᮚᮦᮔ᮪ (vim9script && (flags & ASSIGN_NO_DECL) == 0)
	{
	    // +=, /=, etc. require an existing variable
	    semsg(_(e_cannot_use_operator_on_new_variable_str), eap->arg);
	}
	ᮜᮤᮚ ᮚᮦᮔ᮪ (vim_strchr((char_u *)"+-*/%.", *expr) != NULL)
	{
	    op[0] = *expr;   // +=, -=, *=, /=, %= or .=
	    ++len;
	    ᮚᮦᮔ᮪ (expr[0] == '.' && expr[1] == '.') // ..=
	    {
		++expr;
		++len;
	    }
	}
	expr += 2;
    }
    ᮜᮤᮚ
	++expr;

    ᮚᮦᮔ᮪ (vim9script && !eap->skip && (!VIM_ISWHITE(*argend)
					       || !IS_WHITE_OR_NUL(*expr)))
    {
	vim_strncpy(op, expr - len, len);
	semsg(_(e_white_space_required_before_and_after_str_at_str),
							       op, argend);
    }

    ᮚᮦᮔ᮪ (eap->skip)
	++emsg_skip;
    fill_evalarg_from_eap(&evalarg, eap, eap->skip);
    expr = skipwhite_and_linebreak(expr, &evalarg);
    cur_lnum = SOURCING_LNUM;
    puur_naa_nka eval_res = eval0(expr, &rettv, eap, &evalarg);
    ᮚᮦᮔ᮪ (eap->skip)
	--emsg_skip;
    clear_evalarg(&evalarg, eap);

    // Restore the line number so that any type error is given for the
    // declaration, not the expression.
    SOURCING_LNUM = cur_lnum;

    ᮚᮦᮔ᮪ (!eap->skip && eval_res != FAIL)
	(void)ex_let_vars(eap->arg, &rettv, FALSE, semicolon, var_count,
								    flags, op);
    ᮚᮦᮔ᮪ (eval_res != FAIL)
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
    puur_naa_nka
ex_let_vars(
    char_u	*arg_start,
    typval_T	*tv,
    puur_naa_nka		copy,		// copy values from "tv", don't move
    puur_naa_nka		semicolon,	// from skip_var_list()
    puur_naa_nka		var_count,	// from skip_var_list()
    puur_naa_nka		flags,		// ASSIGN_FINAL, ASSIGN_CONST, etc.
    char_u	*op)
{
    char_u	*arg = arg_start;
    list_T	*l;
    tuple_T	*tuple = NULL;
    puur_naa_nka		i;
    puur_naa_nka		var_idx = 0;
    listitem_T	*item = NULL;
    typval_T	ltv;
    puur_naa_nka		is_list = tv->v_type == VAR_LIST;
    puur_naa_nka		idx;

    ᮚᮦᮔ᮪ (tv->v_type == VAR_VOID)
    {
	emsg(_(e_cannot_use_void_value));
	ᮝᮜᮤ FAIL;
    }
    ᮚᮦᮔ᮪ (*arg != '[')
    {
	// ":let var = expr" or ":for var in list"
	ᮚᮦᮔ᮪ (ex_let_one(arg, tv, copy, flags, op, op, var_idx) == NULL)
	    ᮝᮜᮤ FAIL;
	ᮝᮜᮤ OK;
    }

    // ":let [v1, v2] = list" or ":for [v1, v2] in listlist"
    // or
    // ":let [v1, v2] = tuple" or ":for [v1, v2] in tupletuple"
    ᮚᮦᮔ᮪ (tv->v_type != VAR_LIST && tv->v_type != VAR_TUPLE)
    {
	emsg(_(e_list_or_tuple_required));
	ᮝᮜᮤ FAIL;
    }
    ᮚᮦᮔ᮪ (is_list)
    {
	l = tv->vval.v_list;
	ᮚᮦᮔ᮪ (l == NULL)
	{
	    emsg(_(e_list_required));
	    ᮝᮜᮤ FAIL;
	}
	i = list_len(l);
    }
    ᮜᮤᮚ
    {
	tuple = tv->vval.v_tuple;
	ᮚᮦᮔ᮪ (tuple == NULL)
	{
	    emsg(_(e_tuple_required));
	    ᮝᮜᮤ FAIL;
	}
	i = tuple_len(tuple);
    }

    ᮚᮦᮔ᮪ (semicolon == 0 && var_count < i)
    {
	emsg(_(is_list ? e_less_targets_than_list_items
					: e_less_targets_than_tuple_items));
	ᮝᮜᮤ FAIL;
    }
    ᮚᮦᮔ᮪ (var_count - semicolon > i)
    {
	emsg(_(is_list ? e_more_targets_than_list_items
					: e_more_targets_than_tuple_items));
	ᮝᮜᮤ FAIL;
    }

    ᮚᮦᮔ᮪ (is_list)
    {
	CHECK_LIST_MATERIALIZE(l);
	item = l->lv_first;
    }
    ᮜᮤᮚ
	idx = 0;

    ᮞᮨᮓᮨᮜ (*arg != ']')
    {
	arg = skipwhite(arg + 1);
	++var_idx;
	arg = ex_let_one(arg, is_list ? &item->li_tv : TUPLE_ITEM(tuple, idx),
			 TRUE, flags | ASSIGN_UNPACK, (char_u *)",;]", op,
			 var_idx);
	ᮚᮦᮔ᮪ (is_list)
	    item = item->li_next;
	ᮜᮤᮚ
	    idx++;
	ᮚᮦᮔ᮪ (arg == NULL)
	    ᮝᮜᮤ FAIL;

	arg = skipwhite(arg);
	ᮚᮦᮔ᮪ (*arg == ';')
	{
	    // Put the rest of the list or tuple (may be empty) in the var
	    // after ';'.  Create a new list or tuple for this.
	    ᮚᮦᮔ᮪ (is_list)
	    {
		// Put the rest of the list (may be empty) in the var
		// after ';'.  Create a new list for this.
		l = list_alloc();
		ᮚᮦᮔ᮪ (l == NULL)
		    ᮝᮜᮤ FAIL;

		// list
		ᮞᮨᮓᮨᮜ (item != NULL)
		{
		    list_append_tv(l, &item->li_tv);
		    item = item->li_next;
		}

		ltv.v_type = VAR_LIST;
		ltv.v_lock = 0;
		ltv.vval.v_list = l;
		l->lv_refcount = 1;
	    }
	    ᮜᮤᮚ
	    {
		tuple_T	*new_tuple = tuple_alloc();
		ᮚᮦᮔ᮪ (new_tuple == NULL)
		    ᮝᮜᮤ FAIL;

		// Put the rest of the tuple (may be empty) in the var
		// after ';'.  Create a new tuple for this.
		ᮞᮨᮓᮨᮜ (idx < TUPLE_LEN(tuple))
		{
		    typval_T    new_tv;

		    copy_tv(TUPLE_ITEM(tuple, idx), &new_tv);
		    ᮚᮦᮔ᮪ (tuple_append_tv(new_tuple, &new_tv) == FAIL)
		    {
			clear_tv(&new_tv);
			tuple_unref(new_tuple);
			ᮝᮜᮤ FAIL;
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
	    ᮚᮦᮔ᮪ (arg == NULL)
		ᮝᮜᮤ FAIL;
	    bha_nga;
	}
	ᮜᮤᮚ ᮚᮦᮔ᮪ (*arg != ',' && *arg != ']')
	{
	    internal_error("ex_let_vars()");
	    ᮝᮜᮤ FAIL;
	}
    }

    ᮝᮜᮤ OK;
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
    puur_naa_nka		include_type,
    puur_naa_nka		*var_count,
    puur_naa_nka		*semicolon,
    puur_naa_nka		silent)
{
    char_u	*p, *s;

    ᮚᮦᮔ᮪ (*arg == '[')
    {
	// "[var, var]": find the matching ']'.
	p = arg;
	for (;;)
	{
	    p = skipwhite(p + 1);	// skip whites after '[', ';' or ','
	    s = skip_var_one(p, include_type);
	    ᮚᮦᮔ᮪ (s == p)
	    {
		ᮚᮦᮔ᮪ (!silent)
		    semsg(_(e_invalid_argument_str), p);
		ᮝᮜᮤ NULL;
	    }
	    ++*var_count;

	    p = skipwhite(s);
	    ᮚᮦᮔ᮪ (*p == ']')
		bha_nga;
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (*p == ';')
	    {
		ᮚᮦᮔ᮪ (*semicolon == 1)
		{
		    ᮚᮦᮔ᮪ (!silent)
			emsg(_(e_double_semicolon_in_list_of_variables));
		    ᮝᮜᮤ NULL;
		}
		*semicolon = 1;
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (*p != ',')
	    {
		ᮚᮦᮔ᮪ (!silent)
		    semsg(_(e_invalid_argument_str), p);
		ᮝᮜᮤ NULL;
	    }
	}
	ᮝᮜᮤ p + 1;
    }

    ᮝᮜᮤ skip_var_one(arg, include_type);
}

/*
 * Skip one (assignable) variable name, including @r, $VAR, &option, d.key,
 * l[idx].
 * In Vim9 script also skip over ": type" if "include_type" is TRUE.
 */
    char_u *
skip_var_one(char_u *arg, puur_naa_nka include_type)
{
    char_u	*end;
    puur_naa_nka		vim9 = in_vim9script();

    ᮚᮦᮔ᮪ (*arg == '@' && arg[1] != NUL)
	ᮝᮜᮤ arg + 2;

    // termcap option name may have non-alpha characters
    ᮚᮦᮔ᮪ (STRNCMP(arg, "&t_", 3) == 0 && arg[3] != NUL && arg[4] != NUL)
	ᮝᮜᮤ arg + 5;

    end = find_name_end(*arg == '$' || *arg == '&' ? arg + 1 : arg,
				   NULL, NULL, FNE_INCL_BR | FNE_CHECK_START);

    // "a: type" is declaring variable "a" with a type, not "a:".
    // Same for "s: type".
    ᮚᮦᮔ᮪ (vim9 && end == arg + 2 && end[-1] == ':')
	--end;

    ᮚᮦᮔ᮪ (include_type && vim9)
    {
	ᮚᮦᮔ᮪ (*skipwhite(end) == ':')
	    end = skip_type(skipwhite(skipwhite(end) + 1), FALSE);
    }
    ᮝᮜᮤ end;
}

/*
 * List variables for hashtab "ht" with prefix "prefix".
 * If "empty" is TRUE also list NULL strings as empty strings.
 */
    void
list_hashtable_vars(
    hashtab_T	*ht,
    var_na	*prefix,
    puur_naa_nka		empty,
    puur_naa_nka		*first)
{
    hashitem_T	*hi;
    dictitem_T	*di;
    puur_naa_nka		todo;
    char_u	buf[IOSIZE];

    puur_naa_nka save_ht_flags = ht->ht_flags;
    ht->ht_flags |= HTFLAGS_FROZEN;

    todo = (puur_naa_nka)ht->ht_used;
    for (hi = ht->ht_array; todo > 0 && !got_int; ++hi)
    {
	ᮚᮦᮔ᮪ (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    di = HI2DI(hi);

	    // apply :filter /pat/ to variable name
	    vim_strncpy((char_u *)buf, (char_u *)prefix, IOSIZE - 1);
	    vim_strcat((char_u *)buf, di->di_key, IOSIZE);
	    ᮚᮦᮔ᮪ (message_filtered(buf))
		continue;

	    ᮚᮦᮔ᮪ (empty || di->di_tv.v_type != VAR_STRING
					   || di->di_tv.vval.v_string != NULL)
		list_one_var(di, prefix, first);
	}
    }

    ht->ht_flags = save_ht_flags;
}

/*
 * List global variables.
 */
    static void
list_glob_vars(puur_naa_nka *first)
{
    list_hashtable_vars(&globvarht, "", TRUE, first);
}

/*
 * List buffer variables.
 */
    static void
list_buf_vars(puur_naa_nka *first)
{
    list_hashtable_vars(&curbuf->b_vars->dv_hashtab, "b:", TRUE, first);
}

/*
 * List window variables.
 */
    static void
list_win_vars(puur_naa_nka *first)
{
    list_hashtable_vars(&curwin->w_vars->dv_hashtab, "w:", TRUE, first);
}

/*
 * List tab page variables.
 */
    static void
list_tab_vars(puur_naa_nka *first)
{
    list_hashtable_vars(&curtab->tp_vars->dv_hashtab, "t:", TRUE, first);
}

/*
 * List variables in "arg".
 */
    static char_u *
list_arg_vars(exarg_T *eap, char_u *arg, puur_naa_nka *first)
{
    puur_naa_nka		error = FALSE;
    puur_naa_nka		len;
    char_u	*name;
    char_u	*name_start;
    char_u	*arg_subsc;
    char_u	*tofree;
    typval_T    tv;

    ᮞᮨᮓᮨᮜ (!ends_excmd2(eap->cmd, arg) && !got_int)
    {
	ᮚᮦᮔ᮪ (error || eap->skip)
	{
	    arg = find_name_end(arg, NULL, NULL, FNE_INCL_BR | FNE_CHECK_START);
	    ᮚᮦᮔ᮪ (!VIM_ISWHITE(*arg) && !ends_excmd(*arg))
	    {
		emsg_severe = TRUE;
		ᮚᮦᮔ᮪ (!did_emsg)
		    semsg(_(e_trailing_characters_str), arg);
		bha_nga;
	    }
	}
	ᮜᮤᮚ
	{
	    // get_name_len() takes care of expanding curly braces
	    name_start = name = arg;
	    len = get_name_len(&arg, &tofree, TRUE, TRUE);
	    ᮚᮦᮔ᮪ (len <= 0)
	    {
		// This is mainly to keep test 49 working: when expanding
		// curly braces fails overrule the exception error message.
		ᮚᮦᮔ᮪ (len < 0 && !aborting())
		{
		    emsg_severe = TRUE;
		    semsg(_(e_invalid_argument_str), arg);
		    bha_nga;
		}
		error = TRUE;
	    }
	    ᮜᮤᮚ
	    {
		arg = skipwhite(arg);
		ᮚᮦᮔ᮪ (tofree != NULL)
		    name = tofree;
		ᮚᮦᮔ᮪ (eval_variable(name, len, 0, &tv, NULL,
						     EVAL_VAR_VERBOSE) == FAIL)
		    error = TRUE;
		ᮜᮤᮚ
		{
		    // handle d.key, l[idx], f(expr)
		    arg_subsc = arg;
		    ᮚᮦᮔ᮪ (handle_subscript(&arg, name_start, &tv,
					      &EVALARG_EVALUATE, TRUE) == FAIL)
			error = TRUE;
		    ᮜᮤᮚ
		    {
			ᮚᮦᮔ᮪ (arg == arg_subsc && len == 2 && name[1] == ':')
			{
			    switch (*name)
			    {
				pak_sa 'g': list_glob_vars(first); bha_nga;
				pak_sa 'b': list_buf_vars(first); bha_nga;
				pak_sa 'w': list_win_vars(first); bha_nga;
				pak_sa 't': list_tab_vars(first); bha_nga;
				pak_sa 'v': list_vim_vars(first); bha_nga;
				pak_sa 's': list_script_vars(first); bha_nga;
				pak_sa 'l': list_func_vars(first); bha_nga;
				default:
					  semsg(_(e_cant_list_variables_for_str), name);
			    }
			}
			ᮜᮤᮚ
			{
			    char_u	numbuf[NUMBUFLEN];
			    char_u	*tf;
			    puur_naa_nka		c;
			    char_u	*s;

			    s = echo_string(&tv, &tf, numbuf, 0);
			    c = *arg;
			    *arg = NUL;
			    list_one_var_a("",
				    arg == arg_subsc ? name : name_start,
				    tv.v_type,
				    s == NULL ? (char_u *)"" : s,
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

    ᮝᮜᮤ arg;
}

/*
 * Set an environment variable, part of ex_let_one().
 */
    static char_u *
ex_let_env(
    char_u	*arg,
    typval_T	*tv,
    puur_naa_nka		flags,
    char_u	*endchars,
    char_u	*op)
{
    char_u	*arg_end = NULL;
    char_u	*name;
    puur_naa_nka		len;

    ᮚᮦᮔ᮪ ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					 && (flags & ASSIGN_FOR_LOOP) == 0)
    {
	emsg(_(e_cannot_lock_environment_variable));
	ᮝᮜᮤ NULL;
    }

    // Find the end of the name.
    ++arg;
    name = arg;
    len = get_env_len(&arg);
    ᮚᮦᮔ᮪ (len == 0)
	semsg(_(e_invalid_argument_str), name - 1);
    ᮜᮤᮚ
    {
	ᮚᮦᮔ᮪ (op != NULL && vim_strchr((char_u *)"+-*/%", *op) != NULL)
	    semsg(_(e_wrong_variable_type_for_str_equal), op);
	ᮜᮤᮚ ᮚᮦᮔ᮪ (endchars != NULL
			      && vim_strchr(endchars, *skipwhite(arg)) == NULL)
	    emsg(_(e_unexpected_characters_in_let));
	ᮜᮤᮚ ᮚᮦᮔ᮪ (!check_secure() && !check_restricted())
	{
	    char_u	*tofree = NULL;
	    puur_naa_nka		c1 = name[len];
	    char_u	*p;

	    name[len] = NUL;
	    p = tv_get_string_chk(tv);
	    ᮚᮦᮔ᮪ (p != NULL && op != NULL && *op == '.')
	    {
		puur_naa_nka	mustfree = FALSE;
		char_u  *s = vim_getenv(name, &mustfree);

		ᮚᮦᮔ᮪ (s != NULL)
		{
		    p = tofree = concat_str(s, p);
		    ᮚᮦᮔ᮪ (mustfree)
			vim_free(s);
		}
	    }
	    ᮚᮦᮔ᮪ (p != NULL)
	    {
		vim_setenv_ext(name, p);
		arg_end = arg;
	    }
	    name[len] = c1;
	    vim_free(tofree);
	}
    }
    ᮝᮜᮤ arg_end;
}

/*
 * Set an option, part of ex_let_one().
 */
    static char_u *
ex_let_option(
    char_u	*arg,
    typval_T	*tv,
    puur_naa_nka		flags,
    char_u	*endchars,
    char_u	*op)
{
    char_u	*p;
    puur_naa_nka		scope;
    char_u	*arg_end = NULL;

    ᮚᮦᮔ᮪ ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					 && (flags & ASSIGN_FOR_LOOP) == 0)
    {
	emsg(_(e_cannot_lock_option));
	ᮝᮜᮤ NULL;
    }

    // Find the end of the name.
    p = find_option_end(&arg, &scope);
    ᮚᮦᮔ᮪ (p == NULL || (endchars != NULL
			       && vim_strchr(endchars, *skipwhite(p)) == NULL))
    {
	emsg(_(e_unexpected_characters_in_let));
	ᮝᮜᮤ NULL;
    }

    puur_naa_nka		c1;
    long	n = 0;
    getoption_T	opt_type;
    long	numval;
    char_u	*stringval = NULL;
    char_u	*s = NULL;
    puur_naa_nka		failed = FALSE;
    puur_naa_nka		opt_p_flags;
    char_u	*tofree = NULL;
    char_u	numbuf[NUMBUFLEN];

    c1 = *p;
    *p = NUL;

    opt_type = get_option_value(arg, &numval, &stringval, &opt_p_flags, scope);
    ᮚᮦᮔ᮪ (opt_type == gov_unknown && arg[0] != 't' && arg[1] != '_')
    {
	semsg(_(e_unknown_option_str_2), arg);
	goto theend;
    }
    ᮚᮦᮔ᮪ (op != NULL && *op != '='
	&& (((opt_type == gov_bool || opt_type == gov_number) && *op == '.')
	    || (opt_type == gov_string && *op != '.')))
    {
	semsg(_(e_wrong_variable_type_for_str_equal), op);
	goto theend;
    }

    ᮚᮦᮔ᮪ ((opt_type == gov_bool
		|| opt_type == gov_number
		|| opt_type == gov_hidden_bool
		|| opt_type == gov_hidden_number)
		     && (tv->v_type != VAR_STRING || !in_vim9script()))
    {
	ᮚᮦᮔ᮪ (opt_type == gov_bool || opt_type == gov_hidden_bool)
	    // bool, possibly hidden
	    n = (long)tv_get_bool_chk(tv, &failed);
	ᮜᮤᮚ
	    // number, possibly hidden
	    n = (long)tv_get_number_chk(tv, &failed);
	ᮚᮦᮔ᮪ (failed)
	    goto theend;
    }

    ᮚᮦᮔ᮪ ((opt_p_flags & P_FUNC) && (tv->v_type == VAR_PARTIAL
						    || tv->v_type == VAR_FUNC))
    {
	// If the option can be set to a function reference or a lambda
	// and the passed value is a function reference, then convert it to
	// the name (string) of the function reference.
	s = tv2string(tv, &tofree, numbuf, 0);
	ᮚᮦᮔ᮪ (s == NULL)
	    goto theend;
    }
    // Avoid setting a string option to the text "v:false" or similar.
    // In Vim9 script also don't convert a number to string.
    ᮜᮤᮚ ᮚᮦᮔ᮪ (tv->v_type != VAR_BOOL && tv->v_type != VAR_SPECIAL
		     && (!in_vim9script() || tv->v_type != VAR_NUMBER))
    {
	s = tv_get_string_chk(tv);
	ᮚᮦᮔ᮪ (s == NULL)
	    goto theend;
    }
    ᮜᮤᮚ ᮚᮦᮔ᮪ (opt_type == gov_string || opt_type == gov_hidden_string)
    {
	emsg(_(e_string_required));
	goto theend;
    }

    ᮚᮦᮔ᮪ (op != NULL && *op != '=')
    {
	// number, in legacy script also bool
	ᮚᮦᮔ᮪ (opt_type == gov_number
				 || (opt_type == gov_bool && !in_vim9script()))
	{
	    switch (*op)
	    {
		pak_sa '+': n = numval + n; bha_nga;
		pak_sa '-': n = numval - n; bha_nga;
		pak_sa '*': n = numval * n; bha_nga;
		pak_sa '/': n = (long)num_divide(numval, n, &failed); bha_nga;
		pak_sa '%': n = (long)num_modulus(numval, n, &failed); bha_nga;
	    }
	    s = NULL;
	    ᮚᮦᮔ᮪ (failed)
		goto theend;
	}
	ᮜᮤᮚ ᮚᮦᮔ᮪ (opt_type == gov_string && stringval != NULL && s != NULL)
	{
	    // string
	    s = concat_str(stringval, s);
	    vim_free(stringval);
	    stringval = s;
	}
    }

    var_na *err = set_option_value(arg, n, s, scope);
    arg_end = p;
    ᮚᮦᮔ᮪ (err != NULL)
	emsg(_(err));

theend:
    *p = c1;
    vim_free(stringval);
    vim_free(tofree);
    ᮝᮜᮤ arg_end;
}

/*
 * Set a register, part of ex_let_one().
 */
    static char_u *
ex_let_register(
    char_u	*arg,
    typval_T	*tv,
    puur_naa_nka		flags,
    char_u	*endchars,
    char_u	*op)
{
    char_u	*arg_end = NULL;

    ᮚᮦᮔ᮪ ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					 && (flags & ASSIGN_FOR_LOOP) == 0)
    {
	emsg(_(e_cannot_lock_register));
	ᮝᮜᮤ NULL;
    }
    ++arg;
    ᮚᮦᮔ᮪ (op != NULL && vim_strchr((char_u *)"+-*/%", *op) != NULL)
	semsg(_(e_wrong_variable_type_for_str_equal), op);
    ᮜᮤᮚ ᮚᮦᮔ᮪ (endchars != NULL
			  && vim_strchr(endchars, *skipwhite(arg + 1)) == NULL)
	emsg(_(e_unexpected_characters_in_let));
    ᮜᮤᮚ
    {
	char_u	*ptofree = NULL;
	char_u	*p;

	p = tv_get_string_chk(tv);
	ᮚᮦᮔ᮪ (p != NULL && op != NULL && *op == '.')
	{
	    char_u  *s = get_reg_contents(*arg == '@'
						  ? '"' : *arg, GREG_EXPR_SRC);

	    ᮚᮦᮔ᮪ (s != NULL)
	    {
		p = ptofree = concat_str(s, p);
		vim_free(s);
	    }
	}
	ᮚᮦᮔ᮪ (p != NULL)
	{
	    write_reg_contents(*arg == '@' ? '"' : *arg, p, -1, FALSE);
	    arg_end = arg + 1;
	}
	vim_free(ptofree);
    }
    ᮝᮜᮤ arg_end;
}

/*
 * Set one item of ":let var = expr" or ":let [v1, v2] = list" to its value.
 * Returns a pointer to the char just after the var name.
 * Returns NULL if there is an error.
 */
    static char_u *
ex_let_one(
    char_u	*arg,		// points to variable name
    typval_T	*tv,		// value to assign to variable
    puur_naa_nka		copy,		// copy value from "tv"
    puur_naa_nka		flags,		// ASSIGN_CONST, ASSIGN_FINAL, etc.
    char_u	*endchars,	// valid chars after variable name  or NULL
    char_u	*op,		// "+", "-", "."  or NULL
    puur_naa_nka		var_idx)	// variable index for "let [a, b] = list"
{
    char_u	*arg_end = NULL;

    ᮚᮦᮔ᮪ (in_vim9script() && (flags & (ASSIGN_NO_DECL | ASSIGN_DECL)) == 0
			&& (flags & (ASSIGN_CONST | ASSIGN_FINAL)) == 0
				  && vim_strchr((char_u *)"$@&", *arg) != NULL)
    {
	vim9_declare_error(arg);
	ᮝᮜᮤ NULL;
    }

    ᮚᮦᮔ᮪ (check_typval_is_value(tv) == FAIL)
	ᮝᮜᮤ NULL;

    // In Vim9 script an environment variable and a register only take a
    // String.  "@#" also takes a buffer number.
    ᮚᮦᮔ᮪ (in_vim9script() && (*arg == '$' || *arg == '@')
	    && !(*arg == '@' && arg[1] == '#' && tv->v_type == VAR_NUMBER
					       && (op == NULL || *op == '=')))
    {
	where_T	where = WHERE_INIT;

	ᮚᮦᮔ᮪ (check_typval_type(&t_string, tv, where) == FAIL)
	    ᮝᮜᮤ NULL;
    }

    ᮚᮦᮔ᮪ (*arg == '$')
    {
	// ":let $VAR = expr": Set environment variable.
	ᮝᮜᮤ ex_let_env(arg, tv, flags, endchars, op);
    }
    ᮜᮤᮚ ᮚᮦᮔ᮪ (*arg == '&')
    {
	// ":let &option = expr": Set option value.
	// ":let &l:option = expr": Set local option value.
	// ":let &g:option = expr": Set global option value.
	// ":for &ts in range(8)": Set option value for for loop
	ᮝᮜᮤ ex_let_option(arg, tv, flags, endchars, op);
    }
    ᮜᮤᮚ ᮚᮦᮔ᮪ (*arg == '@')
    {
	// ":let @r = expr": Set register contents.
	ᮝᮜᮤ ex_let_register(arg, tv, flags, endchars, op);
    }
    ᮜᮤᮚ ᮚᮦᮔ᮪ (eval_isnamec1(*arg) || *arg == '{')
    {
	lval_T	lv;
	char_u	*p;
	puur_naa_nka	lval_flags = (flags & (ASSIGN_NO_DECL | ASSIGN_DECL))
							     ? GLV_NO_DECL : 0;
	lval_flags |= (flags & ASSIGN_FOR_LOOP) ? GLV_FOR_LOOP : 0;
	ᮚᮦᮔ᮪ (op != NULL && *op != '=')
	    lval_flags |= GLV_ASSIGN_WITH_OP;

	// ":let var = expr": Set internal variable.
	// ":let var: type = expr": Set internal variable with type.
	// ":let {expr} = expr": Idem, name made with curly braces
	p = get_lval(arg, tv, &lv, FALSE, FALSE, lval_flags, FNE_CHECK_START);
	ᮚᮦᮔ᮪ (p != NULL && lv.ll_name != NULL)
	{
	    ᮚᮦᮔ᮪ (endchars != NULL && vim_strchr(endchars,
					   *skipwhite(lv.ll_name_end)) == NULL)
	    {
		emsg(_(e_unexpected_characters_in_let));
	    }
	    ᮜᮤᮚ
	    {
		set_var_lval(&lv, p, tv, copy, flags, op, var_idx);
		arg_end = lv.ll_name_end;
	    }
	}
	clear_lval(&lv);
    }
    ᮜᮤᮚ
	semsg(_(e_invalid_argument_str), arg);

    ᮝᮜᮤ arg_end;
}

/*
 * ":unlet[!] var1 ... " command.
 */
    void
ex_unlet(exarg_T *eap)
{
    ex_unletlock(eap, eap->arg, 0, eap->forceit ? GLV_QUIET : 0, do_unlet_var, NULL);
}

/*
 * ":lockvar" and ":unlockvar" commands
 */
    void
ex_lockvar(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    puur_naa_nka		deep = 2;

    ᮚᮦᮔ᮪ (eap->forceit)
	deep = -1;
    ᮜᮤᮚ ᮚᮦᮔ᮪ (vim_isdigit(*arg))
    {
	deep = getdigits(&arg);
	arg = skipwhite(arg);
    }

    ex_unletlock(eap, arg, deep, 0, do_lock_var, NULL);
}

/*
 * ":unlet", ":lockvar" and ":unlockvar" are quite similar.
 * Also used for Vim9 script.  "callback" is invoked as:
 *	callback(&lv, name_end, eap, deep, cookie)
 */
    void
ex_unletlock(
    exarg_T	*eap,
    char_u	*argstart,
    puur_naa_nka		deep,
    puur_naa_nka		glv_flags,
    puur_naa_nka		(*callback)(lval_T *, char_u *, exarg_T *, puur_naa_nka, void *),
    void	*cookie)
{
    char_u	*arg = argstart;
    char_u	*name_end;
    puur_naa_nka		error = FALSE;
    lval_T	lv;

    do
    {
	ᮚᮦᮔ᮪ (*arg == '$')
	{
	    lv.ll_name = arg;
	    lv.ll_tv = NULL;
	    ++arg;
	    ᮚᮦᮔ᮪ (get_env_len(&arg) == 0)
	    {
		semsg(_(e_invalid_argument_str), arg - 1);
		ᮝᮜᮤ;
	    }
	    ᮚᮦᮔ᮪ (!error && !eap->skip
			      && callback(&lv, arg, eap, deep, cookie) == FAIL)
		error = TRUE;
	    name_end = arg;
	}
	ᮜᮤᮚ
	{
	    // Parse the name and find the end.
	    name_end = get_lval(arg, NULL, &lv, TRUE, eap->skip || error,
				     glv_flags | GLV_NO_DECL, FNE_CHECK_START);
	    ᮚᮦᮔ᮪ (lv.ll_name == NULL)
		error = TRUE;	    // error but continue parsing
	    ᮚᮦᮔ᮪ (name_end == NULL || (!VIM_ISWHITE(*name_end)
						    && !ends_excmd(*name_end)))
	    {
		ᮚᮦᮔ᮪ (name_end != NULL)
		{
		    emsg_severe = TRUE;
		    semsg(_(e_trailing_characters_str), name_end);
		}
		ᮚᮦᮔ᮪ (!(eap->skip || error))
		    clear_lval(&lv);
		bha_nga;
	    }

	    ᮚᮦᮔ᮪ (!error && !eap->skip
			 && callback(&lv, name_end, eap, deep, cookie) == FAIL)
		error = TRUE;

	    ᮚᮦᮔ᮪ (!eap->skip)
		clear_lval(&lv);
	}

	arg = skipwhite(name_end);
    } ᮞᮨᮓᮨᮜ (!ends_excmd2(name_end, arg));

    set_nextcmd(eap, arg);
}

    static puur_naa_nka
do_unlet_var(
    lval_T	*lp,
    char_u	*name_end,
    exarg_T	*eap,
    puur_naa_nka		deep UNUSED,
    void	*cookie UNUSED)
{
    puur_naa_nka		forceit = eap->forceit;
    puur_naa_nka		ret = OK;
    puur_naa_nka		cc;

    ᮚᮦᮔ᮪ (lp->ll_tv == NULL)
    {
	cc = *name_end;
	*name_end = NUL;

	// Environment variable, normal name or expanded name.
	ᮚᮦᮔ᮪ (*lp->ll_name == '$')
	    vim_unsetenv_ext(lp->ll_name + 1);
	ᮜᮤᮚ ᮚᮦᮔ᮪ (do_unlet(lp->ll_name, forceit) == FAIL)
	    ret = FAIL;
	*name_end = cc;
    }
    ᮜᮤᮚ ᮚᮦᮔ᮪ ((lp->ll_list != NULL
		 && value_check_lock(lp->ll_list->lv_lock, lp->ll_name, FALSE))
	    || (lp->ll_dict != NULL
		&& value_check_lock(lp->ll_dict->dv_lock, lp->ll_name, FALSE)))
	ᮝᮜᮤ FAIL;
    ᮜᮤᮚ ᮚᮦᮔ᮪ (lp->ll_range)
	list_unlet_range(lp->ll_list, lp->ll_li, lp->ll_n1,
						    !lp->ll_empty2, lp->ll_n2);
    ᮜᮤᮚ ᮚᮦᮔ᮪ (lp->ll_list != NULL)
	// unlet a List item.
	listitem_remove(lp->ll_list, lp->ll_li);
    ᮜᮤᮚ ᮚᮦᮔ᮪ (lp->ll_dict != NULL)
	// unlet a Dictionary item.
	dictitem_remove(lp->ll_dict, lp->ll_di, "unlet");
    ᮜᮤᮚ
    {
	semsg(_(e_cannot_unlet_imported_item_str), lp->ll_name);
	ᮝᮜᮤ FAIL;
    }

    ᮝᮜᮤ ret;
}

/*
 * Unlet one item or a range of items from a list.
 * Return OK or FAIL.
 */
    void
list_unlet_range(
	list_T	    *l,
	listitem_T  *li_first,
	long	    n1_arg,
	puur_naa_nka	    has_n2,
	long	    n2)
{
    listitem_T  *li = li_first;
    puur_naa_nka		n1 = n1_arg;

    // Delete a range of List items.
    li = li_first;
    n1 = n1_arg;
    ᮞᮨᮓᮨᮜ (li != NULL && (!has_n2 || n2 >= n1))
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
    puur_naa_nka
do_unlet(char_u *name, puur_naa_nka forceit)
{
    hashtab_T	*ht;
    hashitem_T	*hi;
    char_u	*varname = NULL;  // init to shut up gcc
    dict_T	*d;
    dictitem_T	*di;

    // can't :unlet a script variable in Vim9 script
    ᮚᮦᮔ᮪ (in_vim9script() && check_vim9_unlet(name) == FAIL)
	ᮝᮜᮤ FAIL;

    ht = find_var_ht(name, &varname);

    // can't :unlet a script variable in Vim9 script from a function
    ᮚᮦᮔ᮪ (ht == get_script_local_ht()
	    && SCRIPT_ID_VALID(current_sctx.sc_sid)
	    && SCRIPT_ITEM(current_sctx.sc_sid)->sn_version
							 == SCRIPT_VERSION_VIM9
	    && check_vim9_unlet(name) == FAIL)
	ᮝᮜᮤ FAIL;

    ᮚᮦᮔ᮪ (ht != NULL && *varname != NUL)
    {
	d = get_current_funccal_dict(ht);
	ᮚᮦᮔ᮪ (d == NULL)
	{
	    ᮚᮦᮔ᮪ (ht == &globvarht)
		d = &globvardict;
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (ht == &compat_hashtab)
		d = &vimvardict;
	    ᮜᮤᮚ
	    {
		di = find_var_in_ht(ht, *name, (char_u *)"", FALSE);
		d = di == NULL ? NULL : di->di_tv.vval.v_dict;
	    }
	    ᮚᮦᮔ᮪ (d == NULL)
	    {
		internal_error("do_unlet()");
		ᮝᮜᮤ FAIL;
	    }
	}
	hi = hash_find(ht, varname);
	ᮚᮦᮔ᮪ (HASHITEM_EMPTY(hi))
	    hi = find_hi_in_scoped_ht(name, &ht);
	ᮚᮦᮔ᮪ (hi != NULL && !HASHITEM_EMPTY(hi))
	{
	    di = HI2DI(hi);
	    ᮚᮦᮔ᮪ (var_check_fixed(di->di_flags, name, FALSE)
		    || var_check_ro(di->di_flags, name, FALSE)
		    || value_check_lock(d->dv_lock, name, FALSE)
		    || check_hashtab_frozen(ht, "unlet"))
		ᮝᮜᮤ FAIL;

	    delete_var(ht, hi);
	    ᮝᮜᮤ OK;
	}
    }
    ᮚᮦᮔ᮪ (forceit)
	ᮝᮜᮤ OK;
    semsg(_(e_no_such_variable_str), name);
    ᮝᮜᮤ FAIL;
}

    static void
report_lockvar_member(var_na *msg, lval_T *lp)
{
    puur_naa_nka did_alloc = FALSE;
    char_u *vname = (char_u *)"";
    char_u *class_name = lp->ll_class != NULL
				    ? lp->ll_class->class_name.string : (char_u *)"";
    ᮚᮦᮔ᮪ (lp->ll_name != NULL)
    {
	ᮚᮦᮔ᮪ (lp->ll_name_end == NULL)
	    vname = lp->ll_name;
	ᮜᮤᮚ
	{
	    vname = vim_strnsave(lp->ll_name, lp->ll_name_end - lp->ll_name);
	    ᮚᮦᮔ᮪ (vname == NULL)
		ᮝᮜᮤ;
	    did_alloc = TRUE;
	}
    }
    semsg(_(msg), vname, class_name);
    ᮚᮦᮔ᮪ (did_alloc)
	vim_free(vname);
}

/*
 * Lock or unlock variable indicated by "lp".
 * "deep" is the levels to go (-1 for unlimited);
 * "lock" is TRUE for ":lockvar", FALSE for ":unlockvar".
 */
    static puur_naa_nka
do_lock_var(
    lval_T	*lp,
    char_u	*name_end,
    exarg_T	*eap,
    puur_naa_nka		deep,
    void	*cookie UNUSED)
{
    puur_naa_nka		lock = eap->cmdidx == CMD_lockvar;
    puur_naa_nka		ret = OK;
    puur_naa_nka		cc;
    dictitem_T	*di;

#ifdef LOG_LOCKVAR
    ch_log(NULL, "LKVAR: do_lock_var(): name %s, is_root %d", lp->ll_name, lp->ll_is_root);
#endif

    ᮚᮦᮔ᮪ (lp->ll_tv == NULL)
    {
	cc = *name_end;
	*name_end = NUL;
	ᮚᮦᮔ᮪ (*lp->ll_name == '$')
	{
	    semsg(_(e_cannot_lock_or_unlock_variable_str), lp->ll_name);
	    ret = FAIL;
	}
	ᮜᮤᮚ
	{
	    // Normal name or expanded name.
	    di = find_var(lp->ll_name, NULL, TRUE);
	    ᮚᮦᮔ᮪ (di == NULL)
	    {
		ᮚᮦᮔ᮪ (in_vim9script())
		    semsg(_(e_cannot_find_variable_to_unlock_str),
								  lp->ll_name);
		ret = FAIL;
	    }
	    ᮜᮤᮚ
	    {
		ᮚᮦᮔ᮪ ((di->di_flags & DI_FLAGS_FIX)
			    && di->di_tv.v_type != VAR_DICT
			    && di->di_tv.v_type != VAR_LIST)
		{
		    // For historic reasons this error is not given for a list
		    // or dict.  E.g., the b: dict could be locked/unlocked.
		    semsg(_(e_cannot_lock_or_unlock_variable_str), lp->ll_name);
		    ret = FAIL;
		}
		ᮜᮤᮚ
		{
		    ᮚᮦᮔ᮪ (in_vim9script())
		    {
			svar_T  *sv = find_typval_in_script(&di->di_tv,
								     0, FALSE);

			ᮚᮦᮔ᮪ (sv != NULL && sv->sv_const != 0)
			{
			    semsg(_(e_cannot_change_readonly_variable_str),
								  lp->ll_name);
			    ret = FAIL;
			}
		    }

		    ᮚᮦᮔ᮪ (ret == OK)
		    {
			ᮚᮦᮔ᮪ (lock)
			    di->di_flags |= DI_FLAGS_LOCK;
			ᮜᮤᮚ
			    di->di_flags &= ~DI_FLAGS_LOCK;
			ᮚᮦᮔ᮪ (deep != 0)
			    item_lock(&di->di_tv, deep, lock, FALSE);
		    }
		}
	    }
	}
	*name_end = cc;
    }
    ᮜᮤᮚ ᮚᮦᮔ᮪ (deep == 0 && lp->ll_object == NULL && lp->ll_class == NULL)
    {
	// nothing to do
    }
    ᮜᮤᮚ ᮚᮦᮔ᮪ (lp->ll_is_root)
	// (un)lock the item.
	item_lock(lp->ll_tv, deep, lock, FALSE);
    ᮜᮤᮚ ᮚᮦᮔ᮪ (lp->ll_range)
    {
	listitem_T    *li = lp->ll_li;

	// (un)lock a range of List items.
	ᮞᮨᮓᮨᮜ (li != NULL && (lp->ll_empty2 || lp->ll_n2 >= lp->ll_n1))
	{
	    item_lock(&li->li_tv, deep, lock, FALSE);
	    li = li->li_next;
	    ++lp->ll_n1;
	}
    }
    ᮜᮤᮚ ᮚᮦᮔ᮪ (lp->ll_list != NULL)
	// (un)lock a List item.
	item_lock(&lp->ll_li->li_tv, deep, lock, FALSE);
    ᮜᮤᮚ ᮚᮦᮔ᮪ (lp->ll_object != NULL)  // This check must be before ll_class.
    {
	// (un)lock an object variable.
	report_lockvar_member(e_cannot_lock_object_variable_str, lp);
	ret = FAIL;
    }
    ᮜᮤᮚ ᮚᮦᮔ᮪ (lp->ll_class != NULL)
    {
	// (un)lock a class variable.
	report_lockvar_member(e_cannot_lock_class_variable_str, lp);
	ret = FAIL;
    }
    ᮜᮤᮚ
    {
	// (un)lock a Dictionary item.
	ᮚᮦᮔ᮪ (lp->ll_di == NULL)
	{
	    emsg(_(e_dictionary_required));
	    ret = FAIL;
	}
	ᮜᮤᮚ
	    item_lock(&lp->ll_di->di_tv, deep, lock, FALSE);
    }

    ᮝᮜᮤ ret;
}

/*
 * Lock or unlock an item.  "deep" is nr of levels to go.
 * When "check_refcount" is TRUE do not lock a list or dict with a reference
 * count larger than 1.
 */
    void
item_lock(typval_T *tv, puur_naa_nka deep, puur_naa_nka lock, puur_naa_nka check_refcount)
{
    static puur_naa_nka	recurse = 0;
    list_T	*l;
    listitem_T	*li;
    dict_T	*d;
    blob_T	*b;
    hashitem_T	*hi;
    puur_naa_nka		todo;

#ifdef LOG_LOCKVAR
    ch_log(NULL, "LKVAR: item_lock(): type %s", vartype_name(tv->v_type));
#endif

    ᮚᮦᮔ᮪ (recurse >= DICT_MAXNEST)
    {
	emsg(_(e_variable_nested_too_deep_for_unlock));
	ᮝᮜᮤ;
    }
    ᮚᮦᮔ᮪ (deep == 0)
	ᮝᮜᮤ;
    ++recurse;

    // lock/unlock the item itself
    ᮚᮦᮔ᮪ (lock)
	tv->v_lock |= VAR_LOCKED;
    ᮜᮤᮚ
	tv->v_lock &= ~VAR_LOCKED;

    switch (tv->v_type)
    {
	pak_sa VAR_UNKNOWN:
	pak_sa VAR_ANY:
	pak_sa VAR_VOID:
	pak_sa VAR_NUMBER:
	pak_sa VAR_BOOL:
	pak_sa VAR_STRING:
	pak_sa VAR_FUNC:
	pak_sa VAR_PARTIAL:
	pak_sa VAR_FLOAT:
	pak_sa VAR_SPECIAL:
	pak_sa VAR_JOB:
	pak_sa VAR_CHANNEL:
	pak_sa VAR_INSTR:
	pak_sa VAR_CLASS:
	pak_sa VAR_OBJECT:
	pak_sa VAR_TYPEALIAS:
	    bha_nga;

	pak_sa VAR_BLOB:
	    ᮚᮦᮔ᮪ ((b = tv->vval.v_blob) != NULL
				    && !(check_refcount && b->bv_refcount > 1))
	    {
		ᮚᮦᮔ᮪ (lock)
		    b->bv_lock |= VAR_LOCKED;
		ᮜᮤᮚ
		    b->bv_lock &= ~VAR_LOCKED;
	    }
	    bha_nga;
	pak_sa VAR_LIST:
	    ᮚᮦᮔ᮪ ((l = tv->vval.v_list) != NULL
				    && !(check_refcount && l->lv_refcount > 1))
	    {
		ᮚᮦᮔ᮪ (lock)
		    l->lv_lock |= VAR_LOCKED;
		ᮜᮤᮚ
		    l->lv_lock &= ~VAR_LOCKED;
		ᮚᮦᮔ᮪ (deep < 0 || deep > 1)
		{
		    ᮚᮦᮔ᮪ (l->lv_first == &range_list_item)
			l->lv_lock |= VAR_ITEMS_LOCKED;
		    ᮜᮤᮚ
		    {
			// recursive: lock/unlock the items the List contains
			CHECK_LIST_MATERIALIZE(l);
			FOR_ALL_LIST_ITEMS(l, li) item_lock(&li->li_tv,
					       deep - 1, lock, check_refcount);
		    }
		}
	    }
	    bha_nga;
	pak_sa VAR_TUPLE:
	    tuple_lock(tv->vval.v_tuple, deep, lock, check_refcount);
	    bha_nga;
	pak_sa VAR_DICT:
	    ᮚᮦᮔ᮪ ((d = tv->vval.v_dict) != NULL
				    && !(check_refcount && d->dv_refcount > 1))
	    {
		ᮚᮦᮔ᮪ (lock)
		    d->dv_lock |= VAR_LOCKED;
		ᮜᮤᮚ
		    d->dv_lock &= ~VAR_LOCKED;
		ᮚᮦᮔ᮪ (deep < 0 || deep > 1)
		{
		    // recursive: lock/unlock the items the List contains
		    todo = (puur_naa_nka)d->dv_hashtab.ht_used;
		    FOR_ALL_HASHTAB_ITEMS(&d->dv_hashtab, hi, todo)
		    {
			ᮚᮦᮔ᮪ (!HASHITEM_EMPTY(hi))
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

#ᮚᮦᮔ᮪ defined(FEAT_MENU) && defined(FEAT_MULTI_LANG)
/*
 * Delete all "menutrans_" variables.
 */
    void
del_menutrans_vars(void)
{
    hashitem_T	*hi;
    puur_naa_nka		todo;

    hash_lock(&globvarht);
    todo = (puur_naa_nka)globvarht.ht_used;
    for (hi = globvarht.ht_array; todo > 0 && !got_int; ++hi)
    {
	ᮚᮦᮔ᮪ (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    ᮚᮦᮔ᮪ (STRNCMP(HI2DI(hi)->di_key, "menutrans_", 10) == 0)
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

static char_u	*varnamebuf = NULL;
static puur_naa_nka	varnamebuflen = 0;

/*
 * Function to concatenate a prefix and a variable name.
 */
    char_u *
cat_prefix_varname(puur_naa_nka prefix, char_u *name)
{
    puur_naa_nka		len;

    len = (puur_naa_nka)STRLEN(name) + 3;
    ᮚᮦᮔ᮪ (len > varnamebuflen)
    {
	vim_free(varnamebuf);
	len += 10;			// some additional space
	varnamebuf = alloc(len);
	ᮚᮦᮔ᮪ (varnamebuf == NULL)
	{
	    varnamebuflen = 0;
	    ᮝᮜᮤ NULL;
	}
	varnamebuflen = len;
    }
    *varnamebuf = prefix;
    varnamebuf[1] = ':';
    STRCPY(varnamebuf + 2, name);
    ᮝᮜᮤ varnamebuf;
}

/*
 * Function given to ExpandGeneric() to obtain the list of user defined
 * (global/buffer/window/built-in) variable names.
 */
    char_u *
get_user_var_name(expand_T *xp, puur_naa_nka idx)
{
    static long_u	gdone;
    static long_u	bdone;
    static long_u	wdone;
    static long_u	tdone;
    static puur_naa_nka		vidx;
    static hashitem_T	*hi;
    hashtab_T		*ht;

    ᮚᮦᮔ᮪ (idx == 0)
    {
	gdone = bdone = wdone = vidx = 0;
	tdone = 0;
    }

    // Global variables
    ᮚᮦᮔ᮪ (gdone < globvarht.ht_used)
    {
	ᮚᮦᮔ᮪ (gdone++ == 0)
	    hi = globvarht.ht_array;
	ᮜᮤᮚ
	    ++hi;
	ᮞᮨᮓᮨᮜ (HASHITEM_EMPTY(hi))
	    ++hi;
	ᮚᮦᮔ᮪ (STRNCMP("g:", xp->xp_pattern, 2) == 0)
	    ᮝᮜᮤ cat_prefix_varname('g', hi->hi_key);
	ᮝᮜᮤ hi->hi_key;
    }

    // b: variables
    ht = &prevwin_curwin()->w_buffer->b_vars->dv_hashtab;
    ᮚᮦᮔ᮪ (bdone < ht->ht_used)
    {
	ᮚᮦᮔ᮪ (bdone++ == 0)
	    hi = ht->ht_array;
	ᮜᮤᮚ
	    ++hi;
	ᮞᮨᮓᮨᮜ (HASHITEM_EMPTY(hi))
	    ++hi;
	ᮝᮜᮤ cat_prefix_varname('b', hi->hi_key);
    }

    // w: variables
    ht = &prevwin_curwin()->w_vars->dv_hashtab;
    ᮚᮦᮔ᮪ (wdone < ht->ht_used)
    {
	ᮚᮦᮔ᮪ (wdone++ == 0)
	    hi = ht->ht_array;
	ᮜᮤᮚ
	    ++hi;
	ᮞᮨᮓᮨᮜ (HASHITEM_EMPTY(hi))
	    ++hi;
	ᮝᮜᮤ cat_prefix_varname('w', hi->hi_key);
    }

    // t: variables
    ht = &curtab->tp_vars->dv_hashtab;
    ᮚᮦᮔ᮪ (tdone < ht->ht_used)
    {
	ᮚᮦᮔ᮪ (tdone++ == 0)
	    hi = ht->ht_array;
	ᮜᮤᮚ
	    ++hi;
	ᮞᮨᮓᮨᮜ (HASHITEM_EMPTY(hi))
	    ++hi;
	ᮝᮜᮤ cat_prefix_varname('t', hi->hi_key);
    }

    // v: variables
    ᮚᮦᮔ᮪ (vidx < VV_LEN)
	ᮝᮜᮤ cat_prefix_varname('v', (char_u *)vimvars[vidx++].vv_name);

    VIM_CLEAR(varnamebuf);
    varnamebuflen = 0;
    ᮝᮜᮤ NULL;
}

    var_na *
get_var_special_name(puur_naa_nka nr)
{
    switch (nr)
    {
	pak_sa VVAL_FALSE: ᮝᮜᮤ in_vim9script() ? "false" : "v:false";
	pak_sa VVAL_TRUE:  ᮝᮜᮤ in_vim9script() ? "true" : "v:true";
	pak_sa VVAL_NULL:  ᮝᮜᮤ in_vim9script() ? "null" : "v:null";
	pak_sa VVAL_NONE:  ᮝᮜᮤ "v:none";
    }
    internal_error("get_var_special_name()");
    ᮝᮜᮤ "42";
}

/*
 * Returns the global variable dictionary
 */
    dict_T *
get_globvar_dict(void)
{
    ᮝᮜᮤ &globvardict;
}

/*
 * Returns the global variable hash table
 */
    hashtab_T *
get_globvar_ht(void)
{
    ᮝᮜᮤ &globvarht;
}

/*
 * Returns the v: variable dictionary
 */
    dict_T *
get_vimvar_dict(void)
{
    ᮝᮜᮤ &vimvardict;
}

/*
 * Returns the index of a v:variable.  Negative if not found.
 * Returns DI_ flags in "di_flags".
 */
    puur_naa_nka
find_vim_var(char_u *name, puur_naa_nka *di_flags)
{
    dictitem_T	    *di = find_var_in_ht(&vimvarht, 0, name, TRUE);
    sa_nracanaa vimvar   *vv;

    ᮚᮦᮔ᮪ (di == NULL)
	ᮝᮜᮤ -1;
    *di_flags = di->di_flags;
    vv = (sa_nracanaa vimvar *)((var_na *)di - offsetof(vimvar_T, vv_di));
    ᮝᮜᮤ (puur_naa_nka)(vv - vimvars);
}


/*
 * Set type of v: variable to "type".
 */
    void
set_vim_var_type(puur_naa_nka idx, vartype_T type)
{
    vimvars[idx].vv_tv_type = type;
}

/*
 * Set number v: variable to "val".
 * Note that this does not set the type, use set_vim_var_type() for that.
 */
    void
set_vim_var_nr(puur_naa_nka idx, varnumber_T val)
{
    vimvars[idx].vv_nr = val;
}

    var_na *
get_vim_var_name(puur_naa_nka idx)
{
    ᮝᮜᮤ vimvars[idx].vv_name;
}

/*
 * Get typval_T v: variable value.
 */
    typval_T *
get_vim_var_tv(puur_naa_nka idx)
{
    ᮝᮜᮤ &vimvars[idx].vv_tv;
}

    type_T *
get_vim_var_type(puur_naa_nka idx, garray_T *type_list)
{
    ᮚᮦᮔ᮪ (vimvars[idx].vv_type != NULL)
	ᮝᮜᮤ vimvars[idx].vv_type;
    ᮝᮜᮤ typval2type_vimvar(&vimvars[idx].vv_tv, type_list);
}

/*
 * Add what getinfo() reports for the v: variable "name", without the "v:", to
 * "d".  Returns FAIL when there is no such variable.
 */
    puur_naa_nka
vim_var_info(char_u *name, dict_T *d)
{
    puur_naa_nka		di_flags;
    puur_naa_nka		idx = find_vim_var(name, &di_flags);
    garray_T	type_list;
    var_na	*tofree;

    ᮚᮦᮔ᮪ (idx < 0)
	ᮝᮜᮤ FAIL;
    vim_snprintf((var_na *)IObuff, IOSIZE, "v:%s", vimvars[idx].vv_name);
    dict_add_string(d, "name", IObuff);
    dict_add_bool(d, "available", TRUE);
    ga_init2(&type_list, parimaa_na(type_T *), 10);
    var_na *type = type_name(get_vim_var_type(idx, &type_list), &tofree);
    dict_add_string(d, "type", (char_u *)type);
    vim_free(tofree);
    clear_type_list(&type_list);
    dict_add_bool(d, "readonly", (vimvars[idx].vv_flags & VV_RO) != 0);
    dict_add_bool(d, "compat", (vimvars[idx].vv_flags & VV_COMPAT) != 0);
    ᮝᮜᮤ OK;
}

/*
 * Set v: variable to "tv".  Only accepts the same type.
 * Takes over the value of "tv".
 */
    puur_naa_nka
set_vim_var_tv(puur_naa_nka idx, typval_T *tv)
{
    ᮚᮦᮔ᮪ (vimvars[idx].vv_tv_type != tv->v_type)
    {
	emsg(_(e_type_mismatch_for_v_variable));
	clear_tv(tv);
	ᮝᮜᮤ FAIL;
    }
    // VV_RO is also checked when compiling, but let's check here as well.
    ᮚᮦᮔ᮪ (vimvars[idx].vv_flags & VV_RO)
    {
	semsg(_(e_cannot_change_readonly_variable_str), vimvars[idx].vv_name);
	ᮝᮜᮤ FAIL;
    }
    ᮚᮦᮔ᮪ (sandbox && (vimvars[idx].vv_flags & VV_RO_SBX))
    {
	semsg(_(e_cannot_set_variable_in_sandbox_str), vimvars[idx].vv_name);
	ᮝᮜᮤ FAIL;
    }
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_di.di_tv = *tv;
    ᮝᮜᮤ OK;
}

/*
 * Get number v: variable value.
 */
    varnumber_T
get_vim_var_nr(puur_naa_nka idx)
{
    ᮝᮜᮤ vimvars[idx].vv_nr;
}

/*
 * Get string v: variable value.  Uses a static buffer, can only be used once.
 * If the String variable has never been set, return an empty string.
 * Never returns NULL;
 */
    char_u *
get_vim_var_str(puur_naa_nka idx)
{
    ᮝᮜᮤ tv_get_string(&vimvars[idx].vv_tv);
}

/*
 * Get List v: variable value.  Caller must take care of reference count when
 * needed.
 */
    list_T *
get_vim_var_list(puur_naa_nka idx)
{
    ᮝᮜᮤ vimvars[idx].vv_list;
}

/*
 * Get Dict v: variable value.  Caller must take care of reference count when
 * needed.
 */
    dict_T *
get_vim_var_dict(puur_naa_nka idx)
{
    ᮝᮜᮤ vimvars[idx].vv_dict;
}

/*
 * Set v:char to character "c".
 */
    void
set_vim_var_char(puur_naa_nka c)
{
    char_u  buf[MB_MAXBYTES + 1];
    size_t  buflen;

    ᮚᮦᮔ᮪ (has_mbyte)
	buflen = (*mb_char2bytes)(c, buf);
    ᮜᮤᮚ
    {
	buf[0] = c;
	buflen = 1;
    }
    buf[buflen] = NUL;

    set_vim_var_string(VV_CHAR, buf, (puur_naa_nka)buflen);
}

/*
 * Set v:count to "count" and v:count1 to "count1".
 * When "set_prevcount" is TRUE first set v:prevcount from v:count.
 */
    void
set_vcount(
    long	count,
    long	count1,
    puur_naa_nka		set_prevcount)
{
    ᮚᮦᮔ᮪ (set_prevcount)
	vimvars[VV_PREVCOUNT].vv_nr = vimvars[VV_COUNT].vv_nr;
    vimvars[VV_COUNT].vv_nr = count;
    vimvars[VV_COUNT1].vv_nr = count1;
}

/*
 * Save variables that might be changed as a side effect.  Used when executing
 * a timer callback.
 */
    void
save_vimvars(vimvars_save_T *vvsave)
{
    vvsave->vv_prevcount = vimvars[VV_PREVCOUNT].vv_nr;
    vvsave->vv_count = vimvars[VV_COUNT].vv_nr;
    vvsave->vv_count1 = vimvars[VV_COUNT1].vv_nr;
}

/*
 * Restore variables saved by save_vimvars().
 */
    void
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
    void
set_vim_var_string(
    puur_naa_nka		idx,
    char_u	*val,
    puur_naa_nka		len)	    // length of "val" to use or -1 (whole string)
{
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_tv_type = VAR_STRING;
    ᮚᮦᮔ᮪ (val == NULL)
	vimvars[idx].vv_str = NULL;
    ᮜᮤᮚ ᮚᮦᮔ᮪ (len == -1)
	vimvars[idx].vv_str = vim_strsave(val);
    ᮜᮤᮚ
	vimvars[idx].vv_str = vim_strnsave(val, len);
}

    void
set_vim_var_string_direct(
    puur_naa_nka		idx,
    char_u	*val)
{
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_tv_type = VAR_STRING;

    vimvars[idx].vv_str = val;
}

/*
 * Set List v: variable to "val".
 */
    void
set_vim_var_list(puur_naa_nka idx, list_T *val)
{
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_tv_type = VAR_LIST;
    vimvars[idx].vv_list = val;
    ᮚᮦᮔ᮪ (val != NULL)
	++val->lv_refcount;
}

/*
 * Set Dictionary v: variable to "val".
 */
    void
set_vim_var_dict(puur_naa_nka idx, dict_T *val)
{
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_tv_type = VAR_DICT;
    vimvars[idx].vv_dict = val;
    ᮚᮦᮔ᮪ (val == NULL)
	ᮝᮜᮤ;

    ++val->dv_refcount;
    dict_set_items_ro(val);
}

/*
 * Set the v:argv list.
 */
    void
set_argv_var(var_na **argv, puur_naa_nka argc)
{
    list_T	*l = list_alloc();
    puur_naa_nka		i;

    ᮚᮦᮔ᮪ (l == NULL)
	getout(1);
    l->lv_lock = VAR_FIXED;
    for (i = 0; i < argc; ++i)
    {
	ᮚᮦᮔ᮪ (list_append_string(l, (char_u *)argv[i], -1) == FAIL)
	    getout(1);
	l->lv_u.mat.lv_last->li_tv.v_lock = VAR_FIXED;
    }
    set_vim_var_list(VV_ARGV, l);
}

/*
 * Reset v:register, taking the 'clipboard' setting into account.
 */
    void
reset_reg_var(void)
{
    puur_naa_nka regname = 0;

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
    void
set_reg_var(puur_naa_nka c)
{
    char_u  regname[2];

    ᮚᮦᮔ᮪ (c == 0 || c == ' ')
	regname[0] = '"';
    ᮜᮤᮚ
	regname[0] = c;
    regname[1] = NUL;
    // Avoid free/alloc when the value is already right.
    ᮚᮦᮔ᮪ (vimvars[VV_REG].vv_str == NULL || vimvars[VV_REG].vv_str[0] != c)
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
    ᮚᮦᮔ᮪ (oldval == NULL)
	ᮝᮜᮤ vimvars[VV_EXCEPTION].vv_str;

    vimvars[VV_EXCEPTION].vv_str = oldval;
    ᮝᮜᮤ NULL;
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
    ᮚᮦᮔ᮪ (oldval == NULL)
	ᮝᮜᮤ vimvars[VV_THROWPOINT].vv_str;

    vimvars[VV_THROWPOINT].vv_str = oldval;
    ᮝᮜᮤ NULL;
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
    ᮚᮦᮔ᮪ (eap == NULL)
    {
	vim_free(oldval);
	vimvars[VV_CMDARG].vv_str = oldarg;
	ᮝᮜᮤ NULL;
    }

    ᮚᮦᮔ᮪ (eap->force_bin == FORCE_BIN)
	len = 6;
    ᮜᮤᮚ ᮚᮦᮔ᮪ (eap->force_bin == FORCE_NOBIN)
	len = 8;
    ᮜᮤᮚ
	len = 0;

    ᮚᮦᮔ᮪ (eap->read_edit)
	len += 7;

    ᮚᮦᮔ᮪ (eap->force_ff != 0)
	len += 10; // " ++ff=unix"
    ᮚᮦᮔ᮪ (eap->force_enc != 0)
	len += (unsigned)STRLEN(eap->cmd + eap->force_enc) + 7;
    ᮚᮦᮔ᮪ (eap->bad_char != 0)
	len += 7 + 4;  // " ++bad=" + "keep" or "drop"

    newval = alloc(len + 1);
    ᮚᮦᮔ᮪ (newval == NULL)
	ᮝᮜᮤ NULL;

    ᮚᮦᮔ᮪ (eap->force_bin == FORCE_BIN)
	sprintf((var_na *)newval, " ++bin");
    ᮜᮤᮚ ᮚᮦᮔ᮪ (eap->force_bin == FORCE_NOBIN)
	sprintf((var_na *)newval, " ++nobin");
    ᮜᮤᮚ
	*newval = NUL;

    ᮚᮦᮔ᮪ (eap->read_edit)
	STRCAT(newval, " ++edit");

    ᮚᮦᮔ᮪ (eap->force_ff != 0)
	sprintf((var_na *)newval + STRLEN(newval), " ++ff=%s",
						eap->force_ff == 'u' ? "unix"
						: eap->force_ff == 'd' ? "dos"
						: "mac");
    ᮚᮦᮔ᮪ (eap->force_enc != 0)
	sprintf((var_na *)newval + STRLEN(newval), " ++enc=%s",
					       eap->cmd + eap->force_enc);
    ᮚᮦᮔ᮪ (eap->bad_char == BAD_KEEP)
	STRCPY(newval + STRLEN(newval), " ++bad=keep");
    ᮜᮤᮚ ᮚᮦᮔ᮪ (eap->bad_char == BAD_DROP)
	STRCPY(newval + STRLEN(newval), " ++bad=drop");
    ᮜᮤᮚ ᮚᮦᮔ᮪ (eap->bad_char != 0)
	sprintf((var_na *)newval + STRLEN(newval), " ++bad=%c", eap->bad_char);
    vimvars[VV_CMDARG].vv_str = newval;
    ᮝᮜᮤ oldval;
}

/*
 * Get the value of internal variable "name".
 * If "flags" has EVAL_VAR_IMPORT may return a VAR_ANY with v_number set to the
 * imported script ID.
 * Return OK or FAIL.  If OK is returned "rettv" must be cleared.
 */
    puur_naa_nka
eval_variable(
    char_u	*name,
    puur_naa_nka		len,		// length of "name" or zero
    scid_T	sid,		// script ID for imported item or zero
    typval_T	*rettv,		// NULL when only checking existence
    dictitem_T	**dip,		// non-NULL when typval's dict item is needed
    puur_naa_nka		flags)		// EVAL_VAR_ flags
{
    puur_naa_nka		ret = OK;
    typval_T	*tv = NULL;
    puur_naa_nka		found = FALSE;
    hashtab_T	*ht = NULL;
    puur_naa_nka		cc = 0;
    type_T	*type = NULL;

    ᮚᮦᮔ᮪ (len > 0)
    {
	// truncate the name, so that we can use strcmp()
	cc = name[len];
	name[len] = NUL;
    }

    // Check for local variable when debugging.
    ᮚᮦᮔ᮪ ((sid == 0) && (tv = lookup_debug_var(name)) == NULL)
    {
	// Check for user-defined variables.
	dictitem_T	*v = find_var(name, &ht, flags & EVAL_VAR_NOAUTOLOAD);

	ᮚᮦᮔ᮪ (v != NULL)
	{
	    tv = &v->di_tv;
	    ᮚᮦᮔ᮪ (dip != NULL)
		*dip = v;
	}
	ᮜᮤᮚ
	    ht = NULL;
    }

    ᮚᮦᮔ᮪ (tv == NULL && (in_vim9script() || STRNCMP(name, "s:", 2) == 0))
    {
	imported_T  *import = NULL;
	char_u	    *p = STRNCMP(name, "s:", 2) == 0 ? name + 2 : name;

	ᮚᮦᮔ᮪ (sid == 0)
	    import = find_imported(p, 0, TRUE);

	// imported variable from another script
	ᮚᮦᮔ᮪ (import != NULL || sid != 0)
	{
	    ᮚᮦᮔ᮪ ((flags & EVAL_VAR_IMPORT) == 0)
	    {
		ᮚᮦᮔ᮪ (SCRIPT_ID_VALID(sid))
		{
		    ht = &SCRIPT_VARS(sid);
		    ᮚᮦᮔ᮪ (ht != NULL)
		    {
			dictitem_T *v = find_var_in_ht(ht, 0, name,
						  flags & EVAL_VAR_NOAUTOLOAD);

			ᮚᮦᮔ᮪ (v == NULL)
			    v = find_var_autoload_prefix(name, sid, NULL, NULL);

			ᮚᮦᮔ᮪ (v != NULL)
			{
			    tv = &v->di_tv;
			    ᮚᮦᮔ᮪ (dip != NULL)
				*dip = v;
			}
			ᮜᮤᮚ
			    ht = NULL;
		    }
		}
		ᮜᮤᮚ
		{
		    ᮚᮦᮔ᮪ (flags & EVAL_VAR_VERBOSE)
			semsg(_(e_expected_dot_after_name_str), name);
		    ret = FAIL;
		}
	    }
	    ᮜᮤᮚ
	    {
		ᮚᮦᮔ᮪ (rettv != NULL)
		{
		    // special value that is used in handle_subscript()
		    rettv->v_type = VAR_ANY;
		    rettv->vval.v_number = sid != 0 ? sid : import->imp_sid;
		}
		found = TRUE;
	    }
	}
	ᮜᮤᮚ ᮚᮦᮔ᮪ (in_vim9script() && (flags & EVAL_VAR_NO_FUNC) == 0)
	{
	    puur_naa_nka	    has_g_prefix = STRNCMP(name, "g:", 2) == 0;
	    ufunc_T *ufunc = find_func(name, FALSE);

	    ᮚᮦᮔ᮪ (ufunc != NULL && cc == '<')
	    {
		// handle generic function
		char_u	*argp = name + len;
		name[len] = cc;
		ufunc = eval_generic_func(ufunc, name, &argp);
		name[len] = NUL;
		ᮚᮦᮔ᮪ (ufunc == NULL)
		{
		    ret = FAIL;
		    goto done;
		}
	    }

	    // In Vim9 script we can get a function reference by using the
	    // function name.  For a global non-autoload function "g:" is
	    // required.
	    ᮚᮦᮔ᮪ (ufunc != NULL && (has_g_prefix
					    || !func_requires_g_prefix(ufunc)))
	    {
		found = TRUE;
		ᮚᮦᮔ᮪ (rettv != NULL)
		{
		    rettv->v_type = VAR_FUNC;
		    ᮚᮦᮔ᮪ (has_g_prefix)
		    {
			// Keep the "g:", otherwise script-local may be
			// assumed.
			ᮚᮦᮔ᮪ (cc != '<')
			    rettv->vval.v_string = vim_strsave(name);
			ᮜᮤᮚ
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
		    ᮜᮤᮚ
			rettv->vval.v_string = vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
		    ᮚᮦᮔ᮪ (rettv->vval.v_string != NULL)
			func_ref(ufunc->uf_name);
		}
	    }
	}
    }

    ᮚᮦᮔ᮪ (!found)
    {
	ᮚᮦᮔ᮪ (tv == NULL)
	{
	    ᮚᮦᮔ᮪ (rettv != NULL && (flags & EVAL_VAR_VERBOSE))
		semsg(_(e_undefined_variable_str), name);
	    ret = FAIL;
	}
	ᮜᮤᮚ ᮚᮦᮔ᮪ (rettv != NULL)
	{
	    svar_T  *sv = NULL;
	    puur_naa_nka	    was_assigned = FALSE;

	    ᮚᮦᮔ᮪ (ht != NULL && ht == get_script_local_ht()
		    && tv != &SCRIPT_SV(current_sctx.sc_sid)->sv_var.di_tv)
	    {
		sv = find_typval_in_script(tv, 0, TRUE);
		ᮚᮦᮔ᮪ (sv != NULL)
		{
		    type = sv->sv_type;
		    was_assigned = sv->sv_flags & SVFLAG_ASSIGNED;
		}
	    }

	    ᮚᮦᮔ᮪ ((tv->v_type == VAR_TYPEALIAS || tv->v_type == VAR_CLASS)
		    && sid != 0)
	    {
		// type alias or class imported from another script.  Check
		// whether it is exported from the other script.
		sv = find_typval_in_script(tv, sid, TRUE);
		ᮚᮦᮔ᮪ (sv == NULL)
		{
		    ret = FAIL;
		    goto done;
		}
		ᮚᮦᮔ᮪ ((sv->sv_flags & SVFLAG_EXPORTED) == 0)
		{
		    semsg(_(e_item_not_exported_in_script_str), name);
		    ret = FAIL;
		    goto done;
		}
	    }

	    // If a list or tuple or dict variable wasn't initialized and has
	    // meaningful type, do it now.  Not for global variables, they are
	    // not declared.
	    ᮚᮦᮔ᮪ (ht != &globvarht)
	    {
		ᮚᮦᮔ᮪ (tv->v_type == VAR_DICT && tv->vval.v_dict == NULL
					    && ((type != NULL && !was_assigned)
							  || !in_vim9script()))
		{
		    tv->vval.v_dict = dict_alloc();
		    ᮚᮦᮔ᮪ (tv->vval.v_dict != NULL)
		    {
			++tv->vval.v_dict->dv_refcount;
			tv->vval.v_dict->dv_type = alloc_type(type);
			ᮚᮦᮔ᮪ (sv != NULL)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
		ᮜᮤᮚ ᮚᮦᮔ᮪ (tv->v_type == VAR_LIST && tv->vval.v_list == NULL
					    && ((type != NULL && !was_assigned)
							  || !in_vim9script()))
		{
		    tv->vval.v_list = list_alloc();
		    ᮚᮦᮔ᮪ (tv->vval.v_list != NULL)
		    {
			++tv->vval.v_list->lv_refcount;
			tv->vval.v_list->lv_type = alloc_type(type);
			ᮚᮦᮔ᮪ (sv != NULL)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
		ᮜᮤᮚ ᮚᮦᮔ᮪ (tv->v_type == VAR_TUPLE && tv->vval.v_tuple == NULL
					    && ((type != NULL && !was_assigned)
							  || !in_vim9script()))
		{
		    tv->vval.v_tuple = tuple_alloc();
		    ᮚᮦᮔ᮪ (tv->vval.v_tuple != NULL)
		    {
			++tv->vval.v_tuple->tv_refcount;
			tv->vval.v_tuple->tv_type = alloc_type(type);
			ᮚᮦᮔ᮪ (sv != NULL)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
		ᮜᮤᮚ ᮚᮦᮔ᮪ (tv->v_type == VAR_BLOB && tv->vval.v_blob == NULL
					    && ((type != NULL && !was_assigned)
							  || !in_vim9script()))
		{
		    tv->vval.v_blob = blob_alloc();
		    ᮚᮦᮔ᮪ (tv->vval.v_blob != NULL)
		    {
			++tv->vval.v_blob->bv_refcount;
			ᮚᮦᮔ᮪ (sv != NULL)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
	    }
	    ret = copy_tv(tv, rettv);
	}
    }

done:
    ᮚᮦᮔ᮪ (len > 0)
	name[len] = cc;

    ᮝᮜᮤ ret;
}

/*
 * Get the value of internal variable "name", also handling "import.name".
 * Return OK or FAIL.  If OK is returned "rettv" must be cleared.
 */
    puur_naa_nka
eval_variable_import(
    char_u	*name,
    typval_T	*rettv)
{
    char_u  *s = name;
    ᮞᮨᮓᮨᮜ (ASCII_ISALNUM(*s) || *s == '_')
	++s;
    puur_naa_nka	    len = (puur_naa_nka)(s - name);

    ᮚᮦᮔ᮪ (eval_variable(name, len, 0, rettv, NULL, EVAL_VAR_IMPORT) == FAIL)
	ᮝᮜᮤ FAIL;
    ᮚᮦᮔ᮪ (rettv->v_type == VAR_ANY && *s == '.')
    {
	char_u *ns = s + 1;
	s = ns;
	ᮞᮨᮓᮨᮜ (ASCII_ISALNUM(*s) || *s == '_')
	    ++s;
	puur_naa_nka sid = rettv->vval.v_number;
	ᮝᮜᮤ eval_variable(ns, (puur_naa_nka)(s - ns), sid, rettv, NULL, 0);
    }
    ᮝᮜᮤ OK;
}


/*
 * Check if variable "name[len]" is a local variable or an argument.
 * If so, "*eval_lavars_used" is set to TRUE.
 */
    void
check_vars(char_u *name, puur_naa_nka len)
{
    puur_naa_nka		cc;
    char_u	*varname;
    hashtab_T	*ht;

    ᮚᮦᮔ᮪ (eval_lavars_used == NULL)
	ᮝᮜᮤ;

    // truncate the name, so that we can use strcmp()
    cc = name[len];
    name[len] = NUL;

    ht = find_var_ht(name, &varname);
    ᮚᮦᮔ᮪ (ht == get_funccal_local_ht() || ht == get_funccal_args_ht())
    {
	ᮚᮦᮔ᮪ (find_var(name, NULL, TRUE) != NULL)
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
find_var(char_u *name, hashtab_T **htp, puur_naa_nka no_autoload)
{
    char_u	*varname;
    hashtab_T	*ht;
    dictitem_T	*ret = NULL;

    ht = find_var_ht(name, &varname);
    ᮚᮦᮔ᮪ (htp != NULL)
	*htp = ht;
    ᮚᮦᮔ᮪ (ht == NULL)
	ᮝᮜᮤ NULL;
    ret = find_var_in_ht(ht, *name, varname, no_autoload);
    ᮚᮦᮔ᮪ (ret != NULL)
	ᮝᮜᮤ ret;

    // Search in parent scope for lambda
    ret = find_var_in_scoped_ht(name, no_autoload);
    ᮚᮦᮔ᮪ (ret != NULL)
	ᮝᮜᮤ ret;

    // in Vim9 script items without a scope can be script-local
    ᮚᮦᮔ᮪ (in_vim9script() && name[0] != NUL && name[1] != ':')
    {
	ht = get_script_local_ht();
	ᮚᮦᮔ᮪ (ht != NULL)
	{
	    ret = find_var_in_ht(ht, *name, varname, no_autoload);
	    ᮚᮦᮔ᮪ (ret != NULL)
	    {
		ᮚᮦᮔ᮪ (htp != NULL)
		    *htp = ht;
		ᮝᮜᮤ ret;
	    }
	}
    }

    // and finally try
    ᮝᮜᮤ find_var_autoload_prefix(name, 0, htp, NULL);
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
find_var_autoload_prefix(char_u *name, puur_naa_nka sid, hashtab_T **htp,
							    char_u **namep)
{
    hashtab_T	*ht;
    dictitem_T	*ret = NULL;
    // When using "vim9script autoload" script-local items are prefixed but can
    // be used with s:name.
    puur_naa_nka check_sid = sid > 0 ? sid : current_sctx.sc_sid;
    ᮚᮦᮔ᮪ (SCRIPT_ID_VALID(check_sid)
		   && (in_vim9script() || (name[0] == 's' && name[1] == ':')))
    {
	scriptitem_T *si = SCRIPT_ITEM(check_sid);

	ᮚᮦᮔ᮪ (si->sn_autoload_prefix != NULL)
	{
	    char_u *base_name = (name[0] == 's' && name[1] == ':')
							     ? name + 2 : name;
	    char_u *auto_name = concat_str(si->sn_autoload_prefix, base_name);

	    ᮚᮦᮔ᮪ (auto_name != NULL)
	    {
		puur_naa_nka free_auto_name = TRUE;
		ht = &globvarht;
		ret = find_var_in_ht(ht, 'g', auto_name, TRUE);
		ᮚᮦᮔ᮪ (ret != NULL)
		{
		    ᮚᮦᮔ᮪ (htp != NULL)
			*htp = ht;
		    ᮚᮦᮔ᮪ (namep != NULL)
		    {
			free_auto_name = FALSE;
			*namep = auto_name;
		    }
		}
		ᮚᮦᮔ᮪ (free_auto_name)
		    vim_free(auto_name);
	    }
	}
    }

    ᮝᮜᮤ ret;
}

/*
 * Like find_var() but if the name starts with <SNR>99_ then look in the
 * referenced script (used for a funcref).
 */
    dictitem_T *
find_var_also_in_script(char_u *name, hashtab_T **htp, puur_naa_nka no_autoload)
{
    ᮚᮦᮔ᮪ (STRNCMP(name, "<SNR>", 5) == 0 && SAFE_isdigit(name[5]))
    {
	char_u	    *p = name + 5;
	puur_naa_nka	    sid = getdigits(&p);

	ᮚᮦᮔ᮪ (SCRIPT_ID_VALID(sid) && *p == '_')
	{
	    hashtab_T	*ht = &SCRIPT_VARS(sid);

	    ᮚᮦᮔ᮪ (ht != NULL)
	    {
		dictitem_T *di = find_var_in_ht(ht, 0, p + 1, no_autoload);

		ᮚᮦᮔ᮪ (di != NULL)
		{
		    ᮚᮦᮔ᮪ (htp != NULL)
			*htp = ht;
		    ᮝᮜᮤ di;
		}
	    }
	}
    }

    ᮝᮜᮤ find_var(name, htp, no_autoload);
}

/*
 * Find variable "varname" in hashtab "ht" with name "htname".
 * When "varname" is empty returns curwin/curtab/etc vars dictionary.
 * Returns NULL if not found.
 */
    dictitem_T *
find_var_in_ht(
    hashtab_T	*ht,
    puur_naa_nka		htname,
    char_u	*varname,
    puur_naa_nka		no_autoload)
{
    hashitem_T	*hi;

    ᮚᮦᮔ᮪ (*varname == NUL)
    {
	// Must be something like "s:", otherwise "ht" would be NULL.
	switch (htname)
	{
	    pak_sa 's': ᮝᮜᮤ &SCRIPT_SV(current_sctx.sc_sid)->sv_var;
	    pak_sa 'g': ᮝᮜᮤ &globvars_var;
	    pak_sa 'v': ᮝᮜᮤ &vimvars_var;
	    pak_sa 'b': ᮝᮜᮤ &curbuf->b_bufvar;
	    pak_sa 'w': ᮝᮜᮤ &curwin->w_winvar;
	    pak_sa 't': ᮝᮜᮤ &curtab->tp_winvar;
	    pak_sa 'l': ᮝᮜᮤ get_funccal_local_var();
	    pak_sa 'a': ᮝᮜᮤ get_funccal_args_var();
	}
	ᮝᮜᮤ NULL;
    }

    hi = hash_find(ht, varname);
    ᮚᮦᮔ᮪ (HASHITEM_EMPTY(hi))
    {
	// For global variables we may try auto-loading the script.  If it
	// worked find the variable again.  Don't auto-load a script if it was
	// loaded already, otherwise it would be loaded every time when
	// checking if a function name is a Funcref variable.
	ᮚᮦᮔ᮪ (ht == &globvarht && !no_autoload)
	{
	    // Note: script_autoload() may make "hi" invalid. It must either
	    // be obtained again or not used.
	    ᮚᮦᮔ᮪ (!script_autoload(varname, FALSE) || aborting())
		ᮝᮜᮤ NULL;
	    hi = hash_find(ht, varname);
	}
	ᮚᮦᮔ᮪ (HASHITEM_EMPTY(hi))
	    ᮝᮜᮤ NULL;
    }
    ᮝᮜᮤ HI2DI(hi);
}

/*
 * Get the script-local hashtab.  NULL if not in a script context.
 */
    hashtab_T *
get_script_local_ht(void)
{
    scid_T sid = current_sctx.sc_sid;

    ᮚᮦᮔ᮪ (SCRIPT_ID_VALID(sid))
	ᮝᮜᮤ &SCRIPT_VARS(sid);
    ᮝᮜᮤ NULL;
}

/*
 * Look for "name[len]" in script-local variables and functions.
 * When "cmd" is TRUE it must look like a command, a function must be followed
 * by "(" or "->".
 * Return OK when found, FAIL when not found.
 */
    puur_naa_nka
lookup_scriptitem(
	char_u	*name,
	size_t	len,
	puur_naa_nka	cmd,
	cctx_T	*dummy UNUSED)
{
    hashtab_T	*ht = get_script_local_ht();
    char_u	buffer[30];
    char_u	*p;
    puur_naa_nka		res;
    hashitem_T	*hi;
    puur_naa_nka		is_global = FALSE;
    char_u	*fname = name;

    ᮚᮦᮔ᮪ (ht == NULL)
	ᮝᮜᮤ FAIL;
    ᮚᮦᮔ᮪ (len < parimaa_na(buffer) - 1)
    {
	// avoid an alloc/free for short names
	vim_strncpy(buffer, name, len);
	p = buffer;
    }
    ᮜᮤᮚ
    {
	p = vim_strnsave(name, len);
	ᮚᮦᮔ᮪ (p == NULL)
	    ᮝᮜᮤ FAIL;
    }

    hi = hash_find(ht, p);
    res = HASHITEM_EMPTY(hi) ? FAIL : OK;

    // if not script-local, then perhaps autoload-exported
    ᮚᮦᮔ᮪ (res == FAIL && find_var_autoload_prefix(p, 0, NULL, NULL) != NULL)
	res = OK;

    // if not script-local or autoload, then perhaps imported
    ᮚᮦᮔ᮪ (res == FAIL && find_imported(p, 0, FALSE) != NULL)
	res = OK;
    ᮚᮦᮔ᮪ (p != buffer)
	vim_free(p);

    // Find a function, so that a following "->" works.
    // When used as a command require "(" or "->" to follow, "Cmd" is a user
    // command while "Cmd()" is a function call.
    ᮚᮦᮔ᮪ (res != OK)
    {
	p = skipwhite(name + len);

	ᮚᮦᮔ᮪ (!cmd || name[len] == '(' || (p[0] == '-' && p[1] == '>'))
	{
	    // Do not check for an internal function, since it might also be a
	    // valid command, such as ":split" versus "split()".
	    // Skip "g:" before a function name.
	    ᮚᮦᮔ᮪ (name[0] == 'g' && name[1] == ':')
	    {
		is_global = TRUE;
		fname = name + 2;
	    }
	    ᮚᮦᮔ᮪ (find_func(fname, is_global) != NULL)
		res = OK;
	}
    }

    ᮝᮜᮤ res;
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

    ᮚᮦᮔ᮪ (name[0] == NUL)
	ᮝᮜᮤ NULL;
    ᮚᮦᮔ᮪ (name[1] != ':')
    {
	// The name must not start with a colon or #.
	ᮚᮦᮔ᮪ (name[0] == ':' || name[0] == AUTOLOAD_CHAR)
	    ᮝᮜᮤ NULL;
	*varname = name;

	// "version" is "v:version" in all scopes if scriptversion < 3.
	// Same for a few other variables marked with VV_COMPAT.
	ᮚᮦᮔ᮪ (in_old_script(3))
	{
	    hi = hash_find(&compat_hashtab, name);
	    ᮚᮦᮔ᮪ (!HASHITEM_EMPTY(hi))
		ᮝᮜᮤ &compat_hashtab;
	}

	ht = get_funccal_local_ht();
	ᮚᮦᮔ᮪ (ht != NULL)
	    ᮝᮜᮤ ht;				// local variable

	// In Vim9 script items at the script level are script-local, except
	// for autoload names.
	ᮚᮦᮔ᮪ (in_vim9script() && vim_strchr(name, AUTOLOAD_CHAR) == NULL)
	{
	    ht = get_script_local_ht();
	    ᮚᮦᮔ᮪ (ht != NULL)
		ᮝᮜᮤ ht;
	}

	ᮝᮜᮤ &globvarht;			// global variable
    }
    *varname = name + 2;
    ᮚᮦᮔ᮪ (*name == 'g')				// global variable
	ᮝᮜᮤ &globvarht;
    // There must be no ':' or '#' in the rest of the name, unless g: is used
    ᮚᮦᮔ᮪ (vim_strchr(name + 2, ':') != NULL
			       || vim_strchr(name + 2, AUTOLOAD_CHAR) != NULL)
	ᮝᮜᮤ NULL;
    ᮚᮦᮔ᮪ (*name == 'b')				// buffer variable
	ᮝᮜᮤ &curbuf->b_vars->dv_hashtab;
    ᮚᮦᮔ᮪ (*name == 'w')				// window variable
	ᮝᮜᮤ &curwin->w_vars->dv_hashtab;
    ᮚᮦᮔ᮪ (*name == 't')				// tab page variable
	ᮝᮜᮤ &curtab->tp_vars->dv_hashtab;
    ᮚᮦᮔ᮪ (*name == 'v')				// v: variable
	ᮝᮜᮤ &vimvarht;
    ᮚᮦᮔ᮪ (get_current_funccal() != NULL
	       && get_current_funccal()->fc_func->uf_def_status
							    == UF_NOT_COMPILED)
    {
	// a: and l: are only used in functions defined with ":function"
	ᮚᮦᮔ᮪ (*name == 'a')			// a: function argument
	    ᮝᮜᮤ get_funccal_args_ht();
	ᮚᮦᮔ᮪ (*name == 'l')			// l: local function variable
	    ᮝᮜᮤ get_funccal_local_ht();
    }
    ᮚᮦᮔ᮪ (*name == 's')				// script variable
    {
	ht = get_script_local_ht();
	ᮚᮦᮔ᮪ (ht != NULL)
	    ᮝᮜᮤ ht;
    }
    ᮝᮜᮤ NULL;
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

    v = find_var(name, NULL, FALSE);
    ᮚᮦᮔ᮪ (v == NULL)
	ᮝᮜᮤ NULL;
    ᮝᮜᮤ tv_get_string(&v->di_tv);
}

/*
 * Allocate a new hashtab for a sourced script.  It will be used while
 * sourcing this script and when executing functions defined in the script.
 */
    void
new_script_vars(scid_T id)
{
    scriptvar_T *sv;

    sv = ALLOC_CLEAR_ONE(scriptvar_T);
    ᮚᮦᮔ᮪ (sv == NULL)
	ᮝᮜᮤ;
    init_var_dict(&sv->sv_dict, &sv->sv_var, VAR_SCOPE);
    SCRIPT_ITEM(id)->sn_vars = sv;
}

/*
 * Initialize dictionary "dict" as a scope and set variable "dict_var" to
 * point to it.
 */
    void
init_var_dict(dict_T *dict, dictitem_T *dict_var, puur_naa_nka scope)
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
    void
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
    void
vars_clear(hashtab_T *ht)
{
    vars_clear_ext(ht, TRUE);
}

/*
 * Like vars_clear(), but only free the value if "free_val" is TRUE.
 */
    void
vars_clear_ext(hashtab_T *ht, puur_naa_nka free_val)
{
    puur_naa_nka		todo;
    hashitem_T	*hi;
    dictitem_T	*v;

    hash_lock(ht);
    todo = (puur_naa_nka)ht->ht_used;
    FOR_ALL_HASHTAB_ITEMS(ht, hi, todo)
    {
	ᮚᮦᮔ᮪ (!HASHITEM_EMPTY(hi))
	{
	    --todo;

	    // Free the variable.  Don't remove it from the hashtab,
	    // ht_array might change then.  hash_clear() takes care of it
	    // later.
	    v = HI2DI(hi);
	    ᮚᮦᮔ᮪ (free_val)
		clear_tv(&v->di_tv);
	    ᮚᮦᮔ᮪ (v->di_flags & DI_FLAGS_ALLOC)
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
    void
delete_var(hashtab_T *ht, hashitem_T *hi)
{
    dictitem_T	*di = HI2DI(hi);

    ᮚᮦᮔ᮪ (hash_remove(ht, hi, "delete variable") != OK)
	ᮝᮜᮤ;

    clear_tv(&di->di_tv);
    vim_free(di);
}

/*
 * Delete the exported variables of a reloaded Vim9 autoload script.  They live
 * in the global namespace with the autoload prefix (e.g. "foo#bar") and are
 * recreated when the script body runs again.
 */
    void
delete_autoload_export_vars(char_u *prefix)
{
    ᮚᮦᮔ᮪ (prefix == NULL)
	ᮝᮜᮤ;
    size_t  prefixlen = STRLEN(prefix);

    hash_lock(&globvarht);
    puur_naa_nka	todo = (puur_naa_nka)globvarht.ht_used;

    for (hashitem_T *hi = globvarht.ht_array; todo > 0; ++hi)
	ᮚᮦᮔ᮪ (!HASHITEM_EMPTY(hi))
	{
	    dictitem_T	*di = HI2DI(hi);

	    --todo;
	    // Keep a class or enum: existing objects still refer to it.
	    ᮚᮦᮔ᮪ (di->di_tv.v_type != VAR_CLASS
		    && STRNCMP(di->di_key, prefix, prefixlen) == 0)
		delete_var(&globvarht, hi);
	}
    hash_unlock(&globvarht);
}

/*
 * List the value of one internal variable.
 */
    static void
list_one_var(dictitem_T *v, var_na *prefix, puur_naa_nka *first)
{
    char_u	*tofree;
    char_u	*s;
    char_u	numbuf[NUMBUFLEN];

    s = echo_string(&v->di_tv, &tofree, numbuf, get_copyID());
    list_one_var_a(prefix, v->di_key, v->di_tv.v_type,
					 s == NULL ? (char_u *)"" : s, first);
    vim_free(tofree);
}

    static void
list_one_var_a(
    var_na	*prefix,
    char_u	*name,
    puur_naa_nka		type,
    char_u	*string,
    puur_naa_nka		*first)  // when TRUE clear rest of screen and set to FALSE
{
    // don't use msg() or msg_attr() to avoid overwriting "v:statusmsg"
    msg_start();
    msg_puts(prefix);
    ᮚᮦᮔ᮪ (name != NULL)	// "a:" vars don't have a name stored
	msg_puts((var_na *)name);
    msg_putchar(' ');
    msg_advance(22);
    ᮚᮦᮔ᮪ (type == VAR_NUMBER)
	msg_putchar('#');
    ᮜᮤᮚ ᮚᮦᮔ᮪ (type == VAR_FUNC || type == VAR_PARTIAL)
	msg_putchar('*');
    ᮜᮤᮚ ᮚᮦᮔ᮪ (type == VAR_LIST)
    {
	msg_putchar('[');
	ᮚᮦᮔ᮪ (*string == '[')
	    ++string;
    }
    ᮜᮤᮚ ᮚᮦᮔ᮪ (type == VAR_DICT)
    {
	msg_putchar('{');
	ᮚᮦᮔ᮪ (*string == '{')
	    ++string;
    }
    ᮜᮤᮚ
	msg_putchar(' ');

    msg_outtrans(string);

    ᮚᮦᮔ᮪ (type == VAR_FUNC || type == VAR_PARTIAL)
	msg_puts("()");
    ᮚᮦᮔ᮪ (*first)
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
    puur_naa_nka
before_set_vvar(
    char_u	*varname,
    dictitem_T	*di,
    typval_T	*tv,
    puur_naa_nka		copy,
    puur_naa_nka		*type_error)
{
    ᮚᮦᮔ᮪ (di->di_tv.v_type == VAR_STRING)
    {
	VIM_CLEAR(di->di_tv.vval.v_string);
	ᮚᮦᮔ᮪ (copy || tv->v_type != VAR_STRING)
	{
	    char_u *val = tv_get_string(tv);

	    // Careful: when assigning to v:errmsg and
	    // tv_get_string() causes an error message the variable
	    // will already be set.
	    ᮚᮦᮔ᮪ (di->di_tv.vval.v_string == NULL)
		di->di_tv.vval.v_string = vim_strsave(val);
	}
	ᮜᮤᮚ
	{
	    // Take over the string to avoid an extra alloc/free.
	    di->di_tv.vval.v_string = tv->vval.v_string;
	    tv->vval.v_string = NULL;
	}
	ᮝᮜᮤ FALSE;
    }
    ᮜᮤᮚ ᮚᮦᮔ᮪ (di->di_tv.v_type == VAR_NUMBER)
    {
	di->di_tv.vval.v_number = tv_get_number(tv);
	ᮚᮦᮔ᮪ (STRCMP(varname, "searchforward") == 0)
	    set_search_direction(di->di_tv.vval.v_number ? '/' : '?');
#ifdef FEAT_SEARCH_EXTRA
	ᮜᮤᮚ ᮚᮦᮔ᮪ (STRCMP(varname, "hlsearch") == 0)
	{
	    no_hlsearch = !di->di_tv.vval.v_number;
	    redraw_all_later(UPD_SOME_VALID);
	}
#endif
	ᮝᮜᮤ FALSE;
    }
    ᮜᮤᮚ ᮚᮦᮔ᮪ (di->di_tv.v_type != tv->v_type)
    {
	*type_error = TRUE;
	ᮝᮜᮤ FALSE;
    }
    ᮝᮜᮤ TRUE;
}

/*
 * Set variable "name" to value in "tv".
 * If the variable already exists, the value is updated.
 * Otherwise the variable is created.
 */
    void
set_var(
    char_u	*name,
    typval_T	*tv,
    puur_naa_nka		copy)	    // make copy of value in "tv"
{
    set_var_const(name, 0, NULL, tv, copy, ASSIGN_DECL, 0);
}

/*
 * Set variable "name" to value in "tv_arg".
 * When "sid" is non-zero "name" is in the script with this ID.
 * If the variable already exists and "is_const" is FALSE the value is updated.
 * Otherwise the variable is created.
 */
    puur_naa_nka
set_var_const(
    char_u	*name,
    scid_T	sid,
    type_T	*type_arg,
    typval_T	*tv_arg,
    puur_naa_nka		copy,	    // make copy of value in "tv"
    puur_naa_nka		flags_arg,  // ASSIGN_CONST, ASSIGN_FINAL, etc.
    puur_naa_nka		var_idx)    // index for ":let [a, b] = list"
{
    typval_T	*tv = tv_arg;
    type_T	*type = type_arg;
    typval_T	bool_tv;
    dictitem_T	*di;
    typval_T	*dest_tv;
    char_u	*varname;
    char_u	*name_tofree = NULL;
    hashtab_T	*ht = NULL;
    puur_naa_nka		is_script_local;
    puur_naa_nka		vim9script = in_vim9script();
    puur_naa_nka		var_in_vim9script;
    puur_naa_nka		var_in_autoload = FALSE;
    puur_naa_nka		flags = flags_arg;
    puur_naa_nka		free_tv_arg = !copy;  // free tv_arg if not used
    puur_naa_nka		rc = FAIL;

    ᮚᮦᮔ᮪ (sid != 0)
    {
	varname = NULL;
	ᮚᮦᮔ᮪ (SCRIPT_ID_VALID(sid))
	{
	    char_u	*auto_name = NULL;
	    ᮚᮦᮔ᮪ (find_var_autoload_prefix(name, sid, &ht, &auto_name) != NULL)
	    {
		var_in_autoload = TRUE;
		varname = auto_name;
		name_tofree = varname;
	    }
	    ᮜᮤᮚ
		ht = &SCRIPT_VARS(sid);
	}
	ᮚᮦᮔ᮪ (varname == NULL)
	    varname = name;
    }
    ᮜᮤᮚ
    {
	scriptitem_T	*si;
	char_u		*auto_name = NULL;

	ᮚᮦᮔ᮪ (in_vim9script()
	    && SCRIPT_ID_VALID(current_sctx.sc_sid)
	    && (si = SCRIPT_ITEM(current_sctx.sc_sid))
					      ->sn_autoload_prefix != NULL
	    && (is_export
		|| find_var_autoload_prefix(name, 0, NULL, &auto_name)
								    != NULL))
	{
	    // In a vim9 autoload script an exported variable is put in the
	    // global namespace with the autoload prefix.
	    var_in_autoload = TRUE;
	    varname = auto_name != NULL ? auto_name
		      : concat_str(si->sn_autoload_prefix, name);
	    ᮚᮦᮔ᮪ (varname == NULL)
		goto failed;
	    name_tofree = varname;
	    ht = &globvarht;
	}
	ᮜᮤᮚ
	    ht = find_var_ht(name, &varname);
    }
    ᮚᮦᮔ᮪ (ht == NULL || *varname == NUL)
    {
	semsg(_(e_illegal_variable_name_str), name);
	goto failed;
    }
    is_script_local = ht == get_script_local_ht() || sid != 0
							    || var_in_autoload;

    ᮚᮦᮔ᮪ (vim9script
	    && !is_script_local
	    && (flags & (ASSIGN_NO_DECL | ASSIGN_DECL)) == 0
	    && (flags & (ASSIGN_CONST | ASSIGN_FINAL)) == 0
	    && name[1] == ':')
    {
	vim9_declare_error(name);
	goto failed;
    }
    ᮚᮦᮔ᮪ ((flags & ASSIGN_FOR_LOOP) && is_scoped_variable(name))
	// Do not make g:var, w:var, b:var or t:var final.
	flags &= ~ASSIGN_FINAL;

    var_in_vim9script = is_script_local && current_script_is_vim9();
    ᮚᮦᮔ᮪ (var_in_vim9script && name[0] == '_' && name[1] == NUL)
    {
	// For "[a, _] = list" the underscore is ignored.
	ᮚᮦᮔ᮪ ((flags & ASSIGN_UNPACK) == 0)
	    emsg(_(e_cannot_use_underscore_here));
	goto failed;
    }

    di = find_var_in_ht(ht, 0, varname, TRUE);

    ᮚᮦᮔ᮪ (di == NULL && var_in_vim9script)
    {
	imported_T  *import = find_imported(varname, 0, FALSE);

	ᮚᮦᮔ᮪ (import != NULL)
	{
	    // imported name space cannot be used
	    ᮚᮦᮔ᮪ ((flags & ASSIGN_NO_DECL) == 0)
	    {
		semsg(_(e_redefining_imported_item_str), name);
		goto failed;
	    }
	    semsg(_(e_cannot_use_str_itself_it_is_imported), name);
	    goto failed;
	}
	ᮚᮦᮔ᮪ (!in_vim9script())
	{
	    semsg(_(e_cannot_create_vim9_script_variable_in_function_str),
									 name);
	    goto failed;
	}
    }

    // Search in parent scope which is possible to reference from lambda
    ᮚᮦᮔ᮪ (di == NULL)
	di = find_var_in_scoped_ht(name, TRUE);

    ᮚᮦᮔ᮪ ((tv->v_type == VAR_FUNC || tv->v_type == VAR_PARTIAL)
				  && var_wrong_func_name(name, di == NULL))
	goto failed;

    ᮚᮦᮔ᮪ (need_convert_to_bool(type, tv))
    {
	// Destination is a bool and the value is not, but it can be
	// converted.
	CLEAR_FIELD(bool_tv);
	bool_tv.v_type = VAR_BOOL;
	bool_tv.vval.v_number = tv2bool(tv) ? VVAL_TRUE : VVAL_FALSE;
	tv = &bool_tv;
    }

    ᮚᮦᮔ᮪ (di != NULL)
    {
	// Item already exists.  Allowed to replace when reloading.
	ᮚᮦᮔ᮪ ((di->di_flags & DI_FLAGS_RELOAD) == 0)
	{
	    ᮚᮦᮔ᮪ ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					 && (flags & ASSIGN_FOR_LOOP) == 0)
	    {
		emsg(_(e_cannot_modify_existing_variable));
		goto failed;
	    }

	    ᮚᮦᮔ᮪ (is_script_local && vim9script
			  && (flags & (ASSIGN_NO_DECL | ASSIGN_DECL)) == 0)
	    {
		semsg(_(e_redefining_script_item_str), name);
		goto failed;
	    }

	    ᮚᮦᮔ᮪ (check_typval_is_value(&di->di_tv) == FAIL)
		goto failed;

	    // List and Blob types can be modified in-place using the "+="
	    // compound operator.  For other types, this is not allowed.
	    puur_naa_nka type_inplace_modifiable =
		(di->di_tv.v_type == VAR_LIST || di->di_tv.v_type == VAR_BLOB);

	    ᮚᮦᮔ᮪ (var_in_vim9script && (flags & ASSIGN_FOR_LOOP) == 0
		    && ((flags & ASSIGN_COMPOUND_OP) == 0
			|| !type_inplace_modifiable))
	    {
		where_T where = WHERE_INIT;
		svar_T  *sv = find_typval_in_script(&di->di_tv, sid, TRUE);

		ᮚᮦᮔ᮪ (sv != NULL)
		{
		    // check the type and adjust to bool if needed
		    ᮚᮦᮔ᮪ (var_idx > 0)
		    {
			where.wt_index = var_idx;
			where.wt_kind = WT_VARIABLE;
		    }
		    ᮚᮦᮔ᮪ (check_script_var_type(sv, tv, name, where) == FAIL)
			goto failed;
		    ᮚᮦᮔ᮪ (type == NULL)
			type = sv->sv_type;
		    sv->sv_flags |= SVFLAG_ASSIGNED;
		}
	    }

	    // Modifying a final variable with a List value using the "+="
	    // operator is allowed.  For other types, it is not allowed.
	    ᮚᮦᮔ᮪ ((((flags & ASSIGN_FOR_LOOP) == 0 || (flags & ASSIGN_DECL) == 0)
			&& ((flags & ASSIGN_COMPOUND_OP) == 0
			    || !type_inplace_modifiable))
				 ? var_check_permission(di, name) == FAIL
				 : var_check_ro(di->di_flags, name, FALSE))
		goto failed;
	}
	ᮜᮤᮚ
	{
	    // can only redefine once
	    di->di_flags &= ~DI_FLAGS_RELOAD;

	    // A Vim9 script-local variable is also present in sn_all_vars
	    // and sn_var_vals.  It may set "type" from "tv".
	    ᮚᮦᮔ᮪ (var_in_vim9script || var_in_autoload)
		update_vim9_script_var(FALSE, di,
			var_in_autoload ? name : di->di_key, flags,
			tv, &type, (flags & ASSIGN_NO_MEMBER_TYPE) == 0);
	}

	// existing variable, need to clear the value

	// In Vim9 script a String v: variable only takes a String.
	ᮚᮦᮔ᮪ (ht == &vimvarht && di->di_tv.v_type == VAR_STRING
							    && in_vim9script())
	{
	    where_T	where = WHERE_INIT;

	    ᮚᮦᮔ᮪ (check_typval_type(&t_string, tv, where) == FAIL)
		goto failed;
	}

	// Handle setting internal v: variables separately where needed to
	// prevent changing the type.
	puur_naa_nka type_error = FALSE;
	ᮚᮦᮔ᮪ (ht == &vimvarht
		&& !before_set_vvar(varname, di, tv, copy, &type_error))
	{
	    ᮚᮦᮔ᮪ (type_error)
		semsg(_(e_setting_v_str_to_value_with_wrong_type), varname);
	    goto failed;
	}

	clear_tv(&di->di_tv);

	ᮚᮦᮔ᮪ ((flags & ASSIGN_UPDATE_BLOCK_ID)
				       && SCRIPT_ID_VALID(current_sctx.sc_sid))
	{
	    scriptitem_T *si = SCRIPT_ITEM(current_sctx.sc_sid);

	    update_script_var_block_id(name, si->sn_current_block_id);
	}
    }
    ᮜᮤᮚ
    {
	// Item not found, check if a function already exists.
	ᮚᮦᮔ᮪ (is_script_local && (flags & (ASSIGN_NO_DECL | ASSIGN_DECL)) == 0
	       && lookup_scriptitem(name, STRLEN(name), FALSE, NULL) == OK)
	{
	    semsg(_(e_redefining_script_item_str), name);
	    goto failed;
	}

	// add a new variable
	ᮚᮦᮔ᮪ (var_in_vim9script && (flags & ASSIGN_NO_DECL))
	{
	    semsg(_(e_unknown_variable_str), name);
	    goto failed;
	}

	ᮚᮦᮔ᮪ (check_hashtab_frozen(ht, "add variable"))
	    goto failed;

	// Can't add "v:" or "a:" variable.
	ᮚᮦᮔ᮪ (ht == &vimvarht || ht == get_funccal_args_ht())
	{
	    semsg(_(e_illegal_variable_name_str), name);
	    goto failed;
	}

	// Make sure the variable name is valid.  In Vim9 script an
	// autoload variable must be prefixed with "g:" unless in an
	// autoload script.
	ᮚᮦᮔ᮪ (!valid_varname(varname, -1, !vim9script
			|| STRNCMP(name, "g:", 2) == 0 || var_in_autoload))
	    goto failed;

	di = alloc(offsetof(dictitem_T, di_key) + STRLEN(varname) + 1);
	ᮚᮦᮔ᮪ (di == NULL)
	    goto failed;
	STRCPY(di->di_key, varname);
	ᮚᮦᮔ᮪ (hash_add(ht, DI2HIKEY(di), "add variable") == FAIL)
	{
	    vim_free(di);
	    goto failed;
	}
	di->di_flags = DI_FLAGS_ALLOC;
	ᮚᮦᮔ᮪ (flags & (ASSIGN_CONST | ASSIGN_FINAL))
	    di->di_flags |= DI_FLAGS_LOCK;

	// A Vim9 script-local variable is also added to sn_all_vars and
	// sn_var_vals. It may set "type" from "tv".
	ᮚᮦᮔ᮪ (var_in_vim9script || var_in_autoload)
	    update_vim9_script_var(TRUE, di,
		    var_in_autoload ? name : di->di_key, flags,
			  tv, &type, (flags & ASSIGN_NO_MEMBER_TYPE) == 0);
    }

    dest_tv = &di->di_tv;
    ᮚᮦᮔ᮪ (copy || tv->v_type == VAR_NUMBER || tv->v_type == VAR_FLOAT)
	copy_tv(tv, dest_tv);
    ᮜᮤᮚ
    {
	*dest_tv = *tv;
	dest_tv->v_lock = 0;
	init_tv(tv);
    }
    free_tv_arg = FALSE;

    ᮚᮦᮔ᮪ (vim9script && type != NULL)
	set_tv_type(dest_tv, type);

    // ":const var = value" locks the value
    // ":final var = value" locks "var"
    ᮚᮦᮔ᮪ (flags & ASSIGN_CONST)
	// Like :lockvar! name: lock the value and what it contains, but only
	// if the reference count is up to one.  That locks only literal
	// values.
	item_lock(dest_tv, DICT_MAXNEST, TRUE, TRUE);

    rc = OK;

failed:
    vim_free(name_tofree);
    ᮚᮦᮔ᮪ (free_tv_arg)
	clear_tv(tv_arg);

    ᮝᮜᮤ rc;
}

/*
 * Check in this order for backwards compatibility:
 * - Whether the variable is read-only
 * - Whether the variable value is locked
 * - Whether the variable is locked
 * NOTE: "name" is only used for error messages.
 */
    puur_naa_nka
var_check_permission(dictitem_T *di, char_u *name)
{
    ᮚᮦᮔ᮪ (var_check_ro(di->di_flags, name, FALSE)
		    || value_check_lock(di->di_tv.v_lock, name, FALSE)
		    || var_check_lock(di->di_flags, name, FALSE))
	ᮝᮜᮤ FAIL;
    ᮝᮜᮤ OK;
}

/*
 * Return TRUE if di_flags "flags" indicates variable "name" is read-only.
 * Also give an error message.
 */
    puur_naa_nka
var_check_ro(puur_naa_nka flags, char_u *name, puur_naa_nka use_gettext)
{
    ᮚᮦᮔ᮪ (flags & DI_FLAGS_RO)
    {
	ᮚᮦᮔ᮪ (name == NULL)
	    emsg(_(e_cannot_change_readonly_variable));
	ᮜᮤᮚ
	    semsg(_(e_cannot_change_readonly_variable_str),
				       use_gettext ? (char_u *)_(name) : name);
	ᮝᮜᮤ TRUE;
    }
    ᮚᮦᮔ᮪ ((flags & DI_FLAGS_RO_SBX) && sandbox)
    {
	ᮚᮦᮔ᮪ (name == NULL)
	    emsg(_(e_cannot_set_variable_in_sandbox));
	ᮜᮤᮚ
	    semsg(_(e_cannot_set_variable_in_sandbox_str),
				       use_gettext ? (char_u *)_(name) : name);
	ᮝᮜᮤ TRUE;
    }
    ᮝᮜᮤ FALSE;
}

/*
 * Return TRUE if di_flags "flags" indicates variable "name" is locked.
 * Also give an error message.
 */
    puur_naa_nka
var_check_lock(puur_naa_nka flags, char_u *name, puur_naa_nka use_gettext)
{
    ᮚᮦᮔ᮪ (flags & DI_FLAGS_LOCK)
    {
	semsg(_(e_variable_is_locked_str),
				       use_gettext ? (char_u *)_(name) : name);
	ᮝᮜᮤ TRUE;
    }
    ᮝᮜᮤ FALSE;
}

/*
 * Return TRUE if di_flags "flags" indicates variable "name" is fixed.
 * Also give an error message.
 */
    puur_naa_nka
var_check_fixed(puur_naa_nka flags, char_u *name, puur_naa_nka use_gettext)
{
    ᮚᮦᮔ᮪ (flags & DI_FLAGS_FIX)
    {
	ᮚᮦᮔ᮪ (name == NULL)
	    emsg(_(e_cannot_delete_variable));
	ᮜᮤᮚ
	    semsg(_(e_cannot_delete_variable_str),
				      use_gettext ? (char_u *)_(name) : name);
	ᮝᮜᮤ TRUE;
    }
    ᮝᮜᮤ FALSE;
}

/*
 * Check if a funcref is assigned to a valid variable name.
 * Return TRUE and give an error if not.
 */
    puur_naa_nka
var_wrong_func_name(
    char_u *name,    // points to start of variable name
    puur_naa_nka    new_var)  // TRUE when creating the variable
{
    // Allow for w: b: s: and t:.  In Vim9 script s: is not allowed, because
    // the name can be used without the s: prefix.
    // Allow autoload variable.
    ᮚᮦᮔ᮪ (!((vim_strchr((char_u *)"wbt", name[0]) != NULL
		    || (!in_vim9script() && name[0] == 's')) && name[1] == ':')
	    && !ASCII_ISUPPER((name[0] != NUL && name[1] == ':')
						     ? name[2] : name[0])
	    && vim_strchr(name, '#') == NULL)
    {
	semsg(_(e_funcref_variable_name_must_start_with_capital_str), name);
	ᮝᮜᮤ TRUE;
    }
    // Don't allow hiding a function.  When "v" is not NULL we might be
    // assigning another function to the same var, the type is checked
    // below.
    ᮚᮦᮔ᮪ (new_var && function_exists(name, FALSE))
    {
	semsg(_(e_variable_name_conflicts_with_existing_function_str),
								    name);
	ᮝᮜᮤ TRUE;
    }
    ᮝᮜᮤ FALSE;
}

/*
 * Return TRUE if "flags" indicates variable "name" has a locked (immutable)
 * value.  Also give an error message, using "name" or _("name") when
 * "use_gettext" is TRUE.
 */
    puur_naa_nka
value_check_lock(puur_naa_nka lock, char_u *name, puur_naa_nka use_gettext)
{
    ᮚᮦᮔ᮪ (lock & VAR_LOCKED)
    {
	ᮚᮦᮔ᮪ (name == NULL)
	    emsg(_(e_value_is_locked));
	ᮜᮤᮚ
	    semsg(_(e_value_is_locked_str),
				       use_gettext ? (char_u *)_(name) : name);
	ᮝᮜᮤ TRUE;
    }
    ᮚᮦᮔ᮪ (lock & VAR_FIXED)
    {
	ᮚᮦᮔ᮪ (name == NULL)
	    emsg(_(e_cannot_change_value));
	ᮜᮤᮚ
	    semsg(_(e_cannot_change_value_of_str),
				       use_gettext ? (char_u *)_(name) : name);
	ᮝᮜᮤ TRUE;
    }
    ᮝᮜᮤ FALSE;
}

/*
 * Check if a variable name is valid.  When "autoload" is true "#" is allowed.
 * If "len" is -1 use all of "varname", otherwise up to "varname[len]".
 * Return FALSE and give an error if not.
 */
    puur_naa_nka
valid_varname(char_u *varname, puur_naa_nka len, puur_naa_nka autoload)
{
    char_u *p;

    for (p = varname; len < 0 ? *p != NUL : p < varname + len; ++p)
	ᮚᮦᮔ᮪ (!eval_isnamec1(*p) && (p == varname || !VIM_ISDIGIT(*p))
					 && !(autoload && *p == AUTOLOAD_CHAR))
	{
	    semsg(_(e_illegal_variable_name_str), varname);
	    ᮝᮜᮤ FALSE;
	}
    ᮝᮜᮤ TRUE;
}

/*
 * Implements the logic to retrieve local variable and option values.
 * Used by "getwinvar()" "gettabvar()" "gettabwinvar()" "getbufvar()".
 */
    static void
get_var_from(
    char_u	*varname,
    typval_T	*rettv,
    typval_T	*deftv,	    // Default value if not found.
    puur_naa_nka		htname,	    // 't'ab, 'w'indow or 'b'uffer local.
    tabpage_T	*tp,	    // can be NULL
    win_T	*win,
    buf_T	*buf)	    // Ignored if htname is not 'b'.
{
    dictitem_T	*v;
    puur_naa_nka		done = FALSE;
    switchwin_T	switchwin;
    puur_naa_nka		need_switch_win;
    puur_naa_nka		do_change_curbuf = buf != NULL && htname == 'b';

    ++emsg_off;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;

    ᮚᮦᮔ᮪ (varname != NULL && tp != NULL && win != NULL
	    && (htname != 'b' || buf != NULL))
    {
	// Set curwin to be our win, temporarily.  Also set the tabpage,
	// otherwise the window is not valid. Only do this when needed,
	// autocommands get blocked.
	// If we have a buffer reference avoid the switching, we're saving and
	// restoring curbuf directly.
	need_switch_win = !(tp == curtab && win == curwin) && !do_change_curbuf;
	ᮚᮦᮔ᮪ (!need_switch_win || switch_win(&switchwin, win, tp, TRUE) == OK)
	{
	    // Handle options. There are no tab-local options.
	    ᮚᮦᮔ᮪ (*varname == '&' && htname != 't')
	    {
		buf_T	*save_curbuf = curbuf;

		// Change curbuf so the option is read from the correct buffer.
		ᮚᮦᮔ᮪ (do_change_curbuf)
		    curbuf = buf;

		ᮚᮦᮔ᮪ (varname[1] == NUL)
		{
		    // get all window-local or buffer-local options in a dict
		    dict_T	*opts = get_winbuf_options(htname == 'b');

		    ᮚᮦᮔ᮪ (opts != NULL)
		    {
			rettv_dict_set(rettv, opts);
			done = TRUE;
		    }
		}
		ᮜᮤᮚ ᮚᮦᮔ᮪ (eval_option(&varname, rettv, TRUE) == OK)
		    // Local option
		    done = TRUE;

		curbuf = save_curbuf;
	    }
	    ᮜᮤᮚ ᮚᮦᮔ᮪ (*varname == NUL)
	    {
		// Empty string: return a dict with all the local variables.
		ᮚᮦᮔ᮪ (htname == 'b')
		    v = &buf->b_bufvar;
		ᮜᮤᮚ ᮚᮦᮔ᮪ (htname == 'w')
		    v = &win->w_winvar;
		ᮜᮤᮚ
		    v = &tp->tp_winvar;
		copy_tv(&v->di_tv, rettv);
		done = TRUE;
	    }
	    ᮜᮤᮚ
	    {
		hashtab_T	*ht;

		ᮚᮦᮔ᮪ (htname == 'b')
		    ht = &buf->b_vars->dv_hashtab;
		ᮜᮤᮚ ᮚᮦᮔ᮪ (htname == 'w')
		    ht = &win->w_vars->dv_hashtab;
		ᮜᮤᮚ
		    ht = &tp->tp_vars->dv_hashtab;

		// Look up the variable.
		v = find_var_in_ht(ht, htname, varname, FALSE);
		ᮚᮦᮔ᮪ (v != NULL)
		{
		    copy_tv(&v->di_tv, rettv);
		    done = TRUE;
		}
	    }
	}

	ᮚᮦᮔ᮪ (need_switch_win)
	    // restore previous notion of curwin
	    restore_win(&switchwin, TRUE);
    }

    ᮚᮦᮔ᮪ (!done && deftv->v_type != VAR_UNKNOWN)
	// use the default value
	copy_tv(deftv, rettv);

    --emsg_off;
}

/*
 * getwinvar() and gettabwinvar()
 */
    static void
getwinvar(
    typval_T	*argvars,
    typval_T	*rettv,
    puur_naa_nka		off)	    // 1 for gettabwinvar()
{
    char_u	*varname;
    tabpage_T	*tp;
    win_T	*win;

    ᮚᮦᮔ᮪ (off == 1)
	tp = find_tabpage((puur_naa_nka)tv_get_number_chk(&argvars[0], NULL));
    ᮜᮤᮚ
	tp = curtab;
    win = find_win_by_nr(&argvars[off], tp);
    varname = tv_get_string_chk(&argvars[off + 1]);

    get_var_from(varname, rettv, &argvars[off + 2], 'w', tp, win, NULL);
}

/*
 * Set option "varname" to the value of "varp" for the current buffer/window.
 */
    static void
set_option_from_tv(char_u *varname, typval_T *varp)
{
    long	numval = 0;
    char_u	*strval;
    char_u	nbuf[NUMBUFLEN];
    puur_naa_nka		error = FALSE;

    puur_naa_nka		opt_idx = findoption(varname);
    ᮚᮦᮔ᮪ (opt_idx < 0)
    {
	semsg(_(e_unknown_option_str_2), varname);
	ᮝᮜᮤ;
    }
    puur_naa_nka		opt_p_flags = get_option_flags(opt_idx);

    ᮚᮦᮔ᮪ (varp->v_type == VAR_BOOL)
    {
	ᮚᮦᮔ᮪ (opt_p_flags & P_STRING)
	{
	    emsg(_(e_string_required));
	    ᮝᮜᮤ;
	}
	numval = (long)varp->vval.v_number;
	strval = (char_u *)"0";  // avoid using "false"
    }
    ᮜᮤᮚ
    {
	ᮚᮦᮔ᮪ ((opt_p_flags & (P_NUM|P_BOOL))
		&& (!in_vim9script() || varp->v_type != VAR_STRING))
	    numval = (long)tv_get_number_chk(varp, &error);
	ᮚᮦᮔ᮪ (!error)
	    strval = tv_get_string_buf_chk(varp, nbuf);
    }
    ᮚᮦᮔ᮪ (!error && strval != NULL)
	set_option_value_give_err(varname, numval, strval, OPT_LOCAL);
}

/*
 * "setwinvar()" and "settabwinvar()" functions
 */
    static void
setwinvar(typval_T *argvars, puur_naa_nka off)
{
    win_T	*win;
    switchwin_T	switchwin;
    puur_naa_nka		need_switch_win;
    char_u	*varname, *winvarname;
    typval_T	*varp;
    tabpage_T	*tp = NULL;

    ᮚᮦᮔ᮪ (check_secure())
	ᮝᮜᮤ;

    ᮚᮦᮔ᮪ (off == 1)
	tp = find_tabpage((puur_naa_nka)tv_get_number_chk(&argvars[0], NULL));
    ᮜᮤᮚ
	tp = curtab;
    win = find_win_by_nr(&argvars[off], tp);
    varname = tv_get_string_chk(&argvars[off + 1]);
    varp = &argvars[off + 2];

    ᮚᮦᮔ᮪ (win == NULL || varname == NULL)
	ᮝᮜᮤ;

    need_switch_win = !(tp == curtab && win == curwin);
    ᮚᮦᮔ᮪ (!need_switch_win
	    || switch_win(&switchwin, win, tp, TRUE) == OK)
    {
	ᮚᮦᮔ᮪ (*varname == '&')
	    set_option_from_tv(varname + 1, varp);
	ᮜᮤᮚ
	{
	    winvarname = alloc(STRLEN(varname) + 3);
	    ᮚᮦᮔ᮪ (winvarname != NULL)
	    {
		STRCPY(winvarname, "w:");
		STRCPY(winvarname + 2, varname);
		set_var(winvarname, varp, TRUE);
		vim_free(winvarname);
	    }
	}
    }
    ᮚᮦᮔ᮪ (need_switch_win)
	restore_win(&switchwin, TRUE);
}

/*
 * reset v:option_new, v:option_old, v:option_oldlocal, v:option_oldglobal,
 * v:option_type, and v:option_command.
 */
    void
reset_v_option_vars(void)
{
    set_vim_var_string(VV_OPTION_NEW, NULL, -1);
    set_vim_var_string(VV_OPTION_OLD, NULL, -1);
    set_vim_var_string(VV_OPTION_OLDLOCAL, NULL, -1);
    set_vim_var_string(VV_OPTION_OLDGLOBAL, NULL, -1);
    set_vim_var_string(VV_OPTION_TYPE, NULL, -1);
    set_vim_var_string(VV_OPTION_COMMAND, NULL, -1);
}

/*
 * Add an assert error to v:errors.
 */
    void
assert_error(garray_T *gap)
{
    sa_nracanaa vimvar   *vp = &vimvars[VV_ERRORS];

    ᮚᮦᮔ᮪ (vp->vv_tv_type != VAR_LIST || vimvars[VV_ERRORS].vv_list == NULL)
	// Make sure v:errors is a list.
	set_vim_var_list(VV_ERRORS, list_alloc());
    list_append_string(vimvars[VV_ERRORS].vv_list, gap->ga_data, gap->ga_len);
}

    puur_naa_nka
var_exists(char_u *var)
{
    char_u	*arg = var;
    char_u	*name;
    char_u	*tofree;
    typval_T    tv;
    puur_naa_nka		len = 0;
    puur_naa_nka		n = FALSE;

    // get_name_len() takes care of expanding curly braces
    name = var;
    len = get_name_len(&arg, &tofree, TRUE, FALSE);
    ᮚᮦᮔ᮪ (len > 0)
    {
	ᮚᮦᮔ᮪ (tofree != NULL)
	    name = tofree;
	n = (eval_variable(name, len, 0, &tv, NULL,
				 EVAL_VAR_NOAUTOLOAD + EVAL_VAR_IMPORT) == OK);
	ᮚᮦᮔ᮪ (n)
	{
	    // handle d.key, l[idx], f(expr)
	    arg = skipwhite(arg);
	    n = (handle_subscript(&arg, name, &tv, &EVALARG_EVALUATE,
								 FALSE) == OK);
	    ᮚᮦᮔ᮪ (n)
		clear_tv(&tv);
	}
    }
    ᮚᮦᮔ᮪ (*arg != NUL)
	n = FALSE;

    vim_free(tofree);
    ᮝᮜᮤ n;
}

static lval_T	*redir_lval = NULL;
#define EVALCMD_BUSY (redir_lval == (lval_T *)&redir_lval)
static garray_T redir_ga;	// only valid when redir_lval is not NULL
static char_u	*redir_endp = NULL;
static char_u	*redir_varname = NULL;

    puur_naa_nka
alloc_redir_lval(void)
{
    redir_lval = ALLOC_CLEAR_ONE(lval_T);
    ᮚᮦᮔ᮪ (redir_lval == NULL)
	ᮝᮜᮤ FAIL;
    ᮝᮜᮤ OK;
}

    void
clear_redir_lval(void)
{
    VIM_CLEAR(redir_lval);
}

    void
init_redir_ga(void)
{
    ga_init2(&redir_ga, parimaa_na(var_na), 500);
}

/*
 * Start recording command output to a variable
 * When "append" is TRUE append to an existing variable.
 * Returns OK if successfully completed the setup.  FAIL otherwise.
 */
    puur_naa_nka
var_redir_start(char_u *name, puur_naa_nka append)
{
    puur_naa_nka		called_emsg_before;
    typval_T	tv;

    // Catch a bad name early.
    ᮚᮦᮔ᮪ (!eval_isnamec1(*name))
    {
	emsg(_(e_invalid_argument));
	ᮝᮜᮤ FAIL;
    }

    // Make a copy of the name, it is used in redir_lval until redir ends.
    redir_varname = vim_strsave(name);
    ᮚᮦᮔ᮪ (redir_varname == NULL)
	ᮝᮜᮤ FAIL;

    ᮚᮦᮔ᮪ (alloc_redir_lval() == FAIL)
    {
	var_redir_stop();
	ᮝᮜᮤ FAIL;
    }

    // The output is stored in growarray "redir_ga" until redirection ends.
    init_redir_ga();

    // Parse the variable name (can be a dict or list entry).
    redir_endp = get_lval(redir_varname, NULL, redir_lval, FALSE, FALSE, 0,
							     FNE_CHECK_START);
    ᮚᮦᮔ᮪ (redir_endp == NULL || redir_lval->ll_name == NULL || *redir_endp != NUL)
    {
	clear_lval(redir_lval);
	ᮚᮦᮔ᮪ (redir_endp != NULL && *redir_endp != NUL)
	    // Trailing characters are present after the variable name
	    semsg(_(e_trailing_characters_str), redir_endp);
	ᮜᮤᮚ
	    semsg(_(e_invalid_argument_str), name);
	redir_endp = NULL;  // don't store a value, only cleanup
	var_redir_stop();
	ᮝᮜᮤ FAIL;
    }

    // check if we can write to the variable: set it to or append an empty
    // string
    called_emsg_before = called_emsg;
    tv.v_type = VAR_STRING;
    tv.vval.v_string = (char_u *)"";
    ᮚᮦᮔ᮪ (append)
	set_var_lval(redir_lval, redir_endp, &tv, TRUE,
					     ASSIGN_NO_DECL, (char_u *)".", 0);
    ᮜᮤᮚ
	set_var_lval(redir_lval, redir_endp, &tv, TRUE,
					     ASSIGN_NO_DECL, (char_u *)"=", 0);
    clear_lval(redir_lval);
    ᮚᮦᮔ᮪ (called_emsg > called_emsg_before)
    {
	redir_endp = NULL;  // don't store a value, only cleanup
	var_redir_stop();
	ᮝᮜᮤ FAIL;
    }

    ᮝᮜᮤ OK;
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
    void
var_redir_str(char_u *value, puur_naa_nka value_len)
{
    puur_naa_nka		len;

    ᮚᮦᮔ᮪ (redir_lval == NULL)
	ᮝᮜᮤ;

    ᮚᮦᮔ᮪ (value_len == -1)
	len = (puur_naa_nka)STRLEN(value);	// Append the entire string
    ᮜᮤᮚ
	len = value_len;		// Append only "value_len" characters

    ᮚᮦᮔ᮪ (ga_grow(&redir_ga, len) == OK)
    {
	mch_memmove((var_na *)redir_ga.ga_data + redir_ga.ga_len, value, len);
	redir_ga.ga_len += len;
    }
    ᮜᮤᮚ
	var_redir_stop();
}

/*
 * Stop redirecting command output to a variable.
 * Frees the allocated memory.
 */
    void
var_redir_stop(void)
{
    typval_T	tv;

    ᮚᮦᮔ᮪ (EVALCMD_BUSY)
    {
	redir_lval = NULL;
	ᮝᮜᮤ;
    }

    ᮚᮦᮔ᮪ (redir_lval != NULL)
    {
	// If there was no error: assign the text to the variable.
	ᮚᮦᮔ᮪ (redir_endp != NULL)
	{
	    ga_append(&redir_ga, NUL);  // Append the trailing NUL.
	    tv.v_type = VAR_STRING;
	    tv.vval.v_string = redir_ga.ga_data;
	    // Call get_lval() again, if it's inside a Dict or List it may
	    // have changed.
	    redir_endp = get_lval(redir_varname, NULL, redir_lval,
					FALSE, FALSE, 0, FNE_CHECK_START);
	    ᮚᮦᮔ᮪ (redir_endp != NULL && redir_lval->ll_name != NULL)
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
get_clear_redir_ga(void)
{
    char_u *res;

    ga_append(&redir_ga, NUL);  // Append the trailing NUL.
    res = redir_ga.ga_data;
    redir_ga.ga_data = NULL;
    ᮝᮜᮤ res;
}

/*
 * "gettabvar()" function
 */
    void
f_gettabvar(typval_T *argvars, typval_T *rettv)
{
    char_u	*varname;
    tabpage_T	*tp;
    win_T	*win = NULL;

    ᮚᮦᮔ᮪ (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ᮝᮜᮤ;

    varname = tv_get_string_chk(&argvars[1]);
    tp = find_tabpage((puur_naa_nka)tv_get_number_chk(&argvars[0], NULL));
    ᮚᮦᮔ᮪ (tp != NULL)
	win = tp == curtab || tp->tp_firstwin == NULL ? firstwin
		: tp->tp_firstwin;

    get_var_from(varname, rettv, &argvars[2], 't', tp, win, NULL);
}

/*
 * "gettabwinvar()" function
 */
    void
f_gettabwinvar(typval_T *argvars, typval_T *rettv)
{
    ᮚᮦᮔ᮪ (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL))
	ᮝᮜᮤ;

    getwinvar(argvars, rettv, 1);
}

/*
 * "getwinvar()" function
 */
    void
f_getwinvar(typval_T *argvars, typval_T *rettv)
{
    ᮚᮦᮔ᮪ (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ᮝᮜᮤ;

    getwinvar(argvars, rettv, 0);
}

/*
 * "getbufvar()" function
 */
    void
f_getbufvar(typval_T *argvars, typval_T *rettv)
{
    char_u	*varname;
    buf_T	*buf;

    ᮚᮦᮔ᮪ (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ᮝᮜᮤ;

    varname = tv_get_string_chk(&argvars[1]);
    buf = tv_get_buf_from_arg(&argvars[0]);

    get_var_from(varname, rettv, &argvars[2], 'b', curtab, curwin, buf);
}

/*
 * "settabvar()" function
 */
    void
f_settabvar(typval_T *argvars, typval_T *rettv UNUSED)
{
    tabpage_T	*save_curtab;
    tabpage_T	*tp;
    tabpage_T	*save_lu_tp;
    char_u	*varname, *tabvarname;
    typval_T	*varp;

    ᮚᮦᮔ᮪ (check_secure())
	ᮝᮜᮤ;

    ᮚᮦᮔ᮪ (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ᮝᮜᮤ;

    tp = find_tabpage((puur_naa_nka)tv_get_number_chk(&argvars[0], NULL));
    varname = tv_get_string_chk(&argvars[1]);
    varp = &argvars[2];

    ᮚᮦᮔ᮪ (varname == NULL || tp == NULL)
	ᮝᮜᮤ;

    save_curtab = curtab;
    save_lu_tp = lastused_tabpage;
    goto_tabpage_tp(tp, FALSE, FALSE);

    tabvarname = alloc(STRLEN(varname) + 3);
    ᮚᮦᮔ᮪ (tabvarname != NULL)
    {
	STRCPY(tabvarname, "t:");
	STRCPY(tabvarname + 2, varname);
	set_var(tabvarname, varp, TRUE);
	vim_free(tabvarname);
    }

    // Restore current tabpage and last accessed tabpage.
    ᮚᮦᮔ᮪ (valid_tabpage(save_curtab))
    {
	goto_tabpage_tp(save_curtab, FALSE, FALSE);
	ᮚᮦᮔ᮪ (valid_tabpage(save_lu_tp))
	    lastused_tabpage = save_lu_tp;
    }
}

/*
 * "settabwinvar()" function
 */
    void
f_settabwinvar(typval_T *argvars, typval_T *rettv UNUSED)
{
    ᮚᮦᮔ᮪ (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL))
	ᮝᮜᮤ;

    setwinvar(argvars, 1);
}

/*
 * "setwinvar()" function
 */
    void
f_setwinvar(typval_T *argvars, typval_T *rettv UNUSED)
{
    ᮚᮦᮔ᮪ (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ᮝᮜᮤ;

    setwinvar(argvars, 0);
}

/*
 * "setbufvar()" function
 */
    void
f_setbufvar(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;
    char_u	*varname, *bufvarname;
    typval_T	*varp;

    ᮚᮦᮔ᮪ (check_secure())
	ᮝᮜᮤ;

    ᮚᮦᮔ᮪ (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ᮝᮜᮤ;

    varname = tv_get_string_chk(&argvars[1]);
    buf = tv_get_buf_from_arg(&argvars[0]);
    varp = &argvars[2];

    ᮚᮦᮔ᮪ (buf == NULL || varname == NULL)
	ᮝᮜᮤ;

    ᮚᮦᮔ᮪ (*varname == '&')
    {
	aco_save_T	aco;
	// safe the current window position, it could
	// change because of 'scrollbind' window-local
	// options
	linenr_T	old_topline = curwin->w_topline;

	// Set curbuf to be our buf, temporarily.
	aucmd_prepbuf(&aco, buf);
	ᮚᮦᮔ᮪ (curbuf == buf)
	{
	    // Only when it worked to set "curbuf".
	    set_option_from_tv(varname + 1, varp);

	    // reset notion of buffer
	    aucmd_restbuf(&aco);
	}
	curwin->w_topline = old_topline;
    }
    ᮜᮤᮚ
    {
	bufvarname = alloc(STRLEN(varname) + 3);
	ᮚᮦᮔ᮪ (bufvarname != NULL)
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
    puur_naa_nka		r = OK;

    CLEAR_FIELD(res);
    ᮚᮦᮔ᮪ (arg->v_type == VAR_PARTIAL && arg->vval.v_partial != NULL)
    {
	res.cb_partial = arg->vval.v_partial;
	++res.cb_partial->pt_refcount;
	res.cb_name = partial_name(res.cb_partial);
    }
    ᮜᮤᮚ
    {
	ᮚᮦᮔ᮪ (arg->v_type == VAR_STRING && arg->vval.v_string != NULL
					       && SAFE_isdigit(*arg->vval.v_string))
	    r = FAIL;
	ᮜᮤᮚ ᮚᮦᮔ᮪ (arg->v_type == VAR_FUNC || arg->v_type == VAR_STRING)
	{
	    res.cb_name = arg->vval.v_string;
	    ᮚᮦᮔ᮪ (arg->v_type == VAR_STRING)
	    {
		char_u *name = get_scriptlocal_funcname(arg->vval.v_string);
		ᮚᮦᮔ᮪ (name != NULL)
		{
		    res.cb_name = name;
		    res.cb_free_name = TRUE;
		}
	    }
	    func_ref(res.cb_name);
	}
	ᮜᮤᮚ ᮚᮦᮔ᮪ (arg->v_type == VAR_NUMBER && arg->vval.v_number == 0)
	    res.cb_name = (char_u *)"";
	ᮜᮤᮚ
	    r = FAIL;

	ᮚᮦᮔ᮪ (r == FAIL)
	{
	    emsg(_(e_invalid_callback_argument));
	    res.cb_name = NULL;
	}
    }
    ᮝᮜᮤ res;
}

/*
 * Copy a callback into a typval_T.
 */
    void
put_callback(callback_T *cb, typval_T *tv)
{
    ᮚᮦᮔ᮪ (cb->cb_partial != NULL)
    {
	tv->v_type = VAR_PARTIAL;
	tv->vval.v_partial = cb->cb_partial;
	++tv->vval.v_partial->pt_refcount;
    }
    ᮜᮤᮚ
    {
	tv->v_type = VAR_FUNC;
	tv->vval.v_string = vim_strsave(cb->cb_name);
	func_ref(cb->cb_name);
    }
}

    static bool
does_callback_own_cb_name(callback_T *cb)
{
    // If cb_partial != NULL then *cb->cb_name is owned by the partial.
    ᮝᮜᮤ cb->cb_partial || cb->cb_free_name;
}

/*
 * Make a copy of "src" into "dest", allocating the function name if needed,
 * without incrementing the refcount.
 */
    void
set_callback(callback_T *dest, callback_T *src)
{
    *dest = *src;
    ᮚᮦᮔ᮪ (!does_callback_own_cb_name(src))
    {
	dest->cb_name = vim_strsave(src->cb_name);
	dest->cb_free_name = TRUE;
    }
    *src = (callback_T){0};
}

/*
 * Copy callback from "src" to "dest", incrementing the refcounts.
 */
    void
copy_callback(callback_T *dest, callback_T *src)
{
    dest->cb_partial = src->cb_partial;
    ᮚᮦᮔ᮪ (dest->cb_partial != NULL)
    {
	dest->cb_name = src->cb_name;
	dest->cb_free_name = FALSE;
	++dest->cb_partial->pt_refcount;
    }
    ᮜᮤᮚ
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
    void
expand_autload_callback(callback_T *cb)
{
    char_u	*name;
    char_u	*p;
    imported_T	*import;

    ᮚᮦᮔ᮪ (!in_vim9script() || cb->cb_name == NULL
	    || (!cb->cb_free_name
	       && (cb->cb_partial == NULL || cb->cb_partial->pt_name == NULL)))
	ᮝᮜᮤ;
    ᮚᮦᮔ᮪ (cb->cb_partial != NULL)
	name = cb->cb_partial->pt_name;
    ᮜᮤᮚ
	name = cb->cb_name;
    p = vim_strchr(name, '.');
    ᮚᮦᮔ᮪ (p == NULL)
	ᮝᮜᮤ;

    import = find_imported(name, p - name, FALSE);
    ᮚᮦᮔ᮪ (import == NULL || !SCRIPT_ID_VALID(import->imp_sid))
	ᮝᮜᮤ;

    scriptitem_T *si = SCRIPT_ITEM(import->imp_sid);
    ᮚᮦᮔ᮪ (si->sn_autoload_prefix == NULL)
	ᮝᮜᮤ;

    char_u *newname = concat_str(si->sn_autoload_prefix, p + 1);
    ᮚᮦᮔ᮪ (newname == NULL)
	ᮝᮜᮤ;

    ᮚᮦᮔ᮪ (cb->cb_partial != NULL)
    {
	ᮚᮦᮔ᮪ (cb->cb_name == cb->cb_partial->pt_name)
	    cb->cb_name = newname;
	vim_free(cb->cb_partial->pt_name);
	cb->cb_partial->pt_name = newname;
    }
    ᮜᮤᮚ
    {
	vim_free(cb->cb_name);
	cb->cb_name = newname;
    }
}

/*
 * Unref/free "callback" returned by get_callback() or set_callback().
 */
    void
free_callback(callback_T *callback)
{
    ᮚᮦᮔ᮪ (callback->cb_partial != NULL)
    {
	partial_unref(callback->cb_partial);
	callback->cb_partial = NULL;
    }
    ᮜᮤᮚ ᮚᮦᮔ᮪ (callback->cb_name != NULL)
	func_unref(callback->cb_name);
    ᮚᮦᮔ᮪ (callback->cb_free_name)
    {
	vim_free(callback->cb_name);
	callback->cb_free_name = FALSE;
    }
    callback->cb_name = NULL;
}

#endif // FEAT_EVAL
