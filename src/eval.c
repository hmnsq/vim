/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * eval.c: Expression evaluation.
 */
#నిర్వచించు USING_FLOAT_STUFF

#అన్తర్భావ "vim.h"

#ఒకవేళ defined(FEAT_EVAL)

#నిర్వచితమైతే VMS
# include <float.h>
#అయితే_ముగింపు

#నిర్వచించు NAMESPACE_CHAR	(char_u *)"abglstvw"

స్థిర పూర్ణాంకం eval0_simple_funccal(char_u *arg, typval_T *rettv, exarg_T *eap, evalarg_T *evalarg);
స్థిర పూర్ణాంకం eval2(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
స్థిర పూర్ణాంకం eval3(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
స్థిర పూర్ణాంకం eval4(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
స్థిర పూర్ణాంకం eval5(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
స్థిర పూర్ణాంకం eval6(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
స్థిర పూర్ణాంకం eval7(char_u **arg, typval_T *rettv, evalarg_T *evalarg, పూర్ణాంకం want_string);
స్థిర పూర్ణాంకం eval8(char_u **arg, typval_T *rettv, evalarg_T *evalarg, పూర్ణాంకం want_string);
స్థిర పూర్ణాంకం eval9(char_u **arg, typval_T *rettv, evalarg_T *evalarg, పూర్ణాంకం want_string);
స్థిర పూర్ణాంకం eval9_leader(typval_T *rettv, పూర్ణాంకం numeric_only, char_u *ప్రారంభం_leader, char_u **చివర_leaderp);

స్థిర char_u *make_expanded_name(char_u *లోపల_ప్రారంభం, char_u *సమాసం_ప్రారంభం, char_u *సమాసం_చివర, char_u *లోపల_చివర);

/*
 * Return "n1" divided by "n2", taking care of dividing by zero.
 * If "failed" is not NULL set it to TRUE when dividing by zero fails.
 */
	varnumber_T
అంకె_divide(varnumber_T n1, varnumber_T n2, పూర్ణాంకం *failed)
{
    varnumber_T	ఫలితం;

    ఒకవేళ (n2 == 0)
    {
	ఒకవేళ (లోపల_vim9script())
	{
	    emsg(_(e_divide_by_zero));
	    ఒకవేళ (failed != ఖాళీ)
		*failed = TRUE;
	}
	ఒకవేళ (n1 == 0)
	    ఫలితం = VARNUM_MIN; // similar to NaN
	లేకపోతే ఒకవేళ (n1 < 0)
	    ఫలితం = -VARNUM_MAX;
	లేకపోతే
	    ఫలితం = VARNUM_MAX;
    }
    లేకపోతే ఒకవేళ (n1 == VARNUM_MIN && n2 == -1)
    {
	// specific case: trying to do VARNUM_MIN / -1 results in a positive
	// number that doesn't fit in varnumber_T and causes an FPE
	ఫలితం = VARNUM_MAX;
    }
    లేకపోతే
	ఫలితం = n1 / n2;

    తిరిగి ఫలితం;
}

/*
 * Return "n1" modulus "n2", taking care of dividing by zero.
 * If "failed" is not NULL set it to TRUE when dividing by zero fails.
 */
	varnumber_T
అంకె_modulus(varnumber_T n1, varnumber_T n2, పూర్ణాంకం *failed)
{
    ఒకవేళ (n2 == 0 && లోపల_vim9script())
    {
	emsg(_(e_divide_by_zero));
	ఒకవేళ (failed != ఖాళీ)
	    *failed = TRUE;
    }
    తిరిగి (n2 == 0) ? 0 : (n1 % n2);
}

/*
 * Initialize the global and v: variables.
 */
    శూన్య
eval_init(శూన్య)
{
    evalvars_init();
    func_init();
}

#ఒకవేళ defined(EXITFREE)
    శూన్య
eval_clear(శూన్య)
{
    evalvars_clear();
    free_scriptnames();  // must come after evalvars_clear().
    free_locales();

    // autoloaded script names
    free_autoload_scriptnames();

    // unreferenced lists, tuples and dicts
    (శూన్య)garbage_collect(FALSE);

    // functions not garbage collected
    free_all_functions();
}
#అయితే_ముగింపు

    శూన్య
fill_evalarg_from_eap(evalarg_T *evalarg, exarg_T *eap, పూర్ణాంకం skip)
{
    init_evalarg(evalarg);
    evalarg->eval_flags = skip ? 0 : EVAL_EVALUATE;

    ఒకవేళ (eap == ఖాళీ)
	తిరిగి;

    evalarg->eval_cstack = eap->cstack;
    ఒకవేళ (sourcing_a_script(eap) || eap->ea_getline == get_list_పంక్తి)
    {
	evalarg->eval_getline = eap->ea_getline;
	evalarg->eval_cookie = eap->cookie;
    }
    evalarg->eval_class = eap->ea_class;
}

/*
 * Top level evaluation function, returning a boolean.
 * Sets "error" to TRUE if there was an error.
 * Return TRUE or FALSE.
 */
    పూర్ణాంకం
eval_గమ్యం_bool(
    char_u	*arg,
    పూర్ణాంకం		*error,
    exarg_T	*eap,
    పూర్ణాంకం		skip,	    // only parse, don't execute
    పూర్ణాంకం		use_simple_function)
{
    typval_T	tv;
    varnumber_T	retval = FALSE;
    evalarg_T	evalarg;
    పూర్ణాంకం		r;

    fill_evalarg_from_eap(&evalarg, eap, skip);

    ఒకవేళ (skip)
	++emsg_skip;
    ఒకవేళ (use_simple_function)
	r = eval0_simple_funccal(arg, &tv, eap, &evalarg);
    లేకపోతే
	r = eval0(arg, &tv, eap, &evalarg);
    ఒకవేళ (r == FAIL)
	*error = TRUE;
    లేకపోతే
    {
	*error = FALSE;
	ఒకవేళ (!skip)
	{
	    ఒకవేళ (లోపల_vim9script())
		retval = tv_get_bool_chk(&tv, error);
	    లేకపోతే
		retval = (tv_get_సంఖ్య_chk(&tv, error) != 0);
	    clear_tv(&tv);
	}
    }
    ఒకవేళ (skip)
	--emsg_skip;
    clear_evalarg(&evalarg, eap);

    తిరిగి (పూర్ణాంకం)retval;
}

/*
 * Call eval1() and give an error message if not done at a lower level.
 */
    స్థిర పూర్ణాంకం
eval1_emsg(char_u **arg, typval_T *rettv, exarg_T *eap)
{
    char_u	*ప్రారంభం = *arg;
    పూర్ణాంకం		ret;
    పూర్ణాంకం		did_emsg_before = did_emsg;
    పూర్ణాంకం		called_emsg_before = called_emsg;
    evalarg_T	evalarg;

    fill_evalarg_from_eap(&evalarg, eap, eap != ఖాళీ && eap->skip);

    ret = eval1(arg, rettv, &evalarg);
    ఒకవేళ (ret == FAIL)
    {
	// Report the invalid expression unless the expression evaluation has
	// been cancelled due to an aborting error, an interrupt, or an
	// exception, or we already gave a more specific error.
	// Also check called_emsg for when using assert_fails().
	ఒకవేళ (!aborting() && did_emsg == did_emsg_before
					  && called_emsg == called_emsg_before)
	    semsg(_(e_invalid_వ్యక్తీకరణ_str), ప్రారంభం);
    }
    clear_evalarg(&evalarg, eap);
    తిరిగి ret;
}

/*
 * Return whether a typval is a valid expression to pass to eval_expr_typval()
 * or eval_expr_to_bool().  An empty string returns FALSE;
 */
    పూర్ణాంకం
eval_సమాసం_valid_arg(typval_T *tv)
{
    తిరిగి tv->v_type != VAR_UNKNOWN
	    && (tv->v_type != VAR_STRING
		  || (tv->vval.v_string != ఖాళీ && *tv->vval.v_string != NUL));
}

/*
 * When calling eval_expr_typval() many times we only need one funccall_T.
 * Returns NULL when no funccall_T is to be used.
 * When returning non-NULL remove_funccal() must be called later.
 */
    funccall_T *
eval_సమాసం_get_funccal(typval_T *సమాసం, typval_T *rettv)
{
    ఒకవేళ (సమాసం->v_type != VAR_PARTIAL)
	తిరిగి ఖాళీ;

    partial_T *partial = సమాసం->vval.v_partial;
    ఒకవేళ (partial == ఖాళీ)
	తిరిగి ఖాళీ;
    ఒకవేళ (partial->pt_func == ఖాళీ
	    || partial->pt_func->uf_def_స్థితి == UF_NOT_COMPILED)
	తిరిగి ఖాళీ;

    తిరిగి create_funccal(partial->pt_func, rettv);
}

/*
 * Evaluate a partial.
 * Pass arguments "argv[argc]".
 * "fc_arg" is from eval_expr_get_funccal() or NULL;
 * Return the result in "rettv" and OK or FAIL.
 */
    స్థిర పూర్ణాంకం
eval_సమాసం_partial(
    typval_T	*సమాసం,
    typval_T	*argv,
    పూర్ణాంకం		argc,
    funccall_T	*fc_arg,
    typval_T	*rettv)
{
    partial_T	*partial = సమాసం->vval.v_partial;

    ఒకవేళ (partial == ఖాళీ)
	తిరిగి FAIL;

    ఒకవేళ (partial->pt_func != ఖాళీ
			&& partial->pt_func->uf_def_స్థితి != UF_NOT_COMPILED)
    {
	funccall_T	*fc = fc_arg != ఖాళీ ? fc_arg
				: create_funccal(partial->pt_func, rettv);
	పూర్ణాంకం		r;

	ఒకవేళ (fc == ఖాళీ)
	    తిరిగి FAIL;

	// Shortcut to call a compiled function with minimal overhead.
	ఒకవేళ (partial->pt_obj != ఖాళీ)
	    partial->pt_obj->obj_refcount++;
	r = call_def_function(partial->pt_func, argc, argv, DEF_USE_PT_ARGV,
					partial, partial->pt_obj, fc, rettv);
	ఒకవేళ (fc_arg == ఖాళీ)
	    remove_funccal();
	ఒకవేళ (r == FAIL)
	    తిరిగి FAIL;
    }
    లేకపోతే
    {
	char_u		*s = partial_name(partial);
	funcexe_T	funcexe;

	ఒకవేళ (s == ఖాళీ || *s == NUL)
	    తిరిగి FAIL;

	CLEAR_క్షేత్రం(funcexe);
	funcexe.fe_evaluate = TRUE;
	funcexe.fe_partial = partial;
	ఒకవేళ (call_func(s, -1, rettv, argc, argv, &funcexe) == FAIL)
	    తిరిగి FAIL;
    }

    తిరిగి OK;
}

/*
 * Evaluate an expression which is a function.
 * Pass arguments "argv[argc]".
 * Return the result in "rettv" and OK or FAIL.
 */
    స్థిర పూర్ణాంకం
eval_సమాసం_func(
    typval_T	*సమాసం,
    typval_T	*argv,
    పూర్ణాంకం		argc,
    typval_T	*rettv)
{
    funcexe_T	funcexe;
    char_u	buf[NUMBUFLEN];
    char_u	*s;

    ఒకవేళ (సమాసం->v_type == VAR_FUNC)
	s = సమాసం->vval.v_string;
    లేకపోతే
	s = tv_get_string_buf_chk_strict(సమాసం, buf, లోపల_vim9script());
    ఒకవేళ (s == ఖాళీ || *s == NUL)
	తిరిగి FAIL;

    CLEAR_క్షేత్రం(funcexe);
    funcexe.fe_evaluate = TRUE;
    ఒకవేళ (call_func(s, -1, rettv, argc, argv, &funcexe) == FAIL)
	తిరిగి FAIL;

    తిరిగి OK;
}

/*
 * Evaluate an expression, which is a string.
 * Return the result in "rettv" and OK or FAIL.
 */
    స్థిర పూర్ణాంకం
eval_సమాసం_string(
    typval_T	*సమాసం,
    typval_T	*rettv)
{
    char_u	*s;
    char_u	buf[NUMBUFLEN];

    s = tv_get_string_buf_chk_strict(సమాసం, buf, లోపల_vim9script());
    ఒకవేళ (s == ఖాళీ)
	తిరిగి FAIL;

    s = skipwhite(s);
    ఒకవేళ (eval1_emsg(&s, rettv, ఖాళీ) == FAIL)
	తిరిగి FAIL;

    ఒకవేళ (*skipwhite(s) != NUL)  // check for trailing chars after expr
    {
	clear_tv(rettv);
	semsg(_(e_invalid_వ్యక్తీకరణ_str), s);
	తిరిగి FAIL;
    }

    తిరిగి OK;
}

/*
 * Evaluate an expression, which can be a function, partial or string.
 * Pass arguments "argv[argc]".
 * If "want_func" is TRUE treat a string as a function name, not an expression.
 * "fc_arg" is from eval_expr_get_funccal() or NULL;
 * Return the result in "rettv" and OK or FAIL.
 */
    పూర్ణాంకం
eval_సమాసం_typval(
	typval_T    *సమాసం,
	పూర్ణాంకం	    want_func,
	typval_T    *argv,
	పూర్ణాంకం	    argc,
	funccall_T  *fc_arg,
	typval_T    *rettv)
{
    ఒకవేళ (సమాసం->v_type == VAR_PARTIAL)
	తిరిగి eval_సమాసం_partial(సమాసం, argv, argc, fc_arg, rettv);
    ఒకవేళ (సమాసం->v_type == VAR_INSTR)
	తిరిగి exe_typval_instr(సమాసం, rettv);
    ఒకవేళ (సమాసం->v_type == VAR_FUNC || want_func)
	తిరిగి eval_సమాసం_func(సమాసం, argv, argc, rettv);

    తిరిగి eval_సమాసం_string(సమాసం, rettv);
}

/*
 * Like eval_to_bool() but using a typval_T instead of a string.
 * Works for string, funcref and partial.
 */
    పూర్ణాంకం
eval_సమాసం_గమ్యం_bool(typval_T *సమాసం, పూర్ణాంకం *error)
{
    typval_T	rettv;
    పూర్ణాంకం		res;

    ఒకవేళ (eval_సమాసం_typval(సమాసం, FALSE, ఖాళీ, 0, ఖాళీ, &rettv) == FAIL)
    {
	*error = TRUE;
	తిరిగి FALSE;
    }
    res = (tv_get_bool_chk(&rettv, error) != 0);
    clear_tv(&rettv);
    తిరిగి res;
}

/*
 * Top level evaluation function, returning a string.  If "skip" is TRUE,
 * only parsing to "nextcmd" is done, without reporting errors.  Return
 * pointer to allocated memory, or NULL for failure or when "skip" is TRUE.
 */
    char_u *
eval_గమ్యం_string_skip(
    char_u	*arg,
    exarg_T	*eap,
    పూర్ణాంకం		skip)	    // only parse, don't execute
{
    typval_T	tv;
    char_u	*retval;
    evalarg_T	evalarg;

    fill_evalarg_from_eap(&evalarg, eap, skip);
    ఒకవేళ (skip)
	++emsg_skip;
    ఒకవేళ (eval0(arg, &tv, eap, &evalarg) == FAIL || skip)
	retval = ఖాళీ;
    లేకపోతే
    {
	retval = vim_strsave(tv_get_string(&tv));
	clear_tv(&tv);
    }
    ఒకవేళ (skip)
	--emsg_skip;
    clear_evalarg(&evalarg, eap);

    తిరిగి retval;
}

/*
 * Initialize "evalarg" for use.
 */
    శూన్య
init_evalarg(evalarg_T *evalarg)
{
    CLEAR_సూచిక(evalarg);
    ga_init2(&evalarg->eval_tofree_ga, పరిమాణం(char_u *), 20);
}

/*
 * If "evalarg->eval_tofree" is not NULL free it later.
 * Caller is expected to overwrite "evalarg->eval_tofree" next.
 */
    స్థిర శూన్య
free_eval_tofree_later(evalarg_T *evalarg)
{
    ఒకవేళ (evalarg->eval_tofree == ఖాళీ)
	తిరిగి;

    ఒకవేళ (ga_grow(&evalarg->eval_tofree_ga, 1) == OK)
	((char_u **)evalarg->eval_tofree_ga.ga_దత్తాంశం)
	    [evalarg->eval_tofree_ga.ga_len++]
	    = evalarg->eval_tofree;
    లేకపోతే
	vim_free(evalarg->eval_tofree);
}

/*
 * After using "evalarg" filled from "eap": free the memory.
 */
    శూన్య
clear_evalarg(evalarg_T *evalarg, exarg_T *eap)
{
    ఒకవేళ (evalarg == ఖాళీ)
	తిరిగి;

    garray_T *etga = &evalarg->eval_tofree_ga;

    ఒకవేళ (evalarg->eval_tofree != ఖాళీ || evalarg->eval_using_cmdline)
    {
	ఒకవేళ (eap != ఖాళీ)
	{
	    // We may need to keep the original command line, e.g. for
	    // ":let" it has the variable names.  But we may also need
	    // the new one, "nextcmd" points into it.  Keep both.
	    vim_free(eap->cmdline_tofree);
	    eap->cmdline_tofree = *eap->cmdlinep;

	    ఒకవేళ (evalarg->eval_using_cmdline && etga->ga_len > 0)
	    {
		// "nextcmd" points into the last line in eval_tofree_ga,
		// need to keep it around.
		--etga->ga_len;
		*eap->cmdlinep = ((char_u **)etga->ga_దత్తాంశం)[etga->ga_len];
		vim_free(evalarg->eval_tofree);
	    }
	    లేకపోతే
		*eap->cmdlinep = evalarg->eval_tofree;
	}
	లేకపోతే
	    vim_free(evalarg->eval_tofree);
	evalarg->eval_tofree = ఖాళీ;
    }

    ga_clear_అక్షరశ్రేణులు(etga);
    VIM_CLEAR(evalarg->eval_tofree_lambda);
}

/*
 * Skip over an expression at "*pp".
 * Return FAIL for an error, OK otherwise.
 */
    పూర్ణాంకం
skip_సమాసం(char_u **pp, evalarg_T *evalarg)
{
    typval_T	rettv;

    *pp = skipwhite(*pp);
    తిరిగి eval1(pp, &rettv, evalarg);
}

/*
 * Skip over an expression at "*arg".
 * If in Vim9 script and line breaks are encountered, the lines are
 * concatenated.  "evalarg->eval_tofree" will be set accordingly.
 * "arg" is advanced to just after the expression.
 * "start" is set to the start of the expression, "end" to just after the end.
 * Also when the expression is copied to allocated memory.
 * Return FAIL for an error, OK otherwise.
 */
    పూర్ణాంకం
skip_సమాసం_concatenate(
	char_u	    **arg,
	char_u	    **ప్రారంభం,
	char_u	    **చివర,
	evalarg_T   *evalarg)
{
    typval_T	rettv;
    పూర్ణాంకం		res;
    పూర్ణాంకం		vim9script = లోపల_vim9script();
    garray_T    *gap = evalarg == ఖాళీ ? ఖాళీ : &evalarg->eval_ga;
    garray_T    *freegap = evalarg == ఖాళీ ? ఖాళీ : &evalarg->eval_freega;
    పూర్ణాంకం		save_flags = evalarg == ఖాళీ ? 0 : evalarg->eval_flags;
    పూర్ణాంకం		evaluate = evalarg == ఖాళీ
			       ? FALSE : (evalarg->eval_flags & EVAL_EVALUATE);

    ఒకవేళ (vim9script && evaluate
	       && (evalarg->eval_cookie != ఖాళీ || evalarg->eval_cctx != ఖాళీ))
    {
	ga_init2(gap, పరిమాణం(char_u *), 10);
	// leave room for "start"
	ఒకవేళ (ga_grow(gap, 1) == OK)
	    ++gap->ga_len;
	ga_init2(freegap, పరిమాణం(char_u *), 10);
    }
    *ప్రారంభం = *arg;

    // Don't evaluate the expression.
    ఒకవేళ (evalarg != ఖాళీ)
	evalarg->eval_flags &= ~EVAL_EVALUATE;
    *arg = skipwhite(*arg);
    res = eval1(arg, &rettv, evalarg);
    *చివర = *arg;
    ఒకవేళ (evalarg != ఖాళీ)
	evalarg->eval_flags = save_flags;

    ఒకవేళ (vim9script && evaluate
	    && (evalarg->eval_cookie != ఖాళీ || evalarg->eval_cctx != ఖాళీ))
    {
	ఒకవేళ (evalarg->eval_ga.ga_len == 1)
	{
	    // just the one line, no need to concatenate
	    ga_clear(gap);
	    gap->ga_itemsize = 0;
	}
	లేకపోతే
	{
	    char_u	    *p;
	    size_t	    endoff = STRLEN(*arg);

	    // Line breaks encountered, concatenate all the lines.
	    *((char_u **)gap->ga_దత్తాంశం) = *ప్రారంభం;
	    p = ga_concat_అక్షరశ్రేణులు(gap, " ");

	    // free the lines only when using getsourceline()
	    ఒకవేళ (evalarg->eval_cookie != ఖాళీ)
	    {
		// Do not free the first line, the caller can still use it.
		*((char_u **)gap->ga_దత్తాంశం) = ఖాళీ;
		// Do not free the last line, "arg" points into it, free it
		// later.  Also free "eval_tofree" later if needed.
		free_eval_tofree_later(evalarg);
		evalarg->eval_tofree =
				    ((char_u **)gap->ga_దత్తాంశం)[gap->ga_len - 1];
		((char_u **)gap->ga_దత్తాంశం)[gap->ga_len - 1] = ఖాళీ;
		ga_clear_అక్షరశ్రేణులు(gap);
		ga_clear(freegap);
	    }
	    లేకపోతే
	    {
		ga_clear(gap);

		// free lines that were explicitly marked for freeing
		ga_clear_అక్షరశ్రేణులు(freegap);
	    }

	    gap->ga_itemsize = 0;
	    ఒకవేళ (p == ఖాళీ)
		తిరిగి FAIL;
	    *ప్రారంభం = p;
	    vim_free(evalarg->eval_tofree_lambda);
	    evalarg->eval_tofree_lambda = p;
	    // Compute "end" relative to the end.
	    *చివర = *ప్రారంభం + STRLEN(*ప్రారంభం) - endoff;
	}
    }

    తిరిగి res;
}

/*
 * Convert "tv" to a string.
 * When "join_list" is TRUE convert a List or a Tuple into a sequence of lines.
 * Returns an allocated string (NULL when out of memory).
 */
    char_u *
typval2string(typval_T *tv, పూర్ణాంకం join_list)
{
    garray_T	ga;
    char_u	*retval = ఖాళీ;

    ఒకవేళ (join_list && (tv->v_type == VAR_LIST || tv->v_type == VAR_TUPLE))
    {
	ఒకవేళ (tv->v_type == VAR_LIST)
	{
	    ga_init2(&ga, పరిమాణం(అక్షరం), 80);
	    ఒకవేళ (tv->vval.v_list != ఖాళీ)
	    {
		list_join(&ga, tv->vval.v_list, (char_u *)"\n", TRUE, FALSE,
									0);
		ఒకవేళ (tv->vval.v_list->lv_len > 0)
		    ga_append(&ga, NL);
	    }
	    ga_append(&ga, NUL);
	    retval = (char_u *)ga.ga_దత్తాంశం;
	}
	లేకపోతే
	{
	    // tuple
	    ga_init2(&ga, పరిమాణం(అక్షరం), 80);
	    ఒకవేళ (tv->vval.v_tuple != ఖాళీ)
	    {
		tuple_join(&ga, tv->vval.v_tuple, (char_u *)"\n", TRUE, FALSE,
									0);
		ఒకవేళ (TUPLE_LEN(tv->vval.v_tuple) > 0)
		    ga_append(&ga, NL);
	    }
	    ga_append(&ga, NUL);
	    retval = (char_u *)ga.ga_దత్తాంశం;
	}
    }
    లేకపోతే ఒకవేళ (tv->v_type == VAR_LIST
	    || tv->v_type == VAR_TUPLE
	    || tv->v_type == VAR_DICT)
    {
	char_u	*tofree;
	char_u	numbuf[NUMBUFLEN];

	retval = tv2string(tv, &tofree, numbuf, 0);
	// Make a copy if we have a value but it's not in allocated memory.
	ఒకవేళ (retval != ఖాళీ && tofree == ఖాళీ)
	    retval = vim_strsave(retval);
    }
    లేకపోతే
	retval = vim_strsave(tv_get_string(tv));
    తిరిగి retval;
}

/*
 * Top level evaluation function, returning a string.  Does not handle line
 * breaks.
 * When "join_list" is TRUE convert a List and a Tuple into a sequence of
 * lines.
 * Return pointer to allocated memory, or NULL for failure.
 */
    char_u *
eval_గమ్యం_string_eap(
    char_u	*arg,
    పూర్ణాంకం		join_list,
    exarg_T	*eap,
    పూర్ణాంకం		use_simple_function)
{
    typval_T	tv;
    char_u	*retval;
    evalarg_T	evalarg;
    పూర్ణాంకం		r;

    fill_evalarg_from_eap(&evalarg, eap, eap != ఖాళీ && eap->skip);
    ఒకవేళ (use_simple_function)
	r = eval0_simple_funccal(arg, &tv, ఖాళీ, &evalarg);
    లేకపోతే
	r = eval0(arg, &tv, ఖాళీ, &evalarg);
    ఒకవేళ (r == FAIL)
	retval = ఖాళీ;
    లేకపోతే
    {
	retval = typval2string(&tv, join_list);
	clear_tv(&tv);
    }
    clear_evalarg(&evalarg, ఖాళీ);

    తిరిగి retval;
}

    char_u *
eval_గమ్యం_string(
    char_u	*arg,
    పూర్ణాంకం		join_list,
    పూర్ణాంకం		use_simple_function)
{
    తిరిగి eval_గమ్యం_string_eap(arg, join_list, ఖాళీ, use_simple_function);
}

/*
 * Call eval_to_string() without using current local variables and using
 * textlock.  When "use_sandbox" is TRUE use the sandbox.
 * Use legacy Vim script syntax.
 */
    char_u *
eval_గమ్యం_string_సురక్షిత(
    char_u	*arg,
    పూర్ణాంకం		use_sandbox,
    పూర్ణాంకం		keep_script_సంస్కరణ,
    పూర్ణాంకం		use_simple_function)
{
    char_u	*retval;
    funccal_నమోదు_T funccal_నమోదు;
    పూర్ణాంకం		save_sc_సంస్కరణ = current_sctx.sc_సంస్కరణ;
    పూర్ణాంకం		save_garbage = may_garbage_collect;

    ఒకవేళ (!keep_script_సంస్కరణ)
	current_sctx.sc_సంస్కరణ = 1;
    save_funccal(&funccal_నమోదు);
    ఒకవేళ (use_sandbox)
	++sandbox;
    ++textlock;
    may_garbage_collect = FALSE;
    retval = eval_గమ్యం_string(arg, FALSE, use_simple_function);
    ఒకవేళ (use_sandbox)
	--sandbox;
    --textlock;
    may_garbage_collect = save_garbage;
    restore_funccal();
    current_sctx.sc_సంస్కరణ = save_sc_సంస్కరణ;
    తిరిగి retval;
}

/*
 * Top level evaluation function, returning a number.
 * Evaluates "expr" silently.
 * Returns -1 for an error.
 */
    varnumber_T
eval_గమ్యం_సంఖ్య(char_u *సమాసం, పూర్ణాంకం use_simple_function)
{
    typval_T	rettv;
    varnumber_T	retval;
    char_u	*p = skipwhite(సమాసం);
    పూర్ణాంకం		r = NOTDONE;

    ++emsg_off;

    ఒకవేళ (use_simple_function)
	r = may_call_simple_func(సమాసం, &rettv);
    ఒకవేళ (r == NOTDONE)
	r = eval1(&p, &rettv, &EVALARG_EVALUATE);
    ఒకవేళ (r == FAIL)
	retval = -1;
    లేకపోతే
    {
	retval = tv_get_సంఖ్య_chk(&rettv, ఖాళీ);
	clear_tv(&rettv);
    }
    --emsg_off;

    తిరిగి retval;
}

/*
 * Top level evaluation function.
 * Returns an allocated typval_T with the result.
 * Returns NULL when there is an error.
 */
    typval_T *
eval_సమాసం(char_u *arg, exarg_T *eap)
{
    తిరిగి eval_సమాసం_ext(arg, eap, FALSE);
}

    typval_T *
eval_సమాసం_ext(char_u *arg, exarg_T *eap, పూర్ణాంకం use_simple_function)
{
    typval_T	*tv;
    evalarg_T	evalarg;

    fill_evalarg_from_eap(&evalarg, eap, eap != ఖాళీ && eap->skip);

    tv = ALLOC_ONE(typval_T);
    ఒకవేళ (tv != ఖాళీ)
    {
	పూర్ణాంకం	r = NOTDONE;

	ఒకవేళ (use_simple_function)
	    r = eval0_simple_funccal(arg, tv, eap, &evalarg);
	ఒకవేళ (r == NOTDONE)
	    r = eval0(arg, tv, eap, &evalarg);

	ఒకవేళ (r == FAIL)
	    VIM_CLEAR(tv);
    }

    clear_evalarg(&evalarg, eap);
    తిరిగి tv;
}

/*
 * "*arg" points to what can be a function name in the form of "import.Name" or
 * "Funcref".  Return the name of the function.  Set "tofree" to something that
 * was allocated.
 * If "verbose" is FALSE no errors are given.
 * Return NULL for any failure.
 */
    స్థిర char_u *
deref_function_name(
	    char_u	**arg,
	    char_u	**tofree,
	    evalarg_T	*evalarg,
	    పూర్ణాంకం		verbose)
{
    typval_T	ref;
    char_u	*name = *arg;
    పూర్ణాంకం		save_flags = 0;
    పూర్ణాంకం		evaluate = evalarg != ఖాళీ
				      && (evalarg->eval_flags & EVAL_EVALUATE);

    ref.v_type = VAR_UNKNOWN;
    ఒకవేళ (evalarg != ఖాళీ)
    {
	// need to evaluate this to get an import, like in "a.Func"
	save_flags = evalarg->eval_flags;
	evalarg->eval_flags |= EVAL_EVALUATE;
    }
    ఒకవేళ (eval9(arg, &ref, evalarg, FALSE) == FAIL)
    {
	dictitem_T	*v;

	// If <SID>VarName was used it would not be found, try another way.
	v = కనుగొను_var_also_లోపల_script(name, ఖాళీ, FALSE);
	ఒకవేళ (v == ఖాళీ)
	{
	    name = ఖాళీ;
	    వెళ్ళు theend;
	}
	copy_tv(&v->di_tv, &ref);
    }
    ఒకవేళ (*skipwhite(*arg) != NUL)
    {
	ఒకవేళ (verbose)
	    semsg(_(e_trailing_characters_str), *arg);
	name = ఖాళీ;
    }
    లేకపోతే ఒకవేళ (ref.v_type == VAR_FUNC && ref.vval.v_string != ఖాళీ)
    {
	name = ref.vval.v_string;
	ref.vval.v_string = ఖాళీ;
	*tofree = name;
    }
    లేకపోతే ఒకవేళ (ref.v_type == VAR_PARTIAL && ref.vval.v_partial != ఖాళీ)
    {
	ఒకవేళ (ref.vval.v_partial->pt_argc > 0
		|| ref.vval.v_partial->pt_dict != ఖాళీ)
	{
	    ఒకవేళ (verbose)
		emsg(_(e_cannot_use_partial_ఇక్కడ));
	    name = ఖాళీ;
	}
	లేకపోతే
	{
	    name = vim_strsave(partial_name(ref.vval.v_partial));
	    *tofree = name;
	}
    }
    లేకపోతే ఒకవేళ (evaluate)
    {
	ఒకవేళ (verbose)
	    semsg(_(e_not_callable_type_str), name);
	name = ఖాళీ;
    }

theend:
    clear_tv(&ref);
    ఒకవేళ (evalarg != ఖాళీ)
	evalarg->eval_flags = save_flags;
    తిరిగి name;
}

/*
 * Call some Vim script function and return the result in "*rettv".
 * Uses argv[0] to argv[argc - 1] for the function arguments.  argv[argc]
 * should have type VAR_UNKNOWN.
 * Returns OK or FAIL.
 */
    పూర్ణాంకం
call_vim_function(
    char_u      *func,
    పూర్ణాంకం		argc,
    typval_T	*argv,
    typval_T	*rettv)
{
    పూర్ణాంకం		ret;
    funcexe_T	funcexe;
    char_u	*arg;
    char_u	*name;
    char_u	*tofree = ఖాళీ;
    పూర్ణాంకం		ignore_errors;

    rettv->v_type = VAR_UNKNOWN;		// clear_tv() uses this
    CLEAR_క్షేత్రం(funcexe);
    funcexe.fe_firstline = curwin->w_cursor.lnum;
    funcexe.fe_lastline = curwin->w_cursor.lnum;
    funcexe.fe_evaluate = TRUE;

    // The name might be "import.Func" or "Funcref".  We don't know, we need to
    // ignore errors for an undefined name.  But we do want errors when an
    // autoload script has errors.  Guess that when there is a dot in the name
    // showing errors is the right choice.
    ignore_errors = vim_strchr(func, '.') == ఖాళీ;
    arg = func;
    ఒకవేళ (ignore_errors)
	++emsg_off;
    name = deref_function_name(&arg, &tofree, &EVALARG_EVALUATE, FALSE);
    ఒకవేళ (ignore_errors)
	--emsg_off;
    ఒకవేళ (name == ఖాళీ)
	name = func;

    ret = call_func(name, -1, rettv, argc, argv, &funcexe);

    ఒకవేళ (ret == FAIL)
	clear_tv(rettv);
    vim_free(tofree);

    తిరిగి ret;
}

/*
 * Call Vim script function "func" and return the result as a string.
 * Uses "argv[0]" to "argv[argc - 1]" for the function arguments. "argv[argc]"
 * should have type VAR_UNKNOWN.
 * Returns NULL when calling the function fails.
 */
    శూన్య *
call_func_retstr(
    char_u      *func,
    పూర్ణాంకం		argc,
    typval_T	*argv)
{
    typval_T	rettv;
    char_u	*retval;

    ఒకవేళ (call_vim_function(func, argc, argv, &rettv) == FAIL)
	తిరిగి ఖాళీ;

    retval = vim_strsave(tv_get_string(&rettv));
    clear_tv(&rettv);
    తిరిగి retval;
}

/*
 * Call Vim script function "func" and return the result as a List.
 * Uses "argv" and "argc" as call_func_retstr().
 * Returns NULL when there is something wrong.
 * Gives an error when the returned value is not a list.
 */
    శూన్య *
call_func_retlist(
    char_u      *func,
    పూర్ణాంకం		argc,
    typval_T	*argv)
{
    typval_T	rettv;

    ఒకవేళ (call_vim_function(func, argc, argv, &rettv) == FAIL)
	తిరిగి ఖాళీ;

    ఒకవేళ (rettv.v_type != VAR_LIST)
    {
	semsg(_(e_custom_list_completion_function_does_not_return_list_but_str),
		vartype_name(rettv.v_type));
	clear_tv(&rettv);
	తిరిగి ఖాళీ;
    }

    తిరిగి rettv.vval.v_list;
}

#ఒకవేళ defined(FEAT_FOLDING)
/*
 * Evaluate "arg", which is 'foldexpr'.
 * Note: caller must set "curwin" to match "arg".
 * Returns the foldlevel, and any character preceding it in "*cp".  Doesn't
 * give error messages.
 */
    పూర్ణాంకం
eval_foldexpr(win_T *wp, పూర్ణాంకం *cp)
{
    char_u	*arg;
    typval_T	tv;
    varnumber_T	retval;
    char_u	*s;
    sctx_T	saved_sctx = current_sctx;
    పూర్ణాంకం		use_sandbox = was_సమితి_insecurely(wp, (char_u *)"foldexpr",
								    OPT_LOCAL);

    arg = skipwhite(wp->w_p_fde);
    current_sctx = wp->w_p_script_ctx[WV_FDE];

    ++emsg_off;
    ఒకవేళ (use_sandbox)
	++sandbox;
    ++textlock;
    *cp = NUL;

    // Evaluate the expression.  If the expression is "FuncName()" call the
    // function directly.
    ఒకవేళ (eval0_simple_funccal(arg, &tv, ఖాళీ, &EVALARG_EVALUATE) == FAIL)
	retval = 0;
    లేకపోతే
    {
	// If the result is a number, just return the number.
	ఒకవేళ (tv.v_type == VAR_సంఖ్య)
	    retval = tv.vval.v_సంఖ్య;
	లేకపోతే ఒకవేళ (tv.v_type != VAR_STRING || tv.vval.v_string == ఖాళీ)
	    retval = 0;
	లేకపోతే
	{
	    // If the result is a string, check if there is a non-digit before
	    // the number.
	    s = tv.vval.v_string;
	    ఒకవేళ (*s != NUL && !VIM_ISDIGIT(*s) && *s != '-')
		*cp = *s++;
	    retval = atol((అక్షరం *)s);
	}
	clear_tv(&tv);
    }
    --emsg_off;
    ఒకవేళ (use_sandbox)
	--sandbox;
    --textlock;
    clear_evalarg(&EVALARG_EVALUATE, ఖాళీ);
    current_sctx = saved_sctx;

    తిరిగి (పూర్ణాంకం)retval;
}
#అయితే_ముగింపు

#నిర్వచితమైతే LOG_LOCKVAR
రకనిర్వచనం నిర్మాణం flag_string_S
{
    పూర్ణాంకం	    flag;
    అక్షరం    *str;
} flag_string_T;

    స్థిర అక్షరం *
flags_tostring(పూర్ణాంకం flags, flag_string_T *_fstring, అక్షరం *buf, size_t n)
{
    అక్షరం *p = buf;
    *p = NUL;
    కొరకు (flag_string_T *fstring = _fstring; fstring->flag; ++fstring)
    {
	ఒకవేళ ((fstring->flag & flags) != 0)
	{
	    size_t len = STRLEN(fstring->str);
	    ఒకవేళ (n > p - buf + len + 7)
	    {
		STRCAT(p, fstring->str);
		p += len;
		STRCAT(p, " ");
		++p;
	    }
	    లేకపోతే
	    {
		STRCAT(buf, "...");
		ఆపు;
	    }
	}
    }
    తిరిగి buf;
}

flag_string_T glv_flag_అక్షరశ్రేణులు[] = {
    { GLV_QUIET,		"QUIET" },
    { GLV_NO_AUTOLOAD,		"NO_AUTOLOAD" },
    { GLV_READ_ONLY,		"READ_ONLY" },
    { GLV_NO_DECL,		"NO_DECL" },
    { GLV_COMPILING,		"COMPILING" },
    { GLV_కేటాయించు_WITH_OP,	"ASSIGN_WITH_OP" },
    { GLV_PREFER_FUNC,		"PREFER_FUNC" },
    { 0,			ఖాళీ }
};
#అయితే_ముగింపు

/*
 * Fill in "lp" using "root". This is used in a special case when
 * "get_lval()" parses a bare word when "lval_root" is not NULL.
 *
 * This is typically called with "lval_root" as "root". For a class, find
 * the name from lp in the class from root, fill in lval_T if found. For a
 * complex type, list/tuple/dict use it as the result; just put the root into
 * ll_tv.
 *
 * "lval_root" is a hack used during run-time/instr-execution to provide the
 * starting point for "get_lval()" to traverse a chain of indexes. In some
 * cases get_lval sees a bare name and uses this function to populate the
 * lval_T.
 *
 * For setting up "lval_root" (currently only used with lockvar)
 *	compile_lock_unlock - pushes object on stack (which becomes lval_root)
 *	execute_instructions: ISN_LOCKUNLOCK - sets lval_root from stack.
 */
    స్థిర శూన్య
fill_lval_from_lval_మూలం(lval_T *lp, lval_మూలం_T *lr)
{
#నిర్వచితమైతే LOG_LOCKVAR
    ch_log(ఖాళీ, "LKVAR: fill_lval_from_lval_root(): name %s, tv %p",
						lp->ll_name, (శూన్య*)lr->lr_tv);
#అయితే_ముగింపు
    ఒకవేళ (lr->lr_tv == ఖాళీ)
	తిరిగి;
    ఒకవేళ (!lr->lr_is_arg && lr->lr_tv->v_type == VAR_CLASS)
    {
	ఒకవేళ (lr->lr_tv->vval.v_class != ఖాళీ)
	{
	    // Special special case. Look for a bare class variable reference.
	    class_T	*cl = lr->lr_tv->vval.v_class;
	    పూర్ణాంకం		m_idx;
	    ocmember_T	*m = class_member_lookup(cl, lp->ll_name,
					lp->ll_name_చివర - lp->ll_name, &m_idx);
	    ఒకవేళ (m != ఖాళీ)
	    {
		// Assuming "inside class" since bare reference.
		lp->ll_class = lr->lr_tv->vval.v_class;
		lp->ll_oi = m_idx;
		lp->ll_valtype = m->ocm_type;
		lp->ll_tv = &lp->ll_class->class_members_tv[m_idx];
#నిర్వచితమైతే LOG_LOCKVAR
		ch_log(ఖాళీ, "LKVAR:    ... class member %s.%s",
					lp->ll_class->class_name.string, lp->ll_name);
#అయితే_ముగింపు
		తిరిగి;
	    }
	}
    }

#నిర్వచితమైతే LOG_LOCKVAR
    ch_log(ఖాళీ, "LKVAR:    ... type: %s", vartype_name(lr->lr_tv->v_type));
#అయితే_ముగింపు
    lp->ll_tv = lr->lr_tv;
    lp->ll_is_మూలం = TRUE;
}

/*
 * Check if the class has permission to access the member.
 * Returns OK or FAIL.
 */
    స్థిర పూర్ణాంకం
get_lval_check_access(
    class_T	*cl_exec,   // executing class, NULL if :def or script level
    class_T	*cl,	    // class which contains the member
    ocmember_T	*om,	    // member being accessed
    char_u	*p,	    // char after member name
    పూర్ణాంకం		flags)	    // GLV flags to check if writing to lval
{
#నిర్వచితమైతే LOG_LOCKVAR
    ch_log(ఖాళీ, "LKVAR: get_lval_check_access(), cl_exec %p, cl %p, %c",
						(శూన్య*)cl_exec, (శూన్య*)cl, *p);
#అయితే_ముగింపు
    ఒకవేళ (cl_exec != ఖాళీ && cl_exec == cl)
	తిరిగి OK;

    అక్షరం *సందేశం = ఖాళీ;
    ఎంపిక (om->ocm_access)
    {
	స్థితి VIM_ACCESS_వ్యక్తిగత:
	    సందేశం = e_cannot_access_protected_variable_str;
	    ఆపు;
	స్థితి VIM_ACCESS_READ:
	    // If [idx] or .key following, read only OK.
	    ఒకవేళ (*p == '[' || *p == '.')
		ఆపు;
	    ఒకవేళ ((flags & GLV_READ_ONLY) == 0)
	    {
		ఒకవేళ (IS_ENUM(cl))
		{
		    ఒకవేళ (om->ocm_type->tt_type == VAR_OBJECT)
			semsg(_(e_enumvalue_str_cannot_be_modified),
				cl->class_name.string, om->ocm_name.string);
		    లేకపోతే
			సందేశం = e_variable_is_not_writable_str;
		}
		లేకపోతే
		    సందేశం = e_variable_is_not_writable_str;
	    }
	    ఆపు;
	స్థితి VIM_ACCESS_ALL:
	    ఆపు;
    }
    ఒకవేళ (సందేశం != ఖాళీ)
    {
	emsg_var_cl_define(సందేశం, om->ocm_name.string, 0, cl);
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * Get lval information for a variable imported from script "imp_sid".  On
 * success, updates "lp" with the variable name, type, script ID and typval.
 * The variable name starts at or after "p".
 * If "rettv" is not NULL it points to the value to be assigned.  This used to
 * match the rhs and lhs types.
 * Returns a pointer to the character after the variable name if the imported
 * variable is valid and writable.
 * Returns NULL if the variable is not exported or typval is not found or the
 * rhs type doesn't match the lhs type or the variable is not writable.
 */
    స్థిర char_u *
get_lval_imported(
    lval_T	*lp,
    scid_T	imp_sid,
    char_u	*p,
    dictitem_T	**dip,
    పూర్ణాంకం		fne_flags)
{
    ufunc_T	*ufunc;
    type_T	*type = ఖాళీ;
    పూర్ణాంకం		cc;
    పూర్ణాంకం		rc = FAIL;

    p = skipwhite(p);

    import_check_sourced_sid(&imp_sid);
    lp->ll_sid = imp_sid;
    lp->ll_name = p;
    p = కనుగొను_name_చివర(lp->ll_name, ఖాళీ, ఖాళీ, fne_flags);
    lp->ll_name_చివర = p;

    // check the item is exported
    cc = *p;
    *p = NUL;
    ఒకవేళ (కనుగొను_exported(imp_sid, lp->ll_name, &ufunc, &type, ఖాళీ, ఖాళీ,
								TRUE) == -1)
	వెళ్ళు failed;

    // Get the typval for the exported item
    hashtab_T *ht = &SCRIPT_VARS(imp_sid);
    ఒకవేళ (ht == ఖాళీ)
	వెళ్ళు failed;

    dictitem_T *di = కనుగొను_var_లోపల_ht(ht, 0, lp->ll_name, TRUE);
    ఒకవేళ (di == ఖాళీ)
	// script is autoloaded.  So variable will be found later
	వెళ్ళు success;

    *dip = di;

    // Check whether the variable is writable.
    svar_T *sv = కనుగొను_typval_లోపల_script(&di->di_tv, imp_sid, FALSE);
    ఒకవేళ (sv != ఖాళీ && sv->sv_const != 0)
    {
	semsg(_(e_cannot_change_readonly_variable_str), lp->ll_name);
	వెళ్ళు failed;
    }

    // check whether variable is locked
    ఒకవేళ (విలువ_check_తాళం(di->di_tv.v_తాళం, lp->ll_name, FALSE))
	వెళ్ళు failed;

    lp->ll_tv = &di->di_tv;
    lp->ll_valtype = type;

success:
    rc = OK;

failed:
    *p = cc;
    తిరిగి rc == OK ? p : ఖాళీ;
}

రకనిర్వచనం గణన {
    GLV_FAIL,
    GLV_OK,
    GLV_STOP
} glv_స్థితి_T;

/*
 * Get an Dict lval variable that can be assigned a value to: "name",
 * "name[expr]", "name[expr][expr]", "name.key", "name.key[expr]" etc.
 * "name" points to the start of the name.
 * If "rettv" is not NULL it points to the value to be assigned.
 * "unlet" is TRUE for ":unlet": slightly different behavior when something is
 * wrong; must end in space or cmd separator.
 *
 * flags:
 *  GLV_QUIET:       do not give error messages
 *  GLV_READ_ONLY:   will not change the variable
 *  GLV_NO_AUTOLOAD: do not use script autoloading
 *
 * The Dict is returned in 'lp'.  Returns GLV_OK on success and GLV_FAIL on
 * failure.  Returns GLV_STOP to stop processing the characters following
 * 'key_end'.
 */
    స్థిర పూర్ణాంకం
get_lval_dict_అంశం(
    lval_T	*lp,
    char_u	*name,
    char_u	*కీలకం,
    పూర్ణాంకం		len,
    char_u	**కీలకం_చివర,
    typval_T	*var1,
    పూర్ణాంకం		flags,
    పూర్ణాంకం		unlet,
    typval_T	*rettv)
{
    పూర్ణాంకం		quiet = flags & GLV_QUIET;
    char_u	*p = *కీలకం_చివర;

    ఒకవేళ (len == -1)
    {
	// "[key]": get key from "var1"
	కీలకం = tv_get_string_chk(var1);	// is number or string
	ఒకవేళ (కీలకం == ఖాళీ)
	    తిరిగి GLV_FAIL;
    }
    lp->ll_list = ఖాళీ;
    lp->ll_list = ఖాళీ;
    lp->ll_blob = ఖాళీ;
    lp->ll_object = ఖాళీ;
    lp->ll_class = ఖాళీ;
    lp->ll_tuple = ఖాళీ;

    // a NULL dict is equivalent with an empty dict
    ఒకవేళ (lp->ll_tv->vval.v_dict == ఖాళీ)
    {
	lp->ll_tv->vval.v_dict = dict_alloc();
	ఒకవేళ (lp->ll_tv->vval.v_dict == ఖాళీ)
	    తిరిగి GLV_FAIL;
	++lp->ll_tv->vval.v_dict->dv_refcount;
    }
    lp->ll_dict = lp->ll_tv->vval.v_dict;

    lp->ll_di = dict_కనుగొను(lp->ll_dict, కీలకం, len);

    // When assigning to a scope dictionary check that a function and
    // variable name is valid (only variable name unless it is l: or
    // g: dictionary). Disallow overwriting a builtin function.
    ఒకవేళ (rettv != ఖాళీ && lp->ll_dict->dv_scope != 0)
    {
	పూర్ణాంకం prevval;

	ఒకవేళ (len != -1)
	{
	    prevval = కీలకం[len];
	    కీలకం[len] = NUL;
	}
	లేకపోతే
	    prevval = 0; // avoid compiler warning
	పూర్ణాంకం wrong = (lp->ll_dict->dv_scope == VAR_DEF_SCOPE
		&& (rettv->v_type == VAR_FUNC
		    || rettv->v_type == VAR_PARTIAL)
		&& var_wrong_func_name(కీలకం, lp->ll_di == ఖాళీ))
	    || !valid_varname(కీలకం, -1, TRUE);
	ఒకవేళ (len != -1)
	    కీలకం[len] = prevval;
	ఒకవేళ (wrong)
	    తిరిగి GLV_FAIL;
    }

    ఒకవేళ (lp->ll_valtype != ఖాళీ)
	// use the type of the member
	lp->ll_valtype = lp->ll_valtype->tt_member;

    ఒకవేళ (lp->ll_di == ఖాళీ)
    {
	// Can't add "v:" or "a:" variable.
	ఒకవేళ (lp->ll_dict == get_vimvar_dict()
		|| &lp->ll_dict->dv_hashtab == get_funccal_వాదనలు_ht())
	{
	    semsg(_(e_illegal_variable_name_str), name);
	    తిరిగి GLV_FAIL;
	}

	// Key does not exist in dict: may need to add it.
	ఒకవేళ (*p == '[' || *p == '.' || unlet)
	{
	    ఒకవేళ (!quiet)
		semsg(_(e_కీలకం_not_present_లోపల_dictionary_str), కీలకం);
	    తిరిగి GLV_FAIL;
	}
	ఒకవేళ (len == -1)
	    lp->ll_newkey = vim_strsave(కీలకం);
	లేకపోతే
	    lp->ll_newkey = vim_strnsave(కీలకం, len);
	ఒకవేళ (lp->ll_newkey == ఖాళీ)
	    p = ఖాళీ;

	*కీలకం_చివర = p;
	తిరిగి GLV_STOP;
    }
    // existing variable, need to check if it can be changed
    లేకపోతే ఒకవేళ ((flags & GLV_READ_ONLY) == 0
	    && (var_check_ro(lp->ll_di->di_flags, name, FALSE)
		|| var_check_తాళం(lp->ll_di->di_flags, name, FALSE)))
	తిరిగి GLV_FAIL;

    lp->ll_tv = &lp->ll_di->di_tv;

    తిరిగి GLV_OK;
}

/*
 * Get an blob lval variable that can be assigned a value to: "name",
 * "na{me}", "name[expr]", "name[expr:expr]", "name[expr][expr]", etc.
 *
 * 'var1' specifies the starting blob index and 'var2' specifies the ending
 * blob index.  If the first index is not specified in a range, then 'empty1'
 * is TRUE.  If 'quiet' is TRUE, then error messages are not displayed for
 * invalid indexes.
 *
 * The blob is returned in 'lp'.  Returns OK on success and FAIL on failure.
 */
    స్థిర పూర్ణాంకం
get_lval_blob(
    lval_T	*lp,
    typval_T	*var1,
    typval_T	*var2,
    పూర్ణాంకం		empty1,
    పూర్ణాంకం		quiet)
{
    దీర్ఘ	bloblen = blob_len(lp->ll_tv->vval.v_blob);

    lp->ll_list = ఖాళీ;
    lp->ll_dict = ఖాళీ;
    lp->ll_object = ఖాళీ;
    lp->ll_class = ఖాళీ;
    lp->ll_tuple = ఖాళీ;

    // Get the number and item for the only or first index of a List or Tuple.
    ఒకవేళ (empty1)
	lp->ll_n1 = 0;
    లేకపోతే
	// is number or string
	lp->ll_n1 = (దీర్ఘ)tv_get_సంఖ్య(var1);

    ఒకవేళ (check_blob_సూచీ(bloblen, lp->ll_n1, quiet) == FAIL)
	తిరిగి FAIL;
    ఒకవేళ (lp->ll_range && !lp->ll_empty2)
    {
	lp->ll_n2 = (దీర్ఘ)tv_get_సంఖ్య(var2);
	ఒకవేళ (check_blob_range(bloblen, lp->ll_n1, lp->ll_n2, quiet) == FAIL)
	    తిరిగి FAIL;
    }

    ఒకవేళ (!lp->ll_range)
	// Indexing a single byte in a blob.  So the rhs type is a
	// number.
	lp->ll_valtype = &t_సంఖ్య;

    lp->ll_blob = lp->ll_tv->vval.v_blob;
    lp->ll_tv = ఖాళీ;

    తిరిగి OK;
}

/*
 * Get a List lval variable that can be assigned a value to: "name",
 * "na{me}", "name[expr]", "name[expr:expr]", "name[expr][expr]", etc.
 *
 * 'var1' specifies the starting List index and 'var2' specifies the ending
 * List index.  If the first index is not specified in a range, then 'empty1'
 * is TRUE.  If 'quiet' is TRUE, then error messages are not displayed for
 * invalid indexes.
 *
 * The List is returned in 'lp'.  Returns OK on success and FAIL on failure.
 */
    స్థిర పూర్ణాంకం
get_lval_list(
    lval_T	*lp,
    typval_T	*var1,
    typval_T	*var2,
    పూర్ణాంకం		empty1,
    పూర్ణాంకం		flags,
    పూర్ణాంకం		quiet)
{
    /*
     * Get the number and item for the only or first index of the List.
     */
    ఒకవేళ (empty1)
	lp->ll_n1 = 0;
    లేకపోతే
	// is number or string
	lp->ll_n1 = (దీర్ఘ)tv_get_సంఖ్య(var1);

    lp->ll_dict = ఖాళీ;
    lp->ll_object = ఖాళీ;
    lp->ll_class = ఖాళీ;
    lp->ll_tuple = ఖాళీ;
    lp->ll_list = lp->ll_tv->vval.v_list;
    lp->ll_li = check_range_సూచీ_one(lp->ll_list, &lp->ll_n1,
				(flags & GLV_కేటాయించు_WITH_OP) == 0, quiet);
    ఒకవేళ (lp->ll_li == ఖాళీ)
	తిరిగి FAIL;

    ఒకవేళ (lp->ll_valtype != ఖాళీ && !lp->ll_range)
    {
	// use the type of the member
	ఒకవేళ (lp->ll_valtype->tt_member != ఖాళీ)
	    lp->ll_valtype = lp->ll_valtype->tt_member;
	లేకపోతే
	    // If the LHS member type is not known (VAR_ANY), then get it from
	    // the list item (after indexing)
	    lp->ll_valtype = typval2type(&lp->ll_li->li_tv, get_copyID(),
					 &lp->ll_type_list, TVTT_DO_MEMBER);

    }

    /*
     * May need to find the item or absolute index for the second
     * index of a range.
     * When no index given: "lp->ll_empty2" is TRUE.
     * Otherwise "lp->ll_n2" is set to the second index.
     */
    ఒకవేళ (lp->ll_range && !lp->ll_empty2)
    {
	lp->ll_n2 = (దీర్ఘ)tv_get_సంఖ్య(var2);
	// is number or string
	ఒకవేళ (check_range_సూచీ_two(lp->ll_list,
			&lp->ll_n1, lp->ll_li, &lp->ll_n2, quiet) == FAIL)
	    తిరిగి FAIL;
    }

    lp->ll_tv = &lp->ll_li->li_tv;

    తిరిగి OK;
}

/*
 * Get a tuple lval variable that can be assigned a value to: "name",
 * "na{me}", "name[expr]", "name[expr][expr]", etc.
 *
 * 'idx' specifies the tuple index.
 * If 'quiet' is TRUE, then error messages are not displayed for an invalid
 * index.
 *
 * The typval is returned in 'lp'.  Returns GLV_OK on success and GLV_FAIL on
 * failure.
 */
    స్థిర పూర్ణాంకం
get_lval_tuple(
    lval_T	*lp,
    typval_T	*idx,
    పూర్ణాంకం		quiet)
{
    // is number or string
    lp->ll_n1 = (దీర్ఘ)tv_get_సంఖ్య(idx);

    lp->ll_list = ఖాళీ;
    lp->ll_dict = ఖాళీ;
    lp->ll_blob = ఖాళీ;
    lp->ll_object = ఖాళీ;
    lp->ll_class = ఖాళీ;

    lp->ll_tuple = lp->ll_tv->vval.v_tuple;
    lp->ll_tv = tuple_కనుగొను(lp->ll_tuple, lp->ll_n1);
    ఒకవేళ (lp->ll_tv == ఖాళీ)
    {
	ఒకవేళ (!quiet)
	    semsg(_(e_tuple_సూచీ_బహిర్గతం_యొక్క_range_nr), lp->ll_n1);
	తిరిగి GLV_FAIL;
    }

    // use the type of the member
    ఒకవేళ (lp->ll_valtype != ఖాళీ)
    {
	ఒకవేళ (lp->ll_valtype != ఖాళీ
		&& lp->ll_valtype->tt_type == VAR_TUPLE
		&& lp->ll_valtype->tt_argcount == 1)
	{
	    // a variadic tuple or a single item tuple
	    ఒకవేళ (lp->ll_valtype->tt_flags & TTFLAG_VARARGS)
		lp->ll_valtype = lp->ll_valtype->tt_వాదనలు[0]->tt_member;
	    లేకపోతే
		lp->ll_valtype = lp->ll_valtype->tt_వాదనలు[0];
	}
	లేకపోతే
	    // If the LHS member type is not known (VAR_ANY), then get it from
	    // the tuple item (after indexing)
	    lp->ll_valtype = typval2type(lp->ll_tv, get_copyID(),
					&lp->ll_type_list, TVTT_DO_MEMBER);
    }

    తిరిగి GLV_OK;
}

/*
 * Get a class or object lval method in class "cl".  The 'key' argument points
 * to the method name and 'key_end' points to the character after 'key'.
 * 'v_type' is VAR_CLASS or VAR_OBJECT.
 *
 * The method index, method function pointer and method type are returned in
 * "lp".
 */
    స్థిర పూర్ణాంకం
get_lval_oc_పద్ధతి(
    lval_T	*lp,
    class_T	*cl,
    char_u	*కీలకం,
    char_u	**కీలకం_చివర,
    vartype_T	v_type)
{
    // Look for a method with this name.
    // round 1: class functions (skipped for an object)
    // round 2: object methods
    కొరకు (పూర్ణాంకం విడత = v_type == VAR_OBJECT ? 2 : 1; విడత <= 2; ++విడత)
    {
	పూర్ణాంకం	m_idx;
	ufunc_T	*fp;

	fp = పద్ధతి_lookup(cl, విడత == 1 ? VAR_CLASS : VAR_OBJECT,
						కీలకం, *కీలకం_చివర - కీలకం, &m_idx);
	lp->ll_oi = m_idx;

	// process generic method (if present)
	ఒకవేళ (fp && (fp = eval_generic_func(fp, కీలకం, కీలకం_చివర)) == ఖాళీ)
	    తిరిగి FAIL;

	ఒకవేళ (fp != ఖాళీ)
	{
	    lp->ll_ufunc = fp;
	    lp->ll_valtype = fp->uf_func_type;
	    ఆపు;
	}
    }

    తిరిగి OK;
}

/*
 * Get a class or object lval variable in class "cl".  The "key" argument
 * points to the variable name and "key_end" points to the character after
 * "key".  "v_type" is VAR_CLASS or VAR_OBJECT.  "cl_exec" is the class that is
 * executing, or NULL.
 *
 * The variable index, typval and type are returned in "lp".  Returns FAIL if
 * the variable is not writable.  Otherwise returns OK.
 */
    స్థిర పూర్ణాంకం
get_lval_oc_variable(
    lval_T	*lp,
    class_T	*cl,
    char_u	*కీలకం,
    char_u	*కీలకం_చివర,
    vartype_T	v_type,
    class_T	*cl_exec,
    పూర్ణాంకం		flags)
{
    పూర్ణాంకం		m_idx;
    ocmember_T	*om;

    om = member_lookup(cl, v_type, కీలకం, కీలకం_చివర - కీలకం, &m_idx);
    lp->ll_oi = m_idx;
    ఒకవేళ (om == ఖాళీ)
	తిరిగి OK;

    // Check variable is accessible
    ఒకవేళ (get_lval_check_access(cl_exec, cl, om, కీలకం_చివర, flags) == FAIL)
	తిరిగి FAIL;

    // When lhs is used to modify the variable, check it is not a read-only
    // variable.
    ఒకవేళ ((flags & GLV_READ_ONLY) == 0 && (*కీలకం_చివర != '.' && *కీలకం_చివర != '[')
	    && oc_var_check_ro(cl, om))
	తిరిగి FAIL;

    lp->ll_valtype = om->ocm_type;

    ఒకవేళ (v_type == VAR_OBJECT)
	lp->ll_tv = ((typval_T *)(lp->ll_tv->vval.v_object + 1)) + m_idx;
    లేకపోతే
	lp->ll_tv = &cl->class_members_tv[m_idx];

    తిరిగి OK;
}

/*
 * Get a Class or Object lval variable or method that can be assigned a value
 * to: "name", "name.key", "name.key[expr]" etc.
 *
 * The 'key' argument points to the member name and 'key_end' points to the
 * character after 'key'.  'v_type' is VAR_CLASS or VAR_OBJECT.  'cl_exec' is
 * the class that is executing, or NULL.  If 'quiet' is TRUE, then error
 * messages are not displayed for invalid indexes.
 *
 * The Class or Object is returned in 'lp'.  Returns OK on success and FAIL on
 * failure.
 */
    స్థిర పూర్ణాంకం
get_lval_class_or_obj(
    lval_T	*lp,
    char_u	*కీలకం,
    char_u	**కీలకం_చివర,
    vartype_T	v_type,
    class_T	*cl_exec,
    పూర్ణాంకం		flags,
    పూర్ణాంకం		quiet)
{
    lp->ll_dict = ఖాళీ;
    lp->ll_list = ఖాళీ;
    lp->ll_tuple = ఖాళీ;

    class_T *cl;
    ఒకవేళ (v_type == VAR_OBJECT)
    {
	ఒకవేళ (lp->ll_tv->vval.v_object == ఖాళీ)
	{
	    ఒకవేళ (!quiet)
		emsg(_(e_using_null_object));
	    తిరిగి FAIL;
	}
	cl = lp->ll_tv->vval.v_object->obj_class;
	lp->ll_object = lp->ll_tv->vval.v_object;
    }
    లేకపోతే
    {
	cl = lp->ll_tv->vval.v_class;
	lp->ll_object = ఖాళీ;
    }
    lp->ll_class = cl;

    ఒకవేళ (cl == ఖాళీ)
	// TODO: what if class is NULL?
	తిరిగి OK;

    lp->ll_valtype = ఖాళీ;

    ఒకవేళ (flags & GLV_PREFER_FUNC)
	ఒకవేళ (get_lval_oc_పద్ధతి(lp, cl, కీలకం, కీలకం_చివర, v_type) == FAIL)
	    తిరిగి FAIL;

    // Look for object/class member variable
    ఒకవేళ (lp->ll_valtype == ఖాళీ)
    {
	ఒకవేళ (get_lval_oc_variable(lp, cl, కీలకం, *కీలకం_చివర, v_type, cl_exec, flags)
								== FAIL)
	    తిరిగి FAIL;
    }

    ఒకవేళ (lp->ll_valtype == ఖాళీ)
    {
	member_not_దొరికింది_సందేశం(cl, v_type, కీలకం, *కీలకం_చివర - కీలకం);
	తిరిగి FAIL;
    }

    తిరిగి OK;
}

/*
 * Check whether dot (".") is allowed after the variable "name" with type
 * "v_type".  Only Dict, Class and Object types support a dot after the name.
 * Returns TRUE if dot is allowed after the name.
 */
    స్థిర పూర్ణాంకం
dot_allowed_after_type(char_u *name, vartype_T v_type, పూర్ణాంకం quiet)
{
    ఒకవేళ (v_type != VAR_DICT && v_type != VAR_OBJECT && v_type != VAR_CLASS)
    {
	ఒకవేళ (!quiet)
	    semsg(_(e_dot_not_allowed_after_str_str),
		    vartype_name(v_type), name);
	తిరిగి FALSE;
    }

    తిరిగి TRUE;
}

/*
 * Check whether left bracket ("[") is allowed after the variable "name" with
 * type "v_type".  Only Dict, List, Tuple and Blob types support a bracket
 * after the variable name.  Returns TRUE if bracket is allowed after the name.
 */
    స్థిర పూర్ణాంకం
bracket_allowed_after_type(char_u *name, vartype_T v_type, పూర్ణాంకం quiet)
{
    ఒకవేళ (v_type == VAR_CLASS || v_type == VAR_OBJECT)
    {
	ఒకవేళ (!quiet)
	    semsg(_(e_సూచీ_not_allowed_after_str_str),
		    vartype_name(v_type), name);
	తిరిగి FALSE;
    }

    తిరిగి TRUE;
}

/*
 * Check whether the variable "name" with type "v_type" can be followed by an
 * index.  Only Dict, List, Tuple, Blob, Object and Class types support
 * indexing.  Returns TRUE if indexing is allowed after the name.
 */
    స్థిర పూర్ణాంకం
సూచీ_allowed_after_type(char_u *name, vartype_T v_type, పూర్ణాంకం quiet)
{
    ఒకవేళ (v_type != VAR_LIST
	    && v_type != VAR_TUPLE
	    && v_type != VAR_DICT
	    && v_type != VAR_BLOB
	    && v_type != VAR_OBJECT
	    && v_type != VAR_CLASS)
    {
	ఒకవేళ (!quiet)
	    semsg(_(e_సూచీ_not_allowed_after_str_str),
		    vartype_name(v_type), name);
	తిరిగి FALSE;
    }

    తిరిగి TRUE;
}

/*
 * Get the lval of a list/tuple/dict/blob/object/class subitem starting at "p".
 * Loop until no more [idx] or .key is following.
 *
 * If "rettv" is not NULL it points to the value to be assigned.
 * "unlet" is TRUE for ":unlet".
 *
 * Returns a pointer to the character after the subscript on success or NULL on
 * failure.
 */
    స్థిర char_u *
get_lval_subscript(
    lval_T	*lp,
    char_u	*p,
    char_u	*name,
    typval_T	*rettv,
    hashtab_T	*ht,
    dictitem_T	*v,
    పూర్ణాంకం		unlet,
    పూర్ణాంకం		flags,	    // GLV_ values
    class_T	*cl_exec)
{
    పూర్ణాంకం		quiet = flags & GLV_QUIET;
    char_u	*కీలకం = ఖాళీ;
    పూర్ణాంకం		len;
    typval_T	var1;
    typval_T	var2;
    పూర్ణాంకం		empty1 = FALSE;
    పూర్ణాంకం		rc = FAIL;

    /*
     * Loop until no more [idx] or .key is following.
     */
    var1.v_type = VAR_UNKNOWN;
    var2.v_type = VAR_UNKNOWN;

    అయితే (*p == '[' || (*p == '.' && p[1] != '=' && p[1] != '.'))
    {
	vartype_T v_type = lp->ll_tv->v_type;

	ఒకవేళ (*p == '.' && !dot_allowed_after_type(name, v_type, quiet))
	    వెళ్ళు పూర్తి;

	ఒకవేళ (*p == '[' && !bracket_allowed_after_type(name, v_type, quiet))
	    వెళ్ళు పూర్తి;

	ఒకవేళ (!సూచీ_allowed_after_type(name, v_type, quiet))
	    వెళ్ళు పూర్తి;

	// A NULL list/blob works like an empty list/blob, allocate one now.
	పూర్ణాంకం r = OK;
	ఒకవేళ (v_type == VAR_LIST && lp->ll_tv->vval.v_list == ఖాళీ)
	    r = rettv_list_alloc(lp->ll_tv);
	లేకపోతే ఒకవేళ (v_type == VAR_BLOB && lp->ll_tv->vval.v_blob == ఖాళీ)
	    r = rettv_blob_alloc(lp->ll_tv);
	ఒకవేళ (r == FAIL)
	    వెళ్ళు పూర్తి;

	ఒకవేళ (lp->ll_range)
	{
	    ఒకవేళ (!quiet)
		emsg(_(e_ముక్క_must_come_చివరి));
	    వెళ్ళు పూర్తి;
	}
#నిర్వచితమైతే LOG_LOCKVAR
	ch_log(ఖాళీ, "LKVAR: get_lval() loop: p: %s, type: %s", p,
		vartype_name(v_type));
#అయితే_ముగింపు

	ఒకవేళ (current_script_is_vim9() && lp->ll_valtype == ఖాళీ
		&& v != ఖాళీ
		&& lp->ll_tv == &v->di_tv
		&& ht != ఖాళీ && ht == get_script_local_ht())
	{
	    svar_T  *sv = కనుగొను_typval_లోపల_script(lp->ll_tv, 0, TRUE);

	    // Vim9 script local variable: get the type
	    ఒకవేళ (sv != ఖాళీ)
	    {
		lp->ll_valtype = sv->sv_type;
#నిర్వచితమైతే LOG_LOCKVAR
		ch_log(ఖాళీ, "LKVAR:    ... loop: vim9 assign type: %s",
			vartype_name(lp->ll_valtype->tt_type));
#అయితే_ముగింపు
	    }
	}

	len = -1;
	ఒకవేళ (*p == '.')
	{
	    కీలకం = p + 1;

	    కొరకు (len = 0; ASCII_ISALNUM(కీలకం[len]) || కీలకం[len] == '_'; ++len)
		;
	    ఒకవేళ (len == 0)
	    {
		ఒకవేళ (!quiet)
		    emsg(_(e_cannot_use_ఖాళీలు_కీలకం_for_dictionary));
		వెళ్ళు పూర్తి;
	    }
	    p = కీలకం + len;
	}
	లేకపోతే
	{
	    // Get the index [expr] or the first index [expr: ].
	    p = skipwhite(p + 1);
	    ఒకవేళ (*p == ':')
		empty1 = TRUE;
	    లేకపోతే
	    {
		empty1 = FALSE;
		ఒకవేళ (eval1(&p, &var1, &EVALARG_EVALUATE) == FAIL)  // recursive!
		    వెళ్ళు పూర్తి;
		ఒకవేళ (tv_get_string_chk(&var1) == ఖాళీ)
		    // not a number or string
		    వెళ్ళు పూర్తి;
		p = skipwhite(p);
	    }

	    // Optionally get the second index [ :expr].
	    ఒకవేళ (*p == ':')
	    {
		ఒకవేళ (v_type == VAR_DICT)
		{
		    ఒకవేళ (!quiet)
			emsg(_(e_cannot_ముక్క_dictionary));
		    వెళ్ళు పూర్తి;
		}
		ఒకవేళ (v_type == VAR_TUPLE)
		{
		    ఒకవేళ (!quiet)
			emsg(_(e_cannot_ముక్క_tuple));
		    వెళ్ళు పూర్తి;
		}
		ఒకవేళ (rettv != ఖాళీ
			&& !(rettv->v_type == VAR_LIST
			    && rettv->vval.v_list != ఖాళీ)
			&& !(rettv->v_type == VAR_BLOB
			    && rettv->vval.v_blob != ఖాళీ))
		{
		    ఒకవేళ (!quiet)
			emsg(_(e_ముక్క_requires_list_or_blob_విలువ));
		    వెళ్ళు పూర్తి;
		}
		p = skipwhite(p + 1);
		ఒకవేళ (*p == ']')
		    lp->ll_empty2 = TRUE;
		లేకపోతే
		{
		    lp->ll_empty2 = FALSE;
		    // recursive!
		    ఒకవేళ (eval1(&p, &var2, &EVALARG_EVALUATE) == FAIL)
			వెళ్ళు పూర్తి;
		    ఒకవేళ (tv_get_string_chk(&var2) == ఖాళీ)
			// not a number or string
			వెళ్ళు పూర్తి;
		}
		lp->ll_range = TRUE;
	    }
	    లేకపోతే
		lp->ll_range = FALSE;

	    ఒకవేళ (*p != ']')
	    {
		ఒకవేళ (!quiet)
		    emsg(_(e_missing_closing_వర్గం_brace));
		వెళ్ళు పూర్తి;
	    }

	    // Skip to past ']'.
	    ++p;
	}
#నిర్వచితమైతే LOG_LOCKVAR
	ఒకవేళ (len == -1)
	    ch_log(ఖాళీ, "LKVAR:    ... loop: p: %s, '[' key: %s", p,
		    empty1 ? ":" : (అక్షరం*)tv_get_string(&var1));
	లేకపోతే
	    ch_log(ఖాళీ, "LKVAR:    ... loop: p: %s, '.' key: %s", p, కీలకం);
#అయితే_ముగింపు

	ఒకవేళ (v_type == VAR_DICT)
	{
	    glv_స్థితి_T glv_స్థితి;

	    glv_స్థితి = get_lval_dict_అంశం(lp, name, కీలకం, len, &p, &var1,
							flags, unlet, rettv);
	    ఒకవేళ (glv_స్థితి == GLV_FAIL)
		వెళ్ళు పూర్తి;
	    ఒకవేళ (glv_స్థితి == GLV_STOP)
		ఆపు;
	}
	లేకపోతే ఒకవేళ (v_type == VAR_BLOB)
	{
	    ఒకవేళ (get_lval_blob(lp, &var1, &var2, empty1, quiet) == FAIL)
		వెళ్ళు పూర్తి;

	    ఆపు;
	}
	లేకపోతే ఒకవేళ (v_type == VAR_LIST)
	{
	    ఒకవేళ (get_lval_list(lp, &var1, &var2, empty1, flags, quiet) == FAIL)
		వెళ్ళు పూర్తి;
	}
	లేకపోతే ఒకవేళ (v_type == VAR_TUPLE)
	{
	    ఒకవేళ (get_lval_tuple(lp, &var1, quiet) == FAIL)
		వెళ్ళు పూర్తి;
	}
	లేకపోతే  // v_type == VAR_CLASS || v_type == VAR_OBJECT
	{
	    ఒకవేళ (get_lval_class_or_obj(lp, కీలకం, &p, v_type, cl_exec, flags,
							quiet) == FAIL)
		వెళ్ళు పూర్తి;
	}

	clear_tv(&var1);
	clear_tv(&var2);
	var1.v_type = VAR_UNKNOWN;
	var2.v_type = VAR_UNKNOWN;
    }

    ఒకవేళ (lp->ll_tuple != ఖాళీ && (flags & GLV_READ_ONLY) == 0)
    {
	ఒకవేళ (!quiet)
	    emsg(_(e_tuple_is_immutable));
	వెళ్ళు పూర్తి;
    }

    rc = OK;

పూర్తి:
    clear_tv(&var1);
    clear_tv(&var2);
    తిరిగి rc == OK ? p : ఖాళీ;
}

/*
 * Get an lval: variable, Dict item or List item that can be assigned a value
 * to: "name", "na{me}", "name[expr]", "name[expr:expr]", "name[expr][expr]",
 * "name.key", "name.key[expr]" etc.
 * Indexing only works if "name" is an existing List or Dictionary.
 * "name" points to the start of the name.
 * If "rettv" is not NULL it points to the value to be assigned.
 * "unlet" is TRUE for ":unlet": slightly different behavior when something is
 * wrong; must end in space or cmd separator.
 *
 * flags:
 *  GLV_QUIET:       do not give error messages
 *  GLV_READ_ONLY:   will not change the variable
 *  GLV_NO_AUTOLOAD: do not use script autoloading
 *
 * Returns a pointer to just after the name, including indexes.
 * When an evaluation error occurs "lp->ll_name" is NULL;
 * Returns NULL for a parsing error.  Still need to free items in "lp"!
 */
    char_u *
get_lval(
    char_u	*name,
    typval_T	*rettv,
    lval_T	*lp,
    పూర్ణాంకం		unlet,
    పూర్ణాంకం		skip,
    పూర్ణాంకం		flags,	    // GLV_ values
    పూర్ణాంకం		fne_flags)  // flags for find_name_end()
{
    char_u	*p;
    char_u	*సమాసం_ప్రారంభం, *సమాసం_చివర;
    పూర్ణాంకం		cc;
    dictitem_T	*v = ఖాళీ;
    hashtab_T	*ht = ఖాళీ;
    పూర్ణాంకం		quiet = flags & GLV_QUIET;
    పూర్ణాంకం		writing = 0;
    పూర్ణాంకం		vim9script = లోపల_vim9script();
    class_T	*cl_exec = ఖాళీ;    // class that is executing, or NULL.

#నిర్వచితమైతే LOG_LOCKVAR
    ఒకవేళ (lval_మూలం == ఖాళీ)
	ch_log(ఖాళీ, "LKVAR: get_lval(): name: %s, lval_root (nil)", name);
    లేకపోతే
	ch_log(ఖాళీ, "LKVAR: get_lval(): name: %s, lr_tv %p lr_is_arg %d",
			name, (శూన్య*)lval_మూలం->lr_tv, lval_మూలం->lr_is_arg);
    అక్షరం buf[80];
    ch_log(ఖాళీ, "LKVAR:    ...: GLV flags: %s",
		    flags_tostring(flags, glv_flag_అక్షరశ్రేణులు, buf, పరిమాణం(buf)));
#అయితే_ముగింపు

    // Clear everything in "lp".
    CLEAR_సూచిక(lp);
    ga_init2(&lp->ll_type_list, పరిమాణం(type_T *), 10);

    ఒకవేళ (skip || (flags & GLV_COMPILING))
    {
	// When skipping or compiling just find the end of the name.
	lp->ll_name = name;
	lp->ll_name_చివర = కనుగొను_name_చివర(name, ఖాళీ, ఖాళీ,
						      FNE_INCL_BR | fne_flags);
	తిరిగి lp->ll_name_చివర;
    }

    // Cannot use "s:var" at the Vim9 script level.  "s: type" is OK.
    ఒకవేళ (vim9script && at_script_level()
		  && name[0] == 's' && name[1] == ':' && !VIM_ISWHITE(name[2]))
    {
	semsg(_(e_cannot_use_s_colon_లోపల_vim9_script_str), name);
	తిరిగి ఖాళీ;
    }

    // Find the end of the name.
    p = కనుగొను_name_చివర(name, &సమాసం_ప్రారంభం, &సమాసం_చివర, fne_flags);
    lp->ll_name_చివర = p;
    ఒకవేళ (సమాసం_ప్రారంభం != ఖాళీ)
    {
	// Don't expand the name when we already know there is an error.
	ఒకవేళ (unlet && !VIM_ISWHITE(*p) && !చివర్లు_excmd(*p)
						    && *p != '[' && *p != '.')
	{
	    semsg(_(e_trailing_characters_str), p);
	    తిరిగి ఖాళీ;
	}

	lp->ll_exp_name = make_expanded_name(name, సమాసం_ప్రారంభం, సమాసం_చివర, p);
	ఒకవేళ (lp->ll_exp_name == ఖాళీ)
	{
	    // Report an invalid expression in braces, unless the
	    // expression evaluation has been cancelled due to an
	    // aborting error, an interrupt, or an exception.
	    ఒకవేళ (!aborting() && !quiet)
	    {
		emsg_severe = TRUE;
		semsg(_(e_invalid_వాదం_str), name);
		తిరిగి ఖాళీ;
	    }
	}
	lp->ll_name = lp->ll_exp_name;
    }
    లేకపోతే
    {
	lp->ll_name = name;

	ఒకవేళ (vim9script)
	{
	    // "a: type" is declaring variable "a" with a type, not "a:".
	    // However, "g:[key]" is indexing a dictionary.
	    ఒకవేళ (p == name + 2 && p[-1] == ':' && *p != '[')
	    {
		--p;
		lp->ll_name_చివర = p;
	    }
	    ఒకవేళ (*skipwhite(p) == ':')
	    {
		char_u	    *tp = skipwhite(p + 1);

		ఒకవేళ (is_scoped_variable(name))
		{
		    semsg(_(e_cannot_use_type_with_this_variable_str), name);
		    తిరిగి ఖాళీ;
		}
		ఒకవేళ (VIM_ISWHITE(*p))
		{
		    semsg(_(e_no_white_space_allowed_before_colon_str), p);
		    తిరిగి ఖాళీ;
		}
		ఒకవేళ (tp == p + 1 && !quiet)
		{
		    semsg(_(e_white_space_required_after_str_str), ":", p);
		    తిరిగి ఖాళీ;
		}
		ఒకవేళ (!SCRIPT_ID_VALID(current_sctx.sc_sid))
		{
		    semsg(_(e_using_type_not_లోపల_script_context_str), p);
		    తిరిగి ఖాళీ;
		}
		ఒకవేళ (vim9script && (flags & GLV_NO_DECL) &&
			!(flags & GLV_FOR_LOOP))
		{
		    // Using a type and not in a "var" declaration.
		    semsg(_(e_trailing_characters_str), p);
		    తిరిగి ఖాళీ;
		}


		// parse the type after the name
		lp->ll_type = parse_type(&tp,
			       &SCRIPT_అంశం(current_sctx.sc_sid)->sn_type_list,
			       ఖాళీ, ఖాళీ, !quiet);
		ఒకవేళ (!quiet && (lp->ll_type == ఖాళీ
			    || !valid_declaration_type(lp->ll_type)))
		    తిరిగి ఖాళీ;
		lp->ll_name_చివర = tp;
	    }
	    // TODO: check inside class?
	}
    }
    ఒకవేళ (lp->ll_name == ఖాళీ)
	తిరిగి p;

    ఒకవేళ (*p == '.')
    {
	// In legacy script, when a local variable and import exists with this name,
	// prioritize local variable over imports to avoid conflicts.
	పూర్ణాంకం var_exists = FALSE;
	ఒకవేళ (!vim9script)
	{
	    cc = *p;
	    *p = NUL;
	    hashtab_T *local_ht = get_funccal_local_ht();
	    ఒకవేళ (local_ht != ఖాళీ)
	    {
		hashitem_T *hi = hash_కనుగొను(local_ht, lp->ll_name);
		ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
		    var_exists = TRUE;
	    }
	    *p = cc;
	}

	ఒకవేళ (!var_exists)
	{
	    imported_T *import = కనుగొను_imported(lp->ll_name, p - lp->ll_name, TRUE);
	    ఒకవేళ (import != ఖాళీ)
	    {
		p++;	// skip '.'
		p = get_lval_imported(lp, import->imp_sid, p, &v, fne_flags);
		ఒకవేళ (p == ఖాళీ)
		    తిరిగి ఖాళీ;
	    }
	}
    }

    // Without [idx] or .key we are done.
    ఒకవేళ (*p != '[' && *p != '.')
    {
	ఒకవేళ (lval_మూలం != ఖాళీ)
	    fill_lval_from_lval_మూలం(lp, lval_మూలం);
	తిరిగి p;
    }

    ఒకవేళ (vim9script && lval_మూలం != ఖాళీ)
	cl_exec = lval_మూలం->lr_cl_exec;
    ఒకవేళ (vim9script && lval_మూలం != ఖాళీ && lval_మూలం->lr_tv != ఖాళీ)
    {
	// using local variable
	lp->ll_tv = lval_మూలం->lr_tv;
	v = ఖాళీ;
    }
    లేకపోతే ఒకవేళ (lp->ll_tv == ఖాళీ)
    {
	cc = *p;
	*p = NUL;
	// When we would write to the variable pass &ht and prevent autoload.
	writing = !(flags & GLV_READ_ONLY);
	v = కనుగొను_var(lp->ll_name, writing ? &ht : ఖాళీ,
					 (flags & GLV_NO_AUTOLOAD) || writing);
	ఒకవేళ (v == ఖాళీ && !quiet)
	    semsg(_(e_undefined_variable_str), lp->ll_name);
	*p = cc;
	ఒకవేళ (v == ఖాళీ)
	    తిరిగి ఖాళీ;
	lp->ll_tv = &v->di_tv;
    }

    ఒకవేళ (vim9script && (flags & GLV_NO_DECL) == 0)
    {
	ఒకవేళ (!quiet)
	    semsg(_(e_variable_already_declared_str), lp->ll_name);
	తిరిగి ఖాళీ;
    }

    // If the next character is a "." or a "[", then process the subitem.
    p = get_lval_subscript(lp, p, name, rettv, ht, v, unlet, flags, cl_exec);
    ఒకవేళ (p == ఖాళీ)
	తిరిగి ఖాళీ;

    ఒకవేళ (vim9script && lp->ll_valtype != ఖాళీ && rettv != ఖాళీ)
    {
	where_T	    where = WHERE_INIT;

	// In a Vim9 script, do type check and make sure the variable is
	// writable.
	ఒకవేళ (check_typval_type(lp->ll_valtype, rettv, where) == FAIL)
	    తిరిగి ఖాళీ;
    }

    lp->ll_name_చివర = p;
    తిరిగి p;
}

/*
 * Clear lval "lp" that was filled by get_lval().
 */
    శూన్య
clear_lval(lval_T *lp)
{
    vim_free(lp->ll_exp_name);
    vim_free(lp->ll_newkey);
    clear_type_list(&lp->ll_type_list);
}

/*
 * Set a variable that was parsed by get_lval() to "rettv".
 * "endp" points to just after the parsed name.
 * "op" is NULL, "+" for "+=", "-" for "-=", "*" for "*=", "/" for "/=",
 * "%" for "%=", "." for ".=" or "=" for "=".
 */
    శూన్య
సమితి_var_lval(
    lval_T	*lp,
    char_u	*endp,
    typval_T	*rettv,
    పూర్ణాంకం		copy,
    పూర్ణాంకం		flags,	    // ASSIGN_CONST, ASSIGN_NO_DECL
    char_u	*op,
    పూర్ణాంకం		var_idx)    // index for "let [a, b] = list"
{
    పూర్ణాంకం		cc;
    dictitem_T	*di;

    ఒకవేళ (lp->ll_tv == ఖాళీ)
    {
	cc = *endp;
	*endp = NUL;
	ఒకవేళ (లోపల_vim9script() && check_reserved_name(lp->ll_name, FALSE) == FAIL)
	    తిరిగి;

	ఒకవేళ (lp->ll_blob != ఖాళీ)
	{
	    పూర్ణాంకం	    error = FALSE;

	    ఒకవేళ (op != ఖాళీ && *op != '=')
	    {
		semsg(_(e_wrong_variable_type_for_str_equal), op);
		తిరిగి;
	    }
	    ఒకవేళ (విలువ_check_తాళం(lp->ll_blob->bv_తాళం, lp->ll_name, FALSE))
		తిరిగి;

	    ఒకవేళ (lp->ll_range && rettv->v_type == VAR_BLOB)
	    {
		ఒకవేళ (lp->ll_empty2)
		    lp->ll_n2 = blob_len(lp->ll_blob) - 1;

		ఒకవేళ (blob_సమితి_range(lp->ll_blob, lp->ll_n1, lp->ll_n2,
								rettv) == FAIL)
		    తిరిగి;
	    }
	    లేకపోతే
	    {
		varnumber_T	val = tv_get_సంఖ్య_chk(rettv, &error);
		ఒకవేళ (!error)
		{
		    ఒకవేళ (val < 0 || val > 255)
			semsg(_(e_invalid_విలువ_for_blob_nr), val);
		    లేకపోతే
			blob_సమితి_append(lp->ll_blob, lp->ll_n1, val);
		}
	    }
	}
	లేకపోతే ఒకవేళ (op != ఖాళీ && *op != '=')
	{
	    typval_T tv;

	    ఒకవేళ ((flags & (కేటాయించు_CONST | కేటాయించు_FINAL))
					     && (flags & కేటాయించు_FOR_LOOP) == 0)
	    {
		emsg(_(e_cannot_modify_existing_variable));
		*endp = cc;
		తిరిగి;
	    }

	    // handle +=, -=, *=, /=, %= and .=
	    di = ఖాళీ;
	    ఒకవేళ (eval_variable(lp->ll_name, (పూర్ణాంకం)STRLEN(lp->ll_name),
				 lp->ll_sid, &tv, &di, EVAL_VAR_VERBOSE) == OK)
	    {
		ఒకవేళ (di != ఖాళీ && check_typval_is_విలువ(&di->di_tv) == FAIL)
		{
		    clear_tv(&tv);
		    తిరిగి;
		}

		// In Vim9 script a String v: variable only takes a String.
		ఒకవేళ (లోపల_vim9script() && STRNCMP(lp->ll_name, "v:", 2) == 0
			&& tv.v_type == VAR_STRING)
		{
		    where_T	where = WHERE_INIT;

		    ఒకవేళ (check_typval_type(&t_string, rettv, where) == FAIL)
		    {
			clear_tv(&tv);
			*endp = cc;
			తిరిగి;
		    }
		}

		ఒకవేళ ((di == ఖాళీ
			 || (!var_check_ro(di->di_flags, lp->ll_name, FALSE)
			   && !tv_check_తాళం(&di->di_tv, lp->ll_name, FALSE)))
			&& tv_op(&tv, rettv, op) == OK)
		    సమితి_var_const(lp->ll_name, lp->ll_sid, ఖాళీ, &tv, FALSE,
				కేటాయించు_NO_DECL | కేటాయించు_COMPOUND_OP, 0);
		clear_tv(&tv);
	    }
	}
	లేకపోతే
	{
	    ఒకవేళ (lp->ll_type != ఖాళీ && check_typval_arg_type(lp->ll_type, rettv,
							      ఖాళీ, 0) == FAIL)
		తిరిగి;
	    సమితి_var_const(lp->ll_name, lp->ll_sid, lp->ll_type, rettv, copy,
							       flags, var_idx);
	}
	*endp = cc;
    }
    లేకపోతే ఒకవేళ (విలువ_check_తాళం(lp->ll_newkey == ఖాళీ
		? lp->ll_tv->v_తాళం
		: lp->ll_tv->vval.v_dict->dv_తాళం, lp->ll_name, FALSE))
	;
    లేకపోతే ఒకవేళ (lp->ll_range)
    {
	ఒకవేళ ((flags & (కేటాయించు_CONST | కేటాయించు_FINAL))
					     && (flags & కేటాయించు_FOR_LOOP) == 0)
	{
	    emsg(_(e_cannot_తాళం_range));
	    తిరిగి;
	}

	(శూన్య)list_కేటాయించు_range(lp->ll_list, rettv->vval.v_list,
			 lp->ll_n1, lp->ll_n2, lp->ll_empty2, op, lp->ll_name);
    }
    లేకపోతే
    {
	/*
	 * Assign to a List, Dictionary or Object item.
	 */
	ఒకవేళ ((flags & (కేటాయించు_CONST | కేటాయించు_FINAL))
					     && (flags & కేటాయించు_FOR_LOOP) == 0)
	{
	    emsg(_(e_cannot_తాళం_list_or_dict));
	    తిరిగి;
	}

	ఒకవేళ (lp->ll_valtype != ఖాళీ
		    && check_typval_arg_type(lp->ll_valtype, rettv,
							      ఖాళీ, 0) == FAIL)
	{
	    lp->ll_name_చివర = ఖాళీ;
	    తిరిగి;
	}

	// If the lval is a List and the type of the list is not yet set,
	// then set the item type from the declared type of the variable.
	ఒకవేళ (లోపల_vim9script() && rettv->v_type == VAR_LIST
				&& rettv->vval.v_list != ఖాళీ
				&& rettv->vval.v_list->lv_type == ఖాళీ)
	{
	    ఒకవేళ (lp->ll_tv->v_type == VAR_LIST
		    && lp->ll_tv->vval.v_list != ఖాళీ
		    && lp->ll_tv->vval.v_list->lv_type != ఖాళీ)
		సమితి_tv_type(rettv, lp->ll_tv->vval.v_list->lv_type);
	}

	ఒకవేళ (lp->ll_newkey != ఖాళీ)
	{
	    ఒకవేళ (op != ఖాళీ && *op != '=')
	    {
		semsg(_(e_కీలకం_not_present_లోపల_dictionary_str), lp->ll_newkey);
		తిరిగి;
	    }
	    ఒకవేళ (dict_wrong_func_name(lp->ll_tv->vval.v_dict, rettv,
								lp->ll_newkey))
		తిరిగి;

	    // Need to add an item to the Dictionary.
	    di = dictitem_alloc(lp->ll_newkey);
	    ఒకవేళ (di == ఖాళీ)
		తిరిగి;
	    ఒకవేళ (dict_కూడు(lp->ll_tv->vval.v_dict, di) == FAIL)
	    {
		vim_free(di);
		తిరిగి;
	    }
	    lp->ll_tv = &di->di_tv;
	}
	లేకపోతే ఒకవేళ (op != ఖాళీ && *op != '=')
	{
	    tv_op(lp->ll_tv, rettv, op);
	    తిరిగి;
	}
	లేకపోతే
	    clear_tv(lp->ll_tv);

	/*
	 * Assign the value to the variable or list item.
	 */
	ఒకవేళ (copy)
	    copy_tv(rettv, lp->ll_tv);
	లేకపోతే
	{
	    *lp->ll_tv = *rettv;
	    lp->ll_tv->v_తాళం = 0;
	    init_tv(rettv);
	}
    }
}

/*
 * Handle "blob1 += blob2".
 * Returns OK or FAIL.
 */
    స్థిర పూర్ణాంకం
tv_op_blob(typval_T *tv1, typval_T *tv2, char_u *op)
{
    ఒకవేళ (*op != '+' || tv2->v_type != VAR_BLOB)
	తిరిగి FAIL;

    // Blob += Blob
    ఒకవేళ (tv2->vval.v_blob == ఖాళీ)
	తిరిగి OK;

    ఒకవేళ (tv1->vval.v_blob == ఖాళీ)
    {
	tv1->vval.v_blob = tv2->vval.v_blob;
	++tv1->vval.v_blob->bv_refcount;
	తిరిగి OK;
    }

    blob_T	*b1 = tv1->vval.v_blob;
    blob_T	*b2 = tv2->vval.v_blob;
    పూర్ణాంకం		len = blob_len(b2);

    ఒకవేళ (len > 0 && ga_grow(&b1->bv_ga, len) == OK)
    {
	mch_memmove((char_u *)b1->bv_ga.ga_దత్తాంశం + b1->bv_ga.ga_len,
		    (char_u *)b2->bv_ga.ga_దత్తాంశం, (size_t)len);
	b1->bv_ga.ga_len += len;
    }
    లేకపోతే
	కొరకు (పూర్ణాంకం i = 0; i < len; i++)
	    (శూన్య)ga_append(&b1->bv_ga, blob_get(b2, i));

    తిరిగి OK;
}

/*
 * Handle "list1 += list2".
 * Returns OK or FAIL.
 */
    స్థిర పూర్ణాంకం
tv_op_list(typval_T *tv1, typval_T *tv2, char_u *op)
{
    ఒకవేళ (*op != '+' || tv2->v_type != VAR_LIST)
	తిరిగి FAIL;

    // List += List
    ఒకవేళ (tv2->vval.v_list == ఖాళీ)
	తిరిగి OK;

    ఒకవేళ (tv1->vval.v_list == ఖాళీ)
    {
	tv1->vval.v_list = tv2->vval.v_list;
	++tv1->vval.v_list->lv_refcount;
    }
    లేకపోతే
	list_extend(tv1->vval.v_list, tv2->vval.v_list, ఖాళీ);

    తిరిగి OK;
}

/*
 * Handle number operations:
 *	nr += nr , nr -= nr , nr *=nr , nr /= nr , nr %= nr
 *
 * Returns OK or FAIL.
 */
    స్థిర పూర్ణాంకం
tv_op_సంఖ్య(typval_T *tv1, typval_T *tv2, char_u *op)
{
    varnumber_T	n;
    పూర్ణాంకం		failed = FALSE;

    n = tv_get_సంఖ్య(tv1);
    ఒకవేళ (tv2->v_type == VAR_FLOAT)
    {
	float_T f = n;

	ఒకవేళ (*op == '%')
	    తిరిగి FAIL;
	ఎంపిక (*op)
	{
	    స్థితి '+': f += tv2->vval.v_float; ఆపు;
	    స్థితి '-': f -= tv2->vval.v_float; ఆపు;
	    స్థితి '*': f *= tv2->vval.v_float; ఆపు;
	    స్థితి '/': f /= tv2->vval.v_float; ఆపు;
	}
	clear_tv(tv1);
	tv1->v_type = VAR_FLOAT;
	tv1->vval.v_float = f;
    }
    లేకపోతే
    {
	ఎంపిక (*op)
	{
	    స్థితి '+': n += tv_get_సంఖ్య(tv2); ఆపు;
	    స్థితి '-': n -= tv_get_సంఖ్య(tv2); ఆపు;
	    స్థితి '*': n *= tv_get_సంఖ్య(tv2); ఆపు;
	    స్థితి '/': n = అంకె_divide(n, tv_get_సంఖ్య(tv2), &failed); ఆపు;
	    స్థితి '%': n = అంకె_modulus(n, tv_get_సంఖ్య(tv2), &failed); ఆపు;
	}
	clear_tv(tv1);
	tv1->v_type = VAR_సంఖ్య;
	tv1->vval.v_సంఖ్య = n;
    }

    తిరిగి failed ? FAIL : OK;
}

/*
 * Append string "s2" to the string in "tv1".
 * Returns OK if "tv1" was grown in place, FAIL otherwise.
 */
    స్థిర పూర్ణాంకం
grow_string_tv(typval_T *tv1, char_u *s2)
{
    ఒకవేళ (tv1->v_type != VAR_STRING || tv1->vval.v_string == ఖాళీ)
	తిరిగి FAIL;

    size_t len1 = STRLEN(tv1->vval.v_string);
    size_t len2 = STRLEN(s2);
    char_u *p = vim_realloc(tv1->vval.v_string, len1 + len2 + 1);

    ఒకవేళ (p == ఖాళీ)
	తిరిగి FAIL;

    mch_memmove(p + len1, s2, len2 + 1);
    tv1->vval.v_string = p;
    తిరిగి OK;
}

/*
 * Handle "str1 .= str2"
 * Returns OK or FAIL.
 */
    స్థిర పూర్ణాంకం
tv_op_string(typval_T *tv1, typval_T *tv2, char_u *op వాడని)
{
    char_u	numbuf[NUMBUFLEN];
    char_u	*s;
    char_u	*s2;

    ఒకవేళ (tv2->v_type == VAR_FLOAT)
	తిరిగి FAIL;

    // str .= str
    s2 = tv_get_string_buf(tv2, numbuf);
    ఒకవేళ (grow_string_tv(tv1, s2) == OK)
	తిరిగి OK;

    s = tv_get_string(tv1);
    s = concat_str(s, s2);
    clear_tv(tv1);
    tv1->v_type = VAR_STRING;
    tv1->vval.v_string = s;

    తిరిగి OK;
}

/*
 * Handle "tv1 += tv2", "tv1 -= tv2", "tv1 *= tv2", "tv1 /= tv2", "tv1 %= tv2"
 * and "tv1 .= tv2"
 * Returns OK or FAIL.
 */
    స్థిర పూర్ణాంకం
tv_op_nr_or_string(typval_T *tv1, typval_T *tv2, char_u *op)
{
    ఒకవేళ (tv2->v_type == VAR_LIST)
	తిరిగి FAIL;

    ఒకవేళ (vim_strchr((char_u *)"+-*/%", *op) != ఖాళీ)
	తిరిగి tv_op_సంఖ్య(tv1, tv2, op);

    తిరిగి tv_op_string(tv1, tv2, op);
}

/*
 * Handle "f1 += f2", "f1 -= f2", "f1 *= f2", "f1 /= f2".
 * Returns OK or FAIL.
 */
    స్థిర పూర్ణాంకం
tv_op_float(typval_T *tv1, typval_T *tv2, char_u *op)
{
    float_T f;

    ఒకవేళ (*op == '%' || *op == '.'
	    || (tv2->v_type != VAR_FLOAT
		&& tv2->v_type != VAR_సంఖ్య
		&& tv2->v_type != VAR_STRING))
	తిరిగి FAIL;

    ఒకవేళ (tv2->v_type == VAR_FLOAT)
	f = tv2->vval.v_float;
    లేకపోతే
	f = tv_get_సంఖ్య(tv2);
    ఎంపిక (*op)
    {
	స్థితి '+': tv1->vval.v_float += f; ఆపు;
	స్థితి '-': tv1->vval.v_float -= f; ఆపు;
	స్థితి '*': tv1->vval.v_float *= f; ఆపు;
	స్థితి '/': tv1->vval.v_float /= f; ఆపు;
    }

    తిరిగి OK;
}

/*
 * Handle "tv1 += tv2", "tv1 -= tv2", "tv1 *= tv2", "tv1 /= tv2", "tv1 %= tv2"
 * and "tv1 .= tv2"
 * Returns OK or FAIL.
 */
    పూర్ణాంకం
tv_op(typval_T *tv1, typval_T *tv2, char_u *op)
{
    // Can't do anything with a Funcref or Dict on the right.
    // v:true and friends only work with "..=".
    ఒకవేళ (tv2->v_type == VAR_FUNC || tv2->v_type == VAR_DICT
		|| ((tv2->v_type == VAR_BOOL || tv2->v_type == VAR_SPECIAL)
							&& *op != '.'))
    {
	semsg(_(e_wrong_variable_type_for_str_equal), op);
	తిరిగి FAIL;
    }

    పూర్ణాంకం retval = FAIL;
    ఎంపిక (tv1->v_type)
    {
	స్థితి VAR_UNKNOWN:
	స్థితి VAR_ANY:
	స్థితి VAR_VOID:
	స్థితి VAR_DICT:
	స్థితి VAR_FUNC:
	స్థితి VAR_PARTIAL:
	స్థితి VAR_BOOL:
	స్థితి VAR_SPECIAL:
	స్థితి VAR_పని:
	స్థితి VAR_CHANNEL:
	స్థితి VAR_INSTR:
	స్థితి VAR_OBJECT:
	స్థితి VAR_CLASS:
	స్థితి VAR_TYPEALIAS:
	స్థితి VAR_TUPLE:
	    ఆపు;

	స్థితి VAR_BLOB:
	    retval = tv_op_blob(tv1, tv2, op);
	    ఆపు;

	స్థితి VAR_LIST:
	    retval = tv_op_list(tv1, tv2, op);
	    ఆపు;

	స్థితి VAR_సంఖ్య:
	స్థితి VAR_STRING:
	    retval = tv_op_nr_or_string(tv1, tv2, op);
	    ఆపు;

	స్థితి VAR_FLOAT:
	    retval = tv_op_float(tv1, tv2, op);
	    ఆపు;
    }

    ఒకవేళ (retval != OK)
	semsg(_(e_wrong_variable_type_for_str_equal), op);

    తిరిగి retval;
}

/*
 * Evaluate the expression used in a ":for var in expr" command.
 * "arg" points to "var".
 * Set "*errp" to TRUE for an error, FALSE otherwise;
 * Return a pointer that holds the info.  Null when there is an error.
 */
    శూన్య *
eval_for_పంక్తి(
    char_u	*arg,
    పూర్ణాంకం		*errp,
    exarg_T	*eap,
    evalarg_T	*evalarg)
{
    forinfo_T	*fi;
    char_u	*var_list_చివర;
    char_u	*సమాసం;
    typval_T	tv;
    list_T	*l;
    tuple_T	*tuple;
    పూర్ణాంకం		skip = !(evalarg->eval_flags & EVAL_EVALUATE);

    *errp = TRUE;	// default: there is an error

    fi = ALLOC_CLEAR_ONE(forinfo_T);
    ఒకవేళ (fi == ఖాళీ)
	తిరిగి ఖాళీ;

    var_list_చివర = skip_var_list(arg, TRUE, &fi->fi_varcount,
						     &fi->fi_semicolon, FALSE);
    ఒకవేళ (var_list_చివర == ఖాళీ)
	తిరిగి fi;

    సమాసం = skipwhite_and_linebreak(var_list_చివర, evalarg);
    ఒకవేళ (సమాసం[0] != 'i' || సమాసం[1] != 'n'
				  || !(సమాసం[2] == NUL || VIM_ISWHITE(సమాసం[2])))
    {
	ఒకవేళ (లోపల_vim9script() && *సమాసం == ':' && సమాసం != var_list_చివర)
	    semsg(_(e_no_white_space_allowed_before_colon_str), సమాసం);
	లేకపోతే
	    emsg(_(e_missing_లోపల_after_for));
	తిరిగి fi;
    }

    ఒకవేళ (skip)
	++emsg_skip;
    సమాసం = skipwhite_and_linebreak(సమాసం + 2, evalarg);
    ఒకవేళ (eval0(సమాసం, &tv, eap, evalarg) == OK)
    {
	*errp = FALSE;
	ఒకవేళ (!skip)
	{
	    ఒకవేళ (tv.v_type == VAR_LIST)
	    {
		l = tv.vval.v_list;
		ఒకవేళ (l == ఖాళీ)
		{
		    // a null list is like an empty list: do nothing
		    clear_tv(&tv);
		}
		లేకపోతే
		{
		    // Need a real list here.
		    CHECK_LIST_MATERIALIZE(l);

		    // No need to increment the refcount, it's already set for
		    // the list being used in "tv".
		    fi->fi_list = l;
		    list_కూడు_watch(l, &fi->fi_lw);
		    fi->fi_lw.lw_అంశం = l->lv_మొదటి;
		}
	    }
	    లేకపోతే ఒకవేళ (tv.v_type == VAR_TUPLE)
	    {
		tuple = tv.vval.v_tuple;
		ఒకవేళ (tuple == ఖాళీ)
		{
		    // a null tuple is like an empty tuple: do nothing
		    clear_tv(&tv);
		}
		లేకపోతే
		{
		    // No need to increment the refcount, it's already set for
		    // the tuple being used in "tv".
		    fi->fi_tuple = tuple;
		    fi->fi_tuple_idx = 0;
		}
	    }
	    లేకపోతే ఒకవేళ (tv.v_type == VAR_BLOB)
	    {
		fi->fi_bi = 0;
		ఒకవేళ (tv.vval.v_blob != ఖాళీ)
		{
		    // Make a copy, so that the iteration still works when the
		    // blob is changed.
		    fi->fi_blob = blob_copy(tv.vval.v_blob);
		}
		clear_tv(&tv);
	    }
	    లేకపోతే ఒకవేళ (tv.v_type == VAR_STRING)
	    {
		fi->fi_byte_idx = 0;
		fi->fi_string = tv.vval.v_string;
		tv.vval.v_string = ఖాళీ;
		ఒకవేళ (fi->fi_string == ఖాళీ)
		    fi->fi_string = vim_strsave((char_u *)"");
	    }
	    లేకపోతే
	    {
		emsg(_(e_string_list_tuple_or_blob_required));
		clear_tv(&tv);
	    }
	}
    }
    ఒకవేళ (skip)
	--emsg_skip;
    fi->fi_break_లెక్క = evalarg->eval_break_లెక్క;

    తిరిగి fi;
}

/*
 * Used when looping over a :for line, skip the "in expr" part.
 */
    శూన్య
skip_for_పంక్తులు(శూన్య *fi_void, evalarg_T *evalarg)
{
    forinfo_T	*fi = (forinfo_T *)fi_void;
    పూర్ణాంకం		i;

    కొరకు (i = 0; i < fi->fi_break_లెక్క; ++i)
	eval_తదుపరి_పంక్తి(ఖాళీ, evalarg);
}

/*
 * Use the first item in a ":for" list.  Advance to the next.
 * Assign the values to the variable (list).  "arg" points to the first one.
 * Return TRUE when a valid item was found, FALSE when at end of list or
 * something wrong.
 */
    పూర్ణాంకం
తదుపరి_for_అంశం(శూన్య *fi_void, char_u *arg)
{
    forinfo_T	*fi = (forinfo_T *)fi_void;
    పూర్ణాంకం		ఫలితం;
    పూర్ణాంకం		flag = కేటాయించు_FOR_LOOP | (లోపల_vim9script()
			 ? (కేటాయించు_FINAL
			     // first round: error if variable exists
			     | (fi->fi_bi == 0 ? 0 : కేటాయించు_DECL)
			     | కేటాయించు_NO_MEMBER_TYPE
			     | కేటాయించు_UPDATE_ఖండం_ID)
			 : 0);
    listitem_T	*అంశం;
    పూర్ణాంకం		skip_కేటాయించు = లోపల_vim9script() && arg[0] == '_'
						      && !eval_isnamec(arg[1]);

    ఒకవేళ (fi->fi_blob != ఖాళీ)
    {
	typval_T	tv;

	ఒకవేళ (fi->fi_bi >= blob_len(fi->fi_blob))
	    తిరిగి FALSE;
	tv.v_type = VAR_సంఖ్య;
	tv.v_తాళం = VAR_FIXED;
	tv.vval.v_సంఖ్య = blob_get(fi->fi_blob, fi->fi_bi);
	++fi->fi_bi;
	ఒకవేళ (skip_కేటాయించు)
	    తిరిగి TRUE;
	తిరిగి ex_let_vars(arg, &tv, TRUE, fi->fi_semicolon,
					    fi->fi_varcount, flag, ఖాళీ) == OK;
    }

    ఒకవేళ (fi->fi_string != ఖాళీ)
    {
	typval_T	tv;
	పూర్ణాంకం		len;

	len = mb_ptr2len(fi->fi_string + fi->fi_byte_idx);
	ఒకవేళ (len == 0)
	    తిరిగి FALSE;
	tv.v_type = VAR_STRING;
	tv.v_తాళం = VAR_FIXED;
	tv.vval.v_string = vim_strnsave(fi->fi_string + fi->fi_byte_idx, len);
	fi->fi_byte_idx += len;
	++fi->fi_bi;
	ఒకవేళ (skip_కేటాయించు)
	    ఫలితం = TRUE;
	లేకపోతే
	    ఫలితం = ex_let_vars(arg, &tv, TRUE, fi->fi_semicolon,
					    fi->fi_varcount, flag, ఖాళీ) == OK;
	vim_free(tv.vval.v_string);
	తిరిగి ఫలితం;
    }

    ఒకవేళ (fi->fi_tuple != ఖాళీ)
    {
	typval_T	tv;

	ఒకవేళ (fi->fi_tuple_idx >= TUPLE_LEN(fi->fi_tuple))
	    తిరిగి FALSE;

	copy_tv(TUPLE_అంశం(fi->fi_tuple, fi->fi_tuple_idx), &tv);
	++fi->fi_tuple_idx;
	++fi->fi_bi;
	ఒకవేళ (skip_కేటాయించు)
	    ఫలితం = TRUE;
	లేకపోతే
	    ఫలితం = ex_let_vars(arg, &tv, TRUE, fi->fi_semicolon,
					    fi->fi_varcount, flag, ఖాళీ) == OK;
	clear_tv(&tv);
	తిరిగి ఫలితం;
    }

    అంశం = fi->fi_lw.lw_అంశం;
    ఒకవేళ (అంశం == ఖాళీ)
	ఫలితం = FALSE;
    లేకపోతే
    {
	fi->fi_lw.lw_అంశం = అంశం->li_తదుపరి;
	++fi->fi_bi;
	ఒకవేళ (skip_కేటాయించు)
	    ఫలితం = TRUE;
	లేకపోతే
	    ఫలితం = (ex_let_vars(arg, &అంశం->li_tv, TRUE, fi->fi_semicolon,
					   fi->fi_varcount, flag, ఖాళీ) == OK);
    }
    తిరిగి ఫలితం;
}

/*
 * Free the structure used to store info used by ":for".
 */
    శూన్య
free_for_info(శూన్య *fi_void)
{
    forinfo_T    *fi = (forinfo_T *)fi_void;

    ఒకవేళ (fi == ఖాళీ)
	తిరిగి;
    ఒకవేళ (fi->fi_list != ఖాళీ)
    {
	list_rem_watch(fi->fi_list, &fi->fi_lw);
	list_unref(fi->fi_list);
    }
    లేకపోతే ఒకవేళ (fi->fi_blob != ఖాళీ)
	blob_unref(fi->fi_blob);
    లేకపోతే ఒకవేళ (fi->fi_tuple != ఖాళీ)
	tuple_unref(fi->fi_tuple);
    లేకపోతే
	vim_free(fi->fi_string);
    vim_free(fi);
}

    శూన్య
సమితి_context_for_వ్యక్తీకరణ(
    expand_T	*xp,
    char_u	*arg,
    cmdidx_T	cmdidx)
{
    పూర్ణాంకం		has_సమాసం = cmdidx != CMD_let && cmdidx != CMD_var;
    పూర్ణాంకం		c;
    char_u	*p;

    ఒకవేళ (cmdidx == CMD_let || cmdidx == CMD_var
				 || cmdidx == CMD_const || cmdidx == CMD_final)
    {
	xp->xp_context = EXPAND_USER_VARS;
	ఒకవేళ (vim_strpbrk(arg, (char_u *)"\"'+-*/%.=!?~|&$([<>,#") == ఖాళీ)
	{
	    // ":let var1 var2 ...": find last space.
	    కొరకు (p = arg + STRLEN(arg); p >= arg; )
	    {
		xp->xp_నమూనా = p;
		MB_PTR_వెనుక(arg, p);
		ఒకవేళ (VIM_ISWHITE(*p))
		    ఆపు;
	    }
	    తిరిగి;
	}
    }
    లేకపోతే
	xp->xp_context = cmdidx == CMD_call ? EXPAND_FUNCTIONS
							  : EXPAND_వ్యక్తీకరణ;
    అయితే ((xp->xp_నమూనా = vim_strpbrk(arg,
				  (char_u *)"\"'+-*/%.=!?~|&$([<>,#")) != ఖాళీ)
    {
	c = *xp->xp_నమూనా;
	ఒకవేళ (c == '&')
	{
	    c = xp->xp_నమూనా[1];
	    ఒకవేళ (c == '&')
	    {
		++xp->xp_నమూనా;
		xp->xp_context = has_సమాసం ? EXPAND_వ్యక్తీకరణ : EXPAND_NOTHING;
	    }
	    లేకపోతే ఒకవేళ (c != ' ')
	    {
		xp->xp_context = EXPAND_SETTINGS;
		ఒకవేళ ((c == 'l' || c == 'g') && xp->xp_నమూనా[2] == ':')
		    xp->xp_నమూనా += 2;

	    }
	}
	లేకపోతే ఒకవేళ (c == '$')
	{
	    // environment variable
	    xp->xp_context = EXPAND_ENV_VARS;
	}
	లేకపోతే ఒకవేళ (c == '=')
	{
	    has_సమాసం = TRUE;
	    xp->xp_context = EXPAND_వ్యక్తీకరణ;
	}
	లేకపోతే ఒకవేళ (c == '#'
		&& xp->xp_context == EXPAND_వ్యక్తీకరణ)
	{
	    // Autoload function/variable contains '#'.
	    ఆపు;
	}
	లేకపోతే ఒకవేళ ((c == '<' || c == '#')
		&& xp->xp_context == EXPAND_FUNCTIONS
		&& vim_strchr(xp->xp_నమూనా, '(') == ఖాళీ)
	{
	    // Function name can start with "<SNR>" and contain '#'.
	    ఆపు;
	}
	లేకపోతే ఒకవేళ (has_సమాసం)
	{
	    ఒకవేళ (c == '"')	    // string
	    {
		అయితే ((c = *++xp->xp_నమూనా) != NUL && c != '"')
		    ఒకవేళ (c == '\\' && xp->xp_నమూనా[1] != NUL)
			++xp->xp_నమూనా;
		xp->xp_context = EXPAND_NOTHING;
	    }
	    లేకపోతే ఒకవేళ (c == '\'')	    // literal string
	    {
		// Trick: '' is like stopping and starting a literal string.
		అయితే ((c = *++xp->xp_నమూనా) != NUL && c != '\'')
		    /* skip */ ;
		xp->xp_context = EXPAND_NOTHING;
	    }
	    లేకపోతే ఒకవేళ (c == '|')
	    {
		ఒకవేళ (xp->xp_నమూనా[1] == '|')
		{
		    ++xp->xp_నమూనా;
		    xp->xp_context = EXPAND_వ్యక్తీకరణ;
		}
		లేకపోతే
		    xp->xp_context = EXPAND_COMMANDS;
	    }
	    లేకపోతే
		xp->xp_context = EXPAND_వ్యక్తీకరణ;
	}
	లేకపోతే
	    // Doesn't look like something valid, expand as an expression
	    // anyway.
	    xp->xp_context = EXPAND_వ్యక్తీకరణ;
	arg = xp->xp_నమూనా;
	ఒకవేళ (*arg != NUL)
	    అయితే ((c = *++arg) != NUL && (c == ' ' || c == '\t'))
		/* skip */ ;
    }

    // ":exe one two" completes "two"
    ఒకవేళ ((cmdidx == CMD_execute
		|| cmdidx == CMD_echo
		|| cmdidx == CMD_echon
		|| cmdidx == CMD_echomsg
		|| cmdidx == CMD_echoerr
		|| cmdidx == CMD_echoconsole
		|| cmdidx == CMD_echowindow)
	    && xp->xp_context == EXPAND_వ్యక్తీకరణ)
    {
	కొరకు (;;)
	{
	    char_u *n = skiptowhite(arg);

	    ఒకవేళ (n == arg || IS_WHITE_OR_NUL(*skipwhite(n)))
		ఆపు;
	    arg = skipwhite(n);
	}
    }

    xp->xp_నమూనా = arg;
}

/*
 * Cache with the compiled program of the last pattern used by
 * pattern_match() and the match functions.  Script loops often evaluate the
 * same pattern many times; reusing the program avoids compiling it for
 * every evaluation.  The cache owns the program only between uses:
 * eval_regcomp() hands it to the caller and empties the cache, and
 * eval_regfree() adopts the program the caller ends up with.  Thus when
 * executing replaced the program (the automatic engine falling back to
 * backtracking frees the original) no freed program is left behind in the
 * cache.
 * Adopting the caller's program also means that after a fallback the
 * backtracking program stays cached until the pattern or an option in the
 * key changes; the automatic engine is not tried again for it.
 * The cache must only be used where the pattern is compiled with
 * RE_MAGIC + RE_STRING and 'cpoptions' made empty, as every caller here
 * does; both functions check these conditions and fall back to plain
 * compilation and freeing when they do not hold.
 * eval_prog_pat and eval_prog_enc are non-NULL whenever eval_prog_cache is
 * non-NULL.
 */
స్థిర regprog_T    *eval_prog_cache = ఖాళీ;
స్థిర char_u	    *eval_prog_pat = ఖాళీ;	// pattern it was compiled for
స్థిర char_u	    *eval_prog_enc = ఖాళీ;	// 'encoding' when compiled
స్థిర దీర్ఘ	    eval_prog_re;		// 'regexpengine' when compiled
// State sampled by eval_regcomp() for the eval_regfree() call that gives the
// program back: user code run in between, e.g. an object's string() method
// invoked by match() on a list, can change these options, and then the
// program must not be cached under the changed values.  eval_compile_enc is
// only ever compared against p_enc, never dereferenced.
స్థిర దీర్ఘ	    eval_compile_re;
స్థిర char_u	    *eval_compile_enc;
స్థిర పూర్ణాంకం	    eval_prog_busy = 0;	// programs eval_regcomp() handed out
					// that were not given back yet
స్థిర పూర్ణాంకం	    eval_prog_reentered = FALSE;  // eval_regcomp() ran while
						  // a program was handed out

/*
 * Return true when compiling "pat" depends on more state than the cache key
 * covers: "~" is replaced with the previous substitute string, bracket
 * classes like [:alpha:], [=a=] and [.a.] can depend on the locale, the
 * cursor-relative atoms \%.l, \%.c and \%.v compile the current cursor
 * position into the program, and a "\%#=" engine prefix makes vim_regcomp()
 * report E864 for a bad value on every compilation.  The check is
 * intentionally over-approximate, e.g. a literal "~" or "%." also matches;
 * such a pattern is compiled every time, which is never wrong.
 */
    స్థిర bool
eval_prog_volatile(char_u *pat)
{
    char_u *p;

    ఒకవేళ (STRNCMP(pat, "\\%#=", 4) == 0)
	తిరిగి true;
    కొరకు (p = pat; *p != NUL; ++p)
	ఒకవేళ (*p == '~'
		|| (p[0] == '[' && (p[1] == ':' || p[1] == '=' || p[1] == '.'))
		|| (p[0] == '%' && (p[1] == '.'
			|| ((p[1] == '<' || p[1] == '>') && p[2] == '.'))))
	    తిరిగి true;
    తిరిగి false;
}

/*
 * Compile pattern "pat" like vim_regcomp(pat, RE_MAGIC + RE_STRING) would,
 * but reuse the cached program when it was compiled for the same pattern.
 * Free the result with eval_regfree(), not with vim_regfree().
 */
    regprog_T *
eval_regcomp(char_u *pat)
{
    regprog_T	*prog;

    // Sample the option state the key covers, eval_regfree() checks that it
    // did not change while the caller was using the program.
    eval_compile_re = p_re;
    eval_compile_enc = p_enc;
    ఒకవేళ (eval_prog_busy > 0)
	// The samples above no longer describe the program already handed
	// out: its eval_regfree() must drop it instead of caching it.
	eval_prog_reentered = TRUE;

    ఒకవేళ (eval_prog_cache != ఖాళీ
	    && *p_cpo == NUL
	    && eval_prog_cache->re_flags == RE_MAGIC + RE_STRING
	    && eval_prog_re == p_re
	    && STRCMP(eval_prog_pat, pat) == 0
	    && STRCMP(eval_prog_enc, p_enc) == 0)
    {
	prog = eval_prog_cache;
	// The caller now owns the program, eval_regfree() adopts the
	// program the caller ends up with.
	eval_prog_cache = ఖాళీ;
    }
    లేకపోతే
	prog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
    ఒకవేళ (prog != ఖాళీ)
	++eval_prog_busy;	// paired with the eval_regfree() call
    తిరిగి prog;
}

/*
 * Free program "prog", obtained with eval_regcomp() for pattern "pat", by
 * keeping it in the cache for the next use.  "pat" must be the same string
 * the program was compiled for and must still be valid.
 */
    శూన్య
eval_regfree(char_u *pat, regprog_T *prog)
{
    పూర్ణాంకం	    reentered = eval_prog_reentered;

    ఒకవేళ (eval_prog_busy > 0 && --eval_prog_busy == 0)
	eval_prog_reentered = FALSE;
    ఒకవేళ (prog == ఖాళీ)
	తిరిగి;
    ఒకవేళ (reentered || *p_cpo != NUL
	    || eval_compile_re != p_re || eval_compile_enc != p_enc)
    {
	// 'cpoptions' is not empty, an option in the cache key changed, or
	// another program was handed out while the caller was using this
	// one: the samples may not describe this program, do not cache it.
	vim_regfree(prog);
	తిరిగి;
    }
    ఒకవేళ (eval_prog_pat == ఖాళీ || STRCMP(eval_prog_pat, pat) != 0
	    || STRCMP(eval_prog_enc, p_enc) != 0)
    {
	char_u	*pat_copy;
	char_u	*enc_copy;

	// A cached pattern is known not to be volatile, thus the scan is
	// only needed when installing a new key.
	ఒకవేళ (eval_prog_volatile(pat))
	{
	    // compiling depends on state the cache key does not cover
	    vim_regfree(prog);
	    తిరిగి;
	}
	pat_copy = vim_strsave(pat);
	enc_copy = vim_strsave(p_enc);
	ఒకవేళ (pat_copy == ఖాళీ || enc_copy == ఖాళీ)
	{
	    vim_free(pat_copy);
	    vim_free(enc_copy);
	    vim_regfree(prog);
	    తిరిగి;
	}
	vim_free(eval_prog_pat);
	vim_free(eval_prog_enc);
	eval_prog_pat = pat_copy;
	eval_prog_enc = enc_copy;
    }
    // On a cache miss eval_regcomp() left the previous program in the
    // cache, and a nested evaluation may have stored another one: keep the
    // most recent program.
    vim_regfree(eval_prog_cache);
    eval_prog_cache = prog;
    eval_prog_re = eval_compile_re;
}

#ఒకవేళ defined(EXITFREE)
    శూన్య
free_eval_regcomp_cache(శూన్య)
{
    vim_regfree(eval_prog_cache);
    eval_prog_cache = ఖాళీ;
    VIM_CLEAR(eval_prog_pat);
    VIM_CLEAR(eval_prog_enc);
    eval_prog_busy = 0;
    eval_prog_reentered = FALSE;
}
#అయితే_ముగింపు

/*
 * Return TRUE if "pat" matches "text".
 * Does not use 'cpo' and always uses 'magic'.
 */
    పూర్ణాంకం
నమూనా_సరిపోలిక(char_u *pat, char_u *పాఠ్యం, పూర్ణాంకం ic)
{
    పూర్ణాంకం		matches = FALSE;
    char_u	*save_cpo;
    regmatch_T	regmatch;

    // avoid 'l' flag in 'cpoptions'
    save_cpo = p_cpo;
    p_cpo = ఖాళీలు_option;
    regmatch.regprog = eval_regcomp(pat);
    ఒకవేళ (regmatch.regprog != ఖాళీ)
    {
	regmatch.rm_ic = ic;
	matches = vim_regexec_nl(&regmatch, పాఠ్యం, (colnr_T)0);
	eval_regfree(pat, regmatch.regprog);
    }
    p_cpo = save_cpo;
    తిరిగి matches;
}

/*
 * Handle a name followed by "(".  Both for just "name(arg)" and for
 * "expr->name(arg)".
 * Returns OK or FAIL.
 */
    స్థిర పూర్ణాంకం
eval_func(
	char_u	    **arg,	// points to "(", will be advanced
	evalarg_T   *evalarg,
	char_u	    *name,
	పూర్ణాంకం	    name_len,
	typval_T    *rettv,
	పూర్ణాంకం	    flags,
	typval_T    *basetv)	// "expr" for "expr->name(arg)"
{
    పూర్ణాంకం		evaluate = flags & EVAL_EVALUATE;
    char_u	*s = name;
    పూర్ణాంకం		len = name_len;
    partial_T	*partial;
    పూర్ణాంకం		ret = OK;
    type_T	*type = ఖాళీ;
    పూర్ణాంకం		దొరికింది_var = FALSE;

    ఒకవేళ (!evaluate)
	check_vars(s, len);

    // If "s" is the name of a variable of type VAR_FUNC
    // use its contents.
    s = deref_func_name(s, &len, &partial,
		 లోపల_vim9script() ? &type : ఖాళీ, !evaluate, FALSE, &దొరికింది_var);

    // Need to make a copy, in case evaluating the arguments makes
    // the name invalid.
    s = vim_strsave(s);
    ఒకవేళ (s == ఖాళీ || (evaluate && *s == NUL))
	ret = FAIL;
    లేకపోతే
    {
	funcexe_T funcexe;

	// Invoke the function.
	CLEAR_క్షేత్రం(funcexe);
	funcexe.fe_firstline = curwin->w_cursor.lnum;
	funcexe.fe_lastline = curwin->w_cursor.lnum;
	funcexe.fe_evaluate = evaluate;
	funcexe.fe_partial = partial;
	ఒకవేళ (partial != ఖాళీ)
	{
	    funcexe.fe_object = partial->pt_obj;
	    ఒకవేళ (funcexe.fe_object != ఖాళీ)
		++funcexe.fe_object->obj_refcount;
	}
	funcexe.fe_basetv = basetv;
	funcexe.fe_check_type = type;
	funcexe.fe_దొరికింది_var = దొరికింది_var;
	ఒకవేళ (evalarg != ఖాళీ)
	    funcexe.fe_cctx = evalarg->eval_cctx;
	ret = get_func_tv(s, len, rettv, arg, evalarg, &funcexe);
    }
    vim_free(s);

    // If evaluate is FALSE rettv->v_type was not set in
    // get_func_tv, but it's needed in handle_subscript() to parse
    // what follows. So set it here.
    ఒకవేళ (rettv->v_type == VAR_UNKNOWN && !evaluate && **arg == '(')
    {
	rettv->vval.v_string = ఖాళీ;
	rettv->v_type = VAR_FUNC;
    }

    // Stop the expression evaluation when immediately
    // aborting on error, or when an interrupt occurred or
    // an exception was thrown but not caught.
    ఒకవేళ (evaluate && aborting())
    {
	ఒకవేళ (ret == OK)
	    clear_tv(rettv);
	ret = FAIL;
    }
    తిరిగి ret;
}

/*
 * After a NL, skip over empty lines and comment-only lines.
 */
    స్థిర char_u *
newline_skip_comments(char_u *arg)
{
    char_u *p = arg + 1;

    కొరకు (;;)
    {
	p = skipwhite(p);

	ఒకవేళ (*p == NUL)
	    ఆపు;
	ఒకవేళ (vim9_comment_ప్రారంభం(p))
	{
	    char_u *nl = vim_strchr(p, NL);

	    ఒకవేళ (nl == ఖాళీ)
		ఆపు;
	    p = nl;
	}
	ఒకవేళ (*p != NL)
	    ఆపు;
	++p;  // skip another NL
    }
    తిరిగి p;
}

/*
 * Get the next line source line without advancing.  But do skip over comment
 * lines.
 * Only called for Vim9 script.
 */
    స్థిర char_u *
getline_peek_skip_comments(evalarg_T *evalarg)
{
    కొరకు (;;)
    {
	char_u *తదుపరి = getline_peek(evalarg->eval_getline,
							 evalarg->eval_cookie);
	char_u *p;

	ఒకవేళ (తదుపరి == ఖాళీ)
	    ఆపు;
	p = skipwhite(తదుపరి);
	ఒకవేళ (*p != NUL && !vim9_comment_ప్రారంభం(p))
	    తిరిగి తదుపరి;
	ఒకవేళ (eval_తదుపరి_పంక్తి(ఖాళీ, evalarg) == ఖాళీ)
	    ఆపు;
    }
    తిరిగి ఖాళీ;
}

/*
 * If inside Vim9 script, "arg" points to the end of a line (ignoring a #
 * comment) and there is a next line, return the next line (skipping blanks)
 * and set "getnext".
 * Otherwise return the next non-white at or after "arg" and set "getnext" to
 * FALSE.
 * "arg" must point somewhere inside a line, not at the start.
 */
    char_u *
eval_తదుపరి_non_blank(char_u *arg, evalarg_T *evalarg, పూర్ణాంకం *getnext)
{
    char_u *p = skipwhite(arg);

    *getnext = FALSE;
    ఒకవేళ (లోపల_vim9script()
	    && evalarg != ఖాళీ
	    && (evalarg->eval_cookie != ఖాళీ || evalarg->eval_cctx != ఖాళీ
								   || *p == NL)
	    && (*p == NUL || *p == NL
			     || (vim9_comment_ప్రారంభం(p) && VIM_ISWHITE(p[-1]))))
    {
	char_u *తదుపరి;

	ఒకవేళ (*p == NL)
	    తదుపరి = newline_skip_comments(p);
	లేకపోతే ఒకవేళ (evalarg->eval_cookie != ఖాళీ)
	    తదుపరి = getline_peek_skip_comments(evalarg);
	లేకపోతే
	    తదుపరి = peek_తదుపరి_పంక్తి_from_context(evalarg->eval_cctx);

	ఒకవేళ (తదుపరి != ఖాళీ)
	{
	    *getnext = *p != NL;
	    తిరిగి skipwhite(తదుపరి);
	}
    }
    తిరిగి p;
}

/*
 * To be called after eval_next_non_blank() sets "getnext" to TRUE.
 * Only called for Vim9 script.
 *
 * If "arg" is not NULL, then the caller should assign the return value to
 * "arg".
 */
    char_u *
eval_తదుపరి_పంక్తి(char_u *arg, evalarg_T *evalarg)
{
    garray_T	*gap = &evalarg->eval_ga;
    char_u	*పంక్తి;

    ఒకవేళ (arg != ఖాళీ)
    {
	ఒకవేళ (*arg == NL)
	    తిరిగి newline_skip_comments(arg);
	// Truncate before a trailing comment, so that concatenating the lines
	// won't turn the rest into a comment.
	ఒకవేళ (*skipwhite(arg) == '#')
	    *arg = NUL;
    }

    ఒకవేళ (evalarg->eval_cookie != ఖాళీ)
	పంక్తి = evalarg->eval_getline(0, evalarg->eval_cookie, 0,
							   GETLINE_CONCAT_ALL);
    లేకపోతే
	పంక్తి = తదుపరి_పంక్తి_from_context(evalarg->eval_cctx, TRUE);
    ఒకవేళ (పంక్తి == ఖాళీ)
	తిరిగి ఖాళీ;

    ++evalarg->eval_break_లెక్క;
    ఒకవేళ (gap->ga_itemsize > 0 && ga_grow(gap, 1) == OK)
    {
	char_u *p = skipwhite(పంక్తి);

	// Going to concatenate the lines after parsing.  For an empty or
	// comment line use an empty string.
	ఒకవేళ (*p == NUL || vim9_comment_ప్రారంభం(p))
	{
	    vim_free(పంక్తి);
	    పంక్తి = vim_strsave((char_u *)"");
	}

	((char_u **)gap->ga_దత్తాంశం)[gap->ga_len] = పంక్తి;
	++gap->ga_len;
    }
    లేకపోతే ఒకవేళ (evalarg->eval_cookie != ఖాళీ)
    {
	free_eval_tofree_later(evalarg);
	evalarg->eval_tofree = పంక్తి;
    }

    // Advanced to the next line, "arg" no longer points into the previous
    // line.  The caller assigns the return value to "arg".
    // If "arg" is NULL, then the return value is discarded.  In that case,
    // "arg" still points to the previous line.  So don't reset
    // "eval_using_cmdline".
    ఒకవేళ (arg != ఖాళీ)
	evalarg->eval_using_cmdline = FALSE;
    తిరిగి skipwhite(పంక్తి);
}

/*
 * Call eval_next_non_blank() and get the next line if needed.
 */
    char_u *
skipwhite_and_linebreak(char_u *arg, evalarg_T *evalarg)
{
    పూర్ణాంకం	    getnext;
    char_u  *p = skipwhite_and_nl(arg);

    ఒకవేళ (evalarg == ఖాళీ)
	తిరిగి skipwhite(arg);
    eval_తదుపరి_non_blank(p, evalarg, &getnext);
    ఒకవేళ (getnext)
	తిరిగి eval_తదుపరి_పంక్తి(arg, evalarg);
    తిరిగి p;
}

/*
 * The "eval" functions have an "evalarg" argument: When NULL or
 * "evalarg->eval_flags" does not have EVAL_EVALUATE, then the argument is only
 * parsed but not executed.  The functions may return OK, but the rettv will be
 * of type VAR_UNKNOWN.  The functions still returns FAIL for a syntax error.
 */

/*
 * Handle zero level expression.
 * This calls eval1() and handles error message and nextcmd.
 * Put the result in "rettv" when returning OK and "evaluate" is TRUE.
 * "evalarg" can be NULL, EVALARG_EVALUATE or a pointer.
 * Return OK or FAIL.
 */
    పూర్ణాంకం
eval0(
    char_u	*arg,
    typval_T	*rettv,
    exarg_T	*eap,
    evalarg_T	*evalarg)
{
    తిరిగి eval0_retarg(arg, rettv, eap, evalarg, ఖాళీ);
}

/*
 * If "arg" is a simple function call without arguments then call it and return
 * the result.  Otherwise return NOTDONE.
 */
    పూర్ణాంకం
may_call_simple_func(
    char_u	*arg,
    typval_T	*rettv)
{
    char_u  *parens = (char_u *)అక్షరశ్రేణి_శోధన((అక్షరం *)arg, "()");
    పూర్ణాంకం	    r = NOTDONE;

    // If the expression is "FuncName()" then we can skip a lot of overhead.
    ఒకవేళ (parens != ఖాళీ && *skipwhite(parens + 2) == NUL)
    {
	char_u *p = STRNCMP(arg, "<SNR>", 5) == 0 ? skipdigits(arg + 5) : arg;

	ఒకవేళ (గమ్యం_name_చివర(p, TRUE) == parens)
	    r = call_simple_func(arg, (size_t)(parens - arg), rettv);
    }
    తిరిగి r;
}

/*
 * Handle zero level expression with optimization for a simple function call.
 * Same arguments and return value as eval0().
 */
    స్థిర పూర్ణాంకం
eval0_simple_funccal(
    char_u	*arg,
    typval_T	*rettv,
    exarg_T	*eap,
    evalarg_T	*evalarg)
{
    పూర్ణాంకం	    r = may_call_simple_func(arg, rettv);

    ఒకవేళ (r == NOTDONE)
	r = eval0_retarg(arg, rettv, eap, evalarg, ఖాళీ);
    తిరిగి r;
}

/*
 * Like eval0() but when "retarg" is not NULL store the pointer to after the
 * expression and don't check what comes after the expression.
 */
    పూర్ణాంకం
eval0_retarg(
    char_u	*arg,
    typval_T	*rettv,
    exarg_T	*eap,
    evalarg_T	*evalarg,
    char_u	**retarg)
{
    పూర్ణాంకం		ret;
    char_u	*p;
    char_u	*సమాసం_చివర;
    పూర్ణాంకం		did_emsg_before = did_emsg;
    పూర్ణాంకం		called_emsg_before = called_emsg;
    పూర్ణాంకం		check_for_చివర = retarg == ఖాళీ;
    పూర్ణాంకం		చివర_error = FALSE;

    p = skipwhite(arg);
    ret = eval1(&p, rettv, evalarg);

    ఒకవేళ (ret != FAIL)
    {
	సమాసం_చివర = p;
	p = skipwhite(p);

	// In Vim9 script a command block is not split at NL characters for
	// commands using an expression argument.  Skip over a '#' comment to
	// check for a following NL.  Require white space before the '#'.
	ఒకవేళ (లోపల_vim9script() && p > సమాసం_చివర && retarg == ఖాళీ)
	    అయితే (*p == '#')
	    {
		char_u *nl = vim_strchr(p, NL);

		ఒకవేళ (nl == ఖాళీ)
		    ఆపు;
		p = skipwhite(nl + 1);
		ఒకవేళ (eap != ఖాళీ && *p != NUL)
		    eap->nextcmd = p;
		check_for_చివర = FALSE;
	    }

	ఒకవేళ (check_for_చివర)
	    చివర_error = !చివర్లు_excmd2(arg, p);
    }

    ఒకవేళ (ret == FAIL || చివర_error)
    {
	ఒకవేళ (ret != FAIL)
	    clear_tv(rettv);
	/*
	 * Report the invalid expression unless the expression evaluation has
	 * been cancelled due to an aborting error, an interrupt, or an
	 * exception, or we already gave a more specific error.
	 * Also check called_emsg for when using assert_fails().
	 */
	ఒకవేళ (!aborting()
		&& did_emsg == did_emsg_before
		&& called_emsg == called_emsg_before
		&& (!లోపల_vim9script() || !vim9_bad_comment(p)))
	{
	    ఒకవేళ (చివర_error)
		semsg(_(e_trailing_characters_str), p);
	    లేకపోతే
		semsg(_(e_invalid_వ్యక్తీకరణ_str), arg);
	}

	ఒకవేళ (eap != ఖాళీ && p != ఖాళీ)
	{
	    // Some of the expression may not have been consumed.
	    // Only execute a next command if it cannot be a "||" operator.
	    // The next command may be "catch".
	    char_u *nextcmd = check_nextcmd(p);
	    ఒకవేళ (nextcmd != ఖాళీ && *nextcmd != '|')
		eap->nextcmd = nextcmd;
	}
	తిరిగి FAIL;
    }

    ఒకవేళ (retarg != ఖాళీ)
	*retarg = p;
    లేకపోతే ఒకవేళ (check_for_చివర && eap != ఖాళీ)
	సమితి_nextcmd(eap, p);

    తిరిగి ret;
}

/*
 * Handle top level expression:
 *	expr2 ? expr1 : expr1
 *	expr2 ?? expr1
 *
 * "arg" must point to the first non-white of the expression.
 * "arg" is advanced to just after the recognized expression.
 *
 * Return OK or FAIL.
 */
    పూర్ణాంకం
eval1(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    char_u  *p;
    పూర్ణాంకం	    getnext;

    CLEAR_సూచిక(rettv);

    /*
     * Get the first variable.
     */
    ఒకవేళ (eval2(arg, rettv, evalarg) == FAIL)
	తిరిగి FAIL;

    p = eval_తదుపరి_non_blank(*arg, evalarg, &getnext);
    ఒకవేళ (*p == '?')
    {
	పూర్ణాంకం		op_falsy = p[1] == '?';
	పూర్ణాంకం		ఫలితం;
	typval_T	var2;
	evalarg_T	*evalarg_వాడిన = evalarg;
	evalarg_T	local_evalarg;
	పూర్ణాంకం		orig_flags;
	పూర్ణాంకం		evaluate;
	పూర్ణాంకం		vim9script = లోపల_vim9script();

	ఒకవేళ (evalarg == ఖాళీ)
	{
	    init_evalarg(&local_evalarg);
	    evalarg_వాడిన = &local_evalarg;
	}
	orig_flags = evalarg_వాడిన->eval_flags;
	evaluate = evalarg_వాడిన->eval_flags & EVAL_EVALUATE;

	ఒకవేళ (getnext)
	    *arg = eval_తదుపరి_పంక్తి(*arg, evalarg_వాడిన);
	లేకపోతే
	{
	    ఒకవేళ (evaluate && vim9script && !VIM_ISWHITE(p[-1]))
	    {
		error_white_both(p, op_falsy ? 2 : 1);
		clear_tv(rettv);
		తిరిగి FAIL;
	    }
	    *arg = p;
	}

	ఫలితం = FALSE;
	ఒకవేళ (evaluate)
	{
	    పూర్ణాంకం		error = FALSE;

	    ఒకవేళ (op_falsy)
	    {
		// Is this typeval supported with the falsy operator?
		ఒకవేళ (check_typval_is_విలువ(rettv) == FAIL)
		{
		    clear_tv(rettv);
		    తిరిగి FAIL;
		}
		ఫలితం = tv2bool(rettv);
	    }
	    లేకపోతే ఒకవేళ (vim9script)
		ఫలితం = tv_get_bool_chk(rettv, &error);
	    లేకపోతే ఒకవేళ (tv_get_సంఖ్య_chk(rettv, &error) != 0)
		ఫలితం = TRUE;
	    ఒకవేళ (error || !op_falsy || !ఫలితం)
		clear_tv(rettv);
	    ఒకవేళ (error)
		తిరిగి FAIL;
	}

	/*
	 * Get the second variable.  Recursive!
	 */
	ఒకవేళ (op_falsy)
	    ++*arg;
	ఒకవేళ (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[1]))
	{
	    error_white_both(*arg - (op_falsy ? 1 : 0), op_falsy ? 2 : 1);
	    clear_tv(rettv);
	    తిరిగి FAIL;
	}
	*arg = skipwhite_and_linebreak(*arg + 1, evalarg_వాడిన);
	evalarg_వాడిన->eval_flags = (op_falsy ? !ఫలితం : ఫలితం)
				  ? orig_flags : (orig_flags & ~EVAL_EVALUATE);
	ఒకవేళ (eval1(arg, &var2, evalarg_వాడిన) == FAIL)
	{
	    evalarg_వాడిన->eval_flags = orig_flags;
	    తిరిగి FAIL;
	}
	ఒకవేళ (!op_falsy || !ఫలితం)
	    *rettv = var2;

	ఒకవేళ (!op_falsy)
	{
	    /*
	     * Check for the ":".
	     */
	    p = eval_తదుపరి_non_blank(*arg, evalarg_వాడిన, &getnext);
	    ఒకవేళ (*p != ':')
	    {
		emsg(_(e_missing_colon_after_questionmark));
		ఒకవేళ (evaluate && ఫలితం)
		    clear_tv(rettv);
		evalarg_వాడిన->eval_flags = orig_flags;
		తిరిగి FAIL;
	    }
	    ఒకవేళ (getnext)
		*arg = eval_తదుపరి_పంక్తి(*arg, evalarg_వాడిన);
	    లేకపోతే
	    {
		ఒకవేళ (evaluate && vim9script && !VIM_ISWHITE(p[-1]))
		{
		    error_white_both(p, 1);
		    clear_tv(rettv);
		    evalarg_వాడిన->eval_flags = orig_flags;
		    తిరిగి FAIL;
		}
		*arg = p;
	    }

	    /*
	     * Get the third variable.  Recursive!
	     */
	    ఒకవేళ (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[1]))
	    {
		error_white_both(*arg, 1);
		clear_tv(rettv);
		evalarg_వాడిన->eval_flags = orig_flags;
		తిరిగి FAIL;
	    }
	    *arg = skipwhite_and_linebreak(*arg + 1, evalarg_వాడిన);
	    evalarg_వాడిన->eval_flags = !ఫలితం ? orig_flags
					       : (orig_flags & ~EVAL_EVALUATE);
	    ఒకవేళ (eval1(arg, &var2, evalarg_వాడిన) == FAIL)
	    {
		ఒకవేళ (evaluate && ఫలితం)
		    clear_tv(rettv);
		evalarg_వాడిన->eval_flags = orig_flags;
		తిరిగి FAIL;
	    }
	    ఒకవేళ (evaluate && !ఫలితం)
		*rettv = var2;
	}

	ఒకవేళ (evalarg == ఖాళీ)
	    clear_evalarg(&local_evalarg, ఖాళీ);
	లేకపోతే
	    evalarg->eval_flags = orig_flags;
    }

    తిరిగి OK;
}

/*
 * Handle first level expression:
 *	expr2 || expr2 || expr2	    logical OR
 *
 * "arg" must point to the first non-white of the expression.
 * "arg" is advanced to just after the recognized expression.
 *
 * Return OK or FAIL.
 */
    స్థిర పూర్ణాంకం
eval2(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    char_u	*p;
    పూర్ణాంకం		getnext;

    /*
     * Get the first expression.
     */
    ఒకవేళ (eval3(arg, rettv, evalarg) == FAIL)
	తిరిగి FAIL;

    /*
     * Handle the  "||" operator.
     */
    p = eval_తదుపరి_non_blank(*arg, evalarg, &getnext);
    ఒకవేళ (p[0] == '|' && p[1] == '|')
    {
	evalarg_T   *evalarg_వాడిన = evalarg;
	evalarg_T   local_evalarg;
	పూర్ణాంకం	    evaluate;
	పూర్ణాంకం	    orig_flags;
	దీర్ఘ	    ఫలితం = FALSE;
	typval_T    var2;
	పూర్ణాంకం	    error = FALSE;
	పూర్ణాంకం	    vim9script = లోపల_vim9script();

	ఒకవేళ (evalarg == ఖాళీ)
	{
	    init_evalarg(&local_evalarg);
	    evalarg_వాడిన = &local_evalarg;
	}
	orig_flags = evalarg_వాడిన->eval_flags;
	evaluate = orig_flags & EVAL_EVALUATE;
	ఒకవేళ (evaluate)
	{
	    ఒకవేళ (vim9script)
		ఫలితం = tv_get_bool_chk(rettv, &error);
	    లేకపోతే ఒకవేళ (tv_get_సంఖ్య_chk(rettv, &error) != 0)
		ఫలితం = TRUE;
	    clear_tv(rettv);
	    ఒకవేళ (error)
		తిరిగి FAIL;
	}

	/*
	 * Repeat until there is no following "||".
	 */
	అయితే (p[0] == '|' && p[1] == '|')
	{
	    ఒకవేళ (getnext)
		*arg = eval_తదుపరి_పంక్తి(*arg, evalarg_వాడిన);
	    లేకపోతే
	    {
		ఒకవేళ (evaluate && vim9script && !VIM_ISWHITE(p[-1]))
		{
		    error_white_both(p, 2);
		    clear_tv(rettv);
		    తిరిగి FAIL;
		}
		*arg = p;
	    }

	    /*
	     * Get the second variable.
	     */
	    ఒకవేళ (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[2]))
	    {
		error_white_both(*arg, 2);
		clear_tv(rettv);
		తిరిగి FAIL;
	    }
	    *arg = skipwhite_and_linebreak(*arg + 2, evalarg_వాడిన);
	    evalarg_వాడిన->eval_flags = !ఫలితం ? orig_flags
					       : (orig_flags & ~EVAL_EVALUATE);
	    ఒకవేళ (eval3(arg, &var2, evalarg_వాడిన) == FAIL)
		తిరిగి FAIL;

	    /*
	     * Compute the result.
	     */
	    ఒకవేళ (evaluate && !ఫలితం)
	    {
		ఒకవేళ (vim9script)
		    ఫలితం = tv_get_bool_chk(&var2, &error);
		లేకపోతే ఒకవేళ (tv_get_సంఖ్య_chk(&var2, &error) != 0)
		    ఫలితం = TRUE;
		clear_tv(&var2);
		ఒకవేళ (error)
		    తిరిగి FAIL;
	    }
	    ఒకవేళ (evaluate)
	    {
		ఒకవేళ (vim9script)
		{
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_సంఖ్య = ఫలితం ? VVAL_TRUE : VVAL_FALSE;
		}
		లేకపోతే
		{
		    rettv->v_type = VAR_సంఖ్య;
		    rettv->vval.v_సంఖ్య = ఫలితం;
		}
	    }

	    p = eval_తదుపరి_non_blank(*arg, evalarg_వాడిన, &getnext);
	}

	ఒకవేళ (evalarg == ఖాళీ)
	    clear_evalarg(&local_evalarg, ఖాళీ);
	లేకపోతే
	    evalarg->eval_flags = orig_flags;
    }

    తిరిగి OK;
}

/*
 * Handle second level expression:
 *	expr3 && expr3 && expr3	    logical AND
 *
 * "arg" must point to the first non-white of the expression.
 * "arg" is advanced to just after the recognized expression.
 *
 * Return OK or FAIL.
 */
    స్థిర పూర్ణాంకం
eval3(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    char_u	*p;
    పూర్ణాంకం		getnext;

    /*
     * Get the first expression.
     */
    ఒకవేళ (eval4(arg, rettv, evalarg) == FAIL)
	తిరిగి FAIL;

    /*
     * Handle the "&&" operator.
     */
    p = eval_తదుపరి_non_blank(*arg, evalarg, &getnext);
    ఒకవేళ (p[0] == '&' && p[1] == '&')
    {
	evalarg_T   *evalarg_వాడిన = evalarg;
	evalarg_T   local_evalarg;
	పూర్ణాంకం	    orig_flags;
	పూర్ణాంకం	    evaluate;
	దీర్ఘ	    ఫలితం = TRUE;
	typval_T    var2;
	పూర్ణాంకం	    error = FALSE;
	పూర్ణాంకం	    vim9script = లోపల_vim9script();

	ఒకవేళ (evalarg == ఖాళీ)
	{
	    init_evalarg(&local_evalarg);
	    evalarg_వాడిన = &local_evalarg;
	}
	orig_flags = evalarg_వాడిన->eval_flags;
	evaluate = orig_flags & EVAL_EVALUATE;
	ఒకవేళ (evaluate)
	{
	    ఒకవేళ (vim9script)
		ఫలితం = tv_get_bool_chk(rettv, &error);
	    లేకపోతే ఒకవేళ (tv_get_సంఖ్య_chk(rettv, &error) == 0)
		ఫలితం = FALSE;
	    clear_tv(rettv);
	    ఒకవేళ (error)
		తిరిగి FAIL;
	}

	/*
	 * Repeat until there is no following "&&".
	 */
	అయితే (p[0] == '&' && p[1] == '&')
	{
	    ఒకవేళ (getnext)
		*arg = eval_తదుపరి_పంక్తి(*arg, evalarg_వాడిన);
	    లేకపోతే
	    {
		ఒకవేళ (evaluate && vim9script && !VIM_ISWHITE(p[-1]))
		{
		    error_white_both(p, 2);
		    clear_tv(rettv);
		    తిరిగి FAIL;
		}
		*arg = p;
	    }

	    /*
	     * Get the second variable.
	     */
	    ఒకవేళ (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[2]))
	    {
		error_white_both(*arg, 2);
		clear_tv(rettv);
		తిరిగి FAIL;
	    }
	    *arg = skipwhite_and_linebreak(*arg + 2, evalarg_వాడిన);
	    evalarg_వాడిన->eval_flags = ఫలితం ? orig_flags
					      : (orig_flags & ~EVAL_EVALUATE);
	    CLEAR_క్షేత్రం(var2);
	    ఒకవేళ (eval4(arg, &var2, evalarg_వాడిన) == FAIL)
		తిరిగి FAIL;

	    /*
	     * Compute the result.
	     */
	    ఒకవేళ (evaluate && ఫలితం)
	    {
		ఒకవేళ (vim9script)
		    ఫలితం = tv_get_bool_chk(&var2, &error);
		లేకపోతే ఒకవేళ (tv_get_సంఖ్య_chk(&var2, &error) == 0)
		    ఫలితం = FALSE;
		clear_tv(&var2);
		ఒకవేళ (error)
		    తిరిగి FAIL;
	    }
	    ఒకవేళ (evaluate)
	    {
		ఒకవేళ (vim9script)
		{
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_సంఖ్య = ఫలితం ? VVAL_TRUE : VVAL_FALSE;
		}
		లేకపోతే
		{
		    rettv->v_type = VAR_సంఖ్య;
		    rettv->vval.v_సంఖ్య = ఫలితం;
		}
	    }

	    p = eval_తదుపరి_non_blank(*arg, evalarg_వాడిన, &getnext);
	}

	ఒకవేళ (evalarg == ఖాళీ)
	    clear_evalarg(&local_evalarg, ఖాళీ);
	లేకపోతే
	    evalarg->eval_flags = orig_flags;
    }

    తిరిగి OK;
}

/*
 * Handle third level expression:
 *	var1 == var2
 *	var1 =~ var2
 *	var1 != var2
 *	var1 !~ var2
 *	var1 > var2
 *	var1 >= var2
 *	var1 < var2
 *	var1 <= var2
 *	var1 is var2
 *	var1 isnot var2
 *
 * "arg" must point to the first non-white of the expression.
 * "arg" is advanced to just after the recognized expression.
 *
 * Return OK or FAIL.
 */
    స్థిర పూర్ణాంకం
eval4(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    char_u	*p;
    పూర్ణాంకం		getnext;
    exprtype_T	type = సమాసం_UNKNOWN;
    పూర్ణాంకం		len = 2;
    పూర్ణాంకం		type_is = FALSE;

    /*
     * Get the first expression.
     */
    ఒకవేళ (eval5(arg, rettv, evalarg) == FAIL)
	తిరిగి FAIL;

    p = eval_తదుపరి_non_blank(*arg, evalarg, &getnext);

    type = get_compare_type(p, &len, &type_is);

    /*
     * If there is a comparative operator, use it.
     */
    ఒకవేళ (type != సమాసం_UNKNOWN)
    {
	typval_T    var2;
	పూర్ణాంకం	    ic;
	పూర్ణాంకం	    vim9script = లోపల_vim9script();
	పూర్ణాంకం	    evaluate = evalarg == ఖాళీ
				   ? 0 : (evalarg->eval_flags & EVAL_EVALUATE);
	దీర్ఘ	    comp_lnum = SOURCING_LNUM;

	ఒకవేళ (getnext)
	{
	    *arg = eval_తదుపరి_పంక్తి(*arg, evalarg);
	    p = *arg;
	}
	లేకపోతే ఒకవేళ (evaluate && vim9script && !VIM_ISWHITE(**arg))
	{
	    error_white_both(*arg, len);
	    clear_tv(rettv);
	    తిరిగి FAIL;
	}

	ఒకవేళ (vim9script && type_is && (p[len] == '?' || p[len] == '#'))
	{
	    semsg(_(e_invalid_వ్యక్తీకరణ_str), p);
	    clear_tv(rettv);
	    తిరిగి FAIL;
	}

	// extra question mark appended: ignore case
	ఒకవేళ (p[len] == '?')
	{
	    ic = TRUE;
	    ++len;
	}
	// extra '#' appended: match case
	లేకపోతే ఒకవేళ (p[len] == '#')
	{
	    ic = FALSE;
	    ++len;
	}
	// nothing appended: use 'ignorecase' if not in Vim script
	లేకపోతే
	    ic = vim9script ? FALSE : p_ic;

	/*
	 * Get the second variable.
	 */
	ఒకవేళ (evaluate && vim9script && !IS_WHITE_OR_NUL(p[len]))
	{
	    error_white_both(p, len);
	    clear_tv(rettv);
	    తిరిగి FAIL;
	}
	*arg = skipwhite_and_linebreak(p + len, evalarg);
	ఒకవేళ (eval5(arg, &var2, evalarg) == FAIL)
	{
	    clear_tv(rettv);
	    తిరిగి FAIL;
	}
	ఒకవేళ (evaluate)
	{
	    పూర్ణాంకం ret;

	    // use the line of the comparison for messages
	    SOURCING_LNUM = comp_lnum;
	    ఒకవేళ (vim9script && check_compare_types(type, rettv, &var2) == FAIL)
	    {
		ret = FAIL;
		clear_tv(rettv);
	    }
	    లేకపోతే
		ret = typval_compare(rettv, &var2, type, ic);
	    clear_tv(&var2);
	    తిరిగి ret;
	}
    }

    తిరిగి OK;
}

/*
 * Make a copy of blob "tv1" and append blob "tv2".
 */
    శూన్య
eval_addblob(typval_T *tv1, typval_T *tv2)
{
    blob_T  *b1 = tv1->vval.v_blob;
    blob_T  *b2 = tv2->vval.v_blob;
    blob_T  *b = blob_alloc();
    దీర్ఘ    len1, len2;
    దీర్ఘ    totallen;

    ఒకవేళ (b == ఖాళీ)
	తిరిగి;

    len1 = blob_len(b1);
    len2 = blob_len(b2);
    totallen = len1 + len2;

    ఒకవేళ (totallen > 0 && totallen <= గరిష్ఠ_పూర్ణాంకం
				    && ga_grow(&b->bv_ga, (పూర్ణాంకం)totallen) == OK)
    {
	ఒకవేళ (len1 > 0)
	    mch_memmove((char_u *)b->bv_ga.ga_దత్తాంశం,
		    (char_u *)b1->bv_ga.ga_దత్తాంశం, (size_t)len1);
	ఒకవేళ (len2 > 0)
	    mch_memmove((char_u *)b->bv_ga.ga_దత్తాంశం + len1,
		    (char_u *)b2->bv_ga.ga_దత్తాంశం, (size_t)len2);
	b->bv_ga.ga_len = (పూర్ణాంకం)totallen;
    }

    clear_tv(tv1);
    rettv_blob_సమితి(tv1, b);
}

/*
 * Make a copy of list "tv1" and append list "tv2".
 */
    పూర్ణాంకం
eval_addlist(typval_T *tv1, typval_T *tv2)
{
    typval_T var3;

    // concatenate Lists
    ఒకవేళ (list_concat(tv1->vval.v_list, tv2->vval.v_list, &var3) == FAIL)
    {
	clear_tv(tv1);
	clear_tv(tv2);
	తిరిగి FAIL;
    }
    clear_tv(tv1);
    *tv1 = var3;
    తిరిగి OK;
}

/*
 * Make a copy of tuple "tv1" and append tuple "tv2".
 */
    పూర్ణాంకం
eval_addtuple(typval_T *tv1, typval_T *tv2)
{
    పూర్ణాంకం		vim9script = లోపల_vim9script();
    typval_T	var3;

    ఒకవేళ (vim9script && tv1->vval.v_tuple != ఖాళీ && tv2->vval.v_tuple != ఖాళీ
	    && tv1->vval.v_tuple->tv_type != ఖాళీ
	    && tv2->vval.v_tuple->tv_type != ఖాళీ)
    {
	ఒకవేళ (!check_tuples_addable(tv1->vval.v_tuple->tv_type,
						tv2->vval.v_tuple->tv_type))
	    తిరిగి FAIL;
    }

    // concatenate tuples
    ఒకవేళ (tuple_concat(tv1->vval.v_tuple, tv2->vval.v_tuple, &var3) == FAIL)
    {
	clear_tv(tv1);
	clear_tv(tv2);
	తిరిగి FAIL;
    }
    clear_tv(tv1);
    *tv1 = var3;
    తిరిగి OK;
}

/*
 * Left or right shift the number "tv1" by the number "tv2" and store the
 * result in "tv1".
 *
 * Return OK or FAIL.
 */
    స్థిర పూర్ణాంకం
eval_జరుపు_సంఖ్య(typval_T *tv1, typval_T *tv2, పూర్ణాంకం జరుపు_type)
{
    ఒకవేళ (tv2->v_type != VAR_సంఖ్య || tv2->vval.v_సంఖ్య < 0)
    {
	// right operand should be a positive number
	ఒకవేళ (tv2->v_type != VAR_సంఖ్య)
	    emsg(_(e_bitshift_ops_must_be_సంఖ్య));
	లేకపోతే
	    emsg(_(e_bitshift_ops_must_be_ధనాత్మక));
	clear_tv(tv1);
	clear_tv(tv2);
	తిరిగి FAIL;
    }

    ఒకవేళ (tv2->vval.v_సంఖ్య > MAX_LSHIFT_బిట్లు)
	// shifting more bits than we have always results in zero
	tv1->vval.v_సంఖ్య = 0;
    లేకపోతే ఒకవేళ (జరుపు_type == సమాసం_LSHIFT)
	tv1->vval.v_సంఖ్య =
	    (uvarnumber_T)tv1->vval.v_సంఖ్య << tv2->vval.v_సంఖ్య;
    లేకపోతే
	tv1->vval.v_సంఖ్య =
	    (uvarnumber_T)tv1->vval.v_సంఖ్య >> tv2->vval.v_సంఖ్య;

    తిరిగి OK;
}

/*
 * Handle fourth level expression (bitwise left/right shift operators):
 *	var1 << var2
 *	var1 >> var2
 *
 * "arg" must point to the first non-white of the expression.
 * "arg" is advanced to just after the recognized expression.
 *
 * Return OK or FAIL.
 */
    స్థిర పూర్ణాంకం
eval5(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    /*
     * Get the first expression.
     */
    ఒకవేళ (eval6(arg, rettv, evalarg) == FAIL)
	తిరిగి FAIL;

    /*
     * Repeat computing, until no '<<' or '>>' is following.
     */
    కొరకు (;;)
    {
	char_u		*p;
	పూర్ణాంకం		getnext;
	exprtype_T	exprtype;
	పూర్ణాంకం		evaluate;
	typval_T	var2;
	పూర్ణాంకం		vim9script;

	p = eval_తదుపరి_non_blank(*arg, evalarg, &getnext);
	ఒకవేళ (p[0] == '<' && p[1] == '<')
	    exprtype = సమాసం_LSHIFT;
	లేకపోతే ఒకవేళ (p[0] == '>' && p[1] == '>')
	    exprtype = సమాసం_RSHIFT;
	లేకపోతే
	    తిరిగి OK;

	// Handle a bitwise left or right shift operator
	evaluate = evalarg == ఖాళీ ? 0 : (evalarg->eval_flags & EVAL_EVALUATE);
	ఒకవేళ (evaluate && rettv->v_type != VAR_సంఖ్య)
	{
	    // left operand should be a number
	    emsg(_(e_bitshift_ops_must_be_సంఖ్య));
	    clear_tv(rettv);
	    తిరిగి FAIL;
	}

	vim9script = లోపల_vim9script();
	ఒకవేళ (getnext)
	{
	    *arg = eval_తదుపరి_పంక్తి(*arg, evalarg);
	    p = *arg;
	}
	లేకపోతే ఒకవేళ (evaluate && vim9script && !VIM_ISWHITE(**arg))
	{
	    error_white_both(*arg, 2);
	    clear_tv(rettv);
	    తిరిగి FAIL;
	}

	/*
	 * Get the second variable.
	 */
	ఒకవేళ (evaluate && vim9script && !IS_WHITE_OR_NUL(p[2]))
	{
	    error_white_both(p, 2);
	    clear_tv(rettv);
	    తిరిగి FAIL;
	}
	*arg = skipwhite_and_linebreak(p + 2, evalarg);
	ఒకవేళ (eval6(arg, &var2, evalarg) == FAIL)
	{
	    clear_tv(rettv);
	    తిరిగి FAIL;
	}

	ఒకవేళ (evaluate)
	{
	    ఒకవేళ (eval_జరుపు_సంఖ్య(rettv, &var2, exprtype) == FAIL)
		తిరిగి FAIL;
	}

	clear_tv(&var2);
    }

    తిరిగి OK;
}

/*
 * Concatenate strings "tv1" and "tv2" and store the result in "tv1".
 */
    స్థిర పూర్ణాంకం
eval_concat_str(typval_T *tv1, typval_T *tv2)
{
    char_u	buf1[NUMBUFLEN], buf2[NUMBUFLEN];
    char_u	*s1 = tv_get_string_buf(tv1, buf1);
    char_u	*s2 = ఖాళీ;
    char_u	*p;
    పూర్ణాంకం		vim9script = లోపల_vim9script();

    ఒకవేళ (vim9script && (tv2->v_type == VAR_VOID
		|| tv2->v_type == VAR_CHANNEL
		|| tv2->v_type == VAR_పని))
	semsg(_(e_using_invalid_విలువ_as_string_str),
		vartype_name(tv2->v_type));
    లేకపోతే ఒకవేళ (vim9script && tv2->v_type == VAR_FLOAT)
    {
	vim_snprintf((అక్షరం *)buf2, NUMBUFLEN, "%g",
		tv2->vval.v_float);
	s2 = buf2;
    }
    లేకపోతే
	s2 = tv_get_string_buf_chk(tv2, buf2);
    ఒకవేళ (s2 == ఖాళీ)		// type error ?
    {
	clear_tv(tv1);
	clear_tv(tv2);
	తిరిగి FAIL;
    }

    // When possible, grow the existing string in place to avoid alloc/free.
    ఒకవేళ (grow_string_tv(tv1, s2) == OK)
	తిరిగి OK;

    p = concat_str(s1, s2);
    clear_tv(tv1);
    tv1->v_type = VAR_STRING;
    tv1->vval.v_string = p;

    తిరిగి OK;
}

/*
 * Add or subtract numbers "tv1" and "tv2" and store the result in "tv1".
 * The numbers can be whole numbers or floats.
 */
    స్థిర పూర్ణాంకం
eval_addsub_సంఖ్య(typval_T *tv1, typval_T *tv2, పూర్ణాంకం op)
{
    పూర్ణాంకం		error = FALSE;
    varnumber_T	n1, n2;
    float_T	f1 = 0, f2 = 0;

    ఒకవేళ (tv1->v_type == VAR_FLOAT)
    {
	f1 = tv1->vval.v_float;
	n1 = 0;
    }
    లేకపోతే
    {
	n1 = tv_get_సంఖ్య_chk(tv1, &error);
	ఒకవేళ (error)
	{
	    // This can only happen for "list + non-list" or
	    // "blob + non-blob".  For "non-list + ..." or
	    // "something - ...", we returned before evaluating the
	    // 2nd operand.
	    clear_tv(tv1);
	    clear_tv(tv2);
	    తిరిగి FAIL;
	}
	ఒకవేళ (tv2->v_type == VAR_FLOAT)
	    f1 = n1;
    }
    ఒకవేళ (tv2->v_type == VAR_FLOAT)
    {
	f2 = tv2->vval.v_float;
	n2 = 0;
    }
    లేకపోతే
    {
	n2 = tv_get_సంఖ్య_chk(tv2, &error);
	ఒకవేళ (error)
	{
	    clear_tv(tv1);
	    clear_tv(tv2);
	    తిరిగి FAIL;
	}
	ఒకవేళ (tv1->v_type == VAR_FLOAT)
	    f2 = n2;
    }
    clear_tv(tv1);

    // If there is a float on either side the result is a float.
    ఒకవేళ (tv1->v_type == VAR_FLOAT || tv2->v_type == VAR_FLOAT)
    {
	ఒకవేళ (op == '+')
	    f1 = f1 + f2;
	లేకపోతే
	    f1 = f1 - f2;
	tv1->v_type = VAR_FLOAT;
	tv1->vval.v_float = f1;
    }
    లేకపోతే
    {
	ఒకవేళ (op == '+')
	    n1 = n1 + n2;
	లేకపోతే
	    n1 = n1 - n2;
	tv1->v_type = VAR_సంఖ్య;
	tv1->vval.v_సంఖ్య = n1;
    }

    తిరిగి OK;
}

/*
 * Handle fifth level expression:
 *	+	number addition, concatenation of list or blob
 *	-	number subtraction
 *	.	string concatenation (if script version is 1)
 *	..	string concatenation
 *
 * "arg" must point to the first non-white of the expression.
 * "arg" is advanced to just after the recognized expression.
 *
 * Return OK or FAIL.
 */
    స్థిర పూర్ణాంకం
eval6(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    /*
     * Get the first expression.
     */
    ఒకవేళ (eval7(arg, rettv, evalarg, FALSE) == FAIL)
	తిరిగి FAIL;

    /*
     * Repeat computing, until no '+', '-' or '.' is following.
     */
    కొరకు (;;)
    {
	పూర్ణాంకం	    evaluate;
	పూర్ణాంకం	    getnext;
	char_u	    *p;
	పూర్ణాంకం	    op;
	పూర్ణాంకం	    oplen;
	పూర్ణాంకం	    concat;
	typval_T    var2;
	పూర్ణాంకం	    vim9script = లోపల_vim9script();
	దీర్ఘ	    op_lnum = SOURCING_LNUM;

	// "." is only string concatenation when scriptversion is 1
	// "+=", "-=" and "..=" are assignments
	// "++" and "--" on the next line are a separate command.
	p = eval_తదుపరి_non_blank(*arg, evalarg, &getnext);
	op = *p;
	concat = op == '.' && (*(p + 1) == '.' || లోపల_old_script(2));
	ఒకవేళ ((op != '+' && op != '-' && !concat) || p[1] == '='
					       || (p[1] == '.' && p[2] == '='))
	    ఆపు;
	ఒకవేళ (getnext && (op == '+' || op == '-') && p[0] == p[1])
	    ఆపు;

	evaluate = evalarg == ఖాళీ ? 0 : (evalarg->eval_flags & EVAL_EVALUATE);
	oplen = (concat && p[1] == '.') ? 2 : 1;
	ఒకవేళ (getnext)
	    *arg = eval_తదుపరి_పంక్తి(*arg, evalarg);
	లేకపోతే
	{
	    ఒకవేళ (evaluate && vim9script && !VIM_ISWHITE(**arg))
	    {
		error_white_both(*arg, oplen);
		clear_tv(rettv);
		తిరిగి FAIL;
	    }
	    *arg = p;
	}
	ఒకవేళ ((op != '+' || (rettv->v_type != VAR_LIST
						&& rettv->v_type != VAR_TUPLE
						&& rettv->v_type != VAR_BLOB))
		&& (op == '.' || rettv->v_type != VAR_FLOAT)
		&& evaluate)
	{
	    పూర్ణాంకం		error = FALSE;

	    // For "list + ...", an illegal use of the first operand as
	    // a number cannot be determined before evaluating the 2nd
	    // operand: if this is also a list, all is ok.
	    // For "something . ...", "something - ..." or "non-list + ...",
	    // we know that the first operand needs to be a string or number
	    // without evaluating the 2nd operand.  So check before to avoid
	    // side effects after an error.
	    ఒకవేళ (op != '.')
		tv_get_సంఖ్య_chk(rettv, &error);
	    ఒకవేళ ((op == '.' && tv_get_string_chk(rettv) == ఖాళీ) || error)
	    {
		clear_tv(rettv);
		తిరిగి FAIL;
	    }
	}

	/*
	 * Get the second variable.
	 */
	ఒకవేళ (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[oplen]))
	{
	    error_white_both(*arg, oplen);
	    clear_tv(rettv);
	    తిరిగి FAIL;
	}
	*arg = skipwhite_and_linebreak(*arg + oplen, evalarg);
	ఒకవేళ (eval7(arg, &var2, evalarg, !vim9script && op == '.') == FAIL)
	{
	    clear_tv(rettv);
	    తిరిగి FAIL;
	}

	ఒకవేళ (evaluate)
	{
	    /*
	     * Compute the result.
	     */
	    // use the line of the operation for messages
	    SOURCING_LNUM = op_lnum;
	    ఒకవేళ (op == '.')
	    {
		ఒకవేళ (eval_concat_str(rettv, &var2) == FAIL)
		    తిరిగి FAIL;
	    }
	    లేకపోతే ఒకవేళ (op == '+' && rettv->v_type == VAR_BLOB
						   && var2.v_type == VAR_BLOB)
		eval_addblob(rettv, &var2);
	    లేకపోతే ఒకవేళ (op == '+' && rettv->v_type == VAR_LIST
						   && var2.v_type == VAR_LIST)
	    {
		ఒకవేళ (eval_addlist(rettv, &var2) == FAIL)
		    తిరిగి FAIL;
	    }
	    లేకపోతే ఒకవేళ (op == '+' && rettv->v_type == VAR_TUPLE
					   && var2.v_type == VAR_TUPLE)
	    {
		ఒకవేళ (eval_addtuple(rettv, &var2) == FAIL)
		    తిరిగి FAIL;
	    }
	    లేకపోతే
	    {
		ఒకవేళ (eval_addsub_సంఖ్య(rettv, &var2, op) == FAIL)
		    తిరిగి FAIL;
	    }
	    clear_tv(&var2);
	}
    }
    తిరిగి OK;
}

/*
 * Multiply or divide or compute the modulo of numbers "tv1" and "tv2" and
 * store the result in "tv1".  The numbers can be whole numbers or floats.
 */
    స్థిర పూర్ణాంకం
eval_multdiv_సంఖ్య(typval_T *tv1, typval_T *tv2, పూర్ణాంకం op)
{
    varnumber_T	n1, n2;
    float_T	f1, f2;
    పూర్ణాంకం		error;
    పూర్ణాంకం		use_float = FALSE;

    f1 = 0;
    f2 = 0;
    error = FALSE;
    ఒకవేళ (tv1->v_type == VAR_FLOAT)
    {
	f1 = tv1->vval.v_float;
	use_float = TRUE;
	n1 = 0;
    }
    లేకపోతే
	n1 = tv_get_సంఖ్య_chk(tv1, &error);
    clear_tv(tv1);
    ఒకవేళ (error)
    {
	clear_tv(tv2);
	తిరిగి FAIL;
    }

    ఒకవేళ (tv2->v_type == VAR_FLOAT)
    {
	ఒకవేళ (!use_float)
	{
	    f1 = n1;
	    use_float = TRUE;
	}
	f2 = tv2->vval.v_float;
	n2 = 0;
    }
    లేకపోతే
    {
	n2 = tv_get_సంఖ్య_chk(tv2, &error);
	clear_tv(tv2);
	ఒకవేళ (error)
	    తిరిగి FAIL;
	ఒకవేళ (use_float)
	    f2 = n2;
    }

    /*
     * Compute the result.
     * When either side is a float the result is a float.
     */
    ఒకవేళ (use_float)
    {
	ఒకవేళ (op == '*')
	    f1 = f1 * f2;
	లేకపోతే ఒకవేళ (op == '/')
	{
#నిర్వచితమైతే VMS
	    // VMS crashes on divide by zero, work around it
	    ఒకవేళ (f2 == 0.0)
	    {
		ఒకవేళ (f1 == 0)
		    f1 = -1 * __F_FLT_MAX - 1L;   // similar to NaN
		లేకపోతే ఒకవేళ (f1 < 0)
		    f1 = -1 * __F_FLT_MAX;
		లేకపోతే
		    f1 = __F_FLT_MAX;
	    }
	    లేకపోతే
		f1 = f1 / f2;
#లేకపోతే
	    // We rely on the floating point library to handle divide
	    // by zero to result in "inf" and not a crash.
	    f1 = f1 / f2;
#అయితే_ముగింపు
	}
	లేకపోతే
	{
	    emsg(_(e_cannot_use_percent_with_float));
	    తిరిగి FAIL;
	}
	tv1->v_type = VAR_FLOAT;
	tv1->vval.v_float = f1;
    }
    లేకపోతే
    {
	పూర్ణాంకం	    failed = FALSE;

	ఒకవేళ (op == '*')
	    n1 = n1 * n2;
	లేకపోతే ఒకవేళ (op == '/')
	    n1 = అంకె_divide(n1, n2, &failed);
	లేకపోతే
	    n1 = అంకె_modulus(n1, n2, &failed);
	ఒకవేళ (failed)
	    తిరిగి FAIL;

	tv1->v_type = VAR_సంఖ్య;
	tv1->vval.v_సంఖ్య = n1;
    }

    తిరిగి OK;
}

/*
 * Handle sixth level expression:
 *	*	number multiplication
 *	/	number division
 *	%	number modulo
 *
 * "arg" must point to the first non-white of the expression.
 * "arg" is advanced to just after the recognized expression.
 *
 * Return OK or FAIL.
 */
    స్థిర పూర్ణాంకం
eval7(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    పూర్ణాంకం		want_string)  // after "." operator
{
    /*
     * Get the first expression.
     */
    ఒకవేళ (eval8(arg, rettv, evalarg, want_string) == FAIL)
	తిరిగి FAIL;

    /*
     * Repeat computing, until no '*', '/' or '%' is following.
     */
    కొరకు (;;)
    {
	పూర్ణాంకం	    evaluate;
	పూర్ణాంకం	    getnext;
	typval_T    var2;
	char_u	    *p;
	పూర్ణాంకం	    op;

	// "*=", "/=" and "%=" are assignments
	p = eval_తదుపరి_non_blank(*arg, evalarg, &getnext);
	op = *p;
	ఒకవేళ ((op != '*' && op != '/' && op != '%') || p[1] == '=')
	    ఆపు;

	evaluate = evalarg == ఖాళీ ? 0 : (evalarg->eval_flags & EVAL_EVALUATE);
	ఒకవేళ (getnext)
	    *arg = eval_తదుపరి_పంక్తి(*arg, evalarg);
	లేకపోతే
	{
	    ఒకవేళ (evaluate && లోపల_vim9script() && !VIM_ISWHITE(**arg))
	    {
		error_white_both(*arg, 1);
		clear_tv(rettv);
		తిరిగి FAIL;
	    }
	    *arg = p;
	}

	/*
	 * Get the second variable.
	 */
	ఒకవేళ (evaluate && లోపల_vim9script() && !IS_WHITE_OR_NUL((*arg)[1]))
	{
	    error_white_both(*arg, 1);
	    clear_tv(rettv);
	    తిరిగి FAIL;
	}
	*arg = skipwhite_and_linebreak(*arg + 1, evalarg);
	ఒకవేళ (eval8(arg, &var2, evalarg, FALSE) == FAIL)
	    తిరిగి FAIL;

	ఒకవేళ (evaluate)
	    // Compute the result.
	    ఒకవేళ (eval_multdiv_సంఖ్య(rettv, &var2, op) == FAIL)
		తిరిగి FAIL;
    }

    తిరిగి OK;
}

/*
 * Handle seventh level expression:
 *	a type cast before a base level expression.
 * "arg" must point to the first non-white of the expression.
 * "arg" is advanced to just after the recognized expression.
 * Return OK or FAIL.
 */
    స్థిర పూర్ణాంకం
eval8(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    పూర్ణాంకం		want_string)	// after "." operator
{
    type_T	*want_type = ఖాళీ;
    garray_T	type_list;	    // list of pointers to allocated types
    పూర్ణాంకం		res;
    పూర్ణాంకం		evaluate = evalarg == ఖాళీ ? 0
				       : (evalarg->eval_flags & EVAL_EVALUATE);

    // Recognize <type> in Vim9 script only.
    ఒకవేళ (లోపల_vim9script() && **arg == '<' && eval_isnamec1((*arg)[1])
					     && STRNCMP(*arg, "<SNR>", 5) != 0)
    {
	++*arg;
	ga_init2(&type_list, పరిమాణం(type_T *), 10);
	ఒకవేళ (!evaluate)
	    // Only skip over the type, it may use a type variable that is only
	    // known when the expression is compiled, e.g. in a lambda.
	    *arg = skip_type(*arg, FALSE);
	లేకపోతే
	{
	    want_type = parse_type(arg, &type_list, ఖాళీ, ఖాళీ, TRUE);
	    ఒకవేళ (want_type == ఖాళీ)
	    {
		clear_type_list(&type_list);
		తిరిగి FAIL;
	    }
	}

	ఒకవేళ (**arg != '>')
	{
	    ఒకవేళ (*skipwhite(*arg) == '>')
		semsg(_(e_no_white_space_allowed_before_str_str), ">", *arg);
	    లేకపోతే
		emsg(_(e_missing_gt));
	    clear_type_list(&type_list);
	    తిరిగి FAIL;
	}
	++*arg;
	*arg = skipwhite_and_linebreak(*arg, evalarg);
    }

    res = eval9(arg, rettv, evalarg, want_string);

    ఒకవేళ (want_type != ఖాళీ)
    {
	ఒకవేళ (evaluate && res == OK)
	{
	    type_T *actual = typval2type(rettv, get_copyID(), &type_list,
							       TVTT_DO_MEMBER);

	    ఒకవేళ (!equal_type(want_type, actual, 0))
	    {
		ఒకవేళ (want_type->tt_type == VAR_BOOL
					&& actual->tt_type != VAR_BOOL
					&& (actual->tt_flags & TTFLAG_BOOL_OK))
		{
		    పూర్ణాంకం n = tv2bool(rettv);

		    // can use "0" and "1" for boolean in some places
		    clear_tv(rettv);
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_సంఖ్య = n ? VVAL_TRUE : VVAL_FALSE;
		}
		లేకపోతే
		{
		    where_T where = WHERE_INIT;

		    res = check_type(want_type, actual, TRUE, where);
		}
	    }
	}
	clear_type_list(&type_list);
    }

    తిరిగి res;
}

    పూర్ణాంకం
eval_leader(char_u **arg, పూర్ణాంకం vim9)
{
    char_u	*s = *arg;
    char_u	*p = *arg;

    అయితే (*p == '!' || *p == '-' || *p == '+')
    {
	char_u *n = skipwhite(p + 1);

	// ++, --, -+ and +- are not accepted in Vim9 script
	ఒకవేళ (vim9 && (*p == '-' || *p == '+') && (*n == '-' || *n == '+'))
	{
	    semsg(_(e_invalid_వ్యక్తీకరణ_str), s);
	    తిరిగి FAIL;
	}
	p = n;
    }
    *arg = p;
    తిరిగి OK;
}

/*
 * Check for a predefined value "true", "false" and "null.*".
 * Return OK when recognized.
 */
    పూర్ణాంకం
handle_predefined(char_u *s, పూర్ణాంకం len, typval_T *rettv)
{
    ఎంపిక (len)
    {
	స్థితి 4: ఒకవేళ (STRNCMP(s, "true", 4) == 0)
		{
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_సంఖ్య = VVAL_TRUE;
		    తిరిగి OK;
		}
		ఒకవేళ (STRNCMP(s, "null", 4) == 0)
		{
		    rettv->v_type = VAR_SPECIAL;
		    rettv->vval.v_సంఖ్య = VVAL_NULL;
		    తిరిగి OK;
		}
		ఆపు;
	స్థితి 5: ఒకవేళ (STRNCMP(s, "false", 5) == 0)
		{
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_సంఖ్య = VVAL_FALSE;
		    తిరిగి OK;
		}
		ఆపు;
	స్థితి 8: ఒకవేళ (STRNCMP(s, "null_job", 8) == 0)
		{
#నిర్వచితమైతే FEAT_పని_CHANNEL
		    rettv->v_type = VAR_పని;
		    rettv->vval.v_పని = ఖాళీ;
#లేకపోతే
		    rettv->v_type = VAR_SPECIAL;
		    rettv->vval.v_సంఖ్య = VVAL_NULL;
#అయితే_ముగింపు
		    తిరిగి OK;
		}
		ఆపు;
	స్థితి 9:
		ఒకవేళ (STRNCMP(s, "null_", 5) != 0)
		    ఆపు;
		// null_list
		ఒకవేళ (STRNCMP(s + 5, "list", 4) == 0)
		{
		    rettv->v_type = VAR_LIST;
		    rettv->vval.v_list = ఖాళీ;
		    తిరిగి OK;
		}
		// null_dict
		ఒకవేళ (STRNCMP(s + 5, "dict", 4) == 0)
		{
		    rettv->v_type = VAR_DICT;
		    rettv->vval.v_dict = ఖాళీ;
		    తిరిగి OK;
		}
		// null_blob
		ఒకవేళ (STRNCMP(s + 5, "blob", 4) == 0)
		{
		    rettv->v_type = VAR_BLOB;
		    rettv->vval.v_blob = ఖాళీ;
		    తిరిగి OK;
		}
		ఆపు;
	స్థితి 10:
		ఒకవేళ (STRNCMP(s, "null_", 5) != 0)
		    ఆపు;
		// null_class
		ఒకవేళ (STRNCMP(s + 5, "class", 5) == 0)
		{
		    rettv->v_type = VAR_CLASS;
		    rettv->vval.v_class = ఖాళీ;
		    తిరిగి OK;
		}
		ఒకవేళ (STRNCMP(s + 5, "tuple", 5) == 0)
		{
		    rettv->v_type = VAR_TUPLE;
		    rettv->vval.v_tuple = ఖాళీ;
		    తిరిగి OK;
		}
		ఆపు;
	స్థితి 11: ఒకవేళ (STRNCMP(s, "null_string", 11) == 0)
		{
		    rettv->v_type = VAR_STRING;
		    rettv->vval.v_string = ఖాళీ;
		    తిరిగి OK;
		}
		ఒకవేళ (STRNCMP(s, "null_object", 11) == 0)
		{
		    rettv->v_type = VAR_OBJECT;
		    rettv->vval.v_object = ఖాళీ;
		    తిరిగి OK;
		}
		ఆపు;
	స్థితి 12:
		ఒకవేళ (STRNCMP(s, "null_channel", 12) == 0)
		{
#నిర్వచితమైతే FEAT_పని_CHANNEL
		    rettv->v_type = VAR_CHANNEL;
		    rettv->vval.v_channel = ఖాళీ;
#లేకపోతే
		    rettv->v_type = VAR_SPECIAL;
		    rettv->vval.v_సంఖ్య = VVAL_NULL;
#అయితే_ముగింపు
		    తిరిగి OK;
		}
		ఒకవేళ (STRNCMP(s, "null_partial", 12) == 0)
		{
		    rettv->v_type = VAR_PARTIAL;
		    rettv->vval.v_partial = ఖాళీ;
		    తిరిగి OK;
		}
		ఆపు;
	స్థితి 13: ఒకవేళ (STRNCMP(s, "null_function", 13) == 0)
		{
		    rettv->v_type = VAR_FUNC;
		    rettv->vval.v_string = ఖాళీ;
		    తిరిగి OK;
		}
		ఆపు;
    }
    తిరిగి FAIL;
}

/*
 * Handle register contents: @r.
 */
    స్థిర శూన్య
eval9_reg_contents(
    char_u	**arg,
    typval_T	*rettv,
    పూర్ణాంకం		evaluate)
{
    పూర్ణాంకం		vim9script = లోపల_vim9script();

    ++*arg;	// skip '@'
    ఒకవేళ (evaluate)
    {
	ఒకవేళ (vim9script && IS_WHITE_OR_NUL(**arg))
	    semsg(_(e_syntax_error_at_str), *arg);
	లేకపోతే ఒకవేళ (vim9script && !valid_yank_reg(**arg, FALSE))
	    emsg_invreg(**arg);
	లేకపోతే
	{
	    rettv->v_type = VAR_STRING;
	    rettv->vval.v_string = get_reg_contents(**arg,
		    GREG_సమాసం_SRC);
	}
    }
    ఒకవేళ (**arg != NUL)
	++*arg;
}

/*
 * Handle a nested expression: (expression) or lambda: (arg) => expr
 */
    స్థిర పూర్ణాంకం
eval9_nested_సమాసం(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    పూర్ణాంకం		evaluate)
{
    పూర్ణాంకం		ret = NOTDONE;
    పూర్ణాంకం		vim9script = లోపల_vim9script();

    ఒకవేళ (vim9script)
    {
	ret = get_lambda_tv(arg, rettv, TRUE, evalarg, ఖాళీ);
	ఒకవేళ (ret == OK && evaluate)
	{
	    ufunc_T *ufunc = rettv->vval.v_partial->pt_func;

	    // Compile it here to get the return type.  The return
	    // type is optional, when it's missing use t_unknown.
	    // This is recognized in compile_return().
	    ఒకవేళ (ufunc->uf_ret_type->tt_type == VAR_VOID)
		ufunc->uf_ret_type = &t_unknown;
	    ఒకవేళ (compile_def_function(ufunc, FALSE,
				get_compile_type(ufunc), ఖాళీ) == FAIL)
	    {
		clear_tv(rettv);
		ret = FAIL;
	    }
	}
    }
    ఒకవేళ (ret == NOTDONE)
    {
	*arg = skipwhite_and_linebreak(*arg + 1, evalarg);
	ఒకవేళ (**arg == ')')
	    // empty tuple
	    ret = eval_tuple(arg, rettv, evalarg, TRUE);
	లేకపోతే
	{
	    ret = eval1(arg, rettv, evalarg);	// recursive!
	    ఒకవేళ (ret != OK)
		తిరిగి ret;

	    *arg = skipwhite_and_linebreak(*arg, evalarg);

	    ఒకవేళ (**arg == ',')
		// tuple
		ret = eval_tuple(arg, rettv, evalarg, TRUE);
	    లేకపోతే ఒకవేళ (**arg == ')')
		++*arg;
	    లేకపోతే ఒకవేళ (ret == OK)
	    {
		emsg(_(e_missing_closing_paren));
		clear_tv(rettv);
		ret = FAIL;
	    }
	}
    }

    తిరిగి ret;
}

/*
* Handle be a variable or function name.
* Can also be a curly-braces kind of name: {expr}.
*/
    స్థిర పూర్ణాంకం
eval9_var_func_name(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    పూర్ణాంకం		evaluate,
    char_u	**name_ప్రారంభం)
{
    char_u	*s;
    పూర్ణాంకం		len;
    char_u	*alias;
    పూర్ణాంకం		ret = OK;
    పూర్ణాంకం		vim9script = లోపల_vim9script();

    s = *arg;
    len = get_name_len(arg, &alias, evaluate, TRUE);
    ఒకవేళ (alias != ఖాళీ)
	s = alias;

    ఒకవేళ (len <= 0)
	ret = FAIL;
    లేకపోతే
    {
	పూర్ణాంకం	flags = evalarg == ఖాళీ ? 0 : evalarg->eval_flags;

	ఒకవేళ (evaluate && vim9script && len == 1 && *s == '_')
	{
	    emsg(_(e_cannot_use_underscore_ఇక్కడ));
	    ret = FAIL;
	}
	లేకపోతే ఒకవేళ (evaluate && vim9script && len > 2
						&& s[0] == 's' && s[1] == ':')
	{
	    semsg(_(e_cannot_use_s_colon_లోపల_vim9_script_str), s);
	    ret = FAIL;
	}
	లేకపోతే ఒకవేళ ((vim9script ? **arg : *skipwhite(*arg)) == '('
				|| (vim9script && generic_func_call(arg)))
	{
	    // "name(..."  recursive!
	    *arg = skipwhite(*arg);
	    ret = eval_func(arg, evalarg, s, len, rettv, flags, ఖాళీ);
	}
	లేకపోతే ఒకవేళ (evaluate)
	{
	    // get the value of "true", "false", etc. or a variable
	    ret = FAIL;
	    ఒకవేళ (vim9script)
		ret = handle_predefined(s, len, rettv);
	    ఒకవేళ (ret == FAIL)
	    {
		*name_ప్రారంభం = s;
		ret = eval_variable(s, len, 0, rettv, ఖాళీ,
					EVAL_VAR_VERBOSE + EVAL_VAR_IMPORT);

		// skip the generic function arguments (if present)
		// they are already processed by eval_variable
		ఒకవేళ (ret == OK && vim9script && **arg == '<'
						&& rettv->v_type == VAR_FUNC)
		    ret = skip_generic_func_type_వాదనలు(arg);
	    }
	}
	లేకపోతే
	{
	    // skip the name
	    check_vars(s, len);

	    // Skip the type arguments of a generic function, its name starts
	    // with an uppercase letter.
	    ఒకవేళ (vim9script && **arg == '<' && ASCII_ISUPPER(*s))
		skip_generic_func_type_వాదనలు(arg);
	    ret = OK;
	}
    }
    vim_free(alias);

    తిరిగి ret;
}

/*
 * Handle eighth level expression:
 *  number		number constant
 *  0zFFFFFFFF		Blob constant
 *  "string"		string constant
 *  'string'		literal string constant
 *  &option-name	option value
 *  @r			register contents
 *  identifier		variable value
 *  function()		function call
 *  $VAR		environment variable
 *  (expression)	nested expression
 *  [expr, expr]	List
 *  (expr, expr)	Tuple
 *  {arg, arg -> expr}	Lambda
 *  {key: val, key: val}   Dictionary
 *  #{key: val, key: val}  Dictionary with literal keys
 *
 *  Also handle:
 *  ! in front		logical NOT
 *  - in front		unary minus
 *  + in front		unary plus (ignored)
 *  trailing []		subscript in String or List or Tuple
 *  trailing .name	entry in Dictionary
 *  trailing ->name()	method call
 *
 * "arg" must point to the first non-white of the expression.
 * "arg" is advanced to just after the recognized expression.
 *
 * Return OK or FAIL.
 */
    స్థిర పూర్ణాంకం
eval9(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    పూర్ణాంకం		want_string)	// after "." operator
{
    పూర్ణాంకం		evaluate = evalarg != ఖాళీ
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    char_u	*name_ప్రారంభం = ఖాళీ;
    char_u	*ప్రారంభం_leader, *చివర_leader;
    పూర్ణాంకం		ret = OK;
    స్థిర పూర్ణాంకం	recurse = 0;
    పూర్ణాంకం		vim9script = లోపల_vim9script();

    /*
     * Initialise variable so that clear_tv() can't mistake this for a
     * string and free a string that isn't there.
     */
    rettv->v_type = VAR_UNKNOWN;

    /*
     * Skip '!', '-' and '+' characters.  They are handled later.
     */
    ప్రారంభం_leader = *arg;
    ఒకవేళ (eval_leader(arg, vim9script) == FAIL)
	తిరిగి FAIL;
    చివర_leader = *arg;

    ఒకవేళ (**arg == '.' && (!సురక్షిత_isdigit(*(*arg + 1)) || లోపల_old_script(2)))
    {
	semsg(_(e_invalid_వ్యక్తీకరణ_str), *arg);
	++*arg;
	తిరిగి FAIL;
    }

    // Limit recursion to 1000 levels.  At least at 10000 we run out of stack
    // and crash.  With MSVC the stack is smaller.
    ఒకవేళ (recurse ==
#నిర్వచితమైతే _MSC_VER
		    300
#లేకపోతే
		    1000
#అయితే_ముగింపు
		    )
    {
	semsg(_(e_వ్యక్తీకరణ_too_recursive_str), *arg);
	తిరిగి FAIL;
    }
    ++recurse;

    ఎంపిక (**arg)
    {
    /*
     * Number constant.
     */
    స్థితి '0':
    స్థితి '1':
    స్థితి '2':
    స్థితి '3':
    స్థితి '4':
    స్థితి '5':
    స్థితి '6':
    స్థితి '7':
    స్థితి '8':
    స్థితి '9':
    స్థితి '.':	ret = eval_సంఖ్య(arg, rettv, evaluate, want_string);

		// Apply prefixed "-" and "+" now.  Matters especially when
		// "->" follows.
		ఒకవేళ (ret == OK && evaluate && చివర_leader > ప్రారంభం_leader
						  && rettv->v_type != VAR_BLOB)
		    ret = eval9_leader(rettv, TRUE, ప్రారంభం_leader, &చివర_leader);
		ఆపు;

    /*
     * String constant: "string".
     */
    స్థితి '"':	ret = eval_string(arg, rettv, evaluate, FALSE);
		ఆపు;

    /*
     * Literal string constant: 'str''ing'.
     */
    స్థితి '\'':	ret = eval_lit_string(arg, rettv, evaluate, FALSE);
		ఆపు;

    /*
     * List: [expr, expr]
     */
    స్థితి '[':	ret = eval_list(arg, rettv, evalarg, TRUE);
		ఆపు;

    /*
     * Literal Dictionary: #{key: val, key: val}
     */
    స్థితి '#':	ret = eval_lit_dict(arg, rettv, evalarg);
		ఆపు;

    /*
     * Lambda: {arg, arg -> expr}
     * Dictionary: {'key': val, 'key': val}
     */
    స్థితి '{':	ఒకవేళ (vim9script)
		    ret = NOTDONE;
		లేకపోతే
		    ret = get_lambda_tv(arg, rettv, vim9script, evalarg, ఖాళీ);
		ఒకవేళ (ret == NOTDONE)
		    ret = eval_dict(arg, rettv, evalarg, FALSE);
		ఆపు;

    /*
     * Option value: &name
     */
    స్థితి '&':	ret = eval_option(arg, rettv, evaluate);
		ఆపు;

    /*
     * Environment variable: $VAR.
     * Interpolated string: $"string" or $'string'.
     */
    స్థితి '$':	ఒకవేళ ((*arg)[1] == '"' || (*arg)[1] == '\'')
		    ret = eval_interp_string(arg, rettv, evaluate);
		లేకపోతే
		    ret = eval_env_var(arg, rettv, evaluate);
		ఆపు;

    /*
     * Register contents: @r.
     */
    స్థితి '@':	eval9_reg_contents(arg, rettv, evaluate);
		ఆపు;

    /*
     * nested expression: (expression).
     * or lambda: (arg) => expr
     * or tuple
     */
    స్థితి '(':	ret = eval9_nested_సమాసం(arg, rettv, evalarg, evaluate);
		ఆపు;

    అప్రమేయ:	ret = NOTDONE;
		ఆపు;
    }

    ఒకవేళ (ret == NOTDONE)
    {
	/*
	 * Must be a variable or function name.
	 * Can also be a curly-braces kind of name: {expr}.
	 */
	ret = eval9_var_func_name(arg, rettv, evalarg, evaluate, &name_ప్రారంభం);
    }

    // Handle following '[', '(' and '.' for expr[expr], expr.name,
    // expr(expr), expr->name(expr)
    ఒకవేళ (ret == OK)
	ret = handle_subscript(arg, name_ప్రారంభం, rettv, evalarg, evaluate);

    /*
     * Apply logical NOT and unary '-', from right to left, ignore '+'.
     */
    ఒకవేళ (ret == OK && evaluate && చివర_leader > ప్రారంభం_leader)
	ret = eval9_leader(rettv, FALSE, ప్రారంభం_leader, &చివర_leader);

    --recurse;
    తిరిగి ret;
}

/*
 * Apply the leading "!" and "-" before an eval9 expression to "rettv".
 * When "numeric_only" is TRUE only handle "+" and "-".
 * Adjusts "end_leaderp" until it is at "start_leader".
 */
    స్థిర పూర్ణాంకం
eval9_leader(
	typval_T    *rettv,
	పూర్ణాంకం	    numeric_only,
	char_u	    *ప్రారంభం_leader,
	char_u	    **చివర_leaderp)
{
    char_u	*చివర_leader = *చివర_leaderp;
    పూర్ణాంకం		ret = OK;
    పూర్ణాంకం		error = FALSE;
    varnumber_T val = 0;
    vartype_T	type = rettv->v_type;
    పూర్ణాంకం		vim9script = లోపల_vim9script();
    float_T	    f = 0.0;

    ఒకవేళ (rettv->v_type == VAR_FLOAT)
	f = rettv->vval.v_float;
    లేకపోతే
    {
	అయితే (VIM_ISWHITE(చివర_leader[-1]))
	    --చివర_leader;
	ఒకవేళ (vim9script && చివర_leader[-1] == '!')
	{
	    // Is this typeval supported with the ! operator?
	    ఒకవేళ (check_typval_is_విలువ(rettv) == FAIL)
	    {
		clear_tv(rettv);
		తిరిగి FAIL;
	    }
	    val = tv2bool(rettv);
	}
	లేకపోతే
	    val = tv_get_సంఖ్య_chk(rettv, &error);
    }
    ఒకవేళ (error)
    {
	clear_tv(rettv);
	ret = FAIL;
    }
    లేకపోతే
    {
	అయితే (చివర_leader > ప్రారంభం_leader)
	{
	    --చివర_leader;
	    ఒకవేళ (*చివర_leader == '!')
	    {
		ఒకవేళ (numeric_only)
		{
		    ++చివర_leader;
		    ఆపు;
		}
		ఒకవేళ (rettv->v_type == VAR_FLOAT)
		{
		    rettv->v_type = VAR_BOOL;
		    val = f == 0.0 ? VVAL_TRUE : VVAL_FALSE;
		    type = VAR_BOOL;
		}
		లేకపోతే
		{
		    val = !val;
		    type = VAR_BOOL;
		}
	    }
	    లేకపోతే ఒకవేళ (*చివర_leader == '-')
	    {
		ఒకవేళ (rettv->v_type == VAR_FLOAT)
		    f = -f;
		లేకపోతే
		{
		    val = -val;
		    type = VAR_సంఖ్య;
		}
	    }
	}
	ఒకవేళ (rettv->v_type == VAR_FLOAT)
	{
	    clear_tv(rettv);
	    rettv->vval.v_float = f;
	}
	లేకపోతే
	{
	    clear_tv(rettv);
	    ఒకవేళ (vim9script)
		rettv->v_type = type;
	    లేకపోతే
		rettv->v_type = VAR_సంఖ్య;
	    rettv->vval.v_సంఖ్య = val;
	}
    }
    *చివర_leaderp = చివర_leader;
    తిరిగి ret;
}

/*
 * Call the function referred to in "rettv".
 */
    స్థిర పూర్ణాంకం
call_func_rettv(
	char_u	    **arg,
	evalarg_T   *evalarg,
	typval_T    *rettv,
	పూర్ణాంకం	    evaluate,
	dict_T	    *selfdict,
	typval_T    *basetv)
{
    partial_T	*pt = ఖాళీ;
    funcexe_T	funcexe;
    typval_T	functv;
    char_u	*s;
    పూర్ణాంకం		ret;

    // need to copy the funcref so that we can clear rettv
    ఒకవేళ (evaluate)
    {
	functv = *rettv;
	rettv->v_type = VAR_UNKNOWN;

	// Invoke the function.  Recursive!
	ఒకవేళ (functv.v_type == VAR_PARTIAL)
	{
	    pt = functv.vval.v_partial;
	    s = partial_name(pt);
	}
	లేకపోతే
	{
	    s = functv.vval.v_string;
	    ఒకవేళ (s == ఖాళీ || *s == NUL)
	    {
		emsg(_(e_ఖాళీలు_function_name));
		ret = FAIL;
		వెళ్ళు theend;
	    }
	}
    }
    లేకపోతే
	s = (char_u *)"";

    CLEAR_క్షేత్రం(funcexe);
    funcexe.fe_firstline = curwin->w_cursor.lnum;
    funcexe.fe_lastline = curwin->w_cursor.lnum;
    funcexe.fe_evaluate = evaluate;
    funcexe.fe_partial = pt;
    ఒకవేళ (pt != ఖాళీ)
    {
	funcexe.fe_object = pt->pt_obj;
	ఒకవేళ (funcexe.fe_object != ఖాళీ)
	    ++funcexe.fe_object->obj_refcount;
    }
    funcexe.fe_selfdict = selfdict;
    funcexe.fe_basetv = basetv;
    ఒకవేళ (evalarg != ఖాళీ)
	funcexe.fe_cctx = evalarg->eval_cctx;
    ret = get_func_tv(s, -1, rettv, arg, evalarg, &funcexe);

theend:
    // Clear the funcref afterwards, so that deleting it while
    // evaluating the arguments is possible (see test55).
    ఒకవేళ (evaluate)
	clear_tv(&functv);

    తిరిగి ret;
}

/*
 * Evaluate "->method()".
 * "*arg" points to "method".
 * Returns FAIL or OK. "*arg" is advanced to after the ')'.
 */
    స్థిర పూర్ణాంకం
eval_lambda(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    పూర్ణాంకం		verbose)	// give error messages
{
    పూర్ణాంకం		evaluate = evalarg != ఖాళీ
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    typval_T	పునాది = *rettv;
    పూర్ణాంకం		ret;

    rettv->v_type = VAR_UNKNOWN;

    ఒకవేళ (**arg == '{')
    {
	// ->{lambda}()
	ret = get_lambda_tv(arg, rettv, FALSE, evalarg, ఖాళీ);
    }
    లేకపోతే
    {
	// ->(lambda)()
	++*arg;
	ret = eval1(arg, rettv, evalarg);
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	ఒకవేళ (**arg != ')')
	{
	    emsg(_(e_missing_closing_paren));
	    తిరిగి FAIL;
	}
	ఒకవేళ (rettv->v_type != VAR_STRING && rettv->v_type != VAR_FUNC
					       && rettv->v_type != VAR_PARTIAL)
	{
	    emsg(_(e_string_or_function_required_for_arrow_parens_సమాసం));
	    తిరిగి FAIL;
	}
	++*arg;
    }
    ఒకవేళ (ret != OK)
	తిరిగి FAIL;

    ఒకవేళ (**arg != '(')
    {
	ఒకవేళ (verbose)
	{
	    ఒకవేళ (*skipwhite(*arg) == '(')
		emsg(_(e_no_white_space_allowed_before_parenthesis));
	    లేకపోతే
		semsg(_(e_missing_parenthesis_str), "lambda");
	}
	clear_tv(rettv);
	ret = FAIL;
    }
    లేకపోతే
	ret = call_func_rettv(arg, evalarg, rettv, evaluate, ఖాళీ, &పునాది);

    // Clear the funcref afterwards, so that deleting it while
    // evaluating the arguments is possible (see test55).
    ఒకవేళ (evaluate)
	clear_tv(&పునాది);

    తిరిగి ret;
}

/*
 * Evaluate "->method()".
 * "*arg" points to "method".
 * Returns FAIL or OK. "*arg" is advanced to after the ')'.
 */
    స్థిర పూర్ణాంకం
eval_పద్ధతి(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    పూర్ణాంకం		verbose)	// give error messages
{
    char_u	*name;
    దీర్ఘ	len;
    char_u	*alias;
    char_u	*tofree = ఖాళీ;
    typval_T	పునాది = *rettv;
    పూర్ణాంకం		ret = OK;
    పూర్ణాంకం		evaluate = evalarg != ఖాళీ
				      && (evalarg->eval_flags & EVAL_EVALUATE);

    rettv->v_type = VAR_UNKNOWN;

    name = *arg;
    len = get_name_len(arg, &alias, evaluate, evaluate);
    ఒకవేళ (alias != ఖాళీ)
	name = alias;

    ఒకవేళ (len <= 0)
    {
	ఒకవేళ (verbose)
	    emsg(_(e_missing_name_after_పద్ధతి));
	ret = FAIL;
    }
    లేకపోతే
    {
	char_u *paren;

	// If there is no "(" immediately following, but there is further on,
	// it can be "import.Func()", "dict.Func()", "list[nr]", etc.
	// Does not handle anything where "(" is part of the expression.
	*arg = skipwhite(*arg);

	ఒకవేళ (**arg != '(' && alias == ఖాళీ
				    && (paren = vim_strchr(*arg, '(')) != ఖాళీ)
	{
	    *arg = name;

	    // Truncate the name at the "(".  Avoid trying to get another line
	    // by making "getline" NULL.
	    *paren = NUL;
	    char_u	*(*getline)(పూర్ణాంకం, శూన్య *, పూర్ణాంకం, getline_opt_T) = ఖాళీ;
	    ఒకవేళ (evalarg != ఖాళీ)
	    {
		getline = evalarg->eval_getline;
		evalarg->eval_getline = ఖాళీ;
	    }

	    char_u *deref = deref_function_name(arg, &tofree, evalarg, verbose);
	    ఒకవేళ (deref == ఖాళీ)
	    {
		*arg = name + len;
		ret = FAIL;
	    }
	    లేకపోతే
	    {
		name = deref;
		len = (దీర్ఘ)STRLEN(name);
	    }

	    *paren = '(';
	    ఒకవేళ (getline != ఖాళీ)
		evalarg->eval_getline = getline;
	}

	ఒకవేళ (ret == OK)
	{
	    *arg = skipwhite(*arg);

	    ఒకవేళ (**arg != '(')
	    {
		ఒకవేళ (verbose)
		    semsg(_(e_missing_parenthesis_str), name);
		ret = FAIL;
	    }
	    లేకపోతే ఒకవేళ (VIM_ISWHITE((*arg)[-1]))
	    {
		ఒకవేళ (verbose)
		    emsg(_(e_no_white_space_allowed_before_parenthesis));
		ret = FAIL;
	    }
	    లేకపోతే
		ret = eval_func(arg, evalarg, name, len, rettv,
					  evaluate ? EVAL_EVALUATE : 0, &పునాది);
	}
    }

    // Clear the funcref afterwards, so that deleting it while
    // evaluating the arguments is possible (see test55).
    ఒకవేళ (evaluate)
	clear_tv(&పునాది);
    vim_free(tofree);

    ఒకవేళ (alias != ఖాళీ)
	vim_free(alias);

    తిరిగి ret;
}

/*
 * Evaluate an "[expr]" or "[expr:expr]" index.  Also "dict.key".
 * "*arg" points to the '[' or '.'.
 * Returns FAIL or OK. "*arg" is advanced to after the ']'.
 */
    స్థిర పూర్ణాంకం
eval_సూచీ(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    పూర్ణాంకం		verbose)	// give error messages
{
    పూర్ణాంకం		evaluate = evalarg != ఖాళీ
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    పూర్ణాంకం		empty1 = FALSE, empty2 = FALSE;
    typval_T	var1, var2;
    పూర్ణాంకం		range = FALSE;
    char_u	*కీలకం = ఖాళీ;
    పూర్ణాంకం		keylen = -1;
    పూర్ణాంకం		vim9script = లోపల_vim9script();

    ఒకవేళ (check_can_సూచీ(rettv, evaluate, verbose) == FAIL)
	తిరిగి FAIL;

    init_tv(&var1);
    init_tv(&var2);
    ఒకవేళ (**arg == '.')
    {
	/*
	 * dict.name
	 */
	కీలకం = *arg + 1;
	కొరకు (keylen = 0; eval_isdictc(కీలకం[keylen]); ++keylen)
	    ;
	ఒకవేళ (keylen == 0)
	    తిరిగి FAIL;
	ఒకవేళ (vim9script && కీలకం[keylen] == '<')
	{
	    // skip generic type arguments
	    char_u	*p = &కీలకం[keylen];

	    ఒకవేళ (skip_generic_func_type_వాదనలు(&p) == FAIL)
		తిరిగి FAIL;
	    keylen = p - కీలకం;
	}
	*arg = కీలకం + keylen;
    }
    లేకపోతే
    {
	/*
	 * something[idx]
	 *
	 * Get the (first) variable from inside the [].
	 */
	*arg = skipwhite_and_linebreak(*arg + 1, evalarg);
	ఒకవేళ (**arg == ':')
	    empty1 = TRUE;
	లేకపోతే ఒకవేళ (eval1(arg, &var1, evalarg) == FAIL)	// recursive!
	    తిరిగి FAIL;
	లేకపోతే ఒకవేళ (vim9script && **arg == ':')
	{
	    semsg(_(e_white_space_required_before_and_after_str_at_str),
								    ":", *arg);
	    clear_tv(&var1);
	    తిరిగి FAIL;
	}
	లేకపోతే ఒకవేళ (evaluate)
	{
	    పూర్ణాంకం error = FALSE;

	    // allow for indexing with float
	    ఒకవేళ (vim9script && rettv->v_type == VAR_DICT
						   && var1.v_type == VAR_FLOAT)
	    {
		var1.vval.v_string = typval_tostring(&var1, TRUE);
		var1.v_type = VAR_STRING;
	    }

	    ఒకవేళ (vim9script && (rettv->v_type == VAR_LIST
						|| rettv->v_type == VAR_TUPLE))
		tv_get_సంఖ్య_chk(&var1, &error);
	    లేకపోతే
		error = tv_get_string_chk(&var1) == ఖాళీ;
	    ఒకవేళ (error)
	    {
		// not a number or string
		clear_tv(&var1);
		తిరిగి FAIL;
	    }
	}

	/*
	 * Get the second variable from inside the [:].
	 */
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	ఒకవేళ (**arg == ':')
	{
	    range = TRUE;
	    ++*arg;
	    ఒకవేళ (vim9script && !IS_WHITE_OR_NUL(**arg) && **arg != ']')
	    {
		semsg(_(e_white_space_required_before_and_after_str_at_str),
								":", *arg - 1);
		ఒకవేళ (!empty1)
		    clear_tv(&var1);
		తిరిగి FAIL;
	    }
	    *arg = skipwhite_and_linebreak(*arg, evalarg);
	    ఒకవేళ (**arg == ']')
		empty2 = TRUE;
	    లేకపోతే ఒకవేళ (eval1(arg, &var2, evalarg) == FAIL)	// recursive!
	    {
		ఒకవేళ (!empty1)
		    clear_tv(&var1);
		తిరిగి FAIL;
	    }
	    లేకపోతే ఒకవేళ (evaluate && tv_get_string_chk(&var2) == ఖాళీ)
	    {
		// not a number or string
		ఒకవేళ (!empty1)
		    clear_tv(&var1);
		clear_tv(&var2);
		తిరిగి FAIL;
	    }
	}

	// Check for the ']'.
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	ఒకవేళ (**arg != ']')
	{
	    ఒకవేళ (verbose)
		emsg(_(e_missing_closing_వర్గం_brace));
	    clear_tv(&var1);
	    ఒకవేళ (range)
		clear_tv(&var2);
	    తిరిగి FAIL;
	}
	*arg = *arg + 1;	// skip over the ']'
    }

    ఒకవేళ (evaluate)
    {
	పూర్ణాంకం res = eval_సూచీ_inner(rettv, range,
		empty1 ? ఖాళీ : &var1, empty2 ? ఖాళీ : &var2, FALSE,
		కీలకం, keylen, verbose);

	ఒకవేళ (!empty1)
	    clear_tv(&var1);
	ఒకవేళ (range)
	    clear_tv(&var2);
	తిరిగి res;
    }
    తిరిగి OK;
}

/*
 * Check if "rettv" can have an [index] or [sli:ce]
 */
    పూర్ణాంకం
check_can_సూచీ(typval_T *rettv, పూర్ణాంకం evaluate, పూర్ణాంకం verbose)
{
    ఎంపిక (rettv->v_type)
    {
	స్థితి VAR_FUNC:
	స్థితి VAR_PARTIAL:
	    ఒకవేళ (verbose)
		emsg(_(e_cannot_సూచీ_a_funcref));
	    తిరిగి FAIL;
	స్థితి VAR_FLOAT:
	    ఒకవేళ (verbose)
		emsg(_(e_using_float_as_string));
	    తిరిగి FAIL;
	స్థితి VAR_BOOL:
	స్థితి VAR_SPECIAL:
	స్థితి VAR_పని:
	స్థితి VAR_CHANNEL:
	స్థితి VAR_INSTR:
	స్థితి VAR_OBJECT:
	    ఒకవేళ (verbose)
		emsg(_(e_cannot_సూచీ_special_variable));
	    తిరిగి FAIL;
	స్థితి VAR_CLASS:
	స్థితి VAR_TYPEALIAS:
	    ఒకవేళ (verbose)
		check_typval_is_విలువ(rettv);
	    తిరిగి FAIL;
	స్థితి VAR_UNKNOWN:
	స్థితి VAR_ANY:
	స్థితి VAR_VOID:
	    ఒకవేళ (evaluate)
	    {
		emsg(_(e_cannot_సూచీ_special_variable));
		తిరిగి FAIL;
	    }
	    // FALLTHROUGH

	స్థితి VAR_STRING:
	స్థితి VAR_LIST:
	స్థితి VAR_TUPLE:
	స్థితి VAR_DICT:
	స్థితి VAR_BLOB:
	    ఆపు;
	స్థితి VAR_సంఖ్య:
	    ఒకవేళ (లోపల_vim9script())
		emsg(_(e_cannot_సూచీ_సంఖ్య));
	    ఆపు;
    }
    తిరిగి OK;
}

/*
 * Apply index or range to "rettv".
 * "var1" is the first index, NULL for [:expr].
 * "var2" is the second index, NULL for [expr] and [expr: ]
 * "exclusive" is TRUE for slice(): second index is exclusive, use character
 * index for string.
 * Alternatively, "key" is not NULL, then key[keylen] is the dict index.
 */
    పూర్ణాంకం
eval_సూచీ_inner(
	typval_T    *rettv,
	పూర్ణాంకం	    is_range,
	typval_T    *var1,
	typval_T    *var2,
	పూర్ణాంకం	    exclusive,
	char_u	    *కీలకం,
	పూర్ణాంకం	    keylen,
	పూర్ణాంకం	    verbose)
{
    varnumber_T	    n1, n2 = 0;
    దీర్ఘ	    len;

    n1 = 0;
    ఒకవేళ (var1 != ఖాళీ && rettv->v_type != VAR_DICT)
	n1 = tv_get_సంఖ్య(var1);

    ఒకవేళ (is_range)
    {
	ఒకవేళ (rettv->v_type == VAR_DICT)
	{
	    ఒకవేళ (verbose)
		emsg(_(e_cannot_ముక్క_dictionary));
	    తిరిగి FAIL;
	}
	ఒకవేళ (var2 != ఖాళీ)
	    n2 = tv_get_సంఖ్య(var2);
	లేకపోతే
	    n2 = VARNUM_MAX;
    }

    ఎంపిక (rettv->v_type)
    {
	స్థితి VAR_UNKNOWN:
	స్థితి VAR_ANY:
	స్థితి VAR_VOID:
	స్థితి VAR_FUNC:
	స్థితి VAR_PARTIAL:
	స్థితి VAR_FLOAT:
	స్థితి VAR_BOOL:
	స్థితి VAR_SPECIAL:
	స్థితి VAR_పని:
	స్థితి VAR_CHANNEL:
	స్థితి VAR_INSTR:
	స్థితి VAR_CLASS:
	స్థితి VAR_OBJECT:
	స్థితి VAR_TYPEALIAS:
	    ఆపు; // not evaluating, skipping over subscript

	స్థితి VAR_సంఖ్య:
	స్థితి VAR_STRING:
	    {
		char_u	*s = tv_get_string(rettv);

		len = (దీర్ఘ)STRLEN(s);
		ఒకవేళ (లోపల_vim9script() || exclusive)
		{
		    ఒకవేళ (is_range)
			s = string_ముక్క(s, n1, n2, exclusive);
		    లేకపోతే
			s = char_from_string(s, n1);
		}
		లేకపోతే ఒకవేళ (is_range)
		{
		    // The resulting variable is a substring.  If the indexes
		    // are out of range the result is empty.
		    ఒకవేళ (n1 < 0)
		    {
			n1 = len + n1;
			ఒకవేళ (n1 < 0)
			    n1 = 0;
		    }
		    ఒకవేళ (n2 < 0)
			n2 = len + n2;
		    లేకపోతే ఒకవేళ (n2 >= len)
			n2 = len;
		    ఒకవేళ (n1 >= len || n2 < 0 || n1 > n2)
			s = ఖాళీ;
		    లేకపోతే
			s = vim_strnsave(s + n1, n2 - n1 + 1);
		}
		లేకపోతే
		{
		    // The resulting variable is a string of a single
		    // character.  If the index is too big or negative the
		    // result is empty.
		    ఒకవేళ (n1 >= len || n1 < 0)
			s = ఖాళీ;
		    లేకపోతే
			s = vim_strnsave(s + n1, 1);
		}
		clear_tv(rettv);
		rettv->v_type = VAR_STRING;
		rettv->vval.v_string = s;
	    }
	    ఆపు;

	స్థితి VAR_BLOB:
	    blob_ముక్క_or_సూచీ(rettv->vval.v_blob, is_range, n1, n2,
							     exclusive, rettv);
	    ఆపు;

	స్థితి VAR_LIST:
	    ఒకవేళ (var1 == ఖాళీ)
		n1 = 0;
	    ఒకవేళ (var2 == ఖాళీ)
		n2 = VARNUM_MAX;
	    ఒకవేళ (list_ముక్క_or_సూచీ(rettv->vval.v_list,
			  is_range, n1, n2, exclusive, rettv, verbose) == FAIL)
		తిరిగి FAIL;
	    ఆపు;

	స్థితి VAR_TUPLE:
	    ఒకవేళ (var1 == ఖాళీ)
		n1 = 0;
	    ఒకవేళ (var2 == ఖాళీ)
		n2 = VARNUM_MAX;
	    ఒకవేళ (tuple_ముక్క_or_సూచీ(rettv->vval.v_tuple,
			  is_range, n1, n2, exclusive, rettv, verbose) == FAIL)
		తిరిగి FAIL;
	    ఆపు;

	స్థితి VAR_DICT:
	    {
		dictitem_T	*అంశం;
		typval_T	tmp;

		ఒకవేళ (కీలకం == ఖాళీ)
		{
		    కీలకం = tv_get_string_chk(var1);
		    ఒకవేళ (కీలకం == ఖాళీ)
			తిరిగి FAIL;
		}

		అంశం = dict_కనుగొను(rettv->vval.v_dict, కీలకం, keylen);

		ఒకవేళ (అంశం == ఖాళీ)
		{
		    ఒకవేళ (verbose)
		    {
			ఒకవేళ (keylen > 0)
			    కీలకం[keylen] = NUL;
			semsg(_(e_కీలకం_not_present_లోపల_dictionary_str), కీలకం);
		    }
		    తిరిగి FAIL;
		}

		copy_tv(&అంశం->di_tv, &tmp);
		clear_tv(rettv);
		*rettv = tmp;
	    }
	    ఆపు;
    }
    తిరిగి OK;
}

/*
 * Return the function name of partial "pt".
 */
    char_u *
partial_name(partial_T *pt)
{
    ఒకవేళ (pt != ఖాళీ)
    {
	ఒకవేళ (pt->pt_name != ఖాళీ)
	    తిరిగి pt->pt_name;
	ఒకవేళ (pt->pt_func != ఖాళీ)
	    తిరిగి pt->pt_func->uf_name;
    }
    తిరిగి (char_u *)"";
}

    స్థిర శూన్య
partial_free(partial_T *pt)
{
    పూర్ణాంకం i;

    కొరకు (i = 0; i < pt->pt_argc; ++i)
	clear_tv(&pt->pt_argv[i]);
    vim_free(pt->pt_argv);
    dict_unref(pt->pt_dict);
    ఒకవేళ (pt->pt_name != ఖాళీ)
    {
	func_unref(pt->pt_name);
	vim_free(pt->pt_name);
    }
    లేకపోతే
	func_ptr_unref(pt->pt_func);
    ఒకవేళ (pt->pt_obj != ఖాళీ)
	object_unref(pt->pt_obj);

    // "out_up" is no longer used, decrement refcount on partial that owns it.
    partial_unref(pt->pt_outer.బహిర్గతం_పైకి_partial);

    // Using pt_outer from another partial.
    partial_unref(pt->pt_outer_partial);

    // Decrease the reference count for the context of a closure.  If down
    // to the minimum it may be time to free it.
    ఒకవేళ (pt->pt_funcstack != ఖాళీ)
    {
	--pt->pt_funcstack->fs_refcount;
	funcstack_check_refcount(pt->pt_funcstack);
    }
    // Similarly for loop variables.
    కొరకు (i = 0; i < MAX_LOOP_లోతు; ++i)
	ఒకవేళ (pt->pt_loopvars[i] != ఖాళీ)
	{
	    --pt->pt_loopvars[i]->lvs_refcount;
	    loopvars_check_refcount(pt->pt_loopvars[i]);
	}

    vim_free(pt);
}

/*
 * Unreference a closure: decrement the reference count and free it when it
 * becomes zero.
 */
    శూన్య
partial_unref(partial_T *pt)
{
    ఒకవేళ (pt == ఖాళీ)
	తిరిగి;

    పూర్ణాంకం	పూర్తి = FALSE;

    ఒకవేళ (--pt->pt_refcount <= 0)
	partial_free(pt);

    // If the reference count goes down to one, the funcstack may be the
    // only reference and can be freed if no other partials reference it.
    లేకపోతే ఒకవేళ (pt->pt_refcount == 1)
    {
	// careful: if the funcstack is freed it may contain this partial
	// and it gets freed as well
	ఒకవేళ (pt->pt_funcstack != ఖాళీ)
	    పూర్తి = funcstack_check_refcount(pt->pt_funcstack);

	ఒకవేళ (!పూర్తి)
	{
	    పూర్ణాంకం	లోతు;

	    కొరకు (లోతు = 0; లోతు < MAX_LOOP_లోతు; ++లోతు)
		ఒకవేళ (pt->pt_loopvars[లోతు] != ఖాళీ
			&& loopvars_check_refcount(pt->pt_loopvars[లోతు]))
		    ఆపు;
	}
    }
}

/*
 * Return a textual representation of a string in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When both "echo_style" and "composite_val" are FALSE, put quotes around
 * strings as "string()", otherwise does not put quotes around strings.
 * May return NULL.
 */
    స్థిర char_u *
string_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    పూర్ణాంకం		echo_style,
    పూర్ణాంకం		సంయుక్త_val)
{
    char_u	*r = ఖాళీ;

    ఒకవేళ (echo_style && !సంయుక్త_val)
    {
	*tofree = ఖాళీ;
	r = tv->vval.v_string;
	ఒకవేళ (r == ఖాళీ)
	    r = (char_u *)"";
    }
    లేకపోతే
    {
	*tofree = string_quote(tv->vval.v_string, FALSE);
	r = *tofree;
    }

    తిరిగి r;
}

/*
 * Return a textual representation of a function in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "echo_style" is FALSE, put quotes around the function name as
 * "function()", otherwise does not put quotes around function name.
 * May return NULL.
 */
    స్థిర char_u *
func_tv2string(typval_T *tv, char_u **tofree, పూర్ణాంకం echo_style)
{
    char_u	*r = ఖాళీ;
    char_u	buf[MAX_FUNC_NAME_LEN];

    ఒకవేళ (echo_style)
    {
	*tofree = ఖాళీ;

	ఒకవేళ (tv->vval.v_string == ఖాళీ)
	    r = (char_u *)"function()";
	లేకపోతే
	{
	    r = make_ufunc_name_readable(tv->vval.v_string, buf,
							MAX_FUNC_NAME_LEN);
	    ఒకవేళ (r == buf)
		r = *tofree = vim_strsave(buf);
	}
    }
    లేకపోతే
    {
	char_u *s = ఖాళీ;

	ఒకవేళ (tv->vval.v_string != ఖాళీ)
	    s = make_ufunc_name_readable(tv->vval.v_string, buf,
							MAX_FUNC_NAME_LEN);

	r = *tofree = string_quote(s, TRUE);
    }

    తిరిగి r;
}

/*
 * Return a textual representation of the object method in "tv", a VAR_PARTIAL.
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "echo_style" is FALSE, put quotes around the function name as
 * "function()", otherwise does not put quotes around function name.
 * May return NULL.
 */
    స్థిర char_u *
పద్ధతి_tv2string(typval_T *tv, char_u **tofree, పూర్ణాంకం echo_style)
{
    char_u	buf[MAX_FUNC_NAME_LEN];
    partial_T	*pt = tv->vval.v_partial;

    size_t len = vim_snprintf((అక్షరం *)buf, పరిమాణం(buf), "<SNR>%d_%s.%s",
			   pt->pt_func->uf_script_ctx.sc_sid,
			   pt->pt_func->uf_class->class_name.string,
			   pt->pt_func->uf_name);
    ఒకవేళ (len >= పరిమాణం(buf))
    {
	ఒకవేళ (echo_style)
	{
	    *tofree = ఖాళీ;
	    తిరిగి (char_u *)"function()";
	}
	లేకపోతే
	    తిరిగి *tofree = string_quote((char_u*)"", TRUE);
    }

    తిరిగి *tofree = echo_style ? vim_strsave(buf) : string_quote(buf, TRUE);
}

/*
 * Return a textual representation of a partial in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * "numbuf" is used for a number.  May return NULL.
 */
    స్థిర char_u *
partial_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    char_u	*numbuf,
    పూర్ణాంకం		copyID)
{
    char_u	*r = ఖాళీ;
    partial_T	*pt;
    char_u	*fname;
    garray_T	ga;
    పూర్ణాంకం		i;
    char_u	*tf;

    pt = tv->vval.v_partial;
    fname = string_quote(pt == ఖాళీ ? ఖాళీ : partial_name(pt), FALSE);

    ga_init2(&ga, 1, 100);
    GA_CONCAT_LITERAL(&ga, "function(");
    ఒకవేళ (fname != ఖాళీ)
    {
	// When using uf_name prepend "g:" for a global function.
	ఒకవేళ (pt != ఖాళీ && pt->pt_name == ఖాళీ && fname[0] == '\''
						&& vim_isupper(fname[1]))
	{
	    GA_CONCAT_LITERAL(&ga, "'g:");
	    ga_concat(&ga, fname + 1);
	}
	లేకపోతే
	    ga_concat(&ga, fname);
	vim_free(fname);
    }
    ఒకవేళ (pt != ఖాళీ && pt->pt_argc > 0)
    {
	GA_CONCAT_LITERAL(&ga, ", [");
	కొరకు (i = 0; i < pt->pt_argc; ++i)
	{
	    ఒకవేళ (i > 0)
		GA_CONCAT_LITERAL(&ga, ", ");
	    ga_concat(&ga, tv2string(&pt->pt_argv[i], &tf, numbuf, copyID));
	    vim_free(tf);
	}
	GA_CONCAT_LITERAL(&ga, "]");
    }
    ఒకవేళ (pt != ఖాళీ && pt->pt_dict != ఖాళీ)
    {
	typval_T dtv;

	GA_CONCAT_LITERAL(&ga, ", ");
	dtv.v_type = VAR_DICT;
	dtv.vval.v_dict = pt->pt_dict;
	ga_concat(&ga, tv2string(&dtv, &tf, numbuf, copyID));
	vim_free(tf);
    }
    // terminate with ')' and a NUL
    GA_CONCAT_LITERAL(&ga, ")");
    ga_append(&ga, NUL);

    *tofree = ga.ga_దత్తాంశం;
    r = *tofree;

    తిరిగి r;
}

/*
 * Return a textual representation of a List in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "copyID" is not zero replace recursive lists with "...".  When
 * "restore_copyID" is FALSE, repeated items in lists are replaced with "...".
 * May return NULL.
 */
    స్థిర char_u *
list_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    పూర్ణాంకం		copyID,
    పూర్ణాంకం		restore_copyID)
{
    char_u	*r = ఖాళీ;

    ఒకవేళ (tv->vval.v_list == ఖాళీ)
    {
	// NULL list is equivalent to empty list.
	*tofree = ఖాళీ;
	r = (char_u *)"[]";
    }
    లేకపోతే ఒకవేళ (copyID != 0 && tv->vval.v_list->lv_copyID == copyID
	    && tv->vval.v_list->lv_len > 0)
    {
	*tofree = ఖాళీ;
	r = (char_u *)"[...]";
    }
    లేకపోతే
    {
	పూర్ణాంకం old_copyID;
	ఒకవేళ (restore_copyID)
	    old_copyID = tv->vval.v_list->lv_copyID;

	tv->vval.v_list->lv_copyID = copyID;
	*tofree = list2string(tv, copyID, restore_copyID);
	ఒకవేళ (restore_copyID)
	    tv->vval.v_list->lv_copyID = old_copyID;
	r = *tofree;
    }

    తిరిగి r;
}

/*
 * Return a textual representation of a Tuple in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "copyID" is not zero replace recursive lists with "...".  When
 * "restore_copyID" is FALSE, repeated items in tuples are replaced with "...".
 * May return NULL.
 */
    స్థిర char_u *
tuple_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    పూర్ణాంకం		copyID,
    పూర్ణాంకం		restore_copyID)
{
    tuple_T	*tuple = tv->vval.v_tuple;
    char_u	*r = ఖాళీ;

    ఒకవేళ (tuple == ఖాళీ)
    {
	// NULL tuple is equivalent to an empty tuple.
	*tofree = ఖాళీ;
	r = (char_u *)"()";
    }
    లేకపోతే ఒకవేళ (copyID != 0 && tuple->tv_copyID == copyID
					&& tuple->tv_వస్తువులు.ga_len > 0)
    {
	*tofree = ఖాళీ;
	r = (char_u *)"(...)";
    }
    లేకపోతే
    {
	పూర్ణాంకం old_copyID;
	ఒకవేళ (restore_copyID)
	    old_copyID = tuple->tv_copyID;

	tuple->tv_copyID = copyID;
	*tofree = tuple2string(tv, copyID, restore_copyID);
	ఒకవేళ (restore_copyID)
	    tuple->tv_copyID = old_copyID;
	r = *tofree;
    }

    తిరిగి r;
}

/*
 * Return a textual representation of a Dict in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "copyID" is not zero replace recursive dicts with "...".
 * When "restore_copyID" is FALSE, repeated items in the dictionary are
 * replaced with "...".  May return NULL.
 */
    స్థిర char_u *
dict_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    పూర్ణాంకం		copyID,
    పూర్ణాంకం		restore_copyID)
{
    char_u	*r = ఖాళీ;

    ఒకవేళ (tv->vval.v_dict == ఖాళీ)
    {
	// NULL dict is equivalent to empty dict.
	*tofree = ఖాళీ;
	r = (char_u *)"{}";
    }
    లేకపోతే ఒకవేళ (copyID != 0 && tv->vval.v_dict->dv_copyID == copyID
	    && tv->vval.v_dict->dv_hashtab.ht_వాడిన != 0)
    {
	*tofree = ఖాళీ;
	r = (char_u *)"{...}";
    }
    లేకపోతే
    {
	పూర్ణాంకం old_copyID;
	ఒకవేళ (restore_copyID)
	    old_copyID = tv->vval.v_dict->dv_copyID;

	tv->vval.v_dict->dv_copyID = copyID;
	*tofree = dict2string(tv, copyID, restore_copyID);
	ఒకవేళ (restore_copyID)
	    tv->vval.v_dict->dv_copyID = old_copyID;
	r = *tofree;
    }

    తిరిగి r;
}

/*
 * Return a textual representation of a job or a channel in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * "numbuf" is used for a number.
 * When "composite_val" is FALSE, put quotes around strings as "string()",
 * otherwise does not put quotes around strings.
 * May return NULL.
 */
    స్థిర char_u *
jobchan_tv2string(
    typval_T	*tv వాడని,
    char_u	**tofree వాడని,
    char_u	*numbuf వాడని,
    పూర్ణాంకం		సంయుక్త_val వాడని)
{
    char_u	*r = ఖాళీ;

#నిర్వచితమైతే FEAT_పని_CHANNEL
    *tofree = ఖాళీ;

    ఒకవేళ (tv->v_type == VAR_పని)
	r = పని_గమ్యం_string_buf(tv, numbuf);
    లేకపోతే
	r = channel_గమ్యం_string_buf(tv, numbuf);

    ఒకవేళ (సంయుక్త_val)
    {
	*tofree = string_quote(r, FALSE);
	r = *tofree;
    }
#అయితే_ముగింపు

    తిరిగి r;
}

/*
 * Return a textual representation of a class in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * May return NULL.
 */
    స్థిర char_u *
class_tv2string(typval_T *tv, char_u **tofree)
{
    char_u	*r = ఖాళీ;
    size_t	rsize;
    class_T	*cl = tv->vval.v_class;
    string_T	class_name = {(char_u *)"[unknown]", 9};
    string_T	s = {(char_u *)"class", 5};

    ఒకవేళ (cl != ఖాళీ)
    {
	class_name.string = cl->class_name.string;
	class_name.నిడివి = cl->class_name.నిడివి;
	ఒకవేళ (IS_INTERFACE(cl))
	    STR_LITERAL_సమితి(s, "interface");
	లేకపోతే ఒకవేళ (IS_ENUM(cl))
	    STR_LITERAL_సమితి(s, "enum");
    }

    rsize = s.నిడివి + 1 + class_name.నిడివి + 1;
    r = *tofree = alloc(rsize);
    ఒకవేళ (r != ఖాళీ)
	vim_snprintf((అక్షరం *)r, rsize, "%s %s", s.string, (అక్షరం *)class_name.string);

    తిరిగి r;
}

/*
 * Return a textual representation of an Object in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "copyID" is not zero replace recursive object with "...".
 * When "restore_copyID" is FALSE, repeated items in the object are
 * replaced with "...".  May return NULL.
 */
    స్థిర char_u *
object_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    పూర్ణాంకం		copyID,
    పూర్ణాంకం		restore_copyID,
    char_u	*numbuf,
    పూర్ణాంకం		echo_style,
    పూర్ణాంకం		సంయుక్త_val)
{
    char_u	*r = ఖాళీ;

    object_T	*obj = tv->vval.v_object;
    ఒకవేళ (obj == ఖాళీ || obj->obj_class == ఖాళీ)
    {
	*tofree = ఖాళీ;
	r = (char_u *)"object of [unknown]";
    }
    లేకపోతే ఒకవేళ (copyID != 0 && obj->obj_copyID == copyID
	    && obj->obj_class->class_obj_member_లెక్క != 0)
    {
	size_t n = 25 + obj->obj_class->class_name.నిడివి;
	r = alloc(n);
	ఒకవేళ (r != ఖాళీ)
	    (శూన్య)vim_snprintf((అక్షరం *)r, n, "object of %s {...}",
						obj->obj_class->class_name.string);
	*tofree = r;
    }
    లేకపోతే
    {
	పూర్ణాంకం old_copyID;
	ఒకవేళ (restore_copyID)
	    old_copyID = obj->obj_copyID;

	obj->obj_copyID = copyID;
	*tofree = object2string(obj, numbuf, copyID, echo_style,
				restore_copyID, సంయుక్త_val);
	ఒకవేళ (restore_copyID)
	    obj->obj_copyID = old_copyID;
	r = *tofree;
    }

    తిరిగి r;
}

/*
 * Return a string with the string representation of a variable.
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * "numbuf" is used for a number.
 * When "copyID" is not zero replace recursive lists and dicts with "...".
 * When both "echo_style" and "composite_val" are FALSE, put quotes around
 * strings as "string()", otherwise does not put quotes around strings, as
 * ":echo" displays values.
 * When "restore_copyID" is FALSE, repeated items in dictionaries and lists
 * are replaced with "...".
 * May return NULL.
 */
    char_u *
echo_string_core(
    typval_T	*tv,
    char_u	**tofree,
    char_u	*numbuf,
    పూర్ణాంకం		copyID,
    పూర్ణాంకం		echo_style,
    పూర్ణాంకం		restore_copyID,
    పూర్ణాంకం		సంయుక్త_val)
{
    స్థిర పూర్ణాంకం	recurse = 0;
    char_u	*r = ఖాళీ;

    ఒకవేళ (recurse >= DICT_MAXNEST)
    {
	ఒకవేళ (!did_echo_string_emsg)
	{
	    // Only give this message once for a recursive call to avoid
	    // flooding the user with errors.  And stop iterating over lists
	    // and dicts and objects.
	    did_echo_string_emsg = TRUE;
	    emsg(_(e_variable_nested_too_deep_for_displaying));
	}
	*tofree = ఖాళీ;
	తిరిగి (char_u *)"{E724}";
    }
    ++recurse;

    ఎంపిక (tv->v_type)
    {
	స్థితి VAR_STRING:
	    r = string_tv2string(tv, tofree, echo_style, సంయుక్త_val);
	    ఆపు;

	స్థితి VAR_FUNC:
	    r = func_tv2string(tv, tofree, echo_style);
	    ఆపు;

	స్థితి VAR_PARTIAL:
	    ఒకవేళ (tv->vval.v_partial == ఖాళీ
		    || tv->vval.v_partial->pt_obj == ఖాళీ)
		r = partial_tv2string(tv, tofree, numbuf, copyID);
	    లేకపోతే
		r = పద్ధతి_tv2string(tv, tofree, echo_style);
	    ఆపు;

	స్థితి VAR_BLOB:
	    r = blob2string(tv->vval.v_blob, tofree, numbuf);
	    ఆపు;

	స్థితి VAR_LIST:
	    r = list_tv2string(tv, tofree, copyID, restore_copyID);
	    ఆపు;

	స్థితి VAR_TUPLE:
	    r = tuple_tv2string(tv, tofree, copyID, restore_copyID);
	    ఆపు;

	స్థితి VAR_DICT:
	    r = dict_tv2string(tv, tofree, copyID, restore_copyID);
	    ఆపు;

	స్థితి VAR_సంఖ్య:
	స్థితి VAR_UNKNOWN:
	స్థితి VAR_ANY:
	స్థితి VAR_VOID:
	    *tofree = ఖాళీ;
	    r = tv_get_string_buf(tv, numbuf);
	    ఆపు;

	స్థితి VAR_పని:
	స్థితి VAR_CHANNEL:
	    r = jobchan_tv2string(tv, tofree, numbuf, సంయుక్త_val);
	    ఆపు;

	స్థితి VAR_INSTR:
	    *tofree = ఖాళీ;
	    r = (char_u *)"instructions";
	    ఆపు;

	స్థితి VAR_CLASS:
	    r = class_tv2string(tv, tofree);
	    ఆపు;

	స్థితి VAR_OBJECT:
	    r = object_tv2string(tv, tofree, copyID, restore_copyID,
					 numbuf, echo_style, సంయుక్త_val);
	    ఆపు;

	స్థితి VAR_FLOAT:
	    *tofree = ఖాళీ;
	    vim_snprintf((అక్షరం *)numbuf, NUMBUFLEN, "%g", tv->vval.v_float);
	    r = numbuf;
	    ఆపు;

	స్థితి VAR_BOOL:
	స్థితి VAR_SPECIAL:
	    *tofree = ఖాళీ;
	    r = (char_u *)get_var_special_name(tv->vval.v_సంఖ్య);
	    ఆపు;

	స్థితి VAR_TYPEALIAS:
	    *tofree = vim_strsave(tv->vval.v_typealias->ta_name);
	    r = *tofree;
	    ఒకవేళ (r == ఖాళీ)
		r = (char_u *)"";
	    ఆపు;
    }

    ఒకవేళ (--recurse == 0)
	did_echo_string_emsg = FALSE;
    తిరిగి r;
}

/*
 * Return a string with the string representation of a variable.
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * "numbuf" is used for a number.
 * Does not put quotes around strings, as ":echo" displays values.
 * When "copyID" is not zero replace recursive lists and dicts with "...".
 * May return NULL.
 */
    char_u *
echo_string(
    typval_T	*tv,
    char_u	**tofree,
    char_u	*numbuf,
    పూర్ణాంకం		copyID)
{
    తిరిగి echo_string_core(tv, tofree, numbuf, copyID, TRUE, FALSE, FALSE);
}

/*
 * Convert the specified byte index of line 'lnum' in buffer 'buf' to a
 * character index.  Works only for loaded buffers. Returns -1 on failure.
 * The index of the first byte and the first character is zero.
 */
    పూర్ణాంకం
buf_byteidx_గమ్యం_charidx(buf_T *buf, పూర్ణాంకం lnum, పూర్ణాంకం byteidx)
{
    char_u	*str;
    char_u	*t;
    పూర్ణాంకం		లెక్క;

    ఒకవేళ (buf == ఖాళీ || buf->b_ml.ml_mfp == ఖాళీ)
	తిరిగి -1;

    ఒకవేళ (lnum > buf->b_ml.ml_పంక్తి_లెక్క)
	lnum = buf->b_ml.ml_పంక్తి_లెక్క;

    str = ml_get_buf(buf, lnum, FALSE);
    ఒకవేళ (str == ఖాళీ)
	తిరిగి -1;

    ఒకవేళ (*str == NUL)
	తిరిగి 0;

    // count the number of characters
    t = str;
    కొరకు (లెక్క = 0; *t != NUL && t <= str + byteidx; లెక్క++)
	t += mb_ptr2len(t);

    // In insert mode, when the cursor is at the end of a non-empty line,
    // byteidx points to the NUL character immediately past the end of the
    // string. In this case, add one to the character count.
    ఒకవేళ (*t == NUL && byteidx != 0 && t == str + byteidx)
	లెక్క++;

    తిరిగి లెక్క - 1;
}

/*
 * Convert the specified character index of line 'lnum' in buffer 'buf' to a
 * byte index.  Works only for loaded buffers. Returns -1 on failure.
 * The index of the first byte and the first character is zero.
 */
    పూర్ణాంకం
buf_charidx_గమ్యం_byteidx(buf_T *buf, పూర్ణాంకం lnum, పూర్ణాంకం charidx)
{
    char_u	*str;
    char_u	*t;

    ఒకవేళ (buf == ఖాళీ || buf->b_ml.ml_mfp == ఖాళీ)
	తిరిగి -1;

    ఒకవేళ (lnum > buf->b_ml.ml_పంక్తి_లెక్క)
	lnum = buf->b_ml.ml_పంక్తి_లెక్క;

    str = ml_get_buf(buf, lnum, FALSE);
    ఒకవేళ (str == ఖాళీ)
	తిరిగి -1;

    // Convert the character offset to a byte offset
    t = str;
    అయితే (*t != NUL && --charidx > 0)
	t += mb_ptr2len(t);

    తిరిగి t - str;
}

/*
 * Translate a String variable into a position.
 * Returns NULL when there is an error.
 */
    pos_T *
var2fpos(
    typval_T	*varp,
    పూర్ణాంకం		dollar_lnum,	// TRUE when $ is last line
    పూర్ణాంకం		*fnum,		// set to fnum for '0, 'A, etc.
    పూర్ణాంకం		charcol)	// return character column
{
    char_u		*name;
    స్థిర pos_T	pos;
    pos_T		*pp;
    పూర్ణాంకం			error = FALSE;

    // Argument can be [lnum, col, coladd].
    ఒకవేళ (varp->v_type == VAR_LIST)
    {
	list_T		*l;
	పూర్ణాంకం		len;
	listitem_T	*li;

	l = varp->vval.v_list;
	ఒకవేళ (l == ఖాళీ)
	    తిరిగి ఖాళీ;

	// Get the line number
	pos.lnum = list_కనుగొను_nr(l, 0L, &error);
	ఒకవేళ (error || pos.lnum <= 0 || pos.lnum > curbuf->b_ml.ml_పంక్తి_లెక్క)
	    తిరిగి ఖాళీ;	// invalid line number
	ఒకవేళ (charcol)
	    len = (దీర్ఘ)mb_charlen(ml_get(pos.lnum));
	లేకపోతే
	    len = (దీర్ఘ)ml_get_len(pos.lnum);

	// Get the column number
	// We accept "$" for the column number: last column.
	li = list_కనుగొను(l, 1L);
	ఒకవేళ (li != ఖాళీ && li->li_tv.v_type == VAR_STRING
		&& li->li_tv.vval.v_string != ఖాళీ
		&& STRCMP(li->li_tv.vval.v_string, "$") == 0)
	{
	    pos.నిలువు = len + 1;
	}
	లేకపోతే
	{
	    pos.నిలువు = list_కనుగొను_nr(l, 1L, &error);
	    ఒకవేళ (error)
		తిరిగి ఖాళీ;
	}

	// Accept a position up to the NUL after the line.
	ఒకవేళ (pos.నిలువు == 0 || (పూర్ణాంకం)pos.నిలువు > len + 1)
	    తిరిగి ఖాళీ;	// invalid column number
	--pos.నిలువు;

	// Get the virtual offset.  Defaults to zero.
	pos.coladd = list_కనుగొను_nr(l, 2L, &error);
	ఒకవేళ (error)
	    pos.coladd = 0;

	తిరిగి &pos;
    }

    ఒకవేళ (లోపల_vim9script() && check_for_string_arg(varp, 0) == FAIL)
	తిరిగి ఖాళీ;

    name = tv_get_string_chk(varp);
    ఒకవేళ (name == ఖాళీ)
	తిరిగి ఖాళీ;

    error = TRUE;
    pos.lnum = 0;
    ఒకవేళ (name[0] == '.')
    {
	ఒకవేళ (!లోపల_vim9script() || name[1] == NUL)
	{
	    error = FALSE;
	    // cursor
	    pos = curwin->w_cursor;
	}
    }
    లేకపోతే ఒకవేళ (name[0] == 'v' && name[1] == NUL)
    {
	// Visual start
	ఒకవేళ (VIsual_చురుకైన)
	    pos = VIsual;
	లేకపోతే
	    pos = curwin->w_cursor;
    }
    లేకపోతే ఒకవేళ (name[0] == '\'')
    {
	ఒకవేళ (!లోపల_vim9script() || (name[1] != NUL && name[2] == NUL))
	{
	    error = FALSE;
	    // mark
	    pp = getmark_buf_fnum(curbuf, name[1], FALSE, fnum);
	    ఒకవేళ (pp == ఖాళీ || pp == (pos_T *)-1 || pp->lnum <= 0)
		తిరిగి ఖాళీ;
	    pos = *pp;
	}
    }
    ఒకవేళ (pos.lnum != 0)
    {
	ఒకవేళ (charcol)
	    pos.నిలువు = buf_byteidx_గమ్యం_charidx(curbuf, pos.lnum, pos.నిలువు);
	తిరిగి &pos;
    }

    pos.coladd = 0;

    ఒకవేళ (name[0] == 'w' && dollar_lnum)
    {
	// the "w_valid" flags are not reset when moving the cursor, but they
	// do matter for update_topline() and validate_botline().
	check_cursor_moved(curwin);

	pos.నిలువు = 0;
	ఒకవేళ (name[1] == '0')		// "w0": first visible line
	{
#నిర్వచితమైతే FEAT_TERMINAL
	    ఒకవేళ (bt_terminal(curwin->w_బఫరు)
				    && curwin->w_బఫరు->b_పదం != ఖాళీ
				    && !పదం_లోపల_సాధారణ_బహుళకం(curwin->w_బఫరు))
		may_move_terminal_గమ్యం_బఫరు(curwin->w_బఫరు->b_పదం, TRUE);
#అయితే_ముగింపు
	    update_topline();
	    // In silent Ex mode topline is zero, but that's not a valid line
	    // number; use one instead.
	    pos.lnum = curwin->w_topline > 0 ? curwin->w_topline : 1;
	    తిరిగి &pos;
	}
	లేకపోతే ఒకవేళ (name[1] == '$')	// "w$": last visible line
	{
#నిర్వచితమైతే FEAT_TERMINAL
	    ఒకవేళ (bt_terminal(curwin->w_బఫరు)
				    && curwin->w_బఫరు->b_పదం != ఖాళీ
				    && !పదం_లోపల_సాధారణ_బహుళకం(curwin->w_బఫరు))
		may_move_terminal_గమ్యం_బఫరు(curwin->w_బఫరు->b_పదం, TRUE);
#అయితే_ముగింపు
	    validate_botline();
	    // In silent Ex mode botline is zero, return zero then.
	    pos.lnum = curwin->w_botline > 0 ? curwin->w_botline - 1 : 0;
	    తిరిగి &pos;
	}
    }
    లేకపోతే ఒకవేళ (name[0] == '$')		// last column or line
    {
	ఒకవేళ (dollar_lnum)
	{
	    pos.lnum = curbuf->b_ml.ml_పంక్తి_లెక్క;
	    pos.నిలువు = 0;
	}
	లేకపోతే
	{
	    pos.lnum = curwin->w_cursor.lnum;
	    ఒకవేళ (charcol)
		pos.నిలువు = (colnr_T)mb_charlen(ml_get_curline());
	    లేకపోతే
		pos.నిలువు = ml_get_curline_len();
	}
	తిరిగి &pos;
    }
    ఒకవేళ (లోపల_vim9script() && error)
	semsg(_(e_invalid_విలువ_for_పంక్తి_సంఖ్య_str), name);
    తిరిగి ఖాళీ;
}

/*
 * Convert list in "arg" into position "posp" and optional file number "fnump".
 * When "fnump" is NULL there is no file number, only 3 items: [lnum, col, off]
 * Note that the column is passed on as-is, the caller may want to decrement
 * it to use 1 for the first column.
 * If "charcol" is TRUE use the column as the character index instead of the
 * byte index.
 * Return FAIL when conversion is not possible, doesn't check the position for
 * validity.
 */
    పూర్ణాంకం
list2fpos(
    typval_T	*arg,
    pos_T	*posp,
    పూర్ణాంకం		*fnump,
    colnr_T	*curswantp,
    పూర్ణాంకం		charcol)
{
    list_T	*l = arg->vval.v_list;
    దీర్ఘ	i = 0;
    దీర్ఘ	n;

    // List must be: [fnum, lnum, col, coladd, curswant], where "fnum" is only
    // there when "fnump" isn't NULL; "coladd" and "curswant" are optional.
    ఒకవేళ (arg->v_type != VAR_LIST
	    || l == ఖాళీ
	    || l->lv_len < (fnump == ఖాళీ ? 2 : 3)
	    || l->lv_len > (fnump == ఖాళీ ? 4 : 5))
	తిరిగి FAIL;

    ఒకవేళ (fnump != ఖాళీ)
    {
	n = list_కనుగొను_nr(l, i++, ఖాళీ);	// fnum
	ఒకవేళ (n < 0)
	    తిరిగి FAIL;
	ఒకవేళ (n == 0)
	    n = curbuf->b_fnum;		// current buffer
	*fnump = n;
    }

    n = list_కనుగొను_nr(l, i++, ఖాళీ);	// lnum
    ఒకవేళ (n < 0)
	తిరిగి FAIL;
    posp->lnum = n;

    n = list_కనుగొను_nr(l, i++, ఖాళీ);	//నిలువు
    ఒకవేళ (n < 0)
	తిరిగి FAIL;
    // If character position is specified, then convert to byte position
    // If the line number is zero use the cursor line.
    ఒకవేళ (charcol)
    {
	buf_T	*buf;

	// Get the text for the specified line in a loaded buffer
	buf = buflist_findnr(fnump == ఖాళీ ? curbuf->b_fnum : *fnump);
	ఒకవేళ (buf == ఖాళీ || buf->b_ml.ml_mfp == ఖాళీ)
	    తిరిగి FAIL;

	n = buf_charidx_గమ్యం_byteidx(buf,
		  posp->lnum == 0 ? curwin->w_cursor.lnum : posp->lnum, n) + 1;
    }
    posp->నిలువు = n;

    n = list_కనుగొను_nr(l, i, ఖాళీ);	// off
    ఒకవేళ (n < 0)
	posp->coladd = 0;
    లేకపోతే
	posp->coladd = n;

    ఒకవేళ (curswantp != ఖాళీ)
	*curswantp = list_కనుగొను_nr(l, i + 1, ఖాళీ);  // curswant

    తిరిగి OK;
}

/*
 * Get the length of an environment variable name.
 * Advance "arg" to the first character after the name.
 * Return 0 for error.
 */
    పూర్ణాంకం
get_env_len(char_u **arg)
{
    char_u	*p;
    పూర్ణాంకం		len;

    కొరకు (p = *arg; vim_isIDc(*p); ++p)
	;
    ఒకవేళ (p == *arg)	    // no name found
	తిరిగి 0;

    len = (పూర్ణాంకం)(p - *arg);
    *arg = p;
    తిరిగి len;
}

/*
 * Get the length of the name of a function or internal variable.
 * "arg" is advanced to after the name.
 * Return 0 if something is wrong.
 */
    పూర్ణాంకం
get_id_len(char_u **arg)
{
    char_u	*p;
    పూర్ణాంకం		len;

    // Find the end of the name.
    కొరకు (p = *arg; eval_isnamec(*p); ++p)
    {
	ఒకవేళ (*p == ':')
	{
	    // "s:" is start of "s:var", but "n:" is not and can be used in
	    // slice "[n:]".  Also "xx:" is not a namespace.
	    len = (పూర్ణాంకం)(p - *arg);
	    ఒకవేళ ((len == 1 && vim_strchr(NAMESPACE_CHAR, **arg) == ఖాళీ)
		    || len > 1)
		ఆపు;
	}
    }
    ఒకవేళ (p == *arg)	    // no name found
	తిరిగి 0;

    len = (పూర్ణాంకం)(p - *arg);
    *arg = p;

    తిరిగి len;
}

/*
 * Get the length of the name of a variable or function.
 * Only the name is recognized, does not handle ".key" or "[idx]".
 * "arg" is advanced to the first non-white character after the name.
 * Return -1 if curly braces expansion failed.
 * Return 0 if something else is wrong.
 * If the name contains 'magic' {}'s, expand them and return the
 * expanded name in an allocated string via 'alias' - caller must free.
 */
    పూర్ణాంకం
get_name_len(
    char_u	**arg,
    char_u	**alias,
    పూర్ణాంకం		evaluate,
    పూర్ణాంకం		verbose)
{
    పూర్ణాంకం		len;
    char_u	*p;
    char_u	*సమాసం_ప్రారంభం;
    char_u	*సమాసం_చివర;

    *alias = ఖాళీ;  // default to no alias

    ఒకవేళ ((*arg)[0] == K_SPECIAL && (*arg)[1] == KS_EXTRA
						  && (*arg)[2] == (పూర్ణాంకం)KE_SNR)
    {
	// hard coded <SNR>, already translated
	*arg += 3;
	తిరిగి get_id_len(arg) + 3;
    }
    len = eval_fname_script(*arg);
    ఒకవేళ (len > 0)
    {
	// literal "<SID>", "s:" or "<SNR>"
	*arg += len;
    }

    /*
     * Find the end of the name; check for {} construction.
     */
    p = కనుగొను_name_చివర(*arg, &సమాసం_ప్రారంభం, &సమాసం_చివర,
					       len > 0 ? 0 : FNE_CHECK_ప్రారంభం);
    ఒకవేళ (సమాసం_ప్రారంభం != ఖాళీ)
    {
	char_u	*తాత్కాలిక_string;

	ఒకవేళ (!evaluate)
	{
	    len += (పూర్ణాంకం)(p - *arg);
	    *arg = skipwhite(p);
	    తిరిగి len;
	}

	/*
	 * Include any <SID> etc in the expanded string:
	 * Thus the -len here.
	 */
	తాత్కాలిక_string = make_expanded_name(*arg - len, సమాసం_ప్రారంభం, సమాసం_చివర, p);
	ఒకవేళ (తాత్కాలిక_string == ఖాళీ)
	    తిరిగి -1;
	*alias = తాత్కాలిక_string;
	*arg = skipwhite(p);
	తిరిగి (పూర్ణాంకం)STRLEN(తాత్కాలిక_string);
    }

    len += get_id_len(arg);
    // Only give an error when there is something, otherwise it will be
    // reported at a higher level.
    ఒకవేళ (len == 0 && verbose && **arg != NUL)
	semsg(_(e_invalid_వ్యక్తీకరణ_str), *arg);

    తిరిగి len;
}

/*
 * Find the end of a variable or function name, taking care of magic braces.
 * If "expr_start" is not NULL then "expr_start" and "expr_end" are set to the
 * start and end of the first magic braces item.
 * "flags" can have FNE_INCL_BR and FNE_CHECK_START.
 * Return a pointer to just after the name.  Equal to "arg" if there is no
 * valid name.
 */
    char_u *
కనుగొను_name_చివర(
    char_u	*arg,
    char_u	**సమాసం_ప్రారంభం,
    char_u	**సమాసం_చివర,
    పూర్ణాంకం		flags)
{
    పూర్ణాంకం		mb_nest = 0;
    పూర్ణాంకం		br_nest = 0;
    char_u	*p;
    పూర్ణాంకం		len;
    పూర్ణాంకం		allow_curly = !లోపల_vim9script();

    ఒకవేళ (సమాసం_ప్రారంభం != ఖాళీ)
    {
	*సమాసం_ప్రారంభం = ఖాళీ;
	*సమాసం_చివర = ఖాళీ;
    }

    // Quick check for valid starting character.
    ఒకవేళ ((flags & FNE_CHECK_ప్రారంభం) && !eval_isnamec1(*arg)
					      && (*arg != '{' || !allow_curly))
	తిరిగి arg;

    కొరకు (p = arg; *p != NUL
		    && (eval_isnamec(*p)
			|| (*p == '{' && allow_curly)
			|| ((flags & FNE_INCL_BR) && (*p == '['
					 || (*p == '.' && eval_isdictc(p[1]))))
			|| mb_nest != 0
			|| br_nest != 0); MB_PTR_ADV(p))
    {
	ఒకవేళ (*p == '\'')
	{
	    // skip over 'string' to avoid counting [ and ] inside it.
	    కొరకు (p = p + 1; *p != NUL && *p != '\''; MB_PTR_ADV(p))
		;
	    ఒకవేళ (*p == NUL)
		ఆపు;
	}
	లేకపోతే ఒకవేళ (*p == '"')
	{
	    // skip over "str\"ing" to avoid counting [ and ] inside it.
	    కొరకు (p = p + 1; *p != NUL && *p != '"'; MB_PTR_ADV(p))
		ఒకవేళ (*p == '\\' && p[1] != NUL)
		    ++p;
	    ఒకవేళ (*p == NUL)
		ఆపు;
	}
	లేకపోతే ఒకవేళ (br_nest == 0 && mb_nest == 0 && *p == ':')
	{
	    // "s:" is start of "s:var", but "n:" is not and can be used in
	    // slice "[n:]".  Also "xx:" is not a namespace. But {ns}: is.
	    len = (పూర్ణాంకం)(p - arg);
	    ఒకవేళ ((len == 1 && vim_strchr(NAMESPACE_CHAR, *arg) == ఖాళీ)
		    || (len > 1 && p[-1] != '}'))
		ఆపు;
	}

	ఒకవేళ (mb_nest == 0)
	{
	    ఒకవేళ (*p == '[')
		++br_nest;
	    లేకపోతే ఒకవేళ (*p == ']')
		--br_nest;
	}

	ఒకవేళ (br_nest == 0 && allow_curly)
	{
	    ఒకవేళ (*p == '{')
	    {
		mb_nest++;
		ఒకవేళ (సమాసం_ప్రారంభం != ఖాళీ && *సమాసం_ప్రారంభం == ఖాళీ)
		    *సమాసం_ప్రారంభం = p;
	    }
	    లేకపోతే ఒకవేళ (*p == '}')
	    {
		mb_nest--;
		ఒకవేళ (సమాసం_ప్రారంభం != ఖాళీ && mb_nest == 0 && *సమాసం_చివర == ఖాళీ)
		    *సమాసం_చివర = p;
	    }
	}
    }

    తిరిగి p;
}

/*
 * Expands out the 'magic' {}'s in a variable/function name.
 * Note that this can call itself recursively, to deal with
 * constructs like foo{bar}{baz}{bam}
 * The four pointer arguments point to "foo{expre}ss{ion}bar"
 *			"in_start"      ^
 *			"expr_start"	   ^
 *			"expr_end"		 ^
 *			"in_end"			    ^
 *
 * Returns a new allocated string, which the caller must free.
 * Returns NULL for failure.
 */
    స్థిర char_u *
make_expanded_name(
    char_u	*లోపల_ప్రారంభం,
    char_u	*సమాసం_ప్రారంభం,
    char_u	*సమాసం_చివర,
    char_u	*లోపల_చివర)
{
    char_u	c1;
    char_u	*retval = ఖాళీ;
    char_u	*తాత్కాలిక_ఫలితం;

    ఒకవేళ (సమాసం_చివర == ఖాళీ || లోపల_చివర == ఖాళీ)
	తిరిగి ఖాళీ;
    *సమాసం_ప్రారంభం	= NUL;
    *సమాసం_చివర = NUL;
    c1 = *లోపల_చివర;
    *లోపల_చివర = NUL;

    తాత్కాలిక_ఫలితం = eval_గమ్యం_string(సమాసం_ప్రారంభం + 1, FALSE, FALSE);
    ఒకవేళ (తాత్కాలిక_ఫలితం != ఖాళీ)
    {
	size_t	retvalsize = (size_t)(సమాసం_ప్రారంభం - లోపల_ప్రారంభం)
				+ STRLEN(తాత్కాలిక_ఫలితం)
				+ (size_t)(లోపల_చివర - సమాసం_చివర) + 1;

	retval = alloc(retvalsize);
	ఒకవేళ (retval != ఖాళీ)
	    vim_snprintf((అక్షరం *)retval, retvalsize, "%s%s%s",
				లోపల_ప్రారంభం, తాత్కాలిక_ఫలితం, సమాసం_చివర + 1);
    }
    vim_free(తాత్కాలిక_ఫలితం);

    *లోపల_చివర = c1;		// put char back for error messages
    *సమాసం_ప్రారంభం = '{';
    *సమాసం_చివర = '}';

    ఒకవేళ (retval != ఖాళీ)
    {
	తాత్కాలిక_ఫలితం = కనుగొను_name_చివర(retval, &సమాసం_ప్రారంభం, &సమాసం_చివర, 0);
	ఒకవేళ (సమాసం_ప్రారంభం != ఖాళీ)
	{
	    // Further expansion!
	    తాత్కాలిక_ఫలితం = make_expanded_name(retval, సమాసం_ప్రారంభం,
						       సమాసం_చివర, తాత్కాలిక_ఫలితం);
	    vim_free(retval);
	    retval = తాత్కాలిక_ఫలితం;
	}
    }

    తిరిగి retval;
}

/*
 * Return TRUE if character "c" can be used in a variable or function name.
 * Does not include '{' or '}' for magic braces.
 */
    పూర్ణాంకం
eval_isnamec(పూర్ణాంకం c)
{
    తిరిగి ASCII_ISALNUM(c) || c == '_' || c == ':' || c == AUTOLOAD_CHAR;
}

/*
 * Return TRUE if character "c" can be used as the first character in a
 * variable or function name (excluding '{' and '}').
 */
    పూర్ణాంకం
eval_isnamec1(పూర్ణాంకం c)
{
    తిరిగి ASCII_ISALPHA(c) || c == '_';
}

/*
 * Return TRUE if character "c" can be used as the first character of a
 * dictionary key.
 */
    పూర్ణాంకం
eval_isdictc(పూర్ణాంకం c)
{
    తిరిగి ASCII_ISALNUM(c) || c == '_';
}

/*
 * Return true when "p" points to "->" followed by a method name or a lambda.
 * Only called for Vim9 script.
 */
    స్థిర bool
పద్ధతి_call_follows(char_u *p)
{
    ఒకవేళ (p[0] != '-' || p[1] != '>')
	తిరిగి false;

    char_u	*after = skipwhite(p + 2);

    తిరిగి ASCII_ISALPHA(*after) || *after == '(';
}

/*
 * Handle:
 * - expr[expr], expr[expr:expr] subscript
 * - ".name" lookup
 * - function call with Funcref variable: func(expr)
 * - method call: var->method()
 *
 * Can all be combined in any order: dict.func(expr)[idx]['func'](expr)->len()
 * "name_start" points to a variable before the subscript or is NULL.
 */
    పూర్ణాంకం
handle_subscript(
    char_u	**arg,
    char_u	*name_ప్రారంభం,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    పూర్ణాంకం		verbose)	// give error messages
{
    పూర్ణాంకం		evaluate = evalarg != ఖాళీ
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    పూర్ణాంకం		ret = OK;
    dict_T	*selfdict = ఖాళీ;
    పూర్ణాంకం		check_white = TRUE;
    పూర్ణాంకం		getnext;
    char_u	*p;

    అయితే (ret == OK)
    {
	// When at the end of the line and ".name" or a method call follows in
	// the next line then consume the line break.
	p = eval_తదుపరి_non_blank(*arg, evalarg, &getnext);
	ఒకవేళ (getnext
	    && ((*p == '.'
		    && ((rettv->v_type == VAR_DICT && eval_isdictc(p[1]))
			|| rettv->v_type == VAR_CLASS
			|| rettv->v_type == VAR_OBJECT))
		|| పద్ధతి_call_follows(p)))
	{
	    *arg = eval_తదుపరి_పంక్తి(*arg, evalarg);
	    p = *arg;
	    check_white = FALSE;
	}

	ఒకవేళ (rettv->v_type == VAR_ANY)
	{
	    char_u	*exp_name;
	    పూర్ణాంకం		cc;
	    పూర్ణాంకం		idx;
	    ufunc_T	*ufunc;
	    type_T	*type;

	    // Found script from "import {name} as name", script item name must
	    // follow.  "rettv->vval.v_number" has the script ID.
	    ఒకవేళ (**arg != '.')
	    {
		ఒకవేళ (verbose)
		    semsg(_(e_expected_dot_after_name_str),
					name_ప్రారంభం != ఖాళీ ? name_ప్రారంభం: *arg);
		ret = FAIL;
		ఆపు;
	    }
	    ++*arg;
	    ఒకవేళ (IS_WHITE_OR_NUL(**arg))
	    {
		ఒకవేళ (verbose)
		    emsg(_(e_no_white_space_allowed_after_dot));
		ret = FAIL;
		ఆపు;
	    }

	    // isolate the name
	    exp_name = *arg;
	    అయితే (eval_isnamec(**arg))
		++*arg;
	    cc = **arg;
	    **arg = NUL;

	    idx = కనుగొను_exported(rettv->vval.v_సంఖ్య, exp_name, &ufunc, &type,
		       evalarg == ఖాళీ ? ఖాళీ : evalarg->eval_cctx,
		       evalarg == ఖాళీ ? ఖాళీ : evalarg->eval_cstack, verbose);
	    **arg = cc;

	    ఒకవేళ (idx < 0 && ufunc == ఖాళీ)
	    {
		ret = FAIL;
		ఆపు;
	    }
	    ఒకవేళ (idx >= 0)
	    {
		scriptitem_T    *si = SCRIPT_అంశం(rettv->vval.v_సంఖ్య);
		svar_T		*sv = ((svar_T *)si->sn_var_vals.ga_దత్తాంశం) + idx;

		copy_tv(sv->sv_tv, rettv);
	    }
	    లేకపోతే
	    {
		rettv->v_type = VAR_FUNC;
		ఒకవేళ (**arg == '<')
		{
		    char_u *s = get_generic_func_name(ufunc, arg);
		    ఒకవేళ (s != ఖాళీ)
			rettv->vval.v_string = s;
		    లేకపోతే
			ret = FAIL;
		}
		లేకపోతే
		{
		    rettv->vval.v_string =
			vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
		}
	    }
	    కొనసాగు;
	}

	ఒకవేళ ((**arg == '(' && (!evaluate || rettv->v_type == VAR_FUNC
			    || rettv->v_type == VAR_PARTIAL))
		    && (!check_white || !VIM_ISWHITE(*(*arg - 1))))
	{
	    ret = call_func_rettv(arg, evalarg, rettv, evaluate,
							       selfdict, ఖాళీ);

	    // Stop the expression evaluation when immediately aborting on
	    // error, or when an interrupt occurred or an exception was thrown
	    // but not caught.
	    ఒకవేళ (aborting())
	    {
		ఒకవేళ (ret == OK)
		    clear_tv(rettv);
		ret = FAIL;
	    }
	    dict_unref(selfdict);
	    selfdict = ఖాళీ;
	}
	లేకపోతే ఒకవేళ (p[0] == '-' && p[1] == '>')
	{
	    ఒకవేళ (లోపల_vim9script())
		*arg = skipwhite(p + 2);
	    లేకపోతే
		*arg = p + 2;
	    ఒకవేళ (ret == OK && evaluate && rettv->v_type == VAR_VOID)
	    {
		ఒకవేళ (verbose)
		    emsg(_(e_cannot_use_void_విలువ));
		ret = FAIL;
	    }
	    లేకపోతే ఒకవేళ (VIM_ISWHITE(**arg))
	    {
		emsg(_(e_no_white_space_allowed_before_parenthesis));
		ret = FAIL;
	    }
	    లేకపోతే ఒకవేళ ((**arg == '{' && !లోపల_vim9script()) || **arg == '(')
		// expr->{lambda}() or expr->(lambda)()
		ret = eval_lambda(arg, rettv, evalarg, verbose);
	    లేకపోతే
		// expr->name()
		ret = eval_పద్ధతి(arg, rettv, evalarg, verbose);
	}
	// "." is ".name" lookup when we found a dict or when evaluating and
	// scriptversion is at least 2, where string concatenation is "..".
	లేకపోతే ఒకవేళ (**arg == '['
		|| (**arg == '.' && (rettv->v_type == VAR_DICT
			|| (!evaluate
			    && (*arg)[1] != '.'
			    && !లోపల_old_script(2)))))
	{
	    dict_unref(selfdict);
	    ఒకవేళ (rettv->v_type == VAR_DICT)
	    {
		selfdict = rettv->vval.v_dict;
		ఒకవేళ (selfdict != ఖాళీ)
		    ++selfdict->dv_refcount;
	    }
	    లేకపోతే
		selfdict = ఖాళీ;
	    ఒకవేళ (eval_సూచీ(arg, rettv, evalarg, verbose) == FAIL)
	    {
		clear_tv(rettv);
		ret = FAIL;
	    }
	}
	లేకపోతే ఒకవేళ (**arg == '.' && (rettv->v_type == VAR_CLASS
					       || rettv->v_type == VAR_OBJECT))
	{
	    // class member: SomeClass.varname
	    // class method: SomeClass.SomeMethod()
	    // class constructor: SomeClass.new()
	    // object member: someObject.varname
	    // object method: someObject.SomeMethod()
	    ఒకవేళ (class_object_సూచీ(arg, rettv, evalarg, verbose) == FAIL)
	    {
		clear_tv(rettv);
		ret = FAIL;
	    }
	}
	లేకపోతే
	    ఆపు;
    }

    // Turn "dict.Func" into a partial for "Func" bound to "dict".
    // Don't do this when "Func" is already a partial that was bound
    // explicitly (pt_auto is FALSE).
    ఒకవేళ (selfdict != ఖాళీ
	    && (rettv->v_type == VAR_FUNC
		|| (rettv->v_type == VAR_PARTIAL
		    && (rettv->vval.v_partial->pt_auto
			|| rettv->vval.v_partial->pt_dict == ఖాళీ))))
	selfdict = make_partial(selfdict, rettv);

    dict_unref(selfdict);
    తిరిగి ret;
}

/*
 * Make a copy of an item.
 * Lists and Dictionaries are also copied.  A deep copy if "deep" is set.
 * "top" is TRUE for the toplevel of copy().
 * For deepcopy() "copyID" is zero for a full copy or the ID for when a
 * reference to an already copied list/dict can be used.
 * Returns FAIL or OK.
 */
    పూర్ణాంకం
అంశం_copy(
    typval_T	*from,
    typval_T	*గమ్యం,
    పూర్ణాంకం		deep,
    పూర్ణాంకం		శిఖరం,
    పూర్ణాంకం		copyID)
{
    స్థిర పూర్ణాంకం	recurse = 0;
    పూర్ణాంకం		ret = OK;

    ఒకవేళ (recurse >= DICT_MAXNEST)
    {
	emsg(_(e_variable_nested_too_deep_for_making_copy));
	తిరిగి FAIL;
    }
    ++recurse;

    ఎంపిక (from->v_type)
    {
	స్థితి VAR_సంఖ్య:
	స్థితి VAR_FLOAT:
	స్థితి VAR_STRING:
	స్థితి VAR_FUNC:
	స్థితి VAR_PARTIAL:
	స్థితి VAR_BOOL:
	స్థితి VAR_SPECIAL:
	స్థితి VAR_పని:
	స్థితి VAR_CHANNEL:
	స్థితి VAR_INSTR:
	స్థితి VAR_CLASS:
	స్థితి VAR_OBJECT:
	స్థితి VAR_TYPEALIAS:
	    copy_tv(from, గమ్యం);
	    ఆపు;
	స్థితి VAR_LIST:
	    గమ్యం->v_type = VAR_LIST;
	    గమ్యం->v_తాళం = 0;
	    ఒకవేళ (from->vval.v_list == ఖాళీ)
		గమ్యం->vval.v_list = ఖాళీ;
	    లేకపోతే ఒకవేళ (copyID != 0 && from->vval.v_list->lv_copyID == copyID)
	    {
		// use the copy made earlier
		గమ్యం->vval.v_list = from->vval.v_list->lv_copylist;
		++గమ్యం->vval.v_list->lv_refcount;
	    }
	    లేకపోతే
	    {
		గమ్యం->vval.v_list = list_copy(from->vval.v_list,
							    deep, శిఖరం, copyID);
		ఒకవేళ (గమ్యం->vval.v_list == ఖాళీ)
		    ret = FAIL;
	    }
	    ఆపు;
	స్థితి VAR_TUPLE:
	    గమ్యం->v_type = VAR_TUPLE;
	    గమ్యం->v_తాళం = 0;
	    ఒకవేళ (from->vval.v_tuple == ఖాళీ)
		గమ్యం->vval.v_tuple = ఖాళీ;
	    లేకపోతే ఒకవేళ (copyID != 0 && from->vval.v_tuple->tv_copyID == copyID)
	    {
		// use the copy made earlier
		గమ్యం->vval.v_tuple = from->vval.v_tuple->tv_copytuple;
		++గమ్యం->vval.v_tuple->tv_refcount;
	    }
	    లేకపోతే
	    {
		గమ్యం->vval.v_tuple = tuple_copy(from->vval.v_tuple,
							    deep, శిఖరం, copyID);
		ఒకవేళ (గమ్యం->vval.v_tuple == ఖాళీ)
		    ret = FAIL;
	    }
	    ఆపు;
	స్థితి VAR_BLOB:
	    గమ్యం->v_type = VAR_BLOB;
	    గమ్యం->v_తాళం = 0;
	    ఒకవేళ (from->vval.v_blob == ఖాళీ)
		గమ్యం->vval.v_blob = ఖాళీ;
	    లేకపోతే
	    {
		గమ్యం->vval.v_blob = blob_copy(from->vval.v_blob);
		ఒకవేళ (గమ్యం->vval.v_blob == ఖాళీ)
		    ret = FAIL;
	    }
	    ఆపు;
	స్థితి VAR_DICT:
	    గమ్యం->v_type = VAR_DICT;
	    గమ్యం->v_తాళం = 0;
	    ఒకవేళ (from->vval.v_dict == ఖాళీ)
		గమ్యం->vval.v_dict = ఖాళీ;
	    లేకపోతే ఒకవేళ (copyID != 0 && from->vval.v_dict->dv_copyID == copyID)
	    {
		// use the copy made earlier
		గమ్యం->vval.v_dict = from->vval.v_dict->dv_copydict;
		++గమ్యం->vval.v_dict->dv_refcount;
	    }
	    లేకపోతే
	    {
		గమ్యం->vval.v_dict = dict_copy(from->vval.v_dict,
							    deep, శిఖరం, copyID);
		ఒకవేళ (గమ్యం->vval.v_dict == ఖాళీ)
		    ret = FAIL;
	    }
	    ఆపు;
	స్థితి VAR_UNKNOWN:
	స్థితి VAR_ANY:
	స్థితి VAR_VOID:
	    అంతర్గత_error_no_abort("item_copy(UNKNOWN)");
	    ret = FAIL;
    }
    --recurse;
    తిరిగి ret;
}

    శూన్య
echo_one(typval_T *rettv, పూర్ణాంకం with_space, పూర్ణాంకం *atstart, పూర్ణాంకం *needclr)
{
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    char_u	*p = echo_string(rettv, &tofree, numbuf, get_copyID());

    ఒకవేళ (*atstart)
    {
	*atstart = FALSE;
	// Call msg_start() after eval1(), evaluating the expression
	// may cause a message to appear.
	ఒకవేళ (with_space)
	{
	    // Mark the saved text as finishing the line, so that what
	    // follows is displayed on a new line when scrolling back
	    // at the more prompt.
	    సందేశం_sb_eol();
	    సందేశం_ప్రారంభం();
	}
    }
    లేకపోతే ఒకవేళ (with_space)
	సందేశం_puts_attr(" ", echo_attr);

    ఒకవేళ (p != ఖాళీ)
	కొరకు ( ; *p != NUL && !got_int; ++p)
	{
	    ఒకవేళ (*p == '\n' || *p == '\r' || *p == TAB)
	    {
		ఒకవేళ (*p != TAB && *needclr)
		{
		    // remove any text still there from the command
		    సందేశం_clr_eos();
		    *needclr = FALSE;
		}
		సందేశం_putchar_attr(*p, echo_attr);
	    }
	    లేకపోతే
	    {
		ఒకవేళ (has_mbyte)
		{
		    పూర్ణాంకం i = (*mb_ptr2len)(p);

		    (శూన్య)సందేశం_outtrans_len_attr(p, i, echo_attr);
		    p += i - 1;
		}
		లేకపోతే
		    (శూన్య)సందేశం_outtrans_len_attr(p, 1, echo_attr);
	    }
	}
    vim_free(tofree);
}

/*
 * ":echo expr1 ..."	print each argument separated with a space, add a
 *			newline at the end.
 * ":echon expr1 ..."	print each argument plain.
 */
    శూన్య
ex_echo(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    typval_T	rettv;
    char_u	*arg_ప్రారంభం;
    పూర్ణాంకం		needclr = TRUE;
    పూర్ణాంకం		atstart = TRUE;
    పూర్ణాంకం		did_emsg_before = did_emsg;
    పూర్ణాంకం		called_emsg_before = called_emsg;
    evalarg_T	evalarg;

    fill_evalarg_from_eap(&evalarg, eap, eap->skip);

    ఒకవేళ (eap->skip)
	++emsg_skip;
    అయితే ((!చివర్లు_excmd2(eap->cmd, arg) || *arg == '"') && !got_int)
    {
	// If eval1() causes an error message the text from the command may
	// still need to be cleared. E.g., "echo 22,44".
	need_clr_eos = needclr;

	arg_ప్రారంభం = arg;
	ఒకవేళ (eval1(&arg, &rettv, &evalarg) == FAIL)
	{
	    /*
	     * Report the invalid expression unless the expression evaluation
	     * has been cancelled due to an aborting error, an interrupt, or an
	     * exception.
	     */
	    ఒకవేళ (!aborting() && did_emsg == did_emsg_before
					  && called_emsg == called_emsg_before)
		semsg(_(e_invalid_వ్యక్తీకరణ_str), arg_ప్రారంభం);
	    need_clr_eos = FALSE;
	    ఆపు;
	}
	need_clr_eos = FALSE;

	ఒకవేళ (!eap->skip)
	{
	    ఒకవేళ (rettv.v_type == VAR_VOID)
	    {
		semsg(_(e_వ్యక్తీకరణ_does_not_ఫలితం_లోపల_విలువ_str), arg_ప్రారంభం);
		ఆపు;
	    }
	    echo_one(&rettv, eap->cmdidx == CMD_echo, &atstart, &needclr);
	}

	clear_tv(&rettv);
	arg = skipwhite(arg);
    }
    సమితి_nextcmd(eap, arg);
    clear_evalarg(&evalarg, eap);

    ఒకవేళ (eap->skip)
	--emsg_skip;
    లేకపోతే
    {
	// remove text that may still be there from the command
	ఒకవేళ (needclr)
	    సందేశం_clr_eos();
	ఒకవేళ (eap->cmdidx == CMD_echo)
	    సందేశం_చివర();
    }
}

/*
 * ":echohl {name}".
 */
    శూన్య
ex_echohl(exarg_T *eap)
{
    echo_attr = syn_name2attr(eap->arg);
}

/*
 * Returns the :echo attribute
 */
    పూర్ణాంకం
get_echo_attr(శూన్య)
{
    తిరిగి echo_attr;
}

/*
 * ":execute expr1 ..."	execute the result of an expression.
 * ":echomsg expr1 ..."	Print a message
 * ":echowindow expr1 ..." Print a message in the messages window
 * ":echoerr expr1 ..."	Print an error
 * ":echoconsole expr1 ..." Print a message on stdout
 * Each gets spaces around each argument and a newline at the end for
 * echo commands
 */
    శూన్య
ex_execute(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    typval_T	rettv;
    పూర్ణాంకం		ret = OK;
    char_u	*p;
    garray_T	ga;
    పూర్ణాంకం		len;
    దీర్ఘ	ప్రారంభం_lnum = SOURCING_LNUM;

    ga_init2(&ga, 1, 80);

    ఒకవేళ (eap->skip)
	++emsg_skip;
    అయితే (!చివర్లు_excmd2(eap->cmd, arg) || *arg == '"')
    {
	ret = eval1_emsg(&arg, &rettv, eap);
	ఒకవేళ (ret == FAIL)
	    ఆపు;

	ఒకవేళ (!eap->skip)
	{
	    char_u   buf[NUMBUFLEN];

	    ఒకవేళ (eap->cmdidx == CMD_execute)
	    {
		ఒకవేళ (rettv.v_type == VAR_CHANNEL || rettv.v_type == VAR_పని)
		{
		    semsg(_(e_using_invalid_విలువ_as_string_str),
						  vartype_name(rettv.v_type));
		    p = ఖాళీ;
		}
		లేకపోతే
		    p = tv_get_string_buf(&rettv, buf);
	    }
	    లేకపోతే
		p = tv_stringify(&rettv, buf);
	    ఒకవేళ (p == ఖాళీ)
	    {
		clear_tv(&rettv);
		ret = FAIL;
		ఆపు;
	    }
	    len = (పూర్ణాంకం)STRLEN(p);
	    ఒకవేళ (ga_grow(&ga, len + 2) == FAIL)
	    {
		clear_tv(&rettv);
		ret = FAIL;
		ఆపు;
	    }
	    ఒకవేళ (ga.ga_len)
		((char_u *)(ga.ga_దత్తాంశం))[ga.ga_len++] = ' ';
	    STRCPY((char_u *)(ga.ga_దత్తాంశం) + ga.ga_len, p);
	    ga.ga_len += len;
	}

	clear_tv(&rettv);
	arg = skipwhite(arg);
    }

    ఒకవేళ (ret != FAIL && ga.ga_దత్తాంశం != ఖాళీ)
    {
	// use the first line of continuation lines for messages
	SOURCING_LNUM = ప్రారంభం_lnum;

	ఒకవేళ (eap->cmdidx == CMD_echomsg
		|| eap->cmdidx == CMD_echowindow
		|| eap->cmdidx == CMD_echoerr)
	{
	    // Mark the already saved text as finishing the line, so that what
	    // follows is displayed on a new line when scrolling back at the
	    // more prompt.
	    సందేశం_sb_eol();
	}

	ఒకవేళ (eap->cmdidx == CMD_echomsg)
	{
	    సందేశం_attr(ga.ga_దత్తాంశం, echo_attr);
	    బహిర్గతం_flush();
	}
	లేకపోతే ఒకవేళ (eap->cmdidx == CMD_echowindow)
	{
#నిర్వచితమైతే HAS_సందేశం_కిటికీ
	    ప్రారంభం_echowindow(eap->addr_లెక్క > 0 ? eap->line2 : 0);
#అయితే_ముగింపు
	    సందేశం_attr(ga.ga_దత్తాంశం, echo_attr);
#నిర్వచితమైతే HAS_సందేశం_కిటికీ
	    చివర_echowindow();
#అయితే_ముగింపు
	}
	లేకపోతే ఒకవేళ (eap->cmdidx == CMD_echoconsole)
	{
	    ui_write(ga.ga_దత్తాంశం, (పూర్ణాంకం)STRLEN(ga.ga_దత్తాంశం), TRUE);
	    ui_write((char_u *)"\r\n", 2, TRUE);
	}
	లేకపోతే ఒకవేళ (eap->cmdidx == CMD_echoerr)
	{
	    పూర్ణాంకం		save_did_emsg = did_emsg;

	    // We don't want to abort following commands, restore did_emsg.
	    emsg(ga.ga_దత్తాంశం);
	    ఒకవేళ (!force_abort)
		did_emsg = save_did_emsg;
	}
	లేకపోతే ఒకవేళ (eap->cmdidx == CMD_execute)
	{
	    పూర్ణాంకం save_sticky_cmdmod_flags = sticky_cmdmod_flags;

	    // "legacy exe cmd" and "vim9cmd exe cmd" applies to "cmd".
	    sticky_cmdmod_flags = cmdmod.cmod_flags
						& (CMOD_LEGACY | CMOD_VIM9CMD);
	    do_cmdline((char_u *)ga.ga_దత్తాంశం,
		       eap->ea_getline, eap->cookie, DOCMD_NOWAIT|DOCMD_VERBOSE);
	    sticky_cmdmod_flags = save_sticky_cmdmod_flags;
	}
    }

    ga_clear(&ga);

    ఒకవేళ (eap->skip)
	--emsg_skip;
    సమితి_nextcmd(eap, arg);
}

/*
 * Skip over the name of an option: "&option", "&g:option" or "&l:option".
 * "arg" points to the "&" or '+' when called, to "option" when returning.
 * Returns NULL when no option name found.  Otherwise pointer to the char
 * after the option name.
 */
    char_u *
కనుగొను_option_చివర(char_u **arg, పూర్ణాంకం *scope)
{
    char_u	*p = *arg;

    ++p;
    ఒకవేళ (*p == 'g' && p[1] == ':')
    {
	*scope = OPT_GLOBAL;
	p += 2;
    }
    లేకపోతే ఒకవేళ (*p == 'l' && p[1] == ':')
    {
	*scope = OPT_LOCAL;
	p += 2;
    }
    లేకపోతే
	*scope = 0;

    ఒకవేళ (!ASCII_ISALPHA(*p))
	తిరిగి ఖాళీ;
    *arg = p;

    ఒకవేళ (p[0] == 't' && p[1] == '_' && p[2] != NUL && p[3] != NUL)
	p += 4;	    // termcap option
    లేకపోతే
	అయితే (ASCII_ISALPHA(*p))
	    ++p;
    తిరిగి p;
}

/*
 * Display script name where an item was last set.
 * Should only be invoked when 'verbose' is non-zero.
 */
    శూన్య
చివరి_సమితి_సందేశం(sctx_T script_ctx)
{
    char_u *p;

    ఒకవేళ (script_ctx.sc_sid == 0)
	తిరిగి;

    p = home_replace_save(ఖాళీ, get_scriptname(script_ctx.sc_sid));
    ఒకవేళ (p == ఖాళీ)
	తిరిగి;

    verbose_enter();
    సందేశం_puts(_("\n\tLast set from "));
    సందేశం_puts((అక్షరం *)p);
    ఒకవేళ (script_ctx.sc_lnum > 0)
    {
	సందేశం_puts(_(పంక్తి_సందేశం));
	సందేశం_outnum((దీర్ఘ)script_ctx.sc_lnum);
    }
    verbose_leave();
    vim_free(p);
}

#అయితే_ముగింపు // FEAT_EVAL

/*
 * Perform a substitution on "str" with pattern "pat" and substitute "sub".
 * When "sub" is NULL "expr" is used, must be a VAR_FUNC or VAR_PARTIAL.
 * "flags" can be "g" to do a global substitute.
 * Returns an allocated string, NULL for error.
 */
    char_u *
do_string_sub(
    char_u	*str,
    size_t	len,
    char_u	*pat,
    char_u	*sub,
    typval_T	*సమాసం,
    char_u	*flags,
    size_t	*ret_len)		// length of returned buffer
{
    regmatch_T	regmatch;
    garray_T	ga;
    char_u	*ret;
    char_u	*save_cpo;

    // Make 'cpoptions' empty, so that the 'l' flag doesn't work here
    save_cpo = p_cpo;
    p_cpo = ఖాళీలు_option;

    ga_init2(&ga, 1, 200);

    regmatch.rm_ic = p_ic;
    regmatch.regprog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
    ఒకవేళ (regmatch.regprog != ఖాళీ)
    {
	char_u	*తోక = str;
	char_u	*చివర = str + len;
	పూర్ణాంకం	do_all = (flags[0] == 'g');
	పూర్ణాంకం	sublen;
	పూర్ణాంకం	i;
	char_u	*zero_వెడల్పు = ఖాళీ;

	అయితే (vim_regexec_nl(&regmatch, str, (colnr_T)(తోక - str)))
	{
	    // Skip empty match except for first match.
	    ఒకవేళ (regmatch.startp[0] == regmatch.endp[0])
	    {
		ఒకవేళ (zero_వెడల్పు == regmatch.startp[0])
		{
		    // avoid getting stuck on a match with an empty string
		    i = mb_ptr2len(తోక);
		    mch_memmove((char_u *)ga.ga_దత్తాంశం + ga.ga_len, తోక,
								   (size_t)i);
		    ga.ga_len += i;
		    తోక += i;
		    కొనసాగు;
		}
		zero_వెడల్పు = regmatch.startp[0];
	    }

	    /*
	     * Get some space for a temporary buffer to do the substitution
	     * into.  It will contain:
	     * - The text up to where the match is.
	     * - The substituted text.
	     * - The text after the match.
	     */
	    sublen = vim_regsub(&regmatch, sub, సమాసం, తోక, 0, REGSUB_MAGIC);
	    ఒకవేళ (sublen <= 0)
	    {
		ga_clear(&ga);
		ఆపు;
	    }
	    ఒకవేళ (ga_grow(&ga, (పూర్ణాంకం)((చివర - తోక) + sublen -
			    (regmatch.endp[0] - regmatch.startp[0]))) == FAIL)
	    {
		ga_clear(&ga);
		ఆపు;
	    }

	    // copy the text up to where the match is
	    i = (పూర్ణాంకం)(regmatch.startp[0] - తోక);
	    mch_memmove((char_u *)ga.ga_దత్తాంశం + ga.ga_len, తోక, (size_t)i);
	    // add the substituted text
	    (శూన్య)vim_regsub(&regmatch, sub, సమాసం,
				  (char_u *)ga.ga_దత్తాంశం + ga.ga_len + i, sublen,
				  REGSUB_COPY | REGSUB_MAGIC);
	    ga.ga_len += i + sublen - 1;
	    తోక = regmatch.endp[0];
	    ఒకవేళ (*తోక == NUL)
		ఆపు;
	    ఒకవేళ (!do_all)
		ఆపు;
	}

	ఒకవేళ (ga.ga_దత్తాంశం != ఖాళీ)
	{
	    STRCPY((అక్షరం *)ga.ga_దత్తాంశం + ga.ga_len, తోక);
	    ga.ga_len += (పూర్ణాంకం)(చివర - తోక);
	}

	vim_regfree(regmatch.regprog);
    }

    ఒకవేళ (ga.ga_దత్తాంశం != ఖాళీ)
    {
	str = (char_u *)ga.ga_దత్తాంశం;
	len = (size_t)ga.ga_len;
    }
    ret = vim_strnsave(str, len);
    ga_clear(&ga);
    ఒకవేళ (p_cpo == ఖాళీలు_option)
	p_cpo = save_cpo;
    లేకపోతే
    {
	// Darn, evaluating {sub} expression or {expr} changed the value.
	// If it's still empty it was changed and restored, need to restore in
	// the complicated way.
	ఒకవేళ (*p_cpo == NUL)
	    సమితి_option_విలువ_give_err((char_u *)"cpo", 0L, save_cpo, 0);
	free_string_option(save_cpo);
    }

    ఒకవేళ (ret_len != ఖాళీ)
	*ret_len = len;

    తిరిగి ret;
}
