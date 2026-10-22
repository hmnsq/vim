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

#شامل "vim.h"

#اگر defined(FEAT_EVAL)

#ifdef VMS
# include <float.h>
#endif

#define NAMESPACE_CHAR	(char_u *)"abglstvw"

ایستا صحیح eval0_simple_funccal(char_u *arg, typval_T *rettv, exarg_T *eap, evalarg_T *evalarg);
ایستا صحیح eval2(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
ایستا صحیح eval3(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
ایستا صحیح eval4(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
ایستا صحیح eval5(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
ایستا صحیح eval6(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
ایستا صحیح eval7(char_u **arg, typval_T *rettv, evalarg_T *evalarg, صحیح want_string);
ایستا صحیح eval8(char_u **arg, typval_T *rettv, evalarg_T *evalarg, صحیح want_string);
ایستا صحیح eval9(char_u **arg, typval_T *rettv, evalarg_T *evalarg, صحیح want_string);
ایستا صحیح eval9_leader(typval_T *rettv, صحیح numeric_only, char_u *start_leader, char_u **end_leaderp);

ایستا char_u *make_expanded_name(char_u *in_start, char_u *عبارت_start, char_u *عبارت_end, char_u *in_end);

/*
 * Return "n1" divided by "n2", taking care of dividing by zero.
 * If "failed" is not NULL set it to TRUE when dividing by zero fails.
 */
	varnumber_T
رقم_divide(varnumber_T n1, varnumber_T n2, صحیح *failed)
{
    varnumber_T	result;

    اگر (n2 == 0)
    {
	اگر (in_vim9script())
	{
	    emsg(_(e_divide_by_zero));
	    اگر (failed != پوچ)
		*failed = TRUE;
	}
	اگر (n1 == 0)
	    result = VARNUM_MIN; // similar to NaN
	وگرنه اگر (n1 < 0)
	    result = -VARNUM_MAX;
	وگرنه
	    result = VARNUM_MAX;
    }
    وگرنه اگر (n1 == VARNUM_MIN && n2 == -1)
    {
	// specific case: trying to do VARNUM_MIN / -1 results in a positive
	// number that doesn't fit in varnumber_T and causes an FPE
	result = VARNUM_MAX;
    }
    وگرنه
	result = n1 / n2;

    بازگشت result;
}

/*
 * Return "n1" modulus "n2", taking care of dividing by zero.
 * If "failed" is not NULL set it to TRUE when dividing by zero fails.
 */
	varnumber_T
رقم_modulus(varnumber_T n1, varnumber_T n2, صحیح *failed)
{
    اگر (n2 == 0 && in_vim9script())
    {
	emsg(_(e_divide_by_zero));
	اگر (failed != پوچ)
	    *failed = TRUE;
    }
    بازگشت (n2 == 0) ? 0 : (n1 % n2);
}

/*
 * Initialize the global and v: variables.
 */
    تهی
eval_init(تهی)
{
    evalvars_init();
    func_init();
}

#اگر defined(EXITFREE)
    تهی
eval_clear(تهی)
{
    evalvars_clear();
    free_scriptnames();  // must come after evalvars_clear().
    free_locales();

    // autoloaded script names
    free_autoload_scriptnames();

    // unreferenced lists, tuples and dicts
    (تهی)garbage_collect(FALSE);

    // functions not garbage collected
    free_all_functions();
}
#endif

    تهی
fill_evalarg_from_eap(evalarg_T *evalarg, exarg_T *eap, صحیح skip)
{
    init_evalarg(evalarg);
    evalarg->eval_flags = skip ? 0 : EVAL_EVALUATE;

    اگر (eap == پوچ)
	بازگشت;

    evalarg->eval_cstack = eap->cstack;
    اگر (sourcing_a_script(eap) || eap->ea_getline == get_list_سطر)
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
    صحیح
eval_to_bool(
    char_u	*arg,
    صحیح		*error,
    exarg_T	*eap,
    صحیح		skip,	    // only parse, don't execute
    صحیح		use_simple_function)
{
    typval_T	tv;
    varnumber_T	retval = FALSE;
    evalarg_T	evalarg;
    صحیح		r;

    fill_evalarg_from_eap(&evalarg, eap, skip);

    اگر (skip)
	++emsg_skip;
    اگر (use_simple_function)
	r = eval0_simple_funccal(arg, &tv, eap, &evalarg);
    وگرنه
	r = eval0(arg, &tv, eap, &evalarg);
    اگر (r == FAIL)
	*error = TRUE;
    وگرنه
    {
	*error = FALSE;
	اگر (!skip)
	{
	    اگر (in_vim9script())
		retval = tv_get_bool_chk(&tv, error);
	    وگرنه
		retval = (tv_get_عدد_chk(&tv, error) != 0);
	    clear_tv(&tv);
	}
    }
    اگر (skip)
	--emsg_skip;
    clear_evalarg(&evalarg, eap);

    بازگشت (صحیح)retval;
}

/*
 * Call eval1() and give an error message if not done at a lower level.
 */
    ایستا صحیح
eval1_emsg(char_u **arg, typval_T *rettv, exarg_T *eap)
{
    char_u	*start = *arg;
    صحیح		ret;
    صحیح		did_emsg_before = did_emsg;
    صحیح		called_emsg_before = called_emsg;
    evalarg_T	evalarg;

    fill_evalarg_from_eap(&evalarg, eap, eap != پوچ && eap->skip);

    ret = eval1(arg, rettv, &evalarg);
    اگر (ret == FAIL)
    {
	// Report the invalid expression unless the expression evaluation has
	// been cancelled due to an aborting error, an interrupt, or an
	// exception, or we already gave a more specific error.
	// Also check called_emsg for when using assert_fails().
	اگر (!aborting() && did_emsg == did_emsg_before
					  && called_emsg == called_emsg_before)
	    semsg(_(e_invalid_expression_str), start);
    }
    clear_evalarg(&evalarg, eap);
    بازگشت ret;
}

/*
 * Return whether a typval is a valid expression to pass to eval_expr_typval()
 * or eval_expr_to_bool().  An empty string returns FALSE;
 */
    صحیح
eval_عبارت_valid_arg(typval_T *tv)
{
    بازگشت tv->v_type != VAR_UNKNOWN
	    && (tv->v_type != VAR_STRING
		  || (tv->vval.v_string != پوچ && *tv->vval.v_string != NUL));
}

/*
 * When calling eval_expr_typval() many times we only need one funccall_T.
 * Returns NULL when no funccall_T is to be used.
 * When returning non-NULL remove_funccal() must be called later.
 */
    funccall_T *
eval_عبارت_get_funccal(typval_T *عبارت, typval_T *rettv)
{
    اگر (عبارت->v_type != VAR_PARTIAL)
	بازگشت پوچ;

    partial_T *partial = عبارت->vval.v_partial;
    اگر (partial == پوچ)
	بازگشت پوچ;
    اگر (partial->pt_func == پوچ
	    || partial->pt_func->uf_def_status == UF_NOT_COMPILED)
	بازگشت پوچ;

    بازگشت create_funccal(partial->pt_func, rettv);
}

/*
 * Evaluate a partial.
 * Pass arguments "argv[argc]".
 * "fc_arg" is from eval_expr_get_funccal() or NULL;
 * Return the result in "rettv" and OK or FAIL.
 */
    ایستا صحیح
eval_عبارت_partial(
    typval_T	*عبارت,
    typval_T	*argv,
    صحیح		argc,
    funccall_T	*fc_arg,
    typval_T	*rettv)
{
    partial_T	*partial = عبارت->vval.v_partial;

    اگر (partial == پوچ)
	بازگشت FAIL;

    اگر (partial->pt_func != پوچ
			&& partial->pt_func->uf_def_status != UF_NOT_COMPILED)
    {
	funccall_T	*fc = fc_arg != پوچ ? fc_arg
				: create_funccal(partial->pt_func, rettv);
	صحیح		r;

	اگر (fc == پوچ)
	    بازگشت FAIL;

	// Shortcut to call a compiled function with minimal overhead.
	اگر (partial->pt_obj != پوچ)
	    partial->pt_obj->obj_refcount++;
	r = call_def_function(partial->pt_func, argc, argv, DEF_USE_PT_ARGV,
					partial, partial->pt_obj, fc, rettv);
	اگر (fc_arg == پوچ)
	    remove_funccal();
	اگر (r == FAIL)
	    بازگشت FAIL;
    }
    وگرنه
    {
	char_u		*s = partial_name(partial);
	funcexe_T	funcexe;

	اگر (s == پوچ || *s == NUL)
	    بازگشت FAIL;

	CLEAR_FIELD(funcexe);
	funcexe.fe_evaluate = TRUE;
	funcexe.fe_partial = partial;
	اگر (call_func(s, -1, rettv, argc, argv, &funcexe) == FAIL)
	    بازگشت FAIL;
    }

    بازگشت OK;
}

/*
 * Evaluate an expression which is a function.
 * Pass arguments "argv[argc]".
 * Return the result in "rettv" and OK or FAIL.
 */
    ایستا صحیح
eval_عبارت_func(
    typval_T	*عبارت,
    typval_T	*argv,
    صحیح		argc,
    typval_T	*rettv)
{
    funcexe_T	funcexe;
    char_u	buf[NUMBUFLEN];
    char_u	*s;

    اگر (عبارت->v_type == VAR_FUNC)
	s = عبارت->vval.v_string;
    وگرنه
	s = tv_get_string_buf_chk_strict(عبارت, buf, in_vim9script());
    اگر (s == پوچ || *s == NUL)
	بازگشت FAIL;

    CLEAR_FIELD(funcexe);
    funcexe.fe_evaluate = TRUE;
    اگر (call_func(s, -1, rettv, argc, argv, &funcexe) == FAIL)
	بازگشت FAIL;

    بازگشت OK;
}

/*
 * Evaluate an expression, which is a string.
 * Return the result in "rettv" and OK or FAIL.
 */
    ایستا صحیح
eval_عبارت_string(
    typval_T	*عبارت,
    typval_T	*rettv)
{
    char_u	*s;
    char_u	buf[NUMBUFLEN];

    s = tv_get_string_buf_chk_strict(عبارت, buf, in_vim9script());
    اگر (s == پوچ)
	بازگشت FAIL;

    s = skipwhite(s);
    اگر (eval1_emsg(&s, rettv, پوچ) == FAIL)
	بازگشت FAIL;

    اگر (*skipwhite(s) != NUL)  // check for trailing chars after expr
    {
	clear_tv(rettv);
	semsg(_(e_invalid_expression_str), s);
	بازگشت FAIL;
    }

    بازگشت OK;
}

/*
 * Evaluate an expression, which can be a function, partial or string.
 * Pass arguments "argv[argc]".
 * If "want_func" is TRUE treat a string as a function name, not an expression.
 * "fc_arg" is from eval_expr_get_funccal() or NULL;
 * Return the result in "rettv" and OK or FAIL.
 */
    صحیح
eval_عبارت_typval(
	typval_T    *عبارت,
	صحیح	    want_func,
	typval_T    *argv,
	صحیح	    argc,
	funccall_T  *fc_arg,
	typval_T    *rettv)
{
    اگر (عبارت->v_type == VAR_PARTIAL)
	بازگشت eval_عبارت_partial(عبارت, argv, argc, fc_arg, rettv);
    اگر (عبارت->v_type == VAR_INSTR)
	بازگشت exe_typval_instr(عبارت, rettv);
    اگر (عبارت->v_type == VAR_FUNC || want_func)
	بازگشت eval_عبارت_func(عبارت, argv, argc, rettv);

    بازگشت eval_عبارت_string(عبارت, rettv);
}

/*
 * Like eval_to_bool() but using a typval_T instead of a string.
 * Works for string, funcref and partial.
 */
    صحیح
eval_عبارت_to_bool(typval_T *عبارت, صحیح *error)
{
    typval_T	rettv;
    صحیح		res;

    اگر (eval_عبارت_typval(عبارت, FALSE, پوچ, 0, پوچ, &rettv) == FAIL)
    {
	*error = TRUE;
	بازگشت FALSE;
    }
    res = (tv_get_bool_chk(&rettv, error) != 0);
    clear_tv(&rettv);
    بازگشت res;
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
    صحیح		skip)	    // only parse, don't execute
{
    typval_T	tv;
    char_u	*retval;
    evalarg_T	evalarg;

    fill_evalarg_from_eap(&evalarg, eap, skip);
    اگر (skip)
	++emsg_skip;
    اگر (eval0(arg, &tv, eap, &evalarg) == FAIL || skip)
	retval = پوچ;
    وگرنه
    {
	retval = vim_strsave(tv_get_string(&tv));
	clear_tv(&tv);
    }
    اگر (skip)
	--emsg_skip;
    clear_evalarg(&evalarg, eap);

    بازگشت retval;
}

/*
 * Initialize "evalarg" for use.
 */
    تهی
init_evalarg(evalarg_T *evalarg)
{
    CLEAR_POINTER(evalarg);
    ga_init2(&evalarg->eval_tofree_ga, اندازه(char_u *), 20);
}

/*
 * If "evalarg->eval_tofree" is not NULL free it later.
 * Caller is expected to overwrite "evalarg->eval_tofree" next.
 */
    ایستا تهی
free_eval_tofree_later(evalarg_T *evalarg)
{
    اگر (evalarg->eval_tofree == پوچ)
	بازگشت;

    اگر (ga_grow(&evalarg->eval_tofree_ga, 1) == OK)
	((char_u **)evalarg->eval_tofree_ga.ga_data)
	    [evalarg->eval_tofree_ga.ga_len++]
	    = evalarg->eval_tofree;
    وگرنه
	vim_free(evalarg->eval_tofree);
}

/*
 * After using "evalarg" filled from "eap": free the memory.
 */
    تهی
clear_evalarg(evalarg_T *evalarg, exarg_T *eap)
{
    اگر (evalarg == پوچ)
	بازگشت;

    garray_T *etga = &evalarg->eval_tofree_ga;

    اگر (evalarg->eval_tofree != پوچ || evalarg->eval_using_cmdline)
    {
	اگر (eap != پوچ)
	{
	    // We may need to keep the original command line, e.g. for
	    // ":let" it has the variable names.  But we may also need
	    // the new one, "nextcmd" points into it.  Keep both.
	    vim_free(eap->cmdline_tofree);
	    eap->cmdline_tofree = *eap->cmdlinep;

	    اگر (evalarg->eval_using_cmdline && etga->ga_len > 0)
	    {
		// "nextcmd" points into the last line in eval_tofree_ga,
		// need to keep it around.
		--etga->ga_len;
		*eap->cmdlinep = ((char_u **)etga->ga_data)[etga->ga_len];
		vim_free(evalarg->eval_tofree);
	    }
	    وگرنه
		*eap->cmdlinep = evalarg->eval_tofree;
	}
	وگرنه
	    vim_free(evalarg->eval_tofree);
	evalarg->eval_tofree = پوچ;
    }

    ga_clear_strings(etga);
    VIM_CLEAR(evalarg->eval_tofree_lambda);
}

/*
 * Skip over an expression at "*pp".
 * Return FAIL for an error, OK otherwise.
 */
    صحیح
skip_عبارت(char_u **pp, evalarg_T *evalarg)
{
    typval_T	rettv;

    *pp = skipwhite(*pp);
    بازگشت eval1(pp, &rettv, evalarg);
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
    صحیح
skip_عبارت_concatenate(
	char_u	    **arg,
	char_u	    **start,
	char_u	    **end,
	evalarg_T   *evalarg)
{
    typval_T	rettv;
    صحیح		res;
    صحیح		vim9script = in_vim9script();
    garray_T    *gap = evalarg == پوچ ? پوچ : &evalarg->eval_ga;
    garray_T    *freegap = evalarg == پوچ ? پوچ : &evalarg->eval_freega;
    صحیح		save_flags = evalarg == پوچ ? 0 : evalarg->eval_flags;
    صحیح		evaluate = evalarg == پوچ
			       ? FALSE : (evalarg->eval_flags & EVAL_EVALUATE);

    اگر (vim9script && evaluate
	       && (evalarg->eval_cookie != پوچ || evalarg->eval_cctx != پوچ))
    {
	ga_init2(gap, اندازه(char_u *), 10);
	// leave room for "start"
	اگر (ga_grow(gap, 1) == OK)
	    ++gap->ga_len;
	ga_init2(freegap, اندازه(char_u *), 10);
    }
    *start = *arg;

    // Don't evaluate the expression.
    اگر (evalarg != پوچ)
	evalarg->eval_flags &= ~EVAL_EVALUATE;
    *arg = skipwhite(*arg);
    res = eval1(arg, &rettv, evalarg);
    *end = *arg;
    اگر (evalarg != پوچ)
	evalarg->eval_flags = save_flags;

    اگر (vim9script && evaluate
	    && (evalarg->eval_cookie != پوچ || evalarg->eval_cctx != پوچ))
    {
	اگر (evalarg->eval_ga.ga_len == 1)
	{
	    // just the one line, no need to concatenate
	    ga_clear(gap);
	    gap->ga_itemsize = 0;
	}
	وگرنه
	{
	    char_u	    *p;
	    size_t	    endoff = STRLEN(*arg);

	    // Line breaks encountered, concatenate all the lines.
	    *((char_u **)gap->ga_data) = *start;
	    p = ga_concat_strings(gap, " ");

	    // free the lines only when using getsourceline()
	    اگر (evalarg->eval_cookie != پوچ)
	    {
		// Do not free the first line, the caller can still use it.
		*((char_u **)gap->ga_data) = پوچ;
		// Do not free the last line, "arg" points into it, free it
		// later.  Also free "eval_tofree" later if needed.
		free_eval_tofree_later(evalarg);
		evalarg->eval_tofree =
				    ((char_u **)gap->ga_data)[gap->ga_len - 1];
		((char_u **)gap->ga_data)[gap->ga_len - 1] = پوچ;
		ga_clear_strings(gap);
		ga_clear(freegap);
	    }
	    وگرنه
	    {
		ga_clear(gap);

		// free lines that were explicitly marked for freeing
		ga_clear_strings(freegap);
	    }

	    gap->ga_itemsize = 0;
	    اگر (p == پوچ)
		بازگشت FAIL;
	    *start = p;
	    vim_free(evalarg->eval_tofree_lambda);
	    evalarg->eval_tofree_lambda = p;
	    // Compute "end" relative to the end.
	    *end = *start + STRLEN(*start) - endoff;
	}
    }

    بازگشت res;
}

/*
 * Convert "tv" to a string.
 * When "join_list" is TRUE convert a List or a Tuple into a sequence of lines.
 * Returns an allocated string (NULL when out of memory).
 */
    char_u *
typval2string(typval_T *tv, صحیح join_list)
{
    garray_T	ga;
    char_u	*retval = پوچ;

    اگر (join_list && (tv->v_type == VAR_LIST || tv->v_type == VAR_TUPLE))
    {
	اگر (tv->v_type == VAR_LIST)
	{
	    ga_init2(&ga, اندازه(نویسه), 80);
	    اگر (tv->vval.v_list != پوچ)
	    {
		list_join(&ga, tv->vval.v_list, (char_u *)"\n", TRUE, FALSE,
									0);
		اگر (tv->vval.v_list->lv_len > 0)
		    ga_append(&ga, NL);
	    }
	    ga_append(&ga, NUL);
	    retval = (char_u *)ga.ga_data;
	}
	وگرنه
	{
	    // tuple
	    ga_init2(&ga, اندازه(نویسه), 80);
	    اگر (tv->vval.v_tuple != پوچ)
	    {
		tuple_join(&ga, tv->vval.v_tuple, (char_u *)"\n", TRUE, FALSE,
									0);
		اگر (TUPLE_LEN(tv->vval.v_tuple) > 0)
		    ga_append(&ga, NL);
	    }
	    ga_append(&ga, NUL);
	    retval = (char_u *)ga.ga_data;
	}
    }
    وگرنه اگر (tv->v_type == VAR_LIST
	    || tv->v_type == VAR_TUPLE
	    || tv->v_type == VAR_DICT)
    {
	char_u	*tofree;
	char_u	numbuf[NUMBUFLEN];

	retval = tv2string(tv, &tofree, numbuf, 0);
	// Make a copy if we have a value but it's not in allocated memory.
	اگر (retval != پوچ && tofree == پوچ)
	    retval = vim_strsave(retval);
    }
    وگرنه
	retval = vim_strsave(tv_get_string(tv));
    بازگشت retval;
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
    صحیح		join_list,
    exarg_T	*eap,
    صحیح		use_simple_function)
{
    typval_T	tv;
    char_u	*retval;
    evalarg_T	evalarg;
    صحیح		r;

    fill_evalarg_from_eap(&evalarg, eap, eap != پوچ && eap->skip);
    اگر (use_simple_function)
	r = eval0_simple_funccal(arg, &tv, پوچ, &evalarg);
    وگرنه
	r = eval0(arg, &tv, پوچ, &evalarg);
    اگر (r == FAIL)
	retval = پوچ;
    وگرنه
    {
	retval = typval2string(&tv, join_list);
	clear_tv(&tv);
    }
    clear_evalarg(&evalarg, پوچ);

    بازگشت retval;
}

    char_u *
eval_to_string(
    char_u	*arg,
    صحیح		join_list,
    صحیح		use_simple_function)
{
    بازگشت eval_to_string_eap(arg, join_list, پوچ, use_simple_function);
}

/*
 * Call eval_to_string() without using current local variables and using
 * textlock.  When "use_sandbox" is TRUE use the sandbox.
 * Use legacy Vim script syntax.
 */
    char_u *
eval_to_string_safe(
    char_u	*arg,
    صحیح		use_sandbox,
    صحیح		keep_script_version,
    صحیح		use_simple_function)
{
    char_u	*retval;
    funccal_entry_T funccal_entry;
    صحیح		save_sc_version = current_sctx.sc_version;
    صحیح		save_garbage = may_garbage_collect;

    اگر (!keep_script_version)
	current_sctx.sc_version = 1;
    save_funccal(&funccal_entry);
    اگر (use_sandbox)
	++sandbox;
    ++textlock;
    may_garbage_collect = FALSE;
    retval = eval_to_string(arg, FALSE, use_simple_function);
    اگر (use_sandbox)
	--sandbox;
    --textlock;
    may_garbage_collect = save_garbage;
    restore_funccal();
    current_sctx.sc_version = save_sc_version;
    بازگشت retval;
}

/*
 * Top level evaluation function, returning a number.
 * Evaluates "expr" silently.
 * Returns -1 for an error.
 */
    varnumber_T
eval_to_عدد(char_u *عبارت, صحیح use_simple_function)
{
    typval_T	rettv;
    varnumber_T	retval;
    char_u	*p = skipwhite(عبارت);
    صحیح		r = NOTDONE;

    ++emsg_off;

    اگر (use_simple_function)
	r = may_call_simple_func(عبارت, &rettv);
    اگر (r == NOTDONE)
	r = eval1(&p, &rettv, &EVALARG_EVALUATE);
    اگر (r == FAIL)
	retval = -1;
    وگرنه
    {
	retval = tv_get_عدد_chk(&rettv, پوچ);
	clear_tv(&rettv);
    }
    --emsg_off;

    بازگشت retval;
}

/*
 * Top level evaluation function.
 * Returns an allocated typval_T with the result.
 * Returns NULL when there is an error.
 */
    typval_T *
eval_عبارت(char_u *arg, exarg_T *eap)
{
    بازگشت eval_عبارت_ext(arg, eap, FALSE);
}

    typval_T *
eval_عبارت_ext(char_u *arg, exarg_T *eap, صحیح use_simple_function)
{
    typval_T	*tv;
    evalarg_T	evalarg;

    fill_evalarg_from_eap(&evalarg, eap, eap != پوچ && eap->skip);

    tv = ALLOC_ONE(typval_T);
    اگر (tv != پوچ)
    {
	صحیح	r = NOTDONE;

	اگر (use_simple_function)
	    r = eval0_simple_funccal(arg, tv, eap, &evalarg);
	اگر (r == NOTDONE)
	    r = eval0(arg, tv, eap, &evalarg);

	اگر (r == FAIL)
	    VIM_CLEAR(tv);
    }

    clear_evalarg(&evalarg, eap);
    بازگشت tv;
}

/*
 * "*arg" points to what can be a function name in the form of "import.Name" or
 * "Funcref".  Return the name of the function.  Set "tofree" to something that
 * was allocated.
 * If "verbose" is FALSE no errors are given.
 * Return NULL for any failure.
 */
    ایستا char_u *
deref_function_name(
	    char_u	**arg,
	    char_u	**tofree,
	    evalarg_T	*evalarg,
	    صحیح		verbose)
{
    typval_T	ref;
    char_u	*name = *arg;
    صحیح		save_flags = 0;
    صحیح		evaluate = evalarg != پوچ
				      && (evalarg->eval_flags & EVAL_EVALUATE);

    ref.v_type = VAR_UNKNOWN;
    اگر (evalarg != پوچ)
    {
	// need to evaluate this to get an import, like in "a.Func"
	save_flags = evalarg->eval_flags;
	evalarg->eval_flags |= EVAL_EVALUATE;
    }
    اگر (eval9(arg, &ref, evalarg, FALSE) == FAIL)
    {
	dictitem_T	*v;

	// If <SID>VarName was used it would not be found, try another way.
	v = find_var_also_in_script(name, پوچ, FALSE);
	اگر (v == پوچ)
	{
	    name = پوچ;
	    برو theend;
	}
	copy_tv(&v->di_tv, &ref);
    }
    اگر (*skipwhite(*arg) != NUL)
    {
	اگر (verbose)
	    semsg(_(e_trailing_characters_str), *arg);
	name = پوچ;
    }
    وگرنه اگر (ref.v_type == VAR_FUNC && ref.vval.v_string != پوچ)
    {
	name = ref.vval.v_string;
	ref.vval.v_string = پوچ;
	*tofree = name;
    }
    وگرنه اگر (ref.v_type == VAR_PARTIAL && ref.vval.v_partial != پوچ)
    {
	اگر (ref.vval.v_partial->pt_argc > 0
		|| ref.vval.v_partial->pt_dict != پوچ)
	{
	    اگر (verbose)
		emsg(_(e_cannot_use_partial_here));
	    name = پوچ;
	}
	وگرنه
	{
	    name = vim_strsave(partial_name(ref.vval.v_partial));
	    *tofree = name;
	}
    }
    وگرنه اگر (evaluate)
    {
	اگر (verbose)
	    semsg(_(e_not_callable_type_str), name);
	name = پوچ;
    }

theend:
    clear_tv(&ref);
    اگر (evalarg != پوچ)
	evalarg->eval_flags = save_flags;
    بازگشت name;
}

/*
 * Call some Vim script function and return the result in "*rettv".
 * Uses argv[0] to argv[argc - 1] for the function arguments.  argv[argc]
 * should have type VAR_UNKNOWN.
 * Returns OK or FAIL.
 */
    صحیح
call_vim_function(
    char_u      *func,
    صحیح		argc,
    typval_T	*argv,
    typval_T	*rettv)
{
    صحیح		ret;
    funcexe_T	funcexe;
    char_u	*arg;
    char_u	*name;
    char_u	*tofree = پوچ;
    صحیح		ignore_errors;

    rettv->v_type = VAR_UNKNOWN;		// clear_tv() uses this
    CLEAR_FIELD(funcexe);
    funcexe.fe_firstline = curwin->w_cursor.lnum;
    funcexe.fe_lastline = curwin->w_cursor.lnum;
    funcexe.fe_evaluate = TRUE;

    // The name might be "import.Func" or "Funcref".  We don't know, we need to
    // ignore errors for an undefined name.  But we do want errors when an
    // autoload script has errors.  Guess that when there is a dot in the name
    // showing errors is the right choice.
    ignore_errors = vim_strchr(func, '.') == پوچ;
    arg = func;
    اگر (ignore_errors)
	++emsg_off;
    name = deref_function_name(&arg, &tofree, &EVALARG_EVALUATE, FALSE);
    اگر (ignore_errors)
	--emsg_off;
    اگر (name == پوچ)
	name = func;

    ret = call_func(name, -1, rettv, argc, argv, &funcexe);

    اگر (ret == FAIL)
	clear_tv(rettv);
    vim_free(tofree);

    بازگشت ret;
}

/*
 * Call Vim script function "func" and return the result as a string.
 * Uses "argv[0]" to "argv[argc - 1]" for the function arguments. "argv[argc]"
 * should have type VAR_UNKNOWN.
 * Returns NULL when calling the function fails.
 */
    تهی *
call_func_retstr(
    char_u      *func,
    صحیح		argc,
    typval_T	*argv)
{
    typval_T	rettv;
    char_u	*retval;

    اگر (call_vim_function(func, argc, argv, &rettv) == FAIL)
	بازگشت پوچ;

    retval = vim_strsave(tv_get_string(&rettv));
    clear_tv(&rettv);
    بازگشت retval;
}

/*
 * Call Vim script function "func" and return the result as a List.
 * Uses "argv" and "argc" as call_func_retstr().
 * Returns NULL when there is something wrong.
 * Gives an error when the returned value is not a list.
 */
    تهی *
call_func_retlist(
    char_u      *func,
    صحیح		argc,
    typval_T	*argv)
{
    typval_T	rettv;

    اگر (call_vim_function(func, argc, argv, &rettv) == FAIL)
	بازگشت پوچ;

    اگر (rettv.v_type != VAR_LIST)
    {
	semsg(_(e_custom_list_completion_function_does_not_return_list_but_str),
		vartype_name(rettv.v_type));
	clear_tv(&rettv);
	بازگشت پوچ;
    }

    بازگشت rettv.vval.v_list;
}

#اگر defined(FEAT_FOLDING)
/*
 * Evaluate "arg", which is 'foldexpr'.
 * Note: caller must set "curwin" to match "arg".
 * Returns the foldlevel, and any character preceding it in "*cp".  Doesn't
 * give error messages.
 */
    صحیح
eval_foldexpr(win_T *wp, صحیح *cp)
{
    char_u	*arg;
    typval_T	tv;
    varnumber_T	retval;
    char_u	*s;
    sctx_T	saved_sctx = current_sctx;
    صحیح		use_sandbox = was_set_insecurely(wp, (char_u *)"foldexpr",
								    OPT_LOCAL);

    arg = skipwhite(wp->w_p_fde);
    current_sctx = wp->w_p_script_ctx[WV_FDE];

    ++emsg_off;
    اگر (use_sandbox)
	++sandbox;
    ++textlock;
    *cp = NUL;

    // Evaluate the expression.  If the expression is "FuncName()" call the
    // function directly.
    اگر (eval0_simple_funccal(arg, &tv, پوچ, &EVALARG_EVALUATE) == FAIL)
	retval = 0;
    وگرنه
    {
	// If the result is a number, just return the number.
	اگر (tv.v_type == VAR_عدد)
	    retval = tv.vval.v_عدد;
	وگرنه اگر (tv.v_type != VAR_STRING || tv.vval.v_string == پوچ)
	    retval = 0;
	وگرنه
	{
	    // If the result is a string, check if there is a non-digit before
	    // the number.
	    s = tv.vval.v_string;
	    اگر (*s != NUL && !VIM_ISDIGIT(*s) && *s != '-')
		*cp = *s++;
	    retval = atol((نویسه *)s);
	}
	clear_tv(&tv);
    }
    --emsg_off;
    اگر (use_sandbox)
	--sandbox;
    --textlock;
    clear_evalarg(&EVALARG_EVALUATE, پوچ);
    current_sctx = saved_sctx;

    بازگشت (صحیح)retval;
}
#endif

#ifdef LOG_LOCKVAR
تعریفنوع ساختار flag_string_S
{
    صحیح	    flag;
    نویسه    *str;
} flag_string_T;

    ایستا نویسه *
flags_tostring(صحیح flags, flag_string_T *_fstring, نویسه *buf, size_t n)
{
    نویسه *p = buf;
    *p = NUL;
    برای (flag_string_T *fstring = _fstring; fstring->flag; ++fstring)
    {
	اگر ((fstring->flag & flags) != 0)
	{
	    size_t len = STRLEN(fstring->str);
	    اگر (n > p - buf + len + 7)
	    {
		STRCAT(p, fstring->str);
		p += len;
		STRCAT(p, " ");
		++p;
	    }
	    وگرنه
	    {
		STRCAT(buf, "...");
		بشکن;
	    }
	}
    }
    بازگشت buf;
}

flag_string_T glv_flag_strings[] = {
    { GLV_QUIET,		"QUIET" },
    { GLV_NO_AUTOLOAD,		"NO_AUTOLOAD" },
    { GLV_READ_ONLY,		"READ_ONLY" },
    { GLV_NO_DECL,		"NO_DECL" },
    { GLV_COMPILING,		"COMPILING" },
    { GLV_ASSIGN_WITH_OP,	"ASSIGN_WITH_OP" },
    { GLV_PREFER_FUNC,		"PREFER_FUNC" },
    { 0,			پوچ }
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
    ایستا تهی
fill_lval_from_lval_ریشه(lval_T *lp, lval_ریشه_T *lr)
{
#ifdef LOG_LOCKVAR
    ch_log(پوچ, "LKVAR: fill_lval_from_lval_root(): name %s, tv %p",
						lp->ll_name, (تهی*)lr->lr_tv);
#endif
    اگر (lr->lr_tv == پوچ)
	بازگشت;
    اگر (!lr->lr_is_arg && lr->lr_tv->v_type == VAR_CLASS)
    {
	اگر (lr->lr_tv->vval.v_class != پوچ)
	{
	    // Special special case. Look for a bare class variable reference.
	    class_T	*cl = lr->lr_tv->vval.v_class;
	    صحیح		m_idx;
	    ocmember_T	*m = class_member_lookup(cl, lp->ll_name,
					lp->ll_name_end - lp->ll_name, &m_idx);
	    اگر (m != پوچ)
	    {
		// Assuming "inside class" since bare reference.
		lp->ll_class = lr->lr_tv->vval.v_class;
		lp->ll_oi = m_idx;
		lp->ll_valtype = m->ocm_type;
		lp->ll_tv = &lp->ll_class->class_members_tv[m_idx];
#ifdef LOG_LOCKVAR
		ch_log(پوچ, "LKVAR:    ... class member %s.%s",
					lp->ll_class->class_name.string, lp->ll_name);
#endif
		بازگشت;
	    }
	}
    }

#ifdef LOG_LOCKVAR
    ch_log(پوچ, "LKVAR:    ... type: %s", vartype_name(lr->lr_tv->v_type));
#endif
    lp->ll_tv = lr->lr_tv;
    lp->ll_is_ریشه = TRUE;
}

/*
 * Check if the class has permission to access the member.
 * Returns OK or FAIL.
 */
    ایستا صحیح
get_lval_check_access(
    class_T	*cl_exec,   // executing class, NULL if :def or script level
    class_T	*cl,	    // class which contains the member
    ocmember_T	*om,	    // member being accessed
    char_u	*p,	    // char after member name
    صحیح		flags)	    // GLV flags to check if writing to lval
{
#ifdef LOG_LOCKVAR
    ch_log(پوچ, "LKVAR: get_lval_check_access(), cl_exec %p, cl %p, %c",
						(تهی*)cl_exec, (تهی*)cl, *p);
#endif
    اگر (cl_exec != پوچ && cl_exec == cl)
	بازگشت OK;

    نویسه *پیام = پوچ;
    انتخاب (om->ocm_access)
    {
	حالت VIM_ACCESS_PRIVATE:
	    پیام = e_cannot_access_protected_variable_str;
	    بشکن;
	حالت VIM_ACCESS_READ:
	    // If [idx] or .key following, read only OK.
	    اگر (*p == '[' || *p == '.')
		بشکن;
	    اگر ((flags & GLV_READ_ONLY) == 0)
	    {
		اگر (IS_ENUM(cl))
		{
		    اگر (om->ocm_type->tt_type == VAR_OBJECT)
			semsg(_(e_enumvalue_str_cannot_be_modified),
				cl->class_name.string, om->ocm_name.string);
		    وگرنه
			پیام = e_variable_is_not_writable_str;
		}
		وگرنه
		    پیام = e_variable_is_not_writable_str;
	    }
	    بشکن;
	حالت VIM_ACCESS_ALL:
	    بشکن;
    }
    اگر (پیام != پوچ)
    {
	emsg_var_cl_define(پیام, om->ocm_name.string, 0, cl);
	بازگشت FAIL;
    }
    بازگشت OK;
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
    ایستا char_u *
get_lval_imported(
    lval_T	*lp,
    scid_T	imp_sid,
    char_u	*p,
    dictitem_T	**dip,
    صحیح		fne_flags)
{
    ufunc_T	*ufunc;
    type_T	*type = پوچ;
    صحیح		cc;
    صحیح		rc = FAIL;

    p = skipwhite(p);

    import_check_sourced_sid(&imp_sid);
    lp->ll_sid = imp_sid;
    lp->ll_name = p;
    p = find_name_end(lp->ll_name, پوچ, پوچ, fne_flags);
    lp->ll_name_end = p;

    // check the item is exported
    cc = *p;
    *p = NUL;
    اگر (find_exported(imp_sid, lp->ll_name, &ufunc, &type, پوچ, پوچ,
								TRUE) == -1)
	برو failed;

    // Get the typval for the exported item
    hashtab_T *ht = &SCRIPT_VARS(imp_sid);
    اگر (ht == پوچ)
	برو failed;

    dictitem_T *di = find_var_in_ht(ht, 0, lp->ll_name, TRUE);
    اگر (di == پوچ)
	// script is autoloaded.  So variable will be found later
	برو success;

    *dip = di;

    // Check whether the variable is writable.
    svar_T *sv = find_typval_in_script(&di->di_tv, imp_sid, FALSE);
    اگر (sv != پوچ && sv->sv_const != 0)
    {
	semsg(_(e_cannot_change_readonly_variable_str), lp->ll_name);
	برو failed;
    }

    // check whether variable is locked
    اگر (مقدار_check_lock(di->di_tv.v_lock, lp->ll_name, FALSE))
	برو failed;

    lp->ll_tv = &di->di_tv;
    lp->ll_valtype = type;

success:
    rc = OK;

failed:
    *p = cc;
    بازگشت rc == OK ? p : پوچ;
}

تعریفنوع شمارش {
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
    ایستا صحیح
get_lval_dict_item(
    lval_T	*lp,
    char_u	*name,
    char_u	*کلید,
    صحیح		len,
    char_u	**کلید_end,
    typval_T	*var1,
    صحیح		flags,
    صحیح		unlet,
    typval_T	*rettv)
{
    صحیح		quiet = flags & GLV_QUIET;
    char_u	*p = *کلید_end;

    اگر (len == -1)
    {
	// "[key]": get key from "var1"
	کلید = tv_get_string_chk(var1);	// is number or string
	اگر (کلید == پوچ)
	    بازگشت GLV_FAIL;
    }
    lp->ll_list = پوچ;
    lp->ll_list = پوچ;
    lp->ll_blob = پوچ;
    lp->ll_object = پوچ;
    lp->ll_class = پوچ;
    lp->ll_tuple = پوچ;

    // a NULL dict is equivalent with an empty dict
    اگر (lp->ll_tv->vval.v_dict == پوچ)
    {
	lp->ll_tv->vval.v_dict = dict_alloc();
	اگر (lp->ll_tv->vval.v_dict == پوچ)
	    بازگشت GLV_FAIL;
	++lp->ll_tv->vval.v_dict->dv_refcount;
    }
    lp->ll_dict = lp->ll_tv->vval.v_dict;

    lp->ll_di = dict_find(lp->ll_dict, کلید, len);

    // When assigning to a scope dictionary check that a function and
    // variable name is valid (only variable name unless it is l: or
    // g: dictionary). Disallow overwriting a builtin function.
    اگر (rettv != پوچ && lp->ll_dict->dv_scope != 0)
    {
	صحیح prevval;

	اگر (len != -1)
	{
	    prevval = کلید[len];
	    کلید[len] = NUL;
	}
	وگرنه
	    prevval = 0; // avoid compiler warning
	صحیح wrong = (lp->ll_dict->dv_scope == VAR_DEF_SCOPE
		&& (rettv->v_type == VAR_FUNC
		    || rettv->v_type == VAR_PARTIAL)
		&& var_wrong_func_name(کلید, lp->ll_di == پوچ))
	    || !valid_varname(کلید, -1, TRUE);
	اگر (len != -1)
	    کلید[len] = prevval;
	اگر (wrong)
	    بازگشت GLV_FAIL;
    }

    اگر (lp->ll_valtype != پوچ)
	// use the type of the member
	lp->ll_valtype = lp->ll_valtype->tt_member;

    اگر (lp->ll_di == پوچ)
    {
	// Can't add "v:" or "a:" variable.
	اگر (lp->ll_dict == get_vimvar_dict()
		|| &lp->ll_dict->dv_hashtab == get_funccal_args_ht())
	{
	    semsg(_(e_illegal_variable_name_str), name);
	    بازگشت GLV_FAIL;
	}

	// Key does not exist in dict: may need to add it.
	اگر (*p == '[' || *p == '.' || unlet)
	{
	    اگر (!quiet)
		semsg(_(e_کلید_not_present_in_dictionary_str), کلید);
	    بازگشت GLV_FAIL;
	}
	اگر (len == -1)
	    lp->ll_newkey = vim_strsave(کلید);
	وگرنه
	    lp->ll_newkey = vim_strnsave(کلید, len);
	اگر (lp->ll_newkey == پوچ)
	    p = پوچ;

	*کلید_end = p;
	بازگشت GLV_STOP;
    }
    // existing variable, need to check if it can be changed
    وگرنه اگر ((flags & GLV_READ_ONLY) == 0
	    && (var_check_ro(lp->ll_di->di_flags, name, FALSE)
		|| var_check_lock(lp->ll_di->di_flags, name, FALSE)))
	بازگشت GLV_FAIL;

    lp->ll_tv = &lp->ll_di->di_tv;

    بازگشت GLV_OK;
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
    ایستا صحیح
get_lval_blob(
    lval_T	*lp,
    typval_T	*var1,
    typval_T	*var2,
    صحیح		empty1,
    صحیح		quiet)
{
    long	bloblen = blob_len(lp->ll_tv->vval.v_blob);

    lp->ll_list = پوچ;
    lp->ll_dict = پوچ;
    lp->ll_object = پوچ;
    lp->ll_class = پوچ;
    lp->ll_tuple = پوچ;

    // Get the number and item for the only or first index of a List or Tuple.
    اگر (empty1)
	lp->ll_n1 = 0;
    وگرنه
	// is number or string
	lp->ll_n1 = (long)tv_get_عدد(var1);

    اگر (check_blob_index(bloblen, lp->ll_n1, quiet) == FAIL)
	بازگشت FAIL;
    اگر (lp->ll_range && !lp->ll_empty2)
    {
	lp->ll_n2 = (long)tv_get_عدد(var2);
	اگر (check_blob_range(bloblen, lp->ll_n1, lp->ll_n2, quiet) == FAIL)
	    بازگشت FAIL;
    }

    اگر (!lp->ll_range)
	// Indexing a single byte in a blob.  So the rhs type is a
	// number.
	lp->ll_valtype = &t_عدد;

    lp->ll_blob = lp->ll_tv->vval.v_blob;
    lp->ll_tv = پوچ;

    بازگشت OK;
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
    ایستا صحیح
get_lval_list(
    lval_T	*lp,
    typval_T	*var1,
    typval_T	*var2,
    صحیح		empty1,
    صحیح		flags,
    صحیح		quiet)
{
    /*
     * Get the number and item for the only or first index of the List.
     */
    اگر (empty1)
	lp->ll_n1 = 0;
    وگرنه
	// is number or string
	lp->ll_n1 = (long)tv_get_عدد(var1);

    lp->ll_dict = پوچ;
    lp->ll_object = پوچ;
    lp->ll_class = پوچ;
    lp->ll_tuple = پوچ;
    lp->ll_list = lp->ll_tv->vval.v_list;
    lp->ll_li = check_range_index_one(lp->ll_list, &lp->ll_n1,
				(flags & GLV_ASSIGN_WITH_OP) == 0, quiet);
    اگر (lp->ll_li == پوچ)
	بازگشت FAIL;

    اگر (lp->ll_valtype != پوچ && !lp->ll_range)
    {
	// use the type of the member
	اگر (lp->ll_valtype->tt_member != پوچ)
	    lp->ll_valtype = lp->ll_valtype->tt_member;
	وگرنه
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
    اگر (lp->ll_range && !lp->ll_empty2)
    {
	lp->ll_n2 = (long)tv_get_عدد(var2);
	// is number or string
	اگر (check_range_index_two(lp->ll_list,
			&lp->ll_n1, lp->ll_li, &lp->ll_n2, quiet) == FAIL)
	    بازگشت FAIL;
    }

    lp->ll_tv = &lp->ll_li->li_tv;

    بازگشت OK;
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
    ایستا صحیح
get_lval_tuple(
    lval_T	*lp,
    typval_T	*idx,
    صحیح		quiet)
{
    // is number or string
    lp->ll_n1 = (long)tv_get_عدد(idx);

    lp->ll_list = پوچ;
    lp->ll_dict = پوچ;
    lp->ll_blob = پوچ;
    lp->ll_object = پوچ;
    lp->ll_class = پوچ;

    lp->ll_tuple = lp->ll_tv->vval.v_tuple;
    lp->ll_tv = tuple_find(lp->ll_tuple, lp->ll_n1);
    اگر (lp->ll_tv == پوچ)
    {
	اگر (!quiet)
	    semsg(_(e_tuple_index_out_of_range_nr), lp->ll_n1);
	بازگشت GLV_FAIL;
    }

    // use the type of the member
    اگر (lp->ll_valtype != پوچ)
    {
	اگر (lp->ll_valtype != پوچ
		&& lp->ll_valtype->tt_type == VAR_TUPLE
		&& lp->ll_valtype->tt_argcount == 1)
	{
	    // a variadic tuple or a single item tuple
	    اگر (lp->ll_valtype->tt_flags & TTFLAG_VARARGS)
		lp->ll_valtype = lp->ll_valtype->tt_args[0]->tt_member;
	    وگرنه
		lp->ll_valtype = lp->ll_valtype->tt_args[0];
	}
	وگرنه
	    // If the LHS member type is not known (VAR_ANY), then get it from
	    // the tuple item (after indexing)
	    lp->ll_valtype = typval2type(lp->ll_tv, get_copyID(),
					&lp->ll_type_list, TVTT_DO_MEMBER);
    }

    بازگشت GLV_OK;
}

/*
 * Get a class or object lval method in class "cl".  The 'key' argument points
 * to the method name and 'key_end' points to the character after 'key'.
 * 'v_type' is VAR_CLASS or VAR_OBJECT.
 *
 * The method index, method function pointer and method type are returned in
 * "lp".
 */
    ایستا صحیح
get_lval_oc_method(
    lval_T	*lp,
    class_T	*cl,
    char_u	*کلید,
    char_u	**کلید_end,
    vartype_T	v_type)
{
    // Look for a method with this name.
    // round 1: class functions (skipped for an object)
    // round 2: object methods
    برای (صحیح round = v_type == VAR_OBJECT ? 2 : 1; round <= 2; ++round)
    {
	صحیح	m_idx;
	ufunc_T	*fp;

	fp = method_lookup(cl, round == 1 ? VAR_CLASS : VAR_OBJECT,
						کلید, *کلید_end - کلید, &m_idx);
	lp->ll_oi = m_idx;

	// process generic method (if present)
	اگر (fp && (fp = eval_generic_func(fp, کلید, کلید_end)) == پوچ)
	    بازگشت FAIL;

	اگر (fp != پوچ)
	{
	    lp->ll_ufunc = fp;
	    lp->ll_valtype = fp->uf_func_type;
	    بشکن;
	}
    }

    بازگشت OK;
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
    ایستا صحیح
get_lval_oc_variable(
    lval_T	*lp,
    class_T	*cl,
    char_u	*کلید,
    char_u	*کلید_end,
    vartype_T	v_type,
    class_T	*cl_exec,
    صحیح		flags)
{
    صحیح		m_idx;
    ocmember_T	*om;

    om = member_lookup(cl, v_type, کلید, کلید_end - کلید, &m_idx);
    lp->ll_oi = m_idx;
    اگر (om == پوچ)
	بازگشت OK;

    // Check variable is accessible
    اگر (get_lval_check_access(cl_exec, cl, om, کلید_end, flags) == FAIL)
	بازگشت FAIL;

    // When lhs is used to modify the variable, check it is not a read-only
    // variable.
    اگر ((flags & GLV_READ_ONLY) == 0 && (*کلید_end != '.' && *کلید_end != '[')
	    && oc_var_check_ro(cl, om))
	بازگشت FAIL;

    lp->ll_valtype = om->ocm_type;

    اگر (v_type == VAR_OBJECT)
	lp->ll_tv = ((typval_T *)(lp->ll_tv->vval.v_object + 1)) + m_idx;
    وگرنه
	lp->ll_tv = &cl->class_members_tv[m_idx];

    بازگشت OK;
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
    ایستا صحیح
get_lval_class_or_obj(
    lval_T	*lp,
    char_u	*کلید,
    char_u	**کلید_end,
    vartype_T	v_type,
    class_T	*cl_exec,
    صحیح		flags,
    صحیح		quiet)
{
    lp->ll_dict = پوچ;
    lp->ll_list = پوچ;
    lp->ll_tuple = پوچ;

    class_T *cl;
    اگر (v_type == VAR_OBJECT)
    {
	اگر (lp->ll_tv->vval.v_object == پوچ)
	{
	    اگر (!quiet)
		emsg(_(e_using_null_object));
	    بازگشت FAIL;
	}
	cl = lp->ll_tv->vval.v_object->obj_class;
	lp->ll_object = lp->ll_tv->vval.v_object;
    }
    وگرنه
    {
	cl = lp->ll_tv->vval.v_class;
	lp->ll_object = پوچ;
    }
    lp->ll_class = cl;

    اگر (cl == پوچ)
	// TODO: what if class is NULL?
	بازگشت OK;

    lp->ll_valtype = پوچ;

    اگر (flags & GLV_PREFER_FUNC)
	اگر (get_lval_oc_method(lp, cl, کلید, کلید_end, v_type) == FAIL)
	    بازگشت FAIL;

    // Look for object/class member variable
    اگر (lp->ll_valtype == پوچ)
    {
	اگر (get_lval_oc_variable(lp, cl, کلید, *کلید_end, v_type, cl_exec, flags)
								== FAIL)
	    بازگشت FAIL;
    }

    اگر (lp->ll_valtype == پوچ)
    {
	member_not_found_پیام(cl, v_type, کلید, *کلید_end - کلید);
	بازگشت FAIL;
    }

    بازگشت OK;
}

/*
 * Check whether dot (".") is allowed after the variable "name" with type
 * "v_type".  Only Dict, Class and Object types support a dot after the name.
 * Returns TRUE if dot is allowed after the name.
 */
    ایستا صحیح
dot_allowed_after_type(char_u *name, vartype_T v_type, صحیح quiet)
{
    اگر (v_type != VAR_DICT && v_type != VAR_OBJECT && v_type != VAR_CLASS)
    {
	اگر (!quiet)
	    semsg(_(e_dot_not_allowed_after_str_str),
		    vartype_name(v_type), name);
	بازگشت FALSE;
    }

    بازگشت TRUE;
}

/*
 * Check whether left bracket ("[") is allowed after the variable "name" with
 * type "v_type".  Only Dict, List, Tuple and Blob types support a bracket
 * after the variable name.  Returns TRUE if bracket is allowed after the name.
 */
    ایستا صحیح
bracket_allowed_after_type(char_u *name, vartype_T v_type, صحیح quiet)
{
    اگر (v_type == VAR_CLASS || v_type == VAR_OBJECT)
    {
	اگر (!quiet)
	    semsg(_(e_index_not_allowed_after_str_str),
		    vartype_name(v_type), name);
	بازگشت FALSE;
    }

    بازگشت TRUE;
}

/*
 * Check whether the variable "name" with type "v_type" can be followed by an
 * index.  Only Dict, List, Tuple, Blob, Object and Class types support
 * indexing.  Returns TRUE if indexing is allowed after the name.
 */
    ایستا صحیح
index_allowed_after_type(char_u *name, vartype_T v_type, صحیح quiet)
{
    اگر (v_type != VAR_LIST
	    && v_type != VAR_TUPLE
	    && v_type != VAR_DICT
	    && v_type != VAR_BLOB
	    && v_type != VAR_OBJECT
	    && v_type != VAR_CLASS)
    {
	اگر (!quiet)
	    semsg(_(e_index_not_allowed_after_str_str),
		    vartype_name(v_type), name);
	بازگشت FALSE;
    }

    بازگشت TRUE;
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
    ایستا char_u *
get_lval_subscript(
    lval_T	*lp,
    char_u	*p,
    char_u	*name,
    typval_T	*rettv,
    hashtab_T	*ht,
    dictitem_T	*v,
    صحیح		unlet,
    صحیح		flags,	    // GLV_ values
    class_T	*cl_exec)
{
    صحیح		quiet = flags & GLV_QUIET;
    char_u	*کلید = پوچ;
    صحیح		len;
    typval_T	var1;
    typval_T	var2;
    صحیح		empty1 = FALSE;
    صحیح		rc = FAIL;

    /*
     * Loop until no more [idx] or .key is following.
     */
    var1.v_type = VAR_UNKNOWN;
    var2.v_type = VAR_UNKNOWN;

    وقتی (*p == '[' || (*p == '.' && p[1] != '=' && p[1] != '.'))
    {
	vartype_T v_type = lp->ll_tv->v_type;

	اگر (*p == '.' && !dot_allowed_after_type(name, v_type, quiet))
	    برو done;

	اگر (*p == '[' && !bracket_allowed_after_type(name, v_type, quiet))
	    برو done;

	اگر (!index_allowed_after_type(name, v_type, quiet))
	    برو done;

	// A NULL list/blob works like an empty list/blob, allocate one now.
	صحیح r = OK;
	اگر (v_type == VAR_LIST && lp->ll_tv->vval.v_list == پوچ)
	    r = rettv_list_alloc(lp->ll_tv);
	وگرنه اگر (v_type == VAR_BLOB && lp->ll_tv->vval.v_blob == پوچ)
	    r = rettv_blob_alloc(lp->ll_tv);
	اگر (r == FAIL)
	    برو done;

	اگر (lp->ll_range)
	{
	    اگر (!quiet)
		emsg(_(e_slice_must_come_last));
	    برو done;
	}
#ifdef LOG_LOCKVAR
	ch_log(پوچ, "LKVAR: get_lval() loop: p: %s, type: %s", p,
		vartype_name(v_type));
#endif

	اگر (current_script_is_vim9() && lp->ll_valtype == پوچ
		&& v != پوچ
		&& lp->ll_tv == &v->di_tv
		&& ht != پوچ && ht == get_script_local_ht())
	{
	    svar_T  *sv = find_typval_in_script(lp->ll_tv, 0, TRUE);

	    // Vim9 script local variable: get the type
	    اگر (sv != پوچ)
	    {
		lp->ll_valtype = sv->sv_type;
#ifdef LOG_LOCKVAR
		ch_log(پوچ, "LKVAR:    ... loop: vim9 assign type: %s",
			vartype_name(lp->ll_valtype->tt_type));
#endif
	    }
	}

	len = -1;
	اگر (*p == '.')
	{
	    کلید = p + 1;

	    برای (len = 0; ASCII_ISALNUM(کلید[len]) || کلید[len] == '_'; ++len)
		;
	    اگر (len == 0)
	    {
		اگر (!quiet)
		    emsg(_(e_cannot_use_empty_کلید_for_dictionary));
		برو done;
	    }
	    p = کلید + len;
	}
	وگرنه
	{
	    // Get the index [expr] or the first index [expr: ].
	    p = skipwhite(p + 1);
	    اگر (*p == ':')
		empty1 = TRUE;
	    وگرنه
	    {
		empty1 = FALSE;
		اگر (eval1(&p, &var1, &EVALARG_EVALUATE) == FAIL)  // recursive!
		    برو done;
		اگر (tv_get_string_chk(&var1) == پوچ)
		    // not a number or string
		    برو done;
		p = skipwhite(p);
	    }

	    // Optionally get the second index [ :expr].
	    اگر (*p == ':')
	    {
		اگر (v_type == VAR_DICT)
		{
		    اگر (!quiet)
			emsg(_(e_cannot_slice_dictionary));
		    برو done;
		}
		اگر (v_type == VAR_TUPLE)
		{
		    اگر (!quiet)
			emsg(_(e_cannot_slice_tuple));
		    برو done;
		}
		اگر (rettv != پوچ
			&& !(rettv->v_type == VAR_LIST
			    && rettv->vval.v_list != پوچ)
			&& !(rettv->v_type == VAR_BLOB
			    && rettv->vval.v_blob != پوچ))
		{
		    اگر (!quiet)
			emsg(_(e_slice_requires_list_or_blob_مقدار));
		    برو done;
		}
		p = skipwhite(p + 1);
		اگر (*p == ']')
		    lp->ll_empty2 = TRUE;
		وگرنه
		{
		    lp->ll_empty2 = FALSE;
		    // recursive!
		    اگر (eval1(&p, &var2, &EVALARG_EVALUATE) == FAIL)
			برو done;
		    اگر (tv_get_string_chk(&var2) == پوچ)
			// not a number or string
			برو done;
		}
		lp->ll_range = TRUE;
	    }
	    وگرنه
		lp->ll_range = FALSE;

	    اگر (*p != ']')
	    {
		اگر (!quiet)
		    emsg(_(e_missing_closing_square_brace));
		برو done;
	    }

	    // Skip to past ']'.
	    ++p;
	}
#ifdef LOG_LOCKVAR
	اگر (len == -1)
	    ch_log(پوچ, "LKVAR:    ... loop: p: %s, '[' key: %s", p,
		    empty1 ? ":" : (نویسه*)tv_get_string(&var1));
	وگرنه
	    ch_log(پوچ, "LKVAR:    ... loop: p: %s, '.' key: %s", p, کلید);
#endif

	اگر (v_type == VAR_DICT)
	{
	    glv_status_T glv_status;

	    glv_status = get_lval_dict_item(lp, name, کلید, len, &p, &var1,
							flags, unlet, rettv);
	    اگر (glv_status == GLV_FAIL)
		برو done;
	    اگر (glv_status == GLV_STOP)
		بشکن;
	}
	وگرنه اگر (v_type == VAR_BLOB)
	{
	    اگر (get_lval_blob(lp, &var1, &var2, empty1, quiet) == FAIL)
		برو done;

	    بشکن;
	}
	وگرنه اگر (v_type == VAR_LIST)
	{
	    اگر (get_lval_list(lp, &var1, &var2, empty1, flags, quiet) == FAIL)
		برو done;
	}
	وگرنه اگر (v_type == VAR_TUPLE)
	{
	    اگر (get_lval_tuple(lp, &var1, quiet) == FAIL)
		برو done;
	}
	وگرنه  // v_type == VAR_CLASS || v_type == VAR_OBJECT
	{
	    اگر (get_lval_class_or_obj(lp, کلید, &p, v_type, cl_exec, flags,
							quiet) == FAIL)
		برو done;
	}

	clear_tv(&var1);
	clear_tv(&var2);
	var1.v_type = VAR_UNKNOWN;
	var2.v_type = VAR_UNKNOWN;
    }

    اگر (lp->ll_tuple != پوچ && (flags & GLV_READ_ONLY) == 0)
    {
	اگر (!quiet)
	    emsg(_(e_tuple_is_immutable));
	برو done;
    }

    rc = OK;

done:
    clear_tv(&var1);
    clear_tv(&var2);
    بازگشت rc == OK ? p : پوچ;
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
    صحیح		unlet,
    صحیح		skip,
    صحیح		flags,	    // GLV_ values
    صحیح		fne_flags)  // flags for find_name_end()
{
    char_u	*p;
    char_u	*عبارت_start, *عبارت_end;
    صحیح		cc;
    dictitem_T	*v = پوچ;
    hashtab_T	*ht = پوچ;
    صحیح		quiet = flags & GLV_QUIET;
    صحیح		writing = 0;
    صحیح		vim9script = in_vim9script();
    class_T	*cl_exec = پوچ;    // class that is executing, or NULL.

#ifdef LOG_LOCKVAR
    اگر (lval_ریشه == پوچ)
	ch_log(پوچ, "LKVAR: get_lval(): name: %s, lval_root (nil)", name);
    وگرنه
	ch_log(پوچ, "LKVAR: get_lval(): name: %s, lr_tv %p lr_is_arg %d",
			name, (تهی*)lval_ریشه->lr_tv, lval_ریشه->lr_is_arg);
    نویسه buf[80];
    ch_log(پوچ, "LKVAR:    ...: GLV flags: %s",
		    flags_tostring(flags, glv_flag_strings, buf, اندازه(buf)));
#endif

    // Clear everything in "lp".
    CLEAR_POINTER(lp);
    ga_init2(&lp->ll_type_list, اندازه(type_T *), 10);

    اگر (skip || (flags & GLV_COMPILING))
    {
	// When skipping or compiling just find the end of the name.
	lp->ll_name = name;
	lp->ll_name_end = find_name_end(name, پوچ, پوچ,
						      FNE_INCL_BR | fne_flags);
	بازگشت lp->ll_name_end;
    }

    // Cannot use "s:var" at the Vim9 script level.  "s: type" is OK.
    اگر (vim9script && at_script_level()
		  && name[0] == 's' && name[1] == ':' && !VIM_ISWHITE(name[2]))
    {
	semsg(_(e_cannot_use_s_colon_in_vim9_script_str), name);
	بازگشت پوچ;
    }

    // Find the end of the name.
    p = find_name_end(name, &عبارت_start, &عبارت_end, fne_flags);
    lp->ll_name_end = p;
    اگر (عبارت_start != پوچ)
    {
	// Don't expand the name when we already know there is an error.
	اگر (unlet && !VIM_ISWHITE(*p) && !ends_excmd(*p)
						    && *p != '[' && *p != '.')
	{
	    semsg(_(e_trailing_characters_str), p);
	    بازگشت پوچ;
	}

	lp->ll_exp_name = make_expanded_name(name, عبارت_start, عبارت_end, p);
	اگر (lp->ll_exp_name == پوچ)
	{
	    // Report an invalid expression in braces, unless the
	    // expression evaluation has been cancelled due to an
	    // aborting error, an interrupt, or an exception.
	    اگر (!aborting() && !quiet)
	    {
		emsg_severe = TRUE;
		semsg(_(e_invalid_argument_str), name);
		بازگشت پوچ;
	    }
	}
	lp->ll_name = lp->ll_exp_name;
    }
    وگرنه
    {
	lp->ll_name = name;

	اگر (vim9script)
	{
	    // "a: type" is declaring variable "a" with a type, not "a:".
	    // However, "g:[key]" is indexing a dictionary.
	    اگر (p == name + 2 && p[-1] == ':' && *p != '[')
	    {
		--p;
		lp->ll_name_end = p;
	    }
	    اگر (*skipwhite(p) == ':')
	    {
		char_u	    *tp = skipwhite(p + 1);

		اگر (is_scoped_variable(name))
		{
		    semsg(_(e_cannot_use_type_with_this_variable_str), name);
		    بازگشت پوچ;
		}
		اگر (VIM_ISWHITE(*p))
		{
		    semsg(_(e_no_white_space_allowed_before_colon_str), p);
		    بازگشت پوچ;
		}
		اگر (tp == p + 1 && !quiet)
		{
		    semsg(_(e_white_space_required_after_str_str), ":", p);
		    بازگشت پوچ;
		}
		اگر (!SCRIPT_ID_VALID(current_sctx.sc_sid))
		{
		    semsg(_(e_using_type_not_in_script_context_str), p);
		    بازگشت پوچ;
		}
		اگر (vim9script && (flags & GLV_NO_DECL) &&
			!(flags & GLV_FOR_LOOP))
		{
		    // Using a type and not in a "var" declaration.
		    semsg(_(e_trailing_characters_str), p);
		    بازگشت پوچ;
		}


		// parse the type after the name
		lp->ll_type = parse_type(&tp,
			       &SCRIPT_ITEM(current_sctx.sc_sid)->sn_type_list,
			       پوچ, پوچ, !quiet);
		اگر (!quiet && (lp->ll_type == پوچ
			    || !valid_declaration_type(lp->ll_type)))
		    بازگشت پوچ;
		lp->ll_name_end = tp;
	    }
	    // TODO: check inside class?
	}
    }
    اگر (lp->ll_name == پوچ)
	بازگشت p;

    اگر (*p == '.')
    {
	// In legacy script, when a local variable and import exists with this name,
	// prioritize local variable over imports to avoid conflicts.
	صحیح var_exists = FALSE;
	اگر (!vim9script)
	{
	    cc = *p;
	    *p = NUL;
	    hashtab_T *local_ht = get_funccal_local_ht();
	    اگر (local_ht != پوچ)
	    {
		hashitem_T *hi = hash_find(local_ht, lp->ll_name);
		اگر (!HASHITEM_EMPTY(hi))
		    var_exists = TRUE;
	    }
	    *p = cc;
	}

	اگر (!var_exists)
	{
	    imported_T *import = find_imported(lp->ll_name, p - lp->ll_name, TRUE);
	    اگر (import != پوچ)
	    {
		p++;	// skip '.'
		p = get_lval_imported(lp, import->imp_sid, p, &v, fne_flags);
		اگر (p == پوچ)
		    بازگشت پوچ;
	    }
	}
    }

    // Without [idx] or .key we are done.
    اگر (*p != '[' && *p != '.')
    {
	اگر (lval_ریشه != پوچ)
	    fill_lval_from_lval_ریشه(lp, lval_ریشه);
	بازگشت p;
    }

    اگر (vim9script && lval_ریشه != پوچ)
	cl_exec = lval_ریشه->lr_cl_exec;
    اگر (vim9script && lval_ریشه != پوچ && lval_ریشه->lr_tv != پوچ)
    {
	// using local variable
	lp->ll_tv = lval_ریشه->lr_tv;
	v = پوچ;
    }
    وگرنه اگر (lp->ll_tv == پوچ)
    {
	cc = *p;
	*p = NUL;
	// When we would write to the variable pass &ht and prevent autoload.
	writing = !(flags & GLV_READ_ONLY);
	v = find_var(lp->ll_name, writing ? &ht : پوچ,
					 (flags & GLV_NO_AUTOLOAD) || writing);
	اگر (v == پوچ && !quiet)
	    semsg(_(e_undefined_variable_str), lp->ll_name);
	*p = cc;
	اگر (v == پوچ)
	    بازگشت پوچ;
	lp->ll_tv = &v->di_tv;
    }

    اگر (vim9script && (flags & GLV_NO_DECL) == 0)
    {
	اگر (!quiet)
	    semsg(_(e_variable_already_declared_str), lp->ll_name);
	بازگشت پوچ;
    }

    // If the next character is a "." or a "[", then process the subitem.
    p = get_lval_subscript(lp, p, name, rettv, ht, v, unlet, flags, cl_exec);
    اگر (p == پوچ)
	بازگشت پوچ;

    اگر (vim9script && lp->ll_valtype != پوچ && rettv != پوچ)
    {
	where_T	    where = WHERE_INIT;

	// In a Vim9 script, do type check and make sure the variable is
	// writable.
	اگر (check_typval_type(lp->ll_valtype, rettv, where) == FAIL)
	    بازگشت پوچ;
    }

    lp->ll_name_end = p;
    بازگشت p;
}

/*
 * Clear lval "lp" that was filled by get_lval().
 */
    تهی
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
    تهی
set_var_lval(
    lval_T	*lp,
    char_u	*endp,
    typval_T	*rettv,
    صحیح		copy,
    صحیح		flags,	    // ASSIGN_CONST, ASSIGN_NO_DECL
    char_u	*op,
    صحیح		var_idx)    // index for "let [a, b] = list"
{
    صحیح		cc;
    dictitem_T	*di;

    اگر (lp->ll_tv == پوچ)
    {
	cc = *endp;
	*endp = NUL;
	اگر (in_vim9script() && check_reserved_name(lp->ll_name, FALSE) == FAIL)
	    بازگشت;

	اگر (lp->ll_blob != پوچ)
	{
	    صحیح	    error = FALSE;

	    اگر (op != پوچ && *op != '=')
	    {
		semsg(_(e_wrong_variable_type_for_str_equal), op);
		بازگشت;
	    }
	    اگر (مقدار_check_lock(lp->ll_blob->bv_lock, lp->ll_name, FALSE))
		بازگشت;

	    اگر (lp->ll_range && rettv->v_type == VAR_BLOB)
	    {
		اگر (lp->ll_empty2)
		    lp->ll_n2 = blob_len(lp->ll_blob) - 1;

		اگر (blob_set_range(lp->ll_blob, lp->ll_n1, lp->ll_n2,
								rettv) == FAIL)
		    بازگشت;
	    }
	    وگرنه
	    {
		varnumber_T	val = tv_get_عدد_chk(rettv, &error);
		اگر (!error)
		{
		    اگر (val < 0 || val > 255)
			semsg(_(e_invalid_مقدار_for_blob_nr), val);
		    وگرنه
			blob_set_append(lp->ll_blob, lp->ll_n1, val);
		}
	    }
	}
	وگرنه اگر (op != پوچ && *op != '=')
	{
	    typval_T tv;

	    اگر ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					     && (flags & ASSIGN_FOR_LOOP) == 0)
	    {
		emsg(_(e_cannot_modify_existing_variable));
		*endp = cc;
		بازگشت;
	    }

	    // handle +=, -=, *=, /=, %= and .=
	    di = پوچ;
	    اگر (eval_variable(lp->ll_name, (صحیح)STRLEN(lp->ll_name),
				 lp->ll_sid, &tv, &di, EVAL_VAR_VERBOSE) == OK)
	    {
		اگر (di != پوچ && check_typval_is_مقدار(&di->di_tv) == FAIL)
		{
		    clear_tv(&tv);
		    بازگشت;
		}

		// In Vim9 script a String v: variable only takes a String.
		اگر (in_vim9script() && STRNCMP(lp->ll_name, "v:", 2) == 0
			&& tv.v_type == VAR_STRING)
		{
		    where_T	where = WHERE_INIT;

		    اگر (check_typval_type(&t_string, rettv, where) == FAIL)
		    {
			clear_tv(&tv);
			*endp = cc;
			بازگشت;
		    }
		}

		اگر ((di == پوچ
			 || (!var_check_ro(di->di_flags, lp->ll_name, FALSE)
			   && !tv_check_lock(&di->di_tv, lp->ll_name, FALSE)))
			&& tv_op(&tv, rettv, op) == OK)
		    set_var_const(lp->ll_name, lp->ll_sid, پوچ, &tv, FALSE,
				ASSIGN_NO_DECL | ASSIGN_COMPOUND_OP, 0);
		clear_tv(&tv);
	    }
	}
	وگرنه
	{
	    اگر (lp->ll_type != پوچ && check_typval_arg_type(lp->ll_type, rettv,
							      پوچ, 0) == FAIL)
		بازگشت;
	    set_var_const(lp->ll_name, lp->ll_sid, lp->ll_type, rettv, copy,
							       flags, var_idx);
	}
	*endp = cc;
    }
    وگرنه اگر (مقدار_check_lock(lp->ll_newkey == پوچ
		? lp->ll_tv->v_lock
		: lp->ll_tv->vval.v_dict->dv_lock, lp->ll_name, FALSE))
	;
    وگرنه اگر (lp->ll_range)
    {
	اگر ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					     && (flags & ASSIGN_FOR_LOOP) == 0)
	{
	    emsg(_(e_cannot_lock_range));
	    بازگشت;
	}

	(تهی)list_assign_range(lp->ll_list, rettv->vval.v_list,
			 lp->ll_n1, lp->ll_n2, lp->ll_empty2, op, lp->ll_name);
    }
    وگرنه
    {
	/*
	 * Assign to a List, Dictionary or Object item.
	 */
	اگر ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					     && (flags & ASSIGN_FOR_LOOP) == 0)
	{
	    emsg(_(e_cannot_lock_list_or_dict));
	    بازگشت;
	}

	اگر (lp->ll_valtype != پوچ
		    && check_typval_arg_type(lp->ll_valtype, rettv,
							      پوچ, 0) == FAIL)
	{
	    lp->ll_name_end = پوچ;
	    بازگشت;
	}

	// If the lval is a List and the type of the list is not yet set,
	// then set the item type from the declared type of the variable.
	اگر (in_vim9script() && rettv->v_type == VAR_LIST
				&& rettv->vval.v_list != پوچ
				&& rettv->vval.v_list->lv_type == پوچ)
	{
	    اگر (lp->ll_tv->v_type == VAR_LIST
		    && lp->ll_tv->vval.v_list != پوچ
		    && lp->ll_tv->vval.v_list->lv_type != پوچ)
		set_tv_type(rettv, lp->ll_tv->vval.v_list->lv_type);
	}

	اگر (lp->ll_newkey != پوچ)
	{
	    اگر (op != پوچ && *op != '=')
	    {
		semsg(_(e_کلید_not_present_in_dictionary_str), lp->ll_newkey);
		بازگشت;
	    }
	    اگر (dict_wrong_func_name(lp->ll_tv->vval.v_dict, rettv,
								lp->ll_newkey))
		بازگشت;

	    // Need to add an item to the Dictionary.
	    di = dictitem_alloc(lp->ll_newkey);
	    اگر (di == پوچ)
		بازگشت;
	    اگر (dict_add(lp->ll_tv->vval.v_dict, di) == FAIL)
	    {
		vim_free(di);
		بازگشت;
	    }
	    lp->ll_tv = &di->di_tv;
	}
	وگرنه اگر (op != پوچ && *op != '=')
	{
	    tv_op(lp->ll_tv, rettv, op);
	    بازگشت;
	}
	وگرنه
	    clear_tv(lp->ll_tv);

	/*
	 * Assign the value to the variable or list item.
	 */
	اگر (copy)
	    copy_tv(rettv, lp->ll_tv);
	وگرنه
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
    ایستا صحیح
tv_op_blob(typval_T *tv1, typval_T *tv2, char_u *op)
{
    اگر (*op != '+' || tv2->v_type != VAR_BLOB)
	بازگشت FAIL;

    // Blob += Blob
    اگر (tv2->vval.v_blob == پوچ)
	بازگشت OK;

    اگر (tv1->vval.v_blob == پوچ)
    {
	tv1->vval.v_blob = tv2->vval.v_blob;
	++tv1->vval.v_blob->bv_refcount;
	بازگشت OK;
    }

    blob_T	*b1 = tv1->vval.v_blob;
    blob_T	*b2 = tv2->vval.v_blob;
    صحیح		len = blob_len(b2);

    اگر (len > 0 && ga_grow(&b1->bv_ga, len) == OK)
    {
	mch_memmove((char_u *)b1->bv_ga.ga_data + b1->bv_ga.ga_len,
		    (char_u *)b2->bv_ga.ga_data, (size_t)len);
	b1->bv_ga.ga_len += len;
    }
    وگرنه
	برای (صحیح i = 0; i < len; i++)
	    (تهی)ga_append(&b1->bv_ga, blob_get(b2, i));

    بازگشت OK;
}

/*
 * Handle "list1 += list2".
 * Returns OK or FAIL.
 */
    ایستا صحیح
tv_op_list(typval_T *tv1, typval_T *tv2, char_u *op)
{
    اگر (*op != '+' || tv2->v_type != VAR_LIST)
	بازگشت FAIL;

    // List += List
    اگر (tv2->vval.v_list == پوچ)
	بازگشت OK;

    اگر (tv1->vval.v_list == پوچ)
    {
	tv1->vval.v_list = tv2->vval.v_list;
	++tv1->vval.v_list->lv_refcount;
    }
    وگرنه
	list_extend(tv1->vval.v_list, tv2->vval.v_list, پوچ);

    بازگشت OK;
}

/*
 * Handle number operations:
 *	nr += nr , nr -= nr , nr *=nr , nr /= nr , nr %= nr
 *
 * Returns OK or FAIL.
 */
    ایستا صحیح
tv_op_عدد(typval_T *tv1, typval_T *tv2, char_u *op)
{
    varnumber_T	n;
    صحیح		failed = FALSE;

    n = tv_get_عدد(tv1);
    اگر (tv2->v_type == VAR_FLOAT)
    {
	float_T f = n;

	اگر (*op == '%')
	    بازگشت FAIL;
	انتخاب (*op)
	{
	    حالت '+': f += tv2->vval.v_float; بشکن;
	    حالت '-': f -= tv2->vval.v_float; بشکن;
	    حالت '*': f *= tv2->vval.v_float; بشکن;
	    حالت '/': f /= tv2->vval.v_float; بشکن;
	}
	clear_tv(tv1);
	tv1->v_type = VAR_FLOAT;
	tv1->vval.v_float = f;
    }
    وگرنه
    {
	انتخاب (*op)
	{
	    حالت '+': n += tv_get_عدد(tv2); بشکن;
	    حالت '-': n -= tv_get_عدد(tv2); بشکن;
	    حالت '*': n *= tv_get_عدد(tv2); بشکن;
	    حالت '/': n = رقم_divide(n, tv_get_عدد(tv2), &failed); بشکن;
	    حالت '%': n = رقم_modulus(n, tv_get_عدد(tv2), &failed); بشکن;
	}
	clear_tv(tv1);
	tv1->v_type = VAR_عدد;
	tv1->vval.v_عدد = n;
    }

    بازگشت failed ? FAIL : OK;
}

/*
 * Append string "s2" to the string in "tv1".
 * Returns OK if "tv1" was grown in place, FAIL otherwise.
 */
    ایستا صحیح
grow_string_tv(typval_T *tv1, char_u *s2)
{
    اگر (tv1->v_type != VAR_STRING || tv1->vval.v_string == پوچ)
	بازگشت FAIL;

    size_t len1 = STRLEN(tv1->vval.v_string);
    size_t len2 = STRLEN(s2);
    char_u *p = vim_realloc(tv1->vval.v_string, len1 + len2 + 1);

    اگر (p == پوچ)
	بازگشت FAIL;

    mch_memmove(p + len1, s2, len2 + 1);
    tv1->vval.v_string = p;
    بازگشت OK;
}

/*
 * Handle "str1 .= str2"
 * Returns OK or FAIL.
 */
    ایستا صحیح
tv_op_string(typval_T *tv1, typval_T *tv2, char_u *op UNUSED)
{
    char_u	numbuf[NUMBUFLEN];
    char_u	*s;
    char_u	*s2;

    اگر (tv2->v_type == VAR_FLOAT)
	بازگشت FAIL;

    // str .= str
    s2 = tv_get_string_buf(tv2, numbuf);
    اگر (grow_string_tv(tv1, s2) == OK)
	بازگشت OK;

    s = tv_get_string(tv1);
    s = concat_str(s, s2);
    clear_tv(tv1);
    tv1->v_type = VAR_STRING;
    tv1->vval.v_string = s;

    بازگشت OK;
}

/*
 * Handle "tv1 += tv2", "tv1 -= tv2", "tv1 *= tv2", "tv1 /= tv2", "tv1 %= tv2"
 * and "tv1 .= tv2"
 * Returns OK or FAIL.
 */
    ایستا صحیح
tv_op_nr_or_string(typval_T *tv1, typval_T *tv2, char_u *op)
{
    اگر (tv2->v_type == VAR_LIST)
	بازگشت FAIL;

    اگر (vim_strchr((char_u *)"+-*/%", *op) != پوچ)
	بازگشت tv_op_عدد(tv1, tv2, op);

    بازگشت tv_op_string(tv1, tv2, op);
}

/*
 * Handle "f1 += f2", "f1 -= f2", "f1 *= f2", "f1 /= f2".
 * Returns OK or FAIL.
 */
    ایستا صحیح
tv_op_float(typval_T *tv1, typval_T *tv2, char_u *op)
{
    float_T f;

    اگر (*op == '%' || *op == '.'
	    || (tv2->v_type != VAR_FLOAT
		&& tv2->v_type != VAR_عدد
		&& tv2->v_type != VAR_STRING))
	بازگشت FAIL;

    اگر (tv2->v_type == VAR_FLOAT)
	f = tv2->vval.v_float;
    وگرنه
	f = tv_get_عدد(tv2);
    انتخاب (*op)
    {
	حالت '+': tv1->vval.v_float += f; بشکن;
	حالت '-': tv1->vval.v_float -= f; بشکن;
	حالت '*': tv1->vval.v_float *= f; بشکن;
	حالت '/': tv1->vval.v_float /= f; بشکن;
    }

    بازگشت OK;
}

/*
 * Handle "tv1 += tv2", "tv1 -= tv2", "tv1 *= tv2", "tv1 /= tv2", "tv1 %= tv2"
 * and "tv1 .= tv2"
 * Returns OK or FAIL.
 */
    صحیح
tv_op(typval_T *tv1, typval_T *tv2, char_u *op)
{
    // Can't do anything with a Funcref or Dict on the right.
    // v:true and friends only work with "..=".
    اگر (tv2->v_type == VAR_FUNC || tv2->v_type == VAR_DICT
		|| ((tv2->v_type == VAR_BOOL || tv2->v_type == VAR_SPECIAL)
							&& *op != '.'))
    {
	semsg(_(e_wrong_variable_type_for_str_equal), op);
	بازگشت FAIL;
    }

    صحیح retval = FAIL;
    انتخاب (tv1->v_type)
    {
	حالت VAR_UNKNOWN:
	حالت VAR_ANY:
	حالت VAR_VOID:
	حالت VAR_DICT:
	حالت VAR_FUNC:
	حالت VAR_PARTIAL:
	حالت VAR_BOOL:
	حالت VAR_SPECIAL:
	حالت VAR_JOB:
	حالت VAR_CHANNEL:
	حالت VAR_INSTR:
	حالت VAR_OBJECT:
	حالت VAR_CLASS:
	حالت VAR_TYPEALIAS:
	حالت VAR_TUPLE:
	    بشکن;

	حالت VAR_BLOB:
	    retval = tv_op_blob(tv1, tv2, op);
	    بشکن;

	حالت VAR_LIST:
	    retval = tv_op_list(tv1, tv2, op);
	    بشکن;

	حالت VAR_عدد:
	حالت VAR_STRING:
	    retval = tv_op_nr_or_string(tv1, tv2, op);
	    بشکن;

	حالت VAR_FLOAT:
	    retval = tv_op_float(tv1, tv2, op);
	    بشکن;
    }

    اگر (retval != OK)
	semsg(_(e_wrong_variable_type_for_str_equal), op);

    بازگشت retval;
}

/*
 * Evaluate the expression used in a ":for var in expr" command.
 * "arg" points to "var".
 * Set "*errp" to TRUE for an error, FALSE otherwise;
 * Return a pointer that holds the info.  Null when there is an error.
 */
    تهی *
eval_for_سطر(
    char_u	*arg,
    صحیح		*errp,
    exarg_T	*eap,
    evalarg_T	*evalarg)
{
    forinfo_T	*fi;
    char_u	*var_list_end;
    char_u	*عبارت;
    typval_T	tv;
    list_T	*l;
    tuple_T	*tuple;
    صحیح		skip = !(evalarg->eval_flags & EVAL_EVALUATE);

    *errp = TRUE;	// default: there is an error

    fi = ALLOC_CLEAR_ONE(forinfo_T);
    اگر (fi == پوچ)
	بازگشت پوچ;

    var_list_end = skip_var_list(arg, TRUE, &fi->fi_varcount,
						     &fi->fi_semicolon, FALSE);
    اگر (var_list_end == پوچ)
	بازگشت fi;

    عبارت = skipwhite_and_linebreak(var_list_end, evalarg);
    اگر (عبارت[0] != 'i' || عبارت[1] != 'n'
				  || !(عبارت[2] == NUL || VIM_ISWHITE(عبارت[2])))
    {
	اگر (in_vim9script() && *عبارت == ':' && عبارت != var_list_end)
	    semsg(_(e_no_white_space_allowed_before_colon_str), عبارت);
	وگرنه
	    emsg(_(e_missing_in_after_for));
	بازگشت fi;
    }

    اگر (skip)
	++emsg_skip;
    عبارت = skipwhite_and_linebreak(عبارت + 2, evalarg);
    اگر (eval0(عبارت, &tv, eap, evalarg) == OK)
    {
	*errp = FALSE;
	اگر (!skip)
	{
	    اگر (tv.v_type == VAR_LIST)
	    {
		l = tv.vval.v_list;
		اگر (l == پوچ)
		{
		    // a null list is like an empty list: do nothing
		    clear_tv(&tv);
		}
		وگرنه
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
	    وگرنه اگر (tv.v_type == VAR_TUPLE)
	    {
		tuple = tv.vval.v_tuple;
		اگر (tuple == پوچ)
		{
		    // a null tuple is like an empty tuple: do nothing
		    clear_tv(&tv);
		}
		وگرنه
		{
		    // No need to increment the refcount, it's already set for
		    // the tuple being used in "tv".
		    fi->fi_tuple = tuple;
		    fi->fi_tuple_idx = 0;
		}
	    }
	    وگرنه اگر (tv.v_type == VAR_BLOB)
	    {
		fi->fi_bi = 0;
		اگر (tv.vval.v_blob != پوچ)
		{
		    // Make a copy, so that the iteration still works when the
		    // blob is changed.
		    fi->fi_blob = blob_copy(tv.vval.v_blob);
		}
		clear_tv(&tv);
	    }
	    وگرنه اگر (tv.v_type == VAR_STRING)
	    {
		fi->fi_byte_idx = 0;
		fi->fi_string = tv.vval.v_string;
		tv.vval.v_string = پوچ;
		اگر (fi->fi_string == پوچ)
		    fi->fi_string = vim_strsave((char_u *)"");
	    }
	    وگرنه
	    {
		emsg(_(e_string_list_tuple_or_blob_required));
		clear_tv(&tv);
	    }
	}
    }
    اگر (skip)
	--emsg_skip;
    fi->fi_break_شمار = evalarg->eval_break_شمار;

    بازگشت fi;
}

/*
 * Used when looping over a :for line, skip the "in expr" part.
 */
    تهی
skip_for_سطرها(تهی *fi_void, evalarg_T *evalarg)
{
    forinfo_T	*fi = (forinfo_T *)fi_void;
    صحیح		i;

    برای (i = 0; i < fi->fi_break_شمار; ++i)
	eval_next_سطر(پوچ, evalarg);
}

/*
 * Use the first item in a ":for" list.  Advance to the next.
 * Assign the values to the variable (list).  "arg" points to the first one.
 * Return TRUE when a valid item was found, FALSE when at end of list or
 * something wrong.
 */
    صحیح
next_for_item(تهی *fi_void, char_u *arg)
{
    forinfo_T	*fi = (forinfo_T *)fi_void;
    صحیح		result;
    صحیح		flag = ASSIGN_FOR_LOOP | (in_vim9script()
			 ? (ASSIGN_FINAL
			     // first round: error if variable exists
			     | (fi->fi_bi == 0 ? 0 : ASSIGN_DECL)
			     | ASSIGN_NO_MEMBER_TYPE
			     | ASSIGN_UPDATE_BLOCK_ID)
			 : 0);
    listitem_T	*item;
    صحیح		skip_assign = in_vim9script() && arg[0] == '_'
						      && !eval_isnamec(arg[1]);

    اگر (fi->fi_blob != پوچ)
    {
	typval_T	tv;

	اگر (fi->fi_bi >= blob_len(fi->fi_blob))
	    بازگشت FALSE;
	tv.v_type = VAR_عدد;
	tv.v_lock = VAR_FIXED;
	tv.vval.v_عدد = blob_get(fi->fi_blob, fi->fi_bi);
	++fi->fi_bi;
	اگر (skip_assign)
	    بازگشت TRUE;
	بازگشت ex_let_vars(arg, &tv, TRUE, fi->fi_semicolon,
					    fi->fi_varcount, flag, پوچ) == OK;
    }

    اگر (fi->fi_string != پوچ)
    {
	typval_T	tv;
	صحیح		len;

	len = mb_ptr2len(fi->fi_string + fi->fi_byte_idx);
	اگر (len == 0)
	    بازگشت FALSE;
	tv.v_type = VAR_STRING;
	tv.v_lock = VAR_FIXED;
	tv.vval.v_string = vim_strnsave(fi->fi_string + fi->fi_byte_idx, len);
	fi->fi_byte_idx += len;
	++fi->fi_bi;
	اگر (skip_assign)
	    result = TRUE;
	وگرنه
	    result = ex_let_vars(arg, &tv, TRUE, fi->fi_semicolon,
					    fi->fi_varcount, flag, پوچ) == OK;
	vim_free(tv.vval.v_string);
	بازگشت result;
    }

    اگر (fi->fi_tuple != پوچ)
    {
	typval_T	tv;

	اگر (fi->fi_tuple_idx >= TUPLE_LEN(fi->fi_tuple))
	    بازگشت FALSE;

	copy_tv(TUPLE_ITEM(fi->fi_tuple, fi->fi_tuple_idx), &tv);
	++fi->fi_tuple_idx;
	++fi->fi_bi;
	اگر (skip_assign)
	    result = TRUE;
	وگرنه
	    result = ex_let_vars(arg, &tv, TRUE, fi->fi_semicolon,
					    fi->fi_varcount, flag, پوچ) == OK;
	clear_tv(&tv);
	بازگشت result;
    }

    item = fi->fi_lw.lw_item;
    اگر (item == پوچ)
	result = FALSE;
    وگرنه
    {
	fi->fi_lw.lw_item = item->li_next;
	++fi->fi_bi;
	اگر (skip_assign)
	    result = TRUE;
	وگرنه
	    result = (ex_let_vars(arg, &item->li_tv, TRUE, fi->fi_semicolon,
					   fi->fi_varcount, flag, پوچ) == OK);
    }
    بازگشت result;
}

/*
 * Free the structure used to store info used by ":for".
 */
    تهی
free_for_info(تهی *fi_void)
{
    forinfo_T    *fi = (forinfo_T *)fi_void;

    اگر (fi == پوچ)
	بازگشت;
    اگر (fi->fi_list != پوچ)
    {
	list_rem_watch(fi->fi_list, &fi->fi_lw);
	list_unref(fi->fi_list);
    }
    وگرنه اگر (fi->fi_blob != پوچ)
	blob_unref(fi->fi_blob);
    وگرنه اگر (fi->fi_tuple != پوچ)
	tuple_unref(fi->fi_tuple);
    وگرنه
	vim_free(fi->fi_string);
    vim_free(fi);
}

    تهی
set_context_for_expression(
    expand_T	*xp,
    char_u	*arg,
    cmdidx_T	cmdidx)
{
    صحیح		has_عبارت = cmdidx != CMD_let && cmdidx != CMD_var;
    صحیح		c;
    char_u	*p;

    اگر (cmdidx == CMD_let || cmdidx == CMD_var
				 || cmdidx == CMD_const || cmdidx == CMD_final)
    {
	xp->xp_context = EXPAND_USER_VARS;
	اگر (vim_strpbrk(arg, (char_u *)"\"'+-*/%.=!?~|&$([<>,#") == پوچ)
	{
	    // ":let var1 var2 ...": find last space.
	    برای (p = arg + STRLEN(arg); p >= arg; )
	    {
		xp->xp_pattern = p;
		MB_PTR_BACK(arg, p);
		اگر (VIM_ISWHITE(*p))
		    بشکن;
	    }
	    بازگشت;
	}
    }
    وگرنه
	xp->xp_context = cmdidx == CMD_call ? EXPAND_FUNCTIONS
							  : EXPAND_EXPRESSION;
    وقتی ((xp->xp_pattern = vim_strpbrk(arg,
				  (char_u *)"\"'+-*/%.=!?~|&$([<>,#")) != پوچ)
    {
	c = *xp->xp_pattern;
	اگر (c == '&')
	{
	    c = xp->xp_pattern[1];
	    اگر (c == '&')
	    {
		++xp->xp_pattern;
		xp->xp_context = has_عبارت ? EXPAND_EXPRESSION : EXPAND_NOTHING;
	    }
	    وگرنه اگر (c != ' ')
	    {
		xp->xp_context = EXPAND_SETTINGS;
		اگر ((c == 'l' || c == 'g') && xp->xp_pattern[2] == ':')
		    xp->xp_pattern += 2;

	    }
	}
	وگرنه اگر (c == '$')
	{
	    // environment variable
	    xp->xp_context = EXPAND_ENV_VARS;
	}
	وگرنه اگر (c == '=')
	{
	    has_عبارت = TRUE;
	    xp->xp_context = EXPAND_EXPRESSION;
	}
	وگرنه اگر (c == '#'
		&& xp->xp_context == EXPAND_EXPRESSION)
	{
	    // Autoload function/variable contains '#'.
	    بشکن;
	}
	وگرنه اگر ((c == '<' || c == '#')
		&& xp->xp_context == EXPAND_FUNCTIONS
		&& vim_strchr(xp->xp_pattern, '(') == پوچ)
	{
	    // Function name can start with "<SNR>" and contain '#'.
	    بشکن;
	}
	وگرنه اگر (has_عبارت)
	{
	    اگر (c == '"')	    // string
	    {
		وقتی ((c = *++xp->xp_pattern) != NUL && c != '"')
		    اگر (c == '\\' && xp->xp_pattern[1] != NUL)
			++xp->xp_pattern;
		xp->xp_context = EXPAND_NOTHING;
	    }
	    وگرنه اگر (c == '\'')	    // literal string
	    {
		// Trick: '' is like stopping and starting a literal string.
		وقتی ((c = *++xp->xp_pattern) != NUL && c != '\'')
		    /* skip */ ;
		xp->xp_context = EXPAND_NOTHING;
	    }
	    وگرنه اگر (c == '|')
	    {
		اگر (xp->xp_pattern[1] == '|')
		{
		    ++xp->xp_pattern;
		    xp->xp_context = EXPAND_EXPRESSION;
		}
		وگرنه
		    xp->xp_context = EXPAND_COMMANDS;
	    }
	    وگرنه
		xp->xp_context = EXPAND_EXPRESSION;
	}
	وگرنه
	    // Doesn't look like something valid, expand as an expression
	    // anyway.
	    xp->xp_context = EXPAND_EXPRESSION;
	arg = xp->xp_pattern;
	اگر (*arg != NUL)
	    وقتی ((c = *++arg) != NUL && (c == ' ' || c == '\t'))
		/* skip */ ;
    }

    // ":exe one two" completes "two"
    اگر ((cmdidx == CMD_execute
		|| cmdidx == CMD_echo
		|| cmdidx == CMD_echon
		|| cmdidx == CMD_echomsg
		|| cmdidx == CMD_echoerr
		|| cmdidx == CMD_echoconsole
		|| cmdidx == CMD_echowindow)
	    && xp->xp_context == EXPAND_EXPRESSION)
    {
	برای (;;)
	{
	    char_u *n = skiptowhite(arg);

	    اگر (n == arg || IS_WHITE_OR_NUL(*skipwhite(n)))
		بشکن;
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
ایستا regprog_T    *eval_prog_cache = پوچ;
ایستا char_u	    *eval_prog_pat = پوچ;	// pattern it was compiled for
ایستا char_u	    *eval_prog_enc = پوچ;	// 'encoding' when compiled
ایستا long	    eval_prog_re;		// 'regexpengine' when compiled
// State sampled by eval_regcomp() for the eval_regfree() call that gives the
// program back: user code run in between, e.g. an object's string() method
// invoked by match() on a list, can change these options, and then the
// program must not be cached under the changed values.  eval_compile_enc is
// only ever compared against p_enc, never dereferenced.
ایستا long	    eval_compile_re;
ایستا char_u	    *eval_compile_enc;
ایستا صحیح	    eval_prog_busy = 0;	// programs eval_regcomp() handed out
					// that were not given back yet
ایستا صحیح	    eval_prog_reentered = FALSE;  // eval_regcomp() ran while
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
    ایستا bool
eval_prog_volatile(char_u *pat)
{
    char_u *p;

    اگر (STRNCMP(pat, "\\%#=", 4) == 0)
	بازگشت true;
    برای (p = pat; *p != NUL; ++p)
	اگر (*p == '~'
		|| (p[0] == '[' && (p[1] == ':' || p[1] == '=' || p[1] == '.'))
		|| (p[0] == '%' && (p[1] == '.'
			|| ((p[1] == '<' || p[1] == '>') && p[2] == '.'))))
	    بازگشت true;
    بازگشت false;
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
    اگر (eval_prog_busy > 0)
	// The samples above no longer describe the program already handed
	// out: its eval_regfree() must drop it instead of caching it.
	eval_prog_reentered = TRUE;

    اگر (eval_prog_cache != پوچ
	    && *p_cpo == NUL
	    && eval_prog_cache->re_flags == RE_MAGIC + RE_STRING
	    && eval_prog_re == p_re
	    && STRCMP(eval_prog_pat, pat) == 0
	    && STRCMP(eval_prog_enc, p_enc) == 0)
    {
	prog = eval_prog_cache;
	// The caller now owns the program, eval_regfree() adopts the
	// program the caller ends up with.
	eval_prog_cache = پوچ;
    }
    وگرنه
	prog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
    اگر (prog != پوچ)
	++eval_prog_busy;	// paired with the eval_regfree() call
    بازگشت prog;
}

/*
 * Free program "prog", obtained with eval_regcomp() for pattern "pat", by
 * keeping it in the cache for the next use.  "pat" must be the same string
 * the program was compiled for and must still be valid.
 */
    تهی
eval_regfree(char_u *pat, regprog_T *prog)
{
    صحیح	    reentered = eval_prog_reentered;

    اگر (eval_prog_busy > 0 && --eval_prog_busy == 0)
	eval_prog_reentered = FALSE;
    اگر (prog == پوچ)
	بازگشت;
    اگر (reentered || *p_cpo != NUL
	    || eval_compile_re != p_re || eval_compile_enc != p_enc)
    {
	// 'cpoptions' is not empty, an option in the cache key changed, or
	// another program was handed out while the caller was using this
	// one: the samples may not describe this program, do not cache it.
	vim_regfree(prog);
	بازگشت;
    }
    اگر (eval_prog_pat == پوچ || STRCMP(eval_prog_pat, pat) != 0
	    || STRCMP(eval_prog_enc, p_enc) != 0)
    {
	char_u	*pat_copy;
	char_u	*enc_copy;

	// A cached pattern is known not to be volatile, thus the scan is
	// only needed when installing a new key.
	اگر (eval_prog_volatile(pat))
	{
	    // compiling depends on state the cache key does not cover
	    vim_regfree(prog);
	    بازگشت;
	}
	pat_copy = vim_strsave(pat);
	enc_copy = vim_strsave(p_enc);
	اگر (pat_copy == پوچ || enc_copy == پوچ)
	{
	    vim_free(pat_copy);
	    vim_free(enc_copy);
	    vim_regfree(prog);
	    بازگشت;
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

#اگر defined(EXITFREE)
    تهی
free_eval_regcomp_cache(تهی)
{
    vim_regfree(eval_prog_cache);
    eval_prog_cache = پوچ;
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
    صحیح
pattern_match(char_u *pat, char_u *text, صحیح ic)
{
    صحیح		matches = FALSE;
    char_u	*save_cpo;
    regmatch_T	regmatch;

    // avoid 'l' flag in 'cpoptions'
    save_cpo = p_cpo;
    p_cpo = empty_option;
    regmatch.regprog = eval_regcomp(pat);
    اگر (regmatch.regprog != پوچ)
    {
	regmatch.rm_ic = ic;
	matches = vim_regexec_nl(&regmatch, text, (colnr_T)0);
	eval_regfree(pat, regmatch.regprog);
    }
    p_cpo = save_cpo;
    بازگشت matches;
}

/*
 * Handle a name followed by "(".  Both for just "name(arg)" and for
 * "expr->name(arg)".
 * Returns OK or FAIL.
 */
    ایستا صحیح
eval_func(
	char_u	    **arg,	// points to "(", will be advanced
	evalarg_T   *evalarg,
	char_u	    *name,
	صحیح	    name_len,
	typval_T    *rettv,
	صحیح	    flags,
	typval_T    *basetv)	// "expr" for "expr->name(arg)"
{
    صحیح		evaluate = flags & EVAL_EVALUATE;
    char_u	*s = name;
    صحیح		len = name_len;
    partial_T	*partial;
    صحیح		ret = OK;
    type_T	*type = پوچ;
    صحیح		found_var = FALSE;

    اگر (!evaluate)
	check_vars(s, len);

    // If "s" is the name of a variable of type VAR_FUNC
    // use its contents.
    s = deref_func_name(s, &len, &partial,
		 in_vim9script() ? &type : پوچ, !evaluate, FALSE, &found_var);

    // Need to make a copy, in case evaluating the arguments makes
    // the name invalid.
    s = vim_strsave(s);
    اگر (s == پوچ || (evaluate && *s == NUL))
	ret = FAIL;
    وگرنه
    {
	funcexe_T funcexe;

	// Invoke the function.
	CLEAR_FIELD(funcexe);
	funcexe.fe_firstline = curwin->w_cursor.lnum;
	funcexe.fe_lastline = curwin->w_cursor.lnum;
	funcexe.fe_evaluate = evaluate;
	funcexe.fe_partial = partial;
	اگر (partial != پوچ)
	{
	    funcexe.fe_object = partial->pt_obj;
	    اگر (funcexe.fe_object != پوچ)
		++funcexe.fe_object->obj_refcount;
	}
	funcexe.fe_basetv = basetv;
	funcexe.fe_check_type = type;
	funcexe.fe_found_var = found_var;
	اگر (evalarg != پوچ)
	    funcexe.fe_cctx = evalarg->eval_cctx;
	ret = get_func_tv(s, len, rettv, arg, evalarg, &funcexe);
    }
    vim_free(s);

    // If evaluate is FALSE rettv->v_type was not set in
    // get_func_tv, but it's needed in handle_subscript() to parse
    // what follows. So set it here.
    اگر (rettv->v_type == VAR_UNKNOWN && !evaluate && **arg == '(')
    {
	rettv->vval.v_string = پوچ;
	rettv->v_type = VAR_FUNC;
    }

    // Stop the expression evaluation when immediately
    // aborting on error, or when an interrupt occurred or
    // an exception was thrown but not caught.
    اگر (evaluate && aborting())
    {
	اگر (ret == OK)
	    clear_tv(rettv);
	ret = FAIL;
    }
    بازگشت ret;
}

/*
 * After a NL, skip over empty lines and comment-only lines.
 */
    ایستا char_u *
newline_skip_comments(char_u *arg)
{
    char_u *p = arg + 1;

    برای (;;)
    {
	p = skipwhite(p);

	اگر (*p == NUL)
	    بشکن;
	اگر (vim9_comment_start(p))
	{
	    char_u *nl = vim_strchr(p, NL);

	    اگر (nl == پوچ)
		بشکن;
	    p = nl;
	}
	اگر (*p != NL)
	    بشکن;
	++p;  // skip another NL
    }
    بازگشت p;
}

/*
 * Get the next line source line without advancing.  But do skip over comment
 * lines.
 * Only called for Vim9 script.
 */
    ایستا char_u *
getline_peek_skip_comments(evalarg_T *evalarg)
{
    برای (;;)
    {
	char_u *next = getline_peek(evalarg->eval_getline,
							 evalarg->eval_cookie);
	char_u *p;

	اگر (next == پوچ)
	    بشکن;
	p = skipwhite(next);
	اگر (*p != NUL && !vim9_comment_start(p))
	    بازگشت next;
	اگر (eval_next_سطر(پوچ, evalarg) == پوچ)
	    بشکن;
    }
    بازگشت پوچ;
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
eval_next_non_blank(char_u *arg, evalarg_T *evalarg, صحیح *getnext)
{
    char_u *p = skipwhite(arg);

    *getnext = FALSE;
    اگر (in_vim9script()
	    && evalarg != پوچ
	    && (evalarg->eval_cookie != پوچ || evalarg->eval_cctx != پوچ
								   || *p == NL)
	    && (*p == NUL || *p == NL
			     || (vim9_comment_start(p) && VIM_ISWHITE(p[-1]))))
    {
	char_u *next;

	اگر (*p == NL)
	    next = newline_skip_comments(p);
	وگرنه اگر (evalarg->eval_cookie != پوچ)
	    next = getline_peek_skip_comments(evalarg);
	وگرنه
	    next = peek_next_سطر_from_context(evalarg->eval_cctx);

	اگر (next != پوچ)
	{
	    *getnext = *p != NL;
	    بازگشت skipwhite(next);
	}
    }
    بازگشت p;
}

/*
 * To be called after eval_next_non_blank() sets "getnext" to TRUE.
 * Only called for Vim9 script.
 *
 * If "arg" is not NULL, then the caller should assign the return value to
 * "arg".
 */
    char_u *
eval_next_سطر(char_u *arg, evalarg_T *evalarg)
{
    garray_T	*gap = &evalarg->eval_ga;
    char_u	*سطر;

    اگر (arg != پوچ)
    {
	اگر (*arg == NL)
	    بازگشت newline_skip_comments(arg);
	// Truncate before a trailing comment, so that concatenating the lines
	// won't turn the rest into a comment.
	اگر (*skipwhite(arg) == '#')
	    *arg = NUL;
    }

    اگر (evalarg->eval_cookie != پوچ)
	سطر = evalarg->eval_getline(0, evalarg->eval_cookie, 0,
							   GETLINE_CONCAT_ALL);
    وگرنه
	سطر = next_سطر_from_context(evalarg->eval_cctx, TRUE);
    اگر (سطر == پوچ)
	بازگشت پوچ;

    ++evalarg->eval_break_شمار;
    اگر (gap->ga_itemsize > 0 && ga_grow(gap, 1) == OK)
    {
	char_u *p = skipwhite(سطر);

	// Going to concatenate the lines after parsing.  For an empty or
	// comment line use an empty string.
	اگر (*p == NUL || vim9_comment_start(p))
	{
	    vim_free(سطر);
	    سطر = vim_strsave((char_u *)"");
	}

	((char_u **)gap->ga_data)[gap->ga_len] = سطر;
	++gap->ga_len;
    }
    وگرنه اگر (evalarg->eval_cookie != پوچ)
    {
	free_eval_tofree_later(evalarg);
	evalarg->eval_tofree = سطر;
    }

    // Advanced to the next line, "arg" no longer points into the previous
    // line.  The caller assigns the return value to "arg".
    // If "arg" is NULL, then the return value is discarded.  In that case,
    // "arg" still points to the previous line.  So don't reset
    // "eval_using_cmdline".
    اگر (arg != پوچ)
	evalarg->eval_using_cmdline = FALSE;
    بازگشت skipwhite(سطر);
}

/*
 * Call eval_next_non_blank() and get the next line if needed.
 */
    char_u *
skipwhite_and_linebreak(char_u *arg, evalarg_T *evalarg)
{
    صحیح	    getnext;
    char_u  *p = skipwhite_and_nl(arg);

    اگر (evalarg == پوچ)
	بازگشت skipwhite(arg);
    eval_next_non_blank(p, evalarg, &getnext);
    اگر (getnext)
	بازگشت eval_next_سطر(arg, evalarg);
    بازگشت p;
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
    صحیح
eval0(
    char_u	*arg,
    typval_T	*rettv,
    exarg_T	*eap,
    evalarg_T	*evalarg)
{
    بازگشت eval0_retarg(arg, rettv, eap, evalarg, پوچ);
}

/*
 * If "arg" is a simple function call without arguments then call it and return
 * the result.  Otherwise return NOTDONE.
 */
    صحیح
may_call_simple_func(
    char_u	*arg,
    typval_T	*rettv)
{
    char_u  *parens = (char_u *)strstr((نویسه *)arg, "()");
    صحیح	    r = NOTDONE;

    // If the expression is "FuncName()" then we can skip a lot of overhead.
    اگر (parens != پوچ && *skipwhite(parens + 2) == NUL)
    {
	char_u *p = STRNCMP(arg, "<SNR>", 5) == 0 ? skipdigits(arg + 5) : arg;

	اگر (to_name_end(p, TRUE) == parens)
	    r = call_simple_func(arg, (size_t)(parens - arg), rettv);
    }
    بازگشت r;
}

/*
 * Handle zero level expression with optimization for a simple function call.
 * Same arguments and return value as eval0().
 */
    ایستا صحیح
eval0_simple_funccal(
    char_u	*arg,
    typval_T	*rettv,
    exarg_T	*eap,
    evalarg_T	*evalarg)
{
    صحیح	    r = may_call_simple_func(arg, rettv);

    اگر (r == NOTDONE)
	r = eval0_retarg(arg, rettv, eap, evalarg, پوچ);
    بازگشت r;
}

/*
 * Like eval0() but when "retarg" is not NULL store the pointer to after the
 * expression and don't check what comes after the expression.
 */
    صحیح
eval0_retarg(
    char_u	*arg,
    typval_T	*rettv,
    exarg_T	*eap,
    evalarg_T	*evalarg,
    char_u	**retarg)
{
    صحیح		ret;
    char_u	*p;
    char_u	*عبارت_end;
    صحیح		did_emsg_before = did_emsg;
    صحیح		called_emsg_before = called_emsg;
    صحیح		check_for_end = retarg == پوچ;
    صحیح		end_error = FALSE;

    p = skipwhite(arg);
    ret = eval1(&p, rettv, evalarg);

    اگر (ret != FAIL)
    {
	عبارت_end = p;
	p = skipwhite(p);

	// In Vim9 script a command block is not split at NL characters for
	// commands using an expression argument.  Skip over a '#' comment to
	// check for a following NL.  Require white space before the '#'.
	اگر (in_vim9script() && p > عبارت_end && retarg == پوچ)
	    وقتی (*p == '#')
	    {
		char_u *nl = vim_strchr(p, NL);

		اگر (nl == پوچ)
		    بشکن;
		p = skipwhite(nl + 1);
		اگر (eap != پوچ && *p != NUL)
		    eap->nextcmd = p;
		check_for_end = FALSE;
	    }

	اگر (check_for_end)
	    end_error = !ends_excmd2(arg, p);
    }

    اگر (ret == FAIL || end_error)
    {
	اگر (ret != FAIL)
	    clear_tv(rettv);
	/*
	 * Report the invalid expression unless the expression evaluation has
	 * been cancelled due to an aborting error, an interrupt, or an
	 * exception, or we already gave a more specific error.
	 * Also check called_emsg for when using assert_fails().
	 */
	اگر (!aborting()
		&& did_emsg == did_emsg_before
		&& called_emsg == called_emsg_before
		&& (!in_vim9script() || !vim9_bad_comment(p)))
	{
	    اگر (end_error)
		semsg(_(e_trailing_characters_str), p);
	    وگرنه
		semsg(_(e_invalid_expression_str), arg);
	}

	اگر (eap != پوچ && p != پوچ)
	{
	    // Some of the expression may not have been consumed.
	    // Only execute a next command if it cannot be a "||" operator.
	    // The next command may be "catch".
	    char_u *nextcmd = check_nextcmd(p);
	    اگر (nextcmd != پوچ && *nextcmd != '|')
		eap->nextcmd = nextcmd;
	}
	بازگشت FAIL;
    }

    اگر (retarg != پوچ)
	*retarg = p;
    وگرنه اگر (check_for_end && eap != پوچ)
	set_nextcmd(eap, p);

    بازگشت ret;
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
    صحیح
eval1(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    char_u  *p;
    صحیح	    getnext;

    CLEAR_POINTER(rettv);

    /*
     * Get the first variable.
     */
    اگر (eval2(arg, rettv, evalarg) == FAIL)
	بازگشت FAIL;

    p = eval_next_non_blank(*arg, evalarg, &getnext);
    اگر (*p == '?')
    {
	صحیح		op_falsy = p[1] == '?';
	صحیح		result;
	typval_T	var2;
	evalarg_T	*evalarg_used = evalarg;
	evalarg_T	local_evalarg;
	صحیح		orig_flags;
	صحیح		evaluate;
	صحیح		vim9script = in_vim9script();

	اگر (evalarg == پوچ)
	{
	    init_evalarg(&local_evalarg);
	    evalarg_used = &local_evalarg;
	}
	orig_flags = evalarg_used->eval_flags;
	evaluate = evalarg_used->eval_flags & EVAL_EVALUATE;

	اگر (getnext)
	    *arg = eval_next_سطر(*arg, evalarg_used);
	وگرنه
	{
	    اگر (evaluate && vim9script && !VIM_ISWHITE(p[-1]))
	    {
		error_white_both(p, op_falsy ? 2 : 1);
		clear_tv(rettv);
		بازگشت FAIL;
	    }
	    *arg = p;
	}

	result = FALSE;
	اگر (evaluate)
	{
	    صحیح		error = FALSE;

	    اگر (op_falsy)
	    {
		// Is this typeval supported with the falsy operator?
		اگر (check_typval_is_مقدار(rettv) == FAIL)
		{
		    clear_tv(rettv);
		    بازگشت FAIL;
		}
		result = tv2bool(rettv);
	    }
	    وگرنه اگر (vim9script)
		result = tv_get_bool_chk(rettv, &error);
	    وگرنه اگر (tv_get_عدد_chk(rettv, &error) != 0)
		result = TRUE;
	    اگر (error || !op_falsy || !result)
		clear_tv(rettv);
	    اگر (error)
		بازگشت FAIL;
	}

	/*
	 * Get the second variable.  Recursive!
	 */
	اگر (op_falsy)
	    ++*arg;
	اگر (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[1]))
	{
	    error_white_both(*arg - (op_falsy ? 1 : 0), op_falsy ? 2 : 1);
	    clear_tv(rettv);
	    بازگشت FAIL;
	}
	*arg = skipwhite_and_linebreak(*arg + 1, evalarg_used);
	evalarg_used->eval_flags = (op_falsy ? !result : result)
				  ? orig_flags : (orig_flags & ~EVAL_EVALUATE);
	اگر (eval1(arg, &var2, evalarg_used) == FAIL)
	{
	    evalarg_used->eval_flags = orig_flags;
	    بازگشت FAIL;
	}
	اگر (!op_falsy || !result)
	    *rettv = var2;

	اگر (!op_falsy)
	{
	    /*
	     * Check for the ":".
	     */
	    p = eval_next_non_blank(*arg, evalarg_used, &getnext);
	    اگر (*p != ':')
	    {
		emsg(_(e_missing_colon_after_questionmark));
		اگر (evaluate && result)
		    clear_tv(rettv);
		evalarg_used->eval_flags = orig_flags;
		بازگشت FAIL;
	    }
	    اگر (getnext)
		*arg = eval_next_سطر(*arg, evalarg_used);
	    وگرنه
	    {
		اگر (evaluate && vim9script && !VIM_ISWHITE(p[-1]))
		{
		    error_white_both(p, 1);
		    clear_tv(rettv);
		    evalarg_used->eval_flags = orig_flags;
		    بازگشت FAIL;
		}
		*arg = p;
	    }

	    /*
	     * Get the third variable.  Recursive!
	     */
	    اگر (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[1]))
	    {
		error_white_both(*arg, 1);
		clear_tv(rettv);
		evalarg_used->eval_flags = orig_flags;
		بازگشت FAIL;
	    }
	    *arg = skipwhite_and_linebreak(*arg + 1, evalarg_used);
	    evalarg_used->eval_flags = !result ? orig_flags
					       : (orig_flags & ~EVAL_EVALUATE);
	    اگر (eval1(arg, &var2, evalarg_used) == FAIL)
	    {
		اگر (evaluate && result)
		    clear_tv(rettv);
		evalarg_used->eval_flags = orig_flags;
		بازگشت FAIL;
	    }
	    اگر (evaluate && !result)
		*rettv = var2;
	}

	اگر (evalarg == پوچ)
	    clear_evalarg(&local_evalarg, پوچ);
	وگرنه
	    evalarg->eval_flags = orig_flags;
    }

    بازگشت OK;
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
    ایستا صحیح
eval2(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    char_u	*p;
    صحیح		getnext;

    /*
     * Get the first expression.
     */
    اگر (eval3(arg, rettv, evalarg) == FAIL)
	بازگشت FAIL;

    /*
     * Handle the  "||" operator.
     */
    p = eval_next_non_blank(*arg, evalarg, &getnext);
    اگر (p[0] == '|' && p[1] == '|')
    {
	evalarg_T   *evalarg_used = evalarg;
	evalarg_T   local_evalarg;
	صحیح	    evaluate;
	صحیح	    orig_flags;
	long	    result = FALSE;
	typval_T    var2;
	صحیح	    error = FALSE;
	صحیح	    vim9script = in_vim9script();

	اگر (evalarg == پوچ)
	{
	    init_evalarg(&local_evalarg);
	    evalarg_used = &local_evalarg;
	}
	orig_flags = evalarg_used->eval_flags;
	evaluate = orig_flags & EVAL_EVALUATE;
	اگر (evaluate)
	{
	    اگر (vim9script)
		result = tv_get_bool_chk(rettv, &error);
	    وگرنه اگر (tv_get_عدد_chk(rettv, &error) != 0)
		result = TRUE;
	    clear_tv(rettv);
	    اگر (error)
		بازگشت FAIL;
	}

	/*
	 * Repeat until there is no following "||".
	 */
	وقتی (p[0] == '|' && p[1] == '|')
	{
	    اگر (getnext)
		*arg = eval_next_سطر(*arg, evalarg_used);
	    وگرنه
	    {
		اگر (evaluate && vim9script && !VIM_ISWHITE(p[-1]))
		{
		    error_white_both(p, 2);
		    clear_tv(rettv);
		    بازگشت FAIL;
		}
		*arg = p;
	    }

	    /*
	     * Get the second variable.
	     */
	    اگر (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[2]))
	    {
		error_white_both(*arg, 2);
		clear_tv(rettv);
		بازگشت FAIL;
	    }
	    *arg = skipwhite_and_linebreak(*arg + 2, evalarg_used);
	    evalarg_used->eval_flags = !result ? orig_flags
					       : (orig_flags & ~EVAL_EVALUATE);
	    اگر (eval3(arg, &var2, evalarg_used) == FAIL)
		بازگشت FAIL;

	    /*
	     * Compute the result.
	     */
	    اگر (evaluate && !result)
	    {
		اگر (vim9script)
		    result = tv_get_bool_chk(&var2, &error);
		وگرنه اگر (tv_get_عدد_chk(&var2, &error) != 0)
		    result = TRUE;
		clear_tv(&var2);
		اگر (error)
		    بازگشت FAIL;
	    }
	    اگر (evaluate)
	    {
		اگر (vim9script)
		{
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_عدد = result ? VVAL_TRUE : VVAL_FALSE;
		}
		وگرنه
		{
		    rettv->v_type = VAR_عدد;
		    rettv->vval.v_عدد = result;
		}
	    }

	    p = eval_next_non_blank(*arg, evalarg_used, &getnext);
	}

	اگر (evalarg == پوچ)
	    clear_evalarg(&local_evalarg, پوچ);
	وگرنه
	    evalarg->eval_flags = orig_flags;
    }

    بازگشت OK;
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
    ایستا صحیح
eval3(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    char_u	*p;
    صحیح		getnext;

    /*
     * Get the first expression.
     */
    اگر (eval4(arg, rettv, evalarg) == FAIL)
	بازگشت FAIL;

    /*
     * Handle the "&&" operator.
     */
    p = eval_next_non_blank(*arg, evalarg, &getnext);
    اگر (p[0] == '&' && p[1] == '&')
    {
	evalarg_T   *evalarg_used = evalarg;
	evalarg_T   local_evalarg;
	صحیح	    orig_flags;
	صحیح	    evaluate;
	long	    result = TRUE;
	typval_T    var2;
	صحیح	    error = FALSE;
	صحیح	    vim9script = in_vim9script();

	اگر (evalarg == پوچ)
	{
	    init_evalarg(&local_evalarg);
	    evalarg_used = &local_evalarg;
	}
	orig_flags = evalarg_used->eval_flags;
	evaluate = orig_flags & EVAL_EVALUATE;
	اگر (evaluate)
	{
	    اگر (vim9script)
		result = tv_get_bool_chk(rettv, &error);
	    وگرنه اگر (tv_get_عدد_chk(rettv, &error) == 0)
		result = FALSE;
	    clear_tv(rettv);
	    اگر (error)
		بازگشت FAIL;
	}

	/*
	 * Repeat until there is no following "&&".
	 */
	وقتی (p[0] == '&' && p[1] == '&')
	{
	    اگر (getnext)
		*arg = eval_next_سطر(*arg, evalarg_used);
	    وگرنه
	    {
		اگر (evaluate && vim9script && !VIM_ISWHITE(p[-1]))
		{
		    error_white_both(p, 2);
		    clear_tv(rettv);
		    بازگشت FAIL;
		}
		*arg = p;
	    }

	    /*
	     * Get the second variable.
	     */
	    اگر (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[2]))
	    {
		error_white_both(*arg, 2);
		clear_tv(rettv);
		بازگشت FAIL;
	    }
	    *arg = skipwhite_and_linebreak(*arg + 2, evalarg_used);
	    evalarg_used->eval_flags = result ? orig_flags
					      : (orig_flags & ~EVAL_EVALUATE);
	    CLEAR_FIELD(var2);
	    اگر (eval4(arg, &var2, evalarg_used) == FAIL)
		بازگشت FAIL;

	    /*
	     * Compute the result.
	     */
	    اگر (evaluate && result)
	    {
		اگر (vim9script)
		    result = tv_get_bool_chk(&var2, &error);
		وگرنه اگر (tv_get_عدد_chk(&var2, &error) == 0)
		    result = FALSE;
		clear_tv(&var2);
		اگر (error)
		    بازگشت FAIL;
	    }
	    اگر (evaluate)
	    {
		اگر (vim9script)
		{
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_عدد = result ? VVAL_TRUE : VVAL_FALSE;
		}
		وگرنه
		{
		    rettv->v_type = VAR_عدد;
		    rettv->vval.v_عدد = result;
		}
	    }

	    p = eval_next_non_blank(*arg, evalarg_used, &getnext);
	}

	اگر (evalarg == پوچ)
	    clear_evalarg(&local_evalarg, پوچ);
	وگرنه
	    evalarg->eval_flags = orig_flags;
    }

    بازگشت OK;
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
    ایستا صحیح
eval4(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    char_u	*p;
    صحیح		getnext;
    exprtype_T	type = عبارت_UNKNOWN;
    صحیح		len = 2;
    صحیح		type_is = FALSE;

    /*
     * Get the first expression.
     */
    اگر (eval5(arg, rettv, evalarg) == FAIL)
	بازگشت FAIL;

    p = eval_next_non_blank(*arg, evalarg, &getnext);

    type = get_compare_type(p, &len, &type_is);

    /*
     * If there is a comparative operator, use it.
     */
    اگر (type != عبارت_UNKNOWN)
    {
	typval_T    var2;
	صحیح	    ic;
	صحیح	    vim9script = in_vim9script();
	صحیح	    evaluate = evalarg == پوچ
				   ? 0 : (evalarg->eval_flags & EVAL_EVALUATE);
	long	    comp_lnum = SOURCING_LNUM;

	اگر (getnext)
	{
	    *arg = eval_next_سطر(*arg, evalarg);
	    p = *arg;
	}
	وگرنه اگر (evaluate && vim9script && !VIM_ISWHITE(**arg))
	{
	    error_white_both(*arg, len);
	    clear_tv(rettv);
	    بازگشت FAIL;
	}

	اگر (vim9script && type_is && (p[len] == '?' || p[len] == '#'))
	{
	    semsg(_(e_invalid_expression_str), p);
	    clear_tv(rettv);
	    بازگشت FAIL;
	}

	// extra question mark appended: ignore case
	اگر (p[len] == '?')
	{
	    ic = TRUE;
	    ++len;
	}
	// extra '#' appended: match case
	وگرنه اگر (p[len] == '#')
	{
	    ic = FALSE;
	    ++len;
	}
	// nothing appended: use 'ignorecase' if not in Vim script
	وگرنه
	    ic = vim9script ? FALSE : p_ic;

	/*
	 * Get the second variable.
	 */
	اگر (evaluate && vim9script && !IS_WHITE_OR_NUL(p[len]))
	{
	    error_white_both(p, len);
	    clear_tv(rettv);
	    بازگشت FAIL;
	}
	*arg = skipwhite_and_linebreak(p + len, evalarg);
	اگر (eval5(arg, &var2, evalarg) == FAIL)
	{
	    clear_tv(rettv);
	    بازگشت FAIL;
	}
	اگر (evaluate)
	{
	    صحیح ret;

	    // use the line of the comparison for messages
	    SOURCING_LNUM = comp_lnum;
	    اگر (vim9script && check_compare_types(type, rettv, &var2) == FAIL)
	    {
		ret = FAIL;
		clear_tv(rettv);
	    }
	    وگرنه
		ret = typval_compare(rettv, &var2, type, ic);
	    clear_tv(&var2);
	    بازگشت ret;
	}
    }

    بازگشت OK;
}

/*
 * Make a copy of blob "tv1" and append blob "tv2".
 */
    تهی
eval_addblob(typval_T *tv1, typval_T *tv2)
{
    blob_T  *b1 = tv1->vval.v_blob;
    blob_T  *b2 = tv2->vval.v_blob;
    blob_T  *b = blob_alloc();
    long    len1, len2;
    long    totallen;

    اگر (b == پوچ)
	بازگشت;

    len1 = blob_len(b1);
    len2 = blob_len(b2);
    totallen = len1 + len2;

    اگر (totallen > 0 && totallen <= INT_MAX
				    && ga_grow(&b->bv_ga, (صحیح)totallen) == OK)
    {
	اگر (len1 > 0)
	    mch_memmove((char_u *)b->bv_ga.ga_data,
		    (char_u *)b1->bv_ga.ga_data, (size_t)len1);
	اگر (len2 > 0)
	    mch_memmove((char_u *)b->bv_ga.ga_data + len1,
		    (char_u *)b2->bv_ga.ga_data, (size_t)len2);
	b->bv_ga.ga_len = (صحیح)totallen;
    }

    clear_tv(tv1);
    rettv_blob_set(tv1, b);
}

/*
 * Make a copy of list "tv1" and append list "tv2".
 */
    صحیح
eval_addlist(typval_T *tv1, typval_T *tv2)
{
    typval_T var3;

    // concatenate Lists
    اگر (list_concat(tv1->vval.v_list, tv2->vval.v_list, &var3) == FAIL)
    {
	clear_tv(tv1);
	clear_tv(tv2);
	بازگشت FAIL;
    }
    clear_tv(tv1);
    *tv1 = var3;
    بازگشت OK;
}

/*
 * Make a copy of tuple "tv1" and append tuple "tv2".
 */
    صحیح
eval_addtuple(typval_T *tv1, typval_T *tv2)
{
    صحیح		vim9script = in_vim9script();
    typval_T	var3;

    اگر (vim9script && tv1->vval.v_tuple != پوچ && tv2->vval.v_tuple != پوچ
	    && tv1->vval.v_tuple->tv_type != پوچ
	    && tv2->vval.v_tuple->tv_type != پوچ)
    {
	اگر (!check_tuples_addable(tv1->vval.v_tuple->tv_type,
						tv2->vval.v_tuple->tv_type))
	    بازگشت FAIL;
    }

    // concatenate tuples
    اگر (tuple_concat(tv1->vval.v_tuple, tv2->vval.v_tuple, &var3) == FAIL)
    {
	clear_tv(tv1);
	clear_tv(tv2);
	بازگشت FAIL;
    }
    clear_tv(tv1);
    *tv1 = var3;
    بازگشت OK;
}

/*
 * Left or right shift the number "tv1" by the number "tv2" and store the
 * result in "tv1".
 *
 * Return OK or FAIL.
 */
    ایستا صحیح
eval_shift_عدد(typval_T *tv1, typval_T *tv2, صحیح shift_type)
{
    اگر (tv2->v_type != VAR_عدد || tv2->vval.v_عدد < 0)
    {
	// right operand should be a positive number
	اگر (tv2->v_type != VAR_عدد)
	    emsg(_(e_bitshift_ops_must_be_عدد));
	وگرنه
	    emsg(_(e_bitshift_ops_must_be_positive));
	clear_tv(tv1);
	clear_tv(tv2);
	بازگشت FAIL;
    }

    اگر (tv2->vval.v_عدد > MAX_LSHIFT_BITS)
	// shifting more bits than we have always results in zero
	tv1->vval.v_عدد = 0;
    وگرنه اگر (shift_type == عبارت_LSHIFT)
	tv1->vval.v_عدد =
	    (uvarnumber_T)tv1->vval.v_عدد << tv2->vval.v_عدد;
    وگرنه
	tv1->vval.v_عدد =
	    (uvarnumber_T)tv1->vval.v_عدد >> tv2->vval.v_عدد;

    بازگشت OK;
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
    ایستا صحیح
eval5(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    /*
     * Get the first expression.
     */
    اگر (eval6(arg, rettv, evalarg) == FAIL)
	بازگشت FAIL;

    /*
     * Repeat computing, until no '<<' or '>>' is following.
     */
    برای (;;)
    {
	char_u		*p;
	صحیح		getnext;
	exprtype_T	exprtype;
	صحیح		evaluate;
	typval_T	var2;
	صحیح		vim9script;

	p = eval_next_non_blank(*arg, evalarg, &getnext);
	اگر (p[0] == '<' && p[1] == '<')
	    exprtype = عبارت_LSHIFT;
	وگرنه اگر (p[0] == '>' && p[1] == '>')
	    exprtype = عبارت_RSHIFT;
	وگرنه
	    بازگشت OK;

	// Handle a bitwise left or right shift operator
	evaluate = evalarg == پوچ ? 0 : (evalarg->eval_flags & EVAL_EVALUATE);
	اگر (evaluate && rettv->v_type != VAR_عدد)
	{
	    // left operand should be a number
	    emsg(_(e_bitshift_ops_must_be_عدد));
	    clear_tv(rettv);
	    بازگشت FAIL;
	}

	vim9script = in_vim9script();
	اگر (getnext)
	{
	    *arg = eval_next_سطر(*arg, evalarg);
	    p = *arg;
	}
	وگرنه اگر (evaluate && vim9script && !VIM_ISWHITE(**arg))
	{
	    error_white_both(*arg, 2);
	    clear_tv(rettv);
	    بازگشت FAIL;
	}

	/*
	 * Get the second variable.
	 */
	اگر (evaluate && vim9script && !IS_WHITE_OR_NUL(p[2]))
	{
	    error_white_both(p, 2);
	    clear_tv(rettv);
	    بازگشت FAIL;
	}
	*arg = skipwhite_and_linebreak(p + 2, evalarg);
	اگر (eval6(arg, &var2, evalarg) == FAIL)
	{
	    clear_tv(rettv);
	    بازگشت FAIL;
	}

	اگر (evaluate)
	{
	    اگر (eval_shift_عدد(rettv, &var2, exprtype) == FAIL)
		بازگشت FAIL;
	}

	clear_tv(&var2);
    }

    بازگشت OK;
}

/*
 * Concatenate strings "tv1" and "tv2" and store the result in "tv1".
 */
    ایستا صحیح
eval_concat_str(typval_T *tv1, typval_T *tv2)
{
    char_u	buf1[NUMBUFLEN], buf2[NUMBUFLEN];
    char_u	*s1 = tv_get_string_buf(tv1, buf1);
    char_u	*s2 = پوچ;
    char_u	*p;
    صحیح		vim9script = in_vim9script();

    اگر (vim9script && (tv2->v_type == VAR_VOID
		|| tv2->v_type == VAR_CHANNEL
		|| tv2->v_type == VAR_JOB))
	semsg(_(e_using_invalid_مقدار_as_string_str),
		vartype_name(tv2->v_type));
    وگرنه اگر (vim9script && tv2->v_type == VAR_FLOAT)
    {
	vim_snprintf((نویسه *)buf2, NUMBUFLEN, "%g",
		tv2->vval.v_float);
	s2 = buf2;
    }
    وگرنه
	s2 = tv_get_string_buf_chk(tv2, buf2);
    اگر (s2 == پوچ)		// type error ?
    {
	clear_tv(tv1);
	clear_tv(tv2);
	بازگشت FAIL;
    }

    // When possible, grow the existing string in place to avoid alloc/free.
    اگر (grow_string_tv(tv1, s2) == OK)
	بازگشت OK;

    p = concat_str(s1, s2);
    clear_tv(tv1);
    tv1->v_type = VAR_STRING;
    tv1->vval.v_string = p;

    بازگشت OK;
}

/*
 * Add or subtract numbers "tv1" and "tv2" and store the result in "tv1".
 * The numbers can be whole numbers or floats.
 */
    ایستا صحیح
eval_addsub_عدد(typval_T *tv1, typval_T *tv2, صحیح op)
{
    صحیح		error = FALSE;
    varnumber_T	n1, n2;
    float_T	f1 = 0, f2 = 0;

    اگر (tv1->v_type == VAR_FLOAT)
    {
	f1 = tv1->vval.v_float;
	n1 = 0;
    }
    وگرنه
    {
	n1 = tv_get_عدد_chk(tv1, &error);
	اگر (error)
	{
	    // This can only happen for "list + non-list" or
	    // "blob + non-blob".  For "non-list + ..." or
	    // "something - ...", we returned before evaluating the
	    // 2nd operand.
	    clear_tv(tv1);
	    clear_tv(tv2);
	    بازگشت FAIL;
	}
	اگر (tv2->v_type == VAR_FLOAT)
	    f1 = n1;
    }
    اگر (tv2->v_type == VAR_FLOAT)
    {
	f2 = tv2->vval.v_float;
	n2 = 0;
    }
    وگرنه
    {
	n2 = tv_get_عدد_chk(tv2, &error);
	اگر (error)
	{
	    clear_tv(tv1);
	    clear_tv(tv2);
	    بازگشت FAIL;
	}
	اگر (tv1->v_type == VAR_FLOAT)
	    f2 = n2;
    }
    clear_tv(tv1);

    // If there is a float on either side the result is a float.
    اگر (tv1->v_type == VAR_FLOAT || tv2->v_type == VAR_FLOAT)
    {
	اگر (op == '+')
	    f1 = f1 + f2;
	وگرنه
	    f1 = f1 - f2;
	tv1->v_type = VAR_FLOAT;
	tv1->vval.v_float = f1;
    }
    وگرنه
    {
	اگر (op == '+')
	    n1 = n1 + n2;
	وگرنه
	    n1 = n1 - n2;
	tv1->v_type = VAR_عدد;
	tv1->vval.v_عدد = n1;
    }

    بازگشت OK;
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
    ایستا صحیح
eval6(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    /*
     * Get the first expression.
     */
    اگر (eval7(arg, rettv, evalarg, FALSE) == FAIL)
	بازگشت FAIL;

    /*
     * Repeat computing, until no '+', '-' or '.' is following.
     */
    برای (;;)
    {
	صحیح	    evaluate;
	صحیح	    getnext;
	char_u	    *p;
	صحیح	    op;
	صحیح	    oplen;
	صحیح	    concat;
	typval_T    var2;
	صحیح	    vim9script = in_vim9script();
	long	    op_lnum = SOURCING_LNUM;

	// "." is only string concatenation when scriptversion is 1
	// "+=", "-=" and "..=" are assignments
	// "++" and "--" on the next line are a separate command.
	p = eval_next_non_blank(*arg, evalarg, &getnext);
	op = *p;
	concat = op == '.' && (*(p + 1) == '.' || in_old_script(2));
	اگر ((op != '+' && op != '-' && !concat) || p[1] == '='
					       || (p[1] == '.' && p[2] == '='))
	    بشکن;
	اگر (getnext && (op == '+' || op == '-') && p[0] == p[1])
	    بشکن;

	evaluate = evalarg == پوچ ? 0 : (evalarg->eval_flags & EVAL_EVALUATE);
	oplen = (concat && p[1] == '.') ? 2 : 1;
	اگر (getnext)
	    *arg = eval_next_سطر(*arg, evalarg);
	وگرنه
	{
	    اگر (evaluate && vim9script && !VIM_ISWHITE(**arg))
	    {
		error_white_both(*arg, oplen);
		clear_tv(rettv);
		بازگشت FAIL;
	    }
	    *arg = p;
	}
	اگر ((op != '+' || (rettv->v_type != VAR_LIST
						&& rettv->v_type != VAR_TUPLE
						&& rettv->v_type != VAR_BLOB))
		&& (op == '.' || rettv->v_type != VAR_FLOAT)
		&& evaluate)
	{
	    صحیح		error = FALSE;

	    // For "list + ...", an illegal use of the first operand as
	    // a number cannot be determined before evaluating the 2nd
	    // operand: if this is also a list, all is ok.
	    // For "something . ...", "something - ..." or "non-list + ...",
	    // we know that the first operand needs to be a string or number
	    // without evaluating the 2nd operand.  So check before to avoid
	    // side effects after an error.
	    اگر (op != '.')
		tv_get_عدد_chk(rettv, &error);
	    اگر ((op == '.' && tv_get_string_chk(rettv) == پوچ) || error)
	    {
		clear_tv(rettv);
		بازگشت FAIL;
	    }
	}

	/*
	 * Get the second variable.
	 */
	اگر (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[oplen]))
	{
	    error_white_both(*arg, oplen);
	    clear_tv(rettv);
	    بازگشت FAIL;
	}
	*arg = skipwhite_and_linebreak(*arg + oplen, evalarg);
	اگر (eval7(arg, &var2, evalarg, !vim9script && op == '.') == FAIL)
	{
	    clear_tv(rettv);
	    بازگشت FAIL;
	}

	اگر (evaluate)
	{
	    /*
	     * Compute the result.
	     */
	    // use the line of the operation for messages
	    SOURCING_LNUM = op_lnum;
	    اگر (op == '.')
	    {
		اگر (eval_concat_str(rettv, &var2) == FAIL)
		    بازگشت FAIL;
	    }
	    وگرنه اگر (op == '+' && rettv->v_type == VAR_BLOB
						   && var2.v_type == VAR_BLOB)
		eval_addblob(rettv, &var2);
	    وگرنه اگر (op == '+' && rettv->v_type == VAR_LIST
						   && var2.v_type == VAR_LIST)
	    {
		اگر (eval_addlist(rettv, &var2) == FAIL)
		    بازگشت FAIL;
	    }
	    وگرنه اگر (op == '+' && rettv->v_type == VAR_TUPLE
					   && var2.v_type == VAR_TUPLE)
	    {
		اگر (eval_addtuple(rettv, &var2) == FAIL)
		    بازگشت FAIL;
	    }
	    وگرنه
	    {
		اگر (eval_addsub_عدد(rettv, &var2, op) == FAIL)
		    بازگشت FAIL;
	    }
	    clear_tv(&var2);
	}
    }
    بازگشت OK;
}

/*
 * Multiply or divide or compute the modulo of numbers "tv1" and "tv2" and
 * store the result in "tv1".  The numbers can be whole numbers or floats.
 */
    ایستا صحیح
eval_multdiv_عدد(typval_T *tv1, typval_T *tv2, صحیح op)
{
    varnumber_T	n1, n2;
    float_T	f1, f2;
    صحیح		error;
    صحیح		use_float = FALSE;

    f1 = 0;
    f2 = 0;
    error = FALSE;
    اگر (tv1->v_type == VAR_FLOAT)
    {
	f1 = tv1->vval.v_float;
	use_float = TRUE;
	n1 = 0;
    }
    وگرنه
	n1 = tv_get_عدد_chk(tv1, &error);
    clear_tv(tv1);
    اگر (error)
    {
	clear_tv(tv2);
	بازگشت FAIL;
    }

    اگر (tv2->v_type == VAR_FLOAT)
    {
	اگر (!use_float)
	{
	    f1 = n1;
	    use_float = TRUE;
	}
	f2 = tv2->vval.v_float;
	n2 = 0;
    }
    وگرنه
    {
	n2 = tv_get_عدد_chk(tv2, &error);
	clear_tv(tv2);
	اگر (error)
	    بازگشت FAIL;
	اگر (use_float)
	    f2 = n2;
    }

    /*
     * Compute the result.
     * When either side is a float the result is a float.
     */
    اگر (use_float)
    {
	اگر (op == '*')
	    f1 = f1 * f2;
	وگرنه اگر (op == '/')
	{
#ifdef VMS
	    // VMS crashes on divide by zero, work around it
	    اگر (f2 == 0.0)
	    {
		اگر (f1 == 0)
		    f1 = -1 * __F_FLT_MAX - 1L;   // similar to NaN
		وگرنه اگر (f1 < 0)
		    f1 = -1 * __F_FLT_MAX;
		وگرنه
		    f1 = __F_FLT_MAX;
	    }
	    وگرنه
		f1 = f1 / f2;
#وگرنه
	    // We rely on the floating point library to handle divide
	    // by zero to result in "inf" and not a crash.
	    f1 = f1 / f2;
#endif
	}
	وگرنه
	{
	    emsg(_(e_cannot_use_percent_with_float));
	    بازگشت FAIL;
	}
	tv1->v_type = VAR_FLOAT;
	tv1->vval.v_float = f1;
    }
    وگرنه
    {
	صحیح	    failed = FALSE;

	اگر (op == '*')
	    n1 = n1 * n2;
	وگرنه اگر (op == '/')
	    n1 = رقم_divide(n1, n2, &failed);
	وگرنه
	    n1 = رقم_modulus(n1, n2, &failed);
	اگر (failed)
	    بازگشت FAIL;

	tv1->v_type = VAR_عدد;
	tv1->vval.v_عدد = n1;
    }

    بازگشت OK;
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
    ایستا صحیح
eval7(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    صحیح		want_string)  // after "." operator
{
    /*
     * Get the first expression.
     */
    اگر (eval8(arg, rettv, evalarg, want_string) == FAIL)
	بازگشت FAIL;

    /*
     * Repeat computing, until no '*', '/' or '%' is following.
     */
    برای (;;)
    {
	صحیح	    evaluate;
	صحیح	    getnext;
	typval_T    var2;
	char_u	    *p;
	صحیح	    op;

	// "*=", "/=" and "%=" are assignments
	p = eval_next_non_blank(*arg, evalarg, &getnext);
	op = *p;
	اگر ((op != '*' && op != '/' && op != '%') || p[1] == '=')
	    بشکن;

	evaluate = evalarg == پوچ ? 0 : (evalarg->eval_flags & EVAL_EVALUATE);
	اگر (getnext)
	    *arg = eval_next_سطر(*arg, evalarg);
	وگرنه
	{
	    اگر (evaluate && in_vim9script() && !VIM_ISWHITE(**arg))
	    {
		error_white_both(*arg, 1);
		clear_tv(rettv);
		بازگشت FAIL;
	    }
	    *arg = p;
	}

	/*
	 * Get the second variable.
	 */
	اگر (evaluate && in_vim9script() && !IS_WHITE_OR_NUL((*arg)[1]))
	{
	    error_white_both(*arg, 1);
	    clear_tv(rettv);
	    بازگشت FAIL;
	}
	*arg = skipwhite_and_linebreak(*arg + 1, evalarg);
	اگر (eval8(arg, &var2, evalarg, FALSE) == FAIL)
	    بازگشت FAIL;

	اگر (evaluate)
	    // Compute the result.
	    اگر (eval_multdiv_عدد(rettv, &var2, op) == FAIL)
		بازگشت FAIL;
    }

    بازگشت OK;
}

/*
 * Handle seventh level expression:
 *	a type cast before a base level expression.
 * "arg" must point to the first non-white of the expression.
 * "arg" is advanced to just after the recognized expression.
 * Return OK or FAIL.
 */
    ایستا صحیح
eval8(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    صحیح		want_string)	// after "." operator
{
    type_T	*want_type = پوچ;
    garray_T	type_list;	    // list of pointers to allocated types
    صحیح		res;
    صحیح		evaluate = evalarg == پوچ ? 0
				       : (evalarg->eval_flags & EVAL_EVALUATE);

    // Recognize <type> in Vim9 script only.
    اگر (in_vim9script() && **arg == '<' && eval_isnamec1((*arg)[1])
					     && STRNCMP(*arg, "<SNR>", 5) != 0)
    {
	++*arg;
	ga_init2(&type_list, اندازه(type_T *), 10);
	اگر (!evaluate)
	    // Only skip over the type, it may use a type variable that is only
	    // known when the expression is compiled, e.g. in a lambda.
	    *arg = skip_type(*arg, FALSE);
	وگرنه
	{
	    want_type = parse_type(arg, &type_list, پوچ, پوچ, TRUE);
	    اگر (want_type == پوچ)
	    {
		clear_type_list(&type_list);
		بازگشت FAIL;
	    }
	}

	اگر (**arg != '>')
	{
	    اگر (*skipwhite(*arg) == '>')
		semsg(_(e_no_white_space_allowed_before_str_str), ">", *arg);
	    وگرنه
		emsg(_(e_missing_gt));
	    clear_type_list(&type_list);
	    بازگشت FAIL;
	}
	++*arg;
	*arg = skipwhite_and_linebreak(*arg, evalarg);
    }

    res = eval9(arg, rettv, evalarg, want_string);

    اگر (want_type != پوچ)
    {
	اگر (evaluate && res == OK)
	{
	    type_T *actual = typval2type(rettv, get_copyID(), &type_list,
							       TVTT_DO_MEMBER);

	    اگر (!equal_type(want_type, actual, 0))
	    {
		اگر (want_type->tt_type == VAR_BOOL
					&& actual->tt_type != VAR_BOOL
					&& (actual->tt_flags & TTFLAG_BOOL_OK))
		{
		    صحیح n = tv2bool(rettv);

		    // can use "0" and "1" for boolean in some places
		    clear_tv(rettv);
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_عدد = n ? VVAL_TRUE : VVAL_FALSE;
		}
		وگرنه
		{
		    where_T where = WHERE_INIT;

		    res = check_type(want_type, actual, TRUE, where);
		}
	    }
	}
	clear_type_list(&type_list);
    }

    بازگشت res;
}

    صحیح
eval_leader(char_u **arg, صحیح vim9)
{
    char_u	*s = *arg;
    char_u	*p = *arg;

    وقتی (*p == '!' || *p == '-' || *p == '+')
    {
	char_u *n = skipwhite(p + 1);

	// ++, --, -+ and +- are not accepted in Vim9 script
	اگر (vim9 && (*p == '-' || *p == '+') && (*n == '-' || *n == '+'))
	{
	    semsg(_(e_invalid_expression_str), s);
	    بازگشت FAIL;
	}
	p = n;
    }
    *arg = p;
    بازگشت OK;
}

/*
 * Check for a predefined value "true", "false" and "null.*".
 * Return OK when recognized.
 */
    صحیح
handle_predefined(char_u *s, صحیح len, typval_T *rettv)
{
    انتخاب (len)
    {
	حالت 4: اگر (STRNCMP(s, "true", 4) == 0)
		{
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_عدد = VVAL_TRUE;
		    بازگشت OK;
		}
		اگر (STRNCMP(s, "null", 4) == 0)
		{
		    rettv->v_type = VAR_SPECIAL;
		    rettv->vval.v_عدد = VVAL_NULL;
		    بازگشت OK;
		}
		بشکن;
	حالت 5: اگر (STRNCMP(s, "false", 5) == 0)
		{
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_عدد = VVAL_FALSE;
		    بازگشت OK;
		}
		بشکن;
	حالت 8: اگر (STRNCMP(s, "null_job", 8) == 0)
		{
#ifdef FEAT_JOB_CHANNEL
		    rettv->v_type = VAR_JOB;
		    rettv->vval.v_job = پوچ;
#وگرنه
		    rettv->v_type = VAR_SPECIAL;
		    rettv->vval.v_عدد = VVAL_NULL;
#endif
		    بازگشت OK;
		}
		بشکن;
	حالت 9:
		اگر (STRNCMP(s, "null_", 5) != 0)
		    بشکن;
		// null_list
		اگر (STRNCMP(s + 5, "list", 4) == 0)
		{
		    rettv->v_type = VAR_LIST;
		    rettv->vval.v_list = پوچ;
		    بازگشت OK;
		}
		// null_dict
		اگر (STRNCMP(s + 5, "dict", 4) == 0)
		{
		    rettv->v_type = VAR_DICT;
		    rettv->vval.v_dict = پوچ;
		    بازگشت OK;
		}
		// null_blob
		اگر (STRNCMP(s + 5, "blob", 4) == 0)
		{
		    rettv->v_type = VAR_BLOB;
		    rettv->vval.v_blob = پوچ;
		    بازگشت OK;
		}
		بشکن;
	حالت 10:
		اگر (STRNCMP(s, "null_", 5) != 0)
		    بشکن;
		// null_class
		اگر (STRNCMP(s + 5, "class", 5) == 0)
		{
		    rettv->v_type = VAR_CLASS;
		    rettv->vval.v_class = پوچ;
		    بازگشت OK;
		}
		اگر (STRNCMP(s + 5, "tuple", 5) == 0)
		{
		    rettv->v_type = VAR_TUPLE;
		    rettv->vval.v_tuple = پوچ;
		    بازگشت OK;
		}
		بشکن;
	حالت 11: اگر (STRNCMP(s, "null_string", 11) == 0)
		{
		    rettv->v_type = VAR_STRING;
		    rettv->vval.v_string = پوچ;
		    بازگشت OK;
		}
		اگر (STRNCMP(s, "null_object", 11) == 0)
		{
		    rettv->v_type = VAR_OBJECT;
		    rettv->vval.v_object = پوچ;
		    بازگشت OK;
		}
		بشکن;
	حالت 12:
		اگر (STRNCMP(s, "null_channel", 12) == 0)
		{
#ifdef FEAT_JOB_CHANNEL
		    rettv->v_type = VAR_CHANNEL;
		    rettv->vval.v_channel = پوچ;
#وگرنه
		    rettv->v_type = VAR_SPECIAL;
		    rettv->vval.v_عدد = VVAL_NULL;
#endif
		    بازگشت OK;
		}
		اگر (STRNCMP(s, "null_partial", 12) == 0)
		{
		    rettv->v_type = VAR_PARTIAL;
		    rettv->vval.v_partial = پوچ;
		    بازگشت OK;
		}
		بشکن;
	حالت 13: اگر (STRNCMP(s, "null_function", 13) == 0)
		{
		    rettv->v_type = VAR_FUNC;
		    rettv->vval.v_string = پوچ;
		    بازگشت OK;
		}
		بشکن;
    }
    بازگشت FAIL;
}

/*
 * Handle register contents: @r.
 */
    ایستا تهی
eval9_reg_contents(
    char_u	**arg,
    typval_T	*rettv,
    صحیح		evaluate)
{
    صحیح		vim9script = in_vim9script();

    ++*arg;	// skip '@'
    اگر (evaluate)
    {
	اگر (vim9script && IS_WHITE_OR_NUL(**arg))
	    semsg(_(e_syntax_error_at_str), *arg);
	وگرنه اگر (vim9script && !valid_yank_reg(**arg, FALSE))
	    emsg_invreg(**arg);
	وگرنه
	{
	    rettv->v_type = VAR_STRING;
	    rettv->vval.v_string = get_reg_contents(**arg,
		    GREG_عبارت_SRC);
	}
    }
    اگر (**arg != NUL)
	++*arg;
}

/*
 * Handle a nested expression: (expression) or lambda: (arg) => expr
 */
    ایستا صحیح
eval9_nested_عبارت(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    صحیح		evaluate)
{
    صحیح		ret = NOTDONE;
    صحیح		vim9script = in_vim9script();

    اگر (vim9script)
    {
	ret = get_lambda_tv(arg, rettv, TRUE, evalarg, پوچ);
	اگر (ret == OK && evaluate)
	{
	    ufunc_T *ufunc = rettv->vval.v_partial->pt_func;

	    // Compile it here to get the return type.  The return
	    // type is optional, when it's missing use t_unknown.
	    // This is recognized in compile_return().
	    اگر (ufunc->uf_ret_type->tt_type == VAR_VOID)
		ufunc->uf_ret_type = &t_unknown;
	    اگر (compile_def_function(ufunc, FALSE,
				get_compile_type(ufunc), پوچ) == FAIL)
	    {
		clear_tv(rettv);
		ret = FAIL;
	    }
	}
    }
    اگر (ret == NOTDONE)
    {
	*arg = skipwhite_and_linebreak(*arg + 1, evalarg);
	اگر (**arg == ')')
	    // empty tuple
	    ret = eval_tuple(arg, rettv, evalarg, TRUE);
	وگرنه
	{
	    ret = eval1(arg, rettv, evalarg);	// recursive!
	    اگر (ret != OK)
		بازگشت ret;

	    *arg = skipwhite_and_linebreak(*arg, evalarg);

	    اگر (**arg == ',')
		// tuple
		ret = eval_tuple(arg, rettv, evalarg, TRUE);
	    وگرنه اگر (**arg == ')')
		++*arg;
	    وگرنه اگر (ret == OK)
	    {
		emsg(_(e_missing_closing_paren));
		clear_tv(rettv);
		ret = FAIL;
	    }
	}
    }

    بازگشت ret;
}

/*
* Handle be a variable or function name.
* Can also be a curly-braces kind of name: {expr}.
*/
    ایستا صحیح
eval9_var_func_name(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    صحیح		evaluate,
    char_u	**name_start)
{
    char_u	*s;
    صحیح		len;
    char_u	*alias;
    صحیح		ret = OK;
    صحیح		vim9script = in_vim9script();

    s = *arg;
    len = get_name_len(arg, &alias, evaluate, TRUE);
    اگر (alias != پوچ)
	s = alias;

    اگر (len <= 0)
	ret = FAIL;
    وگرنه
    {
	صحیح	flags = evalarg == پوچ ? 0 : evalarg->eval_flags;

	اگر (evaluate && vim9script && len == 1 && *s == '_')
	{
	    emsg(_(e_cannot_use_underscore_here));
	    ret = FAIL;
	}
	وگرنه اگر (evaluate && vim9script && len > 2
						&& s[0] == 's' && s[1] == ':')
	{
	    semsg(_(e_cannot_use_s_colon_in_vim9_script_str), s);
	    ret = FAIL;
	}
	وگرنه اگر ((vim9script ? **arg : *skipwhite(*arg)) == '('
				|| (vim9script && generic_func_call(arg)))
	{
	    // "name(..."  recursive!
	    *arg = skipwhite(*arg);
	    ret = eval_func(arg, evalarg, s, len, rettv, flags, پوچ);
	}
	وگرنه اگر (evaluate)
	{
	    // get the value of "true", "false", etc. or a variable
	    ret = FAIL;
	    اگر (vim9script)
		ret = handle_predefined(s, len, rettv);
	    اگر (ret == FAIL)
	    {
		*name_start = s;
		ret = eval_variable(s, len, 0, rettv, پوچ,
					EVAL_VAR_VERBOSE + EVAL_VAR_IMPORT);

		// skip the generic function arguments (if present)
		// they are already processed by eval_variable
		اگر (ret == OK && vim9script && **arg == '<'
						&& rettv->v_type == VAR_FUNC)
		    ret = skip_generic_func_type_args(arg);
	    }
	}
	وگرنه
	{
	    // skip the name
	    check_vars(s, len);

	    // Skip the type arguments of a generic function, its name starts
	    // with an uppercase letter.
	    اگر (vim9script && **arg == '<' && ASCII_ISUPPER(*s))
		skip_generic_func_type_args(arg);
	    ret = OK;
	}
    }
    vim_free(alias);

    بازگشت ret;
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
    ایستا صحیح
eval9(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    صحیح		want_string)	// after "." operator
{
    صحیح		evaluate = evalarg != پوچ
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    char_u	*name_start = پوچ;
    char_u	*start_leader, *end_leader;
    صحیح		ret = OK;
    ایستا صحیح	recurse = 0;
    صحیح		vim9script = in_vim9script();

    /*
     * Initialise variable so that clear_tv() can't mistake this for a
     * string and free a string that isn't there.
     */
    rettv->v_type = VAR_UNKNOWN;

    /*
     * Skip '!', '-' and '+' characters.  They are handled later.
     */
    start_leader = *arg;
    اگر (eval_leader(arg, vim9script) == FAIL)
	بازگشت FAIL;
    end_leader = *arg;

    اگر (**arg == '.' && (!SAFE_isdigit(*(*arg + 1)) || in_old_script(2)))
    {
	semsg(_(e_invalid_expression_str), *arg);
	++*arg;
	بازگشت FAIL;
    }

    // Limit recursion to 1000 levels.  At least at 10000 we run out of stack
    // and crash.  With MSVC the stack is smaller.
    اگر (recurse ==
#ifdef _MSC_VER
		    300
#وگرنه
		    1000
#endif
		    )
    {
	semsg(_(e_expression_too_recursive_str), *arg);
	بازگشت FAIL;
    }
    ++recurse;

    انتخاب (**arg)
    {
    /*
     * Number constant.
     */
    حالت '0':
    حالت '1':
    حالت '2':
    حالت '3':
    حالت '4':
    حالت '5':
    حالت '6':
    حالت '7':
    حالت '8':
    حالت '9':
    حالت '.':	ret = eval_عدد(arg, rettv, evaluate, want_string);

		// Apply prefixed "-" and "+" now.  Matters especially when
		// "->" follows.
		اگر (ret == OK && evaluate && end_leader > start_leader
						  && rettv->v_type != VAR_BLOB)
		    ret = eval9_leader(rettv, TRUE, start_leader, &end_leader);
		بشکن;

    /*
     * String constant: "string".
     */
    حالت '"':	ret = eval_string(arg, rettv, evaluate, FALSE);
		بشکن;

    /*
     * Literal string constant: 'str''ing'.
     */
    حالت '\'':	ret = eval_lit_string(arg, rettv, evaluate, FALSE);
		بشکن;

    /*
     * List: [expr, expr]
     */
    حالت '[':	ret = eval_list(arg, rettv, evalarg, TRUE);
		بشکن;

    /*
     * Literal Dictionary: #{key: val, key: val}
     */
    حالت '#':	ret = eval_lit_dict(arg, rettv, evalarg);
		بشکن;

    /*
     * Lambda: {arg, arg -> expr}
     * Dictionary: {'key': val, 'key': val}
     */
    حالت '{':	اگر (vim9script)
		    ret = NOTDONE;
		وگرنه
		    ret = get_lambda_tv(arg, rettv, vim9script, evalarg, پوچ);
		اگر (ret == NOTDONE)
		    ret = eval_dict(arg, rettv, evalarg, FALSE);
		بشکن;

    /*
     * Option value: &name
     */
    حالت '&':	ret = eval_option(arg, rettv, evaluate);
		بشکن;

    /*
     * Environment variable: $VAR.
     * Interpolated string: $"string" or $'string'.
     */
    حالت '$':	اگر ((*arg)[1] == '"' || (*arg)[1] == '\'')
		    ret = eval_interp_string(arg, rettv, evaluate);
		وگرنه
		    ret = eval_env_var(arg, rettv, evaluate);
		بشکن;

    /*
     * Register contents: @r.
     */
    حالت '@':	eval9_reg_contents(arg, rettv, evaluate);
		بشکن;

    /*
     * nested expression: (expression).
     * or lambda: (arg) => expr
     * or tuple
     */
    حالت '(':	ret = eval9_nested_عبارت(arg, rettv, evalarg, evaluate);
		بشکن;

    پیشفرض:	ret = NOTDONE;
		بشکن;
    }

    اگر (ret == NOTDONE)
    {
	/*
	 * Must be a variable or function name.
	 * Can also be a curly-braces kind of name: {expr}.
	 */
	ret = eval9_var_func_name(arg, rettv, evalarg, evaluate, &name_start);
    }

    // Handle following '[', '(' and '.' for expr[expr], expr.name,
    // expr(expr), expr->name(expr)
    اگر (ret == OK)
	ret = handle_subscript(arg, name_start, rettv, evalarg, evaluate);

    /*
     * Apply logical NOT and unary '-', from right to left, ignore '+'.
     */
    اگر (ret == OK && evaluate && end_leader > start_leader)
	ret = eval9_leader(rettv, FALSE, start_leader, &end_leader);

    --recurse;
    بازگشت ret;
}

/*
 * Apply the leading "!" and "-" before an eval9 expression to "rettv".
 * When "numeric_only" is TRUE only handle "+" and "-".
 * Adjusts "end_leaderp" until it is at "start_leader".
 */
    ایستا صحیح
eval9_leader(
	typval_T    *rettv,
	صحیح	    numeric_only,
	char_u	    *start_leader,
	char_u	    **end_leaderp)
{
    char_u	*end_leader = *end_leaderp;
    صحیح		ret = OK;
    صحیح		error = FALSE;
    varnumber_T val = 0;
    vartype_T	type = rettv->v_type;
    صحیح		vim9script = in_vim9script();
    float_T	    f = 0.0;

    اگر (rettv->v_type == VAR_FLOAT)
	f = rettv->vval.v_float;
    وگرنه
    {
	وقتی (VIM_ISWHITE(end_leader[-1]))
	    --end_leader;
	اگر (vim9script && end_leader[-1] == '!')
	{
	    // Is this typeval supported with the ! operator?
	    اگر (check_typval_is_مقدار(rettv) == FAIL)
	    {
		clear_tv(rettv);
		بازگشت FAIL;
	    }
	    val = tv2bool(rettv);
	}
	وگرنه
	    val = tv_get_عدد_chk(rettv, &error);
    }
    اگر (error)
    {
	clear_tv(rettv);
	ret = FAIL;
    }
    وگرنه
    {
	وقتی (end_leader > start_leader)
	{
	    --end_leader;
	    اگر (*end_leader == '!')
	    {
		اگر (numeric_only)
		{
		    ++end_leader;
		    بشکن;
		}
		اگر (rettv->v_type == VAR_FLOAT)
		{
		    rettv->v_type = VAR_BOOL;
		    val = f == 0.0 ? VVAL_TRUE : VVAL_FALSE;
		    type = VAR_BOOL;
		}
		وگرنه
		{
		    val = !val;
		    type = VAR_BOOL;
		}
	    }
	    وگرنه اگر (*end_leader == '-')
	    {
		اگر (rettv->v_type == VAR_FLOAT)
		    f = -f;
		وگرنه
		{
		    val = -val;
		    type = VAR_عدد;
		}
	    }
	}
	اگر (rettv->v_type == VAR_FLOAT)
	{
	    clear_tv(rettv);
	    rettv->vval.v_float = f;
	}
	وگرنه
	{
	    clear_tv(rettv);
	    اگر (vim9script)
		rettv->v_type = type;
	    وگرنه
		rettv->v_type = VAR_عدد;
	    rettv->vval.v_عدد = val;
	}
    }
    *end_leaderp = end_leader;
    بازگشت ret;
}

/*
 * Call the function referred to in "rettv".
 */
    ایستا صحیح
call_func_rettv(
	char_u	    **arg,
	evalarg_T   *evalarg,
	typval_T    *rettv,
	صحیح	    evaluate,
	dict_T	    *selfdict,
	typval_T    *basetv)
{
    partial_T	*pt = پوچ;
    funcexe_T	funcexe;
    typval_T	functv;
    char_u	*s;
    صحیح		ret;

    // need to copy the funcref so that we can clear rettv
    اگر (evaluate)
    {
	functv = *rettv;
	rettv->v_type = VAR_UNKNOWN;

	// Invoke the function.  Recursive!
	اگر (functv.v_type == VAR_PARTIAL)
	{
	    pt = functv.vval.v_partial;
	    s = partial_name(pt);
	}
	وگرنه
	{
	    s = functv.vval.v_string;
	    اگر (s == پوچ || *s == NUL)
	    {
		emsg(_(e_empty_function_name));
		ret = FAIL;
		برو theend;
	    }
	}
    }
    وگرنه
	s = (char_u *)"";

    CLEAR_FIELD(funcexe);
    funcexe.fe_firstline = curwin->w_cursor.lnum;
    funcexe.fe_lastline = curwin->w_cursor.lnum;
    funcexe.fe_evaluate = evaluate;
    funcexe.fe_partial = pt;
    اگر (pt != پوچ)
    {
	funcexe.fe_object = pt->pt_obj;
	اگر (funcexe.fe_object != پوچ)
	    ++funcexe.fe_object->obj_refcount;
    }
    funcexe.fe_selfdict = selfdict;
    funcexe.fe_basetv = basetv;
    اگر (evalarg != پوچ)
	funcexe.fe_cctx = evalarg->eval_cctx;
    ret = get_func_tv(s, -1, rettv, arg, evalarg, &funcexe);

theend:
    // Clear the funcref afterwards, so that deleting it while
    // evaluating the arguments is possible (see test55).
    اگر (evaluate)
	clear_tv(&functv);

    بازگشت ret;
}

/*
 * Evaluate "->method()".
 * "*arg" points to "method".
 * Returns FAIL or OK. "*arg" is advanced to after the ')'.
 */
    ایستا صحیح
eval_lambda(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    صحیح		verbose)	// give error messages
{
    صحیح		evaluate = evalarg != پوچ
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    typval_T	base = *rettv;
    صحیح		ret;

    rettv->v_type = VAR_UNKNOWN;

    اگر (**arg == '{')
    {
	// ->{lambda}()
	ret = get_lambda_tv(arg, rettv, FALSE, evalarg, پوچ);
    }
    وگرنه
    {
	// ->(lambda)()
	++*arg;
	ret = eval1(arg, rettv, evalarg);
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	اگر (**arg != ')')
	{
	    emsg(_(e_missing_closing_paren));
	    بازگشت FAIL;
	}
	اگر (rettv->v_type != VAR_STRING && rettv->v_type != VAR_FUNC
					       && rettv->v_type != VAR_PARTIAL)
	{
	    emsg(_(e_string_or_function_required_for_arrow_parens_عبارت));
	    بازگشت FAIL;
	}
	++*arg;
    }
    اگر (ret != OK)
	بازگشت FAIL;

    اگر (**arg != '(')
    {
	اگر (verbose)
	{
	    اگر (*skipwhite(*arg) == '(')
		emsg(_(e_no_white_space_allowed_before_parenthesis));
	    وگرنه
		semsg(_(e_missing_parenthesis_str), "lambda");
	}
	clear_tv(rettv);
	ret = FAIL;
    }
    وگرنه
	ret = call_func_rettv(arg, evalarg, rettv, evaluate, پوچ, &base);

    // Clear the funcref afterwards, so that deleting it while
    // evaluating the arguments is possible (see test55).
    اگر (evaluate)
	clear_tv(&base);

    بازگشت ret;
}

/*
 * Evaluate "->method()".
 * "*arg" points to "method".
 * Returns FAIL or OK. "*arg" is advanced to after the ')'.
 */
    ایستا صحیح
eval_method(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    صحیح		verbose)	// give error messages
{
    char_u	*name;
    long	len;
    char_u	*alias;
    char_u	*tofree = پوچ;
    typval_T	base = *rettv;
    صحیح		ret = OK;
    صحیح		evaluate = evalarg != پوچ
				      && (evalarg->eval_flags & EVAL_EVALUATE);

    rettv->v_type = VAR_UNKNOWN;

    name = *arg;
    len = get_name_len(arg, &alias, evaluate, evaluate);
    اگر (alias != پوچ)
	name = alias;

    اگر (len <= 0)
    {
	اگر (verbose)
	    emsg(_(e_missing_name_after_method));
	ret = FAIL;
    }
    وگرنه
    {
	char_u *paren;

	// If there is no "(" immediately following, but there is further on,
	// it can be "import.Func()", "dict.Func()", "list[nr]", etc.
	// Does not handle anything where "(" is part of the expression.
	*arg = skipwhite(*arg);

	اگر (**arg != '(' && alias == پوچ
				    && (paren = vim_strchr(*arg, '(')) != پوچ)
	{
	    *arg = name;

	    // Truncate the name at the "(".  Avoid trying to get another line
	    // by making "getline" NULL.
	    *paren = NUL;
	    char_u	*(*getline)(صحیح, تهی *, صحیح, getline_opt_T) = پوچ;
	    اگر (evalarg != پوچ)
	    {
		getline = evalarg->eval_getline;
		evalarg->eval_getline = پوچ;
	    }

	    char_u *deref = deref_function_name(arg, &tofree, evalarg, verbose);
	    اگر (deref == پوچ)
	    {
		*arg = name + len;
		ret = FAIL;
	    }
	    وگرنه
	    {
		name = deref;
		len = (long)STRLEN(name);
	    }

	    *paren = '(';
	    اگر (getline != پوچ)
		evalarg->eval_getline = getline;
	}

	اگر (ret == OK)
	{
	    *arg = skipwhite(*arg);

	    اگر (**arg != '(')
	    {
		اگر (verbose)
		    semsg(_(e_missing_parenthesis_str), name);
		ret = FAIL;
	    }
	    وگرنه اگر (VIM_ISWHITE((*arg)[-1]))
	    {
		اگر (verbose)
		    emsg(_(e_no_white_space_allowed_before_parenthesis));
		ret = FAIL;
	    }
	    وگرنه
		ret = eval_func(arg, evalarg, name, len, rettv,
					  evaluate ? EVAL_EVALUATE : 0, &base);
	}
    }

    // Clear the funcref afterwards, so that deleting it while
    // evaluating the arguments is possible (see test55).
    اگر (evaluate)
	clear_tv(&base);
    vim_free(tofree);

    اگر (alias != پوچ)
	vim_free(alias);

    بازگشت ret;
}

/*
 * Evaluate an "[expr]" or "[expr:expr]" index.  Also "dict.key".
 * "*arg" points to the '[' or '.'.
 * Returns FAIL or OK. "*arg" is advanced to after the ']'.
 */
    ایستا صحیح
eval_index(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    صحیح		verbose)	// give error messages
{
    صحیح		evaluate = evalarg != پوچ
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    صحیح		empty1 = FALSE, empty2 = FALSE;
    typval_T	var1, var2;
    صحیح		range = FALSE;
    char_u	*کلید = پوچ;
    صحیح		keylen = -1;
    صحیح		vim9script = in_vim9script();

    اگر (check_can_index(rettv, evaluate, verbose) == FAIL)
	بازگشت FAIL;

    init_tv(&var1);
    init_tv(&var2);
    اگر (**arg == '.')
    {
	/*
	 * dict.name
	 */
	کلید = *arg + 1;
	برای (keylen = 0; eval_isdictc(کلید[keylen]); ++keylen)
	    ;
	اگر (keylen == 0)
	    بازگشت FAIL;
	اگر (vim9script && کلید[keylen] == '<')
	{
	    // skip generic type arguments
	    char_u	*p = &کلید[keylen];

	    اگر (skip_generic_func_type_args(&p) == FAIL)
		بازگشت FAIL;
	    keylen = p - کلید;
	}
	*arg = کلید + keylen;
    }
    وگرنه
    {
	/*
	 * something[idx]
	 *
	 * Get the (first) variable from inside the [].
	 */
	*arg = skipwhite_and_linebreak(*arg + 1, evalarg);
	اگر (**arg == ':')
	    empty1 = TRUE;
	وگرنه اگر (eval1(arg, &var1, evalarg) == FAIL)	// recursive!
	    بازگشت FAIL;
	وگرنه اگر (vim9script && **arg == ':')
	{
	    semsg(_(e_white_space_required_before_and_after_str_at_str),
								    ":", *arg);
	    clear_tv(&var1);
	    بازگشت FAIL;
	}
	وگرنه اگر (evaluate)
	{
	    صحیح error = FALSE;

	    // allow for indexing with float
	    اگر (vim9script && rettv->v_type == VAR_DICT
						   && var1.v_type == VAR_FLOAT)
	    {
		var1.vval.v_string = typval_tostring(&var1, TRUE);
		var1.v_type = VAR_STRING;
	    }

	    اگر (vim9script && (rettv->v_type == VAR_LIST
						|| rettv->v_type == VAR_TUPLE))
		tv_get_عدد_chk(&var1, &error);
	    وگرنه
		error = tv_get_string_chk(&var1) == پوچ;
	    اگر (error)
	    {
		// not a number or string
		clear_tv(&var1);
		بازگشت FAIL;
	    }
	}

	/*
	 * Get the second variable from inside the [:].
	 */
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	اگر (**arg == ':')
	{
	    range = TRUE;
	    ++*arg;
	    اگر (vim9script && !IS_WHITE_OR_NUL(**arg) && **arg != ']')
	    {
		semsg(_(e_white_space_required_before_and_after_str_at_str),
								":", *arg - 1);
		اگر (!empty1)
		    clear_tv(&var1);
		بازگشت FAIL;
	    }
	    *arg = skipwhite_and_linebreak(*arg, evalarg);
	    اگر (**arg == ']')
		empty2 = TRUE;
	    وگرنه اگر (eval1(arg, &var2, evalarg) == FAIL)	// recursive!
	    {
		اگر (!empty1)
		    clear_tv(&var1);
		بازگشت FAIL;
	    }
	    وگرنه اگر (evaluate && tv_get_string_chk(&var2) == پوچ)
	    {
		// not a number or string
		اگر (!empty1)
		    clear_tv(&var1);
		clear_tv(&var2);
		بازگشت FAIL;
	    }
	}

	// Check for the ']'.
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	اگر (**arg != ']')
	{
	    اگر (verbose)
		emsg(_(e_missing_closing_square_brace));
	    clear_tv(&var1);
	    اگر (range)
		clear_tv(&var2);
	    بازگشت FAIL;
	}
	*arg = *arg + 1;	// skip over the ']'
    }

    اگر (evaluate)
    {
	صحیح res = eval_index_inner(rettv, range,
		empty1 ? پوچ : &var1, empty2 ? پوچ : &var2, FALSE,
		کلید, keylen, verbose);

	اگر (!empty1)
	    clear_tv(&var1);
	اگر (range)
	    clear_tv(&var2);
	بازگشت res;
    }
    بازگشت OK;
}

/*
 * Check if "rettv" can have an [index] or [sli:ce]
 */
    صحیح
check_can_index(typval_T *rettv, صحیح evaluate, صحیح verbose)
{
    انتخاب (rettv->v_type)
    {
	حالت VAR_FUNC:
	حالت VAR_PARTIAL:
	    اگر (verbose)
		emsg(_(e_cannot_index_a_funcref));
	    بازگشت FAIL;
	حالت VAR_FLOAT:
	    اگر (verbose)
		emsg(_(e_using_float_as_string));
	    بازگشت FAIL;
	حالت VAR_BOOL:
	حالت VAR_SPECIAL:
	حالت VAR_JOB:
	حالت VAR_CHANNEL:
	حالت VAR_INSTR:
	حالت VAR_OBJECT:
	    اگر (verbose)
		emsg(_(e_cannot_index_special_variable));
	    بازگشت FAIL;
	حالت VAR_CLASS:
	حالت VAR_TYPEALIAS:
	    اگر (verbose)
		check_typval_is_مقدار(rettv);
	    بازگشت FAIL;
	حالت VAR_UNKNOWN:
	حالت VAR_ANY:
	حالت VAR_VOID:
	    اگر (evaluate)
	    {
		emsg(_(e_cannot_index_special_variable));
		بازگشت FAIL;
	    }
	    // FALLTHROUGH

	حالت VAR_STRING:
	حالت VAR_LIST:
	حالت VAR_TUPLE:
	حالت VAR_DICT:
	حالت VAR_BLOB:
	    بشکن;
	حالت VAR_عدد:
	    اگر (in_vim9script())
		emsg(_(e_cannot_index_عدد));
	    بشکن;
    }
    بازگشت OK;
}

/*
 * Apply index or range to "rettv".
 * "var1" is the first index, NULL for [:expr].
 * "var2" is the second index, NULL for [expr] and [expr: ]
 * "exclusive" is TRUE for slice(): second index is exclusive, use character
 * index for string.
 * Alternatively, "key" is not NULL, then key[keylen] is the dict index.
 */
    صحیح
eval_index_inner(
	typval_T    *rettv,
	صحیح	    is_range,
	typval_T    *var1,
	typval_T    *var2,
	صحیح	    exclusive,
	char_u	    *کلید,
	صحیح	    keylen,
	صحیح	    verbose)
{
    varnumber_T	    n1, n2 = 0;
    long	    len;

    n1 = 0;
    اگر (var1 != پوچ && rettv->v_type != VAR_DICT)
	n1 = tv_get_عدد(var1);

    اگر (is_range)
    {
	اگر (rettv->v_type == VAR_DICT)
	{
	    اگر (verbose)
		emsg(_(e_cannot_slice_dictionary));
	    بازگشت FAIL;
	}
	اگر (var2 != پوچ)
	    n2 = tv_get_عدد(var2);
	وگرنه
	    n2 = VARNUM_MAX;
    }

    انتخاب (rettv->v_type)
    {
	حالت VAR_UNKNOWN:
	حالت VAR_ANY:
	حالت VAR_VOID:
	حالت VAR_FUNC:
	حالت VAR_PARTIAL:
	حالت VAR_FLOAT:
	حالت VAR_BOOL:
	حالت VAR_SPECIAL:
	حالت VAR_JOB:
	حالت VAR_CHANNEL:
	حالت VAR_INSTR:
	حالت VAR_CLASS:
	حالت VAR_OBJECT:
	حالت VAR_TYPEALIAS:
	    بشکن; // not evaluating, skipping over subscript

	حالت VAR_عدد:
	حالت VAR_STRING:
	    {
		char_u	*s = tv_get_string(rettv);

		len = (long)STRLEN(s);
		اگر (in_vim9script() || exclusive)
		{
		    اگر (is_range)
			s = string_slice(s, n1, n2, exclusive);
		    وگرنه
			s = char_from_string(s, n1);
		}
		وگرنه اگر (is_range)
		{
		    // The resulting variable is a substring.  If the indexes
		    // are out of range the result is empty.
		    اگر (n1 < 0)
		    {
			n1 = len + n1;
			اگر (n1 < 0)
			    n1 = 0;
		    }
		    اگر (n2 < 0)
			n2 = len + n2;
		    وگرنه اگر (n2 >= len)
			n2 = len;
		    اگر (n1 >= len || n2 < 0 || n1 > n2)
			s = پوچ;
		    وگرنه
			s = vim_strnsave(s + n1, n2 - n1 + 1);
		}
		وگرنه
		{
		    // The resulting variable is a string of a single
		    // character.  If the index is too big or negative the
		    // result is empty.
		    اگر (n1 >= len || n1 < 0)
			s = پوچ;
		    وگرنه
			s = vim_strnsave(s + n1, 1);
		}
		clear_tv(rettv);
		rettv->v_type = VAR_STRING;
		rettv->vval.v_string = s;
	    }
	    بشکن;

	حالت VAR_BLOB:
	    blob_slice_or_index(rettv->vval.v_blob, is_range, n1, n2,
							     exclusive, rettv);
	    بشکن;

	حالت VAR_LIST:
	    اگر (var1 == پوچ)
		n1 = 0;
	    اگر (var2 == پوچ)
		n2 = VARNUM_MAX;
	    اگر (list_slice_or_index(rettv->vval.v_list,
			  is_range, n1, n2, exclusive, rettv, verbose) == FAIL)
		بازگشت FAIL;
	    بشکن;

	حالت VAR_TUPLE:
	    اگر (var1 == پوچ)
		n1 = 0;
	    اگر (var2 == پوچ)
		n2 = VARNUM_MAX;
	    اگر (tuple_slice_or_index(rettv->vval.v_tuple,
			  is_range, n1, n2, exclusive, rettv, verbose) == FAIL)
		بازگشت FAIL;
	    بشکن;

	حالت VAR_DICT:
	    {
		dictitem_T	*item;
		typval_T	tmp;

		اگر (کلید == پوچ)
		{
		    کلید = tv_get_string_chk(var1);
		    اگر (کلید == پوچ)
			بازگشت FAIL;
		}

		item = dict_find(rettv->vval.v_dict, کلید, keylen);

		اگر (item == پوچ)
		{
		    اگر (verbose)
		    {
			اگر (keylen > 0)
			    کلید[keylen] = NUL;
			semsg(_(e_کلید_not_present_in_dictionary_str), کلید);
		    }
		    بازگشت FAIL;
		}

		copy_tv(&item->di_tv, &tmp);
		clear_tv(rettv);
		*rettv = tmp;
	    }
	    بشکن;
    }
    بازگشت OK;
}

/*
 * Return the function name of partial "pt".
 */
    char_u *
partial_name(partial_T *pt)
{
    اگر (pt != پوچ)
    {
	اگر (pt->pt_name != پوچ)
	    بازگشت pt->pt_name;
	اگر (pt->pt_func != پوچ)
	    بازگشت pt->pt_func->uf_name;
    }
    بازگشت (char_u *)"";
}

    ایستا تهی
partial_free(partial_T *pt)
{
    صحیح i;

    برای (i = 0; i < pt->pt_argc; ++i)
	clear_tv(&pt->pt_argv[i]);
    vim_free(pt->pt_argv);
    dict_unref(pt->pt_dict);
    اگر (pt->pt_name != پوچ)
    {
	func_unref(pt->pt_name);
	vim_free(pt->pt_name);
    }
    وگرنه
	func_ptr_unref(pt->pt_func);
    اگر (pt->pt_obj != پوچ)
	object_unref(pt->pt_obj);

    // "out_up" is no longer used, decrement refcount on partial that owns it.
    partial_unref(pt->pt_outer.out_up_partial);

    // Using pt_outer from another partial.
    partial_unref(pt->pt_outer_partial);

    // Decrease the reference count for the context of a closure.  If down
    // to the minimum it may be time to free it.
    اگر (pt->pt_funcstack != پوچ)
    {
	--pt->pt_funcstack->fs_refcount;
	funcstack_check_refcount(pt->pt_funcstack);
    }
    // Similarly for loop variables.
    برای (i = 0; i < MAX_LOOP_DEPTH; ++i)
	اگر (pt->pt_loopvars[i] != پوچ)
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
    تهی
partial_unref(partial_T *pt)
{
    اگر (pt == پوچ)
	بازگشت;

    صحیح	done = FALSE;

    اگر (--pt->pt_refcount <= 0)
	partial_free(pt);

    // If the reference count goes down to one, the funcstack may be the
    // only reference and can be freed if no other partials reference it.
    وگرنه اگر (pt->pt_refcount == 1)
    {
	// careful: if the funcstack is freed it may contain this partial
	// and it gets freed as well
	اگر (pt->pt_funcstack != پوچ)
	    done = funcstack_check_refcount(pt->pt_funcstack);

	اگر (!done)
	{
	    صحیح	depth;

	    برای (depth = 0; depth < MAX_LOOP_DEPTH; ++depth)
		اگر (pt->pt_loopvars[depth] != پوچ
			&& loopvars_check_refcount(pt->pt_loopvars[depth]))
		    بشکن;
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
    ایستا char_u *
string_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    صحیح		echo_style,
    صحیح		composite_val)
{
    char_u	*r = پوچ;

    اگر (echo_style && !composite_val)
    {
	*tofree = پوچ;
	r = tv->vval.v_string;
	اگر (r == پوچ)
	    r = (char_u *)"";
    }
    وگرنه
    {
	*tofree = string_quote(tv->vval.v_string, FALSE);
	r = *tofree;
    }

    بازگشت r;
}

/*
 * Return a textual representation of a function in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "echo_style" is FALSE, put quotes around the function name as
 * "function()", otherwise does not put quotes around function name.
 * May return NULL.
 */
    ایستا char_u *
func_tv2string(typval_T *tv, char_u **tofree, صحیح echo_style)
{
    char_u	*r = پوچ;
    char_u	buf[MAX_FUNC_NAME_LEN];

    اگر (echo_style)
    {
	*tofree = پوچ;

	اگر (tv->vval.v_string == پوچ)
	    r = (char_u *)"function()";
	وگرنه
	{
	    r = make_ufunc_name_readable(tv->vval.v_string, buf,
							MAX_FUNC_NAME_LEN);
	    اگر (r == buf)
		r = *tofree = vim_strsave(buf);
	}
    }
    وگرنه
    {
	char_u *s = پوچ;

	اگر (tv->vval.v_string != پوچ)
	    s = make_ufunc_name_readable(tv->vval.v_string, buf,
							MAX_FUNC_NAME_LEN);

	r = *tofree = string_quote(s, TRUE);
    }

    بازگشت r;
}

/*
 * Return a textual representation of the object method in "tv", a VAR_PARTIAL.
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "echo_style" is FALSE, put quotes around the function name as
 * "function()", otherwise does not put quotes around function name.
 * May return NULL.
 */
    ایستا char_u *
method_tv2string(typval_T *tv, char_u **tofree, صحیح echo_style)
{
    char_u	buf[MAX_FUNC_NAME_LEN];
    partial_T	*pt = tv->vval.v_partial;

    size_t len = vim_snprintf((نویسه *)buf, اندازه(buf), "<SNR>%d_%s.%s",
			   pt->pt_func->uf_script_ctx.sc_sid,
			   pt->pt_func->uf_class->class_name.string,
			   pt->pt_func->uf_name);
    اگر (len >= اندازه(buf))
    {
	اگر (echo_style)
	{
	    *tofree = پوچ;
	    بازگشت (char_u *)"function()";
	}
	وگرنه
	    بازگشت *tofree = string_quote((char_u*)"", TRUE);
    }

    بازگشت *tofree = echo_style ? vim_strsave(buf) : string_quote(buf, TRUE);
}

/*
 * Return a textual representation of a partial in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * "numbuf" is used for a number.  May return NULL.
 */
    ایستا char_u *
partial_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    char_u	*numbuf,
    صحیح		copyID)
{
    char_u	*r = پوچ;
    partial_T	*pt;
    char_u	*fname;
    garray_T	ga;
    صحیح		i;
    char_u	*tf;

    pt = tv->vval.v_partial;
    fname = string_quote(pt == پوچ ? پوچ : partial_name(pt), FALSE);

    ga_init2(&ga, 1, 100);
    GA_CONCAT_LITERAL(&ga, "function(");
    اگر (fname != پوچ)
    {
	// When using uf_name prepend "g:" for a global function.
	اگر (pt != پوچ && pt->pt_name == پوچ && fname[0] == '\''
						&& vim_isupper(fname[1]))
	{
	    GA_CONCAT_LITERAL(&ga, "'g:");
	    ga_concat(&ga, fname + 1);
	}
	وگرنه
	    ga_concat(&ga, fname);
	vim_free(fname);
    }
    اگر (pt != پوچ && pt->pt_argc > 0)
    {
	GA_CONCAT_LITERAL(&ga, ", [");
	برای (i = 0; i < pt->pt_argc; ++i)
	{
	    اگر (i > 0)
		GA_CONCAT_LITERAL(&ga, ", ");
	    ga_concat(&ga, tv2string(&pt->pt_argv[i], &tf, numbuf, copyID));
	    vim_free(tf);
	}
	GA_CONCAT_LITERAL(&ga, "]");
    }
    اگر (pt != پوچ && pt->pt_dict != پوچ)
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

    بازگشت r;
}

/*
 * Return a textual representation of a List in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "copyID" is not zero replace recursive lists with "...".  When
 * "restore_copyID" is FALSE, repeated items in lists are replaced with "...".
 * May return NULL.
 */
    ایستا char_u *
list_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    صحیح		copyID,
    صحیح		restore_copyID)
{
    char_u	*r = پوچ;

    اگر (tv->vval.v_list == پوچ)
    {
	// NULL list is equivalent to empty list.
	*tofree = پوچ;
	r = (char_u *)"[]";
    }
    وگرنه اگر (copyID != 0 && tv->vval.v_list->lv_copyID == copyID
	    && tv->vval.v_list->lv_len > 0)
    {
	*tofree = پوچ;
	r = (char_u *)"[...]";
    }
    وگرنه
    {
	صحیح old_copyID;
	اگر (restore_copyID)
	    old_copyID = tv->vval.v_list->lv_copyID;

	tv->vval.v_list->lv_copyID = copyID;
	*tofree = list2string(tv, copyID, restore_copyID);
	اگر (restore_copyID)
	    tv->vval.v_list->lv_copyID = old_copyID;
	r = *tofree;
    }

    بازگشت r;
}

/*
 * Return a textual representation of a Tuple in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "copyID" is not zero replace recursive lists with "...".  When
 * "restore_copyID" is FALSE, repeated items in tuples are replaced with "...".
 * May return NULL.
 */
    ایستا char_u *
tuple_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    صحیح		copyID,
    صحیح		restore_copyID)
{
    tuple_T	*tuple = tv->vval.v_tuple;
    char_u	*r = پوچ;

    اگر (tuple == پوچ)
    {
	// NULL tuple is equivalent to an empty tuple.
	*tofree = پوچ;
	r = (char_u *)"()";
    }
    وگرنه اگر (copyID != 0 && tuple->tv_copyID == copyID
					&& tuple->tv_اقلام.ga_len > 0)
    {
	*tofree = پوچ;
	r = (char_u *)"(...)";
    }
    وگرنه
    {
	صحیح old_copyID;
	اگر (restore_copyID)
	    old_copyID = tuple->tv_copyID;

	tuple->tv_copyID = copyID;
	*tofree = tuple2string(tv, copyID, restore_copyID);
	اگر (restore_copyID)
	    tuple->tv_copyID = old_copyID;
	r = *tofree;
    }

    بازگشت r;
}

/*
 * Return a textual representation of a Dict in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "copyID" is not zero replace recursive dicts with "...".
 * When "restore_copyID" is FALSE, repeated items in the dictionary are
 * replaced with "...".  May return NULL.
 */
    ایستا char_u *
dict_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    صحیح		copyID,
    صحیح		restore_copyID)
{
    char_u	*r = پوچ;

    اگر (tv->vval.v_dict == پوچ)
    {
	// NULL dict is equivalent to empty dict.
	*tofree = پوچ;
	r = (char_u *)"{}";
    }
    وگرنه اگر (copyID != 0 && tv->vval.v_dict->dv_copyID == copyID
	    && tv->vval.v_dict->dv_hashtab.ht_used != 0)
    {
	*tofree = پوچ;
	r = (char_u *)"{...}";
    }
    وگرنه
    {
	صحیح old_copyID;
	اگر (restore_copyID)
	    old_copyID = tv->vval.v_dict->dv_copyID;

	tv->vval.v_dict->dv_copyID = copyID;
	*tofree = dict2string(tv, copyID, restore_copyID);
	اگر (restore_copyID)
	    tv->vval.v_dict->dv_copyID = old_copyID;
	r = *tofree;
    }

    بازگشت r;
}

/*
 * Return a textual representation of a job or a channel in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * "numbuf" is used for a number.
 * When "composite_val" is FALSE, put quotes around strings as "string()",
 * otherwise does not put quotes around strings.
 * May return NULL.
 */
    ایستا char_u *
jobchan_tv2string(
    typval_T	*tv UNUSED,
    char_u	**tofree UNUSED,
    char_u	*numbuf UNUSED,
    صحیح		composite_val UNUSED)
{
    char_u	*r = پوچ;

#ifdef FEAT_JOB_CHANNEL
    *tofree = پوچ;

    اگر (tv->v_type == VAR_JOB)
	r = job_to_string_buf(tv, numbuf);
    وگرنه
	r = channel_to_string_buf(tv, numbuf);

    اگر (composite_val)
    {
	*tofree = string_quote(r, FALSE);
	r = *tofree;
    }
#endif

    بازگشت r;
}

/*
 * Return a textual representation of a class in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * May return NULL.
 */
    ایستا char_u *
class_tv2string(typval_T *tv, char_u **tofree)
{
    char_u	*r = پوچ;
    size_t	rsize;
    class_T	*cl = tv->vval.v_class;
    string_T	class_name = {(char_u *)"[unknown]", 9};
    string_T	s = {(char_u *)"class", 5};

    اگر (cl != پوچ)
    {
	class_name.string = cl->class_name.string;
	class_name.length = cl->class_name.length;
	اگر (IS_INTERFACE(cl))
	    STR_LITERAL_SET(s, "interface");
	وگرنه اگر (IS_ENUM(cl))
	    STR_LITERAL_SET(s, "enum");
    }

    rsize = s.length + 1 + class_name.length + 1;
    r = *tofree = alloc(rsize);
    اگر (r != پوچ)
	vim_snprintf((نویسه *)r, rsize, "%s %s", s.string, (نویسه *)class_name.string);

    بازگشت r;
}

/*
 * Return a textual representation of an Object in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "copyID" is not zero replace recursive object with "...".
 * When "restore_copyID" is FALSE, repeated items in the object are
 * replaced with "...".  May return NULL.
 */
    ایستا char_u *
object_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    صحیح		copyID,
    صحیح		restore_copyID,
    char_u	*numbuf,
    صحیح		echo_style,
    صحیح		composite_val)
{
    char_u	*r = پوچ;

    object_T	*obj = tv->vval.v_object;
    اگر (obj == پوچ || obj->obj_class == پوچ)
    {
	*tofree = پوچ;
	r = (char_u *)"object of [unknown]";
    }
    وگرنه اگر (copyID != 0 && obj->obj_copyID == copyID
	    && obj->obj_class->class_obj_member_شمار != 0)
    {
	size_t n = 25 + obj->obj_class->class_name.length;
	r = alloc(n);
	اگر (r != پوچ)
	    (تهی)vim_snprintf((نویسه *)r, n, "object of %s {...}",
						obj->obj_class->class_name.string);
	*tofree = r;
    }
    وگرنه
    {
	صحیح old_copyID;
	اگر (restore_copyID)
	    old_copyID = obj->obj_copyID;

	obj->obj_copyID = copyID;
	*tofree = object2string(obj, numbuf, copyID, echo_style,
				restore_copyID, composite_val);
	اگر (restore_copyID)
	    obj->obj_copyID = old_copyID;
	r = *tofree;
    }

    بازگشت r;
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
    صحیح		copyID,
    صحیح		echo_style,
    صحیح		restore_copyID,
    صحیح		composite_val)
{
    ایستا صحیح	recurse = 0;
    char_u	*r = پوچ;

    اگر (recurse >= DICT_MAXNEST)
    {
	اگر (!did_echo_string_emsg)
	{
	    // Only give this message once for a recursive call to avoid
	    // flooding the user with errors.  And stop iterating over lists
	    // and dicts and objects.
	    did_echo_string_emsg = TRUE;
	    emsg(_(e_variable_nested_too_deep_for_displaying));
	}
	*tofree = پوچ;
	بازگشت (char_u *)"{E724}";
    }
    ++recurse;

    انتخاب (tv->v_type)
    {
	حالت VAR_STRING:
	    r = string_tv2string(tv, tofree, echo_style, composite_val);
	    بشکن;

	حالت VAR_FUNC:
	    r = func_tv2string(tv, tofree, echo_style);
	    بشکن;

	حالت VAR_PARTIAL:
	    اگر (tv->vval.v_partial == پوچ
		    || tv->vval.v_partial->pt_obj == پوچ)
		r = partial_tv2string(tv, tofree, numbuf, copyID);
	    وگرنه
		r = method_tv2string(tv, tofree, echo_style);
	    بشکن;

	حالت VAR_BLOB:
	    r = blob2string(tv->vval.v_blob, tofree, numbuf);
	    بشکن;

	حالت VAR_LIST:
	    r = list_tv2string(tv, tofree, copyID, restore_copyID);
	    بشکن;

	حالت VAR_TUPLE:
	    r = tuple_tv2string(tv, tofree, copyID, restore_copyID);
	    بشکن;

	حالت VAR_DICT:
	    r = dict_tv2string(tv, tofree, copyID, restore_copyID);
	    بشکن;

	حالت VAR_عدد:
	حالت VAR_UNKNOWN:
	حالت VAR_ANY:
	حالت VAR_VOID:
	    *tofree = پوچ;
	    r = tv_get_string_buf(tv, numbuf);
	    بشکن;

	حالت VAR_JOB:
	حالت VAR_CHANNEL:
	    r = jobchan_tv2string(tv, tofree, numbuf, composite_val);
	    بشکن;

	حالت VAR_INSTR:
	    *tofree = پوچ;
	    r = (char_u *)"instructions";
	    بشکن;

	حالت VAR_CLASS:
	    r = class_tv2string(tv, tofree);
	    بشکن;

	حالت VAR_OBJECT:
	    r = object_tv2string(tv, tofree, copyID, restore_copyID,
					 numbuf, echo_style, composite_val);
	    بشکن;

	حالت VAR_FLOAT:
	    *tofree = پوچ;
	    vim_snprintf((نویسه *)numbuf, NUMBUFLEN, "%g", tv->vval.v_float);
	    r = numbuf;
	    بشکن;

	حالت VAR_BOOL:
	حالت VAR_SPECIAL:
	    *tofree = پوچ;
	    r = (char_u *)get_var_special_name(tv->vval.v_عدد);
	    بشکن;

	حالت VAR_TYPEALIAS:
	    *tofree = vim_strsave(tv->vval.v_typealias->ta_name);
	    r = *tofree;
	    اگر (r == پوچ)
		r = (char_u *)"";
	    بشکن;
    }

    اگر (--recurse == 0)
	did_echo_string_emsg = FALSE;
    بازگشت r;
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
    صحیح		copyID)
{
    بازگشت echo_string_core(tv, tofree, numbuf, copyID, TRUE, FALSE, FALSE);
}

/*
 * Convert the specified byte index of line 'lnum' in buffer 'buf' to a
 * character index.  Works only for loaded buffers. Returns -1 on failure.
 * The index of the first byte and the first character is zero.
 */
    صحیح
buf_byteidx_to_charidx(buf_T *buf, صحیح lnum, صحیح byteidx)
{
    char_u	*str;
    char_u	*t;
    صحیح		شمار;

    اگر (buf == پوچ || buf->b_ml.ml_mfp == پوچ)
	بازگشت -1;

    اگر (lnum > buf->b_ml.ml_سطر_شمار)
	lnum = buf->b_ml.ml_سطر_شمار;

    str = ml_get_buf(buf, lnum, FALSE);
    اگر (str == پوچ)
	بازگشت -1;

    اگر (*str == NUL)
	بازگشت 0;

    // count the number of characters
    t = str;
    برای (شمار = 0; *t != NUL && t <= str + byteidx; شمار++)
	t += mb_ptr2len(t);

    // In insert mode, when the cursor is at the end of a non-empty line,
    // byteidx points to the NUL character immediately past the end of the
    // string. In this case, add one to the character count.
    اگر (*t == NUL && byteidx != 0 && t == str + byteidx)
	شمار++;

    بازگشت شمار - 1;
}

/*
 * Convert the specified character index of line 'lnum' in buffer 'buf' to a
 * byte index.  Works only for loaded buffers. Returns -1 on failure.
 * The index of the first byte and the first character is zero.
 */
    صحیح
buf_charidx_to_byteidx(buf_T *buf, صحیح lnum, صحیح charidx)
{
    char_u	*str;
    char_u	*t;

    اگر (buf == پوچ || buf->b_ml.ml_mfp == پوچ)
	بازگشت -1;

    اگر (lnum > buf->b_ml.ml_سطر_شمار)
	lnum = buf->b_ml.ml_سطر_شمار;

    str = ml_get_buf(buf, lnum, FALSE);
    اگر (str == پوچ)
	بازگشت -1;

    // Convert the character offset to a byte offset
    t = str;
    وقتی (*t != NUL && --charidx > 0)
	t += mb_ptr2len(t);

    بازگشت t - str;
}

/*
 * Translate a String variable into a position.
 * Returns NULL when there is an error.
 */
    pos_T *
var2fpos(
    typval_T	*varp,
    صحیح		dollar_lnum,	// TRUE when $ is last line
    صحیح		*fnum,		// set to fnum for '0, 'A, etc.
    صحیح		charcol)	// return character column
{
    char_u		*name;
    ایستا pos_T	pos;
    pos_T		*pp;
    صحیح			error = FALSE;

    // Argument can be [lnum, col, coladd].
    اگر (varp->v_type == VAR_LIST)
    {
	list_T		*l;
	صحیح		len;
	listitem_T	*li;

	l = varp->vval.v_list;
	اگر (l == پوچ)
	    بازگشت پوچ;

	// Get the line number
	pos.lnum = list_find_nr(l, 0L, &error);
	اگر (error || pos.lnum <= 0 || pos.lnum > curbuf->b_ml.ml_سطر_شمار)
	    بازگشت پوچ;	// invalid line number
	اگر (charcol)
	    len = (long)mb_charlen(ml_get(pos.lnum));
	وگرنه
	    len = (long)ml_get_len(pos.lnum);

	// Get the column number
	// We accept "$" for the column number: last column.
	li = list_find(l, 1L);
	اگر (li != پوچ && li->li_tv.v_type == VAR_STRING
		&& li->li_tv.vval.v_string != پوچ
		&& STRCMP(li->li_tv.vval.v_string, "$") == 0)
	{
	    pos.col = len + 1;
	}
	وگرنه
	{
	    pos.col = list_find_nr(l, 1L, &error);
	    اگر (error)
		بازگشت پوچ;
	}

	// Accept a position up to the NUL after the line.
	اگر (pos.col == 0 || (صحیح)pos.col > len + 1)
	    بازگشت پوچ;	// invalid column number
	--pos.col;

	// Get the virtual offset.  Defaults to zero.
	pos.coladd = list_find_nr(l, 2L, &error);
	اگر (error)
	    pos.coladd = 0;

	بازگشت &pos;
    }

    اگر (in_vim9script() && check_for_string_arg(varp, 0) == FAIL)
	بازگشت پوچ;

    name = tv_get_string_chk(varp);
    اگر (name == پوچ)
	بازگشت پوچ;

    error = TRUE;
    pos.lnum = 0;
    اگر (name[0] == '.')
    {
	اگر (!in_vim9script() || name[1] == NUL)
	{
	    error = FALSE;
	    // cursor
	    pos = curwin->w_cursor;
	}
    }
    وگرنه اگر (name[0] == 'v' && name[1] == NUL)
    {
	// Visual start
	اگر (VIsual_active)
	    pos = VIsual;
	وگرنه
	    pos = curwin->w_cursor;
    }
    وگرنه اگر (name[0] == '\'')
    {
	اگر (!in_vim9script() || (name[1] != NUL && name[2] == NUL))
	{
	    error = FALSE;
	    // mark
	    pp = getmark_buf_fnum(curbuf, name[1], FALSE, fnum);
	    اگر (pp == پوچ || pp == (pos_T *)-1 || pp->lnum <= 0)
		بازگشت پوچ;
	    pos = *pp;
	}
    }
    اگر (pos.lnum != 0)
    {
	اگر (charcol)
	    pos.col = buf_byteidx_to_charidx(curbuf, pos.lnum, pos.col);
	بازگشت &pos;
    }

    pos.coladd = 0;

    اگر (name[0] == 'w' && dollar_lnum)
    {
	// the "w_valid" flags are not reset when moving the cursor, but they
	// do matter for update_topline() and validate_botline().
	check_cursor_moved(curwin);

	pos.col = 0;
	اگر (name[1] == '0')		// "w0": first visible line
	{
#ifdef FEAT_TERMINAL
	    اگر (bt_terminal(curwin->w_buffer)
				    && curwin->w_buffer->b_term != پوچ
				    && !term_in_normal_mode(curwin->w_buffer))
		may_move_terminal_to_buffer(curwin->w_buffer->b_term, TRUE);
#endif
	    update_topline();
	    // In silent Ex mode topline is zero, but that's not a valid line
	    // number; use one instead.
	    pos.lnum = curwin->w_topline > 0 ? curwin->w_topline : 1;
	    بازگشت &pos;
	}
	وگرنه اگر (name[1] == '$')	// "w$": last visible line
	{
#ifdef FEAT_TERMINAL
	    اگر (bt_terminal(curwin->w_buffer)
				    && curwin->w_buffer->b_term != پوچ
				    && !term_in_normal_mode(curwin->w_buffer))
		may_move_terminal_to_buffer(curwin->w_buffer->b_term, TRUE);
#endif
	    validate_botline();
	    // In silent Ex mode botline is zero, return zero then.
	    pos.lnum = curwin->w_botline > 0 ? curwin->w_botline - 1 : 0;
	    بازگشت &pos;
	}
    }
    وگرنه اگر (name[0] == '$')		// last column or line
    {
	اگر (dollar_lnum)
	{
	    pos.lnum = curbuf->b_ml.ml_سطر_شمار;
	    pos.col = 0;
	}
	وگرنه
	{
	    pos.lnum = curwin->w_cursor.lnum;
	    اگر (charcol)
		pos.col = (colnr_T)mb_charlen(ml_get_curline());
	    وگرنه
		pos.col = ml_get_curline_len();
	}
	بازگشت &pos;
    }
    اگر (in_vim9script() && error)
	semsg(_(e_invalid_مقدار_for_سطر_عدد_str), name);
    بازگشت پوچ;
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
    صحیح
list2fpos(
    typval_T	*arg,
    pos_T	*posp,
    صحیح		*fnump,
    colnr_T	*curswantp,
    صحیح		charcol)
{
    list_T	*l = arg->vval.v_list;
    long	i = 0;
    long	n;

    // List must be: [fnum, lnum, col, coladd, curswant], where "fnum" is only
    // there when "fnump" isn't NULL; "coladd" and "curswant" are optional.
    اگر (arg->v_type != VAR_LIST
	    || l == پوچ
	    || l->lv_len < (fnump == پوچ ? 2 : 3)
	    || l->lv_len > (fnump == پوچ ? 4 : 5))
	بازگشت FAIL;

    اگر (fnump != پوچ)
    {
	n = list_find_nr(l, i++, پوچ);	// fnum
	اگر (n < 0)
	    بازگشت FAIL;
	اگر (n == 0)
	    n = curbuf->b_fnum;		// current buffer
	*fnump = n;
    }

    n = list_find_nr(l, i++, پوچ);	// lnum
    اگر (n < 0)
	بازگشت FAIL;
    posp->lnum = n;

    n = list_find_nr(l, i++, پوچ);	// col
    اگر (n < 0)
	بازگشت FAIL;
    // If character position is specified, then convert to byte position
    // If the line number is zero use the cursor line.
    اگر (charcol)
    {
	buf_T	*buf;

	// Get the text for the specified line in a loaded buffer
	buf = buflist_findnr(fnump == پوچ ? curbuf->b_fnum : *fnump);
	اگر (buf == پوچ || buf->b_ml.ml_mfp == پوچ)
	    بازگشت FAIL;

	n = buf_charidx_to_byteidx(buf,
		  posp->lnum == 0 ? curwin->w_cursor.lnum : posp->lnum, n) + 1;
    }
    posp->col = n;

    n = list_find_nr(l, i, پوچ);	// off
    اگر (n < 0)
	posp->coladd = 0;
    وگرنه
	posp->coladd = n;

    اگر (curswantp != پوچ)
	*curswantp = list_find_nr(l, i + 1, پوچ);  // curswant

    بازگشت OK;
}

/*
 * Get the length of an environment variable name.
 * Advance "arg" to the first character after the name.
 * Return 0 for error.
 */
    صحیح
get_env_len(char_u **arg)
{
    char_u	*p;
    صحیح		len;

    برای (p = *arg; vim_isIDc(*p); ++p)
	;
    اگر (p == *arg)	    // no name found
	بازگشت 0;

    len = (صحیح)(p - *arg);
    *arg = p;
    بازگشت len;
}

/*
 * Get the length of the name of a function or internal variable.
 * "arg" is advanced to after the name.
 * Return 0 if something is wrong.
 */
    صحیح
get_id_len(char_u **arg)
{
    char_u	*p;
    صحیح		len;

    // Find the end of the name.
    برای (p = *arg; eval_isnamec(*p); ++p)
    {
	اگر (*p == ':')
	{
	    // "s:" is start of "s:var", but "n:" is not and can be used in
	    // slice "[n:]".  Also "xx:" is not a namespace.
	    len = (صحیح)(p - *arg);
	    اگر ((len == 1 && vim_strchr(NAMESPACE_CHAR, **arg) == پوچ)
		    || len > 1)
		بشکن;
	}
    }
    اگر (p == *arg)	    // no name found
	بازگشت 0;

    len = (صحیح)(p - *arg);
    *arg = p;

    بازگشت len;
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
    صحیح
get_name_len(
    char_u	**arg,
    char_u	**alias,
    صحیح		evaluate,
    صحیح		verbose)
{
    صحیح		len;
    char_u	*p;
    char_u	*عبارت_start;
    char_u	*عبارت_end;

    *alias = پوچ;  // default to no alias

    اگر ((*arg)[0] == K_SPECIAL && (*arg)[1] == KS_EXTRA
						  && (*arg)[2] == (صحیح)KE_SNR)
    {
	// hard coded <SNR>, already translated
	*arg += 3;
	بازگشت get_id_len(arg) + 3;
    }
    len = eval_fname_script(*arg);
    اگر (len > 0)
    {
	// literal "<SID>", "s:" or "<SNR>"
	*arg += len;
    }

    /*
     * Find the end of the name; check for {} construction.
     */
    p = find_name_end(*arg, &عبارت_start, &عبارت_end,
					       len > 0 ? 0 : FNE_CHECK_START);
    اگر (عبارت_start != پوچ)
    {
	char_u	*temp_string;

	اگر (!evaluate)
	{
	    len += (صحیح)(p - *arg);
	    *arg = skipwhite(p);
	    بازگشت len;
	}

	/*
	 * Include any <SID> etc in the expanded string:
	 * Thus the -len here.
	 */
	temp_string = make_expanded_name(*arg - len, عبارت_start, عبارت_end, p);
	اگر (temp_string == پوچ)
	    بازگشت -1;
	*alias = temp_string;
	*arg = skipwhite(p);
	بازگشت (صحیح)STRLEN(temp_string);
    }

    len += get_id_len(arg);
    // Only give an error when there is something, otherwise it will be
    // reported at a higher level.
    اگر (len == 0 && verbose && **arg != NUL)
	semsg(_(e_invalid_expression_str), *arg);

    بازگشت len;
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
    char_u	**عبارت_start,
    char_u	**عبارت_end,
    صحیح		flags)
{
    صحیح		mb_nest = 0;
    صحیح		br_nest = 0;
    char_u	*p;
    صحیح		len;
    صحیح		allow_curly = !in_vim9script();

    اگر (عبارت_start != پوچ)
    {
	*عبارت_start = پوچ;
	*عبارت_end = پوچ;
    }

    // Quick check for valid starting character.
    اگر ((flags & FNE_CHECK_START) && !eval_isnamec1(*arg)
					      && (*arg != '{' || !allow_curly))
	بازگشت arg;

    برای (p = arg; *p != NUL
		    && (eval_isnamec(*p)
			|| (*p == '{' && allow_curly)
			|| ((flags & FNE_INCL_BR) && (*p == '['
					 || (*p == '.' && eval_isdictc(p[1]))))
			|| mb_nest != 0
			|| br_nest != 0); MB_PTR_ADV(p))
    {
	اگر (*p == '\'')
	{
	    // skip over 'string' to avoid counting [ and ] inside it.
	    برای (p = p + 1; *p != NUL && *p != '\''; MB_PTR_ADV(p))
		;
	    اگر (*p == NUL)
		بشکن;
	}
	وگرنه اگر (*p == '"')
	{
	    // skip over "str\"ing" to avoid counting [ and ] inside it.
	    برای (p = p + 1; *p != NUL && *p != '"'; MB_PTR_ADV(p))
		اگر (*p == '\\' && p[1] != NUL)
		    ++p;
	    اگر (*p == NUL)
		بشکن;
	}
	وگرنه اگر (br_nest == 0 && mb_nest == 0 && *p == ':')
	{
	    // "s:" is start of "s:var", but "n:" is not and can be used in
	    // slice "[n:]".  Also "xx:" is not a namespace. But {ns}: is.
	    len = (صحیح)(p - arg);
	    اگر ((len == 1 && vim_strchr(NAMESPACE_CHAR, *arg) == پوچ)
		    || (len > 1 && p[-1] != '}'))
		بشکن;
	}

	اگر (mb_nest == 0)
	{
	    اگر (*p == '[')
		++br_nest;
	    وگرنه اگر (*p == ']')
		--br_nest;
	}

	اگر (br_nest == 0 && allow_curly)
	{
	    اگر (*p == '{')
	    {
		mb_nest++;
		اگر (عبارت_start != پوچ && *عبارت_start == پوچ)
		    *عبارت_start = p;
	    }
	    وگرنه اگر (*p == '}')
	    {
		mb_nest--;
		اگر (عبارت_start != پوچ && mb_nest == 0 && *عبارت_end == پوچ)
		    *عبارت_end = p;
	    }
	}
    }

    بازگشت p;
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
    ایستا char_u *
make_expanded_name(
    char_u	*in_start,
    char_u	*عبارت_start,
    char_u	*عبارت_end,
    char_u	*in_end)
{
    char_u	c1;
    char_u	*retval = پوچ;
    char_u	*temp_result;

    اگر (عبارت_end == پوچ || in_end == پوچ)
	بازگشت پوچ;
    *عبارت_start	= NUL;
    *عبارت_end = NUL;
    c1 = *in_end;
    *in_end = NUL;

    temp_result = eval_to_string(عبارت_start + 1, FALSE, FALSE);
    اگر (temp_result != پوچ)
    {
	size_t	retvalsize = (size_t)(عبارت_start - in_start)
				+ STRLEN(temp_result)
				+ (size_t)(in_end - عبارت_end) + 1;

	retval = alloc(retvalsize);
	اگر (retval != پوچ)
	    vim_snprintf((نویسه *)retval, retvalsize, "%s%s%s",
				in_start, temp_result, عبارت_end + 1);
    }
    vim_free(temp_result);

    *in_end = c1;		// put char back for error messages
    *عبارت_start = '{';
    *عبارت_end = '}';

    اگر (retval != پوچ)
    {
	temp_result = find_name_end(retval, &عبارت_start, &عبارت_end, 0);
	اگر (عبارت_start != پوچ)
	{
	    // Further expansion!
	    temp_result = make_expanded_name(retval, عبارت_start,
						       عبارت_end, temp_result);
	    vim_free(retval);
	    retval = temp_result;
	}
    }

    بازگشت retval;
}

/*
 * Return TRUE if character "c" can be used in a variable or function name.
 * Does not include '{' or '}' for magic braces.
 */
    صحیح
eval_isnamec(صحیح c)
{
    بازگشت ASCII_ISALNUM(c) || c == '_' || c == ':' || c == AUTOLOAD_CHAR;
}

/*
 * Return TRUE if character "c" can be used as the first character in a
 * variable or function name (excluding '{' and '}').
 */
    صحیح
eval_isnamec1(صحیح c)
{
    بازگشت ASCII_ISALPHA(c) || c == '_';
}

/*
 * Return TRUE if character "c" can be used as the first character of a
 * dictionary key.
 */
    صحیح
eval_isdictc(صحیح c)
{
    بازگشت ASCII_ISALNUM(c) || c == '_';
}

/*
 * Return true when "p" points to "->" followed by a method name or a lambda.
 * Only called for Vim9 script.
 */
    ایستا bool
method_call_follows(char_u *p)
{
    اگر (p[0] != '-' || p[1] != '>')
	بازگشت false;

    char_u	*after = skipwhite(p + 2);

    بازگشت ASCII_ISALPHA(*after) || *after == '(';
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
    صحیح
handle_subscript(
    char_u	**arg,
    char_u	*name_start,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    صحیح		verbose)	// give error messages
{
    صحیح		evaluate = evalarg != پوچ
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    صحیح		ret = OK;
    dict_T	*selfdict = پوچ;
    صحیح		check_white = TRUE;
    صحیح		getnext;
    char_u	*p;

    وقتی (ret == OK)
    {
	// When at the end of the line and ".name" or a method call follows in
	// the next line then consume the line break.
	p = eval_next_non_blank(*arg, evalarg, &getnext);
	اگر (getnext
	    && ((*p == '.'
		    && ((rettv->v_type == VAR_DICT && eval_isdictc(p[1]))
			|| rettv->v_type == VAR_CLASS
			|| rettv->v_type == VAR_OBJECT))
		|| method_call_follows(p)))
	{
	    *arg = eval_next_سطر(*arg, evalarg);
	    p = *arg;
	    check_white = FALSE;
	}

	اگر (rettv->v_type == VAR_ANY)
	{
	    char_u	*exp_name;
	    صحیح		cc;
	    صحیح		idx;
	    ufunc_T	*ufunc;
	    type_T	*type;

	    // Found script from "import {name} as name", script item name must
	    // follow.  "rettv->vval.v_number" has the script ID.
	    اگر (**arg != '.')
	    {
		اگر (verbose)
		    semsg(_(e_expected_dot_after_name_str),
					name_start != پوچ ? name_start: *arg);
		ret = FAIL;
		بشکن;
	    }
	    ++*arg;
	    اگر (IS_WHITE_OR_NUL(**arg))
	    {
		اگر (verbose)
		    emsg(_(e_no_white_space_allowed_after_dot));
		ret = FAIL;
		بشکن;
	    }

	    // isolate the name
	    exp_name = *arg;
	    وقتی (eval_isnamec(**arg))
		++*arg;
	    cc = **arg;
	    **arg = NUL;

	    idx = find_exported(rettv->vval.v_عدد, exp_name, &ufunc, &type,
		       evalarg == پوچ ? پوچ : evalarg->eval_cctx,
		       evalarg == پوچ ? پوچ : evalarg->eval_cstack, verbose);
	    **arg = cc;

	    اگر (idx < 0 && ufunc == پوچ)
	    {
		ret = FAIL;
		بشکن;
	    }
	    اگر (idx >= 0)
	    {
		scriptitem_T    *si = SCRIPT_ITEM(rettv->vval.v_عدد);
		svar_T		*sv = ((svar_T *)si->sn_var_vals.ga_data) + idx;

		copy_tv(sv->sv_tv, rettv);
	    }
	    وگرنه
	    {
		rettv->v_type = VAR_FUNC;
		اگر (**arg == '<')
		{
		    char_u *s = get_generic_func_name(ufunc, arg);
		    اگر (s != پوچ)
			rettv->vval.v_string = s;
		    وگرنه
			ret = FAIL;
		}
		وگرنه
		{
		    rettv->vval.v_string =
			vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
		}
	    }
	    ادامه;
	}

	اگر ((**arg == '(' && (!evaluate || rettv->v_type == VAR_FUNC
			    || rettv->v_type == VAR_PARTIAL))
		    && (!check_white || !VIM_ISWHITE(*(*arg - 1))))
	{
	    ret = call_func_rettv(arg, evalarg, rettv, evaluate,
							       selfdict, پوچ);

	    // Stop the expression evaluation when immediately aborting on
	    // error, or when an interrupt occurred or an exception was thrown
	    // but not caught.
	    اگر (aborting())
	    {
		اگر (ret == OK)
		    clear_tv(rettv);
		ret = FAIL;
	    }
	    dict_unref(selfdict);
	    selfdict = پوچ;
	}
	وگرنه اگر (p[0] == '-' && p[1] == '>')
	{
	    اگر (in_vim9script())
		*arg = skipwhite(p + 2);
	    وگرنه
		*arg = p + 2;
	    اگر (ret == OK && evaluate && rettv->v_type == VAR_VOID)
	    {
		اگر (verbose)
		    emsg(_(e_cannot_use_void_مقدار));
		ret = FAIL;
	    }
	    وگرنه اگر (VIM_ISWHITE(**arg))
	    {
		emsg(_(e_no_white_space_allowed_before_parenthesis));
		ret = FAIL;
	    }
	    وگرنه اگر ((**arg == '{' && !in_vim9script()) || **arg == '(')
		// expr->{lambda}() or expr->(lambda)()
		ret = eval_lambda(arg, rettv, evalarg, verbose);
	    وگرنه
		// expr->name()
		ret = eval_method(arg, rettv, evalarg, verbose);
	}
	// "." is ".name" lookup when we found a dict or when evaluating and
	// scriptversion is at least 2, where string concatenation is "..".
	وگرنه اگر (**arg == '['
		|| (**arg == '.' && (rettv->v_type == VAR_DICT
			|| (!evaluate
			    && (*arg)[1] != '.'
			    && !in_old_script(2)))))
	{
	    dict_unref(selfdict);
	    اگر (rettv->v_type == VAR_DICT)
	    {
		selfdict = rettv->vval.v_dict;
		اگر (selfdict != پوچ)
		    ++selfdict->dv_refcount;
	    }
	    وگرنه
		selfdict = پوچ;
	    اگر (eval_index(arg, rettv, evalarg, verbose) == FAIL)
	    {
		clear_tv(rettv);
		ret = FAIL;
	    }
	}
	وگرنه اگر (**arg == '.' && (rettv->v_type == VAR_CLASS
					       || rettv->v_type == VAR_OBJECT))
	{
	    // class member: SomeClass.varname
	    // class method: SomeClass.SomeMethod()
	    // class constructor: SomeClass.new()
	    // object member: someObject.varname
	    // object method: someObject.SomeMethod()
	    اگر (class_object_index(arg, rettv, evalarg, verbose) == FAIL)
	    {
		clear_tv(rettv);
		ret = FAIL;
	    }
	}
	وگرنه
	    بشکن;
    }

    // Turn "dict.Func" into a partial for "Func" bound to "dict".
    // Don't do this when "Func" is already a partial that was bound
    // explicitly (pt_auto is FALSE).
    اگر (selfdict != پوچ
	    && (rettv->v_type == VAR_FUNC
		|| (rettv->v_type == VAR_PARTIAL
		    && (rettv->vval.v_partial->pt_auto
			|| rettv->vval.v_partial->pt_dict == پوچ))))
	selfdict = make_partial(selfdict, rettv);

    dict_unref(selfdict);
    بازگشت ret;
}

/*
 * Make a copy of an item.
 * Lists and Dictionaries are also copied.  A deep copy if "deep" is set.
 * "top" is TRUE for the toplevel of copy().
 * For deepcopy() "copyID" is zero for a full copy or the ID for when a
 * reference to an already copied list/dict can be used.
 * Returns FAIL or OK.
 */
    صحیح
item_copy(
    typval_T	*from,
    typval_T	*to,
    صحیح		deep,
    صحیح		بالاترین,
    صحیح		copyID)
{
    ایستا صحیح	recurse = 0;
    صحیح		ret = OK;

    اگر (recurse >= DICT_MAXNEST)
    {
	emsg(_(e_variable_nested_too_deep_for_making_copy));
	بازگشت FAIL;
    }
    ++recurse;

    انتخاب (from->v_type)
    {
	حالت VAR_عدد:
	حالت VAR_FLOAT:
	حالت VAR_STRING:
	حالت VAR_FUNC:
	حالت VAR_PARTIAL:
	حالت VAR_BOOL:
	حالت VAR_SPECIAL:
	حالت VAR_JOB:
	حالت VAR_CHANNEL:
	حالت VAR_INSTR:
	حالت VAR_CLASS:
	حالت VAR_OBJECT:
	حالت VAR_TYPEALIAS:
	    copy_tv(from, to);
	    بشکن;
	حالت VAR_LIST:
	    to->v_type = VAR_LIST;
	    to->v_lock = 0;
	    اگر (from->vval.v_list == پوچ)
		to->vval.v_list = پوچ;
	    وگرنه اگر (copyID != 0 && from->vval.v_list->lv_copyID == copyID)
	    {
		// use the copy made earlier
		to->vval.v_list = from->vval.v_list->lv_copylist;
		++to->vval.v_list->lv_refcount;
	    }
	    وگرنه
	    {
		to->vval.v_list = list_copy(from->vval.v_list,
							    deep, بالاترین, copyID);
		اگر (to->vval.v_list == پوچ)
		    ret = FAIL;
	    }
	    بشکن;
	حالت VAR_TUPLE:
	    to->v_type = VAR_TUPLE;
	    to->v_lock = 0;
	    اگر (from->vval.v_tuple == پوچ)
		to->vval.v_tuple = پوچ;
	    وگرنه اگر (copyID != 0 && from->vval.v_tuple->tv_copyID == copyID)
	    {
		// use the copy made earlier
		to->vval.v_tuple = from->vval.v_tuple->tv_copytuple;
		++to->vval.v_tuple->tv_refcount;
	    }
	    وگرنه
	    {
		to->vval.v_tuple = tuple_copy(from->vval.v_tuple,
							    deep, بالاترین, copyID);
		اگر (to->vval.v_tuple == پوچ)
		    ret = FAIL;
	    }
	    بشکن;
	حالت VAR_BLOB:
	    to->v_type = VAR_BLOB;
	    to->v_lock = 0;
	    اگر (from->vval.v_blob == پوچ)
		to->vval.v_blob = پوچ;
	    وگرنه
	    {
		to->vval.v_blob = blob_copy(from->vval.v_blob);
		اگر (to->vval.v_blob == پوچ)
		    ret = FAIL;
	    }
	    بشکن;
	حالت VAR_DICT:
	    to->v_type = VAR_DICT;
	    to->v_lock = 0;
	    اگر (from->vval.v_dict == پوچ)
		to->vval.v_dict = پوچ;
	    وگرنه اگر (copyID != 0 && from->vval.v_dict->dv_copyID == copyID)
	    {
		// use the copy made earlier
		to->vval.v_dict = from->vval.v_dict->dv_copydict;
		++to->vval.v_dict->dv_refcount;
	    }
	    وگرنه
	    {
		to->vval.v_dict = dict_copy(from->vval.v_dict,
							    deep, بالاترین, copyID);
		اگر (to->vval.v_dict == پوچ)
		    ret = FAIL;
	    }
	    بشکن;
	حالت VAR_UNKNOWN:
	حالت VAR_ANY:
	حالت VAR_VOID:
	    internal_error_no_abort("item_copy(UNKNOWN)");
	    ret = FAIL;
    }
    --recurse;
    بازگشت ret;
}

    تهی
echo_one(typval_T *rettv, صحیح with_space, صحیح *atstart, صحیح *needclr)
{
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    char_u	*p = echo_string(rettv, &tofree, numbuf, get_copyID());

    اگر (*atstart)
    {
	*atstart = FALSE;
	// Call msg_start() after eval1(), evaluating the expression
	// may cause a message to appear.
	اگر (with_space)
	{
	    // Mark the saved text as finishing the line, so that what
	    // follows is displayed on a new line when scrolling back
	    // at the more prompt.
	    پیام_sb_eol();
	    پیام_start();
	}
    }
    وگرنه اگر (with_space)
	پیام_puts_attr(" ", echo_attr);

    اگر (p != پوچ)
	برای ( ; *p != NUL && !got_int; ++p)
	{
	    اگر (*p == '\n' || *p == '\r' || *p == TAB)
	    {
		اگر (*p != TAB && *needclr)
		{
		    // remove any text still there from the command
		    پیام_clr_eos();
		    *needclr = FALSE;
		}
		پیام_putchar_attr(*p, echo_attr);
	    }
	    وگرنه
	    {
		اگر (has_mbyte)
		{
		    صحیح i = (*mb_ptr2len)(p);

		    (تهی)پیام_outtrans_len_attr(p, i, echo_attr);
		    p += i - 1;
		}
		وگرنه
		    (تهی)پیام_outtrans_len_attr(p, 1, echo_attr);
	    }
	}
    vim_free(tofree);
}

/*
 * ":echo expr1 ..."	print each argument separated with a space, add a
 *			newline at the end.
 * ":echon expr1 ..."	print each argument plain.
 */
    تهی
ex_echo(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    typval_T	rettv;
    char_u	*arg_start;
    صحیح		needclr = TRUE;
    صحیح		atstart = TRUE;
    صحیح		did_emsg_before = did_emsg;
    صحیح		called_emsg_before = called_emsg;
    evalarg_T	evalarg;

    fill_evalarg_from_eap(&evalarg, eap, eap->skip);

    اگر (eap->skip)
	++emsg_skip;
    وقتی ((!ends_excmd2(eap->cmd, arg) || *arg == '"') && !got_int)
    {
	// If eval1() causes an error message the text from the command may
	// still need to be cleared. E.g., "echo 22,44".
	need_clr_eos = needclr;

	arg_start = arg;
	اگر (eval1(&arg, &rettv, &evalarg) == FAIL)
	{
	    /*
	     * Report the invalid expression unless the expression evaluation
	     * has been cancelled due to an aborting error, an interrupt, or an
	     * exception.
	     */
	    اگر (!aborting() && did_emsg == did_emsg_before
					  && called_emsg == called_emsg_before)
		semsg(_(e_invalid_expression_str), arg_start);
	    need_clr_eos = FALSE;
	    بشکن;
	}
	need_clr_eos = FALSE;

	اگر (!eap->skip)
	{
	    اگر (rettv.v_type == VAR_VOID)
	    {
		semsg(_(e_expression_does_not_result_in_مقدار_str), arg_start);
		بشکن;
	    }
	    echo_one(&rettv, eap->cmdidx == CMD_echo, &atstart, &needclr);
	}

	clear_tv(&rettv);
	arg = skipwhite(arg);
    }
    set_nextcmd(eap, arg);
    clear_evalarg(&evalarg, eap);

    اگر (eap->skip)
	--emsg_skip;
    وگرنه
    {
	// remove text that may still be there from the command
	اگر (needclr)
	    پیام_clr_eos();
	اگر (eap->cmdidx == CMD_echo)
	    پیام_end();
    }
}

/*
 * ":echohl {name}".
 */
    تهی
ex_echohl(exarg_T *eap)
{
    echo_attr = syn_name2attr(eap->arg);
}

/*
 * Returns the :echo attribute
 */
    صحیح
get_echo_attr(تهی)
{
    بازگشت echo_attr;
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
    تهی
ex_execute(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    typval_T	rettv;
    صحیح		ret = OK;
    char_u	*p;
    garray_T	ga;
    صحیح		len;
    long	start_lnum = SOURCING_LNUM;

    ga_init2(&ga, 1, 80);

    اگر (eap->skip)
	++emsg_skip;
    وقتی (!ends_excmd2(eap->cmd, arg) || *arg == '"')
    {
	ret = eval1_emsg(&arg, &rettv, eap);
	اگر (ret == FAIL)
	    بشکن;

	اگر (!eap->skip)
	{
	    char_u   buf[NUMBUFLEN];

	    اگر (eap->cmdidx == CMD_execute)
	    {
		اگر (rettv.v_type == VAR_CHANNEL || rettv.v_type == VAR_JOB)
		{
		    semsg(_(e_using_invalid_مقدار_as_string_str),
						  vartype_name(rettv.v_type));
		    p = پوچ;
		}
		وگرنه
		    p = tv_get_string_buf(&rettv, buf);
	    }
	    وگرنه
		p = tv_stringify(&rettv, buf);
	    اگر (p == پوچ)
	    {
		clear_tv(&rettv);
		ret = FAIL;
		بشکن;
	    }
	    len = (صحیح)STRLEN(p);
	    اگر (ga_grow(&ga, len + 2) == FAIL)
	    {
		clear_tv(&rettv);
		ret = FAIL;
		بشکن;
	    }
	    اگر (ga.ga_len)
		((char_u *)(ga.ga_data))[ga.ga_len++] = ' ';
	    STRCPY((char_u *)(ga.ga_data) + ga.ga_len, p);
	    ga.ga_len += len;
	}

	clear_tv(&rettv);
	arg = skipwhite(arg);
    }

    اگر (ret != FAIL && ga.ga_data != پوچ)
    {
	// use the first line of continuation lines for messages
	SOURCING_LNUM = start_lnum;

	اگر (eap->cmdidx == CMD_echomsg
		|| eap->cmdidx == CMD_echowindow
		|| eap->cmdidx == CMD_echoerr)
	{
	    // Mark the already saved text as finishing the line, so that what
	    // follows is displayed on a new line when scrolling back at the
	    // more prompt.
	    پیام_sb_eol();
	}

	اگر (eap->cmdidx == CMD_echomsg)
	{
	    پیام_attr(ga.ga_data, echo_attr);
	    out_flush();
	}
	وگرنه اگر (eap->cmdidx == CMD_echowindow)
	{
#ifdef HAS_MESSAGE_WINDOW
	    start_echowindow(eap->addr_شمار > 0 ? eap->line2 : 0);
#endif
	    پیام_attr(ga.ga_data, echo_attr);
#ifdef HAS_MESSAGE_WINDOW
	    end_echowindow();
#endif
	}
	وگرنه اگر (eap->cmdidx == CMD_echoconsole)
	{
	    ui_write(ga.ga_data, (صحیح)STRLEN(ga.ga_data), TRUE);
	    ui_write((char_u *)"\r\n", 2, TRUE);
	}
	وگرنه اگر (eap->cmdidx == CMD_echoerr)
	{
	    صحیح		save_did_emsg = did_emsg;

	    // We don't want to abort following commands, restore did_emsg.
	    emsg(ga.ga_data);
	    اگر (!force_abort)
		did_emsg = save_did_emsg;
	}
	وگرنه اگر (eap->cmdidx == CMD_execute)
	{
	    صحیح save_sticky_cmdmod_flags = sticky_cmdmod_flags;

	    // "legacy exe cmd" and "vim9cmd exe cmd" applies to "cmd".
	    sticky_cmdmod_flags = cmdmod.cmod_flags
						& (CMOD_LEGACY | CMOD_VIM9CMD);
	    do_cmdline((char_u *)ga.ga_data,
		       eap->ea_getline, eap->cookie, DOCMD_NOWAIT|DOCMD_VERBOSE);
	    sticky_cmdmod_flags = save_sticky_cmdmod_flags;
	}
    }

    ga_clear(&ga);

    اگر (eap->skip)
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
find_option_end(char_u **arg, صحیح *scope)
{
    char_u	*p = *arg;

    ++p;
    اگر (*p == 'g' && p[1] == ':')
    {
	*scope = OPT_GLOBAL;
	p += 2;
    }
    وگرنه اگر (*p == 'l' && p[1] == ':')
    {
	*scope = OPT_LOCAL;
	p += 2;
    }
    وگرنه
	*scope = 0;

    اگر (!ASCII_ISALPHA(*p))
	بازگشت پوچ;
    *arg = p;

    اگر (p[0] == 't' && p[1] == '_' && p[2] != NUL && p[3] != NUL)
	p += 4;	    // termcap option
    وگرنه
	وقتی (ASCII_ISALPHA(*p))
	    ++p;
    بازگشت p;
}

/*
 * Display script name where an item was last set.
 * Should only be invoked when 'verbose' is non-zero.
 */
    تهی
last_set_پیام(sctx_T script_ctx)
{
    char_u *p;

    اگر (script_ctx.sc_sid == 0)
	بازگشت;

    p = home_replace_save(پوچ, get_scriptname(script_ctx.sc_sid));
    اگر (p == پوچ)
	بازگشت;

    verbose_enter();
    پیام_puts(_("\n\tLast set from "));
    پیام_puts((نویسه *)p);
    اگر (script_ctx.sc_lnum > 0)
    {
	پیام_puts(_(سطر_پیام));
	پیام_outnum((long)script_ctx.sc_lnum);
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
    typval_T	*عبارت,
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
    اگر (regmatch.regprog != پوچ)
    {
	char_u	*tail = str;
	char_u	*end = str + len;
	صحیح	do_all = (flags[0] == 'g');
	صحیح	sublen;
	صحیح	i;
	char_u	*zero_width = پوچ;

	وقتی (vim_regexec_nl(&regmatch, str, (colnr_T)(tail - str)))
	{
	    // Skip empty match except for first match.
	    اگر (regmatch.startp[0] == regmatch.endp[0])
	    {
		اگر (zero_width == regmatch.startp[0])
		{
		    // avoid getting stuck on a match with an empty string
		    i = mb_ptr2len(tail);
		    mch_memmove((char_u *)ga.ga_data + ga.ga_len, tail,
								   (size_t)i);
		    ga.ga_len += i;
		    tail += i;
		    ادامه;
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
	    sublen = vim_regsub(&regmatch, sub, عبارت, tail, 0, REGSUB_MAGIC);
	    اگر (sublen <= 0)
	    {
		ga_clear(&ga);
		بشکن;
	    }
	    اگر (ga_grow(&ga, (صحیح)((end - tail) + sublen -
			    (regmatch.endp[0] - regmatch.startp[0]))) == FAIL)
	    {
		ga_clear(&ga);
		بشکن;
	    }

	    // copy the text up to where the match is
	    i = (صحیح)(regmatch.startp[0] - tail);
	    mch_memmove((char_u *)ga.ga_data + ga.ga_len, tail, (size_t)i);
	    // add the substituted text
	    (تهی)vim_regsub(&regmatch, sub, عبارت,
				  (char_u *)ga.ga_data + ga.ga_len + i, sublen,
				  REGSUB_COPY | REGSUB_MAGIC);
	    ga.ga_len += i + sublen - 1;
	    tail = regmatch.endp[0];
	    اگر (*tail == NUL)
		بشکن;
	    اگر (!do_all)
		بشکن;
	}

	اگر (ga.ga_data != پوچ)
	{
	    STRCPY((نویسه *)ga.ga_data + ga.ga_len, tail);
	    ga.ga_len += (صحیح)(end - tail);
	}

	vim_regfree(regmatch.regprog);
    }

    اگر (ga.ga_data != پوچ)
    {
	str = (char_u *)ga.ga_data;
	len = (size_t)ga.ga_len;
    }
    ret = vim_strnsave(str, len);
    ga_clear(&ga);
    اگر (p_cpo == empty_option)
	p_cpo = save_cpo;
    وگرنه
    {
	// Darn, evaluating {sub} expression or {expr} changed the value.
	// If it's still empty it was changed and restored, need to restore in
	// the complicated way.
	اگر (*p_cpo == NUL)
	    set_option_مقدار_give_err((char_u *)"cpo", 0L, save_cpo, 0);
	free_string_option(save_cpo);
    }

    اگر (ret_len != پوچ)
	*ret_len = len;

    بازگشت ret;
}
