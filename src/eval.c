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
#define USING_FLOAT_STUFF

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಯದಿ defined(FEAT_EVAL)

#ifdef VMS
# include <float.h>
#endif

#define NAMESPACE_CHAR	(char_u *)"abglstvw"

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ eval0_simple_funccal(char_u *arg, typval_T *rettv, exarg_T *eap, evalarg_T *evalarg);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ eval2(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ eval3(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ eval4(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ eval5(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ eval6(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ eval7(char_u **arg, typval_T *rettv, evalarg_T *evalarg, ಪೂರ್ಣಾಙ್ಕ want_string);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ eval8(char_u **arg, typval_T *rettv, evalarg_T *evalarg, ಪೂರ್ಣಾಙ್ಕ want_string);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ eval9(char_u **arg, typval_T *rettv, evalarg_T *evalarg, ಪೂರ್ಣಾಙ್ಕ want_string);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ eval9_leader(typval_T *rettv, ಪೂರ್ಣಾಙ್ಕ numeric_only, char_u *start_leader, char_u **end_leaderp);

ಸ್ಥಿರ char_u *make_expanded_name(char_u *in_start, char_u *ಅಭಿವ್ಯಕ್ತಿ_start, char_u *ಅಭಿವ್ಯಕ್ತಿ_end, char_u *in_end);

/*
 * Return "n1" divided by "n2", taking care of dividing by zero.
 * If "failed" is not NULL set it to TRUE when dividing by zero fails.
 */
	varnumber_T
ಅಂಕೆ_divide(varnumber_T n1, varnumber_T n2, ಪೂರ್ಣಾಙ್ಕ *failed)
{
    varnumber_T	result;

    ಯದಿ (n2 == 0)
    {
	ಯದಿ (in_vim9script())
	{
	    emsg(_(e_divide_by_zero));
	    ಯದಿ (failed != ರಿಕ್ತ)
		*failed = TRUE;
	}
	ಯದಿ (n1 == 0)
	    result = VARNUM_MIN; // similar to NaN
	ಅನ್ಯಥಾ ಯದಿ (n1 < 0)
	    result = -VARNUM_MAX;
	ಅನ್ಯಥಾ
	    result = VARNUM_MAX;
    }
    ಅನ್ಯಥಾ ಯದಿ (n1 == VARNUM_MIN && n2 == -1)
    {
	// specific case: trying to do VARNUM_MIN / -1 results in a positive
	// number that doesn't fit in varnumber_T and causes an FPE
	result = VARNUM_MAX;
    }
    ಅನ್ಯಥಾ
	result = n1 / n2;

    ಪ್ರತ್ಯಾಗಮ result;
}

/*
 * Return "n1" modulus "n2", taking care of dividing by zero.
 * If "failed" is not NULL set it to TRUE when dividing by zero fails.
 */
	varnumber_T
ಅಂಕೆ_modulus(varnumber_T n1, varnumber_T n2, ಪೂರ್ಣಾಙ್ಕ *failed)
{
    ಯದಿ (n2 == 0 && in_vim9script())
    {
	emsg(_(e_divide_by_zero));
	ಯದಿ (failed != ರಿಕ್ತ)
	    *failed = TRUE;
    }
    ಪ್ರತ್ಯಾಗಮ (n2 == 0) ? 0 : (n1 % n2);
}

/*
 * Initialize the global and v: variables.
 */
    ಶೂನ್ಯ
eval_init(ಶೂನ್ಯ)
{
    evalvars_init();
    func_init();
}

#ಯದಿ defined(EXITFREE)
    ಶೂನ್ಯ
eval_clear(ಶೂನ್ಯ)
{
    evalvars_clear();
    free_scriptnames();  // must come after evalvars_clear().
    free_locales();

    // autoloaded script names
    free_autoload_scriptnames();

    // unreferenced lists, tuples and dicts
    (ಶೂನ್ಯ)garbage_collect(FALSE);

    // functions not garbage collected
    free_all_functions();
}
#endif

    ಶೂನ್ಯ
fill_evalarg_from_eap(evalarg_T *evalarg, exarg_T *eap, ಪೂರ್ಣಾಙ್ಕ skip)
{
    init_evalarg(evalarg);
    evalarg->eval_flags = skip ? 0 : EVAL_EVALUATE;

    ಯದಿ (eap == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    evalarg->eval_cstack = eap->cstack;
    ಯದಿ (sourcing_a_script(eap) || eap->ea_getline == get_list_ಸಾಲು)
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
    ಪೂರ್ಣಾಙ್ಕ
eval_to_bool(
    char_u	*arg,
    ಪೂರ್ಣಾಙ್ಕ		*error,
    exarg_T	*eap,
    ಪೂರ್ಣಾಙ್ಕ		skip,	    // only parse, don't execute
    ಪೂರ್ಣಾಙ್ಕ		use_simple_function)
{
    typval_T	tv;
    varnumber_T	retval = FALSE;
    evalarg_T	evalarg;
    ಪೂರ್ಣಾಙ್ಕ		r;

    fill_evalarg_from_eap(&evalarg, eap, skip);

    ಯದಿ (skip)
	++emsg_skip;
    ಯದಿ (use_simple_function)
	r = eval0_simple_funccal(arg, &tv, eap, &evalarg);
    ಅನ್ಯಥಾ
	r = eval0(arg, &tv, eap, &evalarg);
    ಯದಿ (r == FAIL)
	*error = TRUE;
    ಅನ್ಯಥಾ
    {
	*error = FALSE;
	ಯದಿ (!skip)
	{
	    ಯದಿ (in_vim9script())
		retval = tv_get_bool_chk(&tv, error);
	    ಅನ್ಯಥಾ
		retval = (tv_get_ಸಂಖ್ಯೆ_chk(&tv, error) != 0);
	    clear_tv(&tv);
	}
    }
    ಯದಿ (skip)
	--emsg_skip;
    clear_evalarg(&evalarg, eap);

    ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)retval;
}

/*
 * Call eval1() and give an error message if not done at a lower level.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
eval1_emsg(char_u **arg, typval_T *rettv, exarg_T *eap)
{
    char_u	*start = *arg;
    ಪೂರ್ಣಾಙ್ಕ		ret;
    ಪೂರ್ಣಾಙ್ಕ		did_emsg_before = did_emsg;
    ಪೂರ್ಣಾಙ್ಕ		called_emsg_before = called_emsg;
    evalarg_T	evalarg;

    fill_evalarg_from_eap(&evalarg, eap, eap != ರಿಕ್ತ && eap->skip);

    ret = eval1(arg, rettv, &evalarg);
    ಯದಿ (ret == FAIL)
    {
	// Report the invalid expression unless the expression evaluation has
	// been cancelled due to an aborting error, an interrupt, or an
	// exception, or we already gave a more specific error.
	// Also check called_emsg for when using assert_fails().
	ಯದಿ (!aborting() && did_emsg == did_emsg_before
					  && called_emsg == called_emsg_before)
	    semsg(_(e_invalid_expression_str), start);
    }
    clear_evalarg(&evalarg, eap);
    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Return whether a typval is a valid expression to pass to eval_expr_typval()
 * or eval_expr_to_bool().  An empty string returns FALSE;
 */
    ಪೂರ್ಣಾಙ್ಕ
eval_ಅಭಿವ್ಯಕ್ತಿ_valid_arg(typval_T *tv)
{
    ಪ್ರತ್ಯಾಗಮ tv->v_type != VAR_UNKNOWN
	    && (tv->v_type != VAR_STRING
		  || (tv->vval.v_string != ರಿಕ್ತ && *tv->vval.v_string != NUL));
}

/*
 * When calling eval_expr_typval() many times we only need one funccall_T.
 * Returns NULL when no funccall_T is to be used.
 * When returning non-NULL remove_funccal() must be called later.
 */
    funccall_T *
eval_ಅಭಿವ್ಯಕ್ತಿ_get_funccal(typval_T *ಅಭಿವ್ಯಕ್ತಿ, typval_T *rettv)
{
    ಯದಿ (ಅಭಿವ್ಯಕ್ತಿ->v_type != VAR_PARTIAL)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    partial_T *partial = ಅಭಿವ್ಯಕ್ತಿ->vval.v_partial;
    ಯದಿ (partial == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಯದಿ (partial->pt_func == ರಿಕ್ತ
	    || partial->pt_func->uf_def_status == UF_NOT_COMPILED)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಪ್ರತ್ಯಾಗಮ create_funccal(partial->pt_func, rettv);
}

/*
 * Evaluate a partial.
 * Pass arguments "argv[argc]".
 * "fc_arg" is from eval_expr_get_funccal() or NULL;
 * Return the result in "rettv" and OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
eval_ಅಭಿವ್ಯಕ್ತಿ_partial(
    typval_T	*ಅಭಿವ್ಯಕ್ತಿ,
    typval_T	*argv,
    ಪೂರ್ಣಾಙ್ಕ		argc,
    funccall_T	*fc_arg,
    typval_T	*rettv)
{
    partial_T	*partial = ಅಭಿವ್ಯಕ್ತಿ->vval.v_partial;

    ಯದಿ (partial == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (partial->pt_func != ರಿಕ್ತ
			&& partial->pt_func->uf_def_status != UF_NOT_COMPILED)
    {
	funccall_T	*fc = fc_arg != ರಿಕ್ತ ? fc_arg
				: create_funccal(partial->pt_func, rettv);
	ಪೂರ್ಣಾಙ್ಕ		r;

	ಯದಿ (fc == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	// Shortcut to call a compiled function with minimal overhead.
	ಯದಿ (partial->pt_obj != ರಿಕ್ತ)
	    partial->pt_obj->obj_refcount++;
	r = call_def_function(partial->pt_func, argc, argv, DEF_USE_PT_ARGV,
					partial, partial->pt_obj, fc, rettv);
	ಯದಿ (fc_arg == ರಿಕ್ತ)
	    remove_funccal();
	ಯದಿ (r == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಅನ್ಯಥಾ
    {
	char_u		*s = partial_name(partial);
	funcexe_T	funcexe;

	ಯದಿ (s == ರಿಕ್ತ || *s == NUL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	CLEAR_FIELD(funcexe);
	funcexe.fe_evaluate = TRUE;
	funcexe.fe_partial = partial;
	ಯದಿ (call_func(s, -1, rettv, argc, argv, &funcexe) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Evaluate an expression which is a function.
 * Pass arguments "argv[argc]".
 * Return the result in "rettv" and OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
eval_ಅಭಿವ್ಯಕ್ತಿ_func(
    typval_T	*ಅಭಿವ್ಯಕ್ತಿ,
    typval_T	*argv,
    ಪೂರ್ಣಾಙ್ಕ		argc,
    typval_T	*rettv)
{
    funcexe_T	funcexe;
    char_u	buf[NUMBUFLEN];
    char_u	*s;

    ಯದಿ (ಅಭಿವ್ಯಕ್ತಿ->v_type == VAR_FUNC)
	s = ಅಭಿವ್ಯಕ್ತಿ->vval.v_string;
    ಅನ್ಯಥಾ
	s = tv_get_string_buf_chk_strict(ಅಭಿವ್ಯಕ್ತಿ, buf, in_vim9script());
    ಯದಿ (s == ರಿಕ್ತ || *s == NUL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    CLEAR_FIELD(funcexe);
    funcexe.fe_evaluate = TRUE;
    ಯದಿ (call_func(s, -1, rettv, argc, argv, &funcexe) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Evaluate an expression, which is a string.
 * Return the result in "rettv" and OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
eval_ಅಭಿವ್ಯಕ್ತಿ_string(
    typval_T	*ಅಭಿವ್ಯಕ್ತಿ,
    typval_T	*rettv)
{
    char_u	*s;
    char_u	buf[NUMBUFLEN];

    s = tv_get_string_buf_chk_strict(ಅಭಿವ್ಯಕ್ತಿ, buf, in_vim9script());
    ಯದಿ (s == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    s = skipwhite(s);
    ಯದಿ (eval1_emsg(&s, rettv, ರಿಕ್ತ) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (*skipwhite(s) != NUL)  // check for trailing chars after expr
    {
	clear_tv(rettv);
	semsg(_(e_invalid_expression_str), s);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Evaluate an expression, which can be a function, partial or string.
 * Pass arguments "argv[argc]".
 * If "want_func" is TRUE treat a string as a function name, not an expression.
 * "fc_arg" is from eval_expr_get_funccal() or NULL;
 * Return the result in "rettv" and OK or FAIL.
 */
    ಪೂರ್ಣಾಙ್ಕ
eval_ಅಭಿವ್ಯಕ್ತಿ_typval(
	typval_T    *ಅಭಿವ್ಯಕ್ತಿ,
	ಪೂರ್ಣಾಙ್ಕ	    want_func,
	typval_T    *argv,
	ಪೂರ್ಣಾಙ್ಕ	    argc,
	funccall_T  *fc_arg,
	typval_T    *rettv)
{
    ಯದಿ (ಅಭಿವ್ಯಕ್ತಿ->v_type == VAR_PARTIAL)
	ಪ್ರತ್ಯಾಗಮ eval_ಅಭಿವ್ಯಕ್ತಿ_partial(ಅಭಿವ್ಯಕ್ತಿ, argv, argc, fc_arg, rettv);
    ಯದಿ (ಅಭಿವ್ಯಕ್ತಿ->v_type == VAR_INSTR)
	ಪ್ರತ್ಯಾಗಮ exe_typval_instr(ಅಭಿವ್ಯಕ್ತಿ, rettv);
    ಯದಿ (ಅಭಿವ್ಯಕ್ತಿ->v_type == VAR_FUNC || want_func)
	ಪ್ರತ್ಯಾಗಮ eval_ಅಭಿವ್ಯಕ್ತಿ_func(ಅಭಿವ್ಯಕ್ತಿ, argv, argc, rettv);

    ಪ್ರತ್ಯಾಗಮ eval_ಅಭಿವ್ಯಕ್ತಿ_string(ಅಭಿವ್ಯಕ್ತಿ, rettv);
}

/*
 * Like eval_to_bool() but using a typval_T instead of a string.
 * Works for string, funcref and partial.
 */
    ಪೂರ್ಣಾಙ್ಕ
eval_ಅಭಿವ್ಯಕ್ತಿ_to_bool(typval_T *ಅಭಿವ್ಯಕ್ತಿ, ಪೂರ್ಣಾಙ್ಕ *error)
{
    typval_T	rettv;
    ಪೂರ್ಣಾಙ್ಕ		res;

    ಯದಿ (eval_ಅಭಿವ್ಯಕ್ತಿ_typval(ಅಭಿವ್ಯಕ್ತಿ, FALSE, ರಿಕ್ತ, 0, ರಿಕ್ತ, &rettv) == FAIL)
    {
	*error = TRUE;
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }
    res = (tv_get_bool_chk(&rettv, error) != 0);
    clear_tv(&rettv);
    ಪ್ರತ್ಯಾಗಮ res;
}

/*
 * Top level evaluation function, returning a string.  If "skip" is TRUE,
 * only parsing to "nextcmd" is done, without reporting errors.  Return
 * pointer to allocated memory, or NULL for failure or when "skip" is TRUE.
 */
    char_u *
eval_to_string_skip(
    char_u	*arg,
    exarg_T	*eap,
    ಪೂರ್ಣಾಙ್ಕ		skip)	    // only parse, don't execute
{
    typval_T	tv;
    char_u	*retval;
    evalarg_T	evalarg;

    fill_evalarg_from_eap(&evalarg, eap, skip);
    ಯದಿ (skip)
	++emsg_skip;
    ಯದಿ (eval0(arg, &tv, eap, &evalarg) == FAIL || skip)
	retval = ರಿಕ್ತ;
    ಅನ್ಯಥಾ
    {
	retval = vim_strsave(tv_get_string(&tv));
	clear_tv(&tv);
    }
    ಯದಿ (skip)
	--emsg_skip;
    clear_evalarg(&evalarg, eap);

    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Initialize "evalarg" for use.
 */
    ಶೂನ್ಯ
init_evalarg(evalarg_T *evalarg)
{
    CLEAR_POINTER(evalarg);
    ga_init2(&evalarg->eval_tofree_ga, ಪರಿಮಾಣ(char_u *), 20);
}

/*
 * If "evalarg->eval_tofree" is not NULL free it later.
 * Caller is expected to overwrite "evalarg->eval_tofree" next.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
free_eval_tofree_later(evalarg_T *evalarg)
{
    ಯದಿ (evalarg->eval_tofree == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (ga_grow(&evalarg->eval_tofree_ga, 1) == OK)
	((char_u **)evalarg->eval_tofree_ga.ga_data)
	    [evalarg->eval_tofree_ga.ga_len++]
	    = evalarg->eval_tofree;
    ಅನ್ಯಥಾ
	vim_free(evalarg->eval_tofree);
}

/*
 * After using "evalarg" filled from "eap": free the memory.
 */
    ಶೂನ್ಯ
clear_evalarg(evalarg_T *evalarg, exarg_T *eap)
{
    ಯದಿ (evalarg == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    garray_T *etga = &evalarg->eval_tofree_ga;

    ಯದಿ (evalarg->eval_tofree != ರಿಕ್ತ || evalarg->eval_using_cmdline)
    {
	ಯದಿ (eap != ರಿಕ್ತ)
	{
	    // We may need to keep the original command line, e.g. for
	    // ":let" it has the variable names.  But we may also need
	    // the new one, "nextcmd" points into it.  Keep both.
	    vim_free(eap->cmdline_tofree);
	    eap->cmdline_tofree = *eap->cmdlinep;

	    ಯದಿ (evalarg->eval_using_cmdline && etga->ga_len > 0)
	    {
		// "nextcmd" points into the last line in eval_tofree_ga,
		// need to keep it around.
		--etga->ga_len;
		*eap->cmdlinep = ((char_u **)etga->ga_data)[etga->ga_len];
		vim_free(evalarg->eval_tofree);
	    }
	    ಅನ್ಯಥಾ
		*eap->cmdlinep = evalarg->eval_tofree;
	}
	ಅನ್ಯಥಾ
	    vim_free(evalarg->eval_tofree);
	evalarg->eval_tofree = ರಿಕ್ತ;
    }

    ga_clear_strings(etga);
    VIM_CLEAR(evalarg->eval_tofree_lambda);
}

/*
 * Skip over an expression at "*pp".
 * Return FAIL for an error, OK otherwise.
 */
    ಪೂರ್ಣಾಙ್ಕ
skip_ಅಭಿವ್ಯಕ್ತಿ(char_u **pp, evalarg_T *evalarg)
{
    typval_T	rettv;

    *pp = skipwhite(*pp);
    ಪ್ರತ್ಯಾಗಮ eval1(pp, &rettv, evalarg);
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
    ಪೂರ್ಣಾಙ್ಕ
skip_ಅಭಿವ್ಯಕ್ತಿ_concatenate(
	char_u	    **arg,
	char_u	    **start,
	char_u	    **end,
	evalarg_T   *evalarg)
{
    typval_T	rettv;
    ಪೂರ್ಣಾಙ್ಕ		res;
    ಪೂರ್ಣಾಙ್ಕ		vim9script = in_vim9script();
    garray_T    *gap = evalarg == ರಿಕ್ತ ? ರಿಕ್ತ : &evalarg->eval_ga;
    garray_T    *freegap = evalarg == ರಿಕ್ತ ? ರಿಕ್ತ : &evalarg->eval_freega;
    ಪೂರ್ಣಾಙ್ಕ		save_flags = evalarg == ರಿಕ್ತ ? 0 : evalarg->eval_flags;
    ಪೂರ್ಣಾಙ್ಕ		evaluate = evalarg == ರಿಕ್ತ
			       ? FALSE : (evalarg->eval_flags & EVAL_EVALUATE);

    ಯದಿ (vim9script && evaluate
	       && (evalarg->eval_cookie != ರಿಕ್ತ || evalarg->eval_cctx != ರಿಕ್ತ))
    {
	ga_init2(gap, ಪರಿಮಾಣ(char_u *), 10);
	// leave room for "start"
	ಯದಿ (ga_grow(gap, 1) == OK)
	    ++gap->ga_len;
	ga_init2(freegap, ಪರಿಮಾಣ(char_u *), 10);
    }
    *start = *arg;

    // Don't evaluate the expression.
    ಯದಿ (evalarg != ರಿಕ್ತ)
	evalarg->eval_flags &= ~EVAL_EVALUATE;
    *arg = skipwhite(*arg);
    res = eval1(arg, &rettv, evalarg);
    *end = *arg;
    ಯದಿ (evalarg != ರಿಕ್ತ)
	evalarg->eval_flags = save_flags;

    ಯದಿ (vim9script && evaluate
	    && (evalarg->eval_cookie != ರಿಕ್ತ || evalarg->eval_cctx != ರಿಕ್ತ))
    {
	ಯದಿ (evalarg->eval_ga.ga_len == 1)
	{
	    // just the one line, no need to concatenate
	    ga_clear(gap);
	    gap->ga_itemsize = 0;
	}
	ಅನ್ಯಥಾ
	{
	    char_u	    *p;
	    size_t	    endoff = STRLEN(*arg);

	    // Line breaks encountered, concatenate all the lines.
	    *((char_u **)gap->ga_data) = *start;
	    p = ga_concat_strings(gap, " ");

	    // free the lines only when using getsourceline()
	    ಯದಿ (evalarg->eval_cookie != ರಿಕ್ತ)
	    {
		// Do not free the first line, the caller can still use it.
		*((char_u **)gap->ga_data) = ರಿಕ್ತ;
		// Do not free the last line, "arg" points into it, free it
		// later.  Also free "eval_tofree" later if needed.
		free_eval_tofree_later(evalarg);
		evalarg->eval_tofree =
				    ((char_u **)gap->ga_data)[gap->ga_len - 1];
		((char_u **)gap->ga_data)[gap->ga_len - 1] = ರಿಕ್ತ;
		ga_clear_strings(gap);
		ga_clear(freegap);
	    }
	    ಅನ್ಯಥಾ
	    {
		ga_clear(gap);

		// free lines that were explicitly marked for freeing
		ga_clear_strings(freegap);
	    }

	    gap->ga_itemsize = 0;
	    ಯದಿ (p == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    *start = p;
	    vim_free(evalarg->eval_tofree_lambda);
	    evalarg->eval_tofree_lambda = p;
	    // Compute "end" relative to the end.
	    *end = *start + STRLEN(*start) - endoff;
	}
    }

    ಪ್ರತ್ಯಾಗಮ res;
}

/*
 * Convert "tv" to a string.
 * When "join_list" is TRUE convert a List or a Tuple into a sequence of lines.
 * Returns an allocated string (NULL when out of memory).
 */
    char_u *
typval2string(typval_T *tv, ಪೂರ್ಣಾಙ್ಕ join_list)
{
    garray_T	ga;
    char_u	*retval = ರಿಕ್ತ;

    ಯದಿ (join_list && (tv->v_type == VAR_LIST || tv->v_type == VAR_TUPLE))
    {
	ಯದಿ (tv->v_type == VAR_LIST)
	{
	    ga_init2(&ga, ಪರಿಮಾಣ(ವರ್ಣ), 80);
	    ಯದಿ (tv->vval.v_list != ರಿಕ್ತ)
	    {
		list_join(&ga, tv->vval.v_list, (char_u *)"\n", TRUE, FALSE,
									0);
		ಯದಿ (tv->vval.v_list->lv_len > 0)
		    ga_append(&ga, NL);
	    }
	    ga_append(&ga, NUL);
	    retval = (char_u *)ga.ga_data;
	}
	ಅನ್ಯಥಾ
	{
	    // tuple
	    ga_init2(&ga, ಪರಿಮಾಣ(ವರ್ಣ), 80);
	    ಯದಿ (tv->vval.v_tuple != ರಿಕ್ತ)
	    {
		tuple_join(&ga, tv->vval.v_tuple, (char_u *)"\n", TRUE, FALSE,
									0);
		ಯದಿ (TUPLE_LEN(tv->vval.v_tuple) > 0)
		    ga_append(&ga, NL);
	    }
	    ga_append(&ga, NUL);
	    retval = (char_u *)ga.ga_data;
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (tv->v_type == VAR_LIST
	    || tv->v_type == VAR_TUPLE
	    || tv->v_type == VAR_DICT)
    {
	char_u	*tofree;
	char_u	numbuf[NUMBUFLEN];

	retval = tv2string(tv, &tofree, numbuf, 0);
	// Make a copy if we have a value but it's not in allocated memory.
	ಯದಿ (retval != ರಿಕ್ತ && tofree == ರಿಕ್ತ)
	    retval = vim_strsave(retval);
    }
    ಅನ್ಯಥಾ
	retval = vim_strsave(tv_get_string(tv));
    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Top level evaluation function, returning a string.  Does not handle line
 * breaks.
 * When "join_list" is TRUE convert a List and a Tuple into a sequence of
 * lines.
 * Return pointer to allocated memory, or NULL for failure.
 */
    char_u *
eval_to_string_eap(
    char_u	*arg,
    ಪೂರ್ಣಾಙ್ಕ		join_list,
    exarg_T	*eap,
    ಪೂರ್ಣಾಙ್ಕ		use_simple_function)
{
    typval_T	tv;
    char_u	*retval;
    evalarg_T	evalarg;
    ಪೂರ್ಣಾಙ್ಕ		r;

    fill_evalarg_from_eap(&evalarg, eap, eap != ರಿಕ್ತ && eap->skip);
    ಯದಿ (use_simple_function)
	r = eval0_simple_funccal(arg, &tv, ರಿಕ್ತ, &evalarg);
    ಅನ್ಯಥಾ
	r = eval0(arg, &tv, ರಿಕ್ತ, &evalarg);
    ಯದಿ (r == FAIL)
	retval = ರಿಕ್ತ;
    ಅನ್ಯಥಾ
    {
	retval = typval2string(&tv, join_list);
	clear_tv(&tv);
    }
    clear_evalarg(&evalarg, ರಿಕ್ತ);

    ಪ್ರತ್ಯಾಗಮ retval;
}

    char_u *
eval_to_string(
    char_u	*arg,
    ಪೂರ್ಣಾಙ್ಕ		join_list,
    ಪೂರ್ಣಾಙ್ಕ		use_simple_function)
{
    ಪ್ರತ್ಯಾಗಮ eval_to_string_eap(arg, join_list, ರಿಕ್ತ, use_simple_function);
}

/*
 * Call eval_to_string() without using current local variables and using
 * textlock.  When "use_sandbox" is TRUE use the sandbox.
 * Use legacy Vim script syntax.
 */
    char_u *
eval_to_string_safe(
    char_u	*arg,
    ಪೂರ್ಣಾಙ್ಕ		use_sandbox,
    ಪೂರ್ಣಾಙ್ಕ		keep_script_version,
    ಪೂರ್ಣಾಙ್ಕ		use_simple_function)
{
    char_u	*retval;
    funccal_entry_T funccal_entry;
    ಪೂರ್ಣಾಙ್ಕ		save_sc_version = current_sctx.sc_version;
    ಪೂರ್ಣಾಙ್ಕ		save_garbage = may_garbage_collect;

    ಯದಿ (!keep_script_version)
	current_sctx.sc_version = 1;
    save_funccal(&funccal_entry);
    ಯದಿ (use_sandbox)
	++sandbox;
    ++textlock;
    may_garbage_collect = FALSE;
    retval = eval_to_string(arg, FALSE, use_simple_function);
    ಯದಿ (use_sandbox)
	--sandbox;
    --textlock;
    may_garbage_collect = save_garbage;
    restore_funccal();
    current_sctx.sc_version = save_sc_version;
    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Top level evaluation function, returning a number.
 * Evaluates "expr" silently.
 * Returns -1 for an error.
 */
    varnumber_T
eval_to_ಸಂಖ್ಯೆ(char_u *ಅಭಿವ್ಯಕ್ತಿ, ಪೂರ್ಣಾಙ್ಕ use_simple_function)
{
    typval_T	rettv;
    varnumber_T	retval;
    char_u	*p = skipwhite(ಅಭಿವ್ಯಕ್ತಿ);
    ಪೂರ್ಣಾಙ್ಕ		r = NOTDONE;

    ++emsg_off;

    ಯದಿ (use_simple_function)
	r = may_call_simple_func(ಅಭಿವ್ಯಕ್ತಿ, &rettv);
    ಯದಿ (r == NOTDONE)
	r = eval1(&p, &rettv, &EVALARG_EVALUATE);
    ಯದಿ (r == FAIL)
	retval = -1;
    ಅನ್ಯಥಾ
    {
	retval = tv_get_ಸಂಖ್ಯೆ_chk(&rettv, ರಿಕ್ತ);
	clear_tv(&rettv);
    }
    --emsg_off;

    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Top level evaluation function.
 * Returns an allocated typval_T with the result.
 * Returns NULL when there is an error.
 */
    typval_T *
eval_ಅಭಿವ್ಯಕ್ತಿ(char_u *arg, exarg_T *eap)
{
    ಪ್ರತ್ಯಾಗಮ eval_ಅಭಿವ್ಯಕ್ತಿ_ext(arg, eap, FALSE);
}

    typval_T *
eval_ಅಭಿವ್ಯಕ್ತಿ_ext(char_u *arg, exarg_T *eap, ಪೂರ್ಣಾಙ್ಕ use_simple_function)
{
    typval_T	*tv;
    evalarg_T	evalarg;

    fill_evalarg_from_eap(&evalarg, eap, eap != ರಿಕ್ತ && eap->skip);

    tv = ALLOC_ONE(typval_T);
    ಯದಿ (tv != ರಿಕ್ತ)
    {
	ಪೂರ್ಣಾಙ್ಕ	r = NOTDONE;

	ಯದಿ (use_simple_function)
	    r = eval0_simple_funccal(arg, tv, eap, &evalarg);
	ಯದಿ (r == NOTDONE)
	    r = eval0(arg, tv, eap, &evalarg);

	ಯದಿ (r == FAIL)
	    VIM_CLEAR(tv);
    }

    clear_evalarg(&evalarg, eap);
    ಪ್ರತ್ಯಾಗಮ tv;
}

/*
 * "*arg" points to what can be a function name in the form of "import.Name" or
 * "Funcref".  Return the name of the function.  Set "tofree" to something that
 * was allocated.
 * If "verbose" is FALSE no errors are given.
 * Return NULL for any failure.
 */
    ಸ್ಥಿರ char_u *
deref_function_name(
	    char_u	**arg,
	    char_u	**tofree,
	    evalarg_T	*evalarg,
	    ಪೂರ್ಣಾಙ್ಕ		verbose)
{
    typval_T	ref;
    char_u	*name = *arg;
    ಪೂರ್ಣಾಙ್ಕ		save_flags = 0;
    ಪೂರ್ಣಾಙ್ಕ		evaluate = evalarg != ರಿಕ್ತ
				      && (evalarg->eval_flags & EVAL_EVALUATE);

    ref.v_type = VAR_UNKNOWN;
    ಯದಿ (evalarg != ರಿಕ್ತ)
    {
	// need to evaluate this to get an import, like in "a.Func"
	save_flags = evalarg->eval_flags;
	evalarg->eval_flags |= EVAL_EVALUATE;
    }
    ಯದಿ (eval9(arg, &ref, evalarg, FALSE) == FAIL)
    {
	dictitem_T	*v;

	// If <SID>VarName was used it would not be found, try another way.
	v = find_var_also_in_script(name, ರಿಕ್ತ, FALSE);
	ಯದಿ (v == ರಿಕ್ತ)
	{
	    name = ರಿಕ್ತ;
	    ಗಚ್ಛ theend;
	}
	copy_tv(&v->di_tv, &ref);
    }
    ಯದಿ (*skipwhite(*arg) != NUL)
    {
	ಯದಿ (verbose)
	    semsg(_(e_trailing_characters_str), *arg);
	name = ರಿಕ್ತ;
    }
    ಅನ್ಯಥಾ ಯದಿ (ref.v_type == VAR_FUNC && ref.vval.v_string != ರಿಕ್ತ)
    {
	name = ref.vval.v_string;
	ref.vval.v_string = ರಿಕ್ತ;
	*tofree = name;
    }
    ಅನ್ಯಥಾ ಯದಿ (ref.v_type == VAR_PARTIAL && ref.vval.v_partial != ರಿಕ್ತ)
    {
	ಯದಿ (ref.vval.v_partial->pt_argc > 0
		|| ref.vval.v_partial->pt_dict != ರಿಕ್ತ)
	{
	    ಯದಿ (verbose)
		emsg(_(e_cannot_use_partial_here));
	    name = ರಿಕ್ತ;
	}
	ಅನ್ಯಥಾ
	{
	    name = vim_strsave(partial_name(ref.vval.v_partial));
	    *tofree = name;
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (evaluate)
    {
	ಯದಿ (verbose)
	    semsg(_(e_not_callable_type_str), name);
	name = ರಿಕ್ತ;
    }

theend:
    clear_tv(&ref);
    ಯದಿ (evalarg != ರಿಕ್ತ)
	evalarg->eval_flags = save_flags;
    ಪ್ರತ್ಯಾಗಮ name;
}

/*
 * Call some Vim script function and return the result in "*rettv".
 * Uses argv[0] to argv[argc - 1] for the function arguments.  argv[argc]
 * should have type VAR_UNKNOWN.
 * Returns OK or FAIL.
 */
    ಪೂರ್ಣಾಙ್ಕ
call_vim_function(
    char_u      *func,
    ಪೂರ್ಣಾಙ್ಕ		argc,
    typval_T	*argv,
    typval_T	*rettv)
{
    ಪೂರ್ಣಾಙ್ಕ		ret;
    funcexe_T	funcexe;
    char_u	*arg;
    char_u	*name;
    char_u	*tofree = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		ignore_errors;

    rettv->v_type = VAR_UNKNOWN;		// clear_tv() uses this
    CLEAR_FIELD(funcexe);
    funcexe.fe_firstline = curwin->w_cursor.lnum;
    funcexe.fe_lastline = curwin->w_cursor.lnum;
    funcexe.fe_evaluate = TRUE;

    // The name might be "import.Func" or "Funcref".  We don't know, we need to
    // ignore errors for an undefined name.  But we do want errors when an
    // autoload script has errors.  Guess that when there is a dot in the name
    // showing errors is the right choice.
    ignore_errors = vim_strchr(func, '.') == ರಿಕ್ತ;
    arg = func;
    ಯದಿ (ignore_errors)
	++emsg_off;
    name = deref_function_name(&arg, &tofree, &EVALARG_EVALUATE, FALSE);
    ಯದಿ (ignore_errors)
	--emsg_off;
    ಯದಿ (name == ರಿಕ್ತ)
	name = func;

    ret = call_func(name, -1, rettv, argc, argv, &funcexe);

    ಯದಿ (ret == FAIL)
	clear_tv(rettv);
    vim_free(tofree);

    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Call Vim script function "func" and return the result as a string.
 * Uses "argv[0]" to "argv[argc - 1]" for the function arguments. "argv[argc]"
 * should have type VAR_UNKNOWN.
 * Returns NULL when calling the function fails.
 */
    ಶೂನ್ಯ *
call_func_retstr(
    char_u      *func,
    ಪೂರ್ಣಾಙ್ಕ		argc,
    typval_T	*argv)
{
    typval_T	rettv;
    char_u	*retval;

    ಯದಿ (call_vim_function(func, argc, argv, &rettv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    retval = vim_strsave(tv_get_string(&rettv));
    clear_tv(&rettv);
    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Call Vim script function "func" and return the result as a List.
 * Uses "argv" and "argc" as call_func_retstr().
 * Returns NULL when there is something wrong.
 * Gives an error when the returned value is not a list.
 */
    ಶೂನ್ಯ *
call_func_retlist(
    char_u      *func,
    ಪೂರ್ಣಾಙ್ಕ		argc,
    typval_T	*argv)
{
    typval_T	rettv;

    ಯದಿ (call_vim_function(func, argc, argv, &rettv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (rettv.v_type != VAR_LIST)
    {
	semsg(_(e_custom_list_completion_function_does_not_return_list_but_str),
		vartype_name(rettv.v_type));
	clear_tv(&rettv);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಪ್ರತ್ಯಾಗಮ rettv.vval.v_list;
}

#ಯದಿ defined(FEAT_FOLDING)
/*
 * Evaluate "arg", which is 'foldexpr'.
 * Note: caller must set "curwin" to match "arg".
 * Returns the foldlevel, and any character preceding it in "*cp".  Doesn't
 * give error messages.
 */
    ಪೂರ್ಣಾಙ್ಕ
eval_foldexpr(win_T *wp, ಪೂರ್ಣಾಙ್ಕ *cp)
{
    char_u	*arg;
    typval_T	tv;
    varnumber_T	retval;
    char_u	*s;
    sctx_T	saved_sctx = current_sctx;
    ಪೂರ್ಣಾಙ್ಕ		use_sandbox = was_set_insecurely(wp, (char_u *)"foldexpr",
								    OPT_LOCAL);

    arg = skipwhite(wp->w_p_fde);
    current_sctx = wp->w_p_script_ctx[WV_FDE];

    ++emsg_off;
    ಯದಿ (use_sandbox)
	++sandbox;
    ++textlock;
    *cp = NUL;

    // Evaluate the expression.  If the expression is "FuncName()" call the
    // function directly.
    ಯದಿ (eval0_simple_funccal(arg, &tv, ರಿಕ್ತ, &EVALARG_EVALUATE) == FAIL)
	retval = 0;
    ಅನ್ಯಥಾ
    {
	// If the result is a number, just return the number.
	ಯದಿ (tv.v_type == VAR_ಸಂಖ್ಯೆ)
	    retval = tv.vval.v_ಸಂಖ್ಯೆ;
	ಅನ್ಯಥಾ ಯದಿ (tv.v_type != VAR_STRING || tv.vval.v_string == ರಿಕ್ತ)
	    retval = 0;
	ಅನ್ಯಥಾ
	{
	    // If the result is a string, check if there is a non-digit before
	    // the number.
	    s = tv.vval.v_string;
	    ಯದಿ (*s != NUL && !VIM_ISDIGIT(*s) && *s != '-')
		*cp = *s++;
	    retval = atol((ವರ್ಣ *)s);
	}
	clear_tv(&tv);
    }
    --emsg_off;
    ಯದಿ (use_sandbox)
	--sandbox;
    --textlock;
    clear_evalarg(&EVALARG_EVALUATE, ರಿಕ್ತ);
    current_sctx = saved_sctx;

    ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)retval;
}
#endif

#ifdef LOG_LOCKVAR
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ flag_string_S
{
    ಪೂರ್ಣಾಙ್ಕ	    flag;
    ವರ್ಣ    *str;
} flag_string_T;

    ಸ್ಥಿರ ವರ್ಣ *
flags_tostring(ಪೂರ್ಣಾಙ್ಕ flags, flag_string_T *_fstring, ವರ್ಣ *buf, size_t n)
{
    ವರ್ಣ *p = buf;
    *p = NUL;
    ಪ್ರತಿ (flag_string_T *fstring = _fstring; fstring->flag; ++fstring)
    {
	ಯದಿ ((fstring->flag & flags) != 0)
	{
	    size_t len = STRLEN(fstring->str);
	    ಯದಿ (n > p - buf + len + 7)
	    {
		STRCAT(p, fstring->str);
		p += len;
		STRCAT(p, " ");
		++p;
	    }
	    ಅನ್ಯಥಾ
	    {
		STRCAT(buf, "...");
		ಭಙ್ಗ;
	    }
	}
    }
    ಪ್ರತ್ಯಾಗಮ buf;
}

flag_string_T glv_flag_strings[] = {
    { GLV_QUIET,		"QUIET" },
    { GLV_NO_AUTOLOAD,		"NO_AUTOLOAD" },
    { GLV_READ_ONLY,		"READ_ONLY" },
    { GLV_NO_DECL,		"NO_DECL" },
    { GLV_COMPILING,		"COMPILING" },
    { GLV_ASSIGN_WITH_OP,	"ASSIGN_WITH_OP" },
    { GLV_PREFER_FUNC,		"PREFER_FUNC" },
    { 0,			ರಿಕ್ತ }
};
#endif

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
    ಸ್ಥಿರ ಶೂನ್ಯ
fill_lval_from_lval_ಬೇರು(lval_T *lp, lval_ಬೇರು_T *lr)
{
#ifdef LOG_LOCKVAR
    ch_log(ರಿಕ್ತ, "LKVAR: fill_lval_from_lval_root(): name %s, tv %p",
						lp->ll_name, (ಶೂನ್ಯ*)lr->lr_tv);
#endif
    ಯದಿ (lr->lr_tv == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (!lr->lr_is_arg && lr->lr_tv->v_type == VAR_CLASS)
    {
	ಯದಿ (lr->lr_tv->vval.v_class != ರಿಕ್ತ)
	{
	    // Special special case. Look for a bare class variable reference.
	    class_T	*cl = lr->lr_tv->vval.v_class;
	    ಪೂರ್ಣಾಙ್ಕ		m_idx;
	    ocmember_T	*m = class_member_lookup(cl, lp->ll_name,
					lp->ll_name_end - lp->ll_name, &m_idx);
	    ಯದಿ (m != ರಿಕ್ತ)
	    {
		// Assuming "inside class" since bare reference.
		lp->ll_class = lr->lr_tv->vval.v_class;
		lp->ll_oi = m_idx;
		lp->ll_valtype = m->ocm_type;
		lp->ll_tv = &lp->ll_class->class_members_tv[m_idx];
#ifdef LOG_LOCKVAR
		ch_log(ರಿಕ್ತ, "LKVAR:    ... class member %s.%s",
					lp->ll_class->class_name.string, lp->ll_name);
#endif
		ಪ್ರತ್ಯಾಗಮ;
	    }
	}
    }

#ifdef LOG_LOCKVAR
    ch_log(ರಿಕ್ತ, "LKVAR:    ... type: %s", vartype_name(lr->lr_tv->v_type));
#endif
    lp->ll_tv = lr->lr_tv;
    lp->ll_is_ಬೇರು = TRUE;
}

/*
 * Check if the class has permission to access the member.
 * Returns OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_lval_check_access(
    class_T	*cl_exec,   // executing class, NULL if :def or script level
    class_T	*cl,	    // class which contains the member
    ocmember_T	*om,	    // member being accessed
    char_u	*p,	    // char after member name
    ಪೂರ್ಣಾಙ್ಕ		flags)	    // GLV flags to check if writing to lval
{
#ifdef LOG_LOCKVAR
    ch_log(ರಿಕ್ತ, "LKVAR: get_lval_check_access(), cl_exec %p, cl %p, %c",
						(ಶೂನ್ಯ*)cl_exec, (ಶೂನ್ಯ*)cl, *p);
#endif
    ಯದಿ (cl_exec != ರಿಕ್ತ && cl_exec == cl)
	ಪ್ರತ್ಯಾಗಮ OK;

    ವರ್ಣ *ಸಂದೇಶ = ರಿಕ್ತ;
    ವಿಕಲ್ಪ (om->ocm_access)
    {
	ಪಕ್ಷ VIM_ACCESS_PRIVATE:
	    ಸಂದೇಶ = e_cannot_access_protected_variable_str;
	    ಭಙ್ಗ;
	ಪಕ್ಷ VIM_ACCESS_READ:
	    // If [idx] or .key following, read only OK.
	    ಯದಿ (*p == '[' || *p == '.')
		ಭಙ್ಗ;
	    ಯದಿ ((flags & GLV_READ_ONLY) == 0)
	    {
		ಯದಿ (IS_ENUM(cl))
		{
		    ಯದಿ (om->ocm_type->tt_type == VAR_OBJECT)
			semsg(_(e_enumvalue_str_cannot_be_modified),
				cl->class_name.string, om->ocm_name.string);
		    ಅನ್ಯಥಾ
			ಸಂದೇಶ = e_variable_is_not_writable_str;
		}
		ಅನ್ಯಥಾ
		    ಸಂದೇಶ = e_variable_is_not_writable_str;
	    }
	    ಭಙ್ಗ;
	ಪಕ್ಷ VIM_ACCESS_ALL:
	    ಭಙ್ಗ;
    }
    ಯದಿ (ಸಂದೇಶ != ರಿಕ್ತ)
    {
	emsg_var_cl_define(ಸಂದೇಶ, om->ocm_name.string, 0, cl);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
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
    ಸ್ಥಿರ char_u *
get_lval_imported(
    lval_T	*lp,
    scid_T	imp_sid,
    char_u	*p,
    dictitem_T	**dip,
    ಪೂರ್ಣಾಙ್ಕ		fne_flags)
{
    ufunc_T	*ufunc;
    type_T	*type = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		cc;
    ಪೂರ್ಣಾಙ್ಕ		rc = FAIL;

    p = skipwhite(p);

    import_check_sourced_sid(&imp_sid);
    lp->ll_sid = imp_sid;
    lp->ll_name = p;
    p = find_name_end(lp->ll_name, ರಿಕ್ತ, ರಿಕ್ತ, fne_flags);
    lp->ll_name_end = p;

    // check the item is exported
    cc = *p;
    *p = NUL;
    ಯದಿ (find_exported(imp_sid, lp->ll_name, &ufunc, &type, ರಿಕ್ತ, ರಿಕ್ತ,
								TRUE) == -1)
	ಗಚ್ಛ failed;

    // Get the typval for the exported item
    hashtab_T *ht = &SCRIPT_VARS(imp_sid);
    ಯದಿ (ht == ರಿಕ್ತ)
	ಗಚ್ಛ failed;

    dictitem_T *di = find_var_in_ht(ht, 0, lp->ll_name, TRUE);
    ಯದಿ (di == ರಿಕ್ತ)
	// script is autoloaded.  So variable will be found later
	ಗಚ್ಛ success;

    *dip = di;

    // Check whether the variable is writable.
    svar_T *sv = find_typval_in_script(&di->di_tv, imp_sid, FALSE);
    ಯದಿ (sv != ರಿಕ್ತ && sv->sv_const != 0)
    {
	semsg(_(e_cannot_change_readonly_variable_str), lp->ll_name);
	ಗಚ್ಛ failed;
    }

    // check whether variable is locked
    ಯದಿ (ಮೌಲ್ಯ_check_lock(di->di_tv.v_lock, lp->ll_name, FALSE))
	ಗಚ್ಛ failed;

    lp->ll_tv = &di->di_tv;
    lp->ll_valtype = type;

success:
    rc = OK;

failed:
    *p = cc;
    ಪ್ರತ್ಯಾಗಮ rc == OK ? p : ರಿಕ್ತ;
}

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
    GLV_FAIL,
    GLV_OK,
    GLV_STOP
} glv_status_T;

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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_lval_dict_item(
    lval_T	*lp,
    char_u	*name,
    char_u	*ಕೀಲಿ,
    ಪೂರ್ಣಾಙ್ಕ		len,
    char_u	**ಕೀಲಿ_end,
    typval_T	*var1,
    ಪೂರ್ಣಾಙ್ಕ		flags,
    ಪೂರ್ಣಾಙ್ಕ		unlet,
    typval_T	*rettv)
{
    ಪೂರ್ಣಾಙ್ಕ		quiet = flags & GLV_QUIET;
    char_u	*p = *ಕೀಲಿ_end;

    ಯದಿ (len == -1)
    {
	// "[key]": get key from "var1"
	ಕೀಲಿ = tv_get_string_chk(var1);	// is number or string
	ಯದಿ (ಕೀಲಿ == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ GLV_FAIL;
    }
    lp->ll_list = ರಿಕ್ತ;
    lp->ll_list = ರಿಕ್ತ;
    lp->ll_blob = ರಿಕ್ತ;
    lp->ll_object = ರಿಕ್ತ;
    lp->ll_class = ರಿಕ್ತ;
    lp->ll_tuple = ರಿಕ್ತ;

    // a NULL dict is equivalent with an empty dict
    ಯದಿ (lp->ll_tv->vval.v_dict == ರಿಕ್ತ)
    {
	lp->ll_tv->vval.v_dict = dict_alloc();
	ಯದಿ (lp->ll_tv->vval.v_dict == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ GLV_FAIL;
	++lp->ll_tv->vval.v_dict->dv_refcount;
    }
    lp->ll_dict = lp->ll_tv->vval.v_dict;

    lp->ll_di = dict_find(lp->ll_dict, ಕೀಲಿ, len);

    // When assigning to a scope dictionary check that a function and
    // variable name is valid (only variable name unless it is l: or
    // g: dictionary). Disallow overwriting a builtin function.
    ಯದಿ (rettv != ರಿಕ್ತ && lp->ll_dict->dv_scope != 0)
    {
	ಪೂರ್ಣಾಙ್ಕ prevval;

	ಯದಿ (len != -1)
	{
	    prevval = ಕೀಲಿ[len];
	    ಕೀಲಿ[len] = NUL;
	}
	ಅನ್ಯಥಾ
	    prevval = 0; // avoid compiler warning
	ಪೂರ್ಣಾಙ್ಕ wrong = (lp->ll_dict->dv_scope == VAR_DEF_SCOPE
		&& (rettv->v_type == VAR_FUNC
		    || rettv->v_type == VAR_PARTIAL)
		&& var_wrong_func_name(ಕೀಲಿ, lp->ll_di == ರಿಕ್ತ))
	    || !valid_varname(ಕೀಲಿ, -1, TRUE);
	ಯದಿ (len != -1)
	    ಕೀಲಿ[len] = prevval;
	ಯದಿ (wrong)
	    ಪ್ರತ್ಯಾಗಮ GLV_FAIL;
    }

    ಯದಿ (lp->ll_valtype != ರಿಕ್ತ)
	// use the type of the member
	lp->ll_valtype = lp->ll_valtype->tt_member;

    ಯದಿ (lp->ll_di == ರಿಕ್ತ)
    {
	// Can't add "v:" or "a:" variable.
	ಯದಿ (lp->ll_dict == get_vimvar_dict()
		|| &lp->ll_dict->dv_hashtab == get_funccal_args_ht())
	{
	    semsg(_(e_illegal_variable_name_str), name);
	    ಪ್ರತ್ಯಾಗಮ GLV_FAIL;
	}

	// Key does not exist in dict: may need to add it.
	ಯದಿ (*p == '[' || *p == '.' || unlet)
	{
	    ಯದಿ (!quiet)
		semsg(_(e_ಕೀಲಿ_not_present_in_dictionary_str), ಕೀಲಿ);
	    ಪ್ರತ್ಯಾಗಮ GLV_FAIL;
	}
	ಯದಿ (len == -1)
	    lp->ll_newkey = vim_strsave(ಕೀಲಿ);
	ಅನ್ಯಥಾ
	    lp->ll_newkey = vim_strnsave(ಕೀಲಿ, len);
	ಯದಿ (lp->ll_newkey == ರಿಕ್ತ)
	    p = ರಿಕ್ತ;

	*ಕೀಲಿ_end = p;
	ಪ್ರತ್ಯಾಗಮ GLV_STOP;
    }
    // existing variable, need to check if it can be changed
    ಅನ್ಯಥಾ ಯದಿ ((flags & GLV_READ_ONLY) == 0
	    && (var_check_ro(lp->ll_di->di_flags, name, FALSE)
		|| var_check_lock(lp->ll_di->di_flags, name, FALSE)))
	ಪ್ರತ್ಯಾಗಮ GLV_FAIL;

    lp->ll_tv = &lp->ll_di->di_tv;

    ಪ್ರತ್ಯಾಗಮ GLV_OK;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_lval_blob(
    lval_T	*lp,
    typval_T	*var1,
    typval_T	*var2,
    ಪೂರ್ಣಾಙ್ಕ		empty1,
    ಪೂರ್ಣಾಙ್ಕ		quiet)
{
    long	bloblen = blob_len(lp->ll_tv->vval.v_blob);

    lp->ll_list = ರಿಕ್ತ;
    lp->ll_dict = ರಿಕ್ತ;
    lp->ll_object = ರಿಕ್ತ;
    lp->ll_class = ರಿಕ್ತ;
    lp->ll_tuple = ರಿಕ್ತ;

    // Get the number and item for the only or first index of a List or Tuple.
    ಯದಿ (empty1)
	lp->ll_n1 = 0;
    ಅನ್ಯಥಾ
	// is number or string
	lp->ll_n1 = (long)tv_get_ಸಂಖ್ಯೆ(var1);

    ಯದಿ (check_blob_index(bloblen, lp->ll_n1, quiet) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಯದಿ (lp->ll_range && !lp->ll_empty2)
    {
	lp->ll_n2 = (long)tv_get_ಸಂಖ್ಯೆ(var2);
	ಯದಿ (check_blob_range(bloblen, lp->ll_n1, lp->ll_n2, quiet) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಯದಿ (!lp->ll_range)
	// Indexing a single byte in a blob.  So the rhs type is a
	// number.
	lp->ll_valtype = &t_ಸಂಖ್ಯೆ;

    lp->ll_blob = lp->ll_tv->vval.v_blob;
    lp->ll_tv = ರಿಕ್ತ;

    ಪ್ರತ್ಯಾಗಮ OK;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_lval_list(
    lval_T	*lp,
    typval_T	*var1,
    typval_T	*var2,
    ಪೂರ್ಣಾಙ್ಕ		empty1,
    ಪೂರ್ಣಾಙ್ಕ		flags,
    ಪೂರ್ಣಾಙ್ಕ		quiet)
{
    /*
     * Get the number and item for the only or first index of the List.
     */
    ಯದಿ (empty1)
	lp->ll_n1 = 0;
    ಅನ್ಯಥಾ
	// is number or string
	lp->ll_n1 = (long)tv_get_ಸಂಖ್ಯೆ(var1);

    lp->ll_dict = ರಿಕ್ತ;
    lp->ll_object = ರಿಕ್ತ;
    lp->ll_class = ರಿಕ್ತ;
    lp->ll_tuple = ರಿಕ್ತ;
    lp->ll_list = lp->ll_tv->vval.v_list;
    lp->ll_li = check_range_index_one(lp->ll_list, &lp->ll_n1,
				(flags & GLV_ASSIGN_WITH_OP) == 0, quiet);
    ಯದಿ (lp->ll_li == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (lp->ll_valtype != ರಿಕ್ತ && !lp->ll_range)
    {
	// use the type of the member
	ಯದಿ (lp->ll_valtype->tt_member != ರಿಕ್ತ)
	    lp->ll_valtype = lp->ll_valtype->tt_member;
	ಅನ್ಯಥಾ
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
    ಯದಿ (lp->ll_range && !lp->ll_empty2)
    {
	lp->ll_n2 = (long)tv_get_ಸಂಖ್ಯೆ(var2);
	// is number or string
	ಯದಿ (check_range_index_two(lp->ll_list,
			&lp->ll_n1, lp->ll_li, &lp->ll_n2, quiet) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    lp->ll_tv = &lp->ll_li->li_tv;

    ಪ್ರತ್ಯಾಗಮ OK;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_lval_tuple(
    lval_T	*lp,
    typval_T	*idx,
    ಪೂರ್ಣಾಙ್ಕ		quiet)
{
    // is number or string
    lp->ll_n1 = (long)tv_get_ಸಂಖ್ಯೆ(idx);

    lp->ll_list = ರಿಕ್ತ;
    lp->ll_dict = ರಿಕ್ತ;
    lp->ll_blob = ರಿಕ್ತ;
    lp->ll_object = ರಿಕ್ತ;
    lp->ll_class = ರಿಕ್ತ;

    lp->ll_tuple = lp->ll_tv->vval.v_tuple;
    lp->ll_tv = tuple_find(lp->ll_tuple, lp->ll_n1);
    ಯದಿ (lp->ll_tv == ರಿಕ್ತ)
    {
	ಯದಿ (!quiet)
	    semsg(_(e_tuple_index_out_of_range_nr), lp->ll_n1);
	ಪ್ರತ್ಯಾಗಮ GLV_FAIL;
    }

    // use the type of the member
    ಯದಿ (lp->ll_valtype != ರಿಕ್ತ)
    {
	ಯದಿ (lp->ll_valtype != ರಿಕ್ತ
		&& lp->ll_valtype->tt_type == VAR_TUPLE
		&& lp->ll_valtype->tt_argcount == 1)
	{
	    // a variadic tuple or a single item tuple
	    ಯದಿ (lp->ll_valtype->tt_flags & TTFLAG_VARARGS)
		lp->ll_valtype = lp->ll_valtype->tt_args[0]->tt_member;
	    ಅನ್ಯಥಾ
		lp->ll_valtype = lp->ll_valtype->tt_args[0];
	}
	ಅನ್ಯಥಾ
	    // If the LHS member type is not known (VAR_ANY), then get it from
	    // the tuple item (after indexing)
	    lp->ll_valtype = typval2type(lp->ll_tv, get_copyID(),
					&lp->ll_type_list, TVTT_DO_MEMBER);
    }

    ಪ್ರತ್ಯಾಗಮ GLV_OK;
}

/*
 * Get a class or object lval method in class "cl".  The 'key' argument points
 * to the method name and 'key_end' points to the character after 'key'.
 * 'v_type' is VAR_CLASS or VAR_OBJECT.
 *
 * The method index, method function pointer and method type are returned in
 * "lp".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_lval_oc_method(
    lval_T	*lp,
    class_T	*cl,
    char_u	*ಕೀಲಿ,
    char_u	**ಕೀಲಿ_end,
    vartype_T	v_type)
{
    // Look for a method with this name.
    // round 1: class functions (skipped for an object)
    // round 2: object methods
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ round = v_type == VAR_OBJECT ? 2 : 1; round <= 2; ++round)
    {
	ಪೂರ್ಣಾಙ್ಕ	m_idx;
	ufunc_T	*fp;

	fp = method_lookup(cl, round == 1 ? VAR_CLASS : VAR_OBJECT,
						ಕೀಲಿ, *ಕೀಲಿ_end - ಕೀಲಿ, &m_idx);
	lp->ll_oi = m_idx;

	// process generic method (if present)
	ಯದಿ (fp && (fp = eval_generic_func(fp, ಕೀಲಿ, ಕೀಲಿ_end)) == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	ಯದಿ (fp != ರಿಕ್ತ)
	{
	    lp->ll_ufunc = fp;
	    lp->ll_valtype = fp->uf_func_type;
	    ಭಙ್ಗ;
	}
    }

    ಪ್ರತ್ಯಾಗಮ OK;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_lval_oc_variable(
    lval_T	*lp,
    class_T	*cl,
    char_u	*ಕೀಲಿ,
    char_u	*ಕೀಲಿ_end,
    vartype_T	v_type,
    class_T	*cl_exec,
    ಪೂರ್ಣಾಙ್ಕ		flags)
{
    ಪೂರ್ಣಾಙ್ಕ		m_idx;
    ocmember_T	*om;

    om = member_lookup(cl, v_type, ಕೀಲಿ, ಕೀಲಿ_end - ಕೀಲಿ, &m_idx);
    lp->ll_oi = m_idx;
    ಯದಿ (om == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ OK;

    // Check variable is accessible
    ಯದಿ (get_lval_check_access(cl_exec, cl, om, ಕೀಲಿ_end, flags) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    // When lhs is used to modify the variable, check it is not a read-only
    // variable.
    ಯದಿ ((flags & GLV_READ_ONLY) == 0 && (*ಕೀಲಿ_end != '.' && *ಕೀಲಿ_end != '[')
	    && oc_var_check_ro(cl, om))
	ಪ್ರತ್ಯಾಗಮ FAIL;

    lp->ll_valtype = om->ocm_type;

    ಯದಿ (v_type == VAR_OBJECT)
	lp->ll_tv = ((typval_T *)(lp->ll_tv->vval.v_object + 1)) + m_idx;
    ಅನ್ಯಥಾ
	lp->ll_tv = &cl->class_members_tv[m_idx];

    ಪ್ರತ್ಯಾಗಮ OK;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_lval_class_or_obj(
    lval_T	*lp,
    char_u	*ಕೀಲಿ,
    char_u	**ಕೀಲಿ_end,
    vartype_T	v_type,
    class_T	*cl_exec,
    ಪೂರ್ಣಾಙ್ಕ		flags,
    ಪೂರ್ಣಾಙ್ಕ		quiet)
{
    lp->ll_dict = ರಿಕ್ತ;
    lp->ll_list = ರಿಕ್ತ;
    lp->ll_tuple = ರಿಕ್ತ;

    class_T *cl;
    ಯದಿ (v_type == VAR_OBJECT)
    {
	ಯದಿ (lp->ll_tv->vval.v_object == ರಿಕ್ತ)
	{
	    ಯದಿ (!quiet)
		emsg(_(e_using_null_object));
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	cl = lp->ll_tv->vval.v_object->obj_class;
	lp->ll_object = lp->ll_tv->vval.v_object;
    }
    ಅನ್ಯಥಾ
    {
	cl = lp->ll_tv->vval.v_class;
	lp->ll_object = ರಿಕ್ತ;
    }
    lp->ll_class = cl;

    ಯದಿ (cl == ರಿಕ್ತ)
	// TODO: what if class is NULL?
	ಪ್ರತ್ಯಾಗಮ OK;

    lp->ll_valtype = ರಿಕ್ತ;

    ಯದಿ (flags & GLV_PREFER_FUNC)
	ಯದಿ (get_lval_oc_method(lp, cl, ಕೀಲಿ, ಕೀಲಿ_end, v_type) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;

    // Look for object/class member variable
    ಯದಿ (lp->ll_valtype == ರಿಕ್ತ)
    {
	ಯದಿ (get_lval_oc_variable(lp, cl, ಕೀಲಿ, *ಕೀಲಿ_end, v_type, cl_exec, flags)
								== FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಯದಿ (lp->ll_valtype == ರಿಕ್ತ)
    {
	member_not_found_ಸಂದೇಶ(cl, v_type, ಕೀಲಿ, *ಕೀಲಿ_end - ಕೀಲಿ);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Check whether dot (".") is allowed after the variable "name" with type
 * "v_type".  Only Dict, Class and Object types support a dot after the name.
 * Returns TRUE if dot is allowed after the name.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
dot_allowed_after_type(char_u *name, vartype_T v_type, ಪೂರ್ಣಾಙ್ಕ quiet)
{
    ಯದಿ (v_type != VAR_DICT && v_type != VAR_OBJECT && v_type != VAR_CLASS)
    {
	ಯದಿ (!quiet)
	    semsg(_(e_dot_not_allowed_after_str_str),
		    vartype_name(v_type), name);
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }

    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * Check whether left bracket ("[") is allowed after the variable "name" with
 * type "v_type".  Only Dict, List, Tuple and Blob types support a bracket
 * after the variable name.  Returns TRUE if bracket is allowed after the name.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
bracket_allowed_after_type(char_u *name, vartype_T v_type, ಪೂರ್ಣಾಙ್ಕ quiet)
{
    ಯದಿ (v_type == VAR_CLASS || v_type == VAR_OBJECT)
    {
	ಯದಿ (!quiet)
	    semsg(_(e_index_not_allowed_after_str_str),
		    vartype_name(v_type), name);
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }

    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * Check whether the variable "name" with type "v_type" can be followed by an
 * index.  Only Dict, List, Tuple, Blob, Object and Class types support
 * indexing.  Returns TRUE if indexing is allowed after the name.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
index_allowed_after_type(char_u *name, vartype_T v_type, ಪೂರ್ಣಾಙ್ಕ quiet)
{
    ಯದಿ (v_type != VAR_LIST
	    && v_type != VAR_TUPLE
	    && v_type != VAR_DICT
	    && v_type != VAR_BLOB
	    && v_type != VAR_OBJECT
	    && v_type != VAR_CLASS)
    {
	ಯದಿ (!quiet)
	    semsg(_(e_index_not_allowed_after_str_str),
		    vartype_name(v_type), name);
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }

    ಪ್ರತ್ಯಾಗಮ TRUE;
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
    ಸ್ಥಿರ char_u *
get_lval_subscript(
    lval_T	*lp,
    char_u	*p,
    char_u	*name,
    typval_T	*rettv,
    hashtab_T	*ht,
    dictitem_T	*v,
    ಪೂರ್ಣಾಙ್ಕ		unlet,
    ಪೂರ್ಣಾಙ್ಕ		flags,	    // GLV_ values
    class_T	*cl_exec)
{
    ಪೂರ್ಣಾಙ್ಕ		quiet = flags & GLV_QUIET;
    char_u	*ಕೀಲಿ = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		len;
    typval_T	var1;
    typval_T	var2;
    ಪೂರ್ಣಾಙ್ಕ		empty1 = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		rc = FAIL;

    /*
     * Loop until no more [idx] or .key is following.
     */
    var1.v_type = VAR_UNKNOWN;
    var2.v_type = VAR_UNKNOWN;

    ಯಾವತ್ (*p == '[' || (*p == '.' && p[1] != '=' && p[1] != '.'))
    {
	vartype_T v_type = lp->ll_tv->v_type;

	ಯದಿ (*p == '.' && !dot_allowed_after_type(name, v_type, quiet))
	    ಗಚ್ಛ done;

	ಯದಿ (*p == '[' && !bracket_allowed_after_type(name, v_type, quiet))
	    ಗಚ್ಛ done;

	ಯದಿ (!index_allowed_after_type(name, v_type, quiet))
	    ಗಚ್ಛ done;

	// A NULL list/blob works like an empty list/blob, allocate one now.
	ಪೂರ್ಣಾಙ್ಕ r = OK;
	ಯದಿ (v_type == VAR_LIST && lp->ll_tv->vval.v_list == ರಿಕ್ತ)
	    r = rettv_list_alloc(lp->ll_tv);
	ಅನ್ಯಥಾ ಯದಿ (v_type == VAR_BLOB && lp->ll_tv->vval.v_blob == ರಿಕ್ತ)
	    r = rettv_blob_alloc(lp->ll_tv);
	ಯದಿ (r == FAIL)
	    ಗಚ್ಛ done;

	ಯದಿ (lp->ll_range)
	{
	    ಯದಿ (!quiet)
		emsg(_(e_slice_must_come_last));
	    ಗಚ್ಛ done;
	}
#ifdef LOG_LOCKVAR
	ch_log(ರಿಕ್ತ, "LKVAR: get_lval() loop: p: %s, type: %s", p,
		vartype_name(v_type));
#endif

	ಯದಿ (current_script_is_vim9() && lp->ll_valtype == ರಿಕ್ತ
		&& v != ರಿಕ್ತ
		&& lp->ll_tv == &v->di_tv
		&& ht != ರಿಕ್ತ && ht == get_script_local_ht())
	{
	    svar_T  *sv = find_typval_in_script(lp->ll_tv, 0, TRUE);

	    // Vim9 script local variable: get the type
	    ಯದಿ (sv != ರಿಕ್ತ)
	    {
		lp->ll_valtype = sv->sv_type;
#ifdef LOG_LOCKVAR
		ch_log(ರಿಕ್ತ, "LKVAR:    ... loop: vim9 assign type: %s",
			vartype_name(lp->ll_valtype->tt_type));
#endif
	    }
	}

	len = -1;
	ಯದಿ (*p == '.')
	{
	    ಕೀಲಿ = p + 1;

	    ಪ್ರತಿ (len = 0; ASCII_ISALNUM(ಕೀಲಿ[len]) || ಕೀಲಿ[len] == '_'; ++len)
		;
	    ಯದಿ (len == 0)
	    {
		ಯದಿ (!quiet)
		    emsg(_(e_cannot_use_empty_ಕೀಲಿ_for_dictionary));
		ಗಚ್ಛ done;
	    }
	    p = ಕೀಲಿ + len;
	}
	ಅನ್ಯಥಾ
	{
	    // Get the index [expr] or the first index [expr: ].
	    p = skipwhite(p + 1);
	    ಯದಿ (*p == ':')
		empty1 = TRUE;
	    ಅನ್ಯಥಾ
	    {
		empty1 = FALSE;
		ಯದಿ (eval1(&p, &var1, &EVALARG_EVALUATE) == FAIL)  // recursive!
		    ಗಚ್ಛ done;
		ಯದಿ (tv_get_string_chk(&var1) == ರಿಕ್ತ)
		    // not a number or string
		    ಗಚ್ಛ done;
		p = skipwhite(p);
	    }

	    // Optionally get the second index [ :expr].
	    ಯದಿ (*p == ':')
	    {
		ಯದಿ (v_type == VAR_DICT)
		{
		    ಯದಿ (!quiet)
			emsg(_(e_cannot_slice_dictionary));
		    ಗಚ್ಛ done;
		}
		ಯದಿ (v_type == VAR_TUPLE)
		{
		    ಯದಿ (!quiet)
			emsg(_(e_cannot_slice_tuple));
		    ಗಚ್ಛ done;
		}
		ಯದಿ (rettv != ರಿಕ್ತ
			&& !(rettv->v_type == VAR_LIST
			    && rettv->vval.v_list != ರಿಕ್ತ)
			&& !(rettv->v_type == VAR_BLOB
			    && rettv->vval.v_blob != ರಿಕ್ತ))
		{
		    ಯದಿ (!quiet)
			emsg(_(e_slice_requires_list_or_blob_ಮೌಲ್ಯ));
		    ಗಚ್ಛ done;
		}
		p = skipwhite(p + 1);
		ಯದಿ (*p == ']')
		    lp->ll_empty2 = TRUE;
		ಅನ್ಯಥಾ
		{
		    lp->ll_empty2 = FALSE;
		    // recursive!
		    ಯದಿ (eval1(&p, &var2, &EVALARG_EVALUATE) == FAIL)
			ಗಚ್ಛ done;
		    ಯದಿ (tv_get_string_chk(&var2) == ರಿಕ್ತ)
			// not a number or string
			ಗಚ್ಛ done;
		}
		lp->ll_range = TRUE;
	    }
	    ಅನ್ಯಥಾ
		lp->ll_range = FALSE;

	    ಯದಿ (*p != ']')
	    {
		ಯದಿ (!quiet)
		    emsg(_(e_missing_closing_square_brace));
		ಗಚ್ಛ done;
	    }

	    // Skip to past ']'.
	    ++p;
	}
#ifdef LOG_LOCKVAR
	ಯದಿ (len == -1)
	    ch_log(ರಿಕ್ತ, "LKVAR:    ... loop: p: %s, '[' key: %s", p,
		    empty1 ? ":" : (ವರ್ಣ*)tv_get_string(&var1));
	ಅನ್ಯಥಾ
	    ch_log(ರಿಕ್ತ, "LKVAR:    ... loop: p: %s, '.' key: %s", p, ಕೀಲಿ);
#endif

	ಯದಿ (v_type == VAR_DICT)
	{
	    glv_status_T glv_status;

	    glv_status = get_lval_dict_item(lp, name, ಕೀಲಿ, len, &p, &var1,
							flags, unlet, rettv);
	    ಯದಿ (glv_status == GLV_FAIL)
		ಗಚ್ಛ done;
	    ಯದಿ (glv_status == GLV_STOP)
		ಭಙ್ಗ;
	}
	ಅನ್ಯಥಾ ಯದಿ (v_type == VAR_BLOB)
	{
	    ಯದಿ (get_lval_blob(lp, &var1, &var2, empty1, quiet) == FAIL)
		ಗಚ್ಛ done;

	    ಭಙ್ಗ;
	}
	ಅನ್ಯಥಾ ಯದಿ (v_type == VAR_LIST)
	{
	    ಯದಿ (get_lval_list(lp, &var1, &var2, empty1, flags, quiet) == FAIL)
		ಗಚ್ಛ done;
	}
	ಅನ್ಯಥಾ ಯದಿ (v_type == VAR_TUPLE)
	{
	    ಯದಿ (get_lval_tuple(lp, &var1, quiet) == FAIL)
		ಗಚ್ಛ done;
	}
	ಅನ್ಯಥಾ  // v_type == VAR_CLASS || v_type == VAR_OBJECT
	{
	    ಯದಿ (get_lval_class_or_obj(lp, ಕೀಲಿ, &p, v_type, cl_exec, flags,
							quiet) == FAIL)
		ಗಚ್ಛ done;
	}

	clear_tv(&var1);
	clear_tv(&var2);
	var1.v_type = VAR_UNKNOWN;
	var2.v_type = VAR_UNKNOWN;
    }

    ಯದಿ (lp->ll_tuple != ರಿಕ್ತ && (flags & GLV_READ_ONLY) == 0)
    {
	ಯದಿ (!quiet)
	    emsg(_(e_tuple_is_immutable));
	ಗಚ್ಛ done;
    }

    rc = OK;

done:
    clear_tv(&var1);
    clear_tv(&var2);
    ಪ್ರತ್ಯಾಗಮ rc == OK ? p : ರಿಕ್ತ;
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
    ಪೂರ್ಣಾಙ್ಕ		unlet,
    ಪೂರ್ಣಾಙ್ಕ		skip,
    ಪೂರ್ಣಾಙ್ಕ		flags,	    // GLV_ values
    ಪೂರ್ಣಾಙ್ಕ		fne_flags)  // flags for find_name_end()
{
    char_u	*p;
    char_u	*ಅಭಿವ್ಯಕ್ತಿ_start, *ಅಭಿವ್ಯಕ್ತಿ_end;
    ಪೂರ್ಣಾಙ್ಕ		cc;
    dictitem_T	*v = ರಿಕ್ತ;
    hashtab_T	*ht = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		quiet = flags & GLV_QUIET;
    ಪೂರ್ಣಾಙ್ಕ		writing = 0;
    ಪೂರ್ಣಾಙ್ಕ		vim9script = in_vim9script();
    class_T	*cl_exec = ರಿಕ್ತ;    // class that is executing, or NULL.

#ifdef LOG_LOCKVAR
    ಯದಿ (lval_ಬೇರು == ರಿಕ್ತ)
	ch_log(ರಿಕ್ತ, "LKVAR: get_lval(): name: %s, lval_root (nil)", name);
    ಅನ್ಯಥಾ
	ch_log(ರಿಕ್ತ, "LKVAR: get_lval(): name: %s, lr_tv %p lr_is_arg %d",
			name, (ಶೂನ್ಯ*)lval_ಬೇರು->lr_tv, lval_ಬೇರು->lr_is_arg);
    ವರ್ಣ buf[80];
    ch_log(ರಿಕ್ತ, "LKVAR:    ...: GLV flags: %s",
		    flags_tostring(flags, glv_flag_strings, buf, ಪರಿಮಾಣ(buf)));
#endif

    // Clear everything in "lp".
    CLEAR_POINTER(lp);
    ga_init2(&lp->ll_type_list, ಪರಿಮಾಣ(type_T *), 10);

    ಯದಿ (skip || (flags & GLV_COMPILING))
    {
	// When skipping or compiling just find the end of the name.
	lp->ll_name = name;
	lp->ll_name_end = find_name_end(name, ರಿಕ್ತ, ರಿಕ್ತ,
						      FNE_INCL_BR | fne_flags);
	ಪ್ರತ್ಯಾಗಮ lp->ll_name_end;
    }

    // Cannot use "s:var" at the Vim9 script level.  "s: type" is OK.
    ಯದಿ (vim9script && at_script_level()
		  && name[0] == 's' && name[1] == ':' && !VIM_ISWHITE(name[2]))
    {
	semsg(_(e_cannot_use_s_colon_in_vim9_script_str), name);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    // Find the end of the name.
    p = find_name_end(name, &ಅಭಿವ್ಯಕ್ತಿ_start, &ಅಭಿವ್ಯಕ್ತಿ_end, fne_flags);
    lp->ll_name_end = p;
    ಯದಿ (ಅಭಿವ್ಯಕ್ತಿ_start != ರಿಕ್ತ)
    {
	// Don't expand the name when we already know there is an error.
	ಯದಿ (unlet && !VIM_ISWHITE(*p) && !ends_excmd(*p)
						    && *p != '[' && *p != '.')
	{
	    semsg(_(e_trailing_characters_str), p);
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}

	lp->ll_exp_name = make_expanded_name(name, ಅಭಿವ್ಯಕ್ತಿ_start, ಅಭಿವ್ಯಕ್ತಿ_end, p);
	ಯದಿ (lp->ll_exp_name == ರಿಕ್ತ)
	{
	    // Report an invalid expression in braces, unless the
	    // expression evaluation has been cancelled due to an
	    // aborting error, an interrupt, or an exception.
	    ಯದಿ (!aborting() && !quiet)
	    {
		emsg_severe = TRUE;
		semsg(_(e_invalid_argument_str), name);
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    }
	}
	lp->ll_name = lp->ll_exp_name;
    }
    ಅನ್ಯಥಾ
    {
	lp->ll_name = name;

	ಯದಿ (vim9script)
	{
	    // "a: type" is declaring variable "a" with a type, not "a:".
	    // However, "g:[key]" is indexing a dictionary.
	    ಯದಿ (p == name + 2 && p[-1] == ':' && *p != '[')
	    {
		--p;
		lp->ll_name_end = p;
	    }
	    ಯದಿ (*skipwhite(p) == ':')
	    {
		char_u	    *tp = skipwhite(p + 1);

		ಯದಿ (is_scoped_variable(name))
		{
		    semsg(_(e_cannot_use_type_with_this_variable_str), name);
		    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
		}
		ಯದಿ (VIM_ISWHITE(*p))
		{
		    semsg(_(e_no_white_space_allowed_before_colon_str), p);
		    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
		}
		ಯದಿ (tp == p + 1 && !quiet)
		{
		    semsg(_(e_white_space_required_after_str_str), ":", p);
		    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
		}
		ಯದಿ (!SCRIPT_ID_VALID(current_sctx.sc_sid))
		{
		    semsg(_(e_using_type_not_in_script_context_str), p);
		    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
		}
		ಯದಿ (vim9script && (flags & GLV_NO_DECL) &&
			!(flags & GLV_FOR_LOOP))
		{
		    // Using a type and not in a "var" declaration.
		    semsg(_(e_trailing_characters_str), p);
		    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
		}


		// parse the type after the name
		lp->ll_type = parse_type(&tp,
			       &SCRIPT_ITEM(current_sctx.sc_sid)->sn_type_list,
			       ರಿಕ್ತ, ರಿಕ್ತ, !quiet);
		ಯದಿ (!quiet && (lp->ll_type == ರಿಕ್ತ
			    || !valid_declaration_type(lp->ll_type)))
		    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
		lp->ll_name_end = tp;
	    }
	    // TODO: check inside class?
	}
    }
    ಯದಿ (lp->ll_name == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ p;

    ಯದಿ (*p == '.')
    {
	// In legacy script, when a local variable and import exists with this name,
	// prioritize local variable over imports to avoid conflicts.
	ಪೂರ್ಣಾಙ್ಕ var_exists = FALSE;
	ಯದಿ (!vim9script)
	{
	    cc = *p;
	    *p = NUL;
	    hashtab_T *local_ht = get_funccal_local_ht();
	    ಯದಿ (local_ht != ರಿಕ್ತ)
	    {
		hashitem_T *hi = hash_find(local_ht, lp->ll_name);
		ಯದಿ (!HASHITEM_EMPTY(hi))
		    var_exists = TRUE;
	    }
	    *p = cc;
	}

	ಯದಿ (!var_exists)
	{
	    imported_T *import = find_imported(lp->ll_name, p - lp->ll_name, TRUE);
	    ಯದಿ (import != ರಿಕ್ತ)
	    {
		p++;	// skip '.'
		p = get_lval_imported(lp, import->imp_sid, p, &v, fne_flags);
		ಯದಿ (p == ರಿಕ್ತ)
		    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    }
	}
    }

    // Without [idx] or .key we are done.
    ಯದಿ (*p != '[' && *p != '.')
    {
	ಯದಿ (lval_ಬೇರು != ರಿಕ್ತ)
	    fill_lval_from_lval_ಬೇರು(lp, lval_ಬೇರು);
	ಪ್ರತ್ಯಾಗಮ p;
    }

    ಯದಿ (vim9script && lval_ಬೇರು != ರಿಕ್ತ)
	cl_exec = lval_ಬೇರು->lr_cl_exec;
    ಯದಿ (vim9script && lval_ಬೇರು != ರಿಕ್ತ && lval_ಬೇರು->lr_tv != ರಿಕ್ತ)
    {
	// using local variable
	lp->ll_tv = lval_ಬೇರು->lr_tv;
	v = ರಿಕ್ತ;
    }
    ಅನ್ಯಥಾ ಯದಿ (lp->ll_tv == ರಿಕ್ತ)
    {
	cc = *p;
	*p = NUL;
	// When we would write to the variable pass &ht and prevent autoload.
	writing = !(flags & GLV_READ_ONLY);
	v = find_var(lp->ll_name, writing ? &ht : ರಿಕ್ತ,
					 (flags & GLV_NO_AUTOLOAD) || writing);
	ಯದಿ (v == ರಿಕ್ತ && !quiet)
	    semsg(_(e_undefined_variable_str), lp->ll_name);
	*p = cc;
	ಯದಿ (v == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	lp->ll_tv = &v->di_tv;
    }

    ಯದಿ (vim9script && (flags & GLV_NO_DECL) == 0)
    {
	ಯದಿ (!quiet)
	    semsg(_(e_variable_already_declared_str), lp->ll_name);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    // If the next character is a "." or a "[", then process the subitem.
    p = get_lval_subscript(lp, p, name, rettv, ht, v, unlet, flags, cl_exec);
    ಯದಿ (p == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (vim9script && lp->ll_valtype != ರಿಕ್ತ && rettv != ರಿಕ್ತ)
    {
	where_T	    where = WHERE_INIT;

	// In a Vim9 script, do type check and make sure the variable is
	// writable.
	ಯದಿ (check_typval_type(lp->ll_valtype, rettv, where) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    lp->ll_name_end = p;
    ಪ್ರತ್ಯಾಗಮ p;
}

/*
 * Clear lval "lp" that was filled by get_lval().
 */
    ಶೂನ್ಯ
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
    ಶೂನ್ಯ
set_var_lval(
    lval_T	*lp,
    char_u	*endp,
    typval_T	*rettv,
    ಪೂರ್ಣಾಙ್ಕ		copy,
    ಪೂರ್ಣಾಙ್ಕ		flags,	    // ASSIGN_CONST, ASSIGN_NO_DECL
    char_u	*op,
    ಪೂರ್ಣಾಙ್ಕ		var_idx)    // index for "let [a, b] = list"
{
    ಪೂರ್ಣಾಙ್ಕ		cc;
    dictitem_T	*di;

    ಯದಿ (lp->ll_tv == ರಿಕ್ತ)
    {
	cc = *endp;
	*endp = NUL;
	ಯದಿ (in_vim9script() && check_reserved_name(lp->ll_name, FALSE) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ;

	ಯದಿ (lp->ll_blob != ರಿಕ್ತ)
	{
	    ಪೂರ್ಣಾಙ್ಕ	    error = FALSE;

	    ಯದಿ (op != ರಿಕ್ತ && *op != '=')
	    {
		semsg(_(e_wrong_variable_type_for_str_equal), op);
		ಪ್ರತ್ಯಾಗಮ;
	    }
	    ಯದಿ (ಮೌಲ್ಯ_check_lock(lp->ll_blob->bv_lock, lp->ll_name, FALSE))
		ಪ್ರತ್ಯಾಗಮ;

	    ಯದಿ (lp->ll_range && rettv->v_type == VAR_BLOB)
	    {
		ಯದಿ (lp->ll_empty2)
		    lp->ll_n2 = blob_len(lp->ll_blob) - 1;

		ಯದಿ (blob_set_range(lp->ll_blob, lp->ll_n1, lp->ll_n2,
								rettv) == FAIL)
		    ಪ್ರತ್ಯಾಗಮ;
	    }
	    ಅನ್ಯಥಾ
	    {
		varnumber_T	val = tv_get_ಸಂಖ್ಯೆ_chk(rettv, &error);
		ಯದಿ (!error)
		{
		    ಯದಿ (val < 0 || val > 255)
			semsg(_(e_invalid_ಮೌಲ್ಯ_for_blob_nr), val);
		    ಅನ್ಯಥಾ
			blob_set_append(lp->ll_blob, lp->ll_n1, val);
		}
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (op != ರಿಕ್ತ && *op != '=')
	{
	    typval_T tv;

	    ಯದಿ ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					     && (flags & ASSIGN_FOR_LOOP) == 0)
	    {
		emsg(_(e_cannot_modify_existing_variable));
		*endp = cc;
		ಪ್ರತ್ಯಾಗಮ;
	    }

	    // handle +=, -=, *=, /=, %= and .=
	    di = ರಿಕ್ತ;
	    ಯದಿ (eval_variable(lp->ll_name, (ಪೂರ್ಣಾಙ್ಕ)STRLEN(lp->ll_name),
				 lp->ll_sid, &tv, &di, EVAL_VAR_VERBOSE) == OK)
	    {
		ಯದಿ (di != ರಿಕ್ತ && check_typval_is_ಮೌಲ್ಯ(&di->di_tv) == FAIL)
		{
		    clear_tv(&tv);
		    ಪ್ರತ್ಯಾಗಮ;
		}

		// In Vim9 script a String v: variable only takes a String.
		ಯದಿ (in_vim9script() && STRNCMP(lp->ll_name, "v:", 2) == 0
			&& tv.v_type == VAR_STRING)
		{
		    where_T	where = WHERE_INIT;

		    ಯದಿ (check_typval_type(&t_string, rettv, where) == FAIL)
		    {
			clear_tv(&tv);
			*endp = cc;
			ಪ್ರತ್ಯಾಗಮ;
		    }
		}

		ಯದಿ ((di == ರಿಕ್ತ
			 || (!var_check_ro(di->di_flags, lp->ll_name, FALSE)
			   && !tv_check_lock(&di->di_tv, lp->ll_name, FALSE)))
			&& tv_op(&tv, rettv, op) == OK)
		    set_var_const(lp->ll_name, lp->ll_sid, ರಿಕ್ತ, &tv, FALSE,
				ASSIGN_NO_DECL | ASSIGN_COMPOUND_OP, 0);
		clear_tv(&tv);
	    }
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (lp->ll_type != ರಿಕ್ತ && check_typval_arg_type(lp->ll_type, rettv,
							      ರಿಕ್ತ, 0) == FAIL)
		ಪ್ರತ್ಯಾಗಮ;
	    set_var_const(lp->ll_name, lp->ll_sid, lp->ll_type, rettv, copy,
							       flags, var_idx);
	}
	*endp = cc;
    }
    ಅನ್ಯಥಾ ಯದಿ (ಮೌಲ್ಯ_check_lock(lp->ll_newkey == ರಿಕ್ತ
		? lp->ll_tv->v_lock
		: lp->ll_tv->vval.v_dict->dv_lock, lp->ll_name, FALSE))
	;
    ಅನ್ಯಥಾ ಯದಿ (lp->ll_range)
    {
	ಯದಿ ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					     && (flags & ASSIGN_FOR_LOOP) == 0)
	{
	    emsg(_(e_cannot_lock_range));
	    ಪ್ರತ್ಯಾಗಮ;
	}

	(ಶೂನ್ಯ)list_assign_range(lp->ll_list, rettv->vval.v_list,
			 lp->ll_n1, lp->ll_n2, lp->ll_empty2, op, lp->ll_name);
    }
    ಅನ್ಯಥಾ
    {
	/*
	 * Assign to a List, Dictionary or Object item.
	 */
	ಯದಿ ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					     && (flags & ASSIGN_FOR_LOOP) == 0)
	{
	    emsg(_(e_cannot_lock_list_or_dict));
	    ಪ್ರತ್ಯಾಗಮ;
	}

	ಯದಿ (lp->ll_valtype != ರಿಕ್ತ
		    && check_typval_arg_type(lp->ll_valtype, rettv,
							      ರಿಕ್ತ, 0) == FAIL)
	{
	    lp->ll_name_end = ರಿಕ್ತ;
	    ಪ್ರತ್ಯಾಗಮ;
	}

	// If the lval is a List and the type of the list is not yet set,
	// then set the item type from the declared type of the variable.
	ಯದಿ (in_vim9script() && rettv->v_type == VAR_LIST
				&& rettv->vval.v_list != ರಿಕ್ತ
				&& rettv->vval.v_list->lv_type == ರಿಕ್ತ)
	{
	    ಯದಿ (lp->ll_tv->v_type == VAR_LIST
		    && lp->ll_tv->vval.v_list != ರಿಕ್ತ
		    && lp->ll_tv->vval.v_list->lv_type != ರಿಕ್ತ)
		set_tv_type(rettv, lp->ll_tv->vval.v_list->lv_type);
	}

	ಯದಿ (lp->ll_newkey != ರಿಕ್ತ)
	{
	    ಯದಿ (op != ರಿಕ್ತ && *op != '=')
	    {
		semsg(_(e_ಕೀಲಿ_not_present_in_dictionary_str), lp->ll_newkey);
		ಪ್ರತ್ಯಾಗಮ;
	    }
	    ಯದಿ (dict_wrong_func_name(lp->ll_tv->vval.v_dict, rettv,
								lp->ll_newkey))
		ಪ್ರತ್ಯಾಗಮ;

	    // Need to add an item to the Dictionary.
	    di = dictitem_alloc(lp->ll_newkey);
	    ಯದಿ (di == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ;
	    ಯದಿ (dict_add(lp->ll_tv->vval.v_dict, di) == FAIL)
	    {
		vim_free(di);
		ಪ್ರತ್ಯಾಗಮ;
	    }
	    lp->ll_tv = &di->di_tv;
	}
	ಅನ್ಯಥಾ ಯದಿ (op != ರಿಕ್ತ && *op != '=')
	{
	    tv_op(lp->ll_tv, rettv, op);
	    ಪ್ರತ್ಯಾಗಮ;
	}
	ಅನ್ಯಥಾ
	    clear_tv(lp->ll_tv);

	/*
	 * Assign the value to the variable or list item.
	 */
	ಯದಿ (copy)
	    copy_tv(rettv, lp->ll_tv);
	ಅನ್ಯಥಾ
	{
	    *lp->ll_tv = *rettv;
	    lp->ll_tv->v_lock = 0;
	    init_tv(rettv);
	}
    }
}

/*
 * Handle "blob1 += blob2".
 * Returns OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
tv_op_blob(typval_T *tv1, typval_T *tv2, char_u *op)
{
    ಯದಿ (*op != '+' || tv2->v_type != VAR_BLOB)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    // Blob += Blob
    ಯದಿ (tv2->vval.v_blob == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ OK;

    ಯದಿ (tv1->vval.v_blob == ರಿಕ್ತ)
    {
	tv1->vval.v_blob = tv2->vval.v_blob;
	++tv1->vval.v_blob->bv_refcount;
	ಪ್ರತ್ಯಾಗಮ OK;
    }

    blob_T	*b1 = tv1->vval.v_blob;
    blob_T	*b2 = tv2->vval.v_blob;
    ಪೂರ್ಣಾಙ್ಕ		len = blob_len(b2);

    ಯದಿ (len > 0 && ga_grow(&b1->bv_ga, len) == OK)
    {
	mch_memmove((char_u *)b1->bv_ga.ga_data + b1->bv_ga.ga_len,
		    (char_u *)b2->bv_ga.ga_data, (size_t)len);
	b1->bv_ga.ga_len += len;
    }
    ಅನ್ಯಥಾ
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < len; i++)
	    (ಶೂನ್ಯ)ga_append(&b1->bv_ga, blob_get(b2, i));

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Handle "list1 += list2".
 * Returns OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
tv_op_list(typval_T *tv1, typval_T *tv2, char_u *op)
{
    ಯದಿ (*op != '+' || tv2->v_type != VAR_LIST)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    // List += List
    ಯದಿ (tv2->vval.v_list == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ OK;

    ಯದಿ (tv1->vval.v_list == ರಿಕ್ತ)
    {
	tv1->vval.v_list = tv2->vval.v_list;
	++tv1->vval.v_list->lv_refcount;
    }
    ಅನ್ಯಥಾ
	list_extend(tv1->vval.v_list, tv2->vval.v_list, ರಿಕ್ತ);

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Handle number operations:
 *	nr += nr , nr -= nr , nr *=nr , nr /= nr , nr %= nr
 *
 * Returns OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
tv_op_ಸಂಖ್ಯೆ(typval_T *tv1, typval_T *tv2, char_u *op)
{
    varnumber_T	n;
    ಪೂರ್ಣಾಙ್ಕ		failed = FALSE;

    n = tv_get_ಸಂಖ್ಯೆ(tv1);
    ಯದಿ (tv2->v_type == VAR_FLOAT)
    {
	float_T f = n;

	ಯದಿ (*op == '%')
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	ವಿಕಲ್ಪ (*op)
	{
	    ಪಕ್ಷ '+': f += tv2->vval.v_float; ಭಙ್ಗ;
	    ಪಕ್ಷ '-': f -= tv2->vval.v_float; ಭಙ್ಗ;
	    ಪಕ್ಷ '*': f *= tv2->vval.v_float; ಭಙ್ಗ;
	    ಪಕ್ಷ '/': f /= tv2->vval.v_float; ಭಙ್ಗ;
	}
	clear_tv(tv1);
	tv1->v_type = VAR_FLOAT;
	tv1->vval.v_float = f;
    }
    ಅನ್ಯಥಾ
    {
	ವಿಕಲ್ಪ (*op)
	{
	    ಪಕ್ಷ '+': n += tv_get_ಸಂಖ್ಯೆ(tv2); ಭಙ್ಗ;
	    ಪಕ್ಷ '-': n -= tv_get_ಸಂಖ್ಯೆ(tv2); ಭಙ್ಗ;
	    ಪಕ್ಷ '*': n *= tv_get_ಸಂಖ್ಯೆ(tv2); ಭಙ್ಗ;
	    ಪಕ್ಷ '/': n = ಅಂಕೆ_divide(n, tv_get_ಸಂಖ್ಯೆ(tv2), &failed); ಭಙ್ಗ;
	    ಪಕ್ಷ '%': n = ಅಂಕೆ_modulus(n, tv_get_ಸಂಖ್ಯೆ(tv2), &failed); ಭಙ್ಗ;
	}
	clear_tv(tv1);
	tv1->v_type = VAR_ಸಂಖ್ಯೆ;
	tv1->vval.v_ಸಂಖ್ಯೆ = n;
    }

    ಪ್ರತ್ಯಾಗಮ failed ? FAIL : OK;
}

/*
 * Append string "s2" to the string in "tv1".
 * Returns OK if "tv1" was grown in place, FAIL otherwise.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
grow_string_tv(typval_T *tv1, char_u *s2)
{
    ಯದಿ (tv1->v_type != VAR_STRING || tv1->vval.v_string == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    size_t len1 = STRLEN(tv1->vval.v_string);
    size_t len2 = STRLEN(s2);
    char_u *p = vim_realloc(tv1->vval.v_string, len1 + len2 + 1);

    ಯದಿ (p == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    mch_memmove(p + len1, s2, len2 + 1);
    tv1->vval.v_string = p;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Handle "str1 .= str2"
 * Returns OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
tv_op_string(typval_T *tv1, typval_T *tv2, char_u *op UNUSED)
{
    char_u	numbuf[NUMBUFLEN];
    char_u	*s;
    char_u	*s2;

    ಯದಿ (tv2->v_type == VAR_FLOAT)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    // str .= str
    s2 = tv_get_string_buf(tv2, numbuf);
    ಯದಿ (grow_string_tv(tv1, s2) == OK)
	ಪ್ರತ್ಯಾಗಮ OK;

    s = tv_get_string(tv1);
    s = concat_str(s, s2);
    clear_tv(tv1);
    tv1->v_type = VAR_STRING;
    tv1->vval.v_string = s;

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Handle "tv1 += tv2", "tv1 -= tv2", "tv1 *= tv2", "tv1 /= tv2", "tv1 %= tv2"
 * and "tv1 .= tv2"
 * Returns OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
tv_op_nr_or_string(typval_T *tv1, typval_T *tv2, char_u *op)
{
    ಯದಿ (tv2->v_type == VAR_LIST)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (vim_strchr((char_u *)"+-*/%", *op) != ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ tv_op_ಸಂಖ್ಯೆ(tv1, tv2, op);

    ಪ್ರತ್ಯಾಗಮ tv_op_string(tv1, tv2, op);
}

/*
 * Handle "f1 += f2", "f1 -= f2", "f1 *= f2", "f1 /= f2".
 * Returns OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
tv_op_float(typval_T *tv1, typval_T *tv2, char_u *op)
{
    float_T f;

    ಯದಿ (*op == '%' || *op == '.'
	    || (tv2->v_type != VAR_FLOAT
		&& tv2->v_type != VAR_ಸಂಖ್ಯೆ
		&& tv2->v_type != VAR_STRING))
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (tv2->v_type == VAR_FLOAT)
	f = tv2->vval.v_float;
    ಅನ್ಯಥಾ
	f = tv_get_ಸಂಖ್ಯೆ(tv2);
    ವಿಕಲ್ಪ (*op)
    {
	ಪಕ್ಷ '+': tv1->vval.v_float += f; ಭಙ್ಗ;
	ಪಕ್ಷ '-': tv1->vval.v_float -= f; ಭಙ್ಗ;
	ಪಕ್ಷ '*': tv1->vval.v_float *= f; ಭಙ್ಗ;
	ಪಕ್ಷ '/': tv1->vval.v_float /= f; ಭಙ್ಗ;
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Handle "tv1 += tv2", "tv1 -= tv2", "tv1 *= tv2", "tv1 /= tv2", "tv1 %= tv2"
 * and "tv1 .= tv2"
 * Returns OK or FAIL.
 */
    ಪೂರ್ಣಾಙ್ಕ
tv_op(typval_T *tv1, typval_T *tv2, char_u *op)
{
    // Can't do anything with a Funcref or Dict on the right.
    // v:true and friends only work with "..=".
    ಯದಿ (tv2->v_type == VAR_FUNC || tv2->v_type == VAR_DICT
		|| ((tv2->v_type == VAR_BOOL || tv2->v_type == VAR_SPECIAL)
							&& *op != '.'))
    {
	semsg(_(e_wrong_variable_type_for_str_equal), op);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಪೂರ್ಣಾಙ್ಕ retval = FAIL;
    ವಿಕಲ್ಪ (tv1->v_type)
    {
	ಪಕ್ಷ VAR_UNKNOWN:
	ಪಕ್ಷ VAR_ANY:
	ಪಕ್ಷ VAR_VOID:
	ಪಕ್ಷ VAR_DICT:
	ಪಕ್ಷ VAR_FUNC:
	ಪಕ್ಷ VAR_PARTIAL:
	ಪಕ್ಷ VAR_BOOL:
	ಪಕ್ಷ VAR_SPECIAL:
	ಪಕ್ಷ VAR_JOB:
	ಪಕ್ಷ VAR_CHANNEL:
	ಪಕ್ಷ VAR_INSTR:
	ಪಕ್ಷ VAR_OBJECT:
	ಪಕ್ಷ VAR_CLASS:
	ಪಕ್ಷ VAR_TYPEALIAS:
	ಪಕ್ಷ VAR_TUPLE:
	    ಭಙ್ಗ;

	ಪಕ್ಷ VAR_BLOB:
	    retval = tv_op_blob(tv1, tv2, op);
	    ಭಙ್ಗ;

	ಪಕ್ಷ VAR_LIST:
	    retval = tv_op_list(tv1, tv2, op);
	    ಭಙ್ಗ;

	ಪಕ್ಷ VAR_ಸಂಖ್ಯೆ:
	ಪಕ್ಷ VAR_STRING:
	    retval = tv_op_nr_or_string(tv1, tv2, op);
	    ಭಙ್ಗ;

	ಪಕ್ಷ VAR_FLOAT:
	    retval = tv_op_float(tv1, tv2, op);
	    ಭಙ್ಗ;
    }

    ಯದಿ (retval != OK)
	semsg(_(e_wrong_variable_type_for_str_equal), op);

    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Evaluate the expression used in a ":for var in expr" command.
 * "arg" points to "var".
 * Set "*errp" to TRUE for an error, FALSE otherwise;
 * Return a pointer that holds the info.  Null when there is an error.
 */
    ಶೂನ್ಯ *
eval_for_ಸಾಲು(
    char_u	*arg,
    ಪೂರ್ಣಾಙ್ಕ		*errp,
    exarg_T	*eap,
    evalarg_T	*evalarg)
{
    forinfo_T	*fi;
    char_u	*var_list_end;
    char_u	*ಅಭಿವ್ಯಕ್ತಿ;
    typval_T	tv;
    list_T	*l;
    tuple_T	*tuple;
    ಪೂರ್ಣಾಙ್ಕ		skip = !(evalarg->eval_flags & EVAL_EVALUATE);

    *errp = TRUE;	// default: there is an error

    fi = ALLOC_CLEAR_ONE(forinfo_T);
    ಯದಿ (fi == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    var_list_end = skip_var_list(arg, TRUE, &fi->fi_varcount,
						     &fi->fi_semicolon, FALSE);
    ಯದಿ (var_list_end == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ fi;

    ಅಭಿವ್ಯಕ್ತಿ = skipwhite_and_linebreak(var_list_end, evalarg);
    ಯದಿ (ಅಭಿವ್ಯಕ್ತಿ[0] != 'i' || ಅಭಿವ್ಯಕ್ತಿ[1] != 'n'
				  || !(ಅಭಿವ್ಯಕ್ತಿ[2] == NUL || VIM_ISWHITE(ಅಭಿವ್ಯಕ್ತಿ[2])))
    {
	ಯದಿ (in_vim9script() && *ಅಭಿವ್ಯಕ್ತಿ == ':' && ಅಭಿವ್ಯಕ್ತಿ != var_list_end)
	    semsg(_(e_no_white_space_allowed_before_colon_str), ಅಭಿವ್ಯಕ್ತಿ);
	ಅನ್ಯಥಾ
	    emsg(_(e_missing_in_after_for));
	ಪ್ರತ್ಯಾಗಮ fi;
    }

    ಯದಿ (skip)
	++emsg_skip;
    ಅಭಿವ್ಯಕ್ತಿ = skipwhite_and_linebreak(ಅಭಿವ್ಯಕ್ತಿ + 2, evalarg);
    ಯದಿ (eval0(ಅಭಿವ್ಯಕ್ತಿ, &tv, eap, evalarg) == OK)
    {
	*errp = FALSE;
	ಯದಿ (!skip)
	{
	    ಯದಿ (tv.v_type == VAR_LIST)
	    {
		l = tv.vval.v_list;
		ಯದಿ (l == ರಿಕ್ತ)
		{
		    // a null list is like an empty list: do nothing
		    clear_tv(&tv);
		}
		ಅನ್ಯಥಾ
		{
		    // Need a real list here.
		    CHECK_LIST_MATERIALIZE(l);

		    // No need to increment the refcount, it's already set for
		    // the list being used in "tv".
		    fi->fi_list = l;
		    list_add_watch(l, &fi->fi_lw);
		    fi->fi_lw.lw_item = l->lv_first;
		}
	    }
	    ಅನ್ಯಥಾ ಯದಿ (tv.v_type == VAR_TUPLE)
	    {
		tuple = tv.vval.v_tuple;
		ಯದಿ (tuple == ರಿಕ್ತ)
		{
		    // a null tuple is like an empty tuple: do nothing
		    clear_tv(&tv);
		}
		ಅನ್ಯಥಾ
		{
		    // No need to increment the refcount, it's already set for
		    // the tuple being used in "tv".
		    fi->fi_tuple = tuple;
		    fi->fi_tuple_idx = 0;
		}
	    }
	    ಅನ್ಯಥಾ ಯದಿ (tv.v_type == VAR_BLOB)
	    {
		fi->fi_bi = 0;
		ಯದಿ (tv.vval.v_blob != ರಿಕ್ತ)
		{
		    // Make a copy, so that the iteration still works when the
		    // blob is changed.
		    fi->fi_blob = blob_copy(tv.vval.v_blob);
		}
		clear_tv(&tv);
	    }
	    ಅನ್ಯಥಾ ಯದಿ (tv.v_type == VAR_STRING)
	    {
		fi->fi_byte_idx = 0;
		fi->fi_string = tv.vval.v_string;
		tv.vval.v_string = ರಿಕ್ತ;
		ಯದಿ (fi->fi_string == ರಿಕ್ತ)
		    fi->fi_string = vim_strsave((char_u *)"");
	    }
	    ಅನ್ಯಥಾ
	    {
		emsg(_(e_string_list_tuple_or_blob_required));
		clear_tv(&tv);
	    }
	}
    }
    ಯದಿ (skip)
	--emsg_skip;
    fi->fi_break_ಎಣಿಕೆ = evalarg->eval_break_ಎಣಿಕೆ;

    ಪ್ರತ್ಯಾಗಮ fi;
}

/*
 * Used when looping over a :for line, skip the "in expr" part.
 */
    ಶೂನ್ಯ
skip_for_ಸಾಲುಗಳು(ಶೂನ್ಯ *fi_void, evalarg_T *evalarg)
{
    forinfo_T	*fi = (forinfo_T *)fi_void;
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಪ್ರತಿ (i = 0; i < fi->fi_break_ಎಣಿಕೆ; ++i)
	eval_next_ಸಾಲು(ರಿಕ್ತ, evalarg);
}

/*
 * Use the first item in a ":for" list.  Advance to the next.
 * Assign the values to the variable (list).  "arg" points to the first one.
 * Return TRUE when a valid item was found, FALSE when at end of list or
 * something wrong.
 */
    ಪೂರ್ಣಾಙ್ಕ
next_for_item(ಶೂನ್ಯ *fi_void, char_u *arg)
{
    forinfo_T	*fi = (forinfo_T *)fi_void;
    ಪೂರ್ಣಾಙ್ಕ		result;
    ಪೂರ್ಣಾಙ್ಕ		flag = ASSIGN_FOR_LOOP | (in_vim9script()
			 ? (ASSIGN_FINAL
			     // first round: error if variable exists
			     | (fi->fi_bi == 0 ? 0 : ASSIGN_DECL)
			     | ASSIGN_NO_MEMBER_TYPE
			     | ASSIGN_UPDATE_BLOCK_ID)
			 : 0);
    listitem_T	*item;
    ಪೂರ್ಣಾಙ್ಕ		skip_assign = in_vim9script() && arg[0] == '_'
						      && !eval_isnamec(arg[1]);

    ಯದಿ (fi->fi_blob != ರಿಕ್ತ)
    {
	typval_T	tv;

	ಯದಿ (fi->fi_bi >= blob_len(fi->fi_blob))
	    ಪ್ರತ್ಯಾಗಮ FALSE;
	tv.v_type = VAR_ಸಂಖ್ಯೆ;
	tv.v_lock = VAR_FIXED;
	tv.vval.v_ಸಂಖ್ಯೆ = blob_get(fi->fi_blob, fi->fi_bi);
	++fi->fi_bi;
	ಯದಿ (skip_assign)
	    ಪ್ರತ್ಯಾಗಮ TRUE;
	ಪ್ರತ್ಯಾಗಮ ex_let_vars(arg, &tv, TRUE, fi->fi_semicolon,
					    fi->fi_varcount, flag, ರಿಕ್ತ) == OK;
    }

    ಯದಿ (fi->fi_string != ರಿಕ್ತ)
    {
	typval_T	tv;
	ಪೂರ್ಣಾಙ್ಕ		len;

	len = mb_ptr2len(fi->fi_string + fi->fi_byte_idx);
	ಯದಿ (len == 0)
	    ಪ್ರತ್ಯಾಗಮ FALSE;
	tv.v_type = VAR_STRING;
	tv.v_lock = VAR_FIXED;
	tv.vval.v_string = vim_strnsave(fi->fi_string + fi->fi_byte_idx, len);
	fi->fi_byte_idx += len;
	++fi->fi_bi;
	ಯದಿ (skip_assign)
	    result = TRUE;
	ಅನ್ಯಥಾ
	    result = ex_let_vars(arg, &tv, TRUE, fi->fi_semicolon,
					    fi->fi_varcount, flag, ರಿಕ್ತ) == OK;
	vim_free(tv.vval.v_string);
	ಪ್ರತ್ಯಾಗಮ result;
    }

    ಯದಿ (fi->fi_tuple != ರಿಕ್ತ)
    {
	typval_T	tv;

	ಯದಿ (fi->fi_tuple_idx >= TUPLE_LEN(fi->fi_tuple))
	    ಪ್ರತ್ಯಾಗಮ FALSE;

	copy_tv(TUPLE_ITEM(fi->fi_tuple, fi->fi_tuple_idx), &tv);
	++fi->fi_tuple_idx;
	++fi->fi_bi;
	ಯದಿ (skip_assign)
	    result = TRUE;
	ಅನ್ಯಥಾ
	    result = ex_let_vars(arg, &tv, TRUE, fi->fi_semicolon,
					    fi->fi_varcount, flag, ರಿಕ್ತ) == OK;
	clear_tv(&tv);
	ಪ್ರತ್ಯಾಗಮ result;
    }

    item = fi->fi_lw.lw_item;
    ಯದಿ (item == ರಿಕ್ತ)
	result = FALSE;
    ಅನ್ಯಥಾ
    {
	fi->fi_lw.lw_item = item->li_next;
	++fi->fi_bi;
	ಯದಿ (skip_assign)
	    result = TRUE;
	ಅನ್ಯಥಾ
	    result = (ex_let_vars(arg, &item->li_tv, TRUE, fi->fi_semicolon,
					   fi->fi_varcount, flag, ರಿಕ್ತ) == OK);
    }
    ಪ್ರತ್ಯಾಗಮ result;
}

/*
 * Free the structure used to store info used by ":for".
 */
    ಶೂನ್ಯ
free_for_info(ಶೂನ್ಯ *fi_void)
{
    forinfo_T    *fi = (forinfo_T *)fi_void;

    ಯದಿ (fi == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (fi->fi_list != ರಿಕ್ತ)
    {
	list_rem_watch(fi->fi_list, &fi->fi_lw);
	list_unref(fi->fi_list);
    }
    ಅನ್ಯಥಾ ಯದಿ (fi->fi_blob != ರಿಕ್ತ)
	blob_unref(fi->fi_blob);
    ಅನ್ಯಥಾ ಯದಿ (fi->fi_tuple != ರಿಕ್ತ)
	tuple_unref(fi->fi_tuple);
    ಅನ್ಯಥಾ
	vim_free(fi->fi_string);
    vim_free(fi);
}

    ಶೂನ್ಯ
set_context_for_expression(
    expand_T	*xp,
    char_u	*arg,
    cmdidx_T	cmdidx)
{
    ಪೂರ್ಣಾಙ್ಕ		has_ಅಭಿವ್ಯಕ್ತಿ = cmdidx != CMD_let && cmdidx != CMD_var;
    ಪೂರ್ಣಾಙ್ಕ		c;
    char_u	*p;

    ಯದಿ (cmdidx == CMD_let || cmdidx == CMD_var
				 || cmdidx == CMD_const || cmdidx == CMD_final)
    {
	xp->xp_context = EXPAND_USER_VARS;
	ಯದಿ (vim_strpbrk(arg, (char_u *)"\"'+-*/%.=!?~|&$([<>,#") == ರಿಕ್ತ)
	{
	    // ":let var1 var2 ...": find last space.
	    ಪ್ರತಿ (p = arg + STRLEN(arg); p >= arg; )
	    {
		xp->xp_pattern = p;
		MB_PTR_BACK(arg, p);
		ಯದಿ (VIM_ISWHITE(*p))
		    ಭಙ್ಗ;
	    }
	    ಪ್ರತ್ಯಾಗಮ;
	}
    }
    ಅನ್ಯಥಾ
	xp->xp_context = cmdidx == CMD_call ? EXPAND_FUNCTIONS
							  : EXPAND_EXPRESSION;
    ಯಾವತ್ ((xp->xp_pattern = vim_strpbrk(arg,
				  (char_u *)"\"'+-*/%.=!?~|&$([<>,#")) != ರಿಕ್ತ)
    {
	c = *xp->xp_pattern;
	ಯದಿ (c == '&')
	{
	    c = xp->xp_pattern[1];
	    ಯದಿ (c == '&')
	    {
		++xp->xp_pattern;
		xp->xp_context = has_ಅಭಿವ್ಯಕ್ತಿ ? EXPAND_EXPRESSION : EXPAND_NOTHING;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (c != ' ')
	    {
		xp->xp_context = EXPAND_SETTINGS;
		ಯದಿ ((c == 'l' || c == 'g') && xp->xp_pattern[2] == ':')
		    xp->xp_pattern += 2;

	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (c == '$')
	{
	    // environment variable
	    xp->xp_context = EXPAND_ENV_VARS;
	}
	ಅನ್ಯಥಾ ಯದಿ (c == '=')
	{
	    has_ಅಭಿವ್ಯಕ್ತಿ = TRUE;
	    xp->xp_context = EXPAND_EXPRESSION;
	}
	ಅನ್ಯಥಾ ಯದಿ (c == '#'
		&& xp->xp_context == EXPAND_EXPRESSION)
	{
	    // Autoload function/variable contains '#'.
	    ಭಙ್ಗ;
	}
	ಅನ್ಯಥಾ ಯದಿ ((c == '<' || c == '#')
		&& xp->xp_context == EXPAND_FUNCTIONS
		&& vim_strchr(xp->xp_pattern, '(') == ರಿಕ್ತ)
	{
	    // Function name can start with "<SNR>" and contain '#'.
	    ಭಙ್ಗ;
	}
	ಅನ್ಯಥಾ ಯದಿ (has_ಅಭಿವ್ಯಕ್ತಿ)
	{
	    ಯದಿ (c == '"')	    // string
	    {
		ಯಾವತ್ ((c = *++xp->xp_pattern) != NUL && c != '"')
		    ಯದಿ (c == '\\' && xp->xp_pattern[1] != NUL)
			++xp->xp_pattern;
		xp->xp_context = EXPAND_NOTHING;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (c == '\'')	    // literal string
	    {
		// Trick: '' is like stopping and starting a literal string.
		ಯಾವತ್ ((c = *++xp->xp_pattern) != NUL && c != '\'')
		    /* skip */ ;
		xp->xp_context = EXPAND_NOTHING;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (c == '|')
	    {
		ಯದಿ (xp->xp_pattern[1] == '|')
		{
		    ++xp->xp_pattern;
		    xp->xp_context = EXPAND_EXPRESSION;
		}
		ಅನ್ಯಥಾ
		    xp->xp_context = EXPAND_COMMANDS;
	    }
	    ಅನ್ಯಥಾ
		xp->xp_context = EXPAND_EXPRESSION;
	}
	ಅನ್ಯಥಾ
	    // Doesn't look like something valid, expand as an expression
	    // anyway.
	    xp->xp_context = EXPAND_EXPRESSION;
	arg = xp->xp_pattern;
	ಯದಿ (*arg != NUL)
	    ಯಾವತ್ ((c = *++arg) != NUL && (c == ' ' || c == '\t'))
		/* skip */ ;
    }

    // ":exe one two" completes "two"
    ಯದಿ ((cmdidx == CMD_execute
		|| cmdidx == CMD_echo
		|| cmdidx == CMD_echon
		|| cmdidx == CMD_echomsg
		|| cmdidx == CMD_echoerr
		|| cmdidx == CMD_echoconsole
		|| cmdidx == CMD_echowindow)
	    && xp->xp_context == EXPAND_EXPRESSION)
    {
	ಪ್ರತಿ (;;)
	{
	    char_u *n = skiptowhite(arg);

	    ಯದಿ (n == arg || IS_WHITE_OR_NUL(*skipwhite(n)))
		ಭಙ್ಗ;
	    arg = skipwhite(n);
	}
    }

    xp->xp_pattern = arg;
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
ಸ್ಥಿರ regprog_T    *eval_prog_cache = ರಿಕ್ತ;
ಸ್ಥಿರ char_u	    *eval_prog_pat = ರಿಕ್ತ;	// pattern it was compiled for
ಸ್ಥಿರ char_u	    *eval_prog_enc = ರಿಕ್ತ;	// 'encoding' when compiled
ಸ್ಥಿರ long	    eval_prog_re;		// 'regexpengine' when compiled
// State sampled by eval_regcomp() for the eval_regfree() call that gives the
// program back: user code run in between, e.g. an object's string() method
// invoked by match() on a list, can change these options, and then the
// program must not be cached under the changed values.  eval_compile_enc is
// only ever compared against p_enc, never dereferenced.
ಸ್ಥಿರ long	    eval_compile_re;
ಸ್ಥಿರ char_u	    *eval_compile_enc;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	    eval_prog_busy = 0;	// programs eval_regcomp() handed out
					// that were not given back yet
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	    eval_prog_reentered = FALSE;  // eval_regcomp() ran while
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
    ಸ್ಥಿರ bool
eval_prog_volatile(char_u *pat)
{
    char_u *p;

    ಯದಿ (STRNCMP(pat, "\\%#=", 4) == 0)
	ಪ್ರತ್ಯಾಗಮ true;
    ಪ್ರತಿ (p = pat; *p != NUL; ++p)
	ಯದಿ (*p == '~'
		|| (p[0] == '[' && (p[1] == ':' || p[1] == '=' || p[1] == '.'))
		|| (p[0] == '%' && (p[1] == '.'
			|| ((p[1] == '<' || p[1] == '>') && p[2] == '.'))))
	    ಪ್ರತ್ಯಾಗಮ true;
    ಪ್ರತ್ಯಾಗಮ false;
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
    ಯದಿ (eval_prog_busy > 0)
	// The samples above no longer describe the program already handed
	// out: its eval_regfree() must drop it instead of caching it.
	eval_prog_reentered = TRUE;

    ಯದಿ (eval_prog_cache != ರಿಕ್ತ
	    && *p_cpo == NUL
	    && eval_prog_cache->re_flags == RE_MAGIC + RE_STRING
	    && eval_prog_re == p_re
	    && STRCMP(eval_prog_pat, pat) == 0
	    && STRCMP(eval_prog_enc, p_enc) == 0)
    {
	prog = eval_prog_cache;
	// The caller now owns the program, eval_regfree() adopts the
	// program the caller ends up with.
	eval_prog_cache = ರಿಕ್ತ;
    }
    ಅನ್ಯಥಾ
	prog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
    ಯದಿ (prog != ರಿಕ್ತ)
	++eval_prog_busy;	// paired with the eval_regfree() call
    ಪ್ರತ್ಯಾಗಮ prog;
}

/*
 * Free program "prog", obtained with eval_regcomp() for pattern "pat", by
 * keeping it in the cache for the next use.  "pat" must be the same string
 * the program was compiled for and must still be valid.
 */
    ಶೂನ್ಯ
eval_regfree(char_u *pat, regprog_T *prog)
{
    ಪೂರ್ಣಾಙ್ಕ	    reentered = eval_prog_reentered;

    ಯದಿ (eval_prog_busy > 0 && --eval_prog_busy == 0)
	eval_prog_reentered = FALSE;
    ಯದಿ (prog == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (reentered || *p_cpo != NUL
	    || eval_compile_re != p_re || eval_compile_enc != p_enc)
    {
	// 'cpoptions' is not empty, an option in the cache key changed, or
	// another program was handed out while the caller was using this
	// one: the samples may not describe this program, do not cache it.
	vim_regfree(prog);
	ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (eval_prog_pat == ರಿಕ್ತ || STRCMP(eval_prog_pat, pat) != 0
	    || STRCMP(eval_prog_enc, p_enc) != 0)
    {
	char_u	*pat_copy;
	char_u	*enc_copy;

	// A cached pattern is known not to be volatile, thus the scan is
	// only needed when installing a new key.
	ಯದಿ (eval_prog_volatile(pat))
	{
	    // compiling depends on state the cache key does not cover
	    vim_regfree(prog);
	    ಪ್ರತ್ಯಾಗಮ;
	}
	pat_copy = vim_strsave(pat);
	enc_copy = vim_strsave(p_enc);
	ಯದಿ (pat_copy == ರಿಕ್ತ || enc_copy == ರಿಕ್ತ)
	{
	    vim_free(pat_copy);
	    vim_free(enc_copy);
	    vim_regfree(prog);
	    ಪ್ರತ್ಯಾಗಮ;
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

#ಯದಿ defined(EXITFREE)
    ಶೂನ್ಯ
free_eval_regcomp_cache(ಶೂನ್ಯ)
{
    vim_regfree(eval_prog_cache);
    eval_prog_cache = ರಿಕ್ತ;
    VIM_CLEAR(eval_prog_pat);
    VIM_CLEAR(eval_prog_enc);
    eval_prog_busy = 0;
    eval_prog_reentered = FALSE;
}
#endif

/*
 * Return TRUE if "pat" matches "text".
 * Does not use 'cpo' and always uses 'magic'.
 */
    ಪೂರ್ಣಾಙ್ಕ
pattern_match(char_u *pat, char_u *text, ಪೂರ್ಣಾಙ್ಕ ic)
{
    ಪೂರ್ಣಾಙ್ಕ		matches = FALSE;
    char_u	*save_cpo;
    regmatch_T	regmatch;

    // avoid 'l' flag in 'cpoptions'
    save_cpo = p_cpo;
    p_cpo = empty_option;
    regmatch.regprog = eval_regcomp(pat);
    ಯದಿ (regmatch.regprog != ರಿಕ್ತ)
    {
	regmatch.rm_ic = ic;
	matches = vim_regexec_nl(&regmatch, text, (colnr_T)0);
	eval_regfree(pat, regmatch.regprog);
    }
    p_cpo = save_cpo;
    ಪ್ರತ್ಯಾಗಮ matches;
}

/*
 * Handle a name followed by "(".  Both for just "name(arg)" and for
 * "expr->name(arg)".
 * Returns OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
eval_func(
	char_u	    **arg,	// points to "(", will be advanced
	evalarg_T   *evalarg,
	char_u	    *name,
	ಪೂರ್ಣಾಙ್ಕ	    name_len,
	typval_T    *rettv,
	ಪೂರ್ಣಾಙ್ಕ	    flags,
	typval_T    *basetv)	// "expr" for "expr->name(arg)"
{
    ಪೂರ್ಣಾಙ್ಕ		evaluate = flags & EVAL_EVALUATE;
    char_u	*s = name;
    ಪೂರ್ಣಾಙ್ಕ		len = name_len;
    partial_T	*partial;
    ಪೂರ್ಣಾಙ್ಕ		ret = OK;
    type_T	*type = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		found_var = FALSE;

    ಯದಿ (!evaluate)
	check_vars(s, len);

    // If "s" is the name of a variable of type VAR_FUNC
    // use its contents.
    s = deref_func_name(s, &len, &partial,
		 in_vim9script() ? &type : ರಿಕ್ತ, !evaluate, FALSE, &found_var);

    // Need to make a copy, in case evaluating the arguments makes
    // the name invalid.
    s = vim_strsave(s);
    ಯದಿ (s == ರಿಕ್ತ || (evaluate && *s == NUL))
	ret = FAIL;
    ಅನ್ಯಥಾ
    {
	funcexe_T funcexe;

	// Invoke the function.
	CLEAR_FIELD(funcexe);
	funcexe.fe_firstline = curwin->w_cursor.lnum;
	funcexe.fe_lastline = curwin->w_cursor.lnum;
	funcexe.fe_evaluate = evaluate;
	funcexe.fe_partial = partial;
	ಯದಿ (partial != ರಿಕ್ತ)
	{
	    funcexe.fe_object = partial->pt_obj;
	    ಯದಿ (funcexe.fe_object != ರಿಕ್ತ)
		++funcexe.fe_object->obj_refcount;
	}
	funcexe.fe_basetv = basetv;
	funcexe.fe_check_type = type;
	funcexe.fe_found_var = found_var;
	ಯದಿ (evalarg != ರಿಕ್ತ)
	    funcexe.fe_cctx = evalarg->eval_cctx;
	ret = get_func_tv(s, len, rettv, arg, evalarg, &funcexe);
    }
    vim_free(s);

    // If evaluate is FALSE rettv->v_type was not set in
    // get_func_tv, but it's needed in handle_subscript() to parse
    // what follows. So set it here.
    ಯದಿ (rettv->v_type == VAR_UNKNOWN && !evaluate && **arg == '(')
    {
	rettv->vval.v_string = ರಿಕ್ತ;
	rettv->v_type = VAR_FUNC;
    }

    // Stop the expression evaluation when immediately
    // aborting on error, or when an interrupt occurred or
    // an exception was thrown but not caught.
    ಯದಿ (evaluate && aborting())
    {
	ಯದಿ (ret == OK)
	    clear_tv(rettv);
	ret = FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * After a NL, skip over empty lines and comment-only lines.
 */
    ಸ್ಥಿರ char_u *
newline_skip_comments(char_u *arg)
{
    char_u *p = arg + 1;

    ಪ್ರತಿ (;;)
    {
	p = skipwhite(p);

	ಯದಿ (*p == NUL)
	    ಭಙ್ಗ;
	ಯದಿ (vim9_comment_start(p))
	{
	    char_u *nl = vim_strchr(p, NL);

	    ಯದಿ (nl == ರಿಕ್ತ)
		ಭಙ್ಗ;
	    p = nl;
	}
	ಯದಿ (*p != NL)
	    ಭಙ್ಗ;
	++p;  // skip another NL
    }
    ಪ್ರತ್ಯಾಗಮ p;
}

/*
 * Get the next line source line without advancing.  But do skip over comment
 * lines.
 * Only called for Vim9 script.
 */
    ಸ್ಥಿರ char_u *
getline_peek_skip_comments(evalarg_T *evalarg)
{
    ಪ್ರತಿ (;;)
    {
	char_u *next = getline_peek(evalarg->eval_getline,
							 evalarg->eval_cookie);
	char_u *p;

	ಯದಿ (next == ರಿಕ್ತ)
	    ಭಙ್ಗ;
	p = skipwhite(next);
	ಯದಿ (*p != NUL && !vim9_comment_start(p))
	    ಪ್ರತ್ಯಾಗಮ next;
	ಯದಿ (eval_next_ಸಾಲು(ರಿಕ್ತ, evalarg) == ರಿಕ್ತ)
	    ಭಙ್ಗ;
    }
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
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
eval_next_non_blank(char_u *arg, evalarg_T *evalarg, ಪೂರ್ಣಾಙ್ಕ *getnext)
{
    char_u *p = skipwhite(arg);

    *getnext = FALSE;
    ಯದಿ (in_vim9script()
	    && evalarg != ರಿಕ್ತ
	    && (evalarg->eval_cookie != ರಿಕ್ತ || evalarg->eval_cctx != ರಿಕ್ತ
								   || *p == NL)
	    && (*p == NUL || *p == NL
			     || (vim9_comment_start(p) && VIM_ISWHITE(p[-1]))))
    {
	char_u *next;

	ಯದಿ (*p == NL)
	    next = newline_skip_comments(p);
	ಅನ್ಯಥಾ ಯದಿ (evalarg->eval_cookie != ರಿಕ್ತ)
	    next = getline_peek_skip_comments(evalarg);
	ಅನ್ಯಥಾ
	    next = peek_next_ಸಾಲು_from_context(evalarg->eval_cctx);

	ಯದಿ (next != ರಿಕ್ತ)
	{
	    *getnext = *p != NL;
	    ಪ್ರತ್ಯಾಗಮ skipwhite(next);
	}
    }
    ಪ್ರತ್ಯಾಗಮ p;
}

/*
 * To be called after eval_next_non_blank() sets "getnext" to TRUE.
 * Only called for Vim9 script.
 *
 * If "arg" is not NULL, then the caller should assign the return value to
 * "arg".
 */
    char_u *
eval_next_ಸಾಲು(char_u *arg, evalarg_T *evalarg)
{
    garray_T	*gap = &evalarg->eval_ga;
    char_u	*ಸಾಲು;

    ಯದಿ (arg != ರಿಕ್ತ)
    {
	ಯದಿ (*arg == NL)
	    ಪ್ರತ್ಯಾಗಮ newline_skip_comments(arg);
	// Truncate before a trailing comment, so that concatenating the lines
	// won't turn the rest into a comment.
	ಯದಿ (*skipwhite(arg) == '#')
	    *arg = NUL;
    }

    ಯದಿ (evalarg->eval_cookie != ರಿಕ್ತ)
	ಸಾಲು = evalarg->eval_getline(0, evalarg->eval_cookie, 0,
							   GETLINE_CONCAT_ALL);
    ಅನ್ಯಥಾ
	ಸಾಲು = next_ಸಾಲು_from_context(evalarg->eval_cctx, TRUE);
    ಯದಿ (ಸಾಲು == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ++evalarg->eval_break_ಎಣಿಕೆ;
    ಯದಿ (gap->ga_itemsize > 0 && ga_grow(gap, 1) == OK)
    {
	char_u *p = skipwhite(ಸಾಲು);

	// Going to concatenate the lines after parsing.  For an empty or
	// comment line use an empty string.
	ಯದಿ (*p == NUL || vim9_comment_start(p))
	{
	    vim_free(ಸಾಲು);
	    ಸಾಲು = vim_strsave((char_u *)"");
	}

	((char_u **)gap->ga_data)[gap->ga_len] = ಸಾಲು;
	++gap->ga_len;
    }
    ಅನ್ಯಥಾ ಯದಿ (evalarg->eval_cookie != ರಿಕ್ತ)
    {
	free_eval_tofree_later(evalarg);
	evalarg->eval_tofree = ಸಾಲು;
    }

    // Advanced to the next line, "arg" no longer points into the previous
    // line.  The caller assigns the return value to "arg".
    // If "arg" is NULL, then the return value is discarded.  In that case,
    // "arg" still points to the previous line.  So don't reset
    // "eval_using_cmdline".
    ಯದಿ (arg != ರಿಕ್ತ)
	evalarg->eval_using_cmdline = FALSE;
    ಪ್ರತ್ಯಾಗಮ skipwhite(ಸಾಲು);
}

/*
 * Call eval_next_non_blank() and get the next line if needed.
 */
    char_u *
skipwhite_and_linebreak(char_u *arg, evalarg_T *evalarg)
{
    ಪೂರ್ಣಾಙ್ಕ	    getnext;
    char_u  *p = skipwhite_and_nl(arg);

    ಯದಿ (evalarg == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ skipwhite(arg);
    eval_next_non_blank(p, evalarg, &getnext);
    ಯದಿ (getnext)
	ಪ್ರತ್ಯಾಗಮ eval_next_ಸಾಲು(arg, evalarg);
    ಪ್ರತ್ಯಾಗಮ p;
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
    ಪೂರ್ಣಾಙ್ಕ
eval0(
    char_u	*arg,
    typval_T	*rettv,
    exarg_T	*eap,
    evalarg_T	*evalarg)
{
    ಪ್ರತ್ಯಾಗಮ eval0_retarg(arg, rettv, eap, evalarg, ರಿಕ್ತ);
}

/*
 * If "arg" is a simple function call without arguments then call it and return
 * the result.  Otherwise return NOTDONE.
 */
    ಪೂರ್ಣಾಙ್ಕ
may_call_simple_func(
    char_u	*arg,
    typval_T	*rettv)
{
    char_u  *parens = (char_u *)strstr((ವರ್ಣ *)arg, "()");
    ಪೂರ್ಣಾಙ್ಕ	    r = NOTDONE;

    // If the expression is "FuncName()" then we can skip a lot of overhead.
    ಯದಿ (parens != ರಿಕ್ತ && *skipwhite(parens + 2) == NUL)
    {
	char_u *p = STRNCMP(arg, "<SNR>", 5) == 0 ? skipdigits(arg + 5) : arg;

	ಯದಿ (to_name_end(p, TRUE) == parens)
	    r = call_simple_func(arg, (size_t)(parens - arg), rettv);
    }
    ಪ್ರತ್ಯಾಗಮ r;
}

/*
 * Handle zero level expression with optimization for a simple function call.
 * Same arguments and return value as eval0().
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
eval0_simple_funccal(
    char_u	*arg,
    typval_T	*rettv,
    exarg_T	*eap,
    evalarg_T	*evalarg)
{
    ಪೂರ್ಣಾಙ್ಕ	    r = may_call_simple_func(arg, rettv);

    ಯದಿ (r == NOTDONE)
	r = eval0_retarg(arg, rettv, eap, evalarg, ರಿಕ್ತ);
    ಪ್ರತ್ಯಾಗಮ r;
}

/*
 * Like eval0() but when "retarg" is not NULL store the pointer to after the
 * expression and don't check what comes after the expression.
 */
    ಪೂರ್ಣಾಙ್ಕ
eval0_retarg(
    char_u	*arg,
    typval_T	*rettv,
    exarg_T	*eap,
    evalarg_T	*evalarg,
    char_u	**retarg)
{
    ಪೂರ್ಣಾಙ್ಕ		ret;
    char_u	*p;
    char_u	*ಅಭಿವ್ಯಕ್ತಿ_end;
    ಪೂರ್ಣಾಙ್ಕ		did_emsg_before = did_emsg;
    ಪೂರ್ಣಾಙ್ಕ		called_emsg_before = called_emsg;
    ಪೂರ್ಣಾಙ್ಕ		check_for_end = retarg == ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		end_error = FALSE;

    p = skipwhite(arg);
    ret = eval1(&p, rettv, evalarg);

    ಯದಿ (ret != FAIL)
    {
	ಅಭಿವ್ಯಕ್ತಿ_end = p;
	p = skipwhite(p);

	// In Vim9 script a command block is not split at NL characters for
	// commands using an expression argument.  Skip over a '#' comment to
	// check for a following NL.  Require white space before the '#'.
	ಯದಿ (in_vim9script() && p > ಅಭಿವ್ಯಕ್ತಿ_end && retarg == ರಿಕ್ತ)
	    ಯಾವತ್ (*p == '#')
	    {
		char_u *nl = vim_strchr(p, NL);

		ಯದಿ (nl == ರಿಕ್ತ)
		    ಭಙ್ಗ;
		p = skipwhite(nl + 1);
		ಯದಿ (eap != ರಿಕ್ತ && *p != NUL)
		    eap->nextcmd = p;
		check_for_end = FALSE;
	    }

	ಯದಿ (check_for_end)
	    end_error = !ends_excmd2(arg, p);
    }

    ಯದಿ (ret == FAIL || end_error)
    {
	ಯದಿ (ret != FAIL)
	    clear_tv(rettv);
	/*
	 * Report the invalid expression unless the expression evaluation has
	 * been cancelled due to an aborting error, an interrupt, or an
	 * exception, or we already gave a more specific error.
	 * Also check called_emsg for when using assert_fails().
	 */
	ಯದಿ (!aborting()
		&& did_emsg == did_emsg_before
		&& called_emsg == called_emsg_before
		&& (!in_vim9script() || !vim9_bad_comment(p)))
	{
	    ಯದಿ (end_error)
		semsg(_(e_trailing_characters_str), p);
	    ಅನ್ಯಥಾ
		semsg(_(e_invalid_expression_str), arg);
	}

	ಯದಿ (eap != ರಿಕ್ತ && p != ರಿಕ್ತ)
	{
	    // Some of the expression may not have been consumed.
	    // Only execute a next command if it cannot be a "||" operator.
	    // The next command may be "catch".
	    char_u *nextcmd = check_nextcmd(p);
	    ಯದಿ (nextcmd != ರಿಕ್ತ && *nextcmd != '|')
		eap->nextcmd = nextcmd;
	}
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಯದಿ (retarg != ರಿಕ್ತ)
	*retarg = p;
    ಅನ್ಯಥಾ ಯದಿ (check_for_end && eap != ರಿಕ್ತ)
	set_nextcmd(eap, p);

    ಪ್ರತ್ಯಾಗಮ ret;
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
    ಪೂರ್ಣಾಙ್ಕ
eval1(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    char_u  *p;
    ಪೂರ್ಣಾಙ್ಕ	    getnext;

    CLEAR_POINTER(rettv);

    /*
     * Get the first variable.
     */
    ಯದಿ (eval2(arg, rettv, evalarg) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    p = eval_next_non_blank(*arg, evalarg, &getnext);
    ಯದಿ (*p == '?')
    {
	ಪೂರ್ಣಾಙ್ಕ		op_falsy = p[1] == '?';
	ಪೂರ್ಣಾಙ್ಕ		result;
	typval_T	var2;
	evalarg_T	*evalarg_used = evalarg;
	evalarg_T	local_evalarg;
	ಪೂರ್ಣಾಙ್ಕ		orig_flags;
	ಪೂರ್ಣಾಙ್ಕ		evaluate;
	ಪೂರ್ಣಾಙ್ಕ		vim9script = in_vim9script();

	ಯದಿ (evalarg == ರಿಕ್ತ)
	{
	    init_evalarg(&local_evalarg);
	    evalarg_used = &local_evalarg;
	}
	orig_flags = evalarg_used->eval_flags;
	evaluate = evalarg_used->eval_flags & EVAL_EVALUATE;

	ಯದಿ (getnext)
	    *arg = eval_next_ಸಾಲು(*arg, evalarg_used);
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (evaluate && vim9script && !VIM_ISWHITE(p[-1]))
	    {
		error_white_both(p, op_falsy ? 2 : 1);
		clear_tv(rettv);
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    *arg = p;
	}

	result = FALSE;
	ಯದಿ (evaluate)
	{
	    ಪೂರ್ಣಾಙ್ಕ		error = FALSE;

	    ಯದಿ (op_falsy)
	    {
		// Is this typeval supported with the falsy operator?
		ಯದಿ (check_typval_is_ಮೌಲ್ಯ(rettv) == FAIL)
		{
		    clear_tv(rettv);
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}
		result = tv2bool(rettv);
	    }
	    ಅನ್ಯಥಾ ಯದಿ (vim9script)
		result = tv_get_bool_chk(rettv, &error);
	    ಅನ್ಯಥಾ ಯದಿ (tv_get_ಸಂಖ್ಯೆ_chk(rettv, &error) != 0)
		result = TRUE;
	    ಯದಿ (error || !op_falsy || !result)
		clear_tv(rettv);
	    ಯದಿ (error)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	}

	/*
	 * Get the second variable.  Recursive!
	 */
	ಯದಿ (op_falsy)
	    ++*arg;
	ಯದಿ (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[1]))
	{
	    error_white_both(*arg - (op_falsy ? 1 : 0), op_falsy ? 2 : 1);
	    clear_tv(rettv);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	*arg = skipwhite_and_linebreak(*arg + 1, evalarg_used);
	evalarg_used->eval_flags = (op_falsy ? !result : result)
				  ? orig_flags : (orig_flags & ~EVAL_EVALUATE);
	ಯದಿ (eval1(arg, &var2, evalarg_used) == FAIL)
	{
	    evalarg_used->eval_flags = orig_flags;
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	ಯದಿ (!op_falsy || !result)
	    *rettv = var2;

	ಯದಿ (!op_falsy)
	{
	    /*
	     * Check for the ":".
	     */
	    p = eval_next_non_blank(*arg, evalarg_used, &getnext);
	    ಯದಿ (*p != ':')
	    {
		emsg(_(e_missing_colon_after_questionmark));
		ಯದಿ (evaluate && result)
		    clear_tv(rettv);
		evalarg_used->eval_flags = orig_flags;
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    ಯದಿ (getnext)
		*arg = eval_next_ಸಾಲು(*arg, evalarg_used);
	    ಅನ್ಯಥಾ
	    {
		ಯದಿ (evaluate && vim9script && !VIM_ISWHITE(p[-1]))
		{
		    error_white_both(p, 1);
		    clear_tv(rettv);
		    evalarg_used->eval_flags = orig_flags;
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}
		*arg = p;
	    }

	    /*
	     * Get the third variable.  Recursive!
	     */
	    ಯದಿ (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[1]))
	    {
		error_white_both(*arg, 1);
		clear_tv(rettv);
		evalarg_used->eval_flags = orig_flags;
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    *arg = skipwhite_and_linebreak(*arg + 1, evalarg_used);
	    evalarg_used->eval_flags = !result ? orig_flags
					       : (orig_flags & ~EVAL_EVALUATE);
	    ಯದಿ (eval1(arg, &var2, evalarg_used) == FAIL)
	    {
		ಯದಿ (evaluate && result)
		    clear_tv(rettv);
		evalarg_used->eval_flags = orig_flags;
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    ಯದಿ (evaluate && !result)
		*rettv = var2;
	}

	ಯದಿ (evalarg == ರಿಕ್ತ)
	    clear_evalarg(&local_evalarg, ರಿಕ್ತ);
	ಅನ್ಯಥಾ
	    evalarg->eval_flags = orig_flags;
    }

    ಪ್ರತ್ಯಾಗಮ OK;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
eval2(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		getnext;

    /*
     * Get the first expression.
     */
    ಯದಿ (eval3(arg, rettv, evalarg) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    /*
     * Handle the  "||" operator.
     */
    p = eval_next_non_blank(*arg, evalarg, &getnext);
    ಯದಿ (p[0] == '|' && p[1] == '|')
    {
	evalarg_T   *evalarg_used = evalarg;
	evalarg_T   local_evalarg;
	ಪೂರ್ಣಾಙ್ಕ	    evaluate;
	ಪೂರ್ಣಾಙ್ಕ	    orig_flags;
	long	    result = FALSE;
	typval_T    var2;
	ಪೂರ್ಣಾಙ್ಕ	    error = FALSE;
	ಪೂರ್ಣಾಙ್ಕ	    vim9script = in_vim9script();

	ಯದಿ (evalarg == ರಿಕ್ತ)
	{
	    init_evalarg(&local_evalarg);
	    evalarg_used = &local_evalarg;
	}
	orig_flags = evalarg_used->eval_flags;
	evaluate = orig_flags & EVAL_EVALUATE;
	ಯದಿ (evaluate)
	{
	    ಯದಿ (vim9script)
		result = tv_get_bool_chk(rettv, &error);
	    ಅನ್ಯಥಾ ಯದಿ (tv_get_ಸಂಖ್ಯೆ_chk(rettv, &error) != 0)
		result = TRUE;
	    clear_tv(rettv);
	    ಯದಿ (error)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	}

	/*
	 * Repeat until there is no following "||".
	 */
	ಯಾವತ್ (p[0] == '|' && p[1] == '|')
	{
	    ಯದಿ (getnext)
		*arg = eval_next_ಸಾಲು(*arg, evalarg_used);
	    ಅನ್ಯಥಾ
	    {
		ಯದಿ (evaluate && vim9script && !VIM_ISWHITE(p[-1]))
		{
		    error_white_both(p, 2);
		    clear_tv(rettv);
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}
		*arg = p;
	    }

	    /*
	     * Get the second variable.
	     */
	    ಯದಿ (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[2]))
	    {
		error_white_both(*arg, 2);
		clear_tv(rettv);
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    *arg = skipwhite_and_linebreak(*arg + 2, evalarg_used);
	    evalarg_used->eval_flags = !result ? orig_flags
					       : (orig_flags & ~EVAL_EVALUATE);
	    ಯದಿ (eval3(arg, &var2, evalarg_used) == FAIL)
		ಪ್ರತ್ಯಾಗಮ FAIL;

	    /*
	     * Compute the result.
	     */
	    ಯದಿ (evaluate && !result)
	    {
		ಯದಿ (vim9script)
		    result = tv_get_bool_chk(&var2, &error);
		ಅನ್ಯಥಾ ಯದಿ (tv_get_ಸಂಖ್ಯೆ_chk(&var2, &error) != 0)
		    result = TRUE;
		clear_tv(&var2);
		ಯದಿ (error)
		    ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    ಯದಿ (evaluate)
	    {
		ಯದಿ (vim9script)
		{
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_ಸಂಖ್ಯೆ = result ? VVAL_TRUE : VVAL_FALSE;
		}
		ಅನ್ಯಥಾ
		{
		    rettv->v_type = VAR_ಸಂಖ್ಯೆ;
		    rettv->vval.v_ಸಂಖ್ಯೆ = result;
		}
	    }

	    p = eval_next_non_blank(*arg, evalarg_used, &getnext);
	}

	ಯದಿ (evalarg == ರಿಕ್ತ)
	    clear_evalarg(&local_evalarg, ರಿಕ್ತ);
	ಅನ್ಯಥಾ
	    evalarg->eval_flags = orig_flags;
    }

    ಪ್ರತ್ಯಾಗಮ OK;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
eval3(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		getnext;

    /*
     * Get the first expression.
     */
    ಯದಿ (eval4(arg, rettv, evalarg) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    /*
     * Handle the "&&" operator.
     */
    p = eval_next_non_blank(*arg, evalarg, &getnext);
    ಯದಿ (p[0] == '&' && p[1] == '&')
    {
	evalarg_T   *evalarg_used = evalarg;
	evalarg_T   local_evalarg;
	ಪೂರ್ಣಾಙ್ಕ	    orig_flags;
	ಪೂರ್ಣಾಙ್ಕ	    evaluate;
	long	    result = TRUE;
	typval_T    var2;
	ಪೂರ್ಣಾಙ್ಕ	    error = FALSE;
	ಪೂರ್ಣಾಙ್ಕ	    vim9script = in_vim9script();

	ಯದಿ (evalarg == ರಿಕ್ತ)
	{
	    init_evalarg(&local_evalarg);
	    evalarg_used = &local_evalarg;
	}
	orig_flags = evalarg_used->eval_flags;
	evaluate = orig_flags & EVAL_EVALUATE;
	ಯದಿ (evaluate)
	{
	    ಯದಿ (vim9script)
		result = tv_get_bool_chk(rettv, &error);
	    ಅನ್ಯಥಾ ಯದಿ (tv_get_ಸಂಖ್ಯೆ_chk(rettv, &error) == 0)
		result = FALSE;
	    clear_tv(rettv);
	    ಯದಿ (error)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	}

	/*
	 * Repeat until there is no following "&&".
	 */
	ಯಾವತ್ (p[0] == '&' && p[1] == '&')
	{
	    ಯದಿ (getnext)
		*arg = eval_next_ಸಾಲು(*arg, evalarg_used);
	    ಅನ್ಯಥಾ
	    {
		ಯದಿ (evaluate && vim9script && !VIM_ISWHITE(p[-1]))
		{
		    error_white_both(p, 2);
		    clear_tv(rettv);
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}
		*arg = p;
	    }

	    /*
	     * Get the second variable.
	     */
	    ಯದಿ (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[2]))
	    {
		error_white_both(*arg, 2);
		clear_tv(rettv);
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    *arg = skipwhite_and_linebreak(*arg + 2, evalarg_used);
	    evalarg_used->eval_flags = result ? orig_flags
					      : (orig_flags & ~EVAL_EVALUATE);
	    CLEAR_FIELD(var2);
	    ಯದಿ (eval4(arg, &var2, evalarg_used) == FAIL)
		ಪ್ರತ್ಯಾಗಮ FAIL;

	    /*
	     * Compute the result.
	     */
	    ಯದಿ (evaluate && result)
	    {
		ಯದಿ (vim9script)
		    result = tv_get_bool_chk(&var2, &error);
		ಅನ್ಯಥಾ ಯದಿ (tv_get_ಸಂಖ್ಯೆ_chk(&var2, &error) == 0)
		    result = FALSE;
		clear_tv(&var2);
		ಯದಿ (error)
		    ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    ಯದಿ (evaluate)
	    {
		ಯದಿ (vim9script)
		{
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_ಸಂಖ್ಯೆ = result ? VVAL_TRUE : VVAL_FALSE;
		}
		ಅನ್ಯಥಾ
		{
		    rettv->v_type = VAR_ಸಂಖ್ಯೆ;
		    rettv->vval.v_ಸಂಖ್ಯೆ = result;
		}
	    }

	    p = eval_next_non_blank(*arg, evalarg_used, &getnext);
	}

	ಯದಿ (evalarg == ರಿಕ್ತ)
	    clear_evalarg(&local_evalarg, ರಿಕ್ತ);
	ಅನ್ಯಥಾ
	    evalarg->eval_flags = orig_flags;
    }

    ಪ್ರತ್ಯಾಗಮ OK;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
eval4(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		getnext;
    exprtype_T	type = ಅಭಿವ್ಯಕ್ತಿ_UNKNOWN;
    ಪೂರ್ಣಾಙ್ಕ		len = 2;
    ಪೂರ್ಣಾಙ್ಕ		type_is = FALSE;

    /*
     * Get the first expression.
     */
    ಯದಿ (eval5(arg, rettv, evalarg) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    p = eval_next_non_blank(*arg, evalarg, &getnext);

    type = get_compare_type(p, &len, &type_is);

    /*
     * If there is a comparative operator, use it.
     */
    ಯದಿ (type != ಅಭಿವ್ಯಕ್ತಿ_UNKNOWN)
    {
	typval_T    var2;
	ಪೂರ್ಣಾಙ್ಕ	    ic;
	ಪೂರ್ಣಾಙ್ಕ	    vim9script = in_vim9script();
	ಪೂರ್ಣಾಙ್ಕ	    evaluate = evalarg == ರಿಕ್ತ
				   ? 0 : (evalarg->eval_flags & EVAL_EVALUATE);
	long	    comp_lnum = SOURCING_LNUM;

	ಯದಿ (getnext)
	{
	    *arg = eval_next_ಸಾಲು(*arg, evalarg);
	    p = *arg;
	}
	ಅನ್ಯಥಾ ಯದಿ (evaluate && vim9script && !VIM_ISWHITE(**arg))
	{
	    error_white_both(*arg, len);
	    clear_tv(rettv);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}

	ಯದಿ (vim9script && type_is && (p[len] == '?' || p[len] == '#'))
	{
	    semsg(_(e_invalid_expression_str), p);
	    clear_tv(rettv);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}

	// extra question mark appended: ignore case
	ಯದಿ (p[len] == '?')
	{
	    ic = TRUE;
	    ++len;
	}
	// extra '#' appended: match case
	ಅನ್ಯಥಾ ಯದಿ (p[len] == '#')
	{
	    ic = FALSE;
	    ++len;
	}
	// nothing appended: use 'ignorecase' if not in Vim script
	ಅನ್ಯಥಾ
	    ic = vim9script ? FALSE : p_ic;

	/*
	 * Get the second variable.
	 */
	ಯದಿ (evaluate && vim9script && !IS_WHITE_OR_NUL(p[len]))
	{
	    error_white_both(p, len);
	    clear_tv(rettv);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	*arg = skipwhite_and_linebreak(p + len, evalarg);
	ಯದಿ (eval5(arg, &var2, evalarg) == FAIL)
	{
	    clear_tv(rettv);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	ಯದಿ (evaluate)
	{
	    ಪೂರ್ಣಾಙ್ಕ ret;

	    // use the line of the comparison for messages
	    SOURCING_LNUM = comp_lnum;
	    ಯದಿ (vim9script && check_compare_types(type, rettv, &var2) == FAIL)
	    {
		ret = FAIL;
		clear_tv(rettv);
	    }
	    ಅನ್ಯಥಾ
		ret = typval_compare(rettv, &var2, type, ic);
	    clear_tv(&var2);
	    ಪ್ರತ್ಯಾಗಮ ret;
	}
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Make a copy of blob "tv1" and append blob "tv2".
 */
    ಶೂನ್ಯ
eval_addblob(typval_T *tv1, typval_T *tv2)
{
    blob_T  *b1 = tv1->vval.v_blob;
    blob_T  *b2 = tv2->vval.v_blob;
    blob_T  *b = blob_alloc();
    long    len1, len2;
    long    totallen;

    ಯದಿ (b == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    len1 = blob_len(b1);
    len2 = blob_len(b2);
    totallen = len1 + len2;

    ಯದಿ (totallen > 0 && totallen <= INT_MAX
				    && ga_grow(&b->bv_ga, (ಪೂರ್ಣಾಙ್ಕ)totallen) == OK)
    {
	ಯದಿ (len1 > 0)
	    mch_memmove((char_u *)b->bv_ga.ga_data,
		    (char_u *)b1->bv_ga.ga_data, (size_t)len1);
	ಯದಿ (len2 > 0)
	    mch_memmove((char_u *)b->bv_ga.ga_data + len1,
		    (char_u *)b2->bv_ga.ga_data, (size_t)len2);
	b->bv_ga.ga_len = (ಪೂರ್ಣಾಙ್ಕ)totallen;
    }

    clear_tv(tv1);
    rettv_blob_set(tv1, b);
}

/*
 * Make a copy of list "tv1" and append list "tv2".
 */
    ಪೂರ್ಣಾಙ್ಕ
eval_addlist(typval_T *tv1, typval_T *tv2)
{
    typval_T var3;

    // concatenate Lists
    ಯದಿ (list_concat(tv1->vval.v_list, tv2->vval.v_list, &var3) == FAIL)
    {
	clear_tv(tv1);
	clear_tv(tv2);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    clear_tv(tv1);
    *tv1 = var3;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Make a copy of tuple "tv1" and append tuple "tv2".
 */
    ಪೂರ್ಣಾಙ್ಕ
eval_addtuple(typval_T *tv1, typval_T *tv2)
{
    ಪೂರ್ಣಾಙ್ಕ		vim9script = in_vim9script();
    typval_T	var3;

    ಯದಿ (vim9script && tv1->vval.v_tuple != ರಿಕ್ತ && tv2->vval.v_tuple != ರಿಕ್ತ
	    && tv1->vval.v_tuple->tv_type != ರಿಕ್ತ
	    && tv2->vval.v_tuple->tv_type != ರಿಕ್ತ)
    {
	ಯದಿ (!check_tuples_addable(tv1->vval.v_tuple->tv_type,
						tv2->vval.v_tuple->tv_type))
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    // concatenate tuples
    ಯದಿ (tuple_concat(tv1->vval.v_tuple, tv2->vval.v_tuple, &var3) == FAIL)
    {
	clear_tv(tv1);
	clear_tv(tv2);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    clear_tv(tv1);
    *tv1 = var3;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Left or right shift the number "tv1" by the number "tv2" and store the
 * result in "tv1".
 *
 * Return OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
eval_shift_ಸಂಖ್ಯೆ(typval_T *tv1, typval_T *tv2, ಪೂರ್ಣಾಙ್ಕ shift_type)
{
    ಯದಿ (tv2->v_type != VAR_ಸಂಖ್ಯೆ || tv2->vval.v_ಸಂಖ್ಯೆ < 0)
    {
	// right operand should be a positive number
	ಯದಿ (tv2->v_type != VAR_ಸಂಖ್ಯೆ)
	    emsg(_(e_bitshift_ops_must_be_ಸಂಖ್ಯೆ));
	ಅನ್ಯಥಾ
	    emsg(_(e_bitshift_ops_must_be_positive));
	clear_tv(tv1);
	clear_tv(tv2);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಯದಿ (tv2->vval.v_ಸಂಖ್ಯೆ > MAX_LSHIFT_BITS)
	// shifting more bits than we have always results in zero
	tv1->vval.v_ಸಂಖ್ಯೆ = 0;
    ಅನ್ಯಥಾ ಯದಿ (shift_type == ಅಭಿವ್ಯಕ್ತಿ_LSHIFT)
	tv1->vval.v_ಸಂಖ್ಯೆ =
	    (uvarnumber_T)tv1->vval.v_ಸಂಖ್ಯೆ << tv2->vval.v_ಸಂಖ್ಯೆ;
    ಅನ್ಯಥಾ
	tv1->vval.v_ಸಂಖ್ಯೆ =
	    (uvarnumber_T)tv1->vval.v_ಸಂಖ್ಯೆ >> tv2->vval.v_ಸಂಖ್ಯೆ;

    ಪ್ರತ್ಯಾಗಮ OK;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
eval5(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    /*
     * Get the first expression.
     */
    ಯದಿ (eval6(arg, rettv, evalarg) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    /*
     * Repeat computing, until no '<<' or '>>' is following.
     */
    ಪ್ರತಿ (;;)
    {
	char_u		*p;
	ಪೂರ್ಣಾಙ್ಕ		getnext;
	exprtype_T	exprtype;
	ಪೂರ್ಣಾಙ್ಕ		evaluate;
	typval_T	var2;
	ಪೂರ್ಣಾಙ್ಕ		vim9script;

	p = eval_next_non_blank(*arg, evalarg, &getnext);
	ಯದಿ (p[0] == '<' && p[1] == '<')
	    exprtype = ಅಭಿವ್ಯಕ್ತಿ_LSHIFT;
	ಅನ್ಯಥಾ ಯದಿ (p[0] == '>' && p[1] == '>')
	    exprtype = ಅಭಿವ್ಯಕ್ತಿ_RSHIFT;
	ಅನ್ಯಥಾ
	    ಪ್ರತ್ಯಾಗಮ OK;

	// Handle a bitwise left or right shift operator
	evaluate = evalarg == ರಿಕ್ತ ? 0 : (evalarg->eval_flags & EVAL_EVALUATE);
	ಯದಿ (evaluate && rettv->v_type != VAR_ಸಂಖ್ಯೆ)
	{
	    // left operand should be a number
	    emsg(_(e_bitshift_ops_must_be_ಸಂಖ್ಯೆ));
	    clear_tv(rettv);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}

	vim9script = in_vim9script();
	ಯದಿ (getnext)
	{
	    *arg = eval_next_ಸಾಲು(*arg, evalarg);
	    p = *arg;
	}
	ಅನ್ಯಥಾ ಯದಿ (evaluate && vim9script && !VIM_ISWHITE(**arg))
	{
	    error_white_both(*arg, 2);
	    clear_tv(rettv);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}

	/*
	 * Get the second variable.
	 */
	ಯದಿ (evaluate && vim9script && !IS_WHITE_OR_NUL(p[2]))
	{
	    error_white_both(p, 2);
	    clear_tv(rettv);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	*arg = skipwhite_and_linebreak(p + 2, evalarg);
	ಯದಿ (eval6(arg, &var2, evalarg) == FAIL)
	{
	    clear_tv(rettv);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}

	ಯದಿ (evaluate)
	{
	    ಯದಿ (eval_shift_ಸಂಖ್ಯೆ(rettv, &var2, exprtype) == FAIL)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	}

	clear_tv(&var2);
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Concatenate strings "tv1" and "tv2" and store the result in "tv1".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
eval_concat_str(typval_T *tv1, typval_T *tv2)
{
    char_u	buf1[NUMBUFLEN], buf2[NUMBUFLEN];
    char_u	*s1 = tv_get_string_buf(tv1, buf1);
    char_u	*s2 = ರಿಕ್ತ;
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		vim9script = in_vim9script();

    ಯದಿ (vim9script && (tv2->v_type == VAR_VOID
		|| tv2->v_type == VAR_CHANNEL
		|| tv2->v_type == VAR_JOB))
	semsg(_(e_using_invalid_ಮೌಲ್ಯ_as_string_str),
		vartype_name(tv2->v_type));
    ಅನ್ಯಥಾ ಯದಿ (vim9script && tv2->v_type == VAR_FLOAT)
    {
	vim_snprintf((ವರ್ಣ *)buf2, NUMBUFLEN, "%g",
		tv2->vval.v_float);
	s2 = buf2;
    }
    ಅನ್ಯಥಾ
	s2 = tv_get_string_buf_chk(tv2, buf2);
    ಯದಿ (s2 == ರಿಕ್ತ)		// type error ?
    {
	clear_tv(tv1);
	clear_tv(tv2);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    // When possible, grow the existing string in place to avoid alloc/free.
    ಯದಿ (grow_string_tv(tv1, s2) == OK)
	ಪ್ರತ್ಯಾಗಮ OK;

    p = concat_str(s1, s2);
    clear_tv(tv1);
    tv1->v_type = VAR_STRING;
    tv1->vval.v_string = p;

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Add or subtract numbers "tv1" and "tv2" and store the result in "tv1".
 * The numbers can be whole numbers or floats.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
eval_addsub_ಸಂಖ್ಯೆ(typval_T *tv1, typval_T *tv2, ಪೂರ್ಣಾಙ್ಕ op)
{
    ಪೂರ್ಣಾಙ್ಕ		error = FALSE;
    varnumber_T	n1, n2;
    float_T	f1 = 0, f2 = 0;

    ಯದಿ (tv1->v_type == VAR_FLOAT)
    {
	f1 = tv1->vval.v_float;
	n1 = 0;
    }
    ಅನ್ಯಥಾ
    {
	n1 = tv_get_ಸಂಖ್ಯೆ_chk(tv1, &error);
	ಯದಿ (error)
	{
	    // This can only happen for "list + non-list" or
	    // "blob + non-blob".  For "non-list + ..." or
	    // "something - ...", we returned before evaluating the
	    // 2nd operand.
	    clear_tv(tv1);
	    clear_tv(tv2);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	ಯದಿ (tv2->v_type == VAR_FLOAT)
	    f1 = n1;
    }
    ಯದಿ (tv2->v_type == VAR_FLOAT)
    {
	f2 = tv2->vval.v_float;
	n2 = 0;
    }
    ಅನ್ಯಥಾ
    {
	n2 = tv_get_ಸಂಖ್ಯೆ_chk(tv2, &error);
	ಯದಿ (error)
	{
	    clear_tv(tv1);
	    clear_tv(tv2);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	ಯದಿ (tv1->v_type == VAR_FLOAT)
	    f2 = n2;
    }
    clear_tv(tv1);

    // If there is a float on either side the result is a float.
    ಯದಿ (tv1->v_type == VAR_FLOAT || tv2->v_type == VAR_FLOAT)
    {
	ಯದಿ (op == '+')
	    f1 = f1 + f2;
	ಅನ್ಯಥಾ
	    f1 = f1 - f2;
	tv1->v_type = VAR_FLOAT;
	tv1->vval.v_float = f1;
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (op == '+')
	    n1 = n1 + n2;
	ಅನ್ಯಥಾ
	    n1 = n1 - n2;
	tv1->v_type = VAR_ಸಂಖ್ಯೆ;
	tv1->vval.v_ಸಂಖ್ಯೆ = n1;
    }

    ಪ್ರತ್ಯಾಗಮ OK;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
eval6(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    /*
     * Get the first expression.
     */
    ಯದಿ (eval7(arg, rettv, evalarg, FALSE) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    /*
     * Repeat computing, until no '+', '-' or '.' is following.
     */
    ಪ್ರತಿ (;;)
    {
	ಪೂರ್ಣಾಙ್ಕ	    evaluate;
	ಪೂರ್ಣಾಙ್ಕ	    getnext;
	char_u	    *p;
	ಪೂರ್ಣಾಙ್ಕ	    op;
	ಪೂರ್ಣಾಙ್ಕ	    oplen;
	ಪೂರ್ಣಾಙ್ಕ	    concat;
	typval_T    var2;
	ಪೂರ್ಣಾಙ್ಕ	    vim9script = in_vim9script();
	long	    op_lnum = SOURCING_LNUM;

	// "." is only string concatenation when scriptversion is 1
	// "+=", "-=" and "..=" are assignments
	// "++" and "--" on the next line are a separate command.
	p = eval_next_non_blank(*arg, evalarg, &getnext);
	op = *p;
	concat = op == '.' && (*(p + 1) == '.' || in_old_script(2));
	ಯದಿ ((op != '+' && op != '-' && !concat) || p[1] == '='
					       || (p[1] == '.' && p[2] == '='))
	    ಭಙ್ಗ;
	ಯದಿ (getnext && (op == '+' || op == '-') && p[0] == p[1])
	    ಭಙ್ಗ;

	evaluate = evalarg == ರಿಕ್ತ ? 0 : (evalarg->eval_flags & EVAL_EVALUATE);
	oplen = (concat && p[1] == '.') ? 2 : 1;
	ಯದಿ (getnext)
	    *arg = eval_next_ಸಾಲು(*arg, evalarg);
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (evaluate && vim9script && !VIM_ISWHITE(**arg))
	    {
		error_white_both(*arg, oplen);
		clear_tv(rettv);
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    *arg = p;
	}
	ಯದಿ ((op != '+' || (rettv->v_type != VAR_LIST
						&& rettv->v_type != VAR_TUPLE
						&& rettv->v_type != VAR_BLOB))
		&& (op == '.' || rettv->v_type != VAR_FLOAT)
		&& evaluate)
	{
	    ಪೂರ್ಣಾಙ್ಕ		error = FALSE;

	    // For "list + ...", an illegal use of the first operand as
	    // a number cannot be determined before evaluating the 2nd
	    // operand: if this is also a list, all is ok.
	    // For "something . ...", "something - ..." or "non-list + ...",
	    // we know that the first operand needs to be a string or number
	    // without evaluating the 2nd operand.  So check before to avoid
	    // side effects after an error.
	    ಯದಿ (op != '.')
		tv_get_ಸಂಖ್ಯೆ_chk(rettv, &error);
	    ಯದಿ ((op == '.' && tv_get_string_chk(rettv) == ರಿಕ್ತ) || error)
	    {
		clear_tv(rettv);
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	}

	/*
	 * Get the second variable.
	 */
	ಯದಿ (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[oplen]))
	{
	    error_white_both(*arg, oplen);
	    clear_tv(rettv);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	*arg = skipwhite_and_linebreak(*arg + oplen, evalarg);
	ಯದಿ (eval7(arg, &var2, evalarg, !vim9script && op == '.') == FAIL)
	{
	    clear_tv(rettv);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}

	ಯದಿ (evaluate)
	{
	    /*
	     * Compute the result.
	     */
	    // use the line of the operation for messages
	    SOURCING_LNUM = op_lnum;
	    ಯದಿ (op == '.')
	    {
		ಯದಿ (eval_concat_str(rettv, &var2) == FAIL)
		    ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (op == '+' && rettv->v_type == VAR_BLOB
						   && var2.v_type == VAR_BLOB)
		eval_addblob(rettv, &var2);
	    ಅನ್ಯಥಾ ಯದಿ (op == '+' && rettv->v_type == VAR_LIST
						   && var2.v_type == VAR_LIST)
	    {
		ಯದಿ (eval_addlist(rettv, &var2) == FAIL)
		    ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (op == '+' && rettv->v_type == VAR_TUPLE
					   && var2.v_type == VAR_TUPLE)
	    {
		ಯದಿ (eval_addtuple(rettv, &var2) == FAIL)
		    ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    ಅನ್ಯಥಾ
	    {
		ಯದಿ (eval_addsub_ಸಂಖ್ಯೆ(rettv, &var2, op) == FAIL)
		    ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    clear_tv(&var2);
	}
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Multiply or divide or compute the modulo of numbers "tv1" and "tv2" and
 * store the result in "tv1".  The numbers can be whole numbers or floats.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
eval_multdiv_ಸಂಖ್ಯೆ(typval_T *tv1, typval_T *tv2, ಪೂರ್ಣಾಙ್ಕ op)
{
    varnumber_T	n1, n2;
    float_T	f1, f2;
    ಪೂರ್ಣಾಙ್ಕ		error;
    ಪೂರ್ಣಾಙ್ಕ		use_float = FALSE;

    f1 = 0;
    f2 = 0;
    error = FALSE;
    ಯದಿ (tv1->v_type == VAR_FLOAT)
    {
	f1 = tv1->vval.v_float;
	use_float = TRUE;
	n1 = 0;
    }
    ಅನ್ಯಥಾ
	n1 = tv_get_ಸಂಖ್ಯೆ_chk(tv1, &error);
    clear_tv(tv1);
    ಯದಿ (error)
    {
	clear_tv(tv2);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಯದಿ (tv2->v_type == VAR_FLOAT)
    {
	ಯದಿ (!use_float)
	{
	    f1 = n1;
	    use_float = TRUE;
	}
	f2 = tv2->vval.v_float;
	n2 = 0;
    }
    ಅನ್ಯಥಾ
    {
	n2 = tv_get_ಸಂಖ್ಯೆ_chk(tv2, &error);
	clear_tv(tv2);
	ಯದಿ (error)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	ಯದಿ (use_float)
	    f2 = n2;
    }

    /*
     * Compute the result.
     * When either side is a float the result is a float.
     */
    ಯದಿ (use_float)
    {
	ಯದಿ (op == '*')
	    f1 = f1 * f2;
	ಅನ್ಯಥಾ ಯದಿ (op == '/')
	{
#ifdef VMS
	    // VMS crashes on divide by zero, work around it
	    ಯದಿ (f2 == 0.0)
	    {
		ಯದಿ (f1 == 0)
		    f1 = -1 * __F_FLT_MAX - 1L;   // similar to NaN
		ಅನ್ಯಥಾ ಯದಿ (f1 < 0)
		    f1 = -1 * __F_FLT_MAX;
		ಅನ್ಯಥಾ
		    f1 = __F_FLT_MAX;
	    }
	    ಅನ್ಯಥಾ
		f1 = f1 / f2;
#ಅನ್ಯಥಾ
	    // We rely on the floating point library to handle divide
	    // by zero to result in "inf" and not a crash.
	    f1 = f1 / f2;
#endif
	}
	ಅನ್ಯಥಾ
	{
	    emsg(_(e_cannot_use_percent_with_float));
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	tv1->v_type = VAR_FLOAT;
	tv1->vval.v_float = f1;
    }
    ಅನ್ಯಥಾ
    {
	ಪೂರ್ಣಾಙ್ಕ	    failed = FALSE;

	ಯದಿ (op == '*')
	    n1 = n1 * n2;
	ಅನ್ಯಥಾ ಯದಿ (op == '/')
	    n1 = ಅಂಕೆ_divide(n1, n2, &failed);
	ಅನ್ಯಥಾ
	    n1 = ಅಂಕೆ_modulus(n1, n2, &failed);
	ಯದಿ (failed)
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	tv1->v_type = VAR_ಸಂಖ್ಯೆ;
	tv1->vval.v_ಸಂಖ್ಯೆ = n1;
    }

    ಪ್ರತ್ಯಾಗಮ OK;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
eval7(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    ಪೂರ್ಣಾಙ್ಕ		want_string)  // after "." operator
{
    /*
     * Get the first expression.
     */
    ಯದಿ (eval8(arg, rettv, evalarg, want_string) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    /*
     * Repeat computing, until no '*', '/' or '%' is following.
     */
    ಪ್ರತಿ (;;)
    {
	ಪೂರ್ಣಾಙ್ಕ	    evaluate;
	ಪೂರ್ಣಾಙ್ಕ	    getnext;
	typval_T    var2;
	char_u	    *p;
	ಪೂರ್ಣಾಙ್ಕ	    op;

	// "*=", "/=" and "%=" are assignments
	p = eval_next_non_blank(*arg, evalarg, &getnext);
	op = *p;
	ಯದಿ ((op != '*' && op != '/' && op != '%') || p[1] == '=')
	    ಭಙ್ಗ;

	evaluate = evalarg == ರಿಕ್ತ ? 0 : (evalarg->eval_flags & EVAL_EVALUATE);
	ಯದಿ (getnext)
	    *arg = eval_next_ಸಾಲು(*arg, evalarg);
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (evaluate && in_vim9script() && !VIM_ISWHITE(**arg))
	    {
		error_white_both(*arg, 1);
		clear_tv(rettv);
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    *arg = p;
	}

	/*
	 * Get the second variable.
	 */
	ಯದಿ (evaluate && in_vim9script() && !IS_WHITE_OR_NUL((*arg)[1]))
	{
	    error_white_both(*arg, 1);
	    clear_tv(rettv);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	*arg = skipwhite_and_linebreak(*arg + 1, evalarg);
	ಯದಿ (eval8(arg, &var2, evalarg, FALSE) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	ಯದಿ (evaluate)
	    // Compute the result.
	    ಯದಿ (eval_multdiv_ಸಂಖ್ಯೆ(rettv, &var2, op) == FAIL)
		ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Handle seventh level expression:
 *	a type cast before a base level expression.
 * "arg" must point to the first non-white of the expression.
 * "arg" is advanced to just after the recognized expression.
 * Return OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
eval8(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    ಪೂರ್ಣಾಙ್ಕ		want_string)	// after "." operator
{
    type_T	*want_type = ರಿಕ್ತ;
    garray_T	type_list;	    // list of pointers to allocated types
    ಪೂರ್ಣಾಙ್ಕ		res;
    ಪೂರ್ಣಾಙ್ಕ		evaluate = evalarg == ರಿಕ್ತ ? 0
				       : (evalarg->eval_flags & EVAL_EVALUATE);

    // Recognize <type> in Vim9 script only.
    ಯದಿ (in_vim9script() && **arg == '<' && eval_isnamec1((*arg)[1])
					     && STRNCMP(*arg, "<SNR>", 5) != 0)
    {
	++*arg;
	ga_init2(&type_list, ಪರಿಮಾಣ(type_T *), 10);
	ಯದಿ (!evaluate)
	    // Only skip over the type, it may use a type variable that is only
	    // known when the expression is compiled, e.g. in a lambda.
	    *arg = skip_type(*arg, FALSE);
	ಅನ್ಯಥಾ
	{
	    want_type = parse_type(arg, &type_list, ರಿಕ್ತ, ರಿಕ್ತ, TRUE);
	    ಯದಿ (want_type == ರಿಕ್ತ)
	    {
		clear_type_list(&type_list);
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	}

	ಯದಿ (**arg != '>')
	{
	    ಯದಿ (*skipwhite(*arg) == '>')
		semsg(_(e_no_white_space_allowed_before_str_str), ">", *arg);
	    ಅನ್ಯಥಾ
		emsg(_(e_missing_gt));
	    clear_type_list(&type_list);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	++*arg;
	*arg = skipwhite_and_linebreak(*arg, evalarg);
    }

    res = eval9(arg, rettv, evalarg, want_string);

    ಯದಿ (want_type != ರಿಕ್ತ)
    {
	ಯದಿ (evaluate && res == OK)
	{
	    type_T *actual = typval2type(rettv, get_copyID(), &type_list,
							       TVTT_DO_MEMBER);

	    ಯದಿ (!equal_type(want_type, actual, 0))
	    {
		ಯದಿ (want_type->tt_type == VAR_BOOL
					&& actual->tt_type != VAR_BOOL
					&& (actual->tt_flags & TTFLAG_BOOL_OK))
		{
		    ಪೂರ್ಣಾಙ್ಕ n = tv2bool(rettv);

		    // can use "0" and "1" for boolean in some places
		    clear_tv(rettv);
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_ಸಂಖ್ಯೆ = n ? VVAL_TRUE : VVAL_FALSE;
		}
		ಅನ್ಯಥಾ
		{
		    where_T where = WHERE_INIT;

		    res = check_type(want_type, actual, TRUE, where);
		}
	    }
	}
	clear_type_list(&type_list);
    }

    ಪ್ರತ್ಯಾಗಮ res;
}

    ಪೂರ್ಣಾಙ್ಕ
eval_leader(char_u **arg, ಪೂರ್ಣಾಙ್ಕ vim9)
{
    char_u	*s = *arg;
    char_u	*p = *arg;

    ಯಾವತ್ (*p == '!' || *p == '-' || *p == '+')
    {
	char_u *n = skipwhite(p + 1);

	// ++, --, -+ and +- are not accepted in Vim9 script
	ಯದಿ (vim9 && (*p == '-' || *p == '+') && (*n == '-' || *n == '+'))
	{
	    semsg(_(e_invalid_expression_str), s);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	p = n;
    }
    *arg = p;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Check for a predefined value "true", "false" and "null.*".
 * Return OK when recognized.
 */
    ಪೂರ್ಣಾಙ್ಕ
handle_predefined(char_u *s, ಪೂರ್ಣಾಙ್ಕ len, typval_T *rettv)
{
    ವಿಕಲ್ಪ (len)
    {
	ಪಕ್ಷ 4: ಯದಿ (STRNCMP(s, "true", 4) == 0)
		{
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_ಸಂಖ್ಯೆ = VVAL_TRUE;
		    ಪ್ರತ್ಯಾಗಮ OK;
		}
		ಯದಿ (STRNCMP(s, "null", 4) == 0)
		{
		    rettv->v_type = VAR_SPECIAL;
		    rettv->vval.v_ಸಂಖ್ಯೆ = VVAL_NULL;
		    ಪ್ರತ್ಯಾಗಮ OK;
		}
		ಭಙ್ಗ;
	ಪಕ್ಷ 5: ಯದಿ (STRNCMP(s, "false", 5) == 0)
		{
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_ಸಂಖ್ಯೆ = VVAL_FALSE;
		    ಪ್ರತ್ಯಾಗಮ OK;
		}
		ಭಙ್ಗ;
	ಪಕ್ಷ 8: ಯದಿ (STRNCMP(s, "null_job", 8) == 0)
		{
#ifdef FEAT_JOB_CHANNEL
		    rettv->v_type = VAR_JOB;
		    rettv->vval.v_job = ರಿಕ್ತ;
#ಅನ್ಯಥಾ
		    rettv->v_type = VAR_SPECIAL;
		    rettv->vval.v_ಸಂಖ್ಯೆ = VVAL_NULL;
#endif
		    ಪ್ರತ್ಯಾಗಮ OK;
		}
		ಭಙ್ಗ;
	ಪಕ್ಷ 9:
		ಯದಿ (STRNCMP(s, "null_", 5) != 0)
		    ಭಙ್ಗ;
		// null_list
		ಯದಿ (STRNCMP(s + 5, "list", 4) == 0)
		{
		    rettv->v_type = VAR_LIST;
		    rettv->vval.v_list = ರಿಕ್ತ;
		    ಪ್ರತ್ಯಾಗಮ OK;
		}
		// null_dict
		ಯದಿ (STRNCMP(s + 5, "dict", 4) == 0)
		{
		    rettv->v_type = VAR_DICT;
		    rettv->vval.v_dict = ರಿಕ್ತ;
		    ಪ್ರತ್ಯಾಗಮ OK;
		}
		// null_blob
		ಯದಿ (STRNCMP(s + 5, "blob", 4) == 0)
		{
		    rettv->v_type = VAR_BLOB;
		    rettv->vval.v_blob = ರಿಕ್ತ;
		    ಪ್ರತ್ಯಾಗಮ OK;
		}
		ಭಙ್ಗ;
	ಪಕ್ಷ 10:
		ಯದಿ (STRNCMP(s, "null_", 5) != 0)
		    ಭಙ್ಗ;
		// null_class
		ಯದಿ (STRNCMP(s + 5, "class", 5) == 0)
		{
		    rettv->v_type = VAR_CLASS;
		    rettv->vval.v_class = ರಿಕ್ತ;
		    ಪ್ರತ್ಯಾಗಮ OK;
		}
		ಯದಿ (STRNCMP(s + 5, "tuple", 5) == 0)
		{
		    rettv->v_type = VAR_TUPLE;
		    rettv->vval.v_tuple = ರಿಕ್ತ;
		    ಪ್ರತ್ಯಾಗಮ OK;
		}
		ಭಙ್ಗ;
	ಪಕ್ಷ 11: ಯದಿ (STRNCMP(s, "null_string", 11) == 0)
		{
		    rettv->v_type = VAR_STRING;
		    rettv->vval.v_string = ರಿಕ್ತ;
		    ಪ್ರತ್ಯಾಗಮ OK;
		}
		ಯದಿ (STRNCMP(s, "null_object", 11) == 0)
		{
		    rettv->v_type = VAR_OBJECT;
		    rettv->vval.v_object = ರಿಕ್ತ;
		    ಪ್ರತ್ಯಾಗಮ OK;
		}
		ಭಙ್ಗ;
	ಪಕ್ಷ 12:
		ಯದಿ (STRNCMP(s, "null_channel", 12) == 0)
		{
#ifdef FEAT_JOB_CHANNEL
		    rettv->v_type = VAR_CHANNEL;
		    rettv->vval.v_channel = ರಿಕ್ತ;
#ಅನ್ಯಥಾ
		    rettv->v_type = VAR_SPECIAL;
		    rettv->vval.v_ಸಂಖ್ಯೆ = VVAL_NULL;
#endif
		    ಪ್ರತ್ಯಾಗಮ OK;
		}
		ಯದಿ (STRNCMP(s, "null_partial", 12) == 0)
		{
		    rettv->v_type = VAR_PARTIAL;
		    rettv->vval.v_partial = ರಿಕ್ತ;
		    ಪ್ರತ್ಯಾಗಮ OK;
		}
		ಭಙ್ಗ;
	ಪಕ್ಷ 13: ಯದಿ (STRNCMP(s, "null_function", 13) == 0)
		{
		    rettv->v_type = VAR_FUNC;
		    rettv->vval.v_string = ರಿಕ್ತ;
		    ಪ್ರತ್ಯಾಗಮ OK;
		}
		ಭಙ್ಗ;
    }
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Handle register contents: @r.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
eval9_reg_contents(
    char_u	**arg,
    typval_T	*rettv,
    ಪೂರ್ಣಾಙ್ಕ		evaluate)
{
    ಪೂರ್ಣಾಙ್ಕ		vim9script = in_vim9script();

    ++*arg;	// skip '@'
    ಯದಿ (evaluate)
    {
	ಯದಿ (vim9script && IS_WHITE_OR_NUL(**arg))
	    semsg(_(e_syntax_error_at_str), *arg);
	ಅನ್ಯಥಾ ಯದಿ (vim9script && !valid_yank_reg(**arg, FALSE))
	    emsg_invreg(**arg);
	ಅನ್ಯಥಾ
	{
	    rettv->v_type = VAR_STRING;
	    rettv->vval.v_string = get_reg_contents(**arg,
		    GREG_ಅಭಿವ್ಯಕ್ತಿ_SRC);
	}
    }
    ಯದಿ (**arg != NUL)
	++*arg;
}

/*
 * Handle a nested expression: (expression) or lambda: (arg) => expr
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
eval9_nested_ಅಭಿವ್ಯಕ್ತಿ(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    ಪೂರ್ಣಾಙ್ಕ		evaluate)
{
    ಪೂರ್ಣಾಙ್ಕ		ret = NOTDONE;
    ಪೂರ್ಣಾಙ್ಕ		vim9script = in_vim9script();

    ಯದಿ (vim9script)
    {
	ret = get_lambda_tv(arg, rettv, TRUE, evalarg, ರಿಕ್ತ);
	ಯದಿ (ret == OK && evaluate)
	{
	    ufunc_T *ufunc = rettv->vval.v_partial->pt_func;

	    // Compile it here to get the return type.  The return
	    // type is optional, when it's missing use t_unknown.
	    // This is recognized in compile_return().
	    ಯದಿ (ufunc->uf_ret_type->tt_type == VAR_VOID)
		ufunc->uf_ret_type = &t_unknown;
	    ಯದಿ (compile_def_function(ufunc, FALSE,
				get_compile_type(ufunc), ರಿಕ್ತ) == FAIL)
	    {
		clear_tv(rettv);
		ret = FAIL;
	    }
	}
    }
    ಯದಿ (ret == NOTDONE)
    {
	*arg = skipwhite_and_linebreak(*arg + 1, evalarg);
	ಯದಿ (**arg == ')')
	    // empty tuple
	    ret = eval_tuple(arg, rettv, evalarg, TRUE);
	ಅನ್ಯಥಾ
	{
	    ret = eval1(arg, rettv, evalarg);	// recursive!
	    ಯದಿ (ret != OK)
		ಪ್ರತ್ಯಾಗಮ ret;

	    *arg = skipwhite_and_linebreak(*arg, evalarg);

	    ಯದಿ (**arg == ',')
		// tuple
		ret = eval_tuple(arg, rettv, evalarg, TRUE);
	    ಅನ್ಯಥಾ ಯದಿ (**arg == ')')
		++*arg;
	    ಅನ್ಯಥಾ ಯದಿ (ret == OK)
	    {
		emsg(_(e_missing_closing_paren));
		clear_tv(rettv);
		ret = FAIL;
	    }
	}
    }

    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
* Handle be a variable or function name.
* Can also be a curly-braces kind of name: {expr}.
*/
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
eval9_var_func_name(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    ಪೂರ್ಣಾಙ್ಕ		evaluate,
    char_u	**name_start)
{
    char_u	*s;
    ಪೂರ್ಣಾಙ್ಕ		len;
    char_u	*alias;
    ಪೂರ್ಣಾಙ್ಕ		ret = OK;
    ಪೂರ್ಣಾಙ್ಕ		vim9script = in_vim9script();

    s = *arg;
    len = get_name_len(arg, &alias, evaluate, TRUE);
    ಯದಿ (alias != ರಿಕ್ತ)
	s = alias;

    ಯದಿ (len <= 0)
	ret = FAIL;
    ಅನ್ಯಥಾ
    {
	ಪೂರ್ಣಾಙ್ಕ	flags = evalarg == ರಿಕ್ತ ? 0 : evalarg->eval_flags;

	ಯದಿ (evaluate && vim9script && len == 1 && *s == '_')
	{
	    emsg(_(e_cannot_use_underscore_here));
	    ret = FAIL;
	}
	ಅನ್ಯಥಾ ಯದಿ (evaluate && vim9script && len > 2
						&& s[0] == 's' && s[1] == ':')
	{
	    semsg(_(e_cannot_use_s_colon_in_vim9_script_str), s);
	    ret = FAIL;
	}
	ಅನ್ಯಥಾ ಯದಿ ((vim9script ? **arg : *skipwhite(*arg)) == '('
				|| (vim9script && generic_func_call(arg)))
	{
	    // "name(..."  recursive!
	    *arg = skipwhite(*arg);
	    ret = eval_func(arg, evalarg, s, len, rettv, flags, ರಿಕ್ತ);
	}
	ಅನ್ಯಥಾ ಯದಿ (evaluate)
	{
	    // get the value of "true", "false", etc. or a variable
	    ret = FAIL;
	    ಯದಿ (vim9script)
		ret = handle_predefined(s, len, rettv);
	    ಯದಿ (ret == FAIL)
	    {
		*name_start = s;
		ret = eval_variable(s, len, 0, rettv, ರಿಕ್ತ,
					EVAL_VAR_VERBOSE + EVAL_VAR_IMPORT);

		// skip the generic function arguments (if present)
		// they are already processed by eval_variable
		ಯದಿ (ret == OK && vim9script && **arg == '<'
						&& rettv->v_type == VAR_FUNC)
		    ret = skip_generic_func_type_args(arg);
	    }
	}
	ಅನ್ಯಥಾ
	{
	    // skip the name
	    check_vars(s, len);

	    // Skip the type arguments of a generic function, its name starts
	    // with an uppercase letter.
	    ಯದಿ (vim9script && **arg == '<' && ASCII_ISUPPER(*s))
		skip_generic_func_type_args(arg);
	    ret = OK;
	}
    }
    vim_free(alias);

    ಪ್ರತ್ಯಾಗಮ ret;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
eval9(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    ಪೂರ್ಣಾಙ್ಕ		want_string)	// after "." operator
{
    ಪೂರ್ಣಾಙ್ಕ		evaluate = evalarg != ರಿಕ್ತ
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    char_u	*name_start = ರಿಕ್ತ;
    char_u	*start_leader, *end_leader;
    ಪೂರ್ಣಾಙ್ಕ		ret = OK;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	recurse = 0;
    ಪೂರ್ಣಾಙ್ಕ		vim9script = in_vim9script();

    /*
     * Initialise variable so that clear_tv() can't mistake this for a
     * string and free a string that isn't there.
     */
    rettv->v_type = VAR_UNKNOWN;

    /*
     * Skip '!', '-' and '+' characters.  They are handled later.
     */
    start_leader = *arg;
    ಯದಿ (eval_leader(arg, vim9script) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    end_leader = *arg;

    ಯದಿ (**arg == '.' && (!SAFE_isdigit(*(*arg + 1)) || in_old_script(2)))
    {
	semsg(_(e_invalid_expression_str), *arg);
	++*arg;
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    // Limit recursion to 1000 levels.  At least at 10000 we run out of stack
    // and crash.  With MSVC the stack is smaller.
    ಯದಿ (recurse ==
#ifdef _MSC_VER
		    300
#ಅನ್ಯಥಾ
		    1000
#endif
		    )
    {
	semsg(_(e_expression_too_recursive_str), *arg);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ++recurse;

    ವಿಕಲ್ಪ (**arg)
    {
    /*
     * Number constant.
     */
    ಪಕ್ಷ '0':
    ಪಕ್ಷ '1':
    ಪಕ್ಷ '2':
    ಪಕ್ಷ '3':
    ಪಕ್ಷ '4':
    ಪಕ್ಷ '5':
    ಪಕ್ಷ '6':
    ಪಕ್ಷ '7':
    ಪಕ್ಷ '8':
    ಪಕ್ಷ '9':
    ಪಕ್ಷ '.':	ret = eval_ಸಂಖ್ಯೆ(arg, rettv, evaluate, want_string);

		// Apply prefixed "-" and "+" now.  Matters especially when
		// "->" follows.
		ಯದಿ (ret == OK && evaluate && end_leader > start_leader
						  && rettv->v_type != VAR_BLOB)
		    ret = eval9_leader(rettv, TRUE, start_leader, &end_leader);
		ಭಙ್ಗ;

    /*
     * String constant: "string".
     */
    ಪಕ್ಷ '"':	ret = eval_string(arg, rettv, evaluate, FALSE);
		ಭಙ್ಗ;

    /*
     * Literal string constant: 'str''ing'.
     */
    ಪಕ್ಷ '\'':	ret = eval_lit_string(arg, rettv, evaluate, FALSE);
		ಭಙ್ಗ;

    /*
     * List: [expr, expr]
     */
    ಪಕ್ಷ '[':	ret = eval_list(arg, rettv, evalarg, TRUE);
		ಭಙ್ಗ;

    /*
     * Literal Dictionary: #{key: val, key: val}
     */
    ಪಕ್ಷ '#':	ret = eval_lit_dict(arg, rettv, evalarg);
		ಭಙ್ಗ;

    /*
     * Lambda: {arg, arg -> expr}
     * Dictionary: {'key': val, 'key': val}
     */
    ಪಕ್ಷ '{':	ಯದಿ (vim9script)
		    ret = NOTDONE;
		ಅನ್ಯಥಾ
		    ret = get_lambda_tv(arg, rettv, vim9script, evalarg, ರಿಕ್ತ);
		ಯದಿ (ret == NOTDONE)
		    ret = eval_dict(arg, rettv, evalarg, FALSE);
		ಭಙ್ಗ;

    /*
     * Option value: &name
     */
    ಪಕ್ಷ '&':	ret = eval_option(arg, rettv, evaluate);
		ಭಙ್ಗ;

    /*
     * Environment variable: $VAR.
     * Interpolated string: $"string" or $'string'.
     */
    ಪಕ್ಷ '$':	ಯದಿ ((*arg)[1] == '"' || (*arg)[1] == '\'')
		    ret = eval_interp_string(arg, rettv, evaluate);
		ಅನ್ಯಥಾ
		    ret = eval_env_var(arg, rettv, evaluate);
		ಭಙ್ಗ;

    /*
     * Register contents: @r.
     */
    ಪಕ್ಷ '@':	eval9_reg_contents(arg, rettv, evaluate);
		ಭಙ್ಗ;

    /*
     * nested expression: (expression).
     * or lambda: (arg) => expr
     * or tuple
     */
    ಪಕ್ಷ '(':	ret = eval9_nested_ಅಭಿವ್ಯಕ್ತಿ(arg, rettv, evalarg, evaluate);
		ಭಙ್ಗ;

    ಸ್ವಭಾವ:	ret = NOTDONE;
		ಭಙ್ಗ;
    }

    ಯದಿ (ret == NOTDONE)
    {
	/*
	 * Must be a variable or function name.
	 * Can also be a curly-braces kind of name: {expr}.
	 */
	ret = eval9_var_func_name(arg, rettv, evalarg, evaluate, &name_start);
    }

    // Handle following '[', '(' and '.' for expr[expr], expr.name,
    // expr(expr), expr->name(expr)
    ಯದಿ (ret == OK)
	ret = handle_subscript(arg, name_start, rettv, evalarg, evaluate);

    /*
     * Apply logical NOT and unary '-', from right to left, ignore '+'.
     */
    ಯದಿ (ret == OK && evaluate && end_leader > start_leader)
	ret = eval9_leader(rettv, FALSE, start_leader, &end_leader);

    --recurse;
    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Apply the leading "!" and "-" before an eval9 expression to "rettv".
 * When "numeric_only" is TRUE only handle "+" and "-".
 * Adjusts "end_leaderp" until it is at "start_leader".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
eval9_leader(
	typval_T    *rettv,
	ಪೂರ್ಣಾಙ್ಕ	    numeric_only,
	char_u	    *start_leader,
	char_u	    **end_leaderp)
{
    char_u	*end_leader = *end_leaderp;
    ಪೂರ್ಣಾಙ್ಕ		ret = OK;
    ಪೂರ್ಣಾಙ್ಕ		error = FALSE;
    varnumber_T val = 0;
    vartype_T	type = rettv->v_type;
    ಪೂರ್ಣಾಙ್ಕ		vim9script = in_vim9script();
    float_T	    f = 0.0;

    ಯದಿ (rettv->v_type == VAR_FLOAT)
	f = rettv->vval.v_float;
    ಅನ್ಯಥಾ
    {
	ಯಾವತ್ (VIM_ISWHITE(end_leader[-1]))
	    --end_leader;
	ಯದಿ (vim9script && end_leader[-1] == '!')
	{
	    // Is this typeval supported with the ! operator?
	    ಯದಿ (check_typval_is_ಮೌಲ್ಯ(rettv) == FAIL)
	    {
		clear_tv(rettv);
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    val = tv2bool(rettv);
	}
	ಅನ್ಯಥಾ
	    val = tv_get_ಸಂಖ್ಯೆ_chk(rettv, &error);
    }
    ಯದಿ (error)
    {
	clear_tv(rettv);
	ret = FAIL;
    }
    ಅನ್ಯಥಾ
    {
	ಯಾವತ್ (end_leader > start_leader)
	{
	    --end_leader;
	    ಯದಿ (*end_leader == '!')
	    {
		ಯದಿ (numeric_only)
		{
		    ++end_leader;
		    ಭಙ್ಗ;
		}
		ಯದಿ (rettv->v_type == VAR_FLOAT)
		{
		    rettv->v_type = VAR_BOOL;
		    val = f == 0.0 ? VVAL_TRUE : VVAL_FALSE;
		    type = VAR_BOOL;
		}
		ಅನ್ಯಥಾ
		{
		    val = !val;
		    type = VAR_BOOL;
		}
	    }
	    ಅನ್ಯಥಾ ಯದಿ (*end_leader == '-')
	    {
		ಯದಿ (rettv->v_type == VAR_FLOAT)
		    f = -f;
		ಅನ್ಯಥಾ
		{
		    val = -val;
		    type = VAR_ಸಂಖ್ಯೆ;
		}
	    }
	}
	ಯದಿ (rettv->v_type == VAR_FLOAT)
	{
	    clear_tv(rettv);
	    rettv->vval.v_float = f;
	}
	ಅನ್ಯಥಾ
	{
	    clear_tv(rettv);
	    ಯದಿ (vim9script)
		rettv->v_type = type;
	    ಅನ್ಯಥಾ
		rettv->v_type = VAR_ಸಂಖ್ಯೆ;
	    rettv->vval.v_ಸಂಖ್ಯೆ = val;
	}
    }
    *end_leaderp = end_leader;
    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Call the function referred to in "rettv".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
call_func_rettv(
	char_u	    **arg,
	evalarg_T   *evalarg,
	typval_T    *rettv,
	ಪೂರ್ಣಾಙ್ಕ	    evaluate,
	dict_T	    *selfdict,
	typval_T    *basetv)
{
    partial_T	*pt = ರಿಕ್ತ;
    funcexe_T	funcexe;
    typval_T	functv;
    char_u	*s;
    ಪೂರ್ಣಾಙ್ಕ		ret;

    // need to copy the funcref so that we can clear rettv
    ಯದಿ (evaluate)
    {
	functv = *rettv;
	rettv->v_type = VAR_UNKNOWN;

	// Invoke the function.  Recursive!
	ಯದಿ (functv.v_type == VAR_PARTIAL)
	{
	    pt = functv.vval.v_partial;
	    s = partial_name(pt);
	}
	ಅನ್ಯಥಾ
	{
	    s = functv.vval.v_string;
	    ಯದಿ (s == ರಿಕ್ತ || *s == NUL)
	    {
		emsg(_(e_empty_function_name));
		ret = FAIL;
		ಗಚ್ಛ theend;
	    }
	}
    }
    ಅನ್ಯಥಾ
	s = (char_u *)"";

    CLEAR_FIELD(funcexe);
    funcexe.fe_firstline = curwin->w_cursor.lnum;
    funcexe.fe_lastline = curwin->w_cursor.lnum;
    funcexe.fe_evaluate = evaluate;
    funcexe.fe_partial = pt;
    ಯದಿ (pt != ರಿಕ್ತ)
    {
	funcexe.fe_object = pt->pt_obj;
	ಯದಿ (funcexe.fe_object != ರಿಕ್ತ)
	    ++funcexe.fe_object->obj_refcount;
    }
    funcexe.fe_selfdict = selfdict;
    funcexe.fe_basetv = basetv;
    ಯದಿ (evalarg != ರಿಕ್ತ)
	funcexe.fe_cctx = evalarg->eval_cctx;
    ret = get_func_tv(s, -1, rettv, arg, evalarg, &funcexe);

theend:
    // Clear the funcref afterwards, so that deleting it while
    // evaluating the arguments is possible (see test55).
    ಯದಿ (evaluate)
	clear_tv(&functv);

    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Evaluate "->method()".
 * "*arg" points to "method".
 * Returns FAIL or OK. "*arg" is advanced to after the ')'.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
eval_lambda(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    ಪೂರ್ಣಾಙ್ಕ		verbose)	// give error messages
{
    ಪೂರ್ಣಾಙ್ಕ		evaluate = evalarg != ರಿಕ್ತ
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    typval_T	base = *rettv;
    ಪೂರ್ಣಾಙ್ಕ		ret;

    rettv->v_type = VAR_UNKNOWN;

    ಯದಿ (**arg == '{')
    {
	// ->{lambda}()
	ret = get_lambda_tv(arg, rettv, FALSE, evalarg, ರಿಕ್ತ);
    }
    ಅನ್ಯಥಾ
    {
	// ->(lambda)()
	++*arg;
	ret = eval1(arg, rettv, evalarg);
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	ಯದಿ (**arg != ')')
	{
	    emsg(_(e_missing_closing_paren));
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	ಯದಿ (rettv->v_type != VAR_STRING && rettv->v_type != VAR_FUNC
					       && rettv->v_type != VAR_PARTIAL)
	{
	    emsg(_(e_string_or_function_required_for_arrow_parens_ಅಭಿವ್ಯಕ್ತಿ));
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	++*arg;
    }
    ಯದಿ (ret != OK)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (**arg != '(')
    {
	ಯದಿ (verbose)
	{
	    ಯದಿ (*skipwhite(*arg) == '(')
		emsg(_(e_no_white_space_allowed_before_parenthesis));
	    ಅನ್ಯಥಾ
		semsg(_(e_missing_parenthesis_str), "lambda");
	}
	clear_tv(rettv);
	ret = FAIL;
    }
    ಅನ್ಯಥಾ
	ret = call_func_rettv(arg, evalarg, rettv, evaluate, ರಿಕ್ತ, &base);

    // Clear the funcref afterwards, so that deleting it while
    // evaluating the arguments is possible (see test55).
    ಯದಿ (evaluate)
	clear_tv(&base);

    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Evaluate "->method()".
 * "*arg" points to "method".
 * Returns FAIL or OK. "*arg" is advanced to after the ')'.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
eval_method(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    ಪೂರ್ಣಾಙ್ಕ		verbose)	// give error messages
{
    char_u	*name;
    long	len;
    char_u	*alias;
    char_u	*tofree = ರಿಕ್ತ;
    typval_T	base = *rettv;
    ಪೂರ್ಣಾಙ್ಕ		ret = OK;
    ಪೂರ್ಣಾಙ್ಕ		evaluate = evalarg != ರಿಕ್ತ
				      && (evalarg->eval_flags & EVAL_EVALUATE);

    rettv->v_type = VAR_UNKNOWN;

    name = *arg;
    len = get_name_len(arg, &alias, evaluate, evaluate);
    ಯದಿ (alias != ರಿಕ್ತ)
	name = alias;

    ಯದಿ (len <= 0)
    {
	ಯದಿ (verbose)
	    emsg(_(e_missing_name_after_method));
	ret = FAIL;
    }
    ಅನ್ಯಥಾ
    {
	char_u *paren;

	// If there is no "(" immediately following, but there is further on,
	// it can be "import.Func()", "dict.Func()", "list[nr]", etc.
	// Does not handle anything where "(" is part of the expression.
	*arg = skipwhite(*arg);

	ಯದಿ (**arg != '(' && alias == ರಿಕ್ತ
				    && (paren = vim_strchr(*arg, '(')) != ರಿಕ್ತ)
	{
	    *arg = name;

	    // Truncate the name at the "(".  Avoid trying to get another line
	    // by making "getline" NULL.
	    *paren = NUL;
	    char_u	*(*getline)(ಪೂರ್ಣಾಙ್ಕ, ಶೂನ್ಯ *, ಪೂರ್ಣಾಙ್ಕ, getline_opt_T) = ರಿಕ್ತ;
	    ಯದಿ (evalarg != ರಿಕ್ತ)
	    {
		getline = evalarg->eval_getline;
		evalarg->eval_getline = ರಿಕ್ತ;
	    }

	    char_u *deref = deref_function_name(arg, &tofree, evalarg, verbose);
	    ಯದಿ (deref == ರಿಕ್ತ)
	    {
		*arg = name + len;
		ret = FAIL;
	    }
	    ಅನ್ಯಥಾ
	    {
		name = deref;
		len = (long)STRLEN(name);
	    }

	    *paren = '(';
	    ಯದಿ (getline != ರಿಕ್ತ)
		evalarg->eval_getline = getline;
	}

	ಯದಿ (ret == OK)
	{
	    *arg = skipwhite(*arg);

	    ಯದಿ (**arg != '(')
	    {
		ಯದಿ (verbose)
		    semsg(_(e_missing_parenthesis_str), name);
		ret = FAIL;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (VIM_ISWHITE((*arg)[-1]))
	    {
		ಯದಿ (verbose)
		    emsg(_(e_no_white_space_allowed_before_parenthesis));
		ret = FAIL;
	    }
	    ಅನ್ಯಥಾ
		ret = eval_func(arg, evalarg, name, len, rettv,
					  evaluate ? EVAL_EVALUATE : 0, &base);
	}
    }

    // Clear the funcref afterwards, so that deleting it while
    // evaluating the arguments is possible (see test55).
    ಯದಿ (evaluate)
	clear_tv(&base);
    vim_free(tofree);

    ಯದಿ (alias != ರಿಕ್ತ)
	vim_free(alias);

    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Evaluate an "[expr]" or "[expr:expr]" index.  Also "dict.key".
 * "*arg" points to the '[' or '.'.
 * Returns FAIL or OK. "*arg" is advanced to after the ']'.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
eval_index(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    ಪೂರ್ಣಾಙ್ಕ		verbose)	// give error messages
{
    ಪೂರ್ಣಾಙ್ಕ		evaluate = evalarg != ರಿಕ್ತ
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    ಪೂರ್ಣಾಙ್ಕ		empty1 = FALSE, empty2 = FALSE;
    typval_T	var1, var2;
    ಪೂರ್ಣಾಙ್ಕ		range = FALSE;
    char_u	*ಕೀಲಿ = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		keylen = -1;
    ಪೂರ್ಣಾಙ್ಕ		vim9script = in_vim9script();

    ಯದಿ (check_can_index(rettv, evaluate, verbose) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    init_tv(&var1);
    init_tv(&var2);
    ಯದಿ (**arg == '.')
    {
	/*
	 * dict.name
	 */
	ಕೀಲಿ = *arg + 1;
	ಪ್ರತಿ (keylen = 0; eval_isdictc(ಕೀಲಿ[keylen]); ++keylen)
	    ;
	ಯದಿ (keylen == 0)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	ಯದಿ (vim9script && ಕೀಲಿ[keylen] == '<')
	{
	    // skip generic type arguments
	    char_u	*p = &ಕೀಲಿ[keylen];

	    ಯದಿ (skip_generic_func_type_args(&p) == FAIL)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    keylen = p - ಕೀಲಿ;
	}
	*arg = ಕೀಲಿ + keylen;
    }
    ಅನ್ಯಥಾ
    {
	/*
	 * something[idx]
	 *
	 * Get the (first) variable from inside the [].
	 */
	*arg = skipwhite_and_linebreak(*arg + 1, evalarg);
	ಯದಿ (**arg == ':')
	    empty1 = TRUE;
	ಅನ್ಯಥಾ ಯದಿ (eval1(arg, &var1, evalarg) == FAIL)	// recursive!
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	ಅನ್ಯಥಾ ಯದಿ (vim9script && **arg == ':')
	{
	    semsg(_(e_white_space_required_before_and_after_str_at_str),
								    ":", *arg);
	    clear_tv(&var1);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	ಅನ್ಯಥಾ ಯದಿ (evaluate)
	{
	    ಪೂರ್ಣಾಙ್ಕ error = FALSE;

	    // allow for indexing with float
	    ಯದಿ (vim9script && rettv->v_type == VAR_DICT
						   && var1.v_type == VAR_FLOAT)
	    {
		var1.vval.v_string = typval_tostring(&var1, TRUE);
		var1.v_type = VAR_STRING;
	    }

	    ಯದಿ (vim9script && (rettv->v_type == VAR_LIST
						|| rettv->v_type == VAR_TUPLE))
		tv_get_ಸಂಖ್ಯೆ_chk(&var1, &error);
	    ಅನ್ಯಥಾ
		error = tv_get_string_chk(&var1) == ರಿಕ್ತ;
	    ಯದಿ (error)
	    {
		// not a number or string
		clear_tv(&var1);
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	}

	/*
	 * Get the second variable from inside the [:].
	 */
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	ಯದಿ (**arg == ':')
	{
	    range = TRUE;
	    ++*arg;
	    ಯದಿ (vim9script && !IS_WHITE_OR_NUL(**arg) && **arg != ']')
	    {
		semsg(_(e_white_space_required_before_and_after_str_at_str),
								":", *arg - 1);
		ಯದಿ (!empty1)
		    clear_tv(&var1);
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    *arg = skipwhite_and_linebreak(*arg, evalarg);
	    ಯದಿ (**arg == ']')
		empty2 = TRUE;
	    ಅನ್ಯಥಾ ಯದಿ (eval1(arg, &var2, evalarg) == FAIL)	// recursive!
	    {
		ಯದಿ (!empty1)
		    clear_tv(&var1);
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (evaluate && tv_get_string_chk(&var2) == ರಿಕ್ತ)
	    {
		// not a number or string
		ಯದಿ (!empty1)
		    clear_tv(&var1);
		clear_tv(&var2);
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	}

	// Check for the ']'.
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	ಯದಿ (**arg != ']')
	{
	    ಯದಿ (verbose)
		emsg(_(e_missing_closing_square_brace));
	    clear_tv(&var1);
	    ಯದಿ (range)
		clear_tv(&var2);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	*arg = *arg + 1;	// skip over the ']'
    }

    ಯದಿ (evaluate)
    {
	ಪೂರ್ಣಾಙ್ಕ res = eval_index_inner(rettv, range,
		empty1 ? ರಿಕ್ತ : &var1, empty2 ? ರಿಕ್ತ : &var2, FALSE,
		ಕೀಲಿ, keylen, verbose);

	ಯದಿ (!empty1)
	    clear_tv(&var1);
	ಯದಿ (range)
	    clear_tv(&var2);
	ಪ್ರತ್ಯಾಗಮ res;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Check if "rettv" can have an [index] or [sli:ce]
 */
    ಪೂರ್ಣಾಙ್ಕ
check_can_index(typval_T *rettv, ಪೂರ್ಣಾಙ್ಕ evaluate, ಪೂರ್ಣಾಙ್ಕ verbose)
{
    ವಿಕಲ್ಪ (rettv->v_type)
    {
	ಪಕ್ಷ VAR_FUNC:
	ಪಕ್ಷ VAR_PARTIAL:
	    ಯದಿ (verbose)
		emsg(_(e_cannot_index_a_funcref));
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	ಪಕ್ಷ VAR_FLOAT:
	    ಯದಿ (verbose)
		emsg(_(e_using_float_as_string));
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	ಪಕ್ಷ VAR_BOOL:
	ಪಕ್ಷ VAR_SPECIAL:
	ಪಕ್ಷ VAR_JOB:
	ಪಕ್ಷ VAR_CHANNEL:
	ಪಕ್ಷ VAR_INSTR:
	ಪಕ್ಷ VAR_OBJECT:
	    ಯದಿ (verbose)
		emsg(_(e_cannot_index_special_variable));
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	ಪಕ್ಷ VAR_CLASS:
	ಪಕ್ಷ VAR_TYPEALIAS:
	    ಯದಿ (verbose)
		check_typval_is_ಮೌಲ್ಯ(rettv);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	ಪಕ್ಷ VAR_UNKNOWN:
	ಪಕ್ಷ VAR_ANY:
	ಪಕ್ಷ VAR_VOID:
	    ಯದಿ (evaluate)
	    {
		emsg(_(e_cannot_index_special_variable));
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    // FALLTHROUGH

	ಪಕ್ಷ VAR_STRING:
	ಪಕ್ಷ VAR_LIST:
	ಪಕ್ಷ VAR_TUPLE:
	ಪಕ್ಷ VAR_DICT:
	ಪಕ್ಷ VAR_BLOB:
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_ಸಂಖ್ಯೆ:
	    ಯದಿ (in_vim9script())
		emsg(_(e_cannot_index_ಸಂಖ್ಯೆ));
	    ಭಙ್ಗ;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Apply index or range to "rettv".
 * "var1" is the first index, NULL for [:expr].
 * "var2" is the second index, NULL for [expr] and [expr: ]
 * "exclusive" is TRUE for slice(): second index is exclusive, use character
 * index for string.
 * Alternatively, "key" is not NULL, then key[keylen] is the dict index.
 */
    ಪೂರ್ಣಾಙ್ಕ
eval_index_inner(
	typval_T    *rettv,
	ಪೂರ್ಣಾಙ್ಕ	    is_range,
	typval_T    *var1,
	typval_T    *var2,
	ಪೂರ್ಣಾಙ್ಕ	    exclusive,
	char_u	    *ಕೀಲಿ,
	ಪೂರ್ಣಾಙ್ಕ	    keylen,
	ಪೂರ್ಣಾಙ್ಕ	    verbose)
{
    varnumber_T	    n1, n2 = 0;
    long	    len;

    n1 = 0;
    ಯದಿ (var1 != ರಿಕ್ತ && rettv->v_type != VAR_DICT)
	n1 = tv_get_ಸಂಖ್ಯೆ(var1);

    ಯದಿ (is_range)
    {
	ಯದಿ (rettv->v_type == VAR_DICT)
	{
	    ಯದಿ (verbose)
		emsg(_(e_cannot_slice_dictionary));
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	ಯದಿ (var2 != ರಿಕ್ತ)
	    n2 = tv_get_ಸಂಖ್ಯೆ(var2);
	ಅನ್ಯಥಾ
	    n2 = VARNUM_MAX;
    }

    ವಿಕಲ್ಪ (rettv->v_type)
    {
	ಪಕ್ಷ VAR_UNKNOWN:
	ಪಕ್ಷ VAR_ANY:
	ಪಕ್ಷ VAR_VOID:
	ಪಕ್ಷ VAR_FUNC:
	ಪಕ್ಷ VAR_PARTIAL:
	ಪಕ್ಷ VAR_FLOAT:
	ಪಕ್ಷ VAR_BOOL:
	ಪಕ್ಷ VAR_SPECIAL:
	ಪಕ್ಷ VAR_JOB:
	ಪಕ್ಷ VAR_CHANNEL:
	ಪಕ್ಷ VAR_INSTR:
	ಪಕ್ಷ VAR_CLASS:
	ಪಕ್ಷ VAR_OBJECT:
	ಪಕ್ಷ VAR_TYPEALIAS:
	    ಭಙ್ಗ; // not evaluating, skipping over subscript

	ಪಕ್ಷ VAR_ಸಂಖ್ಯೆ:
	ಪಕ್ಷ VAR_STRING:
	    {
		char_u	*s = tv_get_string(rettv);

		len = (long)STRLEN(s);
		ಯದಿ (in_vim9script() || exclusive)
		{
		    ಯದಿ (is_range)
			s = string_slice(s, n1, n2, exclusive);
		    ಅನ್ಯಥಾ
			s = char_from_string(s, n1);
		}
		ಅನ್ಯಥಾ ಯದಿ (is_range)
		{
		    // The resulting variable is a substring.  If the indexes
		    // are out of range the result is empty.
		    ಯದಿ (n1 < 0)
		    {
			n1 = len + n1;
			ಯದಿ (n1 < 0)
			    n1 = 0;
		    }
		    ಯದಿ (n2 < 0)
			n2 = len + n2;
		    ಅನ್ಯಥಾ ಯದಿ (n2 >= len)
			n2 = len;
		    ಯದಿ (n1 >= len || n2 < 0 || n1 > n2)
			s = ರಿಕ್ತ;
		    ಅನ್ಯಥಾ
			s = vim_strnsave(s + n1, n2 - n1 + 1);
		}
		ಅನ್ಯಥಾ
		{
		    // The resulting variable is a string of a single
		    // character.  If the index is too big or negative the
		    // result is empty.
		    ಯದಿ (n1 >= len || n1 < 0)
			s = ರಿಕ್ತ;
		    ಅನ್ಯಥಾ
			s = vim_strnsave(s + n1, 1);
		}
		clear_tv(rettv);
		rettv->v_type = VAR_STRING;
		rettv->vval.v_string = s;
	    }
	    ಭಙ್ಗ;

	ಪಕ್ಷ VAR_BLOB:
	    blob_slice_or_index(rettv->vval.v_blob, is_range, n1, n2,
							     exclusive, rettv);
	    ಭಙ್ಗ;

	ಪಕ್ಷ VAR_LIST:
	    ಯದಿ (var1 == ರಿಕ್ತ)
		n1 = 0;
	    ಯದಿ (var2 == ರಿಕ್ತ)
		n2 = VARNUM_MAX;
	    ಯದಿ (list_slice_or_index(rettv->vval.v_list,
			  is_range, n1, n2, exclusive, rettv, verbose) == FAIL)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    ಭಙ್ಗ;

	ಪಕ್ಷ VAR_TUPLE:
	    ಯದಿ (var1 == ರಿಕ್ತ)
		n1 = 0;
	    ಯದಿ (var2 == ರಿಕ್ತ)
		n2 = VARNUM_MAX;
	    ಯದಿ (tuple_slice_or_index(rettv->vval.v_tuple,
			  is_range, n1, n2, exclusive, rettv, verbose) == FAIL)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    ಭಙ್ಗ;

	ಪಕ್ಷ VAR_DICT:
	    {
		dictitem_T	*item;
		typval_T	tmp;

		ಯದಿ (ಕೀಲಿ == ರಿಕ್ತ)
		{
		    ಕೀಲಿ = tv_get_string_chk(var1);
		    ಯದಿ (ಕೀಲಿ == ರಿಕ್ತ)
			ಪ್ರತ್ಯಾಗಮ FAIL;
		}

		item = dict_find(rettv->vval.v_dict, ಕೀಲಿ, keylen);

		ಯದಿ (item == ರಿಕ್ತ)
		{
		    ಯದಿ (verbose)
		    {
			ಯದಿ (keylen > 0)
			    ಕೀಲಿ[keylen] = NUL;
			semsg(_(e_ಕೀಲಿ_not_present_in_dictionary_str), ಕೀಲಿ);
		    }
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}

		copy_tv(&item->di_tv, &tmp);
		clear_tv(rettv);
		*rettv = tmp;
	    }
	    ಭಙ್ಗ;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Return the function name of partial "pt".
 */
    char_u *
partial_name(partial_T *pt)
{
    ಯದಿ (pt != ರಿಕ್ತ)
    {
	ಯದಿ (pt->pt_name != ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ pt->pt_name;
	ಯದಿ (pt->pt_func != ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ pt->pt_func->uf_name;
    }
    ಪ್ರತ್ಯಾಗಮ (char_u *)"";
}

    ಸ್ಥಿರ ಶೂನ್ಯ
partial_free(partial_T *pt)
{
    ಪೂರ್ಣಾಙ್ಕ i;

    ಪ್ರತಿ (i = 0; i < pt->pt_argc; ++i)
	clear_tv(&pt->pt_argv[i]);
    vim_free(pt->pt_argv);
    dict_unref(pt->pt_dict);
    ಯದಿ (pt->pt_name != ರಿಕ್ತ)
    {
	func_unref(pt->pt_name);
	vim_free(pt->pt_name);
    }
    ಅನ್ಯಥಾ
	func_ptr_unref(pt->pt_func);
    ಯದಿ (pt->pt_obj != ರಿಕ್ತ)
	object_unref(pt->pt_obj);

    // "out_up" is no longer used, decrement refcount on partial that owns it.
    partial_unref(pt->pt_outer.out_up_partial);

    // Using pt_outer from another partial.
    partial_unref(pt->pt_outer_partial);

    // Decrease the reference count for the context of a closure.  If down
    // to the minimum it may be time to free it.
    ಯದಿ (pt->pt_funcstack != ರಿಕ್ತ)
    {
	--pt->pt_funcstack->fs_refcount;
	funcstack_check_refcount(pt->pt_funcstack);
    }
    // Similarly for loop variables.
    ಪ್ರತಿ (i = 0; i < MAX_LOOP_DEPTH; ++i)
	ಯದಿ (pt->pt_loopvars[i] != ರಿಕ್ತ)
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
    ಶೂನ್ಯ
partial_unref(partial_T *pt)
{
    ಯದಿ (pt == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಪೂರ್ಣಾಙ್ಕ	done = FALSE;

    ಯದಿ (--pt->pt_refcount <= 0)
	partial_free(pt);

    // If the reference count goes down to one, the funcstack may be the
    // only reference and can be freed if no other partials reference it.
    ಅನ್ಯಥಾ ಯದಿ (pt->pt_refcount == 1)
    {
	// careful: if the funcstack is freed it may contain this partial
	// and it gets freed as well
	ಯದಿ (pt->pt_funcstack != ರಿಕ್ತ)
	    done = funcstack_check_refcount(pt->pt_funcstack);

	ಯದಿ (!done)
	{
	    ಪೂರ್ಣಾಙ್ಕ	depth;

	    ಪ್ರತಿ (depth = 0; depth < MAX_LOOP_DEPTH; ++depth)
		ಯದಿ (pt->pt_loopvars[depth] != ರಿಕ್ತ
			&& loopvars_check_refcount(pt->pt_loopvars[depth]))
		    ಭಙ್ಗ;
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
    ಸ್ಥಿರ char_u *
string_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    ಪೂರ್ಣಾಙ್ಕ		echo_style,
    ಪೂರ್ಣಾಙ್ಕ		composite_val)
{
    char_u	*r = ರಿಕ್ತ;

    ಯದಿ (echo_style && !composite_val)
    {
	*tofree = ರಿಕ್ತ;
	r = tv->vval.v_string;
	ಯದಿ (r == ರಿಕ್ತ)
	    r = (char_u *)"";
    }
    ಅನ್ಯಥಾ
    {
	*tofree = string_quote(tv->vval.v_string, FALSE);
	r = *tofree;
    }

    ಪ್ರತ್ಯಾಗಮ r;
}

/*
 * Return a textual representation of a function in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "echo_style" is FALSE, put quotes around the function name as
 * "function()", otherwise does not put quotes around function name.
 * May return NULL.
 */
    ಸ್ಥಿರ char_u *
func_tv2string(typval_T *tv, char_u **tofree, ಪೂರ್ಣಾಙ್ಕ echo_style)
{
    char_u	*r = ರಿಕ್ತ;
    char_u	buf[MAX_FUNC_NAME_LEN];

    ಯದಿ (echo_style)
    {
	*tofree = ರಿಕ್ತ;

	ಯದಿ (tv->vval.v_string == ರಿಕ್ತ)
	    r = (char_u *)"function()";
	ಅನ್ಯಥಾ
	{
	    r = make_ufunc_name_readable(tv->vval.v_string, buf,
							MAX_FUNC_NAME_LEN);
	    ಯದಿ (r == buf)
		r = *tofree = vim_strsave(buf);
	}
    }
    ಅನ್ಯಥಾ
    {
	char_u *s = ರಿಕ್ತ;

	ಯದಿ (tv->vval.v_string != ರಿಕ್ತ)
	    s = make_ufunc_name_readable(tv->vval.v_string, buf,
							MAX_FUNC_NAME_LEN);

	r = *tofree = string_quote(s, TRUE);
    }

    ಪ್ರತ್ಯಾಗಮ r;
}

/*
 * Return a textual representation of the object method in "tv", a VAR_PARTIAL.
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "echo_style" is FALSE, put quotes around the function name as
 * "function()", otherwise does not put quotes around function name.
 * May return NULL.
 */
    ಸ್ಥಿರ char_u *
method_tv2string(typval_T *tv, char_u **tofree, ಪೂರ್ಣಾಙ್ಕ echo_style)
{
    char_u	buf[MAX_FUNC_NAME_LEN];
    partial_T	*pt = tv->vval.v_partial;

    size_t len = vim_snprintf((ವರ್ಣ *)buf, ಪರಿಮಾಣ(buf), "<SNR>%d_%s.%s",
			   pt->pt_func->uf_script_ctx.sc_sid,
			   pt->pt_func->uf_class->class_name.string,
			   pt->pt_func->uf_name);
    ಯದಿ (len >= ಪರಿಮಾಣ(buf))
    {
	ಯದಿ (echo_style)
	{
	    *tofree = ರಿಕ್ತ;
	    ಪ್ರತ್ಯಾಗಮ (char_u *)"function()";
	}
	ಅನ್ಯಥಾ
	    ಪ್ರತ್ಯಾಗಮ *tofree = string_quote((char_u*)"", TRUE);
    }

    ಪ್ರತ್ಯಾಗಮ *tofree = echo_style ? vim_strsave(buf) : string_quote(buf, TRUE);
}

/*
 * Return a textual representation of a partial in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * "numbuf" is used for a number.  May return NULL.
 */
    ಸ್ಥಿರ char_u *
partial_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    char_u	*numbuf,
    ಪೂರ್ಣಾಙ್ಕ		copyID)
{
    char_u	*r = ರಿಕ್ತ;
    partial_T	*pt;
    char_u	*fname;
    garray_T	ga;
    ಪೂರ್ಣಾಙ್ಕ		i;
    char_u	*tf;

    pt = tv->vval.v_partial;
    fname = string_quote(pt == ರಿಕ್ತ ? ರಿಕ್ತ : partial_name(pt), FALSE);

    ga_init2(&ga, 1, 100);
    GA_CONCAT_LITERAL(&ga, "function(");
    ಯದಿ (fname != ರಿಕ್ತ)
    {
	// When using uf_name prepend "g:" for a global function.
	ಯದಿ (pt != ರಿಕ್ತ && pt->pt_name == ರಿಕ್ತ && fname[0] == '\''
						&& vim_isupper(fname[1]))
	{
	    GA_CONCAT_LITERAL(&ga, "'g:");
	    ga_concat(&ga, fname + 1);
	}
	ಅನ್ಯಥಾ
	    ga_concat(&ga, fname);
	vim_free(fname);
    }
    ಯದಿ (pt != ರಿಕ್ತ && pt->pt_argc > 0)
    {
	GA_CONCAT_LITERAL(&ga, ", [");
	ಪ್ರತಿ (i = 0; i < pt->pt_argc; ++i)
	{
	    ಯದಿ (i > 0)
		GA_CONCAT_LITERAL(&ga, ", ");
	    ga_concat(&ga, tv2string(&pt->pt_argv[i], &tf, numbuf, copyID));
	    vim_free(tf);
	}
	GA_CONCAT_LITERAL(&ga, "]");
    }
    ಯದಿ (pt != ರಿಕ್ತ && pt->pt_dict != ರಿಕ್ತ)
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

    *tofree = ga.ga_data;
    r = *tofree;

    ಪ್ರತ್ಯಾಗಮ r;
}

/*
 * Return a textual representation of a List in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "copyID" is not zero replace recursive lists with "...".  When
 * "restore_copyID" is FALSE, repeated items in lists are replaced with "...".
 * May return NULL.
 */
    ಸ್ಥಿರ char_u *
list_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    ಪೂರ್ಣಾಙ್ಕ		copyID,
    ಪೂರ್ಣಾಙ್ಕ		restore_copyID)
{
    char_u	*r = ರಿಕ್ತ;

    ಯದಿ (tv->vval.v_list == ರಿಕ್ತ)
    {
	// NULL list is equivalent to empty list.
	*tofree = ರಿಕ್ತ;
	r = (char_u *)"[]";
    }
    ಅನ್ಯಥಾ ಯದಿ (copyID != 0 && tv->vval.v_list->lv_copyID == copyID
	    && tv->vval.v_list->lv_len > 0)
    {
	*tofree = ರಿಕ್ತ;
	r = (char_u *)"[...]";
    }
    ಅನ್ಯಥಾ
    {
	ಪೂರ್ಣಾಙ್ಕ old_copyID;
	ಯದಿ (restore_copyID)
	    old_copyID = tv->vval.v_list->lv_copyID;

	tv->vval.v_list->lv_copyID = copyID;
	*tofree = list2string(tv, copyID, restore_copyID);
	ಯದಿ (restore_copyID)
	    tv->vval.v_list->lv_copyID = old_copyID;
	r = *tofree;
    }

    ಪ್ರತ್ಯಾಗಮ r;
}

/*
 * Return a textual representation of a Tuple in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "copyID" is not zero replace recursive lists with "...".  When
 * "restore_copyID" is FALSE, repeated items in tuples are replaced with "...".
 * May return NULL.
 */
    ಸ್ಥಿರ char_u *
tuple_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    ಪೂರ್ಣಾಙ್ಕ		copyID,
    ಪೂರ್ಣಾಙ್ಕ		restore_copyID)
{
    tuple_T	*tuple = tv->vval.v_tuple;
    char_u	*r = ರಿಕ್ತ;

    ಯದಿ (tuple == ರಿಕ್ತ)
    {
	// NULL tuple is equivalent to an empty tuple.
	*tofree = ರಿಕ್ತ;
	r = (char_u *)"()";
    }
    ಅನ್ಯಥಾ ಯದಿ (copyID != 0 && tuple->tv_copyID == copyID
					&& tuple->tv_ವಸ್ತುಗಳು.ga_len > 0)
    {
	*tofree = ರಿಕ್ತ;
	r = (char_u *)"(...)";
    }
    ಅನ್ಯಥಾ
    {
	ಪೂರ್ಣಾಙ್ಕ old_copyID;
	ಯದಿ (restore_copyID)
	    old_copyID = tuple->tv_copyID;

	tuple->tv_copyID = copyID;
	*tofree = tuple2string(tv, copyID, restore_copyID);
	ಯದಿ (restore_copyID)
	    tuple->tv_copyID = old_copyID;
	r = *tofree;
    }

    ಪ್ರತ್ಯಾಗಮ r;
}

/*
 * Return a textual representation of a Dict in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "copyID" is not zero replace recursive dicts with "...".
 * When "restore_copyID" is FALSE, repeated items in the dictionary are
 * replaced with "...".  May return NULL.
 */
    ಸ್ಥಿರ char_u *
dict_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    ಪೂರ್ಣಾಙ್ಕ		copyID,
    ಪೂರ್ಣಾಙ್ಕ		restore_copyID)
{
    char_u	*r = ರಿಕ್ತ;

    ಯದಿ (tv->vval.v_dict == ರಿಕ್ತ)
    {
	// NULL dict is equivalent to empty dict.
	*tofree = ರಿಕ್ತ;
	r = (char_u *)"{}";
    }
    ಅನ್ಯಥಾ ಯದಿ (copyID != 0 && tv->vval.v_dict->dv_copyID == copyID
	    && tv->vval.v_dict->dv_hashtab.ht_used != 0)
    {
	*tofree = ರಿಕ್ತ;
	r = (char_u *)"{...}";
    }
    ಅನ್ಯಥಾ
    {
	ಪೂರ್ಣಾಙ್ಕ old_copyID;
	ಯದಿ (restore_copyID)
	    old_copyID = tv->vval.v_dict->dv_copyID;

	tv->vval.v_dict->dv_copyID = copyID;
	*tofree = dict2string(tv, copyID, restore_copyID);
	ಯದಿ (restore_copyID)
	    tv->vval.v_dict->dv_copyID = old_copyID;
	r = *tofree;
    }

    ಪ್ರತ್ಯಾಗಮ r;
}

/*
 * Return a textual representation of a job or a channel in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * "numbuf" is used for a number.
 * When "composite_val" is FALSE, put quotes around strings as "string()",
 * otherwise does not put quotes around strings.
 * May return NULL.
 */
    ಸ್ಥಿರ char_u *
jobchan_tv2string(
    typval_T	*tv UNUSED,
    char_u	**tofree UNUSED,
    char_u	*numbuf UNUSED,
    ಪೂರ್ಣಾಙ್ಕ		composite_val UNUSED)
{
    char_u	*r = ರಿಕ್ತ;

#ifdef FEAT_JOB_CHANNEL
    *tofree = ರಿಕ್ತ;

    ಯದಿ (tv->v_type == VAR_JOB)
	r = job_to_string_buf(tv, numbuf);
    ಅನ್ಯಥಾ
	r = channel_to_string_buf(tv, numbuf);

    ಯದಿ (composite_val)
    {
	*tofree = string_quote(r, FALSE);
	r = *tofree;
    }
#endif

    ಪ್ರತ್ಯಾಗಮ r;
}

/*
 * Return a textual representation of a class in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * May return NULL.
 */
    ಸ್ಥಿರ char_u *
class_tv2string(typval_T *tv, char_u **tofree)
{
    char_u	*r = ರಿಕ್ತ;
    size_t	rsize;
    class_T	*cl = tv->vval.v_class;
    string_T	class_name = {(char_u *)"[unknown]", 9};
    string_T	s = {(char_u *)"class", 5};

    ಯದಿ (cl != ರಿಕ್ತ)
    {
	class_name.string = cl->class_name.string;
	class_name.length = cl->class_name.length;
	ಯದಿ (IS_INTERFACE(cl))
	    STR_LITERAL_SET(s, "interface");
	ಅನ್ಯಥಾ ಯದಿ (IS_ENUM(cl))
	    STR_LITERAL_SET(s, "enum");
    }

    rsize = s.length + 1 + class_name.length + 1;
    r = *tofree = alloc(rsize);
    ಯದಿ (r != ರಿಕ್ತ)
	vim_snprintf((ವರ್ಣ *)r, rsize, "%s %s", s.string, (ವರ್ಣ *)class_name.string);

    ಪ್ರತ್ಯಾಗಮ r;
}

/*
 * Return a textual representation of an Object in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "copyID" is not zero replace recursive object with "...".
 * When "restore_copyID" is FALSE, repeated items in the object are
 * replaced with "...".  May return NULL.
 */
    ಸ್ಥಿರ char_u *
object_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    ಪೂರ್ಣಾಙ್ಕ		copyID,
    ಪೂರ್ಣಾಙ್ಕ		restore_copyID,
    char_u	*numbuf,
    ಪೂರ್ಣಾಙ್ಕ		echo_style,
    ಪೂರ್ಣಾಙ್ಕ		composite_val)
{
    char_u	*r = ರಿಕ್ತ;

    object_T	*obj = tv->vval.v_object;
    ಯದಿ (obj == ರಿಕ್ತ || obj->obj_class == ರಿಕ್ತ)
    {
	*tofree = ರಿಕ್ತ;
	r = (char_u *)"object of [unknown]";
    }
    ಅನ್ಯಥಾ ಯದಿ (copyID != 0 && obj->obj_copyID == copyID
	    && obj->obj_class->class_obj_member_ಎಣಿಕೆ != 0)
    {
	size_t n = 25 + obj->obj_class->class_name.length;
	r = alloc(n);
	ಯದಿ (r != ರಿಕ್ತ)
	    (ಶೂನ್ಯ)vim_snprintf((ವರ್ಣ *)r, n, "object of %s {...}",
						obj->obj_class->class_name.string);
	*tofree = r;
    }
    ಅನ್ಯಥಾ
    {
	ಪೂರ್ಣಾಙ್ಕ old_copyID;
	ಯದಿ (restore_copyID)
	    old_copyID = obj->obj_copyID;

	obj->obj_copyID = copyID;
	*tofree = object2string(obj, numbuf, copyID, echo_style,
				restore_copyID, composite_val);
	ಯದಿ (restore_copyID)
	    obj->obj_copyID = old_copyID;
	r = *tofree;
    }

    ಪ್ರತ್ಯಾಗಮ r;
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
    ಪೂರ್ಣಾಙ್ಕ		copyID,
    ಪೂರ್ಣಾಙ್ಕ		echo_style,
    ಪೂರ್ಣಾಙ್ಕ		restore_copyID,
    ಪೂರ್ಣಾಙ್ಕ		composite_val)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	recurse = 0;
    char_u	*r = ರಿಕ್ತ;

    ಯದಿ (recurse >= DICT_MAXNEST)
    {
	ಯದಿ (!did_echo_string_emsg)
	{
	    // Only give this message once for a recursive call to avoid
	    // flooding the user with errors.  And stop iterating over lists
	    // and dicts and objects.
	    did_echo_string_emsg = TRUE;
	    emsg(_(e_variable_nested_too_deep_for_displaying));
	}
	*tofree = ರಿಕ್ತ;
	ಪ್ರತ್ಯಾಗಮ (char_u *)"{E724}";
    }
    ++recurse;

    ವಿಕಲ್ಪ (tv->v_type)
    {
	ಪಕ್ಷ VAR_STRING:
	    r = string_tv2string(tv, tofree, echo_style, composite_val);
	    ಭಙ್ಗ;

	ಪಕ್ಷ VAR_FUNC:
	    r = func_tv2string(tv, tofree, echo_style);
	    ಭಙ್ಗ;

	ಪಕ್ಷ VAR_PARTIAL:
	    ಯದಿ (tv->vval.v_partial == ರಿಕ್ತ
		    || tv->vval.v_partial->pt_obj == ರಿಕ್ತ)
		r = partial_tv2string(tv, tofree, numbuf, copyID);
	    ಅನ್ಯಥಾ
		r = method_tv2string(tv, tofree, echo_style);
	    ಭಙ್ಗ;

	ಪಕ್ಷ VAR_BLOB:
	    r = blob2string(tv->vval.v_blob, tofree, numbuf);
	    ಭಙ್ಗ;

	ಪಕ್ಷ VAR_LIST:
	    r = list_tv2string(tv, tofree, copyID, restore_copyID);
	    ಭಙ್ಗ;

	ಪಕ್ಷ VAR_TUPLE:
	    r = tuple_tv2string(tv, tofree, copyID, restore_copyID);
	    ಭಙ್ಗ;

	ಪಕ್ಷ VAR_DICT:
	    r = dict_tv2string(tv, tofree, copyID, restore_copyID);
	    ಭಙ್ಗ;

	ಪಕ್ಷ VAR_ಸಂಖ್ಯೆ:
	ಪಕ್ಷ VAR_UNKNOWN:
	ಪಕ್ಷ VAR_ANY:
	ಪಕ್ಷ VAR_VOID:
	    *tofree = ರಿಕ್ತ;
	    r = tv_get_string_buf(tv, numbuf);
	    ಭಙ್ಗ;

	ಪಕ್ಷ VAR_JOB:
	ಪಕ್ಷ VAR_CHANNEL:
	    r = jobchan_tv2string(tv, tofree, numbuf, composite_val);
	    ಭಙ್ಗ;

	ಪಕ್ಷ VAR_INSTR:
	    *tofree = ರಿಕ್ತ;
	    r = (char_u *)"instructions";
	    ಭಙ್ಗ;

	ಪಕ್ಷ VAR_CLASS:
	    r = class_tv2string(tv, tofree);
	    ಭಙ್ಗ;

	ಪಕ್ಷ VAR_OBJECT:
	    r = object_tv2string(tv, tofree, copyID, restore_copyID,
					 numbuf, echo_style, composite_val);
	    ಭಙ್ಗ;

	ಪಕ್ಷ VAR_FLOAT:
	    *tofree = ರಿಕ್ತ;
	    vim_snprintf((ವರ್ಣ *)numbuf, NUMBUFLEN, "%g", tv->vval.v_float);
	    r = numbuf;
	    ಭಙ್ಗ;

	ಪಕ್ಷ VAR_BOOL:
	ಪಕ್ಷ VAR_SPECIAL:
	    *tofree = ರಿಕ್ತ;
	    r = (char_u *)get_var_special_name(tv->vval.v_ಸಂಖ್ಯೆ);
	    ಭಙ್ಗ;

	ಪಕ್ಷ VAR_TYPEALIAS:
	    *tofree = vim_strsave(tv->vval.v_typealias->ta_name);
	    r = *tofree;
	    ಯದಿ (r == ರಿಕ್ತ)
		r = (char_u *)"";
	    ಭಙ್ಗ;
    }

    ಯದಿ (--recurse == 0)
	did_echo_string_emsg = FALSE;
    ಪ್ರತ್ಯಾಗಮ r;
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
    ಪೂರ್ಣಾಙ್ಕ		copyID)
{
    ಪ್ರತ್ಯಾಗಮ echo_string_core(tv, tofree, numbuf, copyID, TRUE, FALSE, FALSE);
}

/*
 * Convert the specified byte index of line 'lnum' in buffer 'buf' to a
 * character index.  Works only for loaded buffers. Returns -1 on failure.
 * The index of the first byte and the first character is zero.
 */
    ಪೂರ್ಣಾಙ್ಕ
buf_byteidx_to_charidx(buf_T *buf, ಪೂರ್ಣಾಙ್ಕ lnum, ಪೂರ್ಣಾಙ್ಕ byteidx)
{
    char_u	*str;
    char_u	*t;
    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ;

    ಯದಿ (buf == ರಿಕ್ತ || buf->b_ml.ml_mfp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ -1;

    ಯದಿ (lnum > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	lnum = buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;

    str = ml_get_buf(buf, lnum, FALSE);
    ಯದಿ (str == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ -1;

    ಯದಿ (*str == NUL)
	ಪ್ರತ್ಯಾಗಮ 0;

    // count the number of characters
    t = str;
    ಪ್ರತಿ (ಎಣಿಕೆ = 0; *t != NUL && t <= str + byteidx; ಎಣಿಕೆ++)
	t += mb_ptr2len(t);

    // In insert mode, when the cursor is at the end of a non-empty line,
    // byteidx points to the NUL character immediately past the end of the
    // string. In this case, add one to the character count.
    ಯದಿ (*t == NUL && byteidx != 0 && t == str + byteidx)
	ಎಣಿಕೆ++;

    ಪ್ರತ್ಯಾಗಮ ಎಣಿಕೆ - 1;
}

/*
 * Convert the specified character index of line 'lnum' in buffer 'buf' to a
 * byte index.  Works only for loaded buffers. Returns -1 on failure.
 * The index of the first byte and the first character is zero.
 */
    ಪೂರ್ಣಾಙ್ಕ
buf_charidx_to_byteidx(buf_T *buf, ಪೂರ್ಣಾಙ್ಕ lnum, ಪೂರ್ಣಾಙ್ಕ charidx)
{
    char_u	*str;
    char_u	*t;

    ಯದಿ (buf == ರಿಕ್ತ || buf->b_ml.ml_mfp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ -1;

    ಯದಿ (lnum > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	lnum = buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;

    str = ml_get_buf(buf, lnum, FALSE);
    ಯದಿ (str == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ -1;

    // Convert the character offset to a byte offset
    t = str;
    ಯಾವತ್ (*t != NUL && --charidx > 0)
	t += mb_ptr2len(t);

    ಪ್ರತ್ಯಾಗಮ t - str;
}

/*
 * Translate a String variable into a position.
 * Returns NULL when there is an error.
 */
    pos_T *
var2fpos(
    typval_T	*varp,
    ಪೂರ್ಣಾಙ್ಕ		dollar_lnum,	// TRUE when $ is last line
    ಪೂರ್ಣಾಙ್ಕ		*fnum,		// set to fnum for '0, 'A, etc.
    ಪೂರ್ಣಾಙ್ಕ		charcol)	// return character column
{
    char_u		*name;
    ಸ್ಥಿರ pos_T	pos;
    pos_T		*pp;
    ಪೂರ್ಣಾಙ್ಕ			error = FALSE;

    // Argument can be [lnum, col, coladd].
    ಯದಿ (varp->v_type == VAR_LIST)
    {
	list_T		*l;
	ಪೂರ್ಣಾಙ್ಕ		len;
	listitem_T	*li;

	l = varp->vval.v_list;
	ಯದಿ (l == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

	// Get the line number
	pos.lnum = list_find_nr(l, 0L, &error);
	ಯದಿ (error || pos.lnum <= 0 || pos.lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;	// invalid line number
	ಯದಿ (charcol)
	    len = (long)mb_charlen(ml_get(pos.lnum));
	ಅನ್ಯಥಾ
	    len = (long)ml_get_len(pos.lnum);

	// Get the column number
	// We accept "$" for the column number: last column.
	li = list_find(l, 1L);
	ಯದಿ (li != ರಿಕ್ತ && li->li_tv.v_type == VAR_STRING
		&& li->li_tv.vval.v_string != ರಿಕ್ತ
		&& STRCMP(li->li_tv.vval.v_string, "$") == 0)
	{
	    pos.col = len + 1;
	}
	ಅನ್ಯಥಾ
	{
	    pos.col = list_find_nr(l, 1L, &error);
	    ಯದಿ (error)
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}

	// Accept a position up to the NUL after the line.
	ಯದಿ (pos.col == 0 || (ಪೂರ್ಣಾಙ್ಕ)pos.col > len + 1)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;	// invalid column number
	--pos.col;

	// Get the virtual offset.  Defaults to zero.
	pos.coladd = list_find_nr(l, 2L, &error);
	ಯದಿ (error)
	    pos.coladd = 0;

	ಪ್ರತ್ಯಾಗಮ &pos;
    }

    ಯದಿ (in_vim9script() && check_for_string_arg(varp, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    name = tv_get_string_chk(varp);
    ಯದಿ (name == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    error = TRUE;
    pos.lnum = 0;
    ಯದಿ (name[0] == '.')
    {
	ಯದಿ (!in_vim9script() || name[1] == NUL)
	{
	    error = FALSE;
	    // cursor
	    pos = curwin->w_cursor;
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (name[0] == 'v' && name[1] == NUL)
    {
	// Visual start
	ಯದಿ (VIsual_active)
	    pos = VIsual;
	ಅನ್ಯಥಾ
	    pos = curwin->w_cursor;
    }
    ಅನ್ಯಥಾ ಯದಿ (name[0] == '\'')
    {
	ಯದಿ (!in_vim9script() || (name[1] != NUL && name[2] == NUL))
	{
	    error = FALSE;
	    // mark
	    pp = getmark_buf_fnum(curbuf, name[1], FALSE, fnum);
	    ಯದಿ (pp == ರಿಕ್ತ || pp == (pos_T *)-1 || pp->lnum <= 0)
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    pos = *pp;
	}
    }
    ಯದಿ (pos.lnum != 0)
    {
	ಯದಿ (charcol)
	    pos.col = buf_byteidx_to_charidx(curbuf, pos.lnum, pos.col);
	ಪ್ರತ್ಯಾಗಮ &pos;
    }

    pos.coladd = 0;

    ಯದಿ (name[0] == 'w' && dollar_lnum)
    {
	// the "w_valid" flags are not reset when moving the cursor, but they
	// do matter for update_topline() and validate_botline().
	check_cursor_moved(curwin);

	pos.col = 0;
	ಯದಿ (name[1] == '0')		// "w0": first visible line
	{
#ifdef FEAT_TERMINAL
	    ಯದಿ (bt_terminal(curwin->w_buffer)
				    && curwin->w_buffer->b_term != ರಿಕ್ತ
				    && !term_in_normal_mode(curwin->w_buffer))
		may_move_terminal_to_buffer(curwin->w_buffer->b_term, TRUE);
#endif
	    update_topline();
	    // In silent Ex mode topline is zero, but that's not a valid line
	    // number; use one instead.
	    pos.lnum = curwin->w_topline > 0 ? curwin->w_topline : 1;
	    ಪ್ರತ್ಯಾಗಮ &pos;
	}
	ಅನ್ಯಥಾ ಯದಿ (name[1] == '$')	// "w$": last visible line
	{
#ifdef FEAT_TERMINAL
	    ಯದಿ (bt_terminal(curwin->w_buffer)
				    && curwin->w_buffer->b_term != ರಿಕ್ತ
				    && !term_in_normal_mode(curwin->w_buffer))
		may_move_terminal_to_buffer(curwin->w_buffer->b_term, TRUE);
#endif
	    validate_botline();
	    // In silent Ex mode botline is zero, return zero then.
	    pos.lnum = curwin->w_botline > 0 ? curwin->w_botline - 1 : 0;
	    ಪ್ರತ್ಯಾಗಮ &pos;
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (name[0] == '$')		// last column or line
    {
	ಯದಿ (dollar_lnum)
	{
	    pos.lnum = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	    pos.col = 0;
	}
	ಅನ್ಯಥಾ
	{
	    pos.lnum = curwin->w_cursor.lnum;
	    ಯದಿ (charcol)
		pos.col = (colnr_T)mb_charlen(ml_get_curline());
	    ಅನ್ಯಥಾ
		pos.col = ml_get_curline_len();
	}
	ಪ್ರತ್ಯಾಗಮ &pos;
    }
    ಯದಿ (in_vim9script() && error)
	semsg(_(e_invalid_ಮೌಲ್ಯ_for_ಸಾಲು_ಸಂಖ್ಯೆ_str), name);
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
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
    ಪೂರ್ಣಾಙ್ಕ
list2fpos(
    typval_T	*arg,
    pos_T	*posp,
    ಪೂರ್ಣಾಙ್ಕ		*fnump,
    colnr_T	*curswantp,
    ಪೂರ್ಣಾಙ್ಕ		charcol)
{
    list_T	*l = arg->vval.v_list;
    long	i = 0;
    long	n;

    // List must be: [fnum, lnum, col, coladd, curswant], where "fnum" is only
    // there when "fnump" isn't NULL; "coladd" and "curswant" are optional.
    ಯದಿ (arg->v_type != VAR_LIST
	    || l == ರಿಕ್ತ
	    || l->lv_len < (fnump == ರಿಕ್ತ ? 2 : 3)
	    || l->lv_len > (fnump == ರಿಕ್ತ ? 4 : 5))
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (fnump != ರಿಕ್ತ)
    {
	n = list_find_nr(l, i++, ರಿಕ್ತ);	// fnum
	ಯದಿ (n < 0)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	ಯದಿ (n == 0)
	    n = curbuf->b_fnum;		// current buffer
	*fnump = n;
    }

    n = list_find_nr(l, i++, ರಿಕ್ತ);	// lnum
    ಯದಿ (n < 0)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    posp->lnum = n;

    n = list_find_nr(l, i++, ರಿಕ್ತ);	// col
    ಯದಿ (n < 0)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    // If character position is specified, then convert to byte position
    // If the line number is zero use the cursor line.
    ಯದಿ (charcol)
    {
	buf_T	*buf;

	// Get the text for the specified line in a loaded buffer
	buf = buflist_findnr(fnump == ರಿಕ್ತ ? curbuf->b_fnum : *fnump);
	ಯದಿ (buf == ರಿಕ್ತ || buf->b_ml.ml_mfp == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	n = buf_charidx_to_byteidx(buf,
		  posp->lnum == 0 ? curwin->w_cursor.lnum : posp->lnum, n) + 1;
    }
    posp->col = n;

    n = list_find_nr(l, i, ರಿಕ್ತ);	// off
    ಯದಿ (n < 0)
	posp->coladd = 0;
    ಅನ್ಯಥಾ
	posp->coladd = n;

    ಯದಿ (curswantp != ರಿಕ್ತ)
	*curswantp = list_find_nr(l, i + 1, ರಿಕ್ತ);  // curswant

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Get the length of an environment variable name.
 * Advance "arg" to the first character after the name.
 * Return 0 for error.
 */
    ಪೂರ್ಣಾಙ್ಕ
get_env_len(char_u **arg)
{
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		len;

    ಪ್ರತಿ (p = *arg; vim_isIDc(*p); ++p)
	;
    ಯದಿ (p == *arg)	    // no name found
	ಪ್ರತ್ಯಾಗಮ 0;

    len = (ಪೂರ್ಣಾಙ್ಕ)(p - *arg);
    *arg = p;
    ಪ್ರತ್ಯಾಗಮ len;
}

/*
 * Get the length of the name of a function or internal variable.
 * "arg" is advanced to after the name.
 * Return 0 if something is wrong.
 */
    ಪೂರ್ಣಾಙ್ಕ
get_id_len(char_u **arg)
{
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		len;

    // Find the end of the name.
    ಪ್ರತಿ (p = *arg; eval_isnamec(*p); ++p)
    {
	ಯದಿ (*p == ':')
	{
	    // "s:" is start of "s:var", but "n:" is not and can be used in
	    // slice "[n:]".  Also "xx:" is not a namespace.
	    len = (ಪೂರ್ಣಾಙ್ಕ)(p - *arg);
	    ಯದಿ ((len == 1 && vim_strchr(NAMESPACE_CHAR, **arg) == ರಿಕ್ತ)
		    || len > 1)
		ಭಙ್ಗ;
	}
    }
    ಯದಿ (p == *arg)	    // no name found
	ಪ್ರತ್ಯಾಗಮ 0;

    len = (ಪೂರ್ಣಾಙ್ಕ)(p - *arg);
    *arg = p;

    ಪ್ರತ್ಯಾಗಮ len;
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
    ಪೂರ್ಣಾಙ್ಕ
get_name_len(
    char_u	**arg,
    char_u	**alias,
    ಪೂರ್ಣಾಙ್ಕ		evaluate,
    ಪೂರ್ಣಾಙ್ಕ		verbose)
{
    ಪೂರ್ಣಾಙ್ಕ		len;
    char_u	*p;
    char_u	*ಅಭಿವ್ಯಕ್ತಿ_start;
    char_u	*ಅಭಿವ್ಯಕ್ತಿ_end;

    *alias = ರಿಕ್ತ;  // default to no alias

    ಯದಿ ((*arg)[0] == K_SPECIAL && (*arg)[1] == KS_EXTRA
						  && (*arg)[2] == (ಪೂರ್ಣಾಙ್ಕ)KE_SNR)
    {
	// hard coded <SNR>, already translated
	*arg += 3;
	ಪ್ರತ್ಯಾಗಮ get_id_len(arg) + 3;
    }
    len = eval_fname_script(*arg);
    ಯದಿ (len > 0)
    {
	// literal "<SID>", "s:" or "<SNR>"
	*arg += len;
    }

    /*
     * Find the end of the name; check for {} construction.
     */
    p = find_name_end(*arg, &ಅಭಿವ್ಯಕ್ತಿ_start, &ಅಭಿವ್ಯಕ್ತಿ_end,
					       len > 0 ? 0 : FNE_CHECK_START);
    ಯದಿ (ಅಭಿವ್ಯಕ್ತಿ_start != ರಿಕ್ತ)
    {
	char_u	*temp_string;

	ಯದಿ (!evaluate)
	{
	    len += (ಪೂರ್ಣಾಙ್ಕ)(p - *arg);
	    *arg = skipwhite(p);
	    ಪ್ರತ್ಯಾಗಮ len;
	}

	/*
	 * Include any <SID> etc in the expanded string:
	 * Thus the -len here.
	 */
	temp_string = make_expanded_name(*arg - len, ಅಭಿವ್ಯಕ್ತಿ_start, ಅಭಿವ್ಯಕ್ತಿ_end, p);
	ಯದಿ (temp_string == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ -1;
	*alias = temp_string;
	*arg = skipwhite(p);
	ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)STRLEN(temp_string);
    }

    len += get_id_len(arg);
    // Only give an error when there is something, otherwise it will be
    // reported at a higher level.
    ಯದಿ (len == 0 && verbose && **arg != NUL)
	semsg(_(e_invalid_expression_str), *arg);

    ಪ್ರತ್ಯಾಗಮ len;
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
find_name_end(
    char_u	*arg,
    char_u	**ಅಭಿವ್ಯಕ್ತಿ_start,
    char_u	**ಅಭಿವ್ಯಕ್ತಿ_end,
    ಪೂರ್ಣಾಙ್ಕ		flags)
{
    ಪೂರ್ಣಾಙ್ಕ		mb_nest = 0;
    ಪೂರ್ಣಾಙ್ಕ		br_nest = 0;
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		len;
    ಪೂರ್ಣಾಙ್ಕ		allow_curly = !in_vim9script();

    ಯದಿ (ಅಭಿವ್ಯಕ್ತಿ_start != ರಿಕ್ತ)
    {
	*ಅಭಿವ್ಯಕ್ತಿ_start = ರಿಕ್ತ;
	*ಅಭಿವ್ಯಕ್ತಿ_end = ರಿಕ್ತ;
    }

    // Quick check for valid starting character.
    ಯದಿ ((flags & FNE_CHECK_START) && !eval_isnamec1(*arg)
					      && (*arg != '{' || !allow_curly))
	ಪ್ರತ್ಯಾಗಮ arg;

    ಪ್ರತಿ (p = arg; *p != NUL
		    && (eval_isnamec(*p)
			|| (*p == '{' && allow_curly)
			|| ((flags & FNE_INCL_BR) && (*p == '['
					 || (*p == '.' && eval_isdictc(p[1]))))
			|| mb_nest != 0
			|| br_nest != 0); MB_PTR_ADV(p))
    {
	ಯದಿ (*p == '\'')
	{
	    // skip over 'string' to avoid counting [ and ] inside it.
	    ಪ್ರತಿ (p = p + 1; *p != NUL && *p != '\''; MB_PTR_ADV(p))
		;
	    ಯದಿ (*p == NUL)
		ಭಙ್ಗ;
	}
	ಅನ್ಯಥಾ ಯದಿ (*p == '"')
	{
	    // skip over "str\"ing" to avoid counting [ and ] inside it.
	    ಪ್ರತಿ (p = p + 1; *p != NUL && *p != '"'; MB_PTR_ADV(p))
		ಯದಿ (*p == '\\' && p[1] != NUL)
		    ++p;
	    ಯದಿ (*p == NUL)
		ಭಙ್ಗ;
	}
	ಅನ್ಯಥಾ ಯದಿ (br_nest == 0 && mb_nest == 0 && *p == ':')
	{
	    // "s:" is start of "s:var", but "n:" is not and can be used in
	    // slice "[n:]".  Also "xx:" is not a namespace. But {ns}: is.
	    len = (ಪೂರ್ಣಾಙ್ಕ)(p - arg);
	    ಯದಿ ((len == 1 && vim_strchr(NAMESPACE_CHAR, *arg) == ರಿಕ್ತ)
		    || (len > 1 && p[-1] != '}'))
		ಭಙ್ಗ;
	}

	ಯದಿ (mb_nest == 0)
	{
	    ಯದಿ (*p == '[')
		++br_nest;
	    ಅನ್ಯಥಾ ಯದಿ (*p == ']')
		--br_nest;
	}

	ಯದಿ (br_nest == 0 && allow_curly)
	{
	    ಯದಿ (*p == '{')
	    {
		mb_nest++;
		ಯದಿ (ಅಭಿವ್ಯಕ್ತಿ_start != ರಿಕ್ತ && *ಅಭಿವ್ಯಕ್ತಿ_start == ರಿಕ್ತ)
		    *ಅಭಿವ್ಯಕ್ತಿ_start = p;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (*p == '}')
	    {
		mb_nest--;
		ಯದಿ (ಅಭಿವ್ಯಕ್ತಿ_start != ರಿಕ್ತ && mb_nest == 0 && *ಅಭಿವ್ಯಕ್ತಿ_end == ರಿಕ್ತ)
		    *ಅಭಿವ್ಯಕ್ತಿ_end = p;
	    }
	}
    }

    ಪ್ರತ್ಯಾಗಮ p;
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
    ಸ್ಥಿರ char_u *
make_expanded_name(
    char_u	*in_start,
    char_u	*ಅಭಿವ್ಯಕ್ತಿ_start,
    char_u	*ಅಭಿವ್ಯಕ್ತಿ_end,
    char_u	*in_end)
{
    char_u	c1;
    char_u	*retval = ರಿಕ್ತ;
    char_u	*temp_result;

    ಯದಿ (ಅಭಿವ್ಯಕ್ತಿ_end == ರಿಕ್ತ || in_end == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    *ಅಭಿವ್ಯಕ್ತಿ_start	= NUL;
    *ಅಭಿವ್ಯಕ್ತಿ_end = NUL;
    c1 = *in_end;
    *in_end = NUL;

    temp_result = eval_to_string(ಅಭಿವ್ಯಕ್ತಿ_start + 1, FALSE, FALSE);
    ಯದಿ (temp_result != ರಿಕ್ತ)
    {
	size_t	retvalsize = (size_t)(ಅಭಿವ್ಯಕ್ತಿ_start - in_start)
				+ STRLEN(temp_result)
				+ (size_t)(in_end - ಅಭಿವ್ಯಕ್ತಿ_end) + 1;

	retval = alloc(retvalsize);
	ಯದಿ (retval != ರಿಕ್ತ)
	    vim_snprintf((ವರ್ಣ *)retval, retvalsize, "%s%s%s",
				in_start, temp_result, ಅಭಿವ್ಯಕ್ತಿ_end + 1);
    }
    vim_free(temp_result);

    *in_end = c1;		// put char back for error messages
    *ಅಭಿವ್ಯಕ್ತಿ_start = '{';
    *ಅಭಿವ್ಯಕ್ತಿ_end = '}';

    ಯದಿ (retval != ರಿಕ್ತ)
    {
	temp_result = find_name_end(retval, &ಅಭಿವ್ಯಕ್ತಿ_start, &ಅಭಿವ್ಯಕ್ತಿ_end, 0);
	ಯದಿ (ಅಭಿವ್ಯಕ್ತಿ_start != ರಿಕ್ತ)
	{
	    // Further expansion!
	    temp_result = make_expanded_name(retval, ಅಭಿವ್ಯಕ್ತಿ_start,
						       ಅಭಿವ್ಯಕ್ತಿ_end, temp_result);
	    vim_free(retval);
	    retval = temp_result;
	}
    }

    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Return TRUE if character "c" can be used in a variable or function name.
 * Does not include '{' or '}' for magic braces.
 */
    ಪೂರ್ಣಾಙ್ಕ
eval_isnamec(ಪೂರ್ಣಾಙ್ಕ c)
{
    ಪ್ರತ್ಯಾಗಮ ASCII_ISALNUM(c) || c == '_' || c == ':' || c == AUTOLOAD_CHAR;
}

/*
 * Return TRUE if character "c" can be used as the first character in a
 * variable or function name (excluding '{' and '}').
 */
    ಪೂರ್ಣಾಙ್ಕ
eval_isnamec1(ಪೂರ್ಣಾಙ್ಕ c)
{
    ಪ್ರತ್ಯಾಗಮ ASCII_ISALPHA(c) || c == '_';
}

/*
 * Return TRUE if character "c" can be used as the first character of a
 * dictionary key.
 */
    ಪೂರ್ಣಾಙ್ಕ
eval_isdictc(ಪೂರ್ಣಾಙ್ಕ c)
{
    ಪ್ರತ್ಯಾಗಮ ASCII_ISALNUM(c) || c == '_';
}

/*
 * Return true when "p" points to "->" followed by a method name or a lambda.
 * Only called for Vim9 script.
 */
    ಸ್ಥಿರ bool
method_call_follows(char_u *p)
{
    ಯದಿ (p[0] != '-' || p[1] != '>')
	ಪ್ರತ್ಯಾಗಮ false;

    char_u	*after = skipwhite(p + 2);

    ಪ್ರತ್ಯಾಗಮ ASCII_ISALPHA(*after) || *after == '(';
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
    ಪೂರ್ಣಾಙ್ಕ
handle_subscript(
    char_u	**arg,
    char_u	*name_start,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    ಪೂರ್ಣಾಙ್ಕ		verbose)	// give error messages
{
    ಪೂರ್ಣಾಙ್ಕ		evaluate = evalarg != ರಿಕ್ತ
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    ಪೂರ್ಣಾಙ್ಕ		ret = OK;
    dict_T	*selfdict = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		check_white = TRUE;
    ಪೂರ್ಣಾಙ್ಕ		getnext;
    char_u	*p;

    ಯಾವತ್ (ret == OK)
    {
	// When at the end of the line and ".name" or a method call follows in
	// the next line then consume the line break.
	p = eval_next_non_blank(*arg, evalarg, &getnext);
	ಯದಿ (getnext
	    && ((*p == '.'
		    && ((rettv->v_type == VAR_DICT && eval_isdictc(p[1]))
			|| rettv->v_type == VAR_CLASS
			|| rettv->v_type == VAR_OBJECT))
		|| method_call_follows(p)))
	{
	    *arg = eval_next_ಸಾಲು(*arg, evalarg);
	    p = *arg;
	    check_white = FALSE;
	}

	ಯದಿ (rettv->v_type == VAR_ANY)
	{
	    char_u	*exp_name;
	    ಪೂರ್ಣಾಙ್ಕ		cc;
	    ಪೂರ್ಣಾಙ್ಕ		idx;
	    ufunc_T	*ufunc;
	    type_T	*type;

	    // Found script from "import {name} as name", script item name must
	    // follow.  "rettv->vval.v_number" has the script ID.
	    ಯದಿ (**arg != '.')
	    {
		ಯದಿ (verbose)
		    semsg(_(e_expected_dot_after_name_str),
					name_start != ರಿಕ್ತ ? name_start: *arg);
		ret = FAIL;
		ಭಙ್ಗ;
	    }
	    ++*arg;
	    ಯದಿ (IS_WHITE_OR_NUL(**arg))
	    {
		ಯದಿ (verbose)
		    emsg(_(e_no_white_space_allowed_after_dot));
		ret = FAIL;
		ಭಙ್ಗ;
	    }

	    // isolate the name
	    exp_name = *arg;
	    ಯಾವತ್ (eval_isnamec(**arg))
		++*arg;
	    cc = **arg;
	    **arg = NUL;

	    idx = find_exported(rettv->vval.v_ಸಂಖ್ಯೆ, exp_name, &ufunc, &type,
		       evalarg == ರಿಕ್ತ ? ರಿಕ್ತ : evalarg->eval_cctx,
		       evalarg == ರಿಕ್ತ ? ರಿಕ್ತ : evalarg->eval_cstack, verbose);
	    **arg = cc;

	    ಯದಿ (idx < 0 && ufunc == ರಿಕ್ತ)
	    {
		ret = FAIL;
		ಭಙ್ಗ;
	    }
	    ಯದಿ (idx >= 0)
	    {
		scriptitem_T    *si = SCRIPT_ITEM(rettv->vval.v_ಸಂಖ್ಯೆ);
		svar_T		*sv = ((svar_T *)si->sn_var_vals.ga_data) + idx;

		copy_tv(sv->sv_tv, rettv);
	    }
	    ಅನ್ಯಥಾ
	    {
		rettv->v_type = VAR_FUNC;
		ಯದಿ (**arg == '<')
		{
		    char_u *s = get_generic_func_name(ufunc, arg);
		    ಯದಿ (s != ರಿಕ್ತ)
			rettv->vval.v_string = s;
		    ಅನ್ಯಥಾ
			ret = FAIL;
		}
		ಅನ್ಯಥಾ
		{
		    rettv->vval.v_string =
			vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
		}
	    }
	    ಅನುವರ್ತ;
	}

	ಯದಿ ((**arg == '(' && (!evaluate || rettv->v_type == VAR_FUNC
			    || rettv->v_type == VAR_PARTIAL))
		    && (!check_white || !VIM_ISWHITE(*(*arg - 1))))
	{
	    ret = call_func_rettv(arg, evalarg, rettv, evaluate,
							       selfdict, ರಿಕ್ತ);

	    // Stop the expression evaluation when immediately aborting on
	    // error, or when an interrupt occurred or an exception was thrown
	    // but not caught.
	    ಯದಿ (aborting())
	    {
		ಯದಿ (ret == OK)
		    clear_tv(rettv);
		ret = FAIL;
	    }
	    dict_unref(selfdict);
	    selfdict = ರಿಕ್ತ;
	}
	ಅನ್ಯಥಾ ಯದಿ (p[0] == '-' && p[1] == '>')
	{
	    ಯದಿ (in_vim9script())
		*arg = skipwhite(p + 2);
	    ಅನ್ಯಥಾ
		*arg = p + 2;
	    ಯದಿ (ret == OK && evaluate && rettv->v_type == VAR_VOID)
	    {
		ಯದಿ (verbose)
		    emsg(_(e_cannot_use_void_ಮೌಲ್ಯ));
		ret = FAIL;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (VIM_ISWHITE(**arg))
	    {
		emsg(_(e_no_white_space_allowed_before_parenthesis));
		ret = FAIL;
	    }
	    ಅನ್ಯಥಾ ಯದಿ ((**arg == '{' && !in_vim9script()) || **arg == '(')
		// expr->{lambda}() or expr->(lambda)()
		ret = eval_lambda(arg, rettv, evalarg, verbose);
	    ಅನ್ಯಥಾ
		// expr->name()
		ret = eval_method(arg, rettv, evalarg, verbose);
	}
	// "." is ".name" lookup when we found a dict or when evaluating and
	// scriptversion is at least 2, where string concatenation is "..".
	ಅನ್ಯಥಾ ಯದಿ (**arg == '['
		|| (**arg == '.' && (rettv->v_type == VAR_DICT
			|| (!evaluate
			    && (*arg)[1] != '.'
			    && !in_old_script(2)))))
	{
	    dict_unref(selfdict);
	    ಯದಿ (rettv->v_type == VAR_DICT)
	    {
		selfdict = rettv->vval.v_dict;
		ಯದಿ (selfdict != ರಿಕ್ತ)
		    ++selfdict->dv_refcount;
	    }
	    ಅನ್ಯಥಾ
		selfdict = ರಿಕ್ತ;
	    ಯದಿ (eval_index(arg, rettv, evalarg, verbose) == FAIL)
	    {
		clear_tv(rettv);
		ret = FAIL;
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (**arg == '.' && (rettv->v_type == VAR_CLASS
					       || rettv->v_type == VAR_OBJECT))
	{
	    // class member: SomeClass.varname
	    // class method: SomeClass.SomeMethod()
	    // class constructor: SomeClass.new()
	    // object member: someObject.varname
	    // object method: someObject.SomeMethod()
	    ಯದಿ (class_object_index(arg, rettv, evalarg, verbose) == FAIL)
	    {
		clear_tv(rettv);
		ret = FAIL;
	    }
	}
	ಅನ್ಯಥಾ
	    ಭಙ್ಗ;
    }

    // Turn "dict.Func" into a partial for "Func" bound to "dict".
    // Don't do this when "Func" is already a partial that was bound
    // explicitly (pt_auto is FALSE).
    ಯದಿ (selfdict != ರಿಕ್ತ
	    && (rettv->v_type == VAR_FUNC
		|| (rettv->v_type == VAR_PARTIAL
		    && (rettv->vval.v_partial->pt_auto
			|| rettv->vval.v_partial->pt_dict == ರಿಕ್ತ))))
	selfdict = make_partial(selfdict, rettv);

    dict_unref(selfdict);
    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Make a copy of an item.
 * Lists and Dictionaries are also copied.  A deep copy if "deep" is set.
 * "top" is TRUE for the toplevel of copy().
 * For deepcopy() "copyID" is zero for a full copy or the ID for when a
 * reference to an already copied list/dict can be used.
 * Returns FAIL or OK.
 */
    ಪೂರ್ಣಾಙ್ಕ
item_copy(
    typval_T	*from,
    typval_T	*to,
    ಪೂರ್ಣಾಙ್ಕ		deep,
    ಪೂರ್ಣಾಙ್ಕ		ತುದಿ,
    ಪೂರ್ಣಾಙ್ಕ		copyID)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	recurse = 0;
    ಪೂರ್ಣಾಙ್ಕ		ret = OK;

    ಯದಿ (recurse >= DICT_MAXNEST)
    {
	emsg(_(e_variable_nested_too_deep_for_making_copy));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ++recurse;

    ವಿಕಲ್ಪ (from->v_type)
    {
	ಪಕ್ಷ VAR_ಸಂಖ್ಯೆ:
	ಪಕ್ಷ VAR_FLOAT:
	ಪಕ್ಷ VAR_STRING:
	ಪಕ್ಷ VAR_FUNC:
	ಪಕ್ಷ VAR_PARTIAL:
	ಪಕ್ಷ VAR_BOOL:
	ಪಕ್ಷ VAR_SPECIAL:
	ಪಕ್ಷ VAR_JOB:
	ಪಕ್ಷ VAR_CHANNEL:
	ಪಕ್ಷ VAR_INSTR:
	ಪಕ್ಷ VAR_CLASS:
	ಪಕ್ಷ VAR_OBJECT:
	ಪಕ್ಷ VAR_TYPEALIAS:
	    copy_tv(from, to);
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_LIST:
	    to->v_type = VAR_LIST;
	    to->v_lock = 0;
	    ಯದಿ (from->vval.v_list == ರಿಕ್ತ)
		to->vval.v_list = ರಿಕ್ತ;
	    ಅನ್ಯಥಾ ಯದಿ (copyID != 0 && from->vval.v_list->lv_copyID == copyID)
	    {
		// use the copy made earlier
		to->vval.v_list = from->vval.v_list->lv_copylist;
		++to->vval.v_list->lv_refcount;
	    }
	    ಅನ್ಯಥಾ
	    {
		to->vval.v_list = list_copy(from->vval.v_list,
							    deep, ತುದಿ, copyID);
		ಯದಿ (to->vval.v_list == ರಿಕ್ತ)
		    ret = FAIL;
	    }
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_TUPLE:
	    to->v_type = VAR_TUPLE;
	    to->v_lock = 0;
	    ಯದಿ (from->vval.v_tuple == ರಿಕ್ತ)
		to->vval.v_tuple = ರಿಕ್ತ;
	    ಅನ್ಯಥಾ ಯದಿ (copyID != 0 && from->vval.v_tuple->tv_copyID == copyID)
	    {
		// use the copy made earlier
		to->vval.v_tuple = from->vval.v_tuple->tv_copytuple;
		++to->vval.v_tuple->tv_refcount;
	    }
	    ಅನ್ಯಥಾ
	    {
		to->vval.v_tuple = tuple_copy(from->vval.v_tuple,
							    deep, ತುದಿ, copyID);
		ಯದಿ (to->vval.v_tuple == ರಿಕ್ತ)
		    ret = FAIL;
	    }
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_BLOB:
	    to->v_type = VAR_BLOB;
	    to->v_lock = 0;
	    ಯದಿ (from->vval.v_blob == ರಿಕ್ತ)
		to->vval.v_blob = ರಿಕ್ತ;
	    ಅನ್ಯಥಾ
	    {
		to->vval.v_blob = blob_copy(from->vval.v_blob);
		ಯದಿ (to->vval.v_blob == ರಿಕ್ತ)
		    ret = FAIL;
	    }
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_DICT:
	    to->v_type = VAR_DICT;
	    to->v_lock = 0;
	    ಯದಿ (from->vval.v_dict == ರಿಕ್ತ)
		to->vval.v_dict = ರಿಕ್ತ;
	    ಅನ್ಯಥಾ ಯದಿ (copyID != 0 && from->vval.v_dict->dv_copyID == copyID)
	    {
		// use the copy made earlier
		to->vval.v_dict = from->vval.v_dict->dv_copydict;
		++to->vval.v_dict->dv_refcount;
	    }
	    ಅನ್ಯಥಾ
	    {
		to->vval.v_dict = dict_copy(from->vval.v_dict,
							    deep, ತುದಿ, copyID);
		ಯದಿ (to->vval.v_dict == ರಿಕ್ತ)
		    ret = FAIL;
	    }
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_UNKNOWN:
	ಪಕ್ಷ VAR_ANY:
	ಪಕ್ಷ VAR_VOID:
	    internal_error_no_abort("item_copy(UNKNOWN)");
	    ret = FAIL;
    }
    --recurse;
    ಪ್ರತ್ಯಾಗಮ ret;
}

    ಶೂನ್ಯ
echo_one(typval_T *rettv, ಪೂರ್ಣಾಙ್ಕ with_space, ಪೂರ್ಣಾಙ್ಕ *atstart, ಪೂರ್ಣಾಙ್ಕ *needclr)
{
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    char_u	*p = echo_string(rettv, &tofree, numbuf, get_copyID());

    ಯದಿ (*atstart)
    {
	*atstart = FALSE;
	// Call msg_start() after eval1(), evaluating the expression
	// may cause a message to appear.
	ಯದಿ (with_space)
	{
	    // Mark the saved text as finishing the line, so that what
	    // follows is displayed on a new line when scrolling back
	    // at the more prompt.
	    ಸಂದೇಶ_sb_eol();
	    ಸಂದೇಶ_start();
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (with_space)
	ಸಂದೇಶ_puts_attr(" ", echo_attr);

    ಯದಿ (p != ರಿಕ್ತ)
	ಪ್ರತಿ ( ; *p != NUL && !got_int; ++p)
	{
	    ಯದಿ (*p == '\n' || *p == '\r' || *p == TAB)
	    {
		ಯದಿ (*p != TAB && *needclr)
		{
		    // remove any text still there from the command
		    ಸಂದೇಶ_clr_eos();
		    *needclr = FALSE;
		}
		ಸಂದೇಶ_putchar_attr(*p, echo_attr);
	    }
	    ಅನ್ಯಥಾ
	    {
		ಯದಿ (has_mbyte)
		{
		    ಪೂರ್ಣಾಙ್ಕ i = (*mb_ptr2len)(p);

		    (ಶೂನ್ಯ)ಸಂದೇಶ_outtrans_len_attr(p, i, echo_attr);
		    p += i - 1;
		}
		ಅನ್ಯಥಾ
		    (ಶೂನ್ಯ)ಸಂದೇಶ_outtrans_len_attr(p, 1, echo_attr);
	    }
	}
    vim_free(tofree);
}

/*
 * ":echo expr1 ..."	print each argument separated with a space, add a
 *			newline at the end.
 * ":echon expr1 ..."	print each argument plain.
 */
    ಶೂನ್ಯ
ex_echo(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    typval_T	rettv;
    char_u	*arg_start;
    ಪೂರ್ಣಾಙ್ಕ		needclr = TRUE;
    ಪೂರ್ಣಾಙ್ಕ		atstart = TRUE;
    ಪೂರ್ಣಾಙ್ಕ		did_emsg_before = did_emsg;
    ಪೂರ್ಣಾಙ್ಕ		called_emsg_before = called_emsg;
    evalarg_T	evalarg;

    fill_evalarg_from_eap(&evalarg, eap, eap->skip);

    ಯದಿ (eap->skip)
	++emsg_skip;
    ಯಾವತ್ ((!ends_excmd2(eap->cmd, arg) || *arg == '"') && !got_int)
    {
	// If eval1() causes an error message the text from the command may
	// still need to be cleared. E.g., "echo 22,44".
	need_clr_eos = needclr;

	arg_start = arg;
	ಯದಿ (eval1(&arg, &rettv, &evalarg) == FAIL)
	{
	    /*
	     * Report the invalid expression unless the expression evaluation
	     * has been cancelled due to an aborting error, an interrupt, or an
	     * exception.
	     */
	    ಯದಿ (!aborting() && did_emsg == did_emsg_before
					  && called_emsg == called_emsg_before)
		semsg(_(e_invalid_expression_str), arg_start);
	    need_clr_eos = FALSE;
	    ಭಙ್ಗ;
	}
	need_clr_eos = FALSE;

	ಯದಿ (!eap->skip)
	{
	    ಯದಿ (rettv.v_type == VAR_VOID)
	    {
		semsg(_(e_expression_does_not_result_in_ಮೌಲ್ಯ_str), arg_start);
		ಭಙ್ಗ;
	    }
	    echo_one(&rettv, eap->cmdidx == CMD_echo, &atstart, &needclr);
	}

	clear_tv(&rettv);
	arg = skipwhite(arg);
    }
    set_nextcmd(eap, arg);
    clear_evalarg(&evalarg, eap);

    ಯದಿ (eap->skip)
	--emsg_skip;
    ಅನ್ಯಥಾ
    {
	// remove text that may still be there from the command
	ಯದಿ (needclr)
	    ಸಂದೇಶ_clr_eos();
	ಯದಿ (eap->cmdidx == CMD_echo)
	    ಸಂದೇಶ_end();
    }
}

/*
 * ":echohl {name}".
 */
    ಶೂನ್ಯ
ex_echohl(exarg_T *eap)
{
    echo_attr = syn_name2attr(eap->arg);
}

/*
 * Returns the :echo attribute
 */
    ಪೂರ್ಣಾಙ್ಕ
get_echo_attr(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ echo_attr;
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
    ಶೂನ್ಯ
ex_execute(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    typval_T	rettv;
    ಪೂರ್ಣಾಙ್ಕ		ret = OK;
    char_u	*p;
    garray_T	ga;
    ಪೂರ್ಣಾಙ್ಕ		len;
    long	start_lnum = SOURCING_LNUM;

    ga_init2(&ga, 1, 80);

    ಯದಿ (eap->skip)
	++emsg_skip;
    ಯಾವತ್ (!ends_excmd2(eap->cmd, arg) || *arg == '"')
    {
	ret = eval1_emsg(&arg, &rettv, eap);
	ಯದಿ (ret == FAIL)
	    ಭಙ್ಗ;

	ಯದಿ (!eap->skip)
	{
	    char_u   buf[NUMBUFLEN];

	    ಯದಿ (eap->cmdidx == CMD_execute)
	    {
		ಯದಿ (rettv.v_type == VAR_CHANNEL || rettv.v_type == VAR_JOB)
		{
		    semsg(_(e_using_invalid_ಮೌಲ್ಯ_as_string_str),
						  vartype_name(rettv.v_type));
		    p = ರಿಕ್ತ;
		}
		ಅನ್ಯಥಾ
		    p = tv_get_string_buf(&rettv, buf);
	    }
	    ಅನ್ಯಥಾ
		p = tv_stringify(&rettv, buf);
	    ಯದಿ (p == ರಿಕ್ತ)
	    {
		clear_tv(&rettv);
		ret = FAIL;
		ಭಙ್ಗ;
	    }
	    len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(p);
	    ಯದಿ (ga_grow(&ga, len + 2) == FAIL)
	    {
		clear_tv(&rettv);
		ret = FAIL;
		ಭಙ್ಗ;
	    }
	    ಯದಿ (ga.ga_len)
		((char_u *)(ga.ga_data))[ga.ga_len++] = ' ';
	    STRCPY((char_u *)(ga.ga_data) + ga.ga_len, p);
	    ga.ga_len += len;
	}

	clear_tv(&rettv);
	arg = skipwhite(arg);
    }

    ಯದಿ (ret != FAIL && ga.ga_data != ರಿಕ್ತ)
    {
	// use the first line of continuation lines for messages
	SOURCING_LNUM = start_lnum;

	ಯದಿ (eap->cmdidx == CMD_echomsg
		|| eap->cmdidx == CMD_echowindow
		|| eap->cmdidx == CMD_echoerr)
	{
	    // Mark the already saved text as finishing the line, so that what
	    // follows is displayed on a new line when scrolling back at the
	    // more prompt.
	    ಸಂದೇಶ_sb_eol();
	}

	ಯದಿ (eap->cmdidx == CMD_echomsg)
	{
	    ಸಂದೇಶ_attr(ga.ga_data, echo_attr);
	    out_flush();
	}
	ಅನ್ಯಥಾ ಯದಿ (eap->cmdidx == CMD_echowindow)
	{
#ifdef HAS_MESSAGE_WINDOW
	    start_echowindow(eap->addr_ಎಣಿಕೆ > 0 ? eap->line2 : 0);
#endif
	    ಸಂದೇಶ_attr(ga.ga_data, echo_attr);
#ifdef HAS_MESSAGE_WINDOW
	    end_echowindow();
#endif
	}
	ಅನ್ಯಥಾ ಯದಿ (eap->cmdidx == CMD_echoconsole)
	{
	    ui_write(ga.ga_data, (ಪೂರ್ಣಾಙ್ಕ)STRLEN(ga.ga_data), TRUE);
	    ui_write((char_u *)"\r\n", 2, TRUE);
	}
	ಅನ್ಯಥಾ ಯದಿ (eap->cmdidx == CMD_echoerr)
	{
	    ಪೂರ್ಣಾಙ್ಕ		save_did_emsg = did_emsg;

	    // We don't want to abort following commands, restore did_emsg.
	    emsg(ga.ga_data);
	    ಯದಿ (!force_abort)
		did_emsg = save_did_emsg;
	}
	ಅನ್ಯಥಾ ಯದಿ (eap->cmdidx == CMD_execute)
	{
	    ಪೂರ್ಣಾಙ್ಕ save_sticky_cmdmod_flags = sticky_cmdmod_flags;

	    // "legacy exe cmd" and "vim9cmd exe cmd" applies to "cmd".
	    sticky_cmdmod_flags = cmdmod.cmod_flags
						& (CMOD_LEGACY | CMOD_VIM9CMD);
	    do_cmdline((char_u *)ga.ga_data,
		       eap->ea_getline, eap->cookie, DOCMD_NOWAIT|DOCMD_VERBOSE);
	    sticky_cmdmod_flags = save_sticky_cmdmod_flags;
	}
    }

    ga_clear(&ga);

    ಯದಿ (eap->skip)
	--emsg_skip;
    set_nextcmd(eap, arg);
}

/*
 * Skip over the name of an option: "&option", "&g:option" or "&l:option".
 * "arg" points to the "&" or '+' when called, to "option" when returning.
 * Returns NULL when no option name found.  Otherwise pointer to the char
 * after the option name.
 */
    char_u *
find_option_end(char_u **arg, ಪೂರ್ಣಾಙ್ಕ *scope)
{
    char_u	*p = *arg;

    ++p;
    ಯದಿ (*p == 'g' && p[1] == ':')
    {
	*scope = OPT_GLOBAL;
	p += 2;
    }
    ಅನ್ಯಥಾ ಯದಿ (*p == 'l' && p[1] == ':')
    {
	*scope = OPT_LOCAL;
	p += 2;
    }
    ಅನ್ಯಥಾ
	*scope = 0;

    ಯದಿ (!ASCII_ISALPHA(*p))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    *arg = p;

    ಯದಿ (p[0] == 't' && p[1] == '_' && p[2] != NUL && p[3] != NUL)
	p += 4;	    // termcap option
    ಅನ್ಯಥಾ
	ಯಾವತ್ (ASCII_ISALPHA(*p))
	    ++p;
    ಪ್ರತ್ಯಾಗಮ p;
}

/*
 * Display script name where an item was last set.
 * Should only be invoked when 'verbose' is non-zero.
 */
    ಶೂನ್ಯ
last_set_ಸಂದೇಶ(sctx_T script_ctx)
{
    char_u *p;

    ಯದಿ (script_ctx.sc_sid == 0)
	ಪ್ರತ್ಯಾಗಮ;

    p = home_replace_save(ರಿಕ್ತ, get_scriptname(script_ctx.sc_sid));
    ಯದಿ (p == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    verbose_enter();
    ಸಂದೇಶ_puts(_("\n\tLast set from "));
    ಸಂದೇಶ_puts((ವರ್ಣ *)p);
    ಯದಿ (script_ctx.sc_lnum > 0)
    {
	ಸಂದೇಶ_puts(_(ಸಾಲು_ಸಂದೇಶ));
	ಸಂದೇಶ_outnum((long)script_ctx.sc_lnum);
    }
    verbose_leave();
    vim_free(p);
}

#endif // FEAT_EVAL

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
    typval_T	*ಅಭಿವ್ಯಕ್ತಿ,
    char_u	*flags,
    size_t	*ret_len)		// length of returned buffer
{
    regmatch_T	regmatch;
    garray_T	ga;
    char_u	*ret;
    char_u	*save_cpo;

    // Make 'cpoptions' empty, so that the 'l' flag doesn't work here
    save_cpo = p_cpo;
    p_cpo = empty_option;

    ga_init2(&ga, 1, 200);

    regmatch.rm_ic = p_ic;
    regmatch.regprog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
    ಯದಿ (regmatch.regprog != ರಿಕ್ತ)
    {
	char_u	*tail = str;
	char_u	*end = str + len;
	ಪೂರ್ಣಾಙ್ಕ	do_all = (flags[0] == 'g');
	ಪೂರ್ಣಾಙ್ಕ	sublen;
	ಪೂರ್ಣಾಙ್ಕ	i;
	char_u	*zero_width = ರಿಕ್ತ;

	ಯಾವತ್ (vim_regexec_nl(&regmatch, str, (colnr_T)(tail - str)))
	{
	    // Skip empty match except for first match.
	    ಯದಿ (regmatch.startp[0] == regmatch.endp[0])
	    {
		ಯದಿ (zero_width == regmatch.startp[0])
		{
		    // avoid getting stuck on a match with an empty string
		    i = mb_ptr2len(tail);
		    mch_memmove((char_u *)ga.ga_data + ga.ga_len, tail,
								   (size_t)i);
		    ga.ga_len += i;
		    tail += i;
		    ಅನುವರ್ತ;
		}
		zero_width = regmatch.startp[0];
	    }

	    /*
	     * Get some space for a temporary buffer to do the substitution
	     * into.  It will contain:
	     * - The text up to where the match is.
	     * - The substituted text.
	     * - The text after the match.
	     */
	    sublen = vim_regsub(&regmatch, sub, ಅಭಿವ್ಯಕ್ತಿ, tail, 0, REGSUB_MAGIC);
	    ಯದಿ (sublen <= 0)
	    {
		ga_clear(&ga);
		ಭಙ್ಗ;
	    }
	    ಯದಿ (ga_grow(&ga, (ಪೂರ್ಣಾಙ್ಕ)((end - tail) + sublen -
			    (regmatch.endp[0] - regmatch.startp[0]))) == FAIL)
	    {
		ga_clear(&ga);
		ಭಙ್ಗ;
	    }

	    // copy the text up to where the match is
	    i = (ಪೂರ್ಣಾಙ್ಕ)(regmatch.startp[0] - tail);
	    mch_memmove((char_u *)ga.ga_data + ga.ga_len, tail, (size_t)i);
	    // add the substituted text
	    (ಶೂನ್ಯ)vim_regsub(&regmatch, sub, ಅಭಿವ್ಯಕ್ತಿ,
				  (char_u *)ga.ga_data + ga.ga_len + i, sublen,
				  REGSUB_COPY | REGSUB_MAGIC);
	    ga.ga_len += i + sublen - 1;
	    tail = regmatch.endp[0];
	    ಯದಿ (*tail == NUL)
		ಭಙ್ಗ;
	    ಯದಿ (!do_all)
		ಭಙ್ಗ;
	}

	ಯದಿ (ga.ga_data != ರಿಕ್ತ)
	{
	    STRCPY((ವರ್ಣ *)ga.ga_data + ga.ga_len, tail);
	    ga.ga_len += (ಪೂರ್ಣಾಙ್ಕ)(end - tail);
	}

	vim_regfree(regmatch.regprog);
    }

    ಯದಿ (ga.ga_data != ರಿಕ್ತ)
    {
	str = (char_u *)ga.ga_data;
	len = (size_t)ga.ga_len;
    }
    ret = vim_strnsave(str, len);
    ga_clear(&ga);
    ಯದಿ (p_cpo == empty_option)
	p_cpo = save_cpo;
    ಅನ್ಯಥಾ
    {
	// Darn, evaluating {sub} expression or {expr} changed the value.
	// If it's still empty it was changed and restored, need to restore in
	// the complicated way.
	ಯದಿ (*p_cpo == NUL)
	    set_option_ಮೌಲ್ಯ_give_err((char_u *)"cpo", 0L, save_cpo, 0);
	free_string_option(save_cpo);
    }

    ಯದಿ (ret_len != ರಿಕ್ತ)
	*ret_len = len;

    ಪ್ರತ್ಯಾಗಮ ret;
}
