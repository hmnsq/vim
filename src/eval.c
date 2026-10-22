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

#অন্তর্ভাব "vim.h"

#যদি defined(FEAT_EVAL)

#ifdef VMS
# include <float.h>
#endif

#define NAMESPACE_CHAR	(char_u *)"abglstvw"

স্থির পূর্ণাঙ্ক eval0_simple_funccal(char_u *arg, typval_T *rettv, exarg_T *eap, evalarg_T *evalarg);
স্থির পূর্ণাঙ্ক eval2(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
স্থির পূর্ণাঙ্ক eval3(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
স্থির পূর্ণাঙ্ক eval4(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
স্থির পূর্ণাঙ্ক eval5(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
স্থির পূর্ণাঙ্ক eval6(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
স্থির পূর্ণাঙ্ক eval7(char_u **arg, typval_T *rettv, evalarg_T *evalarg, পূর্ণাঙ্ক want_string);
স্থির পূর্ণাঙ্ক eval8(char_u **arg, typval_T *rettv, evalarg_T *evalarg, পূর্ণাঙ্ক want_string);
স্থির পূর্ণাঙ্ক eval9(char_u **arg, typval_T *rettv, evalarg_T *evalarg, পূর্ণাঙ্ক want_string);
স্থির পূর্ণাঙ্ক eval9_leader(typval_T *rettv, পূর্ণাঙ্ক numeric_only, char_u *start_leader, char_u **end_leaderp);

স্থির char_u *make_expanded_name(char_u *in_start, char_u *রাশিমালা_start, char_u *রাশিমালা_end, char_u *in_end);

/*
 * Return "n1" divided by "n2", taking care of dividing by zero.
 * If "failed" is not NULL set it to TRUE when dividing by zero fails.
 */
	varnumber_T
অঙ্ক_divide(varnumber_T n1, varnumber_T n2, পূর্ণাঙ্ক *failed)
{
    varnumber_T	result;

    যদি (n2 == 0)
    {
	যদি (in_vim9script())
	{
	    emsg(_(e_divide_by_zero));
	    যদি (failed != রিক্ত)
		*failed = TRUE;
	}
	যদি (n1 == 0)
	    result = VARNUM_MIN; // similar to NaN
	অন্যথা যদি (n1 < 0)
	    result = -VARNUM_MAX;
	অন্যথা
	    result = VARNUM_MAX;
    }
    অন্যথা যদি (n1 == VARNUM_MIN && n2 == -1)
    {
	// specific case: trying to do VARNUM_MIN / -1 results in a positive
	// number that doesn't fit in varnumber_T and causes an FPE
	result = VARNUM_MAX;
    }
    অন্যথা
	result = n1 / n2;

    প্রত্যাগম result;
}

/*
 * Return "n1" modulus "n2", taking care of dividing by zero.
 * If "failed" is not NULL set it to TRUE when dividing by zero fails.
 */
	varnumber_T
অঙ্ক_modulus(varnumber_T n1, varnumber_T n2, পূর্ণাঙ্ক *failed)
{
    যদি (n2 == 0 && in_vim9script())
    {
	emsg(_(e_divide_by_zero));
	যদি (failed != রিক্ত)
	    *failed = TRUE;
    }
    প্রত্যাগম (n2 == 0) ? 0 : (n1 % n2);
}

/*
 * Initialize the global and v: variables.
 */
    শূন্য
eval_init(শূন্য)
{
    evalvars_init();
    func_init();
}

#যদি defined(EXITFREE)
    শূন্য
eval_clear(শূন্য)
{
    evalvars_clear();
    free_scriptnames();  // must come after evalvars_clear().
    free_locales();

    // autoloaded script names
    free_autoload_scriptnames();

    // unreferenced lists, tuples and dicts
    (শূন্য)garbage_collect(FALSE);

    // functions not garbage collected
    free_all_functions();
}
#endif

    শূন্য
fill_evalarg_from_eap(evalarg_T *evalarg, exarg_T *eap, পূর্ণাঙ্ক skip)
{
    init_evalarg(evalarg);
    evalarg->eval_flags = skip ? 0 : EVAL_EVALUATE;

    যদি (eap == রিক্ত)
	প্রত্যাগম;

    evalarg->eval_cstack = eap->cstack;
    যদি (sourcing_a_script(eap) || eap->ea_getline == get_list_সারি)
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
    পূর্ণাঙ্ক
eval_to_bool(
    char_u	*arg,
    পূর্ণাঙ্ক		*error,
    exarg_T	*eap,
    পূর্ণাঙ্ক		skip,	    // only parse, don't execute
    পূর্ণাঙ্ক		use_simple_function)
{
    typval_T	tv;
    varnumber_T	retval = FALSE;
    evalarg_T	evalarg;
    পূর্ণাঙ্ক		r;

    fill_evalarg_from_eap(&evalarg, eap, skip);

    যদি (skip)
	++emsg_skip;
    যদি (use_simple_function)
	r = eval0_simple_funccal(arg, &tv, eap, &evalarg);
    অন্যথা
	r = eval0(arg, &tv, eap, &evalarg);
    যদি (r == FAIL)
	*error = TRUE;
    অন্যথা
    {
	*error = FALSE;
	যদি (!skip)
	{
	    যদি (in_vim9script())
		retval = tv_get_bool_chk(&tv, error);
	    অন্যথা
		retval = (tv_get_সংখ্যা_chk(&tv, error) != 0);
	    clear_tv(&tv);
	}
    }
    যদি (skip)
	--emsg_skip;
    clear_evalarg(&evalarg, eap);

    প্রত্যাগম (পূর্ণাঙ্ক)retval;
}

/*
 * Call eval1() and give an error message if not done at a lower level.
 */
    স্থির পূর্ণাঙ্ক
eval1_emsg(char_u **arg, typval_T *rettv, exarg_T *eap)
{
    char_u	*start = *arg;
    পূর্ণাঙ্ক		ret;
    পূর্ণাঙ্ক		did_emsg_before = did_emsg;
    পূর্ণাঙ্ক		called_emsg_before = called_emsg;
    evalarg_T	evalarg;

    fill_evalarg_from_eap(&evalarg, eap, eap != রিক্ত && eap->skip);

    ret = eval1(arg, rettv, &evalarg);
    যদি (ret == FAIL)
    {
	// Report the invalid expression unless the expression evaluation has
	// been cancelled due to an aborting error, an interrupt, or an
	// exception, or we already gave a more specific error.
	// Also check called_emsg for when using assert_fails().
	যদি (!aborting() && did_emsg == did_emsg_before
					  && called_emsg == called_emsg_before)
	    semsg(_(e_invalid_expression_str), start);
    }
    clear_evalarg(&evalarg, eap);
    প্রত্যাগম ret;
}

/*
 * Return whether a typval is a valid expression to pass to eval_expr_typval()
 * or eval_expr_to_bool().  An empty string returns FALSE;
 */
    পূর্ণাঙ্ক
eval_রাশিমালা_valid_arg(typval_T *tv)
{
    প্রত্যাগম tv->v_type != VAR_UNKNOWN
	    && (tv->v_type != VAR_STRING
		  || (tv->vval.v_string != রিক্ত && *tv->vval.v_string != NUL));
}

/*
 * When calling eval_expr_typval() many times we only need one funccall_T.
 * Returns NULL when no funccall_T is to be used.
 * When returning non-NULL remove_funccal() must be called later.
 */
    funccall_T *
eval_রাশিমালা_get_funccal(typval_T *রাশিমালা, typval_T *rettv)
{
    যদি (রাশিমালা->v_type != VAR_PARTIAL)
	প্রত্যাগম রিক্ত;

    partial_T *partial = রাশিমালা->vval.v_partial;
    যদি (partial == রিক্ত)
	প্রত্যাগম রিক্ত;
    যদি (partial->pt_func == রিক্ত
	    || partial->pt_func->uf_def_status == UF_NOT_COMPILED)
	প্রত্যাগম রিক্ত;

    প্রত্যাগম create_funccal(partial->pt_func, rettv);
}

/*
 * Evaluate a partial.
 * Pass arguments "argv[argc]".
 * "fc_arg" is from eval_expr_get_funccal() or NULL;
 * Return the result in "rettv" and OK or FAIL.
 */
    স্থির পূর্ণাঙ্ক
eval_রাশিমালা_partial(
    typval_T	*রাশিমালা,
    typval_T	*argv,
    পূর্ণাঙ্ক		argc,
    funccall_T	*fc_arg,
    typval_T	*rettv)
{
    partial_T	*partial = রাশিমালা->vval.v_partial;

    যদি (partial == রিক্ত)
	প্রত্যাগম FAIL;

    যদি (partial->pt_func != রিক্ত
			&& partial->pt_func->uf_def_status != UF_NOT_COMPILED)
    {
	funccall_T	*fc = fc_arg != রিক্ত ? fc_arg
				: create_funccal(partial->pt_func, rettv);
	পূর্ণাঙ্ক		r;

	যদি (fc == রিক্ত)
	    প্রত্যাগম FAIL;

	// Shortcut to call a compiled function with minimal overhead.
	যদি (partial->pt_obj != রিক্ত)
	    partial->pt_obj->obj_refcount++;
	r = call_def_function(partial->pt_func, argc, argv, DEF_USE_PT_ARGV,
					partial, partial->pt_obj, fc, rettv);
	যদি (fc_arg == রিক্ত)
	    remove_funccal();
	যদি (r == FAIL)
	    প্রত্যাগম FAIL;
    }
    অন্যথা
    {
	char_u		*s = partial_name(partial);
	funcexe_T	funcexe;

	যদি (s == রিক্ত || *s == NUL)
	    প্রত্যাগম FAIL;

	CLEAR_FIELD(funcexe);
	funcexe.fe_evaluate = TRUE;
	funcexe.fe_partial = partial;
	যদি (call_func(s, -1, rettv, argc, argv, &funcexe) == FAIL)
	    প্রত্যাগম FAIL;
    }

    প্রত্যাগম OK;
}

/*
 * Evaluate an expression which is a function.
 * Pass arguments "argv[argc]".
 * Return the result in "rettv" and OK or FAIL.
 */
    স্থির পূর্ণাঙ্ক
eval_রাশিমালা_func(
    typval_T	*রাশিমালা,
    typval_T	*argv,
    পূর্ণাঙ্ক		argc,
    typval_T	*rettv)
{
    funcexe_T	funcexe;
    char_u	buf[NUMBUFLEN];
    char_u	*s;

    যদি (রাশিমালা->v_type == VAR_FUNC)
	s = রাশিমালা->vval.v_string;
    অন্যথা
	s = tv_get_string_buf_chk_strict(রাশিমালা, buf, in_vim9script());
    যদি (s == রিক্ত || *s == NUL)
	প্রত্যাগম FAIL;

    CLEAR_FIELD(funcexe);
    funcexe.fe_evaluate = TRUE;
    যদি (call_func(s, -1, rettv, argc, argv, &funcexe) == FAIL)
	প্রত্যাগম FAIL;

    প্রত্যাগম OK;
}

/*
 * Evaluate an expression, which is a string.
 * Return the result in "rettv" and OK or FAIL.
 */
    স্থির পূর্ণাঙ্ক
eval_রাশিমালা_string(
    typval_T	*রাশিমালা,
    typval_T	*rettv)
{
    char_u	*s;
    char_u	buf[NUMBUFLEN];

    s = tv_get_string_buf_chk_strict(রাশিমালা, buf, in_vim9script());
    যদি (s == রিক্ত)
	প্রত্যাগম FAIL;

    s = skipwhite(s);
    যদি (eval1_emsg(&s, rettv, রিক্ত) == FAIL)
	প্রত্যাগম FAIL;

    যদি (*skipwhite(s) != NUL)  // check for trailing chars after expr
    {
	clear_tv(rettv);
	semsg(_(e_invalid_expression_str), s);
	প্রত্যাগম FAIL;
    }

    প্রত্যাগম OK;
}

/*
 * Evaluate an expression, which can be a function, partial or string.
 * Pass arguments "argv[argc]".
 * If "want_func" is TRUE treat a string as a function name, not an expression.
 * "fc_arg" is from eval_expr_get_funccal() or NULL;
 * Return the result in "rettv" and OK or FAIL.
 */
    পূর্ণাঙ্ক
eval_রাশিমালা_typval(
	typval_T    *রাশিমালা,
	পূর্ণাঙ্ক	    want_func,
	typval_T    *argv,
	পূর্ণাঙ্ক	    argc,
	funccall_T  *fc_arg,
	typval_T    *rettv)
{
    যদি (রাশিমালা->v_type == VAR_PARTIAL)
	প্রত্যাগম eval_রাশিমালা_partial(রাশিমালা, argv, argc, fc_arg, rettv);
    যদি (রাশিমালা->v_type == VAR_INSTR)
	প্রত্যাগম exe_typval_instr(রাশিমালা, rettv);
    যদি (রাশিমালা->v_type == VAR_FUNC || want_func)
	প্রত্যাগম eval_রাশিমালা_func(রাশিমালা, argv, argc, rettv);

    প্রত্যাগম eval_রাশিমালা_string(রাশিমালা, rettv);
}

/*
 * Like eval_to_bool() but using a typval_T instead of a string.
 * Works for string, funcref and partial.
 */
    পূর্ণাঙ্ক
eval_রাশিমালা_to_bool(typval_T *রাশিমালা, পূর্ণাঙ্ক *error)
{
    typval_T	rettv;
    পূর্ণাঙ্ক		res;

    যদি (eval_রাশিমালা_typval(রাশিমালা, FALSE, রিক্ত, 0, রিক্ত, &rettv) == FAIL)
    {
	*error = TRUE;
	প্রত্যাগম FALSE;
    }
    res = (tv_get_bool_chk(&rettv, error) != 0);
    clear_tv(&rettv);
    প্রত্যাগম res;
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
    পূর্ণাঙ্ক		skip)	    // only parse, don't execute
{
    typval_T	tv;
    char_u	*retval;
    evalarg_T	evalarg;

    fill_evalarg_from_eap(&evalarg, eap, skip);
    যদি (skip)
	++emsg_skip;
    যদি (eval0(arg, &tv, eap, &evalarg) == FAIL || skip)
	retval = রিক্ত;
    অন্যথা
    {
	retval = vim_strsave(tv_get_string(&tv));
	clear_tv(&tv);
    }
    যদি (skip)
	--emsg_skip;
    clear_evalarg(&evalarg, eap);

    প্রত্যাগম retval;
}

/*
 * Initialize "evalarg" for use.
 */
    শূন্য
init_evalarg(evalarg_T *evalarg)
{
    CLEAR_POINTER(evalarg);
    ga_init2(&evalarg->eval_tofree_ga, পরিমাণ(char_u *), 20);
}

/*
 * If "evalarg->eval_tofree" is not NULL free it later.
 * Caller is expected to overwrite "evalarg->eval_tofree" next.
 */
    স্থির শূন্য
free_eval_tofree_later(evalarg_T *evalarg)
{
    যদি (evalarg->eval_tofree == রিক্ত)
	প্রত্যাগম;

    যদি (ga_grow(&evalarg->eval_tofree_ga, 1) == OK)
	((char_u **)evalarg->eval_tofree_ga.ga_data)
	    [evalarg->eval_tofree_ga.ga_len++]
	    = evalarg->eval_tofree;
    অন্যথা
	vim_free(evalarg->eval_tofree);
}

/*
 * After using "evalarg" filled from "eap": free the memory.
 */
    শূন্য
clear_evalarg(evalarg_T *evalarg, exarg_T *eap)
{
    যদি (evalarg == রিক্ত)
	প্রত্যাগম;

    garray_T *etga = &evalarg->eval_tofree_ga;

    যদি (evalarg->eval_tofree != রিক্ত || evalarg->eval_using_cmdline)
    {
	যদি (eap != রিক্ত)
	{
	    // We may need to keep the original command line, e.g. for
	    // ":let" it has the variable names.  But we may also need
	    // the new one, "nextcmd" points into it.  Keep both.
	    vim_free(eap->cmdline_tofree);
	    eap->cmdline_tofree = *eap->cmdlinep;

	    যদি (evalarg->eval_using_cmdline && etga->ga_len > 0)
	    {
		// "nextcmd" points into the last line in eval_tofree_ga,
		// need to keep it around.
		--etga->ga_len;
		*eap->cmdlinep = ((char_u **)etga->ga_data)[etga->ga_len];
		vim_free(evalarg->eval_tofree);
	    }
	    অন্যথা
		*eap->cmdlinep = evalarg->eval_tofree;
	}
	অন্যথা
	    vim_free(evalarg->eval_tofree);
	evalarg->eval_tofree = রিক্ত;
    }

    ga_clear_strings(etga);
    VIM_CLEAR(evalarg->eval_tofree_lambda);
}

/*
 * Skip over an expression at "*pp".
 * Return FAIL for an error, OK otherwise.
 */
    পূর্ণাঙ্ক
skip_রাশিমালা(char_u **pp, evalarg_T *evalarg)
{
    typval_T	rettv;

    *pp = skipwhite(*pp);
    প্রত্যাগম eval1(pp, &rettv, evalarg);
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
    পূর্ণাঙ্ক
skip_রাশিমালা_concatenate(
	char_u	    **arg,
	char_u	    **start,
	char_u	    **end,
	evalarg_T   *evalarg)
{
    typval_T	rettv;
    পূর্ণাঙ্ক		res;
    পূর্ণাঙ্ক		vim9script = in_vim9script();
    garray_T    *gap = evalarg == রিক্ত ? রিক্ত : &evalarg->eval_ga;
    garray_T    *freegap = evalarg == রিক্ত ? রিক্ত : &evalarg->eval_freega;
    পূর্ণাঙ্ক		save_flags = evalarg == রিক্ত ? 0 : evalarg->eval_flags;
    পূর্ণাঙ্ক		evaluate = evalarg == রিক্ত
			       ? FALSE : (evalarg->eval_flags & EVAL_EVALUATE);

    যদি (vim9script && evaluate
	       && (evalarg->eval_cookie != রিক্ত || evalarg->eval_cctx != রিক্ত))
    {
	ga_init2(gap, পরিমাণ(char_u *), 10);
	// leave room for "start"
	যদি (ga_grow(gap, 1) == OK)
	    ++gap->ga_len;
	ga_init2(freegap, পরিমাণ(char_u *), 10);
    }
    *start = *arg;

    // Don't evaluate the expression.
    যদি (evalarg != রিক্ত)
	evalarg->eval_flags &= ~EVAL_EVALUATE;
    *arg = skipwhite(*arg);
    res = eval1(arg, &rettv, evalarg);
    *end = *arg;
    যদি (evalarg != রিক্ত)
	evalarg->eval_flags = save_flags;

    যদি (vim9script && evaluate
	    && (evalarg->eval_cookie != রিক্ত || evalarg->eval_cctx != রিক্ত))
    {
	যদি (evalarg->eval_ga.ga_len == 1)
	{
	    // just the one line, no need to concatenate
	    ga_clear(gap);
	    gap->ga_itemsize = 0;
	}
	অন্যথা
	{
	    char_u	    *p;
	    size_t	    endoff = STRLEN(*arg);

	    // Line breaks encountered, concatenate all the lines.
	    *((char_u **)gap->ga_data) = *start;
	    p = ga_concat_strings(gap, " ");

	    // free the lines only when using getsourceline()
	    যদি (evalarg->eval_cookie != রিক্ত)
	    {
		// Do not free the first line, the caller can still use it.
		*((char_u **)gap->ga_data) = রিক্ত;
		// Do not free the last line, "arg" points into it, free it
		// later.  Also free "eval_tofree" later if needed.
		free_eval_tofree_later(evalarg);
		evalarg->eval_tofree =
				    ((char_u **)gap->ga_data)[gap->ga_len - 1];
		((char_u **)gap->ga_data)[gap->ga_len - 1] = রিক্ত;
		ga_clear_strings(gap);
		ga_clear(freegap);
	    }
	    অন্যথা
	    {
		ga_clear(gap);

		// free lines that were explicitly marked for freeing
		ga_clear_strings(freegap);
	    }

	    gap->ga_itemsize = 0;
	    যদি (p == রিক্ত)
		প্রত্যাগম FAIL;
	    *start = p;
	    vim_free(evalarg->eval_tofree_lambda);
	    evalarg->eval_tofree_lambda = p;
	    // Compute "end" relative to the end.
	    *end = *start + STRLEN(*start) - endoff;
	}
    }

    প্রত্যাগম res;
}

/*
 * Convert "tv" to a string.
 * When "join_list" is TRUE convert a List or a Tuple into a sequence of lines.
 * Returns an allocated string (NULL when out of memory).
 */
    char_u *
typval2string(typval_T *tv, পূর্ণাঙ্ক join_list)
{
    garray_T	ga;
    char_u	*retval = রিক্ত;

    যদি (join_list && (tv->v_type == VAR_LIST || tv->v_type == VAR_TUPLE))
    {
	যদি (tv->v_type == VAR_LIST)
	{
	    ga_init2(&ga, পরিমাণ(বর্ণ), 80);
	    যদি (tv->vval.v_list != রিক্ত)
	    {
		list_join(&ga, tv->vval.v_list, (char_u *)"\n", TRUE, FALSE,
									0);
		যদি (tv->vval.v_list->lv_len > 0)
		    ga_append(&ga, NL);
	    }
	    ga_append(&ga, NUL);
	    retval = (char_u *)ga.ga_data;
	}
	অন্যথা
	{
	    // tuple
	    ga_init2(&ga, পরিমাণ(বর্ণ), 80);
	    যদি (tv->vval.v_tuple != রিক্ত)
	    {
		tuple_join(&ga, tv->vval.v_tuple, (char_u *)"\n", TRUE, FALSE,
									0);
		যদি (TUPLE_LEN(tv->vval.v_tuple) > 0)
		    ga_append(&ga, NL);
	    }
	    ga_append(&ga, NUL);
	    retval = (char_u *)ga.ga_data;
	}
    }
    অন্যথা যদি (tv->v_type == VAR_LIST
	    || tv->v_type == VAR_TUPLE
	    || tv->v_type == VAR_DICT)
    {
	char_u	*tofree;
	char_u	numbuf[NUMBUFLEN];

	retval = tv2string(tv, &tofree, numbuf, 0);
	// Make a copy if we have a value but it's not in allocated memory.
	যদি (retval != রিক্ত && tofree == রিক্ত)
	    retval = vim_strsave(retval);
    }
    অন্যথা
	retval = vim_strsave(tv_get_string(tv));
    প্রত্যাগম retval;
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
    পূর্ণাঙ্ক		join_list,
    exarg_T	*eap,
    পূর্ণাঙ্ক		use_simple_function)
{
    typval_T	tv;
    char_u	*retval;
    evalarg_T	evalarg;
    পূর্ণাঙ্ক		r;

    fill_evalarg_from_eap(&evalarg, eap, eap != রিক্ত && eap->skip);
    যদি (use_simple_function)
	r = eval0_simple_funccal(arg, &tv, রিক্ত, &evalarg);
    অন্যথা
	r = eval0(arg, &tv, রিক্ত, &evalarg);
    যদি (r == FAIL)
	retval = রিক্ত;
    অন্যথা
    {
	retval = typval2string(&tv, join_list);
	clear_tv(&tv);
    }
    clear_evalarg(&evalarg, রিক্ত);

    প্রত্যাগম retval;
}

    char_u *
eval_to_string(
    char_u	*arg,
    পূর্ণাঙ্ক		join_list,
    পূর্ণাঙ্ক		use_simple_function)
{
    প্রত্যাগম eval_to_string_eap(arg, join_list, রিক্ত, use_simple_function);
}

/*
 * Call eval_to_string() without using current local variables and using
 * textlock.  When "use_sandbox" is TRUE use the sandbox.
 * Use legacy Vim script syntax.
 */
    char_u *
eval_to_string_safe(
    char_u	*arg,
    পূর্ণাঙ্ক		use_sandbox,
    পূর্ণাঙ্ক		keep_script_version,
    পূর্ণাঙ্ক		use_simple_function)
{
    char_u	*retval;
    funccal_entry_T funccal_entry;
    পূর্ণাঙ্ক		save_sc_version = current_sctx.sc_version;
    পূর্ণাঙ্ক		save_garbage = may_garbage_collect;

    যদি (!keep_script_version)
	current_sctx.sc_version = 1;
    save_funccal(&funccal_entry);
    যদি (use_sandbox)
	++sandbox;
    ++textlock;
    may_garbage_collect = FALSE;
    retval = eval_to_string(arg, FALSE, use_simple_function);
    যদি (use_sandbox)
	--sandbox;
    --textlock;
    may_garbage_collect = save_garbage;
    restore_funccal();
    current_sctx.sc_version = save_sc_version;
    প্রত্যাগম retval;
}

/*
 * Top level evaluation function, returning a number.
 * Evaluates "expr" silently.
 * Returns -1 for an error.
 */
    varnumber_T
eval_to_সংখ্যা(char_u *রাশিমালা, পূর্ণাঙ্ক use_simple_function)
{
    typval_T	rettv;
    varnumber_T	retval;
    char_u	*p = skipwhite(রাশিমালা);
    পূর্ণাঙ্ক		r = NOTDONE;

    ++emsg_off;

    যদি (use_simple_function)
	r = may_call_simple_func(রাশিমালা, &rettv);
    যদি (r == NOTDONE)
	r = eval1(&p, &rettv, &EVALARG_EVALUATE);
    যদি (r == FAIL)
	retval = -1;
    অন্যথা
    {
	retval = tv_get_সংখ্যা_chk(&rettv, রিক্ত);
	clear_tv(&rettv);
    }
    --emsg_off;

    প্রত্যাগম retval;
}

/*
 * Top level evaluation function.
 * Returns an allocated typval_T with the result.
 * Returns NULL when there is an error.
 */
    typval_T *
eval_রাশিমালা(char_u *arg, exarg_T *eap)
{
    প্রত্যাগম eval_রাশিমালা_ext(arg, eap, FALSE);
}

    typval_T *
eval_রাশিমালা_ext(char_u *arg, exarg_T *eap, পূর্ণাঙ্ক use_simple_function)
{
    typval_T	*tv;
    evalarg_T	evalarg;

    fill_evalarg_from_eap(&evalarg, eap, eap != রিক্ত && eap->skip);

    tv = ALLOC_ONE(typval_T);
    যদি (tv != রিক্ত)
    {
	পূর্ণাঙ্ক	r = NOTDONE;

	যদি (use_simple_function)
	    r = eval0_simple_funccal(arg, tv, eap, &evalarg);
	যদি (r == NOTDONE)
	    r = eval0(arg, tv, eap, &evalarg);

	যদি (r == FAIL)
	    VIM_CLEAR(tv);
    }

    clear_evalarg(&evalarg, eap);
    প্রত্যাগম tv;
}

/*
 * "*arg" points to what can be a function name in the form of "import.Name" or
 * "Funcref".  Return the name of the function.  Set "tofree" to something that
 * was allocated.
 * If "verbose" is FALSE no errors are given.
 * Return NULL for any failure.
 */
    স্থির char_u *
deref_function_name(
	    char_u	**arg,
	    char_u	**tofree,
	    evalarg_T	*evalarg,
	    পূর্ণাঙ্ক		verbose)
{
    typval_T	ref;
    char_u	*name = *arg;
    পূর্ণাঙ্ক		save_flags = 0;
    পূর্ণাঙ্ক		evaluate = evalarg != রিক্ত
				      && (evalarg->eval_flags & EVAL_EVALUATE);

    ref.v_type = VAR_UNKNOWN;
    যদি (evalarg != রিক্ত)
    {
	// need to evaluate this to get an import, like in "a.Func"
	save_flags = evalarg->eval_flags;
	evalarg->eval_flags |= EVAL_EVALUATE;
    }
    যদি (eval9(arg, &ref, evalarg, FALSE) == FAIL)
    {
	dictitem_T	*v;

	// If <SID>VarName was used it would not be found, try another way.
	v = find_var_also_in_script(name, রিক্ত, FALSE);
	যদি (v == রিক্ত)
	{
	    name = রিক্ত;
	    গচ্ছ theend;
	}
	copy_tv(&v->di_tv, &ref);
    }
    যদি (*skipwhite(*arg) != NUL)
    {
	যদি (verbose)
	    semsg(_(e_trailing_characters_str), *arg);
	name = রিক্ত;
    }
    অন্যথা যদি (ref.v_type == VAR_FUNC && ref.vval.v_string != রিক্ত)
    {
	name = ref.vval.v_string;
	ref.vval.v_string = রিক্ত;
	*tofree = name;
    }
    অন্যথা যদি (ref.v_type == VAR_PARTIAL && ref.vval.v_partial != রিক্ত)
    {
	যদি (ref.vval.v_partial->pt_argc > 0
		|| ref.vval.v_partial->pt_dict != রিক্ত)
	{
	    যদি (verbose)
		emsg(_(e_cannot_use_partial_here));
	    name = রিক্ত;
	}
	অন্যথা
	{
	    name = vim_strsave(partial_name(ref.vval.v_partial));
	    *tofree = name;
	}
    }
    অন্যথা যদি (evaluate)
    {
	যদি (verbose)
	    semsg(_(e_not_callable_type_str), name);
	name = রিক্ত;
    }

theend:
    clear_tv(&ref);
    যদি (evalarg != রিক্ত)
	evalarg->eval_flags = save_flags;
    প্রত্যাগম name;
}

/*
 * Call some Vim script function and return the result in "*rettv".
 * Uses argv[0] to argv[argc - 1] for the function arguments.  argv[argc]
 * should have type VAR_UNKNOWN.
 * Returns OK or FAIL.
 */
    পূর্ণাঙ্ক
call_vim_function(
    char_u      *func,
    পূর্ণাঙ্ক		argc,
    typval_T	*argv,
    typval_T	*rettv)
{
    পূর্ণাঙ্ক		ret;
    funcexe_T	funcexe;
    char_u	*arg;
    char_u	*name;
    char_u	*tofree = রিক্ত;
    পূর্ণাঙ্ক		ignore_errors;

    rettv->v_type = VAR_UNKNOWN;		// clear_tv() uses this
    CLEAR_FIELD(funcexe);
    funcexe.fe_firstline = curwin->w_cursor.lnum;
    funcexe.fe_lastline = curwin->w_cursor.lnum;
    funcexe.fe_evaluate = TRUE;

    // The name might be "import.Func" or "Funcref".  We don't know, we need to
    // ignore errors for an undefined name.  But we do want errors when an
    // autoload script has errors.  Guess that when there is a dot in the name
    // showing errors is the right choice.
    ignore_errors = vim_strchr(func, '.') == রিক্ত;
    arg = func;
    যদি (ignore_errors)
	++emsg_off;
    name = deref_function_name(&arg, &tofree, &EVALARG_EVALUATE, FALSE);
    যদি (ignore_errors)
	--emsg_off;
    যদি (name == রিক্ত)
	name = func;

    ret = call_func(name, -1, rettv, argc, argv, &funcexe);

    যদি (ret == FAIL)
	clear_tv(rettv);
    vim_free(tofree);

    প্রত্যাগম ret;
}

/*
 * Call Vim script function "func" and return the result as a string.
 * Uses "argv[0]" to "argv[argc - 1]" for the function arguments. "argv[argc]"
 * should have type VAR_UNKNOWN.
 * Returns NULL when calling the function fails.
 */
    শূন্য *
call_func_retstr(
    char_u      *func,
    পূর্ণাঙ্ক		argc,
    typval_T	*argv)
{
    typval_T	rettv;
    char_u	*retval;

    যদি (call_vim_function(func, argc, argv, &rettv) == FAIL)
	প্রত্যাগম রিক্ত;

    retval = vim_strsave(tv_get_string(&rettv));
    clear_tv(&rettv);
    প্রত্যাগম retval;
}

/*
 * Call Vim script function "func" and return the result as a List.
 * Uses "argv" and "argc" as call_func_retstr().
 * Returns NULL when there is something wrong.
 * Gives an error when the returned value is not a list.
 */
    শূন্য *
call_func_retlist(
    char_u      *func,
    পূর্ণাঙ্ক		argc,
    typval_T	*argv)
{
    typval_T	rettv;

    যদি (call_vim_function(func, argc, argv, &rettv) == FAIL)
	প্রত্যাগম রিক্ত;

    যদি (rettv.v_type != VAR_LIST)
    {
	semsg(_(e_custom_list_completion_function_does_not_return_list_but_str),
		vartype_name(rettv.v_type));
	clear_tv(&rettv);
	প্রত্যাগম রিক্ত;
    }

    প্রত্যাগম rettv.vval.v_list;
}

#যদি defined(FEAT_FOLDING)
/*
 * Evaluate "arg", which is 'foldexpr'.
 * Note: caller must set "curwin" to match "arg".
 * Returns the foldlevel, and any character preceding it in "*cp".  Doesn't
 * give error messages.
 */
    পূর্ণাঙ্ক
eval_foldexpr(win_T *wp, পূর্ণাঙ্ক *cp)
{
    char_u	*arg;
    typval_T	tv;
    varnumber_T	retval;
    char_u	*s;
    sctx_T	saved_sctx = current_sctx;
    পূর্ণাঙ্ক		use_sandbox = was_set_insecurely(wp, (char_u *)"foldexpr",
								    OPT_LOCAL);

    arg = skipwhite(wp->w_p_fde);
    current_sctx = wp->w_p_script_ctx[WV_FDE];

    ++emsg_off;
    যদি (use_sandbox)
	++sandbox;
    ++textlock;
    *cp = NUL;

    // Evaluate the expression.  If the expression is "FuncName()" call the
    // function directly.
    যদি (eval0_simple_funccal(arg, &tv, রিক্ত, &EVALARG_EVALUATE) == FAIL)
	retval = 0;
    অন্যথা
    {
	// If the result is a number, just return the number.
	যদি (tv.v_type == VAR_সংখ্যা)
	    retval = tv.vval.v_সংখ্যা;
	অন্যথা যদি (tv.v_type != VAR_STRING || tv.vval.v_string == রিক্ত)
	    retval = 0;
	অন্যথা
	{
	    // If the result is a string, check if there is a non-digit before
	    // the number.
	    s = tv.vval.v_string;
	    যদি (*s != NUL && !VIM_ISDIGIT(*s) && *s != '-')
		*cp = *s++;
	    retval = atol((বর্ণ *)s);
	}
	clear_tv(&tv);
    }
    --emsg_off;
    যদি (use_sandbox)
	--sandbox;
    --textlock;
    clear_evalarg(&EVALARG_EVALUATE, রিক্ত);
    current_sctx = saved_sctx;

    প্রত্যাগম (পূর্ণাঙ্ক)retval;
}
#endif

#ifdef LOG_LOCKVAR
প্রকারনির্দেশ সংরচনা flag_string_S
{
    পূর্ণাঙ্ক	    flag;
    বর্ণ    *str;
} flag_string_T;

    স্থির বর্ণ *
flags_tostring(পূর্ণাঙ্ক flags, flag_string_T *_fstring, বর্ণ *buf, size_t n)
{
    বর্ণ *p = buf;
    *p = NUL;
    প্রতি (flag_string_T *fstring = _fstring; fstring->flag; ++fstring)
    {
	যদি ((fstring->flag & flags) != 0)
	{
	    size_t len = STRLEN(fstring->str);
	    যদি (n > p - buf + len + 7)
	    {
		STRCAT(p, fstring->str);
		p += len;
		STRCAT(p, " ");
		++p;
	    }
	    অন্যথা
	    {
		STRCAT(buf, "...");
		ভঙ্গ;
	    }
	}
    }
    প্রত্যাগম buf;
}

flag_string_T glv_flag_strings[] = {
    { GLV_QUIET,		"QUIET" },
    { GLV_NO_AUTOLOAD,		"NO_AUTOLOAD" },
    { GLV_READ_ONLY,		"READ_ONLY" },
    { GLV_NO_DECL,		"NO_DECL" },
    { GLV_COMPILING,		"COMPILING" },
    { GLV_ASSIGN_WITH_OP,	"ASSIGN_WITH_OP" },
    { GLV_PREFER_FUNC,		"PREFER_FUNC" },
    { 0,			রিক্ত }
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
    স্থির শূন্য
fill_lval_from_lval_মূল(lval_T *lp, lval_মূল_T *lr)
{
#ifdef LOG_LOCKVAR
    ch_log(রিক্ত, "LKVAR: fill_lval_from_lval_root(): name %s, tv %p",
						lp->ll_name, (শূন্য*)lr->lr_tv);
#endif
    যদি (lr->lr_tv == রিক্ত)
	প্রত্যাগম;
    যদি (!lr->lr_is_arg && lr->lr_tv->v_type == VAR_CLASS)
    {
	যদি (lr->lr_tv->vval.v_class != রিক্ত)
	{
	    // Special special case. Look for a bare class variable reference.
	    class_T	*cl = lr->lr_tv->vval.v_class;
	    পূর্ণাঙ্ক		m_idx;
	    ocmember_T	*m = class_member_lookup(cl, lp->ll_name,
					lp->ll_name_end - lp->ll_name, &m_idx);
	    যদি (m != রিক্ত)
	    {
		// Assuming "inside class" since bare reference.
		lp->ll_class = lr->lr_tv->vval.v_class;
		lp->ll_oi = m_idx;
		lp->ll_valtype = m->ocm_type;
		lp->ll_tv = &lp->ll_class->class_members_tv[m_idx];
#ifdef LOG_LOCKVAR
		ch_log(রিক্ত, "LKVAR:    ... class member %s.%s",
					lp->ll_class->class_name.string, lp->ll_name);
#endif
		প্রত্যাগম;
	    }
	}
    }

#ifdef LOG_LOCKVAR
    ch_log(রিক্ত, "LKVAR:    ... type: %s", vartype_name(lr->lr_tv->v_type));
#endif
    lp->ll_tv = lr->lr_tv;
    lp->ll_is_মূল = TRUE;
}

/*
 * Check if the class has permission to access the member.
 * Returns OK or FAIL.
 */
    স্থির পূর্ণাঙ্ক
get_lval_check_access(
    class_T	*cl_exec,   // executing class, NULL if :def or script level
    class_T	*cl,	    // class which contains the member
    ocmember_T	*om,	    // member being accessed
    char_u	*p,	    // char after member name
    পূর্ণাঙ্ক		flags)	    // GLV flags to check if writing to lval
{
#ifdef LOG_LOCKVAR
    ch_log(রিক্ত, "LKVAR: get_lval_check_access(), cl_exec %p, cl %p, %c",
						(শূন্য*)cl_exec, (শূন্য*)cl, *p);
#endif
    যদি (cl_exec != রিক্ত && cl_exec == cl)
	প্রত্যাগম OK;

    বর্ণ *বার্তা = রিক্ত;
    বিকল্প (om->ocm_access)
    {
	পক্ষ VIM_ACCESS_PRIVATE:
	    বার্তা = e_cannot_access_protected_variable_str;
	    ভঙ্গ;
	পক্ষ VIM_ACCESS_READ:
	    // If [idx] or .key following, read only OK.
	    যদি (*p == '[' || *p == '.')
		ভঙ্গ;
	    যদি ((flags & GLV_READ_ONLY) == 0)
	    {
		যদি (IS_ENUM(cl))
		{
		    যদি (om->ocm_type->tt_type == VAR_OBJECT)
			semsg(_(e_enumvalue_str_cannot_be_modified),
				cl->class_name.string, om->ocm_name.string);
		    অন্যথা
			বার্তা = e_variable_is_not_writable_str;
		}
		অন্যথা
		    বার্তা = e_variable_is_not_writable_str;
	    }
	    ভঙ্গ;
	পক্ষ VIM_ACCESS_ALL:
	    ভঙ্গ;
    }
    যদি (বার্তা != রিক্ত)
    {
	emsg_var_cl_define(বার্তা, om->ocm_name.string, 0, cl);
	প্রত্যাগম FAIL;
    }
    প্রত্যাগম OK;
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
    স্থির char_u *
get_lval_imported(
    lval_T	*lp,
    scid_T	imp_sid,
    char_u	*p,
    dictitem_T	**dip,
    পূর্ণাঙ্ক		fne_flags)
{
    ufunc_T	*ufunc;
    type_T	*type = রিক্ত;
    পূর্ণাঙ্ক		cc;
    পূর্ণাঙ্ক		rc = FAIL;

    p = skipwhite(p);

    import_check_sourced_sid(&imp_sid);
    lp->ll_sid = imp_sid;
    lp->ll_name = p;
    p = find_name_end(lp->ll_name, রিক্ত, রিক্ত, fne_flags);
    lp->ll_name_end = p;

    // check the item is exported
    cc = *p;
    *p = NUL;
    যদি (find_exported(imp_sid, lp->ll_name, &ufunc, &type, রিক্ত, রিক্ত,
								TRUE) == -1)
	গচ্ছ failed;

    // Get the typval for the exported item
    hashtab_T *ht = &SCRIPT_VARS(imp_sid);
    যদি (ht == রিক্ত)
	গচ্ছ failed;

    dictitem_T *di = find_var_in_ht(ht, 0, lp->ll_name, TRUE);
    যদি (di == রিক্ত)
	// script is autoloaded.  So variable will be found later
	গচ্ছ success;

    *dip = di;

    // Check whether the variable is writable.
    svar_T *sv = find_typval_in_script(&di->di_tv, imp_sid, FALSE);
    যদি (sv != রিক্ত && sv->sv_const != 0)
    {
	semsg(_(e_cannot_change_readonly_variable_str), lp->ll_name);
	গচ্ছ failed;
    }

    // check whether variable is locked
    যদি (মান_check_lock(di->di_tv.v_lock, lp->ll_name, FALSE))
	গচ্ছ failed;

    lp->ll_tv = &di->di_tv;
    lp->ll_valtype = type;

success:
    rc = OK;

failed:
    *p = cc;
    প্রত্যাগম rc == OK ? p : রিক্ত;
}

প্রকারনির্দেশ গণনা {
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
    স্থির পূর্ণাঙ্ক
get_lval_dict_item(
    lval_T	*lp,
    char_u	*name,
    char_u	*চাবি,
    পূর্ণাঙ্ক		len,
    char_u	**চাবি_end,
    typval_T	*var1,
    পূর্ণাঙ্ক		flags,
    পূর্ণাঙ্ক		unlet,
    typval_T	*rettv)
{
    পূর্ণাঙ্ক		quiet = flags & GLV_QUIET;
    char_u	*p = *চাবি_end;

    যদি (len == -1)
    {
	// "[key]": get key from "var1"
	চাবি = tv_get_string_chk(var1);	// is number or string
	যদি (চাবি == রিক্ত)
	    প্রত্যাগম GLV_FAIL;
    }
    lp->ll_list = রিক্ত;
    lp->ll_list = রিক্ত;
    lp->ll_blob = রিক্ত;
    lp->ll_object = রিক্ত;
    lp->ll_class = রিক্ত;
    lp->ll_tuple = রিক্ত;

    // a NULL dict is equivalent with an empty dict
    যদি (lp->ll_tv->vval.v_dict == রিক্ত)
    {
	lp->ll_tv->vval.v_dict = dict_alloc();
	যদি (lp->ll_tv->vval.v_dict == রিক্ত)
	    প্রত্যাগম GLV_FAIL;
	++lp->ll_tv->vval.v_dict->dv_refcount;
    }
    lp->ll_dict = lp->ll_tv->vval.v_dict;

    lp->ll_di = dict_find(lp->ll_dict, চাবি, len);

    // When assigning to a scope dictionary check that a function and
    // variable name is valid (only variable name unless it is l: or
    // g: dictionary). Disallow overwriting a builtin function.
    যদি (rettv != রিক্ত && lp->ll_dict->dv_scope != 0)
    {
	পূর্ণাঙ্ক prevval;

	যদি (len != -1)
	{
	    prevval = চাবি[len];
	    চাবি[len] = NUL;
	}
	অন্যথা
	    prevval = 0; // avoid compiler warning
	পূর্ণাঙ্ক wrong = (lp->ll_dict->dv_scope == VAR_DEF_SCOPE
		&& (rettv->v_type == VAR_FUNC
		    || rettv->v_type == VAR_PARTIAL)
		&& var_wrong_func_name(চাবি, lp->ll_di == রিক্ত))
	    || !valid_varname(চাবি, -1, TRUE);
	যদি (len != -1)
	    চাবি[len] = prevval;
	যদি (wrong)
	    প্রত্যাগম GLV_FAIL;
    }

    যদি (lp->ll_valtype != রিক্ত)
	// use the type of the member
	lp->ll_valtype = lp->ll_valtype->tt_member;

    যদি (lp->ll_di == রিক্ত)
    {
	// Can't add "v:" or "a:" variable.
	যদি (lp->ll_dict == get_vimvar_dict()
		|| &lp->ll_dict->dv_hashtab == get_funccal_args_ht())
	{
	    semsg(_(e_illegal_variable_name_str), name);
	    প্রত্যাগম GLV_FAIL;
	}

	// Key does not exist in dict: may need to add it.
	যদি (*p == '[' || *p == '.' || unlet)
	{
	    যদি (!quiet)
		semsg(_(e_চাবি_not_present_in_dictionary_str), চাবি);
	    প্রত্যাগম GLV_FAIL;
	}
	যদি (len == -1)
	    lp->ll_newkey = vim_strsave(চাবি);
	অন্যথা
	    lp->ll_newkey = vim_strnsave(চাবি, len);
	যদি (lp->ll_newkey == রিক্ত)
	    p = রিক্ত;

	*চাবি_end = p;
	প্রত্যাগম GLV_STOP;
    }
    // existing variable, need to check if it can be changed
    অন্যথা যদি ((flags & GLV_READ_ONLY) == 0
	    && (var_check_ro(lp->ll_di->di_flags, name, FALSE)
		|| var_check_lock(lp->ll_di->di_flags, name, FALSE)))
	প্রত্যাগম GLV_FAIL;

    lp->ll_tv = &lp->ll_di->di_tv;

    প্রত্যাগম GLV_OK;
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
    স্থির পূর্ণাঙ্ক
get_lval_blob(
    lval_T	*lp,
    typval_T	*var1,
    typval_T	*var2,
    পূর্ণাঙ্ক		empty1,
    পূর্ণাঙ্ক		quiet)
{
    long	bloblen = blob_len(lp->ll_tv->vval.v_blob);

    lp->ll_list = রিক্ত;
    lp->ll_dict = রিক্ত;
    lp->ll_object = রিক্ত;
    lp->ll_class = রিক্ত;
    lp->ll_tuple = রিক্ত;

    // Get the number and item for the only or first index of a List or Tuple.
    যদি (empty1)
	lp->ll_n1 = 0;
    অন্যথা
	// is number or string
	lp->ll_n1 = (long)tv_get_সংখ্যা(var1);

    যদি (check_blob_index(bloblen, lp->ll_n1, quiet) == FAIL)
	প্রত্যাগম FAIL;
    যদি (lp->ll_range && !lp->ll_empty2)
    {
	lp->ll_n2 = (long)tv_get_সংখ্যা(var2);
	যদি (check_blob_range(bloblen, lp->ll_n1, lp->ll_n2, quiet) == FAIL)
	    প্রত্যাগম FAIL;
    }

    যদি (!lp->ll_range)
	// Indexing a single byte in a blob.  So the rhs type is a
	// number.
	lp->ll_valtype = &t_সংখ্যা;

    lp->ll_blob = lp->ll_tv->vval.v_blob;
    lp->ll_tv = রিক্ত;

    প্রত্যাগম OK;
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
    স্থির পূর্ণাঙ্ক
get_lval_list(
    lval_T	*lp,
    typval_T	*var1,
    typval_T	*var2,
    পূর্ণাঙ্ক		empty1,
    পূর্ণাঙ্ক		flags,
    পূর্ণাঙ্ক		quiet)
{
    /*
     * Get the number and item for the only or first index of the List.
     */
    যদি (empty1)
	lp->ll_n1 = 0;
    অন্যথা
	// is number or string
	lp->ll_n1 = (long)tv_get_সংখ্যা(var1);

    lp->ll_dict = রিক্ত;
    lp->ll_object = রিক্ত;
    lp->ll_class = রিক্ত;
    lp->ll_tuple = রিক্ত;
    lp->ll_list = lp->ll_tv->vval.v_list;
    lp->ll_li = check_range_index_one(lp->ll_list, &lp->ll_n1,
				(flags & GLV_ASSIGN_WITH_OP) == 0, quiet);
    যদি (lp->ll_li == রিক্ত)
	প্রত্যাগম FAIL;

    যদি (lp->ll_valtype != রিক্ত && !lp->ll_range)
    {
	// use the type of the member
	যদি (lp->ll_valtype->tt_member != রিক্ত)
	    lp->ll_valtype = lp->ll_valtype->tt_member;
	অন্যথা
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
    যদি (lp->ll_range && !lp->ll_empty2)
    {
	lp->ll_n2 = (long)tv_get_সংখ্যা(var2);
	// is number or string
	যদি (check_range_index_two(lp->ll_list,
			&lp->ll_n1, lp->ll_li, &lp->ll_n2, quiet) == FAIL)
	    প্রত্যাগম FAIL;
    }

    lp->ll_tv = &lp->ll_li->li_tv;

    প্রত্যাগম OK;
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
    স্থির পূর্ণাঙ্ক
get_lval_tuple(
    lval_T	*lp,
    typval_T	*idx,
    পূর্ণাঙ্ক		quiet)
{
    // is number or string
    lp->ll_n1 = (long)tv_get_সংখ্যা(idx);

    lp->ll_list = রিক্ত;
    lp->ll_dict = রিক্ত;
    lp->ll_blob = রিক্ত;
    lp->ll_object = রিক্ত;
    lp->ll_class = রিক্ত;

    lp->ll_tuple = lp->ll_tv->vval.v_tuple;
    lp->ll_tv = tuple_find(lp->ll_tuple, lp->ll_n1);
    যদি (lp->ll_tv == রিক্ত)
    {
	যদি (!quiet)
	    semsg(_(e_tuple_index_out_of_range_nr), lp->ll_n1);
	প্রত্যাগম GLV_FAIL;
    }

    // use the type of the member
    যদি (lp->ll_valtype != রিক্ত)
    {
	যদি (lp->ll_valtype != রিক্ত
		&& lp->ll_valtype->tt_type == VAR_TUPLE
		&& lp->ll_valtype->tt_argcount == 1)
	{
	    // a variadic tuple or a single item tuple
	    যদি (lp->ll_valtype->tt_flags & TTFLAG_VARARGS)
		lp->ll_valtype = lp->ll_valtype->tt_args[0]->tt_member;
	    অন্যথা
		lp->ll_valtype = lp->ll_valtype->tt_args[0];
	}
	অন্যথা
	    // If the LHS member type is not known (VAR_ANY), then get it from
	    // the tuple item (after indexing)
	    lp->ll_valtype = typval2type(lp->ll_tv, get_copyID(),
					&lp->ll_type_list, TVTT_DO_MEMBER);
    }

    প্রত্যাগম GLV_OK;
}

/*
 * Get a class or object lval method in class "cl".  The 'key' argument points
 * to the method name and 'key_end' points to the character after 'key'.
 * 'v_type' is VAR_CLASS or VAR_OBJECT.
 *
 * The method index, method function pointer and method type are returned in
 * "lp".
 */
    স্থির পূর্ণাঙ্ক
get_lval_oc_method(
    lval_T	*lp,
    class_T	*cl,
    char_u	*চাবি,
    char_u	**চাবি_end,
    vartype_T	v_type)
{
    // Look for a method with this name.
    // round 1: class functions (skipped for an object)
    // round 2: object methods
    প্রতি (পূর্ণাঙ্ক round = v_type == VAR_OBJECT ? 2 : 1; round <= 2; ++round)
    {
	পূর্ণাঙ্ক	m_idx;
	ufunc_T	*fp;

	fp = method_lookup(cl, round == 1 ? VAR_CLASS : VAR_OBJECT,
						চাবি, *চাবি_end - চাবি, &m_idx);
	lp->ll_oi = m_idx;

	// process generic method (if present)
	যদি (fp && (fp = eval_generic_func(fp, চাবি, চাবি_end)) == রিক্ত)
	    প্রত্যাগম FAIL;

	যদি (fp != রিক্ত)
	{
	    lp->ll_ufunc = fp;
	    lp->ll_valtype = fp->uf_func_type;
	    ভঙ্গ;
	}
    }

    প্রত্যাগম OK;
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
    স্থির পূর্ণাঙ্ক
get_lval_oc_variable(
    lval_T	*lp,
    class_T	*cl,
    char_u	*চাবি,
    char_u	*চাবি_end,
    vartype_T	v_type,
    class_T	*cl_exec,
    পূর্ণাঙ্ক		flags)
{
    পূর্ণাঙ্ক		m_idx;
    ocmember_T	*om;

    om = member_lookup(cl, v_type, চাবি, চাবি_end - চাবি, &m_idx);
    lp->ll_oi = m_idx;
    যদি (om == রিক্ত)
	প্রত্যাগম OK;

    // Check variable is accessible
    যদি (get_lval_check_access(cl_exec, cl, om, চাবি_end, flags) == FAIL)
	প্রত্যাগম FAIL;

    // When lhs is used to modify the variable, check it is not a read-only
    // variable.
    যদি ((flags & GLV_READ_ONLY) == 0 && (*চাবি_end != '.' && *চাবি_end != '[')
	    && oc_var_check_ro(cl, om))
	প্রত্যাগম FAIL;

    lp->ll_valtype = om->ocm_type;

    যদি (v_type == VAR_OBJECT)
	lp->ll_tv = ((typval_T *)(lp->ll_tv->vval.v_object + 1)) + m_idx;
    অন্যথা
	lp->ll_tv = &cl->class_members_tv[m_idx];

    প্রত্যাগম OK;
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
    স্থির পূর্ণাঙ্ক
get_lval_class_or_obj(
    lval_T	*lp,
    char_u	*চাবি,
    char_u	**চাবি_end,
    vartype_T	v_type,
    class_T	*cl_exec,
    পূর্ণাঙ্ক		flags,
    পূর্ণাঙ্ক		quiet)
{
    lp->ll_dict = রিক্ত;
    lp->ll_list = রিক্ত;
    lp->ll_tuple = রিক্ত;

    class_T *cl;
    যদি (v_type == VAR_OBJECT)
    {
	যদি (lp->ll_tv->vval.v_object == রিক্ত)
	{
	    যদি (!quiet)
		emsg(_(e_using_null_object));
	    প্রত্যাগম FAIL;
	}
	cl = lp->ll_tv->vval.v_object->obj_class;
	lp->ll_object = lp->ll_tv->vval.v_object;
    }
    অন্যথা
    {
	cl = lp->ll_tv->vval.v_class;
	lp->ll_object = রিক্ত;
    }
    lp->ll_class = cl;

    যদি (cl == রিক্ত)
	// TODO: what if class is NULL?
	প্রত্যাগম OK;

    lp->ll_valtype = রিক্ত;

    যদি (flags & GLV_PREFER_FUNC)
	যদি (get_lval_oc_method(lp, cl, চাবি, চাবি_end, v_type) == FAIL)
	    প্রত্যাগম FAIL;

    // Look for object/class member variable
    যদি (lp->ll_valtype == রিক্ত)
    {
	যদি (get_lval_oc_variable(lp, cl, চাবি, *চাবি_end, v_type, cl_exec, flags)
								== FAIL)
	    প্রত্যাগম FAIL;
    }

    যদি (lp->ll_valtype == রিক্ত)
    {
	member_not_found_বার্তা(cl, v_type, চাবি, *চাবি_end - চাবি);
	প্রত্যাগম FAIL;
    }

    প্রত্যাগম OK;
}

/*
 * Check whether dot (".") is allowed after the variable "name" with type
 * "v_type".  Only Dict, Class and Object types support a dot after the name.
 * Returns TRUE if dot is allowed after the name.
 */
    স্থির পূর্ণাঙ্ক
dot_allowed_after_type(char_u *name, vartype_T v_type, পূর্ণাঙ্ক quiet)
{
    যদি (v_type != VAR_DICT && v_type != VAR_OBJECT && v_type != VAR_CLASS)
    {
	যদি (!quiet)
	    semsg(_(e_dot_not_allowed_after_str_str),
		    vartype_name(v_type), name);
	প্রত্যাগম FALSE;
    }

    প্রত্যাগম TRUE;
}

/*
 * Check whether left bracket ("[") is allowed after the variable "name" with
 * type "v_type".  Only Dict, List, Tuple and Blob types support a bracket
 * after the variable name.  Returns TRUE if bracket is allowed after the name.
 */
    স্থির পূর্ণাঙ্ক
bracket_allowed_after_type(char_u *name, vartype_T v_type, পূর্ণাঙ্ক quiet)
{
    যদি (v_type == VAR_CLASS || v_type == VAR_OBJECT)
    {
	যদি (!quiet)
	    semsg(_(e_index_not_allowed_after_str_str),
		    vartype_name(v_type), name);
	প্রত্যাগম FALSE;
    }

    প্রত্যাগম TRUE;
}

/*
 * Check whether the variable "name" with type "v_type" can be followed by an
 * index.  Only Dict, List, Tuple, Blob, Object and Class types support
 * indexing.  Returns TRUE if indexing is allowed after the name.
 */
    স্থির পূর্ণাঙ্ক
index_allowed_after_type(char_u *name, vartype_T v_type, পূর্ণাঙ্ক quiet)
{
    যদি (v_type != VAR_LIST
	    && v_type != VAR_TUPLE
	    && v_type != VAR_DICT
	    && v_type != VAR_BLOB
	    && v_type != VAR_OBJECT
	    && v_type != VAR_CLASS)
    {
	যদি (!quiet)
	    semsg(_(e_index_not_allowed_after_str_str),
		    vartype_name(v_type), name);
	প্রত্যাগম FALSE;
    }

    প্রত্যাগম TRUE;
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
    স্থির char_u *
get_lval_subscript(
    lval_T	*lp,
    char_u	*p,
    char_u	*name,
    typval_T	*rettv,
    hashtab_T	*ht,
    dictitem_T	*v,
    পূর্ণাঙ্ক		unlet,
    পূর্ণাঙ্ক		flags,	    // GLV_ values
    class_T	*cl_exec)
{
    পূর্ণাঙ্ক		quiet = flags & GLV_QUIET;
    char_u	*চাবি = রিক্ত;
    পূর্ণাঙ্ক		len;
    typval_T	var1;
    typval_T	var2;
    পূর্ণাঙ্ক		empty1 = FALSE;
    পূর্ণাঙ্ক		rc = FAIL;

    /*
     * Loop until no more [idx] or .key is following.
     */
    var1.v_type = VAR_UNKNOWN;
    var2.v_type = VAR_UNKNOWN;

    যাবত্ (*p == '[' || (*p == '.' && p[1] != '=' && p[1] != '.'))
    {
	vartype_T v_type = lp->ll_tv->v_type;

	যদি (*p == '.' && !dot_allowed_after_type(name, v_type, quiet))
	    গচ্ছ done;

	যদি (*p == '[' && !bracket_allowed_after_type(name, v_type, quiet))
	    গচ্ছ done;

	যদি (!index_allowed_after_type(name, v_type, quiet))
	    গচ্ছ done;

	// A NULL list/blob works like an empty list/blob, allocate one now.
	পূর্ণাঙ্ক r = OK;
	যদি (v_type == VAR_LIST && lp->ll_tv->vval.v_list == রিক্ত)
	    r = rettv_list_alloc(lp->ll_tv);
	অন্যথা যদি (v_type == VAR_BLOB && lp->ll_tv->vval.v_blob == রিক্ত)
	    r = rettv_blob_alloc(lp->ll_tv);
	যদি (r == FAIL)
	    গচ্ছ done;

	যদি (lp->ll_range)
	{
	    যদি (!quiet)
		emsg(_(e_slice_must_come_last));
	    গচ্ছ done;
	}
#ifdef LOG_LOCKVAR
	ch_log(রিক্ত, "LKVAR: get_lval() loop: p: %s, type: %s", p,
		vartype_name(v_type));
#endif

	যদি (current_script_is_vim9() && lp->ll_valtype == রিক্ত
		&& v != রিক্ত
		&& lp->ll_tv == &v->di_tv
		&& ht != রিক্ত && ht == get_script_local_ht())
	{
	    svar_T  *sv = find_typval_in_script(lp->ll_tv, 0, TRUE);

	    // Vim9 script local variable: get the type
	    যদি (sv != রিক্ত)
	    {
		lp->ll_valtype = sv->sv_type;
#ifdef LOG_LOCKVAR
		ch_log(রিক্ত, "LKVAR:    ... loop: vim9 assign type: %s",
			vartype_name(lp->ll_valtype->tt_type));
#endif
	    }
	}

	len = -1;
	যদি (*p == '.')
	{
	    চাবি = p + 1;

	    প্রতি (len = 0; ASCII_ISALNUM(চাবি[len]) || চাবি[len] == '_'; ++len)
		;
	    যদি (len == 0)
	    {
		যদি (!quiet)
		    emsg(_(e_cannot_use_empty_চাবি_for_dictionary));
		গচ্ছ done;
	    }
	    p = চাবি + len;
	}
	অন্যথা
	{
	    // Get the index [expr] or the first index [expr: ].
	    p = skipwhite(p + 1);
	    যদি (*p == ':')
		empty1 = TRUE;
	    অন্যথা
	    {
		empty1 = FALSE;
		যদি (eval1(&p, &var1, &EVALARG_EVALUATE) == FAIL)  // recursive!
		    গচ্ছ done;
		যদি (tv_get_string_chk(&var1) == রিক্ত)
		    // not a number or string
		    গচ্ছ done;
		p = skipwhite(p);
	    }

	    // Optionally get the second index [ :expr].
	    যদি (*p == ':')
	    {
		যদি (v_type == VAR_DICT)
		{
		    যদি (!quiet)
			emsg(_(e_cannot_slice_dictionary));
		    গচ্ছ done;
		}
		যদি (v_type == VAR_TUPLE)
		{
		    যদি (!quiet)
			emsg(_(e_cannot_slice_tuple));
		    গচ্ছ done;
		}
		যদি (rettv != রিক্ত
			&& !(rettv->v_type == VAR_LIST
			    && rettv->vval.v_list != রিক্ত)
			&& !(rettv->v_type == VAR_BLOB
			    && rettv->vval.v_blob != রিক্ত))
		{
		    যদি (!quiet)
			emsg(_(e_slice_requires_list_or_blob_মান));
		    গচ্ছ done;
		}
		p = skipwhite(p + 1);
		যদি (*p == ']')
		    lp->ll_empty2 = TRUE;
		অন্যথা
		{
		    lp->ll_empty2 = FALSE;
		    // recursive!
		    যদি (eval1(&p, &var2, &EVALARG_EVALUATE) == FAIL)
			গচ্ছ done;
		    যদি (tv_get_string_chk(&var2) == রিক্ত)
			// not a number or string
			গচ্ছ done;
		}
		lp->ll_range = TRUE;
	    }
	    অন্যথা
		lp->ll_range = FALSE;

	    যদি (*p != ']')
	    {
		যদি (!quiet)
		    emsg(_(e_missing_closing_square_brace));
		গচ্ছ done;
	    }

	    // Skip to past ']'.
	    ++p;
	}
#ifdef LOG_LOCKVAR
	যদি (len == -1)
	    ch_log(রিক্ত, "LKVAR:    ... loop: p: %s, '[' key: %s", p,
		    empty1 ? ":" : (বর্ণ*)tv_get_string(&var1));
	অন্যথা
	    ch_log(রিক্ত, "LKVAR:    ... loop: p: %s, '.' key: %s", p, চাবি);
#endif

	যদি (v_type == VAR_DICT)
	{
	    glv_status_T glv_status;

	    glv_status = get_lval_dict_item(lp, name, চাবি, len, &p, &var1,
							flags, unlet, rettv);
	    যদি (glv_status == GLV_FAIL)
		গচ্ছ done;
	    যদি (glv_status == GLV_STOP)
		ভঙ্গ;
	}
	অন্যথা যদি (v_type == VAR_BLOB)
	{
	    যদি (get_lval_blob(lp, &var1, &var2, empty1, quiet) == FAIL)
		গচ্ছ done;

	    ভঙ্গ;
	}
	অন্যথা যদি (v_type == VAR_LIST)
	{
	    যদি (get_lval_list(lp, &var1, &var2, empty1, flags, quiet) == FAIL)
		গচ্ছ done;
	}
	অন্যথা যদি (v_type == VAR_TUPLE)
	{
	    যদি (get_lval_tuple(lp, &var1, quiet) == FAIL)
		গচ্ছ done;
	}
	অন্যথা  // v_type == VAR_CLASS || v_type == VAR_OBJECT
	{
	    যদি (get_lval_class_or_obj(lp, চাবি, &p, v_type, cl_exec, flags,
							quiet) == FAIL)
		গচ্ছ done;
	}

	clear_tv(&var1);
	clear_tv(&var2);
	var1.v_type = VAR_UNKNOWN;
	var2.v_type = VAR_UNKNOWN;
    }

    যদি (lp->ll_tuple != রিক্ত && (flags & GLV_READ_ONLY) == 0)
    {
	যদি (!quiet)
	    emsg(_(e_tuple_is_immutable));
	গচ্ছ done;
    }

    rc = OK;

done:
    clear_tv(&var1);
    clear_tv(&var2);
    প্রত্যাগম rc == OK ? p : রিক্ত;
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
    পূর্ণাঙ্ক		unlet,
    পূর্ণাঙ্ক		skip,
    পূর্ণাঙ্ক		flags,	    // GLV_ values
    পূর্ণাঙ্ক		fne_flags)  // flags for find_name_end()
{
    char_u	*p;
    char_u	*রাশিমালা_start, *রাশিমালা_end;
    পূর্ণাঙ্ক		cc;
    dictitem_T	*v = রিক্ত;
    hashtab_T	*ht = রিক্ত;
    পূর্ণাঙ্ক		quiet = flags & GLV_QUIET;
    পূর্ণাঙ্ক		writing = 0;
    পূর্ণাঙ্ক		vim9script = in_vim9script();
    class_T	*cl_exec = রিক্ত;    // class that is executing, or NULL.

#ifdef LOG_LOCKVAR
    যদি (lval_মূল == রিক্ত)
	ch_log(রিক্ত, "LKVAR: get_lval(): name: %s, lval_root (nil)", name);
    অন্যথা
	ch_log(রিক্ত, "LKVAR: get_lval(): name: %s, lr_tv %p lr_is_arg %d",
			name, (শূন্য*)lval_মূল->lr_tv, lval_মূল->lr_is_arg);
    বর্ণ buf[80];
    ch_log(রিক্ত, "LKVAR:    ...: GLV flags: %s",
		    flags_tostring(flags, glv_flag_strings, buf, পরিমাণ(buf)));
#endif

    // Clear everything in "lp".
    CLEAR_POINTER(lp);
    ga_init2(&lp->ll_type_list, পরিমাণ(type_T *), 10);

    যদি (skip || (flags & GLV_COMPILING))
    {
	// When skipping or compiling just find the end of the name.
	lp->ll_name = name;
	lp->ll_name_end = find_name_end(name, রিক্ত, রিক্ত,
						      FNE_INCL_BR | fne_flags);
	প্রত্যাগম lp->ll_name_end;
    }

    // Cannot use "s:var" at the Vim9 script level.  "s: type" is OK.
    যদি (vim9script && at_script_level()
		  && name[0] == 's' && name[1] == ':' && !VIM_ISWHITE(name[2]))
    {
	semsg(_(e_cannot_use_s_colon_in_vim9_script_str), name);
	প্রত্যাগম রিক্ত;
    }

    // Find the end of the name.
    p = find_name_end(name, &রাশিমালা_start, &রাশিমালা_end, fne_flags);
    lp->ll_name_end = p;
    যদি (রাশিমালা_start != রিক্ত)
    {
	// Don't expand the name when we already know there is an error.
	যদি (unlet && !VIM_ISWHITE(*p) && !ends_excmd(*p)
						    && *p != '[' && *p != '.')
	{
	    semsg(_(e_trailing_characters_str), p);
	    প্রত্যাগম রিক্ত;
	}

	lp->ll_exp_name = make_expanded_name(name, রাশিমালা_start, রাশিমালা_end, p);
	যদি (lp->ll_exp_name == রিক্ত)
	{
	    // Report an invalid expression in braces, unless the
	    // expression evaluation has been cancelled due to an
	    // aborting error, an interrupt, or an exception.
	    যদি (!aborting() && !quiet)
	    {
		emsg_severe = TRUE;
		semsg(_(e_invalid_argument_str), name);
		প্রত্যাগম রিক্ত;
	    }
	}
	lp->ll_name = lp->ll_exp_name;
    }
    অন্যথা
    {
	lp->ll_name = name;

	যদি (vim9script)
	{
	    // "a: type" is declaring variable "a" with a type, not "a:".
	    // However, "g:[key]" is indexing a dictionary.
	    যদি (p == name + 2 && p[-1] == ':' && *p != '[')
	    {
		--p;
		lp->ll_name_end = p;
	    }
	    যদি (*skipwhite(p) == ':')
	    {
		char_u	    *tp = skipwhite(p + 1);

		যদি (is_scoped_variable(name))
		{
		    semsg(_(e_cannot_use_type_with_this_variable_str), name);
		    প্রত্যাগম রিক্ত;
		}
		যদি (VIM_ISWHITE(*p))
		{
		    semsg(_(e_no_white_space_allowed_before_colon_str), p);
		    প্রত্যাগম রিক্ত;
		}
		যদি (tp == p + 1 && !quiet)
		{
		    semsg(_(e_white_space_required_after_str_str), ":", p);
		    প্রত্যাগম রিক্ত;
		}
		যদি (!SCRIPT_ID_VALID(current_sctx.sc_sid))
		{
		    semsg(_(e_using_type_not_in_script_context_str), p);
		    প্রত্যাগম রিক্ত;
		}
		যদি (vim9script && (flags & GLV_NO_DECL) &&
			!(flags & GLV_FOR_LOOP))
		{
		    // Using a type and not in a "var" declaration.
		    semsg(_(e_trailing_characters_str), p);
		    প্রত্যাগম রিক্ত;
		}


		// parse the type after the name
		lp->ll_type = parse_type(&tp,
			       &SCRIPT_ITEM(current_sctx.sc_sid)->sn_type_list,
			       রিক্ত, রিক্ত, !quiet);
		যদি (!quiet && (lp->ll_type == রিক্ত
			    || !valid_declaration_type(lp->ll_type)))
		    প্রত্যাগম রিক্ত;
		lp->ll_name_end = tp;
	    }
	    // TODO: check inside class?
	}
    }
    যদি (lp->ll_name == রিক্ত)
	প্রত্যাগম p;

    যদি (*p == '.')
    {
	// In legacy script, when a local variable and import exists with this name,
	// prioritize local variable over imports to avoid conflicts.
	পূর্ণাঙ্ক var_exists = FALSE;
	যদি (!vim9script)
	{
	    cc = *p;
	    *p = NUL;
	    hashtab_T *local_ht = get_funccal_local_ht();
	    যদি (local_ht != রিক্ত)
	    {
		hashitem_T *hi = hash_find(local_ht, lp->ll_name);
		যদি (!HASHITEM_EMPTY(hi))
		    var_exists = TRUE;
	    }
	    *p = cc;
	}

	যদি (!var_exists)
	{
	    imported_T *import = find_imported(lp->ll_name, p - lp->ll_name, TRUE);
	    যদি (import != রিক্ত)
	    {
		p++;	// skip '.'
		p = get_lval_imported(lp, import->imp_sid, p, &v, fne_flags);
		যদি (p == রিক্ত)
		    প্রত্যাগম রিক্ত;
	    }
	}
    }

    // Without [idx] or .key we are done.
    যদি (*p != '[' && *p != '.')
    {
	যদি (lval_মূল != রিক্ত)
	    fill_lval_from_lval_মূল(lp, lval_মূল);
	প্রত্যাগম p;
    }

    যদি (vim9script && lval_মূল != রিক্ত)
	cl_exec = lval_মূল->lr_cl_exec;
    যদি (vim9script && lval_মূল != রিক্ত && lval_মূল->lr_tv != রিক্ত)
    {
	// using local variable
	lp->ll_tv = lval_মূল->lr_tv;
	v = রিক্ত;
    }
    অন্যথা যদি (lp->ll_tv == রিক্ত)
    {
	cc = *p;
	*p = NUL;
	// When we would write to the variable pass &ht and prevent autoload.
	writing = !(flags & GLV_READ_ONLY);
	v = find_var(lp->ll_name, writing ? &ht : রিক্ত,
					 (flags & GLV_NO_AUTOLOAD) || writing);
	যদি (v == রিক্ত && !quiet)
	    semsg(_(e_undefined_variable_str), lp->ll_name);
	*p = cc;
	যদি (v == রিক্ত)
	    প্রত্যাগম রিক্ত;
	lp->ll_tv = &v->di_tv;
    }

    যদি (vim9script && (flags & GLV_NO_DECL) == 0)
    {
	যদি (!quiet)
	    semsg(_(e_variable_already_declared_str), lp->ll_name);
	প্রত্যাগম রিক্ত;
    }

    // If the next character is a "." or a "[", then process the subitem.
    p = get_lval_subscript(lp, p, name, rettv, ht, v, unlet, flags, cl_exec);
    যদি (p == রিক্ত)
	প্রত্যাগম রিক্ত;

    যদি (vim9script && lp->ll_valtype != রিক্ত && rettv != রিক্ত)
    {
	where_T	    where = WHERE_INIT;

	// In a Vim9 script, do type check and make sure the variable is
	// writable.
	যদি (check_typval_type(lp->ll_valtype, rettv, where) == FAIL)
	    প্রত্যাগম রিক্ত;
    }

    lp->ll_name_end = p;
    প্রত্যাগম p;
}

/*
 * Clear lval "lp" that was filled by get_lval().
 */
    শূন্য
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
    শূন্য
set_var_lval(
    lval_T	*lp,
    char_u	*endp,
    typval_T	*rettv,
    পূর্ণাঙ্ক		copy,
    পূর্ণাঙ্ক		flags,	    // ASSIGN_CONST, ASSIGN_NO_DECL
    char_u	*op,
    পূর্ণাঙ্ক		var_idx)    // index for "let [a, b] = list"
{
    পূর্ণাঙ্ক		cc;
    dictitem_T	*di;

    যদি (lp->ll_tv == রিক্ত)
    {
	cc = *endp;
	*endp = NUL;
	যদি (in_vim9script() && check_reserved_name(lp->ll_name, FALSE) == FAIL)
	    প্রত্যাগম;

	যদি (lp->ll_blob != রিক্ত)
	{
	    পূর্ণাঙ্ক	    error = FALSE;

	    যদি (op != রিক্ত && *op != '=')
	    {
		semsg(_(e_wrong_variable_type_for_str_equal), op);
		প্রত্যাগম;
	    }
	    যদি (মান_check_lock(lp->ll_blob->bv_lock, lp->ll_name, FALSE))
		প্রত্যাগম;

	    যদি (lp->ll_range && rettv->v_type == VAR_BLOB)
	    {
		যদি (lp->ll_empty2)
		    lp->ll_n2 = blob_len(lp->ll_blob) - 1;

		যদি (blob_set_range(lp->ll_blob, lp->ll_n1, lp->ll_n2,
								rettv) == FAIL)
		    প্রত্যাগম;
	    }
	    অন্যথা
	    {
		varnumber_T	val = tv_get_সংখ্যা_chk(rettv, &error);
		যদি (!error)
		{
		    যদি (val < 0 || val > 255)
			semsg(_(e_invalid_মান_for_blob_nr), val);
		    অন্যথা
			blob_set_append(lp->ll_blob, lp->ll_n1, val);
		}
	    }
	}
	অন্যথা যদি (op != রিক্ত && *op != '=')
	{
	    typval_T tv;

	    যদি ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					     && (flags & ASSIGN_FOR_LOOP) == 0)
	    {
		emsg(_(e_cannot_modify_existing_variable));
		*endp = cc;
		প্রত্যাগম;
	    }

	    // handle +=, -=, *=, /=, %= and .=
	    di = রিক্ত;
	    যদি (eval_variable(lp->ll_name, (পূর্ণাঙ্ক)STRLEN(lp->ll_name),
				 lp->ll_sid, &tv, &di, EVAL_VAR_VERBOSE) == OK)
	    {
		যদি (di != রিক্ত && check_typval_is_মান(&di->di_tv) == FAIL)
		{
		    clear_tv(&tv);
		    প্রত্যাগম;
		}

		// In Vim9 script a String v: variable only takes a String.
		যদি (in_vim9script() && STRNCMP(lp->ll_name, "v:", 2) == 0
			&& tv.v_type == VAR_STRING)
		{
		    where_T	where = WHERE_INIT;

		    যদি (check_typval_type(&t_string, rettv, where) == FAIL)
		    {
			clear_tv(&tv);
			*endp = cc;
			প্রত্যাগম;
		    }
		}

		যদি ((di == রিক্ত
			 || (!var_check_ro(di->di_flags, lp->ll_name, FALSE)
			   && !tv_check_lock(&di->di_tv, lp->ll_name, FALSE)))
			&& tv_op(&tv, rettv, op) == OK)
		    set_var_const(lp->ll_name, lp->ll_sid, রিক্ত, &tv, FALSE,
				ASSIGN_NO_DECL | ASSIGN_COMPOUND_OP, 0);
		clear_tv(&tv);
	    }
	}
	অন্যথা
	{
	    যদি (lp->ll_type != রিক্ত && check_typval_arg_type(lp->ll_type, rettv,
							      রিক্ত, 0) == FAIL)
		প্রত্যাগম;
	    set_var_const(lp->ll_name, lp->ll_sid, lp->ll_type, rettv, copy,
							       flags, var_idx);
	}
	*endp = cc;
    }
    অন্যথা যদি (মান_check_lock(lp->ll_newkey == রিক্ত
		? lp->ll_tv->v_lock
		: lp->ll_tv->vval.v_dict->dv_lock, lp->ll_name, FALSE))
	;
    অন্যথা যদি (lp->ll_range)
    {
	যদি ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					     && (flags & ASSIGN_FOR_LOOP) == 0)
	{
	    emsg(_(e_cannot_lock_range));
	    প্রত্যাগম;
	}

	(শূন্য)list_assign_range(lp->ll_list, rettv->vval.v_list,
			 lp->ll_n1, lp->ll_n2, lp->ll_empty2, op, lp->ll_name);
    }
    অন্যথা
    {
	/*
	 * Assign to a List, Dictionary or Object item.
	 */
	যদি ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					     && (flags & ASSIGN_FOR_LOOP) == 0)
	{
	    emsg(_(e_cannot_lock_list_or_dict));
	    প্রত্যাগম;
	}

	যদি (lp->ll_valtype != রিক্ত
		    && check_typval_arg_type(lp->ll_valtype, rettv,
							      রিক্ত, 0) == FAIL)
	{
	    lp->ll_name_end = রিক্ত;
	    প্রত্যাগম;
	}

	// If the lval is a List and the type of the list is not yet set,
	// then set the item type from the declared type of the variable.
	যদি (in_vim9script() && rettv->v_type == VAR_LIST
				&& rettv->vval.v_list != রিক্ত
				&& rettv->vval.v_list->lv_type == রিক্ত)
	{
	    যদি (lp->ll_tv->v_type == VAR_LIST
		    && lp->ll_tv->vval.v_list != রিক্ত
		    && lp->ll_tv->vval.v_list->lv_type != রিক্ত)
		set_tv_type(rettv, lp->ll_tv->vval.v_list->lv_type);
	}

	যদি (lp->ll_newkey != রিক্ত)
	{
	    যদি (op != রিক্ত && *op != '=')
	    {
		semsg(_(e_চাবি_not_present_in_dictionary_str), lp->ll_newkey);
		প্রত্যাগম;
	    }
	    যদি (dict_wrong_func_name(lp->ll_tv->vval.v_dict, rettv,
								lp->ll_newkey))
		প্রত্যাগম;

	    // Need to add an item to the Dictionary.
	    di = dictitem_alloc(lp->ll_newkey);
	    যদি (di == রিক্ত)
		প্রত্যাগম;
	    যদি (dict_add(lp->ll_tv->vval.v_dict, di) == FAIL)
	    {
		vim_free(di);
		প্রত্যাগম;
	    }
	    lp->ll_tv = &di->di_tv;
	}
	অন্যথা যদি (op != রিক্ত && *op != '=')
	{
	    tv_op(lp->ll_tv, rettv, op);
	    প্রত্যাগম;
	}
	অন্যথা
	    clear_tv(lp->ll_tv);

	/*
	 * Assign the value to the variable or list item.
	 */
	যদি (copy)
	    copy_tv(rettv, lp->ll_tv);
	অন্যথা
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
    স্থির পূর্ণাঙ্ক
tv_op_blob(typval_T *tv1, typval_T *tv2, char_u *op)
{
    যদি (*op != '+' || tv2->v_type != VAR_BLOB)
	প্রত্যাগম FAIL;

    // Blob += Blob
    যদি (tv2->vval.v_blob == রিক্ত)
	প্রত্যাগম OK;

    যদি (tv1->vval.v_blob == রিক্ত)
    {
	tv1->vval.v_blob = tv2->vval.v_blob;
	++tv1->vval.v_blob->bv_refcount;
	প্রত্যাগম OK;
    }

    blob_T	*b1 = tv1->vval.v_blob;
    blob_T	*b2 = tv2->vval.v_blob;
    পূর্ণাঙ্ক		len = blob_len(b2);

    যদি (len > 0 && ga_grow(&b1->bv_ga, len) == OK)
    {
	mch_memmove((char_u *)b1->bv_ga.ga_data + b1->bv_ga.ga_len,
		    (char_u *)b2->bv_ga.ga_data, (size_t)len);
	b1->bv_ga.ga_len += len;
    }
    অন্যথা
	প্রতি (পূর্ণাঙ্ক i = 0; i < len; i++)
	    (শূন্য)ga_append(&b1->bv_ga, blob_get(b2, i));

    প্রত্যাগম OK;
}

/*
 * Handle "list1 += list2".
 * Returns OK or FAIL.
 */
    স্থির পূর্ণাঙ্ক
tv_op_list(typval_T *tv1, typval_T *tv2, char_u *op)
{
    যদি (*op != '+' || tv2->v_type != VAR_LIST)
	প্রত্যাগম FAIL;

    // List += List
    যদি (tv2->vval.v_list == রিক্ত)
	প্রত্যাগম OK;

    যদি (tv1->vval.v_list == রিক্ত)
    {
	tv1->vval.v_list = tv2->vval.v_list;
	++tv1->vval.v_list->lv_refcount;
    }
    অন্যথা
	list_extend(tv1->vval.v_list, tv2->vval.v_list, রিক্ত);

    প্রত্যাগম OK;
}

/*
 * Handle number operations:
 *	nr += nr , nr -= nr , nr *=nr , nr /= nr , nr %= nr
 *
 * Returns OK or FAIL.
 */
    স্থির পূর্ণাঙ্ক
tv_op_সংখ্যা(typval_T *tv1, typval_T *tv2, char_u *op)
{
    varnumber_T	n;
    পূর্ণাঙ্ক		failed = FALSE;

    n = tv_get_সংখ্যা(tv1);
    যদি (tv2->v_type == VAR_FLOAT)
    {
	float_T f = n;

	যদি (*op == '%')
	    প্রত্যাগম FAIL;
	বিকল্প (*op)
	{
	    পক্ষ '+': f += tv2->vval.v_float; ভঙ্গ;
	    পক্ষ '-': f -= tv2->vval.v_float; ভঙ্গ;
	    পক্ষ '*': f *= tv2->vval.v_float; ভঙ্গ;
	    পক্ষ '/': f /= tv2->vval.v_float; ভঙ্গ;
	}
	clear_tv(tv1);
	tv1->v_type = VAR_FLOAT;
	tv1->vval.v_float = f;
    }
    অন্যথা
    {
	বিকল্প (*op)
	{
	    পক্ষ '+': n += tv_get_সংখ্যা(tv2); ভঙ্গ;
	    পক্ষ '-': n -= tv_get_সংখ্যা(tv2); ভঙ্গ;
	    পক্ষ '*': n *= tv_get_সংখ্যা(tv2); ভঙ্গ;
	    পক্ষ '/': n = অঙ্ক_divide(n, tv_get_সংখ্যা(tv2), &failed); ভঙ্গ;
	    পক্ষ '%': n = অঙ্ক_modulus(n, tv_get_সংখ্যা(tv2), &failed); ভঙ্গ;
	}
	clear_tv(tv1);
	tv1->v_type = VAR_সংখ্যা;
	tv1->vval.v_সংখ্যা = n;
    }

    প্রত্যাগম failed ? FAIL : OK;
}

/*
 * Append string "s2" to the string in "tv1".
 * Returns OK if "tv1" was grown in place, FAIL otherwise.
 */
    স্থির পূর্ণাঙ্ক
grow_string_tv(typval_T *tv1, char_u *s2)
{
    যদি (tv1->v_type != VAR_STRING || tv1->vval.v_string == রিক্ত)
	প্রত্যাগম FAIL;

    size_t len1 = STRLEN(tv1->vval.v_string);
    size_t len2 = STRLEN(s2);
    char_u *p = vim_realloc(tv1->vval.v_string, len1 + len2 + 1);

    যদি (p == রিক্ত)
	প্রত্যাগম FAIL;

    mch_memmove(p + len1, s2, len2 + 1);
    tv1->vval.v_string = p;
    প্রত্যাগম OK;
}

/*
 * Handle "str1 .= str2"
 * Returns OK or FAIL.
 */
    স্থির পূর্ণাঙ্ক
tv_op_string(typval_T *tv1, typval_T *tv2, char_u *op UNUSED)
{
    char_u	numbuf[NUMBUFLEN];
    char_u	*s;
    char_u	*s2;

    যদি (tv2->v_type == VAR_FLOAT)
	প্রত্যাগম FAIL;

    // str .= str
    s2 = tv_get_string_buf(tv2, numbuf);
    যদি (grow_string_tv(tv1, s2) == OK)
	প্রত্যাগম OK;

    s = tv_get_string(tv1);
    s = concat_str(s, s2);
    clear_tv(tv1);
    tv1->v_type = VAR_STRING;
    tv1->vval.v_string = s;

    প্রত্যাগম OK;
}

/*
 * Handle "tv1 += tv2", "tv1 -= tv2", "tv1 *= tv2", "tv1 /= tv2", "tv1 %= tv2"
 * and "tv1 .= tv2"
 * Returns OK or FAIL.
 */
    স্থির পূর্ণাঙ্ক
tv_op_nr_or_string(typval_T *tv1, typval_T *tv2, char_u *op)
{
    যদি (tv2->v_type == VAR_LIST)
	প্রত্যাগম FAIL;

    যদি (vim_strchr((char_u *)"+-*/%", *op) != রিক্ত)
	প্রত্যাগম tv_op_সংখ্যা(tv1, tv2, op);

    প্রত্যাগম tv_op_string(tv1, tv2, op);
}

/*
 * Handle "f1 += f2", "f1 -= f2", "f1 *= f2", "f1 /= f2".
 * Returns OK or FAIL.
 */
    স্থির পূর্ণাঙ্ক
tv_op_float(typval_T *tv1, typval_T *tv2, char_u *op)
{
    float_T f;

    যদি (*op == '%' || *op == '.'
	    || (tv2->v_type != VAR_FLOAT
		&& tv2->v_type != VAR_সংখ্যা
		&& tv2->v_type != VAR_STRING))
	প্রত্যাগম FAIL;

    যদি (tv2->v_type == VAR_FLOAT)
	f = tv2->vval.v_float;
    অন্যথা
	f = tv_get_সংখ্যা(tv2);
    বিকল্প (*op)
    {
	পক্ষ '+': tv1->vval.v_float += f; ভঙ্গ;
	পক্ষ '-': tv1->vval.v_float -= f; ভঙ্গ;
	পক্ষ '*': tv1->vval.v_float *= f; ভঙ্গ;
	পক্ষ '/': tv1->vval.v_float /= f; ভঙ্গ;
    }

    প্রত্যাগম OK;
}

/*
 * Handle "tv1 += tv2", "tv1 -= tv2", "tv1 *= tv2", "tv1 /= tv2", "tv1 %= tv2"
 * and "tv1 .= tv2"
 * Returns OK or FAIL.
 */
    পূর্ণাঙ্ক
tv_op(typval_T *tv1, typval_T *tv2, char_u *op)
{
    // Can't do anything with a Funcref or Dict on the right.
    // v:true and friends only work with "..=".
    যদি (tv2->v_type == VAR_FUNC || tv2->v_type == VAR_DICT
		|| ((tv2->v_type == VAR_BOOL || tv2->v_type == VAR_SPECIAL)
							&& *op != '.'))
    {
	semsg(_(e_wrong_variable_type_for_str_equal), op);
	প্রত্যাগম FAIL;
    }

    পূর্ণাঙ্ক retval = FAIL;
    বিকল্প (tv1->v_type)
    {
	পক্ষ VAR_UNKNOWN:
	পক্ষ VAR_ANY:
	পক্ষ VAR_VOID:
	পক্ষ VAR_DICT:
	পক্ষ VAR_FUNC:
	পক্ষ VAR_PARTIAL:
	পক্ষ VAR_BOOL:
	পক্ষ VAR_SPECIAL:
	পক্ষ VAR_JOB:
	পক্ষ VAR_CHANNEL:
	পক্ষ VAR_INSTR:
	পক্ষ VAR_OBJECT:
	পক্ষ VAR_CLASS:
	পক্ষ VAR_TYPEALIAS:
	পক্ষ VAR_TUPLE:
	    ভঙ্গ;

	পক্ষ VAR_BLOB:
	    retval = tv_op_blob(tv1, tv2, op);
	    ভঙ্গ;

	পক্ষ VAR_LIST:
	    retval = tv_op_list(tv1, tv2, op);
	    ভঙ্গ;

	পক্ষ VAR_সংখ্যা:
	পক্ষ VAR_STRING:
	    retval = tv_op_nr_or_string(tv1, tv2, op);
	    ভঙ্গ;

	পক্ষ VAR_FLOAT:
	    retval = tv_op_float(tv1, tv2, op);
	    ভঙ্গ;
    }

    যদি (retval != OK)
	semsg(_(e_wrong_variable_type_for_str_equal), op);

    প্রত্যাগম retval;
}

/*
 * Evaluate the expression used in a ":for var in expr" command.
 * "arg" points to "var".
 * Set "*errp" to TRUE for an error, FALSE otherwise;
 * Return a pointer that holds the info.  Null when there is an error.
 */
    শূন্য *
eval_for_সারি(
    char_u	*arg,
    পূর্ণাঙ্ক		*errp,
    exarg_T	*eap,
    evalarg_T	*evalarg)
{
    forinfo_T	*fi;
    char_u	*var_list_end;
    char_u	*রাশিমালা;
    typval_T	tv;
    list_T	*l;
    tuple_T	*tuple;
    পূর্ণাঙ্ক		skip = !(evalarg->eval_flags & EVAL_EVALUATE);

    *errp = TRUE;	// default: there is an error

    fi = ALLOC_CLEAR_ONE(forinfo_T);
    যদি (fi == রিক্ত)
	প্রত্যাগম রিক্ত;

    var_list_end = skip_var_list(arg, TRUE, &fi->fi_varcount,
						     &fi->fi_semicolon, FALSE);
    যদি (var_list_end == রিক্ত)
	প্রত্যাগম fi;

    রাশিমালা = skipwhite_and_linebreak(var_list_end, evalarg);
    যদি (রাশিমালা[0] != 'i' || রাশিমালা[1] != 'n'
				  || !(রাশিমালা[2] == NUL || VIM_ISWHITE(রাশিমালা[2])))
    {
	যদি (in_vim9script() && *রাশিমালা == ':' && রাশিমালা != var_list_end)
	    semsg(_(e_no_white_space_allowed_before_colon_str), রাশিমালা);
	অন্যথা
	    emsg(_(e_missing_in_after_for));
	প্রত্যাগম fi;
    }

    যদি (skip)
	++emsg_skip;
    রাশিমালা = skipwhite_and_linebreak(রাশিমালা + 2, evalarg);
    যদি (eval0(রাশিমালা, &tv, eap, evalarg) == OK)
    {
	*errp = FALSE;
	যদি (!skip)
	{
	    যদি (tv.v_type == VAR_LIST)
	    {
		l = tv.vval.v_list;
		যদি (l == রিক্ত)
		{
		    // a null list is like an empty list: do nothing
		    clear_tv(&tv);
		}
		অন্যথা
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
	    অন্যথা যদি (tv.v_type == VAR_TUPLE)
	    {
		tuple = tv.vval.v_tuple;
		যদি (tuple == রিক্ত)
		{
		    // a null tuple is like an empty tuple: do nothing
		    clear_tv(&tv);
		}
		অন্যথা
		{
		    // No need to increment the refcount, it's already set for
		    // the tuple being used in "tv".
		    fi->fi_tuple = tuple;
		    fi->fi_tuple_idx = 0;
		}
	    }
	    অন্যথা যদি (tv.v_type == VAR_BLOB)
	    {
		fi->fi_bi = 0;
		যদি (tv.vval.v_blob != রিক্ত)
		{
		    // Make a copy, so that the iteration still works when the
		    // blob is changed.
		    fi->fi_blob = blob_copy(tv.vval.v_blob);
		}
		clear_tv(&tv);
	    }
	    অন্যথা যদি (tv.v_type == VAR_STRING)
	    {
		fi->fi_byte_idx = 0;
		fi->fi_string = tv.vval.v_string;
		tv.vval.v_string = রিক্ত;
		যদি (fi->fi_string == রিক্ত)
		    fi->fi_string = vim_strsave((char_u *)"");
	    }
	    অন্যথা
	    {
		emsg(_(e_string_list_tuple_or_blob_required));
		clear_tv(&tv);
	    }
	}
    }
    যদি (skip)
	--emsg_skip;
    fi->fi_break_গুনতি = evalarg->eval_break_গুনতি;

    প্রত্যাগম fi;
}

/*
 * Used when looping over a :for line, skip the "in expr" part.
 */
    শূন্য
skip_for_সারিগুলি(শূন্য *fi_void, evalarg_T *evalarg)
{
    forinfo_T	*fi = (forinfo_T *)fi_void;
    পূর্ণাঙ্ক		i;

    প্রতি (i = 0; i < fi->fi_break_গুনতি; ++i)
	eval_next_সারি(রিক্ত, evalarg);
}

/*
 * Use the first item in a ":for" list.  Advance to the next.
 * Assign the values to the variable (list).  "arg" points to the first one.
 * Return TRUE when a valid item was found, FALSE when at end of list or
 * something wrong.
 */
    পূর্ণাঙ্ক
next_for_item(শূন্য *fi_void, char_u *arg)
{
    forinfo_T	*fi = (forinfo_T *)fi_void;
    পূর্ণাঙ্ক		result;
    পূর্ণাঙ্ক		flag = ASSIGN_FOR_LOOP | (in_vim9script()
			 ? (ASSIGN_FINAL
			     // first round: error if variable exists
			     | (fi->fi_bi == 0 ? 0 : ASSIGN_DECL)
			     | ASSIGN_NO_MEMBER_TYPE
			     | ASSIGN_UPDATE_BLOCK_ID)
			 : 0);
    listitem_T	*item;
    পূর্ণাঙ্ক		skip_assign = in_vim9script() && arg[0] == '_'
						      && !eval_isnamec(arg[1]);

    যদি (fi->fi_blob != রিক্ত)
    {
	typval_T	tv;

	যদি (fi->fi_bi >= blob_len(fi->fi_blob))
	    প্রত্যাগম FALSE;
	tv.v_type = VAR_সংখ্যা;
	tv.v_lock = VAR_FIXED;
	tv.vval.v_সংখ্যা = blob_get(fi->fi_blob, fi->fi_bi);
	++fi->fi_bi;
	যদি (skip_assign)
	    প্রত্যাগম TRUE;
	প্রত্যাগম ex_let_vars(arg, &tv, TRUE, fi->fi_semicolon,
					    fi->fi_varcount, flag, রিক্ত) == OK;
    }

    যদি (fi->fi_string != রিক্ত)
    {
	typval_T	tv;
	পূর্ণাঙ্ক		len;

	len = mb_ptr2len(fi->fi_string + fi->fi_byte_idx);
	যদি (len == 0)
	    প্রত্যাগম FALSE;
	tv.v_type = VAR_STRING;
	tv.v_lock = VAR_FIXED;
	tv.vval.v_string = vim_strnsave(fi->fi_string + fi->fi_byte_idx, len);
	fi->fi_byte_idx += len;
	++fi->fi_bi;
	যদি (skip_assign)
	    result = TRUE;
	অন্যথা
	    result = ex_let_vars(arg, &tv, TRUE, fi->fi_semicolon,
					    fi->fi_varcount, flag, রিক্ত) == OK;
	vim_free(tv.vval.v_string);
	প্রত্যাগম result;
    }

    যদি (fi->fi_tuple != রিক্ত)
    {
	typval_T	tv;

	যদি (fi->fi_tuple_idx >= TUPLE_LEN(fi->fi_tuple))
	    প্রত্যাগম FALSE;

	copy_tv(TUPLE_ITEM(fi->fi_tuple, fi->fi_tuple_idx), &tv);
	++fi->fi_tuple_idx;
	++fi->fi_bi;
	যদি (skip_assign)
	    result = TRUE;
	অন্যথা
	    result = ex_let_vars(arg, &tv, TRUE, fi->fi_semicolon,
					    fi->fi_varcount, flag, রিক্ত) == OK;
	clear_tv(&tv);
	প্রত্যাগম result;
    }

    item = fi->fi_lw.lw_item;
    যদি (item == রিক্ত)
	result = FALSE;
    অন্যথা
    {
	fi->fi_lw.lw_item = item->li_next;
	++fi->fi_bi;
	যদি (skip_assign)
	    result = TRUE;
	অন্যথা
	    result = (ex_let_vars(arg, &item->li_tv, TRUE, fi->fi_semicolon,
					   fi->fi_varcount, flag, রিক্ত) == OK);
    }
    প্রত্যাগম result;
}

/*
 * Free the structure used to store info used by ":for".
 */
    শূন্য
free_for_info(শূন্য *fi_void)
{
    forinfo_T    *fi = (forinfo_T *)fi_void;

    যদি (fi == রিক্ত)
	প্রত্যাগম;
    যদি (fi->fi_list != রিক্ত)
    {
	list_rem_watch(fi->fi_list, &fi->fi_lw);
	list_unref(fi->fi_list);
    }
    অন্যথা যদি (fi->fi_blob != রিক্ত)
	blob_unref(fi->fi_blob);
    অন্যথা যদি (fi->fi_tuple != রিক্ত)
	tuple_unref(fi->fi_tuple);
    অন্যথা
	vim_free(fi->fi_string);
    vim_free(fi);
}

    শূন্য
set_context_for_expression(
    expand_T	*xp,
    char_u	*arg,
    cmdidx_T	cmdidx)
{
    পূর্ণাঙ্ক		has_রাশিমালা = cmdidx != CMD_let && cmdidx != CMD_var;
    পূর্ণাঙ্ক		c;
    char_u	*p;

    যদি (cmdidx == CMD_let || cmdidx == CMD_var
				 || cmdidx == CMD_const || cmdidx == CMD_final)
    {
	xp->xp_context = EXPAND_USER_VARS;
	যদি (vim_strpbrk(arg, (char_u *)"\"'+-*/%.=!?~|&$([<>,#") == রিক্ত)
	{
	    // ":let var1 var2 ...": find last space.
	    প্রতি (p = arg + STRLEN(arg); p >= arg; )
	    {
		xp->xp_pattern = p;
		MB_PTR_BACK(arg, p);
		যদি (VIM_ISWHITE(*p))
		    ভঙ্গ;
	    }
	    প্রত্যাগম;
	}
    }
    অন্যথা
	xp->xp_context = cmdidx == CMD_call ? EXPAND_FUNCTIONS
							  : EXPAND_EXPRESSION;
    যাবত্ ((xp->xp_pattern = vim_strpbrk(arg,
				  (char_u *)"\"'+-*/%.=!?~|&$([<>,#")) != রিক্ত)
    {
	c = *xp->xp_pattern;
	যদি (c == '&')
	{
	    c = xp->xp_pattern[1];
	    যদি (c == '&')
	    {
		++xp->xp_pattern;
		xp->xp_context = has_রাশিমালা ? EXPAND_EXPRESSION : EXPAND_NOTHING;
	    }
	    অন্যথা যদি (c != ' ')
	    {
		xp->xp_context = EXPAND_SETTINGS;
		যদি ((c == 'l' || c == 'g') && xp->xp_pattern[2] == ':')
		    xp->xp_pattern += 2;

	    }
	}
	অন্যথা যদি (c == '$')
	{
	    // environment variable
	    xp->xp_context = EXPAND_ENV_VARS;
	}
	অন্যথা যদি (c == '=')
	{
	    has_রাশিমালা = TRUE;
	    xp->xp_context = EXPAND_EXPRESSION;
	}
	অন্যথা যদি (c == '#'
		&& xp->xp_context == EXPAND_EXPRESSION)
	{
	    // Autoload function/variable contains '#'.
	    ভঙ্গ;
	}
	অন্যথা যদি ((c == '<' || c == '#')
		&& xp->xp_context == EXPAND_FUNCTIONS
		&& vim_strchr(xp->xp_pattern, '(') == রিক্ত)
	{
	    // Function name can start with "<SNR>" and contain '#'.
	    ভঙ্গ;
	}
	অন্যথা যদি (has_রাশিমালা)
	{
	    যদি (c == '"')	    // string
	    {
		যাবত্ ((c = *++xp->xp_pattern) != NUL && c != '"')
		    যদি (c == '\\' && xp->xp_pattern[1] != NUL)
			++xp->xp_pattern;
		xp->xp_context = EXPAND_NOTHING;
	    }
	    অন্যথা যদি (c == '\'')	    // literal string
	    {
		// Trick: '' is like stopping and starting a literal string.
		যাবত্ ((c = *++xp->xp_pattern) != NUL && c != '\'')
		    /* skip */ ;
		xp->xp_context = EXPAND_NOTHING;
	    }
	    অন্যথা যদি (c == '|')
	    {
		যদি (xp->xp_pattern[1] == '|')
		{
		    ++xp->xp_pattern;
		    xp->xp_context = EXPAND_EXPRESSION;
		}
		অন্যথা
		    xp->xp_context = EXPAND_COMMANDS;
	    }
	    অন্যথা
		xp->xp_context = EXPAND_EXPRESSION;
	}
	অন্যথা
	    // Doesn't look like something valid, expand as an expression
	    // anyway.
	    xp->xp_context = EXPAND_EXPRESSION;
	arg = xp->xp_pattern;
	যদি (*arg != NUL)
	    যাবত্ ((c = *++arg) != NUL && (c == ' ' || c == '\t'))
		/* skip */ ;
    }

    // ":exe one two" completes "two"
    যদি ((cmdidx == CMD_execute
		|| cmdidx == CMD_echo
		|| cmdidx == CMD_echon
		|| cmdidx == CMD_echomsg
		|| cmdidx == CMD_echoerr
		|| cmdidx == CMD_echoconsole
		|| cmdidx == CMD_echowindow)
	    && xp->xp_context == EXPAND_EXPRESSION)
    {
	প্রতি (;;)
	{
	    char_u *n = skiptowhite(arg);

	    যদি (n == arg || IS_WHITE_OR_NUL(*skipwhite(n)))
		ভঙ্গ;
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
স্থির regprog_T    *eval_prog_cache = রিক্ত;
স্থির char_u	    *eval_prog_pat = রিক্ত;	// pattern it was compiled for
স্থির char_u	    *eval_prog_enc = রিক্ত;	// 'encoding' when compiled
স্থির long	    eval_prog_re;		// 'regexpengine' when compiled
// State sampled by eval_regcomp() for the eval_regfree() call that gives the
// program back: user code run in between, e.g. an object's string() method
// invoked by match() on a list, can change these options, and then the
// program must not be cached under the changed values.  eval_compile_enc is
// only ever compared against p_enc, never dereferenced.
স্থির long	    eval_compile_re;
স্থির char_u	    *eval_compile_enc;
স্থির পূর্ণাঙ্ক	    eval_prog_busy = 0;	// programs eval_regcomp() handed out
					// that were not given back yet
স্থির পূর্ণাঙ্ক	    eval_prog_reentered = FALSE;  // eval_regcomp() ran while
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
    স্থির bool
eval_prog_volatile(char_u *pat)
{
    char_u *p;

    যদি (STRNCMP(pat, "\\%#=", 4) == 0)
	প্রত্যাগম true;
    প্রতি (p = pat; *p != NUL; ++p)
	যদি (*p == '~'
		|| (p[0] == '[' && (p[1] == ':' || p[1] == '=' || p[1] == '.'))
		|| (p[0] == '%' && (p[1] == '.'
			|| ((p[1] == '<' || p[1] == '>') && p[2] == '.'))))
	    প্রত্যাগম true;
    প্রত্যাগম false;
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
    যদি (eval_prog_busy > 0)
	// The samples above no longer describe the program already handed
	// out: its eval_regfree() must drop it instead of caching it.
	eval_prog_reentered = TRUE;

    যদি (eval_prog_cache != রিক্ত
	    && *p_cpo == NUL
	    && eval_prog_cache->re_flags == RE_MAGIC + RE_STRING
	    && eval_prog_re == p_re
	    && STRCMP(eval_prog_pat, pat) == 0
	    && STRCMP(eval_prog_enc, p_enc) == 0)
    {
	prog = eval_prog_cache;
	// The caller now owns the program, eval_regfree() adopts the
	// program the caller ends up with.
	eval_prog_cache = রিক্ত;
    }
    অন্যথা
	prog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
    যদি (prog != রিক্ত)
	++eval_prog_busy;	// paired with the eval_regfree() call
    প্রত্যাগম prog;
}

/*
 * Free program "prog", obtained with eval_regcomp() for pattern "pat", by
 * keeping it in the cache for the next use.  "pat" must be the same string
 * the program was compiled for and must still be valid.
 */
    শূন্য
eval_regfree(char_u *pat, regprog_T *prog)
{
    পূর্ণাঙ্ক	    reentered = eval_prog_reentered;

    যদি (eval_prog_busy > 0 && --eval_prog_busy == 0)
	eval_prog_reentered = FALSE;
    যদি (prog == রিক্ত)
	প্রত্যাগম;
    যদি (reentered || *p_cpo != NUL
	    || eval_compile_re != p_re || eval_compile_enc != p_enc)
    {
	// 'cpoptions' is not empty, an option in the cache key changed, or
	// another program was handed out while the caller was using this
	// one: the samples may not describe this program, do not cache it.
	vim_regfree(prog);
	প্রত্যাগম;
    }
    যদি (eval_prog_pat == রিক্ত || STRCMP(eval_prog_pat, pat) != 0
	    || STRCMP(eval_prog_enc, p_enc) != 0)
    {
	char_u	*pat_copy;
	char_u	*enc_copy;

	// A cached pattern is known not to be volatile, thus the scan is
	// only needed when installing a new key.
	যদি (eval_prog_volatile(pat))
	{
	    // compiling depends on state the cache key does not cover
	    vim_regfree(prog);
	    প্রত্যাগম;
	}
	pat_copy = vim_strsave(pat);
	enc_copy = vim_strsave(p_enc);
	যদি (pat_copy == রিক্ত || enc_copy == রিক্ত)
	{
	    vim_free(pat_copy);
	    vim_free(enc_copy);
	    vim_regfree(prog);
	    প্রত্যাগম;
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

#যদি defined(EXITFREE)
    শূন্য
free_eval_regcomp_cache(শূন্য)
{
    vim_regfree(eval_prog_cache);
    eval_prog_cache = রিক্ত;
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
    পূর্ণাঙ্ক
pattern_match(char_u *pat, char_u *text, পূর্ণাঙ্ক ic)
{
    পূর্ণাঙ্ক		matches = FALSE;
    char_u	*save_cpo;
    regmatch_T	regmatch;

    // avoid 'l' flag in 'cpoptions'
    save_cpo = p_cpo;
    p_cpo = empty_option;
    regmatch.regprog = eval_regcomp(pat);
    যদি (regmatch.regprog != রিক্ত)
    {
	regmatch.rm_ic = ic;
	matches = vim_regexec_nl(&regmatch, text, (colnr_T)0);
	eval_regfree(pat, regmatch.regprog);
    }
    p_cpo = save_cpo;
    প্রত্যাগম matches;
}

/*
 * Handle a name followed by "(".  Both for just "name(arg)" and for
 * "expr->name(arg)".
 * Returns OK or FAIL.
 */
    স্থির পূর্ণাঙ্ক
eval_func(
	char_u	    **arg,	// points to "(", will be advanced
	evalarg_T   *evalarg,
	char_u	    *name,
	পূর্ণাঙ্ক	    name_len,
	typval_T    *rettv,
	পূর্ণাঙ্ক	    flags,
	typval_T    *basetv)	// "expr" for "expr->name(arg)"
{
    পূর্ণাঙ্ক		evaluate = flags & EVAL_EVALUATE;
    char_u	*s = name;
    পূর্ণাঙ্ক		len = name_len;
    partial_T	*partial;
    পূর্ণাঙ্ক		ret = OK;
    type_T	*type = রিক্ত;
    পূর্ণাঙ্ক		found_var = FALSE;

    যদি (!evaluate)
	check_vars(s, len);

    // If "s" is the name of a variable of type VAR_FUNC
    // use its contents.
    s = deref_func_name(s, &len, &partial,
		 in_vim9script() ? &type : রিক্ত, !evaluate, FALSE, &found_var);

    // Need to make a copy, in case evaluating the arguments makes
    // the name invalid.
    s = vim_strsave(s);
    যদি (s == রিক্ত || (evaluate && *s == NUL))
	ret = FAIL;
    অন্যথা
    {
	funcexe_T funcexe;

	// Invoke the function.
	CLEAR_FIELD(funcexe);
	funcexe.fe_firstline = curwin->w_cursor.lnum;
	funcexe.fe_lastline = curwin->w_cursor.lnum;
	funcexe.fe_evaluate = evaluate;
	funcexe.fe_partial = partial;
	যদি (partial != রিক্ত)
	{
	    funcexe.fe_object = partial->pt_obj;
	    যদি (funcexe.fe_object != রিক্ত)
		++funcexe.fe_object->obj_refcount;
	}
	funcexe.fe_basetv = basetv;
	funcexe.fe_check_type = type;
	funcexe.fe_found_var = found_var;
	যদি (evalarg != রিক্ত)
	    funcexe.fe_cctx = evalarg->eval_cctx;
	ret = get_func_tv(s, len, rettv, arg, evalarg, &funcexe);
    }
    vim_free(s);

    // If evaluate is FALSE rettv->v_type was not set in
    // get_func_tv, but it's needed in handle_subscript() to parse
    // what follows. So set it here.
    যদি (rettv->v_type == VAR_UNKNOWN && !evaluate && **arg == '(')
    {
	rettv->vval.v_string = রিক্ত;
	rettv->v_type = VAR_FUNC;
    }

    // Stop the expression evaluation when immediately
    // aborting on error, or when an interrupt occurred or
    // an exception was thrown but not caught.
    যদি (evaluate && aborting())
    {
	যদি (ret == OK)
	    clear_tv(rettv);
	ret = FAIL;
    }
    প্রত্যাগম ret;
}

/*
 * After a NL, skip over empty lines and comment-only lines.
 */
    স্থির char_u *
newline_skip_comments(char_u *arg)
{
    char_u *p = arg + 1;

    প্রতি (;;)
    {
	p = skipwhite(p);

	যদি (*p == NUL)
	    ভঙ্গ;
	যদি (vim9_comment_start(p))
	{
	    char_u *nl = vim_strchr(p, NL);

	    যদি (nl == রিক্ত)
		ভঙ্গ;
	    p = nl;
	}
	যদি (*p != NL)
	    ভঙ্গ;
	++p;  // skip another NL
    }
    প্রত্যাগম p;
}

/*
 * Get the next line source line without advancing.  But do skip over comment
 * lines.
 * Only called for Vim9 script.
 */
    স্থির char_u *
getline_peek_skip_comments(evalarg_T *evalarg)
{
    প্রতি (;;)
    {
	char_u *next = getline_peek(evalarg->eval_getline,
							 evalarg->eval_cookie);
	char_u *p;

	যদি (next == রিক্ত)
	    ভঙ্গ;
	p = skipwhite(next);
	যদি (*p != NUL && !vim9_comment_start(p))
	    প্রত্যাগম next;
	যদি (eval_next_সারি(রিক্ত, evalarg) == রিক্ত)
	    ভঙ্গ;
    }
    প্রত্যাগম রিক্ত;
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
eval_next_non_blank(char_u *arg, evalarg_T *evalarg, পূর্ণাঙ্ক *getnext)
{
    char_u *p = skipwhite(arg);

    *getnext = FALSE;
    যদি (in_vim9script()
	    && evalarg != রিক্ত
	    && (evalarg->eval_cookie != রিক্ত || evalarg->eval_cctx != রিক্ত
								   || *p == NL)
	    && (*p == NUL || *p == NL
			     || (vim9_comment_start(p) && VIM_ISWHITE(p[-1]))))
    {
	char_u *next;

	যদি (*p == NL)
	    next = newline_skip_comments(p);
	অন্যথা যদি (evalarg->eval_cookie != রিক্ত)
	    next = getline_peek_skip_comments(evalarg);
	অন্যথা
	    next = peek_next_সারি_from_context(evalarg->eval_cctx);

	যদি (next != রিক্ত)
	{
	    *getnext = *p != NL;
	    প্রত্যাগম skipwhite(next);
	}
    }
    প্রত্যাগম p;
}

/*
 * To be called after eval_next_non_blank() sets "getnext" to TRUE.
 * Only called for Vim9 script.
 *
 * If "arg" is not NULL, then the caller should assign the return value to
 * "arg".
 */
    char_u *
eval_next_সারি(char_u *arg, evalarg_T *evalarg)
{
    garray_T	*gap = &evalarg->eval_ga;
    char_u	*সারি;

    যদি (arg != রিক্ত)
    {
	যদি (*arg == NL)
	    প্রত্যাগম newline_skip_comments(arg);
	// Truncate before a trailing comment, so that concatenating the lines
	// won't turn the rest into a comment.
	যদি (*skipwhite(arg) == '#')
	    *arg = NUL;
    }

    যদি (evalarg->eval_cookie != রিক্ত)
	সারি = evalarg->eval_getline(0, evalarg->eval_cookie, 0,
							   GETLINE_CONCAT_ALL);
    অন্যথা
	সারি = next_সারি_from_context(evalarg->eval_cctx, TRUE);
    যদি (সারি == রিক্ত)
	প্রত্যাগম রিক্ত;

    ++evalarg->eval_break_গুনতি;
    যদি (gap->ga_itemsize > 0 && ga_grow(gap, 1) == OK)
    {
	char_u *p = skipwhite(সারি);

	// Going to concatenate the lines after parsing.  For an empty or
	// comment line use an empty string.
	যদি (*p == NUL || vim9_comment_start(p))
	{
	    vim_free(সারি);
	    সারি = vim_strsave((char_u *)"");
	}

	((char_u **)gap->ga_data)[gap->ga_len] = সারি;
	++gap->ga_len;
    }
    অন্যথা যদি (evalarg->eval_cookie != রিক্ত)
    {
	free_eval_tofree_later(evalarg);
	evalarg->eval_tofree = সারি;
    }

    // Advanced to the next line, "arg" no longer points into the previous
    // line.  The caller assigns the return value to "arg".
    // If "arg" is NULL, then the return value is discarded.  In that case,
    // "arg" still points to the previous line.  So don't reset
    // "eval_using_cmdline".
    যদি (arg != রিক্ত)
	evalarg->eval_using_cmdline = FALSE;
    প্রত্যাগম skipwhite(সারি);
}

/*
 * Call eval_next_non_blank() and get the next line if needed.
 */
    char_u *
skipwhite_and_linebreak(char_u *arg, evalarg_T *evalarg)
{
    পূর্ণাঙ্ক	    getnext;
    char_u  *p = skipwhite_and_nl(arg);

    যদি (evalarg == রিক্ত)
	প্রত্যাগম skipwhite(arg);
    eval_next_non_blank(p, evalarg, &getnext);
    যদি (getnext)
	প্রত্যাগম eval_next_সারি(arg, evalarg);
    প্রত্যাগম p;
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
    পূর্ণাঙ্ক
eval0(
    char_u	*arg,
    typval_T	*rettv,
    exarg_T	*eap,
    evalarg_T	*evalarg)
{
    প্রত্যাগম eval0_retarg(arg, rettv, eap, evalarg, রিক্ত);
}

/*
 * If "arg" is a simple function call without arguments then call it and return
 * the result.  Otherwise return NOTDONE.
 */
    পূর্ণাঙ্ক
may_call_simple_func(
    char_u	*arg,
    typval_T	*rettv)
{
    char_u  *parens = (char_u *)strstr((বর্ণ *)arg, "()");
    পূর্ণাঙ্ক	    r = NOTDONE;

    // If the expression is "FuncName()" then we can skip a lot of overhead.
    যদি (parens != রিক্ত && *skipwhite(parens + 2) == NUL)
    {
	char_u *p = STRNCMP(arg, "<SNR>", 5) == 0 ? skipdigits(arg + 5) : arg;

	যদি (to_name_end(p, TRUE) == parens)
	    r = call_simple_func(arg, (size_t)(parens - arg), rettv);
    }
    প্রত্যাগম r;
}

/*
 * Handle zero level expression with optimization for a simple function call.
 * Same arguments and return value as eval0().
 */
    স্থির পূর্ণাঙ্ক
eval0_simple_funccal(
    char_u	*arg,
    typval_T	*rettv,
    exarg_T	*eap,
    evalarg_T	*evalarg)
{
    পূর্ণাঙ্ক	    r = may_call_simple_func(arg, rettv);

    যদি (r == NOTDONE)
	r = eval0_retarg(arg, rettv, eap, evalarg, রিক্ত);
    প্রত্যাগম r;
}

/*
 * Like eval0() but when "retarg" is not NULL store the pointer to after the
 * expression and don't check what comes after the expression.
 */
    পূর্ণাঙ্ক
eval0_retarg(
    char_u	*arg,
    typval_T	*rettv,
    exarg_T	*eap,
    evalarg_T	*evalarg,
    char_u	**retarg)
{
    পূর্ণাঙ্ক		ret;
    char_u	*p;
    char_u	*রাশিমালা_end;
    পূর্ণাঙ্ক		did_emsg_before = did_emsg;
    পূর্ণাঙ্ক		called_emsg_before = called_emsg;
    পূর্ণাঙ্ক		check_for_end = retarg == রিক্ত;
    পূর্ণাঙ্ক		end_error = FALSE;

    p = skipwhite(arg);
    ret = eval1(&p, rettv, evalarg);

    যদি (ret != FAIL)
    {
	রাশিমালা_end = p;
	p = skipwhite(p);

	// In Vim9 script a command block is not split at NL characters for
	// commands using an expression argument.  Skip over a '#' comment to
	// check for a following NL.  Require white space before the '#'.
	যদি (in_vim9script() && p > রাশিমালা_end && retarg == রিক্ত)
	    যাবত্ (*p == '#')
	    {
		char_u *nl = vim_strchr(p, NL);

		যদি (nl == রিক্ত)
		    ভঙ্গ;
		p = skipwhite(nl + 1);
		যদি (eap != রিক্ত && *p != NUL)
		    eap->nextcmd = p;
		check_for_end = FALSE;
	    }

	যদি (check_for_end)
	    end_error = !ends_excmd2(arg, p);
    }

    যদি (ret == FAIL || end_error)
    {
	যদি (ret != FAIL)
	    clear_tv(rettv);
	/*
	 * Report the invalid expression unless the expression evaluation has
	 * been cancelled due to an aborting error, an interrupt, or an
	 * exception, or we already gave a more specific error.
	 * Also check called_emsg for when using assert_fails().
	 */
	যদি (!aborting()
		&& did_emsg == did_emsg_before
		&& called_emsg == called_emsg_before
		&& (!in_vim9script() || !vim9_bad_comment(p)))
	{
	    যদি (end_error)
		semsg(_(e_trailing_characters_str), p);
	    অন্যথা
		semsg(_(e_invalid_expression_str), arg);
	}

	যদি (eap != রিক্ত && p != রিক্ত)
	{
	    // Some of the expression may not have been consumed.
	    // Only execute a next command if it cannot be a "||" operator.
	    // The next command may be "catch".
	    char_u *nextcmd = check_nextcmd(p);
	    যদি (nextcmd != রিক্ত && *nextcmd != '|')
		eap->nextcmd = nextcmd;
	}
	প্রত্যাগম FAIL;
    }

    যদি (retarg != রিক্ত)
	*retarg = p;
    অন্যথা যদি (check_for_end && eap != রিক্ত)
	set_nextcmd(eap, p);

    প্রত্যাগম ret;
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
    পূর্ণাঙ্ক
eval1(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    char_u  *p;
    পূর্ণাঙ্ক	    getnext;

    CLEAR_POINTER(rettv);

    /*
     * Get the first variable.
     */
    যদি (eval2(arg, rettv, evalarg) == FAIL)
	প্রত্যাগম FAIL;

    p = eval_next_non_blank(*arg, evalarg, &getnext);
    যদি (*p == '?')
    {
	পূর্ণাঙ্ক		op_falsy = p[1] == '?';
	পূর্ণাঙ্ক		result;
	typval_T	var2;
	evalarg_T	*evalarg_used = evalarg;
	evalarg_T	local_evalarg;
	পূর্ণাঙ্ক		orig_flags;
	পূর্ণাঙ্ক		evaluate;
	পূর্ণাঙ্ক		vim9script = in_vim9script();

	যদি (evalarg == রিক্ত)
	{
	    init_evalarg(&local_evalarg);
	    evalarg_used = &local_evalarg;
	}
	orig_flags = evalarg_used->eval_flags;
	evaluate = evalarg_used->eval_flags & EVAL_EVALUATE;

	যদি (getnext)
	    *arg = eval_next_সারি(*arg, evalarg_used);
	অন্যথা
	{
	    যদি (evaluate && vim9script && !VIM_ISWHITE(p[-1]))
	    {
		error_white_both(p, op_falsy ? 2 : 1);
		clear_tv(rettv);
		প্রত্যাগম FAIL;
	    }
	    *arg = p;
	}

	result = FALSE;
	যদি (evaluate)
	{
	    পূর্ণাঙ্ক		error = FALSE;

	    যদি (op_falsy)
	    {
		// Is this typeval supported with the falsy operator?
		যদি (check_typval_is_মান(rettv) == FAIL)
		{
		    clear_tv(rettv);
		    প্রত্যাগম FAIL;
		}
		result = tv2bool(rettv);
	    }
	    অন্যথা যদি (vim9script)
		result = tv_get_bool_chk(rettv, &error);
	    অন্যথা যদি (tv_get_সংখ্যা_chk(rettv, &error) != 0)
		result = TRUE;
	    যদি (error || !op_falsy || !result)
		clear_tv(rettv);
	    যদি (error)
		প্রত্যাগম FAIL;
	}

	/*
	 * Get the second variable.  Recursive!
	 */
	যদি (op_falsy)
	    ++*arg;
	যদি (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[1]))
	{
	    error_white_both(*arg - (op_falsy ? 1 : 0), op_falsy ? 2 : 1);
	    clear_tv(rettv);
	    প্রত্যাগম FAIL;
	}
	*arg = skipwhite_and_linebreak(*arg + 1, evalarg_used);
	evalarg_used->eval_flags = (op_falsy ? !result : result)
				  ? orig_flags : (orig_flags & ~EVAL_EVALUATE);
	যদি (eval1(arg, &var2, evalarg_used) == FAIL)
	{
	    evalarg_used->eval_flags = orig_flags;
	    প্রত্যাগম FAIL;
	}
	যদি (!op_falsy || !result)
	    *rettv = var2;

	যদি (!op_falsy)
	{
	    /*
	     * Check for the ":".
	     */
	    p = eval_next_non_blank(*arg, evalarg_used, &getnext);
	    যদি (*p != ':')
	    {
		emsg(_(e_missing_colon_after_questionmark));
		যদি (evaluate && result)
		    clear_tv(rettv);
		evalarg_used->eval_flags = orig_flags;
		প্রত্যাগম FAIL;
	    }
	    যদি (getnext)
		*arg = eval_next_সারি(*arg, evalarg_used);
	    অন্যথা
	    {
		যদি (evaluate && vim9script && !VIM_ISWHITE(p[-1]))
		{
		    error_white_both(p, 1);
		    clear_tv(rettv);
		    evalarg_used->eval_flags = orig_flags;
		    প্রত্যাগম FAIL;
		}
		*arg = p;
	    }

	    /*
	     * Get the third variable.  Recursive!
	     */
	    যদি (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[1]))
	    {
		error_white_both(*arg, 1);
		clear_tv(rettv);
		evalarg_used->eval_flags = orig_flags;
		প্রত্যাগম FAIL;
	    }
	    *arg = skipwhite_and_linebreak(*arg + 1, evalarg_used);
	    evalarg_used->eval_flags = !result ? orig_flags
					       : (orig_flags & ~EVAL_EVALUATE);
	    যদি (eval1(arg, &var2, evalarg_used) == FAIL)
	    {
		যদি (evaluate && result)
		    clear_tv(rettv);
		evalarg_used->eval_flags = orig_flags;
		প্রত্যাগম FAIL;
	    }
	    যদি (evaluate && !result)
		*rettv = var2;
	}

	যদি (evalarg == রিক্ত)
	    clear_evalarg(&local_evalarg, রিক্ত);
	অন্যথা
	    evalarg->eval_flags = orig_flags;
    }

    প্রত্যাগম OK;
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
    স্থির পূর্ণাঙ্ক
eval2(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    char_u	*p;
    পূর্ণাঙ্ক		getnext;

    /*
     * Get the first expression.
     */
    যদি (eval3(arg, rettv, evalarg) == FAIL)
	প্রত্যাগম FAIL;

    /*
     * Handle the  "||" operator.
     */
    p = eval_next_non_blank(*arg, evalarg, &getnext);
    যদি (p[0] == '|' && p[1] == '|')
    {
	evalarg_T   *evalarg_used = evalarg;
	evalarg_T   local_evalarg;
	পূর্ণাঙ্ক	    evaluate;
	পূর্ণাঙ্ক	    orig_flags;
	long	    result = FALSE;
	typval_T    var2;
	পূর্ণাঙ্ক	    error = FALSE;
	পূর্ণাঙ্ক	    vim9script = in_vim9script();

	যদি (evalarg == রিক্ত)
	{
	    init_evalarg(&local_evalarg);
	    evalarg_used = &local_evalarg;
	}
	orig_flags = evalarg_used->eval_flags;
	evaluate = orig_flags & EVAL_EVALUATE;
	যদি (evaluate)
	{
	    যদি (vim9script)
		result = tv_get_bool_chk(rettv, &error);
	    অন্যথা যদি (tv_get_সংখ্যা_chk(rettv, &error) != 0)
		result = TRUE;
	    clear_tv(rettv);
	    যদি (error)
		প্রত্যাগম FAIL;
	}

	/*
	 * Repeat until there is no following "||".
	 */
	যাবত্ (p[0] == '|' && p[1] == '|')
	{
	    যদি (getnext)
		*arg = eval_next_সারি(*arg, evalarg_used);
	    অন্যথা
	    {
		যদি (evaluate && vim9script && !VIM_ISWHITE(p[-1]))
		{
		    error_white_both(p, 2);
		    clear_tv(rettv);
		    প্রত্যাগম FAIL;
		}
		*arg = p;
	    }

	    /*
	     * Get the second variable.
	     */
	    যদি (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[2]))
	    {
		error_white_both(*arg, 2);
		clear_tv(rettv);
		প্রত্যাগম FAIL;
	    }
	    *arg = skipwhite_and_linebreak(*arg + 2, evalarg_used);
	    evalarg_used->eval_flags = !result ? orig_flags
					       : (orig_flags & ~EVAL_EVALUATE);
	    যদি (eval3(arg, &var2, evalarg_used) == FAIL)
		প্রত্যাগম FAIL;

	    /*
	     * Compute the result.
	     */
	    যদি (evaluate && !result)
	    {
		যদি (vim9script)
		    result = tv_get_bool_chk(&var2, &error);
		অন্যথা যদি (tv_get_সংখ্যা_chk(&var2, &error) != 0)
		    result = TRUE;
		clear_tv(&var2);
		যদি (error)
		    প্রত্যাগম FAIL;
	    }
	    যদি (evaluate)
	    {
		যদি (vim9script)
		{
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_সংখ্যা = result ? VVAL_TRUE : VVAL_FALSE;
		}
		অন্যথা
		{
		    rettv->v_type = VAR_সংখ্যা;
		    rettv->vval.v_সংখ্যা = result;
		}
	    }

	    p = eval_next_non_blank(*arg, evalarg_used, &getnext);
	}

	যদি (evalarg == রিক্ত)
	    clear_evalarg(&local_evalarg, রিক্ত);
	অন্যথা
	    evalarg->eval_flags = orig_flags;
    }

    প্রত্যাগম OK;
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
    স্থির পূর্ণাঙ্ক
eval3(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    char_u	*p;
    পূর্ণাঙ্ক		getnext;

    /*
     * Get the first expression.
     */
    যদি (eval4(arg, rettv, evalarg) == FAIL)
	প্রত্যাগম FAIL;

    /*
     * Handle the "&&" operator.
     */
    p = eval_next_non_blank(*arg, evalarg, &getnext);
    যদি (p[0] == '&' && p[1] == '&')
    {
	evalarg_T   *evalarg_used = evalarg;
	evalarg_T   local_evalarg;
	পূর্ণাঙ্ক	    orig_flags;
	পূর্ণাঙ্ক	    evaluate;
	long	    result = TRUE;
	typval_T    var2;
	পূর্ণাঙ্ক	    error = FALSE;
	পূর্ণাঙ্ক	    vim9script = in_vim9script();

	যদি (evalarg == রিক্ত)
	{
	    init_evalarg(&local_evalarg);
	    evalarg_used = &local_evalarg;
	}
	orig_flags = evalarg_used->eval_flags;
	evaluate = orig_flags & EVAL_EVALUATE;
	যদি (evaluate)
	{
	    যদি (vim9script)
		result = tv_get_bool_chk(rettv, &error);
	    অন্যথা যদি (tv_get_সংখ্যা_chk(rettv, &error) == 0)
		result = FALSE;
	    clear_tv(rettv);
	    যদি (error)
		প্রত্যাগম FAIL;
	}

	/*
	 * Repeat until there is no following "&&".
	 */
	যাবত্ (p[0] == '&' && p[1] == '&')
	{
	    যদি (getnext)
		*arg = eval_next_সারি(*arg, evalarg_used);
	    অন্যথা
	    {
		যদি (evaluate && vim9script && !VIM_ISWHITE(p[-1]))
		{
		    error_white_both(p, 2);
		    clear_tv(rettv);
		    প্রত্যাগম FAIL;
		}
		*arg = p;
	    }

	    /*
	     * Get the second variable.
	     */
	    যদি (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[2]))
	    {
		error_white_both(*arg, 2);
		clear_tv(rettv);
		প্রত্যাগম FAIL;
	    }
	    *arg = skipwhite_and_linebreak(*arg + 2, evalarg_used);
	    evalarg_used->eval_flags = result ? orig_flags
					      : (orig_flags & ~EVAL_EVALUATE);
	    CLEAR_FIELD(var2);
	    যদি (eval4(arg, &var2, evalarg_used) == FAIL)
		প্রত্যাগম FAIL;

	    /*
	     * Compute the result.
	     */
	    যদি (evaluate && result)
	    {
		যদি (vim9script)
		    result = tv_get_bool_chk(&var2, &error);
		অন্যথা যদি (tv_get_সংখ্যা_chk(&var2, &error) == 0)
		    result = FALSE;
		clear_tv(&var2);
		যদি (error)
		    প্রত্যাগম FAIL;
	    }
	    যদি (evaluate)
	    {
		যদি (vim9script)
		{
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_সংখ্যা = result ? VVAL_TRUE : VVAL_FALSE;
		}
		অন্যথা
		{
		    rettv->v_type = VAR_সংখ্যা;
		    rettv->vval.v_সংখ্যা = result;
		}
	    }

	    p = eval_next_non_blank(*arg, evalarg_used, &getnext);
	}

	যদি (evalarg == রিক্ত)
	    clear_evalarg(&local_evalarg, রিক্ত);
	অন্যথা
	    evalarg->eval_flags = orig_flags;
    }

    প্রত্যাগম OK;
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
    স্থির পূর্ণাঙ্ক
eval4(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    char_u	*p;
    পূর্ণাঙ্ক		getnext;
    exprtype_T	type = রাশিমালা_UNKNOWN;
    পূর্ণাঙ্ক		len = 2;
    পূর্ণাঙ্ক		type_is = FALSE;

    /*
     * Get the first expression.
     */
    যদি (eval5(arg, rettv, evalarg) == FAIL)
	প্রত্যাগম FAIL;

    p = eval_next_non_blank(*arg, evalarg, &getnext);

    type = get_compare_type(p, &len, &type_is);

    /*
     * If there is a comparative operator, use it.
     */
    যদি (type != রাশিমালা_UNKNOWN)
    {
	typval_T    var2;
	পূর্ণাঙ্ক	    ic;
	পূর্ণাঙ্ক	    vim9script = in_vim9script();
	পূর্ণাঙ্ক	    evaluate = evalarg == রিক্ত
				   ? 0 : (evalarg->eval_flags & EVAL_EVALUATE);
	long	    comp_lnum = SOURCING_LNUM;

	যদি (getnext)
	{
	    *arg = eval_next_সারি(*arg, evalarg);
	    p = *arg;
	}
	অন্যথা যদি (evaluate && vim9script && !VIM_ISWHITE(**arg))
	{
	    error_white_both(*arg, len);
	    clear_tv(rettv);
	    প্রত্যাগম FAIL;
	}

	যদি (vim9script && type_is && (p[len] == '?' || p[len] == '#'))
	{
	    semsg(_(e_invalid_expression_str), p);
	    clear_tv(rettv);
	    প্রত্যাগম FAIL;
	}

	// extra question mark appended: ignore case
	যদি (p[len] == '?')
	{
	    ic = TRUE;
	    ++len;
	}
	// extra '#' appended: match case
	অন্যথা যদি (p[len] == '#')
	{
	    ic = FALSE;
	    ++len;
	}
	// nothing appended: use 'ignorecase' if not in Vim script
	অন্যথা
	    ic = vim9script ? FALSE : p_ic;

	/*
	 * Get the second variable.
	 */
	যদি (evaluate && vim9script && !IS_WHITE_OR_NUL(p[len]))
	{
	    error_white_both(p, len);
	    clear_tv(rettv);
	    প্রত্যাগম FAIL;
	}
	*arg = skipwhite_and_linebreak(p + len, evalarg);
	যদি (eval5(arg, &var2, evalarg) == FAIL)
	{
	    clear_tv(rettv);
	    প্রত্যাগম FAIL;
	}
	যদি (evaluate)
	{
	    পূর্ণাঙ্ক ret;

	    // use the line of the comparison for messages
	    SOURCING_LNUM = comp_lnum;
	    যদি (vim9script && check_compare_types(type, rettv, &var2) == FAIL)
	    {
		ret = FAIL;
		clear_tv(rettv);
	    }
	    অন্যথা
		ret = typval_compare(rettv, &var2, type, ic);
	    clear_tv(&var2);
	    প্রত্যাগম ret;
	}
    }

    প্রত্যাগম OK;
}

/*
 * Make a copy of blob "tv1" and append blob "tv2".
 */
    শূন্য
eval_addblob(typval_T *tv1, typval_T *tv2)
{
    blob_T  *b1 = tv1->vval.v_blob;
    blob_T  *b2 = tv2->vval.v_blob;
    blob_T  *b = blob_alloc();
    long    len1, len2;
    long    totallen;

    যদি (b == রিক্ত)
	প্রত্যাগম;

    len1 = blob_len(b1);
    len2 = blob_len(b2);
    totallen = len1 + len2;

    যদি (totallen > 0 && totallen <= INT_MAX
				    && ga_grow(&b->bv_ga, (পূর্ণাঙ্ক)totallen) == OK)
    {
	যদি (len1 > 0)
	    mch_memmove((char_u *)b->bv_ga.ga_data,
		    (char_u *)b1->bv_ga.ga_data, (size_t)len1);
	যদি (len2 > 0)
	    mch_memmove((char_u *)b->bv_ga.ga_data + len1,
		    (char_u *)b2->bv_ga.ga_data, (size_t)len2);
	b->bv_ga.ga_len = (পূর্ণাঙ্ক)totallen;
    }

    clear_tv(tv1);
    rettv_blob_set(tv1, b);
}

/*
 * Make a copy of list "tv1" and append list "tv2".
 */
    পূর্ণাঙ্ক
eval_addlist(typval_T *tv1, typval_T *tv2)
{
    typval_T var3;

    // concatenate Lists
    যদি (list_concat(tv1->vval.v_list, tv2->vval.v_list, &var3) == FAIL)
    {
	clear_tv(tv1);
	clear_tv(tv2);
	প্রত্যাগম FAIL;
    }
    clear_tv(tv1);
    *tv1 = var3;
    প্রত্যাগম OK;
}

/*
 * Make a copy of tuple "tv1" and append tuple "tv2".
 */
    পূর্ণাঙ্ক
eval_addtuple(typval_T *tv1, typval_T *tv2)
{
    পূর্ণাঙ্ক		vim9script = in_vim9script();
    typval_T	var3;

    যদি (vim9script && tv1->vval.v_tuple != রিক্ত && tv2->vval.v_tuple != রিক্ত
	    && tv1->vval.v_tuple->tv_type != রিক্ত
	    && tv2->vval.v_tuple->tv_type != রিক্ত)
    {
	যদি (!check_tuples_addable(tv1->vval.v_tuple->tv_type,
						tv2->vval.v_tuple->tv_type))
	    প্রত্যাগম FAIL;
    }

    // concatenate tuples
    যদি (tuple_concat(tv1->vval.v_tuple, tv2->vval.v_tuple, &var3) == FAIL)
    {
	clear_tv(tv1);
	clear_tv(tv2);
	প্রত্যাগম FAIL;
    }
    clear_tv(tv1);
    *tv1 = var3;
    প্রত্যাগম OK;
}

/*
 * Left or right shift the number "tv1" by the number "tv2" and store the
 * result in "tv1".
 *
 * Return OK or FAIL.
 */
    স্থির পূর্ণাঙ্ক
eval_shift_সংখ্যা(typval_T *tv1, typval_T *tv2, পূর্ণাঙ্ক shift_type)
{
    যদি (tv2->v_type != VAR_সংখ্যা || tv2->vval.v_সংখ্যা < 0)
    {
	// right operand should be a positive number
	যদি (tv2->v_type != VAR_সংখ্যা)
	    emsg(_(e_bitshift_ops_must_be_সংখ্যা));
	অন্যথা
	    emsg(_(e_bitshift_ops_must_be_positive));
	clear_tv(tv1);
	clear_tv(tv2);
	প্রত্যাগম FAIL;
    }

    যদি (tv2->vval.v_সংখ্যা > MAX_LSHIFT_BITS)
	// shifting more bits than we have always results in zero
	tv1->vval.v_সংখ্যা = 0;
    অন্যথা যদি (shift_type == রাশিমালা_LSHIFT)
	tv1->vval.v_সংখ্যা =
	    (uvarnumber_T)tv1->vval.v_সংখ্যা << tv2->vval.v_সংখ্যা;
    অন্যথা
	tv1->vval.v_সংখ্যা =
	    (uvarnumber_T)tv1->vval.v_সংখ্যা >> tv2->vval.v_সংখ্যা;

    প্রত্যাগম OK;
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
    স্থির পূর্ণাঙ্ক
eval5(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    /*
     * Get the first expression.
     */
    যদি (eval6(arg, rettv, evalarg) == FAIL)
	প্রত্যাগম FAIL;

    /*
     * Repeat computing, until no '<<' or '>>' is following.
     */
    প্রতি (;;)
    {
	char_u		*p;
	পূর্ণাঙ্ক		getnext;
	exprtype_T	exprtype;
	পূর্ণাঙ্ক		evaluate;
	typval_T	var2;
	পূর্ণাঙ্ক		vim9script;

	p = eval_next_non_blank(*arg, evalarg, &getnext);
	যদি (p[0] == '<' && p[1] == '<')
	    exprtype = রাশিমালা_LSHIFT;
	অন্যথা যদি (p[0] == '>' && p[1] == '>')
	    exprtype = রাশিমালা_RSHIFT;
	অন্যথা
	    প্রত্যাগম OK;

	// Handle a bitwise left or right shift operator
	evaluate = evalarg == রিক্ত ? 0 : (evalarg->eval_flags & EVAL_EVALUATE);
	যদি (evaluate && rettv->v_type != VAR_সংখ্যা)
	{
	    // left operand should be a number
	    emsg(_(e_bitshift_ops_must_be_সংখ্যা));
	    clear_tv(rettv);
	    প্রত্যাগম FAIL;
	}

	vim9script = in_vim9script();
	যদি (getnext)
	{
	    *arg = eval_next_সারি(*arg, evalarg);
	    p = *arg;
	}
	অন্যথা যদি (evaluate && vim9script && !VIM_ISWHITE(**arg))
	{
	    error_white_both(*arg, 2);
	    clear_tv(rettv);
	    প্রত্যাগম FAIL;
	}

	/*
	 * Get the second variable.
	 */
	যদি (evaluate && vim9script && !IS_WHITE_OR_NUL(p[2]))
	{
	    error_white_both(p, 2);
	    clear_tv(rettv);
	    প্রত্যাগম FAIL;
	}
	*arg = skipwhite_and_linebreak(p + 2, evalarg);
	যদি (eval6(arg, &var2, evalarg) == FAIL)
	{
	    clear_tv(rettv);
	    প্রত্যাগম FAIL;
	}

	যদি (evaluate)
	{
	    যদি (eval_shift_সংখ্যা(rettv, &var2, exprtype) == FAIL)
		প্রত্যাগম FAIL;
	}

	clear_tv(&var2);
    }

    প্রত্যাগম OK;
}

/*
 * Concatenate strings "tv1" and "tv2" and store the result in "tv1".
 */
    স্থির পূর্ণাঙ্ক
eval_concat_str(typval_T *tv1, typval_T *tv2)
{
    char_u	buf1[NUMBUFLEN], buf2[NUMBUFLEN];
    char_u	*s1 = tv_get_string_buf(tv1, buf1);
    char_u	*s2 = রিক্ত;
    char_u	*p;
    পূর্ণাঙ্ক		vim9script = in_vim9script();

    যদি (vim9script && (tv2->v_type == VAR_VOID
		|| tv2->v_type == VAR_CHANNEL
		|| tv2->v_type == VAR_JOB))
	semsg(_(e_using_invalid_মান_as_string_str),
		vartype_name(tv2->v_type));
    অন্যথা যদি (vim9script && tv2->v_type == VAR_FLOAT)
    {
	vim_snprintf((বর্ণ *)buf2, NUMBUFLEN, "%g",
		tv2->vval.v_float);
	s2 = buf2;
    }
    অন্যথা
	s2 = tv_get_string_buf_chk(tv2, buf2);
    যদি (s2 == রিক্ত)		// type error ?
    {
	clear_tv(tv1);
	clear_tv(tv2);
	প্রত্যাগম FAIL;
    }

    // When possible, grow the existing string in place to avoid alloc/free.
    যদি (grow_string_tv(tv1, s2) == OK)
	প্রত্যাগম OK;

    p = concat_str(s1, s2);
    clear_tv(tv1);
    tv1->v_type = VAR_STRING;
    tv1->vval.v_string = p;

    প্রত্যাগম OK;
}

/*
 * Add or subtract numbers "tv1" and "tv2" and store the result in "tv1".
 * The numbers can be whole numbers or floats.
 */
    স্থির পূর্ণাঙ্ক
eval_addsub_সংখ্যা(typval_T *tv1, typval_T *tv2, পূর্ণাঙ্ক op)
{
    পূর্ণাঙ্ক		error = FALSE;
    varnumber_T	n1, n2;
    float_T	f1 = 0, f2 = 0;

    যদি (tv1->v_type == VAR_FLOAT)
    {
	f1 = tv1->vval.v_float;
	n1 = 0;
    }
    অন্যথা
    {
	n1 = tv_get_সংখ্যা_chk(tv1, &error);
	যদি (error)
	{
	    // This can only happen for "list + non-list" or
	    // "blob + non-blob".  For "non-list + ..." or
	    // "something - ...", we returned before evaluating the
	    // 2nd operand.
	    clear_tv(tv1);
	    clear_tv(tv2);
	    প্রত্যাগম FAIL;
	}
	যদি (tv2->v_type == VAR_FLOAT)
	    f1 = n1;
    }
    যদি (tv2->v_type == VAR_FLOAT)
    {
	f2 = tv2->vval.v_float;
	n2 = 0;
    }
    অন্যথা
    {
	n2 = tv_get_সংখ্যা_chk(tv2, &error);
	যদি (error)
	{
	    clear_tv(tv1);
	    clear_tv(tv2);
	    প্রত্যাগম FAIL;
	}
	যদি (tv1->v_type == VAR_FLOAT)
	    f2 = n2;
    }
    clear_tv(tv1);

    // If there is a float on either side the result is a float.
    যদি (tv1->v_type == VAR_FLOAT || tv2->v_type == VAR_FLOAT)
    {
	যদি (op == '+')
	    f1 = f1 + f2;
	অন্যথা
	    f1 = f1 - f2;
	tv1->v_type = VAR_FLOAT;
	tv1->vval.v_float = f1;
    }
    অন্যথা
    {
	যদি (op == '+')
	    n1 = n1 + n2;
	অন্যথা
	    n1 = n1 - n2;
	tv1->v_type = VAR_সংখ্যা;
	tv1->vval.v_সংখ্যা = n1;
    }

    প্রত্যাগম OK;
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
    স্থির পূর্ণাঙ্ক
eval6(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    /*
     * Get the first expression.
     */
    যদি (eval7(arg, rettv, evalarg, FALSE) == FAIL)
	প্রত্যাগম FAIL;

    /*
     * Repeat computing, until no '+', '-' or '.' is following.
     */
    প্রতি (;;)
    {
	পূর্ণাঙ্ক	    evaluate;
	পূর্ণাঙ্ক	    getnext;
	char_u	    *p;
	পূর্ণাঙ্ক	    op;
	পূর্ণাঙ্ক	    oplen;
	পূর্ণাঙ্ক	    concat;
	typval_T    var2;
	পূর্ণাঙ্ক	    vim9script = in_vim9script();
	long	    op_lnum = SOURCING_LNUM;

	// "." is only string concatenation when scriptversion is 1
	// "+=", "-=" and "..=" are assignments
	// "++" and "--" on the next line are a separate command.
	p = eval_next_non_blank(*arg, evalarg, &getnext);
	op = *p;
	concat = op == '.' && (*(p + 1) == '.' || in_old_script(2));
	যদি ((op != '+' && op != '-' && !concat) || p[1] == '='
					       || (p[1] == '.' && p[2] == '='))
	    ভঙ্গ;
	যদি (getnext && (op == '+' || op == '-') && p[0] == p[1])
	    ভঙ্গ;

	evaluate = evalarg == রিক্ত ? 0 : (evalarg->eval_flags & EVAL_EVALUATE);
	oplen = (concat && p[1] == '.') ? 2 : 1;
	যদি (getnext)
	    *arg = eval_next_সারি(*arg, evalarg);
	অন্যথা
	{
	    যদি (evaluate && vim9script && !VIM_ISWHITE(**arg))
	    {
		error_white_both(*arg, oplen);
		clear_tv(rettv);
		প্রত্যাগম FAIL;
	    }
	    *arg = p;
	}
	যদি ((op != '+' || (rettv->v_type != VAR_LIST
						&& rettv->v_type != VAR_TUPLE
						&& rettv->v_type != VAR_BLOB))
		&& (op == '.' || rettv->v_type != VAR_FLOAT)
		&& evaluate)
	{
	    পূর্ণাঙ্ক		error = FALSE;

	    // For "list + ...", an illegal use of the first operand as
	    // a number cannot be determined before evaluating the 2nd
	    // operand: if this is also a list, all is ok.
	    // For "something . ...", "something - ..." or "non-list + ...",
	    // we know that the first operand needs to be a string or number
	    // without evaluating the 2nd operand.  So check before to avoid
	    // side effects after an error.
	    যদি (op != '.')
		tv_get_সংখ্যা_chk(rettv, &error);
	    যদি ((op == '.' && tv_get_string_chk(rettv) == রিক্ত) || error)
	    {
		clear_tv(rettv);
		প্রত্যাগম FAIL;
	    }
	}

	/*
	 * Get the second variable.
	 */
	যদি (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[oplen]))
	{
	    error_white_both(*arg, oplen);
	    clear_tv(rettv);
	    প্রত্যাগম FAIL;
	}
	*arg = skipwhite_and_linebreak(*arg + oplen, evalarg);
	যদি (eval7(arg, &var2, evalarg, !vim9script && op == '.') == FAIL)
	{
	    clear_tv(rettv);
	    প্রত্যাগম FAIL;
	}

	যদি (evaluate)
	{
	    /*
	     * Compute the result.
	     */
	    // use the line of the operation for messages
	    SOURCING_LNUM = op_lnum;
	    যদি (op == '.')
	    {
		যদি (eval_concat_str(rettv, &var2) == FAIL)
		    প্রত্যাগম FAIL;
	    }
	    অন্যথা যদি (op == '+' && rettv->v_type == VAR_BLOB
						   && var2.v_type == VAR_BLOB)
		eval_addblob(rettv, &var2);
	    অন্যথা যদি (op == '+' && rettv->v_type == VAR_LIST
						   && var2.v_type == VAR_LIST)
	    {
		যদি (eval_addlist(rettv, &var2) == FAIL)
		    প্রত্যাগম FAIL;
	    }
	    অন্যথা যদি (op == '+' && rettv->v_type == VAR_TUPLE
					   && var2.v_type == VAR_TUPLE)
	    {
		যদি (eval_addtuple(rettv, &var2) == FAIL)
		    প্রত্যাগম FAIL;
	    }
	    অন্যথা
	    {
		যদি (eval_addsub_সংখ্যা(rettv, &var2, op) == FAIL)
		    প্রত্যাগম FAIL;
	    }
	    clear_tv(&var2);
	}
    }
    প্রত্যাগম OK;
}

/*
 * Multiply or divide or compute the modulo of numbers "tv1" and "tv2" and
 * store the result in "tv1".  The numbers can be whole numbers or floats.
 */
    স্থির পূর্ণাঙ্ক
eval_multdiv_সংখ্যা(typval_T *tv1, typval_T *tv2, পূর্ণাঙ্ক op)
{
    varnumber_T	n1, n2;
    float_T	f1, f2;
    পূর্ণাঙ্ক		error;
    পূর্ণাঙ্ক		use_float = FALSE;

    f1 = 0;
    f2 = 0;
    error = FALSE;
    যদি (tv1->v_type == VAR_FLOAT)
    {
	f1 = tv1->vval.v_float;
	use_float = TRUE;
	n1 = 0;
    }
    অন্যথা
	n1 = tv_get_সংখ্যা_chk(tv1, &error);
    clear_tv(tv1);
    যদি (error)
    {
	clear_tv(tv2);
	প্রত্যাগম FAIL;
    }

    যদি (tv2->v_type == VAR_FLOAT)
    {
	যদি (!use_float)
	{
	    f1 = n1;
	    use_float = TRUE;
	}
	f2 = tv2->vval.v_float;
	n2 = 0;
    }
    অন্যথা
    {
	n2 = tv_get_সংখ্যা_chk(tv2, &error);
	clear_tv(tv2);
	যদি (error)
	    প্রত্যাগম FAIL;
	যদি (use_float)
	    f2 = n2;
    }

    /*
     * Compute the result.
     * When either side is a float the result is a float.
     */
    যদি (use_float)
    {
	যদি (op == '*')
	    f1 = f1 * f2;
	অন্যথা যদি (op == '/')
	{
#ifdef VMS
	    // VMS crashes on divide by zero, work around it
	    যদি (f2 == 0.0)
	    {
		যদি (f1 == 0)
		    f1 = -1 * __F_FLT_MAX - 1L;   // similar to NaN
		অন্যথা যদি (f1 < 0)
		    f1 = -1 * __F_FLT_MAX;
		অন্যথা
		    f1 = __F_FLT_MAX;
	    }
	    অন্যথা
		f1 = f1 / f2;
#অন্যথা
	    // We rely on the floating point library to handle divide
	    // by zero to result in "inf" and not a crash.
	    f1 = f1 / f2;
#endif
	}
	অন্যথা
	{
	    emsg(_(e_cannot_use_percent_with_float));
	    প্রত্যাগম FAIL;
	}
	tv1->v_type = VAR_FLOAT;
	tv1->vval.v_float = f1;
    }
    অন্যথা
    {
	পূর্ণাঙ্ক	    failed = FALSE;

	যদি (op == '*')
	    n1 = n1 * n2;
	অন্যথা যদি (op == '/')
	    n1 = অঙ্ক_divide(n1, n2, &failed);
	অন্যথা
	    n1 = অঙ্ক_modulus(n1, n2, &failed);
	যদি (failed)
	    প্রত্যাগম FAIL;

	tv1->v_type = VAR_সংখ্যা;
	tv1->vval.v_সংখ্যা = n1;
    }

    প্রত্যাগম OK;
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
    স্থির পূর্ণাঙ্ক
eval7(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    পূর্ণাঙ্ক		want_string)  // after "." operator
{
    /*
     * Get the first expression.
     */
    যদি (eval8(arg, rettv, evalarg, want_string) == FAIL)
	প্রত্যাগম FAIL;

    /*
     * Repeat computing, until no '*', '/' or '%' is following.
     */
    প্রতি (;;)
    {
	পূর্ণাঙ্ক	    evaluate;
	পূর্ণাঙ্ক	    getnext;
	typval_T    var2;
	char_u	    *p;
	পূর্ণাঙ্ক	    op;

	// "*=", "/=" and "%=" are assignments
	p = eval_next_non_blank(*arg, evalarg, &getnext);
	op = *p;
	যদি ((op != '*' && op != '/' && op != '%') || p[1] == '=')
	    ভঙ্গ;

	evaluate = evalarg == রিক্ত ? 0 : (evalarg->eval_flags & EVAL_EVALUATE);
	যদি (getnext)
	    *arg = eval_next_সারি(*arg, evalarg);
	অন্যথা
	{
	    যদি (evaluate && in_vim9script() && !VIM_ISWHITE(**arg))
	    {
		error_white_both(*arg, 1);
		clear_tv(rettv);
		প্রত্যাগম FAIL;
	    }
	    *arg = p;
	}

	/*
	 * Get the second variable.
	 */
	যদি (evaluate && in_vim9script() && !IS_WHITE_OR_NUL((*arg)[1]))
	{
	    error_white_both(*arg, 1);
	    clear_tv(rettv);
	    প্রত্যাগম FAIL;
	}
	*arg = skipwhite_and_linebreak(*arg + 1, evalarg);
	যদি (eval8(arg, &var2, evalarg, FALSE) == FAIL)
	    প্রত্যাগম FAIL;

	যদি (evaluate)
	    // Compute the result.
	    যদি (eval_multdiv_সংখ্যা(rettv, &var2, op) == FAIL)
		প্রত্যাগম FAIL;
    }

    প্রত্যাগম OK;
}

/*
 * Handle seventh level expression:
 *	a type cast before a base level expression.
 * "arg" must point to the first non-white of the expression.
 * "arg" is advanced to just after the recognized expression.
 * Return OK or FAIL.
 */
    স্থির পূর্ণাঙ্ক
eval8(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    পূর্ণাঙ্ক		want_string)	// after "." operator
{
    type_T	*want_type = রিক্ত;
    garray_T	type_list;	    // list of pointers to allocated types
    পূর্ণাঙ্ক		res;
    পূর্ণাঙ্ক		evaluate = evalarg == রিক্ত ? 0
				       : (evalarg->eval_flags & EVAL_EVALUATE);

    // Recognize <type> in Vim9 script only.
    যদি (in_vim9script() && **arg == '<' && eval_isnamec1((*arg)[1])
					     && STRNCMP(*arg, "<SNR>", 5) != 0)
    {
	++*arg;
	ga_init2(&type_list, পরিমাণ(type_T *), 10);
	যদি (!evaluate)
	    // Only skip over the type, it may use a type variable that is only
	    // known when the expression is compiled, e.g. in a lambda.
	    *arg = skip_type(*arg, FALSE);
	অন্যথা
	{
	    want_type = parse_type(arg, &type_list, রিক্ত, রিক্ত, TRUE);
	    যদি (want_type == রিক্ত)
	    {
		clear_type_list(&type_list);
		প্রত্যাগম FAIL;
	    }
	}

	যদি (**arg != '>')
	{
	    যদি (*skipwhite(*arg) == '>')
		semsg(_(e_no_white_space_allowed_before_str_str), ">", *arg);
	    অন্যথা
		emsg(_(e_missing_gt));
	    clear_type_list(&type_list);
	    প্রত্যাগম FAIL;
	}
	++*arg;
	*arg = skipwhite_and_linebreak(*arg, evalarg);
    }

    res = eval9(arg, rettv, evalarg, want_string);

    যদি (want_type != রিক্ত)
    {
	যদি (evaluate && res == OK)
	{
	    type_T *actual = typval2type(rettv, get_copyID(), &type_list,
							       TVTT_DO_MEMBER);

	    যদি (!equal_type(want_type, actual, 0))
	    {
		যদি (want_type->tt_type == VAR_BOOL
					&& actual->tt_type != VAR_BOOL
					&& (actual->tt_flags & TTFLAG_BOOL_OK))
		{
		    পূর্ণাঙ্ক n = tv2bool(rettv);

		    // can use "0" and "1" for boolean in some places
		    clear_tv(rettv);
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_সংখ্যা = n ? VVAL_TRUE : VVAL_FALSE;
		}
		অন্যথা
		{
		    where_T where = WHERE_INIT;

		    res = check_type(want_type, actual, TRUE, where);
		}
	    }
	}
	clear_type_list(&type_list);
    }

    প্রত্যাগম res;
}

    পূর্ণাঙ্ক
eval_leader(char_u **arg, পূর্ণাঙ্ক vim9)
{
    char_u	*s = *arg;
    char_u	*p = *arg;

    যাবত্ (*p == '!' || *p == '-' || *p == '+')
    {
	char_u *n = skipwhite(p + 1);

	// ++, --, -+ and +- are not accepted in Vim9 script
	যদি (vim9 && (*p == '-' || *p == '+') && (*n == '-' || *n == '+'))
	{
	    semsg(_(e_invalid_expression_str), s);
	    প্রত্যাগম FAIL;
	}
	p = n;
    }
    *arg = p;
    প্রত্যাগম OK;
}

/*
 * Check for a predefined value "true", "false" and "null.*".
 * Return OK when recognized.
 */
    পূর্ণাঙ্ক
handle_predefined(char_u *s, পূর্ণাঙ্ক len, typval_T *rettv)
{
    বিকল্প (len)
    {
	পক্ষ 4: যদি (STRNCMP(s, "true", 4) == 0)
		{
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_সংখ্যা = VVAL_TRUE;
		    প্রত্যাগম OK;
		}
		যদি (STRNCMP(s, "null", 4) == 0)
		{
		    rettv->v_type = VAR_SPECIAL;
		    rettv->vval.v_সংখ্যা = VVAL_NULL;
		    প্রত্যাগম OK;
		}
		ভঙ্গ;
	পক্ষ 5: যদি (STRNCMP(s, "false", 5) == 0)
		{
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_সংখ্যা = VVAL_FALSE;
		    প্রত্যাগম OK;
		}
		ভঙ্গ;
	পক্ষ 8: যদি (STRNCMP(s, "null_job", 8) == 0)
		{
#ifdef FEAT_JOB_CHANNEL
		    rettv->v_type = VAR_JOB;
		    rettv->vval.v_job = রিক্ত;
#অন্যথা
		    rettv->v_type = VAR_SPECIAL;
		    rettv->vval.v_সংখ্যা = VVAL_NULL;
#endif
		    প্রত্যাগম OK;
		}
		ভঙ্গ;
	পক্ষ 9:
		যদি (STRNCMP(s, "null_", 5) != 0)
		    ভঙ্গ;
		// null_list
		যদি (STRNCMP(s + 5, "list", 4) == 0)
		{
		    rettv->v_type = VAR_LIST;
		    rettv->vval.v_list = রিক্ত;
		    প্রত্যাগম OK;
		}
		// null_dict
		যদি (STRNCMP(s + 5, "dict", 4) == 0)
		{
		    rettv->v_type = VAR_DICT;
		    rettv->vval.v_dict = রিক্ত;
		    প্রত্যাগম OK;
		}
		// null_blob
		যদি (STRNCMP(s + 5, "blob", 4) == 0)
		{
		    rettv->v_type = VAR_BLOB;
		    rettv->vval.v_blob = রিক্ত;
		    প্রত্যাগম OK;
		}
		ভঙ্গ;
	পক্ষ 10:
		যদি (STRNCMP(s, "null_", 5) != 0)
		    ভঙ্গ;
		// null_class
		যদি (STRNCMP(s + 5, "class", 5) == 0)
		{
		    rettv->v_type = VAR_CLASS;
		    rettv->vval.v_class = রিক্ত;
		    প্রত্যাগম OK;
		}
		যদি (STRNCMP(s + 5, "tuple", 5) == 0)
		{
		    rettv->v_type = VAR_TUPLE;
		    rettv->vval.v_tuple = রিক্ত;
		    প্রত্যাগম OK;
		}
		ভঙ্গ;
	পক্ষ 11: যদি (STRNCMP(s, "null_string", 11) == 0)
		{
		    rettv->v_type = VAR_STRING;
		    rettv->vval.v_string = রিক্ত;
		    প্রত্যাগম OK;
		}
		যদি (STRNCMP(s, "null_object", 11) == 0)
		{
		    rettv->v_type = VAR_OBJECT;
		    rettv->vval.v_object = রিক্ত;
		    প্রত্যাগম OK;
		}
		ভঙ্গ;
	পক্ষ 12:
		যদি (STRNCMP(s, "null_channel", 12) == 0)
		{
#ifdef FEAT_JOB_CHANNEL
		    rettv->v_type = VAR_CHANNEL;
		    rettv->vval.v_channel = রিক্ত;
#অন্যথা
		    rettv->v_type = VAR_SPECIAL;
		    rettv->vval.v_সংখ্যা = VVAL_NULL;
#endif
		    প্রত্যাগম OK;
		}
		যদি (STRNCMP(s, "null_partial", 12) == 0)
		{
		    rettv->v_type = VAR_PARTIAL;
		    rettv->vval.v_partial = রিক্ত;
		    প্রত্যাগম OK;
		}
		ভঙ্গ;
	পক্ষ 13: যদি (STRNCMP(s, "null_function", 13) == 0)
		{
		    rettv->v_type = VAR_FUNC;
		    rettv->vval.v_string = রিক্ত;
		    প্রত্যাগম OK;
		}
		ভঙ্গ;
    }
    প্রত্যাগম FAIL;
}

/*
 * Handle register contents: @r.
 */
    স্থির শূন্য
eval9_reg_contents(
    char_u	**arg,
    typval_T	*rettv,
    পূর্ণাঙ্ক		evaluate)
{
    পূর্ণাঙ্ক		vim9script = in_vim9script();

    ++*arg;	// skip '@'
    যদি (evaluate)
    {
	যদি (vim9script && IS_WHITE_OR_NUL(**arg))
	    semsg(_(e_syntax_error_at_str), *arg);
	অন্যথা যদি (vim9script && !valid_yank_reg(**arg, FALSE))
	    emsg_invreg(**arg);
	অন্যথা
	{
	    rettv->v_type = VAR_STRING;
	    rettv->vval.v_string = get_reg_contents(**arg,
		    GREG_রাশিমালা_SRC);
	}
    }
    যদি (**arg != NUL)
	++*arg;
}

/*
 * Handle a nested expression: (expression) or lambda: (arg) => expr
 */
    স্থির পূর্ণাঙ্ক
eval9_nested_রাশিমালা(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    পূর্ণাঙ্ক		evaluate)
{
    পূর্ণাঙ্ক		ret = NOTDONE;
    পূর্ণাঙ্ক		vim9script = in_vim9script();

    যদি (vim9script)
    {
	ret = get_lambda_tv(arg, rettv, TRUE, evalarg, রিক্ত);
	যদি (ret == OK && evaluate)
	{
	    ufunc_T *ufunc = rettv->vval.v_partial->pt_func;

	    // Compile it here to get the return type.  The return
	    // type is optional, when it's missing use t_unknown.
	    // This is recognized in compile_return().
	    যদি (ufunc->uf_ret_type->tt_type == VAR_VOID)
		ufunc->uf_ret_type = &t_unknown;
	    যদি (compile_def_function(ufunc, FALSE,
				get_compile_type(ufunc), রিক্ত) == FAIL)
	    {
		clear_tv(rettv);
		ret = FAIL;
	    }
	}
    }
    যদি (ret == NOTDONE)
    {
	*arg = skipwhite_and_linebreak(*arg + 1, evalarg);
	যদি (**arg == ')')
	    // empty tuple
	    ret = eval_tuple(arg, rettv, evalarg, TRUE);
	অন্যথা
	{
	    ret = eval1(arg, rettv, evalarg);	// recursive!
	    যদি (ret != OK)
		প্রত্যাগম ret;

	    *arg = skipwhite_and_linebreak(*arg, evalarg);

	    যদি (**arg == ',')
		// tuple
		ret = eval_tuple(arg, rettv, evalarg, TRUE);
	    অন্যথা যদি (**arg == ')')
		++*arg;
	    অন্যথা যদি (ret == OK)
	    {
		emsg(_(e_missing_closing_paren));
		clear_tv(rettv);
		ret = FAIL;
	    }
	}
    }

    প্রত্যাগম ret;
}

/*
* Handle be a variable or function name.
* Can also be a curly-braces kind of name: {expr}.
*/
    স্থির পূর্ণাঙ্ক
eval9_var_func_name(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    পূর্ণাঙ্ক		evaluate,
    char_u	**name_start)
{
    char_u	*s;
    পূর্ণাঙ্ক		len;
    char_u	*alias;
    পূর্ণাঙ্ক		ret = OK;
    পূর্ণাঙ্ক		vim9script = in_vim9script();

    s = *arg;
    len = get_name_len(arg, &alias, evaluate, TRUE);
    যদি (alias != রিক্ত)
	s = alias;

    যদি (len <= 0)
	ret = FAIL;
    অন্যথা
    {
	পূর্ণাঙ্ক	flags = evalarg == রিক্ত ? 0 : evalarg->eval_flags;

	যদি (evaluate && vim9script && len == 1 && *s == '_')
	{
	    emsg(_(e_cannot_use_underscore_here));
	    ret = FAIL;
	}
	অন্যথা যদি (evaluate && vim9script && len > 2
						&& s[0] == 's' && s[1] == ':')
	{
	    semsg(_(e_cannot_use_s_colon_in_vim9_script_str), s);
	    ret = FAIL;
	}
	অন্যথা যদি ((vim9script ? **arg : *skipwhite(*arg)) == '('
				|| (vim9script && generic_func_call(arg)))
	{
	    // "name(..."  recursive!
	    *arg = skipwhite(*arg);
	    ret = eval_func(arg, evalarg, s, len, rettv, flags, রিক্ত);
	}
	অন্যথা যদি (evaluate)
	{
	    // get the value of "true", "false", etc. or a variable
	    ret = FAIL;
	    যদি (vim9script)
		ret = handle_predefined(s, len, rettv);
	    যদি (ret == FAIL)
	    {
		*name_start = s;
		ret = eval_variable(s, len, 0, rettv, রিক্ত,
					EVAL_VAR_VERBOSE + EVAL_VAR_IMPORT);

		// skip the generic function arguments (if present)
		// they are already processed by eval_variable
		যদি (ret == OK && vim9script && **arg == '<'
						&& rettv->v_type == VAR_FUNC)
		    ret = skip_generic_func_type_args(arg);
	    }
	}
	অন্যথা
	{
	    // skip the name
	    check_vars(s, len);

	    // Skip the type arguments of a generic function, its name starts
	    // with an uppercase letter.
	    যদি (vim9script && **arg == '<' && ASCII_ISUPPER(*s))
		skip_generic_func_type_args(arg);
	    ret = OK;
	}
    }
    vim_free(alias);

    প্রত্যাগম ret;
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
    স্থির পূর্ণাঙ্ক
eval9(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    পূর্ণাঙ্ক		want_string)	// after "." operator
{
    পূর্ণাঙ্ক		evaluate = evalarg != রিক্ত
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    char_u	*name_start = রিক্ত;
    char_u	*start_leader, *end_leader;
    পূর্ণাঙ্ক		ret = OK;
    স্থির পূর্ণাঙ্ক	recurse = 0;
    পূর্ণাঙ্ক		vim9script = in_vim9script();

    /*
     * Initialise variable so that clear_tv() can't mistake this for a
     * string and free a string that isn't there.
     */
    rettv->v_type = VAR_UNKNOWN;

    /*
     * Skip '!', '-' and '+' characters.  They are handled later.
     */
    start_leader = *arg;
    যদি (eval_leader(arg, vim9script) == FAIL)
	প্রত্যাগম FAIL;
    end_leader = *arg;

    যদি (**arg == '.' && (!SAFE_isdigit(*(*arg + 1)) || in_old_script(2)))
    {
	semsg(_(e_invalid_expression_str), *arg);
	++*arg;
	প্রত্যাগম FAIL;
    }

    // Limit recursion to 1000 levels.  At least at 10000 we run out of stack
    // and crash.  With MSVC the stack is smaller.
    যদি (recurse ==
#ifdef _MSC_VER
		    300
#অন্যথা
		    1000
#endif
		    )
    {
	semsg(_(e_expression_too_recursive_str), *arg);
	প্রত্যাগম FAIL;
    }
    ++recurse;

    বিকল্প (**arg)
    {
    /*
     * Number constant.
     */
    পক্ষ '0':
    পক্ষ '1':
    পক্ষ '2':
    পক্ষ '3':
    পক্ষ '4':
    পক্ষ '5':
    পক্ষ '6':
    পক্ষ '7':
    পক্ষ '8':
    পক্ষ '9':
    পক্ষ '.':	ret = eval_সংখ্যা(arg, rettv, evaluate, want_string);

		// Apply prefixed "-" and "+" now.  Matters especially when
		// "->" follows.
		যদি (ret == OK && evaluate && end_leader > start_leader
						  && rettv->v_type != VAR_BLOB)
		    ret = eval9_leader(rettv, TRUE, start_leader, &end_leader);
		ভঙ্গ;

    /*
     * String constant: "string".
     */
    পক্ষ '"':	ret = eval_string(arg, rettv, evaluate, FALSE);
		ভঙ্গ;

    /*
     * Literal string constant: 'str''ing'.
     */
    পক্ষ '\'':	ret = eval_lit_string(arg, rettv, evaluate, FALSE);
		ভঙ্গ;

    /*
     * List: [expr, expr]
     */
    পক্ষ '[':	ret = eval_list(arg, rettv, evalarg, TRUE);
		ভঙ্গ;

    /*
     * Literal Dictionary: #{key: val, key: val}
     */
    পক্ষ '#':	ret = eval_lit_dict(arg, rettv, evalarg);
		ভঙ্গ;

    /*
     * Lambda: {arg, arg -> expr}
     * Dictionary: {'key': val, 'key': val}
     */
    পক্ষ '{':	যদি (vim9script)
		    ret = NOTDONE;
		অন্যথা
		    ret = get_lambda_tv(arg, rettv, vim9script, evalarg, রিক্ত);
		যদি (ret == NOTDONE)
		    ret = eval_dict(arg, rettv, evalarg, FALSE);
		ভঙ্গ;

    /*
     * Option value: &name
     */
    পক্ষ '&':	ret = eval_option(arg, rettv, evaluate);
		ভঙ্গ;

    /*
     * Environment variable: $VAR.
     * Interpolated string: $"string" or $'string'.
     */
    পক্ষ '$':	যদি ((*arg)[1] == '"' || (*arg)[1] == '\'')
		    ret = eval_interp_string(arg, rettv, evaluate);
		অন্যথা
		    ret = eval_env_var(arg, rettv, evaluate);
		ভঙ্গ;

    /*
     * Register contents: @r.
     */
    পক্ষ '@':	eval9_reg_contents(arg, rettv, evaluate);
		ভঙ্গ;

    /*
     * nested expression: (expression).
     * or lambda: (arg) => expr
     * or tuple
     */
    পক্ষ '(':	ret = eval9_nested_রাশিমালা(arg, rettv, evalarg, evaluate);
		ভঙ্গ;

    স্বভাব:	ret = NOTDONE;
		ভঙ্গ;
    }

    যদি (ret == NOTDONE)
    {
	/*
	 * Must be a variable or function name.
	 * Can also be a curly-braces kind of name: {expr}.
	 */
	ret = eval9_var_func_name(arg, rettv, evalarg, evaluate, &name_start);
    }

    // Handle following '[', '(' and '.' for expr[expr], expr.name,
    // expr(expr), expr->name(expr)
    যদি (ret == OK)
	ret = handle_subscript(arg, name_start, rettv, evalarg, evaluate);

    /*
     * Apply logical NOT and unary '-', from right to left, ignore '+'.
     */
    যদি (ret == OK && evaluate && end_leader > start_leader)
	ret = eval9_leader(rettv, FALSE, start_leader, &end_leader);

    --recurse;
    প্রত্যাগম ret;
}

/*
 * Apply the leading "!" and "-" before an eval9 expression to "rettv".
 * When "numeric_only" is TRUE only handle "+" and "-".
 * Adjusts "end_leaderp" until it is at "start_leader".
 */
    স্থির পূর্ণাঙ্ক
eval9_leader(
	typval_T    *rettv,
	পূর্ণাঙ্ক	    numeric_only,
	char_u	    *start_leader,
	char_u	    **end_leaderp)
{
    char_u	*end_leader = *end_leaderp;
    পূর্ণাঙ্ক		ret = OK;
    পূর্ণাঙ্ক		error = FALSE;
    varnumber_T val = 0;
    vartype_T	type = rettv->v_type;
    পূর্ণাঙ্ক		vim9script = in_vim9script();
    float_T	    f = 0.0;

    যদি (rettv->v_type == VAR_FLOAT)
	f = rettv->vval.v_float;
    অন্যথা
    {
	যাবত্ (VIM_ISWHITE(end_leader[-1]))
	    --end_leader;
	যদি (vim9script && end_leader[-1] == '!')
	{
	    // Is this typeval supported with the ! operator?
	    যদি (check_typval_is_মান(rettv) == FAIL)
	    {
		clear_tv(rettv);
		প্রত্যাগম FAIL;
	    }
	    val = tv2bool(rettv);
	}
	অন্যথা
	    val = tv_get_সংখ্যা_chk(rettv, &error);
    }
    যদি (error)
    {
	clear_tv(rettv);
	ret = FAIL;
    }
    অন্যথা
    {
	যাবত্ (end_leader > start_leader)
	{
	    --end_leader;
	    যদি (*end_leader == '!')
	    {
		যদি (numeric_only)
		{
		    ++end_leader;
		    ভঙ্গ;
		}
		যদি (rettv->v_type == VAR_FLOAT)
		{
		    rettv->v_type = VAR_BOOL;
		    val = f == 0.0 ? VVAL_TRUE : VVAL_FALSE;
		    type = VAR_BOOL;
		}
		অন্যথা
		{
		    val = !val;
		    type = VAR_BOOL;
		}
	    }
	    অন্যথা যদি (*end_leader == '-')
	    {
		যদি (rettv->v_type == VAR_FLOAT)
		    f = -f;
		অন্যথা
		{
		    val = -val;
		    type = VAR_সংখ্যা;
		}
	    }
	}
	যদি (rettv->v_type == VAR_FLOAT)
	{
	    clear_tv(rettv);
	    rettv->vval.v_float = f;
	}
	অন্যথা
	{
	    clear_tv(rettv);
	    যদি (vim9script)
		rettv->v_type = type;
	    অন্যথা
		rettv->v_type = VAR_সংখ্যা;
	    rettv->vval.v_সংখ্যা = val;
	}
    }
    *end_leaderp = end_leader;
    প্রত্যাগম ret;
}

/*
 * Call the function referred to in "rettv".
 */
    স্থির পূর্ণাঙ্ক
call_func_rettv(
	char_u	    **arg,
	evalarg_T   *evalarg,
	typval_T    *rettv,
	পূর্ণাঙ্ক	    evaluate,
	dict_T	    *selfdict,
	typval_T    *basetv)
{
    partial_T	*pt = রিক্ত;
    funcexe_T	funcexe;
    typval_T	functv;
    char_u	*s;
    পূর্ণাঙ্ক		ret;

    // need to copy the funcref so that we can clear rettv
    যদি (evaluate)
    {
	functv = *rettv;
	rettv->v_type = VAR_UNKNOWN;

	// Invoke the function.  Recursive!
	যদি (functv.v_type == VAR_PARTIAL)
	{
	    pt = functv.vval.v_partial;
	    s = partial_name(pt);
	}
	অন্যথা
	{
	    s = functv.vval.v_string;
	    যদি (s == রিক্ত || *s == NUL)
	    {
		emsg(_(e_empty_function_name));
		ret = FAIL;
		গচ্ছ theend;
	    }
	}
    }
    অন্যথা
	s = (char_u *)"";

    CLEAR_FIELD(funcexe);
    funcexe.fe_firstline = curwin->w_cursor.lnum;
    funcexe.fe_lastline = curwin->w_cursor.lnum;
    funcexe.fe_evaluate = evaluate;
    funcexe.fe_partial = pt;
    যদি (pt != রিক্ত)
    {
	funcexe.fe_object = pt->pt_obj;
	যদি (funcexe.fe_object != রিক্ত)
	    ++funcexe.fe_object->obj_refcount;
    }
    funcexe.fe_selfdict = selfdict;
    funcexe.fe_basetv = basetv;
    যদি (evalarg != রিক্ত)
	funcexe.fe_cctx = evalarg->eval_cctx;
    ret = get_func_tv(s, -1, rettv, arg, evalarg, &funcexe);

theend:
    // Clear the funcref afterwards, so that deleting it while
    // evaluating the arguments is possible (see test55).
    যদি (evaluate)
	clear_tv(&functv);

    প্রত্যাগম ret;
}

/*
 * Evaluate "->method()".
 * "*arg" points to "method".
 * Returns FAIL or OK. "*arg" is advanced to after the ')'.
 */
    স্থির পূর্ণাঙ্ক
eval_lambda(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    পূর্ণাঙ্ক		verbose)	// give error messages
{
    পূর্ণাঙ্ক		evaluate = evalarg != রিক্ত
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    typval_T	base = *rettv;
    পূর্ণাঙ্ক		ret;

    rettv->v_type = VAR_UNKNOWN;

    যদি (**arg == '{')
    {
	// ->{lambda}()
	ret = get_lambda_tv(arg, rettv, FALSE, evalarg, রিক্ত);
    }
    অন্যথা
    {
	// ->(lambda)()
	++*arg;
	ret = eval1(arg, rettv, evalarg);
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	যদি (**arg != ')')
	{
	    emsg(_(e_missing_closing_paren));
	    প্রত্যাগম FAIL;
	}
	যদি (rettv->v_type != VAR_STRING && rettv->v_type != VAR_FUNC
					       && rettv->v_type != VAR_PARTIAL)
	{
	    emsg(_(e_string_or_function_required_for_arrow_parens_রাশিমালা));
	    প্রত্যাগম FAIL;
	}
	++*arg;
    }
    যদি (ret != OK)
	প্রত্যাগম FAIL;

    যদি (**arg != '(')
    {
	যদি (verbose)
	{
	    যদি (*skipwhite(*arg) == '(')
		emsg(_(e_no_white_space_allowed_before_parenthesis));
	    অন্যথা
		semsg(_(e_missing_parenthesis_str), "lambda");
	}
	clear_tv(rettv);
	ret = FAIL;
    }
    অন্যথা
	ret = call_func_rettv(arg, evalarg, rettv, evaluate, রিক্ত, &base);

    // Clear the funcref afterwards, so that deleting it while
    // evaluating the arguments is possible (see test55).
    যদি (evaluate)
	clear_tv(&base);

    প্রত্যাগম ret;
}

/*
 * Evaluate "->method()".
 * "*arg" points to "method".
 * Returns FAIL or OK. "*arg" is advanced to after the ')'.
 */
    স্থির পূর্ণাঙ্ক
eval_method(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    পূর্ণাঙ্ক		verbose)	// give error messages
{
    char_u	*name;
    long	len;
    char_u	*alias;
    char_u	*tofree = রিক্ত;
    typval_T	base = *rettv;
    পূর্ণাঙ্ক		ret = OK;
    পূর্ণাঙ্ক		evaluate = evalarg != রিক্ত
				      && (evalarg->eval_flags & EVAL_EVALUATE);

    rettv->v_type = VAR_UNKNOWN;

    name = *arg;
    len = get_name_len(arg, &alias, evaluate, evaluate);
    যদি (alias != রিক্ত)
	name = alias;

    যদি (len <= 0)
    {
	যদি (verbose)
	    emsg(_(e_missing_name_after_method));
	ret = FAIL;
    }
    অন্যথা
    {
	char_u *paren;

	// If there is no "(" immediately following, but there is further on,
	// it can be "import.Func()", "dict.Func()", "list[nr]", etc.
	// Does not handle anything where "(" is part of the expression.
	*arg = skipwhite(*arg);

	যদি (**arg != '(' && alias == রিক্ত
				    && (paren = vim_strchr(*arg, '(')) != রিক্ত)
	{
	    *arg = name;

	    // Truncate the name at the "(".  Avoid trying to get another line
	    // by making "getline" NULL.
	    *paren = NUL;
	    char_u	*(*getline)(পূর্ণাঙ্ক, শূন্য *, পূর্ণাঙ্ক, getline_opt_T) = রিক্ত;
	    যদি (evalarg != রিক্ত)
	    {
		getline = evalarg->eval_getline;
		evalarg->eval_getline = রিক্ত;
	    }

	    char_u *deref = deref_function_name(arg, &tofree, evalarg, verbose);
	    যদি (deref == রিক্ত)
	    {
		*arg = name + len;
		ret = FAIL;
	    }
	    অন্যথা
	    {
		name = deref;
		len = (long)STRLEN(name);
	    }

	    *paren = '(';
	    যদি (getline != রিক্ত)
		evalarg->eval_getline = getline;
	}

	যদি (ret == OK)
	{
	    *arg = skipwhite(*arg);

	    যদি (**arg != '(')
	    {
		যদি (verbose)
		    semsg(_(e_missing_parenthesis_str), name);
		ret = FAIL;
	    }
	    অন্যথা যদি (VIM_ISWHITE((*arg)[-1]))
	    {
		যদি (verbose)
		    emsg(_(e_no_white_space_allowed_before_parenthesis));
		ret = FAIL;
	    }
	    অন্যথা
		ret = eval_func(arg, evalarg, name, len, rettv,
					  evaluate ? EVAL_EVALUATE : 0, &base);
	}
    }

    // Clear the funcref afterwards, so that deleting it while
    // evaluating the arguments is possible (see test55).
    যদি (evaluate)
	clear_tv(&base);
    vim_free(tofree);

    যদি (alias != রিক্ত)
	vim_free(alias);

    প্রত্যাগম ret;
}

/*
 * Evaluate an "[expr]" or "[expr:expr]" index.  Also "dict.key".
 * "*arg" points to the '[' or '.'.
 * Returns FAIL or OK. "*arg" is advanced to after the ']'.
 */
    স্থির পূর্ণাঙ্ক
eval_index(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    পূর্ণাঙ্ক		verbose)	// give error messages
{
    পূর্ণাঙ্ক		evaluate = evalarg != রিক্ত
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    পূর্ণাঙ্ক		empty1 = FALSE, empty2 = FALSE;
    typval_T	var1, var2;
    পূর্ণাঙ্ক		range = FALSE;
    char_u	*চাবি = রিক্ত;
    পূর্ণাঙ্ক		keylen = -1;
    পূর্ণাঙ্ক		vim9script = in_vim9script();

    যদি (check_can_index(rettv, evaluate, verbose) == FAIL)
	প্রত্যাগম FAIL;

    init_tv(&var1);
    init_tv(&var2);
    যদি (**arg == '.')
    {
	/*
	 * dict.name
	 */
	চাবি = *arg + 1;
	প্রতি (keylen = 0; eval_isdictc(চাবি[keylen]); ++keylen)
	    ;
	যদি (keylen == 0)
	    প্রত্যাগম FAIL;
	যদি (vim9script && চাবি[keylen] == '<')
	{
	    // skip generic type arguments
	    char_u	*p = &চাবি[keylen];

	    যদি (skip_generic_func_type_args(&p) == FAIL)
		প্রত্যাগম FAIL;
	    keylen = p - চাবি;
	}
	*arg = চাবি + keylen;
    }
    অন্যথা
    {
	/*
	 * something[idx]
	 *
	 * Get the (first) variable from inside the [].
	 */
	*arg = skipwhite_and_linebreak(*arg + 1, evalarg);
	যদি (**arg == ':')
	    empty1 = TRUE;
	অন্যথা যদি (eval1(arg, &var1, evalarg) == FAIL)	// recursive!
	    প্রত্যাগম FAIL;
	অন্যথা যদি (vim9script && **arg == ':')
	{
	    semsg(_(e_white_space_required_before_and_after_str_at_str),
								    ":", *arg);
	    clear_tv(&var1);
	    প্রত্যাগম FAIL;
	}
	অন্যথা যদি (evaluate)
	{
	    পূর্ণাঙ্ক error = FALSE;

	    // allow for indexing with float
	    যদি (vim9script && rettv->v_type == VAR_DICT
						   && var1.v_type == VAR_FLOAT)
	    {
		var1.vval.v_string = typval_tostring(&var1, TRUE);
		var1.v_type = VAR_STRING;
	    }

	    যদি (vim9script && (rettv->v_type == VAR_LIST
						|| rettv->v_type == VAR_TUPLE))
		tv_get_সংখ্যা_chk(&var1, &error);
	    অন্যথা
		error = tv_get_string_chk(&var1) == রিক্ত;
	    যদি (error)
	    {
		// not a number or string
		clear_tv(&var1);
		প্রত্যাগম FAIL;
	    }
	}

	/*
	 * Get the second variable from inside the [:].
	 */
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	যদি (**arg == ':')
	{
	    range = TRUE;
	    ++*arg;
	    যদি (vim9script && !IS_WHITE_OR_NUL(**arg) && **arg != ']')
	    {
		semsg(_(e_white_space_required_before_and_after_str_at_str),
								":", *arg - 1);
		যদি (!empty1)
		    clear_tv(&var1);
		প্রত্যাগম FAIL;
	    }
	    *arg = skipwhite_and_linebreak(*arg, evalarg);
	    যদি (**arg == ']')
		empty2 = TRUE;
	    অন্যথা যদি (eval1(arg, &var2, evalarg) == FAIL)	// recursive!
	    {
		যদি (!empty1)
		    clear_tv(&var1);
		প্রত্যাগম FAIL;
	    }
	    অন্যথা যদি (evaluate && tv_get_string_chk(&var2) == রিক্ত)
	    {
		// not a number or string
		যদি (!empty1)
		    clear_tv(&var1);
		clear_tv(&var2);
		প্রত্যাগম FAIL;
	    }
	}

	// Check for the ']'.
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	যদি (**arg != ']')
	{
	    যদি (verbose)
		emsg(_(e_missing_closing_square_brace));
	    clear_tv(&var1);
	    যদি (range)
		clear_tv(&var2);
	    প্রত্যাগম FAIL;
	}
	*arg = *arg + 1;	// skip over the ']'
    }

    যদি (evaluate)
    {
	পূর্ণাঙ্ক res = eval_index_inner(rettv, range,
		empty1 ? রিক্ত : &var1, empty2 ? রিক্ত : &var2, FALSE,
		চাবি, keylen, verbose);

	যদি (!empty1)
	    clear_tv(&var1);
	যদি (range)
	    clear_tv(&var2);
	প্রত্যাগম res;
    }
    প্রত্যাগম OK;
}

/*
 * Check if "rettv" can have an [index] or [sli:ce]
 */
    পূর্ণাঙ্ক
check_can_index(typval_T *rettv, পূর্ণাঙ্ক evaluate, পূর্ণাঙ্ক verbose)
{
    বিকল্প (rettv->v_type)
    {
	পক্ষ VAR_FUNC:
	পক্ষ VAR_PARTIAL:
	    যদি (verbose)
		emsg(_(e_cannot_index_a_funcref));
	    প্রত্যাগম FAIL;
	পক্ষ VAR_FLOAT:
	    যদি (verbose)
		emsg(_(e_using_float_as_string));
	    প্রত্যাগম FAIL;
	পক্ষ VAR_BOOL:
	পক্ষ VAR_SPECIAL:
	পক্ষ VAR_JOB:
	পক্ষ VAR_CHANNEL:
	পক্ষ VAR_INSTR:
	পক্ষ VAR_OBJECT:
	    যদি (verbose)
		emsg(_(e_cannot_index_special_variable));
	    প্রত্যাগম FAIL;
	পক্ষ VAR_CLASS:
	পক্ষ VAR_TYPEALIAS:
	    যদি (verbose)
		check_typval_is_মান(rettv);
	    প্রত্যাগম FAIL;
	পক্ষ VAR_UNKNOWN:
	পক্ষ VAR_ANY:
	পক্ষ VAR_VOID:
	    যদি (evaluate)
	    {
		emsg(_(e_cannot_index_special_variable));
		প্রত্যাগম FAIL;
	    }
	    // FALLTHROUGH

	পক্ষ VAR_STRING:
	পক্ষ VAR_LIST:
	পক্ষ VAR_TUPLE:
	পক্ষ VAR_DICT:
	পক্ষ VAR_BLOB:
	    ভঙ্গ;
	পক্ষ VAR_সংখ্যা:
	    যদি (in_vim9script())
		emsg(_(e_cannot_index_সংখ্যা));
	    ভঙ্গ;
    }
    প্রত্যাগম OK;
}

/*
 * Apply index or range to "rettv".
 * "var1" is the first index, NULL for [:expr].
 * "var2" is the second index, NULL for [expr] and [expr: ]
 * "exclusive" is TRUE for slice(): second index is exclusive, use character
 * index for string.
 * Alternatively, "key" is not NULL, then key[keylen] is the dict index.
 */
    পূর্ণাঙ্ক
eval_index_inner(
	typval_T    *rettv,
	পূর্ণাঙ্ক	    is_range,
	typval_T    *var1,
	typval_T    *var2,
	পূর্ণাঙ্ক	    exclusive,
	char_u	    *চাবি,
	পূর্ণাঙ্ক	    keylen,
	পূর্ণাঙ্ক	    verbose)
{
    varnumber_T	    n1, n2 = 0;
    long	    len;

    n1 = 0;
    যদি (var1 != রিক্ত && rettv->v_type != VAR_DICT)
	n1 = tv_get_সংখ্যা(var1);

    যদি (is_range)
    {
	যদি (rettv->v_type == VAR_DICT)
	{
	    যদি (verbose)
		emsg(_(e_cannot_slice_dictionary));
	    প্রত্যাগম FAIL;
	}
	যদি (var2 != রিক্ত)
	    n2 = tv_get_সংখ্যা(var2);
	অন্যথা
	    n2 = VARNUM_MAX;
    }

    বিকল্প (rettv->v_type)
    {
	পক্ষ VAR_UNKNOWN:
	পক্ষ VAR_ANY:
	পক্ষ VAR_VOID:
	পক্ষ VAR_FUNC:
	পক্ষ VAR_PARTIAL:
	পক্ষ VAR_FLOAT:
	পক্ষ VAR_BOOL:
	পক্ষ VAR_SPECIAL:
	পক্ষ VAR_JOB:
	পক্ষ VAR_CHANNEL:
	পক্ষ VAR_INSTR:
	পক্ষ VAR_CLASS:
	পক্ষ VAR_OBJECT:
	পক্ষ VAR_TYPEALIAS:
	    ভঙ্গ; // not evaluating, skipping over subscript

	পক্ষ VAR_সংখ্যা:
	পক্ষ VAR_STRING:
	    {
		char_u	*s = tv_get_string(rettv);

		len = (long)STRLEN(s);
		যদি (in_vim9script() || exclusive)
		{
		    যদি (is_range)
			s = string_slice(s, n1, n2, exclusive);
		    অন্যথা
			s = char_from_string(s, n1);
		}
		অন্যথা যদি (is_range)
		{
		    // The resulting variable is a substring.  If the indexes
		    // are out of range the result is empty.
		    যদি (n1 < 0)
		    {
			n1 = len + n1;
			যদি (n1 < 0)
			    n1 = 0;
		    }
		    যদি (n2 < 0)
			n2 = len + n2;
		    অন্যথা যদি (n2 >= len)
			n2 = len;
		    যদি (n1 >= len || n2 < 0 || n1 > n2)
			s = রিক্ত;
		    অন্যথা
			s = vim_strnsave(s + n1, n2 - n1 + 1);
		}
		অন্যথা
		{
		    // The resulting variable is a string of a single
		    // character.  If the index is too big or negative the
		    // result is empty.
		    যদি (n1 >= len || n1 < 0)
			s = রিক্ত;
		    অন্যথা
			s = vim_strnsave(s + n1, 1);
		}
		clear_tv(rettv);
		rettv->v_type = VAR_STRING;
		rettv->vval.v_string = s;
	    }
	    ভঙ্গ;

	পক্ষ VAR_BLOB:
	    blob_slice_or_index(rettv->vval.v_blob, is_range, n1, n2,
							     exclusive, rettv);
	    ভঙ্গ;

	পক্ষ VAR_LIST:
	    যদি (var1 == রিক্ত)
		n1 = 0;
	    যদি (var2 == রিক্ত)
		n2 = VARNUM_MAX;
	    যদি (list_slice_or_index(rettv->vval.v_list,
			  is_range, n1, n2, exclusive, rettv, verbose) == FAIL)
		প্রত্যাগম FAIL;
	    ভঙ্গ;

	পক্ষ VAR_TUPLE:
	    যদি (var1 == রিক্ত)
		n1 = 0;
	    যদি (var2 == রিক্ত)
		n2 = VARNUM_MAX;
	    যদি (tuple_slice_or_index(rettv->vval.v_tuple,
			  is_range, n1, n2, exclusive, rettv, verbose) == FAIL)
		প্রত্যাগম FAIL;
	    ভঙ্গ;

	পক্ষ VAR_DICT:
	    {
		dictitem_T	*item;
		typval_T	tmp;

		যদি (চাবি == রিক্ত)
		{
		    চাবি = tv_get_string_chk(var1);
		    যদি (চাবি == রিক্ত)
			প্রত্যাগম FAIL;
		}

		item = dict_find(rettv->vval.v_dict, চাবি, keylen);

		যদি (item == রিক্ত)
		{
		    যদি (verbose)
		    {
			যদি (keylen > 0)
			    চাবি[keylen] = NUL;
			semsg(_(e_চাবি_not_present_in_dictionary_str), চাবি);
		    }
		    প্রত্যাগম FAIL;
		}

		copy_tv(&item->di_tv, &tmp);
		clear_tv(rettv);
		*rettv = tmp;
	    }
	    ভঙ্গ;
    }
    প্রত্যাগম OK;
}

/*
 * Return the function name of partial "pt".
 */
    char_u *
partial_name(partial_T *pt)
{
    যদি (pt != রিক্ত)
    {
	যদি (pt->pt_name != রিক্ত)
	    প্রত্যাগম pt->pt_name;
	যদি (pt->pt_func != রিক্ত)
	    প্রত্যাগম pt->pt_func->uf_name;
    }
    প্রত্যাগম (char_u *)"";
}

    স্থির শূন্য
partial_free(partial_T *pt)
{
    পূর্ণাঙ্ক i;

    প্রতি (i = 0; i < pt->pt_argc; ++i)
	clear_tv(&pt->pt_argv[i]);
    vim_free(pt->pt_argv);
    dict_unref(pt->pt_dict);
    যদি (pt->pt_name != রিক্ত)
    {
	func_unref(pt->pt_name);
	vim_free(pt->pt_name);
    }
    অন্যথা
	func_ptr_unref(pt->pt_func);
    যদি (pt->pt_obj != রিক্ত)
	object_unref(pt->pt_obj);

    // "out_up" is no longer used, decrement refcount on partial that owns it.
    partial_unref(pt->pt_outer.out_up_partial);

    // Using pt_outer from another partial.
    partial_unref(pt->pt_outer_partial);

    // Decrease the reference count for the context of a closure.  If down
    // to the minimum it may be time to free it.
    যদি (pt->pt_funcstack != রিক্ত)
    {
	--pt->pt_funcstack->fs_refcount;
	funcstack_check_refcount(pt->pt_funcstack);
    }
    // Similarly for loop variables.
    প্রতি (i = 0; i < MAX_LOOP_DEPTH; ++i)
	যদি (pt->pt_loopvars[i] != রিক্ত)
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
    শূন্য
partial_unref(partial_T *pt)
{
    যদি (pt == রিক্ত)
	প্রত্যাগম;

    পূর্ণাঙ্ক	done = FALSE;

    যদি (--pt->pt_refcount <= 0)
	partial_free(pt);

    // If the reference count goes down to one, the funcstack may be the
    // only reference and can be freed if no other partials reference it.
    অন্যথা যদি (pt->pt_refcount == 1)
    {
	// careful: if the funcstack is freed it may contain this partial
	// and it gets freed as well
	যদি (pt->pt_funcstack != রিক্ত)
	    done = funcstack_check_refcount(pt->pt_funcstack);

	যদি (!done)
	{
	    পূর্ণাঙ্ক	depth;

	    প্রতি (depth = 0; depth < MAX_LOOP_DEPTH; ++depth)
		যদি (pt->pt_loopvars[depth] != রিক্ত
			&& loopvars_check_refcount(pt->pt_loopvars[depth]))
		    ভঙ্গ;
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
    স্থির char_u *
string_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    পূর্ণাঙ্ক		echo_style,
    পূর্ণাঙ্ক		composite_val)
{
    char_u	*r = রিক্ত;

    যদি (echo_style && !composite_val)
    {
	*tofree = রিক্ত;
	r = tv->vval.v_string;
	যদি (r == রিক্ত)
	    r = (char_u *)"";
    }
    অন্যথা
    {
	*tofree = string_quote(tv->vval.v_string, FALSE);
	r = *tofree;
    }

    প্রত্যাগম r;
}

/*
 * Return a textual representation of a function in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "echo_style" is FALSE, put quotes around the function name as
 * "function()", otherwise does not put quotes around function name.
 * May return NULL.
 */
    স্থির char_u *
func_tv2string(typval_T *tv, char_u **tofree, পূর্ণাঙ্ক echo_style)
{
    char_u	*r = রিক্ত;
    char_u	buf[MAX_FUNC_NAME_LEN];

    যদি (echo_style)
    {
	*tofree = রিক্ত;

	যদি (tv->vval.v_string == রিক্ত)
	    r = (char_u *)"function()";
	অন্যথা
	{
	    r = make_ufunc_name_readable(tv->vval.v_string, buf,
							MAX_FUNC_NAME_LEN);
	    যদি (r == buf)
		r = *tofree = vim_strsave(buf);
	}
    }
    অন্যথা
    {
	char_u *s = রিক্ত;

	যদি (tv->vval.v_string != রিক্ত)
	    s = make_ufunc_name_readable(tv->vval.v_string, buf,
							MAX_FUNC_NAME_LEN);

	r = *tofree = string_quote(s, TRUE);
    }

    প্রত্যাগম r;
}

/*
 * Return a textual representation of the object method in "tv", a VAR_PARTIAL.
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "echo_style" is FALSE, put quotes around the function name as
 * "function()", otherwise does not put quotes around function name.
 * May return NULL.
 */
    স্থির char_u *
method_tv2string(typval_T *tv, char_u **tofree, পূর্ণাঙ্ক echo_style)
{
    char_u	buf[MAX_FUNC_NAME_LEN];
    partial_T	*pt = tv->vval.v_partial;

    size_t len = vim_snprintf((বর্ণ *)buf, পরিমাণ(buf), "<SNR>%d_%s.%s",
			   pt->pt_func->uf_script_ctx.sc_sid,
			   pt->pt_func->uf_class->class_name.string,
			   pt->pt_func->uf_name);
    যদি (len >= পরিমাণ(buf))
    {
	যদি (echo_style)
	{
	    *tofree = রিক্ত;
	    প্রত্যাগম (char_u *)"function()";
	}
	অন্যথা
	    প্রত্যাগম *tofree = string_quote((char_u*)"", TRUE);
    }

    প্রত্যাগম *tofree = echo_style ? vim_strsave(buf) : string_quote(buf, TRUE);
}

/*
 * Return a textual representation of a partial in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * "numbuf" is used for a number.  May return NULL.
 */
    স্থির char_u *
partial_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    char_u	*numbuf,
    পূর্ণাঙ্ক		copyID)
{
    char_u	*r = রিক্ত;
    partial_T	*pt;
    char_u	*fname;
    garray_T	ga;
    পূর্ণাঙ্ক		i;
    char_u	*tf;

    pt = tv->vval.v_partial;
    fname = string_quote(pt == রিক্ত ? রিক্ত : partial_name(pt), FALSE);

    ga_init2(&ga, 1, 100);
    GA_CONCAT_LITERAL(&ga, "function(");
    যদি (fname != রিক্ত)
    {
	// When using uf_name prepend "g:" for a global function.
	যদি (pt != রিক্ত && pt->pt_name == রিক্ত && fname[0] == '\''
						&& vim_isupper(fname[1]))
	{
	    GA_CONCAT_LITERAL(&ga, "'g:");
	    ga_concat(&ga, fname + 1);
	}
	অন্যথা
	    ga_concat(&ga, fname);
	vim_free(fname);
    }
    যদি (pt != রিক্ত && pt->pt_argc > 0)
    {
	GA_CONCAT_LITERAL(&ga, ", [");
	প্রতি (i = 0; i < pt->pt_argc; ++i)
	{
	    যদি (i > 0)
		GA_CONCAT_LITERAL(&ga, ", ");
	    ga_concat(&ga, tv2string(&pt->pt_argv[i], &tf, numbuf, copyID));
	    vim_free(tf);
	}
	GA_CONCAT_LITERAL(&ga, "]");
    }
    যদি (pt != রিক্ত && pt->pt_dict != রিক্ত)
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

    প্রত্যাগম r;
}

/*
 * Return a textual representation of a List in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "copyID" is not zero replace recursive lists with "...".  When
 * "restore_copyID" is FALSE, repeated items in lists are replaced with "...".
 * May return NULL.
 */
    স্থির char_u *
list_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    পূর্ণাঙ্ক		copyID,
    পূর্ণাঙ্ক		restore_copyID)
{
    char_u	*r = রিক্ত;

    যদি (tv->vval.v_list == রিক্ত)
    {
	// NULL list is equivalent to empty list.
	*tofree = রিক্ত;
	r = (char_u *)"[]";
    }
    অন্যথা যদি (copyID != 0 && tv->vval.v_list->lv_copyID == copyID
	    && tv->vval.v_list->lv_len > 0)
    {
	*tofree = রিক্ত;
	r = (char_u *)"[...]";
    }
    অন্যথা
    {
	পূর্ণাঙ্ক old_copyID;
	যদি (restore_copyID)
	    old_copyID = tv->vval.v_list->lv_copyID;

	tv->vval.v_list->lv_copyID = copyID;
	*tofree = list2string(tv, copyID, restore_copyID);
	যদি (restore_copyID)
	    tv->vval.v_list->lv_copyID = old_copyID;
	r = *tofree;
    }

    প্রত্যাগম r;
}

/*
 * Return a textual representation of a Tuple in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "copyID" is not zero replace recursive lists with "...".  When
 * "restore_copyID" is FALSE, repeated items in tuples are replaced with "...".
 * May return NULL.
 */
    স্থির char_u *
tuple_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    পূর্ণাঙ্ক		copyID,
    পূর্ণাঙ্ক		restore_copyID)
{
    tuple_T	*tuple = tv->vval.v_tuple;
    char_u	*r = রিক্ত;

    যদি (tuple == রিক্ত)
    {
	// NULL tuple is equivalent to an empty tuple.
	*tofree = রিক্ত;
	r = (char_u *)"()";
    }
    অন্যথা যদি (copyID != 0 && tuple->tv_copyID == copyID
					&& tuple->tv_বস্তুসমূহ.ga_len > 0)
    {
	*tofree = রিক্ত;
	r = (char_u *)"(...)";
    }
    অন্যথা
    {
	পূর্ণাঙ্ক old_copyID;
	যদি (restore_copyID)
	    old_copyID = tuple->tv_copyID;

	tuple->tv_copyID = copyID;
	*tofree = tuple2string(tv, copyID, restore_copyID);
	যদি (restore_copyID)
	    tuple->tv_copyID = old_copyID;
	r = *tofree;
    }

    প্রত্যাগম r;
}

/*
 * Return a textual representation of a Dict in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "copyID" is not zero replace recursive dicts with "...".
 * When "restore_copyID" is FALSE, repeated items in the dictionary are
 * replaced with "...".  May return NULL.
 */
    স্থির char_u *
dict_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    পূর্ণাঙ্ক		copyID,
    পূর্ণাঙ্ক		restore_copyID)
{
    char_u	*r = রিক্ত;

    যদি (tv->vval.v_dict == রিক্ত)
    {
	// NULL dict is equivalent to empty dict.
	*tofree = রিক্ত;
	r = (char_u *)"{}";
    }
    অন্যথা যদি (copyID != 0 && tv->vval.v_dict->dv_copyID == copyID
	    && tv->vval.v_dict->dv_hashtab.ht_used != 0)
    {
	*tofree = রিক্ত;
	r = (char_u *)"{...}";
    }
    অন্যথা
    {
	পূর্ণাঙ্ক old_copyID;
	যদি (restore_copyID)
	    old_copyID = tv->vval.v_dict->dv_copyID;

	tv->vval.v_dict->dv_copyID = copyID;
	*tofree = dict2string(tv, copyID, restore_copyID);
	যদি (restore_copyID)
	    tv->vval.v_dict->dv_copyID = old_copyID;
	r = *tofree;
    }

    প্রত্যাগম r;
}

/*
 * Return a textual representation of a job or a channel in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * "numbuf" is used for a number.
 * When "composite_val" is FALSE, put quotes around strings as "string()",
 * otherwise does not put quotes around strings.
 * May return NULL.
 */
    স্থির char_u *
jobchan_tv2string(
    typval_T	*tv UNUSED,
    char_u	**tofree UNUSED,
    char_u	*numbuf UNUSED,
    পূর্ণাঙ্ক		composite_val UNUSED)
{
    char_u	*r = রিক্ত;

#ifdef FEAT_JOB_CHANNEL
    *tofree = রিক্ত;

    যদি (tv->v_type == VAR_JOB)
	r = job_to_string_buf(tv, numbuf);
    অন্যথা
	r = channel_to_string_buf(tv, numbuf);

    যদি (composite_val)
    {
	*tofree = string_quote(r, FALSE);
	r = *tofree;
    }
#endif

    প্রত্যাগম r;
}

/*
 * Return a textual representation of a class in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * May return NULL.
 */
    স্থির char_u *
class_tv2string(typval_T *tv, char_u **tofree)
{
    char_u	*r = রিক্ত;
    size_t	rsize;
    class_T	*cl = tv->vval.v_class;
    string_T	class_name = {(char_u *)"[unknown]", 9};
    string_T	s = {(char_u *)"class", 5};

    যদি (cl != রিক্ত)
    {
	class_name.string = cl->class_name.string;
	class_name.length = cl->class_name.length;
	যদি (IS_INTERFACE(cl))
	    STR_LITERAL_SET(s, "interface");
	অন্যথা যদি (IS_ENUM(cl))
	    STR_LITERAL_SET(s, "enum");
    }

    rsize = s.length + 1 + class_name.length + 1;
    r = *tofree = alloc(rsize);
    যদি (r != রিক্ত)
	vim_snprintf((বর্ণ *)r, rsize, "%s %s", s.string, (বর্ণ *)class_name.string);

    প্রত্যাগম r;
}

/*
 * Return a textual representation of an Object in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "copyID" is not zero replace recursive object with "...".
 * When "restore_copyID" is FALSE, repeated items in the object are
 * replaced with "...".  May return NULL.
 */
    স্থির char_u *
object_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    পূর্ণাঙ্ক		copyID,
    পূর্ণাঙ্ক		restore_copyID,
    char_u	*numbuf,
    পূর্ণাঙ্ক		echo_style,
    পূর্ণাঙ্ক		composite_val)
{
    char_u	*r = রিক্ত;

    object_T	*obj = tv->vval.v_object;
    যদি (obj == রিক্ত || obj->obj_class == রিক্ত)
    {
	*tofree = রিক্ত;
	r = (char_u *)"object of [unknown]";
    }
    অন্যথা যদি (copyID != 0 && obj->obj_copyID == copyID
	    && obj->obj_class->class_obj_member_গুনতি != 0)
    {
	size_t n = 25 + obj->obj_class->class_name.length;
	r = alloc(n);
	যদি (r != রিক্ত)
	    (শূন্য)vim_snprintf((বর্ণ *)r, n, "object of %s {...}",
						obj->obj_class->class_name.string);
	*tofree = r;
    }
    অন্যথা
    {
	পূর্ণাঙ্ক old_copyID;
	যদি (restore_copyID)
	    old_copyID = obj->obj_copyID;

	obj->obj_copyID = copyID;
	*tofree = object2string(obj, numbuf, copyID, echo_style,
				restore_copyID, composite_val);
	যদি (restore_copyID)
	    obj->obj_copyID = old_copyID;
	r = *tofree;
    }

    প্রত্যাগম r;
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
    পূর্ণাঙ্ক		copyID,
    পূর্ণাঙ্ক		echo_style,
    পূর্ণাঙ্ক		restore_copyID,
    পূর্ণাঙ্ক		composite_val)
{
    স্থির পূর্ণাঙ্ক	recurse = 0;
    char_u	*r = রিক্ত;

    যদি (recurse >= DICT_MAXNEST)
    {
	যদি (!did_echo_string_emsg)
	{
	    // Only give this message once for a recursive call to avoid
	    // flooding the user with errors.  And stop iterating over lists
	    // and dicts and objects.
	    did_echo_string_emsg = TRUE;
	    emsg(_(e_variable_nested_too_deep_for_displaying));
	}
	*tofree = রিক্ত;
	প্রত্যাগম (char_u *)"{E724}";
    }
    ++recurse;

    বিকল্প (tv->v_type)
    {
	পক্ষ VAR_STRING:
	    r = string_tv2string(tv, tofree, echo_style, composite_val);
	    ভঙ্গ;

	পক্ষ VAR_FUNC:
	    r = func_tv2string(tv, tofree, echo_style);
	    ভঙ্গ;

	পক্ষ VAR_PARTIAL:
	    যদি (tv->vval.v_partial == রিক্ত
		    || tv->vval.v_partial->pt_obj == রিক্ত)
		r = partial_tv2string(tv, tofree, numbuf, copyID);
	    অন্যথা
		r = method_tv2string(tv, tofree, echo_style);
	    ভঙ্গ;

	পক্ষ VAR_BLOB:
	    r = blob2string(tv->vval.v_blob, tofree, numbuf);
	    ভঙ্গ;

	পক্ষ VAR_LIST:
	    r = list_tv2string(tv, tofree, copyID, restore_copyID);
	    ভঙ্গ;

	পক্ষ VAR_TUPLE:
	    r = tuple_tv2string(tv, tofree, copyID, restore_copyID);
	    ভঙ্গ;

	পক্ষ VAR_DICT:
	    r = dict_tv2string(tv, tofree, copyID, restore_copyID);
	    ভঙ্গ;

	পক্ষ VAR_সংখ্যা:
	পক্ষ VAR_UNKNOWN:
	পক্ষ VAR_ANY:
	পক্ষ VAR_VOID:
	    *tofree = রিক্ত;
	    r = tv_get_string_buf(tv, numbuf);
	    ভঙ্গ;

	পক্ষ VAR_JOB:
	পক্ষ VAR_CHANNEL:
	    r = jobchan_tv2string(tv, tofree, numbuf, composite_val);
	    ভঙ্গ;

	পক্ষ VAR_INSTR:
	    *tofree = রিক্ত;
	    r = (char_u *)"instructions";
	    ভঙ্গ;

	পক্ষ VAR_CLASS:
	    r = class_tv2string(tv, tofree);
	    ভঙ্গ;

	পক্ষ VAR_OBJECT:
	    r = object_tv2string(tv, tofree, copyID, restore_copyID,
					 numbuf, echo_style, composite_val);
	    ভঙ্গ;

	পক্ষ VAR_FLOAT:
	    *tofree = রিক্ত;
	    vim_snprintf((বর্ণ *)numbuf, NUMBUFLEN, "%g", tv->vval.v_float);
	    r = numbuf;
	    ভঙ্গ;

	পক্ষ VAR_BOOL:
	পক্ষ VAR_SPECIAL:
	    *tofree = রিক্ত;
	    r = (char_u *)get_var_special_name(tv->vval.v_সংখ্যা);
	    ভঙ্গ;

	পক্ষ VAR_TYPEALIAS:
	    *tofree = vim_strsave(tv->vval.v_typealias->ta_name);
	    r = *tofree;
	    যদি (r == রিক্ত)
		r = (char_u *)"";
	    ভঙ্গ;
    }

    যদি (--recurse == 0)
	did_echo_string_emsg = FALSE;
    প্রত্যাগম r;
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
    পূর্ণাঙ্ক		copyID)
{
    প্রত্যাগম echo_string_core(tv, tofree, numbuf, copyID, TRUE, FALSE, FALSE);
}

/*
 * Convert the specified byte index of line 'lnum' in buffer 'buf' to a
 * character index.  Works only for loaded buffers. Returns -1 on failure.
 * The index of the first byte and the first character is zero.
 */
    পূর্ণাঙ্ক
buf_byteidx_to_charidx(buf_T *buf, পূর্ণাঙ্ক lnum, পূর্ণাঙ্ক byteidx)
{
    char_u	*str;
    char_u	*t;
    পূর্ণাঙ্ক		গুনতি;

    যদি (buf == রিক্ত || buf->b_ml.ml_mfp == রিক্ত)
	প্রত্যাগম -1;

    যদি (lnum > buf->b_ml.ml_সারি_গুনতি)
	lnum = buf->b_ml.ml_সারি_গুনতি;

    str = ml_get_buf(buf, lnum, FALSE);
    যদি (str == রিক্ত)
	প্রত্যাগম -1;

    যদি (*str == NUL)
	প্রত্যাগম 0;

    // count the number of characters
    t = str;
    প্রতি (গুনতি = 0; *t != NUL && t <= str + byteidx; গুনতি++)
	t += mb_ptr2len(t);

    // In insert mode, when the cursor is at the end of a non-empty line,
    // byteidx points to the NUL character immediately past the end of the
    // string. In this case, add one to the character count.
    যদি (*t == NUL && byteidx != 0 && t == str + byteidx)
	গুনতি++;

    প্রত্যাগম গুনতি - 1;
}

/*
 * Convert the specified character index of line 'lnum' in buffer 'buf' to a
 * byte index.  Works only for loaded buffers. Returns -1 on failure.
 * The index of the first byte and the first character is zero.
 */
    পূর্ণাঙ্ক
buf_charidx_to_byteidx(buf_T *buf, পূর্ণাঙ্ক lnum, পূর্ণাঙ্ক charidx)
{
    char_u	*str;
    char_u	*t;

    যদি (buf == রিক্ত || buf->b_ml.ml_mfp == রিক্ত)
	প্রত্যাগম -1;

    যদি (lnum > buf->b_ml.ml_সারি_গুনতি)
	lnum = buf->b_ml.ml_সারি_গুনতি;

    str = ml_get_buf(buf, lnum, FALSE);
    যদি (str == রিক্ত)
	প্রত্যাগম -1;

    // Convert the character offset to a byte offset
    t = str;
    যাবত্ (*t != NUL && --charidx > 0)
	t += mb_ptr2len(t);

    প্রত্যাগম t - str;
}

/*
 * Translate a String variable into a position.
 * Returns NULL when there is an error.
 */
    pos_T *
var2fpos(
    typval_T	*varp,
    পূর্ণাঙ্ক		dollar_lnum,	// TRUE when $ is last line
    পূর্ণাঙ্ক		*fnum,		// set to fnum for '0, 'A, etc.
    পূর্ণাঙ্ক		charcol)	// return character column
{
    char_u		*name;
    স্থির pos_T	pos;
    pos_T		*pp;
    পূর্ণাঙ্ক			error = FALSE;

    // Argument can be [lnum, col, coladd].
    যদি (varp->v_type == VAR_LIST)
    {
	list_T		*l;
	পূর্ণাঙ্ক		len;
	listitem_T	*li;

	l = varp->vval.v_list;
	যদি (l == রিক্ত)
	    প্রত্যাগম রিক্ত;

	// Get the line number
	pos.lnum = list_find_nr(l, 0L, &error);
	যদি (error || pos.lnum <= 0 || pos.lnum > curbuf->b_ml.ml_সারি_গুনতি)
	    প্রত্যাগম রিক্ত;	// invalid line number
	যদি (charcol)
	    len = (long)mb_charlen(ml_get(pos.lnum));
	অন্যথা
	    len = (long)ml_get_len(pos.lnum);

	// Get the column number
	// We accept "$" for the column number: last column.
	li = list_find(l, 1L);
	যদি (li != রিক্ত && li->li_tv.v_type == VAR_STRING
		&& li->li_tv.vval.v_string != রিক্ত
		&& STRCMP(li->li_tv.vval.v_string, "$") == 0)
	{
	    pos.col = len + 1;
	}
	অন্যথা
	{
	    pos.col = list_find_nr(l, 1L, &error);
	    যদি (error)
		প্রত্যাগম রিক্ত;
	}

	// Accept a position up to the NUL after the line.
	যদি (pos.col == 0 || (পূর্ণাঙ্ক)pos.col > len + 1)
	    প্রত্যাগম রিক্ত;	// invalid column number
	--pos.col;

	// Get the virtual offset.  Defaults to zero.
	pos.coladd = list_find_nr(l, 2L, &error);
	যদি (error)
	    pos.coladd = 0;

	প্রত্যাগম &pos;
    }

    যদি (in_vim9script() && check_for_string_arg(varp, 0) == FAIL)
	প্রত্যাগম রিক্ত;

    name = tv_get_string_chk(varp);
    যদি (name == রিক্ত)
	প্রত্যাগম রিক্ত;

    error = TRUE;
    pos.lnum = 0;
    যদি (name[0] == '.')
    {
	যদি (!in_vim9script() || name[1] == NUL)
	{
	    error = FALSE;
	    // cursor
	    pos = curwin->w_cursor;
	}
    }
    অন্যথা যদি (name[0] == 'v' && name[1] == NUL)
    {
	// Visual start
	যদি (VIsual_active)
	    pos = VIsual;
	অন্যথা
	    pos = curwin->w_cursor;
    }
    অন্যথা যদি (name[0] == '\'')
    {
	যদি (!in_vim9script() || (name[1] != NUL && name[2] == NUL))
	{
	    error = FALSE;
	    // mark
	    pp = getmark_buf_fnum(curbuf, name[1], FALSE, fnum);
	    যদি (pp == রিক্ত || pp == (pos_T *)-1 || pp->lnum <= 0)
		প্রত্যাগম রিক্ত;
	    pos = *pp;
	}
    }
    যদি (pos.lnum != 0)
    {
	যদি (charcol)
	    pos.col = buf_byteidx_to_charidx(curbuf, pos.lnum, pos.col);
	প্রত্যাগম &pos;
    }

    pos.coladd = 0;

    যদি (name[0] == 'w' && dollar_lnum)
    {
	// the "w_valid" flags are not reset when moving the cursor, but they
	// do matter for update_topline() and validate_botline().
	check_cursor_moved(curwin);

	pos.col = 0;
	যদি (name[1] == '0')		// "w0": first visible line
	{
#ifdef FEAT_TERMINAL
	    যদি (bt_terminal(curwin->w_buffer)
				    && curwin->w_buffer->b_term != রিক্ত
				    && !term_in_normal_mode(curwin->w_buffer))
		may_move_terminal_to_buffer(curwin->w_buffer->b_term, TRUE);
#endif
	    update_topline();
	    // In silent Ex mode topline is zero, but that's not a valid line
	    // number; use one instead.
	    pos.lnum = curwin->w_topline > 0 ? curwin->w_topline : 1;
	    প্রত্যাগম &pos;
	}
	অন্যথা যদি (name[1] == '$')	// "w$": last visible line
	{
#ifdef FEAT_TERMINAL
	    যদি (bt_terminal(curwin->w_buffer)
				    && curwin->w_buffer->b_term != রিক্ত
				    && !term_in_normal_mode(curwin->w_buffer))
		may_move_terminal_to_buffer(curwin->w_buffer->b_term, TRUE);
#endif
	    validate_botline();
	    // In silent Ex mode botline is zero, return zero then.
	    pos.lnum = curwin->w_botline > 0 ? curwin->w_botline - 1 : 0;
	    প্রত্যাগম &pos;
	}
    }
    অন্যথা যদি (name[0] == '$')		// last column or line
    {
	যদি (dollar_lnum)
	{
	    pos.lnum = curbuf->b_ml.ml_সারি_গুনতি;
	    pos.col = 0;
	}
	অন্যথা
	{
	    pos.lnum = curwin->w_cursor.lnum;
	    যদি (charcol)
		pos.col = (colnr_T)mb_charlen(ml_get_curline());
	    অন্যথা
		pos.col = ml_get_curline_len();
	}
	প্রত্যাগম &pos;
    }
    যদি (in_vim9script() && error)
	semsg(_(e_invalid_মান_for_সারি_সংখ্যা_str), name);
    প্রত্যাগম রিক্ত;
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
    পূর্ণাঙ্ক
list2fpos(
    typval_T	*arg,
    pos_T	*posp,
    পূর্ণাঙ্ক		*fnump,
    colnr_T	*curswantp,
    পূর্ণাঙ্ক		charcol)
{
    list_T	*l = arg->vval.v_list;
    long	i = 0;
    long	n;

    // List must be: [fnum, lnum, col, coladd, curswant], where "fnum" is only
    // there when "fnump" isn't NULL; "coladd" and "curswant" are optional.
    যদি (arg->v_type != VAR_LIST
	    || l == রিক্ত
	    || l->lv_len < (fnump == রিক্ত ? 2 : 3)
	    || l->lv_len > (fnump == রিক্ত ? 4 : 5))
	প্রত্যাগম FAIL;

    যদি (fnump != রিক্ত)
    {
	n = list_find_nr(l, i++, রিক্ত);	// fnum
	যদি (n < 0)
	    প্রত্যাগম FAIL;
	যদি (n == 0)
	    n = curbuf->b_fnum;		// current buffer
	*fnump = n;
    }

    n = list_find_nr(l, i++, রিক্ত);	// lnum
    যদি (n < 0)
	প্রত্যাগম FAIL;
    posp->lnum = n;

    n = list_find_nr(l, i++, রিক্ত);	// col
    যদি (n < 0)
	প্রত্যাগম FAIL;
    // If character position is specified, then convert to byte position
    // If the line number is zero use the cursor line.
    যদি (charcol)
    {
	buf_T	*buf;

	// Get the text for the specified line in a loaded buffer
	buf = buflist_findnr(fnump == রিক্ত ? curbuf->b_fnum : *fnump);
	যদি (buf == রিক্ত || buf->b_ml.ml_mfp == রিক্ত)
	    প্রত্যাগম FAIL;

	n = buf_charidx_to_byteidx(buf,
		  posp->lnum == 0 ? curwin->w_cursor.lnum : posp->lnum, n) + 1;
    }
    posp->col = n;

    n = list_find_nr(l, i, রিক্ত);	// off
    যদি (n < 0)
	posp->coladd = 0;
    অন্যথা
	posp->coladd = n;

    যদি (curswantp != রিক্ত)
	*curswantp = list_find_nr(l, i + 1, রিক্ত);  // curswant

    প্রত্যাগম OK;
}

/*
 * Get the length of an environment variable name.
 * Advance "arg" to the first character after the name.
 * Return 0 for error.
 */
    পূর্ণাঙ্ক
get_env_len(char_u **arg)
{
    char_u	*p;
    পূর্ণাঙ্ক		len;

    প্রতি (p = *arg; vim_isIDc(*p); ++p)
	;
    যদি (p == *arg)	    // no name found
	প্রত্যাগম 0;

    len = (পূর্ণাঙ্ক)(p - *arg);
    *arg = p;
    প্রত্যাগম len;
}

/*
 * Get the length of the name of a function or internal variable.
 * "arg" is advanced to after the name.
 * Return 0 if something is wrong.
 */
    পূর্ণাঙ্ক
get_id_len(char_u **arg)
{
    char_u	*p;
    পূর্ণাঙ্ক		len;

    // Find the end of the name.
    প্রতি (p = *arg; eval_isnamec(*p); ++p)
    {
	যদি (*p == ':')
	{
	    // "s:" is start of "s:var", but "n:" is not and can be used in
	    // slice "[n:]".  Also "xx:" is not a namespace.
	    len = (পূর্ণাঙ্ক)(p - *arg);
	    যদি ((len == 1 && vim_strchr(NAMESPACE_CHAR, **arg) == রিক্ত)
		    || len > 1)
		ভঙ্গ;
	}
    }
    যদি (p == *arg)	    // no name found
	প্রত্যাগম 0;

    len = (পূর্ণাঙ্ক)(p - *arg);
    *arg = p;

    প্রত্যাগম len;
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
    পূর্ণাঙ্ক
get_name_len(
    char_u	**arg,
    char_u	**alias,
    পূর্ণাঙ্ক		evaluate,
    পূর্ণাঙ্ক		verbose)
{
    পূর্ণাঙ্ক		len;
    char_u	*p;
    char_u	*রাশিমালা_start;
    char_u	*রাশিমালা_end;

    *alias = রিক্ত;  // default to no alias

    যদি ((*arg)[0] == K_SPECIAL && (*arg)[1] == KS_EXTRA
						  && (*arg)[2] == (পূর্ণাঙ্ক)KE_SNR)
    {
	// hard coded <SNR>, already translated
	*arg += 3;
	প্রত্যাগম get_id_len(arg) + 3;
    }
    len = eval_fname_script(*arg);
    যদি (len > 0)
    {
	// literal "<SID>", "s:" or "<SNR>"
	*arg += len;
    }

    /*
     * Find the end of the name; check for {} construction.
     */
    p = find_name_end(*arg, &রাশিমালা_start, &রাশিমালা_end,
					       len > 0 ? 0 : FNE_CHECK_START);
    যদি (রাশিমালা_start != রিক্ত)
    {
	char_u	*temp_string;

	যদি (!evaluate)
	{
	    len += (পূর্ণাঙ্ক)(p - *arg);
	    *arg = skipwhite(p);
	    প্রত্যাগম len;
	}

	/*
	 * Include any <SID> etc in the expanded string:
	 * Thus the -len here.
	 */
	temp_string = make_expanded_name(*arg - len, রাশিমালা_start, রাশিমালা_end, p);
	যদি (temp_string == রিক্ত)
	    প্রত্যাগম -1;
	*alias = temp_string;
	*arg = skipwhite(p);
	প্রত্যাগম (পূর্ণাঙ্ক)STRLEN(temp_string);
    }

    len += get_id_len(arg);
    // Only give an error when there is something, otherwise it will be
    // reported at a higher level.
    যদি (len == 0 && verbose && **arg != NUL)
	semsg(_(e_invalid_expression_str), *arg);

    প্রত্যাগম len;
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
    char_u	**রাশিমালা_start,
    char_u	**রাশিমালা_end,
    পূর্ণাঙ্ক		flags)
{
    পূর্ণাঙ্ক		mb_nest = 0;
    পূর্ণাঙ্ক		br_nest = 0;
    char_u	*p;
    পূর্ণাঙ্ক		len;
    পূর্ণাঙ্ক		allow_curly = !in_vim9script();

    যদি (রাশিমালা_start != রিক্ত)
    {
	*রাশিমালা_start = রিক্ত;
	*রাশিমালা_end = রিক্ত;
    }

    // Quick check for valid starting character.
    যদি ((flags & FNE_CHECK_START) && !eval_isnamec1(*arg)
					      && (*arg != '{' || !allow_curly))
	প্রত্যাগম arg;

    প্রতি (p = arg; *p != NUL
		    && (eval_isnamec(*p)
			|| (*p == '{' && allow_curly)
			|| ((flags & FNE_INCL_BR) && (*p == '['
					 || (*p == '.' && eval_isdictc(p[1]))))
			|| mb_nest != 0
			|| br_nest != 0); MB_PTR_ADV(p))
    {
	যদি (*p == '\'')
	{
	    // skip over 'string' to avoid counting [ and ] inside it.
	    প্রতি (p = p + 1; *p != NUL && *p != '\''; MB_PTR_ADV(p))
		;
	    যদি (*p == NUL)
		ভঙ্গ;
	}
	অন্যথা যদি (*p == '"')
	{
	    // skip over "str\"ing" to avoid counting [ and ] inside it.
	    প্রতি (p = p + 1; *p != NUL && *p != '"'; MB_PTR_ADV(p))
		যদি (*p == '\\' && p[1] != NUL)
		    ++p;
	    যদি (*p == NUL)
		ভঙ্গ;
	}
	অন্যথা যদি (br_nest == 0 && mb_nest == 0 && *p == ':')
	{
	    // "s:" is start of "s:var", but "n:" is not and can be used in
	    // slice "[n:]".  Also "xx:" is not a namespace. But {ns}: is.
	    len = (পূর্ণাঙ্ক)(p - arg);
	    যদি ((len == 1 && vim_strchr(NAMESPACE_CHAR, *arg) == রিক্ত)
		    || (len > 1 && p[-1] != '}'))
		ভঙ্গ;
	}

	যদি (mb_nest == 0)
	{
	    যদি (*p == '[')
		++br_nest;
	    অন্যথা যদি (*p == ']')
		--br_nest;
	}

	যদি (br_nest == 0 && allow_curly)
	{
	    যদি (*p == '{')
	    {
		mb_nest++;
		যদি (রাশিমালা_start != রিক্ত && *রাশিমালা_start == রিক্ত)
		    *রাশিমালা_start = p;
	    }
	    অন্যথা যদি (*p == '}')
	    {
		mb_nest--;
		যদি (রাশিমালা_start != রিক্ত && mb_nest == 0 && *রাশিমালা_end == রিক্ত)
		    *রাশিমালা_end = p;
	    }
	}
    }

    প্রত্যাগম p;
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
    স্থির char_u *
make_expanded_name(
    char_u	*in_start,
    char_u	*রাশিমালা_start,
    char_u	*রাশিমালা_end,
    char_u	*in_end)
{
    char_u	c1;
    char_u	*retval = রিক্ত;
    char_u	*temp_result;

    যদি (রাশিমালা_end == রিক্ত || in_end == রিক্ত)
	প্রত্যাগম রিক্ত;
    *রাশিমালা_start	= NUL;
    *রাশিমালা_end = NUL;
    c1 = *in_end;
    *in_end = NUL;

    temp_result = eval_to_string(রাশিমালা_start + 1, FALSE, FALSE);
    যদি (temp_result != রিক্ত)
    {
	size_t	retvalsize = (size_t)(রাশিমালা_start - in_start)
				+ STRLEN(temp_result)
				+ (size_t)(in_end - রাশিমালা_end) + 1;

	retval = alloc(retvalsize);
	যদি (retval != রিক্ত)
	    vim_snprintf((বর্ণ *)retval, retvalsize, "%s%s%s",
				in_start, temp_result, রাশিমালা_end + 1);
    }
    vim_free(temp_result);

    *in_end = c1;		// put char back for error messages
    *রাশিমালা_start = '{';
    *রাশিমালা_end = '}';

    যদি (retval != রিক্ত)
    {
	temp_result = find_name_end(retval, &রাশিমালা_start, &রাশিমালা_end, 0);
	যদি (রাশিমালা_start != রিক্ত)
	{
	    // Further expansion!
	    temp_result = make_expanded_name(retval, রাশিমালা_start,
						       রাশিমালা_end, temp_result);
	    vim_free(retval);
	    retval = temp_result;
	}
    }

    প্রত্যাগম retval;
}

/*
 * Return TRUE if character "c" can be used in a variable or function name.
 * Does not include '{' or '}' for magic braces.
 */
    পূর্ণাঙ্ক
eval_isnamec(পূর্ণাঙ্ক c)
{
    প্রত্যাগম ASCII_ISALNUM(c) || c == '_' || c == ':' || c == AUTOLOAD_CHAR;
}

/*
 * Return TRUE if character "c" can be used as the first character in a
 * variable or function name (excluding '{' and '}').
 */
    পূর্ণাঙ্ক
eval_isnamec1(পূর্ণাঙ্ক c)
{
    প্রত্যাগম ASCII_ISALPHA(c) || c == '_';
}

/*
 * Return TRUE if character "c" can be used as the first character of a
 * dictionary key.
 */
    পূর্ণাঙ্ক
eval_isdictc(পূর্ণাঙ্ক c)
{
    প্রত্যাগম ASCII_ISALNUM(c) || c == '_';
}

/*
 * Return true when "p" points to "->" followed by a method name or a lambda.
 * Only called for Vim9 script.
 */
    স্থির bool
method_call_follows(char_u *p)
{
    যদি (p[0] != '-' || p[1] != '>')
	প্রত্যাগম false;

    char_u	*after = skipwhite(p + 2);

    প্রত্যাগম ASCII_ISALPHA(*after) || *after == '(';
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
    পূর্ণাঙ্ক
handle_subscript(
    char_u	**arg,
    char_u	*name_start,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    পূর্ণাঙ্ক		verbose)	// give error messages
{
    পূর্ণাঙ্ক		evaluate = evalarg != রিক্ত
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    পূর্ণাঙ্ক		ret = OK;
    dict_T	*selfdict = রিক্ত;
    পূর্ণাঙ্ক		check_white = TRUE;
    পূর্ণাঙ্ক		getnext;
    char_u	*p;

    যাবত্ (ret == OK)
    {
	// When at the end of the line and ".name" or a method call follows in
	// the next line then consume the line break.
	p = eval_next_non_blank(*arg, evalarg, &getnext);
	যদি (getnext
	    && ((*p == '.'
		    && ((rettv->v_type == VAR_DICT && eval_isdictc(p[1]))
			|| rettv->v_type == VAR_CLASS
			|| rettv->v_type == VAR_OBJECT))
		|| method_call_follows(p)))
	{
	    *arg = eval_next_সারি(*arg, evalarg);
	    p = *arg;
	    check_white = FALSE;
	}

	যদি (rettv->v_type == VAR_ANY)
	{
	    char_u	*exp_name;
	    পূর্ণাঙ্ক		cc;
	    পূর্ণাঙ্ক		idx;
	    ufunc_T	*ufunc;
	    type_T	*type;

	    // Found script from "import {name} as name", script item name must
	    // follow.  "rettv->vval.v_number" has the script ID.
	    যদি (**arg != '.')
	    {
		যদি (verbose)
		    semsg(_(e_expected_dot_after_name_str),
					name_start != রিক্ত ? name_start: *arg);
		ret = FAIL;
		ভঙ্গ;
	    }
	    ++*arg;
	    যদি (IS_WHITE_OR_NUL(**arg))
	    {
		যদি (verbose)
		    emsg(_(e_no_white_space_allowed_after_dot));
		ret = FAIL;
		ভঙ্গ;
	    }

	    // isolate the name
	    exp_name = *arg;
	    যাবত্ (eval_isnamec(**arg))
		++*arg;
	    cc = **arg;
	    **arg = NUL;

	    idx = find_exported(rettv->vval.v_সংখ্যা, exp_name, &ufunc, &type,
		       evalarg == রিক্ত ? রিক্ত : evalarg->eval_cctx,
		       evalarg == রিক্ত ? রিক্ত : evalarg->eval_cstack, verbose);
	    **arg = cc;

	    যদি (idx < 0 && ufunc == রিক্ত)
	    {
		ret = FAIL;
		ভঙ্গ;
	    }
	    যদি (idx >= 0)
	    {
		scriptitem_T    *si = SCRIPT_ITEM(rettv->vval.v_সংখ্যা);
		svar_T		*sv = ((svar_T *)si->sn_var_vals.ga_data) + idx;

		copy_tv(sv->sv_tv, rettv);
	    }
	    অন্যথা
	    {
		rettv->v_type = VAR_FUNC;
		যদি (**arg == '<')
		{
		    char_u *s = get_generic_func_name(ufunc, arg);
		    যদি (s != রিক্ত)
			rettv->vval.v_string = s;
		    অন্যথা
			ret = FAIL;
		}
		অন্যথা
		{
		    rettv->vval.v_string =
			vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
		}
	    }
	    অনুবর্ত;
	}

	যদি ((**arg == '(' && (!evaluate || rettv->v_type == VAR_FUNC
			    || rettv->v_type == VAR_PARTIAL))
		    && (!check_white || !VIM_ISWHITE(*(*arg - 1))))
	{
	    ret = call_func_rettv(arg, evalarg, rettv, evaluate,
							       selfdict, রিক্ত);

	    // Stop the expression evaluation when immediately aborting on
	    // error, or when an interrupt occurred or an exception was thrown
	    // but not caught.
	    যদি (aborting())
	    {
		যদি (ret == OK)
		    clear_tv(rettv);
		ret = FAIL;
	    }
	    dict_unref(selfdict);
	    selfdict = রিক্ত;
	}
	অন্যথা যদি (p[0] == '-' && p[1] == '>')
	{
	    যদি (in_vim9script())
		*arg = skipwhite(p + 2);
	    অন্যথা
		*arg = p + 2;
	    যদি (ret == OK && evaluate && rettv->v_type == VAR_VOID)
	    {
		যদি (verbose)
		    emsg(_(e_cannot_use_void_মান));
		ret = FAIL;
	    }
	    অন্যথা যদি (VIM_ISWHITE(**arg))
	    {
		emsg(_(e_no_white_space_allowed_before_parenthesis));
		ret = FAIL;
	    }
	    অন্যথা যদি ((**arg == '{' && !in_vim9script()) || **arg == '(')
		// expr->{lambda}() or expr->(lambda)()
		ret = eval_lambda(arg, rettv, evalarg, verbose);
	    অন্যথা
		// expr->name()
		ret = eval_method(arg, rettv, evalarg, verbose);
	}
	// "." is ".name" lookup when we found a dict or when evaluating and
	// scriptversion is at least 2, where string concatenation is "..".
	অন্যথা যদি (**arg == '['
		|| (**arg == '.' && (rettv->v_type == VAR_DICT
			|| (!evaluate
			    && (*arg)[1] != '.'
			    && !in_old_script(2)))))
	{
	    dict_unref(selfdict);
	    যদি (rettv->v_type == VAR_DICT)
	    {
		selfdict = rettv->vval.v_dict;
		যদি (selfdict != রিক্ত)
		    ++selfdict->dv_refcount;
	    }
	    অন্যথা
		selfdict = রিক্ত;
	    যদি (eval_index(arg, rettv, evalarg, verbose) == FAIL)
	    {
		clear_tv(rettv);
		ret = FAIL;
	    }
	}
	অন্যথা যদি (**arg == '.' && (rettv->v_type == VAR_CLASS
					       || rettv->v_type == VAR_OBJECT))
	{
	    // class member: SomeClass.varname
	    // class method: SomeClass.SomeMethod()
	    // class constructor: SomeClass.new()
	    // object member: someObject.varname
	    // object method: someObject.SomeMethod()
	    যদি (class_object_index(arg, rettv, evalarg, verbose) == FAIL)
	    {
		clear_tv(rettv);
		ret = FAIL;
	    }
	}
	অন্যথা
	    ভঙ্গ;
    }

    // Turn "dict.Func" into a partial for "Func" bound to "dict".
    // Don't do this when "Func" is already a partial that was bound
    // explicitly (pt_auto is FALSE).
    যদি (selfdict != রিক্ত
	    && (rettv->v_type == VAR_FUNC
		|| (rettv->v_type == VAR_PARTIAL
		    && (rettv->vval.v_partial->pt_auto
			|| rettv->vval.v_partial->pt_dict == রিক্ত))))
	selfdict = make_partial(selfdict, rettv);

    dict_unref(selfdict);
    প্রত্যাগম ret;
}

/*
 * Make a copy of an item.
 * Lists and Dictionaries are also copied.  A deep copy if "deep" is set.
 * "top" is TRUE for the toplevel of copy().
 * For deepcopy() "copyID" is zero for a full copy or the ID for when a
 * reference to an already copied list/dict can be used.
 * Returns FAIL or OK.
 */
    পূর্ণাঙ্ক
item_copy(
    typval_T	*from,
    typval_T	*to,
    পূর্ণাঙ্ক		deep,
    পূর্ণাঙ্ক		শীর্ষ,
    পূর্ণাঙ্ক		copyID)
{
    স্থির পূর্ণাঙ্ক	recurse = 0;
    পূর্ণাঙ্ক		ret = OK;

    যদি (recurse >= DICT_MAXNEST)
    {
	emsg(_(e_variable_nested_too_deep_for_making_copy));
	প্রত্যাগম FAIL;
    }
    ++recurse;

    বিকল্প (from->v_type)
    {
	পক্ষ VAR_সংখ্যা:
	পক্ষ VAR_FLOAT:
	পক্ষ VAR_STRING:
	পক্ষ VAR_FUNC:
	পক্ষ VAR_PARTIAL:
	পক্ষ VAR_BOOL:
	পক্ষ VAR_SPECIAL:
	পক্ষ VAR_JOB:
	পক্ষ VAR_CHANNEL:
	পক্ষ VAR_INSTR:
	পক্ষ VAR_CLASS:
	পক্ষ VAR_OBJECT:
	পক্ষ VAR_TYPEALIAS:
	    copy_tv(from, to);
	    ভঙ্গ;
	পক্ষ VAR_LIST:
	    to->v_type = VAR_LIST;
	    to->v_lock = 0;
	    যদি (from->vval.v_list == রিক্ত)
		to->vval.v_list = রিক্ত;
	    অন্যথা যদি (copyID != 0 && from->vval.v_list->lv_copyID == copyID)
	    {
		// use the copy made earlier
		to->vval.v_list = from->vval.v_list->lv_copylist;
		++to->vval.v_list->lv_refcount;
	    }
	    অন্যথা
	    {
		to->vval.v_list = list_copy(from->vval.v_list,
							    deep, শীর্ষ, copyID);
		যদি (to->vval.v_list == রিক্ত)
		    ret = FAIL;
	    }
	    ভঙ্গ;
	পক্ষ VAR_TUPLE:
	    to->v_type = VAR_TUPLE;
	    to->v_lock = 0;
	    যদি (from->vval.v_tuple == রিক্ত)
		to->vval.v_tuple = রিক্ত;
	    অন্যথা যদি (copyID != 0 && from->vval.v_tuple->tv_copyID == copyID)
	    {
		// use the copy made earlier
		to->vval.v_tuple = from->vval.v_tuple->tv_copytuple;
		++to->vval.v_tuple->tv_refcount;
	    }
	    অন্যথা
	    {
		to->vval.v_tuple = tuple_copy(from->vval.v_tuple,
							    deep, শীর্ষ, copyID);
		যদি (to->vval.v_tuple == রিক্ত)
		    ret = FAIL;
	    }
	    ভঙ্গ;
	পক্ষ VAR_BLOB:
	    to->v_type = VAR_BLOB;
	    to->v_lock = 0;
	    যদি (from->vval.v_blob == রিক্ত)
		to->vval.v_blob = রিক্ত;
	    অন্যথা
	    {
		to->vval.v_blob = blob_copy(from->vval.v_blob);
		যদি (to->vval.v_blob == রিক্ত)
		    ret = FAIL;
	    }
	    ভঙ্গ;
	পক্ষ VAR_DICT:
	    to->v_type = VAR_DICT;
	    to->v_lock = 0;
	    যদি (from->vval.v_dict == রিক্ত)
		to->vval.v_dict = রিক্ত;
	    অন্যথা যদি (copyID != 0 && from->vval.v_dict->dv_copyID == copyID)
	    {
		// use the copy made earlier
		to->vval.v_dict = from->vval.v_dict->dv_copydict;
		++to->vval.v_dict->dv_refcount;
	    }
	    অন্যথা
	    {
		to->vval.v_dict = dict_copy(from->vval.v_dict,
							    deep, শীর্ষ, copyID);
		যদি (to->vval.v_dict == রিক্ত)
		    ret = FAIL;
	    }
	    ভঙ্গ;
	পক্ষ VAR_UNKNOWN:
	পক্ষ VAR_ANY:
	পক্ষ VAR_VOID:
	    internal_error_no_abort("item_copy(UNKNOWN)");
	    ret = FAIL;
    }
    --recurse;
    প্রত্যাগম ret;
}

    শূন্য
echo_one(typval_T *rettv, পূর্ণাঙ্ক with_space, পূর্ণাঙ্ক *atstart, পূর্ণাঙ্ক *needclr)
{
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    char_u	*p = echo_string(rettv, &tofree, numbuf, get_copyID());

    যদি (*atstart)
    {
	*atstart = FALSE;
	// Call msg_start() after eval1(), evaluating the expression
	// may cause a message to appear.
	যদি (with_space)
	{
	    // Mark the saved text as finishing the line, so that what
	    // follows is displayed on a new line when scrolling back
	    // at the more prompt.
	    বার্তা_sb_eol();
	    বার্তা_start();
	}
    }
    অন্যথা যদি (with_space)
	বার্তা_puts_attr(" ", echo_attr);

    যদি (p != রিক্ত)
	প্রতি ( ; *p != NUL && !got_int; ++p)
	{
	    যদি (*p == '\n' || *p == '\r' || *p == TAB)
	    {
		যদি (*p != TAB && *needclr)
		{
		    // remove any text still there from the command
		    বার্তা_clr_eos();
		    *needclr = FALSE;
		}
		বার্তা_putchar_attr(*p, echo_attr);
	    }
	    অন্যথা
	    {
		যদি (has_mbyte)
		{
		    পূর্ণাঙ্ক i = (*mb_ptr2len)(p);

		    (শূন্য)বার্তা_outtrans_len_attr(p, i, echo_attr);
		    p += i - 1;
		}
		অন্যথা
		    (শূন্য)বার্তা_outtrans_len_attr(p, 1, echo_attr);
	    }
	}
    vim_free(tofree);
}

/*
 * ":echo expr1 ..."	print each argument separated with a space, add a
 *			newline at the end.
 * ":echon expr1 ..."	print each argument plain.
 */
    শূন্য
ex_echo(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    typval_T	rettv;
    char_u	*arg_start;
    পূর্ণাঙ্ক		needclr = TRUE;
    পূর্ণাঙ্ক		atstart = TRUE;
    পূর্ণাঙ্ক		did_emsg_before = did_emsg;
    পূর্ণাঙ্ক		called_emsg_before = called_emsg;
    evalarg_T	evalarg;

    fill_evalarg_from_eap(&evalarg, eap, eap->skip);

    যদি (eap->skip)
	++emsg_skip;
    যাবত্ ((!ends_excmd2(eap->cmd, arg) || *arg == '"') && !got_int)
    {
	// If eval1() causes an error message the text from the command may
	// still need to be cleared. E.g., "echo 22,44".
	need_clr_eos = needclr;

	arg_start = arg;
	যদি (eval1(&arg, &rettv, &evalarg) == FAIL)
	{
	    /*
	     * Report the invalid expression unless the expression evaluation
	     * has been cancelled due to an aborting error, an interrupt, or an
	     * exception.
	     */
	    যদি (!aborting() && did_emsg == did_emsg_before
					  && called_emsg == called_emsg_before)
		semsg(_(e_invalid_expression_str), arg_start);
	    need_clr_eos = FALSE;
	    ভঙ্গ;
	}
	need_clr_eos = FALSE;

	যদি (!eap->skip)
	{
	    যদি (rettv.v_type == VAR_VOID)
	    {
		semsg(_(e_expression_does_not_result_in_মান_str), arg_start);
		ভঙ্গ;
	    }
	    echo_one(&rettv, eap->cmdidx == CMD_echo, &atstart, &needclr);
	}

	clear_tv(&rettv);
	arg = skipwhite(arg);
    }
    set_nextcmd(eap, arg);
    clear_evalarg(&evalarg, eap);

    যদি (eap->skip)
	--emsg_skip;
    অন্যথা
    {
	// remove text that may still be there from the command
	যদি (needclr)
	    বার্তা_clr_eos();
	যদি (eap->cmdidx == CMD_echo)
	    বার্তা_end();
    }
}

/*
 * ":echohl {name}".
 */
    শূন্য
ex_echohl(exarg_T *eap)
{
    echo_attr = syn_name2attr(eap->arg);
}

/*
 * Returns the :echo attribute
 */
    পূর্ণাঙ্ক
get_echo_attr(শূন্য)
{
    প্রত্যাগম echo_attr;
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
    শূন্য
ex_execute(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    typval_T	rettv;
    পূর্ণাঙ্ক		ret = OK;
    char_u	*p;
    garray_T	ga;
    পূর্ণাঙ্ক		len;
    long	start_lnum = SOURCING_LNUM;

    ga_init2(&ga, 1, 80);

    যদি (eap->skip)
	++emsg_skip;
    যাবত্ (!ends_excmd2(eap->cmd, arg) || *arg == '"')
    {
	ret = eval1_emsg(&arg, &rettv, eap);
	যদি (ret == FAIL)
	    ভঙ্গ;

	যদি (!eap->skip)
	{
	    char_u   buf[NUMBUFLEN];

	    যদি (eap->cmdidx == CMD_execute)
	    {
		যদি (rettv.v_type == VAR_CHANNEL || rettv.v_type == VAR_JOB)
		{
		    semsg(_(e_using_invalid_মান_as_string_str),
						  vartype_name(rettv.v_type));
		    p = রিক্ত;
		}
		অন্যথা
		    p = tv_get_string_buf(&rettv, buf);
	    }
	    অন্যথা
		p = tv_stringify(&rettv, buf);
	    যদি (p == রিক্ত)
	    {
		clear_tv(&rettv);
		ret = FAIL;
		ভঙ্গ;
	    }
	    len = (পূর্ণাঙ্ক)STRLEN(p);
	    যদি (ga_grow(&ga, len + 2) == FAIL)
	    {
		clear_tv(&rettv);
		ret = FAIL;
		ভঙ্গ;
	    }
	    যদি (ga.ga_len)
		((char_u *)(ga.ga_data))[ga.ga_len++] = ' ';
	    STRCPY((char_u *)(ga.ga_data) + ga.ga_len, p);
	    ga.ga_len += len;
	}

	clear_tv(&rettv);
	arg = skipwhite(arg);
    }

    যদি (ret != FAIL && ga.ga_data != রিক্ত)
    {
	// use the first line of continuation lines for messages
	SOURCING_LNUM = start_lnum;

	যদি (eap->cmdidx == CMD_echomsg
		|| eap->cmdidx == CMD_echowindow
		|| eap->cmdidx == CMD_echoerr)
	{
	    // Mark the already saved text as finishing the line, so that what
	    // follows is displayed on a new line when scrolling back at the
	    // more prompt.
	    বার্তা_sb_eol();
	}

	যদি (eap->cmdidx == CMD_echomsg)
	{
	    বার্তা_attr(ga.ga_data, echo_attr);
	    out_flush();
	}
	অন্যথা যদি (eap->cmdidx == CMD_echowindow)
	{
#ifdef HAS_MESSAGE_WINDOW
	    start_echowindow(eap->addr_গুনতি > 0 ? eap->line2 : 0);
#endif
	    বার্তা_attr(ga.ga_data, echo_attr);
#ifdef HAS_MESSAGE_WINDOW
	    end_echowindow();
#endif
	}
	অন্যথা যদি (eap->cmdidx == CMD_echoconsole)
	{
	    ui_write(ga.ga_data, (পূর্ণাঙ্ক)STRLEN(ga.ga_data), TRUE);
	    ui_write((char_u *)"\r\n", 2, TRUE);
	}
	অন্যথা যদি (eap->cmdidx == CMD_echoerr)
	{
	    পূর্ণাঙ্ক		save_did_emsg = did_emsg;

	    // We don't want to abort following commands, restore did_emsg.
	    emsg(ga.ga_data);
	    যদি (!force_abort)
		did_emsg = save_did_emsg;
	}
	অন্যথা যদি (eap->cmdidx == CMD_execute)
	{
	    পূর্ণাঙ্ক save_sticky_cmdmod_flags = sticky_cmdmod_flags;

	    // "legacy exe cmd" and "vim9cmd exe cmd" applies to "cmd".
	    sticky_cmdmod_flags = cmdmod.cmod_flags
						& (CMOD_LEGACY | CMOD_VIM9CMD);
	    do_cmdline((char_u *)ga.ga_data,
		       eap->ea_getline, eap->cookie, DOCMD_NOWAIT|DOCMD_VERBOSE);
	    sticky_cmdmod_flags = save_sticky_cmdmod_flags;
	}
    }

    ga_clear(&ga);

    যদি (eap->skip)
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
find_option_end(char_u **arg, পূর্ণাঙ্ক *scope)
{
    char_u	*p = *arg;

    ++p;
    যদি (*p == 'g' && p[1] == ':')
    {
	*scope = OPT_GLOBAL;
	p += 2;
    }
    অন্যথা যদি (*p == 'l' && p[1] == ':')
    {
	*scope = OPT_LOCAL;
	p += 2;
    }
    অন্যথা
	*scope = 0;

    যদি (!ASCII_ISALPHA(*p))
	প্রত্যাগম রিক্ত;
    *arg = p;

    যদি (p[0] == 't' && p[1] == '_' && p[2] != NUL && p[3] != NUL)
	p += 4;	    // termcap option
    অন্যথা
	যাবত্ (ASCII_ISALPHA(*p))
	    ++p;
    প্রত্যাগম p;
}

/*
 * Display script name where an item was last set.
 * Should only be invoked when 'verbose' is non-zero.
 */
    শূন্য
last_set_বার্তা(sctx_T script_ctx)
{
    char_u *p;

    যদি (script_ctx.sc_sid == 0)
	প্রত্যাগম;

    p = home_replace_save(রিক্ত, get_scriptname(script_ctx.sc_sid));
    যদি (p == রিক্ত)
	প্রত্যাগম;

    verbose_enter();
    বার্তা_puts(_("\n\tLast set from "));
    বার্তা_puts((বর্ণ *)p);
    যদি (script_ctx.sc_lnum > 0)
    {
	বার্তা_puts(_(সারি_বার্তা));
	বার্তা_outnum((long)script_ctx.sc_lnum);
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
    typval_T	*রাশিমালা,
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
    যদি (regmatch.regprog != রিক্ত)
    {
	char_u	*tail = str;
	char_u	*end = str + len;
	পূর্ণাঙ্ক	do_all = (flags[0] == 'g');
	পূর্ণাঙ্ক	sublen;
	পূর্ণাঙ্ক	i;
	char_u	*zero_width = রিক্ত;

	যাবত্ (vim_regexec_nl(&regmatch, str, (colnr_T)(tail - str)))
	{
	    // Skip empty match except for first match.
	    যদি (regmatch.startp[0] == regmatch.endp[0])
	    {
		যদি (zero_width == regmatch.startp[0])
		{
		    // avoid getting stuck on a match with an empty string
		    i = mb_ptr2len(tail);
		    mch_memmove((char_u *)ga.ga_data + ga.ga_len, tail,
								   (size_t)i);
		    ga.ga_len += i;
		    tail += i;
		    অনুবর্ত;
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
	    sublen = vim_regsub(&regmatch, sub, রাশিমালা, tail, 0, REGSUB_MAGIC);
	    যদি (sublen <= 0)
	    {
		ga_clear(&ga);
		ভঙ্গ;
	    }
	    যদি (ga_grow(&ga, (পূর্ণাঙ্ক)((end - tail) + sublen -
			    (regmatch.endp[0] - regmatch.startp[0]))) == FAIL)
	    {
		ga_clear(&ga);
		ভঙ্গ;
	    }

	    // copy the text up to where the match is
	    i = (পূর্ণাঙ্ক)(regmatch.startp[0] - tail);
	    mch_memmove((char_u *)ga.ga_data + ga.ga_len, tail, (size_t)i);
	    // add the substituted text
	    (শূন্য)vim_regsub(&regmatch, sub, রাশিমালা,
				  (char_u *)ga.ga_data + ga.ga_len + i, sublen,
				  REGSUB_COPY | REGSUB_MAGIC);
	    ga.ga_len += i + sublen - 1;
	    tail = regmatch.endp[0];
	    যদি (*tail == NUL)
		ভঙ্গ;
	    যদি (!do_all)
		ভঙ্গ;
	}

	যদি (ga.ga_data != রিক্ত)
	{
	    STRCPY((বর্ণ *)ga.ga_data + ga.ga_len, tail);
	    ga.ga_len += (পূর্ণাঙ্ক)(end - tail);
	}

	vim_regfree(regmatch.regprog);
    }

    যদি (ga.ga_data != রিক্ত)
    {
	str = (char_u *)ga.ga_data;
	len = (size_t)ga.ga_len;
    }
    ret = vim_strnsave(str, len);
    ga_clear(&ga);
    যদি (p_cpo == empty_option)
	p_cpo = save_cpo;
    অন্যথা
    {
	// Darn, evaluating {sub} expression or {expr} changed the value.
	// If it's still empty it was changed and restored, need to restore in
	// the complicated way.
	যদি (*p_cpo == NUL)
	    set_option_মান_give_err((char_u *)"cpo", 0L, save_cpo, 0);
	free_string_option(save_cpo);
    }

    যদি (ret_len != রিক্ত)
	*ret_len = len;

    প্রত্যাগম ret;
}
