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

#அந்தர்பாவ "vim.h"

#எனில் defined(FEAT_EVAL)

#ifdef VMS
# include <float.h>
#endif

#define NAMESPACE_CHAR	(char_u *)"abglstvw"

நிலையான முழு eval0_simple_funccal(char_u *arg, typval_T *rettv, exarg_T *eap, evalarg_T *evalarg);
நிலையான முழு eval2(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
நிலையான முழு eval3(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
நிலையான முழு eval4(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
நிலையான முழு eval5(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
நிலையான முழு eval6(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
நிலையான முழு eval7(char_u **arg, typval_T *rettv, evalarg_T *evalarg, முழு want_string);
நிலையான முழு eval8(char_u **arg, typval_T *rettv, evalarg_T *evalarg, முழு want_string);
நிலையான முழு eval9(char_u **arg, typval_T *rettv, evalarg_T *evalarg, முழு want_string);
நிலையான முழு eval9_leader(typval_T *rettv, முழு numeric_only, char_u *start_leader, char_u **end_leaderp);

நிலையான char_u *make_expanded_name(char_u *in_start, char_u *கோவை_start, char_u *கோவை_end, char_u *in_end);

/*
 * Return "n1" divided by "n2", taking care of dividing by zero.
 * If "failed" is not NULL set it to TRUE when dividing by zero fails.
 */
	varnumber_T
இலக்கம்_divide(varnumber_T n1, varnumber_T n2, முழு *failed)
{
    varnumber_T	result;

    எனில் (n2 == 0)
    {
	எனில் (in_vim9script())
	{
	    emsg(_(e_divide_by_zero));
	    எனில் (failed != வெற்று)
		*failed = TRUE;
	}
	எனில் (n1 == 0)
	    result = VARNUM_MIN; // similar to NaN
	இல்லையெனில் எனில் (n1 < 0)
	    result = -VARNUM_MAX;
	இல்லையெனில்
	    result = VARNUM_MAX;
    }
    இல்லையெனில் எனில் (n1 == VARNUM_MIN && n2 == -1)
    {
	// specific case: trying to do VARNUM_MIN / -1 results in a positive
	// number that doesn't fit in varnumber_T and causes an FPE
	result = VARNUM_MAX;
    }
    இல்லையெனில்
	result = n1 / n2;

    திரும்பு result;
}

/*
 * Return "n1" modulus "n2", taking care of dividing by zero.
 * If "failed" is not NULL set it to TRUE when dividing by zero fails.
 */
	varnumber_T
இலக்கம்_modulus(varnumber_T n1, varnumber_T n2, முழு *failed)
{
    எனில் (n2 == 0 && in_vim9script())
    {
	emsg(_(e_divide_by_zero));
	எனில் (failed != வெற்று)
	    *failed = TRUE;
    }
    திரும்பு (n2 == 0) ? 0 : (n1 % n2);
}

/*
 * Initialize the global and v: variables.
 */
    வெறுமை
eval_init(வெறுமை)
{
    evalvars_init();
    func_init();
}

#எனில் defined(EXITFREE)
    வெறுமை
eval_clear(வெறுமை)
{
    evalvars_clear();
    free_scriptnames();  // must come after evalvars_clear().
    free_locales();

    // autoloaded script names
    free_autoload_scriptnames();

    // unreferenced lists, tuples and dicts
    (வெறுமை)garbage_collect(FALSE);

    // functions not garbage collected
    free_all_functions();
}
#endif

    வெறுமை
fill_evalarg_from_eap(evalarg_T *evalarg, exarg_T *eap, முழு skip)
{
    init_evalarg(evalarg);
    evalarg->eval_flags = skip ? 0 : EVAL_EVALUATE;

    எனில் (eap == வெற்று)
	திரும்பு;

    evalarg->eval_cstack = eap->cstack;
    எனில் (sourcing_a_script(eap) || eap->ea_getline == get_list_வரி)
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
    முழு
eval_to_bool(
    char_u	*arg,
    முழு		*error,
    exarg_T	*eap,
    முழு		skip,	    // only parse, don't execute
    முழு		use_simple_function)
{
    typval_T	tv;
    varnumber_T	retval = FALSE;
    evalarg_T	evalarg;
    முழு		r;

    fill_evalarg_from_eap(&evalarg, eap, skip);

    எனில் (skip)
	++emsg_skip;
    எனில் (use_simple_function)
	r = eval0_simple_funccal(arg, &tv, eap, &evalarg);
    இல்லையெனில்
	r = eval0(arg, &tv, eap, &evalarg);
    எனில் (r == FAIL)
	*error = TRUE;
    இல்லையெனில்
    {
	*error = FALSE;
	எனில் (!skip)
	{
	    எனில் (in_vim9script())
		retval = tv_get_bool_chk(&tv, error);
	    இல்லையெனில்
		retval = (tv_get_எண்_chk(&tv, error) != 0);
	    clear_tv(&tv);
	}
    }
    எனில் (skip)
	--emsg_skip;
    clear_evalarg(&evalarg, eap);

    திரும்பு (முழு)retval;
}

/*
 * Call eval1() and give an error message if not done at a lower level.
 */
    நிலையான முழு
eval1_emsg(char_u **arg, typval_T *rettv, exarg_T *eap)
{
    char_u	*start = *arg;
    முழு		ret;
    முழு		did_emsg_before = did_emsg;
    முழு		called_emsg_before = called_emsg;
    evalarg_T	evalarg;

    fill_evalarg_from_eap(&evalarg, eap, eap != வெற்று && eap->skip);

    ret = eval1(arg, rettv, &evalarg);
    எனில் (ret == FAIL)
    {
	// Report the invalid expression unless the expression evaluation has
	// been cancelled due to an aborting error, an interrupt, or an
	// exception, or we already gave a more specific error.
	// Also check called_emsg for when using assert_fails().
	எனில் (!aborting() && did_emsg == did_emsg_before
					  && called_emsg == called_emsg_before)
	    semsg(_(e_invalid_expression_str), start);
    }
    clear_evalarg(&evalarg, eap);
    திரும்பு ret;
}

/*
 * Return whether a typval is a valid expression to pass to eval_expr_typval()
 * or eval_expr_to_bool().  An empty string returns FALSE;
 */
    முழு
eval_கோவை_valid_arg(typval_T *tv)
{
    திரும்பு tv->v_type != VAR_UNKNOWN
	    && (tv->v_type != VAR_STRING
		  || (tv->vval.v_string != வெற்று && *tv->vval.v_string != NUL));
}

/*
 * When calling eval_expr_typval() many times we only need one funccall_T.
 * Returns NULL when no funccall_T is to be used.
 * When returning non-NULL remove_funccal() must be called later.
 */
    funccall_T *
eval_கோவை_get_funccal(typval_T *கோவை, typval_T *rettv)
{
    எனில் (கோவை->v_type != VAR_PARTIAL)
	திரும்பு வெற்று;

    partial_T *partial = கோவை->vval.v_partial;
    எனில் (partial == வெற்று)
	திரும்பு வெற்று;
    எனில் (partial->pt_func == வெற்று
	    || partial->pt_func->uf_def_status == UF_NOT_COMPILED)
	திரும்பு வெற்று;

    திரும்பு create_funccal(partial->pt_func, rettv);
}

/*
 * Evaluate a partial.
 * Pass arguments "argv[argc]".
 * "fc_arg" is from eval_expr_get_funccal() or NULL;
 * Return the result in "rettv" and OK or FAIL.
 */
    நிலையான முழு
eval_கோவை_partial(
    typval_T	*கோவை,
    typval_T	*argv,
    முழு		argc,
    funccall_T	*fc_arg,
    typval_T	*rettv)
{
    partial_T	*partial = கோவை->vval.v_partial;

    எனில் (partial == வெற்று)
	திரும்பு FAIL;

    எனில் (partial->pt_func != வெற்று
			&& partial->pt_func->uf_def_status != UF_NOT_COMPILED)
    {
	funccall_T	*fc = fc_arg != வெற்று ? fc_arg
				: create_funccal(partial->pt_func, rettv);
	முழு		r;

	எனில் (fc == வெற்று)
	    திரும்பு FAIL;

	// Shortcut to call a compiled function with minimal overhead.
	எனில் (partial->pt_obj != வெற்று)
	    partial->pt_obj->obj_refcount++;
	r = call_def_function(partial->pt_func, argc, argv, DEF_USE_PT_ARGV,
					partial, partial->pt_obj, fc, rettv);
	எனில் (fc_arg == வெற்று)
	    remove_funccal();
	எனில் (r == FAIL)
	    திரும்பு FAIL;
    }
    இல்லையெனில்
    {
	char_u		*s = partial_name(partial);
	funcexe_T	funcexe;

	எனில் (s == வெற்று || *s == NUL)
	    திரும்பு FAIL;

	CLEAR_FIELD(funcexe);
	funcexe.fe_evaluate = TRUE;
	funcexe.fe_partial = partial;
	எனில் (call_func(s, -1, rettv, argc, argv, &funcexe) == FAIL)
	    திரும்பு FAIL;
    }

    திரும்பு OK;
}

/*
 * Evaluate an expression which is a function.
 * Pass arguments "argv[argc]".
 * Return the result in "rettv" and OK or FAIL.
 */
    நிலையான முழு
eval_கோவை_func(
    typval_T	*கோவை,
    typval_T	*argv,
    முழு		argc,
    typval_T	*rettv)
{
    funcexe_T	funcexe;
    char_u	buf[NUMBUFLEN];
    char_u	*s;

    எனில் (கோவை->v_type == VAR_FUNC)
	s = கோவை->vval.v_string;
    இல்லையெனில்
	s = tv_get_string_buf_chk_strict(கோவை, buf, in_vim9script());
    எனில் (s == வெற்று || *s == NUL)
	திரும்பு FAIL;

    CLEAR_FIELD(funcexe);
    funcexe.fe_evaluate = TRUE;
    எனில் (call_func(s, -1, rettv, argc, argv, &funcexe) == FAIL)
	திரும்பு FAIL;

    திரும்பு OK;
}

/*
 * Evaluate an expression, which is a string.
 * Return the result in "rettv" and OK or FAIL.
 */
    நிலையான முழு
eval_கோவை_string(
    typval_T	*கோவை,
    typval_T	*rettv)
{
    char_u	*s;
    char_u	buf[NUMBUFLEN];

    s = tv_get_string_buf_chk_strict(கோவை, buf, in_vim9script());
    எனில் (s == வெற்று)
	திரும்பு FAIL;

    s = skipwhite(s);
    எனில் (eval1_emsg(&s, rettv, வெற்று) == FAIL)
	திரும்பு FAIL;

    எனில் (*skipwhite(s) != NUL)  // check for trailing chars after expr
    {
	clear_tv(rettv);
	semsg(_(e_invalid_expression_str), s);
	திரும்பு FAIL;
    }

    திரும்பு OK;
}

/*
 * Evaluate an expression, which can be a function, partial or string.
 * Pass arguments "argv[argc]".
 * If "want_func" is TRUE treat a string as a function name, not an expression.
 * "fc_arg" is from eval_expr_get_funccal() or NULL;
 * Return the result in "rettv" and OK or FAIL.
 */
    முழு
eval_கோவை_typval(
	typval_T    *கோவை,
	முழு	    want_func,
	typval_T    *argv,
	முழு	    argc,
	funccall_T  *fc_arg,
	typval_T    *rettv)
{
    எனில் (கோவை->v_type == VAR_PARTIAL)
	திரும்பு eval_கோவை_partial(கோவை, argv, argc, fc_arg, rettv);
    எனில் (கோவை->v_type == VAR_INSTR)
	திரும்பு exe_typval_instr(கோவை, rettv);
    எனில் (கோவை->v_type == VAR_FUNC || want_func)
	திரும்பு eval_கோவை_func(கோவை, argv, argc, rettv);

    திரும்பு eval_கோவை_string(கோவை, rettv);
}

/*
 * Like eval_to_bool() but using a typval_T instead of a string.
 * Works for string, funcref and partial.
 */
    முழு
eval_கோவை_to_bool(typval_T *கோவை, முழு *error)
{
    typval_T	rettv;
    முழு		res;

    எனில் (eval_கோவை_typval(கோவை, FALSE, வெற்று, 0, வெற்று, &rettv) == FAIL)
    {
	*error = TRUE;
	திரும்பு FALSE;
    }
    res = (tv_get_bool_chk(&rettv, error) != 0);
    clear_tv(&rettv);
    திரும்பு res;
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
    முழு		skip)	    // only parse, don't execute
{
    typval_T	tv;
    char_u	*retval;
    evalarg_T	evalarg;

    fill_evalarg_from_eap(&evalarg, eap, skip);
    எனில் (skip)
	++emsg_skip;
    எனில் (eval0(arg, &tv, eap, &evalarg) == FAIL || skip)
	retval = வெற்று;
    இல்லையெனில்
    {
	retval = vim_strsave(tv_get_string(&tv));
	clear_tv(&tv);
    }
    எனில் (skip)
	--emsg_skip;
    clear_evalarg(&evalarg, eap);

    திரும்பு retval;
}

/*
 * Initialize "evalarg" for use.
 */
    வெறுமை
init_evalarg(evalarg_T *evalarg)
{
    CLEAR_POINTER(evalarg);
    ga_init2(&evalarg->eval_tofree_ga, அளவு(char_u *), 20);
}

/*
 * If "evalarg->eval_tofree" is not NULL free it later.
 * Caller is expected to overwrite "evalarg->eval_tofree" next.
 */
    நிலையான வெறுமை
free_eval_tofree_later(evalarg_T *evalarg)
{
    எனில் (evalarg->eval_tofree == வெற்று)
	திரும்பு;

    எனில் (ga_grow(&evalarg->eval_tofree_ga, 1) == OK)
	((char_u **)evalarg->eval_tofree_ga.ga_data)
	    [evalarg->eval_tofree_ga.ga_len++]
	    = evalarg->eval_tofree;
    இல்லையெனில்
	vim_free(evalarg->eval_tofree);
}

/*
 * After using "evalarg" filled from "eap": free the memory.
 */
    வெறுமை
clear_evalarg(evalarg_T *evalarg, exarg_T *eap)
{
    எனில் (evalarg == வெற்று)
	திரும்பு;

    garray_T *etga = &evalarg->eval_tofree_ga;

    எனில் (evalarg->eval_tofree != வெற்று || evalarg->eval_using_cmdline)
    {
	எனில் (eap != வெற்று)
	{
	    // We may need to keep the original command line, e.g. for
	    // ":let" it has the variable names.  But we may also need
	    // the new one, "nextcmd" points into it.  Keep both.
	    vim_free(eap->cmdline_tofree);
	    eap->cmdline_tofree = *eap->cmdlinep;

	    எனில் (evalarg->eval_using_cmdline && etga->ga_len > 0)
	    {
		// "nextcmd" points into the last line in eval_tofree_ga,
		// need to keep it around.
		--etga->ga_len;
		*eap->cmdlinep = ((char_u **)etga->ga_data)[etga->ga_len];
		vim_free(evalarg->eval_tofree);
	    }
	    இல்லையெனில்
		*eap->cmdlinep = evalarg->eval_tofree;
	}
	இல்லையெனில்
	    vim_free(evalarg->eval_tofree);
	evalarg->eval_tofree = வெற்று;
    }

    ga_clear_strings(etga);
    VIM_CLEAR(evalarg->eval_tofree_lambda);
}

/*
 * Skip over an expression at "*pp".
 * Return FAIL for an error, OK otherwise.
 */
    முழு
skip_கோவை(char_u **pp, evalarg_T *evalarg)
{
    typval_T	rettv;

    *pp = skipwhite(*pp);
    திரும்பு eval1(pp, &rettv, evalarg);
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
    முழு
skip_கோவை_concatenate(
	char_u	    **arg,
	char_u	    **start,
	char_u	    **end,
	evalarg_T   *evalarg)
{
    typval_T	rettv;
    முழு		res;
    முழு		vim9script = in_vim9script();
    garray_T    *gap = evalarg == வெற்று ? வெற்று : &evalarg->eval_ga;
    garray_T    *freegap = evalarg == வெற்று ? வெற்று : &evalarg->eval_freega;
    முழு		save_flags = evalarg == வெற்று ? 0 : evalarg->eval_flags;
    முழு		evaluate = evalarg == வெற்று
			       ? FALSE : (evalarg->eval_flags & EVAL_EVALUATE);

    எனில் (vim9script && evaluate
	       && (evalarg->eval_cookie != வெற்று || evalarg->eval_cctx != வெற்று))
    {
	ga_init2(gap, அளவு(char_u *), 10);
	// leave room for "start"
	எனில் (ga_grow(gap, 1) == OK)
	    ++gap->ga_len;
	ga_init2(freegap, அளவு(char_u *), 10);
    }
    *start = *arg;

    // Don't evaluate the expression.
    எனில் (evalarg != வெற்று)
	evalarg->eval_flags &= ~EVAL_EVALUATE;
    *arg = skipwhite(*arg);
    res = eval1(arg, &rettv, evalarg);
    *end = *arg;
    எனில் (evalarg != வெற்று)
	evalarg->eval_flags = save_flags;

    எனில் (vim9script && evaluate
	    && (evalarg->eval_cookie != வெற்று || evalarg->eval_cctx != வெற்று))
    {
	எனில் (evalarg->eval_ga.ga_len == 1)
	{
	    // just the one line, no need to concatenate
	    ga_clear(gap);
	    gap->ga_itemsize = 0;
	}
	இல்லையெனில்
	{
	    char_u	    *p;
	    size_t	    endoff = STRLEN(*arg);

	    // Line breaks encountered, concatenate all the lines.
	    *((char_u **)gap->ga_data) = *start;
	    p = ga_concat_strings(gap, " ");

	    // free the lines only when using getsourceline()
	    எனில் (evalarg->eval_cookie != வெற்று)
	    {
		// Do not free the first line, the caller can still use it.
		*((char_u **)gap->ga_data) = வெற்று;
		// Do not free the last line, "arg" points into it, free it
		// later.  Also free "eval_tofree" later if needed.
		free_eval_tofree_later(evalarg);
		evalarg->eval_tofree =
				    ((char_u **)gap->ga_data)[gap->ga_len - 1];
		((char_u **)gap->ga_data)[gap->ga_len - 1] = வெற்று;
		ga_clear_strings(gap);
		ga_clear(freegap);
	    }
	    இல்லையெனில்
	    {
		ga_clear(gap);

		// free lines that were explicitly marked for freeing
		ga_clear_strings(freegap);
	    }

	    gap->ga_itemsize = 0;
	    எனில் (p == வெற்று)
		திரும்பு FAIL;
	    *start = p;
	    vim_free(evalarg->eval_tofree_lambda);
	    evalarg->eval_tofree_lambda = p;
	    // Compute "end" relative to the end.
	    *end = *start + STRLEN(*start) - endoff;
	}
    }

    திரும்பு res;
}

/*
 * Convert "tv" to a string.
 * When "join_list" is TRUE convert a List or a Tuple into a sequence of lines.
 * Returns an allocated string (NULL when out of memory).
 */
    char_u *
typval2string(typval_T *tv, முழு join_list)
{
    garray_T	ga;
    char_u	*retval = வெற்று;

    எனில் (join_list && (tv->v_type == VAR_LIST || tv->v_type == VAR_TUPLE))
    {
	எனில் (tv->v_type == VAR_LIST)
	{
	    ga_init2(&ga, அளவு(எழுத்து), 80);
	    எனில் (tv->vval.v_list != வெற்று)
	    {
		list_join(&ga, tv->vval.v_list, (char_u *)"\n", TRUE, FALSE,
									0);
		எனில் (tv->vval.v_list->lv_len > 0)
		    ga_append(&ga, NL);
	    }
	    ga_append(&ga, NUL);
	    retval = (char_u *)ga.ga_data;
	}
	இல்லையெனில்
	{
	    // tuple
	    ga_init2(&ga, அளவு(எழுத்து), 80);
	    எனில் (tv->vval.v_tuple != வெற்று)
	    {
		tuple_join(&ga, tv->vval.v_tuple, (char_u *)"\n", TRUE, FALSE,
									0);
		எனில் (TUPLE_LEN(tv->vval.v_tuple) > 0)
		    ga_append(&ga, NL);
	    }
	    ga_append(&ga, NUL);
	    retval = (char_u *)ga.ga_data;
	}
    }
    இல்லையெனில் எனில் (tv->v_type == VAR_LIST
	    || tv->v_type == VAR_TUPLE
	    || tv->v_type == VAR_DICT)
    {
	char_u	*tofree;
	char_u	numbuf[NUMBUFLEN];

	retval = tv2string(tv, &tofree, numbuf, 0);
	// Make a copy if we have a value but it's not in allocated memory.
	எனில் (retval != வெற்று && tofree == வெற்று)
	    retval = vim_strsave(retval);
    }
    இல்லையெனில்
	retval = vim_strsave(tv_get_string(tv));
    திரும்பு retval;
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
    முழு		join_list,
    exarg_T	*eap,
    முழு		use_simple_function)
{
    typval_T	tv;
    char_u	*retval;
    evalarg_T	evalarg;
    முழு		r;

    fill_evalarg_from_eap(&evalarg, eap, eap != வெற்று && eap->skip);
    எனில் (use_simple_function)
	r = eval0_simple_funccal(arg, &tv, வெற்று, &evalarg);
    இல்லையெனில்
	r = eval0(arg, &tv, வெற்று, &evalarg);
    எனில் (r == FAIL)
	retval = வெற்று;
    இல்லையெனில்
    {
	retval = typval2string(&tv, join_list);
	clear_tv(&tv);
    }
    clear_evalarg(&evalarg, வெற்று);

    திரும்பு retval;
}

    char_u *
eval_to_string(
    char_u	*arg,
    முழு		join_list,
    முழு		use_simple_function)
{
    திரும்பு eval_to_string_eap(arg, join_list, வெற்று, use_simple_function);
}

/*
 * Call eval_to_string() without using current local variables and using
 * textlock.  When "use_sandbox" is TRUE use the sandbox.
 * Use legacy Vim script syntax.
 */
    char_u *
eval_to_string_safe(
    char_u	*arg,
    முழு		use_sandbox,
    முழு		keep_script_version,
    முழு		use_simple_function)
{
    char_u	*retval;
    funccal_entry_T funccal_entry;
    முழு		save_sc_version = current_sctx.sc_version;
    முழு		save_garbage = may_garbage_collect;

    எனில் (!keep_script_version)
	current_sctx.sc_version = 1;
    save_funccal(&funccal_entry);
    எனில் (use_sandbox)
	++sandbox;
    ++textlock;
    may_garbage_collect = FALSE;
    retval = eval_to_string(arg, FALSE, use_simple_function);
    எனில் (use_sandbox)
	--sandbox;
    --textlock;
    may_garbage_collect = save_garbage;
    restore_funccal();
    current_sctx.sc_version = save_sc_version;
    திரும்பு retval;
}

/*
 * Top level evaluation function, returning a number.
 * Evaluates "expr" silently.
 * Returns -1 for an error.
 */
    varnumber_T
eval_to_எண்(char_u *கோவை, முழு use_simple_function)
{
    typval_T	rettv;
    varnumber_T	retval;
    char_u	*p = skipwhite(கோவை);
    முழு		r = NOTDONE;

    ++emsg_off;

    எனில் (use_simple_function)
	r = may_call_simple_func(கோவை, &rettv);
    எனில் (r == NOTDONE)
	r = eval1(&p, &rettv, &EVALARG_EVALUATE);
    எனில் (r == FAIL)
	retval = -1;
    இல்லையெனில்
    {
	retval = tv_get_எண்_chk(&rettv, வெற்று);
	clear_tv(&rettv);
    }
    --emsg_off;

    திரும்பு retval;
}

/*
 * Top level evaluation function.
 * Returns an allocated typval_T with the result.
 * Returns NULL when there is an error.
 */
    typval_T *
eval_கோவை(char_u *arg, exarg_T *eap)
{
    திரும்பு eval_கோவை_ext(arg, eap, FALSE);
}

    typval_T *
eval_கோவை_ext(char_u *arg, exarg_T *eap, முழு use_simple_function)
{
    typval_T	*tv;
    evalarg_T	evalarg;

    fill_evalarg_from_eap(&evalarg, eap, eap != வெற்று && eap->skip);

    tv = ALLOC_ONE(typval_T);
    எனில் (tv != வெற்று)
    {
	முழு	r = NOTDONE;

	எனில் (use_simple_function)
	    r = eval0_simple_funccal(arg, tv, eap, &evalarg);
	எனில் (r == NOTDONE)
	    r = eval0(arg, tv, eap, &evalarg);

	எனில் (r == FAIL)
	    VIM_CLEAR(tv);
    }

    clear_evalarg(&evalarg, eap);
    திரும்பு tv;
}

/*
 * "*arg" points to what can be a function name in the form of "import.Name" or
 * "Funcref".  Return the name of the function.  Set "tofree" to something that
 * was allocated.
 * If "verbose" is FALSE no errors are given.
 * Return NULL for any failure.
 */
    நிலையான char_u *
deref_function_name(
	    char_u	**arg,
	    char_u	**tofree,
	    evalarg_T	*evalarg,
	    முழு		verbose)
{
    typval_T	ref;
    char_u	*name = *arg;
    முழு		save_flags = 0;
    முழு		evaluate = evalarg != வெற்று
				      && (evalarg->eval_flags & EVAL_EVALUATE);

    ref.v_type = VAR_UNKNOWN;
    எனில் (evalarg != வெற்று)
    {
	// need to evaluate this to get an import, like in "a.Func"
	save_flags = evalarg->eval_flags;
	evalarg->eval_flags |= EVAL_EVALUATE;
    }
    எனில் (eval9(arg, &ref, evalarg, FALSE) == FAIL)
    {
	dictitem_T	*v;

	// If <SID>VarName was used it would not be found, try another way.
	v = find_var_also_in_script(name, வெற்று, FALSE);
	எனில் (v == வெற்று)
	{
	    name = வெற்று;
	    செல் theend;
	}
	copy_tv(&v->di_tv, &ref);
    }
    எனில் (*skipwhite(*arg) != NUL)
    {
	எனில் (verbose)
	    semsg(_(e_trailing_characters_str), *arg);
	name = வெற்று;
    }
    இல்லையெனில் எனில் (ref.v_type == VAR_FUNC && ref.vval.v_string != வெற்று)
    {
	name = ref.vval.v_string;
	ref.vval.v_string = வெற்று;
	*tofree = name;
    }
    இல்லையெனில் எனில் (ref.v_type == VAR_PARTIAL && ref.vval.v_partial != வெற்று)
    {
	எனில் (ref.vval.v_partial->pt_argc > 0
		|| ref.vval.v_partial->pt_dict != வெற்று)
	{
	    எனில் (verbose)
		emsg(_(e_cannot_use_partial_here));
	    name = வெற்று;
	}
	இல்லையெனில்
	{
	    name = vim_strsave(partial_name(ref.vval.v_partial));
	    *tofree = name;
	}
    }
    இல்லையெனில் எனில் (evaluate)
    {
	எனில் (verbose)
	    semsg(_(e_not_callable_type_str), name);
	name = வெற்று;
    }

theend:
    clear_tv(&ref);
    எனில் (evalarg != வெற்று)
	evalarg->eval_flags = save_flags;
    திரும்பு name;
}

/*
 * Call some Vim script function and return the result in "*rettv".
 * Uses argv[0] to argv[argc - 1] for the function arguments.  argv[argc]
 * should have type VAR_UNKNOWN.
 * Returns OK or FAIL.
 */
    முழு
call_vim_function(
    char_u      *func,
    முழு		argc,
    typval_T	*argv,
    typval_T	*rettv)
{
    முழு		ret;
    funcexe_T	funcexe;
    char_u	*arg;
    char_u	*name;
    char_u	*tofree = வெற்று;
    முழு		ignore_errors;

    rettv->v_type = VAR_UNKNOWN;		// clear_tv() uses this
    CLEAR_FIELD(funcexe);
    funcexe.fe_firstline = curwin->w_cursor.lnum;
    funcexe.fe_lastline = curwin->w_cursor.lnum;
    funcexe.fe_evaluate = TRUE;

    // The name might be "import.Func" or "Funcref".  We don't know, we need to
    // ignore errors for an undefined name.  But we do want errors when an
    // autoload script has errors.  Guess that when there is a dot in the name
    // showing errors is the right choice.
    ignore_errors = vim_strchr(func, '.') == வெற்று;
    arg = func;
    எனில் (ignore_errors)
	++emsg_off;
    name = deref_function_name(&arg, &tofree, &EVALARG_EVALUATE, FALSE);
    எனில் (ignore_errors)
	--emsg_off;
    எனில் (name == வெற்று)
	name = func;

    ret = call_func(name, -1, rettv, argc, argv, &funcexe);

    எனில் (ret == FAIL)
	clear_tv(rettv);
    vim_free(tofree);

    திரும்பு ret;
}

/*
 * Call Vim script function "func" and return the result as a string.
 * Uses "argv[0]" to "argv[argc - 1]" for the function arguments. "argv[argc]"
 * should have type VAR_UNKNOWN.
 * Returns NULL when calling the function fails.
 */
    வெறுமை *
call_func_retstr(
    char_u      *func,
    முழு		argc,
    typval_T	*argv)
{
    typval_T	rettv;
    char_u	*retval;

    எனில் (call_vim_function(func, argc, argv, &rettv) == FAIL)
	திரும்பு வெற்று;

    retval = vim_strsave(tv_get_string(&rettv));
    clear_tv(&rettv);
    திரும்பு retval;
}

/*
 * Call Vim script function "func" and return the result as a List.
 * Uses "argv" and "argc" as call_func_retstr().
 * Returns NULL when there is something wrong.
 * Gives an error when the returned value is not a list.
 */
    வெறுமை *
call_func_retlist(
    char_u      *func,
    முழு		argc,
    typval_T	*argv)
{
    typval_T	rettv;

    எனில் (call_vim_function(func, argc, argv, &rettv) == FAIL)
	திரும்பு வெற்று;

    எனில் (rettv.v_type != VAR_LIST)
    {
	semsg(_(e_custom_list_completion_function_does_not_return_list_but_str),
		vartype_name(rettv.v_type));
	clear_tv(&rettv);
	திரும்பு வெற்று;
    }

    திரும்பு rettv.vval.v_list;
}

#எனில் defined(FEAT_FOLDING)
/*
 * Evaluate "arg", which is 'foldexpr'.
 * Note: caller must set "curwin" to match "arg".
 * Returns the foldlevel, and any character preceding it in "*cp".  Doesn't
 * give error messages.
 */
    முழு
eval_foldexpr(win_T *wp, முழு *cp)
{
    char_u	*arg;
    typval_T	tv;
    varnumber_T	retval;
    char_u	*s;
    sctx_T	saved_sctx = current_sctx;
    முழு		use_sandbox = was_set_insecurely(wp, (char_u *)"foldexpr",
								    OPT_LOCAL);

    arg = skipwhite(wp->w_p_fde);
    current_sctx = wp->w_p_script_ctx[WV_FDE];

    ++emsg_off;
    எனில் (use_sandbox)
	++sandbox;
    ++textlock;
    *cp = NUL;

    // Evaluate the expression.  If the expression is "FuncName()" call the
    // function directly.
    எனில் (eval0_simple_funccal(arg, &tv, வெற்று, &EVALARG_EVALUATE) == FAIL)
	retval = 0;
    இல்லையெனில்
    {
	// If the result is a number, just return the number.
	எனில் (tv.v_type == VAR_எண்)
	    retval = tv.vval.v_எண்;
	இல்லையெனில் எனில் (tv.v_type != VAR_STRING || tv.vval.v_string == வெற்று)
	    retval = 0;
	இல்லையெனில்
	{
	    // If the result is a string, check if there is a non-digit before
	    // the number.
	    s = tv.vval.v_string;
	    எனில் (*s != NUL && !VIM_ISDIGIT(*s) && *s != '-')
		*cp = *s++;
	    retval = atol((எழுத்து *)s);
	}
	clear_tv(&tv);
    }
    --emsg_off;
    எனில் (use_sandbox)
	--sandbox;
    --textlock;
    clear_evalarg(&EVALARG_EVALUATE, வெற்று);
    current_sctx = saved_sctx;

    திரும்பு (முழு)retval;
}
#endif

#ifdef LOG_LOCKVAR
வகைவரை கட்டமைப்பு flag_string_S
{
    முழு	    flag;
    எழுத்து    *str;
} flag_string_T;

    நிலையான எழுத்து *
flags_tostring(முழு flags, flag_string_T *_fstring, எழுத்து *buf, size_t n)
{
    எழுத்து *p = buf;
    *p = NUL;
    க்கு (flag_string_T *fstring = _fstring; fstring->flag; ++fstring)
    {
	எனில் ((fstring->flag & flags) != 0)
	{
	    size_t len = STRLEN(fstring->str);
	    எனில் (n > p - buf + len + 7)
	    {
		STRCAT(p, fstring->str);
		p += len;
		STRCAT(p, " ");
		++p;
	    }
	    இல்லையெனில்
	    {
		STRCAT(buf, "...");
		நிறுத்து;
	    }
	}
    }
    திரும்பு buf;
}

flag_string_T glv_flag_strings[] = {
    { GLV_QUIET,		"QUIET" },
    { GLV_NO_AUTOLOAD,		"NO_AUTOLOAD" },
    { GLV_READ_ONLY,		"READ_ONLY" },
    { GLV_NO_DECL,		"NO_DECL" },
    { GLV_COMPILING,		"COMPILING" },
    { GLV_ASSIGN_WITH_OP,	"ASSIGN_WITH_OP" },
    { GLV_PREFER_FUNC,		"PREFER_FUNC" },
    { 0,			வெற்று }
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
    நிலையான வெறுமை
fill_lval_from_lval_வேர்(lval_T *lp, lval_வேர்_T *lr)
{
#ifdef LOG_LOCKVAR
    ch_log(வெற்று, "LKVAR: fill_lval_from_lval_root(): name %s, tv %p",
						lp->ll_name, (வெறுமை*)lr->lr_tv);
#endif
    எனில் (lr->lr_tv == வெற்று)
	திரும்பு;
    எனில் (!lr->lr_is_arg && lr->lr_tv->v_type == VAR_CLASS)
    {
	எனில் (lr->lr_tv->vval.v_class != வெற்று)
	{
	    // Special special case. Look for a bare class variable reference.
	    class_T	*cl = lr->lr_tv->vval.v_class;
	    முழு		m_idx;
	    ocmember_T	*m = class_member_lookup(cl, lp->ll_name,
					lp->ll_name_end - lp->ll_name, &m_idx);
	    எனில் (m != வெற்று)
	    {
		// Assuming "inside class" since bare reference.
		lp->ll_class = lr->lr_tv->vval.v_class;
		lp->ll_oi = m_idx;
		lp->ll_valtype = m->ocm_type;
		lp->ll_tv = &lp->ll_class->class_members_tv[m_idx];
#ifdef LOG_LOCKVAR
		ch_log(வெற்று, "LKVAR:    ... class member %s.%s",
					lp->ll_class->class_name.string, lp->ll_name);
#endif
		திரும்பு;
	    }
	}
    }

#ifdef LOG_LOCKVAR
    ch_log(வெற்று, "LKVAR:    ... type: %s", vartype_name(lr->lr_tv->v_type));
#endif
    lp->ll_tv = lr->lr_tv;
    lp->ll_is_வேர் = TRUE;
}

/*
 * Check if the class has permission to access the member.
 * Returns OK or FAIL.
 */
    நிலையான முழு
get_lval_check_access(
    class_T	*cl_exec,   // executing class, NULL if :def or script level
    class_T	*cl,	    // class which contains the member
    ocmember_T	*om,	    // member being accessed
    char_u	*p,	    // char after member name
    முழு		flags)	    // GLV flags to check if writing to lval
{
#ifdef LOG_LOCKVAR
    ch_log(வெற்று, "LKVAR: get_lval_check_access(), cl_exec %p, cl %p, %c",
						(வெறுமை*)cl_exec, (வெறுமை*)cl, *p);
#endif
    எனில் (cl_exec != வெற்று && cl_exec == cl)
	திரும்பு OK;

    எழுத்து *செய்தி = வெற்று;
    தேர்வு (om->ocm_access)
    {
	நிலை VIM_ACCESS_PRIVATE:
	    செய்தி = e_cannot_access_protected_variable_str;
	    நிறுத்து;
	நிலை VIM_ACCESS_READ:
	    // If [idx] or .key following, read only OK.
	    எனில் (*p == '[' || *p == '.')
		நிறுத்து;
	    எனில் ((flags & GLV_READ_ONLY) == 0)
	    {
		எனில் (IS_ENUM(cl))
		{
		    எனில் (om->ocm_type->tt_type == VAR_OBJECT)
			semsg(_(e_enumvalue_str_cannot_be_modified),
				cl->class_name.string, om->ocm_name.string);
		    இல்லையெனில்
			செய்தி = e_variable_is_not_writable_str;
		}
		இல்லையெனில்
		    செய்தி = e_variable_is_not_writable_str;
	    }
	    நிறுத்து;
	நிலை VIM_ACCESS_ALL:
	    நிறுத்து;
    }
    எனில் (செய்தி != வெற்று)
    {
	emsg_var_cl_define(செய்தி, om->ocm_name.string, 0, cl);
	திரும்பு FAIL;
    }
    திரும்பு OK;
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
    நிலையான char_u *
get_lval_imported(
    lval_T	*lp,
    scid_T	imp_sid,
    char_u	*p,
    dictitem_T	**dip,
    முழு		fne_flags)
{
    ufunc_T	*ufunc;
    type_T	*type = வெற்று;
    முழு		cc;
    முழு		rc = FAIL;

    p = skipwhite(p);

    import_check_sourced_sid(&imp_sid);
    lp->ll_sid = imp_sid;
    lp->ll_name = p;
    p = find_name_end(lp->ll_name, வெற்று, வெற்று, fne_flags);
    lp->ll_name_end = p;

    // check the item is exported
    cc = *p;
    *p = NUL;
    எனில் (find_exported(imp_sid, lp->ll_name, &ufunc, &type, வெற்று, வெற்று,
								TRUE) == -1)
	செல் failed;

    // Get the typval for the exported item
    hashtab_T *ht = &SCRIPT_VARS(imp_sid);
    எனில் (ht == வெற்று)
	செல் failed;

    dictitem_T *di = find_var_in_ht(ht, 0, lp->ll_name, TRUE);
    எனில் (di == வெற்று)
	// script is autoloaded.  So variable will be found later
	செல் success;

    *dip = di;

    // Check whether the variable is writable.
    svar_T *sv = find_typval_in_script(&di->di_tv, imp_sid, FALSE);
    எனில் (sv != வெற்று && sv->sv_const != 0)
    {
	semsg(_(e_cannot_change_readonly_variable_str), lp->ll_name);
	செல் failed;
    }

    // check whether variable is locked
    எனில் (மதிப்பு_check_lock(di->di_tv.v_lock, lp->ll_name, FALSE))
	செல் failed;

    lp->ll_tv = &di->di_tv;
    lp->ll_valtype = type;

success:
    rc = OK;

failed:
    *p = cc;
    திரும்பு rc == OK ? p : வெற்று;
}

வகைவரை எண்ணிக்கை {
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
    நிலையான முழு
get_lval_dict_item(
    lval_T	*lp,
    char_u	*name,
    char_u	*சாவி,
    முழு		len,
    char_u	**சாவி_end,
    typval_T	*var1,
    முழு		flags,
    முழு		unlet,
    typval_T	*rettv)
{
    முழு		quiet = flags & GLV_QUIET;
    char_u	*p = *சாவி_end;

    எனில் (len == -1)
    {
	// "[key]": get key from "var1"
	சாவி = tv_get_string_chk(var1);	// is number or string
	எனில் (சாவி == வெற்று)
	    திரும்பு GLV_FAIL;
    }
    lp->ll_list = வெற்று;
    lp->ll_list = வெற்று;
    lp->ll_blob = வெற்று;
    lp->ll_object = வெற்று;
    lp->ll_class = வெற்று;
    lp->ll_tuple = வெற்று;

    // a NULL dict is equivalent with an empty dict
    எனில் (lp->ll_tv->vval.v_dict == வெற்று)
    {
	lp->ll_tv->vval.v_dict = dict_alloc();
	எனில் (lp->ll_tv->vval.v_dict == வெற்று)
	    திரும்பு GLV_FAIL;
	++lp->ll_tv->vval.v_dict->dv_refcount;
    }
    lp->ll_dict = lp->ll_tv->vval.v_dict;

    lp->ll_di = dict_find(lp->ll_dict, சாவி, len);

    // When assigning to a scope dictionary check that a function and
    // variable name is valid (only variable name unless it is l: or
    // g: dictionary). Disallow overwriting a builtin function.
    எனில் (rettv != வெற்று && lp->ll_dict->dv_scope != 0)
    {
	முழு prevval;

	எனில் (len != -1)
	{
	    prevval = சாவி[len];
	    சாவி[len] = NUL;
	}
	இல்லையெனில்
	    prevval = 0; // avoid compiler warning
	முழு wrong = (lp->ll_dict->dv_scope == VAR_DEF_SCOPE
		&& (rettv->v_type == VAR_FUNC
		    || rettv->v_type == VAR_PARTIAL)
		&& var_wrong_func_name(சாவி, lp->ll_di == வெற்று))
	    || !valid_varname(சாவி, -1, TRUE);
	எனில் (len != -1)
	    சாவி[len] = prevval;
	எனில் (wrong)
	    திரும்பு GLV_FAIL;
    }

    எனில் (lp->ll_valtype != வெற்று)
	// use the type of the member
	lp->ll_valtype = lp->ll_valtype->tt_member;

    எனில் (lp->ll_di == வெற்று)
    {
	// Can't add "v:" or "a:" variable.
	எனில் (lp->ll_dict == get_vimvar_dict()
		|| &lp->ll_dict->dv_hashtab == get_funccal_args_ht())
	{
	    semsg(_(e_illegal_variable_name_str), name);
	    திரும்பு GLV_FAIL;
	}

	// Key does not exist in dict: may need to add it.
	எனில் (*p == '[' || *p == '.' || unlet)
	{
	    எனில் (!quiet)
		semsg(_(e_சாவி_not_present_in_dictionary_str), சாவி);
	    திரும்பு GLV_FAIL;
	}
	எனில் (len == -1)
	    lp->ll_newkey = vim_strsave(சாவி);
	இல்லையெனில்
	    lp->ll_newkey = vim_strnsave(சாவி, len);
	எனில் (lp->ll_newkey == வெற்று)
	    p = வெற்று;

	*சாவி_end = p;
	திரும்பு GLV_STOP;
    }
    // existing variable, need to check if it can be changed
    இல்லையெனில் எனில் ((flags & GLV_READ_ONLY) == 0
	    && (var_check_ro(lp->ll_di->di_flags, name, FALSE)
		|| var_check_lock(lp->ll_di->di_flags, name, FALSE)))
	திரும்பு GLV_FAIL;

    lp->ll_tv = &lp->ll_di->di_tv;

    திரும்பு GLV_OK;
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
    நிலையான முழு
get_lval_blob(
    lval_T	*lp,
    typval_T	*var1,
    typval_T	*var2,
    முழு		empty1,
    முழு		quiet)
{
    long	bloblen = blob_len(lp->ll_tv->vval.v_blob);

    lp->ll_list = வெற்று;
    lp->ll_dict = வெற்று;
    lp->ll_object = வெற்று;
    lp->ll_class = வெற்று;
    lp->ll_tuple = வெற்று;

    // Get the number and item for the only or first index of a List or Tuple.
    எனில் (empty1)
	lp->ll_n1 = 0;
    இல்லையெனில்
	// is number or string
	lp->ll_n1 = (long)tv_get_எண்(var1);

    எனில் (check_blob_index(bloblen, lp->ll_n1, quiet) == FAIL)
	திரும்பு FAIL;
    எனில் (lp->ll_range && !lp->ll_empty2)
    {
	lp->ll_n2 = (long)tv_get_எண்(var2);
	எனில் (check_blob_range(bloblen, lp->ll_n1, lp->ll_n2, quiet) == FAIL)
	    திரும்பு FAIL;
    }

    எனில் (!lp->ll_range)
	// Indexing a single byte in a blob.  So the rhs type is a
	// number.
	lp->ll_valtype = &t_எண்;

    lp->ll_blob = lp->ll_tv->vval.v_blob;
    lp->ll_tv = வெற்று;

    திரும்பு OK;
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
    நிலையான முழு
get_lval_list(
    lval_T	*lp,
    typval_T	*var1,
    typval_T	*var2,
    முழு		empty1,
    முழு		flags,
    முழு		quiet)
{
    /*
     * Get the number and item for the only or first index of the List.
     */
    எனில் (empty1)
	lp->ll_n1 = 0;
    இல்லையெனில்
	// is number or string
	lp->ll_n1 = (long)tv_get_எண்(var1);

    lp->ll_dict = வெற்று;
    lp->ll_object = வெற்று;
    lp->ll_class = வெற்று;
    lp->ll_tuple = வெற்று;
    lp->ll_list = lp->ll_tv->vval.v_list;
    lp->ll_li = check_range_index_one(lp->ll_list, &lp->ll_n1,
				(flags & GLV_ASSIGN_WITH_OP) == 0, quiet);
    எனில் (lp->ll_li == வெற்று)
	திரும்பு FAIL;

    எனில் (lp->ll_valtype != வெற்று && !lp->ll_range)
    {
	// use the type of the member
	எனில் (lp->ll_valtype->tt_member != வெற்று)
	    lp->ll_valtype = lp->ll_valtype->tt_member;
	இல்லையெனில்
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
    எனில் (lp->ll_range && !lp->ll_empty2)
    {
	lp->ll_n2 = (long)tv_get_எண்(var2);
	// is number or string
	எனில் (check_range_index_two(lp->ll_list,
			&lp->ll_n1, lp->ll_li, &lp->ll_n2, quiet) == FAIL)
	    திரும்பு FAIL;
    }

    lp->ll_tv = &lp->ll_li->li_tv;

    திரும்பு OK;
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
    நிலையான முழு
get_lval_tuple(
    lval_T	*lp,
    typval_T	*idx,
    முழு		quiet)
{
    // is number or string
    lp->ll_n1 = (long)tv_get_எண்(idx);

    lp->ll_list = வெற்று;
    lp->ll_dict = வெற்று;
    lp->ll_blob = வெற்று;
    lp->ll_object = வெற்று;
    lp->ll_class = வெற்று;

    lp->ll_tuple = lp->ll_tv->vval.v_tuple;
    lp->ll_tv = tuple_find(lp->ll_tuple, lp->ll_n1);
    எனில் (lp->ll_tv == வெற்று)
    {
	எனில் (!quiet)
	    semsg(_(e_tuple_index_out_of_range_nr), lp->ll_n1);
	திரும்பு GLV_FAIL;
    }

    // use the type of the member
    எனில் (lp->ll_valtype != வெற்று)
    {
	எனில் (lp->ll_valtype != வெற்று
		&& lp->ll_valtype->tt_type == VAR_TUPLE
		&& lp->ll_valtype->tt_argcount == 1)
	{
	    // a variadic tuple or a single item tuple
	    எனில் (lp->ll_valtype->tt_flags & TTFLAG_VARARGS)
		lp->ll_valtype = lp->ll_valtype->tt_args[0]->tt_member;
	    இல்லையெனில்
		lp->ll_valtype = lp->ll_valtype->tt_args[0];
	}
	இல்லையெனில்
	    // If the LHS member type is not known (VAR_ANY), then get it from
	    // the tuple item (after indexing)
	    lp->ll_valtype = typval2type(lp->ll_tv, get_copyID(),
					&lp->ll_type_list, TVTT_DO_MEMBER);
    }

    திரும்பு GLV_OK;
}

/*
 * Get a class or object lval method in class "cl".  The 'key' argument points
 * to the method name and 'key_end' points to the character after 'key'.
 * 'v_type' is VAR_CLASS or VAR_OBJECT.
 *
 * The method index, method function pointer and method type are returned in
 * "lp".
 */
    நிலையான முழு
get_lval_oc_method(
    lval_T	*lp,
    class_T	*cl,
    char_u	*சாவி,
    char_u	**சாவி_end,
    vartype_T	v_type)
{
    // Look for a method with this name.
    // round 1: class functions (skipped for an object)
    // round 2: object methods
    க்கு (முழு round = v_type == VAR_OBJECT ? 2 : 1; round <= 2; ++round)
    {
	முழு	m_idx;
	ufunc_T	*fp;

	fp = method_lookup(cl, round == 1 ? VAR_CLASS : VAR_OBJECT,
						சாவி, *சாவி_end - சாவி, &m_idx);
	lp->ll_oi = m_idx;

	// process generic method (if present)
	எனில் (fp && (fp = eval_generic_func(fp, சாவி, சாவி_end)) == வெற்று)
	    திரும்பு FAIL;

	எனில் (fp != வெற்று)
	{
	    lp->ll_ufunc = fp;
	    lp->ll_valtype = fp->uf_func_type;
	    நிறுத்து;
	}
    }

    திரும்பு OK;
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
    நிலையான முழு
get_lval_oc_variable(
    lval_T	*lp,
    class_T	*cl,
    char_u	*சாவி,
    char_u	*சாவி_end,
    vartype_T	v_type,
    class_T	*cl_exec,
    முழு		flags)
{
    முழு		m_idx;
    ocmember_T	*om;

    om = member_lookup(cl, v_type, சாவி, சாவி_end - சாவி, &m_idx);
    lp->ll_oi = m_idx;
    எனில் (om == வெற்று)
	திரும்பு OK;

    // Check variable is accessible
    எனில் (get_lval_check_access(cl_exec, cl, om, சாவி_end, flags) == FAIL)
	திரும்பு FAIL;

    // When lhs is used to modify the variable, check it is not a read-only
    // variable.
    எனில் ((flags & GLV_READ_ONLY) == 0 && (*சாவி_end != '.' && *சாவி_end != '[')
	    && oc_var_check_ro(cl, om))
	திரும்பு FAIL;

    lp->ll_valtype = om->ocm_type;

    எனில் (v_type == VAR_OBJECT)
	lp->ll_tv = ((typval_T *)(lp->ll_tv->vval.v_object + 1)) + m_idx;
    இல்லையெனில்
	lp->ll_tv = &cl->class_members_tv[m_idx];

    திரும்பு OK;
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
    நிலையான முழு
get_lval_class_or_obj(
    lval_T	*lp,
    char_u	*சாவி,
    char_u	**சாவி_end,
    vartype_T	v_type,
    class_T	*cl_exec,
    முழு		flags,
    முழு		quiet)
{
    lp->ll_dict = வெற்று;
    lp->ll_list = வெற்று;
    lp->ll_tuple = வெற்று;

    class_T *cl;
    எனில் (v_type == VAR_OBJECT)
    {
	எனில் (lp->ll_tv->vval.v_object == வெற்று)
	{
	    எனில் (!quiet)
		emsg(_(e_using_null_object));
	    திரும்பு FAIL;
	}
	cl = lp->ll_tv->vval.v_object->obj_class;
	lp->ll_object = lp->ll_tv->vval.v_object;
    }
    இல்லையெனில்
    {
	cl = lp->ll_tv->vval.v_class;
	lp->ll_object = வெற்று;
    }
    lp->ll_class = cl;

    எனில் (cl == வெற்று)
	// TODO: what if class is NULL?
	திரும்பு OK;

    lp->ll_valtype = வெற்று;

    எனில் (flags & GLV_PREFER_FUNC)
	எனில் (get_lval_oc_method(lp, cl, சாவி, சாவி_end, v_type) == FAIL)
	    திரும்பு FAIL;

    // Look for object/class member variable
    எனில் (lp->ll_valtype == வெற்று)
    {
	எனில் (get_lval_oc_variable(lp, cl, சாவி, *சாவி_end, v_type, cl_exec, flags)
								== FAIL)
	    திரும்பு FAIL;
    }

    எனில் (lp->ll_valtype == வெற்று)
    {
	member_not_found_செய்தி(cl, v_type, சாவி, *சாவி_end - சாவி);
	திரும்பு FAIL;
    }

    திரும்பு OK;
}

/*
 * Check whether dot (".") is allowed after the variable "name" with type
 * "v_type".  Only Dict, Class and Object types support a dot after the name.
 * Returns TRUE if dot is allowed after the name.
 */
    நிலையான முழு
dot_allowed_after_type(char_u *name, vartype_T v_type, முழு quiet)
{
    எனில் (v_type != VAR_DICT && v_type != VAR_OBJECT && v_type != VAR_CLASS)
    {
	எனில் (!quiet)
	    semsg(_(e_dot_not_allowed_after_str_str),
		    vartype_name(v_type), name);
	திரும்பு FALSE;
    }

    திரும்பு TRUE;
}

/*
 * Check whether left bracket ("[") is allowed after the variable "name" with
 * type "v_type".  Only Dict, List, Tuple and Blob types support a bracket
 * after the variable name.  Returns TRUE if bracket is allowed after the name.
 */
    நிலையான முழு
bracket_allowed_after_type(char_u *name, vartype_T v_type, முழு quiet)
{
    எனில் (v_type == VAR_CLASS || v_type == VAR_OBJECT)
    {
	எனில் (!quiet)
	    semsg(_(e_index_not_allowed_after_str_str),
		    vartype_name(v_type), name);
	திரும்பு FALSE;
    }

    திரும்பு TRUE;
}

/*
 * Check whether the variable "name" with type "v_type" can be followed by an
 * index.  Only Dict, List, Tuple, Blob, Object and Class types support
 * indexing.  Returns TRUE if indexing is allowed after the name.
 */
    நிலையான முழு
index_allowed_after_type(char_u *name, vartype_T v_type, முழு quiet)
{
    எனில் (v_type != VAR_LIST
	    && v_type != VAR_TUPLE
	    && v_type != VAR_DICT
	    && v_type != VAR_BLOB
	    && v_type != VAR_OBJECT
	    && v_type != VAR_CLASS)
    {
	எனில் (!quiet)
	    semsg(_(e_index_not_allowed_after_str_str),
		    vartype_name(v_type), name);
	திரும்பு FALSE;
    }

    திரும்பு TRUE;
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
    நிலையான char_u *
get_lval_subscript(
    lval_T	*lp,
    char_u	*p,
    char_u	*name,
    typval_T	*rettv,
    hashtab_T	*ht,
    dictitem_T	*v,
    முழு		unlet,
    முழு		flags,	    // GLV_ values
    class_T	*cl_exec)
{
    முழு		quiet = flags & GLV_QUIET;
    char_u	*சாவி = வெற்று;
    முழு		len;
    typval_T	var1;
    typval_T	var2;
    முழு		empty1 = FALSE;
    முழு		rc = FAIL;

    /*
     * Loop until no more [idx] or .key is following.
     */
    var1.v_type = VAR_UNKNOWN;
    var2.v_type = VAR_UNKNOWN;

    போது (*p == '[' || (*p == '.' && p[1] != '=' && p[1] != '.'))
    {
	vartype_T v_type = lp->ll_tv->v_type;

	எனில் (*p == '.' && !dot_allowed_after_type(name, v_type, quiet))
	    செல் done;

	எனில் (*p == '[' && !bracket_allowed_after_type(name, v_type, quiet))
	    செல் done;

	எனில் (!index_allowed_after_type(name, v_type, quiet))
	    செல் done;

	// A NULL list/blob works like an empty list/blob, allocate one now.
	முழு r = OK;
	எனில் (v_type == VAR_LIST && lp->ll_tv->vval.v_list == வெற்று)
	    r = rettv_list_alloc(lp->ll_tv);
	இல்லையெனில் எனில் (v_type == VAR_BLOB && lp->ll_tv->vval.v_blob == வெற்று)
	    r = rettv_blob_alloc(lp->ll_tv);
	எனில் (r == FAIL)
	    செல் done;

	எனில் (lp->ll_range)
	{
	    எனில் (!quiet)
		emsg(_(e_slice_must_come_last));
	    செல் done;
	}
#ifdef LOG_LOCKVAR
	ch_log(வெற்று, "LKVAR: get_lval() loop: p: %s, type: %s", p,
		vartype_name(v_type));
#endif

	எனில் (current_script_is_vim9() && lp->ll_valtype == வெற்று
		&& v != வெற்று
		&& lp->ll_tv == &v->di_tv
		&& ht != வெற்று && ht == get_script_local_ht())
	{
	    svar_T  *sv = find_typval_in_script(lp->ll_tv, 0, TRUE);

	    // Vim9 script local variable: get the type
	    எனில் (sv != வெற்று)
	    {
		lp->ll_valtype = sv->sv_type;
#ifdef LOG_LOCKVAR
		ch_log(வெற்று, "LKVAR:    ... loop: vim9 assign type: %s",
			vartype_name(lp->ll_valtype->tt_type));
#endif
	    }
	}

	len = -1;
	எனில் (*p == '.')
	{
	    சாவி = p + 1;

	    க்கு (len = 0; ASCII_ISALNUM(சாவி[len]) || சாவி[len] == '_'; ++len)
		;
	    எனில் (len == 0)
	    {
		எனில் (!quiet)
		    emsg(_(e_cannot_use_empty_சாவி_for_dictionary));
		செல் done;
	    }
	    p = சாவி + len;
	}
	இல்லையெனில்
	{
	    // Get the index [expr] or the first index [expr: ].
	    p = skipwhite(p + 1);
	    எனில் (*p == ':')
		empty1 = TRUE;
	    இல்லையெனில்
	    {
		empty1 = FALSE;
		எனில் (eval1(&p, &var1, &EVALARG_EVALUATE) == FAIL)  // recursive!
		    செல் done;
		எனில் (tv_get_string_chk(&var1) == வெற்று)
		    // not a number or string
		    செல் done;
		p = skipwhite(p);
	    }

	    // Optionally get the second index [ :expr].
	    எனில் (*p == ':')
	    {
		எனில் (v_type == VAR_DICT)
		{
		    எனில் (!quiet)
			emsg(_(e_cannot_slice_dictionary));
		    செல் done;
		}
		எனில் (v_type == VAR_TUPLE)
		{
		    எனில் (!quiet)
			emsg(_(e_cannot_slice_tuple));
		    செல் done;
		}
		எனில் (rettv != வெற்று
			&& !(rettv->v_type == VAR_LIST
			    && rettv->vval.v_list != வெற்று)
			&& !(rettv->v_type == VAR_BLOB
			    && rettv->vval.v_blob != வெற்று))
		{
		    எனில் (!quiet)
			emsg(_(e_slice_requires_list_or_blob_மதிப்பு));
		    செல் done;
		}
		p = skipwhite(p + 1);
		எனில் (*p == ']')
		    lp->ll_empty2 = TRUE;
		இல்லையெனில்
		{
		    lp->ll_empty2 = FALSE;
		    // recursive!
		    எனில் (eval1(&p, &var2, &EVALARG_EVALUATE) == FAIL)
			செல் done;
		    எனில் (tv_get_string_chk(&var2) == வெற்று)
			// not a number or string
			செல் done;
		}
		lp->ll_range = TRUE;
	    }
	    இல்லையெனில்
		lp->ll_range = FALSE;

	    எனில் (*p != ']')
	    {
		எனில் (!quiet)
		    emsg(_(e_missing_closing_square_brace));
		செல் done;
	    }

	    // Skip to past ']'.
	    ++p;
	}
#ifdef LOG_LOCKVAR
	எனில் (len == -1)
	    ch_log(வெற்று, "LKVAR:    ... loop: p: %s, '[' key: %s", p,
		    empty1 ? ":" : (எழுத்து*)tv_get_string(&var1));
	இல்லையெனில்
	    ch_log(வெற்று, "LKVAR:    ... loop: p: %s, '.' key: %s", p, சாவி);
#endif

	எனில் (v_type == VAR_DICT)
	{
	    glv_status_T glv_status;

	    glv_status = get_lval_dict_item(lp, name, சாவி, len, &p, &var1,
							flags, unlet, rettv);
	    எனில் (glv_status == GLV_FAIL)
		செல் done;
	    எனில் (glv_status == GLV_STOP)
		நிறுத்து;
	}
	இல்லையெனில் எனில் (v_type == VAR_BLOB)
	{
	    எனில் (get_lval_blob(lp, &var1, &var2, empty1, quiet) == FAIL)
		செல் done;

	    நிறுத்து;
	}
	இல்லையெனில் எனில் (v_type == VAR_LIST)
	{
	    எனில் (get_lval_list(lp, &var1, &var2, empty1, flags, quiet) == FAIL)
		செல் done;
	}
	இல்லையெனில் எனில் (v_type == VAR_TUPLE)
	{
	    எனில் (get_lval_tuple(lp, &var1, quiet) == FAIL)
		செல் done;
	}
	இல்லையெனில்  // v_type == VAR_CLASS || v_type == VAR_OBJECT
	{
	    எனில் (get_lval_class_or_obj(lp, சாவி, &p, v_type, cl_exec, flags,
							quiet) == FAIL)
		செல் done;
	}

	clear_tv(&var1);
	clear_tv(&var2);
	var1.v_type = VAR_UNKNOWN;
	var2.v_type = VAR_UNKNOWN;
    }

    எனில் (lp->ll_tuple != வெற்று && (flags & GLV_READ_ONLY) == 0)
    {
	எனில் (!quiet)
	    emsg(_(e_tuple_is_immutable));
	செல் done;
    }

    rc = OK;

done:
    clear_tv(&var1);
    clear_tv(&var2);
    திரும்பு rc == OK ? p : வெற்று;
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
    முழு		unlet,
    முழு		skip,
    முழு		flags,	    // GLV_ values
    முழு		fne_flags)  // flags for find_name_end()
{
    char_u	*p;
    char_u	*கோவை_start, *கோவை_end;
    முழு		cc;
    dictitem_T	*v = வெற்று;
    hashtab_T	*ht = வெற்று;
    முழு		quiet = flags & GLV_QUIET;
    முழு		writing = 0;
    முழு		vim9script = in_vim9script();
    class_T	*cl_exec = வெற்று;    // class that is executing, or NULL.

#ifdef LOG_LOCKVAR
    எனில் (lval_வேர் == வெற்று)
	ch_log(வெற்று, "LKVAR: get_lval(): name: %s, lval_root (nil)", name);
    இல்லையெனில்
	ch_log(வெற்று, "LKVAR: get_lval(): name: %s, lr_tv %p lr_is_arg %d",
			name, (வெறுமை*)lval_வேர்->lr_tv, lval_வேர்->lr_is_arg);
    எழுத்து buf[80];
    ch_log(வெற்று, "LKVAR:    ...: GLV flags: %s",
		    flags_tostring(flags, glv_flag_strings, buf, அளவு(buf)));
#endif

    // Clear everything in "lp".
    CLEAR_POINTER(lp);
    ga_init2(&lp->ll_type_list, அளவு(type_T *), 10);

    எனில் (skip || (flags & GLV_COMPILING))
    {
	// When skipping or compiling just find the end of the name.
	lp->ll_name = name;
	lp->ll_name_end = find_name_end(name, வெற்று, வெற்று,
						      FNE_INCL_BR | fne_flags);
	திரும்பு lp->ll_name_end;
    }

    // Cannot use "s:var" at the Vim9 script level.  "s: type" is OK.
    எனில் (vim9script && at_script_level()
		  && name[0] == 's' && name[1] == ':' && !VIM_ISWHITE(name[2]))
    {
	semsg(_(e_cannot_use_s_colon_in_vim9_script_str), name);
	திரும்பு வெற்று;
    }

    // Find the end of the name.
    p = find_name_end(name, &கோவை_start, &கோவை_end, fne_flags);
    lp->ll_name_end = p;
    எனில் (கோவை_start != வெற்று)
    {
	// Don't expand the name when we already know there is an error.
	எனில் (unlet && !VIM_ISWHITE(*p) && !ends_excmd(*p)
						    && *p != '[' && *p != '.')
	{
	    semsg(_(e_trailing_characters_str), p);
	    திரும்பு வெற்று;
	}

	lp->ll_exp_name = make_expanded_name(name, கோவை_start, கோவை_end, p);
	எனில் (lp->ll_exp_name == வெற்று)
	{
	    // Report an invalid expression in braces, unless the
	    // expression evaluation has been cancelled due to an
	    // aborting error, an interrupt, or an exception.
	    எனில் (!aborting() && !quiet)
	    {
		emsg_severe = TRUE;
		semsg(_(e_invalid_argument_str), name);
		திரும்பு வெற்று;
	    }
	}
	lp->ll_name = lp->ll_exp_name;
    }
    இல்லையெனில்
    {
	lp->ll_name = name;

	எனில் (vim9script)
	{
	    // "a: type" is declaring variable "a" with a type, not "a:".
	    // However, "g:[key]" is indexing a dictionary.
	    எனில் (p == name + 2 && p[-1] == ':' && *p != '[')
	    {
		--p;
		lp->ll_name_end = p;
	    }
	    எனில் (*skipwhite(p) == ':')
	    {
		char_u	    *tp = skipwhite(p + 1);

		எனில் (is_scoped_variable(name))
		{
		    semsg(_(e_cannot_use_type_with_this_variable_str), name);
		    திரும்பு வெற்று;
		}
		எனில் (VIM_ISWHITE(*p))
		{
		    semsg(_(e_no_white_space_allowed_before_colon_str), p);
		    திரும்பு வெற்று;
		}
		எனில் (tp == p + 1 && !quiet)
		{
		    semsg(_(e_white_space_required_after_str_str), ":", p);
		    திரும்பு வெற்று;
		}
		எனில் (!SCRIPT_ID_VALID(current_sctx.sc_sid))
		{
		    semsg(_(e_using_type_not_in_script_context_str), p);
		    திரும்பு வெற்று;
		}
		எனில் (vim9script && (flags & GLV_NO_DECL) &&
			!(flags & GLV_FOR_LOOP))
		{
		    // Using a type and not in a "var" declaration.
		    semsg(_(e_trailing_characters_str), p);
		    திரும்பு வெற்று;
		}


		// parse the type after the name
		lp->ll_type = parse_type(&tp,
			       &SCRIPT_ITEM(current_sctx.sc_sid)->sn_type_list,
			       வெற்று, வெற்று, !quiet);
		எனில் (!quiet && (lp->ll_type == வெற்று
			    || !valid_declaration_type(lp->ll_type)))
		    திரும்பு வெற்று;
		lp->ll_name_end = tp;
	    }
	    // TODO: check inside class?
	}
    }
    எனில் (lp->ll_name == வெற்று)
	திரும்பு p;

    எனில் (*p == '.')
    {
	// In legacy script, when a local variable and import exists with this name,
	// prioritize local variable over imports to avoid conflicts.
	முழு var_exists = FALSE;
	எனில் (!vim9script)
	{
	    cc = *p;
	    *p = NUL;
	    hashtab_T *local_ht = get_funccal_local_ht();
	    எனில் (local_ht != வெற்று)
	    {
		hashitem_T *hi = hash_find(local_ht, lp->ll_name);
		எனில் (!HASHITEM_EMPTY(hi))
		    var_exists = TRUE;
	    }
	    *p = cc;
	}

	எனில் (!var_exists)
	{
	    imported_T *import = find_imported(lp->ll_name, p - lp->ll_name, TRUE);
	    எனில் (import != வெற்று)
	    {
		p++;	// skip '.'
		p = get_lval_imported(lp, import->imp_sid, p, &v, fne_flags);
		எனில் (p == வெற்று)
		    திரும்பு வெற்று;
	    }
	}
    }

    // Without [idx] or .key we are done.
    எனில் (*p != '[' && *p != '.')
    {
	எனில் (lval_வேர் != வெற்று)
	    fill_lval_from_lval_வேர்(lp, lval_வேர்);
	திரும்பு p;
    }

    எனில் (vim9script && lval_வேர் != வெற்று)
	cl_exec = lval_வேர்->lr_cl_exec;
    எனில் (vim9script && lval_வேர் != வெற்று && lval_வேர்->lr_tv != வெற்று)
    {
	// using local variable
	lp->ll_tv = lval_வேர்->lr_tv;
	v = வெற்று;
    }
    இல்லையெனில் எனில் (lp->ll_tv == வெற்று)
    {
	cc = *p;
	*p = NUL;
	// When we would write to the variable pass &ht and prevent autoload.
	writing = !(flags & GLV_READ_ONLY);
	v = find_var(lp->ll_name, writing ? &ht : வெற்று,
					 (flags & GLV_NO_AUTOLOAD) || writing);
	எனில் (v == வெற்று && !quiet)
	    semsg(_(e_undefined_variable_str), lp->ll_name);
	*p = cc;
	எனில் (v == வெற்று)
	    திரும்பு வெற்று;
	lp->ll_tv = &v->di_tv;
    }

    எனில் (vim9script && (flags & GLV_NO_DECL) == 0)
    {
	எனில் (!quiet)
	    semsg(_(e_variable_already_declared_str), lp->ll_name);
	திரும்பு வெற்று;
    }

    // If the next character is a "." or a "[", then process the subitem.
    p = get_lval_subscript(lp, p, name, rettv, ht, v, unlet, flags, cl_exec);
    எனில் (p == வெற்று)
	திரும்பு வெற்று;

    எனில் (vim9script && lp->ll_valtype != வெற்று && rettv != வெற்று)
    {
	where_T	    where = WHERE_INIT;

	// In a Vim9 script, do type check and make sure the variable is
	// writable.
	எனில் (check_typval_type(lp->ll_valtype, rettv, where) == FAIL)
	    திரும்பு வெற்று;
    }

    lp->ll_name_end = p;
    திரும்பு p;
}

/*
 * Clear lval "lp" that was filled by get_lval().
 */
    வெறுமை
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
    வெறுமை
set_var_lval(
    lval_T	*lp,
    char_u	*endp,
    typval_T	*rettv,
    முழு		copy,
    முழு		flags,	    // ASSIGN_CONST, ASSIGN_NO_DECL
    char_u	*op,
    முழு		var_idx)    // index for "let [a, b] = list"
{
    முழு		cc;
    dictitem_T	*di;

    எனில் (lp->ll_tv == வெற்று)
    {
	cc = *endp;
	*endp = NUL;
	எனில் (in_vim9script() && check_reserved_name(lp->ll_name, FALSE) == FAIL)
	    திரும்பு;

	எனில் (lp->ll_blob != வெற்று)
	{
	    முழு	    error = FALSE;

	    எனில் (op != வெற்று && *op != '=')
	    {
		semsg(_(e_wrong_variable_type_for_str_equal), op);
		திரும்பு;
	    }
	    எனில் (மதிப்பு_check_lock(lp->ll_blob->bv_lock, lp->ll_name, FALSE))
		திரும்பு;

	    எனில் (lp->ll_range && rettv->v_type == VAR_BLOB)
	    {
		எனில் (lp->ll_empty2)
		    lp->ll_n2 = blob_len(lp->ll_blob) - 1;

		எனில் (blob_set_range(lp->ll_blob, lp->ll_n1, lp->ll_n2,
								rettv) == FAIL)
		    திரும்பு;
	    }
	    இல்லையெனில்
	    {
		varnumber_T	val = tv_get_எண்_chk(rettv, &error);
		எனில் (!error)
		{
		    எனில் (val < 0 || val > 255)
			semsg(_(e_invalid_மதிப்பு_for_blob_nr), val);
		    இல்லையெனில்
			blob_set_append(lp->ll_blob, lp->ll_n1, val);
		}
	    }
	}
	இல்லையெனில் எனில் (op != வெற்று && *op != '=')
	{
	    typval_T tv;

	    எனில் ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					     && (flags & ASSIGN_FOR_LOOP) == 0)
	    {
		emsg(_(e_cannot_modify_existing_variable));
		*endp = cc;
		திரும்பு;
	    }

	    // handle +=, -=, *=, /=, %= and .=
	    di = வெற்று;
	    எனில் (eval_variable(lp->ll_name, (முழு)STRLEN(lp->ll_name),
				 lp->ll_sid, &tv, &di, EVAL_VAR_VERBOSE) == OK)
	    {
		எனில் (di != வெற்று && check_typval_is_மதிப்பு(&di->di_tv) == FAIL)
		{
		    clear_tv(&tv);
		    திரும்பு;
		}

		// In Vim9 script a String v: variable only takes a String.
		எனில் (in_vim9script() && STRNCMP(lp->ll_name, "v:", 2) == 0
			&& tv.v_type == VAR_STRING)
		{
		    where_T	where = WHERE_INIT;

		    எனில் (check_typval_type(&t_string, rettv, where) == FAIL)
		    {
			clear_tv(&tv);
			*endp = cc;
			திரும்பு;
		    }
		}

		எனில் ((di == வெற்று
			 || (!var_check_ro(di->di_flags, lp->ll_name, FALSE)
			   && !tv_check_lock(&di->di_tv, lp->ll_name, FALSE)))
			&& tv_op(&tv, rettv, op) == OK)
		    set_var_const(lp->ll_name, lp->ll_sid, வெற்று, &tv, FALSE,
				ASSIGN_NO_DECL | ASSIGN_COMPOUND_OP, 0);
		clear_tv(&tv);
	    }
	}
	இல்லையெனில்
	{
	    எனில் (lp->ll_type != வெற்று && check_typval_arg_type(lp->ll_type, rettv,
							      வெற்று, 0) == FAIL)
		திரும்பு;
	    set_var_const(lp->ll_name, lp->ll_sid, lp->ll_type, rettv, copy,
							       flags, var_idx);
	}
	*endp = cc;
    }
    இல்லையெனில் எனில் (மதிப்பு_check_lock(lp->ll_newkey == வெற்று
		? lp->ll_tv->v_lock
		: lp->ll_tv->vval.v_dict->dv_lock, lp->ll_name, FALSE))
	;
    இல்லையெனில் எனில் (lp->ll_range)
    {
	எனில் ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					     && (flags & ASSIGN_FOR_LOOP) == 0)
	{
	    emsg(_(e_cannot_lock_range));
	    திரும்பு;
	}

	(வெறுமை)list_assign_range(lp->ll_list, rettv->vval.v_list,
			 lp->ll_n1, lp->ll_n2, lp->ll_empty2, op, lp->ll_name);
    }
    இல்லையெனில்
    {
	/*
	 * Assign to a List, Dictionary or Object item.
	 */
	எனில் ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					     && (flags & ASSIGN_FOR_LOOP) == 0)
	{
	    emsg(_(e_cannot_lock_list_or_dict));
	    திரும்பு;
	}

	எனில் (lp->ll_valtype != வெற்று
		    && check_typval_arg_type(lp->ll_valtype, rettv,
							      வெற்று, 0) == FAIL)
	{
	    lp->ll_name_end = வெற்று;
	    திரும்பு;
	}

	// If the lval is a List and the type of the list is not yet set,
	// then set the item type from the declared type of the variable.
	எனில் (in_vim9script() && rettv->v_type == VAR_LIST
				&& rettv->vval.v_list != வெற்று
				&& rettv->vval.v_list->lv_type == வெற்று)
	{
	    எனில் (lp->ll_tv->v_type == VAR_LIST
		    && lp->ll_tv->vval.v_list != வெற்று
		    && lp->ll_tv->vval.v_list->lv_type != வெற்று)
		set_tv_type(rettv, lp->ll_tv->vval.v_list->lv_type);
	}

	எனில் (lp->ll_newkey != வெற்று)
	{
	    எனில் (op != வெற்று && *op != '=')
	    {
		semsg(_(e_சாவி_not_present_in_dictionary_str), lp->ll_newkey);
		திரும்பு;
	    }
	    எனில் (dict_wrong_func_name(lp->ll_tv->vval.v_dict, rettv,
								lp->ll_newkey))
		திரும்பு;

	    // Need to add an item to the Dictionary.
	    di = dictitem_alloc(lp->ll_newkey);
	    எனில் (di == வெற்று)
		திரும்பு;
	    எனில் (dict_add(lp->ll_tv->vval.v_dict, di) == FAIL)
	    {
		vim_free(di);
		திரும்பு;
	    }
	    lp->ll_tv = &di->di_tv;
	}
	இல்லையெனில் எனில் (op != வெற்று && *op != '=')
	{
	    tv_op(lp->ll_tv, rettv, op);
	    திரும்பு;
	}
	இல்லையெனில்
	    clear_tv(lp->ll_tv);

	/*
	 * Assign the value to the variable or list item.
	 */
	எனில் (copy)
	    copy_tv(rettv, lp->ll_tv);
	இல்லையெனில்
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
    நிலையான முழு
tv_op_blob(typval_T *tv1, typval_T *tv2, char_u *op)
{
    எனில் (*op != '+' || tv2->v_type != VAR_BLOB)
	திரும்பு FAIL;

    // Blob += Blob
    எனில் (tv2->vval.v_blob == வெற்று)
	திரும்பு OK;

    எனில் (tv1->vval.v_blob == வெற்று)
    {
	tv1->vval.v_blob = tv2->vval.v_blob;
	++tv1->vval.v_blob->bv_refcount;
	திரும்பு OK;
    }

    blob_T	*b1 = tv1->vval.v_blob;
    blob_T	*b2 = tv2->vval.v_blob;
    முழு		len = blob_len(b2);

    எனில் (len > 0 && ga_grow(&b1->bv_ga, len) == OK)
    {
	mch_memmove((char_u *)b1->bv_ga.ga_data + b1->bv_ga.ga_len,
		    (char_u *)b2->bv_ga.ga_data, (size_t)len);
	b1->bv_ga.ga_len += len;
    }
    இல்லையெனில்
	க்கு (முழு i = 0; i < len; i++)
	    (வெறுமை)ga_append(&b1->bv_ga, blob_get(b2, i));

    திரும்பு OK;
}

/*
 * Handle "list1 += list2".
 * Returns OK or FAIL.
 */
    நிலையான முழு
tv_op_list(typval_T *tv1, typval_T *tv2, char_u *op)
{
    எனில் (*op != '+' || tv2->v_type != VAR_LIST)
	திரும்பு FAIL;

    // List += List
    எனில் (tv2->vval.v_list == வெற்று)
	திரும்பு OK;

    எனில் (tv1->vval.v_list == வெற்று)
    {
	tv1->vval.v_list = tv2->vval.v_list;
	++tv1->vval.v_list->lv_refcount;
    }
    இல்லையெனில்
	list_extend(tv1->vval.v_list, tv2->vval.v_list, வெற்று);

    திரும்பு OK;
}

/*
 * Handle number operations:
 *	nr += nr , nr -= nr , nr *=nr , nr /= nr , nr %= nr
 *
 * Returns OK or FAIL.
 */
    நிலையான முழு
tv_op_எண்(typval_T *tv1, typval_T *tv2, char_u *op)
{
    varnumber_T	n;
    முழு		failed = FALSE;

    n = tv_get_எண்(tv1);
    எனில் (tv2->v_type == VAR_FLOAT)
    {
	float_T f = n;

	எனில் (*op == '%')
	    திரும்பு FAIL;
	தேர்வு (*op)
	{
	    நிலை '+': f += tv2->vval.v_float; நிறுத்து;
	    நிலை '-': f -= tv2->vval.v_float; நிறுத்து;
	    நிலை '*': f *= tv2->vval.v_float; நிறுத்து;
	    நிலை '/': f /= tv2->vval.v_float; நிறுத்து;
	}
	clear_tv(tv1);
	tv1->v_type = VAR_FLOAT;
	tv1->vval.v_float = f;
    }
    இல்லையெனில்
    {
	தேர்வு (*op)
	{
	    நிலை '+': n += tv_get_எண்(tv2); நிறுத்து;
	    நிலை '-': n -= tv_get_எண்(tv2); நிறுத்து;
	    நிலை '*': n *= tv_get_எண்(tv2); நிறுத்து;
	    நிலை '/': n = இலக்கம்_divide(n, tv_get_எண்(tv2), &failed); நிறுத்து;
	    நிலை '%': n = இலக்கம்_modulus(n, tv_get_எண்(tv2), &failed); நிறுத்து;
	}
	clear_tv(tv1);
	tv1->v_type = VAR_எண்;
	tv1->vval.v_எண் = n;
    }

    திரும்பு failed ? FAIL : OK;
}

/*
 * Append string "s2" to the string in "tv1".
 * Returns OK if "tv1" was grown in place, FAIL otherwise.
 */
    நிலையான முழு
grow_string_tv(typval_T *tv1, char_u *s2)
{
    எனில் (tv1->v_type != VAR_STRING || tv1->vval.v_string == வெற்று)
	திரும்பு FAIL;

    size_t len1 = STRLEN(tv1->vval.v_string);
    size_t len2 = STRLEN(s2);
    char_u *p = vim_realloc(tv1->vval.v_string, len1 + len2 + 1);

    எனில் (p == வெற்று)
	திரும்பு FAIL;

    mch_memmove(p + len1, s2, len2 + 1);
    tv1->vval.v_string = p;
    திரும்பு OK;
}

/*
 * Handle "str1 .= str2"
 * Returns OK or FAIL.
 */
    நிலையான முழு
tv_op_string(typval_T *tv1, typval_T *tv2, char_u *op UNUSED)
{
    char_u	numbuf[NUMBUFLEN];
    char_u	*s;
    char_u	*s2;

    எனில் (tv2->v_type == VAR_FLOAT)
	திரும்பு FAIL;

    // str .= str
    s2 = tv_get_string_buf(tv2, numbuf);
    எனில் (grow_string_tv(tv1, s2) == OK)
	திரும்பு OK;

    s = tv_get_string(tv1);
    s = concat_str(s, s2);
    clear_tv(tv1);
    tv1->v_type = VAR_STRING;
    tv1->vval.v_string = s;

    திரும்பு OK;
}

/*
 * Handle "tv1 += tv2", "tv1 -= tv2", "tv1 *= tv2", "tv1 /= tv2", "tv1 %= tv2"
 * and "tv1 .= tv2"
 * Returns OK or FAIL.
 */
    நிலையான முழு
tv_op_nr_or_string(typval_T *tv1, typval_T *tv2, char_u *op)
{
    எனில் (tv2->v_type == VAR_LIST)
	திரும்பு FAIL;

    எனில் (vim_strchr((char_u *)"+-*/%", *op) != வெற்று)
	திரும்பு tv_op_எண்(tv1, tv2, op);

    திரும்பு tv_op_string(tv1, tv2, op);
}

/*
 * Handle "f1 += f2", "f1 -= f2", "f1 *= f2", "f1 /= f2".
 * Returns OK or FAIL.
 */
    நிலையான முழு
tv_op_float(typval_T *tv1, typval_T *tv2, char_u *op)
{
    float_T f;

    எனில் (*op == '%' || *op == '.'
	    || (tv2->v_type != VAR_FLOAT
		&& tv2->v_type != VAR_எண்
		&& tv2->v_type != VAR_STRING))
	திரும்பு FAIL;

    எனில் (tv2->v_type == VAR_FLOAT)
	f = tv2->vval.v_float;
    இல்லையெனில்
	f = tv_get_எண்(tv2);
    தேர்வு (*op)
    {
	நிலை '+': tv1->vval.v_float += f; நிறுத்து;
	நிலை '-': tv1->vval.v_float -= f; நிறுத்து;
	நிலை '*': tv1->vval.v_float *= f; நிறுத்து;
	நிலை '/': tv1->vval.v_float /= f; நிறுத்து;
    }

    திரும்பு OK;
}

/*
 * Handle "tv1 += tv2", "tv1 -= tv2", "tv1 *= tv2", "tv1 /= tv2", "tv1 %= tv2"
 * and "tv1 .= tv2"
 * Returns OK or FAIL.
 */
    முழு
tv_op(typval_T *tv1, typval_T *tv2, char_u *op)
{
    // Can't do anything with a Funcref or Dict on the right.
    // v:true and friends only work with "..=".
    எனில் (tv2->v_type == VAR_FUNC || tv2->v_type == VAR_DICT
		|| ((tv2->v_type == VAR_BOOL || tv2->v_type == VAR_SPECIAL)
							&& *op != '.'))
    {
	semsg(_(e_wrong_variable_type_for_str_equal), op);
	திரும்பு FAIL;
    }

    முழு retval = FAIL;
    தேர்வு (tv1->v_type)
    {
	நிலை VAR_UNKNOWN:
	நிலை VAR_ANY:
	நிலை VAR_VOID:
	நிலை VAR_DICT:
	நிலை VAR_FUNC:
	நிலை VAR_PARTIAL:
	நிலை VAR_BOOL:
	நிலை VAR_SPECIAL:
	நிலை VAR_JOB:
	நிலை VAR_CHANNEL:
	நிலை VAR_INSTR:
	நிலை VAR_OBJECT:
	நிலை VAR_CLASS:
	நிலை VAR_TYPEALIAS:
	நிலை VAR_TUPLE:
	    நிறுத்து;

	நிலை VAR_BLOB:
	    retval = tv_op_blob(tv1, tv2, op);
	    நிறுத்து;

	நிலை VAR_LIST:
	    retval = tv_op_list(tv1, tv2, op);
	    நிறுத்து;

	நிலை VAR_எண்:
	நிலை VAR_STRING:
	    retval = tv_op_nr_or_string(tv1, tv2, op);
	    நிறுத்து;

	நிலை VAR_FLOAT:
	    retval = tv_op_float(tv1, tv2, op);
	    நிறுத்து;
    }

    எனில் (retval != OK)
	semsg(_(e_wrong_variable_type_for_str_equal), op);

    திரும்பு retval;
}

/*
 * Evaluate the expression used in a ":for var in expr" command.
 * "arg" points to "var".
 * Set "*errp" to TRUE for an error, FALSE otherwise;
 * Return a pointer that holds the info.  Null when there is an error.
 */
    வெறுமை *
eval_for_வரி(
    char_u	*arg,
    முழு		*errp,
    exarg_T	*eap,
    evalarg_T	*evalarg)
{
    forinfo_T	*fi;
    char_u	*var_list_end;
    char_u	*கோவை;
    typval_T	tv;
    list_T	*l;
    tuple_T	*tuple;
    முழு		skip = !(evalarg->eval_flags & EVAL_EVALUATE);

    *errp = TRUE;	// default: there is an error

    fi = ALLOC_CLEAR_ONE(forinfo_T);
    எனில் (fi == வெற்று)
	திரும்பு வெற்று;

    var_list_end = skip_var_list(arg, TRUE, &fi->fi_varcount,
						     &fi->fi_semicolon, FALSE);
    எனில் (var_list_end == வெற்று)
	திரும்பு fi;

    கோவை = skipwhite_and_linebreak(var_list_end, evalarg);
    எனில் (கோவை[0] != 'i' || கோவை[1] != 'n'
				  || !(கோவை[2] == NUL || VIM_ISWHITE(கோவை[2])))
    {
	எனில் (in_vim9script() && *கோவை == ':' && கோவை != var_list_end)
	    semsg(_(e_no_white_space_allowed_before_colon_str), கோவை);
	இல்லையெனில்
	    emsg(_(e_missing_in_after_for));
	திரும்பு fi;
    }

    எனில் (skip)
	++emsg_skip;
    கோவை = skipwhite_and_linebreak(கோவை + 2, evalarg);
    எனில் (eval0(கோவை, &tv, eap, evalarg) == OK)
    {
	*errp = FALSE;
	எனில் (!skip)
	{
	    எனில் (tv.v_type == VAR_LIST)
	    {
		l = tv.vval.v_list;
		எனில் (l == வெற்று)
		{
		    // a null list is like an empty list: do nothing
		    clear_tv(&tv);
		}
		இல்லையெனில்
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
	    இல்லையெனில் எனில் (tv.v_type == VAR_TUPLE)
	    {
		tuple = tv.vval.v_tuple;
		எனில் (tuple == வெற்று)
		{
		    // a null tuple is like an empty tuple: do nothing
		    clear_tv(&tv);
		}
		இல்லையெனில்
		{
		    // No need to increment the refcount, it's already set for
		    // the tuple being used in "tv".
		    fi->fi_tuple = tuple;
		    fi->fi_tuple_idx = 0;
		}
	    }
	    இல்லையெனில் எனில் (tv.v_type == VAR_BLOB)
	    {
		fi->fi_bi = 0;
		எனில் (tv.vval.v_blob != வெற்று)
		{
		    // Make a copy, so that the iteration still works when the
		    // blob is changed.
		    fi->fi_blob = blob_copy(tv.vval.v_blob);
		}
		clear_tv(&tv);
	    }
	    இல்லையெனில் எனில் (tv.v_type == VAR_STRING)
	    {
		fi->fi_byte_idx = 0;
		fi->fi_string = tv.vval.v_string;
		tv.vval.v_string = வெற்று;
		எனில் (fi->fi_string == வெற்று)
		    fi->fi_string = vim_strsave((char_u *)"");
	    }
	    இல்லையெனில்
	    {
		emsg(_(e_string_list_tuple_or_blob_required));
		clear_tv(&tv);
	    }
	}
    }
    எனில் (skip)
	--emsg_skip;
    fi->fi_break_மொத்தம் = evalarg->eval_break_மொத்தம்;

    திரும்பு fi;
}

/*
 * Used when looping over a :for line, skip the "in expr" part.
 */
    வெறுமை
skip_for_வரிகள்(வெறுமை *fi_void, evalarg_T *evalarg)
{
    forinfo_T	*fi = (forinfo_T *)fi_void;
    முழு		i;

    க்கு (i = 0; i < fi->fi_break_மொத்தம்; ++i)
	eval_next_வரி(வெற்று, evalarg);
}

/*
 * Use the first item in a ":for" list.  Advance to the next.
 * Assign the values to the variable (list).  "arg" points to the first one.
 * Return TRUE when a valid item was found, FALSE when at end of list or
 * something wrong.
 */
    முழு
next_for_item(வெறுமை *fi_void, char_u *arg)
{
    forinfo_T	*fi = (forinfo_T *)fi_void;
    முழு		result;
    முழு		flag = ASSIGN_FOR_LOOP | (in_vim9script()
			 ? (ASSIGN_FINAL
			     // first round: error if variable exists
			     | (fi->fi_bi == 0 ? 0 : ASSIGN_DECL)
			     | ASSIGN_NO_MEMBER_TYPE
			     | ASSIGN_UPDATE_BLOCK_ID)
			 : 0);
    listitem_T	*item;
    முழு		skip_assign = in_vim9script() && arg[0] == '_'
						      && !eval_isnamec(arg[1]);

    எனில் (fi->fi_blob != வெற்று)
    {
	typval_T	tv;

	எனில் (fi->fi_bi >= blob_len(fi->fi_blob))
	    திரும்பு FALSE;
	tv.v_type = VAR_எண்;
	tv.v_lock = VAR_FIXED;
	tv.vval.v_எண் = blob_get(fi->fi_blob, fi->fi_bi);
	++fi->fi_bi;
	எனில் (skip_assign)
	    திரும்பு TRUE;
	திரும்பு ex_let_vars(arg, &tv, TRUE, fi->fi_semicolon,
					    fi->fi_varcount, flag, வெற்று) == OK;
    }

    எனில் (fi->fi_string != வெற்று)
    {
	typval_T	tv;
	முழு		len;

	len = mb_ptr2len(fi->fi_string + fi->fi_byte_idx);
	எனில் (len == 0)
	    திரும்பு FALSE;
	tv.v_type = VAR_STRING;
	tv.v_lock = VAR_FIXED;
	tv.vval.v_string = vim_strnsave(fi->fi_string + fi->fi_byte_idx, len);
	fi->fi_byte_idx += len;
	++fi->fi_bi;
	எனில் (skip_assign)
	    result = TRUE;
	இல்லையெனில்
	    result = ex_let_vars(arg, &tv, TRUE, fi->fi_semicolon,
					    fi->fi_varcount, flag, வெற்று) == OK;
	vim_free(tv.vval.v_string);
	திரும்பு result;
    }

    எனில் (fi->fi_tuple != வெற்று)
    {
	typval_T	tv;

	எனில் (fi->fi_tuple_idx >= TUPLE_LEN(fi->fi_tuple))
	    திரும்பு FALSE;

	copy_tv(TUPLE_ITEM(fi->fi_tuple, fi->fi_tuple_idx), &tv);
	++fi->fi_tuple_idx;
	++fi->fi_bi;
	எனில் (skip_assign)
	    result = TRUE;
	இல்லையெனில்
	    result = ex_let_vars(arg, &tv, TRUE, fi->fi_semicolon,
					    fi->fi_varcount, flag, வெற்று) == OK;
	clear_tv(&tv);
	திரும்பு result;
    }

    item = fi->fi_lw.lw_item;
    எனில் (item == வெற்று)
	result = FALSE;
    இல்லையெனில்
    {
	fi->fi_lw.lw_item = item->li_next;
	++fi->fi_bi;
	எனில் (skip_assign)
	    result = TRUE;
	இல்லையெனில்
	    result = (ex_let_vars(arg, &item->li_tv, TRUE, fi->fi_semicolon,
					   fi->fi_varcount, flag, வெற்று) == OK);
    }
    திரும்பு result;
}

/*
 * Free the structure used to store info used by ":for".
 */
    வெறுமை
free_for_info(வெறுமை *fi_void)
{
    forinfo_T    *fi = (forinfo_T *)fi_void;

    எனில் (fi == வெற்று)
	திரும்பு;
    எனில் (fi->fi_list != வெற்று)
    {
	list_rem_watch(fi->fi_list, &fi->fi_lw);
	list_unref(fi->fi_list);
    }
    இல்லையெனில் எனில் (fi->fi_blob != வெற்று)
	blob_unref(fi->fi_blob);
    இல்லையெனில் எனில் (fi->fi_tuple != வெற்று)
	tuple_unref(fi->fi_tuple);
    இல்லையெனில்
	vim_free(fi->fi_string);
    vim_free(fi);
}

    வெறுமை
set_context_for_expression(
    expand_T	*xp,
    char_u	*arg,
    cmdidx_T	cmdidx)
{
    முழு		has_கோவை = cmdidx != CMD_let && cmdidx != CMD_var;
    முழு		c;
    char_u	*p;

    எனில் (cmdidx == CMD_let || cmdidx == CMD_var
				 || cmdidx == CMD_const || cmdidx == CMD_final)
    {
	xp->xp_context = EXPAND_USER_VARS;
	எனில் (vim_strpbrk(arg, (char_u *)"\"'+-*/%.=!?~|&$([<>,#") == வெற்று)
	{
	    // ":let var1 var2 ...": find last space.
	    க்கு (p = arg + STRLEN(arg); p >= arg; )
	    {
		xp->xp_pattern = p;
		MB_PTR_BACK(arg, p);
		எனில் (VIM_ISWHITE(*p))
		    நிறுத்து;
	    }
	    திரும்பு;
	}
    }
    இல்லையெனில்
	xp->xp_context = cmdidx == CMD_call ? EXPAND_FUNCTIONS
							  : EXPAND_EXPRESSION;
    போது ((xp->xp_pattern = vim_strpbrk(arg,
				  (char_u *)"\"'+-*/%.=!?~|&$([<>,#")) != வெற்று)
    {
	c = *xp->xp_pattern;
	எனில் (c == '&')
	{
	    c = xp->xp_pattern[1];
	    எனில் (c == '&')
	    {
		++xp->xp_pattern;
		xp->xp_context = has_கோவை ? EXPAND_EXPRESSION : EXPAND_NOTHING;
	    }
	    இல்லையெனில் எனில் (c != ' ')
	    {
		xp->xp_context = EXPAND_SETTINGS;
		எனில் ((c == 'l' || c == 'g') && xp->xp_pattern[2] == ':')
		    xp->xp_pattern += 2;

	    }
	}
	இல்லையெனில் எனில் (c == '$')
	{
	    // environment variable
	    xp->xp_context = EXPAND_ENV_VARS;
	}
	இல்லையெனில் எனில் (c == '=')
	{
	    has_கோவை = TRUE;
	    xp->xp_context = EXPAND_EXPRESSION;
	}
	இல்லையெனில் எனில் (c == '#'
		&& xp->xp_context == EXPAND_EXPRESSION)
	{
	    // Autoload function/variable contains '#'.
	    நிறுத்து;
	}
	இல்லையெனில் எனில் ((c == '<' || c == '#')
		&& xp->xp_context == EXPAND_FUNCTIONS
		&& vim_strchr(xp->xp_pattern, '(') == வெற்று)
	{
	    // Function name can start with "<SNR>" and contain '#'.
	    நிறுத்து;
	}
	இல்லையெனில் எனில் (has_கோவை)
	{
	    எனில் (c == '"')	    // string
	    {
		போது ((c = *++xp->xp_pattern) != NUL && c != '"')
		    எனில் (c == '\\' && xp->xp_pattern[1] != NUL)
			++xp->xp_pattern;
		xp->xp_context = EXPAND_NOTHING;
	    }
	    இல்லையெனில் எனில் (c == '\'')	    // literal string
	    {
		// Trick: '' is like stopping and starting a literal string.
		போது ((c = *++xp->xp_pattern) != NUL && c != '\'')
		    /* skip */ ;
		xp->xp_context = EXPAND_NOTHING;
	    }
	    இல்லையெனில் எனில் (c == '|')
	    {
		எனில் (xp->xp_pattern[1] == '|')
		{
		    ++xp->xp_pattern;
		    xp->xp_context = EXPAND_EXPRESSION;
		}
		இல்லையெனில்
		    xp->xp_context = EXPAND_COMMANDS;
	    }
	    இல்லையெனில்
		xp->xp_context = EXPAND_EXPRESSION;
	}
	இல்லையெனில்
	    // Doesn't look like something valid, expand as an expression
	    // anyway.
	    xp->xp_context = EXPAND_EXPRESSION;
	arg = xp->xp_pattern;
	எனில் (*arg != NUL)
	    போது ((c = *++arg) != NUL && (c == ' ' || c == '\t'))
		/* skip */ ;
    }

    // ":exe one two" completes "two"
    எனில் ((cmdidx == CMD_execute
		|| cmdidx == CMD_echo
		|| cmdidx == CMD_echon
		|| cmdidx == CMD_echomsg
		|| cmdidx == CMD_echoerr
		|| cmdidx == CMD_echoconsole
		|| cmdidx == CMD_echowindow)
	    && xp->xp_context == EXPAND_EXPRESSION)
    {
	க்கு (;;)
	{
	    char_u *n = skiptowhite(arg);

	    எனில் (n == arg || IS_WHITE_OR_NUL(*skipwhite(n)))
		நிறுத்து;
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
நிலையான regprog_T    *eval_prog_cache = வெற்று;
நிலையான char_u	    *eval_prog_pat = வெற்று;	// pattern it was compiled for
நிலையான char_u	    *eval_prog_enc = வெற்று;	// 'encoding' when compiled
நிலையான long	    eval_prog_re;		// 'regexpengine' when compiled
// State sampled by eval_regcomp() for the eval_regfree() call that gives the
// program back: user code run in between, e.g. an object's string() method
// invoked by match() on a list, can change these options, and then the
// program must not be cached under the changed values.  eval_compile_enc is
// only ever compared against p_enc, never dereferenced.
நிலையான long	    eval_compile_re;
நிலையான char_u	    *eval_compile_enc;
நிலையான முழு	    eval_prog_busy = 0;	// programs eval_regcomp() handed out
					// that were not given back yet
நிலையான முழு	    eval_prog_reentered = FALSE;  // eval_regcomp() ran while
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
    நிலையான bool
eval_prog_volatile(char_u *pat)
{
    char_u *p;

    எனில் (STRNCMP(pat, "\\%#=", 4) == 0)
	திரும்பு true;
    க்கு (p = pat; *p != NUL; ++p)
	எனில் (*p == '~'
		|| (p[0] == '[' && (p[1] == ':' || p[1] == '=' || p[1] == '.'))
		|| (p[0] == '%' && (p[1] == '.'
			|| ((p[1] == '<' || p[1] == '>') && p[2] == '.'))))
	    திரும்பு true;
    திரும்பு false;
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
    எனில் (eval_prog_busy > 0)
	// The samples above no longer describe the program already handed
	// out: its eval_regfree() must drop it instead of caching it.
	eval_prog_reentered = TRUE;

    எனில் (eval_prog_cache != வெற்று
	    && *p_cpo == NUL
	    && eval_prog_cache->re_flags == RE_MAGIC + RE_STRING
	    && eval_prog_re == p_re
	    && STRCMP(eval_prog_pat, pat) == 0
	    && STRCMP(eval_prog_enc, p_enc) == 0)
    {
	prog = eval_prog_cache;
	// The caller now owns the program, eval_regfree() adopts the
	// program the caller ends up with.
	eval_prog_cache = வெற்று;
    }
    இல்லையெனில்
	prog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
    எனில் (prog != வெற்று)
	++eval_prog_busy;	// paired with the eval_regfree() call
    திரும்பு prog;
}

/*
 * Free program "prog", obtained with eval_regcomp() for pattern "pat", by
 * keeping it in the cache for the next use.  "pat" must be the same string
 * the program was compiled for and must still be valid.
 */
    வெறுமை
eval_regfree(char_u *pat, regprog_T *prog)
{
    முழு	    reentered = eval_prog_reentered;

    எனில் (eval_prog_busy > 0 && --eval_prog_busy == 0)
	eval_prog_reentered = FALSE;
    எனில் (prog == வெற்று)
	திரும்பு;
    எனில் (reentered || *p_cpo != NUL
	    || eval_compile_re != p_re || eval_compile_enc != p_enc)
    {
	// 'cpoptions' is not empty, an option in the cache key changed, or
	// another program was handed out while the caller was using this
	// one: the samples may not describe this program, do not cache it.
	vim_regfree(prog);
	திரும்பு;
    }
    எனில் (eval_prog_pat == வெற்று || STRCMP(eval_prog_pat, pat) != 0
	    || STRCMP(eval_prog_enc, p_enc) != 0)
    {
	char_u	*pat_copy;
	char_u	*enc_copy;

	// A cached pattern is known not to be volatile, thus the scan is
	// only needed when installing a new key.
	எனில் (eval_prog_volatile(pat))
	{
	    // compiling depends on state the cache key does not cover
	    vim_regfree(prog);
	    திரும்பு;
	}
	pat_copy = vim_strsave(pat);
	enc_copy = vim_strsave(p_enc);
	எனில் (pat_copy == வெற்று || enc_copy == வெற்று)
	{
	    vim_free(pat_copy);
	    vim_free(enc_copy);
	    vim_regfree(prog);
	    திரும்பு;
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

#எனில் defined(EXITFREE)
    வெறுமை
free_eval_regcomp_cache(வெறுமை)
{
    vim_regfree(eval_prog_cache);
    eval_prog_cache = வெற்று;
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
    முழு
pattern_match(char_u *pat, char_u *text, முழு ic)
{
    முழு		matches = FALSE;
    char_u	*save_cpo;
    regmatch_T	regmatch;

    // avoid 'l' flag in 'cpoptions'
    save_cpo = p_cpo;
    p_cpo = empty_option;
    regmatch.regprog = eval_regcomp(pat);
    எனில் (regmatch.regprog != வெற்று)
    {
	regmatch.rm_ic = ic;
	matches = vim_regexec_nl(&regmatch, text, (colnr_T)0);
	eval_regfree(pat, regmatch.regprog);
    }
    p_cpo = save_cpo;
    திரும்பு matches;
}

/*
 * Handle a name followed by "(".  Both for just "name(arg)" and for
 * "expr->name(arg)".
 * Returns OK or FAIL.
 */
    நிலையான முழு
eval_func(
	char_u	    **arg,	// points to "(", will be advanced
	evalarg_T   *evalarg,
	char_u	    *name,
	முழு	    name_len,
	typval_T    *rettv,
	முழு	    flags,
	typval_T    *basetv)	// "expr" for "expr->name(arg)"
{
    முழு		evaluate = flags & EVAL_EVALUATE;
    char_u	*s = name;
    முழு		len = name_len;
    partial_T	*partial;
    முழு		ret = OK;
    type_T	*type = வெற்று;
    முழு		found_var = FALSE;

    எனில் (!evaluate)
	check_vars(s, len);

    // If "s" is the name of a variable of type VAR_FUNC
    // use its contents.
    s = deref_func_name(s, &len, &partial,
		 in_vim9script() ? &type : வெற்று, !evaluate, FALSE, &found_var);

    // Need to make a copy, in case evaluating the arguments makes
    // the name invalid.
    s = vim_strsave(s);
    எனில் (s == வெற்று || (evaluate && *s == NUL))
	ret = FAIL;
    இல்லையெனில்
    {
	funcexe_T funcexe;

	// Invoke the function.
	CLEAR_FIELD(funcexe);
	funcexe.fe_firstline = curwin->w_cursor.lnum;
	funcexe.fe_lastline = curwin->w_cursor.lnum;
	funcexe.fe_evaluate = evaluate;
	funcexe.fe_partial = partial;
	எனில் (partial != வெற்று)
	{
	    funcexe.fe_object = partial->pt_obj;
	    எனில் (funcexe.fe_object != வெற்று)
		++funcexe.fe_object->obj_refcount;
	}
	funcexe.fe_basetv = basetv;
	funcexe.fe_check_type = type;
	funcexe.fe_found_var = found_var;
	எனில் (evalarg != வெற்று)
	    funcexe.fe_cctx = evalarg->eval_cctx;
	ret = get_func_tv(s, len, rettv, arg, evalarg, &funcexe);
    }
    vim_free(s);

    // If evaluate is FALSE rettv->v_type was not set in
    // get_func_tv, but it's needed in handle_subscript() to parse
    // what follows. So set it here.
    எனில் (rettv->v_type == VAR_UNKNOWN && !evaluate && **arg == '(')
    {
	rettv->vval.v_string = வெற்று;
	rettv->v_type = VAR_FUNC;
    }

    // Stop the expression evaluation when immediately
    // aborting on error, or when an interrupt occurred or
    // an exception was thrown but not caught.
    எனில் (evaluate && aborting())
    {
	எனில் (ret == OK)
	    clear_tv(rettv);
	ret = FAIL;
    }
    திரும்பு ret;
}

/*
 * After a NL, skip over empty lines and comment-only lines.
 */
    நிலையான char_u *
newline_skip_comments(char_u *arg)
{
    char_u *p = arg + 1;

    க்கு (;;)
    {
	p = skipwhite(p);

	எனில் (*p == NUL)
	    நிறுத்து;
	எனில் (vim9_comment_start(p))
	{
	    char_u *nl = vim_strchr(p, NL);

	    எனில் (nl == வெற்று)
		நிறுத்து;
	    p = nl;
	}
	எனில் (*p != NL)
	    நிறுத்து;
	++p;  // skip another NL
    }
    திரும்பு p;
}

/*
 * Get the next line source line without advancing.  But do skip over comment
 * lines.
 * Only called for Vim9 script.
 */
    நிலையான char_u *
getline_peek_skip_comments(evalarg_T *evalarg)
{
    க்கு (;;)
    {
	char_u *next = getline_peek(evalarg->eval_getline,
							 evalarg->eval_cookie);
	char_u *p;

	எனில் (next == வெற்று)
	    நிறுத்து;
	p = skipwhite(next);
	எனில் (*p != NUL && !vim9_comment_start(p))
	    திரும்பு next;
	எனில் (eval_next_வரி(வெற்று, evalarg) == வெற்று)
	    நிறுத்து;
    }
    திரும்பு வெற்று;
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
eval_next_non_blank(char_u *arg, evalarg_T *evalarg, முழு *getnext)
{
    char_u *p = skipwhite(arg);

    *getnext = FALSE;
    எனில் (in_vim9script()
	    && evalarg != வெற்று
	    && (evalarg->eval_cookie != வெற்று || evalarg->eval_cctx != வெற்று
								   || *p == NL)
	    && (*p == NUL || *p == NL
			     || (vim9_comment_start(p) && VIM_ISWHITE(p[-1]))))
    {
	char_u *next;

	எனில் (*p == NL)
	    next = newline_skip_comments(p);
	இல்லையெனில் எனில் (evalarg->eval_cookie != வெற்று)
	    next = getline_peek_skip_comments(evalarg);
	இல்லையெனில்
	    next = peek_next_வரி_from_context(evalarg->eval_cctx);

	எனில் (next != வெற்று)
	{
	    *getnext = *p != NL;
	    திரும்பு skipwhite(next);
	}
    }
    திரும்பு p;
}

/*
 * To be called after eval_next_non_blank() sets "getnext" to TRUE.
 * Only called for Vim9 script.
 *
 * If "arg" is not NULL, then the caller should assign the return value to
 * "arg".
 */
    char_u *
eval_next_வரி(char_u *arg, evalarg_T *evalarg)
{
    garray_T	*gap = &evalarg->eval_ga;
    char_u	*வரி;

    எனில் (arg != வெற்று)
    {
	எனில் (*arg == NL)
	    திரும்பு newline_skip_comments(arg);
	// Truncate before a trailing comment, so that concatenating the lines
	// won't turn the rest into a comment.
	எனில் (*skipwhite(arg) == '#')
	    *arg = NUL;
    }

    எனில் (evalarg->eval_cookie != வெற்று)
	வரி = evalarg->eval_getline(0, evalarg->eval_cookie, 0,
							   GETLINE_CONCAT_ALL);
    இல்லையெனில்
	வரி = next_வரி_from_context(evalarg->eval_cctx, TRUE);
    எனில் (வரி == வெற்று)
	திரும்பு வெற்று;

    ++evalarg->eval_break_மொத்தம்;
    எனில் (gap->ga_itemsize > 0 && ga_grow(gap, 1) == OK)
    {
	char_u *p = skipwhite(வரி);

	// Going to concatenate the lines after parsing.  For an empty or
	// comment line use an empty string.
	எனில் (*p == NUL || vim9_comment_start(p))
	{
	    vim_free(வரி);
	    வரி = vim_strsave((char_u *)"");
	}

	((char_u **)gap->ga_data)[gap->ga_len] = வரி;
	++gap->ga_len;
    }
    இல்லையெனில் எனில் (evalarg->eval_cookie != வெற்று)
    {
	free_eval_tofree_later(evalarg);
	evalarg->eval_tofree = வரி;
    }

    // Advanced to the next line, "arg" no longer points into the previous
    // line.  The caller assigns the return value to "arg".
    // If "arg" is NULL, then the return value is discarded.  In that case,
    // "arg" still points to the previous line.  So don't reset
    // "eval_using_cmdline".
    எனில் (arg != வெற்று)
	evalarg->eval_using_cmdline = FALSE;
    திரும்பு skipwhite(வரி);
}

/*
 * Call eval_next_non_blank() and get the next line if needed.
 */
    char_u *
skipwhite_and_linebreak(char_u *arg, evalarg_T *evalarg)
{
    முழு	    getnext;
    char_u  *p = skipwhite_and_nl(arg);

    எனில் (evalarg == வெற்று)
	திரும்பு skipwhite(arg);
    eval_next_non_blank(p, evalarg, &getnext);
    எனில் (getnext)
	திரும்பு eval_next_வரி(arg, evalarg);
    திரும்பு p;
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
    முழு
eval0(
    char_u	*arg,
    typval_T	*rettv,
    exarg_T	*eap,
    evalarg_T	*evalarg)
{
    திரும்பு eval0_retarg(arg, rettv, eap, evalarg, வெற்று);
}

/*
 * If "arg" is a simple function call without arguments then call it and return
 * the result.  Otherwise return NOTDONE.
 */
    முழு
may_call_simple_func(
    char_u	*arg,
    typval_T	*rettv)
{
    char_u  *parens = (char_u *)strstr((எழுத்து *)arg, "()");
    முழு	    r = NOTDONE;

    // If the expression is "FuncName()" then we can skip a lot of overhead.
    எனில் (parens != வெற்று && *skipwhite(parens + 2) == NUL)
    {
	char_u *p = STRNCMP(arg, "<SNR>", 5) == 0 ? skipdigits(arg + 5) : arg;

	எனில் (to_name_end(p, TRUE) == parens)
	    r = call_simple_func(arg, (size_t)(parens - arg), rettv);
    }
    திரும்பு r;
}

/*
 * Handle zero level expression with optimization for a simple function call.
 * Same arguments and return value as eval0().
 */
    நிலையான முழு
eval0_simple_funccal(
    char_u	*arg,
    typval_T	*rettv,
    exarg_T	*eap,
    evalarg_T	*evalarg)
{
    முழு	    r = may_call_simple_func(arg, rettv);

    எனில் (r == NOTDONE)
	r = eval0_retarg(arg, rettv, eap, evalarg, வெற்று);
    திரும்பு r;
}

/*
 * Like eval0() but when "retarg" is not NULL store the pointer to after the
 * expression and don't check what comes after the expression.
 */
    முழு
eval0_retarg(
    char_u	*arg,
    typval_T	*rettv,
    exarg_T	*eap,
    evalarg_T	*evalarg,
    char_u	**retarg)
{
    முழு		ret;
    char_u	*p;
    char_u	*கோவை_end;
    முழு		did_emsg_before = did_emsg;
    முழு		called_emsg_before = called_emsg;
    முழு		check_for_end = retarg == வெற்று;
    முழு		end_error = FALSE;

    p = skipwhite(arg);
    ret = eval1(&p, rettv, evalarg);

    எனில் (ret != FAIL)
    {
	கோவை_end = p;
	p = skipwhite(p);

	// In Vim9 script a command block is not split at NL characters for
	// commands using an expression argument.  Skip over a '#' comment to
	// check for a following NL.  Require white space before the '#'.
	எனில் (in_vim9script() && p > கோவை_end && retarg == வெற்று)
	    போது (*p == '#')
	    {
		char_u *nl = vim_strchr(p, NL);

		எனில் (nl == வெற்று)
		    நிறுத்து;
		p = skipwhite(nl + 1);
		எனில் (eap != வெற்று && *p != NUL)
		    eap->nextcmd = p;
		check_for_end = FALSE;
	    }

	எனில் (check_for_end)
	    end_error = !ends_excmd2(arg, p);
    }

    எனில் (ret == FAIL || end_error)
    {
	எனில் (ret != FAIL)
	    clear_tv(rettv);
	/*
	 * Report the invalid expression unless the expression evaluation has
	 * been cancelled due to an aborting error, an interrupt, or an
	 * exception, or we already gave a more specific error.
	 * Also check called_emsg for when using assert_fails().
	 */
	எனில் (!aborting()
		&& did_emsg == did_emsg_before
		&& called_emsg == called_emsg_before
		&& (!in_vim9script() || !vim9_bad_comment(p)))
	{
	    எனில் (end_error)
		semsg(_(e_trailing_characters_str), p);
	    இல்லையெனில்
		semsg(_(e_invalid_expression_str), arg);
	}

	எனில் (eap != வெற்று && p != வெற்று)
	{
	    // Some of the expression may not have been consumed.
	    // Only execute a next command if it cannot be a "||" operator.
	    // The next command may be "catch".
	    char_u *nextcmd = check_nextcmd(p);
	    எனில் (nextcmd != வெற்று && *nextcmd != '|')
		eap->nextcmd = nextcmd;
	}
	திரும்பு FAIL;
    }

    எனில் (retarg != வெற்று)
	*retarg = p;
    இல்லையெனில் எனில் (check_for_end && eap != வெற்று)
	set_nextcmd(eap, p);

    திரும்பு ret;
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
    முழு
eval1(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    char_u  *p;
    முழு	    getnext;

    CLEAR_POINTER(rettv);

    /*
     * Get the first variable.
     */
    எனில் (eval2(arg, rettv, evalarg) == FAIL)
	திரும்பு FAIL;

    p = eval_next_non_blank(*arg, evalarg, &getnext);
    எனில் (*p == '?')
    {
	முழு		op_falsy = p[1] == '?';
	முழு		result;
	typval_T	var2;
	evalarg_T	*evalarg_used = evalarg;
	evalarg_T	local_evalarg;
	முழு		orig_flags;
	முழு		evaluate;
	முழு		vim9script = in_vim9script();

	எனில் (evalarg == வெற்று)
	{
	    init_evalarg(&local_evalarg);
	    evalarg_used = &local_evalarg;
	}
	orig_flags = evalarg_used->eval_flags;
	evaluate = evalarg_used->eval_flags & EVAL_EVALUATE;

	எனில் (getnext)
	    *arg = eval_next_வரி(*arg, evalarg_used);
	இல்லையெனில்
	{
	    எனில் (evaluate && vim9script && !VIM_ISWHITE(p[-1]))
	    {
		error_white_both(p, op_falsy ? 2 : 1);
		clear_tv(rettv);
		திரும்பு FAIL;
	    }
	    *arg = p;
	}

	result = FALSE;
	எனில் (evaluate)
	{
	    முழு		error = FALSE;

	    எனில் (op_falsy)
	    {
		// Is this typeval supported with the falsy operator?
		எனில் (check_typval_is_மதிப்பு(rettv) == FAIL)
		{
		    clear_tv(rettv);
		    திரும்பு FAIL;
		}
		result = tv2bool(rettv);
	    }
	    இல்லையெனில் எனில் (vim9script)
		result = tv_get_bool_chk(rettv, &error);
	    இல்லையெனில் எனில் (tv_get_எண்_chk(rettv, &error) != 0)
		result = TRUE;
	    எனில் (error || !op_falsy || !result)
		clear_tv(rettv);
	    எனில் (error)
		திரும்பு FAIL;
	}

	/*
	 * Get the second variable.  Recursive!
	 */
	எனில் (op_falsy)
	    ++*arg;
	எனில் (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[1]))
	{
	    error_white_both(*arg - (op_falsy ? 1 : 0), op_falsy ? 2 : 1);
	    clear_tv(rettv);
	    திரும்பு FAIL;
	}
	*arg = skipwhite_and_linebreak(*arg + 1, evalarg_used);
	evalarg_used->eval_flags = (op_falsy ? !result : result)
				  ? orig_flags : (orig_flags & ~EVAL_EVALUATE);
	எனில் (eval1(arg, &var2, evalarg_used) == FAIL)
	{
	    evalarg_used->eval_flags = orig_flags;
	    திரும்பு FAIL;
	}
	எனில் (!op_falsy || !result)
	    *rettv = var2;

	எனில் (!op_falsy)
	{
	    /*
	     * Check for the ":".
	     */
	    p = eval_next_non_blank(*arg, evalarg_used, &getnext);
	    எனில் (*p != ':')
	    {
		emsg(_(e_missing_colon_after_questionmark));
		எனில் (evaluate && result)
		    clear_tv(rettv);
		evalarg_used->eval_flags = orig_flags;
		திரும்பு FAIL;
	    }
	    எனில் (getnext)
		*arg = eval_next_வரி(*arg, evalarg_used);
	    இல்லையெனில்
	    {
		எனில் (evaluate && vim9script && !VIM_ISWHITE(p[-1]))
		{
		    error_white_both(p, 1);
		    clear_tv(rettv);
		    evalarg_used->eval_flags = orig_flags;
		    திரும்பு FAIL;
		}
		*arg = p;
	    }

	    /*
	     * Get the third variable.  Recursive!
	     */
	    எனில் (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[1]))
	    {
		error_white_both(*arg, 1);
		clear_tv(rettv);
		evalarg_used->eval_flags = orig_flags;
		திரும்பு FAIL;
	    }
	    *arg = skipwhite_and_linebreak(*arg + 1, evalarg_used);
	    evalarg_used->eval_flags = !result ? orig_flags
					       : (orig_flags & ~EVAL_EVALUATE);
	    எனில் (eval1(arg, &var2, evalarg_used) == FAIL)
	    {
		எனில் (evaluate && result)
		    clear_tv(rettv);
		evalarg_used->eval_flags = orig_flags;
		திரும்பு FAIL;
	    }
	    எனில் (evaluate && !result)
		*rettv = var2;
	}

	எனில் (evalarg == வெற்று)
	    clear_evalarg(&local_evalarg, வெற்று);
	இல்லையெனில்
	    evalarg->eval_flags = orig_flags;
    }

    திரும்பு OK;
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
    நிலையான முழு
eval2(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    char_u	*p;
    முழு		getnext;

    /*
     * Get the first expression.
     */
    எனில் (eval3(arg, rettv, evalarg) == FAIL)
	திரும்பு FAIL;

    /*
     * Handle the  "||" operator.
     */
    p = eval_next_non_blank(*arg, evalarg, &getnext);
    எனில் (p[0] == '|' && p[1] == '|')
    {
	evalarg_T   *evalarg_used = evalarg;
	evalarg_T   local_evalarg;
	முழு	    evaluate;
	முழு	    orig_flags;
	long	    result = FALSE;
	typval_T    var2;
	முழு	    error = FALSE;
	முழு	    vim9script = in_vim9script();

	எனில் (evalarg == வெற்று)
	{
	    init_evalarg(&local_evalarg);
	    evalarg_used = &local_evalarg;
	}
	orig_flags = evalarg_used->eval_flags;
	evaluate = orig_flags & EVAL_EVALUATE;
	எனில் (evaluate)
	{
	    எனில் (vim9script)
		result = tv_get_bool_chk(rettv, &error);
	    இல்லையெனில் எனில் (tv_get_எண்_chk(rettv, &error) != 0)
		result = TRUE;
	    clear_tv(rettv);
	    எனில் (error)
		திரும்பு FAIL;
	}

	/*
	 * Repeat until there is no following "||".
	 */
	போது (p[0] == '|' && p[1] == '|')
	{
	    எனில் (getnext)
		*arg = eval_next_வரி(*arg, evalarg_used);
	    இல்லையெனில்
	    {
		எனில் (evaluate && vim9script && !VIM_ISWHITE(p[-1]))
		{
		    error_white_both(p, 2);
		    clear_tv(rettv);
		    திரும்பு FAIL;
		}
		*arg = p;
	    }

	    /*
	     * Get the second variable.
	     */
	    எனில் (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[2]))
	    {
		error_white_both(*arg, 2);
		clear_tv(rettv);
		திரும்பு FAIL;
	    }
	    *arg = skipwhite_and_linebreak(*arg + 2, evalarg_used);
	    evalarg_used->eval_flags = !result ? orig_flags
					       : (orig_flags & ~EVAL_EVALUATE);
	    எனில் (eval3(arg, &var2, evalarg_used) == FAIL)
		திரும்பு FAIL;

	    /*
	     * Compute the result.
	     */
	    எனில் (evaluate && !result)
	    {
		எனில் (vim9script)
		    result = tv_get_bool_chk(&var2, &error);
		இல்லையெனில் எனில் (tv_get_எண்_chk(&var2, &error) != 0)
		    result = TRUE;
		clear_tv(&var2);
		எனில் (error)
		    திரும்பு FAIL;
	    }
	    எனில் (evaluate)
	    {
		எனில் (vim9script)
		{
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_எண் = result ? VVAL_TRUE : VVAL_FALSE;
		}
		இல்லையெனில்
		{
		    rettv->v_type = VAR_எண்;
		    rettv->vval.v_எண் = result;
		}
	    }

	    p = eval_next_non_blank(*arg, evalarg_used, &getnext);
	}

	எனில் (evalarg == வெற்று)
	    clear_evalarg(&local_evalarg, வெற்று);
	இல்லையெனில்
	    evalarg->eval_flags = orig_flags;
    }

    திரும்பு OK;
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
    நிலையான முழு
eval3(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    char_u	*p;
    முழு		getnext;

    /*
     * Get the first expression.
     */
    எனில் (eval4(arg, rettv, evalarg) == FAIL)
	திரும்பு FAIL;

    /*
     * Handle the "&&" operator.
     */
    p = eval_next_non_blank(*arg, evalarg, &getnext);
    எனில் (p[0] == '&' && p[1] == '&')
    {
	evalarg_T   *evalarg_used = evalarg;
	evalarg_T   local_evalarg;
	முழு	    orig_flags;
	முழு	    evaluate;
	long	    result = TRUE;
	typval_T    var2;
	முழு	    error = FALSE;
	முழு	    vim9script = in_vim9script();

	எனில் (evalarg == வெற்று)
	{
	    init_evalarg(&local_evalarg);
	    evalarg_used = &local_evalarg;
	}
	orig_flags = evalarg_used->eval_flags;
	evaluate = orig_flags & EVAL_EVALUATE;
	எனில் (evaluate)
	{
	    எனில் (vim9script)
		result = tv_get_bool_chk(rettv, &error);
	    இல்லையெனில் எனில் (tv_get_எண்_chk(rettv, &error) == 0)
		result = FALSE;
	    clear_tv(rettv);
	    எனில் (error)
		திரும்பு FAIL;
	}

	/*
	 * Repeat until there is no following "&&".
	 */
	போது (p[0] == '&' && p[1] == '&')
	{
	    எனில் (getnext)
		*arg = eval_next_வரி(*arg, evalarg_used);
	    இல்லையெனில்
	    {
		எனில் (evaluate && vim9script && !VIM_ISWHITE(p[-1]))
		{
		    error_white_both(p, 2);
		    clear_tv(rettv);
		    திரும்பு FAIL;
		}
		*arg = p;
	    }

	    /*
	     * Get the second variable.
	     */
	    எனில் (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[2]))
	    {
		error_white_both(*arg, 2);
		clear_tv(rettv);
		திரும்பு FAIL;
	    }
	    *arg = skipwhite_and_linebreak(*arg + 2, evalarg_used);
	    evalarg_used->eval_flags = result ? orig_flags
					      : (orig_flags & ~EVAL_EVALUATE);
	    CLEAR_FIELD(var2);
	    எனில் (eval4(arg, &var2, evalarg_used) == FAIL)
		திரும்பு FAIL;

	    /*
	     * Compute the result.
	     */
	    எனில் (evaluate && result)
	    {
		எனில் (vim9script)
		    result = tv_get_bool_chk(&var2, &error);
		இல்லையெனில் எனில் (tv_get_எண்_chk(&var2, &error) == 0)
		    result = FALSE;
		clear_tv(&var2);
		எனில் (error)
		    திரும்பு FAIL;
	    }
	    எனில் (evaluate)
	    {
		எனில் (vim9script)
		{
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_எண் = result ? VVAL_TRUE : VVAL_FALSE;
		}
		இல்லையெனில்
		{
		    rettv->v_type = VAR_எண்;
		    rettv->vval.v_எண் = result;
		}
	    }

	    p = eval_next_non_blank(*arg, evalarg_used, &getnext);
	}

	எனில் (evalarg == வெற்று)
	    clear_evalarg(&local_evalarg, வெற்று);
	இல்லையெனில்
	    evalarg->eval_flags = orig_flags;
    }

    திரும்பு OK;
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
    நிலையான முழு
eval4(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    char_u	*p;
    முழு		getnext;
    exprtype_T	type = கோவை_UNKNOWN;
    முழு		len = 2;
    முழு		type_is = FALSE;

    /*
     * Get the first expression.
     */
    எனில் (eval5(arg, rettv, evalarg) == FAIL)
	திரும்பு FAIL;

    p = eval_next_non_blank(*arg, evalarg, &getnext);

    type = get_compare_type(p, &len, &type_is);

    /*
     * If there is a comparative operator, use it.
     */
    எனில் (type != கோவை_UNKNOWN)
    {
	typval_T    var2;
	முழு	    ic;
	முழு	    vim9script = in_vim9script();
	முழு	    evaluate = evalarg == வெற்று
				   ? 0 : (evalarg->eval_flags & EVAL_EVALUATE);
	long	    comp_lnum = SOURCING_LNUM;

	எனில் (getnext)
	{
	    *arg = eval_next_வரி(*arg, evalarg);
	    p = *arg;
	}
	இல்லையெனில் எனில் (evaluate && vim9script && !VIM_ISWHITE(**arg))
	{
	    error_white_both(*arg, len);
	    clear_tv(rettv);
	    திரும்பு FAIL;
	}

	எனில் (vim9script && type_is && (p[len] == '?' || p[len] == '#'))
	{
	    semsg(_(e_invalid_expression_str), p);
	    clear_tv(rettv);
	    திரும்பு FAIL;
	}

	// extra question mark appended: ignore case
	எனில் (p[len] == '?')
	{
	    ic = TRUE;
	    ++len;
	}
	// extra '#' appended: match case
	இல்லையெனில் எனில் (p[len] == '#')
	{
	    ic = FALSE;
	    ++len;
	}
	// nothing appended: use 'ignorecase' if not in Vim script
	இல்லையெனில்
	    ic = vim9script ? FALSE : p_ic;

	/*
	 * Get the second variable.
	 */
	எனில் (evaluate && vim9script && !IS_WHITE_OR_NUL(p[len]))
	{
	    error_white_both(p, len);
	    clear_tv(rettv);
	    திரும்பு FAIL;
	}
	*arg = skipwhite_and_linebreak(p + len, evalarg);
	எனில் (eval5(arg, &var2, evalarg) == FAIL)
	{
	    clear_tv(rettv);
	    திரும்பு FAIL;
	}
	எனில் (evaluate)
	{
	    முழு ret;

	    // use the line of the comparison for messages
	    SOURCING_LNUM = comp_lnum;
	    எனில் (vim9script && check_compare_types(type, rettv, &var2) == FAIL)
	    {
		ret = FAIL;
		clear_tv(rettv);
	    }
	    இல்லையெனில்
		ret = typval_compare(rettv, &var2, type, ic);
	    clear_tv(&var2);
	    திரும்பு ret;
	}
    }

    திரும்பு OK;
}

/*
 * Make a copy of blob "tv1" and append blob "tv2".
 */
    வெறுமை
eval_addblob(typval_T *tv1, typval_T *tv2)
{
    blob_T  *b1 = tv1->vval.v_blob;
    blob_T  *b2 = tv2->vval.v_blob;
    blob_T  *b = blob_alloc();
    long    len1, len2;
    long    totallen;

    எனில் (b == வெற்று)
	திரும்பு;

    len1 = blob_len(b1);
    len2 = blob_len(b2);
    totallen = len1 + len2;

    எனில் (totallen > 0 && totallen <= INT_MAX
				    && ga_grow(&b->bv_ga, (முழு)totallen) == OK)
    {
	எனில் (len1 > 0)
	    mch_memmove((char_u *)b->bv_ga.ga_data,
		    (char_u *)b1->bv_ga.ga_data, (size_t)len1);
	எனில் (len2 > 0)
	    mch_memmove((char_u *)b->bv_ga.ga_data + len1,
		    (char_u *)b2->bv_ga.ga_data, (size_t)len2);
	b->bv_ga.ga_len = (முழு)totallen;
    }

    clear_tv(tv1);
    rettv_blob_set(tv1, b);
}

/*
 * Make a copy of list "tv1" and append list "tv2".
 */
    முழு
eval_addlist(typval_T *tv1, typval_T *tv2)
{
    typval_T var3;

    // concatenate Lists
    எனில் (list_concat(tv1->vval.v_list, tv2->vval.v_list, &var3) == FAIL)
    {
	clear_tv(tv1);
	clear_tv(tv2);
	திரும்பு FAIL;
    }
    clear_tv(tv1);
    *tv1 = var3;
    திரும்பு OK;
}

/*
 * Make a copy of tuple "tv1" and append tuple "tv2".
 */
    முழு
eval_addtuple(typval_T *tv1, typval_T *tv2)
{
    முழு		vim9script = in_vim9script();
    typval_T	var3;

    எனில் (vim9script && tv1->vval.v_tuple != வெற்று && tv2->vval.v_tuple != வெற்று
	    && tv1->vval.v_tuple->tv_type != வெற்று
	    && tv2->vval.v_tuple->tv_type != வெற்று)
    {
	எனில் (!check_tuples_addable(tv1->vval.v_tuple->tv_type,
						tv2->vval.v_tuple->tv_type))
	    திரும்பு FAIL;
    }

    // concatenate tuples
    எனில் (tuple_concat(tv1->vval.v_tuple, tv2->vval.v_tuple, &var3) == FAIL)
    {
	clear_tv(tv1);
	clear_tv(tv2);
	திரும்பு FAIL;
    }
    clear_tv(tv1);
    *tv1 = var3;
    திரும்பு OK;
}

/*
 * Left or right shift the number "tv1" by the number "tv2" and store the
 * result in "tv1".
 *
 * Return OK or FAIL.
 */
    நிலையான முழு
eval_shift_எண்(typval_T *tv1, typval_T *tv2, முழு shift_type)
{
    எனில் (tv2->v_type != VAR_எண் || tv2->vval.v_எண் < 0)
    {
	// right operand should be a positive number
	எனில் (tv2->v_type != VAR_எண்)
	    emsg(_(e_bitshift_ops_must_be_எண்));
	இல்லையெனில்
	    emsg(_(e_bitshift_ops_must_be_positive));
	clear_tv(tv1);
	clear_tv(tv2);
	திரும்பு FAIL;
    }

    எனில் (tv2->vval.v_எண் > MAX_LSHIFT_BITS)
	// shifting more bits than we have always results in zero
	tv1->vval.v_எண் = 0;
    இல்லையெனில் எனில் (shift_type == கோவை_LSHIFT)
	tv1->vval.v_எண் =
	    (uvarnumber_T)tv1->vval.v_எண் << tv2->vval.v_எண்;
    இல்லையெனில்
	tv1->vval.v_எண் =
	    (uvarnumber_T)tv1->vval.v_எண் >> tv2->vval.v_எண்;

    திரும்பு OK;
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
    நிலையான முழு
eval5(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    /*
     * Get the first expression.
     */
    எனில் (eval6(arg, rettv, evalarg) == FAIL)
	திரும்பு FAIL;

    /*
     * Repeat computing, until no '<<' or '>>' is following.
     */
    க்கு (;;)
    {
	char_u		*p;
	முழு		getnext;
	exprtype_T	exprtype;
	முழு		evaluate;
	typval_T	var2;
	முழு		vim9script;

	p = eval_next_non_blank(*arg, evalarg, &getnext);
	எனில் (p[0] == '<' && p[1] == '<')
	    exprtype = கோவை_LSHIFT;
	இல்லையெனில் எனில் (p[0] == '>' && p[1] == '>')
	    exprtype = கோவை_RSHIFT;
	இல்லையெனில்
	    திரும்பு OK;

	// Handle a bitwise left or right shift operator
	evaluate = evalarg == வெற்று ? 0 : (evalarg->eval_flags & EVAL_EVALUATE);
	எனில் (evaluate && rettv->v_type != VAR_எண்)
	{
	    // left operand should be a number
	    emsg(_(e_bitshift_ops_must_be_எண்));
	    clear_tv(rettv);
	    திரும்பு FAIL;
	}

	vim9script = in_vim9script();
	எனில் (getnext)
	{
	    *arg = eval_next_வரி(*arg, evalarg);
	    p = *arg;
	}
	இல்லையெனில் எனில் (evaluate && vim9script && !VIM_ISWHITE(**arg))
	{
	    error_white_both(*arg, 2);
	    clear_tv(rettv);
	    திரும்பு FAIL;
	}

	/*
	 * Get the second variable.
	 */
	எனில் (evaluate && vim9script && !IS_WHITE_OR_NUL(p[2]))
	{
	    error_white_both(p, 2);
	    clear_tv(rettv);
	    திரும்பு FAIL;
	}
	*arg = skipwhite_and_linebreak(p + 2, evalarg);
	எனில் (eval6(arg, &var2, evalarg) == FAIL)
	{
	    clear_tv(rettv);
	    திரும்பு FAIL;
	}

	எனில் (evaluate)
	{
	    எனில் (eval_shift_எண்(rettv, &var2, exprtype) == FAIL)
		திரும்பு FAIL;
	}

	clear_tv(&var2);
    }

    திரும்பு OK;
}

/*
 * Concatenate strings "tv1" and "tv2" and store the result in "tv1".
 */
    நிலையான முழு
eval_concat_str(typval_T *tv1, typval_T *tv2)
{
    char_u	buf1[NUMBUFLEN], buf2[NUMBUFLEN];
    char_u	*s1 = tv_get_string_buf(tv1, buf1);
    char_u	*s2 = வெற்று;
    char_u	*p;
    முழு		vim9script = in_vim9script();

    எனில் (vim9script && (tv2->v_type == VAR_VOID
		|| tv2->v_type == VAR_CHANNEL
		|| tv2->v_type == VAR_JOB))
	semsg(_(e_using_invalid_மதிப்பு_as_string_str),
		vartype_name(tv2->v_type));
    இல்லையெனில் எனில் (vim9script && tv2->v_type == VAR_FLOAT)
    {
	vim_snprintf((எழுத்து *)buf2, NUMBUFLEN, "%g",
		tv2->vval.v_float);
	s2 = buf2;
    }
    இல்லையெனில்
	s2 = tv_get_string_buf_chk(tv2, buf2);
    எனில் (s2 == வெற்று)		// type error ?
    {
	clear_tv(tv1);
	clear_tv(tv2);
	திரும்பு FAIL;
    }

    // When possible, grow the existing string in place to avoid alloc/free.
    எனில் (grow_string_tv(tv1, s2) == OK)
	திரும்பு OK;

    p = concat_str(s1, s2);
    clear_tv(tv1);
    tv1->v_type = VAR_STRING;
    tv1->vval.v_string = p;

    திரும்பு OK;
}

/*
 * Add or subtract numbers "tv1" and "tv2" and store the result in "tv1".
 * The numbers can be whole numbers or floats.
 */
    நிலையான முழு
eval_addsub_எண்(typval_T *tv1, typval_T *tv2, முழு op)
{
    முழு		error = FALSE;
    varnumber_T	n1, n2;
    float_T	f1 = 0, f2 = 0;

    எனில் (tv1->v_type == VAR_FLOAT)
    {
	f1 = tv1->vval.v_float;
	n1 = 0;
    }
    இல்லையெனில்
    {
	n1 = tv_get_எண்_chk(tv1, &error);
	எனில் (error)
	{
	    // This can only happen for "list + non-list" or
	    // "blob + non-blob".  For "non-list + ..." or
	    // "something - ...", we returned before evaluating the
	    // 2nd operand.
	    clear_tv(tv1);
	    clear_tv(tv2);
	    திரும்பு FAIL;
	}
	எனில் (tv2->v_type == VAR_FLOAT)
	    f1 = n1;
    }
    எனில் (tv2->v_type == VAR_FLOAT)
    {
	f2 = tv2->vval.v_float;
	n2 = 0;
    }
    இல்லையெனில்
    {
	n2 = tv_get_எண்_chk(tv2, &error);
	எனில் (error)
	{
	    clear_tv(tv1);
	    clear_tv(tv2);
	    திரும்பு FAIL;
	}
	எனில் (tv1->v_type == VAR_FLOAT)
	    f2 = n2;
    }
    clear_tv(tv1);

    // If there is a float on either side the result is a float.
    எனில் (tv1->v_type == VAR_FLOAT || tv2->v_type == VAR_FLOAT)
    {
	எனில் (op == '+')
	    f1 = f1 + f2;
	இல்லையெனில்
	    f1 = f1 - f2;
	tv1->v_type = VAR_FLOAT;
	tv1->vval.v_float = f1;
    }
    இல்லையெனில்
    {
	எனில் (op == '+')
	    n1 = n1 + n2;
	இல்லையெனில்
	    n1 = n1 - n2;
	tv1->v_type = VAR_எண்;
	tv1->vval.v_எண் = n1;
    }

    திரும்பு OK;
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
    நிலையான முழு
eval6(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    /*
     * Get the first expression.
     */
    எனில் (eval7(arg, rettv, evalarg, FALSE) == FAIL)
	திரும்பு FAIL;

    /*
     * Repeat computing, until no '+', '-' or '.' is following.
     */
    க்கு (;;)
    {
	முழு	    evaluate;
	முழு	    getnext;
	char_u	    *p;
	முழு	    op;
	முழு	    oplen;
	முழு	    concat;
	typval_T    var2;
	முழு	    vim9script = in_vim9script();
	long	    op_lnum = SOURCING_LNUM;

	// "." is only string concatenation when scriptversion is 1
	// "+=", "-=" and "..=" are assignments
	// "++" and "--" on the next line are a separate command.
	p = eval_next_non_blank(*arg, evalarg, &getnext);
	op = *p;
	concat = op == '.' && (*(p + 1) == '.' || in_old_script(2));
	எனில் ((op != '+' && op != '-' && !concat) || p[1] == '='
					       || (p[1] == '.' && p[2] == '='))
	    நிறுத்து;
	எனில் (getnext && (op == '+' || op == '-') && p[0] == p[1])
	    நிறுத்து;

	evaluate = evalarg == வெற்று ? 0 : (evalarg->eval_flags & EVAL_EVALUATE);
	oplen = (concat && p[1] == '.') ? 2 : 1;
	எனில் (getnext)
	    *arg = eval_next_வரி(*arg, evalarg);
	இல்லையெனில்
	{
	    எனில் (evaluate && vim9script && !VIM_ISWHITE(**arg))
	    {
		error_white_both(*arg, oplen);
		clear_tv(rettv);
		திரும்பு FAIL;
	    }
	    *arg = p;
	}
	எனில் ((op != '+' || (rettv->v_type != VAR_LIST
						&& rettv->v_type != VAR_TUPLE
						&& rettv->v_type != VAR_BLOB))
		&& (op == '.' || rettv->v_type != VAR_FLOAT)
		&& evaluate)
	{
	    முழு		error = FALSE;

	    // For "list + ...", an illegal use of the first operand as
	    // a number cannot be determined before evaluating the 2nd
	    // operand: if this is also a list, all is ok.
	    // For "something . ...", "something - ..." or "non-list + ...",
	    // we know that the first operand needs to be a string or number
	    // without evaluating the 2nd operand.  So check before to avoid
	    // side effects after an error.
	    எனில் (op != '.')
		tv_get_எண்_chk(rettv, &error);
	    எனில் ((op == '.' && tv_get_string_chk(rettv) == வெற்று) || error)
	    {
		clear_tv(rettv);
		திரும்பு FAIL;
	    }
	}

	/*
	 * Get the second variable.
	 */
	எனில் (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[oplen]))
	{
	    error_white_both(*arg, oplen);
	    clear_tv(rettv);
	    திரும்பு FAIL;
	}
	*arg = skipwhite_and_linebreak(*arg + oplen, evalarg);
	எனில் (eval7(arg, &var2, evalarg, !vim9script && op == '.') == FAIL)
	{
	    clear_tv(rettv);
	    திரும்பு FAIL;
	}

	எனில் (evaluate)
	{
	    /*
	     * Compute the result.
	     */
	    // use the line of the operation for messages
	    SOURCING_LNUM = op_lnum;
	    எனில் (op == '.')
	    {
		எனில் (eval_concat_str(rettv, &var2) == FAIL)
		    திரும்பு FAIL;
	    }
	    இல்லையெனில் எனில் (op == '+' && rettv->v_type == VAR_BLOB
						   && var2.v_type == VAR_BLOB)
		eval_addblob(rettv, &var2);
	    இல்லையெனில் எனில் (op == '+' && rettv->v_type == VAR_LIST
						   && var2.v_type == VAR_LIST)
	    {
		எனில் (eval_addlist(rettv, &var2) == FAIL)
		    திரும்பு FAIL;
	    }
	    இல்லையெனில் எனில் (op == '+' && rettv->v_type == VAR_TUPLE
					   && var2.v_type == VAR_TUPLE)
	    {
		எனில் (eval_addtuple(rettv, &var2) == FAIL)
		    திரும்பு FAIL;
	    }
	    இல்லையெனில்
	    {
		எனில் (eval_addsub_எண்(rettv, &var2, op) == FAIL)
		    திரும்பு FAIL;
	    }
	    clear_tv(&var2);
	}
    }
    திரும்பு OK;
}

/*
 * Multiply or divide or compute the modulo of numbers "tv1" and "tv2" and
 * store the result in "tv1".  The numbers can be whole numbers or floats.
 */
    நிலையான முழு
eval_multdiv_எண்(typval_T *tv1, typval_T *tv2, முழு op)
{
    varnumber_T	n1, n2;
    float_T	f1, f2;
    முழு		error;
    முழு		use_float = FALSE;

    f1 = 0;
    f2 = 0;
    error = FALSE;
    எனில் (tv1->v_type == VAR_FLOAT)
    {
	f1 = tv1->vval.v_float;
	use_float = TRUE;
	n1 = 0;
    }
    இல்லையெனில்
	n1 = tv_get_எண்_chk(tv1, &error);
    clear_tv(tv1);
    எனில் (error)
    {
	clear_tv(tv2);
	திரும்பு FAIL;
    }

    எனில் (tv2->v_type == VAR_FLOAT)
    {
	எனில் (!use_float)
	{
	    f1 = n1;
	    use_float = TRUE;
	}
	f2 = tv2->vval.v_float;
	n2 = 0;
    }
    இல்லையெனில்
    {
	n2 = tv_get_எண்_chk(tv2, &error);
	clear_tv(tv2);
	எனில் (error)
	    திரும்பு FAIL;
	எனில் (use_float)
	    f2 = n2;
    }

    /*
     * Compute the result.
     * When either side is a float the result is a float.
     */
    எனில் (use_float)
    {
	எனில் (op == '*')
	    f1 = f1 * f2;
	இல்லையெனில் எனில் (op == '/')
	{
#ifdef VMS
	    // VMS crashes on divide by zero, work around it
	    எனில் (f2 == 0.0)
	    {
		எனில் (f1 == 0)
		    f1 = -1 * __F_FLT_MAX - 1L;   // similar to NaN
		இல்லையெனில் எனில் (f1 < 0)
		    f1 = -1 * __F_FLT_MAX;
		இல்லையெனில்
		    f1 = __F_FLT_MAX;
	    }
	    இல்லையெனில்
		f1 = f1 / f2;
#இல்லையெனில்
	    // We rely on the floating point library to handle divide
	    // by zero to result in "inf" and not a crash.
	    f1 = f1 / f2;
#endif
	}
	இல்லையெனில்
	{
	    emsg(_(e_cannot_use_percent_with_float));
	    திரும்பு FAIL;
	}
	tv1->v_type = VAR_FLOAT;
	tv1->vval.v_float = f1;
    }
    இல்லையெனில்
    {
	முழு	    failed = FALSE;

	எனில் (op == '*')
	    n1 = n1 * n2;
	இல்லையெனில் எனில் (op == '/')
	    n1 = இலக்கம்_divide(n1, n2, &failed);
	இல்லையெனில்
	    n1 = இலக்கம்_modulus(n1, n2, &failed);
	எனில் (failed)
	    திரும்பு FAIL;

	tv1->v_type = VAR_எண்;
	tv1->vval.v_எண் = n1;
    }

    திரும்பு OK;
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
    நிலையான முழு
eval7(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    முழு		want_string)  // after "." operator
{
    /*
     * Get the first expression.
     */
    எனில் (eval8(arg, rettv, evalarg, want_string) == FAIL)
	திரும்பு FAIL;

    /*
     * Repeat computing, until no '*', '/' or '%' is following.
     */
    க்கு (;;)
    {
	முழு	    evaluate;
	முழு	    getnext;
	typval_T    var2;
	char_u	    *p;
	முழு	    op;

	// "*=", "/=" and "%=" are assignments
	p = eval_next_non_blank(*arg, evalarg, &getnext);
	op = *p;
	எனில் ((op != '*' && op != '/' && op != '%') || p[1] == '=')
	    நிறுத்து;

	evaluate = evalarg == வெற்று ? 0 : (evalarg->eval_flags & EVAL_EVALUATE);
	எனில் (getnext)
	    *arg = eval_next_வரி(*arg, evalarg);
	இல்லையெனில்
	{
	    எனில் (evaluate && in_vim9script() && !VIM_ISWHITE(**arg))
	    {
		error_white_both(*arg, 1);
		clear_tv(rettv);
		திரும்பு FAIL;
	    }
	    *arg = p;
	}

	/*
	 * Get the second variable.
	 */
	எனில் (evaluate && in_vim9script() && !IS_WHITE_OR_NUL((*arg)[1]))
	{
	    error_white_both(*arg, 1);
	    clear_tv(rettv);
	    திரும்பு FAIL;
	}
	*arg = skipwhite_and_linebreak(*arg + 1, evalarg);
	எனில் (eval8(arg, &var2, evalarg, FALSE) == FAIL)
	    திரும்பு FAIL;

	எனில் (evaluate)
	    // Compute the result.
	    எனில் (eval_multdiv_எண்(rettv, &var2, op) == FAIL)
		திரும்பு FAIL;
    }

    திரும்பு OK;
}

/*
 * Handle seventh level expression:
 *	a type cast before a base level expression.
 * "arg" must point to the first non-white of the expression.
 * "arg" is advanced to just after the recognized expression.
 * Return OK or FAIL.
 */
    நிலையான முழு
eval8(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    முழு		want_string)	// after "." operator
{
    type_T	*want_type = வெற்று;
    garray_T	type_list;	    // list of pointers to allocated types
    முழு		res;
    முழு		evaluate = evalarg == வெற்று ? 0
				       : (evalarg->eval_flags & EVAL_EVALUATE);

    // Recognize <type> in Vim9 script only.
    எனில் (in_vim9script() && **arg == '<' && eval_isnamec1((*arg)[1])
					     && STRNCMP(*arg, "<SNR>", 5) != 0)
    {
	++*arg;
	ga_init2(&type_list, அளவு(type_T *), 10);
	எனில் (!evaluate)
	    // Only skip over the type, it may use a type variable that is only
	    // known when the expression is compiled, e.g. in a lambda.
	    *arg = skip_type(*arg, FALSE);
	இல்லையெனில்
	{
	    want_type = parse_type(arg, &type_list, வெற்று, வெற்று, TRUE);
	    எனில் (want_type == வெற்று)
	    {
		clear_type_list(&type_list);
		திரும்பு FAIL;
	    }
	}

	எனில் (**arg != '>')
	{
	    எனில் (*skipwhite(*arg) == '>')
		semsg(_(e_no_white_space_allowed_before_str_str), ">", *arg);
	    இல்லையெனில்
		emsg(_(e_missing_gt));
	    clear_type_list(&type_list);
	    திரும்பு FAIL;
	}
	++*arg;
	*arg = skipwhite_and_linebreak(*arg, evalarg);
    }

    res = eval9(arg, rettv, evalarg, want_string);

    எனில் (want_type != வெற்று)
    {
	எனில் (evaluate && res == OK)
	{
	    type_T *actual = typval2type(rettv, get_copyID(), &type_list,
							       TVTT_DO_MEMBER);

	    எனில் (!equal_type(want_type, actual, 0))
	    {
		எனில் (want_type->tt_type == VAR_BOOL
					&& actual->tt_type != VAR_BOOL
					&& (actual->tt_flags & TTFLAG_BOOL_OK))
		{
		    முழு n = tv2bool(rettv);

		    // can use "0" and "1" for boolean in some places
		    clear_tv(rettv);
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_எண் = n ? VVAL_TRUE : VVAL_FALSE;
		}
		இல்லையெனில்
		{
		    where_T where = WHERE_INIT;

		    res = check_type(want_type, actual, TRUE, where);
		}
	    }
	}
	clear_type_list(&type_list);
    }

    திரும்பு res;
}

    முழு
eval_leader(char_u **arg, முழு vim9)
{
    char_u	*s = *arg;
    char_u	*p = *arg;

    போது (*p == '!' || *p == '-' || *p == '+')
    {
	char_u *n = skipwhite(p + 1);

	// ++, --, -+ and +- are not accepted in Vim9 script
	எனில் (vim9 && (*p == '-' || *p == '+') && (*n == '-' || *n == '+'))
	{
	    semsg(_(e_invalid_expression_str), s);
	    திரும்பு FAIL;
	}
	p = n;
    }
    *arg = p;
    திரும்பு OK;
}

/*
 * Check for a predefined value "true", "false" and "null.*".
 * Return OK when recognized.
 */
    முழு
handle_predefined(char_u *s, முழு len, typval_T *rettv)
{
    தேர்வு (len)
    {
	நிலை 4: எனில் (STRNCMP(s, "true", 4) == 0)
		{
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_எண் = VVAL_TRUE;
		    திரும்பு OK;
		}
		எனில் (STRNCMP(s, "null", 4) == 0)
		{
		    rettv->v_type = VAR_SPECIAL;
		    rettv->vval.v_எண் = VVAL_NULL;
		    திரும்பு OK;
		}
		நிறுத்து;
	நிலை 5: எனில் (STRNCMP(s, "false", 5) == 0)
		{
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_எண் = VVAL_FALSE;
		    திரும்பு OK;
		}
		நிறுத்து;
	நிலை 8: எனில் (STRNCMP(s, "null_job", 8) == 0)
		{
#ifdef FEAT_JOB_CHANNEL
		    rettv->v_type = VAR_JOB;
		    rettv->vval.v_job = வெற்று;
#இல்லையெனில்
		    rettv->v_type = VAR_SPECIAL;
		    rettv->vval.v_எண் = VVAL_NULL;
#endif
		    திரும்பு OK;
		}
		நிறுத்து;
	நிலை 9:
		எனில் (STRNCMP(s, "null_", 5) != 0)
		    நிறுத்து;
		// null_list
		எனில் (STRNCMP(s + 5, "list", 4) == 0)
		{
		    rettv->v_type = VAR_LIST;
		    rettv->vval.v_list = வெற்று;
		    திரும்பு OK;
		}
		// null_dict
		எனில் (STRNCMP(s + 5, "dict", 4) == 0)
		{
		    rettv->v_type = VAR_DICT;
		    rettv->vval.v_dict = வெற்று;
		    திரும்பு OK;
		}
		// null_blob
		எனில் (STRNCMP(s + 5, "blob", 4) == 0)
		{
		    rettv->v_type = VAR_BLOB;
		    rettv->vval.v_blob = வெற்று;
		    திரும்பு OK;
		}
		நிறுத்து;
	நிலை 10:
		எனில் (STRNCMP(s, "null_", 5) != 0)
		    நிறுத்து;
		// null_class
		எனில் (STRNCMP(s + 5, "class", 5) == 0)
		{
		    rettv->v_type = VAR_CLASS;
		    rettv->vval.v_class = வெற்று;
		    திரும்பு OK;
		}
		எனில் (STRNCMP(s + 5, "tuple", 5) == 0)
		{
		    rettv->v_type = VAR_TUPLE;
		    rettv->vval.v_tuple = வெற்று;
		    திரும்பு OK;
		}
		நிறுத்து;
	நிலை 11: எனில் (STRNCMP(s, "null_string", 11) == 0)
		{
		    rettv->v_type = VAR_STRING;
		    rettv->vval.v_string = வெற்று;
		    திரும்பு OK;
		}
		எனில் (STRNCMP(s, "null_object", 11) == 0)
		{
		    rettv->v_type = VAR_OBJECT;
		    rettv->vval.v_object = வெற்று;
		    திரும்பு OK;
		}
		நிறுத்து;
	நிலை 12:
		எனில் (STRNCMP(s, "null_channel", 12) == 0)
		{
#ifdef FEAT_JOB_CHANNEL
		    rettv->v_type = VAR_CHANNEL;
		    rettv->vval.v_channel = வெற்று;
#இல்லையெனில்
		    rettv->v_type = VAR_SPECIAL;
		    rettv->vval.v_எண் = VVAL_NULL;
#endif
		    திரும்பு OK;
		}
		எனில் (STRNCMP(s, "null_partial", 12) == 0)
		{
		    rettv->v_type = VAR_PARTIAL;
		    rettv->vval.v_partial = வெற்று;
		    திரும்பு OK;
		}
		நிறுத்து;
	நிலை 13: எனில் (STRNCMP(s, "null_function", 13) == 0)
		{
		    rettv->v_type = VAR_FUNC;
		    rettv->vval.v_string = வெற்று;
		    திரும்பு OK;
		}
		நிறுத்து;
    }
    திரும்பு FAIL;
}

/*
 * Handle register contents: @r.
 */
    நிலையான வெறுமை
eval9_reg_contents(
    char_u	**arg,
    typval_T	*rettv,
    முழு		evaluate)
{
    முழு		vim9script = in_vim9script();

    ++*arg;	// skip '@'
    எனில் (evaluate)
    {
	எனில் (vim9script && IS_WHITE_OR_NUL(**arg))
	    semsg(_(e_syntax_error_at_str), *arg);
	இல்லையெனில் எனில் (vim9script && !valid_yank_reg(**arg, FALSE))
	    emsg_invreg(**arg);
	இல்லையெனில்
	{
	    rettv->v_type = VAR_STRING;
	    rettv->vval.v_string = get_reg_contents(**arg,
		    GREG_கோவை_SRC);
	}
    }
    எனில் (**arg != NUL)
	++*arg;
}

/*
 * Handle a nested expression: (expression) or lambda: (arg) => expr
 */
    நிலையான முழு
eval9_nested_கோவை(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    முழு		evaluate)
{
    முழு		ret = NOTDONE;
    முழு		vim9script = in_vim9script();

    எனில் (vim9script)
    {
	ret = get_lambda_tv(arg, rettv, TRUE, evalarg, வெற்று);
	எனில் (ret == OK && evaluate)
	{
	    ufunc_T *ufunc = rettv->vval.v_partial->pt_func;

	    // Compile it here to get the return type.  The return
	    // type is optional, when it's missing use t_unknown.
	    // This is recognized in compile_return().
	    எனில் (ufunc->uf_ret_type->tt_type == VAR_VOID)
		ufunc->uf_ret_type = &t_unknown;
	    எனில் (compile_def_function(ufunc, FALSE,
				get_compile_type(ufunc), வெற்று) == FAIL)
	    {
		clear_tv(rettv);
		ret = FAIL;
	    }
	}
    }
    எனில் (ret == NOTDONE)
    {
	*arg = skipwhite_and_linebreak(*arg + 1, evalarg);
	எனில் (**arg == ')')
	    // empty tuple
	    ret = eval_tuple(arg, rettv, evalarg, TRUE);
	இல்லையெனில்
	{
	    ret = eval1(arg, rettv, evalarg);	// recursive!
	    எனில் (ret != OK)
		திரும்பு ret;

	    *arg = skipwhite_and_linebreak(*arg, evalarg);

	    எனில் (**arg == ',')
		// tuple
		ret = eval_tuple(arg, rettv, evalarg, TRUE);
	    இல்லையெனில் எனில் (**arg == ')')
		++*arg;
	    இல்லையெனில் எனில் (ret == OK)
	    {
		emsg(_(e_missing_closing_paren));
		clear_tv(rettv);
		ret = FAIL;
	    }
	}
    }

    திரும்பு ret;
}

/*
* Handle be a variable or function name.
* Can also be a curly-braces kind of name: {expr}.
*/
    நிலையான முழு
eval9_var_func_name(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    முழு		evaluate,
    char_u	**name_start)
{
    char_u	*s;
    முழு		len;
    char_u	*alias;
    முழு		ret = OK;
    முழு		vim9script = in_vim9script();

    s = *arg;
    len = get_name_len(arg, &alias, evaluate, TRUE);
    எனில் (alias != வெற்று)
	s = alias;

    எனில் (len <= 0)
	ret = FAIL;
    இல்லையெனில்
    {
	முழு	flags = evalarg == வெற்று ? 0 : evalarg->eval_flags;

	எனில் (evaluate && vim9script && len == 1 && *s == '_')
	{
	    emsg(_(e_cannot_use_underscore_here));
	    ret = FAIL;
	}
	இல்லையெனில் எனில் (evaluate && vim9script && len > 2
						&& s[0] == 's' && s[1] == ':')
	{
	    semsg(_(e_cannot_use_s_colon_in_vim9_script_str), s);
	    ret = FAIL;
	}
	இல்லையெனில் எனில் ((vim9script ? **arg : *skipwhite(*arg)) == '('
				|| (vim9script && generic_func_call(arg)))
	{
	    // "name(..."  recursive!
	    *arg = skipwhite(*arg);
	    ret = eval_func(arg, evalarg, s, len, rettv, flags, வெற்று);
	}
	இல்லையெனில் எனில் (evaluate)
	{
	    // get the value of "true", "false", etc. or a variable
	    ret = FAIL;
	    எனில் (vim9script)
		ret = handle_predefined(s, len, rettv);
	    எனில் (ret == FAIL)
	    {
		*name_start = s;
		ret = eval_variable(s, len, 0, rettv, வெற்று,
					EVAL_VAR_VERBOSE + EVAL_VAR_IMPORT);

		// skip the generic function arguments (if present)
		// they are already processed by eval_variable
		எனில் (ret == OK && vim9script && **arg == '<'
						&& rettv->v_type == VAR_FUNC)
		    ret = skip_generic_func_type_args(arg);
	    }
	}
	இல்லையெனில்
	{
	    // skip the name
	    check_vars(s, len);

	    // Skip the type arguments of a generic function, its name starts
	    // with an uppercase letter.
	    எனில் (vim9script && **arg == '<' && ASCII_ISUPPER(*s))
		skip_generic_func_type_args(arg);
	    ret = OK;
	}
    }
    vim_free(alias);

    திரும்பு ret;
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
    நிலையான முழு
eval9(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    முழு		want_string)	// after "." operator
{
    முழு		evaluate = evalarg != வெற்று
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    char_u	*name_start = வெற்று;
    char_u	*start_leader, *end_leader;
    முழு		ret = OK;
    நிலையான முழு	recurse = 0;
    முழு		vim9script = in_vim9script();

    /*
     * Initialise variable so that clear_tv() can't mistake this for a
     * string and free a string that isn't there.
     */
    rettv->v_type = VAR_UNKNOWN;

    /*
     * Skip '!', '-' and '+' characters.  They are handled later.
     */
    start_leader = *arg;
    எனில் (eval_leader(arg, vim9script) == FAIL)
	திரும்பு FAIL;
    end_leader = *arg;

    எனில் (**arg == '.' && (!SAFE_isdigit(*(*arg + 1)) || in_old_script(2)))
    {
	semsg(_(e_invalid_expression_str), *arg);
	++*arg;
	திரும்பு FAIL;
    }

    // Limit recursion to 1000 levels.  At least at 10000 we run out of stack
    // and crash.  With MSVC the stack is smaller.
    எனில் (recurse ==
#ifdef _MSC_VER
		    300
#இல்லையெனில்
		    1000
#endif
		    )
    {
	semsg(_(e_expression_too_recursive_str), *arg);
	திரும்பு FAIL;
    }
    ++recurse;

    தேர்வு (**arg)
    {
    /*
     * Number constant.
     */
    நிலை '0':
    நிலை '1':
    நிலை '2':
    நிலை '3':
    நிலை '4':
    நிலை '5':
    நிலை '6':
    நிலை '7':
    நிலை '8':
    நிலை '9':
    நிலை '.':	ret = eval_எண்(arg, rettv, evaluate, want_string);

		// Apply prefixed "-" and "+" now.  Matters especially when
		// "->" follows.
		எனில் (ret == OK && evaluate && end_leader > start_leader
						  && rettv->v_type != VAR_BLOB)
		    ret = eval9_leader(rettv, TRUE, start_leader, &end_leader);
		நிறுத்து;

    /*
     * String constant: "string".
     */
    நிலை '"':	ret = eval_string(arg, rettv, evaluate, FALSE);
		நிறுத்து;

    /*
     * Literal string constant: 'str''ing'.
     */
    நிலை '\'':	ret = eval_lit_string(arg, rettv, evaluate, FALSE);
		நிறுத்து;

    /*
     * List: [expr, expr]
     */
    நிலை '[':	ret = eval_list(arg, rettv, evalarg, TRUE);
		நிறுத்து;

    /*
     * Literal Dictionary: #{key: val, key: val}
     */
    நிலை '#':	ret = eval_lit_dict(arg, rettv, evalarg);
		நிறுத்து;

    /*
     * Lambda: {arg, arg -> expr}
     * Dictionary: {'key': val, 'key': val}
     */
    நிலை '{':	எனில் (vim9script)
		    ret = NOTDONE;
		இல்லையெனில்
		    ret = get_lambda_tv(arg, rettv, vim9script, evalarg, வெற்று);
		எனில் (ret == NOTDONE)
		    ret = eval_dict(arg, rettv, evalarg, FALSE);
		நிறுத்து;

    /*
     * Option value: &name
     */
    நிலை '&':	ret = eval_option(arg, rettv, evaluate);
		நிறுத்து;

    /*
     * Environment variable: $VAR.
     * Interpolated string: $"string" or $'string'.
     */
    நிலை '$':	எனில் ((*arg)[1] == '"' || (*arg)[1] == '\'')
		    ret = eval_interp_string(arg, rettv, evaluate);
		இல்லையெனில்
		    ret = eval_env_var(arg, rettv, evaluate);
		நிறுத்து;

    /*
     * Register contents: @r.
     */
    நிலை '@':	eval9_reg_contents(arg, rettv, evaluate);
		நிறுத்து;

    /*
     * nested expression: (expression).
     * or lambda: (arg) => expr
     * or tuple
     */
    நிலை '(':	ret = eval9_nested_கோவை(arg, rettv, evalarg, evaluate);
		நிறுத்து;

    இயல்பு:	ret = NOTDONE;
		நிறுத்து;
    }

    எனில் (ret == NOTDONE)
    {
	/*
	 * Must be a variable or function name.
	 * Can also be a curly-braces kind of name: {expr}.
	 */
	ret = eval9_var_func_name(arg, rettv, evalarg, evaluate, &name_start);
    }

    // Handle following '[', '(' and '.' for expr[expr], expr.name,
    // expr(expr), expr->name(expr)
    எனில் (ret == OK)
	ret = handle_subscript(arg, name_start, rettv, evalarg, evaluate);

    /*
     * Apply logical NOT and unary '-', from right to left, ignore '+'.
     */
    எனில் (ret == OK && evaluate && end_leader > start_leader)
	ret = eval9_leader(rettv, FALSE, start_leader, &end_leader);

    --recurse;
    திரும்பு ret;
}

/*
 * Apply the leading "!" and "-" before an eval9 expression to "rettv".
 * When "numeric_only" is TRUE only handle "+" and "-".
 * Adjusts "end_leaderp" until it is at "start_leader".
 */
    நிலையான முழு
eval9_leader(
	typval_T    *rettv,
	முழு	    numeric_only,
	char_u	    *start_leader,
	char_u	    **end_leaderp)
{
    char_u	*end_leader = *end_leaderp;
    முழு		ret = OK;
    முழு		error = FALSE;
    varnumber_T val = 0;
    vartype_T	type = rettv->v_type;
    முழு		vim9script = in_vim9script();
    float_T	    f = 0.0;

    எனில் (rettv->v_type == VAR_FLOAT)
	f = rettv->vval.v_float;
    இல்லையெனில்
    {
	போது (VIM_ISWHITE(end_leader[-1]))
	    --end_leader;
	எனில் (vim9script && end_leader[-1] == '!')
	{
	    // Is this typeval supported with the ! operator?
	    எனில் (check_typval_is_மதிப்பு(rettv) == FAIL)
	    {
		clear_tv(rettv);
		திரும்பு FAIL;
	    }
	    val = tv2bool(rettv);
	}
	இல்லையெனில்
	    val = tv_get_எண்_chk(rettv, &error);
    }
    எனில் (error)
    {
	clear_tv(rettv);
	ret = FAIL;
    }
    இல்லையெனில்
    {
	போது (end_leader > start_leader)
	{
	    --end_leader;
	    எனில் (*end_leader == '!')
	    {
		எனில் (numeric_only)
		{
		    ++end_leader;
		    நிறுத்து;
		}
		எனில் (rettv->v_type == VAR_FLOAT)
		{
		    rettv->v_type = VAR_BOOL;
		    val = f == 0.0 ? VVAL_TRUE : VVAL_FALSE;
		    type = VAR_BOOL;
		}
		இல்லையெனில்
		{
		    val = !val;
		    type = VAR_BOOL;
		}
	    }
	    இல்லையெனில் எனில் (*end_leader == '-')
	    {
		எனில் (rettv->v_type == VAR_FLOAT)
		    f = -f;
		இல்லையெனில்
		{
		    val = -val;
		    type = VAR_எண்;
		}
	    }
	}
	எனில் (rettv->v_type == VAR_FLOAT)
	{
	    clear_tv(rettv);
	    rettv->vval.v_float = f;
	}
	இல்லையெனில்
	{
	    clear_tv(rettv);
	    எனில் (vim9script)
		rettv->v_type = type;
	    இல்லையெனில்
		rettv->v_type = VAR_எண்;
	    rettv->vval.v_எண் = val;
	}
    }
    *end_leaderp = end_leader;
    திரும்பு ret;
}

/*
 * Call the function referred to in "rettv".
 */
    நிலையான முழு
call_func_rettv(
	char_u	    **arg,
	evalarg_T   *evalarg,
	typval_T    *rettv,
	முழு	    evaluate,
	dict_T	    *selfdict,
	typval_T    *basetv)
{
    partial_T	*pt = வெற்று;
    funcexe_T	funcexe;
    typval_T	functv;
    char_u	*s;
    முழு		ret;

    // need to copy the funcref so that we can clear rettv
    எனில் (evaluate)
    {
	functv = *rettv;
	rettv->v_type = VAR_UNKNOWN;

	// Invoke the function.  Recursive!
	எனில் (functv.v_type == VAR_PARTIAL)
	{
	    pt = functv.vval.v_partial;
	    s = partial_name(pt);
	}
	இல்லையெனில்
	{
	    s = functv.vval.v_string;
	    எனில் (s == வெற்று || *s == NUL)
	    {
		emsg(_(e_empty_function_name));
		ret = FAIL;
		செல் theend;
	    }
	}
    }
    இல்லையெனில்
	s = (char_u *)"";

    CLEAR_FIELD(funcexe);
    funcexe.fe_firstline = curwin->w_cursor.lnum;
    funcexe.fe_lastline = curwin->w_cursor.lnum;
    funcexe.fe_evaluate = evaluate;
    funcexe.fe_partial = pt;
    எனில் (pt != வெற்று)
    {
	funcexe.fe_object = pt->pt_obj;
	எனில் (funcexe.fe_object != வெற்று)
	    ++funcexe.fe_object->obj_refcount;
    }
    funcexe.fe_selfdict = selfdict;
    funcexe.fe_basetv = basetv;
    எனில் (evalarg != வெற்று)
	funcexe.fe_cctx = evalarg->eval_cctx;
    ret = get_func_tv(s, -1, rettv, arg, evalarg, &funcexe);

theend:
    // Clear the funcref afterwards, so that deleting it while
    // evaluating the arguments is possible (see test55).
    எனில் (evaluate)
	clear_tv(&functv);

    திரும்பு ret;
}

/*
 * Evaluate "->method()".
 * "*arg" points to "method".
 * Returns FAIL or OK. "*arg" is advanced to after the ')'.
 */
    நிலையான முழு
eval_lambda(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    முழு		verbose)	// give error messages
{
    முழு		evaluate = evalarg != வெற்று
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    typval_T	base = *rettv;
    முழு		ret;

    rettv->v_type = VAR_UNKNOWN;

    எனில் (**arg == '{')
    {
	// ->{lambda}()
	ret = get_lambda_tv(arg, rettv, FALSE, evalarg, வெற்று);
    }
    இல்லையெனில்
    {
	// ->(lambda)()
	++*arg;
	ret = eval1(arg, rettv, evalarg);
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	எனில் (**arg != ')')
	{
	    emsg(_(e_missing_closing_paren));
	    திரும்பு FAIL;
	}
	எனில் (rettv->v_type != VAR_STRING && rettv->v_type != VAR_FUNC
					       && rettv->v_type != VAR_PARTIAL)
	{
	    emsg(_(e_string_or_function_required_for_arrow_parens_கோவை));
	    திரும்பு FAIL;
	}
	++*arg;
    }
    எனில் (ret != OK)
	திரும்பு FAIL;

    எனில் (**arg != '(')
    {
	எனில் (verbose)
	{
	    எனில் (*skipwhite(*arg) == '(')
		emsg(_(e_no_white_space_allowed_before_parenthesis));
	    இல்லையெனில்
		semsg(_(e_missing_parenthesis_str), "lambda");
	}
	clear_tv(rettv);
	ret = FAIL;
    }
    இல்லையெனில்
	ret = call_func_rettv(arg, evalarg, rettv, evaluate, வெற்று, &base);

    // Clear the funcref afterwards, so that deleting it while
    // evaluating the arguments is possible (see test55).
    எனில் (evaluate)
	clear_tv(&base);

    திரும்பு ret;
}

/*
 * Evaluate "->method()".
 * "*arg" points to "method".
 * Returns FAIL or OK. "*arg" is advanced to after the ')'.
 */
    நிலையான முழு
eval_method(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    முழு		verbose)	// give error messages
{
    char_u	*name;
    long	len;
    char_u	*alias;
    char_u	*tofree = வெற்று;
    typval_T	base = *rettv;
    முழு		ret = OK;
    முழு		evaluate = evalarg != வெற்று
				      && (evalarg->eval_flags & EVAL_EVALUATE);

    rettv->v_type = VAR_UNKNOWN;

    name = *arg;
    len = get_name_len(arg, &alias, evaluate, evaluate);
    எனில் (alias != வெற்று)
	name = alias;

    எனில் (len <= 0)
    {
	எனில் (verbose)
	    emsg(_(e_missing_name_after_method));
	ret = FAIL;
    }
    இல்லையெனில்
    {
	char_u *paren;

	// If there is no "(" immediately following, but there is further on,
	// it can be "import.Func()", "dict.Func()", "list[nr]", etc.
	// Does not handle anything where "(" is part of the expression.
	*arg = skipwhite(*arg);

	எனில் (**arg != '(' && alias == வெற்று
				    && (paren = vim_strchr(*arg, '(')) != வெற்று)
	{
	    *arg = name;

	    // Truncate the name at the "(".  Avoid trying to get another line
	    // by making "getline" NULL.
	    *paren = NUL;
	    char_u	*(*getline)(முழு, வெறுமை *, முழு, getline_opt_T) = வெற்று;
	    எனில் (evalarg != வெற்று)
	    {
		getline = evalarg->eval_getline;
		evalarg->eval_getline = வெற்று;
	    }

	    char_u *deref = deref_function_name(arg, &tofree, evalarg, verbose);
	    எனில் (deref == வெற்று)
	    {
		*arg = name + len;
		ret = FAIL;
	    }
	    இல்லையெனில்
	    {
		name = deref;
		len = (long)STRLEN(name);
	    }

	    *paren = '(';
	    எனில் (getline != வெற்று)
		evalarg->eval_getline = getline;
	}

	எனில் (ret == OK)
	{
	    *arg = skipwhite(*arg);

	    எனில் (**arg != '(')
	    {
		எனில் (verbose)
		    semsg(_(e_missing_parenthesis_str), name);
		ret = FAIL;
	    }
	    இல்லையெனில் எனில் (VIM_ISWHITE((*arg)[-1]))
	    {
		எனில் (verbose)
		    emsg(_(e_no_white_space_allowed_before_parenthesis));
		ret = FAIL;
	    }
	    இல்லையெனில்
		ret = eval_func(arg, evalarg, name, len, rettv,
					  evaluate ? EVAL_EVALUATE : 0, &base);
	}
    }

    // Clear the funcref afterwards, so that deleting it while
    // evaluating the arguments is possible (see test55).
    எனில் (evaluate)
	clear_tv(&base);
    vim_free(tofree);

    எனில் (alias != வெற்று)
	vim_free(alias);

    திரும்பு ret;
}

/*
 * Evaluate an "[expr]" or "[expr:expr]" index.  Also "dict.key".
 * "*arg" points to the '[' or '.'.
 * Returns FAIL or OK. "*arg" is advanced to after the ']'.
 */
    நிலையான முழு
eval_index(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    முழு		verbose)	// give error messages
{
    முழு		evaluate = evalarg != வெற்று
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    முழு		empty1 = FALSE, empty2 = FALSE;
    typval_T	var1, var2;
    முழு		range = FALSE;
    char_u	*சாவி = வெற்று;
    முழு		keylen = -1;
    முழு		vim9script = in_vim9script();

    எனில் (check_can_index(rettv, evaluate, verbose) == FAIL)
	திரும்பு FAIL;

    init_tv(&var1);
    init_tv(&var2);
    எனில் (**arg == '.')
    {
	/*
	 * dict.name
	 */
	சாவி = *arg + 1;
	க்கு (keylen = 0; eval_isdictc(சாவி[keylen]); ++keylen)
	    ;
	எனில் (keylen == 0)
	    திரும்பு FAIL;
	எனில் (vim9script && சாவி[keylen] == '<')
	{
	    // skip generic type arguments
	    char_u	*p = &சாவி[keylen];

	    எனில் (skip_generic_func_type_args(&p) == FAIL)
		திரும்பு FAIL;
	    keylen = p - சாவி;
	}
	*arg = சாவி + keylen;
    }
    இல்லையெனில்
    {
	/*
	 * something[idx]
	 *
	 * Get the (first) variable from inside the [].
	 */
	*arg = skipwhite_and_linebreak(*arg + 1, evalarg);
	எனில் (**arg == ':')
	    empty1 = TRUE;
	இல்லையெனில் எனில் (eval1(arg, &var1, evalarg) == FAIL)	// recursive!
	    திரும்பு FAIL;
	இல்லையெனில் எனில் (vim9script && **arg == ':')
	{
	    semsg(_(e_white_space_required_before_and_after_str_at_str),
								    ":", *arg);
	    clear_tv(&var1);
	    திரும்பு FAIL;
	}
	இல்லையெனில் எனில் (evaluate)
	{
	    முழு error = FALSE;

	    // allow for indexing with float
	    எனில் (vim9script && rettv->v_type == VAR_DICT
						   && var1.v_type == VAR_FLOAT)
	    {
		var1.vval.v_string = typval_tostring(&var1, TRUE);
		var1.v_type = VAR_STRING;
	    }

	    எனில் (vim9script && (rettv->v_type == VAR_LIST
						|| rettv->v_type == VAR_TUPLE))
		tv_get_எண்_chk(&var1, &error);
	    இல்லையெனில்
		error = tv_get_string_chk(&var1) == வெற்று;
	    எனில் (error)
	    {
		// not a number or string
		clear_tv(&var1);
		திரும்பு FAIL;
	    }
	}

	/*
	 * Get the second variable from inside the [:].
	 */
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	எனில் (**arg == ':')
	{
	    range = TRUE;
	    ++*arg;
	    எனில் (vim9script && !IS_WHITE_OR_NUL(**arg) && **arg != ']')
	    {
		semsg(_(e_white_space_required_before_and_after_str_at_str),
								":", *arg - 1);
		எனில் (!empty1)
		    clear_tv(&var1);
		திரும்பு FAIL;
	    }
	    *arg = skipwhite_and_linebreak(*arg, evalarg);
	    எனில் (**arg == ']')
		empty2 = TRUE;
	    இல்லையெனில் எனில் (eval1(arg, &var2, evalarg) == FAIL)	// recursive!
	    {
		எனில் (!empty1)
		    clear_tv(&var1);
		திரும்பு FAIL;
	    }
	    இல்லையெனில் எனில் (evaluate && tv_get_string_chk(&var2) == வெற்று)
	    {
		// not a number or string
		எனில் (!empty1)
		    clear_tv(&var1);
		clear_tv(&var2);
		திரும்பு FAIL;
	    }
	}

	// Check for the ']'.
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	எனில் (**arg != ']')
	{
	    எனில் (verbose)
		emsg(_(e_missing_closing_square_brace));
	    clear_tv(&var1);
	    எனில் (range)
		clear_tv(&var2);
	    திரும்பு FAIL;
	}
	*arg = *arg + 1;	// skip over the ']'
    }

    எனில் (evaluate)
    {
	முழு res = eval_index_inner(rettv, range,
		empty1 ? வெற்று : &var1, empty2 ? வெற்று : &var2, FALSE,
		சாவி, keylen, verbose);

	எனில் (!empty1)
	    clear_tv(&var1);
	எனில் (range)
	    clear_tv(&var2);
	திரும்பு res;
    }
    திரும்பு OK;
}

/*
 * Check if "rettv" can have an [index] or [sli:ce]
 */
    முழு
check_can_index(typval_T *rettv, முழு evaluate, முழு verbose)
{
    தேர்வு (rettv->v_type)
    {
	நிலை VAR_FUNC:
	நிலை VAR_PARTIAL:
	    எனில் (verbose)
		emsg(_(e_cannot_index_a_funcref));
	    திரும்பு FAIL;
	நிலை VAR_FLOAT:
	    எனில் (verbose)
		emsg(_(e_using_float_as_string));
	    திரும்பு FAIL;
	நிலை VAR_BOOL:
	நிலை VAR_SPECIAL:
	நிலை VAR_JOB:
	நிலை VAR_CHANNEL:
	நிலை VAR_INSTR:
	நிலை VAR_OBJECT:
	    எனில் (verbose)
		emsg(_(e_cannot_index_special_variable));
	    திரும்பு FAIL;
	நிலை VAR_CLASS:
	நிலை VAR_TYPEALIAS:
	    எனில் (verbose)
		check_typval_is_மதிப்பு(rettv);
	    திரும்பு FAIL;
	நிலை VAR_UNKNOWN:
	நிலை VAR_ANY:
	நிலை VAR_VOID:
	    எனில் (evaluate)
	    {
		emsg(_(e_cannot_index_special_variable));
		திரும்பு FAIL;
	    }
	    // FALLTHROUGH

	நிலை VAR_STRING:
	நிலை VAR_LIST:
	நிலை VAR_TUPLE:
	நிலை VAR_DICT:
	நிலை VAR_BLOB:
	    நிறுத்து;
	நிலை VAR_எண்:
	    எனில் (in_vim9script())
		emsg(_(e_cannot_index_எண்));
	    நிறுத்து;
    }
    திரும்பு OK;
}

/*
 * Apply index or range to "rettv".
 * "var1" is the first index, NULL for [:expr].
 * "var2" is the second index, NULL for [expr] and [expr: ]
 * "exclusive" is TRUE for slice(): second index is exclusive, use character
 * index for string.
 * Alternatively, "key" is not NULL, then key[keylen] is the dict index.
 */
    முழு
eval_index_inner(
	typval_T    *rettv,
	முழு	    is_range,
	typval_T    *var1,
	typval_T    *var2,
	முழு	    exclusive,
	char_u	    *சாவி,
	முழு	    keylen,
	முழு	    verbose)
{
    varnumber_T	    n1, n2 = 0;
    long	    len;

    n1 = 0;
    எனில் (var1 != வெற்று && rettv->v_type != VAR_DICT)
	n1 = tv_get_எண்(var1);

    எனில் (is_range)
    {
	எனில் (rettv->v_type == VAR_DICT)
	{
	    எனில் (verbose)
		emsg(_(e_cannot_slice_dictionary));
	    திரும்பு FAIL;
	}
	எனில் (var2 != வெற்று)
	    n2 = tv_get_எண்(var2);
	இல்லையெனில்
	    n2 = VARNUM_MAX;
    }

    தேர்வு (rettv->v_type)
    {
	நிலை VAR_UNKNOWN:
	நிலை VAR_ANY:
	நிலை VAR_VOID:
	நிலை VAR_FUNC:
	நிலை VAR_PARTIAL:
	நிலை VAR_FLOAT:
	நிலை VAR_BOOL:
	நிலை VAR_SPECIAL:
	நிலை VAR_JOB:
	நிலை VAR_CHANNEL:
	நிலை VAR_INSTR:
	நிலை VAR_CLASS:
	நிலை VAR_OBJECT:
	நிலை VAR_TYPEALIAS:
	    நிறுத்து; // not evaluating, skipping over subscript

	நிலை VAR_எண்:
	நிலை VAR_STRING:
	    {
		char_u	*s = tv_get_string(rettv);

		len = (long)STRLEN(s);
		எனில் (in_vim9script() || exclusive)
		{
		    எனில் (is_range)
			s = string_slice(s, n1, n2, exclusive);
		    இல்லையெனில்
			s = char_from_string(s, n1);
		}
		இல்லையெனில் எனில் (is_range)
		{
		    // The resulting variable is a substring.  If the indexes
		    // are out of range the result is empty.
		    எனில் (n1 < 0)
		    {
			n1 = len + n1;
			எனில் (n1 < 0)
			    n1 = 0;
		    }
		    எனில் (n2 < 0)
			n2 = len + n2;
		    இல்லையெனில் எனில் (n2 >= len)
			n2 = len;
		    எனில் (n1 >= len || n2 < 0 || n1 > n2)
			s = வெற்று;
		    இல்லையெனில்
			s = vim_strnsave(s + n1, n2 - n1 + 1);
		}
		இல்லையெனில்
		{
		    // The resulting variable is a string of a single
		    // character.  If the index is too big or negative the
		    // result is empty.
		    எனில் (n1 >= len || n1 < 0)
			s = வெற்று;
		    இல்லையெனில்
			s = vim_strnsave(s + n1, 1);
		}
		clear_tv(rettv);
		rettv->v_type = VAR_STRING;
		rettv->vval.v_string = s;
	    }
	    நிறுத்து;

	நிலை VAR_BLOB:
	    blob_slice_or_index(rettv->vval.v_blob, is_range, n1, n2,
							     exclusive, rettv);
	    நிறுத்து;

	நிலை VAR_LIST:
	    எனில் (var1 == வெற்று)
		n1 = 0;
	    எனில் (var2 == வெற்று)
		n2 = VARNUM_MAX;
	    எனில் (list_slice_or_index(rettv->vval.v_list,
			  is_range, n1, n2, exclusive, rettv, verbose) == FAIL)
		திரும்பு FAIL;
	    நிறுத்து;

	நிலை VAR_TUPLE:
	    எனில் (var1 == வெற்று)
		n1 = 0;
	    எனில் (var2 == வெற்று)
		n2 = VARNUM_MAX;
	    எனில் (tuple_slice_or_index(rettv->vval.v_tuple,
			  is_range, n1, n2, exclusive, rettv, verbose) == FAIL)
		திரும்பு FAIL;
	    நிறுத்து;

	நிலை VAR_DICT:
	    {
		dictitem_T	*item;
		typval_T	tmp;

		எனில் (சாவி == வெற்று)
		{
		    சாவி = tv_get_string_chk(var1);
		    எனில் (சாவி == வெற்று)
			திரும்பு FAIL;
		}

		item = dict_find(rettv->vval.v_dict, சாவி, keylen);

		எனில் (item == வெற்று)
		{
		    எனில் (verbose)
		    {
			எனில் (keylen > 0)
			    சாவி[keylen] = NUL;
			semsg(_(e_சாவி_not_present_in_dictionary_str), சாவி);
		    }
		    திரும்பு FAIL;
		}

		copy_tv(&item->di_tv, &tmp);
		clear_tv(rettv);
		*rettv = tmp;
	    }
	    நிறுத்து;
    }
    திரும்பு OK;
}

/*
 * Return the function name of partial "pt".
 */
    char_u *
partial_name(partial_T *pt)
{
    எனில் (pt != வெற்று)
    {
	எனில் (pt->pt_name != வெற்று)
	    திரும்பு pt->pt_name;
	எனில் (pt->pt_func != வெற்று)
	    திரும்பு pt->pt_func->uf_name;
    }
    திரும்பு (char_u *)"";
}

    நிலையான வெறுமை
partial_free(partial_T *pt)
{
    முழு i;

    க்கு (i = 0; i < pt->pt_argc; ++i)
	clear_tv(&pt->pt_argv[i]);
    vim_free(pt->pt_argv);
    dict_unref(pt->pt_dict);
    எனில் (pt->pt_name != வெற்று)
    {
	func_unref(pt->pt_name);
	vim_free(pt->pt_name);
    }
    இல்லையெனில்
	func_ptr_unref(pt->pt_func);
    எனில் (pt->pt_obj != வெற்று)
	object_unref(pt->pt_obj);

    // "out_up" is no longer used, decrement refcount on partial that owns it.
    partial_unref(pt->pt_outer.out_up_partial);

    // Using pt_outer from another partial.
    partial_unref(pt->pt_outer_partial);

    // Decrease the reference count for the context of a closure.  If down
    // to the minimum it may be time to free it.
    எனில் (pt->pt_funcstack != வெற்று)
    {
	--pt->pt_funcstack->fs_refcount;
	funcstack_check_refcount(pt->pt_funcstack);
    }
    // Similarly for loop variables.
    க்கு (i = 0; i < MAX_LOOP_DEPTH; ++i)
	எனில் (pt->pt_loopvars[i] != வெற்று)
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
    வெறுமை
partial_unref(partial_T *pt)
{
    எனில் (pt == வெற்று)
	திரும்பு;

    முழு	done = FALSE;

    எனில் (--pt->pt_refcount <= 0)
	partial_free(pt);

    // If the reference count goes down to one, the funcstack may be the
    // only reference and can be freed if no other partials reference it.
    இல்லையெனில் எனில் (pt->pt_refcount == 1)
    {
	// careful: if the funcstack is freed it may contain this partial
	// and it gets freed as well
	எனில் (pt->pt_funcstack != வெற்று)
	    done = funcstack_check_refcount(pt->pt_funcstack);

	எனில் (!done)
	{
	    முழு	depth;

	    க்கு (depth = 0; depth < MAX_LOOP_DEPTH; ++depth)
		எனில் (pt->pt_loopvars[depth] != வெற்று
			&& loopvars_check_refcount(pt->pt_loopvars[depth]))
		    நிறுத்து;
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
    நிலையான char_u *
string_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    முழு		echo_style,
    முழு		composite_val)
{
    char_u	*r = வெற்று;

    எனில் (echo_style && !composite_val)
    {
	*tofree = வெற்று;
	r = tv->vval.v_string;
	எனில் (r == வெற்று)
	    r = (char_u *)"";
    }
    இல்லையெனில்
    {
	*tofree = string_quote(tv->vval.v_string, FALSE);
	r = *tofree;
    }

    திரும்பு r;
}

/*
 * Return a textual representation of a function in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "echo_style" is FALSE, put quotes around the function name as
 * "function()", otherwise does not put quotes around function name.
 * May return NULL.
 */
    நிலையான char_u *
func_tv2string(typval_T *tv, char_u **tofree, முழு echo_style)
{
    char_u	*r = வெற்று;
    char_u	buf[MAX_FUNC_NAME_LEN];

    எனில் (echo_style)
    {
	*tofree = வெற்று;

	எனில் (tv->vval.v_string == வெற்று)
	    r = (char_u *)"function()";
	இல்லையெனில்
	{
	    r = make_ufunc_name_readable(tv->vval.v_string, buf,
							MAX_FUNC_NAME_LEN);
	    எனில் (r == buf)
		r = *tofree = vim_strsave(buf);
	}
    }
    இல்லையெனில்
    {
	char_u *s = வெற்று;

	எனில் (tv->vval.v_string != வெற்று)
	    s = make_ufunc_name_readable(tv->vval.v_string, buf,
							MAX_FUNC_NAME_LEN);

	r = *tofree = string_quote(s, TRUE);
    }

    திரும்பு r;
}

/*
 * Return a textual representation of the object method in "tv", a VAR_PARTIAL.
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "echo_style" is FALSE, put quotes around the function name as
 * "function()", otherwise does not put quotes around function name.
 * May return NULL.
 */
    நிலையான char_u *
method_tv2string(typval_T *tv, char_u **tofree, முழு echo_style)
{
    char_u	buf[MAX_FUNC_NAME_LEN];
    partial_T	*pt = tv->vval.v_partial;

    size_t len = vim_snprintf((எழுத்து *)buf, அளவு(buf), "<SNR>%d_%s.%s",
			   pt->pt_func->uf_script_ctx.sc_sid,
			   pt->pt_func->uf_class->class_name.string,
			   pt->pt_func->uf_name);
    எனில் (len >= அளவு(buf))
    {
	எனில் (echo_style)
	{
	    *tofree = வெற்று;
	    திரும்பு (char_u *)"function()";
	}
	இல்லையெனில்
	    திரும்பு *tofree = string_quote((char_u*)"", TRUE);
    }

    திரும்பு *tofree = echo_style ? vim_strsave(buf) : string_quote(buf, TRUE);
}

/*
 * Return a textual representation of a partial in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * "numbuf" is used for a number.  May return NULL.
 */
    நிலையான char_u *
partial_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    char_u	*numbuf,
    முழு		copyID)
{
    char_u	*r = வெற்று;
    partial_T	*pt;
    char_u	*fname;
    garray_T	ga;
    முழு		i;
    char_u	*tf;

    pt = tv->vval.v_partial;
    fname = string_quote(pt == வெற்று ? வெற்று : partial_name(pt), FALSE);

    ga_init2(&ga, 1, 100);
    GA_CONCAT_LITERAL(&ga, "function(");
    எனில் (fname != வெற்று)
    {
	// When using uf_name prepend "g:" for a global function.
	எனில் (pt != வெற்று && pt->pt_name == வெற்று && fname[0] == '\''
						&& vim_isupper(fname[1]))
	{
	    GA_CONCAT_LITERAL(&ga, "'g:");
	    ga_concat(&ga, fname + 1);
	}
	இல்லையெனில்
	    ga_concat(&ga, fname);
	vim_free(fname);
    }
    எனில் (pt != வெற்று && pt->pt_argc > 0)
    {
	GA_CONCAT_LITERAL(&ga, ", [");
	க்கு (i = 0; i < pt->pt_argc; ++i)
	{
	    எனில் (i > 0)
		GA_CONCAT_LITERAL(&ga, ", ");
	    ga_concat(&ga, tv2string(&pt->pt_argv[i], &tf, numbuf, copyID));
	    vim_free(tf);
	}
	GA_CONCAT_LITERAL(&ga, "]");
    }
    எனில் (pt != வெற்று && pt->pt_dict != வெற்று)
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

    திரும்பு r;
}

/*
 * Return a textual representation of a List in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "copyID" is not zero replace recursive lists with "...".  When
 * "restore_copyID" is FALSE, repeated items in lists are replaced with "...".
 * May return NULL.
 */
    நிலையான char_u *
list_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    முழு		copyID,
    முழு		restore_copyID)
{
    char_u	*r = வெற்று;

    எனில் (tv->vval.v_list == வெற்று)
    {
	// NULL list is equivalent to empty list.
	*tofree = வெற்று;
	r = (char_u *)"[]";
    }
    இல்லையெனில் எனில் (copyID != 0 && tv->vval.v_list->lv_copyID == copyID
	    && tv->vval.v_list->lv_len > 0)
    {
	*tofree = வெற்று;
	r = (char_u *)"[...]";
    }
    இல்லையெனில்
    {
	முழு old_copyID;
	எனில் (restore_copyID)
	    old_copyID = tv->vval.v_list->lv_copyID;

	tv->vval.v_list->lv_copyID = copyID;
	*tofree = list2string(tv, copyID, restore_copyID);
	எனில் (restore_copyID)
	    tv->vval.v_list->lv_copyID = old_copyID;
	r = *tofree;
    }

    திரும்பு r;
}

/*
 * Return a textual representation of a Tuple in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "copyID" is not zero replace recursive lists with "...".  When
 * "restore_copyID" is FALSE, repeated items in tuples are replaced with "...".
 * May return NULL.
 */
    நிலையான char_u *
tuple_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    முழு		copyID,
    முழு		restore_copyID)
{
    tuple_T	*tuple = tv->vval.v_tuple;
    char_u	*r = வெற்று;

    எனில் (tuple == வெற்று)
    {
	// NULL tuple is equivalent to an empty tuple.
	*tofree = வெற்று;
	r = (char_u *)"()";
    }
    இல்லையெனில் எனில் (copyID != 0 && tuple->tv_copyID == copyID
					&& tuple->tv_பொருட்கள்.ga_len > 0)
    {
	*tofree = வெற்று;
	r = (char_u *)"(...)";
    }
    இல்லையெனில்
    {
	முழு old_copyID;
	எனில் (restore_copyID)
	    old_copyID = tuple->tv_copyID;

	tuple->tv_copyID = copyID;
	*tofree = tuple2string(tv, copyID, restore_copyID);
	எனில் (restore_copyID)
	    tuple->tv_copyID = old_copyID;
	r = *tofree;
    }

    திரும்பு r;
}

/*
 * Return a textual representation of a Dict in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "copyID" is not zero replace recursive dicts with "...".
 * When "restore_copyID" is FALSE, repeated items in the dictionary are
 * replaced with "...".  May return NULL.
 */
    நிலையான char_u *
dict_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    முழு		copyID,
    முழு		restore_copyID)
{
    char_u	*r = வெற்று;

    எனில் (tv->vval.v_dict == வெற்று)
    {
	// NULL dict is equivalent to empty dict.
	*tofree = வெற்று;
	r = (char_u *)"{}";
    }
    இல்லையெனில் எனில் (copyID != 0 && tv->vval.v_dict->dv_copyID == copyID
	    && tv->vval.v_dict->dv_hashtab.ht_used != 0)
    {
	*tofree = வெற்று;
	r = (char_u *)"{...}";
    }
    இல்லையெனில்
    {
	முழு old_copyID;
	எனில் (restore_copyID)
	    old_copyID = tv->vval.v_dict->dv_copyID;

	tv->vval.v_dict->dv_copyID = copyID;
	*tofree = dict2string(tv, copyID, restore_copyID);
	எனில் (restore_copyID)
	    tv->vval.v_dict->dv_copyID = old_copyID;
	r = *tofree;
    }

    திரும்பு r;
}

/*
 * Return a textual representation of a job or a channel in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * "numbuf" is used for a number.
 * When "composite_val" is FALSE, put quotes around strings as "string()",
 * otherwise does not put quotes around strings.
 * May return NULL.
 */
    நிலையான char_u *
jobchan_tv2string(
    typval_T	*tv UNUSED,
    char_u	**tofree UNUSED,
    char_u	*numbuf UNUSED,
    முழு		composite_val UNUSED)
{
    char_u	*r = வெற்று;

#ifdef FEAT_JOB_CHANNEL
    *tofree = வெற்று;

    எனில் (tv->v_type == VAR_JOB)
	r = job_to_string_buf(tv, numbuf);
    இல்லையெனில்
	r = channel_to_string_buf(tv, numbuf);

    எனில் (composite_val)
    {
	*tofree = string_quote(r, FALSE);
	r = *tofree;
    }
#endif

    திரும்பு r;
}

/*
 * Return a textual representation of a class in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * May return NULL.
 */
    நிலையான char_u *
class_tv2string(typval_T *tv, char_u **tofree)
{
    char_u	*r = வெற்று;
    size_t	rsize;
    class_T	*cl = tv->vval.v_class;
    string_T	class_name = {(char_u *)"[unknown]", 9};
    string_T	s = {(char_u *)"class", 5};

    எனில் (cl != வெற்று)
    {
	class_name.string = cl->class_name.string;
	class_name.length = cl->class_name.length;
	எனில் (IS_INTERFACE(cl))
	    STR_LITERAL_SET(s, "interface");
	இல்லையெனில் எனில் (IS_ENUM(cl))
	    STR_LITERAL_SET(s, "enum");
    }

    rsize = s.length + 1 + class_name.length + 1;
    r = *tofree = alloc(rsize);
    எனில் (r != வெற்று)
	vim_snprintf((எழுத்து *)r, rsize, "%s %s", s.string, (எழுத்து *)class_name.string);

    திரும்பு r;
}

/*
 * Return a textual representation of an Object in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "copyID" is not zero replace recursive object with "...".
 * When "restore_copyID" is FALSE, repeated items in the object are
 * replaced with "...".  May return NULL.
 */
    நிலையான char_u *
object_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    முழு		copyID,
    முழு		restore_copyID,
    char_u	*numbuf,
    முழு		echo_style,
    முழு		composite_val)
{
    char_u	*r = வெற்று;

    object_T	*obj = tv->vval.v_object;
    எனில் (obj == வெற்று || obj->obj_class == வெற்று)
    {
	*tofree = வெற்று;
	r = (char_u *)"object of [unknown]";
    }
    இல்லையெனில் எனில் (copyID != 0 && obj->obj_copyID == copyID
	    && obj->obj_class->class_obj_member_மொத்தம் != 0)
    {
	size_t n = 25 + obj->obj_class->class_name.length;
	r = alloc(n);
	எனில் (r != வெற்று)
	    (வெறுமை)vim_snprintf((எழுத்து *)r, n, "object of %s {...}",
						obj->obj_class->class_name.string);
	*tofree = r;
    }
    இல்லையெனில்
    {
	முழு old_copyID;
	எனில் (restore_copyID)
	    old_copyID = obj->obj_copyID;

	obj->obj_copyID = copyID;
	*tofree = object2string(obj, numbuf, copyID, echo_style,
				restore_copyID, composite_val);
	எனில் (restore_copyID)
	    obj->obj_copyID = old_copyID;
	r = *tofree;
    }

    திரும்பு r;
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
    முழு		copyID,
    முழு		echo_style,
    முழு		restore_copyID,
    முழு		composite_val)
{
    நிலையான முழு	recurse = 0;
    char_u	*r = வெற்று;

    எனில் (recurse >= DICT_MAXNEST)
    {
	எனில் (!did_echo_string_emsg)
	{
	    // Only give this message once for a recursive call to avoid
	    // flooding the user with errors.  And stop iterating over lists
	    // and dicts and objects.
	    did_echo_string_emsg = TRUE;
	    emsg(_(e_variable_nested_too_deep_for_displaying));
	}
	*tofree = வெற்று;
	திரும்பு (char_u *)"{E724}";
    }
    ++recurse;

    தேர்வு (tv->v_type)
    {
	நிலை VAR_STRING:
	    r = string_tv2string(tv, tofree, echo_style, composite_val);
	    நிறுத்து;

	நிலை VAR_FUNC:
	    r = func_tv2string(tv, tofree, echo_style);
	    நிறுத்து;

	நிலை VAR_PARTIAL:
	    எனில் (tv->vval.v_partial == வெற்று
		    || tv->vval.v_partial->pt_obj == வெற்று)
		r = partial_tv2string(tv, tofree, numbuf, copyID);
	    இல்லையெனில்
		r = method_tv2string(tv, tofree, echo_style);
	    நிறுத்து;

	நிலை VAR_BLOB:
	    r = blob2string(tv->vval.v_blob, tofree, numbuf);
	    நிறுத்து;

	நிலை VAR_LIST:
	    r = list_tv2string(tv, tofree, copyID, restore_copyID);
	    நிறுத்து;

	நிலை VAR_TUPLE:
	    r = tuple_tv2string(tv, tofree, copyID, restore_copyID);
	    நிறுத்து;

	நிலை VAR_DICT:
	    r = dict_tv2string(tv, tofree, copyID, restore_copyID);
	    நிறுத்து;

	நிலை VAR_எண்:
	நிலை VAR_UNKNOWN:
	நிலை VAR_ANY:
	நிலை VAR_VOID:
	    *tofree = வெற்று;
	    r = tv_get_string_buf(tv, numbuf);
	    நிறுத்து;

	நிலை VAR_JOB:
	நிலை VAR_CHANNEL:
	    r = jobchan_tv2string(tv, tofree, numbuf, composite_val);
	    நிறுத்து;

	நிலை VAR_INSTR:
	    *tofree = வெற்று;
	    r = (char_u *)"instructions";
	    நிறுத்து;

	நிலை VAR_CLASS:
	    r = class_tv2string(tv, tofree);
	    நிறுத்து;

	நிலை VAR_OBJECT:
	    r = object_tv2string(tv, tofree, copyID, restore_copyID,
					 numbuf, echo_style, composite_val);
	    நிறுத்து;

	நிலை VAR_FLOAT:
	    *tofree = வெற்று;
	    vim_snprintf((எழுத்து *)numbuf, NUMBUFLEN, "%g", tv->vval.v_float);
	    r = numbuf;
	    நிறுத்து;

	நிலை VAR_BOOL:
	நிலை VAR_SPECIAL:
	    *tofree = வெற்று;
	    r = (char_u *)get_var_special_name(tv->vval.v_எண்);
	    நிறுத்து;

	நிலை VAR_TYPEALIAS:
	    *tofree = vim_strsave(tv->vval.v_typealias->ta_name);
	    r = *tofree;
	    எனில் (r == வெற்று)
		r = (char_u *)"";
	    நிறுத்து;
    }

    எனில் (--recurse == 0)
	did_echo_string_emsg = FALSE;
    திரும்பு r;
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
    முழு		copyID)
{
    திரும்பு echo_string_core(tv, tofree, numbuf, copyID, TRUE, FALSE, FALSE);
}

/*
 * Convert the specified byte index of line 'lnum' in buffer 'buf' to a
 * character index.  Works only for loaded buffers. Returns -1 on failure.
 * The index of the first byte and the first character is zero.
 */
    முழு
buf_byteidx_to_charidx(buf_T *buf, முழு lnum, முழு byteidx)
{
    char_u	*str;
    char_u	*t;
    முழு		மொத்தம்;

    எனில் (buf == வெற்று || buf->b_ml.ml_mfp == வெற்று)
	திரும்பு -1;

    எனில் (lnum > buf->b_ml.ml_வரி_மொத்தம்)
	lnum = buf->b_ml.ml_வரி_மொத்தம்;

    str = ml_get_buf(buf, lnum, FALSE);
    எனில் (str == வெற்று)
	திரும்பு -1;

    எனில் (*str == NUL)
	திரும்பு 0;

    // count the number of characters
    t = str;
    க்கு (மொத்தம் = 0; *t != NUL && t <= str + byteidx; மொத்தம்++)
	t += mb_ptr2len(t);

    // In insert mode, when the cursor is at the end of a non-empty line,
    // byteidx points to the NUL character immediately past the end of the
    // string. In this case, add one to the character count.
    எனில் (*t == NUL && byteidx != 0 && t == str + byteidx)
	மொத்தம்++;

    திரும்பு மொத்தம் - 1;
}

/*
 * Convert the specified character index of line 'lnum' in buffer 'buf' to a
 * byte index.  Works only for loaded buffers. Returns -1 on failure.
 * The index of the first byte and the first character is zero.
 */
    முழு
buf_charidx_to_byteidx(buf_T *buf, முழு lnum, முழு charidx)
{
    char_u	*str;
    char_u	*t;

    எனில் (buf == வெற்று || buf->b_ml.ml_mfp == வெற்று)
	திரும்பு -1;

    எனில் (lnum > buf->b_ml.ml_வரி_மொத்தம்)
	lnum = buf->b_ml.ml_வரி_மொத்தம்;

    str = ml_get_buf(buf, lnum, FALSE);
    எனில் (str == வெற்று)
	திரும்பு -1;

    // Convert the character offset to a byte offset
    t = str;
    போது (*t != NUL && --charidx > 0)
	t += mb_ptr2len(t);

    திரும்பு t - str;
}

/*
 * Translate a String variable into a position.
 * Returns NULL when there is an error.
 */
    pos_T *
var2fpos(
    typval_T	*varp,
    முழு		dollar_lnum,	// TRUE when $ is last line
    முழு		*fnum,		// set to fnum for '0, 'A, etc.
    முழு		charcol)	// return character column
{
    char_u		*name;
    நிலையான pos_T	pos;
    pos_T		*pp;
    முழு			error = FALSE;

    // Argument can be [lnum, col, coladd].
    எனில் (varp->v_type == VAR_LIST)
    {
	list_T		*l;
	முழு		len;
	listitem_T	*li;

	l = varp->vval.v_list;
	எனில் (l == வெற்று)
	    திரும்பு வெற்று;

	// Get the line number
	pos.lnum = list_find_nr(l, 0L, &error);
	எனில் (error || pos.lnum <= 0 || pos.lnum > curbuf->b_ml.ml_வரி_மொத்தம்)
	    திரும்பு வெற்று;	// invalid line number
	எனில் (charcol)
	    len = (long)mb_charlen(ml_get(pos.lnum));
	இல்லையெனில்
	    len = (long)ml_get_len(pos.lnum);

	// Get the column number
	// We accept "$" for the column number: last column.
	li = list_find(l, 1L);
	எனில் (li != வெற்று && li->li_tv.v_type == VAR_STRING
		&& li->li_tv.vval.v_string != வெற்று
		&& STRCMP(li->li_tv.vval.v_string, "$") == 0)
	{
	    pos.col = len + 1;
	}
	இல்லையெனில்
	{
	    pos.col = list_find_nr(l, 1L, &error);
	    எனில் (error)
		திரும்பு வெற்று;
	}

	// Accept a position up to the NUL after the line.
	எனில் (pos.col == 0 || (முழு)pos.col > len + 1)
	    திரும்பு வெற்று;	// invalid column number
	--pos.col;

	// Get the virtual offset.  Defaults to zero.
	pos.coladd = list_find_nr(l, 2L, &error);
	எனில் (error)
	    pos.coladd = 0;

	திரும்பு &pos;
    }

    எனில் (in_vim9script() && check_for_string_arg(varp, 0) == FAIL)
	திரும்பு வெற்று;

    name = tv_get_string_chk(varp);
    எனில் (name == வெற்று)
	திரும்பு வெற்று;

    error = TRUE;
    pos.lnum = 0;
    எனில் (name[0] == '.')
    {
	எனில் (!in_vim9script() || name[1] == NUL)
	{
	    error = FALSE;
	    // cursor
	    pos = curwin->w_cursor;
	}
    }
    இல்லையெனில் எனில் (name[0] == 'v' && name[1] == NUL)
    {
	// Visual start
	எனில் (VIsual_active)
	    pos = VIsual;
	இல்லையெனில்
	    pos = curwin->w_cursor;
    }
    இல்லையெனில் எனில் (name[0] == '\'')
    {
	எனில் (!in_vim9script() || (name[1] != NUL && name[2] == NUL))
	{
	    error = FALSE;
	    // mark
	    pp = getmark_buf_fnum(curbuf, name[1], FALSE, fnum);
	    எனில் (pp == வெற்று || pp == (pos_T *)-1 || pp->lnum <= 0)
		திரும்பு வெற்று;
	    pos = *pp;
	}
    }
    எனில் (pos.lnum != 0)
    {
	எனில் (charcol)
	    pos.col = buf_byteidx_to_charidx(curbuf, pos.lnum, pos.col);
	திரும்பு &pos;
    }

    pos.coladd = 0;

    எனில் (name[0] == 'w' && dollar_lnum)
    {
	// the "w_valid" flags are not reset when moving the cursor, but they
	// do matter for update_topline() and validate_botline().
	check_cursor_moved(curwin);

	pos.col = 0;
	எனில் (name[1] == '0')		// "w0": first visible line
	{
#ifdef FEAT_TERMINAL
	    எனில் (bt_terminal(curwin->w_buffer)
				    && curwin->w_buffer->b_term != வெற்று
				    && !term_in_normal_mode(curwin->w_buffer))
		may_move_terminal_to_buffer(curwin->w_buffer->b_term, TRUE);
#endif
	    update_topline();
	    // In silent Ex mode topline is zero, but that's not a valid line
	    // number; use one instead.
	    pos.lnum = curwin->w_topline > 0 ? curwin->w_topline : 1;
	    திரும்பு &pos;
	}
	இல்லையெனில் எனில் (name[1] == '$')	// "w$": last visible line
	{
#ifdef FEAT_TERMINAL
	    எனில் (bt_terminal(curwin->w_buffer)
				    && curwin->w_buffer->b_term != வெற்று
				    && !term_in_normal_mode(curwin->w_buffer))
		may_move_terminal_to_buffer(curwin->w_buffer->b_term, TRUE);
#endif
	    validate_botline();
	    // In silent Ex mode botline is zero, return zero then.
	    pos.lnum = curwin->w_botline > 0 ? curwin->w_botline - 1 : 0;
	    திரும்பு &pos;
	}
    }
    இல்லையெனில் எனில் (name[0] == '$')		// last column or line
    {
	எனில் (dollar_lnum)
	{
	    pos.lnum = curbuf->b_ml.ml_வரி_மொத்தம்;
	    pos.col = 0;
	}
	இல்லையெனில்
	{
	    pos.lnum = curwin->w_cursor.lnum;
	    எனில் (charcol)
		pos.col = (colnr_T)mb_charlen(ml_get_curline());
	    இல்லையெனில்
		pos.col = ml_get_curline_len();
	}
	திரும்பு &pos;
    }
    எனில் (in_vim9script() && error)
	semsg(_(e_invalid_மதிப்பு_for_வரி_எண்_str), name);
    திரும்பு வெற்று;
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
    முழு
list2fpos(
    typval_T	*arg,
    pos_T	*posp,
    முழு		*fnump,
    colnr_T	*curswantp,
    முழு		charcol)
{
    list_T	*l = arg->vval.v_list;
    long	i = 0;
    long	n;

    // List must be: [fnum, lnum, col, coladd, curswant], where "fnum" is only
    // there when "fnump" isn't NULL; "coladd" and "curswant" are optional.
    எனில் (arg->v_type != VAR_LIST
	    || l == வெற்று
	    || l->lv_len < (fnump == வெற்று ? 2 : 3)
	    || l->lv_len > (fnump == வெற்று ? 4 : 5))
	திரும்பு FAIL;

    எனில் (fnump != வெற்று)
    {
	n = list_find_nr(l, i++, வெற்று);	// fnum
	எனில் (n < 0)
	    திரும்பு FAIL;
	எனில் (n == 0)
	    n = curbuf->b_fnum;		// current buffer
	*fnump = n;
    }

    n = list_find_nr(l, i++, வெற்று);	// lnum
    எனில் (n < 0)
	திரும்பு FAIL;
    posp->lnum = n;

    n = list_find_nr(l, i++, வெற்று);	// col
    எனில் (n < 0)
	திரும்பு FAIL;
    // If character position is specified, then convert to byte position
    // If the line number is zero use the cursor line.
    எனில் (charcol)
    {
	buf_T	*buf;

	// Get the text for the specified line in a loaded buffer
	buf = buflist_findnr(fnump == வெற்று ? curbuf->b_fnum : *fnump);
	எனில் (buf == வெற்று || buf->b_ml.ml_mfp == வெற்று)
	    திரும்பு FAIL;

	n = buf_charidx_to_byteidx(buf,
		  posp->lnum == 0 ? curwin->w_cursor.lnum : posp->lnum, n) + 1;
    }
    posp->col = n;

    n = list_find_nr(l, i, வெற்று);	// off
    எனில் (n < 0)
	posp->coladd = 0;
    இல்லையெனில்
	posp->coladd = n;

    எனில் (curswantp != வெற்று)
	*curswantp = list_find_nr(l, i + 1, வெற்று);  // curswant

    திரும்பு OK;
}

/*
 * Get the length of an environment variable name.
 * Advance "arg" to the first character after the name.
 * Return 0 for error.
 */
    முழு
get_env_len(char_u **arg)
{
    char_u	*p;
    முழு		len;

    க்கு (p = *arg; vim_isIDc(*p); ++p)
	;
    எனில் (p == *arg)	    // no name found
	திரும்பு 0;

    len = (முழு)(p - *arg);
    *arg = p;
    திரும்பு len;
}

/*
 * Get the length of the name of a function or internal variable.
 * "arg" is advanced to after the name.
 * Return 0 if something is wrong.
 */
    முழு
get_id_len(char_u **arg)
{
    char_u	*p;
    முழு		len;

    // Find the end of the name.
    க்கு (p = *arg; eval_isnamec(*p); ++p)
    {
	எனில் (*p == ':')
	{
	    // "s:" is start of "s:var", but "n:" is not and can be used in
	    // slice "[n:]".  Also "xx:" is not a namespace.
	    len = (முழு)(p - *arg);
	    எனில் ((len == 1 && vim_strchr(NAMESPACE_CHAR, **arg) == வெற்று)
		    || len > 1)
		நிறுத்து;
	}
    }
    எனில் (p == *arg)	    // no name found
	திரும்பு 0;

    len = (முழு)(p - *arg);
    *arg = p;

    திரும்பு len;
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
    முழு
get_name_len(
    char_u	**arg,
    char_u	**alias,
    முழு		evaluate,
    முழு		verbose)
{
    முழு		len;
    char_u	*p;
    char_u	*கோவை_start;
    char_u	*கோவை_end;

    *alias = வெற்று;  // default to no alias

    எனில் ((*arg)[0] == K_SPECIAL && (*arg)[1] == KS_EXTRA
						  && (*arg)[2] == (முழு)KE_SNR)
    {
	// hard coded <SNR>, already translated
	*arg += 3;
	திரும்பு get_id_len(arg) + 3;
    }
    len = eval_fname_script(*arg);
    எனில் (len > 0)
    {
	// literal "<SID>", "s:" or "<SNR>"
	*arg += len;
    }

    /*
     * Find the end of the name; check for {} construction.
     */
    p = find_name_end(*arg, &கோவை_start, &கோவை_end,
					       len > 0 ? 0 : FNE_CHECK_START);
    எனில் (கோவை_start != வெற்று)
    {
	char_u	*temp_string;

	எனில் (!evaluate)
	{
	    len += (முழு)(p - *arg);
	    *arg = skipwhite(p);
	    திரும்பு len;
	}

	/*
	 * Include any <SID> etc in the expanded string:
	 * Thus the -len here.
	 */
	temp_string = make_expanded_name(*arg - len, கோவை_start, கோவை_end, p);
	எனில் (temp_string == வெற்று)
	    திரும்பு -1;
	*alias = temp_string;
	*arg = skipwhite(p);
	திரும்பு (முழு)STRLEN(temp_string);
    }

    len += get_id_len(arg);
    // Only give an error when there is something, otherwise it will be
    // reported at a higher level.
    எனில் (len == 0 && verbose && **arg != NUL)
	semsg(_(e_invalid_expression_str), *arg);

    திரும்பு len;
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
    char_u	**கோவை_start,
    char_u	**கோவை_end,
    முழு		flags)
{
    முழு		mb_nest = 0;
    முழு		br_nest = 0;
    char_u	*p;
    முழு		len;
    முழு		allow_curly = !in_vim9script();

    எனில் (கோவை_start != வெற்று)
    {
	*கோவை_start = வெற்று;
	*கோவை_end = வெற்று;
    }

    // Quick check for valid starting character.
    எனில் ((flags & FNE_CHECK_START) && !eval_isnamec1(*arg)
					      && (*arg != '{' || !allow_curly))
	திரும்பு arg;

    க்கு (p = arg; *p != NUL
		    && (eval_isnamec(*p)
			|| (*p == '{' && allow_curly)
			|| ((flags & FNE_INCL_BR) && (*p == '['
					 || (*p == '.' && eval_isdictc(p[1]))))
			|| mb_nest != 0
			|| br_nest != 0); MB_PTR_ADV(p))
    {
	எனில் (*p == '\'')
	{
	    // skip over 'string' to avoid counting [ and ] inside it.
	    க்கு (p = p + 1; *p != NUL && *p != '\''; MB_PTR_ADV(p))
		;
	    எனில் (*p == NUL)
		நிறுத்து;
	}
	இல்லையெனில் எனில் (*p == '"')
	{
	    // skip over "str\"ing" to avoid counting [ and ] inside it.
	    க்கு (p = p + 1; *p != NUL && *p != '"'; MB_PTR_ADV(p))
		எனில் (*p == '\\' && p[1] != NUL)
		    ++p;
	    எனில் (*p == NUL)
		நிறுத்து;
	}
	இல்லையெனில் எனில் (br_nest == 0 && mb_nest == 0 && *p == ':')
	{
	    // "s:" is start of "s:var", but "n:" is not and can be used in
	    // slice "[n:]".  Also "xx:" is not a namespace. But {ns}: is.
	    len = (முழு)(p - arg);
	    எனில் ((len == 1 && vim_strchr(NAMESPACE_CHAR, *arg) == வெற்று)
		    || (len > 1 && p[-1] != '}'))
		நிறுத்து;
	}

	எனில் (mb_nest == 0)
	{
	    எனில் (*p == '[')
		++br_nest;
	    இல்லையெனில் எனில் (*p == ']')
		--br_nest;
	}

	எனில் (br_nest == 0 && allow_curly)
	{
	    எனில் (*p == '{')
	    {
		mb_nest++;
		எனில் (கோவை_start != வெற்று && *கோவை_start == வெற்று)
		    *கோவை_start = p;
	    }
	    இல்லையெனில் எனில் (*p == '}')
	    {
		mb_nest--;
		எனில் (கோவை_start != வெற்று && mb_nest == 0 && *கோவை_end == வெற்று)
		    *கோவை_end = p;
	    }
	}
    }

    திரும்பு p;
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
    நிலையான char_u *
make_expanded_name(
    char_u	*in_start,
    char_u	*கோவை_start,
    char_u	*கோவை_end,
    char_u	*in_end)
{
    char_u	c1;
    char_u	*retval = வெற்று;
    char_u	*temp_result;

    எனில் (கோவை_end == வெற்று || in_end == வெற்று)
	திரும்பு வெற்று;
    *கோவை_start	= NUL;
    *கோவை_end = NUL;
    c1 = *in_end;
    *in_end = NUL;

    temp_result = eval_to_string(கோவை_start + 1, FALSE, FALSE);
    எனில் (temp_result != வெற்று)
    {
	size_t	retvalsize = (size_t)(கோவை_start - in_start)
				+ STRLEN(temp_result)
				+ (size_t)(in_end - கோவை_end) + 1;

	retval = alloc(retvalsize);
	எனில் (retval != வெற்று)
	    vim_snprintf((எழுத்து *)retval, retvalsize, "%s%s%s",
				in_start, temp_result, கோவை_end + 1);
    }
    vim_free(temp_result);

    *in_end = c1;		// put char back for error messages
    *கோவை_start = '{';
    *கோவை_end = '}';

    எனில் (retval != வெற்று)
    {
	temp_result = find_name_end(retval, &கோவை_start, &கோவை_end, 0);
	எனில் (கோவை_start != வெற்று)
	{
	    // Further expansion!
	    temp_result = make_expanded_name(retval, கோவை_start,
						       கோவை_end, temp_result);
	    vim_free(retval);
	    retval = temp_result;
	}
    }

    திரும்பு retval;
}

/*
 * Return TRUE if character "c" can be used in a variable or function name.
 * Does not include '{' or '}' for magic braces.
 */
    முழு
eval_isnamec(முழு c)
{
    திரும்பு ASCII_ISALNUM(c) || c == '_' || c == ':' || c == AUTOLOAD_CHAR;
}

/*
 * Return TRUE if character "c" can be used as the first character in a
 * variable or function name (excluding '{' and '}').
 */
    முழு
eval_isnamec1(முழு c)
{
    திரும்பு ASCII_ISALPHA(c) || c == '_';
}

/*
 * Return TRUE if character "c" can be used as the first character of a
 * dictionary key.
 */
    முழு
eval_isdictc(முழு c)
{
    திரும்பு ASCII_ISALNUM(c) || c == '_';
}

/*
 * Return true when "p" points to "->" followed by a method name or a lambda.
 * Only called for Vim9 script.
 */
    நிலையான bool
method_call_follows(char_u *p)
{
    எனில் (p[0] != '-' || p[1] != '>')
	திரும்பு false;

    char_u	*after = skipwhite(p + 2);

    திரும்பு ASCII_ISALPHA(*after) || *after == '(';
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
    முழு
handle_subscript(
    char_u	**arg,
    char_u	*name_start,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    முழு		verbose)	// give error messages
{
    முழு		evaluate = evalarg != வெற்று
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    முழு		ret = OK;
    dict_T	*selfdict = வெற்று;
    முழு		check_white = TRUE;
    முழு		getnext;
    char_u	*p;

    போது (ret == OK)
    {
	// When at the end of the line and ".name" or a method call follows in
	// the next line then consume the line break.
	p = eval_next_non_blank(*arg, evalarg, &getnext);
	எனில் (getnext
	    && ((*p == '.'
		    && ((rettv->v_type == VAR_DICT && eval_isdictc(p[1]))
			|| rettv->v_type == VAR_CLASS
			|| rettv->v_type == VAR_OBJECT))
		|| method_call_follows(p)))
	{
	    *arg = eval_next_வரி(*arg, evalarg);
	    p = *arg;
	    check_white = FALSE;
	}

	எனில் (rettv->v_type == VAR_ANY)
	{
	    char_u	*exp_name;
	    முழு		cc;
	    முழு		idx;
	    ufunc_T	*ufunc;
	    type_T	*type;

	    // Found script from "import {name} as name", script item name must
	    // follow.  "rettv->vval.v_number" has the script ID.
	    எனில் (**arg != '.')
	    {
		எனில் (verbose)
		    semsg(_(e_expected_dot_after_name_str),
					name_start != வெற்று ? name_start: *arg);
		ret = FAIL;
		நிறுத்து;
	    }
	    ++*arg;
	    எனில் (IS_WHITE_OR_NUL(**arg))
	    {
		எனில் (verbose)
		    emsg(_(e_no_white_space_allowed_after_dot));
		ret = FAIL;
		நிறுத்து;
	    }

	    // isolate the name
	    exp_name = *arg;
	    போது (eval_isnamec(**arg))
		++*arg;
	    cc = **arg;
	    **arg = NUL;

	    idx = find_exported(rettv->vval.v_எண், exp_name, &ufunc, &type,
		       evalarg == வெற்று ? வெற்று : evalarg->eval_cctx,
		       evalarg == வெற்று ? வெற்று : evalarg->eval_cstack, verbose);
	    **arg = cc;

	    எனில் (idx < 0 && ufunc == வெற்று)
	    {
		ret = FAIL;
		நிறுத்து;
	    }
	    எனில் (idx >= 0)
	    {
		scriptitem_T    *si = SCRIPT_ITEM(rettv->vval.v_எண்);
		svar_T		*sv = ((svar_T *)si->sn_var_vals.ga_data) + idx;

		copy_tv(sv->sv_tv, rettv);
	    }
	    இல்லையெனில்
	    {
		rettv->v_type = VAR_FUNC;
		எனில் (**arg == '<')
		{
		    char_u *s = get_generic_func_name(ufunc, arg);
		    எனில் (s != வெற்று)
			rettv->vval.v_string = s;
		    இல்லையெனில்
			ret = FAIL;
		}
		இல்லையெனில்
		{
		    rettv->vval.v_string =
			vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
		}
	    }
	    தொடர்;
	}

	எனில் ((**arg == '(' && (!evaluate || rettv->v_type == VAR_FUNC
			    || rettv->v_type == VAR_PARTIAL))
		    && (!check_white || !VIM_ISWHITE(*(*arg - 1))))
	{
	    ret = call_func_rettv(arg, evalarg, rettv, evaluate,
							       selfdict, வெற்று);

	    // Stop the expression evaluation when immediately aborting on
	    // error, or when an interrupt occurred or an exception was thrown
	    // but not caught.
	    எனில் (aborting())
	    {
		எனில் (ret == OK)
		    clear_tv(rettv);
		ret = FAIL;
	    }
	    dict_unref(selfdict);
	    selfdict = வெற்று;
	}
	இல்லையெனில் எனில் (p[0] == '-' && p[1] == '>')
	{
	    எனில் (in_vim9script())
		*arg = skipwhite(p + 2);
	    இல்லையெனில்
		*arg = p + 2;
	    எனில் (ret == OK && evaluate && rettv->v_type == VAR_VOID)
	    {
		எனில் (verbose)
		    emsg(_(e_cannot_use_void_மதிப்பு));
		ret = FAIL;
	    }
	    இல்லையெனில் எனில் (VIM_ISWHITE(**arg))
	    {
		emsg(_(e_no_white_space_allowed_before_parenthesis));
		ret = FAIL;
	    }
	    இல்லையெனில் எனில் ((**arg == '{' && !in_vim9script()) || **arg == '(')
		// expr->{lambda}() or expr->(lambda)()
		ret = eval_lambda(arg, rettv, evalarg, verbose);
	    இல்லையெனில்
		// expr->name()
		ret = eval_method(arg, rettv, evalarg, verbose);
	}
	// "." is ".name" lookup when we found a dict or when evaluating and
	// scriptversion is at least 2, where string concatenation is "..".
	இல்லையெனில் எனில் (**arg == '['
		|| (**arg == '.' && (rettv->v_type == VAR_DICT
			|| (!evaluate
			    && (*arg)[1] != '.'
			    && !in_old_script(2)))))
	{
	    dict_unref(selfdict);
	    எனில் (rettv->v_type == VAR_DICT)
	    {
		selfdict = rettv->vval.v_dict;
		எனில் (selfdict != வெற்று)
		    ++selfdict->dv_refcount;
	    }
	    இல்லையெனில்
		selfdict = வெற்று;
	    எனில் (eval_index(arg, rettv, evalarg, verbose) == FAIL)
	    {
		clear_tv(rettv);
		ret = FAIL;
	    }
	}
	இல்லையெனில் எனில் (**arg == '.' && (rettv->v_type == VAR_CLASS
					       || rettv->v_type == VAR_OBJECT))
	{
	    // class member: SomeClass.varname
	    // class method: SomeClass.SomeMethod()
	    // class constructor: SomeClass.new()
	    // object member: someObject.varname
	    // object method: someObject.SomeMethod()
	    எனில் (class_object_index(arg, rettv, evalarg, verbose) == FAIL)
	    {
		clear_tv(rettv);
		ret = FAIL;
	    }
	}
	இல்லையெனில்
	    நிறுத்து;
    }

    // Turn "dict.Func" into a partial for "Func" bound to "dict".
    // Don't do this when "Func" is already a partial that was bound
    // explicitly (pt_auto is FALSE).
    எனில் (selfdict != வெற்று
	    && (rettv->v_type == VAR_FUNC
		|| (rettv->v_type == VAR_PARTIAL
		    && (rettv->vval.v_partial->pt_auto
			|| rettv->vval.v_partial->pt_dict == வெற்று))))
	selfdict = make_partial(selfdict, rettv);

    dict_unref(selfdict);
    திரும்பு ret;
}

/*
 * Make a copy of an item.
 * Lists and Dictionaries are also copied.  A deep copy if "deep" is set.
 * "top" is TRUE for the toplevel of copy().
 * For deepcopy() "copyID" is zero for a full copy or the ID for when a
 * reference to an already copied list/dict can be used.
 * Returns FAIL or OK.
 */
    முழு
item_copy(
    typval_T	*from,
    typval_T	*to,
    முழு		deep,
    முழு		உச்சி,
    முழு		copyID)
{
    நிலையான முழு	recurse = 0;
    முழு		ret = OK;

    எனில் (recurse >= DICT_MAXNEST)
    {
	emsg(_(e_variable_nested_too_deep_for_making_copy));
	திரும்பு FAIL;
    }
    ++recurse;

    தேர்வு (from->v_type)
    {
	நிலை VAR_எண்:
	நிலை VAR_FLOAT:
	நிலை VAR_STRING:
	நிலை VAR_FUNC:
	நிலை VAR_PARTIAL:
	நிலை VAR_BOOL:
	நிலை VAR_SPECIAL:
	நிலை VAR_JOB:
	நிலை VAR_CHANNEL:
	நிலை VAR_INSTR:
	நிலை VAR_CLASS:
	நிலை VAR_OBJECT:
	நிலை VAR_TYPEALIAS:
	    copy_tv(from, to);
	    நிறுத்து;
	நிலை VAR_LIST:
	    to->v_type = VAR_LIST;
	    to->v_lock = 0;
	    எனில் (from->vval.v_list == வெற்று)
		to->vval.v_list = வெற்று;
	    இல்லையெனில் எனில் (copyID != 0 && from->vval.v_list->lv_copyID == copyID)
	    {
		// use the copy made earlier
		to->vval.v_list = from->vval.v_list->lv_copylist;
		++to->vval.v_list->lv_refcount;
	    }
	    இல்லையெனில்
	    {
		to->vval.v_list = list_copy(from->vval.v_list,
							    deep, உச்சி, copyID);
		எனில் (to->vval.v_list == வெற்று)
		    ret = FAIL;
	    }
	    நிறுத்து;
	நிலை VAR_TUPLE:
	    to->v_type = VAR_TUPLE;
	    to->v_lock = 0;
	    எனில் (from->vval.v_tuple == வெற்று)
		to->vval.v_tuple = வெற்று;
	    இல்லையெனில் எனில் (copyID != 0 && from->vval.v_tuple->tv_copyID == copyID)
	    {
		// use the copy made earlier
		to->vval.v_tuple = from->vval.v_tuple->tv_copytuple;
		++to->vval.v_tuple->tv_refcount;
	    }
	    இல்லையெனில்
	    {
		to->vval.v_tuple = tuple_copy(from->vval.v_tuple,
							    deep, உச்சி, copyID);
		எனில் (to->vval.v_tuple == வெற்று)
		    ret = FAIL;
	    }
	    நிறுத்து;
	நிலை VAR_BLOB:
	    to->v_type = VAR_BLOB;
	    to->v_lock = 0;
	    எனில் (from->vval.v_blob == வெற்று)
		to->vval.v_blob = வெற்று;
	    இல்லையெனில்
	    {
		to->vval.v_blob = blob_copy(from->vval.v_blob);
		எனில் (to->vval.v_blob == வெற்று)
		    ret = FAIL;
	    }
	    நிறுத்து;
	நிலை VAR_DICT:
	    to->v_type = VAR_DICT;
	    to->v_lock = 0;
	    எனில் (from->vval.v_dict == வெற்று)
		to->vval.v_dict = வெற்று;
	    இல்லையெனில் எனில் (copyID != 0 && from->vval.v_dict->dv_copyID == copyID)
	    {
		// use the copy made earlier
		to->vval.v_dict = from->vval.v_dict->dv_copydict;
		++to->vval.v_dict->dv_refcount;
	    }
	    இல்லையெனில்
	    {
		to->vval.v_dict = dict_copy(from->vval.v_dict,
							    deep, உச்சி, copyID);
		எனில் (to->vval.v_dict == வெற்று)
		    ret = FAIL;
	    }
	    நிறுத்து;
	நிலை VAR_UNKNOWN:
	நிலை VAR_ANY:
	நிலை VAR_VOID:
	    internal_error_no_abort("item_copy(UNKNOWN)");
	    ret = FAIL;
    }
    --recurse;
    திரும்பு ret;
}

    வெறுமை
echo_one(typval_T *rettv, முழு with_space, முழு *atstart, முழு *needclr)
{
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    char_u	*p = echo_string(rettv, &tofree, numbuf, get_copyID());

    எனில் (*atstart)
    {
	*atstart = FALSE;
	// Call msg_start() after eval1(), evaluating the expression
	// may cause a message to appear.
	எனில் (with_space)
	{
	    // Mark the saved text as finishing the line, so that what
	    // follows is displayed on a new line when scrolling back
	    // at the more prompt.
	    செய்தி_sb_eol();
	    செய்தி_start();
	}
    }
    இல்லையெனில் எனில் (with_space)
	செய்தி_puts_attr(" ", echo_attr);

    எனில் (p != வெற்று)
	க்கு ( ; *p != NUL && !got_int; ++p)
	{
	    எனில் (*p == '\n' || *p == '\r' || *p == TAB)
	    {
		எனில் (*p != TAB && *needclr)
		{
		    // remove any text still there from the command
		    செய்தி_clr_eos();
		    *needclr = FALSE;
		}
		செய்தி_putchar_attr(*p, echo_attr);
	    }
	    இல்லையெனில்
	    {
		எனில் (has_mbyte)
		{
		    முழு i = (*mb_ptr2len)(p);

		    (வெறுமை)செய்தி_outtrans_len_attr(p, i, echo_attr);
		    p += i - 1;
		}
		இல்லையெனில்
		    (வெறுமை)செய்தி_outtrans_len_attr(p, 1, echo_attr);
	    }
	}
    vim_free(tofree);
}

/*
 * ":echo expr1 ..."	print each argument separated with a space, add a
 *			newline at the end.
 * ":echon expr1 ..."	print each argument plain.
 */
    வெறுமை
ex_echo(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    typval_T	rettv;
    char_u	*arg_start;
    முழு		needclr = TRUE;
    முழு		atstart = TRUE;
    முழு		did_emsg_before = did_emsg;
    முழு		called_emsg_before = called_emsg;
    evalarg_T	evalarg;

    fill_evalarg_from_eap(&evalarg, eap, eap->skip);

    எனில் (eap->skip)
	++emsg_skip;
    போது ((!ends_excmd2(eap->cmd, arg) || *arg == '"') && !got_int)
    {
	// If eval1() causes an error message the text from the command may
	// still need to be cleared. E.g., "echo 22,44".
	need_clr_eos = needclr;

	arg_start = arg;
	எனில் (eval1(&arg, &rettv, &evalarg) == FAIL)
	{
	    /*
	     * Report the invalid expression unless the expression evaluation
	     * has been cancelled due to an aborting error, an interrupt, or an
	     * exception.
	     */
	    எனில் (!aborting() && did_emsg == did_emsg_before
					  && called_emsg == called_emsg_before)
		semsg(_(e_invalid_expression_str), arg_start);
	    need_clr_eos = FALSE;
	    நிறுத்து;
	}
	need_clr_eos = FALSE;

	எனில் (!eap->skip)
	{
	    எனில் (rettv.v_type == VAR_VOID)
	    {
		semsg(_(e_expression_does_not_result_in_மதிப்பு_str), arg_start);
		நிறுத்து;
	    }
	    echo_one(&rettv, eap->cmdidx == CMD_echo, &atstart, &needclr);
	}

	clear_tv(&rettv);
	arg = skipwhite(arg);
    }
    set_nextcmd(eap, arg);
    clear_evalarg(&evalarg, eap);

    எனில் (eap->skip)
	--emsg_skip;
    இல்லையெனில்
    {
	// remove text that may still be there from the command
	எனில் (needclr)
	    செய்தி_clr_eos();
	எனில் (eap->cmdidx == CMD_echo)
	    செய்தி_end();
    }
}

/*
 * ":echohl {name}".
 */
    வெறுமை
ex_echohl(exarg_T *eap)
{
    echo_attr = syn_name2attr(eap->arg);
}

/*
 * Returns the :echo attribute
 */
    முழு
get_echo_attr(வெறுமை)
{
    திரும்பு echo_attr;
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
    வெறுமை
ex_execute(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    typval_T	rettv;
    முழு		ret = OK;
    char_u	*p;
    garray_T	ga;
    முழு		len;
    long	start_lnum = SOURCING_LNUM;

    ga_init2(&ga, 1, 80);

    எனில் (eap->skip)
	++emsg_skip;
    போது (!ends_excmd2(eap->cmd, arg) || *arg == '"')
    {
	ret = eval1_emsg(&arg, &rettv, eap);
	எனில் (ret == FAIL)
	    நிறுத்து;

	எனில் (!eap->skip)
	{
	    char_u   buf[NUMBUFLEN];

	    எனில் (eap->cmdidx == CMD_execute)
	    {
		எனில் (rettv.v_type == VAR_CHANNEL || rettv.v_type == VAR_JOB)
		{
		    semsg(_(e_using_invalid_மதிப்பு_as_string_str),
						  vartype_name(rettv.v_type));
		    p = வெற்று;
		}
		இல்லையெனில்
		    p = tv_get_string_buf(&rettv, buf);
	    }
	    இல்லையெனில்
		p = tv_stringify(&rettv, buf);
	    எனில் (p == வெற்று)
	    {
		clear_tv(&rettv);
		ret = FAIL;
		நிறுத்து;
	    }
	    len = (முழு)STRLEN(p);
	    எனில் (ga_grow(&ga, len + 2) == FAIL)
	    {
		clear_tv(&rettv);
		ret = FAIL;
		நிறுத்து;
	    }
	    எனில் (ga.ga_len)
		((char_u *)(ga.ga_data))[ga.ga_len++] = ' ';
	    STRCPY((char_u *)(ga.ga_data) + ga.ga_len, p);
	    ga.ga_len += len;
	}

	clear_tv(&rettv);
	arg = skipwhite(arg);
    }

    எனில் (ret != FAIL && ga.ga_data != வெற்று)
    {
	// use the first line of continuation lines for messages
	SOURCING_LNUM = start_lnum;

	எனில் (eap->cmdidx == CMD_echomsg
		|| eap->cmdidx == CMD_echowindow
		|| eap->cmdidx == CMD_echoerr)
	{
	    // Mark the already saved text as finishing the line, so that what
	    // follows is displayed on a new line when scrolling back at the
	    // more prompt.
	    செய்தி_sb_eol();
	}

	எனில் (eap->cmdidx == CMD_echomsg)
	{
	    செய்தி_attr(ga.ga_data, echo_attr);
	    out_flush();
	}
	இல்லையெனில் எனில் (eap->cmdidx == CMD_echowindow)
	{
#ifdef HAS_MESSAGE_WINDOW
	    start_echowindow(eap->addr_மொத்தம் > 0 ? eap->line2 : 0);
#endif
	    செய்தி_attr(ga.ga_data, echo_attr);
#ifdef HAS_MESSAGE_WINDOW
	    end_echowindow();
#endif
	}
	இல்லையெனில் எனில் (eap->cmdidx == CMD_echoconsole)
	{
	    ui_write(ga.ga_data, (முழு)STRLEN(ga.ga_data), TRUE);
	    ui_write((char_u *)"\r\n", 2, TRUE);
	}
	இல்லையெனில் எனில் (eap->cmdidx == CMD_echoerr)
	{
	    முழு		save_did_emsg = did_emsg;

	    // We don't want to abort following commands, restore did_emsg.
	    emsg(ga.ga_data);
	    எனில் (!force_abort)
		did_emsg = save_did_emsg;
	}
	இல்லையெனில் எனில் (eap->cmdidx == CMD_execute)
	{
	    முழு save_sticky_cmdmod_flags = sticky_cmdmod_flags;

	    // "legacy exe cmd" and "vim9cmd exe cmd" applies to "cmd".
	    sticky_cmdmod_flags = cmdmod.cmod_flags
						& (CMOD_LEGACY | CMOD_VIM9CMD);
	    do_cmdline((char_u *)ga.ga_data,
		       eap->ea_getline, eap->cookie, DOCMD_NOWAIT|DOCMD_VERBOSE);
	    sticky_cmdmod_flags = save_sticky_cmdmod_flags;
	}
    }

    ga_clear(&ga);

    எனில் (eap->skip)
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
find_option_end(char_u **arg, முழு *scope)
{
    char_u	*p = *arg;

    ++p;
    எனில் (*p == 'g' && p[1] == ':')
    {
	*scope = OPT_GLOBAL;
	p += 2;
    }
    இல்லையெனில் எனில் (*p == 'l' && p[1] == ':')
    {
	*scope = OPT_LOCAL;
	p += 2;
    }
    இல்லையெனில்
	*scope = 0;

    எனில் (!ASCII_ISALPHA(*p))
	திரும்பு வெற்று;
    *arg = p;

    எனில் (p[0] == 't' && p[1] == '_' && p[2] != NUL && p[3] != NUL)
	p += 4;	    // termcap option
    இல்லையெனில்
	போது (ASCII_ISALPHA(*p))
	    ++p;
    திரும்பு p;
}

/*
 * Display script name where an item was last set.
 * Should only be invoked when 'verbose' is non-zero.
 */
    வெறுமை
last_set_செய்தி(sctx_T script_ctx)
{
    char_u *p;

    எனில் (script_ctx.sc_sid == 0)
	திரும்பு;

    p = home_replace_save(வெற்று, get_scriptname(script_ctx.sc_sid));
    எனில் (p == வெற்று)
	திரும்பு;

    verbose_enter();
    செய்தி_puts(_("\n\tLast set from "));
    செய்தி_puts((எழுத்து *)p);
    எனில் (script_ctx.sc_lnum > 0)
    {
	செய்தி_puts(_(வரி_செய்தி));
	செய்தி_outnum((long)script_ctx.sc_lnum);
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
    typval_T	*கோவை,
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
    எனில் (regmatch.regprog != வெற்று)
    {
	char_u	*tail = str;
	char_u	*end = str + len;
	முழு	do_all = (flags[0] == 'g');
	முழு	sublen;
	முழு	i;
	char_u	*zero_width = வெற்று;

	போது (vim_regexec_nl(&regmatch, str, (colnr_T)(tail - str)))
	{
	    // Skip empty match except for first match.
	    எனில் (regmatch.startp[0] == regmatch.endp[0])
	    {
		எனில் (zero_width == regmatch.startp[0])
		{
		    // avoid getting stuck on a match with an empty string
		    i = mb_ptr2len(tail);
		    mch_memmove((char_u *)ga.ga_data + ga.ga_len, tail,
								   (size_t)i);
		    ga.ga_len += i;
		    tail += i;
		    தொடர்;
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
	    sublen = vim_regsub(&regmatch, sub, கோவை, tail, 0, REGSUB_MAGIC);
	    எனில் (sublen <= 0)
	    {
		ga_clear(&ga);
		நிறுத்து;
	    }
	    எனில் (ga_grow(&ga, (முழு)((end - tail) + sublen -
			    (regmatch.endp[0] - regmatch.startp[0]))) == FAIL)
	    {
		ga_clear(&ga);
		நிறுத்து;
	    }

	    // copy the text up to where the match is
	    i = (முழு)(regmatch.startp[0] - tail);
	    mch_memmove((char_u *)ga.ga_data + ga.ga_len, tail, (size_t)i);
	    // add the substituted text
	    (வெறுமை)vim_regsub(&regmatch, sub, கோவை,
				  (char_u *)ga.ga_data + ga.ga_len + i, sublen,
				  REGSUB_COPY | REGSUB_MAGIC);
	    ga.ga_len += i + sublen - 1;
	    tail = regmatch.endp[0];
	    எனில் (*tail == NUL)
		நிறுத்து;
	    எனில் (!do_all)
		நிறுத்து;
	}

	எனில் (ga.ga_data != வெற்று)
	{
	    STRCPY((எழுத்து *)ga.ga_data + ga.ga_len, tail);
	    ga.ga_len += (முழு)(end - tail);
	}

	vim_regfree(regmatch.regprog);
    }

    எனில் (ga.ga_data != வெற்று)
    {
	str = (char_u *)ga.ga_data;
	len = (size_t)ga.ga_len;
    }
    ret = vim_strnsave(str, len);
    ga_clear(&ga);
    எனில் (p_cpo == empty_option)
	p_cpo = save_cpo;
    இல்லையெனில்
    {
	// Darn, evaluating {sub} expression or {expr} changed the value.
	// If it's still empty it was changed and restored, need to restore in
	// the complicated way.
	எனில் (*p_cpo == NUL)
	    set_option_மதிப்பு_give_err((char_u *)"cpo", 0L, save_cpo, 0);
	free_string_option(save_cpo);
    }

    எனில் (ret_len != வெற்று)
	*ret_len = len;

    திரும்பு ret;
}
