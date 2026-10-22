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

#གལ་ཏེ defined(FEAT_EVAL)

#ifdef VMS
# include <float.h>
#endif

#define NAMESPACE_CHAR	(char_u *)"abglstvw"

static ཧྲིལ eval0_simple_funccal(char_u *arg, typval_T *rettv, exarg_T *eap, evalarg_T *evalarg);
static ཧྲིལ eval2(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
static ཧྲིལ eval3(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
static ཧྲིལ eval4(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
static ཧྲིལ eval5(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
static ཧྲིལ eval6(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
static ཧྲིལ eval7(char_u **arg, typval_T *rettv, evalarg_T *evalarg, ཧྲིལ want_string);
static ཧྲིལ eval8(char_u **arg, typval_T *rettv, evalarg_T *evalarg, ཧྲིལ want_string);
static ཧྲིལ eval9(char_u **arg, typval_T *rettv, evalarg_T *evalarg, ཧྲིལ want_string);
static ཧྲིལ eval9_leader(typval_T *rettv, ཧྲིལ numeric_only, char_u *start_leader, char_u **end_leaderp);

static char_u *make_expanded_name(char_u *in_start, char_u *expr_start, char_u *expr_end, char_u *in_end);

/*
 * Return "n1" divided by "n2", taking care of dividing by zero.
 * If "failed" is not NULL set it to TRUE when dividing by zero fails.
 */
	varnumber_T
num_divide(varnumber_T n1, varnumber_T n2, ཧྲིལ *failed)
{
    varnumber_T	result;

    གལ་ཏེ (n2 == 0)
    {
	གལ་ཏེ (in_vim9script())
	{
	    emsg(_(e_divide_by_zero));
	    གལ་ཏེ (failed != NULL)
		*failed = TRUE;
	}
	གལ་ཏེ (n1 == 0)
	    result = VARNUM_MIN; // similar to NaN
	དེ་མིན གལ་ཏེ (n1 < 0)
	    result = -VARNUM_MAX;
	དེ་མིན
	    result = VARNUM_MAX;
    }
    དེ་མིན གལ་ཏེ (n1 == VARNUM_MIN && n2 == -1)
    {
	// specific case: trying to do VARNUM_MIN / -1 results in a positive
	// number that doesn't fit in varnumber_T and causes an FPE
	result = VARNUM_MAX;
    }
    དེ་མིན
	result = n1 / n2;

    ཕྱིར result;
}

/*
 * Return "n1" modulus "n2", taking care of dividing by zero.
 * If "failed" is not NULL set it to TRUE when dividing by zero fails.
 */
	varnumber_T
num_modulus(varnumber_T n1, varnumber_T n2, ཧྲིལ *failed)
{
    གལ་ཏེ (n2 == 0 && in_vim9script())
    {
	emsg(_(e_divide_by_zero));
	གལ་ཏེ (failed != NULL)
	    *failed = TRUE;
    }
    ཕྱིར (n2 == 0) ? 0 : (n1 % n2);
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

#གལ་ཏེ defined(EXITFREE)
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
fill_evalarg_from_eap(evalarg_T *evalarg, exarg_T *eap, ཧྲིལ skip)
{
    init_evalarg(evalarg);
    evalarg->eval_flags = skip ? 0 : EVAL_EVALUATE;

    གལ་ཏེ (eap == NULL)
	ཕྱིར;

    evalarg->eval_cstack = eap->cstack;
    གལ་ཏེ (sourcing_a_script(eap) || eap->ea_getline == get_list_line)
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
    ཧྲིལ
eval_to_bool(
    char_u	*arg,
    ཧྲིལ		*error,
    exarg_T	*eap,
    ཧྲིལ		skip,	    // only parse, don't execute
    ཧྲིལ		use_simple_function)
{
    typval_T	tv;
    varnumber_T	retval = FALSE;
    evalarg_T	evalarg;
    ཧྲིལ		r;

    fill_evalarg_from_eap(&evalarg, eap, skip);

    གལ་ཏེ (skip)
	++emsg_skip;
    གལ་ཏེ (use_simple_function)
	r = eval0_simple_funccal(arg, &tv, eap, &evalarg);
    དེ་མིན
	r = eval0(arg, &tv, eap, &evalarg);
    གལ་ཏེ (r == FAIL)
	*error = TRUE;
    དེ་མིན
    {
	*error = FALSE;
	གལ་ཏེ (!skip)
	{
	    གལ་ཏེ (in_vim9script())
		retval = tv_get_bool_chk(&tv, error);
	    དེ་མིན
		retval = (tv_get_number_chk(&tv, error) != 0);
	    clear_tv(&tv);
	}
    }
    གལ་ཏེ (skip)
	--emsg_skip;
    clear_evalarg(&evalarg, eap);

    ཕྱིར (ཧྲིལ)retval;
}

/*
 * Call eval1() and give an error message if not done at a lower level.
 */
    static ཧྲིལ
eval1_emsg(char_u **arg, typval_T *rettv, exarg_T *eap)
{
    char_u	*start = *arg;
    ཧྲིལ		ret;
    ཧྲིལ		did_emsg_before = did_emsg;
    ཧྲིལ		called_emsg_before = called_emsg;
    evalarg_T	evalarg;

    fill_evalarg_from_eap(&evalarg, eap, eap != NULL && eap->skip);

    ret = eval1(arg, rettv, &evalarg);
    གལ་ཏེ (ret == FAIL)
    {
	// Report the invalid expression unless the expression evaluation has
	// been cancelled due to an aborting error, an interrupt, or an
	// exception, or we already gave a more specific error.
	// Also check called_emsg for when using assert_fails().
	གལ་ཏེ (!aborting() && did_emsg == did_emsg_before
					  && called_emsg == called_emsg_before)
	    semsg(_(e_invalid_expression_str), start);
    }
    clear_evalarg(&evalarg, eap);
    ཕྱིར ret;
}

/*
 * Return whether a typval is a valid expression to pass to eval_expr_typval()
 * or eval_expr_to_bool().  An empty string returns FALSE;
 */
    ཧྲིལ
eval_expr_valid_arg(typval_T *tv)
{
    ཕྱིར tv->v_type != VAR_UNKNOWN
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
    གལ་ཏེ (expr->v_type != VAR_PARTIAL)
	ཕྱིར NULL;

    partial_T *partial = expr->vval.v_partial;
    གལ་ཏེ (partial == NULL)
	ཕྱིར NULL;
    གལ་ཏེ (partial->pt_func == NULL
	    || partial->pt_func->uf_def_status == UF_NOT_COMPILED)
	ཕྱིར NULL;

    ཕྱིར create_funccal(partial->pt_func, rettv);
}

/*
 * Evaluate a partial.
 * Pass arguments "argv[argc]".
 * "fc_arg" is from eval_expr_get_funccal() or NULL;
 * Return the result in "rettv" and OK or FAIL.
 */
    static ཧྲིལ
eval_expr_partial(
    typval_T	*expr,
    typval_T	*argv,
    ཧྲིལ		argc,
    funccall_T	*fc_arg,
    typval_T	*rettv)
{
    partial_T	*partial = expr->vval.v_partial;

    གལ་ཏེ (partial == NULL)
	ཕྱིར FAIL;

    གལ་ཏེ (partial->pt_func != NULL
			&& partial->pt_func->uf_def_status != UF_NOT_COMPILED)
    {
	funccall_T	*fc = fc_arg != NULL ? fc_arg
				: create_funccal(partial->pt_func, rettv);
	ཧྲིལ		r;

	གལ་ཏེ (fc == NULL)
	    ཕྱིར FAIL;

	// Shortcut to call a compiled function with minimal overhead.
	གལ་ཏེ (partial->pt_obj != NULL)
	    partial->pt_obj->obj_refcount++;
	r = call_def_function(partial->pt_func, argc, argv, DEF_USE_PT_ARGV,
					partial, partial->pt_obj, fc, rettv);
	གལ་ཏེ (fc_arg == NULL)
	    remove_funccal();
	གལ་ཏེ (r == FAIL)
	    ཕྱིར FAIL;
    }
    དེ་མིན
    {
	char_u		*s = partial_name(partial);
	funcexe_T	funcexe;

	གལ་ཏེ (s == NULL || *s == NUL)
	    ཕྱིར FAIL;

	CLEAR_FIELD(funcexe);
	funcexe.fe_evaluate = TRUE;
	funcexe.fe_partial = partial;
	གལ་ཏེ (call_func(s, -1, rettv, argc, argv, &funcexe) == FAIL)
	    ཕྱིར FAIL;
    }

    ཕྱིར OK;
}

/*
 * Evaluate an expression which is a function.
 * Pass arguments "argv[argc]".
 * Return the result in "rettv" and OK or FAIL.
 */
    static ཧྲིལ
eval_expr_func(
    typval_T	*expr,
    typval_T	*argv,
    ཧྲིལ		argc,
    typval_T	*rettv)
{
    funcexe_T	funcexe;
    char_u	buf[NUMBUFLEN];
    char_u	*s;

    གལ་ཏེ (expr->v_type == VAR_FUNC)
	s = expr->vval.v_string;
    དེ་མིན
	s = tv_get_string_buf_chk_strict(expr, buf, in_vim9script());
    གལ་ཏེ (s == NULL || *s == NUL)
	ཕྱིར FAIL;

    CLEAR_FIELD(funcexe);
    funcexe.fe_evaluate = TRUE;
    གལ་ཏེ (call_func(s, -1, rettv, argc, argv, &funcexe) == FAIL)
	ཕྱིར FAIL;

    ཕྱིར OK;
}

/*
 * Evaluate an expression, which is a string.
 * Return the result in "rettv" and OK or FAIL.
 */
    static ཧྲིལ
eval_expr_string(
    typval_T	*expr,
    typval_T	*rettv)
{
    char_u	*s;
    char_u	buf[NUMBUFLEN];

    s = tv_get_string_buf_chk_strict(expr, buf, in_vim9script());
    གལ་ཏེ (s == NULL)
	ཕྱིར FAIL;

    s = skipwhite(s);
    གལ་ཏེ (eval1_emsg(&s, rettv, NULL) == FAIL)
	ཕྱིར FAIL;

    གལ་ཏེ (*skipwhite(s) != NUL)  // check for trailing chars after expr
    {
	clear_tv(rettv);
	semsg(_(e_invalid_expression_str), s);
	ཕྱིར FAIL;
    }

    ཕྱིར OK;
}

/*
 * Evaluate an expression, which can be a function, partial or string.
 * Pass arguments "argv[argc]".
 * If "want_func" is TRUE treat a string as a function name, not an expression.
 * "fc_arg" is from eval_expr_get_funccal() or NULL;
 * Return the result in "rettv" and OK or FAIL.
 */
    ཧྲིལ
eval_expr_typval(
	typval_T    *expr,
	ཧྲིལ	    want_func,
	typval_T    *argv,
	ཧྲིལ	    argc,
	funccall_T  *fc_arg,
	typval_T    *rettv)
{
    གལ་ཏེ (expr->v_type == VAR_PARTIAL)
	ཕྱིར eval_expr_partial(expr, argv, argc, fc_arg, rettv);
    གལ་ཏེ (expr->v_type == VAR_INSTR)
	ཕྱིར exe_typval_instr(expr, rettv);
    གལ་ཏེ (expr->v_type == VAR_FUNC || want_func)
	ཕྱིར eval_expr_func(expr, argv, argc, rettv);

    ཕྱིར eval_expr_string(expr, rettv);
}

/*
 * Like eval_to_bool() but using a typval_T instead of a string.
 * Works for string, funcref and partial.
 */
    ཧྲིལ
eval_expr_to_bool(typval_T *expr, ཧྲིལ *error)
{
    typval_T	rettv;
    ཧྲིལ		res;

    གལ་ཏེ (eval_expr_typval(expr, FALSE, NULL, 0, NULL, &rettv) == FAIL)
    {
	*error = TRUE;
	ཕྱིར FALSE;
    }
    res = (tv_get_bool_chk(&rettv, error) != 0);
    clear_tv(&rettv);
    ཕྱིར res;
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
    ཧྲིལ		skip)	    // only parse, don't execute
{
    typval_T	tv;
    char_u	*retval;
    evalarg_T	evalarg;

    fill_evalarg_from_eap(&evalarg, eap, skip);
    གལ་ཏེ (skip)
	++emsg_skip;
    གལ་ཏེ (eval0(arg, &tv, eap, &evalarg) == FAIL || skip)
	retval = NULL;
    དེ་མིན
    {
	retval = vim_strsave(tv_get_string(&tv));
	clear_tv(&tv);
    }
    གལ་ཏེ (skip)
	--emsg_skip;
    clear_evalarg(&evalarg, eap);

    ཕྱིར retval;
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
    གལ་ཏེ (evalarg->eval_tofree == NULL)
	ཕྱིར;

    གལ་ཏེ (ga_grow(&evalarg->eval_tofree_ga, 1) == OK)
	((char_u **)evalarg->eval_tofree_ga.ga_data)
	    [evalarg->eval_tofree_ga.ga_len++]
	    = evalarg->eval_tofree;
    དེ་མིན
	vim_free(evalarg->eval_tofree);
}

/*
 * After using "evalarg" filled from "eap": free the memory.
 */
    void
clear_evalarg(evalarg_T *evalarg, exarg_T *eap)
{
    གལ་ཏེ (evalarg == NULL)
	ཕྱིར;

    garray_T *etga = &evalarg->eval_tofree_ga;

    གལ་ཏེ (evalarg->eval_tofree != NULL || evalarg->eval_using_cmdline)
    {
	གལ་ཏེ (eap != NULL)
	{
	    // We may need to keep the original command line, e.g. for
	    // ":let" it has the variable names.  But we may also need
	    // the new one, "nextcmd" points into it.  Keep both.
	    vim_free(eap->cmdline_tofree);
	    eap->cmdline_tofree = *eap->cmdlinep;

	    གལ་ཏེ (evalarg->eval_using_cmdline && etga->ga_len > 0)
	    {
		// "nextcmd" points into the last line in eval_tofree_ga,
		// need to keep it around.
		--etga->ga_len;
		*eap->cmdlinep = ((char_u **)etga->ga_data)[etga->ga_len];
		vim_free(evalarg->eval_tofree);
	    }
	    དེ་མིན
		*eap->cmdlinep = evalarg->eval_tofree;
	}
	དེ་མིན
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
    ཧྲིལ
skip_expr(char_u **pp, evalarg_T *evalarg)
{
    typval_T	rettv;

    *pp = skipwhite(*pp);
    ཕྱིར eval1(pp, &rettv, evalarg);
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
    ཧྲིལ
skip_expr_concatenate(
	char_u	    **arg,
	char_u	    **start,
	char_u	    **end,
	evalarg_T   *evalarg)
{
    typval_T	rettv;
    ཧྲིལ		res;
    ཧྲིལ		vim9script = in_vim9script();
    garray_T    *gap = evalarg == NULL ? NULL : &evalarg->eval_ga;
    garray_T    *freegap = evalarg == NULL ? NULL : &evalarg->eval_freega;
    ཧྲིལ		save_flags = evalarg == NULL ? 0 : evalarg->eval_flags;
    ཧྲིལ		evaluate = evalarg == NULL
			       ? FALSE : (evalarg->eval_flags & EVAL_EVALUATE);

    གལ་ཏེ (vim9script && evaluate
	       && (evalarg->eval_cookie != NULL || evalarg->eval_cctx != NULL))
    {
	ga_init2(gap, parimaa_na(char_u *), 10);
	// leave room for "start"
	གལ་ཏེ (ga_grow(gap, 1) == OK)
	    ++gap->ga_len;
	ga_init2(freegap, parimaa_na(char_u *), 10);
    }
    *start = *arg;

    // Don't evaluate the expression.
    གལ་ཏེ (evalarg != NULL)
	evalarg->eval_flags &= ~EVAL_EVALUATE;
    *arg = skipwhite(*arg);
    res = eval1(arg, &rettv, evalarg);
    *end = *arg;
    གལ་ཏེ (evalarg != NULL)
	evalarg->eval_flags = save_flags;

    གལ་ཏེ (vim9script && evaluate
	    && (evalarg->eval_cookie != NULL || evalarg->eval_cctx != NULL))
    {
	གལ་ཏེ (evalarg->eval_ga.ga_len == 1)
	{
	    // just the one line, no need to concatenate
	    ga_clear(gap);
	    gap->ga_itemsize = 0;
	}
	དེ་མིན
	{
	    char_u	    *p;
	    size_t	    endoff = STRLEN(*arg);

	    // Line breaks encountered, concatenate all the lines.
	    *((char_u **)gap->ga_data) = *start;
	    p = ga_concat_strings(gap, " ");

	    // free the lines only when using getsourceline()
	    གལ་ཏེ (evalarg->eval_cookie != NULL)
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
	    དེ་མིན
	    {
		ga_clear(gap);

		// free lines that were explicitly marked for freeing
		ga_clear_strings(freegap);
	    }

	    gap->ga_itemsize = 0;
	    གལ་ཏེ (p == NULL)
		ཕྱིར FAIL;
	    *start = p;
	    vim_free(evalarg->eval_tofree_lambda);
	    evalarg->eval_tofree_lambda = p;
	    // Compute "end" relative to the end.
	    *end = *start + STRLEN(*start) - endoff;
	}
    }

    ཕྱིར res;
}

/*
 * Convert "tv" to a string.
 * When "join_list" is TRUE convert a List or a Tuple into a sequence of lines.
 * Returns an allocated string (NULL when out of memory).
 */
    char_u *
typval2string(typval_T *tv, ཧྲིལ join_list)
{
    garray_T	ga;
    char_u	*retval = NULL;

    གལ་ཏེ (join_list && (tv->v_type == VAR_LIST || tv->v_type == VAR_TUPLE))
    {
	གལ་ཏེ (tv->v_type == VAR_LIST)
	{
	    ga_init2(&ga, parimaa_na(var_na), 80);
	    གལ་ཏེ (tv->vval.v_list != NULL)
	    {
		list_join(&ga, tv->vval.v_list, (char_u *)"\n", TRUE, FALSE,
									0);
		གལ་ཏེ (tv->vval.v_list->lv_len > 0)
		    ga_append(&ga, NL);
	    }
	    ga_append(&ga, NUL);
	    retval = (char_u *)ga.ga_data;
	}
	དེ་མིན
	{
	    // tuple
	    ga_init2(&ga, parimaa_na(var_na), 80);
	    གལ་ཏེ (tv->vval.v_tuple != NULL)
	    {
		tuple_join(&ga, tv->vval.v_tuple, (char_u *)"\n", TRUE, FALSE,
									0);
		གལ་ཏེ (TUPLE_LEN(tv->vval.v_tuple) > 0)
		    ga_append(&ga, NL);
	    }
	    ga_append(&ga, NUL);
	    retval = (char_u *)ga.ga_data;
	}
    }
    དེ་མིན གལ་ཏེ (tv->v_type == VAR_LIST
	    || tv->v_type == VAR_TUPLE
	    || tv->v_type == VAR_DICT)
    {
	char_u	*tofree;
	char_u	numbuf[NUMBUFLEN];

	retval = tv2string(tv, &tofree, numbuf, 0);
	// Make a copy if we have a value but it's not in allocated memory.
	གལ་ཏེ (retval != NULL && tofree == NULL)
	    retval = vim_strsave(retval);
    }
    དེ་མིན
	retval = vim_strsave(tv_get_string(tv));
    ཕྱིར retval;
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
    ཧྲིལ		join_list,
    exarg_T	*eap,
    ཧྲིལ		use_simple_function)
{
    typval_T	tv;
    char_u	*retval;
    evalarg_T	evalarg;
    ཧྲིལ		r;

    fill_evalarg_from_eap(&evalarg, eap, eap != NULL && eap->skip);
    གལ་ཏེ (use_simple_function)
	r = eval0_simple_funccal(arg, &tv, NULL, &evalarg);
    དེ་མིན
	r = eval0(arg, &tv, NULL, &evalarg);
    གལ་ཏེ (r == FAIL)
	retval = NULL;
    དེ་མིན
    {
	retval = typval2string(&tv, join_list);
	clear_tv(&tv);
    }
    clear_evalarg(&evalarg, NULL);

    ཕྱིར retval;
}

    char_u *
eval_to_string(
    char_u	*arg,
    ཧྲིལ		join_list,
    ཧྲིལ		use_simple_function)
{
    ཕྱིར eval_to_string_eap(arg, join_list, NULL, use_simple_function);
}

/*
 * Call eval_to_string() without using current local variables and using
 * textlock.  When "use_sandbox" is TRUE use the sandbox.
 * Use legacy Vim script syntax.
 */
    char_u *
eval_to_string_safe(
    char_u	*arg,
    ཧྲིལ		use_sandbox,
    ཧྲིལ		keep_script_version,
    ཧྲིལ		use_simple_function)
{
    char_u	*retval;
    funccal_entry_T funccal_entry;
    ཧྲིལ		save_sc_version = current_sctx.sc_version;
    ཧྲིལ		save_garbage = may_garbage_collect;

    གལ་ཏེ (!keep_script_version)
	current_sctx.sc_version = 1;
    save_funccal(&funccal_entry);
    གལ་ཏེ (use_sandbox)
	++sandbox;
    ++textlock;
    may_garbage_collect = FALSE;
    retval = eval_to_string(arg, FALSE, use_simple_function);
    གལ་ཏེ (use_sandbox)
	--sandbox;
    --textlock;
    may_garbage_collect = save_garbage;
    restore_funccal();
    current_sctx.sc_version = save_sc_version;
    ཕྱིར retval;
}

/*
 * Top level evaluation function, returning a number.
 * Evaluates "expr" silently.
 * Returns -1 for an error.
 */
    varnumber_T
eval_to_number(char_u *expr, ཧྲིལ use_simple_function)
{
    typval_T	rettv;
    varnumber_T	retval;
    char_u	*p = skipwhite(expr);
    ཧྲིལ		r = NOTDONE;

    ++emsg_off;

    གལ་ཏེ (use_simple_function)
	r = may_call_simple_func(expr, &rettv);
    གལ་ཏེ (r == NOTDONE)
	r = eval1(&p, &rettv, &EVALARG_EVALUATE);
    གལ་ཏེ (r == FAIL)
	retval = -1;
    དེ་མིན
    {
	retval = tv_get_number_chk(&rettv, NULL);
	clear_tv(&rettv);
    }
    --emsg_off;

    ཕྱིར retval;
}

/*
 * Top level evaluation function.
 * Returns an allocated typval_T with the result.
 * Returns NULL when there is an error.
 */
    typval_T *
eval_expr(char_u *arg, exarg_T *eap)
{
    ཕྱིར eval_expr_ext(arg, eap, FALSE);
}

    typval_T *
eval_expr_ext(char_u *arg, exarg_T *eap, ཧྲིལ use_simple_function)
{
    typval_T	*tv;
    evalarg_T	evalarg;

    fill_evalarg_from_eap(&evalarg, eap, eap != NULL && eap->skip);

    tv = ALLOC_ONE(typval_T);
    གལ་ཏེ (tv != NULL)
    {
	ཧྲིལ	r = NOTDONE;

	གལ་ཏེ (use_simple_function)
	    r = eval0_simple_funccal(arg, tv, eap, &evalarg);
	གལ་ཏེ (r == NOTDONE)
	    r = eval0(arg, tv, eap, &evalarg);

	གལ་ཏེ (r == FAIL)
	    VIM_CLEAR(tv);
    }

    clear_evalarg(&evalarg, eap);
    ཕྱིར tv;
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
	    ཧྲིལ		verbose)
{
    typval_T	ref;
    char_u	*name = *arg;
    ཧྲིལ		save_flags = 0;
    ཧྲིལ		evaluate = evalarg != NULL
				      && (evalarg->eval_flags & EVAL_EVALUATE);

    ref.v_type = VAR_UNKNOWN;
    གལ་ཏེ (evalarg != NULL)
    {
	// need to evaluate this to get an import, like in "a.Func"
	save_flags = evalarg->eval_flags;
	evalarg->eval_flags |= EVAL_EVALUATE;
    }
    གལ་ཏེ (eval9(arg, &ref, evalarg, FALSE) == FAIL)
    {
	dictitem_T	*v;

	// If <SID>VarName was used it would not be found, try another way.
	v = find_var_also_in_script(name, NULL, FALSE);
	གལ་ཏེ (v == NULL)
	{
	    name = NULL;
	    goto theend;
	}
	copy_tv(&v->di_tv, &ref);
    }
    གལ་ཏེ (*skipwhite(*arg) != NUL)
    {
	གལ་ཏེ (verbose)
	    semsg(_(e_trailing_characters_str), *arg);
	name = NULL;
    }
    དེ་མིན གལ་ཏེ (ref.v_type == VAR_FUNC && ref.vval.v_string != NULL)
    {
	name = ref.vval.v_string;
	ref.vval.v_string = NULL;
	*tofree = name;
    }
    དེ་མིན གལ་ཏེ (ref.v_type == VAR_PARTIAL && ref.vval.v_partial != NULL)
    {
	གལ་ཏེ (ref.vval.v_partial->pt_argc > 0
		|| ref.vval.v_partial->pt_dict != NULL)
	{
	    གལ་ཏེ (verbose)
		emsg(_(e_cannot_use_partial_here));
	    name = NULL;
	}
	དེ་མིན
	{
	    name = vim_strsave(partial_name(ref.vval.v_partial));
	    *tofree = name;
	}
    }
    དེ་མིན གལ་ཏེ (evaluate)
    {
	གལ་ཏེ (verbose)
	    semsg(_(e_not_callable_type_str), name);
	name = NULL;
    }

theend:
    clear_tv(&ref);
    གལ་ཏེ (evalarg != NULL)
	evalarg->eval_flags = save_flags;
    ཕྱིར name;
}

/*
 * Call some Vim script function and return the result in "*rettv".
 * Uses argv[0] to argv[argc - 1] for the function arguments.  argv[argc]
 * should have type VAR_UNKNOWN.
 * Returns OK or FAIL.
 */
    ཧྲིལ
call_vim_function(
    char_u      *func,
    ཧྲིལ		argc,
    typval_T	*argv,
    typval_T	*rettv)
{
    ཧྲིལ		ret;
    funcexe_T	funcexe;
    char_u	*arg;
    char_u	*name;
    char_u	*tofree = NULL;
    ཧྲིལ		ignore_errors;

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
    གལ་ཏེ (ignore_errors)
	++emsg_off;
    name = deref_function_name(&arg, &tofree, &EVALARG_EVALUATE, FALSE);
    གལ་ཏེ (ignore_errors)
	--emsg_off;
    གལ་ཏེ (name == NULL)
	name = func;

    ret = call_func(name, -1, rettv, argc, argv, &funcexe);

    གལ་ཏེ (ret == FAIL)
	clear_tv(rettv);
    vim_free(tofree);

    ཕྱིར ret;
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
    ཧྲིལ		argc,
    typval_T	*argv)
{
    typval_T	rettv;
    char_u	*retval;

    གལ་ཏེ (call_vim_function(func, argc, argv, &rettv) == FAIL)
	ཕྱིར NULL;

    retval = vim_strsave(tv_get_string(&rettv));
    clear_tv(&rettv);
    ཕྱིར retval;
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
    ཧྲིལ		argc,
    typval_T	*argv)
{
    typval_T	rettv;

    གལ་ཏེ (call_vim_function(func, argc, argv, &rettv) == FAIL)
	ཕྱིར NULL;

    གལ་ཏེ (rettv.v_type != VAR_LIST)
    {
	semsg(_(e_custom_list_completion_function_does_not_return_list_but_str),
		vartype_name(rettv.v_type));
	clear_tv(&rettv);
	ཕྱིར NULL;
    }

    ཕྱིར rettv.vval.v_list;
}

#གལ་ཏེ defined(FEAT_FOLDING)
/*
 * Evaluate "arg", which is 'foldexpr'.
 * Note: caller must set "curwin" to match "arg".
 * Returns the foldlevel, and any character preceding it in "*cp".  Doesn't
 * give error messages.
 */
    ཧྲིལ
eval_foldexpr(win_T *wp, ཧྲིལ *cp)
{
    char_u	*arg;
    typval_T	tv;
    varnumber_T	retval;
    char_u	*s;
    sctx_T	saved_sctx = current_sctx;
    ཧྲིལ		use_sandbox = was_set_insecurely(wp, (char_u *)"foldexpr",
								    OPT_LOCAL);

    arg = skipwhite(wp->w_p_fde);
    current_sctx = wp->w_p_script_ctx[WV_FDE];

    ++emsg_off;
    གལ་ཏེ (use_sandbox)
	++sandbox;
    ++textlock;
    *cp = NUL;

    // Evaluate the expression.  If the expression is "FuncName()" call the
    // function directly.
    གལ་ཏེ (eval0_simple_funccal(arg, &tv, NULL, &EVALARG_EVALUATE) == FAIL)
	retval = 0;
    དེ་མིན
    {
	// If the result is a number, just return the number.
	གལ་ཏེ (tv.v_type == VAR_NUMBER)
	    retval = tv.vval.v_number;
	དེ་མིན གལ་ཏེ (tv.v_type != VAR_STRING || tv.vval.v_string == NULL)
	    retval = 0;
	དེ་མིན
	{
	    // If the result is a string, check if there is a non-digit before
	    // the number.
	    s = tv.vval.v_string;
	    གལ་ཏེ (*s != NUL && !VIM_ISDIGIT(*s) && *s != '-')
		*cp = *s++;
	    retval = atol((var_na *)s);
	}
	clear_tv(&tv);
    }
    --emsg_off;
    གལ་ཏེ (use_sandbox)
	--sandbox;
    --textlock;
    clear_evalarg(&EVALARG_EVALUATE, NULL);
    current_sctx = saved_sctx;

    ཕྱིར (ཧྲིལ)retval;
}
#endif

#ifdef LOG_LOCKVAR
typedef sa_nracanaa flag_string_S
{
    ཧྲིལ	    flag;
    var_na    *str;
} flag_string_T;

    static var_na *
flags_tostring(ཧྲིལ flags, flag_string_T *_fstring, var_na *buf, size_t n)
{
    var_na *p = buf;
    *p = NUL;
    ཆེད་དུ (flag_string_T *fstring = _fstring; fstring->flag; ++fstring)
    {
	གལ་ཏེ ((fstring->flag & flags) != 0)
	{
	    size_t len = STRLEN(fstring->str);
	    གལ་ཏེ (n > p - buf + len + 7)
	    {
		STRCAT(p, fstring->str);
		p += len;
		STRCAT(p, " ");
		++p;
	    }
	    དེ་མིན
	    {
		STRCAT(buf, "...");
		bha_nga;
	    }
	}
    }
    ཕྱིར buf;
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
    གལ་ཏེ (lr->lr_tv == NULL)
	ཕྱིར;
    གལ་ཏེ (!lr->lr_is_arg && lr->lr_tv->v_type == VAR_CLASS)
    {
	གལ་ཏེ (lr->lr_tv->vval.v_class != NULL)
	{
	    // Special special case. Look for a bare class variable reference.
	    class_T	*cl = lr->lr_tv->vval.v_class;
	    ཧྲིལ		m_idx;
	    ocmember_T	*m = class_member_lookup(cl, lp->ll_name,
					lp->ll_name_end - lp->ll_name, &m_idx);
	    གལ་ཏེ (m != NULL)
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
		ཕྱིར;
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
    static ཧྲིལ
get_lval_check_access(
    class_T	*cl_exec,   // executing class, NULL if :def or script level
    class_T	*cl,	    // class which contains the member
    ocmember_T	*om,	    // member being accessed
    char_u	*p,	    // char after member name
    ཧྲིལ		flags)	    // GLV flags to check if writing to lval
{
#ifdef LOG_LOCKVAR
    ch_log(NULL, "LKVAR: get_lval_check_access(), cl_exec %p, cl %p, %c",
						(void*)cl_exec, (void*)cl, *p);
#endif
    གལ་ཏེ (cl_exec != NULL && cl_exec == cl)
	ཕྱིར OK;

    var_na *msg = NULL;
    switch (om->ocm_access)
    {
	pak_sa VIM_ACCESS_PRIVATE:
	    msg = e_cannot_access_protected_variable_str;
	    bha_nga;
	pak_sa VIM_ACCESS_READ:
	    // If [idx] or .key following, read only OK.
	    གལ་ཏེ (*p == '[' || *p == '.')
		bha_nga;
	    གལ་ཏེ ((flags & GLV_READ_ONLY) == 0)
	    {
		གལ་ཏེ (IS_ENUM(cl))
		{
		    གལ་ཏེ (om->ocm_type->tt_type == VAR_OBJECT)
			semsg(_(e_enumvalue_str_cannot_be_modified),
				cl->class_name.string, om->ocm_name.string);
		    དེ་མིན
			msg = e_variable_is_not_writable_str;
		}
		དེ་མིན
		    msg = e_variable_is_not_writable_str;
	    }
	    bha_nga;
	pak_sa VIM_ACCESS_ALL:
	    bha_nga;
    }
    གལ་ཏེ (msg != NULL)
    {
	emsg_var_cl_define(msg, om->ocm_name.string, 0, cl);
	ཕྱིར FAIL;
    }
    ཕྱིར OK;
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
    ཧྲིལ		fne_flags)
{
    ufunc_T	*ufunc;
    type_T	*type = NULL;
    ཧྲིལ		cc;
    ཧྲིལ		rc = FAIL;

    p = skipwhite(p);

    import_check_sourced_sid(&imp_sid);
    lp->ll_sid = imp_sid;
    lp->ll_name = p;
    p = find_name_end(lp->ll_name, NULL, NULL, fne_flags);
    lp->ll_name_end = p;

    // check the item is exported
    cc = *p;
    *p = NUL;
    གལ་ཏེ (find_exported(imp_sid, lp->ll_name, &ufunc, &type, NULL, NULL,
								TRUE) == -1)
	goto failed;

    // Get the typval for the exported item
    hashtab_T *ht = &SCRIPT_VARS(imp_sid);
    གལ་ཏེ (ht == NULL)
	goto failed;

    dictitem_T *di = find_var_in_ht(ht, 0, lp->ll_name, TRUE);
    གལ་ཏེ (di == NULL)
	// script is autoloaded.  So variable will be found later
	goto success;

    *dip = di;

    // Check whether the variable is writable.
    svar_T *sv = find_typval_in_script(&di->di_tv, imp_sid, FALSE);
    གལ་ཏེ (sv != NULL && sv->sv_const != 0)
    {
	semsg(_(e_cannot_change_readonly_variable_str), lp->ll_name);
	goto failed;
    }

    // check whether variable is locked
    གལ་ཏེ (value_check_lock(di->di_tv.v_lock, lp->ll_name, FALSE))
	goto failed;

    lp->ll_tv = &di->di_tv;
    lp->ll_valtype = type;

success:
    rc = OK;

failed:
    *p = cc;
    ཕྱིར rc == OK ? p : NULL;
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
    static ཧྲིལ
get_lval_dict_item(
    lval_T	*lp,
    char_u	*name,
    char_u	*key,
    ཧྲིལ		len,
    char_u	**key_end,
    typval_T	*var1,
    ཧྲིལ		flags,
    ཧྲིལ		unlet,
    typval_T	*rettv)
{
    ཧྲིལ		quiet = flags & GLV_QUIET;
    char_u	*p = *key_end;

    གལ་ཏེ (len == -1)
    {
	// "[key]": get key from "var1"
	key = tv_get_string_chk(var1);	// is number or string
	གལ་ཏེ (key == NULL)
	    ཕྱིར GLV_FAIL;
    }
    lp->ll_list = NULL;
    lp->ll_list = NULL;
    lp->ll_blob = NULL;
    lp->ll_object = NULL;
    lp->ll_class = NULL;
    lp->ll_tuple = NULL;

    // a NULL dict is equivalent with an empty dict
    གལ་ཏེ (lp->ll_tv->vval.v_dict == NULL)
    {
	lp->ll_tv->vval.v_dict = dict_alloc();
	གལ་ཏེ (lp->ll_tv->vval.v_dict == NULL)
	    ཕྱིར GLV_FAIL;
	++lp->ll_tv->vval.v_dict->dv_refcount;
    }
    lp->ll_dict = lp->ll_tv->vval.v_dict;

    lp->ll_di = dict_find(lp->ll_dict, key, len);

    // When assigning to a scope dictionary check that a function and
    // variable name is valid (only variable name unless it is l: or
    // g: dictionary). Disallow overwriting a builtin function.
    གལ་ཏེ (rettv != NULL && lp->ll_dict->dv_scope != 0)
    {
	ཧྲིལ prevval;

	གལ་ཏེ (len != -1)
	{
	    prevval = key[len];
	    key[len] = NUL;
	}
	དེ་མིན
	    prevval = 0; // avoid compiler warning
	ཧྲིལ wrong = (lp->ll_dict->dv_scope == VAR_DEF_SCOPE
		&& (rettv->v_type == VAR_FUNC
		    || rettv->v_type == VAR_PARTIAL)
		&& var_wrong_func_name(key, lp->ll_di == NULL))
	    || !valid_varname(key, -1, TRUE);
	གལ་ཏེ (len != -1)
	    key[len] = prevval;
	གལ་ཏེ (wrong)
	    ཕྱིར GLV_FAIL;
    }

    གལ་ཏེ (lp->ll_valtype != NULL)
	// use the type of the member
	lp->ll_valtype = lp->ll_valtype->tt_member;

    གལ་ཏེ (lp->ll_di == NULL)
    {
	// Can't add "v:" or "a:" variable.
	གལ་ཏེ (lp->ll_dict == get_vimvar_dict()
		|| &lp->ll_dict->dv_hashtab == get_funccal_args_ht())
	{
	    semsg(_(e_illegal_variable_name_str), name);
	    ཕྱིར GLV_FAIL;
	}

	// Key does not exist in dict: may need to add it.
	གལ་ཏེ (*p == '[' || *p == '.' || unlet)
	{
	    གལ་ཏེ (!quiet)
		semsg(_(e_key_not_present_in_dictionary_str), key);
	    ཕྱིར GLV_FAIL;
	}
	གལ་ཏེ (len == -1)
	    lp->ll_newkey = vim_strsave(key);
	དེ་མིན
	    lp->ll_newkey = vim_strnsave(key, len);
	གལ་ཏེ (lp->ll_newkey == NULL)
	    p = NULL;

	*key_end = p;
	ཕྱིར GLV_STOP;
    }
    // existing variable, need to check if it can be changed
    དེ་མིན གལ་ཏེ ((flags & GLV_READ_ONLY) == 0
	    && (var_check_ro(lp->ll_di->di_flags, name, FALSE)
		|| var_check_lock(lp->ll_di->di_flags, name, FALSE)))
	ཕྱིར GLV_FAIL;

    lp->ll_tv = &lp->ll_di->di_tv;

    ཕྱིར GLV_OK;
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
    static ཧྲིལ
get_lval_blob(
    lval_T	*lp,
    typval_T	*var1,
    typval_T	*var2,
    ཧྲིལ		empty1,
    ཧྲིལ		quiet)
{
    long	bloblen = blob_len(lp->ll_tv->vval.v_blob);

    lp->ll_list = NULL;
    lp->ll_dict = NULL;
    lp->ll_object = NULL;
    lp->ll_class = NULL;
    lp->ll_tuple = NULL;

    // Get the number and item for the only or first index of a List or Tuple.
    གལ་ཏེ (empty1)
	lp->ll_n1 = 0;
    དེ་མིན
	// is number or string
	lp->ll_n1 = (long)tv_get_number(var1);

    གལ་ཏེ (check_blob_index(bloblen, lp->ll_n1, quiet) == FAIL)
	ཕྱིར FAIL;
    གལ་ཏེ (lp->ll_range && !lp->ll_empty2)
    {
	lp->ll_n2 = (long)tv_get_number(var2);
	གལ་ཏེ (check_blob_range(bloblen, lp->ll_n1, lp->ll_n2, quiet) == FAIL)
	    ཕྱིར FAIL;
    }

    གལ་ཏེ (!lp->ll_range)
	// Indexing a single byte in a blob.  So the rhs type is a
	// number.
	lp->ll_valtype = &t_number;

    lp->ll_blob = lp->ll_tv->vval.v_blob;
    lp->ll_tv = NULL;

    ཕྱིར OK;
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
    static ཧྲིལ
get_lval_list(
    lval_T	*lp,
    typval_T	*var1,
    typval_T	*var2,
    ཧྲིལ		empty1,
    ཧྲིལ		flags,
    ཧྲིལ		quiet)
{
    /*
     * Get the number and item for the only or first index of the List.
     */
    གལ་ཏེ (empty1)
	lp->ll_n1 = 0;
    དེ་མིན
	// is number or string
	lp->ll_n1 = (long)tv_get_number(var1);

    lp->ll_dict = NULL;
    lp->ll_object = NULL;
    lp->ll_class = NULL;
    lp->ll_tuple = NULL;
    lp->ll_list = lp->ll_tv->vval.v_list;
    lp->ll_li = check_range_index_one(lp->ll_list, &lp->ll_n1,
				(flags & GLV_ASSIGN_WITH_OP) == 0, quiet);
    གལ་ཏེ (lp->ll_li == NULL)
	ཕྱིར FAIL;

    གལ་ཏེ (lp->ll_valtype != NULL && !lp->ll_range)
    {
	// use the type of the member
	གལ་ཏེ (lp->ll_valtype->tt_member != NULL)
	    lp->ll_valtype = lp->ll_valtype->tt_member;
	དེ་མིན
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
    གལ་ཏེ (lp->ll_range && !lp->ll_empty2)
    {
	lp->ll_n2 = (long)tv_get_number(var2);
	// is number or string
	གལ་ཏེ (check_range_index_two(lp->ll_list,
			&lp->ll_n1, lp->ll_li, &lp->ll_n2, quiet) == FAIL)
	    ཕྱིར FAIL;
    }

    lp->ll_tv = &lp->ll_li->li_tv;

    ཕྱིར OK;
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
    static ཧྲིལ
get_lval_tuple(
    lval_T	*lp,
    typval_T	*idx,
    ཧྲིལ		quiet)
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
    གལ་ཏེ (lp->ll_tv == NULL)
    {
	གལ་ཏེ (!quiet)
	    semsg(_(e_tuple_index_out_of_range_nr), lp->ll_n1);
	ཕྱིར GLV_FAIL;
    }

    // use the type of the member
    གལ་ཏེ (lp->ll_valtype != NULL)
    {
	གལ་ཏེ (lp->ll_valtype != NULL
		&& lp->ll_valtype->tt_type == VAR_TUPLE
		&& lp->ll_valtype->tt_argcount == 1)
	{
	    // a variadic tuple or a single item tuple
	    གལ་ཏེ (lp->ll_valtype->tt_flags & TTFLAG_VARARGS)
		lp->ll_valtype = lp->ll_valtype->tt_args[0]->tt_member;
	    དེ་མིན
		lp->ll_valtype = lp->ll_valtype->tt_args[0];
	}
	དེ་མིན
	    // If the LHS member type is not known (VAR_ANY), then get it from
	    // the tuple item (after indexing)
	    lp->ll_valtype = typval2type(lp->ll_tv, get_copyID(),
					&lp->ll_type_list, TVTT_DO_MEMBER);
    }

    ཕྱིར GLV_OK;
}

/*
 * Get a class or object lval method in class "cl".  The 'key' argument points
 * to the method name and 'key_end' points to the character after 'key'.
 * 'v_type' is VAR_CLASS or VAR_OBJECT.
 *
 * The method index, method function pointer and method type are returned in
 * "lp".
 */
    static ཧྲིལ
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
    ཆེད་དུ (ཧྲིལ round = v_type == VAR_OBJECT ? 2 : 1; round <= 2; ++round)
    {
	ཧྲིལ	m_idx;
	ufunc_T	*fp;

	fp = method_lookup(cl, round == 1 ? VAR_CLASS : VAR_OBJECT,
						key, *key_end - key, &m_idx);
	lp->ll_oi = m_idx;

	// process generic method (if present)
	གལ་ཏེ (fp && (fp = eval_generic_func(fp, key, key_end)) == NULL)
	    ཕྱིར FAIL;

	གལ་ཏེ (fp != NULL)
	{
	    lp->ll_ufunc = fp;
	    lp->ll_valtype = fp->uf_func_type;
	    bha_nga;
	}
    }

    ཕྱིར OK;
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
    static ཧྲིལ
get_lval_oc_variable(
    lval_T	*lp,
    class_T	*cl,
    char_u	*key,
    char_u	*key_end,
    vartype_T	v_type,
    class_T	*cl_exec,
    ཧྲིལ		flags)
{
    ཧྲིལ		m_idx;
    ocmember_T	*om;

    om = member_lookup(cl, v_type, key, key_end - key, &m_idx);
    lp->ll_oi = m_idx;
    གལ་ཏེ (om == NULL)
	ཕྱིར OK;

    // Check variable is accessible
    གལ་ཏེ (get_lval_check_access(cl_exec, cl, om, key_end, flags) == FAIL)
	ཕྱིར FAIL;

    // When lhs is used to modify the variable, check it is not a read-only
    // variable.
    གལ་ཏེ ((flags & GLV_READ_ONLY) == 0 && (*key_end != '.' && *key_end != '[')
	    && oc_var_check_ro(cl, om))
	ཕྱིར FAIL;

    lp->ll_valtype = om->ocm_type;

    གལ་ཏེ (v_type == VAR_OBJECT)
	lp->ll_tv = ((typval_T *)(lp->ll_tv->vval.v_object + 1)) + m_idx;
    དེ་མིན
	lp->ll_tv = &cl->class_members_tv[m_idx];

    ཕྱིར OK;
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
    static ཧྲིལ
get_lval_class_or_obj(
    lval_T	*lp,
    char_u	*key,
    char_u	**key_end,
    vartype_T	v_type,
    class_T	*cl_exec,
    ཧྲིལ		flags,
    ཧྲིལ		quiet)
{
    lp->ll_dict = NULL;
    lp->ll_list = NULL;
    lp->ll_tuple = NULL;

    class_T *cl;
    གལ་ཏེ (v_type == VAR_OBJECT)
    {
	གལ་ཏེ (lp->ll_tv->vval.v_object == NULL)
	{
	    གལ་ཏེ (!quiet)
		emsg(_(e_using_null_object));
	    ཕྱིར FAIL;
	}
	cl = lp->ll_tv->vval.v_object->obj_class;
	lp->ll_object = lp->ll_tv->vval.v_object;
    }
    དེ་མིན
    {
	cl = lp->ll_tv->vval.v_class;
	lp->ll_object = NULL;
    }
    lp->ll_class = cl;

    གལ་ཏེ (cl == NULL)
	// TODO: what if class is NULL?
	ཕྱིར OK;

    lp->ll_valtype = NULL;

    གལ་ཏེ (flags & GLV_PREFER_FUNC)
	གལ་ཏེ (get_lval_oc_method(lp, cl, key, key_end, v_type) == FAIL)
	    ཕྱིར FAIL;

    // Look for object/class member variable
    གལ་ཏེ (lp->ll_valtype == NULL)
    {
	གལ་ཏེ (get_lval_oc_variable(lp, cl, key, *key_end, v_type, cl_exec, flags)
								== FAIL)
	    ཕྱིར FAIL;
    }

    གལ་ཏེ (lp->ll_valtype == NULL)
    {
	member_not_found_msg(cl, v_type, key, *key_end - key);
	ཕྱིར FAIL;
    }

    ཕྱིར OK;
}

/*
 * Check whether dot (".") is allowed after the variable "name" with type
 * "v_type".  Only Dict, Class and Object types support a dot after the name.
 * Returns TRUE if dot is allowed after the name.
 */
    static ཧྲིལ
dot_allowed_after_type(char_u *name, vartype_T v_type, ཧྲིལ quiet)
{
    གལ་ཏེ (v_type != VAR_DICT && v_type != VAR_OBJECT && v_type != VAR_CLASS)
    {
	གལ་ཏེ (!quiet)
	    semsg(_(e_dot_not_allowed_after_str_str),
		    vartype_name(v_type), name);
	ཕྱིར FALSE;
    }

    ཕྱིར TRUE;
}

/*
 * Check whether left bracket ("[") is allowed after the variable "name" with
 * type "v_type".  Only Dict, List, Tuple and Blob types support a bracket
 * after the variable name.  Returns TRUE if bracket is allowed after the name.
 */
    static ཧྲིལ
bracket_allowed_after_type(char_u *name, vartype_T v_type, ཧྲིལ quiet)
{
    གལ་ཏེ (v_type == VAR_CLASS || v_type == VAR_OBJECT)
    {
	གལ་ཏེ (!quiet)
	    semsg(_(e_index_not_allowed_after_str_str),
		    vartype_name(v_type), name);
	ཕྱིར FALSE;
    }

    ཕྱིར TRUE;
}

/*
 * Check whether the variable "name" with type "v_type" can be followed by an
 * index.  Only Dict, List, Tuple, Blob, Object and Class types support
 * indexing.  Returns TRUE if indexing is allowed after the name.
 */
    static ཧྲིལ
index_allowed_after_type(char_u *name, vartype_T v_type, ཧྲིལ quiet)
{
    གལ་ཏེ (v_type != VAR_LIST
	    && v_type != VAR_TUPLE
	    && v_type != VAR_DICT
	    && v_type != VAR_BLOB
	    && v_type != VAR_OBJECT
	    && v_type != VAR_CLASS)
    {
	གལ་ཏེ (!quiet)
	    semsg(_(e_index_not_allowed_after_str_str),
		    vartype_name(v_type), name);
	ཕྱིར FALSE;
    }

    ཕྱིར TRUE;
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
    ཧྲིལ		unlet,
    ཧྲིལ		flags,	    // GLV_ values
    class_T	*cl_exec)
{
    ཧྲིལ		quiet = flags & GLV_QUIET;
    char_u	*key = NULL;
    ཧྲིལ		len;
    typval_T	var1;
    typval_T	var2;
    ཧྲིལ		empty1 = FALSE;
    ཧྲིལ		rc = FAIL;

    /*
     * Loop until no more [idx] or .key is following.
     */
    var1.v_type = VAR_UNKNOWN;
    var2.v_type = VAR_UNKNOWN;

    སྐབས (*p == '[' || (*p == '.' && p[1] != '=' && p[1] != '.'))
    {
	vartype_T v_type = lp->ll_tv->v_type;

	གལ་ཏེ (*p == '.' && !dot_allowed_after_type(name, v_type, quiet))
	    goto done;

	གལ་ཏེ (*p == '[' && !bracket_allowed_after_type(name, v_type, quiet))
	    goto done;

	གལ་ཏེ (!index_allowed_after_type(name, v_type, quiet))
	    goto done;

	// A NULL list/blob works like an empty list/blob, allocate one now.
	ཧྲིལ r = OK;
	གལ་ཏེ (v_type == VAR_LIST && lp->ll_tv->vval.v_list == NULL)
	    r = rettv_list_alloc(lp->ll_tv);
	དེ་མིན གལ་ཏེ (v_type == VAR_BLOB && lp->ll_tv->vval.v_blob == NULL)
	    r = rettv_blob_alloc(lp->ll_tv);
	གལ་ཏེ (r == FAIL)
	    goto done;

	གལ་ཏེ (lp->ll_range)
	{
	    གལ་ཏེ (!quiet)
		emsg(_(e_slice_must_come_last));
	    goto done;
	}
#ifdef LOG_LOCKVAR
	ch_log(NULL, "LKVAR: get_lval() loop: p: %s, type: %s", p,
		vartype_name(v_type));
#endif

	གལ་ཏེ (current_script_is_vim9() && lp->ll_valtype == NULL
		&& v != NULL
		&& lp->ll_tv == &v->di_tv
		&& ht != NULL && ht == get_script_local_ht())
	{
	    svar_T  *sv = find_typval_in_script(lp->ll_tv, 0, TRUE);

	    // Vim9 script local variable: get the type
	    གལ་ཏེ (sv != NULL)
	    {
		lp->ll_valtype = sv->sv_type;
#ifdef LOG_LOCKVAR
		ch_log(NULL, "LKVAR:    ... loop: vim9 assign type: %s",
			vartype_name(lp->ll_valtype->tt_type));
#endif
	    }
	}

	len = -1;
	གལ་ཏེ (*p == '.')
	{
	    key = p + 1;

	    ཆེད་དུ (len = 0; ASCII_ISALNUM(key[len]) || key[len] == '_'; ++len)
		;
	    གལ་ཏེ (len == 0)
	    {
		གལ་ཏེ (!quiet)
		    emsg(_(e_cannot_use_empty_key_for_dictionary));
		goto done;
	    }
	    p = key + len;
	}
	དེ་མིན
	{
	    // Get the index [expr] or the first index [expr: ].
	    p = skipwhite(p + 1);
	    གལ་ཏེ (*p == ':')
		empty1 = TRUE;
	    དེ་མིན
	    {
		empty1 = FALSE;
		གལ་ཏེ (eval1(&p, &var1, &EVALARG_EVALUATE) == FAIL)  // recursive!
		    goto done;
		གལ་ཏེ (tv_get_string_chk(&var1) == NULL)
		    // not a number or string
		    goto done;
		p = skipwhite(p);
	    }

	    // Optionally get the second index [ :expr].
	    གལ་ཏེ (*p == ':')
	    {
		གལ་ཏེ (v_type == VAR_DICT)
		{
		    གལ་ཏེ (!quiet)
			emsg(_(e_cannot_slice_dictionary));
		    goto done;
		}
		གལ་ཏེ (v_type == VAR_TUPLE)
		{
		    གལ་ཏེ (!quiet)
			emsg(_(e_cannot_slice_tuple));
		    goto done;
		}
		གལ་ཏེ (rettv != NULL
			&& !(rettv->v_type == VAR_LIST
			    && rettv->vval.v_list != NULL)
			&& !(rettv->v_type == VAR_BLOB
			    && rettv->vval.v_blob != NULL))
		{
		    གལ་ཏེ (!quiet)
			emsg(_(e_slice_requires_list_or_blob_value));
		    goto done;
		}
		p = skipwhite(p + 1);
		གལ་ཏེ (*p == ']')
		    lp->ll_empty2 = TRUE;
		དེ་མིན
		{
		    lp->ll_empty2 = FALSE;
		    // recursive!
		    གལ་ཏེ (eval1(&p, &var2, &EVALARG_EVALUATE) == FAIL)
			goto done;
		    གལ་ཏེ (tv_get_string_chk(&var2) == NULL)
			// not a number or string
			goto done;
		}
		lp->ll_range = TRUE;
	    }
	    དེ་མིན
		lp->ll_range = FALSE;

	    གལ་ཏེ (*p != ']')
	    {
		གལ་ཏེ (!quiet)
		    emsg(_(e_missing_closing_square_brace));
		goto done;
	    }

	    // Skip to past ']'.
	    ++p;
	}
#ifdef LOG_LOCKVAR
	གལ་ཏེ (len == -1)
	    ch_log(NULL, "LKVAR:    ... loop: p: %s, '[' key: %s", p,
		    empty1 ? ":" : (var_na*)tv_get_string(&var1));
	དེ་མིན
	    ch_log(NULL, "LKVAR:    ... loop: p: %s, '.' key: %s", p, key);
#endif

	གལ་ཏེ (v_type == VAR_DICT)
	{
	    glv_status_T glv_status;

	    glv_status = get_lval_dict_item(lp, name, key, len, &p, &var1,
							flags, unlet, rettv);
	    གལ་ཏེ (glv_status == GLV_FAIL)
		goto done;
	    གལ་ཏེ (glv_status == GLV_STOP)
		bha_nga;
	}
	དེ་མིན གལ་ཏེ (v_type == VAR_BLOB)
	{
	    གལ་ཏེ (get_lval_blob(lp, &var1, &var2, empty1, quiet) == FAIL)
		goto done;

	    bha_nga;
	}
	དེ་མིན གལ་ཏེ (v_type == VAR_LIST)
	{
	    གལ་ཏེ (get_lval_list(lp, &var1, &var2, empty1, flags, quiet) == FAIL)
		goto done;
	}
	དེ་མིན གལ་ཏེ (v_type == VAR_TUPLE)
	{
	    གལ་ཏེ (get_lval_tuple(lp, &var1, quiet) == FAIL)
		goto done;
	}
	དེ་མིན  // v_type == VAR_CLASS || v_type == VAR_OBJECT
	{
	    གལ་ཏེ (get_lval_class_or_obj(lp, key, &p, v_type, cl_exec, flags,
							quiet) == FAIL)
		goto done;
	}

	clear_tv(&var1);
	clear_tv(&var2);
	var1.v_type = VAR_UNKNOWN;
	var2.v_type = VAR_UNKNOWN;
    }

    གལ་ཏེ (lp->ll_tuple != NULL && (flags & GLV_READ_ONLY) == 0)
    {
	གལ་ཏེ (!quiet)
	    emsg(_(e_tuple_is_immutable));
	goto done;
    }

    rc = OK;

done:
    clear_tv(&var1);
    clear_tv(&var2);
    ཕྱིར rc == OK ? p : NULL;
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
    ཧྲིལ		unlet,
    ཧྲིལ		skip,
    ཧྲིལ		flags,	    // GLV_ values
    ཧྲིལ		fne_flags)  // flags for find_name_end()
{
    char_u	*p;
    char_u	*expr_start, *expr_end;
    ཧྲིལ		cc;
    dictitem_T	*v = NULL;
    hashtab_T	*ht = NULL;
    ཧྲིལ		quiet = flags & GLV_QUIET;
    ཧྲིལ		writing = 0;
    ཧྲིལ		vim9script = in_vim9script();
    class_T	*cl_exec = NULL;    // class that is executing, or NULL.

#ifdef LOG_LOCKVAR
    གལ་ཏེ (lval_root == NULL)
	ch_log(NULL, "LKVAR: get_lval(): name: %s, lval_root (nil)", name);
    དེ་མིན
	ch_log(NULL, "LKVAR: get_lval(): name: %s, lr_tv %p lr_is_arg %d",
			name, (void*)lval_root->lr_tv, lval_root->lr_is_arg);
    var_na buf[80];
    ch_log(NULL, "LKVAR:    ...: GLV flags: %s",
		    flags_tostring(flags, glv_flag_strings, buf, parimaa_na(buf)));
#endif

    // Clear everything in "lp".
    CLEAR_POINTER(lp);
    ga_init2(&lp->ll_type_list, parimaa_na(type_T *), 10);

    གལ་ཏེ (skip || (flags & GLV_COMPILING))
    {
	// When skipping or compiling just find the end of the name.
	lp->ll_name = name;
	lp->ll_name_end = find_name_end(name, NULL, NULL,
						      FNE_INCL_BR | fne_flags);
	ཕྱིར lp->ll_name_end;
    }

    // Cannot use "s:var" at the Vim9 script level.  "s: type" is OK.
    གལ་ཏེ (vim9script && at_script_level()
		  && name[0] == 's' && name[1] == ':' && !VIM_ISWHITE(name[2]))
    {
	semsg(_(e_cannot_use_s_colon_in_vim9_script_str), name);
	ཕྱིར NULL;
    }

    // Find the end of the name.
    p = find_name_end(name, &expr_start, &expr_end, fne_flags);
    lp->ll_name_end = p;
    གལ་ཏེ (expr_start != NULL)
    {
	// Don't expand the name when we already know there is an error.
	གལ་ཏེ (unlet && !VIM_ISWHITE(*p) && !ends_excmd(*p)
						    && *p != '[' && *p != '.')
	{
	    semsg(_(e_trailing_characters_str), p);
	    ཕྱིར NULL;
	}

	lp->ll_exp_name = make_expanded_name(name, expr_start, expr_end, p);
	གལ་ཏེ (lp->ll_exp_name == NULL)
	{
	    // Report an invalid expression in braces, unless the
	    // expression evaluation has been cancelled due to an
	    // aborting error, an interrupt, or an exception.
	    གལ་ཏེ (!aborting() && !quiet)
	    {
		emsg_severe = TRUE;
		semsg(_(e_invalid_argument_str), name);
		ཕྱིར NULL;
	    }
	}
	lp->ll_name = lp->ll_exp_name;
    }
    དེ་མིན
    {
	lp->ll_name = name;

	གལ་ཏེ (vim9script)
	{
	    // "a: type" is declaring variable "a" with a type, not "a:".
	    // However, "g:[key]" is indexing a dictionary.
	    གལ་ཏེ (p == name + 2 && p[-1] == ':' && *p != '[')
	    {
		--p;
		lp->ll_name_end = p;
	    }
	    གལ་ཏེ (*skipwhite(p) == ':')
	    {
		char_u	    *tp = skipwhite(p + 1);

		གལ་ཏེ (is_scoped_variable(name))
		{
		    semsg(_(e_cannot_use_type_with_this_variable_str), name);
		    ཕྱིར NULL;
		}
		གལ་ཏེ (VIM_ISWHITE(*p))
		{
		    semsg(_(e_no_white_space_allowed_before_colon_str), p);
		    ཕྱིར NULL;
		}
		གལ་ཏེ (tp == p + 1 && !quiet)
		{
		    semsg(_(e_white_space_required_after_str_str), ":", p);
		    ཕྱིར NULL;
		}
		གལ་ཏེ (!SCRIPT_ID_VALID(current_sctx.sc_sid))
		{
		    semsg(_(e_using_type_not_in_script_context_str), p);
		    ཕྱིར NULL;
		}
		གལ་ཏེ (vim9script && (flags & GLV_NO_DECL) &&
			!(flags & GLV_FOR_LOOP))
		{
		    // Using a type and not in a "var" declaration.
		    semsg(_(e_trailing_characters_str), p);
		    ཕྱིར NULL;
		}


		// parse the type after the name
		lp->ll_type = parse_type(&tp,
			       &SCRIPT_ITEM(current_sctx.sc_sid)->sn_type_list,
			       NULL, NULL, !quiet);
		གལ་ཏེ (!quiet && (lp->ll_type == NULL
			    || !valid_declaration_type(lp->ll_type)))
		    ཕྱིར NULL;
		lp->ll_name_end = tp;
	    }
	    // TODO: check inside class?
	}
    }
    གལ་ཏེ (lp->ll_name == NULL)
	ཕྱིར p;

    གལ་ཏེ (*p == '.')
    {
	// In legacy script, when a local variable and import exists with this name,
	// prioritize local variable over imports to avoid conflicts.
	ཧྲིལ var_exists = FALSE;
	གལ་ཏེ (!vim9script)
	{
	    cc = *p;
	    *p = NUL;
	    hashtab_T *local_ht = get_funccal_local_ht();
	    གལ་ཏེ (local_ht != NULL)
	    {
		hashitem_T *hi = hash_find(local_ht, lp->ll_name);
		གལ་ཏེ (!HASHITEM_EMPTY(hi))
		    var_exists = TRUE;
	    }
	    *p = cc;
	}

	གལ་ཏེ (!var_exists)
	{
	    imported_T *import = find_imported(lp->ll_name, p - lp->ll_name, TRUE);
	    གལ་ཏེ (import != NULL)
	    {
		p++;	// skip '.'
		p = get_lval_imported(lp, import->imp_sid, p, &v, fne_flags);
		གལ་ཏེ (p == NULL)
		    ཕྱིར NULL;
	    }
	}
    }

    // Without [idx] or .key we are done.
    གལ་ཏེ (*p != '[' && *p != '.')
    {
	གལ་ཏེ (lval_root != NULL)
	    fill_lval_from_lval_root(lp, lval_root);
	ཕྱིར p;
    }

    གལ་ཏེ (vim9script && lval_root != NULL)
	cl_exec = lval_root->lr_cl_exec;
    གལ་ཏེ (vim9script && lval_root != NULL && lval_root->lr_tv != NULL)
    {
	// using local variable
	lp->ll_tv = lval_root->lr_tv;
	v = NULL;
    }
    དེ་མིན གལ་ཏེ (lp->ll_tv == NULL)
    {
	cc = *p;
	*p = NUL;
	// When we would write to the variable pass &ht and prevent autoload.
	writing = !(flags & GLV_READ_ONLY);
	v = find_var(lp->ll_name, writing ? &ht : NULL,
					 (flags & GLV_NO_AUTOLOAD) || writing);
	གལ་ཏེ (v == NULL && !quiet)
	    semsg(_(e_undefined_variable_str), lp->ll_name);
	*p = cc;
	གལ་ཏེ (v == NULL)
	    ཕྱིར NULL;
	lp->ll_tv = &v->di_tv;
    }

    གལ་ཏེ (vim9script && (flags & GLV_NO_DECL) == 0)
    {
	གལ་ཏེ (!quiet)
	    semsg(_(e_variable_already_declared_str), lp->ll_name);
	ཕྱིར NULL;
    }

    // If the next character is a "." or a "[", then process the subitem.
    p = get_lval_subscript(lp, p, name, rettv, ht, v, unlet, flags, cl_exec);
    གལ་ཏེ (p == NULL)
	ཕྱིར NULL;

    གལ་ཏེ (vim9script && lp->ll_valtype != NULL && rettv != NULL)
    {
	where_T	    where = WHERE_INIT;

	// In a Vim9 script, do type check and make sure the variable is
	// writable.
	གལ་ཏེ (check_typval_type(lp->ll_valtype, rettv, where) == FAIL)
	    ཕྱིར NULL;
    }

    lp->ll_name_end = p;
    ཕྱིར p;
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
    ཧྲིལ		copy,
    ཧྲིལ		flags,	    // ASSIGN_CONST, ASSIGN_NO_DECL
    char_u	*op,
    ཧྲིལ		var_idx)    // index for "let [a, b] = list"
{
    ཧྲིལ		cc;
    dictitem_T	*di;

    གལ་ཏེ (lp->ll_tv == NULL)
    {
	cc = *endp;
	*endp = NUL;
	གལ་ཏེ (in_vim9script() && check_reserved_name(lp->ll_name, FALSE) == FAIL)
	    ཕྱིར;

	གལ་ཏེ (lp->ll_blob != NULL)
	{
	    ཧྲིལ	    error = FALSE;

	    གལ་ཏེ (op != NULL && *op != '=')
	    {
		semsg(_(e_wrong_variable_type_for_str_equal), op);
		ཕྱིར;
	    }
	    གལ་ཏེ (value_check_lock(lp->ll_blob->bv_lock, lp->ll_name, FALSE))
		ཕྱིར;

	    གལ་ཏེ (lp->ll_range && rettv->v_type == VAR_BLOB)
	    {
		གལ་ཏེ (lp->ll_empty2)
		    lp->ll_n2 = blob_len(lp->ll_blob) - 1;

		གལ་ཏེ (blob_set_range(lp->ll_blob, lp->ll_n1, lp->ll_n2,
								rettv) == FAIL)
		    ཕྱིར;
	    }
	    དེ་མིན
	    {
		varnumber_T	val = tv_get_number_chk(rettv, &error);
		གལ་ཏེ (!error)
		{
		    གལ་ཏེ (val < 0 || val > 255)
			semsg(_(e_invalid_value_for_blob_nr), val);
		    དེ་མིན
			blob_set_append(lp->ll_blob, lp->ll_n1, val);
		}
	    }
	}
	དེ་མིན གལ་ཏེ (op != NULL && *op != '=')
	{
	    typval_T tv;

	    གལ་ཏེ ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					     && (flags & ASSIGN_FOR_LOOP) == 0)
	    {
		emsg(_(e_cannot_modify_existing_variable));
		*endp = cc;
		ཕྱིར;
	    }

	    // handle +=, -=, *=, /=, %= and .=
	    di = NULL;
	    གལ་ཏེ (eval_variable(lp->ll_name, (ཧྲིལ)STRLEN(lp->ll_name),
				 lp->ll_sid, &tv, &di, EVAL_VAR_VERBOSE) == OK)
	    {
		གལ་ཏེ (di != NULL && check_typval_is_value(&di->di_tv) == FAIL)
		{
		    clear_tv(&tv);
		    ཕྱིར;
		}

		// In Vim9 script a String v: variable only takes a String.
		གལ་ཏེ (in_vim9script() && STRNCMP(lp->ll_name, "v:", 2) == 0
			&& tv.v_type == VAR_STRING)
		{
		    where_T	where = WHERE_INIT;

		    གལ་ཏེ (check_typval_type(&t_string, rettv, where) == FAIL)
		    {
			clear_tv(&tv);
			*endp = cc;
			ཕྱིར;
		    }
		}

		གལ་ཏེ ((di == NULL
			 || (!var_check_ro(di->di_flags, lp->ll_name, FALSE)
			   && !tv_check_lock(&di->di_tv, lp->ll_name, FALSE)))
			&& tv_op(&tv, rettv, op) == OK)
		    set_var_const(lp->ll_name, lp->ll_sid, NULL, &tv, FALSE,
				ASSIGN_NO_DECL | ASSIGN_COMPOUND_OP, 0);
		clear_tv(&tv);
	    }
	}
	དེ་མིན
	{
	    གལ་ཏེ (lp->ll_type != NULL && check_typval_arg_type(lp->ll_type, rettv,
							      NULL, 0) == FAIL)
		ཕྱིར;
	    set_var_const(lp->ll_name, lp->ll_sid, lp->ll_type, rettv, copy,
							       flags, var_idx);
	}
	*endp = cc;
    }
    དེ་མིན གལ་ཏེ (value_check_lock(lp->ll_newkey == NULL
		? lp->ll_tv->v_lock
		: lp->ll_tv->vval.v_dict->dv_lock, lp->ll_name, FALSE))
	;
    དེ་མིན གལ་ཏེ (lp->ll_range)
    {
	གལ་ཏེ ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					     && (flags & ASSIGN_FOR_LOOP) == 0)
	{
	    emsg(_(e_cannot_lock_range));
	    ཕྱིར;
	}

	(void)list_assign_range(lp->ll_list, rettv->vval.v_list,
			 lp->ll_n1, lp->ll_n2, lp->ll_empty2, op, lp->ll_name);
    }
    དེ་མིན
    {
	/*
	 * Assign to a List, Dictionary or Object item.
	 */
	གལ་ཏེ ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					     && (flags & ASSIGN_FOR_LOOP) == 0)
	{
	    emsg(_(e_cannot_lock_list_or_dict));
	    ཕྱིར;
	}

	གལ་ཏེ (lp->ll_valtype != NULL
		    && check_typval_arg_type(lp->ll_valtype, rettv,
							      NULL, 0) == FAIL)
	{
	    lp->ll_name_end = NULL;
	    ཕྱིར;
	}

	// If the lval is a List and the type of the list is not yet set,
	// then set the item type from the declared type of the variable.
	གལ་ཏེ (in_vim9script() && rettv->v_type == VAR_LIST
				&& rettv->vval.v_list != NULL
				&& rettv->vval.v_list->lv_type == NULL)
	{
	    གལ་ཏེ (lp->ll_tv->v_type == VAR_LIST
		    && lp->ll_tv->vval.v_list != NULL
		    && lp->ll_tv->vval.v_list->lv_type != NULL)
		set_tv_type(rettv, lp->ll_tv->vval.v_list->lv_type);
	}

	གལ་ཏེ (lp->ll_newkey != NULL)
	{
	    གལ་ཏེ (op != NULL && *op != '=')
	    {
		semsg(_(e_key_not_present_in_dictionary_str), lp->ll_newkey);
		ཕྱིར;
	    }
	    གལ་ཏེ (dict_wrong_func_name(lp->ll_tv->vval.v_dict, rettv,
								lp->ll_newkey))
		ཕྱིར;

	    // Need to add an item to the Dictionary.
	    di = dictitem_alloc(lp->ll_newkey);
	    གལ་ཏེ (di == NULL)
		ཕྱིར;
	    གལ་ཏེ (dict_add(lp->ll_tv->vval.v_dict, di) == FAIL)
	    {
		vim_free(di);
		ཕྱིར;
	    }
	    lp->ll_tv = &di->di_tv;
	}
	དེ་མིན གལ་ཏེ (op != NULL && *op != '=')
	{
	    tv_op(lp->ll_tv, rettv, op);
	    ཕྱིར;
	}
	དེ་མིན
	    clear_tv(lp->ll_tv);

	/*
	 * Assign the value to the variable or list item.
	 */
	གལ་ཏེ (copy)
	    copy_tv(rettv, lp->ll_tv);
	དེ་མིན
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
    static ཧྲིལ
tv_op_blob(typval_T *tv1, typval_T *tv2, char_u *op)
{
    གལ་ཏེ (*op != '+' || tv2->v_type != VAR_BLOB)
	ཕྱིར FAIL;

    // Blob += Blob
    གལ་ཏེ (tv2->vval.v_blob == NULL)
	ཕྱིར OK;

    གལ་ཏེ (tv1->vval.v_blob == NULL)
    {
	tv1->vval.v_blob = tv2->vval.v_blob;
	++tv1->vval.v_blob->bv_refcount;
	ཕྱིར OK;
    }

    blob_T	*b1 = tv1->vval.v_blob;
    blob_T	*b2 = tv2->vval.v_blob;
    ཧྲིལ		len = blob_len(b2);

    གལ་ཏེ (len > 0 && ga_grow(&b1->bv_ga, len) == OK)
    {
	mch_memmove((char_u *)b1->bv_ga.ga_data + b1->bv_ga.ga_len,
		    (char_u *)b2->bv_ga.ga_data, (size_t)len);
	b1->bv_ga.ga_len += len;
    }
    དེ་མིན
	ཆེད་དུ (ཧྲིལ i = 0; i < len; i++)
	    (void)ga_append(&b1->bv_ga, blob_get(b2, i));

    ཕྱིར OK;
}

/*
 * Handle "list1 += list2".
 * Returns OK or FAIL.
 */
    static ཧྲིལ
tv_op_list(typval_T *tv1, typval_T *tv2, char_u *op)
{
    གལ་ཏེ (*op != '+' || tv2->v_type != VAR_LIST)
	ཕྱིར FAIL;

    // List += List
    གལ་ཏེ (tv2->vval.v_list == NULL)
	ཕྱིར OK;

    གལ་ཏེ (tv1->vval.v_list == NULL)
    {
	tv1->vval.v_list = tv2->vval.v_list;
	++tv1->vval.v_list->lv_refcount;
    }
    དེ་མིན
	list_extend(tv1->vval.v_list, tv2->vval.v_list, NULL);

    ཕྱིར OK;
}

/*
 * Handle number operations:
 *	nr += nr , nr -= nr , nr *=nr , nr /= nr , nr %= nr
 *
 * Returns OK or FAIL.
 */
    static ཧྲིལ
tv_op_number(typval_T *tv1, typval_T *tv2, char_u *op)
{
    varnumber_T	n;
    ཧྲིལ		failed = FALSE;

    n = tv_get_number(tv1);
    གལ་ཏེ (tv2->v_type == VAR_FLOAT)
    {
	float_T f = n;

	གལ་ཏེ (*op == '%')
	    ཕྱིར FAIL;
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
    དེ་མིན
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

    ཕྱིར failed ? FAIL : OK;
}

/*
 * Append string "s2" to the string in "tv1".
 * Returns OK if "tv1" was grown in place, FAIL otherwise.
 */
    static ཧྲིལ
grow_string_tv(typval_T *tv1, char_u *s2)
{
    གལ་ཏེ (tv1->v_type != VAR_STRING || tv1->vval.v_string == NULL)
	ཕྱིར FAIL;

    size_t len1 = STRLEN(tv1->vval.v_string);
    size_t len2 = STRLEN(s2);
    char_u *p = vim_realloc(tv1->vval.v_string, len1 + len2 + 1);

    གལ་ཏེ (p == NULL)
	ཕྱིར FAIL;

    mch_memmove(p + len1, s2, len2 + 1);
    tv1->vval.v_string = p;
    ཕྱིར OK;
}

/*
 * Handle "str1 .= str2"
 * Returns OK or FAIL.
 */
    static ཧྲིལ
tv_op_string(typval_T *tv1, typval_T *tv2, char_u *op UNUSED)
{
    char_u	numbuf[NUMBUFLEN];
    char_u	*s;
    char_u	*s2;

    གལ་ཏེ (tv2->v_type == VAR_FLOAT)
	ཕྱིར FAIL;

    // str .= str
    s2 = tv_get_string_buf(tv2, numbuf);
    གལ་ཏེ (grow_string_tv(tv1, s2) == OK)
	ཕྱིར OK;

    s = tv_get_string(tv1);
    s = concat_str(s, s2);
    clear_tv(tv1);
    tv1->v_type = VAR_STRING;
    tv1->vval.v_string = s;

    ཕྱིར OK;
}

/*
 * Handle "tv1 += tv2", "tv1 -= tv2", "tv1 *= tv2", "tv1 /= tv2", "tv1 %= tv2"
 * and "tv1 .= tv2"
 * Returns OK or FAIL.
 */
    static ཧྲིལ
tv_op_nr_or_string(typval_T *tv1, typval_T *tv2, char_u *op)
{
    གལ་ཏེ (tv2->v_type == VAR_LIST)
	ཕྱིར FAIL;

    གལ་ཏེ (vim_strchr((char_u *)"+-*/%", *op) != NULL)
	ཕྱིར tv_op_number(tv1, tv2, op);

    ཕྱིར tv_op_string(tv1, tv2, op);
}

/*
 * Handle "f1 += f2", "f1 -= f2", "f1 *= f2", "f1 /= f2".
 * Returns OK or FAIL.
 */
    static ཧྲིལ
tv_op_float(typval_T *tv1, typval_T *tv2, char_u *op)
{
    float_T f;

    གལ་ཏེ (*op == '%' || *op == '.'
	    || (tv2->v_type != VAR_FLOAT
		&& tv2->v_type != VAR_NUMBER
		&& tv2->v_type != VAR_STRING))
	ཕྱིར FAIL;

    གལ་ཏེ (tv2->v_type == VAR_FLOAT)
	f = tv2->vval.v_float;
    དེ་མིན
	f = tv_get_number(tv2);
    switch (*op)
    {
	pak_sa '+': tv1->vval.v_float += f; bha_nga;
	pak_sa '-': tv1->vval.v_float -= f; bha_nga;
	pak_sa '*': tv1->vval.v_float *= f; bha_nga;
	pak_sa '/': tv1->vval.v_float /= f; bha_nga;
    }

    ཕྱིར OK;
}

/*
 * Handle "tv1 += tv2", "tv1 -= tv2", "tv1 *= tv2", "tv1 /= tv2", "tv1 %= tv2"
 * and "tv1 .= tv2"
 * Returns OK or FAIL.
 */
    ཧྲིལ
tv_op(typval_T *tv1, typval_T *tv2, char_u *op)
{
    // Can't do anything with a Funcref or Dict on the right.
    // v:true and friends only work with "..=".
    གལ་ཏེ (tv2->v_type == VAR_FUNC || tv2->v_type == VAR_DICT
		|| ((tv2->v_type == VAR_BOOL || tv2->v_type == VAR_SPECIAL)
							&& *op != '.'))
    {
	semsg(_(e_wrong_variable_type_for_str_equal), op);
	ཕྱིར FAIL;
    }

    ཧྲིལ retval = FAIL;
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

    གལ་ཏེ (retval != OK)
	semsg(_(e_wrong_variable_type_for_str_equal), op);

    ཕྱིར retval;
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
    ཧྲིལ		*errp,
    exarg_T	*eap,
    evalarg_T	*evalarg)
{
    forinfo_T	*fi;
    char_u	*var_list_end;
    char_u	*expr;
    typval_T	tv;
    list_T	*l;
    tuple_T	*tuple;
    ཧྲིལ		skip = !(evalarg->eval_flags & EVAL_EVALUATE);

    *errp = TRUE;	// default: there is an error

    fi = ALLOC_CLEAR_ONE(forinfo_T);
    གལ་ཏེ (fi == NULL)
	ཕྱིར NULL;

    var_list_end = skip_var_list(arg, TRUE, &fi->fi_varcount,
						     &fi->fi_semicolon, FALSE);
    གལ་ཏེ (var_list_end == NULL)
	ཕྱིར fi;

    expr = skipwhite_and_linebreak(var_list_end, evalarg);
    གལ་ཏེ (expr[0] != 'i' || expr[1] != 'n'
				  || !(expr[2] == NUL || VIM_ISWHITE(expr[2])))
    {
	གལ་ཏེ (in_vim9script() && *expr == ':' && expr != var_list_end)
	    semsg(_(e_no_white_space_allowed_before_colon_str), expr);
	དེ་མིན
	    emsg(_(e_missing_in_after_for));
	ཕྱིར fi;
    }

    གལ་ཏེ (skip)
	++emsg_skip;
    expr = skipwhite_and_linebreak(expr + 2, evalarg);
    གལ་ཏེ (eval0(expr, &tv, eap, evalarg) == OK)
    {
	*errp = FALSE;
	གལ་ཏེ (!skip)
	{
	    གལ་ཏེ (tv.v_type == VAR_LIST)
	    {
		l = tv.vval.v_list;
		གལ་ཏེ (l == NULL)
		{
		    // a null list is like an empty list: do nothing
		    clear_tv(&tv);
		}
		དེ་མིན
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
	    དེ་མིན གལ་ཏེ (tv.v_type == VAR_TUPLE)
	    {
		tuple = tv.vval.v_tuple;
		གལ་ཏེ (tuple == NULL)
		{
		    // a null tuple is like an empty tuple: do nothing
		    clear_tv(&tv);
		}
		དེ་མིན
		{
		    // No need to increment the refcount, it's already set for
		    // the tuple being used in "tv".
		    fi->fi_tuple = tuple;
		    fi->fi_tuple_idx = 0;
		}
	    }
	    དེ་མིན གལ་ཏེ (tv.v_type == VAR_BLOB)
	    {
		fi->fi_bi = 0;
		གལ་ཏེ (tv.vval.v_blob != NULL)
		{
		    // Make a copy, so that the iteration still works when the
		    // blob is changed.
		    fi->fi_blob = blob_copy(tv.vval.v_blob);
		}
		clear_tv(&tv);
	    }
	    དེ་མིན གལ་ཏེ (tv.v_type == VAR_STRING)
	    {
		fi->fi_byte_idx = 0;
		fi->fi_string = tv.vval.v_string;
		tv.vval.v_string = NULL;
		གལ་ཏེ (fi->fi_string == NULL)
		    fi->fi_string = vim_strsave((char_u *)"");
	    }
	    དེ་མིན
	    {
		emsg(_(e_string_list_tuple_or_blob_required));
		clear_tv(&tv);
	    }
	}
    }
    གལ་ཏེ (skip)
	--emsg_skip;
    fi->fi_break_count = evalarg->eval_break_count;

    ཕྱིར fi;
}

/*
 * Used when looping over a :for line, skip the "in expr" part.
 */
    void
skip_for_lines(void *fi_void, evalarg_T *evalarg)
{
    forinfo_T	*fi = (forinfo_T *)fi_void;
    ཧྲིལ		i;

    ཆེད་དུ (i = 0; i < fi->fi_break_count; ++i)
	eval_next_line(NULL, evalarg);
}

/*
 * Use the first item in a ":for" list.  Advance to the next.
 * Assign the values to the variable (list).  "arg" points to the first one.
 * Return TRUE when a valid item was found, FALSE when at end of list or
 * something wrong.
 */
    ཧྲིལ
next_for_item(void *fi_void, char_u *arg)
{
    forinfo_T	*fi = (forinfo_T *)fi_void;
    ཧྲིལ		result;
    ཧྲིལ		flag = ASSIGN_FOR_LOOP | (in_vim9script()
			 ? (ASSIGN_FINAL
			     // first round: error if variable exists
			     | (fi->fi_bi == 0 ? 0 : ASSIGN_DECL)
			     | ASSIGN_NO_MEMBER_TYPE
			     | ASSIGN_UPDATE_BLOCK_ID)
			 : 0);
    listitem_T	*item;
    ཧྲིལ		skip_assign = in_vim9script() && arg[0] == '_'
						      && !eval_isnamec(arg[1]);

    གལ་ཏེ (fi->fi_blob != NULL)
    {
	typval_T	tv;

	གལ་ཏེ (fi->fi_bi >= blob_len(fi->fi_blob))
	    ཕྱིར FALSE;
	tv.v_type = VAR_NUMBER;
	tv.v_lock = VAR_FIXED;
	tv.vval.v_number = blob_get(fi->fi_blob, fi->fi_bi);
	++fi->fi_bi;
	གལ་ཏེ (skip_assign)
	    ཕྱིར TRUE;
	ཕྱིར ex_let_vars(arg, &tv, TRUE, fi->fi_semicolon,
					    fi->fi_varcount, flag, NULL) == OK;
    }

    གལ་ཏེ (fi->fi_string != NULL)
    {
	typval_T	tv;
	ཧྲིལ		len;

	len = mb_ptr2len(fi->fi_string + fi->fi_byte_idx);
	གལ་ཏེ (len == 0)
	    ཕྱིར FALSE;
	tv.v_type = VAR_STRING;
	tv.v_lock = VAR_FIXED;
	tv.vval.v_string = vim_strnsave(fi->fi_string + fi->fi_byte_idx, len);
	fi->fi_byte_idx += len;
	++fi->fi_bi;
	གལ་ཏེ (skip_assign)
	    result = TRUE;
	དེ་མིན
	    result = ex_let_vars(arg, &tv, TRUE, fi->fi_semicolon,
					    fi->fi_varcount, flag, NULL) == OK;
	vim_free(tv.vval.v_string);
	ཕྱིར result;
    }

    གལ་ཏེ (fi->fi_tuple != NULL)
    {
	typval_T	tv;

	གལ་ཏེ (fi->fi_tuple_idx >= TUPLE_LEN(fi->fi_tuple))
	    ཕྱིར FALSE;

	copy_tv(TUPLE_ITEM(fi->fi_tuple, fi->fi_tuple_idx), &tv);
	++fi->fi_tuple_idx;
	++fi->fi_bi;
	གལ་ཏེ (skip_assign)
	    result = TRUE;
	དེ་མིན
	    result = ex_let_vars(arg, &tv, TRUE, fi->fi_semicolon,
					    fi->fi_varcount, flag, NULL) == OK;
	clear_tv(&tv);
	ཕྱིར result;
    }

    item = fi->fi_lw.lw_item;
    གལ་ཏེ (item == NULL)
	result = FALSE;
    དེ་མིན
    {
	fi->fi_lw.lw_item = item->li_next;
	++fi->fi_bi;
	གལ་ཏེ (skip_assign)
	    result = TRUE;
	དེ་མིན
	    result = (ex_let_vars(arg, &item->li_tv, TRUE, fi->fi_semicolon,
					   fi->fi_varcount, flag, NULL) == OK);
    }
    ཕྱིར result;
}

/*
 * Free the structure used to store info used by ":for".
 */
    void
free_for_info(void *fi_void)
{
    forinfo_T    *fi = (forinfo_T *)fi_void;

    གལ་ཏེ (fi == NULL)
	ཕྱིར;
    གལ་ཏེ (fi->fi_list != NULL)
    {
	list_rem_watch(fi->fi_list, &fi->fi_lw);
	list_unref(fi->fi_list);
    }
    དེ་མིན གལ་ཏེ (fi->fi_blob != NULL)
	blob_unref(fi->fi_blob);
    དེ་མིན གལ་ཏེ (fi->fi_tuple != NULL)
	tuple_unref(fi->fi_tuple);
    དེ་མིན
	vim_free(fi->fi_string);
    vim_free(fi);
}

    void
set_context_for_expression(
    expand_T	*xp,
    char_u	*arg,
    cmdidx_T	cmdidx)
{
    ཧྲིལ		has_expr = cmdidx != CMD_let && cmdidx != CMD_var;
    ཧྲིལ		c;
    char_u	*p;

    གལ་ཏེ (cmdidx == CMD_let || cmdidx == CMD_var
				 || cmdidx == CMD_const || cmdidx == CMD_final)
    {
	xp->xp_context = EXPAND_USER_VARS;
	གལ་ཏེ (vim_strpbrk(arg, (char_u *)"\"'+-*/%.=!?~|&$([<>,#") == NULL)
	{
	    // ":let var1 var2 ...": find last space.
	    ཆེད་དུ (p = arg + STRLEN(arg); p >= arg; )
	    {
		xp->xp_pattern = p;
		MB_PTR_BACK(arg, p);
		གལ་ཏེ (VIM_ISWHITE(*p))
		    bha_nga;
	    }
	    ཕྱིར;
	}
    }
    དེ་མིན
	xp->xp_context = cmdidx == CMD_call ? EXPAND_FUNCTIONS
							  : EXPAND_EXPRESSION;
    སྐབས ((xp->xp_pattern = vim_strpbrk(arg,
				  (char_u *)"\"'+-*/%.=!?~|&$([<>,#")) != NULL)
    {
	c = *xp->xp_pattern;
	གལ་ཏེ (c == '&')
	{
	    c = xp->xp_pattern[1];
	    གལ་ཏེ (c == '&')
	    {
		++xp->xp_pattern;
		xp->xp_context = has_expr ? EXPAND_EXPRESSION : EXPAND_NOTHING;
	    }
	    དེ་མིན གལ་ཏེ (c != ' ')
	    {
		xp->xp_context = EXPAND_SETTINGS;
		གལ་ཏེ ((c == 'l' || c == 'g') && xp->xp_pattern[2] == ':')
		    xp->xp_pattern += 2;

	    }
	}
	དེ་མིན གལ་ཏེ (c == '$')
	{
	    // environment variable
	    xp->xp_context = EXPAND_ENV_VARS;
	}
	དེ་མིན གལ་ཏེ (c == '=')
	{
	    has_expr = TRUE;
	    xp->xp_context = EXPAND_EXPRESSION;
	}
	དེ་མིན གལ་ཏེ (c == '#'
		&& xp->xp_context == EXPAND_EXPRESSION)
	{
	    // Autoload function/variable contains '#'.
	    bha_nga;
	}
	དེ་མིན གལ་ཏེ ((c == '<' || c == '#')
		&& xp->xp_context == EXPAND_FUNCTIONS
		&& vim_strchr(xp->xp_pattern, '(') == NULL)
	{
	    // Function name can start with "<SNR>" and contain '#'.
	    bha_nga;
	}
	དེ་མིན གལ་ཏེ (has_expr)
	{
	    གལ་ཏེ (c == '"')	    // string
	    {
		སྐབས ((c = *++xp->xp_pattern) != NUL && c != '"')
		    གལ་ཏེ (c == '\\' && xp->xp_pattern[1] != NUL)
			++xp->xp_pattern;
		xp->xp_context = EXPAND_NOTHING;
	    }
	    དེ་མིན གལ་ཏེ (c == '\'')	    // literal string
	    {
		// Trick: '' is like stopping and starting a literal string.
		སྐབས ((c = *++xp->xp_pattern) != NUL && c != '\'')
		    /* skip */ ;
		xp->xp_context = EXPAND_NOTHING;
	    }
	    དེ་མིན གལ་ཏེ (c == '|')
	    {
		གལ་ཏེ (xp->xp_pattern[1] == '|')
		{
		    ++xp->xp_pattern;
		    xp->xp_context = EXPAND_EXPRESSION;
		}
		དེ་མིན
		    xp->xp_context = EXPAND_COMMANDS;
	    }
	    དེ་མིན
		xp->xp_context = EXPAND_EXPRESSION;
	}
	དེ་མིན
	    // Doesn't look like something valid, expand as an expression
	    // anyway.
	    xp->xp_context = EXPAND_EXPRESSION;
	arg = xp->xp_pattern;
	གལ་ཏེ (*arg != NUL)
	    སྐབས ((c = *++arg) != NUL && (c == ' ' || c == '\t'))
		/* skip */ ;
    }

    // ":exe one two" completes "two"
    གལ་ཏེ ((cmdidx == CMD_execute
		|| cmdidx == CMD_echo
		|| cmdidx == CMD_echon
		|| cmdidx == CMD_echomsg
		|| cmdidx == CMD_echoerr
		|| cmdidx == CMD_echoconsole
		|| cmdidx == CMD_echowindow)
	    && xp->xp_context == EXPAND_EXPRESSION)
    {
	ཆེད་དུ (;;)
	{
	    char_u *n = skiptowhite(arg);

	    གལ་ཏེ (n == arg || IS_WHITE_OR_NUL(*skipwhite(n)))
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
static ཧྲིལ	    eval_prog_busy = 0;	// programs eval_regcomp() handed out
					// that were not given back yet
static ཧྲིལ	    eval_prog_reentered = FALSE;  // eval_regcomp() ran while
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

    གལ་ཏེ (STRNCMP(pat, "\\%#=", 4) == 0)
	ཕྱིར true;
    ཆེད་དུ (p = pat; *p != NUL; ++p)
	གལ་ཏེ (*p == '~'
		|| (p[0] == '[' && (p[1] == ':' || p[1] == '=' || p[1] == '.'))
		|| (p[0] == '%' && (p[1] == '.'
			|| ((p[1] == '<' || p[1] == '>') && p[2] == '.'))))
	    ཕྱིར true;
    ཕྱིར false;
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
    གལ་ཏེ (eval_prog_busy > 0)
	// The samples above no longer describe the program already handed
	// out: its eval_regfree() must drop it instead of caching it.
	eval_prog_reentered = TRUE;

    གལ་ཏེ (eval_prog_cache != NULL
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
    དེ་མིན
	prog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
    གལ་ཏེ (prog != NULL)
	++eval_prog_busy;	// paired with the eval_regfree() call
    ཕྱིར prog;
}

/*
 * Free program "prog", obtained with eval_regcomp() for pattern "pat", by
 * keeping it in the cache for the next use.  "pat" must be the same string
 * the program was compiled for and must still be valid.
 */
    void
eval_regfree(char_u *pat, regprog_T *prog)
{
    ཧྲིལ	    reentered = eval_prog_reentered;

    གལ་ཏེ (eval_prog_busy > 0 && --eval_prog_busy == 0)
	eval_prog_reentered = FALSE;
    གལ་ཏེ (prog == NULL)
	ཕྱིར;
    གལ་ཏེ (reentered || *p_cpo != NUL
	    || eval_compile_re != p_re || eval_compile_enc != p_enc)
    {
	// 'cpoptions' is not empty, an option in the cache key changed, or
	// another program was handed out while the caller was using this
	// one: the samples may not describe this program, do not cache it.
	vim_regfree(prog);
	ཕྱིར;
    }
    གལ་ཏེ (eval_prog_pat == NULL || STRCMP(eval_prog_pat, pat) != 0
	    || STRCMP(eval_prog_enc, p_enc) != 0)
    {
	char_u	*pat_copy;
	char_u	*enc_copy;

	// A cached pattern is known not to be volatile, thus the scan is
	// only needed when installing a new key.
	གལ་ཏེ (eval_prog_volatile(pat))
	{
	    // compiling depends on state the cache key does not cover
	    vim_regfree(prog);
	    ཕྱིར;
	}
	pat_copy = vim_strsave(pat);
	enc_copy = vim_strsave(p_enc);
	གལ་ཏེ (pat_copy == NULL || enc_copy == NULL)
	{
	    vim_free(pat_copy);
	    vim_free(enc_copy);
	    vim_regfree(prog);
	    ཕྱིར;
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

#གལ་ཏེ defined(EXITFREE)
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
    ཧྲིལ
pattern_match(char_u *pat, char_u *text, ཧྲིལ ic)
{
    ཧྲིལ		matches = FALSE;
    char_u	*save_cpo;
    regmatch_T	regmatch;

    // avoid 'l' flag in 'cpoptions'
    save_cpo = p_cpo;
    p_cpo = empty_option;
    regmatch.regprog = eval_regcomp(pat);
    གལ་ཏེ (regmatch.regprog != NULL)
    {
	regmatch.rm_ic = ic;
	matches = vim_regexec_nl(&regmatch, text, (colnr_T)0);
	eval_regfree(pat, regmatch.regprog);
    }
    p_cpo = save_cpo;
    ཕྱིར matches;
}

/*
 * Handle a name followed by "(".  Both for just "name(arg)" and for
 * "expr->name(arg)".
 * Returns OK or FAIL.
 */
    static ཧྲིལ
eval_func(
	char_u	    **arg,	// points to "(", will be advanced
	evalarg_T   *evalarg,
	char_u	    *name,
	ཧྲིལ	    name_len,
	typval_T    *rettv,
	ཧྲིལ	    flags,
	typval_T    *basetv)	// "expr" for "expr->name(arg)"
{
    ཧྲིལ		evaluate = flags & EVAL_EVALUATE;
    char_u	*s = name;
    ཧྲིལ		len = name_len;
    partial_T	*partial;
    ཧྲིལ		ret = OK;
    type_T	*type = NULL;
    ཧྲིལ		found_var = FALSE;

    གལ་ཏེ (!evaluate)
	check_vars(s, len);

    // If "s" is the name of a variable of type VAR_FUNC
    // use its contents.
    s = deref_func_name(s, &len, &partial,
		 in_vim9script() ? &type : NULL, !evaluate, FALSE, &found_var);

    // Need to make a copy, in case evaluating the arguments makes
    // the name invalid.
    s = vim_strsave(s);
    གལ་ཏེ (s == NULL || (evaluate && *s == NUL))
	ret = FAIL;
    དེ་མིན
    {
	funcexe_T funcexe;

	// Invoke the function.
	CLEAR_FIELD(funcexe);
	funcexe.fe_firstline = curwin->w_cursor.lnum;
	funcexe.fe_lastline = curwin->w_cursor.lnum;
	funcexe.fe_evaluate = evaluate;
	funcexe.fe_partial = partial;
	གལ་ཏེ (partial != NULL)
	{
	    funcexe.fe_object = partial->pt_obj;
	    གལ་ཏེ (funcexe.fe_object != NULL)
		++funcexe.fe_object->obj_refcount;
	}
	funcexe.fe_basetv = basetv;
	funcexe.fe_check_type = type;
	funcexe.fe_found_var = found_var;
	གལ་ཏེ (evalarg != NULL)
	    funcexe.fe_cctx = evalarg->eval_cctx;
	ret = get_func_tv(s, len, rettv, arg, evalarg, &funcexe);
    }
    vim_free(s);

    // If evaluate is FALSE rettv->v_type was not set in
    // get_func_tv, but it's needed in handle_subscript() to parse
    // what follows. So set it here.
    གལ་ཏེ (rettv->v_type == VAR_UNKNOWN && !evaluate && **arg == '(')
    {
	rettv->vval.v_string = NULL;
	rettv->v_type = VAR_FUNC;
    }

    // Stop the expression evaluation when immediately
    // aborting on error, or when an interrupt occurred or
    // an exception was thrown but not caught.
    གལ་ཏེ (evaluate && aborting())
    {
	གལ་ཏེ (ret == OK)
	    clear_tv(rettv);
	ret = FAIL;
    }
    ཕྱིར ret;
}

/*
 * After a NL, skip over empty lines and comment-only lines.
 */
    static char_u *
newline_skip_comments(char_u *arg)
{
    char_u *p = arg + 1;

    ཆེད་དུ (;;)
    {
	p = skipwhite(p);

	གལ་ཏེ (*p == NUL)
	    bha_nga;
	གལ་ཏེ (vim9_comment_start(p))
	{
	    char_u *nl = vim_strchr(p, NL);

	    གལ་ཏེ (nl == NULL)
		bha_nga;
	    p = nl;
	}
	གལ་ཏེ (*p != NL)
	    bha_nga;
	++p;  // skip another NL
    }
    ཕྱིར p;
}

/*
 * Get the next line source line without advancing.  But do skip over comment
 * lines.
 * Only called for Vim9 script.
 */
    static char_u *
getline_peek_skip_comments(evalarg_T *evalarg)
{
    ཆེད་དུ (;;)
    {
	char_u *next = getline_peek(evalarg->eval_getline,
							 evalarg->eval_cookie);
	char_u *p;

	གལ་ཏེ (next == NULL)
	    bha_nga;
	p = skipwhite(next);
	གལ་ཏེ (*p != NUL && !vim9_comment_start(p))
	    ཕྱིར next;
	གལ་ཏེ (eval_next_line(NULL, evalarg) == NULL)
	    bha_nga;
    }
    ཕྱིར NULL;
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
eval_next_non_blank(char_u *arg, evalarg_T *evalarg, ཧྲིལ *getnext)
{
    char_u *p = skipwhite(arg);

    *getnext = FALSE;
    གལ་ཏེ (in_vim9script()
	    && evalarg != NULL
	    && (evalarg->eval_cookie != NULL || evalarg->eval_cctx != NULL
								   || *p == NL)
	    && (*p == NUL || *p == NL
			     || (vim9_comment_start(p) && VIM_ISWHITE(p[-1]))))
    {
	char_u *next;

	གལ་ཏེ (*p == NL)
	    next = newline_skip_comments(p);
	དེ་མིན གལ་ཏེ (evalarg->eval_cookie != NULL)
	    next = getline_peek_skip_comments(evalarg);
	དེ་མིན
	    next = peek_next_line_from_context(evalarg->eval_cctx);

	གལ་ཏེ (next != NULL)
	{
	    *getnext = *p != NL;
	    ཕྱིར skipwhite(next);
	}
    }
    ཕྱིར p;
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

    གལ་ཏེ (arg != NULL)
    {
	གལ་ཏེ (*arg == NL)
	    ཕྱིར newline_skip_comments(arg);
	// Truncate before a trailing comment, so that concatenating the lines
	// won't turn the rest into a comment.
	གལ་ཏེ (*skipwhite(arg) == '#')
	    *arg = NUL;
    }

    གལ་ཏེ (evalarg->eval_cookie != NULL)
	line = evalarg->eval_getline(0, evalarg->eval_cookie, 0,
							   GETLINE_CONCAT_ALL);
    དེ་མིན
	line = next_line_from_context(evalarg->eval_cctx, TRUE);
    གལ་ཏེ (line == NULL)
	ཕྱིར NULL;

    ++evalarg->eval_break_count;
    གལ་ཏེ (gap->ga_itemsize > 0 && ga_grow(gap, 1) == OK)
    {
	char_u *p = skipwhite(line);

	// Going to concatenate the lines after parsing.  For an empty or
	// comment line use an empty string.
	གལ་ཏེ (*p == NUL || vim9_comment_start(p))
	{
	    vim_free(line);
	    line = vim_strsave((char_u *)"");
	}

	((char_u **)gap->ga_data)[gap->ga_len] = line;
	++gap->ga_len;
    }
    དེ་མིན གལ་ཏེ (evalarg->eval_cookie != NULL)
    {
	free_eval_tofree_later(evalarg);
	evalarg->eval_tofree = line;
    }

    // Advanced to the next line, "arg" no longer points into the previous
    // line.  The caller assigns the return value to "arg".
    // If "arg" is NULL, then the return value is discarded.  In that case,
    // "arg" still points to the previous line.  So don't reset
    // "eval_using_cmdline".
    གལ་ཏེ (arg != NULL)
	evalarg->eval_using_cmdline = FALSE;
    ཕྱིར skipwhite(line);
}

/*
 * Call eval_next_non_blank() and get the next line if needed.
 */
    char_u *
skipwhite_and_linebreak(char_u *arg, evalarg_T *evalarg)
{
    ཧྲིལ	    getnext;
    char_u  *p = skipwhite_and_nl(arg);

    གལ་ཏེ (evalarg == NULL)
	ཕྱིར skipwhite(arg);
    eval_next_non_blank(p, evalarg, &getnext);
    གལ་ཏེ (getnext)
	ཕྱིར eval_next_line(arg, evalarg);
    ཕྱིར p;
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
    ཧྲིལ
eval0(
    char_u	*arg,
    typval_T	*rettv,
    exarg_T	*eap,
    evalarg_T	*evalarg)
{
    ཕྱིར eval0_retarg(arg, rettv, eap, evalarg, NULL);
}

/*
 * If "arg" is a simple function call without arguments then call it and return
 * the result.  Otherwise return NOTDONE.
 */
    ཧྲིལ
may_call_simple_func(
    char_u	*arg,
    typval_T	*rettv)
{
    char_u  *parens = (char_u *)strstr((var_na *)arg, "()");
    ཧྲིལ	    r = NOTDONE;

    // If the expression is "FuncName()" then we can skip a lot of overhead.
    གལ་ཏེ (parens != NULL && *skipwhite(parens + 2) == NUL)
    {
	char_u *p = STRNCMP(arg, "<SNR>", 5) == 0 ? skipdigits(arg + 5) : arg;

	གལ་ཏེ (to_name_end(p, TRUE) == parens)
	    r = call_simple_func(arg, (size_t)(parens - arg), rettv);
    }
    ཕྱིར r;
}

/*
 * Handle zero level expression with optimization for a simple function call.
 * Same arguments and return value as eval0().
 */
    static ཧྲིལ
eval0_simple_funccal(
    char_u	*arg,
    typval_T	*rettv,
    exarg_T	*eap,
    evalarg_T	*evalarg)
{
    ཧྲིལ	    r = may_call_simple_func(arg, rettv);

    གལ་ཏེ (r == NOTDONE)
	r = eval0_retarg(arg, rettv, eap, evalarg, NULL);
    ཕྱིར r;
}

/*
 * Like eval0() but when "retarg" is not NULL store the pointer to after the
 * expression and don't check what comes after the expression.
 */
    ཧྲིལ
eval0_retarg(
    char_u	*arg,
    typval_T	*rettv,
    exarg_T	*eap,
    evalarg_T	*evalarg,
    char_u	**retarg)
{
    ཧྲིལ		ret;
    char_u	*p;
    char_u	*expr_end;
    ཧྲིལ		did_emsg_before = did_emsg;
    ཧྲིལ		called_emsg_before = called_emsg;
    ཧྲིལ		check_for_end = retarg == NULL;
    ཧྲིལ		end_error = FALSE;

    p = skipwhite(arg);
    ret = eval1(&p, rettv, evalarg);

    གལ་ཏེ (ret != FAIL)
    {
	expr_end = p;
	p = skipwhite(p);

	// In Vim9 script a command block is not split at NL characters for
	// commands using an expression argument.  Skip over a '#' comment to
	// check for a following NL.  Require white space before the '#'.
	གལ་ཏེ (in_vim9script() && p > expr_end && retarg == NULL)
	    སྐབས (*p == '#')
	    {
		char_u *nl = vim_strchr(p, NL);

		གལ་ཏེ (nl == NULL)
		    bha_nga;
		p = skipwhite(nl + 1);
		གལ་ཏེ (eap != NULL && *p != NUL)
		    eap->nextcmd = p;
		check_for_end = FALSE;
	    }

	གལ་ཏེ (check_for_end)
	    end_error = !ends_excmd2(arg, p);
    }

    གལ་ཏེ (ret == FAIL || end_error)
    {
	གལ་ཏེ (ret != FAIL)
	    clear_tv(rettv);
	/*
	 * Report the invalid expression unless the expression evaluation has
	 * been cancelled due to an aborting error, an interrupt, or an
	 * exception, or we already gave a more specific error.
	 * Also check called_emsg for when using assert_fails().
	 */
	གལ་ཏེ (!aborting()
		&& did_emsg == did_emsg_before
		&& called_emsg == called_emsg_before
		&& (!in_vim9script() || !vim9_bad_comment(p)))
	{
	    གལ་ཏེ (end_error)
		semsg(_(e_trailing_characters_str), p);
	    དེ་མིན
		semsg(_(e_invalid_expression_str), arg);
	}

	གལ་ཏེ (eap != NULL && p != NULL)
	{
	    // Some of the expression may not have been consumed.
	    // Only execute a next command if it cannot be a "||" operator.
	    // The next command may be "catch".
	    char_u *nextcmd = check_nextcmd(p);
	    གལ་ཏེ (nextcmd != NULL && *nextcmd != '|')
		eap->nextcmd = nextcmd;
	}
	ཕྱིར FAIL;
    }

    གལ་ཏེ (retarg != NULL)
	*retarg = p;
    དེ་མིན གལ་ཏེ (check_for_end && eap != NULL)
	set_nextcmd(eap, p);

    ཕྱིར ret;
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
    ཧྲིལ
eval1(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    char_u  *p;
    ཧྲིལ	    getnext;

    CLEAR_POINTER(rettv);

    /*
     * Get the first variable.
     */
    གལ་ཏེ (eval2(arg, rettv, evalarg) == FAIL)
	ཕྱིར FAIL;

    p = eval_next_non_blank(*arg, evalarg, &getnext);
    གལ་ཏེ (*p == '?')
    {
	ཧྲིལ		op_falsy = p[1] == '?';
	ཧྲིལ		result;
	typval_T	var2;
	evalarg_T	*evalarg_used = evalarg;
	evalarg_T	local_evalarg;
	ཧྲིལ		orig_flags;
	ཧྲིལ		evaluate;
	ཧྲིལ		vim9script = in_vim9script();

	གལ་ཏེ (evalarg == NULL)
	{
	    init_evalarg(&local_evalarg);
	    evalarg_used = &local_evalarg;
	}
	orig_flags = evalarg_used->eval_flags;
	evaluate = evalarg_used->eval_flags & EVAL_EVALUATE;

	གལ་ཏེ (getnext)
	    *arg = eval_next_line(*arg, evalarg_used);
	དེ་མིན
	{
	    གལ་ཏེ (evaluate && vim9script && !VIM_ISWHITE(p[-1]))
	    {
		error_white_both(p, op_falsy ? 2 : 1);
		clear_tv(rettv);
		ཕྱིར FAIL;
	    }
	    *arg = p;
	}

	result = FALSE;
	གལ་ཏེ (evaluate)
	{
	    ཧྲིལ		error = FALSE;

	    གལ་ཏེ (op_falsy)
	    {
		// Is this typeval supported with the falsy operator?
		གལ་ཏེ (check_typval_is_value(rettv) == FAIL)
		{
		    clear_tv(rettv);
		    ཕྱིར FAIL;
		}
		result = tv2bool(rettv);
	    }
	    དེ་མིན གལ་ཏེ (vim9script)
		result = tv_get_bool_chk(rettv, &error);
	    དེ་མིན གལ་ཏེ (tv_get_number_chk(rettv, &error) != 0)
		result = TRUE;
	    གལ་ཏེ (error || !op_falsy || !result)
		clear_tv(rettv);
	    གལ་ཏེ (error)
		ཕྱིར FAIL;
	}

	/*
	 * Get the second variable.  Recursive!
	 */
	གལ་ཏེ (op_falsy)
	    ++*arg;
	གལ་ཏེ (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[1]))
	{
	    error_white_both(*arg - (op_falsy ? 1 : 0), op_falsy ? 2 : 1);
	    clear_tv(rettv);
	    ཕྱིར FAIL;
	}
	*arg = skipwhite_and_linebreak(*arg + 1, evalarg_used);
	evalarg_used->eval_flags = (op_falsy ? !result : result)
				  ? orig_flags : (orig_flags & ~EVAL_EVALUATE);
	གལ་ཏེ (eval1(arg, &var2, evalarg_used) == FAIL)
	{
	    evalarg_used->eval_flags = orig_flags;
	    ཕྱིར FAIL;
	}
	གལ་ཏེ (!op_falsy || !result)
	    *rettv = var2;

	གལ་ཏེ (!op_falsy)
	{
	    /*
	     * Check for the ":".
	     */
	    p = eval_next_non_blank(*arg, evalarg_used, &getnext);
	    གལ་ཏེ (*p != ':')
	    {
		emsg(_(e_missing_colon_after_questionmark));
		གལ་ཏེ (evaluate && result)
		    clear_tv(rettv);
		evalarg_used->eval_flags = orig_flags;
		ཕྱིར FAIL;
	    }
	    གལ་ཏེ (getnext)
		*arg = eval_next_line(*arg, evalarg_used);
	    དེ་མིན
	    {
		གལ་ཏེ (evaluate && vim9script && !VIM_ISWHITE(p[-1]))
		{
		    error_white_both(p, 1);
		    clear_tv(rettv);
		    evalarg_used->eval_flags = orig_flags;
		    ཕྱིར FAIL;
		}
		*arg = p;
	    }

	    /*
	     * Get the third variable.  Recursive!
	     */
	    གལ་ཏེ (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[1]))
	    {
		error_white_both(*arg, 1);
		clear_tv(rettv);
		evalarg_used->eval_flags = orig_flags;
		ཕྱིར FAIL;
	    }
	    *arg = skipwhite_and_linebreak(*arg + 1, evalarg_used);
	    evalarg_used->eval_flags = !result ? orig_flags
					       : (orig_flags & ~EVAL_EVALUATE);
	    གལ་ཏེ (eval1(arg, &var2, evalarg_used) == FAIL)
	    {
		གལ་ཏེ (evaluate && result)
		    clear_tv(rettv);
		evalarg_used->eval_flags = orig_flags;
		ཕྱིར FAIL;
	    }
	    གལ་ཏེ (evaluate && !result)
		*rettv = var2;
	}

	གལ་ཏེ (evalarg == NULL)
	    clear_evalarg(&local_evalarg, NULL);
	དེ་མིན
	    evalarg->eval_flags = orig_flags;
    }

    ཕྱིར OK;
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
    static ཧྲིལ
eval2(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    char_u	*p;
    ཧྲིལ		getnext;

    /*
     * Get the first expression.
     */
    གལ་ཏེ (eval3(arg, rettv, evalarg) == FAIL)
	ཕྱིར FAIL;

    /*
     * Handle the  "||" operator.
     */
    p = eval_next_non_blank(*arg, evalarg, &getnext);
    གལ་ཏེ (p[0] == '|' && p[1] == '|')
    {
	evalarg_T   *evalarg_used = evalarg;
	evalarg_T   local_evalarg;
	ཧྲིལ	    evaluate;
	ཧྲིལ	    orig_flags;
	long	    result = FALSE;
	typval_T    var2;
	ཧྲིལ	    error = FALSE;
	ཧྲིལ	    vim9script = in_vim9script();

	གལ་ཏེ (evalarg == NULL)
	{
	    init_evalarg(&local_evalarg);
	    evalarg_used = &local_evalarg;
	}
	orig_flags = evalarg_used->eval_flags;
	evaluate = orig_flags & EVAL_EVALUATE;
	གལ་ཏེ (evaluate)
	{
	    གལ་ཏེ (vim9script)
		result = tv_get_bool_chk(rettv, &error);
	    དེ་མིན གལ་ཏེ (tv_get_number_chk(rettv, &error) != 0)
		result = TRUE;
	    clear_tv(rettv);
	    གལ་ཏེ (error)
		ཕྱིར FAIL;
	}

	/*
	 * Repeat until there is no following "||".
	 */
	སྐབས (p[0] == '|' && p[1] == '|')
	{
	    གལ་ཏེ (getnext)
		*arg = eval_next_line(*arg, evalarg_used);
	    དེ་མིན
	    {
		གལ་ཏེ (evaluate && vim9script && !VIM_ISWHITE(p[-1]))
		{
		    error_white_both(p, 2);
		    clear_tv(rettv);
		    ཕྱིར FAIL;
		}
		*arg = p;
	    }

	    /*
	     * Get the second variable.
	     */
	    གལ་ཏེ (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[2]))
	    {
		error_white_both(*arg, 2);
		clear_tv(rettv);
		ཕྱིར FAIL;
	    }
	    *arg = skipwhite_and_linebreak(*arg + 2, evalarg_used);
	    evalarg_used->eval_flags = !result ? orig_flags
					       : (orig_flags & ~EVAL_EVALUATE);
	    གལ་ཏེ (eval3(arg, &var2, evalarg_used) == FAIL)
		ཕྱིར FAIL;

	    /*
	     * Compute the result.
	     */
	    གལ་ཏེ (evaluate && !result)
	    {
		གལ་ཏེ (vim9script)
		    result = tv_get_bool_chk(&var2, &error);
		དེ་མིན གལ་ཏེ (tv_get_number_chk(&var2, &error) != 0)
		    result = TRUE;
		clear_tv(&var2);
		གལ་ཏེ (error)
		    ཕྱིར FAIL;
	    }
	    གལ་ཏེ (evaluate)
	    {
		གལ་ཏེ (vim9script)
		{
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_number = result ? VVAL_TRUE : VVAL_FALSE;
		}
		དེ་མིན
		{
		    rettv->v_type = VAR_NUMBER;
		    rettv->vval.v_number = result;
		}
	    }

	    p = eval_next_non_blank(*arg, evalarg_used, &getnext);
	}

	གལ་ཏེ (evalarg == NULL)
	    clear_evalarg(&local_evalarg, NULL);
	དེ་མིན
	    evalarg->eval_flags = orig_flags;
    }

    ཕྱིར OK;
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
    static ཧྲིལ
eval3(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    char_u	*p;
    ཧྲིལ		getnext;

    /*
     * Get the first expression.
     */
    གལ་ཏེ (eval4(arg, rettv, evalarg) == FAIL)
	ཕྱིར FAIL;

    /*
     * Handle the "&&" operator.
     */
    p = eval_next_non_blank(*arg, evalarg, &getnext);
    གལ་ཏེ (p[0] == '&' && p[1] == '&')
    {
	evalarg_T   *evalarg_used = evalarg;
	evalarg_T   local_evalarg;
	ཧྲིལ	    orig_flags;
	ཧྲིལ	    evaluate;
	long	    result = TRUE;
	typval_T    var2;
	ཧྲིལ	    error = FALSE;
	ཧྲིལ	    vim9script = in_vim9script();

	གལ་ཏེ (evalarg == NULL)
	{
	    init_evalarg(&local_evalarg);
	    evalarg_used = &local_evalarg;
	}
	orig_flags = evalarg_used->eval_flags;
	evaluate = orig_flags & EVAL_EVALUATE;
	གལ་ཏེ (evaluate)
	{
	    གལ་ཏེ (vim9script)
		result = tv_get_bool_chk(rettv, &error);
	    དེ་མིན གལ་ཏེ (tv_get_number_chk(rettv, &error) == 0)
		result = FALSE;
	    clear_tv(rettv);
	    གལ་ཏེ (error)
		ཕྱིར FAIL;
	}

	/*
	 * Repeat until there is no following "&&".
	 */
	སྐབས (p[0] == '&' && p[1] == '&')
	{
	    གལ་ཏེ (getnext)
		*arg = eval_next_line(*arg, evalarg_used);
	    དེ་མིན
	    {
		གལ་ཏེ (evaluate && vim9script && !VIM_ISWHITE(p[-1]))
		{
		    error_white_both(p, 2);
		    clear_tv(rettv);
		    ཕྱིར FAIL;
		}
		*arg = p;
	    }

	    /*
	     * Get the second variable.
	     */
	    གལ་ཏེ (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[2]))
	    {
		error_white_both(*arg, 2);
		clear_tv(rettv);
		ཕྱིར FAIL;
	    }
	    *arg = skipwhite_and_linebreak(*arg + 2, evalarg_used);
	    evalarg_used->eval_flags = result ? orig_flags
					      : (orig_flags & ~EVAL_EVALUATE);
	    CLEAR_FIELD(var2);
	    གལ་ཏེ (eval4(arg, &var2, evalarg_used) == FAIL)
		ཕྱིར FAIL;

	    /*
	     * Compute the result.
	     */
	    གལ་ཏེ (evaluate && result)
	    {
		གལ་ཏེ (vim9script)
		    result = tv_get_bool_chk(&var2, &error);
		དེ་མིན གལ་ཏེ (tv_get_number_chk(&var2, &error) == 0)
		    result = FALSE;
		clear_tv(&var2);
		གལ་ཏེ (error)
		    ཕྱིར FAIL;
	    }
	    གལ་ཏེ (evaluate)
	    {
		གལ་ཏེ (vim9script)
		{
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_number = result ? VVAL_TRUE : VVAL_FALSE;
		}
		དེ་མིན
		{
		    rettv->v_type = VAR_NUMBER;
		    rettv->vval.v_number = result;
		}
	    }

	    p = eval_next_non_blank(*arg, evalarg_used, &getnext);
	}

	གལ་ཏེ (evalarg == NULL)
	    clear_evalarg(&local_evalarg, NULL);
	དེ་མིན
	    evalarg->eval_flags = orig_flags;
    }

    ཕྱིར OK;
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
    static ཧྲིལ
eval4(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    char_u	*p;
    ཧྲིལ		getnext;
    exprtype_T	type = EXPR_UNKNOWN;
    ཧྲིལ		len = 2;
    ཧྲིལ		type_is = FALSE;

    /*
     * Get the first expression.
     */
    གལ་ཏེ (eval5(arg, rettv, evalarg) == FAIL)
	ཕྱིར FAIL;

    p = eval_next_non_blank(*arg, evalarg, &getnext);

    type = get_compare_type(p, &len, &type_is);

    /*
     * If there is a comparative operator, use it.
     */
    གལ་ཏེ (type != EXPR_UNKNOWN)
    {
	typval_T    var2;
	ཧྲིལ	    ic;
	ཧྲིལ	    vim9script = in_vim9script();
	ཧྲིལ	    evaluate = evalarg == NULL
				   ? 0 : (evalarg->eval_flags & EVAL_EVALUATE);
	long	    comp_lnum = SOURCING_LNUM;

	གལ་ཏེ (getnext)
	{
	    *arg = eval_next_line(*arg, evalarg);
	    p = *arg;
	}
	དེ་མིན གལ་ཏེ (evaluate && vim9script && !VIM_ISWHITE(**arg))
	{
	    error_white_both(*arg, len);
	    clear_tv(rettv);
	    ཕྱིར FAIL;
	}

	གལ་ཏེ (vim9script && type_is && (p[len] == '?' || p[len] == '#'))
	{
	    semsg(_(e_invalid_expression_str), p);
	    clear_tv(rettv);
	    ཕྱིར FAIL;
	}

	// extra question mark appended: ignore case
	གལ་ཏེ (p[len] == '?')
	{
	    ic = TRUE;
	    ++len;
	}
	// extra '#' appended: match case
	དེ་མིན གལ་ཏེ (p[len] == '#')
	{
	    ic = FALSE;
	    ++len;
	}
	// nothing appended: use 'ignorecase' if not in Vim script
	དེ་མིན
	    ic = vim9script ? FALSE : p_ic;

	/*
	 * Get the second variable.
	 */
	གལ་ཏེ (evaluate && vim9script && !IS_WHITE_OR_NUL(p[len]))
	{
	    error_white_both(p, len);
	    clear_tv(rettv);
	    ཕྱིར FAIL;
	}
	*arg = skipwhite_and_linebreak(p + len, evalarg);
	གལ་ཏེ (eval5(arg, &var2, evalarg) == FAIL)
	{
	    clear_tv(rettv);
	    ཕྱིར FAIL;
	}
	གལ་ཏེ (evaluate)
	{
	    ཧྲིལ ret;

	    // use the line of the comparison for messages
	    SOURCING_LNUM = comp_lnum;
	    གལ་ཏེ (vim9script && check_compare_types(type, rettv, &var2) == FAIL)
	    {
		ret = FAIL;
		clear_tv(rettv);
	    }
	    དེ་མིན
		ret = typval_compare(rettv, &var2, type, ic);
	    clear_tv(&var2);
	    ཕྱིར ret;
	}
    }

    ཕྱིར OK;
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

    གལ་ཏེ (b == NULL)
	ཕྱིར;

    len1 = blob_len(b1);
    len2 = blob_len(b2);
    totallen = len1 + len2;

    གལ་ཏེ (totallen > 0 && totallen <= INT_MAX
				    && ga_grow(&b->bv_ga, (ཧྲིལ)totallen) == OK)
    {
	གལ་ཏེ (len1 > 0)
	    mch_memmove((char_u *)b->bv_ga.ga_data,
		    (char_u *)b1->bv_ga.ga_data, (size_t)len1);
	གལ་ཏེ (len2 > 0)
	    mch_memmove((char_u *)b->bv_ga.ga_data + len1,
		    (char_u *)b2->bv_ga.ga_data, (size_t)len2);
	b->bv_ga.ga_len = (ཧྲིལ)totallen;
    }

    clear_tv(tv1);
    rettv_blob_set(tv1, b);
}

/*
 * Make a copy of list "tv1" and append list "tv2".
 */
    ཧྲིལ
eval_addlist(typval_T *tv1, typval_T *tv2)
{
    typval_T var3;

    // concatenate Lists
    གལ་ཏེ (list_concat(tv1->vval.v_list, tv2->vval.v_list, &var3) == FAIL)
    {
	clear_tv(tv1);
	clear_tv(tv2);
	ཕྱིར FAIL;
    }
    clear_tv(tv1);
    *tv1 = var3;
    ཕྱིར OK;
}

/*
 * Make a copy of tuple "tv1" and append tuple "tv2".
 */
    ཧྲིལ
eval_addtuple(typval_T *tv1, typval_T *tv2)
{
    ཧྲིལ		vim9script = in_vim9script();
    typval_T	var3;

    གལ་ཏེ (vim9script && tv1->vval.v_tuple != NULL && tv2->vval.v_tuple != NULL
	    && tv1->vval.v_tuple->tv_type != NULL
	    && tv2->vval.v_tuple->tv_type != NULL)
    {
	གལ་ཏེ (!check_tuples_addable(tv1->vval.v_tuple->tv_type,
						tv2->vval.v_tuple->tv_type))
	    ཕྱིར FAIL;
    }

    // concatenate tuples
    གལ་ཏེ (tuple_concat(tv1->vval.v_tuple, tv2->vval.v_tuple, &var3) == FAIL)
    {
	clear_tv(tv1);
	clear_tv(tv2);
	ཕྱིར FAIL;
    }
    clear_tv(tv1);
    *tv1 = var3;
    ཕྱིར OK;
}

/*
 * Left or right shift the number "tv1" by the number "tv2" and store the
 * result in "tv1".
 *
 * Return OK or FAIL.
 */
    static ཧྲིལ
eval_shift_number(typval_T *tv1, typval_T *tv2, ཧྲིལ shift_type)
{
    གལ་ཏེ (tv2->v_type != VAR_NUMBER || tv2->vval.v_number < 0)
    {
	// right operand should be a positive number
	གལ་ཏེ (tv2->v_type != VAR_NUMBER)
	    emsg(_(e_bitshift_ops_must_be_number));
	དེ་མིན
	    emsg(_(e_bitshift_ops_must_be_positive));
	clear_tv(tv1);
	clear_tv(tv2);
	ཕྱིར FAIL;
    }

    གལ་ཏེ (tv2->vval.v_number > MAX_LSHIFT_BITS)
	// shifting more bits than we have always results in zero
	tv1->vval.v_number = 0;
    དེ་མིན གལ་ཏེ (shift_type == EXPR_LSHIFT)
	tv1->vval.v_number =
	    (uvarnumber_T)tv1->vval.v_number << tv2->vval.v_number;
    དེ་མིན
	tv1->vval.v_number =
	    (uvarnumber_T)tv1->vval.v_number >> tv2->vval.v_number;

    ཕྱིར OK;
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
    static ཧྲིལ
eval5(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    /*
     * Get the first expression.
     */
    གལ་ཏེ (eval6(arg, rettv, evalarg) == FAIL)
	ཕྱིར FAIL;

    /*
     * Repeat computing, until no '<<' or '>>' is following.
     */
    ཆེད་དུ (;;)
    {
	char_u		*p;
	ཧྲིལ		getnext;
	exprtype_T	exprtype;
	ཧྲིལ		evaluate;
	typval_T	var2;
	ཧྲིལ		vim9script;

	p = eval_next_non_blank(*arg, evalarg, &getnext);
	གལ་ཏེ (p[0] == '<' && p[1] == '<')
	    exprtype = EXPR_LSHIFT;
	དེ་མིན གལ་ཏེ (p[0] == '>' && p[1] == '>')
	    exprtype = EXPR_RSHIFT;
	དེ་མིན
	    ཕྱིར OK;

	// Handle a bitwise left or right shift operator
	evaluate = evalarg == NULL ? 0 : (evalarg->eval_flags & EVAL_EVALUATE);
	གལ་ཏེ (evaluate && rettv->v_type != VAR_NUMBER)
	{
	    // left operand should be a number
	    emsg(_(e_bitshift_ops_must_be_number));
	    clear_tv(rettv);
	    ཕྱིར FAIL;
	}

	vim9script = in_vim9script();
	གལ་ཏེ (getnext)
	{
	    *arg = eval_next_line(*arg, evalarg);
	    p = *arg;
	}
	དེ་མིན གལ་ཏེ (evaluate && vim9script && !VIM_ISWHITE(**arg))
	{
	    error_white_both(*arg, 2);
	    clear_tv(rettv);
	    ཕྱིར FAIL;
	}

	/*
	 * Get the second variable.
	 */
	གལ་ཏེ (evaluate && vim9script && !IS_WHITE_OR_NUL(p[2]))
	{
	    error_white_both(p, 2);
	    clear_tv(rettv);
	    ཕྱིར FAIL;
	}
	*arg = skipwhite_and_linebreak(p + 2, evalarg);
	གལ་ཏེ (eval6(arg, &var2, evalarg) == FAIL)
	{
	    clear_tv(rettv);
	    ཕྱིར FAIL;
	}

	གལ་ཏེ (evaluate)
	{
	    གལ་ཏེ (eval_shift_number(rettv, &var2, exprtype) == FAIL)
		ཕྱིར FAIL;
	}

	clear_tv(&var2);
    }

    ཕྱིར OK;
}

/*
 * Concatenate strings "tv1" and "tv2" and store the result in "tv1".
 */
    static ཧྲིལ
eval_concat_str(typval_T *tv1, typval_T *tv2)
{
    char_u	buf1[NUMBUFLEN], buf2[NUMBUFLEN];
    char_u	*s1 = tv_get_string_buf(tv1, buf1);
    char_u	*s2 = NULL;
    char_u	*p;
    ཧྲིལ		vim9script = in_vim9script();

    གལ་ཏེ (vim9script && (tv2->v_type == VAR_VOID
		|| tv2->v_type == VAR_CHANNEL
		|| tv2->v_type == VAR_JOB))
	semsg(_(e_using_invalid_value_as_string_str),
		vartype_name(tv2->v_type));
    དེ་མིན གལ་ཏེ (vim9script && tv2->v_type == VAR_FLOAT)
    {
	vim_snprintf((var_na *)buf2, NUMBUFLEN, "%g",
		tv2->vval.v_float);
	s2 = buf2;
    }
    དེ་མིན
	s2 = tv_get_string_buf_chk(tv2, buf2);
    གལ་ཏེ (s2 == NULL)		// type error ?
    {
	clear_tv(tv1);
	clear_tv(tv2);
	ཕྱིར FAIL;
    }

    // When possible, grow the existing string in place to avoid alloc/free.
    གལ་ཏེ (grow_string_tv(tv1, s2) == OK)
	ཕྱིར OK;

    p = concat_str(s1, s2);
    clear_tv(tv1);
    tv1->v_type = VAR_STRING;
    tv1->vval.v_string = p;

    ཕྱིར OK;
}

/*
 * Add or subtract numbers "tv1" and "tv2" and store the result in "tv1".
 * The numbers can be whole numbers or floats.
 */
    static ཧྲིལ
eval_addsub_number(typval_T *tv1, typval_T *tv2, ཧྲིལ op)
{
    ཧྲིལ		error = FALSE;
    varnumber_T	n1, n2;
    float_T	f1 = 0, f2 = 0;

    གལ་ཏེ (tv1->v_type == VAR_FLOAT)
    {
	f1 = tv1->vval.v_float;
	n1 = 0;
    }
    དེ་མིན
    {
	n1 = tv_get_number_chk(tv1, &error);
	གལ་ཏེ (error)
	{
	    // This can only happen for "list + non-list" or
	    // "blob + non-blob".  For "non-list + ..." or
	    // "something - ...", we returned before evaluating the
	    // 2nd operand.
	    clear_tv(tv1);
	    clear_tv(tv2);
	    ཕྱིར FAIL;
	}
	གལ་ཏེ (tv2->v_type == VAR_FLOAT)
	    f1 = n1;
    }
    གལ་ཏེ (tv2->v_type == VAR_FLOAT)
    {
	f2 = tv2->vval.v_float;
	n2 = 0;
    }
    དེ་མིན
    {
	n2 = tv_get_number_chk(tv2, &error);
	གལ་ཏེ (error)
	{
	    clear_tv(tv1);
	    clear_tv(tv2);
	    ཕྱིར FAIL;
	}
	གལ་ཏེ (tv1->v_type == VAR_FLOAT)
	    f2 = n2;
    }
    clear_tv(tv1);

    // If there is a float on either side the result is a float.
    གལ་ཏེ (tv1->v_type == VAR_FLOAT || tv2->v_type == VAR_FLOAT)
    {
	གལ་ཏེ (op == '+')
	    f1 = f1 + f2;
	དེ་མིན
	    f1 = f1 - f2;
	tv1->v_type = VAR_FLOAT;
	tv1->vval.v_float = f1;
    }
    དེ་མིན
    {
	གལ་ཏེ (op == '+')
	    n1 = n1 + n2;
	དེ་མིན
	    n1 = n1 - n2;
	tv1->v_type = VAR_NUMBER;
	tv1->vval.v_number = n1;
    }

    ཕྱིར OK;
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
    static ཧྲིལ
eval6(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    /*
     * Get the first expression.
     */
    གལ་ཏེ (eval7(arg, rettv, evalarg, FALSE) == FAIL)
	ཕྱིར FAIL;

    /*
     * Repeat computing, until no '+', '-' or '.' is following.
     */
    ཆེད་དུ (;;)
    {
	ཧྲིལ	    evaluate;
	ཧྲིལ	    getnext;
	char_u	    *p;
	ཧྲིལ	    op;
	ཧྲིལ	    oplen;
	ཧྲིལ	    concat;
	typval_T    var2;
	ཧྲིལ	    vim9script = in_vim9script();
	long	    op_lnum = SOURCING_LNUM;

	// "." is only string concatenation when scriptversion is 1
	// "+=", "-=" and "..=" are assignments
	// "++" and "--" on the next line are a separate command.
	p = eval_next_non_blank(*arg, evalarg, &getnext);
	op = *p;
	concat = op == '.' && (*(p + 1) == '.' || in_old_script(2));
	གལ་ཏེ ((op != '+' && op != '-' && !concat) || p[1] == '='
					       || (p[1] == '.' && p[2] == '='))
	    bha_nga;
	གལ་ཏེ (getnext && (op == '+' || op == '-') && p[0] == p[1])
	    bha_nga;

	evaluate = evalarg == NULL ? 0 : (evalarg->eval_flags & EVAL_EVALUATE);
	oplen = (concat && p[1] == '.') ? 2 : 1;
	གལ་ཏེ (getnext)
	    *arg = eval_next_line(*arg, evalarg);
	དེ་མིན
	{
	    གལ་ཏེ (evaluate && vim9script && !VIM_ISWHITE(**arg))
	    {
		error_white_both(*arg, oplen);
		clear_tv(rettv);
		ཕྱིར FAIL;
	    }
	    *arg = p;
	}
	གལ་ཏེ ((op != '+' || (rettv->v_type != VAR_LIST
						&& rettv->v_type != VAR_TUPLE
						&& rettv->v_type != VAR_BLOB))
		&& (op == '.' || rettv->v_type != VAR_FLOAT)
		&& evaluate)
	{
	    ཧྲིལ		error = FALSE;

	    // For "list + ...", an illegal use of the first operand as
	    // a number cannot be determined before evaluating the 2nd
	    // operand: if this is also a list, all is ok.
	    // For "something . ...", "something - ..." or "non-list + ...",
	    // we know that the first operand needs to be a string or number
	    // without evaluating the 2nd operand.  So check before to avoid
	    // side effects after an error.
	    གལ་ཏེ (op != '.')
		tv_get_number_chk(rettv, &error);
	    གལ་ཏེ ((op == '.' && tv_get_string_chk(rettv) == NULL) || error)
	    {
		clear_tv(rettv);
		ཕྱིར FAIL;
	    }
	}

	/*
	 * Get the second variable.
	 */
	གལ་ཏེ (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[oplen]))
	{
	    error_white_both(*arg, oplen);
	    clear_tv(rettv);
	    ཕྱིར FAIL;
	}
	*arg = skipwhite_and_linebreak(*arg + oplen, evalarg);
	གལ་ཏེ (eval7(arg, &var2, evalarg, !vim9script && op == '.') == FAIL)
	{
	    clear_tv(rettv);
	    ཕྱིར FAIL;
	}

	གལ་ཏེ (evaluate)
	{
	    /*
	     * Compute the result.
	     */
	    // use the line of the operation for messages
	    SOURCING_LNUM = op_lnum;
	    གལ་ཏེ (op == '.')
	    {
		གལ་ཏེ (eval_concat_str(rettv, &var2) == FAIL)
		    ཕྱིར FAIL;
	    }
	    དེ་མིན གལ་ཏེ (op == '+' && rettv->v_type == VAR_BLOB
						   && var2.v_type == VAR_BLOB)
		eval_addblob(rettv, &var2);
	    དེ་མིན གལ་ཏེ (op == '+' && rettv->v_type == VAR_LIST
						   && var2.v_type == VAR_LIST)
	    {
		གལ་ཏེ (eval_addlist(rettv, &var2) == FAIL)
		    ཕྱིར FAIL;
	    }
	    དེ་མིན གལ་ཏེ (op == '+' && rettv->v_type == VAR_TUPLE
					   && var2.v_type == VAR_TUPLE)
	    {
		གལ་ཏེ (eval_addtuple(rettv, &var2) == FAIL)
		    ཕྱིར FAIL;
	    }
	    དེ་མིན
	    {
		གལ་ཏེ (eval_addsub_number(rettv, &var2, op) == FAIL)
		    ཕྱིར FAIL;
	    }
	    clear_tv(&var2);
	}
    }
    ཕྱིར OK;
}

/*
 * Multiply or divide or compute the modulo of numbers "tv1" and "tv2" and
 * store the result in "tv1".  The numbers can be whole numbers or floats.
 */
    static ཧྲིལ
eval_multdiv_number(typval_T *tv1, typval_T *tv2, ཧྲིལ op)
{
    varnumber_T	n1, n2;
    float_T	f1, f2;
    ཧྲིལ		error;
    ཧྲིལ		use_float = FALSE;

    f1 = 0;
    f2 = 0;
    error = FALSE;
    གལ་ཏེ (tv1->v_type == VAR_FLOAT)
    {
	f1 = tv1->vval.v_float;
	use_float = TRUE;
	n1 = 0;
    }
    དེ་མིན
	n1 = tv_get_number_chk(tv1, &error);
    clear_tv(tv1);
    གལ་ཏེ (error)
    {
	clear_tv(tv2);
	ཕྱིར FAIL;
    }

    གལ་ཏེ (tv2->v_type == VAR_FLOAT)
    {
	གལ་ཏེ (!use_float)
	{
	    f1 = n1;
	    use_float = TRUE;
	}
	f2 = tv2->vval.v_float;
	n2 = 0;
    }
    དེ་མིན
    {
	n2 = tv_get_number_chk(tv2, &error);
	clear_tv(tv2);
	གལ་ཏེ (error)
	    ཕྱིར FAIL;
	གལ་ཏེ (use_float)
	    f2 = n2;
    }

    /*
     * Compute the result.
     * When either side is a float the result is a float.
     */
    གལ་ཏེ (use_float)
    {
	གལ་ཏེ (op == '*')
	    f1 = f1 * f2;
	དེ་མིན གལ་ཏེ (op == '/')
	{
#ifdef VMS
	    // VMS crashes on divide by zero, work around it
	    གལ་ཏེ (f2 == 0.0)
	    {
		གལ་ཏེ (f1 == 0)
		    f1 = -1 * __F_FLT_MAX - 1L;   // similar to NaN
		དེ་མིན གལ་ཏེ (f1 < 0)
		    f1 = -1 * __F_FLT_MAX;
		དེ་མིན
		    f1 = __F_FLT_MAX;
	    }
	    དེ་མིན
		f1 = f1 / f2;
#དེ་མིན
	    // We rely on the floating point library to handle divide
	    // by zero to result in "inf" and not a crash.
	    f1 = f1 / f2;
#endif
	}
	དེ་མིན
	{
	    emsg(_(e_cannot_use_percent_with_float));
	    ཕྱིར FAIL;
	}
	tv1->v_type = VAR_FLOAT;
	tv1->vval.v_float = f1;
    }
    དེ་མིན
    {
	ཧྲིལ	    failed = FALSE;

	གལ་ཏེ (op == '*')
	    n1 = n1 * n2;
	དེ་མིན གལ་ཏེ (op == '/')
	    n1 = num_divide(n1, n2, &failed);
	དེ་མིན
	    n1 = num_modulus(n1, n2, &failed);
	གལ་ཏེ (failed)
	    ཕྱིར FAIL;

	tv1->v_type = VAR_NUMBER;
	tv1->vval.v_number = n1;
    }

    ཕྱིར OK;
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
    static ཧྲིལ
eval7(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    ཧྲིལ		want_string)  // after "." operator
{
    /*
     * Get the first expression.
     */
    གལ་ཏེ (eval8(arg, rettv, evalarg, want_string) == FAIL)
	ཕྱིར FAIL;

    /*
     * Repeat computing, until no '*', '/' or '%' is following.
     */
    ཆེད་དུ (;;)
    {
	ཧྲིལ	    evaluate;
	ཧྲིལ	    getnext;
	typval_T    var2;
	char_u	    *p;
	ཧྲིལ	    op;

	// "*=", "/=" and "%=" are assignments
	p = eval_next_non_blank(*arg, evalarg, &getnext);
	op = *p;
	གལ་ཏེ ((op != '*' && op != '/' && op != '%') || p[1] == '=')
	    bha_nga;

	evaluate = evalarg == NULL ? 0 : (evalarg->eval_flags & EVAL_EVALUATE);
	གལ་ཏེ (getnext)
	    *arg = eval_next_line(*arg, evalarg);
	དེ་མིན
	{
	    གལ་ཏེ (evaluate && in_vim9script() && !VIM_ISWHITE(**arg))
	    {
		error_white_both(*arg, 1);
		clear_tv(rettv);
		ཕྱིར FAIL;
	    }
	    *arg = p;
	}

	/*
	 * Get the second variable.
	 */
	གལ་ཏེ (evaluate && in_vim9script() && !IS_WHITE_OR_NUL((*arg)[1]))
	{
	    error_white_both(*arg, 1);
	    clear_tv(rettv);
	    ཕྱིར FAIL;
	}
	*arg = skipwhite_and_linebreak(*arg + 1, evalarg);
	གལ་ཏེ (eval8(arg, &var2, evalarg, FALSE) == FAIL)
	    ཕྱིར FAIL;

	གལ་ཏེ (evaluate)
	    // Compute the result.
	    གལ་ཏེ (eval_multdiv_number(rettv, &var2, op) == FAIL)
		ཕྱིར FAIL;
    }

    ཕྱིར OK;
}

/*
 * Handle seventh level expression:
 *	a type cast before a base level expression.
 * "arg" must point to the first non-white of the expression.
 * "arg" is advanced to just after the recognized expression.
 * Return OK or FAIL.
 */
    static ཧྲིལ
eval8(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    ཧྲིལ		want_string)	// after "." operator
{
    type_T	*want_type = NULL;
    garray_T	type_list;	    // list of pointers to allocated types
    ཧྲིལ		res;
    ཧྲིལ		evaluate = evalarg == NULL ? 0
				       : (evalarg->eval_flags & EVAL_EVALUATE);

    // Recognize <type> in Vim9 script only.
    གལ་ཏེ (in_vim9script() && **arg == '<' && eval_isnamec1((*arg)[1])
					     && STRNCMP(*arg, "<SNR>", 5) != 0)
    {
	++*arg;
	ga_init2(&type_list, parimaa_na(type_T *), 10);
	གལ་ཏེ (!evaluate)
	    // Only skip over the type, it may use a type variable that is only
	    // known when the expression is compiled, e.g. in a lambda.
	    *arg = skip_type(*arg, FALSE);
	དེ་མིན
	{
	    want_type = parse_type(arg, &type_list, NULL, NULL, TRUE);
	    གལ་ཏེ (want_type == NULL)
	    {
		clear_type_list(&type_list);
		ཕྱིར FAIL;
	    }
	}

	གལ་ཏེ (**arg != '>')
	{
	    གལ་ཏེ (*skipwhite(*arg) == '>')
		semsg(_(e_no_white_space_allowed_before_str_str), ">", *arg);
	    དེ་མིན
		emsg(_(e_missing_gt));
	    clear_type_list(&type_list);
	    ཕྱིར FAIL;
	}
	++*arg;
	*arg = skipwhite_and_linebreak(*arg, evalarg);
    }

    res = eval9(arg, rettv, evalarg, want_string);

    གལ་ཏེ (want_type != NULL)
    {
	གལ་ཏེ (evaluate && res == OK)
	{
	    type_T *actual = typval2type(rettv, get_copyID(), &type_list,
							       TVTT_DO_MEMBER);

	    གལ་ཏེ (!equal_type(want_type, actual, 0))
	    {
		གལ་ཏེ (want_type->tt_type == VAR_BOOL
					&& actual->tt_type != VAR_BOOL
					&& (actual->tt_flags & TTFLAG_BOOL_OK))
		{
		    ཧྲིལ n = tv2bool(rettv);

		    // can use "0" and "1" for boolean in some places
		    clear_tv(rettv);
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_number = n ? VVAL_TRUE : VVAL_FALSE;
		}
		དེ་མིན
		{
		    where_T where = WHERE_INIT;

		    res = check_type(want_type, actual, TRUE, where);
		}
	    }
	}
	clear_type_list(&type_list);
    }

    ཕྱིར res;
}

    ཧྲིལ
eval_leader(char_u **arg, ཧྲིལ vim9)
{
    char_u	*s = *arg;
    char_u	*p = *arg;

    སྐབས (*p == '!' || *p == '-' || *p == '+')
    {
	char_u *n = skipwhite(p + 1);

	// ++, --, -+ and +- are not accepted in Vim9 script
	གལ་ཏེ (vim9 && (*p == '-' || *p == '+') && (*n == '-' || *n == '+'))
	{
	    semsg(_(e_invalid_expression_str), s);
	    ཕྱིར FAIL;
	}
	p = n;
    }
    *arg = p;
    ཕྱིར OK;
}

/*
 * Check for a predefined value "true", "false" and "null.*".
 * Return OK when recognized.
 */
    ཧྲིལ
handle_predefined(char_u *s, ཧྲིལ len, typval_T *rettv)
{
    switch (len)
    {
	pak_sa 4: གལ་ཏེ (STRNCMP(s, "true", 4) == 0)
		{
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_number = VVAL_TRUE;
		    ཕྱིར OK;
		}
		གལ་ཏེ (STRNCMP(s, "null", 4) == 0)
		{
		    rettv->v_type = VAR_SPECIAL;
		    rettv->vval.v_number = VVAL_NULL;
		    ཕྱིར OK;
		}
		bha_nga;
	pak_sa 5: གལ་ཏེ (STRNCMP(s, "false", 5) == 0)
		{
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_number = VVAL_FALSE;
		    ཕྱིར OK;
		}
		bha_nga;
	pak_sa 8: གལ་ཏེ (STRNCMP(s, "null_job", 8) == 0)
		{
#ifdef FEAT_JOB_CHANNEL
		    rettv->v_type = VAR_JOB;
		    rettv->vval.v_job = NULL;
#དེ་མིན
		    rettv->v_type = VAR_SPECIAL;
		    rettv->vval.v_number = VVAL_NULL;
#endif
		    ཕྱིར OK;
		}
		bha_nga;
	pak_sa 9:
		གལ་ཏེ (STRNCMP(s, "null_", 5) != 0)
		    bha_nga;
		// null_list
		གལ་ཏེ (STRNCMP(s + 5, "list", 4) == 0)
		{
		    rettv->v_type = VAR_LIST;
		    rettv->vval.v_list = NULL;
		    ཕྱིར OK;
		}
		// null_dict
		གལ་ཏེ (STRNCMP(s + 5, "dict", 4) == 0)
		{
		    rettv->v_type = VAR_DICT;
		    rettv->vval.v_dict = NULL;
		    ཕྱིར OK;
		}
		// null_blob
		གལ་ཏེ (STRNCMP(s + 5, "blob", 4) == 0)
		{
		    rettv->v_type = VAR_BLOB;
		    rettv->vval.v_blob = NULL;
		    ཕྱིར OK;
		}
		bha_nga;
	pak_sa 10:
		གལ་ཏེ (STRNCMP(s, "null_", 5) != 0)
		    bha_nga;
		// null_class
		གལ་ཏེ (STRNCMP(s + 5, "class", 5) == 0)
		{
		    rettv->v_type = VAR_CLASS;
		    rettv->vval.v_class = NULL;
		    ཕྱིར OK;
		}
		གལ་ཏེ (STRNCMP(s + 5, "tuple", 5) == 0)
		{
		    rettv->v_type = VAR_TUPLE;
		    rettv->vval.v_tuple = NULL;
		    ཕྱིར OK;
		}
		bha_nga;
	pak_sa 11: གལ་ཏེ (STRNCMP(s, "null_string", 11) == 0)
		{
		    rettv->v_type = VAR_STRING;
		    rettv->vval.v_string = NULL;
		    ཕྱིར OK;
		}
		གལ་ཏེ (STRNCMP(s, "null_object", 11) == 0)
		{
		    rettv->v_type = VAR_OBJECT;
		    rettv->vval.v_object = NULL;
		    ཕྱིར OK;
		}
		bha_nga;
	pak_sa 12:
		གལ་ཏེ (STRNCMP(s, "null_channel", 12) == 0)
		{
#ifdef FEAT_JOB_CHANNEL
		    rettv->v_type = VAR_CHANNEL;
		    rettv->vval.v_channel = NULL;
#དེ་མིན
		    rettv->v_type = VAR_SPECIAL;
		    rettv->vval.v_number = VVAL_NULL;
#endif
		    ཕྱིར OK;
		}
		གལ་ཏེ (STRNCMP(s, "null_partial", 12) == 0)
		{
		    rettv->v_type = VAR_PARTIAL;
		    rettv->vval.v_partial = NULL;
		    ཕྱིར OK;
		}
		bha_nga;
	pak_sa 13: གལ་ཏེ (STRNCMP(s, "null_function", 13) == 0)
		{
		    rettv->v_type = VAR_FUNC;
		    rettv->vval.v_string = NULL;
		    ཕྱིར OK;
		}
		bha_nga;
    }
    ཕྱིར FAIL;
}

/*
 * Handle register contents: @r.
 */
    static void
eval9_reg_contents(
    char_u	**arg,
    typval_T	*rettv,
    ཧྲིལ		evaluate)
{
    ཧྲིལ		vim9script = in_vim9script();

    ++*arg;	// skip '@'
    གལ་ཏེ (evaluate)
    {
	གལ་ཏེ (vim9script && IS_WHITE_OR_NUL(**arg))
	    semsg(_(e_syntax_error_at_str), *arg);
	དེ་མིན གལ་ཏེ (vim9script && !valid_yank_reg(**arg, FALSE))
	    emsg_invreg(**arg);
	དེ་མིན
	{
	    rettv->v_type = VAR_STRING;
	    rettv->vval.v_string = get_reg_contents(**arg,
		    GREG_EXPR_SRC);
	}
    }
    གལ་ཏེ (**arg != NUL)
	++*arg;
}

/*
 * Handle a nested expression: (expression) or lambda: (arg) => expr
 */
    static ཧྲིལ
eval9_nested_expr(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    ཧྲིལ		evaluate)
{
    ཧྲིལ		ret = NOTDONE;
    ཧྲིལ		vim9script = in_vim9script();

    གལ་ཏེ (vim9script)
    {
	ret = get_lambda_tv(arg, rettv, TRUE, evalarg, NULL);
	གལ་ཏེ (ret == OK && evaluate)
	{
	    ufunc_T *ufunc = rettv->vval.v_partial->pt_func;

	    // Compile it here to get the return type.  The return
	    // type is optional, when it's missing use t_unknown.
	    // This is recognized in compile_return().
	    གལ་ཏེ (ufunc->uf_ret_type->tt_type == VAR_VOID)
		ufunc->uf_ret_type = &t_unknown;
	    གལ་ཏེ (compile_def_function(ufunc, FALSE,
				get_compile_type(ufunc), NULL) == FAIL)
	    {
		clear_tv(rettv);
		ret = FAIL;
	    }
	}
    }
    གལ་ཏེ (ret == NOTDONE)
    {
	*arg = skipwhite_and_linebreak(*arg + 1, evalarg);
	གལ་ཏེ (**arg == ')')
	    // empty tuple
	    ret = eval_tuple(arg, rettv, evalarg, TRUE);
	དེ་མིན
	{
	    ret = eval1(arg, rettv, evalarg);	// recursive!
	    གལ་ཏེ (ret != OK)
		ཕྱིར ret;

	    *arg = skipwhite_and_linebreak(*arg, evalarg);

	    གལ་ཏེ (**arg == ',')
		// tuple
		ret = eval_tuple(arg, rettv, evalarg, TRUE);
	    དེ་མིན གལ་ཏེ (**arg == ')')
		++*arg;
	    དེ་མིན གལ་ཏེ (ret == OK)
	    {
		emsg(_(e_missing_closing_paren));
		clear_tv(rettv);
		ret = FAIL;
	    }
	}
    }

    ཕྱིར ret;
}

/*
* Handle be a variable or function name.
* Can also be a curly-braces kind of name: {expr}.
*/
    static ཧྲིལ
eval9_var_func_name(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    ཧྲིལ		evaluate,
    char_u	**name_start)
{
    char_u	*s;
    ཧྲིལ		len;
    char_u	*alias;
    ཧྲིལ		ret = OK;
    ཧྲིལ		vim9script = in_vim9script();

    s = *arg;
    len = get_name_len(arg, &alias, evaluate, TRUE);
    གལ་ཏེ (alias != NULL)
	s = alias;

    གལ་ཏེ (len <= 0)
	ret = FAIL;
    དེ་མིན
    {
	ཧྲིལ	flags = evalarg == NULL ? 0 : evalarg->eval_flags;

	གལ་ཏེ (evaluate && vim9script && len == 1 && *s == '_')
	{
	    emsg(_(e_cannot_use_underscore_here));
	    ret = FAIL;
	}
	དེ་མིན གལ་ཏེ (evaluate && vim9script && len > 2
						&& s[0] == 's' && s[1] == ':')
	{
	    semsg(_(e_cannot_use_s_colon_in_vim9_script_str), s);
	    ret = FAIL;
	}
	དེ་མིན གལ་ཏེ ((vim9script ? **arg : *skipwhite(*arg)) == '('
				|| (vim9script && generic_func_call(arg)))
	{
	    // "name(..."  recursive!
	    *arg = skipwhite(*arg);
	    ret = eval_func(arg, evalarg, s, len, rettv, flags, NULL);
	}
	དེ་མིན གལ་ཏེ (evaluate)
	{
	    // get the value of "true", "false", etc. or a variable
	    ret = FAIL;
	    གལ་ཏེ (vim9script)
		ret = handle_predefined(s, len, rettv);
	    གལ་ཏེ (ret == FAIL)
	    {
		*name_start = s;
		ret = eval_variable(s, len, 0, rettv, NULL,
					EVAL_VAR_VERBOSE + EVAL_VAR_IMPORT);

		// skip the generic function arguments (if present)
		// they are already processed by eval_variable
		གལ་ཏེ (ret == OK && vim9script && **arg == '<'
						&& rettv->v_type == VAR_FUNC)
		    ret = skip_generic_func_type_args(arg);
	    }
	}
	དེ་མིན
	{
	    // skip the name
	    check_vars(s, len);

	    // Skip the type arguments of a generic function, its name starts
	    // with an uppercase letter.
	    གལ་ཏེ (vim9script && **arg == '<' && ASCII_ISUPPER(*s))
		skip_generic_func_type_args(arg);
	    ret = OK;
	}
    }
    vim_free(alias);

    ཕྱིར ret;
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
    static ཧྲིལ
eval9(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    ཧྲིལ		want_string)	// after "." operator
{
    ཧྲིལ		evaluate = evalarg != NULL
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    char_u	*name_start = NULL;
    char_u	*start_leader, *end_leader;
    ཧྲིལ		ret = OK;
    static ཧྲིལ	recurse = 0;
    ཧྲིལ		vim9script = in_vim9script();

    /*
     * Initialise variable so that clear_tv() can't mistake this for a
     * string and free a string that isn't there.
     */
    rettv->v_type = VAR_UNKNOWN;

    /*
     * Skip '!', '-' and '+' characters.  They are handled later.
     */
    start_leader = *arg;
    གལ་ཏེ (eval_leader(arg, vim9script) == FAIL)
	ཕྱིར FAIL;
    end_leader = *arg;

    གལ་ཏེ (**arg == '.' && (!SAFE_isdigit(*(*arg + 1)) || in_old_script(2)))
    {
	semsg(_(e_invalid_expression_str), *arg);
	++*arg;
	ཕྱིར FAIL;
    }

    // Limit recursion to 1000 levels.  At least at 10000 we run out of stack
    // and crash.  With MSVC the stack is smaller.
    གལ་ཏེ (recurse ==
#ifdef _MSC_VER
		    300
#དེ་མིན
		    1000
#endif
		    )
    {
	semsg(_(e_expression_too_recursive_str), *arg);
	ཕྱིར FAIL;
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
		གལ་ཏེ (ret == OK && evaluate && end_leader > start_leader
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
    pak_sa '{':	གལ་ཏེ (vim9script)
		    ret = NOTDONE;
		དེ་མིན
		    ret = get_lambda_tv(arg, rettv, vim9script, evalarg, NULL);
		གལ་ཏེ (ret == NOTDONE)
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
    pak_sa '$':	གལ་ཏེ ((*arg)[1] == '"' || (*arg)[1] == '\'')
		    ret = eval_interp_string(arg, rettv, evaluate);
		དེ་མིན
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

    གལ་ཏེ (ret == NOTDONE)
    {
	/*
	 * Must be a variable or function name.
	 * Can also be a curly-braces kind of name: {expr}.
	 */
	ret = eval9_var_func_name(arg, rettv, evalarg, evaluate, &name_start);
    }

    // Handle following '[', '(' and '.' for expr[expr], expr.name,
    // expr(expr), expr->name(expr)
    གལ་ཏེ (ret == OK)
	ret = handle_subscript(arg, name_start, rettv, evalarg, evaluate);

    /*
     * Apply logical NOT and unary '-', from right to left, ignore '+'.
     */
    གལ་ཏེ (ret == OK && evaluate && end_leader > start_leader)
	ret = eval9_leader(rettv, FALSE, start_leader, &end_leader);

    --recurse;
    ཕྱིར ret;
}

/*
 * Apply the leading "!" and "-" before an eval9 expression to "rettv".
 * When "numeric_only" is TRUE only handle "+" and "-".
 * Adjusts "end_leaderp" until it is at "start_leader".
 */
    static ཧྲིལ
eval9_leader(
	typval_T    *rettv,
	ཧྲིལ	    numeric_only,
	char_u	    *start_leader,
	char_u	    **end_leaderp)
{
    char_u	*end_leader = *end_leaderp;
    ཧྲིལ		ret = OK;
    ཧྲིལ		error = FALSE;
    varnumber_T val = 0;
    vartype_T	type = rettv->v_type;
    ཧྲིལ		vim9script = in_vim9script();
    float_T	    f = 0.0;

    གལ་ཏེ (rettv->v_type == VAR_FLOAT)
	f = rettv->vval.v_float;
    དེ་མིན
    {
	སྐབས (VIM_ISWHITE(end_leader[-1]))
	    --end_leader;
	གལ་ཏེ (vim9script && end_leader[-1] == '!')
	{
	    // Is this typeval supported with the ! operator?
	    གལ་ཏེ (check_typval_is_value(rettv) == FAIL)
	    {
		clear_tv(rettv);
		ཕྱིར FAIL;
	    }
	    val = tv2bool(rettv);
	}
	དེ་མིན
	    val = tv_get_number_chk(rettv, &error);
    }
    གལ་ཏེ (error)
    {
	clear_tv(rettv);
	ret = FAIL;
    }
    དེ་མིན
    {
	སྐབས (end_leader > start_leader)
	{
	    --end_leader;
	    གལ་ཏེ (*end_leader == '!')
	    {
		གལ་ཏེ (numeric_only)
		{
		    ++end_leader;
		    bha_nga;
		}
		གལ་ཏེ (rettv->v_type == VAR_FLOAT)
		{
		    rettv->v_type = VAR_BOOL;
		    val = f == 0.0 ? VVAL_TRUE : VVAL_FALSE;
		    type = VAR_BOOL;
		}
		དེ་མིན
		{
		    val = !val;
		    type = VAR_BOOL;
		}
	    }
	    དེ་མིན གལ་ཏེ (*end_leader == '-')
	    {
		གལ་ཏེ (rettv->v_type == VAR_FLOAT)
		    f = -f;
		དེ་མིན
		{
		    val = -val;
		    type = VAR_NUMBER;
		}
	    }
	}
	གལ་ཏེ (rettv->v_type == VAR_FLOAT)
	{
	    clear_tv(rettv);
	    rettv->vval.v_float = f;
	}
	དེ་མིན
	{
	    clear_tv(rettv);
	    གལ་ཏེ (vim9script)
		rettv->v_type = type;
	    དེ་མིན
		rettv->v_type = VAR_NUMBER;
	    rettv->vval.v_number = val;
	}
    }
    *end_leaderp = end_leader;
    ཕྱིར ret;
}

/*
 * Call the function referred to in "rettv".
 */
    static ཧྲིལ
call_func_rettv(
	char_u	    **arg,
	evalarg_T   *evalarg,
	typval_T    *rettv,
	ཧྲིལ	    evaluate,
	dict_T	    *selfdict,
	typval_T    *basetv)
{
    partial_T	*pt = NULL;
    funcexe_T	funcexe;
    typval_T	functv;
    char_u	*s;
    ཧྲིལ		ret;

    // need to copy the funcref so that we can clear rettv
    གལ་ཏེ (evaluate)
    {
	functv = *rettv;
	rettv->v_type = VAR_UNKNOWN;

	// Invoke the function.  Recursive!
	གལ་ཏེ (functv.v_type == VAR_PARTIAL)
	{
	    pt = functv.vval.v_partial;
	    s = partial_name(pt);
	}
	དེ་མིན
	{
	    s = functv.vval.v_string;
	    གལ་ཏེ (s == NULL || *s == NUL)
	    {
		emsg(_(e_empty_function_name));
		ret = FAIL;
		goto theend;
	    }
	}
    }
    དེ་མིན
	s = (char_u *)"";

    CLEAR_FIELD(funcexe);
    funcexe.fe_firstline = curwin->w_cursor.lnum;
    funcexe.fe_lastline = curwin->w_cursor.lnum;
    funcexe.fe_evaluate = evaluate;
    funcexe.fe_partial = pt;
    གལ་ཏེ (pt != NULL)
    {
	funcexe.fe_object = pt->pt_obj;
	གལ་ཏེ (funcexe.fe_object != NULL)
	    ++funcexe.fe_object->obj_refcount;
    }
    funcexe.fe_selfdict = selfdict;
    funcexe.fe_basetv = basetv;
    གལ་ཏེ (evalarg != NULL)
	funcexe.fe_cctx = evalarg->eval_cctx;
    ret = get_func_tv(s, -1, rettv, arg, evalarg, &funcexe);

theend:
    // Clear the funcref afterwards, so that deleting it while
    // evaluating the arguments is possible (see test55).
    གལ་ཏེ (evaluate)
	clear_tv(&functv);

    ཕྱིར ret;
}

/*
 * Evaluate "->method()".
 * "*arg" points to "method".
 * Returns FAIL or OK. "*arg" is advanced to after the ')'.
 */
    static ཧྲིལ
eval_lambda(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    ཧྲིལ		verbose)	// give error messages
{
    ཧྲིལ		evaluate = evalarg != NULL
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    typval_T	base = *rettv;
    ཧྲིལ		ret;

    rettv->v_type = VAR_UNKNOWN;

    གལ་ཏེ (**arg == '{')
    {
	// ->{lambda}()
	ret = get_lambda_tv(arg, rettv, FALSE, evalarg, NULL);
    }
    དེ་མིན
    {
	// ->(lambda)()
	++*arg;
	ret = eval1(arg, rettv, evalarg);
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	གལ་ཏེ (**arg != ')')
	{
	    emsg(_(e_missing_closing_paren));
	    ཕྱིར FAIL;
	}
	གལ་ཏེ (rettv->v_type != VAR_STRING && rettv->v_type != VAR_FUNC
					       && rettv->v_type != VAR_PARTIAL)
	{
	    emsg(_(e_string_or_function_required_for_arrow_parens_expr));
	    ཕྱིར FAIL;
	}
	++*arg;
    }
    གལ་ཏེ (ret != OK)
	ཕྱིར FAIL;

    གལ་ཏེ (**arg != '(')
    {
	གལ་ཏེ (verbose)
	{
	    གལ་ཏེ (*skipwhite(*arg) == '(')
		emsg(_(e_no_white_space_allowed_before_parenthesis));
	    དེ་མིན
		semsg(_(e_missing_parenthesis_str), "lambda");
	}
	clear_tv(rettv);
	ret = FAIL;
    }
    དེ་མིན
	ret = call_func_rettv(arg, evalarg, rettv, evaluate, NULL, &base);

    // Clear the funcref afterwards, so that deleting it while
    // evaluating the arguments is possible (see test55).
    གལ་ཏེ (evaluate)
	clear_tv(&base);

    ཕྱིར ret;
}

/*
 * Evaluate "->method()".
 * "*arg" points to "method".
 * Returns FAIL or OK. "*arg" is advanced to after the ')'.
 */
    static ཧྲིལ
eval_method(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    ཧྲིལ		verbose)	// give error messages
{
    char_u	*name;
    long	len;
    char_u	*alias;
    char_u	*tofree = NULL;
    typval_T	base = *rettv;
    ཧྲིལ		ret = OK;
    ཧྲིལ		evaluate = evalarg != NULL
				      && (evalarg->eval_flags & EVAL_EVALUATE);

    rettv->v_type = VAR_UNKNOWN;

    name = *arg;
    len = get_name_len(arg, &alias, evaluate, evaluate);
    གལ་ཏེ (alias != NULL)
	name = alias;

    གལ་ཏེ (len <= 0)
    {
	གལ་ཏེ (verbose)
	    emsg(_(e_missing_name_after_method));
	ret = FAIL;
    }
    དེ་མིན
    {
	char_u *paren;

	// If there is no "(" immediately following, but there is further on,
	// it can be "import.Func()", "dict.Func()", "list[nr]", etc.
	// Does not handle anything where "(" is part of the expression.
	*arg = skipwhite(*arg);

	གལ་ཏེ (**arg != '(' && alias == NULL
				    && (paren = vim_strchr(*arg, '(')) != NULL)
	{
	    *arg = name;

	    // Truncate the name at the "(".  Avoid trying to get another line
	    // by making "getline" NULL.
	    *paren = NUL;
	    char_u	*(*getline)(ཧྲིལ, void *, ཧྲིལ, getline_opt_T) = NULL;
	    གལ་ཏེ (evalarg != NULL)
	    {
		getline = evalarg->eval_getline;
		evalarg->eval_getline = NULL;
	    }

	    char_u *deref = deref_function_name(arg, &tofree, evalarg, verbose);
	    གལ་ཏེ (deref == NULL)
	    {
		*arg = name + len;
		ret = FAIL;
	    }
	    དེ་མིན
	    {
		name = deref;
		len = (long)STRLEN(name);
	    }

	    *paren = '(';
	    གལ་ཏེ (getline != NULL)
		evalarg->eval_getline = getline;
	}

	གལ་ཏེ (ret == OK)
	{
	    *arg = skipwhite(*arg);

	    གལ་ཏེ (**arg != '(')
	    {
		གལ་ཏེ (verbose)
		    semsg(_(e_missing_parenthesis_str), name);
		ret = FAIL;
	    }
	    དེ་མིན གལ་ཏེ (VIM_ISWHITE((*arg)[-1]))
	    {
		གལ་ཏེ (verbose)
		    emsg(_(e_no_white_space_allowed_before_parenthesis));
		ret = FAIL;
	    }
	    དེ་མིན
		ret = eval_func(arg, evalarg, name, len, rettv,
					  evaluate ? EVAL_EVALUATE : 0, &base);
	}
    }

    // Clear the funcref afterwards, so that deleting it while
    // evaluating the arguments is possible (see test55).
    གལ་ཏེ (evaluate)
	clear_tv(&base);
    vim_free(tofree);

    གལ་ཏེ (alias != NULL)
	vim_free(alias);

    ཕྱིར ret;
}

/*
 * Evaluate an "[expr]" or "[expr:expr]" index.  Also "dict.key".
 * "*arg" points to the '[' or '.'.
 * Returns FAIL or OK. "*arg" is advanced to after the ']'.
 */
    static ཧྲིལ
eval_index(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    ཧྲིལ		verbose)	// give error messages
{
    ཧྲིལ		evaluate = evalarg != NULL
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    ཧྲིལ		empty1 = FALSE, empty2 = FALSE;
    typval_T	var1, var2;
    ཧྲིལ		range = FALSE;
    char_u	*key = NULL;
    ཧྲིལ		keylen = -1;
    ཧྲིལ		vim9script = in_vim9script();

    གལ་ཏེ (check_can_index(rettv, evaluate, verbose) == FAIL)
	ཕྱིར FAIL;

    init_tv(&var1);
    init_tv(&var2);
    གལ་ཏེ (**arg == '.')
    {
	/*
	 * dict.name
	 */
	key = *arg + 1;
	ཆེད་དུ (keylen = 0; eval_isdictc(key[keylen]); ++keylen)
	    ;
	གལ་ཏེ (keylen == 0)
	    ཕྱིར FAIL;
	གལ་ཏེ (vim9script && key[keylen] == '<')
	{
	    // skip generic type arguments
	    char_u	*p = &key[keylen];

	    གལ་ཏེ (skip_generic_func_type_args(&p) == FAIL)
		ཕྱིར FAIL;
	    keylen = p - key;
	}
	*arg = key + keylen;
    }
    དེ་མིན
    {
	/*
	 * something[idx]
	 *
	 * Get the (first) variable from inside the [].
	 */
	*arg = skipwhite_and_linebreak(*arg + 1, evalarg);
	གལ་ཏེ (**arg == ':')
	    empty1 = TRUE;
	དེ་མིན གལ་ཏེ (eval1(arg, &var1, evalarg) == FAIL)	// recursive!
	    ཕྱིར FAIL;
	དེ་མིན གལ་ཏེ (vim9script && **arg == ':')
	{
	    semsg(_(e_white_space_required_before_and_after_str_at_str),
								    ":", *arg);
	    clear_tv(&var1);
	    ཕྱིར FAIL;
	}
	དེ་མིན གལ་ཏེ (evaluate)
	{
	    ཧྲིལ error = FALSE;

	    // allow for indexing with float
	    གལ་ཏེ (vim9script && rettv->v_type == VAR_DICT
						   && var1.v_type == VAR_FLOAT)
	    {
		var1.vval.v_string = typval_tostring(&var1, TRUE);
		var1.v_type = VAR_STRING;
	    }

	    གལ་ཏེ (vim9script && (rettv->v_type == VAR_LIST
						|| rettv->v_type == VAR_TUPLE))
		tv_get_number_chk(&var1, &error);
	    དེ་མིན
		error = tv_get_string_chk(&var1) == NULL;
	    གལ་ཏེ (error)
	    {
		// not a number or string
		clear_tv(&var1);
		ཕྱིར FAIL;
	    }
	}

	/*
	 * Get the second variable from inside the [:].
	 */
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	གལ་ཏེ (**arg == ':')
	{
	    range = TRUE;
	    ++*arg;
	    གལ་ཏེ (vim9script && !IS_WHITE_OR_NUL(**arg) && **arg != ']')
	    {
		semsg(_(e_white_space_required_before_and_after_str_at_str),
								":", *arg - 1);
		གལ་ཏེ (!empty1)
		    clear_tv(&var1);
		ཕྱིར FAIL;
	    }
	    *arg = skipwhite_and_linebreak(*arg, evalarg);
	    གལ་ཏེ (**arg == ']')
		empty2 = TRUE;
	    དེ་མིན གལ་ཏེ (eval1(arg, &var2, evalarg) == FAIL)	// recursive!
	    {
		གལ་ཏེ (!empty1)
		    clear_tv(&var1);
		ཕྱིར FAIL;
	    }
	    དེ་མིན གལ་ཏེ (evaluate && tv_get_string_chk(&var2) == NULL)
	    {
		// not a number or string
		གལ་ཏེ (!empty1)
		    clear_tv(&var1);
		clear_tv(&var2);
		ཕྱིར FAIL;
	    }
	}

	// Check for the ']'.
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	གལ་ཏེ (**arg != ']')
	{
	    གལ་ཏེ (verbose)
		emsg(_(e_missing_closing_square_brace));
	    clear_tv(&var1);
	    གལ་ཏེ (range)
		clear_tv(&var2);
	    ཕྱིར FAIL;
	}
	*arg = *arg + 1;	// skip over the ']'
    }

    གལ་ཏེ (evaluate)
    {
	ཧྲིལ res = eval_index_inner(rettv, range,
		empty1 ? NULL : &var1, empty2 ? NULL : &var2, FALSE,
		key, keylen, verbose);

	གལ་ཏེ (!empty1)
	    clear_tv(&var1);
	གལ་ཏེ (range)
	    clear_tv(&var2);
	ཕྱིར res;
    }
    ཕྱིར OK;
}

/*
 * Check if "rettv" can have an [index] or [sli:ce]
 */
    ཧྲིལ
check_can_index(typval_T *rettv, ཧྲིལ evaluate, ཧྲིལ verbose)
{
    switch (rettv->v_type)
    {
	pak_sa VAR_FUNC:
	pak_sa VAR_PARTIAL:
	    གལ་ཏེ (verbose)
		emsg(_(e_cannot_index_a_funcref));
	    ཕྱིར FAIL;
	pak_sa VAR_FLOAT:
	    གལ་ཏེ (verbose)
		emsg(_(e_using_float_as_string));
	    ཕྱིར FAIL;
	pak_sa VAR_BOOL:
	pak_sa VAR_SPECIAL:
	pak_sa VAR_JOB:
	pak_sa VAR_CHANNEL:
	pak_sa VAR_INSTR:
	pak_sa VAR_OBJECT:
	    གལ་ཏེ (verbose)
		emsg(_(e_cannot_index_special_variable));
	    ཕྱིར FAIL;
	pak_sa VAR_CLASS:
	pak_sa VAR_TYPEALIAS:
	    གལ་ཏེ (verbose)
		check_typval_is_value(rettv);
	    ཕྱིར FAIL;
	pak_sa VAR_UNKNOWN:
	pak_sa VAR_ANY:
	pak_sa VAR_VOID:
	    གལ་ཏེ (evaluate)
	    {
		emsg(_(e_cannot_index_special_variable));
		ཕྱིར FAIL;
	    }
	    // FALLTHROUGH

	pak_sa VAR_STRING:
	pak_sa VAR_LIST:
	pak_sa VAR_TUPLE:
	pak_sa VAR_DICT:
	pak_sa VAR_BLOB:
	    bha_nga;
	pak_sa VAR_NUMBER:
	    གལ་ཏེ (in_vim9script())
		emsg(_(e_cannot_index_number));
	    bha_nga;
    }
    ཕྱིར OK;
}

/*
 * Apply index or range to "rettv".
 * "var1" is the first index, NULL for [:expr].
 * "var2" is the second index, NULL for [expr] and [expr: ]
 * "exclusive" is TRUE for slice(): second index is exclusive, use character
 * index for string.
 * Alternatively, "key" is not NULL, then key[keylen] is the dict index.
 */
    ཧྲིལ
eval_index_inner(
	typval_T    *rettv,
	ཧྲིལ	    is_range,
	typval_T    *var1,
	typval_T    *var2,
	ཧྲིལ	    exclusive,
	char_u	    *key,
	ཧྲིལ	    keylen,
	ཧྲིལ	    verbose)
{
    varnumber_T	    n1, n2 = 0;
    long	    len;

    n1 = 0;
    གལ་ཏེ (var1 != NULL && rettv->v_type != VAR_DICT)
	n1 = tv_get_number(var1);

    གལ་ཏེ (is_range)
    {
	གལ་ཏེ (rettv->v_type == VAR_DICT)
	{
	    གལ་ཏེ (verbose)
		emsg(_(e_cannot_slice_dictionary));
	    ཕྱིར FAIL;
	}
	གལ་ཏེ (var2 != NULL)
	    n2 = tv_get_number(var2);
	དེ་མིན
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
		གལ་ཏེ (in_vim9script() || exclusive)
		{
		    གལ་ཏེ (is_range)
			s = string_slice(s, n1, n2, exclusive);
		    དེ་མིན
			s = char_from_string(s, n1);
		}
		དེ་མིན གལ་ཏེ (is_range)
		{
		    // The resulting variable is a substring.  If the indexes
		    // are out of range the result is empty.
		    གལ་ཏེ (n1 < 0)
		    {
			n1 = len + n1;
			གལ་ཏེ (n1 < 0)
			    n1 = 0;
		    }
		    གལ་ཏེ (n2 < 0)
			n2 = len + n2;
		    དེ་མིན གལ་ཏེ (n2 >= len)
			n2 = len;
		    གལ་ཏེ (n1 >= len || n2 < 0 || n1 > n2)
			s = NULL;
		    དེ་མིན
			s = vim_strnsave(s + n1, n2 - n1 + 1);
		}
		དེ་མིན
		{
		    // The resulting variable is a string of a single
		    // character.  If the index is too big or negative the
		    // result is empty.
		    གལ་ཏེ (n1 >= len || n1 < 0)
			s = NULL;
		    དེ་མིན
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
	    གལ་ཏེ (var1 == NULL)
		n1 = 0;
	    གལ་ཏེ (var2 == NULL)
		n2 = VARNUM_MAX;
	    གལ་ཏེ (list_slice_or_index(rettv->vval.v_list,
			  is_range, n1, n2, exclusive, rettv, verbose) == FAIL)
		ཕྱིར FAIL;
	    bha_nga;

	pak_sa VAR_TUPLE:
	    གལ་ཏེ (var1 == NULL)
		n1 = 0;
	    གལ་ཏེ (var2 == NULL)
		n2 = VARNUM_MAX;
	    གལ་ཏེ (tuple_slice_or_index(rettv->vval.v_tuple,
			  is_range, n1, n2, exclusive, rettv, verbose) == FAIL)
		ཕྱིར FAIL;
	    bha_nga;

	pak_sa VAR_DICT:
	    {
		dictitem_T	*item;
		typval_T	tmp;

		གལ་ཏེ (key == NULL)
		{
		    key = tv_get_string_chk(var1);
		    གལ་ཏེ (key == NULL)
			ཕྱིར FAIL;
		}

		item = dict_find(rettv->vval.v_dict, key, keylen);

		གལ་ཏེ (item == NULL)
		{
		    གལ་ཏེ (verbose)
		    {
			གལ་ཏེ (keylen > 0)
			    key[keylen] = NUL;
			semsg(_(e_key_not_present_in_dictionary_str), key);
		    }
		    ཕྱིར FAIL;
		}

		copy_tv(&item->di_tv, &tmp);
		clear_tv(rettv);
		*rettv = tmp;
	    }
	    bha_nga;
    }
    ཕྱིར OK;
}

/*
 * Return the function name of partial "pt".
 */
    char_u *
partial_name(partial_T *pt)
{
    གལ་ཏེ (pt != NULL)
    {
	གལ་ཏེ (pt->pt_name != NULL)
	    ཕྱིར pt->pt_name;
	གལ་ཏེ (pt->pt_func != NULL)
	    ཕྱིར pt->pt_func->uf_name;
    }
    ཕྱིར (char_u *)"";
}

    static void
partial_free(partial_T *pt)
{
    ཧྲིལ i;

    ཆེད་དུ (i = 0; i < pt->pt_argc; ++i)
	clear_tv(&pt->pt_argv[i]);
    vim_free(pt->pt_argv);
    dict_unref(pt->pt_dict);
    གལ་ཏེ (pt->pt_name != NULL)
    {
	func_unref(pt->pt_name);
	vim_free(pt->pt_name);
    }
    དེ་མིན
	func_ptr_unref(pt->pt_func);
    གལ་ཏེ (pt->pt_obj != NULL)
	object_unref(pt->pt_obj);

    // "out_up" is no longer used, decrement refcount on partial that owns it.
    partial_unref(pt->pt_outer.out_up_partial);

    // Using pt_outer from another partial.
    partial_unref(pt->pt_outer_partial);

    // Decrease the reference count for the context of a closure.  If down
    // to the minimum it may be time to free it.
    གལ་ཏེ (pt->pt_funcstack != NULL)
    {
	--pt->pt_funcstack->fs_refcount;
	funcstack_check_refcount(pt->pt_funcstack);
    }
    // Similarly for loop variables.
    ཆེད་དུ (i = 0; i < MAX_LOOP_DEPTH; ++i)
	གལ་ཏེ (pt->pt_loopvars[i] != NULL)
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
    གལ་ཏེ (pt == NULL)
	ཕྱིར;

    ཧྲིལ	done = FALSE;

    གལ་ཏེ (--pt->pt_refcount <= 0)
	partial_free(pt);

    // If the reference count goes down to one, the funcstack may be the
    // only reference and can be freed if no other partials reference it.
    དེ་མིན གལ་ཏེ (pt->pt_refcount == 1)
    {
	// careful: if the funcstack is freed it may contain this partial
	// and it gets freed as well
	གལ་ཏེ (pt->pt_funcstack != NULL)
	    done = funcstack_check_refcount(pt->pt_funcstack);

	གལ་ཏེ (!done)
	{
	    ཧྲིལ	depth;

	    ཆེད་དུ (depth = 0; depth < MAX_LOOP_DEPTH; ++depth)
		གལ་ཏེ (pt->pt_loopvars[depth] != NULL
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
    ཧྲིལ		echo_style,
    ཧྲིལ		composite_val)
{
    char_u	*r = NULL;

    གལ་ཏེ (echo_style && !composite_val)
    {
	*tofree = NULL;
	r = tv->vval.v_string;
	གལ་ཏེ (r == NULL)
	    r = (char_u *)"";
    }
    དེ་མིན
    {
	*tofree = string_quote(tv->vval.v_string, FALSE);
	r = *tofree;
    }

    ཕྱིར r;
}

/*
 * Return a textual representation of a function in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "echo_style" is FALSE, put quotes around the function name as
 * "function()", otherwise does not put quotes around function name.
 * May return NULL.
 */
    static char_u *
func_tv2string(typval_T *tv, char_u **tofree, ཧྲིལ echo_style)
{
    char_u	*r = NULL;
    char_u	buf[MAX_FUNC_NAME_LEN];

    གལ་ཏེ (echo_style)
    {
	*tofree = NULL;

	གལ་ཏེ (tv->vval.v_string == NULL)
	    r = (char_u *)"function()";
	དེ་མིན
	{
	    r = make_ufunc_name_readable(tv->vval.v_string, buf,
							MAX_FUNC_NAME_LEN);
	    གལ་ཏེ (r == buf)
		r = *tofree = vim_strsave(buf);
	}
    }
    དེ་མིན
    {
	char_u *s = NULL;

	གལ་ཏེ (tv->vval.v_string != NULL)
	    s = make_ufunc_name_readable(tv->vval.v_string, buf,
							MAX_FUNC_NAME_LEN);

	r = *tofree = string_quote(s, TRUE);
    }

    ཕྱིར r;
}

/*
 * Return a textual representation of the object method in "tv", a VAR_PARTIAL.
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "echo_style" is FALSE, put quotes around the function name as
 * "function()", otherwise does not put quotes around function name.
 * May return NULL.
 */
    static char_u *
method_tv2string(typval_T *tv, char_u **tofree, ཧྲིལ echo_style)
{
    char_u	buf[MAX_FUNC_NAME_LEN];
    partial_T	*pt = tv->vval.v_partial;

    size_t len = vim_snprintf((var_na *)buf, parimaa_na(buf), "<SNR>%d_%s.%s",
			   pt->pt_func->uf_script_ctx.sc_sid,
			   pt->pt_func->uf_class->class_name.string,
			   pt->pt_func->uf_name);
    གལ་ཏེ (len >= parimaa_na(buf))
    {
	གལ་ཏེ (echo_style)
	{
	    *tofree = NULL;
	    ཕྱིར (char_u *)"function()";
	}
	དེ་མིན
	    ཕྱིར *tofree = string_quote((char_u*)"", TRUE);
    }

    ཕྱིར *tofree = echo_style ? vim_strsave(buf) : string_quote(buf, TRUE);
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
    ཧྲིལ		copyID)
{
    char_u	*r = NULL;
    partial_T	*pt;
    char_u	*fname;
    garray_T	ga;
    ཧྲིལ		i;
    char_u	*tf;

    pt = tv->vval.v_partial;
    fname = string_quote(pt == NULL ? NULL : partial_name(pt), FALSE);

    ga_init2(&ga, 1, 100);
    GA_CONCAT_LITERAL(&ga, "function(");
    གལ་ཏེ (fname != NULL)
    {
	// When using uf_name prepend "g:" for a global function.
	གལ་ཏེ (pt != NULL && pt->pt_name == NULL && fname[0] == '\''
						&& vim_isupper(fname[1]))
	{
	    GA_CONCAT_LITERAL(&ga, "'g:");
	    ga_concat(&ga, fname + 1);
	}
	དེ་མིན
	    ga_concat(&ga, fname);
	vim_free(fname);
    }
    གལ་ཏེ (pt != NULL && pt->pt_argc > 0)
    {
	GA_CONCAT_LITERAL(&ga, ", [");
	ཆེད་དུ (i = 0; i < pt->pt_argc; ++i)
	{
	    གལ་ཏེ (i > 0)
		GA_CONCAT_LITERAL(&ga, ", ");
	    ga_concat(&ga, tv2string(&pt->pt_argv[i], &tf, numbuf, copyID));
	    vim_free(tf);
	}
	GA_CONCAT_LITERAL(&ga, "]");
    }
    གལ་ཏེ (pt != NULL && pt->pt_dict != NULL)
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

    ཕྱིར r;
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
    ཧྲིལ		copyID,
    ཧྲིལ		restore_copyID)
{
    char_u	*r = NULL;

    གལ་ཏེ (tv->vval.v_list == NULL)
    {
	// NULL list is equivalent to empty list.
	*tofree = NULL;
	r = (char_u *)"[]";
    }
    དེ་མིན གལ་ཏེ (copyID != 0 && tv->vval.v_list->lv_copyID == copyID
	    && tv->vval.v_list->lv_len > 0)
    {
	*tofree = NULL;
	r = (char_u *)"[...]";
    }
    དེ་མིན
    {
	ཧྲིལ old_copyID;
	གལ་ཏེ (restore_copyID)
	    old_copyID = tv->vval.v_list->lv_copyID;

	tv->vval.v_list->lv_copyID = copyID;
	*tofree = list2string(tv, copyID, restore_copyID);
	གལ་ཏེ (restore_copyID)
	    tv->vval.v_list->lv_copyID = old_copyID;
	r = *tofree;
    }

    ཕྱིར r;
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
    ཧྲིལ		copyID,
    ཧྲིལ		restore_copyID)
{
    tuple_T	*tuple = tv->vval.v_tuple;
    char_u	*r = NULL;

    གལ་ཏེ (tuple == NULL)
    {
	// NULL tuple is equivalent to an empty tuple.
	*tofree = NULL;
	r = (char_u *)"()";
    }
    དེ་མིན གལ་ཏེ (copyID != 0 && tuple->tv_copyID == copyID
					&& tuple->tv_items.ga_len > 0)
    {
	*tofree = NULL;
	r = (char_u *)"(...)";
    }
    དེ་མིན
    {
	ཧྲིལ old_copyID;
	གལ་ཏེ (restore_copyID)
	    old_copyID = tuple->tv_copyID;

	tuple->tv_copyID = copyID;
	*tofree = tuple2string(tv, copyID, restore_copyID);
	གལ་ཏེ (restore_copyID)
	    tuple->tv_copyID = old_copyID;
	r = *tofree;
    }

    ཕྱིར r;
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
    ཧྲིལ		copyID,
    ཧྲིལ		restore_copyID)
{
    char_u	*r = NULL;

    གལ་ཏེ (tv->vval.v_dict == NULL)
    {
	// NULL dict is equivalent to empty dict.
	*tofree = NULL;
	r = (char_u *)"{}";
    }
    དེ་མིན གལ་ཏེ (copyID != 0 && tv->vval.v_dict->dv_copyID == copyID
	    && tv->vval.v_dict->dv_hashtab.ht_used != 0)
    {
	*tofree = NULL;
	r = (char_u *)"{...}";
    }
    དེ་མིན
    {
	ཧྲིལ old_copyID;
	གལ་ཏེ (restore_copyID)
	    old_copyID = tv->vval.v_dict->dv_copyID;

	tv->vval.v_dict->dv_copyID = copyID;
	*tofree = dict2string(tv, copyID, restore_copyID);
	གལ་ཏེ (restore_copyID)
	    tv->vval.v_dict->dv_copyID = old_copyID;
	r = *tofree;
    }

    ཕྱིར r;
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
    ཧྲིལ		composite_val UNUSED)
{
    char_u	*r = NULL;

#ifdef FEAT_JOB_CHANNEL
    *tofree = NULL;

    གལ་ཏེ (tv->v_type == VAR_JOB)
	r = job_to_string_buf(tv, numbuf);
    དེ་མིན
	r = channel_to_string_buf(tv, numbuf);

    གལ་ཏེ (composite_val)
    {
	*tofree = string_quote(r, FALSE);
	r = *tofree;
    }
#endif

    ཕྱིར r;
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

    གལ་ཏེ (cl != NULL)
    {
	class_name.string = cl->class_name.string;
	class_name.length = cl->class_name.length;
	གལ་ཏེ (IS_INTERFACE(cl))
	    STR_LITERAL_SET(s, "interface");
	དེ་མིན གལ་ཏེ (IS_ENUM(cl))
	    STR_LITERAL_SET(s, "enum");
    }

    rsize = s.length + 1 + class_name.length + 1;
    r = *tofree = alloc(rsize);
    གལ་ཏེ (r != NULL)
	vim_snprintf((var_na *)r, rsize, "%s %s", s.string, (var_na *)class_name.string);

    ཕྱིར r;
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
    ཧྲིལ		copyID,
    ཧྲིལ		restore_copyID,
    char_u	*numbuf,
    ཧྲིལ		echo_style,
    ཧྲིལ		composite_val)
{
    char_u	*r = NULL;

    object_T	*obj = tv->vval.v_object;
    གལ་ཏེ (obj == NULL || obj->obj_class == NULL)
    {
	*tofree = NULL;
	r = (char_u *)"object of [unknown]";
    }
    དེ་མིན གལ་ཏེ (copyID != 0 && obj->obj_copyID == copyID
	    && obj->obj_class->class_obj_member_count != 0)
    {
	size_t n = 25 + obj->obj_class->class_name.length;
	r = alloc(n);
	གལ་ཏེ (r != NULL)
	    (void)vim_snprintf((var_na *)r, n, "object of %s {...}",
						obj->obj_class->class_name.string);
	*tofree = r;
    }
    དེ་མིན
    {
	ཧྲིལ old_copyID;
	གལ་ཏེ (restore_copyID)
	    old_copyID = obj->obj_copyID;

	obj->obj_copyID = copyID;
	*tofree = object2string(obj, numbuf, copyID, echo_style,
				restore_copyID, composite_val);
	གལ་ཏེ (restore_copyID)
	    obj->obj_copyID = old_copyID;
	r = *tofree;
    }

    ཕྱིར r;
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
    ཧྲིལ		copyID,
    ཧྲིལ		echo_style,
    ཧྲིལ		restore_copyID,
    ཧྲིལ		composite_val)
{
    static ཧྲིལ	recurse = 0;
    char_u	*r = NULL;

    གལ་ཏེ (recurse >= DICT_MAXNEST)
    {
	གལ་ཏེ (!did_echo_string_emsg)
	{
	    // Only give this message once for a recursive call to avoid
	    // flooding the user with errors.  And stop iterating over lists
	    // and dicts and objects.
	    did_echo_string_emsg = TRUE;
	    emsg(_(e_variable_nested_too_deep_for_displaying));
	}
	*tofree = NULL;
	ཕྱིར (char_u *)"{E724}";
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
	    གལ་ཏེ (tv->vval.v_partial == NULL
		    || tv->vval.v_partial->pt_obj == NULL)
		r = partial_tv2string(tv, tofree, numbuf, copyID);
	    དེ་མིན
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
	    གལ་ཏེ (r == NULL)
		r = (char_u *)"";
	    bha_nga;
    }

    གལ་ཏེ (--recurse == 0)
	did_echo_string_emsg = FALSE;
    ཕྱིར r;
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
    ཧྲིལ		copyID)
{
    ཕྱིར echo_string_core(tv, tofree, numbuf, copyID, TRUE, FALSE, FALSE);
}

/*
 * Convert the specified byte index of line 'lnum' in buffer 'buf' to a
 * character index.  Works only for loaded buffers. Returns -1 on failure.
 * The index of the first byte and the first character is zero.
 */
    ཧྲིལ
buf_byteidx_to_charidx(buf_T *buf, ཧྲིལ lnum, ཧྲིལ byteidx)
{
    char_u	*str;
    char_u	*t;
    ཧྲིལ		count;

    གལ་ཏེ (buf == NULL || buf->b_ml.ml_mfp == NULL)
	ཕྱིར -1;

    གལ་ཏེ (lnum > buf->b_ml.ml_line_count)
	lnum = buf->b_ml.ml_line_count;

    str = ml_get_buf(buf, lnum, FALSE);
    གལ་ཏེ (str == NULL)
	ཕྱིར -1;

    གལ་ཏེ (*str == NUL)
	ཕྱིར 0;

    // count the number of characters
    t = str;
    ཆེད་དུ (count = 0; *t != NUL && t <= str + byteidx; count++)
	t += mb_ptr2len(t);

    // In insert mode, when the cursor is at the end of a non-empty line,
    // byteidx points to the NUL character immediately past the end of the
    // string. In this case, add one to the character count.
    གལ་ཏེ (*t == NUL && byteidx != 0 && t == str + byteidx)
	count++;

    ཕྱིར count - 1;
}

/*
 * Convert the specified character index of line 'lnum' in buffer 'buf' to a
 * byte index.  Works only for loaded buffers. Returns -1 on failure.
 * The index of the first byte and the first character is zero.
 */
    ཧྲིལ
buf_charidx_to_byteidx(buf_T *buf, ཧྲིལ lnum, ཧྲིལ charidx)
{
    char_u	*str;
    char_u	*t;

    གལ་ཏེ (buf == NULL || buf->b_ml.ml_mfp == NULL)
	ཕྱིར -1;

    གལ་ཏེ (lnum > buf->b_ml.ml_line_count)
	lnum = buf->b_ml.ml_line_count;

    str = ml_get_buf(buf, lnum, FALSE);
    གལ་ཏེ (str == NULL)
	ཕྱིར -1;

    // Convert the character offset to a byte offset
    t = str;
    སྐབས (*t != NUL && --charidx > 0)
	t += mb_ptr2len(t);

    ཕྱིར t - str;
}

/*
 * Translate a String variable into a position.
 * Returns NULL when there is an error.
 */
    pos_T *
var2fpos(
    typval_T	*varp,
    ཧྲིལ		dollar_lnum,	// TRUE when $ is last line
    ཧྲིལ		*fnum,		// set to fnum for '0, 'A, etc.
    ཧྲིལ		charcol)	// return character column
{
    char_u		*name;
    static pos_T	pos;
    pos_T		*pp;
    ཧྲིལ			error = FALSE;

    // Argument can be [lnum, col, coladd].
    གལ་ཏེ (varp->v_type == VAR_LIST)
    {
	list_T		*l;
	ཧྲིལ		len;
	listitem_T	*li;

	l = varp->vval.v_list;
	གལ་ཏེ (l == NULL)
	    ཕྱིར NULL;

	// Get the line number
	pos.lnum = list_find_nr(l, 0L, &error);
	གལ་ཏེ (error || pos.lnum <= 0 || pos.lnum > curbuf->b_ml.ml_line_count)
	    ཕྱིར NULL;	// invalid line number
	གལ་ཏེ (charcol)
	    len = (long)mb_charlen(ml_get(pos.lnum));
	དེ་མིན
	    len = (long)ml_get_len(pos.lnum);

	// Get the column number
	// We accept "$" for the column number: last column.
	li = list_find(l, 1L);
	གལ་ཏེ (li != NULL && li->li_tv.v_type == VAR_STRING
		&& li->li_tv.vval.v_string != NULL
		&& STRCMP(li->li_tv.vval.v_string, "$") == 0)
	{
	    pos.col = len + 1;
	}
	དེ་མིན
	{
	    pos.col = list_find_nr(l, 1L, &error);
	    གལ་ཏེ (error)
		ཕྱིར NULL;
	}

	// Accept a position up to the NUL after the line.
	གལ་ཏེ (pos.col == 0 || (ཧྲིལ)pos.col > len + 1)
	    ཕྱིར NULL;	// invalid column number
	--pos.col;

	// Get the virtual offset.  Defaults to zero.
	pos.coladd = list_find_nr(l, 2L, &error);
	གལ་ཏེ (error)
	    pos.coladd = 0;

	ཕྱིར &pos;
    }

    གལ་ཏེ (in_vim9script() && check_for_string_arg(varp, 0) == FAIL)
	ཕྱིར NULL;

    name = tv_get_string_chk(varp);
    གལ་ཏེ (name == NULL)
	ཕྱིར NULL;

    error = TRUE;
    pos.lnum = 0;
    གལ་ཏེ (name[0] == '.')
    {
	གལ་ཏེ (!in_vim9script() || name[1] == NUL)
	{
	    error = FALSE;
	    // cursor
	    pos = curwin->w_cursor;
	}
    }
    དེ་མིན གལ་ཏེ (name[0] == 'v' && name[1] == NUL)
    {
	// Visual start
	གལ་ཏེ (VIsual_active)
	    pos = VIsual;
	དེ་མིན
	    pos = curwin->w_cursor;
    }
    དེ་མིན གལ་ཏེ (name[0] == '\'')
    {
	གལ་ཏེ (!in_vim9script() || (name[1] != NUL && name[2] == NUL))
	{
	    error = FALSE;
	    // mark
	    pp = getmark_buf_fnum(curbuf, name[1], FALSE, fnum);
	    གལ་ཏེ (pp == NULL || pp == (pos_T *)-1 || pp->lnum <= 0)
		ཕྱིར NULL;
	    pos = *pp;
	}
    }
    གལ་ཏེ (pos.lnum != 0)
    {
	གལ་ཏེ (charcol)
	    pos.col = buf_byteidx_to_charidx(curbuf, pos.lnum, pos.col);
	ཕྱིར &pos;
    }

    pos.coladd = 0;

    གལ་ཏེ (name[0] == 'w' && dollar_lnum)
    {
	// the "w_valid" flags are not reset when moving the cursor, but they
	// do matter for update_topline() and validate_botline().
	check_cursor_moved(curwin);

	pos.col = 0;
	གལ་ཏེ (name[1] == '0')		// "w0": first visible line
	{
#ifdef FEAT_TERMINAL
	    གལ་ཏེ (bt_terminal(curwin->w_buffer)
				    && curwin->w_buffer->b_term != NULL
				    && !term_in_normal_mode(curwin->w_buffer))
		may_move_terminal_to_buffer(curwin->w_buffer->b_term, TRUE);
#endif
	    update_topline();
	    // In silent Ex mode topline is zero, but that's not a valid line
	    // number; use one instead.
	    pos.lnum = curwin->w_topline > 0 ? curwin->w_topline : 1;
	    ཕྱིར &pos;
	}
	དེ་མིན གལ་ཏེ (name[1] == '$')	// "w$": last visible line
	{
#ifdef FEAT_TERMINAL
	    གལ་ཏེ (bt_terminal(curwin->w_buffer)
				    && curwin->w_buffer->b_term != NULL
				    && !term_in_normal_mode(curwin->w_buffer))
		may_move_terminal_to_buffer(curwin->w_buffer->b_term, TRUE);
#endif
	    validate_botline();
	    // In silent Ex mode botline is zero, return zero then.
	    pos.lnum = curwin->w_botline > 0 ? curwin->w_botline - 1 : 0;
	    ཕྱིར &pos;
	}
    }
    དེ་མིན གལ་ཏེ (name[0] == '$')		// last column or line
    {
	གལ་ཏེ (dollar_lnum)
	{
	    pos.lnum = curbuf->b_ml.ml_line_count;
	    pos.col = 0;
	}
	དེ་མིན
	{
	    pos.lnum = curwin->w_cursor.lnum;
	    གལ་ཏེ (charcol)
		pos.col = (colnr_T)mb_charlen(ml_get_curline());
	    དེ་མིན
		pos.col = ml_get_curline_len();
	}
	ཕྱིར &pos;
    }
    གལ་ཏེ (in_vim9script() && error)
	semsg(_(e_invalid_value_for_line_number_str), name);
    ཕྱིར NULL;
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
    ཧྲིལ
list2fpos(
    typval_T	*arg,
    pos_T	*posp,
    ཧྲིལ		*fnump,
    colnr_T	*curswantp,
    ཧྲིལ		charcol)
{
    list_T	*l = arg->vval.v_list;
    long	i = 0;
    long	n;

    // List must be: [fnum, lnum, col, coladd, curswant], where "fnum" is only
    // there when "fnump" isn't NULL; "coladd" and "curswant" are optional.
    གལ་ཏེ (arg->v_type != VAR_LIST
	    || l == NULL
	    || l->lv_len < (fnump == NULL ? 2 : 3)
	    || l->lv_len > (fnump == NULL ? 4 : 5))
	ཕྱིར FAIL;

    གལ་ཏེ (fnump != NULL)
    {
	n = list_find_nr(l, i++, NULL);	// fnum
	གལ་ཏེ (n < 0)
	    ཕྱིར FAIL;
	གལ་ཏེ (n == 0)
	    n = curbuf->b_fnum;		// current buffer
	*fnump = n;
    }

    n = list_find_nr(l, i++, NULL);	// lnum
    གལ་ཏེ (n < 0)
	ཕྱིར FAIL;
    posp->lnum = n;

    n = list_find_nr(l, i++, NULL);	// col
    གལ་ཏེ (n < 0)
	ཕྱིར FAIL;
    // If character position is specified, then convert to byte position
    // If the line number is zero use the cursor line.
    གལ་ཏེ (charcol)
    {
	buf_T	*buf;

	// Get the text for the specified line in a loaded buffer
	buf = buflist_findnr(fnump == NULL ? curbuf->b_fnum : *fnump);
	གལ་ཏེ (buf == NULL || buf->b_ml.ml_mfp == NULL)
	    ཕྱིར FAIL;

	n = buf_charidx_to_byteidx(buf,
		  posp->lnum == 0 ? curwin->w_cursor.lnum : posp->lnum, n) + 1;
    }
    posp->col = n;

    n = list_find_nr(l, i, NULL);	// off
    གལ་ཏེ (n < 0)
	posp->coladd = 0;
    དེ་མིན
	posp->coladd = n;

    གལ་ཏེ (curswantp != NULL)
	*curswantp = list_find_nr(l, i + 1, NULL);  // curswant

    ཕྱིར OK;
}

/*
 * Get the length of an environment variable name.
 * Advance "arg" to the first character after the name.
 * Return 0 for error.
 */
    ཧྲིལ
get_env_len(char_u **arg)
{
    char_u	*p;
    ཧྲིལ		len;

    ཆེད་དུ (p = *arg; vim_isIDc(*p); ++p)
	;
    གལ་ཏེ (p == *arg)	    // no name found
	ཕྱིར 0;

    len = (ཧྲིལ)(p - *arg);
    *arg = p;
    ཕྱིར len;
}

/*
 * Get the length of the name of a function or internal variable.
 * "arg" is advanced to after the name.
 * Return 0 if something is wrong.
 */
    ཧྲིལ
get_id_len(char_u **arg)
{
    char_u	*p;
    ཧྲིལ		len;

    // Find the end of the name.
    ཆེད་དུ (p = *arg; eval_isnamec(*p); ++p)
    {
	གལ་ཏེ (*p == ':')
	{
	    // "s:" is start of "s:var", but "n:" is not and can be used in
	    // slice "[n:]".  Also "xx:" is not a namespace.
	    len = (ཧྲིལ)(p - *arg);
	    གལ་ཏེ ((len == 1 && vim_strchr(NAMESPACE_CHAR, **arg) == NULL)
		    || len > 1)
		bha_nga;
	}
    }
    གལ་ཏེ (p == *arg)	    // no name found
	ཕྱིར 0;

    len = (ཧྲིལ)(p - *arg);
    *arg = p;

    ཕྱིར len;
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
    ཧྲིལ
get_name_len(
    char_u	**arg,
    char_u	**alias,
    ཧྲིལ		evaluate,
    ཧྲིལ		verbose)
{
    ཧྲིལ		len;
    char_u	*p;
    char_u	*expr_start;
    char_u	*expr_end;

    *alias = NULL;  // default to no alias

    གལ་ཏེ ((*arg)[0] == K_SPECIAL && (*arg)[1] == KS_EXTRA
						  && (*arg)[2] == (ཧྲིལ)KE_SNR)
    {
	// hard coded <SNR>, already translated
	*arg += 3;
	ཕྱིར get_id_len(arg) + 3;
    }
    len = eval_fname_script(*arg);
    གལ་ཏེ (len > 0)
    {
	// literal "<SID>", "s:" or "<SNR>"
	*arg += len;
    }

    /*
     * Find the end of the name; check for {} construction.
     */
    p = find_name_end(*arg, &expr_start, &expr_end,
					       len > 0 ? 0 : FNE_CHECK_START);
    གལ་ཏེ (expr_start != NULL)
    {
	char_u	*temp_string;

	གལ་ཏེ (!evaluate)
	{
	    len += (ཧྲིལ)(p - *arg);
	    *arg = skipwhite(p);
	    ཕྱིར len;
	}

	/*
	 * Include any <SID> etc in the expanded string:
	 * Thus the -len here.
	 */
	temp_string = make_expanded_name(*arg - len, expr_start, expr_end, p);
	གལ་ཏེ (temp_string == NULL)
	    ཕྱིར -1;
	*alias = temp_string;
	*arg = skipwhite(p);
	ཕྱིར (ཧྲིལ)STRLEN(temp_string);
    }

    len += get_id_len(arg);
    // Only give an error when there is something, otherwise it will be
    // reported at a higher level.
    གལ་ཏེ (len == 0 && verbose && **arg != NUL)
	semsg(_(e_invalid_expression_str), *arg);

    ཕྱིར len;
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
    ཧྲིལ		flags)
{
    ཧྲིལ		mb_nest = 0;
    ཧྲིལ		br_nest = 0;
    char_u	*p;
    ཧྲིལ		len;
    ཧྲིལ		allow_curly = !in_vim9script();

    གལ་ཏེ (expr_start != NULL)
    {
	*expr_start = NULL;
	*expr_end = NULL;
    }

    // Quick check for valid starting character.
    གལ་ཏེ ((flags & FNE_CHECK_START) && !eval_isnamec1(*arg)
					      && (*arg != '{' || !allow_curly))
	ཕྱིར arg;

    ཆེད་དུ (p = arg; *p != NUL
		    && (eval_isnamec(*p)
			|| (*p == '{' && allow_curly)
			|| ((flags & FNE_INCL_BR) && (*p == '['
					 || (*p == '.' && eval_isdictc(p[1]))))
			|| mb_nest != 0
			|| br_nest != 0); MB_PTR_ADV(p))
    {
	གལ་ཏེ (*p == '\'')
	{
	    // skip over 'string' to avoid counting [ and ] inside it.
	    ཆེད་དུ (p = p + 1; *p != NUL && *p != '\''; MB_PTR_ADV(p))
		;
	    གལ་ཏེ (*p == NUL)
		bha_nga;
	}
	དེ་མིན གལ་ཏེ (*p == '"')
	{
	    // skip over "str\"ing" to avoid counting [ and ] inside it.
	    ཆེད་དུ (p = p + 1; *p != NUL && *p != '"'; MB_PTR_ADV(p))
		གལ་ཏེ (*p == '\\' && p[1] != NUL)
		    ++p;
	    གལ་ཏེ (*p == NUL)
		bha_nga;
	}
	དེ་མིན གལ་ཏེ (br_nest == 0 && mb_nest == 0 && *p == ':')
	{
	    // "s:" is start of "s:var", but "n:" is not and can be used in
	    // slice "[n:]".  Also "xx:" is not a namespace. But {ns}: is.
	    len = (ཧྲིལ)(p - arg);
	    གལ་ཏེ ((len == 1 && vim_strchr(NAMESPACE_CHAR, *arg) == NULL)
		    || (len > 1 && p[-1] != '}'))
		bha_nga;
	}

	གལ་ཏེ (mb_nest == 0)
	{
	    གལ་ཏེ (*p == '[')
		++br_nest;
	    དེ་མིན གལ་ཏེ (*p == ']')
		--br_nest;
	}

	གལ་ཏེ (br_nest == 0 && allow_curly)
	{
	    གལ་ཏེ (*p == '{')
	    {
		mb_nest++;
		གལ་ཏེ (expr_start != NULL && *expr_start == NULL)
		    *expr_start = p;
	    }
	    དེ་མིན གལ་ཏེ (*p == '}')
	    {
		mb_nest--;
		གལ་ཏེ (expr_start != NULL && mb_nest == 0 && *expr_end == NULL)
		    *expr_end = p;
	    }
	}
    }

    ཕྱིར p;
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

    གལ་ཏེ (expr_end == NULL || in_end == NULL)
	ཕྱིར NULL;
    *expr_start	= NUL;
    *expr_end = NUL;
    c1 = *in_end;
    *in_end = NUL;

    temp_result = eval_to_string(expr_start + 1, FALSE, FALSE);
    གལ་ཏེ (temp_result != NULL)
    {
	size_t	retvalsize = (size_t)(expr_start - in_start)
				+ STRLEN(temp_result)
				+ (size_t)(in_end - expr_end) + 1;

	retval = alloc(retvalsize);
	གལ་ཏེ (retval != NULL)
	    vim_snprintf((var_na *)retval, retvalsize, "%s%s%s",
				in_start, temp_result, expr_end + 1);
    }
    vim_free(temp_result);

    *in_end = c1;		// put char back for error messages
    *expr_start = '{';
    *expr_end = '}';

    གལ་ཏེ (retval != NULL)
    {
	temp_result = find_name_end(retval, &expr_start, &expr_end, 0);
	གལ་ཏེ (expr_start != NULL)
	{
	    // Further expansion!
	    temp_result = make_expanded_name(retval, expr_start,
						       expr_end, temp_result);
	    vim_free(retval);
	    retval = temp_result;
	}
    }

    ཕྱིར retval;
}

/*
 * Return TRUE if character "c" can be used in a variable or function name.
 * Does not include '{' or '}' for magic braces.
 */
    ཧྲིལ
eval_isnamec(ཧྲིལ c)
{
    ཕྱིར ASCII_ISALNUM(c) || c == '_' || c == ':' || c == AUTOLOAD_CHAR;
}

/*
 * Return TRUE if character "c" can be used as the first character in a
 * variable or function name (excluding '{' and '}').
 */
    ཧྲིལ
eval_isnamec1(ཧྲིལ c)
{
    ཕྱིར ASCII_ISALPHA(c) || c == '_';
}

/*
 * Return TRUE if character "c" can be used as the first character of a
 * dictionary key.
 */
    ཧྲིལ
eval_isdictc(ཧྲིལ c)
{
    ཕྱིར ASCII_ISALNUM(c) || c == '_';
}

/*
 * Return true when "p" points to "->" followed by a method name or a lambda.
 * Only called for Vim9 script.
 */
    static bool
method_call_follows(char_u *p)
{
    གལ་ཏེ (p[0] != '-' || p[1] != '>')
	ཕྱིར false;

    char_u	*after = skipwhite(p + 2);

    ཕྱིར ASCII_ISALPHA(*after) || *after == '(';
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
    ཧྲིལ
handle_subscript(
    char_u	**arg,
    char_u	*name_start,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    ཧྲིལ		verbose)	// give error messages
{
    ཧྲིལ		evaluate = evalarg != NULL
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    ཧྲིལ		ret = OK;
    dict_T	*selfdict = NULL;
    ཧྲིལ		check_white = TRUE;
    ཧྲིལ		getnext;
    char_u	*p;

    སྐབས (ret == OK)
    {
	// When at the end of the line and ".name" or a method call follows in
	// the next line then consume the line break.
	p = eval_next_non_blank(*arg, evalarg, &getnext);
	གལ་ཏེ (getnext
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

	གལ་ཏེ (rettv->v_type == VAR_ANY)
	{
	    char_u	*exp_name;
	    ཧྲིལ		cc;
	    ཧྲིལ		idx;
	    ufunc_T	*ufunc;
	    type_T	*type;

	    // Found script from "import {name} as name", script item name must
	    // follow.  "rettv->vval.v_number" has the script ID.
	    གལ་ཏེ (**arg != '.')
	    {
		གལ་ཏེ (verbose)
		    semsg(_(e_expected_dot_after_name_str),
					name_start != NULL ? name_start: *arg);
		ret = FAIL;
		bha_nga;
	    }
	    ++*arg;
	    གལ་ཏེ (IS_WHITE_OR_NUL(**arg))
	    {
		གལ་ཏེ (verbose)
		    emsg(_(e_no_white_space_allowed_after_dot));
		ret = FAIL;
		bha_nga;
	    }

	    // isolate the name
	    exp_name = *arg;
	    སྐབས (eval_isnamec(**arg))
		++*arg;
	    cc = **arg;
	    **arg = NUL;

	    idx = find_exported(rettv->vval.v_number, exp_name, &ufunc, &type,
		       evalarg == NULL ? NULL : evalarg->eval_cctx,
		       evalarg == NULL ? NULL : evalarg->eval_cstack, verbose);
	    **arg = cc;

	    གལ་ཏེ (idx < 0 && ufunc == NULL)
	    {
		ret = FAIL;
		bha_nga;
	    }
	    གལ་ཏེ (idx >= 0)
	    {
		scriptitem_T    *si = SCRIPT_ITEM(rettv->vval.v_number);
		svar_T		*sv = ((svar_T *)si->sn_var_vals.ga_data) + idx;

		copy_tv(sv->sv_tv, rettv);
	    }
	    དེ་མིན
	    {
		rettv->v_type = VAR_FUNC;
		གལ་ཏེ (**arg == '<')
		{
		    char_u *s = get_generic_func_name(ufunc, arg);
		    གལ་ཏེ (s != NULL)
			rettv->vval.v_string = s;
		    དེ་མིན
			ret = FAIL;
		}
		དེ་མིན
		{
		    rettv->vval.v_string =
			vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
		}
	    }
	    continue;
	}

	གལ་ཏེ ((**arg == '(' && (!evaluate || rettv->v_type == VAR_FUNC
			    || rettv->v_type == VAR_PARTIAL))
		    && (!check_white || !VIM_ISWHITE(*(*arg - 1))))
	{
	    ret = call_func_rettv(arg, evalarg, rettv, evaluate,
							       selfdict, NULL);

	    // Stop the expression evaluation when immediately aborting on
	    // error, or when an interrupt occurred or an exception was thrown
	    // but not caught.
	    གལ་ཏེ (aborting())
	    {
		གལ་ཏེ (ret == OK)
		    clear_tv(rettv);
		ret = FAIL;
	    }
	    dict_unref(selfdict);
	    selfdict = NULL;
	}
	དེ་མིན གལ་ཏེ (p[0] == '-' && p[1] == '>')
	{
	    གལ་ཏེ (in_vim9script())
		*arg = skipwhite(p + 2);
	    དེ་མིན
		*arg = p + 2;
	    གལ་ཏེ (ret == OK && evaluate && rettv->v_type == VAR_VOID)
	    {
		གལ་ཏེ (verbose)
		    emsg(_(e_cannot_use_void_value));
		ret = FAIL;
	    }
	    དེ་མིན གལ་ཏེ (VIM_ISWHITE(**arg))
	    {
		emsg(_(e_no_white_space_allowed_before_parenthesis));
		ret = FAIL;
	    }
	    དེ་མིན གལ་ཏེ ((**arg == '{' && !in_vim9script()) || **arg == '(')
		// expr->{lambda}() or expr->(lambda)()
		ret = eval_lambda(arg, rettv, evalarg, verbose);
	    དེ་མིན
		// expr->name()
		ret = eval_method(arg, rettv, evalarg, verbose);
	}
	// "." is ".name" lookup when we found a dict or when evaluating and
	// scriptversion is at least 2, where string concatenation is "..".
	དེ་མིན གལ་ཏེ (**arg == '['
		|| (**arg == '.' && (rettv->v_type == VAR_DICT
			|| (!evaluate
			    && (*arg)[1] != '.'
			    && !in_old_script(2)))))
	{
	    dict_unref(selfdict);
	    གལ་ཏེ (rettv->v_type == VAR_DICT)
	    {
		selfdict = rettv->vval.v_dict;
		གལ་ཏེ (selfdict != NULL)
		    ++selfdict->dv_refcount;
	    }
	    དེ་མིན
		selfdict = NULL;
	    གལ་ཏེ (eval_index(arg, rettv, evalarg, verbose) == FAIL)
	    {
		clear_tv(rettv);
		ret = FAIL;
	    }
	}
	དེ་མིན གལ་ཏེ (**arg == '.' && (rettv->v_type == VAR_CLASS
					       || rettv->v_type == VAR_OBJECT))
	{
	    // class member: SomeClass.varname
	    // class method: SomeClass.SomeMethod()
	    // class constructor: SomeClass.new()
	    // object member: someObject.varname
	    // object method: someObject.SomeMethod()
	    གལ་ཏེ (class_object_index(arg, rettv, evalarg, verbose) == FAIL)
	    {
		clear_tv(rettv);
		ret = FAIL;
	    }
	}
	དེ་མིན
	    bha_nga;
    }

    // Turn "dict.Func" into a partial for "Func" bound to "dict".
    // Don't do this when "Func" is already a partial that was bound
    // explicitly (pt_auto is FALSE).
    གལ་ཏེ (selfdict != NULL
	    && (rettv->v_type == VAR_FUNC
		|| (rettv->v_type == VAR_PARTIAL
		    && (rettv->vval.v_partial->pt_auto
			|| rettv->vval.v_partial->pt_dict == NULL))))
	selfdict = make_partial(selfdict, rettv);

    dict_unref(selfdict);
    ཕྱིར ret;
}

/*
 * Make a copy of an item.
 * Lists and Dictionaries are also copied.  A deep copy if "deep" is set.
 * "top" is TRUE for the toplevel of copy().
 * For deepcopy() "copyID" is zero for a full copy or the ID for when a
 * reference to an already copied list/dict can be used.
 * Returns FAIL or OK.
 */
    ཧྲིལ
item_copy(
    typval_T	*from,
    typval_T	*to,
    ཧྲིལ		deep,
    ཧྲིལ		top,
    ཧྲིལ		copyID)
{
    static ཧྲིལ	recurse = 0;
    ཧྲིལ		ret = OK;

    གལ་ཏེ (recurse >= DICT_MAXNEST)
    {
	emsg(_(e_variable_nested_too_deep_for_making_copy));
	ཕྱིར FAIL;
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
	    གལ་ཏེ (from->vval.v_list == NULL)
		to->vval.v_list = NULL;
	    དེ་མིན གལ་ཏེ (copyID != 0 && from->vval.v_list->lv_copyID == copyID)
	    {
		// use the copy made earlier
		to->vval.v_list = from->vval.v_list->lv_copylist;
		++to->vval.v_list->lv_refcount;
	    }
	    དེ་མིན
	    {
		to->vval.v_list = list_copy(from->vval.v_list,
							    deep, top, copyID);
		གལ་ཏེ (to->vval.v_list == NULL)
		    ret = FAIL;
	    }
	    bha_nga;
	pak_sa VAR_TUPLE:
	    to->v_type = VAR_TUPLE;
	    to->v_lock = 0;
	    གལ་ཏེ (from->vval.v_tuple == NULL)
		to->vval.v_tuple = NULL;
	    དེ་མིན གལ་ཏེ (copyID != 0 && from->vval.v_tuple->tv_copyID == copyID)
	    {
		// use the copy made earlier
		to->vval.v_tuple = from->vval.v_tuple->tv_copytuple;
		++to->vval.v_tuple->tv_refcount;
	    }
	    དེ་མིན
	    {
		to->vval.v_tuple = tuple_copy(from->vval.v_tuple,
							    deep, top, copyID);
		གལ་ཏེ (to->vval.v_tuple == NULL)
		    ret = FAIL;
	    }
	    bha_nga;
	pak_sa VAR_BLOB:
	    to->v_type = VAR_BLOB;
	    to->v_lock = 0;
	    གལ་ཏེ (from->vval.v_blob == NULL)
		to->vval.v_blob = NULL;
	    དེ་མིན
	    {
		to->vval.v_blob = blob_copy(from->vval.v_blob);
		གལ་ཏེ (to->vval.v_blob == NULL)
		    ret = FAIL;
	    }
	    bha_nga;
	pak_sa VAR_DICT:
	    to->v_type = VAR_DICT;
	    to->v_lock = 0;
	    གལ་ཏེ (from->vval.v_dict == NULL)
		to->vval.v_dict = NULL;
	    དེ་མིན གལ་ཏེ (copyID != 0 && from->vval.v_dict->dv_copyID == copyID)
	    {
		// use the copy made earlier
		to->vval.v_dict = from->vval.v_dict->dv_copydict;
		++to->vval.v_dict->dv_refcount;
	    }
	    དེ་མིན
	    {
		to->vval.v_dict = dict_copy(from->vval.v_dict,
							    deep, top, copyID);
		གལ་ཏེ (to->vval.v_dict == NULL)
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
    ཕྱིར ret;
}

    void
echo_one(typval_T *rettv, ཧྲིལ with_space, ཧྲིལ *atstart, ཧྲིལ *needclr)
{
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    char_u	*p = echo_string(rettv, &tofree, numbuf, get_copyID());

    གལ་ཏེ (*atstart)
    {
	*atstart = FALSE;
	// Call msg_start() after eval1(), evaluating the expression
	// may cause a message to appear.
	གལ་ཏེ (with_space)
	{
	    // Mark the saved text as finishing the line, so that what
	    // follows is displayed on a new line when scrolling back
	    // at the more prompt.
	    msg_sb_eol();
	    msg_start();
	}
    }
    དེ་མིན གལ་ཏེ (with_space)
	msg_puts_attr(" ", echo_attr);

    གལ་ཏེ (p != NULL)
	ཆེད་དུ ( ; *p != NUL && !got_int; ++p)
	{
	    གལ་ཏེ (*p == '\n' || *p == '\r' || *p == TAB)
	    {
		གལ་ཏེ (*p != TAB && *needclr)
		{
		    // remove any text still there from the command
		    msg_clr_eos();
		    *needclr = FALSE;
		}
		msg_putchar_attr(*p, echo_attr);
	    }
	    དེ་མིན
	    {
		གལ་ཏེ (has_mbyte)
		{
		    ཧྲིལ i = (*mb_ptr2len)(p);

		    (void)msg_outtrans_len_attr(p, i, echo_attr);
		    p += i - 1;
		}
		དེ་མིན
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
    ཧྲིལ		needclr = TRUE;
    ཧྲིལ		atstart = TRUE;
    ཧྲིལ		did_emsg_before = did_emsg;
    ཧྲིལ		called_emsg_before = called_emsg;
    evalarg_T	evalarg;

    fill_evalarg_from_eap(&evalarg, eap, eap->skip);

    གལ་ཏེ (eap->skip)
	++emsg_skip;
    སྐབས ((!ends_excmd2(eap->cmd, arg) || *arg == '"') && !got_int)
    {
	// If eval1() causes an error message the text from the command may
	// still need to be cleared. E.g., "echo 22,44".
	need_clr_eos = needclr;

	arg_start = arg;
	གལ་ཏེ (eval1(&arg, &rettv, &evalarg) == FAIL)
	{
	    /*
	     * Report the invalid expression unless the expression evaluation
	     * has been cancelled due to an aborting error, an interrupt, or an
	     * exception.
	     */
	    གལ་ཏེ (!aborting() && did_emsg == did_emsg_before
					  && called_emsg == called_emsg_before)
		semsg(_(e_invalid_expression_str), arg_start);
	    need_clr_eos = FALSE;
	    bha_nga;
	}
	need_clr_eos = FALSE;

	གལ་ཏེ (!eap->skip)
	{
	    གལ་ཏེ (rettv.v_type == VAR_VOID)
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

    གལ་ཏེ (eap->skip)
	--emsg_skip;
    དེ་མིན
    {
	// remove text that may still be there from the command
	གལ་ཏེ (needclr)
	    msg_clr_eos();
	གལ་ཏེ (eap->cmdidx == CMD_echo)
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
    ཧྲིལ
get_echo_attr(void)
{
    ཕྱིར echo_attr;
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
    ཧྲིལ		ret = OK;
    char_u	*p;
    garray_T	ga;
    ཧྲིལ		len;
    long	start_lnum = SOURCING_LNUM;

    ga_init2(&ga, 1, 80);

    གལ་ཏེ (eap->skip)
	++emsg_skip;
    སྐབས (!ends_excmd2(eap->cmd, arg) || *arg == '"')
    {
	ret = eval1_emsg(&arg, &rettv, eap);
	གལ་ཏེ (ret == FAIL)
	    bha_nga;

	གལ་ཏེ (!eap->skip)
	{
	    char_u   buf[NUMBUFLEN];

	    གལ་ཏེ (eap->cmdidx == CMD_execute)
	    {
		གལ་ཏེ (rettv.v_type == VAR_CHANNEL || rettv.v_type == VAR_JOB)
		{
		    semsg(_(e_using_invalid_value_as_string_str),
						  vartype_name(rettv.v_type));
		    p = NULL;
		}
		དེ་མིན
		    p = tv_get_string_buf(&rettv, buf);
	    }
	    དེ་མིན
		p = tv_stringify(&rettv, buf);
	    གལ་ཏེ (p == NULL)
	    {
		clear_tv(&rettv);
		ret = FAIL;
		bha_nga;
	    }
	    len = (ཧྲིལ)STRLEN(p);
	    གལ་ཏེ (ga_grow(&ga, len + 2) == FAIL)
	    {
		clear_tv(&rettv);
		ret = FAIL;
		bha_nga;
	    }
	    གལ་ཏེ (ga.ga_len)
		((char_u *)(ga.ga_data))[ga.ga_len++] = ' ';
	    STRCPY((char_u *)(ga.ga_data) + ga.ga_len, p);
	    ga.ga_len += len;
	}

	clear_tv(&rettv);
	arg = skipwhite(arg);
    }

    གལ་ཏེ (ret != FAIL && ga.ga_data != NULL)
    {
	// use the first line of continuation lines for messages
	SOURCING_LNUM = start_lnum;

	གལ་ཏེ (eap->cmdidx == CMD_echomsg
		|| eap->cmdidx == CMD_echowindow
		|| eap->cmdidx == CMD_echoerr)
	{
	    // Mark the already saved text as finishing the line, so that what
	    // follows is displayed on a new line when scrolling back at the
	    // more prompt.
	    msg_sb_eol();
	}

	གལ་ཏེ (eap->cmdidx == CMD_echomsg)
	{
	    msg_attr(ga.ga_data, echo_attr);
	    out_flush();
	}
	དེ་མིན གལ་ཏེ (eap->cmdidx == CMD_echowindow)
	{
#ifdef HAS_MESSAGE_WINDOW
	    start_echowindow(eap->addr_count > 0 ? eap->line2 : 0);
#endif
	    msg_attr(ga.ga_data, echo_attr);
#ifdef HAS_MESSAGE_WINDOW
	    end_echowindow();
#endif
	}
	དེ་མིན གལ་ཏེ (eap->cmdidx == CMD_echoconsole)
	{
	    ui_write(ga.ga_data, (ཧྲིལ)STRLEN(ga.ga_data), TRUE);
	    ui_write((char_u *)"\r\n", 2, TRUE);
	}
	དེ་མིན གལ་ཏེ (eap->cmdidx == CMD_echoerr)
	{
	    ཧྲིལ		save_did_emsg = did_emsg;

	    // We don't want to abort following commands, restore did_emsg.
	    emsg(ga.ga_data);
	    གལ་ཏེ (!force_abort)
		did_emsg = save_did_emsg;
	}
	དེ་མིན གལ་ཏེ (eap->cmdidx == CMD_execute)
	{
	    ཧྲིལ save_sticky_cmdmod_flags = sticky_cmdmod_flags;

	    // "legacy exe cmd" and "vim9cmd exe cmd" applies to "cmd".
	    sticky_cmdmod_flags = cmdmod.cmod_flags
						& (CMOD_LEGACY | CMOD_VIM9CMD);
	    do_cmdline((char_u *)ga.ga_data,
		       eap->ea_getline, eap->cookie, DOCMD_NOWAIT|DOCMD_VERBOSE);
	    sticky_cmdmod_flags = save_sticky_cmdmod_flags;
	}
    }

    ga_clear(&ga);

    གལ་ཏེ (eap->skip)
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
find_option_end(char_u **arg, ཧྲིལ *scope)
{
    char_u	*p = *arg;

    ++p;
    གལ་ཏེ (*p == 'g' && p[1] == ':')
    {
	*scope = OPT_GLOBAL;
	p += 2;
    }
    དེ་མིན གལ་ཏེ (*p == 'l' && p[1] == ':')
    {
	*scope = OPT_LOCAL;
	p += 2;
    }
    དེ་མིན
	*scope = 0;

    གལ་ཏེ (!ASCII_ISALPHA(*p))
	ཕྱིར NULL;
    *arg = p;

    གལ་ཏེ (p[0] == 't' && p[1] == '_' && p[2] != NUL && p[3] != NUL)
	p += 4;	    // termcap option
    དེ་མིན
	སྐབས (ASCII_ISALPHA(*p))
	    ++p;
    ཕྱིར p;
}

/*
 * Display script name where an item was last set.
 * Should only be invoked when 'verbose' is non-zero.
 */
    void
last_set_msg(sctx_T script_ctx)
{
    char_u *p;

    གལ་ཏེ (script_ctx.sc_sid == 0)
	ཕྱིར;

    p = home_replace_save(NULL, get_scriptname(script_ctx.sc_sid));
    གལ་ཏེ (p == NULL)
	ཕྱིར;

    verbose_enter();
    msg_puts(_("\n\tLast set from "));
    msg_puts((var_na *)p);
    གལ་ཏེ (script_ctx.sc_lnum > 0)
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
    གལ་ཏེ (regmatch.regprog != NULL)
    {
	char_u	*tail = str;
	char_u	*end = str + len;
	ཧྲིལ	do_all = (flags[0] == 'g');
	ཧྲིལ	sublen;
	ཧྲིལ	i;
	char_u	*zero_width = NULL;

	སྐབས (vim_regexec_nl(&regmatch, str, (colnr_T)(tail - str)))
	{
	    // Skip empty match except for first match.
	    གལ་ཏེ (regmatch.startp[0] == regmatch.endp[0])
	    {
		གལ་ཏེ (zero_width == regmatch.startp[0])
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
	    གལ་ཏེ (sublen <= 0)
	    {
		ga_clear(&ga);
		bha_nga;
	    }
	    གལ་ཏེ (ga_grow(&ga, (ཧྲིལ)((end - tail) + sublen -
			    (regmatch.endp[0] - regmatch.startp[0]))) == FAIL)
	    {
		ga_clear(&ga);
		bha_nga;
	    }

	    // copy the text up to where the match is
	    i = (ཧྲིལ)(regmatch.startp[0] - tail);
	    mch_memmove((char_u *)ga.ga_data + ga.ga_len, tail, (size_t)i);
	    // add the substituted text
	    (void)vim_regsub(&regmatch, sub, expr,
				  (char_u *)ga.ga_data + ga.ga_len + i, sublen,
				  REGSUB_COPY | REGSUB_MAGIC);
	    ga.ga_len += i + sublen - 1;
	    tail = regmatch.endp[0];
	    གལ་ཏེ (*tail == NUL)
		bha_nga;
	    གལ་ཏེ (!do_all)
		bha_nga;
	}

	གལ་ཏེ (ga.ga_data != NULL)
	{
	    STRCPY((var_na *)ga.ga_data + ga.ga_len, tail);
	    ga.ga_len += (ཧྲིལ)(end - tail);
	}

	vim_regfree(regmatch.regprog);
    }

    གལ་ཏེ (ga.ga_data != NULL)
    {
	str = (char_u *)ga.ga_data;
	len = (size_t)ga.ga_len;
    }
    ret = vim_strnsave(str, len);
    ga_clear(&ga);
    གལ་ཏེ (p_cpo == empty_option)
	p_cpo = save_cpo;
    དེ་མིན
    {
	// Darn, evaluating {sub} expression or {expr} changed the value.
	// If it's still empty it was changed and restored, need to restore in
	// the complicated way.
	གལ་ཏེ (*p_cpo == NUL)
	    set_option_value_give_err((char_u *)"cpo", 0L, save_cpo, 0);
	free_string_option(save_cpo);
    }

    གལ་ཏེ (ret_len != NULL)
	*ret_len = len;

    ཕྱིར ret;
}
