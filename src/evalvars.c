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

#만약 defined(FEAT_EVAL)

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

typedef struct vimvar vimvar_T;

static struct vimvar
{
    char	*vv_name;	// name of variable, without v:
    dictitem16_T vv_di;		// value and name for key (max 16 chars!)
    type_T	*vv_type;	// type or NULL
    char	vv_flags;	// VV_COMPAT, VV_RO, VV_RO_SBX
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

static void list_glob_vars(정수 *first);
static void list_buf_vars(정수 *first);
static void list_win_vars(정수 *first);
static void list_tab_vars(정수 *first);
static char_u *list_arg_vars(exarg_T *eap, char_u *arg, 정수 *first);
static char_u *ex_let_one(char_u *arg, typval_T *tv, 정수 copy, 정수 flags, char_u *endchars, char_u *op, 정수 var_idx);
static 정수 do_unlet_var(lval_T *lp, char_u *name_end, exarg_T *eap, 정수 deep, void *cookie);
static 정수 do_lock_var(lval_T *lp, char_u *name_end, exarg_T *eap, 정수 deep, void *cookie);
static void list_one_var(dictitem_T *v, char *prefix, 정수 *first);
static void list_one_var_a(char *prefix, char_u *name, 정수 type, char_u *string, 정수 *first);

/*
 * Initialize global and vim special variables
 */
    void
evalvars_init(void)
{
    정수		    i;
    struct vimvar   *p;

    init_var_dict(&globvardict, &globvars_var, VAR_DEF_SCOPE);
    init_var_dict(&vimvardict, &vimvars_var, VAR_SCOPE);
    vimvardict.dv_lock = VAR_FIXED;
    hash_init(&compat_hashtab);

    위해 (i = 0; i < VV_LEN; ++i)
    {
	p = &vimvars[i];
	만약 (p->vv_flags & VV_RO)
	    p->vv_di.di_flags = DI_FLAGS_RO | DI_FLAGS_FIX;
	아니면 만약 (p->vv_flags & VV_RO_SBX)
	    p->vv_di.di_flags = DI_FLAGS_RO_SBX | DI_FLAGS_FIX;
	아니면
	    p->vv_di.di_flags = DI_FLAGS_FIX;

	// add to v: scope dict, unless the value is not always available
	만약 (p->vv_tv_type != VAR_UNKNOWN)
	    hash_add(&vimvarht, p->vv_di.di_key, "initialization");
	만약 (p->vv_flags & VV_COMPAT)
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
    set_vim_var_nr(VV_NUMBERSIZE, sizeof(varnumber_T) * 8);
    set_vim_var_nr(VV_SIZEOFINT, sizeof(정수));
    set_vim_var_nr(VV_SIZEOFLONG, sizeof(long));
    set_vim_var_nr(VV_SIZEOFPOINTER, sizeof(char *));
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

#만약 defined(EXITFREE)
/*
 * Free all vim variables information on exit
 */
    void
evalvars_clear(void)
{
    정수		    i;
    struct vimvar   *p;

    위해 (i = 0; i < VV_LEN; ++i)
    {
	p = &vimvars[i];
	만약 (p->vv_di.di_tv.v_type == VAR_STRING)
	    VIM_CLEAR(p->vv_str);
	아니면 만약 (p->vv_di.di_tv.v_type == VAR_LIST)
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
    위해 (i = 1; i <= script_items.ga_len; ++i)
	vars_clear(&SCRIPT_VARS(i));
}
#endif

    정수
garbage_collect_globvars(정수 copyID)
{
    반환 set_ref_in_ht(&globvarht, copyID, NULL, NULL);
}

    정수
garbage_collect_vimvars(정수 copyID)
{
    반환 set_ref_in_ht(&vimvarht, copyID, NULL, NULL);
}

    정수
garbage_collect_scriptvars(정수 copyID)
{
    정수		    i;
    정수		    idx;
    정수		    abort = FALSE;
    scriptitem_T    *si;

    위해 (i = 1; i <= script_items.ga_len; ++i)
    {
	abort = abort || set_ref_in_ht(&SCRIPT_VARS(i), copyID, NULL, NULL);

	si = SCRIPT_ITEM(i);
	위해 (idx = 0; idx < si->sn_var_vals.ga_len; ++idx)
	{
	    svar_T    *sv = ((svar_T *)si->sn_var_vals.ga_data) + idx;

	    만약 (sv->sv_name != NULL)
		abort = abort || set_ref_in_item(sv->sv_tv, copyID, NULL, NULL, NULL);
	}
    }

    반환 abort;
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
    만약 (val == NULL)
	반환;

    tvp = alloc_string_tv(val);
    만약 (tvp == NULL)
	반환;

    set_var(name, tvp, FALSE);
    free_tv(tvp);
}

    정수
eval_charconvert(
    char_u	*enc_from,
    char_u	*enc_to,
    char_u	*fname_from,
    char_u	*fname_to)
{
    정수		err = FALSE;
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;

    set_vim_var_string(VV_CC_FROM, enc_from, -1);
    set_vim_var_string(VV_CC_TO, enc_to, -1);
    set_vim_var_string(VV_FNAME_IN, fname_from, -1);
    set_vim_var_string(VV_FNAME_OUT, fname_to, -1);
    ctx = get_option_sctx("charconvert");
    만약 (ctx != NULL)
	current_sctx = *ctx;

    만약 (eval_to_bool(p_ccv, &err, NULL, FALSE, TRUE))
	err = TRUE;

    set_vim_var_string(VV_CC_FROM, NULL, -1);
    set_vim_var_string(VV_CC_TO, NULL, -1);
    set_vim_var_string(VV_FNAME_IN, NULL, -1);
    set_vim_var_string(VV_FNAME_OUT, NULL, -1);
    current_sctx = saved_sctx;

    만약 (err)
	반환 FAIL;
    반환 OK;
}

#만약 defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
    정수
eval_printexpr(char_u *fname, char_u *args)
{
    정수		err = FALSE;
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;

    set_vim_var_string(VV_FNAME_IN, fname, -1);
    set_vim_var_string(VV_CMDARG, args, -1);
    ctx = get_option_sctx("printexpr");
    만약 (ctx != NULL)
	current_sctx = *ctx;

    만약 (eval_to_bool(p_pexpr, &err, NULL, FALSE, TRUE))
	err = TRUE;

    set_vim_var_string(VV_FNAME_IN, NULL, -1);
    set_vim_var_string(VV_CMDARG, NULL, -1);
    current_sctx = saved_sctx;

    만약 (err)
    {
	mch_remove(fname);
	반환 FAIL;
    }
    반환 OK;
}
#endif

#만약 defined(FEAT_DIFF)
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
    만약 (ctx != NULL)
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
    만약 (ctx != NULL)
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

#만약 defined(FEAT_SPELL)
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
    정수		r;

    // Set "v:val" to the bad word.
    prepare_vimvar(VV_VAL, &save_val);
    set_vim_var_string(VV_VAL, badword, -1);
    만약 (p_verbose == 0)
	++emsg_off;
    ctx = get_option_sctx("spellsuggest");
    만약 (ctx != NULL)
	current_sctx = *ctx;

    r = may_call_simple_func(p, &rettv);
    만약 (r == NOTDONE)
	r = eval1(&p, &rettv, &EVALARG_EVALUATE);
    만약 (r == OK)
    {
	만약 (rettv.v_type != VAR_LIST)
	    clear_tv(&rettv);
	아니면
	    list = rettv.vval.v_list;
    }

    만약 (p_verbose == 0)
	--emsg_off;
    clear_tv(get_vim_var_tv(VV_VAL));
    restore_vimvar(VV_VAL, &save_val);
    current_sctx = saved_sctx;

    반환 list;
}

/*
 * "list" is supposed to contain two items: a word and a number.  Return the
 * word in "pp" and the number as the return value.
 * Return -1 if anything isn't right.
 * Used to get the good word and score from the eval_spell_expr() result.
 */
    정수
get_spellword(list_T *list, char_u **pp)
{
    listitem_T	*li;

    li = list->lv_first;
    만약 (li == NULL)
	반환 -1;
    *pp = tv_get_string(&li->li_tv);

    li = li->li_next;
    만약 (li == NULL)
	반환 -1;
    반환 (정수)tv_get_number(&li->li_tv);
}
#endif

/*
 * Prepare v: variable "idx" to be used.
 * Save the current typeval in "save_tv" and clear it.
 * When not used yet add the variable to the v: hashtable.
 */
    void
prepare_vimvar(정수 idx, typval_T *save_tv)
{
    *save_tv = vimvars[idx].vv_tv;
    vimvars[idx].vv_str = NULL;  // don't free it now
    만약 (vimvars[idx].vv_tv_type == VAR_UNKNOWN)
	hash_add(&vimvarht, vimvars[idx].vv_di.di_key, "prepare vimvar");
}

/*
 * Restore v: variable "idx" to typeval "save_tv".
 * Note that the v: variable must have been cleared already.
 * When no longer defined, remove the variable from the v: hashtable.
 */
    void
restore_vimvar(정수 idx, typval_T *save_tv)
{
    hashitem_T	*hi;

    vimvars[idx].vv_tv = *save_tv;
    만약 (vimvars[idx].vv_tv_type != VAR_UNKNOWN)
	반환;

    hi = hash_find(&vimvarht, vimvars[idx].vv_di.di_key);
    만약 (HASHITEM_EMPTY(hi))
	internal_error("restore_vimvar()");
    아니면
	hash_remove(&vimvarht, hi, "restore vimvar");
}

/*
 * List Vim variables.
 */
    static void
list_vim_vars(정수 *first)
{
    list_hashtable_vars(&vimvarht, "v:", FALSE, first);
}

/*
 * List script-local variables, if there is a script.
 */
    static void
list_script_vars(정수 *first)
{
    만약 (SCRIPT_ID_VALID(current_sctx.sc_sid))
	list_hashtable_vars(&SCRIPT_VARS(current_sctx.sc_sid),
							   "s:", FALSE, first);
}

/*
 * Return TRUE if "name" starts with "g:", "w:", "t:" or "b:".
 * But only when an identifier character follows.
 */
    정수
is_scoped_variable(char_u *name)
{
    반환 vim_strchr((char_u *)"gwbt", name[0]) != NULL
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
eval_one_expr_in_str(char_u *p, garray_T *gap, 정수 evaluate)
{
    char_u	*block_start = skipwhite(p + 1);  // skip the opening {
    char_u	*block_end = block_start;
    char_u	*expr_val;

    만약 (*block_start == NUL)
    {
	semsg(_(e_missing_close_curly_str), p);
	반환 NULL;
    }
    만약 (skip_expr(&block_end, NULL) == FAIL)
	반환 NULL;
    block_end = skipwhite(block_end);
    만약 (*block_end != '}')
    {
	semsg(_(e_missing_close_curly_str), p);
	반환 NULL;
    }
    만약 (evaluate)
    {
	*block_end = NUL;
	expr_val = eval_to_string(block_start, FALSE, FALSE);
	*block_end = '}';
	만약 (expr_val == NULL)
	    반환 NULL;
	ga_concat(gap, expr_val);
	vim_free(expr_val);
    }

    반환 block_end + 1;
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

    동안 (*p != NUL)
    {
	char_u	*lit_start;
	정수	escaped_brace = FALSE;

	// Look for a block start.
	lit_start = p;
	동안 (*p != '{' && *p != '}' && *p != NUL)
	    ++p;

	만약 (*p != NUL && *p == p[1])
	{
	    // Escaped brace, unescape and continue.
	    // Include the brace in the literal string.
	    ++p;
	    escaped_brace = TRUE;
	}
	아니면 만약 (*p == '}')
	{
	    semsg(_(e_stray_closing_curly_str), str);
	    ga_clear(&ga);
	    반환 NULL;
	}

	// Append the literal part.
	ga_concat_len(&ga, lit_start, (size_t)(p - lit_start));

	만약 (*p == NUL)
	    break;

	만약 (escaped_brace)
	{
	    // Skip the second brace.
	    ++p;
	    continue;
	}

	// Evaluate the expression and append the result.
	p = eval_one_expr_in_str(p, &ga, TRUE);
	만약 (p == NULL)
	{
	    ga_clear(&ga);
	    반환 NULL;
	}
    }
    ga_append(&ga, NUL);

    반환 ga.ga_data;
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
heredoc_get(exarg_T *eap, char_u *cmd, 정수 script_get, 정수 vim9compile)
{
    char_u	*theline = NULL;
    char_u	*marker;
    list_T	*l;
    char_u	*p;
    char_u	*str;
    정수		marker_indent_len = 0;
    정수		text_indent_len = 0;
    char_u	*text_indent = NULL;
    char_u	dot[] = ".";
    정수		comment_char = in_vim9script() ? '#' : '"';
    정수		evalstr = FALSE;
    정수		eval_failed = FALSE;
    cctx_T	*cctx = vim9compile ? eap->cookie : NULL;
    정수		count = 0;
    정수		heredoc_in_string = FALSE;
    char_u	*line_arg = NULL;
    char_u	*nl_ptr = vim_strchr(cmd, '\n');

    만약 (nl_ptr != NULL)
    {
	heredoc_in_string = TRUE;
	line_arg = nl_ptr + 1;
	*nl_ptr = NUL;
    }
    아니면 만약 (eap->ea_getline == NULL)
    {
	emsg(_(e_cannot_use_heredoc_here));
	반환 NULL;
    }

    // Check for the optional 'trim' word before the marker
    cmd = skipwhite(cmd);

    동안 (TRUE)
    {
	만약 (STRNCMP(cmd, "trim", 4) == 0
		&& (cmd[4] == NUL || VIM_ISWHITE(cmd[4])))
	{
	    cmd = skipwhite(cmd + 4);

	    // Trim the indentation from all the lines in the here document.
	    // The amount of indentation trimmed is the same as the indentation
	    // of the first line after the :let command line.  To find the end
	    // marker the indent of the :let command line is trimmed.
	    p = *eap->cmdlinep;
	    동안 (VIM_ISWHITE(*p))
	    {
		p++;
		marker_indent_len++;
	    }
	    text_indent_len = -1;

	    continue;
	}
	만약 (STRNCMP(cmd, "eval", 4) == 0
		&& (cmd[4] == NUL || VIM_ISWHITE(cmd[4])))
	{
	    cmd = skipwhite(cmd + 4);
	    evalstr = TRUE;
	    continue;
	}
	break;
    }

    // The marker is the next word.
    만약 (*cmd != NUL && *cmd != comment_char)
    {
	marker = skipwhite(cmd);
	p = skiptowhite(marker);
	만약 (*skipwhite(p) != NUL && *skipwhite(p) != comment_char)
	{
	    semsg(_(e_trailing_characters_str), p);
	    반환 NULL;
	}
	*p = NUL;
	만약 (!script_get && vim_islower(*marker))
	{
	    emsg(_(e_marker_cannot_start_with_lower_case_letter));
	    반환 NULL;
	}
    }
    아니면
    {
	// When getting lines for an embedded script, if the marker is missing,
	// accept '.' as the marker.
	만약 (script_get)
	    marker = dot;
	아니면
	{
	    emsg(_(e_missing_marker));
	    반환 NULL;
	}
    }

    l = list_alloc();
    만약 (l == NULL)
	반환 NULL;

    위해 (;;)
    {
	정수	mi = 0;
	정수	ti = 0;

	만약 (heredoc_in_string)
	{
	    char_u	*next_line;

	    // heredoc in a string separated by newlines.  Get the next line
	    // from the string.

	    만약 (*line_arg == NUL)
	    {
		semsg(_(e_missing_end_marker_str), marker);
		break;
	    }

	    theline = line_arg;
	    next_line = vim_strchr(theline, '\n');
	    만약 (next_line == NULL)
		line_arg += STRLEN(line_arg);
	    아니면
	    {
		*next_line = NUL;
		line_arg = next_line + 1;
	    }
	}
	아니면
	{
	    vim_free(theline);
	    theline = eap->ea_getline(NUL, eap->cookie, 0, FALSE);
	    만약 (theline == NULL)
	    {
		semsg(_(e_missing_end_marker_str), marker);
		break;
	    }
	}

	// with "trim": skip the indent matching the :let line to find the
	// marker
	만약 (marker_indent_len > 0
		&& STRNCMP(theline, *eap->cmdlinep, marker_indent_len) == 0)
	    mi = marker_indent_len;
	만약 (STRCMP(marker, theline + mi) == 0)
	    break;

	// If expression evaluation failed in the heredoc, then skip till the
	// end marker.
	만약 (eval_failed)
	    continue;

	만약 (text_indent_len == -1 && *theline != NUL)
	{
	    // set the text indent from the first line.
	    p = theline;
	    text_indent_len = 0;
	    동안 (VIM_ISWHITE(*p))
	    {
		p++;
		text_indent_len++;
	    }
	    text_indent = vim_strnsave(theline, text_indent_len);
	}
	// with "trim": skip the indent matching the first line
	만약 (text_indent != NULL)
	    위해 (ti = 0; ti < text_indent_len; ++ti)
		만약 (theline[ti] != text_indent[ti])
		    break;

	str = theline + ti;
	만약 (vim9compile)
	{
	    만약 (compile_all_expr_in_str(str, evalstr, cctx) == FAIL)
	    {
		vim_free(theline);
		vim_free(text_indent);
		list_free(l);
		반환 FAIL;
	    }
	    count++;
	}
	아니면
	{
	    정수	    free_str = FALSE;

	    만약 (evalstr && !eap->skip)
	    {
		str = eval_all_expr_in_str(str);
		만약 (str == NULL)
		{
		    // expression evaluation failed
		    eval_failed = TRUE;
		    continue;
		}
		free_str = TRUE;
	    }

	    만약 (list_append_string(l, str, -1) == FAIL)
	    {
		만약 (free_str)
		    vim_free(str);
		break;
	    }
	    만약 (free_str)
		vim_free(str);
	}
    }
    만약 (heredoc_in_string)
	// Next command follows the heredoc in the string.
	eap->nextcmd = line_arg;
    아니면
	vim_free(theline);
    vim_free(text_indent);

    만약 (vim9compile && cctx->ctx_skip != SKIP_YES && !eval_failed)
	generate_NEWLIST(cctx, count, FALSE);

    만약 (eval_failed)
    {
	// expression evaluation in the heredoc failed
	list_free(l);
	반환 NULL;
    }
    반환 l;
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
    정수	    has_var;

    만약 (!in_vim9script())
    {
	semsg(_(e_str_cannot_be_used_in_legacy_vim_script), ":var");
	반환;
    }
    has_var = checkforcmd_noparen(&p, "var", 3);
    만약 (current_sctx.sc_sid == 0 && has_var)
    {
	emsg(_(e_cannot_declare_variable_on_command_line));
	반환;
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
    정수		var_count = 0;
    정수		semicolon = 0;
    char_u	op[4];
    char_u	*argend;
    정수		first = TRUE;
    정수		concat;
    정수		has_assign;
    정수		flags = 0;
    정수		vim9script = in_vim9script();

    만약 (eap->cmdidx == CMD_final && !vim9script)
    {
	// In legacy Vim script ":final" is short for ":finally".
	ex_finally(eap);
	반환;
    }
    만약 (eap->cmdidx == CMD_let && vim9script)
    {
	emsg(_(e_cannot_use_let_in_vim9_script));
	반환;
    }

    만약 (eap->cmdidx == CMD_const)
	flags |= ASSIGN_CONST;
    아니면 만약 (eap->cmdidx == CMD_final)
	flags |= ASSIGN_FINAL;

    // Vim9 assignment without ":let", ":const" or ":final"
    만약 (eap->arg == eap->cmd)
	flags |= ASSIGN_NO_DECL;

    argend = skip_var_list(arg, TRUE, &var_count, &semicolon, FALSE);
    만약 (argend == NULL)
	반환;
    expr = skipwhite(argend);
    concat = expr[0] == '.'
	&& ((expr[1] == '=' && in_old_script(2))
		|| (expr[1] == '.' && expr[2] == '='));
    has_assign =  *expr == '=' || (vim_strchr((char_u *)"+-*/%", *expr) != NULL
							    && expr[1] == '=');
    만약 (!has_assign && !concat)
    {
	// ":let" without "=": list variables
	만약 (*arg == '[')
	    emsg(_(e_invalid_argument));
	아니면 만약 (expr[0] == '.' && expr[1] == '=')
	    emsg(_(e_dot_equal_not_supported_with_script_version_two));
	아니면 만약 (!ends_excmd2(eap->cmd, arg))
	{
	    만약 (vim9script)
	    {
		만약 (!ends_excmd2(eap->cmd, skipwhite(argend)))
		    semsg(_(e_trailing_characters_str), argend);
		아니면
		    // Vim9 declaration ":var name: type"
		    arg = vim9_declare_scriptvar(eap, arg);
	    }
	    아니면
	    {
		// ":let var1 var2" - list values
		arg = list_arg_vars(eap, arg, &first);
	    }
	}
	아니면 만약 (!eap->skip)
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
	반환;
    }

    만약 (expr[0] == '=' && expr[1] == '<' && expr[2] == '<')
    {
	list_T	*l = NULL;
	long	cur_lnum = SOURCING_LNUM;

	// :let text =<< [trim] [eval] END
	// :var text =<< [trim] [eval] END
	만약 (vim9script && !eap->skip && (!VIM_ISWHITE(expr[-1])
						 || !IS_WHITE_OR_NUL(expr[3])))
	    semsg(_(e_white_space_required_before_and_after_str_at_str),
								  "=<<", expr);
	아니면
	    l = heredoc_get(eap, expr + 3, FALSE, FALSE);

	만약 (l != NULL)
	{
	    rettv_list_set(&rettv, l);
	    만약 (!eap->skip)
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
	반환;
    }

    evalarg_T   evalarg;
    정수	    len = 1;

    CLEAR_FIELD(rettv);

    정수 cur_lnum;

    op[0] = '=';
    op[1] = NUL;
    만약 (*expr != '=')
    {
	만약 (vim9script && (flags & ASSIGN_NO_DECL) == 0)
	{
	    // +=, /=, etc. require an existing variable
	    semsg(_(e_cannot_use_operator_on_new_variable_str), eap->arg);
	}
	아니면 만약 (vim_strchr((char_u *)"+-*/%.", *expr) != NULL)
	{
	    op[0] = *expr;   // +=, -=, *=, /=, %= or .=
	    ++len;
	    만약 (expr[0] == '.' && expr[1] == '.') // ..=
	    {
		++expr;
		++len;
	    }
	}
	expr += 2;
    }
    아니면
	++expr;

    만약 (vim9script && !eap->skip && (!VIM_ISWHITE(*argend)
					       || !IS_WHITE_OR_NUL(*expr)))
    {
	vim_strncpy(op, expr - len, len);
	semsg(_(e_white_space_required_before_and_after_str_at_str),
							       op, argend);
    }

    만약 (eap->skip)
	++emsg_skip;
    fill_evalarg_from_eap(&evalarg, eap, eap->skip);
    expr = skipwhite_and_linebreak(expr, &evalarg);
    cur_lnum = SOURCING_LNUM;
    정수 eval_res = eval0(expr, &rettv, eap, &evalarg);
    만약 (eap->skip)
	--emsg_skip;
    clear_evalarg(&evalarg, eap);

    // Restore the line number so that any type error is given for the
    // declaration, not the expression.
    SOURCING_LNUM = cur_lnum;

    만약 (!eap->skip && eval_res != FAIL)
	(void)ex_let_vars(eap->arg, &rettv, FALSE, semicolon, var_count,
								    flags, op);
    만약 (eval_res != FAIL)
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
    정수
ex_let_vars(
    char_u	*arg_start,
    typval_T	*tv,
    정수		copy,		// copy values from "tv", don't move
    정수		semicolon,	// from skip_var_list()
    정수		var_count,	// from skip_var_list()
    정수		flags,		// ASSIGN_FINAL, ASSIGN_CONST, etc.
    char_u	*op)
{
    char_u	*arg = arg_start;
    list_T	*l;
    tuple_T	*tuple = NULL;
    정수		i;
    정수		var_idx = 0;
    listitem_T	*item = NULL;
    typval_T	ltv;
    정수		is_list = tv->v_type == VAR_LIST;
    정수		idx;

    만약 (tv->v_type == VAR_VOID)
    {
	emsg(_(e_cannot_use_void_value));
	반환 FAIL;
    }
    만약 (*arg != '[')
    {
	// ":let var = expr" or ":for var in list"
	만약 (ex_let_one(arg, tv, copy, flags, op, op, var_idx) == NULL)
	    반환 FAIL;
	반환 OK;
    }

    // ":let [v1, v2] = list" or ":for [v1, v2] in listlist"
    // or
    // ":let [v1, v2] = tuple" or ":for [v1, v2] in tupletuple"
    만약 (tv->v_type != VAR_LIST && tv->v_type != VAR_TUPLE)
    {
	emsg(_(e_list_or_tuple_required));
	반환 FAIL;
    }
    만약 (is_list)
    {
	l = tv->vval.v_list;
	만약 (l == NULL)
	{
	    emsg(_(e_list_required));
	    반환 FAIL;
	}
	i = list_len(l);
    }
    아니면
    {
	tuple = tv->vval.v_tuple;
	만약 (tuple == NULL)
	{
	    emsg(_(e_tuple_required));
	    반환 FAIL;
	}
	i = tuple_len(tuple);
    }

    만약 (semicolon == 0 && var_count < i)
    {
	emsg(_(is_list ? e_less_targets_than_list_items
					: e_less_targets_than_tuple_items));
	반환 FAIL;
    }
    만약 (var_count - semicolon > i)
    {
	emsg(_(is_list ? e_more_targets_than_list_items
					: e_more_targets_than_tuple_items));
	반환 FAIL;
    }

    만약 (is_list)
    {
	CHECK_LIST_MATERIALIZE(l);
	item = l->lv_first;
    }
    아니면
	idx = 0;

    동안 (*arg != ']')
    {
	arg = skipwhite(arg + 1);
	++var_idx;
	arg = ex_let_one(arg, is_list ? &item->li_tv : TUPLE_ITEM(tuple, idx),
			 TRUE, flags | ASSIGN_UNPACK, (char_u *)",;]", op,
			 var_idx);
	만약 (is_list)
	    item = item->li_next;
	아니면
	    idx++;
	만약 (arg == NULL)
	    반환 FAIL;

	arg = skipwhite(arg);
	만약 (*arg == ';')
	{
	    // Put the rest of the list or tuple (may be empty) in the var
	    // after ';'.  Create a new list or tuple for this.
	    만약 (is_list)
	    {
		// Put the rest of the list (may be empty) in the var
		// after ';'.  Create a new list for this.
		l = list_alloc();
		만약 (l == NULL)
		    반환 FAIL;

		// list
		동안 (item != NULL)
		{
		    list_append_tv(l, &item->li_tv);
		    item = item->li_next;
		}

		ltv.v_type = VAR_LIST;
		ltv.v_lock = 0;
		ltv.vval.v_list = l;
		l->lv_refcount = 1;
	    }
	    아니면
	    {
		tuple_T	*new_tuple = tuple_alloc();
		만약 (new_tuple == NULL)
		    반환 FAIL;

		// Put the rest of the tuple (may be empty) in the var
		// after ';'.  Create a new tuple for this.
		동안 (idx < TUPLE_LEN(tuple))
		{
		    typval_T    new_tv;

		    copy_tv(TUPLE_ITEM(tuple, idx), &new_tv);
		    만약 (tuple_append_tv(new_tuple, &new_tv) == FAIL)
		    {
			clear_tv(&new_tv);
			tuple_unref(new_tuple);
			반환 FAIL;
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
	    만약 (arg == NULL)
		반환 FAIL;
	    break;
	}
	아니면 만약 (*arg != ',' && *arg != ']')
	{
	    internal_error("ex_let_vars()");
	    반환 FAIL;
	}
    }

    반환 OK;
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
    정수		include_type,
    정수		*var_count,
    정수		*semicolon,
    정수		silent)
{
    char_u	*p, *s;

    만약 (*arg == '[')
    {
	// "[var, var]": find the matching ']'.
	p = arg;
	위해 (;;)
	{
	    p = skipwhite(p + 1);	// skip whites after '[', ';' or ','
	    s = skip_var_one(p, include_type);
	    만약 (s == p)
	    {
		만약 (!silent)
		    semsg(_(e_invalid_argument_str), p);
		반환 NULL;
	    }
	    ++*var_count;

	    p = skipwhite(s);
	    만약 (*p == ']')
		break;
	    아니면 만약 (*p == ';')
	    {
		만약 (*semicolon == 1)
		{
		    만약 (!silent)
			emsg(_(e_double_semicolon_in_list_of_variables));
		    반환 NULL;
		}
		*semicolon = 1;
	    }
	    아니면 만약 (*p != ',')
	    {
		만약 (!silent)
		    semsg(_(e_invalid_argument_str), p);
		반환 NULL;
	    }
	}
	반환 p + 1;
    }

    반환 skip_var_one(arg, include_type);
}

/*
 * Skip one (assignable) variable name, including @r, $VAR, &option, d.key,
 * l[idx].
 * In Vim9 script also skip over ": type" if "include_type" is TRUE.
 */
    char_u *
skip_var_one(char_u *arg, 정수 include_type)
{
    char_u	*end;
    정수		vim9 = in_vim9script();

    만약 (*arg == '@' && arg[1] != NUL)
	반환 arg + 2;

    // termcap option name may have non-alpha characters
    만약 (STRNCMP(arg, "&t_", 3) == 0 && arg[3] != NUL && arg[4] != NUL)
	반환 arg + 5;

    end = find_name_end(*arg == '$' || *arg == '&' ? arg + 1 : arg,
				   NULL, NULL, FNE_INCL_BR | FNE_CHECK_START);

    // "a: type" is declaring variable "a" with a type, not "a:".
    // Same for "s: type".
    만약 (vim9 && end == arg + 2 && end[-1] == ':')
	--end;

    만약 (include_type && vim9)
    {
	만약 (*skipwhite(end) == ':')
	    end = skip_type(skipwhite(skipwhite(end) + 1), FALSE);
    }
    반환 end;
}

/*
 * List variables for hashtab "ht" with prefix "prefix".
 * If "empty" is TRUE also list NULL strings as empty strings.
 */
    void
list_hashtable_vars(
    hashtab_T	*ht,
    char	*prefix,
    정수		empty,
    정수		*first)
{
    hashitem_T	*hi;
    dictitem_T	*di;
    정수		todo;
    char_u	buf[IOSIZE];

    정수 save_ht_flags = ht->ht_flags;
    ht->ht_flags |= HTFLAGS_FROZEN;

    todo = (정수)ht->ht_used;
    위해 (hi = ht->ht_array; todo > 0 && !got_int; ++hi)
    {
	만약 (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    di = HI2DI(hi);

	    // apply :filter /pat/ to variable name
	    vim_strncpy((char_u *)buf, (char_u *)prefix, IOSIZE - 1);
	    vim_strcat((char_u *)buf, di->di_key, IOSIZE);
	    만약 (message_filtered(buf))
		continue;

	    만약 (empty || di->di_tv.v_type != VAR_STRING
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
list_glob_vars(정수 *first)
{
    list_hashtable_vars(&globvarht, "", TRUE, first);
}

/*
 * List buffer variables.
 */
    static void
list_buf_vars(정수 *first)
{
    list_hashtable_vars(&curbuf->b_vars->dv_hashtab, "b:", TRUE, first);
}

/*
 * List window variables.
 */
    static void
list_win_vars(정수 *first)
{
    list_hashtable_vars(&curwin->w_vars->dv_hashtab, "w:", TRUE, first);
}

/*
 * List tab page variables.
 */
    static void
list_tab_vars(정수 *first)
{
    list_hashtable_vars(&curtab->tp_vars->dv_hashtab, "t:", TRUE, first);
}

/*
 * List variables in "arg".
 */
    static char_u *
list_arg_vars(exarg_T *eap, char_u *arg, 정수 *first)
{
    정수		error = FALSE;
    정수		len;
    char_u	*name;
    char_u	*name_start;
    char_u	*arg_subsc;
    char_u	*tofree;
    typval_T    tv;

    동안 (!ends_excmd2(eap->cmd, arg) && !got_int)
    {
	만약 (error || eap->skip)
	{
	    arg = find_name_end(arg, NULL, NULL, FNE_INCL_BR | FNE_CHECK_START);
	    만약 (!VIM_ISWHITE(*arg) && !ends_excmd(*arg))
	    {
		emsg_severe = TRUE;
		만약 (!did_emsg)
		    semsg(_(e_trailing_characters_str), arg);
		break;
	    }
	}
	아니면
	{
	    // get_name_len() takes care of expanding curly braces
	    name_start = name = arg;
	    len = get_name_len(&arg, &tofree, TRUE, TRUE);
	    만약 (len <= 0)
	    {
		// This is mainly to keep test 49 working: when expanding
		// curly braces fails overrule the exception error message.
		만약 (len < 0 && !aborting())
		{
		    emsg_severe = TRUE;
		    semsg(_(e_invalid_argument_str), arg);
		    break;
		}
		error = TRUE;
	    }
	    아니면
	    {
		arg = skipwhite(arg);
		만약 (tofree != NULL)
		    name = tofree;
		만약 (eval_variable(name, len, 0, &tv, NULL,
						     EVAL_VAR_VERBOSE) == FAIL)
		    error = TRUE;
		아니면
		{
		    // handle d.key, l[idx], f(expr)
		    arg_subsc = arg;
		    만약 (handle_subscript(&arg, name_start, &tv,
					      &EVALARG_EVALUATE, TRUE) == FAIL)
			error = TRUE;
		    아니면
		    {
			만약 (arg == arg_subsc && len == 2 && name[1] == ':')
			{
			    switch (*name)
			    {
				case 'g': list_glob_vars(first); break;
				case 'b': list_buf_vars(first); break;
				case 'w': list_win_vars(first); break;
				case 't': list_tab_vars(first); break;
				case 'v': list_vim_vars(first); break;
				case 's': list_script_vars(first); break;
				case 'l': list_func_vars(first); break;
				default:
					  semsg(_(e_cant_list_variables_for_str), name);
			    }
			}
			아니면
			{
			    char_u	numbuf[NUMBUFLEN];
			    char_u	*tf;
			    정수		c;
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

    반환 arg;
}

/*
 * Set an environment variable, part of ex_let_one().
 */
    static char_u *
ex_let_env(
    char_u	*arg,
    typval_T	*tv,
    정수		flags,
    char_u	*endchars,
    char_u	*op)
{
    char_u	*arg_end = NULL;
    char_u	*name;
    정수		len;

    만약 ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					 && (flags & ASSIGN_FOR_LOOP) == 0)
    {
	emsg(_(e_cannot_lock_environment_variable));
	반환 NULL;
    }

    // Find the end of the name.
    ++arg;
    name = arg;
    len = get_env_len(&arg);
    만약 (len == 0)
	semsg(_(e_invalid_argument_str), name - 1);
    아니면
    {
	만약 (op != NULL && vim_strchr((char_u *)"+-*/%", *op) != NULL)
	    semsg(_(e_wrong_variable_type_for_str_equal), op);
	아니면 만약 (endchars != NULL
			      && vim_strchr(endchars, *skipwhite(arg)) == NULL)
	    emsg(_(e_unexpected_characters_in_let));
	아니면 만약 (!check_secure() && !check_restricted())
	{
	    char_u	*tofree = NULL;
	    정수		c1 = name[len];
	    char_u	*p;

	    name[len] = NUL;
	    p = tv_get_string_chk(tv);
	    만약 (p != NULL && op != NULL && *op == '.')
	    {
		정수	mustfree = FALSE;
		char_u  *s = vim_getenv(name, &mustfree);

		만약 (s != NULL)
		{
		    p = tofree = concat_str(s, p);
		    만약 (mustfree)
			vim_free(s);
		}
	    }
	    만약 (p != NULL)
	    {
		vim_setenv_ext(name, p);
		arg_end = arg;
	    }
	    name[len] = c1;
	    vim_free(tofree);
	}
    }
    반환 arg_end;
}

/*
 * Set an option, part of ex_let_one().
 */
    static char_u *
ex_let_option(
    char_u	*arg,
    typval_T	*tv,
    정수		flags,
    char_u	*endchars,
    char_u	*op)
{
    char_u	*p;
    정수		scope;
    char_u	*arg_end = NULL;

    만약 ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					 && (flags & ASSIGN_FOR_LOOP) == 0)
    {
	emsg(_(e_cannot_lock_option));
	반환 NULL;
    }

    // Find the end of the name.
    p = find_option_end(&arg, &scope);
    만약 (p == NULL || (endchars != NULL
			       && vim_strchr(endchars, *skipwhite(p)) == NULL))
    {
	emsg(_(e_unexpected_characters_in_let));
	반환 NULL;
    }

    정수		c1;
    long	n = 0;
    getoption_T	opt_type;
    long	numval;
    char_u	*stringval = NULL;
    char_u	*s = NULL;
    정수		failed = FALSE;
    정수		opt_p_flags;
    char_u	*tofree = NULL;
    char_u	numbuf[NUMBUFLEN];

    c1 = *p;
    *p = NUL;

    opt_type = get_option_value(arg, &numval, &stringval, &opt_p_flags, scope);
    만약 (opt_type == gov_unknown && arg[0] != 't' && arg[1] != '_')
    {
	semsg(_(e_unknown_option_str_2), arg);
	goto theend;
    }
    만약 (op != NULL && *op != '='
	&& (((opt_type == gov_bool || opt_type == gov_number) && *op == '.')
	    || (opt_type == gov_string && *op != '.')))
    {
	semsg(_(e_wrong_variable_type_for_str_equal), op);
	goto theend;
    }

    만약 ((opt_type == gov_bool
		|| opt_type == gov_number
		|| opt_type == gov_hidden_bool
		|| opt_type == gov_hidden_number)
		     && (tv->v_type != VAR_STRING || !in_vim9script()))
    {
	만약 (opt_type == gov_bool || opt_type == gov_hidden_bool)
	    // bool, possibly hidden
	    n = (long)tv_get_bool_chk(tv, &failed);
	아니면
	    // number, possibly hidden
	    n = (long)tv_get_number_chk(tv, &failed);
	만약 (failed)
	    goto theend;
    }

    만약 ((opt_p_flags & P_FUNC) && (tv->v_type == VAR_PARTIAL
						    || tv->v_type == VAR_FUNC))
    {
	// If the option can be set to a function reference or a lambda
	// and the passed value is a function reference, then convert it to
	// the name (string) of the function reference.
	s = tv2string(tv, &tofree, numbuf, 0);
	만약 (s == NULL)
	    goto theend;
    }
    // Avoid setting a string option to the text "v:false" or similar.
    // In Vim9 script also don't convert a number to string.
    아니면 만약 (tv->v_type != VAR_BOOL && tv->v_type != VAR_SPECIAL
		     && (!in_vim9script() || tv->v_type != VAR_NUMBER))
    {
	s = tv_get_string_chk(tv);
	만약 (s == NULL)
	    goto theend;
    }
    아니면 만약 (opt_type == gov_string || opt_type == gov_hidden_string)
    {
	emsg(_(e_string_required));
	goto theend;
    }

    만약 (op != NULL && *op != '=')
    {
	// number, in legacy script also bool
	만약 (opt_type == gov_number
				 || (opt_type == gov_bool && !in_vim9script()))
	{
	    switch (*op)
	    {
		case '+': n = numval + n; break;
		case '-': n = numval - n; break;
		case '*': n = numval * n; break;
		case '/': n = (long)num_divide(numval, n, &failed); break;
		case '%': n = (long)num_modulus(numval, n, &failed); break;
	    }
	    s = NULL;
	    만약 (failed)
		goto theend;
	}
	아니면 만약 (opt_type == gov_string && stringval != NULL && s != NULL)
	{
	    // string
	    s = concat_str(stringval, s);
	    vim_free(stringval);
	    stringval = s;
	}
    }

    char *err = set_option_value(arg, n, s, scope);
    arg_end = p;
    만약 (err != NULL)
	emsg(_(err));

theend:
    *p = c1;
    vim_free(stringval);
    vim_free(tofree);
    반환 arg_end;
}

/*
 * Set a register, part of ex_let_one().
 */
    static char_u *
ex_let_register(
    char_u	*arg,
    typval_T	*tv,
    정수		flags,
    char_u	*endchars,
    char_u	*op)
{
    char_u	*arg_end = NULL;

    만약 ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					 && (flags & ASSIGN_FOR_LOOP) == 0)
    {
	emsg(_(e_cannot_lock_register));
	반환 NULL;
    }
    ++arg;
    만약 (op != NULL && vim_strchr((char_u *)"+-*/%", *op) != NULL)
	semsg(_(e_wrong_variable_type_for_str_equal), op);
    아니면 만약 (endchars != NULL
			  && vim_strchr(endchars, *skipwhite(arg + 1)) == NULL)
	emsg(_(e_unexpected_characters_in_let));
    아니면
    {
	char_u	*ptofree = NULL;
	char_u	*p;

	p = tv_get_string_chk(tv);
	만약 (p != NULL && op != NULL && *op == '.')
	{
	    char_u  *s = get_reg_contents(*arg == '@'
						  ? '"' : *arg, GREG_EXPR_SRC);

	    만약 (s != NULL)
	    {
		p = ptofree = concat_str(s, p);
		vim_free(s);
	    }
	}
	만약 (p != NULL)
	{
	    write_reg_contents(*arg == '@' ? '"' : *arg, p, -1, FALSE);
	    arg_end = arg + 1;
	}
	vim_free(ptofree);
    }
    반환 arg_end;
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
    정수		copy,		// copy value from "tv"
    정수		flags,		// ASSIGN_CONST, ASSIGN_FINAL, etc.
    char_u	*endchars,	// valid chars after variable name  or NULL
    char_u	*op,		// "+", "-", "."  or NULL
    정수		var_idx)	// variable index for "let [a, b] = list"
{
    char_u	*arg_end = NULL;

    만약 (in_vim9script() && (flags & (ASSIGN_NO_DECL | ASSIGN_DECL)) == 0
			&& (flags & (ASSIGN_CONST | ASSIGN_FINAL)) == 0
				  && vim_strchr((char_u *)"$@&", *arg) != NULL)
    {
	vim9_declare_error(arg);
	반환 NULL;
    }

    만약 (check_typval_is_value(tv) == FAIL)
	반환 NULL;

    // In Vim9 script an environment variable and a register only take a
    // String.  "@#" also takes a buffer number.
    만약 (in_vim9script() && (*arg == '$' || *arg == '@')
	    && !(*arg == '@' && arg[1] == '#' && tv->v_type == VAR_NUMBER
					       && (op == NULL || *op == '=')))
    {
	where_T	where = WHERE_INIT;

	만약 (check_typval_type(&t_string, tv, where) == FAIL)
	    반환 NULL;
    }

    만약 (*arg == '$')
    {
	// ":let $VAR = expr": Set environment variable.
	반환 ex_let_env(arg, tv, flags, endchars, op);
    }
    아니면 만약 (*arg == '&')
    {
	// ":let &option = expr": Set option value.
	// ":let &l:option = expr": Set local option value.
	// ":let &g:option = expr": Set global option value.
	// ":for &ts in range(8)": Set option value for for loop
	반환 ex_let_option(arg, tv, flags, endchars, op);
    }
    아니면 만약 (*arg == '@')
    {
	// ":let @r = expr": Set register contents.
	반환 ex_let_register(arg, tv, flags, endchars, op);
    }
    아니면 만약 (eval_isnamec1(*arg) || *arg == '{')
    {
	lval_T	lv;
	char_u	*p;
	정수	lval_flags = (flags & (ASSIGN_NO_DECL | ASSIGN_DECL))
							     ? GLV_NO_DECL : 0;
	lval_flags |= (flags & ASSIGN_FOR_LOOP) ? GLV_FOR_LOOP : 0;
	만약 (op != NULL && *op != '=')
	    lval_flags |= GLV_ASSIGN_WITH_OP;

	// ":let var = expr": Set internal variable.
	// ":let var: type = expr": Set internal variable with type.
	// ":let {expr} = expr": Idem, name made with curly braces
	p = get_lval(arg, tv, &lv, FALSE, FALSE, lval_flags, FNE_CHECK_START);
	만약 (p != NULL && lv.ll_name != NULL)
	{
	    만약 (endchars != NULL && vim_strchr(endchars,
					   *skipwhite(lv.ll_name_end)) == NULL)
	    {
		emsg(_(e_unexpected_characters_in_let));
	    }
	    아니면
	    {
		set_var_lval(&lv, p, tv, copy, flags, op, var_idx);
		arg_end = lv.ll_name_end;
	    }
	}
	clear_lval(&lv);
    }
    아니면
	semsg(_(e_invalid_argument_str), arg);

    반환 arg_end;
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
    정수		deep = 2;

    만약 (eap->forceit)
	deep = -1;
    아니면 만약 (vim_isdigit(*arg))
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
    정수		deep,
    정수		glv_flags,
    정수		(*callback)(lval_T *, char_u *, exarg_T *, 정수, void *),
    void	*cookie)
{
    char_u	*arg = argstart;
    char_u	*name_end;
    정수		error = FALSE;
    lval_T	lv;

    do
    {
	만약 (*arg == '$')
	{
	    lv.ll_name = arg;
	    lv.ll_tv = NULL;
	    ++arg;
	    만약 (get_env_len(&arg) == 0)
	    {
		semsg(_(e_invalid_argument_str), arg - 1);
		반환;
	    }
	    만약 (!error && !eap->skip
			      && callback(&lv, arg, eap, deep, cookie) == FAIL)
		error = TRUE;
	    name_end = arg;
	}
	아니면
	{
	    // Parse the name and find the end.
	    name_end = get_lval(arg, NULL, &lv, TRUE, eap->skip || error,
				     glv_flags | GLV_NO_DECL, FNE_CHECK_START);
	    만약 (lv.ll_name == NULL)
		error = TRUE;	    // error but continue parsing
	    만약 (name_end == NULL || (!VIM_ISWHITE(*name_end)
						    && !ends_excmd(*name_end)))
	    {
		만약 (name_end != NULL)
		{
		    emsg_severe = TRUE;
		    semsg(_(e_trailing_characters_str), name_end);
		}
		만약 (!(eap->skip || error))
		    clear_lval(&lv);
		break;
	    }

	    만약 (!error && !eap->skip
			 && callback(&lv, name_end, eap, deep, cookie) == FAIL)
		error = TRUE;

	    만약 (!eap->skip)
		clear_lval(&lv);
	}

	arg = skipwhite(name_end);
    } 동안 (!ends_excmd2(name_end, arg));

    set_nextcmd(eap, arg);
}

    static 정수
do_unlet_var(
    lval_T	*lp,
    char_u	*name_end,
    exarg_T	*eap,
    정수		deep UNUSED,
    void	*cookie UNUSED)
{
    정수		forceit = eap->forceit;
    정수		ret = OK;
    정수		cc;

    만약 (lp->ll_tv == NULL)
    {
	cc = *name_end;
	*name_end = NUL;

	// Environment variable, normal name or expanded name.
	만약 (*lp->ll_name == '$')
	    vim_unsetenv_ext(lp->ll_name + 1);
	아니면 만약 (do_unlet(lp->ll_name, forceit) == FAIL)
	    ret = FAIL;
	*name_end = cc;
    }
    아니면 만약 ((lp->ll_list != NULL
		 && value_check_lock(lp->ll_list->lv_lock, lp->ll_name, FALSE))
	    || (lp->ll_dict != NULL
		&& value_check_lock(lp->ll_dict->dv_lock, lp->ll_name, FALSE)))
	반환 FAIL;
    아니면 만약 (lp->ll_range)
	list_unlet_range(lp->ll_list, lp->ll_li, lp->ll_n1,
						    !lp->ll_empty2, lp->ll_n2);
    아니면 만약 (lp->ll_list != NULL)
	// unlet a List item.
	listitem_remove(lp->ll_list, lp->ll_li);
    아니면 만약 (lp->ll_dict != NULL)
	// unlet a Dictionary item.
	dictitem_remove(lp->ll_dict, lp->ll_di, "unlet");
    아니면
    {
	semsg(_(e_cannot_unlet_imported_item_str), lp->ll_name);
	반환 FAIL;
    }

    반환 ret;
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
	정수	    has_n2,
	long	    n2)
{
    listitem_T  *li = li_first;
    정수		n1 = n1_arg;

    // Delete a range of List items.
    li = li_first;
    n1 = n1_arg;
    동안 (li != NULL && (!has_n2 || n2 >= n1))
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
    정수
do_unlet(char_u *name, 정수 forceit)
{
    hashtab_T	*ht;
    hashitem_T	*hi;
    char_u	*varname = NULL;  // init to shut up gcc
    dict_T	*d;
    dictitem_T	*di;

    // can't :unlet a script variable in Vim9 script
    만약 (in_vim9script() && check_vim9_unlet(name) == FAIL)
	반환 FAIL;

    ht = find_var_ht(name, &varname);

    // can't :unlet a script variable in Vim9 script from a function
    만약 (ht == get_script_local_ht()
	    && SCRIPT_ID_VALID(current_sctx.sc_sid)
	    && SCRIPT_ITEM(current_sctx.sc_sid)->sn_version
							 == SCRIPT_VERSION_VIM9
	    && check_vim9_unlet(name) == FAIL)
	반환 FAIL;

    만약 (ht != NULL && *varname != NUL)
    {
	d = get_current_funccal_dict(ht);
	만약 (d == NULL)
	{
	    만약 (ht == &globvarht)
		d = &globvardict;
	    아니면 만약 (ht == &compat_hashtab)
		d = &vimvardict;
	    아니면
	    {
		di = find_var_in_ht(ht, *name, (char_u *)"", FALSE);
		d = di == NULL ? NULL : di->di_tv.vval.v_dict;
	    }
	    만약 (d == NULL)
	    {
		internal_error("do_unlet()");
		반환 FAIL;
	    }
	}
	hi = hash_find(ht, varname);
	만약 (HASHITEM_EMPTY(hi))
	    hi = find_hi_in_scoped_ht(name, &ht);
	만약 (hi != NULL && !HASHITEM_EMPTY(hi))
	{
	    di = HI2DI(hi);
	    만약 (var_check_fixed(di->di_flags, name, FALSE)
		    || var_check_ro(di->di_flags, name, FALSE)
		    || value_check_lock(d->dv_lock, name, FALSE)
		    || check_hashtab_frozen(ht, "unlet"))
		반환 FAIL;

	    delete_var(ht, hi);
	    반환 OK;
	}
    }
    만약 (forceit)
	반환 OK;
    semsg(_(e_no_such_variable_str), name);
    반환 FAIL;
}

    static void
report_lockvar_member(char *msg, lval_T *lp)
{
    정수 did_alloc = FALSE;
    char_u *vname = (char_u *)"";
    char_u *class_name = lp->ll_class != NULL
				    ? lp->ll_class->class_name.string : (char_u *)"";
    만약 (lp->ll_name != NULL)
    {
	만약 (lp->ll_name_end == NULL)
	    vname = lp->ll_name;
	아니면
	{
	    vname = vim_strnsave(lp->ll_name, lp->ll_name_end - lp->ll_name);
	    만약 (vname == NULL)
		반환;
	    did_alloc = TRUE;
	}
    }
    semsg(_(msg), vname, class_name);
    만약 (did_alloc)
	vim_free(vname);
}

/*
 * Lock or unlock variable indicated by "lp".
 * "deep" is the levels to go (-1 for unlimited);
 * "lock" is TRUE for ":lockvar", FALSE for ":unlockvar".
 */
    static 정수
do_lock_var(
    lval_T	*lp,
    char_u	*name_end,
    exarg_T	*eap,
    정수		deep,
    void	*cookie UNUSED)
{
    정수		lock = eap->cmdidx == CMD_lockvar;
    정수		ret = OK;
    정수		cc;
    dictitem_T	*di;

#ifdef LOG_LOCKVAR
    ch_log(NULL, "LKVAR: do_lock_var(): name %s, is_root %d", lp->ll_name, lp->ll_is_root);
#endif

    만약 (lp->ll_tv == NULL)
    {
	cc = *name_end;
	*name_end = NUL;
	만약 (*lp->ll_name == '$')
	{
	    semsg(_(e_cannot_lock_or_unlock_variable_str), lp->ll_name);
	    ret = FAIL;
	}
	아니면
	{
	    // Normal name or expanded name.
	    di = find_var(lp->ll_name, NULL, TRUE);
	    만약 (di == NULL)
	    {
		만약 (in_vim9script())
		    semsg(_(e_cannot_find_variable_to_unlock_str),
								  lp->ll_name);
		ret = FAIL;
	    }
	    아니면
	    {
		만약 ((di->di_flags & DI_FLAGS_FIX)
			    && di->di_tv.v_type != VAR_DICT
			    && di->di_tv.v_type != VAR_LIST)
		{
		    // For historic reasons this error is not given for a list
		    // or dict.  E.g., the b: dict could be locked/unlocked.
		    semsg(_(e_cannot_lock_or_unlock_variable_str), lp->ll_name);
		    ret = FAIL;
		}
		아니면
		{
		    만약 (in_vim9script())
		    {
			svar_T  *sv = find_typval_in_script(&di->di_tv,
								     0, FALSE);

			만약 (sv != NULL && sv->sv_const != 0)
			{
			    semsg(_(e_cannot_change_readonly_variable_str),
								  lp->ll_name);
			    ret = FAIL;
			}
		    }

		    만약 (ret == OK)
		    {
			만약 (lock)
			    di->di_flags |= DI_FLAGS_LOCK;
			아니면
			    di->di_flags &= ~DI_FLAGS_LOCK;
			만약 (deep != 0)
			    item_lock(&di->di_tv, deep, lock, FALSE);
		    }
		}
	    }
	}
	*name_end = cc;
    }
    아니면 만약 (deep == 0 && lp->ll_object == NULL && lp->ll_class == NULL)
    {
	// nothing to do
    }
    아니면 만약 (lp->ll_is_root)
	// (un)lock the item.
	item_lock(lp->ll_tv, deep, lock, FALSE);
    아니면 만약 (lp->ll_range)
    {
	listitem_T    *li = lp->ll_li;

	// (un)lock a range of List items.
	동안 (li != NULL && (lp->ll_empty2 || lp->ll_n2 >= lp->ll_n1))
	{
	    item_lock(&li->li_tv, deep, lock, FALSE);
	    li = li->li_next;
	    ++lp->ll_n1;
	}
    }
    아니면 만약 (lp->ll_list != NULL)
	// (un)lock a List item.
	item_lock(&lp->ll_li->li_tv, deep, lock, FALSE);
    아니면 만약 (lp->ll_object != NULL)  // This check must be before ll_class.
    {
	// (un)lock an object variable.
	report_lockvar_member(e_cannot_lock_object_variable_str, lp);
	ret = FAIL;
    }
    아니면 만약 (lp->ll_class != NULL)
    {
	// (un)lock a class variable.
	report_lockvar_member(e_cannot_lock_class_variable_str, lp);
	ret = FAIL;
    }
    아니면
    {
	// (un)lock a Dictionary item.
	만약 (lp->ll_di == NULL)
	{
	    emsg(_(e_dictionary_required));
	    ret = FAIL;
	}
	아니면
	    item_lock(&lp->ll_di->di_tv, deep, lock, FALSE);
    }

    반환 ret;
}

/*
 * Lock or unlock an item.  "deep" is nr of levels to go.
 * When "check_refcount" is TRUE do not lock a list or dict with a reference
 * count larger than 1.
 */
    void
item_lock(typval_T *tv, 정수 deep, 정수 lock, 정수 check_refcount)
{
    static 정수	recurse = 0;
    list_T	*l;
    listitem_T	*li;
    dict_T	*d;
    blob_T	*b;
    hashitem_T	*hi;
    정수		todo;

#ifdef LOG_LOCKVAR
    ch_log(NULL, "LKVAR: item_lock(): type %s", vartype_name(tv->v_type));
#endif

    만약 (recurse >= DICT_MAXNEST)
    {
	emsg(_(e_variable_nested_too_deep_for_unlock));
	반환;
    }
    만약 (deep == 0)
	반환;
    ++recurse;

    // lock/unlock the item itself
    만약 (lock)
	tv->v_lock |= VAR_LOCKED;
    아니면
	tv->v_lock &= ~VAR_LOCKED;

    switch (tv->v_type)
    {
	case VAR_UNKNOWN:
	case VAR_ANY:
	case VAR_VOID:
	case VAR_NUMBER:
	case VAR_BOOL:
	case VAR_STRING:
	case VAR_FUNC:
	case VAR_PARTIAL:
	case VAR_FLOAT:
	case VAR_SPECIAL:
	case VAR_JOB:
	case VAR_CHANNEL:
	case VAR_INSTR:
	case VAR_CLASS:
	case VAR_OBJECT:
	case VAR_TYPEALIAS:
	    break;

	case VAR_BLOB:
	    만약 ((b = tv->vval.v_blob) != NULL
				    && !(check_refcount && b->bv_refcount > 1))
	    {
		만약 (lock)
		    b->bv_lock |= VAR_LOCKED;
		아니면
		    b->bv_lock &= ~VAR_LOCKED;
	    }
	    break;
	case VAR_LIST:
	    만약 ((l = tv->vval.v_list) != NULL
				    && !(check_refcount && l->lv_refcount > 1))
	    {
		만약 (lock)
		    l->lv_lock |= VAR_LOCKED;
		아니면
		    l->lv_lock &= ~VAR_LOCKED;
		만약 (deep < 0 || deep > 1)
		{
		    만약 (l->lv_first == &range_list_item)
			l->lv_lock |= VAR_ITEMS_LOCKED;
		    아니면
		    {
			// recursive: lock/unlock the items the List contains
			CHECK_LIST_MATERIALIZE(l);
			FOR_ALL_LIST_ITEMS(l, li) item_lock(&li->li_tv,
					       deep - 1, lock, check_refcount);
		    }
		}
	    }
	    break;
	case VAR_TUPLE:
	    tuple_lock(tv->vval.v_tuple, deep, lock, check_refcount);
	    break;
	case VAR_DICT:
	    만약 ((d = tv->vval.v_dict) != NULL
				    && !(check_refcount && d->dv_refcount > 1))
	    {
		만약 (lock)
		    d->dv_lock |= VAR_LOCKED;
		아니면
		    d->dv_lock &= ~VAR_LOCKED;
		만약 (deep < 0 || deep > 1)
		{
		    // recursive: lock/unlock the items the List contains
		    todo = (정수)d->dv_hashtab.ht_used;
		    FOR_ALL_HASHTAB_ITEMS(&d->dv_hashtab, hi, todo)
		    {
			만약 (!HASHITEM_EMPTY(hi))
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

#만약 defined(FEAT_MENU) && defined(FEAT_MULTI_LANG)
/*
 * Delete all "menutrans_" variables.
 */
    void
del_menutrans_vars(void)
{
    hashitem_T	*hi;
    정수		todo;

    hash_lock(&globvarht);
    todo = (정수)globvarht.ht_used;
    위해 (hi = globvarht.ht_array; todo > 0 && !got_int; ++hi)
    {
	만약 (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    만약 (STRNCMP(HI2DI(hi)->di_key, "menutrans_", 10) == 0)
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
static 정수	varnamebuflen = 0;

/*
 * Function to concatenate a prefix and a variable name.
 */
    char_u *
cat_prefix_varname(정수 prefix, char_u *name)
{
    정수		len;

    len = (정수)STRLEN(name) + 3;
    만약 (len > varnamebuflen)
    {
	vim_free(varnamebuf);
	len += 10;			// some additional space
	varnamebuf = alloc(len);
	만약 (varnamebuf == NULL)
	{
	    varnamebuflen = 0;
	    반환 NULL;
	}
	varnamebuflen = len;
    }
    *varnamebuf = prefix;
    varnamebuf[1] = ':';
    STRCPY(varnamebuf + 2, name);
    반환 varnamebuf;
}

/*
 * Function given to ExpandGeneric() to obtain the list of user defined
 * (global/buffer/window/built-in) variable names.
 */
    char_u *
get_user_var_name(expand_T *xp, 정수 idx)
{
    static long_u	gdone;
    static long_u	bdone;
    static long_u	wdone;
    static long_u	tdone;
    static 정수		vidx;
    static hashitem_T	*hi;
    hashtab_T		*ht;

    만약 (idx == 0)
    {
	gdone = bdone = wdone = vidx = 0;
	tdone = 0;
    }

    // Global variables
    만약 (gdone < globvarht.ht_used)
    {
	만약 (gdone++ == 0)
	    hi = globvarht.ht_array;
	아니면
	    ++hi;
	동안 (HASHITEM_EMPTY(hi))
	    ++hi;
	만약 (STRNCMP("g:", xp->xp_pattern, 2) == 0)
	    반환 cat_prefix_varname('g', hi->hi_key);
	반환 hi->hi_key;
    }

    // b: variables
    ht = &prevwin_curwin()->w_buffer->b_vars->dv_hashtab;
    만약 (bdone < ht->ht_used)
    {
	만약 (bdone++ == 0)
	    hi = ht->ht_array;
	아니면
	    ++hi;
	동안 (HASHITEM_EMPTY(hi))
	    ++hi;
	반환 cat_prefix_varname('b', hi->hi_key);
    }

    // w: variables
    ht = &prevwin_curwin()->w_vars->dv_hashtab;
    만약 (wdone < ht->ht_used)
    {
	만약 (wdone++ == 0)
	    hi = ht->ht_array;
	아니면
	    ++hi;
	동안 (HASHITEM_EMPTY(hi))
	    ++hi;
	반환 cat_prefix_varname('w', hi->hi_key);
    }

    // t: variables
    ht = &curtab->tp_vars->dv_hashtab;
    만약 (tdone < ht->ht_used)
    {
	만약 (tdone++ == 0)
	    hi = ht->ht_array;
	아니면
	    ++hi;
	동안 (HASHITEM_EMPTY(hi))
	    ++hi;
	반환 cat_prefix_varname('t', hi->hi_key);
    }

    // v: variables
    만약 (vidx < VV_LEN)
	반환 cat_prefix_varname('v', (char_u *)vimvars[vidx++].vv_name);

    VIM_CLEAR(varnamebuf);
    varnamebuflen = 0;
    반환 NULL;
}

    char *
get_var_special_name(정수 nr)
{
    switch (nr)
    {
	case VVAL_FALSE: 반환 in_vim9script() ? "false" : "v:false";
	case VVAL_TRUE:  반환 in_vim9script() ? "true" : "v:true";
	case VVAL_NULL:  반환 in_vim9script() ? "null" : "v:null";
	case VVAL_NONE:  반환 "v:none";
    }
    internal_error("get_var_special_name()");
    반환 "42";
}

/*
 * Returns the global variable dictionary
 */
    dict_T *
get_globvar_dict(void)
{
    반환 &globvardict;
}

/*
 * Returns the global variable hash table
 */
    hashtab_T *
get_globvar_ht(void)
{
    반환 &globvarht;
}

/*
 * Returns the v: variable dictionary
 */
    dict_T *
get_vimvar_dict(void)
{
    반환 &vimvardict;
}

/*
 * Returns the index of a v:variable.  Negative if not found.
 * Returns DI_ flags in "di_flags".
 */
    정수
find_vim_var(char_u *name, 정수 *di_flags)
{
    dictitem_T	    *di = find_var_in_ht(&vimvarht, 0, name, TRUE);
    struct vimvar   *vv;

    만약 (di == NULL)
	반환 -1;
    *di_flags = di->di_flags;
    vv = (struct vimvar *)((char *)di - offsetof(vimvar_T, vv_di));
    반환 (정수)(vv - vimvars);
}


/*
 * Set type of v: variable to "type".
 */
    void
set_vim_var_type(정수 idx, vartype_T type)
{
    vimvars[idx].vv_tv_type = type;
}

/*
 * Set number v: variable to "val".
 * Note that this does not set the type, use set_vim_var_type() for that.
 */
    void
set_vim_var_nr(정수 idx, varnumber_T val)
{
    vimvars[idx].vv_nr = val;
}

    char *
get_vim_var_name(정수 idx)
{
    반환 vimvars[idx].vv_name;
}

/*
 * Get typval_T v: variable value.
 */
    typval_T *
get_vim_var_tv(정수 idx)
{
    반환 &vimvars[idx].vv_tv;
}

    type_T *
get_vim_var_type(정수 idx, garray_T *type_list)
{
    만약 (vimvars[idx].vv_type != NULL)
	반환 vimvars[idx].vv_type;
    반환 typval2type_vimvar(&vimvars[idx].vv_tv, type_list);
}

/*
 * Add what getinfo() reports for the v: variable "name", without the "v:", to
 * "d".  Returns FAIL when there is no such variable.
 */
    정수
vim_var_info(char_u *name, dict_T *d)
{
    정수		di_flags;
    정수		idx = find_vim_var(name, &di_flags);
    garray_T	type_list;
    char	*tofree;

    만약 (idx < 0)
	반환 FAIL;
    vim_snprintf((char *)IObuff, IOSIZE, "v:%s", vimvars[idx].vv_name);
    dict_add_string(d, "name", IObuff);
    dict_add_bool(d, "available", TRUE);
    ga_init2(&type_list, sizeof(type_T *), 10);
    char *type = type_name(get_vim_var_type(idx, &type_list), &tofree);
    dict_add_string(d, "type", (char_u *)type);
    vim_free(tofree);
    clear_type_list(&type_list);
    dict_add_bool(d, "readonly", (vimvars[idx].vv_flags & VV_RO) != 0);
    dict_add_bool(d, "compat", (vimvars[idx].vv_flags & VV_COMPAT) != 0);
    반환 OK;
}

/*
 * Set v: variable to "tv".  Only accepts the same type.
 * Takes over the value of "tv".
 */
    정수
set_vim_var_tv(정수 idx, typval_T *tv)
{
    만약 (vimvars[idx].vv_tv_type != tv->v_type)
    {
	emsg(_(e_type_mismatch_for_v_variable));
	clear_tv(tv);
	반환 FAIL;
    }
    // VV_RO is also checked when compiling, but let's check here as well.
    만약 (vimvars[idx].vv_flags & VV_RO)
    {
	semsg(_(e_cannot_change_readonly_variable_str), vimvars[idx].vv_name);
	반환 FAIL;
    }
    만약 (sandbox && (vimvars[idx].vv_flags & VV_RO_SBX))
    {
	semsg(_(e_cannot_set_variable_in_sandbox_str), vimvars[idx].vv_name);
	반환 FAIL;
    }
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_di.di_tv = *tv;
    반환 OK;
}

/*
 * Get number v: variable value.
 */
    varnumber_T
get_vim_var_nr(정수 idx)
{
    반환 vimvars[idx].vv_nr;
}

/*
 * Get string v: variable value.  Uses a static buffer, can only be used once.
 * If the String variable has never been set, return an empty string.
 * Never returns NULL;
 */
    char_u *
get_vim_var_str(정수 idx)
{
    반환 tv_get_string(&vimvars[idx].vv_tv);
}

/*
 * Get List v: variable value.  Caller must take care of reference count when
 * needed.
 */
    list_T *
get_vim_var_list(정수 idx)
{
    반환 vimvars[idx].vv_list;
}

/*
 * Get Dict v: variable value.  Caller must take care of reference count when
 * needed.
 */
    dict_T *
get_vim_var_dict(정수 idx)
{
    반환 vimvars[idx].vv_dict;
}

/*
 * Set v:char to character "c".
 */
    void
set_vim_var_char(정수 c)
{
    char_u  buf[MB_MAXBYTES + 1];
    size_t  buflen;

    만약 (has_mbyte)
	buflen = (*mb_char2bytes)(c, buf);
    아니면
    {
	buf[0] = c;
	buflen = 1;
    }
    buf[buflen] = NUL;

    set_vim_var_string(VV_CHAR, buf, (정수)buflen);
}

/*
 * Set v:count to "count" and v:count1 to "count1".
 * When "set_prevcount" is TRUE first set v:prevcount from v:count.
 */
    void
set_vcount(
    long	count,
    long	count1,
    정수		set_prevcount)
{
    만약 (set_prevcount)
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
    정수		idx,
    char_u	*val,
    정수		len)	    // length of "val" to use or -1 (whole string)
{
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_tv_type = VAR_STRING;
    만약 (val == NULL)
	vimvars[idx].vv_str = NULL;
    아니면 만약 (len == -1)
	vimvars[idx].vv_str = vim_strsave(val);
    아니면
	vimvars[idx].vv_str = vim_strnsave(val, len);
}

    void
set_vim_var_string_direct(
    정수		idx,
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
set_vim_var_list(정수 idx, list_T *val)
{
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_tv_type = VAR_LIST;
    vimvars[idx].vv_list = val;
    만약 (val != NULL)
	++val->lv_refcount;
}

/*
 * Set Dictionary v: variable to "val".
 */
    void
set_vim_var_dict(정수 idx, dict_T *val)
{
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_tv_type = VAR_DICT;
    vimvars[idx].vv_dict = val;
    만약 (val == NULL)
	반환;

    ++val->dv_refcount;
    dict_set_items_ro(val);
}

/*
 * Set the v:argv list.
 */
    void
set_argv_var(char **argv, 정수 argc)
{
    list_T	*l = list_alloc();
    정수		i;

    만약 (l == NULL)
	getout(1);
    l->lv_lock = VAR_FIXED;
    위해 (i = 0; i < argc; ++i)
    {
	만약 (list_append_string(l, (char_u *)argv[i], -1) == FAIL)
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
    정수 regname = 0;

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
set_reg_var(정수 c)
{
    char_u  regname[2];

    만약 (c == 0 || c == ' ')
	regname[0] = '"';
    아니면
	regname[0] = c;
    regname[1] = NUL;
    // Avoid free/alloc when the value is already right.
    만약 (vimvars[VV_REG].vv_str == NULL || vimvars[VV_REG].vv_str[0] != c)
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
    만약 (oldval == NULL)
	반환 vimvars[VV_EXCEPTION].vv_str;

    vimvars[VV_EXCEPTION].vv_str = oldval;
    반환 NULL;
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
    만약 (oldval == NULL)
	반환 vimvars[VV_THROWPOINT].vv_str;

    vimvars[VV_THROWPOINT].vv_str = oldval;
    반환 NULL;
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
    만약 (eap == NULL)
    {
	vim_free(oldval);
	vimvars[VV_CMDARG].vv_str = oldarg;
	반환 NULL;
    }

    만약 (eap->force_bin == FORCE_BIN)
	len = 6;
    아니면 만약 (eap->force_bin == FORCE_NOBIN)
	len = 8;
    아니면
	len = 0;

    만약 (eap->read_edit)
	len += 7;

    만약 (eap->force_ff != 0)
	len += 10; // " ++ff=unix"
    만약 (eap->force_enc != 0)
	len += (unsigned)STRLEN(eap->cmd + eap->force_enc) + 7;
    만약 (eap->bad_char != 0)
	len += 7 + 4;  // " ++bad=" + "keep" or "drop"

    newval = alloc(len + 1);
    만약 (newval == NULL)
	반환 NULL;

    만약 (eap->force_bin == FORCE_BIN)
	sprintf((char *)newval, " ++bin");
    아니면 만약 (eap->force_bin == FORCE_NOBIN)
	sprintf((char *)newval, " ++nobin");
    아니면
	*newval = NUL;

    만약 (eap->read_edit)
	STRCAT(newval, " ++edit");

    만약 (eap->force_ff != 0)
	sprintf((char *)newval + STRLEN(newval), " ++ff=%s",
						eap->force_ff == 'u' ? "unix"
						: eap->force_ff == 'd' ? "dos"
						: "mac");
    만약 (eap->force_enc != 0)
	sprintf((char *)newval + STRLEN(newval), " ++enc=%s",
					       eap->cmd + eap->force_enc);
    만약 (eap->bad_char == BAD_KEEP)
	STRCPY(newval + STRLEN(newval), " ++bad=keep");
    아니면 만약 (eap->bad_char == BAD_DROP)
	STRCPY(newval + STRLEN(newval), " ++bad=drop");
    아니면 만약 (eap->bad_char != 0)
	sprintf((char *)newval + STRLEN(newval), " ++bad=%c", eap->bad_char);
    vimvars[VV_CMDARG].vv_str = newval;
    반환 oldval;
}

/*
 * Get the value of internal variable "name".
 * If "flags" has EVAL_VAR_IMPORT may return a VAR_ANY with v_number set to the
 * imported script ID.
 * Return OK or FAIL.  If OK is returned "rettv" must be cleared.
 */
    정수
eval_variable(
    char_u	*name,
    정수		len,		// length of "name" or zero
    scid_T	sid,		// script ID for imported item or zero
    typval_T	*rettv,		// NULL when only checking existence
    dictitem_T	**dip,		// non-NULL when typval's dict item is needed
    정수		flags)		// EVAL_VAR_ flags
{
    정수		ret = OK;
    typval_T	*tv = NULL;
    정수		found = FALSE;
    hashtab_T	*ht = NULL;
    정수		cc = 0;
    type_T	*type = NULL;

    만약 (len > 0)
    {
	// truncate the name, so that we can use strcmp()
	cc = name[len];
	name[len] = NUL;
    }

    // Check for local variable when debugging.
    만약 ((sid == 0) && (tv = lookup_debug_var(name)) == NULL)
    {
	// Check for user-defined variables.
	dictitem_T	*v = find_var(name, &ht, flags & EVAL_VAR_NOAUTOLOAD);

	만약 (v != NULL)
	{
	    tv = &v->di_tv;
	    만약 (dip != NULL)
		*dip = v;
	}
	아니면
	    ht = NULL;
    }

    만약 (tv == NULL && (in_vim9script() || STRNCMP(name, "s:", 2) == 0))
    {
	imported_T  *import = NULL;
	char_u	    *p = STRNCMP(name, "s:", 2) == 0 ? name + 2 : name;

	만약 (sid == 0)
	    import = find_imported(p, 0, TRUE);

	// imported variable from another script
	만약 (import != NULL || sid != 0)
	{
	    만약 ((flags & EVAL_VAR_IMPORT) == 0)
	    {
		만약 (SCRIPT_ID_VALID(sid))
		{
		    ht = &SCRIPT_VARS(sid);
		    만약 (ht != NULL)
		    {
			dictitem_T *v = find_var_in_ht(ht, 0, name,
						  flags & EVAL_VAR_NOAUTOLOAD);

			만약 (v == NULL)
			    v = find_var_autoload_prefix(name, sid, NULL, NULL);

			만약 (v != NULL)
			{
			    tv = &v->di_tv;
			    만약 (dip != NULL)
				*dip = v;
			}
			아니면
			    ht = NULL;
		    }
		}
		아니면
		{
		    만약 (flags & EVAL_VAR_VERBOSE)
			semsg(_(e_expected_dot_after_name_str), name);
		    ret = FAIL;
		}
	    }
	    아니면
	    {
		만약 (rettv != NULL)
		{
		    // special value that is used in handle_subscript()
		    rettv->v_type = VAR_ANY;
		    rettv->vval.v_number = sid != 0 ? sid : import->imp_sid;
		}
		found = TRUE;
	    }
	}
	아니면 만약 (in_vim9script() && (flags & EVAL_VAR_NO_FUNC) == 0)
	{
	    정수	    has_g_prefix = STRNCMP(name, "g:", 2) == 0;
	    ufunc_T *ufunc = find_func(name, FALSE);

	    만약 (ufunc != NULL && cc == '<')
	    {
		// handle generic function
		char_u	*argp = name + len;
		name[len] = cc;
		ufunc = eval_generic_func(ufunc, name, &argp);
		name[len] = NUL;
		만약 (ufunc == NULL)
		{
		    ret = FAIL;
		    goto done;
		}
	    }

	    // In Vim9 script we can get a function reference by using the
	    // function name.  For a global non-autoload function "g:" is
	    // required.
	    만약 (ufunc != NULL && (has_g_prefix
					    || !func_requires_g_prefix(ufunc)))
	    {
		found = TRUE;
		만약 (rettv != NULL)
		{
		    rettv->v_type = VAR_FUNC;
		    만약 (has_g_prefix)
		    {
			// Keep the "g:", otherwise script-local may be
			// assumed.
			만약 (cc != '<')
			    rettv->vval.v_string = vim_strsave(name);
			아니면
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
		    아니면
			rettv->vval.v_string = vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
		    만약 (rettv->vval.v_string != NULL)
			func_ref(ufunc->uf_name);
		}
	    }
	}
    }

    만약 (!found)
    {
	만약 (tv == NULL)
	{
	    만약 (rettv != NULL && (flags & EVAL_VAR_VERBOSE))
		semsg(_(e_undefined_variable_str), name);
	    ret = FAIL;
	}
	아니면 만약 (rettv != NULL)
	{
	    svar_T  *sv = NULL;
	    정수	    was_assigned = FALSE;

	    만약 (ht != NULL && ht == get_script_local_ht()
		    && tv != &SCRIPT_SV(current_sctx.sc_sid)->sv_var.di_tv)
	    {
		sv = find_typval_in_script(tv, 0, TRUE);
		만약 (sv != NULL)
		{
		    type = sv->sv_type;
		    was_assigned = sv->sv_flags & SVFLAG_ASSIGNED;
		}
	    }

	    만약 ((tv->v_type == VAR_TYPEALIAS || tv->v_type == VAR_CLASS)
		    && sid != 0)
	    {
		// type alias or class imported from another script.  Check
		// whether it is exported from the other script.
		sv = find_typval_in_script(tv, sid, TRUE);
		만약 (sv == NULL)
		{
		    ret = FAIL;
		    goto done;
		}
		만약 ((sv->sv_flags & SVFLAG_EXPORTED) == 0)
		{
		    semsg(_(e_item_not_exported_in_script_str), name);
		    ret = FAIL;
		    goto done;
		}
	    }

	    // If a list or tuple or dict variable wasn't initialized and has
	    // meaningful type, do it now.  Not for global variables, they are
	    // not declared.
	    만약 (ht != &globvarht)
	    {
		만약 (tv->v_type == VAR_DICT && tv->vval.v_dict == NULL
					    && ((type != NULL && !was_assigned)
							  || !in_vim9script()))
		{
		    tv->vval.v_dict = dict_alloc();
		    만약 (tv->vval.v_dict != NULL)
		    {
			++tv->vval.v_dict->dv_refcount;
			tv->vval.v_dict->dv_type = alloc_type(type);
			만약 (sv != NULL)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
		아니면 만약 (tv->v_type == VAR_LIST && tv->vval.v_list == NULL
					    && ((type != NULL && !was_assigned)
							  || !in_vim9script()))
		{
		    tv->vval.v_list = list_alloc();
		    만약 (tv->vval.v_list != NULL)
		    {
			++tv->vval.v_list->lv_refcount;
			tv->vval.v_list->lv_type = alloc_type(type);
			만약 (sv != NULL)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
		아니면 만약 (tv->v_type == VAR_TUPLE && tv->vval.v_tuple == NULL
					    && ((type != NULL && !was_assigned)
							  || !in_vim9script()))
		{
		    tv->vval.v_tuple = tuple_alloc();
		    만약 (tv->vval.v_tuple != NULL)
		    {
			++tv->vval.v_tuple->tv_refcount;
			tv->vval.v_tuple->tv_type = alloc_type(type);
			만약 (sv != NULL)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
		아니면 만약 (tv->v_type == VAR_BLOB && tv->vval.v_blob == NULL
					    && ((type != NULL && !was_assigned)
							  || !in_vim9script()))
		{
		    tv->vval.v_blob = blob_alloc();
		    만약 (tv->vval.v_blob != NULL)
		    {
			++tv->vval.v_blob->bv_refcount;
			만약 (sv != NULL)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
	    }
	    ret = copy_tv(tv, rettv);
	}
    }

done:
    만약 (len > 0)
	name[len] = cc;

    반환 ret;
}

/*
 * Get the value of internal variable "name", also handling "import.name".
 * Return OK or FAIL.  If OK is returned "rettv" must be cleared.
 */
    정수
eval_variable_import(
    char_u	*name,
    typval_T	*rettv)
{
    char_u  *s = name;
    동안 (ASCII_ISALNUM(*s) || *s == '_')
	++s;
    정수	    len = (정수)(s - name);

    만약 (eval_variable(name, len, 0, rettv, NULL, EVAL_VAR_IMPORT) == FAIL)
	반환 FAIL;
    만약 (rettv->v_type == VAR_ANY && *s == '.')
    {
	char_u *ns = s + 1;
	s = ns;
	동안 (ASCII_ISALNUM(*s) || *s == '_')
	    ++s;
	정수 sid = rettv->vval.v_number;
	반환 eval_variable(ns, (정수)(s - ns), sid, rettv, NULL, 0);
    }
    반환 OK;
}


/*
 * Check if variable "name[len]" is a local variable or an argument.
 * If so, "*eval_lavars_used" is set to TRUE.
 */
    void
check_vars(char_u *name, 정수 len)
{
    정수		cc;
    char_u	*varname;
    hashtab_T	*ht;

    만약 (eval_lavars_used == NULL)
	반환;

    // truncate the name, so that we can use strcmp()
    cc = name[len];
    name[len] = NUL;

    ht = find_var_ht(name, &varname);
    만약 (ht == get_funccal_local_ht() || ht == get_funccal_args_ht())
    {
	만약 (find_var(name, NULL, TRUE) != NULL)
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
find_var(char_u *name, hashtab_T **htp, 정수 no_autoload)
{
    char_u	*varname;
    hashtab_T	*ht;
    dictitem_T	*ret = NULL;

    ht = find_var_ht(name, &varname);
    만약 (htp != NULL)
	*htp = ht;
    만약 (ht == NULL)
	반환 NULL;
    ret = find_var_in_ht(ht, *name, varname, no_autoload);
    만약 (ret != NULL)
	반환 ret;

    // Search in parent scope for lambda
    ret = find_var_in_scoped_ht(name, no_autoload);
    만약 (ret != NULL)
	반환 ret;

    // in Vim9 script items without a scope can be script-local
    만약 (in_vim9script() && name[0] != NUL && name[1] != ':')
    {
	ht = get_script_local_ht();
	만약 (ht != NULL)
	{
	    ret = find_var_in_ht(ht, *name, varname, no_autoload);
	    만약 (ret != NULL)
	    {
		만약 (htp != NULL)
		    *htp = ht;
		반환 ret;
	    }
	}
    }

    // and finally try
    반환 find_var_autoload_prefix(name, 0, htp, NULL);
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
find_var_autoload_prefix(char_u *name, 정수 sid, hashtab_T **htp,
							    char_u **namep)
{
    hashtab_T	*ht;
    dictitem_T	*ret = NULL;
    // When using "vim9script autoload" script-local items are prefixed but can
    // be used with s:name.
    정수 check_sid = sid > 0 ? sid : current_sctx.sc_sid;
    만약 (SCRIPT_ID_VALID(check_sid)
		   && (in_vim9script() || (name[0] == 's' && name[1] == ':')))
    {
	scriptitem_T *si = SCRIPT_ITEM(check_sid);

	만약 (si->sn_autoload_prefix != NULL)
	{
	    char_u *base_name = (name[0] == 's' && name[1] == ':')
							     ? name + 2 : name;
	    char_u *auto_name = concat_str(si->sn_autoload_prefix, base_name);

	    만약 (auto_name != NULL)
	    {
		정수 free_auto_name = TRUE;
		ht = &globvarht;
		ret = find_var_in_ht(ht, 'g', auto_name, TRUE);
		만약 (ret != NULL)
		{
		    만약 (htp != NULL)
			*htp = ht;
		    만약 (namep != NULL)
		    {
			free_auto_name = FALSE;
			*namep = auto_name;
		    }
		}
		만약 (free_auto_name)
		    vim_free(auto_name);
	    }
	}
    }

    반환 ret;
}

/*
 * Like find_var() but if the name starts with <SNR>99_ then look in the
 * referenced script (used for a funcref).
 */
    dictitem_T *
find_var_also_in_script(char_u *name, hashtab_T **htp, 정수 no_autoload)
{
    만약 (STRNCMP(name, "<SNR>", 5) == 0 && SAFE_isdigit(name[5]))
    {
	char_u	    *p = name + 5;
	정수	    sid = getdigits(&p);

	만약 (SCRIPT_ID_VALID(sid) && *p == '_')
	{
	    hashtab_T	*ht = &SCRIPT_VARS(sid);

	    만약 (ht != NULL)
	    {
		dictitem_T *di = find_var_in_ht(ht, 0, p + 1, no_autoload);

		만약 (di != NULL)
		{
		    만약 (htp != NULL)
			*htp = ht;
		    반환 di;
		}
	    }
	}
    }

    반환 find_var(name, htp, no_autoload);
}

/*
 * Find variable "varname" in hashtab "ht" with name "htname".
 * When "varname" is empty returns curwin/curtab/etc vars dictionary.
 * Returns NULL if not found.
 */
    dictitem_T *
find_var_in_ht(
    hashtab_T	*ht,
    정수		htname,
    char_u	*varname,
    정수		no_autoload)
{
    hashitem_T	*hi;

    만약 (*varname == NUL)
    {
	// Must be something like "s:", otherwise "ht" would be NULL.
	switch (htname)
	{
	    case 's': 반환 &SCRIPT_SV(current_sctx.sc_sid)->sv_var;
	    case 'g': 반환 &globvars_var;
	    case 'v': 반환 &vimvars_var;
	    case 'b': 반환 &curbuf->b_bufvar;
	    case 'w': 반환 &curwin->w_winvar;
	    case 't': 반환 &curtab->tp_winvar;
	    case 'l': 반환 get_funccal_local_var();
	    case 'a': 반환 get_funccal_args_var();
	}
	반환 NULL;
    }

    hi = hash_find(ht, varname);
    만약 (HASHITEM_EMPTY(hi))
    {
	// For global variables we may try auto-loading the script.  If it
	// worked find the variable again.  Don't auto-load a script if it was
	// loaded already, otherwise it would be loaded every time when
	// checking if a function name is a Funcref variable.
	만약 (ht == &globvarht && !no_autoload)
	{
	    // Note: script_autoload() may make "hi" invalid. It must either
	    // be obtained again or not used.
	    만약 (!script_autoload(varname, FALSE) || aborting())
		반환 NULL;
	    hi = hash_find(ht, varname);
	}
	만약 (HASHITEM_EMPTY(hi))
	    반환 NULL;
    }
    반환 HI2DI(hi);
}

/*
 * Get the script-local hashtab.  NULL if not in a script context.
 */
    hashtab_T *
get_script_local_ht(void)
{
    scid_T sid = current_sctx.sc_sid;

    만약 (SCRIPT_ID_VALID(sid))
	반환 &SCRIPT_VARS(sid);
    반환 NULL;
}

/*
 * Look for "name[len]" in script-local variables and functions.
 * When "cmd" is TRUE it must look like a command, a function must be followed
 * by "(" or "->".
 * Return OK when found, FAIL when not found.
 */
    정수
lookup_scriptitem(
	char_u	*name,
	size_t	len,
	정수	cmd,
	cctx_T	*dummy UNUSED)
{
    hashtab_T	*ht = get_script_local_ht();
    char_u	buffer[30];
    char_u	*p;
    정수		res;
    hashitem_T	*hi;
    정수		is_global = FALSE;
    char_u	*fname = name;

    만약 (ht == NULL)
	반환 FAIL;
    만약 (len < sizeof(buffer) - 1)
    {
	// avoid an alloc/free for short names
	vim_strncpy(buffer, name, len);
	p = buffer;
    }
    아니면
    {
	p = vim_strnsave(name, len);
	만약 (p == NULL)
	    반환 FAIL;
    }

    hi = hash_find(ht, p);
    res = HASHITEM_EMPTY(hi) ? FAIL : OK;

    // if not script-local, then perhaps autoload-exported
    만약 (res == FAIL && find_var_autoload_prefix(p, 0, NULL, NULL) != NULL)
	res = OK;

    // if not script-local or autoload, then perhaps imported
    만약 (res == FAIL && find_imported(p, 0, FALSE) != NULL)
	res = OK;
    만약 (p != buffer)
	vim_free(p);

    // Find a function, so that a following "->" works.
    // When used as a command require "(" or "->" to follow, "Cmd" is a user
    // command while "Cmd()" is a function call.
    만약 (res != OK)
    {
	p = skipwhite(name + len);

	만약 (!cmd || name[len] == '(' || (p[0] == '-' && p[1] == '>'))
	{
	    // Do not check for an internal function, since it might also be a
	    // valid command, such as ":split" versus "split()".
	    // Skip "g:" before a function name.
	    만약 (name[0] == 'g' && name[1] == ':')
	    {
		is_global = TRUE;
		fname = name + 2;
	    }
	    만약 (find_func(fname, is_global) != NULL)
		res = OK;
	}
    }

    반환 res;
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

    만약 (name[0] == NUL)
	반환 NULL;
    만약 (name[1] != ':')
    {
	// The name must not start with a colon or #.
	만약 (name[0] == ':' || name[0] == AUTOLOAD_CHAR)
	    반환 NULL;
	*varname = name;

	// "version" is "v:version" in all scopes if scriptversion < 3.
	// Same for a few other variables marked with VV_COMPAT.
	만약 (in_old_script(3))
	{
	    hi = hash_find(&compat_hashtab, name);
	    만약 (!HASHITEM_EMPTY(hi))
		반환 &compat_hashtab;
	}

	ht = get_funccal_local_ht();
	만약 (ht != NULL)
	    반환 ht;				// local variable

	// In Vim9 script items at the script level are script-local, except
	// for autoload names.
	만약 (in_vim9script() && vim_strchr(name, AUTOLOAD_CHAR) == NULL)
	{
	    ht = get_script_local_ht();
	    만약 (ht != NULL)
		반환 ht;
	}

	반환 &globvarht;			// global variable
    }
    *varname = name + 2;
    만약 (*name == 'g')				// global variable
	반환 &globvarht;
    // There must be no ':' or '#' in the rest of the name, unless g: is used
    만약 (vim_strchr(name + 2, ':') != NULL
			       || vim_strchr(name + 2, AUTOLOAD_CHAR) != NULL)
	반환 NULL;
    만약 (*name == 'b')				// buffer variable
	반환 &curbuf->b_vars->dv_hashtab;
    만약 (*name == 'w')				// window variable
	반환 &curwin->w_vars->dv_hashtab;
    만약 (*name == 't')				// tab page variable
	반환 &curtab->tp_vars->dv_hashtab;
    만약 (*name == 'v')				// v: variable
	반환 &vimvarht;
    만약 (get_current_funccal() != NULL
	       && get_current_funccal()->fc_func->uf_def_status
							    == UF_NOT_COMPILED)
    {
	// a: and l: are only used in functions defined with ":function"
	만약 (*name == 'a')			// a: function argument
	    반환 get_funccal_args_ht();
	만약 (*name == 'l')			// l: local function variable
	    반환 get_funccal_local_ht();
    }
    만약 (*name == 's')				// script variable
    {
	ht = get_script_local_ht();
	만약 (ht != NULL)
	    반환 ht;
    }
    반환 NULL;
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
    만약 (v == NULL)
	반환 NULL;
    반환 tv_get_string(&v->di_tv);
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
    만약 (sv == NULL)
	반환;
    init_var_dict(&sv->sv_dict, &sv->sv_var, VAR_SCOPE);
    SCRIPT_ITEM(id)->sn_vars = sv;
}

/*
 * Initialize dictionary "dict" as a scope and set variable "dict_var" to
 * point to it.
 */
    void
init_var_dict(dict_T *dict, dictitem_T *dict_var, 정수 scope)
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
vars_clear_ext(hashtab_T *ht, 정수 free_val)
{
    정수		todo;
    hashitem_T	*hi;
    dictitem_T	*v;

    hash_lock(ht);
    todo = (정수)ht->ht_used;
    FOR_ALL_HASHTAB_ITEMS(ht, hi, todo)
    {
	만약 (!HASHITEM_EMPTY(hi))
	{
	    --todo;

	    // Free the variable.  Don't remove it from the hashtab,
	    // ht_array might change then.  hash_clear() takes care of it
	    // later.
	    v = HI2DI(hi);
	    만약 (free_val)
		clear_tv(&v->di_tv);
	    만약 (v->di_flags & DI_FLAGS_ALLOC)
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

    만약 (hash_remove(ht, hi, "delete variable") != OK)
	반환;

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
    만약 (prefix == NULL)
	반환;
    size_t  prefixlen = STRLEN(prefix);

    hash_lock(&globvarht);
    정수	todo = (정수)globvarht.ht_used;

    위해 (hashitem_T *hi = globvarht.ht_array; todo > 0; ++hi)
	만약 (!HASHITEM_EMPTY(hi))
	{
	    dictitem_T	*di = HI2DI(hi);

	    --todo;
	    // Keep a class or enum: existing objects still refer to it.
	    만약 (di->di_tv.v_type != VAR_CLASS
		    && STRNCMP(di->di_key, prefix, prefixlen) == 0)
		delete_var(&globvarht, hi);
	}
    hash_unlock(&globvarht);
}

/*
 * List the value of one internal variable.
 */
    static void
list_one_var(dictitem_T *v, char *prefix, 정수 *first)
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
    char	*prefix,
    char_u	*name,
    정수		type,
    char_u	*string,
    정수		*first)  // when TRUE clear rest of screen and set to FALSE
{
    // don't use msg() or msg_attr() to avoid overwriting "v:statusmsg"
    msg_start();
    msg_puts(prefix);
    만약 (name != NULL)	// "a:" vars don't have a name stored
	msg_puts((char *)name);
    msg_putchar(' ');
    msg_advance(22);
    만약 (type == VAR_NUMBER)
	msg_putchar('#');
    아니면 만약 (type == VAR_FUNC || type == VAR_PARTIAL)
	msg_putchar('*');
    아니면 만약 (type == VAR_LIST)
    {
	msg_putchar('[');
	만약 (*string == '[')
	    ++string;
    }
    아니면 만약 (type == VAR_DICT)
    {
	msg_putchar('{');
	만약 (*string == '{')
	    ++string;
    }
    아니면
	msg_putchar(' ');

    msg_outtrans(string);

    만약 (type == VAR_FUNC || type == VAR_PARTIAL)
	msg_puts("()");
    만약 (*first)
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
    정수
before_set_vvar(
    char_u	*varname,
    dictitem_T	*di,
    typval_T	*tv,
    정수		copy,
    정수		*type_error)
{
    만약 (di->di_tv.v_type == VAR_STRING)
    {
	VIM_CLEAR(di->di_tv.vval.v_string);
	만약 (copy || tv->v_type != VAR_STRING)
	{
	    char_u *val = tv_get_string(tv);

	    // Careful: when assigning to v:errmsg and
	    // tv_get_string() causes an error message the variable
	    // will already be set.
	    만약 (di->di_tv.vval.v_string == NULL)
		di->di_tv.vval.v_string = vim_strsave(val);
	}
	아니면
	{
	    // Take over the string to avoid an extra alloc/free.
	    di->di_tv.vval.v_string = tv->vval.v_string;
	    tv->vval.v_string = NULL;
	}
	반환 FALSE;
    }
    아니면 만약 (di->di_tv.v_type == VAR_NUMBER)
    {
	di->di_tv.vval.v_number = tv_get_number(tv);
	만약 (STRCMP(varname, "searchforward") == 0)
	    set_search_direction(di->di_tv.vval.v_number ? '/' : '?');
#ifdef FEAT_SEARCH_EXTRA
	아니면 만약 (STRCMP(varname, "hlsearch") == 0)
	{
	    no_hlsearch = !di->di_tv.vval.v_number;
	    redraw_all_later(UPD_SOME_VALID);
	}
#endif
	반환 FALSE;
    }
    아니면 만약 (di->di_tv.v_type != tv->v_type)
    {
	*type_error = TRUE;
	반환 FALSE;
    }
    반환 TRUE;
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
    정수		copy)	    // make copy of value in "tv"
{
    set_var_const(name, 0, NULL, tv, copy, ASSIGN_DECL, 0);
}

/*
 * Set variable "name" to value in "tv_arg".
 * When "sid" is non-zero "name" is in the script with this ID.
 * If the variable already exists and "is_const" is FALSE the value is updated.
 * Otherwise the variable is created.
 */
    정수
set_var_const(
    char_u	*name,
    scid_T	sid,
    type_T	*type_arg,
    typval_T	*tv_arg,
    정수		copy,	    // make copy of value in "tv"
    정수		flags_arg,  // ASSIGN_CONST, ASSIGN_FINAL, etc.
    정수		var_idx)    // index for ":let [a, b] = list"
{
    typval_T	*tv = tv_arg;
    type_T	*type = type_arg;
    typval_T	bool_tv;
    dictitem_T	*di;
    typval_T	*dest_tv;
    char_u	*varname;
    char_u	*name_tofree = NULL;
    hashtab_T	*ht = NULL;
    정수		is_script_local;
    정수		vim9script = in_vim9script();
    정수		var_in_vim9script;
    정수		var_in_autoload = FALSE;
    정수		flags = flags_arg;
    정수		free_tv_arg = !copy;  // free tv_arg if not used
    정수		rc = FAIL;

    만약 (sid != 0)
    {
	varname = NULL;
	만약 (SCRIPT_ID_VALID(sid))
	{
	    char_u	*auto_name = NULL;
	    만약 (find_var_autoload_prefix(name, sid, &ht, &auto_name) != NULL)
	    {
		var_in_autoload = TRUE;
		varname = auto_name;
		name_tofree = varname;
	    }
	    아니면
		ht = &SCRIPT_VARS(sid);
	}
	만약 (varname == NULL)
	    varname = name;
    }
    아니면
    {
	scriptitem_T	*si;
	char_u		*auto_name = NULL;

	만약 (in_vim9script()
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
	    만약 (varname == NULL)
		goto failed;
	    name_tofree = varname;
	    ht = &globvarht;
	}
	아니면
	    ht = find_var_ht(name, &varname);
    }
    만약 (ht == NULL || *varname == NUL)
    {
	semsg(_(e_illegal_variable_name_str), name);
	goto failed;
    }
    is_script_local = ht == get_script_local_ht() || sid != 0
							    || var_in_autoload;

    만약 (vim9script
	    && !is_script_local
	    && (flags & (ASSIGN_NO_DECL | ASSIGN_DECL)) == 0
	    && (flags & (ASSIGN_CONST | ASSIGN_FINAL)) == 0
	    && name[1] == ':')
    {
	vim9_declare_error(name);
	goto failed;
    }
    만약 ((flags & ASSIGN_FOR_LOOP) && is_scoped_variable(name))
	// Do not make g:var, w:var, b:var or t:var final.
	flags &= ~ASSIGN_FINAL;

    var_in_vim9script = is_script_local && current_script_is_vim9();
    만약 (var_in_vim9script && name[0] == '_' && name[1] == NUL)
    {
	// For "[a, _] = list" the underscore is ignored.
	만약 ((flags & ASSIGN_UNPACK) == 0)
	    emsg(_(e_cannot_use_underscore_here));
	goto failed;
    }

    di = find_var_in_ht(ht, 0, varname, TRUE);

    만약 (di == NULL && var_in_vim9script)
    {
	imported_T  *import = find_imported(varname, 0, FALSE);

	만약 (import != NULL)
	{
	    // imported name space cannot be used
	    만약 ((flags & ASSIGN_NO_DECL) == 0)
	    {
		semsg(_(e_redefining_imported_item_str), name);
		goto failed;
	    }
	    semsg(_(e_cannot_use_str_itself_it_is_imported), name);
	    goto failed;
	}
	만약 (!in_vim9script())
	{
	    semsg(_(e_cannot_create_vim9_script_variable_in_function_str),
									 name);
	    goto failed;
	}
    }

    // Search in parent scope which is possible to reference from lambda
    만약 (di == NULL)
	di = find_var_in_scoped_ht(name, TRUE);

    만약 ((tv->v_type == VAR_FUNC || tv->v_type == VAR_PARTIAL)
				  && var_wrong_func_name(name, di == NULL))
	goto failed;

    만약 (need_convert_to_bool(type, tv))
    {
	// Destination is a bool and the value is not, but it can be
	// converted.
	CLEAR_FIELD(bool_tv);
	bool_tv.v_type = VAR_BOOL;
	bool_tv.vval.v_number = tv2bool(tv) ? VVAL_TRUE : VVAL_FALSE;
	tv = &bool_tv;
    }

    만약 (di != NULL)
    {
	// Item already exists.  Allowed to replace when reloading.
	만약 ((di->di_flags & DI_FLAGS_RELOAD) == 0)
	{
	    만약 ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					 && (flags & ASSIGN_FOR_LOOP) == 0)
	    {
		emsg(_(e_cannot_modify_existing_variable));
		goto failed;
	    }

	    만약 (is_script_local && vim9script
			  && (flags & (ASSIGN_NO_DECL | ASSIGN_DECL)) == 0)
	    {
		semsg(_(e_redefining_script_item_str), name);
		goto failed;
	    }

	    만약 (check_typval_is_value(&di->di_tv) == FAIL)
		goto failed;

	    // List and Blob types can be modified in-place using the "+="
	    // compound operator.  For other types, this is not allowed.
	    정수 type_inplace_modifiable =
		(di->di_tv.v_type == VAR_LIST || di->di_tv.v_type == VAR_BLOB);

	    만약 (var_in_vim9script && (flags & ASSIGN_FOR_LOOP) == 0
		    && ((flags & ASSIGN_COMPOUND_OP) == 0
			|| !type_inplace_modifiable))
	    {
		where_T where = WHERE_INIT;
		svar_T  *sv = find_typval_in_script(&di->di_tv, sid, TRUE);

		만약 (sv != NULL)
		{
		    // check the type and adjust to bool if needed
		    만약 (var_idx > 0)
		    {
			where.wt_index = var_idx;
			where.wt_kind = WT_VARIABLE;
		    }
		    만약 (check_script_var_type(sv, tv, name, where) == FAIL)
			goto failed;
		    만약 (type == NULL)
			type = sv->sv_type;
		    sv->sv_flags |= SVFLAG_ASSIGNED;
		}
	    }

	    // Modifying a final variable with a List value using the "+="
	    // operator is allowed.  For other types, it is not allowed.
	    만약 ((((flags & ASSIGN_FOR_LOOP) == 0 || (flags & ASSIGN_DECL) == 0)
			&& ((flags & ASSIGN_COMPOUND_OP) == 0
			    || !type_inplace_modifiable))
				 ? var_check_permission(di, name) == FAIL
				 : var_check_ro(di->di_flags, name, FALSE))
		goto failed;
	}
	아니면
	{
	    // can only redefine once
	    di->di_flags &= ~DI_FLAGS_RELOAD;

	    // A Vim9 script-local variable is also present in sn_all_vars
	    // and sn_var_vals.  It may set "type" from "tv".
	    만약 (var_in_vim9script || var_in_autoload)
		update_vim9_script_var(FALSE, di,
			var_in_autoload ? name : di->di_key, flags,
			tv, &type, (flags & ASSIGN_NO_MEMBER_TYPE) == 0);
	}

	// existing variable, need to clear the value

	// In Vim9 script a String v: variable only takes a String.
	만약 (ht == &vimvarht && di->di_tv.v_type == VAR_STRING
							    && in_vim9script())
	{
	    where_T	where = WHERE_INIT;

	    만약 (check_typval_type(&t_string, tv, where) == FAIL)
		goto failed;
	}

	// Handle setting internal v: variables separately where needed to
	// prevent changing the type.
	정수 type_error = FALSE;
	만약 (ht == &vimvarht
		&& !before_set_vvar(varname, di, tv, copy, &type_error))
	{
	    만약 (type_error)
		semsg(_(e_setting_v_str_to_value_with_wrong_type), varname);
	    goto failed;
	}

	clear_tv(&di->di_tv);

	만약 ((flags & ASSIGN_UPDATE_BLOCK_ID)
				       && SCRIPT_ID_VALID(current_sctx.sc_sid))
	{
	    scriptitem_T *si = SCRIPT_ITEM(current_sctx.sc_sid);

	    update_script_var_block_id(name, si->sn_current_block_id);
	}
    }
    아니면
    {
	// Item not found, check if a function already exists.
	만약 (is_script_local && (flags & (ASSIGN_NO_DECL | ASSIGN_DECL)) == 0
	       && lookup_scriptitem(name, STRLEN(name), FALSE, NULL) == OK)
	{
	    semsg(_(e_redefining_script_item_str), name);
	    goto failed;
	}

	// add a new variable
	만약 (var_in_vim9script && (flags & ASSIGN_NO_DECL))
	{
	    semsg(_(e_unknown_variable_str), name);
	    goto failed;
	}

	만약 (check_hashtab_frozen(ht, "add variable"))
	    goto failed;

	// Can't add "v:" or "a:" variable.
	만약 (ht == &vimvarht || ht == get_funccal_args_ht())
	{
	    semsg(_(e_illegal_variable_name_str), name);
	    goto failed;
	}

	// Make sure the variable name is valid.  In Vim9 script an
	// autoload variable must be prefixed with "g:" unless in an
	// autoload script.
	만약 (!valid_varname(varname, -1, !vim9script
			|| STRNCMP(name, "g:", 2) == 0 || var_in_autoload))
	    goto failed;

	di = alloc(offsetof(dictitem_T, di_key) + STRLEN(varname) + 1);
	만약 (di == NULL)
	    goto failed;
	STRCPY(di->di_key, varname);
	만약 (hash_add(ht, DI2HIKEY(di), "add variable") == FAIL)
	{
	    vim_free(di);
	    goto failed;
	}
	di->di_flags = DI_FLAGS_ALLOC;
	만약 (flags & (ASSIGN_CONST | ASSIGN_FINAL))
	    di->di_flags |= DI_FLAGS_LOCK;

	// A Vim9 script-local variable is also added to sn_all_vars and
	// sn_var_vals. It may set "type" from "tv".
	만약 (var_in_vim9script || var_in_autoload)
	    update_vim9_script_var(TRUE, di,
		    var_in_autoload ? name : di->di_key, flags,
			  tv, &type, (flags & ASSIGN_NO_MEMBER_TYPE) == 0);
    }

    dest_tv = &di->di_tv;
    만약 (copy || tv->v_type == VAR_NUMBER || tv->v_type == VAR_FLOAT)
	copy_tv(tv, dest_tv);
    아니면
    {
	*dest_tv = *tv;
	dest_tv->v_lock = 0;
	init_tv(tv);
    }
    free_tv_arg = FALSE;

    만약 (vim9script && type != NULL)
	set_tv_type(dest_tv, type);

    // ":const var = value" locks the value
    // ":final var = value" locks "var"
    만약 (flags & ASSIGN_CONST)
	// Like :lockvar! name: lock the value and what it contains, but only
	// if the reference count is up to one.  That locks only literal
	// values.
	item_lock(dest_tv, DICT_MAXNEST, TRUE, TRUE);

    rc = OK;

failed:
    vim_free(name_tofree);
    만약 (free_tv_arg)
	clear_tv(tv_arg);

    반환 rc;
}

/*
 * Check in this order for backwards compatibility:
 * - Whether the variable is read-only
 * - Whether the variable value is locked
 * - Whether the variable is locked
 * NOTE: "name" is only used for error messages.
 */
    정수
var_check_permission(dictitem_T *di, char_u *name)
{
    만약 (var_check_ro(di->di_flags, name, FALSE)
		    || value_check_lock(di->di_tv.v_lock, name, FALSE)
		    || var_check_lock(di->di_flags, name, FALSE))
	반환 FAIL;
    반환 OK;
}

/*
 * Return TRUE if di_flags "flags" indicates variable "name" is read-only.
 * Also give an error message.
 */
    정수
var_check_ro(정수 flags, char_u *name, 정수 use_gettext)
{
    만약 (flags & DI_FLAGS_RO)
    {
	만약 (name == NULL)
	    emsg(_(e_cannot_change_readonly_variable));
	아니면
	    semsg(_(e_cannot_change_readonly_variable_str),
				       use_gettext ? (char_u *)_(name) : name);
	반환 TRUE;
    }
    만약 ((flags & DI_FLAGS_RO_SBX) && sandbox)
    {
	만약 (name == NULL)
	    emsg(_(e_cannot_set_variable_in_sandbox));
	아니면
	    semsg(_(e_cannot_set_variable_in_sandbox_str),
				       use_gettext ? (char_u *)_(name) : name);
	반환 TRUE;
    }
    반환 FALSE;
}

/*
 * Return TRUE if di_flags "flags" indicates variable "name" is locked.
 * Also give an error message.
 */
    정수
var_check_lock(정수 flags, char_u *name, 정수 use_gettext)
{
    만약 (flags & DI_FLAGS_LOCK)
    {
	semsg(_(e_variable_is_locked_str),
				       use_gettext ? (char_u *)_(name) : name);
	반환 TRUE;
    }
    반환 FALSE;
}

/*
 * Return TRUE if di_flags "flags" indicates variable "name" is fixed.
 * Also give an error message.
 */
    정수
var_check_fixed(정수 flags, char_u *name, 정수 use_gettext)
{
    만약 (flags & DI_FLAGS_FIX)
    {
	만약 (name == NULL)
	    emsg(_(e_cannot_delete_variable));
	아니면
	    semsg(_(e_cannot_delete_variable_str),
				      use_gettext ? (char_u *)_(name) : name);
	반환 TRUE;
    }
    반환 FALSE;
}

/*
 * Check if a funcref is assigned to a valid variable name.
 * Return TRUE and give an error if not.
 */
    정수
var_wrong_func_name(
    char_u *name,    // points to start of variable name
    정수    new_var)  // TRUE when creating the variable
{
    // Allow for w: b: s: and t:.  In Vim9 script s: is not allowed, because
    // the name can be used without the s: prefix.
    // Allow autoload variable.
    만약 (!((vim_strchr((char_u *)"wbt", name[0]) != NULL
		    || (!in_vim9script() && name[0] == 's')) && name[1] == ':')
	    && !ASCII_ISUPPER((name[0] != NUL && name[1] == ':')
						     ? name[2] : name[0])
	    && vim_strchr(name, '#') == NULL)
    {
	semsg(_(e_funcref_variable_name_must_start_with_capital_str), name);
	반환 TRUE;
    }
    // Don't allow hiding a function.  When "v" is not NULL we might be
    // assigning another function to the same var, the type is checked
    // below.
    만약 (new_var && function_exists(name, FALSE))
    {
	semsg(_(e_variable_name_conflicts_with_existing_function_str),
								    name);
	반환 TRUE;
    }
    반환 FALSE;
}

/*
 * Return TRUE if "flags" indicates variable "name" has a locked (immutable)
 * value.  Also give an error message, using "name" or _("name") when
 * "use_gettext" is TRUE.
 */
    정수
value_check_lock(정수 lock, char_u *name, 정수 use_gettext)
{
    만약 (lock & VAR_LOCKED)
    {
	만약 (name == NULL)
	    emsg(_(e_value_is_locked));
	아니면
	    semsg(_(e_value_is_locked_str),
				       use_gettext ? (char_u *)_(name) : name);
	반환 TRUE;
    }
    만약 (lock & VAR_FIXED)
    {
	만약 (name == NULL)
	    emsg(_(e_cannot_change_value));
	아니면
	    semsg(_(e_cannot_change_value_of_str),
				       use_gettext ? (char_u *)_(name) : name);
	반환 TRUE;
    }
    반환 FALSE;
}

/*
 * Check if a variable name is valid.  When "autoload" is true "#" is allowed.
 * If "len" is -1 use all of "varname", otherwise up to "varname[len]".
 * Return FALSE and give an error if not.
 */
    정수
valid_varname(char_u *varname, 정수 len, 정수 autoload)
{
    char_u *p;

    위해 (p = varname; len < 0 ? *p != NUL : p < varname + len; ++p)
	만약 (!eval_isnamec1(*p) && (p == varname || !VIM_ISDIGIT(*p))
					 && !(autoload && *p == AUTOLOAD_CHAR))
	{
	    semsg(_(e_illegal_variable_name_str), varname);
	    반환 FALSE;
	}
    반환 TRUE;
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
    정수		htname,	    // 't'ab, 'w'indow or 'b'uffer local.
    tabpage_T	*tp,	    // can be NULL
    win_T	*win,
    buf_T	*buf)	    // Ignored if htname is not 'b'.
{
    dictitem_T	*v;
    정수		done = FALSE;
    switchwin_T	switchwin;
    정수		need_switch_win;
    정수		do_change_curbuf = buf != NULL && htname == 'b';

    ++emsg_off;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;

    만약 (varname != NULL && tp != NULL && win != NULL
	    && (htname != 'b' || buf != NULL))
    {
	// Set curwin to be our win, temporarily.  Also set the tabpage,
	// otherwise the window is not valid. Only do this when needed,
	// autocommands get blocked.
	// If we have a buffer reference avoid the switching, we're saving and
	// restoring curbuf directly.
	need_switch_win = !(tp == curtab && win == curwin) && !do_change_curbuf;
	만약 (!need_switch_win || switch_win(&switchwin, win, tp, TRUE) == OK)
	{
	    // Handle options. There are no tab-local options.
	    만약 (*varname == '&' && htname != 't')
	    {
		buf_T	*save_curbuf = curbuf;

		// Change curbuf so the option is read from the correct buffer.
		만약 (do_change_curbuf)
		    curbuf = buf;

		만약 (varname[1] == NUL)
		{
		    // get all window-local or buffer-local options in a dict
		    dict_T	*opts = get_winbuf_options(htname == 'b');

		    만약 (opts != NULL)
		    {
			rettv_dict_set(rettv, opts);
			done = TRUE;
		    }
		}
		아니면 만약 (eval_option(&varname, rettv, TRUE) == OK)
		    // Local option
		    done = TRUE;

		curbuf = save_curbuf;
	    }
	    아니면 만약 (*varname == NUL)
	    {
		// Empty string: return a dict with all the local variables.
		만약 (htname == 'b')
		    v = &buf->b_bufvar;
		아니면 만약 (htname == 'w')
		    v = &win->w_winvar;
		아니면
		    v = &tp->tp_winvar;
		copy_tv(&v->di_tv, rettv);
		done = TRUE;
	    }
	    아니면
	    {
		hashtab_T	*ht;

		만약 (htname == 'b')
		    ht = &buf->b_vars->dv_hashtab;
		아니면 만약 (htname == 'w')
		    ht = &win->w_vars->dv_hashtab;
		아니면
		    ht = &tp->tp_vars->dv_hashtab;

		// Look up the variable.
		v = find_var_in_ht(ht, htname, varname, FALSE);
		만약 (v != NULL)
		{
		    copy_tv(&v->di_tv, rettv);
		    done = TRUE;
		}
	    }
	}

	만약 (need_switch_win)
	    // restore previous notion of curwin
	    restore_win(&switchwin, TRUE);
    }

    만약 (!done && deftv->v_type != VAR_UNKNOWN)
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
    정수		off)	    // 1 for gettabwinvar()
{
    char_u	*varname;
    tabpage_T	*tp;
    win_T	*win;

    만약 (off == 1)
	tp = find_tabpage((정수)tv_get_number_chk(&argvars[0], NULL));
    아니면
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
    정수		error = FALSE;

    정수		opt_idx = findoption(varname);
    만약 (opt_idx < 0)
    {
	semsg(_(e_unknown_option_str_2), varname);
	반환;
    }
    정수		opt_p_flags = get_option_flags(opt_idx);

    만약 (varp->v_type == VAR_BOOL)
    {
	만약 (opt_p_flags & P_STRING)
	{
	    emsg(_(e_string_required));
	    반환;
	}
	numval = (long)varp->vval.v_number;
	strval = (char_u *)"0";  // avoid using "false"
    }
    아니면
    {
	만약 ((opt_p_flags & (P_NUM|P_BOOL))
		&& (!in_vim9script() || varp->v_type != VAR_STRING))
	    numval = (long)tv_get_number_chk(varp, &error);
	만약 (!error)
	    strval = tv_get_string_buf_chk(varp, nbuf);
    }
    만약 (!error && strval != NULL)
	set_option_value_give_err(varname, numval, strval, OPT_LOCAL);
}

/*
 * "setwinvar()" and "settabwinvar()" functions
 */
    static void
setwinvar(typval_T *argvars, 정수 off)
{
    win_T	*win;
    switchwin_T	switchwin;
    정수		need_switch_win;
    char_u	*varname, *winvarname;
    typval_T	*varp;
    tabpage_T	*tp = NULL;

    만약 (check_secure())
	반환;

    만약 (off == 1)
	tp = find_tabpage((정수)tv_get_number_chk(&argvars[0], NULL));
    아니면
	tp = curtab;
    win = find_win_by_nr(&argvars[off], tp);
    varname = tv_get_string_chk(&argvars[off + 1]);
    varp = &argvars[off + 2];

    만약 (win == NULL || varname == NULL)
	반환;

    need_switch_win = !(tp == curtab && win == curwin);
    만약 (!need_switch_win
	    || switch_win(&switchwin, win, tp, TRUE) == OK)
    {
	만약 (*varname == '&')
	    set_option_from_tv(varname + 1, varp);
	아니면
	{
	    winvarname = alloc(STRLEN(varname) + 3);
	    만약 (winvarname != NULL)
	    {
		STRCPY(winvarname, "w:");
		STRCPY(winvarname + 2, varname);
		set_var(winvarname, varp, TRUE);
		vim_free(winvarname);
	    }
	}
    }
    만약 (need_switch_win)
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
    struct vimvar   *vp = &vimvars[VV_ERRORS];

    만약 (vp->vv_tv_type != VAR_LIST || vimvars[VV_ERRORS].vv_list == NULL)
	// Make sure v:errors is a list.
	set_vim_var_list(VV_ERRORS, list_alloc());
    list_append_string(vimvars[VV_ERRORS].vv_list, gap->ga_data, gap->ga_len);
}

    정수
var_exists(char_u *var)
{
    char_u	*arg = var;
    char_u	*name;
    char_u	*tofree;
    typval_T    tv;
    정수		len = 0;
    정수		n = FALSE;

    // get_name_len() takes care of expanding curly braces
    name = var;
    len = get_name_len(&arg, &tofree, TRUE, FALSE);
    만약 (len > 0)
    {
	만약 (tofree != NULL)
	    name = tofree;
	n = (eval_variable(name, len, 0, &tv, NULL,
				 EVAL_VAR_NOAUTOLOAD + EVAL_VAR_IMPORT) == OK);
	만약 (n)
	{
	    // handle d.key, l[idx], f(expr)
	    arg = skipwhite(arg);
	    n = (handle_subscript(&arg, name, &tv, &EVALARG_EVALUATE,
								 FALSE) == OK);
	    만약 (n)
		clear_tv(&tv);
	}
    }
    만약 (*arg != NUL)
	n = FALSE;

    vim_free(tofree);
    반환 n;
}

static lval_T	*redir_lval = NULL;
#define EVALCMD_BUSY (redir_lval == (lval_T *)&redir_lval)
static garray_T redir_ga;	// only valid when redir_lval is not NULL
static char_u	*redir_endp = NULL;
static char_u	*redir_varname = NULL;

    정수
alloc_redir_lval(void)
{
    redir_lval = ALLOC_CLEAR_ONE(lval_T);
    만약 (redir_lval == NULL)
	반환 FAIL;
    반환 OK;
}

    void
clear_redir_lval(void)
{
    VIM_CLEAR(redir_lval);
}

    void
init_redir_ga(void)
{
    ga_init2(&redir_ga, sizeof(char), 500);
}

/*
 * Start recording command output to a variable
 * When "append" is TRUE append to an existing variable.
 * Returns OK if successfully completed the setup.  FAIL otherwise.
 */
    정수
var_redir_start(char_u *name, 정수 append)
{
    정수		called_emsg_before;
    typval_T	tv;

    // Catch a bad name early.
    만약 (!eval_isnamec1(*name))
    {
	emsg(_(e_invalid_argument));
	반환 FAIL;
    }

    // Make a copy of the name, it is used in redir_lval until redir ends.
    redir_varname = vim_strsave(name);
    만약 (redir_varname == NULL)
	반환 FAIL;

    만약 (alloc_redir_lval() == FAIL)
    {
	var_redir_stop();
	반환 FAIL;
    }

    // The output is stored in growarray "redir_ga" until redirection ends.
    init_redir_ga();

    // Parse the variable name (can be a dict or list entry).
    redir_endp = get_lval(redir_varname, NULL, redir_lval, FALSE, FALSE, 0,
							     FNE_CHECK_START);
    만약 (redir_endp == NULL || redir_lval->ll_name == NULL || *redir_endp != NUL)
    {
	clear_lval(redir_lval);
	만약 (redir_endp != NULL && *redir_endp != NUL)
	    // Trailing characters are present after the variable name
	    semsg(_(e_trailing_characters_str), redir_endp);
	아니면
	    semsg(_(e_invalid_argument_str), name);
	redir_endp = NULL;  // don't store a value, only cleanup
	var_redir_stop();
	반환 FAIL;
    }

    // check if we can write to the variable: set it to or append an empty
    // string
    called_emsg_before = called_emsg;
    tv.v_type = VAR_STRING;
    tv.vval.v_string = (char_u *)"";
    만약 (append)
	set_var_lval(redir_lval, redir_endp, &tv, TRUE,
					     ASSIGN_NO_DECL, (char_u *)".", 0);
    아니면
	set_var_lval(redir_lval, redir_endp, &tv, TRUE,
					     ASSIGN_NO_DECL, (char_u *)"=", 0);
    clear_lval(redir_lval);
    만약 (called_emsg > called_emsg_before)
    {
	redir_endp = NULL;  // don't store a value, only cleanup
	var_redir_stop();
	반환 FAIL;
    }

    반환 OK;
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
var_redir_str(char_u *value, 정수 value_len)
{
    정수		len;

    만약 (redir_lval == NULL)
	반환;

    만약 (value_len == -1)
	len = (정수)STRLEN(value);	// Append the entire string
    아니면
	len = value_len;		// Append only "value_len" characters

    만약 (ga_grow(&redir_ga, len) == OK)
    {
	mch_memmove((char *)redir_ga.ga_data + redir_ga.ga_len, value, len);
	redir_ga.ga_len += len;
    }
    아니면
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

    만약 (EVALCMD_BUSY)
    {
	redir_lval = NULL;
	반환;
    }

    만약 (redir_lval != NULL)
    {
	// If there was no error: assign the text to the variable.
	만약 (redir_endp != NULL)
	{
	    ga_append(&redir_ga, NUL);  // Append the trailing NUL.
	    tv.v_type = VAR_STRING;
	    tv.vval.v_string = redir_ga.ga_data;
	    // Call get_lval() again, if it's inside a Dict or List it may
	    // have changed.
	    redir_endp = get_lval(redir_varname, NULL, redir_lval,
					FALSE, FALSE, 0, FNE_CHECK_START);
	    만약 (redir_endp != NULL && redir_lval->ll_name != NULL)
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
    반환 res;
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

    만약 (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	반환;

    varname = tv_get_string_chk(&argvars[1]);
    tp = find_tabpage((정수)tv_get_number_chk(&argvars[0], NULL));
    만약 (tp != NULL)
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
    만약 (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL))
	반환;

    getwinvar(argvars, rettv, 1);
}

/*
 * "getwinvar()" function
 */
    void
f_getwinvar(typval_T *argvars, typval_T *rettv)
{
    만약 (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	반환;

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

    만약 (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	반환;

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

    만약 (check_secure())
	반환;

    만약 (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	반환;

    tp = find_tabpage((정수)tv_get_number_chk(&argvars[0], NULL));
    varname = tv_get_string_chk(&argvars[1]);
    varp = &argvars[2];

    만약 (varname == NULL || tp == NULL)
	반환;

    save_curtab = curtab;
    save_lu_tp = lastused_tabpage;
    goto_tabpage_tp(tp, FALSE, FALSE);

    tabvarname = alloc(STRLEN(varname) + 3);
    만약 (tabvarname != NULL)
    {
	STRCPY(tabvarname, "t:");
	STRCPY(tabvarname + 2, varname);
	set_var(tabvarname, varp, TRUE);
	vim_free(tabvarname);
    }

    // Restore current tabpage and last accessed tabpage.
    만약 (valid_tabpage(save_curtab))
    {
	goto_tabpage_tp(save_curtab, FALSE, FALSE);
	만약 (valid_tabpage(save_lu_tp))
	    lastused_tabpage = save_lu_tp;
    }
}

/*
 * "settabwinvar()" function
 */
    void
f_settabwinvar(typval_T *argvars, typval_T *rettv UNUSED)
{
    만약 (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL))
	반환;

    setwinvar(argvars, 1);
}

/*
 * "setwinvar()" function
 */
    void
f_setwinvar(typval_T *argvars, typval_T *rettv UNUSED)
{
    만약 (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	반환;

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

    만약 (check_secure())
	반환;

    만약 (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	반환;

    varname = tv_get_string_chk(&argvars[1]);
    buf = tv_get_buf_from_arg(&argvars[0]);
    varp = &argvars[2];

    만약 (buf == NULL || varname == NULL)
	반환;

    만약 (*varname == '&')
    {
	aco_save_T	aco;
	// safe the current window position, it could
	// change because of 'scrollbind' window-local
	// options
	linenr_T	old_topline = curwin->w_topline;

	// Set curbuf to be our buf, temporarily.
	aucmd_prepbuf(&aco, buf);
	만약 (curbuf == buf)
	{
	    // Only when it worked to set "curbuf".
	    set_option_from_tv(varname + 1, varp);

	    // reset notion of buffer
	    aucmd_restbuf(&aco);
	}
	curwin->w_topline = old_topline;
    }
    아니면
    {
	bufvarname = alloc(STRLEN(varname) + 3);
	만약 (bufvarname != NULL)
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
    정수		r = OK;

    CLEAR_FIELD(res);
    만약 (arg->v_type == VAR_PARTIAL && arg->vval.v_partial != NULL)
    {
	res.cb_partial = arg->vval.v_partial;
	++res.cb_partial->pt_refcount;
	res.cb_name = partial_name(res.cb_partial);
    }
    아니면
    {
	만약 (arg->v_type == VAR_STRING && arg->vval.v_string != NULL
					       && SAFE_isdigit(*arg->vval.v_string))
	    r = FAIL;
	아니면 만약 (arg->v_type == VAR_FUNC || arg->v_type == VAR_STRING)
	{
	    res.cb_name = arg->vval.v_string;
	    만약 (arg->v_type == VAR_STRING)
	    {
		char_u *name = get_scriptlocal_funcname(arg->vval.v_string);
		만약 (name != NULL)
		{
		    res.cb_name = name;
		    res.cb_free_name = TRUE;
		}
	    }
	    func_ref(res.cb_name);
	}
	아니면 만약 (arg->v_type == VAR_NUMBER && arg->vval.v_number == 0)
	    res.cb_name = (char_u *)"";
	아니면
	    r = FAIL;

	만약 (r == FAIL)
	{
	    emsg(_(e_invalid_callback_argument));
	    res.cb_name = NULL;
	}
    }
    반환 res;
}

/*
 * Copy a callback into a typval_T.
 */
    void
put_callback(callback_T *cb, typval_T *tv)
{
    만약 (cb->cb_partial != NULL)
    {
	tv->v_type = VAR_PARTIAL;
	tv->vval.v_partial = cb->cb_partial;
	++tv->vval.v_partial->pt_refcount;
    }
    아니면
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
    반환 cb->cb_partial || cb->cb_free_name;
}

/*
 * Make a copy of "src" into "dest", allocating the function name if needed,
 * without incrementing the refcount.
 */
    void
set_callback(callback_T *dest, callback_T *src)
{
    *dest = *src;
    만약 (!does_callback_own_cb_name(src))
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
    만약 (dest->cb_partial != NULL)
    {
	dest->cb_name = src->cb_name;
	dest->cb_free_name = FALSE;
	++dest->cb_partial->pt_refcount;
    }
    아니면
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

    만약 (!in_vim9script() || cb->cb_name == NULL
	    || (!cb->cb_free_name
	       && (cb->cb_partial == NULL || cb->cb_partial->pt_name == NULL)))
	반환;
    만약 (cb->cb_partial != NULL)
	name = cb->cb_partial->pt_name;
    아니면
	name = cb->cb_name;
    p = vim_strchr(name, '.');
    만약 (p == NULL)
	반환;

    import = find_imported(name, p - name, FALSE);
    만약 (import == NULL || !SCRIPT_ID_VALID(import->imp_sid))
	반환;

    scriptitem_T *si = SCRIPT_ITEM(import->imp_sid);
    만약 (si->sn_autoload_prefix == NULL)
	반환;

    char_u *newname = concat_str(si->sn_autoload_prefix, p + 1);
    만약 (newname == NULL)
	반환;

    만약 (cb->cb_partial != NULL)
    {
	만약 (cb->cb_name == cb->cb_partial->pt_name)
	    cb->cb_name = newname;
	vim_free(cb->cb_partial->pt_name);
	cb->cb_partial->pt_name = newname;
    }
    아니면
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
    만약 (callback->cb_partial != NULL)
    {
	partial_unref(callback->cb_partial);
	callback->cb_partial = NULL;
    }
    아니면 만약 (callback->cb_name != NULL)
	func_unref(callback->cb_name);
    만약 (callback->cb_free_name)
    {
	vim_free(callback->cb_name);
	callback->cb_free_name = FALSE;
    }
    callback->cb_name = NULL;
}

#endif // FEAT_EVAL
