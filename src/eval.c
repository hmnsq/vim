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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USING_FLOAT_STUFF

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ defined(FEAT_EVAL)

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ VMS
# include <float.h>
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NAMESPACE_CHAR	(char_u *)"abglstvw"

ਸਥਿਰ ਪੂਰਨ eval0_simple_funccal(char_u *arg, typval_T *rettv, exarg_T *eap, evalarg_T *evalarg);
ਸਥਿਰ ਪੂਰਨ eval2(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
ਸਥਿਰ ਪੂਰਨ eval3(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
ਸਥਿਰ ਪੂਰਨ eval4(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
ਸਥਿਰ ਪੂਰਨ eval5(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
ਸਥਿਰ ਪੂਰਨ eval6(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
ਸਥਿਰ ਪੂਰਨ eval7(char_u **arg, typval_T *rettv, evalarg_T *evalarg, ਪੂਰਨ want_string);
ਸਥਿਰ ਪੂਰਨ eval8(char_u **arg, typval_T *rettv, evalarg_T *evalarg, ਪੂਰਨ want_string);
ਸਥਿਰ ਪੂਰਨ eval9(char_u **arg, typval_T *rettv, evalarg_T *evalarg, ਪੂਰਨ want_string);
ਸਥਿਰ ਪੂਰਨ eval9_leader(typval_T *rettv, ਪੂਰਨ numeric_only, char_u *ਸ਼ੁਰੂ_leader, char_u **ਸਿਰਾ_leaderp);

ਸਥਿਰ char_u *make_expanded_name(char_u *ਵਿੱਚ_ਸ਼ੁਰੂ, char_u *ਵਿਅੰਜਕ_ਸ਼ੁਰੂ, char_u *ਵਿਅੰਜਕ_ਸਿਰਾ, char_u *ਵਿੱਚ_ਸਿਰਾ);

/*
 * Return "n1" divided by "n2", taking care of dividing by zero.
 * If "failed" is not NULL set it to TRUE when dividing by zero fails.
 */
	varnumber_T
ਅੰਕ_divide(varnumber_T n1, varnumber_T n2, ਪੂਰਨ *failed)
{
    varnumber_T	ਨਤੀਜਾ;

    ਜੇ (n2 == 0)
    {
	ਜੇ (ਵਿੱਚ_vim9script())
	{
	    emsg(_(e_divide_by_zero));
	    ਜੇ (failed != ਨਿਰਾ)
		*failed = TRUE;
	}
	ਜੇ (n1 == 0)
	    ਨਤੀਜਾ = VARNUM_MIN; // similar to NaN
	ਨਹੀਂਤਾਂ ਜੇ (n1 < 0)
	    ਨਤੀਜਾ = -VARNUM_MAX;
	ਨਹੀਂਤਾਂ
	    ਨਤੀਜਾ = VARNUM_MAX;
    }
    ਨਹੀਂਤਾਂ ਜੇ (n1 == VARNUM_MIN && n2 == -1)
    {
	// specific case: trying to do VARNUM_MIN / -1 results in a positive
	// number that doesn't fit in varnumber_T and causes an FPE
	ਨਤੀਜਾ = VARNUM_MAX;
    }
    ਨਹੀਂਤਾਂ
	ਨਤੀਜਾ = n1 / n2;

    ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * Return "n1" modulus "n2", taking care of dividing by zero.
 * If "failed" is not NULL set it to TRUE when dividing by zero fails.
 */
	varnumber_T
ਅੰਕ_modulus(varnumber_T n1, varnumber_T n2, ਪੂਰਨ *failed)
{
    ਜੇ (n2 == 0 && ਵਿੱਚ_vim9script())
    {
	emsg(_(e_divide_by_zero));
	ਜੇ (failed != ਨਿਰਾ)
	    *failed = TRUE;
    }
    ਵਾਪਸ (n2 == 0) ? 0 : (n1 % n2);
}

/*
 * Initialize the global and v: variables.
 */
    ਖਾਲੀ
eval_init(ਖਾਲੀ)
{
    evalvars_init();
    func_init();
}

#ਜੇ defined(EXITFREE)
    ਖਾਲੀ
eval_clear(ਖਾਲੀ)
{
    evalvars_clear();
    free_scriptnames();  // must come after evalvars_clear().
    free_locales();

    // autoloaded script names
    free_autoload_scriptnames();

    // unreferenced lists, tuples and dicts
    (ਖਾਲੀ)garbage_collect(FALSE);

    // functions not garbage collected
    free_all_functions();
}
#ਜੇ_ਅੰਤ

    ਖਾਲੀ
fill_evalarg_from_eap(evalarg_T *evalarg, exarg_T *eap, ਪੂਰਨ skip)
{
    init_evalarg(evalarg);
    evalarg->eval_flags = skip ? 0 : EVAL_EVALUATE;

    ਜੇ (eap == ਨਿਰਾ)
	ਵਾਪਸ;

    evalarg->eval_cstack = eap->cstack;
    ਜੇ (sourcing_a_script(eap) || eap->ea_getline == get_list_ਸਤਰ)
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
    ਪੂਰਨ
eval_ਮੰਜ਼ਿਲ_bool(
    char_u	*arg,
    ਪੂਰਨ		*error,
    exarg_T	*eap,
    ਪੂਰਨ		skip,	    // only parse, don't execute
    ਪੂਰਨ		use_simple_function)
{
    typval_T	tv;
    varnumber_T	retval = FALSE;
    evalarg_T	evalarg;
    ਪੂਰਨ		r;

    fill_evalarg_from_eap(&evalarg, eap, skip);

    ਜੇ (skip)
	++emsg_skip;
    ਜੇ (use_simple_function)
	r = eval0_simple_funccal(arg, &tv, eap, &evalarg);
    ਨਹੀਂਤਾਂ
	r = eval0(arg, &tv, eap, &evalarg);
    ਜੇ (r == FAIL)
	*error = TRUE;
    ਨਹੀਂਤਾਂ
    {
	*error = FALSE;
	ਜੇ (!skip)
	{
	    ਜੇ (ਵਿੱਚ_vim9script())
		retval = tv_get_bool_chk(&tv, error);
	    ਨਹੀਂਤਾਂ
		retval = (tv_get_ਸੰਖਿਆ_chk(&tv, error) != 0);
	    clear_tv(&tv);
	}
    }
    ਜੇ (skip)
	--emsg_skip;
    clear_evalarg(&evalarg, eap);

    ਵਾਪਸ (ਪੂਰਨ)retval;
}

/*
 * Call eval1() and give an error message if not done at a lower level.
 */
    ਸਥਿਰ ਪੂਰਨ
eval1_emsg(char_u **arg, typval_T *rettv, exarg_T *eap)
{
    char_u	*ਸ਼ੁਰੂ = *arg;
    ਪੂਰਨ		ret;
    ਪੂਰਨ		did_emsg_before = did_emsg;
    ਪੂਰਨ		called_emsg_before = called_emsg;
    evalarg_T	evalarg;

    fill_evalarg_from_eap(&evalarg, eap, eap != ਨਿਰਾ && eap->skip);

    ret = eval1(arg, rettv, &evalarg);
    ਜੇ (ret == FAIL)
    {
	// Report the invalid expression unless the expression evaluation has
	// been cancelled due to an aborting error, an interrupt, or an
	// exception, or we already gave a more specific error.
	// Also check called_emsg for when using assert_fails().
	ਜੇ (!aborting() && did_emsg == did_emsg_before
					  && called_emsg == called_emsg_before)
	    semsg(_(e_invalid_ਵਿਅੰਜਕ_str), ਸ਼ੁਰੂ);
    }
    clear_evalarg(&evalarg, eap);
    ਵਾਪਸ ret;
}

/*
 * Return whether a typval is a valid expression to pass to eval_expr_typval()
 * or eval_expr_to_bool().  An empty string returns FALSE;
 */
    ਪੂਰਨ
eval_ਵਿਅੰਜਕ_valid_arg(typval_T *tv)
{
    ਵਾਪਸ tv->v_type != VAR_UNKNOWN
	    && (tv->v_type != VAR_STRING
		  || (tv->vval.v_string != ਨਿਰਾ && *tv->vval.v_string != NUL));
}

/*
 * When calling eval_expr_typval() many times we only need one funccall_T.
 * Returns NULL when no funccall_T is to be used.
 * When returning non-NULL remove_funccal() must be called later.
 */
    funccall_T *
eval_ਵਿਅੰਜਕ_get_funccal(typval_T *ਵਿਅੰਜਕ, typval_T *rettv)
{
    ਜੇ (ਵਿਅੰਜਕ->v_type != VAR_PARTIAL)
	ਵਾਪਸ ਨਿਰਾ;

    partial_T *partial = ਵਿਅੰਜਕ->vval.v_partial;
    ਜੇ (partial == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    ਜੇ (partial->pt_func == ਨਿਰਾ
	    || partial->pt_func->uf_def_ਸਥਿਤੀ == UF_NOT_COMPILED)
	ਵਾਪਸ ਨਿਰਾ;

    ਵਾਪਸ create_funccal(partial->pt_func, rettv);
}

/*
 * Evaluate a partial.
 * Pass arguments "argv[argc]".
 * "fc_arg" is from eval_expr_get_funccal() or NULL;
 * Return the result in "rettv" and OK or FAIL.
 */
    ਸਥਿਰ ਪੂਰਨ
eval_ਵਿਅੰਜਕ_partial(
    typval_T	*ਵਿਅੰਜਕ,
    typval_T	*argv,
    ਪੂਰਨ		argc,
    funccall_T	*fc_arg,
    typval_T	*rettv)
{
    partial_T	*partial = ਵਿਅੰਜਕ->vval.v_partial;

    ਜੇ (partial == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    ਜੇ (partial->pt_func != ਨਿਰਾ
			&& partial->pt_func->uf_def_ਸਥਿਤੀ != UF_NOT_COMPILED)
    {
	funccall_T	*fc = fc_arg != ਨਿਰਾ ? fc_arg
				: create_funccal(partial->pt_func, rettv);
	ਪੂਰਨ		r;

	ਜੇ (fc == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;

	// Shortcut to call a compiled function with minimal overhead.
	ਜੇ (partial->pt_obj != ਨਿਰਾ)
	    partial->pt_obj->obj_refcount++;
	r = call_def_function(partial->pt_func, argc, argv, DEF_USE_PT_ARGV,
					partial, partial->pt_obj, fc, rettv);
	ਜੇ (fc_arg == ਨਿਰਾ)
	    remove_funccal();
	ਜੇ (r == FAIL)
	    ਵਾਪਸ FAIL;
    }
    ਨਹੀਂਤਾਂ
    {
	char_u		*s = partial_name(partial);
	funcexe_T	funcexe;

	ਜੇ (s == ਨਿਰਾ || *s == NUL)
	    ਵਾਪਸ FAIL;

	CLEAR_ਖੇਤਰ(funcexe);
	funcexe.fe_evaluate = TRUE;
	funcexe.fe_partial = partial;
	ਜੇ (call_func(s, -1, rettv, argc, argv, &funcexe) == FAIL)
	    ਵਾਪਸ FAIL;
    }

    ਵਾਪਸ OK;
}

/*
 * Evaluate an expression which is a function.
 * Pass arguments "argv[argc]".
 * Return the result in "rettv" and OK or FAIL.
 */
    ਸਥਿਰ ਪੂਰਨ
eval_ਵਿਅੰਜਕ_func(
    typval_T	*ਵਿਅੰਜਕ,
    typval_T	*argv,
    ਪੂਰਨ		argc,
    typval_T	*rettv)
{
    funcexe_T	funcexe;
    char_u	buf[NUMBUFLEN];
    char_u	*s;

    ਜੇ (ਵਿਅੰਜਕ->v_type == VAR_FUNC)
	s = ਵਿਅੰਜਕ->vval.v_string;
    ਨਹੀਂਤਾਂ
	s = tv_get_string_buf_chk_strict(ਵਿਅੰਜਕ, buf, ਵਿੱਚ_vim9script());
    ਜੇ (s == ਨਿਰਾ || *s == NUL)
	ਵਾਪਸ FAIL;

    CLEAR_ਖੇਤਰ(funcexe);
    funcexe.fe_evaluate = TRUE;
    ਜੇ (call_func(s, -1, rettv, argc, argv, &funcexe) == FAIL)
	ਵਾਪਸ FAIL;

    ਵਾਪਸ OK;
}

/*
 * Evaluate an expression, which is a string.
 * Return the result in "rettv" and OK or FAIL.
 */
    ਸਥਿਰ ਪੂਰਨ
eval_ਵਿਅੰਜਕ_string(
    typval_T	*ਵਿਅੰਜਕ,
    typval_T	*rettv)
{
    char_u	*s;
    char_u	buf[NUMBUFLEN];

    s = tv_get_string_buf_chk_strict(ਵਿਅੰਜਕ, buf, ਵਿੱਚ_vim9script());
    ਜੇ (s == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    s = skipwhite(s);
    ਜੇ (eval1_emsg(&s, rettv, ਨਿਰਾ) == FAIL)
	ਵਾਪਸ FAIL;

    ਜੇ (*skipwhite(s) != NUL)  // check for trailing chars after expr
    {
	clear_tv(rettv);
	semsg(_(e_invalid_ਵਿਅੰਜਕ_str), s);
	ਵਾਪਸ FAIL;
    }

    ਵਾਪਸ OK;
}

/*
 * Evaluate an expression, which can be a function, partial or string.
 * Pass arguments "argv[argc]".
 * If "want_func" is TRUE treat a string as a function name, not an expression.
 * "fc_arg" is from eval_expr_get_funccal() or NULL;
 * Return the result in "rettv" and OK or FAIL.
 */
    ਪੂਰਨ
eval_ਵਿਅੰਜਕ_typval(
	typval_T    *ਵਿਅੰਜਕ,
	ਪੂਰਨ	    want_func,
	typval_T    *argv,
	ਪੂਰਨ	    argc,
	funccall_T  *fc_arg,
	typval_T    *rettv)
{
    ਜੇ (ਵਿਅੰਜਕ->v_type == VAR_PARTIAL)
	ਵਾਪਸ eval_ਵਿਅੰਜਕ_partial(ਵਿਅੰਜਕ, argv, argc, fc_arg, rettv);
    ਜੇ (ਵਿਅੰਜਕ->v_type == VAR_INSTR)
	ਵਾਪਸ exe_typval_instr(ਵਿਅੰਜਕ, rettv);
    ਜੇ (ਵਿਅੰਜਕ->v_type == VAR_FUNC || want_func)
	ਵਾਪਸ eval_ਵਿਅੰਜਕ_func(ਵਿਅੰਜਕ, argv, argc, rettv);

    ਵਾਪਸ eval_ਵਿਅੰਜਕ_string(ਵਿਅੰਜਕ, rettv);
}

/*
 * Like eval_to_bool() but using a typval_T instead of a string.
 * Works for string, funcref and partial.
 */
    ਪੂਰਨ
eval_ਵਿਅੰਜਕ_ਮੰਜ਼ਿਲ_bool(typval_T *ਵਿਅੰਜਕ, ਪੂਰਨ *error)
{
    typval_T	rettv;
    ਪੂਰਨ		res;

    ਜੇ (eval_ਵਿਅੰਜਕ_typval(ਵਿਅੰਜਕ, FALSE, ਨਿਰਾ, 0, ਨਿਰਾ, &rettv) == FAIL)
    {
	*error = TRUE;
	ਵਾਪਸ FALSE;
    }
    res = (tv_get_bool_chk(&rettv, error) != 0);
    clear_tv(&rettv);
    ਵਾਪਸ res;
}

/*
 * Top level evaluation function, returning a string.  If "skip" is TRUE,
 * only parsing to "nextcmd" is done, without reporting errors.  Return
 * pointer to allocated memory, or NULL for failure or when "skip" is TRUE.
 */
    char_u *
eval_ਮੰਜ਼ਿਲ_string_skip(
    char_u	*arg,
    exarg_T	*eap,
    ਪੂਰਨ		skip)	    // only parse, don't execute
{
    typval_T	tv;
    char_u	*retval;
    evalarg_T	evalarg;

    fill_evalarg_from_eap(&evalarg, eap, skip);
    ਜੇ (skip)
	++emsg_skip;
    ਜੇ (eval0(arg, &tv, eap, &evalarg) == FAIL || skip)
	retval = ਨਿਰਾ;
    ਨਹੀਂਤਾਂ
    {
	retval = vim_strsave(tv_get_string(&tv));
	clear_tv(&tv);
    }
    ਜੇ (skip)
	--emsg_skip;
    clear_evalarg(&evalarg, eap);

    ਵਾਪਸ retval;
}

/*
 * Initialize "evalarg" for use.
 */
    ਖਾਲੀ
init_evalarg(evalarg_T *evalarg)
{
    CLEAR_ਸੂਚਕ(evalarg);
    ga_init2(&evalarg->eval_tofree_ga, ਆਕਾਰ(char_u *), 20);
}

/*
 * If "evalarg->eval_tofree" is not NULL free it later.
 * Caller is expected to overwrite "evalarg->eval_tofree" next.
 */
    ਸਥਿਰ ਖਾਲੀ
free_eval_tofree_later(evalarg_T *evalarg)
{
    ਜੇ (evalarg->eval_tofree == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (ga_grow(&evalarg->eval_tofree_ga, 1) == OK)
	((char_u **)evalarg->eval_tofree_ga.ga_ਅੰਕੜੇ)
	    [evalarg->eval_tofree_ga.ga_len++]
	    = evalarg->eval_tofree;
    ਨਹੀਂਤਾਂ
	vim_free(evalarg->eval_tofree);
}

/*
 * After using "evalarg" filled from "eap": free the memory.
 */
    ਖਾਲੀ
clear_evalarg(evalarg_T *evalarg, exarg_T *eap)
{
    ਜੇ (evalarg == ਨਿਰਾ)
	ਵਾਪਸ;

    garray_T *etga = &evalarg->eval_tofree_ga;

    ਜੇ (evalarg->eval_tofree != ਨਿਰਾ || evalarg->eval_using_cmdline)
    {
	ਜੇ (eap != ਨਿਰਾ)
	{
	    // We may need to keep the original command line, e.g. for
	    // ":let" it has the variable names.  But we may also need
	    // the new one, "nextcmd" points into it.  Keep both.
	    vim_free(eap->cmdline_tofree);
	    eap->cmdline_tofree = *eap->cmdlinep;

	    ਜੇ (evalarg->eval_using_cmdline && etga->ga_len > 0)
	    {
		// "nextcmd" points into the last line in eval_tofree_ga,
		// need to keep it around.
		--etga->ga_len;
		*eap->cmdlinep = ((char_u **)etga->ga_ਅੰਕੜੇ)[etga->ga_len];
		vim_free(evalarg->eval_tofree);
	    }
	    ਨਹੀਂਤਾਂ
		*eap->cmdlinep = evalarg->eval_tofree;
	}
	ਨਹੀਂਤਾਂ
	    vim_free(evalarg->eval_tofree);
	evalarg->eval_tofree = ਨਿਰਾ;
    }

    ga_clear_ਲੜੀਆਂ(etga);
    VIM_CLEAR(evalarg->eval_tofree_lambda);
}

/*
 * Skip over an expression at "*pp".
 * Return FAIL for an error, OK otherwise.
 */
    ਪੂਰਨ
skip_ਵਿਅੰਜਕ(char_u **pp, evalarg_T *evalarg)
{
    typval_T	rettv;

    *pp = skipwhite(*pp);
    ਵਾਪਸ eval1(pp, &rettv, evalarg);
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
    ਪੂਰਨ
skip_ਵਿਅੰਜਕ_concatenate(
	char_u	    **arg,
	char_u	    **ਸ਼ੁਰੂ,
	char_u	    **ਸਿਰਾ,
	evalarg_T   *evalarg)
{
    typval_T	rettv;
    ਪੂਰਨ		res;
    ਪੂਰਨ		vim9script = ਵਿੱਚ_vim9script();
    garray_T    *gap = evalarg == ਨਿਰਾ ? ਨਿਰਾ : &evalarg->eval_ga;
    garray_T    *freegap = evalarg == ਨਿਰਾ ? ਨਿਰਾ : &evalarg->eval_freega;
    ਪੂਰਨ		save_flags = evalarg == ਨਿਰਾ ? 0 : evalarg->eval_flags;
    ਪੂਰਨ		evaluate = evalarg == ਨਿਰਾ
			       ? FALSE : (evalarg->eval_flags & EVAL_EVALUATE);

    ਜੇ (vim9script && evaluate
	       && (evalarg->eval_cookie != ਨਿਰਾ || evalarg->eval_cctx != ਨਿਰਾ))
    {
	ga_init2(gap, ਆਕਾਰ(char_u *), 10);
	// leave room for "start"
	ਜੇ (ga_grow(gap, 1) == OK)
	    ++gap->ga_len;
	ga_init2(freegap, ਆਕਾਰ(char_u *), 10);
    }
    *ਸ਼ੁਰੂ = *arg;

    // Don't evaluate the expression.
    ਜੇ (evalarg != ਨਿਰਾ)
	evalarg->eval_flags &= ~EVAL_EVALUATE;
    *arg = skipwhite(*arg);
    res = eval1(arg, &rettv, evalarg);
    *ਸਿਰਾ = *arg;
    ਜੇ (evalarg != ਨਿਰਾ)
	evalarg->eval_flags = save_flags;

    ਜੇ (vim9script && evaluate
	    && (evalarg->eval_cookie != ਨਿਰਾ || evalarg->eval_cctx != ਨਿਰਾ))
    {
	ਜੇ (evalarg->eval_ga.ga_len == 1)
	{
	    // just the one line, no need to concatenate
	    ga_clear(gap);
	    gap->ga_itemsize = 0;
	}
	ਨਹੀਂਤਾਂ
	{
	    char_u	    *p;
	    size_t	    endoff = STRLEN(*arg);

	    // Line breaks encountered, concatenate all the lines.
	    *((char_u **)gap->ga_ਅੰਕੜੇ) = *ਸ਼ੁਰੂ;
	    p = ga_concat_ਲੜੀਆਂ(gap, " ");

	    // free the lines only when using getsourceline()
	    ਜੇ (evalarg->eval_cookie != ਨਿਰਾ)
	    {
		// Do not free the first line, the caller can still use it.
		*((char_u **)gap->ga_ਅੰਕੜੇ) = ਨਿਰਾ;
		// Do not free the last line, "arg" points into it, free it
		// later.  Also free "eval_tofree" later if needed.
		free_eval_tofree_later(evalarg);
		evalarg->eval_tofree =
				    ((char_u **)gap->ga_ਅੰਕੜੇ)[gap->ga_len - 1];
		((char_u **)gap->ga_ਅੰਕੜੇ)[gap->ga_len - 1] = ਨਿਰਾ;
		ga_clear_ਲੜੀਆਂ(gap);
		ga_clear(freegap);
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ga_clear(gap);

		// free lines that were explicitly marked for freeing
		ga_clear_ਲੜੀਆਂ(freegap);
	    }

	    gap->ga_itemsize = 0;
	    ਜੇ (p == ਨਿਰਾ)
		ਵਾਪਸ FAIL;
	    *ਸ਼ੁਰੂ = p;
	    vim_free(evalarg->eval_tofree_lambda);
	    evalarg->eval_tofree_lambda = p;
	    // Compute "end" relative to the end.
	    *ਸਿਰਾ = *ਸ਼ੁਰੂ + STRLEN(*ਸ਼ੁਰੂ) - endoff;
	}
    }

    ਵਾਪਸ res;
}

/*
 * Convert "tv" to a string.
 * When "join_list" is TRUE convert a List or a Tuple into a sequence of lines.
 * Returns an allocated string (NULL when out of memory).
 */
    char_u *
typval2string(typval_T *tv, ਪੂਰਨ join_list)
{
    garray_T	ga;
    char_u	*retval = ਨਿਰਾ;

    ਜੇ (join_list && (tv->v_type == VAR_LIST || tv->v_type == VAR_TUPLE))
    {
	ਜੇ (tv->v_type == VAR_LIST)
	{
	    ga_init2(&ga, ਆਕਾਰ(ਅੱਖਰ), 80);
	    ਜੇ (tv->vval.v_list != ਨਿਰਾ)
	    {
		list_join(&ga, tv->vval.v_list, (char_u *)"\n", TRUE, FALSE,
									0);
		ਜੇ (tv->vval.v_list->lv_len > 0)
		    ga_append(&ga, NL);
	    }
	    ga_append(&ga, NUL);
	    retval = (char_u *)ga.ga_ਅੰਕੜੇ;
	}
	ਨਹੀਂਤਾਂ
	{
	    // tuple
	    ga_init2(&ga, ਆਕਾਰ(ਅੱਖਰ), 80);
	    ਜੇ (tv->vval.v_tuple != ਨਿਰਾ)
	    {
		tuple_join(&ga, tv->vval.v_tuple, (char_u *)"\n", TRUE, FALSE,
									0);
		ਜੇ (TUPLE_LEN(tv->vval.v_tuple) > 0)
		    ga_append(&ga, NL);
	    }
	    ga_append(&ga, NUL);
	    retval = (char_u *)ga.ga_ਅੰਕੜੇ;
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (tv->v_type == VAR_LIST
	    || tv->v_type == VAR_TUPLE
	    || tv->v_type == VAR_DICT)
    {
	char_u	*tofree;
	char_u	numbuf[NUMBUFLEN];

	retval = tv2string(tv, &tofree, numbuf, 0);
	// Make a copy if we have a value but it's not in allocated memory.
	ਜੇ (retval != ਨਿਰਾ && tofree == ਨਿਰਾ)
	    retval = vim_strsave(retval);
    }
    ਨਹੀਂਤਾਂ
	retval = vim_strsave(tv_get_string(tv));
    ਵਾਪਸ retval;
}

/*
 * Top level evaluation function, returning a string.  Does not handle line
 * breaks.
 * When "join_list" is TRUE convert a List and a Tuple into a sequence of
 * lines.
 * Return pointer to allocated memory, or NULL for failure.
 */
    char_u *
eval_ਮੰਜ਼ਿਲ_string_eap(
    char_u	*arg,
    ਪੂਰਨ		join_list,
    exarg_T	*eap,
    ਪੂਰਨ		use_simple_function)
{
    typval_T	tv;
    char_u	*retval;
    evalarg_T	evalarg;
    ਪੂਰਨ		r;

    fill_evalarg_from_eap(&evalarg, eap, eap != ਨਿਰਾ && eap->skip);
    ਜੇ (use_simple_function)
	r = eval0_simple_funccal(arg, &tv, ਨਿਰਾ, &evalarg);
    ਨਹੀਂਤਾਂ
	r = eval0(arg, &tv, ਨਿਰਾ, &evalarg);
    ਜੇ (r == FAIL)
	retval = ਨਿਰਾ;
    ਨਹੀਂਤਾਂ
    {
	retval = typval2string(&tv, join_list);
	clear_tv(&tv);
    }
    clear_evalarg(&evalarg, ਨਿਰਾ);

    ਵਾਪਸ retval;
}

    char_u *
eval_ਮੰਜ਼ਿਲ_string(
    char_u	*arg,
    ਪੂਰਨ		join_list,
    ਪੂਰਨ		use_simple_function)
{
    ਵਾਪਸ eval_ਮੰਜ਼ਿਲ_string_eap(arg, join_list, ਨਿਰਾ, use_simple_function);
}

/*
 * Call eval_to_string() without using current local variables and using
 * textlock.  When "use_sandbox" is TRUE use the sandbox.
 * Use legacy Vim script syntax.
 */
    char_u *
eval_ਮੰਜ਼ਿਲ_string_ਸੁਰੱਖਿਅਤ(
    char_u	*arg,
    ਪੂਰਨ		use_sandbox,
    ਪੂਰਨ		keep_script_ਸੰਸਕਰਣ,
    ਪੂਰਨ		use_simple_function)
{
    char_u	*retval;
    funccal_ਇੰਦਰਾਜ_T funccal_ਇੰਦਰਾਜ;
    ਪੂਰਨ		save_sc_ਸੰਸਕਰਣ = current_sctx.sc_ਸੰਸਕਰਣ;
    ਪੂਰਨ		save_garbage = may_garbage_collect;

    ਜੇ (!keep_script_ਸੰਸਕਰਣ)
	current_sctx.sc_ਸੰਸਕਰਣ = 1;
    save_funccal(&funccal_ਇੰਦਰਾਜ);
    ਜੇ (use_sandbox)
	++sandbox;
    ++textlock;
    may_garbage_collect = FALSE;
    retval = eval_ਮੰਜ਼ਿਲ_string(arg, FALSE, use_simple_function);
    ਜੇ (use_sandbox)
	--sandbox;
    --textlock;
    may_garbage_collect = save_garbage;
    restore_funccal();
    current_sctx.sc_ਸੰਸਕਰਣ = save_sc_ਸੰਸਕਰਣ;
    ਵਾਪਸ retval;
}

/*
 * Top level evaluation function, returning a number.
 * Evaluates "expr" silently.
 * Returns -1 for an error.
 */
    varnumber_T
eval_ਮੰਜ਼ਿਲ_ਸੰਖਿਆ(char_u *ਵਿਅੰਜਕ, ਪੂਰਨ use_simple_function)
{
    typval_T	rettv;
    varnumber_T	retval;
    char_u	*p = skipwhite(ਵਿਅੰਜਕ);
    ਪੂਰਨ		r = NOTDONE;

    ++emsg_off;

    ਜੇ (use_simple_function)
	r = may_call_simple_func(ਵਿਅੰਜਕ, &rettv);
    ਜੇ (r == NOTDONE)
	r = eval1(&p, &rettv, &EVALARG_EVALUATE);
    ਜੇ (r == FAIL)
	retval = -1;
    ਨਹੀਂਤਾਂ
    {
	retval = tv_get_ਸੰਖਿਆ_chk(&rettv, ਨਿਰਾ);
	clear_tv(&rettv);
    }
    --emsg_off;

    ਵਾਪਸ retval;
}

/*
 * Top level evaluation function.
 * Returns an allocated typval_T with the result.
 * Returns NULL when there is an error.
 */
    typval_T *
eval_ਵਿਅੰਜਕ(char_u *arg, exarg_T *eap)
{
    ਵਾਪਸ eval_ਵਿਅੰਜਕ_ext(arg, eap, FALSE);
}

    typval_T *
eval_ਵਿਅੰਜਕ_ext(char_u *arg, exarg_T *eap, ਪੂਰਨ use_simple_function)
{
    typval_T	*tv;
    evalarg_T	evalarg;

    fill_evalarg_from_eap(&evalarg, eap, eap != ਨਿਰਾ && eap->skip);

    tv = ALLOC_ONE(typval_T);
    ਜੇ (tv != ਨਿਰਾ)
    {
	ਪੂਰਨ	r = NOTDONE;

	ਜੇ (use_simple_function)
	    r = eval0_simple_funccal(arg, tv, eap, &evalarg);
	ਜੇ (r == NOTDONE)
	    r = eval0(arg, tv, eap, &evalarg);

	ਜੇ (r == FAIL)
	    VIM_CLEAR(tv);
    }

    clear_evalarg(&evalarg, eap);
    ਵਾਪਸ tv;
}

/*
 * "*arg" points to what can be a function name in the form of "import.Name" or
 * "Funcref".  Return the name of the function.  Set "tofree" to something that
 * was allocated.
 * If "verbose" is FALSE no errors are given.
 * Return NULL for any failure.
 */
    ਸਥਿਰ char_u *
deref_function_name(
	    char_u	**arg,
	    char_u	**tofree,
	    evalarg_T	*evalarg,
	    ਪੂਰਨ		verbose)
{
    typval_T	ref;
    char_u	*name = *arg;
    ਪੂਰਨ		save_flags = 0;
    ਪੂਰਨ		evaluate = evalarg != ਨਿਰਾ
				      && (evalarg->eval_flags & EVAL_EVALUATE);

    ref.v_type = VAR_UNKNOWN;
    ਜੇ (evalarg != ਨਿਰਾ)
    {
	// need to evaluate this to get an import, like in "a.Func"
	save_flags = evalarg->eval_flags;
	evalarg->eval_flags |= EVAL_EVALUATE;
    }
    ਜੇ (eval9(arg, &ref, evalarg, FALSE) == FAIL)
    {
	dictitem_T	*v;

	// If <SID>VarName was used it would not be found, try another way.
	v = ਲੱਭੋ_var_also_ਵਿੱਚ_script(name, ਨਿਰਾ, FALSE);
	ਜੇ (v == ਨਿਰਾ)
	{
	    name = ਨਿਰਾ;
	    ਜਾਓ theend;
	}
	copy_tv(&v->di_tv, &ref);
    }
    ਜੇ (*skipwhite(*arg) != NUL)
    {
	ਜੇ (verbose)
	    semsg(_(e_trailing_characters_str), *arg);
	name = ਨਿਰਾ;
    }
    ਨਹੀਂਤਾਂ ਜੇ (ref.v_type == VAR_FUNC && ref.vval.v_string != ਨਿਰਾ)
    {
	name = ref.vval.v_string;
	ref.vval.v_string = ਨਿਰਾ;
	*tofree = name;
    }
    ਨਹੀਂਤਾਂ ਜੇ (ref.v_type == VAR_PARTIAL && ref.vval.v_partial != ਨਿਰਾ)
    {
	ਜੇ (ref.vval.v_partial->pt_argc > 0
		|| ref.vval.v_partial->pt_dict != ਨਿਰਾ)
	{
	    ਜੇ (verbose)
		emsg(_(e_cannot_use_partial_ਇੱਥੇ));
	    name = ਨਿਰਾ;
	}
	ਨਹੀਂਤਾਂ
	{
	    name = vim_strsave(partial_name(ref.vval.v_partial));
	    *tofree = name;
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (evaluate)
    {
	ਜੇ (verbose)
	    semsg(_(e_not_callable_type_str), name);
	name = ਨਿਰਾ;
    }

theend:
    clear_tv(&ref);
    ਜੇ (evalarg != ਨਿਰਾ)
	evalarg->eval_flags = save_flags;
    ਵਾਪਸ name;
}

/*
 * Call some Vim script function and return the result in "*rettv".
 * Uses argv[0] to argv[argc - 1] for the function arguments.  argv[argc]
 * should have type VAR_UNKNOWN.
 * Returns OK or FAIL.
 */
    ਪੂਰਨ
call_vim_function(
    char_u      *func,
    ਪੂਰਨ		argc,
    typval_T	*argv,
    typval_T	*rettv)
{
    ਪੂਰਨ		ret;
    funcexe_T	funcexe;
    char_u	*arg;
    char_u	*name;
    char_u	*tofree = ਨਿਰਾ;
    ਪੂਰਨ		ignore_errors;

    rettv->v_type = VAR_UNKNOWN;		// clear_tv() uses this
    CLEAR_ਖੇਤਰ(funcexe);
    funcexe.fe_firstline = curwin->w_cursor.lnum;
    funcexe.fe_lastline = curwin->w_cursor.lnum;
    funcexe.fe_evaluate = TRUE;

    // The name might be "import.Func" or "Funcref".  We don't know, we need to
    // ignore errors for an undefined name.  But we do want errors when an
    // autoload script has errors.  Guess that when there is a dot in the name
    // showing errors is the right choice.
    ignore_errors = vim_strchr(func, '.') == ਨਿਰਾ;
    arg = func;
    ਜੇ (ignore_errors)
	++emsg_off;
    name = deref_function_name(&arg, &tofree, &EVALARG_EVALUATE, FALSE);
    ਜੇ (ignore_errors)
	--emsg_off;
    ਜੇ (name == ਨਿਰਾ)
	name = func;

    ret = call_func(name, -1, rettv, argc, argv, &funcexe);

    ਜੇ (ret == FAIL)
	clear_tv(rettv);
    vim_free(tofree);

    ਵਾਪਸ ret;
}

/*
 * Call Vim script function "func" and return the result as a string.
 * Uses "argv[0]" to "argv[argc - 1]" for the function arguments. "argv[argc]"
 * should have type VAR_UNKNOWN.
 * Returns NULL when calling the function fails.
 */
    ਖਾਲੀ *
call_func_retstr(
    char_u      *func,
    ਪੂਰਨ		argc,
    typval_T	*argv)
{
    typval_T	rettv;
    char_u	*retval;

    ਜੇ (call_vim_function(func, argc, argv, &rettv) == FAIL)
	ਵਾਪਸ ਨਿਰਾ;

    retval = vim_strsave(tv_get_string(&rettv));
    clear_tv(&rettv);
    ਵਾਪਸ retval;
}

/*
 * Call Vim script function "func" and return the result as a List.
 * Uses "argv" and "argc" as call_func_retstr().
 * Returns NULL when there is something wrong.
 * Gives an error when the returned value is not a list.
 */
    ਖਾਲੀ *
call_func_retlist(
    char_u      *func,
    ਪੂਰਨ		argc,
    typval_T	*argv)
{
    typval_T	rettv;

    ਜੇ (call_vim_function(func, argc, argv, &rettv) == FAIL)
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (rettv.v_type != VAR_LIST)
    {
	semsg(_(e_custom_list_completion_function_does_not_return_list_but_str),
		vartype_name(rettv.v_type));
	clear_tv(&rettv);
	ਵਾਪਸ ਨਿਰਾ;
    }

    ਵਾਪਸ rettv.vval.v_list;
}

#ਜੇ defined(FEAT_FOLDING)
/*
 * Evaluate "arg", which is 'foldexpr'.
 * Note: caller must set "curwin" to match "arg".
 * Returns the foldlevel, and any character preceding it in "*cp".  Doesn't
 * give error messages.
 */
    ਪੂਰਨ
eval_foldexpr(win_T *wp, ਪੂਰਨ *cp)
{
    char_u	*arg;
    typval_T	tv;
    varnumber_T	retval;
    char_u	*s;
    sctx_T	saved_sctx = current_sctx;
    ਪੂਰਨ		use_sandbox = was_ਸੈੱਟ_insecurely(wp, (char_u *)"foldexpr",
								    OPT_LOCAL);

    arg = skipwhite(wp->w_p_fde);
    current_sctx = wp->w_p_script_ctx[WV_FDE];

    ++emsg_off;
    ਜੇ (use_sandbox)
	++sandbox;
    ++textlock;
    *cp = NUL;

    // Evaluate the expression.  If the expression is "FuncName()" call the
    // function directly.
    ਜੇ (eval0_simple_funccal(arg, &tv, ਨਿਰਾ, &EVALARG_EVALUATE) == FAIL)
	retval = 0;
    ਨਹੀਂਤਾਂ
    {
	// If the result is a number, just return the number.
	ਜੇ (tv.v_type == VAR_ਸੰਖਿਆ)
	    retval = tv.vval.v_ਸੰਖਿਆ;
	ਨਹੀਂਤਾਂ ਜੇ (tv.v_type != VAR_STRING || tv.vval.v_string == ਨਿਰਾ)
	    retval = 0;
	ਨਹੀਂਤਾਂ
	{
	    // If the result is a string, check if there is a non-digit before
	    // the number.
	    s = tv.vval.v_string;
	    ਜੇ (*s != NUL && !VIM_ISDIGIT(*s) && *s != '-')
		*cp = *s++;
	    retval = atol((ਅੱਖਰ *)s);
	}
	clear_tv(&tv);
    }
    --emsg_off;
    ਜੇ (use_sandbox)
	--sandbox;
    --textlock;
    clear_evalarg(&EVALARG_EVALUATE, ਨਿਰਾ);
    current_sctx = saved_sctx;

    ਵਾਪਸ (ਪੂਰਨ)retval;
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ LOG_LOCKVAR
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ flag_string_S
{
    ਪੂਰਨ	    flag;
    ਅੱਖਰ    *str;
} flag_string_T;

    ਸਥਿਰ ਅੱਖਰ *
flags_tostring(ਪੂਰਨ flags, flag_string_T *_fstring, ਅੱਖਰ *buf, size_t n)
{
    ਅੱਖਰ *p = buf;
    *p = NUL;
    ਲਈ (flag_string_T *fstring = _fstring; fstring->flag; ++fstring)
    {
	ਜੇ ((fstring->flag & flags) != 0)
	{
	    size_t len = STRLEN(fstring->str);
	    ਜੇ (n > p - buf + len + 7)
	    {
		STRCAT(p, fstring->str);
		p += len;
		STRCAT(p, " ");
		++p;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		STRCAT(buf, "...");
		ਰੋਕੋ;
	    }
	}
    }
    ਵਾਪਸ buf;
}

flag_string_T glv_flag_ਲੜੀਆਂ[] = {
    { GLV_QUIET,		"QUIET" },
    { GLV_NO_AUTOLOAD,		"NO_AUTOLOAD" },
    { GLV_READ_ONLY,		"READ_ONLY" },
    { GLV_NO_DECL,		"NO_DECL" },
    { GLV_COMPILING,		"COMPILING" },
    { GLV_ਸੌਂਪੋ_WITH_OP,	"ASSIGN_WITH_OP" },
    { GLV_PREFER_FUNC,		"PREFER_FUNC" },
    { 0,			ਨਿਰਾ }
};
#ਜੇ_ਅੰਤ

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
    ਸਥਿਰ ਖਾਲੀ
fill_lval_from_lval_ਜੜ੍ਹ(lval_T *lp, lval_ਜੜ੍ਹ_T *lr)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ LOG_LOCKVAR
    ch_log(ਨਿਰਾ, "LKVAR: fill_lval_from_lval_root(): name %s, tv %p",
						lp->ll_name, (ਖਾਲੀ*)lr->lr_tv);
#ਜੇ_ਅੰਤ
    ਜੇ (lr->lr_tv == ਨਿਰਾ)
	ਵਾਪਸ;
    ਜੇ (!lr->lr_is_arg && lr->lr_tv->v_type == VAR_CLASS)
    {
	ਜੇ (lr->lr_tv->vval.v_class != ਨਿਰਾ)
	{
	    // Special special case. Look for a bare class variable reference.
	    class_T	*cl = lr->lr_tv->vval.v_class;
	    ਪੂਰਨ		m_idx;
	    ocmember_T	*m = class_member_lookup(cl, lp->ll_name,
					lp->ll_name_ਸਿਰਾ - lp->ll_name, &m_idx);
	    ਜੇ (m != ਨਿਰਾ)
	    {
		// Assuming "inside class" since bare reference.
		lp->ll_class = lr->lr_tv->vval.v_class;
		lp->ll_oi = m_idx;
		lp->ll_valtype = m->ocm_type;
		lp->ll_tv = &lp->ll_class->class_members_tv[m_idx];
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ LOG_LOCKVAR
		ch_log(ਨਿਰਾ, "LKVAR:    ... class member %s.%s",
					lp->ll_class->class_name.string, lp->ll_name);
#ਜੇ_ਅੰਤ
		ਵਾਪਸ;
	    }
	}
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ LOG_LOCKVAR
    ch_log(ਨਿਰਾ, "LKVAR:    ... type: %s", vartype_name(lr->lr_tv->v_type));
#ਜੇ_ਅੰਤ
    lp->ll_tv = lr->lr_tv;
    lp->ll_is_ਜੜ੍ਹ = TRUE;
}

/*
 * Check if the class has permission to access the member.
 * Returns OK or FAIL.
 */
    ਸਥਿਰ ਪੂਰਨ
get_lval_check_access(
    class_T	*cl_exec,   // executing class, NULL if :def or script level
    class_T	*cl,	    // class which contains the member
    ocmember_T	*om,	    // member being accessed
    char_u	*p,	    // char after member name
    ਪੂਰਨ		flags)	    // GLV flags to check if writing to lval
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ LOG_LOCKVAR
    ch_log(ਨਿਰਾ, "LKVAR: get_lval_check_access(), cl_exec %p, cl %p, %c",
						(ਖਾਲੀ*)cl_exec, (ਖਾਲੀ*)cl, *p);
#ਜੇ_ਅੰਤ
    ਜੇ (cl_exec != ਨਿਰਾ && cl_exec == cl)
	ਵਾਪਸ OK;

    ਅੱਖਰ *ਸੁਨੇਹਾ = ਨਿਰਾ;
    ਚੋਣ (om->ocm_access)
    {
	ਹਾਲਤ VIM_ACCESS_ਨਿੱਜੀ:
	    ਸੁਨੇਹਾ = e_cannot_access_protected_variable_str;
	    ਰੋਕੋ;
	ਹਾਲਤ VIM_ACCESS_READ:
	    // If [idx] or .key following, read only OK.
	    ਜੇ (*p == '[' || *p == '.')
		ਰੋਕੋ;
	    ਜੇ ((flags & GLV_READ_ONLY) == 0)
	    {
		ਜੇ (IS_ENUM(cl))
		{
		    ਜੇ (om->ocm_type->tt_type == VAR_OBJECT)
			semsg(_(e_enumvalue_str_cannot_be_modified),
				cl->class_name.string, om->ocm_name.string);
		    ਨਹੀਂਤਾਂ
			ਸੁਨੇਹਾ = e_variable_is_not_writable_str;
		}
		ਨਹੀਂਤਾਂ
		    ਸੁਨੇਹਾ = e_variable_is_not_writable_str;
	    }
	    ਰੋਕੋ;
	ਹਾਲਤ VIM_ACCESS_ALL:
	    ਰੋਕੋ;
    }
    ਜੇ (ਸੁਨੇਹਾ != ਨਿਰਾ)
    {
	emsg_var_cl_define(ਸੁਨੇਹਾ, om->ocm_name.string, 0, cl);
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
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
    ਸਥਿਰ char_u *
get_lval_imported(
    lval_T	*lp,
    scid_T	imp_sid,
    char_u	*p,
    dictitem_T	**dip,
    ਪੂਰਨ		fne_flags)
{
    ufunc_T	*ufunc;
    type_T	*type = ਨਿਰਾ;
    ਪੂਰਨ		cc;
    ਪੂਰਨ		rc = FAIL;

    p = skipwhite(p);

    import_check_sourced_sid(&imp_sid);
    lp->ll_sid = imp_sid;
    lp->ll_name = p;
    p = ਲੱਭੋ_name_ਸਿਰਾ(lp->ll_name, ਨਿਰਾ, ਨਿਰਾ, fne_flags);
    lp->ll_name_ਸਿਰਾ = p;

    // check the item is exported
    cc = *p;
    *p = NUL;
    ਜੇ (ਲੱਭੋ_exported(imp_sid, lp->ll_name, &ufunc, &type, ਨਿਰਾ, ਨਿਰਾ,
								TRUE) == -1)
	ਜਾਓ failed;

    // Get the typval for the exported item
    hashtab_T *ht = &SCRIPT_VARS(imp_sid);
    ਜੇ (ht == ਨਿਰਾ)
	ਜਾਓ failed;

    dictitem_T *di = ਲੱਭੋ_var_ਵਿੱਚ_ht(ht, 0, lp->ll_name, TRUE);
    ਜੇ (di == ਨਿਰਾ)
	// script is autoloaded.  So variable will be found later
	ਜਾਓ success;

    *dip = di;

    // Check whether the variable is writable.
    svar_T *sv = ਲੱਭੋ_typval_ਵਿੱਚ_script(&di->di_tv, imp_sid, FALSE);
    ਜੇ (sv != ਨਿਰਾ && sv->sv_const != 0)
    {
	semsg(_(e_cannot_change_readonly_variable_str), lp->ll_name);
	ਜਾਓ failed;
    }

    // check whether variable is locked
    ਜੇ (ਮੁੱਲ_check_ਤਾਲਾ(di->di_tv.v_ਤਾਲਾ, lp->ll_name, FALSE))
	ਜਾਓ failed;

    lp->ll_tv = &di->di_tv;
    lp->ll_valtype = type;

success:
    rc = OK;

failed:
    *p = cc;
    ਵਾਪਸ rc == OK ? p : ਨਿਰਾ;
}

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
    GLV_FAIL,
    GLV_OK,
    GLV_STOP
} glv_ਸਥਿਤੀ_T;

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
    ਸਥਿਰ ਪੂਰਨ
get_lval_dict_ਵਸਤੂ(
    lval_T	*lp,
    char_u	*name,
    char_u	*ਕੁੰਜੀ,
    ਪੂਰਨ		len,
    char_u	**ਕੁੰਜੀ_ਸਿਰਾ,
    typval_T	*var1,
    ਪੂਰਨ		flags,
    ਪੂਰਨ		unlet,
    typval_T	*rettv)
{
    ਪੂਰਨ		quiet = flags & GLV_QUIET;
    char_u	*p = *ਕੁੰਜੀ_ਸਿਰਾ;

    ਜੇ (len == -1)
    {
	// "[key]": get key from "var1"
	ਕੁੰਜੀ = tv_get_string_chk(var1);	// is number or string
	ਜੇ (ਕੁੰਜੀ == ਨਿਰਾ)
	    ਵਾਪਸ GLV_FAIL;
    }
    lp->ll_list = ਨਿਰਾ;
    lp->ll_list = ਨਿਰਾ;
    lp->ll_blob = ਨਿਰਾ;
    lp->ll_object = ਨਿਰਾ;
    lp->ll_class = ਨਿਰਾ;
    lp->ll_tuple = ਨਿਰਾ;

    // a NULL dict is equivalent with an empty dict
    ਜੇ (lp->ll_tv->vval.v_dict == ਨਿਰਾ)
    {
	lp->ll_tv->vval.v_dict = dict_alloc();
	ਜੇ (lp->ll_tv->vval.v_dict == ਨਿਰਾ)
	    ਵਾਪਸ GLV_FAIL;
	++lp->ll_tv->vval.v_dict->dv_refcount;
    }
    lp->ll_dict = lp->ll_tv->vval.v_dict;

    lp->ll_di = dict_ਲੱਭੋ(lp->ll_dict, ਕੁੰਜੀ, len);

    // When assigning to a scope dictionary check that a function and
    // variable name is valid (only variable name unless it is l: or
    // g: dictionary). Disallow overwriting a builtin function.
    ਜੇ (rettv != ਨਿਰਾ && lp->ll_dict->dv_scope != 0)
    {
	ਪੂਰਨ prevval;

	ਜੇ (len != -1)
	{
	    prevval = ਕੁੰਜੀ[len];
	    ਕੁੰਜੀ[len] = NUL;
	}
	ਨਹੀਂਤਾਂ
	    prevval = 0; // avoid compiler warning
	ਪੂਰਨ wrong = (lp->ll_dict->dv_scope == VAR_DEF_SCOPE
		&& (rettv->v_type == VAR_FUNC
		    || rettv->v_type == VAR_PARTIAL)
		&& var_wrong_func_name(ਕੁੰਜੀ, lp->ll_di == ਨਿਰਾ))
	    || !valid_varname(ਕੁੰਜੀ, -1, TRUE);
	ਜੇ (len != -1)
	    ਕੁੰਜੀ[len] = prevval;
	ਜੇ (wrong)
	    ਵਾਪਸ GLV_FAIL;
    }

    ਜੇ (lp->ll_valtype != ਨਿਰਾ)
	// use the type of the member
	lp->ll_valtype = lp->ll_valtype->tt_member;

    ਜੇ (lp->ll_di == ਨਿਰਾ)
    {
	// Can't add "v:" or "a:" variable.
	ਜੇ (lp->ll_dict == get_vimvar_dict()
		|| &lp->ll_dict->dv_hashtab == get_funccal_ਦਲੀਲਾਂ_ht())
	{
	    semsg(_(e_illegal_variable_name_str), name);
	    ਵਾਪਸ GLV_FAIL;
	}

	// Key does not exist in dict: may need to add it.
	ਜੇ (*p == '[' || *p == '.' || unlet)
	{
	    ਜੇ (!quiet)
		semsg(_(e_ਕੁੰਜੀ_not_present_ਵਿੱਚ_dictionary_str), ਕੁੰਜੀ);
	    ਵਾਪਸ GLV_FAIL;
	}
	ਜੇ (len == -1)
	    lp->ll_newkey = vim_strsave(ਕੁੰਜੀ);
	ਨਹੀਂਤਾਂ
	    lp->ll_newkey = vim_strnsave(ਕੁੰਜੀ, len);
	ਜੇ (lp->ll_newkey == ਨਿਰਾ)
	    p = ਨਿਰਾ;

	*ਕੁੰਜੀ_ਸਿਰਾ = p;
	ਵਾਪਸ GLV_STOP;
    }
    // existing variable, need to check if it can be changed
    ਨਹੀਂਤਾਂ ਜੇ ((flags & GLV_READ_ONLY) == 0
	    && (var_check_ro(lp->ll_di->di_flags, name, FALSE)
		|| var_check_ਤਾਲਾ(lp->ll_di->di_flags, name, FALSE)))
	ਵਾਪਸ GLV_FAIL;

    lp->ll_tv = &lp->ll_di->di_tv;

    ਵਾਪਸ GLV_OK;
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
    ਸਥਿਰ ਪੂਰਨ
get_lval_blob(
    lval_T	*lp,
    typval_T	*var1,
    typval_T	*var2,
    ਪੂਰਨ		empty1,
    ਪੂਰਨ		quiet)
{
    ਲੰਮਾ	bloblen = blob_len(lp->ll_tv->vval.v_blob);

    lp->ll_list = ਨਿਰਾ;
    lp->ll_dict = ਨਿਰਾ;
    lp->ll_object = ਨਿਰਾ;
    lp->ll_class = ਨਿਰਾ;
    lp->ll_tuple = ਨਿਰਾ;

    // Get the number and item for the only or first index of a List or Tuple.
    ਜੇ (empty1)
	lp->ll_n1 = 0;
    ਨਹੀਂਤਾਂ
	// is number or string
	lp->ll_n1 = (ਲੰਮਾ)tv_get_ਸੰਖਿਆ(var1);

    ਜੇ (check_blob_ਸੂਚਕਾਂਕ(bloblen, lp->ll_n1, quiet) == FAIL)
	ਵਾਪਸ FAIL;
    ਜੇ (lp->ll_range && !lp->ll_empty2)
    {
	lp->ll_n2 = (ਲੰਮਾ)tv_get_ਸੰਖਿਆ(var2);
	ਜੇ (check_blob_range(bloblen, lp->ll_n1, lp->ll_n2, quiet) == FAIL)
	    ਵਾਪਸ FAIL;
    }

    ਜੇ (!lp->ll_range)
	// Indexing a single byte in a blob.  So the rhs type is a
	// number.
	lp->ll_valtype = &t_ਸੰਖਿਆ;

    lp->ll_blob = lp->ll_tv->vval.v_blob;
    lp->ll_tv = ਨਿਰਾ;

    ਵਾਪਸ OK;
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
    ਸਥਿਰ ਪੂਰਨ
get_lval_list(
    lval_T	*lp,
    typval_T	*var1,
    typval_T	*var2,
    ਪੂਰਨ		empty1,
    ਪੂਰਨ		flags,
    ਪੂਰਨ		quiet)
{
    /*
     * Get the number and item for the only or first index of the List.
     */
    ਜੇ (empty1)
	lp->ll_n1 = 0;
    ਨਹੀਂਤਾਂ
	// is number or string
	lp->ll_n1 = (ਲੰਮਾ)tv_get_ਸੰਖਿਆ(var1);

    lp->ll_dict = ਨਿਰਾ;
    lp->ll_object = ਨਿਰਾ;
    lp->ll_class = ਨਿਰਾ;
    lp->ll_tuple = ਨਿਰਾ;
    lp->ll_list = lp->ll_tv->vval.v_list;
    lp->ll_li = check_range_ਸੂਚਕਾਂਕ_one(lp->ll_list, &lp->ll_n1,
				(flags & GLV_ਸੌਂਪੋ_WITH_OP) == 0, quiet);
    ਜੇ (lp->ll_li == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    ਜੇ (lp->ll_valtype != ਨਿਰਾ && !lp->ll_range)
    {
	// use the type of the member
	ਜੇ (lp->ll_valtype->tt_member != ਨਿਰਾ)
	    lp->ll_valtype = lp->ll_valtype->tt_member;
	ਨਹੀਂਤਾਂ
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
    ਜੇ (lp->ll_range && !lp->ll_empty2)
    {
	lp->ll_n2 = (ਲੰਮਾ)tv_get_ਸੰਖਿਆ(var2);
	// is number or string
	ਜੇ (check_range_ਸੂਚਕਾਂਕ_two(lp->ll_list,
			&lp->ll_n1, lp->ll_li, &lp->ll_n2, quiet) == FAIL)
	    ਵਾਪਸ FAIL;
    }

    lp->ll_tv = &lp->ll_li->li_tv;

    ਵਾਪਸ OK;
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
    ਸਥਿਰ ਪੂਰਨ
get_lval_tuple(
    lval_T	*lp,
    typval_T	*idx,
    ਪੂਰਨ		quiet)
{
    // is number or string
    lp->ll_n1 = (ਲੰਮਾ)tv_get_ਸੰਖਿਆ(idx);

    lp->ll_list = ਨਿਰਾ;
    lp->ll_dict = ਨਿਰਾ;
    lp->ll_blob = ਨਿਰਾ;
    lp->ll_object = ਨਿਰਾ;
    lp->ll_class = ਨਿਰਾ;

    lp->ll_tuple = lp->ll_tv->vval.v_tuple;
    lp->ll_tv = tuple_ਲੱਭੋ(lp->ll_tuple, lp->ll_n1);
    ਜੇ (lp->ll_tv == ਨਿਰਾ)
    {
	ਜੇ (!quiet)
	    semsg(_(e_tuple_ਸੂਚਕਾਂਕ_ਬਾਹਰਲਾ_ਦਾ_range_nr), lp->ll_n1);
	ਵਾਪਸ GLV_FAIL;
    }

    // use the type of the member
    ਜੇ (lp->ll_valtype != ਨਿਰਾ)
    {
	ਜੇ (lp->ll_valtype != ਨਿਰਾ
		&& lp->ll_valtype->tt_type == VAR_TUPLE
		&& lp->ll_valtype->tt_argcount == 1)
	{
	    // a variadic tuple or a single item tuple
	    ਜੇ (lp->ll_valtype->tt_flags & TTFLAG_VARARGS)
		lp->ll_valtype = lp->ll_valtype->tt_ਦਲੀਲਾਂ[0]->tt_member;
	    ਨਹੀਂਤਾਂ
		lp->ll_valtype = lp->ll_valtype->tt_ਦਲੀਲਾਂ[0];
	}
	ਨਹੀਂਤਾਂ
	    // If the LHS member type is not known (VAR_ANY), then get it from
	    // the tuple item (after indexing)
	    lp->ll_valtype = typval2type(lp->ll_tv, get_copyID(),
					&lp->ll_type_list, TVTT_DO_MEMBER);
    }

    ਵਾਪਸ GLV_OK;
}

/*
 * Get a class or object lval method in class "cl".  The 'key' argument points
 * to the method name and 'key_end' points to the character after 'key'.
 * 'v_type' is VAR_CLASS or VAR_OBJECT.
 *
 * The method index, method function pointer and method type are returned in
 * "lp".
 */
    ਸਥਿਰ ਪੂਰਨ
get_lval_oc_ਵਿਧੀ(
    lval_T	*lp,
    class_T	*cl,
    char_u	*ਕੁੰਜੀ,
    char_u	**ਕੁੰਜੀ_ਸਿਰਾ,
    vartype_T	v_type)
{
    // Look for a method with this name.
    // round 1: class functions (skipped for an object)
    // round 2: object methods
    ਲਈ (ਪੂਰਨ ਗੇੜਾ = v_type == VAR_OBJECT ? 2 : 1; ਗੇੜਾ <= 2; ++ਗੇੜਾ)
    {
	ਪੂਰਨ	m_idx;
	ufunc_T	*fp;

	fp = ਵਿਧੀ_lookup(cl, ਗੇੜਾ == 1 ? VAR_CLASS : VAR_OBJECT,
						ਕੁੰਜੀ, *ਕੁੰਜੀ_ਸਿਰਾ - ਕੁੰਜੀ, &m_idx);
	lp->ll_oi = m_idx;

	// process generic method (if present)
	ਜੇ (fp && (fp = eval_generic_func(fp, ਕੁੰਜੀ, ਕੁੰਜੀ_ਸਿਰਾ)) == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;

	ਜੇ (fp != ਨਿਰਾ)
	{
	    lp->ll_ufunc = fp;
	    lp->ll_valtype = fp->uf_func_type;
	    ਰੋਕੋ;
	}
    }

    ਵਾਪਸ OK;
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
    ਸਥਿਰ ਪੂਰਨ
get_lval_oc_variable(
    lval_T	*lp,
    class_T	*cl,
    char_u	*ਕੁੰਜੀ,
    char_u	*ਕੁੰਜੀ_ਸਿਰਾ,
    vartype_T	v_type,
    class_T	*cl_exec,
    ਪੂਰਨ		flags)
{
    ਪੂਰਨ		m_idx;
    ocmember_T	*om;

    om = member_lookup(cl, v_type, ਕੁੰਜੀ, ਕੁੰਜੀ_ਸਿਰਾ - ਕੁੰਜੀ, &m_idx);
    lp->ll_oi = m_idx;
    ਜੇ (om == ਨਿਰਾ)
	ਵਾਪਸ OK;

    // Check variable is accessible
    ਜੇ (get_lval_check_access(cl_exec, cl, om, ਕੁੰਜੀ_ਸਿਰਾ, flags) == FAIL)
	ਵਾਪਸ FAIL;

    // When lhs is used to modify the variable, check it is not a read-only
    // variable.
    ਜੇ ((flags & GLV_READ_ONLY) == 0 && (*ਕੁੰਜੀ_ਸਿਰਾ != '.' && *ਕੁੰਜੀ_ਸਿਰਾ != '[')
	    && oc_var_check_ro(cl, om))
	ਵਾਪਸ FAIL;

    lp->ll_valtype = om->ocm_type;

    ਜੇ (v_type == VAR_OBJECT)
	lp->ll_tv = ((typval_T *)(lp->ll_tv->vval.v_object + 1)) + m_idx;
    ਨਹੀਂਤਾਂ
	lp->ll_tv = &cl->class_members_tv[m_idx];

    ਵਾਪਸ OK;
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
    ਸਥਿਰ ਪੂਰਨ
get_lval_class_or_obj(
    lval_T	*lp,
    char_u	*ਕੁੰਜੀ,
    char_u	**ਕੁੰਜੀ_ਸਿਰਾ,
    vartype_T	v_type,
    class_T	*cl_exec,
    ਪੂਰਨ		flags,
    ਪੂਰਨ		quiet)
{
    lp->ll_dict = ਨਿਰਾ;
    lp->ll_list = ਨਿਰਾ;
    lp->ll_tuple = ਨਿਰਾ;

    class_T *cl;
    ਜੇ (v_type == VAR_OBJECT)
    {
	ਜੇ (lp->ll_tv->vval.v_object == ਨਿਰਾ)
	{
	    ਜੇ (!quiet)
		emsg(_(e_using_null_object));
	    ਵਾਪਸ FAIL;
	}
	cl = lp->ll_tv->vval.v_object->obj_class;
	lp->ll_object = lp->ll_tv->vval.v_object;
    }
    ਨਹੀਂਤਾਂ
    {
	cl = lp->ll_tv->vval.v_class;
	lp->ll_object = ਨਿਰਾ;
    }
    lp->ll_class = cl;

    ਜੇ (cl == ਨਿਰਾ)
	// TODO: what if class is NULL?
	ਵਾਪਸ OK;

    lp->ll_valtype = ਨਿਰਾ;

    ਜੇ (flags & GLV_PREFER_FUNC)
	ਜੇ (get_lval_oc_ਵਿਧੀ(lp, cl, ਕੁੰਜੀ, ਕੁੰਜੀ_ਸਿਰਾ, v_type) == FAIL)
	    ਵਾਪਸ FAIL;

    // Look for object/class member variable
    ਜੇ (lp->ll_valtype == ਨਿਰਾ)
    {
	ਜੇ (get_lval_oc_variable(lp, cl, ਕੁੰਜੀ, *ਕੁੰਜੀ_ਸਿਰਾ, v_type, cl_exec, flags)
								== FAIL)
	    ਵਾਪਸ FAIL;
    }

    ਜੇ (lp->ll_valtype == ਨਿਰਾ)
    {
	member_not_ਮਿਲਿਆ_ਸੁਨੇਹਾ(cl, v_type, ਕੁੰਜੀ, *ਕੁੰਜੀ_ਸਿਰਾ - ਕੁੰਜੀ);
	ਵਾਪਸ FAIL;
    }

    ਵਾਪਸ OK;
}

/*
 * Check whether dot (".") is allowed after the variable "name" with type
 * "v_type".  Only Dict, Class and Object types support a dot after the name.
 * Returns TRUE if dot is allowed after the name.
 */
    ਸਥਿਰ ਪੂਰਨ
dot_allowed_after_type(char_u *name, vartype_T v_type, ਪੂਰਨ quiet)
{
    ਜੇ (v_type != VAR_DICT && v_type != VAR_OBJECT && v_type != VAR_CLASS)
    {
	ਜੇ (!quiet)
	    semsg(_(e_dot_not_allowed_after_str_str),
		    vartype_name(v_type), name);
	ਵਾਪਸ FALSE;
    }

    ਵਾਪਸ TRUE;
}

/*
 * Check whether left bracket ("[") is allowed after the variable "name" with
 * type "v_type".  Only Dict, List, Tuple and Blob types support a bracket
 * after the variable name.  Returns TRUE if bracket is allowed after the name.
 */
    ਸਥਿਰ ਪੂਰਨ
bracket_allowed_after_type(char_u *name, vartype_T v_type, ਪੂਰਨ quiet)
{
    ਜੇ (v_type == VAR_CLASS || v_type == VAR_OBJECT)
    {
	ਜੇ (!quiet)
	    semsg(_(e_ਸੂਚਕਾਂਕ_not_allowed_after_str_str),
		    vartype_name(v_type), name);
	ਵਾਪਸ FALSE;
    }

    ਵਾਪਸ TRUE;
}

/*
 * Check whether the variable "name" with type "v_type" can be followed by an
 * index.  Only Dict, List, Tuple, Blob, Object and Class types support
 * indexing.  Returns TRUE if indexing is allowed after the name.
 */
    ਸਥਿਰ ਪੂਰਨ
ਸੂਚਕਾਂਕ_allowed_after_type(char_u *name, vartype_T v_type, ਪੂਰਨ quiet)
{
    ਜੇ (v_type != VAR_LIST
	    && v_type != VAR_TUPLE
	    && v_type != VAR_DICT
	    && v_type != VAR_BLOB
	    && v_type != VAR_OBJECT
	    && v_type != VAR_CLASS)
    {
	ਜੇ (!quiet)
	    semsg(_(e_ਸੂਚਕਾਂਕ_not_allowed_after_str_str),
		    vartype_name(v_type), name);
	ਵਾਪਸ FALSE;
    }

    ਵਾਪਸ TRUE;
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
    ਸਥਿਰ char_u *
get_lval_subscript(
    lval_T	*lp,
    char_u	*p,
    char_u	*name,
    typval_T	*rettv,
    hashtab_T	*ht,
    dictitem_T	*v,
    ਪੂਰਨ		unlet,
    ਪੂਰਨ		flags,	    // GLV_ values
    class_T	*cl_exec)
{
    ਪੂਰਨ		quiet = flags & GLV_QUIET;
    char_u	*ਕੁੰਜੀ = ਨਿਰਾ;
    ਪੂਰਨ		len;
    typval_T	var1;
    typval_T	var2;
    ਪੂਰਨ		empty1 = FALSE;
    ਪੂਰਨ		rc = FAIL;

    /*
     * Loop until no more [idx] or .key is following.
     */
    var1.v_type = VAR_UNKNOWN;
    var2.v_type = VAR_UNKNOWN;

    ਜਦੋਂ (*p == '[' || (*p == '.' && p[1] != '=' && p[1] != '.'))
    {
	vartype_T v_type = lp->ll_tv->v_type;

	ਜੇ (*p == '.' && !dot_allowed_after_type(name, v_type, quiet))
	    ਜਾਓ ਪੂਰਾ;

	ਜੇ (*p == '[' && !bracket_allowed_after_type(name, v_type, quiet))
	    ਜਾਓ ਪੂਰਾ;

	ਜੇ (!ਸੂਚਕਾਂਕ_allowed_after_type(name, v_type, quiet))
	    ਜਾਓ ਪੂਰਾ;

	// A NULL list/blob works like an empty list/blob, allocate one now.
	ਪੂਰਨ r = OK;
	ਜੇ (v_type == VAR_LIST && lp->ll_tv->vval.v_list == ਨਿਰਾ)
	    r = rettv_list_alloc(lp->ll_tv);
	ਨਹੀਂਤਾਂ ਜੇ (v_type == VAR_BLOB && lp->ll_tv->vval.v_blob == ਨਿਰਾ)
	    r = rettv_blob_alloc(lp->ll_tv);
	ਜੇ (r == FAIL)
	    ਜਾਓ ਪੂਰਾ;

	ਜੇ (lp->ll_range)
	{
	    ਜੇ (!quiet)
		emsg(_(e_ਟੁਕੜਾ_must_come_ਆਖਰੀ));
	    ਜਾਓ ਪੂਰਾ;
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ LOG_LOCKVAR
	ch_log(ਨਿਰਾ, "LKVAR: get_lval() loop: p: %s, type: %s", p,
		vartype_name(v_type));
#ਜੇ_ਅੰਤ

	ਜੇ (current_script_is_vim9() && lp->ll_valtype == ਨਿਰਾ
		&& v != ਨਿਰਾ
		&& lp->ll_tv == &v->di_tv
		&& ht != ਨਿਰਾ && ht == get_script_local_ht())
	{
	    svar_T  *sv = ਲੱਭੋ_typval_ਵਿੱਚ_script(lp->ll_tv, 0, TRUE);

	    // Vim9 script local variable: get the type
	    ਜੇ (sv != ਨਿਰਾ)
	    {
		lp->ll_valtype = sv->sv_type;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ LOG_LOCKVAR
		ch_log(ਨਿਰਾ, "LKVAR:    ... loop: vim9 assign type: %s",
			vartype_name(lp->ll_valtype->tt_type));
#ਜੇ_ਅੰਤ
	    }
	}

	len = -1;
	ਜੇ (*p == '.')
	{
	    ਕੁੰਜੀ = p + 1;

	    ਲਈ (len = 0; ASCII_ISALNUM(ਕੁੰਜੀ[len]) || ਕੁੰਜੀ[len] == '_'; ++len)
		;
	    ਜੇ (len == 0)
	    {
		ਜੇ (!quiet)
		    emsg(_(e_cannot_use_ਖਾਲੀ_ਥਾਵਾਂ_ਕੁੰਜੀ_for_dictionary));
		ਜਾਓ ਪੂਰਾ;
	    }
	    p = ਕੁੰਜੀ + len;
	}
	ਨਹੀਂਤਾਂ
	{
	    // Get the index [expr] or the first index [expr: ].
	    p = skipwhite(p + 1);
	    ਜੇ (*p == ':')
		empty1 = TRUE;
	    ਨਹੀਂਤਾਂ
	    {
		empty1 = FALSE;
		ਜੇ (eval1(&p, &var1, &EVALARG_EVALUATE) == FAIL)  // recursive!
		    ਜਾਓ ਪੂਰਾ;
		ਜੇ (tv_get_string_chk(&var1) == ਨਿਰਾ)
		    // not a number or string
		    ਜਾਓ ਪੂਰਾ;
		p = skipwhite(p);
	    }

	    // Optionally get the second index [ :expr].
	    ਜੇ (*p == ':')
	    {
		ਜੇ (v_type == VAR_DICT)
		{
		    ਜੇ (!quiet)
			emsg(_(e_cannot_ਟੁਕੜਾ_dictionary));
		    ਜਾਓ ਪੂਰਾ;
		}
		ਜੇ (v_type == VAR_TUPLE)
		{
		    ਜੇ (!quiet)
			emsg(_(e_cannot_ਟੁਕੜਾ_tuple));
		    ਜਾਓ ਪੂਰਾ;
		}
		ਜੇ (rettv != ਨਿਰਾ
			&& !(rettv->v_type == VAR_LIST
			    && rettv->vval.v_list != ਨਿਰਾ)
			&& !(rettv->v_type == VAR_BLOB
			    && rettv->vval.v_blob != ਨਿਰਾ))
		{
		    ਜੇ (!quiet)
			emsg(_(e_ਟੁਕੜਾ_requires_list_or_blob_ਮੁੱਲ));
		    ਜਾਓ ਪੂਰਾ;
		}
		p = skipwhite(p + 1);
		ਜੇ (*p == ']')
		    lp->ll_empty2 = TRUE;
		ਨਹੀਂਤਾਂ
		{
		    lp->ll_empty2 = FALSE;
		    // recursive!
		    ਜੇ (eval1(&p, &var2, &EVALARG_EVALUATE) == FAIL)
			ਜਾਓ ਪੂਰਾ;
		    ਜੇ (tv_get_string_chk(&var2) == ਨਿਰਾ)
			// not a number or string
			ਜਾਓ ਪੂਰਾ;
		}
		lp->ll_range = TRUE;
	    }
	    ਨਹੀਂਤਾਂ
		lp->ll_range = FALSE;

	    ਜੇ (*p != ']')
	    {
		ਜੇ (!quiet)
		    emsg(_(e_missing_closing_ਵਰਗਫਲ_brace));
		ਜਾਓ ਪੂਰਾ;
	    }

	    // Skip to past ']'.
	    ++p;
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ LOG_LOCKVAR
	ਜੇ (len == -1)
	    ch_log(ਨਿਰਾ, "LKVAR:    ... loop: p: %s, '[' key: %s", p,
		    empty1 ? ":" : (ਅੱਖਰ*)tv_get_string(&var1));
	ਨਹੀਂਤਾਂ
	    ch_log(ਨਿਰਾ, "LKVAR:    ... loop: p: %s, '.' key: %s", p, ਕੁੰਜੀ);
#ਜੇ_ਅੰਤ

	ਜੇ (v_type == VAR_DICT)
	{
	    glv_ਸਥਿਤੀ_T glv_ਸਥਿਤੀ;

	    glv_ਸਥਿਤੀ = get_lval_dict_ਵਸਤੂ(lp, name, ਕੁੰਜੀ, len, &p, &var1,
							flags, unlet, rettv);
	    ਜੇ (glv_ਸਥਿਤੀ == GLV_FAIL)
		ਜਾਓ ਪੂਰਾ;
	    ਜੇ (glv_ਸਥਿਤੀ == GLV_STOP)
		ਰੋਕੋ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (v_type == VAR_BLOB)
	{
	    ਜੇ (get_lval_blob(lp, &var1, &var2, empty1, quiet) == FAIL)
		ਜਾਓ ਪੂਰਾ;

	    ਰੋਕੋ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (v_type == VAR_LIST)
	{
	    ਜੇ (get_lval_list(lp, &var1, &var2, empty1, flags, quiet) == FAIL)
		ਜਾਓ ਪੂਰਾ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (v_type == VAR_TUPLE)
	{
	    ਜੇ (get_lval_tuple(lp, &var1, quiet) == FAIL)
		ਜਾਓ ਪੂਰਾ;
	}
	ਨਹੀਂਤਾਂ  // v_type == VAR_CLASS || v_type == VAR_OBJECT
	{
	    ਜੇ (get_lval_class_or_obj(lp, ਕੁੰਜੀ, &p, v_type, cl_exec, flags,
							quiet) == FAIL)
		ਜਾਓ ਪੂਰਾ;
	}

	clear_tv(&var1);
	clear_tv(&var2);
	var1.v_type = VAR_UNKNOWN;
	var2.v_type = VAR_UNKNOWN;
    }

    ਜੇ (lp->ll_tuple != ਨਿਰਾ && (flags & GLV_READ_ONLY) == 0)
    {
	ਜੇ (!quiet)
	    emsg(_(e_tuple_is_immutable));
	ਜਾਓ ਪੂਰਾ;
    }

    rc = OK;

ਪੂਰਾ:
    clear_tv(&var1);
    clear_tv(&var2);
    ਵਾਪਸ rc == OK ? p : ਨਿਰਾ;
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
    ਪੂਰਨ		unlet,
    ਪੂਰਨ		skip,
    ਪੂਰਨ		flags,	    // GLV_ values
    ਪੂਰਨ		fne_flags)  // flags for find_name_end()
{
    char_u	*p;
    char_u	*ਵਿਅੰਜਕ_ਸ਼ੁਰੂ, *ਵਿਅੰਜਕ_ਸਿਰਾ;
    ਪੂਰਨ		cc;
    dictitem_T	*v = ਨਿਰਾ;
    hashtab_T	*ht = ਨਿਰਾ;
    ਪੂਰਨ		quiet = flags & GLV_QUIET;
    ਪੂਰਨ		writing = 0;
    ਪੂਰਨ		vim9script = ਵਿੱਚ_vim9script();
    class_T	*cl_exec = ਨਿਰਾ;    // class that is executing, or NULL.

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ LOG_LOCKVAR
    ਜੇ (lval_ਜੜ੍ਹ == ਨਿਰਾ)
	ch_log(ਨਿਰਾ, "LKVAR: get_lval(): name: %s, lval_root (nil)", name);
    ਨਹੀਂਤਾਂ
	ch_log(ਨਿਰਾ, "LKVAR: get_lval(): name: %s, lr_tv %p lr_is_arg %d",
			name, (ਖਾਲੀ*)lval_ਜੜ੍ਹ->lr_tv, lval_ਜੜ੍ਹ->lr_is_arg);
    ਅੱਖਰ buf[80];
    ch_log(ਨਿਰਾ, "LKVAR:    ...: GLV flags: %s",
		    flags_tostring(flags, glv_flag_ਲੜੀਆਂ, buf, ਆਕਾਰ(buf)));
#ਜੇ_ਅੰਤ

    // Clear everything in "lp".
    CLEAR_ਸੂਚਕ(lp);
    ga_init2(&lp->ll_type_list, ਆਕਾਰ(type_T *), 10);

    ਜੇ (skip || (flags & GLV_COMPILING))
    {
	// When skipping or compiling just find the end of the name.
	lp->ll_name = name;
	lp->ll_name_ਸਿਰਾ = ਲੱਭੋ_name_ਸਿਰਾ(name, ਨਿਰਾ, ਨਿਰਾ,
						      FNE_INCL_BR | fne_flags);
	ਵਾਪਸ lp->ll_name_ਸਿਰਾ;
    }

    // Cannot use "s:var" at the Vim9 script level.  "s: type" is OK.
    ਜੇ (vim9script && at_script_level()
		  && name[0] == 's' && name[1] == ':' && !VIM_ISWHITE(name[2]))
    {
	semsg(_(e_cannot_use_s_colon_ਵਿੱਚ_vim9_script_str), name);
	ਵਾਪਸ ਨਿਰਾ;
    }

    // Find the end of the name.
    p = ਲੱਭੋ_name_ਸਿਰਾ(name, &ਵਿਅੰਜਕ_ਸ਼ੁਰੂ, &ਵਿਅੰਜਕ_ਸਿਰਾ, fne_flags);
    lp->ll_name_ਸਿਰਾ = p;
    ਜੇ (ਵਿਅੰਜਕ_ਸ਼ੁਰੂ != ਨਿਰਾ)
    {
	// Don't expand the name when we already know there is an error.
	ਜੇ (unlet && !VIM_ISWHITE(*p) && !ਸਿਰੇ_excmd(*p)
						    && *p != '[' && *p != '.')
	{
	    semsg(_(e_trailing_characters_str), p);
	    ਵਾਪਸ ਨਿਰਾ;
	}

	lp->ll_exp_name = make_expanded_name(name, ਵਿਅੰਜਕ_ਸ਼ੁਰੂ, ਵਿਅੰਜਕ_ਸਿਰਾ, p);
	ਜੇ (lp->ll_exp_name == ਨਿਰਾ)
	{
	    // Report an invalid expression in braces, unless the
	    // expression evaluation has been cancelled due to an
	    // aborting error, an interrupt, or an exception.
	    ਜੇ (!aborting() && !quiet)
	    {
		emsg_severe = TRUE;
		semsg(_(e_invalid_ਦਲੀਲ_str), name);
		ਵਾਪਸ ਨਿਰਾ;
	    }
	}
	lp->ll_name = lp->ll_exp_name;
    }
    ਨਹੀਂਤਾਂ
    {
	lp->ll_name = name;

	ਜੇ (vim9script)
	{
	    // "a: type" is declaring variable "a" with a type, not "a:".
	    // However, "g:[key]" is indexing a dictionary.
	    ਜੇ (p == name + 2 && p[-1] == ':' && *p != '[')
	    {
		--p;
		lp->ll_name_ਸਿਰਾ = p;
	    }
	    ਜੇ (*skipwhite(p) == ':')
	    {
		char_u	    *tp = skipwhite(p + 1);

		ਜੇ (is_scoped_variable(name))
		{
		    semsg(_(e_cannot_use_type_with_this_variable_str), name);
		    ਵਾਪਸ ਨਿਰਾ;
		}
		ਜੇ (VIM_ISWHITE(*p))
		{
		    semsg(_(e_no_white_space_allowed_before_colon_str), p);
		    ਵਾਪਸ ਨਿਰਾ;
		}
		ਜੇ (tp == p + 1 && !quiet)
		{
		    semsg(_(e_white_space_required_after_str_str), ":", p);
		    ਵਾਪਸ ਨਿਰਾ;
		}
		ਜੇ (!SCRIPT_ID_VALID(current_sctx.sc_sid))
		{
		    semsg(_(e_using_type_not_ਵਿੱਚ_script_context_str), p);
		    ਵਾਪਸ ਨਿਰਾ;
		}
		ਜੇ (vim9script && (flags & GLV_NO_DECL) &&
			!(flags & GLV_FOR_LOOP))
		{
		    // Using a type and not in a "var" declaration.
		    semsg(_(e_trailing_characters_str), p);
		    ਵਾਪਸ ਨਿਰਾ;
		}


		// parse the type after the name
		lp->ll_type = parse_type(&tp,
			       &SCRIPT_ਵਸਤੂ(current_sctx.sc_sid)->sn_type_list,
			       ਨਿਰਾ, ਨਿਰਾ, !quiet);
		ਜੇ (!quiet && (lp->ll_type == ਨਿਰਾ
			    || !valid_declaration_type(lp->ll_type)))
		    ਵਾਪਸ ਨਿਰਾ;
		lp->ll_name_ਸਿਰਾ = tp;
	    }
	    // TODO: check inside class?
	}
    }
    ਜੇ (lp->ll_name == ਨਿਰਾ)
	ਵਾਪਸ p;

    ਜੇ (*p == '.')
    {
	// In legacy script, when a local variable and import exists with this name,
	// prioritize local variable over imports to avoid conflicts.
	ਪੂਰਨ var_exists = FALSE;
	ਜੇ (!vim9script)
	{
	    cc = *p;
	    *p = NUL;
	    hashtab_T *local_ht = get_funccal_local_ht();
	    ਜੇ (local_ht != ਨਿਰਾ)
	    {
		hashitem_T *hi = hash_ਲੱਭੋ(local_ht, lp->ll_name);
		ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
		    var_exists = TRUE;
	    }
	    *p = cc;
	}

	ਜੇ (!var_exists)
	{
	    imported_T *import = ਲੱਭੋ_imported(lp->ll_name, p - lp->ll_name, TRUE);
	    ਜੇ (import != ਨਿਰਾ)
	    {
		p++;	// skip '.'
		p = get_lval_imported(lp, import->imp_sid, p, &v, fne_flags);
		ਜੇ (p == ਨਿਰਾ)
		    ਵਾਪਸ ਨਿਰਾ;
	    }
	}
    }

    // Without [idx] or .key we are done.
    ਜੇ (*p != '[' && *p != '.')
    {
	ਜੇ (lval_ਜੜ੍ਹ != ਨਿਰਾ)
	    fill_lval_from_lval_ਜੜ੍ਹ(lp, lval_ਜੜ੍ਹ);
	ਵਾਪਸ p;
    }

    ਜੇ (vim9script && lval_ਜੜ੍ਹ != ਨਿਰਾ)
	cl_exec = lval_ਜੜ੍ਹ->lr_cl_exec;
    ਜੇ (vim9script && lval_ਜੜ੍ਹ != ਨਿਰਾ && lval_ਜੜ੍ਹ->lr_tv != ਨਿਰਾ)
    {
	// using local variable
	lp->ll_tv = lval_ਜੜ੍ਹ->lr_tv;
	v = ਨਿਰਾ;
    }
    ਨਹੀਂਤਾਂ ਜੇ (lp->ll_tv == ਨਿਰਾ)
    {
	cc = *p;
	*p = NUL;
	// When we would write to the variable pass &ht and prevent autoload.
	writing = !(flags & GLV_READ_ONLY);
	v = ਲੱਭੋ_var(lp->ll_name, writing ? &ht : ਨਿਰਾ,
					 (flags & GLV_NO_AUTOLOAD) || writing);
	ਜੇ (v == ਨਿਰਾ && !quiet)
	    semsg(_(e_undefined_variable_str), lp->ll_name);
	*p = cc;
	ਜੇ (v == ਨਿਰਾ)
	    ਵਾਪਸ ਨਿਰਾ;
	lp->ll_tv = &v->di_tv;
    }

    ਜੇ (vim9script && (flags & GLV_NO_DECL) == 0)
    {
	ਜੇ (!quiet)
	    semsg(_(e_variable_already_declared_str), lp->ll_name);
	ਵਾਪਸ ਨਿਰਾ;
    }

    // If the next character is a "." or a "[", then process the subitem.
    p = get_lval_subscript(lp, p, name, rettv, ht, v, unlet, flags, cl_exec);
    ਜੇ (p == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (vim9script && lp->ll_valtype != ਨਿਰਾ && rettv != ਨਿਰਾ)
    {
	where_T	    where = WHERE_INIT;

	// In a Vim9 script, do type check and make sure the variable is
	// writable.
	ਜੇ (check_typval_type(lp->ll_valtype, rettv, where) == FAIL)
	    ਵਾਪਸ ਨਿਰਾ;
    }

    lp->ll_name_ਸਿਰਾ = p;
    ਵਾਪਸ p;
}

/*
 * Clear lval "lp" that was filled by get_lval().
 */
    ਖਾਲੀ
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
    ਖਾਲੀ
ਸੈੱਟ_var_lval(
    lval_T	*lp,
    char_u	*endp,
    typval_T	*rettv,
    ਪੂਰਨ		copy,
    ਪੂਰਨ		flags,	    // ASSIGN_CONST, ASSIGN_NO_DECL
    char_u	*op,
    ਪੂਰਨ		var_idx)    // index for "let [a, b] = list"
{
    ਪੂਰਨ		cc;
    dictitem_T	*di;

    ਜੇ (lp->ll_tv == ਨਿਰਾ)
    {
	cc = *endp;
	*endp = NUL;
	ਜੇ (ਵਿੱਚ_vim9script() && check_reserved_name(lp->ll_name, FALSE) == FAIL)
	    ਵਾਪਸ;

	ਜੇ (lp->ll_blob != ਨਿਰਾ)
	{
	    ਪੂਰਨ	    error = FALSE;

	    ਜੇ (op != ਨਿਰਾ && *op != '=')
	    {
		semsg(_(e_wrong_variable_type_for_str_equal), op);
		ਵਾਪਸ;
	    }
	    ਜੇ (ਮੁੱਲ_check_ਤਾਲਾ(lp->ll_blob->bv_ਤਾਲਾ, lp->ll_name, FALSE))
		ਵਾਪਸ;

	    ਜੇ (lp->ll_range && rettv->v_type == VAR_BLOB)
	    {
		ਜੇ (lp->ll_empty2)
		    lp->ll_n2 = blob_len(lp->ll_blob) - 1;

		ਜੇ (blob_ਸੈੱਟ_range(lp->ll_blob, lp->ll_n1, lp->ll_n2,
								rettv) == FAIL)
		    ਵਾਪਸ;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		varnumber_T	val = tv_get_ਸੰਖਿਆ_chk(rettv, &error);
		ਜੇ (!error)
		{
		    ਜੇ (val < 0 || val > 255)
			semsg(_(e_invalid_ਮੁੱਲ_for_blob_nr), val);
		    ਨਹੀਂਤਾਂ
			blob_ਸੈੱਟ_append(lp->ll_blob, lp->ll_n1, val);
		}
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (op != ਨਿਰਾ && *op != '=')
	{
	    typval_T tv;

	    ਜੇ ((flags & (ਸੌਂਪੋ_CONST | ਸੌਂਪੋ_FINAL))
					     && (flags & ਸੌਂਪੋ_FOR_LOOP) == 0)
	    {
		emsg(_(e_cannot_modify_existing_variable));
		*endp = cc;
		ਵਾਪਸ;
	    }

	    // handle +=, -=, *=, /=, %= and .=
	    di = ਨਿਰਾ;
	    ਜੇ (eval_variable(lp->ll_name, (ਪੂਰਨ)STRLEN(lp->ll_name),
				 lp->ll_sid, &tv, &di, EVAL_VAR_VERBOSE) == OK)
	    {
		ਜੇ (di != ਨਿਰਾ && check_typval_is_ਮੁੱਲ(&di->di_tv) == FAIL)
		{
		    clear_tv(&tv);
		    ਵਾਪਸ;
		}

		// In Vim9 script a String v: variable only takes a String.
		ਜੇ (ਵਿੱਚ_vim9script() && STRNCMP(lp->ll_name, "v:", 2) == 0
			&& tv.v_type == VAR_STRING)
		{
		    where_T	where = WHERE_INIT;

		    ਜੇ (check_typval_type(&t_string, rettv, where) == FAIL)
		    {
			clear_tv(&tv);
			*endp = cc;
			ਵਾਪਸ;
		    }
		}

		ਜੇ ((di == ਨਿਰਾ
			 || (!var_check_ro(di->di_flags, lp->ll_name, FALSE)
			   && !tv_check_ਤਾਲਾ(&di->di_tv, lp->ll_name, FALSE)))
			&& tv_op(&tv, rettv, op) == OK)
		    ਸੈੱਟ_var_const(lp->ll_name, lp->ll_sid, ਨਿਰਾ, &tv, FALSE,
				ਸੌਂਪੋ_NO_DECL | ਸੌਂਪੋ_COMPOUND_OP, 0);
		clear_tv(&tv);
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (lp->ll_type != ਨਿਰਾ && check_typval_arg_type(lp->ll_type, rettv,
							      ਨਿਰਾ, 0) == FAIL)
		ਵਾਪਸ;
	    ਸੈੱਟ_var_const(lp->ll_name, lp->ll_sid, lp->ll_type, rettv, copy,
							       flags, var_idx);
	}
	*endp = cc;
    }
    ਨਹੀਂਤਾਂ ਜੇ (ਮੁੱਲ_check_ਤਾਲਾ(lp->ll_newkey == ਨਿਰਾ
		? lp->ll_tv->v_ਤਾਲਾ
		: lp->ll_tv->vval.v_dict->dv_ਤਾਲਾ, lp->ll_name, FALSE))
	;
    ਨਹੀਂਤਾਂ ਜੇ (lp->ll_range)
    {
	ਜੇ ((flags & (ਸੌਂਪੋ_CONST | ਸੌਂਪੋ_FINAL))
					     && (flags & ਸੌਂਪੋ_FOR_LOOP) == 0)
	{
	    emsg(_(e_cannot_ਤਾਲਾ_range));
	    ਵਾਪਸ;
	}

	(ਖਾਲੀ)list_ਸੌਂਪੋ_range(lp->ll_list, rettv->vval.v_list,
			 lp->ll_n1, lp->ll_n2, lp->ll_empty2, op, lp->ll_name);
    }
    ਨਹੀਂਤਾਂ
    {
	/*
	 * Assign to a List, Dictionary or Object item.
	 */
	ਜੇ ((flags & (ਸੌਂਪੋ_CONST | ਸੌਂਪੋ_FINAL))
					     && (flags & ਸੌਂਪੋ_FOR_LOOP) == 0)
	{
	    emsg(_(e_cannot_ਤਾਲਾ_list_or_dict));
	    ਵਾਪਸ;
	}

	ਜੇ (lp->ll_valtype != ਨਿਰਾ
		    && check_typval_arg_type(lp->ll_valtype, rettv,
							      ਨਿਰਾ, 0) == FAIL)
	{
	    lp->ll_name_ਸਿਰਾ = ਨਿਰਾ;
	    ਵਾਪਸ;
	}

	// If the lval is a List and the type of the list is not yet set,
	// then set the item type from the declared type of the variable.
	ਜੇ (ਵਿੱਚ_vim9script() && rettv->v_type == VAR_LIST
				&& rettv->vval.v_list != ਨਿਰਾ
				&& rettv->vval.v_list->lv_type == ਨਿਰਾ)
	{
	    ਜੇ (lp->ll_tv->v_type == VAR_LIST
		    && lp->ll_tv->vval.v_list != ਨਿਰਾ
		    && lp->ll_tv->vval.v_list->lv_type != ਨਿਰਾ)
		ਸੈੱਟ_tv_type(rettv, lp->ll_tv->vval.v_list->lv_type);
	}

	ਜੇ (lp->ll_newkey != ਨਿਰਾ)
	{
	    ਜੇ (op != ਨਿਰਾ && *op != '=')
	    {
		semsg(_(e_ਕੁੰਜੀ_not_present_ਵਿੱਚ_dictionary_str), lp->ll_newkey);
		ਵਾਪਸ;
	    }
	    ਜੇ (dict_wrong_func_name(lp->ll_tv->vval.v_dict, rettv,
								lp->ll_newkey))
		ਵਾਪਸ;

	    // Need to add an item to the Dictionary.
	    di = dictitem_alloc(lp->ll_newkey);
	    ਜੇ (di == ਨਿਰਾ)
		ਵਾਪਸ;
	    ਜੇ (dict_ਜੋੜੋ(lp->ll_tv->vval.v_dict, di) == FAIL)
	    {
		vim_free(di);
		ਵਾਪਸ;
	    }
	    lp->ll_tv = &di->di_tv;
	}
	ਨਹੀਂਤਾਂ ਜੇ (op != ਨਿਰਾ && *op != '=')
	{
	    tv_op(lp->ll_tv, rettv, op);
	    ਵਾਪਸ;
	}
	ਨਹੀਂਤਾਂ
	    clear_tv(lp->ll_tv);

	/*
	 * Assign the value to the variable or list item.
	 */
	ਜੇ (copy)
	    copy_tv(rettv, lp->ll_tv);
	ਨਹੀਂਤਾਂ
	{
	    *lp->ll_tv = *rettv;
	    lp->ll_tv->v_ਤਾਲਾ = 0;
	    init_tv(rettv);
	}
    }
}

/*
 * Handle "blob1 += blob2".
 * Returns OK or FAIL.
 */
    ਸਥਿਰ ਪੂਰਨ
tv_op_blob(typval_T *tv1, typval_T *tv2, char_u *op)
{
    ਜੇ (*op != '+' || tv2->v_type != VAR_BLOB)
	ਵਾਪਸ FAIL;

    // Blob += Blob
    ਜੇ (tv2->vval.v_blob == ਨਿਰਾ)
	ਵਾਪਸ OK;

    ਜੇ (tv1->vval.v_blob == ਨਿਰਾ)
    {
	tv1->vval.v_blob = tv2->vval.v_blob;
	++tv1->vval.v_blob->bv_refcount;
	ਵਾਪਸ OK;
    }

    blob_T	*b1 = tv1->vval.v_blob;
    blob_T	*b2 = tv2->vval.v_blob;
    ਪੂਰਨ		len = blob_len(b2);

    ਜੇ (len > 0 && ga_grow(&b1->bv_ga, len) == OK)
    {
	mch_memmove((char_u *)b1->bv_ga.ga_ਅੰਕੜੇ + b1->bv_ga.ga_len,
		    (char_u *)b2->bv_ga.ga_ਅੰਕੜੇ, (size_t)len);
	b1->bv_ga.ga_len += len;
    }
    ਨਹੀਂਤਾਂ
	ਲਈ (ਪੂਰਨ i = 0; i < len; i++)
	    (ਖਾਲੀ)ga_append(&b1->bv_ga, blob_get(b2, i));

    ਵਾਪਸ OK;
}

/*
 * Handle "list1 += list2".
 * Returns OK or FAIL.
 */
    ਸਥਿਰ ਪੂਰਨ
tv_op_list(typval_T *tv1, typval_T *tv2, char_u *op)
{
    ਜੇ (*op != '+' || tv2->v_type != VAR_LIST)
	ਵਾਪਸ FAIL;

    // List += List
    ਜੇ (tv2->vval.v_list == ਨਿਰਾ)
	ਵਾਪਸ OK;

    ਜੇ (tv1->vval.v_list == ਨਿਰਾ)
    {
	tv1->vval.v_list = tv2->vval.v_list;
	++tv1->vval.v_list->lv_refcount;
    }
    ਨਹੀਂਤਾਂ
	list_extend(tv1->vval.v_list, tv2->vval.v_list, ਨਿਰਾ);

    ਵਾਪਸ OK;
}

/*
 * Handle number operations:
 *	nr += nr , nr -= nr , nr *=nr , nr /= nr , nr %= nr
 *
 * Returns OK or FAIL.
 */
    ਸਥਿਰ ਪੂਰਨ
tv_op_ਸੰਖਿਆ(typval_T *tv1, typval_T *tv2, char_u *op)
{
    varnumber_T	n;
    ਪੂਰਨ		failed = FALSE;

    n = tv_get_ਸੰਖਿਆ(tv1);
    ਜੇ (tv2->v_type == VAR_FLOAT)
    {
	float_T f = n;

	ਜੇ (*op == '%')
	    ਵਾਪਸ FAIL;
	ਚੋਣ (*op)
	{
	    ਹਾਲਤ '+': f += tv2->vval.v_float; ਰੋਕੋ;
	    ਹਾਲਤ '-': f -= tv2->vval.v_float; ਰੋਕੋ;
	    ਹਾਲਤ '*': f *= tv2->vval.v_float; ਰੋਕੋ;
	    ਹਾਲਤ '/': f /= tv2->vval.v_float; ਰੋਕੋ;
	}
	clear_tv(tv1);
	tv1->v_type = VAR_FLOAT;
	tv1->vval.v_float = f;
    }
    ਨਹੀਂਤਾਂ
    {
	ਚੋਣ (*op)
	{
	    ਹਾਲਤ '+': n += tv_get_ਸੰਖਿਆ(tv2); ਰੋਕੋ;
	    ਹਾਲਤ '-': n -= tv_get_ਸੰਖਿਆ(tv2); ਰੋਕੋ;
	    ਹਾਲਤ '*': n *= tv_get_ਸੰਖਿਆ(tv2); ਰੋਕੋ;
	    ਹਾਲਤ '/': n = ਅੰਕ_divide(n, tv_get_ਸੰਖਿਆ(tv2), &failed); ਰੋਕੋ;
	    ਹਾਲਤ '%': n = ਅੰਕ_modulus(n, tv_get_ਸੰਖਿਆ(tv2), &failed); ਰੋਕੋ;
	}
	clear_tv(tv1);
	tv1->v_type = VAR_ਸੰਖਿਆ;
	tv1->vval.v_ਸੰਖਿਆ = n;
    }

    ਵਾਪਸ failed ? FAIL : OK;
}

/*
 * Append string "s2" to the string in "tv1".
 * Returns OK if "tv1" was grown in place, FAIL otherwise.
 */
    ਸਥਿਰ ਪੂਰਨ
grow_string_tv(typval_T *tv1, char_u *s2)
{
    ਜੇ (tv1->v_type != VAR_STRING || tv1->vval.v_string == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    size_t len1 = STRLEN(tv1->vval.v_string);
    size_t len2 = STRLEN(s2);
    char_u *p = vim_realloc(tv1->vval.v_string, len1 + len2 + 1);

    ਜੇ (p == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    mch_memmove(p + len1, s2, len2 + 1);
    tv1->vval.v_string = p;
    ਵਾਪਸ OK;
}

/*
 * Handle "str1 .= str2"
 * Returns OK or FAIL.
 */
    ਸਥਿਰ ਪੂਰਨ
tv_op_string(typval_T *tv1, typval_T *tv2, char_u *op ਅਣਵਰਤਿਆ)
{
    char_u	numbuf[NUMBUFLEN];
    char_u	*s;
    char_u	*s2;

    ਜੇ (tv2->v_type == VAR_FLOAT)
	ਵਾਪਸ FAIL;

    // str .= str
    s2 = tv_get_string_buf(tv2, numbuf);
    ਜੇ (grow_string_tv(tv1, s2) == OK)
	ਵਾਪਸ OK;

    s = tv_get_string(tv1);
    s = concat_str(s, s2);
    clear_tv(tv1);
    tv1->v_type = VAR_STRING;
    tv1->vval.v_string = s;

    ਵਾਪਸ OK;
}

/*
 * Handle "tv1 += tv2", "tv1 -= tv2", "tv1 *= tv2", "tv1 /= tv2", "tv1 %= tv2"
 * and "tv1 .= tv2"
 * Returns OK or FAIL.
 */
    ਸਥਿਰ ਪੂਰਨ
tv_op_nr_or_string(typval_T *tv1, typval_T *tv2, char_u *op)
{
    ਜੇ (tv2->v_type == VAR_LIST)
	ਵਾਪਸ FAIL;

    ਜੇ (vim_strchr((char_u *)"+-*/%", *op) != ਨਿਰਾ)
	ਵਾਪਸ tv_op_ਸੰਖਿਆ(tv1, tv2, op);

    ਵਾਪਸ tv_op_string(tv1, tv2, op);
}

/*
 * Handle "f1 += f2", "f1 -= f2", "f1 *= f2", "f1 /= f2".
 * Returns OK or FAIL.
 */
    ਸਥਿਰ ਪੂਰਨ
tv_op_float(typval_T *tv1, typval_T *tv2, char_u *op)
{
    float_T f;

    ਜੇ (*op == '%' || *op == '.'
	    || (tv2->v_type != VAR_FLOAT
		&& tv2->v_type != VAR_ਸੰਖਿਆ
		&& tv2->v_type != VAR_STRING))
	ਵਾਪਸ FAIL;

    ਜੇ (tv2->v_type == VAR_FLOAT)
	f = tv2->vval.v_float;
    ਨਹੀਂਤਾਂ
	f = tv_get_ਸੰਖਿਆ(tv2);
    ਚੋਣ (*op)
    {
	ਹਾਲਤ '+': tv1->vval.v_float += f; ਰੋਕੋ;
	ਹਾਲਤ '-': tv1->vval.v_float -= f; ਰੋਕੋ;
	ਹਾਲਤ '*': tv1->vval.v_float *= f; ਰੋਕੋ;
	ਹਾਲਤ '/': tv1->vval.v_float /= f; ਰੋਕੋ;
    }

    ਵਾਪਸ OK;
}

/*
 * Handle "tv1 += tv2", "tv1 -= tv2", "tv1 *= tv2", "tv1 /= tv2", "tv1 %= tv2"
 * and "tv1 .= tv2"
 * Returns OK or FAIL.
 */
    ਪੂਰਨ
tv_op(typval_T *tv1, typval_T *tv2, char_u *op)
{
    // Can't do anything with a Funcref or Dict on the right.
    // v:true and friends only work with "..=".
    ਜੇ (tv2->v_type == VAR_FUNC || tv2->v_type == VAR_DICT
		|| ((tv2->v_type == VAR_BOOL || tv2->v_type == VAR_SPECIAL)
							&& *op != '.'))
    {
	semsg(_(e_wrong_variable_type_for_str_equal), op);
	ਵਾਪਸ FAIL;
    }

    ਪੂਰਨ retval = FAIL;
    ਚੋਣ (tv1->v_type)
    {
	ਹਾਲਤ VAR_UNKNOWN:
	ਹਾਲਤ VAR_ANY:
	ਹਾਲਤ VAR_VOID:
	ਹਾਲਤ VAR_DICT:
	ਹਾਲਤ VAR_FUNC:
	ਹਾਲਤ VAR_PARTIAL:
	ਹਾਲਤ VAR_BOOL:
	ਹਾਲਤ VAR_SPECIAL:
	ਹਾਲਤ VAR_ਕੰਮ:
	ਹਾਲਤ VAR_CHANNEL:
	ਹਾਲਤ VAR_INSTR:
	ਹਾਲਤ VAR_OBJECT:
	ਹਾਲਤ VAR_CLASS:
	ਹਾਲਤ VAR_TYPEALIAS:
	ਹਾਲਤ VAR_TUPLE:
	    ਰੋਕੋ;

	ਹਾਲਤ VAR_BLOB:
	    retval = tv_op_blob(tv1, tv2, op);
	    ਰੋਕੋ;

	ਹਾਲਤ VAR_LIST:
	    retval = tv_op_list(tv1, tv2, op);
	    ਰੋਕੋ;

	ਹਾਲਤ VAR_ਸੰਖਿਆ:
	ਹਾਲਤ VAR_STRING:
	    retval = tv_op_nr_or_string(tv1, tv2, op);
	    ਰੋਕੋ;

	ਹਾਲਤ VAR_FLOAT:
	    retval = tv_op_float(tv1, tv2, op);
	    ਰੋਕੋ;
    }

    ਜੇ (retval != OK)
	semsg(_(e_wrong_variable_type_for_str_equal), op);

    ਵਾਪਸ retval;
}

/*
 * Evaluate the expression used in a ":for var in expr" command.
 * "arg" points to "var".
 * Set "*errp" to TRUE for an error, FALSE otherwise;
 * Return a pointer that holds the info.  Null when there is an error.
 */
    ਖਾਲੀ *
eval_for_ਸਤਰ(
    char_u	*arg,
    ਪੂਰਨ		*errp,
    exarg_T	*eap,
    evalarg_T	*evalarg)
{
    forinfo_T	*fi;
    char_u	*var_list_ਸਿਰਾ;
    char_u	*ਵਿਅੰਜਕ;
    typval_T	tv;
    list_T	*l;
    tuple_T	*tuple;
    ਪੂਰਨ		skip = !(evalarg->eval_flags & EVAL_EVALUATE);

    *errp = TRUE;	// default: there is an error

    fi = ALLOC_CLEAR_ONE(forinfo_T);
    ਜੇ (fi == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    var_list_ਸਿਰਾ = skip_var_list(arg, TRUE, &fi->fi_varcount,
						     &fi->fi_semicolon, FALSE);
    ਜੇ (var_list_ਸਿਰਾ == ਨਿਰਾ)
	ਵਾਪਸ fi;

    ਵਿਅੰਜਕ = skipwhite_and_linebreak(var_list_ਸਿਰਾ, evalarg);
    ਜੇ (ਵਿਅੰਜਕ[0] != 'i' || ਵਿਅੰਜਕ[1] != 'n'
				  || !(ਵਿਅੰਜਕ[2] == NUL || VIM_ISWHITE(ਵਿਅੰਜਕ[2])))
    {
	ਜੇ (ਵਿੱਚ_vim9script() && *ਵਿਅੰਜਕ == ':' && ਵਿਅੰਜਕ != var_list_ਸਿਰਾ)
	    semsg(_(e_no_white_space_allowed_before_colon_str), ਵਿਅੰਜਕ);
	ਨਹੀਂਤਾਂ
	    emsg(_(e_missing_ਵਿੱਚ_after_for));
	ਵਾਪਸ fi;
    }

    ਜੇ (skip)
	++emsg_skip;
    ਵਿਅੰਜਕ = skipwhite_and_linebreak(ਵਿਅੰਜਕ + 2, evalarg);
    ਜੇ (eval0(ਵਿਅੰਜਕ, &tv, eap, evalarg) == OK)
    {
	*errp = FALSE;
	ਜੇ (!skip)
	{
	    ਜੇ (tv.v_type == VAR_LIST)
	    {
		l = tv.vval.v_list;
		ਜੇ (l == ਨਿਰਾ)
		{
		    // a null list is like an empty list: do nothing
		    clear_tv(&tv);
		}
		ਨਹੀਂਤਾਂ
		{
		    // Need a real list here.
		    CHECK_LIST_MATERIALIZE(l);

		    // No need to increment the refcount, it's already set for
		    // the list being used in "tv".
		    fi->fi_list = l;
		    list_ਜੋੜੋ_watch(l, &fi->fi_lw);
		    fi->fi_lw.lw_ਵਸਤੂ = l->lv_ਪਹਿਲਾ;
		}
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (tv.v_type == VAR_TUPLE)
	    {
		tuple = tv.vval.v_tuple;
		ਜੇ (tuple == ਨਿਰਾ)
		{
		    // a null tuple is like an empty tuple: do nothing
		    clear_tv(&tv);
		}
		ਨਹੀਂਤਾਂ
		{
		    // No need to increment the refcount, it's already set for
		    // the tuple being used in "tv".
		    fi->fi_tuple = tuple;
		    fi->fi_tuple_idx = 0;
		}
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (tv.v_type == VAR_BLOB)
	    {
		fi->fi_bi = 0;
		ਜੇ (tv.vval.v_blob != ਨਿਰਾ)
		{
		    // Make a copy, so that the iteration still works when the
		    // blob is changed.
		    fi->fi_blob = blob_copy(tv.vval.v_blob);
		}
		clear_tv(&tv);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (tv.v_type == VAR_STRING)
	    {
		fi->fi_byte_idx = 0;
		fi->fi_string = tv.vval.v_string;
		tv.vval.v_string = ਨਿਰਾ;
		ਜੇ (fi->fi_string == ਨਿਰਾ)
		    fi->fi_string = vim_strsave((char_u *)"");
	    }
	    ਨਹੀਂਤਾਂ
	    {
		emsg(_(e_string_list_tuple_or_blob_required));
		clear_tv(&tv);
	    }
	}
    }
    ਜੇ (skip)
	--emsg_skip;
    fi->fi_break_ਗਿਣਤੀ = evalarg->eval_break_ਗਿਣਤੀ;

    ਵਾਪਸ fi;
}

/*
 * Used when looping over a :for line, skip the "in expr" part.
 */
    ਖਾਲੀ
skip_for_ਸਤਰਾਂ(ਖਾਲੀ *fi_void, evalarg_T *evalarg)
{
    forinfo_T	*fi = (forinfo_T *)fi_void;
    ਪੂਰਨ		i;

    ਲਈ (i = 0; i < fi->fi_break_ਗਿਣਤੀ; ++i)
	eval_ਅਗਲਾ_ਸਤਰ(ਨਿਰਾ, evalarg);
}

/*
 * Use the first item in a ":for" list.  Advance to the next.
 * Assign the values to the variable (list).  "arg" points to the first one.
 * Return TRUE when a valid item was found, FALSE when at end of list or
 * something wrong.
 */
    ਪੂਰਨ
ਅਗਲਾ_for_ਵਸਤੂ(ਖਾਲੀ *fi_void, char_u *arg)
{
    forinfo_T	*fi = (forinfo_T *)fi_void;
    ਪੂਰਨ		ਨਤੀਜਾ;
    ਪੂਰਨ		flag = ਸੌਂਪੋ_FOR_LOOP | (ਵਿੱਚ_vim9script()
			 ? (ਸੌਂਪੋ_FINAL
			     // first round: error if variable exists
			     | (fi->fi_bi == 0 ? 0 : ਸੌਂਪੋ_DECL)
			     | ਸੌਂਪੋ_NO_MEMBER_TYPE
			     | ਸੌਂਪੋ_UPDATE_ਖੰਡ_ID)
			 : 0);
    listitem_T	*ਵਸਤੂ;
    ਪੂਰਨ		skip_ਸੌਂਪੋ = ਵਿੱਚ_vim9script() && arg[0] == '_'
						      && !eval_isnamec(arg[1]);

    ਜੇ (fi->fi_blob != ਨਿਰਾ)
    {
	typval_T	tv;

	ਜੇ (fi->fi_bi >= blob_len(fi->fi_blob))
	    ਵਾਪਸ FALSE;
	tv.v_type = VAR_ਸੰਖਿਆ;
	tv.v_ਤਾਲਾ = VAR_FIXED;
	tv.vval.v_ਸੰਖਿਆ = blob_get(fi->fi_blob, fi->fi_bi);
	++fi->fi_bi;
	ਜੇ (skip_ਸੌਂਪੋ)
	    ਵਾਪਸ TRUE;
	ਵਾਪਸ ex_let_vars(arg, &tv, TRUE, fi->fi_semicolon,
					    fi->fi_varcount, flag, ਨਿਰਾ) == OK;
    }

    ਜੇ (fi->fi_string != ਨਿਰਾ)
    {
	typval_T	tv;
	ਪੂਰਨ		len;

	len = mb_ptr2len(fi->fi_string + fi->fi_byte_idx);
	ਜੇ (len == 0)
	    ਵਾਪਸ FALSE;
	tv.v_type = VAR_STRING;
	tv.v_ਤਾਲਾ = VAR_FIXED;
	tv.vval.v_string = vim_strnsave(fi->fi_string + fi->fi_byte_idx, len);
	fi->fi_byte_idx += len;
	++fi->fi_bi;
	ਜੇ (skip_ਸੌਂਪੋ)
	    ਨਤੀਜਾ = TRUE;
	ਨਹੀਂਤਾਂ
	    ਨਤੀਜਾ = ex_let_vars(arg, &tv, TRUE, fi->fi_semicolon,
					    fi->fi_varcount, flag, ਨਿਰਾ) == OK;
	vim_free(tv.vval.v_string);
	ਵਾਪਸ ਨਤੀਜਾ;
    }

    ਜੇ (fi->fi_tuple != ਨਿਰਾ)
    {
	typval_T	tv;

	ਜੇ (fi->fi_tuple_idx >= TUPLE_LEN(fi->fi_tuple))
	    ਵਾਪਸ FALSE;

	copy_tv(TUPLE_ਵਸਤੂ(fi->fi_tuple, fi->fi_tuple_idx), &tv);
	++fi->fi_tuple_idx;
	++fi->fi_bi;
	ਜੇ (skip_ਸੌਂਪੋ)
	    ਨਤੀਜਾ = TRUE;
	ਨਹੀਂਤਾਂ
	    ਨਤੀਜਾ = ex_let_vars(arg, &tv, TRUE, fi->fi_semicolon,
					    fi->fi_varcount, flag, ਨਿਰਾ) == OK;
	clear_tv(&tv);
	ਵਾਪਸ ਨਤੀਜਾ;
    }

    ਵਸਤੂ = fi->fi_lw.lw_ਵਸਤੂ;
    ਜੇ (ਵਸਤੂ == ਨਿਰਾ)
	ਨਤੀਜਾ = FALSE;
    ਨਹੀਂਤਾਂ
    {
	fi->fi_lw.lw_ਵਸਤੂ = ਵਸਤੂ->li_ਅਗਲਾ;
	++fi->fi_bi;
	ਜੇ (skip_ਸੌਂਪੋ)
	    ਨਤੀਜਾ = TRUE;
	ਨਹੀਂਤਾਂ
	    ਨਤੀਜਾ = (ex_let_vars(arg, &ਵਸਤੂ->li_tv, TRUE, fi->fi_semicolon,
					   fi->fi_varcount, flag, ਨਿਰਾ) == OK);
    }
    ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * Free the structure used to store info used by ":for".
 */
    ਖਾਲੀ
free_for_info(ਖਾਲੀ *fi_void)
{
    forinfo_T    *fi = (forinfo_T *)fi_void;

    ਜੇ (fi == ਨਿਰਾ)
	ਵਾਪਸ;
    ਜੇ (fi->fi_list != ਨਿਰਾ)
    {
	list_rem_watch(fi->fi_list, &fi->fi_lw);
	list_unref(fi->fi_list);
    }
    ਨਹੀਂਤਾਂ ਜੇ (fi->fi_blob != ਨਿਰਾ)
	blob_unref(fi->fi_blob);
    ਨਹੀਂਤਾਂ ਜੇ (fi->fi_tuple != ਨਿਰਾ)
	tuple_unref(fi->fi_tuple);
    ਨਹੀਂਤਾਂ
	vim_free(fi->fi_string);
    vim_free(fi);
}

    ਖਾਲੀ
ਸੈੱਟ_context_for_ਵਿਅੰਜਕ(
    expand_T	*xp,
    char_u	*arg,
    cmdidx_T	cmdidx)
{
    ਪੂਰਨ		has_ਵਿਅੰਜਕ = cmdidx != CMD_let && cmdidx != CMD_var;
    ਪੂਰਨ		c;
    char_u	*p;

    ਜੇ (cmdidx == CMD_let || cmdidx == CMD_var
				 || cmdidx == CMD_const || cmdidx == CMD_final)
    {
	xp->xp_context = EXPAND_USER_VARS;
	ਜੇ (vim_strpbrk(arg, (char_u *)"\"'+-*/%.=!?~|&$([<>,#") == ਨਿਰਾ)
	{
	    // ":let var1 var2 ...": find last space.
	    ਲਈ (p = arg + STRLEN(arg); p >= arg; )
	    {
		xp->xp_ਨਮੂਨਾ = p;
		MB_PTR_ਪਿੱਠ(arg, p);
		ਜੇ (VIM_ISWHITE(*p))
		    ਰੋਕੋ;
	    }
	    ਵਾਪਸ;
	}
    }
    ਨਹੀਂਤਾਂ
	xp->xp_context = cmdidx == CMD_call ? EXPAND_FUNCTIONS
							  : EXPAND_ਵਿਅੰਜਕ;
    ਜਦੋਂ ((xp->xp_ਨਮੂਨਾ = vim_strpbrk(arg,
				  (char_u *)"\"'+-*/%.=!?~|&$([<>,#")) != ਨਿਰਾ)
    {
	c = *xp->xp_ਨਮੂਨਾ;
	ਜੇ (c == '&')
	{
	    c = xp->xp_ਨਮੂਨਾ[1];
	    ਜੇ (c == '&')
	    {
		++xp->xp_ਨਮੂਨਾ;
		xp->xp_context = has_ਵਿਅੰਜਕ ? EXPAND_ਵਿਅੰਜਕ : EXPAND_NOTHING;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (c != ' ')
	    {
		xp->xp_context = EXPAND_SETTINGS;
		ਜੇ ((c == 'l' || c == 'g') && xp->xp_ਨਮੂਨਾ[2] == ':')
		    xp->xp_ਨਮੂਨਾ += 2;

	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (c == '$')
	{
	    // environment variable
	    xp->xp_context = EXPAND_ENV_VARS;
	}
	ਨਹੀਂਤਾਂ ਜੇ (c == '=')
	{
	    has_ਵਿਅੰਜਕ = TRUE;
	    xp->xp_context = EXPAND_ਵਿਅੰਜਕ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (c == '#'
		&& xp->xp_context == EXPAND_ਵਿਅੰਜਕ)
	{
	    // Autoload function/variable contains '#'.
	    ਰੋਕੋ;
	}
	ਨਹੀਂਤਾਂ ਜੇ ((c == '<' || c == '#')
		&& xp->xp_context == EXPAND_FUNCTIONS
		&& vim_strchr(xp->xp_ਨਮੂਨਾ, '(') == ਨਿਰਾ)
	{
	    // Function name can start with "<SNR>" and contain '#'.
	    ਰੋਕੋ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (has_ਵਿਅੰਜਕ)
	{
	    ਜੇ (c == '"')	    // string
	    {
		ਜਦੋਂ ((c = *++xp->xp_ਨਮੂਨਾ) != NUL && c != '"')
		    ਜੇ (c == '\\' && xp->xp_ਨਮੂਨਾ[1] != NUL)
			++xp->xp_ਨਮੂਨਾ;
		xp->xp_context = EXPAND_NOTHING;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (c == '\'')	    // literal string
	    {
		// Trick: '' is like stopping and starting a literal string.
		ਜਦੋਂ ((c = *++xp->xp_ਨਮੂਨਾ) != NUL && c != '\'')
		    /* skip */ ;
		xp->xp_context = EXPAND_NOTHING;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (c == '|')
	    {
		ਜੇ (xp->xp_ਨਮੂਨਾ[1] == '|')
		{
		    ++xp->xp_ਨਮੂਨਾ;
		    xp->xp_context = EXPAND_ਵਿਅੰਜਕ;
		}
		ਨਹੀਂਤਾਂ
		    xp->xp_context = EXPAND_COMMANDS;
	    }
	    ਨਹੀਂਤਾਂ
		xp->xp_context = EXPAND_ਵਿਅੰਜਕ;
	}
	ਨਹੀਂਤਾਂ
	    // Doesn't look like something valid, expand as an expression
	    // anyway.
	    xp->xp_context = EXPAND_ਵਿਅੰਜਕ;
	arg = xp->xp_ਨਮੂਨਾ;
	ਜੇ (*arg != NUL)
	    ਜਦੋਂ ((c = *++arg) != NUL && (c == ' ' || c == '\t'))
		/* skip */ ;
    }

    // ":exe one two" completes "two"
    ਜੇ ((cmdidx == CMD_execute
		|| cmdidx == CMD_echo
		|| cmdidx == CMD_echon
		|| cmdidx == CMD_echomsg
		|| cmdidx == CMD_echoerr
		|| cmdidx == CMD_echoconsole
		|| cmdidx == CMD_echowindow)
	    && xp->xp_context == EXPAND_ਵਿਅੰਜਕ)
    {
	ਲਈ (;;)
	{
	    char_u *n = skiptowhite(arg);

	    ਜੇ (n == arg || IS_WHITE_OR_NUL(*skipwhite(n)))
		ਰੋਕੋ;
	    arg = skipwhite(n);
	}
    }

    xp->xp_ਨਮੂਨਾ = arg;
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
ਸਥਿਰ regprog_T    *eval_prog_cache = ਨਿਰਾ;
ਸਥਿਰ char_u	    *eval_prog_pat = ਨਿਰਾ;	// pattern it was compiled for
ਸਥਿਰ char_u	    *eval_prog_enc = ਨਿਰਾ;	// 'encoding' when compiled
ਸਥਿਰ ਲੰਮਾ	    eval_prog_re;		// 'regexpengine' when compiled
// State sampled by eval_regcomp() for the eval_regfree() call that gives the
// program back: user code run in between, e.g. an object's string() method
// invoked by match() on a list, can change these options, and then the
// program must not be cached under the changed values.  eval_compile_enc is
// only ever compared against p_enc, never dereferenced.
ਸਥਿਰ ਲੰਮਾ	    eval_compile_re;
ਸਥਿਰ char_u	    *eval_compile_enc;
ਸਥਿਰ ਪੂਰਨ	    eval_prog_busy = 0;	// programs eval_regcomp() handed out
					// that were not given back yet
ਸਥਿਰ ਪੂਰਨ	    eval_prog_reentered = FALSE;  // eval_regcomp() ran while
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
    ਸਥਿਰ bool
eval_prog_volatile(char_u *pat)
{
    char_u *p;

    ਜੇ (STRNCMP(pat, "\\%#=", 4) == 0)
	ਵਾਪਸ true;
    ਲਈ (p = pat; *p != NUL; ++p)
	ਜੇ (*p == '~'
		|| (p[0] == '[' && (p[1] == ':' || p[1] == '=' || p[1] == '.'))
		|| (p[0] == '%' && (p[1] == '.'
			|| ((p[1] == '<' || p[1] == '>') && p[2] == '.'))))
	    ਵਾਪਸ true;
    ਵਾਪਸ false;
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
    ਜੇ (eval_prog_busy > 0)
	// The samples above no longer describe the program already handed
	// out: its eval_regfree() must drop it instead of caching it.
	eval_prog_reentered = TRUE;

    ਜੇ (eval_prog_cache != ਨਿਰਾ
	    && *p_cpo == NUL
	    && eval_prog_cache->re_flags == RE_MAGIC + RE_STRING
	    && eval_prog_re == p_re
	    && STRCMP(eval_prog_pat, pat) == 0
	    && STRCMP(eval_prog_enc, p_enc) == 0)
    {
	prog = eval_prog_cache;
	// The caller now owns the program, eval_regfree() adopts the
	// program the caller ends up with.
	eval_prog_cache = ਨਿਰਾ;
    }
    ਨਹੀਂਤਾਂ
	prog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
    ਜੇ (prog != ਨਿਰਾ)
	++eval_prog_busy;	// paired with the eval_regfree() call
    ਵਾਪਸ prog;
}

/*
 * Free program "prog", obtained with eval_regcomp() for pattern "pat", by
 * keeping it in the cache for the next use.  "pat" must be the same string
 * the program was compiled for and must still be valid.
 */
    ਖਾਲੀ
eval_regfree(char_u *pat, regprog_T *prog)
{
    ਪੂਰਨ	    reentered = eval_prog_reentered;

    ਜੇ (eval_prog_busy > 0 && --eval_prog_busy == 0)
	eval_prog_reentered = FALSE;
    ਜੇ (prog == ਨਿਰਾ)
	ਵਾਪਸ;
    ਜੇ (reentered || *p_cpo != NUL
	    || eval_compile_re != p_re || eval_compile_enc != p_enc)
    {
	// 'cpoptions' is not empty, an option in the cache key changed, or
	// another program was handed out while the caller was using this
	// one: the samples may not describe this program, do not cache it.
	vim_regfree(prog);
	ਵਾਪਸ;
    }
    ਜੇ (eval_prog_pat == ਨਿਰਾ || STRCMP(eval_prog_pat, pat) != 0
	    || STRCMP(eval_prog_enc, p_enc) != 0)
    {
	char_u	*pat_copy;
	char_u	*enc_copy;

	// A cached pattern is known not to be volatile, thus the scan is
	// only needed when installing a new key.
	ਜੇ (eval_prog_volatile(pat))
	{
	    // compiling depends on state the cache key does not cover
	    vim_regfree(prog);
	    ਵਾਪਸ;
	}
	pat_copy = vim_strsave(pat);
	enc_copy = vim_strsave(p_enc);
	ਜੇ (pat_copy == ਨਿਰਾ || enc_copy == ਨਿਰਾ)
	{
	    vim_free(pat_copy);
	    vim_free(enc_copy);
	    vim_regfree(prog);
	    ਵਾਪਸ;
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

#ਜੇ defined(EXITFREE)
    ਖਾਲੀ
free_eval_regcomp_cache(ਖਾਲੀ)
{
    vim_regfree(eval_prog_cache);
    eval_prog_cache = ਨਿਰਾ;
    VIM_CLEAR(eval_prog_pat);
    VIM_CLEAR(eval_prog_enc);
    eval_prog_busy = 0;
    eval_prog_reentered = FALSE;
}
#ਜੇ_ਅੰਤ

/*
 * Return TRUE if "pat" matches "text".
 * Does not use 'cpo' and always uses 'magic'.
 */
    ਪੂਰਨ
ਨਮੂਨਾ_ਮੇਲ(char_u *pat, char_u *ਪਾਠ, ਪੂਰਨ ic)
{
    ਪੂਰਨ		matches = FALSE;
    char_u	*save_cpo;
    regmatch_T	regmatch;

    // avoid 'l' flag in 'cpoptions'
    save_cpo = p_cpo;
    p_cpo = ਖਾਲੀ_ਥਾਵਾਂ_option;
    regmatch.regprog = eval_regcomp(pat);
    ਜੇ (regmatch.regprog != ਨਿਰਾ)
    {
	regmatch.rm_ic = ic;
	matches = vim_regexec_nl(&regmatch, ਪਾਠ, (colnr_T)0);
	eval_regfree(pat, regmatch.regprog);
    }
    p_cpo = save_cpo;
    ਵਾਪਸ matches;
}

/*
 * Handle a name followed by "(".  Both for just "name(arg)" and for
 * "expr->name(arg)".
 * Returns OK or FAIL.
 */
    ਸਥਿਰ ਪੂਰਨ
eval_func(
	char_u	    **arg,	// points to "(", will be advanced
	evalarg_T   *evalarg,
	char_u	    *name,
	ਪੂਰਨ	    name_len,
	typval_T    *rettv,
	ਪੂਰਨ	    flags,
	typval_T    *basetv)	// "expr" for "expr->name(arg)"
{
    ਪੂਰਨ		evaluate = flags & EVAL_EVALUATE;
    char_u	*s = name;
    ਪੂਰਨ		len = name_len;
    partial_T	*partial;
    ਪੂਰਨ		ret = OK;
    type_T	*type = ਨਿਰਾ;
    ਪੂਰਨ		ਮਿਲਿਆ_var = FALSE;

    ਜੇ (!evaluate)
	check_vars(s, len);

    // If "s" is the name of a variable of type VAR_FUNC
    // use its contents.
    s = deref_func_name(s, &len, &partial,
		 ਵਿੱਚ_vim9script() ? &type : ਨਿਰਾ, !evaluate, FALSE, &ਮਿਲਿਆ_var);

    // Need to make a copy, in case evaluating the arguments makes
    // the name invalid.
    s = vim_strsave(s);
    ਜੇ (s == ਨਿਰਾ || (evaluate && *s == NUL))
	ret = FAIL;
    ਨਹੀਂਤਾਂ
    {
	funcexe_T funcexe;

	// Invoke the function.
	CLEAR_ਖੇਤਰ(funcexe);
	funcexe.fe_firstline = curwin->w_cursor.lnum;
	funcexe.fe_lastline = curwin->w_cursor.lnum;
	funcexe.fe_evaluate = evaluate;
	funcexe.fe_partial = partial;
	ਜੇ (partial != ਨਿਰਾ)
	{
	    funcexe.fe_object = partial->pt_obj;
	    ਜੇ (funcexe.fe_object != ਨਿਰਾ)
		++funcexe.fe_object->obj_refcount;
	}
	funcexe.fe_basetv = basetv;
	funcexe.fe_check_type = type;
	funcexe.fe_ਮਿਲਿਆ_var = ਮਿਲਿਆ_var;
	ਜੇ (evalarg != ਨਿਰਾ)
	    funcexe.fe_cctx = evalarg->eval_cctx;
	ret = get_func_tv(s, len, rettv, arg, evalarg, &funcexe);
    }
    vim_free(s);

    // If evaluate is FALSE rettv->v_type was not set in
    // get_func_tv, but it's needed in handle_subscript() to parse
    // what follows. So set it here.
    ਜੇ (rettv->v_type == VAR_UNKNOWN && !evaluate && **arg == '(')
    {
	rettv->vval.v_string = ਨਿਰਾ;
	rettv->v_type = VAR_FUNC;
    }

    // Stop the expression evaluation when immediately
    // aborting on error, or when an interrupt occurred or
    // an exception was thrown but not caught.
    ਜੇ (evaluate && aborting())
    {
	ਜੇ (ret == OK)
	    clear_tv(rettv);
	ret = FAIL;
    }
    ਵਾਪਸ ret;
}

/*
 * After a NL, skip over empty lines and comment-only lines.
 */
    ਸਥਿਰ char_u *
newline_skip_comments(char_u *arg)
{
    char_u *p = arg + 1;

    ਲਈ (;;)
    {
	p = skipwhite(p);

	ਜੇ (*p == NUL)
	    ਰੋਕੋ;
	ਜੇ (vim9_comment_ਸ਼ੁਰੂ(p))
	{
	    char_u *nl = vim_strchr(p, NL);

	    ਜੇ (nl == ਨਿਰਾ)
		ਰੋਕੋ;
	    p = nl;
	}
	ਜੇ (*p != NL)
	    ਰੋਕੋ;
	++p;  // skip another NL
    }
    ਵਾਪਸ p;
}

/*
 * Get the next line source line without advancing.  But do skip over comment
 * lines.
 * Only called for Vim9 script.
 */
    ਸਥਿਰ char_u *
getline_peek_skip_comments(evalarg_T *evalarg)
{
    ਲਈ (;;)
    {
	char_u *ਅਗਲਾ = getline_peek(evalarg->eval_getline,
							 evalarg->eval_cookie);
	char_u *p;

	ਜੇ (ਅਗਲਾ == ਨਿਰਾ)
	    ਰੋਕੋ;
	p = skipwhite(ਅਗਲਾ);
	ਜੇ (*p != NUL && !vim9_comment_ਸ਼ੁਰੂ(p))
	    ਵਾਪਸ ਅਗਲਾ;
	ਜੇ (eval_ਅਗਲਾ_ਸਤਰ(ਨਿਰਾ, evalarg) == ਨਿਰਾ)
	    ਰੋਕੋ;
    }
    ਵਾਪਸ ਨਿਰਾ;
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
eval_ਅਗਲਾ_non_blank(char_u *arg, evalarg_T *evalarg, ਪੂਰਨ *getnext)
{
    char_u *p = skipwhite(arg);

    *getnext = FALSE;
    ਜੇ (ਵਿੱਚ_vim9script()
	    && evalarg != ਨਿਰਾ
	    && (evalarg->eval_cookie != ਨਿਰਾ || evalarg->eval_cctx != ਨਿਰਾ
								   || *p == NL)
	    && (*p == NUL || *p == NL
			     || (vim9_comment_ਸ਼ੁਰੂ(p) && VIM_ISWHITE(p[-1]))))
    {
	char_u *ਅਗਲਾ;

	ਜੇ (*p == NL)
	    ਅਗਲਾ = newline_skip_comments(p);
	ਨਹੀਂਤਾਂ ਜੇ (evalarg->eval_cookie != ਨਿਰਾ)
	    ਅਗਲਾ = getline_peek_skip_comments(evalarg);
	ਨਹੀਂਤਾਂ
	    ਅਗਲਾ = peek_ਅਗਲਾ_ਸਤਰ_from_context(evalarg->eval_cctx);

	ਜੇ (ਅਗਲਾ != ਨਿਰਾ)
	{
	    *getnext = *p != NL;
	    ਵਾਪਸ skipwhite(ਅਗਲਾ);
	}
    }
    ਵਾਪਸ p;
}

/*
 * To be called after eval_next_non_blank() sets "getnext" to TRUE.
 * Only called for Vim9 script.
 *
 * If "arg" is not NULL, then the caller should assign the return value to
 * "arg".
 */
    char_u *
eval_ਅਗਲਾ_ਸਤਰ(char_u *arg, evalarg_T *evalarg)
{
    garray_T	*gap = &evalarg->eval_ga;
    char_u	*ਸਤਰ;

    ਜੇ (arg != ਨਿਰਾ)
    {
	ਜੇ (*arg == NL)
	    ਵਾਪਸ newline_skip_comments(arg);
	// Truncate before a trailing comment, so that concatenating the lines
	// won't turn the rest into a comment.
	ਜੇ (*skipwhite(arg) == '#')
	    *arg = NUL;
    }

    ਜੇ (evalarg->eval_cookie != ਨਿਰਾ)
	ਸਤਰ = evalarg->eval_getline(0, evalarg->eval_cookie, 0,
							   GETLINE_CONCAT_ALL);
    ਨਹੀਂਤਾਂ
	ਸਤਰ = ਅਗਲਾ_ਸਤਰ_from_context(evalarg->eval_cctx, TRUE);
    ਜੇ (ਸਤਰ == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    ++evalarg->eval_break_ਗਿਣਤੀ;
    ਜੇ (gap->ga_itemsize > 0 && ga_grow(gap, 1) == OK)
    {
	char_u *p = skipwhite(ਸਤਰ);

	// Going to concatenate the lines after parsing.  For an empty or
	// comment line use an empty string.
	ਜੇ (*p == NUL || vim9_comment_ਸ਼ੁਰੂ(p))
	{
	    vim_free(ਸਤਰ);
	    ਸਤਰ = vim_strsave((char_u *)"");
	}

	((char_u **)gap->ga_ਅੰਕੜੇ)[gap->ga_len] = ਸਤਰ;
	++gap->ga_len;
    }
    ਨਹੀਂਤਾਂ ਜੇ (evalarg->eval_cookie != ਨਿਰਾ)
    {
	free_eval_tofree_later(evalarg);
	evalarg->eval_tofree = ਸਤਰ;
    }

    // Advanced to the next line, "arg" no longer points into the previous
    // line.  The caller assigns the return value to "arg".
    // If "arg" is NULL, then the return value is discarded.  In that case,
    // "arg" still points to the previous line.  So don't reset
    // "eval_using_cmdline".
    ਜੇ (arg != ਨਿਰਾ)
	evalarg->eval_using_cmdline = FALSE;
    ਵਾਪਸ skipwhite(ਸਤਰ);
}

/*
 * Call eval_next_non_blank() and get the next line if needed.
 */
    char_u *
skipwhite_and_linebreak(char_u *arg, evalarg_T *evalarg)
{
    ਪੂਰਨ	    getnext;
    char_u  *p = skipwhite_and_nl(arg);

    ਜੇ (evalarg == ਨਿਰਾ)
	ਵਾਪਸ skipwhite(arg);
    eval_ਅਗਲਾ_non_blank(p, evalarg, &getnext);
    ਜੇ (getnext)
	ਵਾਪਸ eval_ਅਗਲਾ_ਸਤਰ(arg, evalarg);
    ਵਾਪਸ p;
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
    ਪੂਰਨ
eval0(
    char_u	*arg,
    typval_T	*rettv,
    exarg_T	*eap,
    evalarg_T	*evalarg)
{
    ਵਾਪਸ eval0_retarg(arg, rettv, eap, evalarg, ਨਿਰਾ);
}

/*
 * If "arg" is a simple function call without arguments then call it and return
 * the result.  Otherwise return NOTDONE.
 */
    ਪੂਰਨ
may_call_simple_func(
    char_u	*arg,
    typval_T	*rettv)
{
    char_u  *parens = (char_u *)ਲੜੀ_ਖੋਜ((ਅੱਖਰ *)arg, "()");
    ਪੂਰਨ	    r = NOTDONE;

    // If the expression is "FuncName()" then we can skip a lot of overhead.
    ਜੇ (parens != ਨਿਰਾ && *skipwhite(parens + 2) == NUL)
    {
	char_u *p = STRNCMP(arg, "<SNR>", 5) == 0 ? skipdigits(arg + 5) : arg;

	ਜੇ (ਮੰਜ਼ਿਲ_name_ਸਿਰਾ(p, TRUE) == parens)
	    r = call_simple_func(arg, (size_t)(parens - arg), rettv);
    }
    ਵਾਪਸ r;
}

/*
 * Handle zero level expression with optimization for a simple function call.
 * Same arguments and return value as eval0().
 */
    ਸਥਿਰ ਪੂਰਨ
eval0_simple_funccal(
    char_u	*arg,
    typval_T	*rettv,
    exarg_T	*eap,
    evalarg_T	*evalarg)
{
    ਪੂਰਨ	    r = may_call_simple_func(arg, rettv);

    ਜੇ (r == NOTDONE)
	r = eval0_retarg(arg, rettv, eap, evalarg, ਨਿਰਾ);
    ਵਾਪਸ r;
}

/*
 * Like eval0() but when "retarg" is not NULL store the pointer to after the
 * expression and don't check what comes after the expression.
 */
    ਪੂਰਨ
eval0_retarg(
    char_u	*arg,
    typval_T	*rettv,
    exarg_T	*eap,
    evalarg_T	*evalarg,
    char_u	**retarg)
{
    ਪੂਰਨ		ret;
    char_u	*p;
    char_u	*ਵਿਅੰਜਕ_ਸਿਰਾ;
    ਪੂਰਨ		did_emsg_before = did_emsg;
    ਪੂਰਨ		called_emsg_before = called_emsg;
    ਪੂਰਨ		check_for_ਸਿਰਾ = retarg == ਨਿਰਾ;
    ਪੂਰਨ		ਸਿਰਾ_error = FALSE;

    p = skipwhite(arg);
    ret = eval1(&p, rettv, evalarg);

    ਜੇ (ret != FAIL)
    {
	ਵਿਅੰਜਕ_ਸਿਰਾ = p;
	p = skipwhite(p);

	// In Vim9 script a command block is not split at NL characters for
	// commands using an expression argument.  Skip over a '#' comment to
	// check for a following NL.  Require white space before the '#'.
	ਜੇ (ਵਿੱਚ_vim9script() && p > ਵਿਅੰਜਕ_ਸਿਰਾ && retarg == ਨਿਰਾ)
	    ਜਦੋਂ (*p == '#')
	    {
		char_u *nl = vim_strchr(p, NL);

		ਜੇ (nl == ਨਿਰਾ)
		    ਰੋਕੋ;
		p = skipwhite(nl + 1);
		ਜੇ (eap != ਨਿਰਾ && *p != NUL)
		    eap->nextcmd = p;
		check_for_ਸਿਰਾ = FALSE;
	    }

	ਜੇ (check_for_ਸਿਰਾ)
	    ਸਿਰਾ_error = !ਸਿਰੇ_excmd2(arg, p);
    }

    ਜੇ (ret == FAIL || ਸਿਰਾ_error)
    {
	ਜੇ (ret != FAIL)
	    clear_tv(rettv);
	/*
	 * Report the invalid expression unless the expression evaluation has
	 * been cancelled due to an aborting error, an interrupt, or an
	 * exception, or we already gave a more specific error.
	 * Also check called_emsg for when using assert_fails().
	 */
	ਜੇ (!aborting()
		&& did_emsg == did_emsg_before
		&& called_emsg == called_emsg_before
		&& (!ਵਿੱਚ_vim9script() || !vim9_bad_comment(p)))
	{
	    ਜੇ (ਸਿਰਾ_error)
		semsg(_(e_trailing_characters_str), p);
	    ਨਹੀਂਤਾਂ
		semsg(_(e_invalid_ਵਿਅੰਜਕ_str), arg);
	}

	ਜੇ (eap != ਨਿਰਾ && p != ਨਿਰਾ)
	{
	    // Some of the expression may not have been consumed.
	    // Only execute a next command if it cannot be a "||" operator.
	    // The next command may be "catch".
	    char_u *nextcmd = check_nextcmd(p);
	    ਜੇ (nextcmd != ਨਿਰਾ && *nextcmd != '|')
		eap->nextcmd = nextcmd;
	}
	ਵਾਪਸ FAIL;
    }

    ਜੇ (retarg != ਨਿਰਾ)
	*retarg = p;
    ਨਹੀਂਤਾਂ ਜੇ (check_for_ਸਿਰਾ && eap != ਨਿਰਾ)
	ਸੈੱਟ_nextcmd(eap, p);

    ਵਾਪਸ ret;
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
    ਪੂਰਨ
eval1(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    char_u  *p;
    ਪੂਰਨ	    getnext;

    CLEAR_ਸੂਚਕ(rettv);

    /*
     * Get the first variable.
     */
    ਜੇ (eval2(arg, rettv, evalarg) == FAIL)
	ਵਾਪਸ FAIL;

    p = eval_ਅਗਲਾ_non_blank(*arg, evalarg, &getnext);
    ਜੇ (*p == '?')
    {
	ਪੂਰਨ		op_falsy = p[1] == '?';
	ਪੂਰਨ		ਨਤੀਜਾ;
	typval_T	var2;
	evalarg_T	*evalarg_ਵਰਤਿਆ = evalarg;
	evalarg_T	local_evalarg;
	ਪੂਰਨ		orig_flags;
	ਪੂਰਨ		evaluate;
	ਪੂਰਨ		vim9script = ਵਿੱਚ_vim9script();

	ਜੇ (evalarg == ਨਿਰਾ)
	{
	    init_evalarg(&local_evalarg);
	    evalarg_ਵਰਤਿਆ = &local_evalarg;
	}
	orig_flags = evalarg_ਵਰਤਿਆ->eval_flags;
	evaluate = evalarg_ਵਰਤਿਆ->eval_flags & EVAL_EVALUATE;

	ਜੇ (getnext)
	    *arg = eval_ਅਗਲਾ_ਸਤਰ(*arg, evalarg_ਵਰਤਿਆ);
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (evaluate && vim9script && !VIM_ISWHITE(p[-1]))
	    {
		error_white_both(p, op_falsy ? 2 : 1);
		clear_tv(rettv);
		ਵਾਪਸ FAIL;
	    }
	    *arg = p;
	}

	ਨਤੀਜਾ = FALSE;
	ਜੇ (evaluate)
	{
	    ਪੂਰਨ		error = FALSE;

	    ਜੇ (op_falsy)
	    {
		// Is this typeval supported with the falsy operator?
		ਜੇ (check_typval_is_ਮੁੱਲ(rettv) == FAIL)
		{
		    clear_tv(rettv);
		    ਵਾਪਸ FAIL;
		}
		ਨਤੀਜਾ = tv2bool(rettv);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (vim9script)
		ਨਤੀਜਾ = tv_get_bool_chk(rettv, &error);
	    ਨਹੀਂਤਾਂ ਜੇ (tv_get_ਸੰਖਿਆ_chk(rettv, &error) != 0)
		ਨਤੀਜਾ = TRUE;
	    ਜੇ (error || !op_falsy || !ਨਤੀਜਾ)
		clear_tv(rettv);
	    ਜੇ (error)
		ਵਾਪਸ FAIL;
	}

	/*
	 * Get the second variable.  Recursive!
	 */
	ਜੇ (op_falsy)
	    ++*arg;
	ਜੇ (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[1]))
	{
	    error_white_both(*arg - (op_falsy ? 1 : 0), op_falsy ? 2 : 1);
	    clear_tv(rettv);
	    ਵਾਪਸ FAIL;
	}
	*arg = skipwhite_and_linebreak(*arg + 1, evalarg_ਵਰਤਿਆ);
	evalarg_ਵਰਤਿਆ->eval_flags = (op_falsy ? !ਨਤੀਜਾ : ਨਤੀਜਾ)
				  ? orig_flags : (orig_flags & ~EVAL_EVALUATE);
	ਜੇ (eval1(arg, &var2, evalarg_ਵਰਤਿਆ) == FAIL)
	{
	    evalarg_ਵਰਤਿਆ->eval_flags = orig_flags;
	    ਵਾਪਸ FAIL;
	}
	ਜੇ (!op_falsy || !ਨਤੀਜਾ)
	    *rettv = var2;

	ਜੇ (!op_falsy)
	{
	    /*
	     * Check for the ":".
	     */
	    p = eval_ਅਗਲਾ_non_blank(*arg, evalarg_ਵਰਤਿਆ, &getnext);
	    ਜੇ (*p != ':')
	    {
		emsg(_(e_missing_colon_after_questionmark));
		ਜੇ (evaluate && ਨਤੀਜਾ)
		    clear_tv(rettv);
		evalarg_ਵਰਤਿਆ->eval_flags = orig_flags;
		ਵਾਪਸ FAIL;
	    }
	    ਜੇ (getnext)
		*arg = eval_ਅਗਲਾ_ਸਤਰ(*arg, evalarg_ਵਰਤਿਆ);
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ (evaluate && vim9script && !VIM_ISWHITE(p[-1]))
		{
		    error_white_both(p, 1);
		    clear_tv(rettv);
		    evalarg_ਵਰਤਿਆ->eval_flags = orig_flags;
		    ਵਾਪਸ FAIL;
		}
		*arg = p;
	    }

	    /*
	     * Get the third variable.  Recursive!
	     */
	    ਜੇ (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[1]))
	    {
		error_white_both(*arg, 1);
		clear_tv(rettv);
		evalarg_ਵਰਤਿਆ->eval_flags = orig_flags;
		ਵਾਪਸ FAIL;
	    }
	    *arg = skipwhite_and_linebreak(*arg + 1, evalarg_ਵਰਤਿਆ);
	    evalarg_ਵਰਤਿਆ->eval_flags = !ਨਤੀਜਾ ? orig_flags
					       : (orig_flags & ~EVAL_EVALUATE);
	    ਜੇ (eval1(arg, &var2, evalarg_ਵਰਤਿਆ) == FAIL)
	    {
		ਜੇ (evaluate && ਨਤੀਜਾ)
		    clear_tv(rettv);
		evalarg_ਵਰਤਿਆ->eval_flags = orig_flags;
		ਵਾਪਸ FAIL;
	    }
	    ਜੇ (evaluate && !ਨਤੀਜਾ)
		*rettv = var2;
	}

	ਜੇ (evalarg == ਨਿਰਾ)
	    clear_evalarg(&local_evalarg, ਨਿਰਾ);
	ਨਹੀਂਤਾਂ
	    evalarg->eval_flags = orig_flags;
    }

    ਵਾਪਸ OK;
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
    ਸਥਿਰ ਪੂਰਨ
eval2(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    char_u	*p;
    ਪੂਰਨ		getnext;

    /*
     * Get the first expression.
     */
    ਜੇ (eval3(arg, rettv, evalarg) == FAIL)
	ਵਾਪਸ FAIL;

    /*
     * Handle the  "||" operator.
     */
    p = eval_ਅਗਲਾ_non_blank(*arg, evalarg, &getnext);
    ਜੇ (p[0] == '|' && p[1] == '|')
    {
	evalarg_T   *evalarg_ਵਰਤਿਆ = evalarg;
	evalarg_T   local_evalarg;
	ਪੂਰਨ	    evaluate;
	ਪੂਰਨ	    orig_flags;
	ਲੰਮਾ	    ਨਤੀਜਾ = FALSE;
	typval_T    var2;
	ਪੂਰਨ	    error = FALSE;
	ਪੂਰਨ	    vim9script = ਵਿੱਚ_vim9script();

	ਜੇ (evalarg == ਨਿਰਾ)
	{
	    init_evalarg(&local_evalarg);
	    evalarg_ਵਰਤਿਆ = &local_evalarg;
	}
	orig_flags = evalarg_ਵਰਤਿਆ->eval_flags;
	evaluate = orig_flags & EVAL_EVALUATE;
	ਜੇ (evaluate)
	{
	    ਜੇ (vim9script)
		ਨਤੀਜਾ = tv_get_bool_chk(rettv, &error);
	    ਨਹੀਂਤਾਂ ਜੇ (tv_get_ਸੰਖਿਆ_chk(rettv, &error) != 0)
		ਨਤੀਜਾ = TRUE;
	    clear_tv(rettv);
	    ਜੇ (error)
		ਵਾਪਸ FAIL;
	}

	/*
	 * Repeat until there is no following "||".
	 */
	ਜਦੋਂ (p[0] == '|' && p[1] == '|')
	{
	    ਜੇ (getnext)
		*arg = eval_ਅਗਲਾ_ਸਤਰ(*arg, evalarg_ਵਰਤਿਆ);
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ (evaluate && vim9script && !VIM_ISWHITE(p[-1]))
		{
		    error_white_both(p, 2);
		    clear_tv(rettv);
		    ਵਾਪਸ FAIL;
		}
		*arg = p;
	    }

	    /*
	     * Get the second variable.
	     */
	    ਜੇ (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[2]))
	    {
		error_white_both(*arg, 2);
		clear_tv(rettv);
		ਵਾਪਸ FAIL;
	    }
	    *arg = skipwhite_and_linebreak(*arg + 2, evalarg_ਵਰਤਿਆ);
	    evalarg_ਵਰਤਿਆ->eval_flags = !ਨਤੀਜਾ ? orig_flags
					       : (orig_flags & ~EVAL_EVALUATE);
	    ਜੇ (eval3(arg, &var2, evalarg_ਵਰਤਿਆ) == FAIL)
		ਵਾਪਸ FAIL;

	    /*
	     * Compute the result.
	     */
	    ਜੇ (evaluate && !ਨਤੀਜਾ)
	    {
		ਜੇ (vim9script)
		    ਨਤੀਜਾ = tv_get_bool_chk(&var2, &error);
		ਨਹੀਂਤਾਂ ਜੇ (tv_get_ਸੰਖਿਆ_chk(&var2, &error) != 0)
		    ਨਤੀਜਾ = TRUE;
		clear_tv(&var2);
		ਜੇ (error)
		    ਵਾਪਸ FAIL;
	    }
	    ਜੇ (evaluate)
	    {
		ਜੇ (vim9script)
		{
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_ਸੰਖਿਆ = ਨਤੀਜਾ ? VVAL_TRUE : VVAL_FALSE;
		}
		ਨਹੀਂਤਾਂ
		{
		    rettv->v_type = VAR_ਸੰਖਿਆ;
		    rettv->vval.v_ਸੰਖਿਆ = ਨਤੀਜਾ;
		}
	    }

	    p = eval_ਅਗਲਾ_non_blank(*arg, evalarg_ਵਰਤਿਆ, &getnext);
	}

	ਜੇ (evalarg == ਨਿਰਾ)
	    clear_evalarg(&local_evalarg, ਨਿਰਾ);
	ਨਹੀਂਤਾਂ
	    evalarg->eval_flags = orig_flags;
    }

    ਵਾਪਸ OK;
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
    ਸਥਿਰ ਪੂਰਨ
eval3(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    char_u	*p;
    ਪੂਰਨ		getnext;

    /*
     * Get the first expression.
     */
    ਜੇ (eval4(arg, rettv, evalarg) == FAIL)
	ਵਾਪਸ FAIL;

    /*
     * Handle the "&&" operator.
     */
    p = eval_ਅਗਲਾ_non_blank(*arg, evalarg, &getnext);
    ਜੇ (p[0] == '&' && p[1] == '&')
    {
	evalarg_T   *evalarg_ਵਰਤਿਆ = evalarg;
	evalarg_T   local_evalarg;
	ਪੂਰਨ	    orig_flags;
	ਪੂਰਨ	    evaluate;
	ਲੰਮਾ	    ਨਤੀਜਾ = TRUE;
	typval_T    var2;
	ਪੂਰਨ	    error = FALSE;
	ਪੂਰਨ	    vim9script = ਵਿੱਚ_vim9script();

	ਜੇ (evalarg == ਨਿਰਾ)
	{
	    init_evalarg(&local_evalarg);
	    evalarg_ਵਰਤਿਆ = &local_evalarg;
	}
	orig_flags = evalarg_ਵਰਤਿਆ->eval_flags;
	evaluate = orig_flags & EVAL_EVALUATE;
	ਜੇ (evaluate)
	{
	    ਜੇ (vim9script)
		ਨਤੀਜਾ = tv_get_bool_chk(rettv, &error);
	    ਨਹੀਂਤਾਂ ਜੇ (tv_get_ਸੰਖਿਆ_chk(rettv, &error) == 0)
		ਨਤੀਜਾ = FALSE;
	    clear_tv(rettv);
	    ਜੇ (error)
		ਵਾਪਸ FAIL;
	}

	/*
	 * Repeat until there is no following "&&".
	 */
	ਜਦੋਂ (p[0] == '&' && p[1] == '&')
	{
	    ਜੇ (getnext)
		*arg = eval_ਅਗਲਾ_ਸਤਰ(*arg, evalarg_ਵਰਤਿਆ);
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ (evaluate && vim9script && !VIM_ISWHITE(p[-1]))
		{
		    error_white_both(p, 2);
		    clear_tv(rettv);
		    ਵਾਪਸ FAIL;
		}
		*arg = p;
	    }

	    /*
	     * Get the second variable.
	     */
	    ਜੇ (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[2]))
	    {
		error_white_both(*arg, 2);
		clear_tv(rettv);
		ਵਾਪਸ FAIL;
	    }
	    *arg = skipwhite_and_linebreak(*arg + 2, evalarg_ਵਰਤਿਆ);
	    evalarg_ਵਰਤਿਆ->eval_flags = ਨਤੀਜਾ ? orig_flags
					      : (orig_flags & ~EVAL_EVALUATE);
	    CLEAR_ਖੇਤਰ(var2);
	    ਜੇ (eval4(arg, &var2, evalarg_ਵਰਤਿਆ) == FAIL)
		ਵਾਪਸ FAIL;

	    /*
	     * Compute the result.
	     */
	    ਜੇ (evaluate && ਨਤੀਜਾ)
	    {
		ਜੇ (vim9script)
		    ਨਤੀਜਾ = tv_get_bool_chk(&var2, &error);
		ਨਹੀਂਤਾਂ ਜੇ (tv_get_ਸੰਖਿਆ_chk(&var2, &error) == 0)
		    ਨਤੀਜਾ = FALSE;
		clear_tv(&var2);
		ਜੇ (error)
		    ਵਾਪਸ FAIL;
	    }
	    ਜੇ (evaluate)
	    {
		ਜੇ (vim9script)
		{
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_ਸੰਖਿਆ = ਨਤੀਜਾ ? VVAL_TRUE : VVAL_FALSE;
		}
		ਨਹੀਂਤਾਂ
		{
		    rettv->v_type = VAR_ਸੰਖਿਆ;
		    rettv->vval.v_ਸੰਖਿਆ = ਨਤੀਜਾ;
		}
	    }

	    p = eval_ਅਗਲਾ_non_blank(*arg, evalarg_ਵਰਤਿਆ, &getnext);
	}

	ਜੇ (evalarg == ਨਿਰਾ)
	    clear_evalarg(&local_evalarg, ਨਿਰਾ);
	ਨਹੀਂਤਾਂ
	    evalarg->eval_flags = orig_flags;
    }

    ਵਾਪਸ OK;
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
    ਸਥਿਰ ਪੂਰਨ
eval4(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    char_u	*p;
    ਪੂਰਨ		getnext;
    exprtype_T	type = ਵਿਅੰਜਕ_UNKNOWN;
    ਪੂਰਨ		len = 2;
    ਪੂਰਨ		type_is = FALSE;

    /*
     * Get the first expression.
     */
    ਜੇ (eval5(arg, rettv, evalarg) == FAIL)
	ਵਾਪਸ FAIL;

    p = eval_ਅਗਲਾ_non_blank(*arg, evalarg, &getnext);

    type = get_compare_type(p, &len, &type_is);

    /*
     * If there is a comparative operator, use it.
     */
    ਜੇ (type != ਵਿਅੰਜਕ_UNKNOWN)
    {
	typval_T    var2;
	ਪੂਰਨ	    ic;
	ਪੂਰਨ	    vim9script = ਵਿੱਚ_vim9script();
	ਪੂਰਨ	    evaluate = evalarg == ਨਿਰਾ
				   ? 0 : (evalarg->eval_flags & EVAL_EVALUATE);
	ਲੰਮਾ	    comp_lnum = SOURCING_LNUM;

	ਜੇ (getnext)
	{
	    *arg = eval_ਅਗਲਾ_ਸਤਰ(*arg, evalarg);
	    p = *arg;
	}
	ਨਹੀਂਤਾਂ ਜੇ (evaluate && vim9script && !VIM_ISWHITE(**arg))
	{
	    error_white_both(*arg, len);
	    clear_tv(rettv);
	    ਵਾਪਸ FAIL;
	}

	ਜੇ (vim9script && type_is && (p[len] == '?' || p[len] == '#'))
	{
	    semsg(_(e_invalid_ਵਿਅੰਜਕ_str), p);
	    clear_tv(rettv);
	    ਵਾਪਸ FAIL;
	}

	// extra question mark appended: ignore case
	ਜੇ (p[len] == '?')
	{
	    ic = TRUE;
	    ++len;
	}
	// extra '#' appended: match case
	ਨਹੀਂਤਾਂ ਜੇ (p[len] == '#')
	{
	    ic = FALSE;
	    ++len;
	}
	// nothing appended: use 'ignorecase' if not in Vim script
	ਨਹੀਂਤਾਂ
	    ic = vim9script ? FALSE : p_ic;

	/*
	 * Get the second variable.
	 */
	ਜੇ (evaluate && vim9script && !IS_WHITE_OR_NUL(p[len]))
	{
	    error_white_both(p, len);
	    clear_tv(rettv);
	    ਵਾਪਸ FAIL;
	}
	*arg = skipwhite_and_linebreak(p + len, evalarg);
	ਜੇ (eval5(arg, &var2, evalarg) == FAIL)
	{
	    clear_tv(rettv);
	    ਵਾਪਸ FAIL;
	}
	ਜੇ (evaluate)
	{
	    ਪੂਰਨ ret;

	    // use the line of the comparison for messages
	    SOURCING_LNUM = comp_lnum;
	    ਜੇ (vim9script && check_compare_types(type, rettv, &var2) == FAIL)
	    {
		ret = FAIL;
		clear_tv(rettv);
	    }
	    ਨਹੀਂਤਾਂ
		ret = typval_compare(rettv, &var2, type, ic);
	    clear_tv(&var2);
	    ਵਾਪਸ ret;
	}
    }

    ਵਾਪਸ OK;
}

/*
 * Make a copy of blob "tv1" and append blob "tv2".
 */
    ਖਾਲੀ
eval_addblob(typval_T *tv1, typval_T *tv2)
{
    blob_T  *b1 = tv1->vval.v_blob;
    blob_T  *b2 = tv2->vval.v_blob;
    blob_T  *b = blob_alloc();
    ਲੰਮਾ    len1, len2;
    ਲੰਮਾ    totallen;

    ਜੇ (b == ਨਿਰਾ)
	ਵਾਪਸ;

    len1 = blob_len(b1);
    len2 = blob_len(b2);
    totallen = len1 + len2;

    ਜੇ (totallen > 0 && totallen <= ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ
				    && ga_grow(&b->bv_ga, (ਪੂਰਨ)totallen) == OK)
    {
	ਜੇ (len1 > 0)
	    mch_memmove((char_u *)b->bv_ga.ga_ਅੰਕੜੇ,
		    (char_u *)b1->bv_ga.ga_ਅੰਕੜੇ, (size_t)len1);
	ਜੇ (len2 > 0)
	    mch_memmove((char_u *)b->bv_ga.ga_ਅੰਕੜੇ + len1,
		    (char_u *)b2->bv_ga.ga_ਅੰਕੜੇ, (size_t)len2);
	b->bv_ga.ga_len = (ਪੂਰਨ)totallen;
    }

    clear_tv(tv1);
    rettv_blob_ਸੈੱਟ(tv1, b);
}

/*
 * Make a copy of list "tv1" and append list "tv2".
 */
    ਪੂਰਨ
eval_addlist(typval_T *tv1, typval_T *tv2)
{
    typval_T var3;

    // concatenate Lists
    ਜੇ (list_concat(tv1->vval.v_list, tv2->vval.v_list, &var3) == FAIL)
    {
	clear_tv(tv1);
	clear_tv(tv2);
	ਵਾਪਸ FAIL;
    }
    clear_tv(tv1);
    *tv1 = var3;
    ਵਾਪਸ OK;
}

/*
 * Make a copy of tuple "tv1" and append tuple "tv2".
 */
    ਪੂਰਨ
eval_addtuple(typval_T *tv1, typval_T *tv2)
{
    ਪੂਰਨ		vim9script = ਵਿੱਚ_vim9script();
    typval_T	var3;

    ਜੇ (vim9script && tv1->vval.v_tuple != ਨਿਰਾ && tv2->vval.v_tuple != ਨਿਰਾ
	    && tv1->vval.v_tuple->tv_type != ਨਿਰਾ
	    && tv2->vval.v_tuple->tv_type != ਨਿਰਾ)
    {
	ਜੇ (!check_tuples_addable(tv1->vval.v_tuple->tv_type,
						tv2->vval.v_tuple->tv_type))
	    ਵਾਪਸ FAIL;
    }

    // concatenate tuples
    ਜੇ (tuple_concat(tv1->vval.v_tuple, tv2->vval.v_tuple, &var3) == FAIL)
    {
	clear_tv(tv1);
	clear_tv(tv2);
	ਵਾਪਸ FAIL;
    }
    clear_tv(tv1);
    *tv1 = var3;
    ਵਾਪਸ OK;
}

/*
 * Left or right shift the number "tv1" by the number "tv2" and store the
 * result in "tv1".
 *
 * Return OK or FAIL.
 */
    ਸਥਿਰ ਪੂਰਨ
eval_ਖਿਸਕਾਓ_ਸੰਖਿਆ(typval_T *tv1, typval_T *tv2, ਪੂਰਨ ਖਿਸਕਾਓ_type)
{
    ਜੇ (tv2->v_type != VAR_ਸੰਖਿਆ || tv2->vval.v_ਸੰਖਿਆ < 0)
    {
	// right operand should be a positive number
	ਜੇ (tv2->v_type != VAR_ਸੰਖਿਆ)
	    emsg(_(e_bitshift_ops_must_be_ਸੰਖਿਆ));
	ਨਹੀਂਤਾਂ
	    emsg(_(e_bitshift_ops_must_be_ਸਕਾਰਾਤਮਕ));
	clear_tv(tv1);
	clear_tv(tv2);
	ਵਾਪਸ FAIL;
    }

    ਜੇ (tv2->vval.v_ਸੰਖਿਆ > MAX_LSHIFT_ਬਿੱਟ)
	// shifting more bits than we have always results in zero
	tv1->vval.v_ਸੰਖਿਆ = 0;
    ਨਹੀਂਤਾਂ ਜੇ (ਖਿਸਕਾਓ_type == ਵਿਅੰਜਕ_LSHIFT)
	tv1->vval.v_ਸੰਖਿਆ =
	    (uvarnumber_T)tv1->vval.v_ਸੰਖਿਆ << tv2->vval.v_ਸੰਖਿਆ;
    ਨਹੀਂਤਾਂ
	tv1->vval.v_ਸੰਖਿਆ =
	    (uvarnumber_T)tv1->vval.v_ਸੰਖਿਆ >> tv2->vval.v_ਸੰਖਿਆ;

    ਵਾਪਸ OK;
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
    ਸਥਿਰ ਪੂਰਨ
eval5(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    /*
     * Get the first expression.
     */
    ਜੇ (eval6(arg, rettv, evalarg) == FAIL)
	ਵਾਪਸ FAIL;

    /*
     * Repeat computing, until no '<<' or '>>' is following.
     */
    ਲਈ (;;)
    {
	char_u		*p;
	ਪੂਰਨ		getnext;
	exprtype_T	exprtype;
	ਪੂਰਨ		evaluate;
	typval_T	var2;
	ਪੂਰਨ		vim9script;

	p = eval_ਅਗਲਾ_non_blank(*arg, evalarg, &getnext);
	ਜੇ (p[0] == '<' && p[1] == '<')
	    exprtype = ਵਿਅੰਜਕ_LSHIFT;
	ਨਹੀਂਤਾਂ ਜੇ (p[0] == '>' && p[1] == '>')
	    exprtype = ਵਿਅੰਜਕ_RSHIFT;
	ਨਹੀਂਤਾਂ
	    ਵਾਪਸ OK;

	// Handle a bitwise left or right shift operator
	evaluate = evalarg == ਨਿਰਾ ? 0 : (evalarg->eval_flags & EVAL_EVALUATE);
	ਜੇ (evaluate && rettv->v_type != VAR_ਸੰਖਿਆ)
	{
	    // left operand should be a number
	    emsg(_(e_bitshift_ops_must_be_ਸੰਖਿਆ));
	    clear_tv(rettv);
	    ਵਾਪਸ FAIL;
	}

	vim9script = ਵਿੱਚ_vim9script();
	ਜੇ (getnext)
	{
	    *arg = eval_ਅਗਲਾ_ਸਤਰ(*arg, evalarg);
	    p = *arg;
	}
	ਨਹੀਂਤਾਂ ਜੇ (evaluate && vim9script && !VIM_ISWHITE(**arg))
	{
	    error_white_both(*arg, 2);
	    clear_tv(rettv);
	    ਵਾਪਸ FAIL;
	}

	/*
	 * Get the second variable.
	 */
	ਜੇ (evaluate && vim9script && !IS_WHITE_OR_NUL(p[2]))
	{
	    error_white_both(p, 2);
	    clear_tv(rettv);
	    ਵਾਪਸ FAIL;
	}
	*arg = skipwhite_and_linebreak(p + 2, evalarg);
	ਜੇ (eval6(arg, &var2, evalarg) == FAIL)
	{
	    clear_tv(rettv);
	    ਵਾਪਸ FAIL;
	}

	ਜੇ (evaluate)
	{
	    ਜੇ (eval_ਖਿਸਕਾਓ_ਸੰਖਿਆ(rettv, &var2, exprtype) == FAIL)
		ਵਾਪਸ FAIL;
	}

	clear_tv(&var2);
    }

    ਵਾਪਸ OK;
}

/*
 * Concatenate strings "tv1" and "tv2" and store the result in "tv1".
 */
    ਸਥਿਰ ਪੂਰਨ
eval_concat_str(typval_T *tv1, typval_T *tv2)
{
    char_u	buf1[NUMBUFLEN], buf2[NUMBUFLEN];
    char_u	*s1 = tv_get_string_buf(tv1, buf1);
    char_u	*s2 = ਨਿਰਾ;
    char_u	*p;
    ਪੂਰਨ		vim9script = ਵਿੱਚ_vim9script();

    ਜੇ (vim9script && (tv2->v_type == VAR_VOID
		|| tv2->v_type == VAR_CHANNEL
		|| tv2->v_type == VAR_ਕੰਮ))
	semsg(_(e_using_invalid_ਮੁੱਲ_as_string_str),
		vartype_name(tv2->v_type));
    ਨਹੀਂਤਾਂ ਜੇ (vim9script && tv2->v_type == VAR_FLOAT)
    {
	vim_snprintf((ਅੱਖਰ *)buf2, NUMBUFLEN, "%g",
		tv2->vval.v_float);
	s2 = buf2;
    }
    ਨਹੀਂਤਾਂ
	s2 = tv_get_string_buf_chk(tv2, buf2);
    ਜੇ (s2 == ਨਿਰਾ)		// type error ?
    {
	clear_tv(tv1);
	clear_tv(tv2);
	ਵਾਪਸ FAIL;
    }

    // When possible, grow the existing string in place to avoid alloc/free.
    ਜੇ (grow_string_tv(tv1, s2) == OK)
	ਵਾਪਸ OK;

    p = concat_str(s1, s2);
    clear_tv(tv1);
    tv1->v_type = VAR_STRING;
    tv1->vval.v_string = p;

    ਵਾਪਸ OK;
}

/*
 * Add or subtract numbers "tv1" and "tv2" and store the result in "tv1".
 * The numbers can be whole numbers or floats.
 */
    ਸਥਿਰ ਪੂਰਨ
eval_addsub_ਸੰਖਿਆ(typval_T *tv1, typval_T *tv2, ਪੂਰਨ op)
{
    ਪੂਰਨ		error = FALSE;
    varnumber_T	n1, n2;
    float_T	f1 = 0, f2 = 0;

    ਜੇ (tv1->v_type == VAR_FLOAT)
    {
	f1 = tv1->vval.v_float;
	n1 = 0;
    }
    ਨਹੀਂਤਾਂ
    {
	n1 = tv_get_ਸੰਖਿਆ_chk(tv1, &error);
	ਜੇ (error)
	{
	    // This can only happen for "list + non-list" or
	    // "blob + non-blob".  For "non-list + ..." or
	    // "something - ...", we returned before evaluating the
	    // 2nd operand.
	    clear_tv(tv1);
	    clear_tv(tv2);
	    ਵਾਪਸ FAIL;
	}
	ਜੇ (tv2->v_type == VAR_FLOAT)
	    f1 = n1;
    }
    ਜੇ (tv2->v_type == VAR_FLOAT)
    {
	f2 = tv2->vval.v_float;
	n2 = 0;
    }
    ਨਹੀਂਤਾਂ
    {
	n2 = tv_get_ਸੰਖਿਆ_chk(tv2, &error);
	ਜੇ (error)
	{
	    clear_tv(tv1);
	    clear_tv(tv2);
	    ਵਾਪਸ FAIL;
	}
	ਜੇ (tv1->v_type == VAR_FLOAT)
	    f2 = n2;
    }
    clear_tv(tv1);

    // If there is a float on either side the result is a float.
    ਜੇ (tv1->v_type == VAR_FLOAT || tv2->v_type == VAR_FLOAT)
    {
	ਜੇ (op == '+')
	    f1 = f1 + f2;
	ਨਹੀਂਤਾਂ
	    f1 = f1 - f2;
	tv1->v_type = VAR_FLOAT;
	tv1->vval.v_float = f1;
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (op == '+')
	    n1 = n1 + n2;
	ਨਹੀਂਤਾਂ
	    n1 = n1 - n2;
	tv1->v_type = VAR_ਸੰਖਿਆ;
	tv1->vval.v_ਸੰਖਿਆ = n1;
    }

    ਵਾਪਸ OK;
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
    ਸਥਿਰ ਪੂਰਨ
eval6(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    /*
     * Get the first expression.
     */
    ਜੇ (eval7(arg, rettv, evalarg, FALSE) == FAIL)
	ਵਾਪਸ FAIL;

    /*
     * Repeat computing, until no '+', '-' or '.' is following.
     */
    ਲਈ (;;)
    {
	ਪੂਰਨ	    evaluate;
	ਪੂਰਨ	    getnext;
	char_u	    *p;
	ਪੂਰਨ	    op;
	ਪੂਰਨ	    oplen;
	ਪੂਰਨ	    concat;
	typval_T    var2;
	ਪੂਰਨ	    vim9script = ਵਿੱਚ_vim9script();
	ਲੰਮਾ	    op_lnum = SOURCING_LNUM;

	// "." is only string concatenation when scriptversion is 1
	// "+=", "-=" and "..=" are assignments
	// "++" and "--" on the next line are a separate command.
	p = eval_ਅਗਲਾ_non_blank(*arg, evalarg, &getnext);
	op = *p;
	concat = op == '.' && (*(p + 1) == '.' || ਵਿੱਚ_old_script(2));
	ਜੇ ((op != '+' && op != '-' && !concat) || p[1] == '='
					       || (p[1] == '.' && p[2] == '='))
	    ਰੋਕੋ;
	ਜੇ (getnext && (op == '+' || op == '-') && p[0] == p[1])
	    ਰੋਕੋ;

	evaluate = evalarg == ਨਿਰਾ ? 0 : (evalarg->eval_flags & EVAL_EVALUATE);
	oplen = (concat && p[1] == '.') ? 2 : 1;
	ਜੇ (getnext)
	    *arg = eval_ਅਗਲਾ_ਸਤਰ(*arg, evalarg);
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (evaluate && vim9script && !VIM_ISWHITE(**arg))
	    {
		error_white_both(*arg, oplen);
		clear_tv(rettv);
		ਵਾਪਸ FAIL;
	    }
	    *arg = p;
	}
	ਜੇ ((op != '+' || (rettv->v_type != VAR_LIST
						&& rettv->v_type != VAR_TUPLE
						&& rettv->v_type != VAR_BLOB))
		&& (op == '.' || rettv->v_type != VAR_FLOAT)
		&& evaluate)
	{
	    ਪੂਰਨ		error = FALSE;

	    // For "list + ...", an illegal use of the first operand as
	    // a number cannot be determined before evaluating the 2nd
	    // operand: if this is also a list, all is ok.
	    // For "something . ...", "something - ..." or "non-list + ...",
	    // we know that the first operand needs to be a string or number
	    // without evaluating the 2nd operand.  So check before to avoid
	    // side effects after an error.
	    ਜੇ (op != '.')
		tv_get_ਸੰਖਿਆ_chk(rettv, &error);
	    ਜੇ ((op == '.' && tv_get_string_chk(rettv) == ਨਿਰਾ) || error)
	    {
		clear_tv(rettv);
		ਵਾਪਸ FAIL;
	    }
	}

	/*
	 * Get the second variable.
	 */
	ਜੇ (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[oplen]))
	{
	    error_white_both(*arg, oplen);
	    clear_tv(rettv);
	    ਵਾਪਸ FAIL;
	}
	*arg = skipwhite_and_linebreak(*arg + oplen, evalarg);
	ਜੇ (eval7(arg, &var2, evalarg, !vim9script && op == '.') == FAIL)
	{
	    clear_tv(rettv);
	    ਵਾਪਸ FAIL;
	}

	ਜੇ (evaluate)
	{
	    /*
	     * Compute the result.
	     */
	    // use the line of the operation for messages
	    SOURCING_LNUM = op_lnum;
	    ਜੇ (op == '.')
	    {
		ਜੇ (eval_concat_str(rettv, &var2) == FAIL)
		    ਵਾਪਸ FAIL;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (op == '+' && rettv->v_type == VAR_BLOB
						   && var2.v_type == VAR_BLOB)
		eval_addblob(rettv, &var2);
	    ਨਹੀਂਤਾਂ ਜੇ (op == '+' && rettv->v_type == VAR_LIST
						   && var2.v_type == VAR_LIST)
	    {
		ਜੇ (eval_addlist(rettv, &var2) == FAIL)
		    ਵਾਪਸ FAIL;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (op == '+' && rettv->v_type == VAR_TUPLE
					   && var2.v_type == VAR_TUPLE)
	    {
		ਜੇ (eval_addtuple(rettv, &var2) == FAIL)
		    ਵਾਪਸ FAIL;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ (eval_addsub_ਸੰਖਿਆ(rettv, &var2, op) == FAIL)
		    ਵਾਪਸ FAIL;
	    }
	    clear_tv(&var2);
	}
    }
    ਵਾਪਸ OK;
}

/*
 * Multiply or divide or compute the modulo of numbers "tv1" and "tv2" and
 * store the result in "tv1".  The numbers can be whole numbers or floats.
 */
    ਸਥਿਰ ਪੂਰਨ
eval_multdiv_ਸੰਖਿਆ(typval_T *tv1, typval_T *tv2, ਪੂਰਨ op)
{
    varnumber_T	n1, n2;
    float_T	f1, f2;
    ਪੂਰਨ		error;
    ਪੂਰਨ		use_float = FALSE;

    f1 = 0;
    f2 = 0;
    error = FALSE;
    ਜੇ (tv1->v_type == VAR_FLOAT)
    {
	f1 = tv1->vval.v_float;
	use_float = TRUE;
	n1 = 0;
    }
    ਨਹੀਂਤਾਂ
	n1 = tv_get_ਸੰਖਿਆ_chk(tv1, &error);
    clear_tv(tv1);
    ਜੇ (error)
    {
	clear_tv(tv2);
	ਵਾਪਸ FAIL;
    }

    ਜੇ (tv2->v_type == VAR_FLOAT)
    {
	ਜੇ (!use_float)
	{
	    f1 = n1;
	    use_float = TRUE;
	}
	f2 = tv2->vval.v_float;
	n2 = 0;
    }
    ਨਹੀਂਤਾਂ
    {
	n2 = tv_get_ਸੰਖਿਆ_chk(tv2, &error);
	clear_tv(tv2);
	ਜੇ (error)
	    ਵਾਪਸ FAIL;
	ਜੇ (use_float)
	    f2 = n2;
    }

    /*
     * Compute the result.
     * When either side is a float the result is a float.
     */
    ਜੇ (use_float)
    {
	ਜੇ (op == '*')
	    f1 = f1 * f2;
	ਨਹੀਂਤਾਂ ਜੇ (op == '/')
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ VMS
	    // VMS crashes on divide by zero, work around it
	    ਜੇ (f2 == 0.0)
	    {
		ਜੇ (f1 == 0)
		    f1 = -1 * __F_FLT_MAX - 1L;   // similar to NaN
		ਨਹੀਂਤਾਂ ਜੇ (f1 < 0)
		    f1 = -1 * __F_FLT_MAX;
		ਨਹੀਂਤਾਂ
		    f1 = __F_FLT_MAX;
	    }
	    ਨਹੀਂਤਾਂ
		f1 = f1 / f2;
#ਨਹੀਂਤਾਂ
	    // We rely on the floating point library to handle divide
	    // by zero to result in "inf" and not a crash.
	    f1 = f1 / f2;
#ਜੇ_ਅੰਤ
	}
	ਨਹੀਂਤਾਂ
	{
	    emsg(_(e_cannot_use_percent_with_float));
	    ਵਾਪਸ FAIL;
	}
	tv1->v_type = VAR_FLOAT;
	tv1->vval.v_float = f1;
    }
    ਨਹੀਂਤਾਂ
    {
	ਪੂਰਨ	    failed = FALSE;

	ਜੇ (op == '*')
	    n1 = n1 * n2;
	ਨਹੀਂਤਾਂ ਜੇ (op == '/')
	    n1 = ਅੰਕ_divide(n1, n2, &failed);
	ਨਹੀਂਤਾਂ
	    n1 = ਅੰਕ_modulus(n1, n2, &failed);
	ਜੇ (failed)
	    ਵਾਪਸ FAIL;

	tv1->v_type = VAR_ਸੰਖਿਆ;
	tv1->vval.v_ਸੰਖਿਆ = n1;
    }

    ਵਾਪਸ OK;
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
    ਸਥਿਰ ਪੂਰਨ
eval7(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    ਪੂਰਨ		want_string)  // after "." operator
{
    /*
     * Get the first expression.
     */
    ਜੇ (eval8(arg, rettv, evalarg, want_string) == FAIL)
	ਵਾਪਸ FAIL;

    /*
     * Repeat computing, until no '*', '/' or '%' is following.
     */
    ਲਈ (;;)
    {
	ਪੂਰਨ	    evaluate;
	ਪੂਰਨ	    getnext;
	typval_T    var2;
	char_u	    *p;
	ਪੂਰਨ	    op;

	// "*=", "/=" and "%=" are assignments
	p = eval_ਅਗਲਾ_non_blank(*arg, evalarg, &getnext);
	op = *p;
	ਜੇ ((op != '*' && op != '/' && op != '%') || p[1] == '=')
	    ਰੋਕੋ;

	evaluate = evalarg == ਨਿਰਾ ? 0 : (evalarg->eval_flags & EVAL_EVALUATE);
	ਜੇ (getnext)
	    *arg = eval_ਅਗਲਾ_ਸਤਰ(*arg, evalarg);
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (evaluate && ਵਿੱਚ_vim9script() && !VIM_ISWHITE(**arg))
	    {
		error_white_both(*arg, 1);
		clear_tv(rettv);
		ਵਾਪਸ FAIL;
	    }
	    *arg = p;
	}

	/*
	 * Get the second variable.
	 */
	ਜੇ (evaluate && ਵਿੱਚ_vim9script() && !IS_WHITE_OR_NUL((*arg)[1]))
	{
	    error_white_both(*arg, 1);
	    clear_tv(rettv);
	    ਵਾਪਸ FAIL;
	}
	*arg = skipwhite_and_linebreak(*arg + 1, evalarg);
	ਜੇ (eval8(arg, &var2, evalarg, FALSE) == FAIL)
	    ਵਾਪਸ FAIL;

	ਜੇ (evaluate)
	    // Compute the result.
	    ਜੇ (eval_multdiv_ਸੰਖਿਆ(rettv, &var2, op) == FAIL)
		ਵਾਪਸ FAIL;
    }

    ਵਾਪਸ OK;
}

/*
 * Handle seventh level expression:
 *	a type cast before a base level expression.
 * "arg" must point to the first non-white of the expression.
 * "arg" is advanced to just after the recognized expression.
 * Return OK or FAIL.
 */
    ਸਥਿਰ ਪੂਰਨ
eval8(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    ਪੂਰਨ		want_string)	// after "." operator
{
    type_T	*want_type = ਨਿਰਾ;
    garray_T	type_list;	    // list of pointers to allocated types
    ਪੂਰਨ		res;
    ਪੂਰਨ		evaluate = evalarg == ਨਿਰਾ ? 0
				       : (evalarg->eval_flags & EVAL_EVALUATE);

    // Recognize <type> in Vim9 script only.
    ਜੇ (ਵਿੱਚ_vim9script() && **arg == '<' && eval_isnamec1((*arg)[1])
					     && STRNCMP(*arg, "<SNR>", 5) != 0)
    {
	++*arg;
	ga_init2(&type_list, ਆਕਾਰ(type_T *), 10);
	ਜੇ (!evaluate)
	    // Only skip over the type, it may use a type variable that is only
	    // known when the expression is compiled, e.g. in a lambda.
	    *arg = skip_type(*arg, FALSE);
	ਨਹੀਂਤਾਂ
	{
	    want_type = parse_type(arg, &type_list, ਨਿਰਾ, ਨਿਰਾ, TRUE);
	    ਜੇ (want_type == ਨਿਰਾ)
	    {
		clear_type_list(&type_list);
		ਵਾਪਸ FAIL;
	    }
	}

	ਜੇ (**arg != '>')
	{
	    ਜੇ (*skipwhite(*arg) == '>')
		semsg(_(e_no_white_space_allowed_before_str_str), ">", *arg);
	    ਨਹੀਂਤਾਂ
		emsg(_(e_missing_gt));
	    clear_type_list(&type_list);
	    ਵਾਪਸ FAIL;
	}
	++*arg;
	*arg = skipwhite_and_linebreak(*arg, evalarg);
    }

    res = eval9(arg, rettv, evalarg, want_string);

    ਜੇ (want_type != ਨਿਰਾ)
    {
	ਜੇ (evaluate && res == OK)
	{
	    type_T *actual = typval2type(rettv, get_copyID(), &type_list,
							       TVTT_DO_MEMBER);

	    ਜੇ (!equal_type(want_type, actual, 0))
	    {
		ਜੇ (want_type->tt_type == VAR_BOOL
					&& actual->tt_type != VAR_BOOL
					&& (actual->tt_flags & TTFLAG_BOOL_OK))
		{
		    ਪੂਰਨ n = tv2bool(rettv);

		    // can use "0" and "1" for boolean in some places
		    clear_tv(rettv);
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_ਸੰਖਿਆ = n ? VVAL_TRUE : VVAL_FALSE;
		}
		ਨਹੀਂਤਾਂ
		{
		    where_T where = WHERE_INIT;

		    res = check_type(want_type, actual, TRUE, where);
		}
	    }
	}
	clear_type_list(&type_list);
    }

    ਵਾਪਸ res;
}

    ਪੂਰਨ
eval_leader(char_u **arg, ਪੂਰਨ vim9)
{
    char_u	*s = *arg;
    char_u	*p = *arg;

    ਜਦੋਂ (*p == '!' || *p == '-' || *p == '+')
    {
	char_u *n = skipwhite(p + 1);

	// ++, --, -+ and +- are not accepted in Vim9 script
	ਜੇ (vim9 && (*p == '-' || *p == '+') && (*n == '-' || *n == '+'))
	{
	    semsg(_(e_invalid_ਵਿਅੰਜਕ_str), s);
	    ਵਾਪਸ FAIL;
	}
	p = n;
    }
    *arg = p;
    ਵਾਪਸ OK;
}

/*
 * Check for a predefined value "true", "false" and "null.*".
 * Return OK when recognized.
 */
    ਪੂਰਨ
handle_predefined(char_u *s, ਪੂਰਨ len, typval_T *rettv)
{
    ਚੋਣ (len)
    {
	ਹਾਲਤ 4: ਜੇ (STRNCMP(s, "true", 4) == 0)
		{
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_ਸੰਖਿਆ = VVAL_TRUE;
		    ਵਾਪਸ OK;
		}
		ਜੇ (STRNCMP(s, "null", 4) == 0)
		{
		    rettv->v_type = VAR_SPECIAL;
		    rettv->vval.v_ਸੰਖਿਆ = VVAL_NULL;
		    ਵਾਪਸ OK;
		}
		ਰੋਕੋ;
	ਹਾਲਤ 5: ਜੇ (STRNCMP(s, "false", 5) == 0)
		{
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_ਸੰਖਿਆ = VVAL_FALSE;
		    ਵਾਪਸ OK;
		}
		ਰੋਕੋ;
	ਹਾਲਤ 8: ਜੇ (STRNCMP(s, "null_job", 8) == 0)
		{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
		    rettv->v_type = VAR_ਕੰਮ;
		    rettv->vval.v_ਕੰਮ = ਨਿਰਾ;
#ਨਹੀਂਤਾਂ
		    rettv->v_type = VAR_SPECIAL;
		    rettv->vval.v_ਸੰਖਿਆ = VVAL_NULL;
#ਜੇ_ਅੰਤ
		    ਵਾਪਸ OK;
		}
		ਰੋਕੋ;
	ਹਾਲਤ 9:
		ਜੇ (STRNCMP(s, "null_", 5) != 0)
		    ਰੋਕੋ;
		// null_list
		ਜੇ (STRNCMP(s + 5, "list", 4) == 0)
		{
		    rettv->v_type = VAR_LIST;
		    rettv->vval.v_list = ਨਿਰਾ;
		    ਵਾਪਸ OK;
		}
		// null_dict
		ਜੇ (STRNCMP(s + 5, "dict", 4) == 0)
		{
		    rettv->v_type = VAR_DICT;
		    rettv->vval.v_dict = ਨਿਰਾ;
		    ਵਾਪਸ OK;
		}
		// null_blob
		ਜੇ (STRNCMP(s + 5, "blob", 4) == 0)
		{
		    rettv->v_type = VAR_BLOB;
		    rettv->vval.v_blob = ਨਿਰਾ;
		    ਵਾਪਸ OK;
		}
		ਰੋਕੋ;
	ਹਾਲਤ 10:
		ਜੇ (STRNCMP(s, "null_", 5) != 0)
		    ਰੋਕੋ;
		// null_class
		ਜੇ (STRNCMP(s + 5, "class", 5) == 0)
		{
		    rettv->v_type = VAR_CLASS;
		    rettv->vval.v_class = ਨਿਰਾ;
		    ਵਾਪਸ OK;
		}
		ਜੇ (STRNCMP(s + 5, "tuple", 5) == 0)
		{
		    rettv->v_type = VAR_TUPLE;
		    rettv->vval.v_tuple = ਨਿਰਾ;
		    ਵਾਪਸ OK;
		}
		ਰੋਕੋ;
	ਹਾਲਤ 11: ਜੇ (STRNCMP(s, "null_string", 11) == 0)
		{
		    rettv->v_type = VAR_STRING;
		    rettv->vval.v_string = ਨਿਰਾ;
		    ਵਾਪਸ OK;
		}
		ਜੇ (STRNCMP(s, "null_object", 11) == 0)
		{
		    rettv->v_type = VAR_OBJECT;
		    rettv->vval.v_object = ਨਿਰਾ;
		    ਵਾਪਸ OK;
		}
		ਰੋਕੋ;
	ਹਾਲਤ 12:
		ਜੇ (STRNCMP(s, "null_channel", 12) == 0)
		{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
		    rettv->v_type = VAR_CHANNEL;
		    rettv->vval.v_channel = ਨਿਰਾ;
#ਨਹੀਂਤਾਂ
		    rettv->v_type = VAR_SPECIAL;
		    rettv->vval.v_ਸੰਖਿਆ = VVAL_NULL;
#ਜੇ_ਅੰਤ
		    ਵਾਪਸ OK;
		}
		ਜੇ (STRNCMP(s, "null_partial", 12) == 0)
		{
		    rettv->v_type = VAR_PARTIAL;
		    rettv->vval.v_partial = ਨਿਰਾ;
		    ਵਾਪਸ OK;
		}
		ਰੋਕੋ;
	ਹਾਲਤ 13: ਜੇ (STRNCMP(s, "null_function", 13) == 0)
		{
		    rettv->v_type = VAR_FUNC;
		    rettv->vval.v_string = ਨਿਰਾ;
		    ਵਾਪਸ OK;
		}
		ਰੋਕੋ;
    }
    ਵਾਪਸ FAIL;
}

/*
 * Handle register contents: @r.
 */
    ਸਥਿਰ ਖਾਲੀ
eval9_reg_contents(
    char_u	**arg,
    typval_T	*rettv,
    ਪੂਰਨ		evaluate)
{
    ਪੂਰਨ		vim9script = ਵਿੱਚ_vim9script();

    ++*arg;	// skip '@'
    ਜੇ (evaluate)
    {
	ਜੇ (vim9script && IS_WHITE_OR_NUL(**arg))
	    semsg(_(e_syntax_error_at_str), *arg);
	ਨਹੀਂਤਾਂ ਜੇ (vim9script && !valid_yank_reg(**arg, FALSE))
	    emsg_invreg(**arg);
	ਨਹੀਂਤਾਂ
	{
	    rettv->v_type = VAR_STRING;
	    rettv->vval.v_string = get_reg_contents(**arg,
		    GREG_ਵਿਅੰਜਕ_SRC);
	}
    }
    ਜੇ (**arg != NUL)
	++*arg;
}

/*
 * Handle a nested expression: (expression) or lambda: (arg) => expr
 */
    ਸਥਿਰ ਪੂਰਨ
eval9_nested_ਵਿਅੰਜਕ(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    ਪੂਰਨ		evaluate)
{
    ਪੂਰਨ		ret = NOTDONE;
    ਪੂਰਨ		vim9script = ਵਿੱਚ_vim9script();

    ਜੇ (vim9script)
    {
	ret = get_lambda_tv(arg, rettv, TRUE, evalarg, ਨਿਰਾ);
	ਜੇ (ret == OK && evaluate)
	{
	    ufunc_T *ufunc = rettv->vval.v_partial->pt_func;

	    // Compile it here to get the return type.  The return
	    // type is optional, when it's missing use t_unknown.
	    // This is recognized in compile_return().
	    ਜੇ (ufunc->uf_ret_type->tt_type == VAR_VOID)
		ufunc->uf_ret_type = &t_unknown;
	    ਜੇ (compile_def_function(ufunc, FALSE,
				get_compile_type(ufunc), ਨਿਰਾ) == FAIL)
	    {
		clear_tv(rettv);
		ret = FAIL;
	    }
	}
    }
    ਜੇ (ret == NOTDONE)
    {
	*arg = skipwhite_and_linebreak(*arg + 1, evalarg);
	ਜੇ (**arg == ')')
	    // empty tuple
	    ret = eval_tuple(arg, rettv, evalarg, TRUE);
	ਨਹੀਂਤਾਂ
	{
	    ret = eval1(arg, rettv, evalarg);	// recursive!
	    ਜੇ (ret != OK)
		ਵਾਪਸ ret;

	    *arg = skipwhite_and_linebreak(*arg, evalarg);

	    ਜੇ (**arg == ',')
		// tuple
		ret = eval_tuple(arg, rettv, evalarg, TRUE);
	    ਨਹੀਂਤਾਂ ਜੇ (**arg == ')')
		++*arg;
	    ਨਹੀਂਤਾਂ ਜੇ (ret == OK)
	    {
		emsg(_(e_missing_closing_paren));
		clear_tv(rettv);
		ret = FAIL;
	    }
	}
    }

    ਵਾਪਸ ret;
}

/*
* Handle be a variable or function name.
* Can also be a curly-braces kind of name: {expr}.
*/
    ਸਥਿਰ ਪੂਰਨ
eval9_var_func_name(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    ਪੂਰਨ		evaluate,
    char_u	**name_ਸ਼ੁਰੂ)
{
    char_u	*s;
    ਪੂਰਨ		len;
    char_u	*alias;
    ਪੂਰਨ		ret = OK;
    ਪੂਰਨ		vim9script = ਵਿੱਚ_vim9script();

    s = *arg;
    len = get_name_len(arg, &alias, evaluate, TRUE);
    ਜੇ (alias != ਨਿਰਾ)
	s = alias;

    ਜੇ (len <= 0)
	ret = FAIL;
    ਨਹੀਂਤਾਂ
    {
	ਪੂਰਨ	flags = evalarg == ਨਿਰਾ ? 0 : evalarg->eval_flags;

	ਜੇ (evaluate && vim9script && len == 1 && *s == '_')
	{
	    emsg(_(e_cannot_use_underscore_ਇੱਥੇ));
	    ret = FAIL;
	}
	ਨਹੀਂਤਾਂ ਜੇ (evaluate && vim9script && len > 2
						&& s[0] == 's' && s[1] == ':')
	{
	    semsg(_(e_cannot_use_s_colon_ਵਿੱਚ_vim9_script_str), s);
	    ret = FAIL;
	}
	ਨਹੀਂਤਾਂ ਜੇ ((vim9script ? **arg : *skipwhite(*arg)) == '('
				|| (vim9script && generic_func_call(arg)))
	{
	    // "name(..."  recursive!
	    *arg = skipwhite(*arg);
	    ret = eval_func(arg, evalarg, s, len, rettv, flags, ਨਿਰਾ);
	}
	ਨਹੀਂਤਾਂ ਜੇ (evaluate)
	{
	    // get the value of "true", "false", etc. or a variable
	    ret = FAIL;
	    ਜੇ (vim9script)
		ret = handle_predefined(s, len, rettv);
	    ਜੇ (ret == FAIL)
	    {
		*name_ਸ਼ੁਰੂ = s;
		ret = eval_variable(s, len, 0, rettv, ਨਿਰਾ,
					EVAL_VAR_VERBOSE + EVAL_VAR_IMPORT);

		// skip the generic function arguments (if present)
		// they are already processed by eval_variable
		ਜੇ (ret == OK && vim9script && **arg == '<'
						&& rettv->v_type == VAR_FUNC)
		    ret = skip_generic_func_type_ਦਲੀਲਾਂ(arg);
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    // skip the name
	    check_vars(s, len);

	    // Skip the type arguments of a generic function, its name starts
	    // with an uppercase letter.
	    ਜੇ (vim9script && **arg == '<' && ASCII_ISUPPER(*s))
		skip_generic_func_type_ਦਲੀਲਾਂ(arg);
	    ret = OK;
	}
    }
    vim_free(alias);

    ਵਾਪਸ ret;
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
    ਸਥਿਰ ਪੂਰਨ
eval9(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    ਪੂਰਨ		want_string)	// after "." operator
{
    ਪੂਰਨ		evaluate = evalarg != ਨਿਰਾ
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    char_u	*name_ਸ਼ੁਰੂ = ਨਿਰਾ;
    char_u	*ਸ਼ੁਰੂ_leader, *ਸਿਰਾ_leader;
    ਪੂਰਨ		ret = OK;
    ਸਥਿਰ ਪੂਰਨ	recurse = 0;
    ਪੂਰਨ		vim9script = ਵਿੱਚ_vim9script();

    /*
     * Initialise variable so that clear_tv() can't mistake this for a
     * string and free a string that isn't there.
     */
    rettv->v_type = VAR_UNKNOWN;

    /*
     * Skip '!', '-' and '+' characters.  They are handled later.
     */
    ਸ਼ੁਰੂ_leader = *arg;
    ਜੇ (eval_leader(arg, vim9script) == FAIL)
	ਵਾਪਸ FAIL;
    ਸਿਰਾ_leader = *arg;

    ਜੇ (**arg == '.' && (!ਸੁਰੱਖਿਅਤ_isdigit(*(*arg + 1)) || ਵਿੱਚ_old_script(2)))
    {
	semsg(_(e_invalid_ਵਿਅੰਜਕ_str), *arg);
	++*arg;
	ਵਾਪਸ FAIL;
    }

    // Limit recursion to 1000 levels.  At least at 10000 we run out of stack
    // and crash.  With MSVC the stack is smaller.
    ਜੇ (recurse ==
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _MSC_VER
		    300
#ਨਹੀਂਤਾਂ
		    1000
#ਜੇ_ਅੰਤ
		    )
    {
	semsg(_(e_ਵਿਅੰਜਕ_too_recursive_str), *arg);
	ਵਾਪਸ FAIL;
    }
    ++recurse;

    ਚੋਣ (**arg)
    {
    /*
     * Number constant.
     */
    ਹਾਲਤ '0':
    ਹਾਲਤ '1':
    ਹਾਲਤ '2':
    ਹਾਲਤ '3':
    ਹਾਲਤ '4':
    ਹਾਲਤ '5':
    ਹਾਲਤ '6':
    ਹਾਲਤ '7':
    ਹਾਲਤ '8':
    ਹਾਲਤ '9':
    ਹਾਲਤ '.':	ret = eval_ਸੰਖਿਆ(arg, rettv, evaluate, want_string);

		// Apply prefixed "-" and "+" now.  Matters especially when
		// "->" follows.
		ਜੇ (ret == OK && evaluate && ਸਿਰਾ_leader > ਸ਼ੁਰੂ_leader
						  && rettv->v_type != VAR_BLOB)
		    ret = eval9_leader(rettv, TRUE, ਸ਼ੁਰੂ_leader, &ਸਿਰਾ_leader);
		ਰੋਕੋ;

    /*
     * String constant: "string".
     */
    ਹਾਲਤ '"':	ret = eval_string(arg, rettv, evaluate, FALSE);
		ਰੋਕੋ;

    /*
     * Literal string constant: 'str''ing'.
     */
    ਹਾਲਤ '\'':	ret = eval_lit_string(arg, rettv, evaluate, FALSE);
		ਰੋਕੋ;

    /*
     * List: [expr, expr]
     */
    ਹਾਲਤ '[':	ret = eval_list(arg, rettv, evalarg, TRUE);
		ਰੋਕੋ;

    /*
     * Literal Dictionary: #{key: val, key: val}
     */
    ਹਾਲਤ '#':	ret = eval_lit_dict(arg, rettv, evalarg);
		ਰੋਕੋ;

    /*
     * Lambda: {arg, arg -> expr}
     * Dictionary: {'key': val, 'key': val}
     */
    ਹਾਲਤ '{':	ਜੇ (vim9script)
		    ret = NOTDONE;
		ਨਹੀਂਤਾਂ
		    ret = get_lambda_tv(arg, rettv, vim9script, evalarg, ਨਿਰਾ);
		ਜੇ (ret == NOTDONE)
		    ret = eval_dict(arg, rettv, evalarg, FALSE);
		ਰੋਕੋ;

    /*
     * Option value: &name
     */
    ਹਾਲਤ '&':	ret = eval_option(arg, rettv, evaluate);
		ਰੋਕੋ;

    /*
     * Environment variable: $VAR.
     * Interpolated string: $"string" or $'string'.
     */
    ਹਾਲਤ '$':	ਜੇ ((*arg)[1] == '"' || (*arg)[1] == '\'')
		    ret = eval_interp_string(arg, rettv, evaluate);
		ਨਹੀਂਤਾਂ
		    ret = eval_env_var(arg, rettv, evaluate);
		ਰੋਕੋ;

    /*
     * Register contents: @r.
     */
    ਹਾਲਤ '@':	eval9_reg_contents(arg, rettv, evaluate);
		ਰੋਕੋ;

    /*
     * nested expression: (expression).
     * or lambda: (arg) => expr
     * or tuple
     */
    ਹਾਲਤ '(':	ret = eval9_nested_ਵਿਅੰਜਕ(arg, rettv, evalarg, evaluate);
		ਰੋਕੋ;

    ਮੂਲ:	ret = NOTDONE;
		ਰੋਕੋ;
    }

    ਜੇ (ret == NOTDONE)
    {
	/*
	 * Must be a variable or function name.
	 * Can also be a curly-braces kind of name: {expr}.
	 */
	ret = eval9_var_func_name(arg, rettv, evalarg, evaluate, &name_ਸ਼ੁਰੂ);
    }

    // Handle following '[', '(' and '.' for expr[expr], expr.name,
    // expr(expr), expr->name(expr)
    ਜੇ (ret == OK)
	ret = handle_subscript(arg, name_ਸ਼ੁਰੂ, rettv, evalarg, evaluate);

    /*
     * Apply logical NOT and unary '-', from right to left, ignore '+'.
     */
    ਜੇ (ret == OK && evaluate && ਸਿਰਾ_leader > ਸ਼ੁਰੂ_leader)
	ret = eval9_leader(rettv, FALSE, ਸ਼ੁਰੂ_leader, &ਸਿਰਾ_leader);

    --recurse;
    ਵਾਪਸ ret;
}

/*
 * Apply the leading "!" and "-" before an eval9 expression to "rettv".
 * When "numeric_only" is TRUE only handle "+" and "-".
 * Adjusts "end_leaderp" until it is at "start_leader".
 */
    ਸਥਿਰ ਪੂਰਨ
eval9_leader(
	typval_T    *rettv,
	ਪੂਰਨ	    numeric_only,
	char_u	    *ਸ਼ੁਰੂ_leader,
	char_u	    **ਸਿਰਾ_leaderp)
{
    char_u	*ਸਿਰਾ_leader = *ਸਿਰਾ_leaderp;
    ਪੂਰਨ		ret = OK;
    ਪੂਰਨ		error = FALSE;
    varnumber_T val = 0;
    vartype_T	type = rettv->v_type;
    ਪੂਰਨ		vim9script = ਵਿੱਚ_vim9script();
    float_T	    f = 0.0;

    ਜੇ (rettv->v_type == VAR_FLOAT)
	f = rettv->vval.v_float;
    ਨਹੀਂਤਾਂ
    {
	ਜਦੋਂ (VIM_ISWHITE(ਸਿਰਾ_leader[-1]))
	    --ਸਿਰਾ_leader;
	ਜੇ (vim9script && ਸਿਰਾ_leader[-1] == '!')
	{
	    // Is this typeval supported with the ! operator?
	    ਜੇ (check_typval_is_ਮੁੱਲ(rettv) == FAIL)
	    {
		clear_tv(rettv);
		ਵਾਪਸ FAIL;
	    }
	    val = tv2bool(rettv);
	}
	ਨਹੀਂਤਾਂ
	    val = tv_get_ਸੰਖਿਆ_chk(rettv, &error);
    }
    ਜੇ (error)
    {
	clear_tv(rettv);
	ret = FAIL;
    }
    ਨਹੀਂਤਾਂ
    {
	ਜਦੋਂ (ਸਿਰਾ_leader > ਸ਼ੁਰੂ_leader)
	{
	    --ਸਿਰਾ_leader;
	    ਜੇ (*ਸਿਰਾ_leader == '!')
	    {
		ਜੇ (numeric_only)
		{
		    ++ਸਿਰਾ_leader;
		    ਰੋਕੋ;
		}
		ਜੇ (rettv->v_type == VAR_FLOAT)
		{
		    rettv->v_type = VAR_BOOL;
		    val = f == 0.0 ? VVAL_TRUE : VVAL_FALSE;
		    type = VAR_BOOL;
		}
		ਨਹੀਂਤਾਂ
		{
		    val = !val;
		    type = VAR_BOOL;
		}
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (*ਸਿਰਾ_leader == '-')
	    {
		ਜੇ (rettv->v_type == VAR_FLOAT)
		    f = -f;
		ਨਹੀਂਤਾਂ
		{
		    val = -val;
		    type = VAR_ਸੰਖਿਆ;
		}
	    }
	}
	ਜੇ (rettv->v_type == VAR_FLOAT)
	{
	    clear_tv(rettv);
	    rettv->vval.v_float = f;
	}
	ਨਹੀਂਤਾਂ
	{
	    clear_tv(rettv);
	    ਜੇ (vim9script)
		rettv->v_type = type;
	    ਨਹੀਂਤਾਂ
		rettv->v_type = VAR_ਸੰਖਿਆ;
	    rettv->vval.v_ਸੰਖਿਆ = val;
	}
    }
    *ਸਿਰਾ_leaderp = ਸਿਰਾ_leader;
    ਵਾਪਸ ret;
}

/*
 * Call the function referred to in "rettv".
 */
    ਸਥਿਰ ਪੂਰਨ
call_func_rettv(
	char_u	    **arg,
	evalarg_T   *evalarg,
	typval_T    *rettv,
	ਪੂਰਨ	    evaluate,
	dict_T	    *selfdict,
	typval_T    *basetv)
{
    partial_T	*pt = ਨਿਰਾ;
    funcexe_T	funcexe;
    typval_T	functv;
    char_u	*s;
    ਪੂਰਨ		ret;

    // need to copy the funcref so that we can clear rettv
    ਜੇ (evaluate)
    {
	functv = *rettv;
	rettv->v_type = VAR_UNKNOWN;

	// Invoke the function.  Recursive!
	ਜੇ (functv.v_type == VAR_PARTIAL)
	{
	    pt = functv.vval.v_partial;
	    s = partial_name(pt);
	}
	ਨਹੀਂਤਾਂ
	{
	    s = functv.vval.v_string;
	    ਜੇ (s == ਨਿਰਾ || *s == NUL)
	    {
		emsg(_(e_ਖਾਲੀ_ਥਾਵਾਂ_function_name));
		ret = FAIL;
		ਜਾਓ theend;
	    }
	}
    }
    ਨਹੀਂਤਾਂ
	s = (char_u *)"";

    CLEAR_ਖੇਤਰ(funcexe);
    funcexe.fe_firstline = curwin->w_cursor.lnum;
    funcexe.fe_lastline = curwin->w_cursor.lnum;
    funcexe.fe_evaluate = evaluate;
    funcexe.fe_partial = pt;
    ਜੇ (pt != ਨਿਰਾ)
    {
	funcexe.fe_object = pt->pt_obj;
	ਜੇ (funcexe.fe_object != ਨਿਰਾ)
	    ++funcexe.fe_object->obj_refcount;
    }
    funcexe.fe_selfdict = selfdict;
    funcexe.fe_basetv = basetv;
    ਜੇ (evalarg != ਨਿਰਾ)
	funcexe.fe_cctx = evalarg->eval_cctx;
    ret = get_func_tv(s, -1, rettv, arg, evalarg, &funcexe);

theend:
    // Clear the funcref afterwards, so that deleting it while
    // evaluating the arguments is possible (see test55).
    ਜੇ (evaluate)
	clear_tv(&functv);

    ਵਾਪਸ ret;
}

/*
 * Evaluate "->method()".
 * "*arg" points to "method".
 * Returns FAIL or OK. "*arg" is advanced to after the ')'.
 */
    ਸਥਿਰ ਪੂਰਨ
eval_lambda(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    ਪੂਰਨ		verbose)	// give error messages
{
    ਪੂਰਨ		evaluate = evalarg != ਨਿਰਾ
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    typval_T	ਅਧਾਰ = *rettv;
    ਪੂਰਨ		ret;

    rettv->v_type = VAR_UNKNOWN;

    ਜੇ (**arg == '{')
    {
	// ->{lambda}()
	ret = get_lambda_tv(arg, rettv, FALSE, evalarg, ਨਿਰਾ);
    }
    ਨਹੀਂਤਾਂ
    {
	// ->(lambda)()
	++*arg;
	ret = eval1(arg, rettv, evalarg);
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	ਜੇ (**arg != ')')
	{
	    emsg(_(e_missing_closing_paren));
	    ਵਾਪਸ FAIL;
	}
	ਜੇ (rettv->v_type != VAR_STRING && rettv->v_type != VAR_FUNC
					       && rettv->v_type != VAR_PARTIAL)
	{
	    emsg(_(e_string_or_function_required_for_arrow_parens_ਵਿਅੰਜਕ));
	    ਵਾਪਸ FAIL;
	}
	++*arg;
    }
    ਜੇ (ret != OK)
	ਵਾਪਸ FAIL;

    ਜੇ (**arg != '(')
    {
	ਜੇ (verbose)
	{
	    ਜੇ (*skipwhite(*arg) == '(')
		emsg(_(e_no_white_space_allowed_before_parenthesis));
	    ਨਹੀਂਤਾਂ
		semsg(_(e_missing_parenthesis_str), "lambda");
	}
	clear_tv(rettv);
	ret = FAIL;
    }
    ਨਹੀਂਤਾਂ
	ret = call_func_rettv(arg, evalarg, rettv, evaluate, ਨਿਰਾ, &ਅਧਾਰ);

    // Clear the funcref afterwards, so that deleting it while
    // evaluating the arguments is possible (see test55).
    ਜੇ (evaluate)
	clear_tv(&ਅਧਾਰ);

    ਵਾਪਸ ret;
}

/*
 * Evaluate "->method()".
 * "*arg" points to "method".
 * Returns FAIL or OK. "*arg" is advanced to after the ')'.
 */
    ਸਥਿਰ ਪੂਰਨ
eval_ਵਿਧੀ(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    ਪੂਰਨ		verbose)	// give error messages
{
    char_u	*name;
    ਲੰਮਾ	len;
    char_u	*alias;
    char_u	*tofree = ਨਿਰਾ;
    typval_T	ਅਧਾਰ = *rettv;
    ਪੂਰਨ		ret = OK;
    ਪੂਰਨ		evaluate = evalarg != ਨਿਰਾ
				      && (evalarg->eval_flags & EVAL_EVALUATE);

    rettv->v_type = VAR_UNKNOWN;

    name = *arg;
    len = get_name_len(arg, &alias, evaluate, evaluate);
    ਜੇ (alias != ਨਿਰਾ)
	name = alias;

    ਜੇ (len <= 0)
    {
	ਜੇ (verbose)
	    emsg(_(e_missing_name_after_ਵਿਧੀ));
	ret = FAIL;
    }
    ਨਹੀਂਤਾਂ
    {
	char_u *paren;

	// If there is no "(" immediately following, but there is further on,
	// it can be "import.Func()", "dict.Func()", "list[nr]", etc.
	// Does not handle anything where "(" is part of the expression.
	*arg = skipwhite(*arg);

	ਜੇ (**arg != '(' && alias == ਨਿਰਾ
				    && (paren = vim_strchr(*arg, '(')) != ਨਿਰਾ)
	{
	    *arg = name;

	    // Truncate the name at the "(".  Avoid trying to get another line
	    // by making "getline" NULL.
	    *paren = NUL;
	    char_u	*(*getline)(ਪੂਰਨ, ਖਾਲੀ *, ਪੂਰਨ, getline_opt_T) = ਨਿਰਾ;
	    ਜੇ (evalarg != ਨਿਰਾ)
	    {
		getline = evalarg->eval_getline;
		evalarg->eval_getline = ਨਿਰਾ;
	    }

	    char_u *deref = deref_function_name(arg, &tofree, evalarg, verbose);
	    ਜੇ (deref == ਨਿਰਾ)
	    {
		*arg = name + len;
		ret = FAIL;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		name = deref;
		len = (ਲੰਮਾ)STRLEN(name);
	    }

	    *paren = '(';
	    ਜੇ (getline != ਨਿਰਾ)
		evalarg->eval_getline = getline;
	}

	ਜੇ (ret == OK)
	{
	    *arg = skipwhite(*arg);

	    ਜੇ (**arg != '(')
	    {
		ਜੇ (verbose)
		    semsg(_(e_missing_parenthesis_str), name);
		ret = FAIL;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (VIM_ISWHITE((*arg)[-1]))
	    {
		ਜੇ (verbose)
		    emsg(_(e_no_white_space_allowed_before_parenthesis));
		ret = FAIL;
	    }
	    ਨਹੀਂਤਾਂ
		ret = eval_func(arg, evalarg, name, len, rettv,
					  evaluate ? EVAL_EVALUATE : 0, &ਅਧਾਰ);
	}
    }

    // Clear the funcref afterwards, so that deleting it while
    // evaluating the arguments is possible (see test55).
    ਜੇ (evaluate)
	clear_tv(&ਅਧਾਰ);
    vim_free(tofree);

    ਜੇ (alias != ਨਿਰਾ)
	vim_free(alias);

    ਵਾਪਸ ret;
}

/*
 * Evaluate an "[expr]" or "[expr:expr]" index.  Also "dict.key".
 * "*arg" points to the '[' or '.'.
 * Returns FAIL or OK. "*arg" is advanced to after the ']'.
 */
    ਸਥਿਰ ਪੂਰਨ
eval_ਸੂਚਕਾਂਕ(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    ਪੂਰਨ		verbose)	// give error messages
{
    ਪੂਰਨ		evaluate = evalarg != ਨਿਰਾ
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    ਪੂਰਨ		empty1 = FALSE, empty2 = FALSE;
    typval_T	var1, var2;
    ਪੂਰਨ		range = FALSE;
    char_u	*ਕੁੰਜੀ = ਨਿਰਾ;
    ਪੂਰਨ		keylen = -1;
    ਪੂਰਨ		vim9script = ਵਿੱਚ_vim9script();

    ਜੇ (check_can_ਸੂਚਕਾਂਕ(rettv, evaluate, verbose) == FAIL)
	ਵਾਪਸ FAIL;

    init_tv(&var1);
    init_tv(&var2);
    ਜੇ (**arg == '.')
    {
	/*
	 * dict.name
	 */
	ਕੁੰਜੀ = *arg + 1;
	ਲਈ (keylen = 0; eval_isdictc(ਕੁੰਜੀ[keylen]); ++keylen)
	    ;
	ਜੇ (keylen == 0)
	    ਵਾਪਸ FAIL;
	ਜੇ (vim9script && ਕੁੰਜੀ[keylen] == '<')
	{
	    // skip generic type arguments
	    char_u	*p = &ਕੁੰਜੀ[keylen];

	    ਜੇ (skip_generic_func_type_ਦਲੀਲਾਂ(&p) == FAIL)
		ਵਾਪਸ FAIL;
	    keylen = p - ਕੁੰਜੀ;
	}
	*arg = ਕੁੰਜੀ + keylen;
    }
    ਨਹੀਂਤਾਂ
    {
	/*
	 * something[idx]
	 *
	 * Get the (first) variable from inside the [].
	 */
	*arg = skipwhite_and_linebreak(*arg + 1, evalarg);
	ਜੇ (**arg == ':')
	    empty1 = TRUE;
	ਨਹੀਂਤਾਂ ਜੇ (eval1(arg, &var1, evalarg) == FAIL)	// recursive!
	    ਵਾਪਸ FAIL;
	ਨਹੀਂਤਾਂ ਜੇ (vim9script && **arg == ':')
	{
	    semsg(_(e_white_space_required_before_and_after_str_at_str),
								    ":", *arg);
	    clear_tv(&var1);
	    ਵਾਪਸ FAIL;
	}
	ਨਹੀਂਤਾਂ ਜੇ (evaluate)
	{
	    ਪੂਰਨ error = FALSE;

	    // allow for indexing with float
	    ਜੇ (vim9script && rettv->v_type == VAR_DICT
						   && var1.v_type == VAR_FLOAT)
	    {
		var1.vval.v_string = typval_tostring(&var1, TRUE);
		var1.v_type = VAR_STRING;
	    }

	    ਜੇ (vim9script && (rettv->v_type == VAR_LIST
						|| rettv->v_type == VAR_TUPLE))
		tv_get_ਸੰਖਿਆ_chk(&var1, &error);
	    ਨਹੀਂਤਾਂ
		error = tv_get_string_chk(&var1) == ਨਿਰਾ;
	    ਜੇ (error)
	    {
		// not a number or string
		clear_tv(&var1);
		ਵਾਪਸ FAIL;
	    }
	}

	/*
	 * Get the second variable from inside the [:].
	 */
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	ਜੇ (**arg == ':')
	{
	    range = TRUE;
	    ++*arg;
	    ਜੇ (vim9script && !IS_WHITE_OR_NUL(**arg) && **arg != ']')
	    {
		semsg(_(e_white_space_required_before_and_after_str_at_str),
								":", *arg - 1);
		ਜੇ (!empty1)
		    clear_tv(&var1);
		ਵਾਪਸ FAIL;
	    }
	    *arg = skipwhite_and_linebreak(*arg, evalarg);
	    ਜੇ (**arg == ']')
		empty2 = TRUE;
	    ਨਹੀਂਤਾਂ ਜੇ (eval1(arg, &var2, evalarg) == FAIL)	// recursive!
	    {
		ਜੇ (!empty1)
		    clear_tv(&var1);
		ਵਾਪਸ FAIL;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (evaluate && tv_get_string_chk(&var2) == ਨਿਰਾ)
	    {
		// not a number or string
		ਜੇ (!empty1)
		    clear_tv(&var1);
		clear_tv(&var2);
		ਵਾਪਸ FAIL;
	    }
	}

	// Check for the ']'.
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	ਜੇ (**arg != ']')
	{
	    ਜੇ (verbose)
		emsg(_(e_missing_closing_ਵਰਗਫਲ_brace));
	    clear_tv(&var1);
	    ਜੇ (range)
		clear_tv(&var2);
	    ਵਾਪਸ FAIL;
	}
	*arg = *arg + 1;	// skip over the ']'
    }

    ਜੇ (evaluate)
    {
	ਪੂਰਨ res = eval_ਸੂਚਕਾਂਕ_inner(rettv, range,
		empty1 ? ਨਿਰਾ : &var1, empty2 ? ਨਿਰਾ : &var2, FALSE,
		ਕੁੰਜੀ, keylen, verbose);

	ਜੇ (!empty1)
	    clear_tv(&var1);
	ਜੇ (range)
	    clear_tv(&var2);
	ਵਾਪਸ res;
    }
    ਵਾਪਸ OK;
}

/*
 * Check if "rettv" can have an [index] or [sli:ce]
 */
    ਪੂਰਨ
check_can_ਸੂਚਕਾਂਕ(typval_T *rettv, ਪੂਰਨ evaluate, ਪੂਰਨ verbose)
{
    ਚੋਣ (rettv->v_type)
    {
	ਹਾਲਤ VAR_FUNC:
	ਹਾਲਤ VAR_PARTIAL:
	    ਜੇ (verbose)
		emsg(_(e_cannot_ਸੂਚਕਾਂਕ_a_funcref));
	    ਵਾਪਸ FAIL;
	ਹਾਲਤ VAR_FLOAT:
	    ਜੇ (verbose)
		emsg(_(e_using_float_as_string));
	    ਵਾਪਸ FAIL;
	ਹਾਲਤ VAR_BOOL:
	ਹਾਲਤ VAR_SPECIAL:
	ਹਾਲਤ VAR_ਕੰਮ:
	ਹਾਲਤ VAR_CHANNEL:
	ਹਾਲਤ VAR_INSTR:
	ਹਾਲਤ VAR_OBJECT:
	    ਜੇ (verbose)
		emsg(_(e_cannot_ਸੂਚਕਾਂਕ_special_variable));
	    ਵਾਪਸ FAIL;
	ਹਾਲਤ VAR_CLASS:
	ਹਾਲਤ VAR_TYPEALIAS:
	    ਜੇ (verbose)
		check_typval_is_ਮੁੱਲ(rettv);
	    ਵਾਪਸ FAIL;
	ਹਾਲਤ VAR_UNKNOWN:
	ਹਾਲਤ VAR_ANY:
	ਹਾਲਤ VAR_VOID:
	    ਜੇ (evaluate)
	    {
		emsg(_(e_cannot_ਸੂਚਕਾਂਕ_special_variable));
		ਵਾਪਸ FAIL;
	    }
	    // FALLTHROUGH

	ਹਾਲਤ VAR_STRING:
	ਹਾਲਤ VAR_LIST:
	ਹਾਲਤ VAR_TUPLE:
	ਹਾਲਤ VAR_DICT:
	ਹਾਲਤ VAR_BLOB:
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_ਸੰਖਿਆ:
	    ਜੇ (ਵਿੱਚ_vim9script())
		emsg(_(e_cannot_ਸੂਚਕਾਂਕ_ਸੰਖਿਆ));
	    ਰੋਕੋ;
    }
    ਵਾਪਸ OK;
}

/*
 * Apply index or range to "rettv".
 * "var1" is the first index, NULL for [:expr].
 * "var2" is the second index, NULL for [expr] and [expr: ]
 * "exclusive" is TRUE for slice(): second index is exclusive, use character
 * index for string.
 * Alternatively, "key" is not NULL, then key[keylen] is the dict index.
 */
    ਪੂਰਨ
eval_ਸੂਚਕਾਂਕ_inner(
	typval_T    *rettv,
	ਪੂਰਨ	    is_range,
	typval_T    *var1,
	typval_T    *var2,
	ਪੂਰਨ	    exclusive,
	char_u	    *ਕੁੰਜੀ,
	ਪੂਰਨ	    keylen,
	ਪੂਰਨ	    verbose)
{
    varnumber_T	    n1, n2 = 0;
    ਲੰਮਾ	    len;

    n1 = 0;
    ਜੇ (var1 != ਨਿਰਾ && rettv->v_type != VAR_DICT)
	n1 = tv_get_ਸੰਖਿਆ(var1);

    ਜੇ (is_range)
    {
	ਜੇ (rettv->v_type == VAR_DICT)
	{
	    ਜੇ (verbose)
		emsg(_(e_cannot_ਟੁਕੜਾ_dictionary));
	    ਵਾਪਸ FAIL;
	}
	ਜੇ (var2 != ਨਿਰਾ)
	    n2 = tv_get_ਸੰਖਿਆ(var2);
	ਨਹੀਂਤਾਂ
	    n2 = VARNUM_MAX;
    }

    ਚੋਣ (rettv->v_type)
    {
	ਹਾਲਤ VAR_UNKNOWN:
	ਹਾਲਤ VAR_ANY:
	ਹਾਲਤ VAR_VOID:
	ਹਾਲਤ VAR_FUNC:
	ਹਾਲਤ VAR_PARTIAL:
	ਹਾਲਤ VAR_FLOAT:
	ਹਾਲਤ VAR_BOOL:
	ਹਾਲਤ VAR_SPECIAL:
	ਹਾਲਤ VAR_ਕੰਮ:
	ਹਾਲਤ VAR_CHANNEL:
	ਹਾਲਤ VAR_INSTR:
	ਹਾਲਤ VAR_CLASS:
	ਹਾਲਤ VAR_OBJECT:
	ਹਾਲਤ VAR_TYPEALIAS:
	    ਰੋਕੋ; // not evaluating, skipping over subscript

	ਹਾਲਤ VAR_ਸੰਖਿਆ:
	ਹਾਲਤ VAR_STRING:
	    {
		char_u	*s = tv_get_string(rettv);

		len = (ਲੰਮਾ)STRLEN(s);
		ਜੇ (ਵਿੱਚ_vim9script() || exclusive)
		{
		    ਜੇ (is_range)
			s = string_ਟੁਕੜਾ(s, n1, n2, exclusive);
		    ਨਹੀਂਤਾਂ
			s = char_from_string(s, n1);
		}
		ਨਹੀਂਤਾਂ ਜੇ (is_range)
		{
		    // The resulting variable is a substring.  If the indexes
		    // are out of range the result is empty.
		    ਜੇ (n1 < 0)
		    {
			n1 = len + n1;
			ਜੇ (n1 < 0)
			    n1 = 0;
		    }
		    ਜੇ (n2 < 0)
			n2 = len + n2;
		    ਨਹੀਂਤਾਂ ਜੇ (n2 >= len)
			n2 = len;
		    ਜੇ (n1 >= len || n2 < 0 || n1 > n2)
			s = ਨਿਰਾ;
		    ਨਹੀਂਤਾਂ
			s = vim_strnsave(s + n1, n2 - n1 + 1);
		}
		ਨਹੀਂਤਾਂ
		{
		    // The resulting variable is a string of a single
		    // character.  If the index is too big or negative the
		    // result is empty.
		    ਜੇ (n1 >= len || n1 < 0)
			s = ਨਿਰਾ;
		    ਨਹੀਂਤਾਂ
			s = vim_strnsave(s + n1, 1);
		}
		clear_tv(rettv);
		rettv->v_type = VAR_STRING;
		rettv->vval.v_string = s;
	    }
	    ਰੋਕੋ;

	ਹਾਲਤ VAR_BLOB:
	    blob_ਟੁਕੜਾ_or_ਸੂਚਕਾਂਕ(rettv->vval.v_blob, is_range, n1, n2,
							     exclusive, rettv);
	    ਰੋਕੋ;

	ਹਾਲਤ VAR_LIST:
	    ਜੇ (var1 == ਨਿਰਾ)
		n1 = 0;
	    ਜੇ (var2 == ਨਿਰਾ)
		n2 = VARNUM_MAX;
	    ਜੇ (list_ਟੁਕੜਾ_or_ਸੂਚਕਾਂਕ(rettv->vval.v_list,
			  is_range, n1, n2, exclusive, rettv, verbose) == FAIL)
		ਵਾਪਸ FAIL;
	    ਰੋਕੋ;

	ਹਾਲਤ VAR_TUPLE:
	    ਜੇ (var1 == ਨਿਰਾ)
		n1 = 0;
	    ਜੇ (var2 == ਨਿਰਾ)
		n2 = VARNUM_MAX;
	    ਜੇ (tuple_ਟੁਕੜਾ_or_ਸੂਚਕਾਂਕ(rettv->vval.v_tuple,
			  is_range, n1, n2, exclusive, rettv, verbose) == FAIL)
		ਵਾਪਸ FAIL;
	    ਰੋਕੋ;

	ਹਾਲਤ VAR_DICT:
	    {
		dictitem_T	*ਵਸਤੂ;
		typval_T	tmp;

		ਜੇ (ਕੁੰਜੀ == ਨਿਰਾ)
		{
		    ਕੁੰਜੀ = tv_get_string_chk(var1);
		    ਜੇ (ਕੁੰਜੀ == ਨਿਰਾ)
			ਵਾਪਸ FAIL;
		}

		ਵਸਤੂ = dict_ਲੱਭੋ(rettv->vval.v_dict, ਕੁੰਜੀ, keylen);

		ਜੇ (ਵਸਤੂ == ਨਿਰਾ)
		{
		    ਜੇ (verbose)
		    {
			ਜੇ (keylen > 0)
			    ਕੁੰਜੀ[keylen] = NUL;
			semsg(_(e_ਕੁੰਜੀ_not_present_ਵਿੱਚ_dictionary_str), ਕੁੰਜੀ);
		    }
		    ਵਾਪਸ FAIL;
		}

		copy_tv(&ਵਸਤੂ->di_tv, &tmp);
		clear_tv(rettv);
		*rettv = tmp;
	    }
	    ਰੋਕੋ;
    }
    ਵਾਪਸ OK;
}

/*
 * Return the function name of partial "pt".
 */
    char_u *
partial_name(partial_T *pt)
{
    ਜੇ (pt != ਨਿਰਾ)
    {
	ਜੇ (pt->pt_name != ਨਿਰਾ)
	    ਵਾਪਸ pt->pt_name;
	ਜੇ (pt->pt_func != ਨਿਰਾ)
	    ਵਾਪਸ pt->pt_func->uf_name;
    }
    ਵਾਪਸ (char_u *)"";
}

    ਸਥਿਰ ਖਾਲੀ
partial_free(partial_T *pt)
{
    ਪੂਰਨ i;

    ਲਈ (i = 0; i < pt->pt_argc; ++i)
	clear_tv(&pt->pt_argv[i]);
    vim_free(pt->pt_argv);
    dict_unref(pt->pt_dict);
    ਜੇ (pt->pt_name != ਨਿਰਾ)
    {
	func_unref(pt->pt_name);
	vim_free(pt->pt_name);
    }
    ਨਹੀਂਤਾਂ
	func_ptr_unref(pt->pt_func);
    ਜੇ (pt->pt_obj != ਨਿਰਾ)
	object_unref(pt->pt_obj);

    // "out_up" is no longer used, decrement refcount on partial that owns it.
    partial_unref(pt->pt_outer.ਬਾਹਰਲਾ_ਉੱਪਰ_partial);

    // Using pt_outer from another partial.
    partial_unref(pt->pt_outer_partial);

    // Decrease the reference count for the context of a closure.  If down
    // to the minimum it may be time to free it.
    ਜੇ (pt->pt_funcstack != ਨਿਰਾ)
    {
	--pt->pt_funcstack->fs_refcount;
	funcstack_check_refcount(pt->pt_funcstack);
    }
    // Similarly for loop variables.
    ਲਈ (i = 0; i < MAX_LOOP_ਡੂੰਘਾਈ; ++i)
	ਜੇ (pt->pt_loopvars[i] != ਨਿਰਾ)
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
    ਖਾਲੀ
partial_unref(partial_T *pt)
{
    ਜੇ (pt == ਨਿਰਾ)
	ਵਾਪਸ;

    ਪੂਰਨ	ਪੂਰਾ = FALSE;

    ਜੇ (--pt->pt_refcount <= 0)
	partial_free(pt);

    // If the reference count goes down to one, the funcstack may be the
    // only reference and can be freed if no other partials reference it.
    ਨਹੀਂਤਾਂ ਜੇ (pt->pt_refcount == 1)
    {
	// careful: if the funcstack is freed it may contain this partial
	// and it gets freed as well
	ਜੇ (pt->pt_funcstack != ਨਿਰਾ)
	    ਪੂਰਾ = funcstack_check_refcount(pt->pt_funcstack);

	ਜੇ (!ਪੂਰਾ)
	{
	    ਪੂਰਨ	ਡੂੰਘਾਈ;

	    ਲਈ (ਡੂੰਘਾਈ = 0; ਡੂੰਘਾਈ < MAX_LOOP_ਡੂੰਘਾਈ; ++ਡੂੰਘਾਈ)
		ਜੇ (pt->pt_loopvars[ਡੂੰਘਾਈ] != ਨਿਰਾ
			&& loopvars_check_refcount(pt->pt_loopvars[ਡੂੰਘਾਈ]))
		    ਰੋਕੋ;
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
    ਸਥਿਰ char_u *
string_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    ਪੂਰਨ		echo_style,
    ਪੂਰਨ		ਸੰਯੁਕਤ_val)
{
    char_u	*r = ਨਿਰਾ;

    ਜੇ (echo_style && !ਸੰਯੁਕਤ_val)
    {
	*tofree = ਨਿਰਾ;
	r = tv->vval.v_string;
	ਜੇ (r == ਨਿਰਾ)
	    r = (char_u *)"";
    }
    ਨਹੀਂਤਾਂ
    {
	*tofree = string_quote(tv->vval.v_string, FALSE);
	r = *tofree;
    }

    ਵਾਪਸ r;
}

/*
 * Return a textual representation of a function in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "echo_style" is FALSE, put quotes around the function name as
 * "function()", otherwise does not put quotes around function name.
 * May return NULL.
 */
    ਸਥਿਰ char_u *
func_tv2string(typval_T *tv, char_u **tofree, ਪੂਰਨ echo_style)
{
    char_u	*r = ਨਿਰਾ;
    char_u	buf[MAX_FUNC_NAME_LEN];

    ਜੇ (echo_style)
    {
	*tofree = ਨਿਰਾ;

	ਜੇ (tv->vval.v_string == ਨਿਰਾ)
	    r = (char_u *)"function()";
	ਨਹੀਂਤਾਂ
	{
	    r = make_ufunc_name_readable(tv->vval.v_string, buf,
							MAX_FUNC_NAME_LEN);
	    ਜੇ (r == buf)
		r = *tofree = vim_strsave(buf);
	}
    }
    ਨਹੀਂਤਾਂ
    {
	char_u *s = ਨਿਰਾ;

	ਜੇ (tv->vval.v_string != ਨਿਰਾ)
	    s = make_ufunc_name_readable(tv->vval.v_string, buf,
							MAX_FUNC_NAME_LEN);

	r = *tofree = string_quote(s, TRUE);
    }

    ਵਾਪਸ r;
}

/*
 * Return a textual representation of the object method in "tv", a VAR_PARTIAL.
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "echo_style" is FALSE, put quotes around the function name as
 * "function()", otherwise does not put quotes around function name.
 * May return NULL.
 */
    ਸਥਿਰ char_u *
ਵਿਧੀ_tv2string(typval_T *tv, char_u **tofree, ਪੂਰਨ echo_style)
{
    char_u	buf[MAX_FUNC_NAME_LEN];
    partial_T	*pt = tv->vval.v_partial;

    size_t len = vim_snprintf((ਅੱਖਰ *)buf, ਆਕਾਰ(buf), "<SNR>%d_%s.%s",
			   pt->pt_func->uf_script_ctx.sc_sid,
			   pt->pt_func->uf_class->class_name.string,
			   pt->pt_func->uf_name);
    ਜੇ (len >= ਆਕਾਰ(buf))
    {
	ਜੇ (echo_style)
	{
	    *tofree = ਨਿਰਾ;
	    ਵਾਪਸ (char_u *)"function()";
	}
	ਨਹੀਂਤਾਂ
	    ਵਾਪਸ *tofree = string_quote((char_u*)"", TRUE);
    }

    ਵਾਪਸ *tofree = echo_style ? vim_strsave(buf) : string_quote(buf, TRUE);
}

/*
 * Return a textual representation of a partial in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * "numbuf" is used for a number.  May return NULL.
 */
    ਸਥਿਰ char_u *
partial_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    char_u	*numbuf,
    ਪੂਰਨ		copyID)
{
    char_u	*r = ਨਿਰਾ;
    partial_T	*pt;
    char_u	*fname;
    garray_T	ga;
    ਪੂਰਨ		i;
    char_u	*tf;

    pt = tv->vval.v_partial;
    fname = string_quote(pt == ਨਿਰਾ ? ਨਿਰਾ : partial_name(pt), FALSE);

    ga_init2(&ga, 1, 100);
    GA_CONCAT_LITERAL(&ga, "function(");
    ਜੇ (fname != ਨਿਰਾ)
    {
	// When using uf_name prepend "g:" for a global function.
	ਜੇ (pt != ਨਿਰਾ && pt->pt_name == ਨਿਰਾ && fname[0] == '\''
						&& vim_isupper(fname[1]))
	{
	    GA_CONCAT_LITERAL(&ga, "'g:");
	    ga_concat(&ga, fname + 1);
	}
	ਨਹੀਂਤਾਂ
	    ga_concat(&ga, fname);
	vim_free(fname);
    }
    ਜੇ (pt != ਨਿਰਾ && pt->pt_argc > 0)
    {
	GA_CONCAT_LITERAL(&ga, ", [");
	ਲਈ (i = 0; i < pt->pt_argc; ++i)
	{
	    ਜੇ (i > 0)
		GA_CONCAT_LITERAL(&ga, ", ");
	    ga_concat(&ga, tv2string(&pt->pt_argv[i], &tf, numbuf, copyID));
	    vim_free(tf);
	}
	GA_CONCAT_LITERAL(&ga, "]");
    }
    ਜੇ (pt != ਨਿਰਾ && pt->pt_dict != ਨਿਰਾ)
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

    *tofree = ga.ga_ਅੰਕੜੇ;
    r = *tofree;

    ਵਾਪਸ r;
}

/*
 * Return a textual representation of a List in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "copyID" is not zero replace recursive lists with "...".  When
 * "restore_copyID" is FALSE, repeated items in lists are replaced with "...".
 * May return NULL.
 */
    ਸਥਿਰ char_u *
list_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    ਪੂਰਨ		copyID,
    ਪੂਰਨ		restore_copyID)
{
    char_u	*r = ਨਿਰਾ;

    ਜੇ (tv->vval.v_list == ਨਿਰਾ)
    {
	// NULL list is equivalent to empty list.
	*tofree = ਨਿਰਾ;
	r = (char_u *)"[]";
    }
    ਨਹੀਂਤਾਂ ਜੇ (copyID != 0 && tv->vval.v_list->lv_copyID == copyID
	    && tv->vval.v_list->lv_len > 0)
    {
	*tofree = ਨਿਰਾ;
	r = (char_u *)"[...]";
    }
    ਨਹੀਂਤਾਂ
    {
	ਪੂਰਨ old_copyID;
	ਜੇ (restore_copyID)
	    old_copyID = tv->vval.v_list->lv_copyID;

	tv->vval.v_list->lv_copyID = copyID;
	*tofree = list2string(tv, copyID, restore_copyID);
	ਜੇ (restore_copyID)
	    tv->vval.v_list->lv_copyID = old_copyID;
	r = *tofree;
    }

    ਵਾਪਸ r;
}

/*
 * Return a textual representation of a Tuple in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "copyID" is not zero replace recursive lists with "...".  When
 * "restore_copyID" is FALSE, repeated items in tuples are replaced with "...".
 * May return NULL.
 */
    ਸਥਿਰ char_u *
tuple_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    ਪੂਰਨ		copyID,
    ਪੂਰਨ		restore_copyID)
{
    tuple_T	*tuple = tv->vval.v_tuple;
    char_u	*r = ਨਿਰਾ;

    ਜੇ (tuple == ਨਿਰਾ)
    {
	// NULL tuple is equivalent to an empty tuple.
	*tofree = ਨਿਰਾ;
	r = (char_u *)"()";
    }
    ਨਹੀਂਤਾਂ ਜੇ (copyID != 0 && tuple->tv_copyID == copyID
					&& tuple->tv_ਚੀਜ਼ਾਂ.ga_len > 0)
    {
	*tofree = ਨਿਰਾ;
	r = (char_u *)"(...)";
    }
    ਨਹੀਂਤਾਂ
    {
	ਪੂਰਨ old_copyID;
	ਜੇ (restore_copyID)
	    old_copyID = tuple->tv_copyID;

	tuple->tv_copyID = copyID;
	*tofree = tuple2string(tv, copyID, restore_copyID);
	ਜੇ (restore_copyID)
	    tuple->tv_copyID = old_copyID;
	r = *tofree;
    }

    ਵਾਪਸ r;
}

/*
 * Return a textual representation of a Dict in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "copyID" is not zero replace recursive dicts with "...".
 * When "restore_copyID" is FALSE, repeated items in the dictionary are
 * replaced with "...".  May return NULL.
 */
    ਸਥਿਰ char_u *
dict_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    ਪੂਰਨ		copyID,
    ਪੂਰਨ		restore_copyID)
{
    char_u	*r = ਨਿਰਾ;

    ਜੇ (tv->vval.v_dict == ਨਿਰਾ)
    {
	// NULL dict is equivalent to empty dict.
	*tofree = ਨਿਰਾ;
	r = (char_u *)"{}";
    }
    ਨਹੀਂਤਾਂ ਜੇ (copyID != 0 && tv->vval.v_dict->dv_copyID == copyID
	    && tv->vval.v_dict->dv_hashtab.ht_ਵਰਤਿਆ != 0)
    {
	*tofree = ਨਿਰਾ;
	r = (char_u *)"{...}";
    }
    ਨਹੀਂਤਾਂ
    {
	ਪੂਰਨ old_copyID;
	ਜੇ (restore_copyID)
	    old_copyID = tv->vval.v_dict->dv_copyID;

	tv->vval.v_dict->dv_copyID = copyID;
	*tofree = dict2string(tv, copyID, restore_copyID);
	ਜੇ (restore_copyID)
	    tv->vval.v_dict->dv_copyID = old_copyID;
	r = *tofree;
    }

    ਵਾਪਸ r;
}

/*
 * Return a textual representation of a job or a channel in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * "numbuf" is used for a number.
 * When "composite_val" is FALSE, put quotes around strings as "string()",
 * otherwise does not put quotes around strings.
 * May return NULL.
 */
    ਸਥਿਰ char_u *
jobchan_tv2string(
    typval_T	*tv ਅਣਵਰਤਿਆ,
    char_u	**tofree ਅਣਵਰਤਿਆ,
    char_u	*numbuf ਅਣਵਰਤਿਆ,
    ਪੂਰਨ		ਸੰਯੁਕਤ_val ਅਣਵਰਤਿਆ)
{
    char_u	*r = ਨਿਰਾ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
    *tofree = ਨਿਰਾ;

    ਜੇ (tv->v_type == VAR_ਕੰਮ)
	r = ਕੰਮ_ਮੰਜ਼ਿਲ_string_buf(tv, numbuf);
    ਨਹੀਂਤਾਂ
	r = channel_ਮੰਜ਼ਿਲ_string_buf(tv, numbuf);

    ਜੇ (ਸੰਯੁਕਤ_val)
    {
	*tofree = string_quote(r, FALSE);
	r = *tofree;
    }
#ਜੇ_ਅੰਤ

    ਵਾਪਸ r;
}

/*
 * Return a textual representation of a class in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * May return NULL.
 */
    ਸਥਿਰ char_u *
class_tv2string(typval_T *tv, char_u **tofree)
{
    char_u	*r = ਨਿਰਾ;
    size_t	rsize;
    class_T	*cl = tv->vval.v_class;
    string_T	class_name = {(char_u *)"[unknown]", 9};
    string_T	s = {(char_u *)"class", 5};

    ਜੇ (cl != ਨਿਰਾ)
    {
	class_name.string = cl->class_name.string;
	class_name.ਲੰਮਾਈ = cl->class_name.ਲੰਮਾਈ;
	ਜੇ (IS_INTERFACE(cl))
	    STR_LITERAL_ਸੈੱਟ(s, "interface");
	ਨਹੀਂਤਾਂ ਜੇ (IS_ENUM(cl))
	    STR_LITERAL_ਸੈੱਟ(s, "enum");
    }

    rsize = s.ਲੰਮਾਈ + 1 + class_name.ਲੰਮਾਈ + 1;
    r = *tofree = alloc(rsize);
    ਜੇ (r != ਨਿਰਾ)
	vim_snprintf((ਅੱਖਰ *)r, rsize, "%s %s", s.string, (ਅੱਖਰ *)class_name.string);

    ਵਾਪਸ r;
}

/*
 * Return a textual representation of an Object in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "copyID" is not zero replace recursive object with "...".
 * When "restore_copyID" is FALSE, repeated items in the object are
 * replaced with "...".  May return NULL.
 */
    ਸਥਿਰ char_u *
object_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    ਪੂਰਨ		copyID,
    ਪੂਰਨ		restore_copyID,
    char_u	*numbuf,
    ਪੂਰਨ		echo_style,
    ਪੂਰਨ		ਸੰਯੁਕਤ_val)
{
    char_u	*r = ਨਿਰਾ;

    object_T	*obj = tv->vval.v_object;
    ਜੇ (obj == ਨਿਰਾ || obj->obj_class == ਨਿਰਾ)
    {
	*tofree = ਨਿਰਾ;
	r = (char_u *)"object of [unknown]";
    }
    ਨਹੀਂਤਾਂ ਜੇ (copyID != 0 && obj->obj_copyID == copyID
	    && obj->obj_class->class_obj_member_ਗਿਣਤੀ != 0)
    {
	size_t n = 25 + obj->obj_class->class_name.ਲੰਮਾਈ;
	r = alloc(n);
	ਜੇ (r != ਨਿਰਾ)
	    (ਖਾਲੀ)vim_snprintf((ਅੱਖਰ *)r, n, "object of %s {...}",
						obj->obj_class->class_name.string);
	*tofree = r;
    }
    ਨਹੀਂਤਾਂ
    {
	ਪੂਰਨ old_copyID;
	ਜੇ (restore_copyID)
	    old_copyID = obj->obj_copyID;

	obj->obj_copyID = copyID;
	*tofree = object2string(obj, numbuf, copyID, echo_style,
				restore_copyID, ਸੰਯੁਕਤ_val);
	ਜੇ (restore_copyID)
	    obj->obj_copyID = old_copyID;
	r = *tofree;
    }

    ਵਾਪਸ r;
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
    ਪੂਰਨ		copyID,
    ਪੂਰਨ		echo_style,
    ਪੂਰਨ		restore_copyID,
    ਪੂਰਨ		ਸੰਯੁਕਤ_val)
{
    ਸਥਿਰ ਪੂਰਨ	recurse = 0;
    char_u	*r = ਨਿਰਾ;

    ਜੇ (recurse >= DICT_MAXNEST)
    {
	ਜੇ (!did_echo_string_emsg)
	{
	    // Only give this message once for a recursive call to avoid
	    // flooding the user with errors.  And stop iterating over lists
	    // and dicts and objects.
	    did_echo_string_emsg = TRUE;
	    emsg(_(e_variable_nested_too_deep_for_displaying));
	}
	*tofree = ਨਿਰਾ;
	ਵਾਪਸ (char_u *)"{E724}";
    }
    ++recurse;

    ਚੋਣ (tv->v_type)
    {
	ਹਾਲਤ VAR_STRING:
	    r = string_tv2string(tv, tofree, echo_style, ਸੰਯੁਕਤ_val);
	    ਰੋਕੋ;

	ਹਾਲਤ VAR_FUNC:
	    r = func_tv2string(tv, tofree, echo_style);
	    ਰੋਕੋ;

	ਹਾਲਤ VAR_PARTIAL:
	    ਜੇ (tv->vval.v_partial == ਨਿਰਾ
		    || tv->vval.v_partial->pt_obj == ਨਿਰਾ)
		r = partial_tv2string(tv, tofree, numbuf, copyID);
	    ਨਹੀਂਤਾਂ
		r = ਵਿਧੀ_tv2string(tv, tofree, echo_style);
	    ਰੋਕੋ;

	ਹਾਲਤ VAR_BLOB:
	    r = blob2string(tv->vval.v_blob, tofree, numbuf);
	    ਰੋਕੋ;

	ਹਾਲਤ VAR_LIST:
	    r = list_tv2string(tv, tofree, copyID, restore_copyID);
	    ਰੋਕੋ;

	ਹਾਲਤ VAR_TUPLE:
	    r = tuple_tv2string(tv, tofree, copyID, restore_copyID);
	    ਰੋਕੋ;

	ਹਾਲਤ VAR_DICT:
	    r = dict_tv2string(tv, tofree, copyID, restore_copyID);
	    ਰੋਕੋ;

	ਹਾਲਤ VAR_ਸੰਖਿਆ:
	ਹਾਲਤ VAR_UNKNOWN:
	ਹਾਲਤ VAR_ANY:
	ਹਾਲਤ VAR_VOID:
	    *tofree = ਨਿਰਾ;
	    r = tv_get_string_buf(tv, numbuf);
	    ਰੋਕੋ;

	ਹਾਲਤ VAR_ਕੰਮ:
	ਹਾਲਤ VAR_CHANNEL:
	    r = jobchan_tv2string(tv, tofree, numbuf, ਸੰਯੁਕਤ_val);
	    ਰੋਕੋ;

	ਹਾਲਤ VAR_INSTR:
	    *tofree = ਨਿਰਾ;
	    r = (char_u *)"instructions";
	    ਰੋਕੋ;

	ਹਾਲਤ VAR_CLASS:
	    r = class_tv2string(tv, tofree);
	    ਰੋਕੋ;

	ਹਾਲਤ VAR_OBJECT:
	    r = object_tv2string(tv, tofree, copyID, restore_copyID,
					 numbuf, echo_style, ਸੰਯੁਕਤ_val);
	    ਰੋਕੋ;

	ਹਾਲਤ VAR_FLOAT:
	    *tofree = ਨਿਰਾ;
	    vim_snprintf((ਅੱਖਰ *)numbuf, NUMBUFLEN, "%g", tv->vval.v_float);
	    r = numbuf;
	    ਰੋਕੋ;

	ਹਾਲਤ VAR_BOOL:
	ਹਾਲਤ VAR_SPECIAL:
	    *tofree = ਨਿਰਾ;
	    r = (char_u *)get_var_special_name(tv->vval.v_ਸੰਖਿਆ);
	    ਰੋਕੋ;

	ਹਾਲਤ VAR_TYPEALIAS:
	    *tofree = vim_strsave(tv->vval.v_typealias->ta_name);
	    r = *tofree;
	    ਜੇ (r == ਨਿਰਾ)
		r = (char_u *)"";
	    ਰੋਕੋ;
    }

    ਜੇ (--recurse == 0)
	did_echo_string_emsg = FALSE;
    ਵਾਪਸ r;
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
    ਪੂਰਨ		copyID)
{
    ਵਾਪਸ echo_string_core(tv, tofree, numbuf, copyID, TRUE, FALSE, FALSE);
}

/*
 * Convert the specified byte index of line 'lnum' in buffer 'buf' to a
 * character index.  Works only for loaded buffers. Returns -1 on failure.
 * The index of the first byte and the first character is zero.
 */
    ਪੂਰਨ
buf_byteidx_ਮੰਜ਼ਿਲ_charidx(buf_T *buf, ਪੂਰਨ lnum, ਪੂਰਨ byteidx)
{
    char_u	*str;
    char_u	*t;
    ਪੂਰਨ		ਗਿਣਤੀ_2;

    ਜੇ (buf == ਨਿਰਾ || buf->b_ml.ml_mfp == ਨਿਰਾ)
	ਵਾਪਸ -1;

    ਜੇ (lnum > buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	lnum = buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;

    str = ml_get_buf(buf, lnum, FALSE);
    ਜੇ (str == ਨਿਰਾ)
	ਵਾਪਸ -1;

    ਜੇ (*str == NUL)
	ਵਾਪਸ 0;

    // count the number of characters
    t = str;
    ਲਈ (ਗਿਣਤੀ_2 = 0; *t != NUL && t <= str + byteidx; ਗਿਣਤੀ_2++)
	t += mb_ptr2len(t);

    // In insert mode, when the cursor is at the end of a non-empty line,
    // byteidx points to the NUL character immediately past the end of the
    // string. In this case, add one to the character count.
    ਜੇ (*t == NUL && byteidx != 0 && t == str + byteidx)
	ਗਿਣਤੀ_2++;

    ਵਾਪਸ ਗਿਣਤੀ_2 - 1;
}

/*
 * Convert the specified character index of line 'lnum' in buffer 'buf' to a
 * byte index.  Works only for loaded buffers. Returns -1 on failure.
 * The index of the first byte and the first character is zero.
 */
    ਪੂਰਨ
buf_charidx_ਮੰਜ਼ਿਲ_byteidx(buf_T *buf, ਪੂਰਨ lnum, ਪੂਰਨ charidx)
{
    char_u	*str;
    char_u	*t;

    ਜੇ (buf == ਨਿਰਾ || buf->b_ml.ml_mfp == ਨਿਰਾ)
	ਵਾਪਸ -1;

    ਜੇ (lnum > buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	lnum = buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;

    str = ml_get_buf(buf, lnum, FALSE);
    ਜੇ (str == ਨਿਰਾ)
	ਵਾਪਸ -1;

    // Convert the character offset to a byte offset
    t = str;
    ਜਦੋਂ (*t != NUL && --charidx > 0)
	t += mb_ptr2len(t);

    ਵਾਪਸ t - str;
}

/*
 * Translate a String variable into a position.
 * Returns NULL when there is an error.
 */
    pos_T *
var2fpos(
    typval_T	*varp,
    ਪੂਰਨ		dollar_lnum,	// TRUE when $ is last line
    ਪੂਰਨ		*fnum,		// set to fnum for '0, 'A, etc.
    ਪੂਰਨ		charcol)	// return character column
{
    char_u		*name;
    ਸਥਿਰ pos_T	pos;
    pos_T		*pp;
    ਪੂਰਨ			error = FALSE;

    // Argument can be [lnum, col, coladd].
    ਜੇ (varp->v_type == VAR_LIST)
    {
	list_T		*l;
	ਪੂਰਨ		len;
	listitem_T	*li;

	l = varp->vval.v_list;
	ਜੇ (l == ਨਿਰਾ)
	    ਵਾਪਸ ਨਿਰਾ;

	// Get the line number
	pos.lnum = list_ਲੱਭੋ_nr(l, 0L, &error);
	ਜੇ (error || pos.lnum <= 0 || pos.lnum > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	    ਵਾਪਸ ਨਿਰਾ;	// invalid line number
	ਜੇ (charcol)
	    len = (ਲੰਮਾ)mb_charlen(ml_get(pos.lnum));
	ਨਹੀਂਤਾਂ
	    len = (ਲੰਮਾ)ml_get_len(pos.lnum);

	// Get the column number
	// We accept "$" for the column number: last column.
	li = list_ਲੱਭੋ(l, 1L);
	ਜੇ (li != ਨਿਰਾ && li->li_tv.v_type == VAR_STRING
		&& li->li_tv.vval.v_string != ਨਿਰਾ
		&& STRCMP(li->li_tv.vval.v_string, "$") == 0)
	{
	    pos.ਖਾਨਾ = len + 1;
	}
	ਨਹੀਂਤਾਂ
	{
	    pos.ਖਾਨਾ = list_ਲੱਭੋ_nr(l, 1L, &error);
	    ਜੇ (error)
		ਵਾਪਸ ਨਿਰਾ;
	}

	// Accept a position up to the NUL after the line.
	ਜੇ (pos.ਖਾਨਾ == 0 || (ਪੂਰਨ)pos.ਖਾਨਾ > len + 1)
	    ਵਾਪਸ ਨਿਰਾ;	// invalid column number
	--pos.ਖਾਨਾ;

	// Get the virtual offset.  Defaults to zero.
	pos.coladd = list_ਲੱਭੋ_nr(l, 2L, &error);
	ਜੇ (error)
	    pos.coladd = 0;

	ਵਾਪਸ &pos;
    }

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_string_arg(varp, 0) == FAIL)
	ਵਾਪਸ ਨਿਰਾ;

    name = tv_get_string_chk(varp);
    ਜੇ (name == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    error = TRUE;
    pos.lnum = 0;
    ਜੇ (name[0] == '.')
    {
	ਜੇ (!ਵਿੱਚ_vim9script() || name[1] == NUL)
	{
	    error = FALSE;
	    // cursor
	    pos = curwin->w_cursor;
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (name[0] == 'v' && name[1] == NUL)
    {
	// Visual start
	ਜੇ (VIsual_ਸਰਗਰਮ)
	    pos = VIsual;
	ਨਹੀਂਤਾਂ
	    pos = curwin->w_cursor;
    }
    ਨਹੀਂਤਾਂ ਜੇ (name[0] == '\'')
    {
	ਜੇ (!ਵਿੱਚ_vim9script() || (name[1] != NUL && name[2] == NUL))
	{
	    error = FALSE;
	    // mark
	    pp = getmark_buf_fnum(curbuf, name[1], FALSE, fnum);
	    ਜੇ (pp == ਨਿਰਾ || pp == (pos_T *)-1 || pp->lnum <= 0)
		ਵਾਪਸ ਨਿਰਾ;
	    pos = *pp;
	}
    }
    ਜੇ (pos.lnum != 0)
    {
	ਜੇ (charcol)
	    pos.ਖਾਨਾ = buf_byteidx_ਮੰਜ਼ਿਲ_charidx(curbuf, pos.lnum, pos.ਖਾਨਾ);
	ਵਾਪਸ &pos;
    }

    pos.coladd = 0;

    ਜੇ (name[0] == 'w' && dollar_lnum)
    {
	// the "w_valid" flags are not reset when moving the cursor, but they
	// do matter for update_topline() and validate_botline().
	check_cursor_moved(curwin);

	pos.ਖਾਨਾ = 0;
	ਜੇ (name[1] == '0')		// "w0": first visible line
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
	    ਜੇ (bt_terminal(curwin->w_ਬਫਰ)
				    && curwin->w_ਬਫਰ->b_ਪਦ != ਨਿਰਾ
				    && !ਪਦ_ਵਿੱਚ_ਸਾਧਾਰਨ_ਬਹੁਲਕ(curwin->w_ਬਫਰ))
		may_move_terminal_ਮੰਜ਼ਿਲ_ਬਫਰ(curwin->w_ਬਫਰ->b_ਪਦ, TRUE);
#ਜੇ_ਅੰਤ
	    update_topline();
	    // In silent Ex mode topline is zero, but that's not a valid line
	    // number; use one instead.
	    pos.lnum = curwin->w_topline > 0 ? curwin->w_topline : 1;
	    ਵਾਪਸ &pos;
	}
	ਨਹੀਂਤਾਂ ਜੇ (name[1] == '$')	// "w$": last visible line
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
	    ਜੇ (bt_terminal(curwin->w_ਬਫਰ)
				    && curwin->w_ਬਫਰ->b_ਪਦ != ਨਿਰਾ
				    && !ਪਦ_ਵਿੱਚ_ਸਾਧਾਰਨ_ਬਹੁਲਕ(curwin->w_ਬਫਰ))
		may_move_terminal_ਮੰਜ਼ਿਲ_ਬਫਰ(curwin->w_ਬਫਰ->b_ਪਦ, TRUE);
#ਜੇ_ਅੰਤ
	    validate_botline();
	    // In silent Ex mode botline is zero, return zero then.
	    pos.lnum = curwin->w_botline > 0 ? curwin->w_botline - 1 : 0;
	    ਵਾਪਸ &pos;
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (name[0] == '$')		// last column or line
    {
	ਜੇ (dollar_lnum)
	{
	    pos.lnum = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
	    pos.ਖਾਨਾ = 0;
	}
	ਨਹੀਂਤਾਂ
	{
	    pos.lnum = curwin->w_cursor.lnum;
	    ਜੇ (charcol)
		pos.ਖਾਨਾ = (colnr_T)mb_charlen(ml_get_curline());
	    ਨਹੀਂਤਾਂ
		pos.ਖਾਨਾ = ml_get_curline_len();
	}
	ਵਾਪਸ &pos;
    }
    ਜੇ (ਵਿੱਚ_vim9script() && error)
	semsg(_(e_invalid_ਮੁੱਲ_for_ਸਤਰ_ਸੰਖਿਆ_str), name);
    ਵਾਪਸ ਨਿਰਾ;
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
    ਪੂਰਨ
list2fpos(
    typval_T	*arg,
    pos_T	*posp,
    ਪੂਰਨ		*fnump,
    colnr_T	*curswantp,
    ਪੂਰਨ		charcol)
{
    list_T	*l = arg->vval.v_list;
    ਲੰਮਾ	i = 0;
    ਲੰਮਾ	n;

    // List must be: [fnum, lnum, col, coladd, curswant], where "fnum" is only
    // there when "fnump" isn't NULL; "coladd" and "curswant" are optional.
    ਜੇ (arg->v_type != VAR_LIST
	    || l == ਨਿਰਾ
	    || l->lv_len < (fnump == ਨਿਰਾ ? 2 : 3)
	    || l->lv_len > (fnump == ਨਿਰਾ ? 4 : 5))
	ਵਾਪਸ FAIL;

    ਜੇ (fnump != ਨਿਰਾ)
    {
	n = list_ਲੱਭੋ_nr(l, i++, ਨਿਰਾ);	// fnum
	ਜੇ (n < 0)
	    ਵਾਪਸ FAIL;
	ਜੇ (n == 0)
	    n = curbuf->b_fnum;		// current buffer
	*fnump = n;
    }

    n = list_ਲੱਭੋ_nr(l, i++, ਨਿਰਾ);	// lnum
    ਜੇ (n < 0)
	ਵਾਪਸ FAIL;
    posp->lnum = n;

    n = list_ਲੱਭੋ_nr(l, i++, ਨਿਰਾ);	//ਖਾਨਾ
    ਜੇ (n < 0)
	ਵਾਪਸ FAIL;
    // If character position is specified, then convert to byte position
    // If the line number is zero use the cursor line.
    ਜੇ (charcol)
    {
	buf_T	*buf;

	// Get the text for the specified line in a loaded buffer
	buf = buflist_findnr(fnump == ਨਿਰਾ ? curbuf->b_fnum : *fnump);
	ਜੇ (buf == ਨਿਰਾ || buf->b_ml.ml_mfp == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;

	n = buf_charidx_ਮੰਜ਼ਿਲ_byteidx(buf,
		  posp->lnum == 0 ? curwin->w_cursor.lnum : posp->lnum, n) + 1;
    }
    posp->ਖਾਨਾ = n;

    n = list_ਲੱਭੋ_nr(l, i, ਨਿਰਾ);	// off
    ਜੇ (n < 0)
	posp->coladd = 0;
    ਨਹੀਂਤਾਂ
	posp->coladd = n;

    ਜੇ (curswantp != ਨਿਰਾ)
	*curswantp = list_ਲੱਭੋ_nr(l, i + 1, ਨਿਰਾ);  // curswant

    ਵਾਪਸ OK;
}

/*
 * Get the length of an environment variable name.
 * Advance "arg" to the first character after the name.
 * Return 0 for error.
 */
    ਪੂਰਨ
get_env_len(char_u **arg)
{
    char_u	*p;
    ਪੂਰਨ		len;

    ਲਈ (p = *arg; vim_isIDc(*p); ++p)
	;
    ਜੇ (p == *arg)	    // no name found
	ਵਾਪਸ 0;

    len = (ਪੂਰਨ)(p - *arg);
    *arg = p;
    ਵਾਪਸ len;
}

/*
 * Get the length of the name of a function or internal variable.
 * "arg" is advanced to after the name.
 * Return 0 if something is wrong.
 */
    ਪੂਰਨ
get_id_len(char_u **arg)
{
    char_u	*p;
    ਪੂਰਨ		len;

    // Find the end of the name.
    ਲਈ (p = *arg; eval_isnamec(*p); ++p)
    {
	ਜੇ (*p == ':')
	{
	    // "s:" is start of "s:var", but "n:" is not and can be used in
	    // slice "[n:]".  Also "xx:" is not a namespace.
	    len = (ਪੂਰਨ)(p - *arg);
	    ਜੇ ((len == 1 && vim_strchr(NAMESPACE_CHAR, **arg) == ਨਿਰਾ)
		    || len > 1)
		ਰੋਕੋ;
	}
    }
    ਜੇ (p == *arg)	    // no name found
	ਵਾਪਸ 0;

    len = (ਪੂਰਨ)(p - *arg);
    *arg = p;

    ਵਾਪਸ len;
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
    ਪੂਰਨ
get_name_len(
    char_u	**arg,
    char_u	**alias,
    ਪੂਰਨ		evaluate,
    ਪੂਰਨ		verbose)
{
    ਪੂਰਨ		len;
    char_u	*p;
    char_u	*ਵਿਅੰਜਕ_ਸ਼ੁਰੂ;
    char_u	*ਵਿਅੰਜਕ_ਸਿਰਾ;

    *alias = ਨਿਰਾ;  // default to no alias

    ਜੇ ((*arg)[0] == K_SPECIAL && (*arg)[1] == KS_EXTRA
						  && (*arg)[2] == (ਪੂਰਨ)KE_SNR)
    {
	// hard coded <SNR>, already translated
	*arg += 3;
	ਵਾਪਸ get_id_len(arg) + 3;
    }
    len = eval_fname_script(*arg);
    ਜੇ (len > 0)
    {
	// literal "<SID>", "s:" or "<SNR>"
	*arg += len;
    }

    /*
     * Find the end of the name; check for {} construction.
     */
    p = ਲੱਭੋ_name_ਸਿਰਾ(*arg, &ਵਿਅੰਜਕ_ਸ਼ੁਰੂ, &ਵਿਅੰਜਕ_ਸਿਰਾ,
					       len > 0 ? 0 : FNE_CHECK_ਸ਼ੁਰੂ);
    ਜੇ (ਵਿਅੰਜਕ_ਸ਼ੁਰੂ != ਨਿਰਾ)
    {
	char_u	*ਆਰਜ਼ੀ_string;

	ਜੇ (!evaluate)
	{
	    len += (ਪੂਰਨ)(p - *arg);
	    *arg = skipwhite(p);
	    ਵਾਪਸ len;
	}

	/*
	 * Include any <SID> etc in the expanded string:
	 * Thus the -len here.
	 */
	ਆਰਜ਼ੀ_string = make_expanded_name(*arg - len, ਵਿਅੰਜਕ_ਸ਼ੁਰੂ, ਵਿਅੰਜਕ_ਸਿਰਾ, p);
	ਜੇ (ਆਰਜ਼ੀ_string == ਨਿਰਾ)
	    ਵਾਪਸ -1;
	*alias = ਆਰਜ਼ੀ_string;
	*arg = skipwhite(p);
	ਵਾਪਸ (ਪੂਰਨ)STRLEN(ਆਰਜ਼ੀ_string);
    }

    len += get_id_len(arg);
    // Only give an error when there is something, otherwise it will be
    // reported at a higher level.
    ਜੇ (len == 0 && verbose && **arg != NUL)
	semsg(_(e_invalid_ਵਿਅੰਜਕ_str), *arg);

    ਵਾਪਸ len;
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
ਲੱਭੋ_name_ਸਿਰਾ(
    char_u	*arg,
    char_u	**ਵਿਅੰਜਕ_ਸ਼ੁਰੂ,
    char_u	**ਵਿਅੰਜਕ_ਸਿਰਾ,
    ਪੂਰਨ		flags)
{
    ਪੂਰਨ		mb_nest = 0;
    ਪੂਰਨ		br_nest = 0;
    char_u	*p;
    ਪੂਰਨ		len;
    ਪੂਰਨ		allow_curly = !ਵਿੱਚ_vim9script();

    ਜੇ (ਵਿਅੰਜਕ_ਸ਼ੁਰੂ != ਨਿਰਾ)
    {
	*ਵਿਅੰਜਕ_ਸ਼ੁਰੂ = ਨਿਰਾ;
	*ਵਿਅੰਜਕ_ਸਿਰਾ = ਨਿਰਾ;
    }

    // Quick check for valid starting character.
    ਜੇ ((flags & FNE_CHECK_ਸ਼ੁਰੂ) && !eval_isnamec1(*arg)
					      && (*arg != '{' || !allow_curly))
	ਵਾਪਸ arg;

    ਲਈ (p = arg; *p != NUL
		    && (eval_isnamec(*p)
			|| (*p == '{' && allow_curly)
			|| ((flags & FNE_INCL_BR) && (*p == '['
					 || (*p == '.' && eval_isdictc(p[1]))))
			|| mb_nest != 0
			|| br_nest != 0); MB_PTR_ADV(p))
    {
	ਜੇ (*p == '\'')
	{
	    // skip over 'string' to avoid counting [ and ] inside it.
	    ਲਈ (p = p + 1; *p != NUL && *p != '\''; MB_PTR_ADV(p))
		;
	    ਜੇ (*p == NUL)
		ਰੋਕੋ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (*p == '"')
	{
	    // skip over "str\"ing" to avoid counting [ and ] inside it.
	    ਲਈ (p = p + 1; *p != NUL && *p != '"'; MB_PTR_ADV(p))
		ਜੇ (*p == '\\' && p[1] != NUL)
		    ++p;
	    ਜੇ (*p == NUL)
		ਰੋਕੋ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (br_nest == 0 && mb_nest == 0 && *p == ':')
	{
	    // "s:" is start of "s:var", but "n:" is not and can be used in
	    // slice "[n:]".  Also "xx:" is not a namespace. But {ns}: is.
	    len = (ਪੂਰਨ)(p - arg);
	    ਜੇ ((len == 1 && vim_strchr(NAMESPACE_CHAR, *arg) == ਨਿਰਾ)
		    || (len > 1 && p[-1] != '}'))
		ਰੋਕੋ;
	}

	ਜੇ (mb_nest == 0)
	{
	    ਜੇ (*p == '[')
		++br_nest;
	    ਨਹੀਂਤਾਂ ਜੇ (*p == ']')
		--br_nest;
	}

	ਜੇ (br_nest == 0 && allow_curly)
	{
	    ਜੇ (*p == '{')
	    {
		mb_nest++;
		ਜੇ (ਵਿਅੰਜਕ_ਸ਼ੁਰੂ != ਨਿਰਾ && *ਵਿਅੰਜਕ_ਸ਼ੁਰੂ == ਨਿਰਾ)
		    *ਵਿਅੰਜਕ_ਸ਼ੁਰੂ = p;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (*p == '}')
	    {
		mb_nest--;
		ਜੇ (ਵਿਅੰਜਕ_ਸ਼ੁਰੂ != ਨਿਰਾ && mb_nest == 0 && *ਵਿਅੰਜਕ_ਸਿਰਾ == ਨਿਰਾ)
		    *ਵਿਅੰਜਕ_ਸਿਰਾ = p;
	    }
	}
    }

    ਵਾਪਸ p;
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
    ਸਥਿਰ char_u *
make_expanded_name(
    char_u	*ਵਿੱਚ_ਸ਼ੁਰੂ,
    char_u	*ਵਿਅੰਜਕ_ਸ਼ੁਰੂ,
    char_u	*ਵਿਅੰਜਕ_ਸਿਰਾ,
    char_u	*ਵਿੱਚ_ਸਿਰਾ)
{
    char_u	c1;
    char_u	*retval = ਨਿਰਾ;
    char_u	*ਆਰਜ਼ੀ_ਨਤੀਜਾ;

    ਜੇ (ਵਿਅੰਜਕ_ਸਿਰਾ == ਨਿਰਾ || ਵਿੱਚ_ਸਿਰਾ == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    *ਵਿਅੰਜਕ_ਸ਼ੁਰੂ	= NUL;
    *ਵਿਅੰਜਕ_ਸਿਰਾ = NUL;
    c1 = *ਵਿੱਚ_ਸਿਰਾ;
    *ਵਿੱਚ_ਸਿਰਾ = NUL;

    ਆਰਜ਼ੀ_ਨਤੀਜਾ = eval_ਮੰਜ਼ਿਲ_string(ਵਿਅੰਜਕ_ਸ਼ੁਰੂ + 1, FALSE, FALSE);
    ਜੇ (ਆਰਜ਼ੀ_ਨਤੀਜਾ != ਨਿਰਾ)
    {
	size_t	retvalsize = (size_t)(ਵਿਅੰਜਕ_ਸ਼ੁਰੂ - ਵਿੱਚ_ਸ਼ੁਰੂ)
				+ STRLEN(ਆਰਜ਼ੀ_ਨਤੀਜਾ)
				+ (size_t)(ਵਿੱਚ_ਸਿਰਾ - ਵਿਅੰਜਕ_ਸਿਰਾ) + 1;

	retval = alloc(retvalsize);
	ਜੇ (retval != ਨਿਰਾ)
	    vim_snprintf((ਅੱਖਰ *)retval, retvalsize, "%s%s%s",
				ਵਿੱਚ_ਸ਼ੁਰੂ, ਆਰਜ਼ੀ_ਨਤੀਜਾ, ਵਿਅੰਜਕ_ਸਿਰਾ + 1);
    }
    vim_free(ਆਰਜ਼ੀ_ਨਤੀਜਾ);

    *ਵਿੱਚ_ਸਿਰਾ = c1;		// put char back for error messages
    *ਵਿਅੰਜਕ_ਸ਼ੁਰੂ = '{';
    *ਵਿਅੰਜਕ_ਸਿਰਾ = '}';

    ਜੇ (retval != ਨਿਰਾ)
    {
	ਆਰਜ਼ੀ_ਨਤੀਜਾ = ਲੱਭੋ_name_ਸਿਰਾ(retval, &ਵਿਅੰਜਕ_ਸ਼ੁਰੂ, &ਵਿਅੰਜਕ_ਸਿਰਾ, 0);
	ਜੇ (ਵਿਅੰਜਕ_ਸ਼ੁਰੂ != ਨਿਰਾ)
	{
	    // Further expansion!
	    ਆਰਜ਼ੀ_ਨਤੀਜਾ = make_expanded_name(retval, ਵਿਅੰਜਕ_ਸ਼ੁਰੂ,
						       ਵਿਅੰਜਕ_ਸਿਰਾ, ਆਰਜ਼ੀ_ਨਤੀਜਾ);
	    vim_free(retval);
	    retval = ਆਰਜ਼ੀ_ਨਤੀਜਾ;
	}
    }

    ਵਾਪਸ retval;
}

/*
 * Return TRUE if character "c" can be used in a variable or function name.
 * Does not include '{' or '}' for magic braces.
 */
    ਪੂਰਨ
eval_isnamec(ਪੂਰਨ c)
{
    ਵਾਪਸ ASCII_ISALNUM(c) || c == '_' || c == ':' || c == AUTOLOAD_CHAR;
}

/*
 * Return TRUE if character "c" can be used as the first character in a
 * variable or function name (excluding '{' and '}').
 */
    ਪੂਰਨ
eval_isnamec1(ਪੂਰਨ c)
{
    ਵਾਪਸ ASCII_ISALPHA(c) || c == '_';
}

/*
 * Return TRUE if character "c" can be used as the first character of a
 * dictionary key.
 */
    ਪੂਰਨ
eval_isdictc(ਪੂਰਨ c)
{
    ਵਾਪਸ ASCII_ISALNUM(c) || c == '_';
}

/*
 * Return true when "p" points to "->" followed by a method name or a lambda.
 * Only called for Vim9 script.
 */
    ਸਥਿਰ bool
ਵਿਧੀ_call_follows(char_u *p)
{
    ਜੇ (p[0] != '-' || p[1] != '>')
	ਵਾਪਸ false;

    char_u	*after = skipwhite(p + 2);

    ਵਾਪਸ ASCII_ISALPHA(*after) || *after == '(';
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
    ਪੂਰਨ
handle_subscript(
    char_u	**arg,
    char_u	*name_ਸ਼ੁਰੂ,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    ਪੂਰਨ		verbose)	// give error messages
{
    ਪੂਰਨ		evaluate = evalarg != ਨਿਰਾ
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    ਪੂਰਨ		ret = OK;
    dict_T	*selfdict = ਨਿਰਾ;
    ਪੂਰਨ		check_white = TRUE;
    ਪੂਰਨ		getnext;
    char_u	*p;

    ਜਦੋਂ (ret == OK)
    {
	// When at the end of the line and ".name" or a method call follows in
	// the next line then consume the line break.
	p = eval_ਅਗਲਾ_non_blank(*arg, evalarg, &getnext);
	ਜੇ (getnext
	    && ((*p == '.'
		    && ((rettv->v_type == VAR_DICT && eval_isdictc(p[1]))
			|| rettv->v_type == VAR_CLASS
			|| rettv->v_type == VAR_OBJECT))
		|| ਵਿਧੀ_call_follows(p)))
	{
	    *arg = eval_ਅਗਲਾ_ਸਤਰ(*arg, evalarg);
	    p = *arg;
	    check_white = FALSE;
	}

	ਜੇ (rettv->v_type == VAR_ANY)
	{
	    char_u	*exp_name;
	    ਪੂਰਨ		cc;
	    ਪੂਰਨ		idx;
	    ufunc_T	*ufunc;
	    type_T	*type;

	    // Found script from "import {name} as name", script item name must
	    // follow.  "rettv->vval.v_number" has the script ID.
	    ਜੇ (**arg != '.')
	    {
		ਜੇ (verbose)
		    semsg(_(e_expected_dot_after_name_str),
					name_ਸ਼ੁਰੂ != ਨਿਰਾ ? name_ਸ਼ੁਰੂ: *arg);
		ret = FAIL;
		ਰੋਕੋ;
	    }
	    ++*arg;
	    ਜੇ (IS_WHITE_OR_NUL(**arg))
	    {
		ਜੇ (verbose)
		    emsg(_(e_no_white_space_allowed_after_dot));
		ret = FAIL;
		ਰੋਕੋ;
	    }

	    // isolate the name
	    exp_name = *arg;
	    ਜਦੋਂ (eval_isnamec(**arg))
		++*arg;
	    cc = **arg;
	    **arg = NUL;

	    idx = ਲੱਭੋ_exported(rettv->vval.v_ਸੰਖਿਆ, exp_name, &ufunc, &type,
		       evalarg == ਨਿਰਾ ? ਨਿਰਾ : evalarg->eval_cctx,
		       evalarg == ਨਿਰਾ ? ਨਿਰਾ : evalarg->eval_cstack, verbose);
	    **arg = cc;

	    ਜੇ (idx < 0 && ufunc == ਨਿਰਾ)
	    {
		ret = FAIL;
		ਰੋਕੋ;
	    }
	    ਜੇ (idx >= 0)
	    {
		scriptitem_T    *si = SCRIPT_ਵਸਤੂ(rettv->vval.v_ਸੰਖਿਆ);
		svar_T		*sv = ((svar_T *)si->sn_var_vals.ga_ਅੰਕੜੇ) + idx;

		copy_tv(sv->sv_tv, rettv);
	    }
	    ਨਹੀਂਤਾਂ
	    {
		rettv->v_type = VAR_FUNC;
		ਜੇ (**arg == '<')
		{
		    char_u *s = get_generic_func_name(ufunc, arg);
		    ਜੇ (s != ਨਿਰਾ)
			rettv->vval.v_string = s;
		    ਨਹੀਂਤਾਂ
			ret = FAIL;
		}
		ਨਹੀਂਤਾਂ
		{
		    rettv->vval.v_string =
			vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
		}
	    }
	    ਜਾਰੀ;
	}

	ਜੇ ((**arg == '(' && (!evaluate || rettv->v_type == VAR_FUNC
			    || rettv->v_type == VAR_PARTIAL))
		    && (!check_white || !VIM_ISWHITE(*(*arg - 1))))
	{
	    ret = call_func_rettv(arg, evalarg, rettv, evaluate,
							       selfdict, ਨਿਰਾ);

	    // Stop the expression evaluation when immediately aborting on
	    // error, or when an interrupt occurred or an exception was thrown
	    // but not caught.
	    ਜੇ (aborting())
	    {
		ਜੇ (ret == OK)
		    clear_tv(rettv);
		ret = FAIL;
	    }
	    dict_unref(selfdict);
	    selfdict = ਨਿਰਾ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (p[0] == '-' && p[1] == '>')
	{
	    ਜੇ (ਵਿੱਚ_vim9script())
		*arg = skipwhite(p + 2);
	    ਨਹੀਂਤਾਂ
		*arg = p + 2;
	    ਜੇ (ret == OK && evaluate && rettv->v_type == VAR_VOID)
	    {
		ਜੇ (verbose)
		    emsg(_(e_cannot_use_void_ਮੁੱਲ));
		ret = FAIL;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (VIM_ISWHITE(**arg))
	    {
		emsg(_(e_no_white_space_allowed_before_parenthesis));
		ret = FAIL;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ ((**arg == '{' && !ਵਿੱਚ_vim9script()) || **arg == '(')
		// expr->{lambda}() or expr->(lambda)()
		ret = eval_lambda(arg, rettv, evalarg, verbose);
	    ਨਹੀਂਤਾਂ
		// expr->name()
		ret = eval_ਵਿਧੀ(arg, rettv, evalarg, verbose);
	}
	// "." is ".name" lookup when we found a dict or when evaluating and
	// scriptversion is at least 2, where string concatenation is "..".
	ਨਹੀਂਤਾਂ ਜੇ (**arg == '['
		|| (**arg == '.' && (rettv->v_type == VAR_DICT
			|| (!evaluate
			    && (*arg)[1] != '.'
			    && !ਵਿੱਚ_old_script(2)))))
	{
	    dict_unref(selfdict);
	    ਜੇ (rettv->v_type == VAR_DICT)
	    {
		selfdict = rettv->vval.v_dict;
		ਜੇ (selfdict != ਨਿਰਾ)
		    ++selfdict->dv_refcount;
	    }
	    ਨਹੀਂਤਾਂ
		selfdict = ਨਿਰਾ;
	    ਜੇ (eval_ਸੂਚਕਾਂਕ(arg, rettv, evalarg, verbose) == FAIL)
	    {
		clear_tv(rettv);
		ret = FAIL;
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (**arg == '.' && (rettv->v_type == VAR_CLASS
					       || rettv->v_type == VAR_OBJECT))
	{
	    // class member: SomeClass.varname
	    // class method: SomeClass.SomeMethod()
	    // class constructor: SomeClass.new()
	    // object member: someObject.varname
	    // object method: someObject.SomeMethod()
	    ਜੇ (class_object_ਸੂਚਕਾਂਕ(arg, rettv, evalarg, verbose) == FAIL)
	    {
		clear_tv(rettv);
		ret = FAIL;
	    }
	}
	ਨਹੀਂਤਾਂ
	    ਰੋਕੋ;
    }

    // Turn "dict.Func" into a partial for "Func" bound to "dict".
    // Don't do this when "Func" is already a partial that was bound
    // explicitly (pt_auto is FALSE).
    ਜੇ (selfdict != ਨਿਰਾ
	    && (rettv->v_type == VAR_FUNC
		|| (rettv->v_type == VAR_PARTIAL
		    && (rettv->vval.v_partial->pt_auto
			|| rettv->vval.v_partial->pt_dict == ਨਿਰਾ))))
	selfdict = make_partial(selfdict, rettv);

    dict_unref(selfdict);
    ਵਾਪਸ ret;
}

/*
 * Make a copy of an item.
 * Lists and Dictionaries are also copied.  A deep copy if "deep" is set.
 * "top" is TRUE for the toplevel of copy().
 * For deepcopy() "copyID" is zero for a full copy or the ID for when a
 * reference to an already copied list/dict can be used.
 * Returns FAIL or OK.
 */
    ਪੂਰਨ
ਵਸਤੂ_copy(
    typval_T	*from,
    typval_T	*ਮੰਜ਼ਿਲ,
    ਪੂਰਨ		deep,
    ਪੂਰਨ		ਸਿਖਰ,
    ਪੂਰਨ		copyID)
{
    ਸਥਿਰ ਪੂਰਨ	recurse = 0;
    ਪੂਰਨ		ret = OK;

    ਜੇ (recurse >= DICT_MAXNEST)
    {
	emsg(_(e_variable_nested_too_deep_for_making_copy));
	ਵਾਪਸ FAIL;
    }
    ++recurse;

    ਚੋਣ (from->v_type)
    {
	ਹਾਲਤ VAR_ਸੰਖਿਆ:
	ਹਾਲਤ VAR_FLOAT:
	ਹਾਲਤ VAR_STRING:
	ਹਾਲਤ VAR_FUNC:
	ਹਾਲਤ VAR_PARTIAL:
	ਹਾਲਤ VAR_BOOL:
	ਹਾਲਤ VAR_SPECIAL:
	ਹਾਲਤ VAR_ਕੰਮ:
	ਹਾਲਤ VAR_CHANNEL:
	ਹਾਲਤ VAR_INSTR:
	ਹਾਲਤ VAR_CLASS:
	ਹਾਲਤ VAR_OBJECT:
	ਹਾਲਤ VAR_TYPEALIAS:
	    copy_tv(from, ਮੰਜ਼ਿਲ);
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_LIST:
	    ਮੰਜ਼ਿਲ->v_type = VAR_LIST;
	    ਮੰਜ਼ਿਲ->v_ਤਾਲਾ = 0;
	    ਜੇ (from->vval.v_list == ਨਿਰਾ)
		ਮੰਜ਼ਿਲ->vval.v_list = ਨਿਰਾ;
	    ਨਹੀਂਤਾਂ ਜੇ (copyID != 0 && from->vval.v_list->lv_copyID == copyID)
	    {
		// use the copy made earlier
		ਮੰਜ਼ਿਲ->vval.v_list = from->vval.v_list->lv_copylist;
		++ਮੰਜ਼ਿਲ->vval.v_list->lv_refcount;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਮੰਜ਼ਿਲ->vval.v_list = list_copy(from->vval.v_list,
							    deep, ਸਿਖਰ, copyID);
		ਜੇ (ਮੰਜ਼ਿਲ->vval.v_list == ਨਿਰਾ)
		    ret = FAIL;
	    }
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_TUPLE:
	    ਮੰਜ਼ਿਲ->v_type = VAR_TUPLE;
	    ਮੰਜ਼ਿਲ->v_ਤਾਲਾ = 0;
	    ਜੇ (from->vval.v_tuple == ਨਿਰਾ)
		ਮੰਜ਼ਿਲ->vval.v_tuple = ਨਿਰਾ;
	    ਨਹੀਂਤਾਂ ਜੇ (copyID != 0 && from->vval.v_tuple->tv_copyID == copyID)
	    {
		// use the copy made earlier
		ਮੰਜ਼ਿਲ->vval.v_tuple = from->vval.v_tuple->tv_copytuple;
		++ਮੰਜ਼ਿਲ->vval.v_tuple->tv_refcount;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਮੰਜ਼ਿਲ->vval.v_tuple = tuple_copy(from->vval.v_tuple,
							    deep, ਸਿਖਰ, copyID);
		ਜੇ (ਮੰਜ਼ਿਲ->vval.v_tuple == ਨਿਰਾ)
		    ret = FAIL;
	    }
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_BLOB:
	    ਮੰਜ਼ਿਲ->v_type = VAR_BLOB;
	    ਮੰਜ਼ਿਲ->v_ਤਾਲਾ = 0;
	    ਜੇ (from->vval.v_blob == ਨਿਰਾ)
		ਮੰਜ਼ਿਲ->vval.v_blob = ਨਿਰਾ;
	    ਨਹੀਂਤਾਂ
	    {
		ਮੰਜ਼ਿਲ->vval.v_blob = blob_copy(from->vval.v_blob);
		ਜੇ (ਮੰਜ਼ਿਲ->vval.v_blob == ਨਿਰਾ)
		    ret = FAIL;
	    }
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_DICT:
	    ਮੰਜ਼ਿਲ->v_type = VAR_DICT;
	    ਮੰਜ਼ਿਲ->v_ਤਾਲਾ = 0;
	    ਜੇ (from->vval.v_dict == ਨਿਰਾ)
		ਮੰਜ਼ਿਲ->vval.v_dict = ਨਿਰਾ;
	    ਨਹੀਂਤਾਂ ਜੇ (copyID != 0 && from->vval.v_dict->dv_copyID == copyID)
	    {
		// use the copy made earlier
		ਮੰਜ਼ਿਲ->vval.v_dict = from->vval.v_dict->dv_copydict;
		++ਮੰਜ਼ਿਲ->vval.v_dict->dv_refcount;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਮੰਜ਼ਿਲ->vval.v_dict = dict_copy(from->vval.v_dict,
							    deep, ਸਿਖਰ, copyID);
		ਜੇ (ਮੰਜ਼ਿਲ->vval.v_dict == ਨਿਰਾ)
		    ret = FAIL;
	    }
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_UNKNOWN:
	ਹਾਲਤ VAR_ANY:
	ਹਾਲਤ VAR_VOID:
	    ਅੰਦਰੂਨੀ_error_no_abort("item_copy(UNKNOWN)");
	    ret = FAIL;
    }
    --recurse;
    ਵਾਪਸ ret;
}

    ਖਾਲੀ
echo_one(typval_T *rettv, ਪੂਰਨ with_space, ਪੂਰਨ *atstart, ਪੂਰਨ *needclr)
{
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    char_u	*p = echo_string(rettv, &tofree, numbuf, get_copyID());

    ਜੇ (*atstart)
    {
	*atstart = FALSE;
	// Call msg_start() after eval1(), evaluating the expression
	// may cause a message to appear.
	ਜੇ (with_space)
	{
	    // Mark the saved text as finishing the line, so that what
	    // follows is displayed on a new line when scrolling back
	    // at the more prompt.
	    ਸੁਨੇਹਾ_sb_eol();
	    ਸੁਨੇਹਾ_ਸ਼ੁਰੂ();
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (with_space)
	ਸੁਨੇਹਾ_puts_attr(" ", echo_attr);

    ਜੇ (p != ਨਿਰਾ)
	ਲਈ ( ; *p != NUL && !got_int; ++p)
	{
	    ਜੇ (*p == '\n' || *p == '\r' || *p == TAB)
	    {
		ਜੇ (*p != TAB && *needclr)
		{
		    // remove any text still there from the command
		    ਸੁਨੇਹਾ_clr_eos();
		    *needclr = FALSE;
		}
		ਸੁਨੇਹਾ_putchar_attr(*p, echo_attr);
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ (has_mbyte)
		{
		    ਪੂਰਨ i = (*mb_ptr2len)(p);

		    (ਖਾਲੀ)ਸੁਨੇਹਾ_outtrans_len_attr(p, i, echo_attr);
		    p += i - 1;
		}
		ਨਹੀਂਤਾਂ
		    (ਖਾਲੀ)ਸੁਨੇਹਾ_outtrans_len_attr(p, 1, echo_attr);
	    }
	}
    vim_free(tofree);
}

/*
 * ":echo expr1 ..."	print each argument separated with a space, add a
 *			newline at the end.
 * ":echon expr1 ..."	print each argument plain.
 */
    ਖਾਲੀ
ex_echo(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    typval_T	rettv;
    char_u	*arg_ਸ਼ੁਰੂ;
    ਪੂਰਨ		needclr = TRUE;
    ਪੂਰਨ		atstart = TRUE;
    ਪੂਰਨ		did_emsg_before = did_emsg;
    ਪੂਰਨ		called_emsg_before = called_emsg;
    evalarg_T	evalarg;

    fill_evalarg_from_eap(&evalarg, eap, eap->skip);

    ਜੇ (eap->skip)
	++emsg_skip;
    ਜਦੋਂ ((!ਸਿਰੇ_excmd2(eap->cmd, arg) || *arg == '"') && !got_int)
    {
	// If eval1() causes an error message the text from the command may
	// still need to be cleared. E.g., "echo 22,44".
	need_clr_eos = needclr;

	arg_ਸ਼ੁਰੂ = arg;
	ਜੇ (eval1(&arg, &rettv, &evalarg) == FAIL)
	{
	    /*
	     * Report the invalid expression unless the expression evaluation
	     * has been cancelled due to an aborting error, an interrupt, or an
	     * exception.
	     */
	    ਜੇ (!aborting() && did_emsg == did_emsg_before
					  && called_emsg == called_emsg_before)
		semsg(_(e_invalid_ਵਿਅੰਜਕ_str), arg_ਸ਼ੁਰੂ);
	    need_clr_eos = FALSE;
	    ਰੋਕੋ;
	}
	need_clr_eos = FALSE;

	ਜੇ (!eap->skip)
	{
	    ਜੇ (rettv.v_type == VAR_VOID)
	    {
		semsg(_(e_ਵਿਅੰਜਕ_does_not_ਨਤੀਜਾ_ਵਿੱਚ_ਮੁੱਲ_str), arg_ਸ਼ੁਰੂ);
		ਰੋਕੋ;
	    }
	    echo_one(&rettv, eap->cmdidx == CMD_echo, &atstart, &needclr);
	}

	clear_tv(&rettv);
	arg = skipwhite(arg);
    }
    ਸੈੱਟ_nextcmd(eap, arg);
    clear_evalarg(&evalarg, eap);

    ਜੇ (eap->skip)
	--emsg_skip;
    ਨਹੀਂਤਾਂ
    {
	// remove text that may still be there from the command
	ਜੇ (needclr)
	    ਸੁਨੇਹਾ_clr_eos();
	ਜੇ (eap->cmdidx == CMD_echo)
	    ਸੁਨੇਹਾ_ਸਿਰਾ();
    }
}

/*
 * ":echohl {name}".
 */
    ਖਾਲੀ
ex_echohl(exarg_T *eap)
{
    echo_attr = syn_name2attr(eap->arg);
}

/*
 * Returns the :echo attribute
 */
    ਪੂਰਨ
get_echo_attr(ਖਾਲੀ)
{
    ਵਾਪਸ echo_attr;
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
    ਖਾਲੀ
ex_execute(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    typval_T	rettv;
    ਪੂਰਨ		ret = OK;
    char_u	*p;
    garray_T	ga;
    ਪੂਰਨ		len;
    ਲੰਮਾ	ਸ਼ੁਰੂ_lnum = SOURCING_LNUM;

    ga_init2(&ga, 1, 80);

    ਜੇ (eap->skip)
	++emsg_skip;
    ਜਦੋਂ (!ਸਿਰੇ_excmd2(eap->cmd, arg) || *arg == '"')
    {
	ret = eval1_emsg(&arg, &rettv, eap);
	ਜੇ (ret == FAIL)
	    ਰੋਕੋ;

	ਜੇ (!eap->skip)
	{
	    char_u   buf[NUMBUFLEN];

	    ਜੇ (eap->cmdidx == CMD_execute)
	    {
		ਜੇ (rettv.v_type == VAR_CHANNEL || rettv.v_type == VAR_ਕੰਮ)
		{
		    semsg(_(e_using_invalid_ਮੁੱਲ_as_string_str),
						  vartype_name(rettv.v_type));
		    p = ਨਿਰਾ;
		}
		ਨਹੀਂਤਾਂ
		    p = tv_get_string_buf(&rettv, buf);
	    }
	    ਨਹੀਂਤਾਂ
		p = tv_stringify(&rettv, buf);
	    ਜੇ (p == ਨਿਰਾ)
	    {
		clear_tv(&rettv);
		ret = FAIL;
		ਰੋਕੋ;
	    }
	    len = (ਪੂਰਨ)STRLEN(p);
	    ਜੇ (ga_grow(&ga, len + 2) == FAIL)
	    {
		clear_tv(&rettv);
		ret = FAIL;
		ਰੋਕੋ;
	    }
	    ਜੇ (ga.ga_len)
		((char_u *)(ga.ga_ਅੰਕੜੇ))[ga.ga_len++] = ' ';
	    STRCPY((char_u *)(ga.ga_ਅੰਕੜੇ) + ga.ga_len, p);
	    ga.ga_len += len;
	}

	clear_tv(&rettv);
	arg = skipwhite(arg);
    }

    ਜੇ (ret != FAIL && ga.ga_ਅੰਕੜੇ != ਨਿਰਾ)
    {
	// use the first line of continuation lines for messages
	SOURCING_LNUM = ਸ਼ੁਰੂ_lnum;

	ਜੇ (eap->cmdidx == CMD_echomsg
		|| eap->cmdidx == CMD_echowindow
		|| eap->cmdidx == CMD_echoerr)
	{
	    // Mark the already saved text as finishing the line, so that what
	    // follows is displayed on a new line when scrolling back at the
	    // more prompt.
	    ਸੁਨੇਹਾ_sb_eol();
	}

	ਜੇ (eap->cmdidx == CMD_echomsg)
	{
	    ਸੁਨੇਹਾ_attr(ga.ga_ਅੰਕੜੇ, echo_attr);
	    ਬਾਹਰਲਾ_flush();
	}
	ਨਹੀਂਤਾਂ ਜੇ (eap->cmdidx == CMD_echowindow)
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAS_ਸੁਨੇਹਾ_ਖਿੜਕੀ
	    ਸ਼ੁਰੂ_echowindow(eap->addr_ਗਿਣਤੀ > 0 ? eap->line2 : 0);
#ਜੇ_ਅੰਤ
	    ਸੁਨੇਹਾ_attr(ga.ga_ਅੰਕੜੇ, echo_attr);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAS_ਸੁਨੇਹਾ_ਖਿੜਕੀ
	    ਸਿਰਾ_echowindow();
#ਜੇ_ਅੰਤ
	}
	ਨਹੀਂਤਾਂ ਜੇ (eap->cmdidx == CMD_echoconsole)
	{
	    ui_write(ga.ga_ਅੰਕੜੇ, (ਪੂਰਨ)STRLEN(ga.ga_ਅੰਕੜੇ), TRUE);
	    ui_write((char_u *)"\r\n", 2, TRUE);
	}
	ਨਹੀਂਤਾਂ ਜੇ (eap->cmdidx == CMD_echoerr)
	{
	    ਪੂਰਨ		save_did_emsg = did_emsg;

	    // We don't want to abort following commands, restore did_emsg.
	    emsg(ga.ga_ਅੰਕੜੇ);
	    ਜੇ (!force_abort)
		did_emsg = save_did_emsg;
	}
	ਨਹੀਂਤਾਂ ਜੇ (eap->cmdidx == CMD_execute)
	{
	    ਪੂਰਨ save_sticky_cmdmod_flags = sticky_cmdmod_flags;

	    // "legacy exe cmd" and "vim9cmd exe cmd" applies to "cmd".
	    sticky_cmdmod_flags = cmdmod.cmod_flags
						& (CMOD_LEGACY | CMOD_VIM9CMD);
	    do_cmdline((char_u *)ga.ga_ਅੰਕੜੇ,
		       eap->ea_getline, eap->cookie, DOCMD_NOWAIT|DOCMD_VERBOSE);
	    sticky_cmdmod_flags = save_sticky_cmdmod_flags;
	}
    }

    ga_clear(&ga);

    ਜੇ (eap->skip)
	--emsg_skip;
    ਸੈੱਟ_nextcmd(eap, arg);
}

/*
 * Skip over the name of an option: "&option", "&g:option" or "&l:option".
 * "arg" points to the "&" or '+' when called, to "option" when returning.
 * Returns NULL when no option name found.  Otherwise pointer to the char
 * after the option name.
 */
    char_u *
ਲੱਭੋ_option_ਸਿਰਾ(char_u **arg, ਪੂਰਨ *scope)
{
    char_u	*p = *arg;

    ++p;
    ਜੇ (*p == 'g' && p[1] == ':')
    {
	*scope = OPT_GLOBAL;
	p += 2;
    }
    ਨਹੀਂਤਾਂ ਜੇ (*p == 'l' && p[1] == ':')
    {
	*scope = OPT_LOCAL;
	p += 2;
    }
    ਨਹੀਂਤਾਂ
	*scope = 0;

    ਜੇ (!ASCII_ISALPHA(*p))
	ਵਾਪਸ ਨਿਰਾ;
    *arg = p;

    ਜੇ (p[0] == 't' && p[1] == '_' && p[2] != NUL && p[3] != NUL)
	p += 4;	    // termcap option
    ਨਹੀਂਤਾਂ
	ਜਦੋਂ (ASCII_ISALPHA(*p))
	    ++p;
    ਵਾਪਸ p;
}

/*
 * Display script name where an item was last set.
 * Should only be invoked when 'verbose' is non-zero.
 */
    ਖਾਲੀ
ਆਖਰੀ_ਸੈੱਟ_ਸੁਨੇਹਾ(sctx_T script_ctx)
{
    char_u *p;

    ਜੇ (script_ctx.sc_sid == 0)
	ਵਾਪਸ;

    p = home_replace_save(ਨਿਰਾ, get_scriptname(script_ctx.sc_sid));
    ਜੇ (p == ਨਿਰਾ)
	ਵਾਪਸ;

    verbose_enter();
    ਸੁਨੇਹਾ_puts(_("\n\tLast set from "));
    ਸੁਨੇਹਾ_puts((ਅੱਖਰ *)p);
    ਜੇ (script_ctx.sc_lnum > 0)
    {
	ਸੁਨੇਹਾ_puts(_(ਸਤਰ_ਸੁਨੇਹਾ));
	ਸੁਨੇਹਾ_outnum((ਲੰਮਾ)script_ctx.sc_lnum);
    }
    verbose_leave();
    vim_free(p);
}

#ਜੇ_ਅੰਤ // FEAT_EVAL

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
    typval_T	*ਵਿਅੰਜਕ,
    char_u	*flags,
    size_t	*ret_len)		// length of returned buffer
{
    regmatch_T	regmatch;
    garray_T	ga;
    char_u	*ret;
    char_u	*save_cpo;

    // Make 'cpoptions' empty, so that the 'l' flag doesn't work here
    save_cpo = p_cpo;
    p_cpo = ਖਾਲੀ_ਥਾਵਾਂ_option;

    ga_init2(&ga, 1, 200);

    regmatch.rm_ic = p_ic;
    regmatch.regprog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
    ਜੇ (regmatch.regprog != ਨਿਰਾ)
    {
	char_u	*ਪੂਛ = str;
	char_u	*ਸਿਰਾ = str + len;
	ਪੂਰਨ	do_all = (flags[0] == 'g');
	ਪੂਰਨ	sublen;
	ਪੂਰਨ	i;
	char_u	*zero_ਚੌੜਾਈ = ਨਿਰਾ;

	ਜਦੋਂ (vim_regexec_nl(&regmatch, str, (colnr_T)(ਪੂਛ - str)))
	{
	    // Skip empty match except for first match.
	    ਜੇ (regmatch.startp[0] == regmatch.endp[0])
	    {
		ਜੇ (zero_ਚੌੜਾਈ == regmatch.startp[0])
		{
		    // avoid getting stuck on a match with an empty string
		    i = mb_ptr2len(ਪੂਛ);
		    mch_memmove((char_u *)ga.ga_ਅੰਕੜੇ + ga.ga_len, ਪੂਛ,
								   (size_t)i);
		    ga.ga_len += i;
		    ਪੂਛ += i;
		    ਜਾਰੀ;
		}
		zero_ਚੌੜਾਈ = regmatch.startp[0];
	    }

	    /*
	     * Get some space for a temporary buffer to do the substitution
	     * into.  It will contain:
	     * - The text up to where the match is.
	     * - The substituted text.
	     * - The text after the match.
	     */
	    sublen = vim_regsub(&regmatch, sub, ਵਿਅੰਜਕ, ਪੂਛ, 0, REGSUB_MAGIC);
	    ਜੇ (sublen <= 0)
	    {
		ga_clear(&ga);
		ਰੋਕੋ;
	    }
	    ਜੇ (ga_grow(&ga, (ਪੂਰਨ)((ਸਿਰਾ - ਪੂਛ) + sublen -
			    (regmatch.endp[0] - regmatch.startp[0]))) == FAIL)
	    {
		ga_clear(&ga);
		ਰੋਕੋ;
	    }

	    // copy the text up to where the match is
	    i = (ਪੂਰਨ)(regmatch.startp[0] - ਪੂਛ);
	    mch_memmove((char_u *)ga.ga_ਅੰਕੜੇ + ga.ga_len, ਪੂਛ, (size_t)i);
	    // add the substituted text
	    (ਖਾਲੀ)vim_regsub(&regmatch, sub, ਵਿਅੰਜਕ,
				  (char_u *)ga.ga_ਅੰਕੜੇ + ga.ga_len + i, sublen,
				  REGSUB_COPY | REGSUB_MAGIC);
	    ga.ga_len += i + sublen - 1;
	    ਪੂਛ = regmatch.endp[0];
	    ਜੇ (*ਪੂਛ == NUL)
		ਰੋਕੋ;
	    ਜੇ (!do_all)
		ਰੋਕੋ;
	}

	ਜੇ (ga.ga_ਅੰਕੜੇ != ਨਿਰਾ)
	{
	    STRCPY((ਅੱਖਰ *)ga.ga_ਅੰਕੜੇ + ga.ga_len, ਪੂਛ);
	    ga.ga_len += (ਪੂਰਨ)(ਸਿਰਾ - ਪੂਛ);
	}

	vim_regfree(regmatch.regprog);
    }

    ਜੇ (ga.ga_ਅੰਕੜੇ != ਨਿਰਾ)
    {
	str = (char_u *)ga.ga_ਅੰਕੜੇ;
	len = (size_t)ga.ga_len;
    }
    ret = vim_strnsave(str, len);
    ga_clear(&ga);
    ਜੇ (p_cpo == ਖਾਲੀ_ਥਾਵਾਂ_option)
	p_cpo = save_cpo;
    ਨਹੀਂਤਾਂ
    {
	// Darn, evaluating {sub} expression or {expr} changed the value.
	// If it's still empty it was changed and restored, need to restore in
	// the complicated way.
	ਜੇ (*p_cpo == NUL)
	    ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u *)"cpo", 0L, save_cpo, 0);
	free_string_option(save_cpo);
    }

    ਜੇ (ret_len != ਨਿਰਾ)
	*ret_len = len;

    ਵਾਪਸ ret;
}
