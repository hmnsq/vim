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

#include "vim.h"

#නම් defined(FEAT_EVAL)

#ifdef VMS
# include <float.h>
#endif

#define NAMESPACE_CHAR	(char_u *)"abglstvw"

static පූර්ණ_සංඛ්‍යා eval0_simple_funccal(char_u *arg, typval_T *rettv, exarg_T *eap, evalarg_T *evalarg);
static පූර්ණ_සංඛ්‍යා eval2(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
static පූර්ණ_සංඛ්‍යා eval3(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
static පූර්ණ_සංඛ්‍යා eval4(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
static පූර්ණ_සංඛ්‍යා eval5(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
static පූර්ණ_සංඛ්‍යා eval6(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
static පූර්ණ_සංඛ්‍යා eval7(char_u **arg, typval_T *rettv, evalarg_T *evalarg, පූර්ණ_සංඛ්‍යා want_string);
static පූර්ණ_සංඛ්‍යා eval8(char_u **arg, typval_T *rettv, evalarg_T *evalarg, පූර්ණ_සංඛ්‍යා want_string);
static පූර්ණ_සංඛ්‍යා eval9(char_u **arg, typval_T *rettv, evalarg_T *evalarg, පූර්ණ_සංඛ්‍යා want_string);
static පූර්ණ_සංඛ්‍යා eval9_leader(typval_T *rettv, පූර්ණ_සංඛ්‍යා numeric_only, char_u *start_leader, char_u **end_leaderp);

static char_u *make_expanded_name(char_u *in_start, char_u *expr_start, char_u *expr_end, char_u *in_end);

/*
 * Return "n1" divided by "n2", taking care of dividing by zero.
 * If "failed" is not NULL set it to TRUE when dividing by zero fails.
 */
	varnumber_T
num_divide(varnumber_T n1, varnumber_T n2, පූර්ණ_සංඛ්‍යා *failed)
{
    varnumber_T	result;

    නම් (n2 == 0)
    {
	නම් (in_vim9script())
	{
	    emsg(_(e_divide_by_zero));
	    නම් (failed != NULL)
		*failed = TRUE;
	}
	නම් (n1 == 0)
	    result = VARNUM_MIN; // similar to NaN
	නැතහොත් නම් (n1 < 0)
	    result = -VARNUM_MAX;
	නැතහොත්
	    result = VARNUM_MAX;
    }
    නැතහොත් නම් (n1 == VARNUM_MIN && n2 == -1)
    {
	// specific case: trying to do VARNUM_MIN / -1 results in a positive
	// number that doesn't fit in varnumber_T and causes an FPE
	result = VARNUM_MAX;
    }
    නැතහොත්
	result = n1 / n2;

    ආපසු result;
}

/*
 * Return "n1" modulus "n2", taking care of dividing by zero.
 * If "failed" is not NULL set it to TRUE when dividing by zero fails.
 */
	varnumber_T
num_modulus(varnumber_T n1, varnumber_T n2, පූර්ණ_සංඛ්‍යා *failed)
{
    නම් (n2 == 0 && in_vim9script())
    {
	emsg(_(e_divide_by_zero));
	නම් (failed != NULL)
	    *failed = TRUE;
    }
    ආපසු (n2 == 0) ? 0 : (n1 % n2);
}

/*
 * Initialize the global and v: variables.
 */
    void
eval_init(void)
{
    evalvars_init();
    func_init();
}

#නම් defined(EXITFREE)
    void
eval_clear(void)
{
    evalvars_clear();
    free_scriptnames();  // must come after evalvars_clear().
    free_locales();

    // autoloaded script names
    free_autoload_scriptnames();

    // unreferenced lists, tuples and dicts
    (void)garbage_collect(FALSE);

    // functions not garbage collected
    free_all_functions();
}
#endif

    void
fill_evalarg_from_eap(evalarg_T *evalarg, exarg_T *eap, පූර්ණ_සංඛ්‍යා skip)
{
    init_evalarg(evalarg);
    evalarg->eval_flags = skip ? 0 : EVAL_EVALUATE;

    නම් (eap == NULL)
	ආපසු;

    evalarg->eval_cstack = eap->cstack;
    නම් (sourcing_a_script(eap) || eap->ea_getline == get_list_line)
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
    පූර්ණ_සංඛ්‍යා
eval_to_bool(
    char_u	*arg,
    පූර්ණ_සංඛ්‍යා		*error,
    exarg_T	*eap,
    පූර්ණ_සංඛ්‍යා		skip,	    // only parse, don't execute
    පූර්ණ_සංඛ්‍යා		use_simple_function)
{
    typval_T	tv;
    varnumber_T	retval = FALSE;
    evalarg_T	evalarg;
    පූර්ණ_සංඛ්‍යා		r;

    fill_evalarg_from_eap(&evalarg, eap, skip);

    නම් (skip)
	++emsg_skip;
    නම් (use_simple_function)
	r = eval0_simple_funccal(arg, &tv, eap, &evalarg);
    නැතහොත්
	r = eval0(arg, &tv, eap, &evalarg);
    නම් (r == FAIL)
	*error = TRUE;
    නැතහොත්
    {
	*error = FALSE;
	නම් (!skip)
	{
	    නම් (in_vim9script())
		retval = tv_get_bool_chk(&tv, error);
	    නැතහොත්
		retval = (tv_get_number_chk(&tv, error) != 0);
	    clear_tv(&tv);
	}
    }
    නම් (skip)
	--emsg_skip;
    clear_evalarg(&evalarg, eap);

    ආපසු (පූර්ණ_සංඛ්‍යා)retval;
}

/*
 * Call eval1() and give an error message if not done at a lower level.
 */
    static පූර්ණ_සංඛ්‍යා
eval1_emsg(char_u **arg, typval_T *rettv, exarg_T *eap)
{
    char_u	*start = *arg;
    පූර්ණ_සංඛ්‍යා		ret;
    පූර්ණ_සංඛ්‍යා		did_emsg_before = did_emsg;
    පූර්ණ_සංඛ්‍යා		called_emsg_before = called_emsg;
    evalarg_T	evalarg;

    fill_evalarg_from_eap(&evalarg, eap, eap != NULL && eap->skip);

    ret = eval1(arg, rettv, &evalarg);
    නම් (ret == FAIL)
    {
	// Report the invalid expression unless the expression evaluation has
	// been cancelled due to an aborting error, an interrupt, or an
	// exception, or we already gave a more specific error.
	// Also check called_emsg for when using assert_fails().
	නම් (!aborting() && did_emsg == did_emsg_before
					  && called_emsg == called_emsg_before)
	    semsg(_(e_invalid_expression_str), start);
    }
    clear_evalarg(&evalarg, eap);
    ආපසු ret;
}

/*
 * Return whether a typval is a valid expression to pass to eval_expr_typval()
 * or eval_expr_to_bool().  An empty string returns FALSE;
 */
    පූර්ණ_සංඛ්‍යා
eval_expr_valid_arg(typval_T *tv)
{
    ආපසු tv->v_type != VAR_UNKNOWN
	    && (tv->v_type != VAR_STRING
		  || (tv->vval.v_string != NULL && *tv->vval.v_string != NUL));
}

/*
 * When calling eval_expr_typval() many times we only need one funccall_T.
 * Returns NULL when no funccall_T is to be used.
 * When returning non-NULL remove_funccal() must be called later.
 */
    funccall_T *
eval_expr_get_funccal(typval_T *expr, typval_T *rettv)
{
    නම් (expr->v_type != VAR_PARTIAL)
	ආපසු NULL;

    partial_T *partial = expr->vval.v_partial;
    නම් (partial == NULL)
	ආපසු NULL;
    නම් (partial->pt_func == NULL
	    || partial->pt_func->uf_def_status == UF_NOT_COMPILED)
	ආපසු NULL;

    ආපසු create_funccal(partial->pt_func, rettv);
}

/*
 * Evaluate a partial.
 * Pass arguments "argv[argc]".
 * "fc_arg" is from eval_expr_get_funccal() or NULL;
 * Return the result in "rettv" and OK or FAIL.
 */
    static පූර්ණ_සංඛ්‍යා
eval_expr_partial(
    typval_T	*expr,
    typval_T	*argv,
    පූර්ණ_සංඛ්‍යා		argc,
    funccall_T	*fc_arg,
    typval_T	*rettv)
{
    partial_T	*partial = expr->vval.v_partial;

    නම් (partial == NULL)
	ආපසු FAIL;

    නම් (partial->pt_func != NULL
			&& partial->pt_func->uf_def_status != UF_NOT_COMPILED)
    {
	funccall_T	*fc = fc_arg != NULL ? fc_arg
				: create_funccal(partial->pt_func, rettv);
	පූර්ණ_සංඛ්‍යා		r;

	නම් (fc == NULL)
	    ආපසු FAIL;

	// Shortcut to call a compiled function with minimal overhead.
	නම් (partial->pt_obj != NULL)
	    partial->pt_obj->obj_refcount++;
	r = call_def_function(partial->pt_func, argc, argv, DEF_USE_PT_ARGV,
					partial, partial->pt_obj, fc, rettv);
	නම් (fc_arg == NULL)
	    remove_funccal();
	නම් (r == FAIL)
	    ආපසු FAIL;
    }
    නැතහොත්
    {
	char_u		*s = partial_name(partial);
	funcexe_T	funcexe;

	නම් (s == NULL || *s == NUL)
	    ආපසු FAIL;

	CLEAR_FIELD(funcexe);
	funcexe.fe_evaluate = TRUE;
	funcexe.fe_partial = partial;
	නම් (call_func(s, -1, rettv, argc, argv, &funcexe) == FAIL)
	    ආපසු FAIL;
    }

    ආපසු OK;
}

/*
 * Evaluate an expression which is a function.
 * Pass arguments "argv[argc]".
 * Return the result in "rettv" and OK or FAIL.
 */
    static පූර්ණ_සංඛ්‍යා
eval_expr_func(
    typval_T	*expr,
    typval_T	*argv,
    පූර්ණ_සංඛ්‍යා		argc,
    typval_T	*rettv)
{
    funcexe_T	funcexe;
    char_u	buf[NUMBUFLEN];
    char_u	*s;

    නම් (expr->v_type == VAR_FUNC)
	s = expr->vval.v_string;
    නැතහොත්
	s = tv_get_string_buf_chk_strict(expr, buf, in_vim9script());
    නම් (s == NULL || *s == NUL)
	ආපසු FAIL;

    CLEAR_FIELD(funcexe);
    funcexe.fe_evaluate = TRUE;
    නම් (call_func(s, -1, rettv, argc, argv, &funcexe) == FAIL)
	ආපසු FAIL;

    ආපසු OK;
}

/*
 * Evaluate an expression, which is a string.
 * Return the result in "rettv" and OK or FAIL.
 */
    static පූර්ණ_සංඛ්‍යා
eval_expr_string(
    typval_T	*expr,
    typval_T	*rettv)
{
    char_u	*s;
    char_u	buf[NUMBUFLEN];

    s = tv_get_string_buf_chk_strict(expr, buf, in_vim9script());
    නම් (s == NULL)
	ආපසු FAIL;

    s = skipwhite(s);
    නම් (eval1_emsg(&s, rettv, NULL) == FAIL)
	ආපසු FAIL;

    නම් (*skipwhite(s) != NUL)  // check for trailing chars after expr
    {
	clear_tv(rettv);
	semsg(_(e_invalid_expression_str), s);
	ආපසු FAIL;
    }

    ආපසු OK;
}

/*
 * Evaluate an expression, which can be a function, partial or string.
 * Pass arguments "argv[argc]".
 * If "want_func" is TRUE treat a string as a function name, not an expression.
 * "fc_arg" is from eval_expr_get_funccal() or NULL;
 * Return the result in "rettv" and OK or FAIL.
 */
    පූර්ණ_සංඛ්‍යා
eval_expr_typval(
	typval_T    *expr,
	පූර්ණ_සංඛ්‍යා	    want_func,
	typval_T    *argv,
	පූර්ණ_සංඛ්‍යා	    argc,
	funccall_T  *fc_arg,
	typval_T    *rettv)
{
    නම් (expr->v_type == VAR_PARTIAL)
	ආපසු eval_expr_partial(expr, argv, argc, fc_arg, rettv);
    නම් (expr->v_type == VAR_INSTR)
	ආපසු exe_typval_instr(expr, rettv);
    නම් (expr->v_type == VAR_FUNC || want_func)
	ආපසු eval_expr_func(expr, argv, argc, rettv);

    ආපසු eval_expr_string(expr, rettv);
}

/*
 * Like eval_to_bool() but using a typval_T instead of a string.
 * Works for string, funcref and partial.
 */
    පූර්ණ_සංඛ්‍යා
eval_expr_to_bool(typval_T *expr, පූර්ණ_සංඛ්‍යා *error)
{
    typval_T	rettv;
    පූර්ණ_සංඛ්‍යා		res;

    නම් (eval_expr_typval(expr, FALSE, NULL, 0, NULL, &rettv) == FAIL)
    {
	*error = TRUE;
	ආපසු FALSE;
    }
    res = (tv_get_bool_chk(&rettv, error) != 0);
    clear_tv(&rettv);
    ආපසු res;
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
    පූර්ණ_සංඛ්‍යා		skip)	    // only parse, don't execute
{
    typval_T	tv;
    char_u	*retval;
    evalarg_T	evalarg;

    fill_evalarg_from_eap(&evalarg, eap, skip);
    නම් (skip)
	++emsg_skip;
    නම් (eval0(arg, &tv, eap, &evalarg) == FAIL || skip)
	retval = NULL;
    නැතහොත්
    {
	retval = vim_strsave(tv_get_string(&tv));
	clear_tv(&tv);
    }
    නම් (skip)
	--emsg_skip;
    clear_evalarg(&evalarg, eap);

    ආපසු retval;
}

/*
 * Initialize "evalarg" for use.
 */
    void
init_evalarg(evalarg_T *evalarg)
{
    CLEAR_POINTER(evalarg);
    ga_init2(&evalarg->eval_tofree_ga, parimaa_na(char_u *), 20);
}

/*
 * If "evalarg->eval_tofree" is not NULL free it later.
 * Caller is expected to overwrite "evalarg->eval_tofree" next.
 */
    static void
free_eval_tofree_later(evalarg_T *evalarg)
{
    නම් (evalarg->eval_tofree == NULL)
	ආපසු;

    නම් (ga_grow(&evalarg->eval_tofree_ga, 1) == OK)
	((char_u **)evalarg->eval_tofree_ga.ga_data)
	    [evalarg->eval_tofree_ga.ga_len++]
	    = evalarg->eval_tofree;
    නැතහොත්
	vim_free(evalarg->eval_tofree);
}

/*
 * After using "evalarg" filled from "eap": free the memory.
 */
    void
clear_evalarg(evalarg_T *evalarg, exarg_T *eap)
{
    නම් (evalarg == NULL)
	ආපසු;

    garray_T *etga = &evalarg->eval_tofree_ga;

    නම් (evalarg->eval_tofree != NULL || evalarg->eval_using_cmdline)
    {
	නම් (eap != NULL)
	{
	    // We may need to keep the original command line, e.g. for
	    // ":let" it has the variable names.  But we may also need
	    // the new one, "nextcmd" points into it.  Keep both.
	    vim_free(eap->cmdline_tofree);
	    eap->cmdline_tofree = *eap->cmdlinep;

	    නම් (evalarg->eval_using_cmdline && etga->ga_len > 0)
	    {
		// "nextcmd" points into the last line in eval_tofree_ga,
		// need to keep it around.
		--etga->ga_len;
		*eap->cmdlinep = ((char_u **)etga->ga_data)[etga->ga_len];
		vim_free(evalarg->eval_tofree);
	    }
	    නැතහොත්
		*eap->cmdlinep = evalarg->eval_tofree;
	}
	නැතහොත්
	    vim_free(evalarg->eval_tofree);
	evalarg->eval_tofree = NULL;
    }

    ga_clear_strings(etga);
    VIM_CLEAR(evalarg->eval_tofree_lambda);
}

/*
 * Skip over an expression at "*pp".
 * Return FAIL for an error, OK otherwise.
 */
    පූර්ණ_සංඛ්‍යා
skip_expr(char_u **pp, evalarg_T *evalarg)
{
    typval_T	rettv;

    *pp = skipwhite(*pp);
    ආපසු eval1(pp, &rettv, evalarg);
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
    පූර්ණ_සංඛ්‍යා
skip_expr_concatenate(
	char_u	    **arg,
	char_u	    **start,
	char_u	    **end,
	evalarg_T   *evalarg)
{
    typval_T	rettv;
    පූර්ණ_සංඛ්‍යා		res;
    පූර්ණ_සංඛ්‍යා		vim9script = in_vim9script();
    garray_T    *gap = evalarg == NULL ? NULL : &evalarg->eval_ga;
    garray_T    *freegap = evalarg == NULL ? NULL : &evalarg->eval_freega;
    පූර්ණ_සංඛ්‍යා		save_flags = evalarg == NULL ? 0 : evalarg->eval_flags;
    පූර්ණ_සංඛ්‍යා		evaluate = evalarg == NULL
			       ? FALSE : (evalarg->eval_flags & EVAL_EVALUATE);

    නම් (vim9script && evaluate
	       && (evalarg->eval_cookie != NULL || evalarg->eval_cctx != NULL))
    {
	ga_init2(gap, parimaa_na(char_u *), 10);
	// leave room for "start"
	නම් (ga_grow(gap, 1) == OK)
	    ++gap->ga_len;
	ga_init2(freegap, parimaa_na(char_u *), 10);
    }
    *start = *arg;

    // Don't evaluate the expression.
    නම් (evalarg != NULL)
	evalarg->eval_flags &= ~EVAL_EVALUATE;
    *arg = skipwhite(*arg);
    res = eval1(arg, &rettv, evalarg);
    *end = *arg;
    නම් (evalarg != NULL)
	evalarg->eval_flags = save_flags;

    නම් (vim9script && evaluate
	    && (evalarg->eval_cookie != NULL || evalarg->eval_cctx != NULL))
    {
	නම් (evalarg->eval_ga.ga_len == 1)
	{
	    // just the one line, no need to concatenate
	    ga_clear(gap);
	    gap->ga_itemsize = 0;
	}
	නැතහොත්
	{
	    char_u	    *p;
	    size_t	    endoff = STRLEN(*arg);

	    // Line breaks encountered, concatenate all the lines.
	    *((char_u **)gap->ga_data) = *start;
	    p = ga_concat_strings(gap, " ");

	    // free the lines only when using getsourceline()
	    නම් (evalarg->eval_cookie != NULL)
	    {
		// Do not free the first line, the caller can still use it.
		*((char_u **)gap->ga_data) = NULL;
		// Do not free the last line, "arg" points into it, free it
		// later.  Also free "eval_tofree" later if needed.
		free_eval_tofree_later(evalarg);
		evalarg->eval_tofree =
				    ((char_u **)gap->ga_data)[gap->ga_len - 1];
		((char_u **)gap->ga_data)[gap->ga_len - 1] = NULL;
		ga_clear_strings(gap);
		ga_clear(freegap);
	    }
	    නැතහොත්
	    {
		ga_clear(gap);

		// free lines that were explicitly marked for freeing
		ga_clear_strings(freegap);
	    }

	    gap->ga_itemsize = 0;
	    නම් (p == NULL)
		ආපසු FAIL;
	    *start = p;
	    vim_free(evalarg->eval_tofree_lambda);
	    evalarg->eval_tofree_lambda = p;
	    // Compute "end" relative to the end.
	    *end = *start + STRLEN(*start) - endoff;
	}
    }

    ආපසු res;
}

/*
 * Convert "tv" to a string.
 * When "join_list" is TRUE convert a List or a Tuple into a sequence of lines.
 * Returns an allocated string (NULL when out of memory).
 */
    char_u *
typval2string(typval_T *tv, පූර්ණ_සංඛ්‍යා join_list)
{
    garray_T	ga;
    char_u	*retval = NULL;

    නම් (join_list && (tv->v_type == VAR_LIST || tv->v_type == VAR_TUPLE))
    {
	නම් (tv->v_type == VAR_LIST)
	{
	    ga_init2(&ga, parimaa_na(var_na), 80);
	    නම් (tv->vval.v_list != NULL)
	    {
		list_join(&ga, tv->vval.v_list, (char_u *)"\n", TRUE, FALSE,
									0);
		නම් (tv->vval.v_list->lv_len > 0)
		    ga_append(&ga, NL);
	    }
	    ga_append(&ga, NUL);
	    retval = (char_u *)ga.ga_data;
	}
	නැතහොත්
	{
	    // tuple
	    ga_init2(&ga, parimaa_na(var_na), 80);
	    නම් (tv->vval.v_tuple != NULL)
	    {
		tuple_join(&ga, tv->vval.v_tuple, (char_u *)"\n", TRUE, FALSE,
									0);
		නම් (TUPLE_LEN(tv->vval.v_tuple) > 0)
		    ga_append(&ga, NL);
	    }
	    ga_append(&ga, NUL);
	    retval = (char_u *)ga.ga_data;
	}
    }
    නැතහොත් නම් (tv->v_type == VAR_LIST
	    || tv->v_type == VAR_TUPLE
	    || tv->v_type == VAR_DICT)
    {
	char_u	*tofree;
	char_u	numbuf[NUMBUFLEN];

	retval = tv2string(tv, &tofree, numbuf, 0);
	// Make a copy if we have a value but it's not in allocated memory.
	නම් (retval != NULL && tofree == NULL)
	    retval = vim_strsave(retval);
    }
    නැතහොත්
	retval = vim_strsave(tv_get_string(tv));
    ආපසු retval;
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
    පූර්ණ_සංඛ්‍යා		join_list,
    exarg_T	*eap,
    පූර්ණ_සංඛ්‍යා		use_simple_function)
{
    typval_T	tv;
    char_u	*retval;
    evalarg_T	evalarg;
    පූර්ණ_සංඛ්‍යා		r;

    fill_evalarg_from_eap(&evalarg, eap, eap != NULL && eap->skip);
    නම් (use_simple_function)
	r = eval0_simple_funccal(arg, &tv, NULL, &evalarg);
    නැතහොත්
	r = eval0(arg, &tv, NULL, &evalarg);
    නම් (r == FAIL)
	retval = NULL;
    නැතහොත්
    {
	retval = typval2string(&tv, join_list);
	clear_tv(&tv);
    }
    clear_evalarg(&evalarg, NULL);

    ආපසු retval;
}

    char_u *
eval_to_string(
    char_u	*arg,
    පූර්ණ_සංඛ්‍යා		join_list,
    පූර්ණ_සංඛ්‍යා		use_simple_function)
{
    ආපසු eval_to_string_eap(arg, join_list, NULL, use_simple_function);
}

/*
 * Call eval_to_string() without using current local variables and using
 * textlock.  When "use_sandbox" is TRUE use the sandbox.
 * Use legacy Vim script syntax.
 */
    char_u *
eval_to_string_safe(
    char_u	*arg,
    පූර්ණ_සංඛ්‍යා		use_sandbox,
    පූර්ණ_සංඛ්‍යා		keep_script_version,
    පූර්ණ_සංඛ්‍යා		use_simple_function)
{
    char_u	*retval;
    funccal_entry_T funccal_entry;
    පූර්ණ_සංඛ්‍යා		save_sc_version = current_sctx.sc_version;
    පූර්ණ_සංඛ්‍යා		save_garbage = may_garbage_collect;

    නම් (!keep_script_version)
	current_sctx.sc_version = 1;
    save_funccal(&funccal_entry);
    නම් (use_sandbox)
	++sandbox;
    ++textlock;
    may_garbage_collect = FALSE;
    retval = eval_to_string(arg, FALSE, use_simple_function);
    නම් (use_sandbox)
	--sandbox;
    --textlock;
    may_garbage_collect = save_garbage;
    restore_funccal();
    current_sctx.sc_version = save_sc_version;
    ආපසු retval;
}

/*
 * Top level evaluation function, returning a number.
 * Evaluates "expr" silently.
 * Returns -1 for an error.
 */
    varnumber_T
eval_to_number(char_u *expr, පූර්ණ_සංඛ්‍යා use_simple_function)
{
    typval_T	rettv;
    varnumber_T	retval;
    char_u	*p = skipwhite(expr);
    පූර්ණ_සංඛ්‍යා		r = NOTDONE;

    ++emsg_off;

    නම් (use_simple_function)
	r = may_call_simple_func(expr, &rettv);
    නම් (r == NOTDONE)
	r = eval1(&p, &rettv, &EVALARG_EVALUATE);
    නම් (r == FAIL)
	retval = -1;
    නැතහොත්
    {
	retval = tv_get_number_chk(&rettv, NULL);
	clear_tv(&rettv);
    }
    --emsg_off;

    ආපසු retval;
}

/*
 * Top level evaluation function.
 * Returns an allocated typval_T with the result.
 * Returns NULL when there is an error.
 */
    typval_T *
eval_expr(char_u *arg, exarg_T *eap)
{
    ආපසු eval_expr_ext(arg, eap, FALSE);
}

    typval_T *
eval_expr_ext(char_u *arg, exarg_T *eap, පූර්ණ_සංඛ්‍යා use_simple_function)
{
    typval_T	*tv;
    evalarg_T	evalarg;

    fill_evalarg_from_eap(&evalarg, eap, eap != NULL && eap->skip);

    tv = ALLOC_ONE(typval_T);
    නම් (tv != NULL)
    {
	පූර්ණ_සංඛ්‍යා	r = NOTDONE;

	නම් (use_simple_function)
	    r = eval0_simple_funccal(arg, tv, eap, &evalarg);
	නම් (r == NOTDONE)
	    r = eval0(arg, tv, eap, &evalarg);

	නම් (r == FAIL)
	    VIM_CLEAR(tv);
    }

    clear_evalarg(&evalarg, eap);
    ආපසු tv;
}

/*
 * "*arg" points to what can be a function name in the form of "import.Name" or
 * "Funcref".  Return the name of the function.  Set "tofree" to something that
 * was allocated.
 * If "verbose" is FALSE no errors are given.
 * Return NULL for any failure.
 */
    static char_u *
deref_function_name(
	    char_u	**arg,
	    char_u	**tofree,
	    evalarg_T	*evalarg,
	    පූර්ණ_සංඛ්‍යා		verbose)
{
    typval_T	ref;
    char_u	*name = *arg;
    පූර්ණ_සංඛ්‍යා		save_flags = 0;
    පූර්ණ_සංඛ්‍යා		evaluate = evalarg != NULL
				      && (evalarg->eval_flags & EVAL_EVALUATE);

    ref.v_type = VAR_UNKNOWN;
    නම් (evalarg != NULL)
    {
	// need to evaluate this to get an import, like in "a.Func"
	save_flags = evalarg->eval_flags;
	evalarg->eval_flags |= EVAL_EVALUATE;
    }
    නම් (eval9(arg, &ref, evalarg, FALSE) == FAIL)
    {
	dictitem_T	*v;

	// If <SID>VarName was used it would not be found, try another way.
	v = find_var_also_in_script(name, NULL, FALSE);
	නම් (v == NULL)
	{
	    name = NULL;
	    goto theend;
	}
	copy_tv(&v->di_tv, &ref);
    }
    නම් (*skipwhite(*arg) != NUL)
    {
	නම් (verbose)
	    semsg(_(e_trailing_characters_str), *arg);
	name = NULL;
    }
    නැතහොත් නම් (ref.v_type == VAR_FUNC && ref.vval.v_string != NULL)
    {
	name = ref.vval.v_string;
	ref.vval.v_string = NULL;
	*tofree = name;
    }
    නැතහොත් නම් (ref.v_type == VAR_PARTIAL && ref.vval.v_partial != NULL)
    {
	නම් (ref.vval.v_partial->pt_argc > 0
		|| ref.vval.v_partial->pt_dict != NULL)
	{
	    නම් (verbose)
		emsg(_(e_cannot_use_partial_here));
	    name = NULL;
	}
	නැතහොත්
	{
	    name = vim_strsave(partial_name(ref.vval.v_partial));
	    *tofree = name;
	}
    }
    නැතහොත් නම් (evaluate)
    {
	නම් (verbose)
	    semsg(_(e_not_callable_type_str), name);
	name = NULL;
    }

theend:
    clear_tv(&ref);
    නම් (evalarg != NULL)
	evalarg->eval_flags = save_flags;
    ආපසු name;
}

/*
 * Call some Vim script function and return the result in "*rettv".
 * Uses argv[0] to argv[argc - 1] for the function arguments.  argv[argc]
 * should have type VAR_UNKNOWN.
 * Returns OK or FAIL.
 */
    පූර්ණ_සංඛ්‍යා
call_vim_function(
    char_u      *func,
    පූර්ණ_සංඛ්‍යා		argc,
    typval_T	*argv,
    typval_T	*rettv)
{
    පූර්ණ_සංඛ්‍යා		ret;
    funcexe_T	funcexe;
    char_u	*arg;
    char_u	*name;
    char_u	*tofree = NULL;
    පූර්ණ_සංඛ්‍යා		ignore_errors;

    rettv->v_type = VAR_UNKNOWN;		// clear_tv() uses this
    CLEAR_FIELD(funcexe);
    funcexe.fe_firstline = curwin->w_cursor.lnum;
    funcexe.fe_lastline = curwin->w_cursor.lnum;
    funcexe.fe_evaluate = TRUE;

    // The name might be "import.Func" or "Funcref".  We don't know, we need to
    // ignore errors for an undefined name.  But we do want errors when an
    // autoload script has errors.  Guess that when there is a dot in the name
    // showing errors is the right choice.
    ignore_errors = vim_strchr(func, '.') == NULL;
    arg = func;
    නම් (ignore_errors)
	++emsg_off;
    name = deref_function_name(&arg, &tofree, &EVALARG_EVALUATE, FALSE);
    නම් (ignore_errors)
	--emsg_off;
    නම් (name == NULL)
	name = func;

    ret = call_func(name, -1, rettv, argc, argv, &funcexe);

    නම් (ret == FAIL)
	clear_tv(rettv);
    vim_free(tofree);

    ආපසු ret;
}

/*
 * Call Vim script function "func" and return the result as a string.
 * Uses "argv[0]" to "argv[argc - 1]" for the function arguments. "argv[argc]"
 * should have type VAR_UNKNOWN.
 * Returns NULL when calling the function fails.
 */
    void *
call_func_retstr(
    char_u      *func,
    පූර්ණ_සංඛ්‍යා		argc,
    typval_T	*argv)
{
    typval_T	rettv;
    char_u	*retval;

    නම් (call_vim_function(func, argc, argv, &rettv) == FAIL)
	ආපසු NULL;

    retval = vim_strsave(tv_get_string(&rettv));
    clear_tv(&rettv);
    ආපසු retval;
}

/*
 * Call Vim script function "func" and return the result as a List.
 * Uses "argv" and "argc" as call_func_retstr().
 * Returns NULL when there is something wrong.
 * Gives an error when the returned value is not a list.
 */
    void *
call_func_retlist(
    char_u      *func,
    පූර්ණ_සංඛ්‍යා		argc,
    typval_T	*argv)
{
    typval_T	rettv;

    නම් (call_vim_function(func, argc, argv, &rettv) == FAIL)
	ආපසු NULL;

    නම් (rettv.v_type != VAR_LIST)
    {
	semsg(_(e_custom_list_completion_function_does_not_return_list_but_str),
		vartype_name(rettv.v_type));
	clear_tv(&rettv);
	ආපසු NULL;
    }

    ආපසු rettv.vval.v_list;
}

#නම් defined(FEAT_FOLDING)
/*
 * Evaluate "arg", which is 'foldexpr'.
 * Note: caller must set "curwin" to match "arg".
 * Returns the foldlevel, and any character preceding it in "*cp".  Doesn't
 * give error messages.
 */
    පූර්ණ_සංඛ්‍යා
eval_foldexpr(win_T *wp, පූර්ණ_සංඛ්‍යා *cp)
{
    char_u	*arg;
    typval_T	tv;
    varnumber_T	retval;
    char_u	*s;
    sctx_T	saved_sctx = current_sctx;
    පූර්ණ_සංඛ්‍යා		use_sandbox = was_set_insecurely(wp, (char_u *)"foldexpr",
								    OPT_LOCAL);

    arg = skipwhite(wp->w_p_fde);
    current_sctx = wp->w_p_script_ctx[WV_FDE];

    ++emsg_off;
    නම් (use_sandbox)
	++sandbox;
    ++textlock;
    *cp = NUL;

    // Evaluate the expression.  If the expression is "FuncName()" call the
    // function directly.
    නම් (eval0_simple_funccal(arg, &tv, NULL, &EVALARG_EVALUATE) == FAIL)
	retval = 0;
    නැතහොත්
    {
	// If the result is a number, just return the number.
	නම් (tv.v_type == VAR_NUMBER)
	    retval = tv.vval.v_number;
	නැතහොත් නම් (tv.v_type != VAR_STRING || tv.vval.v_string == NULL)
	    retval = 0;
	නැතහොත්
	{
	    // If the result is a string, check if there is a non-digit before
	    // the number.
	    s = tv.vval.v_string;
	    නම් (*s != NUL && !VIM_ISDIGIT(*s) && *s != '-')
		*cp = *s++;
	    retval = atol((var_na *)s);
	}
	clear_tv(&tv);
    }
    --emsg_off;
    නම් (use_sandbox)
	--sandbox;
    --textlock;
    clear_evalarg(&EVALARG_EVALUATE, NULL);
    current_sctx = saved_sctx;

    ආපසු (පූර්ණ_සංඛ්‍යා)retval;
}
#endif

#ifdef LOG_LOCKVAR
typedef sa_nracanaa flag_string_S
{
    පූර්ණ_සංඛ්‍යා	    flag;
    var_na    *str;
} flag_string_T;

    static var_na *
flags_tostring(පූර්ණ_සංඛ්‍යා flags, flag_string_T *_fstring, var_na *buf, size_t n)
{
    var_na *p = buf;
    *p = NUL;
    සඳහා (flag_string_T *fstring = _fstring; fstring->flag; ++fstring)
    {
	නම් ((fstring->flag & flags) != 0)
	{
	    size_t len = STRLEN(fstring->str);
	    නම් (n > p - buf + len + 7)
	    {
		STRCAT(p, fstring->str);
		p += len;
		STRCAT(p, " ");
		++p;
	    }
	    නැතහොත්
	    {
		STRCAT(buf, "...");
		bha_nga;
	    }
	}
    }
    ආපසු buf;
}

flag_string_T glv_flag_strings[] = {
    { GLV_QUIET,		"QUIET" },
    { GLV_NO_AUTOLOAD,		"NO_AUTOLOAD" },
    { GLV_READ_ONLY,		"READ_ONLY" },
    { GLV_NO_DECL,		"NO_DECL" },
    { GLV_COMPILING,		"COMPILING" },
    { GLV_ASSIGN_WITH_OP,	"ASSIGN_WITH_OP" },
    { GLV_PREFER_FUNC,		"PREFER_FUNC" },
    { 0,			NULL }
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
    static void
fill_lval_from_lval_root(lval_T *lp, lval_root_T *lr)
{
#ifdef LOG_LOCKVAR
    ch_log(NULL, "LKVAR: fill_lval_from_lval_root(): name %s, tv %p",
						lp->ll_name, (void*)lr->lr_tv);
#endif
    නම් (lr->lr_tv == NULL)
	ආපසු;
    නම් (!lr->lr_is_arg && lr->lr_tv->v_type == VAR_CLASS)
    {
	නම් (lr->lr_tv->vval.v_class != NULL)
	{
	    // Special special case. Look for a bare class variable reference.
	    class_T	*cl = lr->lr_tv->vval.v_class;
	    පූර්ණ_සංඛ්‍යා		m_idx;
	    ocmember_T	*m = class_member_lookup(cl, lp->ll_name,
					lp->ll_name_end - lp->ll_name, &m_idx);
	    නම් (m != NULL)
	    {
		// Assuming "inside class" since bare reference.
		lp->ll_class = lr->lr_tv->vval.v_class;
		lp->ll_oi = m_idx;
		lp->ll_valtype = m->ocm_type;
		lp->ll_tv = &lp->ll_class->class_members_tv[m_idx];
#ifdef LOG_LOCKVAR
		ch_log(NULL, "LKVAR:    ... class member %s.%s",
					lp->ll_class->class_name.string, lp->ll_name);
#endif
		ආපසු;
	    }
	}
    }

#ifdef LOG_LOCKVAR
    ch_log(NULL, "LKVAR:    ... type: %s", vartype_name(lr->lr_tv->v_type));
#endif
    lp->ll_tv = lr->lr_tv;
    lp->ll_is_root = TRUE;
}

/*
 * Check if the class has permission to access the member.
 * Returns OK or FAIL.
 */
    static පූර්ණ_සංඛ්‍යා
get_lval_check_access(
    class_T	*cl_exec,   // executing class, NULL if :def or script level
    class_T	*cl,	    // class which contains the member
    ocmember_T	*om,	    // member being accessed
    char_u	*p,	    // char after member name
    පූර්ණ_සංඛ්‍යා		flags)	    // GLV flags to check if writing to lval
{
#ifdef LOG_LOCKVAR
    ch_log(NULL, "LKVAR: get_lval_check_access(), cl_exec %p, cl %p, %c",
						(void*)cl_exec, (void*)cl, *p);
#endif
    නම් (cl_exec != NULL && cl_exec == cl)
	ආපසු OK;

    var_na *msg = NULL;
    switch (om->ocm_access)
    {
	pak_sa VIM_ACCESS_PRIVATE:
	    msg = e_cannot_access_protected_variable_str;
	    bha_nga;
	pak_sa VIM_ACCESS_READ:
	    // If [idx] or .key following, read only OK.
	    නම් (*p == '[' || *p == '.')
		bha_nga;
	    නම් ((flags & GLV_READ_ONLY) == 0)
	    {
		නම් (IS_ENUM(cl))
		{
		    නම් (om->ocm_type->tt_type == VAR_OBJECT)
			semsg(_(e_enumvalue_str_cannot_be_modified),
				cl->class_name.string, om->ocm_name.string);
		    නැතහොත්
			msg = e_variable_is_not_writable_str;
		}
		නැතහොත්
		    msg = e_variable_is_not_writable_str;
	    }
	    bha_nga;
	pak_sa VIM_ACCESS_ALL:
	    bha_nga;
    }
    නම් (msg != NULL)
    {
	emsg_var_cl_define(msg, om->ocm_name.string, 0, cl);
	ආපසු FAIL;
    }
    ආපසු OK;
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
    static char_u *
get_lval_imported(
    lval_T	*lp,
    scid_T	imp_sid,
    char_u	*p,
    dictitem_T	**dip,
    පූර්ණ_සංඛ්‍යා		fne_flags)
{
    ufunc_T	*ufunc;
    type_T	*type = NULL;
    පූර්ණ_සංඛ්‍යා		cc;
    පූර්ණ_සංඛ්‍යා		rc = FAIL;

    p = skipwhite(p);

    import_check_sourced_sid(&imp_sid);
    lp->ll_sid = imp_sid;
    lp->ll_name = p;
    p = find_name_end(lp->ll_name, NULL, NULL, fne_flags);
    lp->ll_name_end = p;

    // check the item is exported
    cc = *p;
    *p = NUL;
    නම් (find_exported(imp_sid, lp->ll_name, &ufunc, &type, NULL, NULL,
								TRUE) == -1)
	goto failed;

    // Get the typval for the exported item
    hashtab_T *ht = &SCRIPT_VARS(imp_sid);
    නම් (ht == NULL)
	goto failed;

    dictitem_T *di = find_var_in_ht(ht, 0, lp->ll_name, TRUE);
    නම් (di == NULL)
	// script is autoloaded.  So variable will be found later
	goto success;

    *dip = di;

    // Check whether the variable is writable.
    svar_T *sv = find_typval_in_script(&di->di_tv, imp_sid, FALSE);
    නම් (sv != NULL && sv->sv_const != 0)
    {
	semsg(_(e_cannot_change_readonly_variable_str), lp->ll_name);
	goto failed;
    }

    // check whether variable is locked
    නම් (value_check_lock(di->di_tv.v_lock, lp->ll_name, FALSE))
	goto failed;

    lp->ll_tv = &di->di_tv;
    lp->ll_valtype = type;

success:
    rc = OK;

failed:
    *p = cc;
    ආපසු rc == OK ? p : NULL;
}

typedef ga_nanaa {
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
    static පූර්ණ_සංඛ්‍යා
get_lval_dict_item(
    lval_T	*lp,
    char_u	*name,
    char_u	*key,
    පූර්ණ_සංඛ්‍යා		len,
    char_u	**key_end,
    typval_T	*var1,
    පූර්ණ_සංඛ්‍යා		flags,
    පූර්ණ_සංඛ්‍යා		unlet,
    typval_T	*rettv)
{
    පූර්ණ_සංඛ්‍යා		quiet = flags & GLV_QUIET;
    char_u	*p = *key_end;

    නම් (len == -1)
    {
	// "[key]": get key from "var1"
	key = tv_get_string_chk(var1);	// is number or string
	නම් (key == NULL)
	    ආපසු GLV_FAIL;
    }
    lp->ll_list = NULL;
    lp->ll_list = NULL;
    lp->ll_blob = NULL;
    lp->ll_object = NULL;
    lp->ll_class = NULL;
    lp->ll_tuple = NULL;

    // a NULL dict is equivalent with an empty dict
    නම් (lp->ll_tv->vval.v_dict == NULL)
    {
	lp->ll_tv->vval.v_dict = dict_alloc();
	නම් (lp->ll_tv->vval.v_dict == NULL)
	    ආපසු GLV_FAIL;
	++lp->ll_tv->vval.v_dict->dv_refcount;
    }
    lp->ll_dict = lp->ll_tv->vval.v_dict;

    lp->ll_di = dict_find(lp->ll_dict, key, len);

    // When assigning to a scope dictionary check that a function and
    // variable name is valid (only variable name unless it is l: or
    // g: dictionary). Disallow overwriting a builtin function.
    නම් (rettv != NULL && lp->ll_dict->dv_scope != 0)
    {
	පූර්ණ_සංඛ්‍යා prevval;

	නම් (len != -1)
	{
	    prevval = key[len];
	    key[len] = NUL;
	}
	නැතහොත්
	    prevval = 0; // avoid compiler warning
	පූර්ණ_සංඛ්‍යා wrong = (lp->ll_dict->dv_scope == VAR_DEF_SCOPE
		&& (rettv->v_type == VAR_FUNC
		    || rettv->v_type == VAR_PARTIAL)
		&& var_wrong_func_name(key, lp->ll_di == NULL))
	    || !valid_varname(key, -1, TRUE);
	නම් (len != -1)
	    key[len] = prevval;
	නම් (wrong)
	    ආපසු GLV_FAIL;
    }

    නම් (lp->ll_valtype != NULL)
	// use the type of the member
	lp->ll_valtype = lp->ll_valtype->tt_member;

    නම් (lp->ll_di == NULL)
    {
	// Can't add "v:" or "a:" variable.
	නම් (lp->ll_dict == get_vimvar_dict()
		|| &lp->ll_dict->dv_hashtab == get_funccal_args_ht())
	{
	    semsg(_(e_illegal_variable_name_str), name);
	    ආපසු GLV_FAIL;
	}

	// Key does not exist in dict: may need to add it.
	නම් (*p == '[' || *p == '.' || unlet)
	{
	    නම් (!quiet)
		semsg(_(e_key_not_present_in_dictionary_str), key);
	    ආපසු GLV_FAIL;
	}
	නම් (len == -1)
	    lp->ll_newkey = vim_strsave(key);
	නැතහොත්
	    lp->ll_newkey = vim_strnsave(key, len);
	නම් (lp->ll_newkey == NULL)
	    p = NULL;

	*key_end = p;
	ආපසු GLV_STOP;
    }
    // existing variable, need to check if it can be changed
    නැතහොත් නම් ((flags & GLV_READ_ONLY) == 0
	    && (var_check_ro(lp->ll_di->di_flags, name, FALSE)
		|| var_check_lock(lp->ll_di->di_flags, name, FALSE)))
	ආපසු GLV_FAIL;

    lp->ll_tv = &lp->ll_di->di_tv;

    ආපසු GLV_OK;
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
    static පූර්ණ_සංඛ්‍යා
get_lval_blob(
    lval_T	*lp,
    typval_T	*var1,
    typval_T	*var2,
    පූර්ණ_සංඛ්‍යා		empty1,
    පූර්ණ_සංඛ්‍යා		quiet)
{
    long	bloblen = blob_len(lp->ll_tv->vval.v_blob);

    lp->ll_list = NULL;
    lp->ll_dict = NULL;
    lp->ll_object = NULL;
    lp->ll_class = NULL;
    lp->ll_tuple = NULL;

    // Get the number and item for the only or first index of a List or Tuple.
    නම් (empty1)
	lp->ll_n1 = 0;
    නැතහොත්
	// is number or string
	lp->ll_n1 = (long)tv_get_number(var1);

    නම් (check_blob_index(bloblen, lp->ll_n1, quiet) == FAIL)
	ආපසු FAIL;
    නම් (lp->ll_range && !lp->ll_empty2)
    {
	lp->ll_n2 = (long)tv_get_number(var2);
	නම් (check_blob_range(bloblen, lp->ll_n1, lp->ll_n2, quiet) == FAIL)
	    ආපසු FAIL;
    }

    නම් (!lp->ll_range)
	// Indexing a single byte in a blob.  So the rhs type is a
	// number.
	lp->ll_valtype = &t_number;

    lp->ll_blob = lp->ll_tv->vval.v_blob;
    lp->ll_tv = NULL;

    ආපසු OK;
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
    static පූර්ණ_සංඛ්‍යා
get_lval_list(
    lval_T	*lp,
    typval_T	*var1,
    typval_T	*var2,
    පූර්ණ_සංඛ්‍යා		empty1,
    පූර්ණ_සංඛ්‍යා		flags,
    පූර්ණ_සංඛ්‍යා		quiet)
{
    /*
     * Get the number and item for the only or first index of the List.
     */
    නම් (empty1)
	lp->ll_n1 = 0;
    නැතහොත්
	// is number or string
	lp->ll_n1 = (long)tv_get_number(var1);

    lp->ll_dict = NULL;
    lp->ll_object = NULL;
    lp->ll_class = NULL;
    lp->ll_tuple = NULL;
    lp->ll_list = lp->ll_tv->vval.v_list;
    lp->ll_li = check_range_index_one(lp->ll_list, &lp->ll_n1,
				(flags & GLV_ASSIGN_WITH_OP) == 0, quiet);
    නම් (lp->ll_li == NULL)
	ආපසු FAIL;

    නම් (lp->ll_valtype != NULL && !lp->ll_range)
    {
	// use the type of the member
	නම් (lp->ll_valtype->tt_member != NULL)
	    lp->ll_valtype = lp->ll_valtype->tt_member;
	නැතහොත්
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
    නම් (lp->ll_range && !lp->ll_empty2)
    {
	lp->ll_n2 = (long)tv_get_number(var2);
	// is number or string
	නම් (check_range_index_two(lp->ll_list,
			&lp->ll_n1, lp->ll_li, &lp->ll_n2, quiet) == FAIL)
	    ආපසු FAIL;
    }

    lp->ll_tv = &lp->ll_li->li_tv;

    ආපසු OK;
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
    static පූර්ණ_සංඛ්‍යා
get_lval_tuple(
    lval_T	*lp,
    typval_T	*idx,
    පූර්ණ_සංඛ්‍යා		quiet)
{
    // is number or string
    lp->ll_n1 = (long)tv_get_number(idx);

    lp->ll_list = NULL;
    lp->ll_dict = NULL;
    lp->ll_blob = NULL;
    lp->ll_object = NULL;
    lp->ll_class = NULL;

    lp->ll_tuple = lp->ll_tv->vval.v_tuple;
    lp->ll_tv = tuple_find(lp->ll_tuple, lp->ll_n1);
    නම් (lp->ll_tv == NULL)
    {
	නම් (!quiet)
	    semsg(_(e_tuple_index_out_of_range_nr), lp->ll_n1);
	ආපසු GLV_FAIL;
    }

    // use the type of the member
    නම් (lp->ll_valtype != NULL)
    {
	නම් (lp->ll_valtype != NULL
		&& lp->ll_valtype->tt_type == VAR_TUPLE
		&& lp->ll_valtype->tt_argcount == 1)
	{
	    // a variadic tuple or a single item tuple
	    නම් (lp->ll_valtype->tt_flags & TTFLAG_VARARGS)
		lp->ll_valtype = lp->ll_valtype->tt_args[0]->tt_member;
	    නැතහොත්
		lp->ll_valtype = lp->ll_valtype->tt_args[0];
	}
	නැතහොත්
	    // If the LHS member type is not known (VAR_ANY), then get it from
	    // the tuple item (after indexing)
	    lp->ll_valtype = typval2type(lp->ll_tv, get_copyID(),
					&lp->ll_type_list, TVTT_DO_MEMBER);
    }

    ආපසු GLV_OK;
}

/*
 * Get a class or object lval method in class "cl".  The 'key' argument points
 * to the method name and 'key_end' points to the character after 'key'.
 * 'v_type' is VAR_CLASS or VAR_OBJECT.
 *
 * The method index, method function pointer and method type are returned in
 * "lp".
 */
    static පූර්ණ_සංඛ්‍යා
get_lval_oc_method(
    lval_T	*lp,
    class_T	*cl,
    char_u	*key,
    char_u	**key_end,
    vartype_T	v_type)
{
    // Look for a method with this name.
    // round 1: class functions (skipped for an object)
    // round 2: object methods
    සඳහා (පූර්ණ_සංඛ්‍යා round = v_type == VAR_OBJECT ? 2 : 1; round <= 2; ++round)
    {
	පූර්ණ_සංඛ්‍යා	m_idx;
	ufunc_T	*fp;

	fp = method_lookup(cl, round == 1 ? VAR_CLASS : VAR_OBJECT,
						key, *key_end - key, &m_idx);
	lp->ll_oi = m_idx;

	// process generic method (if present)
	නම් (fp && (fp = eval_generic_func(fp, key, key_end)) == NULL)
	    ආපසු FAIL;

	නම් (fp != NULL)
	{
	    lp->ll_ufunc = fp;
	    lp->ll_valtype = fp->uf_func_type;
	    bha_nga;
	}
    }

    ආපසු OK;
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
    static පූර්ණ_සංඛ්‍යා
get_lval_oc_variable(
    lval_T	*lp,
    class_T	*cl,
    char_u	*key,
    char_u	*key_end,
    vartype_T	v_type,
    class_T	*cl_exec,
    පූර්ණ_සංඛ්‍යා		flags)
{
    පූර්ණ_සංඛ්‍යා		m_idx;
    ocmember_T	*om;

    om = member_lookup(cl, v_type, key, key_end - key, &m_idx);
    lp->ll_oi = m_idx;
    නම් (om == NULL)
	ආපසු OK;

    // Check variable is accessible
    නම් (get_lval_check_access(cl_exec, cl, om, key_end, flags) == FAIL)
	ආපසු FAIL;

    // When lhs is used to modify the variable, check it is not a read-only
    // variable.
    නම් ((flags & GLV_READ_ONLY) == 0 && (*key_end != '.' && *key_end != '[')
	    && oc_var_check_ro(cl, om))
	ආපසු FAIL;

    lp->ll_valtype = om->ocm_type;

    නම් (v_type == VAR_OBJECT)
	lp->ll_tv = ((typval_T *)(lp->ll_tv->vval.v_object + 1)) + m_idx;
    නැතහොත්
	lp->ll_tv = &cl->class_members_tv[m_idx];

    ආපසු OK;
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
    static පූර්ණ_සංඛ්‍යා
get_lval_class_or_obj(
    lval_T	*lp,
    char_u	*key,
    char_u	**key_end,
    vartype_T	v_type,
    class_T	*cl_exec,
    පූර්ණ_සංඛ්‍යා		flags,
    පූර්ණ_සංඛ්‍යා		quiet)
{
    lp->ll_dict = NULL;
    lp->ll_list = NULL;
    lp->ll_tuple = NULL;

    class_T *cl;
    නම් (v_type == VAR_OBJECT)
    {
	නම් (lp->ll_tv->vval.v_object == NULL)
	{
	    නම් (!quiet)
		emsg(_(e_using_null_object));
	    ආපසු FAIL;
	}
	cl = lp->ll_tv->vval.v_object->obj_class;
	lp->ll_object = lp->ll_tv->vval.v_object;
    }
    නැතහොත්
    {
	cl = lp->ll_tv->vval.v_class;
	lp->ll_object = NULL;
    }
    lp->ll_class = cl;

    නම් (cl == NULL)
	// TODO: what if class is NULL?
	ආපසු OK;

    lp->ll_valtype = NULL;

    නම් (flags & GLV_PREFER_FUNC)
	නම් (get_lval_oc_method(lp, cl, key, key_end, v_type) == FAIL)
	    ආපසු FAIL;

    // Look for object/class member variable
    නම් (lp->ll_valtype == NULL)
    {
	නම් (get_lval_oc_variable(lp, cl, key, *key_end, v_type, cl_exec, flags)
								== FAIL)
	    ආපසු FAIL;
    }

    නම් (lp->ll_valtype == NULL)
    {
	member_not_found_msg(cl, v_type, key, *key_end - key);
	ආපසු FAIL;
    }

    ආපසු OK;
}

/*
 * Check whether dot (".") is allowed after the variable "name" with type
 * "v_type".  Only Dict, Class and Object types support a dot after the name.
 * Returns TRUE if dot is allowed after the name.
 */
    static පූර්ණ_සංඛ්‍යා
dot_allowed_after_type(char_u *name, vartype_T v_type, පූර්ණ_සංඛ්‍යා quiet)
{
    නම් (v_type != VAR_DICT && v_type != VAR_OBJECT && v_type != VAR_CLASS)
    {
	නම් (!quiet)
	    semsg(_(e_dot_not_allowed_after_str_str),
		    vartype_name(v_type), name);
	ආපසු FALSE;
    }

    ආපසු TRUE;
}

/*
 * Check whether left bracket ("[") is allowed after the variable "name" with
 * type "v_type".  Only Dict, List, Tuple and Blob types support a bracket
 * after the variable name.  Returns TRUE if bracket is allowed after the name.
 */
    static පූර්ණ_සංඛ්‍යා
bracket_allowed_after_type(char_u *name, vartype_T v_type, පූර්ණ_සංඛ්‍යා quiet)
{
    නම් (v_type == VAR_CLASS || v_type == VAR_OBJECT)
    {
	නම් (!quiet)
	    semsg(_(e_index_not_allowed_after_str_str),
		    vartype_name(v_type), name);
	ආපසු FALSE;
    }

    ආපසු TRUE;
}

/*
 * Check whether the variable "name" with type "v_type" can be followed by an
 * index.  Only Dict, List, Tuple, Blob, Object and Class types support
 * indexing.  Returns TRUE if indexing is allowed after the name.
 */
    static පූර්ණ_සංඛ්‍යා
index_allowed_after_type(char_u *name, vartype_T v_type, පූර්ණ_සංඛ්‍යා quiet)
{
    නම් (v_type != VAR_LIST
	    && v_type != VAR_TUPLE
	    && v_type != VAR_DICT
	    && v_type != VAR_BLOB
	    && v_type != VAR_OBJECT
	    && v_type != VAR_CLASS)
    {
	නම් (!quiet)
	    semsg(_(e_index_not_allowed_after_str_str),
		    vartype_name(v_type), name);
	ආපසු FALSE;
    }

    ආපසු TRUE;
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
    static char_u *
get_lval_subscript(
    lval_T	*lp,
    char_u	*p,
    char_u	*name,
    typval_T	*rettv,
    hashtab_T	*ht,
    dictitem_T	*v,
    පූර්ණ_සංඛ්‍යා		unlet,
    පූර්ණ_සංඛ්‍යා		flags,	    // GLV_ values
    class_T	*cl_exec)
{
    පූර්ණ_සංඛ්‍යා		quiet = flags & GLV_QUIET;
    char_u	*key = NULL;
    පූර්ණ_සංඛ්‍යා		len;
    typval_T	var1;
    typval_T	var2;
    පූර්ණ_සංඛ්‍යා		empty1 = FALSE;
    පූර්ණ_සංඛ්‍යා		rc = FAIL;

    /*
     * Loop until no more [idx] or .key is following.
     */
    var1.v_type = VAR_UNKNOWN;
    var2.v_type = VAR_UNKNOWN;

    අතරතුර (*p == '[' || (*p == '.' && p[1] != '=' && p[1] != '.'))
    {
	vartype_T v_type = lp->ll_tv->v_type;

	නම් (*p == '.' && !dot_allowed_after_type(name, v_type, quiet))
	    goto done;

	නම් (*p == '[' && !bracket_allowed_after_type(name, v_type, quiet))
	    goto done;

	නම් (!index_allowed_after_type(name, v_type, quiet))
	    goto done;

	// A NULL list/blob works like an empty list/blob, allocate one now.
	පූර්ණ_සංඛ්‍යා r = OK;
	නම් (v_type == VAR_LIST && lp->ll_tv->vval.v_list == NULL)
	    r = rettv_list_alloc(lp->ll_tv);
	නැතහොත් නම් (v_type == VAR_BLOB && lp->ll_tv->vval.v_blob == NULL)
	    r = rettv_blob_alloc(lp->ll_tv);
	නම් (r == FAIL)
	    goto done;

	නම් (lp->ll_range)
	{
	    නම් (!quiet)
		emsg(_(e_slice_must_come_last));
	    goto done;
	}
#ifdef LOG_LOCKVAR
	ch_log(NULL, "LKVAR: get_lval() loop: p: %s, type: %s", p,
		vartype_name(v_type));
#endif

	නම් (current_script_is_vim9() && lp->ll_valtype == NULL
		&& v != NULL
		&& lp->ll_tv == &v->di_tv
		&& ht != NULL && ht == get_script_local_ht())
	{
	    svar_T  *sv = find_typval_in_script(lp->ll_tv, 0, TRUE);

	    // Vim9 script local variable: get the type
	    නම් (sv != NULL)
	    {
		lp->ll_valtype = sv->sv_type;
#ifdef LOG_LOCKVAR
		ch_log(NULL, "LKVAR:    ... loop: vim9 assign type: %s",
			vartype_name(lp->ll_valtype->tt_type));
#endif
	    }
	}

	len = -1;
	නම් (*p == '.')
	{
	    key = p + 1;

	    සඳහා (len = 0; ASCII_ISALNUM(key[len]) || key[len] == '_'; ++len)
		;
	    නම් (len == 0)
	    {
		නම් (!quiet)
		    emsg(_(e_cannot_use_empty_key_for_dictionary));
		goto done;
	    }
	    p = key + len;
	}
	නැතහොත්
	{
	    // Get the index [expr] or the first index [expr: ].
	    p = skipwhite(p + 1);
	    නම් (*p == ':')
		empty1 = TRUE;
	    නැතහොත්
	    {
		empty1 = FALSE;
		නම් (eval1(&p, &var1, &EVALARG_EVALUATE) == FAIL)  // recursive!
		    goto done;
		නම් (tv_get_string_chk(&var1) == NULL)
		    // not a number or string
		    goto done;
		p = skipwhite(p);
	    }

	    // Optionally get the second index [ :expr].
	    නම් (*p == ':')
	    {
		නම් (v_type == VAR_DICT)
		{
		    නම් (!quiet)
			emsg(_(e_cannot_slice_dictionary));
		    goto done;
		}
		නම් (v_type == VAR_TUPLE)
		{
		    නම් (!quiet)
			emsg(_(e_cannot_slice_tuple));
		    goto done;
		}
		නම් (rettv != NULL
			&& !(rettv->v_type == VAR_LIST
			    && rettv->vval.v_list != NULL)
			&& !(rettv->v_type == VAR_BLOB
			    && rettv->vval.v_blob != NULL))
		{
		    නම් (!quiet)
			emsg(_(e_slice_requires_list_or_blob_value));
		    goto done;
		}
		p = skipwhite(p + 1);
		නම් (*p == ']')
		    lp->ll_empty2 = TRUE;
		නැතහොත්
		{
		    lp->ll_empty2 = FALSE;
		    // recursive!
		    නම් (eval1(&p, &var2, &EVALARG_EVALUATE) == FAIL)
			goto done;
		    නම් (tv_get_string_chk(&var2) == NULL)
			// not a number or string
			goto done;
		}
		lp->ll_range = TRUE;
	    }
	    නැතහොත්
		lp->ll_range = FALSE;

	    නම් (*p != ']')
	    {
		නම් (!quiet)
		    emsg(_(e_missing_closing_square_brace));
		goto done;
	    }

	    // Skip to past ']'.
	    ++p;
	}
#ifdef LOG_LOCKVAR
	නම් (len == -1)
	    ch_log(NULL, "LKVAR:    ... loop: p: %s, '[' key: %s", p,
		    empty1 ? ":" : (var_na*)tv_get_string(&var1));
	නැතහොත්
	    ch_log(NULL, "LKVAR:    ... loop: p: %s, '.' key: %s", p, key);
#endif

	නම් (v_type == VAR_DICT)
	{
	    glv_status_T glv_status;

	    glv_status = get_lval_dict_item(lp, name, key, len, &p, &var1,
							flags, unlet, rettv);
	    නම් (glv_status == GLV_FAIL)
		goto done;
	    නම් (glv_status == GLV_STOP)
		bha_nga;
	}
	නැතහොත් නම් (v_type == VAR_BLOB)
	{
	    නම් (get_lval_blob(lp, &var1, &var2, empty1, quiet) == FAIL)
		goto done;

	    bha_nga;
	}
	නැතහොත් නම් (v_type == VAR_LIST)
	{
	    නම් (get_lval_list(lp, &var1, &var2, empty1, flags, quiet) == FAIL)
		goto done;
	}
	නැතහොත් නම් (v_type == VAR_TUPLE)
	{
	    නම් (get_lval_tuple(lp, &var1, quiet) == FAIL)
		goto done;
	}
	නැතහොත්  // v_type == VAR_CLASS || v_type == VAR_OBJECT
	{
	    නම් (get_lval_class_or_obj(lp, key, &p, v_type, cl_exec, flags,
							quiet) == FAIL)
		goto done;
	}

	clear_tv(&var1);
	clear_tv(&var2);
	var1.v_type = VAR_UNKNOWN;
	var2.v_type = VAR_UNKNOWN;
    }

    නම් (lp->ll_tuple != NULL && (flags & GLV_READ_ONLY) == 0)
    {
	නම් (!quiet)
	    emsg(_(e_tuple_is_immutable));
	goto done;
    }

    rc = OK;

done:
    clear_tv(&var1);
    clear_tv(&var2);
    ආපසු rc == OK ? p : NULL;
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
    පූර්ණ_සංඛ්‍යා		unlet,
    පූර්ණ_සංඛ්‍යා		skip,
    පූර්ණ_සංඛ්‍යා		flags,	    // GLV_ values
    පූර්ණ_සංඛ්‍යා		fne_flags)  // flags for find_name_end()
{
    char_u	*p;
    char_u	*expr_start, *expr_end;
    පූර්ණ_සංඛ්‍යා		cc;
    dictitem_T	*v = NULL;
    hashtab_T	*ht = NULL;
    පූර්ණ_සංඛ්‍යා		quiet = flags & GLV_QUIET;
    පූර්ණ_සංඛ්‍යා		writing = 0;
    පූර්ණ_සංඛ්‍යා		vim9script = in_vim9script();
    class_T	*cl_exec = NULL;    // class that is executing, or NULL.

#ifdef LOG_LOCKVAR
    නම් (lval_root == NULL)
	ch_log(NULL, "LKVAR: get_lval(): name: %s, lval_root (nil)", name);
    නැතහොත්
	ch_log(NULL, "LKVAR: get_lval(): name: %s, lr_tv %p lr_is_arg %d",
			name, (void*)lval_root->lr_tv, lval_root->lr_is_arg);
    var_na buf[80];
    ch_log(NULL, "LKVAR:    ...: GLV flags: %s",
		    flags_tostring(flags, glv_flag_strings, buf, parimaa_na(buf)));
#endif

    // Clear everything in "lp".
    CLEAR_POINTER(lp);
    ga_init2(&lp->ll_type_list, parimaa_na(type_T *), 10);

    නම් (skip || (flags & GLV_COMPILING))
    {
	// When skipping or compiling just find the end of the name.
	lp->ll_name = name;
	lp->ll_name_end = find_name_end(name, NULL, NULL,
						      FNE_INCL_BR | fne_flags);
	ආපසු lp->ll_name_end;
    }

    // Cannot use "s:var" at the Vim9 script level.  "s: type" is OK.
    නම් (vim9script && at_script_level()
		  && name[0] == 's' && name[1] == ':' && !VIM_ISWHITE(name[2]))
    {
	semsg(_(e_cannot_use_s_colon_in_vim9_script_str), name);
	ආපසු NULL;
    }

    // Find the end of the name.
    p = find_name_end(name, &expr_start, &expr_end, fne_flags);
    lp->ll_name_end = p;
    නම් (expr_start != NULL)
    {
	// Don't expand the name when we already know there is an error.
	නම් (unlet && !VIM_ISWHITE(*p) && !ends_excmd(*p)
						    && *p != '[' && *p != '.')
	{
	    semsg(_(e_trailing_characters_str), p);
	    ආපසු NULL;
	}

	lp->ll_exp_name = make_expanded_name(name, expr_start, expr_end, p);
	නම් (lp->ll_exp_name == NULL)
	{
	    // Report an invalid expression in braces, unless the
	    // expression evaluation has been cancelled due to an
	    // aborting error, an interrupt, or an exception.
	    නම් (!aborting() && !quiet)
	    {
		emsg_severe = TRUE;
		semsg(_(e_invalid_argument_str), name);
		ආපසු NULL;
	    }
	}
	lp->ll_name = lp->ll_exp_name;
    }
    නැතහොත්
    {
	lp->ll_name = name;

	නම් (vim9script)
	{
	    // "a: type" is declaring variable "a" with a type, not "a:".
	    // However, "g:[key]" is indexing a dictionary.
	    නම් (p == name + 2 && p[-1] == ':' && *p != '[')
	    {
		--p;
		lp->ll_name_end = p;
	    }
	    නම් (*skipwhite(p) == ':')
	    {
		char_u	    *tp = skipwhite(p + 1);

		නම් (is_scoped_variable(name))
		{
		    semsg(_(e_cannot_use_type_with_this_variable_str), name);
		    ආපසු NULL;
		}
		නම් (VIM_ISWHITE(*p))
		{
		    semsg(_(e_no_white_space_allowed_before_colon_str), p);
		    ආපසු NULL;
		}
		නම් (tp == p + 1 && !quiet)
		{
		    semsg(_(e_white_space_required_after_str_str), ":", p);
		    ආපසු NULL;
		}
		නම් (!SCRIPT_ID_VALID(current_sctx.sc_sid))
		{
		    semsg(_(e_using_type_not_in_script_context_str), p);
		    ආපසු NULL;
		}
		නම් (vim9script && (flags & GLV_NO_DECL) &&
			!(flags & GLV_FOR_LOOP))
		{
		    // Using a type and not in a "var" declaration.
		    semsg(_(e_trailing_characters_str), p);
		    ආපසු NULL;
		}


		// parse the type after the name
		lp->ll_type = parse_type(&tp,
			       &SCRIPT_ITEM(current_sctx.sc_sid)->sn_type_list,
			       NULL, NULL, !quiet);
		නම් (!quiet && (lp->ll_type == NULL
			    || !valid_declaration_type(lp->ll_type)))
		    ආපසු NULL;
		lp->ll_name_end = tp;
	    }
	    // TODO: check inside class?
	}
    }
    නම් (lp->ll_name == NULL)
	ආපසු p;

    නම් (*p == '.')
    {
	// In legacy script, when a local variable and import exists with this name,
	// prioritize local variable over imports to avoid conflicts.
	පූර්ණ_සංඛ්‍යා var_exists = FALSE;
	නම් (!vim9script)
	{
	    cc = *p;
	    *p = NUL;
	    hashtab_T *local_ht = get_funccal_local_ht();
	    නම් (local_ht != NULL)
	    {
		hashitem_T *hi = hash_find(local_ht, lp->ll_name);
		නම් (!HASHITEM_EMPTY(hi))
		    var_exists = TRUE;
	    }
	    *p = cc;
	}

	නම් (!var_exists)
	{
	    imported_T *import = find_imported(lp->ll_name, p - lp->ll_name, TRUE);
	    නම් (import != NULL)
	    {
		p++;	// skip '.'
		p = get_lval_imported(lp, import->imp_sid, p, &v, fne_flags);
		නම් (p == NULL)
		    ආපසු NULL;
	    }
	}
    }

    // Without [idx] or .key we are done.
    නම් (*p != '[' && *p != '.')
    {
	නම් (lval_root != NULL)
	    fill_lval_from_lval_root(lp, lval_root);
	ආපසු p;
    }

    නම් (vim9script && lval_root != NULL)
	cl_exec = lval_root->lr_cl_exec;
    නම් (vim9script && lval_root != NULL && lval_root->lr_tv != NULL)
    {
	// using local variable
	lp->ll_tv = lval_root->lr_tv;
	v = NULL;
    }
    නැතහොත් නම් (lp->ll_tv == NULL)
    {
	cc = *p;
	*p = NUL;
	// When we would write to the variable pass &ht and prevent autoload.
	writing = !(flags & GLV_READ_ONLY);
	v = find_var(lp->ll_name, writing ? &ht : NULL,
					 (flags & GLV_NO_AUTOLOAD) || writing);
	නම් (v == NULL && !quiet)
	    semsg(_(e_undefined_variable_str), lp->ll_name);
	*p = cc;
	නම් (v == NULL)
	    ආපසු NULL;
	lp->ll_tv = &v->di_tv;
    }

    නම් (vim9script && (flags & GLV_NO_DECL) == 0)
    {
	නම් (!quiet)
	    semsg(_(e_variable_already_declared_str), lp->ll_name);
	ආපසු NULL;
    }

    // If the next character is a "." or a "[", then process the subitem.
    p = get_lval_subscript(lp, p, name, rettv, ht, v, unlet, flags, cl_exec);
    නම් (p == NULL)
	ආපසු NULL;

    නම් (vim9script && lp->ll_valtype != NULL && rettv != NULL)
    {
	where_T	    where = WHERE_INIT;

	// In a Vim9 script, do type check and make sure the variable is
	// writable.
	නම් (check_typval_type(lp->ll_valtype, rettv, where) == FAIL)
	    ආපසු NULL;
    }

    lp->ll_name_end = p;
    ආපසු p;
}

/*
 * Clear lval "lp" that was filled by get_lval().
 */
    void
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
    void
set_var_lval(
    lval_T	*lp,
    char_u	*endp,
    typval_T	*rettv,
    පූර්ණ_සංඛ්‍යා		copy,
    පූර්ණ_සංඛ්‍යා		flags,	    // ASSIGN_CONST, ASSIGN_NO_DECL
    char_u	*op,
    පූර්ණ_සංඛ්‍යා		var_idx)    // index for "let [a, b] = list"
{
    පූර්ණ_සංඛ්‍යා		cc;
    dictitem_T	*di;

    නම් (lp->ll_tv == NULL)
    {
	cc = *endp;
	*endp = NUL;
	නම් (in_vim9script() && check_reserved_name(lp->ll_name, FALSE) == FAIL)
	    ආපසු;

	නම් (lp->ll_blob != NULL)
	{
	    පූර්ණ_සංඛ්‍යා	    error = FALSE;

	    නම් (op != NULL && *op != '=')
	    {
		semsg(_(e_wrong_variable_type_for_str_equal), op);
		ආපසු;
	    }
	    නම් (value_check_lock(lp->ll_blob->bv_lock, lp->ll_name, FALSE))
		ආපසු;

	    නම් (lp->ll_range && rettv->v_type == VAR_BLOB)
	    {
		නම් (lp->ll_empty2)
		    lp->ll_n2 = blob_len(lp->ll_blob) - 1;

		නම් (blob_set_range(lp->ll_blob, lp->ll_n1, lp->ll_n2,
								rettv) == FAIL)
		    ආපසු;
	    }
	    නැතහොත්
	    {
		varnumber_T	val = tv_get_number_chk(rettv, &error);
		නම් (!error)
		{
		    නම් (val < 0 || val > 255)
			semsg(_(e_invalid_value_for_blob_nr), val);
		    නැතහොත්
			blob_set_append(lp->ll_blob, lp->ll_n1, val);
		}
	    }
	}
	නැතහොත් නම් (op != NULL && *op != '=')
	{
	    typval_T tv;

	    නම් ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					     && (flags & ASSIGN_FOR_LOOP) == 0)
	    {
		emsg(_(e_cannot_modify_existing_variable));
		*endp = cc;
		ආපසු;
	    }

	    // handle +=, -=, *=, /=, %= and .=
	    di = NULL;
	    නම් (eval_variable(lp->ll_name, (පූර්ණ_සංඛ්‍යා)STRLEN(lp->ll_name),
				 lp->ll_sid, &tv, &di, EVAL_VAR_VERBOSE) == OK)
	    {
		නම් (di != NULL && check_typval_is_value(&di->di_tv) == FAIL)
		{
		    clear_tv(&tv);
		    ආපසු;
		}

		// In Vim9 script a String v: variable only takes a String.
		නම් (in_vim9script() && STRNCMP(lp->ll_name, "v:", 2) == 0
			&& tv.v_type == VAR_STRING)
		{
		    where_T	where = WHERE_INIT;

		    නම් (check_typval_type(&t_string, rettv, where) == FAIL)
		    {
			clear_tv(&tv);
			*endp = cc;
			ආපසු;
		    }
		}

		නම් ((di == NULL
			 || (!var_check_ro(di->di_flags, lp->ll_name, FALSE)
			   && !tv_check_lock(&di->di_tv, lp->ll_name, FALSE)))
			&& tv_op(&tv, rettv, op) == OK)
		    set_var_const(lp->ll_name, lp->ll_sid, NULL, &tv, FALSE,
				ASSIGN_NO_DECL | ASSIGN_COMPOUND_OP, 0);
		clear_tv(&tv);
	    }
	}
	නැතහොත්
	{
	    නම් (lp->ll_type != NULL && check_typval_arg_type(lp->ll_type, rettv,
							      NULL, 0) == FAIL)
		ආපසු;
	    set_var_const(lp->ll_name, lp->ll_sid, lp->ll_type, rettv, copy,
							       flags, var_idx);
	}
	*endp = cc;
    }
    නැතහොත් නම් (value_check_lock(lp->ll_newkey == NULL
		? lp->ll_tv->v_lock
		: lp->ll_tv->vval.v_dict->dv_lock, lp->ll_name, FALSE))
	;
    නැතහොත් නම් (lp->ll_range)
    {
	නම් ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					     && (flags & ASSIGN_FOR_LOOP) == 0)
	{
	    emsg(_(e_cannot_lock_range));
	    ආපසු;
	}

	(void)list_assign_range(lp->ll_list, rettv->vval.v_list,
			 lp->ll_n1, lp->ll_n2, lp->ll_empty2, op, lp->ll_name);
    }
    නැතහොත්
    {
	/*
	 * Assign to a List, Dictionary or Object item.
	 */
	නම් ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					     && (flags & ASSIGN_FOR_LOOP) == 0)
	{
	    emsg(_(e_cannot_lock_list_or_dict));
	    ආපසු;
	}

	නම් (lp->ll_valtype != NULL
		    && check_typval_arg_type(lp->ll_valtype, rettv,
							      NULL, 0) == FAIL)
	{
	    lp->ll_name_end = NULL;
	    ආපසු;
	}

	// If the lval is a List and the type of the list is not yet set,
	// then set the item type from the declared type of the variable.
	නම් (in_vim9script() && rettv->v_type == VAR_LIST
				&& rettv->vval.v_list != NULL
				&& rettv->vval.v_list->lv_type == NULL)
	{
	    නම් (lp->ll_tv->v_type == VAR_LIST
		    && lp->ll_tv->vval.v_list != NULL
		    && lp->ll_tv->vval.v_list->lv_type != NULL)
		set_tv_type(rettv, lp->ll_tv->vval.v_list->lv_type);
	}

	නම් (lp->ll_newkey != NULL)
	{
	    නම් (op != NULL && *op != '=')
	    {
		semsg(_(e_key_not_present_in_dictionary_str), lp->ll_newkey);
		ආපසු;
	    }
	    නම් (dict_wrong_func_name(lp->ll_tv->vval.v_dict, rettv,
								lp->ll_newkey))
		ආපසු;

	    // Need to add an item to the Dictionary.
	    di = dictitem_alloc(lp->ll_newkey);
	    නම් (di == NULL)
		ආපසු;
	    නම් (dict_add(lp->ll_tv->vval.v_dict, di) == FAIL)
	    {
		vim_free(di);
		ආපසු;
	    }
	    lp->ll_tv = &di->di_tv;
	}
	නැතහොත් නම් (op != NULL && *op != '=')
	{
	    tv_op(lp->ll_tv, rettv, op);
	    ආපසු;
	}
	නැතහොත්
	    clear_tv(lp->ll_tv);

	/*
	 * Assign the value to the variable or list item.
	 */
	නම් (copy)
	    copy_tv(rettv, lp->ll_tv);
	නැතහොත්
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
    static පූර්ණ_සංඛ්‍යා
tv_op_blob(typval_T *tv1, typval_T *tv2, char_u *op)
{
    නම් (*op != '+' || tv2->v_type != VAR_BLOB)
	ආපසු FAIL;

    // Blob += Blob
    නම් (tv2->vval.v_blob == NULL)
	ආපසු OK;

    නම් (tv1->vval.v_blob == NULL)
    {
	tv1->vval.v_blob = tv2->vval.v_blob;
	++tv1->vval.v_blob->bv_refcount;
	ආපසු OK;
    }

    blob_T	*b1 = tv1->vval.v_blob;
    blob_T	*b2 = tv2->vval.v_blob;
    පූර්ණ_සංඛ්‍යා		len = blob_len(b2);

    නම් (len > 0 && ga_grow(&b1->bv_ga, len) == OK)
    {
	mch_memmove((char_u *)b1->bv_ga.ga_data + b1->bv_ga.ga_len,
		    (char_u *)b2->bv_ga.ga_data, (size_t)len);
	b1->bv_ga.ga_len += len;
    }
    නැතහොත්
	සඳහා (පූර්ණ_සංඛ්‍යා i = 0; i < len; i++)
	    (void)ga_append(&b1->bv_ga, blob_get(b2, i));

    ආපසු OK;
}

/*
 * Handle "list1 += list2".
 * Returns OK or FAIL.
 */
    static පූර්ණ_සංඛ්‍යා
tv_op_list(typval_T *tv1, typval_T *tv2, char_u *op)
{
    නම් (*op != '+' || tv2->v_type != VAR_LIST)
	ආපසු FAIL;

    // List += List
    නම් (tv2->vval.v_list == NULL)
	ආපසු OK;

    නම් (tv1->vval.v_list == NULL)
    {
	tv1->vval.v_list = tv2->vval.v_list;
	++tv1->vval.v_list->lv_refcount;
    }
    නැතහොත්
	list_extend(tv1->vval.v_list, tv2->vval.v_list, NULL);

    ආපසු OK;
}

/*
 * Handle number operations:
 *	nr += nr , nr -= nr , nr *=nr , nr /= nr , nr %= nr
 *
 * Returns OK or FAIL.
 */
    static පූර්ණ_සංඛ්‍යා
tv_op_number(typval_T *tv1, typval_T *tv2, char_u *op)
{
    varnumber_T	n;
    පූර්ණ_සංඛ්‍යා		failed = FALSE;

    n = tv_get_number(tv1);
    නම් (tv2->v_type == VAR_FLOAT)
    {
	float_T f = n;

	නම් (*op == '%')
	    ආපසු FAIL;
	switch (*op)
	{
	    pak_sa '+': f += tv2->vval.v_float; bha_nga;
	    pak_sa '-': f -= tv2->vval.v_float; bha_nga;
	    pak_sa '*': f *= tv2->vval.v_float; bha_nga;
	    pak_sa '/': f /= tv2->vval.v_float; bha_nga;
	}
	clear_tv(tv1);
	tv1->v_type = VAR_FLOAT;
	tv1->vval.v_float = f;
    }
    නැතහොත්
    {
	switch (*op)
	{
	    pak_sa '+': n += tv_get_number(tv2); bha_nga;
	    pak_sa '-': n -= tv_get_number(tv2); bha_nga;
	    pak_sa '*': n *= tv_get_number(tv2); bha_nga;
	    pak_sa '/': n = num_divide(n, tv_get_number(tv2), &failed); bha_nga;
	    pak_sa '%': n = num_modulus(n, tv_get_number(tv2), &failed); bha_nga;
	}
	clear_tv(tv1);
	tv1->v_type = VAR_NUMBER;
	tv1->vval.v_number = n;
    }

    ආපසු failed ? FAIL : OK;
}

/*
 * Append string "s2" to the string in "tv1".
 * Returns OK if "tv1" was grown in place, FAIL otherwise.
 */
    static පූර්ණ_සංඛ්‍යා
grow_string_tv(typval_T *tv1, char_u *s2)
{
    නම් (tv1->v_type != VAR_STRING || tv1->vval.v_string == NULL)
	ආපසු FAIL;

    size_t len1 = STRLEN(tv1->vval.v_string);
    size_t len2 = STRLEN(s2);
    char_u *p = vim_realloc(tv1->vval.v_string, len1 + len2 + 1);

    නම් (p == NULL)
	ආපසු FAIL;

    mch_memmove(p + len1, s2, len2 + 1);
    tv1->vval.v_string = p;
    ආපසු OK;
}

/*
 * Handle "str1 .= str2"
 * Returns OK or FAIL.
 */
    static පූර්ණ_සංඛ්‍යා
tv_op_string(typval_T *tv1, typval_T *tv2, char_u *op UNUSED)
{
    char_u	numbuf[NUMBUFLEN];
    char_u	*s;
    char_u	*s2;

    නම් (tv2->v_type == VAR_FLOAT)
	ආපසු FAIL;

    // str .= str
    s2 = tv_get_string_buf(tv2, numbuf);
    නම් (grow_string_tv(tv1, s2) == OK)
	ආපසු OK;

    s = tv_get_string(tv1);
    s = concat_str(s, s2);
    clear_tv(tv1);
    tv1->v_type = VAR_STRING;
    tv1->vval.v_string = s;

    ආපසු OK;
}

/*
 * Handle "tv1 += tv2", "tv1 -= tv2", "tv1 *= tv2", "tv1 /= tv2", "tv1 %= tv2"
 * and "tv1 .= tv2"
 * Returns OK or FAIL.
 */
    static පූර්ණ_සංඛ්‍යා
tv_op_nr_or_string(typval_T *tv1, typval_T *tv2, char_u *op)
{
    නම් (tv2->v_type == VAR_LIST)
	ආපසු FAIL;

    නම් (vim_strchr((char_u *)"+-*/%", *op) != NULL)
	ආපසු tv_op_number(tv1, tv2, op);

    ආපසු tv_op_string(tv1, tv2, op);
}

/*
 * Handle "f1 += f2", "f1 -= f2", "f1 *= f2", "f1 /= f2".
 * Returns OK or FAIL.
 */
    static පූර්ණ_සංඛ්‍යා
tv_op_float(typval_T *tv1, typval_T *tv2, char_u *op)
{
    float_T f;

    නම් (*op == '%' || *op == '.'
	    || (tv2->v_type != VAR_FLOAT
		&& tv2->v_type != VAR_NUMBER
		&& tv2->v_type != VAR_STRING))
	ආපසු FAIL;

    නම් (tv2->v_type == VAR_FLOAT)
	f = tv2->vval.v_float;
    නැතහොත්
	f = tv_get_number(tv2);
    switch (*op)
    {
	pak_sa '+': tv1->vval.v_float += f; bha_nga;
	pak_sa '-': tv1->vval.v_float -= f; bha_nga;
	pak_sa '*': tv1->vval.v_float *= f; bha_nga;
	pak_sa '/': tv1->vval.v_float /= f; bha_nga;
    }

    ආපසු OK;
}

/*
 * Handle "tv1 += tv2", "tv1 -= tv2", "tv1 *= tv2", "tv1 /= tv2", "tv1 %= tv2"
 * and "tv1 .= tv2"
 * Returns OK or FAIL.
 */
    පූර්ණ_සංඛ්‍යා
tv_op(typval_T *tv1, typval_T *tv2, char_u *op)
{
    // Can't do anything with a Funcref or Dict on the right.
    // v:true and friends only work with "..=".
    නම් (tv2->v_type == VAR_FUNC || tv2->v_type == VAR_DICT
		|| ((tv2->v_type == VAR_BOOL || tv2->v_type == VAR_SPECIAL)
							&& *op != '.'))
    {
	semsg(_(e_wrong_variable_type_for_str_equal), op);
	ආපසු FAIL;
    }

    පූර්ණ_සංඛ්‍යා retval = FAIL;
    switch (tv1->v_type)
    {
	pak_sa VAR_UNKNOWN:
	pak_sa VAR_ANY:
	pak_sa VAR_VOID:
	pak_sa VAR_DICT:
	pak_sa VAR_FUNC:
	pak_sa VAR_PARTIAL:
	pak_sa VAR_BOOL:
	pak_sa VAR_SPECIAL:
	pak_sa VAR_JOB:
	pak_sa VAR_CHANNEL:
	pak_sa VAR_INSTR:
	pak_sa VAR_OBJECT:
	pak_sa VAR_CLASS:
	pak_sa VAR_TYPEALIAS:
	pak_sa VAR_TUPLE:
	    bha_nga;

	pak_sa VAR_BLOB:
	    retval = tv_op_blob(tv1, tv2, op);
	    bha_nga;

	pak_sa VAR_LIST:
	    retval = tv_op_list(tv1, tv2, op);
	    bha_nga;

	pak_sa VAR_NUMBER:
	pak_sa VAR_STRING:
	    retval = tv_op_nr_or_string(tv1, tv2, op);
	    bha_nga;

	pak_sa VAR_FLOAT:
	    retval = tv_op_float(tv1, tv2, op);
	    bha_nga;
    }

    නම් (retval != OK)
	semsg(_(e_wrong_variable_type_for_str_equal), op);

    ආපසු retval;
}

/*
 * Evaluate the expression used in a ":for var in expr" command.
 * "arg" points to "var".
 * Set "*errp" to TRUE for an error, FALSE otherwise;
 * Return a pointer that holds the info.  Null when there is an error.
 */
    void *
eval_for_line(
    char_u	*arg,
    පූර්ණ_සංඛ්‍යා		*errp,
    exarg_T	*eap,
    evalarg_T	*evalarg)
{
    forinfo_T	*fi;
    char_u	*var_list_end;
    char_u	*expr;
    typval_T	tv;
    list_T	*l;
    tuple_T	*tuple;
    පූර්ණ_සංඛ්‍යා		skip = !(evalarg->eval_flags & EVAL_EVALUATE);

    *errp = TRUE;	// default: there is an error

    fi = ALLOC_CLEAR_ONE(forinfo_T);
    නම් (fi == NULL)
	ආපසු NULL;

    var_list_end = skip_var_list(arg, TRUE, &fi->fi_varcount,
						     &fi->fi_semicolon, FALSE);
    නම් (var_list_end == NULL)
	ආපසු fi;

    expr = skipwhite_and_linebreak(var_list_end, evalarg);
    නම් (expr[0] != 'i' || expr[1] != 'n'
				  || !(expr[2] == NUL || VIM_ISWHITE(expr[2])))
    {
	නම් (in_vim9script() && *expr == ':' && expr != var_list_end)
	    semsg(_(e_no_white_space_allowed_before_colon_str), expr);
	නැතහොත්
	    emsg(_(e_missing_in_after_for));
	ආපසු fi;
    }

    නම් (skip)
	++emsg_skip;
    expr = skipwhite_and_linebreak(expr + 2, evalarg);
    නම් (eval0(expr, &tv, eap, evalarg) == OK)
    {
	*errp = FALSE;
	නම් (!skip)
	{
	    නම් (tv.v_type == VAR_LIST)
	    {
		l = tv.vval.v_list;
		නම් (l == NULL)
		{
		    // a null list is like an empty list: do nothing
		    clear_tv(&tv);
		}
		නැතහොත්
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
	    නැතහොත් නම් (tv.v_type == VAR_TUPLE)
	    {
		tuple = tv.vval.v_tuple;
		නම් (tuple == NULL)
		{
		    // a null tuple is like an empty tuple: do nothing
		    clear_tv(&tv);
		}
		නැතහොත්
		{
		    // No need to increment the refcount, it's already set for
		    // the tuple being used in "tv".
		    fi->fi_tuple = tuple;
		    fi->fi_tuple_idx = 0;
		}
	    }
	    නැතහොත් නම් (tv.v_type == VAR_BLOB)
	    {
		fi->fi_bi = 0;
		නම් (tv.vval.v_blob != NULL)
		{
		    // Make a copy, so that the iteration still works when the
		    // blob is changed.
		    fi->fi_blob = blob_copy(tv.vval.v_blob);
		}
		clear_tv(&tv);
	    }
	    නැතහොත් නම් (tv.v_type == VAR_STRING)
	    {
		fi->fi_byte_idx = 0;
		fi->fi_string = tv.vval.v_string;
		tv.vval.v_string = NULL;
		නම් (fi->fi_string == NULL)
		    fi->fi_string = vim_strsave((char_u *)"");
	    }
	    නැතහොත්
	    {
		emsg(_(e_string_list_tuple_or_blob_required));
		clear_tv(&tv);
	    }
	}
    }
    නම් (skip)
	--emsg_skip;
    fi->fi_break_count = evalarg->eval_break_count;

    ආපසු fi;
}

/*
 * Used when looping over a :for line, skip the "in expr" part.
 */
    void
skip_for_lines(void *fi_void, evalarg_T *evalarg)
{
    forinfo_T	*fi = (forinfo_T *)fi_void;
    පූර්ණ_සංඛ්‍යා		i;

    සඳහා (i = 0; i < fi->fi_break_count; ++i)
	eval_next_line(NULL, evalarg);
}

/*
 * Use the first item in a ":for" list.  Advance to the next.
 * Assign the values to the variable (list).  "arg" points to the first one.
 * Return TRUE when a valid item was found, FALSE when at end of list or
 * something wrong.
 */
    පූර්ණ_සංඛ්‍යා
next_for_item(void *fi_void, char_u *arg)
{
    forinfo_T	*fi = (forinfo_T *)fi_void;
    පූර්ණ_සංඛ්‍යා		result;
    පූර්ණ_සංඛ්‍යා		flag = ASSIGN_FOR_LOOP | (in_vim9script()
			 ? (ASSIGN_FINAL
			     // first round: error if variable exists
			     | (fi->fi_bi == 0 ? 0 : ASSIGN_DECL)
			     | ASSIGN_NO_MEMBER_TYPE
			     | ASSIGN_UPDATE_BLOCK_ID)
			 : 0);
    listitem_T	*item;
    පූර්ණ_සංඛ්‍යා		skip_assign = in_vim9script() && arg[0] == '_'
						      && !eval_isnamec(arg[1]);

    නම් (fi->fi_blob != NULL)
    {
	typval_T	tv;

	නම් (fi->fi_bi >= blob_len(fi->fi_blob))
	    ආපසු FALSE;
	tv.v_type = VAR_NUMBER;
	tv.v_lock = VAR_FIXED;
	tv.vval.v_number = blob_get(fi->fi_blob, fi->fi_bi);
	++fi->fi_bi;
	නම් (skip_assign)
	    ආපසු TRUE;
	ආපසු ex_let_vars(arg, &tv, TRUE, fi->fi_semicolon,
					    fi->fi_varcount, flag, NULL) == OK;
    }

    නම් (fi->fi_string != NULL)
    {
	typval_T	tv;
	පූර්ණ_සංඛ්‍යා		len;

	len = mb_ptr2len(fi->fi_string + fi->fi_byte_idx);
	නම් (len == 0)
	    ආපසු FALSE;
	tv.v_type = VAR_STRING;
	tv.v_lock = VAR_FIXED;
	tv.vval.v_string = vim_strnsave(fi->fi_string + fi->fi_byte_idx, len);
	fi->fi_byte_idx += len;
	++fi->fi_bi;
	නම් (skip_assign)
	    result = TRUE;
	නැතහොත්
	    result = ex_let_vars(arg, &tv, TRUE, fi->fi_semicolon,
					    fi->fi_varcount, flag, NULL) == OK;
	vim_free(tv.vval.v_string);
	ආපසු result;
    }

    නම් (fi->fi_tuple != NULL)
    {
	typval_T	tv;

	නම් (fi->fi_tuple_idx >= TUPLE_LEN(fi->fi_tuple))
	    ආපසු FALSE;

	copy_tv(TUPLE_ITEM(fi->fi_tuple, fi->fi_tuple_idx), &tv);
	++fi->fi_tuple_idx;
	++fi->fi_bi;
	නම් (skip_assign)
	    result = TRUE;
	නැතහොත්
	    result = ex_let_vars(arg, &tv, TRUE, fi->fi_semicolon,
					    fi->fi_varcount, flag, NULL) == OK;
	clear_tv(&tv);
	ආපසු result;
    }

    item = fi->fi_lw.lw_item;
    නම් (item == NULL)
	result = FALSE;
    නැතහොත්
    {
	fi->fi_lw.lw_item = item->li_next;
	++fi->fi_bi;
	නම් (skip_assign)
	    result = TRUE;
	නැතහොත්
	    result = (ex_let_vars(arg, &item->li_tv, TRUE, fi->fi_semicolon,
					   fi->fi_varcount, flag, NULL) == OK);
    }
    ආපසු result;
}

/*
 * Free the structure used to store info used by ":for".
 */
    void
free_for_info(void *fi_void)
{
    forinfo_T    *fi = (forinfo_T *)fi_void;

    නම් (fi == NULL)
	ආපසු;
    නම් (fi->fi_list != NULL)
    {
	list_rem_watch(fi->fi_list, &fi->fi_lw);
	list_unref(fi->fi_list);
    }
    නැතහොත් නම් (fi->fi_blob != NULL)
	blob_unref(fi->fi_blob);
    නැතහොත් නම් (fi->fi_tuple != NULL)
	tuple_unref(fi->fi_tuple);
    නැතහොත්
	vim_free(fi->fi_string);
    vim_free(fi);
}

    void
set_context_for_expression(
    expand_T	*xp,
    char_u	*arg,
    cmdidx_T	cmdidx)
{
    පූර්ණ_සංඛ්‍යා		has_expr = cmdidx != CMD_let && cmdidx != CMD_var;
    පූර්ණ_සංඛ්‍යා		c;
    char_u	*p;

    නම් (cmdidx == CMD_let || cmdidx == CMD_var
				 || cmdidx == CMD_const || cmdidx == CMD_final)
    {
	xp->xp_context = EXPAND_USER_VARS;
	නම් (vim_strpbrk(arg, (char_u *)"\"'+-*/%.=!?~|&$([<>,#") == NULL)
	{
	    // ":let var1 var2 ...": find last space.
	    සඳහා (p = arg + STRLEN(arg); p >= arg; )
	    {
		xp->xp_pattern = p;
		MB_PTR_BACK(arg, p);
		නම් (VIM_ISWHITE(*p))
		    bha_nga;
	    }
	    ආපසු;
	}
    }
    නැතහොත්
	xp->xp_context = cmdidx == CMD_call ? EXPAND_FUNCTIONS
							  : EXPAND_EXPRESSION;
    අතරතුර ((xp->xp_pattern = vim_strpbrk(arg,
				  (char_u *)"\"'+-*/%.=!?~|&$([<>,#")) != NULL)
    {
	c = *xp->xp_pattern;
	නම් (c == '&')
	{
	    c = xp->xp_pattern[1];
	    නම් (c == '&')
	    {
		++xp->xp_pattern;
		xp->xp_context = has_expr ? EXPAND_EXPRESSION : EXPAND_NOTHING;
	    }
	    නැතහොත් නම් (c != ' ')
	    {
		xp->xp_context = EXPAND_SETTINGS;
		නම් ((c == 'l' || c == 'g') && xp->xp_pattern[2] == ':')
		    xp->xp_pattern += 2;

	    }
	}
	නැතහොත් නම් (c == '$')
	{
	    // environment variable
	    xp->xp_context = EXPAND_ENV_VARS;
	}
	නැතහොත් නම් (c == '=')
	{
	    has_expr = TRUE;
	    xp->xp_context = EXPAND_EXPRESSION;
	}
	නැතහොත් නම් (c == '#'
		&& xp->xp_context == EXPAND_EXPRESSION)
	{
	    // Autoload function/variable contains '#'.
	    bha_nga;
	}
	නැතහොත් නම් ((c == '<' || c == '#')
		&& xp->xp_context == EXPAND_FUNCTIONS
		&& vim_strchr(xp->xp_pattern, '(') == NULL)
	{
	    // Function name can start with "<SNR>" and contain '#'.
	    bha_nga;
	}
	නැතහොත් නම් (has_expr)
	{
	    නම් (c == '"')	    // string
	    {
		අතරතුර ((c = *++xp->xp_pattern) != NUL && c != '"')
		    නම් (c == '\\' && xp->xp_pattern[1] != NUL)
			++xp->xp_pattern;
		xp->xp_context = EXPAND_NOTHING;
	    }
	    නැතහොත් නම් (c == '\'')	    // literal string
	    {
		// Trick: '' is like stopping and starting a literal string.
		අතරතුර ((c = *++xp->xp_pattern) != NUL && c != '\'')
		    /* skip */ ;
		xp->xp_context = EXPAND_NOTHING;
	    }
	    නැතහොත් නම් (c == '|')
	    {
		නම් (xp->xp_pattern[1] == '|')
		{
		    ++xp->xp_pattern;
		    xp->xp_context = EXPAND_EXPRESSION;
		}
		නැතහොත්
		    xp->xp_context = EXPAND_COMMANDS;
	    }
	    නැතහොත්
		xp->xp_context = EXPAND_EXPRESSION;
	}
	නැතහොත්
	    // Doesn't look like something valid, expand as an expression
	    // anyway.
	    xp->xp_context = EXPAND_EXPRESSION;
	arg = xp->xp_pattern;
	නම් (*arg != NUL)
	    අතරතුර ((c = *++arg) != NUL && (c == ' ' || c == '\t'))
		/* skip */ ;
    }

    // ":exe one two" completes "two"
    නම් ((cmdidx == CMD_execute
		|| cmdidx == CMD_echo
		|| cmdidx == CMD_echon
		|| cmdidx == CMD_echomsg
		|| cmdidx == CMD_echoerr
		|| cmdidx == CMD_echoconsole
		|| cmdidx == CMD_echowindow)
	    && xp->xp_context == EXPAND_EXPRESSION)
    {
	සඳහා (;;)
	{
	    char_u *n = skiptowhite(arg);

	    නම් (n == arg || IS_WHITE_OR_NUL(*skipwhite(n)))
		bha_nga;
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
static regprog_T    *eval_prog_cache = NULL;
static char_u	    *eval_prog_pat = NULL;	// pattern it was compiled for
static char_u	    *eval_prog_enc = NULL;	// 'encoding' when compiled
static long	    eval_prog_re;		// 'regexpengine' when compiled
// State sampled by eval_regcomp() for the eval_regfree() call that gives the
// program back: user code run in between, e.g. an object's string() method
// invoked by match() on a list, can change these options, and then the
// program must not be cached under the changed values.  eval_compile_enc is
// only ever compared against p_enc, never dereferenced.
static long	    eval_compile_re;
static char_u	    *eval_compile_enc;
static පූර්ණ_සංඛ්‍යා	    eval_prog_busy = 0;	// programs eval_regcomp() handed out
					// that were not given back yet
static පූර්ණ_සංඛ්‍යා	    eval_prog_reentered = FALSE;  // eval_regcomp() ran while
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
    static bool
eval_prog_volatile(char_u *pat)
{
    char_u *p;

    නම් (STRNCMP(pat, "\\%#=", 4) == 0)
	ආපසු true;
    සඳහා (p = pat; *p != NUL; ++p)
	නම් (*p == '~'
		|| (p[0] == '[' && (p[1] == ':' || p[1] == '=' || p[1] == '.'))
		|| (p[0] == '%' && (p[1] == '.'
			|| ((p[1] == '<' || p[1] == '>') && p[2] == '.'))))
	    ආපසු true;
    ආපසු false;
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
    නම් (eval_prog_busy > 0)
	// The samples above no longer describe the program already handed
	// out: its eval_regfree() must drop it instead of caching it.
	eval_prog_reentered = TRUE;

    නම් (eval_prog_cache != NULL
	    && *p_cpo == NUL
	    && eval_prog_cache->re_flags == RE_MAGIC + RE_STRING
	    && eval_prog_re == p_re
	    && STRCMP(eval_prog_pat, pat) == 0
	    && STRCMP(eval_prog_enc, p_enc) == 0)
    {
	prog = eval_prog_cache;
	// The caller now owns the program, eval_regfree() adopts the
	// program the caller ends up with.
	eval_prog_cache = NULL;
    }
    නැතහොත්
	prog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
    නම් (prog != NULL)
	++eval_prog_busy;	// paired with the eval_regfree() call
    ආපසු prog;
}

/*
 * Free program "prog", obtained with eval_regcomp() for pattern "pat", by
 * keeping it in the cache for the next use.  "pat" must be the same string
 * the program was compiled for and must still be valid.
 */
    void
eval_regfree(char_u *pat, regprog_T *prog)
{
    පූර්ණ_සංඛ්‍යා	    reentered = eval_prog_reentered;

    නම් (eval_prog_busy > 0 && --eval_prog_busy == 0)
	eval_prog_reentered = FALSE;
    නම් (prog == NULL)
	ආපසු;
    නම් (reentered || *p_cpo != NUL
	    || eval_compile_re != p_re || eval_compile_enc != p_enc)
    {
	// 'cpoptions' is not empty, an option in the cache key changed, or
	// another program was handed out while the caller was using this
	// one: the samples may not describe this program, do not cache it.
	vim_regfree(prog);
	ආපසු;
    }
    නම් (eval_prog_pat == NULL || STRCMP(eval_prog_pat, pat) != 0
	    || STRCMP(eval_prog_enc, p_enc) != 0)
    {
	char_u	*pat_copy;
	char_u	*enc_copy;

	// A cached pattern is known not to be volatile, thus the scan is
	// only needed when installing a new key.
	නම් (eval_prog_volatile(pat))
	{
	    // compiling depends on state the cache key does not cover
	    vim_regfree(prog);
	    ආපසු;
	}
	pat_copy = vim_strsave(pat);
	enc_copy = vim_strsave(p_enc);
	නම් (pat_copy == NULL || enc_copy == NULL)
	{
	    vim_free(pat_copy);
	    vim_free(enc_copy);
	    vim_regfree(prog);
	    ආපසු;
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

#නම් defined(EXITFREE)
    void
free_eval_regcomp_cache(void)
{
    vim_regfree(eval_prog_cache);
    eval_prog_cache = NULL;
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
    පූර්ණ_සංඛ්‍යා
pattern_match(char_u *pat, char_u *text, පූර්ණ_සංඛ්‍යා ic)
{
    පූර්ණ_සංඛ්‍යා		matches = FALSE;
    char_u	*save_cpo;
    regmatch_T	regmatch;

    // avoid 'l' flag in 'cpoptions'
    save_cpo = p_cpo;
    p_cpo = empty_option;
    regmatch.regprog = eval_regcomp(pat);
    නම් (regmatch.regprog != NULL)
    {
	regmatch.rm_ic = ic;
	matches = vim_regexec_nl(&regmatch, text, (colnr_T)0);
	eval_regfree(pat, regmatch.regprog);
    }
    p_cpo = save_cpo;
    ආපසු matches;
}

/*
 * Handle a name followed by "(".  Both for just "name(arg)" and for
 * "expr->name(arg)".
 * Returns OK or FAIL.
 */
    static පූර්ණ_සංඛ්‍යා
eval_func(
	char_u	    **arg,	// points to "(", will be advanced
	evalarg_T   *evalarg,
	char_u	    *name,
	පූර්ණ_සංඛ්‍යා	    name_len,
	typval_T    *rettv,
	පූර්ණ_සංඛ්‍යා	    flags,
	typval_T    *basetv)	// "expr" for "expr->name(arg)"
{
    පූර්ණ_සංඛ්‍යා		evaluate = flags & EVAL_EVALUATE;
    char_u	*s = name;
    පූර්ණ_සංඛ්‍යා		len = name_len;
    partial_T	*partial;
    පූර්ණ_සංඛ්‍යා		ret = OK;
    type_T	*type = NULL;
    පූර්ණ_සංඛ්‍යා		found_var = FALSE;

    නම් (!evaluate)
	check_vars(s, len);

    // If "s" is the name of a variable of type VAR_FUNC
    // use its contents.
    s = deref_func_name(s, &len, &partial,
		 in_vim9script() ? &type : NULL, !evaluate, FALSE, &found_var);

    // Need to make a copy, in case evaluating the arguments makes
    // the name invalid.
    s = vim_strsave(s);
    නම් (s == NULL || (evaluate && *s == NUL))
	ret = FAIL;
    නැතහොත්
    {
	funcexe_T funcexe;

	// Invoke the function.
	CLEAR_FIELD(funcexe);
	funcexe.fe_firstline = curwin->w_cursor.lnum;
	funcexe.fe_lastline = curwin->w_cursor.lnum;
	funcexe.fe_evaluate = evaluate;
	funcexe.fe_partial = partial;
	නම් (partial != NULL)
	{
	    funcexe.fe_object = partial->pt_obj;
	    නම් (funcexe.fe_object != NULL)
		++funcexe.fe_object->obj_refcount;
	}
	funcexe.fe_basetv = basetv;
	funcexe.fe_check_type = type;
	funcexe.fe_found_var = found_var;
	නම් (evalarg != NULL)
	    funcexe.fe_cctx = evalarg->eval_cctx;
	ret = get_func_tv(s, len, rettv, arg, evalarg, &funcexe);
    }
    vim_free(s);

    // If evaluate is FALSE rettv->v_type was not set in
    // get_func_tv, but it's needed in handle_subscript() to parse
    // what follows. So set it here.
    නම් (rettv->v_type == VAR_UNKNOWN && !evaluate && **arg == '(')
    {
	rettv->vval.v_string = NULL;
	rettv->v_type = VAR_FUNC;
    }

    // Stop the expression evaluation when immediately
    // aborting on error, or when an interrupt occurred or
    // an exception was thrown but not caught.
    නම් (evaluate && aborting())
    {
	නම් (ret == OK)
	    clear_tv(rettv);
	ret = FAIL;
    }
    ආපසු ret;
}

/*
 * After a NL, skip over empty lines and comment-only lines.
 */
    static char_u *
newline_skip_comments(char_u *arg)
{
    char_u *p = arg + 1;

    සඳහා (;;)
    {
	p = skipwhite(p);

	නම් (*p == NUL)
	    bha_nga;
	නම් (vim9_comment_start(p))
	{
	    char_u *nl = vim_strchr(p, NL);

	    නම් (nl == NULL)
		bha_nga;
	    p = nl;
	}
	නම් (*p != NL)
	    bha_nga;
	++p;  // skip another NL
    }
    ආපසු p;
}

/*
 * Get the next line source line without advancing.  But do skip over comment
 * lines.
 * Only called for Vim9 script.
 */
    static char_u *
getline_peek_skip_comments(evalarg_T *evalarg)
{
    සඳහා (;;)
    {
	char_u *next = getline_peek(evalarg->eval_getline,
							 evalarg->eval_cookie);
	char_u *p;

	නම් (next == NULL)
	    bha_nga;
	p = skipwhite(next);
	නම් (*p != NUL && !vim9_comment_start(p))
	    ආපසු next;
	නම් (eval_next_line(NULL, evalarg) == NULL)
	    bha_nga;
    }
    ආපසු NULL;
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
eval_next_non_blank(char_u *arg, evalarg_T *evalarg, පූර්ණ_සංඛ්‍යා *getnext)
{
    char_u *p = skipwhite(arg);

    *getnext = FALSE;
    නම් (in_vim9script()
	    && evalarg != NULL
	    && (evalarg->eval_cookie != NULL || evalarg->eval_cctx != NULL
								   || *p == NL)
	    && (*p == NUL || *p == NL
			     || (vim9_comment_start(p) && VIM_ISWHITE(p[-1]))))
    {
	char_u *next;

	නම් (*p == NL)
	    next = newline_skip_comments(p);
	නැතහොත් නම් (evalarg->eval_cookie != NULL)
	    next = getline_peek_skip_comments(evalarg);
	නැතහොත්
	    next = peek_next_line_from_context(evalarg->eval_cctx);

	නම් (next != NULL)
	{
	    *getnext = *p != NL;
	    ආපසු skipwhite(next);
	}
    }
    ආපසු p;
}

/*
 * To be called after eval_next_non_blank() sets "getnext" to TRUE.
 * Only called for Vim9 script.
 *
 * If "arg" is not NULL, then the caller should assign the return value to
 * "arg".
 */
    char_u *
eval_next_line(char_u *arg, evalarg_T *evalarg)
{
    garray_T	*gap = &evalarg->eval_ga;
    char_u	*line;

    නම් (arg != NULL)
    {
	නම් (*arg == NL)
	    ආපසු newline_skip_comments(arg);
	// Truncate before a trailing comment, so that concatenating the lines
	// won't turn the rest into a comment.
	නම් (*skipwhite(arg) == '#')
	    *arg = NUL;
    }

    නම් (evalarg->eval_cookie != NULL)
	line = evalarg->eval_getline(0, evalarg->eval_cookie, 0,
							   GETLINE_CONCAT_ALL);
    නැතහොත්
	line = next_line_from_context(evalarg->eval_cctx, TRUE);
    නම් (line == NULL)
	ආපසු NULL;

    ++evalarg->eval_break_count;
    නම් (gap->ga_itemsize > 0 && ga_grow(gap, 1) == OK)
    {
	char_u *p = skipwhite(line);

	// Going to concatenate the lines after parsing.  For an empty or
	// comment line use an empty string.
	නම් (*p == NUL || vim9_comment_start(p))
	{
	    vim_free(line);
	    line = vim_strsave((char_u *)"");
	}

	((char_u **)gap->ga_data)[gap->ga_len] = line;
	++gap->ga_len;
    }
    නැතහොත් නම් (evalarg->eval_cookie != NULL)
    {
	free_eval_tofree_later(evalarg);
	evalarg->eval_tofree = line;
    }

    // Advanced to the next line, "arg" no longer points into the previous
    // line.  The caller assigns the return value to "arg".
    // If "arg" is NULL, then the return value is discarded.  In that case,
    // "arg" still points to the previous line.  So don't reset
    // "eval_using_cmdline".
    නම් (arg != NULL)
	evalarg->eval_using_cmdline = FALSE;
    ආපසු skipwhite(line);
}

/*
 * Call eval_next_non_blank() and get the next line if needed.
 */
    char_u *
skipwhite_and_linebreak(char_u *arg, evalarg_T *evalarg)
{
    පූර්ණ_සංඛ්‍යා	    getnext;
    char_u  *p = skipwhite_and_nl(arg);

    නම් (evalarg == NULL)
	ආපසු skipwhite(arg);
    eval_next_non_blank(p, evalarg, &getnext);
    නම් (getnext)
	ආපසු eval_next_line(arg, evalarg);
    ආපසු p;
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
    පූර්ණ_සංඛ්‍යා
eval0(
    char_u	*arg,
    typval_T	*rettv,
    exarg_T	*eap,
    evalarg_T	*evalarg)
{
    ආපසු eval0_retarg(arg, rettv, eap, evalarg, NULL);
}

/*
 * If "arg" is a simple function call without arguments then call it and return
 * the result.  Otherwise return NOTDONE.
 */
    පූර්ණ_සංඛ්‍යා
may_call_simple_func(
    char_u	*arg,
    typval_T	*rettv)
{
    char_u  *parens = (char_u *)strstr((var_na *)arg, "()");
    පූර්ණ_සංඛ්‍යා	    r = NOTDONE;

    // If the expression is "FuncName()" then we can skip a lot of overhead.
    නම් (parens != NULL && *skipwhite(parens + 2) == NUL)
    {
	char_u *p = STRNCMP(arg, "<SNR>", 5) == 0 ? skipdigits(arg + 5) : arg;

	නම් (to_name_end(p, TRUE) == parens)
	    r = call_simple_func(arg, (size_t)(parens - arg), rettv);
    }
    ආපසු r;
}

/*
 * Handle zero level expression with optimization for a simple function call.
 * Same arguments and return value as eval0().
 */
    static පූර්ණ_සංඛ්‍යා
eval0_simple_funccal(
    char_u	*arg,
    typval_T	*rettv,
    exarg_T	*eap,
    evalarg_T	*evalarg)
{
    පූර්ණ_සංඛ්‍යා	    r = may_call_simple_func(arg, rettv);

    නම් (r == NOTDONE)
	r = eval0_retarg(arg, rettv, eap, evalarg, NULL);
    ආපසු r;
}

/*
 * Like eval0() but when "retarg" is not NULL store the pointer to after the
 * expression and don't check what comes after the expression.
 */
    පූර්ණ_සංඛ්‍යා
eval0_retarg(
    char_u	*arg,
    typval_T	*rettv,
    exarg_T	*eap,
    evalarg_T	*evalarg,
    char_u	**retarg)
{
    පූර්ණ_සංඛ්‍යා		ret;
    char_u	*p;
    char_u	*expr_end;
    පූර්ණ_සංඛ්‍යා		did_emsg_before = did_emsg;
    පූර්ණ_සංඛ්‍යා		called_emsg_before = called_emsg;
    පූර්ණ_සංඛ්‍යා		check_for_end = retarg == NULL;
    පූර්ණ_සංඛ්‍යා		end_error = FALSE;

    p = skipwhite(arg);
    ret = eval1(&p, rettv, evalarg);

    නම් (ret != FAIL)
    {
	expr_end = p;
	p = skipwhite(p);

	// In Vim9 script a command block is not split at NL characters for
	// commands using an expression argument.  Skip over a '#' comment to
	// check for a following NL.  Require white space before the '#'.
	නම් (in_vim9script() && p > expr_end && retarg == NULL)
	    අතරතුර (*p == '#')
	    {
		char_u *nl = vim_strchr(p, NL);

		නම් (nl == NULL)
		    bha_nga;
		p = skipwhite(nl + 1);
		නම් (eap != NULL && *p != NUL)
		    eap->nextcmd = p;
		check_for_end = FALSE;
	    }

	නම් (check_for_end)
	    end_error = !ends_excmd2(arg, p);
    }

    නම් (ret == FAIL || end_error)
    {
	නම් (ret != FAIL)
	    clear_tv(rettv);
	/*
	 * Report the invalid expression unless the expression evaluation has
	 * been cancelled due to an aborting error, an interrupt, or an
	 * exception, or we already gave a more specific error.
	 * Also check called_emsg for when using assert_fails().
	 */
	නම් (!aborting()
		&& did_emsg == did_emsg_before
		&& called_emsg == called_emsg_before
		&& (!in_vim9script() || !vim9_bad_comment(p)))
	{
	    නම් (end_error)
		semsg(_(e_trailing_characters_str), p);
	    නැතහොත්
		semsg(_(e_invalid_expression_str), arg);
	}

	නම් (eap != NULL && p != NULL)
	{
	    // Some of the expression may not have been consumed.
	    // Only execute a next command if it cannot be a "||" operator.
	    // The next command may be "catch".
	    char_u *nextcmd = check_nextcmd(p);
	    නම් (nextcmd != NULL && *nextcmd != '|')
		eap->nextcmd = nextcmd;
	}
	ආපසු FAIL;
    }

    නම් (retarg != NULL)
	*retarg = p;
    නැතහොත් නම් (check_for_end && eap != NULL)
	set_nextcmd(eap, p);

    ආපසු ret;
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
    පූර්ණ_සංඛ්‍යා
eval1(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    char_u  *p;
    පූර්ණ_සංඛ්‍යා	    getnext;

    CLEAR_POINTER(rettv);

    /*
     * Get the first variable.
     */
    නම් (eval2(arg, rettv, evalarg) == FAIL)
	ආපසු FAIL;

    p = eval_next_non_blank(*arg, evalarg, &getnext);
    නම් (*p == '?')
    {
	පූර්ණ_සංඛ්‍යා		op_falsy = p[1] == '?';
	පූර්ණ_සංඛ්‍යා		result;
	typval_T	var2;
	evalarg_T	*evalarg_used = evalarg;
	evalarg_T	local_evalarg;
	පූර්ණ_සංඛ්‍යා		orig_flags;
	පූර්ණ_සංඛ්‍යා		evaluate;
	පූර්ණ_සංඛ්‍යා		vim9script = in_vim9script();

	නම් (evalarg == NULL)
	{
	    init_evalarg(&local_evalarg);
	    evalarg_used = &local_evalarg;
	}
	orig_flags = evalarg_used->eval_flags;
	evaluate = evalarg_used->eval_flags & EVAL_EVALUATE;

	නම් (getnext)
	    *arg = eval_next_line(*arg, evalarg_used);
	නැතහොත්
	{
	    නම් (evaluate && vim9script && !VIM_ISWHITE(p[-1]))
	    {
		error_white_both(p, op_falsy ? 2 : 1);
		clear_tv(rettv);
		ආපසු FAIL;
	    }
	    *arg = p;
	}

	result = FALSE;
	නම් (evaluate)
	{
	    පූර්ණ_සංඛ්‍යා		error = FALSE;

	    නම් (op_falsy)
	    {
		// Is this typeval supported with the falsy operator?
		නම් (check_typval_is_value(rettv) == FAIL)
		{
		    clear_tv(rettv);
		    ආපසු FAIL;
		}
		result = tv2bool(rettv);
	    }
	    නැතහොත් නම් (vim9script)
		result = tv_get_bool_chk(rettv, &error);
	    නැතහොත් නම් (tv_get_number_chk(rettv, &error) != 0)
		result = TRUE;
	    නම් (error || !op_falsy || !result)
		clear_tv(rettv);
	    නම් (error)
		ආපසු FAIL;
	}

	/*
	 * Get the second variable.  Recursive!
	 */
	නම් (op_falsy)
	    ++*arg;
	නම් (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[1]))
	{
	    error_white_both(*arg - (op_falsy ? 1 : 0), op_falsy ? 2 : 1);
	    clear_tv(rettv);
	    ආපසු FAIL;
	}
	*arg = skipwhite_and_linebreak(*arg + 1, evalarg_used);
	evalarg_used->eval_flags = (op_falsy ? !result : result)
				  ? orig_flags : (orig_flags & ~EVAL_EVALUATE);
	නම් (eval1(arg, &var2, evalarg_used) == FAIL)
	{
	    evalarg_used->eval_flags = orig_flags;
	    ආපසු FAIL;
	}
	නම් (!op_falsy || !result)
	    *rettv = var2;

	නම් (!op_falsy)
	{
	    /*
	     * Check for the ":".
	     */
	    p = eval_next_non_blank(*arg, evalarg_used, &getnext);
	    නම් (*p != ':')
	    {
		emsg(_(e_missing_colon_after_questionmark));
		නම් (evaluate && result)
		    clear_tv(rettv);
		evalarg_used->eval_flags = orig_flags;
		ආපසු FAIL;
	    }
	    නම් (getnext)
		*arg = eval_next_line(*arg, evalarg_used);
	    නැතහොත්
	    {
		නම් (evaluate && vim9script && !VIM_ISWHITE(p[-1]))
		{
		    error_white_both(p, 1);
		    clear_tv(rettv);
		    evalarg_used->eval_flags = orig_flags;
		    ආපසු FAIL;
		}
		*arg = p;
	    }

	    /*
	     * Get the third variable.  Recursive!
	     */
	    නම් (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[1]))
	    {
		error_white_both(*arg, 1);
		clear_tv(rettv);
		evalarg_used->eval_flags = orig_flags;
		ආපසු FAIL;
	    }
	    *arg = skipwhite_and_linebreak(*arg + 1, evalarg_used);
	    evalarg_used->eval_flags = !result ? orig_flags
					       : (orig_flags & ~EVAL_EVALUATE);
	    නම් (eval1(arg, &var2, evalarg_used) == FAIL)
	    {
		නම් (evaluate && result)
		    clear_tv(rettv);
		evalarg_used->eval_flags = orig_flags;
		ආපසු FAIL;
	    }
	    නම් (evaluate && !result)
		*rettv = var2;
	}

	නම් (evalarg == NULL)
	    clear_evalarg(&local_evalarg, NULL);
	නැතහොත්
	    evalarg->eval_flags = orig_flags;
    }

    ආපසු OK;
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
    static පූර්ණ_සංඛ්‍යා
eval2(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    char_u	*p;
    පූර්ණ_සංඛ්‍යා		getnext;

    /*
     * Get the first expression.
     */
    නම් (eval3(arg, rettv, evalarg) == FAIL)
	ආපසු FAIL;

    /*
     * Handle the  "||" operator.
     */
    p = eval_next_non_blank(*arg, evalarg, &getnext);
    නම් (p[0] == '|' && p[1] == '|')
    {
	evalarg_T   *evalarg_used = evalarg;
	evalarg_T   local_evalarg;
	පූර්ණ_සංඛ්‍යා	    evaluate;
	පූර්ණ_සංඛ්‍යා	    orig_flags;
	long	    result = FALSE;
	typval_T    var2;
	පූර්ණ_සංඛ්‍යා	    error = FALSE;
	පූර්ණ_සංඛ්‍යා	    vim9script = in_vim9script();

	නම් (evalarg == NULL)
	{
	    init_evalarg(&local_evalarg);
	    evalarg_used = &local_evalarg;
	}
	orig_flags = evalarg_used->eval_flags;
	evaluate = orig_flags & EVAL_EVALUATE;
	නම් (evaluate)
	{
	    නම් (vim9script)
		result = tv_get_bool_chk(rettv, &error);
	    නැතහොත් නම් (tv_get_number_chk(rettv, &error) != 0)
		result = TRUE;
	    clear_tv(rettv);
	    නම් (error)
		ආපසු FAIL;
	}

	/*
	 * Repeat until there is no following "||".
	 */
	අතරතුර (p[0] == '|' && p[1] == '|')
	{
	    නම් (getnext)
		*arg = eval_next_line(*arg, evalarg_used);
	    නැතහොත්
	    {
		නම් (evaluate && vim9script && !VIM_ISWHITE(p[-1]))
		{
		    error_white_both(p, 2);
		    clear_tv(rettv);
		    ආපසු FAIL;
		}
		*arg = p;
	    }

	    /*
	     * Get the second variable.
	     */
	    නම් (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[2]))
	    {
		error_white_both(*arg, 2);
		clear_tv(rettv);
		ආපසු FAIL;
	    }
	    *arg = skipwhite_and_linebreak(*arg + 2, evalarg_used);
	    evalarg_used->eval_flags = !result ? orig_flags
					       : (orig_flags & ~EVAL_EVALUATE);
	    නම් (eval3(arg, &var2, evalarg_used) == FAIL)
		ආපසු FAIL;

	    /*
	     * Compute the result.
	     */
	    නම් (evaluate && !result)
	    {
		නම් (vim9script)
		    result = tv_get_bool_chk(&var2, &error);
		නැතහොත් නම් (tv_get_number_chk(&var2, &error) != 0)
		    result = TRUE;
		clear_tv(&var2);
		නම් (error)
		    ආපසු FAIL;
	    }
	    නම් (evaluate)
	    {
		නම් (vim9script)
		{
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_number = result ? VVAL_TRUE : VVAL_FALSE;
		}
		නැතහොත්
		{
		    rettv->v_type = VAR_NUMBER;
		    rettv->vval.v_number = result;
		}
	    }

	    p = eval_next_non_blank(*arg, evalarg_used, &getnext);
	}

	නම් (evalarg == NULL)
	    clear_evalarg(&local_evalarg, NULL);
	නැතහොත්
	    evalarg->eval_flags = orig_flags;
    }

    ආපසු OK;
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
    static පූර්ණ_සංඛ්‍යා
eval3(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    char_u	*p;
    පූර්ණ_සංඛ්‍යා		getnext;

    /*
     * Get the first expression.
     */
    නම් (eval4(arg, rettv, evalarg) == FAIL)
	ආපසු FAIL;

    /*
     * Handle the "&&" operator.
     */
    p = eval_next_non_blank(*arg, evalarg, &getnext);
    නම් (p[0] == '&' && p[1] == '&')
    {
	evalarg_T   *evalarg_used = evalarg;
	evalarg_T   local_evalarg;
	පූර්ණ_සංඛ්‍යා	    orig_flags;
	පූර්ණ_සංඛ්‍යා	    evaluate;
	long	    result = TRUE;
	typval_T    var2;
	පූර්ණ_සංඛ්‍යා	    error = FALSE;
	පූර්ණ_සංඛ්‍යා	    vim9script = in_vim9script();

	නම් (evalarg == NULL)
	{
	    init_evalarg(&local_evalarg);
	    evalarg_used = &local_evalarg;
	}
	orig_flags = evalarg_used->eval_flags;
	evaluate = orig_flags & EVAL_EVALUATE;
	නම් (evaluate)
	{
	    නම් (vim9script)
		result = tv_get_bool_chk(rettv, &error);
	    නැතහොත් නම් (tv_get_number_chk(rettv, &error) == 0)
		result = FALSE;
	    clear_tv(rettv);
	    නම් (error)
		ආපසු FAIL;
	}

	/*
	 * Repeat until there is no following "&&".
	 */
	අතරතුර (p[0] == '&' && p[1] == '&')
	{
	    නම් (getnext)
		*arg = eval_next_line(*arg, evalarg_used);
	    නැතහොත්
	    {
		නම් (evaluate && vim9script && !VIM_ISWHITE(p[-1]))
		{
		    error_white_both(p, 2);
		    clear_tv(rettv);
		    ආපසු FAIL;
		}
		*arg = p;
	    }

	    /*
	     * Get the second variable.
	     */
	    නම් (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[2]))
	    {
		error_white_both(*arg, 2);
		clear_tv(rettv);
		ආපසු FAIL;
	    }
	    *arg = skipwhite_and_linebreak(*arg + 2, evalarg_used);
	    evalarg_used->eval_flags = result ? orig_flags
					      : (orig_flags & ~EVAL_EVALUATE);
	    CLEAR_FIELD(var2);
	    නම් (eval4(arg, &var2, evalarg_used) == FAIL)
		ආපසු FAIL;

	    /*
	     * Compute the result.
	     */
	    නම් (evaluate && result)
	    {
		නම් (vim9script)
		    result = tv_get_bool_chk(&var2, &error);
		නැතහොත් නම් (tv_get_number_chk(&var2, &error) == 0)
		    result = FALSE;
		clear_tv(&var2);
		නම් (error)
		    ආපසු FAIL;
	    }
	    නම් (evaluate)
	    {
		නම් (vim9script)
		{
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_number = result ? VVAL_TRUE : VVAL_FALSE;
		}
		නැතහොත්
		{
		    rettv->v_type = VAR_NUMBER;
		    rettv->vval.v_number = result;
		}
	    }

	    p = eval_next_non_blank(*arg, evalarg_used, &getnext);
	}

	නම් (evalarg == NULL)
	    clear_evalarg(&local_evalarg, NULL);
	නැතහොත්
	    evalarg->eval_flags = orig_flags;
    }

    ආපසු OK;
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
    static පූර්ණ_සංඛ්‍යා
eval4(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    char_u	*p;
    පූර්ණ_සංඛ්‍යා		getnext;
    exprtype_T	type = EXPR_UNKNOWN;
    පූර්ණ_සංඛ්‍යා		len = 2;
    පූර්ණ_සංඛ්‍යා		type_is = FALSE;

    /*
     * Get the first expression.
     */
    නම් (eval5(arg, rettv, evalarg) == FAIL)
	ආපසු FAIL;

    p = eval_next_non_blank(*arg, evalarg, &getnext);

    type = get_compare_type(p, &len, &type_is);

    /*
     * If there is a comparative operator, use it.
     */
    නම් (type != EXPR_UNKNOWN)
    {
	typval_T    var2;
	පූර්ණ_සංඛ්‍යා	    ic;
	පූර්ණ_සංඛ්‍යා	    vim9script = in_vim9script();
	පූර්ණ_සංඛ්‍යා	    evaluate = evalarg == NULL
				   ? 0 : (evalarg->eval_flags & EVAL_EVALUATE);
	long	    comp_lnum = SOURCING_LNUM;

	නම් (getnext)
	{
	    *arg = eval_next_line(*arg, evalarg);
	    p = *arg;
	}
	නැතහොත් නම් (evaluate && vim9script && !VIM_ISWHITE(**arg))
	{
	    error_white_both(*arg, len);
	    clear_tv(rettv);
	    ආපසු FAIL;
	}

	නම් (vim9script && type_is && (p[len] == '?' || p[len] == '#'))
	{
	    semsg(_(e_invalid_expression_str), p);
	    clear_tv(rettv);
	    ආපසු FAIL;
	}

	// extra question mark appended: ignore case
	නම් (p[len] == '?')
	{
	    ic = TRUE;
	    ++len;
	}
	// extra '#' appended: match case
	නැතහොත් නම් (p[len] == '#')
	{
	    ic = FALSE;
	    ++len;
	}
	// nothing appended: use 'ignorecase' if not in Vim script
	නැතහොත්
	    ic = vim9script ? FALSE : p_ic;

	/*
	 * Get the second variable.
	 */
	නම් (evaluate && vim9script && !IS_WHITE_OR_NUL(p[len]))
	{
	    error_white_both(p, len);
	    clear_tv(rettv);
	    ආපසු FAIL;
	}
	*arg = skipwhite_and_linebreak(p + len, evalarg);
	නම් (eval5(arg, &var2, evalarg) == FAIL)
	{
	    clear_tv(rettv);
	    ආපසු FAIL;
	}
	නම් (evaluate)
	{
	    පූර්ණ_සංඛ්‍යා ret;

	    // use the line of the comparison for messages
	    SOURCING_LNUM = comp_lnum;
	    නම් (vim9script && check_compare_types(type, rettv, &var2) == FAIL)
	    {
		ret = FAIL;
		clear_tv(rettv);
	    }
	    නැතහොත්
		ret = typval_compare(rettv, &var2, type, ic);
	    clear_tv(&var2);
	    ආපසු ret;
	}
    }

    ආපසු OK;
}

/*
 * Make a copy of blob "tv1" and append blob "tv2".
 */
    void
eval_addblob(typval_T *tv1, typval_T *tv2)
{
    blob_T  *b1 = tv1->vval.v_blob;
    blob_T  *b2 = tv2->vval.v_blob;
    blob_T  *b = blob_alloc();
    long    len1, len2;
    long    totallen;

    නම් (b == NULL)
	ආපසු;

    len1 = blob_len(b1);
    len2 = blob_len(b2);
    totallen = len1 + len2;

    නම් (totallen > 0 && totallen <= INT_MAX
				    && ga_grow(&b->bv_ga, (පූර්ණ_සංඛ්‍යා)totallen) == OK)
    {
	නම් (len1 > 0)
	    mch_memmove((char_u *)b->bv_ga.ga_data,
		    (char_u *)b1->bv_ga.ga_data, (size_t)len1);
	නම් (len2 > 0)
	    mch_memmove((char_u *)b->bv_ga.ga_data + len1,
		    (char_u *)b2->bv_ga.ga_data, (size_t)len2);
	b->bv_ga.ga_len = (පූර්ණ_සංඛ්‍යා)totallen;
    }

    clear_tv(tv1);
    rettv_blob_set(tv1, b);
}

/*
 * Make a copy of list "tv1" and append list "tv2".
 */
    පූර්ණ_සංඛ්‍යා
eval_addlist(typval_T *tv1, typval_T *tv2)
{
    typval_T var3;

    // concatenate Lists
    නම් (list_concat(tv1->vval.v_list, tv2->vval.v_list, &var3) == FAIL)
    {
	clear_tv(tv1);
	clear_tv(tv2);
	ආපසු FAIL;
    }
    clear_tv(tv1);
    *tv1 = var3;
    ආපසු OK;
}

/*
 * Make a copy of tuple "tv1" and append tuple "tv2".
 */
    පූර්ණ_සංඛ්‍යා
eval_addtuple(typval_T *tv1, typval_T *tv2)
{
    පූර්ණ_සංඛ්‍යා		vim9script = in_vim9script();
    typval_T	var3;

    නම් (vim9script && tv1->vval.v_tuple != NULL && tv2->vval.v_tuple != NULL
	    && tv1->vval.v_tuple->tv_type != NULL
	    && tv2->vval.v_tuple->tv_type != NULL)
    {
	නම් (!check_tuples_addable(tv1->vval.v_tuple->tv_type,
						tv2->vval.v_tuple->tv_type))
	    ආපසු FAIL;
    }

    // concatenate tuples
    නම් (tuple_concat(tv1->vval.v_tuple, tv2->vval.v_tuple, &var3) == FAIL)
    {
	clear_tv(tv1);
	clear_tv(tv2);
	ආපසු FAIL;
    }
    clear_tv(tv1);
    *tv1 = var3;
    ආපසු OK;
}

/*
 * Left or right shift the number "tv1" by the number "tv2" and store the
 * result in "tv1".
 *
 * Return OK or FAIL.
 */
    static පූර්ණ_සංඛ්‍යා
eval_shift_number(typval_T *tv1, typval_T *tv2, පූර්ණ_සංඛ්‍යා shift_type)
{
    නම් (tv2->v_type != VAR_NUMBER || tv2->vval.v_number < 0)
    {
	// right operand should be a positive number
	නම් (tv2->v_type != VAR_NUMBER)
	    emsg(_(e_bitshift_ops_must_be_number));
	නැතහොත්
	    emsg(_(e_bitshift_ops_must_be_positive));
	clear_tv(tv1);
	clear_tv(tv2);
	ආපසු FAIL;
    }

    නම් (tv2->vval.v_number > MAX_LSHIFT_BITS)
	// shifting more bits than we have always results in zero
	tv1->vval.v_number = 0;
    නැතහොත් නම් (shift_type == EXPR_LSHIFT)
	tv1->vval.v_number =
	    (uvarnumber_T)tv1->vval.v_number << tv2->vval.v_number;
    නැතහොත්
	tv1->vval.v_number =
	    (uvarnumber_T)tv1->vval.v_number >> tv2->vval.v_number;

    ආපසු OK;
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
    static පූර්ණ_සංඛ්‍යා
eval5(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    /*
     * Get the first expression.
     */
    නම් (eval6(arg, rettv, evalarg) == FAIL)
	ආපසු FAIL;

    /*
     * Repeat computing, until no '<<' or '>>' is following.
     */
    සඳහා (;;)
    {
	char_u		*p;
	පූර්ණ_සංඛ්‍යා		getnext;
	exprtype_T	exprtype;
	පූර්ණ_සංඛ්‍යා		evaluate;
	typval_T	var2;
	පූර්ණ_සංඛ්‍යා		vim9script;

	p = eval_next_non_blank(*arg, evalarg, &getnext);
	නම් (p[0] == '<' && p[1] == '<')
	    exprtype = EXPR_LSHIFT;
	නැතහොත් නම් (p[0] == '>' && p[1] == '>')
	    exprtype = EXPR_RSHIFT;
	නැතහොත්
	    ආපසු OK;

	// Handle a bitwise left or right shift operator
	evaluate = evalarg == NULL ? 0 : (evalarg->eval_flags & EVAL_EVALUATE);
	නම් (evaluate && rettv->v_type != VAR_NUMBER)
	{
	    // left operand should be a number
	    emsg(_(e_bitshift_ops_must_be_number));
	    clear_tv(rettv);
	    ආපසු FAIL;
	}

	vim9script = in_vim9script();
	නම් (getnext)
	{
	    *arg = eval_next_line(*arg, evalarg);
	    p = *arg;
	}
	නැතහොත් නම් (evaluate && vim9script && !VIM_ISWHITE(**arg))
	{
	    error_white_both(*arg, 2);
	    clear_tv(rettv);
	    ආපසු FAIL;
	}

	/*
	 * Get the second variable.
	 */
	නම් (evaluate && vim9script && !IS_WHITE_OR_NUL(p[2]))
	{
	    error_white_both(p, 2);
	    clear_tv(rettv);
	    ආපසු FAIL;
	}
	*arg = skipwhite_and_linebreak(p + 2, evalarg);
	නම් (eval6(arg, &var2, evalarg) == FAIL)
	{
	    clear_tv(rettv);
	    ආපසු FAIL;
	}

	නම් (evaluate)
	{
	    නම් (eval_shift_number(rettv, &var2, exprtype) == FAIL)
		ආපසු FAIL;
	}

	clear_tv(&var2);
    }

    ආපසු OK;
}

/*
 * Concatenate strings "tv1" and "tv2" and store the result in "tv1".
 */
    static පූර්ණ_සංඛ්‍යා
eval_concat_str(typval_T *tv1, typval_T *tv2)
{
    char_u	buf1[NUMBUFLEN], buf2[NUMBUFLEN];
    char_u	*s1 = tv_get_string_buf(tv1, buf1);
    char_u	*s2 = NULL;
    char_u	*p;
    පූර්ණ_සංඛ්‍යා		vim9script = in_vim9script();

    නම් (vim9script && (tv2->v_type == VAR_VOID
		|| tv2->v_type == VAR_CHANNEL
		|| tv2->v_type == VAR_JOB))
	semsg(_(e_using_invalid_value_as_string_str),
		vartype_name(tv2->v_type));
    නැතහොත් නම් (vim9script && tv2->v_type == VAR_FLOAT)
    {
	vim_snprintf((var_na *)buf2, NUMBUFLEN, "%g",
		tv2->vval.v_float);
	s2 = buf2;
    }
    නැතහොත්
	s2 = tv_get_string_buf_chk(tv2, buf2);
    නම් (s2 == NULL)		// type error ?
    {
	clear_tv(tv1);
	clear_tv(tv2);
	ආපසු FAIL;
    }

    // When possible, grow the existing string in place to avoid alloc/free.
    නම් (grow_string_tv(tv1, s2) == OK)
	ආපසු OK;

    p = concat_str(s1, s2);
    clear_tv(tv1);
    tv1->v_type = VAR_STRING;
    tv1->vval.v_string = p;

    ආපසු OK;
}

/*
 * Add or subtract numbers "tv1" and "tv2" and store the result in "tv1".
 * The numbers can be whole numbers or floats.
 */
    static පූර්ණ_සංඛ්‍යා
eval_addsub_number(typval_T *tv1, typval_T *tv2, පූර්ණ_සංඛ්‍යා op)
{
    පූර්ණ_සංඛ්‍යා		error = FALSE;
    varnumber_T	n1, n2;
    float_T	f1 = 0, f2 = 0;

    නම් (tv1->v_type == VAR_FLOAT)
    {
	f1 = tv1->vval.v_float;
	n1 = 0;
    }
    නැතහොත්
    {
	n1 = tv_get_number_chk(tv1, &error);
	නම් (error)
	{
	    // This can only happen for "list + non-list" or
	    // "blob + non-blob".  For "non-list + ..." or
	    // "something - ...", we returned before evaluating the
	    // 2nd operand.
	    clear_tv(tv1);
	    clear_tv(tv2);
	    ආපසු FAIL;
	}
	නම් (tv2->v_type == VAR_FLOAT)
	    f1 = n1;
    }
    නම් (tv2->v_type == VAR_FLOAT)
    {
	f2 = tv2->vval.v_float;
	n2 = 0;
    }
    නැතහොත්
    {
	n2 = tv_get_number_chk(tv2, &error);
	නම් (error)
	{
	    clear_tv(tv1);
	    clear_tv(tv2);
	    ආපසු FAIL;
	}
	නම් (tv1->v_type == VAR_FLOAT)
	    f2 = n2;
    }
    clear_tv(tv1);

    // If there is a float on either side the result is a float.
    නම් (tv1->v_type == VAR_FLOAT || tv2->v_type == VAR_FLOAT)
    {
	නම් (op == '+')
	    f1 = f1 + f2;
	නැතහොත්
	    f1 = f1 - f2;
	tv1->v_type = VAR_FLOAT;
	tv1->vval.v_float = f1;
    }
    නැතහොත්
    {
	නම් (op == '+')
	    n1 = n1 + n2;
	නැතහොත්
	    n1 = n1 - n2;
	tv1->v_type = VAR_NUMBER;
	tv1->vval.v_number = n1;
    }

    ආපසු OK;
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
    static පූර්ණ_සංඛ්‍යා
eval6(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    /*
     * Get the first expression.
     */
    නම් (eval7(arg, rettv, evalarg, FALSE) == FAIL)
	ආපසු FAIL;

    /*
     * Repeat computing, until no '+', '-' or '.' is following.
     */
    සඳහා (;;)
    {
	පූර්ණ_සංඛ්‍යා	    evaluate;
	පූර්ණ_සංඛ්‍යා	    getnext;
	char_u	    *p;
	පූර්ණ_සංඛ්‍යා	    op;
	පූර්ණ_සංඛ්‍යා	    oplen;
	පූර්ණ_සංඛ්‍යා	    concat;
	typval_T    var2;
	පූර්ණ_සංඛ්‍යා	    vim9script = in_vim9script();
	long	    op_lnum = SOURCING_LNUM;

	// "." is only string concatenation when scriptversion is 1
	// "+=", "-=" and "..=" are assignments
	// "++" and "--" on the next line are a separate command.
	p = eval_next_non_blank(*arg, evalarg, &getnext);
	op = *p;
	concat = op == '.' && (*(p + 1) == '.' || in_old_script(2));
	නම් ((op != '+' && op != '-' && !concat) || p[1] == '='
					       || (p[1] == '.' && p[2] == '='))
	    bha_nga;
	නම් (getnext && (op == '+' || op == '-') && p[0] == p[1])
	    bha_nga;

	evaluate = evalarg == NULL ? 0 : (evalarg->eval_flags & EVAL_EVALUATE);
	oplen = (concat && p[1] == '.') ? 2 : 1;
	නම් (getnext)
	    *arg = eval_next_line(*arg, evalarg);
	නැතහොත්
	{
	    නම් (evaluate && vim9script && !VIM_ISWHITE(**arg))
	    {
		error_white_both(*arg, oplen);
		clear_tv(rettv);
		ආපසු FAIL;
	    }
	    *arg = p;
	}
	නම් ((op != '+' || (rettv->v_type != VAR_LIST
						&& rettv->v_type != VAR_TUPLE
						&& rettv->v_type != VAR_BLOB))
		&& (op == '.' || rettv->v_type != VAR_FLOAT)
		&& evaluate)
	{
	    පූර්ණ_සංඛ්‍යා		error = FALSE;

	    // For "list + ...", an illegal use of the first operand as
	    // a number cannot be determined before evaluating the 2nd
	    // operand: if this is also a list, all is ok.
	    // For "something . ...", "something - ..." or "non-list + ...",
	    // we know that the first operand needs to be a string or number
	    // without evaluating the 2nd operand.  So check before to avoid
	    // side effects after an error.
	    නම් (op != '.')
		tv_get_number_chk(rettv, &error);
	    නම් ((op == '.' && tv_get_string_chk(rettv) == NULL) || error)
	    {
		clear_tv(rettv);
		ආපසු FAIL;
	    }
	}

	/*
	 * Get the second variable.
	 */
	නම් (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[oplen]))
	{
	    error_white_both(*arg, oplen);
	    clear_tv(rettv);
	    ආපසු FAIL;
	}
	*arg = skipwhite_and_linebreak(*arg + oplen, evalarg);
	නම් (eval7(arg, &var2, evalarg, !vim9script && op == '.') == FAIL)
	{
	    clear_tv(rettv);
	    ආපසු FAIL;
	}

	නම් (evaluate)
	{
	    /*
	     * Compute the result.
	     */
	    // use the line of the operation for messages
	    SOURCING_LNUM = op_lnum;
	    නම් (op == '.')
	    {
		නම් (eval_concat_str(rettv, &var2) == FAIL)
		    ආපසු FAIL;
	    }
	    නැතහොත් නම් (op == '+' && rettv->v_type == VAR_BLOB
						   && var2.v_type == VAR_BLOB)
		eval_addblob(rettv, &var2);
	    නැතහොත් නම් (op == '+' && rettv->v_type == VAR_LIST
						   && var2.v_type == VAR_LIST)
	    {
		නම් (eval_addlist(rettv, &var2) == FAIL)
		    ආපසු FAIL;
	    }
	    නැතහොත් නම් (op == '+' && rettv->v_type == VAR_TUPLE
					   && var2.v_type == VAR_TUPLE)
	    {
		නම් (eval_addtuple(rettv, &var2) == FAIL)
		    ආපසු FAIL;
	    }
	    නැතහොත්
	    {
		නම් (eval_addsub_number(rettv, &var2, op) == FAIL)
		    ආපසු FAIL;
	    }
	    clear_tv(&var2);
	}
    }
    ආපසු OK;
}

/*
 * Multiply or divide or compute the modulo of numbers "tv1" and "tv2" and
 * store the result in "tv1".  The numbers can be whole numbers or floats.
 */
    static පූර්ණ_සංඛ්‍යා
eval_multdiv_number(typval_T *tv1, typval_T *tv2, පූර්ණ_සංඛ්‍යා op)
{
    varnumber_T	n1, n2;
    float_T	f1, f2;
    පූර්ණ_සංඛ්‍යා		error;
    පූර්ණ_සංඛ්‍යා		use_float = FALSE;

    f1 = 0;
    f2 = 0;
    error = FALSE;
    නම් (tv1->v_type == VAR_FLOAT)
    {
	f1 = tv1->vval.v_float;
	use_float = TRUE;
	n1 = 0;
    }
    නැතහොත්
	n1 = tv_get_number_chk(tv1, &error);
    clear_tv(tv1);
    නම් (error)
    {
	clear_tv(tv2);
	ආපසු FAIL;
    }

    නම් (tv2->v_type == VAR_FLOAT)
    {
	නම් (!use_float)
	{
	    f1 = n1;
	    use_float = TRUE;
	}
	f2 = tv2->vval.v_float;
	n2 = 0;
    }
    නැතහොත්
    {
	n2 = tv_get_number_chk(tv2, &error);
	clear_tv(tv2);
	නම් (error)
	    ආපසු FAIL;
	නම් (use_float)
	    f2 = n2;
    }

    /*
     * Compute the result.
     * When either side is a float the result is a float.
     */
    නම් (use_float)
    {
	නම් (op == '*')
	    f1 = f1 * f2;
	නැතහොත් නම් (op == '/')
	{
#ifdef VMS
	    // VMS crashes on divide by zero, work around it
	    නම් (f2 == 0.0)
	    {
		නම් (f1 == 0)
		    f1 = -1 * __F_FLT_MAX - 1L;   // similar to NaN
		නැතහොත් නම් (f1 < 0)
		    f1 = -1 * __F_FLT_MAX;
		නැතහොත්
		    f1 = __F_FLT_MAX;
	    }
	    නැතහොත්
		f1 = f1 / f2;
#නැතහොත්
	    // We rely on the floating point library to handle divide
	    // by zero to result in "inf" and not a crash.
	    f1 = f1 / f2;
#endif
	}
	නැතහොත්
	{
	    emsg(_(e_cannot_use_percent_with_float));
	    ආපසු FAIL;
	}
	tv1->v_type = VAR_FLOAT;
	tv1->vval.v_float = f1;
    }
    නැතහොත්
    {
	පූර්ණ_සංඛ්‍යා	    failed = FALSE;

	නම් (op == '*')
	    n1 = n1 * n2;
	නැතහොත් නම් (op == '/')
	    n1 = num_divide(n1, n2, &failed);
	නැතහොත්
	    n1 = num_modulus(n1, n2, &failed);
	නම් (failed)
	    ආපසු FAIL;

	tv1->v_type = VAR_NUMBER;
	tv1->vval.v_number = n1;
    }

    ආපසු OK;
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
    static පූර්ණ_සංඛ්‍යා
eval7(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    පූර්ණ_සංඛ්‍යා		want_string)  // after "." operator
{
    /*
     * Get the first expression.
     */
    නම් (eval8(arg, rettv, evalarg, want_string) == FAIL)
	ආපසු FAIL;

    /*
     * Repeat computing, until no '*', '/' or '%' is following.
     */
    සඳහා (;;)
    {
	පූර්ණ_සංඛ්‍යා	    evaluate;
	පූර්ණ_සංඛ්‍යා	    getnext;
	typval_T    var2;
	char_u	    *p;
	පූර්ණ_සංඛ්‍යා	    op;

	// "*=", "/=" and "%=" are assignments
	p = eval_next_non_blank(*arg, evalarg, &getnext);
	op = *p;
	නම් ((op != '*' && op != '/' && op != '%') || p[1] == '=')
	    bha_nga;

	evaluate = evalarg == NULL ? 0 : (evalarg->eval_flags & EVAL_EVALUATE);
	නම් (getnext)
	    *arg = eval_next_line(*arg, evalarg);
	නැතහොත්
	{
	    නම් (evaluate && in_vim9script() && !VIM_ISWHITE(**arg))
	    {
		error_white_both(*arg, 1);
		clear_tv(rettv);
		ආපසු FAIL;
	    }
	    *arg = p;
	}

	/*
	 * Get the second variable.
	 */
	නම් (evaluate && in_vim9script() && !IS_WHITE_OR_NUL((*arg)[1]))
	{
	    error_white_both(*arg, 1);
	    clear_tv(rettv);
	    ආපසු FAIL;
	}
	*arg = skipwhite_and_linebreak(*arg + 1, evalarg);
	නම් (eval8(arg, &var2, evalarg, FALSE) == FAIL)
	    ආපසු FAIL;

	නම් (evaluate)
	    // Compute the result.
	    නම් (eval_multdiv_number(rettv, &var2, op) == FAIL)
		ආපසු FAIL;
    }

    ආපසු OK;
}

/*
 * Handle seventh level expression:
 *	a type cast before a base level expression.
 * "arg" must point to the first non-white of the expression.
 * "arg" is advanced to just after the recognized expression.
 * Return OK or FAIL.
 */
    static පූර්ණ_සංඛ්‍යා
eval8(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    පූර්ණ_සංඛ්‍යා		want_string)	// after "." operator
{
    type_T	*want_type = NULL;
    garray_T	type_list;	    // list of pointers to allocated types
    පූර්ණ_සංඛ්‍යා		res;
    පූර්ණ_සංඛ්‍යා		evaluate = evalarg == NULL ? 0
				       : (evalarg->eval_flags & EVAL_EVALUATE);

    // Recognize <type> in Vim9 script only.
    නම් (in_vim9script() && **arg == '<' && eval_isnamec1((*arg)[1])
					     && STRNCMP(*arg, "<SNR>", 5) != 0)
    {
	++*arg;
	ga_init2(&type_list, parimaa_na(type_T *), 10);
	නම් (!evaluate)
	    // Only skip over the type, it may use a type variable that is only
	    // known when the expression is compiled, e.g. in a lambda.
	    *arg = skip_type(*arg, FALSE);
	නැතහොත්
	{
	    want_type = parse_type(arg, &type_list, NULL, NULL, TRUE);
	    නම් (want_type == NULL)
	    {
		clear_type_list(&type_list);
		ආපසු FAIL;
	    }
	}

	නම් (**arg != '>')
	{
	    නම් (*skipwhite(*arg) == '>')
		semsg(_(e_no_white_space_allowed_before_str_str), ">", *arg);
	    නැතහොත්
		emsg(_(e_missing_gt));
	    clear_type_list(&type_list);
	    ආපසු FAIL;
	}
	++*arg;
	*arg = skipwhite_and_linebreak(*arg, evalarg);
    }

    res = eval9(arg, rettv, evalarg, want_string);

    නම් (want_type != NULL)
    {
	නම් (evaluate && res == OK)
	{
	    type_T *actual = typval2type(rettv, get_copyID(), &type_list,
							       TVTT_DO_MEMBER);

	    නම් (!equal_type(want_type, actual, 0))
	    {
		නම් (want_type->tt_type == VAR_BOOL
					&& actual->tt_type != VAR_BOOL
					&& (actual->tt_flags & TTFLAG_BOOL_OK))
		{
		    පූර්ණ_සංඛ්‍යා n = tv2bool(rettv);

		    // can use "0" and "1" for boolean in some places
		    clear_tv(rettv);
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_number = n ? VVAL_TRUE : VVAL_FALSE;
		}
		නැතහොත්
		{
		    where_T where = WHERE_INIT;

		    res = check_type(want_type, actual, TRUE, where);
		}
	    }
	}
	clear_type_list(&type_list);
    }

    ආපසු res;
}

    පූර්ණ_සංඛ්‍යා
eval_leader(char_u **arg, පූර්ණ_සංඛ්‍යා vim9)
{
    char_u	*s = *arg;
    char_u	*p = *arg;

    අතරතුර (*p == '!' || *p == '-' || *p == '+')
    {
	char_u *n = skipwhite(p + 1);

	// ++, --, -+ and +- are not accepted in Vim9 script
	නම් (vim9 && (*p == '-' || *p == '+') && (*n == '-' || *n == '+'))
	{
	    semsg(_(e_invalid_expression_str), s);
	    ආපසු FAIL;
	}
	p = n;
    }
    *arg = p;
    ආපසු OK;
}

/*
 * Check for a predefined value "true", "false" and "null.*".
 * Return OK when recognized.
 */
    පූර්ණ_සංඛ්‍යා
handle_predefined(char_u *s, පූර්ණ_සංඛ්‍යා len, typval_T *rettv)
{
    switch (len)
    {
	pak_sa 4: නම් (STRNCMP(s, "true", 4) == 0)
		{
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_number = VVAL_TRUE;
		    ආපසු OK;
		}
		නම් (STRNCMP(s, "null", 4) == 0)
		{
		    rettv->v_type = VAR_SPECIAL;
		    rettv->vval.v_number = VVAL_NULL;
		    ආපසු OK;
		}
		bha_nga;
	pak_sa 5: නම් (STRNCMP(s, "false", 5) == 0)
		{
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_number = VVAL_FALSE;
		    ආපසු OK;
		}
		bha_nga;
	pak_sa 8: නම් (STRNCMP(s, "null_job", 8) == 0)
		{
#ifdef FEAT_JOB_CHANNEL
		    rettv->v_type = VAR_JOB;
		    rettv->vval.v_job = NULL;
#නැතහොත්
		    rettv->v_type = VAR_SPECIAL;
		    rettv->vval.v_number = VVAL_NULL;
#endif
		    ආපසු OK;
		}
		bha_nga;
	pak_sa 9:
		නම් (STRNCMP(s, "null_", 5) != 0)
		    bha_nga;
		// null_list
		නම් (STRNCMP(s + 5, "list", 4) == 0)
		{
		    rettv->v_type = VAR_LIST;
		    rettv->vval.v_list = NULL;
		    ආපසු OK;
		}
		// null_dict
		නම් (STRNCMP(s + 5, "dict", 4) == 0)
		{
		    rettv->v_type = VAR_DICT;
		    rettv->vval.v_dict = NULL;
		    ආපසු OK;
		}
		// null_blob
		නම් (STRNCMP(s + 5, "blob", 4) == 0)
		{
		    rettv->v_type = VAR_BLOB;
		    rettv->vval.v_blob = NULL;
		    ආපසු OK;
		}
		bha_nga;
	pak_sa 10:
		නම් (STRNCMP(s, "null_", 5) != 0)
		    bha_nga;
		// null_class
		නම් (STRNCMP(s + 5, "class", 5) == 0)
		{
		    rettv->v_type = VAR_CLASS;
		    rettv->vval.v_class = NULL;
		    ආපසු OK;
		}
		නම් (STRNCMP(s + 5, "tuple", 5) == 0)
		{
		    rettv->v_type = VAR_TUPLE;
		    rettv->vval.v_tuple = NULL;
		    ආපසු OK;
		}
		bha_nga;
	pak_sa 11: නම් (STRNCMP(s, "null_string", 11) == 0)
		{
		    rettv->v_type = VAR_STRING;
		    rettv->vval.v_string = NULL;
		    ආපසු OK;
		}
		නම් (STRNCMP(s, "null_object", 11) == 0)
		{
		    rettv->v_type = VAR_OBJECT;
		    rettv->vval.v_object = NULL;
		    ආපසු OK;
		}
		bha_nga;
	pak_sa 12:
		නම් (STRNCMP(s, "null_channel", 12) == 0)
		{
#ifdef FEAT_JOB_CHANNEL
		    rettv->v_type = VAR_CHANNEL;
		    rettv->vval.v_channel = NULL;
#නැතහොත්
		    rettv->v_type = VAR_SPECIAL;
		    rettv->vval.v_number = VVAL_NULL;
#endif
		    ආපසු OK;
		}
		නම් (STRNCMP(s, "null_partial", 12) == 0)
		{
		    rettv->v_type = VAR_PARTIAL;
		    rettv->vval.v_partial = NULL;
		    ආපසු OK;
		}
		bha_nga;
	pak_sa 13: නම් (STRNCMP(s, "null_function", 13) == 0)
		{
		    rettv->v_type = VAR_FUNC;
		    rettv->vval.v_string = NULL;
		    ආපසු OK;
		}
		bha_nga;
    }
    ආපසු FAIL;
}

/*
 * Handle register contents: @r.
 */
    static void
eval9_reg_contents(
    char_u	**arg,
    typval_T	*rettv,
    පූර්ණ_සංඛ්‍යා		evaluate)
{
    පූර්ණ_සංඛ්‍යා		vim9script = in_vim9script();

    ++*arg;	// skip '@'
    නම් (evaluate)
    {
	නම් (vim9script && IS_WHITE_OR_NUL(**arg))
	    semsg(_(e_syntax_error_at_str), *arg);
	නැතහොත් නම් (vim9script && !valid_yank_reg(**arg, FALSE))
	    emsg_invreg(**arg);
	නැතහොත්
	{
	    rettv->v_type = VAR_STRING;
	    rettv->vval.v_string = get_reg_contents(**arg,
		    GREG_EXPR_SRC);
	}
    }
    නම් (**arg != NUL)
	++*arg;
}

/*
 * Handle a nested expression: (expression) or lambda: (arg) => expr
 */
    static පූර්ණ_සංඛ්‍යා
eval9_nested_expr(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    පූර්ණ_සංඛ්‍යා		evaluate)
{
    පූර්ණ_සංඛ්‍යා		ret = NOTDONE;
    පූර්ණ_සංඛ්‍යා		vim9script = in_vim9script();

    නම් (vim9script)
    {
	ret = get_lambda_tv(arg, rettv, TRUE, evalarg, NULL);
	නම් (ret == OK && evaluate)
	{
	    ufunc_T *ufunc = rettv->vval.v_partial->pt_func;

	    // Compile it here to get the return type.  The return
	    // type is optional, when it's missing use t_unknown.
	    // This is recognized in compile_return().
	    නම් (ufunc->uf_ret_type->tt_type == VAR_VOID)
		ufunc->uf_ret_type = &t_unknown;
	    නම් (compile_def_function(ufunc, FALSE,
				get_compile_type(ufunc), NULL) == FAIL)
	    {
		clear_tv(rettv);
		ret = FAIL;
	    }
	}
    }
    නම් (ret == NOTDONE)
    {
	*arg = skipwhite_and_linebreak(*arg + 1, evalarg);
	නම් (**arg == ')')
	    // empty tuple
	    ret = eval_tuple(arg, rettv, evalarg, TRUE);
	නැතහොත්
	{
	    ret = eval1(arg, rettv, evalarg);	// recursive!
	    නම් (ret != OK)
		ආපසු ret;

	    *arg = skipwhite_and_linebreak(*arg, evalarg);

	    නම් (**arg == ',')
		// tuple
		ret = eval_tuple(arg, rettv, evalarg, TRUE);
	    නැතහොත් නම් (**arg == ')')
		++*arg;
	    නැතහොත් නම් (ret == OK)
	    {
		emsg(_(e_missing_closing_paren));
		clear_tv(rettv);
		ret = FAIL;
	    }
	}
    }

    ආපසු ret;
}

/*
* Handle be a variable or function name.
* Can also be a curly-braces kind of name: {expr}.
*/
    static පූර්ණ_සංඛ්‍යා
eval9_var_func_name(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    පූර්ණ_සංඛ්‍යා		evaluate,
    char_u	**name_start)
{
    char_u	*s;
    පූර්ණ_සංඛ්‍යා		len;
    char_u	*alias;
    පූර්ණ_සංඛ්‍යා		ret = OK;
    පූර්ණ_සංඛ්‍යා		vim9script = in_vim9script();

    s = *arg;
    len = get_name_len(arg, &alias, evaluate, TRUE);
    නම් (alias != NULL)
	s = alias;

    නම් (len <= 0)
	ret = FAIL;
    නැතහොත්
    {
	පූර්ණ_සංඛ්‍යා	flags = evalarg == NULL ? 0 : evalarg->eval_flags;

	නම් (evaluate && vim9script && len == 1 && *s == '_')
	{
	    emsg(_(e_cannot_use_underscore_here));
	    ret = FAIL;
	}
	නැතහොත් නම් (evaluate && vim9script && len > 2
						&& s[0] == 's' && s[1] == ':')
	{
	    semsg(_(e_cannot_use_s_colon_in_vim9_script_str), s);
	    ret = FAIL;
	}
	නැතහොත් නම් ((vim9script ? **arg : *skipwhite(*arg)) == '('
				|| (vim9script && generic_func_call(arg)))
	{
	    // "name(..."  recursive!
	    *arg = skipwhite(*arg);
	    ret = eval_func(arg, evalarg, s, len, rettv, flags, NULL);
	}
	නැතහොත් නම් (evaluate)
	{
	    // get the value of "true", "false", etc. or a variable
	    ret = FAIL;
	    නම් (vim9script)
		ret = handle_predefined(s, len, rettv);
	    නම් (ret == FAIL)
	    {
		*name_start = s;
		ret = eval_variable(s, len, 0, rettv, NULL,
					EVAL_VAR_VERBOSE + EVAL_VAR_IMPORT);

		// skip the generic function arguments (if present)
		// they are already processed by eval_variable
		නම් (ret == OK && vim9script && **arg == '<'
						&& rettv->v_type == VAR_FUNC)
		    ret = skip_generic_func_type_args(arg);
	    }
	}
	නැතහොත්
	{
	    // skip the name
	    check_vars(s, len);

	    // Skip the type arguments of a generic function, its name starts
	    // with an uppercase letter.
	    නම් (vim9script && **arg == '<' && ASCII_ISUPPER(*s))
		skip_generic_func_type_args(arg);
	    ret = OK;
	}
    }
    vim_free(alias);

    ආපසු ret;
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
    static පූර්ණ_සංඛ්‍යා
eval9(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    පූර්ණ_සංඛ්‍යා		want_string)	// after "." operator
{
    පූර්ණ_සංඛ්‍යා		evaluate = evalarg != NULL
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    char_u	*name_start = NULL;
    char_u	*start_leader, *end_leader;
    පූර්ණ_සංඛ්‍යා		ret = OK;
    static පූර්ණ_සංඛ්‍යා	recurse = 0;
    පූර්ණ_සංඛ්‍යා		vim9script = in_vim9script();

    /*
     * Initialise variable so that clear_tv() can't mistake this for a
     * string and free a string that isn't there.
     */
    rettv->v_type = VAR_UNKNOWN;

    /*
     * Skip '!', '-' and '+' characters.  They are handled later.
     */
    start_leader = *arg;
    නම් (eval_leader(arg, vim9script) == FAIL)
	ආපසු FAIL;
    end_leader = *arg;

    නම් (**arg == '.' && (!SAFE_isdigit(*(*arg + 1)) || in_old_script(2)))
    {
	semsg(_(e_invalid_expression_str), *arg);
	++*arg;
	ආපසු FAIL;
    }

    // Limit recursion to 1000 levels.  At least at 10000 we run out of stack
    // and crash.  With MSVC the stack is smaller.
    නම් (recurse ==
#ifdef _MSC_VER
		    300
#නැතහොත්
		    1000
#endif
		    )
    {
	semsg(_(e_expression_too_recursive_str), *arg);
	ආපසු FAIL;
    }
    ++recurse;

    switch (**arg)
    {
    /*
     * Number constant.
     */
    pak_sa '0':
    pak_sa '1':
    pak_sa '2':
    pak_sa '3':
    pak_sa '4':
    pak_sa '5':
    pak_sa '6':
    pak_sa '7':
    pak_sa '8':
    pak_sa '9':
    pak_sa '.':	ret = eval_number(arg, rettv, evaluate, want_string);

		// Apply prefixed "-" and "+" now.  Matters especially when
		// "->" follows.
		නම් (ret == OK && evaluate && end_leader > start_leader
						  && rettv->v_type != VAR_BLOB)
		    ret = eval9_leader(rettv, TRUE, start_leader, &end_leader);
		bha_nga;

    /*
     * String constant: "string".
     */
    pak_sa '"':	ret = eval_string(arg, rettv, evaluate, FALSE);
		bha_nga;

    /*
     * Literal string constant: 'str''ing'.
     */
    pak_sa '\'':	ret = eval_lit_string(arg, rettv, evaluate, FALSE);
		bha_nga;

    /*
     * List: [expr, expr]
     */
    pak_sa '[':	ret = eval_list(arg, rettv, evalarg, TRUE);
		bha_nga;

    /*
     * Literal Dictionary: #{key: val, key: val}
     */
    pak_sa '#':	ret = eval_lit_dict(arg, rettv, evalarg);
		bha_nga;

    /*
     * Lambda: {arg, arg -> expr}
     * Dictionary: {'key': val, 'key': val}
     */
    pak_sa '{':	නම් (vim9script)
		    ret = NOTDONE;
		නැතහොත්
		    ret = get_lambda_tv(arg, rettv, vim9script, evalarg, NULL);
		නම් (ret == NOTDONE)
		    ret = eval_dict(arg, rettv, evalarg, FALSE);
		bha_nga;

    /*
     * Option value: &name
     */
    pak_sa '&':	ret = eval_option(arg, rettv, evaluate);
		bha_nga;

    /*
     * Environment variable: $VAR.
     * Interpolated string: $"string" or $'string'.
     */
    pak_sa '$':	නම් ((*arg)[1] == '"' || (*arg)[1] == '\'')
		    ret = eval_interp_string(arg, rettv, evaluate);
		නැතහොත්
		    ret = eval_env_var(arg, rettv, evaluate);
		bha_nga;

    /*
     * Register contents: @r.
     */
    pak_sa '@':	eval9_reg_contents(arg, rettv, evaluate);
		bha_nga;

    /*
     * nested expression: (expression).
     * or lambda: (arg) => expr
     * or tuple
     */
    pak_sa '(':	ret = eval9_nested_expr(arg, rettv, evalarg, evaluate);
		bha_nga;

    default:	ret = NOTDONE;
		bha_nga;
    }

    නම් (ret == NOTDONE)
    {
	/*
	 * Must be a variable or function name.
	 * Can also be a curly-braces kind of name: {expr}.
	 */
	ret = eval9_var_func_name(arg, rettv, evalarg, evaluate, &name_start);
    }

    // Handle following '[', '(' and '.' for expr[expr], expr.name,
    // expr(expr), expr->name(expr)
    නම් (ret == OK)
	ret = handle_subscript(arg, name_start, rettv, evalarg, evaluate);

    /*
     * Apply logical NOT and unary '-', from right to left, ignore '+'.
     */
    නම් (ret == OK && evaluate && end_leader > start_leader)
	ret = eval9_leader(rettv, FALSE, start_leader, &end_leader);

    --recurse;
    ආපසු ret;
}

/*
 * Apply the leading "!" and "-" before an eval9 expression to "rettv".
 * When "numeric_only" is TRUE only handle "+" and "-".
 * Adjusts "end_leaderp" until it is at "start_leader".
 */
    static පූර්ණ_සංඛ්‍යා
eval9_leader(
	typval_T    *rettv,
	පූර්ණ_සංඛ්‍යා	    numeric_only,
	char_u	    *start_leader,
	char_u	    **end_leaderp)
{
    char_u	*end_leader = *end_leaderp;
    පූර්ණ_සංඛ්‍යා		ret = OK;
    පූර්ණ_සංඛ්‍යා		error = FALSE;
    varnumber_T val = 0;
    vartype_T	type = rettv->v_type;
    පූර්ණ_සංඛ්‍යා		vim9script = in_vim9script();
    float_T	    f = 0.0;

    නම් (rettv->v_type == VAR_FLOAT)
	f = rettv->vval.v_float;
    නැතහොත්
    {
	අතරතුර (VIM_ISWHITE(end_leader[-1]))
	    --end_leader;
	නම් (vim9script && end_leader[-1] == '!')
	{
	    // Is this typeval supported with the ! operator?
	    නම් (check_typval_is_value(rettv) == FAIL)
	    {
		clear_tv(rettv);
		ආපසු FAIL;
	    }
	    val = tv2bool(rettv);
	}
	නැතහොත්
	    val = tv_get_number_chk(rettv, &error);
    }
    නම් (error)
    {
	clear_tv(rettv);
	ret = FAIL;
    }
    නැතහොත්
    {
	අතරතුර (end_leader > start_leader)
	{
	    --end_leader;
	    නම් (*end_leader == '!')
	    {
		නම් (numeric_only)
		{
		    ++end_leader;
		    bha_nga;
		}
		නම් (rettv->v_type == VAR_FLOAT)
		{
		    rettv->v_type = VAR_BOOL;
		    val = f == 0.0 ? VVAL_TRUE : VVAL_FALSE;
		    type = VAR_BOOL;
		}
		නැතහොත්
		{
		    val = !val;
		    type = VAR_BOOL;
		}
	    }
	    නැතහොත් නම් (*end_leader == '-')
	    {
		නම් (rettv->v_type == VAR_FLOAT)
		    f = -f;
		නැතහොත්
		{
		    val = -val;
		    type = VAR_NUMBER;
		}
	    }
	}
	නම් (rettv->v_type == VAR_FLOAT)
	{
	    clear_tv(rettv);
	    rettv->vval.v_float = f;
	}
	නැතහොත්
	{
	    clear_tv(rettv);
	    නම් (vim9script)
		rettv->v_type = type;
	    නැතහොත්
		rettv->v_type = VAR_NUMBER;
	    rettv->vval.v_number = val;
	}
    }
    *end_leaderp = end_leader;
    ආපසු ret;
}

/*
 * Call the function referred to in "rettv".
 */
    static පූර්ණ_සංඛ්‍යා
call_func_rettv(
	char_u	    **arg,
	evalarg_T   *evalarg,
	typval_T    *rettv,
	පූර්ණ_සංඛ්‍යා	    evaluate,
	dict_T	    *selfdict,
	typval_T    *basetv)
{
    partial_T	*pt = NULL;
    funcexe_T	funcexe;
    typval_T	functv;
    char_u	*s;
    පූර්ණ_සංඛ්‍යා		ret;

    // need to copy the funcref so that we can clear rettv
    නම් (evaluate)
    {
	functv = *rettv;
	rettv->v_type = VAR_UNKNOWN;

	// Invoke the function.  Recursive!
	නම් (functv.v_type == VAR_PARTIAL)
	{
	    pt = functv.vval.v_partial;
	    s = partial_name(pt);
	}
	නැතහොත්
	{
	    s = functv.vval.v_string;
	    නම් (s == NULL || *s == NUL)
	    {
		emsg(_(e_empty_function_name));
		ret = FAIL;
		goto theend;
	    }
	}
    }
    නැතහොත්
	s = (char_u *)"";

    CLEAR_FIELD(funcexe);
    funcexe.fe_firstline = curwin->w_cursor.lnum;
    funcexe.fe_lastline = curwin->w_cursor.lnum;
    funcexe.fe_evaluate = evaluate;
    funcexe.fe_partial = pt;
    නම් (pt != NULL)
    {
	funcexe.fe_object = pt->pt_obj;
	නම් (funcexe.fe_object != NULL)
	    ++funcexe.fe_object->obj_refcount;
    }
    funcexe.fe_selfdict = selfdict;
    funcexe.fe_basetv = basetv;
    නම් (evalarg != NULL)
	funcexe.fe_cctx = evalarg->eval_cctx;
    ret = get_func_tv(s, -1, rettv, arg, evalarg, &funcexe);

theend:
    // Clear the funcref afterwards, so that deleting it while
    // evaluating the arguments is possible (see test55).
    නම් (evaluate)
	clear_tv(&functv);

    ආපසු ret;
}

/*
 * Evaluate "->method()".
 * "*arg" points to "method".
 * Returns FAIL or OK. "*arg" is advanced to after the ')'.
 */
    static පූර්ණ_සංඛ්‍යා
eval_lambda(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    පූර්ණ_සංඛ්‍යා		verbose)	// give error messages
{
    පූර්ණ_සංඛ්‍යා		evaluate = evalarg != NULL
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    typval_T	base = *rettv;
    පූර්ණ_සංඛ්‍යා		ret;

    rettv->v_type = VAR_UNKNOWN;

    නම් (**arg == '{')
    {
	// ->{lambda}()
	ret = get_lambda_tv(arg, rettv, FALSE, evalarg, NULL);
    }
    නැතහොත්
    {
	// ->(lambda)()
	++*arg;
	ret = eval1(arg, rettv, evalarg);
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	නම් (**arg != ')')
	{
	    emsg(_(e_missing_closing_paren));
	    ආපසු FAIL;
	}
	නම් (rettv->v_type != VAR_STRING && rettv->v_type != VAR_FUNC
					       && rettv->v_type != VAR_PARTIAL)
	{
	    emsg(_(e_string_or_function_required_for_arrow_parens_expr));
	    ආපසු FAIL;
	}
	++*arg;
    }
    නම් (ret != OK)
	ආපසු FAIL;

    නම් (**arg != '(')
    {
	නම් (verbose)
	{
	    නම් (*skipwhite(*arg) == '(')
		emsg(_(e_no_white_space_allowed_before_parenthesis));
	    නැතහොත්
		semsg(_(e_missing_parenthesis_str), "lambda");
	}
	clear_tv(rettv);
	ret = FAIL;
    }
    නැතහොත්
	ret = call_func_rettv(arg, evalarg, rettv, evaluate, NULL, &base);

    // Clear the funcref afterwards, so that deleting it while
    // evaluating the arguments is possible (see test55).
    නම් (evaluate)
	clear_tv(&base);

    ආපසු ret;
}

/*
 * Evaluate "->method()".
 * "*arg" points to "method".
 * Returns FAIL or OK. "*arg" is advanced to after the ')'.
 */
    static පූර්ණ_සංඛ්‍යා
eval_method(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    පූර්ණ_සංඛ්‍යා		verbose)	// give error messages
{
    char_u	*name;
    long	len;
    char_u	*alias;
    char_u	*tofree = NULL;
    typval_T	base = *rettv;
    පූර්ණ_සංඛ්‍යා		ret = OK;
    පූර්ණ_සංඛ්‍යා		evaluate = evalarg != NULL
				      && (evalarg->eval_flags & EVAL_EVALUATE);

    rettv->v_type = VAR_UNKNOWN;

    name = *arg;
    len = get_name_len(arg, &alias, evaluate, evaluate);
    නම් (alias != NULL)
	name = alias;

    නම් (len <= 0)
    {
	නම් (verbose)
	    emsg(_(e_missing_name_after_method));
	ret = FAIL;
    }
    නැතහොත්
    {
	char_u *paren;

	// If there is no "(" immediately following, but there is further on,
	// it can be "import.Func()", "dict.Func()", "list[nr]", etc.
	// Does not handle anything where "(" is part of the expression.
	*arg = skipwhite(*arg);

	නම් (**arg != '(' && alias == NULL
				    && (paren = vim_strchr(*arg, '(')) != NULL)
	{
	    *arg = name;

	    // Truncate the name at the "(".  Avoid trying to get another line
	    // by making "getline" NULL.
	    *paren = NUL;
	    char_u	*(*getline)(පූර්ණ_සංඛ්‍යා, void *, පූර්ණ_සංඛ්‍යා, getline_opt_T) = NULL;
	    නම් (evalarg != NULL)
	    {
		getline = evalarg->eval_getline;
		evalarg->eval_getline = NULL;
	    }

	    char_u *deref = deref_function_name(arg, &tofree, evalarg, verbose);
	    නම් (deref == NULL)
	    {
		*arg = name + len;
		ret = FAIL;
	    }
	    නැතහොත්
	    {
		name = deref;
		len = (long)STRLEN(name);
	    }

	    *paren = '(';
	    නම් (getline != NULL)
		evalarg->eval_getline = getline;
	}

	නම් (ret == OK)
	{
	    *arg = skipwhite(*arg);

	    නම් (**arg != '(')
	    {
		නම් (verbose)
		    semsg(_(e_missing_parenthesis_str), name);
		ret = FAIL;
	    }
	    නැතහොත් නම් (VIM_ISWHITE((*arg)[-1]))
	    {
		නම් (verbose)
		    emsg(_(e_no_white_space_allowed_before_parenthesis));
		ret = FAIL;
	    }
	    නැතහොත්
		ret = eval_func(arg, evalarg, name, len, rettv,
					  evaluate ? EVAL_EVALUATE : 0, &base);
	}
    }

    // Clear the funcref afterwards, so that deleting it while
    // evaluating the arguments is possible (see test55).
    නම් (evaluate)
	clear_tv(&base);
    vim_free(tofree);

    නම් (alias != NULL)
	vim_free(alias);

    ආපසු ret;
}

/*
 * Evaluate an "[expr]" or "[expr:expr]" index.  Also "dict.key".
 * "*arg" points to the '[' or '.'.
 * Returns FAIL or OK. "*arg" is advanced to after the ']'.
 */
    static පූර්ණ_සංඛ්‍යා
eval_index(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    පූර්ණ_සංඛ්‍යා		verbose)	// give error messages
{
    පූර්ණ_සංඛ්‍යා		evaluate = evalarg != NULL
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    පූර්ණ_සංඛ්‍යා		empty1 = FALSE, empty2 = FALSE;
    typval_T	var1, var2;
    පූර්ණ_සංඛ්‍යා		range = FALSE;
    char_u	*key = NULL;
    පූර්ණ_සංඛ්‍යා		keylen = -1;
    පූර්ණ_සංඛ්‍යා		vim9script = in_vim9script();

    නම් (check_can_index(rettv, evaluate, verbose) == FAIL)
	ආපසු FAIL;

    init_tv(&var1);
    init_tv(&var2);
    නම් (**arg == '.')
    {
	/*
	 * dict.name
	 */
	key = *arg + 1;
	සඳහා (keylen = 0; eval_isdictc(key[keylen]); ++keylen)
	    ;
	නම් (keylen == 0)
	    ආපසු FAIL;
	නම් (vim9script && key[keylen] == '<')
	{
	    // skip generic type arguments
	    char_u	*p = &key[keylen];

	    නම් (skip_generic_func_type_args(&p) == FAIL)
		ආපසු FAIL;
	    keylen = p - key;
	}
	*arg = key + keylen;
    }
    නැතහොත්
    {
	/*
	 * something[idx]
	 *
	 * Get the (first) variable from inside the [].
	 */
	*arg = skipwhite_and_linebreak(*arg + 1, evalarg);
	නම් (**arg == ':')
	    empty1 = TRUE;
	නැතහොත් නම් (eval1(arg, &var1, evalarg) == FAIL)	// recursive!
	    ආපසු FAIL;
	නැතහොත් නම් (vim9script && **arg == ':')
	{
	    semsg(_(e_white_space_required_before_and_after_str_at_str),
								    ":", *arg);
	    clear_tv(&var1);
	    ආපසු FAIL;
	}
	නැතහොත් නම් (evaluate)
	{
	    පූර්ණ_සංඛ්‍යා error = FALSE;

	    // allow for indexing with float
	    නම් (vim9script && rettv->v_type == VAR_DICT
						   && var1.v_type == VAR_FLOAT)
	    {
		var1.vval.v_string = typval_tostring(&var1, TRUE);
		var1.v_type = VAR_STRING;
	    }

	    නම් (vim9script && (rettv->v_type == VAR_LIST
						|| rettv->v_type == VAR_TUPLE))
		tv_get_number_chk(&var1, &error);
	    නැතහොත්
		error = tv_get_string_chk(&var1) == NULL;
	    නම් (error)
	    {
		// not a number or string
		clear_tv(&var1);
		ආපසු FAIL;
	    }
	}

	/*
	 * Get the second variable from inside the [:].
	 */
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	නම් (**arg == ':')
	{
	    range = TRUE;
	    ++*arg;
	    නම් (vim9script && !IS_WHITE_OR_NUL(**arg) && **arg != ']')
	    {
		semsg(_(e_white_space_required_before_and_after_str_at_str),
								":", *arg - 1);
		නම් (!empty1)
		    clear_tv(&var1);
		ආපසු FAIL;
	    }
	    *arg = skipwhite_and_linebreak(*arg, evalarg);
	    නම් (**arg == ']')
		empty2 = TRUE;
	    නැතහොත් නම් (eval1(arg, &var2, evalarg) == FAIL)	// recursive!
	    {
		නම් (!empty1)
		    clear_tv(&var1);
		ආපසු FAIL;
	    }
	    නැතහොත් නම් (evaluate && tv_get_string_chk(&var2) == NULL)
	    {
		// not a number or string
		නම් (!empty1)
		    clear_tv(&var1);
		clear_tv(&var2);
		ආපසු FAIL;
	    }
	}

	// Check for the ']'.
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	නම් (**arg != ']')
	{
	    නම් (verbose)
		emsg(_(e_missing_closing_square_brace));
	    clear_tv(&var1);
	    නම් (range)
		clear_tv(&var2);
	    ආපසු FAIL;
	}
	*arg = *arg + 1;	// skip over the ']'
    }

    නම් (evaluate)
    {
	පූර්ණ_සංඛ්‍යා res = eval_index_inner(rettv, range,
		empty1 ? NULL : &var1, empty2 ? NULL : &var2, FALSE,
		key, keylen, verbose);

	නම් (!empty1)
	    clear_tv(&var1);
	නම් (range)
	    clear_tv(&var2);
	ආපසු res;
    }
    ආපසු OK;
}

/*
 * Check if "rettv" can have an [index] or [sli:ce]
 */
    පූර්ණ_සංඛ්‍යා
check_can_index(typval_T *rettv, පූර්ණ_සංඛ්‍යා evaluate, පූර්ණ_සංඛ්‍යා verbose)
{
    switch (rettv->v_type)
    {
	pak_sa VAR_FUNC:
	pak_sa VAR_PARTIAL:
	    නම් (verbose)
		emsg(_(e_cannot_index_a_funcref));
	    ආපසු FAIL;
	pak_sa VAR_FLOAT:
	    නම් (verbose)
		emsg(_(e_using_float_as_string));
	    ආපසු FAIL;
	pak_sa VAR_BOOL:
	pak_sa VAR_SPECIAL:
	pak_sa VAR_JOB:
	pak_sa VAR_CHANNEL:
	pak_sa VAR_INSTR:
	pak_sa VAR_OBJECT:
	    නම් (verbose)
		emsg(_(e_cannot_index_special_variable));
	    ආපසු FAIL;
	pak_sa VAR_CLASS:
	pak_sa VAR_TYPEALIAS:
	    නම් (verbose)
		check_typval_is_value(rettv);
	    ආපසු FAIL;
	pak_sa VAR_UNKNOWN:
	pak_sa VAR_ANY:
	pak_sa VAR_VOID:
	    නම් (evaluate)
	    {
		emsg(_(e_cannot_index_special_variable));
		ආපසු FAIL;
	    }
	    // FALLTHROUGH

	pak_sa VAR_STRING:
	pak_sa VAR_LIST:
	pak_sa VAR_TUPLE:
	pak_sa VAR_DICT:
	pak_sa VAR_BLOB:
	    bha_nga;
	pak_sa VAR_NUMBER:
	    නම් (in_vim9script())
		emsg(_(e_cannot_index_number));
	    bha_nga;
    }
    ආපසු OK;
}

/*
 * Apply index or range to "rettv".
 * "var1" is the first index, NULL for [:expr].
 * "var2" is the second index, NULL for [expr] and [expr: ]
 * "exclusive" is TRUE for slice(): second index is exclusive, use character
 * index for string.
 * Alternatively, "key" is not NULL, then key[keylen] is the dict index.
 */
    පූර්ණ_සංඛ්‍යා
eval_index_inner(
	typval_T    *rettv,
	පූර්ණ_සංඛ්‍යා	    is_range,
	typval_T    *var1,
	typval_T    *var2,
	පූර්ණ_සංඛ්‍යා	    exclusive,
	char_u	    *key,
	පූර්ණ_සංඛ්‍යා	    keylen,
	පූර්ණ_සංඛ්‍යා	    verbose)
{
    varnumber_T	    n1, n2 = 0;
    long	    len;

    n1 = 0;
    නම් (var1 != NULL && rettv->v_type != VAR_DICT)
	n1 = tv_get_number(var1);

    නම් (is_range)
    {
	නම් (rettv->v_type == VAR_DICT)
	{
	    නම් (verbose)
		emsg(_(e_cannot_slice_dictionary));
	    ආපසු FAIL;
	}
	නම් (var2 != NULL)
	    n2 = tv_get_number(var2);
	නැතහොත්
	    n2 = VARNUM_MAX;
    }

    switch (rettv->v_type)
    {
	pak_sa VAR_UNKNOWN:
	pak_sa VAR_ANY:
	pak_sa VAR_VOID:
	pak_sa VAR_FUNC:
	pak_sa VAR_PARTIAL:
	pak_sa VAR_FLOAT:
	pak_sa VAR_BOOL:
	pak_sa VAR_SPECIAL:
	pak_sa VAR_JOB:
	pak_sa VAR_CHANNEL:
	pak_sa VAR_INSTR:
	pak_sa VAR_CLASS:
	pak_sa VAR_OBJECT:
	pak_sa VAR_TYPEALIAS:
	    bha_nga; // not evaluating, skipping over subscript

	pak_sa VAR_NUMBER:
	pak_sa VAR_STRING:
	    {
		char_u	*s = tv_get_string(rettv);

		len = (long)STRLEN(s);
		නම් (in_vim9script() || exclusive)
		{
		    නම් (is_range)
			s = string_slice(s, n1, n2, exclusive);
		    නැතහොත්
			s = char_from_string(s, n1);
		}
		නැතහොත් නම් (is_range)
		{
		    // The resulting variable is a substring.  If the indexes
		    // are out of range the result is empty.
		    නම් (n1 < 0)
		    {
			n1 = len + n1;
			නම් (n1 < 0)
			    n1 = 0;
		    }
		    නම් (n2 < 0)
			n2 = len + n2;
		    නැතහොත් නම් (n2 >= len)
			n2 = len;
		    නම් (n1 >= len || n2 < 0 || n1 > n2)
			s = NULL;
		    නැතහොත්
			s = vim_strnsave(s + n1, n2 - n1 + 1);
		}
		නැතහොත්
		{
		    // The resulting variable is a string of a single
		    // character.  If the index is too big or negative the
		    // result is empty.
		    නම් (n1 >= len || n1 < 0)
			s = NULL;
		    නැතහොත්
			s = vim_strnsave(s + n1, 1);
		}
		clear_tv(rettv);
		rettv->v_type = VAR_STRING;
		rettv->vval.v_string = s;
	    }
	    bha_nga;

	pak_sa VAR_BLOB:
	    blob_slice_or_index(rettv->vval.v_blob, is_range, n1, n2,
							     exclusive, rettv);
	    bha_nga;

	pak_sa VAR_LIST:
	    නම් (var1 == NULL)
		n1 = 0;
	    නම් (var2 == NULL)
		n2 = VARNUM_MAX;
	    නම් (list_slice_or_index(rettv->vval.v_list,
			  is_range, n1, n2, exclusive, rettv, verbose) == FAIL)
		ආපසු FAIL;
	    bha_nga;

	pak_sa VAR_TUPLE:
	    නම් (var1 == NULL)
		n1 = 0;
	    නම් (var2 == NULL)
		n2 = VARNUM_MAX;
	    නම් (tuple_slice_or_index(rettv->vval.v_tuple,
			  is_range, n1, n2, exclusive, rettv, verbose) == FAIL)
		ආපසු FAIL;
	    bha_nga;

	pak_sa VAR_DICT:
	    {
		dictitem_T	*item;
		typval_T	tmp;

		නම් (key == NULL)
		{
		    key = tv_get_string_chk(var1);
		    නම් (key == NULL)
			ආපසු FAIL;
		}

		item = dict_find(rettv->vval.v_dict, key, keylen);

		නම් (item == NULL)
		{
		    නම් (verbose)
		    {
			නම් (keylen > 0)
			    key[keylen] = NUL;
			semsg(_(e_key_not_present_in_dictionary_str), key);
		    }
		    ආපසු FAIL;
		}

		copy_tv(&item->di_tv, &tmp);
		clear_tv(rettv);
		*rettv = tmp;
	    }
	    bha_nga;
    }
    ආපසු OK;
}

/*
 * Return the function name of partial "pt".
 */
    char_u *
partial_name(partial_T *pt)
{
    නම් (pt != NULL)
    {
	නම් (pt->pt_name != NULL)
	    ආපසු pt->pt_name;
	නම් (pt->pt_func != NULL)
	    ආපසු pt->pt_func->uf_name;
    }
    ආපසු (char_u *)"";
}

    static void
partial_free(partial_T *pt)
{
    පූර්ණ_සංඛ්‍යා i;

    සඳහා (i = 0; i < pt->pt_argc; ++i)
	clear_tv(&pt->pt_argv[i]);
    vim_free(pt->pt_argv);
    dict_unref(pt->pt_dict);
    නම් (pt->pt_name != NULL)
    {
	func_unref(pt->pt_name);
	vim_free(pt->pt_name);
    }
    නැතහොත්
	func_ptr_unref(pt->pt_func);
    නම් (pt->pt_obj != NULL)
	object_unref(pt->pt_obj);

    // "out_up" is no longer used, decrement refcount on partial that owns it.
    partial_unref(pt->pt_outer.out_up_partial);

    // Using pt_outer from another partial.
    partial_unref(pt->pt_outer_partial);

    // Decrease the reference count for the context of a closure.  If down
    // to the minimum it may be time to free it.
    නම් (pt->pt_funcstack != NULL)
    {
	--pt->pt_funcstack->fs_refcount;
	funcstack_check_refcount(pt->pt_funcstack);
    }
    // Similarly for loop variables.
    සඳහා (i = 0; i < MAX_LOOP_DEPTH; ++i)
	නම් (pt->pt_loopvars[i] != NULL)
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
    void
partial_unref(partial_T *pt)
{
    නම් (pt == NULL)
	ආපසු;

    පූර්ණ_සංඛ්‍යා	done = FALSE;

    නම් (--pt->pt_refcount <= 0)
	partial_free(pt);

    // If the reference count goes down to one, the funcstack may be the
    // only reference and can be freed if no other partials reference it.
    නැතහොත් නම් (pt->pt_refcount == 1)
    {
	// careful: if the funcstack is freed it may contain this partial
	// and it gets freed as well
	නම් (pt->pt_funcstack != NULL)
	    done = funcstack_check_refcount(pt->pt_funcstack);

	නම් (!done)
	{
	    පූර්ණ_සංඛ්‍යා	depth;

	    සඳහා (depth = 0; depth < MAX_LOOP_DEPTH; ++depth)
		නම් (pt->pt_loopvars[depth] != NULL
			&& loopvars_check_refcount(pt->pt_loopvars[depth]))
		    bha_nga;
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
    static char_u *
string_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    පූර්ණ_සංඛ්‍යා		echo_style,
    පූර්ණ_සංඛ්‍යා		composite_val)
{
    char_u	*r = NULL;

    නම් (echo_style && !composite_val)
    {
	*tofree = NULL;
	r = tv->vval.v_string;
	නම් (r == NULL)
	    r = (char_u *)"";
    }
    නැතහොත්
    {
	*tofree = string_quote(tv->vval.v_string, FALSE);
	r = *tofree;
    }

    ආපසු r;
}

/*
 * Return a textual representation of a function in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "echo_style" is FALSE, put quotes around the function name as
 * "function()", otherwise does not put quotes around function name.
 * May return NULL.
 */
    static char_u *
func_tv2string(typval_T *tv, char_u **tofree, පූර්ණ_සංඛ්‍යා echo_style)
{
    char_u	*r = NULL;
    char_u	buf[MAX_FUNC_NAME_LEN];

    නම් (echo_style)
    {
	*tofree = NULL;

	නම් (tv->vval.v_string == NULL)
	    r = (char_u *)"function()";
	නැතහොත්
	{
	    r = make_ufunc_name_readable(tv->vval.v_string, buf,
							MAX_FUNC_NAME_LEN);
	    නම් (r == buf)
		r = *tofree = vim_strsave(buf);
	}
    }
    නැතහොත්
    {
	char_u *s = NULL;

	නම් (tv->vval.v_string != NULL)
	    s = make_ufunc_name_readable(tv->vval.v_string, buf,
							MAX_FUNC_NAME_LEN);

	r = *tofree = string_quote(s, TRUE);
    }

    ආපසු r;
}

/*
 * Return a textual representation of the object method in "tv", a VAR_PARTIAL.
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "echo_style" is FALSE, put quotes around the function name as
 * "function()", otherwise does not put quotes around function name.
 * May return NULL.
 */
    static char_u *
method_tv2string(typval_T *tv, char_u **tofree, පූර්ණ_සංඛ්‍යා echo_style)
{
    char_u	buf[MAX_FUNC_NAME_LEN];
    partial_T	*pt = tv->vval.v_partial;

    size_t len = vim_snprintf((var_na *)buf, parimaa_na(buf), "<SNR>%d_%s.%s",
			   pt->pt_func->uf_script_ctx.sc_sid,
			   pt->pt_func->uf_class->class_name.string,
			   pt->pt_func->uf_name);
    නම් (len >= parimaa_na(buf))
    {
	නම් (echo_style)
	{
	    *tofree = NULL;
	    ආපසු (char_u *)"function()";
	}
	නැතහොත්
	    ආපසු *tofree = string_quote((char_u*)"", TRUE);
    }

    ආපසු *tofree = echo_style ? vim_strsave(buf) : string_quote(buf, TRUE);
}

/*
 * Return a textual representation of a partial in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * "numbuf" is used for a number.  May return NULL.
 */
    static char_u *
partial_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    char_u	*numbuf,
    පූර්ණ_සංඛ්‍යා		copyID)
{
    char_u	*r = NULL;
    partial_T	*pt;
    char_u	*fname;
    garray_T	ga;
    පූර්ණ_සංඛ්‍යා		i;
    char_u	*tf;

    pt = tv->vval.v_partial;
    fname = string_quote(pt == NULL ? NULL : partial_name(pt), FALSE);

    ga_init2(&ga, 1, 100);
    GA_CONCAT_LITERAL(&ga, "function(");
    නම් (fname != NULL)
    {
	// When using uf_name prepend "g:" for a global function.
	නම් (pt != NULL && pt->pt_name == NULL && fname[0] == '\''
						&& vim_isupper(fname[1]))
	{
	    GA_CONCAT_LITERAL(&ga, "'g:");
	    ga_concat(&ga, fname + 1);
	}
	නැතහොත්
	    ga_concat(&ga, fname);
	vim_free(fname);
    }
    නම් (pt != NULL && pt->pt_argc > 0)
    {
	GA_CONCAT_LITERAL(&ga, ", [");
	සඳහා (i = 0; i < pt->pt_argc; ++i)
	{
	    නම් (i > 0)
		GA_CONCAT_LITERAL(&ga, ", ");
	    ga_concat(&ga, tv2string(&pt->pt_argv[i], &tf, numbuf, copyID));
	    vim_free(tf);
	}
	GA_CONCAT_LITERAL(&ga, "]");
    }
    නම් (pt != NULL && pt->pt_dict != NULL)
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

    ආපසු r;
}

/*
 * Return a textual representation of a List in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "copyID" is not zero replace recursive lists with "...".  When
 * "restore_copyID" is FALSE, repeated items in lists are replaced with "...".
 * May return NULL.
 */
    static char_u *
list_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    පූර්ණ_සංඛ්‍යා		copyID,
    පූර්ණ_සංඛ්‍යා		restore_copyID)
{
    char_u	*r = NULL;

    නම් (tv->vval.v_list == NULL)
    {
	// NULL list is equivalent to empty list.
	*tofree = NULL;
	r = (char_u *)"[]";
    }
    නැතහොත් නම් (copyID != 0 && tv->vval.v_list->lv_copyID == copyID
	    && tv->vval.v_list->lv_len > 0)
    {
	*tofree = NULL;
	r = (char_u *)"[...]";
    }
    නැතහොත්
    {
	පූර්ණ_සංඛ්‍යා old_copyID;
	නම් (restore_copyID)
	    old_copyID = tv->vval.v_list->lv_copyID;

	tv->vval.v_list->lv_copyID = copyID;
	*tofree = list2string(tv, copyID, restore_copyID);
	නම් (restore_copyID)
	    tv->vval.v_list->lv_copyID = old_copyID;
	r = *tofree;
    }

    ආපසු r;
}

/*
 * Return a textual representation of a Tuple in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "copyID" is not zero replace recursive lists with "...".  When
 * "restore_copyID" is FALSE, repeated items in tuples are replaced with "...".
 * May return NULL.
 */
    static char_u *
tuple_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    පූර්ණ_සංඛ්‍යා		copyID,
    පූර්ණ_සංඛ්‍යා		restore_copyID)
{
    tuple_T	*tuple = tv->vval.v_tuple;
    char_u	*r = NULL;

    නම් (tuple == NULL)
    {
	// NULL tuple is equivalent to an empty tuple.
	*tofree = NULL;
	r = (char_u *)"()";
    }
    නැතහොත් නම් (copyID != 0 && tuple->tv_copyID == copyID
					&& tuple->tv_items.ga_len > 0)
    {
	*tofree = NULL;
	r = (char_u *)"(...)";
    }
    නැතහොත්
    {
	පූර්ණ_සංඛ්‍යා old_copyID;
	නම් (restore_copyID)
	    old_copyID = tuple->tv_copyID;

	tuple->tv_copyID = copyID;
	*tofree = tuple2string(tv, copyID, restore_copyID);
	නම් (restore_copyID)
	    tuple->tv_copyID = old_copyID;
	r = *tofree;
    }

    ආපසු r;
}

/*
 * Return a textual representation of a Dict in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "copyID" is not zero replace recursive dicts with "...".
 * When "restore_copyID" is FALSE, repeated items in the dictionary are
 * replaced with "...".  May return NULL.
 */
    static char_u *
dict_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    පූර්ණ_සංඛ්‍යා		copyID,
    පූර්ණ_සංඛ්‍යා		restore_copyID)
{
    char_u	*r = NULL;

    නම් (tv->vval.v_dict == NULL)
    {
	// NULL dict is equivalent to empty dict.
	*tofree = NULL;
	r = (char_u *)"{}";
    }
    නැතහොත් නම් (copyID != 0 && tv->vval.v_dict->dv_copyID == copyID
	    && tv->vval.v_dict->dv_hashtab.ht_used != 0)
    {
	*tofree = NULL;
	r = (char_u *)"{...}";
    }
    නැතහොත්
    {
	පූර්ණ_සංඛ්‍යා old_copyID;
	නම් (restore_copyID)
	    old_copyID = tv->vval.v_dict->dv_copyID;

	tv->vval.v_dict->dv_copyID = copyID;
	*tofree = dict2string(tv, copyID, restore_copyID);
	නම් (restore_copyID)
	    tv->vval.v_dict->dv_copyID = old_copyID;
	r = *tofree;
    }

    ආපසු r;
}

/*
 * Return a textual representation of a job or a channel in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * "numbuf" is used for a number.
 * When "composite_val" is FALSE, put quotes around strings as "string()",
 * otherwise does not put quotes around strings.
 * May return NULL.
 */
    static char_u *
jobchan_tv2string(
    typval_T	*tv UNUSED,
    char_u	**tofree UNUSED,
    char_u	*numbuf UNUSED,
    පූර්ණ_සංඛ්‍යා		composite_val UNUSED)
{
    char_u	*r = NULL;

#ifdef FEAT_JOB_CHANNEL
    *tofree = NULL;

    නම් (tv->v_type == VAR_JOB)
	r = job_to_string_buf(tv, numbuf);
    නැතහොත්
	r = channel_to_string_buf(tv, numbuf);

    නම් (composite_val)
    {
	*tofree = string_quote(r, FALSE);
	r = *tofree;
    }
#endif

    ආපසු r;
}

/*
 * Return a textual representation of a class in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * May return NULL.
 */
    static char_u *
class_tv2string(typval_T *tv, char_u **tofree)
{
    char_u	*r = NULL;
    size_t	rsize;
    class_T	*cl = tv->vval.v_class;
    string_T	class_name = {(char_u *)"[unknown]", 9};
    string_T	s = {(char_u *)"class", 5};

    නම් (cl != NULL)
    {
	class_name.string = cl->class_name.string;
	class_name.length = cl->class_name.length;
	නම් (IS_INTERFACE(cl))
	    STR_LITERAL_SET(s, "interface");
	නැතහොත් නම් (IS_ENUM(cl))
	    STR_LITERAL_SET(s, "enum");
    }

    rsize = s.length + 1 + class_name.length + 1;
    r = *tofree = alloc(rsize);
    නම් (r != NULL)
	vim_snprintf((var_na *)r, rsize, "%s %s", s.string, (var_na *)class_name.string);

    ආපසු r;
}

/*
 * Return a textual representation of an Object in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "copyID" is not zero replace recursive object with "...".
 * When "restore_copyID" is FALSE, repeated items in the object are
 * replaced with "...".  May return NULL.
 */
    static char_u *
object_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    පූර්ණ_සංඛ්‍යා		copyID,
    පූර්ණ_සංඛ්‍යා		restore_copyID,
    char_u	*numbuf,
    පූර්ණ_සංඛ්‍යා		echo_style,
    පූර්ණ_සංඛ්‍යා		composite_val)
{
    char_u	*r = NULL;

    object_T	*obj = tv->vval.v_object;
    නම් (obj == NULL || obj->obj_class == NULL)
    {
	*tofree = NULL;
	r = (char_u *)"object of [unknown]";
    }
    නැතහොත් නම් (copyID != 0 && obj->obj_copyID == copyID
	    && obj->obj_class->class_obj_member_count != 0)
    {
	size_t n = 25 + obj->obj_class->class_name.length;
	r = alloc(n);
	නම් (r != NULL)
	    (void)vim_snprintf((var_na *)r, n, "object of %s {...}",
						obj->obj_class->class_name.string);
	*tofree = r;
    }
    නැතහොත්
    {
	පූර්ණ_සංඛ්‍යා old_copyID;
	නම් (restore_copyID)
	    old_copyID = obj->obj_copyID;

	obj->obj_copyID = copyID;
	*tofree = object2string(obj, numbuf, copyID, echo_style,
				restore_copyID, composite_val);
	නම් (restore_copyID)
	    obj->obj_copyID = old_copyID;
	r = *tofree;
    }

    ආපසු r;
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
    පූර්ණ_සංඛ්‍යා		copyID,
    පූර්ණ_සංඛ්‍යා		echo_style,
    පූර්ණ_සංඛ්‍යා		restore_copyID,
    පූර්ණ_සංඛ්‍යා		composite_val)
{
    static පූර්ණ_සංඛ්‍යා	recurse = 0;
    char_u	*r = NULL;

    නම් (recurse >= DICT_MAXNEST)
    {
	නම් (!did_echo_string_emsg)
	{
	    // Only give this message once for a recursive call to avoid
	    // flooding the user with errors.  And stop iterating over lists
	    // and dicts and objects.
	    did_echo_string_emsg = TRUE;
	    emsg(_(e_variable_nested_too_deep_for_displaying));
	}
	*tofree = NULL;
	ආපසු (char_u *)"{E724}";
    }
    ++recurse;

    switch (tv->v_type)
    {
	pak_sa VAR_STRING:
	    r = string_tv2string(tv, tofree, echo_style, composite_val);
	    bha_nga;

	pak_sa VAR_FUNC:
	    r = func_tv2string(tv, tofree, echo_style);
	    bha_nga;

	pak_sa VAR_PARTIAL:
	    නම් (tv->vval.v_partial == NULL
		    || tv->vval.v_partial->pt_obj == NULL)
		r = partial_tv2string(tv, tofree, numbuf, copyID);
	    නැතහොත්
		r = method_tv2string(tv, tofree, echo_style);
	    bha_nga;

	pak_sa VAR_BLOB:
	    r = blob2string(tv->vval.v_blob, tofree, numbuf);
	    bha_nga;

	pak_sa VAR_LIST:
	    r = list_tv2string(tv, tofree, copyID, restore_copyID);
	    bha_nga;

	pak_sa VAR_TUPLE:
	    r = tuple_tv2string(tv, tofree, copyID, restore_copyID);
	    bha_nga;

	pak_sa VAR_DICT:
	    r = dict_tv2string(tv, tofree, copyID, restore_copyID);
	    bha_nga;

	pak_sa VAR_NUMBER:
	pak_sa VAR_UNKNOWN:
	pak_sa VAR_ANY:
	pak_sa VAR_VOID:
	    *tofree = NULL;
	    r = tv_get_string_buf(tv, numbuf);
	    bha_nga;

	pak_sa VAR_JOB:
	pak_sa VAR_CHANNEL:
	    r = jobchan_tv2string(tv, tofree, numbuf, composite_val);
	    bha_nga;

	pak_sa VAR_INSTR:
	    *tofree = NULL;
	    r = (char_u *)"instructions";
	    bha_nga;

	pak_sa VAR_CLASS:
	    r = class_tv2string(tv, tofree);
	    bha_nga;

	pak_sa VAR_OBJECT:
	    r = object_tv2string(tv, tofree, copyID, restore_copyID,
					 numbuf, echo_style, composite_val);
	    bha_nga;

	pak_sa VAR_FLOAT:
	    *tofree = NULL;
	    vim_snprintf((var_na *)numbuf, NUMBUFLEN, "%g", tv->vval.v_float);
	    r = numbuf;
	    bha_nga;

	pak_sa VAR_BOOL:
	pak_sa VAR_SPECIAL:
	    *tofree = NULL;
	    r = (char_u *)get_var_special_name(tv->vval.v_number);
	    bha_nga;

	pak_sa VAR_TYPEALIAS:
	    *tofree = vim_strsave(tv->vval.v_typealias->ta_name);
	    r = *tofree;
	    නම් (r == NULL)
		r = (char_u *)"";
	    bha_nga;
    }

    නම් (--recurse == 0)
	did_echo_string_emsg = FALSE;
    ආපසු r;
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
    පූර්ණ_සංඛ්‍යා		copyID)
{
    ආපසු echo_string_core(tv, tofree, numbuf, copyID, TRUE, FALSE, FALSE);
}

/*
 * Convert the specified byte index of line 'lnum' in buffer 'buf' to a
 * character index.  Works only for loaded buffers. Returns -1 on failure.
 * The index of the first byte and the first character is zero.
 */
    පූර්ණ_සංඛ්‍යා
buf_byteidx_to_charidx(buf_T *buf, පූර්ණ_සංඛ්‍යා lnum, පූර්ණ_සංඛ්‍යා byteidx)
{
    char_u	*str;
    char_u	*t;
    පූර්ණ_සංඛ්‍යා		count;

    නම් (buf == NULL || buf->b_ml.ml_mfp == NULL)
	ආපසු -1;

    නම් (lnum > buf->b_ml.ml_line_count)
	lnum = buf->b_ml.ml_line_count;

    str = ml_get_buf(buf, lnum, FALSE);
    නම් (str == NULL)
	ආපසු -1;

    නම් (*str == NUL)
	ආපසු 0;

    // count the number of characters
    t = str;
    සඳහා (count = 0; *t != NUL && t <= str + byteidx; count++)
	t += mb_ptr2len(t);

    // In insert mode, when the cursor is at the end of a non-empty line,
    // byteidx points to the NUL character immediately past the end of the
    // string. In this case, add one to the character count.
    නම් (*t == NUL && byteidx != 0 && t == str + byteidx)
	count++;

    ආපසු count - 1;
}

/*
 * Convert the specified character index of line 'lnum' in buffer 'buf' to a
 * byte index.  Works only for loaded buffers. Returns -1 on failure.
 * The index of the first byte and the first character is zero.
 */
    පූර්ණ_සංඛ්‍යා
buf_charidx_to_byteidx(buf_T *buf, පූර්ණ_සංඛ්‍යා lnum, පූර්ණ_සංඛ්‍යා charidx)
{
    char_u	*str;
    char_u	*t;

    නම් (buf == NULL || buf->b_ml.ml_mfp == NULL)
	ආපසු -1;

    නම් (lnum > buf->b_ml.ml_line_count)
	lnum = buf->b_ml.ml_line_count;

    str = ml_get_buf(buf, lnum, FALSE);
    නම් (str == NULL)
	ආපසු -1;

    // Convert the character offset to a byte offset
    t = str;
    අතරතුර (*t != NUL && --charidx > 0)
	t += mb_ptr2len(t);

    ආපසු t - str;
}

/*
 * Translate a String variable into a position.
 * Returns NULL when there is an error.
 */
    pos_T *
var2fpos(
    typval_T	*varp,
    පූර්ණ_සංඛ්‍යා		dollar_lnum,	// TRUE when $ is last line
    පූර්ණ_සංඛ්‍යා		*fnum,		// set to fnum for '0, 'A, etc.
    පූර්ණ_සංඛ්‍යා		charcol)	// return character column
{
    char_u		*name;
    static pos_T	pos;
    pos_T		*pp;
    පූර්ණ_සංඛ්‍යා			error = FALSE;

    // Argument can be [lnum, col, coladd].
    නම් (varp->v_type == VAR_LIST)
    {
	list_T		*l;
	පූර්ණ_සංඛ්‍යා		len;
	listitem_T	*li;

	l = varp->vval.v_list;
	නම් (l == NULL)
	    ආපසු NULL;

	// Get the line number
	pos.lnum = list_find_nr(l, 0L, &error);
	නම් (error || pos.lnum <= 0 || pos.lnum > curbuf->b_ml.ml_line_count)
	    ආපසු NULL;	// invalid line number
	නම් (charcol)
	    len = (long)mb_charlen(ml_get(pos.lnum));
	නැතහොත්
	    len = (long)ml_get_len(pos.lnum);

	// Get the column number
	// We accept "$" for the column number: last column.
	li = list_find(l, 1L);
	නම් (li != NULL && li->li_tv.v_type == VAR_STRING
		&& li->li_tv.vval.v_string != NULL
		&& STRCMP(li->li_tv.vval.v_string, "$") == 0)
	{
	    pos.col = len + 1;
	}
	නැතහොත්
	{
	    pos.col = list_find_nr(l, 1L, &error);
	    නම් (error)
		ආපසු NULL;
	}

	// Accept a position up to the NUL after the line.
	නම් (pos.col == 0 || (පූර්ණ_සංඛ්‍යා)pos.col > len + 1)
	    ආපසු NULL;	// invalid column number
	--pos.col;

	// Get the virtual offset.  Defaults to zero.
	pos.coladd = list_find_nr(l, 2L, &error);
	නම් (error)
	    pos.coladd = 0;

	ආපසු &pos;
    }

    නම් (in_vim9script() && check_for_string_arg(varp, 0) == FAIL)
	ආපසු NULL;

    name = tv_get_string_chk(varp);
    නම් (name == NULL)
	ආපසු NULL;

    error = TRUE;
    pos.lnum = 0;
    නම් (name[0] == '.')
    {
	නම් (!in_vim9script() || name[1] == NUL)
	{
	    error = FALSE;
	    // cursor
	    pos = curwin->w_cursor;
	}
    }
    නැතහොත් නම් (name[0] == 'v' && name[1] == NUL)
    {
	// Visual start
	නම් (VIsual_active)
	    pos = VIsual;
	නැතහොත්
	    pos = curwin->w_cursor;
    }
    නැතහොත් නම් (name[0] == '\'')
    {
	නම් (!in_vim9script() || (name[1] != NUL && name[2] == NUL))
	{
	    error = FALSE;
	    // mark
	    pp = getmark_buf_fnum(curbuf, name[1], FALSE, fnum);
	    නම් (pp == NULL || pp == (pos_T *)-1 || pp->lnum <= 0)
		ආපසු NULL;
	    pos = *pp;
	}
    }
    නම් (pos.lnum != 0)
    {
	නම් (charcol)
	    pos.col = buf_byteidx_to_charidx(curbuf, pos.lnum, pos.col);
	ආපසු &pos;
    }

    pos.coladd = 0;

    නම් (name[0] == 'w' && dollar_lnum)
    {
	// the "w_valid" flags are not reset when moving the cursor, but they
	// do matter for update_topline() and validate_botline().
	check_cursor_moved(curwin);

	pos.col = 0;
	නම් (name[1] == '0')		// "w0": first visible line
	{
#ifdef FEAT_TERMINAL
	    නම් (bt_terminal(curwin->w_buffer)
				    && curwin->w_buffer->b_term != NULL
				    && !term_in_normal_mode(curwin->w_buffer))
		may_move_terminal_to_buffer(curwin->w_buffer->b_term, TRUE);
#endif
	    update_topline();
	    // In silent Ex mode topline is zero, but that's not a valid line
	    // number; use one instead.
	    pos.lnum = curwin->w_topline > 0 ? curwin->w_topline : 1;
	    ආපසු &pos;
	}
	නැතහොත් නම් (name[1] == '$')	// "w$": last visible line
	{
#ifdef FEAT_TERMINAL
	    නම් (bt_terminal(curwin->w_buffer)
				    && curwin->w_buffer->b_term != NULL
				    && !term_in_normal_mode(curwin->w_buffer))
		may_move_terminal_to_buffer(curwin->w_buffer->b_term, TRUE);
#endif
	    validate_botline();
	    // In silent Ex mode botline is zero, return zero then.
	    pos.lnum = curwin->w_botline > 0 ? curwin->w_botline - 1 : 0;
	    ආපසු &pos;
	}
    }
    නැතහොත් නම් (name[0] == '$')		// last column or line
    {
	නම් (dollar_lnum)
	{
	    pos.lnum = curbuf->b_ml.ml_line_count;
	    pos.col = 0;
	}
	නැතහොත්
	{
	    pos.lnum = curwin->w_cursor.lnum;
	    නම් (charcol)
		pos.col = (colnr_T)mb_charlen(ml_get_curline());
	    නැතහොත්
		pos.col = ml_get_curline_len();
	}
	ආපසු &pos;
    }
    නම් (in_vim9script() && error)
	semsg(_(e_invalid_value_for_line_number_str), name);
    ආපසු NULL;
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
    පූර්ණ_සංඛ්‍යා
list2fpos(
    typval_T	*arg,
    pos_T	*posp,
    පූර්ණ_සංඛ්‍යා		*fnump,
    colnr_T	*curswantp,
    පූර්ණ_සංඛ්‍යා		charcol)
{
    list_T	*l = arg->vval.v_list;
    long	i = 0;
    long	n;

    // List must be: [fnum, lnum, col, coladd, curswant], where "fnum" is only
    // there when "fnump" isn't NULL; "coladd" and "curswant" are optional.
    නම් (arg->v_type != VAR_LIST
	    || l == NULL
	    || l->lv_len < (fnump == NULL ? 2 : 3)
	    || l->lv_len > (fnump == NULL ? 4 : 5))
	ආපසු FAIL;

    නම් (fnump != NULL)
    {
	n = list_find_nr(l, i++, NULL);	// fnum
	නම් (n < 0)
	    ආපසු FAIL;
	නම් (n == 0)
	    n = curbuf->b_fnum;		// current buffer
	*fnump = n;
    }

    n = list_find_nr(l, i++, NULL);	// lnum
    නම් (n < 0)
	ආපසු FAIL;
    posp->lnum = n;

    n = list_find_nr(l, i++, NULL);	// col
    නම් (n < 0)
	ආපසු FAIL;
    // If character position is specified, then convert to byte position
    // If the line number is zero use the cursor line.
    නම් (charcol)
    {
	buf_T	*buf;

	// Get the text for the specified line in a loaded buffer
	buf = buflist_findnr(fnump == NULL ? curbuf->b_fnum : *fnump);
	නම් (buf == NULL || buf->b_ml.ml_mfp == NULL)
	    ආපසු FAIL;

	n = buf_charidx_to_byteidx(buf,
		  posp->lnum == 0 ? curwin->w_cursor.lnum : posp->lnum, n) + 1;
    }
    posp->col = n;

    n = list_find_nr(l, i, NULL);	// off
    නම් (n < 0)
	posp->coladd = 0;
    නැතහොත්
	posp->coladd = n;

    නම් (curswantp != NULL)
	*curswantp = list_find_nr(l, i + 1, NULL);  // curswant

    ආපසු OK;
}

/*
 * Get the length of an environment variable name.
 * Advance "arg" to the first character after the name.
 * Return 0 for error.
 */
    පූර්ණ_සංඛ්‍යා
get_env_len(char_u **arg)
{
    char_u	*p;
    පූර්ණ_සංඛ්‍යා		len;

    සඳහා (p = *arg; vim_isIDc(*p); ++p)
	;
    නම් (p == *arg)	    // no name found
	ආපසු 0;

    len = (පූර්ණ_සංඛ්‍යා)(p - *arg);
    *arg = p;
    ආපසු len;
}

/*
 * Get the length of the name of a function or internal variable.
 * "arg" is advanced to after the name.
 * Return 0 if something is wrong.
 */
    පූර්ණ_සංඛ්‍යා
get_id_len(char_u **arg)
{
    char_u	*p;
    පූර්ණ_සංඛ්‍යා		len;

    // Find the end of the name.
    සඳහා (p = *arg; eval_isnamec(*p); ++p)
    {
	නම් (*p == ':')
	{
	    // "s:" is start of "s:var", but "n:" is not and can be used in
	    // slice "[n:]".  Also "xx:" is not a namespace.
	    len = (පූර්ණ_සංඛ්‍යා)(p - *arg);
	    නම් ((len == 1 && vim_strchr(NAMESPACE_CHAR, **arg) == NULL)
		    || len > 1)
		bha_nga;
	}
    }
    නම් (p == *arg)	    // no name found
	ආපසු 0;

    len = (පූර්ණ_සංඛ්‍යා)(p - *arg);
    *arg = p;

    ආපසු len;
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
    පූර්ණ_සංඛ්‍යා
get_name_len(
    char_u	**arg,
    char_u	**alias,
    පූර්ණ_සංඛ්‍යා		evaluate,
    පූර්ණ_සංඛ්‍යා		verbose)
{
    පූර්ණ_සංඛ්‍යා		len;
    char_u	*p;
    char_u	*expr_start;
    char_u	*expr_end;

    *alias = NULL;  // default to no alias

    නම් ((*arg)[0] == K_SPECIAL && (*arg)[1] == KS_EXTRA
						  && (*arg)[2] == (පූර්ණ_සංඛ්‍යා)KE_SNR)
    {
	// hard coded <SNR>, already translated
	*arg += 3;
	ආපසු get_id_len(arg) + 3;
    }
    len = eval_fname_script(*arg);
    නම් (len > 0)
    {
	// literal "<SID>", "s:" or "<SNR>"
	*arg += len;
    }

    /*
     * Find the end of the name; check for {} construction.
     */
    p = find_name_end(*arg, &expr_start, &expr_end,
					       len > 0 ? 0 : FNE_CHECK_START);
    නම් (expr_start != NULL)
    {
	char_u	*temp_string;

	නම් (!evaluate)
	{
	    len += (පූර්ණ_සංඛ්‍යා)(p - *arg);
	    *arg = skipwhite(p);
	    ආපසු len;
	}

	/*
	 * Include any <SID> etc in the expanded string:
	 * Thus the -len here.
	 */
	temp_string = make_expanded_name(*arg - len, expr_start, expr_end, p);
	නම් (temp_string == NULL)
	    ආපසු -1;
	*alias = temp_string;
	*arg = skipwhite(p);
	ආපසු (පූර්ණ_සංඛ්‍යා)STRLEN(temp_string);
    }

    len += get_id_len(arg);
    // Only give an error when there is something, otherwise it will be
    // reported at a higher level.
    නම් (len == 0 && verbose && **arg != NUL)
	semsg(_(e_invalid_expression_str), *arg);

    ආපසු len;
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
    char_u	**expr_start,
    char_u	**expr_end,
    පූර්ණ_සංඛ්‍යා		flags)
{
    පූර්ණ_සංඛ්‍යා		mb_nest = 0;
    පූර්ණ_සංඛ්‍යා		br_nest = 0;
    char_u	*p;
    පූර්ණ_සංඛ්‍යා		len;
    පූර්ණ_සංඛ්‍යා		allow_curly = !in_vim9script();

    නම් (expr_start != NULL)
    {
	*expr_start = NULL;
	*expr_end = NULL;
    }

    // Quick check for valid starting character.
    නම් ((flags & FNE_CHECK_START) && !eval_isnamec1(*arg)
					      && (*arg != '{' || !allow_curly))
	ආපසු arg;

    සඳහා (p = arg; *p != NUL
		    && (eval_isnamec(*p)
			|| (*p == '{' && allow_curly)
			|| ((flags & FNE_INCL_BR) && (*p == '['
					 || (*p == '.' && eval_isdictc(p[1]))))
			|| mb_nest != 0
			|| br_nest != 0); MB_PTR_ADV(p))
    {
	නම් (*p == '\'')
	{
	    // skip over 'string' to avoid counting [ and ] inside it.
	    සඳහා (p = p + 1; *p != NUL && *p != '\''; MB_PTR_ADV(p))
		;
	    නම් (*p == NUL)
		bha_nga;
	}
	නැතහොත් නම් (*p == '"')
	{
	    // skip over "str\"ing" to avoid counting [ and ] inside it.
	    සඳහා (p = p + 1; *p != NUL && *p != '"'; MB_PTR_ADV(p))
		නම් (*p == '\\' && p[1] != NUL)
		    ++p;
	    නම් (*p == NUL)
		bha_nga;
	}
	නැතහොත් නම් (br_nest == 0 && mb_nest == 0 && *p == ':')
	{
	    // "s:" is start of "s:var", but "n:" is not and can be used in
	    // slice "[n:]".  Also "xx:" is not a namespace. But {ns}: is.
	    len = (පූර්ණ_සංඛ්‍යා)(p - arg);
	    නම් ((len == 1 && vim_strchr(NAMESPACE_CHAR, *arg) == NULL)
		    || (len > 1 && p[-1] != '}'))
		bha_nga;
	}

	නම් (mb_nest == 0)
	{
	    නම් (*p == '[')
		++br_nest;
	    නැතහොත් නම් (*p == ']')
		--br_nest;
	}

	නම් (br_nest == 0 && allow_curly)
	{
	    නම් (*p == '{')
	    {
		mb_nest++;
		නම් (expr_start != NULL && *expr_start == NULL)
		    *expr_start = p;
	    }
	    නැතහොත් නම් (*p == '}')
	    {
		mb_nest--;
		නම් (expr_start != NULL && mb_nest == 0 && *expr_end == NULL)
		    *expr_end = p;
	    }
	}
    }

    ආපසු p;
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
    static char_u *
make_expanded_name(
    char_u	*in_start,
    char_u	*expr_start,
    char_u	*expr_end,
    char_u	*in_end)
{
    char_u	c1;
    char_u	*retval = NULL;
    char_u	*temp_result;

    නම් (expr_end == NULL || in_end == NULL)
	ආපසු NULL;
    *expr_start	= NUL;
    *expr_end = NUL;
    c1 = *in_end;
    *in_end = NUL;

    temp_result = eval_to_string(expr_start + 1, FALSE, FALSE);
    නම් (temp_result != NULL)
    {
	size_t	retvalsize = (size_t)(expr_start - in_start)
				+ STRLEN(temp_result)
				+ (size_t)(in_end - expr_end) + 1;

	retval = alloc(retvalsize);
	නම් (retval != NULL)
	    vim_snprintf((var_na *)retval, retvalsize, "%s%s%s",
				in_start, temp_result, expr_end + 1);
    }
    vim_free(temp_result);

    *in_end = c1;		// put char back for error messages
    *expr_start = '{';
    *expr_end = '}';

    නම් (retval != NULL)
    {
	temp_result = find_name_end(retval, &expr_start, &expr_end, 0);
	නම් (expr_start != NULL)
	{
	    // Further expansion!
	    temp_result = make_expanded_name(retval, expr_start,
						       expr_end, temp_result);
	    vim_free(retval);
	    retval = temp_result;
	}
    }

    ආපසු retval;
}

/*
 * Return TRUE if character "c" can be used in a variable or function name.
 * Does not include '{' or '}' for magic braces.
 */
    පූර්ණ_සංඛ්‍යා
eval_isnamec(පූර්ණ_සංඛ්‍යා c)
{
    ආපසු ASCII_ISALNUM(c) || c == '_' || c == ':' || c == AUTOLOAD_CHAR;
}

/*
 * Return TRUE if character "c" can be used as the first character in a
 * variable or function name (excluding '{' and '}').
 */
    පූර්ණ_සංඛ්‍යා
eval_isnamec1(පූර්ණ_සංඛ්‍යා c)
{
    ආපසු ASCII_ISALPHA(c) || c == '_';
}

/*
 * Return TRUE if character "c" can be used as the first character of a
 * dictionary key.
 */
    පූර්ණ_සංඛ්‍යා
eval_isdictc(පූර්ණ_සංඛ්‍යා c)
{
    ආපසු ASCII_ISALNUM(c) || c == '_';
}

/*
 * Return true when "p" points to "->" followed by a method name or a lambda.
 * Only called for Vim9 script.
 */
    static bool
method_call_follows(char_u *p)
{
    නම් (p[0] != '-' || p[1] != '>')
	ආපසු false;

    char_u	*after = skipwhite(p + 2);

    ආපසු ASCII_ISALPHA(*after) || *after == '(';
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
    පූර්ණ_සංඛ්‍යා
handle_subscript(
    char_u	**arg,
    char_u	*name_start,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    පූර්ණ_සංඛ්‍යා		verbose)	// give error messages
{
    පූර්ණ_සංඛ්‍යා		evaluate = evalarg != NULL
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    පූර්ණ_සංඛ්‍යා		ret = OK;
    dict_T	*selfdict = NULL;
    පූර්ණ_සංඛ්‍යා		check_white = TRUE;
    පූර්ණ_සංඛ්‍යා		getnext;
    char_u	*p;

    අතරතුර (ret == OK)
    {
	// When at the end of the line and ".name" or a method call follows in
	// the next line then consume the line break.
	p = eval_next_non_blank(*arg, evalarg, &getnext);
	නම් (getnext
	    && ((*p == '.'
		    && ((rettv->v_type == VAR_DICT && eval_isdictc(p[1]))
			|| rettv->v_type == VAR_CLASS
			|| rettv->v_type == VAR_OBJECT))
		|| method_call_follows(p)))
	{
	    *arg = eval_next_line(*arg, evalarg);
	    p = *arg;
	    check_white = FALSE;
	}

	නම් (rettv->v_type == VAR_ANY)
	{
	    char_u	*exp_name;
	    පූර්ණ_සංඛ්‍යා		cc;
	    පූර්ණ_සංඛ්‍යා		idx;
	    ufunc_T	*ufunc;
	    type_T	*type;

	    // Found script from "import {name} as name", script item name must
	    // follow.  "rettv->vval.v_number" has the script ID.
	    නම් (**arg != '.')
	    {
		නම් (verbose)
		    semsg(_(e_expected_dot_after_name_str),
					name_start != NULL ? name_start: *arg);
		ret = FAIL;
		bha_nga;
	    }
	    ++*arg;
	    නම් (IS_WHITE_OR_NUL(**arg))
	    {
		නම් (verbose)
		    emsg(_(e_no_white_space_allowed_after_dot));
		ret = FAIL;
		bha_nga;
	    }

	    // isolate the name
	    exp_name = *arg;
	    අතරතුර (eval_isnamec(**arg))
		++*arg;
	    cc = **arg;
	    **arg = NUL;

	    idx = find_exported(rettv->vval.v_number, exp_name, &ufunc, &type,
		       evalarg == NULL ? NULL : evalarg->eval_cctx,
		       evalarg == NULL ? NULL : evalarg->eval_cstack, verbose);
	    **arg = cc;

	    නම් (idx < 0 && ufunc == NULL)
	    {
		ret = FAIL;
		bha_nga;
	    }
	    නම් (idx >= 0)
	    {
		scriptitem_T    *si = SCRIPT_ITEM(rettv->vval.v_number);
		svar_T		*sv = ((svar_T *)si->sn_var_vals.ga_data) + idx;

		copy_tv(sv->sv_tv, rettv);
	    }
	    නැතහොත්
	    {
		rettv->v_type = VAR_FUNC;
		නම් (**arg == '<')
		{
		    char_u *s = get_generic_func_name(ufunc, arg);
		    නම් (s != NULL)
			rettv->vval.v_string = s;
		    නැතහොත්
			ret = FAIL;
		}
		නැතහොත්
		{
		    rettv->vval.v_string =
			vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
		}
	    }
	    continue;
	}

	නම් ((**arg == '(' && (!evaluate || rettv->v_type == VAR_FUNC
			    || rettv->v_type == VAR_PARTIAL))
		    && (!check_white || !VIM_ISWHITE(*(*arg - 1))))
	{
	    ret = call_func_rettv(arg, evalarg, rettv, evaluate,
							       selfdict, NULL);

	    // Stop the expression evaluation when immediately aborting on
	    // error, or when an interrupt occurred or an exception was thrown
	    // but not caught.
	    නම් (aborting())
	    {
		නම් (ret == OK)
		    clear_tv(rettv);
		ret = FAIL;
	    }
	    dict_unref(selfdict);
	    selfdict = NULL;
	}
	නැතහොත් නම් (p[0] == '-' && p[1] == '>')
	{
	    නම් (in_vim9script())
		*arg = skipwhite(p + 2);
	    නැතහොත්
		*arg = p + 2;
	    නම් (ret == OK && evaluate && rettv->v_type == VAR_VOID)
	    {
		නම් (verbose)
		    emsg(_(e_cannot_use_void_value));
		ret = FAIL;
	    }
	    නැතහොත් නම් (VIM_ISWHITE(**arg))
	    {
		emsg(_(e_no_white_space_allowed_before_parenthesis));
		ret = FAIL;
	    }
	    නැතහොත් නම් ((**arg == '{' && !in_vim9script()) || **arg == '(')
		// expr->{lambda}() or expr->(lambda)()
		ret = eval_lambda(arg, rettv, evalarg, verbose);
	    නැතහොත්
		// expr->name()
		ret = eval_method(arg, rettv, evalarg, verbose);
	}
	// "." is ".name" lookup when we found a dict or when evaluating and
	// scriptversion is at least 2, where string concatenation is "..".
	නැතහොත් නම් (**arg == '['
		|| (**arg == '.' && (rettv->v_type == VAR_DICT
			|| (!evaluate
			    && (*arg)[1] != '.'
			    && !in_old_script(2)))))
	{
	    dict_unref(selfdict);
	    නම් (rettv->v_type == VAR_DICT)
	    {
		selfdict = rettv->vval.v_dict;
		නම් (selfdict != NULL)
		    ++selfdict->dv_refcount;
	    }
	    නැතහොත්
		selfdict = NULL;
	    නම් (eval_index(arg, rettv, evalarg, verbose) == FAIL)
	    {
		clear_tv(rettv);
		ret = FAIL;
	    }
	}
	නැතහොත් නම් (**arg == '.' && (rettv->v_type == VAR_CLASS
					       || rettv->v_type == VAR_OBJECT))
	{
	    // class member: SomeClass.varname
	    // class method: SomeClass.SomeMethod()
	    // class constructor: SomeClass.new()
	    // object member: someObject.varname
	    // object method: someObject.SomeMethod()
	    නම් (class_object_index(arg, rettv, evalarg, verbose) == FAIL)
	    {
		clear_tv(rettv);
		ret = FAIL;
	    }
	}
	නැතහොත්
	    bha_nga;
    }

    // Turn "dict.Func" into a partial for "Func" bound to "dict".
    // Don't do this when "Func" is already a partial that was bound
    // explicitly (pt_auto is FALSE).
    නම් (selfdict != NULL
	    && (rettv->v_type == VAR_FUNC
		|| (rettv->v_type == VAR_PARTIAL
		    && (rettv->vval.v_partial->pt_auto
			|| rettv->vval.v_partial->pt_dict == NULL))))
	selfdict = make_partial(selfdict, rettv);

    dict_unref(selfdict);
    ආපසු ret;
}

/*
 * Make a copy of an item.
 * Lists and Dictionaries are also copied.  A deep copy if "deep" is set.
 * "top" is TRUE for the toplevel of copy().
 * For deepcopy() "copyID" is zero for a full copy or the ID for when a
 * reference to an already copied list/dict can be used.
 * Returns FAIL or OK.
 */
    පූර්ණ_සංඛ්‍යා
item_copy(
    typval_T	*from,
    typval_T	*to,
    පූර්ණ_සංඛ්‍යා		deep,
    පූර්ණ_සංඛ්‍යා		top,
    පූර්ණ_සංඛ්‍යා		copyID)
{
    static පූර්ණ_සංඛ්‍යා	recurse = 0;
    පූර්ණ_සංඛ්‍යා		ret = OK;

    නම් (recurse >= DICT_MAXNEST)
    {
	emsg(_(e_variable_nested_too_deep_for_making_copy));
	ආපසු FAIL;
    }
    ++recurse;

    switch (from->v_type)
    {
	pak_sa VAR_NUMBER:
	pak_sa VAR_FLOAT:
	pak_sa VAR_STRING:
	pak_sa VAR_FUNC:
	pak_sa VAR_PARTIAL:
	pak_sa VAR_BOOL:
	pak_sa VAR_SPECIAL:
	pak_sa VAR_JOB:
	pak_sa VAR_CHANNEL:
	pak_sa VAR_INSTR:
	pak_sa VAR_CLASS:
	pak_sa VAR_OBJECT:
	pak_sa VAR_TYPEALIAS:
	    copy_tv(from, to);
	    bha_nga;
	pak_sa VAR_LIST:
	    to->v_type = VAR_LIST;
	    to->v_lock = 0;
	    නම් (from->vval.v_list == NULL)
		to->vval.v_list = NULL;
	    නැතහොත් නම් (copyID != 0 && from->vval.v_list->lv_copyID == copyID)
	    {
		// use the copy made earlier
		to->vval.v_list = from->vval.v_list->lv_copylist;
		++to->vval.v_list->lv_refcount;
	    }
	    නැතහොත්
	    {
		to->vval.v_list = list_copy(from->vval.v_list,
							    deep, top, copyID);
		නම් (to->vval.v_list == NULL)
		    ret = FAIL;
	    }
	    bha_nga;
	pak_sa VAR_TUPLE:
	    to->v_type = VAR_TUPLE;
	    to->v_lock = 0;
	    නම් (from->vval.v_tuple == NULL)
		to->vval.v_tuple = NULL;
	    නැතහොත් නම් (copyID != 0 && from->vval.v_tuple->tv_copyID == copyID)
	    {
		// use the copy made earlier
		to->vval.v_tuple = from->vval.v_tuple->tv_copytuple;
		++to->vval.v_tuple->tv_refcount;
	    }
	    නැතහොත්
	    {
		to->vval.v_tuple = tuple_copy(from->vval.v_tuple,
							    deep, top, copyID);
		නම් (to->vval.v_tuple == NULL)
		    ret = FAIL;
	    }
	    bha_nga;
	pak_sa VAR_BLOB:
	    to->v_type = VAR_BLOB;
	    to->v_lock = 0;
	    නම් (from->vval.v_blob == NULL)
		to->vval.v_blob = NULL;
	    නැතහොත්
	    {
		to->vval.v_blob = blob_copy(from->vval.v_blob);
		නම් (to->vval.v_blob == NULL)
		    ret = FAIL;
	    }
	    bha_nga;
	pak_sa VAR_DICT:
	    to->v_type = VAR_DICT;
	    to->v_lock = 0;
	    නම් (from->vval.v_dict == NULL)
		to->vval.v_dict = NULL;
	    නැතහොත් නම් (copyID != 0 && from->vval.v_dict->dv_copyID == copyID)
	    {
		// use the copy made earlier
		to->vval.v_dict = from->vval.v_dict->dv_copydict;
		++to->vval.v_dict->dv_refcount;
	    }
	    නැතහොත්
	    {
		to->vval.v_dict = dict_copy(from->vval.v_dict,
							    deep, top, copyID);
		නම් (to->vval.v_dict == NULL)
		    ret = FAIL;
	    }
	    bha_nga;
	pak_sa VAR_UNKNOWN:
	pak_sa VAR_ANY:
	pak_sa VAR_VOID:
	    internal_error_no_abort("item_copy(UNKNOWN)");
	    ret = FAIL;
    }
    --recurse;
    ආපසු ret;
}

    void
echo_one(typval_T *rettv, පූර්ණ_සංඛ්‍යා with_space, පූර්ණ_සංඛ්‍යා *atstart, පූර්ණ_සංඛ්‍යා *needclr)
{
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    char_u	*p = echo_string(rettv, &tofree, numbuf, get_copyID());

    නම් (*atstart)
    {
	*atstart = FALSE;
	// Call msg_start() after eval1(), evaluating the expression
	// may cause a message to appear.
	නම් (with_space)
	{
	    // Mark the saved text as finishing the line, so that what
	    // follows is displayed on a new line when scrolling back
	    // at the more prompt.
	    msg_sb_eol();
	    msg_start();
	}
    }
    නැතහොත් නම් (with_space)
	msg_puts_attr(" ", echo_attr);

    නම් (p != NULL)
	සඳහා ( ; *p != NUL && !got_int; ++p)
	{
	    නම් (*p == '\n' || *p == '\r' || *p == TAB)
	    {
		නම් (*p != TAB && *needclr)
		{
		    // remove any text still there from the command
		    msg_clr_eos();
		    *needclr = FALSE;
		}
		msg_putchar_attr(*p, echo_attr);
	    }
	    නැතහොත්
	    {
		නම් (has_mbyte)
		{
		    පූර්ණ_සංඛ්‍යා i = (*mb_ptr2len)(p);

		    (void)msg_outtrans_len_attr(p, i, echo_attr);
		    p += i - 1;
		}
		නැතහොත්
		    (void)msg_outtrans_len_attr(p, 1, echo_attr);
	    }
	}
    vim_free(tofree);
}

/*
 * ":echo expr1 ..."	print each argument separated with a space, add a
 *			newline at the end.
 * ":echon expr1 ..."	print each argument plain.
 */
    void
ex_echo(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    typval_T	rettv;
    char_u	*arg_start;
    පූර්ණ_සංඛ්‍යා		needclr = TRUE;
    පූර්ණ_සංඛ්‍යා		atstart = TRUE;
    පූර්ණ_සංඛ්‍යා		did_emsg_before = did_emsg;
    පූර්ණ_සංඛ්‍යා		called_emsg_before = called_emsg;
    evalarg_T	evalarg;

    fill_evalarg_from_eap(&evalarg, eap, eap->skip);

    නම් (eap->skip)
	++emsg_skip;
    අතරතුර ((!ends_excmd2(eap->cmd, arg) || *arg == '"') && !got_int)
    {
	// If eval1() causes an error message the text from the command may
	// still need to be cleared. E.g., "echo 22,44".
	need_clr_eos = needclr;

	arg_start = arg;
	නම් (eval1(&arg, &rettv, &evalarg) == FAIL)
	{
	    /*
	     * Report the invalid expression unless the expression evaluation
	     * has been cancelled due to an aborting error, an interrupt, or an
	     * exception.
	     */
	    නම් (!aborting() && did_emsg == did_emsg_before
					  && called_emsg == called_emsg_before)
		semsg(_(e_invalid_expression_str), arg_start);
	    need_clr_eos = FALSE;
	    bha_nga;
	}
	need_clr_eos = FALSE;

	නම් (!eap->skip)
	{
	    නම් (rettv.v_type == VAR_VOID)
	    {
		semsg(_(e_expression_does_not_result_in_value_str), arg_start);
		bha_nga;
	    }
	    echo_one(&rettv, eap->cmdidx == CMD_echo, &atstart, &needclr);
	}

	clear_tv(&rettv);
	arg = skipwhite(arg);
    }
    set_nextcmd(eap, arg);
    clear_evalarg(&evalarg, eap);

    නම් (eap->skip)
	--emsg_skip;
    නැතහොත්
    {
	// remove text that may still be there from the command
	නම් (needclr)
	    msg_clr_eos();
	නම් (eap->cmdidx == CMD_echo)
	    msg_end();
    }
}

/*
 * ":echohl {name}".
 */
    void
ex_echohl(exarg_T *eap)
{
    echo_attr = syn_name2attr(eap->arg);
}

/*
 * Returns the :echo attribute
 */
    පූර්ණ_සංඛ්‍යා
get_echo_attr(void)
{
    ආපසු echo_attr;
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
    void
ex_execute(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    typval_T	rettv;
    පූර්ණ_සංඛ්‍යා		ret = OK;
    char_u	*p;
    garray_T	ga;
    පූර්ණ_සංඛ්‍යා		len;
    long	start_lnum = SOURCING_LNUM;

    ga_init2(&ga, 1, 80);

    නම් (eap->skip)
	++emsg_skip;
    අතරතුර (!ends_excmd2(eap->cmd, arg) || *arg == '"')
    {
	ret = eval1_emsg(&arg, &rettv, eap);
	නම් (ret == FAIL)
	    bha_nga;

	නම් (!eap->skip)
	{
	    char_u   buf[NUMBUFLEN];

	    නම් (eap->cmdidx == CMD_execute)
	    {
		නම් (rettv.v_type == VAR_CHANNEL || rettv.v_type == VAR_JOB)
		{
		    semsg(_(e_using_invalid_value_as_string_str),
						  vartype_name(rettv.v_type));
		    p = NULL;
		}
		නැතහොත්
		    p = tv_get_string_buf(&rettv, buf);
	    }
	    නැතහොත්
		p = tv_stringify(&rettv, buf);
	    නම් (p == NULL)
	    {
		clear_tv(&rettv);
		ret = FAIL;
		bha_nga;
	    }
	    len = (පූර්ණ_සංඛ්‍යා)STRLEN(p);
	    නම් (ga_grow(&ga, len + 2) == FAIL)
	    {
		clear_tv(&rettv);
		ret = FAIL;
		bha_nga;
	    }
	    නම් (ga.ga_len)
		((char_u *)(ga.ga_data))[ga.ga_len++] = ' ';
	    STRCPY((char_u *)(ga.ga_data) + ga.ga_len, p);
	    ga.ga_len += len;
	}

	clear_tv(&rettv);
	arg = skipwhite(arg);
    }

    නම් (ret != FAIL && ga.ga_data != NULL)
    {
	// use the first line of continuation lines for messages
	SOURCING_LNUM = start_lnum;

	නම් (eap->cmdidx == CMD_echomsg
		|| eap->cmdidx == CMD_echowindow
		|| eap->cmdidx == CMD_echoerr)
	{
	    // Mark the already saved text as finishing the line, so that what
	    // follows is displayed on a new line when scrolling back at the
	    // more prompt.
	    msg_sb_eol();
	}

	නම් (eap->cmdidx == CMD_echomsg)
	{
	    msg_attr(ga.ga_data, echo_attr);
	    out_flush();
	}
	නැතහොත් නම් (eap->cmdidx == CMD_echowindow)
	{
#ifdef HAS_MESSAGE_WINDOW
	    start_echowindow(eap->addr_count > 0 ? eap->line2 : 0);
#endif
	    msg_attr(ga.ga_data, echo_attr);
#ifdef HAS_MESSAGE_WINDOW
	    end_echowindow();
#endif
	}
	නැතහොත් නම් (eap->cmdidx == CMD_echoconsole)
	{
	    ui_write(ga.ga_data, (පූර්ණ_සංඛ්‍යා)STRLEN(ga.ga_data), TRUE);
	    ui_write((char_u *)"\r\n", 2, TRUE);
	}
	නැතහොත් නම් (eap->cmdidx == CMD_echoerr)
	{
	    පූර්ණ_සංඛ්‍යා		save_did_emsg = did_emsg;

	    // We don't want to abort following commands, restore did_emsg.
	    emsg(ga.ga_data);
	    නම් (!force_abort)
		did_emsg = save_did_emsg;
	}
	නැතහොත් නම් (eap->cmdidx == CMD_execute)
	{
	    පූර්ණ_සංඛ්‍යා save_sticky_cmdmod_flags = sticky_cmdmod_flags;

	    // "legacy exe cmd" and "vim9cmd exe cmd" applies to "cmd".
	    sticky_cmdmod_flags = cmdmod.cmod_flags
						& (CMOD_LEGACY | CMOD_VIM9CMD);
	    do_cmdline((char_u *)ga.ga_data,
		       eap->ea_getline, eap->cookie, DOCMD_NOWAIT|DOCMD_VERBOSE);
	    sticky_cmdmod_flags = save_sticky_cmdmod_flags;
	}
    }

    ga_clear(&ga);

    නම් (eap->skip)
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
find_option_end(char_u **arg, පූර්ණ_සංඛ්‍යා *scope)
{
    char_u	*p = *arg;

    ++p;
    නම් (*p == 'g' && p[1] == ':')
    {
	*scope = OPT_GLOBAL;
	p += 2;
    }
    නැතහොත් නම් (*p == 'l' && p[1] == ':')
    {
	*scope = OPT_LOCAL;
	p += 2;
    }
    නැතහොත්
	*scope = 0;

    නම් (!ASCII_ISALPHA(*p))
	ආපසු NULL;
    *arg = p;

    නම් (p[0] == 't' && p[1] == '_' && p[2] != NUL && p[3] != NUL)
	p += 4;	    // termcap option
    නැතහොත්
	අතරතුර (ASCII_ISALPHA(*p))
	    ++p;
    ආපසු p;
}

/*
 * Display script name where an item was last set.
 * Should only be invoked when 'verbose' is non-zero.
 */
    void
last_set_msg(sctx_T script_ctx)
{
    char_u *p;

    නම් (script_ctx.sc_sid == 0)
	ආපසු;

    p = home_replace_save(NULL, get_scriptname(script_ctx.sc_sid));
    නම් (p == NULL)
	ආපසු;

    verbose_enter();
    msg_puts(_("\n\tLast set from "));
    msg_puts((var_na *)p);
    නම් (script_ctx.sc_lnum > 0)
    {
	msg_puts(_(line_msg));
	msg_outnum((long)script_ctx.sc_lnum);
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
    typval_T	*expr,
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
    නම් (regmatch.regprog != NULL)
    {
	char_u	*tail = str;
	char_u	*end = str + len;
	පූර්ණ_සංඛ්‍යා	do_all = (flags[0] == 'g');
	පූර්ණ_සංඛ්‍යා	sublen;
	පූර්ණ_සංඛ්‍යා	i;
	char_u	*zero_width = NULL;

	අතරතුර (vim_regexec_nl(&regmatch, str, (colnr_T)(tail - str)))
	{
	    // Skip empty match except for first match.
	    නම් (regmatch.startp[0] == regmatch.endp[0])
	    {
		නම් (zero_width == regmatch.startp[0])
		{
		    // avoid getting stuck on a match with an empty string
		    i = mb_ptr2len(tail);
		    mch_memmove((char_u *)ga.ga_data + ga.ga_len, tail,
								   (size_t)i);
		    ga.ga_len += i;
		    tail += i;
		    continue;
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
	    sublen = vim_regsub(&regmatch, sub, expr, tail, 0, REGSUB_MAGIC);
	    නම් (sublen <= 0)
	    {
		ga_clear(&ga);
		bha_nga;
	    }
	    නම් (ga_grow(&ga, (පූර්ණ_සංඛ්‍යා)((end - tail) + sublen -
			    (regmatch.endp[0] - regmatch.startp[0]))) == FAIL)
	    {
		ga_clear(&ga);
		bha_nga;
	    }

	    // copy the text up to where the match is
	    i = (පූර්ණ_සංඛ්‍යා)(regmatch.startp[0] - tail);
	    mch_memmove((char_u *)ga.ga_data + ga.ga_len, tail, (size_t)i);
	    // add the substituted text
	    (void)vim_regsub(&regmatch, sub, expr,
				  (char_u *)ga.ga_data + ga.ga_len + i, sublen,
				  REGSUB_COPY | REGSUB_MAGIC);
	    ga.ga_len += i + sublen - 1;
	    tail = regmatch.endp[0];
	    නම් (*tail == NUL)
		bha_nga;
	    නම් (!do_all)
		bha_nga;
	}

	නම් (ga.ga_data != NULL)
	{
	    STRCPY((var_na *)ga.ga_data + ga.ga_len, tail);
	    ga.ga_len += (පූර්ණ_සංඛ්‍යා)(end - tail);
	}

	vim_regfree(regmatch.regprog);
    }

    නම් (ga.ga_data != NULL)
    {
	str = (char_u *)ga.ga_data;
	len = (size_t)ga.ga_len;
    }
    ret = vim_strnsave(str, len);
    ga_clear(&ga);
    නම් (p_cpo == empty_option)
	p_cpo = save_cpo;
    නැතහොත්
    {
	// Darn, evaluating {sub} expression or {expr} changed the value.
	// If it's still empty it was changed and restored, need to restore in
	// the complicated way.
	නම් (*p_cpo == NUL)
	    set_option_value_give_err((char_u *)"cpo", 0L, save_cpo, 0);
	free_string_option(save_cpo);
    }

    නම් (ret_len != NULL)
	*ret_len = len;

    ආපසු ret;
}
