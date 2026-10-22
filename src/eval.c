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
#ವ್ಯಾಖ್ಯಾನಿಸು USING_FLOAT_STUFF

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಆದರೆ defined(FEAT_EVAL)

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VMS
# include <float.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು NAMESPACE_CHAR	(char_u *)"abglstvw"

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ eval0_simple_funccal(char_u *arg, typval_T *rettv, exarg_T *eap, evalarg_T *evalarg);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ eval2(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ eval3(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ eval4(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ eval5(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ eval6(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ eval7(char_u **arg, typval_T *rettv, evalarg_T *evalarg, ಪೂರ್ಣಾಂಕ want_string);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ eval8(char_u **arg, typval_T *rettv, evalarg_T *evalarg, ಪೂರ್ಣಾಂಕ want_string);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ eval9(char_u **arg, typval_T *rettv, evalarg_T *evalarg, ಪೂರ್ಣಾಂಕ want_string);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ eval9_leader(typval_T *rettv, ಪೂರ್ಣಾಂಕ numeric_only, char_u *ಆರಂಭ_leader, char_u **ಕೊನೆ_leaderp);

ಸ್ಥಿರ char_u *make_expanded_name(char_u *ಒಳ_ಆರಂಭ, char_u *ಅಭಿವ್ಯಕ್ತಿ_ಆರಂಭ, char_u *ಅಭಿವ್ಯಕ್ತಿ_ಕೊನೆ, char_u *ಒಳ_ಕೊನೆ);

/*
 * Return "n1" divided by "n2", taking care of dividing by zero.
 * If "failed" is not NULL set it to TRUE when dividing by zero fails.
 */
	varnumber_T
ಅಂಕೆ_divide(varnumber_T n1, varnumber_T n2, ಪೂರ್ಣಾಂಕ *failed)
{
    varnumber_T	ಫಲಿತಾಂಶ;

    ಆದರೆ (n2 == 0)
    {
	ಆದರೆ (ಒಳ_vim9script())
	{
	    emsg(_(e_divide_by_zero));
	    ಆದರೆ (failed != ಖಾಲಿ)
		*failed = TRUE;
	}
	ಆದರೆ (n1 == 0)
	    ಫಲಿತಾಂಶ = VARNUM_MIN; // similar to NaN
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (n1 < 0)
	    ಫಲಿತಾಂಶ = -VARNUM_MAX;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಫಲಿತಾಂಶ = VARNUM_MAX;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (n1 == VARNUM_MIN && n2 == -1)
    {
	// specific case: trying to do VARNUM_MIN / -1 results in a positive
	// number that doesn't fit in varnumber_T and causes an FPE
	ಫಲಿತಾಂಶ = VARNUM_MAX;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ಫಲಿತಾಂಶ = n1 / n2;

    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * Return "n1" modulus "n2", taking care of dividing by zero.
 * If "failed" is not NULL set it to TRUE when dividing by zero fails.
 */
	varnumber_T
ಅಂಕೆ_modulus(varnumber_T n1, varnumber_T n2, ಪೂರ್ಣಾಂಕ *failed)
{
    ಆದರೆ (n2 == 0 && ಒಳ_vim9script())
    {
	emsg(_(e_divide_by_zero));
	ಆದರೆ (failed != ಖಾಲಿ)
	    *failed = TRUE;
    }
    ಹಿಂತಿರುಗಿ (n2 == 0) ? 0 : (n1 % n2);
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

#ಆದರೆ defined(EXITFREE)
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
#ಆದರೆ_ಅಂತ್ಯ

    ಶೂನ್ಯ
fill_evalarg_from_eap(evalarg_T *evalarg, exarg_T *eap, ಪೂರ್ಣಾಂಕ skip)
{
    init_evalarg(evalarg);
    evalarg->eval_flags = skip ? 0 : EVAL_EVALUATE;

    ಆದರೆ (eap == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    evalarg->eval_cstack = eap->cstack;
    ಆದರೆ (sourcing_a_script(eap) || eap->ea_getline == get_list_ಸಾಲು)
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
    ಪೂರ್ಣಾಂಕ
eval_ಗಮ್ಯ_bool(
    char_u	*arg,
    ಪೂರ್ಣಾಂಕ		*error,
    exarg_T	*eap,
    ಪೂರ್ಣಾಂಕ		skip,	    // only parse, don't execute
    ಪೂರ್ಣಾಂಕ		use_simple_function)
{
    typval_T	tv;
    varnumber_T	retval = FALSE;
    evalarg_T	evalarg;
    ಪೂರ್ಣಾಂಕ		r;

    fill_evalarg_from_eap(&evalarg, eap, skip);

    ಆದರೆ (skip)
	++emsg_skip;
    ಆದರೆ (use_simple_function)
	r = eval0_simple_funccal(arg, &tv, eap, &evalarg);
    ಇಲ್ಲದಿದ್ದರೆ
	r = eval0(arg, &tv, eap, &evalarg);
    ಆದರೆ (r == FAIL)
	*error = TRUE;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	*error = FALSE;
	ಆದರೆ (!skip)
	{
	    ಆದರೆ (ಒಳ_vim9script())
		retval = tv_get_bool_chk(&tv, error);
	    ಇಲ್ಲದಿದ್ದರೆ
		retval = (tv_get_ಸಂಖ್ಯೆ_chk(&tv, error) != 0);
	    clear_tv(&tv);
	}
    }
    ಆದರೆ (skip)
	--emsg_skip;
    clear_evalarg(&evalarg, eap);

    ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)retval;
}

/*
 * Call eval1() and give an error message if not done at a lower level.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
eval1_emsg(char_u **arg, typval_T *rettv, exarg_T *eap)
{
    char_u	*ಆರಂಭ = *arg;
    ಪೂರ್ಣಾಂಕ		ret;
    ಪೂರ್ಣಾಂಕ		did_emsg_before = did_emsg;
    ಪೂರ್ಣಾಂಕ		called_emsg_before = called_emsg;
    evalarg_T	evalarg;

    fill_evalarg_from_eap(&evalarg, eap, eap != ಖಾಲಿ && eap->skip);

    ret = eval1(arg, rettv, &evalarg);
    ಆದರೆ (ret == FAIL)
    {
	// Report the invalid expression unless the expression evaluation has
	// been cancelled due to an aborting error, an interrupt, or an
	// exception, or we already gave a more specific error.
	// Also check called_emsg for when using assert_fails().
	ಆದರೆ (!aborting() && did_emsg == did_emsg_before
					  && called_emsg == called_emsg_before)
	    semsg(_(e_invalid_ಅಭಿವ್ಯಕ್ತಿ_str), ಆರಂಭ);
    }
    clear_evalarg(&evalarg, eap);
    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Return whether a typval is a valid expression to pass to eval_expr_typval()
 * or eval_expr_to_bool().  An empty string returns FALSE;
 */
    ಪೂರ್ಣಾಂಕ
eval_ಅಭಿವ್ಯಕ್ತಿ_valid_arg(typval_T *tv)
{
    ಹಿಂತಿರುಗಿ tv->v_type != VAR_UNKNOWN
	    && (tv->v_type != VAR_STRING
		  || (tv->vval.v_string != ಖಾಲಿ && *tv->vval.v_string != NUL));
}

/*
 * When calling eval_expr_typval() many times we only need one funccall_T.
 * Returns NULL when no funccall_T is to be used.
 * When returning non-NULL remove_funccal() must be called later.
 */
    funccall_T *
eval_ಅಭಿವ್ಯಕ್ತಿ_get_funccal(typval_T *ಅಭಿವ್ಯಕ್ತಿ, typval_T *rettv)
{
    ಆದರೆ (ಅಭಿವ್ಯಕ್ತಿ->v_type != VAR_PARTIAL)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    partial_T *partial = ಅಭಿವ್ಯಕ್ತಿ->vval.v_partial;
    ಆದರೆ (partial == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಆದರೆ (partial->pt_func == ಖಾಲಿ
	    || partial->pt_func->uf_def_ಸ್ಥಿತಿ == UF_NOT_COMPILED)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಹಿಂತಿರುಗಿ create_funccal(partial->pt_func, rettv);
}

/*
 * Evaluate a partial.
 * Pass arguments "argv[argc]".
 * "fc_arg" is from eval_expr_get_funccal() or NULL;
 * Return the result in "rettv" and OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
eval_ಅಭಿವ್ಯಕ್ತಿ_partial(
    typval_T	*ಅಭಿವ್ಯಕ್ತಿ,
    typval_T	*argv,
    ಪೂರ್ಣಾಂಕ		argc,
    funccall_T	*fc_arg,
    typval_T	*rettv)
{
    partial_T	*partial = ಅಭಿವ್ಯಕ್ತಿ->vval.v_partial;

    ಆದರೆ (partial == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (partial->pt_func != ಖಾಲಿ
			&& partial->pt_func->uf_def_ಸ್ಥಿತಿ != UF_NOT_COMPILED)
    {
	funccall_T	*fc = fc_arg != ಖಾಲಿ ? fc_arg
				: create_funccal(partial->pt_func, rettv);
	ಪೂರ್ಣಾಂಕ		r;

	ಆದರೆ (fc == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;

	// Shortcut to call a compiled function with minimal overhead.
	ಆದರೆ (partial->pt_obj != ಖಾಲಿ)
	    partial->pt_obj->obj_refcount++;
	r = call_def_function(partial->pt_func, argc, argv, DEF_USE_PT_ARGV,
					partial, partial->pt_obj, fc, rettv);
	ಆದರೆ (fc_arg == ಖಾಲಿ)
	    remove_funccal();
	ಆದರೆ (r == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	char_u		*s = partial_name(partial);
	funcexe_T	funcexe;

	ಆದರೆ (s == ಖಾಲಿ || *s == NUL)
	    ಹಿಂತಿರುಗಿ FAIL;

	CLEAR_ಕ್ಷೇತ್ರ(funcexe);
	funcexe.fe_evaluate = TRUE;
	funcexe.fe_partial = partial;
	ಆದರೆ (call_func(s, -1, rettv, argc, argv, &funcexe) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Evaluate an expression which is a function.
 * Pass arguments "argv[argc]".
 * Return the result in "rettv" and OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
eval_ಅಭಿವ್ಯಕ್ತಿ_func(
    typval_T	*ಅಭಿವ್ಯಕ್ತಿ,
    typval_T	*argv,
    ಪೂರ್ಣಾಂಕ		argc,
    typval_T	*rettv)
{
    funcexe_T	funcexe;
    char_u	buf[NUMBUFLEN];
    char_u	*s;

    ಆದರೆ (ಅಭಿವ್ಯಕ್ತಿ->v_type == VAR_FUNC)
	s = ಅಭಿವ್ಯಕ್ತಿ->vval.v_string;
    ಇಲ್ಲದಿದ್ದರೆ
	s = tv_get_string_buf_chk_strict(ಅಭಿವ್ಯಕ್ತಿ, buf, ಒಳ_vim9script());
    ಆದರೆ (s == ಖಾಲಿ || *s == NUL)
	ಹಿಂತಿರುಗಿ FAIL;

    CLEAR_ಕ್ಷೇತ್ರ(funcexe);
    funcexe.fe_evaluate = TRUE;
    ಆದರೆ (call_func(s, -1, rettv, argc, argv, &funcexe) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Evaluate an expression, which is a string.
 * Return the result in "rettv" and OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
eval_ಅಭಿವ್ಯಕ್ತಿ_string(
    typval_T	*ಅಭಿವ್ಯಕ್ತಿ,
    typval_T	*rettv)
{
    char_u	*s;
    char_u	buf[NUMBUFLEN];

    s = tv_get_string_buf_chk_strict(ಅಭಿವ್ಯಕ್ತಿ, buf, ಒಳ_vim9script());
    ಆದರೆ (s == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    s = skipwhite(s);
    ಆದರೆ (eval1_emsg(&s, rettv, ಖಾಲಿ) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (*skipwhite(s) != NUL)  // check for trailing chars after expr
    {
	clear_tv(rettv);
	semsg(_(e_invalid_ಅಭಿವ್ಯಕ್ತಿ_str), s);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Evaluate an expression, which can be a function, partial or string.
 * Pass arguments "argv[argc]".
 * If "want_func" is TRUE treat a string as a function name, not an expression.
 * "fc_arg" is from eval_expr_get_funccal() or NULL;
 * Return the result in "rettv" and OK or FAIL.
 */
    ಪೂರ್ಣಾಂಕ
eval_ಅಭಿವ್ಯಕ್ತಿ_typval(
	typval_T    *ಅಭಿವ್ಯಕ್ತಿ,
	ಪೂರ್ಣಾಂಕ	    want_func,
	typval_T    *argv,
	ಪೂರ್ಣಾಂಕ	    argc,
	funccall_T  *fc_arg,
	typval_T    *rettv)
{
    ಆದರೆ (ಅಭಿವ್ಯಕ್ತಿ->v_type == VAR_PARTIAL)
	ಹಿಂತಿರುಗಿ eval_ಅಭಿವ್ಯಕ್ತಿ_partial(ಅಭಿವ್ಯಕ್ತಿ, argv, argc, fc_arg, rettv);
    ಆದರೆ (ಅಭಿವ್ಯಕ್ತಿ->v_type == VAR_INSTR)
	ಹಿಂತಿರುಗಿ exe_typval_instr(ಅಭಿವ್ಯಕ್ತಿ, rettv);
    ಆದರೆ (ಅಭಿವ್ಯಕ್ತಿ->v_type == VAR_FUNC || want_func)
	ಹಿಂತಿರುಗಿ eval_ಅಭಿವ್ಯಕ್ತಿ_func(ಅಭಿವ್ಯಕ್ತಿ, argv, argc, rettv);

    ಹಿಂತಿರುಗಿ eval_ಅಭಿವ್ಯಕ್ತಿ_string(ಅಭಿವ್ಯಕ್ತಿ, rettv);
}

/*
 * Like eval_to_bool() but using a typval_T instead of a string.
 * Works for string, funcref and partial.
 */
    ಪೂರ್ಣಾಂಕ
eval_ಅಭಿವ್ಯಕ್ತಿ_ಗಮ್ಯ_bool(typval_T *ಅಭಿವ್ಯಕ್ತಿ, ಪೂರ್ಣಾಂಕ *error)
{
    typval_T	rettv;
    ಪೂರ್ಣಾಂಕ		res;

    ಆದರೆ (eval_ಅಭಿವ್ಯಕ್ತಿ_typval(ಅಭಿವ್ಯಕ್ತಿ, FALSE, ಖಾಲಿ, 0, ಖಾಲಿ, &rettv) == FAIL)
    {
	*error = TRUE;
	ಹಿಂತಿರುಗಿ FALSE;
    }
    res = (tv_get_bool_chk(&rettv, error) != 0);
    clear_tv(&rettv);
    ಹಿಂತಿರುಗಿ res;
}

/*
 * Top level evaluation function, returning a string.  If "skip" is TRUE,
 * only parsing to "nextcmd" is done, without reporting errors.  Return
 * pointer to allocated memory, or NULL for failure or when "skip" is TRUE.
 */
    char_u *
eval_ಗಮ್ಯ_string_skip(
    char_u	*arg,
    exarg_T	*eap,
    ಪೂರ್ಣಾಂಕ		skip)	    // only parse, don't execute
{
    typval_T	tv;
    char_u	*retval;
    evalarg_T	evalarg;

    fill_evalarg_from_eap(&evalarg, eap, skip);
    ಆದರೆ (skip)
	++emsg_skip;
    ಆದರೆ (eval0(arg, &tv, eap, &evalarg) == FAIL || skip)
	retval = ಖಾಲಿ;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	retval = vim_strsave(tv_get_string(&tv));
	clear_tv(&tv);
    }
    ಆದರೆ (skip)
	--emsg_skip;
    clear_evalarg(&evalarg, eap);

    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Initialize "evalarg" for use.
 */
    ಶೂನ್ಯ
init_evalarg(evalarg_T *evalarg)
{
    CLEAR_ಸೂಚಕ(evalarg);
    ga_init2(&evalarg->eval_tofree_ga, ಗಾತ್ರ(char_u *), 20);
}

/*
 * If "evalarg->eval_tofree" is not NULL free it later.
 * Caller is expected to overwrite "evalarg->eval_tofree" next.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
free_eval_tofree_later(evalarg_T *evalarg)
{
    ಆದರೆ (evalarg->eval_tofree == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ga_grow(&evalarg->eval_tofree_ga, 1) == OK)
	((char_u **)evalarg->eval_tofree_ga.ga_ದತ್ತಾಂಶ)
	    [evalarg->eval_tofree_ga.ga_len++]
	    = evalarg->eval_tofree;
    ಇಲ್ಲದಿದ್ದರೆ
	vim_free(evalarg->eval_tofree);
}

/*
 * After using "evalarg" filled from "eap": free the memory.
 */
    ಶೂನ್ಯ
clear_evalarg(evalarg_T *evalarg, exarg_T *eap)
{
    ಆದರೆ (evalarg == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    garray_T *etga = &evalarg->eval_tofree_ga;

    ಆದರೆ (evalarg->eval_tofree != ಖಾಲಿ || evalarg->eval_using_cmdline)
    {
	ಆದರೆ (eap != ಖಾಲಿ)
	{
	    // We may need to keep the original command line, e.g. for
	    // ":let" it has the variable names.  But we may also need
	    // the new one, "nextcmd" points into it.  Keep both.
	    vim_free(eap->cmdline_tofree);
	    eap->cmdline_tofree = *eap->cmdlinep;

	    ಆದರೆ (evalarg->eval_using_cmdline && etga->ga_len > 0)
	    {
		// "nextcmd" points into the last line in eval_tofree_ga,
		// need to keep it around.
		--etga->ga_len;
		*eap->cmdlinep = ((char_u **)etga->ga_ದತ್ತಾಂಶ)[etga->ga_len];
		vim_free(evalarg->eval_tofree);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		*eap->cmdlinep = evalarg->eval_tofree;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    vim_free(evalarg->eval_tofree);
	evalarg->eval_tofree = ಖಾಲಿ;
    }

    ga_clear_ಅಕ್ಷರಸರಣಿಗಳು(etga);
    VIM_CLEAR(evalarg->eval_tofree_lambda);
}

/*
 * Skip over an expression at "*pp".
 * Return FAIL for an error, OK otherwise.
 */
    ಪೂರ್ಣಾಂಕ
skip_ಅಭಿವ್ಯಕ್ತಿ(char_u **pp, evalarg_T *evalarg)
{
    typval_T	rettv;

    *pp = skipwhite(*pp);
    ಹಿಂತಿರುಗಿ eval1(pp, &rettv, evalarg);
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
    ಪೂರ್ಣಾಂಕ
skip_ಅಭಿವ್ಯಕ್ತಿ_concatenate(
	char_u	    **arg,
	char_u	    **ಆರಂಭ,
	char_u	    **ಕೊನೆ,
	evalarg_T   *evalarg)
{
    typval_T	rettv;
    ಪೂರ್ಣಾಂಕ		res;
    ಪೂರ್ಣಾಂಕ		vim9script = ಒಳ_vim9script();
    garray_T    *gap = evalarg == ಖಾಲಿ ? ಖಾಲಿ : &evalarg->eval_ga;
    garray_T    *freegap = evalarg == ಖಾಲಿ ? ಖಾಲಿ : &evalarg->eval_freega;
    ಪೂರ್ಣಾಂಕ		save_flags = evalarg == ಖಾಲಿ ? 0 : evalarg->eval_flags;
    ಪೂರ್ಣಾಂಕ		evaluate = evalarg == ಖಾಲಿ
			       ? FALSE : (evalarg->eval_flags & EVAL_EVALUATE);

    ಆದರೆ (vim9script && evaluate
	       && (evalarg->eval_cookie != ಖಾಲಿ || evalarg->eval_cctx != ಖಾಲಿ))
    {
	ga_init2(gap, ಗಾತ್ರ(char_u *), 10);
	// leave room for "start"
	ಆದರೆ (ga_grow(gap, 1) == OK)
	    ++gap->ga_len;
	ga_init2(freegap, ಗಾತ್ರ(char_u *), 10);
    }
    *ಆರಂಭ = *arg;

    // Don't evaluate the expression.
    ಆದರೆ (evalarg != ಖಾಲಿ)
	evalarg->eval_flags &= ~EVAL_EVALUATE;
    *arg = skipwhite(*arg);
    res = eval1(arg, &rettv, evalarg);
    *ಕೊನೆ = *arg;
    ಆದರೆ (evalarg != ಖಾಲಿ)
	evalarg->eval_flags = save_flags;

    ಆದರೆ (vim9script && evaluate
	    && (evalarg->eval_cookie != ಖಾಲಿ || evalarg->eval_cctx != ಖಾಲಿ))
    {
	ಆದರೆ (evalarg->eval_ga.ga_len == 1)
	{
	    // just the one line, no need to concatenate
	    ga_clear(gap);
	    gap->ga_itemsize = 0;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    char_u	    *p;
	    size_t	    endoff = STRLEN(*arg);

	    // Line breaks encountered, concatenate all the lines.
	    *((char_u **)gap->ga_ದತ್ತಾಂಶ) = *ಆರಂಭ;
	    p = ga_concat_ಅಕ್ಷರಸರಣಿಗಳು(gap, " ");

	    // free the lines only when using getsourceline()
	    ಆದರೆ (evalarg->eval_cookie != ಖಾಲಿ)
	    {
		// Do not free the first line, the caller can still use it.
		*((char_u **)gap->ga_ದತ್ತಾಂಶ) = ಖಾಲಿ;
		// Do not free the last line, "arg" points into it, free it
		// later.  Also free "eval_tofree" later if needed.
		free_eval_tofree_later(evalarg);
		evalarg->eval_tofree =
				    ((char_u **)gap->ga_ದತ್ತಾಂಶ)[gap->ga_len - 1];
		((char_u **)gap->ga_ದತ್ತಾಂಶ)[gap->ga_len - 1] = ಖಾಲಿ;
		ga_clear_ಅಕ್ಷರಸರಣಿಗಳು(gap);
		ga_clear(freegap);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ga_clear(gap);

		// free lines that were explicitly marked for freeing
		ga_clear_ಅಕ್ಷರಸರಣಿಗಳು(freegap);
	    }

	    gap->ga_itemsize = 0;
	    ಆದರೆ (p == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ FAIL;
	    *ಆರಂಭ = p;
	    vim_free(evalarg->eval_tofree_lambda);
	    evalarg->eval_tofree_lambda = p;
	    // Compute "end" relative to the end.
	    *ಕೊನೆ = *ಆರಂಭ + STRLEN(*ಆರಂಭ) - endoff;
	}
    }

    ಹಿಂತಿರುಗಿ res;
}

/*
 * Convert "tv" to a string.
 * When "join_list" is TRUE convert a List or a Tuple into a sequence of lines.
 * Returns an allocated string (NULL when out of memory).
 */
    char_u *
typval2string(typval_T *tv, ಪೂರ್ಣಾಂಕ join_list)
{
    garray_T	ga;
    char_u	*retval = ಖಾಲಿ;

    ಆದರೆ (join_list && (tv->v_type == VAR_LIST || tv->v_type == VAR_TUPLE))
    {
	ಆದರೆ (tv->v_type == VAR_LIST)
	{
	    ga_init2(&ga, ಗಾತ್ರ(ಅಕ್ಷರ), 80);
	    ಆದರೆ (tv->vval.v_list != ಖಾಲಿ)
	    {
		list_join(&ga, tv->vval.v_list, (char_u *)"\n", TRUE, FALSE,
									0);
		ಆದರೆ (tv->vval.v_list->lv_len > 0)
		    ga_append(&ga, NL);
	    }
	    ga_append(&ga, NUL);
	    retval = (char_u *)ga.ga_ದತ್ತಾಂಶ;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // tuple
	    ga_init2(&ga, ಗಾತ್ರ(ಅಕ್ಷರ), 80);
	    ಆದರೆ (tv->vval.v_tuple != ಖಾಲಿ)
	    {
		tuple_join(&ga, tv->vval.v_tuple, (char_u *)"\n", TRUE, FALSE,
									0);
		ಆದರೆ (TUPLE_LEN(tv->vval.v_tuple) > 0)
		    ga_append(&ga, NL);
	    }
	    ga_append(&ga, NUL);
	    retval = (char_u *)ga.ga_ದತ್ತಾಂಶ;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tv->v_type == VAR_LIST
	    || tv->v_type == VAR_TUPLE
	    || tv->v_type == VAR_DICT)
    {
	char_u	*tofree;
	char_u	numbuf[NUMBUFLEN];

	retval = tv2string(tv, &tofree, numbuf, 0);
	// Make a copy if we have a value but it's not in allocated memory.
	ಆದರೆ (retval != ಖಾಲಿ && tofree == ಖಾಲಿ)
	    retval = vim_strsave(retval);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	retval = vim_strsave(tv_get_string(tv));
    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Top level evaluation function, returning a string.  Does not handle line
 * breaks.
 * When "join_list" is TRUE convert a List and a Tuple into a sequence of
 * lines.
 * Return pointer to allocated memory, or NULL for failure.
 */
    char_u *
eval_ಗಮ್ಯ_string_eap(
    char_u	*arg,
    ಪೂರ್ಣಾಂಕ		join_list,
    exarg_T	*eap,
    ಪೂರ್ಣಾಂಕ		use_simple_function)
{
    typval_T	tv;
    char_u	*retval;
    evalarg_T	evalarg;
    ಪೂರ್ಣಾಂಕ		r;

    fill_evalarg_from_eap(&evalarg, eap, eap != ಖಾಲಿ && eap->skip);
    ಆದರೆ (use_simple_function)
	r = eval0_simple_funccal(arg, &tv, ಖಾಲಿ, &evalarg);
    ಇಲ್ಲದಿದ್ದರೆ
	r = eval0(arg, &tv, ಖಾಲಿ, &evalarg);
    ಆದರೆ (r == FAIL)
	retval = ಖಾಲಿ;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	retval = typval2string(&tv, join_list);
	clear_tv(&tv);
    }
    clear_evalarg(&evalarg, ಖಾಲಿ);

    ಹಿಂತಿರುಗಿ retval;
}

    char_u *
eval_ಗಮ್ಯ_string(
    char_u	*arg,
    ಪೂರ್ಣಾಂಕ		join_list,
    ಪೂರ್ಣಾಂಕ		use_simple_function)
{
    ಹಿಂತಿರುಗಿ eval_ಗಮ್ಯ_string_eap(arg, join_list, ಖಾಲಿ, use_simple_function);
}

/*
 * Call eval_to_string() without using current local variables and using
 * textlock.  When "use_sandbox" is TRUE use the sandbox.
 * Use legacy Vim script syntax.
 */
    char_u *
eval_ಗಮ್ಯ_string_ಸುರಕ್ಷಿತ(
    char_u	*arg,
    ಪೂರ್ಣಾಂಕ		use_sandbox,
    ಪೂರ್ಣಾಂಕ		keep_script_ಆವೃತ್ತಿ,
    ಪೂರ್ಣಾಂಕ		use_simple_function)
{
    char_u	*retval;
    funccal_ನಮೂದು_T funccal_ನಮೂದು;
    ಪೂರ್ಣಾಂಕ		save_sc_ಆವೃತ್ತಿ = current_sctx.sc_ಆವೃತ್ತಿ;
    ಪೂರ್ಣಾಂಕ		save_garbage = may_garbage_collect;

    ಆದರೆ (!keep_script_ಆವೃತ್ತಿ)
	current_sctx.sc_ಆವೃತ್ತಿ = 1;
    save_funccal(&funccal_ನಮೂದು);
    ಆದರೆ (use_sandbox)
	++sandbox;
    ++textlock;
    may_garbage_collect = FALSE;
    retval = eval_ಗಮ್ಯ_string(arg, FALSE, use_simple_function);
    ಆದರೆ (use_sandbox)
	--sandbox;
    --textlock;
    may_garbage_collect = save_garbage;
    restore_funccal();
    current_sctx.sc_ಆವೃತ್ತಿ = save_sc_ಆವೃತ್ತಿ;
    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Top level evaluation function, returning a number.
 * Evaluates "expr" silently.
 * Returns -1 for an error.
 */
    varnumber_T
eval_ಗಮ್ಯ_ಸಂಖ್ಯೆ(char_u *ಅಭಿವ್ಯಕ್ತಿ, ಪೂರ್ಣಾಂಕ use_simple_function)
{
    typval_T	rettv;
    varnumber_T	retval;
    char_u	*p = skipwhite(ಅಭಿವ್ಯಕ್ತಿ);
    ಪೂರ್ಣಾಂಕ		r = NOTDONE;

    ++emsg_off;

    ಆದರೆ (use_simple_function)
	r = may_call_simple_func(ಅಭಿವ್ಯಕ್ತಿ, &rettv);
    ಆದರೆ (r == NOTDONE)
	r = eval1(&p, &rettv, &EVALARG_EVALUATE);
    ಆದರೆ (r == FAIL)
	retval = -1;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	retval = tv_get_ಸಂಖ್ಯೆ_chk(&rettv, ಖಾಲಿ);
	clear_tv(&rettv);
    }
    --emsg_off;

    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Top level evaluation function.
 * Returns an allocated typval_T with the result.
 * Returns NULL when there is an error.
 */
    typval_T *
eval_ಅಭಿವ್ಯಕ್ತಿ(char_u *arg, exarg_T *eap)
{
    ಹಿಂತಿರುಗಿ eval_ಅಭಿವ್ಯಕ್ತಿ_ext(arg, eap, FALSE);
}

    typval_T *
eval_ಅಭಿವ್ಯಕ್ತಿ_ext(char_u *arg, exarg_T *eap, ಪೂರ್ಣಾಂಕ use_simple_function)
{
    typval_T	*tv;
    evalarg_T	evalarg;

    fill_evalarg_from_eap(&evalarg, eap, eap != ಖಾಲಿ && eap->skip);

    tv = ALLOC_ONE(typval_T);
    ಆದರೆ (tv != ಖಾಲಿ)
    {
	ಪೂರ್ಣಾಂಕ	r = NOTDONE;

	ಆದರೆ (use_simple_function)
	    r = eval0_simple_funccal(arg, tv, eap, &evalarg);
	ಆದರೆ (r == NOTDONE)
	    r = eval0(arg, tv, eap, &evalarg);

	ಆದರೆ (r == FAIL)
	    VIM_CLEAR(tv);
    }

    clear_evalarg(&evalarg, eap);
    ಹಿಂತಿರುಗಿ tv;
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
	    ಪೂರ್ಣಾಂಕ		verbose)
{
    typval_T	ref;
    char_u	*name = *arg;
    ಪೂರ್ಣಾಂಕ		save_flags = 0;
    ಪೂರ್ಣಾಂಕ		evaluate = evalarg != ಖಾಲಿ
				      && (evalarg->eval_flags & EVAL_EVALUATE);

    ref.v_type = VAR_UNKNOWN;
    ಆದರೆ (evalarg != ಖಾಲಿ)
    {
	// need to evaluate this to get an import, like in "a.Func"
	save_flags = evalarg->eval_flags;
	evalarg->eval_flags |= EVAL_EVALUATE;
    }
    ಆದರೆ (eval9(arg, &ref, evalarg, FALSE) == FAIL)
    {
	dictitem_T	*v;

	// If <SID>VarName was used it would not be found, try another way.
	v = ಕಂಡುಹಿಡಿ_var_also_ಒಳ_script(name, ಖಾಲಿ, FALSE);
	ಆದರೆ (v == ಖಾಲಿ)
	{
	    name = ಖಾಲಿ;
	    ಹೋಗು theend;
	}
	copy_tv(&v->di_tv, &ref);
    }
    ಆದರೆ (*skipwhite(*arg) != NUL)
    {
	ಆದರೆ (verbose)
	    semsg(_(e_trailing_characters_str), *arg);
	name = ಖಾಲಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ref.v_type == VAR_FUNC && ref.vval.v_string != ಖಾಲಿ)
    {
	name = ref.vval.v_string;
	ref.vval.v_string = ಖಾಲಿ;
	*tofree = name;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ref.v_type == VAR_PARTIAL && ref.vval.v_partial != ಖಾಲಿ)
    {
	ಆದರೆ (ref.vval.v_partial->pt_argc > 0
		|| ref.vval.v_partial->pt_dict != ಖಾಲಿ)
	{
	    ಆದರೆ (verbose)
		emsg(_(e_cannot_use_partial_ಇಲ್ಲಿ));
	    name = ಖಾಲಿ;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    name = vim_strsave(partial_name(ref.vval.v_partial));
	    *tofree = name;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (evaluate)
    {
	ಆದರೆ (verbose)
	    semsg(_(e_not_callable_type_str), name);
	name = ಖಾಲಿ;
    }

theend:
    clear_tv(&ref);
    ಆದರೆ (evalarg != ಖಾಲಿ)
	evalarg->eval_flags = save_flags;
    ಹಿಂತಿರುಗಿ name;
}

/*
 * Call some Vim script function and return the result in "*rettv".
 * Uses argv[0] to argv[argc - 1] for the function arguments.  argv[argc]
 * should have type VAR_UNKNOWN.
 * Returns OK or FAIL.
 */
    ಪೂರ್ಣಾಂಕ
call_vim_function(
    char_u      *func,
    ಪೂರ್ಣಾಂಕ		argc,
    typval_T	*argv,
    typval_T	*rettv)
{
    ಪೂರ್ಣಾಂಕ		ret;
    funcexe_T	funcexe;
    char_u	*arg;
    char_u	*name;
    char_u	*tofree = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		ignore_errors;

    rettv->v_type = VAR_UNKNOWN;		// clear_tv() uses this
    CLEAR_ಕ್ಷೇತ್ರ(funcexe);
    funcexe.fe_firstline = curwin->w_cursor.lnum;
    funcexe.fe_lastline = curwin->w_cursor.lnum;
    funcexe.fe_evaluate = TRUE;

    // The name might be "import.Func" or "Funcref".  We don't know, we need to
    // ignore errors for an undefined name.  But we do want errors when an
    // autoload script has errors.  Guess that when there is a dot in the name
    // showing errors is the right choice.
    ignore_errors = vim_strchr(func, '.') == ಖಾಲಿ;
    arg = func;
    ಆದರೆ (ignore_errors)
	++emsg_off;
    name = deref_function_name(&arg, &tofree, &EVALARG_EVALUATE, FALSE);
    ಆದರೆ (ignore_errors)
	--emsg_off;
    ಆದರೆ (name == ಖಾಲಿ)
	name = func;

    ret = call_func(name, -1, rettv, argc, argv, &funcexe);

    ಆದರೆ (ret == FAIL)
	clear_tv(rettv);
    vim_free(tofree);

    ಹಿಂತಿರುಗಿ ret;
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
    ಪೂರ್ಣಾಂಕ		argc,
    typval_T	*argv)
{
    typval_T	rettv;
    char_u	*retval;

    ಆದರೆ (call_vim_function(func, argc, argv, &rettv) == FAIL)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    retval = vim_strsave(tv_get_string(&rettv));
    clear_tv(&rettv);
    ಹಿಂತಿರುಗಿ retval;
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
    ಪೂರ್ಣಾಂಕ		argc,
    typval_T	*argv)
{
    typval_T	rettv;

    ಆದರೆ (call_vim_function(func, argc, argv, &rettv) == FAIL)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (rettv.v_type != VAR_LIST)
    {
	semsg(_(e_custom_list_completion_function_does_not_return_list_but_str),
		vartype_name(rettv.v_type));
	clear_tv(&rettv);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಹಿಂತಿರುಗಿ rettv.vval.v_list;
}

#ಆದರೆ defined(FEAT_FOLDING)
/*
 * Evaluate "arg", which is 'foldexpr'.
 * Note: caller must set "curwin" to match "arg".
 * Returns the foldlevel, and any character preceding it in "*cp".  Doesn't
 * give error messages.
 */
    ಪೂರ್ಣಾಂಕ
eval_foldexpr(win_T *wp, ಪೂರ್ಣಾಂಕ *cp)
{
    char_u	*arg;
    typval_T	tv;
    varnumber_T	retval;
    char_u	*s;
    sctx_T	saved_sctx = current_sctx;
    ಪೂರ್ಣಾಂಕ		use_sandbox = was_ಗಣ_insecurely(wp, (char_u *)"foldexpr",
								    OPT_LOCAL);

    arg = skipwhite(wp->w_p_fde);
    current_sctx = wp->w_p_script_ctx[WV_FDE];

    ++emsg_off;
    ಆದರೆ (use_sandbox)
	++sandbox;
    ++textlock;
    *cp = NUL;

    // Evaluate the expression.  If the expression is "FuncName()" call the
    // function directly.
    ಆದರೆ (eval0_simple_funccal(arg, &tv, ಖಾಲಿ, &EVALARG_EVALUATE) == FAIL)
	retval = 0;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// If the result is a number, just return the number.
	ಆದರೆ (tv.v_type == VAR_ಸಂಖ್ಯೆ)
	    retval = tv.vval.v_ಸಂಖ್ಯೆ;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tv.v_type != VAR_STRING || tv.vval.v_string == ಖಾಲಿ)
	    retval = 0;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // If the result is a string, check if there is a non-digit before
	    // the number.
	    s = tv.vval.v_string;
	    ಆದರೆ (*s != NUL && !VIM_ISDIGIT(*s) && *s != '-')
		*cp = *s++;
	    retval = atol((ಅಕ್ಷರ *)s);
	}
	clear_tv(&tv);
    }
    --emsg_off;
    ಆದರೆ (use_sandbox)
	--sandbox;
    --textlock;
    clear_evalarg(&EVALARG_EVALUATE, ಖಾಲಿ);
    current_sctx = saved_sctx;

    ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)retval;
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ LOG_LOCKVAR
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ flag_string_S
{
    ಪೂರ್ಣಾಂಕ	    flag;
    ಅಕ್ಷರ    *str;
} flag_string_T;

    ಸ್ಥಿರ ಅಕ್ಷರ *
flags_tostring(ಪೂರ್ಣಾಂಕ flags, flag_string_T *_fstring, ಅಕ್ಷರ *buf, size_t n)
{
    ಅಕ್ಷರ *p = buf;
    *p = NUL;
    ಗಾಗಿ (flag_string_T *fstring = _fstring; fstring->flag; ++fstring)
    {
	ಆದರೆ ((fstring->flag & flags) != 0)
	{
	    size_t len = STRLEN(fstring->str);
	    ಆದರೆ (n > p - buf + len + 7)
	    {
		STRCAT(p, fstring->str);
		p += len;
		STRCAT(p, " ");
		++p;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		STRCAT(buf, "...");
		ನಿಲ್ಲಿಸು;
	    }
	}
    }
    ಹಿಂತಿರುಗಿ buf;
}

flag_string_T glv_flag_ಅಕ್ಷರಸರಣಿಗಳು[] = {
    { GLV_QUIET,		"QUIET" },
    { GLV_NO_AUTOLOAD,		"NO_AUTOLOAD" },
    { GLV_READ_ONLY,		"READ_ONLY" },
    { GLV_NO_DECL,		"NO_DECL" },
    { GLV_COMPILING,		"COMPILING" },
    { GLV_ನಿಯೋಜಿಸು_WITH_OP,	"ASSIGN_WITH_OP" },
    { GLV_PREFER_FUNC,		"PREFER_FUNC" },
    { 0,			ಖಾಲಿ }
};
#ಆದರೆ_ಅಂತ್ಯ

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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ LOG_LOCKVAR
    ch_log(ಖಾಲಿ, "LKVAR: fill_lval_from_lval_root(): name %s, tv %p",
						lp->ll_name, (ಶೂನ್ಯ*)lr->lr_tv);
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (lr->lr_tv == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (!lr->lr_is_arg && lr->lr_tv->v_type == VAR_CLASS)
    {
	ಆದರೆ (lr->lr_tv->vval.v_class != ಖಾಲಿ)
	{
	    // Special special case. Look for a bare class variable reference.
	    class_T	*cl = lr->lr_tv->vval.v_class;
	    ಪೂರ್ಣಾಂಕ		m_idx;
	    ocmember_T	*m = class_member_lookup(cl, lp->ll_name,
					lp->ll_name_ಕೊನೆ - lp->ll_name, &m_idx);
	    ಆದರೆ (m != ಖಾಲಿ)
	    {
		// Assuming "inside class" since bare reference.
		lp->ll_class = lr->lr_tv->vval.v_class;
		lp->ll_oi = m_idx;
		lp->ll_valtype = m->ocm_type;
		lp->ll_tv = &lp->ll_class->class_members_tv[m_idx];
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ LOG_LOCKVAR
		ch_log(ಖಾಲಿ, "LKVAR:    ... class member %s.%s",
					lp->ll_class->class_name.string, lp->ll_name);
#ಆದರೆ_ಅಂತ್ಯ
		ಹಿಂತಿರುಗಿ;
	    }
	}
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ LOG_LOCKVAR
    ch_log(ಖಾಲಿ, "LKVAR:    ... type: %s", vartype_name(lr->lr_tv->v_type));
#ಆದರೆ_ಅಂತ್ಯ
    lp->ll_tv = lr->lr_tv;
    lp->ll_is_ಬೇರು = TRUE;
}

/*
 * Check if the class has permission to access the member.
 * Returns OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_lval_check_access(
    class_T	*cl_exec,   // executing class, NULL if :def or script level
    class_T	*cl,	    // class which contains the member
    ocmember_T	*om,	    // member being accessed
    char_u	*p,	    // char after member name
    ಪೂರ್ಣಾಂಕ		flags)	    // GLV flags to check if writing to lval
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ LOG_LOCKVAR
    ch_log(ಖಾಲಿ, "LKVAR: get_lval_check_access(), cl_exec %p, cl %p, %c",
						(ಶೂನ್ಯ*)cl_exec, (ಶೂನ್ಯ*)cl, *p);
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (cl_exec != ಖಾಲಿ && cl_exec == cl)
	ಹಿಂತಿರುಗಿ OK;

    ಅಕ್ಷರ *ಸಂದೇಶ = ಖಾಲಿ;
    ಆಯ್ಕೆ (om->ocm_access)
    {
	ಸ್ಥಿತಿ VIM_ACCESS_ಖಾಸಗಿ:
	    ಸಂದೇಶ = e_cannot_access_protected_variable_str;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VIM_ACCESS_READ:
	    // If [idx] or .key following, read only OK.
	    ಆದರೆ (*p == '[' || *p == '.')
		ನಿಲ್ಲಿಸು;
	    ಆದರೆ ((flags & GLV_READ_ONLY) == 0)
	    {
		ಆದರೆ (IS_ENUM(cl))
		{
		    ಆದರೆ (om->ocm_type->tt_type == VAR_OBJECT)
			semsg(_(e_enumvalue_str_cannot_be_modified),
				cl->class_name.string, om->ocm_name.string);
		    ಇಲ್ಲದಿದ್ದರೆ
			ಸಂದೇಶ = e_variable_is_not_writable_str;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    ಸಂದೇಶ = e_variable_is_not_writable_str;
	    }
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VIM_ACCESS_ALL:
	    ನಿಲ್ಲಿಸು;
    }
    ಆದರೆ (ಸಂದೇಶ != ಖಾಲಿ)
    {
	emsg_var_cl_define(ಸಂದೇಶ, om->ocm_name.string, 0, cl);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
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
    ಪೂರ್ಣಾಂಕ		fne_flags)
{
    ufunc_T	*ufunc;
    type_T	*type = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		cc;
    ಪೂರ್ಣಾಂಕ		rc = FAIL;

    p = skipwhite(p);

    import_check_sourced_sid(&imp_sid);
    lp->ll_sid = imp_sid;
    lp->ll_name = p;
    p = ಕಂಡುಹಿಡಿ_name_ಕೊನೆ(lp->ll_name, ಖಾಲಿ, ಖಾಲಿ, fne_flags);
    lp->ll_name_ಕೊನೆ = p;

    // check the item is exported
    cc = *p;
    *p = NUL;
    ಆದರೆ (ಕಂಡುಹಿಡಿ_exported(imp_sid, lp->ll_name, &ufunc, &type, ಖಾಲಿ, ಖಾಲಿ,
								TRUE) == -1)
	ಹೋಗು failed;

    // Get the typval for the exported item
    hashtab_T *ht = &SCRIPT_VARS(imp_sid);
    ಆದರೆ (ht == ಖಾಲಿ)
	ಹೋಗು failed;

    dictitem_T *di = ಕಂಡುಹಿಡಿ_var_ಒಳ_ht(ht, 0, lp->ll_name, TRUE);
    ಆದರೆ (di == ಖಾಲಿ)
	// script is autoloaded.  So variable will be found later
	ಹೋಗು success;

    *dip = di;

    // Check whether the variable is writable.
    svar_T *sv = ಕಂಡುಹಿಡಿ_typval_ಒಳ_script(&di->di_tv, imp_sid, FALSE);
    ಆದರೆ (sv != ಖಾಲಿ && sv->sv_const != 0)
    {
	semsg(_(e_cannot_change_readonly_variable_str), lp->ll_name);
	ಹೋಗು failed;
    }

    // check whether variable is locked
    ಆದರೆ (ಮೌಲ್ಯ_check_ಬೀಗ(di->di_tv.v_ಬೀಗ, lp->ll_name, FALSE))
	ಹೋಗು failed;

    lp->ll_tv = &di->di_tv;
    lp->ll_valtype = type;

success:
    rc = OK;

failed:
    *p = cc;
    ಹಿಂತಿರುಗಿ rc == OK ? p : ಖಾಲಿ;
}

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    GLV_FAIL,
    GLV_OK,
    GLV_STOP
} glv_ಸ್ಥಿತಿ_T;

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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_lval_dict_ಅಂಶ(
    lval_T	*lp,
    char_u	*name,
    char_u	*ಕೀಲಿ,
    ಪೂರ್ಣಾಂಕ		len,
    char_u	**ಕೀಲಿ_ಕೊನೆ,
    typval_T	*var1,
    ಪೂರ್ಣಾಂಕ		flags,
    ಪೂರ್ಣಾಂಕ		unlet,
    typval_T	*rettv)
{
    ಪೂರ್ಣಾಂಕ		quiet = flags & GLV_QUIET;
    char_u	*p = *ಕೀಲಿ_ಕೊನೆ;

    ಆದರೆ (len == -1)
    {
	// "[key]": get key from "var1"
	ಕೀಲಿ = tv_get_string_chk(var1);	// is number or string
	ಆದರೆ (ಕೀಲಿ == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ GLV_FAIL;
    }
    lp->ll_list = ಖಾಲಿ;
    lp->ll_list = ಖಾಲಿ;
    lp->ll_blob = ಖಾಲಿ;
    lp->ll_object = ಖಾಲಿ;
    lp->ll_class = ಖಾಲಿ;
    lp->ll_tuple = ಖಾಲಿ;

    // a NULL dict is equivalent with an empty dict
    ಆದರೆ (lp->ll_tv->vval.v_dict == ಖಾಲಿ)
    {
	lp->ll_tv->vval.v_dict = dict_alloc();
	ಆದರೆ (lp->ll_tv->vval.v_dict == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ GLV_FAIL;
	++lp->ll_tv->vval.v_dict->dv_refcount;
    }
    lp->ll_dict = lp->ll_tv->vval.v_dict;

    lp->ll_di = dict_ಕಂಡುಹಿಡಿ(lp->ll_dict, ಕೀಲಿ, len);

    // When assigning to a scope dictionary check that a function and
    // variable name is valid (only variable name unless it is l: or
    // g: dictionary). Disallow overwriting a builtin function.
    ಆದರೆ (rettv != ಖಾಲಿ && lp->ll_dict->dv_scope != 0)
    {
	ಪೂರ್ಣಾಂಕ prevval;

	ಆದರೆ (len != -1)
	{
	    prevval = ಕೀಲಿ[len];
	    ಕೀಲಿ[len] = NUL;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    prevval = 0; // avoid compiler warning
	ಪೂರ್ಣಾಂಕ wrong = (lp->ll_dict->dv_scope == VAR_DEF_SCOPE
		&& (rettv->v_type == VAR_FUNC
		    || rettv->v_type == VAR_PARTIAL)
		&& var_wrong_func_name(ಕೀಲಿ, lp->ll_di == ಖಾಲಿ))
	    || !valid_varname(ಕೀಲಿ, -1, TRUE);
	ಆದರೆ (len != -1)
	    ಕೀಲಿ[len] = prevval;
	ಆದರೆ (wrong)
	    ಹಿಂತಿರುಗಿ GLV_FAIL;
    }

    ಆದರೆ (lp->ll_valtype != ಖಾಲಿ)
	// use the type of the member
	lp->ll_valtype = lp->ll_valtype->tt_member;

    ಆದರೆ (lp->ll_di == ಖಾಲಿ)
    {
	// Can't add "v:" or "a:" variable.
	ಆದರೆ (lp->ll_dict == get_vimvar_dict()
		|| &lp->ll_dict->dv_hashtab == get_funccal_ವಾದಗಳು_ht())
	{
	    semsg(_(e_illegal_variable_name_str), name);
	    ಹಿಂತಿರುಗಿ GLV_FAIL;
	}

	// Key does not exist in dict: may need to add it.
	ಆದರೆ (*p == '[' || *p == '.' || unlet)
	{
	    ಆದರೆ (!quiet)
		semsg(_(e_ಕೀಲಿ_not_present_ಒಳ_dictionary_str), ಕೀಲಿ);
	    ಹಿಂತಿರುಗಿ GLV_FAIL;
	}
	ಆದರೆ (len == -1)
	    lp->ll_newkey = vim_strsave(ಕೀಲಿ);
	ಇಲ್ಲದಿದ್ದರೆ
	    lp->ll_newkey = vim_strnsave(ಕೀಲಿ, len);
	ಆದರೆ (lp->ll_newkey == ಖಾಲಿ)
	    p = ಖಾಲಿ;

	*ಕೀಲಿ_ಕೊನೆ = p;
	ಹಿಂತಿರುಗಿ GLV_STOP;
    }
    // existing variable, need to check if it can be changed
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((flags & GLV_READ_ONLY) == 0
	    && (var_check_ro(lp->ll_di->di_flags, name, FALSE)
		|| var_check_ಬೀಗ(lp->ll_di->di_flags, name, FALSE)))
	ಹಿಂತಿರುಗಿ GLV_FAIL;

    lp->ll_tv = &lp->ll_di->di_tv;

    ಹಿಂತಿರುಗಿ GLV_OK;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_lval_blob(
    lval_T	*lp,
    typval_T	*var1,
    typval_T	*var2,
    ಪೂರ್ಣಾಂಕ		empty1,
    ಪೂರ್ಣಾಂಕ		quiet)
{
    ದೀರ್ಘ	bloblen = blob_len(lp->ll_tv->vval.v_blob);

    lp->ll_list = ಖಾಲಿ;
    lp->ll_dict = ಖಾಲಿ;
    lp->ll_object = ಖಾಲಿ;
    lp->ll_class = ಖಾಲಿ;
    lp->ll_tuple = ಖಾಲಿ;

    // Get the number and item for the only or first index of a List or Tuple.
    ಆದರೆ (empty1)
	lp->ll_n1 = 0;
    ಇಲ್ಲದಿದ್ದರೆ
	// is number or string
	lp->ll_n1 = (ದೀರ್ಘ)tv_get_ಸಂಖ್ಯೆ(var1);

    ಆದರೆ (check_blob_ಸೂಚ್ಯಂಕ(bloblen, lp->ll_n1, quiet) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;
    ಆದರೆ (lp->ll_range && !lp->ll_empty2)
    {
	lp->ll_n2 = (ದೀರ್ಘ)tv_get_ಸಂಖ್ಯೆ(var2);
	ಆದರೆ (check_blob_range(bloblen, lp->ll_n1, lp->ll_n2, quiet) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
    }

    ಆದರೆ (!lp->ll_range)
	// Indexing a single byte in a blob.  So the rhs type is a
	// number.
	lp->ll_valtype = &t_ಸಂಖ್ಯೆ;

    lp->ll_blob = lp->ll_tv->vval.v_blob;
    lp->ll_tv = ಖಾಲಿ;

    ಹಿಂತಿರುಗಿ OK;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_lval_list(
    lval_T	*lp,
    typval_T	*var1,
    typval_T	*var2,
    ಪೂರ್ಣಾಂಕ		empty1,
    ಪೂರ್ಣಾಂಕ		flags,
    ಪೂರ್ಣಾಂಕ		quiet)
{
    /*
     * Get the number and item for the only or first index of the List.
     */
    ಆದರೆ (empty1)
	lp->ll_n1 = 0;
    ಇಲ್ಲದಿದ್ದರೆ
	// is number or string
	lp->ll_n1 = (ದೀರ್ಘ)tv_get_ಸಂಖ್ಯೆ(var1);

    lp->ll_dict = ಖಾಲಿ;
    lp->ll_object = ಖಾಲಿ;
    lp->ll_class = ಖಾಲಿ;
    lp->ll_tuple = ಖಾಲಿ;
    lp->ll_list = lp->ll_tv->vval.v_list;
    lp->ll_li = check_range_ಸೂಚ್ಯಂಕ_one(lp->ll_list, &lp->ll_n1,
				(flags & GLV_ನಿಯೋಜಿಸು_WITH_OP) == 0, quiet);
    ಆದರೆ (lp->ll_li == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (lp->ll_valtype != ಖಾಲಿ && !lp->ll_range)
    {
	// use the type of the member
	ಆದರೆ (lp->ll_valtype->tt_member != ಖಾಲಿ)
	    lp->ll_valtype = lp->ll_valtype->tt_member;
	ಇಲ್ಲದಿದ್ದರೆ
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
    ಆದರೆ (lp->ll_range && !lp->ll_empty2)
    {
	lp->ll_n2 = (ದೀರ್ಘ)tv_get_ಸಂಖ್ಯೆ(var2);
	// is number or string
	ಆದರೆ (check_range_ಸೂಚ್ಯಂಕ_two(lp->ll_list,
			&lp->ll_n1, lp->ll_li, &lp->ll_n2, quiet) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
    }

    lp->ll_tv = &lp->ll_li->li_tv;

    ಹಿಂತಿರುಗಿ OK;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_lval_tuple(
    lval_T	*lp,
    typval_T	*idx,
    ಪೂರ್ಣಾಂಕ		quiet)
{
    // is number or string
    lp->ll_n1 = (ದೀರ್ಘ)tv_get_ಸಂಖ್ಯೆ(idx);

    lp->ll_list = ಖಾಲಿ;
    lp->ll_dict = ಖಾಲಿ;
    lp->ll_blob = ಖಾಲಿ;
    lp->ll_object = ಖಾಲಿ;
    lp->ll_class = ಖಾಲಿ;

    lp->ll_tuple = lp->ll_tv->vval.v_tuple;
    lp->ll_tv = tuple_ಕಂಡುಹಿಡಿ(lp->ll_tuple, lp->ll_n1);
    ಆದರೆ (lp->ll_tv == ಖಾಲಿ)
    {
	ಆದರೆ (!quiet)
	    semsg(_(e_tuple_ಸೂಚ್ಯಂಕ_ಹೊರಹೋಗು_ದ_range_nr), lp->ll_n1);
	ಹಿಂತಿರುಗಿ GLV_FAIL;
    }

    // use the type of the member
    ಆದರೆ (lp->ll_valtype != ಖಾಲಿ)
    {
	ಆದರೆ (lp->ll_valtype != ಖಾಲಿ
		&& lp->ll_valtype->tt_type == VAR_TUPLE
		&& lp->ll_valtype->tt_argcount == 1)
	{
	    // a variadic tuple or a single item tuple
	    ಆದರೆ (lp->ll_valtype->tt_flags & TTFLAG_VARARGS)
		lp->ll_valtype = lp->ll_valtype->tt_ವಾದಗಳು[0]->tt_member;
	    ಇಲ್ಲದಿದ್ದರೆ
		lp->ll_valtype = lp->ll_valtype->tt_ವಾದಗಳು[0];
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    // If the LHS member type is not known (VAR_ANY), then get it from
	    // the tuple item (after indexing)
	    lp->ll_valtype = typval2type(lp->ll_tv, get_copyID(),
					&lp->ll_type_list, TVTT_DO_MEMBER);
    }

    ಹಿಂತಿರುಗಿ GLV_OK;
}

/*
 * Get a class or object lval method in class "cl".  The 'key' argument points
 * to the method name and 'key_end' points to the character after 'key'.
 * 'v_type' is VAR_CLASS or VAR_OBJECT.
 *
 * The method index, method function pointer and method type are returned in
 * "lp".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_lval_oc_ವಿಧಾನ(
    lval_T	*lp,
    class_T	*cl,
    char_u	*ಕೀಲಿ,
    char_u	**ಕೀಲಿ_ಕೊನೆ,
    vartype_T	v_type)
{
    // Look for a method with this name.
    // round 1: class functions (skipped for an object)
    // round 2: object methods
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ ಸುತ್ತು = v_type == VAR_OBJECT ? 2 : 1; ಸುತ್ತು <= 2; ++ಸುತ್ತು)
    {
	ಪೂರ್ಣಾಂಕ	m_idx;
	ufunc_T	*fp;

	fp = ವಿಧಾನ_lookup(cl, ಸುತ್ತು == 1 ? VAR_CLASS : VAR_OBJECT,
						ಕೀಲಿ, *ಕೀಲಿ_ಕೊನೆ - ಕೀಲಿ, &m_idx);
	lp->ll_oi = m_idx;

	// process generic method (if present)
	ಆದರೆ (fp && (fp = eval_generic_func(fp, ಕೀಲಿ, ಕೀಲಿ_ಕೊನೆ)) == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;

	ಆದರೆ (fp != ಖಾಲಿ)
	{
	    lp->ll_ufunc = fp;
	    lp->ll_valtype = fp->uf_func_type;
	    ನಿಲ್ಲಿಸು;
	}
    }

    ಹಿಂತಿರುಗಿ OK;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_lval_oc_variable(
    lval_T	*lp,
    class_T	*cl,
    char_u	*ಕೀಲಿ,
    char_u	*ಕೀಲಿ_ಕೊನೆ,
    vartype_T	v_type,
    class_T	*cl_exec,
    ಪೂರ್ಣಾಂಕ		flags)
{
    ಪೂರ್ಣಾಂಕ		m_idx;
    ocmember_T	*om;

    om = member_lookup(cl, v_type, ಕೀಲಿ, ಕೀಲಿ_ಕೊನೆ - ಕೀಲಿ, &m_idx);
    lp->ll_oi = m_idx;
    ಆದರೆ (om == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ OK;

    // Check variable is accessible
    ಆದರೆ (get_lval_check_access(cl_exec, cl, om, ಕೀಲಿ_ಕೊನೆ, flags) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    // When lhs is used to modify the variable, check it is not a read-only
    // variable.
    ಆದರೆ ((flags & GLV_READ_ONLY) == 0 && (*ಕೀಲಿ_ಕೊನೆ != '.' && *ಕೀಲಿ_ಕೊನೆ != '[')
	    && oc_var_check_ro(cl, om))
	ಹಿಂತಿರುಗಿ FAIL;

    lp->ll_valtype = om->ocm_type;

    ಆದರೆ (v_type == VAR_OBJECT)
	lp->ll_tv = ((typval_T *)(lp->ll_tv->vval.v_object + 1)) + m_idx;
    ಇಲ್ಲದಿದ್ದರೆ
	lp->ll_tv = &cl->class_members_tv[m_idx];

    ಹಿಂತಿರುಗಿ OK;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_lval_class_or_obj(
    lval_T	*lp,
    char_u	*ಕೀಲಿ,
    char_u	**ಕೀಲಿ_ಕೊನೆ,
    vartype_T	v_type,
    class_T	*cl_exec,
    ಪೂರ್ಣಾಂಕ		flags,
    ಪೂರ್ಣಾಂಕ		quiet)
{
    lp->ll_dict = ಖಾಲಿ;
    lp->ll_list = ಖಾಲಿ;
    lp->ll_tuple = ಖಾಲಿ;

    class_T *cl;
    ಆದರೆ (v_type == VAR_OBJECT)
    {
	ಆದರೆ (lp->ll_tv->vval.v_object == ಖಾಲಿ)
	{
	    ಆದರೆ (!quiet)
		emsg(_(e_using_null_object));
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	cl = lp->ll_tv->vval.v_object->obj_class;
	lp->ll_object = lp->ll_tv->vval.v_object;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	cl = lp->ll_tv->vval.v_class;
	lp->ll_object = ಖಾಲಿ;
    }
    lp->ll_class = cl;

    ಆದರೆ (cl == ಖಾಲಿ)
	// TODO: what if class is NULL?
	ಹಿಂತಿರುಗಿ OK;

    lp->ll_valtype = ಖಾಲಿ;

    ಆದರೆ (flags & GLV_PREFER_FUNC)
	ಆದರೆ (get_lval_oc_ವಿಧಾನ(lp, cl, ಕೀಲಿ, ಕೀಲಿ_ಕೊನೆ, v_type) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;

    // Look for object/class member variable
    ಆದರೆ (lp->ll_valtype == ಖಾಲಿ)
    {
	ಆದರೆ (get_lval_oc_variable(lp, cl, ಕೀಲಿ, *ಕೀಲಿ_ಕೊನೆ, v_type, cl_exec, flags)
								== FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
    }

    ಆದರೆ (lp->ll_valtype == ಖಾಲಿ)
    {
	member_not_ಸಿಕ್ಕಿತು_ಸಂದೇಶ(cl, v_type, ಕೀಲಿ, *ಕೀಲಿ_ಕೊನೆ - ಕೀಲಿ);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Check whether dot (".") is allowed after the variable "name" with type
 * "v_type".  Only Dict, Class and Object types support a dot after the name.
 * Returns TRUE if dot is allowed after the name.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
dot_allowed_after_type(char_u *name, vartype_T v_type, ಪೂರ್ಣಾಂಕ quiet)
{
    ಆದರೆ (v_type != VAR_DICT && v_type != VAR_OBJECT && v_type != VAR_CLASS)
    {
	ಆದರೆ (!quiet)
	    semsg(_(e_dot_not_allowed_after_str_str),
		    vartype_name(v_type), name);
	ಹಿಂತಿರುಗಿ FALSE;
    }

    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Check whether left bracket ("[") is allowed after the variable "name" with
 * type "v_type".  Only Dict, List, Tuple and Blob types support a bracket
 * after the variable name.  Returns TRUE if bracket is allowed after the name.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
bracket_allowed_after_type(char_u *name, vartype_T v_type, ಪೂರ್ಣಾಂಕ quiet)
{
    ಆದರೆ (v_type == VAR_CLASS || v_type == VAR_OBJECT)
    {
	ಆದರೆ (!quiet)
	    semsg(_(e_ಸೂಚ್ಯಂಕ_not_allowed_after_str_str),
		    vartype_name(v_type), name);
	ಹಿಂತಿರುಗಿ FALSE;
    }

    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Check whether the variable "name" with type "v_type" can be followed by an
 * index.  Only Dict, List, Tuple, Blob, Object and Class types support
 * indexing.  Returns TRUE if indexing is allowed after the name.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಸೂಚ್ಯಂಕ_allowed_after_type(char_u *name, vartype_T v_type, ಪೂರ್ಣಾಂಕ quiet)
{
    ಆದರೆ (v_type != VAR_LIST
	    && v_type != VAR_TUPLE
	    && v_type != VAR_DICT
	    && v_type != VAR_BLOB
	    && v_type != VAR_OBJECT
	    && v_type != VAR_CLASS)
    {
	ಆದರೆ (!quiet)
	    semsg(_(e_ಸೂಚ್ಯಂಕ_not_allowed_after_str_str),
		    vartype_name(v_type), name);
	ಹಿಂತಿರುಗಿ FALSE;
    }

    ಹಿಂತಿರುಗಿ TRUE;
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
    ಪೂರ್ಣಾಂಕ		unlet,
    ಪೂರ್ಣಾಂಕ		flags,	    // GLV_ values
    class_T	*cl_exec)
{
    ಪೂರ್ಣಾಂಕ		quiet = flags & GLV_QUIET;
    char_u	*ಕೀಲಿ = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		len;
    typval_T	var1;
    typval_T	var2;
    ಪೂರ್ಣಾಂಕ		empty1 = FALSE;
    ಪೂರ್ಣಾಂಕ		rc = FAIL;

    /*
     * Loop until no more [idx] or .key is following.
     */
    var1.v_type = VAR_UNKNOWN;
    var2.v_type = VAR_UNKNOWN;

    ಆಗ (*p == '[' || (*p == '.' && p[1] != '=' && p[1] != '.'))
    {
	vartype_T v_type = lp->ll_tv->v_type;

	ಆದರೆ (*p == '.' && !dot_allowed_after_type(name, v_type, quiet))
	    ಹೋಗು ಮುಗಿದ;

	ಆದರೆ (*p == '[' && !bracket_allowed_after_type(name, v_type, quiet))
	    ಹೋಗು ಮುಗಿದ;

	ಆದರೆ (!ಸೂಚ್ಯಂಕ_allowed_after_type(name, v_type, quiet))
	    ಹೋಗು ಮುಗಿದ;

	// A NULL list/blob works like an empty list/blob, allocate one now.
	ಪೂರ್ಣಾಂಕ r = OK;
	ಆದರೆ (v_type == VAR_LIST && lp->ll_tv->vval.v_list == ಖಾಲಿ)
	    r = rettv_list_alloc(lp->ll_tv);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (v_type == VAR_BLOB && lp->ll_tv->vval.v_blob == ಖಾಲಿ)
	    r = rettv_blob_alloc(lp->ll_tv);
	ಆದರೆ (r == FAIL)
	    ಹೋಗು ಮುಗಿದ;

	ಆದರೆ (lp->ll_range)
	{
	    ಆದರೆ (!quiet)
		emsg(_(e_ತುಂಡು_must_come_ಕೊನೆಯ));
	    ಹೋಗು ಮುಗಿದ;
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ LOG_LOCKVAR
	ch_log(ಖಾಲಿ, "LKVAR: get_lval() loop: p: %s, type: %s", p,
		vartype_name(v_type));
#ಆದರೆ_ಅಂತ್ಯ

	ಆದರೆ (current_script_is_vim9() && lp->ll_valtype == ಖಾಲಿ
		&& v != ಖಾಲಿ
		&& lp->ll_tv == &v->di_tv
		&& ht != ಖಾಲಿ && ht == get_script_local_ht())
	{
	    svar_T  *sv = ಕಂಡುಹಿಡಿ_typval_ಒಳ_script(lp->ll_tv, 0, TRUE);

	    // Vim9 script local variable: get the type
	    ಆದರೆ (sv != ಖಾಲಿ)
	    {
		lp->ll_valtype = sv->sv_type;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ LOG_LOCKVAR
		ch_log(ಖಾಲಿ, "LKVAR:    ... loop: vim9 assign type: %s",
			vartype_name(lp->ll_valtype->tt_type));
#ಆದರೆ_ಅಂತ್ಯ
	    }
	}

	len = -1;
	ಆದರೆ (*p == '.')
	{
	    ಕೀಲಿ = p + 1;

	    ಗಾಗಿ (len = 0; ASCII_ISALNUM(ಕೀಲಿ[len]) || ಕೀಲಿ[len] == '_'; ++len)
		;
	    ಆದರೆ (len == 0)
	    {
		ಆದರೆ (!quiet)
		    emsg(_(e_cannot_use_ಖಾಲಿಗಳು_ಕೀಲಿ_for_dictionary));
		ಹೋಗು ಮುಗಿದ;
	    }
	    p = ಕೀಲಿ + len;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // Get the index [expr] or the first index [expr: ].
	    p = skipwhite(p + 1);
	    ಆದರೆ (*p == ':')
		empty1 = TRUE;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		empty1 = FALSE;
		ಆದರೆ (eval1(&p, &var1, &EVALARG_EVALUATE) == FAIL)  // recursive!
		    ಹೋಗು ಮುಗಿದ;
		ಆದರೆ (tv_get_string_chk(&var1) == ಖಾಲಿ)
		    // not a number or string
		    ಹೋಗು ಮುಗಿದ;
		p = skipwhite(p);
	    }

	    // Optionally get the second index [ :expr].
	    ಆದರೆ (*p == ':')
	    {
		ಆದರೆ (v_type == VAR_DICT)
		{
		    ಆದರೆ (!quiet)
			emsg(_(e_cannot_ತುಂಡು_dictionary));
		    ಹೋಗು ಮುಗಿದ;
		}
		ಆದರೆ (v_type == VAR_TUPLE)
		{
		    ಆದರೆ (!quiet)
			emsg(_(e_cannot_ತುಂಡು_tuple));
		    ಹೋಗು ಮುಗಿದ;
		}
		ಆದರೆ (rettv != ಖಾಲಿ
			&& !(rettv->v_type == VAR_LIST
			    && rettv->vval.v_list != ಖಾಲಿ)
			&& !(rettv->v_type == VAR_BLOB
			    && rettv->vval.v_blob != ಖಾಲಿ))
		{
		    ಆದರೆ (!quiet)
			emsg(_(e_ತುಂಡು_requires_list_or_blob_ಮೌಲ್ಯ));
		    ಹೋಗು ಮುಗಿದ;
		}
		p = skipwhite(p + 1);
		ಆದರೆ (*p == ']')
		    lp->ll_empty2 = TRUE;
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    lp->ll_empty2 = FALSE;
		    // recursive!
		    ಆದರೆ (eval1(&p, &var2, &EVALARG_EVALUATE) == FAIL)
			ಹೋಗು ಮುಗಿದ;
		    ಆದರೆ (tv_get_string_chk(&var2) == ಖಾಲಿ)
			// not a number or string
			ಹೋಗು ಮುಗಿದ;
		}
		lp->ll_range = TRUE;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		lp->ll_range = FALSE;

	    ಆದರೆ (*p != ']')
	    {
		ಆದರೆ (!quiet)
		    emsg(_(e_missing_closing_ವರ್ಗಫಲ_brace));
		ಹೋಗು ಮುಗಿದ;
	    }

	    // Skip to past ']'.
	    ++p;
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ LOG_LOCKVAR
	ಆದರೆ (len == -1)
	    ch_log(ಖಾಲಿ, "LKVAR:    ... loop: p: %s, '[' key: %s", p,
		    empty1 ? ":" : (ಅಕ್ಷರ*)tv_get_string(&var1));
	ಇಲ್ಲದಿದ್ದರೆ
	    ch_log(ಖಾಲಿ, "LKVAR:    ... loop: p: %s, '.' key: %s", p, ಕೀಲಿ);
#ಆದರೆ_ಅಂತ್ಯ

	ಆದರೆ (v_type == VAR_DICT)
	{
	    glv_ಸ್ಥಿತಿ_T glv_ಸ್ಥಿತಿ;

	    glv_ಸ್ಥಿತಿ = get_lval_dict_ಅಂಶ(lp, name, ಕೀಲಿ, len, &p, &var1,
							flags, unlet, rettv);
	    ಆದರೆ (glv_ಸ್ಥಿತಿ == GLV_FAIL)
		ಹೋಗು ಮುಗಿದ;
	    ಆದರೆ (glv_ಸ್ಥಿತಿ == GLV_STOP)
		ನಿಲ್ಲಿಸು;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (v_type == VAR_BLOB)
	{
	    ಆದರೆ (get_lval_blob(lp, &var1, &var2, empty1, quiet) == FAIL)
		ಹೋಗು ಮುಗಿದ;

	    ನಿಲ್ಲಿಸು;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (v_type == VAR_LIST)
	{
	    ಆದರೆ (get_lval_list(lp, &var1, &var2, empty1, flags, quiet) == FAIL)
		ಹೋಗು ಮುಗಿದ;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (v_type == VAR_TUPLE)
	{
	    ಆದರೆ (get_lval_tuple(lp, &var1, quiet) == FAIL)
		ಹೋಗು ಮುಗಿದ;
	}
	ಇಲ್ಲದಿದ್ದರೆ  // v_type == VAR_CLASS || v_type == VAR_OBJECT
	{
	    ಆದರೆ (get_lval_class_or_obj(lp, ಕೀಲಿ, &p, v_type, cl_exec, flags,
							quiet) == FAIL)
		ಹೋಗು ಮುಗಿದ;
	}

	clear_tv(&var1);
	clear_tv(&var2);
	var1.v_type = VAR_UNKNOWN;
	var2.v_type = VAR_UNKNOWN;
    }

    ಆದರೆ (lp->ll_tuple != ಖಾಲಿ && (flags & GLV_READ_ONLY) == 0)
    {
	ಆದರೆ (!quiet)
	    emsg(_(e_tuple_is_immutable));
	ಹೋಗು ಮುಗಿದ;
    }

    rc = OK;

ಮುಗಿದ:
    clear_tv(&var1);
    clear_tv(&var2);
    ಹಿಂತಿರುಗಿ rc == OK ? p : ಖಾಲಿ;
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
    ಪೂರ್ಣಾಂಕ		unlet,
    ಪೂರ್ಣಾಂಕ		skip,
    ಪೂರ್ಣಾಂಕ		flags,	    // GLV_ values
    ಪೂರ್ಣಾಂಕ		fne_flags)  // flags for find_name_end()
{
    char_u	*p;
    char_u	*ಅಭಿವ್ಯಕ್ತಿ_ಆರಂಭ, *ಅಭಿವ್ಯಕ್ತಿ_ಕೊನೆ;
    ಪೂರ್ಣಾಂಕ		cc;
    dictitem_T	*v = ಖಾಲಿ;
    hashtab_T	*ht = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		quiet = flags & GLV_QUIET;
    ಪೂರ್ಣಾಂಕ		writing = 0;
    ಪೂರ್ಣಾಂಕ		vim9script = ಒಳ_vim9script();
    class_T	*cl_exec = ಖಾಲಿ;    // class that is executing, or NULL.

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ LOG_LOCKVAR
    ಆದರೆ (lval_ಬೇರು == ಖಾಲಿ)
	ch_log(ಖಾಲಿ, "LKVAR: get_lval(): name: %s, lval_root (nil)", name);
    ಇಲ್ಲದಿದ್ದರೆ
	ch_log(ಖಾಲಿ, "LKVAR: get_lval(): name: %s, lr_tv %p lr_is_arg %d",
			name, (ಶೂನ್ಯ*)lval_ಬೇರು->lr_tv, lval_ಬೇರು->lr_is_arg);
    ಅಕ್ಷರ buf[80];
    ch_log(ಖಾಲಿ, "LKVAR:    ...: GLV flags: %s",
		    flags_tostring(flags, glv_flag_ಅಕ್ಷರಸರಣಿಗಳು, buf, ಗಾತ್ರ(buf)));
#ಆದರೆ_ಅಂತ್ಯ

    // Clear everything in "lp".
    CLEAR_ಸೂಚಕ(lp);
    ga_init2(&lp->ll_type_list, ಗಾತ್ರ(type_T *), 10);

    ಆದರೆ (skip || (flags & GLV_COMPILING))
    {
	// When skipping or compiling just find the end of the name.
	lp->ll_name = name;
	lp->ll_name_ಕೊನೆ = ಕಂಡುಹಿಡಿ_name_ಕೊನೆ(name, ಖಾಲಿ, ಖಾಲಿ,
						      FNE_INCL_BR | fne_flags);
	ಹಿಂತಿರುಗಿ lp->ll_name_ಕೊನೆ;
    }

    // Cannot use "s:var" at the Vim9 script level.  "s: type" is OK.
    ಆದರೆ (vim9script && at_script_level()
		  && name[0] == 's' && name[1] == ':' && !VIM_ISWHITE(name[2]))
    {
	semsg(_(e_cannot_use_s_colon_ಒಳ_vim9_script_str), name);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    // Find the end of the name.
    p = ಕಂಡುಹಿಡಿ_name_ಕೊನೆ(name, &ಅಭಿವ್ಯಕ್ತಿ_ಆರಂಭ, &ಅಭಿವ್ಯಕ್ತಿ_ಕೊನೆ, fne_flags);
    lp->ll_name_ಕೊನೆ = p;
    ಆದರೆ (ಅಭಿವ್ಯಕ್ತಿ_ಆರಂಭ != ಖಾಲಿ)
    {
	// Don't expand the name when we already know there is an error.
	ಆದರೆ (unlet && !VIM_ISWHITE(*p) && !ತುದಿಗಳು_excmd(*p)
						    && *p != '[' && *p != '.')
	{
	    semsg(_(e_trailing_characters_str), p);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}

	lp->ll_exp_name = make_expanded_name(name, ಅಭಿವ್ಯಕ್ತಿ_ಆರಂಭ, ಅಭಿವ್ಯಕ್ತಿ_ಕೊನೆ, p);
	ಆದರೆ (lp->ll_exp_name == ಖಾಲಿ)
	{
	    // Report an invalid expression in braces, unless the
	    // expression evaluation has been cancelled due to an
	    // aborting error, an interrupt, or an exception.
	    ಆದರೆ (!aborting() && !quiet)
	    {
		emsg_severe = TRUE;
		semsg(_(e_invalid_ವಾದ_str), name);
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    }
	}
	lp->ll_name = lp->ll_exp_name;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	lp->ll_name = name;

	ಆದರೆ (vim9script)
	{
	    // "a: type" is declaring variable "a" with a type, not "a:".
	    // However, "g:[key]" is indexing a dictionary.
	    ಆದರೆ (p == name + 2 && p[-1] == ':' && *p != '[')
	    {
		--p;
		lp->ll_name_ಕೊನೆ = p;
	    }
	    ಆದರೆ (*skipwhite(p) == ':')
	    {
		char_u	    *tp = skipwhite(p + 1);

		ಆದರೆ (is_scoped_variable(name))
		{
		    semsg(_(e_cannot_use_type_with_this_variable_str), name);
		    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
		}
		ಆದರೆ (VIM_ISWHITE(*p))
		{
		    semsg(_(e_no_white_space_allowed_before_colon_str), p);
		    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
		}
		ಆದರೆ (tp == p + 1 && !quiet)
		{
		    semsg(_(e_white_space_required_after_str_str), ":", p);
		    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
		}
		ಆದರೆ (!SCRIPT_ID_VALID(current_sctx.sc_sid))
		{
		    semsg(_(e_using_type_not_ಒಳ_script_context_str), p);
		    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
		}
		ಆದರೆ (vim9script && (flags & GLV_NO_DECL) &&
			!(flags & GLV_FOR_LOOP))
		{
		    // Using a type and not in a "var" declaration.
		    semsg(_(e_trailing_characters_str), p);
		    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
		}


		// parse the type after the name
		lp->ll_type = parse_type(&tp,
			       &SCRIPT_ಅಂಶ(current_sctx.sc_sid)->sn_type_list,
			       ಖಾಲಿ, ಖಾಲಿ, !quiet);
		ಆದರೆ (!quiet && (lp->ll_type == ಖಾಲಿ
			    || !valid_declaration_type(lp->ll_type)))
		    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
		lp->ll_name_ಕೊನೆ = tp;
	    }
	    // TODO: check inside class?
	}
    }
    ಆದರೆ (lp->ll_name == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ p;

    ಆದರೆ (*p == '.')
    {
	// In legacy script, when a local variable and import exists with this name,
	// prioritize local variable over imports to avoid conflicts.
	ಪೂರ್ಣಾಂಕ var_exists = FALSE;
	ಆದರೆ (!vim9script)
	{
	    cc = *p;
	    *p = NUL;
	    hashtab_T *local_ht = get_funccal_local_ht();
	    ಆದರೆ (local_ht != ಖಾಲಿ)
	    {
		hashitem_T *hi = hash_ಕಂಡುಹಿಡಿ(local_ht, lp->ll_name);
		ಆದರೆ (!HASHITEM_ಖಾಲಿಗಳು(hi))
		    var_exists = TRUE;
	    }
	    *p = cc;
	}

	ಆದರೆ (!var_exists)
	{
	    imported_T *import = ಕಂಡುಹಿಡಿ_imported(lp->ll_name, p - lp->ll_name, TRUE);
	    ಆದರೆ (import != ಖಾಲಿ)
	    {
		p++;	// skip '.'
		p = get_lval_imported(lp, import->imp_sid, p, &v, fne_flags);
		ಆದರೆ (p == ಖಾಲಿ)
		    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    }
	}
    }

    // Without [idx] or .key we are done.
    ಆದರೆ (*p != '[' && *p != '.')
    {
	ಆದರೆ (lval_ಬೇರು != ಖಾಲಿ)
	    fill_lval_from_lval_ಬೇರು(lp, lval_ಬೇರು);
	ಹಿಂತಿರುಗಿ p;
    }

    ಆದರೆ (vim9script && lval_ಬೇರು != ಖಾಲಿ)
	cl_exec = lval_ಬೇರು->lr_cl_exec;
    ಆದರೆ (vim9script && lval_ಬೇರು != ಖಾಲಿ && lval_ಬೇರು->lr_tv != ಖಾಲಿ)
    {
	// using local variable
	lp->ll_tv = lval_ಬೇರು->lr_tv;
	v = ಖಾಲಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lp->ll_tv == ಖಾಲಿ)
    {
	cc = *p;
	*p = NUL;
	// When we would write to the variable pass &ht and prevent autoload.
	writing = !(flags & GLV_READ_ONLY);
	v = ಕಂಡುಹಿಡಿ_var(lp->ll_name, writing ? &ht : ಖಾಲಿ,
					 (flags & GLV_NO_AUTOLOAD) || writing);
	ಆದರೆ (v == ಖಾಲಿ && !quiet)
	    semsg(_(e_undefined_variable_str), lp->ll_name);
	*p = cc;
	ಆದರೆ (v == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	lp->ll_tv = &v->di_tv;
    }

    ಆದರೆ (vim9script && (flags & GLV_NO_DECL) == 0)
    {
	ಆದರೆ (!quiet)
	    semsg(_(e_variable_already_declared_str), lp->ll_name);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    // If the next character is a "." or a "[", then process the subitem.
    p = get_lval_subscript(lp, p, name, rettv, ht, v, unlet, flags, cl_exec);
    ಆದರೆ (p == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (vim9script && lp->ll_valtype != ಖಾಲಿ && rettv != ಖಾಲಿ)
    {
	where_T	    where = WHERE_INIT;

	// In a Vim9 script, do type check and make sure the variable is
	// writable.
	ಆದರೆ (check_typval_type(lp->ll_valtype, rettv, where) == FAIL)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    lp->ll_name_ಕೊನೆ = p;
    ಹಿಂತಿರುಗಿ p;
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
ಗಣ_var_lval(
    lval_T	*lp,
    char_u	*endp,
    typval_T	*rettv,
    ಪೂರ್ಣಾಂಕ		copy,
    ಪೂರ್ಣಾಂಕ		flags,	    // ASSIGN_CONST, ASSIGN_NO_DECL
    char_u	*op,
    ಪೂರ್ಣಾಂಕ		var_idx)    // index for "let [a, b] = list"
{
    ಪೂರ್ಣಾಂಕ		cc;
    dictitem_T	*di;

    ಆದರೆ (lp->ll_tv == ಖಾಲಿ)
    {
	cc = *endp;
	*endp = NUL;
	ಆದರೆ (ಒಳ_vim9script() && check_reserved_name(lp->ll_name, FALSE) == FAIL)
	    ಹಿಂತಿರುಗಿ;

	ಆದರೆ (lp->ll_blob != ಖಾಲಿ)
	{
	    ಪೂರ್ಣಾಂಕ	    error = FALSE;

	    ಆದರೆ (op != ಖಾಲಿ && *op != '=')
	    {
		semsg(_(e_wrong_variable_type_for_str_equal), op);
		ಹಿಂತಿರುಗಿ;
	    }
	    ಆದರೆ (ಮೌಲ್ಯ_check_ಬೀಗ(lp->ll_blob->bv_ಬೀಗ, lp->ll_name, FALSE))
		ಹಿಂತಿರುಗಿ;

	    ಆದರೆ (lp->ll_range && rettv->v_type == VAR_BLOB)
	    {
		ಆದರೆ (lp->ll_empty2)
		    lp->ll_n2 = blob_len(lp->ll_blob) - 1;

		ಆದರೆ (blob_ಗಣ_range(lp->ll_blob, lp->ll_n1, lp->ll_n2,
								rettv) == FAIL)
		    ಹಿಂತಿರುಗಿ;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		varnumber_T	val = tv_get_ಸಂಖ್ಯೆ_chk(rettv, &error);
		ಆದರೆ (!error)
		{
		    ಆದರೆ (val < 0 || val > 255)
			semsg(_(e_invalid_ಮೌಲ್ಯ_for_blob_nr), val);
		    ಇಲ್ಲದಿದ್ದರೆ
			blob_ಗಣ_append(lp->ll_blob, lp->ll_n1, val);
		}
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (op != ಖಾಲಿ && *op != '=')
	{
	    typval_T tv;

	    ಆದರೆ ((flags & (ನಿಯೋಜಿಸು_CONST | ನಿಯೋಜಿಸು_FINAL))
					     && (flags & ನಿಯೋಜಿಸು_FOR_LOOP) == 0)
	    {
		emsg(_(e_cannot_modify_existing_variable));
		*endp = cc;
		ಹಿಂತಿರುಗಿ;
	    }

	    // handle +=, -=, *=, /=, %= and .=
	    di = ಖಾಲಿ;
	    ಆದರೆ (eval_variable(lp->ll_name, (ಪೂರ್ಣಾಂಕ)STRLEN(lp->ll_name),
				 lp->ll_sid, &tv, &di, EVAL_VAR_VERBOSE) == OK)
	    {
		ಆದರೆ (di != ಖಾಲಿ && check_typval_is_ಮೌಲ್ಯ(&di->di_tv) == FAIL)
		{
		    clear_tv(&tv);
		    ಹಿಂತಿರುಗಿ;
		}

		// In Vim9 script a String v: variable only takes a String.
		ಆದರೆ (ಒಳ_vim9script() && STRNCMP(lp->ll_name, "v:", 2) == 0
			&& tv.v_type == VAR_STRING)
		{
		    where_T	where = WHERE_INIT;

		    ಆದರೆ (check_typval_type(&t_string, rettv, where) == FAIL)
		    {
			clear_tv(&tv);
			*endp = cc;
			ಹಿಂತಿರುಗಿ;
		    }
		}

		ಆದರೆ ((di == ಖಾಲಿ
			 || (!var_check_ro(di->di_flags, lp->ll_name, FALSE)
			   && !tv_check_ಬೀಗ(&di->di_tv, lp->ll_name, FALSE)))
			&& tv_op(&tv, rettv, op) == OK)
		    ಗಣ_var_const(lp->ll_name, lp->ll_sid, ಖಾಲಿ, &tv, FALSE,
				ನಿಯೋಜಿಸು_NO_DECL | ನಿಯೋಜಿಸು_COMPOUND_OP, 0);
		clear_tv(&tv);
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (lp->ll_type != ಖಾಲಿ && check_typval_arg_type(lp->ll_type, rettv,
							      ಖಾಲಿ, 0) == FAIL)
		ಹಿಂತಿರುಗಿ;
	    ಗಣ_var_const(lp->ll_name, lp->ll_sid, lp->ll_type, rettv, copy,
							       flags, var_idx);
	}
	*endp = cc;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಮೌಲ್ಯ_check_ಬೀಗ(lp->ll_newkey == ಖಾಲಿ
		? lp->ll_tv->v_ಬೀಗ
		: lp->ll_tv->vval.v_dict->dv_ಬೀಗ, lp->ll_name, FALSE))
	;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lp->ll_range)
    {
	ಆದರೆ ((flags & (ನಿಯೋಜಿಸು_CONST | ನಿಯೋಜಿಸು_FINAL))
					     && (flags & ನಿಯೋಜಿಸು_FOR_LOOP) == 0)
	{
	    emsg(_(e_cannot_ಬೀಗ_range));
	    ಹಿಂತಿರುಗಿ;
	}

	(ಶೂನ್ಯ)list_ನಿಯೋಜಿಸು_range(lp->ll_list, rettv->vval.v_list,
			 lp->ll_n1, lp->ll_n2, lp->ll_empty2, op, lp->ll_name);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	/*
	 * Assign to a List, Dictionary or Object item.
	 */
	ಆದರೆ ((flags & (ನಿಯೋಜಿಸು_CONST | ನಿಯೋಜಿಸು_FINAL))
					     && (flags & ನಿಯೋಜಿಸು_FOR_LOOP) == 0)
	{
	    emsg(_(e_cannot_ಬೀಗ_list_or_dict));
	    ಹಿಂತಿರುಗಿ;
	}

	ಆದರೆ (lp->ll_valtype != ಖಾಲಿ
		    && check_typval_arg_type(lp->ll_valtype, rettv,
							      ಖಾಲಿ, 0) == FAIL)
	{
	    lp->ll_name_ಕೊನೆ = ಖಾಲಿ;
	    ಹಿಂತಿರುಗಿ;
	}

	// If the lval is a List and the type of the list is not yet set,
	// then set the item type from the declared type of the variable.
	ಆದರೆ (ಒಳ_vim9script() && rettv->v_type == VAR_LIST
				&& rettv->vval.v_list != ಖಾಲಿ
				&& rettv->vval.v_list->lv_type == ಖಾಲಿ)
	{
	    ಆದರೆ (lp->ll_tv->v_type == VAR_LIST
		    && lp->ll_tv->vval.v_list != ಖಾಲಿ
		    && lp->ll_tv->vval.v_list->lv_type != ಖಾಲಿ)
		ಗಣ_tv_type(rettv, lp->ll_tv->vval.v_list->lv_type);
	}

	ಆದರೆ (lp->ll_newkey != ಖಾಲಿ)
	{
	    ಆದರೆ (op != ಖಾಲಿ && *op != '=')
	    {
		semsg(_(e_ಕೀಲಿ_not_present_ಒಳ_dictionary_str), lp->ll_newkey);
		ಹಿಂತಿರುಗಿ;
	    }
	    ಆದರೆ (dict_wrong_func_name(lp->ll_tv->vval.v_dict, rettv,
								lp->ll_newkey))
		ಹಿಂತಿರುಗಿ;

	    // Need to add an item to the Dictionary.
	    di = dictitem_alloc(lp->ll_newkey);
	    ಆದರೆ (di == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ;
	    ಆದರೆ (dict_ಕೂಡಿಸು(lp->ll_tv->vval.v_dict, di) == FAIL)
	    {
		vim_free(di);
		ಹಿಂತಿರುಗಿ;
	    }
	    lp->ll_tv = &di->di_tv;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (op != ಖಾಲಿ && *op != '=')
	{
	    tv_op(lp->ll_tv, rettv, op);
	    ಹಿಂತಿರುಗಿ;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    clear_tv(lp->ll_tv);

	/*
	 * Assign the value to the variable or list item.
	 */
	ಆದರೆ (copy)
	    copy_tv(rettv, lp->ll_tv);
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    *lp->ll_tv = *rettv;
	    lp->ll_tv->v_ಬೀಗ = 0;
	    init_tv(rettv);
	}
    }
}

/*
 * Handle "blob1 += blob2".
 * Returns OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
tv_op_blob(typval_T *tv1, typval_T *tv2, char_u *op)
{
    ಆದರೆ (*op != '+' || tv2->v_type != VAR_BLOB)
	ಹಿಂತಿರುಗಿ FAIL;

    // Blob += Blob
    ಆದರೆ (tv2->vval.v_blob == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ OK;

    ಆದರೆ (tv1->vval.v_blob == ಖಾಲಿ)
    {
	tv1->vval.v_blob = tv2->vval.v_blob;
	++tv1->vval.v_blob->bv_refcount;
	ಹಿಂತಿರುಗಿ OK;
    }

    blob_T	*b1 = tv1->vval.v_blob;
    blob_T	*b2 = tv2->vval.v_blob;
    ಪೂರ್ಣಾಂಕ		len = blob_len(b2);

    ಆದರೆ (len > 0 && ga_grow(&b1->bv_ga, len) == OK)
    {
	mch_memmove((char_u *)b1->bv_ga.ga_ದತ್ತಾಂಶ + b1->bv_ga.ga_len,
		    (char_u *)b2->bv_ga.ga_ದತ್ತಾಂಶ, (size_t)len);
	b1->bv_ga.ga_len += len;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < len; i++)
	    (ಶೂನ್ಯ)ga_append(&b1->bv_ga, blob_get(b2, i));

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Handle "list1 += list2".
 * Returns OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
tv_op_list(typval_T *tv1, typval_T *tv2, char_u *op)
{
    ಆದರೆ (*op != '+' || tv2->v_type != VAR_LIST)
	ಹಿಂತಿರುಗಿ FAIL;

    // List += List
    ಆದರೆ (tv2->vval.v_list == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ OK;

    ಆದರೆ (tv1->vval.v_list == ಖಾಲಿ)
    {
	tv1->vval.v_list = tv2->vval.v_list;
	++tv1->vval.v_list->lv_refcount;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	list_extend(tv1->vval.v_list, tv2->vval.v_list, ಖಾಲಿ);

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Handle number operations:
 *	nr += nr , nr -= nr , nr *=nr , nr /= nr , nr %= nr
 *
 * Returns OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
tv_op_ಸಂಖ್ಯೆ(typval_T *tv1, typval_T *tv2, char_u *op)
{
    varnumber_T	n;
    ಪೂರ್ಣಾಂಕ		failed = FALSE;

    n = tv_get_ಸಂಖ್ಯೆ(tv1);
    ಆದರೆ (tv2->v_type == VAR_FLOAT)
    {
	float_T f = n;

	ಆದರೆ (*op == '%')
	    ಹಿಂತಿರುಗಿ FAIL;
	ಆಯ್ಕೆ (*op)
	{
	    ಸ್ಥಿತಿ '+': f += tv2->vval.v_float; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ '-': f -= tv2->vval.v_float; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ '*': f *= tv2->vval.v_float; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ '/': f /= tv2->vval.v_float; ನಿಲ್ಲಿಸು;
	}
	clear_tv(tv1);
	tv1->v_type = VAR_FLOAT;
	tv1->vval.v_float = f;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆಯ್ಕೆ (*op)
	{
	    ಸ್ಥಿತಿ '+': n += tv_get_ಸಂಖ್ಯೆ(tv2); ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ '-': n -= tv_get_ಸಂಖ್ಯೆ(tv2); ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ '*': n *= tv_get_ಸಂಖ್ಯೆ(tv2); ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ '/': n = ಅಂಕೆ_divide(n, tv_get_ಸಂಖ್ಯೆ(tv2), &failed); ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ '%': n = ಅಂಕೆ_modulus(n, tv_get_ಸಂಖ್ಯೆ(tv2), &failed); ನಿಲ್ಲಿಸು;
	}
	clear_tv(tv1);
	tv1->v_type = VAR_ಸಂಖ್ಯೆ;
	tv1->vval.v_ಸಂಖ್ಯೆ = n;
    }

    ಹಿಂತಿರುಗಿ failed ? FAIL : OK;
}

/*
 * Append string "s2" to the string in "tv1".
 * Returns OK if "tv1" was grown in place, FAIL otherwise.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
grow_string_tv(typval_T *tv1, char_u *s2)
{
    ಆದರೆ (tv1->v_type != VAR_STRING || tv1->vval.v_string == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    size_t len1 = STRLEN(tv1->vval.v_string);
    size_t len2 = STRLEN(s2);
    char_u *p = vim_realloc(tv1->vval.v_string, len1 + len2 + 1);

    ಆದರೆ (p == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    mch_memmove(p + len1, s2, len2 + 1);
    tv1->vval.v_string = p;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Handle "str1 .= str2"
 * Returns OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
tv_op_string(typval_T *tv1, typval_T *tv2, char_u *op ಬಳಸದ)
{
    char_u	numbuf[NUMBUFLEN];
    char_u	*s;
    char_u	*s2;

    ಆದರೆ (tv2->v_type == VAR_FLOAT)
	ಹಿಂತಿರುಗಿ FAIL;

    // str .= str
    s2 = tv_get_string_buf(tv2, numbuf);
    ಆದರೆ (grow_string_tv(tv1, s2) == OK)
	ಹಿಂತಿರುಗಿ OK;

    s = tv_get_string(tv1);
    s = concat_str(s, s2);
    clear_tv(tv1);
    tv1->v_type = VAR_STRING;
    tv1->vval.v_string = s;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Handle "tv1 += tv2", "tv1 -= tv2", "tv1 *= tv2", "tv1 /= tv2", "tv1 %= tv2"
 * and "tv1 .= tv2"
 * Returns OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
tv_op_nr_or_string(typval_T *tv1, typval_T *tv2, char_u *op)
{
    ಆದರೆ (tv2->v_type == VAR_LIST)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (vim_strchr((char_u *)"+-*/%", *op) != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ tv_op_ಸಂಖ್ಯೆ(tv1, tv2, op);

    ಹಿಂತಿರುಗಿ tv_op_string(tv1, tv2, op);
}

/*
 * Handle "f1 += f2", "f1 -= f2", "f1 *= f2", "f1 /= f2".
 * Returns OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
tv_op_float(typval_T *tv1, typval_T *tv2, char_u *op)
{
    float_T f;

    ಆದರೆ (*op == '%' || *op == '.'
	    || (tv2->v_type != VAR_FLOAT
		&& tv2->v_type != VAR_ಸಂಖ್ಯೆ
		&& tv2->v_type != VAR_STRING))
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (tv2->v_type == VAR_FLOAT)
	f = tv2->vval.v_float;
    ಇಲ್ಲದಿದ್ದರೆ
	f = tv_get_ಸಂಖ್ಯೆ(tv2);
    ಆಯ್ಕೆ (*op)
    {
	ಸ್ಥಿತಿ '+': tv1->vval.v_float += f; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ '-': tv1->vval.v_float -= f; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ '*': tv1->vval.v_float *= f; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ '/': tv1->vval.v_float /= f; ನಿಲ್ಲಿಸು;
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Handle "tv1 += tv2", "tv1 -= tv2", "tv1 *= tv2", "tv1 /= tv2", "tv1 %= tv2"
 * and "tv1 .= tv2"
 * Returns OK or FAIL.
 */
    ಪೂರ್ಣಾಂಕ
tv_op(typval_T *tv1, typval_T *tv2, char_u *op)
{
    // Can't do anything with a Funcref or Dict on the right.
    // v:true and friends only work with "..=".
    ಆದರೆ (tv2->v_type == VAR_FUNC || tv2->v_type == VAR_DICT
		|| ((tv2->v_type == VAR_BOOL || tv2->v_type == VAR_SPECIAL)
							&& *op != '.'))
    {
	semsg(_(e_wrong_variable_type_for_str_equal), op);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಪೂರ್ಣಾಂಕ retval = FAIL;
    ಆಯ್ಕೆ (tv1->v_type)
    {
	ಸ್ಥಿತಿ VAR_UNKNOWN:
	ಸ್ಥಿತಿ VAR_ANY:
	ಸ್ಥಿತಿ VAR_VOID:
	ಸ್ಥಿತಿ VAR_DICT:
	ಸ್ಥಿತಿ VAR_FUNC:
	ಸ್ಥಿತಿ VAR_PARTIAL:
	ಸ್ಥಿತಿ VAR_BOOL:
	ಸ್ಥಿತಿ VAR_SPECIAL:
	ಸ್ಥಿತಿ VAR_ಕೆಲಸ:
	ಸ್ಥಿತಿ VAR_CHANNEL:
	ಸ್ಥಿತಿ VAR_INSTR:
	ಸ್ಥಿತಿ VAR_OBJECT:
	ಸ್ಥಿತಿ VAR_CLASS:
	ಸ್ಥಿತಿ VAR_TYPEALIAS:
	ಸ್ಥಿತಿ VAR_TUPLE:
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ VAR_BLOB:
	    retval = tv_op_blob(tv1, tv2, op);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ VAR_LIST:
	    retval = tv_op_list(tv1, tv2, op);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ VAR_ಸಂಖ್ಯೆ:
	ಸ್ಥಿತಿ VAR_STRING:
	    retval = tv_op_nr_or_string(tv1, tv2, op);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ VAR_FLOAT:
	    retval = tv_op_float(tv1, tv2, op);
	    ನಿಲ್ಲಿಸು;
    }

    ಆದರೆ (retval != OK)
	semsg(_(e_wrong_variable_type_for_str_equal), op);

    ಹಿಂತಿರುಗಿ retval;
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
    ಪೂರ್ಣಾಂಕ		*errp,
    exarg_T	*eap,
    evalarg_T	*evalarg)
{
    forinfo_T	*fi;
    char_u	*var_list_ಕೊನೆ;
    char_u	*ಅಭಿವ್ಯಕ್ತಿ;
    typval_T	tv;
    list_T	*l;
    tuple_T	*tuple;
    ಪೂರ್ಣಾಂಕ		skip = !(evalarg->eval_flags & EVAL_EVALUATE);

    *errp = TRUE;	// default: there is an error

    fi = ALLOC_CLEAR_ONE(forinfo_T);
    ಆದರೆ (fi == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    var_list_ಕೊನೆ = skip_var_list(arg, TRUE, &fi->fi_varcount,
						     &fi->fi_semicolon, FALSE);
    ಆದರೆ (var_list_ಕೊನೆ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ fi;

    ಅಭಿವ್ಯಕ್ತಿ = skipwhite_and_linebreak(var_list_ಕೊನೆ, evalarg);
    ಆದರೆ (ಅಭಿವ್ಯಕ್ತಿ[0] != 'i' || ಅಭಿವ್ಯಕ್ತಿ[1] != 'n'
				  || !(ಅಭಿವ್ಯಕ್ತಿ[2] == NUL || VIM_ISWHITE(ಅಭಿವ್ಯಕ್ತಿ[2])))
    {
	ಆದರೆ (ಒಳ_vim9script() && *ಅಭಿವ್ಯಕ್ತಿ == ':' && ಅಭಿವ್ಯಕ್ತಿ != var_list_ಕೊನೆ)
	    semsg(_(e_no_white_space_allowed_before_colon_str), ಅಭಿವ್ಯಕ್ತಿ);
	ಇಲ್ಲದಿದ್ದರೆ
	    emsg(_(e_missing_ಒಳ_after_for));
	ಹಿಂತಿರುಗಿ fi;
    }

    ಆದರೆ (skip)
	++emsg_skip;
    ಅಭಿವ್ಯಕ್ತಿ = skipwhite_and_linebreak(ಅಭಿವ್ಯಕ್ತಿ + 2, evalarg);
    ಆದರೆ (eval0(ಅಭಿವ್ಯಕ್ತಿ, &tv, eap, evalarg) == OK)
    {
	*errp = FALSE;
	ಆದರೆ (!skip)
	{
	    ಆದರೆ (tv.v_type == VAR_LIST)
	    {
		l = tv.vval.v_list;
		ಆದರೆ (l == ಖಾಲಿ)
		{
		    // a null list is like an empty list: do nothing
		    clear_tv(&tv);
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    // Need a real list here.
		    CHECK_LIST_MATERIALIZE(l);

		    // No need to increment the refcount, it's already set for
		    // the list being used in "tv".
		    fi->fi_list = l;
		    list_ಕೂಡಿಸು_watch(l, &fi->fi_lw);
		    fi->fi_lw.lw_ಅಂಶ = l->lv_ಮೊದಲ;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tv.v_type == VAR_TUPLE)
	    {
		tuple = tv.vval.v_tuple;
		ಆದರೆ (tuple == ಖಾಲಿ)
		{
		    // a null tuple is like an empty tuple: do nothing
		    clear_tv(&tv);
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    // No need to increment the refcount, it's already set for
		    // the tuple being used in "tv".
		    fi->fi_tuple = tuple;
		    fi->fi_tuple_idx = 0;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tv.v_type == VAR_BLOB)
	    {
		fi->fi_bi = 0;
		ಆದರೆ (tv.vval.v_blob != ಖಾಲಿ)
		{
		    // Make a copy, so that the iteration still works when the
		    // blob is changed.
		    fi->fi_blob = blob_copy(tv.vval.v_blob);
		}
		clear_tv(&tv);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tv.v_type == VAR_STRING)
	    {
		fi->fi_byte_idx = 0;
		fi->fi_string = tv.vval.v_string;
		tv.vval.v_string = ಖಾಲಿ;
		ಆದರೆ (fi->fi_string == ಖಾಲಿ)
		    fi->fi_string = vim_strsave((char_u *)"");
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		emsg(_(e_string_list_tuple_or_blob_required));
		clear_tv(&tv);
	    }
	}
    }
    ಆದರೆ (skip)
	--emsg_skip;
    fi->fi_break_ಎಣಿಕೆ = evalarg->eval_break_ಎಣಿಕೆ;

    ಹಿಂತಿರುಗಿ fi;
}

/*
 * Used when looping over a :for line, skip the "in expr" part.
 */
    ಶೂನ್ಯ
skip_for_ಸಾಲುಗಳು(ಶೂನ್ಯ *fi_void, evalarg_T *evalarg)
{
    forinfo_T	*fi = (forinfo_T *)fi_void;
    ಪೂರ್ಣಾಂಕ		i;

    ಗಾಗಿ (i = 0; i < fi->fi_break_ಎಣಿಕೆ; ++i)
	eval_ಮುಂದಿನ_ಸಾಲು(ಖಾಲಿ, evalarg);
}

/*
 * Use the first item in a ":for" list.  Advance to the next.
 * Assign the values to the variable (list).  "arg" points to the first one.
 * Return TRUE when a valid item was found, FALSE when at end of list or
 * something wrong.
 */
    ಪೂರ್ಣಾಂಕ
ಮುಂದಿನ_for_ಅಂಶ(ಶೂನ್ಯ *fi_void, char_u *arg)
{
    forinfo_T	*fi = (forinfo_T *)fi_void;
    ಪೂರ್ಣಾಂಕ		ಫಲಿತಾಂಶ;
    ಪೂರ್ಣಾಂಕ		flag = ನಿಯೋಜಿಸು_FOR_LOOP | (ಒಳ_vim9script()
			 ? (ನಿಯೋಜಿಸು_FINAL
			     // first round: error if variable exists
			     | (fi->fi_bi == 0 ? 0 : ನಿಯೋಜಿಸು_DECL)
			     | ನಿಯೋಜಿಸು_NO_MEMBER_TYPE
			     | ನಿಯೋಜಿಸು_UPDATE_ಖಂಡ_ID)
			 : 0);
    listitem_T	*ಅಂಶ;
    ಪೂರ್ಣಾಂಕ		skip_ನಿಯೋಜಿಸು = ಒಳ_vim9script() && arg[0] == '_'
						      && !eval_isnamec(arg[1]);

    ಆದರೆ (fi->fi_blob != ಖಾಲಿ)
    {
	typval_T	tv;

	ಆದರೆ (fi->fi_bi >= blob_len(fi->fi_blob))
	    ಹಿಂತಿರುಗಿ FALSE;
	tv.v_type = VAR_ಸಂಖ್ಯೆ;
	tv.v_ಬೀಗ = VAR_FIXED;
	tv.vval.v_ಸಂಖ್ಯೆ = blob_get(fi->fi_blob, fi->fi_bi);
	++fi->fi_bi;
	ಆದರೆ (skip_ನಿಯೋಜಿಸು)
	    ಹಿಂತಿರುಗಿ TRUE;
	ಹಿಂತಿರುಗಿ ex_let_vars(arg, &tv, TRUE, fi->fi_semicolon,
					    fi->fi_varcount, flag, ಖಾಲಿ) == OK;
    }

    ಆದರೆ (fi->fi_string != ಖಾಲಿ)
    {
	typval_T	tv;
	ಪೂರ್ಣಾಂಕ		len;

	len = mb_ptr2len(fi->fi_string + fi->fi_byte_idx);
	ಆದರೆ (len == 0)
	    ಹಿಂತಿರುಗಿ FALSE;
	tv.v_type = VAR_STRING;
	tv.v_ಬೀಗ = VAR_FIXED;
	tv.vval.v_string = vim_strnsave(fi->fi_string + fi->fi_byte_idx, len);
	fi->fi_byte_idx += len;
	++fi->fi_bi;
	ಆದರೆ (skip_ನಿಯೋಜಿಸು)
	    ಫಲಿತಾಂಶ = TRUE;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಫಲಿತಾಂಶ = ex_let_vars(arg, &tv, TRUE, fi->fi_semicolon,
					    fi->fi_varcount, flag, ಖಾಲಿ) == OK;
	vim_free(tv.vval.v_string);
	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }

    ಆದರೆ (fi->fi_tuple != ಖಾಲಿ)
    {
	typval_T	tv;

	ಆದರೆ (fi->fi_tuple_idx >= TUPLE_LEN(fi->fi_tuple))
	    ಹಿಂತಿರುಗಿ FALSE;

	copy_tv(TUPLE_ಅಂಶ(fi->fi_tuple, fi->fi_tuple_idx), &tv);
	++fi->fi_tuple_idx;
	++fi->fi_bi;
	ಆದರೆ (skip_ನಿಯೋಜಿಸು)
	    ಫಲಿತಾಂಶ = TRUE;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಫಲಿತಾಂಶ = ex_let_vars(arg, &tv, TRUE, fi->fi_semicolon,
					    fi->fi_varcount, flag, ಖಾಲಿ) == OK;
	clear_tv(&tv);
	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }

    ಅಂಶ = fi->fi_lw.lw_ಅಂಶ;
    ಆದರೆ (ಅಂಶ == ಖಾಲಿ)
	ಫಲಿತಾಂಶ = FALSE;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	fi->fi_lw.lw_ಅಂಶ = ಅಂಶ->li_ಮುಂದಿನ;
	++fi->fi_bi;
	ಆದರೆ (skip_ನಿಯೋಜಿಸು)
	    ಫಲಿತಾಂಶ = TRUE;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಫಲಿತಾಂಶ = (ex_let_vars(arg, &ಅಂಶ->li_tv, TRUE, fi->fi_semicolon,
					   fi->fi_varcount, flag, ಖಾಲಿ) == OK);
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * Free the structure used to store info used by ":for".
 */
    ಶೂನ್ಯ
free_for_info(ಶೂನ್ಯ *fi_void)
{
    forinfo_T    *fi = (forinfo_T *)fi_void;

    ಆದರೆ (fi == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (fi->fi_list != ಖಾಲಿ)
    {
	list_rem_watch(fi->fi_list, &fi->fi_lw);
	list_unref(fi->fi_list);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fi->fi_blob != ಖಾಲಿ)
	blob_unref(fi->fi_blob);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fi->fi_tuple != ಖಾಲಿ)
	tuple_unref(fi->fi_tuple);
    ಇಲ್ಲದಿದ್ದರೆ
	vim_free(fi->fi_string);
    vim_free(fi);
}

    ಶೂನ್ಯ
ಗಣ_context_for_ಅಭಿವ್ಯಕ್ತಿ(
    expand_T	*xp,
    char_u	*arg,
    cmdidx_T	cmdidx)
{
    ಪೂರ್ಣಾಂಕ		has_ಅಭಿವ್ಯಕ್ತಿ = cmdidx != CMD_let && cmdidx != CMD_var;
    ಪೂರ್ಣಾಂಕ		c;
    char_u	*p;

    ಆದರೆ (cmdidx == CMD_let || cmdidx == CMD_var
				 || cmdidx == CMD_const || cmdidx == CMD_final)
    {
	xp->xp_context = EXPAND_USER_VARS;
	ಆದರೆ (vim_strpbrk(arg, (char_u *)"\"'+-*/%.=!?~|&$([<>,#") == ಖಾಲಿ)
	{
	    // ":let var1 var2 ...": find last space.
	    ಗಾಗಿ (p = arg + STRLEN(arg); p >= arg; )
	    {
		xp->xp_ನಮೂನೆ = p;
		MB_PTR_ಹಿಂಭಾಗ(arg, p);
		ಆದರೆ (VIM_ISWHITE(*p))
		    ನಿಲ್ಲಿಸು;
	    }
	    ಹಿಂತಿರುಗಿ;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
	xp->xp_context = cmdidx == CMD_call ? EXPAND_FUNCTIONS
							  : EXPAND_ಅಭಿವ್ಯಕ್ತಿ;
    ಆಗ ((xp->xp_ನಮೂನೆ = vim_strpbrk(arg,
				  (char_u *)"\"'+-*/%.=!?~|&$([<>,#")) != ಖಾಲಿ)
    {
	c = *xp->xp_ನಮೂನೆ;
	ಆದರೆ (c == '&')
	{
	    c = xp->xp_ನಮೂನೆ[1];
	    ಆದರೆ (c == '&')
	    {
		++xp->xp_ನಮೂನೆ;
		xp->xp_context = has_ಅಭಿವ್ಯಕ್ತಿ ? EXPAND_ಅಭಿವ್ಯಕ್ತಿ : EXPAND_NOTHING;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c != ' ')
	    {
		xp->xp_context = EXPAND_SETTINGS;
		ಆದರೆ ((c == 'l' || c == 'g') && xp->xp_ನಮೂನೆ[2] == ':')
		    xp->xp_ನಮೂನೆ += 2;

	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == '$')
	{
	    // environment variable
	    xp->xp_context = EXPAND_ENV_VARS;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == '=')
	{
	    has_ಅಭಿವ್ಯಕ್ತಿ = TRUE;
	    xp->xp_context = EXPAND_ಅಭಿವ್ಯಕ್ತಿ;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == '#'
		&& xp->xp_context == EXPAND_ಅಭಿವ್ಯಕ್ತಿ)
	{
	    // Autoload function/variable contains '#'.
	    ನಿಲ್ಲಿಸು;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((c == '<' || c == '#')
		&& xp->xp_context == EXPAND_FUNCTIONS
		&& vim_strchr(xp->xp_ನಮೂನೆ, '(') == ಖಾಲಿ)
	{
	    // Function name can start with "<SNR>" and contain '#'.
	    ನಿಲ್ಲಿಸು;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (has_ಅಭಿವ್ಯಕ್ತಿ)
	{
	    ಆದರೆ (c == '"')	    // string
	    {
		ಆಗ ((c = *++xp->xp_ನಮೂನೆ) != NUL && c != '"')
		    ಆದರೆ (c == '\\' && xp->xp_ನಮೂನೆ[1] != NUL)
			++xp->xp_ನಮೂನೆ;
		xp->xp_context = EXPAND_NOTHING;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == '\'')	    // literal string
	    {
		// Trick: '' is like stopping and starting a literal string.
		ಆಗ ((c = *++xp->xp_ನಮೂನೆ) != NUL && c != '\'')
		    /* skip */ ;
		xp->xp_context = EXPAND_NOTHING;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == '|')
	    {
		ಆದರೆ (xp->xp_ನಮೂನೆ[1] == '|')
		{
		    ++xp->xp_ನಮೂನೆ;
		    xp->xp_context = EXPAND_ಅಭಿವ್ಯಕ್ತಿ;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    xp->xp_context = EXPAND_COMMANDS;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		xp->xp_context = EXPAND_ಅಭಿವ್ಯಕ್ತಿ;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    // Doesn't look like something valid, expand as an expression
	    // anyway.
	    xp->xp_context = EXPAND_ಅಭಿವ್ಯಕ್ತಿ;
	arg = xp->xp_ನಮೂನೆ;
	ಆದರೆ (*arg != NUL)
	    ಆಗ ((c = *++arg) != NUL && (c == ' ' || c == '\t'))
		/* skip */ ;
    }

    // ":exe one two" completes "two"
    ಆದರೆ ((cmdidx == CMD_execute
		|| cmdidx == CMD_echo
		|| cmdidx == CMD_echon
		|| cmdidx == CMD_echomsg
		|| cmdidx == CMD_echoerr
		|| cmdidx == CMD_echoconsole
		|| cmdidx == CMD_echowindow)
	    && xp->xp_context == EXPAND_ಅಭಿವ್ಯಕ್ತಿ)
    {
	ಗಾಗಿ (;;)
	{
	    char_u *n = skiptowhite(arg);

	    ಆದರೆ (n == arg || IS_WHITE_OR_NUL(*skipwhite(n)))
		ನಿಲ್ಲಿಸು;
	    arg = skipwhite(n);
	}
    }

    xp->xp_ನಮೂನೆ = arg;
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
ಸ್ಥಿರ regprog_T    *eval_prog_cache = ಖಾಲಿ;
ಸ್ಥಿರ char_u	    *eval_prog_pat = ಖಾಲಿ;	// pattern it was compiled for
ಸ್ಥಿರ char_u	    *eval_prog_enc = ಖಾಲಿ;	// 'encoding' when compiled
ಸ್ಥಿರ ದೀರ್ಘ	    eval_prog_re;		// 'regexpengine' when compiled
// State sampled by eval_regcomp() for the eval_regfree() call that gives the
// program back: user code run in between, e.g. an object's string() method
// invoked by match() on a list, can change these options, and then the
// program must not be cached under the changed values.  eval_compile_enc is
// only ever compared against p_enc, never dereferenced.
ಸ್ಥಿರ ದೀರ್ಘ	    eval_compile_re;
ಸ್ಥಿರ char_u	    *eval_compile_enc;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	    eval_prog_busy = 0;	// programs eval_regcomp() handed out
					// that were not given back yet
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	    eval_prog_reentered = FALSE;  // eval_regcomp() ran while
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

    ಆದರೆ (STRNCMP(pat, "\\%#=", 4) == 0)
	ಹಿಂತಿರುಗಿ true;
    ಗಾಗಿ (p = pat; *p != NUL; ++p)
	ಆದರೆ (*p == '~'
		|| (p[0] == '[' && (p[1] == ':' || p[1] == '=' || p[1] == '.'))
		|| (p[0] == '%' && (p[1] == '.'
			|| ((p[1] == '<' || p[1] == '>') && p[2] == '.'))))
	    ಹಿಂತಿರುಗಿ true;
    ಹಿಂತಿರುಗಿ false;
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
    ಆದರೆ (eval_prog_busy > 0)
	// The samples above no longer describe the program already handed
	// out: its eval_regfree() must drop it instead of caching it.
	eval_prog_reentered = TRUE;

    ಆದರೆ (eval_prog_cache != ಖಾಲಿ
	    && *p_cpo == NUL
	    && eval_prog_cache->re_flags == RE_MAGIC + RE_STRING
	    && eval_prog_re == p_re
	    && STRCMP(eval_prog_pat, pat) == 0
	    && STRCMP(eval_prog_enc, p_enc) == 0)
    {
	prog = eval_prog_cache;
	// The caller now owns the program, eval_regfree() adopts the
	// program the caller ends up with.
	eval_prog_cache = ಖಾಲಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	prog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
    ಆದರೆ (prog != ಖಾಲಿ)
	++eval_prog_busy;	// paired with the eval_regfree() call
    ಹಿಂತಿರುಗಿ prog;
}

/*
 * Free program "prog", obtained with eval_regcomp() for pattern "pat", by
 * keeping it in the cache for the next use.  "pat" must be the same string
 * the program was compiled for and must still be valid.
 */
    ಶೂನ್ಯ
eval_regfree(char_u *pat, regprog_T *prog)
{
    ಪೂರ್ಣಾಂಕ	    reentered = eval_prog_reentered;

    ಆದರೆ (eval_prog_busy > 0 && --eval_prog_busy == 0)
	eval_prog_reentered = FALSE;
    ಆದರೆ (prog == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (reentered || *p_cpo != NUL
	    || eval_compile_re != p_re || eval_compile_enc != p_enc)
    {
	// 'cpoptions' is not empty, an option in the cache key changed, or
	// another program was handed out while the caller was using this
	// one: the samples may not describe this program, do not cache it.
	vim_regfree(prog);
	ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (eval_prog_pat == ಖಾಲಿ || STRCMP(eval_prog_pat, pat) != 0
	    || STRCMP(eval_prog_enc, p_enc) != 0)
    {
	char_u	*pat_copy;
	char_u	*enc_copy;

	// A cached pattern is known not to be volatile, thus the scan is
	// only needed when installing a new key.
	ಆದರೆ (eval_prog_volatile(pat))
	{
	    // compiling depends on state the cache key does not cover
	    vim_regfree(prog);
	    ಹಿಂತಿರುಗಿ;
	}
	pat_copy = vim_strsave(pat);
	enc_copy = vim_strsave(p_enc);
	ಆದರೆ (pat_copy == ಖಾಲಿ || enc_copy == ಖಾಲಿ)
	{
	    vim_free(pat_copy);
	    vim_free(enc_copy);
	    vim_regfree(prog);
	    ಹಿಂತಿರುಗಿ;
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

#ಆದರೆ defined(EXITFREE)
    ಶೂನ್ಯ
free_eval_regcomp_cache(ಶೂನ್ಯ)
{
    vim_regfree(eval_prog_cache);
    eval_prog_cache = ಖಾಲಿ;
    VIM_CLEAR(eval_prog_pat);
    VIM_CLEAR(eval_prog_enc);
    eval_prog_busy = 0;
    eval_prog_reentered = FALSE;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Return TRUE if "pat" matches "text".
 * Does not use 'cpo' and always uses 'magic'.
 */
    ಪೂರ್ಣಾಂಕ
ನಮೂನೆ_ಹೊಂದಿಕೆ(char_u *pat, char_u *ಪಠ್ಯ, ಪೂರ್ಣಾಂಕ ic)
{
    ಪೂರ್ಣಾಂಕ		matches = FALSE;
    char_u	*save_cpo;
    regmatch_T	regmatch;

    // avoid 'l' flag in 'cpoptions'
    save_cpo = p_cpo;
    p_cpo = ಖಾಲಿಗಳು_option;
    regmatch.regprog = eval_regcomp(pat);
    ಆದರೆ (regmatch.regprog != ಖಾಲಿ)
    {
	regmatch.rm_ic = ic;
	matches = vim_regexec_nl(&regmatch, ಪಠ್ಯ, (colnr_T)0);
	eval_regfree(pat, regmatch.regprog);
    }
    p_cpo = save_cpo;
    ಹಿಂತಿರುಗಿ matches;
}

/*
 * Handle a name followed by "(".  Both for just "name(arg)" and for
 * "expr->name(arg)".
 * Returns OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
eval_func(
	char_u	    **arg,	// points to "(", will be advanced
	evalarg_T   *evalarg,
	char_u	    *name,
	ಪೂರ್ಣಾಂಕ	    name_len,
	typval_T    *rettv,
	ಪೂರ್ಣಾಂಕ	    flags,
	typval_T    *basetv)	// "expr" for "expr->name(arg)"
{
    ಪೂರ್ಣಾಂಕ		evaluate = flags & EVAL_EVALUATE;
    char_u	*s = name;
    ಪೂರ್ಣಾಂಕ		len = name_len;
    partial_T	*partial;
    ಪೂರ್ಣಾಂಕ		ret = OK;
    type_T	*type = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		ಸಿಕ್ಕಿತು_var = FALSE;

    ಆದರೆ (!evaluate)
	check_vars(s, len);

    // If "s" is the name of a variable of type VAR_FUNC
    // use its contents.
    s = deref_func_name(s, &len, &partial,
		 ಒಳ_vim9script() ? &type : ಖಾಲಿ, !evaluate, FALSE, &ಸಿಕ್ಕಿತು_var);

    // Need to make a copy, in case evaluating the arguments makes
    // the name invalid.
    s = vim_strsave(s);
    ಆದರೆ (s == ಖಾಲಿ || (evaluate && *s == NUL))
	ret = FAIL;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	funcexe_T funcexe;

	// Invoke the function.
	CLEAR_ಕ್ಷೇತ್ರ(funcexe);
	funcexe.fe_firstline = curwin->w_cursor.lnum;
	funcexe.fe_lastline = curwin->w_cursor.lnum;
	funcexe.fe_evaluate = evaluate;
	funcexe.fe_partial = partial;
	ಆದರೆ (partial != ಖಾಲಿ)
	{
	    funcexe.fe_object = partial->pt_obj;
	    ಆದರೆ (funcexe.fe_object != ಖಾಲಿ)
		++funcexe.fe_object->obj_refcount;
	}
	funcexe.fe_basetv = basetv;
	funcexe.fe_check_type = type;
	funcexe.fe_ಸಿಕ್ಕಿತು_var = ಸಿಕ್ಕಿತು_var;
	ಆದರೆ (evalarg != ಖಾಲಿ)
	    funcexe.fe_cctx = evalarg->eval_cctx;
	ret = get_func_tv(s, len, rettv, arg, evalarg, &funcexe);
    }
    vim_free(s);

    // If evaluate is FALSE rettv->v_type was not set in
    // get_func_tv, but it's needed in handle_subscript() to parse
    // what follows. So set it here.
    ಆದರೆ (rettv->v_type == VAR_UNKNOWN && !evaluate && **arg == '(')
    {
	rettv->vval.v_string = ಖಾಲಿ;
	rettv->v_type = VAR_FUNC;
    }

    // Stop the expression evaluation when immediately
    // aborting on error, or when an interrupt occurred or
    // an exception was thrown but not caught.
    ಆದರೆ (evaluate && aborting())
    {
	ಆದರೆ (ret == OK)
	    clear_tv(rettv);
	ret = FAIL;
    }
    ಹಿಂತಿರುಗಿ ret;
}

/*
 * After a NL, skip over empty lines and comment-only lines.
 */
    ಸ್ಥಿರ char_u *
newline_skip_comments(char_u *arg)
{
    char_u *p = arg + 1;

    ಗಾಗಿ (;;)
    {
	p = skipwhite(p);

	ಆದರೆ (*p == NUL)
	    ನಿಲ್ಲಿಸು;
	ಆದರೆ (vim9_comment_ಆರಂಭ(p))
	{
	    char_u *nl = vim_strchr(p, NL);

	    ಆದರೆ (nl == ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;
	    p = nl;
	}
	ಆದರೆ (*p != NL)
	    ನಿಲ್ಲಿಸು;
	++p;  // skip another NL
    }
    ಹಿಂತಿರುಗಿ p;
}

/*
 * Get the next line source line without advancing.  But do skip over comment
 * lines.
 * Only called for Vim9 script.
 */
    ಸ್ಥಿರ char_u *
getline_peek_skip_comments(evalarg_T *evalarg)
{
    ಗಾಗಿ (;;)
    {
	char_u *ಮುಂದಿನ = getline_peek(evalarg->eval_getline,
							 evalarg->eval_cookie);
	char_u *p;

	ಆದರೆ (ಮುಂದಿನ == ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;
	p = skipwhite(ಮುಂದಿನ);
	ಆದರೆ (*p != NUL && !vim9_comment_ಆರಂಭ(p))
	    ಹಿಂತಿರುಗಿ ಮುಂದಿನ;
	ಆದರೆ (eval_ಮುಂದಿನ_ಸಾಲು(ಖಾಲಿ, evalarg) == ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;
    }
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
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
eval_ಮುಂದಿನ_non_blank(char_u *arg, evalarg_T *evalarg, ಪೂರ್ಣಾಂಕ *getnext)
{
    char_u *p = skipwhite(arg);

    *getnext = FALSE;
    ಆದರೆ (ಒಳ_vim9script()
	    && evalarg != ಖಾಲಿ
	    && (evalarg->eval_cookie != ಖಾಲಿ || evalarg->eval_cctx != ಖಾಲಿ
								   || *p == NL)
	    && (*p == NUL || *p == NL
			     || (vim9_comment_ಆರಂಭ(p) && VIM_ISWHITE(p[-1]))))
    {
	char_u *ಮುಂದಿನ;

	ಆದರೆ (*p == NL)
	    ಮುಂದಿನ = newline_skip_comments(p);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (evalarg->eval_cookie != ಖಾಲಿ)
	    ಮುಂದಿನ = getline_peek_skip_comments(evalarg);
	ಇಲ್ಲದಿದ್ದರೆ
	    ಮುಂದಿನ = peek_ಮುಂದಿನ_ಸಾಲು_from_context(evalarg->eval_cctx);

	ಆದರೆ (ಮುಂದಿನ != ಖಾಲಿ)
	{
	    *getnext = *p != NL;
	    ಹಿಂತಿರುಗಿ skipwhite(ಮುಂದಿನ);
	}
    }
    ಹಿಂತಿರುಗಿ p;
}

/*
 * To be called after eval_next_non_blank() sets "getnext" to TRUE.
 * Only called for Vim9 script.
 *
 * If "arg" is not NULL, then the caller should assign the return value to
 * "arg".
 */
    char_u *
eval_ಮುಂದಿನ_ಸಾಲು(char_u *arg, evalarg_T *evalarg)
{
    garray_T	*gap = &evalarg->eval_ga;
    char_u	*ಸಾಲು;

    ಆದರೆ (arg != ಖಾಲಿ)
    {
	ಆದರೆ (*arg == NL)
	    ಹಿಂತಿರುಗಿ newline_skip_comments(arg);
	// Truncate before a trailing comment, so that concatenating the lines
	// won't turn the rest into a comment.
	ಆದರೆ (*skipwhite(arg) == '#')
	    *arg = NUL;
    }

    ಆದರೆ (evalarg->eval_cookie != ಖಾಲಿ)
	ಸಾಲು = evalarg->eval_getline(0, evalarg->eval_cookie, 0,
							   GETLINE_CONCAT_ALL);
    ಇಲ್ಲದಿದ್ದರೆ
	ಸಾಲು = ಮುಂದಿನ_ಸಾಲು_from_context(evalarg->eval_cctx, TRUE);
    ಆದರೆ (ಸಾಲು == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ++evalarg->eval_break_ಎಣಿಕೆ;
    ಆದರೆ (gap->ga_itemsize > 0 && ga_grow(gap, 1) == OK)
    {
	char_u *p = skipwhite(ಸಾಲು);

	// Going to concatenate the lines after parsing.  For an empty or
	// comment line use an empty string.
	ಆದರೆ (*p == NUL || vim9_comment_ಆರಂಭ(p))
	{
	    vim_free(ಸಾಲು);
	    ಸಾಲು = vim_strsave((char_u *)"");
	}

	((char_u **)gap->ga_ದತ್ತಾಂಶ)[gap->ga_len] = ಸಾಲು;
	++gap->ga_len;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (evalarg->eval_cookie != ಖಾಲಿ)
    {
	free_eval_tofree_later(evalarg);
	evalarg->eval_tofree = ಸಾಲು;
    }

    // Advanced to the next line, "arg" no longer points into the previous
    // line.  The caller assigns the return value to "arg".
    // If "arg" is NULL, then the return value is discarded.  In that case,
    // "arg" still points to the previous line.  So don't reset
    // "eval_using_cmdline".
    ಆದರೆ (arg != ಖಾಲಿ)
	evalarg->eval_using_cmdline = FALSE;
    ಹಿಂತಿರುಗಿ skipwhite(ಸಾಲು);
}

/*
 * Call eval_next_non_blank() and get the next line if needed.
 */
    char_u *
skipwhite_and_linebreak(char_u *arg, evalarg_T *evalarg)
{
    ಪೂರ್ಣಾಂಕ	    getnext;
    char_u  *p = skipwhite_and_nl(arg);

    ಆದರೆ (evalarg == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ skipwhite(arg);
    eval_ಮುಂದಿನ_non_blank(p, evalarg, &getnext);
    ಆದರೆ (getnext)
	ಹಿಂತಿರುಗಿ eval_ಮುಂದಿನ_ಸಾಲು(arg, evalarg);
    ಹಿಂತಿರುಗಿ p;
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
    ಪೂರ್ಣಾಂಕ
eval0(
    char_u	*arg,
    typval_T	*rettv,
    exarg_T	*eap,
    evalarg_T	*evalarg)
{
    ಹಿಂತಿರುಗಿ eval0_retarg(arg, rettv, eap, evalarg, ಖಾಲಿ);
}

/*
 * If "arg" is a simple function call without arguments then call it and return
 * the result.  Otherwise return NOTDONE.
 */
    ಪೂರ್ಣಾಂಕ
may_call_simple_func(
    char_u	*arg,
    typval_T	*rettv)
{
    char_u  *parens = (char_u *)ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು((ಅಕ್ಷರ *)arg, "()");
    ಪೂರ್ಣಾಂಕ	    r = NOTDONE;

    // If the expression is "FuncName()" then we can skip a lot of overhead.
    ಆದರೆ (parens != ಖಾಲಿ && *skipwhite(parens + 2) == NUL)
    {
	char_u *p = STRNCMP(arg, "<SNR>", 5) == 0 ? skipdigits(arg + 5) : arg;

	ಆದರೆ (ಗಮ್ಯ_name_ಕೊನೆ(p, TRUE) == parens)
	    r = call_simple_func(arg, (size_t)(parens - arg), rettv);
    }
    ಹಿಂತಿರುಗಿ r;
}

/*
 * Handle zero level expression with optimization for a simple function call.
 * Same arguments and return value as eval0().
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
eval0_simple_funccal(
    char_u	*arg,
    typval_T	*rettv,
    exarg_T	*eap,
    evalarg_T	*evalarg)
{
    ಪೂರ್ಣಾಂಕ	    r = may_call_simple_func(arg, rettv);

    ಆದರೆ (r == NOTDONE)
	r = eval0_retarg(arg, rettv, eap, evalarg, ಖಾಲಿ);
    ಹಿಂತಿರುಗಿ r;
}

/*
 * Like eval0() but when "retarg" is not NULL store the pointer to after the
 * expression and don't check what comes after the expression.
 */
    ಪೂರ್ಣಾಂಕ
eval0_retarg(
    char_u	*arg,
    typval_T	*rettv,
    exarg_T	*eap,
    evalarg_T	*evalarg,
    char_u	**retarg)
{
    ಪೂರ್ಣಾಂಕ		ret;
    char_u	*p;
    char_u	*ಅಭಿವ್ಯಕ್ತಿ_ಕೊನೆ;
    ಪೂರ್ಣಾಂಕ		did_emsg_before = did_emsg;
    ಪೂರ್ಣಾಂಕ		called_emsg_before = called_emsg;
    ಪೂರ್ಣಾಂಕ		check_for_ಕೊನೆ = retarg == ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		ಕೊನೆ_error = FALSE;

    p = skipwhite(arg);
    ret = eval1(&p, rettv, evalarg);

    ಆದರೆ (ret != FAIL)
    {
	ಅಭಿವ್ಯಕ್ತಿ_ಕೊನೆ = p;
	p = skipwhite(p);

	// In Vim9 script a command block is not split at NL characters for
	// commands using an expression argument.  Skip over a '#' comment to
	// check for a following NL.  Require white space before the '#'.
	ಆದರೆ (ಒಳ_vim9script() && p > ಅಭಿವ್ಯಕ್ತಿ_ಕೊನೆ && retarg == ಖಾಲಿ)
	    ಆಗ (*p == '#')
	    {
		char_u *nl = vim_strchr(p, NL);

		ಆದರೆ (nl == ಖಾಲಿ)
		    ನಿಲ್ಲಿಸು;
		p = skipwhite(nl + 1);
		ಆದರೆ (eap != ಖಾಲಿ && *p != NUL)
		    eap->nextcmd = p;
		check_for_ಕೊನೆ = FALSE;
	    }

	ಆದರೆ (check_for_ಕೊನೆ)
	    ಕೊನೆ_error = !ತುದಿಗಳು_excmd2(arg, p);
    }

    ಆದರೆ (ret == FAIL || ಕೊನೆ_error)
    {
	ಆದರೆ (ret != FAIL)
	    clear_tv(rettv);
	/*
	 * Report the invalid expression unless the expression evaluation has
	 * been cancelled due to an aborting error, an interrupt, or an
	 * exception, or we already gave a more specific error.
	 * Also check called_emsg for when using assert_fails().
	 */
	ಆದರೆ (!aborting()
		&& did_emsg == did_emsg_before
		&& called_emsg == called_emsg_before
		&& (!ಒಳ_vim9script() || !vim9_bad_comment(p)))
	{
	    ಆದರೆ (ಕೊನೆ_error)
		semsg(_(e_trailing_characters_str), p);
	    ಇಲ್ಲದಿದ್ದರೆ
		semsg(_(e_invalid_ಅಭಿವ್ಯಕ್ತಿ_str), arg);
	}

	ಆದರೆ (eap != ಖಾಲಿ && p != ಖಾಲಿ)
	{
	    // Some of the expression may not have been consumed.
	    // Only execute a next command if it cannot be a "||" operator.
	    // The next command may be "catch".
	    char_u *nextcmd = check_nextcmd(p);
	    ಆದರೆ (nextcmd != ಖಾಲಿ && *nextcmd != '|')
		eap->nextcmd = nextcmd;
	}
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಆದರೆ (retarg != ಖಾಲಿ)
	*retarg = p;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (check_for_ಕೊನೆ && eap != ಖಾಲಿ)
	ಗಣ_nextcmd(eap, p);

    ಹಿಂತಿರುಗಿ ret;
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
    ಪೂರ್ಣಾಂಕ
eval1(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    char_u  *p;
    ಪೂರ್ಣಾಂಕ	    getnext;

    CLEAR_ಸೂಚಕ(rettv);

    /*
     * Get the first variable.
     */
    ಆದರೆ (eval2(arg, rettv, evalarg) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    p = eval_ಮುಂದಿನ_non_blank(*arg, evalarg, &getnext);
    ಆದರೆ (*p == '?')
    {
	ಪೂರ್ಣಾಂಕ		op_falsy = p[1] == '?';
	ಪೂರ್ಣಾಂಕ		ಫಲಿತಾಂಶ;
	typval_T	var2;
	evalarg_T	*evalarg_ಬಳಸಿದ = evalarg;
	evalarg_T	local_evalarg;
	ಪೂರ್ಣಾಂಕ		orig_flags;
	ಪೂರ್ಣಾಂಕ		evaluate;
	ಪೂರ್ಣಾಂಕ		vim9script = ಒಳ_vim9script();

	ಆದರೆ (evalarg == ಖಾಲಿ)
	{
	    init_evalarg(&local_evalarg);
	    evalarg_ಬಳಸಿದ = &local_evalarg;
	}
	orig_flags = evalarg_ಬಳಸಿದ->eval_flags;
	evaluate = evalarg_ಬಳಸಿದ->eval_flags & EVAL_EVALUATE;

	ಆದರೆ (getnext)
	    *arg = eval_ಮುಂದಿನ_ಸಾಲು(*arg, evalarg_ಬಳಸಿದ);
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (evaluate && vim9script && !VIM_ISWHITE(p[-1]))
	    {
		error_white_both(p, op_falsy ? 2 : 1);
		clear_tv(rettv);
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    *arg = p;
	}

	ಫಲಿತಾಂಶ = FALSE;
	ಆದರೆ (evaluate)
	{
	    ಪೂರ್ಣಾಂಕ		error = FALSE;

	    ಆದರೆ (op_falsy)
	    {
		// Is this typeval supported with the falsy operator?
		ಆದರೆ (check_typval_is_ಮೌಲ್ಯ(rettv) == FAIL)
		{
		    clear_tv(rettv);
		    ಹಿಂತಿರುಗಿ FAIL;
		}
		ಫಲಿತಾಂಶ = tv2bool(rettv);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vim9script)
		ಫಲಿತಾಂಶ = tv_get_bool_chk(rettv, &error);
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tv_get_ಸಂಖ್ಯೆ_chk(rettv, &error) != 0)
		ಫಲಿತಾಂಶ = TRUE;
	    ಆದರೆ (error || !op_falsy || !ಫಲಿತಾಂಶ)
		clear_tv(rettv);
	    ಆದರೆ (error)
		ಹಿಂತಿರುಗಿ FAIL;
	}

	/*
	 * Get the second variable.  Recursive!
	 */
	ಆದರೆ (op_falsy)
	    ++*arg;
	ಆದರೆ (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[1]))
	{
	    error_white_both(*arg - (op_falsy ? 1 : 0), op_falsy ? 2 : 1);
	    clear_tv(rettv);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	*arg = skipwhite_and_linebreak(*arg + 1, evalarg_ಬಳಸಿದ);
	evalarg_ಬಳಸಿದ->eval_flags = (op_falsy ? !ಫಲಿತಾಂಶ : ಫಲಿತಾಂಶ)
				  ? orig_flags : (orig_flags & ~EVAL_EVALUATE);
	ಆದರೆ (eval1(arg, &var2, evalarg_ಬಳಸಿದ) == FAIL)
	{
	    evalarg_ಬಳಸಿದ->eval_flags = orig_flags;
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	ಆದರೆ (!op_falsy || !ಫಲಿತಾಂಶ)
	    *rettv = var2;

	ಆದರೆ (!op_falsy)
	{
	    /*
	     * Check for the ":".
	     */
	    p = eval_ಮುಂದಿನ_non_blank(*arg, evalarg_ಬಳಸಿದ, &getnext);
	    ಆದರೆ (*p != ':')
	    {
		emsg(_(e_missing_colon_after_questionmark));
		ಆದರೆ (evaluate && ಫಲಿತಾಂಶ)
		    clear_tv(rettv);
		evalarg_ಬಳಸಿದ->eval_flags = orig_flags;
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    ಆದರೆ (getnext)
		*arg = eval_ಮುಂದಿನ_ಸಾಲು(*arg, evalarg_ಬಳಸಿದ);
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ (evaluate && vim9script && !VIM_ISWHITE(p[-1]))
		{
		    error_white_both(p, 1);
		    clear_tv(rettv);
		    evalarg_ಬಳಸಿದ->eval_flags = orig_flags;
		    ಹಿಂತಿರುಗಿ FAIL;
		}
		*arg = p;
	    }

	    /*
	     * Get the third variable.  Recursive!
	     */
	    ಆದರೆ (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[1]))
	    {
		error_white_both(*arg, 1);
		clear_tv(rettv);
		evalarg_ಬಳಸಿದ->eval_flags = orig_flags;
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    *arg = skipwhite_and_linebreak(*arg + 1, evalarg_ಬಳಸಿದ);
	    evalarg_ಬಳಸಿದ->eval_flags = !ಫಲಿತಾಂಶ ? orig_flags
					       : (orig_flags & ~EVAL_EVALUATE);
	    ಆದರೆ (eval1(arg, &var2, evalarg_ಬಳಸಿದ) == FAIL)
	    {
		ಆದರೆ (evaluate && ಫಲಿತಾಂಶ)
		    clear_tv(rettv);
		evalarg_ಬಳಸಿದ->eval_flags = orig_flags;
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    ಆದರೆ (evaluate && !ಫಲಿತಾಂಶ)
		*rettv = var2;
	}

	ಆದರೆ (evalarg == ಖಾಲಿ)
	    clear_evalarg(&local_evalarg, ಖಾಲಿ);
	ಇಲ್ಲದಿದ್ದರೆ
	    evalarg->eval_flags = orig_flags;
    }

    ಹಿಂತಿರುಗಿ OK;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
eval2(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		getnext;

    /*
     * Get the first expression.
     */
    ಆದರೆ (eval3(arg, rettv, evalarg) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    /*
     * Handle the  "||" operator.
     */
    p = eval_ಮುಂದಿನ_non_blank(*arg, evalarg, &getnext);
    ಆದರೆ (p[0] == '|' && p[1] == '|')
    {
	evalarg_T   *evalarg_ಬಳಸಿದ = evalarg;
	evalarg_T   local_evalarg;
	ಪೂರ್ಣಾಂಕ	    evaluate;
	ಪೂರ್ಣಾಂಕ	    orig_flags;
	ದೀರ್ಘ	    ಫಲಿತಾಂಶ = FALSE;
	typval_T    var2;
	ಪೂರ್ಣಾಂಕ	    error = FALSE;
	ಪೂರ್ಣಾಂಕ	    vim9script = ಒಳ_vim9script();

	ಆದರೆ (evalarg == ಖಾಲಿ)
	{
	    init_evalarg(&local_evalarg);
	    evalarg_ಬಳಸಿದ = &local_evalarg;
	}
	orig_flags = evalarg_ಬಳಸಿದ->eval_flags;
	evaluate = orig_flags & EVAL_EVALUATE;
	ಆದರೆ (evaluate)
	{
	    ಆದರೆ (vim9script)
		ಫಲಿತಾಂಶ = tv_get_bool_chk(rettv, &error);
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tv_get_ಸಂಖ್ಯೆ_chk(rettv, &error) != 0)
		ಫಲಿತಾಂಶ = TRUE;
	    clear_tv(rettv);
	    ಆದರೆ (error)
		ಹಿಂತಿರುಗಿ FAIL;
	}

	/*
	 * Repeat until there is no following "||".
	 */
	ಆಗ (p[0] == '|' && p[1] == '|')
	{
	    ಆದರೆ (getnext)
		*arg = eval_ಮುಂದಿನ_ಸಾಲು(*arg, evalarg_ಬಳಸಿದ);
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ (evaluate && vim9script && !VIM_ISWHITE(p[-1]))
		{
		    error_white_both(p, 2);
		    clear_tv(rettv);
		    ಹಿಂತಿರುಗಿ FAIL;
		}
		*arg = p;
	    }

	    /*
	     * Get the second variable.
	     */
	    ಆದರೆ (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[2]))
	    {
		error_white_both(*arg, 2);
		clear_tv(rettv);
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    *arg = skipwhite_and_linebreak(*arg + 2, evalarg_ಬಳಸಿದ);
	    evalarg_ಬಳಸಿದ->eval_flags = !ಫಲಿತಾಂಶ ? orig_flags
					       : (orig_flags & ~EVAL_EVALUATE);
	    ಆದರೆ (eval3(arg, &var2, evalarg_ಬಳಸಿದ) == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;

	    /*
	     * Compute the result.
	     */
	    ಆದರೆ (evaluate && !ಫಲಿತಾಂಶ)
	    {
		ಆದರೆ (vim9script)
		    ಫಲಿತಾಂಶ = tv_get_bool_chk(&var2, &error);
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tv_get_ಸಂಖ್ಯೆ_chk(&var2, &error) != 0)
		    ಫಲಿತಾಂಶ = TRUE;
		clear_tv(&var2);
		ಆದರೆ (error)
		    ಹಿಂತಿರುಗಿ FAIL;
	    }
	    ಆದರೆ (evaluate)
	    {
		ಆದರೆ (vim9script)
		{
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_ಸಂಖ್ಯೆ = ಫಲಿತಾಂಶ ? VVAL_TRUE : VVAL_FALSE;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    rettv->v_type = VAR_ಸಂಖ್ಯೆ;
		    rettv->vval.v_ಸಂಖ್ಯೆ = ಫಲಿತಾಂಶ;
		}
	    }

	    p = eval_ಮುಂದಿನ_non_blank(*arg, evalarg_ಬಳಸಿದ, &getnext);
	}

	ಆದರೆ (evalarg == ಖಾಲಿ)
	    clear_evalarg(&local_evalarg, ಖಾಲಿ);
	ಇಲ್ಲದಿದ್ದರೆ
	    evalarg->eval_flags = orig_flags;
    }

    ಹಿಂತಿರುಗಿ OK;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
eval3(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		getnext;

    /*
     * Get the first expression.
     */
    ಆದರೆ (eval4(arg, rettv, evalarg) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    /*
     * Handle the "&&" operator.
     */
    p = eval_ಮುಂದಿನ_non_blank(*arg, evalarg, &getnext);
    ಆದರೆ (p[0] == '&' && p[1] == '&')
    {
	evalarg_T   *evalarg_ಬಳಸಿದ = evalarg;
	evalarg_T   local_evalarg;
	ಪೂರ್ಣಾಂಕ	    orig_flags;
	ಪೂರ್ಣಾಂಕ	    evaluate;
	ದೀರ್ಘ	    ಫಲಿತಾಂಶ = TRUE;
	typval_T    var2;
	ಪೂರ್ಣಾಂಕ	    error = FALSE;
	ಪೂರ್ಣಾಂಕ	    vim9script = ಒಳ_vim9script();

	ಆದರೆ (evalarg == ಖಾಲಿ)
	{
	    init_evalarg(&local_evalarg);
	    evalarg_ಬಳಸಿದ = &local_evalarg;
	}
	orig_flags = evalarg_ಬಳಸಿದ->eval_flags;
	evaluate = orig_flags & EVAL_EVALUATE;
	ಆದರೆ (evaluate)
	{
	    ಆದರೆ (vim9script)
		ಫಲಿತಾಂಶ = tv_get_bool_chk(rettv, &error);
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tv_get_ಸಂಖ್ಯೆ_chk(rettv, &error) == 0)
		ಫಲಿತಾಂಶ = FALSE;
	    clear_tv(rettv);
	    ಆದರೆ (error)
		ಹಿಂತಿರುಗಿ FAIL;
	}

	/*
	 * Repeat until there is no following "&&".
	 */
	ಆಗ (p[0] == '&' && p[1] == '&')
	{
	    ಆದರೆ (getnext)
		*arg = eval_ಮುಂದಿನ_ಸಾಲು(*arg, evalarg_ಬಳಸಿದ);
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ (evaluate && vim9script && !VIM_ISWHITE(p[-1]))
		{
		    error_white_both(p, 2);
		    clear_tv(rettv);
		    ಹಿಂತಿರುಗಿ FAIL;
		}
		*arg = p;
	    }

	    /*
	     * Get the second variable.
	     */
	    ಆದರೆ (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[2]))
	    {
		error_white_both(*arg, 2);
		clear_tv(rettv);
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    *arg = skipwhite_and_linebreak(*arg + 2, evalarg_ಬಳಸಿದ);
	    evalarg_ಬಳಸಿದ->eval_flags = ಫಲಿತಾಂಶ ? orig_flags
					      : (orig_flags & ~EVAL_EVALUATE);
	    CLEAR_ಕ್ಷೇತ್ರ(var2);
	    ಆದರೆ (eval4(arg, &var2, evalarg_ಬಳಸಿದ) == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;

	    /*
	     * Compute the result.
	     */
	    ಆದರೆ (evaluate && ಫಲಿತಾಂಶ)
	    {
		ಆದರೆ (vim9script)
		    ಫಲಿತಾಂಶ = tv_get_bool_chk(&var2, &error);
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tv_get_ಸಂಖ್ಯೆ_chk(&var2, &error) == 0)
		    ಫಲಿತಾಂಶ = FALSE;
		clear_tv(&var2);
		ಆದರೆ (error)
		    ಹಿಂತಿರುಗಿ FAIL;
	    }
	    ಆದರೆ (evaluate)
	    {
		ಆದರೆ (vim9script)
		{
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_ಸಂಖ್ಯೆ = ಫಲಿತಾಂಶ ? VVAL_TRUE : VVAL_FALSE;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    rettv->v_type = VAR_ಸಂಖ್ಯೆ;
		    rettv->vval.v_ಸಂಖ್ಯೆ = ಫಲಿತಾಂಶ;
		}
	    }

	    p = eval_ಮುಂದಿನ_non_blank(*arg, evalarg_ಬಳಸಿದ, &getnext);
	}

	ಆದರೆ (evalarg == ಖಾಲಿ)
	    clear_evalarg(&local_evalarg, ಖಾಲಿ);
	ಇಲ್ಲದಿದ್ದರೆ
	    evalarg->eval_flags = orig_flags;
    }

    ಹಿಂತಿರುಗಿ OK;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
eval4(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		getnext;
    exprtype_T	type = ಅಭಿವ್ಯಕ್ತಿ_UNKNOWN;
    ಪೂರ್ಣಾಂಕ		len = 2;
    ಪೂರ್ಣಾಂಕ		type_is = FALSE;

    /*
     * Get the first expression.
     */
    ಆದರೆ (eval5(arg, rettv, evalarg) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    p = eval_ಮುಂದಿನ_non_blank(*arg, evalarg, &getnext);

    type = get_compare_type(p, &len, &type_is);

    /*
     * If there is a comparative operator, use it.
     */
    ಆದರೆ (type != ಅಭಿವ್ಯಕ್ತಿ_UNKNOWN)
    {
	typval_T    var2;
	ಪೂರ್ಣಾಂಕ	    ic;
	ಪೂರ್ಣಾಂಕ	    vim9script = ಒಳ_vim9script();
	ಪೂರ್ಣಾಂಕ	    evaluate = evalarg == ಖಾಲಿ
				   ? 0 : (evalarg->eval_flags & EVAL_EVALUATE);
	ದೀರ್ಘ	    comp_lnum = SOURCING_LNUM;

	ಆದರೆ (getnext)
	{
	    *arg = eval_ಮುಂದಿನ_ಸಾಲು(*arg, evalarg);
	    p = *arg;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (evaluate && vim9script && !VIM_ISWHITE(**arg))
	{
	    error_white_both(*arg, len);
	    clear_tv(rettv);
	    ಹಿಂತಿರುಗಿ FAIL;
	}

	ಆದರೆ (vim9script && type_is && (p[len] == '?' || p[len] == '#'))
	{
	    semsg(_(e_invalid_ಅಭಿವ್ಯಕ್ತಿ_str), p);
	    clear_tv(rettv);
	    ಹಿಂತಿರುಗಿ FAIL;
	}

	// extra question mark appended: ignore case
	ಆದರೆ (p[len] == '?')
	{
	    ic = TRUE;
	    ++len;
	}
	// extra '#' appended: match case
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p[len] == '#')
	{
	    ic = FALSE;
	    ++len;
	}
	// nothing appended: use 'ignorecase' if not in Vim script
	ಇಲ್ಲದಿದ್ದರೆ
	    ic = vim9script ? FALSE : p_ic;

	/*
	 * Get the second variable.
	 */
	ಆದರೆ (evaluate && vim9script && !IS_WHITE_OR_NUL(p[len]))
	{
	    error_white_both(p, len);
	    clear_tv(rettv);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	*arg = skipwhite_and_linebreak(p + len, evalarg);
	ಆದರೆ (eval5(arg, &var2, evalarg) == FAIL)
	{
	    clear_tv(rettv);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	ಆದರೆ (evaluate)
	{
	    ಪೂರ್ಣಾಂಕ ret;

	    // use the line of the comparison for messages
	    SOURCING_LNUM = comp_lnum;
	    ಆದರೆ (vim9script && check_compare_types(type, rettv, &var2) == FAIL)
	    {
		ret = FAIL;
		clear_tv(rettv);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		ret = typval_compare(rettv, &var2, type, ic);
	    clear_tv(&var2);
	    ಹಿಂತಿರುಗಿ ret;
	}
    }

    ಹಿಂತಿರುಗಿ OK;
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
    ದೀರ್ಘ    len1, len2;
    ದೀರ್ಘ    totallen;

    ಆದರೆ (b == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    len1 = blob_len(b1);
    len2 = blob_len(b2);
    totallen = len1 + len2;

    ಆದರೆ (totallen > 0 && totallen <= ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ
				    && ga_grow(&b->bv_ga, (ಪೂರ್ಣಾಂಕ)totallen) == OK)
    {
	ಆದರೆ (len1 > 0)
	    mch_memmove((char_u *)b->bv_ga.ga_ದತ್ತಾಂಶ,
		    (char_u *)b1->bv_ga.ga_ದತ್ತಾಂಶ, (size_t)len1);
	ಆದರೆ (len2 > 0)
	    mch_memmove((char_u *)b->bv_ga.ga_ದತ್ತಾಂಶ + len1,
		    (char_u *)b2->bv_ga.ga_ದತ್ತಾಂಶ, (size_t)len2);
	b->bv_ga.ga_len = (ಪೂರ್ಣಾಂಕ)totallen;
    }

    clear_tv(tv1);
    rettv_blob_ಗಣ(tv1, b);
}

/*
 * Make a copy of list "tv1" and append list "tv2".
 */
    ಪೂರ್ಣಾಂಕ
eval_addlist(typval_T *tv1, typval_T *tv2)
{
    typval_T var3;

    // concatenate Lists
    ಆದರೆ (list_concat(tv1->vval.v_list, tv2->vval.v_list, &var3) == FAIL)
    {
	clear_tv(tv1);
	clear_tv(tv2);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    clear_tv(tv1);
    *tv1 = var3;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Make a copy of tuple "tv1" and append tuple "tv2".
 */
    ಪೂರ್ಣಾಂಕ
eval_addtuple(typval_T *tv1, typval_T *tv2)
{
    ಪೂರ್ಣಾಂಕ		vim9script = ಒಳ_vim9script();
    typval_T	var3;

    ಆದರೆ (vim9script && tv1->vval.v_tuple != ಖಾಲಿ && tv2->vval.v_tuple != ಖಾಲಿ
	    && tv1->vval.v_tuple->tv_type != ಖಾಲಿ
	    && tv2->vval.v_tuple->tv_type != ಖಾಲಿ)
    {
	ಆದರೆ (!check_tuples_addable(tv1->vval.v_tuple->tv_type,
						tv2->vval.v_tuple->tv_type))
	    ಹಿಂತಿರುಗಿ FAIL;
    }

    // concatenate tuples
    ಆದರೆ (tuple_concat(tv1->vval.v_tuple, tv2->vval.v_tuple, &var3) == FAIL)
    {
	clear_tv(tv1);
	clear_tv(tv2);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    clear_tv(tv1);
    *tv1 = var3;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Left or right shift the number "tv1" by the number "tv2" and store the
 * result in "tv1".
 *
 * Return OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
eval_ಸರಿಸು_ಸಂಖ್ಯೆ(typval_T *tv1, typval_T *tv2, ಪೂರ್ಣಾಂಕ ಸರಿಸು_type)
{
    ಆದರೆ (tv2->v_type != VAR_ಸಂಖ್ಯೆ || tv2->vval.v_ಸಂಖ್ಯೆ < 0)
    {
	// right operand should be a positive number
	ಆದರೆ (tv2->v_type != VAR_ಸಂಖ್ಯೆ)
	    emsg(_(e_bitshift_ops_must_be_ಸಂಖ್ಯೆ));
	ಇಲ್ಲದಿದ್ದರೆ
	    emsg(_(e_bitshift_ops_must_be_ಧನಾತ್ಮಕ));
	clear_tv(tv1);
	clear_tv(tv2);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಆದರೆ (tv2->vval.v_ಸಂಖ್ಯೆ > MAX_LSHIFT_ಬಿಟ್ಟುಗಳು)
	// shifting more bits than we have always results in zero
	tv1->vval.v_ಸಂಖ್ಯೆ = 0;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಸರಿಸು_type == ಅಭಿವ್ಯಕ್ತಿ_LSHIFT)
	tv1->vval.v_ಸಂಖ್ಯೆ =
	    (uvarnumber_T)tv1->vval.v_ಸಂಖ್ಯೆ << tv2->vval.v_ಸಂಖ್ಯೆ;
    ಇಲ್ಲದಿದ್ದರೆ
	tv1->vval.v_ಸಂಖ್ಯೆ =
	    (uvarnumber_T)tv1->vval.v_ಸಂಖ್ಯೆ >> tv2->vval.v_ಸಂಖ್ಯೆ;

    ಹಿಂತಿರುಗಿ OK;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
eval5(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    /*
     * Get the first expression.
     */
    ಆದರೆ (eval6(arg, rettv, evalarg) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    /*
     * Repeat computing, until no '<<' or '>>' is following.
     */
    ಗಾಗಿ (;;)
    {
	char_u		*p;
	ಪೂರ್ಣಾಂಕ		getnext;
	exprtype_T	exprtype;
	ಪೂರ್ಣಾಂಕ		evaluate;
	typval_T	var2;
	ಪೂರ್ಣಾಂಕ		vim9script;

	p = eval_ಮುಂದಿನ_non_blank(*arg, evalarg, &getnext);
	ಆದರೆ (p[0] == '<' && p[1] == '<')
	    exprtype = ಅಭಿವ್ಯಕ್ತಿ_LSHIFT;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p[0] == '>' && p[1] == '>')
	    exprtype = ಅಭಿವ್ಯಕ್ತಿ_RSHIFT;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಹಿಂತಿರುಗಿ OK;

	// Handle a bitwise left or right shift operator
	evaluate = evalarg == ಖಾಲಿ ? 0 : (evalarg->eval_flags & EVAL_EVALUATE);
	ಆದರೆ (evaluate && rettv->v_type != VAR_ಸಂಖ್ಯೆ)
	{
	    // left operand should be a number
	    emsg(_(e_bitshift_ops_must_be_ಸಂಖ್ಯೆ));
	    clear_tv(rettv);
	    ಹಿಂತಿರುಗಿ FAIL;
	}

	vim9script = ಒಳ_vim9script();
	ಆದರೆ (getnext)
	{
	    *arg = eval_ಮುಂದಿನ_ಸಾಲು(*arg, evalarg);
	    p = *arg;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (evaluate && vim9script && !VIM_ISWHITE(**arg))
	{
	    error_white_both(*arg, 2);
	    clear_tv(rettv);
	    ಹಿಂತಿರುಗಿ FAIL;
	}

	/*
	 * Get the second variable.
	 */
	ಆದರೆ (evaluate && vim9script && !IS_WHITE_OR_NUL(p[2]))
	{
	    error_white_both(p, 2);
	    clear_tv(rettv);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	*arg = skipwhite_and_linebreak(p + 2, evalarg);
	ಆದರೆ (eval6(arg, &var2, evalarg) == FAIL)
	{
	    clear_tv(rettv);
	    ಹಿಂತಿರುಗಿ FAIL;
	}

	ಆದರೆ (evaluate)
	{
	    ಆದರೆ (eval_ಸರಿಸು_ಸಂಖ್ಯೆ(rettv, &var2, exprtype) == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;
	}

	clear_tv(&var2);
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Concatenate strings "tv1" and "tv2" and store the result in "tv1".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
eval_concat_str(typval_T *tv1, typval_T *tv2)
{
    char_u	buf1[NUMBUFLEN], buf2[NUMBUFLEN];
    char_u	*s1 = tv_get_string_buf(tv1, buf1);
    char_u	*s2 = ಖಾಲಿ;
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		vim9script = ಒಳ_vim9script();

    ಆದರೆ (vim9script && (tv2->v_type == VAR_VOID
		|| tv2->v_type == VAR_CHANNEL
		|| tv2->v_type == VAR_ಕೆಲಸ))
	semsg(_(e_using_invalid_ಮೌಲ್ಯ_as_string_str),
		vartype_name(tv2->v_type));
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vim9script && tv2->v_type == VAR_FLOAT)
    {
	vim_snprintf((ಅಕ್ಷರ *)buf2, NUMBUFLEN, "%g",
		tv2->vval.v_float);
	s2 = buf2;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	s2 = tv_get_string_buf_chk(tv2, buf2);
    ಆದರೆ (s2 == ಖಾಲಿ)		// type error ?
    {
	clear_tv(tv1);
	clear_tv(tv2);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    // When possible, grow the existing string in place to avoid alloc/free.
    ಆದರೆ (grow_string_tv(tv1, s2) == OK)
	ಹಿಂತಿರುಗಿ OK;

    p = concat_str(s1, s2);
    clear_tv(tv1);
    tv1->v_type = VAR_STRING;
    tv1->vval.v_string = p;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Add or subtract numbers "tv1" and "tv2" and store the result in "tv1".
 * The numbers can be whole numbers or floats.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
eval_addsub_ಸಂಖ್ಯೆ(typval_T *tv1, typval_T *tv2, ಪೂರ್ಣಾಂಕ op)
{
    ಪೂರ್ಣಾಂಕ		error = FALSE;
    varnumber_T	n1, n2;
    float_T	f1 = 0, f2 = 0;

    ಆದರೆ (tv1->v_type == VAR_FLOAT)
    {
	f1 = tv1->vval.v_float;
	n1 = 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	n1 = tv_get_ಸಂಖ್ಯೆ_chk(tv1, &error);
	ಆದರೆ (error)
	{
	    // This can only happen for "list + non-list" or
	    // "blob + non-blob".  For "non-list + ..." or
	    // "something - ...", we returned before evaluating the
	    // 2nd operand.
	    clear_tv(tv1);
	    clear_tv(tv2);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	ಆದರೆ (tv2->v_type == VAR_FLOAT)
	    f1 = n1;
    }
    ಆದರೆ (tv2->v_type == VAR_FLOAT)
    {
	f2 = tv2->vval.v_float;
	n2 = 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	n2 = tv_get_ಸಂಖ್ಯೆ_chk(tv2, &error);
	ಆದರೆ (error)
	{
	    clear_tv(tv1);
	    clear_tv(tv2);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	ಆದರೆ (tv1->v_type == VAR_FLOAT)
	    f2 = n2;
    }
    clear_tv(tv1);

    // If there is a float on either side the result is a float.
    ಆದರೆ (tv1->v_type == VAR_FLOAT || tv2->v_type == VAR_FLOAT)
    {
	ಆದರೆ (op == '+')
	    f1 = f1 + f2;
	ಇಲ್ಲದಿದ್ದರೆ
	    f1 = f1 - f2;
	tv1->v_type = VAR_FLOAT;
	tv1->vval.v_float = f1;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (op == '+')
	    n1 = n1 + n2;
	ಇಲ್ಲದಿದ್ದರೆ
	    n1 = n1 - n2;
	tv1->v_type = VAR_ಸಂಖ್ಯೆ;
	tv1->vval.v_ಸಂಖ್ಯೆ = n1;
    }

    ಹಿಂತಿರುಗಿ OK;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
eval6(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    /*
     * Get the first expression.
     */
    ಆದರೆ (eval7(arg, rettv, evalarg, FALSE) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    /*
     * Repeat computing, until no '+', '-' or '.' is following.
     */
    ಗಾಗಿ (;;)
    {
	ಪೂರ್ಣಾಂಕ	    evaluate;
	ಪೂರ್ಣಾಂಕ	    getnext;
	char_u	    *p;
	ಪೂರ್ಣಾಂಕ	    op;
	ಪೂರ್ಣಾಂಕ	    oplen;
	ಪೂರ್ಣಾಂಕ	    concat;
	typval_T    var2;
	ಪೂರ್ಣಾಂಕ	    vim9script = ಒಳ_vim9script();
	ದೀರ್ಘ	    op_lnum = SOURCING_LNUM;

	// "." is only string concatenation when scriptversion is 1
	// "+=", "-=" and "..=" are assignments
	// "++" and "--" on the next line are a separate command.
	p = eval_ಮುಂದಿನ_non_blank(*arg, evalarg, &getnext);
	op = *p;
	concat = op == '.' && (*(p + 1) == '.' || ಒಳ_old_script(2));
	ಆದರೆ ((op != '+' && op != '-' && !concat) || p[1] == '='
					       || (p[1] == '.' && p[2] == '='))
	    ನಿಲ್ಲಿಸು;
	ಆದರೆ (getnext && (op == '+' || op == '-') && p[0] == p[1])
	    ನಿಲ್ಲಿಸು;

	evaluate = evalarg == ಖಾಲಿ ? 0 : (evalarg->eval_flags & EVAL_EVALUATE);
	oplen = (concat && p[1] == '.') ? 2 : 1;
	ಆದರೆ (getnext)
	    *arg = eval_ಮುಂದಿನ_ಸಾಲು(*arg, evalarg);
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (evaluate && vim9script && !VIM_ISWHITE(**arg))
	    {
		error_white_both(*arg, oplen);
		clear_tv(rettv);
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    *arg = p;
	}
	ಆದರೆ ((op != '+' || (rettv->v_type != VAR_LIST
						&& rettv->v_type != VAR_TUPLE
						&& rettv->v_type != VAR_BLOB))
		&& (op == '.' || rettv->v_type != VAR_FLOAT)
		&& evaluate)
	{
	    ಪೂರ್ಣಾಂಕ		error = FALSE;

	    // For "list + ...", an illegal use of the first operand as
	    // a number cannot be determined before evaluating the 2nd
	    // operand: if this is also a list, all is ok.
	    // For "something . ...", "something - ..." or "non-list + ...",
	    // we know that the first operand needs to be a string or number
	    // without evaluating the 2nd operand.  So check before to avoid
	    // side effects after an error.
	    ಆದರೆ (op != '.')
		tv_get_ಸಂಖ್ಯೆ_chk(rettv, &error);
	    ಆದರೆ ((op == '.' && tv_get_string_chk(rettv) == ಖಾಲಿ) || error)
	    {
		clear_tv(rettv);
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	}

	/*
	 * Get the second variable.
	 */
	ಆದರೆ (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[oplen]))
	{
	    error_white_both(*arg, oplen);
	    clear_tv(rettv);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	*arg = skipwhite_and_linebreak(*arg + oplen, evalarg);
	ಆದರೆ (eval7(arg, &var2, evalarg, !vim9script && op == '.') == FAIL)
	{
	    clear_tv(rettv);
	    ಹಿಂತಿರುಗಿ FAIL;
	}

	ಆದರೆ (evaluate)
	{
	    /*
	     * Compute the result.
	     */
	    // use the line of the operation for messages
	    SOURCING_LNUM = op_lnum;
	    ಆದರೆ (op == '.')
	    {
		ಆದರೆ (eval_concat_str(rettv, &var2) == FAIL)
		    ಹಿಂತಿರುಗಿ FAIL;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (op == '+' && rettv->v_type == VAR_BLOB
						   && var2.v_type == VAR_BLOB)
		eval_addblob(rettv, &var2);
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (op == '+' && rettv->v_type == VAR_LIST
						   && var2.v_type == VAR_LIST)
	    {
		ಆದರೆ (eval_addlist(rettv, &var2) == FAIL)
		    ಹಿಂತಿರುಗಿ FAIL;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (op == '+' && rettv->v_type == VAR_TUPLE
					   && var2.v_type == VAR_TUPLE)
	    {
		ಆದರೆ (eval_addtuple(rettv, &var2) == FAIL)
		    ಹಿಂತಿರುಗಿ FAIL;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ (eval_addsub_ಸಂಖ್ಯೆ(rettv, &var2, op) == FAIL)
		    ಹಿಂತಿರುಗಿ FAIL;
	    }
	    clear_tv(&var2);
	}
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Multiply or divide or compute the modulo of numbers "tv1" and "tv2" and
 * store the result in "tv1".  The numbers can be whole numbers or floats.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
eval_multdiv_ಸಂಖ್ಯೆ(typval_T *tv1, typval_T *tv2, ಪೂರ್ಣಾಂಕ op)
{
    varnumber_T	n1, n2;
    float_T	f1, f2;
    ಪೂರ್ಣಾಂಕ		error;
    ಪೂರ್ಣಾಂಕ		use_float = FALSE;

    f1 = 0;
    f2 = 0;
    error = FALSE;
    ಆದರೆ (tv1->v_type == VAR_FLOAT)
    {
	f1 = tv1->vval.v_float;
	use_float = TRUE;
	n1 = 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	n1 = tv_get_ಸಂಖ್ಯೆ_chk(tv1, &error);
    clear_tv(tv1);
    ಆದರೆ (error)
    {
	clear_tv(tv2);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಆದರೆ (tv2->v_type == VAR_FLOAT)
    {
	ಆದರೆ (!use_float)
	{
	    f1 = n1;
	    use_float = TRUE;
	}
	f2 = tv2->vval.v_float;
	n2 = 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	n2 = tv_get_ಸಂಖ್ಯೆ_chk(tv2, &error);
	clear_tv(tv2);
	ಆದರೆ (error)
	    ಹಿಂತಿರುಗಿ FAIL;
	ಆದರೆ (use_float)
	    f2 = n2;
    }

    /*
     * Compute the result.
     * When either side is a float the result is a float.
     */
    ಆದರೆ (use_float)
    {
	ಆದರೆ (op == '*')
	    f1 = f1 * f2;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (op == '/')
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VMS
	    // VMS crashes on divide by zero, work around it
	    ಆದರೆ (f2 == 0.0)
	    {
		ಆದರೆ (f1 == 0)
		    f1 = -1 * __F_FLT_MAX - 1L;   // similar to NaN
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (f1 < 0)
		    f1 = -1 * __F_FLT_MAX;
		ಇಲ್ಲದಿದ್ದರೆ
		    f1 = __F_FLT_MAX;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		f1 = f1 / f2;
#ಇಲ್ಲದಿದ್ದರೆ
	    // We rely on the floating point library to handle divide
	    // by zero to result in "inf" and not a crash.
	    f1 = f1 / f2;
#ಆದರೆ_ಅಂತ್ಯ
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    emsg(_(e_cannot_use_percent_with_float));
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	tv1->v_type = VAR_FLOAT;
	tv1->vval.v_float = f1;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಪೂರ್ಣಾಂಕ	    failed = FALSE;

	ಆದರೆ (op == '*')
	    n1 = n1 * n2;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (op == '/')
	    n1 = ಅಂಕೆ_divide(n1, n2, &failed);
	ಇಲ್ಲದಿದ್ದರೆ
	    n1 = ಅಂಕೆ_modulus(n1, n2, &failed);
	ಆದರೆ (failed)
	    ಹಿಂತಿರುಗಿ FAIL;

	tv1->v_type = VAR_ಸಂಖ್ಯೆ;
	tv1->vval.v_ಸಂಖ್ಯೆ = n1;
    }

    ಹಿಂತಿರುಗಿ OK;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
eval7(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    ಪೂರ್ಣಾಂಕ		want_string)  // after "." operator
{
    /*
     * Get the first expression.
     */
    ಆದರೆ (eval8(arg, rettv, evalarg, want_string) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    /*
     * Repeat computing, until no '*', '/' or '%' is following.
     */
    ಗಾಗಿ (;;)
    {
	ಪೂರ್ಣಾಂಕ	    evaluate;
	ಪೂರ್ಣಾಂಕ	    getnext;
	typval_T    var2;
	char_u	    *p;
	ಪೂರ್ಣಾಂಕ	    op;

	// "*=", "/=" and "%=" are assignments
	p = eval_ಮುಂದಿನ_non_blank(*arg, evalarg, &getnext);
	op = *p;
	ಆದರೆ ((op != '*' && op != '/' && op != '%') || p[1] == '=')
	    ನಿಲ್ಲಿಸು;

	evaluate = evalarg == ಖಾಲಿ ? 0 : (evalarg->eval_flags & EVAL_EVALUATE);
	ಆದರೆ (getnext)
	    *arg = eval_ಮುಂದಿನ_ಸಾಲು(*arg, evalarg);
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (evaluate && ಒಳ_vim9script() && !VIM_ISWHITE(**arg))
	    {
		error_white_both(*arg, 1);
		clear_tv(rettv);
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    *arg = p;
	}

	/*
	 * Get the second variable.
	 */
	ಆದರೆ (evaluate && ಒಳ_vim9script() && !IS_WHITE_OR_NUL((*arg)[1]))
	{
	    error_white_both(*arg, 1);
	    clear_tv(rettv);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	*arg = skipwhite_and_linebreak(*arg + 1, evalarg);
	ಆದರೆ (eval8(arg, &var2, evalarg, FALSE) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;

	ಆದರೆ (evaluate)
	    // Compute the result.
	    ಆದರೆ (eval_multdiv_ಸಂಖ್ಯೆ(rettv, &var2, op) == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Handle seventh level expression:
 *	a type cast before a base level expression.
 * "arg" must point to the first non-white of the expression.
 * "arg" is advanced to just after the recognized expression.
 * Return OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
eval8(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    ಪೂರ್ಣಾಂಕ		want_string)	// after "." operator
{
    type_T	*want_type = ಖಾಲಿ;
    garray_T	type_list;	    // list of pointers to allocated types
    ಪೂರ್ಣಾಂಕ		res;
    ಪೂರ್ಣಾಂಕ		evaluate = evalarg == ಖಾಲಿ ? 0
				       : (evalarg->eval_flags & EVAL_EVALUATE);

    // Recognize <type> in Vim9 script only.
    ಆದರೆ (ಒಳ_vim9script() && **arg == '<' && eval_isnamec1((*arg)[1])
					     && STRNCMP(*arg, "<SNR>", 5) != 0)
    {
	++*arg;
	ga_init2(&type_list, ಗಾತ್ರ(type_T *), 10);
	ಆದರೆ (!evaluate)
	    // Only skip over the type, it may use a type variable that is only
	    // known when the expression is compiled, e.g. in a lambda.
	    *arg = skip_type(*arg, FALSE);
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    want_type = parse_type(arg, &type_list, ಖಾಲಿ, ಖಾಲಿ, TRUE);
	    ಆದರೆ (want_type == ಖಾಲಿ)
	    {
		clear_type_list(&type_list);
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	}

	ಆದರೆ (**arg != '>')
	{
	    ಆದರೆ (*skipwhite(*arg) == '>')
		semsg(_(e_no_white_space_allowed_before_str_str), ">", *arg);
	    ಇಲ್ಲದಿದ್ದರೆ
		emsg(_(e_missing_gt));
	    clear_type_list(&type_list);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	++*arg;
	*arg = skipwhite_and_linebreak(*arg, evalarg);
    }

    res = eval9(arg, rettv, evalarg, want_string);

    ಆದರೆ (want_type != ಖಾಲಿ)
    {
	ಆದರೆ (evaluate && res == OK)
	{
	    type_T *actual = typval2type(rettv, get_copyID(), &type_list,
							       TVTT_DO_MEMBER);

	    ಆದರೆ (!equal_type(want_type, actual, 0))
	    {
		ಆದರೆ (want_type->tt_type == VAR_BOOL
					&& actual->tt_type != VAR_BOOL
					&& (actual->tt_flags & TTFLAG_BOOL_OK))
		{
		    ಪೂರ್ಣಾಂಕ n = tv2bool(rettv);

		    // can use "0" and "1" for boolean in some places
		    clear_tv(rettv);
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_ಸಂಖ್ಯೆ = n ? VVAL_TRUE : VVAL_FALSE;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    where_T where = WHERE_INIT;

		    res = check_type(want_type, actual, TRUE, where);
		}
	    }
	}
	clear_type_list(&type_list);
    }

    ಹಿಂತಿರುಗಿ res;
}

    ಪೂರ್ಣಾಂಕ
eval_leader(char_u **arg, ಪೂರ್ಣಾಂಕ vim9)
{
    char_u	*s = *arg;
    char_u	*p = *arg;

    ಆಗ (*p == '!' || *p == '-' || *p == '+')
    {
	char_u *n = skipwhite(p + 1);

	// ++, --, -+ and +- are not accepted in Vim9 script
	ಆದರೆ (vim9 && (*p == '-' || *p == '+') && (*n == '-' || *n == '+'))
	{
	    semsg(_(e_invalid_ಅಭಿವ್ಯಕ್ತಿ_str), s);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	p = n;
    }
    *arg = p;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Check for a predefined value "true", "false" and "null.*".
 * Return OK when recognized.
 */
    ಪೂರ್ಣಾಂಕ
handle_predefined(char_u *s, ಪೂರ್ಣಾಂಕ len, typval_T *rettv)
{
    ಆಯ್ಕೆ (len)
    {
	ಸ್ಥಿತಿ 4: ಆದರೆ (STRNCMP(s, "true", 4) == 0)
		{
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_ಸಂಖ್ಯೆ = VVAL_TRUE;
		    ಹಿಂತಿರುಗಿ OK;
		}
		ಆದರೆ (STRNCMP(s, "null", 4) == 0)
		{
		    rettv->v_type = VAR_SPECIAL;
		    rettv->vval.v_ಸಂಖ್ಯೆ = VVAL_NULL;
		    ಹಿಂತಿರುಗಿ OK;
		}
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 5: ಆದರೆ (STRNCMP(s, "false", 5) == 0)
		{
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_ಸಂಖ್ಯೆ = VVAL_FALSE;
		    ಹಿಂತಿರುಗಿ OK;
		}
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 8: ಆದರೆ (STRNCMP(s, "null_job", 8) == 0)
		{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
		    rettv->v_type = VAR_ಕೆಲಸ;
		    rettv->vval.v_ಕೆಲಸ = ಖಾಲಿ;
#ಇಲ್ಲದಿದ್ದರೆ
		    rettv->v_type = VAR_SPECIAL;
		    rettv->vval.v_ಸಂಖ್ಯೆ = VVAL_NULL;
#ಆದರೆ_ಅಂತ್ಯ
		    ಹಿಂತಿರುಗಿ OK;
		}
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 9:
		ಆದರೆ (STRNCMP(s, "null_", 5) != 0)
		    ನಿಲ್ಲಿಸು;
		// null_list
		ಆದರೆ (STRNCMP(s + 5, "list", 4) == 0)
		{
		    rettv->v_type = VAR_LIST;
		    rettv->vval.v_list = ಖಾಲಿ;
		    ಹಿಂತಿರುಗಿ OK;
		}
		// null_dict
		ಆದರೆ (STRNCMP(s + 5, "dict", 4) == 0)
		{
		    rettv->v_type = VAR_DICT;
		    rettv->vval.v_dict = ಖಾಲಿ;
		    ಹಿಂತಿರುಗಿ OK;
		}
		// null_blob
		ಆದರೆ (STRNCMP(s + 5, "blob", 4) == 0)
		{
		    rettv->v_type = VAR_BLOB;
		    rettv->vval.v_blob = ಖಾಲಿ;
		    ಹಿಂತಿರುಗಿ OK;
		}
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 10:
		ಆದರೆ (STRNCMP(s, "null_", 5) != 0)
		    ನಿಲ್ಲಿಸು;
		// null_class
		ಆದರೆ (STRNCMP(s + 5, "class", 5) == 0)
		{
		    rettv->v_type = VAR_CLASS;
		    rettv->vval.v_class = ಖಾಲಿ;
		    ಹಿಂತಿರುಗಿ OK;
		}
		ಆದರೆ (STRNCMP(s + 5, "tuple", 5) == 0)
		{
		    rettv->v_type = VAR_TUPLE;
		    rettv->vval.v_tuple = ಖಾಲಿ;
		    ಹಿಂತಿರುಗಿ OK;
		}
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 11: ಆದರೆ (STRNCMP(s, "null_string", 11) == 0)
		{
		    rettv->v_type = VAR_STRING;
		    rettv->vval.v_string = ಖಾಲಿ;
		    ಹಿಂತಿರುಗಿ OK;
		}
		ಆದರೆ (STRNCMP(s, "null_object", 11) == 0)
		{
		    rettv->v_type = VAR_OBJECT;
		    rettv->vval.v_object = ಖಾಲಿ;
		    ಹಿಂತಿರುಗಿ OK;
		}
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 12:
		ಆದರೆ (STRNCMP(s, "null_channel", 12) == 0)
		{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
		    rettv->v_type = VAR_CHANNEL;
		    rettv->vval.v_channel = ಖಾಲಿ;
#ಇಲ್ಲದಿದ್ದರೆ
		    rettv->v_type = VAR_SPECIAL;
		    rettv->vval.v_ಸಂಖ್ಯೆ = VVAL_NULL;
#ಆದರೆ_ಅಂತ್ಯ
		    ಹಿಂತಿರುಗಿ OK;
		}
		ಆದರೆ (STRNCMP(s, "null_partial", 12) == 0)
		{
		    rettv->v_type = VAR_PARTIAL;
		    rettv->vval.v_partial = ಖಾಲಿ;
		    ಹಿಂತಿರುಗಿ OK;
		}
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 13: ಆದರೆ (STRNCMP(s, "null_function", 13) == 0)
		{
		    rettv->v_type = VAR_FUNC;
		    rettv->vval.v_string = ಖಾಲಿ;
		    ಹಿಂತಿರುಗಿ OK;
		}
		ನಿಲ್ಲಿಸು;
    }
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Handle register contents: @r.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
eval9_reg_contents(
    char_u	**arg,
    typval_T	*rettv,
    ಪೂರ್ಣಾಂಕ		evaluate)
{
    ಪೂರ್ಣಾಂಕ		vim9script = ಒಳ_vim9script();

    ++*arg;	// skip '@'
    ಆದರೆ (evaluate)
    {
	ಆದರೆ (vim9script && IS_WHITE_OR_NUL(**arg))
	    semsg(_(e_syntax_error_at_str), *arg);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vim9script && !valid_yank_reg(**arg, FALSE))
	    emsg_invreg(**arg);
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    rettv->v_type = VAR_STRING;
	    rettv->vval.v_string = get_reg_contents(**arg,
		    GREG_ಅಭಿವ್ಯಕ್ತಿ_SRC);
	}
    }
    ಆದರೆ (**arg != NUL)
	++*arg;
}

/*
 * Handle a nested expression: (expression) or lambda: (arg) => expr
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
eval9_nested_ಅಭಿವ್ಯಕ್ತಿ(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    ಪೂರ್ಣಾಂಕ		evaluate)
{
    ಪೂರ್ಣಾಂಕ		ret = NOTDONE;
    ಪೂರ್ಣಾಂಕ		vim9script = ಒಳ_vim9script();

    ಆದರೆ (vim9script)
    {
	ret = get_lambda_tv(arg, rettv, TRUE, evalarg, ಖಾಲಿ);
	ಆದರೆ (ret == OK && evaluate)
	{
	    ufunc_T *ufunc = rettv->vval.v_partial->pt_func;

	    // Compile it here to get the return type.  The return
	    // type is optional, when it's missing use t_unknown.
	    // This is recognized in compile_return().
	    ಆದರೆ (ufunc->uf_ret_type->tt_type == VAR_VOID)
		ufunc->uf_ret_type = &t_unknown;
	    ಆದರೆ (compile_def_function(ufunc, FALSE,
				get_compile_type(ufunc), ಖಾಲಿ) == FAIL)
	    {
		clear_tv(rettv);
		ret = FAIL;
	    }
	}
    }
    ಆದರೆ (ret == NOTDONE)
    {
	*arg = skipwhite_and_linebreak(*arg + 1, evalarg);
	ಆದರೆ (**arg == ')')
	    // empty tuple
	    ret = eval_tuple(arg, rettv, evalarg, TRUE);
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ret = eval1(arg, rettv, evalarg);	// recursive!
	    ಆದರೆ (ret != OK)
		ಹಿಂತಿರುಗಿ ret;

	    *arg = skipwhite_and_linebreak(*arg, evalarg);

	    ಆದರೆ (**arg == ',')
		// tuple
		ret = eval_tuple(arg, rettv, evalarg, TRUE);
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (**arg == ')')
		++*arg;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ret == OK)
	    {
		emsg(_(e_missing_closing_paren));
		clear_tv(rettv);
		ret = FAIL;
	    }
	}
    }

    ಹಿಂತಿರುಗಿ ret;
}

/*
* Handle be a variable or function name.
* Can also be a curly-braces kind of name: {expr}.
*/
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
eval9_var_func_name(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    ಪೂರ್ಣಾಂಕ		evaluate,
    char_u	**name_ಆರಂಭ)
{
    char_u	*s;
    ಪೂರ್ಣಾಂಕ		len;
    char_u	*alias;
    ಪೂರ್ಣಾಂಕ		ret = OK;
    ಪೂರ್ಣಾಂಕ		vim9script = ಒಳ_vim9script();

    s = *arg;
    len = get_name_len(arg, &alias, evaluate, TRUE);
    ಆದರೆ (alias != ಖಾಲಿ)
	s = alias;

    ಆದರೆ (len <= 0)
	ret = FAIL;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಪೂರ್ಣಾಂಕ	flags = evalarg == ಖಾಲಿ ? 0 : evalarg->eval_flags;

	ಆದರೆ (evaluate && vim9script && len == 1 && *s == '_')
	{
	    emsg(_(e_cannot_use_underscore_ಇಲ್ಲಿ));
	    ret = FAIL;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (evaluate && vim9script && len > 2
						&& s[0] == 's' && s[1] == ':')
	{
	    semsg(_(e_cannot_use_s_colon_ಒಳ_vim9_script_str), s);
	    ret = FAIL;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((vim9script ? **arg : *skipwhite(*arg)) == '('
				|| (vim9script && generic_func_call(arg)))
	{
	    // "name(..."  recursive!
	    *arg = skipwhite(*arg);
	    ret = eval_func(arg, evalarg, s, len, rettv, flags, ಖಾಲಿ);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (evaluate)
	{
	    // get the value of "true", "false", etc. or a variable
	    ret = FAIL;
	    ಆದರೆ (vim9script)
		ret = handle_predefined(s, len, rettv);
	    ಆದರೆ (ret == FAIL)
	    {
		*name_ಆರಂಭ = s;
		ret = eval_variable(s, len, 0, rettv, ಖಾಲಿ,
					EVAL_VAR_VERBOSE + EVAL_VAR_IMPORT);

		// skip the generic function arguments (if present)
		// they are already processed by eval_variable
		ಆದರೆ (ret == OK && vim9script && **arg == '<'
						&& rettv->v_type == VAR_FUNC)
		    ret = skip_generic_func_type_ವಾದಗಳು(arg);
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // skip the name
	    check_vars(s, len);

	    // Skip the type arguments of a generic function, its name starts
	    // with an uppercase letter.
	    ಆದರೆ (vim9script && **arg == '<' && ASCII_ISUPPER(*s))
		skip_generic_func_type_ವಾದಗಳು(arg);
	    ret = OK;
	}
    }
    vim_free(alias);

    ಹಿಂತಿರುಗಿ ret;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
eval9(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    ಪೂರ್ಣಾಂಕ		want_string)	// after "." operator
{
    ಪೂರ್ಣಾಂಕ		evaluate = evalarg != ಖಾಲಿ
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    char_u	*name_ಆರಂಭ = ಖಾಲಿ;
    char_u	*ಆರಂಭ_leader, *ಕೊನೆ_leader;
    ಪೂರ್ಣಾಂಕ		ret = OK;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	recurse = 0;
    ಪೂರ್ಣಾಂಕ		vim9script = ಒಳ_vim9script();

    /*
     * Initialise variable so that clear_tv() can't mistake this for a
     * string and free a string that isn't there.
     */
    rettv->v_type = VAR_UNKNOWN;

    /*
     * Skip '!', '-' and '+' characters.  They are handled later.
     */
    ಆರಂಭ_leader = *arg;
    ಆದರೆ (eval_leader(arg, vim9script) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;
    ಕೊನೆ_leader = *arg;

    ಆದರೆ (**arg == '.' && (!ಸುರಕ್ಷಿತ_isdigit(*(*arg + 1)) || ಒಳ_old_script(2)))
    {
	semsg(_(e_invalid_ಅಭಿವ್ಯಕ್ತಿ_str), *arg);
	++*arg;
	ಹಿಂತಿರುಗಿ FAIL;
    }

    // Limit recursion to 1000 levels.  At least at 10000 we run out of stack
    // and crash.  With MSVC the stack is smaller.
    ಆದರೆ (recurse ==
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _MSC_VER
		    300
#ಇಲ್ಲದಿದ್ದರೆ
		    1000
#ಆದರೆ_ಅಂತ್ಯ
		    )
    {
	semsg(_(e_ಅಭಿವ್ಯಕ್ತಿ_too_recursive_str), *arg);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ++recurse;

    ಆಯ್ಕೆ (**arg)
    {
    /*
     * Number constant.
     */
    ಸ್ಥಿತಿ '0':
    ಸ್ಥಿತಿ '1':
    ಸ್ಥಿತಿ '2':
    ಸ್ಥಿತಿ '3':
    ಸ್ಥಿತಿ '4':
    ಸ್ಥಿತಿ '5':
    ಸ್ಥಿತಿ '6':
    ಸ್ಥಿತಿ '7':
    ಸ್ಥಿತಿ '8':
    ಸ್ಥಿತಿ '9':
    ಸ್ಥಿತಿ '.':	ret = eval_ಸಂಖ್ಯೆ(arg, rettv, evaluate, want_string);

		// Apply prefixed "-" and "+" now.  Matters especially when
		// "->" follows.
		ಆದರೆ (ret == OK && evaluate && ಕೊನೆ_leader > ಆರಂಭ_leader
						  && rettv->v_type != VAR_BLOB)
		    ret = eval9_leader(rettv, TRUE, ಆರಂಭ_leader, &ಕೊನೆ_leader);
		ನಿಲ್ಲಿಸು;

    /*
     * String constant: "string".
     */
    ಸ್ಥಿತಿ '"':	ret = eval_string(arg, rettv, evaluate, FALSE);
		ನಿಲ್ಲಿಸು;

    /*
     * Literal string constant: 'str''ing'.
     */
    ಸ್ಥಿತಿ '\'':	ret = eval_lit_string(arg, rettv, evaluate, FALSE);
		ನಿಲ್ಲಿಸು;

    /*
     * List: [expr, expr]
     */
    ಸ್ಥಿತಿ '[':	ret = eval_list(arg, rettv, evalarg, TRUE);
		ನಿಲ್ಲಿಸು;

    /*
     * Literal Dictionary: #{key: val, key: val}
     */
    ಸ್ಥಿತಿ '#':	ret = eval_lit_dict(arg, rettv, evalarg);
		ನಿಲ್ಲಿಸು;

    /*
     * Lambda: {arg, arg -> expr}
     * Dictionary: {'key': val, 'key': val}
     */
    ಸ್ಥಿತಿ '{':	ಆದರೆ (vim9script)
		    ret = NOTDONE;
		ಇಲ್ಲದಿದ್ದರೆ
		    ret = get_lambda_tv(arg, rettv, vim9script, evalarg, ಖಾಲಿ);
		ಆದರೆ (ret == NOTDONE)
		    ret = eval_dict(arg, rettv, evalarg, FALSE);
		ನಿಲ್ಲಿಸು;

    /*
     * Option value: &name
     */
    ಸ್ಥಿತಿ '&':	ret = eval_option(arg, rettv, evaluate);
		ನಿಲ್ಲಿಸು;

    /*
     * Environment variable: $VAR.
     * Interpolated string: $"string" or $'string'.
     */
    ಸ್ಥಿತಿ '$':	ಆದರೆ ((*arg)[1] == '"' || (*arg)[1] == '\'')
		    ret = eval_interp_string(arg, rettv, evaluate);
		ಇಲ್ಲದಿದ್ದರೆ
		    ret = eval_env_var(arg, rettv, evaluate);
		ನಿಲ್ಲಿಸು;

    /*
     * Register contents: @r.
     */
    ಸ್ಥಿತಿ '@':	eval9_reg_contents(arg, rettv, evaluate);
		ನಿಲ್ಲಿಸು;

    /*
     * nested expression: (expression).
     * or lambda: (arg) => expr
     * or tuple
     */
    ಸ್ಥಿತಿ '(':	ret = eval9_nested_ಅಭಿವ್ಯಕ್ತಿ(arg, rettv, evalarg, evaluate);
		ನಿಲ್ಲಿಸು;

    ಪೂರ್ವನಿಯೋಜಿತ:	ret = NOTDONE;
		ನಿಲ್ಲಿಸು;
    }

    ಆದರೆ (ret == NOTDONE)
    {
	/*
	 * Must be a variable or function name.
	 * Can also be a curly-braces kind of name: {expr}.
	 */
	ret = eval9_var_func_name(arg, rettv, evalarg, evaluate, &name_ಆರಂಭ);
    }

    // Handle following '[', '(' and '.' for expr[expr], expr.name,
    // expr(expr), expr->name(expr)
    ಆದರೆ (ret == OK)
	ret = handle_subscript(arg, name_ಆರಂಭ, rettv, evalarg, evaluate);

    /*
     * Apply logical NOT and unary '-', from right to left, ignore '+'.
     */
    ಆದರೆ (ret == OK && evaluate && ಕೊನೆ_leader > ಆರಂಭ_leader)
	ret = eval9_leader(rettv, FALSE, ಆರಂಭ_leader, &ಕೊನೆ_leader);

    --recurse;
    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Apply the leading "!" and "-" before an eval9 expression to "rettv".
 * When "numeric_only" is TRUE only handle "+" and "-".
 * Adjusts "end_leaderp" until it is at "start_leader".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
eval9_leader(
	typval_T    *rettv,
	ಪೂರ್ಣಾಂಕ	    numeric_only,
	char_u	    *ಆರಂಭ_leader,
	char_u	    **ಕೊನೆ_leaderp)
{
    char_u	*ಕೊನೆ_leader = *ಕೊನೆ_leaderp;
    ಪೂರ್ಣಾಂಕ		ret = OK;
    ಪೂರ್ಣಾಂಕ		error = FALSE;
    varnumber_T val = 0;
    vartype_T	type = rettv->v_type;
    ಪೂರ್ಣಾಂಕ		vim9script = ಒಳ_vim9script();
    float_T	    f = 0.0;

    ಆದರೆ (rettv->v_type == VAR_FLOAT)
	f = rettv->vval.v_float;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆಗ (VIM_ISWHITE(ಕೊನೆ_leader[-1]))
	    --ಕೊನೆ_leader;
	ಆದರೆ (vim9script && ಕೊನೆ_leader[-1] == '!')
	{
	    // Is this typeval supported with the ! operator?
	    ಆದರೆ (check_typval_is_ಮೌಲ್ಯ(rettv) == FAIL)
	    {
		clear_tv(rettv);
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    val = tv2bool(rettv);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    val = tv_get_ಸಂಖ್ಯೆ_chk(rettv, &error);
    }
    ಆದರೆ (error)
    {
	clear_tv(rettv);
	ret = FAIL;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆಗ (ಕೊನೆ_leader > ಆರಂಭ_leader)
	{
	    --ಕೊನೆ_leader;
	    ಆದರೆ (*ಕೊನೆ_leader == '!')
	    {
		ಆದರೆ (numeric_only)
		{
		    ++ಕೊನೆ_leader;
		    ನಿಲ್ಲಿಸು;
		}
		ಆದರೆ (rettv->v_type == VAR_FLOAT)
		{
		    rettv->v_type = VAR_BOOL;
		    val = f == 0.0 ? VVAL_TRUE : VVAL_FALSE;
		    type = VAR_BOOL;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    val = !val;
		    type = VAR_BOOL;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*ಕೊನೆ_leader == '-')
	    {
		ಆದರೆ (rettv->v_type == VAR_FLOAT)
		    f = -f;
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    val = -val;
		    type = VAR_ಸಂಖ್ಯೆ;
		}
	    }
	}
	ಆದರೆ (rettv->v_type == VAR_FLOAT)
	{
	    clear_tv(rettv);
	    rettv->vval.v_float = f;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    clear_tv(rettv);
	    ಆದರೆ (vim9script)
		rettv->v_type = type;
	    ಇಲ್ಲದಿದ್ದರೆ
		rettv->v_type = VAR_ಸಂಖ್ಯೆ;
	    rettv->vval.v_ಸಂಖ್ಯೆ = val;
	}
    }
    *ಕೊನೆ_leaderp = ಕೊನೆ_leader;
    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Call the function referred to in "rettv".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
call_func_rettv(
	char_u	    **arg,
	evalarg_T   *evalarg,
	typval_T    *rettv,
	ಪೂರ್ಣಾಂಕ	    evaluate,
	dict_T	    *selfdict,
	typval_T    *basetv)
{
    partial_T	*pt = ಖಾಲಿ;
    funcexe_T	funcexe;
    typval_T	functv;
    char_u	*s;
    ಪೂರ್ಣಾಂಕ		ret;

    // need to copy the funcref so that we can clear rettv
    ಆದರೆ (evaluate)
    {
	functv = *rettv;
	rettv->v_type = VAR_UNKNOWN;

	// Invoke the function.  Recursive!
	ಆದರೆ (functv.v_type == VAR_PARTIAL)
	{
	    pt = functv.vval.v_partial;
	    s = partial_name(pt);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    s = functv.vval.v_string;
	    ಆದರೆ (s == ಖಾಲಿ || *s == NUL)
	    {
		emsg(_(e_ಖಾಲಿಗಳು_function_name));
		ret = FAIL;
		ಹೋಗು theend;
	    }
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
	s = (char_u *)"";

    CLEAR_ಕ್ಷೇತ್ರ(funcexe);
    funcexe.fe_firstline = curwin->w_cursor.lnum;
    funcexe.fe_lastline = curwin->w_cursor.lnum;
    funcexe.fe_evaluate = evaluate;
    funcexe.fe_partial = pt;
    ಆದರೆ (pt != ಖಾಲಿ)
    {
	funcexe.fe_object = pt->pt_obj;
	ಆದರೆ (funcexe.fe_object != ಖಾಲಿ)
	    ++funcexe.fe_object->obj_refcount;
    }
    funcexe.fe_selfdict = selfdict;
    funcexe.fe_basetv = basetv;
    ಆದರೆ (evalarg != ಖಾಲಿ)
	funcexe.fe_cctx = evalarg->eval_cctx;
    ret = get_func_tv(s, -1, rettv, arg, evalarg, &funcexe);

theend:
    // Clear the funcref afterwards, so that deleting it while
    // evaluating the arguments is possible (see test55).
    ಆದರೆ (evaluate)
	clear_tv(&functv);

    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Evaluate "->method()".
 * "*arg" points to "method".
 * Returns FAIL or OK. "*arg" is advanced to after the ')'.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
eval_lambda(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    ಪೂರ್ಣಾಂಕ		verbose)	// give error messages
{
    ಪೂರ್ಣಾಂಕ		evaluate = evalarg != ಖಾಲಿ
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    typval_T	ತಳ = *rettv;
    ಪೂರ್ಣಾಂಕ		ret;

    rettv->v_type = VAR_UNKNOWN;

    ಆದರೆ (**arg == '{')
    {
	// ->{lambda}()
	ret = get_lambda_tv(arg, rettv, FALSE, evalarg, ಖಾಲಿ);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// ->(lambda)()
	++*arg;
	ret = eval1(arg, rettv, evalarg);
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	ಆದರೆ (**arg != ')')
	{
	    emsg(_(e_missing_closing_paren));
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	ಆದರೆ (rettv->v_type != VAR_STRING && rettv->v_type != VAR_FUNC
					       && rettv->v_type != VAR_PARTIAL)
	{
	    emsg(_(e_string_or_function_required_for_arrow_parens_ಅಭಿವ್ಯಕ್ತಿ));
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	++*arg;
    }
    ಆದರೆ (ret != OK)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (**arg != '(')
    {
	ಆದರೆ (verbose)
	{
	    ಆದರೆ (*skipwhite(*arg) == '(')
		emsg(_(e_no_white_space_allowed_before_parenthesis));
	    ಇಲ್ಲದಿದ್ದರೆ
		semsg(_(e_missing_parenthesis_str), "lambda");
	}
	clear_tv(rettv);
	ret = FAIL;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ret = call_func_rettv(arg, evalarg, rettv, evaluate, ಖಾಲಿ, &ತಳ);

    // Clear the funcref afterwards, so that deleting it while
    // evaluating the arguments is possible (see test55).
    ಆದರೆ (evaluate)
	clear_tv(&ತಳ);

    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Evaluate "->method()".
 * "*arg" points to "method".
 * Returns FAIL or OK. "*arg" is advanced to after the ')'.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
eval_ವಿಧಾನ(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    ಪೂರ್ಣಾಂಕ		verbose)	// give error messages
{
    char_u	*name;
    ದೀರ್ಘ	len;
    char_u	*alias;
    char_u	*tofree = ಖಾಲಿ;
    typval_T	ತಳ = *rettv;
    ಪೂರ್ಣಾಂಕ		ret = OK;
    ಪೂರ್ಣಾಂಕ		evaluate = evalarg != ಖಾಲಿ
				      && (evalarg->eval_flags & EVAL_EVALUATE);

    rettv->v_type = VAR_UNKNOWN;

    name = *arg;
    len = get_name_len(arg, &alias, evaluate, evaluate);
    ಆದರೆ (alias != ಖಾಲಿ)
	name = alias;

    ಆದರೆ (len <= 0)
    {
	ಆದರೆ (verbose)
	    emsg(_(e_missing_name_after_ವಿಧಾನ));
	ret = FAIL;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	char_u *paren;

	// If there is no "(" immediately following, but there is further on,
	// it can be "import.Func()", "dict.Func()", "list[nr]", etc.
	// Does not handle anything where "(" is part of the expression.
	*arg = skipwhite(*arg);

	ಆದರೆ (**arg != '(' && alias == ಖಾಲಿ
				    && (paren = vim_strchr(*arg, '(')) != ಖಾಲಿ)
	{
	    *arg = name;

	    // Truncate the name at the "(".  Avoid trying to get another line
	    // by making "getline" NULL.
	    *paren = NUL;
	    char_u	*(*getline)(ಪೂರ್ಣಾಂಕ, ಶೂನ್ಯ *, ಪೂರ್ಣಾಂಕ, getline_opt_T) = ಖಾಲಿ;
	    ಆದರೆ (evalarg != ಖಾಲಿ)
	    {
		getline = evalarg->eval_getline;
		evalarg->eval_getline = ಖಾಲಿ;
	    }

	    char_u *deref = deref_function_name(arg, &tofree, evalarg, verbose);
	    ಆದರೆ (deref == ಖಾಲಿ)
	    {
		*arg = name + len;
		ret = FAIL;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		name = deref;
		len = (ದೀರ್ಘ)STRLEN(name);
	    }

	    *paren = '(';
	    ಆದರೆ (getline != ಖಾಲಿ)
		evalarg->eval_getline = getline;
	}

	ಆದರೆ (ret == OK)
	{
	    *arg = skipwhite(*arg);

	    ಆದರೆ (**arg != '(')
	    {
		ಆದರೆ (verbose)
		    semsg(_(e_missing_parenthesis_str), name);
		ret = FAIL;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (VIM_ISWHITE((*arg)[-1]))
	    {
		ಆದರೆ (verbose)
		    emsg(_(e_no_white_space_allowed_before_parenthesis));
		ret = FAIL;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		ret = eval_func(arg, evalarg, name, len, rettv,
					  evaluate ? EVAL_EVALUATE : 0, &ತಳ);
	}
    }

    // Clear the funcref afterwards, so that deleting it while
    // evaluating the arguments is possible (see test55).
    ಆದರೆ (evaluate)
	clear_tv(&ತಳ);
    vim_free(tofree);

    ಆದರೆ (alias != ಖಾಲಿ)
	vim_free(alias);

    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Evaluate an "[expr]" or "[expr:expr]" index.  Also "dict.key".
 * "*arg" points to the '[' or '.'.
 * Returns FAIL or OK. "*arg" is advanced to after the ']'.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
eval_ಸೂಚ್ಯಂಕ(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    ಪೂರ್ಣಾಂಕ		verbose)	// give error messages
{
    ಪೂರ್ಣಾಂಕ		evaluate = evalarg != ಖಾಲಿ
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    ಪೂರ್ಣಾಂಕ		empty1 = FALSE, empty2 = FALSE;
    typval_T	var1, var2;
    ಪೂರ್ಣಾಂಕ		range = FALSE;
    char_u	*ಕೀಲಿ = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		keylen = -1;
    ಪೂರ್ಣಾಂಕ		vim9script = ಒಳ_vim9script();

    ಆದರೆ (check_can_ಸೂಚ್ಯಂಕ(rettv, evaluate, verbose) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    init_tv(&var1);
    init_tv(&var2);
    ಆದರೆ (**arg == '.')
    {
	/*
	 * dict.name
	 */
	ಕೀಲಿ = *arg + 1;
	ಗಾಗಿ (keylen = 0; eval_isdictc(ಕೀಲಿ[keylen]); ++keylen)
	    ;
	ಆದರೆ (keylen == 0)
	    ಹಿಂತಿರುಗಿ FAIL;
	ಆದರೆ (vim9script && ಕೀಲಿ[keylen] == '<')
	{
	    // skip generic type arguments
	    char_u	*p = &ಕೀಲಿ[keylen];

	    ಆದರೆ (skip_generic_func_type_ವಾದಗಳು(&p) == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;
	    keylen = p - ಕೀಲಿ;
	}
	*arg = ಕೀಲಿ + keylen;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	/*
	 * something[idx]
	 *
	 * Get the (first) variable from inside the [].
	 */
	*arg = skipwhite_and_linebreak(*arg + 1, evalarg);
	ಆದರೆ (**arg == ':')
	    empty1 = TRUE;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (eval1(arg, &var1, evalarg) == FAIL)	// recursive!
	    ಹಿಂತಿರುಗಿ FAIL;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vim9script && **arg == ':')
	{
	    semsg(_(e_white_space_required_before_and_after_str_at_str),
								    ":", *arg);
	    clear_tv(&var1);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (evaluate)
	{
	    ಪೂರ್ಣಾಂಕ error = FALSE;

	    // allow for indexing with float
	    ಆದರೆ (vim9script && rettv->v_type == VAR_DICT
						   && var1.v_type == VAR_FLOAT)
	    {
		var1.vval.v_string = typval_tostring(&var1, TRUE);
		var1.v_type = VAR_STRING;
	    }

	    ಆದರೆ (vim9script && (rettv->v_type == VAR_LIST
						|| rettv->v_type == VAR_TUPLE))
		tv_get_ಸಂಖ್ಯೆ_chk(&var1, &error);
	    ಇಲ್ಲದಿದ್ದರೆ
		error = tv_get_string_chk(&var1) == ಖಾಲಿ;
	    ಆದರೆ (error)
	    {
		// not a number or string
		clear_tv(&var1);
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	}

	/*
	 * Get the second variable from inside the [:].
	 */
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	ಆದರೆ (**arg == ':')
	{
	    range = TRUE;
	    ++*arg;
	    ಆದರೆ (vim9script && !IS_WHITE_OR_NUL(**arg) && **arg != ']')
	    {
		semsg(_(e_white_space_required_before_and_after_str_at_str),
								":", *arg - 1);
		ಆದರೆ (!empty1)
		    clear_tv(&var1);
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    *arg = skipwhite_and_linebreak(*arg, evalarg);
	    ಆದರೆ (**arg == ']')
		empty2 = TRUE;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (eval1(arg, &var2, evalarg) == FAIL)	// recursive!
	    {
		ಆದರೆ (!empty1)
		    clear_tv(&var1);
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (evaluate && tv_get_string_chk(&var2) == ಖಾಲಿ)
	    {
		// not a number or string
		ಆದರೆ (!empty1)
		    clear_tv(&var1);
		clear_tv(&var2);
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	}

	// Check for the ']'.
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	ಆದರೆ (**arg != ']')
	{
	    ಆದರೆ (verbose)
		emsg(_(e_missing_closing_ವರ್ಗಫಲ_brace));
	    clear_tv(&var1);
	    ಆದರೆ (range)
		clear_tv(&var2);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	*arg = *arg + 1;	// skip over the ']'
    }

    ಆದರೆ (evaluate)
    {
	ಪೂರ್ಣಾಂಕ res = eval_ಸೂಚ್ಯಂಕ_inner(rettv, range,
		empty1 ? ಖಾಲಿ : &var1, empty2 ? ಖಾಲಿ : &var2, FALSE,
		ಕೀಲಿ, keylen, verbose);

	ಆದರೆ (!empty1)
	    clear_tv(&var1);
	ಆದರೆ (range)
	    clear_tv(&var2);
	ಹಿಂತಿರುಗಿ res;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Check if "rettv" can have an [index] or [sli:ce]
 */
    ಪೂರ್ಣಾಂಕ
check_can_ಸೂಚ್ಯಂಕ(typval_T *rettv, ಪೂರ್ಣಾಂಕ evaluate, ಪೂರ್ಣಾಂಕ verbose)
{
    ಆಯ್ಕೆ (rettv->v_type)
    {
	ಸ್ಥಿತಿ VAR_FUNC:
	ಸ್ಥಿತಿ VAR_PARTIAL:
	    ಆದರೆ (verbose)
		emsg(_(e_cannot_ಸೂಚ್ಯಂಕ_a_funcref));
	    ಹಿಂತಿರುಗಿ FAIL;
	ಸ್ಥಿತಿ VAR_FLOAT:
	    ಆದರೆ (verbose)
		emsg(_(e_using_float_as_string));
	    ಹಿಂತಿರುಗಿ FAIL;
	ಸ್ಥಿತಿ VAR_BOOL:
	ಸ್ಥಿತಿ VAR_SPECIAL:
	ಸ್ಥಿತಿ VAR_ಕೆಲಸ:
	ಸ್ಥಿತಿ VAR_CHANNEL:
	ಸ್ಥಿತಿ VAR_INSTR:
	ಸ್ಥಿತಿ VAR_OBJECT:
	    ಆದರೆ (verbose)
		emsg(_(e_cannot_ಸೂಚ್ಯಂಕ_special_variable));
	    ಹಿಂತಿರುಗಿ FAIL;
	ಸ್ಥಿತಿ VAR_CLASS:
	ಸ್ಥಿತಿ VAR_TYPEALIAS:
	    ಆದರೆ (verbose)
		check_typval_is_ಮೌಲ್ಯ(rettv);
	    ಹಿಂತಿರುಗಿ FAIL;
	ಸ್ಥಿತಿ VAR_UNKNOWN:
	ಸ್ಥಿತಿ VAR_ANY:
	ಸ್ಥಿತಿ VAR_VOID:
	    ಆದರೆ (evaluate)
	    {
		emsg(_(e_cannot_ಸೂಚ್ಯಂಕ_special_variable));
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    // FALLTHROUGH

	ಸ್ಥಿತಿ VAR_STRING:
	ಸ್ಥಿತಿ VAR_LIST:
	ಸ್ಥಿತಿ VAR_TUPLE:
	ಸ್ಥಿತಿ VAR_DICT:
	ಸ್ಥಿತಿ VAR_BLOB:
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_ಸಂಖ್ಯೆ:
	    ಆದರೆ (ಒಳ_vim9script())
		emsg(_(e_cannot_ಸೂಚ್ಯಂಕ_ಸಂಖ್ಯೆ));
	    ನಿಲ್ಲಿಸು;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Apply index or range to "rettv".
 * "var1" is the first index, NULL for [:expr].
 * "var2" is the second index, NULL for [expr] and [expr: ]
 * "exclusive" is TRUE for slice(): second index is exclusive, use character
 * index for string.
 * Alternatively, "key" is not NULL, then key[keylen] is the dict index.
 */
    ಪೂರ್ಣಾಂಕ
eval_ಸೂಚ್ಯಂಕ_inner(
	typval_T    *rettv,
	ಪೂರ್ಣಾಂಕ	    is_range,
	typval_T    *var1,
	typval_T    *var2,
	ಪೂರ್ಣಾಂಕ	    exclusive,
	char_u	    *ಕೀಲಿ,
	ಪೂರ್ಣಾಂಕ	    keylen,
	ಪೂರ್ಣಾಂಕ	    verbose)
{
    varnumber_T	    n1, n2 = 0;
    ದೀರ್ಘ	    len;

    n1 = 0;
    ಆದರೆ (var1 != ಖಾಲಿ && rettv->v_type != VAR_DICT)
	n1 = tv_get_ಸಂಖ್ಯೆ(var1);

    ಆದರೆ (is_range)
    {
	ಆದರೆ (rettv->v_type == VAR_DICT)
	{
	    ಆದರೆ (verbose)
		emsg(_(e_cannot_ತುಂಡು_dictionary));
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	ಆದರೆ (var2 != ಖಾಲಿ)
	    n2 = tv_get_ಸಂಖ್ಯೆ(var2);
	ಇಲ್ಲದಿದ್ದರೆ
	    n2 = VARNUM_MAX;
    }

    ಆಯ್ಕೆ (rettv->v_type)
    {
	ಸ್ಥಿತಿ VAR_UNKNOWN:
	ಸ್ಥಿತಿ VAR_ANY:
	ಸ್ಥಿತಿ VAR_VOID:
	ಸ್ಥಿತಿ VAR_FUNC:
	ಸ್ಥಿತಿ VAR_PARTIAL:
	ಸ್ಥಿತಿ VAR_FLOAT:
	ಸ್ಥಿತಿ VAR_BOOL:
	ಸ್ಥಿತಿ VAR_SPECIAL:
	ಸ್ಥಿತಿ VAR_ಕೆಲಸ:
	ಸ್ಥಿತಿ VAR_CHANNEL:
	ಸ್ಥಿತಿ VAR_INSTR:
	ಸ್ಥಿತಿ VAR_CLASS:
	ಸ್ಥಿತಿ VAR_OBJECT:
	ಸ್ಥಿತಿ VAR_TYPEALIAS:
	    ನಿಲ್ಲಿಸು; // not evaluating, skipping over subscript

	ಸ್ಥಿತಿ VAR_ಸಂಖ್ಯೆ:
	ಸ್ಥಿತಿ VAR_STRING:
	    {
		char_u	*s = tv_get_string(rettv);

		len = (ದೀರ್ಘ)STRLEN(s);
		ಆದರೆ (ಒಳ_vim9script() || exclusive)
		{
		    ಆದರೆ (is_range)
			s = string_ತುಂಡು(s, n1, n2, exclusive);
		    ಇಲ್ಲದಿದ್ದರೆ
			s = char_from_string(s, n1);
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (is_range)
		{
		    // The resulting variable is a substring.  If the indexes
		    // are out of range the result is empty.
		    ಆದರೆ (n1 < 0)
		    {
			n1 = len + n1;
			ಆದರೆ (n1 < 0)
			    n1 = 0;
		    }
		    ಆದರೆ (n2 < 0)
			n2 = len + n2;
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (n2 >= len)
			n2 = len;
		    ಆದರೆ (n1 >= len || n2 < 0 || n1 > n2)
			s = ಖಾಲಿ;
		    ಇಲ್ಲದಿದ್ದರೆ
			s = vim_strnsave(s + n1, n2 - n1 + 1);
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    // The resulting variable is a string of a single
		    // character.  If the index is too big or negative the
		    // result is empty.
		    ಆದರೆ (n1 >= len || n1 < 0)
			s = ಖಾಲಿ;
		    ಇಲ್ಲದಿದ್ದರೆ
			s = vim_strnsave(s + n1, 1);
		}
		clear_tv(rettv);
		rettv->v_type = VAR_STRING;
		rettv->vval.v_string = s;
	    }
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ VAR_BLOB:
	    blob_ತುಂಡು_or_ಸೂಚ್ಯಂಕ(rettv->vval.v_blob, is_range, n1, n2,
							     exclusive, rettv);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ VAR_LIST:
	    ಆದರೆ (var1 == ಖಾಲಿ)
		n1 = 0;
	    ಆದರೆ (var2 == ಖಾಲಿ)
		n2 = VARNUM_MAX;
	    ಆದರೆ (list_ತುಂಡು_or_ಸೂಚ್ಯಂಕ(rettv->vval.v_list,
			  is_range, n1, n2, exclusive, rettv, verbose) == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ VAR_TUPLE:
	    ಆದರೆ (var1 == ಖಾಲಿ)
		n1 = 0;
	    ಆದರೆ (var2 == ಖಾಲಿ)
		n2 = VARNUM_MAX;
	    ಆದರೆ (tuple_ತುಂಡು_or_ಸೂಚ್ಯಂಕ(rettv->vval.v_tuple,
			  is_range, n1, n2, exclusive, rettv, verbose) == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ VAR_DICT:
	    {
		dictitem_T	*ಅಂಶ;
		typval_T	tmp;

		ಆದರೆ (ಕೀಲಿ == ಖಾಲಿ)
		{
		    ಕೀಲಿ = tv_get_string_chk(var1);
		    ಆದರೆ (ಕೀಲಿ == ಖಾಲಿ)
			ಹಿಂತಿರುಗಿ FAIL;
		}

		ಅಂಶ = dict_ಕಂಡುಹಿಡಿ(rettv->vval.v_dict, ಕೀಲಿ, keylen);

		ಆದರೆ (ಅಂಶ == ಖಾಲಿ)
		{
		    ಆದರೆ (verbose)
		    {
			ಆದರೆ (keylen > 0)
			    ಕೀಲಿ[keylen] = NUL;
			semsg(_(e_ಕೀಲಿ_not_present_ಒಳ_dictionary_str), ಕೀಲಿ);
		    }
		    ಹಿಂತಿರುಗಿ FAIL;
		}

		copy_tv(&ಅಂಶ->di_tv, &tmp);
		clear_tv(rettv);
		*rettv = tmp;
	    }
	    ನಿಲ್ಲಿಸು;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Return the function name of partial "pt".
 */
    char_u *
partial_name(partial_T *pt)
{
    ಆದರೆ (pt != ಖಾಲಿ)
    {
	ಆದರೆ (pt->pt_name != ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ pt->pt_name;
	ಆದರೆ (pt->pt_func != ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ pt->pt_func->uf_name;
    }
    ಹಿಂತಿರುಗಿ (char_u *)"";
}

    ಸ್ಥಿರ ಶೂನ್ಯ
partial_free(partial_T *pt)
{
    ಪೂರ್ಣಾಂಕ i;

    ಗಾಗಿ (i = 0; i < pt->pt_argc; ++i)
	clear_tv(&pt->pt_argv[i]);
    vim_free(pt->pt_argv);
    dict_unref(pt->pt_dict);
    ಆದರೆ (pt->pt_name != ಖಾಲಿ)
    {
	func_unref(pt->pt_name);
	vim_free(pt->pt_name);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	func_ptr_unref(pt->pt_func);
    ಆದರೆ (pt->pt_obj != ಖಾಲಿ)
	object_unref(pt->pt_obj);

    // "out_up" is no longer used, decrement refcount on partial that owns it.
    partial_unref(pt->pt_outer.ಹೊರಹೋಗು_ಮೇಲೆ_partial);

    // Using pt_outer from another partial.
    partial_unref(pt->pt_outer_partial);

    // Decrease the reference count for the context of a closure.  If down
    // to the minimum it may be time to free it.
    ಆದರೆ (pt->pt_funcstack != ಖಾಲಿ)
    {
	--pt->pt_funcstack->fs_refcount;
	funcstack_check_refcount(pt->pt_funcstack);
    }
    // Similarly for loop variables.
    ಗಾಗಿ (i = 0; i < MAX_LOOP_ಆಳ; ++i)
	ಆದರೆ (pt->pt_loopvars[i] != ಖಾಲಿ)
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
    ಆದರೆ (pt == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಪೂರ್ಣಾಂಕ	ಮುಗಿದ = FALSE;

    ಆದರೆ (--pt->pt_refcount <= 0)
	partial_free(pt);

    // If the reference count goes down to one, the funcstack may be the
    // only reference and can be freed if no other partials reference it.
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (pt->pt_refcount == 1)
    {
	// careful: if the funcstack is freed it may contain this partial
	// and it gets freed as well
	ಆದರೆ (pt->pt_funcstack != ಖಾಲಿ)
	    ಮುಗಿದ = funcstack_check_refcount(pt->pt_funcstack);

	ಆದರೆ (!ಮುಗಿದ)
	{
	    ಪೂರ್ಣಾಂಕ	ಆಳ;

	    ಗಾಗಿ (ಆಳ = 0; ಆಳ < MAX_LOOP_ಆಳ; ++ಆಳ)
		ಆದರೆ (pt->pt_loopvars[ಆಳ] != ಖಾಲಿ
			&& loopvars_check_refcount(pt->pt_loopvars[ಆಳ]))
		    ನಿಲ್ಲಿಸು;
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
    ಪೂರ್ಣಾಂಕ		echo_style,
    ಪೂರ್ಣಾಂಕ		ಸಂಯುಕ್ತ_val)
{
    char_u	*r = ಖಾಲಿ;

    ಆದರೆ (echo_style && !ಸಂಯುಕ್ತ_val)
    {
	*tofree = ಖಾಲಿ;
	r = tv->vval.v_string;
	ಆದರೆ (r == ಖಾಲಿ)
	    r = (char_u *)"";
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	*tofree = string_quote(tv->vval.v_string, FALSE);
	r = *tofree;
    }

    ಹಿಂತಿರುಗಿ r;
}

/*
 * Return a textual representation of a function in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "echo_style" is FALSE, put quotes around the function name as
 * "function()", otherwise does not put quotes around function name.
 * May return NULL.
 */
    ಸ್ಥಿರ char_u *
func_tv2string(typval_T *tv, char_u **tofree, ಪೂರ್ಣಾಂಕ echo_style)
{
    char_u	*r = ಖಾಲಿ;
    char_u	buf[MAX_FUNC_NAME_LEN];

    ಆದರೆ (echo_style)
    {
	*tofree = ಖಾಲಿ;

	ಆದರೆ (tv->vval.v_string == ಖಾಲಿ)
	    r = (char_u *)"function()";
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    r = make_ufunc_name_readable(tv->vval.v_string, buf,
							MAX_FUNC_NAME_LEN);
	    ಆದರೆ (r == buf)
		r = *tofree = vim_strsave(buf);
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	char_u *s = ಖಾಲಿ;

	ಆದರೆ (tv->vval.v_string != ಖಾಲಿ)
	    s = make_ufunc_name_readable(tv->vval.v_string, buf,
							MAX_FUNC_NAME_LEN);

	r = *tofree = string_quote(s, TRUE);
    }

    ಹಿಂತಿರುಗಿ r;
}

/*
 * Return a textual representation of the object method in "tv", a VAR_PARTIAL.
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "echo_style" is FALSE, put quotes around the function name as
 * "function()", otherwise does not put quotes around function name.
 * May return NULL.
 */
    ಸ್ಥಿರ char_u *
ವಿಧಾನ_tv2string(typval_T *tv, char_u **tofree, ಪೂರ್ಣಾಂಕ echo_style)
{
    char_u	buf[MAX_FUNC_NAME_LEN];
    partial_T	*pt = tv->vval.v_partial;

    size_t len = vim_snprintf((ಅಕ್ಷರ *)buf, ಗಾತ್ರ(buf), "<SNR>%d_%s.%s",
			   pt->pt_func->uf_script_ctx.sc_sid,
			   pt->pt_func->uf_class->class_name.string,
			   pt->pt_func->uf_name);
    ಆದರೆ (len >= ಗಾತ್ರ(buf))
    {
	ಆದರೆ (echo_style)
	{
	    *tofree = ಖಾಲಿ;
	    ಹಿಂತಿರುಗಿ (char_u *)"function()";
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ಹಿಂತಿರುಗಿ *tofree = string_quote((char_u*)"", TRUE);
    }

    ಹಿಂತಿರುಗಿ *tofree = echo_style ? vim_strsave(buf) : string_quote(buf, TRUE);
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
    ಪೂರ್ಣಾಂಕ		copyID)
{
    char_u	*r = ಖಾಲಿ;
    partial_T	*pt;
    char_u	*fname;
    garray_T	ga;
    ಪೂರ್ಣಾಂಕ		i;
    char_u	*tf;

    pt = tv->vval.v_partial;
    fname = string_quote(pt == ಖಾಲಿ ? ಖಾಲಿ : partial_name(pt), FALSE);

    ga_init2(&ga, 1, 100);
    GA_CONCAT_LITERAL(&ga, "function(");
    ಆದರೆ (fname != ಖಾಲಿ)
    {
	// When using uf_name prepend "g:" for a global function.
	ಆದರೆ (pt != ಖಾಲಿ && pt->pt_name == ಖಾಲಿ && fname[0] == '\''
						&& vim_isupper(fname[1]))
	{
	    GA_CONCAT_LITERAL(&ga, "'g:");
	    ga_concat(&ga, fname + 1);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ga_concat(&ga, fname);
	vim_free(fname);
    }
    ಆದರೆ (pt != ಖಾಲಿ && pt->pt_argc > 0)
    {
	GA_CONCAT_LITERAL(&ga, ", [");
	ಗಾಗಿ (i = 0; i < pt->pt_argc; ++i)
	{
	    ಆದರೆ (i > 0)
		GA_CONCAT_LITERAL(&ga, ", ");
	    ga_concat(&ga, tv2string(&pt->pt_argv[i], &tf, numbuf, copyID));
	    vim_free(tf);
	}
	GA_CONCAT_LITERAL(&ga, "]");
    }
    ಆದರೆ (pt != ಖಾಲಿ && pt->pt_dict != ಖಾಲಿ)
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

    *tofree = ga.ga_ದತ್ತಾಂಶ;
    r = *tofree;

    ಹಿಂತಿರುಗಿ r;
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
    ಪೂರ್ಣಾಂಕ		copyID,
    ಪೂರ್ಣಾಂಕ		restore_copyID)
{
    char_u	*r = ಖಾಲಿ;

    ಆದರೆ (tv->vval.v_list == ಖಾಲಿ)
    {
	// NULL list is equivalent to empty list.
	*tofree = ಖಾಲಿ;
	r = (char_u *)"[]";
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (copyID != 0 && tv->vval.v_list->lv_copyID == copyID
	    && tv->vval.v_list->lv_len > 0)
    {
	*tofree = ಖಾಲಿ;
	r = (char_u *)"[...]";
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಪೂರ್ಣಾಂಕ old_copyID;
	ಆದರೆ (restore_copyID)
	    old_copyID = tv->vval.v_list->lv_copyID;

	tv->vval.v_list->lv_copyID = copyID;
	*tofree = list2string(tv, copyID, restore_copyID);
	ಆದರೆ (restore_copyID)
	    tv->vval.v_list->lv_copyID = old_copyID;
	r = *tofree;
    }

    ಹಿಂತಿರುಗಿ r;
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
    ಪೂರ್ಣಾಂಕ		copyID,
    ಪೂರ್ಣಾಂಕ		restore_copyID)
{
    tuple_T	*tuple = tv->vval.v_tuple;
    char_u	*r = ಖಾಲಿ;

    ಆದರೆ (tuple == ಖಾಲಿ)
    {
	// NULL tuple is equivalent to an empty tuple.
	*tofree = ಖಾಲಿ;
	r = (char_u *)"()";
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (copyID != 0 && tuple->tv_copyID == copyID
					&& tuple->tv_ವಸ್ತುಗಳು.ga_len > 0)
    {
	*tofree = ಖಾಲಿ;
	r = (char_u *)"(...)";
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಪೂರ್ಣಾಂಕ old_copyID;
	ಆದರೆ (restore_copyID)
	    old_copyID = tuple->tv_copyID;

	tuple->tv_copyID = copyID;
	*tofree = tuple2string(tv, copyID, restore_copyID);
	ಆದರೆ (restore_copyID)
	    tuple->tv_copyID = old_copyID;
	r = *tofree;
    }

    ಹಿಂತಿರುಗಿ r;
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
    ಪೂರ್ಣಾಂಕ		copyID,
    ಪೂರ್ಣಾಂಕ		restore_copyID)
{
    char_u	*r = ಖಾಲಿ;

    ಆದರೆ (tv->vval.v_dict == ಖಾಲಿ)
    {
	// NULL dict is equivalent to empty dict.
	*tofree = ಖಾಲಿ;
	r = (char_u *)"{}";
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (copyID != 0 && tv->vval.v_dict->dv_copyID == copyID
	    && tv->vval.v_dict->dv_hashtab.ht_ಬಳಸಿದ != 0)
    {
	*tofree = ಖಾಲಿ;
	r = (char_u *)"{...}";
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಪೂರ್ಣಾಂಕ old_copyID;
	ಆದರೆ (restore_copyID)
	    old_copyID = tv->vval.v_dict->dv_copyID;

	tv->vval.v_dict->dv_copyID = copyID;
	*tofree = dict2string(tv, copyID, restore_copyID);
	ಆದರೆ (restore_copyID)
	    tv->vval.v_dict->dv_copyID = old_copyID;
	r = *tofree;
    }

    ಹಿಂತಿರುಗಿ r;
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
    typval_T	*tv ಬಳಸದ,
    char_u	**tofree ಬಳಸದ,
    char_u	*numbuf ಬಳಸದ,
    ಪೂರ್ಣಾಂಕ		ಸಂಯುಕ್ತ_val ಬಳಸದ)
{
    char_u	*r = ಖಾಲಿ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
    *tofree = ಖಾಲಿ;

    ಆದರೆ (tv->v_type == VAR_ಕೆಲಸ)
	r = ಕೆಲಸ_ಗಮ್ಯ_string_buf(tv, numbuf);
    ಇಲ್ಲದಿದ್ದರೆ
	r = channel_ಗಮ್ಯ_string_buf(tv, numbuf);

    ಆದರೆ (ಸಂಯುಕ್ತ_val)
    {
	*tofree = string_quote(r, FALSE);
	r = *tofree;
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ r;
}

/*
 * Return a textual representation of a class in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * May return NULL.
 */
    ಸ್ಥಿರ char_u *
class_tv2string(typval_T *tv, char_u **tofree)
{
    char_u	*r = ಖಾಲಿ;
    size_t	rsize;
    class_T	*cl = tv->vval.v_class;
    string_T	class_name = {(char_u *)"[unknown]", 9};
    string_T	s = {(char_u *)"class", 5};

    ಆದರೆ (cl != ಖಾಲಿ)
    {
	class_name.string = cl->class_name.string;
	class_name.ಉದ್ದಳತೆ = cl->class_name.ಉದ್ದಳತೆ;
	ಆದರೆ (IS_INTERFACE(cl))
	    STR_LITERAL_ಗಣ(s, "interface");
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (IS_ENUM(cl))
	    STR_LITERAL_ಗಣ(s, "enum");
    }

    rsize = s.ಉದ್ದಳತೆ + 1 + class_name.ಉದ್ದಳತೆ + 1;
    r = *tofree = alloc(rsize);
    ಆದರೆ (r != ಖಾಲಿ)
	vim_snprintf((ಅಕ್ಷರ *)r, rsize, "%s %s", s.string, (ಅಕ್ಷರ *)class_name.string);

    ಹಿಂತಿರುಗಿ r;
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
    ಪೂರ್ಣಾಂಕ		copyID,
    ಪೂರ್ಣಾಂಕ		restore_copyID,
    char_u	*numbuf,
    ಪೂರ್ಣಾಂಕ		echo_style,
    ಪೂರ್ಣಾಂಕ		ಸಂಯುಕ್ತ_val)
{
    char_u	*r = ಖಾಲಿ;

    object_T	*obj = tv->vval.v_object;
    ಆದರೆ (obj == ಖಾಲಿ || obj->obj_class == ಖಾಲಿ)
    {
	*tofree = ಖಾಲಿ;
	r = (char_u *)"object of [unknown]";
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (copyID != 0 && obj->obj_copyID == copyID
	    && obj->obj_class->class_obj_member_ಎಣಿಕೆ != 0)
    {
	size_t n = 25 + obj->obj_class->class_name.ಉದ್ದಳತೆ;
	r = alloc(n);
	ಆದರೆ (r != ಖಾಲಿ)
	    (ಶೂನ್ಯ)vim_snprintf((ಅಕ್ಷರ *)r, n, "object of %s {...}",
						obj->obj_class->class_name.string);
	*tofree = r;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಪೂರ್ಣಾಂಕ old_copyID;
	ಆದರೆ (restore_copyID)
	    old_copyID = obj->obj_copyID;

	obj->obj_copyID = copyID;
	*tofree = object2string(obj, numbuf, copyID, echo_style,
				restore_copyID, ಸಂಯುಕ್ತ_val);
	ಆದರೆ (restore_copyID)
	    obj->obj_copyID = old_copyID;
	r = *tofree;
    }

    ಹಿಂತಿರುಗಿ r;
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
    ಪೂರ್ಣಾಂಕ		copyID,
    ಪೂರ್ಣಾಂಕ		echo_style,
    ಪೂರ್ಣಾಂಕ		restore_copyID,
    ಪೂರ್ಣಾಂಕ		ಸಂಯುಕ್ತ_val)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	recurse = 0;
    char_u	*r = ಖಾಲಿ;

    ಆದರೆ (recurse >= DICT_MAXNEST)
    {
	ಆದರೆ (!did_echo_string_emsg)
	{
	    // Only give this message once for a recursive call to avoid
	    // flooding the user with errors.  And stop iterating over lists
	    // and dicts and objects.
	    did_echo_string_emsg = TRUE;
	    emsg(_(e_variable_nested_too_deep_for_displaying));
	}
	*tofree = ಖಾಲಿ;
	ಹಿಂತಿರುಗಿ (char_u *)"{E724}";
    }
    ++recurse;

    ಆಯ್ಕೆ (tv->v_type)
    {
	ಸ್ಥಿತಿ VAR_STRING:
	    r = string_tv2string(tv, tofree, echo_style, ಸಂಯುಕ್ತ_val);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ VAR_FUNC:
	    r = func_tv2string(tv, tofree, echo_style);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ VAR_PARTIAL:
	    ಆದರೆ (tv->vval.v_partial == ಖಾಲಿ
		    || tv->vval.v_partial->pt_obj == ಖಾಲಿ)
		r = partial_tv2string(tv, tofree, numbuf, copyID);
	    ಇಲ್ಲದಿದ್ದರೆ
		r = ವಿಧಾನ_tv2string(tv, tofree, echo_style);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ VAR_BLOB:
	    r = blob2string(tv->vval.v_blob, tofree, numbuf);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ VAR_LIST:
	    r = list_tv2string(tv, tofree, copyID, restore_copyID);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ VAR_TUPLE:
	    r = tuple_tv2string(tv, tofree, copyID, restore_copyID);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ VAR_DICT:
	    r = dict_tv2string(tv, tofree, copyID, restore_copyID);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ VAR_ಸಂಖ್ಯೆ:
	ಸ್ಥಿತಿ VAR_UNKNOWN:
	ಸ್ಥಿತಿ VAR_ANY:
	ಸ್ಥಿತಿ VAR_VOID:
	    *tofree = ಖಾಲಿ;
	    r = tv_get_string_buf(tv, numbuf);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ VAR_ಕೆಲಸ:
	ಸ್ಥಿತಿ VAR_CHANNEL:
	    r = jobchan_tv2string(tv, tofree, numbuf, ಸಂಯುಕ್ತ_val);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ VAR_INSTR:
	    *tofree = ಖಾಲಿ;
	    r = (char_u *)"instructions";
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ VAR_CLASS:
	    r = class_tv2string(tv, tofree);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ VAR_OBJECT:
	    r = object_tv2string(tv, tofree, copyID, restore_copyID,
					 numbuf, echo_style, ಸಂಯುಕ್ತ_val);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ VAR_FLOAT:
	    *tofree = ಖಾಲಿ;
	    vim_snprintf((ಅಕ್ಷರ *)numbuf, NUMBUFLEN, "%g", tv->vval.v_float);
	    r = numbuf;
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ VAR_BOOL:
	ಸ್ಥಿತಿ VAR_SPECIAL:
	    *tofree = ಖಾಲಿ;
	    r = (char_u *)get_var_special_name(tv->vval.v_ಸಂಖ್ಯೆ);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ VAR_TYPEALIAS:
	    *tofree = vim_strsave(tv->vval.v_typealias->ta_name);
	    r = *tofree;
	    ಆದರೆ (r == ಖಾಲಿ)
		r = (char_u *)"";
	    ನಿಲ್ಲಿಸು;
    }

    ಆದರೆ (--recurse == 0)
	did_echo_string_emsg = FALSE;
    ಹಿಂತಿರುಗಿ r;
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
    ಪೂರ್ಣಾಂಕ		copyID)
{
    ಹಿಂತಿರುಗಿ echo_string_core(tv, tofree, numbuf, copyID, TRUE, FALSE, FALSE);
}

/*
 * Convert the specified byte index of line 'lnum' in buffer 'buf' to a
 * character index.  Works only for loaded buffers. Returns -1 on failure.
 * The index of the first byte and the first character is zero.
 */
    ಪೂರ್ಣಾಂಕ
buf_byteidx_ಗಮ್ಯ_charidx(buf_T *buf, ಪೂರ್ಣಾಂಕ lnum, ಪೂರ್ಣಾಂಕ byteidx)
{
    char_u	*str;
    char_u	*t;
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2;

    ಆದರೆ (buf == ಖಾಲಿ || buf->b_ml.ml_mfp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ -1;

    ಆದರೆ (lnum > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	lnum = buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;

    str = ml_get_buf(buf, lnum, FALSE);
    ಆದರೆ (str == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ -1;

    ಆದರೆ (*str == NUL)
	ಹಿಂತಿರುಗಿ 0;

    // count the number of characters
    t = str;
    ಗಾಗಿ (ಎಣಿಕೆ_2 = 0; *t != NUL && t <= str + byteidx; ಎಣಿಕೆ_2++)
	t += mb_ptr2len(t);

    // In insert mode, when the cursor is at the end of a non-empty line,
    // byteidx points to the NUL character immediately past the end of the
    // string. In this case, add one to the character count.
    ಆದರೆ (*t == NUL && byteidx != 0 && t == str + byteidx)
	ಎಣಿಕೆ_2++;

    ಹಿಂತಿರುಗಿ ಎಣಿಕೆ_2 - 1;
}

/*
 * Convert the specified character index of line 'lnum' in buffer 'buf' to a
 * byte index.  Works only for loaded buffers. Returns -1 on failure.
 * The index of the first byte and the first character is zero.
 */
    ಪೂರ್ಣಾಂಕ
buf_charidx_ಗಮ್ಯ_byteidx(buf_T *buf, ಪೂರ್ಣಾಂಕ lnum, ಪೂರ್ಣಾಂಕ charidx)
{
    char_u	*str;
    char_u	*t;

    ಆದರೆ (buf == ಖಾಲಿ || buf->b_ml.ml_mfp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ -1;

    ಆದರೆ (lnum > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	lnum = buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;

    str = ml_get_buf(buf, lnum, FALSE);
    ಆದರೆ (str == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ -1;

    // Convert the character offset to a byte offset
    t = str;
    ಆಗ (*t != NUL && --charidx > 0)
	t += mb_ptr2len(t);

    ಹಿಂತಿರುಗಿ t - str;
}

/*
 * Translate a String variable into a position.
 * Returns NULL when there is an error.
 */
    pos_T *
var2fpos(
    typval_T	*varp,
    ಪೂರ್ಣಾಂಕ		dollar_lnum,	// TRUE when $ is last line
    ಪೂರ್ಣಾಂಕ		*fnum,		// set to fnum for '0, 'A, etc.
    ಪೂರ್ಣಾಂಕ		charcol)	// return character column
{
    char_u		*name;
    ಸ್ಥಿರ pos_T	pos;
    pos_T		*pp;
    ಪೂರ್ಣಾಂಕ			error = FALSE;

    // Argument can be [lnum, col, coladd].
    ಆದರೆ (varp->v_type == VAR_LIST)
    {
	list_T		*l;
	ಪೂರ್ಣಾಂಕ		len;
	listitem_T	*li;

	l = varp->vval.v_list;
	ಆದರೆ (l == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	// Get the line number
	pos.lnum = list_ಕಂಡುಹಿಡಿ_nr(l, 0L, &error);
	ಆದರೆ (error || pos.lnum <= 0 || pos.lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;	// invalid line number
	ಆದರೆ (charcol)
	    len = (ದೀರ್ಘ)mb_charlen(ml_get(pos.lnum));
	ಇಲ್ಲದಿದ್ದರೆ
	    len = (ದೀರ್ಘ)ml_get_len(pos.lnum);

	// Get the column number
	// We accept "$" for the column number: last column.
	li = list_ಕಂಡುಹಿಡಿ(l, 1L);
	ಆದರೆ (li != ಖಾಲಿ && li->li_tv.v_type == VAR_STRING
		&& li->li_tv.vval.v_string != ಖಾಲಿ
		&& STRCMP(li->li_tv.vval.v_string, "$") == 0)
	{
	    pos.ಲಂಬಸಾಲು = len + 1;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    pos.ಲಂಬಸಾಲು = list_ಕಂಡುಹಿಡಿ_nr(l, 1L, &error);
	    ಆದರೆ (error)
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}

	// Accept a position up to the NUL after the line.
	ಆದರೆ (pos.ಲಂಬಸಾಲು == 0 || (ಪೂರ್ಣಾಂಕ)pos.ಲಂಬಸಾಲು > len + 1)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;	// invalid column number
	--pos.ಲಂಬಸಾಲು;

	// Get the virtual offset.  Defaults to zero.
	pos.coladd = list_ಕಂಡುಹಿಡಿ_nr(l, 2L, &error);
	ಆದರೆ (error)
	    pos.coladd = 0;

	ಹಿಂತಿರುಗಿ &pos;
    }

    ಆದರೆ (ಒಳ_vim9script() && check_for_string_arg(varp, 0) == FAIL)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    name = tv_get_string_chk(varp);
    ಆದರೆ (name == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    error = TRUE;
    pos.lnum = 0;
    ಆದರೆ (name[0] == '.')
    {
	ಆದರೆ (!ಒಳ_vim9script() || name[1] == NUL)
	{
	    error = FALSE;
	    // cursor
	    pos = curwin->w_cursor;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (name[0] == 'v' && name[1] == NUL)
    {
	// Visual start
	ಆದರೆ (VIsual_ಸಕ್ರಿಯ)
	    pos = VIsual;
	ಇಲ್ಲದಿದ್ದರೆ
	    pos = curwin->w_cursor;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (name[0] == '\'')
    {
	ಆದರೆ (!ಒಳ_vim9script() || (name[1] != NUL && name[2] == NUL))
	{
	    error = FALSE;
	    // mark
	    pp = getmark_buf_fnum(curbuf, name[1], FALSE, fnum);
	    ಆದರೆ (pp == ಖಾಲಿ || pp == (pos_T *)-1 || pp->lnum <= 0)
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    pos = *pp;
	}
    }
    ಆದರೆ (pos.lnum != 0)
    {
	ಆದರೆ (charcol)
	    pos.ಲಂಬಸಾಲು = buf_byteidx_ಗಮ್ಯ_charidx(curbuf, pos.lnum, pos.ಲಂಬಸಾಲು);
	ಹಿಂತಿರುಗಿ &pos;
    }

    pos.coladd = 0;

    ಆದರೆ (name[0] == 'w' && dollar_lnum)
    {
	// the "w_valid" flags are not reset when moving the cursor, but they
	// do matter for update_topline() and validate_botline().
	check_cursor_moved(curwin);

	pos.ಲಂಬಸಾಲು = 0;
	ಆದರೆ (name[1] == '0')		// "w0": first visible line
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
	    ಆದರೆ (bt_terminal(curwin->w_ಬಫರ್)
				    && curwin->w_ಬಫರ್->b_ಪದ != ಖಾಲಿ
				    && !ಪದ_ಒಳ_ಸಾಮಾನ್ಯ_ಬಹುಲಕ(curwin->w_ಬಫರ್))
		may_move_terminal_ಗಮ್ಯ_ಬಫರ್(curwin->w_ಬಫರ್->b_ಪದ, TRUE);
#ಆದರೆ_ಅಂತ್ಯ
	    update_topline();
	    // In silent Ex mode topline is zero, but that's not a valid line
	    // number; use one instead.
	    pos.lnum = curwin->w_topline > 0 ? curwin->w_topline : 1;
	    ಹಿಂತಿರುಗಿ &pos;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (name[1] == '$')	// "w$": last visible line
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
	    ಆದರೆ (bt_terminal(curwin->w_ಬಫರ್)
				    && curwin->w_ಬಫರ್->b_ಪದ != ಖಾಲಿ
				    && !ಪದ_ಒಳ_ಸಾಮಾನ್ಯ_ಬಹುಲಕ(curwin->w_ಬಫರ್))
		may_move_terminal_ಗಮ್ಯ_ಬಫರ್(curwin->w_ಬಫರ್->b_ಪದ, TRUE);
#ಆದರೆ_ಅಂತ್ಯ
	    validate_botline();
	    // In silent Ex mode botline is zero, return zero then.
	    pos.lnum = curwin->w_botline > 0 ? curwin->w_botline - 1 : 0;
	    ಹಿಂತಿರುಗಿ &pos;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (name[0] == '$')		// last column or line
    {
	ಆದರೆ (dollar_lnum)
	{
	    pos.lnum = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	    pos.ಲಂಬಸಾಲು = 0;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    pos.lnum = curwin->w_cursor.lnum;
	    ಆದರೆ (charcol)
		pos.ಲಂಬಸಾಲು = (colnr_T)mb_charlen(ml_get_curline());
	    ಇಲ್ಲದಿದ್ದರೆ
		pos.ಲಂಬಸಾಲು = ml_get_curline_len();
	}
	ಹಿಂತಿರುಗಿ &pos;
    }
    ಆದರೆ (ಒಳ_vim9script() && error)
	semsg(_(e_invalid_ಮೌಲ್ಯ_for_ಸಾಲು_ಸಂಖ್ಯೆ_str), name);
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
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
    ಪೂರ್ಣಾಂಕ
list2fpos(
    typval_T	*arg,
    pos_T	*posp,
    ಪೂರ್ಣಾಂಕ		*fnump,
    colnr_T	*curswantp,
    ಪೂರ್ಣಾಂಕ		charcol)
{
    list_T	*l = arg->vval.v_list;
    ದೀರ್ಘ	i = 0;
    ದೀರ್ಘ	n;

    // List must be: [fnum, lnum, col, coladd, curswant], where "fnum" is only
    // there when "fnump" isn't NULL; "coladd" and "curswant" are optional.
    ಆದರೆ (arg->v_type != VAR_LIST
	    || l == ಖಾಲಿ
	    || l->lv_len < (fnump == ಖಾಲಿ ? 2 : 3)
	    || l->lv_len > (fnump == ಖಾಲಿ ? 4 : 5))
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (fnump != ಖಾಲಿ)
    {
	n = list_ಕಂಡುಹಿಡಿ_nr(l, i++, ಖಾಲಿ);	// fnum
	ಆದರೆ (n < 0)
	    ಹಿಂತಿರುಗಿ FAIL;
	ಆದರೆ (n == 0)
	    n = curbuf->b_fnum;		// current buffer
	*fnump = n;
    }

    n = list_ಕಂಡುಹಿಡಿ_nr(l, i++, ಖಾಲಿ);	// lnum
    ಆದರೆ (n < 0)
	ಹಿಂತಿರುಗಿ FAIL;
    posp->lnum = n;

    n = list_ಕಂಡುಹಿಡಿ_nr(l, i++, ಖಾಲಿ);	//ಲಂಬಸಾಲು
    ಆದರೆ (n < 0)
	ಹಿಂತಿರುಗಿ FAIL;
    // If character position is specified, then convert to byte position
    // If the line number is zero use the cursor line.
    ಆದರೆ (charcol)
    {
	buf_T	*buf;

	// Get the text for the specified line in a loaded buffer
	buf = buflist_findnr(fnump == ಖಾಲಿ ? curbuf->b_fnum : *fnump);
	ಆದರೆ (buf == ಖಾಲಿ || buf->b_ml.ml_mfp == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;

	n = buf_charidx_ಗಮ್ಯ_byteidx(buf,
		  posp->lnum == 0 ? curwin->w_cursor.lnum : posp->lnum, n) + 1;
    }
    posp->ಲಂಬಸಾಲು = n;

    n = list_ಕಂಡುಹಿಡಿ_nr(l, i, ಖಾಲಿ);	// off
    ಆದರೆ (n < 0)
	posp->coladd = 0;
    ಇಲ್ಲದಿದ್ದರೆ
	posp->coladd = n;

    ಆದರೆ (curswantp != ಖಾಲಿ)
	*curswantp = list_ಕಂಡುಹಿಡಿ_nr(l, i + 1, ಖಾಲಿ);  // curswant

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Get the length of an environment variable name.
 * Advance "arg" to the first character after the name.
 * Return 0 for error.
 */
    ಪೂರ್ಣಾಂಕ
get_env_len(char_u **arg)
{
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		len;

    ಗಾಗಿ (p = *arg; vim_isIDc(*p); ++p)
	;
    ಆದರೆ (p == *arg)	    // no name found
	ಹಿಂತಿರುಗಿ 0;

    len = (ಪೂರ್ಣಾಂಕ)(p - *arg);
    *arg = p;
    ಹಿಂತಿರುಗಿ len;
}

/*
 * Get the length of the name of a function or internal variable.
 * "arg" is advanced to after the name.
 * Return 0 if something is wrong.
 */
    ಪೂರ್ಣಾಂಕ
get_id_len(char_u **arg)
{
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		len;

    // Find the end of the name.
    ಗಾಗಿ (p = *arg; eval_isnamec(*p); ++p)
    {
	ಆದರೆ (*p == ':')
	{
	    // "s:" is start of "s:var", but "n:" is not and can be used in
	    // slice "[n:]".  Also "xx:" is not a namespace.
	    len = (ಪೂರ್ಣಾಂಕ)(p - *arg);
	    ಆದರೆ ((len == 1 && vim_strchr(NAMESPACE_CHAR, **arg) == ಖಾಲಿ)
		    || len > 1)
		ನಿಲ್ಲಿಸು;
	}
    }
    ಆದರೆ (p == *arg)	    // no name found
	ಹಿಂತಿರುಗಿ 0;

    len = (ಪೂರ್ಣಾಂಕ)(p - *arg);
    *arg = p;

    ಹಿಂತಿರುಗಿ len;
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
    ಪೂರ್ಣಾಂಕ
get_name_len(
    char_u	**arg,
    char_u	**alias,
    ಪೂರ್ಣಾಂಕ		evaluate,
    ಪೂರ್ಣಾಂಕ		verbose)
{
    ಪೂರ್ಣಾಂಕ		len;
    char_u	*p;
    char_u	*ಅಭಿವ್ಯಕ್ತಿ_ಆರಂಭ;
    char_u	*ಅಭಿವ್ಯಕ್ತಿ_ಕೊನೆ;

    *alias = ಖಾಲಿ;  // default to no alias

    ಆದರೆ ((*arg)[0] == K_SPECIAL && (*arg)[1] == KS_EXTRA
						  && (*arg)[2] == (ಪೂರ್ಣಾಂಕ)KE_SNR)
    {
	// hard coded <SNR>, already translated
	*arg += 3;
	ಹಿಂತಿರುಗಿ get_id_len(arg) + 3;
    }
    len = eval_fname_script(*arg);
    ಆದರೆ (len > 0)
    {
	// literal "<SID>", "s:" or "<SNR>"
	*arg += len;
    }

    /*
     * Find the end of the name; check for {} construction.
     */
    p = ಕಂಡುಹಿಡಿ_name_ಕೊನೆ(*arg, &ಅಭಿವ್ಯಕ್ತಿ_ಆರಂಭ, &ಅಭಿವ್ಯಕ್ತಿ_ಕೊನೆ,
					       len > 0 ? 0 : FNE_CHECK_ಆರಂಭ);
    ಆದರೆ (ಅಭಿವ್ಯಕ್ತಿ_ಆರಂಭ != ಖಾಲಿ)
    {
	char_u	*ತಾತ್ಕಾಲಿಕ_string;

	ಆದರೆ (!evaluate)
	{
	    len += (ಪೂರ್ಣಾಂಕ)(p - *arg);
	    *arg = skipwhite(p);
	    ಹಿಂತಿರುಗಿ len;
	}

	/*
	 * Include any <SID> etc in the expanded string:
	 * Thus the -len here.
	 */
	ತಾತ್ಕಾಲಿಕ_string = make_expanded_name(*arg - len, ಅಭಿವ್ಯಕ್ತಿ_ಆರಂಭ, ಅಭಿವ್ಯಕ್ತಿ_ಕೊನೆ, p);
	ಆದರೆ (ತಾತ್ಕಾಲಿಕ_string == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ -1;
	*alias = ತಾತ್ಕಾಲಿಕ_string;
	*arg = skipwhite(p);
	ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)STRLEN(ತಾತ್ಕಾಲಿಕ_string);
    }

    len += get_id_len(arg);
    // Only give an error when there is something, otherwise it will be
    // reported at a higher level.
    ಆದರೆ (len == 0 && verbose && **arg != NUL)
	semsg(_(e_invalid_ಅಭಿವ್ಯಕ್ತಿ_str), *arg);

    ಹಿಂತಿರುಗಿ len;
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
ಕಂಡುಹಿಡಿ_name_ಕೊನೆ(
    char_u	*arg,
    char_u	**ಅಭಿವ್ಯಕ್ತಿ_ಆರಂಭ,
    char_u	**ಅಭಿವ್ಯಕ್ತಿ_ಕೊನೆ,
    ಪೂರ್ಣಾಂಕ		flags)
{
    ಪೂರ್ಣಾಂಕ		mb_nest = 0;
    ಪೂರ್ಣಾಂಕ		br_nest = 0;
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		len;
    ಪೂರ್ಣಾಂಕ		allow_curly = !ಒಳ_vim9script();

    ಆದರೆ (ಅಭಿವ್ಯಕ್ತಿ_ಆರಂಭ != ಖಾಲಿ)
    {
	*ಅಭಿವ್ಯಕ್ತಿ_ಆರಂಭ = ಖಾಲಿ;
	*ಅಭಿವ್ಯಕ್ತಿ_ಕೊನೆ = ಖಾಲಿ;
    }

    // Quick check for valid starting character.
    ಆದರೆ ((flags & FNE_CHECK_ಆರಂಭ) && !eval_isnamec1(*arg)
					      && (*arg != '{' || !allow_curly))
	ಹಿಂತಿರುಗಿ arg;

    ಗಾಗಿ (p = arg; *p != NUL
		    && (eval_isnamec(*p)
			|| (*p == '{' && allow_curly)
			|| ((flags & FNE_INCL_BR) && (*p == '['
					 || (*p == '.' && eval_isdictc(p[1]))))
			|| mb_nest != 0
			|| br_nest != 0); MB_PTR_ADV(p))
    {
	ಆದರೆ (*p == '\'')
	{
	    // skip over 'string' to avoid counting [ and ] inside it.
	    ಗಾಗಿ (p = p + 1; *p != NUL && *p != '\''; MB_PTR_ADV(p))
		;
	    ಆದರೆ (*p == NUL)
		ನಿಲ್ಲಿಸು;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == '"')
	{
	    // skip over "str\"ing" to avoid counting [ and ] inside it.
	    ಗಾಗಿ (p = p + 1; *p != NUL && *p != '"'; MB_PTR_ADV(p))
		ಆದರೆ (*p == '\\' && p[1] != NUL)
		    ++p;
	    ಆದರೆ (*p == NUL)
		ನಿಲ್ಲಿಸು;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (br_nest == 0 && mb_nest == 0 && *p == ':')
	{
	    // "s:" is start of "s:var", but "n:" is not and can be used in
	    // slice "[n:]".  Also "xx:" is not a namespace. But {ns}: is.
	    len = (ಪೂರ್ಣಾಂಕ)(p - arg);
	    ಆದರೆ ((len == 1 && vim_strchr(NAMESPACE_CHAR, *arg) == ಖಾಲಿ)
		    || (len > 1 && p[-1] != '}'))
		ನಿಲ್ಲಿಸು;
	}

	ಆದರೆ (mb_nest == 0)
	{
	    ಆದರೆ (*p == '[')
		++br_nest;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == ']')
		--br_nest;
	}

	ಆದರೆ (br_nest == 0 && allow_curly)
	{
	    ಆದರೆ (*p == '{')
	    {
		mb_nest++;
		ಆದರೆ (ಅಭಿವ್ಯಕ್ತಿ_ಆರಂಭ != ಖಾಲಿ && *ಅಭಿವ್ಯಕ್ತಿ_ಆರಂಭ == ಖಾಲಿ)
		    *ಅಭಿವ್ಯಕ್ತಿ_ಆರಂಭ = p;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == '}')
	    {
		mb_nest--;
		ಆದರೆ (ಅಭಿವ್ಯಕ್ತಿ_ಆರಂಭ != ಖಾಲಿ && mb_nest == 0 && *ಅಭಿವ್ಯಕ್ತಿ_ಕೊನೆ == ಖಾಲಿ)
		    *ಅಭಿವ್ಯಕ್ತಿ_ಕೊನೆ = p;
	    }
	}
    }

    ಹಿಂತಿರುಗಿ p;
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
    char_u	*ಒಳ_ಆರಂಭ,
    char_u	*ಅಭಿವ್ಯಕ್ತಿ_ಆರಂಭ,
    char_u	*ಅಭಿವ್ಯಕ್ತಿ_ಕೊನೆ,
    char_u	*ಒಳ_ಕೊನೆ)
{
    char_u	c1;
    char_u	*retval = ಖಾಲಿ;
    char_u	*ತಾತ್ಕಾಲಿಕ_ಫಲಿತಾಂಶ;

    ಆದರೆ (ಅಭಿವ್ಯಕ್ತಿ_ಕೊನೆ == ಖಾಲಿ || ಒಳ_ಕೊನೆ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    *ಅಭಿವ್ಯಕ್ತಿ_ಆರಂಭ	= NUL;
    *ಅಭಿವ್ಯಕ್ತಿ_ಕೊನೆ = NUL;
    c1 = *ಒಳ_ಕೊನೆ;
    *ಒಳ_ಕೊನೆ = NUL;

    ತಾತ್ಕಾಲಿಕ_ಫಲಿತಾಂಶ = eval_ಗಮ್ಯ_string(ಅಭಿವ್ಯಕ್ತಿ_ಆರಂಭ + 1, FALSE, FALSE);
    ಆದರೆ (ತಾತ್ಕಾಲಿಕ_ಫಲಿತಾಂಶ != ಖಾಲಿ)
    {
	size_t	retvalsize = (size_t)(ಅಭಿವ್ಯಕ್ತಿ_ಆರಂಭ - ಒಳ_ಆರಂಭ)
				+ STRLEN(ತಾತ್ಕಾಲಿಕ_ಫಲಿತಾಂಶ)
				+ (size_t)(ಒಳ_ಕೊನೆ - ಅಭಿವ್ಯಕ್ತಿ_ಕೊನೆ) + 1;

	retval = alloc(retvalsize);
	ಆದರೆ (retval != ಖಾಲಿ)
	    vim_snprintf((ಅಕ್ಷರ *)retval, retvalsize, "%s%s%s",
				ಒಳ_ಆರಂಭ, ತಾತ್ಕಾಲಿಕ_ಫಲಿತಾಂಶ, ಅಭಿವ್ಯಕ್ತಿ_ಕೊನೆ + 1);
    }
    vim_free(ತಾತ್ಕಾಲಿಕ_ಫಲಿತಾಂಶ);

    *ಒಳ_ಕೊನೆ = c1;		// put char back for error messages
    *ಅಭಿವ್ಯಕ್ತಿ_ಆರಂಭ = '{';
    *ಅಭಿವ್ಯಕ್ತಿ_ಕೊನೆ = '}';

    ಆದರೆ (retval != ಖಾಲಿ)
    {
	ತಾತ್ಕಾಲಿಕ_ಫಲಿತಾಂಶ = ಕಂಡುಹಿಡಿ_name_ಕೊನೆ(retval, &ಅಭಿವ್ಯಕ್ತಿ_ಆರಂಭ, &ಅಭಿವ್ಯಕ್ತಿ_ಕೊನೆ, 0);
	ಆದರೆ (ಅಭಿವ್ಯಕ್ತಿ_ಆರಂಭ != ಖಾಲಿ)
	{
	    // Further expansion!
	    ತಾತ್ಕಾಲಿಕ_ಫಲಿತಾಂಶ = make_expanded_name(retval, ಅಭಿವ್ಯಕ್ತಿ_ಆರಂಭ,
						       ಅಭಿವ್ಯಕ್ತಿ_ಕೊನೆ, ತಾತ್ಕಾಲಿಕ_ಫಲಿತಾಂಶ);
	    vim_free(retval);
	    retval = ತಾತ್ಕಾಲಿಕ_ಫಲಿತಾಂಶ;
	}
    }

    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Return TRUE if character "c" can be used in a variable or function name.
 * Does not include '{' or '}' for magic braces.
 */
    ಪೂರ್ಣಾಂಕ
eval_isnamec(ಪೂರ್ಣಾಂಕ c)
{
    ಹಿಂತಿರುಗಿ ASCII_ISALNUM(c) || c == '_' || c == ':' || c == AUTOLOAD_CHAR;
}

/*
 * Return TRUE if character "c" can be used as the first character in a
 * variable or function name (excluding '{' and '}').
 */
    ಪೂರ್ಣಾಂಕ
eval_isnamec1(ಪೂರ್ಣಾಂಕ c)
{
    ಹಿಂತಿರುಗಿ ASCII_ISALPHA(c) || c == '_';
}

/*
 * Return TRUE if character "c" can be used as the first character of a
 * dictionary key.
 */
    ಪೂರ್ಣಾಂಕ
eval_isdictc(ಪೂರ್ಣಾಂಕ c)
{
    ಹಿಂತಿರುಗಿ ASCII_ISALNUM(c) || c == '_';
}

/*
 * Return true when "p" points to "->" followed by a method name or a lambda.
 * Only called for Vim9 script.
 */
    ಸ್ಥಿರ bool
ವಿಧಾನ_call_follows(char_u *p)
{
    ಆದರೆ (p[0] != '-' || p[1] != '>')
	ಹಿಂತಿರುಗಿ false;

    char_u	*after = skipwhite(p + 2);

    ಹಿಂತಿರುಗಿ ASCII_ISALPHA(*after) || *after == '(';
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
    ಪೂರ್ಣಾಂಕ
handle_subscript(
    char_u	**arg,
    char_u	*name_ಆರಂಭ,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    ಪೂರ್ಣಾಂಕ		verbose)	// give error messages
{
    ಪೂರ್ಣಾಂಕ		evaluate = evalarg != ಖಾಲಿ
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    ಪೂರ್ಣಾಂಕ		ret = OK;
    dict_T	*selfdict = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		check_white = TRUE;
    ಪೂರ್ಣಾಂಕ		getnext;
    char_u	*p;

    ಆಗ (ret == OK)
    {
	// When at the end of the line and ".name" or a method call follows in
	// the next line then consume the line break.
	p = eval_ಮುಂದಿನ_non_blank(*arg, evalarg, &getnext);
	ಆದರೆ (getnext
	    && ((*p == '.'
		    && ((rettv->v_type == VAR_DICT && eval_isdictc(p[1]))
			|| rettv->v_type == VAR_CLASS
			|| rettv->v_type == VAR_OBJECT))
		|| ವಿಧಾನ_call_follows(p)))
	{
	    *arg = eval_ಮುಂದಿನ_ಸಾಲು(*arg, evalarg);
	    p = *arg;
	    check_white = FALSE;
	}

	ಆದರೆ (rettv->v_type == VAR_ANY)
	{
	    char_u	*exp_name;
	    ಪೂರ್ಣಾಂಕ		cc;
	    ಪೂರ್ಣಾಂಕ		idx;
	    ufunc_T	*ufunc;
	    type_T	*type;

	    // Found script from "import {name} as name", script item name must
	    // follow.  "rettv->vval.v_number" has the script ID.
	    ಆದರೆ (**arg != '.')
	    {
		ಆದರೆ (verbose)
		    semsg(_(e_expected_dot_after_name_str),
					name_ಆರಂಭ != ಖಾಲಿ ? name_ಆರಂಭ: *arg);
		ret = FAIL;
		ನಿಲ್ಲಿಸು;
	    }
	    ++*arg;
	    ಆದರೆ (IS_WHITE_OR_NUL(**arg))
	    {
		ಆದರೆ (verbose)
		    emsg(_(e_no_white_space_allowed_after_dot));
		ret = FAIL;
		ನಿಲ್ಲಿಸು;
	    }

	    // isolate the name
	    exp_name = *arg;
	    ಆಗ (eval_isnamec(**arg))
		++*arg;
	    cc = **arg;
	    **arg = NUL;

	    idx = ಕಂಡುಹಿಡಿ_exported(rettv->vval.v_ಸಂಖ್ಯೆ, exp_name, &ufunc, &type,
		       evalarg == ಖಾಲಿ ? ಖಾಲಿ : evalarg->eval_cctx,
		       evalarg == ಖಾಲಿ ? ಖಾಲಿ : evalarg->eval_cstack, verbose);
	    **arg = cc;

	    ಆದರೆ (idx < 0 && ufunc == ಖಾಲಿ)
	    {
		ret = FAIL;
		ನಿಲ್ಲಿಸು;
	    }
	    ಆದರೆ (idx >= 0)
	    {
		scriptitem_T    *si = SCRIPT_ಅಂಶ(rettv->vval.v_ಸಂಖ್ಯೆ);
		svar_T		*sv = ((svar_T *)si->sn_var_vals.ga_ದತ್ತಾಂಶ) + idx;

		copy_tv(sv->sv_tv, rettv);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		rettv->v_type = VAR_FUNC;
		ಆದರೆ (**arg == '<')
		{
		    char_u *s = get_generic_func_name(ufunc, arg);
		    ಆದರೆ (s != ಖಾಲಿ)
			rettv->vval.v_string = s;
		    ಇಲ್ಲದಿದ್ದರೆ
			ret = FAIL;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    rettv->vval.v_string =
			vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
		}
	    }
	    ಮುಂದುವರಿ;
	}

	ಆದರೆ ((**arg == '(' && (!evaluate || rettv->v_type == VAR_FUNC
			    || rettv->v_type == VAR_PARTIAL))
		    && (!check_white || !VIM_ISWHITE(*(*arg - 1))))
	{
	    ret = call_func_rettv(arg, evalarg, rettv, evaluate,
							       selfdict, ಖಾಲಿ);

	    // Stop the expression evaluation when immediately aborting on
	    // error, or when an interrupt occurred or an exception was thrown
	    // but not caught.
	    ಆದರೆ (aborting())
	    {
		ಆದರೆ (ret == OK)
		    clear_tv(rettv);
		ret = FAIL;
	    }
	    dict_unref(selfdict);
	    selfdict = ಖಾಲಿ;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p[0] == '-' && p[1] == '>')
	{
	    ಆದರೆ (ಒಳ_vim9script())
		*arg = skipwhite(p + 2);
	    ಇಲ್ಲದಿದ್ದರೆ
		*arg = p + 2;
	    ಆದರೆ (ret == OK && evaluate && rettv->v_type == VAR_VOID)
	    {
		ಆದರೆ (verbose)
		    emsg(_(e_cannot_use_void_ಮೌಲ್ಯ));
		ret = FAIL;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (VIM_ISWHITE(**arg))
	    {
		emsg(_(e_no_white_space_allowed_before_parenthesis));
		ret = FAIL;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((**arg == '{' && !ಒಳ_vim9script()) || **arg == '(')
		// expr->{lambda}() or expr->(lambda)()
		ret = eval_lambda(arg, rettv, evalarg, verbose);
	    ಇಲ್ಲದಿದ್ದರೆ
		// expr->name()
		ret = eval_ವಿಧಾನ(arg, rettv, evalarg, verbose);
	}
	// "." is ".name" lookup when we found a dict or when evaluating and
	// scriptversion is at least 2, where string concatenation is "..".
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (**arg == '['
		|| (**arg == '.' && (rettv->v_type == VAR_DICT
			|| (!evaluate
			    && (*arg)[1] != '.'
			    && !ಒಳ_old_script(2)))))
	{
	    dict_unref(selfdict);
	    ಆದರೆ (rettv->v_type == VAR_DICT)
	    {
		selfdict = rettv->vval.v_dict;
		ಆದರೆ (selfdict != ಖಾಲಿ)
		    ++selfdict->dv_refcount;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		selfdict = ಖಾಲಿ;
	    ಆದರೆ (eval_ಸೂಚ್ಯಂಕ(arg, rettv, evalarg, verbose) == FAIL)
	    {
		clear_tv(rettv);
		ret = FAIL;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (**arg == '.' && (rettv->v_type == VAR_CLASS
					       || rettv->v_type == VAR_OBJECT))
	{
	    // class member: SomeClass.varname
	    // class method: SomeClass.SomeMethod()
	    // class constructor: SomeClass.new()
	    // object member: someObject.varname
	    // object method: someObject.SomeMethod()
	    ಆದರೆ (class_object_ಸೂಚ್ಯಂಕ(arg, rettv, evalarg, verbose) == FAIL)
	    {
		clear_tv(rettv);
		ret = FAIL;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ನಿಲ್ಲಿಸು;
    }

    // Turn "dict.Func" into a partial for "Func" bound to "dict".
    // Don't do this when "Func" is already a partial that was bound
    // explicitly (pt_auto is FALSE).
    ಆದರೆ (selfdict != ಖಾಲಿ
	    && (rettv->v_type == VAR_FUNC
		|| (rettv->v_type == VAR_PARTIAL
		    && (rettv->vval.v_partial->pt_auto
			|| rettv->vval.v_partial->pt_dict == ಖಾಲಿ))))
	selfdict = make_partial(selfdict, rettv);

    dict_unref(selfdict);
    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Make a copy of an item.
 * Lists and Dictionaries are also copied.  A deep copy if "deep" is set.
 * "top" is TRUE for the toplevel of copy().
 * For deepcopy() "copyID" is zero for a full copy or the ID for when a
 * reference to an already copied list/dict can be used.
 * Returns FAIL or OK.
 */
    ಪೂರ್ಣಾಂಕ
ಅಂಶ_copy(
    typval_T	*from,
    typval_T	*ಗಮ್ಯ,
    ಪೂರ್ಣಾಂಕ		deep,
    ಪೂರ್ಣಾಂಕ		ತುದಿ,
    ಪೂರ್ಣಾಂಕ		copyID)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	recurse = 0;
    ಪೂರ್ಣಾಂಕ		ret = OK;

    ಆದರೆ (recurse >= DICT_MAXNEST)
    {
	emsg(_(e_variable_nested_too_deep_for_making_copy));
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ++recurse;

    ಆಯ್ಕೆ (from->v_type)
    {
	ಸ್ಥಿತಿ VAR_ಸಂಖ್ಯೆ:
	ಸ್ಥಿತಿ VAR_FLOAT:
	ಸ್ಥಿತಿ VAR_STRING:
	ಸ್ಥಿತಿ VAR_FUNC:
	ಸ್ಥಿತಿ VAR_PARTIAL:
	ಸ್ಥಿತಿ VAR_BOOL:
	ಸ್ಥಿತಿ VAR_SPECIAL:
	ಸ್ಥಿತಿ VAR_ಕೆಲಸ:
	ಸ್ಥಿತಿ VAR_CHANNEL:
	ಸ್ಥಿತಿ VAR_INSTR:
	ಸ್ಥಿತಿ VAR_CLASS:
	ಸ್ಥಿತಿ VAR_OBJECT:
	ಸ್ಥಿತಿ VAR_TYPEALIAS:
	    copy_tv(from, ಗಮ್ಯ);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_LIST:
	    ಗಮ್ಯ->v_type = VAR_LIST;
	    ಗಮ್ಯ->v_ಬೀಗ = 0;
	    ಆದರೆ (from->vval.v_list == ಖಾಲಿ)
		ಗಮ್ಯ->vval.v_list = ಖಾಲಿ;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (copyID != 0 && from->vval.v_list->lv_copyID == copyID)
	    {
		// use the copy made earlier
		ಗಮ್ಯ->vval.v_list = from->vval.v_list->lv_copylist;
		++ಗಮ್ಯ->vval.v_list->lv_refcount;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಗಮ್ಯ->vval.v_list = list_copy(from->vval.v_list,
							    deep, ತುದಿ, copyID);
		ಆದರೆ (ಗಮ್ಯ->vval.v_list == ಖಾಲಿ)
		    ret = FAIL;
	    }
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_TUPLE:
	    ಗಮ್ಯ->v_type = VAR_TUPLE;
	    ಗಮ್ಯ->v_ಬೀಗ = 0;
	    ಆದರೆ (from->vval.v_tuple == ಖಾಲಿ)
		ಗಮ್ಯ->vval.v_tuple = ಖಾಲಿ;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (copyID != 0 && from->vval.v_tuple->tv_copyID == copyID)
	    {
		// use the copy made earlier
		ಗಮ್ಯ->vval.v_tuple = from->vval.v_tuple->tv_copytuple;
		++ಗಮ್ಯ->vval.v_tuple->tv_refcount;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಗಮ್ಯ->vval.v_tuple = tuple_copy(from->vval.v_tuple,
							    deep, ತುದಿ, copyID);
		ಆದರೆ (ಗಮ್ಯ->vval.v_tuple == ಖಾಲಿ)
		    ret = FAIL;
	    }
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_BLOB:
	    ಗಮ್ಯ->v_type = VAR_BLOB;
	    ಗಮ್ಯ->v_ಬೀಗ = 0;
	    ಆದರೆ (from->vval.v_blob == ಖಾಲಿ)
		ಗಮ್ಯ->vval.v_blob = ಖಾಲಿ;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಗಮ್ಯ->vval.v_blob = blob_copy(from->vval.v_blob);
		ಆದರೆ (ಗಮ್ಯ->vval.v_blob == ಖಾಲಿ)
		    ret = FAIL;
	    }
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_DICT:
	    ಗಮ್ಯ->v_type = VAR_DICT;
	    ಗಮ್ಯ->v_ಬೀಗ = 0;
	    ಆದರೆ (from->vval.v_dict == ಖಾಲಿ)
		ಗಮ್ಯ->vval.v_dict = ಖಾಲಿ;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (copyID != 0 && from->vval.v_dict->dv_copyID == copyID)
	    {
		// use the copy made earlier
		ಗಮ್ಯ->vval.v_dict = from->vval.v_dict->dv_copydict;
		++ಗಮ್ಯ->vval.v_dict->dv_refcount;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಗಮ್ಯ->vval.v_dict = dict_copy(from->vval.v_dict,
							    deep, ತುದಿ, copyID);
		ಆದರೆ (ಗಮ್ಯ->vval.v_dict == ಖಾಲಿ)
		    ret = FAIL;
	    }
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_UNKNOWN:
	ಸ್ಥಿತಿ VAR_ANY:
	ಸ್ಥಿತಿ VAR_VOID:
	    ಆಂತರಿಕ_error_no_abort("item_copy(UNKNOWN)");
	    ret = FAIL;
    }
    --recurse;
    ಹಿಂತಿರುಗಿ ret;
}

    ಶೂನ್ಯ
echo_one(typval_T *rettv, ಪೂರ್ಣಾಂಕ with_space, ಪೂರ್ಣಾಂಕ *atstart, ಪೂರ್ಣಾಂಕ *needclr)
{
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    char_u	*p = echo_string(rettv, &tofree, numbuf, get_copyID());

    ಆದರೆ (*atstart)
    {
	*atstart = FALSE;
	// Call msg_start() after eval1(), evaluating the expression
	// may cause a message to appear.
	ಆದರೆ (with_space)
	{
	    // Mark the saved text as finishing the line, so that what
	    // follows is displayed on a new line when scrolling back
	    // at the more prompt.
	    ಸಂದೇಶ_sb_eol();
	    ಸಂದೇಶ_ಆರಂಭ();
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (with_space)
	ಸಂದೇಶ_puts_attr(" ", echo_attr);

    ಆದರೆ (p != ಖಾಲಿ)
	ಗಾಗಿ ( ; *p != NUL && !got_int; ++p)
	{
	    ಆದರೆ (*p == '\n' || *p == '\r' || *p == TAB)
	    {
		ಆದರೆ (*p != TAB && *needclr)
		{
		    // remove any text still there from the command
		    ಸಂದೇಶ_clr_eos();
		    *needclr = FALSE;
		}
		ಸಂದೇಶ_putchar_attr(*p, echo_attr);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ (has_mbyte)
		{
		    ಪೂರ್ಣಾಂಕ i = (*mb_ptr2len)(p);

		    (ಶೂನ್ಯ)ಸಂದೇಶ_outtrans_len_attr(p, i, echo_attr);
		    p += i - 1;
		}
		ಇಲ್ಲದಿದ್ದರೆ
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
    char_u	*arg_ಆರಂಭ;
    ಪೂರ್ಣಾಂಕ		needclr = TRUE;
    ಪೂರ್ಣಾಂಕ		atstart = TRUE;
    ಪೂರ್ಣಾಂಕ		did_emsg_before = did_emsg;
    ಪೂರ್ಣಾಂಕ		called_emsg_before = called_emsg;
    evalarg_T	evalarg;

    fill_evalarg_from_eap(&evalarg, eap, eap->skip);

    ಆದರೆ (eap->skip)
	++emsg_skip;
    ಆಗ ((!ತುದಿಗಳು_excmd2(eap->cmd, arg) || *arg == '"') && !got_int)
    {
	// If eval1() causes an error message the text from the command may
	// still need to be cleared. E.g., "echo 22,44".
	need_clr_eos = needclr;

	arg_ಆರಂಭ = arg;
	ಆದರೆ (eval1(&arg, &rettv, &evalarg) == FAIL)
	{
	    /*
	     * Report the invalid expression unless the expression evaluation
	     * has been cancelled due to an aborting error, an interrupt, or an
	     * exception.
	     */
	    ಆದರೆ (!aborting() && did_emsg == did_emsg_before
					  && called_emsg == called_emsg_before)
		semsg(_(e_invalid_ಅಭಿವ್ಯಕ್ತಿ_str), arg_ಆರಂಭ);
	    need_clr_eos = FALSE;
	    ನಿಲ್ಲಿಸು;
	}
	need_clr_eos = FALSE;

	ಆದರೆ (!eap->skip)
	{
	    ಆದರೆ (rettv.v_type == VAR_VOID)
	    {
		semsg(_(e_ಅಭಿವ್ಯಕ್ತಿ_does_not_ಫಲಿತಾಂಶ_ಒಳ_ಮೌಲ್ಯ_str), arg_ಆರಂಭ);
		ನಿಲ್ಲಿಸು;
	    }
	    echo_one(&rettv, eap->cmdidx == CMD_echo, &atstart, &needclr);
	}

	clear_tv(&rettv);
	arg = skipwhite(arg);
    }
    ಗಣ_nextcmd(eap, arg);
    clear_evalarg(&evalarg, eap);

    ಆದರೆ (eap->skip)
	--emsg_skip;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// remove text that may still be there from the command
	ಆದರೆ (needclr)
	    ಸಂದೇಶ_clr_eos();
	ಆದರೆ (eap->cmdidx == CMD_echo)
	    ಸಂದೇಶ_ಕೊನೆ();
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
    ಪೂರ್ಣಾಂಕ
get_echo_attr(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ echo_attr;
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
    ಪೂರ್ಣಾಂಕ		ret = OK;
    char_u	*p;
    garray_T	ga;
    ಪೂರ್ಣಾಂಕ		len;
    ದೀರ್ಘ	ಆರಂಭ_lnum = SOURCING_LNUM;

    ga_init2(&ga, 1, 80);

    ಆದರೆ (eap->skip)
	++emsg_skip;
    ಆಗ (!ತುದಿಗಳು_excmd2(eap->cmd, arg) || *arg == '"')
    {
	ret = eval1_emsg(&arg, &rettv, eap);
	ಆದರೆ (ret == FAIL)
	    ನಿಲ್ಲಿಸು;

	ಆದರೆ (!eap->skip)
	{
	    char_u   buf[NUMBUFLEN];

	    ಆದರೆ (eap->cmdidx == CMD_execute)
	    {
		ಆದರೆ (rettv.v_type == VAR_CHANNEL || rettv.v_type == VAR_ಕೆಲಸ)
		{
		    semsg(_(e_using_invalid_ಮೌಲ್ಯ_as_string_str),
						  vartype_name(rettv.v_type));
		    p = ಖಾಲಿ;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    p = tv_get_string_buf(&rettv, buf);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		p = tv_stringify(&rettv, buf);
	    ಆದರೆ (p == ಖಾಲಿ)
	    {
		clear_tv(&rettv);
		ret = FAIL;
		ನಿಲ್ಲಿಸು;
	    }
	    len = (ಪೂರ್ಣಾಂಕ)STRLEN(p);
	    ಆದರೆ (ga_grow(&ga, len + 2) == FAIL)
	    {
		clear_tv(&rettv);
		ret = FAIL;
		ನಿಲ್ಲಿಸು;
	    }
	    ಆದರೆ (ga.ga_len)
		((char_u *)(ga.ga_ದತ್ತಾಂಶ))[ga.ga_len++] = ' ';
	    STRCPY((char_u *)(ga.ga_ದತ್ತಾಂಶ) + ga.ga_len, p);
	    ga.ga_len += len;
	}

	clear_tv(&rettv);
	arg = skipwhite(arg);
    }

    ಆದರೆ (ret != FAIL && ga.ga_ದತ್ತಾಂಶ != ಖಾಲಿ)
    {
	// use the first line of continuation lines for messages
	SOURCING_LNUM = ಆರಂಭ_lnum;

	ಆದರೆ (eap->cmdidx == CMD_echomsg
		|| eap->cmdidx == CMD_echowindow
		|| eap->cmdidx == CMD_echoerr)
	{
	    // Mark the already saved text as finishing the line, so that what
	    // follows is displayed on a new line when scrolling back at the
	    // more prompt.
	    ಸಂದೇಶ_sb_eol();
	}

	ಆದರೆ (eap->cmdidx == CMD_echomsg)
	{
	    ಸಂದೇಶ_attr(ga.ga_ದತ್ತಾಂಶ, echo_attr);
	    ಹೊರಹೋಗು_flush();
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (eap->cmdidx == CMD_echowindow)
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAS_ಸಂದೇಶ_ಕಿಟಕಿ
	    ಆರಂಭ_echowindow(eap->addr_ಎಣಿಕೆ > 0 ? eap->line2 : 0);
#ಆದರೆ_ಅಂತ್ಯ
	    ಸಂದೇಶ_attr(ga.ga_ದತ್ತಾಂಶ, echo_attr);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAS_ಸಂದೇಶ_ಕಿಟಕಿ
	    ಕೊನೆ_echowindow();
#ಆದರೆ_ಅಂತ್ಯ
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (eap->cmdidx == CMD_echoconsole)
	{
	    ui_write(ga.ga_ದತ್ತಾಂಶ, (ಪೂರ್ಣಾಂಕ)STRLEN(ga.ga_ದತ್ತಾಂಶ), TRUE);
	    ui_write((char_u *)"\r\n", 2, TRUE);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (eap->cmdidx == CMD_echoerr)
	{
	    ಪೂರ್ಣಾಂಕ		save_did_emsg = did_emsg;

	    // We don't want to abort following commands, restore did_emsg.
	    emsg(ga.ga_ದತ್ತಾಂಶ);
	    ಆದರೆ (!force_abort)
		did_emsg = save_did_emsg;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (eap->cmdidx == CMD_execute)
	{
	    ಪೂರ್ಣಾಂಕ save_sticky_cmdmod_flags = sticky_cmdmod_flags;

	    // "legacy exe cmd" and "vim9cmd exe cmd" applies to "cmd".
	    sticky_cmdmod_flags = cmdmod.cmod_flags
						& (CMOD_LEGACY | CMOD_VIM9CMD);
	    do_cmdline((char_u *)ga.ga_ದತ್ತಾಂಶ,
		       eap->ea_getline, eap->cookie, DOCMD_NOWAIT|DOCMD_VERBOSE);
	    sticky_cmdmod_flags = save_sticky_cmdmod_flags;
	}
    }

    ga_clear(&ga);

    ಆದರೆ (eap->skip)
	--emsg_skip;
    ಗಣ_nextcmd(eap, arg);
}

/*
 * Skip over the name of an option: "&option", "&g:option" or "&l:option".
 * "arg" points to the "&" or '+' when called, to "option" when returning.
 * Returns NULL when no option name found.  Otherwise pointer to the char
 * after the option name.
 */
    char_u *
ಕಂಡುಹಿಡಿ_option_ಕೊನೆ(char_u **arg, ಪೂರ್ಣಾಂಕ *scope)
{
    char_u	*p = *arg;

    ++p;
    ಆದರೆ (*p == 'g' && p[1] == ':')
    {
	*scope = OPT_GLOBAL;
	p += 2;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == 'l' && p[1] == ':')
    {
	*scope = OPT_LOCAL;
	p += 2;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	*scope = 0;

    ಆದರೆ (!ASCII_ISALPHA(*p))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    *arg = p;

    ಆದರೆ (p[0] == 't' && p[1] == '_' && p[2] != NUL && p[3] != NUL)
	p += 4;	    // termcap option
    ಇಲ್ಲದಿದ್ದರೆ
	ಆಗ (ASCII_ISALPHA(*p))
	    ++p;
    ಹಿಂತಿರುಗಿ p;
}

/*
 * Display script name where an item was last set.
 * Should only be invoked when 'verbose' is non-zero.
 */
    ಶೂನ್ಯ
ಕೊನೆಯ_ಗಣ_ಸಂದೇಶ(sctx_T script_ctx)
{
    char_u *p;

    ಆದರೆ (script_ctx.sc_sid == 0)
	ಹಿಂತಿರುಗಿ;

    p = home_replace_save(ಖಾಲಿ, get_scriptname(script_ctx.sc_sid));
    ಆದರೆ (p == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    verbose_enter();
    ಸಂದೇಶ_puts(_("\n\tLast set from "));
    ಸಂದೇಶ_puts((ಅಕ್ಷರ *)p);
    ಆದರೆ (script_ctx.sc_lnum > 0)
    {
	ಸಂದೇಶ_puts(_(ಸಾಲು_ಸಂದೇಶ));
	ಸಂದೇಶ_outnum((ದೀರ್ಘ)script_ctx.sc_lnum);
    }
    verbose_leave();
    vim_free(p);
}

#ಆದರೆ_ಅಂತ್ಯ // FEAT_EVAL

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
    p_cpo = ಖಾಲಿಗಳು_option;

    ga_init2(&ga, 1, 200);

    regmatch.rm_ic = p_ic;
    regmatch.regprog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
    ಆದರೆ (regmatch.regprog != ಖಾಲಿ)
    {
	char_u	*ಬಾಲ = str;
	char_u	*ಕೊನೆ = str + len;
	ಪೂರ್ಣಾಂಕ	do_all = (flags[0] == 'g');
	ಪೂರ್ಣಾಂಕ	sublen;
	ಪೂರ್ಣಾಂಕ	i;
	char_u	*zero_ಅಗಲ = ಖಾಲಿ;

	ಆಗ (vim_regexec_nl(&regmatch, str, (colnr_T)(ಬಾಲ - str)))
	{
	    // Skip empty match except for first match.
	    ಆದರೆ (regmatch.startp[0] == regmatch.endp[0])
	    {
		ಆದರೆ (zero_ಅಗಲ == regmatch.startp[0])
		{
		    // avoid getting stuck on a match with an empty string
		    i = mb_ptr2len(ಬಾಲ);
		    mch_memmove((char_u *)ga.ga_ದತ್ತಾಂಶ + ga.ga_len, ಬಾಲ,
								   (size_t)i);
		    ga.ga_len += i;
		    ಬಾಲ += i;
		    ಮುಂದುವರಿ;
		}
		zero_ಅಗಲ = regmatch.startp[0];
	    }

	    /*
	     * Get some space for a temporary buffer to do the substitution
	     * into.  It will contain:
	     * - The text up to where the match is.
	     * - The substituted text.
	     * - The text after the match.
	     */
	    sublen = vim_regsub(&regmatch, sub, ಅಭಿವ್ಯಕ್ತಿ, ಬಾಲ, 0, REGSUB_MAGIC);
	    ಆದರೆ (sublen <= 0)
	    {
		ga_clear(&ga);
		ನಿಲ್ಲಿಸು;
	    }
	    ಆದರೆ (ga_grow(&ga, (ಪೂರ್ಣಾಂಕ)((ಕೊನೆ - ಬಾಲ) + sublen -
			    (regmatch.endp[0] - regmatch.startp[0]))) == FAIL)
	    {
		ga_clear(&ga);
		ನಿಲ್ಲಿಸು;
	    }

	    // copy the text up to where the match is
	    i = (ಪೂರ್ಣಾಂಕ)(regmatch.startp[0] - ಬಾಲ);
	    mch_memmove((char_u *)ga.ga_ದತ್ತಾಂಶ + ga.ga_len, ಬಾಲ, (size_t)i);
	    // add the substituted text
	    (ಶೂನ್ಯ)vim_regsub(&regmatch, sub, ಅಭಿವ್ಯಕ್ತಿ,
				  (char_u *)ga.ga_ದತ್ತಾಂಶ + ga.ga_len + i, sublen,
				  REGSUB_COPY | REGSUB_MAGIC);
	    ga.ga_len += i + sublen - 1;
	    ಬಾಲ = regmatch.endp[0];
	    ಆದರೆ (*ಬಾಲ == NUL)
		ನಿಲ್ಲಿಸು;
	    ಆದರೆ (!do_all)
		ನಿಲ್ಲಿಸು;
	}

	ಆದರೆ (ga.ga_ದತ್ತಾಂಶ != ಖಾಲಿ)
	{
	    STRCPY((ಅಕ್ಷರ *)ga.ga_ದತ್ತಾಂಶ + ga.ga_len, ಬಾಲ);
	    ga.ga_len += (ಪೂರ್ಣಾಂಕ)(ಕೊನೆ - ಬಾಲ);
	}

	vim_regfree(regmatch.regprog);
    }

    ಆದರೆ (ga.ga_ದತ್ತಾಂಶ != ಖಾಲಿ)
    {
	str = (char_u *)ga.ga_ದತ್ತಾಂಶ;
	len = (size_t)ga.ga_len;
    }
    ret = vim_strnsave(str, len);
    ga_clear(&ga);
    ಆದರೆ (p_cpo == ಖಾಲಿಗಳು_option)
	p_cpo = save_cpo;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Darn, evaluating {sub} expression or {expr} changed the value.
	// If it's still empty it was changed and restored, need to restore in
	// the complicated way.
	ಆದರೆ (*p_cpo == NUL)
	    ಗಣ_option_ಮೌಲ್ಯ_give_err((char_u *)"cpo", 0L, save_cpo, 0);
	free_string_option(save_cpo);
    }

    ಆದರೆ (ret_len != ಖಾಲಿ)
	*ret_len = len;

    ಹಿಂತಿರುಗಿ ret;
}
