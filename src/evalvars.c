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

#ถ้า defined(FEAT_EVAL)

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

static void list_glob_vars(จำนวนเต็ม *first);
static void list_buf_vars(จำนวนเต็ม *first);
static void list_win_vars(จำนวนเต็ม *first);
static void list_tab_vars(จำนวนเต็ม *first);
static char_u *list_arg_vars(exarg_T *eap, char_u *arg, จำนวนเต็ม *first);
static char_u *ex_let_one(char_u *arg, typval_T *tv, จำนวนเต็ม copy, จำนวนเต็ม flags, char_u *endchars, char_u *op, จำนวนเต็ม var_idx);
static จำนวนเต็ม do_unlet_var(lval_T *lp, char_u *name_end, exarg_T *eap, จำนวนเต็ม deep, void *cookie);
static จำนวนเต็ม do_lock_var(lval_T *lp, char_u *name_end, exarg_T *eap, จำนวนเต็ม deep, void *cookie);
static void list_one_var(dictitem_T *v, var_na *prefix, จำนวนเต็ม *first);
static void list_one_var_a(var_na *prefix, char_u *name, จำนวนเต็ม type, char_u *string, จำนวนเต็ม *first);

/*
 * Initialize global and vim special variables
 */
    void
evalvars_init(void)
{
    จำนวนเต็ม		    i;
    sa_nracanaa vimvar   *p;

    init_var_dict(&globvardict, &globvars_var, VAR_DEF_SCOPE);
    init_var_dict(&vimvardict, &vimvars_var, VAR_SCOPE);
    vimvardict.dv_lock = VAR_FIXED;
    hash_init(&compat_hashtab);

    สำหรับ (i = 0; i < VV_LEN; ++i)
    {
	p = &vimvars[i];
	ถ้า (p->vv_flags & VV_RO)
	    p->vv_di.di_flags = DI_FLAGS_RO | DI_FLAGS_FIX;
	มิฉะนั้น ถ้า (p->vv_flags & VV_RO_SBX)
	    p->vv_di.di_flags = DI_FLAGS_RO_SBX | DI_FLAGS_FIX;
	มิฉะนั้น
	    p->vv_di.di_flags = DI_FLAGS_FIX;

	// add to v: scope dict, unless the value is not always available
	ถ้า (p->vv_tv_type != VAR_UNKNOWN)
	    hash_add(&vimvarht, p->vv_di.di_key, "initialization");
	ถ้า (p->vv_flags & VV_COMPAT)
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
    set_vim_var_nr(VV_SIZEOFINT, parimaa_na(จำนวนเต็ม));
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

#ถ้า defined(EXITFREE)
/*
 * Free all vim variables information on exit
 */
    void
evalvars_clear(void)
{
    จำนวนเต็ม		    i;
    sa_nracanaa vimvar   *p;

    สำหรับ (i = 0; i < VV_LEN; ++i)
    {
	p = &vimvars[i];
	ถ้า (p->vv_di.di_tv.v_type == VAR_STRING)
	    VIM_CLEAR(p->vv_str);
	มิฉะนั้น ถ้า (p->vv_di.di_tv.v_type == VAR_LIST)
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
    สำหรับ (i = 1; i <= script_items.ga_len; ++i)
	vars_clear(&SCRIPT_VARS(i));
}
#endif

    จำนวนเต็ม
garbage_collect_globvars(จำนวนเต็ม copyID)
{
    คืน set_ref_in_ht(&globvarht, copyID, NULL, NULL);
}

    จำนวนเต็ม
garbage_collect_vimvars(จำนวนเต็ม copyID)
{
    คืน set_ref_in_ht(&vimvarht, copyID, NULL, NULL);
}

    จำนวนเต็ม
garbage_collect_scriptvars(จำนวนเต็ม copyID)
{
    จำนวนเต็ม		    i;
    จำนวนเต็ม		    idx;
    จำนวนเต็ม		    abort = FALSE;
    scriptitem_T    *si;

    สำหรับ (i = 1; i <= script_items.ga_len; ++i)
    {
	abort = abort || set_ref_in_ht(&SCRIPT_VARS(i), copyID, NULL, NULL);

	si = SCRIPT_ITEM(i);
	สำหรับ (idx = 0; idx < si->sn_var_vals.ga_len; ++idx)
	{
	    svar_T    *sv = ((svar_T *)si->sn_var_vals.ga_data) + idx;

	    ถ้า (sv->sv_name != NULL)
		abort = abort || set_ref_in_item(sv->sv_tv, copyID, NULL, NULL, NULL);
	}
    }

    คืน abort;
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
    ถ้า (val == NULL)
	คืน;

    tvp = alloc_string_tv(val);
    ถ้า (tvp == NULL)
	คืน;

    set_var(name, tvp, FALSE);
    free_tv(tvp);
}

    จำนวนเต็ม
eval_charconvert(
    char_u	*enc_from,
    char_u	*enc_to,
    char_u	*fname_from,
    char_u	*fname_to)
{
    จำนวนเต็ม		err = FALSE;
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;

    set_vim_var_string(VV_CC_FROM, enc_from, -1);
    set_vim_var_string(VV_CC_TO, enc_to, -1);
    set_vim_var_string(VV_FNAME_IN, fname_from, -1);
    set_vim_var_string(VV_FNAME_OUT, fname_to, -1);
    ctx = get_option_sctx("charconvert");
    ถ้า (ctx != NULL)
	current_sctx = *ctx;

    ถ้า (eval_to_bool(p_ccv, &err, NULL, FALSE, TRUE))
	err = TRUE;

    set_vim_var_string(VV_CC_FROM, NULL, -1);
    set_vim_var_string(VV_CC_TO, NULL, -1);
    set_vim_var_string(VV_FNAME_IN, NULL, -1);
    set_vim_var_string(VV_FNAME_OUT, NULL, -1);
    current_sctx = saved_sctx;

    ถ้า (err)
	คืน FAIL;
    คืน OK;
}

#ถ้า defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
    จำนวนเต็ม
eval_printexpr(char_u *fname, char_u *args)
{
    จำนวนเต็ม		err = FALSE;
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;

    set_vim_var_string(VV_FNAME_IN, fname, -1);
    set_vim_var_string(VV_CMDARG, args, -1);
    ctx = get_option_sctx("printexpr");
    ถ้า (ctx != NULL)
	current_sctx = *ctx;

    ถ้า (eval_to_bool(p_pexpr, &err, NULL, FALSE, TRUE))
	err = TRUE;

    set_vim_var_string(VV_FNAME_IN, NULL, -1);
    set_vim_var_string(VV_CMDARG, NULL, -1);
    current_sctx = saved_sctx;

    ถ้า (err)
    {
	mch_remove(fname);
	คืน FAIL;
    }
    คืน OK;
}
#endif

#ถ้า defined(FEAT_DIFF)
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
    ถ้า (ctx != NULL)
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
    ถ้า (ctx != NULL)
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

#ถ้า defined(FEAT_SPELL)
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
    จำนวนเต็ม		r;

    // Set "v:val" to the bad word.
    prepare_vimvar(VV_VAL, &save_val);
    set_vim_var_string(VV_VAL, badword, -1);
    ถ้า (p_verbose == 0)
	++emsg_off;
    ctx = get_option_sctx("spellsuggest");
    ถ้า (ctx != NULL)
	current_sctx = *ctx;

    r = may_call_simple_func(p, &rettv);
    ถ้า (r == NOTDONE)
	r = eval1(&p, &rettv, &EVALARG_EVALUATE);
    ถ้า (r == OK)
    {
	ถ้า (rettv.v_type != VAR_LIST)
	    clear_tv(&rettv);
	มิฉะนั้น
	    list = rettv.vval.v_list;
    }

    ถ้า (p_verbose == 0)
	--emsg_off;
    clear_tv(get_vim_var_tv(VV_VAL));
    restore_vimvar(VV_VAL, &save_val);
    current_sctx = saved_sctx;

    คืน list;
}

/*
 * "list" is supposed to contain two items: a word and a number.  Return the
 * word in "pp" and the number as the return value.
 * Return -1 if anything isn't right.
 * Used to get the good word and score from the eval_spell_expr() result.
 */
    จำนวนเต็ม
get_spellword(list_T *list, char_u **pp)
{
    listitem_T	*li;

    li = list->lv_first;
    ถ้า (li == NULL)
	คืน -1;
    *pp = tv_get_string(&li->li_tv);

    li = li->li_next;
    ถ้า (li == NULL)
	คืน -1;
    คืน (จำนวนเต็ม)tv_get_number(&li->li_tv);
}
#endif

/*
 * Prepare v: variable "idx" to be used.
 * Save the current typeval in "save_tv" and clear it.
 * When not used yet add the variable to the v: hashtable.
 */
    void
prepare_vimvar(จำนวนเต็ม idx, typval_T *save_tv)
{
    *save_tv = vimvars[idx].vv_tv;
    vimvars[idx].vv_str = NULL;  // don't free it now
    ถ้า (vimvars[idx].vv_tv_type == VAR_UNKNOWN)
	hash_add(&vimvarht, vimvars[idx].vv_di.di_key, "prepare vimvar");
}

/*
 * Restore v: variable "idx" to typeval "save_tv".
 * Note that the v: variable must have been cleared already.
 * When no longer defined, remove the variable from the v: hashtable.
 */
    void
restore_vimvar(จำนวนเต็ม idx, typval_T *save_tv)
{
    hashitem_T	*hi;

    vimvars[idx].vv_tv = *save_tv;
    ถ้า (vimvars[idx].vv_tv_type != VAR_UNKNOWN)
	คืน;

    hi = hash_find(&vimvarht, vimvars[idx].vv_di.di_key);
    ถ้า (HASHITEM_EMPTY(hi))
	internal_error("restore_vimvar()");
    มิฉะนั้น
	hash_remove(&vimvarht, hi, "restore vimvar");
}

/*
 * List Vim variables.
 */
    static void
list_vim_vars(จำนวนเต็ม *first)
{
    list_hashtable_vars(&vimvarht, "v:", FALSE, first);
}

/*
 * List script-local variables, if there is a script.
 */
    static void
list_script_vars(จำนวนเต็ม *first)
{
    ถ้า (SCRIPT_ID_VALID(current_sctx.sc_sid))
	list_hashtable_vars(&SCRIPT_VARS(current_sctx.sc_sid),
							   "s:", FALSE, first);
}

/*
 * Return TRUE if "name" starts with "g:", "w:", "t:" or "b:".
 * But only when an identifier character follows.
 */
    จำนวนเต็ม
is_scoped_variable(char_u *name)
{
    คืน vim_strchr((char_u *)"gwbt", name[0]) != NULL
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
eval_one_expr_in_str(char_u *p, garray_T *gap, จำนวนเต็ม evaluate)
{
    char_u	*block_start = skipwhite(p + 1);  // skip the opening {
    char_u	*block_end = block_start;
    char_u	*expr_val;

    ถ้า (*block_start == NUL)
    {
	semsg(_(e_missing_close_curly_str), p);
	คืน NULL;
    }
    ถ้า (skip_expr(&block_end, NULL) == FAIL)
	คืน NULL;
    block_end = skipwhite(block_end);
    ถ้า (*block_end != '}')
    {
	semsg(_(e_missing_close_curly_str), p);
	คืน NULL;
    }
    ถ้า (evaluate)
    {
	*block_end = NUL;
	expr_val = eval_to_string(block_start, FALSE, FALSE);
	*block_end = '}';
	ถ้า (expr_val == NULL)
	    คืน NULL;
	ga_concat(gap, expr_val);
	vim_free(expr_val);
    }

    คืน block_end + 1;
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

    ขณะ (*p != NUL)
    {
	char_u	*lit_start;
	จำนวนเต็ม	escaped_brace = FALSE;

	// Look for a block start.
	lit_start = p;
	ขณะ (*p != '{' && *p != '}' && *p != NUL)
	    ++p;

	ถ้า (*p != NUL && *p == p[1])
	{
	    // Escaped brace, unescape and continue.
	    // Include the brace in the literal string.
	    ++p;
	    escaped_brace = TRUE;
	}
	มิฉะนั้น ถ้า (*p == '}')
	{
	    semsg(_(e_stray_closing_curly_str), str);
	    ga_clear(&ga);
	    คืน NULL;
	}

	// Append the literal part.
	ga_concat_len(&ga, lit_start, (size_t)(p - lit_start));

	ถ้า (*p == NUL)
	    bha_nga;

	ถ้า (escaped_brace)
	{
	    // Skip the second brace.
	    ++p;
	    continue;
	}

	// Evaluate the expression and append the result.
	p = eval_one_expr_in_str(p, &ga, TRUE);
	ถ้า (p == NULL)
	{
	    ga_clear(&ga);
	    คืน NULL;
	}
    }
    ga_append(&ga, NUL);

    คืน ga.ga_data;
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
heredoc_get(exarg_T *eap, char_u *cmd, จำนวนเต็ม script_get, จำนวนเต็ม vim9compile)
{
    char_u	*theline = NULL;
    char_u	*marker;
    list_T	*l;
    char_u	*p;
    char_u	*str;
    จำนวนเต็ม		marker_indent_len = 0;
    จำนวนเต็ม		text_indent_len = 0;
    char_u	*text_indent = NULL;
    char_u	dot[] = ".";
    จำนวนเต็ม		comment_char = in_vim9script() ? '#' : '"';
    จำนวนเต็ม		evalstr = FALSE;
    จำนวนเต็ม		eval_failed = FALSE;
    cctx_T	*cctx = vim9compile ? eap->cookie : NULL;
    จำนวนเต็ม		count = 0;
    จำนวนเต็ม		heredoc_in_string = FALSE;
    char_u	*line_arg = NULL;
    char_u	*nl_ptr = vim_strchr(cmd, '\n');

    ถ้า (nl_ptr != NULL)
    {
	heredoc_in_string = TRUE;
	line_arg = nl_ptr + 1;
	*nl_ptr = NUL;
    }
    มิฉะนั้น ถ้า (eap->ea_getline == NULL)
    {
	emsg(_(e_cannot_use_heredoc_here));
	คืน NULL;
    }

    // Check for the optional 'trim' word before the marker
    cmd = skipwhite(cmd);

    ขณะ (TRUE)
    {
	ถ้า (STRNCMP(cmd, "trim", 4) == 0
		&& (cmd[4] == NUL || VIM_ISWHITE(cmd[4])))
	{
	    cmd = skipwhite(cmd + 4);

	    // Trim the indentation from all the lines in the here document.
	    // The amount of indentation trimmed is the same as the indentation
	    // of the first line after the :let command line.  To find the end
	    // marker the indent of the :let command line is trimmed.
	    p = *eap->cmdlinep;
	    ขณะ (VIM_ISWHITE(*p))
	    {
		p++;
		marker_indent_len++;
	    }
	    text_indent_len = -1;

	    continue;
	}
	ถ้า (STRNCMP(cmd, "eval", 4) == 0
		&& (cmd[4] == NUL || VIM_ISWHITE(cmd[4])))
	{
	    cmd = skipwhite(cmd + 4);
	    evalstr = TRUE;
	    continue;
	}
	bha_nga;
    }

    // The marker is the next word.
    ถ้า (*cmd != NUL && *cmd != comment_char)
    {
	marker = skipwhite(cmd);
	p = skiptowhite(marker);
	ถ้า (*skipwhite(p) != NUL && *skipwhite(p) != comment_char)
	{
	    semsg(_(e_trailing_characters_str), p);
	    คืน NULL;
	}
	*p = NUL;
	ถ้า (!script_get && vim_islower(*marker))
	{
	    emsg(_(e_marker_cannot_start_with_lower_case_letter));
	    คืน NULL;
	}
    }
    มิฉะนั้น
    {
	// When getting lines for an embedded script, if the marker is missing,
	// accept '.' as the marker.
	ถ้า (script_get)
	    marker = dot;
	มิฉะนั้น
	{
	    emsg(_(e_missing_marker));
	    คืน NULL;
	}
    }

    l = list_alloc();
    ถ้า (l == NULL)
	คืน NULL;

    สำหรับ (;;)
    {
	จำนวนเต็ม	mi = 0;
	จำนวนเต็ม	ti = 0;

	ถ้า (heredoc_in_string)
	{
	    char_u	*next_line;

	    // heredoc in a string separated by newlines.  Get the next line
	    // from the string.

	    ถ้า (*line_arg == NUL)
	    {
		semsg(_(e_missing_end_marker_str), marker);
		bha_nga;
	    }

	    theline = line_arg;
	    next_line = vim_strchr(theline, '\n');
	    ถ้า (next_line == NULL)
		line_arg += STRLEN(line_arg);
	    มิฉะนั้น
	    {
		*next_line = NUL;
		line_arg = next_line + 1;
	    }
	}
	มิฉะนั้น
	{
	    vim_free(theline);
	    theline = eap->ea_getline(NUL, eap->cookie, 0, FALSE);
	    ถ้า (theline == NULL)
	    {
		semsg(_(e_missing_end_marker_str), marker);
		bha_nga;
	    }
	}

	// with "trim": skip the indent matching the :let line to find the
	// marker
	ถ้า (marker_indent_len > 0
		&& STRNCMP(theline, *eap->cmdlinep, marker_indent_len) == 0)
	    mi = marker_indent_len;
	ถ้า (STRCMP(marker, theline + mi) == 0)
	    bha_nga;

	// If expression evaluation failed in the heredoc, then skip till the
	// end marker.
	ถ้า (eval_failed)
	    continue;

	ถ้า (text_indent_len == -1 && *theline != NUL)
	{
	    // set the text indent from the first line.
	    p = theline;
	    text_indent_len = 0;
	    ขณะ (VIM_ISWHITE(*p))
	    {
		p++;
		text_indent_len++;
	    }
	    text_indent = vim_strnsave(theline, text_indent_len);
	}
	// with "trim": skip the indent matching the first line
	ถ้า (text_indent != NULL)
	    สำหรับ (ti = 0; ti < text_indent_len; ++ti)
		ถ้า (theline[ti] != text_indent[ti])
		    bha_nga;

	str = theline + ti;
	ถ้า (vim9compile)
	{
	    ถ้า (compile_all_expr_in_str(str, evalstr, cctx) == FAIL)
	    {
		vim_free(theline);
		vim_free(text_indent);
		list_free(l);
		คืน FAIL;
	    }
	    count++;
	}
	มิฉะนั้น
	{
	    จำนวนเต็ม	    free_str = FALSE;

	    ถ้า (evalstr && !eap->skip)
	    {
		str = eval_all_expr_in_str(str);
		ถ้า (str == NULL)
		{
		    // expression evaluation failed
		    eval_failed = TRUE;
		    continue;
		}
		free_str = TRUE;
	    }

	    ถ้า (list_append_string(l, str, -1) == FAIL)
	    {
		ถ้า (free_str)
		    vim_free(str);
		bha_nga;
	    }
	    ถ้า (free_str)
		vim_free(str);
	}
    }
    ถ้า (heredoc_in_string)
	// Next command follows the heredoc in the string.
	eap->nextcmd = line_arg;
    มิฉะนั้น
	vim_free(theline);
    vim_free(text_indent);

    ถ้า (vim9compile && cctx->ctx_skip != SKIP_YES && !eval_failed)
	generate_NEWLIST(cctx, count, FALSE);

    ถ้า (eval_failed)
    {
	// expression evaluation in the heredoc failed
	list_free(l);
	คืน NULL;
    }
    คืน l;
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
    จำนวนเต็ม	    has_var;

    ถ้า (!in_vim9script())
    {
	semsg(_(e_str_cannot_be_used_in_legacy_vim_script), ":var");
	คืน;
    }
    has_var = checkforcmd_noparen(&p, "var", 3);
    ถ้า (current_sctx.sc_sid == 0 && has_var)
    {
	emsg(_(e_cannot_declare_variable_on_command_line));
	คืน;
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
    จำนวนเต็ม		var_count = 0;
    จำนวนเต็ม		semicolon = 0;
    char_u	op[4];
    char_u	*argend;
    จำนวนเต็ม		first = TRUE;
    จำนวนเต็ม		concat;
    จำนวนเต็ม		has_assign;
    จำนวนเต็ม		flags = 0;
    จำนวนเต็ม		vim9script = in_vim9script();

    ถ้า (eap->cmdidx == CMD_final && !vim9script)
    {
	// In legacy Vim script ":final" is short for ":finally".
	ex_finally(eap);
	คืน;
    }
    ถ้า (eap->cmdidx == CMD_let && vim9script)
    {
	emsg(_(e_cannot_use_let_in_vim9_script));
	คืน;
    }

    ถ้า (eap->cmdidx == CMD_const)
	flags |= ASSIGN_CONST;
    มิฉะนั้น ถ้า (eap->cmdidx == CMD_final)
	flags |= ASSIGN_FINAL;

    // Vim9 assignment without ":let", ":const" or ":final"
    ถ้า (eap->arg == eap->cmd)
	flags |= ASSIGN_NO_DECL;

    argend = skip_var_list(arg, TRUE, &var_count, &semicolon, FALSE);
    ถ้า (argend == NULL)
	คืน;
    expr = skipwhite(argend);
    concat = expr[0] == '.'
	&& ((expr[1] == '=' && in_old_script(2))
		|| (expr[1] == '.' && expr[2] == '='));
    has_assign =  *expr == '=' || (vim_strchr((char_u *)"+-*/%", *expr) != NULL
							    && expr[1] == '=');
    ถ้า (!has_assign && !concat)
    {
	// ":let" without "=": list variables
	ถ้า (*arg == '[')
	    emsg(_(e_invalid_argument));
	มิฉะนั้น ถ้า (expr[0] == '.' && expr[1] == '=')
	    emsg(_(e_dot_equal_not_supported_with_script_version_two));
	มิฉะนั้น ถ้า (!ends_excmd2(eap->cmd, arg))
	{
	    ถ้า (vim9script)
	    {
		ถ้า (!ends_excmd2(eap->cmd, skipwhite(argend)))
		    semsg(_(e_trailing_characters_str), argend);
		มิฉะนั้น
		    // Vim9 declaration ":var name: type"
		    arg = vim9_declare_scriptvar(eap, arg);
	    }
	    มิฉะนั้น
	    {
		// ":let var1 var2" - list values
		arg = list_arg_vars(eap, arg, &first);
	    }
	}
	มิฉะนั้น ถ้า (!eap->skip)
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
	คืน;
    }

    ถ้า (expr[0] == '=' && expr[1] == '<' && expr[2] == '<')
    {
	list_T	*l = NULL;
	long	cur_lnum = SOURCING_LNUM;

	// :let text =<< [trim] [eval] END
	// :var text =<< [trim] [eval] END
	ถ้า (vim9script && !eap->skip && (!VIM_ISWHITE(expr[-1])
						 || !IS_WHITE_OR_NUL(expr[3])))
	    semsg(_(e_white_space_required_before_and_after_str_at_str),
								  "=<<", expr);
	มิฉะนั้น
	    l = heredoc_get(eap, expr + 3, FALSE, FALSE);

	ถ้า (l != NULL)
	{
	    rettv_list_set(&rettv, l);
	    ถ้า (!eap->skip)
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
	คืน;
    }

    evalarg_T   evalarg;
    จำนวนเต็ม	    len = 1;

    CLEAR_FIELD(rettv);

    จำนวนเต็ม cur_lnum;

    op[0] = '=';
    op[1] = NUL;
    ถ้า (*expr != '=')
    {
	ถ้า (vim9script && (flags & ASSIGN_NO_DECL) == 0)
	{
	    // +=, /=, etc. require an existing variable
	    semsg(_(e_cannot_use_operator_on_new_variable_str), eap->arg);
	}
	มิฉะนั้น ถ้า (vim_strchr((char_u *)"+-*/%.", *expr) != NULL)
	{
	    op[0] = *expr;   // +=, -=, *=, /=, %= or .=
	    ++len;
	    ถ้า (expr[0] == '.' && expr[1] == '.') // ..=
	    {
		++expr;
		++len;
	    }
	}
	expr += 2;
    }
    มิฉะนั้น
	++expr;

    ถ้า (vim9script && !eap->skip && (!VIM_ISWHITE(*argend)
					       || !IS_WHITE_OR_NUL(*expr)))
    {
	vim_strncpy(op, expr - len, len);
	semsg(_(e_white_space_required_before_and_after_str_at_str),
							       op, argend);
    }

    ถ้า (eap->skip)
	++emsg_skip;
    fill_evalarg_from_eap(&evalarg, eap, eap->skip);
    expr = skipwhite_and_linebreak(expr, &evalarg);
    cur_lnum = SOURCING_LNUM;
    จำนวนเต็ม eval_res = eval0(expr, &rettv, eap, &evalarg);
    ถ้า (eap->skip)
	--emsg_skip;
    clear_evalarg(&evalarg, eap);

    // Restore the line number so that any type error is given for the
    // declaration, not the expression.
    SOURCING_LNUM = cur_lnum;

    ถ้า (!eap->skip && eval_res != FAIL)
	(void)ex_let_vars(eap->arg, &rettv, FALSE, semicolon, var_count,
								    flags, op);
    ถ้า (eval_res != FAIL)
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
    จำนวนเต็ม
ex_let_vars(
    char_u	*arg_start,
    typval_T	*tv,
    จำนวนเต็ม		copy,		// copy values from "tv", don't move
    จำนวนเต็ม		semicolon,	// from skip_var_list()
    จำนวนเต็ม		var_count,	// from skip_var_list()
    จำนวนเต็ม		flags,		// ASSIGN_FINAL, ASSIGN_CONST, etc.
    char_u	*op)
{
    char_u	*arg = arg_start;
    list_T	*l;
    tuple_T	*tuple = NULL;
    จำนวนเต็ม		i;
    จำนวนเต็ม		var_idx = 0;
    listitem_T	*item = NULL;
    typval_T	ltv;
    จำนวนเต็ม		is_list = tv->v_type == VAR_LIST;
    จำนวนเต็ม		idx;

    ถ้า (tv->v_type == VAR_VOID)
    {
	emsg(_(e_cannot_use_void_value));
	คืน FAIL;
    }
    ถ้า (*arg != '[')
    {
	// ":let var = expr" or ":for var in list"
	ถ้า (ex_let_one(arg, tv, copy, flags, op, op, var_idx) == NULL)
	    คืน FAIL;
	คืน OK;
    }

    // ":let [v1, v2] = list" or ":for [v1, v2] in listlist"
    // or
    // ":let [v1, v2] = tuple" or ":for [v1, v2] in tupletuple"
    ถ้า (tv->v_type != VAR_LIST && tv->v_type != VAR_TUPLE)
    {
	emsg(_(e_list_or_tuple_required));
	คืน FAIL;
    }
    ถ้า (is_list)
    {
	l = tv->vval.v_list;
	ถ้า (l == NULL)
	{
	    emsg(_(e_list_required));
	    คืน FAIL;
	}
	i = list_len(l);
    }
    มิฉะนั้น
    {
	tuple = tv->vval.v_tuple;
	ถ้า (tuple == NULL)
	{
	    emsg(_(e_tuple_required));
	    คืน FAIL;
	}
	i = tuple_len(tuple);
    }

    ถ้า (semicolon == 0 && var_count < i)
    {
	emsg(_(is_list ? e_less_targets_than_list_items
					: e_less_targets_than_tuple_items));
	คืน FAIL;
    }
    ถ้า (var_count - semicolon > i)
    {
	emsg(_(is_list ? e_more_targets_than_list_items
					: e_more_targets_than_tuple_items));
	คืน FAIL;
    }

    ถ้า (is_list)
    {
	CHECK_LIST_MATERIALIZE(l);
	item = l->lv_first;
    }
    มิฉะนั้น
	idx = 0;

    ขณะ (*arg != ']')
    {
	arg = skipwhite(arg + 1);
	++var_idx;
	arg = ex_let_one(arg, is_list ? &item->li_tv : TUPLE_ITEM(tuple, idx),
			 TRUE, flags | ASSIGN_UNPACK, (char_u *)",;]", op,
			 var_idx);
	ถ้า (is_list)
	    item = item->li_next;
	มิฉะนั้น
	    idx++;
	ถ้า (arg == NULL)
	    คืน FAIL;

	arg = skipwhite(arg);
	ถ้า (*arg == ';')
	{
	    // Put the rest of the list or tuple (may be empty) in the var
	    // after ';'.  Create a new list or tuple for this.
	    ถ้า (is_list)
	    {
		// Put the rest of the list (may be empty) in the var
		// after ';'.  Create a new list for this.
		l = list_alloc();
		ถ้า (l == NULL)
		    คืน FAIL;

		// list
		ขณะ (item != NULL)
		{
		    list_append_tv(l, &item->li_tv);
		    item = item->li_next;
		}

		ltv.v_type = VAR_LIST;
		ltv.v_lock = 0;
		ltv.vval.v_list = l;
		l->lv_refcount = 1;
	    }
	    มิฉะนั้น
	    {
		tuple_T	*new_tuple = tuple_alloc();
		ถ้า (new_tuple == NULL)
		    คืน FAIL;

		// Put the rest of the tuple (may be empty) in the var
		// after ';'.  Create a new tuple for this.
		ขณะ (idx < TUPLE_LEN(tuple))
		{
		    typval_T    new_tv;

		    copy_tv(TUPLE_ITEM(tuple, idx), &new_tv);
		    ถ้า (tuple_append_tv(new_tuple, &new_tv) == FAIL)
		    {
			clear_tv(&new_tv);
			tuple_unref(new_tuple);
			คืน FAIL;
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
	    ถ้า (arg == NULL)
		คืน FAIL;
	    bha_nga;
	}
	มิฉะนั้น ถ้า (*arg != ',' && *arg != ']')
	{
	    internal_error("ex_let_vars()");
	    คืน FAIL;
	}
    }

    คืน OK;
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
    จำนวนเต็ม		include_type,
    จำนวนเต็ม		*var_count,
    จำนวนเต็ม		*semicolon,
    จำนวนเต็ม		silent)
{
    char_u	*p, *s;

    ถ้า (*arg == '[')
    {
	// "[var, var]": find the matching ']'.
	p = arg;
	สำหรับ (;;)
	{
	    p = skipwhite(p + 1);	// skip whites after '[', ';' or ','
	    s = skip_var_one(p, include_type);
	    ถ้า (s == p)
	    {
		ถ้า (!silent)
		    semsg(_(e_invalid_argument_str), p);
		คืน NULL;
	    }
	    ++*var_count;

	    p = skipwhite(s);
	    ถ้า (*p == ']')
		bha_nga;
	    มิฉะนั้น ถ้า (*p == ';')
	    {
		ถ้า (*semicolon == 1)
		{
		    ถ้า (!silent)
			emsg(_(e_double_semicolon_in_list_of_variables));
		    คืน NULL;
		}
		*semicolon = 1;
	    }
	    มิฉะนั้น ถ้า (*p != ',')
	    {
		ถ้า (!silent)
		    semsg(_(e_invalid_argument_str), p);
		คืน NULL;
	    }
	}
	คืน p + 1;
    }

    คืน skip_var_one(arg, include_type);
}

/*
 * Skip one (assignable) variable name, including @r, $VAR, &option, d.key,
 * l[idx].
 * In Vim9 script also skip over ": type" if "include_type" is TRUE.
 */
    char_u *
skip_var_one(char_u *arg, จำนวนเต็ม include_type)
{
    char_u	*end;
    จำนวนเต็ม		vim9 = in_vim9script();

    ถ้า (*arg == '@' && arg[1] != NUL)
	คืน arg + 2;

    // termcap option name may have non-alpha characters
    ถ้า (STRNCMP(arg, "&t_", 3) == 0 && arg[3] != NUL && arg[4] != NUL)
	คืน arg + 5;

    end = find_name_end(*arg == '$' || *arg == '&' ? arg + 1 : arg,
				   NULL, NULL, FNE_INCL_BR | FNE_CHECK_START);

    // "a: type" is declaring variable "a" with a type, not "a:".
    // Same for "s: type".
    ถ้า (vim9 && end == arg + 2 && end[-1] == ':')
	--end;

    ถ้า (include_type && vim9)
    {
	ถ้า (*skipwhite(end) == ':')
	    end = skip_type(skipwhite(skipwhite(end) + 1), FALSE);
    }
    คืน end;
}

/*
 * List variables for hashtab "ht" with prefix "prefix".
 * If "empty" is TRUE also list NULL strings as empty strings.
 */
    void
list_hashtable_vars(
    hashtab_T	*ht,
    var_na	*prefix,
    จำนวนเต็ม		empty,
    จำนวนเต็ม		*first)
{
    hashitem_T	*hi;
    dictitem_T	*di;
    จำนวนเต็ม		todo;
    char_u	buf[IOSIZE];

    จำนวนเต็ม save_ht_flags = ht->ht_flags;
    ht->ht_flags |= HTFLAGS_FROZEN;

    todo = (จำนวนเต็ม)ht->ht_used;
    สำหรับ (hi = ht->ht_array; todo > 0 && !got_int; ++hi)
    {
	ถ้า (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    di = HI2DI(hi);

	    // apply :filter /pat/ to variable name
	    vim_strncpy((char_u *)buf, (char_u *)prefix, IOSIZE - 1);
	    vim_strcat((char_u *)buf, di->di_key, IOSIZE);
	    ถ้า (message_filtered(buf))
		continue;

	    ถ้า (empty || di->di_tv.v_type != VAR_STRING
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
list_glob_vars(จำนวนเต็ม *first)
{
    list_hashtable_vars(&globvarht, "", TRUE, first);
}

/*
 * List buffer variables.
 */
    static void
list_buf_vars(จำนวนเต็ม *first)
{
    list_hashtable_vars(&curbuf->b_vars->dv_hashtab, "b:", TRUE, first);
}

/*
 * List window variables.
 */
    static void
list_win_vars(จำนวนเต็ม *first)
{
    list_hashtable_vars(&curwin->w_vars->dv_hashtab, "w:", TRUE, first);
}

/*
 * List tab page variables.
 */
    static void
list_tab_vars(จำนวนเต็ม *first)
{
    list_hashtable_vars(&curtab->tp_vars->dv_hashtab, "t:", TRUE, first);
}

/*
 * List variables in "arg".
 */
    static char_u *
list_arg_vars(exarg_T *eap, char_u *arg, จำนวนเต็ม *first)
{
    จำนวนเต็ม		error = FALSE;
    จำนวนเต็ม		len;
    char_u	*name;
    char_u	*name_start;
    char_u	*arg_subsc;
    char_u	*tofree;
    typval_T    tv;

    ขณะ (!ends_excmd2(eap->cmd, arg) && !got_int)
    {
	ถ้า (error || eap->skip)
	{
	    arg = find_name_end(arg, NULL, NULL, FNE_INCL_BR | FNE_CHECK_START);
	    ถ้า (!VIM_ISWHITE(*arg) && !ends_excmd(*arg))
	    {
		emsg_severe = TRUE;
		ถ้า (!did_emsg)
		    semsg(_(e_trailing_characters_str), arg);
		bha_nga;
	    }
	}
	มิฉะนั้น
	{
	    // get_name_len() takes care of expanding curly braces
	    name_start = name = arg;
	    len = get_name_len(&arg, &tofree, TRUE, TRUE);
	    ถ้า (len <= 0)
	    {
		// This is mainly to keep test 49 working: when expanding
		// curly braces fails overrule the exception error message.
		ถ้า (len < 0 && !aborting())
		{
		    emsg_severe = TRUE;
		    semsg(_(e_invalid_argument_str), arg);
		    bha_nga;
		}
		error = TRUE;
	    }
	    มิฉะนั้น
	    {
		arg = skipwhite(arg);
		ถ้า (tofree != NULL)
		    name = tofree;
		ถ้า (eval_variable(name, len, 0, &tv, NULL,
						     EVAL_VAR_VERBOSE) == FAIL)
		    error = TRUE;
		มิฉะนั้น
		{
		    // handle d.key, l[idx], f(expr)
		    arg_subsc = arg;
		    ถ้า (handle_subscript(&arg, name_start, &tv,
					      &EVALARG_EVALUATE, TRUE) == FAIL)
			error = TRUE;
		    มิฉะนั้น
		    {
			ถ้า (arg == arg_subsc && len == 2 && name[1] == ':')
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
			มิฉะนั้น
			{
			    char_u	numbuf[NUMBUFLEN];
			    char_u	*tf;
			    จำนวนเต็ม		c;
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

    คืน arg;
}

/*
 * Set an environment variable, part of ex_let_one().
 */
    static char_u *
ex_let_env(
    char_u	*arg,
    typval_T	*tv,
    จำนวนเต็ม		flags,
    char_u	*endchars,
    char_u	*op)
{
    char_u	*arg_end = NULL;
    char_u	*name;
    จำนวนเต็ม		len;

    ถ้า ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					 && (flags & ASSIGN_FOR_LOOP) == 0)
    {
	emsg(_(e_cannot_lock_environment_variable));
	คืน NULL;
    }

    // Find the end of the name.
    ++arg;
    name = arg;
    len = get_env_len(&arg);
    ถ้า (len == 0)
	semsg(_(e_invalid_argument_str), name - 1);
    มิฉะนั้น
    {
	ถ้า (op != NULL && vim_strchr((char_u *)"+-*/%", *op) != NULL)
	    semsg(_(e_wrong_variable_type_for_str_equal), op);
	มิฉะนั้น ถ้า (endchars != NULL
			      && vim_strchr(endchars, *skipwhite(arg)) == NULL)
	    emsg(_(e_unexpected_characters_in_let));
	มิฉะนั้น ถ้า (!check_secure() && !check_restricted())
	{
	    char_u	*tofree = NULL;
	    จำนวนเต็ม		c1 = name[len];
	    char_u	*p;

	    name[len] = NUL;
	    p = tv_get_string_chk(tv);
	    ถ้า (p != NULL && op != NULL && *op == '.')
	    {
		จำนวนเต็ม	mustfree = FALSE;
		char_u  *s = vim_getenv(name, &mustfree);

		ถ้า (s != NULL)
		{
		    p = tofree = concat_str(s, p);
		    ถ้า (mustfree)
			vim_free(s);
		}
	    }
	    ถ้า (p != NULL)
	    {
		vim_setenv_ext(name, p);
		arg_end = arg;
	    }
	    name[len] = c1;
	    vim_free(tofree);
	}
    }
    คืน arg_end;
}

/*
 * Set an option, part of ex_let_one().
 */
    static char_u *
ex_let_option(
    char_u	*arg,
    typval_T	*tv,
    จำนวนเต็ม		flags,
    char_u	*endchars,
    char_u	*op)
{
    char_u	*p;
    จำนวนเต็ม		scope;
    char_u	*arg_end = NULL;

    ถ้า ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					 && (flags & ASSIGN_FOR_LOOP) == 0)
    {
	emsg(_(e_cannot_lock_option));
	คืน NULL;
    }

    // Find the end of the name.
    p = find_option_end(&arg, &scope);
    ถ้า (p == NULL || (endchars != NULL
			       && vim_strchr(endchars, *skipwhite(p)) == NULL))
    {
	emsg(_(e_unexpected_characters_in_let));
	คืน NULL;
    }

    จำนวนเต็ม		c1;
    long	n = 0;
    getoption_T	opt_type;
    long	numval;
    char_u	*stringval = NULL;
    char_u	*s = NULL;
    จำนวนเต็ม		failed = FALSE;
    จำนวนเต็ม		opt_p_flags;
    char_u	*tofree = NULL;
    char_u	numbuf[NUMBUFLEN];

    c1 = *p;
    *p = NUL;

    opt_type = get_option_value(arg, &numval, &stringval, &opt_p_flags, scope);
    ถ้า (opt_type == gov_unknown && arg[0] != 't' && arg[1] != '_')
    {
	semsg(_(e_unknown_option_str_2), arg);
	goto theend;
    }
    ถ้า (op != NULL && *op != '='
	&& (((opt_type == gov_bool || opt_type == gov_number) && *op == '.')
	    || (opt_type == gov_string && *op != '.')))
    {
	semsg(_(e_wrong_variable_type_for_str_equal), op);
	goto theend;
    }

    ถ้า ((opt_type == gov_bool
		|| opt_type == gov_number
		|| opt_type == gov_hidden_bool
		|| opt_type == gov_hidden_number)
		     && (tv->v_type != VAR_STRING || !in_vim9script()))
    {
	ถ้า (opt_type == gov_bool || opt_type == gov_hidden_bool)
	    // bool, possibly hidden
	    n = (long)tv_get_bool_chk(tv, &failed);
	มิฉะนั้น
	    // number, possibly hidden
	    n = (long)tv_get_number_chk(tv, &failed);
	ถ้า (failed)
	    goto theend;
    }

    ถ้า ((opt_p_flags & P_FUNC) && (tv->v_type == VAR_PARTIAL
						    || tv->v_type == VAR_FUNC))
    {
	// If the option can be set to a function reference or a lambda
	// and the passed value is a function reference, then convert it to
	// the name (string) of the function reference.
	s = tv2string(tv, &tofree, numbuf, 0);
	ถ้า (s == NULL)
	    goto theend;
    }
    // Avoid setting a string option to the text "v:false" or similar.
    // In Vim9 script also don't convert a number to string.
    มิฉะนั้น ถ้า (tv->v_type != VAR_BOOL && tv->v_type != VAR_SPECIAL
		     && (!in_vim9script() || tv->v_type != VAR_NUMBER))
    {
	s = tv_get_string_chk(tv);
	ถ้า (s == NULL)
	    goto theend;
    }
    มิฉะนั้น ถ้า (opt_type == gov_string || opt_type == gov_hidden_string)
    {
	emsg(_(e_string_required));
	goto theend;
    }

    ถ้า (op != NULL && *op != '=')
    {
	// number, in legacy script also bool
	ถ้า (opt_type == gov_number
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
	    ถ้า (failed)
		goto theend;
	}
	มิฉะนั้น ถ้า (opt_type == gov_string && stringval != NULL && s != NULL)
	{
	    // string
	    s = concat_str(stringval, s);
	    vim_free(stringval);
	    stringval = s;
	}
    }

    var_na *err = set_option_value(arg, n, s, scope);
    arg_end = p;
    ถ้า (err != NULL)
	emsg(_(err));

theend:
    *p = c1;
    vim_free(stringval);
    vim_free(tofree);
    คืน arg_end;
}

/*
 * Set a register, part of ex_let_one().
 */
    static char_u *
ex_let_register(
    char_u	*arg,
    typval_T	*tv,
    จำนวนเต็ม		flags,
    char_u	*endchars,
    char_u	*op)
{
    char_u	*arg_end = NULL;

    ถ้า ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					 && (flags & ASSIGN_FOR_LOOP) == 0)
    {
	emsg(_(e_cannot_lock_register));
	คืน NULL;
    }
    ++arg;
    ถ้า (op != NULL && vim_strchr((char_u *)"+-*/%", *op) != NULL)
	semsg(_(e_wrong_variable_type_for_str_equal), op);
    มิฉะนั้น ถ้า (endchars != NULL
			  && vim_strchr(endchars, *skipwhite(arg + 1)) == NULL)
	emsg(_(e_unexpected_characters_in_let));
    มิฉะนั้น
    {
	char_u	*ptofree = NULL;
	char_u	*p;

	p = tv_get_string_chk(tv);
	ถ้า (p != NULL && op != NULL && *op == '.')
	{
	    char_u  *s = get_reg_contents(*arg == '@'
						  ? '"' : *arg, GREG_EXPR_SRC);

	    ถ้า (s != NULL)
	    {
		p = ptofree = concat_str(s, p);
		vim_free(s);
	    }
	}
	ถ้า (p != NULL)
	{
	    write_reg_contents(*arg == '@' ? '"' : *arg, p, -1, FALSE);
	    arg_end = arg + 1;
	}
	vim_free(ptofree);
    }
    คืน arg_end;
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
    จำนวนเต็ม		copy,		// copy value from "tv"
    จำนวนเต็ม		flags,		// ASSIGN_CONST, ASSIGN_FINAL, etc.
    char_u	*endchars,	// valid chars after variable name  or NULL
    char_u	*op,		// "+", "-", "."  or NULL
    จำนวนเต็ม		var_idx)	// variable index for "let [a, b] = list"
{
    char_u	*arg_end = NULL;

    ถ้า (in_vim9script() && (flags & (ASSIGN_NO_DECL | ASSIGN_DECL)) == 0
			&& (flags & (ASSIGN_CONST | ASSIGN_FINAL)) == 0
				  && vim_strchr((char_u *)"$@&", *arg) != NULL)
    {
	vim9_declare_error(arg);
	คืน NULL;
    }

    ถ้า (check_typval_is_value(tv) == FAIL)
	คืน NULL;

    // In Vim9 script an environment variable and a register only take a
    // String.  "@#" also takes a buffer number.
    ถ้า (in_vim9script() && (*arg == '$' || *arg == '@')
	    && !(*arg == '@' && arg[1] == '#' && tv->v_type == VAR_NUMBER
					       && (op == NULL || *op == '=')))
    {
	where_T	where = WHERE_INIT;

	ถ้า (check_typval_type(&t_string, tv, where) == FAIL)
	    คืน NULL;
    }

    ถ้า (*arg == '$')
    {
	// ":let $VAR = expr": Set environment variable.
	คืน ex_let_env(arg, tv, flags, endchars, op);
    }
    มิฉะนั้น ถ้า (*arg == '&')
    {
	// ":let &option = expr": Set option value.
	// ":let &l:option = expr": Set local option value.
	// ":let &g:option = expr": Set global option value.
	// ":for &ts in range(8)": Set option value for for loop
	คืน ex_let_option(arg, tv, flags, endchars, op);
    }
    มิฉะนั้น ถ้า (*arg == '@')
    {
	// ":let @r = expr": Set register contents.
	คืน ex_let_register(arg, tv, flags, endchars, op);
    }
    มิฉะนั้น ถ้า (eval_isnamec1(*arg) || *arg == '{')
    {
	lval_T	lv;
	char_u	*p;
	จำนวนเต็ม	lval_flags = (flags & (ASSIGN_NO_DECL | ASSIGN_DECL))
							     ? GLV_NO_DECL : 0;
	lval_flags |= (flags & ASSIGN_FOR_LOOP) ? GLV_FOR_LOOP : 0;
	ถ้า (op != NULL && *op != '=')
	    lval_flags |= GLV_ASSIGN_WITH_OP;

	// ":let var = expr": Set internal variable.
	// ":let var: type = expr": Set internal variable with type.
	// ":let {expr} = expr": Idem, name made with curly braces
	p = get_lval(arg, tv, &lv, FALSE, FALSE, lval_flags, FNE_CHECK_START);
	ถ้า (p != NULL && lv.ll_name != NULL)
	{
	    ถ้า (endchars != NULL && vim_strchr(endchars,
					   *skipwhite(lv.ll_name_end)) == NULL)
	    {
		emsg(_(e_unexpected_characters_in_let));
	    }
	    มิฉะนั้น
	    {
		set_var_lval(&lv, p, tv, copy, flags, op, var_idx);
		arg_end = lv.ll_name_end;
	    }
	}
	clear_lval(&lv);
    }
    มิฉะนั้น
	semsg(_(e_invalid_argument_str), arg);

    คืน arg_end;
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
    จำนวนเต็ม		deep = 2;

    ถ้า (eap->forceit)
	deep = -1;
    มิฉะนั้น ถ้า (vim_isdigit(*arg))
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
    จำนวนเต็ม		deep,
    จำนวนเต็ม		glv_flags,
    จำนวนเต็ม		(*callback)(lval_T *, char_u *, exarg_T *, จำนวนเต็ม, void *),
    void	*cookie)
{
    char_u	*arg = argstart;
    char_u	*name_end;
    จำนวนเต็ม		error = FALSE;
    lval_T	lv;

    do
    {
	ถ้า (*arg == '$')
	{
	    lv.ll_name = arg;
	    lv.ll_tv = NULL;
	    ++arg;
	    ถ้า (get_env_len(&arg) == 0)
	    {
		semsg(_(e_invalid_argument_str), arg - 1);
		คืน;
	    }
	    ถ้า (!error && !eap->skip
			      && callback(&lv, arg, eap, deep, cookie) == FAIL)
		error = TRUE;
	    name_end = arg;
	}
	มิฉะนั้น
	{
	    // Parse the name and find the end.
	    name_end = get_lval(arg, NULL, &lv, TRUE, eap->skip || error,
				     glv_flags | GLV_NO_DECL, FNE_CHECK_START);
	    ถ้า (lv.ll_name == NULL)
		error = TRUE;	    // error but continue parsing
	    ถ้า (name_end == NULL || (!VIM_ISWHITE(*name_end)
						    && !ends_excmd(*name_end)))
	    {
		ถ้า (name_end != NULL)
		{
		    emsg_severe = TRUE;
		    semsg(_(e_trailing_characters_str), name_end);
		}
		ถ้า (!(eap->skip || error))
		    clear_lval(&lv);
		bha_nga;
	    }

	    ถ้า (!error && !eap->skip
			 && callback(&lv, name_end, eap, deep, cookie) == FAIL)
		error = TRUE;

	    ถ้า (!eap->skip)
		clear_lval(&lv);
	}

	arg = skipwhite(name_end);
    } ขณะ (!ends_excmd2(name_end, arg));

    set_nextcmd(eap, arg);
}

    static จำนวนเต็ม
do_unlet_var(
    lval_T	*lp,
    char_u	*name_end,
    exarg_T	*eap,
    จำนวนเต็ม		deep UNUSED,
    void	*cookie UNUSED)
{
    จำนวนเต็ม		forceit = eap->forceit;
    จำนวนเต็ม		ret = OK;
    จำนวนเต็ม		cc;

    ถ้า (lp->ll_tv == NULL)
    {
	cc = *name_end;
	*name_end = NUL;

	// Environment variable, normal name or expanded name.
	ถ้า (*lp->ll_name == '$')
	    vim_unsetenv_ext(lp->ll_name + 1);
	มิฉะนั้น ถ้า (do_unlet(lp->ll_name, forceit) == FAIL)
	    ret = FAIL;
	*name_end = cc;
    }
    มิฉะนั้น ถ้า ((lp->ll_list != NULL
		 && value_check_lock(lp->ll_list->lv_lock, lp->ll_name, FALSE))
	    || (lp->ll_dict != NULL
		&& value_check_lock(lp->ll_dict->dv_lock, lp->ll_name, FALSE)))
	คืน FAIL;
    มิฉะนั้น ถ้า (lp->ll_range)
	list_unlet_range(lp->ll_list, lp->ll_li, lp->ll_n1,
						    !lp->ll_empty2, lp->ll_n2);
    มิฉะนั้น ถ้า (lp->ll_list != NULL)
	// unlet a List item.
	listitem_remove(lp->ll_list, lp->ll_li);
    มิฉะนั้น ถ้า (lp->ll_dict != NULL)
	// unlet a Dictionary item.
	dictitem_remove(lp->ll_dict, lp->ll_di, "unlet");
    มิฉะนั้น
    {
	semsg(_(e_cannot_unlet_imported_item_str), lp->ll_name);
	คืน FAIL;
    }

    คืน ret;
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
	จำนวนเต็ม	    has_n2,
	long	    n2)
{
    listitem_T  *li = li_first;
    จำนวนเต็ม		n1 = n1_arg;

    // Delete a range of List items.
    li = li_first;
    n1 = n1_arg;
    ขณะ (li != NULL && (!has_n2 || n2 >= n1))
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
    จำนวนเต็ม
do_unlet(char_u *name, จำนวนเต็ม forceit)
{
    hashtab_T	*ht;
    hashitem_T	*hi;
    char_u	*varname = NULL;  // init to shut up gcc
    dict_T	*d;
    dictitem_T	*di;

    // can't :unlet a script variable in Vim9 script
    ถ้า (in_vim9script() && check_vim9_unlet(name) == FAIL)
	คืน FAIL;

    ht = find_var_ht(name, &varname);

    // can't :unlet a script variable in Vim9 script from a function
    ถ้า (ht == get_script_local_ht()
	    && SCRIPT_ID_VALID(current_sctx.sc_sid)
	    && SCRIPT_ITEM(current_sctx.sc_sid)->sn_version
							 == SCRIPT_VERSION_VIM9
	    && check_vim9_unlet(name) == FAIL)
	คืน FAIL;

    ถ้า (ht != NULL && *varname != NUL)
    {
	d = get_current_funccal_dict(ht);
	ถ้า (d == NULL)
	{
	    ถ้า (ht == &globvarht)
		d = &globvardict;
	    มิฉะนั้น ถ้า (ht == &compat_hashtab)
		d = &vimvardict;
	    มิฉะนั้น
	    {
		di = find_var_in_ht(ht, *name, (char_u *)"", FALSE);
		d = di == NULL ? NULL : di->di_tv.vval.v_dict;
	    }
	    ถ้า (d == NULL)
	    {
		internal_error("do_unlet()");
		คืน FAIL;
	    }
	}
	hi = hash_find(ht, varname);
	ถ้า (HASHITEM_EMPTY(hi))
	    hi = find_hi_in_scoped_ht(name, &ht);
	ถ้า (hi != NULL && !HASHITEM_EMPTY(hi))
	{
	    di = HI2DI(hi);
	    ถ้า (var_check_fixed(di->di_flags, name, FALSE)
		    || var_check_ro(di->di_flags, name, FALSE)
		    || value_check_lock(d->dv_lock, name, FALSE)
		    || check_hashtab_frozen(ht, "unlet"))
		คืน FAIL;

	    delete_var(ht, hi);
	    คืน OK;
	}
    }
    ถ้า (forceit)
	คืน OK;
    semsg(_(e_no_such_variable_str), name);
    คืน FAIL;
}

    static void
report_lockvar_member(var_na *msg, lval_T *lp)
{
    จำนวนเต็ม did_alloc = FALSE;
    char_u *vname = (char_u *)"";
    char_u *class_name = lp->ll_class != NULL
				    ? lp->ll_class->class_name.string : (char_u *)"";
    ถ้า (lp->ll_name != NULL)
    {
	ถ้า (lp->ll_name_end == NULL)
	    vname = lp->ll_name;
	มิฉะนั้น
	{
	    vname = vim_strnsave(lp->ll_name, lp->ll_name_end - lp->ll_name);
	    ถ้า (vname == NULL)
		คืน;
	    did_alloc = TRUE;
	}
    }
    semsg(_(msg), vname, class_name);
    ถ้า (did_alloc)
	vim_free(vname);
}

/*
 * Lock or unlock variable indicated by "lp".
 * "deep" is the levels to go (-1 for unlimited);
 * "lock" is TRUE for ":lockvar", FALSE for ":unlockvar".
 */
    static จำนวนเต็ม
do_lock_var(
    lval_T	*lp,
    char_u	*name_end,
    exarg_T	*eap,
    จำนวนเต็ม		deep,
    void	*cookie UNUSED)
{
    จำนวนเต็ม		lock = eap->cmdidx == CMD_lockvar;
    จำนวนเต็ม		ret = OK;
    จำนวนเต็ม		cc;
    dictitem_T	*di;

#ifdef LOG_LOCKVAR
    ch_log(NULL, "LKVAR: do_lock_var(): name %s, is_root %d", lp->ll_name, lp->ll_is_root);
#endif

    ถ้า (lp->ll_tv == NULL)
    {
	cc = *name_end;
	*name_end = NUL;
	ถ้า (*lp->ll_name == '$')
	{
	    semsg(_(e_cannot_lock_or_unlock_variable_str), lp->ll_name);
	    ret = FAIL;
	}
	มิฉะนั้น
	{
	    // Normal name or expanded name.
	    di = find_var(lp->ll_name, NULL, TRUE);
	    ถ้า (di == NULL)
	    {
		ถ้า (in_vim9script())
		    semsg(_(e_cannot_find_variable_to_unlock_str),
								  lp->ll_name);
		ret = FAIL;
	    }
	    มิฉะนั้น
	    {
		ถ้า ((di->di_flags & DI_FLAGS_FIX)
			    && di->di_tv.v_type != VAR_DICT
			    && di->di_tv.v_type != VAR_LIST)
		{
		    // For historic reasons this error is not given for a list
		    // or dict.  E.g., the b: dict could be locked/unlocked.
		    semsg(_(e_cannot_lock_or_unlock_variable_str), lp->ll_name);
		    ret = FAIL;
		}
		มิฉะนั้น
		{
		    ถ้า (in_vim9script())
		    {
			svar_T  *sv = find_typval_in_script(&di->di_tv,
								     0, FALSE);

			ถ้า (sv != NULL && sv->sv_const != 0)
			{
			    semsg(_(e_cannot_change_readonly_variable_str),
								  lp->ll_name);
			    ret = FAIL;
			}
		    }

		    ถ้า (ret == OK)
		    {
			ถ้า (lock)
			    di->di_flags |= DI_FLAGS_LOCK;
			มิฉะนั้น
			    di->di_flags &= ~DI_FLAGS_LOCK;
			ถ้า (deep != 0)
			    item_lock(&di->di_tv, deep, lock, FALSE);
		    }
		}
	    }
	}
	*name_end = cc;
    }
    มิฉะนั้น ถ้า (deep == 0 && lp->ll_object == NULL && lp->ll_class == NULL)
    {
	// nothing to do
    }
    มิฉะนั้น ถ้า (lp->ll_is_root)
	// (un)lock the item.
	item_lock(lp->ll_tv, deep, lock, FALSE);
    มิฉะนั้น ถ้า (lp->ll_range)
    {
	listitem_T    *li = lp->ll_li;

	// (un)lock a range of List items.
	ขณะ (li != NULL && (lp->ll_empty2 || lp->ll_n2 >= lp->ll_n1))
	{
	    item_lock(&li->li_tv, deep, lock, FALSE);
	    li = li->li_next;
	    ++lp->ll_n1;
	}
    }
    มิฉะนั้น ถ้า (lp->ll_list != NULL)
	// (un)lock a List item.
	item_lock(&lp->ll_li->li_tv, deep, lock, FALSE);
    มิฉะนั้น ถ้า (lp->ll_object != NULL)  // This check must be before ll_class.
    {
	// (un)lock an object variable.
	report_lockvar_member(e_cannot_lock_object_variable_str, lp);
	ret = FAIL;
    }
    มิฉะนั้น ถ้า (lp->ll_class != NULL)
    {
	// (un)lock a class variable.
	report_lockvar_member(e_cannot_lock_class_variable_str, lp);
	ret = FAIL;
    }
    มิฉะนั้น
    {
	// (un)lock a Dictionary item.
	ถ้า (lp->ll_di == NULL)
	{
	    emsg(_(e_dictionary_required));
	    ret = FAIL;
	}
	มิฉะนั้น
	    item_lock(&lp->ll_di->di_tv, deep, lock, FALSE);
    }

    คืน ret;
}

/*
 * Lock or unlock an item.  "deep" is nr of levels to go.
 * When "check_refcount" is TRUE do not lock a list or dict with a reference
 * count larger than 1.
 */
    void
item_lock(typval_T *tv, จำนวนเต็ม deep, จำนวนเต็ม lock, จำนวนเต็ม check_refcount)
{
    static จำนวนเต็ม	recurse = 0;
    list_T	*l;
    listitem_T	*li;
    dict_T	*d;
    blob_T	*b;
    hashitem_T	*hi;
    จำนวนเต็ม		todo;

#ifdef LOG_LOCKVAR
    ch_log(NULL, "LKVAR: item_lock(): type %s", vartype_name(tv->v_type));
#endif

    ถ้า (recurse >= DICT_MAXNEST)
    {
	emsg(_(e_variable_nested_too_deep_for_unlock));
	คืน;
    }
    ถ้า (deep == 0)
	คืน;
    ++recurse;

    // lock/unlock the item itself
    ถ้า (lock)
	tv->v_lock |= VAR_LOCKED;
    มิฉะนั้น
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
	    ถ้า ((b = tv->vval.v_blob) != NULL
				    && !(check_refcount && b->bv_refcount > 1))
	    {
		ถ้า (lock)
		    b->bv_lock |= VAR_LOCKED;
		มิฉะนั้น
		    b->bv_lock &= ~VAR_LOCKED;
	    }
	    bha_nga;
	pak_sa VAR_LIST:
	    ถ้า ((l = tv->vval.v_list) != NULL
				    && !(check_refcount && l->lv_refcount > 1))
	    {
		ถ้า (lock)
		    l->lv_lock |= VAR_LOCKED;
		มิฉะนั้น
		    l->lv_lock &= ~VAR_LOCKED;
		ถ้า (deep < 0 || deep > 1)
		{
		    ถ้า (l->lv_first == &range_list_item)
			l->lv_lock |= VAR_ITEMS_LOCKED;
		    มิฉะนั้น
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
	    ถ้า ((d = tv->vval.v_dict) != NULL
				    && !(check_refcount && d->dv_refcount > 1))
	    {
		ถ้า (lock)
		    d->dv_lock |= VAR_LOCKED;
		มิฉะนั้น
		    d->dv_lock &= ~VAR_LOCKED;
		ถ้า (deep < 0 || deep > 1)
		{
		    // recursive: lock/unlock the items the List contains
		    todo = (จำนวนเต็ม)d->dv_hashtab.ht_used;
		    FOR_ALL_HASHTAB_ITEMS(&d->dv_hashtab, hi, todo)
		    {
			ถ้า (!HASHITEM_EMPTY(hi))
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

#ถ้า defined(FEAT_MENU) && defined(FEAT_MULTI_LANG)
/*
 * Delete all "menutrans_" variables.
 */
    void
del_menutrans_vars(void)
{
    hashitem_T	*hi;
    จำนวนเต็ม		todo;

    hash_lock(&globvarht);
    todo = (จำนวนเต็ม)globvarht.ht_used;
    สำหรับ (hi = globvarht.ht_array; todo > 0 && !got_int; ++hi)
    {
	ถ้า (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    ถ้า (STRNCMP(HI2DI(hi)->di_key, "menutrans_", 10) == 0)
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
static จำนวนเต็ม	varnamebuflen = 0;

/*
 * Function to concatenate a prefix and a variable name.
 */
    char_u *
cat_prefix_varname(จำนวนเต็ม prefix, char_u *name)
{
    จำนวนเต็ม		len;

    len = (จำนวนเต็ม)STRLEN(name) + 3;
    ถ้า (len > varnamebuflen)
    {
	vim_free(varnamebuf);
	len += 10;			// some additional space
	varnamebuf = alloc(len);
	ถ้า (varnamebuf == NULL)
	{
	    varnamebuflen = 0;
	    คืน NULL;
	}
	varnamebuflen = len;
    }
    *varnamebuf = prefix;
    varnamebuf[1] = ':';
    STRCPY(varnamebuf + 2, name);
    คืน varnamebuf;
}

/*
 * Function given to ExpandGeneric() to obtain the list of user defined
 * (global/buffer/window/built-in) variable names.
 */
    char_u *
get_user_var_name(expand_T *xp, จำนวนเต็ม idx)
{
    static long_u	gdone;
    static long_u	bdone;
    static long_u	wdone;
    static long_u	tdone;
    static จำนวนเต็ม		vidx;
    static hashitem_T	*hi;
    hashtab_T		*ht;

    ถ้า (idx == 0)
    {
	gdone = bdone = wdone = vidx = 0;
	tdone = 0;
    }

    // Global variables
    ถ้า (gdone < globvarht.ht_used)
    {
	ถ้า (gdone++ == 0)
	    hi = globvarht.ht_array;
	มิฉะนั้น
	    ++hi;
	ขณะ (HASHITEM_EMPTY(hi))
	    ++hi;
	ถ้า (STRNCMP("g:", xp->xp_pattern, 2) == 0)
	    คืน cat_prefix_varname('g', hi->hi_key);
	คืน hi->hi_key;
    }

    // b: variables
    ht = &prevwin_curwin()->w_buffer->b_vars->dv_hashtab;
    ถ้า (bdone < ht->ht_used)
    {
	ถ้า (bdone++ == 0)
	    hi = ht->ht_array;
	มิฉะนั้น
	    ++hi;
	ขณะ (HASHITEM_EMPTY(hi))
	    ++hi;
	คืน cat_prefix_varname('b', hi->hi_key);
    }

    // w: variables
    ht = &prevwin_curwin()->w_vars->dv_hashtab;
    ถ้า (wdone < ht->ht_used)
    {
	ถ้า (wdone++ == 0)
	    hi = ht->ht_array;
	มิฉะนั้น
	    ++hi;
	ขณะ (HASHITEM_EMPTY(hi))
	    ++hi;
	คืน cat_prefix_varname('w', hi->hi_key);
    }

    // t: variables
    ht = &curtab->tp_vars->dv_hashtab;
    ถ้า (tdone < ht->ht_used)
    {
	ถ้า (tdone++ == 0)
	    hi = ht->ht_array;
	มิฉะนั้น
	    ++hi;
	ขณะ (HASHITEM_EMPTY(hi))
	    ++hi;
	คืน cat_prefix_varname('t', hi->hi_key);
    }

    // v: variables
    ถ้า (vidx < VV_LEN)
	คืน cat_prefix_varname('v', (char_u *)vimvars[vidx++].vv_name);

    VIM_CLEAR(varnamebuf);
    varnamebuflen = 0;
    คืน NULL;
}

    var_na *
get_var_special_name(จำนวนเต็ม nr)
{
    switch (nr)
    {
	pak_sa VVAL_FALSE: คืน in_vim9script() ? "false" : "v:false";
	pak_sa VVAL_TRUE:  คืน in_vim9script() ? "true" : "v:true";
	pak_sa VVAL_NULL:  คืน in_vim9script() ? "null" : "v:null";
	pak_sa VVAL_NONE:  คืน "v:none";
    }
    internal_error("get_var_special_name()");
    คืน "42";
}

/*
 * Returns the global variable dictionary
 */
    dict_T *
get_globvar_dict(void)
{
    คืน &globvardict;
}

/*
 * Returns the global variable hash table
 */
    hashtab_T *
get_globvar_ht(void)
{
    คืน &globvarht;
}

/*
 * Returns the v: variable dictionary
 */
    dict_T *
get_vimvar_dict(void)
{
    คืน &vimvardict;
}

/*
 * Returns the index of a v:variable.  Negative if not found.
 * Returns DI_ flags in "di_flags".
 */
    จำนวนเต็ม
find_vim_var(char_u *name, จำนวนเต็ม *di_flags)
{
    dictitem_T	    *di = find_var_in_ht(&vimvarht, 0, name, TRUE);
    sa_nracanaa vimvar   *vv;

    ถ้า (di == NULL)
	คืน -1;
    *di_flags = di->di_flags;
    vv = (sa_nracanaa vimvar *)((var_na *)di - offsetof(vimvar_T, vv_di));
    คืน (จำนวนเต็ม)(vv - vimvars);
}


/*
 * Set type of v: variable to "type".
 */
    void
set_vim_var_type(จำนวนเต็ม idx, vartype_T type)
{
    vimvars[idx].vv_tv_type = type;
}

/*
 * Set number v: variable to "val".
 * Note that this does not set the type, use set_vim_var_type() for that.
 */
    void
set_vim_var_nr(จำนวนเต็ม idx, varnumber_T val)
{
    vimvars[idx].vv_nr = val;
}

    var_na *
get_vim_var_name(จำนวนเต็ม idx)
{
    คืน vimvars[idx].vv_name;
}

/*
 * Get typval_T v: variable value.
 */
    typval_T *
get_vim_var_tv(จำนวนเต็ม idx)
{
    คืน &vimvars[idx].vv_tv;
}

    type_T *
get_vim_var_type(จำนวนเต็ม idx, garray_T *type_list)
{
    ถ้า (vimvars[idx].vv_type != NULL)
	คืน vimvars[idx].vv_type;
    คืน typval2type_vimvar(&vimvars[idx].vv_tv, type_list);
}

/*
 * Add what getinfo() reports for the v: variable "name", without the "v:", to
 * "d".  Returns FAIL when there is no such variable.
 */
    จำนวนเต็ม
vim_var_info(char_u *name, dict_T *d)
{
    จำนวนเต็ม		di_flags;
    จำนวนเต็ม		idx = find_vim_var(name, &di_flags);
    garray_T	type_list;
    var_na	*tofree;

    ถ้า (idx < 0)
	คืน FAIL;
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
    คืน OK;
}

/*
 * Set v: variable to "tv".  Only accepts the same type.
 * Takes over the value of "tv".
 */
    จำนวนเต็ม
set_vim_var_tv(จำนวนเต็ม idx, typval_T *tv)
{
    ถ้า (vimvars[idx].vv_tv_type != tv->v_type)
    {
	emsg(_(e_type_mismatch_for_v_variable));
	clear_tv(tv);
	คืน FAIL;
    }
    // VV_RO is also checked when compiling, but let's check here as well.
    ถ้า (vimvars[idx].vv_flags & VV_RO)
    {
	semsg(_(e_cannot_change_readonly_variable_str), vimvars[idx].vv_name);
	คืน FAIL;
    }
    ถ้า (sandbox && (vimvars[idx].vv_flags & VV_RO_SBX))
    {
	semsg(_(e_cannot_set_variable_in_sandbox_str), vimvars[idx].vv_name);
	คืน FAIL;
    }
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_di.di_tv = *tv;
    คืน OK;
}

/*
 * Get number v: variable value.
 */
    varnumber_T
get_vim_var_nr(จำนวนเต็ม idx)
{
    คืน vimvars[idx].vv_nr;
}

/*
 * Get string v: variable value.  Uses a static buffer, can only be used once.
 * If the String variable has never been set, return an empty string.
 * Never returns NULL;
 */
    char_u *
get_vim_var_str(จำนวนเต็ม idx)
{
    คืน tv_get_string(&vimvars[idx].vv_tv);
}

/*
 * Get List v: variable value.  Caller must take care of reference count when
 * needed.
 */
    list_T *
get_vim_var_list(จำนวนเต็ม idx)
{
    คืน vimvars[idx].vv_list;
}

/*
 * Get Dict v: variable value.  Caller must take care of reference count when
 * needed.
 */
    dict_T *
get_vim_var_dict(จำนวนเต็ม idx)
{
    คืน vimvars[idx].vv_dict;
}

/*
 * Set v:char to character "c".
 */
    void
set_vim_var_char(จำนวนเต็ม c)
{
    char_u  buf[MB_MAXBYTES + 1];
    size_t  buflen;

    ถ้า (has_mbyte)
	buflen = (*mb_char2bytes)(c, buf);
    มิฉะนั้น
    {
	buf[0] = c;
	buflen = 1;
    }
    buf[buflen] = NUL;

    set_vim_var_string(VV_CHAR, buf, (จำนวนเต็ม)buflen);
}

/*
 * Set v:count to "count" and v:count1 to "count1".
 * When "set_prevcount" is TRUE first set v:prevcount from v:count.
 */
    void
set_vcount(
    long	count,
    long	count1,
    จำนวนเต็ม		set_prevcount)
{
    ถ้า (set_prevcount)
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
    จำนวนเต็ม		idx,
    char_u	*val,
    จำนวนเต็ม		len)	    // length of "val" to use or -1 (whole string)
{
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_tv_type = VAR_STRING;
    ถ้า (val == NULL)
	vimvars[idx].vv_str = NULL;
    มิฉะนั้น ถ้า (len == -1)
	vimvars[idx].vv_str = vim_strsave(val);
    มิฉะนั้น
	vimvars[idx].vv_str = vim_strnsave(val, len);
}

    void
set_vim_var_string_direct(
    จำนวนเต็ม		idx,
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
set_vim_var_list(จำนวนเต็ม idx, list_T *val)
{
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_tv_type = VAR_LIST;
    vimvars[idx].vv_list = val;
    ถ้า (val != NULL)
	++val->lv_refcount;
}

/*
 * Set Dictionary v: variable to "val".
 */
    void
set_vim_var_dict(จำนวนเต็ม idx, dict_T *val)
{
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_tv_type = VAR_DICT;
    vimvars[idx].vv_dict = val;
    ถ้า (val == NULL)
	คืน;

    ++val->dv_refcount;
    dict_set_items_ro(val);
}

/*
 * Set the v:argv list.
 */
    void
set_argv_var(var_na **argv, จำนวนเต็ม argc)
{
    list_T	*l = list_alloc();
    จำนวนเต็ม		i;

    ถ้า (l == NULL)
	getout(1);
    l->lv_lock = VAR_FIXED;
    สำหรับ (i = 0; i < argc; ++i)
    {
	ถ้า (list_append_string(l, (char_u *)argv[i], -1) == FAIL)
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
    จำนวนเต็ม regname = 0;

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
set_reg_var(จำนวนเต็ม c)
{
    char_u  regname[2];

    ถ้า (c == 0 || c == ' ')
	regname[0] = '"';
    มิฉะนั้น
	regname[0] = c;
    regname[1] = NUL;
    // Avoid free/alloc when the value is already right.
    ถ้า (vimvars[VV_REG].vv_str == NULL || vimvars[VV_REG].vv_str[0] != c)
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
    ถ้า (oldval == NULL)
	คืน vimvars[VV_EXCEPTION].vv_str;

    vimvars[VV_EXCEPTION].vv_str = oldval;
    คืน NULL;
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
    ถ้า (oldval == NULL)
	คืน vimvars[VV_THROWPOINT].vv_str;

    vimvars[VV_THROWPOINT].vv_str = oldval;
    คืน NULL;
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
    ถ้า (eap == NULL)
    {
	vim_free(oldval);
	vimvars[VV_CMDARG].vv_str = oldarg;
	คืน NULL;
    }

    ถ้า (eap->force_bin == FORCE_BIN)
	len = 6;
    มิฉะนั้น ถ้า (eap->force_bin == FORCE_NOBIN)
	len = 8;
    มิฉะนั้น
	len = 0;

    ถ้า (eap->read_edit)
	len += 7;

    ถ้า (eap->force_ff != 0)
	len += 10; // " ++ff=unix"
    ถ้า (eap->force_enc != 0)
	len += (unsigned)STRLEN(eap->cmd + eap->force_enc) + 7;
    ถ้า (eap->bad_char != 0)
	len += 7 + 4;  // " ++bad=" + "keep" or "drop"

    newval = alloc(len + 1);
    ถ้า (newval == NULL)
	คืน NULL;

    ถ้า (eap->force_bin == FORCE_BIN)
	sprintf((var_na *)newval, " ++bin");
    มิฉะนั้น ถ้า (eap->force_bin == FORCE_NOBIN)
	sprintf((var_na *)newval, " ++nobin");
    มิฉะนั้น
	*newval = NUL;

    ถ้า (eap->read_edit)
	STRCAT(newval, " ++edit");

    ถ้า (eap->force_ff != 0)
	sprintf((var_na *)newval + STRLEN(newval), " ++ff=%s",
						eap->force_ff == 'u' ? "unix"
						: eap->force_ff == 'd' ? "dos"
						: "mac");
    ถ้า (eap->force_enc != 0)
	sprintf((var_na *)newval + STRLEN(newval), " ++enc=%s",
					       eap->cmd + eap->force_enc);
    ถ้า (eap->bad_char == BAD_KEEP)
	STRCPY(newval + STRLEN(newval), " ++bad=keep");
    มิฉะนั้น ถ้า (eap->bad_char == BAD_DROP)
	STRCPY(newval + STRLEN(newval), " ++bad=drop");
    มิฉะนั้น ถ้า (eap->bad_char != 0)
	sprintf((var_na *)newval + STRLEN(newval), " ++bad=%c", eap->bad_char);
    vimvars[VV_CMDARG].vv_str = newval;
    คืน oldval;
}

/*
 * Get the value of internal variable "name".
 * If "flags" has EVAL_VAR_IMPORT may return a VAR_ANY with v_number set to the
 * imported script ID.
 * Return OK or FAIL.  If OK is returned "rettv" must be cleared.
 */
    จำนวนเต็ม
eval_variable(
    char_u	*name,
    จำนวนเต็ม		len,		// length of "name" or zero
    scid_T	sid,		// script ID for imported item or zero
    typval_T	*rettv,		// NULL when only checking existence
    dictitem_T	**dip,		// non-NULL when typval's dict item is needed
    จำนวนเต็ม		flags)		// EVAL_VAR_ flags
{
    จำนวนเต็ม		ret = OK;
    typval_T	*tv = NULL;
    จำนวนเต็ม		found = FALSE;
    hashtab_T	*ht = NULL;
    จำนวนเต็ม		cc = 0;
    type_T	*type = NULL;

    ถ้า (len > 0)
    {
	// truncate the name, so that we can use strcmp()
	cc = name[len];
	name[len] = NUL;
    }

    // Check for local variable when debugging.
    ถ้า ((sid == 0) && (tv = lookup_debug_var(name)) == NULL)
    {
	// Check for user-defined variables.
	dictitem_T	*v = find_var(name, &ht, flags & EVAL_VAR_NOAUTOLOAD);

	ถ้า (v != NULL)
	{
	    tv = &v->di_tv;
	    ถ้า (dip != NULL)
		*dip = v;
	}
	มิฉะนั้น
	    ht = NULL;
    }

    ถ้า (tv == NULL && (in_vim9script() || STRNCMP(name, "s:", 2) == 0))
    {
	imported_T  *import = NULL;
	char_u	    *p = STRNCMP(name, "s:", 2) == 0 ? name + 2 : name;

	ถ้า (sid == 0)
	    import = find_imported(p, 0, TRUE);

	// imported variable from another script
	ถ้า (import != NULL || sid != 0)
	{
	    ถ้า ((flags & EVAL_VAR_IMPORT) == 0)
	    {
		ถ้า (SCRIPT_ID_VALID(sid))
		{
		    ht = &SCRIPT_VARS(sid);
		    ถ้า (ht != NULL)
		    {
			dictitem_T *v = find_var_in_ht(ht, 0, name,
						  flags & EVAL_VAR_NOAUTOLOAD);

			ถ้า (v == NULL)
			    v = find_var_autoload_prefix(name, sid, NULL, NULL);

			ถ้า (v != NULL)
			{
			    tv = &v->di_tv;
			    ถ้า (dip != NULL)
				*dip = v;
			}
			มิฉะนั้น
			    ht = NULL;
		    }
		}
		มิฉะนั้น
		{
		    ถ้า (flags & EVAL_VAR_VERBOSE)
			semsg(_(e_expected_dot_after_name_str), name);
		    ret = FAIL;
		}
	    }
	    มิฉะนั้น
	    {
		ถ้า (rettv != NULL)
		{
		    // special value that is used in handle_subscript()
		    rettv->v_type = VAR_ANY;
		    rettv->vval.v_number = sid != 0 ? sid : import->imp_sid;
		}
		found = TRUE;
	    }
	}
	มิฉะนั้น ถ้า (in_vim9script() && (flags & EVAL_VAR_NO_FUNC) == 0)
	{
	    จำนวนเต็ม	    has_g_prefix = STRNCMP(name, "g:", 2) == 0;
	    ufunc_T *ufunc = find_func(name, FALSE);

	    ถ้า (ufunc != NULL && cc == '<')
	    {
		// handle generic function
		char_u	*argp = name + len;
		name[len] = cc;
		ufunc = eval_generic_func(ufunc, name, &argp);
		name[len] = NUL;
		ถ้า (ufunc == NULL)
		{
		    ret = FAIL;
		    goto done;
		}
	    }

	    // In Vim9 script we can get a function reference by using the
	    // function name.  For a global non-autoload function "g:" is
	    // required.
	    ถ้า (ufunc != NULL && (has_g_prefix
					    || !func_requires_g_prefix(ufunc)))
	    {
		found = TRUE;
		ถ้า (rettv != NULL)
		{
		    rettv->v_type = VAR_FUNC;
		    ถ้า (has_g_prefix)
		    {
			// Keep the "g:", otherwise script-local may be
			// assumed.
			ถ้า (cc != '<')
			    rettv->vval.v_string = vim_strsave(name);
			มิฉะนั้น
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
		    มิฉะนั้น
			rettv->vval.v_string = vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
		    ถ้า (rettv->vval.v_string != NULL)
			func_ref(ufunc->uf_name);
		}
	    }
	}
    }

    ถ้า (!found)
    {
	ถ้า (tv == NULL)
	{
	    ถ้า (rettv != NULL && (flags & EVAL_VAR_VERBOSE))
		semsg(_(e_undefined_variable_str), name);
	    ret = FAIL;
	}
	มิฉะนั้น ถ้า (rettv != NULL)
	{
	    svar_T  *sv = NULL;
	    จำนวนเต็ม	    was_assigned = FALSE;

	    ถ้า (ht != NULL && ht == get_script_local_ht()
		    && tv != &SCRIPT_SV(current_sctx.sc_sid)->sv_var.di_tv)
	    {
		sv = find_typval_in_script(tv, 0, TRUE);
		ถ้า (sv != NULL)
		{
		    type = sv->sv_type;
		    was_assigned = sv->sv_flags & SVFLAG_ASSIGNED;
		}
	    }

	    ถ้า ((tv->v_type == VAR_TYPEALIAS || tv->v_type == VAR_CLASS)
		    && sid != 0)
	    {
		// type alias or class imported from another script.  Check
		// whether it is exported from the other script.
		sv = find_typval_in_script(tv, sid, TRUE);
		ถ้า (sv == NULL)
		{
		    ret = FAIL;
		    goto done;
		}
		ถ้า ((sv->sv_flags & SVFLAG_EXPORTED) == 0)
		{
		    semsg(_(e_item_not_exported_in_script_str), name);
		    ret = FAIL;
		    goto done;
		}
	    }

	    // If a list or tuple or dict variable wasn't initialized and has
	    // meaningful type, do it now.  Not for global variables, they are
	    // not declared.
	    ถ้า (ht != &globvarht)
	    {
		ถ้า (tv->v_type == VAR_DICT && tv->vval.v_dict == NULL
					    && ((type != NULL && !was_assigned)
							  || !in_vim9script()))
		{
		    tv->vval.v_dict = dict_alloc();
		    ถ้า (tv->vval.v_dict != NULL)
		    {
			++tv->vval.v_dict->dv_refcount;
			tv->vval.v_dict->dv_type = alloc_type(type);
			ถ้า (sv != NULL)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
		มิฉะนั้น ถ้า (tv->v_type == VAR_LIST && tv->vval.v_list == NULL
					    && ((type != NULL && !was_assigned)
							  || !in_vim9script()))
		{
		    tv->vval.v_list = list_alloc();
		    ถ้า (tv->vval.v_list != NULL)
		    {
			++tv->vval.v_list->lv_refcount;
			tv->vval.v_list->lv_type = alloc_type(type);
			ถ้า (sv != NULL)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
		มิฉะนั้น ถ้า (tv->v_type == VAR_TUPLE && tv->vval.v_tuple == NULL
					    && ((type != NULL && !was_assigned)
							  || !in_vim9script()))
		{
		    tv->vval.v_tuple = tuple_alloc();
		    ถ้า (tv->vval.v_tuple != NULL)
		    {
			++tv->vval.v_tuple->tv_refcount;
			tv->vval.v_tuple->tv_type = alloc_type(type);
			ถ้า (sv != NULL)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
		มิฉะนั้น ถ้า (tv->v_type == VAR_BLOB && tv->vval.v_blob == NULL
					    && ((type != NULL && !was_assigned)
							  || !in_vim9script()))
		{
		    tv->vval.v_blob = blob_alloc();
		    ถ้า (tv->vval.v_blob != NULL)
		    {
			++tv->vval.v_blob->bv_refcount;
			ถ้า (sv != NULL)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
	    }
	    ret = copy_tv(tv, rettv);
	}
    }

done:
    ถ้า (len > 0)
	name[len] = cc;

    คืน ret;
}

/*
 * Get the value of internal variable "name", also handling "import.name".
 * Return OK or FAIL.  If OK is returned "rettv" must be cleared.
 */
    จำนวนเต็ม
eval_variable_import(
    char_u	*name,
    typval_T	*rettv)
{
    char_u  *s = name;
    ขณะ (ASCII_ISALNUM(*s) || *s == '_')
	++s;
    จำนวนเต็ม	    len = (จำนวนเต็ม)(s - name);

    ถ้า (eval_variable(name, len, 0, rettv, NULL, EVAL_VAR_IMPORT) == FAIL)
	คืน FAIL;
    ถ้า (rettv->v_type == VAR_ANY && *s == '.')
    {
	char_u *ns = s + 1;
	s = ns;
	ขณะ (ASCII_ISALNUM(*s) || *s == '_')
	    ++s;
	จำนวนเต็ม sid = rettv->vval.v_number;
	คืน eval_variable(ns, (จำนวนเต็ม)(s - ns), sid, rettv, NULL, 0);
    }
    คืน OK;
}


/*
 * Check if variable "name[len]" is a local variable or an argument.
 * If so, "*eval_lavars_used" is set to TRUE.
 */
    void
check_vars(char_u *name, จำนวนเต็ม len)
{
    จำนวนเต็ม		cc;
    char_u	*varname;
    hashtab_T	*ht;

    ถ้า (eval_lavars_used == NULL)
	คืน;

    // truncate the name, so that we can use strcmp()
    cc = name[len];
    name[len] = NUL;

    ht = find_var_ht(name, &varname);
    ถ้า (ht == get_funccal_local_ht() || ht == get_funccal_args_ht())
    {
	ถ้า (find_var(name, NULL, TRUE) != NULL)
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
find_var(char_u *name, hashtab_T **htp, จำนวนเต็ม no_autoload)
{
    char_u	*varname;
    hashtab_T	*ht;
    dictitem_T	*ret = NULL;

    ht = find_var_ht(name, &varname);
    ถ้า (htp != NULL)
	*htp = ht;
    ถ้า (ht == NULL)
	คืน NULL;
    ret = find_var_in_ht(ht, *name, varname, no_autoload);
    ถ้า (ret != NULL)
	คืน ret;

    // Search in parent scope for lambda
    ret = find_var_in_scoped_ht(name, no_autoload);
    ถ้า (ret != NULL)
	คืน ret;

    // in Vim9 script items without a scope can be script-local
    ถ้า (in_vim9script() && name[0] != NUL && name[1] != ':')
    {
	ht = get_script_local_ht();
	ถ้า (ht != NULL)
	{
	    ret = find_var_in_ht(ht, *name, varname, no_autoload);
	    ถ้า (ret != NULL)
	    {
		ถ้า (htp != NULL)
		    *htp = ht;
		คืน ret;
	    }
	}
    }

    // and finally try
    คืน find_var_autoload_prefix(name, 0, htp, NULL);
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
find_var_autoload_prefix(char_u *name, จำนวนเต็ม sid, hashtab_T **htp,
							    char_u **namep)
{
    hashtab_T	*ht;
    dictitem_T	*ret = NULL;
    // When using "vim9script autoload" script-local items are prefixed but can
    // be used with s:name.
    จำนวนเต็ม check_sid = sid > 0 ? sid : current_sctx.sc_sid;
    ถ้า (SCRIPT_ID_VALID(check_sid)
		   && (in_vim9script() || (name[0] == 's' && name[1] == ':')))
    {
	scriptitem_T *si = SCRIPT_ITEM(check_sid);

	ถ้า (si->sn_autoload_prefix != NULL)
	{
	    char_u *base_name = (name[0] == 's' && name[1] == ':')
							     ? name + 2 : name;
	    char_u *auto_name = concat_str(si->sn_autoload_prefix, base_name);

	    ถ้า (auto_name != NULL)
	    {
		จำนวนเต็ม free_auto_name = TRUE;
		ht = &globvarht;
		ret = find_var_in_ht(ht, 'g', auto_name, TRUE);
		ถ้า (ret != NULL)
		{
		    ถ้า (htp != NULL)
			*htp = ht;
		    ถ้า (namep != NULL)
		    {
			free_auto_name = FALSE;
			*namep = auto_name;
		    }
		}
		ถ้า (free_auto_name)
		    vim_free(auto_name);
	    }
	}
    }

    คืน ret;
}

/*
 * Like find_var() but if the name starts with <SNR>99_ then look in the
 * referenced script (used for a funcref).
 */
    dictitem_T *
find_var_also_in_script(char_u *name, hashtab_T **htp, จำนวนเต็ม no_autoload)
{
    ถ้า (STRNCMP(name, "<SNR>", 5) == 0 && SAFE_isdigit(name[5]))
    {
	char_u	    *p = name + 5;
	จำนวนเต็ม	    sid = getdigits(&p);

	ถ้า (SCRIPT_ID_VALID(sid) && *p == '_')
	{
	    hashtab_T	*ht = &SCRIPT_VARS(sid);

	    ถ้า (ht != NULL)
	    {
		dictitem_T *di = find_var_in_ht(ht, 0, p + 1, no_autoload);

		ถ้า (di != NULL)
		{
		    ถ้า (htp != NULL)
			*htp = ht;
		    คืน di;
		}
	    }
	}
    }

    คืน find_var(name, htp, no_autoload);
}

/*
 * Find variable "varname" in hashtab "ht" with name "htname".
 * When "varname" is empty returns curwin/curtab/etc vars dictionary.
 * Returns NULL if not found.
 */
    dictitem_T *
find_var_in_ht(
    hashtab_T	*ht,
    จำนวนเต็ม		htname,
    char_u	*varname,
    จำนวนเต็ม		no_autoload)
{
    hashitem_T	*hi;

    ถ้า (*varname == NUL)
    {
	// Must be something like "s:", otherwise "ht" would be NULL.
	switch (htname)
	{
	    pak_sa 's': คืน &SCRIPT_SV(current_sctx.sc_sid)->sv_var;
	    pak_sa 'g': คืน &globvars_var;
	    pak_sa 'v': คืน &vimvars_var;
	    pak_sa 'b': คืน &curbuf->b_bufvar;
	    pak_sa 'w': คืน &curwin->w_winvar;
	    pak_sa 't': คืน &curtab->tp_winvar;
	    pak_sa 'l': คืน get_funccal_local_var();
	    pak_sa 'a': คืน get_funccal_args_var();
	}
	คืน NULL;
    }

    hi = hash_find(ht, varname);
    ถ้า (HASHITEM_EMPTY(hi))
    {
	// For global variables we may try auto-loading the script.  If it
	// worked find the variable again.  Don't auto-load a script if it was
	// loaded already, otherwise it would be loaded every time when
	// checking if a function name is a Funcref variable.
	ถ้า (ht == &globvarht && !no_autoload)
	{
	    // Note: script_autoload() may make "hi" invalid. It must either
	    // be obtained again or not used.
	    ถ้า (!script_autoload(varname, FALSE) || aborting())
		คืน NULL;
	    hi = hash_find(ht, varname);
	}
	ถ้า (HASHITEM_EMPTY(hi))
	    คืน NULL;
    }
    คืน HI2DI(hi);
}

/*
 * Get the script-local hashtab.  NULL if not in a script context.
 */
    hashtab_T *
get_script_local_ht(void)
{
    scid_T sid = current_sctx.sc_sid;

    ถ้า (SCRIPT_ID_VALID(sid))
	คืน &SCRIPT_VARS(sid);
    คืน NULL;
}

/*
 * Look for "name[len]" in script-local variables and functions.
 * When "cmd" is TRUE it must look like a command, a function must be followed
 * by "(" or "->".
 * Return OK when found, FAIL when not found.
 */
    จำนวนเต็ม
lookup_scriptitem(
	char_u	*name,
	size_t	len,
	จำนวนเต็ม	cmd,
	cctx_T	*dummy UNUSED)
{
    hashtab_T	*ht = get_script_local_ht();
    char_u	buffer[30];
    char_u	*p;
    จำนวนเต็ม		res;
    hashitem_T	*hi;
    จำนวนเต็ม		is_global = FALSE;
    char_u	*fname = name;

    ถ้า (ht == NULL)
	คืน FAIL;
    ถ้า (len < parimaa_na(buffer) - 1)
    {
	// avoid an alloc/free for short names
	vim_strncpy(buffer, name, len);
	p = buffer;
    }
    มิฉะนั้น
    {
	p = vim_strnsave(name, len);
	ถ้า (p == NULL)
	    คืน FAIL;
    }

    hi = hash_find(ht, p);
    res = HASHITEM_EMPTY(hi) ? FAIL : OK;

    // if not script-local, then perhaps autoload-exported
    ถ้า (res == FAIL && find_var_autoload_prefix(p, 0, NULL, NULL) != NULL)
	res = OK;

    // if not script-local or autoload, then perhaps imported
    ถ้า (res == FAIL && find_imported(p, 0, FALSE) != NULL)
	res = OK;
    ถ้า (p != buffer)
	vim_free(p);

    // Find a function, so that a following "->" works.
    // When used as a command require "(" or "->" to follow, "Cmd" is a user
    // command while "Cmd()" is a function call.
    ถ้า (res != OK)
    {
	p = skipwhite(name + len);

	ถ้า (!cmd || name[len] == '(' || (p[0] == '-' && p[1] == '>'))
	{
	    // Do not check for an internal function, since it might also be a
	    // valid command, such as ":split" versus "split()".
	    // Skip "g:" before a function name.
	    ถ้า (name[0] == 'g' && name[1] == ':')
	    {
		is_global = TRUE;
		fname = name + 2;
	    }
	    ถ้า (find_func(fname, is_global) != NULL)
		res = OK;
	}
    }

    คืน res;
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

    ถ้า (name[0] == NUL)
	คืน NULL;
    ถ้า (name[1] != ':')
    {
	// The name must not start with a colon or #.
	ถ้า (name[0] == ':' || name[0] == AUTOLOAD_CHAR)
	    คืน NULL;
	*varname = name;

	// "version" is "v:version" in all scopes if scriptversion < 3.
	// Same for a few other variables marked with VV_COMPAT.
	ถ้า (in_old_script(3))
	{
	    hi = hash_find(&compat_hashtab, name);
	    ถ้า (!HASHITEM_EMPTY(hi))
		คืน &compat_hashtab;
	}

	ht = get_funccal_local_ht();
	ถ้า (ht != NULL)
	    คืน ht;				// local variable

	// In Vim9 script items at the script level are script-local, except
	// for autoload names.
	ถ้า (in_vim9script() && vim_strchr(name, AUTOLOAD_CHAR) == NULL)
	{
	    ht = get_script_local_ht();
	    ถ้า (ht != NULL)
		คืน ht;
	}

	คืน &globvarht;			// global variable
    }
    *varname = name + 2;
    ถ้า (*name == 'g')				// global variable
	คืน &globvarht;
    // There must be no ':' or '#' in the rest of the name, unless g: is used
    ถ้า (vim_strchr(name + 2, ':') != NULL
			       || vim_strchr(name + 2, AUTOLOAD_CHAR) != NULL)
	คืน NULL;
    ถ้า (*name == 'b')				// buffer variable
	คืน &curbuf->b_vars->dv_hashtab;
    ถ้า (*name == 'w')				// window variable
	คืน &curwin->w_vars->dv_hashtab;
    ถ้า (*name == 't')				// tab page variable
	คืน &curtab->tp_vars->dv_hashtab;
    ถ้า (*name == 'v')				// v: variable
	คืน &vimvarht;
    ถ้า (get_current_funccal() != NULL
	       && get_current_funccal()->fc_func->uf_def_status
							    == UF_NOT_COMPILED)
    {
	// a: and l: are only used in functions defined with ":function"
	ถ้า (*name == 'a')			// a: function argument
	    คืน get_funccal_args_ht();
	ถ้า (*name == 'l')			// l: local function variable
	    คืน get_funccal_local_ht();
    }
    ถ้า (*name == 's')				// script variable
    {
	ht = get_script_local_ht();
	ถ้า (ht != NULL)
	    คืน ht;
    }
    คืน NULL;
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
    ถ้า (v == NULL)
	คืน NULL;
    คืน tv_get_string(&v->di_tv);
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
    ถ้า (sv == NULL)
	คืน;
    init_var_dict(&sv->sv_dict, &sv->sv_var, VAR_SCOPE);
    SCRIPT_ITEM(id)->sn_vars = sv;
}

/*
 * Initialize dictionary "dict" as a scope and set variable "dict_var" to
 * point to it.
 */
    void
init_var_dict(dict_T *dict, dictitem_T *dict_var, จำนวนเต็ม scope)
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
vars_clear_ext(hashtab_T *ht, จำนวนเต็ม free_val)
{
    จำนวนเต็ม		todo;
    hashitem_T	*hi;
    dictitem_T	*v;

    hash_lock(ht);
    todo = (จำนวนเต็ม)ht->ht_used;
    FOR_ALL_HASHTAB_ITEMS(ht, hi, todo)
    {
	ถ้า (!HASHITEM_EMPTY(hi))
	{
	    --todo;

	    // Free the variable.  Don't remove it from the hashtab,
	    // ht_array might change then.  hash_clear() takes care of it
	    // later.
	    v = HI2DI(hi);
	    ถ้า (free_val)
		clear_tv(&v->di_tv);
	    ถ้า (v->di_flags & DI_FLAGS_ALLOC)
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

    ถ้า (hash_remove(ht, hi, "delete variable") != OK)
	คืน;

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
    ถ้า (prefix == NULL)
	คืน;
    size_t  prefixlen = STRLEN(prefix);

    hash_lock(&globvarht);
    จำนวนเต็ม	todo = (จำนวนเต็ม)globvarht.ht_used;

    สำหรับ (hashitem_T *hi = globvarht.ht_array; todo > 0; ++hi)
	ถ้า (!HASHITEM_EMPTY(hi))
	{
	    dictitem_T	*di = HI2DI(hi);

	    --todo;
	    // Keep a class or enum: existing objects still refer to it.
	    ถ้า (di->di_tv.v_type != VAR_CLASS
		    && STRNCMP(di->di_key, prefix, prefixlen) == 0)
		delete_var(&globvarht, hi);
	}
    hash_unlock(&globvarht);
}

/*
 * List the value of one internal variable.
 */
    static void
list_one_var(dictitem_T *v, var_na *prefix, จำนวนเต็ม *first)
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
    จำนวนเต็ม		type,
    char_u	*string,
    จำนวนเต็ม		*first)  // when TRUE clear rest of screen and set to FALSE
{
    // don't use msg() or msg_attr() to avoid overwriting "v:statusmsg"
    msg_start();
    msg_puts(prefix);
    ถ้า (name != NULL)	// "a:" vars don't have a name stored
	msg_puts((var_na *)name);
    msg_putchar(' ');
    msg_advance(22);
    ถ้า (type == VAR_NUMBER)
	msg_putchar('#');
    มิฉะนั้น ถ้า (type == VAR_FUNC || type == VAR_PARTIAL)
	msg_putchar('*');
    มิฉะนั้น ถ้า (type == VAR_LIST)
    {
	msg_putchar('[');
	ถ้า (*string == '[')
	    ++string;
    }
    มิฉะนั้น ถ้า (type == VAR_DICT)
    {
	msg_putchar('{');
	ถ้า (*string == '{')
	    ++string;
    }
    มิฉะนั้น
	msg_putchar(' ');

    msg_outtrans(string);

    ถ้า (type == VAR_FUNC || type == VAR_PARTIAL)
	msg_puts("()");
    ถ้า (*first)
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
    จำนวนเต็ม
before_set_vvar(
    char_u	*varname,
    dictitem_T	*di,
    typval_T	*tv,
    จำนวนเต็ม		copy,
    จำนวนเต็ม		*type_error)
{
    ถ้า (di->di_tv.v_type == VAR_STRING)
    {
	VIM_CLEAR(di->di_tv.vval.v_string);
	ถ้า (copy || tv->v_type != VAR_STRING)
	{
	    char_u *val = tv_get_string(tv);

	    // Careful: when assigning to v:errmsg and
	    // tv_get_string() causes an error message the variable
	    // will already be set.
	    ถ้า (di->di_tv.vval.v_string == NULL)
		di->di_tv.vval.v_string = vim_strsave(val);
	}
	มิฉะนั้น
	{
	    // Take over the string to avoid an extra alloc/free.
	    di->di_tv.vval.v_string = tv->vval.v_string;
	    tv->vval.v_string = NULL;
	}
	คืน FALSE;
    }
    มิฉะนั้น ถ้า (di->di_tv.v_type == VAR_NUMBER)
    {
	di->di_tv.vval.v_number = tv_get_number(tv);
	ถ้า (STRCMP(varname, "searchforward") == 0)
	    set_search_direction(di->di_tv.vval.v_number ? '/' : '?');
#ifdef FEAT_SEARCH_EXTRA
	มิฉะนั้น ถ้า (STRCMP(varname, "hlsearch") == 0)
	{
	    no_hlsearch = !di->di_tv.vval.v_number;
	    redraw_all_later(UPD_SOME_VALID);
	}
#endif
	คืน FALSE;
    }
    มิฉะนั้น ถ้า (di->di_tv.v_type != tv->v_type)
    {
	*type_error = TRUE;
	คืน FALSE;
    }
    คืน TRUE;
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
    จำนวนเต็ม		copy)	    // make copy of value in "tv"
{
    set_var_const(name, 0, NULL, tv, copy, ASSIGN_DECL, 0);
}

/*
 * Set variable "name" to value in "tv_arg".
 * When "sid" is non-zero "name" is in the script with this ID.
 * If the variable already exists and "is_const" is FALSE the value is updated.
 * Otherwise the variable is created.
 */
    จำนวนเต็ม
set_var_const(
    char_u	*name,
    scid_T	sid,
    type_T	*type_arg,
    typval_T	*tv_arg,
    จำนวนเต็ม		copy,	    // make copy of value in "tv"
    จำนวนเต็ม		flags_arg,  // ASSIGN_CONST, ASSIGN_FINAL, etc.
    จำนวนเต็ม		var_idx)    // index for ":let [a, b] = list"
{
    typval_T	*tv = tv_arg;
    type_T	*type = type_arg;
    typval_T	bool_tv;
    dictitem_T	*di;
    typval_T	*dest_tv;
    char_u	*varname;
    char_u	*name_tofree = NULL;
    hashtab_T	*ht = NULL;
    จำนวนเต็ม		is_script_local;
    จำนวนเต็ม		vim9script = in_vim9script();
    จำนวนเต็ม		var_in_vim9script;
    จำนวนเต็ม		var_in_autoload = FALSE;
    จำนวนเต็ม		flags = flags_arg;
    จำนวนเต็ม		free_tv_arg = !copy;  // free tv_arg if not used
    จำนวนเต็ม		rc = FAIL;

    ถ้า (sid != 0)
    {
	varname = NULL;
	ถ้า (SCRIPT_ID_VALID(sid))
	{
	    char_u	*auto_name = NULL;
	    ถ้า (find_var_autoload_prefix(name, sid, &ht, &auto_name) != NULL)
	    {
		var_in_autoload = TRUE;
		varname = auto_name;
		name_tofree = varname;
	    }
	    มิฉะนั้น
		ht = &SCRIPT_VARS(sid);
	}
	ถ้า (varname == NULL)
	    varname = name;
    }
    มิฉะนั้น
    {
	scriptitem_T	*si;
	char_u		*auto_name = NULL;

	ถ้า (in_vim9script()
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
	    ถ้า (varname == NULL)
		goto failed;
	    name_tofree = varname;
	    ht = &globvarht;
	}
	มิฉะนั้น
	    ht = find_var_ht(name, &varname);
    }
    ถ้า (ht == NULL || *varname == NUL)
    {
	semsg(_(e_illegal_variable_name_str), name);
	goto failed;
    }
    is_script_local = ht == get_script_local_ht() || sid != 0
							    || var_in_autoload;

    ถ้า (vim9script
	    && !is_script_local
	    && (flags & (ASSIGN_NO_DECL | ASSIGN_DECL)) == 0
	    && (flags & (ASSIGN_CONST | ASSIGN_FINAL)) == 0
	    && name[1] == ':')
    {
	vim9_declare_error(name);
	goto failed;
    }
    ถ้า ((flags & ASSIGN_FOR_LOOP) && is_scoped_variable(name))
	// Do not make g:var, w:var, b:var or t:var final.
	flags &= ~ASSIGN_FINAL;

    var_in_vim9script = is_script_local && current_script_is_vim9();
    ถ้า (var_in_vim9script && name[0] == '_' && name[1] == NUL)
    {
	// For "[a, _] = list" the underscore is ignored.
	ถ้า ((flags & ASSIGN_UNPACK) == 0)
	    emsg(_(e_cannot_use_underscore_here));
	goto failed;
    }

    di = find_var_in_ht(ht, 0, varname, TRUE);

    ถ้า (di == NULL && var_in_vim9script)
    {
	imported_T  *import = find_imported(varname, 0, FALSE);

	ถ้า (import != NULL)
	{
	    // imported name space cannot be used
	    ถ้า ((flags & ASSIGN_NO_DECL) == 0)
	    {
		semsg(_(e_redefining_imported_item_str), name);
		goto failed;
	    }
	    semsg(_(e_cannot_use_str_itself_it_is_imported), name);
	    goto failed;
	}
	ถ้า (!in_vim9script())
	{
	    semsg(_(e_cannot_create_vim9_script_variable_in_function_str),
									 name);
	    goto failed;
	}
    }

    // Search in parent scope which is possible to reference from lambda
    ถ้า (di == NULL)
	di = find_var_in_scoped_ht(name, TRUE);

    ถ้า ((tv->v_type == VAR_FUNC || tv->v_type == VAR_PARTIAL)
				  && var_wrong_func_name(name, di == NULL))
	goto failed;

    ถ้า (need_convert_to_bool(type, tv))
    {
	// Destination is a bool and the value is not, but it can be
	// converted.
	CLEAR_FIELD(bool_tv);
	bool_tv.v_type = VAR_BOOL;
	bool_tv.vval.v_number = tv2bool(tv) ? VVAL_TRUE : VVAL_FALSE;
	tv = &bool_tv;
    }

    ถ้า (di != NULL)
    {
	// Item already exists.  Allowed to replace when reloading.
	ถ้า ((di->di_flags & DI_FLAGS_RELOAD) == 0)
	{
	    ถ้า ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					 && (flags & ASSIGN_FOR_LOOP) == 0)
	    {
		emsg(_(e_cannot_modify_existing_variable));
		goto failed;
	    }

	    ถ้า (is_script_local && vim9script
			  && (flags & (ASSIGN_NO_DECL | ASSIGN_DECL)) == 0)
	    {
		semsg(_(e_redefining_script_item_str), name);
		goto failed;
	    }

	    ถ้า (check_typval_is_value(&di->di_tv) == FAIL)
		goto failed;

	    // List and Blob types can be modified in-place using the "+="
	    // compound operator.  For other types, this is not allowed.
	    จำนวนเต็ม type_inplace_modifiable =
		(di->di_tv.v_type == VAR_LIST || di->di_tv.v_type == VAR_BLOB);

	    ถ้า (var_in_vim9script && (flags & ASSIGN_FOR_LOOP) == 0
		    && ((flags & ASSIGN_COMPOUND_OP) == 0
			|| !type_inplace_modifiable))
	    {
		where_T where = WHERE_INIT;
		svar_T  *sv = find_typval_in_script(&di->di_tv, sid, TRUE);

		ถ้า (sv != NULL)
		{
		    // check the type and adjust to bool if needed
		    ถ้า (var_idx > 0)
		    {
			where.wt_index = var_idx;
			where.wt_kind = WT_VARIABLE;
		    }
		    ถ้า (check_script_var_type(sv, tv, name, where) == FAIL)
			goto failed;
		    ถ้า (type == NULL)
			type = sv->sv_type;
		    sv->sv_flags |= SVFLAG_ASSIGNED;
		}
	    }

	    // Modifying a final variable with a List value using the "+="
	    // operator is allowed.  For other types, it is not allowed.
	    ถ้า ((((flags & ASSIGN_FOR_LOOP) == 0 || (flags & ASSIGN_DECL) == 0)
			&& ((flags & ASSIGN_COMPOUND_OP) == 0
			    || !type_inplace_modifiable))
				 ? var_check_permission(di, name) == FAIL
				 : var_check_ro(di->di_flags, name, FALSE))
		goto failed;
	}
	มิฉะนั้น
	{
	    // can only redefine once
	    di->di_flags &= ~DI_FLAGS_RELOAD;

	    // A Vim9 script-local variable is also present in sn_all_vars
	    // and sn_var_vals.  It may set "type" from "tv".
	    ถ้า (var_in_vim9script || var_in_autoload)
		update_vim9_script_var(FALSE, di,
			var_in_autoload ? name : di->di_key, flags,
			tv, &type, (flags & ASSIGN_NO_MEMBER_TYPE) == 0);
	}

	// existing variable, need to clear the value

	// In Vim9 script a String v: variable only takes a String.
	ถ้า (ht == &vimvarht && di->di_tv.v_type == VAR_STRING
							    && in_vim9script())
	{
	    where_T	where = WHERE_INIT;

	    ถ้า (check_typval_type(&t_string, tv, where) == FAIL)
		goto failed;
	}

	// Handle setting internal v: variables separately where needed to
	// prevent changing the type.
	จำนวนเต็ม type_error = FALSE;
	ถ้า (ht == &vimvarht
		&& !before_set_vvar(varname, di, tv, copy, &type_error))
	{
	    ถ้า (type_error)
		semsg(_(e_setting_v_str_to_value_with_wrong_type), varname);
	    goto failed;
	}

	clear_tv(&di->di_tv);

	ถ้า ((flags & ASSIGN_UPDATE_BLOCK_ID)
				       && SCRIPT_ID_VALID(current_sctx.sc_sid))
	{
	    scriptitem_T *si = SCRIPT_ITEM(current_sctx.sc_sid);

	    update_script_var_block_id(name, si->sn_current_block_id);
	}
    }
    มิฉะนั้น
    {
	// Item not found, check if a function already exists.
	ถ้า (is_script_local && (flags & (ASSIGN_NO_DECL | ASSIGN_DECL)) == 0
	       && lookup_scriptitem(name, STRLEN(name), FALSE, NULL) == OK)
	{
	    semsg(_(e_redefining_script_item_str), name);
	    goto failed;
	}

	// add a new variable
	ถ้า (var_in_vim9script && (flags & ASSIGN_NO_DECL))
	{
	    semsg(_(e_unknown_variable_str), name);
	    goto failed;
	}

	ถ้า (check_hashtab_frozen(ht, "add variable"))
	    goto failed;

	// Can't add "v:" or "a:" variable.
	ถ้า (ht == &vimvarht || ht == get_funccal_args_ht())
	{
	    semsg(_(e_illegal_variable_name_str), name);
	    goto failed;
	}

	// Make sure the variable name is valid.  In Vim9 script an
	// autoload variable must be prefixed with "g:" unless in an
	// autoload script.
	ถ้า (!valid_varname(varname, -1, !vim9script
			|| STRNCMP(name, "g:", 2) == 0 || var_in_autoload))
	    goto failed;

	di = alloc(offsetof(dictitem_T, di_key) + STRLEN(varname) + 1);
	ถ้า (di == NULL)
	    goto failed;
	STRCPY(di->di_key, varname);
	ถ้า (hash_add(ht, DI2HIKEY(di), "add variable") == FAIL)
	{
	    vim_free(di);
	    goto failed;
	}
	di->di_flags = DI_FLAGS_ALLOC;
	ถ้า (flags & (ASSIGN_CONST | ASSIGN_FINAL))
	    di->di_flags |= DI_FLAGS_LOCK;

	// A Vim9 script-local variable is also added to sn_all_vars and
	// sn_var_vals. It may set "type" from "tv".
	ถ้า (var_in_vim9script || var_in_autoload)
	    update_vim9_script_var(TRUE, di,
		    var_in_autoload ? name : di->di_key, flags,
			  tv, &type, (flags & ASSIGN_NO_MEMBER_TYPE) == 0);
    }

    dest_tv = &di->di_tv;
    ถ้า (copy || tv->v_type == VAR_NUMBER || tv->v_type == VAR_FLOAT)
	copy_tv(tv, dest_tv);
    มิฉะนั้น
    {
	*dest_tv = *tv;
	dest_tv->v_lock = 0;
	init_tv(tv);
    }
    free_tv_arg = FALSE;

    ถ้า (vim9script && type != NULL)
	set_tv_type(dest_tv, type);

    // ":const var = value" locks the value
    // ":final var = value" locks "var"
    ถ้า (flags & ASSIGN_CONST)
	// Like :lockvar! name: lock the value and what it contains, but only
	// if the reference count is up to one.  That locks only literal
	// values.
	item_lock(dest_tv, DICT_MAXNEST, TRUE, TRUE);

    rc = OK;

failed:
    vim_free(name_tofree);
    ถ้า (free_tv_arg)
	clear_tv(tv_arg);

    คืน rc;
}

/*
 * Check in this order for backwards compatibility:
 * - Whether the variable is read-only
 * - Whether the variable value is locked
 * - Whether the variable is locked
 * NOTE: "name" is only used for error messages.
 */
    จำนวนเต็ม
var_check_permission(dictitem_T *di, char_u *name)
{
    ถ้า (var_check_ro(di->di_flags, name, FALSE)
		    || value_check_lock(di->di_tv.v_lock, name, FALSE)
		    || var_check_lock(di->di_flags, name, FALSE))
	คืน FAIL;
    คืน OK;
}

/*
 * Return TRUE if di_flags "flags" indicates variable "name" is read-only.
 * Also give an error message.
 */
    จำนวนเต็ม
var_check_ro(จำนวนเต็ม flags, char_u *name, จำนวนเต็ม use_gettext)
{
    ถ้า (flags & DI_FLAGS_RO)
    {
	ถ้า (name == NULL)
	    emsg(_(e_cannot_change_readonly_variable));
	มิฉะนั้น
	    semsg(_(e_cannot_change_readonly_variable_str),
				       use_gettext ? (char_u *)_(name) : name);
	คืน TRUE;
    }
    ถ้า ((flags & DI_FLAGS_RO_SBX) && sandbox)
    {
	ถ้า (name == NULL)
	    emsg(_(e_cannot_set_variable_in_sandbox));
	มิฉะนั้น
	    semsg(_(e_cannot_set_variable_in_sandbox_str),
				       use_gettext ? (char_u *)_(name) : name);
	คืน TRUE;
    }
    คืน FALSE;
}

/*
 * Return TRUE if di_flags "flags" indicates variable "name" is locked.
 * Also give an error message.
 */
    จำนวนเต็ม
var_check_lock(จำนวนเต็ม flags, char_u *name, จำนวนเต็ม use_gettext)
{
    ถ้า (flags & DI_FLAGS_LOCK)
    {
	semsg(_(e_variable_is_locked_str),
				       use_gettext ? (char_u *)_(name) : name);
	คืน TRUE;
    }
    คืน FALSE;
}

/*
 * Return TRUE if di_flags "flags" indicates variable "name" is fixed.
 * Also give an error message.
 */
    จำนวนเต็ม
var_check_fixed(จำนวนเต็ม flags, char_u *name, จำนวนเต็ม use_gettext)
{
    ถ้า (flags & DI_FLAGS_FIX)
    {
	ถ้า (name == NULL)
	    emsg(_(e_cannot_delete_variable));
	มิฉะนั้น
	    semsg(_(e_cannot_delete_variable_str),
				      use_gettext ? (char_u *)_(name) : name);
	คืน TRUE;
    }
    คืน FALSE;
}

/*
 * Check if a funcref is assigned to a valid variable name.
 * Return TRUE and give an error if not.
 */
    จำนวนเต็ม
var_wrong_func_name(
    char_u *name,    // points to start of variable name
    จำนวนเต็ม    new_var)  // TRUE when creating the variable
{
    // Allow for w: b: s: and t:.  In Vim9 script s: is not allowed, because
    // the name can be used without the s: prefix.
    // Allow autoload variable.
    ถ้า (!((vim_strchr((char_u *)"wbt", name[0]) != NULL
		    || (!in_vim9script() && name[0] == 's')) && name[1] == ':')
	    && !ASCII_ISUPPER((name[0] != NUL && name[1] == ':')
						     ? name[2] : name[0])
	    && vim_strchr(name, '#') == NULL)
    {
	semsg(_(e_funcref_variable_name_must_start_with_capital_str), name);
	คืน TRUE;
    }
    // Don't allow hiding a function.  When "v" is not NULL we might be
    // assigning another function to the same var, the type is checked
    // below.
    ถ้า (new_var && function_exists(name, FALSE))
    {
	semsg(_(e_variable_name_conflicts_with_existing_function_str),
								    name);
	คืน TRUE;
    }
    คืน FALSE;
}

/*
 * Return TRUE if "flags" indicates variable "name" has a locked (immutable)
 * value.  Also give an error message, using "name" or _("name") when
 * "use_gettext" is TRUE.
 */
    จำนวนเต็ม
value_check_lock(จำนวนเต็ม lock, char_u *name, จำนวนเต็ม use_gettext)
{
    ถ้า (lock & VAR_LOCKED)
    {
	ถ้า (name == NULL)
	    emsg(_(e_value_is_locked));
	มิฉะนั้น
	    semsg(_(e_value_is_locked_str),
				       use_gettext ? (char_u *)_(name) : name);
	คืน TRUE;
    }
    ถ้า (lock & VAR_FIXED)
    {
	ถ้า (name == NULL)
	    emsg(_(e_cannot_change_value));
	มิฉะนั้น
	    semsg(_(e_cannot_change_value_of_str),
				       use_gettext ? (char_u *)_(name) : name);
	คืน TRUE;
    }
    คืน FALSE;
}

/*
 * Check if a variable name is valid.  When "autoload" is true "#" is allowed.
 * If "len" is -1 use all of "varname", otherwise up to "varname[len]".
 * Return FALSE and give an error if not.
 */
    จำนวนเต็ม
valid_varname(char_u *varname, จำนวนเต็ม len, จำนวนเต็ม autoload)
{
    char_u *p;

    สำหรับ (p = varname; len < 0 ? *p != NUL : p < varname + len; ++p)
	ถ้า (!eval_isnamec1(*p) && (p == varname || !VIM_ISDIGIT(*p))
					 && !(autoload && *p == AUTOLOAD_CHAR))
	{
	    semsg(_(e_illegal_variable_name_str), varname);
	    คืน FALSE;
	}
    คืน TRUE;
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
    จำนวนเต็ม		htname,	    // 't'ab, 'w'indow or 'b'uffer local.
    tabpage_T	*tp,	    // can be NULL
    win_T	*win,
    buf_T	*buf)	    // Ignored if htname is not 'b'.
{
    dictitem_T	*v;
    จำนวนเต็ม		done = FALSE;
    switchwin_T	switchwin;
    จำนวนเต็ม		need_switch_win;
    จำนวนเต็ม		do_change_curbuf = buf != NULL && htname == 'b';

    ++emsg_off;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;

    ถ้า (varname != NULL && tp != NULL && win != NULL
	    && (htname != 'b' || buf != NULL))
    {
	// Set curwin to be our win, temporarily.  Also set the tabpage,
	// otherwise the window is not valid. Only do this when needed,
	// autocommands get blocked.
	// If we have a buffer reference avoid the switching, we're saving and
	// restoring curbuf directly.
	need_switch_win = !(tp == curtab && win == curwin) && !do_change_curbuf;
	ถ้า (!need_switch_win || switch_win(&switchwin, win, tp, TRUE) == OK)
	{
	    // Handle options. There are no tab-local options.
	    ถ้า (*varname == '&' && htname != 't')
	    {
		buf_T	*save_curbuf = curbuf;

		// Change curbuf so the option is read from the correct buffer.
		ถ้า (do_change_curbuf)
		    curbuf = buf;

		ถ้า (varname[1] == NUL)
		{
		    // get all window-local or buffer-local options in a dict
		    dict_T	*opts = get_winbuf_options(htname == 'b');

		    ถ้า (opts != NULL)
		    {
			rettv_dict_set(rettv, opts);
			done = TRUE;
		    }
		}
		มิฉะนั้น ถ้า (eval_option(&varname, rettv, TRUE) == OK)
		    // Local option
		    done = TRUE;

		curbuf = save_curbuf;
	    }
	    มิฉะนั้น ถ้า (*varname == NUL)
	    {
		// Empty string: return a dict with all the local variables.
		ถ้า (htname == 'b')
		    v = &buf->b_bufvar;
		มิฉะนั้น ถ้า (htname == 'w')
		    v = &win->w_winvar;
		มิฉะนั้น
		    v = &tp->tp_winvar;
		copy_tv(&v->di_tv, rettv);
		done = TRUE;
	    }
	    มิฉะนั้น
	    {
		hashtab_T	*ht;

		ถ้า (htname == 'b')
		    ht = &buf->b_vars->dv_hashtab;
		มิฉะนั้น ถ้า (htname == 'w')
		    ht = &win->w_vars->dv_hashtab;
		มิฉะนั้น
		    ht = &tp->tp_vars->dv_hashtab;

		// Look up the variable.
		v = find_var_in_ht(ht, htname, varname, FALSE);
		ถ้า (v != NULL)
		{
		    copy_tv(&v->di_tv, rettv);
		    done = TRUE;
		}
	    }
	}

	ถ้า (need_switch_win)
	    // restore previous notion of curwin
	    restore_win(&switchwin, TRUE);
    }

    ถ้า (!done && deftv->v_type != VAR_UNKNOWN)
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
    จำนวนเต็ม		off)	    // 1 for gettabwinvar()
{
    char_u	*varname;
    tabpage_T	*tp;
    win_T	*win;

    ถ้า (off == 1)
	tp = find_tabpage((จำนวนเต็ม)tv_get_number_chk(&argvars[0], NULL));
    มิฉะนั้น
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
    จำนวนเต็ม		error = FALSE;

    จำนวนเต็ม		opt_idx = findoption(varname);
    ถ้า (opt_idx < 0)
    {
	semsg(_(e_unknown_option_str_2), varname);
	คืน;
    }
    จำนวนเต็ม		opt_p_flags = get_option_flags(opt_idx);

    ถ้า (varp->v_type == VAR_BOOL)
    {
	ถ้า (opt_p_flags & P_STRING)
	{
	    emsg(_(e_string_required));
	    คืน;
	}
	numval = (long)varp->vval.v_number;
	strval = (char_u *)"0";  // avoid using "false"
    }
    มิฉะนั้น
    {
	ถ้า ((opt_p_flags & (P_NUM|P_BOOL))
		&& (!in_vim9script() || varp->v_type != VAR_STRING))
	    numval = (long)tv_get_number_chk(varp, &error);
	ถ้า (!error)
	    strval = tv_get_string_buf_chk(varp, nbuf);
    }
    ถ้า (!error && strval != NULL)
	set_option_value_give_err(varname, numval, strval, OPT_LOCAL);
}

/*
 * "setwinvar()" and "settabwinvar()" functions
 */
    static void
setwinvar(typval_T *argvars, จำนวนเต็ม off)
{
    win_T	*win;
    switchwin_T	switchwin;
    จำนวนเต็ม		need_switch_win;
    char_u	*varname, *winvarname;
    typval_T	*varp;
    tabpage_T	*tp = NULL;

    ถ้า (check_secure())
	คืน;

    ถ้า (off == 1)
	tp = find_tabpage((จำนวนเต็ม)tv_get_number_chk(&argvars[0], NULL));
    มิฉะนั้น
	tp = curtab;
    win = find_win_by_nr(&argvars[off], tp);
    varname = tv_get_string_chk(&argvars[off + 1]);
    varp = &argvars[off + 2];

    ถ้า (win == NULL || varname == NULL)
	คืน;

    need_switch_win = !(tp == curtab && win == curwin);
    ถ้า (!need_switch_win
	    || switch_win(&switchwin, win, tp, TRUE) == OK)
    {
	ถ้า (*varname == '&')
	    set_option_from_tv(varname + 1, varp);
	มิฉะนั้น
	{
	    winvarname = alloc(STRLEN(varname) + 3);
	    ถ้า (winvarname != NULL)
	    {
		STRCPY(winvarname, "w:");
		STRCPY(winvarname + 2, varname);
		set_var(winvarname, varp, TRUE);
		vim_free(winvarname);
	    }
	}
    }
    ถ้า (need_switch_win)
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

    ถ้า (vp->vv_tv_type != VAR_LIST || vimvars[VV_ERRORS].vv_list == NULL)
	// Make sure v:errors is a list.
	set_vim_var_list(VV_ERRORS, list_alloc());
    list_append_string(vimvars[VV_ERRORS].vv_list, gap->ga_data, gap->ga_len);
}

    จำนวนเต็ม
var_exists(char_u *var)
{
    char_u	*arg = var;
    char_u	*name;
    char_u	*tofree;
    typval_T    tv;
    จำนวนเต็ม		len = 0;
    จำนวนเต็ม		n = FALSE;

    // get_name_len() takes care of expanding curly braces
    name = var;
    len = get_name_len(&arg, &tofree, TRUE, FALSE);
    ถ้า (len > 0)
    {
	ถ้า (tofree != NULL)
	    name = tofree;
	n = (eval_variable(name, len, 0, &tv, NULL,
				 EVAL_VAR_NOAUTOLOAD + EVAL_VAR_IMPORT) == OK);
	ถ้า (n)
	{
	    // handle d.key, l[idx], f(expr)
	    arg = skipwhite(arg);
	    n = (handle_subscript(&arg, name, &tv, &EVALARG_EVALUATE,
								 FALSE) == OK);
	    ถ้า (n)
		clear_tv(&tv);
	}
    }
    ถ้า (*arg != NUL)
	n = FALSE;

    vim_free(tofree);
    คืน n;
}

static lval_T	*redir_lval = NULL;
#define EVALCMD_BUSY (redir_lval == (lval_T *)&redir_lval)
static garray_T redir_ga;	// only valid when redir_lval is not NULL
static char_u	*redir_endp = NULL;
static char_u	*redir_varname = NULL;

    จำนวนเต็ม
alloc_redir_lval(void)
{
    redir_lval = ALLOC_CLEAR_ONE(lval_T);
    ถ้า (redir_lval == NULL)
	คืน FAIL;
    คืน OK;
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
    จำนวนเต็ม
var_redir_start(char_u *name, จำนวนเต็ม append)
{
    จำนวนเต็ม		called_emsg_before;
    typval_T	tv;

    // Catch a bad name early.
    ถ้า (!eval_isnamec1(*name))
    {
	emsg(_(e_invalid_argument));
	คืน FAIL;
    }

    // Make a copy of the name, it is used in redir_lval until redir ends.
    redir_varname = vim_strsave(name);
    ถ้า (redir_varname == NULL)
	คืน FAIL;

    ถ้า (alloc_redir_lval() == FAIL)
    {
	var_redir_stop();
	คืน FAIL;
    }

    // The output is stored in growarray "redir_ga" until redirection ends.
    init_redir_ga();

    // Parse the variable name (can be a dict or list entry).
    redir_endp = get_lval(redir_varname, NULL, redir_lval, FALSE, FALSE, 0,
							     FNE_CHECK_START);
    ถ้า (redir_endp == NULL || redir_lval->ll_name == NULL || *redir_endp != NUL)
    {
	clear_lval(redir_lval);
	ถ้า (redir_endp != NULL && *redir_endp != NUL)
	    // Trailing characters are present after the variable name
	    semsg(_(e_trailing_characters_str), redir_endp);
	มิฉะนั้น
	    semsg(_(e_invalid_argument_str), name);
	redir_endp = NULL;  // don't store a value, only cleanup
	var_redir_stop();
	คืน FAIL;
    }

    // check if we can write to the variable: set it to or append an empty
    // string
    called_emsg_before = called_emsg;
    tv.v_type = VAR_STRING;
    tv.vval.v_string = (char_u *)"";
    ถ้า (append)
	set_var_lval(redir_lval, redir_endp, &tv, TRUE,
					     ASSIGN_NO_DECL, (char_u *)".", 0);
    มิฉะนั้น
	set_var_lval(redir_lval, redir_endp, &tv, TRUE,
					     ASSIGN_NO_DECL, (char_u *)"=", 0);
    clear_lval(redir_lval);
    ถ้า (called_emsg > called_emsg_before)
    {
	redir_endp = NULL;  // don't store a value, only cleanup
	var_redir_stop();
	คืน FAIL;
    }

    คืน OK;
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
var_redir_str(char_u *value, จำนวนเต็ม value_len)
{
    จำนวนเต็ม		len;

    ถ้า (redir_lval == NULL)
	คืน;

    ถ้า (value_len == -1)
	len = (จำนวนเต็ม)STRLEN(value);	// Append the entire string
    มิฉะนั้น
	len = value_len;		// Append only "value_len" characters

    ถ้า (ga_grow(&redir_ga, len) == OK)
    {
	mch_memmove((var_na *)redir_ga.ga_data + redir_ga.ga_len, value, len);
	redir_ga.ga_len += len;
    }
    มิฉะนั้น
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

    ถ้า (EVALCMD_BUSY)
    {
	redir_lval = NULL;
	คืน;
    }

    ถ้า (redir_lval != NULL)
    {
	// If there was no error: assign the text to the variable.
	ถ้า (redir_endp != NULL)
	{
	    ga_append(&redir_ga, NUL);  // Append the trailing NUL.
	    tv.v_type = VAR_STRING;
	    tv.vval.v_string = redir_ga.ga_data;
	    // Call get_lval() again, if it's inside a Dict or List it may
	    // have changed.
	    redir_endp = get_lval(redir_varname, NULL, redir_lval,
					FALSE, FALSE, 0, FNE_CHECK_START);
	    ถ้า (redir_endp != NULL && redir_lval->ll_name != NULL)
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
    คืน res;
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

    ถ้า (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	คืน;

    varname = tv_get_string_chk(&argvars[1]);
    tp = find_tabpage((จำนวนเต็ม)tv_get_number_chk(&argvars[0], NULL));
    ถ้า (tp != NULL)
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
    ถ้า (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL))
	คืน;

    getwinvar(argvars, rettv, 1);
}

/*
 * "getwinvar()" function
 */
    void
f_getwinvar(typval_T *argvars, typval_T *rettv)
{
    ถ้า (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	คืน;

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

    ถ้า (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	คืน;

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

    ถ้า (check_secure())
	คืน;

    ถ้า (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	คืน;

    tp = find_tabpage((จำนวนเต็ม)tv_get_number_chk(&argvars[0], NULL));
    varname = tv_get_string_chk(&argvars[1]);
    varp = &argvars[2];

    ถ้า (varname == NULL || tp == NULL)
	คืน;

    save_curtab = curtab;
    save_lu_tp = lastused_tabpage;
    goto_tabpage_tp(tp, FALSE, FALSE);

    tabvarname = alloc(STRLEN(varname) + 3);
    ถ้า (tabvarname != NULL)
    {
	STRCPY(tabvarname, "t:");
	STRCPY(tabvarname + 2, varname);
	set_var(tabvarname, varp, TRUE);
	vim_free(tabvarname);
    }

    // Restore current tabpage and last accessed tabpage.
    ถ้า (valid_tabpage(save_curtab))
    {
	goto_tabpage_tp(save_curtab, FALSE, FALSE);
	ถ้า (valid_tabpage(save_lu_tp))
	    lastused_tabpage = save_lu_tp;
    }
}

/*
 * "settabwinvar()" function
 */
    void
f_settabwinvar(typval_T *argvars, typval_T *rettv UNUSED)
{
    ถ้า (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL))
	คืน;

    setwinvar(argvars, 1);
}

/*
 * "setwinvar()" function
 */
    void
f_setwinvar(typval_T *argvars, typval_T *rettv UNUSED)
{
    ถ้า (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	คืน;

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

    ถ้า (check_secure())
	คืน;

    ถ้า (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	คืน;

    varname = tv_get_string_chk(&argvars[1]);
    buf = tv_get_buf_from_arg(&argvars[0]);
    varp = &argvars[2];

    ถ้า (buf == NULL || varname == NULL)
	คืน;

    ถ้า (*varname == '&')
    {
	aco_save_T	aco;
	// safe the current window position, it could
	// change because of 'scrollbind' window-local
	// options
	linenr_T	old_topline = curwin->w_topline;

	// Set curbuf to be our buf, temporarily.
	aucmd_prepbuf(&aco, buf);
	ถ้า (curbuf == buf)
	{
	    // Only when it worked to set "curbuf".
	    set_option_from_tv(varname + 1, varp);

	    // reset notion of buffer
	    aucmd_restbuf(&aco);
	}
	curwin->w_topline = old_topline;
    }
    มิฉะนั้น
    {
	bufvarname = alloc(STRLEN(varname) + 3);
	ถ้า (bufvarname != NULL)
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
    จำนวนเต็ม		r = OK;

    CLEAR_FIELD(res);
    ถ้า (arg->v_type == VAR_PARTIAL && arg->vval.v_partial != NULL)
    {
	res.cb_partial = arg->vval.v_partial;
	++res.cb_partial->pt_refcount;
	res.cb_name = partial_name(res.cb_partial);
    }
    มิฉะนั้น
    {
	ถ้า (arg->v_type == VAR_STRING && arg->vval.v_string != NULL
					       && SAFE_isdigit(*arg->vval.v_string))
	    r = FAIL;
	มิฉะนั้น ถ้า (arg->v_type == VAR_FUNC || arg->v_type == VAR_STRING)
	{
	    res.cb_name = arg->vval.v_string;
	    ถ้า (arg->v_type == VAR_STRING)
	    {
		char_u *name = get_scriptlocal_funcname(arg->vval.v_string);
		ถ้า (name != NULL)
		{
		    res.cb_name = name;
		    res.cb_free_name = TRUE;
		}
	    }
	    func_ref(res.cb_name);
	}
	มิฉะนั้น ถ้า (arg->v_type == VAR_NUMBER && arg->vval.v_number == 0)
	    res.cb_name = (char_u *)"";
	มิฉะนั้น
	    r = FAIL;

	ถ้า (r == FAIL)
	{
	    emsg(_(e_invalid_callback_argument));
	    res.cb_name = NULL;
	}
    }
    คืน res;
}

/*
 * Copy a callback into a typval_T.
 */
    void
put_callback(callback_T *cb, typval_T *tv)
{
    ถ้า (cb->cb_partial != NULL)
    {
	tv->v_type = VAR_PARTIAL;
	tv->vval.v_partial = cb->cb_partial;
	++tv->vval.v_partial->pt_refcount;
    }
    มิฉะนั้น
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
    คืน cb->cb_partial || cb->cb_free_name;
}

/*
 * Make a copy of "src" into "dest", allocating the function name if needed,
 * without incrementing the refcount.
 */
    void
set_callback(callback_T *dest, callback_T *src)
{
    *dest = *src;
    ถ้า (!does_callback_own_cb_name(src))
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
    ถ้า (dest->cb_partial != NULL)
    {
	dest->cb_name = src->cb_name;
	dest->cb_free_name = FALSE;
	++dest->cb_partial->pt_refcount;
    }
    มิฉะนั้น
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

    ถ้า (!in_vim9script() || cb->cb_name == NULL
	    || (!cb->cb_free_name
	       && (cb->cb_partial == NULL || cb->cb_partial->pt_name == NULL)))
	คืน;
    ถ้า (cb->cb_partial != NULL)
	name = cb->cb_partial->pt_name;
    มิฉะนั้น
	name = cb->cb_name;
    p = vim_strchr(name, '.');
    ถ้า (p == NULL)
	คืน;

    import = find_imported(name, p - name, FALSE);
    ถ้า (import == NULL || !SCRIPT_ID_VALID(import->imp_sid))
	คืน;

    scriptitem_T *si = SCRIPT_ITEM(import->imp_sid);
    ถ้า (si->sn_autoload_prefix == NULL)
	คืน;

    char_u *newname = concat_str(si->sn_autoload_prefix, p + 1);
    ถ้า (newname == NULL)
	คืน;

    ถ้า (cb->cb_partial != NULL)
    {
	ถ้า (cb->cb_name == cb->cb_partial->pt_name)
	    cb->cb_name = newname;
	vim_free(cb->cb_partial->pt_name);
	cb->cb_partial->pt_name = newname;
    }
    มิฉะนั้น
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
    ถ้า (callback->cb_partial != NULL)
    {
	partial_unref(callback->cb_partial);
	callback->cb_partial = NULL;
    }
    มิฉะนั้น ถ้า (callback->cb_name != NULL)
	func_unref(callback->cb_name);
    ถ้า (callback->cb_free_name)
    {
	vim_free(callback->cb_name);
	callback->cb_free_name = FALSE;
    }
    callback->cb_name = NULL;
}

#endif // FEAT_EVAL
