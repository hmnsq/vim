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

#అన్తర్భావ "vim.h"

#ఒకవేళ defined(FEAT_EVAL)

స్థిర dictitem_T	globvars_var;		// variable used for g:
స్థిర dict_T		globvardict;		// Dictionary with g: variables
#నిర్వచించు globvarht globvardict.dv_hashtab

/*
 * Old Vim variables such as "v:version" are also available without the "v:".
 * Also in functions.  We need a special hashtable for them.
 */
స్థిర hashtab_T	compat_hashtab;

/*
 * Array to hold the value of v: variables.
 * The value is in a dictitem, so that it can also be used in the v: scope.
 * The reason to use this table anyway is for very quick access to the
 * variables with the VV_ defines.
 */

// values for vv_flags:
#నిర్వచించు VV_COMPAT	1	// compatible, also used without "v:"
#నిర్వచించు VV_RO		2	// read-only
#నిర్వచించు VV_RO_SBX	4	// read-only in the sandbox

#నిర్వచించు VV_NAME(s, t)	s, {{t, 0, {0}}, 0, {s}}

రకనిర్వచనం నిర్మాణం vimvar vimvar_T;

స్థిర నిర్మాణం vimvar
{
    అక్షరం	*vv_name;	// name of variable, without v:
    dictitem16_T vv_di;		// value and name for key (max 16 chars!)
    type_T	*vv_type;	// type or NULL
    అక్షరం	vv_flags;	// VV_COMPAT, VV_RO, VV_RO_SBX
} vimvars[VV_LEN] =
{
    // The order here must match the VV_ defines in vim.h!
    // Initializing a union does not work, leave tv.vval empty to get zero's.
    {VV_NAME("count",		 VAR_సంఖ్య), ఖాళీ, VV_COMPAT+VV_RO},
    {VV_NAME("count1",		 VAR_సంఖ్య), ఖాళీ, VV_RO},
    {VV_NAME("prevcount",	 VAR_సంఖ్య), ఖాళీ, VV_RO},
    {VV_NAME("errmsg",		 VAR_STRING), ఖాళీ, VV_COMPAT},
    {VV_NAME("warningmsg",	 VAR_STRING), ఖాళీ, 0},
    {VV_NAME("statusmsg",	 VAR_STRING), ఖాళీ, 0},
    {VV_NAME("shell_error",	 VAR_సంఖ్య), ఖాళీ, VV_COMPAT+VV_RO},
    {VV_NAME("this_session",	 VAR_STRING), ఖాళీ, VV_COMPAT},
    {VV_NAME("version",		 VAR_సంఖ్య), ఖాళీ, VV_COMPAT+VV_RO},
    {VV_NAME("lnum",		 VAR_సంఖ్య), ఖాళీ, VV_RO_SBX},
    {VV_NAME("termresponse",	 VAR_STRING), ఖాళీ, VV_RO},
    {VV_NAME("fname",		 VAR_STRING), ఖాళీ, VV_RO},
    {VV_NAME("lang",		 VAR_STRING), ఖాళీ, VV_RO},
    {VV_NAME("lc_time",		 VAR_STRING), ఖాళీ, VV_RO},
    {VV_NAME("ctype",		 VAR_STRING), ఖాళీ, VV_RO},
    {VV_NAME("charconvert_from", VAR_STRING), ఖాళీ, VV_RO},
    {VV_NAME("charconvert_to",	 VAR_STRING), ఖాళీ, VV_RO},
    {VV_NAME("fname_in",	 VAR_STRING), ఖాళీ, VV_RO},
    {VV_NAME("fname_out",	 VAR_STRING), ఖాళీ, VV_RO},
    {VV_NAME("fname_new",	 VAR_STRING), ఖాళీ, VV_RO},
    {VV_NAME("fname_diff",	 VAR_STRING), ఖాళీ, VV_RO},
    {VV_NAME("cmdarg",		 VAR_STRING), ఖాళీ, VV_RO},
    {VV_NAME("foldstart",	 VAR_సంఖ్య), ఖాళీ, VV_RO_SBX},
    {VV_NAME("foldend",		 VAR_సంఖ్య), ఖాళీ, VV_RO_SBX},
    {VV_NAME("folddashes",	 VAR_STRING), ఖాళీ, VV_RO_SBX},
    {VV_NAME("foldlevel",	 VAR_సంఖ్య), ఖాళీ, VV_RO_SBX},
    {VV_NAME("progname",	 VAR_STRING), ఖాళీ, VV_RO},
    {VV_NAME("servername",	 VAR_STRING), ఖాళీ, VV_RO},
    {VV_NAME("dying",		 VAR_సంఖ్య), ఖాళీ, VV_RO},
    {VV_NAME("exception",	 VAR_STRING), ఖాళీ, VV_RO},
    {VV_NAME("throwpoint",	 VAR_STRING), ఖాళీ, VV_RO},
    {VV_NAME("register",	 VAR_STRING), ఖాళీ, VV_RO},
    {VV_NAME("cmdbang",		 VAR_సంఖ్య), ఖాళీ, VV_RO},
    {VV_NAME("insertmode",	 VAR_STRING), ఖాళీ, VV_RO},
    {VV_NAME("val",		 VAR_UNKNOWN), ఖాళీ, VV_RO},
    {VV_NAME("key",		 VAR_UNKNOWN), ఖాళీ, VV_RO},
    {VV_NAME("profiling",	 VAR_సంఖ్య), ఖాళీ, VV_RO},
    {VV_NAME("fcs_reason",	 VAR_STRING), ఖాళీ, VV_RO},
    {VV_NAME("fcs_choice",	 VAR_STRING), ఖాళీ, 0},
    {VV_NAME("beval_bufnr",	 VAR_సంఖ్య), ఖాళీ, VV_RO},
    {VV_NAME("beval_winnr",	 VAR_సంఖ్య), ఖాళీ, VV_RO},
    {VV_NAME("beval_winid",	 VAR_సంఖ్య), ఖాళీ, VV_RO},
    {VV_NAME("beval_lnum",	 VAR_సంఖ్య), ఖాళీ, VV_RO},
    {VV_NAME("beval_col",	 VAR_సంఖ్య), ఖాళీ, VV_RO},
    {VV_NAME("beval_text",	 VAR_STRING), ఖాళీ, VV_RO},
    {VV_NAME("scrollstart",	 VAR_STRING), ఖాళీ, 0},
    {VV_NAME("swapname",	 VAR_STRING), ఖాళీ, VV_RO},
    {VV_NAME("swapchoice",	 VAR_STRING), ఖాళీ, 0},
    {VV_NAME("swapcommand",	 VAR_STRING), ఖాళీ, VV_RO},
    {VV_NAME("char",		 VAR_STRING), ఖాళీ, 0},
    {VV_NAME("mouse_win",	 VAR_సంఖ్య), ఖాళీ, 0},
    {VV_NAME("mouse_winid",	 VAR_సంఖ్య), ఖాళీ, 0},
    {VV_NAME("mouse_lnum",	 VAR_సంఖ్య), ఖాళీ, 0},
    {VV_NAME("mouse_col",	 VAR_సంఖ్య), ఖాళీ, 0},
    {VV_NAME("operator",	 VAR_STRING), ఖాళీ, VV_RO},
    {VV_NAME("searchforward",	 VAR_సంఖ్య), ఖాళీ, 0},
    {VV_NAME("hlsearch",	 VAR_సంఖ్య), ఖాళీ, 0},
    {VV_NAME("oldfiles",	 VAR_LIST), &t_list_string, 0},
    {VV_NAME("windowid",	 VAR_సంఖ్య), ఖాళీ, VV_RO},
    {VV_NAME("progpath",	 VAR_STRING), ఖాళీ, VV_RO},
    {VV_NAME("completed_item",	 VAR_DICT), &t_dict_string, 0},
    {VV_NAME("option_new",	 VAR_STRING), ఖాళీ, VV_RO},
    {VV_NAME("option_old",	 VAR_STRING), ఖాళీ, VV_RO},
    {VV_NAME("option_oldlocal",	 VAR_STRING), ఖాళీ, VV_RO},
    {VV_NAME("option_oldglobal", VAR_STRING), ఖాళీ, VV_RO},
    {VV_NAME("option_command",	 VAR_STRING), ఖాళీ, VV_RO},
    {VV_NAME("option_type",	 VAR_STRING), ఖాళీ, VV_RO},
    {VV_NAME("errors",		 VAR_LIST), &t_list_string, 0},
    {VV_NAME("false",		 VAR_BOOL), ఖాళీ, VV_RO},
    {VV_NAME("true",		 VAR_BOOL), ఖాళీ, VV_RO},
    {VV_NAME("none",		 VAR_SPECIAL), ఖాళీ, VV_RO},
    {VV_NAME("null",		 VAR_SPECIAL), ఖాళీ, VV_RO},
    {VV_NAME("numbermax",	 VAR_సంఖ్య), ఖాళీ, VV_RO},
    {VV_NAME("numbermin",	 VAR_సంఖ్య), ఖాళీ, VV_RO},
    {VV_NAME("numbersize",	 VAR_సంఖ్య), ఖాళీ, VV_RO},
    {VV_NAME("vim_did_enter",	 VAR_సంఖ్య), ఖాళీ, VV_RO},
    {VV_NAME("testing",		 VAR_సంఖ్య), ఖాళీ, 0},
    {VV_NAME("t_number",	 VAR_సంఖ్య), ఖాళీ, VV_RO},
    {VV_NAME("t_string",	 VAR_సంఖ్య), ఖాళీ, VV_RO},
    {VV_NAME("t_func",		 VAR_సంఖ్య), ఖాళీ, VV_RO},
    {VV_NAME("t_list",		 VAR_సంఖ్య), ఖాళీ, VV_RO},
    {VV_NAME("t_dict",		 VAR_సంఖ్య), ఖాళీ, VV_RO},
    {VV_NAME("t_float",		 VAR_సంఖ్య), ఖాళీ, VV_RO},
    {VV_NAME("t_bool",		 VAR_సంఖ్య), ఖాళీ, VV_RO},
    {VV_NAME("t_none",		 VAR_సంఖ్య), ఖాళీ, VV_RO},
    {VV_NAME("t_job",		 VAR_సంఖ్య), ఖాళీ, VV_RO},
    {VV_NAME("t_channel",	 VAR_సంఖ్య), ఖాళీ, VV_RO},
    {VV_NAME("t_blob",		 VAR_సంఖ్య), ఖాళీ, VV_RO},
    {VV_NAME("t_class",		 VAR_సంఖ్య), ఖాళీ, VV_RO},
    {VV_NAME("t_object",	 VAR_సంఖ్య), ఖాళీ, VV_RO},
    {VV_NAME("termrfgresp",	 VAR_STRING), ఖాళీ, VV_RO},
    {VV_NAME("termrbgresp",	 VAR_STRING), ఖాళీ, VV_RO},
    {VV_NAME("termu7resp",	 VAR_STRING), ఖాళీ, VV_RO},
    {VV_NAME("termstyleresp",	 VAR_STRING), ఖాళీ, VV_RO},
    {VV_NAME("termblinkresp",	 VAR_STRING), ఖాళీ, VV_RO},
    {VV_NAME("event",		 VAR_DICT), ఖాళీ, VV_RO},
    {VV_NAME("versionlong",	 VAR_సంఖ్య), ఖాళీ, VV_RO},
    {VV_NAME("echospace",	 VAR_సంఖ్య), ఖాళీ, VV_RO},
    {VV_NAME("argv",		 VAR_LIST), &t_list_string, VV_RO},
    {VV_NAME("collate",		 VAR_STRING), ఖాళీ, VV_RO},
    {VV_NAME("exiting",		 VAR_SPECIAL), ఖాళీ, VV_RO},
    {VV_NAME("colornames",       VAR_DICT), &t_dict_string, VV_RO},
    {VV_NAME("sizeofint",	 VAR_సంఖ్య), ఖాళీ, VV_RO},
    {VV_NAME("sizeoflong",	 VAR_సంఖ్య), ఖాళీ, VV_RO},
    {VV_NAME("sizeofpointer",	 VAR_సంఖ్య), ఖాళీ, VV_RO},
    {VV_NAME("maxcol",		 VAR_సంఖ్య), ఖాళీ, VV_RO},
    {VV_NAME("python3_version",	 VAR_సంఖ్య), ఖాళీ, VV_RO},
    {VV_NAME("t_typealias",	 VAR_సంఖ్య), ఖాళీ, VV_RO},
    {VV_NAME("t_enum",		 VAR_సంఖ్య), ఖాళీ, VV_RO},
    {VV_NAME("t_enumvalue",	 VAR_సంఖ్య), ఖాళీ, VV_RO},
    {VV_NAME("stacktrace",	 VAR_LIST), &t_list_dict_any, VV_RO},
    {VV_NAME("t_tuple",		 VAR_సంఖ్య), ఖాళీ, VV_RO},
    {VV_NAME("wayland_display",  VAR_STRING), ఖాళీ, VV_RO},
    {VV_NAME("clipmethod",	 VAR_STRING), ఖాళీ, VV_RO},
    {VV_NAME("termda1",		 VAR_STRING), ఖాళీ, VV_RO},
    {VV_NAME("termosc",	 VAR_STRING), ఖాళీ, VV_RO},
    {VV_NAME("vim_did_init",	 VAR_సంఖ్య), ఖాళీ, VV_RO},
    {VV_NAME("clipproviders",	 VAR_DICT), ఖాళీ, VV_RO},
    {VV_NAME("imagebackend",	 VAR_STRING), ఖాళీ, VV_RO}
};

// shorthand
#నిర్వచించు vv_tv_type	vv_di.di_tv.v_type
#నిర్వచించు vv_nr		vv_di.di_tv.vval.v_సంఖ్య
#నిర్వచించు vv_float	vv_di.di_tv.vval.v_float
#నిర్వచించు vv_str		vv_di.di_tv.vval.v_string
#నిర్వచించు vv_list		vv_di.di_tv.vval.v_list
#నిర్వచించు vv_dict		vv_di.di_tv.vval.v_dict
#నిర్వచించు vv_blob		vv_di.di_tv.vval.v_blob
#నిర్వచించు vv_tv		vv_di.di_tv

స్థిర dictitem_T	vimvars_var;		// variable used for v:
స్థిర dict_T		vimvardict;		// Dictionary with v: variables
#నిర్వచించు vimvarht  vimvardict.dv_hashtab

// for VIM_VERSION_ defines
#అన్తర్భావ "version.h"

స్థిర శూన్య list_glob_vars(పూర్ణాంకం *మొదటి);
స్థిర శూన్య list_buf_vars(పూర్ణాంకం *మొదటి);
స్థిర శూన్య list_win_vars(పూర్ణాంకం *మొదటి);
స్థిర శూన్య list_tab_vars(పూర్ణాంకం *మొదటి);
స్థిర char_u *list_arg_vars(exarg_T *eap, char_u *arg, పూర్ణాంకం *మొదటి);
స్థిర char_u *ex_let_one(char_u *arg, typval_T *tv, పూర్ణాంకం copy, పూర్ణాంకం flags, char_u *endchars, char_u *op, పూర్ణాంకం var_idx);
స్థిర పూర్ణాంకం do_unlet_var(lval_T *lp, char_u *name_చివర, exarg_T *eap, పూర్ణాంకం deep, శూన్య *cookie);
స్థిర పూర్ణాంకం do_తాళం_var(lval_T *lp, char_u *name_చివర, exarg_T *eap, పూర్ణాంకం deep, శూన్య *cookie);
స్థిర శూన్య list_one_var(dictitem_T *v, అక్షరం *ఉపసర్గ, పూర్ణాంకం *మొదటి);
స్థిర శూన్య list_one_var_a(అక్షరం *ఉపసర్గ, char_u *name, పూర్ణాంకం type, char_u *string, పూర్ణాంకం *మొదటి);

/*
 * Initialize global and vim special variables
 */
    శూన్య
evalvars_init(శూన్య)
{
    పూర్ణాంకం		    i;
    నిర్మాణం vimvar   *p;

    init_var_dict(&globvardict, &globvars_var, VAR_DEF_SCOPE);
    init_var_dict(&vimvardict, &vimvars_var, VAR_SCOPE);
    vimvardict.dv_తాళం = VAR_FIXED;
    hash_init(&compat_hashtab);

    కొరకు (i = 0; i < VV_LEN; ++i)
    {
	p = &vimvars[i];
	ఒకవేళ (p->vv_flags & VV_RO)
	    p->vv_di.di_flags = DI_FLAGS_RO | DI_FLAGS_FIX;
	లేకపోతే ఒకవేళ (p->vv_flags & VV_RO_SBX)
	    p->vv_di.di_flags = DI_FLAGS_RO_SBX | DI_FLAGS_FIX;
	లేకపోతే
	    p->vv_di.di_flags = DI_FLAGS_FIX;

	// add to v: scope dict, unless the value is not always available
	ఒకవేళ (p->vv_tv_type != VAR_UNKNOWN)
	    hash_కూడు(&vimvarht, p->vv_di.di_కీలకం, "initialization");
	ఒకవేళ (p->vv_flags & VV_COMPAT)
	    // add to compat scope dict
	    hash_కూడు(&compat_hashtab, p->vv_di.di_కీలకం, "initialization");
    }
    సమితి_vim_var_nr(VV_సంస్కరణ, VIM_సంస్కరణ_100);
    సమితి_vim_var_nr(VV_VERSIONLONG, VIM_సంస్కరణ_100 * 10000 + highest_patch());

    సమితి_vim_var_nr(VV_SEARCHFORWARD, 1L);
    సమితి_vim_var_nr(VV_HLSEARCH, 1L);
    సమితి_vim_var_nr(VV_EXITING, VVAL_NULL);
    సమితి_vim_var_dict(VV_COMPLETED_అంశం, dict_alloc_తాళం(VAR_FIXED));
    సమితి_vim_var_list(VV_ERRORS, list_alloc());
    సమితి_vim_var_dict(VV_EVENT, dict_alloc_తాళం(VAR_FIXED));

    సమితి_vim_var_nr(VV_FALSE, VVAL_FALSE);
    సమితి_vim_var_nr(VV_TRUE, VVAL_TRUE);
    సమితి_vim_var_nr(VV_NONE, VVAL_NONE);
    సమితి_vim_var_nr(VV_NULL, VVAL_NULL);
    సమితి_vim_var_nr(VV_NUMBERMAX, VARNUM_MAX);
    సమితి_vim_var_nr(VV_NUMBERMIN, VARNUM_MIN);
    సమితి_vim_var_nr(VV_NUMBERSIZE, పరిమాణం(varnumber_T) * 8);
    సమితి_vim_var_nr(VV_SIZEOFINT, పరిమాణం(పూర్ణాంకం));
    సమితి_vim_var_nr(VV_SIZEOFLONG, పరిమాణం(దీర్ఘ));
    సమితి_vim_var_nr(VV_SIZEOFPOINTER, పరిమాణం(అక్షరం *));
    సమితి_vim_var_nr(VV_MAXCOL, MAXCOL);

    సమితి_vim_var_nr(VV_TYPE_సంఖ్య,  VAR_TYPE_సంఖ్య);
    సమితి_vim_var_nr(VV_TYPE_STRING,  VAR_TYPE_STRING);
    సమితి_vim_var_nr(VV_TYPE_FUNC,    VAR_TYPE_FUNC);
    సమితి_vim_var_nr(VV_TYPE_LIST,    VAR_TYPE_LIST);
    సమితి_vim_var_nr(VV_TYPE_DICT,    VAR_TYPE_DICT);
    సమితి_vim_var_nr(VV_TYPE_FLOAT,   VAR_TYPE_FLOAT);
    సమితి_vim_var_nr(VV_TYPE_BOOL,    VAR_TYPE_BOOL);
    సమితి_vim_var_nr(VV_TYPE_NONE,    VAR_TYPE_NONE);
    సమితి_vim_var_nr(VV_TYPE_పని,     VAR_TYPE_పని);
    సమితి_vim_var_nr(VV_TYPE_CHANNEL, VAR_TYPE_CHANNEL);
    సమితి_vim_var_nr(VV_TYPE_BLOB,    VAR_TYPE_BLOB);
    సమితి_vim_var_nr(VV_TYPE_CLASS,   VAR_TYPE_CLASS);
    సమితి_vim_var_nr(VV_TYPE_OBJECT,  VAR_TYPE_OBJECT);
    సమితి_vim_var_nr(VV_TYPE_TYPEALIAS,  VAR_TYPE_TYPEALIAS);
    సమితి_vim_var_nr(VV_TYPE_ENUM,    VAR_TYPE_ENUM);
    సమితి_vim_var_nr(VV_TYPE_ENUMVALUE,  VAR_TYPE_ENUMVALUE);
    సమితి_vim_var_nr(VV_TYPE_TUPLE,   VAR_TYPE_TUPLE);

    సమితి_vim_var_nr(VV_ECHOSPACE,    sc_నిలువు - 1);

    సమితి_vim_var_dict(VV_COLORNAMES, dict_alloc());
    సమితి_vim_var_dict(VV_CLIPPROVIDERS, dict_alloc());

#నిర్వచితమైతే FEAT_PYTHON3
    సమితి_vim_var_nr(VV_PYTHON3_సంస్కరణ, python3_సంస్కరణ());
#అయితే_ముగింపు

    // Default for v:register is not 0 but '"'.  This is adjusted once the
    // clipboard has been setup by calling reset_reg_var().
    సమితి_reg_var(0);
}

#ఒకవేళ defined(EXITFREE)
/*
 * Free all vim variables information on exit
 */
    శూన్య
evalvars_clear(శూన్య)
{
    పూర్ణాంకం		    i;
    నిర్మాణం vimvar   *p;

    కొరకు (i = 0; i < VV_LEN; ++i)
    {
	p = &vimvars[i];
	ఒకవేళ (p->vv_di.di_tv.v_type == VAR_STRING)
	    VIM_CLEAR(p->vv_str);
	లేకపోతే ఒకవేళ (p->vv_di.di_tv.v_type == VAR_LIST)
	{
	    list_unref(p->vv_list);
	    p->vv_list = ఖాళీ;
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
    కొరకు (i = 1; i <= script_వస్తువులు.ga_len; ++i)
	vars_clear(&SCRIPT_VARS(i));
}
#అయితే_ముగింపు

    పూర్ణాంకం
garbage_collect_globvars(పూర్ణాంకం copyID)
{
    తిరిగి సమితి_ref_లోపల_ht(&globvarht, copyID, ఖాళీ, ఖాళీ);
}

    పూర్ణాంకం
garbage_collect_vimvars(పూర్ణాంకం copyID)
{
    తిరిగి సమితి_ref_లోపల_ht(&vimvarht, copyID, ఖాళీ, ఖాళీ);
}

    పూర్ణాంకం
garbage_collect_scriptvars(పూర్ణాంకం copyID)
{
    పూర్ణాంకం		    i;
    పూర్ణాంకం		    idx;
    పూర్ణాంకం		    abort = FALSE;
    scriptitem_T    *si;

    కొరకు (i = 1; i <= script_వస్తువులు.ga_len; ++i)
    {
	abort = abort || సమితి_ref_లోపల_ht(&SCRIPT_VARS(i), copyID, ఖాళీ, ఖాళీ);

	si = SCRIPT_అంశం(i);
	కొరకు (idx = 0; idx < si->sn_var_vals.ga_len; ++idx)
	{
	    svar_T    *sv = ((svar_T *)si->sn_var_vals.ga_దత్తాంశం) + idx;

	    ఒకవేళ (sv->sv_name != ఖాళీ)
		abort = abort || సమితి_ref_లోపల_అంశం(sv->sv_tv, copyID, ఖాళీ, ఖాళీ, ఖాళీ);
	}
    }

    తిరిగి abort;
}

/*
 * Set an internal variable to a string value. Creates the variable if it does
 * not already exist.
 */
    శూన్య
సమితి_అంతర్గత_string_var(char_u *name, char_u *విలువ)
{
    char_u	*val;
    typval_T	*tvp;

    val = vim_strsave(విలువ);
    ఒకవేళ (val == ఖాళీ)
	తిరిగి;

    tvp = alloc_string_tv(val);
    ఒకవేళ (tvp == ఖాళీ)
	తిరిగి;

    సమితి_var(name, tvp, FALSE);
    free_tv(tvp);
}

    పూర్ణాంకం
eval_charconvert(
    char_u	*enc_from,
    char_u	*enc_గమ్యం,
    char_u	*fname_from,
    char_u	*fname_గమ్యం)
{
    పూర్ణాంకం		err = FALSE;
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;

    సమితి_vim_var_string(VV_CC_FROM, enc_from, -1);
    సమితి_vim_var_string(VV_CC_గమ్యం, enc_గమ్యం, -1);
    సమితి_vim_var_string(VV_FNAME_లోపల, fname_from, -1);
    సమితి_vim_var_string(VV_FNAME_బహిర్గతం, fname_గమ్యం, -1);
    ctx = get_option_sctx("charconvert");
    ఒకవేళ (ctx != ఖాళీ)
	current_sctx = *ctx;

    ఒకవేళ (eval_గమ్యం_bool(p_ccv, &err, ఖాళీ, FALSE, TRUE))
	err = TRUE;

    సమితి_vim_var_string(VV_CC_FROM, ఖాళీ, -1);
    సమితి_vim_var_string(VV_CC_గమ్యం, ఖాళీ, -1);
    సమితి_vim_var_string(VV_FNAME_లోపల, ఖాళీ, -1);
    సమితి_vim_var_string(VV_FNAME_బహిర్గతం, ఖాళీ, -1);
    current_sctx = saved_sctx;

    ఒకవేళ (err)
	తిరిగి FAIL;
    తిరిగి OK;
}

#ఒకవేళ defined(FEAT_POSTSCRIPT) || defined(FEAT_PRINT_PANGO)
    పూర్ణాంకం
eval_printexpr(char_u *fname, char_u *వాదనలు)
{
    పూర్ణాంకం		err = FALSE;
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;

    సమితి_vim_var_string(VV_FNAME_లోపల, fname, -1);
    సమితి_vim_var_string(VV_CMDARG, వాదనలు, -1);
    ctx = get_option_sctx("printexpr");
    ఒకవేళ (ctx != ఖాళీ)
	current_sctx = *ctx;

    ఒకవేళ (eval_గమ్యం_bool(p_pexpr, &err, ఖాళీ, FALSE, TRUE))
	err = TRUE;

    సమితి_vim_var_string(VV_FNAME_లోపల, ఖాళీ, -1);
    సమితి_vim_var_string(VV_CMDARG, ఖాళీ, -1);
    current_sctx = saved_sctx;

    ఒకవేళ (err)
    {
	mch_remove(fname);
	తిరిగి FAIL;
    }
    తిరిగి OK;
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_DIFF)
    శూన్య
eval_diff(
    char_u	*origfile,
    char_u	*newfile,
    char_u	*outfile)
{
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;
    typval_T	*tv;

    సమితి_vim_var_string(VV_FNAME_లోపల, origfile, -1);
    సమితి_vim_var_string(VV_FNAME_NEW, newfile, -1);
    సమితి_vim_var_string(VV_FNAME_బహిర్గతం, outfile, -1);

    ctx = get_option_sctx("diffexpr");
    ఒకవేళ (ctx != ఖాళీ)
	current_sctx = *ctx;

    // errors are ignored
    tv = eval_సమాసం_ext(p_dex, ఖాళీ, TRUE);
    free_tv(tv);

    సమితి_vim_var_string(VV_FNAME_లోపల, ఖాళీ, -1);
    సమితి_vim_var_string(VV_FNAME_NEW, ఖాళీ, -1);
    సమితి_vim_var_string(VV_FNAME_బహిర్గతం, ఖాళీ, -1);
    current_sctx = saved_sctx;
}

    శూన్య
eval_patch(
    char_u	*origfile,
    char_u	*difffile,
    char_u	*outfile)
{
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;
    typval_T	*tv;

    సమితి_vim_var_string(VV_FNAME_లోపల, origfile, -1);
    సమితి_vim_var_string(VV_FNAME_DIFF, difffile, -1);
    సమితి_vim_var_string(VV_FNAME_బహిర్గతం, outfile, -1);

    ctx = get_option_sctx("patchexpr");
    ఒకవేళ (ctx != ఖాళీ)
	current_sctx = *ctx;

    // errors are ignored
    tv = eval_సమాసం_ext(p_pex, ఖాళీ, TRUE);
    free_tv(tv);

    సమితి_vim_var_string(VV_FNAME_లోపల, ఖాళీ, -1);
    సమితి_vim_var_string(VV_FNAME_DIFF, ఖాళీ, -1);
    సమితి_vim_var_string(VV_FNAME_బహిర్గతం, ఖాళీ, -1);
    current_sctx = saved_sctx;
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_SPELL)
/*
 * Evaluate an expression to a list with suggestions.
 * For the "expr:" part of 'spellsuggest'.
 * Returns NULL when there is an error.
 */
    list_T *
eval_spell_సమాసం(char_u *badword, char_u *సమాసం)
{
    typval_T	save_val;
    typval_T	rettv;
    list_T	*list = ఖాళీ;
    char_u	*p = skipwhite(సమాసం);
    sctx_T	saved_sctx = current_sctx;
    sctx_T	*ctx;
    పూర్ణాంకం		r;

    // Set "v:val" to the bad word.
    prepare_vimvar(VV_VAL, &save_val);
    సమితి_vim_var_string(VV_VAL, badword, -1);
    ఒకవేళ (p_verbose == 0)
	++emsg_off;
    ctx = get_option_sctx("spellsuggest");
    ఒకవేళ (ctx != ఖాళీ)
	current_sctx = *ctx;

    r = may_call_simple_func(p, &rettv);
    ఒకవేళ (r == NOTDONE)
	r = eval1(&p, &rettv, &EVALARG_EVALUATE);
    ఒకవేళ (r == OK)
    {
	ఒకవేళ (rettv.v_type != VAR_LIST)
	    clear_tv(&rettv);
	లేకపోతే
	    list = rettv.vval.v_list;
    }

    ఒకవేళ (p_verbose == 0)
	--emsg_off;
    clear_tv(get_vim_var_tv(VV_VAL));
    restore_vimvar(VV_VAL, &save_val);
    current_sctx = saved_sctx;

    తిరిగి list;
}

/*
 * "list" is supposed to contain two items: a word and a number.  Return the
 * word in "pp" and the number as the return value.
 * Return -1 if anything isn't right.
 * Used to get the good word and score from the eval_spell_expr() result.
 */
    పూర్ణాంకం
get_spellword(list_T *list, char_u **pp)
{
    listitem_T	*li;

    li = list->lv_మొదటి;
    ఒకవేళ (li == ఖాళీ)
	తిరిగి -1;
    *pp = tv_get_string(&li->li_tv);

    li = li->li_తదుపరి;
    ఒకవేళ (li == ఖాళీ)
	తిరిగి -1;
    తిరిగి (పూర్ణాంకం)tv_get_సంఖ్య(&li->li_tv);
}
#అయితే_ముగింపు

/*
 * Prepare v: variable "idx" to be used.
 * Save the current typeval in "save_tv" and clear it.
 * When not used yet add the variable to the v: hashtable.
 */
    శూన్య
prepare_vimvar(పూర్ణాంకం idx, typval_T *save_tv)
{
    *save_tv = vimvars[idx].vv_tv;
    vimvars[idx].vv_str = ఖాళీ;  // don't free it now
    ఒకవేళ (vimvars[idx].vv_tv_type == VAR_UNKNOWN)
	hash_కూడు(&vimvarht, vimvars[idx].vv_di.di_కీలకం, "prepare vimvar");
}

/*
 * Restore v: variable "idx" to typeval "save_tv".
 * Note that the v: variable must have been cleared already.
 * When no longer defined, remove the variable from the v: hashtable.
 */
    శూన్య
restore_vimvar(పూర్ణాంకం idx, typval_T *save_tv)
{
    hashitem_T	*hi;

    vimvars[idx].vv_tv = *save_tv;
    ఒకవేళ (vimvars[idx].vv_tv_type != VAR_UNKNOWN)
	తిరిగి;

    hi = hash_కనుగొను(&vimvarht, vimvars[idx].vv_di.di_కీలకం);
    ఒకవేళ (HASHITEM_ఖాళీలు(hi))
	అంతర్గత_error("restore_vimvar()");
    లేకపోతే
	hash_remove(&vimvarht, hi, "restore vimvar");
}

/*
 * List Vim variables.
 */
    స్థిర శూన్య
list_vim_vars(పూర్ణాంకం *మొదటి)
{
    list_hashtable_vars(&vimvarht, "v:", FALSE, మొదటి);
}

/*
 * List script-local variables, if there is a script.
 */
    స్థిర శూన్య
list_script_vars(పూర్ణాంకం *మొదటి)
{
    ఒకవేళ (SCRIPT_ID_VALID(current_sctx.sc_sid))
	list_hashtable_vars(&SCRIPT_VARS(current_sctx.sc_sid),
							   "s:", FALSE, మొదటి);
}

/*
 * Return TRUE if "name" starts with "g:", "w:", "t:" or "b:".
 * But only when an identifier character follows.
 */
    పూర్ణాంకం
is_scoped_variable(char_u *name)
{
    తిరిగి vim_strchr((char_u *)"gwbt", name[0]) != ఖాళీ
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
eval_one_సమాసం_లోపల_str(char_u *p, garray_T *gap, పూర్ణాంకం evaluate)
{
    char_u	*ఖండం_ప్రారంభం = skipwhite(p + 1);  // skip the opening {
    char_u	*ఖండం_చివర = ఖండం_ప్రారంభం;
    char_u	*సమాసం_val;

    ఒకవేళ (*ఖండం_ప్రారంభం == NUL)
    {
	semsg(_(e_missing_close_curly_str), p);
	తిరిగి ఖాళీ;
    }
    ఒకవేళ (skip_సమాసం(&ఖండం_చివర, ఖాళీ) == FAIL)
	తిరిగి ఖాళీ;
    ఖండం_చివర = skipwhite(ఖండం_చివర);
    ఒకవేళ (*ఖండం_చివర != '}')
    {
	semsg(_(e_missing_close_curly_str), p);
	తిరిగి ఖాళీ;
    }
    ఒకవేళ (evaluate)
    {
	*ఖండం_చివర = NUL;
	సమాసం_val = eval_గమ్యం_string(ఖండం_ప్రారంభం, FALSE, FALSE);
	*ఖండం_చివర = '}';
	ఒకవేళ (సమాసం_val == ఖాళీ)
	    తిరిగి ఖాళీ;
	ga_concat(gap, సమాసం_val);
	vim_free(సమాసం_val);
    }

    తిరిగి ఖండం_చివర + 1;
}

/*
 * Evaluate all the Vim expressions {expr} in "str" and return the resulting
 * string in allocated memory.  "{{" is reduced to "{" and "}}" to "}".
 * Used for a heredoc assignment.
 * Returns NULL for an error.
 */
    స్థిర char_u *
eval_all_సమాసం_లోపల_str(char_u *str)
{
    garray_T	ga;
    char_u	*p;

    ga_init2(&ga, 1, 80);
    p = str;

    అయితే (*p != NUL)
    {
	char_u	*lit_ప్రారంభం;
	పూర్ణాంకం	escaped_brace = FALSE;

	// Look for a block start.
	lit_ప్రారంభం = p;
	అయితే (*p != '{' && *p != '}' && *p != NUL)
	    ++p;

	ఒకవేళ (*p != NUL && *p == p[1])
	{
	    // Escaped brace, unescape and continue.
	    // Include the brace in the literal string.
	    ++p;
	    escaped_brace = TRUE;
	}
	లేకపోతే ఒకవేళ (*p == '}')
	{
	    semsg(_(e_stray_closing_curly_str), str);
	    ga_clear(&ga);
	    తిరిగి ఖాళీ;
	}

	// Append the literal part.
	ga_concat_len(&ga, lit_ప్రారంభం, (size_t)(p - lit_ప్రారంభం));

	ఒకవేళ (*p == NUL)
	    ఆపు;

	ఒకవేళ (escaped_brace)
	{
	    // Skip the second brace.
	    ++p;
	    కొనసాగు;
	}

	// Evaluate the expression and append the result.
	p = eval_one_సమాసం_లోపల_str(p, &ga, TRUE);
	ఒకవేళ (p == ఖాళీ)
	{
	    ga_clear(&ga);
	    తిరిగి ఖాళీ;
	}
    }
    ga_append(&ga, NUL);

    తిరిగి ga.ga_దత్తాంశం;
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
heredoc_get(exarg_T *eap, char_u *cmd, పూర్ణాంకం script_get, పూర్ణాంకం vim9compile)
{
    char_u	*theline = ఖాళీ;
    char_u	*marker;
    list_T	*l;
    char_u	*p;
    char_u	*str;
    పూర్ణాంకం		marker_indent_len = 0;
    పూర్ణాంకం		పాఠ్యం_indent_len = 0;
    char_u	*పాఠ్యం_indent = ఖాళీ;
    char_u	dot[] = ".";
    పూర్ణాంకం		comment_char = లోపల_vim9script() ? '#' : '"';
    పూర్ణాంకం		evalstr = FALSE;
    పూర్ణాంకం		eval_failed = FALSE;
    cctx_T	*cctx = vim9compile ? eap->cookie : ఖాళీ;
    పూర్ణాంకం		లెక్క = 0;
    పూర్ణాంకం		heredoc_లోపల_string = FALSE;
    char_u	*పంక్తి_arg = ఖాళీ;
    char_u	*nl_ptr = vim_strchr(cmd, '\n');

    ఒకవేళ (nl_ptr != ఖాళీ)
    {
	heredoc_లోపల_string = TRUE;
	పంక్తి_arg = nl_ptr + 1;
	*nl_ptr = NUL;
    }
    లేకపోతే ఒకవేళ (eap->ea_getline == ఖాళీ)
    {
	emsg(_(e_cannot_use_heredoc_ఇక్కడ));
	తిరిగి ఖాళీ;
    }

    // Check for the optional 'trim' word before the marker
    cmd = skipwhite(cmd);

    అయితే (TRUE)
    {
	ఒకవేళ (STRNCMP(cmd, "trim", 4) == 0
		&& (cmd[4] == NUL || VIM_ISWHITE(cmd[4])))
	{
	    cmd = skipwhite(cmd + 4);

	    // Trim the indentation from all the lines in the here document.
	    // The amount of indentation trimmed is the same as the indentation
	    // of the first line after the :let command line.  To find the end
	    // marker the indent of the :let command line is trimmed.
	    p = *eap->cmdlinep;
	    అయితే (VIM_ISWHITE(*p))
	    {
		p++;
		marker_indent_len++;
	    }
	    పాఠ్యం_indent_len = -1;

	    కొనసాగు;
	}
	ఒకవేళ (STRNCMP(cmd, "eval", 4) == 0
		&& (cmd[4] == NUL || VIM_ISWHITE(cmd[4])))
	{
	    cmd = skipwhite(cmd + 4);
	    evalstr = TRUE;
	    కొనసాగు;
	}
	ఆపు;
    }

    // The marker is the next word.
    ఒకవేళ (*cmd != NUL && *cmd != comment_char)
    {
	marker = skipwhite(cmd);
	p = skiptowhite(marker);
	ఒకవేళ (*skipwhite(p) != NUL && *skipwhite(p) != comment_char)
	{
	    semsg(_(e_trailing_characters_str), p);
	    తిరిగి ఖాళీ;
	}
	*p = NUL;
	ఒకవేళ (!script_get && vim_islower(*marker))
	{
	    emsg(_(e_marker_cannot_ప్రారంభం_with_క్రింది_case_letter));
	    తిరిగి ఖాళీ;
	}
    }
    లేకపోతే
    {
	// When getting lines for an embedded script, if the marker is missing,
	// accept '.' as the marker.
	ఒకవేళ (script_get)
	    marker = dot;
	లేకపోతే
	{
	    emsg(_(e_missing_marker));
	    తిరిగి ఖాళీ;
	}
    }

    l = list_alloc();
    ఒకవేళ (l == ఖాళీ)
	తిరిగి ఖాళీ;

    కొరకు (;;)
    {
	పూర్ణాంకం	mi = 0;
	పూర్ణాంకం	ti = 0;

	ఒకవేళ (heredoc_లోపల_string)
	{
	    char_u	*తదుపరి_పంక్తి;

	    // heredoc in a string separated by newlines.  Get the next line
	    // from the string.

	    ఒకవేళ (*పంక్తి_arg == NUL)
	    {
		semsg(_(e_missing_చివర_marker_str), marker);
		ఆపు;
	    }

	    theline = పంక్తి_arg;
	    తదుపరి_పంక్తి = vim_strchr(theline, '\n');
	    ఒకవేళ (తదుపరి_పంక్తి == ఖాళీ)
		పంక్తి_arg += STRLEN(పంక్తి_arg);
	    లేకపోతే
	    {
		*తదుపరి_పంక్తి = NUL;
		పంక్తి_arg = తదుపరి_పంక్తి + 1;
	    }
	}
	లేకపోతే
	{
	    vim_free(theline);
	    theline = eap->ea_getline(NUL, eap->cookie, 0, FALSE);
	    ఒకవేళ (theline == ఖాళీ)
	    {
		semsg(_(e_missing_చివర_marker_str), marker);
		ఆపు;
	    }
	}

	// with "trim": skip the indent matching the :let line to find the
	// marker
	ఒకవేళ (marker_indent_len > 0
		&& STRNCMP(theline, *eap->cmdlinep, marker_indent_len) == 0)
	    mi = marker_indent_len;
	ఒకవేళ (STRCMP(marker, theline + mi) == 0)
	    ఆపు;

	// If expression evaluation failed in the heredoc, then skip till the
	// end marker.
	ఒకవేళ (eval_failed)
	    కొనసాగు;

	ఒకవేళ (పాఠ్యం_indent_len == -1 && *theline != NUL)
	{
	    // set the text indent from the first line.
	    p = theline;
	    పాఠ్యం_indent_len = 0;
	    అయితే (VIM_ISWHITE(*p))
	    {
		p++;
		పాఠ్యం_indent_len++;
	    }
	    పాఠ్యం_indent = vim_strnsave(theline, పాఠ్యం_indent_len);
	}
	// with "trim": skip the indent matching the first line
	ఒకవేళ (పాఠ్యం_indent != ఖాళీ)
	    కొరకు (ti = 0; ti < పాఠ్యం_indent_len; ++ti)
		ఒకవేళ (theline[ti] != పాఠ్యం_indent[ti])
		    ఆపు;

	str = theline + ti;
	ఒకవేళ (vim9compile)
	{
	    ఒకవేళ (compile_all_సమాసం_లోపల_str(str, evalstr, cctx) == FAIL)
	    {
		vim_free(theline);
		vim_free(పాఠ్యం_indent);
		list_free(l);
		తిరిగి FAIL;
	    }
	    లెక్క++;
	}
	లేకపోతే
	{
	    పూర్ణాంకం	    free_str = FALSE;

	    ఒకవేళ (evalstr && !eap->skip)
	    {
		str = eval_all_సమాసం_లోపల_str(str);
		ఒకవేళ (str == ఖాళీ)
		{
		    // expression evaluation failed
		    eval_failed = TRUE;
		    కొనసాగు;
		}
		free_str = TRUE;
	    }

	    ఒకవేళ (list_append_string(l, str, -1) == FAIL)
	    {
		ఒకవేళ (free_str)
		    vim_free(str);
		ఆపు;
	    }
	    ఒకవేళ (free_str)
		vim_free(str);
	}
    }
    ఒకవేళ (heredoc_లోపల_string)
	// Next command follows the heredoc in the string.
	eap->nextcmd = పంక్తి_arg;
    లేకపోతే
	vim_free(theline);
    vim_free(పాఠ్యం_indent);

    ఒకవేళ (vim9compile && cctx->ctx_skip != SKIP_YES && !eval_failed)
	generate_NEWLIST(cctx, లెక్క, FALSE);

    ఒకవేళ (eval_failed)
    {
	// expression evaluation in the heredoc failed
	list_free(l);
	తిరిగి ఖాళీ;
    }
    తిరిగి l;
}

/*
 * Vim9 variable declaration:
 * ":var name"
 * ":var name: type"
 * ":var name = expr"
 * ":var name: type = expr"
 * etc.
 */
    శూన్య
ex_var(exarg_T *eap)
{
    char_u *p = eap->cmd;
    పూర్ణాంకం	    has_var;

    ఒకవేళ (!లోపల_vim9script())
    {
	semsg(_(e_str_cannot_be_వాడిన_లోపల_legacy_vim_script), ":var");
	తిరిగి;
    }
    has_var = checkforcmd_noparen(&p, "var", 3);
    ఒకవేళ (current_sctx.sc_sid == 0 && has_var)
    {
	emsg(_(e_cannot_declare_variable_on_command_పంక్తి));
	తిరిగి;
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
    శూన్య
ex_let(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    char_u	*సమాసం = ఖాళీ;
    typval_T	rettv;
    పూర్ణాంకం		var_లెక్క = 0;
    పూర్ణాంకం		semicolon = 0;
    char_u	op[4];
    char_u	*argend;
    పూర్ణాంకం		మొదటి = TRUE;
    పూర్ణాంకం		concat;
    పూర్ణాంకం		has_కేటాయించు;
    పూర్ణాంకం		flags = 0;
    పూర్ణాంకం		vim9script = లోపల_vim9script();

    ఒకవేళ (eap->cmdidx == CMD_final && !vim9script)
    {
	// In legacy Vim script ":final" is short for ":finally".
	ex_finally(eap);
	తిరిగి;
    }
    ఒకవేళ (eap->cmdidx == CMD_let && vim9script)
    {
	emsg(_(e_cannot_use_let_లోపల_vim9_script));
	తిరిగి;
    }

    ఒకవేళ (eap->cmdidx == CMD_const)
	flags |= కేటాయించు_CONST;
    లేకపోతే ఒకవేళ (eap->cmdidx == CMD_final)
	flags |= కేటాయించు_FINAL;

    // Vim9 assignment without ":let", ":const" or ":final"
    ఒకవేళ (eap->arg == eap->cmd)
	flags |= కేటాయించు_NO_DECL;

    argend = skip_var_list(arg, TRUE, &var_లెక్క, &semicolon, FALSE);
    ఒకవేళ (argend == ఖాళీ)
	తిరిగి;
    సమాసం = skipwhite(argend);
    concat = సమాసం[0] == '.'
	&& ((సమాసం[1] == '=' && లోపల_old_script(2))
		|| (సమాసం[1] == '.' && సమాసం[2] == '='));
    has_కేటాయించు =  *సమాసం == '=' || (vim_strchr((char_u *)"+-*/%", *సమాసం) != ఖాళీ
							    && సమాసం[1] == '=');
    ఒకవేళ (!has_కేటాయించు && !concat)
    {
	// ":let" without "=": list variables
	ఒకవేళ (*arg == '[')
	    emsg(_(e_invalid_వాదం));
	లేకపోతే ఒకవేళ (సమాసం[0] == '.' && సమాసం[1] == '=')
	    emsg(_(e_dot_equal_not_supported_with_script_సంస్కరణ_two));
	లేకపోతే ఒకవేళ (!చివర్లు_excmd2(eap->cmd, arg))
	{
	    ఒకవేళ (vim9script)
	    {
		ఒకవేళ (!చివర్లు_excmd2(eap->cmd, skipwhite(argend)))
		    semsg(_(e_trailing_characters_str), argend);
		లేకపోతే
		    // Vim9 declaration ":var name: type"
		    arg = vim9_declare_scriptvar(eap, arg);
	    }
	    లేకపోతే
	    {
		// ":let var1 var2" - list values
		arg = list_arg_vars(eap, arg, &మొదటి);
	    }
	}
	లేకపోతే ఒకవేళ (!eap->skip)
	{
	    // ":let"
	    list_glob_vars(&మొదటి);
	    list_buf_vars(&మొదటి);
	    list_win_vars(&మొదటి);
	    list_tab_vars(&మొదటి);
	    list_script_vars(&మొదటి);
	    list_func_vars(&మొదటి);
	    list_vim_vars(&మొదటి);
	}
	సమితి_nextcmd(eap, arg);
	తిరిగి;
    }

    ఒకవేళ (సమాసం[0] == '=' && సమాసం[1] == '<' && సమాసం[2] == '<')
    {
	list_T	*l = ఖాళీ;
	దీర్ఘ	cur_lnum = SOURCING_LNUM;

	// :let text =<< [trim] [eval] END
	// :var text =<< [trim] [eval] END
	ఒకవేళ (vim9script && !eap->skip && (!VIM_ISWHITE(సమాసం[-1])
						 || !IS_WHITE_OR_NUL(సమాసం[3])))
	    semsg(_(e_white_space_required_before_and_after_str_at_str),
								  "=<<", సమాసం);
	లేకపోతే
	    l = heredoc_get(eap, సమాసం + 3, FALSE, FALSE);

	ఒకవేళ (l != ఖాళీ)
	{
	    rettv_list_సమితి(&rettv, l);
	    ఒకవేళ (!eap->skip)
	    {
		// errors are for the assignment, not the end marker
		SOURCING_LNUM = cur_lnum;
		op[0] = '=';
		op[1] = NUL;
		(శూన్య)ex_let_vars(eap->arg, &rettv, FALSE, semicolon, var_లెక్క,
								flags, op);
	    }
	    clear_tv(&rettv);
	}
	తిరిగి;
    }

    evalarg_T   evalarg;
    పూర్ణాంకం	    len = 1;

    CLEAR_క్షేత్రం(rettv);

    పూర్ణాంకం cur_lnum;

    op[0] = '=';
    op[1] = NUL;
    ఒకవేళ (*సమాసం != '=')
    {
	ఒకవేళ (vim9script && (flags & కేటాయించు_NO_DECL) == 0)
	{
	    // +=, /=, etc. require an existing variable
	    semsg(_(e_cannot_use_operator_on_new_variable_str), eap->arg);
	}
	లేకపోతే ఒకవేళ (vim_strchr((char_u *)"+-*/%.", *సమాసం) != ఖాళీ)
	{
	    op[0] = *సమాసం;   // +=, -=, *=, /=, %= or .=
	    ++len;
	    ఒకవేళ (సమాసం[0] == '.' && సమాసం[1] == '.') // ..=
	    {
		++సమాసం;
		++len;
	    }
	}
	సమాసం += 2;
    }
    లేకపోతే
	++సమాసం;

    ఒకవేళ (vim9script && !eap->skip && (!VIM_ISWHITE(*argend)
					       || !IS_WHITE_OR_NUL(*సమాసం)))
    {
	vim_strncpy(op, సమాసం - len, len);
	semsg(_(e_white_space_required_before_and_after_str_at_str),
							       op, argend);
    }

    ఒకవేళ (eap->skip)
	++emsg_skip;
    fill_evalarg_from_eap(&evalarg, eap, eap->skip);
    సమాసం = skipwhite_and_linebreak(సమాసం, &evalarg);
    cur_lnum = SOURCING_LNUM;
    పూర్ణాంకం eval_res = eval0(సమాసం, &rettv, eap, &evalarg);
    ఒకవేళ (eap->skip)
	--emsg_skip;
    clear_evalarg(&evalarg, eap);

    // Restore the line number so that any type error is given for the
    // declaration, not the expression.
    SOURCING_LNUM = cur_lnum;

    ఒకవేళ (!eap->skip && eval_res != FAIL)
	(శూన్య)ex_let_vars(eap->arg, &rettv, FALSE, semicolon, var_లెక్క,
								    flags, op);
    ఒకవేళ (eval_res != FAIL)
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
    పూర్ణాంకం
ex_let_vars(
    char_u	*arg_ప్రారంభం,
    typval_T	*tv,
    పూర్ణాంకం		copy,		// copy values from "tv", don't move
    పూర్ణాంకం		semicolon,	// from skip_var_list()
    పూర్ణాంకం		var_లెక్క,	// from skip_var_list()
    పూర్ణాంకం		flags,		// ASSIGN_FINAL, ASSIGN_CONST, etc.
    char_u	*op)
{
    char_u	*arg = arg_ప్రారంభం;
    list_T	*l;
    tuple_T	*tuple = ఖాళీ;
    పూర్ణాంకం		i;
    పూర్ణాంకం		var_idx = 0;
    listitem_T	*అంశం = ఖాళీ;
    typval_T	ltv;
    పూర్ణాంకం		is_list = tv->v_type == VAR_LIST;
    పూర్ణాంకం		idx;

    ఒకవేళ (tv->v_type == VAR_VOID)
    {
	emsg(_(e_cannot_use_void_విలువ));
	తిరిగి FAIL;
    }
    ఒకవేళ (*arg != '[')
    {
	// ":let var = expr" or ":for var in list"
	ఒకవేళ (ex_let_one(arg, tv, copy, flags, op, op, var_idx) == ఖాళీ)
	    తిరిగి FAIL;
	తిరిగి OK;
    }

    // ":let [v1, v2] = list" or ":for [v1, v2] in listlist"
    // or
    // ":let [v1, v2] = tuple" or ":for [v1, v2] in tupletuple"
    ఒకవేళ (tv->v_type != VAR_LIST && tv->v_type != VAR_TUPLE)
    {
	emsg(_(e_list_or_tuple_required));
	తిరిగి FAIL;
    }
    ఒకవేళ (is_list)
    {
	l = tv->vval.v_list;
	ఒకవేళ (l == ఖాళీ)
	{
	    emsg(_(e_list_required));
	    తిరిగి FAIL;
	}
	i = list_len(l);
    }
    లేకపోతే
    {
	tuple = tv->vval.v_tuple;
	ఒకవేళ (tuple == ఖాళీ)
	{
	    emsg(_(e_tuple_required));
	    తిరిగి FAIL;
	}
	i = tuple_len(tuple);
    }

    ఒకవేళ (semicolon == 0 && var_లెక్క < i)
    {
	emsg(_(is_list ? e_less_targets_than_list_వస్తువులు
					: e_less_targets_than_tuple_వస్తువులు));
	తిరిగి FAIL;
    }
    ఒకవేళ (var_లెక్క - semicolon > i)
    {
	emsg(_(is_list ? e_మరిన్ని_targets_than_list_వస్తువులు
					: e_మరిన్ని_targets_than_tuple_వస్తువులు));
	తిరిగి FAIL;
    }

    ఒకవేళ (is_list)
    {
	CHECK_LIST_MATERIALIZE(l);
	అంశం = l->lv_మొదటి;
    }
    లేకపోతే
	idx = 0;

    అయితే (*arg != ']')
    {
	arg = skipwhite(arg + 1);
	++var_idx;
	arg = ex_let_one(arg, is_list ? &అంశం->li_tv : TUPLE_అంశం(tuple, idx),
			 TRUE, flags | కేటాయించు_UNPACK, (char_u *)",;]", op,
			 var_idx);
	ఒకవేళ (is_list)
	    అంశం = అంశం->li_తదుపరి;
	లేకపోతే
	    idx++;
	ఒకవేళ (arg == ఖాళీ)
	    తిరిగి FAIL;

	arg = skipwhite(arg);
	ఒకవేళ (*arg == ';')
	{
	    // Put the rest of the list or tuple (may be empty) in the var
	    // after ';'.  Create a new list or tuple for this.
	    ఒకవేళ (is_list)
	    {
		// Put the rest of the list (may be empty) in the var
		// after ';'.  Create a new list for this.
		l = list_alloc();
		ఒకవేళ (l == ఖాళీ)
		    తిరిగి FAIL;

		// list
		అయితే (అంశం != ఖాళీ)
		{
		    list_append_tv(l, &అంశం->li_tv);
		    అంశం = అంశం->li_తదుపరి;
		}

		ltv.v_type = VAR_LIST;
		ltv.v_తాళం = 0;
		ltv.vval.v_list = l;
		l->lv_refcount = 1;
	    }
	    లేకపోతే
	    {
		tuple_T	*new_tuple = tuple_alloc();
		ఒకవేళ (new_tuple == ఖాళీ)
		    తిరిగి FAIL;

		// Put the rest of the tuple (may be empty) in the var
		// after ';'.  Create a new tuple for this.
		అయితే (idx < TUPLE_LEN(tuple))
		{
		    typval_T    new_tv;

		    copy_tv(TUPLE_అంశం(tuple, idx), &new_tv);
		    ఒకవేళ (tuple_append_tv(new_tuple, &new_tv) == FAIL)
		    {
			clear_tv(&new_tv);
			tuple_unref(new_tuple);
			తిరిగి FAIL;
		    }
		    idx++;
		}

		ltv.v_type = VAR_TUPLE;
		ltv.v_తాళం = 0;
		ltv.vval.v_tuple = new_tuple;
		new_tuple->tv_refcount = 1;
	    }

	    ++var_idx;
	    arg = ex_let_one(skipwhite(arg + 1), &ltv, FALSE,
			flags | కేటాయించు_UNPACK, (char_u *)"]", op, var_idx);
	    clear_tv(&ltv);
	    ఒకవేళ (arg == ఖాళీ)
		తిరిగి FAIL;
	    ఆపు;
	}
	లేకపోతే ఒకవేళ (*arg != ',' && *arg != ']')
	{
	    అంతర్గత_error("ex_let_vars()");
	    తిరిగి FAIL;
	}
    }

    తిరిగి OK;
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
    పూర్ణాంకం		include_type,
    పూర్ణాంకం		*var_లెక్క,
    పూర్ణాంకం		*semicolon,
    పూర్ణాంకం		silent)
{
    char_u	*p, *s;

    ఒకవేళ (*arg == '[')
    {
	// "[var, var]": find the matching ']'.
	p = arg;
	కొరకు (;;)
	{
	    p = skipwhite(p + 1);	// skip whites after '[', ';' or ','
	    s = skip_var_one(p, include_type);
	    ఒకవేళ (s == p)
	    {
		ఒకవేళ (!silent)
		    semsg(_(e_invalid_వాదం_str), p);
		తిరిగి ఖాళీ;
	    }
	    ++*var_లెక్క;

	    p = skipwhite(s);
	    ఒకవేళ (*p == ']')
		ఆపు;
	    లేకపోతే ఒకవేళ (*p == ';')
	    {
		ఒకవేళ (*semicolon == 1)
		{
		    ఒకవేళ (!silent)
			emsg(_(e_double_semicolon_లోపల_list_యొక్క_variables));
		    తిరిగి ఖాళీ;
		}
		*semicolon = 1;
	    }
	    లేకపోతే ఒకవేళ (*p != ',')
	    {
		ఒకవేళ (!silent)
		    semsg(_(e_invalid_వాదం_str), p);
		తిరిగి ఖాళీ;
	    }
	}
	తిరిగి p + 1;
    }

    తిరిగి skip_var_one(arg, include_type);
}

/*
 * Skip one (assignable) variable name, including @r, $VAR, &option, d.key,
 * l[idx].
 * In Vim9 script also skip over ": type" if "include_type" is TRUE.
 */
    char_u *
skip_var_one(char_u *arg, పూర్ణాంకం include_type)
{
    char_u	*చివర;
    పూర్ణాంకం		vim9 = లోపల_vim9script();

    ఒకవేళ (*arg == '@' && arg[1] != NUL)
	తిరిగి arg + 2;

    // termcap option name may have non-alpha characters
    ఒకవేళ (STRNCMP(arg, "&t_", 3) == 0 && arg[3] != NUL && arg[4] != NUL)
	తిరిగి arg + 5;

    చివర = కనుగొను_name_చివర(*arg == '$' || *arg == '&' ? arg + 1 : arg,
				   ఖాళీ, ఖాళీ, FNE_INCL_BR | FNE_CHECK_ప్రారంభం);

    // "a: type" is declaring variable "a" with a type, not "a:".
    // Same for "s: type".
    ఒకవేళ (vim9 && చివర == arg + 2 && చివర[-1] == ':')
	--చివర;

    ఒకవేళ (include_type && vim9)
    {
	ఒకవేళ (*skipwhite(చివర) == ':')
	    చివర = skip_type(skipwhite(skipwhite(చివర) + 1), FALSE);
    }
    తిరిగి చివర;
}

/*
 * List variables for hashtab "ht" with prefix "prefix".
 * If "empty" is TRUE also list NULL strings as empty strings.
 */
    శూన్య
list_hashtable_vars(
    hashtab_T	*ht,
    అక్షరం	*ఉపసర్గ,
    పూర్ణాంకం		ఖాళీలు_2,
    పూర్ణాంకం		*మొదటి)
{
    hashitem_T	*hi;
    dictitem_T	*di;
    పూర్ణాంకం		todo;
    char_u	buf[IOSIZE];

    పూర్ణాంకం save_ht_flags = ht->ht_flags;
    ht->ht_flags |= HTFLAGS_FROZEN;

    todo = (పూర్ణాంకం)ht->ht_వాడిన;
    కొరకు (hi = ht->ht_array; todo > 0 && !got_int; ++hi)
    {
	ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
	{
	    --todo;
	    di = HI2DI(hi);

	    // apply :filter /pat/ to variable name
	    vim_strncpy((char_u *)buf, (char_u *)ఉపసర్గ, IOSIZE - 1);
	    vim_strcat((char_u *)buf, di->di_కీలకం, IOSIZE);
	    ఒకవేళ (సందేశం_filtered(buf))
		కొనసాగు;

	    ఒకవేళ (ఖాళీలు_2 || di->di_tv.v_type != VAR_STRING
					   || di->di_tv.vval.v_string != ఖాళీ)
		list_one_var(di, ఉపసర్గ, మొదటి);
	}
    }

    ht->ht_flags = save_ht_flags;
}

/*
 * List global variables.
 */
    స్థిర శూన్య
list_glob_vars(పూర్ణాంకం *మొదటి)
{
    list_hashtable_vars(&globvarht, "", TRUE, మొదటి);
}

/*
 * List buffer variables.
 */
    స్థిర శూన్య
list_buf_vars(పూర్ణాంకం *మొదటి)
{
    list_hashtable_vars(&curbuf->b_vars->dv_hashtab, "b:", TRUE, మొదటి);
}

/*
 * List window variables.
 */
    స్థిర శూన్య
list_win_vars(పూర్ణాంకం *మొదటి)
{
    list_hashtable_vars(&curwin->w_vars->dv_hashtab, "w:", TRUE, మొదటి);
}

/*
 * List tab page variables.
 */
    స్థిర శూన్య
list_tab_vars(పూర్ణాంకం *మొదటి)
{
    list_hashtable_vars(&curtab->tp_vars->dv_hashtab, "t:", TRUE, మొదటి);
}

/*
 * List variables in "arg".
 */
    స్థిర char_u *
list_arg_vars(exarg_T *eap, char_u *arg, పూర్ణాంకం *మొదటి)
{
    పూర్ణాంకం		error = FALSE;
    పూర్ణాంకం		len;
    char_u	*name;
    char_u	*name_ప్రారంభం;
    char_u	*arg_subsc;
    char_u	*tofree;
    typval_T    tv;

    అయితే (!చివర్లు_excmd2(eap->cmd, arg) && !got_int)
    {
	ఒకవేళ (error || eap->skip)
	{
	    arg = కనుగొను_name_చివర(arg, ఖాళీ, ఖాళీ, FNE_INCL_BR | FNE_CHECK_ప్రారంభం);
	    ఒకవేళ (!VIM_ISWHITE(*arg) && !చివర్లు_excmd(*arg))
	    {
		emsg_severe = TRUE;
		ఒకవేళ (!did_emsg)
		    semsg(_(e_trailing_characters_str), arg);
		ఆపు;
	    }
	}
	లేకపోతే
	{
	    // get_name_len() takes care of expanding curly braces
	    name_ప్రారంభం = name = arg;
	    len = get_name_len(&arg, &tofree, TRUE, TRUE);
	    ఒకవేళ (len <= 0)
	    {
		// This is mainly to keep test 49 working: when expanding
		// curly braces fails overrule the exception error message.
		ఒకవేళ (len < 0 && !aborting())
		{
		    emsg_severe = TRUE;
		    semsg(_(e_invalid_వాదం_str), arg);
		    ఆపు;
		}
		error = TRUE;
	    }
	    లేకపోతే
	    {
		arg = skipwhite(arg);
		ఒకవేళ (tofree != ఖాళీ)
		    name = tofree;
		ఒకవేళ (eval_variable(name, len, 0, &tv, ఖాళీ,
						     EVAL_VAR_VERBOSE) == FAIL)
		    error = TRUE;
		లేకపోతే
		{
		    // handle d.key, l[idx], f(expr)
		    arg_subsc = arg;
		    ఒకవేళ (handle_subscript(&arg, name_ప్రారంభం, &tv,
					      &EVALARG_EVALUATE, TRUE) == FAIL)
			error = TRUE;
		    లేకపోతే
		    {
			ఒకవేళ (arg == arg_subsc && len == 2 && name[1] == ':')
			{
			    ఎంపిక (*name)
			    {
				స్థితి 'g': list_glob_vars(మొదటి); ఆపు;
				స్థితి 'b': list_buf_vars(మొదటి); ఆపు;
				స్థితి 'w': list_win_vars(మొదటి); ఆపు;
				స్థితి 't': list_tab_vars(మొదటి); ఆపు;
				స్థితి 'v': list_vim_vars(మొదటి); ఆపు;
				స్థితి 's': list_script_vars(మొదటి); ఆపు;
				స్థితి 'l': list_func_vars(మొదటి); ఆపు;
				అప్రమేయ:
					  semsg(_(e_cant_list_variables_for_str), name);
			    }
			}
			లేకపోతే
			{
			    char_u	numbuf[NUMBUFLEN];
			    char_u	*tf;
			    పూర్ణాంకం		c;
			    char_u	*s;

			    s = echo_string(&tv, &tf, numbuf, 0);
			    c = *arg;
			    *arg = NUL;
			    list_one_var_a("",
				    arg == arg_subsc ? name : name_ప్రారంభం,
				    tv.v_type,
				    s == ఖాళీ ? (char_u *)"" : s,
				    మొదటి);
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

    తిరిగి arg;
}

/*
 * Set an environment variable, part of ex_let_one().
 */
    స్థిర char_u *
ex_let_env(
    char_u	*arg,
    typval_T	*tv,
    పూర్ణాంకం		flags,
    char_u	*endchars,
    char_u	*op)
{
    char_u	*arg_చివర = ఖాళీ;
    char_u	*name;
    పూర్ణాంకం		len;

    ఒకవేళ ((flags & (కేటాయించు_CONST | కేటాయించు_FINAL))
					 && (flags & కేటాయించు_FOR_LOOP) == 0)
    {
	emsg(_(e_cannot_తాళం_environment_variable));
	తిరిగి ఖాళీ;
    }

    // Find the end of the name.
    ++arg;
    name = arg;
    len = get_env_len(&arg);
    ఒకవేళ (len == 0)
	semsg(_(e_invalid_వాదం_str), name - 1);
    లేకపోతే
    {
	ఒకవేళ (op != ఖాళీ && vim_strchr((char_u *)"+-*/%", *op) != ఖాళీ)
	    semsg(_(e_wrong_variable_type_for_str_equal), op);
	లేకపోతే ఒకవేళ (endchars != ఖాళీ
			      && vim_strchr(endchars, *skipwhite(arg)) == ఖాళీ)
	    emsg(_(e_unexpected_characters_లోపల_let));
	లేకపోతే ఒకవేళ (!check_secure() && !check_restricted())
	{
	    char_u	*tofree = ఖాళీ;
	    పూర్ణాంకం		c1 = name[len];
	    char_u	*p;

	    name[len] = NUL;
	    p = tv_get_string_chk(tv);
	    ఒకవేళ (p != ఖాళీ && op != ఖాళీ && *op == '.')
	    {
		పూర్ణాంకం	mustfree = FALSE;
		char_u  *s = vim_getenv(name, &mustfree);

		ఒకవేళ (s != ఖాళీ)
		{
		    p = tofree = concat_str(s, p);
		    ఒకవేళ (mustfree)
			vim_free(s);
		}
	    }
	    ఒకవేళ (p != ఖాళీ)
	    {
		vim_setenv_ext(name, p);
		arg_చివర = arg;
	    }
	    name[len] = c1;
	    vim_free(tofree);
	}
    }
    తిరిగి arg_చివర;
}

/*
 * Set an option, part of ex_let_one().
 */
    స్థిర char_u *
ex_let_option(
    char_u	*arg,
    typval_T	*tv,
    పూర్ణాంకం		flags,
    char_u	*endchars,
    char_u	*op)
{
    char_u	*p;
    పూర్ణాంకం		scope;
    char_u	*arg_చివర = ఖాళీ;

    ఒకవేళ ((flags & (కేటాయించు_CONST | కేటాయించు_FINAL))
					 && (flags & కేటాయించు_FOR_LOOP) == 0)
    {
	emsg(_(e_cannot_తాళం_option));
	తిరిగి ఖాళీ;
    }

    // Find the end of the name.
    p = కనుగొను_option_చివర(&arg, &scope);
    ఒకవేళ (p == ఖాళీ || (endchars != ఖాళీ
			       && vim_strchr(endchars, *skipwhite(p)) == ఖాళీ))
    {
	emsg(_(e_unexpected_characters_లోపల_let));
	తిరిగి ఖాళీ;
    }

    పూర్ణాంకం		c1;
    దీర్ఘ	n = 0;
    getoption_T	opt_type;
    దీర్ఘ	numval;
    char_u	*stringval = ఖాళీ;
    char_u	*s = ఖాళీ;
    పూర్ణాంకం		failed = FALSE;
    పూర్ణాంకం		opt_p_flags;
    char_u	*tofree = ఖాళీ;
    char_u	numbuf[NUMBUFLEN];

    c1 = *p;
    *p = NUL;

    opt_type = get_option_విలువ(arg, &numval, &stringval, &opt_p_flags, scope);
    ఒకవేళ (opt_type == gov_unknown && arg[0] != 't' && arg[1] != '_')
    {
	semsg(_(e_unknown_option_str_2), arg);
	వెళ్ళు theend;
    }
    ఒకవేళ (op != ఖాళీ && *op != '='
	&& (((opt_type == gov_bool || opt_type == gov_సంఖ్య) && *op == '.')
	    || (opt_type == gov_string && *op != '.')))
    {
	semsg(_(e_wrong_variable_type_for_str_equal), op);
	వెళ్ళు theend;
    }

    ఒకవేళ ((opt_type == gov_bool
		|| opt_type == gov_సంఖ్య
		|| opt_type == gov_hidden_bool
		|| opt_type == gov_hidden_సంఖ్య)
		     && (tv->v_type != VAR_STRING || !లోపల_vim9script()))
    {
	ఒకవేళ (opt_type == gov_bool || opt_type == gov_hidden_bool)
	    // bool, possibly hidden
	    n = (దీర్ఘ)tv_get_bool_chk(tv, &failed);
	లేకపోతే
	    // number, possibly hidden
	    n = (దీర్ఘ)tv_get_సంఖ్య_chk(tv, &failed);
	ఒకవేళ (failed)
	    వెళ్ళు theend;
    }

    ఒకవేళ ((opt_p_flags & P_FUNC) && (tv->v_type == VAR_PARTIAL
						    || tv->v_type == VAR_FUNC))
    {
	// If the option can be set to a function reference or a lambda
	// and the passed value is a function reference, then convert it to
	// the name (string) of the function reference.
	s = tv2string(tv, &tofree, numbuf, 0);
	ఒకవేళ (s == ఖాళీ)
	    వెళ్ళు theend;
    }
    // Avoid setting a string option to the text "v:false" or similar.
    // In Vim9 script also don't convert a number to string.
    లేకపోతే ఒకవేళ (tv->v_type != VAR_BOOL && tv->v_type != VAR_SPECIAL
		     && (!లోపల_vim9script() || tv->v_type != VAR_సంఖ్య))
    {
	s = tv_get_string_chk(tv);
	ఒకవేళ (s == ఖాళీ)
	    వెళ్ళు theend;
    }
    లేకపోతే ఒకవేళ (opt_type == gov_string || opt_type == gov_hidden_string)
    {
	emsg(_(e_string_required));
	వెళ్ళు theend;
    }

    ఒకవేళ (op != ఖాళీ && *op != '=')
    {
	// number, in legacy script also bool
	ఒకవేళ (opt_type == gov_సంఖ్య
				 || (opt_type == gov_bool && !లోపల_vim9script()))
	{
	    ఎంపిక (*op)
	    {
		స్థితి '+': n = numval + n; ఆపు;
		స్థితి '-': n = numval - n; ఆపు;
		స్థితి '*': n = numval * n; ఆపు;
		స్థితి '/': n = (దీర్ఘ)అంకె_divide(numval, n, &failed); ఆపు;
		స్థితి '%': n = (దీర్ఘ)అంకె_modulus(numval, n, &failed); ఆపు;
	    }
	    s = ఖాళీ;
	    ఒకవేళ (failed)
		వెళ్ళు theend;
	}
	లేకపోతే ఒకవేళ (opt_type == gov_string && stringval != ఖాళీ && s != ఖాళీ)
	{
	    // string
	    s = concat_str(stringval, s);
	    vim_free(stringval);
	    stringval = s;
	}
    }

    అక్షరం *err = సమితి_option_విలువ(arg, n, s, scope);
    arg_చివర = p;
    ఒకవేళ (err != ఖాళీ)
	emsg(_(err));

theend:
    *p = c1;
    vim_free(stringval);
    vim_free(tofree);
    తిరిగి arg_చివర;
}

/*
 * Set a register, part of ex_let_one().
 */
    స్థిర char_u *
ex_let_రిజిస్టరు(
    char_u	*arg,
    typval_T	*tv,
    పూర్ణాంకం		flags,
    char_u	*endchars,
    char_u	*op)
{
    char_u	*arg_చివర = ఖాళీ;

    ఒకవేళ ((flags & (కేటాయించు_CONST | కేటాయించు_FINAL))
					 && (flags & కేటాయించు_FOR_LOOP) == 0)
    {
	emsg(_(e_cannot_తాళం_రిజిస్టరు));
	తిరిగి ఖాళీ;
    }
    ++arg;
    ఒకవేళ (op != ఖాళీ && vim_strchr((char_u *)"+-*/%", *op) != ఖాళీ)
	semsg(_(e_wrong_variable_type_for_str_equal), op);
    లేకపోతే ఒకవేళ (endchars != ఖాళీ
			  && vim_strchr(endchars, *skipwhite(arg + 1)) == ఖాళీ)
	emsg(_(e_unexpected_characters_లోపల_let));
    లేకపోతే
    {
	char_u	*ptofree = ఖాళీ;
	char_u	*p;

	p = tv_get_string_chk(tv);
	ఒకవేళ (p != ఖాళీ && op != ఖాళీ && *op == '.')
	{
	    char_u  *s = get_reg_contents(*arg == '@'
						  ? '"' : *arg, GREG_సమాసం_SRC);

	    ఒకవేళ (s != ఖాళీ)
	    {
		p = ptofree = concat_str(s, p);
		vim_free(s);
	    }
	}
	ఒకవేళ (p != ఖాళీ)
	{
	    write_reg_contents(*arg == '@' ? '"' : *arg, p, -1, FALSE);
	    arg_చివర = arg + 1;
	}
	vim_free(ptofree);
    }
    తిరిగి arg_చివర;
}

/*
 * Set one item of ":let var = expr" or ":let [v1, v2] = list" to its value.
 * Returns a pointer to the char just after the var name.
 * Returns NULL if there is an error.
 */
    స్థిర char_u *
ex_let_one(
    char_u	*arg,		// points to variable name
    typval_T	*tv,		// value to assign to variable
    పూర్ణాంకం		copy,		// copy value from "tv"
    పూర్ణాంకం		flags,		// ASSIGN_CONST, ASSIGN_FINAL, etc.
    char_u	*endchars,	// valid chars after variable name  or NULL
    char_u	*op,		// "+", "-", "."  or NULL
    పూర్ణాంకం		var_idx)	// variable index for "let [a, b] = list"
{
    char_u	*arg_చివర = ఖాళీ;

    ఒకవేళ (లోపల_vim9script() && (flags & (కేటాయించు_NO_DECL | కేటాయించు_DECL)) == 0
			&& (flags & (కేటాయించు_CONST | కేటాయించు_FINAL)) == 0
				  && vim_strchr((char_u *)"$@&", *arg) != ఖాళీ)
    {
	vim9_declare_error(arg);
	తిరిగి ఖాళీ;
    }

    ఒకవేళ (check_typval_is_విలువ(tv) == FAIL)
	తిరిగి ఖాళీ;

    // In Vim9 script an environment variable and a register only take a
    // String.  "@#" also takes a buffer number.
    ఒకవేళ (లోపల_vim9script() && (*arg == '$' || *arg == '@')
	    && !(*arg == '@' && arg[1] == '#' && tv->v_type == VAR_సంఖ్య
					       && (op == ఖాళీ || *op == '=')))
    {
	where_T	where = WHERE_INIT;

	ఒకవేళ (check_typval_type(&t_string, tv, where) == FAIL)
	    తిరిగి ఖాళీ;
    }

    ఒకవేళ (*arg == '$')
    {
	// ":let $VAR = expr": Set environment variable.
	తిరిగి ex_let_env(arg, tv, flags, endchars, op);
    }
    లేకపోతే ఒకవేళ (*arg == '&')
    {
	// ":let &option = expr": Set option value.
	// ":let &l:option = expr": Set local option value.
	// ":let &g:option = expr": Set global option value.
	// ":for &ts in range(8)": Set option value for for loop
	తిరిగి ex_let_option(arg, tv, flags, endchars, op);
    }
    లేకపోతే ఒకవేళ (*arg == '@')
    {
	// ":let @r = expr": Set register contents.
	తిరిగి ex_let_రిజిస్టరు(arg, tv, flags, endchars, op);
    }
    లేకపోతే ఒకవేళ (eval_isnamec1(*arg) || *arg == '{')
    {
	lval_T	lv;
	char_u	*p;
	పూర్ణాంకం	lval_flags = (flags & (కేటాయించు_NO_DECL | కేటాయించు_DECL))
							     ? GLV_NO_DECL : 0;
	lval_flags |= (flags & కేటాయించు_FOR_LOOP) ? GLV_FOR_LOOP : 0;
	ఒకవేళ (op != ఖాళీ && *op != '=')
	    lval_flags |= GLV_కేటాయించు_WITH_OP;

	// ":let var = expr": Set internal variable.
	// ":let var: type = expr": Set internal variable with type.
	// ":let {expr} = expr": Idem, name made with curly braces
	p = get_lval(arg, tv, &lv, FALSE, FALSE, lval_flags, FNE_CHECK_ప్రారంభం);
	ఒకవేళ (p != ఖాళీ && lv.ll_name != ఖాళీ)
	{
	    ఒకవేళ (endchars != ఖాళీ && vim_strchr(endchars,
					   *skipwhite(lv.ll_name_చివర)) == ఖాళీ)
	    {
		emsg(_(e_unexpected_characters_లోపల_let));
	    }
	    లేకపోతే
	    {
		సమితి_var_lval(&lv, p, tv, copy, flags, op, var_idx);
		arg_చివర = lv.ll_name_చివర;
	    }
	}
	clear_lval(&lv);
    }
    లేకపోతే
	semsg(_(e_invalid_వాదం_str), arg);

    తిరిగి arg_చివర;
}

/*
 * ":unlet[!] var1 ... " command.
 */
    శూన్య
ex_unlet(exarg_T *eap)
{
    ex_unletlock(eap, eap->arg, 0, eap->forceit ? GLV_QUIET : 0, do_unlet_var, ఖాళీ);
}

/*
 * ":lockvar" and ":unlockvar" commands
 */
    శూన్య
ex_lockvar(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    పూర్ణాంకం		deep = 2;

    ఒకవేళ (eap->forceit)
	deep = -1;
    లేకపోతే ఒకవేళ (vim_isdigit(*arg))
    {
	deep = getdigits(&arg);
	arg = skipwhite(arg);
    }

    ex_unletlock(eap, arg, deep, 0, do_తాళం_var, ఖాళీ);
}

/*
 * ":unlet", ":lockvar" and ":unlockvar" are quite similar.
 * Also used for Vim9 script.  "callback" is invoked as:
 *	callback(&lv, name_end, eap, deep, cookie)
 */
    శూన్య
ex_unletlock(
    exarg_T	*eap,
    char_u	*argstart,
    పూర్ణాంకం		deep,
    పూర్ణాంకం		glv_flags,
    పూర్ణాంకం		(*callback)(lval_T *, char_u *, exarg_T *, పూర్ణాంకం, శూన్య *),
    శూన్య	*cookie)
{
    char_u	*arg = argstart;
    char_u	*name_చివర;
    పూర్ణాంకం		error = FALSE;
    lval_T	lv;

    చేయి
    {
	ఒకవేళ (*arg == '$')
	{
	    lv.ll_name = arg;
	    lv.ll_tv = ఖాళీ;
	    ++arg;
	    ఒకవేళ (get_env_len(&arg) == 0)
	    {
		semsg(_(e_invalid_వాదం_str), arg - 1);
		తిరిగి;
	    }
	    ఒకవేళ (!error && !eap->skip
			      && callback(&lv, arg, eap, deep, cookie) == FAIL)
		error = TRUE;
	    name_చివర = arg;
	}
	లేకపోతే
	{
	    // Parse the name and find the end.
	    name_చివర = get_lval(arg, ఖాళీ, &lv, TRUE, eap->skip || error,
				     glv_flags | GLV_NO_DECL, FNE_CHECK_ప్రారంభం);
	    ఒకవేళ (lv.ll_name == ఖాళీ)
		error = TRUE;	    // error but continue parsing
	    ఒకవేళ (name_చివర == ఖాళీ || (!VIM_ISWHITE(*name_చివర)
						    && !చివర్లు_excmd(*name_చివర)))
	    {
		ఒకవేళ (name_చివర != ఖాళీ)
		{
		    emsg_severe = TRUE;
		    semsg(_(e_trailing_characters_str), name_చివర);
		}
		ఒకవేళ (!(eap->skip || error))
		    clear_lval(&lv);
		ఆపు;
	    }

	    ఒకవేళ (!error && !eap->skip
			 && callback(&lv, name_చివర, eap, deep, cookie) == FAIL)
		error = TRUE;

	    ఒకవేళ (!eap->skip)
		clear_lval(&lv);
	}

	arg = skipwhite(name_చివర);
    } అయితే (!చివర్లు_excmd2(name_చివర, arg));

    సమితి_nextcmd(eap, arg);
}

    స్థిర పూర్ణాంకం
do_unlet_var(
    lval_T	*lp,
    char_u	*name_చివర,
    exarg_T	*eap,
    పూర్ణాంకం		deep వాడని,
    శూన్య	*cookie వాడని)
{
    పూర్ణాంకం		forceit = eap->forceit;
    పూర్ణాంకం		ret = OK;
    పూర్ణాంకం		cc;

    ఒకవేళ (lp->ll_tv == ఖాళీ)
    {
	cc = *name_చివర;
	*name_చివర = NUL;

	// Environment variable, normal name or expanded name.
	ఒకవేళ (*lp->ll_name == '$')
	    vim_unsetenv_ext(lp->ll_name + 1);
	లేకపోతే ఒకవేళ (do_unlet(lp->ll_name, forceit) == FAIL)
	    ret = FAIL;
	*name_చివర = cc;
    }
    లేకపోతే ఒకవేళ ((lp->ll_list != ఖాళీ
		 && విలువ_check_తాళం(lp->ll_list->lv_తాళం, lp->ll_name, FALSE))
	    || (lp->ll_dict != ఖాళీ
		&& విలువ_check_తాళం(lp->ll_dict->dv_తాళం, lp->ll_name, FALSE)))
	తిరిగి FAIL;
    లేకపోతే ఒకవేళ (lp->ll_range)
	list_unlet_range(lp->ll_list, lp->ll_li, lp->ll_n1,
						    !lp->ll_empty2, lp->ll_n2);
    లేకపోతే ఒకవేళ (lp->ll_list != ఖాళీ)
	// unlet a List item.
	listitem_remove(lp->ll_list, lp->ll_li);
    లేకపోతే ఒకవేళ (lp->ll_dict != ఖాళీ)
	// unlet a Dictionary item.
	dictitem_remove(lp->ll_dict, lp->ll_di, "unlet");
    లేకపోతే
    {
	semsg(_(e_cannot_unlet_imported_అంశం_str), lp->ll_name);
	తిరిగి FAIL;
    }

    తిరిగి ret;
}

/*
 * Unlet one item or a range of items from a list.
 * Return OK or FAIL.
 */
    శూన్య
list_unlet_range(
	list_T	    *l,
	listitem_T  *li_మొదటి,
	దీర్ఘ	    n1_arg,
	పూర్ణాంకం	    has_n2,
	దీర్ఘ	    n2)
{
    listitem_T  *li = li_మొదటి;
    పూర్ణాంకం		n1 = n1_arg;

    // Delete a range of List items.
    li = li_మొదటి;
    n1 = n1_arg;
    అయితే (li != ఖాళీ && (!has_n2 || n2 >= n1))
    {
	listitem_T *తదుపరి = li->li_తదుపరి;

	listitem_remove(l, li);
	li = తదుపరి;
	++n1;
    }
}
/*
 * "unlet" a variable.  Return OK if it existed, FAIL if not.
 * When "forceit" is TRUE don't complain if the variable doesn't exist.
 */
    పూర్ణాంకం
do_unlet(char_u *name, పూర్ణాంకం forceit)
{
    hashtab_T	*ht;
    hashitem_T	*hi;
    char_u	*varname = ఖాళీ;  // init to shut up gcc
    dict_T	*d;
    dictitem_T	*di;

    // can't :unlet a script variable in Vim9 script
    ఒకవేళ (లోపల_vim9script() && check_vim9_unlet(name) == FAIL)
	తిరిగి FAIL;

    ht = కనుగొను_var_ht(name, &varname);

    // can't :unlet a script variable in Vim9 script from a function
    ఒకవేళ (ht == get_script_local_ht()
	    && SCRIPT_ID_VALID(current_sctx.sc_sid)
	    && SCRIPT_అంశం(current_sctx.sc_sid)->sn_సంస్కరణ
							 == SCRIPT_సంస్కరణ_VIM9
	    && check_vim9_unlet(name) == FAIL)
	తిరిగి FAIL;

    ఒకవేళ (ht != ఖాళీ && *varname != NUL)
    {
	d = get_current_funccal_dict(ht);
	ఒకవేళ (d == ఖాళీ)
	{
	    ఒకవేళ (ht == &globvarht)
		d = &globvardict;
	    లేకపోతే ఒకవేళ (ht == &compat_hashtab)
		d = &vimvardict;
	    లేకపోతే
	    {
		di = కనుగొను_var_లోపల_ht(ht, *name, (char_u *)"", FALSE);
		d = di == ఖాళీ ? ఖాళీ : di->di_tv.vval.v_dict;
	    }
	    ఒకవేళ (d == ఖాళీ)
	    {
		అంతర్గత_error("do_unlet()");
		తిరిగి FAIL;
	    }
	}
	hi = hash_కనుగొను(ht, varname);
	ఒకవేళ (HASHITEM_ఖాళీలు(hi))
	    hi = కనుగొను_hi_లోపల_scoped_ht(name, &ht);
	ఒకవేళ (hi != ఖాళీ && !HASHITEM_ఖాళీలు(hi))
	{
	    di = HI2DI(hi);
	    ఒకవేళ (var_check_fixed(di->di_flags, name, FALSE)
		    || var_check_ro(di->di_flags, name, FALSE)
		    || విలువ_check_తాళం(d->dv_తాళం, name, FALSE)
		    || check_hashtab_frozen(ht, "unlet"))
		తిరిగి FAIL;

	    delete_var(ht, hi);
	    తిరిగి OK;
	}
    }
    ఒకవేళ (forceit)
	తిరిగి OK;
    semsg(_(e_no_such_variable_str), name);
    తిరిగి FAIL;
}

    స్థిర శూన్య
report_lockvar_member(అక్షరం *సందేశం, lval_T *lp)
{
    పూర్ణాంకం did_alloc = FALSE;
    char_u *vname = (char_u *)"";
    char_u *class_name = lp->ll_class != ఖాళీ
				    ? lp->ll_class->class_name.string : (char_u *)"";
    ఒకవేళ (lp->ll_name != ఖాళీ)
    {
	ఒకవేళ (lp->ll_name_చివర == ఖాళీ)
	    vname = lp->ll_name;
	లేకపోతే
	{
	    vname = vim_strnsave(lp->ll_name, lp->ll_name_చివర - lp->ll_name);
	    ఒకవేళ (vname == ఖాళీ)
		తిరిగి;
	    did_alloc = TRUE;
	}
    }
    semsg(_(సందేశం), vname, class_name);
    ఒకవేళ (did_alloc)
	vim_free(vname);
}

/*
 * Lock or unlock variable indicated by "lp".
 * "deep" is the levels to go (-1 for unlimited);
 * "lock" is TRUE for ":lockvar", FALSE for ":unlockvar".
 */
    స్థిర పూర్ణాంకం
do_తాళం_var(
    lval_T	*lp,
    char_u	*name_చివర,
    exarg_T	*eap,
    పూర్ణాంకం		deep,
    శూన్య	*cookie వాడని)
{
    పూర్ణాంకం		తాళం = eap->cmdidx == CMD_lockvar;
    పూర్ణాంకం		ret = OK;
    పూర్ణాంకం		cc;
    dictitem_T	*di;

#నిర్వచితమైతే LOG_LOCKVAR
    ch_log(ఖాళీ, "LKVAR: do_lock_var(): name %s, is_root %d", lp->ll_name, lp->ll_is_మూలం);
#అయితే_ముగింపు

    ఒకవేళ (lp->ll_tv == ఖాళీ)
    {
	cc = *name_చివర;
	*name_చివర = NUL;
	ఒకవేళ (*lp->ll_name == '$')
	{
	    semsg(_(e_cannot_తాళం_or_unlock_variable_str), lp->ll_name);
	    ret = FAIL;
	}
	లేకపోతే
	{
	    // Normal name or expanded name.
	    di = కనుగొను_var(lp->ll_name, ఖాళీ, TRUE);
	    ఒకవేళ (di == ఖాళీ)
	    {
		ఒకవేళ (లోపల_vim9script())
		    semsg(_(e_cannot_కనుగొను_variable_గమ్యం_unlock_str),
								  lp->ll_name);
		ret = FAIL;
	    }
	    లేకపోతే
	    {
		ఒకవేళ ((di->di_flags & DI_FLAGS_FIX)
			    && di->di_tv.v_type != VAR_DICT
			    && di->di_tv.v_type != VAR_LIST)
		{
		    // For historic reasons this error is not given for a list
		    // or dict.  E.g., the b: dict could be locked/unlocked.
		    semsg(_(e_cannot_తాళం_or_unlock_variable_str), lp->ll_name);
		    ret = FAIL;
		}
		లేకపోతే
		{
		    ఒకవేళ (లోపల_vim9script())
		    {
			svar_T  *sv = కనుగొను_typval_లోపల_script(&di->di_tv,
								     0, FALSE);

			ఒకవేళ (sv != ఖాళీ && sv->sv_const != 0)
			{
			    semsg(_(e_cannot_change_readonly_variable_str),
								  lp->ll_name);
			    ret = FAIL;
			}
		    }

		    ఒకవేళ (ret == OK)
		    {
			ఒకవేళ (తాళం)
			    di->di_flags |= DI_FLAGS_తాళం;
			లేకపోతే
			    di->di_flags &= ~DI_FLAGS_తాళం;
			ఒకవేళ (deep != 0)
			    అంశం_తాళం(&di->di_tv, deep, తాళం, FALSE);
		    }
		}
	    }
	}
	*name_చివర = cc;
    }
    లేకపోతే ఒకవేళ (deep == 0 && lp->ll_object == ఖాళీ && lp->ll_class == ఖాళీ)
    {
	// nothing to do
    }
    లేకపోతే ఒకవేళ (lp->ll_is_మూలం)
	// (un)lock the item.
	అంశం_తాళం(lp->ll_tv, deep, తాళం, FALSE);
    లేకపోతే ఒకవేళ (lp->ll_range)
    {
	listitem_T    *li = lp->ll_li;

	// (un)lock a range of List items.
	అయితే (li != ఖాళీ && (lp->ll_empty2 || lp->ll_n2 >= lp->ll_n1))
	{
	    అంశం_తాళం(&li->li_tv, deep, తాళం, FALSE);
	    li = li->li_తదుపరి;
	    ++lp->ll_n1;
	}
    }
    లేకపోతే ఒకవేళ (lp->ll_list != ఖాళీ)
	// (un)lock a List item.
	అంశం_తాళం(&lp->ll_li->li_tv, deep, తాళం, FALSE);
    లేకపోతే ఒకవేళ (lp->ll_object != ఖాళీ)  // This check must be before ll_class.
    {
	// (un)lock an object variable.
	report_lockvar_member(e_cannot_తాళం_object_variable_str, lp);
	ret = FAIL;
    }
    లేకపోతే ఒకవేళ (lp->ll_class != ఖాళీ)
    {
	// (un)lock a class variable.
	report_lockvar_member(e_cannot_తాళం_class_variable_str, lp);
	ret = FAIL;
    }
    లేకపోతే
    {
	// (un)lock a Dictionary item.
	ఒకవేళ (lp->ll_di == ఖాళీ)
	{
	    emsg(_(e_dictionary_required));
	    ret = FAIL;
	}
	లేకపోతే
	    అంశం_తాళం(&lp->ll_di->di_tv, deep, తాళం, FALSE);
    }

    తిరిగి ret;
}

/*
 * Lock or unlock an item.  "deep" is nr of levels to go.
 * When "check_refcount" is TRUE do not lock a list or dict with a reference
 * count larger than 1.
 */
    శూన్య
అంశం_తాళం(typval_T *tv, పూర్ణాంకం deep, పూర్ణాంకం తాళం, పూర్ణాంకం check_refcount)
{
    స్థిర పూర్ణాంకం	recurse = 0;
    list_T	*l;
    listitem_T	*li;
    dict_T	*d;
    blob_T	*b;
    hashitem_T	*hi;
    పూర్ణాంకం		todo;

#నిర్వచితమైతే LOG_LOCKVAR
    ch_log(ఖాళీ, "LKVAR: item_lock(): type %s", vartype_name(tv->v_type));
#అయితే_ముగింపు

    ఒకవేళ (recurse >= DICT_MAXNEST)
    {
	emsg(_(e_variable_nested_too_deep_for_unlock));
	తిరిగి;
    }
    ఒకవేళ (deep == 0)
	తిరిగి;
    ++recurse;

    // lock/unlock the item itself
    ఒకవేళ (తాళం)
	tv->v_తాళం |= VAR_LOCKED;
    లేకపోతే
	tv->v_తాళం &= ~VAR_LOCKED;

    ఎంపిక (tv->v_type)
    {
	స్థితి VAR_UNKNOWN:
	స్థితి VAR_ANY:
	స్థితి VAR_VOID:
	స్థితి VAR_సంఖ్య:
	స్థితి VAR_BOOL:
	స్థితి VAR_STRING:
	స్థితి VAR_FUNC:
	స్థితి VAR_PARTIAL:
	స్థితి VAR_FLOAT:
	స్థితి VAR_SPECIAL:
	స్థితి VAR_పని:
	స్థితి VAR_CHANNEL:
	స్థితి VAR_INSTR:
	స్థితి VAR_CLASS:
	స్థితి VAR_OBJECT:
	స్థితి VAR_TYPEALIAS:
	    ఆపు;

	స్థితి VAR_BLOB:
	    ఒకవేళ ((b = tv->vval.v_blob) != ఖాళీ
				    && !(check_refcount && b->bv_refcount > 1))
	    {
		ఒకవేళ (తాళం)
		    b->bv_తాళం |= VAR_LOCKED;
		లేకపోతే
		    b->bv_తాళం &= ~VAR_LOCKED;
	    }
	    ఆపు;
	స్థితి VAR_LIST:
	    ఒకవేళ ((l = tv->vval.v_list) != ఖాళీ
				    && !(check_refcount && l->lv_refcount > 1))
	    {
		ఒకవేళ (తాళం)
		    l->lv_తాళం |= VAR_LOCKED;
		లేకపోతే
		    l->lv_తాళం &= ~VAR_LOCKED;
		ఒకవేళ (deep < 0 || deep > 1)
		{
		    ఒకవేళ (l->lv_మొదటి == &range_list_అంశం)
			l->lv_తాళం |= VAR_వస్తువులు_LOCKED;
		    లేకపోతే
		    {
			// recursive: lock/unlock the items the List contains
			CHECK_LIST_MATERIALIZE(l);
			FOR_ALL_LIST_వస్తువులు(l, li) అంశం_తాళం(&li->li_tv,
					       deep - 1, తాళం, check_refcount);
		    }
		}
	    }
	    ఆపు;
	స్థితి VAR_TUPLE:
	    tuple_తాళం(tv->vval.v_tuple, deep, తాళం, check_refcount);
	    ఆపు;
	స్థితి VAR_DICT:
	    ఒకవేళ ((d = tv->vval.v_dict) != ఖాళీ
				    && !(check_refcount && d->dv_refcount > 1))
	    {
		ఒకవేళ (తాళం)
		    d->dv_తాళం |= VAR_LOCKED;
		లేకపోతే
		    d->dv_తాళం &= ~VAR_LOCKED;
		ఒకవేళ (deep < 0 || deep > 1)
		{
		    // recursive: lock/unlock the items the List contains
		    todo = (పూర్ణాంకం)d->dv_hashtab.ht_వాడిన;
		    FOR_ALL_HASHTAB_వస్తువులు(&d->dv_hashtab, hi, todo)
		    {
			ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
			{
			    --todo;
			    అంశం_తాళం(&HI2DI(hi)->di_tv, deep - 1, తాళం,
							       check_refcount);
			}
		    }
		}
	    }
    }
    --recurse;
}

#ఒకవేళ defined(FEAT_MENU) && defined(FEAT_MULTI_LANG)
/*
 * Delete all "menutrans_" variables.
 */
    శూన్య
del_menutrans_vars(శూన్య)
{
    hashitem_T	*hi;
    పూర్ణాంకం		todo;

    hash_తాళం(&globvarht);
    todo = (పూర్ణాంకం)globvarht.ht_వాడిన;
    కొరకు (hi = globvarht.ht_array; todo > 0 && !got_int; ++hi)
    {
	ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
	{
	    --todo;
	    ఒకవేళ (STRNCMP(HI2DI(hi)->di_కీలకం, "menutrans_", 10) == 0)
		delete_var(&globvarht, hi);
	}
    }
    hash_unlock(&globvarht);
}
#అయితే_ముగింపు

/*
 * Local string buffer for the next two functions to store a variable name
 * with its prefix. Allocated in cat_prefix_varname(), freed later in
 * get_user_var_name().
 */

స్థిర char_u	*varnamebuf = ఖాళీ;
స్థిర పూర్ణాంకం	varnamebuflen = 0;

/*
 * Function to concatenate a prefix and a variable name.
 */
    char_u *
cat_ఉపసర్గ_varname(పూర్ణాంకం ఉపసర్గ, char_u *name)
{
    పూర్ణాంకం		len;

    len = (పూర్ణాంకం)STRLEN(name) + 3;
    ఒకవేళ (len > varnamebuflen)
    {
	vim_free(varnamebuf);
	len += 10;			// some additional space
	varnamebuf = alloc(len);
	ఒకవేళ (varnamebuf == ఖాళీ)
	{
	    varnamebuflen = 0;
	    తిరిగి ఖాళీ;
	}
	varnamebuflen = len;
    }
    *varnamebuf = ఉపసర్గ;
    varnamebuf[1] = ':';
    STRCPY(varnamebuf + 2, name);
    తిరిగి varnamebuf;
}

/*
 * Function given to ExpandGeneric() to obtain the list of user defined
 * (global/buffer/window/built-in) variable names.
 */
    char_u *
get_user_var_name(expand_T *xp, పూర్ణాంకం idx)
{
    స్థిర long_u	gdone;
    స్థిర long_u	bdone;
    స్థిర long_u	wdone;
    స్థిర long_u	tdone;
    స్థిర పూర్ణాంకం		vidx;
    స్థిర hashitem_T	*hi;
    hashtab_T		*ht;

    ఒకవేళ (idx == 0)
    {
	gdone = bdone = wdone = vidx = 0;
	tdone = 0;
    }

    // Global variables
    ఒకవేళ (gdone < globvarht.ht_వాడిన)
    {
	ఒకవేళ (gdone++ == 0)
	    hi = globvarht.ht_array;
	లేకపోతే
	    ++hi;
	అయితే (HASHITEM_ఖాళీలు(hi))
	    ++hi;
	ఒకవేళ (STRNCMP("g:", xp->xp_నమూనా, 2) == 0)
	    తిరిగి cat_ఉపసర్గ_varname('g', hi->hi_కీలకం);
	తిరిగి hi->hi_కీలకం;
    }

    // b: variables
    ht = &prevwin_curwin()->w_బఫరు->b_vars->dv_hashtab;
    ఒకవేళ (bdone < ht->ht_వాడిన)
    {
	ఒకవేళ (bdone++ == 0)
	    hi = ht->ht_array;
	లేకపోతే
	    ++hi;
	అయితే (HASHITEM_ఖాళీలు(hi))
	    ++hi;
	తిరిగి cat_ఉపసర్గ_varname('b', hi->hi_కీలకం);
    }

    // w: variables
    ht = &prevwin_curwin()->w_vars->dv_hashtab;
    ఒకవేళ (wdone < ht->ht_వాడిన)
    {
	ఒకవేళ (wdone++ == 0)
	    hi = ht->ht_array;
	లేకపోతే
	    ++hi;
	అయితే (HASHITEM_ఖాళీలు(hi))
	    ++hi;
	తిరిగి cat_ఉపసర్గ_varname('w', hi->hi_కీలకం);
    }

    // t: variables
    ht = &curtab->tp_vars->dv_hashtab;
    ఒకవేళ (tdone < ht->ht_వాడిన)
    {
	ఒకవేళ (tdone++ == 0)
	    hi = ht->ht_array;
	లేకపోతే
	    ++hi;
	అయితే (HASHITEM_ఖాళీలు(hi))
	    ++hi;
	తిరిగి cat_ఉపసర్గ_varname('t', hi->hi_కీలకం);
    }

    // v: variables
    ఒకవేళ (vidx < VV_LEN)
	తిరిగి cat_ఉపసర్గ_varname('v', (char_u *)vimvars[vidx++].vv_name);

    VIM_CLEAR(varnamebuf);
    varnamebuflen = 0;
    తిరిగి ఖాళీ;
}

    అక్షరం *
get_var_special_name(పూర్ణాంకం nr)
{
    ఎంపిక (nr)
    {
	స్థితి VVAL_FALSE: తిరిగి లోపల_vim9script() ? "false" : "v:false";
	స్థితి VVAL_TRUE:  తిరిగి లోపల_vim9script() ? "true" : "v:true";
	స్థితి VVAL_NULL:  తిరిగి లోపల_vim9script() ? "null" : "v:null";
	స్థితి VVAL_NONE:  తిరిగి "v:none";
    }
    అంతర్గత_error("get_var_special_name()");
    తిరిగి "42";
}

/*
 * Returns the global variable dictionary
 */
    dict_T *
get_globvar_dict(శూన్య)
{
    తిరిగి &globvardict;
}

/*
 * Returns the global variable hash table
 */
    hashtab_T *
get_globvar_ht(శూన్య)
{
    తిరిగి &globvarht;
}

/*
 * Returns the v: variable dictionary
 */
    dict_T *
get_vimvar_dict(శూన్య)
{
    తిరిగి &vimvardict;
}

/*
 * Returns the index of a v:variable.  Negative if not found.
 * Returns DI_ flags in "di_flags".
 */
    పూర్ణాంకం
కనుగొను_vim_var(char_u *name, పూర్ణాంకం *di_flags)
{
    dictitem_T	    *di = కనుగొను_var_లోపల_ht(&vimvarht, 0, name, TRUE);
    నిర్మాణం vimvar   *vv;

    ఒకవేళ (di == ఖాళీ)
	తిరిగి -1;
    *di_flags = di->di_flags;
    vv = (నిర్మాణం vimvar *)((అక్షరం *)di - offsetof(vimvar_T, vv_di));
    తిరిగి (పూర్ణాంకం)(vv - vimvars);
}


/*
 * Set type of v: variable to "type".
 */
    శూన్య
సమితి_vim_var_type(పూర్ణాంకం idx, vartype_T type)
{
    vimvars[idx].vv_tv_type = type;
}

/*
 * Set number v: variable to "val".
 * Note that this does not set the type, use set_vim_var_type() for that.
 */
    శూన్య
సమితి_vim_var_nr(పూర్ణాంకం idx, varnumber_T val)
{
    vimvars[idx].vv_nr = val;
}

    అక్షరం *
get_vim_var_name(పూర్ణాంకం idx)
{
    తిరిగి vimvars[idx].vv_name;
}

/*
 * Get typval_T v: variable value.
 */
    typval_T *
get_vim_var_tv(పూర్ణాంకం idx)
{
    తిరిగి &vimvars[idx].vv_tv;
}

    type_T *
get_vim_var_type(పూర్ణాంకం idx, garray_T *type_list)
{
    ఒకవేళ (vimvars[idx].vv_type != ఖాళీ)
	తిరిగి vimvars[idx].vv_type;
    తిరిగి typval2type_vimvar(&vimvars[idx].vv_tv, type_list);
}

/*
 * Add what getinfo() reports for the v: variable "name", without the "v:", to
 * "d".  Returns FAIL when there is no such variable.
 */
    పూర్ణాంకం
vim_var_info(char_u *name, dict_T *d)
{
    పూర్ణాంకం		di_flags;
    పూర్ణాంకం		idx = కనుగొను_vim_var(name, &di_flags);
    garray_T	type_list;
    అక్షరం	*tofree;

    ఒకవేళ (idx < 0)
	తిరిగి FAIL;
    vim_snprintf((అక్షరం *)IObuff, IOSIZE, "v:%s", vimvars[idx].vv_name);
    dict_కూడు_string(d, "name", IObuff);
    dict_కూడు_bool(d, "available", TRUE);
    ga_init2(&type_list, పరిమాణం(type_T *), 10);
    అక్షరం *type = type_name(get_vim_var_type(idx, &type_list), &tofree);
    dict_కూడు_string(d, "type", (char_u *)type);
    vim_free(tofree);
    clear_type_list(&type_list);
    dict_కూడు_bool(d, "readonly", (vimvars[idx].vv_flags & VV_RO) != 0);
    dict_కూడు_bool(d, "compat", (vimvars[idx].vv_flags & VV_COMPAT) != 0);
    తిరిగి OK;
}

/*
 * Set v: variable to "tv".  Only accepts the same type.
 * Takes over the value of "tv".
 */
    పూర్ణాంకం
సమితి_vim_var_tv(పూర్ణాంకం idx, typval_T *tv)
{
    ఒకవేళ (vimvars[idx].vv_tv_type != tv->v_type)
    {
	emsg(_(e_type_mismatch_for_v_variable));
	clear_tv(tv);
	తిరిగి FAIL;
    }
    // VV_RO is also checked when compiling, but let's check here as well.
    ఒకవేళ (vimvars[idx].vv_flags & VV_RO)
    {
	semsg(_(e_cannot_change_readonly_variable_str), vimvars[idx].vv_name);
	తిరిగి FAIL;
    }
    ఒకవేళ (sandbox && (vimvars[idx].vv_flags & VV_RO_SBX))
    {
	semsg(_(e_cannot_సమితి_variable_లోపల_sandbox_str), vimvars[idx].vv_name);
	తిరిగి FAIL;
    }
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_di.di_tv = *tv;
    తిరిగి OK;
}

/*
 * Get number v: variable value.
 */
    varnumber_T
get_vim_var_nr(పూర్ణాంకం idx)
{
    తిరిగి vimvars[idx].vv_nr;
}

/*
 * Get string v: variable value.  Uses a static buffer, can only be used once.
 * If the String variable has never been set, return an empty string.
 * Never returns NULL;
 */
    char_u *
get_vim_var_str(పూర్ణాంకం idx)
{
    తిరిగి tv_get_string(&vimvars[idx].vv_tv);
}

/*
 * Get List v: variable value.  Caller must take care of reference count when
 * needed.
 */
    list_T *
get_vim_var_list(పూర్ణాంకం idx)
{
    తిరిగి vimvars[idx].vv_list;
}

/*
 * Get Dict v: variable value.  Caller must take care of reference count when
 * needed.
 */
    dict_T *
get_vim_var_dict(పూర్ణాంకం idx)
{
    తిరిగి vimvars[idx].vv_dict;
}

/*
 * Set v:char to character "c".
 */
    శూన్య
సమితి_vim_var_char(పూర్ణాంకం c)
{
    char_u  buf[MB_MAXBYTES + 1];
    size_t  buflen;

    ఒకవేళ (has_mbyte)
	buflen = (*mb_char2bytes)(c, buf);
    లేకపోతే
    {
	buf[0] = c;
	buflen = 1;
    }
    buf[buflen] = NUL;

    సమితి_vim_var_string(VV_CHAR, buf, (పూర్ణాంకం)buflen);
}

/*
 * Set v:count to "count" and v:count1 to "count1".
 * When "set_prevcount" is TRUE first set v:prevcount from v:count.
 */
    శూన్య
సమితి_vcount(
    దీర్ఘ	లెక్క,
    దీర్ఘ	count1,
    పూర్ణాంకం		సమితి_prevcount)
{
    ఒకవేళ (సమితి_prevcount)
	vimvars[VV_PREVCOUNT].vv_nr = vimvars[VV_లెక్క].vv_nr;
    vimvars[VV_లెక్క].vv_nr = లెక్క;
    vimvars[VV_COUNT1].vv_nr = count1;
}

/*
 * Save variables that might be changed as a side effect.  Used when executing
 * a timer callback.
 */
    శూన్య
save_vimvars(vimvars_save_T *vvsave)
{
    vvsave->vv_prevcount = vimvars[VV_PREVCOUNT].vv_nr;
    vvsave->vv_లెక్క = vimvars[VV_లెక్క].vv_nr;
    vvsave->vv_count1 = vimvars[VV_COUNT1].vv_nr;
}

/*
 * Restore variables saved by save_vimvars().
 */
    శూన్య
restore_vimvars(vimvars_save_T *vvsave)
{
    vimvars[VV_PREVCOUNT].vv_nr = vvsave->vv_prevcount;
    vimvars[VV_లెక్క].vv_nr = vvsave->vv_లెక్క;
    vimvars[VV_COUNT1].vv_nr = vvsave->vv_count1;
}

/*
 * Set string v: variable to a copy of "val". If 'copy' is FALSE, then set the
 * value.
 */
    శూన్య
సమితి_vim_var_string(
    పూర్ణాంకం		idx,
    char_u	*val,
    పూర్ణాంకం		len)	    // length of "val" to use or -1 (whole string)
{
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_tv_type = VAR_STRING;
    ఒకవేళ (val == ఖాళీ)
	vimvars[idx].vv_str = ఖాళీ;
    లేకపోతే ఒకవేళ (len == -1)
	vimvars[idx].vv_str = vim_strsave(val);
    లేకపోతే
	vimvars[idx].vv_str = vim_strnsave(val, len);
}

    శూన్య
సమితి_vim_var_string_ప్రత్యక్ష(
    పూర్ణాంకం		idx,
    char_u	*val)
{
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_tv_type = VAR_STRING;

    vimvars[idx].vv_str = val;
}

/*
 * Set List v: variable to "val".
 */
    శూన్య
సమితి_vim_var_list(పూర్ణాంకం idx, list_T *val)
{
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_tv_type = VAR_LIST;
    vimvars[idx].vv_list = val;
    ఒకవేళ (val != ఖాళీ)
	++val->lv_refcount;
}

/*
 * Set Dictionary v: variable to "val".
 */
    శూన్య
సమితి_vim_var_dict(పూర్ణాంకం idx, dict_T *val)
{
    clear_tv(&vimvars[idx].vv_di.di_tv);
    vimvars[idx].vv_tv_type = VAR_DICT;
    vimvars[idx].vv_dict = val;
    ఒకవేళ (val == ఖాళీ)
	తిరిగి;

    ++val->dv_refcount;
    dict_సమితి_వస్తువులు_ro(val);
}

/*
 * Set the v:argv list.
 */
    శూన్య
సమితి_argv_var(అక్షరం **argv, పూర్ణాంకం argc)
{
    list_T	*l = list_alloc();
    పూర్ణాంకం		i;

    ఒకవేళ (l == ఖాళీ)
	getout(1);
    l->lv_తాళం = VAR_FIXED;
    కొరకు (i = 0; i < argc; ++i)
    {
	ఒకవేళ (list_append_string(l, (char_u *)argv[i], -1) == FAIL)
	    getout(1);
	l->lv_u.mat.lv_చివరి->li_tv.v_తాళం = VAR_FIXED;
    }
    సమితి_vim_var_list(VV_ARGV, l);
}

/*
 * Reset v:register, taking the 'clipboard' setting into account.
 */
    శూన్య
reset_reg_var(శూన్య)
{
    పూర్ణాంకం regname = 0;

    // Adjust the register according to 'clipboard', so that when
    // "unnamed" is present it becomes '*' or '+' instead of '"'.
#నిర్వచితమైతే HAVE_CLIPMETHOD
    adjust_clip_reg(&regname);
#అయితే_ముగింపు
    సమితి_reg_var(regname);
}

/*
 * Set v:register if needed.
 */
    శూన్య
సమితి_reg_var(పూర్ణాంకం c)
{
    char_u  regname[2];

    ఒకవేళ (c == 0 || c == ' ')
	regname[0] = '"';
    లేకపోతే
	regname[0] = c;
    regname[1] = NUL;
    // Avoid free/alloc when the value is already right.
    ఒకవేళ (vimvars[VV_REG].vv_str == ఖాళీ || vimvars[VV_REG].vv_str[0] != c)
	సమితి_vim_var_string(VV_REG, regname, 1);
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
    ఒకవేళ (oldval == ఖాళీ)
	తిరిగి vimvars[VV_EXCEPTION].vv_str;

    vimvars[VV_EXCEPTION].vv_str = oldval;
    తిరిగి ఖాళీ;
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
    ఒకవేళ (oldval == ఖాళీ)
	తిరిగి vimvars[VV_THROWPOINT].vv_str;

    vimvars[VV_THROWPOINT].vv_str = oldval;
    తిరిగి ఖాళీ;
}

/*
 * Set v:cmdarg.
 * If "eap" != NULL, use "eap" to generate the value and return the old value.
 * If "oldarg" != NULL, restore the value to "oldarg" and return NULL.
 * Must always be called in pairs!
 */
    char_u *
సమితి_cmdarg(exarg_T *eap, char_u *oldarg)
{
    char_u	*oldval;
    char_u	*newval;
    గుర్తులేని	len;

    oldval = vimvars[VV_CMDARG].vv_str;
    ఒకవేళ (eap == ఖాళీ)
    {
	vim_free(oldval);
	vimvars[VV_CMDARG].vv_str = oldarg;
	తిరిగి ఖాళీ;
    }

    ఒకవేళ (eap->force_bin == FORCE_BIN)
	len = 6;
    లేకపోతే ఒకవేళ (eap->force_bin == FORCE_NOBIN)
	len = 8;
    లేకపోతే
	len = 0;

    ఒకవేళ (eap->read_edit)
	len += 7;

    ఒకవేళ (eap->force_ff != 0)
	len += 10; // " ++ff=unix"
    ఒకవేళ (eap->force_enc != 0)
	len += (గుర్తులేని)STRLEN(eap->cmd + eap->force_enc) + 7;
    ఒకవేళ (eap->bad_char != 0)
	len += 7 + 4;  // " ++bad=" + "keep" or "drop"

    newval = alloc(len + 1);
    ఒకవేళ (newval == ఖాళీ)
	తిరిగి ఖాళీ;

    ఒకవేళ (eap->force_bin == FORCE_BIN)
	sprintf((అక్షరం *)newval, " ++bin");
    లేకపోతే ఒకవేళ (eap->force_bin == FORCE_NOBIN)
	sprintf((అక్షరం *)newval, " ++nobin");
    లేకపోతే
	*newval = NUL;

    ఒకవేళ (eap->read_edit)
	STRCAT(newval, " ++edit");

    ఒకవేళ (eap->force_ff != 0)
	sprintf((అక్షరం *)newval + STRLEN(newval), " ++ff=%s",
						eap->force_ff == 'u' ? "unix"
						: eap->force_ff == 'd' ? "dos"
						: "mac");
    ఒకవేళ (eap->force_enc != 0)
	sprintf((అక్షరం *)newval + STRLEN(newval), " ++enc=%s",
					       eap->cmd + eap->force_enc);
    ఒకవేళ (eap->bad_char == BAD_KEEP)
	STRCPY(newval + STRLEN(newval), " ++bad=keep");
    లేకపోతే ఒకవేళ (eap->bad_char == BAD_DROP)
	STRCPY(newval + STRLEN(newval), " ++bad=drop");
    లేకపోతే ఒకవేళ (eap->bad_char != 0)
	sprintf((అక్షరం *)newval + STRLEN(newval), " ++bad=%c", eap->bad_char);
    vimvars[VV_CMDARG].vv_str = newval;
    తిరిగి oldval;
}

/*
 * Get the value of internal variable "name".
 * If "flags" has EVAL_VAR_IMPORT may return a VAR_ANY with v_number set to the
 * imported script ID.
 * Return OK or FAIL.  If OK is returned "rettv" must be cleared.
 */
    పూర్ణాంకం
eval_variable(
    char_u	*name,
    పూర్ణాంకం		len,		// length of "name" or zero
    scid_T	sid,		// script ID for imported item or zero
    typval_T	*rettv,		// NULL when only checking existence
    dictitem_T	**dip,		// non-NULL when typval's dict item is needed
    పూర్ణాంకం		flags)		// EVAL_VAR_ flags
{
    పూర్ణాంకం		ret = OK;
    typval_T	*tv = ఖాళీ;
    పూర్ణాంకం		దొరికింది = FALSE;
    hashtab_T	*ht = ఖాళీ;
    పూర్ణాంకం		cc = 0;
    type_T	*type = ఖాళీ;

    ఒకవేళ (len > 0)
    {
	// truncate the name, so that we can use strcmp()
	cc = name[len];
	name[len] = NUL;
    }

    // Check for local variable when debugging.
    ఒకవేళ ((sid == 0) && (tv = lookup_debug_var(name)) == ఖాళీ)
    {
	// Check for user-defined variables.
	dictitem_T	*v = కనుగొను_var(name, &ht, flags & EVAL_VAR_NOAUTOLOAD);

	ఒకవేళ (v != ఖాళీ)
	{
	    tv = &v->di_tv;
	    ఒకవేళ (dip != ఖాళీ)
		*dip = v;
	}
	లేకపోతే
	    ht = ఖాళీ;
    }

    ఒకవేళ (tv == ఖాళీ && (లోపల_vim9script() || STRNCMP(name, "s:", 2) == 0))
    {
	imported_T  *import = ఖాళీ;
	char_u	    *p = STRNCMP(name, "s:", 2) == 0 ? name + 2 : name;

	ఒకవేళ (sid == 0)
	    import = కనుగొను_imported(p, 0, TRUE);

	// imported variable from another script
	ఒకవేళ (import != ఖాళీ || sid != 0)
	{
	    ఒకవేళ ((flags & EVAL_VAR_IMPORT) == 0)
	    {
		ఒకవేళ (SCRIPT_ID_VALID(sid))
		{
		    ht = &SCRIPT_VARS(sid);
		    ఒకవేళ (ht != ఖాళీ)
		    {
			dictitem_T *v = కనుగొను_var_లోపల_ht(ht, 0, name,
						  flags & EVAL_VAR_NOAUTOLOAD);

			ఒకవేళ (v == ఖాళీ)
			    v = కనుగొను_var_autoload_ఉపసర్గ(name, sid, ఖాళీ, ఖాళీ);

			ఒకవేళ (v != ఖాళీ)
			{
			    tv = &v->di_tv;
			    ఒకవేళ (dip != ఖాళీ)
				*dip = v;
			}
			లేకపోతే
			    ht = ఖాళీ;
		    }
		}
		లేకపోతే
		{
		    ఒకవేళ (flags & EVAL_VAR_VERBOSE)
			semsg(_(e_expected_dot_after_name_str), name);
		    ret = FAIL;
		}
	    }
	    లేకపోతే
	    {
		ఒకవేళ (rettv != ఖాళీ)
		{
		    // special value that is used in handle_subscript()
		    rettv->v_type = VAR_ANY;
		    rettv->vval.v_సంఖ్య = sid != 0 ? sid : import->imp_sid;
		}
		దొరికింది = TRUE;
	    }
	}
	లేకపోతే ఒకవేళ (లోపల_vim9script() && (flags & EVAL_VAR_NO_FUNC) == 0)
	{
	    పూర్ణాంకం	    has_g_ఉపసర్గ = STRNCMP(name, "g:", 2) == 0;
	    ufunc_T *ufunc = కనుగొను_func(name, FALSE);

	    ఒకవేళ (ufunc != ఖాళీ && cc == '<')
	    {
		// handle generic function
		char_u	*argp = name + len;
		name[len] = cc;
		ufunc = eval_generic_func(ufunc, name, &argp);
		name[len] = NUL;
		ఒకవేళ (ufunc == ఖాళీ)
		{
		    ret = FAIL;
		    వెళ్ళు పూర్తి;
		}
	    }

	    // In Vim9 script we can get a function reference by using the
	    // function name.  For a global non-autoload function "g:" is
	    // required.
	    ఒకవేళ (ufunc != ఖాళీ && (has_g_ఉపసర్గ
					    || !func_requires_g_ఉపసర్గ(ufunc)))
	    {
		దొరికింది = TRUE;
		ఒకవేళ (rettv != ఖాళీ)
		{
		    rettv->v_type = VAR_FUNC;
		    ఒకవేళ (has_g_ఉపసర్గ)
		    {
			// Keep the "g:", otherwise script-local may be
			// assumed.
			ఒకవేళ (cc != '<')
			    rettv->vval.v_string = vim_strsave(name);
			లేకపోతే
			{
			    // append the generic function arguments
			    char_u	*argp = name + len;
			    name[len] = cc;
			    rettv->vval.v_string =
				append_generic_func_type_వాదనలు(name, len,
									&argp);
			    name[len] = NUL;
			}
		    }
		    లేకపోతే
			rettv->vval.v_string = vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
		    ఒకవేళ (rettv->vval.v_string != ఖాళీ)
			func_ref(ufunc->uf_name);
		}
	    }
	}
    }

    ఒకవేళ (!దొరికింది)
    {
	ఒకవేళ (tv == ఖాళీ)
	{
	    ఒకవేళ (rettv != ఖాళీ && (flags & EVAL_VAR_VERBOSE))
		semsg(_(e_undefined_variable_str), name);
	    ret = FAIL;
	}
	లేకపోతే ఒకవేళ (rettv != ఖాళీ)
	{
	    svar_T  *sv = ఖాళీ;
	    పూర్ణాంకం	    was_assigned = FALSE;

	    ఒకవేళ (ht != ఖాళీ && ht == get_script_local_ht()
		    && tv != &SCRIPT_SV(current_sctx.sc_sid)->sv_var.di_tv)
	    {
		sv = కనుగొను_typval_లోపల_script(tv, 0, TRUE);
		ఒకవేళ (sv != ఖాళీ)
		{
		    type = sv->sv_type;
		    was_assigned = sv->sv_flags & SVFLAG_ASSIGNED;
		}
	    }

	    ఒకవేళ ((tv->v_type == VAR_TYPEALIAS || tv->v_type == VAR_CLASS)
		    && sid != 0)
	    {
		// type alias or class imported from another script.  Check
		// whether it is exported from the other script.
		sv = కనుగొను_typval_లోపల_script(tv, sid, TRUE);
		ఒకవేళ (sv == ఖాళీ)
		{
		    ret = FAIL;
		    వెళ్ళు పూర్తి;
		}
		ఒకవేళ ((sv->sv_flags & SVFLAG_EXPORTED) == 0)
		{
		    semsg(_(e_అంశం_not_exported_లోపల_script_str), name);
		    ret = FAIL;
		    వెళ్ళు పూర్తి;
		}
	    }

	    // If a list or tuple or dict variable wasn't initialized and has
	    // meaningful type, do it now.  Not for global variables, they are
	    // not declared.
	    ఒకవేళ (ht != &globvarht)
	    {
		ఒకవేళ (tv->v_type == VAR_DICT && tv->vval.v_dict == ఖాళీ
					    && ((type != ఖాళీ && !was_assigned)
							  || !లోపల_vim9script()))
		{
		    tv->vval.v_dict = dict_alloc();
		    ఒకవేళ (tv->vval.v_dict != ఖాళీ)
		    {
			++tv->vval.v_dict->dv_refcount;
			tv->vval.v_dict->dv_type = alloc_type(type);
			ఒకవేళ (sv != ఖాళీ)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
		లేకపోతే ఒకవేళ (tv->v_type == VAR_LIST && tv->vval.v_list == ఖాళీ
					    && ((type != ఖాళీ && !was_assigned)
							  || !లోపల_vim9script()))
		{
		    tv->vval.v_list = list_alloc();
		    ఒకవేళ (tv->vval.v_list != ఖాళీ)
		    {
			++tv->vval.v_list->lv_refcount;
			tv->vval.v_list->lv_type = alloc_type(type);
			ఒకవేళ (sv != ఖాళీ)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
		లేకపోతే ఒకవేళ (tv->v_type == VAR_TUPLE && tv->vval.v_tuple == ఖాళీ
					    && ((type != ఖాళీ && !was_assigned)
							  || !లోపల_vim9script()))
		{
		    tv->vval.v_tuple = tuple_alloc();
		    ఒకవేళ (tv->vval.v_tuple != ఖాళీ)
		    {
			++tv->vval.v_tuple->tv_refcount;
			tv->vval.v_tuple->tv_type = alloc_type(type);
			ఒకవేళ (sv != ఖాళీ)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
		లేకపోతే ఒకవేళ (tv->v_type == VAR_BLOB && tv->vval.v_blob == ఖాళీ
					    && ((type != ఖాళీ && !was_assigned)
							  || !లోపల_vim9script()))
		{
		    tv->vval.v_blob = blob_alloc();
		    ఒకవేళ (tv->vval.v_blob != ఖాళీ)
		    {
			++tv->vval.v_blob->bv_refcount;
			ఒకవేళ (sv != ఖాళీ)
			    sv->sv_flags |= SVFLAG_ASSIGNED;
		    }
		}
	    }
	    ret = copy_tv(tv, rettv);
	}
    }

పూర్తి:
    ఒకవేళ (len > 0)
	name[len] = cc;

    తిరిగి ret;
}

/*
 * Get the value of internal variable "name", also handling "import.name".
 * Return OK or FAIL.  If OK is returned "rettv" must be cleared.
 */
    పూర్ణాంకం
eval_variable_import(
    char_u	*name,
    typval_T	*rettv)
{
    char_u  *s = name;
    అయితే (ASCII_ISALNUM(*s) || *s == '_')
	++s;
    పూర్ణాంకం	    len = (పూర్ణాంకం)(s - name);

    ఒకవేళ (eval_variable(name, len, 0, rettv, ఖాళీ, EVAL_VAR_IMPORT) == FAIL)
	తిరిగి FAIL;
    ఒకవేళ (rettv->v_type == VAR_ANY && *s == '.')
    {
	char_u *ns = s + 1;
	s = ns;
	అయితే (ASCII_ISALNUM(*s) || *s == '_')
	    ++s;
	పూర్ణాంకం sid = rettv->vval.v_సంఖ్య;
	తిరిగి eval_variable(ns, (పూర్ణాంకం)(s - ns), sid, rettv, ఖాళీ, 0);
    }
    తిరిగి OK;
}


/*
 * Check if variable "name[len]" is a local variable or an argument.
 * If so, "*eval_lavars_used" is set to TRUE.
 */
    శూన్య
check_vars(char_u *name, పూర్ణాంకం len)
{
    పూర్ణాంకం		cc;
    char_u	*varname;
    hashtab_T	*ht;

    ఒకవేళ (eval_lavars_వాడిన == ఖాళీ)
	తిరిగి;

    // truncate the name, so that we can use strcmp()
    cc = name[len];
    name[len] = NUL;

    ht = కనుగొను_var_ht(name, &varname);
    ఒకవేళ (ht == get_funccal_local_ht() || ht == get_funccal_వాదనలు_ht())
    {
	ఒకవేళ (కనుగొను_var(name, ఖాళీ, TRUE) != ఖాళీ)
	    *eval_lavars_వాడిన = TRUE;
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
కనుగొను_var(char_u *name, hashtab_T **htp, పూర్ణాంకం no_autoload)
{
    char_u	*varname;
    hashtab_T	*ht;
    dictitem_T	*ret = ఖాళీ;

    ht = కనుగొను_var_ht(name, &varname);
    ఒకవేళ (htp != ఖాళీ)
	*htp = ht;
    ఒకవేళ (ht == ఖాళీ)
	తిరిగి ఖాళీ;
    ret = కనుగొను_var_లోపల_ht(ht, *name, varname, no_autoload);
    ఒకవేళ (ret != ఖాళీ)
	తిరిగి ret;

    // Search in parent scope for lambda
    ret = కనుగొను_var_లోపల_scoped_ht(name, no_autoload);
    ఒకవేళ (ret != ఖాళీ)
	తిరిగి ret;

    // in Vim9 script items without a scope can be script-local
    ఒకవేళ (లోపల_vim9script() && name[0] != NUL && name[1] != ':')
    {
	ht = get_script_local_ht();
	ఒకవేళ (ht != ఖాళీ)
	{
	    ret = కనుగొను_var_లోపల_ht(ht, *name, varname, no_autoload);
	    ఒకవేళ (ret != ఖాళీ)
	    {
		ఒకవేళ (htp != ఖాళీ)
		    *htp = ht;
		తిరిగి ret;
	    }
	}
    }

    // and finally try
    తిరిగి కనుగొను_var_autoload_ఉపసర్గ(name, 0, htp, ఖాళీ);
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
కనుగొను_var_autoload_ఉపసర్గ(char_u *name, పూర్ణాంకం sid, hashtab_T **htp,
							    char_u **namep)
{
    hashtab_T	*ht;
    dictitem_T	*ret = ఖాళీ;
    // When using "vim9script autoload" script-local items are prefixed but can
    // be used with s:name.
    పూర్ణాంకం check_sid = sid > 0 ? sid : current_sctx.sc_sid;
    ఒకవేళ (SCRIPT_ID_VALID(check_sid)
		   && (లోపల_vim9script() || (name[0] == 's' && name[1] == ':')))
    {
	scriptitem_T *si = SCRIPT_అంశం(check_sid);

	ఒకవేళ (si->sn_autoload_ఉపసర్గ != ఖాళీ)
	{
	    char_u *పునాది_name = (name[0] == 's' && name[1] == ':')
							     ? name + 2 : name;
	    char_u *auto_name = concat_str(si->sn_autoload_ఉపసర్గ, పునాది_name);

	    ఒకవేళ (auto_name != ఖాళీ)
	    {
		పూర్ణాంకం free_auto_name = TRUE;
		ht = &globvarht;
		ret = కనుగొను_var_లోపల_ht(ht, 'g', auto_name, TRUE);
		ఒకవేళ (ret != ఖాళీ)
		{
		    ఒకవేళ (htp != ఖాళీ)
			*htp = ht;
		    ఒకవేళ (namep != ఖాళీ)
		    {
			free_auto_name = FALSE;
			*namep = auto_name;
		    }
		}
		ఒకవేళ (free_auto_name)
		    vim_free(auto_name);
	    }
	}
    }

    తిరిగి ret;
}

/*
 * Like find_var() but if the name starts with <SNR>99_ then look in the
 * referenced script (used for a funcref).
 */
    dictitem_T *
కనుగొను_var_also_లోపల_script(char_u *name, hashtab_T **htp, పూర్ణాంకం no_autoload)
{
    ఒకవేళ (STRNCMP(name, "<SNR>", 5) == 0 && సురక్షిత_isdigit(name[5]))
    {
	char_u	    *p = name + 5;
	పూర్ణాంకం	    sid = getdigits(&p);

	ఒకవేళ (SCRIPT_ID_VALID(sid) && *p == '_')
	{
	    hashtab_T	*ht = &SCRIPT_VARS(sid);

	    ఒకవేళ (ht != ఖాళీ)
	    {
		dictitem_T *di = కనుగొను_var_లోపల_ht(ht, 0, p + 1, no_autoload);

		ఒకవేళ (di != ఖాళీ)
		{
		    ఒకవేళ (htp != ఖాళీ)
			*htp = ht;
		    తిరిగి di;
		}
	    }
	}
    }

    తిరిగి కనుగొను_var(name, htp, no_autoload);
}

/*
 * Find variable "varname" in hashtab "ht" with name "htname".
 * When "varname" is empty returns curwin/curtab/etc vars dictionary.
 * Returns NULL if not found.
 */
    dictitem_T *
కనుగొను_var_లోపల_ht(
    hashtab_T	*ht,
    పూర్ణాంకం		htname,
    char_u	*varname,
    పూర్ణాంకం		no_autoload)
{
    hashitem_T	*hi;

    ఒకవేళ (*varname == NUL)
    {
	// Must be something like "s:", otherwise "ht" would be NULL.
	ఎంపిక (htname)
	{
	    స్థితి 's': తిరిగి &SCRIPT_SV(current_sctx.sc_sid)->sv_var;
	    స్థితి 'g': తిరిగి &globvars_var;
	    స్థితి 'v': తిరిగి &vimvars_var;
	    స్థితి 'b': తిరిగి &curbuf->b_bufvar;
	    స్థితి 'w': తిరిగి &curwin->w_winvar;
	    స్థితి 't': తిరిగి &curtab->tp_winvar;
	    స్థితి 'l': తిరిగి get_funccal_local_var();
	    స్థితి 'a': తిరిగి get_funccal_వాదనలు_var();
	}
	తిరిగి ఖాళీ;
    }

    hi = hash_కనుగొను(ht, varname);
    ఒకవేళ (HASHITEM_ఖాళీలు(hi))
    {
	// For global variables we may try auto-loading the script.  If it
	// worked find the variable again.  Don't auto-load a script if it was
	// loaded already, otherwise it would be loaded every time when
	// checking if a function name is a Funcref variable.
	ఒకవేళ (ht == &globvarht && !no_autoload)
	{
	    // Note: script_autoload() may make "hi" invalid. It must either
	    // be obtained again or not used.
	    ఒకవేళ (!script_autoload(varname, FALSE) || aborting())
		తిరిగి ఖాళీ;
	    hi = hash_కనుగొను(ht, varname);
	}
	ఒకవేళ (HASHITEM_ఖాళీలు(hi))
	    తిరిగి ఖాళీ;
    }
    తిరిగి HI2DI(hi);
}

/*
 * Get the script-local hashtab.  NULL if not in a script context.
 */
    hashtab_T *
get_script_local_ht(శూన్య)
{
    scid_T sid = current_sctx.sc_sid;

    ఒకవేళ (SCRIPT_ID_VALID(sid))
	తిరిగి &SCRIPT_VARS(sid);
    తిరిగి ఖాళీ;
}

/*
 * Look for "name[len]" in script-local variables and functions.
 * When "cmd" is TRUE it must look like a command, a function must be followed
 * by "(" or "->".
 * Return OK when found, FAIL when not found.
 */
    పూర్ణాంకం
lookup_scriptitem(
	char_u	*name,
	size_t	len,
	పూర్ణాంకం	cmd,
	cctx_T	*dummy వాడని)
{
    hashtab_T	*ht = get_script_local_ht();
    char_u	బఫరు[30];
    char_u	*p;
    పూర్ణాంకం		res;
    hashitem_T	*hi;
    పూర్ణాంకం		is_global = FALSE;
    char_u	*fname = name;

    ఒకవేళ (ht == ఖాళీ)
	తిరిగి FAIL;
    ఒకవేళ (len < పరిమాణం(బఫరు) - 1)
    {
	// avoid an alloc/free for short names
	vim_strncpy(బఫరు, name, len);
	p = బఫరు;
    }
    లేకపోతే
    {
	p = vim_strnsave(name, len);
	ఒకవేళ (p == ఖాళీ)
	    తిరిగి FAIL;
    }

    hi = hash_కనుగొను(ht, p);
    res = HASHITEM_ఖాళీలు(hi) ? FAIL : OK;

    // if not script-local, then perhaps autoload-exported
    ఒకవేళ (res == FAIL && కనుగొను_var_autoload_ఉపసర్గ(p, 0, ఖాళీ, ఖాళీ) != ఖాళీ)
	res = OK;

    // if not script-local or autoload, then perhaps imported
    ఒకవేళ (res == FAIL && కనుగొను_imported(p, 0, FALSE) != ఖాళీ)
	res = OK;
    ఒకవేళ (p != బఫరు)
	vim_free(p);

    // Find a function, so that a following "->" works.
    // When used as a command require "(" or "->" to follow, "Cmd" is a user
    // command while "Cmd()" is a function call.
    ఒకవేళ (res != OK)
    {
	p = skipwhite(name + len);

	ఒకవేళ (!cmd || name[len] == '(' || (p[0] == '-' && p[1] == '>'))
	{
	    // Do not check for an internal function, since it might also be a
	    // valid command, such as ":split" versus "split()".
	    // Skip "g:" before a function name.
	    ఒకవేళ (name[0] == 'g' && name[1] == ':')
	    {
		is_global = TRUE;
		fname = name + 2;
	    }
	    ఒకవేళ (కనుగొను_func(fname, is_global) != ఖాళీ)
		res = OK;
	}
    }

    తిరిగి res;
}

/*
 * Find the hashtab used for a variable name.
 * Return NULL if the name is not valid.
 * Set "varname" to the start of name without ':'.
 */
    hashtab_T *
కనుగొను_var_ht(char_u *name, char_u **varname)
{
    hashitem_T	*hi;
    hashtab_T	*ht;

    ఒకవేళ (name[0] == NUL)
	తిరిగి ఖాళీ;
    ఒకవేళ (name[1] != ':')
    {
	// The name must not start with a colon or #.
	ఒకవేళ (name[0] == ':' || name[0] == AUTOLOAD_CHAR)
	    తిరిగి ఖాళీ;
	*varname = name;

	// "version" is "v:version" in all scopes if scriptversion < 3.
	// Same for a few other variables marked with VV_COMPAT.
	ఒకవేళ (లోపల_old_script(3))
	{
	    hi = hash_కనుగొను(&compat_hashtab, name);
	    ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
		తిరిగి &compat_hashtab;
	}

	ht = get_funccal_local_ht();
	ఒకవేళ (ht != ఖాళీ)
	    తిరిగి ht;				// local variable

	// In Vim9 script items at the script level are script-local, except
	// for autoload names.
	ఒకవేళ (లోపల_vim9script() && vim_strchr(name, AUTOLOAD_CHAR) == ఖాళీ)
	{
	    ht = get_script_local_ht();
	    ఒకవేళ (ht != ఖాళీ)
		తిరిగి ht;
	}

	తిరిగి &globvarht;			// global variable
    }
    *varname = name + 2;
    ఒకవేళ (*name == 'g')				// global variable
	తిరిగి &globvarht;
    // There must be no ':' or '#' in the rest of the name, unless g: is used
    ఒకవేళ (vim_strchr(name + 2, ':') != ఖాళీ
			       || vim_strchr(name + 2, AUTOLOAD_CHAR) != ఖాళీ)
	తిరిగి ఖాళీ;
    ఒకవేళ (*name == 'b')				// buffer variable
	తిరిగి &curbuf->b_vars->dv_hashtab;
    ఒకవేళ (*name == 'w')				// window variable
	తిరిగి &curwin->w_vars->dv_hashtab;
    ఒకవేళ (*name == 't')				// tab page variable
	తిరిగి &curtab->tp_vars->dv_hashtab;
    ఒకవేళ (*name == 'v')				// v: variable
	తిరిగి &vimvarht;
    ఒకవేళ (get_current_funccal() != ఖాళీ
	       && get_current_funccal()->fc_func->uf_def_స్థితి
							    == UF_NOT_COMPILED)
    {
	// a: and l: are only used in functions defined with ":function"
	ఒకవేళ (*name == 'a')			// a: function argument
	    తిరిగి get_funccal_వాదనలు_ht();
	ఒకవేళ (*name == 'l')			// l: local function variable
	    తిరిగి get_funccal_local_ht();
    }
    ఒకవేళ (*name == 's')				// script variable
    {
	ht = get_script_local_ht();
	ఒకవేళ (ht != ఖాళీ)
	    తిరిగి ht;
    }
    తిరిగి ఖాళీ;
}

/*
 * Get the string value of a (global/local) variable.
 * Note: see tv_get_string() for how long the pointer remains valid.
 * Returns NULL when it doesn't exist.
 */
    char_u *
get_var_విలువ(char_u *name)
{
    dictitem_T	*v;

    v = కనుగొను_var(name, ఖాళీ, FALSE);
    ఒకవేళ (v == ఖాళీ)
	తిరిగి ఖాళీ;
    తిరిగి tv_get_string(&v->di_tv);
}

/*
 * Allocate a new hashtab for a sourced script.  It will be used while
 * sourcing this script and when executing functions defined in the script.
 */
    శూన్య
new_script_vars(scid_T id)
{
    scriptvar_T *sv;

    sv = ALLOC_CLEAR_ONE(scriptvar_T);
    ఒకవేళ (sv == ఖాళీ)
	తిరిగి;
    init_var_dict(&sv->sv_dict, &sv->sv_var, VAR_SCOPE);
    SCRIPT_అంశం(id)->sn_vars = sv;
}

/*
 * Initialize dictionary "dict" as a scope and set variable "dict_var" to
 * point to it.
 */
    శూన్య
init_var_dict(dict_T *dict, dictitem_T *dict_var, పూర్ణాంకం scope)
{
    hash_init(&dict->dv_hashtab);
    dict->dv_తాళం = 0;
    dict->dv_scope = scope;
    dict->dv_refcount = DO_NOT_FREE_CNT;
    dict->dv_copyID = 0;
    dict_var->di_tv.vval.v_dict = dict;
    dict_var->di_tv.v_type = VAR_DICT;
    dict_var->di_tv.v_తాళం = VAR_FIXED;
    dict_var->di_flags = DI_FLAGS_RO | DI_FLAGS_FIX;
    dict_var->di_కీలకం[0] = NUL;
}

/*
 * Unreference a dictionary initialized by init_var_dict().
 */
    శూన్య
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
    శూన్య
vars_clear(hashtab_T *ht)
{
    vars_clear_ext(ht, TRUE);
}

/*
 * Like vars_clear(), but only free the value if "free_val" is TRUE.
 */
    శూన్య
vars_clear_ext(hashtab_T *ht, పూర్ణాంకం free_val)
{
    పూర్ణాంకం		todo;
    hashitem_T	*hi;
    dictitem_T	*v;

    hash_తాళం(ht);
    todo = (పూర్ణాంకం)ht->ht_వాడిన;
    FOR_ALL_HASHTAB_వస్తువులు(ht, hi, todo)
    {
	ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
	{
	    --todo;

	    // Free the variable.  Don't remove it from the hashtab,
	    // ht_array might change then.  hash_clear() takes care of it
	    // later.
	    v = HI2DI(hi);
	    ఒకవేళ (free_val)
		clear_tv(&v->di_tv);
	    ఒకవేళ (v->di_flags & DI_FLAGS_ALLOC)
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
    శూన్య
delete_var(hashtab_T *ht, hashitem_T *hi)
{
    dictitem_T	*di = HI2DI(hi);

    ఒకవేళ (hash_remove(ht, hi, "delete variable") != OK)
	తిరిగి;

    clear_tv(&di->di_tv);
    vim_free(di);
}

/*
 * Delete the exported variables of a reloaded Vim9 autoload script.  They live
 * in the global namespace with the autoload prefix (e.g. "foo#bar") and are
 * recreated when the script body runs again.
 */
    శూన్య
delete_autoload_export_vars(char_u *ఉపసర్గ)
{
    ఒకవేళ (ఉపసర్గ == ఖాళీ)
	తిరిగి;
    size_t  prefixlen = STRLEN(ఉపసర్గ);

    hash_తాళం(&globvarht);
    పూర్ణాంకం	todo = (పూర్ణాంకం)globvarht.ht_వాడిన;

    కొరకు (hashitem_T *hi = globvarht.ht_array; todo > 0; ++hi)
	ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
	{
	    dictitem_T	*di = HI2DI(hi);

	    --todo;
	    // Keep a class or enum: existing objects still refer to it.
	    ఒకవేళ (di->di_tv.v_type != VAR_CLASS
		    && STRNCMP(di->di_కీలకం, ఉపసర్గ, prefixlen) == 0)
		delete_var(&globvarht, hi);
	}
    hash_unlock(&globvarht);
}

/*
 * List the value of one internal variable.
 */
    స్థిర శూన్య
list_one_var(dictitem_T *v, అక్షరం *ఉపసర్గ, పూర్ణాంకం *మొదటి)
{
    char_u	*tofree;
    char_u	*s;
    char_u	numbuf[NUMBUFLEN];

    s = echo_string(&v->di_tv, &tofree, numbuf, get_copyID());
    list_one_var_a(ఉపసర్గ, v->di_కీలకం, v->di_tv.v_type,
					 s == ఖాళీ ? (char_u *)"" : s, మొదటి);
    vim_free(tofree);
}

    స్థిర శూన్య
list_one_var_a(
    అక్షరం	*ఉపసర్గ,
    char_u	*name,
    పూర్ణాంకం		type,
    char_u	*string,
    పూర్ణాంకం		*మొదటి)  // when TRUE clear rest of screen and set to FALSE
{
    // don't use msg() or msg_attr() to avoid overwriting "v:statusmsg"
    సందేశం_ప్రారంభం();
    సందేశం_puts(ఉపసర్గ);
    ఒకవేళ (name != ఖాళీ)	// "a:" vars don't have a name stored
	సందేశం_puts((అక్షరం *)name);
    సందేశం_putchar(' ');
    సందేశం_advance(22);
    ఒకవేళ (type == VAR_సంఖ్య)
	సందేశం_putchar('#');
    లేకపోతే ఒకవేళ (type == VAR_FUNC || type == VAR_PARTIAL)
	సందేశం_putchar('*');
    లేకపోతే ఒకవేళ (type == VAR_LIST)
    {
	సందేశం_putchar('[');
	ఒకవేళ (*string == '[')
	    ++string;
    }
    లేకపోతే ఒకవేళ (type == VAR_DICT)
    {
	సందేశం_putchar('{');
	ఒకవేళ (*string == '{')
	    ++string;
    }
    లేకపోతే
	సందేశం_putchar(' ');

    సందేశం_outtrans(string);

    ఒకవేళ (type == VAR_FUNC || type == VAR_PARTIAL)
	సందేశం_puts("()");
    ఒకవేళ (*మొదటి)
    {
	సందేశం_clr_eos();
	*మొదటి = FALSE;
    }
}

/*
 * Addition handling for setting a v: variable.
 * Return TRUE if the variable should be set normally,
 *        FALSE if nothing else needs to be done.
 */
    పూర్ణాంకం
before_సమితి_vvar(
    char_u	*varname,
    dictitem_T	*di,
    typval_T	*tv,
    పూర్ణాంకం		copy,
    పూర్ణాంకం		*type_error)
{
    ఒకవేళ (di->di_tv.v_type == VAR_STRING)
    {
	VIM_CLEAR(di->di_tv.vval.v_string);
	ఒకవేళ (copy || tv->v_type != VAR_STRING)
	{
	    char_u *val = tv_get_string(tv);

	    // Careful: when assigning to v:errmsg and
	    // tv_get_string() causes an error message the variable
	    // will already be set.
	    ఒకవేళ (di->di_tv.vval.v_string == ఖాళీ)
		di->di_tv.vval.v_string = vim_strsave(val);
	}
	లేకపోతే
	{
	    // Take over the string to avoid an extra alloc/free.
	    di->di_tv.vval.v_string = tv->vval.v_string;
	    tv->vval.v_string = ఖాళీ;
	}
	తిరిగి FALSE;
    }
    లేకపోతే ఒకవేళ (di->di_tv.v_type == VAR_సంఖ్య)
    {
	di->di_tv.vval.v_సంఖ్య = tv_get_సంఖ్య(tv);
	ఒకవేళ (STRCMP(varname, "searchforward") == 0)
	    సమితి_శోధన_direction(di->di_tv.vval.v_సంఖ్య ? '/' : '?');
#నిర్వచితమైతే FEAT_శోధన_EXTRA
	లేకపోతే ఒకవేళ (STRCMP(varname, "hlsearch") == 0)
	{
	    no_hlsearch = !di->di_tv.vval.v_సంఖ్య;
	    redraw_all_later(UPD_SOME_VALID);
	}
#అయితే_ముగింపు
	తిరిగి FALSE;
    }
    లేకపోతే ఒకవేళ (di->di_tv.v_type != tv->v_type)
    {
	*type_error = TRUE;
	తిరిగి FALSE;
    }
    తిరిగి TRUE;
}

/*
 * Set variable "name" to value in "tv".
 * If the variable already exists, the value is updated.
 * Otherwise the variable is created.
 */
    శూన్య
సమితి_var(
    char_u	*name,
    typval_T	*tv,
    పూర్ణాంకం		copy)	    // make copy of value in "tv"
{
    సమితి_var_const(name, 0, ఖాళీ, tv, copy, కేటాయించు_DECL, 0);
}

/*
 * Set variable "name" to value in "tv_arg".
 * When "sid" is non-zero "name" is in the script with this ID.
 * If the variable already exists and "is_const" is FALSE the value is updated.
 * Otherwise the variable is created.
 */
    పూర్ణాంకం
సమితి_var_const(
    char_u	*name,
    scid_T	sid,
    type_T	*type_arg,
    typval_T	*tv_arg,
    పూర్ణాంకం		copy,	    // make copy of value in "tv"
    పూర్ణాంకం		flags_arg,  // ASSIGN_CONST, ASSIGN_FINAL, etc.
    పూర్ణాంకం		var_idx)    // index for ":let [a, b] = list"
{
    typval_T	*tv = tv_arg;
    type_T	*type = type_arg;
    typval_T	bool_tv;
    dictitem_T	*di;
    typval_T	*dest_tv;
    char_u	*varname;
    char_u	*name_tofree = ఖాళీ;
    hashtab_T	*ht = ఖాళీ;
    పూర్ణాంకం		is_script_local;
    పూర్ణాంకం		vim9script = లోపల_vim9script();
    పూర్ణాంకం		var_లోపల_vim9script;
    పూర్ణాంకం		var_లోపల_autoload = FALSE;
    పూర్ణాంకం		flags = flags_arg;
    పూర్ణాంకం		free_tv_arg = !copy;  // free tv_arg if not used
    పూర్ణాంకం		rc = FAIL;

    ఒకవేళ (sid != 0)
    {
	varname = ఖాళీ;
	ఒకవేళ (SCRIPT_ID_VALID(sid))
	{
	    char_u	*auto_name = ఖాళీ;
	    ఒకవేళ (కనుగొను_var_autoload_ఉపసర్గ(name, sid, &ht, &auto_name) != ఖాళీ)
	    {
		var_లోపల_autoload = TRUE;
		varname = auto_name;
		name_tofree = varname;
	    }
	    లేకపోతే
		ht = &SCRIPT_VARS(sid);
	}
	ఒకవేళ (varname == ఖాళీ)
	    varname = name;
    }
    లేకపోతే
    {
	scriptitem_T	*si;
	char_u		*auto_name = ఖాళీ;

	ఒకవేళ (లోపల_vim9script()
	    && SCRIPT_ID_VALID(current_sctx.sc_sid)
	    && (si = SCRIPT_అంశం(current_sctx.sc_sid))
					      ->sn_autoload_ఉపసర్గ != ఖాళీ
	    && (is_export
		|| కనుగొను_var_autoload_ఉపసర్గ(name, 0, ఖాళీ, &auto_name)
								    != ఖాళీ))
	{
	    // In a vim9 autoload script an exported variable is put in the
	    // global namespace with the autoload prefix.
	    var_లోపల_autoload = TRUE;
	    varname = auto_name != ఖాళీ ? auto_name
		      : concat_str(si->sn_autoload_ఉపసర్గ, name);
	    ఒకవేళ (varname == ఖాళీ)
		వెళ్ళు failed;
	    name_tofree = varname;
	    ht = &globvarht;
	}
	లేకపోతే
	    ht = కనుగొను_var_ht(name, &varname);
    }
    ఒకవేళ (ht == ఖాళీ || *varname == NUL)
    {
	semsg(_(e_illegal_variable_name_str), name);
	వెళ్ళు failed;
    }
    is_script_local = ht == get_script_local_ht() || sid != 0
							    || var_లోపల_autoload;

    ఒకవేళ (vim9script
	    && !is_script_local
	    && (flags & (కేటాయించు_NO_DECL | కేటాయించు_DECL)) == 0
	    && (flags & (కేటాయించు_CONST | కేటాయించు_FINAL)) == 0
	    && name[1] == ':')
    {
	vim9_declare_error(name);
	వెళ్ళు failed;
    }
    ఒకవేళ ((flags & కేటాయించు_FOR_LOOP) && is_scoped_variable(name))
	// Do not make g:var, w:var, b:var or t:var final.
	flags &= ~కేటాయించు_FINAL;

    var_లోపల_vim9script = is_script_local && current_script_is_vim9();
    ఒకవేళ (var_లోపల_vim9script && name[0] == '_' && name[1] == NUL)
    {
	// For "[a, _] = list" the underscore is ignored.
	ఒకవేళ ((flags & కేటాయించు_UNPACK) == 0)
	    emsg(_(e_cannot_use_underscore_ఇక్కడ));
	వెళ్ళు failed;
    }

    di = కనుగొను_var_లోపల_ht(ht, 0, varname, TRUE);

    ఒకవేళ (di == ఖాళీ && var_లోపల_vim9script)
    {
	imported_T  *import = కనుగొను_imported(varname, 0, FALSE);

	ఒకవేళ (import != ఖాళీ)
	{
	    // imported name space cannot be used
	    ఒకవేళ ((flags & కేటాయించు_NO_DECL) == 0)
	    {
		semsg(_(e_redefining_imported_అంశం_str), name);
		వెళ్ళు failed;
	    }
	    semsg(_(e_cannot_use_str_itself_it_is_imported), name);
	    వెళ్ళు failed;
	}
	ఒకవేళ (!లోపల_vim9script())
	{
	    semsg(_(e_cannot_create_vim9_script_variable_లోపల_function_str),
									 name);
	    వెళ్ళు failed;
	}
    }

    // Search in parent scope which is possible to reference from lambda
    ఒకవేళ (di == ఖాళీ)
	di = కనుగొను_var_లోపల_scoped_ht(name, TRUE);

    ఒకవేళ ((tv->v_type == VAR_FUNC || tv->v_type == VAR_PARTIAL)
				  && var_wrong_func_name(name, di == ఖాళీ))
	వెళ్ళు failed;

    ఒకవేళ (need_convert_గమ్యం_bool(type, tv))
    {
	// Destination is a bool and the value is not, but it can be
	// converted.
	CLEAR_క్షేత్రం(bool_tv);
	bool_tv.v_type = VAR_BOOL;
	bool_tv.vval.v_సంఖ్య = tv2bool(tv) ? VVAL_TRUE : VVAL_FALSE;
	tv = &bool_tv;
    }

    ఒకవేళ (di != ఖాళీ)
    {
	// Item already exists.  Allowed to replace when reloading.
	ఒకవేళ ((di->di_flags & DI_FLAGS_RELOAD) == 0)
	{
	    ఒకవేళ ((flags & (కేటాయించు_CONST | కేటాయించు_FINAL))
					 && (flags & కేటాయించు_FOR_LOOP) == 0)
	    {
		emsg(_(e_cannot_modify_existing_variable));
		వెళ్ళు failed;
	    }

	    ఒకవేళ (is_script_local && vim9script
			  && (flags & (కేటాయించు_NO_DECL | కేటాయించు_DECL)) == 0)
	    {
		semsg(_(e_redefining_script_అంశం_str), name);
		వెళ్ళు failed;
	    }

	    ఒకవేళ (check_typval_is_విలువ(&di->di_tv) == FAIL)
		వెళ్ళు failed;

	    // List and Blob types can be modified in-place using the "+="
	    // compound operator.  For other types, this is not allowed.
	    పూర్ణాంకం type_inplace_modifiable =
		(di->di_tv.v_type == VAR_LIST || di->di_tv.v_type == VAR_BLOB);

	    ఒకవేళ (var_లోపల_vim9script && (flags & కేటాయించు_FOR_LOOP) == 0
		    && ((flags & కేటాయించు_COMPOUND_OP) == 0
			|| !type_inplace_modifiable))
	    {
		where_T where = WHERE_INIT;
		svar_T  *sv = కనుగొను_typval_లోపల_script(&di->di_tv, sid, TRUE);

		ఒకవేళ (sv != ఖాళీ)
		{
		    // check the type and adjust to bool if needed
		    ఒకవేళ (var_idx > 0)
		    {
			where.wt_సూచీ = var_idx;
			where.wt_రకం = WT_VARIABLE;
		    }
		    ఒకవేళ (check_script_var_type(sv, tv, name, where) == FAIL)
			వెళ్ళు failed;
		    ఒకవేళ (type == ఖాళీ)
			type = sv->sv_type;
		    sv->sv_flags |= SVFLAG_ASSIGNED;
		}
	    }

	    // Modifying a final variable with a List value using the "+="
	    // operator is allowed.  For other types, it is not allowed.
	    ఒకవేళ ((((flags & కేటాయించు_FOR_LOOP) == 0 || (flags & కేటాయించు_DECL) == 0)
			&& ((flags & కేటాయించు_COMPOUND_OP) == 0
			    || !type_inplace_modifiable))
				 ? var_check_permission(di, name) == FAIL
				 : var_check_ro(di->di_flags, name, FALSE))
		వెళ్ళు failed;
	}
	లేకపోతే
	{
	    // can only redefine once
	    di->di_flags &= ~DI_FLAGS_RELOAD;

	    // A Vim9 script-local variable is also present in sn_all_vars
	    // and sn_var_vals.  It may set "type" from "tv".
	    ఒకవేళ (var_లోపల_vim9script || var_లోపల_autoload)
		update_vim9_script_var(FALSE, di,
			var_లోపల_autoload ? name : di->di_కీలకం, flags,
			tv, &type, (flags & కేటాయించు_NO_MEMBER_TYPE) == 0);
	}

	// existing variable, need to clear the value

	// In Vim9 script a String v: variable only takes a String.
	ఒకవేళ (ht == &vimvarht && di->di_tv.v_type == VAR_STRING
							    && లోపల_vim9script())
	{
	    where_T	where = WHERE_INIT;

	    ఒకవేళ (check_typval_type(&t_string, tv, where) == FAIL)
		వెళ్ళు failed;
	}

	// Handle setting internal v: variables separately where needed to
	// prevent changing the type.
	పూర్ణాంకం type_error = FALSE;
	ఒకవేళ (ht == &vimvarht
		&& !before_సమితి_vvar(varname, di, tv, copy, &type_error))
	{
	    ఒకవేళ (type_error)
		semsg(_(e_setting_v_str_గమ్యం_విలువ_with_wrong_type), varname);
	    వెళ్ళు failed;
	}

	clear_tv(&di->di_tv);

	ఒకవేళ ((flags & కేటాయించు_UPDATE_ఖండం_ID)
				       && SCRIPT_ID_VALID(current_sctx.sc_sid))
	{
	    scriptitem_T *si = SCRIPT_అంశం(current_sctx.sc_sid);

	    update_script_var_ఖండం_id(name, si->sn_current_ఖండం_id);
	}
    }
    లేకపోతే
    {
	// Item not found, check if a function already exists.
	ఒకవేళ (is_script_local && (flags & (కేటాయించు_NO_DECL | కేటాయించు_DECL)) == 0
	       && lookup_scriptitem(name, STRLEN(name), FALSE, ఖాళీ) == OK)
	{
	    semsg(_(e_redefining_script_అంశం_str), name);
	    వెళ్ళు failed;
	}

	// add a new variable
	ఒకవేళ (var_లోపల_vim9script && (flags & కేటాయించు_NO_DECL))
	{
	    semsg(_(e_unknown_variable_str), name);
	    వెళ్ళు failed;
	}

	ఒకవేళ (check_hashtab_frozen(ht, "add variable"))
	    వెళ్ళు failed;

	// Can't add "v:" or "a:" variable.
	ఒకవేళ (ht == &vimvarht || ht == get_funccal_వాదనలు_ht())
	{
	    semsg(_(e_illegal_variable_name_str), name);
	    వెళ్ళు failed;
	}

	// Make sure the variable name is valid.  In Vim9 script an
	// autoload variable must be prefixed with "g:" unless in an
	// autoload script.
	ఒకవేళ (!valid_varname(varname, -1, !vim9script
			|| STRNCMP(name, "g:", 2) == 0 || var_లోపల_autoload))
	    వెళ్ళు failed;

	di = alloc(offsetof(dictitem_T, di_కీలకం) + STRLEN(varname) + 1);
	ఒకవేళ (di == ఖాళీ)
	    వెళ్ళు failed;
	STRCPY(di->di_కీలకం, varname);
	ఒకవేళ (hash_కూడు(ht, DI2HIKEY(di), "add variable") == FAIL)
	{
	    vim_free(di);
	    వెళ్ళు failed;
	}
	di->di_flags = DI_FLAGS_ALLOC;
	ఒకవేళ (flags & (కేటాయించు_CONST | కేటాయించు_FINAL))
	    di->di_flags |= DI_FLAGS_తాళం;

	// A Vim9 script-local variable is also added to sn_all_vars and
	// sn_var_vals. It may set "type" from "tv".
	ఒకవేళ (var_లోపల_vim9script || var_లోపల_autoload)
	    update_vim9_script_var(TRUE, di,
		    var_లోపల_autoload ? name : di->di_కీలకం, flags,
			  tv, &type, (flags & కేటాయించు_NO_MEMBER_TYPE) == 0);
    }

    dest_tv = &di->di_tv;
    ఒకవేళ (copy || tv->v_type == VAR_సంఖ్య || tv->v_type == VAR_FLOAT)
	copy_tv(tv, dest_tv);
    లేకపోతే
    {
	*dest_tv = *tv;
	dest_tv->v_తాళం = 0;
	init_tv(tv);
    }
    free_tv_arg = FALSE;

    ఒకవేళ (vim9script && type != ఖాళీ)
	సమితి_tv_type(dest_tv, type);

    // ":const var = value" locks the value
    // ":final var = value" locks "var"
    ఒకవేళ (flags & కేటాయించు_CONST)
	// Like :lockvar! name: lock the value and what it contains, but only
	// if the reference count is up to one.  That locks only literal
	// values.
	అంశం_తాళం(dest_tv, DICT_MAXNEST, TRUE, TRUE);

    rc = OK;

failed:
    vim_free(name_tofree);
    ఒకవేళ (free_tv_arg)
	clear_tv(tv_arg);

    తిరిగి rc;
}

/*
 * Check in this order for backwards compatibility:
 * - Whether the variable is read-only
 * - Whether the variable value is locked
 * - Whether the variable is locked
 * NOTE: "name" is only used for error messages.
 */
    పూర్ణాంకం
var_check_permission(dictitem_T *di, char_u *name)
{
    ఒకవేళ (var_check_ro(di->di_flags, name, FALSE)
		    || విలువ_check_తాళం(di->di_tv.v_తాళం, name, FALSE)
		    || var_check_తాళం(di->di_flags, name, FALSE))
	తిరిగి FAIL;
    తిరిగి OK;
}

/*
 * Return TRUE if di_flags "flags" indicates variable "name" is read-only.
 * Also give an error message.
 */
    పూర్ణాంకం
var_check_ro(పూర్ణాంకం flags, char_u *name, పూర్ణాంకం use_gettext)
{
    ఒకవేళ (flags & DI_FLAGS_RO)
    {
	ఒకవేళ (name == ఖాళీ)
	    emsg(_(e_cannot_change_readonly_variable));
	లేకపోతే
	    semsg(_(e_cannot_change_readonly_variable_str),
				       use_gettext ? (char_u *)_(name) : name);
	తిరిగి TRUE;
    }
    ఒకవేళ ((flags & DI_FLAGS_RO_SBX) && sandbox)
    {
	ఒకవేళ (name == ఖాళీ)
	    emsg(_(e_cannot_సమితి_variable_లోపల_sandbox));
	లేకపోతే
	    semsg(_(e_cannot_సమితి_variable_లోపల_sandbox_str),
				       use_gettext ? (char_u *)_(name) : name);
	తిరిగి TRUE;
    }
    తిరిగి FALSE;
}

/*
 * Return TRUE if di_flags "flags" indicates variable "name" is locked.
 * Also give an error message.
 */
    పూర్ణాంకం
var_check_తాళం(పూర్ణాంకం flags, char_u *name, పూర్ణాంకం use_gettext)
{
    ఒకవేళ (flags & DI_FLAGS_తాళం)
    {
	semsg(_(e_variable_is_locked_str),
				       use_gettext ? (char_u *)_(name) : name);
	తిరిగి TRUE;
    }
    తిరిగి FALSE;
}

/*
 * Return TRUE if di_flags "flags" indicates variable "name" is fixed.
 * Also give an error message.
 */
    పూర్ణాంకం
var_check_fixed(పూర్ణాంకం flags, char_u *name, పూర్ణాంకం use_gettext)
{
    ఒకవేళ (flags & DI_FLAGS_FIX)
    {
	ఒకవేళ (name == ఖాళీ)
	    emsg(_(e_cannot_delete_variable));
	లేకపోతే
	    semsg(_(e_cannot_delete_variable_str),
				      use_gettext ? (char_u *)_(name) : name);
	తిరిగి TRUE;
    }
    తిరిగి FALSE;
}

/*
 * Check if a funcref is assigned to a valid variable name.
 * Return TRUE and give an error if not.
 */
    పూర్ణాంకం
var_wrong_func_name(
    char_u *name,    // points to start of variable name
    పూర్ణాంకం    new_var)  // TRUE when creating the variable
{
    // Allow for w: b: s: and t:.  In Vim9 script s: is not allowed, because
    // the name can be used without the s: prefix.
    // Allow autoload variable.
    ఒకవేళ (!((vim_strchr((char_u *)"wbt", name[0]) != ఖాళీ
		    || (!లోపల_vim9script() && name[0] == 's')) && name[1] == ':')
	    && !ASCII_ISUPPER((name[0] != NUL && name[1] == ':')
						     ? name[2] : name[0])
	    && vim_strchr(name, '#') == ఖాళీ)
    {
	semsg(_(e_funcref_variable_name_must_ప్రారంభం_with_capital_str), name);
	తిరిగి TRUE;
    }
    // Don't allow hiding a function.  When "v" is not NULL we might be
    // assigning another function to the same var, the type is checked
    // below.
    ఒకవేళ (new_var && function_exists(name, FALSE))
    {
	semsg(_(e_variable_name_conflicts_with_existing_function_str),
								    name);
	తిరిగి TRUE;
    }
    తిరిగి FALSE;
}

/*
 * Return TRUE if "flags" indicates variable "name" has a locked (immutable)
 * value.  Also give an error message, using "name" or _("name") when
 * "use_gettext" is TRUE.
 */
    పూర్ణాంకం
విలువ_check_తాళం(పూర్ణాంకం తాళం, char_u *name, పూర్ణాంకం use_gettext)
{
    ఒకవేళ (తాళం & VAR_LOCKED)
    {
	ఒకవేళ (name == ఖాళీ)
	    emsg(_(e_విలువ_is_locked));
	లేకపోతే
	    semsg(_(e_విలువ_is_locked_str),
				       use_gettext ? (char_u *)_(name) : name);
	తిరిగి TRUE;
    }
    ఒకవేళ (తాళం & VAR_FIXED)
    {
	ఒకవేళ (name == ఖాళీ)
	    emsg(_(e_cannot_change_విలువ));
	లేకపోతే
	    semsg(_(e_cannot_change_విలువ_యొక్క_str),
				       use_gettext ? (char_u *)_(name) : name);
	తిరిగి TRUE;
    }
    తిరిగి FALSE;
}

/*
 * Check if a variable name is valid.  When "autoload" is true "#" is allowed.
 * If "len" is -1 use all of "varname", otherwise up to "varname[len]".
 * Return FALSE and give an error if not.
 */
    పూర్ణాంకం
valid_varname(char_u *varname, పూర్ణాంకం len, పూర్ణాంకం autoload)
{
    char_u *p;

    కొరకు (p = varname; len < 0 ? *p != NUL : p < varname + len; ++p)
	ఒకవేళ (!eval_isnamec1(*p) && (p == varname || !VIM_ISDIGIT(*p))
					 && !(autoload && *p == AUTOLOAD_CHAR))
	{
	    semsg(_(e_illegal_variable_name_str), varname);
	    తిరిగి FALSE;
	}
    తిరిగి TRUE;
}

/*
 * Implements the logic to retrieve local variable and option values.
 * Used by "getwinvar()" "gettabvar()" "gettabwinvar()" "getbufvar()".
 */
    స్థిర శూన్య
get_var_from(
    char_u	*varname,
    typval_T	*rettv,
    typval_T	*deftv,	    // Default value if not found.
    పూర్ణాంకం		htname,	    // 't'ab, 'w'indow or 'b'uffer local.
    tabpage_T	*tp,	    // can be NULL
    win_T	*win,
    buf_T	*buf)	    // Ignored if htname is not 'b'.
{
    dictitem_T	*v;
    పూర్ణాంకం		పూర్తి = FALSE;
    switchwin_T	switchwin;
    పూర్ణాంకం		need_switch_win;
    పూర్ణాంకం		do_change_curbuf = buf != ఖాళీ && htname == 'b';

    ++emsg_off;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ఖాళీ;

    ఒకవేళ (varname != ఖాళీ && tp != ఖాళీ && win != ఖాళీ
	    && (htname != 'b' || buf != ఖాళీ))
    {
	// Set curwin to be our win, temporarily.  Also set the tabpage,
	// otherwise the window is not valid. Only do this when needed,
	// autocommands get blocked.
	// If we have a buffer reference avoid the switching, we're saving and
	// restoring curbuf directly.
	need_switch_win = !(tp == curtab && win == curwin) && !do_change_curbuf;
	ఒకవేళ (!need_switch_win || switch_win(&switchwin, win, tp, TRUE) == OK)
	{
	    // Handle options. There are no tab-local options.
	    ఒకవేళ (*varname == '&' && htname != 't')
	    {
		buf_T	*save_curbuf = curbuf;

		// Change curbuf so the option is read from the correct buffer.
		ఒకవేళ (do_change_curbuf)
		    curbuf = buf;

		ఒకవేళ (varname[1] == NUL)
		{
		    // get all window-local or buffer-local options in a dict
		    dict_T	*opts = get_winbuf_options(htname == 'b');

		    ఒకవేళ (opts != ఖాళీ)
		    {
			rettv_dict_సమితి(rettv, opts);
			పూర్తి = TRUE;
		    }
		}
		లేకపోతే ఒకవేళ (eval_option(&varname, rettv, TRUE) == OK)
		    // Local option
		    పూర్తి = TRUE;

		curbuf = save_curbuf;
	    }
	    లేకపోతే ఒకవేళ (*varname == NUL)
	    {
		// Empty string: return a dict with all the local variables.
		ఒకవేళ (htname == 'b')
		    v = &buf->b_bufvar;
		లేకపోతే ఒకవేళ (htname == 'w')
		    v = &win->w_winvar;
		లేకపోతే
		    v = &tp->tp_winvar;
		copy_tv(&v->di_tv, rettv);
		పూర్తి = TRUE;
	    }
	    లేకపోతే
	    {
		hashtab_T	*ht;

		ఒకవేళ (htname == 'b')
		    ht = &buf->b_vars->dv_hashtab;
		లేకపోతే ఒకవేళ (htname == 'w')
		    ht = &win->w_vars->dv_hashtab;
		లేకపోతే
		    ht = &tp->tp_vars->dv_hashtab;

		// Look up the variable.
		v = కనుగొను_var_లోపల_ht(ht, htname, varname, FALSE);
		ఒకవేళ (v != ఖాళీ)
		{
		    copy_tv(&v->di_tv, rettv);
		    పూర్తి = TRUE;
		}
	    }
	}

	ఒకవేళ (need_switch_win)
	    // restore previous notion of curwin
	    restore_win(&switchwin, TRUE);
    }

    ఒకవేళ (!పూర్తి && deftv->v_type != VAR_UNKNOWN)
	// use the default value
	copy_tv(deftv, rettv);

    --emsg_off;
}

/*
 * getwinvar() and gettabwinvar()
 */
    స్థిర శూన్య
getwinvar(
    typval_T	*argvars,
    typval_T	*rettv,
    పూర్ణాంకం		off)	    // 1 for gettabwinvar()
{
    char_u	*varname;
    tabpage_T	*tp;
    win_T	*win;

    ఒకవేళ (off == 1)
	tp = కనుగొను_tabpage((పూర్ణాంకం)tv_get_సంఖ్య_chk(&argvars[0], ఖాళీ));
    లేకపోతే
	tp = curtab;
    win = కనుగొను_win_by_nr(&argvars[off], tp);
    varname = tv_get_string_chk(&argvars[off + 1]);

    get_var_from(varname, rettv, &argvars[off + 2], 'w', tp, win, ఖాళీ);
}

/*
 * Set option "varname" to the value of "varp" for the current buffer/window.
 */
    స్థిర శూన్య
సమితి_option_from_tv(char_u *varname, typval_T *varp)
{
    దీర్ఘ	numval = 0;
    char_u	*strval;
    char_u	nbuf[NUMBUFLEN];
    పూర్ణాంకం		error = FALSE;

    పూర్ణాంకం		opt_idx = findoption(varname);
    ఒకవేళ (opt_idx < 0)
    {
	semsg(_(e_unknown_option_str_2), varname);
	తిరిగి;
    }
    పూర్ణాంకం		opt_p_flags = get_option_flags(opt_idx);

    ఒకవేళ (varp->v_type == VAR_BOOL)
    {
	ఒకవేళ (opt_p_flags & P_STRING)
	{
	    emsg(_(e_string_required));
	    తిరిగి;
	}
	numval = (దీర్ఘ)varp->vval.v_సంఖ్య;
	strval = (char_u *)"0";  // avoid using "false"
    }
    లేకపోతే
    {
	ఒకవేళ ((opt_p_flags & (P_అంకె|P_BOOL))
		&& (!లోపల_vim9script() || varp->v_type != VAR_STRING))
	    numval = (దీర్ఘ)tv_get_సంఖ్య_chk(varp, &error);
	ఒకవేళ (!error)
	    strval = tv_get_string_buf_chk(varp, nbuf);
    }
    ఒకవేళ (!error && strval != ఖాళీ)
	సమితి_option_విలువ_give_err(varname, numval, strval, OPT_LOCAL);
}

/*
 * "setwinvar()" and "settabwinvar()" functions
 */
    స్థిర శూన్య
setwinvar(typval_T *argvars, పూర్ణాంకం off)
{
    win_T	*win;
    switchwin_T	switchwin;
    పూర్ణాంకం		need_switch_win;
    char_u	*varname, *winvarname;
    typval_T	*varp;
    tabpage_T	*tp = ఖాళీ;

    ఒకవేళ (check_secure())
	తిరిగి;

    ఒకవేళ (off == 1)
	tp = కనుగొను_tabpage((పూర్ణాంకం)tv_get_సంఖ్య_chk(&argvars[0], ఖాళీ));
    లేకపోతే
	tp = curtab;
    win = కనుగొను_win_by_nr(&argvars[off], tp);
    varname = tv_get_string_chk(&argvars[off + 1]);
    varp = &argvars[off + 2];

    ఒకవేళ (win == ఖాళీ || varname == ఖాళీ)
	తిరిగి;

    need_switch_win = !(tp == curtab && win == curwin);
    ఒకవేళ (!need_switch_win
	    || switch_win(&switchwin, win, tp, TRUE) == OK)
    {
	ఒకవేళ (*varname == '&')
	    సమితి_option_from_tv(varname + 1, varp);
	లేకపోతే
	{
	    winvarname = alloc(STRLEN(varname) + 3);
	    ఒకవేళ (winvarname != ఖాళీ)
	    {
		STRCPY(winvarname, "w:");
		STRCPY(winvarname + 2, varname);
		సమితి_var(winvarname, varp, TRUE);
		vim_free(winvarname);
	    }
	}
    }
    ఒకవేళ (need_switch_win)
	restore_win(&switchwin, TRUE);
}

/*
 * reset v:option_new, v:option_old, v:option_oldlocal, v:option_oldglobal,
 * v:option_type, and v:option_command.
 */
    శూన్య
reset_v_option_vars(శూన్య)
{
    సమితి_vim_var_string(VV_OPTION_NEW, ఖాళీ, -1);
    సమితి_vim_var_string(VV_OPTION_OLD, ఖాళీ, -1);
    సమితి_vim_var_string(VV_OPTION_OLDLOCAL, ఖాళీ, -1);
    సమితి_vim_var_string(VV_OPTION_OLDGLOBAL, ఖాళీ, -1);
    సమితి_vim_var_string(VV_OPTION_TYPE, ఖాళీ, -1);
    సమితి_vim_var_string(VV_OPTION_COMMAND, ఖాళీ, -1);
}

/*
 * Add an assert error to v:errors.
 */
    శూన్య
assert_error(garray_T *gap)
{
    నిర్మాణం vimvar   *vp = &vimvars[VV_ERRORS];

    ఒకవేళ (vp->vv_tv_type != VAR_LIST || vimvars[VV_ERRORS].vv_list == ఖాళీ)
	// Make sure v:errors is a list.
	సమితి_vim_var_list(VV_ERRORS, list_alloc());
    list_append_string(vimvars[VV_ERRORS].vv_list, gap->ga_దత్తాంశం, gap->ga_len);
}

    పూర్ణాంకం
var_exists(char_u *var)
{
    char_u	*arg = var;
    char_u	*name;
    char_u	*tofree;
    typval_T    tv;
    పూర్ణాంకం		len = 0;
    పూర్ణాంకం		n = FALSE;

    // get_name_len() takes care of expanding curly braces
    name = var;
    len = get_name_len(&arg, &tofree, TRUE, FALSE);
    ఒకవేళ (len > 0)
    {
	ఒకవేళ (tofree != ఖాళీ)
	    name = tofree;
	n = (eval_variable(name, len, 0, &tv, ఖాళీ,
				 EVAL_VAR_NOAUTOLOAD + EVAL_VAR_IMPORT) == OK);
	ఒకవేళ (n)
	{
	    // handle d.key, l[idx], f(expr)
	    arg = skipwhite(arg);
	    n = (handle_subscript(&arg, name, &tv, &EVALARG_EVALUATE,
								 FALSE) == OK);
	    ఒకవేళ (n)
		clear_tv(&tv);
	}
    }
    ఒకవేళ (*arg != NUL)
	n = FALSE;

    vim_free(tofree);
    తిరిగి n;
}

స్థిర lval_T	*redir_lval = ఖాళీ;
#నిర్వచించు EVALCMD_BUSY (redir_lval == (lval_T *)&redir_lval)
స్థిర garray_T redir_ga;	// only valid when redir_lval is not NULL
స్థిర char_u	*redir_endp = ఖాళీ;
స్థిర char_u	*redir_varname = ఖాళీ;

    పూర్ణాంకం
alloc_redir_lval(శూన్య)
{
    redir_lval = ALLOC_CLEAR_ONE(lval_T);
    ఒకవేళ (redir_lval == ఖాళీ)
	తిరిగి FAIL;
    తిరిగి OK;
}

    శూన్య
clear_redir_lval(శూన్య)
{
    VIM_CLEAR(redir_lval);
}

    శూన్య
init_redir_ga(శూన్య)
{
    ga_init2(&redir_ga, పరిమాణం(అక్షరం), 500);
}

/*
 * Start recording command output to a variable
 * When "append" is TRUE append to an existing variable.
 * Returns OK if successfully completed the setup.  FAIL otherwise.
 */
    పూర్ణాంకం
var_redir_ప్రారంభం(char_u *name, పూర్ణాంకం append)
{
    పూర్ణాంకం		called_emsg_before;
    typval_T	tv;

    // Catch a bad name early.
    ఒకవేళ (!eval_isnamec1(*name))
    {
	emsg(_(e_invalid_వాదం));
	తిరిగి FAIL;
    }

    // Make a copy of the name, it is used in redir_lval until redir ends.
    redir_varname = vim_strsave(name);
    ఒకవేళ (redir_varname == ఖాళీ)
	తిరిగి FAIL;

    ఒకవేళ (alloc_redir_lval() == FAIL)
    {
	var_redir_stop();
	తిరిగి FAIL;
    }

    // The output is stored in growarray "redir_ga" until redirection ends.
    init_redir_ga();

    // Parse the variable name (can be a dict or list entry).
    redir_endp = get_lval(redir_varname, ఖాళీ, redir_lval, FALSE, FALSE, 0,
							     FNE_CHECK_ప్రారంభం);
    ఒకవేళ (redir_endp == ఖాళీ || redir_lval->ll_name == ఖాళీ || *redir_endp != NUL)
    {
	clear_lval(redir_lval);
	ఒకవేళ (redir_endp != ఖాళీ && *redir_endp != NUL)
	    // Trailing characters are present after the variable name
	    semsg(_(e_trailing_characters_str), redir_endp);
	లేకపోతే
	    semsg(_(e_invalid_వాదం_str), name);
	redir_endp = ఖాళీ;  // don't store a value, only cleanup
	var_redir_stop();
	తిరిగి FAIL;
    }

    // check if we can write to the variable: set it to or append an empty
    // string
    called_emsg_before = called_emsg;
    tv.v_type = VAR_STRING;
    tv.vval.v_string = (char_u *)"";
    ఒకవేళ (append)
	సమితి_var_lval(redir_lval, redir_endp, &tv, TRUE,
					     కేటాయించు_NO_DECL, (char_u *)".", 0);
    లేకపోతే
	సమితి_var_lval(redir_lval, redir_endp, &tv, TRUE,
					     కేటాయించు_NO_DECL, (char_u *)"=", 0);
    clear_lval(redir_lval);
    ఒకవేళ (called_emsg > called_emsg_before)
    {
	redir_endp = ఖాళీ;  // don't store a value, only cleanup
	var_redir_stop();
	తిరిగి FAIL;
    }

    తిరిగి OK;
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
    శూన్య
var_redir_str(char_u *విలువ, పూర్ణాంకం విలువ_len)
{
    పూర్ణాంకం		len;

    ఒకవేళ (redir_lval == ఖాళీ)
	తిరిగి;

    ఒకవేళ (విలువ_len == -1)
	len = (పూర్ణాంకం)STRLEN(విలువ);	// Append the entire string
    లేకపోతే
	len = విలువ_len;		// Append only "value_len" characters

    ఒకవేళ (ga_grow(&redir_ga, len) == OK)
    {
	mch_memmove((అక్షరం *)redir_ga.ga_దత్తాంశం + redir_ga.ga_len, విలువ, len);
	redir_ga.ga_len += len;
    }
    లేకపోతే
	var_redir_stop();
}

/*
 * Stop redirecting command output to a variable.
 * Frees the allocated memory.
 */
    శూన్య
var_redir_stop(శూన్య)
{
    typval_T	tv;

    ఒకవేళ (EVALCMD_BUSY)
    {
	redir_lval = ఖాళీ;
	తిరిగి;
    }

    ఒకవేళ (redir_lval != ఖాళీ)
    {
	// If there was no error: assign the text to the variable.
	ఒకవేళ (redir_endp != ఖాళీ)
	{
	    ga_append(&redir_ga, NUL);  // Append the trailing NUL.
	    tv.v_type = VAR_STRING;
	    tv.vval.v_string = redir_ga.ga_దత్తాంశం;
	    // Call get_lval() again, if it's inside a Dict or List it may
	    // have changed.
	    redir_endp = get_lval(redir_varname, ఖాళీ, redir_lval,
					FALSE, FALSE, 0, FNE_CHECK_ప్రారంభం);
	    ఒకవేళ (redir_endp != ఖాళీ && redir_lval->ll_name != ఖాళీ)
		సమితి_var_lval(redir_lval, redir_endp, &tv, FALSE, 0,
							     (char_u *)".", 0);
	    clear_lval(redir_lval);
	}

	// free the collected output
	VIM_CLEAR(redir_ga.ga_దత్తాంశం);

	VIM_CLEAR(redir_lval);
    }
    VIM_CLEAR(redir_varname);
}

/*
 * Get the collected redirected text and clear redir_ga.
 */
    char_u *
get_clear_redir_ga(శూన్య)
{
    char_u *res;

    ga_append(&redir_ga, NUL);  // Append the trailing NUL.
    res = redir_ga.ga_దత్తాంశం;
    redir_ga.ga_దత్తాంశం = ఖాళీ;
    తిరిగి res;
}

/*
 * "gettabvar()" function
 */
    శూన్య
f_gettabvar(typval_T *argvars, typval_T *rettv)
{
    char_u	*varname;
    tabpage_T	*tp;
    win_T	*win = ఖాళీ;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_సంఖ్య_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	తిరిగి;

    varname = tv_get_string_chk(&argvars[1]);
    tp = కనుగొను_tabpage((పూర్ణాంకం)tv_get_సంఖ్య_chk(&argvars[0], ఖాళీ));
    ఒకవేళ (tp != ఖాళీ)
	win = tp == curtab || tp->tp_firstwin == ఖాళీ ? firstwin
		: tp->tp_firstwin;

    get_var_from(varname, rettv, &argvars[2], 't', tp, win, ఖాళీ);
}

/*
 * "gettabwinvar()" function
 */
    శూన్య
f_gettabwinvar(typval_T *argvars, typval_T *rettv)
{
    ఒకవేళ (లోపల_vim9script()
	    && (check_for_సంఖ్య_arg(argvars, 0) == FAIL
		|| check_for_సంఖ్య_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL))
	తిరిగి;

    getwinvar(argvars, rettv, 1);
}

/*
 * "getwinvar()" function
 */
    శూన్య
f_getwinvar(typval_T *argvars, typval_T *rettv)
{
    ఒకవేళ (లోపల_vim9script()
	    && (check_for_సంఖ్య_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	తిరిగి;

    getwinvar(argvars, rettv, 0);
}

/*
 * "getbufvar()" function
 */
    శూన్య
f_getbufvar(typval_T *argvars, typval_T *rettv)
{
    char_u	*varname;
    buf_T	*buf;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_బఫరు_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	తిరిగి;

    varname = tv_get_string_chk(&argvars[1]);
    buf = tv_get_buf_from_arg(&argvars[0]);

    get_var_from(varname, rettv, &argvars[2], 'b', curtab, curwin, buf);
}

/*
 * "settabvar()" function
 */
    శూన్య
f_settabvar(typval_T *argvars, typval_T *rettv వాడని)
{
    tabpage_T	*save_curtab;
    tabpage_T	*tp;
    tabpage_T	*save_lu_tp;
    char_u	*varname, *tabvarname;
    typval_T	*varp;

    ఒకవేళ (check_secure())
	తిరిగి;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_సంఖ్య_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	తిరిగి;

    tp = కనుగొను_tabpage((పూర్ణాంకం)tv_get_సంఖ్య_chk(&argvars[0], ఖాళీ));
    varname = tv_get_string_chk(&argvars[1]);
    varp = &argvars[2];

    ఒకవేళ (varname == ఖాళీ || tp == ఖాళీ)
	తిరిగి;

    save_curtab = curtab;
    save_lu_tp = lastused_tabpage;
    goto_tabpage_tp(tp, FALSE, FALSE);

    tabvarname = alloc(STRLEN(varname) + 3);
    ఒకవేళ (tabvarname != ఖాళీ)
    {
	STRCPY(tabvarname, "t:");
	STRCPY(tabvarname + 2, varname);
	సమితి_var(tabvarname, varp, TRUE);
	vim_free(tabvarname);
    }

    // Restore current tabpage and last accessed tabpage.
    ఒకవేళ (valid_tabpage(save_curtab))
    {
	goto_tabpage_tp(save_curtab, FALSE, FALSE);
	ఒకవేళ (valid_tabpage(save_lu_tp))
	    lastused_tabpage = save_lu_tp;
    }
}

/*
 * "settabwinvar()" function
 */
    శూన్య
f_settabwinvar(typval_T *argvars, typval_T *rettv వాడని)
{
    ఒకవేళ (లోపల_vim9script()
	    && (check_for_సంఖ్య_arg(argvars, 0) == FAIL
		|| check_for_సంఖ్య_arg(argvars, 1) == FAIL
		|| check_for_string_arg(argvars, 2) == FAIL))
	తిరిగి;

    setwinvar(argvars, 1);
}

/*
 * "setwinvar()" function
 */
    శూన్య
f_setwinvar(typval_T *argvars, typval_T *rettv వాడని)
{
    ఒకవేళ (లోపల_vim9script()
	    && (check_for_సంఖ్య_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	తిరిగి;

    setwinvar(argvars, 0);
}

/*
 * "setbufvar()" function
 */
    శూన్య
f_setbufvar(typval_T *argvars, typval_T *rettv వాడని)
{
    buf_T	*buf;
    char_u	*varname, *bufvarname;
    typval_T	*varp;

    ఒకవేళ (check_secure())
	తిరిగి;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_బఫరు_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	తిరిగి;

    varname = tv_get_string_chk(&argvars[1]);
    buf = tv_get_buf_from_arg(&argvars[0]);
    varp = &argvars[2];

    ఒకవేళ (buf == ఖాళీ || varname == ఖాళీ)
	తిరిగి;

    ఒకవేళ (*varname == '&')
    {
	aco_save_T	aco;
	// safe the current window position, it could
	// change because of 'scrollbind' window-local
	// options
	linenr_T	old_topline = curwin->w_topline;

	// Set curbuf to be our buf, temporarily.
	aucmd_prepbuf(&aco, buf);
	ఒకవేళ (curbuf == buf)
	{
	    // Only when it worked to set "curbuf".
	    సమితి_option_from_tv(varname + 1, varp);

	    // reset notion of buffer
	    aucmd_restbuf(&aco);
	}
	curwin->w_topline = old_topline;
    }
    లేకపోతే
    {
	bufvarname = alloc(STRLEN(varname) + 3);
	ఒకవేళ (bufvarname != ఖాళీ)
	{
	    buf_T *save_curbuf = curbuf;

	    curbuf = buf;
	    STRCPY(bufvarname, "b:");
	    STRCPY(bufvarname + 2, varname);
	    సమితి_var(bufvarname, varp, TRUE);
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
    పూర్ణాంకం		r = OK;

    CLEAR_క్షేత్రం(res);
    ఒకవేళ (arg->v_type == VAR_PARTIAL && arg->vval.v_partial != ఖాళీ)
    {
	res.cb_partial = arg->vval.v_partial;
	++res.cb_partial->pt_refcount;
	res.cb_name = partial_name(res.cb_partial);
    }
    లేకపోతే
    {
	ఒకవేళ (arg->v_type == VAR_STRING && arg->vval.v_string != ఖాళీ
					       && సురక్షిత_isdigit(*arg->vval.v_string))
	    r = FAIL;
	లేకపోతే ఒకవేళ (arg->v_type == VAR_FUNC || arg->v_type == VAR_STRING)
	{
	    res.cb_name = arg->vval.v_string;
	    ఒకవేళ (arg->v_type == VAR_STRING)
	    {
		char_u *name = get_scriptlocal_funcname(arg->vval.v_string);
		ఒకవేళ (name != ఖాళీ)
		{
		    res.cb_name = name;
		    res.cb_free_name = TRUE;
		}
	    }
	    func_ref(res.cb_name);
	}
	లేకపోతే ఒకవేళ (arg->v_type == VAR_సంఖ్య && arg->vval.v_సంఖ్య == 0)
	    res.cb_name = (char_u *)"";
	లేకపోతే
	    r = FAIL;

	ఒకవేళ (r == FAIL)
	{
	    emsg(_(e_invalid_callback_వాదం));
	    res.cb_name = ఖాళీ;
	}
    }
    తిరిగి res;
}

/*
 * Copy a callback into a typval_T.
 */
    శూన్య
put_callback(callback_T *cb, typval_T *tv)
{
    ఒకవేళ (cb->cb_partial != ఖాళీ)
    {
	tv->v_type = VAR_PARTIAL;
	tv->vval.v_partial = cb->cb_partial;
	++tv->vval.v_partial->pt_refcount;
    }
    లేకపోతే
    {
	tv->v_type = VAR_FUNC;
	tv->vval.v_string = vim_strsave(cb->cb_name);
	func_ref(cb->cb_name);
    }
}

    స్థిర bool
does_callback_own_cb_name(callback_T *cb)
{
    // If cb_partial != NULL then *cb->cb_name is owned by the partial.
    తిరిగి cb->cb_partial || cb->cb_free_name;
}

/*
 * Make a copy of "src" into "dest", allocating the function name if needed,
 * without incrementing the refcount.
 */
    శూన్య
సమితి_callback(callback_T *dest, callback_T *src)
{
    *dest = *src;
    ఒకవేళ (!does_callback_own_cb_name(src))
    {
	dest->cb_name = vim_strsave(src->cb_name);
	dest->cb_free_name = TRUE;
    }
    *src = (callback_T){0};
}

/*
 * Copy callback from "src" to "dest", incrementing the refcounts.
 */
    శూన్య
copy_callback(callback_T *dest, callback_T *src)
{
    dest->cb_partial = src->cb_partial;
    ఒకవేళ (dest->cb_partial != ఖాళీ)
    {
	dest->cb_name = src->cb_name;
	dest->cb_free_name = FALSE;
	++dest->cb_partial->pt_refcount;
    }
    లేకపోతే
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
    శూన్య
expand_autload_callback(callback_T *cb)
{
    char_u	*name;
    char_u	*p;
    imported_T	*import;

    ఒకవేళ (!లోపల_vim9script() || cb->cb_name == ఖాళీ
	    || (!cb->cb_free_name
	       && (cb->cb_partial == ఖాళీ || cb->cb_partial->pt_name == ఖాళీ)))
	తిరిగి;
    ఒకవేళ (cb->cb_partial != ఖాళీ)
	name = cb->cb_partial->pt_name;
    లేకపోతే
	name = cb->cb_name;
    p = vim_strchr(name, '.');
    ఒకవేళ (p == ఖాళీ)
	తిరిగి;

    import = కనుగొను_imported(name, p - name, FALSE);
    ఒకవేళ (import == ఖాళీ || !SCRIPT_ID_VALID(import->imp_sid))
	తిరిగి;

    scriptitem_T *si = SCRIPT_అంశం(import->imp_sid);
    ఒకవేళ (si->sn_autoload_ఉపసర్గ == ఖాళీ)
	తిరిగి;

    char_u *newname = concat_str(si->sn_autoload_ఉపసర్గ, p + 1);
    ఒకవేళ (newname == ఖాళీ)
	తిరిగి;

    ఒకవేళ (cb->cb_partial != ఖాళీ)
    {
	ఒకవేళ (cb->cb_name == cb->cb_partial->pt_name)
	    cb->cb_name = newname;
	vim_free(cb->cb_partial->pt_name);
	cb->cb_partial->pt_name = newname;
    }
    లేకపోతే
    {
	vim_free(cb->cb_name);
	cb->cb_name = newname;
    }
}

/*
 * Unref/free "callback" returned by get_callback() or set_callback().
 */
    శూన్య
free_callback(callback_T *callback)
{
    ఒకవేళ (callback->cb_partial != ఖాళీ)
    {
	partial_unref(callback->cb_partial);
	callback->cb_partial = ఖాళీ;
    }
    లేకపోతే ఒకవేళ (callback->cb_name != ఖాళీ)
	func_unref(callback->cb_name);
    ఒకవేళ (callback->cb_free_name)
    {
	vim_free(callback->cb_name);
	callback->cb_free_name = FALSE;
    }
    callback->cb_name = ఖాళీ;
}

#అయితే_ముగింపు // FEAT_EVAL
