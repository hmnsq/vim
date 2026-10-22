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

#अन्तर्भाव "vim.h"

#जइ defined(FEAT_EVAL)

#ifdef VMS
# include <float.h>
#endif

#define NAMESPACE_CHAR	(char_u *)"abglstvw"

स्थिर पूर्णाङ्क eval0_simple_funccal(char_u *arg, typval_T *rettv, exarg_T *eap, evalarg_T *evalarg);
स्थिर पूर्णाङ्क eval2(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
स्थिर पूर्णाङ्क eval3(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
स्थिर पूर्णाङ्क eval4(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
स्थिर पूर्णाङ्क eval5(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
स्थिर पूर्णाङ्क eval6(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
स्थिर पूर्णाङ्क eval7(char_u **arg, typval_T *rettv, evalarg_T *evalarg, पूर्णाङ्क want_string);
स्थिर पूर्णाङ्क eval8(char_u **arg, typval_T *rettv, evalarg_T *evalarg, पूर्णाङ्क want_string);
स्थिर पूर्णाङ्क eval9(char_u **arg, typval_T *rettv, evalarg_T *evalarg, पूर्णाङ्क want_string);
स्थिर पूर्णाङ्क eval9_leader(typval_T *rettv, पूर्णाङ्क numeric_only, char_u *start_leader, char_u **end_leaderp);

स्थिर char_u *make_expanded_name(char_u *in_start, char_u *व्यञ्जकम्_start, char_u *व्यञ्जकम्_end, char_u *in_end);

/*
 * Return "n1" divided by "n2", taking care of dividing by zero.
 * If "failed" is not NULL set it to TRUE when dividing by zero fails.
 */
	varnumber_T
अङ्कः_divide(varnumber_T n1, varnumber_T n2, पूर्णाङ्क *failed)
{
    varnumber_T	result;

    जइ (n2 == 0)
    {
	जइ (in_vim9script())
	{
	    emsg(_(e_divide_by_zero));
	    जइ (failed != रिक्त)
		*failed = TRUE;
	}
	जइ (n1 == 0)
	    result = VARNUM_MIN; // similar to NaN
	अण्णहा जइ (n1 < 0)
	    result = -VARNUM_MAX;
	अण्णहा
	    result = VARNUM_MAX;
    }
    अण्णहा जइ (n1 == VARNUM_MIN && n2 == -1)
    {
	// specific case: trying to do VARNUM_MIN / -1 results in a positive
	// number that doesn't fit in varnumber_T and causes an FPE
	result = VARNUM_MAX;
    }
    अण्णहा
	result = n1 / n2;

    प्रत्यागम result;
}

/*
 * Return "n1" modulus "n2", taking care of dividing by zero.
 * If "failed" is not NULL set it to TRUE when dividing by zero fails.
 */
	varnumber_T
अङ्कः_modulus(varnumber_T n1, varnumber_T n2, पूर्णाङ्क *failed)
{
    जइ (n2 == 0 && in_vim9script())
    {
	emsg(_(e_divide_by_zero));
	जइ (failed != रिक्त)
	    *failed = TRUE;
    }
    प्रत्यागम (n2 == 0) ? 0 : (n1 % n2);
}

/*
 * Initialize the global and v: variables.
 */
    शून्य
eval_init(शून्य)
{
    evalvars_init();
    func_init();
}

#जइ defined(EXITFREE)
    शून्य
eval_clear(शून्य)
{
    evalvars_clear();
    free_scriptnames();  // must come after evalvars_clear().
    free_locales();

    // autoloaded script names
    free_autoload_scriptnames();

    // unreferenced lists, tuples and dicts
    (शून्य)garbage_collect(FALSE);

    // functions not garbage collected
    free_all_functions();
}
#endif

    शून्य
fill_evalarg_from_eap(evalarg_T *evalarg, exarg_T *eap, पूर्णाङ्क skip)
{
    init_evalarg(evalarg);
    evalarg->eval_flags = skip ? 0 : EVAL_EVALUATE;

    जइ (eap == रिक्त)
	प्रत्यागम;

    evalarg->eval_cstack = eap->cstack;
    जइ (sourcing_a_script(eap) || eap->ea_getline == get_list_पङ्क्तिः)
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
    पूर्णाङ्क
eval_to_bool(
    char_u	*arg,
    पूर्णाङ्क		*error,
    exarg_T	*eap,
    पूर्णाङ्क		skip,	    // only parse, don't execute
    पूर्णाङ्क		use_simple_function)
{
    typval_T	tv;
    varnumber_T	retval = FALSE;
    evalarg_T	evalarg;
    पूर्णाङ्क		r;

    fill_evalarg_from_eap(&evalarg, eap, skip);

    जइ (skip)
	++emsg_skip;
    जइ (use_simple_function)
	r = eval0_simple_funccal(arg, &tv, eap, &evalarg);
    अण्णहा
	r = eval0(arg, &tv, eap, &evalarg);
    जइ (r == FAIL)
	*error = TRUE;
    अण्णहा
    {
	*error = FALSE;
	जइ (!skip)
	{
	    जइ (in_vim9script())
		retval = tv_get_bool_chk(&tv, error);
	    अण्णहा
		retval = (tv_get_सङ्ख्या_chk(&tv, error) != 0);
	    clear_tv(&tv);
	}
    }
    जइ (skip)
	--emsg_skip;
    clear_evalarg(&evalarg, eap);

    प्रत्यागम (पूर्णाङ्क)retval;
}

/*
 * Call eval1() and give an error message if not done at a lower level.
 */
    स्थिर पूर्णाङ्क
eval1_emsg(char_u **arg, typval_T *rettv, exarg_T *eap)
{
    char_u	*start = *arg;
    पूर्णाङ्क		ret;
    पूर्णाङ्क		did_emsg_before = did_emsg;
    पूर्णाङ्क		called_emsg_before = called_emsg;
    evalarg_T	evalarg;

    fill_evalarg_from_eap(&evalarg, eap, eap != रिक्त && eap->skip);

    ret = eval1(arg, rettv, &evalarg);
    जइ (ret == FAIL)
    {
	// Report the invalid expression unless the expression evaluation has
	// been cancelled due to an aborting error, an interrupt, or an
	// exception, or we already gave a more specific error.
	// Also check called_emsg for when using assert_fails().
	जइ (!aborting() && did_emsg == did_emsg_before
					  && called_emsg == called_emsg_before)
	    semsg(_(e_invalid_expression_str), start);
    }
    clear_evalarg(&evalarg, eap);
    प्रत्यागम ret;
}

/*
 * Return whether a typval is a valid expression to pass to eval_expr_typval()
 * or eval_expr_to_bool().  An empty string returns FALSE;
 */
    पूर्णाङ्क
eval_व्यञ्जकम्_valid_arg(typval_T *tv)
{
    प्रत्यागम tv->v_type != VAR_UNKNOWN
	    && (tv->v_type != VAR_STRING
		  || (tv->vval.v_string != रिक्त && *tv->vval.v_string != NUL));
}

/*
 * When calling eval_expr_typval() many times we only need one funccall_T.
 * Returns NULL when no funccall_T is to be used.
 * When returning non-NULL remove_funccal() must be called later.
 */
    funccall_T *
eval_व्यञ्जकम्_get_funccal(typval_T *व्यञ्जकम्, typval_T *rettv)
{
    जइ (व्यञ्जकम्->v_type != VAR_PARTIAL)
	प्रत्यागम रिक्त;

    partial_T *partial = व्यञ्जकम्->vval.v_partial;
    जइ (partial == रिक्त)
	प्रत्यागम रिक्त;
    जइ (partial->pt_func == रिक्त
	    || partial->pt_func->uf_def_status == UF_NOT_COMPILED)
	प्रत्यागम रिक्त;

    प्रत्यागम create_funccal(partial->pt_func, rettv);
}

/*
 * Evaluate a partial.
 * Pass arguments "argv[argc]".
 * "fc_arg" is from eval_expr_get_funccal() or NULL;
 * Return the result in "rettv" and OK or FAIL.
 */
    स्थिर पूर्णाङ्क
eval_व्यञ्जकम्_partial(
    typval_T	*व्यञ्जकम्,
    typval_T	*argv,
    पूर्णाङ्क		argc,
    funccall_T	*fc_arg,
    typval_T	*rettv)
{
    partial_T	*partial = व्यञ्जकम्->vval.v_partial;

    जइ (partial == रिक्त)
	प्रत्यागम FAIL;

    जइ (partial->pt_func != रिक्त
			&& partial->pt_func->uf_def_status != UF_NOT_COMPILED)
    {
	funccall_T	*fc = fc_arg != रिक्त ? fc_arg
				: create_funccal(partial->pt_func, rettv);
	पूर्णाङ्क		r;

	जइ (fc == रिक्त)
	    प्रत्यागम FAIL;

	// Shortcut to call a compiled function with minimal overhead.
	जइ (partial->pt_obj != रिक्त)
	    partial->pt_obj->obj_refcount++;
	r = call_def_function(partial->pt_func, argc, argv, DEF_USE_PT_ARGV,
					partial, partial->pt_obj, fc, rettv);
	जइ (fc_arg == रिक्त)
	    remove_funccal();
	जइ (r == FAIL)
	    प्रत्यागम FAIL;
    }
    अण्णहा
    {
	char_u		*s = partial_name(partial);
	funcexe_T	funcexe;

	जइ (s == रिक्त || *s == NUL)
	    प्रत्यागम FAIL;

	CLEAR_FIELD(funcexe);
	funcexe.fe_evaluate = TRUE;
	funcexe.fe_partial = partial;
	जइ (call_func(s, -1, rettv, argc, argv, &funcexe) == FAIL)
	    प्रत्यागम FAIL;
    }

    प्रत्यागम OK;
}

/*
 * Evaluate an expression which is a function.
 * Pass arguments "argv[argc]".
 * Return the result in "rettv" and OK or FAIL.
 */
    स्थिर पूर्णाङ्क
eval_व्यञ्जकम्_func(
    typval_T	*व्यञ्जकम्,
    typval_T	*argv,
    पूर्णाङ्क		argc,
    typval_T	*rettv)
{
    funcexe_T	funcexe;
    char_u	buf[NUMBUFLEN];
    char_u	*s;

    जइ (व्यञ्जकम्->v_type == VAR_FUNC)
	s = व्यञ्जकम्->vval.v_string;
    अण्णहा
	s = tv_get_string_buf_chk_strict(व्यञ्जकम्, buf, in_vim9script());
    जइ (s == रिक्त || *s == NUL)
	प्रत्यागम FAIL;

    CLEAR_FIELD(funcexe);
    funcexe.fe_evaluate = TRUE;
    जइ (call_func(s, -1, rettv, argc, argv, &funcexe) == FAIL)
	प्रत्यागम FAIL;

    प्रत्यागम OK;
}

/*
 * Evaluate an expression, which is a string.
 * Return the result in "rettv" and OK or FAIL.
 */
    स्थिर पूर्णाङ्क
eval_व्यञ्जकम्_string(
    typval_T	*व्यञ्जकम्,
    typval_T	*rettv)
{
    char_u	*s;
    char_u	buf[NUMBUFLEN];

    s = tv_get_string_buf_chk_strict(व्यञ्जकम्, buf, in_vim9script());
    जइ (s == रिक्त)
	प्रत्यागम FAIL;

    s = skipwhite(s);
    जइ (eval1_emsg(&s, rettv, रिक्त) == FAIL)
	प्रत्यागम FAIL;

    जइ (*skipwhite(s) != NUL)  // check for trailing chars after expr
    {
	clear_tv(rettv);
	semsg(_(e_invalid_expression_str), s);
	प्रत्यागम FAIL;
    }

    प्रत्यागम OK;
}

/*
 * Evaluate an expression, which can be a function, partial or string.
 * Pass arguments "argv[argc]".
 * If "want_func" is TRUE treat a string as a function name, not an expression.
 * "fc_arg" is from eval_expr_get_funccal() or NULL;
 * Return the result in "rettv" and OK or FAIL.
 */
    पूर्णाङ्क
eval_व्यञ्जकम्_typval(
	typval_T    *व्यञ्जकम्,
	पूर्णाङ्क	    want_func,
	typval_T    *argv,
	पूर्णाङ्क	    argc,
	funccall_T  *fc_arg,
	typval_T    *rettv)
{
    जइ (व्यञ्जकम्->v_type == VAR_PARTIAL)
	प्रत्यागम eval_व्यञ्जकम्_partial(व्यञ्जकम्, argv, argc, fc_arg, rettv);
    जइ (व्यञ्जकम्->v_type == VAR_INSTR)
	प्रत्यागम exe_typval_instr(व्यञ्जकम्, rettv);
    जइ (व्यञ्जकम्->v_type == VAR_FUNC || want_func)
	प्रत्यागम eval_व्यञ्जकम्_func(व्यञ्जकम्, argv, argc, rettv);

    प्रत्यागम eval_व्यञ्जकम्_string(व्यञ्जकम्, rettv);
}

/*
 * Like eval_to_bool() but using a typval_T instead of a string.
 * Works for string, funcref and partial.
 */
    पूर्णाङ्क
eval_व्यञ्जकम्_to_bool(typval_T *व्यञ्जकम्, पूर्णाङ्क *error)
{
    typval_T	rettv;
    पूर्णाङ्क		res;

    जइ (eval_व्यञ्जकम्_typval(व्यञ्जकम्, FALSE, रिक्त, 0, रिक्त, &rettv) == FAIL)
    {
	*error = TRUE;
	प्रत्यागम FALSE;
    }
    res = (tv_get_bool_chk(&rettv, error) != 0);
    clear_tv(&rettv);
    प्रत्यागम res;
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
    पूर्णाङ्क		skip)	    // only parse, don't execute
{
    typval_T	tv;
    char_u	*retval;
    evalarg_T	evalarg;

    fill_evalarg_from_eap(&evalarg, eap, skip);
    जइ (skip)
	++emsg_skip;
    जइ (eval0(arg, &tv, eap, &evalarg) == FAIL || skip)
	retval = रिक्त;
    अण्णहा
    {
	retval = vim_strsave(tv_get_string(&tv));
	clear_tv(&tv);
    }
    जइ (skip)
	--emsg_skip;
    clear_evalarg(&evalarg, eap);

    प्रत्यागम retval;
}

/*
 * Initialize "evalarg" for use.
 */
    शून्य
init_evalarg(evalarg_T *evalarg)
{
    CLEAR_POINTER(evalarg);
    ga_init2(&evalarg->eval_tofree_ga, परिमाण(char_u *), 20);
}

/*
 * If "evalarg->eval_tofree" is not NULL free it later.
 * Caller is expected to overwrite "evalarg->eval_tofree" next.
 */
    स्थिर शून्य
free_eval_tofree_later(evalarg_T *evalarg)
{
    जइ (evalarg->eval_tofree == रिक्त)
	प्रत्यागम;

    जइ (ga_grow(&evalarg->eval_tofree_ga, 1) == OK)
	((char_u **)evalarg->eval_tofree_ga.ga_data)
	    [evalarg->eval_tofree_ga.ga_len++]
	    = evalarg->eval_tofree;
    अण्णहा
	vim_free(evalarg->eval_tofree);
}

/*
 * After using "evalarg" filled from "eap": free the memory.
 */
    शून्य
clear_evalarg(evalarg_T *evalarg, exarg_T *eap)
{
    जइ (evalarg == रिक्त)
	प्रत्यागम;

    garray_T *etga = &evalarg->eval_tofree_ga;

    जइ (evalarg->eval_tofree != रिक्त || evalarg->eval_using_cmdline)
    {
	जइ (eap != रिक्त)
	{
	    // We may need to keep the original command line, e.g. for
	    // ":let" it has the variable names.  But we may also need
	    // the new one, "nextcmd" points into it.  Keep both.
	    vim_free(eap->cmdline_tofree);
	    eap->cmdline_tofree = *eap->cmdlinep;

	    जइ (evalarg->eval_using_cmdline && etga->ga_len > 0)
	    {
		// "nextcmd" points into the last line in eval_tofree_ga,
		// need to keep it around.
		--etga->ga_len;
		*eap->cmdlinep = ((char_u **)etga->ga_data)[etga->ga_len];
		vim_free(evalarg->eval_tofree);
	    }
	    अण्णहा
		*eap->cmdlinep = evalarg->eval_tofree;
	}
	अण्णहा
	    vim_free(evalarg->eval_tofree);
	evalarg->eval_tofree = रिक्त;
    }

    ga_clear_strings(etga);
    VIM_CLEAR(evalarg->eval_tofree_lambda);
}

/*
 * Skip over an expression at "*pp".
 * Return FAIL for an error, OK otherwise.
 */
    पूर्णाङ्क
skip_व्यञ्जकम्(char_u **pp, evalarg_T *evalarg)
{
    typval_T	rettv;

    *pp = skipwhite(*pp);
    प्रत्यागम eval1(pp, &rettv, evalarg);
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
    पूर्णाङ्क
skip_व्यञ्जकम्_concatenate(
	char_u	    **arg,
	char_u	    **start,
	char_u	    **end,
	evalarg_T   *evalarg)
{
    typval_T	rettv;
    पूर्णाङ्क		res;
    पूर्णाङ्क		vim9script = in_vim9script();
    garray_T    *gap = evalarg == रिक्त ? रिक्त : &evalarg->eval_ga;
    garray_T    *freegap = evalarg == रिक्त ? रिक्त : &evalarg->eval_freega;
    पूर्णाङ्क		save_flags = evalarg == रिक्त ? 0 : evalarg->eval_flags;
    पूर्णाङ्क		evaluate = evalarg == रिक्त
			       ? FALSE : (evalarg->eval_flags & EVAL_EVALUATE);

    जइ (vim9script && evaluate
	       && (evalarg->eval_cookie != रिक्त || evalarg->eval_cctx != रिक्त))
    {
	ga_init2(gap, परिमाण(char_u *), 10);
	// leave room for "start"
	जइ (ga_grow(gap, 1) == OK)
	    ++gap->ga_len;
	ga_init2(freegap, परिमाण(char_u *), 10);
    }
    *start = *arg;

    // Don't evaluate the expression.
    जइ (evalarg != रिक्त)
	evalarg->eval_flags &= ~EVAL_EVALUATE;
    *arg = skipwhite(*arg);
    res = eval1(arg, &rettv, evalarg);
    *end = *arg;
    जइ (evalarg != रिक्त)
	evalarg->eval_flags = save_flags;

    जइ (vim9script && evaluate
	    && (evalarg->eval_cookie != रिक्त || evalarg->eval_cctx != रिक्त))
    {
	जइ (evalarg->eval_ga.ga_len == 1)
	{
	    // just the one line, no need to concatenate
	    ga_clear(gap);
	    gap->ga_itemsize = 0;
	}
	अण्णहा
	{
	    char_u	    *p;
	    size_t	    endoff = STRLEN(*arg);

	    // Line breaks encountered, concatenate all the lines.
	    *((char_u **)gap->ga_data) = *start;
	    p = ga_concat_strings(gap, " ");

	    // free the lines only when using getsourceline()
	    जइ (evalarg->eval_cookie != रिक्त)
	    {
		// Do not free the first line, the caller can still use it.
		*((char_u **)gap->ga_data) = रिक्त;
		// Do not free the last line, "arg" points into it, free it
		// later.  Also free "eval_tofree" later if needed.
		free_eval_tofree_later(evalarg);
		evalarg->eval_tofree =
				    ((char_u **)gap->ga_data)[gap->ga_len - 1];
		((char_u **)gap->ga_data)[gap->ga_len - 1] = रिक्त;
		ga_clear_strings(gap);
		ga_clear(freegap);
	    }
	    अण्णहा
	    {
		ga_clear(gap);

		// free lines that were explicitly marked for freeing
		ga_clear_strings(freegap);
	    }

	    gap->ga_itemsize = 0;
	    जइ (p == रिक्त)
		प्रत्यागम FAIL;
	    *start = p;
	    vim_free(evalarg->eval_tofree_lambda);
	    evalarg->eval_tofree_lambda = p;
	    // Compute "end" relative to the end.
	    *end = *start + STRLEN(*start) - endoff;
	}
    }

    प्रत्यागम res;
}

/*
 * Convert "tv" to a string.
 * When "join_list" is TRUE convert a List or a Tuple into a sequence of lines.
 * Returns an allocated string (NULL when out of memory).
 */
    char_u *
typval2string(typval_T *tv, पूर्णाङ्क join_list)
{
    garray_T	ga;
    char_u	*retval = रिक्त;

    जइ (join_list && (tv->v_type == VAR_LIST || tv->v_type == VAR_TUPLE))
    {
	जइ (tv->v_type == VAR_LIST)
	{
	    ga_init2(&ga, परिमाण(वर्ण), 80);
	    जइ (tv->vval.v_list != रिक्त)
	    {
		list_join(&ga, tv->vval.v_list, (char_u *)"\n", TRUE, FALSE,
									0);
		जइ (tv->vval.v_list->lv_len > 0)
		    ga_append(&ga, NL);
	    }
	    ga_append(&ga, NUL);
	    retval = (char_u *)ga.ga_data;
	}
	अण्णहा
	{
	    // tuple
	    ga_init2(&ga, परिमाण(वर्ण), 80);
	    जइ (tv->vval.v_tuple != रिक्त)
	    {
		tuple_join(&ga, tv->vval.v_tuple, (char_u *)"\n", TRUE, FALSE,
									0);
		जइ (TUPLE_LEN(tv->vval.v_tuple) > 0)
		    ga_append(&ga, NL);
	    }
	    ga_append(&ga, NUL);
	    retval = (char_u *)ga.ga_data;
	}
    }
    अण्णहा जइ (tv->v_type == VAR_LIST
	    || tv->v_type == VAR_TUPLE
	    || tv->v_type == VAR_DICT)
    {
	char_u	*tofree;
	char_u	numbuf[NUMBUFLEN];

	retval = tv2string(tv, &tofree, numbuf, 0);
	// Make a copy if we have a value but it's not in allocated memory.
	जइ (retval != रिक्त && tofree == रिक्त)
	    retval = vim_strsave(retval);
    }
    अण्णहा
	retval = vim_strsave(tv_get_string(tv));
    प्रत्यागम retval;
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
    पूर्णाङ्क		join_list,
    exarg_T	*eap,
    पूर्णाङ्क		use_simple_function)
{
    typval_T	tv;
    char_u	*retval;
    evalarg_T	evalarg;
    पूर्णाङ्क		r;

    fill_evalarg_from_eap(&evalarg, eap, eap != रिक्त && eap->skip);
    जइ (use_simple_function)
	r = eval0_simple_funccal(arg, &tv, रिक्त, &evalarg);
    अण्णहा
	r = eval0(arg, &tv, रिक्त, &evalarg);
    जइ (r == FAIL)
	retval = रिक्त;
    अण्णहा
    {
	retval = typval2string(&tv, join_list);
	clear_tv(&tv);
    }
    clear_evalarg(&evalarg, रिक्त);

    प्रत्यागम retval;
}

    char_u *
eval_to_string(
    char_u	*arg,
    पूर्णाङ्क		join_list,
    पूर्णाङ्क		use_simple_function)
{
    प्रत्यागम eval_to_string_eap(arg, join_list, रिक्त, use_simple_function);
}

/*
 * Call eval_to_string() without using current local variables and using
 * textlock.  When "use_sandbox" is TRUE use the sandbox.
 * Use legacy Vim script syntax.
 */
    char_u *
eval_to_string_safe(
    char_u	*arg,
    पूर्णाङ्क		use_sandbox,
    पूर्णाङ्क		keep_script_version,
    पूर्णाङ्क		use_simple_function)
{
    char_u	*retval;
    funccal_entry_T funccal_entry;
    पूर्णाङ्क		save_sc_version = current_sctx.sc_version;
    पूर्णाङ्क		save_garbage = may_garbage_collect;

    जइ (!keep_script_version)
	current_sctx.sc_version = 1;
    save_funccal(&funccal_entry);
    जइ (use_sandbox)
	++sandbox;
    ++textlock;
    may_garbage_collect = FALSE;
    retval = eval_to_string(arg, FALSE, use_simple_function);
    जइ (use_sandbox)
	--sandbox;
    --textlock;
    may_garbage_collect = save_garbage;
    restore_funccal();
    current_sctx.sc_version = save_sc_version;
    प्रत्यागम retval;
}

/*
 * Top level evaluation function, returning a number.
 * Evaluates "expr" silently.
 * Returns -1 for an error.
 */
    varnumber_T
eval_to_सङ्ख्या(char_u *व्यञ्जकम्, पूर्णाङ्क use_simple_function)
{
    typval_T	rettv;
    varnumber_T	retval;
    char_u	*p = skipwhite(व्यञ्जकम्);
    पूर्णाङ्क		r = NOTDONE;

    ++emsg_off;

    जइ (use_simple_function)
	r = may_call_simple_func(व्यञ्जकम्, &rettv);
    जइ (r == NOTDONE)
	r = eval1(&p, &rettv, &EVALARG_EVALUATE);
    जइ (r == FAIL)
	retval = -1;
    अण्णहा
    {
	retval = tv_get_सङ्ख्या_chk(&rettv, रिक्त);
	clear_tv(&rettv);
    }
    --emsg_off;

    प्रत्यागम retval;
}

/*
 * Top level evaluation function.
 * Returns an allocated typval_T with the result.
 * Returns NULL when there is an error.
 */
    typval_T *
eval_व्यञ्जकम्(char_u *arg, exarg_T *eap)
{
    प्रत्यागम eval_व्यञ्जकम्_ext(arg, eap, FALSE);
}

    typval_T *
eval_व्यञ्जकम्_ext(char_u *arg, exarg_T *eap, पूर्णाङ्क use_simple_function)
{
    typval_T	*tv;
    evalarg_T	evalarg;

    fill_evalarg_from_eap(&evalarg, eap, eap != रिक्त && eap->skip);

    tv = ALLOC_ONE(typval_T);
    जइ (tv != रिक्त)
    {
	पूर्णाङ्क	r = NOTDONE;

	जइ (use_simple_function)
	    r = eval0_simple_funccal(arg, tv, eap, &evalarg);
	जइ (r == NOTDONE)
	    r = eval0(arg, tv, eap, &evalarg);

	जइ (r == FAIL)
	    VIM_CLEAR(tv);
    }

    clear_evalarg(&evalarg, eap);
    प्रत्यागम tv;
}

/*
 * "*arg" points to what can be a function name in the form of "import.Name" or
 * "Funcref".  Return the name of the function.  Set "tofree" to something that
 * was allocated.
 * If "verbose" is FALSE no errors are given.
 * Return NULL for any failure.
 */
    स्थिर char_u *
deref_function_name(
	    char_u	**arg,
	    char_u	**tofree,
	    evalarg_T	*evalarg,
	    पूर्णाङ्क		verbose)
{
    typval_T	ref;
    char_u	*name = *arg;
    पूर्णाङ्क		save_flags = 0;
    पूर्णाङ्क		evaluate = evalarg != रिक्त
				      && (evalarg->eval_flags & EVAL_EVALUATE);

    ref.v_type = VAR_UNKNOWN;
    जइ (evalarg != रिक्त)
    {
	// need to evaluate this to get an import, like in "a.Func"
	save_flags = evalarg->eval_flags;
	evalarg->eval_flags |= EVAL_EVALUATE;
    }
    जइ (eval9(arg, &ref, evalarg, FALSE) == FAIL)
    {
	dictitem_T	*v;

	// If <SID>VarName was used it would not be found, try another way.
	v = find_var_also_in_script(name, रिक्त, FALSE);
	जइ (v == रिक्त)
	{
	    name = रिक्त;
	    गच्छ theend;
	}
	copy_tv(&v->di_tv, &ref);
    }
    जइ (*skipwhite(*arg) != NUL)
    {
	जइ (verbose)
	    semsg(_(e_trailing_characters_str), *arg);
	name = रिक्त;
    }
    अण्णहा जइ (ref.v_type == VAR_FUNC && ref.vval.v_string != रिक्त)
    {
	name = ref.vval.v_string;
	ref.vval.v_string = रिक्त;
	*tofree = name;
    }
    अण्णहा जइ (ref.v_type == VAR_PARTIAL && ref.vval.v_partial != रिक्त)
    {
	जइ (ref.vval.v_partial->pt_argc > 0
		|| ref.vval.v_partial->pt_dict != रिक्त)
	{
	    जइ (verbose)
		emsg(_(e_cannot_use_partial_here));
	    name = रिक्त;
	}
	अण्णहा
	{
	    name = vim_strsave(partial_name(ref.vval.v_partial));
	    *tofree = name;
	}
    }
    अण्णहा जइ (evaluate)
    {
	जइ (verbose)
	    semsg(_(e_not_callable_type_str), name);
	name = रिक्त;
    }

theend:
    clear_tv(&ref);
    जइ (evalarg != रिक्त)
	evalarg->eval_flags = save_flags;
    प्रत्यागम name;
}

/*
 * Call some Vim script function and return the result in "*rettv".
 * Uses argv[0] to argv[argc - 1] for the function arguments.  argv[argc]
 * should have type VAR_UNKNOWN.
 * Returns OK or FAIL.
 */
    पूर्णाङ्क
call_vim_function(
    char_u      *func,
    पूर्णाङ्क		argc,
    typval_T	*argv,
    typval_T	*rettv)
{
    पूर्णाङ्क		ret;
    funcexe_T	funcexe;
    char_u	*arg;
    char_u	*name;
    char_u	*tofree = रिक्त;
    पूर्णाङ्क		ignore_errors;

    rettv->v_type = VAR_UNKNOWN;		// clear_tv() uses this
    CLEAR_FIELD(funcexe);
    funcexe.fe_firstline = curwin->w_cursor.lnum;
    funcexe.fe_lastline = curwin->w_cursor.lnum;
    funcexe.fe_evaluate = TRUE;

    // The name might be "import.Func" or "Funcref".  We don't know, we need to
    // ignore errors for an undefined name.  But we do want errors when an
    // autoload script has errors.  Guess that when there is a dot in the name
    // showing errors is the right choice.
    ignore_errors = vim_strchr(func, '.') == रिक्त;
    arg = func;
    जइ (ignore_errors)
	++emsg_off;
    name = deref_function_name(&arg, &tofree, &EVALARG_EVALUATE, FALSE);
    जइ (ignore_errors)
	--emsg_off;
    जइ (name == रिक्त)
	name = func;

    ret = call_func(name, -1, rettv, argc, argv, &funcexe);

    जइ (ret == FAIL)
	clear_tv(rettv);
    vim_free(tofree);

    प्रत्यागम ret;
}

/*
 * Call Vim script function "func" and return the result as a string.
 * Uses "argv[0]" to "argv[argc - 1]" for the function arguments. "argv[argc]"
 * should have type VAR_UNKNOWN.
 * Returns NULL when calling the function fails.
 */
    शून्य *
call_func_retstr(
    char_u      *func,
    पूर्णाङ्क		argc,
    typval_T	*argv)
{
    typval_T	rettv;
    char_u	*retval;

    जइ (call_vim_function(func, argc, argv, &rettv) == FAIL)
	प्रत्यागम रिक्त;

    retval = vim_strsave(tv_get_string(&rettv));
    clear_tv(&rettv);
    प्रत्यागम retval;
}

/*
 * Call Vim script function "func" and return the result as a List.
 * Uses "argv" and "argc" as call_func_retstr().
 * Returns NULL when there is something wrong.
 * Gives an error when the returned value is not a list.
 */
    शून्य *
call_func_retlist(
    char_u      *func,
    पूर्णाङ्क		argc,
    typval_T	*argv)
{
    typval_T	rettv;

    जइ (call_vim_function(func, argc, argv, &rettv) == FAIL)
	प्रत्यागम रिक्त;

    जइ (rettv.v_type != VAR_LIST)
    {
	semsg(_(e_custom_list_completion_function_does_not_return_list_but_str),
		vartype_name(rettv.v_type));
	clear_tv(&rettv);
	प्रत्यागम रिक्त;
    }

    प्रत्यागम rettv.vval.v_list;
}

#जइ defined(FEAT_FOLDING)
/*
 * Evaluate "arg", which is 'foldexpr'.
 * Note: caller must set "curwin" to match "arg".
 * Returns the foldlevel, and any character preceding it in "*cp".  Doesn't
 * give error messages.
 */
    पूर्णाङ्क
eval_foldexpr(win_T *wp, पूर्णाङ्क *cp)
{
    char_u	*arg;
    typval_T	tv;
    varnumber_T	retval;
    char_u	*s;
    sctx_T	saved_sctx = current_sctx;
    पूर्णाङ्क		use_sandbox = was_set_insecurely(wp, (char_u *)"foldexpr",
								    OPT_LOCAL);

    arg = skipwhite(wp->w_p_fde);
    current_sctx = wp->w_p_script_ctx[WV_FDE];

    ++emsg_off;
    जइ (use_sandbox)
	++sandbox;
    ++textlock;
    *cp = NUL;

    // Evaluate the expression.  If the expression is "FuncName()" call the
    // function directly.
    जइ (eval0_simple_funccal(arg, &tv, रिक्त, &EVALARG_EVALUATE) == FAIL)
	retval = 0;
    अण्णहा
    {
	// If the result is a number, just return the number.
	जइ (tv.v_type == VAR_सङ्ख्या)
	    retval = tv.vval.v_सङ्ख्या;
	अण्णहा जइ (tv.v_type != VAR_STRING || tv.vval.v_string == रिक्त)
	    retval = 0;
	अण्णहा
	{
	    // If the result is a string, check if there is a non-digit before
	    // the number.
	    s = tv.vval.v_string;
	    जइ (*s != NUL && !VIM_ISDIGIT(*s) && *s != '-')
		*cp = *s++;
	    retval = atol((वर्ण *)s);
	}
	clear_tv(&tv);
    }
    --emsg_off;
    जइ (use_sandbox)
	--sandbox;
    --textlock;
    clear_evalarg(&EVALARG_EVALUATE, रिक्त);
    current_sctx = saved_sctx;

    प्रत्यागम (पूर्णाङ्क)retval;
}
#endif

#ifdef LOG_LOCKVAR
प्रकारनिर्देश संरचना flag_string_S
{
    पूर्णाङ्क	    flag;
    वर्ण    *str;
} flag_string_T;

    स्थिर वर्ण *
flags_tostring(पूर्णाङ्क flags, flag_string_T *_fstring, वर्ण *buf, size_t n)
{
    वर्ण *p = buf;
    *p = NUL;
    प्रति (flag_string_T *fstring = _fstring; fstring->flag; ++fstring)
    {
	जइ ((fstring->flag & flags) != 0)
	{
	    size_t len = STRLEN(fstring->str);
	    जइ (n > p - buf + len + 7)
	    {
		STRCAT(p, fstring->str);
		p += len;
		STRCAT(p, " ");
		++p;
	    }
	    अण्णहा
	    {
		STRCAT(buf, "...");
		भङ्ग;
	    }
	}
    }
    प्रत्यागम buf;
}

flag_string_T glv_flag_strings[] = {
    { GLV_QUIET,		"QUIET" },
    { GLV_NO_AUTOLOAD,		"NO_AUTOLOAD" },
    { GLV_READ_ONLY,		"READ_ONLY" },
    { GLV_NO_DECL,		"NO_DECL" },
    { GLV_COMPILING,		"COMPILING" },
    { GLV_ASSIGN_WITH_OP,	"ASSIGN_WITH_OP" },
    { GLV_PREFER_FUNC,		"PREFER_FUNC" },
    { 0,			रिक्त }
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
    स्थिर शून्य
fill_lval_from_lval_मूलम्(lval_T *lp, lval_मूलम्_T *lr)
{
#ifdef LOG_LOCKVAR
    ch_log(रिक्त, "LKVAR: fill_lval_from_lval_root(): name %s, tv %p",
						lp->ll_name, (शून्य*)lr->lr_tv);
#endif
    जइ (lr->lr_tv == रिक्त)
	प्रत्यागम;
    जइ (!lr->lr_is_arg && lr->lr_tv->v_type == VAR_CLASS)
    {
	जइ (lr->lr_tv->vval.v_class != रिक्त)
	{
	    // Special special case. Look for a bare class variable reference.
	    class_T	*cl = lr->lr_tv->vval.v_class;
	    पूर्णाङ्क		m_idx;
	    ocmember_T	*m = class_member_lookup(cl, lp->ll_name,
					lp->ll_name_end - lp->ll_name, &m_idx);
	    जइ (m != रिक्त)
	    {
		// Assuming "inside class" since bare reference.
		lp->ll_class = lr->lr_tv->vval.v_class;
		lp->ll_oi = m_idx;
		lp->ll_valtype = m->ocm_type;
		lp->ll_tv = &lp->ll_class->class_members_tv[m_idx];
#ifdef LOG_LOCKVAR
		ch_log(रिक्त, "LKVAR:    ... class member %s.%s",
					lp->ll_class->class_name.string, lp->ll_name);
#endif
		प्रत्यागम;
	    }
	}
    }

#ifdef LOG_LOCKVAR
    ch_log(रिक्त, "LKVAR:    ... type: %s", vartype_name(lr->lr_tv->v_type));
#endif
    lp->ll_tv = lr->lr_tv;
    lp->ll_is_मूलम् = TRUE;
}

/*
 * Check if the class has permission to access the member.
 * Returns OK or FAIL.
 */
    स्थिर पूर्णाङ्क
get_lval_check_access(
    class_T	*cl_exec,   // executing class, NULL if :def or script level
    class_T	*cl,	    // class which contains the member
    ocmember_T	*om,	    // member being accessed
    char_u	*p,	    // char after member name
    पूर्णाङ्क		flags)	    // GLV flags to check if writing to lval
{
#ifdef LOG_LOCKVAR
    ch_log(रिक्त, "LKVAR: get_lval_check_access(), cl_exec %p, cl %p, %c",
						(शून्य*)cl_exec, (शून्य*)cl, *p);
#endif
    जइ (cl_exec != रिक्त && cl_exec == cl)
	प्रत्यागम OK;

    वर्ण *सन्देशः = रिक्त;
    विकल्प (om->ocm_access)
    {
	पक्ष VIM_ACCESS_PRIVATE:
	    सन्देशः = e_cannot_access_protected_variable_str;
	    भङ्ग;
	पक्ष VIM_ACCESS_READ:
	    // If [idx] or .key following, read only OK.
	    जइ (*p == '[' || *p == '.')
		भङ्ग;
	    जइ ((flags & GLV_READ_ONLY) == 0)
	    {
		जइ (IS_ENUM(cl))
		{
		    जइ (om->ocm_type->tt_type == VAR_OBJECT)
			semsg(_(e_enumvalue_str_cannot_be_modified),
				cl->class_name.string, om->ocm_name.string);
		    अण्णहा
			सन्देशः = e_variable_is_not_writable_str;
		}
		अण्णहा
		    सन्देशः = e_variable_is_not_writable_str;
	    }
	    भङ्ग;
	पक्ष VIM_ACCESS_ALL:
	    भङ्ग;
    }
    जइ (सन्देशः != रिक्त)
    {
	emsg_var_cl_define(सन्देशः, om->ocm_name.string, 0, cl);
	प्रत्यागम FAIL;
    }
    प्रत्यागम OK;
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
    स्थिर char_u *
get_lval_imported(
    lval_T	*lp,
    scid_T	imp_sid,
    char_u	*p,
    dictitem_T	**dip,
    पूर्णाङ्क		fne_flags)
{
    ufunc_T	*ufunc;
    type_T	*type = रिक्त;
    पूर्णाङ्क		cc;
    पूर्णाङ्क		rc = FAIL;

    p = skipwhite(p);

    import_check_sourced_sid(&imp_sid);
    lp->ll_sid = imp_sid;
    lp->ll_name = p;
    p = find_name_end(lp->ll_name, रिक्त, रिक्त, fne_flags);
    lp->ll_name_end = p;

    // check the item is exported
    cc = *p;
    *p = NUL;
    जइ (find_exported(imp_sid, lp->ll_name, &ufunc, &type, रिक्त, रिक्त,
								TRUE) == -1)
	गच्छ failed;

    // Get the typval for the exported item
    hashtab_T *ht = &SCRIPT_VARS(imp_sid);
    जइ (ht == रिक्त)
	गच्छ failed;

    dictitem_T *di = find_var_in_ht(ht, 0, lp->ll_name, TRUE);
    जइ (di == रिक्त)
	// script is autoloaded.  So variable will be found later
	गच्छ success;

    *dip = di;

    // Check whether the variable is writable.
    svar_T *sv = find_typval_in_script(&di->di_tv, imp_sid, FALSE);
    जइ (sv != रिक्त && sv->sv_const != 0)
    {
	semsg(_(e_cannot_change_readonly_variable_str), lp->ll_name);
	गच्छ failed;
    }

    // check whether variable is locked
    जइ (मानम्_check_lock(di->di_tv.v_lock, lp->ll_name, FALSE))
	गच्छ failed;

    lp->ll_tv = &di->di_tv;
    lp->ll_valtype = type;

success:
    rc = OK;

failed:
    *p = cc;
    प्रत्यागम rc == OK ? p : रिक्त;
}

प्रकारनिर्देश गणना {
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
    स्थिर पूर्णाङ्क
get_lval_dict_item(
    lval_T	*lp,
    char_u	*name,
    char_u	*कुञ्चिका,
    पूर्णाङ्क		len,
    char_u	**कुञ्चिका_end,
    typval_T	*var1,
    पूर्णाङ्क		flags,
    पूर्णाङ्क		unlet,
    typval_T	*rettv)
{
    पूर्णाङ्क		quiet = flags & GLV_QUIET;
    char_u	*p = *कुञ्चिका_end;

    जइ (len == -1)
    {
	// "[key]": get key from "var1"
	कुञ्चिका = tv_get_string_chk(var1);	// is number or string
	जइ (कुञ्चिका == रिक्त)
	    प्रत्यागम GLV_FAIL;
    }
    lp->ll_list = रिक्त;
    lp->ll_list = रिक्त;
    lp->ll_blob = रिक्त;
    lp->ll_object = रिक्त;
    lp->ll_class = रिक्त;
    lp->ll_tuple = रिक्त;

    // a NULL dict is equivalent with an empty dict
    जइ (lp->ll_tv->vval.v_dict == रिक्त)
    {
	lp->ll_tv->vval.v_dict = dict_alloc();
	जइ (lp->ll_tv->vval.v_dict == रिक्त)
	    प्रत्यागम GLV_FAIL;
	++lp->ll_tv->vval.v_dict->dv_refcount;
    }
    lp->ll_dict = lp->ll_tv->vval.v_dict;

    lp->ll_di = dict_find(lp->ll_dict, कुञ्चिका, len);

    // When assigning to a scope dictionary check that a function and
    // variable name is valid (only variable name unless it is l: or
    // g: dictionary). Disallow overwriting a builtin function.
    जइ (rettv != रिक्त && lp->ll_dict->dv_scope != 0)
    {
	पूर्णाङ्क prevval;

	जइ (len != -1)
	{
	    prevval = कुञ्चिका[len];
	    कुञ्चिका[len] = NUL;
	}
	अण्णहा
	    prevval = 0; // avoid compiler warning
	पूर्णाङ्क wrong = (lp->ll_dict->dv_scope == VAR_DEF_SCOPE
		&& (rettv->v_type == VAR_FUNC
		    || rettv->v_type == VAR_PARTIAL)
		&& var_wrong_func_name(कुञ्चिका, lp->ll_di == रिक्त))
	    || !valid_varname(कुञ्चिका, -1, TRUE);
	जइ (len != -1)
	    कुञ्चिका[len] = prevval;
	जइ (wrong)
	    प्रत्यागम GLV_FAIL;
    }

    जइ (lp->ll_valtype != रिक्त)
	// use the type of the member
	lp->ll_valtype = lp->ll_valtype->tt_member;

    जइ (lp->ll_di == रिक्त)
    {
	// Can't add "v:" or "a:" variable.
	जइ (lp->ll_dict == get_vimvar_dict()
		|| &lp->ll_dict->dv_hashtab == get_funccal_args_ht())
	{
	    semsg(_(e_illegal_variable_name_str), name);
	    प्रत्यागम GLV_FAIL;
	}

	// Key does not exist in dict: may need to add it.
	जइ (*p == '[' || *p == '.' || unlet)
	{
	    जइ (!quiet)
		semsg(_(e_कुञ्चिका_not_present_in_dictionary_str), कुञ्चिका);
	    प्रत्यागम GLV_FAIL;
	}
	जइ (len == -1)
	    lp->ll_newkey = vim_strsave(कुञ्चिका);
	अण्णहा
	    lp->ll_newkey = vim_strnsave(कुञ्चिका, len);
	जइ (lp->ll_newkey == रिक्त)
	    p = रिक्त;

	*कुञ्चिका_end = p;
	प्रत्यागम GLV_STOP;
    }
    // existing variable, need to check if it can be changed
    अण्णहा जइ ((flags & GLV_READ_ONLY) == 0
	    && (var_check_ro(lp->ll_di->di_flags, name, FALSE)
		|| var_check_lock(lp->ll_di->di_flags, name, FALSE)))
	प्रत्यागम GLV_FAIL;

    lp->ll_tv = &lp->ll_di->di_tv;

    प्रत्यागम GLV_OK;
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
    स्थिर पूर्णाङ्क
get_lval_blob(
    lval_T	*lp,
    typval_T	*var1,
    typval_T	*var2,
    पूर्णाङ्क		empty1,
    पूर्णाङ्क		quiet)
{
    long	bloblen = blob_len(lp->ll_tv->vval.v_blob);

    lp->ll_list = रिक्त;
    lp->ll_dict = रिक्त;
    lp->ll_object = रिक्त;
    lp->ll_class = रिक्त;
    lp->ll_tuple = रिक्त;

    // Get the number and item for the only or first index of a List or Tuple.
    जइ (empty1)
	lp->ll_n1 = 0;
    अण्णहा
	// is number or string
	lp->ll_n1 = (long)tv_get_सङ्ख्या(var1);

    जइ (check_blob_index(bloblen, lp->ll_n1, quiet) == FAIL)
	प्रत्यागम FAIL;
    जइ (lp->ll_range && !lp->ll_empty2)
    {
	lp->ll_n2 = (long)tv_get_सङ्ख्या(var2);
	जइ (check_blob_range(bloblen, lp->ll_n1, lp->ll_n2, quiet) == FAIL)
	    प्रत्यागम FAIL;
    }

    जइ (!lp->ll_range)
	// Indexing a single byte in a blob.  So the rhs type is a
	// number.
	lp->ll_valtype = &t_सङ्ख्या;

    lp->ll_blob = lp->ll_tv->vval.v_blob;
    lp->ll_tv = रिक्त;

    प्रत्यागम OK;
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
    स्थिर पूर्णाङ्क
get_lval_list(
    lval_T	*lp,
    typval_T	*var1,
    typval_T	*var2,
    पूर्णाङ्क		empty1,
    पूर्णाङ्क		flags,
    पूर्णाङ्क		quiet)
{
    /*
     * Get the number and item for the only or first index of the List.
     */
    जइ (empty1)
	lp->ll_n1 = 0;
    अण्णहा
	// is number or string
	lp->ll_n1 = (long)tv_get_सङ्ख्या(var1);

    lp->ll_dict = रिक्त;
    lp->ll_object = रिक्त;
    lp->ll_class = रिक्त;
    lp->ll_tuple = रिक्त;
    lp->ll_list = lp->ll_tv->vval.v_list;
    lp->ll_li = check_range_index_one(lp->ll_list, &lp->ll_n1,
				(flags & GLV_ASSIGN_WITH_OP) == 0, quiet);
    जइ (lp->ll_li == रिक्त)
	प्रत्यागम FAIL;

    जइ (lp->ll_valtype != रिक्त && !lp->ll_range)
    {
	// use the type of the member
	जइ (lp->ll_valtype->tt_member != रिक्त)
	    lp->ll_valtype = lp->ll_valtype->tt_member;
	अण्णहा
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
    जइ (lp->ll_range && !lp->ll_empty2)
    {
	lp->ll_n2 = (long)tv_get_सङ्ख्या(var2);
	// is number or string
	जइ (check_range_index_two(lp->ll_list,
			&lp->ll_n1, lp->ll_li, &lp->ll_n2, quiet) == FAIL)
	    प्रत्यागम FAIL;
    }

    lp->ll_tv = &lp->ll_li->li_tv;

    प्रत्यागम OK;
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
    स्थिर पूर्णाङ्क
get_lval_tuple(
    lval_T	*lp,
    typval_T	*idx,
    पूर्णाङ्क		quiet)
{
    // is number or string
    lp->ll_n1 = (long)tv_get_सङ्ख्या(idx);

    lp->ll_list = रिक्त;
    lp->ll_dict = रिक्त;
    lp->ll_blob = रिक्त;
    lp->ll_object = रिक्त;
    lp->ll_class = रिक्त;

    lp->ll_tuple = lp->ll_tv->vval.v_tuple;
    lp->ll_tv = tuple_find(lp->ll_tuple, lp->ll_n1);
    जइ (lp->ll_tv == रिक्त)
    {
	जइ (!quiet)
	    semsg(_(e_tuple_index_out_of_range_nr), lp->ll_n1);
	प्रत्यागम GLV_FAIL;
    }

    // use the type of the member
    जइ (lp->ll_valtype != रिक्त)
    {
	जइ (lp->ll_valtype != रिक्त
		&& lp->ll_valtype->tt_type == VAR_TUPLE
		&& lp->ll_valtype->tt_argcount == 1)
	{
	    // a variadic tuple or a single item tuple
	    जइ (lp->ll_valtype->tt_flags & TTFLAG_VARARGS)
		lp->ll_valtype = lp->ll_valtype->tt_args[0]->tt_member;
	    अण्णहा
		lp->ll_valtype = lp->ll_valtype->tt_args[0];
	}
	अण्णहा
	    // If the LHS member type is not known (VAR_ANY), then get it from
	    // the tuple item (after indexing)
	    lp->ll_valtype = typval2type(lp->ll_tv, get_copyID(),
					&lp->ll_type_list, TVTT_DO_MEMBER);
    }

    प्रत्यागम GLV_OK;
}

/*
 * Get a class or object lval method in class "cl".  The 'key' argument points
 * to the method name and 'key_end' points to the character after 'key'.
 * 'v_type' is VAR_CLASS or VAR_OBJECT.
 *
 * The method index, method function pointer and method type are returned in
 * "lp".
 */
    स्थिर पूर्णाङ्क
get_lval_oc_method(
    lval_T	*lp,
    class_T	*cl,
    char_u	*कुञ्चिका,
    char_u	**कुञ्चिका_end,
    vartype_T	v_type)
{
    // Look for a method with this name.
    // round 1: class functions (skipped for an object)
    // round 2: object methods
    प्रति (पूर्णाङ्क round = v_type == VAR_OBJECT ? 2 : 1; round <= 2; ++round)
    {
	पूर्णाङ्क	m_idx;
	ufunc_T	*fp;

	fp = method_lookup(cl, round == 1 ? VAR_CLASS : VAR_OBJECT,
						कुञ्चिका, *कुञ्चिका_end - कुञ्चिका, &m_idx);
	lp->ll_oi = m_idx;

	// process generic method (if present)
	जइ (fp && (fp = eval_generic_func(fp, कुञ्चिका, कुञ्चिका_end)) == रिक्त)
	    प्रत्यागम FAIL;

	जइ (fp != रिक्त)
	{
	    lp->ll_ufunc = fp;
	    lp->ll_valtype = fp->uf_func_type;
	    भङ्ग;
	}
    }

    प्रत्यागम OK;
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
    स्थिर पूर्णाङ्क
get_lval_oc_variable(
    lval_T	*lp,
    class_T	*cl,
    char_u	*कुञ्चिका,
    char_u	*कुञ्चिका_end,
    vartype_T	v_type,
    class_T	*cl_exec,
    पूर्णाङ्क		flags)
{
    पूर्णाङ्क		m_idx;
    ocmember_T	*om;

    om = member_lookup(cl, v_type, कुञ्चिका, कुञ्चिका_end - कुञ्चिका, &m_idx);
    lp->ll_oi = m_idx;
    जइ (om == रिक्त)
	प्रत्यागम OK;

    // Check variable is accessible
    जइ (get_lval_check_access(cl_exec, cl, om, कुञ्चिका_end, flags) == FAIL)
	प्रत्यागम FAIL;

    // When lhs is used to modify the variable, check it is not a read-only
    // variable.
    जइ ((flags & GLV_READ_ONLY) == 0 && (*कुञ्चिका_end != '.' && *कुञ्चिका_end != '[')
	    && oc_var_check_ro(cl, om))
	प्रत्यागम FAIL;

    lp->ll_valtype = om->ocm_type;

    जइ (v_type == VAR_OBJECT)
	lp->ll_tv = ((typval_T *)(lp->ll_tv->vval.v_object + 1)) + m_idx;
    अण्णहा
	lp->ll_tv = &cl->class_members_tv[m_idx];

    प्रत्यागम OK;
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
    स्थिर पूर्णाङ्क
get_lval_class_or_obj(
    lval_T	*lp,
    char_u	*कुञ्चिका,
    char_u	**कुञ्चिका_end,
    vartype_T	v_type,
    class_T	*cl_exec,
    पूर्णाङ्क		flags,
    पूर्णाङ्क		quiet)
{
    lp->ll_dict = रिक्त;
    lp->ll_list = रिक्त;
    lp->ll_tuple = रिक्त;

    class_T *cl;
    जइ (v_type == VAR_OBJECT)
    {
	जइ (lp->ll_tv->vval.v_object == रिक्त)
	{
	    जइ (!quiet)
		emsg(_(e_using_null_object));
	    प्रत्यागम FAIL;
	}
	cl = lp->ll_tv->vval.v_object->obj_class;
	lp->ll_object = lp->ll_tv->vval.v_object;
    }
    अण्णहा
    {
	cl = lp->ll_tv->vval.v_class;
	lp->ll_object = रिक्त;
    }
    lp->ll_class = cl;

    जइ (cl == रिक्त)
	// TODO: what if class is NULL?
	प्रत्यागम OK;

    lp->ll_valtype = रिक्त;

    जइ (flags & GLV_PREFER_FUNC)
	जइ (get_lval_oc_method(lp, cl, कुञ्चिका, कुञ्चिका_end, v_type) == FAIL)
	    प्रत्यागम FAIL;

    // Look for object/class member variable
    जइ (lp->ll_valtype == रिक्त)
    {
	जइ (get_lval_oc_variable(lp, cl, कुञ्चिका, *कुञ्चिका_end, v_type, cl_exec, flags)
								== FAIL)
	    प्रत्यागम FAIL;
    }

    जइ (lp->ll_valtype == रिक्त)
    {
	member_not_found_सन्देशः(cl, v_type, कुञ्चिका, *कुञ्चिका_end - कुञ्चिका);
	प्रत्यागम FAIL;
    }

    प्रत्यागम OK;
}

/*
 * Check whether dot (".") is allowed after the variable "name" with type
 * "v_type".  Only Dict, Class and Object types support a dot after the name.
 * Returns TRUE if dot is allowed after the name.
 */
    स्थिर पूर्णाङ्क
dot_allowed_after_type(char_u *name, vartype_T v_type, पूर्णाङ्क quiet)
{
    जइ (v_type != VAR_DICT && v_type != VAR_OBJECT && v_type != VAR_CLASS)
    {
	जइ (!quiet)
	    semsg(_(e_dot_not_allowed_after_str_str),
		    vartype_name(v_type), name);
	प्रत्यागम FALSE;
    }

    प्रत्यागम TRUE;
}

/*
 * Check whether left bracket ("[") is allowed after the variable "name" with
 * type "v_type".  Only Dict, List, Tuple and Blob types support a bracket
 * after the variable name.  Returns TRUE if bracket is allowed after the name.
 */
    स्थिर पूर्णाङ्क
bracket_allowed_after_type(char_u *name, vartype_T v_type, पूर्णाङ्क quiet)
{
    जइ (v_type == VAR_CLASS || v_type == VAR_OBJECT)
    {
	जइ (!quiet)
	    semsg(_(e_index_not_allowed_after_str_str),
		    vartype_name(v_type), name);
	प्रत्यागम FALSE;
    }

    प्रत्यागम TRUE;
}

/*
 * Check whether the variable "name" with type "v_type" can be followed by an
 * index.  Only Dict, List, Tuple, Blob, Object and Class types support
 * indexing.  Returns TRUE if indexing is allowed after the name.
 */
    स्थिर पूर्णाङ्क
index_allowed_after_type(char_u *name, vartype_T v_type, पूर्णाङ्क quiet)
{
    जइ (v_type != VAR_LIST
	    && v_type != VAR_TUPLE
	    && v_type != VAR_DICT
	    && v_type != VAR_BLOB
	    && v_type != VAR_OBJECT
	    && v_type != VAR_CLASS)
    {
	जइ (!quiet)
	    semsg(_(e_index_not_allowed_after_str_str),
		    vartype_name(v_type), name);
	प्रत्यागम FALSE;
    }

    प्रत्यागम TRUE;
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
    स्थिर char_u *
get_lval_subscript(
    lval_T	*lp,
    char_u	*p,
    char_u	*name,
    typval_T	*rettv,
    hashtab_T	*ht,
    dictitem_T	*v,
    पूर्णाङ्क		unlet,
    पूर्णाङ्क		flags,	    // GLV_ values
    class_T	*cl_exec)
{
    पूर्णाङ्क		quiet = flags & GLV_QUIET;
    char_u	*कुञ्चिका = रिक्त;
    पूर्णाङ्क		len;
    typval_T	var1;
    typval_T	var2;
    पूर्णाङ्क		empty1 = FALSE;
    पूर्णाङ्क		rc = FAIL;

    /*
     * Loop until no more [idx] or .key is following.
     */
    var1.v_type = VAR_UNKNOWN;
    var2.v_type = VAR_UNKNOWN;

    जाव (*p == '[' || (*p == '.' && p[1] != '=' && p[1] != '.'))
    {
	vartype_T v_type = lp->ll_tv->v_type;

	जइ (*p == '.' && !dot_allowed_after_type(name, v_type, quiet))
	    गच्छ done;

	जइ (*p == '[' && !bracket_allowed_after_type(name, v_type, quiet))
	    गच्छ done;

	जइ (!index_allowed_after_type(name, v_type, quiet))
	    गच्छ done;

	// A NULL list/blob works like an empty list/blob, allocate one now.
	पूर्णाङ्क r = OK;
	जइ (v_type == VAR_LIST && lp->ll_tv->vval.v_list == रिक्त)
	    r = rettv_list_alloc(lp->ll_tv);
	अण्णहा जइ (v_type == VAR_BLOB && lp->ll_tv->vval.v_blob == रिक्त)
	    r = rettv_blob_alloc(lp->ll_tv);
	जइ (r == FAIL)
	    गच्छ done;

	जइ (lp->ll_range)
	{
	    जइ (!quiet)
		emsg(_(e_slice_must_come_last));
	    गच्छ done;
	}
#ifdef LOG_LOCKVAR
	ch_log(रिक्त, "LKVAR: get_lval() loop: p: %s, type: %s", p,
		vartype_name(v_type));
#endif

	जइ (current_script_is_vim9() && lp->ll_valtype == रिक्त
		&& v != रिक्त
		&& lp->ll_tv == &v->di_tv
		&& ht != रिक्त && ht == get_script_local_ht())
	{
	    svar_T  *sv = find_typval_in_script(lp->ll_tv, 0, TRUE);

	    // Vim9 script local variable: get the type
	    जइ (sv != रिक्त)
	    {
		lp->ll_valtype = sv->sv_type;
#ifdef LOG_LOCKVAR
		ch_log(रिक्त, "LKVAR:    ... loop: vim9 assign type: %s",
			vartype_name(lp->ll_valtype->tt_type));
#endif
	    }
	}

	len = -1;
	जइ (*p == '.')
	{
	    कुञ्चिका = p + 1;

	    प्रति (len = 0; ASCII_ISALNUM(कुञ्चिका[len]) || कुञ्चिका[len] == '_'; ++len)
		;
	    जइ (len == 0)
	    {
		जइ (!quiet)
		    emsg(_(e_cannot_use_empty_कुञ्चिका_for_dictionary));
		गच्छ done;
	    }
	    p = कुञ्चिका + len;
	}
	अण्णहा
	{
	    // Get the index [expr] or the first index [expr: ].
	    p = skipwhite(p + 1);
	    जइ (*p == ':')
		empty1 = TRUE;
	    अण्णहा
	    {
		empty1 = FALSE;
		जइ (eval1(&p, &var1, &EVALARG_EVALUATE) == FAIL)  // recursive!
		    गच्छ done;
		जइ (tv_get_string_chk(&var1) == रिक्त)
		    // not a number or string
		    गच्छ done;
		p = skipwhite(p);
	    }

	    // Optionally get the second index [ :expr].
	    जइ (*p == ':')
	    {
		जइ (v_type == VAR_DICT)
		{
		    जइ (!quiet)
			emsg(_(e_cannot_slice_dictionary));
		    गच्छ done;
		}
		जइ (v_type == VAR_TUPLE)
		{
		    जइ (!quiet)
			emsg(_(e_cannot_slice_tuple));
		    गच्छ done;
		}
		जइ (rettv != रिक्त
			&& !(rettv->v_type == VAR_LIST
			    && rettv->vval.v_list != रिक्त)
			&& !(rettv->v_type == VAR_BLOB
			    && rettv->vval.v_blob != रिक्त))
		{
		    जइ (!quiet)
			emsg(_(e_slice_requires_list_or_blob_मानम्));
		    गच्छ done;
		}
		p = skipwhite(p + 1);
		जइ (*p == ']')
		    lp->ll_empty2 = TRUE;
		अण्णहा
		{
		    lp->ll_empty2 = FALSE;
		    // recursive!
		    जइ (eval1(&p, &var2, &EVALARG_EVALUATE) == FAIL)
			गच्छ done;
		    जइ (tv_get_string_chk(&var2) == रिक्त)
			// not a number or string
			गच्छ done;
		}
		lp->ll_range = TRUE;
	    }
	    अण्णहा
		lp->ll_range = FALSE;

	    जइ (*p != ']')
	    {
		जइ (!quiet)
		    emsg(_(e_missing_closing_square_brace));
		गच्छ done;
	    }

	    // Skip to past ']'.
	    ++p;
	}
#ifdef LOG_LOCKVAR
	जइ (len == -1)
	    ch_log(रिक्त, "LKVAR:    ... loop: p: %s, '[' key: %s", p,
		    empty1 ? ":" : (वर्ण*)tv_get_string(&var1));
	अण्णहा
	    ch_log(रिक्त, "LKVAR:    ... loop: p: %s, '.' key: %s", p, कुञ्चिका);
#endif

	जइ (v_type == VAR_DICT)
	{
	    glv_status_T glv_status;

	    glv_status = get_lval_dict_item(lp, name, कुञ्चिका, len, &p, &var1,
							flags, unlet, rettv);
	    जइ (glv_status == GLV_FAIL)
		गच्छ done;
	    जइ (glv_status == GLV_STOP)
		भङ्ग;
	}
	अण्णहा जइ (v_type == VAR_BLOB)
	{
	    जइ (get_lval_blob(lp, &var1, &var2, empty1, quiet) == FAIL)
		गच्छ done;

	    भङ्ग;
	}
	अण्णहा जइ (v_type == VAR_LIST)
	{
	    जइ (get_lval_list(lp, &var1, &var2, empty1, flags, quiet) == FAIL)
		गच्छ done;
	}
	अण्णहा जइ (v_type == VAR_TUPLE)
	{
	    जइ (get_lval_tuple(lp, &var1, quiet) == FAIL)
		गच्छ done;
	}
	अण्णहा  // v_type == VAR_CLASS || v_type == VAR_OBJECT
	{
	    जइ (get_lval_class_or_obj(lp, कुञ्चिका, &p, v_type, cl_exec, flags,
							quiet) == FAIL)
		गच्छ done;
	}

	clear_tv(&var1);
	clear_tv(&var2);
	var1.v_type = VAR_UNKNOWN;
	var2.v_type = VAR_UNKNOWN;
    }

    जइ (lp->ll_tuple != रिक्त && (flags & GLV_READ_ONLY) == 0)
    {
	जइ (!quiet)
	    emsg(_(e_tuple_is_immutable));
	गच्छ done;
    }

    rc = OK;

done:
    clear_tv(&var1);
    clear_tv(&var2);
    प्रत्यागम rc == OK ? p : रिक्त;
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
    पूर्णाङ्क		unlet,
    पूर्णाङ्क		skip,
    पूर्णाङ्क		flags,	    // GLV_ values
    पूर्णाङ्क		fne_flags)  // flags for find_name_end()
{
    char_u	*p;
    char_u	*व्यञ्जकम्_start, *व्यञ्जकम्_end;
    पूर्णाङ्क		cc;
    dictitem_T	*v = रिक्त;
    hashtab_T	*ht = रिक्त;
    पूर्णाङ्क		quiet = flags & GLV_QUIET;
    पूर्णाङ्क		writing = 0;
    पूर्णाङ्क		vim9script = in_vim9script();
    class_T	*cl_exec = रिक्त;    // class that is executing, or NULL.

#ifdef LOG_LOCKVAR
    जइ (lval_मूलम् == रिक्त)
	ch_log(रिक्त, "LKVAR: get_lval(): name: %s, lval_root (nil)", name);
    अण्णहा
	ch_log(रिक्त, "LKVAR: get_lval(): name: %s, lr_tv %p lr_is_arg %d",
			name, (शून्य*)lval_मूलम्->lr_tv, lval_मूलम्->lr_is_arg);
    वर्ण buf[80];
    ch_log(रिक्त, "LKVAR:    ...: GLV flags: %s",
		    flags_tostring(flags, glv_flag_strings, buf, परिमाण(buf)));
#endif

    // Clear everything in "lp".
    CLEAR_POINTER(lp);
    ga_init2(&lp->ll_type_list, परिमाण(type_T *), 10);

    जइ (skip || (flags & GLV_COMPILING))
    {
	// When skipping or compiling just find the end of the name.
	lp->ll_name = name;
	lp->ll_name_end = find_name_end(name, रिक्त, रिक्त,
						      FNE_INCL_BR | fne_flags);
	प्रत्यागम lp->ll_name_end;
    }

    // Cannot use "s:var" at the Vim9 script level.  "s: type" is OK.
    जइ (vim9script && at_script_level()
		  && name[0] == 's' && name[1] == ':' && !VIM_ISWHITE(name[2]))
    {
	semsg(_(e_cannot_use_s_colon_in_vim9_script_str), name);
	प्रत्यागम रिक्त;
    }

    // Find the end of the name.
    p = find_name_end(name, &व्यञ्जकम्_start, &व्यञ्जकम्_end, fne_flags);
    lp->ll_name_end = p;
    जइ (व्यञ्जकम्_start != रिक्त)
    {
	// Don't expand the name when we already know there is an error.
	जइ (unlet && !VIM_ISWHITE(*p) && !ends_excmd(*p)
						    && *p != '[' && *p != '.')
	{
	    semsg(_(e_trailing_characters_str), p);
	    प्रत्यागम रिक्त;
	}

	lp->ll_exp_name = make_expanded_name(name, व्यञ्जकम्_start, व्यञ्जकम्_end, p);
	जइ (lp->ll_exp_name == रिक्त)
	{
	    // Report an invalid expression in braces, unless the
	    // expression evaluation has been cancelled due to an
	    // aborting error, an interrupt, or an exception.
	    जइ (!aborting() && !quiet)
	    {
		emsg_severe = TRUE;
		semsg(_(e_invalid_argument_str), name);
		प्रत्यागम रिक्त;
	    }
	}
	lp->ll_name = lp->ll_exp_name;
    }
    अण्णहा
    {
	lp->ll_name = name;

	जइ (vim9script)
	{
	    // "a: type" is declaring variable "a" with a type, not "a:".
	    // However, "g:[key]" is indexing a dictionary.
	    जइ (p == name + 2 && p[-1] == ':' && *p != '[')
	    {
		--p;
		lp->ll_name_end = p;
	    }
	    जइ (*skipwhite(p) == ':')
	    {
		char_u	    *tp = skipwhite(p + 1);

		जइ (is_scoped_variable(name))
		{
		    semsg(_(e_cannot_use_type_with_this_variable_str), name);
		    प्रत्यागम रिक्त;
		}
		जइ (VIM_ISWHITE(*p))
		{
		    semsg(_(e_no_white_space_allowed_before_colon_str), p);
		    प्रत्यागम रिक्त;
		}
		जइ (tp == p + 1 && !quiet)
		{
		    semsg(_(e_white_space_required_after_str_str), ":", p);
		    प्रत्यागम रिक्त;
		}
		जइ (!SCRIPT_ID_VALID(current_sctx.sc_sid))
		{
		    semsg(_(e_using_type_not_in_script_context_str), p);
		    प्रत्यागम रिक्त;
		}
		जइ (vim9script && (flags & GLV_NO_DECL) &&
			!(flags & GLV_FOR_LOOP))
		{
		    // Using a type and not in a "var" declaration.
		    semsg(_(e_trailing_characters_str), p);
		    प्रत्यागम रिक्त;
		}


		// parse the type after the name
		lp->ll_type = parse_type(&tp,
			       &SCRIPT_ITEM(current_sctx.sc_sid)->sn_type_list,
			       रिक्त, रिक्त, !quiet);
		जइ (!quiet && (lp->ll_type == रिक्त
			    || !valid_declaration_type(lp->ll_type)))
		    प्रत्यागम रिक्त;
		lp->ll_name_end = tp;
	    }
	    // TODO: check inside class?
	}
    }
    जइ (lp->ll_name == रिक्त)
	प्रत्यागम p;

    जइ (*p == '.')
    {
	// In legacy script, when a local variable and import exists with this name,
	// prioritize local variable over imports to avoid conflicts.
	पूर्णाङ्क var_exists = FALSE;
	जइ (!vim9script)
	{
	    cc = *p;
	    *p = NUL;
	    hashtab_T *local_ht = get_funccal_local_ht();
	    जइ (local_ht != रिक्त)
	    {
		hashitem_T *hi = hash_find(local_ht, lp->ll_name);
		जइ (!HASHITEM_EMPTY(hi))
		    var_exists = TRUE;
	    }
	    *p = cc;
	}

	जइ (!var_exists)
	{
	    imported_T *import = find_imported(lp->ll_name, p - lp->ll_name, TRUE);
	    जइ (import != रिक्त)
	    {
		p++;	// skip '.'
		p = get_lval_imported(lp, import->imp_sid, p, &v, fne_flags);
		जइ (p == रिक्त)
		    प्रत्यागम रिक्त;
	    }
	}
    }

    // Without [idx] or .key we are done.
    जइ (*p != '[' && *p != '.')
    {
	जइ (lval_मूलम् != रिक्त)
	    fill_lval_from_lval_मूलम्(lp, lval_मूलम्);
	प्रत्यागम p;
    }

    जइ (vim9script && lval_मूलम् != रिक्त)
	cl_exec = lval_मूलम्->lr_cl_exec;
    जइ (vim9script && lval_मूलम् != रिक्त && lval_मूलम्->lr_tv != रिक्त)
    {
	// using local variable
	lp->ll_tv = lval_मूलम्->lr_tv;
	v = रिक्त;
    }
    अण्णहा जइ (lp->ll_tv == रिक्त)
    {
	cc = *p;
	*p = NUL;
	// When we would write to the variable pass &ht and prevent autoload.
	writing = !(flags & GLV_READ_ONLY);
	v = find_var(lp->ll_name, writing ? &ht : रिक्त,
					 (flags & GLV_NO_AUTOLOAD) || writing);
	जइ (v == रिक्त && !quiet)
	    semsg(_(e_undefined_variable_str), lp->ll_name);
	*p = cc;
	जइ (v == रिक्त)
	    प्रत्यागम रिक्त;
	lp->ll_tv = &v->di_tv;
    }

    जइ (vim9script && (flags & GLV_NO_DECL) == 0)
    {
	जइ (!quiet)
	    semsg(_(e_variable_already_declared_str), lp->ll_name);
	प्रत्यागम रिक्त;
    }

    // If the next character is a "." or a "[", then process the subitem.
    p = get_lval_subscript(lp, p, name, rettv, ht, v, unlet, flags, cl_exec);
    जइ (p == रिक्त)
	प्रत्यागम रिक्त;

    जइ (vim9script && lp->ll_valtype != रिक्त && rettv != रिक्त)
    {
	where_T	    where = WHERE_INIT;

	// In a Vim9 script, do type check and make sure the variable is
	// writable.
	जइ (check_typval_type(lp->ll_valtype, rettv, where) == FAIL)
	    प्रत्यागम रिक्त;
    }

    lp->ll_name_end = p;
    प्रत्यागम p;
}

/*
 * Clear lval "lp" that was filled by get_lval().
 */
    शून्य
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
    शून्य
set_var_lval(
    lval_T	*lp,
    char_u	*endp,
    typval_T	*rettv,
    पूर्णाङ्क		copy,
    पूर्णाङ्क		flags,	    // ASSIGN_CONST, ASSIGN_NO_DECL
    char_u	*op,
    पूर्णाङ्क		var_idx)    // index for "let [a, b] = list"
{
    पूर्णाङ्क		cc;
    dictitem_T	*di;

    जइ (lp->ll_tv == रिक्त)
    {
	cc = *endp;
	*endp = NUL;
	जइ (in_vim9script() && check_reserved_name(lp->ll_name, FALSE) == FAIL)
	    प्रत्यागम;

	जइ (lp->ll_blob != रिक्त)
	{
	    पूर्णाङ्क	    error = FALSE;

	    जइ (op != रिक्त && *op != '=')
	    {
		semsg(_(e_wrong_variable_type_for_str_equal), op);
		प्रत्यागम;
	    }
	    जइ (मानम्_check_lock(lp->ll_blob->bv_lock, lp->ll_name, FALSE))
		प्रत्यागम;

	    जइ (lp->ll_range && rettv->v_type == VAR_BLOB)
	    {
		जइ (lp->ll_empty2)
		    lp->ll_n2 = blob_len(lp->ll_blob) - 1;

		जइ (blob_set_range(lp->ll_blob, lp->ll_n1, lp->ll_n2,
								rettv) == FAIL)
		    प्रत्यागम;
	    }
	    अण्णहा
	    {
		varnumber_T	val = tv_get_सङ्ख्या_chk(rettv, &error);
		जइ (!error)
		{
		    जइ (val < 0 || val > 255)
			semsg(_(e_invalid_मानम्_for_blob_nr), val);
		    अण्णहा
			blob_set_append(lp->ll_blob, lp->ll_n1, val);
		}
	    }
	}
	अण्णहा जइ (op != रिक्त && *op != '=')
	{
	    typval_T tv;

	    जइ ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					     && (flags & ASSIGN_FOR_LOOP) == 0)
	    {
		emsg(_(e_cannot_modify_existing_variable));
		*endp = cc;
		प्रत्यागम;
	    }

	    // handle +=, -=, *=, /=, %= and .=
	    di = रिक्त;
	    जइ (eval_variable(lp->ll_name, (पूर्णाङ्क)STRLEN(lp->ll_name),
				 lp->ll_sid, &tv, &di, EVAL_VAR_VERBOSE) == OK)
	    {
		जइ (di != रिक्त && check_typval_is_मानम्(&di->di_tv) == FAIL)
		{
		    clear_tv(&tv);
		    प्रत्यागम;
		}

		// In Vim9 script a String v: variable only takes a String.
		जइ (in_vim9script() && STRNCMP(lp->ll_name, "v:", 2) == 0
			&& tv.v_type == VAR_STRING)
		{
		    where_T	where = WHERE_INIT;

		    जइ (check_typval_type(&t_string, rettv, where) == FAIL)
		    {
			clear_tv(&tv);
			*endp = cc;
			प्रत्यागम;
		    }
		}

		जइ ((di == रिक्त
			 || (!var_check_ro(di->di_flags, lp->ll_name, FALSE)
			   && !tv_check_lock(&di->di_tv, lp->ll_name, FALSE)))
			&& tv_op(&tv, rettv, op) == OK)
		    set_var_const(lp->ll_name, lp->ll_sid, रिक्त, &tv, FALSE,
				ASSIGN_NO_DECL | ASSIGN_COMPOUND_OP, 0);
		clear_tv(&tv);
	    }
	}
	अण्णहा
	{
	    जइ (lp->ll_type != रिक्त && check_typval_arg_type(lp->ll_type, rettv,
							      रिक्त, 0) == FAIL)
		प्रत्यागम;
	    set_var_const(lp->ll_name, lp->ll_sid, lp->ll_type, rettv, copy,
							       flags, var_idx);
	}
	*endp = cc;
    }
    अण्णहा जइ (मानम्_check_lock(lp->ll_newkey == रिक्त
		? lp->ll_tv->v_lock
		: lp->ll_tv->vval.v_dict->dv_lock, lp->ll_name, FALSE))
	;
    अण्णहा जइ (lp->ll_range)
    {
	जइ ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					     && (flags & ASSIGN_FOR_LOOP) == 0)
	{
	    emsg(_(e_cannot_lock_range));
	    प्रत्यागम;
	}

	(शून्य)list_assign_range(lp->ll_list, rettv->vval.v_list,
			 lp->ll_n1, lp->ll_n2, lp->ll_empty2, op, lp->ll_name);
    }
    अण्णहा
    {
	/*
	 * Assign to a List, Dictionary or Object item.
	 */
	जइ ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					     && (flags & ASSIGN_FOR_LOOP) == 0)
	{
	    emsg(_(e_cannot_lock_list_or_dict));
	    प्रत्यागम;
	}

	जइ (lp->ll_valtype != रिक्त
		    && check_typval_arg_type(lp->ll_valtype, rettv,
							      रिक्त, 0) == FAIL)
	{
	    lp->ll_name_end = रिक्त;
	    प्रत्यागम;
	}

	// If the lval is a List and the type of the list is not yet set,
	// then set the item type from the declared type of the variable.
	जइ (in_vim9script() && rettv->v_type == VAR_LIST
				&& rettv->vval.v_list != रिक्त
				&& rettv->vval.v_list->lv_type == रिक्त)
	{
	    जइ (lp->ll_tv->v_type == VAR_LIST
		    && lp->ll_tv->vval.v_list != रिक्त
		    && lp->ll_tv->vval.v_list->lv_type != रिक्त)
		set_tv_type(rettv, lp->ll_tv->vval.v_list->lv_type);
	}

	जइ (lp->ll_newkey != रिक्त)
	{
	    जइ (op != रिक्त && *op != '=')
	    {
		semsg(_(e_कुञ्चिका_not_present_in_dictionary_str), lp->ll_newkey);
		प्रत्यागम;
	    }
	    जइ (dict_wrong_func_name(lp->ll_tv->vval.v_dict, rettv,
								lp->ll_newkey))
		प्रत्यागम;

	    // Need to add an item to the Dictionary.
	    di = dictitem_alloc(lp->ll_newkey);
	    जइ (di == रिक्त)
		प्रत्यागम;
	    जइ (dict_add(lp->ll_tv->vval.v_dict, di) == FAIL)
	    {
		vim_free(di);
		प्रत्यागम;
	    }
	    lp->ll_tv = &di->di_tv;
	}
	अण्णहा जइ (op != रिक्त && *op != '=')
	{
	    tv_op(lp->ll_tv, rettv, op);
	    प्रत्यागम;
	}
	अण्णहा
	    clear_tv(lp->ll_tv);

	/*
	 * Assign the value to the variable or list item.
	 */
	जइ (copy)
	    copy_tv(rettv, lp->ll_tv);
	अण्णहा
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
    स्थिर पूर्णाङ्क
tv_op_blob(typval_T *tv1, typval_T *tv2, char_u *op)
{
    जइ (*op != '+' || tv2->v_type != VAR_BLOB)
	प्रत्यागम FAIL;

    // Blob += Blob
    जइ (tv2->vval.v_blob == रिक्त)
	प्रत्यागम OK;

    जइ (tv1->vval.v_blob == रिक्त)
    {
	tv1->vval.v_blob = tv2->vval.v_blob;
	++tv1->vval.v_blob->bv_refcount;
	प्रत्यागम OK;
    }

    blob_T	*b1 = tv1->vval.v_blob;
    blob_T	*b2 = tv2->vval.v_blob;
    पूर्णाङ्क		len = blob_len(b2);

    जइ (len > 0 && ga_grow(&b1->bv_ga, len) == OK)
    {
	mch_memmove((char_u *)b1->bv_ga.ga_data + b1->bv_ga.ga_len,
		    (char_u *)b2->bv_ga.ga_data, (size_t)len);
	b1->bv_ga.ga_len += len;
    }
    अण्णहा
	प्रति (पूर्णाङ्क i = 0; i < len; i++)
	    (शून्य)ga_append(&b1->bv_ga, blob_get(b2, i));

    प्रत्यागम OK;
}

/*
 * Handle "list1 += list2".
 * Returns OK or FAIL.
 */
    स्थिर पूर्णाङ्क
tv_op_list(typval_T *tv1, typval_T *tv2, char_u *op)
{
    जइ (*op != '+' || tv2->v_type != VAR_LIST)
	प्रत्यागम FAIL;

    // List += List
    जइ (tv2->vval.v_list == रिक्त)
	प्रत्यागम OK;

    जइ (tv1->vval.v_list == रिक्त)
    {
	tv1->vval.v_list = tv2->vval.v_list;
	++tv1->vval.v_list->lv_refcount;
    }
    अण्णहा
	list_extend(tv1->vval.v_list, tv2->vval.v_list, रिक्त);

    प्रत्यागम OK;
}

/*
 * Handle number operations:
 *	nr += nr , nr -= nr , nr *=nr , nr /= nr , nr %= nr
 *
 * Returns OK or FAIL.
 */
    स्थिर पूर्णाङ्क
tv_op_सङ्ख्या(typval_T *tv1, typval_T *tv2, char_u *op)
{
    varnumber_T	n;
    पूर्णाङ्क		failed = FALSE;

    n = tv_get_सङ्ख्या(tv1);
    जइ (tv2->v_type == VAR_FLOAT)
    {
	float_T f = n;

	जइ (*op == '%')
	    प्रत्यागम FAIL;
	विकल्प (*op)
	{
	    पक्ष '+': f += tv2->vval.v_float; भङ्ग;
	    पक्ष '-': f -= tv2->vval.v_float; भङ्ग;
	    पक्ष '*': f *= tv2->vval.v_float; भङ्ग;
	    पक्ष '/': f /= tv2->vval.v_float; भङ्ग;
	}
	clear_tv(tv1);
	tv1->v_type = VAR_FLOAT;
	tv1->vval.v_float = f;
    }
    अण्णहा
    {
	विकल्प (*op)
	{
	    पक्ष '+': n += tv_get_सङ्ख्या(tv2); भङ्ग;
	    पक्ष '-': n -= tv_get_सङ्ख्या(tv2); भङ्ग;
	    पक्ष '*': n *= tv_get_सङ्ख्या(tv2); भङ्ग;
	    पक्ष '/': n = अङ्कः_divide(n, tv_get_सङ्ख्या(tv2), &failed); भङ्ग;
	    पक्ष '%': n = अङ्कः_modulus(n, tv_get_सङ्ख्या(tv2), &failed); भङ्ग;
	}
	clear_tv(tv1);
	tv1->v_type = VAR_सङ्ख्या;
	tv1->vval.v_सङ्ख्या = n;
    }

    प्रत्यागम failed ? FAIL : OK;
}

/*
 * Append string "s2" to the string in "tv1".
 * Returns OK if "tv1" was grown in place, FAIL otherwise.
 */
    स्थिर पूर्णाङ्क
grow_string_tv(typval_T *tv1, char_u *s2)
{
    जइ (tv1->v_type != VAR_STRING || tv1->vval.v_string == रिक्त)
	प्रत्यागम FAIL;

    size_t len1 = STRLEN(tv1->vval.v_string);
    size_t len2 = STRLEN(s2);
    char_u *p = vim_realloc(tv1->vval.v_string, len1 + len2 + 1);

    जइ (p == रिक्त)
	प्रत्यागम FAIL;

    mch_memmove(p + len1, s2, len2 + 1);
    tv1->vval.v_string = p;
    प्रत्यागम OK;
}

/*
 * Handle "str1 .= str2"
 * Returns OK or FAIL.
 */
    स्थिर पूर्णाङ्क
tv_op_string(typval_T *tv1, typval_T *tv2, char_u *op UNUSED)
{
    char_u	numbuf[NUMBUFLEN];
    char_u	*s;
    char_u	*s2;

    जइ (tv2->v_type == VAR_FLOAT)
	प्रत्यागम FAIL;

    // str .= str
    s2 = tv_get_string_buf(tv2, numbuf);
    जइ (grow_string_tv(tv1, s2) == OK)
	प्रत्यागम OK;

    s = tv_get_string(tv1);
    s = concat_str(s, s2);
    clear_tv(tv1);
    tv1->v_type = VAR_STRING;
    tv1->vval.v_string = s;

    प्रत्यागम OK;
}

/*
 * Handle "tv1 += tv2", "tv1 -= tv2", "tv1 *= tv2", "tv1 /= tv2", "tv1 %= tv2"
 * and "tv1 .= tv2"
 * Returns OK or FAIL.
 */
    स्थिर पूर्णाङ्क
tv_op_nr_or_string(typval_T *tv1, typval_T *tv2, char_u *op)
{
    जइ (tv2->v_type == VAR_LIST)
	प्रत्यागम FAIL;

    जइ (vim_strchr((char_u *)"+-*/%", *op) != रिक्त)
	प्रत्यागम tv_op_सङ्ख्या(tv1, tv2, op);

    प्रत्यागम tv_op_string(tv1, tv2, op);
}

/*
 * Handle "f1 += f2", "f1 -= f2", "f1 *= f2", "f1 /= f2".
 * Returns OK or FAIL.
 */
    स्थिर पूर्णाङ्क
tv_op_float(typval_T *tv1, typval_T *tv2, char_u *op)
{
    float_T f;

    जइ (*op == '%' || *op == '.'
	    || (tv2->v_type != VAR_FLOAT
		&& tv2->v_type != VAR_सङ्ख्या
		&& tv2->v_type != VAR_STRING))
	प्रत्यागम FAIL;

    जइ (tv2->v_type == VAR_FLOAT)
	f = tv2->vval.v_float;
    अण्णहा
	f = tv_get_सङ्ख्या(tv2);
    विकल्प (*op)
    {
	पक्ष '+': tv1->vval.v_float += f; भङ्ग;
	पक्ष '-': tv1->vval.v_float -= f; भङ्ग;
	पक्ष '*': tv1->vval.v_float *= f; भङ्ग;
	पक्ष '/': tv1->vval.v_float /= f; भङ्ग;
    }

    प्रत्यागम OK;
}

/*
 * Handle "tv1 += tv2", "tv1 -= tv2", "tv1 *= tv2", "tv1 /= tv2", "tv1 %= tv2"
 * and "tv1 .= tv2"
 * Returns OK or FAIL.
 */
    पूर्णाङ्क
tv_op(typval_T *tv1, typval_T *tv2, char_u *op)
{
    // Can't do anything with a Funcref or Dict on the right.
    // v:true and friends only work with "..=".
    जइ (tv2->v_type == VAR_FUNC || tv2->v_type == VAR_DICT
		|| ((tv2->v_type == VAR_BOOL || tv2->v_type == VAR_SPECIAL)
							&& *op != '.'))
    {
	semsg(_(e_wrong_variable_type_for_str_equal), op);
	प्रत्यागम FAIL;
    }

    पूर्णाङ्क retval = FAIL;
    विकल्प (tv1->v_type)
    {
	पक्ष VAR_UNKNOWN:
	पक्ष VAR_ANY:
	पक्ष VAR_VOID:
	पक्ष VAR_DICT:
	पक्ष VAR_FUNC:
	पक्ष VAR_PARTIAL:
	पक्ष VAR_BOOL:
	पक्ष VAR_SPECIAL:
	पक्ष VAR_JOB:
	पक्ष VAR_CHANNEL:
	पक्ष VAR_INSTR:
	पक्ष VAR_OBJECT:
	पक्ष VAR_CLASS:
	पक्ष VAR_TYPEALIAS:
	पक्ष VAR_TUPLE:
	    भङ्ग;

	पक्ष VAR_BLOB:
	    retval = tv_op_blob(tv1, tv2, op);
	    भङ्ग;

	पक्ष VAR_LIST:
	    retval = tv_op_list(tv1, tv2, op);
	    भङ्ग;

	पक्ष VAR_सङ्ख्या:
	पक्ष VAR_STRING:
	    retval = tv_op_nr_or_string(tv1, tv2, op);
	    भङ्ग;

	पक्ष VAR_FLOAT:
	    retval = tv_op_float(tv1, tv2, op);
	    भङ्ग;
    }

    जइ (retval != OK)
	semsg(_(e_wrong_variable_type_for_str_equal), op);

    प्रत्यागम retval;
}

/*
 * Evaluate the expression used in a ":for var in expr" command.
 * "arg" points to "var".
 * Set "*errp" to TRUE for an error, FALSE otherwise;
 * Return a pointer that holds the info.  Null when there is an error.
 */
    शून्य *
eval_for_पङ्क्तिः(
    char_u	*arg,
    पूर्णाङ्क		*errp,
    exarg_T	*eap,
    evalarg_T	*evalarg)
{
    forinfo_T	*fi;
    char_u	*var_list_end;
    char_u	*व्यञ्जकम्;
    typval_T	tv;
    list_T	*l;
    tuple_T	*tuple;
    पूर्णाङ्क		skip = !(evalarg->eval_flags & EVAL_EVALUATE);

    *errp = TRUE;	// default: there is an error

    fi = ALLOC_CLEAR_ONE(forinfo_T);
    जइ (fi == रिक्त)
	प्रत्यागम रिक्त;

    var_list_end = skip_var_list(arg, TRUE, &fi->fi_varcount,
						     &fi->fi_semicolon, FALSE);
    जइ (var_list_end == रिक्त)
	प्रत्यागम fi;

    व्यञ्जकम् = skipwhite_and_linebreak(var_list_end, evalarg);
    जइ (व्यञ्जकम्[0] != 'i' || व्यञ्जकम्[1] != 'n'
				  || !(व्यञ्जकम्[2] == NUL || VIM_ISWHITE(व्यञ्जकम्[2])))
    {
	जइ (in_vim9script() && *व्यञ्जकम् == ':' && व्यञ्जकम् != var_list_end)
	    semsg(_(e_no_white_space_allowed_before_colon_str), व्यञ्जकम्);
	अण्णहा
	    emsg(_(e_missing_in_after_for));
	प्रत्यागम fi;
    }

    जइ (skip)
	++emsg_skip;
    व्यञ्जकम् = skipwhite_and_linebreak(व्यञ्जकम् + 2, evalarg);
    जइ (eval0(व्यञ्जकम्, &tv, eap, evalarg) == OK)
    {
	*errp = FALSE;
	जइ (!skip)
	{
	    जइ (tv.v_type == VAR_LIST)
	    {
		l = tv.vval.v_list;
		जइ (l == रिक्त)
		{
		    // a null list is like an empty list: do nothing
		    clear_tv(&tv);
		}
		अण्णहा
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
	    अण्णहा जइ (tv.v_type == VAR_TUPLE)
	    {
		tuple = tv.vval.v_tuple;
		जइ (tuple == रिक्त)
		{
		    // a null tuple is like an empty tuple: do nothing
		    clear_tv(&tv);
		}
		अण्णहा
		{
		    // No need to increment the refcount, it's already set for
		    // the tuple being used in "tv".
		    fi->fi_tuple = tuple;
		    fi->fi_tuple_idx = 0;
		}
	    }
	    अण्णहा जइ (tv.v_type == VAR_BLOB)
	    {
		fi->fi_bi = 0;
		जइ (tv.vval.v_blob != रिक्त)
		{
		    // Make a copy, so that the iteration still works when the
		    // blob is changed.
		    fi->fi_blob = blob_copy(tv.vval.v_blob);
		}
		clear_tv(&tv);
	    }
	    अण्णहा जइ (tv.v_type == VAR_STRING)
	    {
		fi->fi_byte_idx = 0;
		fi->fi_string = tv.vval.v_string;
		tv.vval.v_string = रिक्त;
		जइ (fi->fi_string == रिक्त)
		    fi->fi_string = vim_strsave((char_u *)"");
	    }
	    अण्णहा
	    {
		emsg(_(e_string_list_tuple_or_blob_required));
		clear_tv(&tv);
	    }
	}
    }
    जइ (skip)
	--emsg_skip;
    fi->fi_break_गणनम् = evalarg->eval_break_गणनम्;

    प्रत्यागम fi;
}

/*
 * Used when looping over a :for line, skip the "in expr" part.
 */
    शून्य
skip_for_पङ्क्तयः(शून्य *fi_void, evalarg_T *evalarg)
{
    forinfo_T	*fi = (forinfo_T *)fi_void;
    पूर्णाङ्क		i;

    प्रति (i = 0; i < fi->fi_break_गणनम्; ++i)
	eval_next_पङ्क्तिः(रिक्त, evalarg);
}

/*
 * Use the first item in a ":for" list.  Advance to the next.
 * Assign the values to the variable (list).  "arg" points to the first one.
 * Return TRUE when a valid item was found, FALSE when at end of list or
 * something wrong.
 */
    पूर्णाङ्क
next_for_item(शून्य *fi_void, char_u *arg)
{
    forinfo_T	*fi = (forinfo_T *)fi_void;
    पूर्णाङ्क		result;
    पूर्णाङ्क		flag = ASSIGN_FOR_LOOP | (in_vim9script()
			 ? (ASSIGN_FINAL
			     // first round: error if variable exists
			     | (fi->fi_bi == 0 ? 0 : ASSIGN_DECL)
			     | ASSIGN_NO_MEMBER_TYPE
			     | ASSIGN_UPDATE_BLOCK_ID)
			 : 0);
    listitem_T	*item;
    पूर्णाङ्क		skip_assign = in_vim9script() && arg[0] == '_'
						      && !eval_isnamec(arg[1]);

    जइ (fi->fi_blob != रिक्त)
    {
	typval_T	tv;

	जइ (fi->fi_bi >= blob_len(fi->fi_blob))
	    प्रत्यागम FALSE;
	tv.v_type = VAR_सङ्ख्या;
	tv.v_lock = VAR_FIXED;
	tv.vval.v_सङ्ख्या = blob_get(fi->fi_blob, fi->fi_bi);
	++fi->fi_bi;
	जइ (skip_assign)
	    प्रत्यागम TRUE;
	प्रत्यागम ex_let_vars(arg, &tv, TRUE, fi->fi_semicolon,
					    fi->fi_varcount, flag, रिक्त) == OK;
    }

    जइ (fi->fi_string != रिक्त)
    {
	typval_T	tv;
	पूर्णाङ्क		len;

	len = mb_ptr2len(fi->fi_string + fi->fi_byte_idx);
	जइ (len == 0)
	    प्रत्यागम FALSE;
	tv.v_type = VAR_STRING;
	tv.v_lock = VAR_FIXED;
	tv.vval.v_string = vim_strnsave(fi->fi_string + fi->fi_byte_idx, len);
	fi->fi_byte_idx += len;
	++fi->fi_bi;
	जइ (skip_assign)
	    result = TRUE;
	अण्णहा
	    result = ex_let_vars(arg, &tv, TRUE, fi->fi_semicolon,
					    fi->fi_varcount, flag, रिक्त) == OK;
	vim_free(tv.vval.v_string);
	प्रत्यागम result;
    }

    जइ (fi->fi_tuple != रिक्त)
    {
	typval_T	tv;

	जइ (fi->fi_tuple_idx >= TUPLE_LEN(fi->fi_tuple))
	    प्रत्यागम FALSE;

	copy_tv(TUPLE_ITEM(fi->fi_tuple, fi->fi_tuple_idx), &tv);
	++fi->fi_tuple_idx;
	++fi->fi_bi;
	जइ (skip_assign)
	    result = TRUE;
	अण्णहा
	    result = ex_let_vars(arg, &tv, TRUE, fi->fi_semicolon,
					    fi->fi_varcount, flag, रिक्त) == OK;
	clear_tv(&tv);
	प्रत्यागम result;
    }

    item = fi->fi_lw.lw_item;
    जइ (item == रिक्त)
	result = FALSE;
    अण्णहा
    {
	fi->fi_lw.lw_item = item->li_next;
	++fi->fi_bi;
	जइ (skip_assign)
	    result = TRUE;
	अण्णहा
	    result = (ex_let_vars(arg, &item->li_tv, TRUE, fi->fi_semicolon,
					   fi->fi_varcount, flag, रिक्त) == OK);
    }
    प्रत्यागम result;
}

/*
 * Free the structure used to store info used by ":for".
 */
    शून्य
free_for_info(शून्य *fi_void)
{
    forinfo_T    *fi = (forinfo_T *)fi_void;

    जइ (fi == रिक्त)
	प्रत्यागम;
    जइ (fi->fi_list != रिक्त)
    {
	list_rem_watch(fi->fi_list, &fi->fi_lw);
	list_unref(fi->fi_list);
    }
    अण्णहा जइ (fi->fi_blob != रिक्त)
	blob_unref(fi->fi_blob);
    अण्णहा जइ (fi->fi_tuple != रिक्त)
	tuple_unref(fi->fi_tuple);
    अण्णहा
	vim_free(fi->fi_string);
    vim_free(fi);
}

    शून्य
set_context_for_expression(
    expand_T	*xp,
    char_u	*arg,
    cmdidx_T	cmdidx)
{
    पूर्णाङ्क		has_व्यञ्जकम् = cmdidx != CMD_let && cmdidx != CMD_var;
    पूर्णाङ्क		c;
    char_u	*p;

    जइ (cmdidx == CMD_let || cmdidx == CMD_var
				 || cmdidx == CMD_const || cmdidx == CMD_final)
    {
	xp->xp_context = EXPAND_USER_VARS;
	जइ (vim_strpbrk(arg, (char_u *)"\"'+-*/%.=!?~|&$([<>,#") == रिक्त)
	{
	    // ":let var1 var2 ...": find last space.
	    प्रति (p = arg + STRLEN(arg); p >= arg; )
	    {
		xp->xp_pattern = p;
		MB_PTR_BACK(arg, p);
		जइ (VIM_ISWHITE(*p))
		    भङ्ग;
	    }
	    प्रत्यागम;
	}
    }
    अण्णहा
	xp->xp_context = cmdidx == CMD_call ? EXPAND_FUNCTIONS
							  : EXPAND_EXPRESSION;
    जाव ((xp->xp_pattern = vim_strpbrk(arg,
				  (char_u *)"\"'+-*/%.=!?~|&$([<>,#")) != रिक्त)
    {
	c = *xp->xp_pattern;
	जइ (c == '&')
	{
	    c = xp->xp_pattern[1];
	    जइ (c == '&')
	    {
		++xp->xp_pattern;
		xp->xp_context = has_व्यञ्जकम् ? EXPAND_EXPRESSION : EXPAND_NOTHING;
	    }
	    अण्णहा जइ (c != ' ')
	    {
		xp->xp_context = EXPAND_SETTINGS;
		जइ ((c == 'l' || c == 'g') && xp->xp_pattern[2] == ':')
		    xp->xp_pattern += 2;

	    }
	}
	अण्णहा जइ (c == '$')
	{
	    // environment variable
	    xp->xp_context = EXPAND_ENV_VARS;
	}
	अण्णहा जइ (c == '=')
	{
	    has_व्यञ्जकम् = TRUE;
	    xp->xp_context = EXPAND_EXPRESSION;
	}
	अण्णहा जइ (c == '#'
		&& xp->xp_context == EXPAND_EXPRESSION)
	{
	    // Autoload function/variable contains '#'.
	    भङ्ग;
	}
	अण्णहा जइ ((c == '<' || c == '#')
		&& xp->xp_context == EXPAND_FUNCTIONS
		&& vim_strchr(xp->xp_pattern, '(') == रिक्त)
	{
	    // Function name can start with "<SNR>" and contain '#'.
	    भङ्ग;
	}
	अण्णहा जइ (has_व्यञ्जकम्)
	{
	    जइ (c == '"')	    // string
	    {
		जाव ((c = *++xp->xp_pattern) != NUL && c != '"')
		    जइ (c == '\\' && xp->xp_pattern[1] != NUL)
			++xp->xp_pattern;
		xp->xp_context = EXPAND_NOTHING;
	    }
	    अण्णहा जइ (c == '\'')	    // literal string
	    {
		// Trick: '' is like stopping and starting a literal string.
		जाव ((c = *++xp->xp_pattern) != NUL && c != '\'')
		    /* skip */ ;
		xp->xp_context = EXPAND_NOTHING;
	    }
	    अण्णहा जइ (c == '|')
	    {
		जइ (xp->xp_pattern[1] == '|')
		{
		    ++xp->xp_pattern;
		    xp->xp_context = EXPAND_EXPRESSION;
		}
		अण्णहा
		    xp->xp_context = EXPAND_COMMANDS;
	    }
	    अण्णहा
		xp->xp_context = EXPAND_EXPRESSION;
	}
	अण्णहा
	    // Doesn't look like something valid, expand as an expression
	    // anyway.
	    xp->xp_context = EXPAND_EXPRESSION;
	arg = xp->xp_pattern;
	जइ (*arg != NUL)
	    जाव ((c = *++arg) != NUL && (c == ' ' || c == '\t'))
		/* skip */ ;
    }

    // ":exe one two" completes "two"
    जइ ((cmdidx == CMD_execute
		|| cmdidx == CMD_echo
		|| cmdidx == CMD_echon
		|| cmdidx == CMD_echomsg
		|| cmdidx == CMD_echoerr
		|| cmdidx == CMD_echoconsole
		|| cmdidx == CMD_echowindow)
	    && xp->xp_context == EXPAND_EXPRESSION)
    {
	प्रति (;;)
	{
	    char_u *n = skiptowhite(arg);

	    जइ (n == arg || IS_WHITE_OR_NUL(*skipwhite(n)))
		भङ्ग;
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
स्थिर regprog_T    *eval_prog_cache = रिक्त;
स्थिर char_u	    *eval_prog_pat = रिक्त;	// pattern it was compiled for
स्थिर char_u	    *eval_prog_enc = रिक्त;	// 'encoding' when compiled
स्थिर long	    eval_prog_re;		// 'regexpengine' when compiled
// State sampled by eval_regcomp() for the eval_regfree() call that gives the
// program back: user code run in between, e.g. an object's string() method
// invoked by match() on a list, can change these options, and then the
// program must not be cached under the changed values.  eval_compile_enc is
// only ever compared against p_enc, never dereferenced.
स्थिर long	    eval_compile_re;
स्थिर char_u	    *eval_compile_enc;
स्थिर पूर्णाङ्क	    eval_prog_busy = 0;	// programs eval_regcomp() handed out
					// that were not given back yet
स्थिर पूर्णाङ्क	    eval_prog_reentered = FALSE;  // eval_regcomp() ran while
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
    स्थिर bool
eval_prog_volatile(char_u *pat)
{
    char_u *p;

    जइ (STRNCMP(pat, "\\%#=", 4) == 0)
	प्रत्यागम true;
    प्रति (p = pat; *p != NUL; ++p)
	जइ (*p == '~'
		|| (p[0] == '[' && (p[1] == ':' || p[1] == '=' || p[1] == '.'))
		|| (p[0] == '%' && (p[1] == '.'
			|| ((p[1] == '<' || p[1] == '>') && p[2] == '.'))))
	    प्रत्यागम true;
    प्रत्यागम false;
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
    जइ (eval_prog_busy > 0)
	// The samples above no longer describe the program already handed
	// out: its eval_regfree() must drop it instead of caching it.
	eval_prog_reentered = TRUE;

    जइ (eval_prog_cache != रिक्त
	    && *p_cpo == NUL
	    && eval_prog_cache->re_flags == RE_MAGIC + RE_STRING
	    && eval_prog_re == p_re
	    && STRCMP(eval_prog_pat, pat) == 0
	    && STRCMP(eval_prog_enc, p_enc) == 0)
    {
	prog = eval_prog_cache;
	// The caller now owns the program, eval_regfree() adopts the
	// program the caller ends up with.
	eval_prog_cache = रिक्त;
    }
    अण्णहा
	prog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
    जइ (prog != रिक्त)
	++eval_prog_busy;	// paired with the eval_regfree() call
    प्रत्यागम prog;
}

/*
 * Free program "prog", obtained with eval_regcomp() for pattern "pat", by
 * keeping it in the cache for the next use.  "pat" must be the same string
 * the program was compiled for and must still be valid.
 */
    शून्य
eval_regfree(char_u *pat, regprog_T *prog)
{
    पूर्णाङ्क	    reentered = eval_prog_reentered;

    जइ (eval_prog_busy > 0 && --eval_prog_busy == 0)
	eval_prog_reentered = FALSE;
    जइ (prog == रिक्त)
	प्रत्यागम;
    जइ (reentered || *p_cpo != NUL
	    || eval_compile_re != p_re || eval_compile_enc != p_enc)
    {
	// 'cpoptions' is not empty, an option in the cache key changed, or
	// another program was handed out while the caller was using this
	// one: the samples may not describe this program, do not cache it.
	vim_regfree(prog);
	प्रत्यागम;
    }
    जइ (eval_prog_pat == रिक्त || STRCMP(eval_prog_pat, pat) != 0
	    || STRCMP(eval_prog_enc, p_enc) != 0)
    {
	char_u	*pat_copy;
	char_u	*enc_copy;

	// A cached pattern is known not to be volatile, thus the scan is
	// only needed when installing a new key.
	जइ (eval_prog_volatile(pat))
	{
	    // compiling depends on state the cache key does not cover
	    vim_regfree(prog);
	    प्रत्यागम;
	}
	pat_copy = vim_strsave(pat);
	enc_copy = vim_strsave(p_enc);
	जइ (pat_copy == रिक्त || enc_copy == रिक्त)
	{
	    vim_free(pat_copy);
	    vim_free(enc_copy);
	    vim_regfree(prog);
	    प्रत्यागम;
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

#जइ defined(EXITFREE)
    शून्य
free_eval_regcomp_cache(शून्य)
{
    vim_regfree(eval_prog_cache);
    eval_prog_cache = रिक्त;
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
    पूर्णाङ्क
pattern_match(char_u *pat, char_u *text, पूर्णाङ्क ic)
{
    पूर्णाङ्क		matches = FALSE;
    char_u	*save_cpo;
    regmatch_T	regmatch;

    // avoid 'l' flag in 'cpoptions'
    save_cpo = p_cpo;
    p_cpo = empty_option;
    regmatch.regprog = eval_regcomp(pat);
    जइ (regmatch.regprog != रिक्त)
    {
	regmatch.rm_ic = ic;
	matches = vim_regexec_nl(&regmatch, text, (colnr_T)0);
	eval_regfree(pat, regmatch.regprog);
    }
    p_cpo = save_cpo;
    प्रत्यागम matches;
}

/*
 * Handle a name followed by "(".  Both for just "name(arg)" and for
 * "expr->name(arg)".
 * Returns OK or FAIL.
 */
    स्थिर पूर्णाङ्क
eval_func(
	char_u	    **arg,	// points to "(", will be advanced
	evalarg_T   *evalarg,
	char_u	    *name,
	पूर्णाङ्क	    name_len,
	typval_T    *rettv,
	पूर्णाङ्क	    flags,
	typval_T    *basetv)	// "expr" for "expr->name(arg)"
{
    पूर्णाङ्क		evaluate = flags & EVAL_EVALUATE;
    char_u	*s = name;
    पूर्णाङ्क		len = name_len;
    partial_T	*partial;
    पूर्णाङ्क		ret = OK;
    type_T	*type = रिक्त;
    पूर्णाङ्क		found_var = FALSE;

    जइ (!evaluate)
	check_vars(s, len);

    // If "s" is the name of a variable of type VAR_FUNC
    // use its contents.
    s = deref_func_name(s, &len, &partial,
		 in_vim9script() ? &type : रिक्त, !evaluate, FALSE, &found_var);

    // Need to make a copy, in case evaluating the arguments makes
    // the name invalid.
    s = vim_strsave(s);
    जइ (s == रिक्त || (evaluate && *s == NUL))
	ret = FAIL;
    अण्णहा
    {
	funcexe_T funcexe;

	// Invoke the function.
	CLEAR_FIELD(funcexe);
	funcexe.fe_firstline = curwin->w_cursor.lnum;
	funcexe.fe_lastline = curwin->w_cursor.lnum;
	funcexe.fe_evaluate = evaluate;
	funcexe.fe_partial = partial;
	जइ (partial != रिक्त)
	{
	    funcexe.fe_object = partial->pt_obj;
	    जइ (funcexe.fe_object != रिक्त)
		++funcexe.fe_object->obj_refcount;
	}
	funcexe.fe_basetv = basetv;
	funcexe.fe_check_type = type;
	funcexe.fe_found_var = found_var;
	जइ (evalarg != रिक्त)
	    funcexe.fe_cctx = evalarg->eval_cctx;
	ret = get_func_tv(s, len, rettv, arg, evalarg, &funcexe);
    }
    vim_free(s);

    // If evaluate is FALSE rettv->v_type was not set in
    // get_func_tv, but it's needed in handle_subscript() to parse
    // what follows. So set it here.
    जइ (rettv->v_type == VAR_UNKNOWN && !evaluate && **arg == '(')
    {
	rettv->vval.v_string = रिक्त;
	rettv->v_type = VAR_FUNC;
    }

    // Stop the expression evaluation when immediately
    // aborting on error, or when an interrupt occurred or
    // an exception was thrown but not caught.
    जइ (evaluate && aborting())
    {
	जइ (ret == OK)
	    clear_tv(rettv);
	ret = FAIL;
    }
    प्रत्यागम ret;
}

/*
 * After a NL, skip over empty lines and comment-only lines.
 */
    स्थिर char_u *
newline_skip_comments(char_u *arg)
{
    char_u *p = arg + 1;

    प्रति (;;)
    {
	p = skipwhite(p);

	जइ (*p == NUL)
	    भङ्ग;
	जइ (vim9_comment_start(p))
	{
	    char_u *nl = vim_strchr(p, NL);

	    जइ (nl == रिक्त)
		भङ्ग;
	    p = nl;
	}
	जइ (*p != NL)
	    भङ्ग;
	++p;  // skip another NL
    }
    प्रत्यागम p;
}

/*
 * Get the next line source line without advancing.  But do skip over comment
 * lines.
 * Only called for Vim9 script.
 */
    स्थिर char_u *
getline_peek_skip_comments(evalarg_T *evalarg)
{
    प्रति (;;)
    {
	char_u *next = getline_peek(evalarg->eval_getline,
							 evalarg->eval_cookie);
	char_u *p;

	जइ (next == रिक्त)
	    भङ्ग;
	p = skipwhite(next);
	जइ (*p != NUL && !vim9_comment_start(p))
	    प्रत्यागम next;
	जइ (eval_next_पङ्क्तिः(रिक्त, evalarg) == रिक्त)
	    भङ्ग;
    }
    प्रत्यागम रिक्त;
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
eval_next_non_blank(char_u *arg, evalarg_T *evalarg, पूर्णाङ्क *getnext)
{
    char_u *p = skipwhite(arg);

    *getnext = FALSE;
    जइ (in_vim9script()
	    && evalarg != रिक्त
	    && (evalarg->eval_cookie != रिक्त || evalarg->eval_cctx != रिक्त
								   || *p == NL)
	    && (*p == NUL || *p == NL
			     || (vim9_comment_start(p) && VIM_ISWHITE(p[-1]))))
    {
	char_u *next;

	जइ (*p == NL)
	    next = newline_skip_comments(p);
	अण्णहा जइ (evalarg->eval_cookie != रिक्त)
	    next = getline_peek_skip_comments(evalarg);
	अण्णहा
	    next = peek_next_पङ्क्तिः_from_context(evalarg->eval_cctx);

	जइ (next != रिक्त)
	{
	    *getnext = *p != NL;
	    प्रत्यागम skipwhite(next);
	}
    }
    प्रत्यागम p;
}

/*
 * To be called after eval_next_non_blank() sets "getnext" to TRUE.
 * Only called for Vim9 script.
 *
 * If "arg" is not NULL, then the caller should assign the return value to
 * "arg".
 */
    char_u *
eval_next_पङ्क्तिः(char_u *arg, evalarg_T *evalarg)
{
    garray_T	*gap = &evalarg->eval_ga;
    char_u	*पङ्क्तिः;

    जइ (arg != रिक्त)
    {
	जइ (*arg == NL)
	    प्रत्यागम newline_skip_comments(arg);
	// Truncate before a trailing comment, so that concatenating the lines
	// won't turn the rest into a comment.
	जइ (*skipwhite(arg) == '#')
	    *arg = NUL;
    }

    जइ (evalarg->eval_cookie != रिक्त)
	पङ्क्तिः = evalarg->eval_getline(0, evalarg->eval_cookie, 0,
							   GETLINE_CONCAT_ALL);
    अण्णहा
	पङ्क्तिः = next_पङ्क्तिः_from_context(evalarg->eval_cctx, TRUE);
    जइ (पङ्क्तिः == रिक्त)
	प्रत्यागम रिक्त;

    ++evalarg->eval_break_गणनम्;
    जइ (gap->ga_itemsize > 0 && ga_grow(gap, 1) == OK)
    {
	char_u *p = skipwhite(पङ्क्तिः);

	// Going to concatenate the lines after parsing.  For an empty or
	// comment line use an empty string.
	जइ (*p == NUL || vim9_comment_start(p))
	{
	    vim_free(पङ्क्तिः);
	    पङ्क्तिः = vim_strsave((char_u *)"");
	}

	((char_u **)gap->ga_data)[gap->ga_len] = पङ्क्तिः;
	++gap->ga_len;
    }
    अण्णहा जइ (evalarg->eval_cookie != रिक्त)
    {
	free_eval_tofree_later(evalarg);
	evalarg->eval_tofree = पङ्क्तिः;
    }

    // Advanced to the next line, "arg" no longer points into the previous
    // line.  The caller assigns the return value to "arg".
    // If "arg" is NULL, then the return value is discarded.  In that case,
    // "arg" still points to the previous line.  So don't reset
    // "eval_using_cmdline".
    जइ (arg != रिक्त)
	evalarg->eval_using_cmdline = FALSE;
    प्रत्यागम skipwhite(पङ्क्तिः);
}

/*
 * Call eval_next_non_blank() and get the next line if needed.
 */
    char_u *
skipwhite_and_linebreak(char_u *arg, evalarg_T *evalarg)
{
    पूर्णाङ्क	    getnext;
    char_u  *p = skipwhite_and_nl(arg);

    जइ (evalarg == रिक्त)
	प्रत्यागम skipwhite(arg);
    eval_next_non_blank(p, evalarg, &getnext);
    जइ (getnext)
	प्रत्यागम eval_next_पङ्क्तिः(arg, evalarg);
    प्रत्यागम p;
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
    पूर्णाङ्क
eval0(
    char_u	*arg,
    typval_T	*rettv,
    exarg_T	*eap,
    evalarg_T	*evalarg)
{
    प्रत्यागम eval0_retarg(arg, rettv, eap, evalarg, रिक्त);
}

/*
 * If "arg" is a simple function call without arguments then call it and return
 * the result.  Otherwise return NOTDONE.
 */
    पूर्णाङ्क
may_call_simple_func(
    char_u	*arg,
    typval_T	*rettv)
{
    char_u  *parens = (char_u *)strstr((वर्ण *)arg, "()");
    पूर्णाङ्क	    r = NOTDONE;

    // If the expression is "FuncName()" then we can skip a lot of overhead.
    जइ (parens != रिक्त && *skipwhite(parens + 2) == NUL)
    {
	char_u *p = STRNCMP(arg, "<SNR>", 5) == 0 ? skipdigits(arg + 5) : arg;

	जइ (to_name_end(p, TRUE) == parens)
	    r = call_simple_func(arg, (size_t)(parens - arg), rettv);
    }
    प्रत्यागम r;
}

/*
 * Handle zero level expression with optimization for a simple function call.
 * Same arguments and return value as eval0().
 */
    स्थिर पूर्णाङ्क
eval0_simple_funccal(
    char_u	*arg,
    typval_T	*rettv,
    exarg_T	*eap,
    evalarg_T	*evalarg)
{
    पूर्णाङ्क	    r = may_call_simple_func(arg, rettv);

    जइ (r == NOTDONE)
	r = eval0_retarg(arg, rettv, eap, evalarg, रिक्त);
    प्रत्यागम r;
}

/*
 * Like eval0() but when "retarg" is not NULL store the pointer to after the
 * expression and don't check what comes after the expression.
 */
    पूर्णाङ्क
eval0_retarg(
    char_u	*arg,
    typval_T	*rettv,
    exarg_T	*eap,
    evalarg_T	*evalarg,
    char_u	**retarg)
{
    पूर्णाङ्क		ret;
    char_u	*p;
    char_u	*व्यञ्जकम्_end;
    पूर्णाङ्क		did_emsg_before = did_emsg;
    पूर्णाङ्क		called_emsg_before = called_emsg;
    पूर्णाङ्क		check_for_end = retarg == रिक्त;
    पूर्णाङ्क		end_error = FALSE;

    p = skipwhite(arg);
    ret = eval1(&p, rettv, evalarg);

    जइ (ret != FAIL)
    {
	व्यञ्जकम्_end = p;
	p = skipwhite(p);

	// In Vim9 script a command block is not split at NL characters for
	// commands using an expression argument.  Skip over a '#' comment to
	// check for a following NL.  Require white space before the '#'.
	जइ (in_vim9script() && p > व्यञ्जकम्_end && retarg == रिक्त)
	    जाव (*p == '#')
	    {
		char_u *nl = vim_strchr(p, NL);

		जइ (nl == रिक्त)
		    भङ्ग;
		p = skipwhite(nl + 1);
		जइ (eap != रिक्त && *p != NUL)
		    eap->nextcmd = p;
		check_for_end = FALSE;
	    }

	जइ (check_for_end)
	    end_error = !ends_excmd2(arg, p);
    }

    जइ (ret == FAIL || end_error)
    {
	जइ (ret != FAIL)
	    clear_tv(rettv);
	/*
	 * Report the invalid expression unless the expression evaluation has
	 * been cancelled due to an aborting error, an interrupt, or an
	 * exception, or we already gave a more specific error.
	 * Also check called_emsg for when using assert_fails().
	 */
	जइ (!aborting()
		&& did_emsg == did_emsg_before
		&& called_emsg == called_emsg_before
		&& (!in_vim9script() || !vim9_bad_comment(p)))
	{
	    जइ (end_error)
		semsg(_(e_trailing_characters_str), p);
	    अण्णहा
		semsg(_(e_invalid_expression_str), arg);
	}

	जइ (eap != रिक्त && p != रिक्त)
	{
	    // Some of the expression may not have been consumed.
	    // Only execute a next command if it cannot be a "||" operator.
	    // The next command may be "catch".
	    char_u *nextcmd = check_nextcmd(p);
	    जइ (nextcmd != रिक्त && *nextcmd != '|')
		eap->nextcmd = nextcmd;
	}
	प्रत्यागम FAIL;
    }

    जइ (retarg != रिक्त)
	*retarg = p;
    अण्णहा जइ (check_for_end && eap != रिक्त)
	set_nextcmd(eap, p);

    प्रत्यागम ret;
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
    पूर्णाङ्क
eval1(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    char_u  *p;
    पूर्णाङ्क	    getnext;

    CLEAR_POINTER(rettv);

    /*
     * Get the first variable.
     */
    जइ (eval2(arg, rettv, evalarg) == FAIL)
	प्रत्यागम FAIL;

    p = eval_next_non_blank(*arg, evalarg, &getnext);
    जइ (*p == '?')
    {
	पूर्णाङ्क		op_falsy = p[1] == '?';
	पूर्णाङ्क		result;
	typval_T	var2;
	evalarg_T	*evalarg_used = evalarg;
	evalarg_T	local_evalarg;
	पूर्णाङ्क		orig_flags;
	पूर्णाङ्क		evaluate;
	पूर्णाङ्क		vim9script = in_vim9script();

	जइ (evalarg == रिक्त)
	{
	    init_evalarg(&local_evalarg);
	    evalarg_used = &local_evalarg;
	}
	orig_flags = evalarg_used->eval_flags;
	evaluate = evalarg_used->eval_flags & EVAL_EVALUATE;

	जइ (getnext)
	    *arg = eval_next_पङ्क्तिः(*arg, evalarg_used);
	अण्णहा
	{
	    जइ (evaluate && vim9script && !VIM_ISWHITE(p[-1]))
	    {
		error_white_both(p, op_falsy ? 2 : 1);
		clear_tv(rettv);
		प्रत्यागम FAIL;
	    }
	    *arg = p;
	}

	result = FALSE;
	जइ (evaluate)
	{
	    पूर्णाङ्क		error = FALSE;

	    जइ (op_falsy)
	    {
		// Is this typeval supported with the falsy operator?
		जइ (check_typval_is_मानम्(rettv) == FAIL)
		{
		    clear_tv(rettv);
		    प्रत्यागम FAIL;
		}
		result = tv2bool(rettv);
	    }
	    अण्णहा जइ (vim9script)
		result = tv_get_bool_chk(rettv, &error);
	    अण्णहा जइ (tv_get_सङ्ख्या_chk(rettv, &error) != 0)
		result = TRUE;
	    जइ (error || !op_falsy || !result)
		clear_tv(rettv);
	    जइ (error)
		प्रत्यागम FAIL;
	}

	/*
	 * Get the second variable.  Recursive!
	 */
	जइ (op_falsy)
	    ++*arg;
	जइ (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[1]))
	{
	    error_white_both(*arg - (op_falsy ? 1 : 0), op_falsy ? 2 : 1);
	    clear_tv(rettv);
	    प्रत्यागम FAIL;
	}
	*arg = skipwhite_and_linebreak(*arg + 1, evalarg_used);
	evalarg_used->eval_flags = (op_falsy ? !result : result)
				  ? orig_flags : (orig_flags & ~EVAL_EVALUATE);
	जइ (eval1(arg, &var2, evalarg_used) == FAIL)
	{
	    evalarg_used->eval_flags = orig_flags;
	    प्रत्यागम FAIL;
	}
	जइ (!op_falsy || !result)
	    *rettv = var2;

	जइ (!op_falsy)
	{
	    /*
	     * Check for the ":".
	     */
	    p = eval_next_non_blank(*arg, evalarg_used, &getnext);
	    जइ (*p != ':')
	    {
		emsg(_(e_missing_colon_after_questionmark));
		जइ (evaluate && result)
		    clear_tv(rettv);
		evalarg_used->eval_flags = orig_flags;
		प्रत्यागम FAIL;
	    }
	    जइ (getnext)
		*arg = eval_next_पङ्क्तिः(*arg, evalarg_used);
	    अण्णहा
	    {
		जइ (evaluate && vim9script && !VIM_ISWHITE(p[-1]))
		{
		    error_white_both(p, 1);
		    clear_tv(rettv);
		    evalarg_used->eval_flags = orig_flags;
		    प्रत्यागम FAIL;
		}
		*arg = p;
	    }

	    /*
	     * Get the third variable.  Recursive!
	     */
	    जइ (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[1]))
	    {
		error_white_both(*arg, 1);
		clear_tv(rettv);
		evalarg_used->eval_flags = orig_flags;
		प्रत्यागम FAIL;
	    }
	    *arg = skipwhite_and_linebreak(*arg + 1, evalarg_used);
	    evalarg_used->eval_flags = !result ? orig_flags
					       : (orig_flags & ~EVAL_EVALUATE);
	    जइ (eval1(arg, &var2, evalarg_used) == FAIL)
	    {
		जइ (evaluate && result)
		    clear_tv(rettv);
		evalarg_used->eval_flags = orig_flags;
		प्रत्यागम FAIL;
	    }
	    जइ (evaluate && !result)
		*rettv = var2;
	}

	जइ (evalarg == रिक्त)
	    clear_evalarg(&local_evalarg, रिक्त);
	अण्णहा
	    evalarg->eval_flags = orig_flags;
    }

    प्रत्यागम OK;
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
    स्थिर पूर्णाङ्क
eval2(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    char_u	*p;
    पूर्णाङ्क		getnext;

    /*
     * Get the first expression.
     */
    जइ (eval3(arg, rettv, evalarg) == FAIL)
	प्रत्यागम FAIL;

    /*
     * Handle the  "||" operator.
     */
    p = eval_next_non_blank(*arg, evalarg, &getnext);
    जइ (p[0] == '|' && p[1] == '|')
    {
	evalarg_T   *evalarg_used = evalarg;
	evalarg_T   local_evalarg;
	पूर्णाङ्क	    evaluate;
	पूर्णाङ्क	    orig_flags;
	long	    result = FALSE;
	typval_T    var2;
	पूर्णाङ्क	    error = FALSE;
	पूर्णाङ्क	    vim9script = in_vim9script();

	जइ (evalarg == रिक्त)
	{
	    init_evalarg(&local_evalarg);
	    evalarg_used = &local_evalarg;
	}
	orig_flags = evalarg_used->eval_flags;
	evaluate = orig_flags & EVAL_EVALUATE;
	जइ (evaluate)
	{
	    जइ (vim9script)
		result = tv_get_bool_chk(rettv, &error);
	    अण्णहा जइ (tv_get_सङ्ख्या_chk(rettv, &error) != 0)
		result = TRUE;
	    clear_tv(rettv);
	    जइ (error)
		प्रत्यागम FAIL;
	}

	/*
	 * Repeat until there is no following "||".
	 */
	जाव (p[0] == '|' && p[1] == '|')
	{
	    जइ (getnext)
		*arg = eval_next_पङ्क्तिः(*arg, evalarg_used);
	    अण्णहा
	    {
		जइ (evaluate && vim9script && !VIM_ISWHITE(p[-1]))
		{
		    error_white_both(p, 2);
		    clear_tv(rettv);
		    प्रत्यागम FAIL;
		}
		*arg = p;
	    }

	    /*
	     * Get the second variable.
	     */
	    जइ (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[2]))
	    {
		error_white_both(*arg, 2);
		clear_tv(rettv);
		प्रत्यागम FAIL;
	    }
	    *arg = skipwhite_and_linebreak(*arg + 2, evalarg_used);
	    evalarg_used->eval_flags = !result ? orig_flags
					       : (orig_flags & ~EVAL_EVALUATE);
	    जइ (eval3(arg, &var2, evalarg_used) == FAIL)
		प्रत्यागम FAIL;

	    /*
	     * Compute the result.
	     */
	    जइ (evaluate && !result)
	    {
		जइ (vim9script)
		    result = tv_get_bool_chk(&var2, &error);
		अण्णहा जइ (tv_get_सङ्ख्या_chk(&var2, &error) != 0)
		    result = TRUE;
		clear_tv(&var2);
		जइ (error)
		    प्रत्यागम FAIL;
	    }
	    जइ (evaluate)
	    {
		जइ (vim9script)
		{
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_सङ्ख्या = result ? VVAL_TRUE : VVAL_FALSE;
		}
		अण्णहा
		{
		    rettv->v_type = VAR_सङ्ख्या;
		    rettv->vval.v_सङ्ख्या = result;
		}
	    }

	    p = eval_next_non_blank(*arg, evalarg_used, &getnext);
	}

	जइ (evalarg == रिक्त)
	    clear_evalarg(&local_evalarg, रिक्त);
	अण्णहा
	    evalarg->eval_flags = orig_flags;
    }

    प्रत्यागम OK;
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
    स्थिर पूर्णाङ्क
eval3(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    char_u	*p;
    पूर्णाङ्क		getnext;

    /*
     * Get the first expression.
     */
    जइ (eval4(arg, rettv, evalarg) == FAIL)
	प्रत्यागम FAIL;

    /*
     * Handle the "&&" operator.
     */
    p = eval_next_non_blank(*arg, evalarg, &getnext);
    जइ (p[0] == '&' && p[1] == '&')
    {
	evalarg_T   *evalarg_used = evalarg;
	evalarg_T   local_evalarg;
	पूर्णाङ्क	    orig_flags;
	पूर्णाङ्क	    evaluate;
	long	    result = TRUE;
	typval_T    var2;
	पूर्णाङ्क	    error = FALSE;
	पूर्णाङ्क	    vim9script = in_vim9script();

	जइ (evalarg == रिक्त)
	{
	    init_evalarg(&local_evalarg);
	    evalarg_used = &local_evalarg;
	}
	orig_flags = evalarg_used->eval_flags;
	evaluate = orig_flags & EVAL_EVALUATE;
	जइ (evaluate)
	{
	    जइ (vim9script)
		result = tv_get_bool_chk(rettv, &error);
	    अण्णहा जइ (tv_get_सङ्ख्या_chk(rettv, &error) == 0)
		result = FALSE;
	    clear_tv(rettv);
	    जइ (error)
		प्रत्यागम FAIL;
	}

	/*
	 * Repeat until there is no following "&&".
	 */
	जाव (p[0] == '&' && p[1] == '&')
	{
	    जइ (getnext)
		*arg = eval_next_पङ्क्तिः(*arg, evalarg_used);
	    अण्णहा
	    {
		जइ (evaluate && vim9script && !VIM_ISWHITE(p[-1]))
		{
		    error_white_both(p, 2);
		    clear_tv(rettv);
		    प्रत्यागम FAIL;
		}
		*arg = p;
	    }

	    /*
	     * Get the second variable.
	     */
	    जइ (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[2]))
	    {
		error_white_both(*arg, 2);
		clear_tv(rettv);
		प्रत्यागम FAIL;
	    }
	    *arg = skipwhite_and_linebreak(*arg + 2, evalarg_used);
	    evalarg_used->eval_flags = result ? orig_flags
					      : (orig_flags & ~EVAL_EVALUATE);
	    CLEAR_FIELD(var2);
	    जइ (eval4(arg, &var2, evalarg_used) == FAIL)
		प्रत्यागम FAIL;

	    /*
	     * Compute the result.
	     */
	    जइ (evaluate && result)
	    {
		जइ (vim9script)
		    result = tv_get_bool_chk(&var2, &error);
		अण्णहा जइ (tv_get_सङ्ख्या_chk(&var2, &error) == 0)
		    result = FALSE;
		clear_tv(&var2);
		जइ (error)
		    प्रत्यागम FAIL;
	    }
	    जइ (evaluate)
	    {
		जइ (vim9script)
		{
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_सङ्ख्या = result ? VVAL_TRUE : VVAL_FALSE;
		}
		अण्णहा
		{
		    rettv->v_type = VAR_सङ्ख्या;
		    rettv->vval.v_सङ्ख्या = result;
		}
	    }

	    p = eval_next_non_blank(*arg, evalarg_used, &getnext);
	}

	जइ (evalarg == रिक्त)
	    clear_evalarg(&local_evalarg, रिक्त);
	अण्णहा
	    evalarg->eval_flags = orig_flags;
    }

    प्रत्यागम OK;
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
    स्थिर पूर्णाङ्क
eval4(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    char_u	*p;
    पूर्णाङ्क		getnext;
    exprtype_T	type = व्यञ्जकम्_UNKNOWN;
    पूर्णाङ्क		len = 2;
    पूर्णाङ्क		type_is = FALSE;

    /*
     * Get the first expression.
     */
    जइ (eval5(arg, rettv, evalarg) == FAIL)
	प्रत्यागम FAIL;

    p = eval_next_non_blank(*arg, evalarg, &getnext);

    type = get_compare_type(p, &len, &type_is);

    /*
     * If there is a comparative operator, use it.
     */
    जइ (type != व्यञ्जकम्_UNKNOWN)
    {
	typval_T    var2;
	पूर्णाङ्क	    ic;
	पूर्णाङ्क	    vim9script = in_vim9script();
	पूर्णाङ्क	    evaluate = evalarg == रिक्त
				   ? 0 : (evalarg->eval_flags & EVAL_EVALUATE);
	long	    comp_lnum = SOURCING_LNUM;

	जइ (getnext)
	{
	    *arg = eval_next_पङ्क्तिः(*arg, evalarg);
	    p = *arg;
	}
	अण्णहा जइ (evaluate && vim9script && !VIM_ISWHITE(**arg))
	{
	    error_white_both(*arg, len);
	    clear_tv(rettv);
	    प्रत्यागम FAIL;
	}

	जइ (vim9script && type_is && (p[len] == '?' || p[len] == '#'))
	{
	    semsg(_(e_invalid_expression_str), p);
	    clear_tv(rettv);
	    प्रत्यागम FAIL;
	}

	// extra question mark appended: ignore case
	जइ (p[len] == '?')
	{
	    ic = TRUE;
	    ++len;
	}
	// extra '#' appended: match case
	अण्णहा जइ (p[len] == '#')
	{
	    ic = FALSE;
	    ++len;
	}
	// nothing appended: use 'ignorecase' if not in Vim script
	अण्णहा
	    ic = vim9script ? FALSE : p_ic;

	/*
	 * Get the second variable.
	 */
	जइ (evaluate && vim9script && !IS_WHITE_OR_NUL(p[len]))
	{
	    error_white_both(p, len);
	    clear_tv(rettv);
	    प्रत्यागम FAIL;
	}
	*arg = skipwhite_and_linebreak(p + len, evalarg);
	जइ (eval5(arg, &var2, evalarg) == FAIL)
	{
	    clear_tv(rettv);
	    प्रत्यागम FAIL;
	}
	जइ (evaluate)
	{
	    पूर्णाङ्क ret;

	    // use the line of the comparison for messages
	    SOURCING_LNUM = comp_lnum;
	    जइ (vim9script && check_compare_types(type, rettv, &var2) == FAIL)
	    {
		ret = FAIL;
		clear_tv(rettv);
	    }
	    अण्णहा
		ret = typval_compare(rettv, &var2, type, ic);
	    clear_tv(&var2);
	    प्रत्यागम ret;
	}
    }

    प्रत्यागम OK;
}

/*
 * Make a copy of blob "tv1" and append blob "tv2".
 */
    शून्य
eval_addblob(typval_T *tv1, typval_T *tv2)
{
    blob_T  *b1 = tv1->vval.v_blob;
    blob_T  *b2 = tv2->vval.v_blob;
    blob_T  *b = blob_alloc();
    long    len1, len2;
    long    totallen;

    जइ (b == रिक्त)
	प्रत्यागम;

    len1 = blob_len(b1);
    len2 = blob_len(b2);
    totallen = len1 + len2;

    जइ (totallen > 0 && totallen <= INT_MAX
				    && ga_grow(&b->bv_ga, (पूर्णाङ्क)totallen) == OK)
    {
	जइ (len1 > 0)
	    mch_memmove((char_u *)b->bv_ga.ga_data,
		    (char_u *)b1->bv_ga.ga_data, (size_t)len1);
	जइ (len2 > 0)
	    mch_memmove((char_u *)b->bv_ga.ga_data + len1,
		    (char_u *)b2->bv_ga.ga_data, (size_t)len2);
	b->bv_ga.ga_len = (पूर्णाङ्क)totallen;
    }

    clear_tv(tv1);
    rettv_blob_set(tv1, b);
}

/*
 * Make a copy of list "tv1" and append list "tv2".
 */
    पूर्णाङ्क
eval_addlist(typval_T *tv1, typval_T *tv2)
{
    typval_T var3;

    // concatenate Lists
    जइ (list_concat(tv1->vval.v_list, tv2->vval.v_list, &var3) == FAIL)
    {
	clear_tv(tv1);
	clear_tv(tv2);
	प्रत्यागम FAIL;
    }
    clear_tv(tv1);
    *tv1 = var3;
    प्रत्यागम OK;
}

/*
 * Make a copy of tuple "tv1" and append tuple "tv2".
 */
    पूर्णाङ्क
eval_addtuple(typval_T *tv1, typval_T *tv2)
{
    पूर्णाङ्क		vim9script = in_vim9script();
    typval_T	var3;

    जइ (vim9script && tv1->vval.v_tuple != रिक्त && tv2->vval.v_tuple != रिक्त
	    && tv1->vval.v_tuple->tv_type != रिक्त
	    && tv2->vval.v_tuple->tv_type != रिक्त)
    {
	जइ (!check_tuples_addable(tv1->vval.v_tuple->tv_type,
						tv2->vval.v_tuple->tv_type))
	    प्रत्यागम FAIL;
    }

    // concatenate tuples
    जइ (tuple_concat(tv1->vval.v_tuple, tv2->vval.v_tuple, &var3) == FAIL)
    {
	clear_tv(tv1);
	clear_tv(tv2);
	प्रत्यागम FAIL;
    }
    clear_tv(tv1);
    *tv1 = var3;
    प्रत्यागम OK;
}

/*
 * Left or right shift the number "tv1" by the number "tv2" and store the
 * result in "tv1".
 *
 * Return OK or FAIL.
 */
    स्थिर पूर्णाङ्क
eval_shift_सङ्ख्या(typval_T *tv1, typval_T *tv2, पूर्णाङ्क shift_type)
{
    जइ (tv2->v_type != VAR_सङ्ख्या || tv2->vval.v_सङ्ख्या < 0)
    {
	// right operand should be a positive number
	जइ (tv2->v_type != VAR_सङ्ख्या)
	    emsg(_(e_bitshift_ops_must_be_सङ्ख्या));
	अण्णहा
	    emsg(_(e_bitshift_ops_must_be_positive));
	clear_tv(tv1);
	clear_tv(tv2);
	प्रत्यागम FAIL;
    }

    जइ (tv2->vval.v_सङ्ख्या > MAX_LSHIFT_BITS)
	// shifting more bits than we have always results in zero
	tv1->vval.v_सङ्ख्या = 0;
    अण्णहा जइ (shift_type == व्यञ्जकम्_LSHIFT)
	tv1->vval.v_सङ्ख्या =
	    (uvarnumber_T)tv1->vval.v_सङ्ख्या << tv2->vval.v_सङ्ख्या;
    अण्णहा
	tv1->vval.v_सङ्ख्या =
	    (uvarnumber_T)tv1->vval.v_सङ्ख्या >> tv2->vval.v_सङ्ख्या;

    प्रत्यागम OK;
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
    स्थिर पूर्णाङ्क
eval5(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    /*
     * Get the first expression.
     */
    जइ (eval6(arg, rettv, evalarg) == FAIL)
	प्रत्यागम FAIL;

    /*
     * Repeat computing, until no '<<' or '>>' is following.
     */
    प्रति (;;)
    {
	char_u		*p;
	पूर्णाङ्क		getnext;
	exprtype_T	exprtype;
	पूर्णाङ्क		evaluate;
	typval_T	var2;
	पूर्णाङ्क		vim9script;

	p = eval_next_non_blank(*arg, evalarg, &getnext);
	जइ (p[0] == '<' && p[1] == '<')
	    exprtype = व्यञ्जकम्_LSHIFT;
	अण्णहा जइ (p[0] == '>' && p[1] == '>')
	    exprtype = व्यञ्जकम्_RSHIFT;
	अण्णहा
	    प्रत्यागम OK;

	// Handle a bitwise left or right shift operator
	evaluate = evalarg == रिक्त ? 0 : (evalarg->eval_flags & EVAL_EVALUATE);
	जइ (evaluate && rettv->v_type != VAR_सङ्ख्या)
	{
	    // left operand should be a number
	    emsg(_(e_bitshift_ops_must_be_सङ्ख्या));
	    clear_tv(rettv);
	    प्रत्यागम FAIL;
	}

	vim9script = in_vim9script();
	जइ (getnext)
	{
	    *arg = eval_next_पङ्क्तिः(*arg, evalarg);
	    p = *arg;
	}
	अण्णहा जइ (evaluate && vim9script && !VIM_ISWHITE(**arg))
	{
	    error_white_both(*arg, 2);
	    clear_tv(rettv);
	    प्रत्यागम FAIL;
	}

	/*
	 * Get the second variable.
	 */
	जइ (evaluate && vim9script && !IS_WHITE_OR_NUL(p[2]))
	{
	    error_white_both(p, 2);
	    clear_tv(rettv);
	    प्रत्यागम FAIL;
	}
	*arg = skipwhite_and_linebreak(p + 2, evalarg);
	जइ (eval6(arg, &var2, evalarg) == FAIL)
	{
	    clear_tv(rettv);
	    प्रत्यागम FAIL;
	}

	जइ (evaluate)
	{
	    जइ (eval_shift_सङ्ख्या(rettv, &var2, exprtype) == FAIL)
		प्रत्यागम FAIL;
	}

	clear_tv(&var2);
    }

    प्रत्यागम OK;
}

/*
 * Concatenate strings "tv1" and "tv2" and store the result in "tv1".
 */
    स्थिर पूर्णाङ्क
eval_concat_str(typval_T *tv1, typval_T *tv2)
{
    char_u	buf1[NUMBUFLEN], buf2[NUMBUFLEN];
    char_u	*s1 = tv_get_string_buf(tv1, buf1);
    char_u	*s2 = रिक्त;
    char_u	*p;
    पूर्णाङ्क		vim9script = in_vim9script();

    जइ (vim9script && (tv2->v_type == VAR_VOID
		|| tv2->v_type == VAR_CHANNEL
		|| tv2->v_type == VAR_JOB))
	semsg(_(e_using_invalid_मानम्_as_string_str),
		vartype_name(tv2->v_type));
    अण्णहा जइ (vim9script && tv2->v_type == VAR_FLOAT)
    {
	vim_snprintf((वर्ण *)buf2, NUMBUFLEN, "%g",
		tv2->vval.v_float);
	s2 = buf2;
    }
    अण्णहा
	s2 = tv_get_string_buf_chk(tv2, buf2);
    जइ (s2 == रिक्त)		// type error ?
    {
	clear_tv(tv1);
	clear_tv(tv2);
	प्रत्यागम FAIL;
    }

    // When possible, grow the existing string in place to avoid alloc/free.
    जइ (grow_string_tv(tv1, s2) == OK)
	प्रत्यागम OK;

    p = concat_str(s1, s2);
    clear_tv(tv1);
    tv1->v_type = VAR_STRING;
    tv1->vval.v_string = p;

    प्रत्यागम OK;
}

/*
 * Add or subtract numbers "tv1" and "tv2" and store the result in "tv1".
 * The numbers can be whole numbers or floats.
 */
    स्थिर पूर्णाङ्क
eval_addsub_सङ्ख्या(typval_T *tv1, typval_T *tv2, पूर्णाङ्क op)
{
    पूर्णाङ्क		error = FALSE;
    varnumber_T	n1, n2;
    float_T	f1 = 0, f2 = 0;

    जइ (tv1->v_type == VAR_FLOAT)
    {
	f1 = tv1->vval.v_float;
	n1 = 0;
    }
    अण्णहा
    {
	n1 = tv_get_सङ्ख्या_chk(tv1, &error);
	जइ (error)
	{
	    // This can only happen for "list + non-list" or
	    // "blob + non-blob".  For "non-list + ..." or
	    // "something - ...", we returned before evaluating the
	    // 2nd operand.
	    clear_tv(tv1);
	    clear_tv(tv2);
	    प्रत्यागम FAIL;
	}
	जइ (tv2->v_type == VAR_FLOAT)
	    f1 = n1;
    }
    जइ (tv2->v_type == VAR_FLOAT)
    {
	f2 = tv2->vval.v_float;
	n2 = 0;
    }
    अण्णहा
    {
	n2 = tv_get_सङ्ख्या_chk(tv2, &error);
	जइ (error)
	{
	    clear_tv(tv1);
	    clear_tv(tv2);
	    प्रत्यागम FAIL;
	}
	जइ (tv1->v_type == VAR_FLOAT)
	    f2 = n2;
    }
    clear_tv(tv1);

    // If there is a float on either side the result is a float.
    जइ (tv1->v_type == VAR_FLOAT || tv2->v_type == VAR_FLOAT)
    {
	जइ (op == '+')
	    f1 = f1 + f2;
	अण्णहा
	    f1 = f1 - f2;
	tv1->v_type = VAR_FLOAT;
	tv1->vval.v_float = f1;
    }
    अण्णहा
    {
	जइ (op == '+')
	    n1 = n1 + n2;
	अण्णहा
	    n1 = n1 - n2;
	tv1->v_type = VAR_सङ्ख्या;
	tv1->vval.v_सङ्ख्या = n1;
    }

    प्रत्यागम OK;
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
    स्थिर पूर्णाङ्क
eval6(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    /*
     * Get the first expression.
     */
    जइ (eval7(arg, rettv, evalarg, FALSE) == FAIL)
	प्रत्यागम FAIL;

    /*
     * Repeat computing, until no '+', '-' or '.' is following.
     */
    प्रति (;;)
    {
	पूर्णाङ्क	    evaluate;
	पूर्णाङ्क	    getnext;
	char_u	    *p;
	पूर्णाङ्क	    op;
	पूर्णाङ्क	    oplen;
	पूर्णाङ्क	    concat;
	typval_T    var2;
	पूर्णाङ्क	    vim9script = in_vim9script();
	long	    op_lnum = SOURCING_LNUM;

	// "." is only string concatenation when scriptversion is 1
	// "+=", "-=" and "..=" are assignments
	// "++" and "--" on the next line are a separate command.
	p = eval_next_non_blank(*arg, evalarg, &getnext);
	op = *p;
	concat = op == '.' && (*(p + 1) == '.' || in_old_script(2));
	जइ ((op != '+' && op != '-' && !concat) || p[1] == '='
					       || (p[1] == '.' && p[2] == '='))
	    भङ्ग;
	जइ (getnext && (op == '+' || op == '-') && p[0] == p[1])
	    भङ्ग;

	evaluate = evalarg == रिक्त ? 0 : (evalarg->eval_flags & EVAL_EVALUATE);
	oplen = (concat && p[1] == '.') ? 2 : 1;
	जइ (getnext)
	    *arg = eval_next_पङ्क्तिः(*arg, evalarg);
	अण्णहा
	{
	    जइ (evaluate && vim9script && !VIM_ISWHITE(**arg))
	    {
		error_white_both(*arg, oplen);
		clear_tv(rettv);
		प्रत्यागम FAIL;
	    }
	    *arg = p;
	}
	जइ ((op != '+' || (rettv->v_type != VAR_LIST
						&& rettv->v_type != VAR_TUPLE
						&& rettv->v_type != VAR_BLOB))
		&& (op == '.' || rettv->v_type != VAR_FLOAT)
		&& evaluate)
	{
	    पूर्णाङ्क		error = FALSE;

	    // For "list + ...", an illegal use of the first operand as
	    // a number cannot be determined before evaluating the 2nd
	    // operand: if this is also a list, all is ok.
	    // For "something . ...", "something - ..." or "non-list + ...",
	    // we know that the first operand needs to be a string or number
	    // without evaluating the 2nd operand.  So check before to avoid
	    // side effects after an error.
	    जइ (op != '.')
		tv_get_सङ्ख्या_chk(rettv, &error);
	    जइ ((op == '.' && tv_get_string_chk(rettv) == रिक्त) || error)
	    {
		clear_tv(rettv);
		प्रत्यागम FAIL;
	    }
	}

	/*
	 * Get the second variable.
	 */
	जइ (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[oplen]))
	{
	    error_white_both(*arg, oplen);
	    clear_tv(rettv);
	    प्रत्यागम FAIL;
	}
	*arg = skipwhite_and_linebreak(*arg + oplen, evalarg);
	जइ (eval7(arg, &var2, evalarg, !vim9script && op == '.') == FAIL)
	{
	    clear_tv(rettv);
	    प्रत्यागम FAIL;
	}

	जइ (evaluate)
	{
	    /*
	     * Compute the result.
	     */
	    // use the line of the operation for messages
	    SOURCING_LNUM = op_lnum;
	    जइ (op == '.')
	    {
		जइ (eval_concat_str(rettv, &var2) == FAIL)
		    प्रत्यागम FAIL;
	    }
	    अण्णहा जइ (op == '+' && rettv->v_type == VAR_BLOB
						   && var2.v_type == VAR_BLOB)
		eval_addblob(rettv, &var2);
	    अण्णहा जइ (op == '+' && rettv->v_type == VAR_LIST
						   && var2.v_type == VAR_LIST)
	    {
		जइ (eval_addlist(rettv, &var2) == FAIL)
		    प्रत्यागम FAIL;
	    }
	    अण्णहा जइ (op == '+' && rettv->v_type == VAR_TUPLE
					   && var2.v_type == VAR_TUPLE)
	    {
		जइ (eval_addtuple(rettv, &var2) == FAIL)
		    प्रत्यागम FAIL;
	    }
	    अण्णहा
	    {
		जइ (eval_addsub_सङ्ख्या(rettv, &var2, op) == FAIL)
		    प्रत्यागम FAIL;
	    }
	    clear_tv(&var2);
	}
    }
    प्रत्यागम OK;
}

/*
 * Multiply or divide or compute the modulo of numbers "tv1" and "tv2" and
 * store the result in "tv1".  The numbers can be whole numbers or floats.
 */
    स्थिर पूर्णाङ्क
eval_multdiv_सङ्ख्या(typval_T *tv1, typval_T *tv2, पूर्णाङ्क op)
{
    varnumber_T	n1, n2;
    float_T	f1, f2;
    पूर्णाङ्क		error;
    पूर्णाङ्क		use_float = FALSE;

    f1 = 0;
    f2 = 0;
    error = FALSE;
    जइ (tv1->v_type == VAR_FLOAT)
    {
	f1 = tv1->vval.v_float;
	use_float = TRUE;
	n1 = 0;
    }
    अण्णहा
	n1 = tv_get_सङ्ख्या_chk(tv1, &error);
    clear_tv(tv1);
    जइ (error)
    {
	clear_tv(tv2);
	प्रत्यागम FAIL;
    }

    जइ (tv2->v_type == VAR_FLOAT)
    {
	जइ (!use_float)
	{
	    f1 = n1;
	    use_float = TRUE;
	}
	f2 = tv2->vval.v_float;
	n2 = 0;
    }
    अण्णहा
    {
	n2 = tv_get_सङ्ख्या_chk(tv2, &error);
	clear_tv(tv2);
	जइ (error)
	    प्रत्यागम FAIL;
	जइ (use_float)
	    f2 = n2;
    }

    /*
     * Compute the result.
     * When either side is a float the result is a float.
     */
    जइ (use_float)
    {
	जइ (op == '*')
	    f1 = f1 * f2;
	अण्णहा जइ (op == '/')
	{
#ifdef VMS
	    // VMS crashes on divide by zero, work around it
	    जइ (f2 == 0.0)
	    {
		जइ (f1 == 0)
		    f1 = -1 * __F_FLT_MAX - 1L;   // similar to NaN
		अण्णहा जइ (f1 < 0)
		    f1 = -1 * __F_FLT_MAX;
		अण्णहा
		    f1 = __F_FLT_MAX;
	    }
	    अण्णहा
		f1 = f1 / f2;
#अण्णहा
	    // We rely on the floating point library to handle divide
	    // by zero to result in "inf" and not a crash.
	    f1 = f1 / f2;
#endif
	}
	अण्णहा
	{
	    emsg(_(e_cannot_use_percent_with_float));
	    प्रत्यागम FAIL;
	}
	tv1->v_type = VAR_FLOAT;
	tv1->vval.v_float = f1;
    }
    अण्णहा
    {
	पूर्णाङ्क	    failed = FALSE;

	जइ (op == '*')
	    n1 = n1 * n2;
	अण्णहा जइ (op == '/')
	    n1 = अङ्कः_divide(n1, n2, &failed);
	अण्णहा
	    n1 = अङ्कः_modulus(n1, n2, &failed);
	जइ (failed)
	    प्रत्यागम FAIL;

	tv1->v_type = VAR_सङ्ख्या;
	tv1->vval.v_सङ्ख्या = n1;
    }

    प्रत्यागम OK;
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
    स्थिर पूर्णाङ्क
eval7(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    पूर्णाङ्क		want_string)  // after "." operator
{
    /*
     * Get the first expression.
     */
    जइ (eval8(arg, rettv, evalarg, want_string) == FAIL)
	प्रत्यागम FAIL;

    /*
     * Repeat computing, until no '*', '/' or '%' is following.
     */
    प्रति (;;)
    {
	पूर्णाङ्क	    evaluate;
	पूर्णाङ्क	    getnext;
	typval_T    var2;
	char_u	    *p;
	पूर्णाङ्क	    op;

	// "*=", "/=" and "%=" are assignments
	p = eval_next_non_blank(*arg, evalarg, &getnext);
	op = *p;
	जइ ((op != '*' && op != '/' && op != '%') || p[1] == '=')
	    भङ्ग;

	evaluate = evalarg == रिक्त ? 0 : (evalarg->eval_flags & EVAL_EVALUATE);
	जइ (getnext)
	    *arg = eval_next_पङ्क्तिः(*arg, evalarg);
	अण्णहा
	{
	    जइ (evaluate && in_vim9script() && !VIM_ISWHITE(**arg))
	    {
		error_white_both(*arg, 1);
		clear_tv(rettv);
		प्रत्यागम FAIL;
	    }
	    *arg = p;
	}

	/*
	 * Get the second variable.
	 */
	जइ (evaluate && in_vim9script() && !IS_WHITE_OR_NUL((*arg)[1]))
	{
	    error_white_both(*arg, 1);
	    clear_tv(rettv);
	    प्रत्यागम FAIL;
	}
	*arg = skipwhite_and_linebreak(*arg + 1, evalarg);
	जइ (eval8(arg, &var2, evalarg, FALSE) == FAIL)
	    प्रत्यागम FAIL;

	जइ (evaluate)
	    // Compute the result.
	    जइ (eval_multdiv_सङ्ख्या(rettv, &var2, op) == FAIL)
		प्रत्यागम FAIL;
    }

    प्रत्यागम OK;
}

/*
 * Handle seventh level expression:
 *	a type cast before a base level expression.
 * "arg" must point to the first non-white of the expression.
 * "arg" is advanced to just after the recognized expression.
 * Return OK or FAIL.
 */
    स्थिर पूर्णाङ्क
eval8(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    पूर्णाङ्क		want_string)	// after "." operator
{
    type_T	*want_type = रिक्त;
    garray_T	type_list;	    // list of pointers to allocated types
    पूर्णाङ्क		res;
    पूर्णाङ्क		evaluate = evalarg == रिक्त ? 0
				       : (evalarg->eval_flags & EVAL_EVALUATE);

    // Recognize <type> in Vim9 script only.
    जइ (in_vim9script() && **arg == '<' && eval_isnamec1((*arg)[1])
					     && STRNCMP(*arg, "<SNR>", 5) != 0)
    {
	++*arg;
	ga_init2(&type_list, परिमाण(type_T *), 10);
	जइ (!evaluate)
	    // Only skip over the type, it may use a type variable that is only
	    // known when the expression is compiled, e.g. in a lambda.
	    *arg = skip_type(*arg, FALSE);
	अण्णहा
	{
	    want_type = parse_type(arg, &type_list, रिक्त, रिक्त, TRUE);
	    जइ (want_type == रिक्त)
	    {
		clear_type_list(&type_list);
		प्रत्यागम FAIL;
	    }
	}

	जइ (**arg != '>')
	{
	    जइ (*skipwhite(*arg) == '>')
		semsg(_(e_no_white_space_allowed_before_str_str), ">", *arg);
	    अण्णहा
		emsg(_(e_missing_gt));
	    clear_type_list(&type_list);
	    प्रत्यागम FAIL;
	}
	++*arg;
	*arg = skipwhite_and_linebreak(*arg, evalarg);
    }

    res = eval9(arg, rettv, evalarg, want_string);

    जइ (want_type != रिक्त)
    {
	जइ (evaluate && res == OK)
	{
	    type_T *actual = typval2type(rettv, get_copyID(), &type_list,
							       TVTT_DO_MEMBER);

	    जइ (!equal_type(want_type, actual, 0))
	    {
		जइ (want_type->tt_type == VAR_BOOL
					&& actual->tt_type != VAR_BOOL
					&& (actual->tt_flags & TTFLAG_BOOL_OK))
		{
		    पूर्णाङ्क n = tv2bool(rettv);

		    // can use "0" and "1" for boolean in some places
		    clear_tv(rettv);
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_सङ्ख्या = n ? VVAL_TRUE : VVAL_FALSE;
		}
		अण्णहा
		{
		    where_T where = WHERE_INIT;

		    res = check_type(want_type, actual, TRUE, where);
		}
	    }
	}
	clear_type_list(&type_list);
    }

    प्रत्यागम res;
}

    पूर्णाङ्क
eval_leader(char_u **arg, पूर्णाङ्क vim9)
{
    char_u	*s = *arg;
    char_u	*p = *arg;

    जाव (*p == '!' || *p == '-' || *p == '+')
    {
	char_u *n = skipwhite(p + 1);

	// ++, --, -+ and +- are not accepted in Vim9 script
	जइ (vim9 && (*p == '-' || *p == '+') && (*n == '-' || *n == '+'))
	{
	    semsg(_(e_invalid_expression_str), s);
	    प्रत्यागम FAIL;
	}
	p = n;
    }
    *arg = p;
    प्रत्यागम OK;
}

/*
 * Check for a predefined value "true", "false" and "null.*".
 * Return OK when recognized.
 */
    पूर्णाङ्क
handle_predefined(char_u *s, पूर्णाङ्क len, typval_T *rettv)
{
    विकल्प (len)
    {
	पक्ष 4: जइ (STRNCMP(s, "true", 4) == 0)
		{
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_सङ्ख्या = VVAL_TRUE;
		    प्रत्यागम OK;
		}
		जइ (STRNCMP(s, "null", 4) == 0)
		{
		    rettv->v_type = VAR_SPECIAL;
		    rettv->vval.v_सङ्ख्या = VVAL_NULL;
		    प्रत्यागम OK;
		}
		भङ्ग;
	पक्ष 5: जइ (STRNCMP(s, "false", 5) == 0)
		{
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_सङ्ख्या = VVAL_FALSE;
		    प्रत्यागम OK;
		}
		भङ्ग;
	पक्ष 8: जइ (STRNCMP(s, "null_job", 8) == 0)
		{
#ifdef FEAT_JOB_CHANNEL
		    rettv->v_type = VAR_JOB;
		    rettv->vval.v_job = रिक्त;
#अण्णहा
		    rettv->v_type = VAR_SPECIAL;
		    rettv->vval.v_सङ्ख्या = VVAL_NULL;
#endif
		    प्रत्यागम OK;
		}
		भङ्ग;
	पक्ष 9:
		जइ (STRNCMP(s, "null_", 5) != 0)
		    भङ्ग;
		// null_list
		जइ (STRNCMP(s + 5, "list", 4) == 0)
		{
		    rettv->v_type = VAR_LIST;
		    rettv->vval.v_list = रिक्त;
		    प्रत्यागम OK;
		}
		// null_dict
		जइ (STRNCMP(s + 5, "dict", 4) == 0)
		{
		    rettv->v_type = VAR_DICT;
		    rettv->vval.v_dict = रिक्त;
		    प्रत्यागम OK;
		}
		// null_blob
		जइ (STRNCMP(s + 5, "blob", 4) == 0)
		{
		    rettv->v_type = VAR_BLOB;
		    rettv->vval.v_blob = रिक्त;
		    प्रत्यागम OK;
		}
		भङ्ग;
	पक्ष 10:
		जइ (STRNCMP(s, "null_", 5) != 0)
		    भङ्ग;
		// null_class
		जइ (STRNCMP(s + 5, "class", 5) == 0)
		{
		    rettv->v_type = VAR_CLASS;
		    rettv->vval.v_class = रिक्त;
		    प्रत्यागम OK;
		}
		जइ (STRNCMP(s + 5, "tuple", 5) == 0)
		{
		    rettv->v_type = VAR_TUPLE;
		    rettv->vval.v_tuple = रिक्त;
		    प्रत्यागम OK;
		}
		भङ्ग;
	पक्ष 11: जइ (STRNCMP(s, "null_string", 11) == 0)
		{
		    rettv->v_type = VAR_STRING;
		    rettv->vval.v_string = रिक्त;
		    प्रत्यागम OK;
		}
		जइ (STRNCMP(s, "null_object", 11) == 0)
		{
		    rettv->v_type = VAR_OBJECT;
		    rettv->vval.v_object = रिक्त;
		    प्रत्यागम OK;
		}
		भङ्ग;
	पक्ष 12:
		जइ (STRNCMP(s, "null_channel", 12) == 0)
		{
#ifdef FEAT_JOB_CHANNEL
		    rettv->v_type = VAR_CHANNEL;
		    rettv->vval.v_channel = रिक्त;
#अण्णहा
		    rettv->v_type = VAR_SPECIAL;
		    rettv->vval.v_सङ्ख्या = VVAL_NULL;
#endif
		    प्रत्यागम OK;
		}
		जइ (STRNCMP(s, "null_partial", 12) == 0)
		{
		    rettv->v_type = VAR_PARTIAL;
		    rettv->vval.v_partial = रिक्त;
		    प्रत्यागम OK;
		}
		भङ्ग;
	पक्ष 13: जइ (STRNCMP(s, "null_function", 13) == 0)
		{
		    rettv->v_type = VAR_FUNC;
		    rettv->vval.v_string = रिक्त;
		    प्रत्यागम OK;
		}
		भङ्ग;
    }
    प्रत्यागम FAIL;
}

/*
 * Handle register contents: @r.
 */
    स्थिर शून्य
eval9_reg_contents(
    char_u	**arg,
    typval_T	*rettv,
    पूर्णाङ्क		evaluate)
{
    पूर्णाङ्क		vim9script = in_vim9script();

    ++*arg;	// skip '@'
    जइ (evaluate)
    {
	जइ (vim9script && IS_WHITE_OR_NUL(**arg))
	    semsg(_(e_syntax_error_at_str), *arg);
	अण्णहा जइ (vim9script && !valid_yank_reg(**arg, FALSE))
	    emsg_invreg(**arg);
	अण्णहा
	{
	    rettv->v_type = VAR_STRING;
	    rettv->vval.v_string = get_reg_contents(**arg,
		    GREG_व्यञ्जकम्_SRC);
	}
    }
    जइ (**arg != NUL)
	++*arg;
}

/*
 * Handle a nested expression: (expression) or lambda: (arg) => expr
 */
    स्थिर पूर्णाङ्क
eval9_nested_व्यञ्जकम्(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    पूर्णाङ्क		evaluate)
{
    पूर्णाङ्क		ret = NOTDONE;
    पूर्णाङ्क		vim9script = in_vim9script();

    जइ (vim9script)
    {
	ret = get_lambda_tv(arg, rettv, TRUE, evalarg, रिक्त);
	जइ (ret == OK && evaluate)
	{
	    ufunc_T *ufunc = rettv->vval.v_partial->pt_func;

	    // Compile it here to get the return type.  The return
	    // type is optional, when it's missing use t_unknown.
	    // This is recognized in compile_return().
	    जइ (ufunc->uf_ret_type->tt_type == VAR_VOID)
		ufunc->uf_ret_type = &t_unknown;
	    जइ (compile_def_function(ufunc, FALSE,
				get_compile_type(ufunc), रिक्त) == FAIL)
	    {
		clear_tv(rettv);
		ret = FAIL;
	    }
	}
    }
    जइ (ret == NOTDONE)
    {
	*arg = skipwhite_and_linebreak(*arg + 1, evalarg);
	जइ (**arg == ')')
	    // empty tuple
	    ret = eval_tuple(arg, rettv, evalarg, TRUE);
	अण्णहा
	{
	    ret = eval1(arg, rettv, evalarg);	// recursive!
	    जइ (ret != OK)
		प्रत्यागम ret;

	    *arg = skipwhite_and_linebreak(*arg, evalarg);

	    जइ (**arg == ',')
		// tuple
		ret = eval_tuple(arg, rettv, evalarg, TRUE);
	    अण्णहा जइ (**arg == ')')
		++*arg;
	    अण्णहा जइ (ret == OK)
	    {
		emsg(_(e_missing_closing_paren));
		clear_tv(rettv);
		ret = FAIL;
	    }
	}
    }

    प्रत्यागम ret;
}

/*
* Handle be a variable or function name.
* Can also be a curly-braces kind of name: {expr}.
*/
    स्थिर पूर्णाङ्क
eval9_var_func_name(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    पूर्णाङ्क		evaluate,
    char_u	**name_start)
{
    char_u	*s;
    पूर्णाङ्क		len;
    char_u	*alias;
    पूर्णाङ्क		ret = OK;
    पूर्णाङ्क		vim9script = in_vim9script();

    s = *arg;
    len = get_name_len(arg, &alias, evaluate, TRUE);
    जइ (alias != रिक्त)
	s = alias;

    जइ (len <= 0)
	ret = FAIL;
    अण्णहा
    {
	पूर्णाङ्क	flags = evalarg == रिक्त ? 0 : evalarg->eval_flags;

	जइ (evaluate && vim9script && len == 1 && *s == '_')
	{
	    emsg(_(e_cannot_use_underscore_here));
	    ret = FAIL;
	}
	अण्णहा जइ (evaluate && vim9script && len > 2
						&& s[0] == 's' && s[1] == ':')
	{
	    semsg(_(e_cannot_use_s_colon_in_vim9_script_str), s);
	    ret = FAIL;
	}
	अण्णहा जइ ((vim9script ? **arg : *skipwhite(*arg)) == '('
				|| (vim9script && generic_func_call(arg)))
	{
	    // "name(..."  recursive!
	    *arg = skipwhite(*arg);
	    ret = eval_func(arg, evalarg, s, len, rettv, flags, रिक्त);
	}
	अण्णहा जइ (evaluate)
	{
	    // get the value of "true", "false", etc. or a variable
	    ret = FAIL;
	    जइ (vim9script)
		ret = handle_predefined(s, len, rettv);
	    जइ (ret == FAIL)
	    {
		*name_start = s;
		ret = eval_variable(s, len, 0, rettv, रिक्त,
					EVAL_VAR_VERBOSE + EVAL_VAR_IMPORT);

		// skip the generic function arguments (if present)
		// they are already processed by eval_variable
		जइ (ret == OK && vim9script && **arg == '<'
						&& rettv->v_type == VAR_FUNC)
		    ret = skip_generic_func_type_args(arg);
	    }
	}
	अण्णहा
	{
	    // skip the name
	    check_vars(s, len);

	    // Skip the type arguments of a generic function, its name starts
	    // with an uppercase letter.
	    जइ (vim9script && **arg == '<' && ASCII_ISUPPER(*s))
		skip_generic_func_type_args(arg);
	    ret = OK;
	}
    }
    vim_free(alias);

    प्रत्यागम ret;
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
    स्थिर पूर्णाङ्क
eval9(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    पूर्णाङ्क		want_string)	// after "." operator
{
    पूर्णाङ्क		evaluate = evalarg != रिक्त
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    char_u	*name_start = रिक्त;
    char_u	*start_leader, *end_leader;
    पूर्णाङ्क		ret = OK;
    स्थिर पूर्णाङ्क	recurse = 0;
    पूर्णाङ्क		vim9script = in_vim9script();

    /*
     * Initialise variable so that clear_tv() can't mistake this for a
     * string and free a string that isn't there.
     */
    rettv->v_type = VAR_UNKNOWN;

    /*
     * Skip '!', '-' and '+' characters.  They are handled later.
     */
    start_leader = *arg;
    जइ (eval_leader(arg, vim9script) == FAIL)
	प्रत्यागम FAIL;
    end_leader = *arg;

    जइ (**arg == '.' && (!SAFE_isdigit(*(*arg + 1)) || in_old_script(2)))
    {
	semsg(_(e_invalid_expression_str), *arg);
	++*arg;
	प्रत्यागम FAIL;
    }

    // Limit recursion to 1000 levels.  At least at 10000 we run out of stack
    // and crash.  With MSVC the stack is smaller.
    जइ (recurse ==
#ifdef _MSC_VER
		    300
#अण्णहा
		    1000
#endif
		    )
    {
	semsg(_(e_expression_too_recursive_str), *arg);
	प्रत्यागम FAIL;
    }
    ++recurse;

    विकल्प (**arg)
    {
    /*
     * Number constant.
     */
    पक्ष '0':
    पक्ष '1':
    पक्ष '2':
    पक्ष '3':
    पक्ष '4':
    पक्ष '5':
    पक्ष '6':
    पक्ष '7':
    पक्ष '8':
    पक्ष '9':
    पक्ष '.':	ret = eval_सङ्ख्या(arg, rettv, evaluate, want_string);

		// Apply prefixed "-" and "+" now.  Matters especially when
		// "->" follows.
		जइ (ret == OK && evaluate && end_leader > start_leader
						  && rettv->v_type != VAR_BLOB)
		    ret = eval9_leader(rettv, TRUE, start_leader, &end_leader);
		भङ्ग;

    /*
     * String constant: "string".
     */
    पक्ष '"':	ret = eval_string(arg, rettv, evaluate, FALSE);
		भङ्ग;

    /*
     * Literal string constant: 'str''ing'.
     */
    पक्ष '\'':	ret = eval_lit_string(arg, rettv, evaluate, FALSE);
		भङ्ग;

    /*
     * List: [expr, expr]
     */
    पक्ष '[':	ret = eval_list(arg, rettv, evalarg, TRUE);
		भङ्ग;

    /*
     * Literal Dictionary: #{key: val, key: val}
     */
    पक्ष '#':	ret = eval_lit_dict(arg, rettv, evalarg);
		भङ्ग;

    /*
     * Lambda: {arg, arg -> expr}
     * Dictionary: {'key': val, 'key': val}
     */
    पक्ष '{':	जइ (vim9script)
		    ret = NOTDONE;
		अण्णहा
		    ret = get_lambda_tv(arg, rettv, vim9script, evalarg, रिक्त);
		जइ (ret == NOTDONE)
		    ret = eval_dict(arg, rettv, evalarg, FALSE);
		भङ्ग;

    /*
     * Option value: &name
     */
    पक्ष '&':	ret = eval_option(arg, rettv, evaluate);
		भङ्ग;

    /*
     * Environment variable: $VAR.
     * Interpolated string: $"string" or $'string'.
     */
    पक्ष '$':	जइ ((*arg)[1] == '"' || (*arg)[1] == '\'')
		    ret = eval_interp_string(arg, rettv, evaluate);
		अण्णहा
		    ret = eval_env_var(arg, rettv, evaluate);
		भङ्ग;

    /*
     * Register contents: @r.
     */
    पक्ष '@':	eval9_reg_contents(arg, rettv, evaluate);
		भङ्ग;

    /*
     * nested expression: (expression).
     * or lambda: (arg) => expr
     * or tuple
     */
    पक्ष '(':	ret = eval9_nested_व्यञ्जकम्(arg, rettv, evalarg, evaluate);
		भङ्ग;

    स्वभाव:	ret = NOTDONE;
		भङ्ग;
    }

    जइ (ret == NOTDONE)
    {
	/*
	 * Must be a variable or function name.
	 * Can also be a curly-braces kind of name: {expr}.
	 */
	ret = eval9_var_func_name(arg, rettv, evalarg, evaluate, &name_start);
    }

    // Handle following '[', '(' and '.' for expr[expr], expr.name,
    // expr(expr), expr->name(expr)
    जइ (ret == OK)
	ret = handle_subscript(arg, name_start, rettv, evalarg, evaluate);

    /*
     * Apply logical NOT and unary '-', from right to left, ignore '+'.
     */
    जइ (ret == OK && evaluate && end_leader > start_leader)
	ret = eval9_leader(rettv, FALSE, start_leader, &end_leader);

    --recurse;
    प्रत्यागम ret;
}

/*
 * Apply the leading "!" and "-" before an eval9 expression to "rettv".
 * When "numeric_only" is TRUE only handle "+" and "-".
 * Adjusts "end_leaderp" until it is at "start_leader".
 */
    स्थिर पूर्णाङ्क
eval9_leader(
	typval_T    *rettv,
	पूर्णाङ्क	    numeric_only,
	char_u	    *start_leader,
	char_u	    **end_leaderp)
{
    char_u	*end_leader = *end_leaderp;
    पूर्णाङ्क		ret = OK;
    पूर्णाङ्क		error = FALSE;
    varnumber_T val = 0;
    vartype_T	type = rettv->v_type;
    पूर्णाङ्क		vim9script = in_vim9script();
    float_T	    f = 0.0;

    जइ (rettv->v_type == VAR_FLOAT)
	f = rettv->vval.v_float;
    अण्णहा
    {
	जाव (VIM_ISWHITE(end_leader[-1]))
	    --end_leader;
	जइ (vim9script && end_leader[-1] == '!')
	{
	    // Is this typeval supported with the ! operator?
	    जइ (check_typval_is_मानम्(rettv) == FAIL)
	    {
		clear_tv(rettv);
		प्रत्यागम FAIL;
	    }
	    val = tv2bool(rettv);
	}
	अण्णहा
	    val = tv_get_सङ्ख्या_chk(rettv, &error);
    }
    जइ (error)
    {
	clear_tv(rettv);
	ret = FAIL;
    }
    अण्णहा
    {
	जाव (end_leader > start_leader)
	{
	    --end_leader;
	    जइ (*end_leader == '!')
	    {
		जइ (numeric_only)
		{
		    ++end_leader;
		    भङ्ग;
		}
		जइ (rettv->v_type == VAR_FLOAT)
		{
		    rettv->v_type = VAR_BOOL;
		    val = f == 0.0 ? VVAL_TRUE : VVAL_FALSE;
		    type = VAR_BOOL;
		}
		अण्णहा
		{
		    val = !val;
		    type = VAR_BOOL;
		}
	    }
	    अण्णहा जइ (*end_leader == '-')
	    {
		जइ (rettv->v_type == VAR_FLOAT)
		    f = -f;
		अण्णहा
		{
		    val = -val;
		    type = VAR_सङ्ख्या;
		}
	    }
	}
	जइ (rettv->v_type == VAR_FLOAT)
	{
	    clear_tv(rettv);
	    rettv->vval.v_float = f;
	}
	अण्णहा
	{
	    clear_tv(rettv);
	    जइ (vim9script)
		rettv->v_type = type;
	    अण्णहा
		rettv->v_type = VAR_सङ्ख्या;
	    rettv->vval.v_सङ्ख्या = val;
	}
    }
    *end_leaderp = end_leader;
    प्रत्यागम ret;
}

/*
 * Call the function referred to in "rettv".
 */
    स्थिर पूर्णाङ्क
call_func_rettv(
	char_u	    **arg,
	evalarg_T   *evalarg,
	typval_T    *rettv,
	पूर्णाङ्क	    evaluate,
	dict_T	    *selfdict,
	typval_T    *basetv)
{
    partial_T	*pt = रिक्त;
    funcexe_T	funcexe;
    typval_T	functv;
    char_u	*s;
    पूर्णाङ्क		ret;

    // need to copy the funcref so that we can clear rettv
    जइ (evaluate)
    {
	functv = *rettv;
	rettv->v_type = VAR_UNKNOWN;

	// Invoke the function.  Recursive!
	जइ (functv.v_type == VAR_PARTIAL)
	{
	    pt = functv.vval.v_partial;
	    s = partial_name(pt);
	}
	अण्णहा
	{
	    s = functv.vval.v_string;
	    जइ (s == रिक्त || *s == NUL)
	    {
		emsg(_(e_empty_function_name));
		ret = FAIL;
		गच्छ theend;
	    }
	}
    }
    अण्णहा
	s = (char_u *)"";

    CLEAR_FIELD(funcexe);
    funcexe.fe_firstline = curwin->w_cursor.lnum;
    funcexe.fe_lastline = curwin->w_cursor.lnum;
    funcexe.fe_evaluate = evaluate;
    funcexe.fe_partial = pt;
    जइ (pt != रिक्त)
    {
	funcexe.fe_object = pt->pt_obj;
	जइ (funcexe.fe_object != रिक्त)
	    ++funcexe.fe_object->obj_refcount;
    }
    funcexe.fe_selfdict = selfdict;
    funcexe.fe_basetv = basetv;
    जइ (evalarg != रिक्त)
	funcexe.fe_cctx = evalarg->eval_cctx;
    ret = get_func_tv(s, -1, rettv, arg, evalarg, &funcexe);

theend:
    // Clear the funcref afterwards, so that deleting it while
    // evaluating the arguments is possible (see test55).
    जइ (evaluate)
	clear_tv(&functv);

    प्रत्यागम ret;
}

/*
 * Evaluate "->method()".
 * "*arg" points to "method".
 * Returns FAIL or OK. "*arg" is advanced to after the ')'.
 */
    स्थिर पूर्णाङ्क
eval_lambda(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    पूर्णाङ्क		verbose)	// give error messages
{
    पूर्णाङ्क		evaluate = evalarg != रिक्त
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    typval_T	base = *rettv;
    पूर्णाङ्क		ret;

    rettv->v_type = VAR_UNKNOWN;

    जइ (**arg == '{')
    {
	// ->{lambda}()
	ret = get_lambda_tv(arg, rettv, FALSE, evalarg, रिक्त);
    }
    अण्णहा
    {
	// ->(lambda)()
	++*arg;
	ret = eval1(arg, rettv, evalarg);
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	जइ (**arg != ')')
	{
	    emsg(_(e_missing_closing_paren));
	    प्रत्यागम FAIL;
	}
	जइ (rettv->v_type != VAR_STRING && rettv->v_type != VAR_FUNC
					       && rettv->v_type != VAR_PARTIAL)
	{
	    emsg(_(e_string_or_function_required_for_arrow_parens_व्यञ्जकम्));
	    प्रत्यागम FAIL;
	}
	++*arg;
    }
    जइ (ret != OK)
	प्रत्यागम FAIL;

    जइ (**arg != '(')
    {
	जइ (verbose)
	{
	    जइ (*skipwhite(*arg) == '(')
		emsg(_(e_no_white_space_allowed_before_parenthesis));
	    अण्णहा
		semsg(_(e_missing_parenthesis_str), "lambda");
	}
	clear_tv(rettv);
	ret = FAIL;
    }
    अण्णहा
	ret = call_func_rettv(arg, evalarg, rettv, evaluate, रिक्त, &base);

    // Clear the funcref afterwards, so that deleting it while
    // evaluating the arguments is possible (see test55).
    जइ (evaluate)
	clear_tv(&base);

    प्रत्यागम ret;
}

/*
 * Evaluate "->method()".
 * "*arg" points to "method".
 * Returns FAIL or OK. "*arg" is advanced to after the ')'.
 */
    स्थिर पूर्णाङ्क
eval_method(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    पूर्णाङ्क		verbose)	// give error messages
{
    char_u	*name;
    long	len;
    char_u	*alias;
    char_u	*tofree = रिक्त;
    typval_T	base = *rettv;
    पूर्णाङ्क		ret = OK;
    पूर्णाङ्क		evaluate = evalarg != रिक्त
				      && (evalarg->eval_flags & EVAL_EVALUATE);

    rettv->v_type = VAR_UNKNOWN;

    name = *arg;
    len = get_name_len(arg, &alias, evaluate, evaluate);
    जइ (alias != रिक्त)
	name = alias;

    जइ (len <= 0)
    {
	जइ (verbose)
	    emsg(_(e_missing_name_after_method));
	ret = FAIL;
    }
    अण्णहा
    {
	char_u *paren;

	// If there is no "(" immediately following, but there is further on,
	// it can be "import.Func()", "dict.Func()", "list[nr]", etc.
	// Does not handle anything where "(" is part of the expression.
	*arg = skipwhite(*arg);

	जइ (**arg != '(' && alias == रिक्त
				    && (paren = vim_strchr(*arg, '(')) != रिक्त)
	{
	    *arg = name;

	    // Truncate the name at the "(".  Avoid trying to get another line
	    // by making "getline" NULL.
	    *paren = NUL;
	    char_u	*(*getline)(पूर्णाङ्क, शून्य *, पूर्णाङ्क, getline_opt_T) = रिक्त;
	    जइ (evalarg != रिक्त)
	    {
		getline = evalarg->eval_getline;
		evalarg->eval_getline = रिक्त;
	    }

	    char_u *deref = deref_function_name(arg, &tofree, evalarg, verbose);
	    जइ (deref == रिक्त)
	    {
		*arg = name + len;
		ret = FAIL;
	    }
	    अण्णहा
	    {
		name = deref;
		len = (long)STRLEN(name);
	    }

	    *paren = '(';
	    जइ (getline != रिक्त)
		evalarg->eval_getline = getline;
	}

	जइ (ret == OK)
	{
	    *arg = skipwhite(*arg);

	    जइ (**arg != '(')
	    {
		जइ (verbose)
		    semsg(_(e_missing_parenthesis_str), name);
		ret = FAIL;
	    }
	    अण्णहा जइ (VIM_ISWHITE((*arg)[-1]))
	    {
		जइ (verbose)
		    emsg(_(e_no_white_space_allowed_before_parenthesis));
		ret = FAIL;
	    }
	    अण्णहा
		ret = eval_func(arg, evalarg, name, len, rettv,
					  evaluate ? EVAL_EVALUATE : 0, &base);
	}
    }

    // Clear the funcref afterwards, so that deleting it while
    // evaluating the arguments is possible (see test55).
    जइ (evaluate)
	clear_tv(&base);
    vim_free(tofree);

    जइ (alias != रिक्त)
	vim_free(alias);

    प्रत्यागम ret;
}

/*
 * Evaluate an "[expr]" or "[expr:expr]" index.  Also "dict.key".
 * "*arg" points to the '[' or '.'.
 * Returns FAIL or OK. "*arg" is advanced to after the ']'.
 */
    स्थिर पूर्णाङ्क
eval_index(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    पूर्णाङ्क		verbose)	// give error messages
{
    पूर्णाङ्क		evaluate = evalarg != रिक्त
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    पूर्णाङ्क		empty1 = FALSE, empty2 = FALSE;
    typval_T	var1, var2;
    पूर्णाङ्क		range = FALSE;
    char_u	*कुञ्चिका = रिक्त;
    पूर्णाङ्क		keylen = -1;
    पूर्णाङ्क		vim9script = in_vim9script();

    जइ (check_can_index(rettv, evaluate, verbose) == FAIL)
	प्रत्यागम FAIL;

    init_tv(&var1);
    init_tv(&var2);
    जइ (**arg == '.')
    {
	/*
	 * dict.name
	 */
	कुञ्चिका = *arg + 1;
	प्रति (keylen = 0; eval_isdictc(कुञ्चिका[keylen]); ++keylen)
	    ;
	जइ (keylen == 0)
	    प्रत्यागम FAIL;
	जइ (vim9script && कुञ्चिका[keylen] == '<')
	{
	    // skip generic type arguments
	    char_u	*p = &कुञ्चिका[keylen];

	    जइ (skip_generic_func_type_args(&p) == FAIL)
		प्रत्यागम FAIL;
	    keylen = p - कुञ्चिका;
	}
	*arg = कुञ्चिका + keylen;
    }
    अण्णहा
    {
	/*
	 * something[idx]
	 *
	 * Get the (first) variable from inside the [].
	 */
	*arg = skipwhite_and_linebreak(*arg + 1, evalarg);
	जइ (**arg == ':')
	    empty1 = TRUE;
	अण्णहा जइ (eval1(arg, &var1, evalarg) == FAIL)	// recursive!
	    प्रत्यागम FAIL;
	अण्णहा जइ (vim9script && **arg == ':')
	{
	    semsg(_(e_white_space_required_before_and_after_str_at_str),
								    ":", *arg);
	    clear_tv(&var1);
	    प्रत्यागम FAIL;
	}
	अण्णहा जइ (evaluate)
	{
	    पूर्णाङ्क error = FALSE;

	    // allow for indexing with float
	    जइ (vim9script && rettv->v_type == VAR_DICT
						   && var1.v_type == VAR_FLOAT)
	    {
		var1.vval.v_string = typval_tostring(&var1, TRUE);
		var1.v_type = VAR_STRING;
	    }

	    जइ (vim9script && (rettv->v_type == VAR_LIST
						|| rettv->v_type == VAR_TUPLE))
		tv_get_सङ्ख्या_chk(&var1, &error);
	    अण्णहा
		error = tv_get_string_chk(&var1) == रिक्त;
	    जइ (error)
	    {
		// not a number or string
		clear_tv(&var1);
		प्रत्यागम FAIL;
	    }
	}

	/*
	 * Get the second variable from inside the [:].
	 */
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	जइ (**arg == ':')
	{
	    range = TRUE;
	    ++*arg;
	    जइ (vim9script && !IS_WHITE_OR_NUL(**arg) && **arg != ']')
	    {
		semsg(_(e_white_space_required_before_and_after_str_at_str),
								":", *arg - 1);
		जइ (!empty1)
		    clear_tv(&var1);
		प्रत्यागम FAIL;
	    }
	    *arg = skipwhite_and_linebreak(*arg, evalarg);
	    जइ (**arg == ']')
		empty2 = TRUE;
	    अण्णहा जइ (eval1(arg, &var2, evalarg) == FAIL)	// recursive!
	    {
		जइ (!empty1)
		    clear_tv(&var1);
		प्रत्यागम FAIL;
	    }
	    अण्णहा जइ (evaluate && tv_get_string_chk(&var2) == रिक्त)
	    {
		// not a number or string
		जइ (!empty1)
		    clear_tv(&var1);
		clear_tv(&var2);
		प्रत्यागम FAIL;
	    }
	}

	// Check for the ']'.
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	जइ (**arg != ']')
	{
	    जइ (verbose)
		emsg(_(e_missing_closing_square_brace));
	    clear_tv(&var1);
	    जइ (range)
		clear_tv(&var2);
	    प्रत्यागम FAIL;
	}
	*arg = *arg + 1;	// skip over the ']'
    }

    जइ (evaluate)
    {
	पूर्णाङ्क res = eval_index_inner(rettv, range,
		empty1 ? रिक्त : &var1, empty2 ? रिक्त : &var2, FALSE,
		कुञ्चिका, keylen, verbose);

	जइ (!empty1)
	    clear_tv(&var1);
	जइ (range)
	    clear_tv(&var2);
	प्रत्यागम res;
    }
    प्रत्यागम OK;
}

/*
 * Check if "rettv" can have an [index] or [sli:ce]
 */
    पूर्णाङ्क
check_can_index(typval_T *rettv, पूर्णाङ्क evaluate, पूर्णाङ्क verbose)
{
    विकल्प (rettv->v_type)
    {
	पक्ष VAR_FUNC:
	पक्ष VAR_PARTIAL:
	    जइ (verbose)
		emsg(_(e_cannot_index_a_funcref));
	    प्रत्यागम FAIL;
	पक्ष VAR_FLOAT:
	    जइ (verbose)
		emsg(_(e_using_float_as_string));
	    प्रत्यागम FAIL;
	पक्ष VAR_BOOL:
	पक्ष VAR_SPECIAL:
	पक्ष VAR_JOB:
	पक्ष VAR_CHANNEL:
	पक्ष VAR_INSTR:
	पक्ष VAR_OBJECT:
	    जइ (verbose)
		emsg(_(e_cannot_index_special_variable));
	    प्रत्यागम FAIL;
	पक्ष VAR_CLASS:
	पक्ष VAR_TYPEALIAS:
	    जइ (verbose)
		check_typval_is_मानम्(rettv);
	    प्रत्यागम FAIL;
	पक्ष VAR_UNKNOWN:
	पक्ष VAR_ANY:
	पक्ष VAR_VOID:
	    जइ (evaluate)
	    {
		emsg(_(e_cannot_index_special_variable));
		प्रत्यागम FAIL;
	    }
	    // FALLTHROUGH

	पक्ष VAR_STRING:
	पक्ष VAR_LIST:
	पक्ष VAR_TUPLE:
	पक्ष VAR_DICT:
	पक्ष VAR_BLOB:
	    भङ्ग;
	पक्ष VAR_सङ्ख्या:
	    जइ (in_vim9script())
		emsg(_(e_cannot_index_सङ्ख्या));
	    भङ्ग;
    }
    प्रत्यागम OK;
}

/*
 * Apply index or range to "rettv".
 * "var1" is the first index, NULL for [:expr].
 * "var2" is the second index, NULL for [expr] and [expr: ]
 * "exclusive" is TRUE for slice(): second index is exclusive, use character
 * index for string.
 * Alternatively, "key" is not NULL, then key[keylen] is the dict index.
 */
    पूर्णाङ्क
eval_index_inner(
	typval_T    *rettv,
	पूर्णाङ्क	    is_range,
	typval_T    *var1,
	typval_T    *var2,
	पूर्णाङ्क	    exclusive,
	char_u	    *कुञ्चिका,
	पूर्णाङ्क	    keylen,
	पूर्णाङ्क	    verbose)
{
    varnumber_T	    n1, n2 = 0;
    long	    len;

    n1 = 0;
    जइ (var1 != रिक्त && rettv->v_type != VAR_DICT)
	n1 = tv_get_सङ्ख्या(var1);

    जइ (is_range)
    {
	जइ (rettv->v_type == VAR_DICT)
	{
	    जइ (verbose)
		emsg(_(e_cannot_slice_dictionary));
	    प्रत्यागम FAIL;
	}
	जइ (var2 != रिक्त)
	    n2 = tv_get_सङ्ख्या(var2);
	अण्णहा
	    n2 = VARNUM_MAX;
    }

    विकल्प (rettv->v_type)
    {
	पक्ष VAR_UNKNOWN:
	पक्ष VAR_ANY:
	पक्ष VAR_VOID:
	पक्ष VAR_FUNC:
	पक्ष VAR_PARTIAL:
	पक्ष VAR_FLOAT:
	पक्ष VAR_BOOL:
	पक्ष VAR_SPECIAL:
	पक्ष VAR_JOB:
	पक्ष VAR_CHANNEL:
	पक्ष VAR_INSTR:
	पक्ष VAR_CLASS:
	पक्ष VAR_OBJECT:
	पक्ष VAR_TYPEALIAS:
	    भङ्ग; // not evaluating, skipping over subscript

	पक्ष VAR_सङ्ख्या:
	पक्ष VAR_STRING:
	    {
		char_u	*s = tv_get_string(rettv);

		len = (long)STRLEN(s);
		जइ (in_vim9script() || exclusive)
		{
		    जइ (is_range)
			s = string_slice(s, n1, n2, exclusive);
		    अण्णहा
			s = char_from_string(s, n1);
		}
		अण्णहा जइ (is_range)
		{
		    // The resulting variable is a substring.  If the indexes
		    // are out of range the result is empty.
		    जइ (n1 < 0)
		    {
			n1 = len + n1;
			जइ (n1 < 0)
			    n1 = 0;
		    }
		    जइ (n2 < 0)
			n2 = len + n2;
		    अण्णहा जइ (n2 >= len)
			n2 = len;
		    जइ (n1 >= len || n2 < 0 || n1 > n2)
			s = रिक्त;
		    अण्णहा
			s = vim_strnsave(s + n1, n2 - n1 + 1);
		}
		अण्णहा
		{
		    // The resulting variable is a string of a single
		    // character.  If the index is too big or negative the
		    // result is empty.
		    जइ (n1 >= len || n1 < 0)
			s = रिक्त;
		    अण्णहा
			s = vim_strnsave(s + n1, 1);
		}
		clear_tv(rettv);
		rettv->v_type = VAR_STRING;
		rettv->vval.v_string = s;
	    }
	    भङ्ग;

	पक्ष VAR_BLOB:
	    blob_slice_or_index(rettv->vval.v_blob, is_range, n1, n2,
							     exclusive, rettv);
	    भङ्ग;

	पक्ष VAR_LIST:
	    जइ (var1 == रिक्त)
		n1 = 0;
	    जइ (var2 == रिक्त)
		n2 = VARNUM_MAX;
	    जइ (list_slice_or_index(rettv->vval.v_list,
			  is_range, n1, n2, exclusive, rettv, verbose) == FAIL)
		प्रत्यागम FAIL;
	    भङ्ग;

	पक्ष VAR_TUPLE:
	    जइ (var1 == रिक्त)
		n1 = 0;
	    जइ (var2 == रिक्त)
		n2 = VARNUM_MAX;
	    जइ (tuple_slice_or_index(rettv->vval.v_tuple,
			  is_range, n1, n2, exclusive, rettv, verbose) == FAIL)
		प्रत्यागम FAIL;
	    भङ्ग;

	पक्ष VAR_DICT:
	    {
		dictitem_T	*item;
		typval_T	tmp;

		जइ (कुञ्चिका == रिक्त)
		{
		    कुञ्चिका = tv_get_string_chk(var1);
		    जइ (कुञ्चिका == रिक्त)
			प्रत्यागम FAIL;
		}

		item = dict_find(rettv->vval.v_dict, कुञ्चिका, keylen);

		जइ (item == रिक्त)
		{
		    जइ (verbose)
		    {
			जइ (keylen > 0)
			    कुञ्चिका[keylen] = NUL;
			semsg(_(e_कुञ्चिका_not_present_in_dictionary_str), कुञ्चिका);
		    }
		    प्रत्यागम FAIL;
		}

		copy_tv(&item->di_tv, &tmp);
		clear_tv(rettv);
		*rettv = tmp;
	    }
	    भङ्ग;
    }
    प्रत्यागम OK;
}

/*
 * Return the function name of partial "pt".
 */
    char_u *
partial_name(partial_T *pt)
{
    जइ (pt != रिक्त)
    {
	जइ (pt->pt_name != रिक्त)
	    प्रत्यागम pt->pt_name;
	जइ (pt->pt_func != रिक्त)
	    प्रत्यागम pt->pt_func->uf_name;
    }
    प्रत्यागम (char_u *)"";
}

    स्थिर शून्य
partial_free(partial_T *pt)
{
    पूर्णाङ्क i;

    प्रति (i = 0; i < pt->pt_argc; ++i)
	clear_tv(&pt->pt_argv[i]);
    vim_free(pt->pt_argv);
    dict_unref(pt->pt_dict);
    जइ (pt->pt_name != रिक्त)
    {
	func_unref(pt->pt_name);
	vim_free(pt->pt_name);
    }
    अण्णहा
	func_ptr_unref(pt->pt_func);
    जइ (pt->pt_obj != रिक्त)
	object_unref(pt->pt_obj);

    // "out_up" is no longer used, decrement refcount on partial that owns it.
    partial_unref(pt->pt_outer.out_up_partial);

    // Using pt_outer from another partial.
    partial_unref(pt->pt_outer_partial);

    // Decrease the reference count for the context of a closure.  If down
    // to the minimum it may be time to free it.
    जइ (pt->pt_funcstack != रिक्त)
    {
	--pt->pt_funcstack->fs_refcount;
	funcstack_check_refcount(pt->pt_funcstack);
    }
    // Similarly for loop variables.
    प्रति (i = 0; i < MAX_LOOP_DEPTH; ++i)
	जइ (pt->pt_loopvars[i] != रिक्त)
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
    शून्य
partial_unref(partial_T *pt)
{
    जइ (pt == रिक्त)
	प्रत्यागम;

    पूर्णाङ्क	done = FALSE;

    जइ (--pt->pt_refcount <= 0)
	partial_free(pt);

    // If the reference count goes down to one, the funcstack may be the
    // only reference and can be freed if no other partials reference it.
    अण्णहा जइ (pt->pt_refcount == 1)
    {
	// careful: if the funcstack is freed it may contain this partial
	// and it gets freed as well
	जइ (pt->pt_funcstack != रिक्त)
	    done = funcstack_check_refcount(pt->pt_funcstack);

	जइ (!done)
	{
	    पूर्णाङ्क	depth;

	    प्रति (depth = 0; depth < MAX_LOOP_DEPTH; ++depth)
		जइ (pt->pt_loopvars[depth] != रिक्त
			&& loopvars_check_refcount(pt->pt_loopvars[depth]))
		    भङ्ग;
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
    स्थिर char_u *
string_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    पूर्णाङ्क		echo_style,
    पूर्णाङ्क		composite_val)
{
    char_u	*r = रिक्त;

    जइ (echo_style && !composite_val)
    {
	*tofree = रिक्त;
	r = tv->vval.v_string;
	जइ (r == रिक्त)
	    r = (char_u *)"";
    }
    अण्णहा
    {
	*tofree = string_quote(tv->vval.v_string, FALSE);
	r = *tofree;
    }

    प्रत्यागम r;
}

/*
 * Return a textual representation of a function in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "echo_style" is FALSE, put quotes around the function name as
 * "function()", otherwise does not put quotes around function name.
 * May return NULL.
 */
    स्थिर char_u *
func_tv2string(typval_T *tv, char_u **tofree, पूर्णाङ्क echo_style)
{
    char_u	*r = रिक्त;
    char_u	buf[MAX_FUNC_NAME_LEN];

    जइ (echo_style)
    {
	*tofree = रिक्त;

	जइ (tv->vval.v_string == रिक्त)
	    r = (char_u *)"function()";
	अण्णहा
	{
	    r = make_ufunc_name_readable(tv->vval.v_string, buf,
							MAX_FUNC_NAME_LEN);
	    जइ (r == buf)
		r = *tofree = vim_strsave(buf);
	}
    }
    अण्णहा
    {
	char_u *s = रिक्त;

	जइ (tv->vval.v_string != रिक्त)
	    s = make_ufunc_name_readable(tv->vval.v_string, buf,
							MAX_FUNC_NAME_LEN);

	r = *tofree = string_quote(s, TRUE);
    }

    प्रत्यागम r;
}

/*
 * Return a textual representation of the object method in "tv", a VAR_PARTIAL.
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "echo_style" is FALSE, put quotes around the function name as
 * "function()", otherwise does not put quotes around function name.
 * May return NULL.
 */
    स्थिर char_u *
method_tv2string(typval_T *tv, char_u **tofree, पूर्णाङ्क echo_style)
{
    char_u	buf[MAX_FUNC_NAME_LEN];
    partial_T	*pt = tv->vval.v_partial;

    size_t len = vim_snprintf((वर्ण *)buf, परिमाण(buf), "<SNR>%d_%s.%s",
			   pt->pt_func->uf_script_ctx.sc_sid,
			   pt->pt_func->uf_class->class_name.string,
			   pt->pt_func->uf_name);
    जइ (len >= परिमाण(buf))
    {
	जइ (echo_style)
	{
	    *tofree = रिक्त;
	    प्रत्यागम (char_u *)"function()";
	}
	अण्णहा
	    प्रत्यागम *tofree = string_quote((char_u*)"", TRUE);
    }

    प्रत्यागम *tofree = echo_style ? vim_strsave(buf) : string_quote(buf, TRUE);
}

/*
 * Return a textual representation of a partial in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * "numbuf" is used for a number.  May return NULL.
 */
    स्थिर char_u *
partial_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    char_u	*numbuf,
    पूर्णाङ्क		copyID)
{
    char_u	*r = रिक्त;
    partial_T	*pt;
    char_u	*fname;
    garray_T	ga;
    पूर्णाङ्क		i;
    char_u	*tf;

    pt = tv->vval.v_partial;
    fname = string_quote(pt == रिक्त ? रिक्त : partial_name(pt), FALSE);

    ga_init2(&ga, 1, 100);
    GA_CONCAT_LITERAL(&ga, "function(");
    जइ (fname != रिक्त)
    {
	// When using uf_name prepend "g:" for a global function.
	जइ (pt != रिक्त && pt->pt_name == रिक्त && fname[0] == '\''
						&& vim_isupper(fname[1]))
	{
	    GA_CONCAT_LITERAL(&ga, "'g:");
	    ga_concat(&ga, fname + 1);
	}
	अण्णहा
	    ga_concat(&ga, fname);
	vim_free(fname);
    }
    जइ (pt != रिक्त && pt->pt_argc > 0)
    {
	GA_CONCAT_LITERAL(&ga, ", [");
	प्रति (i = 0; i < pt->pt_argc; ++i)
	{
	    जइ (i > 0)
		GA_CONCAT_LITERAL(&ga, ", ");
	    ga_concat(&ga, tv2string(&pt->pt_argv[i], &tf, numbuf, copyID));
	    vim_free(tf);
	}
	GA_CONCAT_LITERAL(&ga, "]");
    }
    जइ (pt != रिक्त && pt->pt_dict != रिक्त)
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

    प्रत्यागम r;
}

/*
 * Return a textual representation of a List in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "copyID" is not zero replace recursive lists with "...".  When
 * "restore_copyID" is FALSE, repeated items in lists are replaced with "...".
 * May return NULL.
 */
    स्थिर char_u *
list_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    पूर्णाङ्क		copyID,
    पूर्णाङ्क		restore_copyID)
{
    char_u	*r = रिक्त;

    जइ (tv->vval.v_list == रिक्त)
    {
	// NULL list is equivalent to empty list.
	*tofree = रिक्त;
	r = (char_u *)"[]";
    }
    अण्णहा जइ (copyID != 0 && tv->vval.v_list->lv_copyID == copyID
	    && tv->vval.v_list->lv_len > 0)
    {
	*tofree = रिक्त;
	r = (char_u *)"[...]";
    }
    अण्णहा
    {
	पूर्णाङ्क old_copyID;
	जइ (restore_copyID)
	    old_copyID = tv->vval.v_list->lv_copyID;

	tv->vval.v_list->lv_copyID = copyID;
	*tofree = list2string(tv, copyID, restore_copyID);
	जइ (restore_copyID)
	    tv->vval.v_list->lv_copyID = old_copyID;
	r = *tofree;
    }

    प्रत्यागम r;
}

/*
 * Return a textual representation of a Tuple in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "copyID" is not zero replace recursive lists with "...".  When
 * "restore_copyID" is FALSE, repeated items in tuples are replaced with "...".
 * May return NULL.
 */
    स्थिर char_u *
tuple_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    पूर्णाङ्क		copyID,
    पूर्णाङ्क		restore_copyID)
{
    tuple_T	*tuple = tv->vval.v_tuple;
    char_u	*r = रिक्त;

    जइ (tuple == रिक्त)
    {
	// NULL tuple is equivalent to an empty tuple.
	*tofree = रिक्त;
	r = (char_u *)"()";
    }
    अण्णहा जइ (copyID != 0 && tuple->tv_copyID == copyID
					&& tuple->tv_वस्तूनि.ga_len > 0)
    {
	*tofree = रिक्त;
	r = (char_u *)"(...)";
    }
    अण्णहा
    {
	पूर्णाङ्क old_copyID;
	जइ (restore_copyID)
	    old_copyID = tuple->tv_copyID;

	tuple->tv_copyID = copyID;
	*tofree = tuple2string(tv, copyID, restore_copyID);
	जइ (restore_copyID)
	    tuple->tv_copyID = old_copyID;
	r = *tofree;
    }

    प्रत्यागम r;
}

/*
 * Return a textual representation of a Dict in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "copyID" is not zero replace recursive dicts with "...".
 * When "restore_copyID" is FALSE, repeated items in the dictionary are
 * replaced with "...".  May return NULL.
 */
    स्थिर char_u *
dict_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    पूर्णाङ्क		copyID,
    पूर्णाङ्क		restore_copyID)
{
    char_u	*r = रिक्त;

    जइ (tv->vval.v_dict == रिक्त)
    {
	// NULL dict is equivalent to empty dict.
	*tofree = रिक्त;
	r = (char_u *)"{}";
    }
    अण्णहा जइ (copyID != 0 && tv->vval.v_dict->dv_copyID == copyID
	    && tv->vval.v_dict->dv_hashtab.ht_used != 0)
    {
	*tofree = रिक्त;
	r = (char_u *)"{...}";
    }
    अण्णहा
    {
	पूर्णाङ्क old_copyID;
	जइ (restore_copyID)
	    old_copyID = tv->vval.v_dict->dv_copyID;

	tv->vval.v_dict->dv_copyID = copyID;
	*tofree = dict2string(tv, copyID, restore_copyID);
	जइ (restore_copyID)
	    tv->vval.v_dict->dv_copyID = old_copyID;
	r = *tofree;
    }

    प्रत्यागम r;
}

/*
 * Return a textual representation of a job or a channel in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * "numbuf" is used for a number.
 * When "composite_val" is FALSE, put quotes around strings as "string()",
 * otherwise does not put quotes around strings.
 * May return NULL.
 */
    स्थिर char_u *
jobchan_tv2string(
    typval_T	*tv UNUSED,
    char_u	**tofree UNUSED,
    char_u	*numbuf UNUSED,
    पूर्णाङ्क		composite_val UNUSED)
{
    char_u	*r = रिक्त;

#ifdef FEAT_JOB_CHANNEL
    *tofree = रिक्त;

    जइ (tv->v_type == VAR_JOB)
	r = job_to_string_buf(tv, numbuf);
    अण्णहा
	r = channel_to_string_buf(tv, numbuf);

    जइ (composite_val)
    {
	*tofree = string_quote(r, FALSE);
	r = *tofree;
    }
#endif

    प्रत्यागम r;
}

/*
 * Return a textual representation of a class in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * May return NULL.
 */
    स्थिर char_u *
class_tv2string(typval_T *tv, char_u **tofree)
{
    char_u	*r = रिक्त;
    size_t	rsize;
    class_T	*cl = tv->vval.v_class;
    string_T	class_name = {(char_u *)"[unknown]", 9};
    string_T	s = {(char_u *)"class", 5};

    जइ (cl != रिक्त)
    {
	class_name.string = cl->class_name.string;
	class_name.length = cl->class_name.length;
	जइ (IS_INTERFACE(cl))
	    STR_LITERAL_SET(s, "interface");
	अण्णहा जइ (IS_ENUM(cl))
	    STR_LITERAL_SET(s, "enum");
    }

    rsize = s.length + 1 + class_name.length + 1;
    r = *tofree = alloc(rsize);
    जइ (r != रिक्त)
	vim_snprintf((वर्ण *)r, rsize, "%s %s", s.string, (वर्ण *)class_name.string);

    प्रत्यागम r;
}

/*
 * Return a textual representation of an Object in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "copyID" is not zero replace recursive object with "...".
 * When "restore_copyID" is FALSE, repeated items in the object are
 * replaced with "...".  May return NULL.
 */
    स्थिर char_u *
object_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    पूर्णाङ्क		copyID,
    पूर्णाङ्क		restore_copyID,
    char_u	*numbuf,
    पूर्णाङ्क		echo_style,
    पूर्णाङ्क		composite_val)
{
    char_u	*r = रिक्त;

    object_T	*obj = tv->vval.v_object;
    जइ (obj == रिक्त || obj->obj_class == रिक्त)
    {
	*tofree = रिक्त;
	r = (char_u *)"object of [unknown]";
    }
    अण्णहा जइ (copyID != 0 && obj->obj_copyID == copyID
	    && obj->obj_class->class_obj_member_गणनम् != 0)
    {
	size_t n = 25 + obj->obj_class->class_name.length;
	r = alloc(n);
	जइ (r != रिक्त)
	    (शून्य)vim_snprintf((वर्ण *)r, n, "object of %s {...}",
						obj->obj_class->class_name.string);
	*tofree = r;
    }
    अण्णहा
    {
	पूर्णाङ्क old_copyID;
	जइ (restore_copyID)
	    old_copyID = obj->obj_copyID;

	obj->obj_copyID = copyID;
	*tofree = object2string(obj, numbuf, copyID, echo_style,
				restore_copyID, composite_val);
	जइ (restore_copyID)
	    obj->obj_copyID = old_copyID;
	r = *tofree;
    }

    प्रत्यागम r;
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
    पूर्णाङ्क		copyID,
    पूर्णाङ्क		echo_style,
    पूर्णाङ्क		restore_copyID,
    पूर्णाङ्क		composite_val)
{
    स्थिर पूर्णाङ्क	recurse = 0;
    char_u	*r = रिक्त;

    जइ (recurse >= DICT_MAXNEST)
    {
	जइ (!did_echo_string_emsg)
	{
	    // Only give this message once for a recursive call to avoid
	    // flooding the user with errors.  And stop iterating over lists
	    // and dicts and objects.
	    did_echo_string_emsg = TRUE;
	    emsg(_(e_variable_nested_too_deep_for_displaying));
	}
	*tofree = रिक्त;
	प्रत्यागम (char_u *)"{E724}";
    }
    ++recurse;

    विकल्प (tv->v_type)
    {
	पक्ष VAR_STRING:
	    r = string_tv2string(tv, tofree, echo_style, composite_val);
	    भङ्ग;

	पक्ष VAR_FUNC:
	    r = func_tv2string(tv, tofree, echo_style);
	    भङ्ग;

	पक्ष VAR_PARTIAL:
	    जइ (tv->vval.v_partial == रिक्त
		    || tv->vval.v_partial->pt_obj == रिक्त)
		r = partial_tv2string(tv, tofree, numbuf, copyID);
	    अण्णहा
		r = method_tv2string(tv, tofree, echo_style);
	    भङ्ग;

	पक्ष VAR_BLOB:
	    r = blob2string(tv->vval.v_blob, tofree, numbuf);
	    भङ्ग;

	पक्ष VAR_LIST:
	    r = list_tv2string(tv, tofree, copyID, restore_copyID);
	    भङ्ग;

	पक्ष VAR_TUPLE:
	    r = tuple_tv2string(tv, tofree, copyID, restore_copyID);
	    भङ्ग;

	पक्ष VAR_DICT:
	    r = dict_tv2string(tv, tofree, copyID, restore_copyID);
	    भङ्ग;

	पक्ष VAR_सङ्ख्या:
	पक्ष VAR_UNKNOWN:
	पक्ष VAR_ANY:
	पक्ष VAR_VOID:
	    *tofree = रिक्त;
	    r = tv_get_string_buf(tv, numbuf);
	    भङ्ग;

	पक्ष VAR_JOB:
	पक्ष VAR_CHANNEL:
	    r = jobchan_tv2string(tv, tofree, numbuf, composite_val);
	    भङ्ग;

	पक्ष VAR_INSTR:
	    *tofree = रिक्त;
	    r = (char_u *)"instructions";
	    भङ्ग;

	पक्ष VAR_CLASS:
	    r = class_tv2string(tv, tofree);
	    भङ्ग;

	पक्ष VAR_OBJECT:
	    r = object_tv2string(tv, tofree, copyID, restore_copyID,
					 numbuf, echo_style, composite_val);
	    भङ्ग;

	पक्ष VAR_FLOAT:
	    *tofree = रिक्त;
	    vim_snprintf((वर्ण *)numbuf, NUMBUFLEN, "%g", tv->vval.v_float);
	    r = numbuf;
	    भङ्ग;

	पक्ष VAR_BOOL:
	पक्ष VAR_SPECIAL:
	    *tofree = रिक्त;
	    r = (char_u *)get_var_special_name(tv->vval.v_सङ्ख्या);
	    भङ्ग;

	पक्ष VAR_TYPEALIAS:
	    *tofree = vim_strsave(tv->vval.v_typealias->ta_name);
	    r = *tofree;
	    जइ (r == रिक्त)
		r = (char_u *)"";
	    भङ्ग;
    }

    जइ (--recurse == 0)
	did_echo_string_emsg = FALSE;
    प्रत्यागम r;
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
    पूर्णाङ्क		copyID)
{
    प्रत्यागम echo_string_core(tv, tofree, numbuf, copyID, TRUE, FALSE, FALSE);
}

/*
 * Convert the specified byte index of line 'lnum' in buffer 'buf' to a
 * character index.  Works only for loaded buffers. Returns -1 on failure.
 * The index of the first byte and the first character is zero.
 */
    पूर्णाङ्क
buf_byteidx_to_charidx(buf_T *buf, पूर्णाङ्क lnum, पूर्णाङ्क byteidx)
{
    char_u	*str;
    char_u	*t;
    पूर्णाङ्क		गणनम्;

    जइ (buf == रिक्त || buf->b_ml.ml_mfp == रिक्त)
	प्रत्यागम -1;

    जइ (lnum > buf->b_ml.ml_पङ्क्तिः_गणनम्)
	lnum = buf->b_ml.ml_पङ्क्तिः_गणनम्;

    str = ml_get_buf(buf, lnum, FALSE);
    जइ (str == रिक्त)
	प्रत्यागम -1;

    जइ (*str == NUL)
	प्रत्यागम 0;

    // count the number of characters
    t = str;
    प्रति (गणनम् = 0; *t != NUL && t <= str + byteidx; गणनम्++)
	t += mb_ptr2len(t);

    // In insert mode, when the cursor is at the end of a non-empty line,
    // byteidx points to the NUL character immediately past the end of the
    // string. In this case, add one to the character count.
    जइ (*t == NUL && byteidx != 0 && t == str + byteidx)
	गणनम्++;

    प्रत्यागम गणनम् - 1;
}

/*
 * Convert the specified character index of line 'lnum' in buffer 'buf' to a
 * byte index.  Works only for loaded buffers. Returns -1 on failure.
 * The index of the first byte and the first character is zero.
 */
    पूर्णाङ्क
buf_charidx_to_byteidx(buf_T *buf, पूर्णाङ्क lnum, पूर्णाङ्क charidx)
{
    char_u	*str;
    char_u	*t;

    जइ (buf == रिक्त || buf->b_ml.ml_mfp == रिक्त)
	प्रत्यागम -1;

    जइ (lnum > buf->b_ml.ml_पङ्क्तिः_गणनम्)
	lnum = buf->b_ml.ml_पङ्क्तिः_गणनम्;

    str = ml_get_buf(buf, lnum, FALSE);
    जइ (str == रिक्त)
	प्रत्यागम -1;

    // Convert the character offset to a byte offset
    t = str;
    जाव (*t != NUL && --charidx > 0)
	t += mb_ptr2len(t);

    प्रत्यागम t - str;
}

/*
 * Translate a String variable into a position.
 * Returns NULL when there is an error.
 */
    pos_T *
var2fpos(
    typval_T	*varp,
    पूर्णाङ्क		dollar_lnum,	// TRUE when $ is last line
    पूर्णाङ्क		*fnum,		// set to fnum for '0, 'A, etc.
    पूर्णाङ्क		charcol)	// return character column
{
    char_u		*name;
    स्थिर pos_T	pos;
    pos_T		*pp;
    पूर्णाङ्क			error = FALSE;

    // Argument can be [lnum, col, coladd].
    जइ (varp->v_type == VAR_LIST)
    {
	list_T		*l;
	पूर्णाङ्क		len;
	listitem_T	*li;

	l = varp->vval.v_list;
	जइ (l == रिक्त)
	    प्रत्यागम रिक्त;

	// Get the line number
	pos.lnum = list_find_nr(l, 0L, &error);
	जइ (error || pos.lnum <= 0 || pos.lnum > curbuf->b_ml.ml_पङ्क्तिः_गणनम्)
	    प्रत्यागम रिक्त;	// invalid line number
	जइ (charcol)
	    len = (long)mb_charlen(ml_get(pos.lnum));
	अण्णहा
	    len = (long)ml_get_len(pos.lnum);

	// Get the column number
	// We accept "$" for the column number: last column.
	li = list_find(l, 1L);
	जइ (li != रिक्त && li->li_tv.v_type == VAR_STRING
		&& li->li_tv.vval.v_string != रिक्त
		&& STRCMP(li->li_tv.vval.v_string, "$") == 0)
	{
	    pos.col = len + 1;
	}
	अण्णहा
	{
	    pos.col = list_find_nr(l, 1L, &error);
	    जइ (error)
		प्रत्यागम रिक्त;
	}

	// Accept a position up to the NUL after the line.
	जइ (pos.col == 0 || (पूर्णाङ्क)pos.col > len + 1)
	    प्रत्यागम रिक्त;	// invalid column number
	--pos.col;

	// Get the virtual offset.  Defaults to zero.
	pos.coladd = list_find_nr(l, 2L, &error);
	जइ (error)
	    pos.coladd = 0;

	प्रत्यागम &pos;
    }

    जइ (in_vim9script() && check_for_string_arg(varp, 0) == FAIL)
	प्रत्यागम रिक्त;

    name = tv_get_string_chk(varp);
    जइ (name == रिक्त)
	प्रत्यागम रिक्त;

    error = TRUE;
    pos.lnum = 0;
    जइ (name[0] == '.')
    {
	जइ (!in_vim9script() || name[1] == NUL)
	{
	    error = FALSE;
	    // cursor
	    pos = curwin->w_cursor;
	}
    }
    अण्णहा जइ (name[0] == 'v' && name[1] == NUL)
    {
	// Visual start
	जइ (VIsual_active)
	    pos = VIsual;
	अण्णहा
	    pos = curwin->w_cursor;
    }
    अण्णहा जइ (name[0] == '\'')
    {
	जइ (!in_vim9script() || (name[1] != NUL && name[2] == NUL))
	{
	    error = FALSE;
	    // mark
	    pp = getmark_buf_fnum(curbuf, name[1], FALSE, fnum);
	    जइ (pp == रिक्त || pp == (pos_T *)-1 || pp->lnum <= 0)
		प्रत्यागम रिक्त;
	    pos = *pp;
	}
    }
    जइ (pos.lnum != 0)
    {
	जइ (charcol)
	    pos.col = buf_byteidx_to_charidx(curbuf, pos.lnum, pos.col);
	प्रत्यागम &pos;
    }

    pos.coladd = 0;

    जइ (name[0] == 'w' && dollar_lnum)
    {
	// the "w_valid" flags are not reset when moving the cursor, but they
	// do matter for update_topline() and validate_botline().
	check_cursor_moved(curwin);

	pos.col = 0;
	जइ (name[1] == '0')		// "w0": first visible line
	{
#ifdef FEAT_TERMINAL
	    जइ (bt_terminal(curwin->w_buffer)
				    && curwin->w_buffer->b_term != रिक्त
				    && !term_in_normal_mode(curwin->w_buffer))
		may_move_terminal_to_buffer(curwin->w_buffer->b_term, TRUE);
#endif
	    update_topline();
	    // In silent Ex mode topline is zero, but that's not a valid line
	    // number; use one instead.
	    pos.lnum = curwin->w_topline > 0 ? curwin->w_topline : 1;
	    प्रत्यागम &pos;
	}
	अण्णहा जइ (name[1] == '$')	// "w$": last visible line
	{
#ifdef FEAT_TERMINAL
	    जइ (bt_terminal(curwin->w_buffer)
				    && curwin->w_buffer->b_term != रिक्त
				    && !term_in_normal_mode(curwin->w_buffer))
		may_move_terminal_to_buffer(curwin->w_buffer->b_term, TRUE);
#endif
	    validate_botline();
	    // In silent Ex mode botline is zero, return zero then.
	    pos.lnum = curwin->w_botline > 0 ? curwin->w_botline - 1 : 0;
	    प्रत्यागम &pos;
	}
    }
    अण्णहा जइ (name[0] == '$')		// last column or line
    {
	जइ (dollar_lnum)
	{
	    pos.lnum = curbuf->b_ml.ml_पङ्क्तिः_गणनम्;
	    pos.col = 0;
	}
	अण्णहा
	{
	    pos.lnum = curwin->w_cursor.lnum;
	    जइ (charcol)
		pos.col = (colnr_T)mb_charlen(ml_get_curline());
	    अण्णहा
		pos.col = ml_get_curline_len();
	}
	प्रत्यागम &pos;
    }
    जइ (in_vim9script() && error)
	semsg(_(e_invalid_मानम्_for_पङ्क्तिः_सङ्ख्या_str), name);
    प्रत्यागम रिक्त;
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
    पूर्णाङ्क
list2fpos(
    typval_T	*arg,
    pos_T	*posp,
    पूर्णाङ्क		*fnump,
    colnr_T	*curswantp,
    पूर्णाङ्क		charcol)
{
    list_T	*l = arg->vval.v_list;
    long	i = 0;
    long	n;

    // List must be: [fnum, lnum, col, coladd, curswant], where "fnum" is only
    // there when "fnump" isn't NULL; "coladd" and "curswant" are optional.
    जइ (arg->v_type != VAR_LIST
	    || l == रिक्त
	    || l->lv_len < (fnump == रिक्त ? 2 : 3)
	    || l->lv_len > (fnump == रिक्त ? 4 : 5))
	प्रत्यागम FAIL;

    जइ (fnump != रिक्त)
    {
	n = list_find_nr(l, i++, रिक्त);	// fnum
	जइ (n < 0)
	    प्रत्यागम FAIL;
	जइ (n == 0)
	    n = curbuf->b_fnum;		// current buffer
	*fnump = n;
    }

    n = list_find_nr(l, i++, रिक्त);	// lnum
    जइ (n < 0)
	प्रत्यागम FAIL;
    posp->lnum = n;

    n = list_find_nr(l, i++, रिक्त);	// col
    जइ (n < 0)
	प्रत्यागम FAIL;
    // If character position is specified, then convert to byte position
    // If the line number is zero use the cursor line.
    जइ (charcol)
    {
	buf_T	*buf;

	// Get the text for the specified line in a loaded buffer
	buf = buflist_findnr(fnump == रिक्त ? curbuf->b_fnum : *fnump);
	जइ (buf == रिक्त || buf->b_ml.ml_mfp == रिक्त)
	    प्रत्यागम FAIL;

	n = buf_charidx_to_byteidx(buf,
		  posp->lnum == 0 ? curwin->w_cursor.lnum : posp->lnum, n) + 1;
    }
    posp->col = n;

    n = list_find_nr(l, i, रिक्त);	// off
    जइ (n < 0)
	posp->coladd = 0;
    अण्णहा
	posp->coladd = n;

    जइ (curswantp != रिक्त)
	*curswantp = list_find_nr(l, i + 1, रिक्त);  // curswant

    प्रत्यागम OK;
}

/*
 * Get the length of an environment variable name.
 * Advance "arg" to the first character after the name.
 * Return 0 for error.
 */
    पूर्णाङ्क
get_env_len(char_u **arg)
{
    char_u	*p;
    पूर्णाङ्क		len;

    प्रति (p = *arg; vim_isIDc(*p); ++p)
	;
    जइ (p == *arg)	    // no name found
	प्रत्यागम 0;

    len = (पूर्णाङ्क)(p - *arg);
    *arg = p;
    प्रत्यागम len;
}

/*
 * Get the length of the name of a function or internal variable.
 * "arg" is advanced to after the name.
 * Return 0 if something is wrong.
 */
    पूर्णाङ्क
get_id_len(char_u **arg)
{
    char_u	*p;
    पूर्णाङ्क		len;

    // Find the end of the name.
    प्रति (p = *arg; eval_isnamec(*p); ++p)
    {
	जइ (*p == ':')
	{
	    // "s:" is start of "s:var", but "n:" is not and can be used in
	    // slice "[n:]".  Also "xx:" is not a namespace.
	    len = (पूर्णाङ्क)(p - *arg);
	    जइ ((len == 1 && vim_strchr(NAMESPACE_CHAR, **arg) == रिक्त)
		    || len > 1)
		भङ्ग;
	}
    }
    जइ (p == *arg)	    // no name found
	प्रत्यागम 0;

    len = (पूर्णाङ्क)(p - *arg);
    *arg = p;

    प्रत्यागम len;
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
    पूर्णाङ्क
get_name_len(
    char_u	**arg,
    char_u	**alias,
    पूर्णाङ्क		evaluate,
    पूर्णाङ्क		verbose)
{
    पूर्णाङ्क		len;
    char_u	*p;
    char_u	*व्यञ्जकम्_start;
    char_u	*व्यञ्जकम्_end;

    *alias = रिक्त;  // default to no alias

    जइ ((*arg)[0] == K_SPECIAL && (*arg)[1] == KS_EXTRA
						  && (*arg)[2] == (पूर्णाङ्क)KE_SNR)
    {
	// hard coded <SNR>, already translated
	*arg += 3;
	प्रत्यागम get_id_len(arg) + 3;
    }
    len = eval_fname_script(*arg);
    जइ (len > 0)
    {
	// literal "<SID>", "s:" or "<SNR>"
	*arg += len;
    }

    /*
     * Find the end of the name; check for {} construction.
     */
    p = find_name_end(*arg, &व्यञ्जकम्_start, &व्यञ्जकम्_end,
					       len > 0 ? 0 : FNE_CHECK_START);
    जइ (व्यञ्जकम्_start != रिक्त)
    {
	char_u	*temp_string;

	जइ (!evaluate)
	{
	    len += (पूर्णाङ्क)(p - *arg);
	    *arg = skipwhite(p);
	    प्रत्यागम len;
	}

	/*
	 * Include any <SID> etc in the expanded string:
	 * Thus the -len here.
	 */
	temp_string = make_expanded_name(*arg - len, व्यञ्जकम्_start, व्यञ्जकम्_end, p);
	जइ (temp_string == रिक्त)
	    प्रत्यागम -1;
	*alias = temp_string;
	*arg = skipwhite(p);
	प्रत्यागम (पूर्णाङ्क)STRLEN(temp_string);
    }

    len += get_id_len(arg);
    // Only give an error when there is something, otherwise it will be
    // reported at a higher level.
    जइ (len == 0 && verbose && **arg != NUL)
	semsg(_(e_invalid_expression_str), *arg);

    प्रत्यागम len;
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
    char_u	**व्यञ्जकम्_start,
    char_u	**व्यञ्जकम्_end,
    पूर्णाङ्क		flags)
{
    पूर्णाङ्क		mb_nest = 0;
    पूर्णाङ्क		br_nest = 0;
    char_u	*p;
    पूर्णाङ्क		len;
    पूर्णाङ्क		allow_curly = !in_vim9script();

    जइ (व्यञ्जकम्_start != रिक्त)
    {
	*व्यञ्जकम्_start = रिक्त;
	*व्यञ्जकम्_end = रिक्त;
    }

    // Quick check for valid starting character.
    जइ ((flags & FNE_CHECK_START) && !eval_isnamec1(*arg)
					      && (*arg != '{' || !allow_curly))
	प्रत्यागम arg;

    प्रति (p = arg; *p != NUL
		    && (eval_isnamec(*p)
			|| (*p == '{' && allow_curly)
			|| ((flags & FNE_INCL_BR) && (*p == '['
					 || (*p == '.' && eval_isdictc(p[1]))))
			|| mb_nest != 0
			|| br_nest != 0); MB_PTR_ADV(p))
    {
	जइ (*p == '\'')
	{
	    // skip over 'string' to avoid counting [ and ] inside it.
	    प्रति (p = p + 1; *p != NUL && *p != '\''; MB_PTR_ADV(p))
		;
	    जइ (*p == NUL)
		भङ्ग;
	}
	अण्णहा जइ (*p == '"')
	{
	    // skip over "str\"ing" to avoid counting [ and ] inside it.
	    प्रति (p = p + 1; *p != NUL && *p != '"'; MB_PTR_ADV(p))
		जइ (*p == '\\' && p[1] != NUL)
		    ++p;
	    जइ (*p == NUL)
		भङ्ग;
	}
	अण्णहा जइ (br_nest == 0 && mb_nest == 0 && *p == ':')
	{
	    // "s:" is start of "s:var", but "n:" is not and can be used in
	    // slice "[n:]".  Also "xx:" is not a namespace. But {ns}: is.
	    len = (पूर्णाङ्क)(p - arg);
	    जइ ((len == 1 && vim_strchr(NAMESPACE_CHAR, *arg) == रिक्त)
		    || (len > 1 && p[-1] != '}'))
		भङ्ग;
	}

	जइ (mb_nest == 0)
	{
	    जइ (*p == '[')
		++br_nest;
	    अण्णहा जइ (*p == ']')
		--br_nest;
	}

	जइ (br_nest == 0 && allow_curly)
	{
	    जइ (*p == '{')
	    {
		mb_nest++;
		जइ (व्यञ्जकम्_start != रिक्त && *व्यञ्जकम्_start == रिक्त)
		    *व्यञ्जकम्_start = p;
	    }
	    अण्णहा जइ (*p == '}')
	    {
		mb_nest--;
		जइ (व्यञ्जकम्_start != रिक्त && mb_nest == 0 && *व्यञ्जकम्_end == रिक्त)
		    *व्यञ्जकम्_end = p;
	    }
	}
    }

    प्रत्यागम p;
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
    स्थिर char_u *
make_expanded_name(
    char_u	*in_start,
    char_u	*व्यञ्जकम्_start,
    char_u	*व्यञ्जकम्_end,
    char_u	*in_end)
{
    char_u	c1;
    char_u	*retval = रिक्त;
    char_u	*temp_result;

    जइ (व्यञ्जकम्_end == रिक्त || in_end == रिक्त)
	प्रत्यागम रिक्त;
    *व्यञ्जकम्_start	= NUL;
    *व्यञ्जकम्_end = NUL;
    c1 = *in_end;
    *in_end = NUL;

    temp_result = eval_to_string(व्यञ्जकम्_start + 1, FALSE, FALSE);
    जइ (temp_result != रिक्त)
    {
	size_t	retvalsize = (size_t)(व्यञ्जकम्_start - in_start)
				+ STRLEN(temp_result)
				+ (size_t)(in_end - व्यञ्जकम्_end) + 1;

	retval = alloc(retvalsize);
	जइ (retval != रिक्त)
	    vim_snprintf((वर्ण *)retval, retvalsize, "%s%s%s",
				in_start, temp_result, व्यञ्जकम्_end + 1);
    }
    vim_free(temp_result);

    *in_end = c1;		// put char back for error messages
    *व्यञ्जकम्_start = '{';
    *व्यञ्जकम्_end = '}';

    जइ (retval != रिक्त)
    {
	temp_result = find_name_end(retval, &व्यञ्जकम्_start, &व्यञ्जकम्_end, 0);
	जइ (व्यञ्जकम्_start != रिक्त)
	{
	    // Further expansion!
	    temp_result = make_expanded_name(retval, व्यञ्जकम्_start,
						       व्यञ्जकम्_end, temp_result);
	    vim_free(retval);
	    retval = temp_result;
	}
    }

    प्रत्यागम retval;
}

/*
 * Return TRUE if character "c" can be used in a variable or function name.
 * Does not include '{' or '}' for magic braces.
 */
    पूर्णाङ्क
eval_isnamec(पूर्णाङ्क c)
{
    प्रत्यागम ASCII_ISALNUM(c) || c == '_' || c == ':' || c == AUTOLOAD_CHAR;
}

/*
 * Return TRUE if character "c" can be used as the first character in a
 * variable or function name (excluding '{' and '}').
 */
    पूर्णाङ्क
eval_isnamec1(पूर्णाङ्क c)
{
    प्रत्यागम ASCII_ISALPHA(c) || c == '_';
}

/*
 * Return TRUE if character "c" can be used as the first character of a
 * dictionary key.
 */
    पूर्णाङ्क
eval_isdictc(पूर्णाङ्क c)
{
    प्रत्यागम ASCII_ISALNUM(c) || c == '_';
}

/*
 * Return true when "p" points to "->" followed by a method name or a lambda.
 * Only called for Vim9 script.
 */
    स्थिर bool
method_call_follows(char_u *p)
{
    जइ (p[0] != '-' || p[1] != '>')
	प्रत्यागम false;

    char_u	*after = skipwhite(p + 2);

    प्रत्यागम ASCII_ISALPHA(*after) || *after == '(';
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
    पूर्णाङ्क
handle_subscript(
    char_u	**arg,
    char_u	*name_start,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    पूर्णाङ्क		verbose)	// give error messages
{
    पूर्णाङ्क		evaluate = evalarg != रिक्त
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    पूर्णाङ्क		ret = OK;
    dict_T	*selfdict = रिक्त;
    पूर्णाङ्क		check_white = TRUE;
    पूर्णाङ्क		getnext;
    char_u	*p;

    जाव (ret == OK)
    {
	// When at the end of the line and ".name" or a method call follows in
	// the next line then consume the line break.
	p = eval_next_non_blank(*arg, evalarg, &getnext);
	जइ (getnext
	    && ((*p == '.'
		    && ((rettv->v_type == VAR_DICT && eval_isdictc(p[1]))
			|| rettv->v_type == VAR_CLASS
			|| rettv->v_type == VAR_OBJECT))
		|| method_call_follows(p)))
	{
	    *arg = eval_next_पङ्क्तिः(*arg, evalarg);
	    p = *arg;
	    check_white = FALSE;
	}

	जइ (rettv->v_type == VAR_ANY)
	{
	    char_u	*exp_name;
	    पूर्णाङ्क		cc;
	    पूर्णाङ्क		idx;
	    ufunc_T	*ufunc;
	    type_T	*type;

	    // Found script from "import {name} as name", script item name must
	    // follow.  "rettv->vval.v_number" has the script ID.
	    जइ (**arg != '.')
	    {
		जइ (verbose)
		    semsg(_(e_expected_dot_after_name_str),
					name_start != रिक्त ? name_start: *arg);
		ret = FAIL;
		भङ्ग;
	    }
	    ++*arg;
	    जइ (IS_WHITE_OR_NUL(**arg))
	    {
		जइ (verbose)
		    emsg(_(e_no_white_space_allowed_after_dot));
		ret = FAIL;
		भङ्ग;
	    }

	    // isolate the name
	    exp_name = *arg;
	    जाव (eval_isnamec(**arg))
		++*arg;
	    cc = **arg;
	    **arg = NUL;

	    idx = find_exported(rettv->vval.v_सङ्ख्या, exp_name, &ufunc, &type,
		       evalarg == रिक्त ? रिक्त : evalarg->eval_cctx,
		       evalarg == रिक्त ? रिक्त : evalarg->eval_cstack, verbose);
	    **arg = cc;

	    जइ (idx < 0 && ufunc == रिक्त)
	    {
		ret = FAIL;
		भङ्ग;
	    }
	    जइ (idx >= 0)
	    {
		scriptitem_T    *si = SCRIPT_ITEM(rettv->vval.v_सङ्ख्या);
		svar_T		*sv = ((svar_T *)si->sn_var_vals.ga_data) + idx;

		copy_tv(sv->sv_tv, rettv);
	    }
	    अण्णहा
	    {
		rettv->v_type = VAR_FUNC;
		जइ (**arg == '<')
		{
		    char_u *s = get_generic_func_name(ufunc, arg);
		    जइ (s != रिक्त)
			rettv->vval.v_string = s;
		    अण्णहा
			ret = FAIL;
		}
		अण्णहा
		{
		    rettv->vval.v_string =
			vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
		}
	    }
	    अनुवर्त;
	}

	जइ ((**arg == '(' && (!evaluate || rettv->v_type == VAR_FUNC
			    || rettv->v_type == VAR_PARTIAL))
		    && (!check_white || !VIM_ISWHITE(*(*arg - 1))))
	{
	    ret = call_func_rettv(arg, evalarg, rettv, evaluate,
							       selfdict, रिक्त);

	    // Stop the expression evaluation when immediately aborting on
	    // error, or when an interrupt occurred or an exception was thrown
	    // but not caught.
	    जइ (aborting())
	    {
		जइ (ret == OK)
		    clear_tv(rettv);
		ret = FAIL;
	    }
	    dict_unref(selfdict);
	    selfdict = रिक्त;
	}
	अण्णहा जइ (p[0] == '-' && p[1] == '>')
	{
	    जइ (in_vim9script())
		*arg = skipwhite(p + 2);
	    अण्णहा
		*arg = p + 2;
	    जइ (ret == OK && evaluate && rettv->v_type == VAR_VOID)
	    {
		जइ (verbose)
		    emsg(_(e_cannot_use_void_मानम्));
		ret = FAIL;
	    }
	    अण्णहा जइ (VIM_ISWHITE(**arg))
	    {
		emsg(_(e_no_white_space_allowed_before_parenthesis));
		ret = FAIL;
	    }
	    अण्णहा जइ ((**arg == '{' && !in_vim9script()) || **arg == '(')
		// expr->{lambda}() or expr->(lambda)()
		ret = eval_lambda(arg, rettv, evalarg, verbose);
	    अण्णहा
		// expr->name()
		ret = eval_method(arg, rettv, evalarg, verbose);
	}
	// "." is ".name" lookup when we found a dict or when evaluating and
	// scriptversion is at least 2, where string concatenation is "..".
	अण्णहा जइ (**arg == '['
		|| (**arg == '.' && (rettv->v_type == VAR_DICT
			|| (!evaluate
			    && (*arg)[1] != '.'
			    && !in_old_script(2)))))
	{
	    dict_unref(selfdict);
	    जइ (rettv->v_type == VAR_DICT)
	    {
		selfdict = rettv->vval.v_dict;
		जइ (selfdict != रिक्त)
		    ++selfdict->dv_refcount;
	    }
	    अण्णहा
		selfdict = रिक्त;
	    जइ (eval_index(arg, rettv, evalarg, verbose) == FAIL)
	    {
		clear_tv(rettv);
		ret = FAIL;
	    }
	}
	अण्णहा जइ (**arg == '.' && (rettv->v_type == VAR_CLASS
					       || rettv->v_type == VAR_OBJECT))
	{
	    // class member: SomeClass.varname
	    // class method: SomeClass.SomeMethod()
	    // class constructor: SomeClass.new()
	    // object member: someObject.varname
	    // object method: someObject.SomeMethod()
	    जइ (class_object_index(arg, rettv, evalarg, verbose) == FAIL)
	    {
		clear_tv(rettv);
		ret = FAIL;
	    }
	}
	अण्णहा
	    भङ्ग;
    }

    // Turn "dict.Func" into a partial for "Func" bound to "dict".
    // Don't do this when "Func" is already a partial that was bound
    // explicitly (pt_auto is FALSE).
    जइ (selfdict != रिक्त
	    && (rettv->v_type == VAR_FUNC
		|| (rettv->v_type == VAR_PARTIAL
		    && (rettv->vval.v_partial->pt_auto
			|| rettv->vval.v_partial->pt_dict == रिक्त))))
	selfdict = make_partial(selfdict, rettv);

    dict_unref(selfdict);
    प्रत्यागम ret;
}

/*
 * Make a copy of an item.
 * Lists and Dictionaries are also copied.  A deep copy if "deep" is set.
 * "top" is TRUE for the toplevel of copy().
 * For deepcopy() "copyID" is zero for a full copy or the ID for when a
 * reference to an already copied list/dict can be used.
 * Returns FAIL or OK.
 */
    पूर्णाङ्क
item_copy(
    typval_T	*from,
    typval_T	*to,
    पूर्णाङ्क		deep,
    पूर्णाङ्क		शिखरम्,
    पूर्णाङ्क		copyID)
{
    स्थिर पूर्णाङ्क	recurse = 0;
    पूर्णाङ्क		ret = OK;

    जइ (recurse >= DICT_MAXNEST)
    {
	emsg(_(e_variable_nested_too_deep_for_making_copy));
	प्रत्यागम FAIL;
    }
    ++recurse;

    विकल्प (from->v_type)
    {
	पक्ष VAR_सङ्ख्या:
	पक्ष VAR_FLOAT:
	पक्ष VAR_STRING:
	पक्ष VAR_FUNC:
	पक्ष VAR_PARTIAL:
	पक्ष VAR_BOOL:
	पक्ष VAR_SPECIAL:
	पक्ष VAR_JOB:
	पक्ष VAR_CHANNEL:
	पक्ष VAR_INSTR:
	पक्ष VAR_CLASS:
	पक्ष VAR_OBJECT:
	पक्ष VAR_TYPEALIAS:
	    copy_tv(from, to);
	    भङ्ग;
	पक्ष VAR_LIST:
	    to->v_type = VAR_LIST;
	    to->v_lock = 0;
	    जइ (from->vval.v_list == रिक्त)
		to->vval.v_list = रिक्त;
	    अण्णहा जइ (copyID != 0 && from->vval.v_list->lv_copyID == copyID)
	    {
		// use the copy made earlier
		to->vval.v_list = from->vval.v_list->lv_copylist;
		++to->vval.v_list->lv_refcount;
	    }
	    अण्णहा
	    {
		to->vval.v_list = list_copy(from->vval.v_list,
							    deep, शिखरम्, copyID);
		जइ (to->vval.v_list == रिक्त)
		    ret = FAIL;
	    }
	    भङ्ग;
	पक्ष VAR_TUPLE:
	    to->v_type = VAR_TUPLE;
	    to->v_lock = 0;
	    जइ (from->vval.v_tuple == रिक्त)
		to->vval.v_tuple = रिक्त;
	    अण्णहा जइ (copyID != 0 && from->vval.v_tuple->tv_copyID == copyID)
	    {
		// use the copy made earlier
		to->vval.v_tuple = from->vval.v_tuple->tv_copytuple;
		++to->vval.v_tuple->tv_refcount;
	    }
	    अण्णहा
	    {
		to->vval.v_tuple = tuple_copy(from->vval.v_tuple,
							    deep, शिखरम्, copyID);
		जइ (to->vval.v_tuple == रिक्त)
		    ret = FAIL;
	    }
	    भङ्ग;
	पक्ष VAR_BLOB:
	    to->v_type = VAR_BLOB;
	    to->v_lock = 0;
	    जइ (from->vval.v_blob == रिक्त)
		to->vval.v_blob = रिक्त;
	    अण्णहा
	    {
		to->vval.v_blob = blob_copy(from->vval.v_blob);
		जइ (to->vval.v_blob == रिक्त)
		    ret = FAIL;
	    }
	    भङ्ग;
	पक्ष VAR_DICT:
	    to->v_type = VAR_DICT;
	    to->v_lock = 0;
	    जइ (from->vval.v_dict == रिक्त)
		to->vval.v_dict = रिक्त;
	    अण्णहा जइ (copyID != 0 && from->vval.v_dict->dv_copyID == copyID)
	    {
		// use the copy made earlier
		to->vval.v_dict = from->vval.v_dict->dv_copydict;
		++to->vval.v_dict->dv_refcount;
	    }
	    अण्णहा
	    {
		to->vval.v_dict = dict_copy(from->vval.v_dict,
							    deep, शिखरम्, copyID);
		जइ (to->vval.v_dict == रिक्त)
		    ret = FAIL;
	    }
	    भङ्ग;
	पक्ष VAR_UNKNOWN:
	पक्ष VAR_ANY:
	पक्ष VAR_VOID:
	    internal_error_no_abort("item_copy(UNKNOWN)");
	    ret = FAIL;
    }
    --recurse;
    प्रत्यागम ret;
}

    शून्य
echo_one(typval_T *rettv, पूर्णाङ्क with_space, पूर्णाङ्क *atstart, पूर्णाङ्क *needclr)
{
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    char_u	*p = echo_string(rettv, &tofree, numbuf, get_copyID());

    जइ (*atstart)
    {
	*atstart = FALSE;
	// Call msg_start() after eval1(), evaluating the expression
	// may cause a message to appear.
	जइ (with_space)
	{
	    // Mark the saved text as finishing the line, so that what
	    // follows is displayed on a new line when scrolling back
	    // at the more prompt.
	    सन्देशः_sb_eol();
	    सन्देशः_start();
	}
    }
    अण्णहा जइ (with_space)
	सन्देशः_puts_attr(" ", echo_attr);

    जइ (p != रिक्त)
	प्रति ( ; *p != NUL && !got_int; ++p)
	{
	    जइ (*p == '\n' || *p == '\r' || *p == TAB)
	    {
		जइ (*p != TAB && *needclr)
		{
		    // remove any text still there from the command
		    सन्देशः_clr_eos();
		    *needclr = FALSE;
		}
		सन्देशः_putchar_attr(*p, echo_attr);
	    }
	    अण्णहा
	    {
		जइ (has_mbyte)
		{
		    पूर्णाङ्क i = (*mb_ptr2len)(p);

		    (शून्य)सन्देशः_outtrans_len_attr(p, i, echo_attr);
		    p += i - 1;
		}
		अण्णहा
		    (शून्य)सन्देशः_outtrans_len_attr(p, 1, echo_attr);
	    }
	}
    vim_free(tofree);
}

/*
 * ":echo expr1 ..."	print each argument separated with a space, add a
 *			newline at the end.
 * ":echon expr1 ..."	print each argument plain.
 */
    शून्य
ex_echo(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    typval_T	rettv;
    char_u	*arg_start;
    पूर्णाङ्क		needclr = TRUE;
    पूर्णाङ्क		atstart = TRUE;
    पूर्णाङ्क		did_emsg_before = did_emsg;
    पूर्णाङ्क		called_emsg_before = called_emsg;
    evalarg_T	evalarg;

    fill_evalarg_from_eap(&evalarg, eap, eap->skip);

    जइ (eap->skip)
	++emsg_skip;
    जाव ((!ends_excmd2(eap->cmd, arg) || *arg == '"') && !got_int)
    {
	// If eval1() causes an error message the text from the command may
	// still need to be cleared. E.g., "echo 22,44".
	need_clr_eos = needclr;

	arg_start = arg;
	जइ (eval1(&arg, &rettv, &evalarg) == FAIL)
	{
	    /*
	     * Report the invalid expression unless the expression evaluation
	     * has been cancelled due to an aborting error, an interrupt, or an
	     * exception.
	     */
	    जइ (!aborting() && did_emsg == did_emsg_before
					  && called_emsg == called_emsg_before)
		semsg(_(e_invalid_expression_str), arg_start);
	    need_clr_eos = FALSE;
	    भङ्ग;
	}
	need_clr_eos = FALSE;

	जइ (!eap->skip)
	{
	    जइ (rettv.v_type == VAR_VOID)
	    {
		semsg(_(e_expression_does_not_result_in_मानम्_str), arg_start);
		भङ्ग;
	    }
	    echo_one(&rettv, eap->cmdidx == CMD_echo, &atstart, &needclr);
	}

	clear_tv(&rettv);
	arg = skipwhite(arg);
    }
    set_nextcmd(eap, arg);
    clear_evalarg(&evalarg, eap);

    जइ (eap->skip)
	--emsg_skip;
    अण्णहा
    {
	// remove text that may still be there from the command
	जइ (needclr)
	    सन्देशः_clr_eos();
	जइ (eap->cmdidx == CMD_echo)
	    सन्देशः_end();
    }
}

/*
 * ":echohl {name}".
 */
    शून्य
ex_echohl(exarg_T *eap)
{
    echo_attr = syn_name2attr(eap->arg);
}

/*
 * Returns the :echo attribute
 */
    पूर्णाङ्क
get_echo_attr(शून्य)
{
    प्रत्यागम echo_attr;
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
    शून्य
ex_execute(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    typval_T	rettv;
    पूर्णाङ्क		ret = OK;
    char_u	*p;
    garray_T	ga;
    पूर्णाङ्क		len;
    long	start_lnum = SOURCING_LNUM;

    ga_init2(&ga, 1, 80);

    जइ (eap->skip)
	++emsg_skip;
    जाव (!ends_excmd2(eap->cmd, arg) || *arg == '"')
    {
	ret = eval1_emsg(&arg, &rettv, eap);
	जइ (ret == FAIL)
	    भङ्ग;

	जइ (!eap->skip)
	{
	    char_u   buf[NUMBUFLEN];

	    जइ (eap->cmdidx == CMD_execute)
	    {
		जइ (rettv.v_type == VAR_CHANNEL || rettv.v_type == VAR_JOB)
		{
		    semsg(_(e_using_invalid_मानम्_as_string_str),
						  vartype_name(rettv.v_type));
		    p = रिक्त;
		}
		अण्णहा
		    p = tv_get_string_buf(&rettv, buf);
	    }
	    अण्णहा
		p = tv_stringify(&rettv, buf);
	    जइ (p == रिक्त)
	    {
		clear_tv(&rettv);
		ret = FAIL;
		भङ्ग;
	    }
	    len = (पूर्णाङ्क)STRLEN(p);
	    जइ (ga_grow(&ga, len + 2) == FAIL)
	    {
		clear_tv(&rettv);
		ret = FAIL;
		भङ्ग;
	    }
	    जइ (ga.ga_len)
		((char_u *)(ga.ga_data))[ga.ga_len++] = ' ';
	    STRCPY((char_u *)(ga.ga_data) + ga.ga_len, p);
	    ga.ga_len += len;
	}

	clear_tv(&rettv);
	arg = skipwhite(arg);
    }

    जइ (ret != FAIL && ga.ga_data != रिक्त)
    {
	// use the first line of continuation lines for messages
	SOURCING_LNUM = start_lnum;

	जइ (eap->cmdidx == CMD_echomsg
		|| eap->cmdidx == CMD_echowindow
		|| eap->cmdidx == CMD_echoerr)
	{
	    // Mark the already saved text as finishing the line, so that what
	    // follows is displayed on a new line when scrolling back at the
	    // more prompt.
	    सन्देशः_sb_eol();
	}

	जइ (eap->cmdidx == CMD_echomsg)
	{
	    सन्देशः_attr(ga.ga_data, echo_attr);
	    out_flush();
	}
	अण्णहा जइ (eap->cmdidx == CMD_echowindow)
	{
#ifdef HAS_MESSAGE_WINDOW
	    start_echowindow(eap->addr_गणनम् > 0 ? eap->line2 : 0);
#endif
	    सन्देशः_attr(ga.ga_data, echo_attr);
#ifdef HAS_MESSAGE_WINDOW
	    end_echowindow();
#endif
	}
	अण्णहा जइ (eap->cmdidx == CMD_echoconsole)
	{
	    ui_write(ga.ga_data, (पूर्णाङ्क)STRLEN(ga.ga_data), TRUE);
	    ui_write((char_u *)"\r\n", 2, TRUE);
	}
	अण्णहा जइ (eap->cmdidx == CMD_echoerr)
	{
	    पूर्णाङ्क		save_did_emsg = did_emsg;

	    // We don't want to abort following commands, restore did_emsg.
	    emsg(ga.ga_data);
	    जइ (!force_abort)
		did_emsg = save_did_emsg;
	}
	अण्णहा जइ (eap->cmdidx == CMD_execute)
	{
	    पूर्णाङ्क save_sticky_cmdmod_flags = sticky_cmdmod_flags;

	    // "legacy exe cmd" and "vim9cmd exe cmd" applies to "cmd".
	    sticky_cmdmod_flags = cmdmod.cmod_flags
						& (CMOD_LEGACY | CMOD_VIM9CMD);
	    do_cmdline((char_u *)ga.ga_data,
		       eap->ea_getline, eap->cookie, DOCMD_NOWAIT|DOCMD_VERBOSE);
	    sticky_cmdmod_flags = save_sticky_cmdmod_flags;
	}
    }

    ga_clear(&ga);

    जइ (eap->skip)
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
find_option_end(char_u **arg, पूर्णाङ्क *scope)
{
    char_u	*p = *arg;

    ++p;
    जइ (*p == 'g' && p[1] == ':')
    {
	*scope = OPT_GLOBAL;
	p += 2;
    }
    अण्णहा जइ (*p == 'l' && p[1] == ':')
    {
	*scope = OPT_LOCAL;
	p += 2;
    }
    अण्णहा
	*scope = 0;

    जइ (!ASCII_ISALPHA(*p))
	प्रत्यागम रिक्त;
    *arg = p;

    जइ (p[0] == 't' && p[1] == '_' && p[2] != NUL && p[3] != NUL)
	p += 4;	    // termcap option
    अण्णहा
	जाव (ASCII_ISALPHA(*p))
	    ++p;
    प्रत्यागम p;
}

/*
 * Display script name where an item was last set.
 * Should only be invoked when 'verbose' is non-zero.
 */
    शून्य
last_set_सन्देशः(sctx_T script_ctx)
{
    char_u *p;

    जइ (script_ctx.sc_sid == 0)
	प्रत्यागम;

    p = home_replace_save(रिक्त, get_scriptname(script_ctx.sc_sid));
    जइ (p == रिक्त)
	प्रत्यागम;

    verbose_enter();
    सन्देशः_puts(_("\n\tLast set from "));
    सन्देशः_puts((वर्ण *)p);
    जइ (script_ctx.sc_lnum > 0)
    {
	सन्देशः_puts(_(पङ्क्तिः_सन्देशः));
	सन्देशः_outnum((long)script_ctx.sc_lnum);
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
    typval_T	*व्यञ्जकम्,
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
    जइ (regmatch.regprog != रिक्त)
    {
	char_u	*tail = str;
	char_u	*end = str + len;
	पूर्णाङ्क	do_all = (flags[0] == 'g');
	पूर्णाङ्क	sublen;
	पूर्णाङ्क	i;
	char_u	*zero_width = रिक्त;

	जाव (vim_regexec_nl(&regmatch, str, (colnr_T)(tail - str)))
	{
	    // Skip empty match except for first match.
	    जइ (regmatch.startp[0] == regmatch.endp[0])
	    {
		जइ (zero_width == regmatch.startp[0])
		{
		    // avoid getting stuck on a match with an empty string
		    i = mb_ptr2len(tail);
		    mch_memmove((char_u *)ga.ga_data + ga.ga_len, tail,
								   (size_t)i);
		    ga.ga_len += i;
		    tail += i;
		    अनुवर्त;
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
	    sublen = vim_regsub(&regmatch, sub, व्यञ्जकम्, tail, 0, REGSUB_MAGIC);
	    जइ (sublen <= 0)
	    {
		ga_clear(&ga);
		भङ्ग;
	    }
	    जइ (ga_grow(&ga, (पूर्णाङ्क)((end - tail) + sublen -
			    (regmatch.endp[0] - regmatch.startp[0]))) == FAIL)
	    {
		ga_clear(&ga);
		भङ्ग;
	    }

	    // copy the text up to where the match is
	    i = (पूर्णाङ्क)(regmatch.startp[0] - tail);
	    mch_memmove((char_u *)ga.ga_data + ga.ga_len, tail, (size_t)i);
	    // add the substituted text
	    (शून्य)vim_regsub(&regmatch, sub, व्यञ्जकम्,
				  (char_u *)ga.ga_data + ga.ga_len + i, sublen,
				  REGSUB_COPY | REGSUB_MAGIC);
	    ga.ga_len += i + sublen - 1;
	    tail = regmatch.endp[0];
	    जइ (*tail == NUL)
		भङ्ग;
	    जइ (!do_all)
		भङ्ग;
	}

	जइ (ga.ga_data != रिक्त)
	{
	    STRCPY((वर्ण *)ga.ga_data + ga.ga_len, tail);
	    ga.ga_len += (पूर्णाङ्क)(end - tail);
	}

	vim_regfree(regmatch.regprog);
    }

    जइ (ga.ga_data != रिक्त)
    {
	str = (char_u *)ga.ga_data;
	len = (size_t)ga.ga_len;
    }
    ret = vim_strnsave(str, len);
    ga_clear(&ga);
    जइ (p_cpo == empty_option)
	p_cpo = save_cpo;
    अण्णहा
    {
	// Darn, evaluating {sub} expression or {expr} changed the value.
	// If it's still empty it was changed and restored, need to restore in
	// the complicated way.
	जइ (*p_cpo == NUL)
	    set_option_मानम्_give_err((char_u *)"cpo", 0L, save_cpo, 0);
	free_string_option(save_cpo);
    }

    जइ (ret_len != रिक्त)
	*ret_len = len;

    प्रत्यागम ret;
}
