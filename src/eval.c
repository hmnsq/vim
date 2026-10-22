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
#وصف_ڪريو USING_FLOAT_STUFF

#ضمن "vim.h"

#إذا defined(FEAT_EVAL)

#جيڪڏهن_وصف_ٿيل VMS
# include <float.h>
#جيڪڏهن_پورو

#وصف_ڪريو NAMESPACE_CHAR	(char_u *)"abglstvw"

ساكن صحيح eval0_simple_funccal(char_u *arg, typval_T *rettv, exarg_T *eap, evalarg_T *evalarg);
ساكن صحيح eval2(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
ساكن صحيح eval3(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
ساكن صحيح eval4(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
ساكن صحيح eval5(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
ساكن صحيح eval6(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
ساكن صحيح eval7(char_u **arg, typval_T *rettv, evalarg_T *evalarg, صحيح want_string);
ساكن صحيح eval8(char_u **arg, typval_T *rettv, evalarg_T *evalarg, صحيح want_string);
ساكن صحيح eval9(char_u **arg, typval_T *rettv, evalarg_T *evalarg, صحيح want_string);
ساكن صحيح eval9_leader(typval_T *rettv, صحيح numeric_only, char_u *شروعات_leader, char_u **پڇاڙي_leaderp);

ساكن char_u *make_expanded_name(char_u *اندريون_شروعات, char_u *اظهار_شروعات, char_u *اظهار_پڇاڙي, char_u *اندريون_پڇاڙي);

/*
 * Return "n1" divided by "n2", taking care of dividing by zero.
 * If "failed" is not NULL set it to TRUE when dividing by zero fails.
 */
	varnumber_T
انگ_divide(varnumber_T n1, varnumber_T n2, صحيح *failed)
{
    varnumber_T	نتيجو;

    إذا (n2 == 0)
    {
	إذا (اندريون_vim9script())
	{
	    emsg(_(e_divide_by_zero));
	    إذا (failed != عدم)
		*failed = TRUE;
	}
	إذا (n1 == 0)
	    نتيجو = VARNUM_MIN; // similar to NaN
	وإلا إذا (n1 < 0)
	    نتيجو = -VARNUM_MAX;
	وإلا
	    نتيجو = VARNUM_MAX;
    }
    وإلا إذا (n1 == VARNUM_MIN && n2 == -1)
    {
	// specific case: trying to do VARNUM_MIN / -1 results in a positive
	// number that doesn't fit in varnumber_T and causes an FPE
	نتيجو = VARNUM_MAX;
    }
    وإلا
	نتيجو = n1 / n2;

    أرجع نتيجو;
}

/*
 * Return "n1" modulus "n2", taking care of dividing by zero.
 * If "failed" is not NULL set it to TRUE when dividing by zero fails.
 */
	varnumber_T
انگ_modulus(varnumber_T n1, varnumber_T n2, صحيح *failed)
{
    إذا (n2 == 0 && اندريون_vim9script())
    {
	emsg(_(e_divide_by_zero));
	إذا (failed != عدم)
	    *failed = TRUE;
    }
    أرجع (n2 == 0) ? 0 : (n1 % n2);
}

/*
 * Initialize the global and v: variables.
 */
    فراغ
eval_init(فراغ)
{
    evalvars_init();
    func_init();
}

#إذا defined(EXITFREE)
    فراغ
eval_clear(فراغ)
{
    evalvars_clear();
    free_scriptnames();  // must come after evalvars_clear().
    free_locales();

    // autoloaded script names
    free_autoload_scriptnames();

    // unreferenced lists, tuples and dicts
    (فراغ)garbage_collect(FALSE);

    // functions not garbage collected
    free_all_functions();
}
#جيڪڏهن_پورو

    فراغ
fill_evalarg_from_eap(evalarg_T *evalarg, exarg_T *eap, صحيح skip)
{
    init_evalarg(evalarg);
    evalarg->eval_flags = skip ? 0 : EVAL_EVALUATE;

    إذا (eap == عدم)
	أرجع;

    evalarg->eval_cstack = eap->cstack;
    إذا (sourcing_a_script(eap) || eap->ea_getline == get_list_سٽ)
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
    صحيح
eval_منزل_bool(
    char_u	*arg,
    صحيح		*error,
    exarg_T	*eap,
    صحيح		skip,	    // only parse, don't execute
    صحيح		use_simple_function)
{
    typval_T	tv;
    varnumber_T	retval = FALSE;
    evalarg_T	evalarg;
    صحيح		r;

    fill_evalarg_from_eap(&evalarg, eap, skip);

    إذا (skip)
	++emsg_skip;
    إذا (use_simple_function)
	r = eval0_simple_funccal(arg, &tv, eap, &evalarg);
    وإلا
	r = eval0(arg, &tv, eap, &evalarg);
    إذا (r == FAIL)
	*error = TRUE;
    وإلا
    {
	*error = FALSE;
	إذا (!skip)
	{
	    إذا (اندريون_vim9script())
		retval = tv_get_bool_chk(&tv, error);
	    وإلا
		retval = (tv_get_عدد_chk(&tv, error) != 0);
	    clear_tv(&tv);
	}
    }
    إذا (skip)
	--emsg_skip;
    clear_evalarg(&evalarg, eap);

    أرجع (صحيح)retval;
}

/*
 * Call eval1() and give an error message if not done at a lower level.
 */
    ساكن صحيح
eval1_emsg(char_u **arg, typval_T *rettv, exarg_T *eap)
{
    char_u	*شروعات = *arg;
    صحيح		ret;
    صحيح		did_emsg_before = did_emsg;
    صحيح		called_emsg_before = called_emsg;
    evalarg_T	evalarg;

    fill_evalarg_from_eap(&evalarg, eap, eap != عدم && eap->skip);

    ret = eval1(arg, rettv, &evalarg);
    إذا (ret == FAIL)
    {
	// Report the invalid expression unless the expression evaluation has
	// been cancelled due to an aborting error, an interrupt, or an
	// exception, or we already gave a more specific error.
	// Also check called_emsg for when using assert_fails().
	إذا (!aborting() && did_emsg == did_emsg_before
					  && called_emsg == called_emsg_before)
	    semsg(_(e_invalid_اظهار_str), شروعات);
    }
    clear_evalarg(&evalarg, eap);
    أرجع ret;
}

/*
 * Return whether a typval is a valid expression to pass to eval_expr_typval()
 * or eval_expr_to_bool().  An empty string returns FALSE;
 */
    صحيح
eval_اظهار_valid_arg(typval_T *tv)
{
    أرجع tv->v_type != VAR_UNKNOWN
	    && (tv->v_type != VAR_STRING
		  || (tv->vval.v_string != عدم && *tv->vval.v_string != NUL));
}

/*
 * When calling eval_expr_typval() many times we only need one funccall_T.
 * Returns NULL when no funccall_T is to be used.
 * When returning non-NULL remove_funccal() must be called later.
 */
    funccall_T *
eval_اظهار_get_funccal(typval_T *اظهار, typval_T *rettv)
{
    إذا (اظهار->v_type != VAR_PARTIAL)
	أرجع عدم;

    partial_T *partial = اظهار->vval.v_partial;
    إذا (partial == عدم)
	أرجع عدم;
    إذا (partial->pt_func == عدم
	    || partial->pt_func->uf_def_حالت == UF_NOT_COMPILED)
	أرجع عدم;

    أرجع create_funccal(partial->pt_func, rettv);
}

/*
 * Evaluate a partial.
 * Pass arguments "argv[argc]".
 * "fc_arg" is from eval_expr_get_funccal() or NULL;
 * Return the result in "rettv" and OK or FAIL.
 */
    ساكن صحيح
eval_اظهار_partial(
    typval_T	*اظهار,
    typval_T	*argv,
    صحيح		argc,
    funccall_T	*fc_arg,
    typval_T	*rettv)
{
    partial_T	*partial = اظهار->vval.v_partial;

    إذا (partial == عدم)
	أرجع FAIL;

    إذا (partial->pt_func != عدم
			&& partial->pt_func->uf_def_حالت != UF_NOT_COMPILED)
    {
	funccall_T	*fc = fc_arg != عدم ? fc_arg
				: create_funccal(partial->pt_func, rettv);
	صحيح		r;

	إذا (fc == عدم)
	    أرجع FAIL;

	// Shortcut to call a compiled function with minimal overhead.
	إذا (partial->pt_obj != عدم)
	    partial->pt_obj->obj_refcount++;
	r = call_def_function(partial->pt_func, argc, argv, DEF_USE_PT_ARGV,
					partial, partial->pt_obj, fc, rettv);
	إذا (fc_arg == عدم)
	    remove_funccal();
	إذا (r == FAIL)
	    أرجع FAIL;
    }
    وإلا
    {
	char_u		*s = partial_name(partial);
	funcexe_T	funcexe;

	إذا (s == عدم || *s == NUL)
	    أرجع FAIL;

	CLEAR_ميدان(funcexe);
	funcexe.fe_evaluate = TRUE;
	funcexe.fe_partial = partial;
	إذا (call_func(s, -1, rettv, argc, argv, &funcexe) == FAIL)
	    أرجع FAIL;
    }

    أرجع OK;
}

/*
 * Evaluate an expression which is a function.
 * Pass arguments "argv[argc]".
 * Return the result in "rettv" and OK or FAIL.
 */
    ساكن صحيح
eval_اظهار_func(
    typval_T	*اظهار,
    typval_T	*argv,
    صحيح		argc,
    typval_T	*rettv)
{
    funcexe_T	funcexe;
    char_u	buf[NUMBUFLEN];
    char_u	*s;

    إذا (اظهار->v_type == VAR_FUNC)
	s = اظهار->vval.v_string;
    وإلا
	s = tv_get_string_buf_chk_strict(اظهار, buf, اندريون_vim9script());
    إذا (s == عدم || *s == NUL)
	أرجع FAIL;

    CLEAR_ميدان(funcexe);
    funcexe.fe_evaluate = TRUE;
    إذا (call_func(s, -1, rettv, argc, argv, &funcexe) == FAIL)
	أرجع FAIL;

    أرجع OK;
}

/*
 * Evaluate an expression, which is a string.
 * Return the result in "rettv" and OK or FAIL.
 */
    ساكن صحيح
eval_اظهار_string(
    typval_T	*اظهار,
    typval_T	*rettv)
{
    char_u	*s;
    char_u	buf[NUMBUFLEN];

    s = tv_get_string_buf_chk_strict(اظهار, buf, اندريون_vim9script());
    إذا (s == عدم)
	أرجع FAIL;

    s = skipwhite(s);
    إذا (eval1_emsg(&s, rettv, عدم) == FAIL)
	أرجع FAIL;

    إذا (*skipwhite(s) != NUL)  // check for trailing chars after expr
    {
	clear_tv(rettv);
	semsg(_(e_invalid_اظهار_str), s);
	أرجع FAIL;
    }

    أرجع OK;
}

/*
 * Evaluate an expression, which can be a function, partial or string.
 * Pass arguments "argv[argc]".
 * If "want_func" is TRUE treat a string as a function name, not an expression.
 * "fc_arg" is from eval_expr_get_funccal() or NULL;
 * Return the result in "rettv" and OK or FAIL.
 */
    صحيح
eval_اظهار_typval(
	typval_T    *اظهار,
	صحيح	    want_func,
	typval_T    *argv,
	صحيح	    argc,
	funccall_T  *fc_arg,
	typval_T    *rettv)
{
    إذا (اظهار->v_type == VAR_PARTIAL)
	أرجع eval_اظهار_partial(اظهار, argv, argc, fc_arg, rettv);
    إذا (اظهار->v_type == VAR_INSTR)
	أرجع exe_typval_instr(اظهار, rettv);
    إذا (اظهار->v_type == VAR_FUNC || want_func)
	أرجع eval_اظهار_func(اظهار, argv, argc, rettv);

    أرجع eval_اظهار_string(اظهار, rettv);
}

/*
 * Like eval_to_bool() but using a typval_T instead of a string.
 * Works for string, funcref and partial.
 */
    صحيح
eval_اظهار_منزل_bool(typval_T *اظهار, صحيح *error)
{
    typval_T	rettv;
    صحيح		res;

    إذا (eval_اظهار_typval(اظهار, FALSE, عدم, 0, عدم, &rettv) == FAIL)
    {
	*error = TRUE;
	أرجع FALSE;
    }
    res = (tv_get_bool_chk(&rettv, error) != 0);
    clear_tv(&rettv);
    أرجع res;
}

/*
 * Top level evaluation function, returning a string.  If "skip" is TRUE,
 * only parsing to "nextcmd" is done, without reporting errors.  Return
 * pointer to allocated memory, or NULL for failure or when "skip" is TRUE.
 */
    char_u *
eval_منزل_string_skip(
    char_u	*arg,
    exarg_T	*eap,
    صحيح		skip)	    // only parse, don't execute
{
    typval_T	tv;
    char_u	*retval;
    evalarg_T	evalarg;

    fill_evalarg_from_eap(&evalarg, eap, skip);
    إذا (skip)
	++emsg_skip;
    إذا (eval0(arg, &tv, eap, &evalarg) == FAIL || skip)
	retval = عدم;
    وإلا
    {
	retval = vim_strsave(tv_get_string(&tv));
	clear_tv(&tv);
    }
    إذا (skip)
	--emsg_skip;
    clear_evalarg(&evalarg, eap);

    أرجع retval;
}

/*
 * Initialize "evalarg" for use.
 */
    فراغ
init_evalarg(evalarg_T *evalarg)
{
    CLEAR_اشارو(evalarg);
    ga_init2(&evalarg->eval_tofree_ga, حجم(char_u *), 20);
}

/*
 * If "evalarg->eval_tofree" is not NULL free it later.
 * Caller is expected to overwrite "evalarg->eval_tofree" next.
 */
    ساكن فراغ
free_eval_tofree_later(evalarg_T *evalarg)
{
    إذا (evalarg->eval_tofree == عدم)
	أرجع;

    إذا (ga_grow(&evalarg->eval_tofree_ga, 1) == OK)
	((char_u **)evalarg->eval_tofree_ga.ga_معلومات)
	    [evalarg->eval_tofree_ga.ga_len++]
	    = evalarg->eval_tofree;
    وإلا
	vim_free(evalarg->eval_tofree);
}

/*
 * After using "evalarg" filled from "eap": free the memory.
 */
    فراغ
clear_evalarg(evalarg_T *evalarg, exarg_T *eap)
{
    إذا (evalarg == عدم)
	أرجع;

    garray_T *etga = &evalarg->eval_tofree_ga;

    إذا (evalarg->eval_tofree != عدم || evalarg->eval_using_cmdline)
    {
	إذا (eap != عدم)
	{
	    // We may need to keep the original command line, e.g. for
	    // ":let" it has the variable names.  But we may also need
	    // the new one, "nextcmd" points into it.  Keep both.
	    vim_free(eap->cmdline_tofree);
	    eap->cmdline_tofree = *eap->cmdlinep;

	    إذا (evalarg->eval_using_cmdline && etga->ga_len > 0)
	    {
		// "nextcmd" points into the last line in eval_tofree_ga,
		// need to keep it around.
		--etga->ga_len;
		*eap->cmdlinep = ((char_u **)etga->ga_معلومات)[etga->ga_len];
		vim_free(evalarg->eval_tofree);
	    }
	    وإلا
		*eap->cmdlinep = evalarg->eval_tofree;
	}
	وإلا
	    vim_free(evalarg->eval_tofree);
	evalarg->eval_tofree = عدم;
    }

    ga_clear_لڙيون(etga);
    VIM_CLEAR(evalarg->eval_tofree_lambda);
}

/*
 * Skip over an expression at "*pp".
 * Return FAIL for an error, OK otherwise.
 */
    صحيح
skip_اظهار(char_u **pp, evalarg_T *evalarg)
{
    typval_T	rettv;

    *pp = skipwhite(*pp);
    أرجع eval1(pp, &rettv, evalarg);
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
    صحيح
skip_اظهار_concatenate(
	char_u	    **arg,
	char_u	    **شروعات,
	char_u	    **پڇاڙي,
	evalarg_T   *evalarg)
{
    typval_T	rettv;
    صحيح		res;
    صحيح		vim9script = اندريون_vim9script();
    garray_T    *gap = evalarg == عدم ? عدم : &evalarg->eval_ga;
    garray_T    *freegap = evalarg == عدم ? عدم : &evalarg->eval_freega;
    صحيح		save_flags = evalarg == عدم ? 0 : evalarg->eval_flags;
    صحيح		evaluate = evalarg == عدم
			       ? FALSE : (evalarg->eval_flags & EVAL_EVALUATE);

    إذا (vim9script && evaluate
	       && (evalarg->eval_cookie != عدم || evalarg->eval_cctx != عدم))
    {
	ga_init2(gap, حجم(char_u *), 10);
	// leave room for "start"
	إذا (ga_grow(gap, 1) == OK)
	    ++gap->ga_len;
	ga_init2(freegap, حجم(char_u *), 10);
    }
    *شروعات = *arg;

    // Don't evaluate the expression.
    إذا (evalarg != عدم)
	evalarg->eval_flags &= ~EVAL_EVALUATE;
    *arg = skipwhite(*arg);
    res = eval1(arg, &rettv, evalarg);
    *پڇاڙي = *arg;
    إذا (evalarg != عدم)
	evalarg->eval_flags = save_flags;

    إذا (vim9script && evaluate
	    && (evalarg->eval_cookie != عدم || evalarg->eval_cctx != عدم))
    {
	إذا (evalarg->eval_ga.ga_len == 1)
	{
	    // just the one line, no need to concatenate
	    ga_clear(gap);
	    gap->ga_itemsize = 0;
	}
	وإلا
	{
	    char_u	    *p;
	    size_t	    endoff = STRLEN(*arg);

	    // Line breaks encountered, concatenate all the lines.
	    *((char_u **)gap->ga_معلومات) = *شروعات;
	    p = ga_concat_لڙيون(gap, " ");

	    // free the lines only when using getsourceline()
	    إذا (evalarg->eval_cookie != عدم)
	    {
		// Do not free the first line, the caller can still use it.
		*((char_u **)gap->ga_معلومات) = عدم;
		// Do not free the last line, "arg" points into it, free it
		// later.  Also free "eval_tofree" later if needed.
		free_eval_tofree_later(evalarg);
		evalarg->eval_tofree =
				    ((char_u **)gap->ga_معلومات)[gap->ga_len - 1];
		((char_u **)gap->ga_معلومات)[gap->ga_len - 1] = عدم;
		ga_clear_لڙيون(gap);
		ga_clear(freegap);
	    }
	    وإلا
	    {
		ga_clear(gap);

		// free lines that were explicitly marked for freeing
		ga_clear_لڙيون(freegap);
	    }

	    gap->ga_itemsize = 0;
	    إذا (p == عدم)
		أرجع FAIL;
	    *شروعات = p;
	    vim_free(evalarg->eval_tofree_lambda);
	    evalarg->eval_tofree_lambda = p;
	    // Compute "end" relative to the end.
	    *پڇاڙي = *شروعات + STRLEN(*شروعات) - endoff;
	}
    }

    أرجع res;
}

/*
 * Convert "tv" to a string.
 * When "join_list" is TRUE convert a List or a Tuple into a sequence of lines.
 * Returns an allocated string (NULL when out of memory).
 */
    char_u *
typval2string(typval_T *tv, صحيح join_list)
{
    garray_T	ga;
    char_u	*retval = عدم;

    إذا (join_list && (tv->v_type == VAR_LIST || tv->v_type == VAR_TUPLE))
    {
	إذا (tv->v_type == VAR_LIST)
	{
	    ga_init2(&ga, حجم(حرف), 80);
	    إذا (tv->vval.v_list != عدم)
	    {
		list_join(&ga, tv->vval.v_list, (char_u *)"\n", TRUE, FALSE,
									0);
		إذا (tv->vval.v_list->lv_len > 0)
		    ga_append(&ga, NL);
	    }
	    ga_append(&ga, NUL);
	    retval = (char_u *)ga.ga_معلومات;
	}
	وإلا
	{
	    // tuple
	    ga_init2(&ga, حجم(حرف), 80);
	    إذا (tv->vval.v_tuple != عدم)
	    {
		tuple_join(&ga, tv->vval.v_tuple, (char_u *)"\n", TRUE, FALSE,
									0);
		إذا (TUPLE_LEN(tv->vval.v_tuple) > 0)
		    ga_append(&ga, NL);
	    }
	    ga_append(&ga, NUL);
	    retval = (char_u *)ga.ga_معلومات;
	}
    }
    وإلا إذا (tv->v_type == VAR_LIST
	    || tv->v_type == VAR_TUPLE
	    || tv->v_type == VAR_DICT)
    {
	char_u	*tofree;
	char_u	numbuf[NUMBUFLEN];

	retval = tv2string(tv, &tofree, numbuf, 0);
	// Make a copy if we have a value but it's not in allocated memory.
	إذا (retval != عدم && tofree == عدم)
	    retval = vim_strsave(retval);
    }
    وإلا
	retval = vim_strsave(tv_get_string(tv));
    أرجع retval;
}

/*
 * Top level evaluation function, returning a string.  Does not handle line
 * breaks.
 * When "join_list" is TRUE convert a List and a Tuple into a sequence of
 * lines.
 * Return pointer to allocated memory, or NULL for failure.
 */
    char_u *
eval_منزل_string_eap(
    char_u	*arg,
    صحيح		join_list,
    exarg_T	*eap,
    صحيح		use_simple_function)
{
    typval_T	tv;
    char_u	*retval;
    evalarg_T	evalarg;
    صحيح		r;

    fill_evalarg_from_eap(&evalarg, eap, eap != عدم && eap->skip);
    إذا (use_simple_function)
	r = eval0_simple_funccal(arg, &tv, عدم, &evalarg);
    وإلا
	r = eval0(arg, &tv, عدم, &evalarg);
    إذا (r == FAIL)
	retval = عدم;
    وإلا
    {
	retval = typval2string(&tv, join_list);
	clear_tv(&tv);
    }
    clear_evalarg(&evalarg, عدم);

    أرجع retval;
}

    char_u *
eval_منزل_string(
    char_u	*arg,
    صحيح		join_list,
    صحيح		use_simple_function)
{
    أرجع eval_منزل_string_eap(arg, join_list, عدم, use_simple_function);
}

/*
 * Call eval_to_string() without using current local variables and using
 * textlock.  When "use_sandbox" is TRUE use the sandbox.
 * Use legacy Vim script syntax.
 */
    char_u *
eval_منزل_string_محفوظ(
    char_u	*arg,
    صحيح		use_sandbox,
    صحيح		keep_script_نسخو,
    صحيح		use_simple_function)
{
    char_u	*retval;
    funccal_داخلا_T funccal_داخلا;
    صحيح		save_sc_نسخو = current_sctx.sc_نسخو;
    صحيح		save_garbage = may_garbage_collect;

    إذا (!keep_script_نسخو)
	current_sctx.sc_نسخو = 1;
    save_funccal(&funccal_داخلا);
    إذا (use_sandbox)
	++sandbox;
    ++textlock;
    may_garbage_collect = FALSE;
    retval = eval_منزل_string(arg, FALSE, use_simple_function);
    إذا (use_sandbox)
	--sandbox;
    --textlock;
    may_garbage_collect = save_garbage;
    restore_funccal();
    current_sctx.sc_نسخو = save_sc_نسخو;
    أرجع retval;
}

/*
 * Top level evaluation function, returning a number.
 * Evaluates "expr" silently.
 * Returns -1 for an error.
 */
    varnumber_T
eval_منزل_عدد(char_u *اظهار, صحيح use_simple_function)
{
    typval_T	rettv;
    varnumber_T	retval;
    char_u	*p = skipwhite(اظهار);
    صحيح		r = NOTDONE;

    ++emsg_off;

    إذا (use_simple_function)
	r = may_call_simple_func(اظهار, &rettv);
    إذا (r == NOTDONE)
	r = eval1(&p, &rettv, &EVALARG_EVALUATE);
    إذا (r == FAIL)
	retval = -1;
    وإلا
    {
	retval = tv_get_عدد_chk(&rettv, عدم);
	clear_tv(&rettv);
    }
    --emsg_off;

    أرجع retval;
}

/*
 * Top level evaluation function.
 * Returns an allocated typval_T with the result.
 * Returns NULL when there is an error.
 */
    typval_T *
eval_اظهار(char_u *arg, exarg_T *eap)
{
    أرجع eval_اظهار_ext(arg, eap, FALSE);
}

    typval_T *
eval_اظهار_ext(char_u *arg, exarg_T *eap, صحيح use_simple_function)
{
    typval_T	*tv;
    evalarg_T	evalarg;

    fill_evalarg_from_eap(&evalarg, eap, eap != عدم && eap->skip);

    tv = ALLOC_ONE(typval_T);
    إذا (tv != عدم)
    {
	صحيح	r = NOTDONE;

	إذا (use_simple_function)
	    r = eval0_simple_funccal(arg, tv, eap, &evalarg);
	إذا (r == NOTDONE)
	    r = eval0(arg, tv, eap, &evalarg);

	إذا (r == FAIL)
	    VIM_CLEAR(tv);
    }

    clear_evalarg(&evalarg, eap);
    أرجع tv;
}

/*
 * "*arg" points to what can be a function name in the form of "import.Name" or
 * "Funcref".  Return the name of the function.  Set "tofree" to something that
 * was allocated.
 * If "verbose" is FALSE no errors are given.
 * Return NULL for any failure.
 */
    ساكن char_u *
deref_function_name(
	    char_u	**arg,
	    char_u	**tofree,
	    evalarg_T	*evalarg,
	    صحيح		verbose)
{
    typval_T	ref;
    char_u	*name = *arg;
    صحيح		save_flags = 0;
    صحيح		evaluate = evalarg != عدم
				      && (evalarg->eval_flags & EVAL_EVALUATE);

    ref.v_type = VAR_UNKNOWN;
    إذا (evalarg != عدم)
    {
	// need to evaluate this to get an import, like in "a.Func"
	save_flags = evalarg->eval_flags;
	evalarg->eval_flags |= EVAL_EVALUATE;
    }
    إذا (eval9(arg, &ref, evalarg, FALSE) == FAIL)
    {
	dictitem_T	*v;

	// If <SID>VarName was used it would not be found, try another way.
	v = ڳوليو_var_also_اندريون_script(name, عدم, FALSE);
	إذا (v == عدم)
	{
	    name = عدم;
	    اذهب theend;
	}
	copy_tv(&v->di_tv, &ref);
    }
    إذا (*skipwhite(*arg) != NUL)
    {
	إذا (verbose)
	    semsg(_(e_trailing_characters_str), *arg);
	name = عدم;
    }
    وإلا إذا (ref.v_type == VAR_FUNC && ref.vval.v_string != عدم)
    {
	name = ref.vval.v_string;
	ref.vval.v_string = عدم;
	*tofree = name;
    }
    وإلا إذا (ref.v_type == VAR_PARTIAL && ref.vval.v_partial != عدم)
    {
	إذا (ref.vval.v_partial->pt_argc > 0
		|| ref.vval.v_partial->pt_dict != عدم)
	{
	    إذا (verbose)
		emsg(_(e_cannot_use_partial_هتي));
	    name = عدم;
	}
	وإلا
	{
	    name = vim_strsave(partial_name(ref.vval.v_partial));
	    *tofree = name;
	}
    }
    وإلا إذا (evaluate)
    {
	إذا (verbose)
	    semsg(_(e_not_callable_type_str), name);
	name = عدم;
    }

theend:
    clear_tv(&ref);
    إذا (evalarg != عدم)
	evalarg->eval_flags = save_flags;
    أرجع name;
}

/*
 * Call some Vim script function and return the result in "*rettv".
 * Uses argv[0] to argv[argc - 1] for the function arguments.  argv[argc]
 * should have type VAR_UNKNOWN.
 * Returns OK or FAIL.
 */
    صحيح
call_vim_function(
    char_u      *func,
    صحيح		argc,
    typval_T	*argv,
    typval_T	*rettv)
{
    صحيح		ret;
    funcexe_T	funcexe;
    char_u	*arg;
    char_u	*name;
    char_u	*tofree = عدم;
    صحيح		ignore_errors;

    rettv->v_type = VAR_UNKNOWN;		// clear_tv() uses this
    CLEAR_ميدان(funcexe);
    funcexe.fe_firstline = curwin->w_cursor.lnum;
    funcexe.fe_lastline = curwin->w_cursor.lnum;
    funcexe.fe_evaluate = TRUE;

    // The name might be "import.Func" or "Funcref".  We don't know, we need to
    // ignore errors for an undefined name.  But we do want errors when an
    // autoload script has errors.  Guess that when there is a dot in the name
    // showing errors is the right choice.
    ignore_errors = vim_strchr(func, '.') == عدم;
    arg = func;
    إذا (ignore_errors)
	++emsg_off;
    name = deref_function_name(&arg, &tofree, &EVALARG_EVALUATE, FALSE);
    إذا (ignore_errors)
	--emsg_off;
    إذا (name == عدم)
	name = func;

    ret = call_func(name, -1, rettv, argc, argv, &funcexe);

    إذا (ret == FAIL)
	clear_tv(rettv);
    vim_free(tofree);

    أرجع ret;
}

/*
 * Call Vim script function "func" and return the result as a string.
 * Uses "argv[0]" to "argv[argc - 1]" for the function arguments. "argv[argc]"
 * should have type VAR_UNKNOWN.
 * Returns NULL when calling the function fails.
 */
    فراغ *
call_func_retstr(
    char_u      *func,
    صحيح		argc,
    typval_T	*argv)
{
    typval_T	rettv;
    char_u	*retval;

    إذا (call_vim_function(func, argc, argv, &rettv) == FAIL)
	أرجع عدم;

    retval = vim_strsave(tv_get_string(&rettv));
    clear_tv(&rettv);
    أرجع retval;
}

/*
 * Call Vim script function "func" and return the result as a List.
 * Uses "argv" and "argc" as call_func_retstr().
 * Returns NULL when there is something wrong.
 * Gives an error when the returned value is not a list.
 */
    فراغ *
call_func_retlist(
    char_u      *func,
    صحيح		argc,
    typval_T	*argv)
{
    typval_T	rettv;

    إذا (call_vim_function(func, argc, argv, &rettv) == FAIL)
	أرجع عدم;

    إذا (rettv.v_type != VAR_LIST)
    {
	semsg(_(e_custom_list_completion_function_does_not_return_list_but_str),
		vartype_name(rettv.v_type));
	clear_tv(&rettv);
	أرجع عدم;
    }

    أرجع rettv.vval.v_list;
}

#إذا defined(FEAT_FOLDING)
/*
 * Evaluate "arg", which is 'foldexpr'.
 * Note: caller must set "curwin" to match "arg".
 * Returns the foldlevel, and any character preceding it in "*cp".  Doesn't
 * give error messages.
 */
    صحيح
eval_foldexpr(win_T *wp, صحيح *cp)
{
    char_u	*arg;
    typval_T	tv;
    varnumber_T	retval;
    char_u	*s;
    sctx_T	saved_sctx = current_sctx;
    صحيح		use_sandbox = was_سيٽ_insecurely(wp, (char_u *)"foldexpr",
								    OPT_LOCAL);

    arg = skipwhite(wp->w_p_fde);
    current_sctx = wp->w_p_script_ctx[WV_FDE];

    ++emsg_off;
    إذا (use_sandbox)
	++sandbox;
    ++textlock;
    *cp = NUL;

    // Evaluate the expression.  If the expression is "FuncName()" call the
    // function directly.
    إذا (eval0_simple_funccal(arg, &tv, عدم, &EVALARG_EVALUATE) == FAIL)
	retval = 0;
    وإلا
    {
	// If the result is a number, just return the number.
	إذا (tv.v_type == VAR_عدد)
	    retval = tv.vval.v_عدد;
	وإلا إذا (tv.v_type != VAR_STRING || tv.vval.v_string == عدم)
	    retval = 0;
	وإلا
	{
	    // If the result is a string, check if there is a non-digit before
	    // the number.
	    s = tv.vval.v_string;
	    إذا (*s != NUL && !VIM_ISDIGIT(*s) && *s != '-')
		*cp = *s++;
	    retval = atol((حرف *)s);
	}
	clear_tv(&tv);
    }
    --emsg_off;
    إذا (use_sandbox)
	--sandbox;
    --textlock;
    clear_evalarg(&EVALARG_EVALUATE, عدم);
    current_sctx = saved_sctx;

    أرجع (صحيح)retval;
}
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل LOG_LOCKVAR
تعريفنوع بنية flag_string_S
{
    صحيح	    flag;
    حرف    *str;
} flag_string_T;

    ساكن حرف *
flags_tostring(صحيح flags, flag_string_T *_fstring, حرف *buf, size_t n)
{
    حرف *p = buf;
    *p = NUL;
    لكل (flag_string_T *fstring = _fstring; fstring->flag; ++fstring)
    {
	إذا ((fstring->flag & flags) != 0)
	{
	    size_t len = STRLEN(fstring->str);
	    إذا (n > p - buf + len + 7)
	    {
		STRCAT(p, fstring->str);
		p += len;
		STRCAT(p, " ");
		++p;
	    }
	    وإلا
	    {
		STRCAT(buf, "...");
		اكسر;
	    }
	}
    }
    أرجع buf;
}

flag_string_T glv_flag_لڙيون[] = {
    { GLV_QUIET,		"QUIET" },
    { GLV_NO_AUTOLOAD,		"NO_AUTOLOAD" },
    { GLV_READ_ONLY,		"READ_ONLY" },
    { GLV_NO_DECL,		"NO_DECL" },
    { GLV_COMPILING,		"COMPILING" },
    { GLV_سونپيو_WITH_OP,	"ASSIGN_WITH_OP" },
    { GLV_PREFER_FUNC,		"PREFER_FUNC" },
    { 0,			عدم }
};
#جيڪڏهن_پورو

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
    ساكن فراغ
fill_lval_from_lval_پاڙ(lval_T *lp, lval_پاڙ_T *lr)
{
#جيڪڏهن_وصف_ٿيل LOG_LOCKVAR
    ch_log(عدم, "LKVAR: fill_lval_from_lval_root(): name %s, tv %p",
						lp->ll_name, (فراغ*)lr->lr_tv);
#جيڪڏهن_پورو
    إذا (lr->lr_tv == عدم)
	أرجع;
    إذا (!lr->lr_is_arg && lr->lr_tv->v_type == VAR_CLASS)
    {
	إذا (lr->lr_tv->vval.v_class != عدم)
	{
	    // Special special case. Look for a bare class variable reference.
	    class_T	*cl = lr->lr_tv->vval.v_class;
	    صحيح		m_idx;
	    ocmember_T	*m = class_member_lookup(cl, lp->ll_name,
					lp->ll_name_پڇاڙي - lp->ll_name, &m_idx);
	    إذا (m != عدم)
	    {
		// Assuming "inside class" since bare reference.
		lp->ll_class = lr->lr_tv->vval.v_class;
		lp->ll_oi = m_idx;
		lp->ll_valtype = m->ocm_type;
		lp->ll_tv = &lp->ll_class->class_members_tv[m_idx];
#جيڪڏهن_وصف_ٿيل LOG_LOCKVAR
		ch_log(عدم, "LKVAR:    ... class member %s.%s",
					lp->ll_class->class_name.string, lp->ll_name);
#جيڪڏهن_پورو
		أرجع;
	    }
	}
    }

#جيڪڏهن_وصف_ٿيل LOG_LOCKVAR
    ch_log(عدم, "LKVAR:    ... type: %s", vartype_name(lr->lr_tv->v_type));
#جيڪڏهن_پورو
    lp->ll_tv = lr->lr_tv;
    lp->ll_is_پاڙ = TRUE;
}

/*
 * Check if the class has permission to access the member.
 * Returns OK or FAIL.
 */
    ساكن صحيح
get_lval_check_access(
    class_T	*cl_exec,   // executing class, NULL if :def or script level
    class_T	*cl,	    // class which contains the member
    ocmember_T	*om,	    // member being accessed
    char_u	*p,	    // char after member name
    صحيح		flags)	    // GLV flags to check if writing to lval
{
#جيڪڏهن_وصف_ٿيل LOG_LOCKVAR
    ch_log(عدم, "LKVAR: get_lval_check_access(), cl_exec %p, cl %p, %c",
						(فراغ*)cl_exec, (فراغ*)cl, *p);
#جيڪڏهن_پورو
    إذا (cl_exec != عدم && cl_exec == cl)
	أرجع OK;

    حرف *نياپو = عدم;
    بدل (om->ocm_access)
    {
	حالة VIM_ACCESS_ذاتي:
	    نياپو = e_cannot_access_protected_variable_str;
	    اكسر;
	حالة VIM_ACCESS_READ:
	    // If [idx] or .key following, read only OK.
	    إذا (*p == '[' || *p == '.')
		اكسر;
	    إذا ((flags & GLV_READ_ONLY) == 0)
	    {
		إذا (IS_ENUM(cl))
		{
		    إذا (om->ocm_type->tt_type == VAR_OBJECT)
			semsg(_(e_enumvalue_str_cannot_be_modified),
				cl->class_name.string, om->ocm_name.string);
		    وإلا
			نياپو = e_variable_is_not_writable_str;
		}
		وإلا
		    نياپو = e_variable_is_not_writable_str;
	    }
	    اكسر;
	حالة VIM_ACCESS_ALL:
	    اكسر;
    }
    إذا (نياپو != عدم)
    {
	emsg_var_cl_define(نياپو, om->ocm_name.string, 0, cl);
	أرجع FAIL;
    }
    أرجع OK;
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
    ساكن char_u *
get_lval_imported(
    lval_T	*lp,
    scid_T	imp_sid,
    char_u	*p,
    dictitem_T	**dip,
    صحيح		fne_flags)
{
    ufunc_T	*ufunc;
    type_T	*type = عدم;
    صحيح		cc;
    صحيح		rc = FAIL;

    p = skipwhite(p);

    import_check_sourced_sid(&imp_sid);
    lp->ll_sid = imp_sid;
    lp->ll_name = p;
    p = ڳوليو_name_پڇاڙي(lp->ll_name, عدم, عدم, fne_flags);
    lp->ll_name_پڇاڙي = p;

    // check the item is exported
    cc = *p;
    *p = NUL;
    إذا (ڳوليو_exported(imp_sid, lp->ll_name, &ufunc, &type, عدم, عدم,
								TRUE) == -1)
	اذهب failed;

    // Get the typval for the exported item
    hashtab_T *ht = &SCRIPT_VARS(imp_sid);
    إذا (ht == عدم)
	اذهب failed;

    dictitem_T *di = ڳوليو_var_اندريون_ht(ht, 0, lp->ll_name, TRUE);
    إذا (di == عدم)
	// script is autoloaded.  So variable will be found later
	اذهب success;

    *dip = di;

    // Check whether the variable is writable.
    svar_T *sv = ڳوليو_typval_اندريون_script(&di->di_tv, imp_sid, FALSE);
    إذا (sv != عدم && sv->sv_const != 0)
    {
	semsg(_(e_cannot_change_readonly_variable_str), lp->ll_name);
	اذهب failed;
    }

    // check whether variable is locked
    إذا (قدر_check_تالو(di->di_tv.v_تالو, lp->ll_name, FALSE))
	اذهب failed;

    lp->ll_tv = &di->di_tv;
    lp->ll_valtype = type;

success:
    rc = OK;

failed:
    *p = cc;
    أرجع rc == OK ? p : عدم;
}

تعريفنوع تعداد {
    GLV_FAIL,
    GLV_OK,
    GLV_STOP
} glv_حالت_T;

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
    ساكن صحيح
get_lval_dict_شي(
    lval_T	*lp,
    char_u	*name,
    char_u	*ڪنجي,
    صحيح		len,
    char_u	**ڪنجي_پڇاڙي,
    typval_T	*var1,
    صحيح		flags,
    صحيح		unlet,
    typval_T	*rettv)
{
    صحيح		quiet = flags & GLV_QUIET;
    char_u	*p = *ڪنجي_پڇاڙي;

    إذا (len == -1)
    {
	// "[key]": get key from "var1"
	ڪنجي = tv_get_string_chk(var1);	// is number or string
	إذا (ڪنجي == عدم)
	    أرجع GLV_FAIL;
    }
    lp->ll_list = عدم;
    lp->ll_list = عدم;
    lp->ll_blob = عدم;
    lp->ll_object = عدم;
    lp->ll_class = عدم;
    lp->ll_tuple = عدم;

    // a NULL dict is equivalent with an empty dict
    إذا (lp->ll_tv->vval.v_dict == عدم)
    {
	lp->ll_tv->vval.v_dict = dict_alloc();
	إذا (lp->ll_tv->vval.v_dict == عدم)
	    أرجع GLV_FAIL;
	++lp->ll_tv->vval.v_dict->dv_refcount;
    }
    lp->ll_dict = lp->ll_tv->vval.v_dict;

    lp->ll_di = dict_ڳوليو(lp->ll_dict, ڪنجي, len);

    // When assigning to a scope dictionary check that a function and
    // variable name is valid (only variable name unless it is l: or
    // g: dictionary). Disallow overwriting a builtin function.
    إذا (rettv != عدم && lp->ll_dict->dv_scope != 0)
    {
	صحيح prevval;

	إذا (len != -1)
	{
	    prevval = ڪنجي[len];
	    ڪنجي[len] = NUL;
	}
	وإلا
	    prevval = 0; // avoid compiler warning
	صحيح wrong = (lp->ll_dict->dv_scope == VAR_DEF_SCOPE
		&& (rettv->v_type == VAR_FUNC
		    || rettv->v_type == VAR_PARTIAL)
		&& var_wrong_func_name(ڪنجي, lp->ll_di == عدم))
	    || !valid_varname(ڪنجي, -1, TRUE);
	إذا (len != -1)
	    ڪنجي[len] = prevval;
	إذا (wrong)
	    أرجع GLV_FAIL;
    }

    إذا (lp->ll_valtype != عدم)
	// use the type of the member
	lp->ll_valtype = lp->ll_valtype->tt_member;

    إذا (lp->ll_di == عدم)
    {
	// Can't add "v:" or "a:" variable.
	إذا (lp->ll_dict == get_vimvar_dict()
		|| &lp->ll_dict->dv_hashtab == get_funccal_دليلون_ht())
	{
	    semsg(_(e_illegal_variable_name_str), name);
	    أرجع GLV_FAIL;
	}

	// Key does not exist in dict: may need to add it.
	إذا (*p == '[' || *p == '.' || unlet)
	{
	    إذا (!quiet)
		semsg(_(e_ڪنجي_not_present_اندريون_dictionary_str), ڪنجي);
	    أرجع GLV_FAIL;
	}
	إذا (len == -1)
	    lp->ll_newkey = vim_strsave(ڪنجي);
	وإلا
	    lp->ll_newkey = vim_strnsave(ڪنجي, len);
	إذا (lp->ll_newkey == عدم)
	    p = عدم;

	*ڪنجي_پڇاڙي = p;
	أرجع GLV_STOP;
    }
    // existing variable, need to check if it can be changed
    وإلا إذا ((flags & GLV_READ_ONLY) == 0
	    && (var_check_ro(lp->ll_di->di_flags, name, FALSE)
		|| var_check_تالو(lp->ll_di->di_flags, name, FALSE)))
	أرجع GLV_FAIL;

    lp->ll_tv = &lp->ll_di->di_tv;

    أرجع GLV_OK;
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
    ساكن صحيح
get_lval_blob(
    lval_T	*lp,
    typval_T	*var1,
    typval_T	*var2,
    صحيح		empty1,
    صحيح		quiet)
{
    ڊگهو	bloblen = blob_len(lp->ll_tv->vval.v_blob);

    lp->ll_list = عدم;
    lp->ll_dict = عدم;
    lp->ll_object = عدم;
    lp->ll_class = عدم;
    lp->ll_tuple = عدم;

    // Get the number and item for the only or first index of a List or Tuple.
    إذا (empty1)
	lp->ll_n1 = 0;
    وإلا
	// is number or string
	lp->ll_n1 = (ڊگهو)tv_get_عدد(var1);

    إذا (check_blob_اشاريو(bloblen, lp->ll_n1, quiet) == FAIL)
	أرجع FAIL;
    إذا (lp->ll_range && !lp->ll_empty2)
    {
	lp->ll_n2 = (ڊگهو)tv_get_عدد(var2);
	إذا (check_blob_range(bloblen, lp->ll_n1, lp->ll_n2, quiet) == FAIL)
	    أرجع FAIL;
    }

    إذا (!lp->ll_range)
	// Indexing a single byte in a blob.  So the rhs type is a
	// number.
	lp->ll_valtype = &t_عدد;

    lp->ll_blob = lp->ll_tv->vval.v_blob;
    lp->ll_tv = عدم;

    أرجع OK;
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
    ساكن صحيح
get_lval_list(
    lval_T	*lp,
    typval_T	*var1,
    typval_T	*var2,
    صحيح		empty1,
    صحيح		flags,
    صحيح		quiet)
{
    /*
     * Get the number and item for the only or first index of the List.
     */
    إذا (empty1)
	lp->ll_n1 = 0;
    وإلا
	// is number or string
	lp->ll_n1 = (ڊگهو)tv_get_عدد(var1);

    lp->ll_dict = عدم;
    lp->ll_object = عدم;
    lp->ll_class = عدم;
    lp->ll_tuple = عدم;
    lp->ll_list = lp->ll_tv->vval.v_list;
    lp->ll_li = check_range_اشاريو_one(lp->ll_list, &lp->ll_n1,
				(flags & GLV_سونپيو_WITH_OP) == 0, quiet);
    إذا (lp->ll_li == عدم)
	أرجع FAIL;

    إذا (lp->ll_valtype != عدم && !lp->ll_range)
    {
	// use the type of the member
	إذا (lp->ll_valtype->tt_member != عدم)
	    lp->ll_valtype = lp->ll_valtype->tt_member;
	وإلا
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
    إذا (lp->ll_range && !lp->ll_empty2)
    {
	lp->ll_n2 = (ڊگهو)tv_get_عدد(var2);
	// is number or string
	إذا (check_range_اشاريو_two(lp->ll_list,
			&lp->ll_n1, lp->ll_li, &lp->ll_n2, quiet) == FAIL)
	    أرجع FAIL;
    }

    lp->ll_tv = &lp->ll_li->li_tv;

    أرجع OK;
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
    ساكن صحيح
get_lval_tuple(
    lval_T	*lp,
    typval_T	*idx,
    صحيح		quiet)
{
    // is number or string
    lp->ll_n1 = (ڊگهو)tv_get_عدد(idx);

    lp->ll_list = عدم;
    lp->ll_dict = عدم;
    lp->ll_blob = عدم;
    lp->ll_object = عدم;
    lp->ll_class = عدم;

    lp->ll_tuple = lp->ll_tv->vval.v_tuple;
    lp->ll_tv = tuple_ڳوليو(lp->ll_tuple, lp->ll_n1);
    إذا (lp->ll_tv == عدم)
    {
	إذا (!quiet)
	    semsg(_(e_tuple_اشاريو_ٻاهريون_جو_range_nr), lp->ll_n1);
	أرجع GLV_FAIL;
    }

    // use the type of the member
    إذا (lp->ll_valtype != عدم)
    {
	إذا (lp->ll_valtype != عدم
		&& lp->ll_valtype->tt_type == VAR_TUPLE
		&& lp->ll_valtype->tt_argcount == 1)
	{
	    // a variadic tuple or a single item tuple
	    إذا (lp->ll_valtype->tt_flags & TTFLAG_VARARGS)
		lp->ll_valtype = lp->ll_valtype->tt_دليلون[0]->tt_member;
	    وإلا
		lp->ll_valtype = lp->ll_valtype->tt_دليلون[0];
	}
	وإلا
	    // If the LHS member type is not known (VAR_ANY), then get it from
	    // the tuple item (after indexing)
	    lp->ll_valtype = typval2type(lp->ll_tv, get_copyID(),
					&lp->ll_type_list, TVTT_DO_MEMBER);
    }

    أرجع GLV_OK;
}

/*
 * Get a class or object lval method in class "cl".  The 'key' argument points
 * to the method name and 'key_end' points to the character after 'key'.
 * 'v_type' is VAR_CLASS or VAR_OBJECT.
 *
 * The method index, method function pointer and method type are returned in
 * "lp".
 */
    ساكن صحيح
get_lval_oc_طريقو(
    lval_T	*lp,
    class_T	*cl,
    char_u	*ڪنجي,
    char_u	**ڪنجي_پڇاڙي,
    vartype_T	v_type)
{
    // Look for a method with this name.
    // round 1: class functions (skipped for an object)
    // round 2: object methods
    لكل (صحيح ڦيرو = v_type == VAR_OBJECT ? 2 : 1; ڦيرو <= 2; ++ڦيرو)
    {
	صحيح	m_idx;
	ufunc_T	*fp;

	fp = طريقو_lookup(cl, ڦيرو == 1 ? VAR_CLASS : VAR_OBJECT,
						ڪنجي, *ڪنجي_پڇاڙي - ڪنجي, &m_idx);
	lp->ll_oi = m_idx;

	// process generic method (if present)
	إذا (fp && (fp = eval_generic_func(fp, ڪنجي, ڪنجي_پڇاڙي)) == عدم)
	    أرجع FAIL;

	إذا (fp != عدم)
	{
	    lp->ll_ufunc = fp;
	    lp->ll_valtype = fp->uf_func_type;
	    اكسر;
	}
    }

    أرجع OK;
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
    ساكن صحيح
get_lval_oc_variable(
    lval_T	*lp,
    class_T	*cl,
    char_u	*ڪنجي,
    char_u	*ڪنجي_پڇاڙي,
    vartype_T	v_type,
    class_T	*cl_exec,
    صحيح		flags)
{
    صحيح		m_idx;
    ocmember_T	*om;

    om = member_lookup(cl, v_type, ڪنجي, ڪنجي_پڇاڙي - ڪنجي, &m_idx);
    lp->ll_oi = m_idx;
    إذا (om == عدم)
	أرجع OK;

    // Check variable is accessible
    إذا (get_lval_check_access(cl_exec, cl, om, ڪنجي_پڇاڙي, flags) == FAIL)
	أرجع FAIL;

    // When lhs is used to modify the variable, check it is not a read-only
    // variable.
    إذا ((flags & GLV_READ_ONLY) == 0 && (*ڪنجي_پڇاڙي != '.' && *ڪنجي_پڇاڙي != '[')
	    && oc_var_check_ro(cl, om))
	أرجع FAIL;

    lp->ll_valtype = om->ocm_type;

    إذا (v_type == VAR_OBJECT)
	lp->ll_tv = ((typval_T *)(lp->ll_tv->vval.v_object + 1)) + m_idx;
    وإلا
	lp->ll_tv = &cl->class_members_tv[m_idx];

    أرجع OK;
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
    ساكن صحيح
get_lval_class_or_obj(
    lval_T	*lp,
    char_u	*ڪنجي,
    char_u	**ڪنجي_پڇاڙي,
    vartype_T	v_type,
    class_T	*cl_exec,
    صحيح		flags,
    صحيح		quiet)
{
    lp->ll_dict = عدم;
    lp->ll_list = عدم;
    lp->ll_tuple = عدم;

    class_T *cl;
    إذا (v_type == VAR_OBJECT)
    {
	إذا (lp->ll_tv->vval.v_object == عدم)
	{
	    إذا (!quiet)
		emsg(_(e_using_null_object));
	    أرجع FAIL;
	}
	cl = lp->ll_tv->vval.v_object->obj_class;
	lp->ll_object = lp->ll_tv->vval.v_object;
    }
    وإلا
    {
	cl = lp->ll_tv->vval.v_class;
	lp->ll_object = عدم;
    }
    lp->ll_class = cl;

    إذا (cl == عدم)
	// TODO: what if class is NULL?
	أرجع OK;

    lp->ll_valtype = عدم;

    إذا (flags & GLV_PREFER_FUNC)
	إذا (get_lval_oc_طريقو(lp, cl, ڪنجي, ڪنجي_پڇاڙي, v_type) == FAIL)
	    أرجع FAIL;

    // Look for object/class member variable
    إذا (lp->ll_valtype == عدم)
    {
	إذا (get_lval_oc_variable(lp, cl, ڪنجي, *ڪنجي_پڇاڙي, v_type, cl_exec, flags)
								== FAIL)
	    أرجع FAIL;
    }

    إذا (lp->ll_valtype == عدم)
    {
	member_not_مليو_نياپو(cl, v_type, ڪنجي, *ڪنجي_پڇاڙي - ڪنجي);
	أرجع FAIL;
    }

    أرجع OK;
}

/*
 * Check whether dot (".") is allowed after the variable "name" with type
 * "v_type".  Only Dict, Class and Object types support a dot after the name.
 * Returns TRUE if dot is allowed after the name.
 */
    ساكن صحيح
dot_allowed_after_type(char_u *name, vartype_T v_type, صحيح quiet)
{
    إذا (v_type != VAR_DICT && v_type != VAR_OBJECT && v_type != VAR_CLASS)
    {
	إذا (!quiet)
	    semsg(_(e_dot_not_allowed_after_str_str),
		    vartype_name(v_type), name);
	أرجع FALSE;
    }

    أرجع TRUE;
}

/*
 * Check whether left bracket ("[") is allowed after the variable "name" with
 * type "v_type".  Only Dict, List, Tuple and Blob types support a bracket
 * after the variable name.  Returns TRUE if bracket is allowed after the name.
 */
    ساكن صحيح
bracket_allowed_after_type(char_u *name, vartype_T v_type, صحيح quiet)
{
    إذا (v_type == VAR_CLASS || v_type == VAR_OBJECT)
    {
	إذا (!quiet)
	    semsg(_(e_اشاريو_not_allowed_after_str_str),
		    vartype_name(v_type), name);
	أرجع FALSE;
    }

    أرجع TRUE;
}

/*
 * Check whether the variable "name" with type "v_type" can be followed by an
 * index.  Only Dict, List, Tuple, Blob, Object and Class types support
 * indexing.  Returns TRUE if indexing is allowed after the name.
 */
    ساكن صحيح
اشاريو_allowed_after_type(char_u *name, vartype_T v_type, صحيح quiet)
{
    إذا (v_type != VAR_LIST
	    && v_type != VAR_TUPLE
	    && v_type != VAR_DICT
	    && v_type != VAR_BLOB
	    && v_type != VAR_OBJECT
	    && v_type != VAR_CLASS)
    {
	إذا (!quiet)
	    semsg(_(e_اشاريو_not_allowed_after_str_str),
		    vartype_name(v_type), name);
	أرجع FALSE;
    }

    أرجع TRUE;
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
    ساكن char_u *
get_lval_subscript(
    lval_T	*lp,
    char_u	*p,
    char_u	*name,
    typval_T	*rettv,
    hashtab_T	*ht,
    dictitem_T	*v,
    صحيح		unlet,
    صحيح		flags,	    // GLV_ values
    class_T	*cl_exec)
{
    صحيح		quiet = flags & GLV_QUIET;
    char_u	*ڪنجي = عدم;
    صحيح		len;
    typval_T	var1;
    typval_T	var2;
    صحيح		empty1 = FALSE;
    صحيح		rc = FAIL;

    /*
     * Loop until no more [idx] or .key is following.
     */
    var1.v_type = VAR_UNKNOWN;
    var2.v_type = VAR_UNKNOWN;

    طالما (*p == '[' || (*p == '.' && p[1] != '=' && p[1] != '.'))
    {
	vartype_T v_type = lp->ll_tv->v_type;

	إذا (*p == '.' && !dot_allowed_after_type(name, v_type, quiet))
	    اذهب پورو;

	إذا (*p == '[' && !bracket_allowed_after_type(name, v_type, quiet))
	    اذهب پورو;

	إذا (!اشاريو_allowed_after_type(name, v_type, quiet))
	    اذهب پورو;

	// A NULL list/blob works like an empty list/blob, allocate one now.
	صحيح r = OK;
	إذا (v_type == VAR_LIST && lp->ll_tv->vval.v_list == عدم)
	    r = rettv_list_alloc(lp->ll_tv);
	وإلا إذا (v_type == VAR_BLOB && lp->ll_tv->vval.v_blob == عدم)
	    r = rettv_blob_alloc(lp->ll_tv);
	إذا (r == FAIL)
	    اذهب پورو;

	إذا (lp->ll_range)
	{
	    إذا (!quiet)
		emsg(_(e_ٽڪرو_must_come_آخري));
	    اذهب پورو;
	}
#جيڪڏهن_وصف_ٿيل LOG_LOCKVAR
	ch_log(عدم, "LKVAR: get_lval() loop: p: %s, type: %s", p,
		vartype_name(v_type));
#جيڪڏهن_پورو

	إذا (current_script_is_vim9() && lp->ll_valtype == عدم
		&& v != عدم
		&& lp->ll_tv == &v->di_tv
		&& ht != عدم && ht == get_script_local_ht())
	{
	    svar_T  *sv = ڳوليو_typval_اندريون_script(lp->ll_tv, 0, TRUE);

	    // Vim9 script local variable: get the type
	    إذا (sv != عدم)
	    {
		lp->ll_valtype = sv->sv_type;
#جيڪڏهن_وصف_ٿيل LOG_LOCKVAR
		ch_log(عدم, "LKVAR:    ... loop: vim9 assign type: %s",
			vartype_name(lp->ll_valtype->tt_type));
#جيڪڏهن_پورو
	    }
	}

	len = -1;
	إذا (*p == '.')
	{
	    ڪنجي = p + 1;

	    لكل (len = 0; ASCII_ISALNUM(ڪنجي[len]) || ڪنجي[len] == '_'; ++len)
		;
	    إذا (len == 0)
	    {
		إذا (!quiet)
		    emsg(_(e_cannot_use_خالي_جايون_ڪنجي_for_dictionary));
		اذهب پورو;
	    }
	    p = ڪنجي + len;
	}
	وإلا
	{
	    // Get the index [expr] or the first index [expr: ].
	    p = skipwhite(p + 1);
	    إذا (*p == ':')
		empty1 = TRUE;
	    وإلا
	    {
		empty1 = FALSE;
		إذا (eval1(&p, &var1, &EVALARG_EVALUATE) == FAIL)  // recursive!
		    اذهب پورو;
		إذا (tv_get_string_chk(&var1) == عدم)
		    // not a number or string
		    اذهب پورو;
		p = skipwhite(p);
	    }

	    // Optionally get the second index [ :expr].
	    إذا (*p == ':')
	    {
		إذا (v_type == VAR_DICT)
		{
		    إذا (!quiet)
			emsg(_(e_cannot_ٽڪرو_dictionary));
		    اذهب پورو;
		}
		إذا (v_type == VAR_TUPLE)
		{
		    إذا (!quiet)
			emsg(_(e_cannot_ٽڪرو_tuple));
		    اذهب پورو;
		}
		إذا (rettv != عدم
			&& !(rettv->v_type == VAR_LIST
			    && rettv->vval.v_list != عدم)
			&& !(rettv->v_type == VAR_BLOB
			    && rettv->vval.v_blob != عدم))
		{
		    إذا (!quiet)
			emsg(_(e_ٽڪرو_requires_list_or_blob_قدر));
		    اذهب پورو;
		}
		p = skipwhite(p + 1);
		إذا (*p == ']')
		    lp->ll_empty2 = TRUE;
		وإلا
		{
		    lp->ll_empty2 = FALSE;
		    // recursive!
		    إذا (eval1(&p, &var2, &EVALARG_EVALUATE) == FAIL)
			اذهب پورو;
		    إذا (tv_get_string_chk(&var2) == عدم)
			// not a number or string
			اذهب پورو;
		}
		lp->ll_range = TRUE;
	    }
	    وإلا
		lp->ll_range = FALSE;

	    إذا (*p != ']')
	    {
		إذا (!quiet)
		    emsg(_(e_missing_closing_چورس_brace));
		اذهب پورو;
	    }

	    // Skip to past ']'.
	    ++p;
	}
#جيڪڏهن_وصف_ٿيل LOG_LOCKVAR
	إذا (len == -1)
	    ch_log(عدم, "LKVAR:    ... loop: p: %s, '[' key: %s", p,
		    empty1 ? ":" : (حرف*)tv_get_string(&var1));
	وإلا
	    ch_log(عدم, "LKVAR:    ... loop: p: %s, '.' key: %s", p, ڪنجي);
#جيڪڏهن_پورو

	إذا (v_type == VAR_DICT)
	{
	    glv_حالت_T glv_حالت;

	    glv_حالت = get_lval_dict_شي(lp, name, ڪنجي, len, &p, &var1,
							flags, unlet, rettv);
	    إذا (glv_حالت == GLV_FAIL)
		اذهب پورو;
	    إذا (glv_حالت == GLV_STOP)
		اكسر;
	}
	وإلا إذا (v_type == VAR_BLOB)
	{
	    إذا (get_lval_blob(lp, &var1, &var2, empty1, quiet) == FAIL)
		اذهب پورو;

	    اكسر;
	}
	وإلا إذا (v_type == VAR_LIST)
	{
	    إذا (get_lval_list(lp, &var1, &var2, empty1, flags, quiet) == FAIL)
		اذهب پورو;
	}
	وإلا إذا (v_type == VAR_TUPLE)
	{
	    إذا (get_lval_tuple(lp, &var1, quiet) == FAIL)
		اذهب پورو;
	}
	وإلا  // v_type == VAR_CLASS || v_type == VAR_OBJECT
	{
	    إذا (get_lval_class_or_obj(lp, ڪنجي, &p, v_type, cl_exec, flags,
							quiet) == FAIL)
		اذهب پورو;
	}

	clear_tv(&var1);
	clear_tv(&var2);
	var1.v_type = VAR_UNKNOWN;
	var2.v_type = VAR_UNKNOWN;
    }

    إذا (lp->ll_tuple != عدم && (flags & GLV_READ_ONLY) == 0)
    {
	إذا (!quiet)
	    emsg(_(e_tuple_is_immutable));
	اذهب پورو;
    }

    rc = OK;

پورو:
    clear_tv(&var1);
    clear_tv(&var2);
    أرجع rc == OK ? p : عدم;
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
    صحيح		unlet,
    صحيح		skip,
    صحيح		flags,	    // GLV_ values
    صحيح		fne_flags)  // flags for find_name_end()
{
    char_u	*p;
    char_u	*اظهار_شروعات, *اظهار_پڇاڙي;
    صحيح		cc;
    dictitem_T	*v = عدم;
    hashtab_T	*ht = عدم;
    صحيح		quiet = flags & GLV_QUIET;
    صحيح		writing = 0;
    صحيح		vim9script = اندريون_vim9script();
    class_T	*cl_exec = عدم;    // class that is executing, or NULL.

#جيڪڏهن_وصف_ٿيل LOG_LOCKVAR
    إذا (lval_پاڙ == عدم)
	ch_log(عدم, "LKVAR: get_lval(): name: %s, lval_root (nil)", name);
    وإلا
	ch_log(عدم, "LKVAR: get_lval(): name: %s, lr_tv %p lr_is_arg %d",
			name, (فراغ*)lval_پاڙ->lr_tv, lval_پاڙ->lr_is_arg);
    حرف buf[80];
    ch_log(عدم, "LKVAR:    ...: GLV flags: %s",
		    flags_tostring(flags, glv_flag_لڙيون, buf, حجم(buf)));
#جيڪڏهن_پورو

    // Clear everything in "lp".
    CLEAR_اشارو(lp);
    ga_init2(&lp->ll_type_list, حجم(type_T *), 10);

    إذا (skip || (flags & GLV_COMPILING))
    {
	// When skipping or compiling just find the end of the name.
	lp->ll_name = name;
	lp->ll_name_پڇاڙي = ڳوليو_name_پڇاڙي(name, عدم, عدم,
						      FNE_INCL_BR | fne_flags);
	أرجع lp->ll_name_پڇاڙي;
    }

    // Cannot use "s:var" at the Vim9 script level.  "s: type" is OK.
    إذا (vim9script && at_script_level()
		  && name[0] == 's' && name[1] == ':' && !VIM_ISWHITE(name[2]))
    {
	semsg(_(e_cannot_use_s_colon_اندريون_vim9_script_str), name);
	أرجع عدم;
    }

    // Find the end of the name.
    p = ڳوليو_name_پڇاڙي(name, &اظهار_شروعات, &اظهار_پڇاڙي, fne_flags);
    lp->ll_name_پڇاڙي = p;
    إذا (اظهار_شروعات != عدم)
    {
	// Don't expand the name when we already know there is an error.
	إذا (unlet && !VIM_ISWHITE(*p) && !پڇاڙيون_excmd(*p)
						    && *p != '[' && *p != '.')
	{
	    semsg(_(e_trailing_characters_str), p);
	    أرجع عدم;
	}

	lp->ll_exp_name = make_expanded_name(name, اظهار_شروعات, اظهار_پڇاڙي, p);
	إذا (lp->ll_exp_name == عدم)
	{
	    // Report an invalid expression in braces, unless the
	    // expression evaluation has been cancelled due to an
	    // aborting error, an interrupt, or an exception.
	    إذا (!aborting() && !quiet)
	    {
		emsg_severe = TRUE;
		semsg(_(e_invalid_دليل_str), name);
		أرجع عدم;
	    }
	}
	lp->ll_name = lp->ll_exp_name;
    }
    وإلا
    {
	lp->ll_name = name;

	إذا (vim9script)
	{
	    // "a: type" is declaring variable "a" with a type, not "a:".
	    // However, "g:[key]" is indexing a dictionary.
	    إذا (p == name + 2 && p[-1] == ':' && *p != '[')
	    {
		--p;
		lp->ll_name_پڇاڙي = p;
	    }
	    إذا (*skipwhite(p) == ':')
	    {
		char_u	    *tp = skipwhite(p + 1);

		إذا (is_scoped_variable(name))
		{
		    semsg(_(e_cannot_use_type_with_this_variable_str), name);
		    أرجع عدم;
		}
		إذا (VIM_ISWHITE(*p))
		{
		    semsg(_(e_no_white_space_allowed_before_colon_str), p);
		    أرجع عدم;
		}
		إذا (tp == p + 1 && !quiet)
		{
		    semsg(_(e_white_space_required_after_str_str), ":", p);
		    أرجع عدم;
		}
		إذا (!SCRIPT_ID_VALID(current_sctx.sc_sid))
		{
		    semsg(_(e_using_type_not_اندريون_script_context_str), p);
		    أرجع عدم;
		}
		إذا (vim9script && (flags & GLV_NO_DECL) &&
			!(flags & GLV_FOR_LOOP))
		{
		    // Using a type and not in a "var" declaration.
		    semsg(_(e_trailing_characters_str), p);
		    أرجع عدم;
		}


		// parse the type after the name
		lp->ll_type = parse_type(&tp,
			       &SCRIPT_شي(current_sctx.sc_sid)->sn_type_list,
			       عدم, عدم, !quiet);
		إذا (!quiet && (lp->ll_type == عدم
			    || !valid_declaration_type(lp->ll_type)))
		    أرجع عدم;
		lp->ll_name_پڇاڙي = tp;
	    }
	    // TODO: check inside class?
	}
    }
    إذا (lp->ll_name == عدم)
	أرجع p;

    إذا (*p == '.')
    {
	// In legacy script, when a local variable and import exists with this name,
	// prioritize local variable over imports to avoid conflicts.
	صحيح var_exists = FALSE;
	إذا (!vim9script)
	{
	    cc = *p;
	    *p = NUL;
	    hashtab_T *local_ht = get_funccal_local_ht();
	    إذا (local_ht != عدم)
	    {
		hashitem_T *hi = hash_ڳوليو(local_ht, lp->ll_name);
		إذا (!HASHITEM_خالي_جايون(hi))
		    var_exists = TRUE;
	    }
	    *p = cc;
	}

	إذا (!var_exists)
	{
	    imported_T *import = ڳوليو_imported(lp->ll_name, p - lp->ll_name, TRUE);
	    إذا (import != عدم)
	    {
		p++;	// skip '.'
		p = get_lval_imported(lp, import->imp_sid, p, &v, fne_flags);
		إذا (p == عدم)
		    أرجع عدم;
	    }
	}
    }

    // Without [idx] or .key we are done.
    إذا (*p != '[' && *p != '.')
    {
	إذا (lval_پاڙ != عدم)
	    fill_lval_from_lval_پاڙ(lp, lval_پاڙ);
	أرجع p;
    }

    إذا (vim9script && lval_پاڙ != عدم)
	cl_exec = lval_پاڙ->lr_cl_exec;
    إذا (vim9script && lval_پاڙ != عدم && lval_پاڙ->lr_tv != عدم)
    {
	// using local variable
	lp->ll_tv = lval_پاڙ->lr_tv;
	v = عدم;
    }
    وإلا إذا (lp->ll_tv == عدم)
    {
	cc = *p;
	*p = NUL;
	// When we would write to the variable pass &ht and prevent autoload.
	writing = !(flags & GLV_READ_ONLY);
	v = ڳوليو_var(lp->ll_name, writing ? &ht : عدم,
					 (flags & GLV_NO_AUTOLOAD) || writing);
	إذا (v == عدم && !quiet)
	    semsg(_(e_undefined_variable_str), lp->ll_name);
	*p = cc;
	إذا (v == عدم)
	    أرجع عدم;
	lp->ll_tv = &v->di_tv;
    }

    إذا (vim9script && (flags & GLV_NO_DECL) == 0)
    {
	إذا (!quiet)
	    semsg(_(e_variable_already_declared_str), lp->ll_name);
	أرجع عدم;
    }

    // If the next character is a "." or a "[", then process the subitem.
    p = get_lval_subscript(lp, p, name, rettv, ht, v, unlet, flags, cl_exec);
    إذا (p == عدم)
	أرجع عدم;

    إذا (vim9script && lp->ll_valtype != عدم && rettv != عدم)
    {
	where_T	    where = WHERE_INIT;

	// In a Vim9 script, do type check and make sure the variable is
	// writable.
	إذا (check_typval_type(lp->ll_valtype, rettv, where) == FAIL)
	    أرجع عدم;
    }

    lp->ll_name_پڇاڙي = p;
    أرجع p;
}

/*
 * Clear lval "lp" that was filled by get_lval().
 */
    فراغ
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
    فراغ
سيٽ_var_lval(
    lval_T	*lp,
    char_u	*endp,
    typval_T	*rettv,
    صحيح		copy,
    صحيح		flags,	    // ASSIGN_CONST, ASSIGN_NO_DECL
    char_u	*op,
    صحيح		var_idx)    // index for "let [a, b] = list"
{
    صحيح		cc;
    dictitem_T	*di;

    إذا (lp->ll_tv == عدم)
    {
	cc = *endp;
	*endp = NUL;
	إذا (اندريون_vim9script() && check_reserved_name(lp->ll_name, FALSE) == FAIL)
	    أرجع;

	إذا (lp->ll_blob != عدم)
	{
	    صحيح	    error = FALSE;

	    إذا (op != عدم && *op != '=')
	    {
		semsg(_(e_wrong_variable_type_for_str_equal), op);
		أرجع;
	    }
	    إذا (قدر_check_تالو(lp->ll_blob->bv_تالو, lp->ll_name, FALSE))
		أرجع;

	    إذا (lp->ll_range && rettv->v_type == VAR_BLOB)
	    {
		إذا (lp->ll_empty2)
		    lp->ll_n2 = blob_len(lp->ll_blob) - 1;

		إذا (blob_سيٽ_range(lp->ll_blob, lp->ll_n1, lp->ll_n2,
								rettv) == FAIL)
		    أرجع;
	    }
	    وإلا
	    {
		varnumber_T	val = tv_get_عدد_chk(rettv, &error);
		إذا (!error)
		{
		    إذا (val < 0 || val > 255)
			semsg(_(e_invalid_قدر_for_blob_nr), val);
		    وإلا
			blob_سيٽ_append(lp->ll_blob, lp->ll_n1, val);
		}
	    }
	}
	وإلا إذا (op != عدم && *op != '=')
	{
	    typval_T tv;

	    إذا ((flags & (سونپيو_CONST | سونپيو_FINAL))
					     && (flags & سونپيو_FOR_LOOP) == 0)
	    {
		emsg(_(e_cannot_modify_existing_variable));
		*endp = cc;
		أرجع;
	    }

	    // handle +=, -=, *=, /=, %= and .=
	    di = عدم;
	    إذا (eval_variable(lp->ll_name, (صحيح)STRLEN(lp->ll_name),
				 lp->ll_sid, &tv, &di, EVAL_VAR_VERBOSE) == OK)
	    {
		إذا (di != عدم && check_typval_is_قدر(&di->di_tv) == FAIL)
		{
		    clear_tv(&tv);
		    أرجع;
		}

		// In Vim9 script a String v: variable only takes a String.
		إذا (اندريون_vim9script() && STRNCMP(lp->ll_name, "v:", 2) == 0
			&& tv.v_type == VAR_STRING)
		{
		    where_T	where = WHERE_INIT;

		    إذا (check_typval_type(&t_string, rettv, where) == FAIL)
		    {
			clear_tv(&tv);
			*endp = cc;
			أرجع;
		    }
		}

		إذا ((di == عدم
			 || (!var_check_ro(di->di_flags, lp->ll_name, FALSE)
			   && !tv_check_تالو(&di->di_tv, lp->ll_name, FALSE)))
			&& tv_op(&tv, rettv, op) == OK)
		    سيٽ_var_const(lp->ll_name, lp->ll_sid, عدم, &tv, FALSE,
				سونپيو_NO_DECL | سونپيو_COMPOUND_OP, 0);
		clear_tv(&tv);
	    }
	}
	وإلا
	{
	    إذا (lp->ll_type != عدم && check_typval_arg_type(lp->ll_type, rettv,
							      عدم, 0) == FAIL)
		أرجع;
	    سيٽ_var_const(lp->ll_name, lp->ll_sid, lp->ll_type, rettv, copy,
							       flags, var_idx);
	}
	*endp = cc;
    }
    وإلا إذا (قدر_check_تالو(lp->ll_newkey == عدم
		? lp->ll_tv->v_تالو
		: lp->ll_tv->vval.v_dict->dv_تالو, lp->ll_name, FALSE))
	;
    وإلا إذا (lp->ll_range)
    {
	إذا ((flags & (سونپيو_CONST | سونپيو_FINAL))
					     && (flags & سونپيو_FOR_LOOP) == 0)
	{
	    emsg(_(e_cannot_تالو_range));
	    أرجع;
	}

	(فراغ)list_سونپيو_range(lp->ll_list, rettv->vval.v_list,
			 lp->ll_n1, lp->ll_n2, lp->ll_empty2, op, lp->ll_name);
    }
    وإلا
    {
	/*
	 * Assign to a List, Dictionary or Object item.
	 */
	إذا ((flags & (سونپيو_CONST | سونپيو_FINAL))
					     && (flags & سونپيو_FOR_LOOP) == 0)
	{
	    emsg(_(e_cannot_تالو_list_or_dict));
	    أرجع;
	}

	إذا (lp->ll_valtype != عدم
		    && check_typval_arg_type(lp->ll_valtype, rettv,
							      عدم, 0) == FAIL)
	{
	    lp->ll_name_پڇاڙي = عدم;
	    أرجع;
	}

	// If the lval is a List and the type of the list is not yet set,
	// then set the item type from the declared type of the variable.
	إذا (اندريون_vim9script() && rettv->v_type == VAR_LIST
				&& rettv->vval.v_list != عدم
				&& rettv->vval.v_list->lv_type == عدم)
	{
	    إذا (lp->ll_tv->v_type == VAR_LIST
		    && lp->ll_tv->vval.v_list != عدم
		    && lp->ll_tv->vval.v_list->lv_type != عدم)
		سيٽ_tv_type(rettv, lp->ll_tv->vval.v_list->lv_type);
	}

	إذا (lp->ll_newkey != عدم)
	{
	    إذا (op != عدم && *op != '=')
	    {
		semsg(_(e_ڪنجي_not_present_اندريون_dictionary_str), lp->ll_newkey);
		أرجع;
	    }
	    إذا (dict_wrong_func_name(lp->ll_tv->vval.v_dict, rettv,
								lp->ll_newkey))
		أرجع;

	    // Need to add an item to the Dictionary.
	    di = dictitem_alloc(lp->ll_newkey);
	    إذا (di == عدم)
		أرجع;
	    إذا (dict_جوڙيو(lp->ll_tv->vval.v_dict, di) == FAIL)
	    {
		vim_free(di);
		أرجع;
	    }
	    lp->ll_tv = &di->di_tv;
	}
	وإلا إذا (op != عدم && *op != '=')
	{
	    tv_op(lp->ll_tv, rettv, op);
	    أرجع;
	}
	وإلا
	    clear_tv(lp->ll_tv);

	/*
	 * Assign the value to the variable or list item.
	 */
	إذا (copy)
	    copy_tv(rettv, lp->ll_tv);
	وإلا
	{
	    *lp->ll_tv = *rettv;
	    lp->ll_tv->v_تالو = 0;
	    init_tv(rettv);
	}
    }
}

/*
 * Handle "blob1 += blob2".
 * Returns OK or FAIL.
 */
    ساكن صحيح
tv_op_blob(typval_T *tv1, typval_T *tv2, char_u *op)
{
    إذا (*op != '+' || tv2->v_type != VAR_BLOB)
	أرجع FAIL;

    // Blob += Blob
    إذا (tv2->vval.v_blob == عدم)
	أرجع OK;

    إذا (tv1->vval.v_blob == عدم)
    {
	tv1->vval.v_blob = tv2->vval.v_blob;
	++tv1->vval.v_blob->bv_refcount;
	أرجع OK;
    }

    blob_T	*b1 = tv1->vval.v_blob;
    blob_T	*b2 = tv2->vval.v_blob;
    صحيح		len = blob_len(b2);

    إذا (len > 0 && ga_grow(&b1->bv_ga, len) == OK)
    {
	mch_memmove((char_u *)b1->bv_ga.ga_معلومات + b1->bv_ga.ga_len,
		    (char_u *)b2->bv_ga.ga_معلومات, (size_t)len);
	b1->bv_ga.ga_len += len;
    }
    وإلا
	لكل (صحيح i = 0; i < len; i++)
	    (فراغ)ga_append(&b1->bv_ga, blob_get(b2, i));

    أرجع OK;
}

/*
 * Handle "list1 += list2".
 * Returns OK or FAIL.
 */
    ساكن صحيح
tv_op_list(typval_T *tv1, typval_T *tv2, char_u *op)
{
    إذا (*op != '+' || tv2->v_type != VAR_LIST)
	أرجع FAIL;

    // List += List
    إذا (tv2->vval.v_list == عدم)
	أرجع OK;

    إذا (tv1->vval.v_list == عدم)
    {
	tv1->vval.v_list = tv2->vval.v_list;
	++tv1->vval.v_list->lv_refcount;
    }
    وإلا
	list_extend(tv1->vval.v_list, tv2->vval.v_list, عدم);

    أرجع OK;
}

/*
 * Handle number operations:
 *	nr += nr , nr -= nr , nr *=nr , nr /= nr , nr %= nr
 *
 * Returns OK or FAIL.
 */
    ساكن صحيح
tv_op_عدد(typval_T *tv1, typval_T *tv2, char_u *op)
{
    varnumber_T	n;
    صحيح		failed = FALSE;

    n = tv_get_عدد(tv1);
    إذا (tv2->v_type == VAR_FLOAT)
    {
	float_T f = n;

	إذا (*op == '%')
	    أرجع FAIL;
	بدل (*op)
	{
	    حالة '+': f += tv2->vval.v_float; اكسر;
	    حالة '-': f -= tv2->vval.v_float; اكسر;
	    حالة '*': f *= tv2->vval.v_float; اكسر;
	    حالة '/': f /= tv2->vval.v_float; اكسر;
	}
	clear_tv(tv1);
	tv1->v_type = VAR_FLOAT;
	tv1->vval.v_float = f;
    }
    وإلا
    {
	بدل (*op)
	{
	    حالة '+': n += tv_get_عدد(tv2); اكسر;
	    حالة '-': n -= tv_get_عدد(tv2); اكسر;
	    حالة '*': n *= tv_get_عدد(tv2); اكسر;
	    حالة '/': n = انگ_divide(n, tv_get_عدد(tv2), &failed); اكسر;
	    حالة '%': n = انگ_modulus(n, tv_get_عدد(tv2), &failed); اكسر;
	}
	clear_tv(tv1);
	tv1->v_type = VAR_عدد;
	tv1->vval.v_عدد = n;
    }

    أرجع failed ? FAIL : OK;
}

/*
 * Append string "s2" to the string in "tv1".
 * Returns OK if "tv1" was grown in place, FAIL otherwise.
 */
    ساكن صحيح
grow_string_tv(typval_T *tv1, char_u *s2)
{
    إذا (tv1->v_type != VAR_STRING || tv1->vval.v_string == عدم)
	أرجع FAIL;

    size_t len1 = STRLEN(tv1->vval.v_string);
    size_t len2 = STRLEN(s2);
    char_u *p = vim_realloc(tv1->vval.v_string, len1 + len2 + 1);

    إذا (p == عدم)
	أرجع FAIL;

    mch_memmove(p + len1, s2, len2 + 1);
    tv1->vval.v_string = p;
    أرجع OK;
}

/*
 * Handle "str1 .= str2"
 * Returns OK or FAIL.
 */
    ساكن صحيح
tv_op_string(typval_T *tv1, typval_T *tv2, char_u *op اڻ_استعمال_ٿيل)
{
    char_u	numbuf[NUMBUFLEN];
    char_u	*s;
    char_u	*s2;

    إذا (tv2->v_type == VAR_FLOAT)
	أرجع FAIL;

    // str .= str
    s2 = tv_get_string_buf(tv2, numbuf);
    إذا (grow_string_tv(tv1, s2) == OK)
	أرجع OK;

    s = tv_get_string(tv1);
    s = concat_str(s, s2);
    clear_tv(tv1);
    tv1->v_type = VAR_STRING;
    tv1->vval.v_string = s;

    أرجع OK;
}

/*
 * Handle "tv1 += tv2", "tv1 -= tv2", "tv1 *= tv2", "tv1 /= tv2", "tv1 %= tv2"
 * and "tv1 .= tv2"
 * Returns OK or FAIL.
 */
    ساكن صحيح
tv_op_nr_or_string(typval_T *tv1, typval_T *tv2, char_u *op)
{
    إذا (tv2->v_type == VAR_LIST)
	أرجع FAIL;

    إذا (vim_strchr((char_u *)"+-*/%", *op) != عدم)
	أرجع tv_op_عدد(tv1, tv2, op);

    أرجع tv_op_string(tv1, tv2, op);
}

/*
 * Handle "f1 += f2", "f1 -= f2", "f1 *= f2", "f1 /= f2".
 * Returns OK or FAIL.
 */
    ساكن صحيح
tv_op_float(typval_T *tv1, typval_T *tv2, char_u *op)
{
    float_T f;

    إذا (*op == '%' || *op == '.'
	    || (tv2->v_type != VAR_FLOAT
		&& tv2->v_type != VAR_عدد
		&& tv2->v_type != VAR_STRING))
	أرجع FAIL;

    إذا (tv2->v_type == VAR_FLOAT)
	f = tv2->vval.v_float;
    وإلا
	f = tv_get_عدد(tv2);
    بدل (*op)
    {
	حالة '+': tv1->vval.v_float += f; اكسر;
	حالة '-': tv1->vval.v_float -= f; اكسر;
	حالة '*': tv1->vval.v_float *= f; اكسر;
	حالة '/': tv1->vval.v_float /= f; اكسر;
    }

    أرجع OK;
}

/*
 * Handle "tv1 += tv2", "tv1 -= tv2", "tv1 *= tv2", "tv1 /= tv2", "tv1 %= tv2"
 * and "tv1 .= tv2"
 * Returns OK or FAIL.
 */
    صحيح
tv_op(typval_T *tv1, typval_T *tv2, char_u *op)
{
    // Can't do anything with a Funcref or Dict on the right.
    // v:true and friends only work with "..=".
    إذا (tv2->v_type == VAR_FUNC || tv2->v_type == VAR_DICT
		|| ((tv2->v_type == VAR_BOOL || tv2->v_type == VAR_SPECIAL)
							&& *op != '.'))
    {
	semsg(_(e_wrong_variable_type_for_str_equal), op);
	أرجع FAIL;
    }

    صحيح retval = FAIL;
    بدل (tv1->v_type)
    {
	حالة VAR_UNKNOWN:
	حالة VAR_ANY:
	حالة VAR_VOID:
	حالة VAR_DICT:
	حالة VAR_FUNC:
	حالة VAR_PARTIAL:
	حالة VAR_BOOL:
	حالة VAR_SPECIAL:
	حالة VAR_ڪم:
	حالة VAR_CHANNEL:
	حالة VAR_INSTR:
	حالة VAR_OBJECT:
	حالة VAR_CLASS:
	حالة VAR_TYPEALIAS:
	حالة VAR_TUPLE:
	    اكسر;

	حالة VAR_BLOB:
	    retval = tv_op_blob(tv1, tv2, op);
	    اكسر;

	حالة VAR_LIST:
	    retval = tv_op_list(tv1, tv2, op);
	    اكسر;

	حالة VAR_عدد:
	حالة VAR_STRING:
	    retval = tv_op_nr_or_string(tv1, tv2, op);
	    اكسر;

	حالة VAR_FLOAT:
	    retval = tv_op_float(tv1, tv2, op);
	    اكسر;
    }

    إذا (retval != OK)
	semsg(_(e_wrong_variable_type_for_str_equal), op);

    أرجع retval;
}

/*
 * Evaluate the expression used in a ":for var in expr" command.
 * "arg" points to "var".
 * Set "*errp" to TRUE for an error, FALSE otherwise;
 * Return a pointer that holds the info.  Null when there is an error.
 */
    فراغ *
eval_for_سٽ(
    char_u	*arg,
    صحيح		*errp,
    exarg_T	*eap,
    evalarg_T	*evalarg)
{
    forinfo_T	*fi;
    char_u	*var_list_پڇاڙي;
    char_u	*اظهار;
    typval_T	tv;
    list_T	*l;
    tuple_T	*tuple;
    صحيح		skip = !(evalarg->eval_flags & EVAL_EVALUATE);

    *errp = TRUE;	// default: there is an error

    fi = ALLOC_CLEAR_ONE(forinfo_T);
    إذا (fi == عدم)
	أرجع عدم;

    var_list_پڇاڙي = skip_var_list(arg, TRUE, &fi->fi_varcount,
						     &fi->fi_semicolon, FALSE);
    إذا (var_list_پڇاڙي == عدم)
	أرجع fi;

    اظهار = skipwhite_and_linebreak(var_list_پڇاڙي, evalarg);
    إذا (اظهار[0] != 'i' || اظهار[1] != 'n'
				  || !(اظهار[2] == NUL || VIM_ISWHITE(اظهار[2])))
    {
	إذا (اندريون_vim9script() && *اظهار == ':' && اظهار != var_list_پڇاڙي)
	    semsg(_(e_no_white_space_allowed_before_colon_str), اظهار);
	وإلا
	    emsg(_(e_missing_اندريون_after_for));
	أرجع fi;
    }

    إذا (skip)
	++emsg_skip;
    اظهار = skipwhite_and_linebreak(اظهار + 2, evalarg);
    إذا (eval0(اظهار, &tv, eap, evalarg) == OK)
    {
	*errp = FALSE;
	إذا (!skip)
	{
	    إذا (tv.v_type == VAR_LIST)
	    {
		l = tv.vval.v_list;
		إذا (l == عدم)
		{
		    // a null list is like an empty list: do nothing
		    clear_tv(&tv);
		}
		وإلا
		{
		    // Need a real list here.
		    CHECK_LIST_MATERIALIZE(l);

		    // No need to increment the refcount, it's already set for
		    // the list being used in "tv".
		    fi->fi_list = l;
		    list_جوڙيو_watch(l, &fi->fi_lw);
		    fi->fi_lw.lw_شي = l->lv_پهريون;
		}
	    }
	    وإلا إذا (tv.v_type == VAR_TUPLE)
	    {
		tuple = tv.vval.v_tuple;
		إذا (tuple == عدم)
		{
		    // a null tuple is like an empty tuple: do nothing
		    clear_tv(&tv);
		}
		وإلا
		{
		    // No need to increment the refcount, it's already set for
		    // the tuple being used in "tv".
		    fi->fi_tuple = tuple;
		    fi->fi_tuple_idx = 0;
		}
	    }
	    وإلا إذا (tv.v_type == VAR_BLOB)
	    {
		fi->fi_bi = 0;
		إذا (tv.vval.v_blob != عدم)
		{
		    // Make a copy, so that the iteration still works when the
		    // blob is changed.
		    fi->fi_blob = blob_copy(tv.vval.v_blob);
		}
		clear_tv(&tv);
	    }
	    وإلا إذا (tv.v_type == VAR_STRING)
	    {
		fi->fi_byte_idx = 0;
		fi->fi_string = tv.vval.v_string;
		tv.vval.v_string = عدم;
		إذا (fi->fi_string == عدم)
		    fi->fi_string = vim_strsave((char_u *)"");
	    }
	    وإلا
	    {
		emsg(_(e_string_list_tuple_or_blob_required));
		clear_tv(&tv);
	    }
	}
    }
    إذا (skip)
	--emsg_skip;
    fi->fi_break_ڳڻپ = evalarg->eval_break_ڳڻپ;

    أرجع fi;
}

/*
 * Used when looping over a :for line, skip the "in expr" part.
 */
    فراغ
skip_for_سٽون(فراغ *fi_void, evalarg_T *evalarg)
{
    forinfo_T	*fi = (forinfo_T *)fi_void;
    صحيح		i;

    لكل (i = 0; i < fi->fi_break_ڳڻپ; ++i)
	eval_ايندڙ_سٽ(عدم, evalarg);
}

/*
 * Use the first item in a ":for" list.  Advance to the next.
 * Assign the values to the variable (list).  "arg" points to the first one.
 * Return TRUE when a valid item was found, FALSE when at end of list or
 * something wrong.
 */
    صحيح
ايندڙ_for_شي(فراغ *fi_void, char_u *arg)
{
    forinfo_T	*fi = (forinfo_T *)fi_void;
    صحيح		نتيجو;
    صحيح		flag = سونپيو_FOR_LOOP | (اندريون_vim9script()
			 ? (سونپيو_FINAL
			     // first round: error if variable exists
			     | (fi->fi_bi == 0 ? 0 : سونپيو_DECL)
			     | سونپيو_NO_MEMBER_TYPE
			     | سونپيو_UPDATE_بلاڪ_ID)
			 : 0);
    listitem_T	*شي;
    صحيح		skip_سونپيو = اندريون_vim9script() && arg[0] == '_'
						      && !eval_isnamec(arg[1]);

    إذا (fi->fi_blob != عدم)
    {
	typval_T	tv;

	إذا (fi->fi_bi >= blob_len(fi->fi_blob))
	    أرجع FALSE;
	tv.v_type = VAR_عدد;
	tv.v_تالو = VAR_FIXED;
	tv.vval.v_عدد = blob_get(fi->fi_blob, fi->fi_bi);
	++fi->fi_bi;
	إذا (skip_سونپيو)
	    أرجع TRUE;
	أرجع ex_let_vars(arg, &tv, TRUE, fi->fi_semicolon,
					    fi->fi_varcount, flag, عدم) == OK;
    }

    إذا (fi->fi_string != عدم)
    {
	typval_T	tv;
	صحيح		len;

	len = mb_ptr2len(fi->fi_string + fi->fi_byte_idx);
	إذا (len == 0)
	    أرجع FALSE;
	tv.v_type = VAR_STRING;
	tv.v_تالو = VAR_FIXED;
	tv.vval.v_string = vim_strnsave(fi->fi_string + fi->fi_byte_idx, len);
	fi->fi_byte_idx += len;
	++fi->fi_bi;
	إذا (skip_سونپيو)
	    نتيجو = TRUE;
	وإلا
	    نتيجو = ex_let_vars(arg, &tv, TRUE, fi->fi_semicolon,
					    fi->fi_varcount, flag, عدم) == OK;
	vim_free(tv.vval.v_string);
	أرجع نتيجو;
    }

    إذا (fi->fi_tuple != عدم)
    {
	typval_T	tv;

	إذا (fi->fi_tuple_idx >= TUPLE_LEN(fi->fi_tuple))
	    أرجع FALSE;

	copy_tv(TUPLE_شي(fi->fi_tuple, fi->fi_tuple_idx), &tv);
	++fi->fi_tuple_idx;
	++fi->fi_bi;
	إذا (skip_سونپيو)
	    نتيجو = TRUE;
	وإلا
	    نتيجو = ex_let_vars(arg, &tv, TRUE, fi->fi_semicolon,
					    fi->fi_varcount, flag, عدم) == OK;
	clear_tv(&tv);
	أرجع نتيجو;
    }

    شي = fi->fi_lw.lw_شي;
    إذا (شي == عدم)
	نتيجو = FALSE;
    وإلا
    {
	fi->fi_lw.lw_شي = شي->li_ايندڙ;
	++fi->fi_bi;
	إذا (skip_سونپيو)
	    نتيجو = TRUE;
	وإلا
	    نتيجو = (ex_let_vars(arg, &شي->li_tv, TRUE, fi->fi_semicolon,
					   fi->fi_varcount, flag, عدم) == OK);
    }
    أرجع نتيجو;
}

/*
 * Free the structure used to store info used by ":for".
 */
    فراغ
free_for_info(فراغ *fi_void)
{
    forinfo_T    *fi = (forinfo_T *)fi_void;

    إذا (fi == عدم)
	أرجع;
    إذا (fi->fi_list != عدم)
    {
	list_rem_watch(fi->fi_list, &fi->fi_lw);
	list_unref(fi->fi_list);
    }
    وإلا إذا (fi->fi_blob != عدم)
	blob_unref(fi->fi_blob);
    وإلا إذا (fi->fi_tuple != عدم)
	tuple_unref(fi->fi_tuple);
    وإلا
	vim_free(fi->fi_string);
    vim_free(fi);
}

    فراغ
سيٽ_context_for_اظهار(
    expand_T	*xp,
    char_u	*arg,
    cmdidx_T	cmdidx)
{
    صحيح		has_اظهار = cmdidx != CMD_let && cmdidx != CMD_var;
    صحيح		c;
    char_u	*p;

    إذا (cmdidx == CMD_let || cmdidx == CMD_var
				 || cmdidx == CMD_const || cmdidx == CMD_final)
    {
	xp->xp_context = EXPAND_USER_VARS;
	إذا (vim_strpbrk(arg, (char_u *)"\"'+-*/%.=!?~|&$([<>,#") == عدم)
	{
	    // ":let var1 var2 ...": find last space.
	    لكل (p = arg + STRLEN(arg); p >= arg; )
	    {
		xp->xp_نمونو = p;
		MB_PTR_پٺ(arg, p);
		إذا (VIM_ISWHITE(*p))
		    اكسر;
	    }
	    أرجع;
	}
    }
    وإلا
	xp->xp_context = cmdidx == CMD_call ? EXPAND_FUNCTIONS
							  : EXPAND_اظهار;
    طالما ((xp->xp_نمونو = vim_strpbrk(arg,
				  (char_u *)"\"'+-*/%.=!?~|&$([<>,#")) != عدم)
    {
	c = *xp->xp_نمونو;
	إذا (c == '&')
	{
	    c = xp->xp_نمونو[1];
	    إذا (c == '&')
	    {
		++xp->xp_نمونو;
		xp->xp_context = has_اظهار ? EXPAND_اظهار : EXPAND_NOTHING;
	    }
	    وإلا إذا (c != ' ')
	    {
		xp->xp_context = EXPAND_SETTINGS;
		إذا ((c == 'l' || c == 'g') && xp->xp_نمونو[2] == ':')
		    xp->xp_نمونو += 2;

	    }
	}
	وإلا إذا (c == '$')
	{
	    // environment variable
	    xp->xp_context = EXPAND_ENV_VARS;
	}
	وإلا إذا (c == '=')
	{
	    has_اظهار = TRUE;
	    xp->xp_context = EXPAND_اظهار;
	}
	وإلا إذا (c == '#'
		&& xp->xp_context == EXPAND_اظهار)
	{
	    // Autoload function/variable contains '#'.
	    اكسر;
	}
	وإلا إذا ((c == '<' || c == '#')
		&& xp->xp_context == EXPAND_FUNCTIONS
		&& vim_strchr(xp->xp_نمونو, '(') == عدم)
	{
	    // Function name can start with "<SNR>" and contain '#'.
	    اكسر;
	}
	وإلا إذا (has_اظهار)
	{
	    إذا (c == '"')	    // string
	    {
		طالما ((c = *++xp->xp_نمونو) != NUL && c != '"')
		    إذا (c == '\\' && xp->xp_نمونو[1] != NUL)
			++xp->xp_نمونو;
		xp->xp_context = EXPAND_NOTHING;
	    }
	    وإلا إذا (c == '\'')	    // literal string
	    {
		// Trick: '' is like stopping and starting a literal string.
		طالما ((c = *++xp->xp_نمونو) != NUL && c != '\'')
		    /* skip */ ;
		xp->xp_context = EXPAND_NOTHING;
	    }
	    وإلا إذا (c == '|')
	    {
		إذا (xp->xp_نمونو[1] == '|')
		{
		    ++xp->xp_نمونو;
		    xp->xp_context = EXPAND_اظهار;
		}
		وإلا
		    xp->xp_context = EXPAND_COMMANDS;
	    }
	    وإلا
		xp->xp_context = EXPAND_اظهار;
	}
	وإلا
	    // Doesn't look like something valid, expand as an expression
	    // anyway.
	    xp->xp_context = EXPAND_اظهار;
	arg = xp->xp_نمونو;
	إذا (*arg != NUL)
	    طالما ((c = *++arg) != NUL && (c == ' ' || c == '\t'))
		/* skip */ ;
    }

    // ":exe one two" completes "two"
    إذا ((cmdidx == CMD_execute
		|| cmdidx == CMD_echo
		|| cmdidx == CMD_echon
		|| cmdidx == CMD_echomsg
		|| cmdidx == CMD_echoerr
		|| cmdidx == CMD_echoconsole
		|| cmdidx == CMD_echowindow)
	    && xp->xp_context == EXPAND_اظهار)
    {
	لكل (;;)
	{
	    char_u *n = skiptowhite(arg);

	    إذا (n == arg || IS_WHITE_OR_NUL(*skipwhite(n)))
		اكسر;
	    arg = skipwhite(n);
	}
    }

    xp->xp_نمونو = arg;
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
ساكن regprog_T    *eval_prog_cache = عدم;
ساكن char_u	    *eval_prog_pat = عدم;	// pattern it was compiled for
ساكن char_u	    *eval_prog_enc = عدم;	// 'encoding' when compiled
ساكن ڊگهو	    eval_prog_re;		// 'regexpengine' when compiled
// State sampled by eval_regcomp() for the eval_regfree() call that gives the
// program back: user code run in between, e.g. an object's string() method
// invoked by match() on a list, can change these options, and then the
// program must not be cached under the changed values.  eval_compile_enc is
// only ever compared against p_enc, never dereferenced.
ساكن ڊگهو	    eval_compile_re;
ساكن char_u	    *eval_compile_enc;
ساكن صحيح	    eval_prog_busy = 0;	// programs eval_regcomp() handed out
					// that were not given back yet
ساكن صحيح	    eval_prog_reentered = FALSE;  // eval_regcomp() ran while
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
    ساكن bool
eval_prog_volatile(char_u *pat)
{
    char_u *p;

    إذا (STRNCMP(pat, "\\%#=", 4) == 0)
	أرجع true;
    لكل (p = pat; *p != NUL; ++p)
	إذا (*p == '~'
		|| (p[0] == '[' && (p[1] == ':' || p[1] == '=' || p[1] == '.'))
		|| (p[0] == '%' && (p[1] == '.'
			|| ((p[1] == '<' || p[1] == '>') && p[2] == '.'))))
	    أرجع true;
    أرجع false;
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
    إذا (eval_prog_busy > 0)
	// The samples above no longer describe the program already handed
	// out: its eval_regfree() must drop it instead of caching it.
	eval_prog_reentered = TRUE;

    إذا (eval_prog_cache != عدم
	    && *p_cpo == NUL
	    && eval_prog_cache->re_flags == RE_MAGIC + RE_STRING
	    && eval_prog_re == p_re
	    && STRCMP(eval_prog_pat, pat) == 0
	    && STRCMP(eval_prog_enc, p_enc) == 0)
    {
	prog = eval_prog_cache;
	// The caller now owns the program, eval_regfree() adopts the
	// program the caller ends up with.
	eval_prog_cache = عدم;
    }
    وإلا
	prog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
    إذا (prog != عدم)
	++eval_prog_busy;	// paired with the eval_regfree() call
    أرجع prog;
}

/*
 * Free program "prog", obtained with eval_regcomp() for pattern "pat", by
 * keeping it in the cache for the next use.  "pat" must be the same string
 * the program was compiled for and must still be valid.
 */
    فراغ
eval_regfree(char_u *pat, regprog_T *prog)
{
    صحيح	    reentered = eval_prog_reentered;

    إذا (eval_prog_busy > 0 && --eval_prog_busy == 0)
	eval_prog_reentered = FALSE;
    إذا (prog == عدم)
	أرجع;
    إذا (reentered || *p_cpo != NUL
	    || eval_compile_re != p_re || eval_compile_enc != p_enc)
    {
	// 'cpoptions' is not empty, an option in the cache key changed, or
	// another program was handed out while the caller was using this
	// one: the samples may not describe this program, do not cache it.
	vim_regfree(prog);
	أرجع;
    }
    إذا (eval_prog_pat == عدم || STRCMP(eval_prog_pat, pat) != 0
	    || STRCMP(eval_prog_enc, p_enc) != 0)
    {
	char_u	*pat_copy;
	char_u	*enc_copy;

	// A cached pattern is known not to be volatile, thus the scan is
	// only needed when installing a new key.
	إذا (eval_prog_volatile(pat))
	{
	    // compiling depends on state the cache key does not cover
	    vim_regfree(prog);
	    أرجع;
	}
	pat_copy = vim_strsave(pat);
	enc_copy = vim_strsave(p_enc);
	إذا (pat_copy == عدم || enc_copy == عدم)
	{
	    vim_free(pat_copy);
	    vim_free(enc_copy);
	    vim_regfree(prog);
	    أرجع;
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

#إذا defined(EXITFREE)
    فراغ
free_eval_regcomp_cache(فراغ)
{
    vim_regfree(eval_prog_cache);
    eval_prog_cache = عدم;
    VIM_CLEAR(eval_prog_pat);
    VIM_CLEAR(eval_prog_enc);
    eval_prog_busy = 0;
    eval_prog_reentered = FALSE;
}
#جيڪڏهن_پورو

/*
 * Return TRUE if "pat" matches "text".
 * Does not use 'cpo' and always uses 'magic'.
 */
    صحيح
نمونو_ميلاپ(char_u *pat, char_u *متن, صحيح ic)
{
    صحيح		matches = FALSE;
    char_u	*save_cpo;
    regmatch_T	regmatch;

    // avoid 'l' flag in 'cpoptions'
    save_cpo = p_cpo;
    p_cpo = خالي_جايون_option;
    regmatch.regprog = eval_regcomp(pat);
    إذا (regmatch.regprog != عدم)
    {
	regmatch.rm_ic = ic;
	matches = vim_regexec_nl(&regmatch, متن, (colnr_T)0);
	eval_regfree(pat, regmatch.regprog);
    }
    p_cpo = save_cpo;
    أرجع matches;
}

/*
 * Handle a name followed by "(".  Both for just "name(arg)" and for
 * "expr->name(arg)".
 * Returns OK or FAIL.
 */
    ساكن صحيح
eval_func(
	char_u	    **arg,	// points to "(", will be advanced
	evalarg_T   *evalarg,
	char_u	    *name,
	صحيح	    name_len,
	typval_T    *rettv,
	صحيح	    flags,
	typval_T    *basetv)	// "expr" for "expr->name(arg)"
{
    صحيح		evaluate = flags & EVAL_EVALUATE;
    char_u	*s = name;
    صحيح		len = name_len;
    partial_T	*partial;
    صحيح		ret = OK;
    type_T	*type = عدم;
    صحيح		مليو_var = FALSE;

    إذا (!evaluate)
	check_vars(s, len);

    // If "s" is the name of a variable of type VAR_FUNC
    // use its contents.
    s = deref_func_name(s, &len, &partial,
		 اندريون_vim9script() ? &type : عدم, !evaluate, FALSE, &مليو_var);

    // Need to make a copy, in case evaluating the arguments makes
    // the name invalid.
    s = vim_strsave(s);
    إذا (s == عدم || (evaluate && *s == NUL))
	ret = FAIL;
    وإلا
    {
	funcexe_T funcexe;

	// Invoke the function.
	CLEAR_ميدان(funcexe);
	funcexe.fe_firstline = curwin->w_cursor.lnum;
	funcexe.fe_lastline = curwin->w_cursor.lnum;
	funcexe.fe_evaluate = evaluate;
	funcexe.fe_partial = partial;
	إذا (partial != عدم)
	{
	    funcexe.fe_object = partial->pt_obj;
	    إذا (funcexe.fe_object != عدم)
		++funcexe.fe_object->obj_refcount;
	}
	funcexe.fe_basetv = basetv;
	funcexe.fe_check_type = type;
	funcexe.fe_مليو_var = مليو_var;
	إذا (evalarg != عدم)
	    funcexe.fe_cctx = evalarg->eval_cctx;
	ret = get_func_tv(s, len, rettv, arg, evalarg, &funcexe);
    }
    vim_free(s);

    // If evaluate is FALSE rettv->v_type was not set in
    // get_func_tv, but it's needed in handle_subscript() to parse
    // what follows. So set it here.
    إذا (rettv->v_type == VAR_UNKNOWN && !evaluate && **arg == '(')
    {
	rettv->vval.v_string = عدم;
	rettv->v_type = VAR_FUNC;
    }

    // Stop the expression evaluation when immediately
    // aborting on error, or when an interrupt occurred or
    // an exception was thrown but not caught.
    إذا (evaluate && aborting())
    {
	إذا (ret == OK)
	    clear_tv(rettv);
	ret = FAIL;
    }
    أرجع ret;
}

/*
 * After a NL, skip over empty lines and comment-only lines.
 */
    ساكن char_u *
newline_skip_comments(char_u *arg)
{
    char_u *p = arg + 1;

    لكل (;;)
    {
	p = skipwhite(p);

	إذا (*p == NUL)
	    اكسر;
	إذا (vim9_comment_شروعات(p))
	{
	    char_u *nl = vim_strchr(p, NL);

	    إذا (nl == عدم)
		اكسر;
	    p = nl;
	}
	إذا (*p != NL)
	    اكسر;
	++p;  // skip another NL
    }
    أرجع p;
}

/*
 * Get the next line source line without advancing.  But do skip over comment
 * lines.
 * Only called for Vim9 script.
 */
    ساكن char_u *
getline_peek_skip_comments(evalarg_T *evalarg)
{
    لكل (;;)
    {
	char_u *ايندڙ = getline_peek(evalarg->eval_getline,
							 evalarg->eval_cookie);
	char_u *p;

	إذا (ايندڙ == عدم)
	    اكسر;
	p = skipwhite(ايندڙ);
	إذا (*p != NUL && !vim9_comment_شروعات(p))
	    أرجع ايندڙ;
	إذا (eval_ايندڙ_سٽ(عدم, evalarg) == عدم)
	    اكسر;
    }
    أرجع عدم;
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
eval_ايندڙ_non_blank(char_u *arg, evalarg_T *evalarg, صحيح *getnext)
{
    char_u *p = skipwhite(arg);

    *getnext = FALSE;
    إذا (اندريون_vim9script()
	    && evalarg != عدم
	    && (evalarg->eval_cookie != عدم || evalarg->eval_cctx != عدم
								   || *p == NL)
	    && (*p == NUL || *p == NL
			     || (vim9_comment_شروعات(p) && VIM_ISWHITE(p[-1]))))
    {
	char_u *ايندڙ;

	إذا (*p == NL)
	    ايندڙ = newline_skip_comments(p);
	وإلا إذا (evalarg->eval_cookie != عدم)
	    ايندڙ = getline_peek_skip_comments(evalarg);
	وإلا
	    ايندڙ = peek_ايندڙ_سٽ_from_context(evalarg->eval_cctx);

	إذا (ايندڙ != عدم)
	{
	    *getnext = *p != NL;
	    أرجع skipwhite(ايندڙ);
	}
    }
    أرجع p;
}

/*
 * To be called after eval_next_non_blank() sets "getnext" to TRUE.
 * Only called for Vim9 script.
 *
 * If "arg" is not NULL, then the caller should assign the return value to
 * "arg".
 */
    char_u *
eval_ايندڙ_سٽ(char_u *arg, evalarg_T *evalarg)
{
    garray_T	*gap = &evalarg->eval_ga;
    char_u	*سٽ;

    إذا (arg != عدم)
    {
	إذا (*arg == NL)
	    أرجع newline_skip_comments(arg);
	// Truncate before a trailing comment, so that concatenating the lines
	// won't turn the rest into a comment.
	إذا (*skipwhite(arg) == '#')
	    *arg = NUL;
    }

    إذا (evalarg->eval_cookie != عدم)
	سٽ = evalarg->eval_getline(0, evalarg->eval_cookie, 0,
							   GETLINE_CONCAT_ALL);
    وإلا
	سٽ = ايندڙ_سٽ_from_context(evalarg->eval_cctx, TRUE);
    إذا (سٽ == عدم)
	أرجع عدم;

    ++evalarg->eval_break_ڳڻپ;
    إذا (gap->ga_itemsize > 0 && ga_grow(gap, 1) == OK)
    {
	char_u *p = skipwhite(سٽ);

	// Going to concatenate the lines after parsing.  For an empty or
	// comment line use an empty string.
	إذا (*p == NUL || vim9_comment_شروعات(p))
	{
	    vim_free(سٽ);
	    سٽ = vim_strsave((char_u *)"");
	}

	((char_u **)gap->ga_معلومات)[gap->ga_len] = سٽ;
	++gap->ga_len;
    }
    وإلا إذا (evalarg->eval_cookie != عدم)
    {
	free_eval_tofree_later(evalarg);
	evalarg->eval_tofree = سٽ;
    }

    // Advanced to the next line, "arg" no longer points into the previous
    // line.  The caller assigns the return value to "arg".
    // If "arg" is NULL, then the return value is discarded.  In that case,
    // "arg" still points to the previous line.  So don't reset
    // "eval_using_cmdline".
    إذا (arg != عدم)
	evalarg->eval_using_cmdline = FALSE;
    أرجع skipwhite(سٽ);
}

/*
 * Call eval_next_non_blank() and get the next line if needed.
 */
    char_u *
skipwhite_and_linebreak(char_u *arg, evalarg_T *evalarg)
{
    صحيح	    getnext;
    char_u  *p = skipwhite_and_nl(arg);

    إذا (evalarg == عدم)
	أرجع skipwhite(arg);
    eval_ايندڙ_non_blank(p, evalarg, &getnext);
    إذا (getnext)
	أرجع eval_ايندڙ_سٽ(arg, evalarg);
    أرجع p;
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
    صحيح
eval0(
    char_u	*arg,
    typval_T	*rettv,
    exarg_T	*eap,
    evalarg_T	*evalarg)
{
    أرجع eval0_retarg(arg, rettv, eap, evalarg, عدم);
}

/*
 * If "arg" is a simple function call without arguments then call it and return
 * the result.  Otherwise return NOTDONE.
 */
    صحيح
may_call_simple_func(
    char_u	*arg,
    typval_T	*rettv)
{
    char_u  *parens = (char_u *)لڙي_ڳولا((حرف *)arg, "()");
    صحيح	    r = NOTDONE;

    // If the expression is "FuncName()" then we can skip a lot of overhead.
    إذا (parens != عدم && *skipwhite(parens + 2) == NUL)
    {
	char_u *p = STRNCMP(arg, "<SNR>", 5) == 0 ? skipdigits(arg + 5) : arg;

	إذا (منزل_name_پڇاڙي(p, TRUE) == parens)
	    r = call_simple_func(arg, (size_t)(parens - arg), rettv);
    }
    أرجع r;
}

/*
 * Handle zero level expression with optimization for a simple function call.
 * Same arguments and return value as eval0().
 */
    ساكن صحيح
eval0_simple_funccal(
    char_u	*arg,
    typval_T	*rettv,
    exarg_T	*eap,
    evalarg_T	*evalarg)
{
    صحيح	    r = may_call_simple_func(arg, rettv);

    إذا (r == NOTDONE)
	r = eval0_retarg(arg, rettv, eap, evalarg, عدم);
    أرجع r;
}

/*
 * Like eval0() but when "retarg" is not NULL store the pointer to after the
 * expression and don't check what comes after the expression.
 */
    صحيح
eval0_retarg(
    char_u	*arg,
    typval_T	*rettv,
    exarg_T	*eap,
    evalarg_T	*evalarg,
    char_u	**retarg)
{
    صحيح		ret;
    char_u	*p;
    char_u	*اظهار_پڇاڙي;
    صحيح		did_emsg_before = did_emsg;
    صحيح		called_emsg_before = called_emsg;
    صحيح		check_for_پڇاڙي = retarg == عدم;
    صحيح		پڇاڙي_error = FALSE;

    p = skipwhite(arg);
    ret = eval1(&p, rettv, evalarg);

    إذا (ret != FAIL)
    {
	اظهار_پڇاڙي = p;
	p = skipwhite(p);

	// In Vim9 script a command block is not split at NL characters for
	// commands using an expression argument.  Skip over a '#' comment to
	// check for a following NL.  Require white space before the '#'.
	إذا (اندريون_vim9script() && p > اظهار_پڇاڙي && retarg == عدم)
	    طالما (*p == '#')
	    {
		char_u *nl = vim_strchr(p, NL);

		إذا (nl == عدم)
		    اكسر;
		p = skipwhite(nl + 1);
		إذا (eap != عدم && *p != NUL)
		    eap->nextcmd = p;
		check_for_پڇاڙي = FALSE;
	    }

	إذا (check_for_پڇاڙي)
	    پڇاڙي_error = !پڇاڙيون_excmd2(arg, p);
    }

    إذا (ret == FAIL || پڇاڙي_error)
    {
	إذا (ret != FAIL)
	    clear_tv(rettv);
	/*
	 * Report the invalid expression unless the expression evaluation has
	 * been cancelled due to an aborting error, an interrupt, or an
	 * exception, or we already gave a more specific error.
	 * Also check called_emsg for when using assert_fails().
	 */
	إذا (!aborting()
		&& did_emsg == did_emsg_before
		&& called_emsg == called_emsg_before
		&& (!اندريون_vim9script() || !vim9_bad_comment(p)))
	{
	    إذا (پڇاڙي_error)
		semsg(_(e_trailing_characters_str), p);
	    وإلا
		semsg(_(e_invalid_اظهار_str), arg);
	}

	إذا (eap != عدم && p != عدم)
	{
	    // Some of the expression may not have been consumed.
	    // Only execute a next command if it cannot be a "||" operator.
	    // The next command may be "catch".
	    char_u *nextcmd = check_nextcmd(p);
	    إذا (nextcmd != عدم && *nextcmd != '|')
		eap->nextcmd = nextcmd;
	}
	أرجع FAIL;
    }

    إذا (retarg != عدم)
	*retarg = p;
    وإلا إذا (check_for_پڇاڙي && eap != عدم)
	سيٽ_nextcmd(eap, p);

    أرجع ret;
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
    صحيح
eval1(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    char_u  *p;
    صحيح	    getnext;

    CLEAR_اشارو(rettv);

    /*
     * Get the first variable.
     */
    إذا (eval2(arg, rettv, evalarg) == FAIL)
	أرجع FAIL;

    p = eval_ايندڙ_non_blank(*arg, evalarg, &getnext);
    إذا (*p == '?')
    {
	صحيح		op_falsy = p[1] == '?';
	صحيح		نتيجو;
	typval_T	var2;
	evalarg_T	*evalarg_استعمال_ٿيل = evalarg;
	evalarg_T	local_evalarg;
	صحيح		orig_flags;
	صحيح		evaluate;
	صحيح		vim9script = اندريون_vim9script();

	إذا (evalarg == عدم)
	{
	    init_evalarg(&local_evalarg);
	    evalarg_استعمال_ٿيل = &local_evalarg;
	}
	orig_flags = evalarg_استعمال_ٿيل->eval_flags;
	evaluate = evalarg_استعمال_ٿيل->eval_flags & EVAL_EVALUATE;

	إذا (getnext)
	    *arg = eval_ايندڙ_سٽ(*arg, evalarg_استعمال_ٿيل);
	وإلا
	{
	    إذا (evaluate && vim9script && !VIM_ISWHITE(p[-1]))
	    {
		error_white_both(p, op_falsy ? 2 : 1);
		clear_tv(rettv);
		أرجع FAIL;
	    }
	    *arg = p;
	}

	نتيجو = FALSE;
	إذا (evaluate)
	{
	    صحيح		error = FALSE;

	    إذا (op_falsy)
	    {
		// Is this typeval supported with the falsy operator?
		إذا (check_typval_is_قدر(rettv) == FAIL)
		{
		    clear_tv(rettv);
		    أرجع FAIL;
		}
		نتيجو = tv2bool(rettv);
	    }
	    وإلا إذا (vim9script)
		نتيجو = tv_get_bool_chk(rettv, &error);
	    وإلا إذا (tv_get_عدد_chk(rettv, &error) != 0)
		نتيجو = TRUE;
	    إذا (error || !op_falsy || !نتيجو)
		clear_tv(rettv);
	    إذا (error)
		أرجع FAIL;
	}

	/*
	 * Get the second variable.  Recursive!
	 */
	إذا (op_falsy)
	    ++*arg;
	إذا (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[1]))
	{
	    error_white_both(*arg - (op_falsy ? 1 : 0), op_falsy ? 2 : 1);
	    clear_tv(rettv);
	    أرجع FAIL;
	}
	*arg = skipwhite_and_linebreak(*arg + 1, evalarg_استعمال_ٿيل);
	evalarg_استعمال_ٿيل->eval_flags = (op_falsy ? !نتيجو : نتيجو)
				  ? orig_flags : (orig_flags & ~EVAL_EVALUATE);
	إذا (eval1(arg, &var2, evalarg_استعمال_ٿيل) == FAIL)
	{
	    evalarg_استعمال_ٿيل->eval_flags = orig_flags;
	    أرجع FAIL;
	}
	إذا (!op_falsy || !نتيجو)
	    *rettv = var2;

	إذا (!op_falsy)
	{
	    /*
	     * Check for the ":".
	     */
	    p = eval_ايندڙ_non_blank(*arg, evalarg_استعمال_ٿيل, &getnext);
	    إذا (*p != ':')
	    {
		emsg(_(e_missing_colon_after_questionmark));
		إذا (evaluate && نتيجو)
		    clear_tv(rettv);
		evalarg_استعمال_ٿيل->eval_flags = orig_flags;
		أرجع FAIL;
	    }
	    إذا (getnext)
		*arg = eval_ايندڙ_سٽ(*arg, evalarg_استعمال_ٿيل);
	    وإلا
	    {
		إذا (evaluate && vim9script && !VIM_ISWHITE(p[-1]))
		{
		    error_white_both(p, 1);
		    clear_tv(rettv);
		    evalarg_استعمال_ٿيل->eval_flags = orig_flags;
		    أرجع FAIL;
		}
		*arg = p;
	    }

	    /*
	     * Get the third variable.  Recursive!
	     */
	    إذا (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[1]))
	    {
		error_white_both(*arg, 1);
		clear_tv(rettv);
		evalarg_استعمال_ٿيل->eval_flags = orig_flags;
		أرجع FAIL;
	    }
	    *arg = skipwhite_and_linebreak(*arg + 1, evalarg_استعمال_ٿيل);
	    evalarg_استعمال_ٿيل->eval_flags = !نتيجو ? orig_flags
					       : (orig_flags & ~EVAL_EVALUATE);
	    إذا (eval1(arg, &var2, evalarg_استعمال_ٿيل) == FAIL)
	    {
		إذا (evaluate && نتيجو)
		    clear_tv(rettv);
		evalarg_استعمال_ٿيل->eval_flags = orig_flags;
		أرجع FAIL;
	    }
	    إذا (evaluate && !نتيجو)
		*rettv = var2;
	}

	إذا (evalarg == عدم)
	    clear_evalarg(&local_evalarg, عدم);
	وإلا
	    evalarg->eval_flags = orig_flags;
    }

    أرجع OK;
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
    ساكن صحيح
eval2(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    char_u	*p;
    صحيح		getnext;

    /*
     * Get the first expression.
     */
    إذا (eval3(arg, rettv, evalarg) == FAIL)
	أرجع FAIL;

    /*
     * Handle the  "||" operator.
     */
    p = eval_ايندڙ_non_blank(*arg, evalarg, &getnext);
    إذا (p[0] == '|' && p[1] == '|')
    {
	evalarg_T   *evalarg_استعمال_ٿيل = evalarg;
	evalarg_T   local_evalarg;
	صحيح	    evaluate;
	صحيح	    orig_flags;
	ڊگهو	    نتيجو = FALSE;
	typval_T    var2;
	صحيح	    error = FALSE;
	صحيح	    vim9script = اندريون_vim9script();

	إذا (evalarg == عدم)
	{
	    init_evalarg(&local_evalarg);
	    evalarg_استعمال_ٿيل = &local_evalarg;
	}
	orig_flags = evalarg_استعمال_ٿيل->eval_flags;
	evaluate = orig_flags & EVAL_EVALUATE;
	إذا (evaluate)
	{
	    إذا (vim9script)
		نتيجو = tv_get_bool_chk(rettv, &error);
	    وإلا إذا (tv_get_عدد_chk(rettv, &error) != 0)
		نتيجو = TRUE;
	    clear_tv(rettv);
	    إذا (error)
		أرجع FAIL;
	}

	/*
	 * Repeat until there is no following "||".
	 */
	طالما (p[0] == '|' && p[1] == '|')
	{
	    إذا (getnext)
		*arg = eval_ايندڙ_سٽ(*arg, evalarg_استعمال_ٿيل);
	    وإلا
	    {
		إذا (evaluate && vim9script && !VIM_ISWHITE(p[-1]))
		{
		    error_white_both(p, 2);
		    clear_tv(rettv);
		    أرجع FAIL;
		}
		*arg = p;
	    }

	    /*
	     * Get the second variable.
	     */
	    إذا (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[2]))
	    {
		error_white_both(*arg, 2);
		clear_tv(rettv);
		أرجع FAIL;
	    }
	    *arg = skipwhite_and_linebreak(*arg + 2, evalarg_استعمال_ٿيل);
	    evalarg_استعمال_ٿيل->eval_flags = !نتيجو ? orig_flags
					       : (orig_flags & ~EVAL_EVALUATE);
	    إذا (eval3(arg, &var2, evalarg_استعمال_ٿيل) == FAIL)
		أرجع FAIL;

	    /*
	     * Compute the result.
	     */
	    إذا (evaluate && !نتيجو)
	    {
		إذا (vim9script)
		    نتيجو = tv_get_bool_chk(&var2, &error);
		وإلا إذا (tv_get_عدد_chk(&var2, &error) != 0)
		    نتيجو = TRUE;
		clear_tv(&var2);
		إذا (error)
		    أرجع FAIL;
	    }
	    إذا (evaluate)
	    {
		إذا (vim9script)
		{
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_عدد = نتيجو ? VVAL_TRUE : VVAL_FALSE;
		}
		وإلا
		{
		    rettv->v_type = VAR_عدد;
		    rettv->vval.v_عدد = نتيجو;
		}
	    }

	    p = eval_ايندڙ_non_blank(*arg, evalarg_استعمال_ٿيل, &getnext);
	}

	إذا (evalarg == عدم)
	    clear_evalarg(&local_evalarg, عدم);
	وإلا
	    evalarg->eval_flags = orig_flags;
    }

    أرجع OK;
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
    ساكن صحيح
eval3(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    char_u	*p;
    صحيح		getnext;

    /*
     * Get the first expression.
     */
    إذا (eval4(arg, rettv, evalarg) == FAIL)
	أرجع FAIL;

    /*
     * Handle the "&&" operator.
     */
    p = eval_ايندڙ_non_blank(*arg, evalarg, &getnext);
    إذا (p[0] == '&' && p[1] == '&')
    {
	evalarg_T   *evalarg_استعمال_ٿيل = evalarg;
	evalarg_T   local_evalarg;
	صحيح	    orig_flags;
	صحيح	    evaluate;
	ڊگهو	    نتيجو = TRUE;
	typval_T    var2;
	صحيح	    error = FALSE;
	صحيح	    vim9script = اندريون_vim9script();

	إذا (evalarg == عدم)
	{
	    init_evalarg(&local_evalarg);
	    evalarg_استعمال_ٿيل = &local_evalarg;
	}
	orig_flags = evalarg_استعمال_ٿيل->eval_flags;
	evaluate = orig_flags & EVAL_EVALUATE;
	إذا (evaluate)
	{
	    إذا (vim9script)
		نتيجو = tv_get_bool_chk(rettv, &error);
	    وإلا إذا (tv_get_عدد_chk(rettv, &error) == 0)
		نتيجو = FALSE;
	    clear_tv(rettv);
	    إذا (error)
		أرجع FAIL;
	}

	/*
	 * Repeat until there is no following "&&".
	 */
	طالما (p[0] == '&' && p[1] == '&')
	{
	    إذا (getnext)
		*arg = eval_ايندڙ_سٽ(*arg, evalarg_استعمال_ٿيل);
	    وإلا
	    {
		إذا (evaluate && vim9script && !VIM_ISWHITE(p[-1]))
		{
		    error_white_both(p, 2);
		    clear_tv(rettv);
		    أرجع FAIL;
		}
		*arg = p;
	    }

	    /*
	     * Get the second variable.
	     */
	    إذا (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[2]))
	    {
		error_white_both(*arg, 2);
		clear_tv(rettv);
		أرجع FAIL;
	    }
	    *arg = skipwhite_and_linebreak(*arg + 2, evalarg_استعمال_ٿيل);
	    evalarg_استعمال_ٿيل->eval_flags = نتيجو ? orig_flags
					      : (orig_flags & ~EVAL_EVALUATE);
	    CLEAR_ميدان(var2);
	    إذا (eval4(arg, &var2, evalarg_استعمال_ٿيل) == FAIL)
		أرجع FAIL;

	    /*
	     * Compute the result.
	     */
	    إذا (evaluate && نتيجو)
	    {
		إذا (vim9script)
		    نتيجو = tv_get_bool_chk(&var2, &error);
		وإلا إذا (tv_get_عدد_chk(&var2, &error) == 0)
		    نتيجو = FALSE;
		clear_tv(&var2);
		إذا (error)
		    أرجع FAIL;
	    }
	    إذا (evaluate)
	    {
		إذا (vim9script)
		{
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_عدد = نتيجو ? VVAL_TRUE : VVAL_FALSE;
		}
		وإلا
		{
		    rettv->v_type = VAR_عدد;
		    rettv->vval.v_عدد = نتيجو;
		}
	    }

	    p = eval_ايندڙ_non_blank(*arg, evalarg_استعمال_ٿيل, &getnext);
	}

	إذا (evalarg == عدم)
	    clear_evalarg(&local_evalarg, عدم);
	وإلا
	    evalarg->eval_flags = orig_flags;
    }

    أرجع OK;
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
    ساكن صحيح
eval4(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    char_u	*p;
    صحيح		getnext;
    exprtype_T	type = اظهار_UNKNOWN;
    صحيح		len = 2;
    صحيح		type_is = FALSE;

    /*
     * Get the first expression.
     */
    إذا (eval5(arg, rettv, evalarg) == FAIL)
	أرجع FAIL;

    p = eval_ايندڙ_non_blank(*arg, evalarg, &getnext);

    type = get_compare_type(p, &len, &type_is);

    /*
     * If there is a comparative operator, use it.
     */
    إذا (type != اظهار_UNKNOWN)
    {
	typval_T    var2;
	صحيح	    ic;
	صحيح	    vim9script = اندريون_vim9script();
	صحيح	    evaluate = evalarg == عدم
				   ? 0 : (evalarg->eval_flags & EVAL_EVALUATE);
	ڊگهو	    comp_lnum = SOURCING_LNUM;

	إذا (getnext)
	{
	    *arg = eval_ايندڙ_سٽ(*arg, evalarg);
	    p = *arg;
	}
	وإلا إذا (evaluate && vim9script && !VIM_ISWHITE(**arg))
	{
	    error_white_both(*arg, len);
	    clear_tv(rettv);
	    أرجع FAIL;
	}

	إذا (vim9script && type_is && (p[len] == '?' || p[len] == '#'))
	{
	    semsg(_(e_invalid_اظهار_str), p);
	    clear_tv(rettv);
	    أرجع FAIL;
	}

	// extra question mark appended: ignore case
	إذا (p[len] == '?')
	{
	    ic = TRUE;
	    ++len;
	}
	// extra '#' appended: match case
	وإلا إذا (p[len] == '#')
	{
	    ic = FALSE;
	    ++len;
	}
	// nothing appended: use 'ignorecase' if not in Vim script
	وإلا
	    ic = vim9script ? FALSE : p_ic;

	/*
	 * Get the second variable.
	 */
	إذا (evaluate && vim9script && !IS_WHITE_OR_NUL(p[len]))
	{
	    error_white_both(p, len);
	    clear_tv(rettv);
	    أرجع FAIL;
	}
	*arg = skipwhite_and_linebreak(p + len, evalarg);
	إذا (eval5(arg, &var2, evalarg) == FAIL)
	{
	    clear_tv(rettv);
	    أرجع FAIL;
	}
	إذا (evaluate)
	{
	    صحيح ret;

	    // use the line of the comparison for messages
	    SOURCING_LNUM = comp_lnum;
	    إذا (vim9script && check_compare_types(type, rettv, &var2) == FAIL)
	    {
		ret = FAIL;
		clear_tv(rettv);
	    }
	    وإلا
		ret = typval_compare(rettv, &var2, type, ic);
	    clear_tv(&var2);
	    أرجع ret;
	}
    }

    أرجع OK;
}

/*
 * Make a copy of blob "tv1" and append blob "tv2".
 */
    فراغ
eval_addblob(typval_T *tv1, typval_T *tv2)
{
    blob_T  *b1 = tv1->vval.v_blob;
    blob_T  *b2 = tv2->vval.v_blob;
    blob_T  *b = blob_alloc();
    ڊگهو    len1, len2;
    ڊگهو    totallen;

    إذا (b == عدم)
	أرجع;

    len1 = blob_len(b1);
    len2 = blob_len(b2);
    totallen = len1 + len2;

    إذا (totallen > 0 && totallen <= وڌ_کان_وڌ_عدد
				    && ga_grow(&b->bv_ga, (صحيح)totallen) == OK)
    {
	إذا (len1 > 0)
	    mch_memmove((char_u *)b->bv_ga.ga_معلومات,
		    (char_u *)b1->bv_ga.ga_معلومات, (size_t)len1);
	إذا (len2 > 0)
	    mch_memmove((char_u *)b->bv_ga.ga_معلومات + len1,
		    (char_u *)b2->bv_ga.ga_معلومات, (size_t)len2);
	b->bv_ga.ga_len = (صحيح)totallen;
    }

    clear_tv(tv1);
    rettv_blob_سيٽ(tv1, b);
}

/*
 * Make a copy of list "tv1" and append list "tv2".
 */
    صحيح
eval_addlist(typval_T *tv1, typval_T *tv2)
{
    typval_T var3;

    // concatenate Lists
    إذا (list_concat(tv1->vval.v_list, tv2->vval.v_list, &var3) == FAIL)
    {
	clear_tv(tv1);
	clear_tv(tv2);
	أرجع FAIL;
    }
    clear_tv(tv1);
    *tv1 = var3;
    أرجع OK;
}

/*
 * Make a copy of tuple "tv1" and append tuple "tv2".
 */
    صحيح
eval_addtuple(typval_T *tv1, typval_T *tv2)
{
    صحيح		vim9script = اندريون_vim9script();
    typval_T	var3;

    إذا (vim9script && tv1->vval.v_tuple != عدم && tv2->vval.v_tuple != عدم
	    && tv1->vval.v_tuple->tv_type != عدم
	    && tv2->vval.v_tuple->tv_type != عدم)
    {
	إذا (!check_tuples_addable(tv1->vval.v_tuple->tv_type,
						tv2->vval.v_tuple->tv_type))
	    أرجع FAIL;
    }

    // concatenate tuples
    إذا (tuple_concat(tv1->vval.v_tuple, tv2->vval.v_tuple, &var3) == FAIL)
    {
	clear_tv(tv1);
	clear_tv(tv2);
	أرجع FAIL;
    }
    clear_tv(tv1);
    *tv1 = var3;
    أرجع OK;
}

/*
 * Left or right shift the number "tv1" by the number "tv2" and store the
 * result in "tv1".
 *
 * Return OK or FAIL.
 */
    ساكن صحيح
eval_سرڪايو_عدد(typval_T *tv1, typval_T *tv2, صحيح سرڪايو_type)
{
    إذا (tv2->v_type != VAR_عدد || tv2->vval.v_عدد < 0)
    {
	// right operand should be a positive number
	إذا (tv2->v_type != VAR_عدد)
	    emsg(_(e_bitshift_ops_must_be_عدد));
	وإلا
	    emsg(_(e_bitshift_ops_must_be_مثبت));
	clear_tv(tv1);
	clear_tv(tv2);
	أرجع FAIL;
    }

    إذا (tv2->vval.v_عدد > MAX_LSHIFT_بٽ)
	// shifting more bits than we have always results in zero
	tv1->vval.v_عدد = 0;
    وإلا إذا (سرڪايو_type == اظهار_LSHIFT)
	tv1->vval.v_عدد =
	    (uvarnumber_T)tv1->vval.v_عدد << tv2->vval.v_عدد;
    وإلا
	tv1->vval.v_عدد =
	    (uvarnumber_T)tv1->vval.v_عدد >> tv2->vval.v_عدد;

    أرجع OK;
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
    ساكن صحيح
eval5(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    /*
     * Get the first expression.
     */
    إذا (eval6(arg, rettv, evalarg) == FAIL)
	أرجع FAIL;

    /*
     * Repeat computing, until no '<<' or '>>' is following.
     */
    لكل (;;)
    {
	char_u		*p;
	صحيح		getnext;
	exprtype_T	exprtype;
	صحيح		evaluate;
	typval_T	var2;
	صحيح		vim9script;

	p = eval_ايندڙ_non_blank(*arg, evalarg, &getnext);
	إذا (p[0] == '<' && p[1] == '<')
	    exprtype = اظهار_LSHIFT;
	وإلا إذا (p[0] == '>' && p[1] == '>')
	    exprtype = اظهار_RSHIFT;
	وإلا
	    أرجع OK;

	// Handle a bitwise left or right shift operator
	evaluate = evalarg == عدم ? 0 : (evalarg->eval_flags & EVAL_EVALUATE);
	إذا (evaluate && rettv->v_type != VAR_عدد)
	{
	    // left operand should be a number
	    emsg(_(e_bitshift_ops_must_be_عدد));
	    clear_tv(rettv);
	    أرجع FAIL;
	}

	vim9script = اندريون_vim9script();
	إذا (getnext)
	{
	    *arg = eval_ايندڙ_سٽ(*arg, evalarg);
	    p = *arg;
	}
	وإلا إذا (evaluate && vim9script && !VIM_ISWHITE(**arg))
	{
	    error_white_both(*arg, 2);
	    clear_tv(rettv);
	    أرجع FAIL;
	}

	/*
	 * Get the second variable.
	 */
	إذا (evaluate && vim9script && !IS_WHITE_OR_NUL(p[2]))
	{
	    error_white_both(p, 2);
	    clear_tv(rettv);
	    أرجع FAIL;
	}
	*arg = skipwhite_and_linebreak(p + 2, evalarg);
	إذا (eval6(arg, &var2, evalarg) == FAIL)
	{
	    clear_tv(rettv);
	    أرجع FAIL;
	}

	إذا (evaluate)
	{
	    إذا (eval_سرڪايو_عدد(rettv, &var2, exprtype) == FAIL)
		أرجع FAIL;
	}

	clear_tv(&var2);
    }

    أرجع OK;
}

/*
 * Concatenate strings "tv1" and "tv2" and store the result in "tv1".
 */
    ساكن صحيح
eval_concat_str(typval_T *tv1, typval_T *tv2)
{
    char_u	buf1[NUMBUFLEN], buf2[NUMBUFLEN];
    char_u	*s1 = tv_get_string_buf(tv1, buf1);
    char_u	*s2 = عدم;
    char_u	*p;
    صحيح		vim9script = اندريون_vim9script();

    إذا (vim9script && (tv2->v_type == VAR_VOID
		|| tv2->v_type == VAR_CHANNEL
		|| tv2->v_type == VAR_ڪم))
	semsg(_(e_using_invalid_قدر_as_string_str),
		vartype_name(tv2->v_type));
    وإلا إذا (vim9script && tv2->v_type == VAR_FLOAT)
    {
	vim_snprintf((حرف *)buf2, NUMBUFLEN, "%g",
		tv2->vval.v_float);
	s2 = buf2;
    }
    وإلا
	s2 = tv_get_string_buf_chk(tv2, buf2);
    إذا (s2 == عدم)		// type error ?
    {
	clear_tv(tv1);
	clear_tv(tv2);
	أرجع FAIL;
    }

    // When possible, grow the existing string in place to avoid alloc/free.
    إذا (grow_string_tv(tv1, s2) == OK)
	أرجع OK;

    p = concat_str(s1, s2);
    clear_tv(tv1);
    tv1->v_type = VAR_STRING;
    tv1->vval.v_string = p;

    أرجع OK;
}

/*
 * Add or subtract numbers "tv1" and "tv2" and store the result in "tv1".
 * The numbers can be whole numbers or floats.
 */
    ساكن صحيح
eval_addsub_عدد(typval_T *tv1, typval_T *tv2, صحيح op)
{
    صحيح		error = FALSE;
    varnumber_T	n1, n2;
    float_T	f1 = 0, f2 = 0;

    إذا (tv1->v_type == VAR_FLOAT)
    {
	f1 = tv1->vval.v_float;
	n1 = 0;
    }
    وإلا
    {
	n1 = tv_get_عدد_chk(tv1, &error);
	إذا (error)
	{
	    // This can only happen for "list + non-list" or
	    // "blob + non-blob".  For "non-list + ..." or
	    // "something - ...", we returned before evaluating the
	    // 2nd operand.
	    clear_tv(tv1);
	    clear_tv(tv2);
	    أرجع FAIL;
	}
	إذا (tv2->v_type == VAR_FLOAT)
	    f1 = n1;
    }
    إذا (tv2->v_type == VAR_FLOAT)
    {
	f2 = tv2->vval.v_float;
	n2 = 0;
    }
    وإلا
    {
	n2 = tv_get_عدد_chk(tv2, &error);
	إذا (error)
	{
	    clear_tv(tv1);
	    clear_tv(tv2);
	    أرجع FAIL;
	}
	إذا (tv1->v_type == VAR_FLOAT)
	    f2 = n2;
    }
    clear_tv(tv1);

    // If there is a float on either side the result is a float.
    إذا (tv1->v_type == VAR_FLOAT || tv2->v_type == VAR_FLOAT)
    {
	إذا (op == '+')
	    f1 = f1 + f2;
	وإلا
	    f1 = f1 - f2;
	tv1->v_type = VAR_FLOAT;
	tv1->vval.v_float = f1;
    }
    وإلا
    {
	إذا (op == '+')
	    n1 = n1 + n2;
	وإلا
	    n1 = n1 - n2;
	tv1->v_type = VAR_عدد;
	tv1->vval.v_عدد = n1;
    }

    أرجع OK;
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
    ساكن صحيح
eval6(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    /*
     * Get the first expression.
     */
    إذا (eval7(arg, rettv, evalarg, FALSE) == FAIL)
	أرجع FAIL;

    /*
     * Repeat computing, until no '+', '-' or '.' is following.
     */
    لكل (;;)
    {
	صحيح	    evaluate;
	صحيح	    getnext;
	char_u	    *p;
	صحيح	    op;
	صحيح	    oplen;
	صحيح	    concat;
	typval_T    var2;
	صحيح	    vim9script = اندريون_vim9script();
	ڊگهو	    op_lnum = SOURCING_LNUM;

	// "." is only string concatenation when scriptversion is 1
	// "+=", "-=" and "..=" are assignments
	// "++" and "--" on the next line are a separate command.
	p = eval_ايندڙ_non_blank(*arg, evalarg, &getnext);
	op = *p;
	concat = op == '.' && (*(p + 1) == '.' || اندريون_old_script(2));
	إذا ((op != '+' && op != '-' && !concat) || p[1] == '='
					       || (p[1] == '.' && p[2] == '='))
	    اكسر;
	إذا (getnext && (op == '+' || op == '-') && p[0] == p[1])
	    اكسر;

	evaluate = evalarg == عدم ? 0 : (evalarg->eval_flags & EVAL_EVALUATE);
	oplen = (concat && p[1] == '.') ? 2 : 1;
	إذا (getnext)
	    *arg = eval_ايندڙ_سٽ(*arg, evalarg);
	وإلا
	{
	    إذا (evaluate && vim9script && !VIM_ISWHITE(**arg))
	    {
		error_white_both(*arg, oplen);
		clear_tv(rettv);
		أرجع FAIL;
	    }
	    *arg = p;
	}
	إذا ((op != '+' || (rettv->v_type != VAR_LIST
						&& rettv->v_type != VAR_TUPLE
						&& rettv->v_type != VAR_BLOB))
		&& (op == '.' || rettv->v_type != VAR_FLOAT)
		&& evaluate)
	{
	    صحيح		error = FALSE;

	    // For "list + ...", an illegal use of the first operand as
	    // a number cannot be determined before evaluating the 2nd
	    // operand: if this is also a list, all is ok.
	    // For "something . ...", "something - ..." or "non-list + ...",
	    // we know that the first operand needs to be a string or number
	    // without evaluating the 2nd operand.  So check before to avoid
	    // side effects after an error.
	    إذا (op != '.')
		tv_get_عدد_chk(rettv, &error);
	    إذا ((op == '.' && tv_get_string_chk(rettv) == عدم) || error)
	    {
		clear_tv(rettv);
		أرجع FAIL;
	    }
	}

	/*
	 * Get the second variable.
	 */
	إذا (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[oplen]))
	{
	    error_white_both(*arg, oplen);
	    clear_tv(rettv);
	    أرجع FAIL;
	}
	*arg = skipwhite_and_linebreak(*arg + oplen, evalarg);
	إذا (eval7(arg, &var2, evalarg, !vim9script && op == '.') == FAIL)
	{
	    clear_tv(rettv);
	    أرجع FAIL;
	}

	إذا (evaluate)
	{
	    /*
	     * Compute the result.
	     */
	    // use the line of the operation for messages
	    SOURCING_LNUM = op_lnum;
	    إذا (op == '.')
	    {
		إذا (eval_concat_str(rettv, &var2) == FAIL)
		    أرجع FAIL;
	    }
	    وإلا إذا (op == '+' && rettv->v_type == VAR_BLOB
						   && var2.v_type == VAR_BLOB)
		eval_addblob(rettv, &var2);
	    وإلا إذا (op == '+' && rettv->v_type == VAR_LIST
						   && var2.v_type == VAR_LIST)
	    {
		إذا (eval_addlist(rettv, &var2) == FAIL)
		    أرجع FAIL;
	    }
	    وإلا إذا (op == '+' && rettv->v_type == VAR_TUPLE
					   && var2.v_type == VAR_TUPLE)
	    {
		إذا (eval_addtuple(rettv, &var2) == FAIL)
		    أرجع FAIL;
	    }
	    وإلا
	    {
		إذا (eval_addsub_عدد(rettv, &var2, op) == FAIL)
		    أرجع FAIL;
	    }
	    clear_tv(&var2);
	}
    }
    أرجع OK;
}

/*
 * Multiply or divide or compute the modulo of numbers "tv1" and "tv2" and
 * store the result in "tv1".  The numbers can be whole numbers or floats.
 */
    ساكن صحيح
eval_multdiv_عدد(typval_T *tv1, typval_T *tv2, صحيح op)
{
    varnumber_T	n1, n2;
    float_T	f1, f2;
    صحيح		error;
    صحيح		use_float = FALSE;

    f1 = 0;
    f2 = 0;
    error = FALSE;
    إذا (tv1->v_type == VAR_FLOAT)
    {
	f1 = tv1->vval.v_float;
	use_float = TRUE;
	n1 = 0;
    }
    وإلا
	n1 = tv_get_عدد_chk(tv1, &error);
    clear_tv(tv1);
    إذا (error)
    {
	clear_tv(tv2);
	أرجع FAIL;
    }

    إذا (tv2->v_type == VAR_FLOAT)
    {
	إذا (!use_float)
	{
	    f1 = n1;
	    use_float = TRUE;
	}
	f2 = tv2->vval.v_float;
	n2 = 0;
    }
    وإلا
    {
	n2 = tv_get_عدد_chk(tv2, &error);
	clear_tv(tv2);
	إذا (error)
	    أرجع FAIL;
	إذا (use_float)
	    f2 = n2;
    }

    /*
     * Compute the result.
     * When either side is a float the result is a float.
     */
    إذا (use_float)
    {
	إذا (op == '*')
	    f1 = f1 * f2;
	وإلا إذا (op == '/')
	{
#جيڪڏهن_وصف_ٿيل VMS
	    // VMS crashes on divide by zero, work around it
	    إذا (f2 == 0.0)
	    {
		إذا (f1 == 0)
		    f1 = -1 * __F_FLT_MAX - 1L;   // similar to NaN
		وإلا إذا (f1 < 0)
		    f1 = -1 * __F_FLT_MAX;
		وإلا
		    f1 = __F_FLT_MAX;
	    }
	    وإلا
		f1 = f1 / f2;
#وإلا
	    // We rely on the floating point library to handle divide
	    // by zero to result in "inf" and not a crash.
	    f1 = f1 / f2;
#جيڪڏهن_پورو
	}
	وإلا
	{
	    emsg(_(e_cannot_use_percent_with_float));
	    أرجع FAIL;
	}
	tv1->v_type = VAR_FLOAT;
	tv1->vval.v_float = f1;
    }
    وإلا
    {
	صحيح	    failed = FALSE;

	إذا (op == '*')
	    n1 = n1 * n2;
	وإلا إذا (op == '/')
	    n1 = انگ_divide(n1, n2, &failed);
	وإلا
	    n1 = انگ_modulus(n1, n2, &failed);
	إذا (failed)
	    أرجع FAIL;

	tv1->v_type = VAR_عدد;
	tv1->vval.v_عدد = n1;
    }

    أرجع OK;
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
    ساكن صحيح
eval7(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    صحيح		want_string)  // after "." operator
{
    /*
     * Get the first expression.
     */
    إذا (eval8(arg, rettv, evalarg, want_string) == FAIL)
	أرجع FAIL;

    /*
     * Repeat computing, until no '*', '/' or '%' is following.
     */
    لكل (;;)
    {
	صحيح	    evaluate;
	صحيح	    getnext;
	typval_T    var2;
	char_u	    *p;
	صحيح	    op;

	// "*=", "/=" and "%=" are assignments
	p = eval_ايندڙ_non_blank(*arg, evalarg, &getnext);
	op = *p;
	إذا ((op != '*' && op != '/' && op != '%') || p[1] == '=')
	    اكسر;

	evaluate = evalarg == عدم ? 0 : (evalarg->eval_flags & EVAL_EVALUATE);
	إذا (getnext)
	    *arg = eval_ايندڙ_سٽ(*arg, evalarg);
	وإلا
	{
	    إذا (evaluate && اندريون_vim9script() && !VIM_ISWHITE(**arg))
	    {
		error_white_both(*arg, 1);
		clear_tv(rettv);
		أرجع FAIL;
	    }
	    *arg = p;
	}

	/*
	 * Get the second variable.
	 */
	إذا (evaluate && اندريون_vim9script() && !IS_WHITE_OR_NUL((*arg)[1]))
	{
	    error_white_both(*arg, 1);
	    clear_tv(rettv);
	    أرجع FAIL;
	}
	*arg = skipwhite_and_linebreak(*arg + 1, evalarg);
	إذا (eval8(arg, &var2, evalarg, FALSE) == FAIL)
	    أرجع FAIL;

	إذا (evaluate)
	    // Compute the result.
	    إذا (eval_multdiv_عدد(rettv, &var2, op) == FAIL)
		أرجع FAIL;
    }

    أرجع OK;
}

/*
 * Handle seventh level expression:
 *	a type cast before a base level expression.
 * "arg" must point to the first non-white of the expression.
 * "arg" is advanced to just after the recognized expression.
 * Return OK or FAIL.
 */
    ساكن صحيح
eval8(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    صحيح		want_string)	// after "." operator
{
    type_T	*want_type = عدم;
    garray_T	type_list;	    // list of pointers to allocated types
    صحيح		res;
    صحيح		evaluate = evalarg == عدم ? 0
				       : (evalarg->eval_flags & EVAL_EVALUATE);

    // Recognize <type> in Vim9 script only.
    إذا (اندريون_vim9script() && **arg == '<' && eval_isnamec1((*arg)[1])
					     && STRNCMP(*arg, "<SNR>", 5) != 0)
    {
	++*arg;
	ga_init2(&type_list, حجم(type_T *), 10);
	إذا (!evaluate)
	    // Only skip over the type, it may use a type variable that is only
	    // known when the expression is compiled, e.g. in a lambda.
	    *arg = skip_type(*arg, FALSE);
	وإلا
	{
	    want_type = parse_type(arg, &type_list, عدم, عدم, TRUE);
	    إذا (want_type == عدم)
	    {
		clear_type_list(&type_list);
		أرجع FAIL;
	    }
	}

	إذا (**arg != '>')
	{
	    إذا (*skipwhite(*arg) == '>')
		semsg(_(e_no_white_space_allowed_before_str_str), ">", *arg);
	    وإلا
		emsg(_(e_missing_gt));
	    clear_type_list(&type_list);
	    أرجع FAIL;
	}
	++*arg;
	*arg = skipwhite_and_linebreak(*arg, evalarg);
    }

    res = eval9(arg, rettv, evalarg, want_string);

    إذا (want_type != عدم)
    {
	إذا (evaluate && res == OK)
	{
	    type_T *actual = typval2type(rettv, get_copyID(), &type_list,
							       TVTT_DO_MEMBER);

	    إذا (!equal_type(want_type, actual, 0))
	    {
		إذا (want_type->tt_type == VAR_BOOL
					&& actual->tt_type != VAR_BOOL
					&& (actual->tt_flags & TTFLAG_BOOL_OK))
		{
		    صحيح n = tv2bool(rettv);

		    // can use "0" and "1" for boolean in some places
		    clear_tv(rettv);
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_عدد = n ? VVAL_TRUE : VVAL_FALSE;
		}
		وإلا
		{
		    where_T where = WHERE_INIT;

		    res = check_type(want_type, actual, TRUE, where);
		}
	    }
	}
	clear_type_list(&type_list);
    }

    أرجع res;
}

    صحيح
eval_leader(char_u **arg, صحيح vim9)
{
    char_u	*s = *arg;
    char_u	*p = *arg;

    طالما (*p == '!' || *p == '-' || *p == '+')
    {
	char_u *n = skipwhite(p + 1);

	// ++, --, -+ and +- are not accepted in Vim9 script
	إذا (vim9 && (*p == '-' || *p == '+') && (*n == '-' || *n == '+'))
	{
	    semsg(_(e_invalid_اظهار_str), s);
	    أرجع FAIL;
	}
	p = n;
    }
    *arg = p;
    أرجع OK;
}

/*
 * Check for a predefined value "true", "false" and "null.*".
 * Return OK when recognized.
 */
    صحيح
handle_predefined(char_u *s, صحيح len, typval_T *rettv)
{
    بدل (len)
    {
	حالة 4: إذا (STRNCMP(s, "true", 4) == 0)
		{
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_عدد = VVAL_TRUE;
		    أرجع OK;
		}
		إذا (STRNCMP(s, "null", 4) == 0)
		{
		    rettv->v_type = VAR_SPECIAL;
		    rettv->vval.v_عدد = VVAL_NULL;
		    أرجع OK;
		}
		اكسر;
	حالة 5: إذا (STRNCMP(s, "false", 5) == 0)
		{
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_عدد = VVAL_FALSE;
		    أرجع OK;
		}
		اكسر;
	حالة 8: إذا (STRNCMP(s, "null_job", 8) == 0)
		{
#جيڪڏهن_وصف_ٿيل FEAT_ڪم_CHANNEL
		    rettv->v_type = VAR_ڪم;
		    rettv->vval.v_ڪم = عدم;
#وإلا
		    rettv->v_type = VAR_SPECIAL;
		    rettv->vval.v_عدد = VVAL_NULL;
#جيڪڏهن_پورو
		    أرجع OK;
		}
		اكسر;
	حالة 9:
		إذا (STRNCMP(s, "null_", 5) != 0)
		    اكسر;
		// null_list
		إذا (STRNCMP(s + 5, "list", 4) == 0)
		{
		    rettv->v_type = VAR_LIST;
		    rettv->vval.v_list = عدم;
		    أرجع OK;
		}
		// null_dict
		إذا (STRNCMP(s + 5, "dict", 4) == 0)
		{
		    rettv->v_type = VAR_DICT;
		    rettv->vval.v_dict = عدم;
		    أرجع OK;
		}
		// null_blob
		إذا (STRNCMP(s + 5, "blob", 4) == 0)
		{
		    rettv->v_type = VAR_BLOB;
		    rettv->vval.v_blob = عدم;
		    أرجع OK;
		}
		اكسر;
	حالة 10:
		إذا (STRNCMP(s, "null_", 5) != 0)
		    اكسر;
		// null_class
		إذا (STRNCMP(s + 5, "class", 5) == 0)
		{
		    rettv->v_type = VAR_CLASS;
		    rettv->vval.v_class = عدم;
		    أرجع OK;
		}
		إذا (STRNCMP(s + 5, "tuple", 5) == 0)
		{
		    rettv->v_type = VAR_TUPLE;
		    rettv->vval.v_tuple = عدم;
		    أرجع OK;
		}
		اكسر;
	حالة 11: إذا (STRNCMP(s, "null_string", 11) == 0)
		{
		    rettv->v_type = VAR_STRING;
		    rettv->vval.v_string = عدم;
		    أرجع OK;
		}
		إذا (STRNCMP(s, "null_object", 11) == 0)
		{
		    rettv->v_type = VAR_OBJECT;
		    rettv->vval.v_object = عدم;
		    أرجع OK;
		}
		اكسر;
	حالة 12:
		إذا (STRNCMP(s, "null_channel", 12) == 0)
		{
#جيڪڏهن_وصف_ٿيل FEAT_ڪم_CHANNEL
		    rettv->v_type = VAR_CHANNEL;
		    rettv->vval.v_channel = عدم;
#وإلا
		    rettv->v_type = VAR_SPECIAL;
		    rettv->vval.v_عدد = VVAL_NULL;
#جيڪڏهن_پورو
		    أرجع OK;
		}
		إذا (STRNCMP(s, "null_partial", 12) == 0)
		{
		    rettv->v_type = VAR_PARTIAL;
		    rettv->vval.v_partial = عدم;
		    أرجع OK;
		}
		اكسر;
	حالة 13: إذا (STRNCMP(s, "null_function", 13) == 0)
		{
		    rettv->v_type = VAR_FUNC;
		    rettv->vval.v_string = عدم;
		    أرجع OK;
		}
		اكسر;
    }
    أرجع FAIL;
}

/*
 * Handle register contents: @r.
 */
    ساكن فراغ
eval9_reg_contents(
    char_u	**arg,
    typval_T	*rettv,
    صحيح		evaluate)
{
    صحيح		vim9script = اندريون_vim9script();

    ++*arg;	// skip '@'
    إذا (evaluate)
    {
	إذا (vim9script && IS_WHITE_OR_NUL(**arg))
	    semsg(_(e_syntax_error_at_str), *arg);
	وإلا إذا (vim9script && !valid_yank_reg(**arg, FALSE))
	    emsg_invreg(**arg);
	وإلا
	{
	    rettv->v_type = VAR_STRING;
	    rettv->vval.v_string = get_reg_contents(**arg,
		    GREG_اظهار_SRC);
	}
    }
    إذا (**arg != NUL)
	++*arg;
}

/*
 * Handle a nested expression: (expression) or lambda: (arg) => expr
 */
    ساكن صحيح
eval9_nested_اظهار(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    صحيح		evaluate)
{
    صحيح		ret = NOTDONE;
    صحيح		vim9script = اندريون_vim9script();

    إذا (vim9script)
    {
	ret = get_lambda_tv(arg, rettv, TRUE, evalarg, عدم);
	إذا (ret == OK && evaluate)
	{
	    ufunc_T *ufunc = rettv->vval.v_partial->pt_func;

	    // Compile it here to get the return type.  The return
	    // type is optional, when it's missing use t_unknown.
	    // This is recognized in compile_return().
	    إذا (ufunc->uf_ret_type->tt_type == VAR_VOID)
		ufunc->uf_ret_type = &t_unknown;
	    إذا (compile_def_function(ufunc, FALSE,
				get_compile_type(ufunc), عدم) == FAIL)
	    {
		clear_tv(rettv);
		ret = FAIL;
	    }
	}
    }
    إذا (ret == NOTDONE)
    {
	*arg = skipwhite_and_linebreak(*arg + 1, evalarg);
	إذا (**arg == ')')
	    // empty tuple
	    ret = eval_tuple(arg, rettv, evalarg, TRUE);
	وإلا
	{
	    ret = eval1(arg, rettv, evalarg);	// recursive!
	    إذا (ret != OK)
		أرجع ret;

	    *arg = skipwhite_and_linebreak(*arg, evalarg);

	    إذا (**arg == ',')
		// tuple
		ret = eval_tuple(arg, rettv, evalarg, TRUE);
	    وإلا إذا (**arg == ')')
		++*arg;
	    وإلا إذا (ret == OK)
	    {
		emsg(_(e_missing_closing_paren));
		clear_tv(rettv);
		ret = FAIL;
	    }
	}
    }

    أرجع ret;
}

/*
* Handle be a variable or function name.
* Can also be a curly-braces kind of name: {expr}.
*/
    ساكن صحيح
eval9_var_func_name(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    صحيح		evaluate,
    char_u	**name_شروعات)
{
    char_u	*s;
    صحيح		len;
    char_u	*alias;
    صحيح		ret = OK;
    صحيح		vim9script = اندريون_vim9script();

    s = *arg;
    len = get_name_len(arg, &alias, evaluate, TRUE);
    إذا (alias != عدم)
	s = alias;

    إذا (len <= 0)
	ret = FAIL;
    وإلا
    {
	صحيح	flags = evalarg == عدم ? 0 : evalarg->eval_flags;

	إذا (evaluate && vim9script && len == 1 && *s == '_')
	{
	    emsg(_(e_cannot_use_underscore_هتي));
	    ret = FAIL;
	}
	وإلا إذا (evaluate && vim9script && len > 2
						&& s[0] == 's' && s[1] == ':')
	{
	    semsg(_(e_cannot_use_s_colon_اندريون_vim9_script_str), s);
	    ret = FAIL;
	}
	وإلا إذا ((vim9script ? **arg : *skipwhite(*arg)) == '('
				|| (vim9script && generic_func_call(arg)))
	{
	    // "name(..."  recursive!
	    *arg = skipwhite(*arg);
	    ret = eval_func(arg, evalarg, s, len, rettv, flags, عدم);
	}
	وإلا إذا (evaluate)
	{
	    // get the value of "true", "false", etc. or a variable
	    ret = FAIL;
	    إذا (vim9script)
		ret = handle_predefined(s, len, rettv);
	    إذا (ret == FAIL)
	    {
		*name_شروعات = s;
		ret = eval_variable(s, len, 0, rettv, عدم,
					EVAL_VAR_VERBOSE + EVAL_VAR_IMPORT);

		// skip the generic function arguments (if present)
		// they are already processed by eval_variable
		إذا (ret == OK && vim9script && **arg == '<'
						&& rettv->v_type == VAR_FUNC)
		    ret = skip_generic_func_type_دليلون(arg);
	    }
	}
	وإلا
	{
	    // skip the name
	    check_vars(s, len);

	    // Skip the type arguments of a generic function, its name starts
	    // with an uppercase letter.
	    إذا (vim9script && **arg == '<' && ASCII_ISUPPER(*s))
		skip_generic_func_type_دليلون(arg);
	    ret = OK;
	}
    }
    vim_free(alias);

    أرجع ret;
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
    ساكن صحيح
eval9(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    صحيح		want_string)	// after "." operator
{
    صحيح		evaluate = evalarg != عدم
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    char_u	*name_شروعات = عدم;
    char_u	*شروعات_leader, *پڇاڙي_leader;
    صحيح		ret = OK;
    ساكن صحيح	recurse = 0;
    صحيح		vim9script = اندريون_vim9script();

    /*
     * Initialise variable so that clear_tv() can't mistake this for a
     * string and free a string that isn't there.
     */
    rettv->v_type = VAR_UNKNOWN;

    /*
     * Skip '!', '-' and '+' characters.  They are handled later.
     */
    شروعات_leader = *arg;
    إذا (eval_leader(arg, vim9script) == FAIL)
	أرجع FAIL;
    پڇاڙي_leader = *arg;

    إذا (**arg == '.' && (!محفوظ_isdigit(*(*arg + 1)) || اندريون_old_script(2)))
    {
	semsg(_(e_invalid_اظهار_str), *arg);
	++*arg;
	أرجع FAIL;
    }

    // Limit recursion to 1000 levels.  At least at 10000 we run out of stack
    // and crash.  With MSVC the stack is smaller.
    إذا (recurse ==
#جيڪڏهن_وصف_ٿيل _MSC_VER
		    300
#وإلا
		    1000
#جيڪڏهن_پورو
		    )
    {
	semsg(_(e_اظهار_too_recursive_str), *arg);
	أرجع FAIL;
    }
    ++recurse;

    بدل (**arg)
    {
    /*
     * Number constant.
     */
    حالة '0':
    حالة '1':
    حالة '2':
    حالة '3':
    حالة '4':
    حالة '5':
    حالة '6':
    حالة '7':
    حالة '8':
    حالة '9':
    حالة '.':	ret = eval_عدد(arg, rettv, evaluate, want_string);

		// Apply prefixed "-" and "+" now.  Matters especially when
		// "->" follows.
		إذا (ret == OK && evaluate && پڇاڙي_leader > شروعات_leader
						  && rettv->v_type != VAR_BLOB)
		    ret = eval9_leader(rettv, TRUE, شروعات_leader, &پڇاڙي_leader);
		اكسر;

    /*
     * String constant: "string".
     */
    حالة '"':	ret = eval_string(arg, rettv, evaluate, FALSE);
		اكسر;

    /*
     * Literal string constant: 'str''ing'.
     */
    حالة '\'':	ret = eval_lit_string(arg, rettv, evaluate, FALSE);
		اكسر;

    /*
     * List: [expr, expr]
     */
    حالة '[':	ret = eval_list(arg, rettv, evalarg, TRUE);
		اكسر;

    /*
     * Literal Dictionary: #{key: val, key: val}
     */
    حالة '#':	ret = eval_lit_dict(arg, rettv, evalarg);
		اكسر;

    /*
     * Lambda: {arg, arg -> expr}
     * Dictionary: {'key': val, 'key': val}
     */
    حالة '{':	إذا (vim9script)
		    ret = NOTDONE;
		وإلا
		    ret = get_lambda_tv(arg, rettv, vim9script, evalarg, عدم);
		إذا (ret == NOTDONE)
		    ret = eval_dict(arg, rettv, evalarg, FALSE);
		اكسر;

    /*
     * Option value: &name
     */
    حالة '&':	ret = eval_option(arg, rettv, evaluate);
		اكسر;

    /*
     * Environment variable: $VAR.
     * Interpolated string: $"string" or $'string'.
     */
    حالة '$':	إذا ((*arg)[1] == '"' || (*arg)[1] == '\'')
		    ret = eval_interp_string(arg, rettv, evaluate);
		وإلا
		    ret = eval_env_var(arg, rettv, evaluate);
		اكسر;

    /*
     * Register contents: @r.
     */
    حالة '@':	eval9_reg_contents(arg, rettv, evaluate);
		اكسر;

    /*
     * nested expression: (expression).
     * or lambda: (arg) => expr
     * or tuple
     */
    حالة '(':	ret = eval9_nested_اظهار(arg, rettv, evalarg, evaluate);
		اكسر;

    افتراضي:	ret = NOTDONE;
		اكسر;
    }

    إذا (ret == NOTDONE)
    {
	/*
	 * Must be a variable or function name.
	 * Can also be a curly-braces kind of name: {expr}.
	 */
	ret = eval9_var_func_name(arg, rettv, evalarg, evaluate, &name_شروعات);
    }

    // Handle following '[', '(' and '.' for expr[expr], expr.name,
    // expr(expr), expr->name(expr)
    إذا (ret == OK)
	ret = handle_subscript(arg, name_شروعات, rettv, evalarg, evaluate);

    /*
     * Apply logical NOT and unary '-', from right to left, ignore '+'.
     */
    إذا (ret == OK && evaluate && پڇاڙي_leader > شروعات_leader)
	ret = eval9_leader(rettv, FALSE, شروعات_leader, &پڇاڙي_leader);

    --recurse;
    أرجع ret;
}

/*
 * Apply the leading "!" and "-" before an eval9 expression to "rettv".
 * When "numeric_only" is TRUE only handle "+" and "-".
 * Adjusts "end_leaderp" until it is at "start_leader".
 */
    ساكن صحيح
eval9_leader(
	typval_T    *rettv,
	صحيح	    numeric_only,
	char_u	    *شروعات_leader,
	char_u	    **پڇاڙي_leaderp)
{
    char_u	*پڇاڙي_leader = *پڇاڙي_leaderp;
    صحيح		ret = OK;
    صحيح		error = FALSE;
    varnumber_T val = 0;
    vartype_T	type = rettv->v_type;
    صحيح		vim9script = اندريون_vim9script();
    float_T	    f = 0.0;

    إذا (rettv->v_type == VAR_FLOAT)
	f = rettv->vval.v_float;
    وإلا
    {
	طالما (VIM_ISWHITE(پڇاڙي_leader[-1]))
	    --پڇاڙي_leader;
	إذا (vim9script && پڇاڙي_leader[-1] == '!')
	{
	    // Is this typeval supported with the ! operator?
	    إذا (check_typval_is_قدر(rettv) == FAIL)
	    {
		clear_tv(rettv);
		أرجع FAIL;
	    }
	    val = tv2bool(rettv);
	}
	وإلا
	    val = tv_get_عدد_chk(rettv, &error);
    }
    إذا (error)
    {
	clear_tv(rettv);
	ret = FAIL;
    }
    وإلا
    {
	طالما (پڇاڙي_leader > شروعات_leader)
	{
	    --پڇاڙي_leader;
	    إذا (*پڇاڙي_leader == '!')
	    {
		إذا (numeric_only)
		{
		    ++پڇاڙي_leader;
		    اكسر;
		}
		إذا (rettv->v_type == VAR_FLOAT)
		{
		    rettv->v_type = VAR_BOOL;
		    val = f == 0.0 ? VVAL_TRUE : VVAL_FALSE;
		    type = VAR_BOOL;
		}
		وإلا
		{
		    val = !val;
		    type = VAR_BOOL;
		}
	    }
	    وإلا إذا (*پڇاڙي_leader == '-')
	    {
		إذا (rettv->v_type == VAR_FLOAT)
		    f = -f;
		وإلا
		{
		    val = -val;
		    type = VAR_عدد;
		}
	    }
	}
	إذا (rettv->v_type == VAR_FLOAT)
	{
	    clear_tv(rettv);
	    rettv->vval.v_float = f;
	}
	وإلا
	{
	    clear_tv(rettv);
	    إذا (vim9script)
		rettv->v_type = type;
	    وإلا
		rettv->v_type = VAR_عدد;
	    rettv->vval.v_عدد = val;
	}
    }
    *پڇاڙي_leaderp = پڇاڙي_leader;
    أرجع ret;
}

/*
 * Call the function referred to in "rettv".
 */
    ساكن صحيح
call_func_rettv(
	char_u	    **arg,
	evalarg_T   *evalarg,
	typval_T    *rettv,
	صحيح	    evaluate,
	dict_T	    *selfdict,
	typval_T    *basetv)
{
    partial_T	*pt = عدم;
    funcexe_T	funcexe;
    typval_T	functv;
    char_u	*s;
    صحيح		ret;

    // need to copy the funcref so that we can clear rettv
    إذا (evaluate)
    {
	functv = *rettv;
	rettv->v_type = VAR_UNKNOWN;

	// Invoke the function.  Recursive!
	إذا (functv.v_type == VAR_PARTIAL)
	{
	    pt = functv.vval.v_partial;
	    s = partial_name(pt);
	}
	وإلا
	{
	    s = functv.vval.v_string;
	    إذا (s == عدم || *s == NUL)
	    {
		emsg(_(e_خالي_جايون_function_name));
		ret = FAIL;
		اذهب theend;
	    }
	}
    }
    وإلا
	s = (char_u *)"";

    CLEAR_ميدان(funcexe);
    funcexe.fe_firstline = curwin->w_cursor.lnum;
    funcexe.fe_lastline = curwin->w_cursor.lnum;
    funcexe.fe_evaluate = evaluate;
    funcexe.fe_partial = pt;
    إذا (pt != عدم)
    {
	funcexe.fe_object = pt->pt_obj;
	إذا (funcexe.fe_object != عدم)
	    ++funcexe.fe_object->obj_refcount;
    }
    funcexe.fe_selfdict = selfdict;
    funcexe.fe_basetv = basetv;
    إذا (evalarg != عدم)
	funcexe.fe_cctx = evalarg->eval_cctx;
    ret = get_func_tv(s, -1, rettv, arg, evalarg, &funcexe);

theend:
    // Clear the funcref afterwards, so that deleting it while
    // evaluating the arguments is possible (see test55).
    إذا (evaluate)
	clear_tv(&functv);

    أرجع ret;
}

/*
 * Evaluate "->method()".
 * "*arg" points to "method".
 * Returns FAIL or OK. "*arg" is advanced to after the ')'.
 */
    ساكن صحيح
eval_lambda(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    صحيح		verbose)	// give error messages
{
    صحيح		evaluate = evalarg != عدم
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    typval_T	بنياد = *rettv;
    صحيح		ret;

    rettv->v_type = VAR_UNKNOWN;

    إذا (**arg == '{')
    {
	// ->{lambda}()
	ret = get_lambda_tv(arg, rettv, FALSE, evalarg, عدم);
    }
    وإلا
    {
	// ->(lambda)()
	++*arg;
	ret = eval1(arg, rettv, evalarg);
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	إذا (**arg != ')')
	{
	    emsg(_(e_missing_closing_paren));
	    أرجع FAIL;
	}
	إذا (rettv->v_type != VAR_STRING && rettv->v_type != VAR_FUNC
					       && rettv->v_type != VAR_PARTIAL)
	{
	    emsg(_(e_string_or_function_required_for_arrow_parens_اظهار));
	    أرجع FAIL;
	}
	++*arg;
    }
    إذا (ret != OK)
	أرجع FAIL;

    إذا (**arg != '(')
    {
	إذا (verbose)
	{
	    إذا (*skipwhite(*arg) == '(')
		emsg(_(e_no_white_space_allowed_before_parenthesis));
	    وإلا
		semsg(_(e_missing_parenthesis_str), "lambda");
	}
	clear_tv(rettv);
	ret = FAIL;
    }
    وإلا
	ret = call_func_rettv(arg, evalarg, rettv, evaluate, عدم, &بنياد);

    // Clear the funcref afterwards, so that deleting it while
    // evaluating the arguments is possible (see test55).
    إذا (evaluate)
	clear_tv(&بنياد);

    أرجع ret;
}

/*
 * Evaluate "->method()".
 * "*arg" points to "method".
 * Returns FAIL or OK. "*arg" is advanced to after the ')'.
 */
    ساكن صحيح
eval_طريقو(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    صحيح		verbose)	// give error messages
{
    char_u	*name;
    ڊگهو	len;
    char_u	*alias;
    char_u	*tofree = عدم;
    typval_T	بنياد = *rettv;
    صحيح		ret = OK;
    صحيح		evaluate = evalarg != عدم
				      && (evalarg->eval_flags & EVAL_EVALUATE);

    rettv->v_type = VAR_UNKNOWN;

    name = *arg;
    len = get_name_len(arg, &alias, evaluate, evaluate);
    إذا (alias != عدم)
	name = alias;

    إذا (len <= 0)
    {
	إذا (verbose)
	    emsg(_(e_missing_name_after_طريقو));
	ret = FAIL;
    }
    وإلا
    {
	char_u *paren;

	// If there is no "(" immediately following, but there is further on,
	// it can be "import.Func()", "dict.Func()", "list[nr]", etc.
	// Does not handle anything where "(" is part of the expression.
	*arg = skipwhite(*arg);

	إذا (**arg != '(' && alias == عدم
				    && (paren = vim_strchr(*arg, '(')) != عدم)
	{
	    *arg = name;

	    // Truncate the name at the "(".  Avoid trying to get another line
	    // by making "getline" NULL.
	    *paren = NUL;
	    char_u	*(*getline)(صحيح, فراغ *, صحيح, getline_opt_T) = عدم;
	    إذا (evalarg != عدم)
	    {
		getline = evalarg->eval_getline;
		evalarg->eval_getline = عدم;
	    }

	    char_u *deref = deref_function_name(arg, &tofree, evalarg, verbose);
	    إذا (deref == عدم)
	    {
		*arg = name + len;
		ret = FAIL;
	    }
	    وإلا
	    {
		name = deref;
		len = (ڊگهو)STRLEN(name);
	    }

	    *paren = '(';
	    إذا (getline != عدم)
		evalarg->eval_getline = getline;
	}

	إذا (ret == OK)
	{
	    *arg = skipwhite(*arg);

	    إذا (**arg != '(')
	    {
		إذا (verbose)
		    semsg(_(e_missing_parenthesis_str), name);
		ret = FAIL;
	    }
	    وإلا إذا (VIM_ISWHITE((*arg)[-1]))
	    {
		إذا (verbose)
		    emsg(_(e_no_white_space_allowed_before_parenthesis));
		ret = FAIL;
	    }
	    وإلا
		ret = eval_func(arg, evalarg, name, len, rettv,
					  evaluate ? EVAL_EVALUATE : 0, &بنياد);
	}
    }

    // Clear the funcref afterwards, so that deleting it while
    // evaluating the arguments is possible (see test55).
    إذا (evaluate)
	clear_tv(&بنياد);
    vim_free(tofree);

    إذا (alias != عدم)
	vim_free(alias);

    أرجع ret;
}

/*
 * Evaluate an "[expr]" or "[expr:expr]" index.  Also "dict.key".
 * "*arg" points to the '[' or '.'.
 * Returns FAIL or OK. "*arg" is advanced to after the ']'.
 */
    ساكن صحيح
eval_اشاريو(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    صحيح		verbose)	// give error messages
{
    صحيح		evaluate = evalarg != عدم
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    صحيح		empty1 = FALSE, empty2 = FALSE;
    typval_T	var1, var2;
    صحيح		range = FALSE;
    char_u	*ڪنجي = عدم;
    صحيح		keylen = -1;
    صحيح		vim9script = اندريون_vim9script();

    إذا (check_can_اشاريو(rettv, evaluate, verbose) == FAIL)
	أرجع FAIL;

    init_tv(&var1);
    init_tv(&var2);
    إذا (**arg == '.')
    {
	/*
	 * dict.name
	 */
	ڪنجي = *arg + 1;
	لكل (keylen = 0; eval_isdictc(ڪنجي[keylen]); ++keylen)
	    ;
	إذا (keylen == 0)
	    أرجع FAIL;
	إذا (vim9script && ڪنجي[keylen] == '<')
	{
	    // skip generic type arguments
	    char_u	*p = &ڪنجي[keylen];

	    إذا (skip_generic_func_type_دليلون(&p) == FAIL)
		أرجع FAIL;
	    keylen = p - ڪنجي;
	}
	*arg = ڪنجي + keylen;
    }
    وإلا
    {
	/*
	 * something[idx]
	 *
	 * Get the (first) variable from inside the [].
	 */
	*arg = skipwhite_and_linebreak(*arg + 1, evalarg);
	إذا (**arg == ':')
	    empty1 = TRUE;
	وإلا إذا (eval1(arg, &var1, evalarg) == FAIL)	// recursive!
	    أرجع FAIL;
	وإلا إذا (vim9script && **arg == ':')
	{
	    semsg(_(e_white_space_required_before_and_after_str_at_str),
								    ":", *arg);
	    clear_tv(&var1);
	    أرجع FAIL;
	}
	وإلا إذا (evaluate)
	{
	    صحيح error = FALSE;

	    // allow for indexing with float
	    إذا (vim9script && rettv->v_type == VAR_DICT
						   && var1.v_type == VAR_FLOAT)
	    {
		var1.vval.v_string = typval_tostring(&var1, TRUE);
		var1.v_type = VAR_STRING;
	    }

	    إذا (vim9script && (rettv->v_type == VAR_LIST
						|| rettv->v_type == VAR_TUPLE))
		tv_get_عدد_chk(&var1, &error);
	    وإلا
		error = tv_get_string_chk(&var1) == عدم;
	    إذا (error)
	    {
		// not a number or string
		clear_tv(&var1);
		أرجع FAIL;
	    }
	}

	/*
	 * Get the second variable from inside the [:].
	 */
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	إذا (**arg == ':')
	{
	    range = TRUE;
	    ++*arg;
	    إذا (vim9script && !IS_WHITE_OR_NUL(**arg) && **arg != ']')
	    {
		semsg(_(e_white_space_required_before_and_after_str_at_str),
								":", *arg - 1);
		إذا (!empty1)
		    clear_tv(&var1);
		أرجع FAIL;
	    }
	    *arg = skipwhite_and_linebreak(*arg, evalarg);
	    إذا (**arg == ']')
		empty2 = TRUE;
	    وإلا إذا (eval1(arg, &var2, evalarg) == FAIL)	// recursive!
	    {
		إذا (!empty1)
		    clear_tv(&var1);
		أرجع FAIL;
	    }
	    وإلا إذا (evaluate && tv_get_string_chk(&var2) == عدم)
	    {
		// not a number or string
		إذا (!empty1)
		    clear_tv(&var1);
		clear_tv(&var2);
		أرجع FAIL;
	    }
	}

	// Check for the ']'.
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	إذا (**arg != ']')
	{
	    إذا (verbose)
		emsg(_(e_missing_closing_چورس_brace));
	    clear_tv(&var1);
	    إذا (range)
		clear_tv(&var2);
	    أرجع FAIL;
	}
	*arg = *arg + 1;	// skip over the ']'
    }

    إذا (evaluate)
    {
	صحيح res = eval_اشاريو_inner(rettv, range,
		empty1 ? عدم : &var1, empty2 ? عدم : &var2, FALSE,
		ڪنجي, keylen, verbose);

	إذا (!empty1)
	    clear_tv(&var1);
	إذا (range)
	    clear_tv(&var2);
	أرجع res;
    }
    أرجع OK;
}

/*
 * Check if "rettv" can have an [index] or [sli:ce]
 */
    صحيح
check_can_اشاريو(typval_T *rettv, صحيح evaluate, صحيح verbose)
{
    بدل (rettv->v_type)
    {
	حالة VAR_FUNC:
	حالة VAR_PARTIAL:
	    إذا (verbose)
		emsg(_(e_cannot_اشاريو_a_funcref));
	    أرجع FAIL;
	حالة VAR_FLOAT:
	    إذا (verbose)
		emsg(_(e_using_float_as_string));
	    أرجع FAIL;
	حالة VAR_BOOL:
	حالة VAR_SPECIAL:
	حالة VAR_ڪم:
	حالة VAR_CHANNEL:
	حالة VAR_INSTR:
	حالة VAR_OBJECT:
	    إذا (verbose)
		emsg(_(e_cannot_اشاريو_special_variable));
	    أرجع FAIL;
	حالة VAR_CLASS:
	حالة VAR_TYPEALIAS:
	    إذا (verbose)
		check_typval_is_قدر(rettv);
	    أرجع FAIL;
	حالة VAR_UNKNOWN:
	حالة VAR_ANY:
	حالة VAR_VOID:
	    إذا (evaluate)
	    {
		emsg(_(e_cannot_اشاريو_special_variable));
		أرجع FAIL;
	    }
	    // FALLTHROUGH

	حالة VAR_STRING:
	حالة VAR_LIST:
	حالة VAR_TUPLE:
	حالة VAR_DICT:
	حالة VAR_BLOB:
	    اكسر;
	حالة VAR_عدد:
	    إذا (اندريون_vim9script())
		emsg(_(e_cannot_اشاريو_عدد));
	    اكسر;
    }
    أرجع OK;
}

/*
 * Apply index or range to "rettv".
 * "var1" is the first index, NULL for [:expr].
 * "var2" is the second index, NULL for [expr] and [expr: ]
 * "exclusive" is TRUE for slice(): second index is exclusive, use character
 * index for string.
 * Alternatively, "key" is not NULL, then key[keylen] is the dict index.
 */
    صحيح
eval_اشاريو_inner(
	typval_T    *rettv,
	صحيح	    is_range,
	typval_T    *var1,
	typval_T    *var2,
	صحيح	    exclusive,
	char_u	    *ڪنجي,
	صحيح	    keylen,
	صحيح	    verbose)
{
    varnumber_T	    n1, n2 = 0;
    ڊگهو	    len;

    n1 = 0;
    إذا (var1 != عدم && rettv->v_type != VAR_DICT)
	n1 = tv_get_عدد(var1);

    إذا (is_range)
    {
	إذا (rettv->v_type == VAR_DICT)
	{
	    إذا (verbose)
		emsg(_(e_cannot_ٽڪرو_dictionary));
	    أرجع FAIL;
	}
	إذا (var2 != عدم)
	    n2 = tv_get_عدد(var2);
	وإلا
	    n2 = VARNUM_MAX;
    }

    بدل (rettv->v_type)
    {
	حالة VAR_UNKNOWN:
	حالة VAR_ANY:
	حالة VAR_VOID:
	حالة VAR_FUNC:
	حالة VAR_PARTIAL:
	حالة VAR_FLOAT:
	حالة VAR_BOOL:
	حالة VAR_SPECIAL:
	حالة VAR_ڪم:
	حالة VAR_CHANNEL:
	حالة VAR_INSTR:
	حالة VAR_CLASS:
	حالة VAR_OBJECT:
	حالة VAR_TYPEALIAS:
	    اكسر; // not evaluating, skipping over subscript

	حالة VAR_عدد:
	حالة VAR_STRING:
	    {
		char_u	*s = tv_get_string(rettv);

		len = (ڊگهو)STRLEN(s);
		إذا (اندريون_vim9script() || exclusive)
		{
		    إذا (is_range)
			s = string_ٽڪرو(s, n1, n2, exclusive);
		    وإلا
			s = char_from_string(s, n1);
		}
		وإلا إذا (is_range)
		{
		    // The resulting variable is a substring.  If the indexes
		    // are out of range the result is empty.
		    إذا (n1 < 0)
		    {
			n1 = len + n1;
			إذا (n1 < 0)
			    n1 = 0;
		    }
		    إذا (n2 < 0)
			n2 = len + n2;
		    وإلا إذا (n2 >= len)
			n2 = len;
		    إذا (n1 >= len || n2 < 0 || n1 > n2)
			s = عدم;
		    وإلا
			s = vim_strnsave(s + n1, n2 - n1 + 1);
		}
		وإلا
		{
		    // The resulting variable is a string of a single
		    // character.  If the index is too big or negative the
		    // result is empty.
		    إذا (n1 >= len || n1 < 0)
			s = عدم;
		    وإلا
			s = vim_strnsave(s + n1, 1);
		}
		clear_tv(rettv);
		rettv->v_type = VAR_STRING;
		rettv->vval.v_string = s;
	    }
	    اكسر;

	حالة VAR_BLOB:
	    blob_ٽڪرو_or_اشاريو(rettv->vval.v_blob, is_range, n1, n2,
							     exclusive, rettv);
	    اكسر;

	حالة VAR_LIST:
	    إذا (var1 == عدم)
		n1 = 0;
	    إذا (var2 == عدم)
		n2 = VARNUM_MAX;
	    إذا (list_ٽڪرو_or_اشاريو(rettv->vval.v_list,
			  is_range, n1, n2, exclusive, rettv, verbose) == FAIL)
		أرجع FAIL;
	    اكسر;

	حالة VAR_TUPLE:
	    إذا (var1 == عدم)
		n1 = 0;
	    إذا (var2 == عدم)
		n2 = VARNUM_MAX;
	    إذا (tuple_ٽڪرو_or_اشاريو(rettv->vval.v_tuple,
			  is_range, n1, n2, exclusive, rettv, verbose) == FAIL)
		أرجع FAIL;
	    اكسر;

	حالة VAR_DICT:
	    {
		dictitem_T	*شي;
		typval_T	tmp;

		إذا (ڪنجي == عدم)
		{
		    ڪنجي = tv_get_string_chk(var1);
		    إذا (ڪنجي == عدم)
			أرجع FAIL;
		}

		شي = dict_ڳوليو(rettv->vval.v_dict, ڪنجي, keylen);

		إذا (شي == عدم)
		{
		    إذا (verbose)
		    {
			إذا (keylen > 0)
			    ڪنجي[keylen] = NUL;
			semsg(_(e_ڪنجي_not_present_اندريون_dictionary_str), ڪنجي);
		    }
		    أرجع FAIL;
		}

		copy_tv(&شي->di_tv, &tmp);
		clear_tv(rettv);
		*rettv = tmp;
	    }
	    اكسر;
    }
    أرجع OK;
}

/*
 * Return the function name of partial "pt".
 */
    char_u *
partial_name(partial_T *pt)
{
    إذا (pt != عدم)
    {
	إذا (pt->pt_name != عدم)
	    أرجع pt->pt_name;
	إذا (pt->pt_func != عدم)
	    أرجع pt->pt_func->uf_name;
    }
    أرجع (char_u *)"";
}

    ساكن فراغ
partial_free(partial_T *pt)
{
    صحيح i;

    لكل (i = 0; i < pt->pt_argc; ++i)
	clear_tv(&pt->pt_argv[i]);
    vim_free(pt->pt_argv);
    dict_unref(pt->pt_dict);
    إذا (pt->pt_name != عدم)
    {
	func_unref(pt->pt_name);
	vim_free(pt->pt_name);
    }
    وإلا
	func_ptr_unref(pt->pt_func);
    إذا (pt->pt_obj != عدم)
	object_unref(pt->pt_obj);

    // "out_up" is no longer used, decrement refcount on partial that owns it.
    partial_unref(pt->pt_outer.ٻاهريون_مٿي_partial);

    // Using pt_outer from another partial.
    partial_unref(pt->pt_outer_partial);

    // Decrease the reference count for the context of a closure.  If down
    // to the minimum it may be time to free it.
    إذا (pt->pt_funcstack != عدم)
    {
	--pt->pt_funcstack->fs_refcount;
	funcstack_check_refcount(pt->pt_funcstack);
    }
    // Similarly for loop variables.
    لكل (i = 0; i < MAX_LOOP_اونهائي; ++i)
	إذا (pt->pt_loopvars[i] != عدم)
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
    فراغ
partial_unref(partial_T *pt)
{
    إذا (pt == عدم)
	أرجع;

    صحيح	پورو = FALSE;

    إذا (--pt->pt_refcount <= 0)
	partial_free(pt);

    // If the reference count goes down to one, the funcstack may be the
    // only reference and can be freed if no other partials reference it.
    وإلا إذا (pt->pt_refcount == 1)
    {
	// careful: if the funcstack is freed it may contain this partial
	// and it gets freed as well
	إذا (pt->pt_funcstack != عدم)
	    پورو = funcstack_check_refcount(pt->pt_funcstack);

	إذا (!پورو)
	{
	    صحيح	اونهائي;

	    لكل (اونهائي = 0; اونهائي < MAX_LOOP_اونهائي; ++اونهائي)
		إذا (pt->pt_loopvars[اونهائي] != عدم
			&& loopvars_check_refcount(pt->pt_loopvars[اونهائي]))
		    اكسر;
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
    ساكن char_u *
string_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    صحيح		echo_style,
    صحيح		مرکب_val)
{
    char_u	*r = عدم;

    إذا (echo_style && !مرکب_val)
    {
	*tofree = عدم;
	r = tv->vval.v_string;
	إذا (r == عدم)
	    r = (char_u *)"";
    }
    وإلا
    {
	*tofree = string_quote(tv->vval.v_string, FALSE);
	r = *tofree;
    }

    أرجع r;
}

/*
 * Return a textual representation of a function in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "echo_style" is FALSE, put quotes around the function name as
 * "function()", otherwise does not put quotes around function name.
 * May return NULL.
 */
    ساكن char_u *
func_tv2string(typval_T *tv, char_u **tofree, صحيح echo_style)
{
    char_u	*r = عدم;
    char_u	buf[MAX_FUNC_NAME_LEN];

    إذا (echo_style)
    {
	*tofree = عدم;

	إذا (tv->vval.v_string == عدم)
	    r = (char_u *)"function()";
	وإلا
	{
	    r = make_ufunc_name_readable(tv->vval.v_string, buf,
							MAX_FUNC_NAME_LEN);
	    إذا (r == buf)
		r = *tofree = vim_strsave(buf);
	}
    }
    وإلا
    {
	char_u *s = عدم;

	إذا (tv->vval.v_string != عدم)
	    s = make_ufunc_name_readable(tv->vval.v_string, buf,
							MAX_FUNC_NAME_LEN);

	r = *tofree = string_quote(s, TRUE);
    }

    أرجع r;
}

/*
 * Return a textual representation of the object method in "tv", a VAR_PARTIAL.
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "echo_style" is FALSE, put quotes around the function name as
 * "function()", otherwise does not put quotes around function name.
 * May return NULL.
 */
    ساكن char_u *
طريقو_tv2string(typval_T *tv, char_u **tofree, صحيح echo_style)
{
    char_u	buf[MAX_FUNC_NAME_LEN];
    partial_T	*pt = tv->vval.v_partial;

    size_t len = vim_snprintf((حرف *)buf, حجم(buf), "<SNR>%d_%s.%s",
			   pt->pt_func->uf_script_ctx.sc_sid,
			   pt->pt_func->uf_class->class_name.string,
			   pt->pt_func->uf_name);
    إذا (len >= حجم(buf))
    {
	إذا (echo_style)
	{
	    *tofree = عدم;
	    أرجع (char_u *)"function()";
	}
	وإلا
	    أرجع *tofree = string_quote((char_u*)"", TRUE);
    }

    أرجع *tofree = echo_style ? vim_strsave(buf) : string_quote(buf, TRUE);
}

/*
 * Return a textual representation of a partial in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * "numbuf" is used for a number.  May return NULL.
 */
    ساكن char_u *
partial_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    char_u	*numbuf,
    صحيح		copyID)
{
    char_u	*r = عدم;
    partial_T	*pt;
    char_u	*fname;
    garray_T	ga;
    صحيح		i;
    char_u	*tf;

    pt = tv->vval.v_partial;
    fname = string_quote(pt == عدم ? عدم : partial_name(pt), FALSE);

    ga_init2(&ga, 1, 100);
    GA_CONCAT_LITERAL(&ga, "function(");
    إذا (fname != عدم)
    {
	// When using uf_name prepend "g:" for a global function.
	إذا (pt != عدم && pt->pt_name == عدم && fname[0] == '\''
						&& vim_isupper(fname[1]))
	{
	    GA_CONCAT_LITERAL(&ga, "'g:");
	    ga_concat(&ga, fname + 1);
	}
	وإلا
	    ga_concat(&ga, fname);
	vim_free(fname);
    }
    إذا (pt != عدم && pt->pt_argc > 0)
    {
	GA_CONCAT_LITERAL(&ga, ", [");
	لكل (i = 0; i < pt->pt_argc; ++i)
	{
	    إذا (i > 0)
		GA_CONCAT_LITERAL(&ga, ", ");
	    ga_concat(&ga, tv2string(&pt->pt_argv[i], &tf, numbuf, copyID));
	    vim_free(tf);
	}
	GA_CONCAT_LITERAL(&ga, "]");
    }
    إذا (pt != عدم && pt->pt_dict != عدم)
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

    *tofree = ga.ga_معلومات;
    r = *tofree;

    أرجع r;
}

/*
 * Return a textual representation of a List in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "copyID" is not zero replace recursive lists with "...".  When
 * "restore_copyID" is FALSE, repeated items in lists are replaced with "...".
 * May return NULL.
 */
    ساكن char_u *
list_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    صحيح		copyID,
    صحيح		restore_copyID)
{
    char_u	*r = عدم;

    إذا (tv->vval.v_list == عدم)
    {
	// NULL list is equivalent to empty list.
	*tofree = عدم;
	r = (char_u *)"[]";
    }
    وإلا إذا (copyID != 0 && tv->vval.v_list->lv_copyID == copyID
	    && tv->vval.v_list->lv_len > 0)
    {
	*tofree = عدم;
	r = (char_u *)"[...]";
    }
    وإلا
    {
	صحيح old_copyID;
	إذا (restore_copyID)
	    old_copyID = tv->vval.v_list->lv_copyID;

	tv->vval.v_list->lv_copyID = copyID;
	*tofree = list2string(tv, copyID, restore_copyID);
	إذا (restore_copyID)
	    tv->vval.v_list->lv_copyID = old_copyID;
	r = *tofree;
    }

    أرجع r;
}

/*
 * Return a textual representation of a Tuple in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "copyID" is not zero replace recursive lists with "...".  When
 * "restore_copyID" is FALSE, repeated items in tuples are replaced with "...".
 * May return NULL.
 */
    ساكن char_u *
tuple_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    صحيح		copyID,
    صحيح		restore_copyID)
{
    tuple_T	*tuple = tv->vval.v_tuple;
    char_u	*r = عدم;

    إذا (tuple == عدم)
    {
	// NULL tuple is equivalent to an empty tuple.
	*tofree = عدم;
	r = (char_u *)"()";
    }
    وإلا إذا (copyID != 0 && tuple->tv_copyID == copyID
					&& tuple->tv_شيون.ga_len > 0)
    {
	*tofree = عدم;
	r = (char_u *)"(...)";
    }
    وإلا
    {
	صحيح old_copyID;
	إذا (restore_copyID)
	    old_copyID = tuple->tv_copyID;

	tuple->tv_copyID = copyID;
	*tofree = tuple2string(tv, copyID, restore_copyID);
	إذا (restore_copyID)
	    tuple->tv_copyID = old_copyID;
	r = *tofree;
    }

    أرجع r;
}

/*
 * Return a textual representation of a Dict in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "copyID" is not zero replace recursive dicts with "...".
 * When "restore_copyID" is FALSE, repeated items in the dictionary are
 * replaced with "...".  May return NULL.
 */
    ساكن char_u *
dict_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    صحيح		copyID,
    صحيح		restore_copyID)
{
    char_u	*r = عدم;

    إذا (tv->vval.v_dict == عدم)
    {
	// NULL dict is equivalent to empty dict.
	*tofree = عدم;
	r = (char_u *)"{}";
    }
    وإلا إذا (copyID != 0 && tv->vval.v_dict->dv_copyID == copyID
	    && tv->vval.v_dict->dv_hashtab.ht_استعمال_ٿيل != 0)
    {
	*tofree = عدم;
	r = (char_u *)"{...}";
    }
    وإلا
    {
	صحيح old_copyID;
	إذا (restore_copyID)
	    old_copyID = tv->vval.v_dict->dv_copyID;

	tv->vval.v_dict->dv_copyID = copyID;
	*tofree = dict2string(tv, copyID, restore_copyID);
	إذا (restore_copyID)
	    tv->vval.v_dict->dv_copyID = old_copyID;
	r = *tofree;
    }

    أرجع r;
}

/*
 * Return a textual representation of a job or a channel in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * "numbuf" is used for a number.
 * When "composite_val" is FALSE, put quotes around strings as "string()",
 * otherwise does not put quotes around strings.
 * May return NULL.
 */
    ساكن char_u *
jobchan_tv2string(
    typval_T	*tv اڻ_استعمال_ٿيل,
    char_u	**tofree اڻ_استعمال_ٿيل,
    char_u	*numbuf اڻ_استعمال_ٿيل,
    صحيح		مرکب_val اڻ_استعمال_ٿيل)
{
    char_u	*r = عدم;

#جيڪڏهن_وصف_ٿيل FEAT_ڪم_CHANNEL
    *tofree = عدم;

    إذا (tv->v_type == VAR_ڪم)
	r = ڪم_منزل_string_buf(tv, numbuf);
    وإلا
	r = channel_منزل_string_buf(tv, numbuf);

    إذا (مرکب_val)
    {
	*tofree = string_quote(r, FALSE);
	r = *tofree;
    }
#جيڪڏهن_پورو

    أرجع r;
}

/*
 * Return a textual representation of a class in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * May return NULL.
 */
    ساكن char_u *
class_tv2string(typval_T *tv, char_u **tofree)
{
    char_u	*r = عدم;
    size_t	rsize;
    class_T	*cl = tv->vval.v_class;
    string_T	class_name = {(char_u *)"[unknown]", 9};
    string_T	s = {(char_u *)"class", 5};

    إذا (cl != عدم)
    {
	class_name.string = cl->class_name.string;
	class_name.ڊيگهائي = cl->class_name.ڊيگهائي;
	إذا (IS_INTERFACE(cl))
	    STR_LITERAL_سيٽ(s, "interface");
	وإلا إذا (IS_ENUM(cl))
	    STR_LITERAL_سيٽ(s, "enum");
    }

    rsize = s.ڊيگهائي + 1 + class_name.ڊيگهائي + 1;
    r = *tofree = alloc(rsize);
    إذا (r != عدم)
	vim_snprintf((حرف *)r, rsize, "%s %s", s.string, (حرف *)class_name.string);

    أرجع r;
}

/*
 * Return a textual representation of an Object in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "copyID" is not zero replace recursive object with "...".
 * When "restore_copyID" is FALSE, repeated items in the object are
 * replaced with "...".  May return NULL.
 */
    ساكن char_u *
object_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    صحيح		copyID,
    صحيح		restore_copyID,
    char_u	*numbuf,
    صحيح		echo_style,
    صحيح		مرکب_val)
{
    char_u	*r = عدم;

    object_T	*obj = tv->vval.v_object;
    إذا (obj == عدم || obj->obj_class == عدم)
    {
	*tofree = عدم;
	r = (char_u *)"object of [unknown]";
    }
    وإلا إذا (copyID != 0 && obj->obj_copyID == copyID
	    && obj->obj_class->class_obj_member_ڳڻپ != 0)
    {
	size_t n = 25 + obj->obj_class->class_name.ڊيگهائي;
	r = alloc(n);
	إذا (r != عدم)
	    (فراغ)vim_snprintf((حرف *)r, n, "object of %s {...}",
						obj->obj_class->class_name.string);
	*tofree = r;
    }
    وإلا
    {
	صحيح old_copyID;
	إذا (restore_copyID)
	    old_copyID = obj->obj_copyID;

	obj->obj_copyID = copyID;
	*tofree = object2string(obj, numbuf, copyID, echo_style,
				restore_copyID, مرکب_val);
	إذا (restore_copyID)
	    obj->obj_copyID = old_copyID;
	r = *tofree;
    }

    أرجع r;
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
    صحيح		copyID,
    صحيح		echo_style,
    صحيح		restore_copyID,
    صحيح		مرکب_val)
{
    ساكن صحيح	recurse = 0;
    char_u	*r = عدم;

    إذا (recurse >= DICT_MAXNEST)
    {
	إذا (!did_echo_string_emsg)
	{
	    // Only give this message once for a recursive call to avoid
	    // flooding the user with errors.  And stop iterating over lists
	    // and dicts and objects.
	    did_echo_string_emsg = TRUE;
	    emsg(_(e_variable_nested_too_deep_for_displaying));
	}
	*tofree = عدم;
	أرجع (char_u *)"{E724}";
    }
    ++recurse;

    بدل (tv->v_type)
    {
	حالة VAR_STRING:
	    r = string_tv2string(tv, tofree, echo_style, مرکب_val);
	    اكسر;

	حالة VAR_FUNC:
	    r = func_tv2string(tv, tofree, echo_style);
	    اكسر;

	حالة VAR_PARTIAL:
	    إذا (tv->vval.v_partial == عدم
		    || tv->vval.v_partial->pt_obj == عدم)
		r = partial_tv2string(tv, tofree, numbuf, copyID);
	    وإلا
		r = طريقو_tv2string(tv, tofree, echo_style);
	    اكسر;

	حالة VAR_BLOB:
	    r = blob2string(tv->vval.v_blob, tofree, numbuf);
	    اكسر;

	حالة VAR_LIST:
	    r = list_tv2string(tv, tofree, copyID, restore_copyID);
	    اكسر;

	حالة VAR_TUPLE:
	    r = tuple_tv2string(tv, tofree, copyID, restore_copyID);
	    اكسر;

	حالة VAR_DICT:
	    r = dict_tv2string(tv, tofree, copyID, restore_copyID);
	    اكسر;

	حالة VAR_عدد:
	حالة VAR_UNKNOWN:
	حالة VAR_ANY:
	حالة VAR_VOID:
	    *tofree = عدم;
	    r = tv_get_string_buf(tv, numbuf);
	    اكسر;

	حالة VAR_ڪم:
	حالة VAR_CHANNEL:
	    r = jobchan_tv2string(tv, tofree, numbuf, مرکب_val);
	    اكسر;

	حالة VAR_INSTR:
	    *tofree = عدم;
	    r = (char_u *)"instructions";
	    اكسر;

	حالة VAR_CLASS:
	    r = class_tv2string(tv, tofree);
	    اكسر;

	حالة VAR_OBJECT:
	    r = object_tv2string(tv, tofree, copyID, restore_copyID,
					 numbuf, echo_style, مرکب_val);
	    اكسر;

	حالة VAR_FLOAT:
	    *tofree = عدم;
	    vim_snprintf((حرف *)numbuf, NUMBUFLEN, "%g", tv->vval.v_float);
	    r = numbuf;
	    اكسر;

	حالة VAR_BOOL:
	حالة VAR_SPECIAL:
	    *tofree = عدم;
	    r = (char_u *)get_var_special_name(tv->vval.v_عدد);
	    اكسر;

	حالة VAR_TYPEALIAS:
	    *tofree = vim_strsave(tv->vval.v_typealias->ta_name);
	    r = *tofree;
	    إذا (r == عدم)
		r = (char_u *)"";
	    اكسر;
    }

    إذا (--recurse == 0)
	did_echo_string_emsg = FALSE;
    أرجع r;
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
    صحيح		copyID)
{
    أرجع echo_string_core(tv, tofree, numbuf, copyID, TRUE, FALSE, FALSE);
}

/*
 * Convert the specified byte index of line 'lnum' in buffer 'buf' to a
 * character index.  Works only for loaded buffers. Returns -1 on failure.
 * The index of the first byte and the first character is zero.
 */
    صحيح
buf_byteidx_منزل_charidx(buf_T *buf, صحيح lnum, صحيح byteidx)
{
    char_u	*str;
    char_u	*t;
    صحيح		ڳڻپ;

    إذا (buf == عدم || buf->b_ml.ml_mfp == عدم)
	أرجع -1;

    إذا (lnum > buf->b_ml.ml_سٽ_ڳڻپ)
	lnum = buf->b_ml.ml_سٽ_ڳڻپ;

    str = ml_get_buf(buf, lnum, FALSE);
    إذا (str == عدم)
	أرجع -1;

    إذا (*str == NUL)
	أرجع 0;

    // count the number of characters
    t = str;
    لكل (ڳڻپ = 0; *t != NUL && t <= str + byteidx; ڳڻپ++)
	t += mb_ptr2len(t);

    // In insert mode, when the cursor is at the end of a non-empty line,
    // byteidx points to the NUL character immediately past the end of the
    // string. In this case, add one to the character count.
    إذا (*t == NUL && byteidx != 0 && t == str + byteidx)
	ڳڻپ++;

    أرجع ڳڻپ - 1;
}

/*
 * Convert the specified character index of line 'lnum' in buffer 'buf' to a
 * byte index.  Works only for loaded buffers. Returns -1 on failure.
 * The index of the first byte and the first character is zero.
 */
    صحيح
buf_charidx_منزل_byteidx(buf_T *buf, صحيح lnum, صحيح charidx)
{
    char_u	*str;
    char_u	*t;

    إذا (buf == عدم || buf->b_ml.ml_mfp == عدم)
	أرجع -1;

    إذا (lnum > buf->b_ml.ml_سٽ_ڳڻپ)
	lnum = buf->b_ml.ml_سٽ_ڳڻپ;

    str = ml_get_buf(buf, lnum, FALSE);
    إذا (str == عدم)
	أرجع -1;

    // Convert the character offset to a byte offset
    t = str;
    طالما (*t != NUL && --charidx > 0)
	t += mb_ptr2len(t);

    أرجع t - str;
}

/*
 * Translate a String variable into a position.
 * Returns NULL when there is an error.
 */
    pos_T *
var2fpos(
    typval_T	*varp,
    صحيح		dollar_lnum,	// TRUE when $ is last line
    صحيح		*fnum,		// set to fnum for '0, 'A, etc.
    صحيح		charcol)	// return character column
{
    char_u		*name;
    ساكن pos_T	pos;
    pos_T		*pp;
    صحيح			error = FALSE;

    // Argument can be [lnum, col, coladd].
    إذا (varp->v_type == VAR_LIST)
    {
	list_T		*l;
	صحيح		len;
	listitem_T	*li;

	l = varp->vval.v_list;
	إذا (l == عدم)
	    أرجع عدم;

	// Get the line number
	pos.lnum = list_ڳوليو_nr(l, 0L, &error);
	إذا (error || pos.lnum <= 0 || pos.lnum > curbuf->b_ml.ml_سٽ_ڳڻپ)
	    أرجع عدم;	// invalid line number
	إذا (charcol)
	    len = (ڊگهو)mb_charlen(ml_get(pos.lnum));
	وإلا
	    len = (ڊگهو)ml_get_len(pos.lnum);

	// Get the column number
	// We accept "$" for the column number: last column.
	li = list_ڳوليو(l, 1L);
	إذا (li != عدم && li->li_tv.v_type == VAR_STRING
		&& li->li_tv.vval.v_string != عدم
		&& STRCMP(li->li_tv.vval.v_string, "$") == 0)
	{
	    pos.کانو = len + 1;
	}
	وإلا
	{
	    pos.کانو = list_ڳوليو_nr(l, 1L, &error);
	    إذا (error)
		أرجع عدم;
	}

	// Accept a position up to the NUL after the line.
	إذا (pos.کانو == 0 || (صحيح)pos.کانو > len + 1)
	    أرجع عدم;	// invalid column number
	--pos.کانو;

	// Get the virtual offset.  Defaults to zero.
	pos.coladd = list_ڳوليو_nr(l, 2L, &error);
	إذا (error)
	    pos.coladd = 0;

	أرجع &pos;
    }

    إذا (اندريون_vim9script() && check_for_string_arg(varp, 0) == FAIL)
	أرجع عدم;

    name = tv_get_string_chk(varp);
    إذا (name == عدم)
	أرجع عدم;

    error = TRUE;
    pos.lnum = 0;
    إذا (name[0] == '.')
    {
	إذا (!اندريون_vim9script() || name[1] == NUL)
	{
	    error = FALSE;
	    // cursor
	    pos = curwin->w_cursor;
	}
    }
    وإلا إذا (name[0] == 'v' && name[1] == NUL)
    {
	// Visual start
	إذا (VIsual_سرگرم)
	    pos = VIsual;
	وإلا
	    pos = curwin->w_cursor;
    }
    وإلا إذا (name[0] == '\'')
    {
	إذا (!اندريون_vim9script() || (name[1] != NUL && name[2] == NUL))
	{
	    error = FALSE;
	    // mark
	    pp = getmark_buf_fnum(curbuf, name[1], FALSE, fnum);
	    إذا (pp == عدم || pp == (pos_T *)-1 || pp->lnum <= 0)
		أرجع عدم;
	    pos = *pp;
	}
    }
    إذا (pos.lnum != 0)
    {
	إذا (charcol)
	    pos.کانو = buf_byteidx_منزل_charidx(curbuf, pos.lnum, pos.کانو);
	أرجع &pos;
    }

    pos.coladd = 0;

    إذا (name[0] == 'w' && dollar_lnum)
    {
	// the "w_valid" flags are not reset when moving the cursor, but they
	// do matter for update_topline() and validate_botline().
	check_cursor_moved(curwin);

	pos.کانو = 0;
	إذا (name[1] == '0')		// "w0": first visible line
	{
#جيڪڏهن_وصف_ٿيل FEAT_TERMINAL
	    إذا (bt_terminal(curwin->w_بفر)
				    && curwin->w_بفر->b_رڪن != عدم
				    && !رڪن_اندريون_عام_گهڻائي(curwin->w_بفر))
		may_move_terminal_منزل_بفر(curwin->w_بفر->b_رڪن, TRUE);
#جيڪڏهن_پورو
	    update_topline();
	    // In silent Ex mode topline is zero, but that's not a valid line
	    // number; use one instead.
	    pos.lnum = curwin->w_topline > 0 ? curwin->w_topline : 1;
	    أرجع &pos;
	}
	وإلا إذا (name[1] == '$')	// "w$": last visible line
	{
#جيڪڏهن_وصف_ٿيل FEAT_TERMINAL
	    إذا (bt_terminal(curwin->w_بفر)
				    && curwin->w_بفر->b_رڪن != عدم
				    && !رڪن_اندريون_عام_گهڻائي(curwin->w_بفر))
		may_move_terminal_منزل_بفر(curwin->w_بفر->b_رڪن, TRUE);
#جيڪڏهن_پورو
	    validate_botline();
	    // In silent Ex mode botline is zero, return zero then.
	    pos.lnum = curwin->w_botline > 0 ? curwin->w_botline - 1 : 0;
	    أرجع &pos;
	}
    }
    وإلا إذا (name[0] == '$')		// last column or line
    {
	إذا (dollar_lnum)
	{
	    pos.lnum = curbuf->b_ml.ml_سٽ_ڳڻپ;
	    pos.کانو = 0;
	}
	وإلا
	{
	    pos.lnum = curwin->w_cursor.lnum;
	    إذا (charcol)
		pos.کانو = (colnr_T)mb_charlen(ml_get_curline());
	    وإلا
		pos.کانو = ml_get_curline_len();
	}
	أرجع &pos;
    }
    إذا (اندريون_vim9script() && error)
	semsg(_(e_invalid_قدر_for_سٽ_عدد_str), name);
    أرجع عدم;
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
    صحيح
list2fpos(
    typval_T	*arg,
    pos_T	*posp,
    صحيح		*fnump,
    colnr_T	*curswantp,
    صحيح		charcol)
{
    list_T	*l = arg->vval.v_list;
    ڊگهو	i = 0;
    ڊگهو	n;

    // List must be: [fnum, lnum, col, coladd, curswant], where "fnum" is only
    // there when "fnump" isn't NULL; "coladd" and "curswant" are optional.
    إذا (arg->v_type != VAR_LIST
	    || l == عدم
	    || l->lv_len < (fnump == عدم ? 2 : 3)
	    || l->lv_len > (fnump == عدم ? 4 : 5))
	أرجع FAIL;

    إذا (fnump != عدم)
    {
	n = list_ڳوليو_nr(l, i++, عدم);	// fnum
	إذا (n < 0)
	    أرجع FAIL;
	إذا (n == 0)
	    n = curbuf->b_fnum;		// current buffer
	*fnump = n;
    }

    n = list_ڳوليو_nr(l, i++, عدم);	// lnum
    إذا (n < 0)
	أرجع FAIL;
    posp->lnum = n;

    n = list_ڳوليو_nr(l, i++, عدم);	//کانو
    إذا (n < 0)
	أرجع FAIL;
    // If character position is specified, then convert to byte position
    // If the line number is zero use the cursor line.
    إذا (charcol)
    {
	buf_T	*buf;

	// Get the text for the specified line in a loaded buffer
	buf = buflist_findnr(fnump == عدم ? curbuf->b_fnum : *fnump);
	إذا (buf == عدم || buf->b_ml.ml_mfp == عدم)
	    أرجع FAIL;

	n = buf_charidx_منزل_byteidx(buf,
		  posp->lnum == 0 ? curwin->w_cursor.lnum : posp->lnum, n) + 1;
    }
    posp->کانو = n;

    n = list_ڳوليو_nr(l, i, عدم);	// off
    إذا (n < 0)
	posp->coladd = 0;
    وإلا
	posp->coladd = n;

    إذا (curswantp != عدم)
	*curswantp = list_ڳوليو_nr(l, i + 1, عدم);  // curswant

    أرجع OK;
}

/*
 * Get the length of an environment variable name.
 * Advance "arg" to the first character after the name.
 * Return 0 for error.
 */
    صحيح
get_env_len(char_u **arg)
{
    char_u	*p;
    صحيح		len;

    لكل (p = *arg; vim_isIDc(*p); ++p)
	;
    إذا (p == *arg)	    // no name found
	أرجع 0;

    len = (صحيح)(p - *arg);
    *arg = p;
    أرجع len;
}

/*
 * Get the length of the name of a function or internal variable.
 * "arg" is advanced to after the name.
 * Return 0 if something is wrong.
 */
    صحيح
get_id_len(char_u **arg)
{
    char_u	*p;
    صحيح		len;

    // Find the end of the name.
    لكل (p = *arg; eval_isnamec(*p); ++p)
    {
	إذا (*p == ':')
	{
	    // "s:" is start of "s:var", but "n:" is not and can be used in
	    // slice "[n:]".  Also "xx:" is not a namespace.
	    len = (صحيح)(p - *arg);
	    إذا ((len == 1 && vim_strchr(NAMESPACE_CHAR, **arg) == عدم)
		    || len > 1)
		اكسر;
	}
    }
    إذا (p == *arg)	    // no name found
	أرجع 0;

    len = (صحيح)(p - *arg);
    *arg = p;

    أرجع len;
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
    صحيح
get_name_len(
    char_u	**arg,
    char_u	**alias,
    صحيح		evaluate,
    صحيح		verbose)
{
    صحيح		len;
    char_u	*p;
    char_u	*اظهار_شروعات;
    char_u	*اظهار_پڇاڙي;

    *alias = عدم;  // default to no alias

    إذا ((*arg)[0] == K_SPECIAL && (*arg)[1] == KS_EXTRA
						  && (*arg)[2] == (صحيح)KE_SNR)
    {
	// hard coded <SNR>, already translated
	*arg += 3;
	أرجع get_id_len(arg) + 3;
    }
    len = eval_fname_script(*arg);
    إذا (len > 0)
    {
	// literal "<SID>", "s:" or "<SNR>"
	*arg += len;
    }

    /*
     * Find the end of the name; check for {} construction.
     */
    p = ڳوليو_name_پڇاڙي(*arg, &اظهار_شروعات, &اظهار_پڇاڙي,
					       len > 0 ? 0 : FNE_CHECK_شروعات);
    إذا (اظهار_شروعات != عدم)
    {
	char_u	*عارضي_string;

	إذا (!evaluate)
	{
	    len += (صحيح)(p - *arg);
	    *arg = skipwhite(p);
	    أرجع len;
	}

	/*
	 * Include any <SID> etc in the expanded string:
	 * Thus the -len here.
	 */
	عارضي_string = make_expanded_name(*arg - len, اظهار_شروعات, اظهار_پڇاڙي, p);
	إذا (عارضي_string == عدم)
	    أرجع -1;
	*alias = عارضي_string;
	*arg = skipwhite(p);
	أرجع (صحيح)STRLEN(عارضي_string);
    }

    len += get_id_len(arg);
    // Only give an error when there is something, otherwise it will be
    // reported at a higher level.
    إذا (len == 0 && verbose && **arg != NUL)
	semsg(_(e_invalid_اظهار_str), *arg);

    أرجع len;
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
ڳوليو_name_پڇاڙي(
    char_u	*arg,
    char_u	**اظهار_شروعات,
    char_u	**اظهار_پڇاڙي,
    صحيح		flags)
{
    صحيح		mb_nest = 0;
    صحيح		br_nest = 0;
    char_u	*p;
    صحيح		len;
    صحيح		allow_curly = !اندريون_vim9script();

    إذا (اظهار_شروعات != عدم)
    {
	*اظهار_شروعات = عدم;
	*اظهار_پڇاڙي = عدم;
    }

    // Quick check for valid starting character.
    إذا ((flags & FNE_CHECK_شروعات) && !eval_isnamec1(*arg)
					      && (*arg != '{' || !allow_curly))
	أرجع arg;

    لكل (p = arg; *p != NUL
		    && (eval_isnamec(*p)
			|| (*p == '{' && allow_curly)
			|| ((flags & FNE_INCL_BR) && (*p == '['
					 || (*p == '.' && eval_isdictc(p[1]))))
			|| mb_nest != 0
			|| br_nest != 0); MB_PTR_ADV(p))
    {
	إذا (*p == '\'')
	{
	    // skip over 'string' to avoid counting [ and ] inside it.
	    لكل (p = p + 1; *p != NUL && *p != '\''; MB_PTR_ADV(p))
		;
	    إذا (*p == NUL)
		اكسر;
	}
	وإلا إذا (*p == '"')
	{
	    // skip over "str\"ing" to avoid counting [ and ] inside it.
	    لكل (p = p + 1; *p != NUL && *p != '"'; MB_PTR_ADV(p))
		إذا (*p == '\\' && p[1] != NUL)
		    ++p;
	    إذا (*p == NUL)
		اكسر;
	}
	وإلا إذا (br_nest == 0 && mb_nest == 0 && *p == ':')
	{
	    // "s:" is start of "s:var", but "n:" is not and can be used in
	    // slice "[n:]".  Also "xx:" is not a namespace. But {ns}: is.
	    len = (صحيح)(p - arg);
	    إذا ((len == 1 && vim_strchr(NAMESPACE_CHAR, *arg) == عدم)
		    || (len > 1 && p[-1] != '}'))
		اكسر;
	}

	إذا (mb_nest == 0)
	{
	    إذا (*p == '[')
		++br_nest;
	    وإلا إذا (*p == ']')
		--br_nest;
	}

	إذا (br_nest == 0 && allow_curly)
	{
	    إذا (*p == '{')
	    {
		mb_nest++;
		إذا (اظهار_شروعات != عدم && *اظهار_شروعات == عدم)
		    *اظهار_شروعات = p;
	    }
	    وإلا إذا (*p == '}')
	    {
		mb_nest--;
		إذا (اظهار_شروعات != عدم && mb_nest == 0 && *اظهار_پڇاڙي == عدم)
		    *اظهار_پڇاڙي = p;
	    }
	}
    }

    أرجع p;
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
    ساكن char_u *
make_expanded_name(
    char_u	*اندريون_شروعات,
    char_u	*اظهار_شروعات,
    char_u	*اظهار_پڇاڙي,
    char_u	*اندريون_پڇاڙي)
{
    char_u	c1;
    char_u	*retval = عدم;
    char_u	*عارضي_نتيجو;

    إذا (اظهار_پڇاڙي == عدم || اندريون_پڇاڙي == عدم)
	أرجع عدم;
    *اظهار_شروعات	= NUL;
    *اظهار_پڇاڙي = NUL;
    c1 = *اندريون_پڇاڙي;
    *اندريون_پڇاڙي = NUL;

    عارضي_نتيجو = eval_منزل_string(اظهار_شروعات + 1, FALSE, FALSE);
    إذا (عارضي_نتيجو != عدم)
    {
	size_t	retvalsize = (size_t)(اظهار_شروعات - اندريون_شروعات)
				+ STRLEN(عارضي_نتيجو)
				+ (size_t)(اندريون_پڇاڙي - اظهار_پڇاڙي) + 1;

	retval = alloc(retvalsize);
	إذا (retval != عدم)
	    vim_snprintf((حرف *)retval, retvalsize, "%s%s%s",
				اندريون_شروعات, عارضي_نتيجو, اظهار_پڇاڙي + 1);
    }
    vim_free(عارضي_نتيجو);

    *اندريون_پڇاڙي = c1;		// put char back for error messages
    *اظهار_شروعات = '{';
    *اظهار_پڇاڙي = '}';

    إذا (retval != عدم)
    {
	عارضي_نتيجو = ڳوليو_name_پڇاڙي(retval, &اظهار_شروعات, &اظهار_پڇاڙي, 0);
	إذا (اظهار_شروعات != عدم)
	{
	    // Further expansion!
	    عارضي_نتيجو = make_expanded_name(retval, اظهار_شروعات,
						       اظهار_پڇاڙي, عارضي_نتيجو);
	    vim_free(retval);
	    retval = عارضي_نتيجو;
	}
    }

    أرجع retval;
}

/*
 * Return TRUE if character "c" can be used in a variable or function name.
 * Does not include '{' or '}' for magic braces.
 */
    صحيح
eval_isnamec(صحيح c)
{
    أرجع ASCII_ISALNUM(c) || c == '_' || c == ':' || c == AUTOLOAD_CHAR;
}

/*
 * Return TRUE if character "c" can be used as the first character in a
 * variable or function name (excluding '{' and '}').
 */
    صحيح
eval_isnamec1(صحيح c)
{
    أرجع ASCII_ISALPHA(c) || c == '_';
}

/*
 * Return TRUE if character "c" can be used as the first character of a
 * dictionary key.
 */
    صحيح
eval_isdictc(صحيح c)
{
    أرجع ASCII_ISALNUM(c) || c == '_';
}

/*
 * Return true when "p" points to "->" followed by a method name or a lambda.
 * Only called for Vim9 script.
 */
    ساكن bool
طريقو_call_follows(char_u *p)
{
    إذا (p[0] != '-' || p[1] != '>')
	أرجع false;

    char_u	*after = skipwhite(p + 2);

    أرجع ASCII_ISALPHA(*after) || *after == '(';
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
    صحيح
handle_subscript(
    char_u	**arg,
    char_u	*name_شروعات,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    صحيح		verbose)	// give error messages
{
    صحيح		evaluate = evalarg != عدم
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    صحيح		ret = OK;
    dict_T	*selfdict = عدم;
    صحيح		check_white = TRUE;
    صحيح		getnext;
    char_u	*p;

    طالما (ret == OK)
    {
	// When at the end of the line and ".name" or a method call follows in
	// the next line then consume the line break.
	p = eval_ايندڙ_non_blank(*arg, evalarg, &getnext);
	إذا (getnext
	    && ((*p == '.'
		    && ((rettv->v_type == VAR_DICT && eval_isdictc(p[1]))
			|| rettv->v_type == VAR_CLASS
			|| rettv->v_type == VAR_OBJECT))
		|| طريقو_call_follows(p)))
	{
	    *arg = eval_ايندڙ_سٽ(*arg, evalarg);
	    p = *arg;
	    check_white = FALSE;
	}

	إذا (rettv->v_type == VAR_ANY)
	{
	    char_u	*exp_name;
	    صحيح		cc;
	    صحيح		idx;
	    ufunc_T	*ufunc;
	    type_T	*type;

	    // Found script from "import {name} as name", script item name must
	    // follow.  "rettv->vval.v_number" has the script ID.
	    إذا (**arg != '.')
	    {
		إذا (verbose)
		    semsg(_(e_expected_dot_after_name_str),
					name_شروعات != عدم ? name_شروعات: *arg);
		ret = FAIL;
		اكسر;
	    }
	    ++*arg;
	    إذا (IS_WHITE_OR_NUL(**arg))
	    {
		إذا (verbose)
		    emsg(_(e_no_white_space_allowed_after_dot));
		ret = FAIL;
		اكسر;
	    }

	    // isolate the name
	    exp_name = *arg;
	    طالما (eval_isnamec(**arg))
		++*arg;
	    cc = **arg;
	    **arg = NUL;

	    idx = ڳوليو_exported(rettv->vval.v_عدد, exp_name, &ufunc, &type,
		       evalarg == عدم ? عدم : evalarg->eval_cctx,
		       evalarg == عدم ? عدم : evalarg->eval_cstack, verbose);
	    **arg = cc;

	    إذا (idx < 0 && ufunc == عدم)
	    {
		ret = FAIL;
		اكسر;
	    }
	    إذا (idx >= 0)
	    {
		scriptitem_T    *si = SCRIPT_شي(rettv->vval.v_عدد);
		svar_T		*sv = ((svar_T *)si->sn_var_vals.ga_معلومات) + idx;

		copy_tv(sv->sv_tv, rettv);
	    }
	    وإلا
	    {
		rettv->v_type = VAR_FUNC;
		إذا (**arg == '<')
		{
		    char_u *s = get_generic_func_name(ufunc, arg);
		    إذا (s != عدم)
			rettv->vval.v_string = s;
		    وإلا
			ret = FAIL;
		}
		وإلا
		{
		    rettv->vval.v_string =
			vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
		}
	    }
	    استمر;
	}

	إذا ((**arg == '(' && (!evaluate || rettv->v_type == VAR_FUNC
			    || rettv->v_type == VAR_PARTIAL))
		    && (!check_white || !VIM_ISWHITE(*(*arg - 1))))
	{
	    ret = call_func_rettv(arg, evalarg, rettv, evaluate,
							       selfdict, عدم);

	    // Stop the expression evaluation when immediately aborting on
	    // error, or when an interrupt occurred or an exception was thrown
	    // but not caught.
	    إذا (aborting())
	    {
		إذا (ret == OK)
		    clear_tv(rettv);
		ret = FAIL;
	    }
	    dict_unref(selfdict);
	    selfdict = عدم;
	}
	وإلا إذا (p[0] == '-' && p[1] == '>')
	{
	    إذا (اندريون_vim9script())
		*arg = skipwhite(p + 2);
	    وإلا
		*arg = p + 2;
	    إذا (ret == OK && evaluate && rettv->v_type == VAR_VOID)
	    {
		إذا (verbose)
		    emsg(_(e_cannot_use_void_قدر));
		ret = FAIL;
	    }
	    وإلا إذا (VIM_ISWHITE(**arg))
	    {
		emsg(_(e_no_white_space_allowed_before_parenthesis));
		ret = FAIL;
	    }
	    وإلا إذا ((**arg == '{' && !اندريون_vim9script()) || **arg == '(')
		// expr->{lambda}() or expr->(lambda)()
		ret = eval_lambda(arg, rettv, evalarg, verbose);
	    وإلا
		// expr->name()
		ret = eval_طريقو(arg, rettv, evalarg, verbose);
	}
	// "." is ".name" lookup when we found a dict or when evaluating and
	// scriptversion is at least 2, where string concatenation is "..".
	وإلا إذا (**arg == '['
		|| (**arg == '.' && (rettv->v_type == VAR_DICT
			|| (!evaluate
			    && (*arg)[1] != '.'
			    && !اندريون_old_script(2)))))
	{
	    dict_unref(selfdict);
	    إذا (rettv->v_type == VAR_DICT)
	    {
		selfdict = rettv->vval.v_dict;
		إذا (selfdict != عدم)
		    ++selfdict->dv_refcount;
	    }
	    وإلا
		selfdict = عدم;
	    إذا (eval_اشاريو(arg, rettv, evalarg, verbose) == FAIL)
	    {
		clear_tv(rettv);
		ret = FAIL;
	    }
	}
	وإلا إذا (**arg == '.' && (rettv->v_type == VAR_CLASS
					       || rettv->v_type == VAR_OBJECT))
	{
	    // class member: SomeClass.varname
	    // class method: SomeClass.SomeMethod()
	    // class constructor: SomeClass.new()
	    // object member: someObject.varname
	    // object method: someObject.SomeMethod()
	    إذا (class_object_اشاريو(arg, rettv, evalarg, verbose) == FAIL)
	    {
		clear_tv(rettv);
		ret = FAIL;
	    }
	}
	وإلا
	    اكسر;
    }

    // Turn "dict.Func" into a partial for "Func" bound to "dict".
    // Don't do this when "Func" is already a partial that was bound
    // explicitly (pt_auto is FALSE).
    إذا (selfdict != عدم
	    && (rettv->v_type == VAR_FUNC
		|| (rettv->v_type == VAR_PARTIAL
		    && (rettv->vval.v_partial->pt_auto
			|| rettv->vval.v_partial->pt_dict == عدم))))
	selfdict = make_partial(selfdict, rettv);

    dict_unref(selfdict);
    أرجع ret;
}

/*
 * Make a copy of an item.
 * Lists and Dictionaries are also copied.  A deep copy if "deep" is set.
 * "top" is TRUE for the toplevel of copy().
 * For deepcopy() "copyID" is zero for a full copy or the ID for when a
 * reference to an already copied list/dict can be used.
 * Returns FAIL or OK.
 */
    صحيح
شي_copy(
    typval_T	*from,
    typval_T	*منزل,
    صحيح		deep,
    صحيح		چوٽي,
    صحيح		copyID)
{
    ساكن صحيح	recurse = 0;
    صحيح		ret = OK;

    إذا (recurse >= DICT_MAXNEST)
    {
	emsg(_(e_variable_nested_too_deep_for_making_copy));
	أرجع FAIL;
    }
    ++recurse;

    بدل (from->v_type)
    {
	حالة VAR_عدد:
	حالة VAR_FLOAT:
	حالة VAR_STRING:
	حالة VAR_FUNC:
	حالة VAR_PARTIAL:
	حالة VAR_BOOL:
	حالة VAR_SPECIAL:
	حالة VAR_ڪم:
	حالة VAR_CHANNEL:
	حالة VAR_INSTR:
	حالة VAR_CLASS:
	حالة VAR_OBJECT:
	حالة VAR_TYPEALIAS:
	    copy_tv(from, منزل);
	    اكسر;
	حالة VAR_LIST:
	    منزل->v_type = VAR_LIST;
	    منزل->v_تالو = 0;
	    إذا (from->vval.v_list == عدم)
		منزل->vval.v_list = عدم;
	    وإلا إذا (copyID != 0 && from->vval.v_list->lv_copyID == copyID)
	    {
		// use the copy made earlier
		منزل->vval.v_list = from->vval.v_list->lv_copylist;
		++منزل->vval.v_list->lv_refcount;
	    }
	    وإلا
	    {
		منزل->vval.v_list = list_copy(from->vval.v_list,
							    deep, چوٽي, copyID);
		إذا (منزل->vval.v_list == عدم)
		    ret = FAIL;
	    }
	    اكسر;
	حالة VAR_TUPLE:
	    منزل->v_type = VAR_TUPLE;
	    منزل->v_تالو = 0;
	    إذا (from->vval.v_tuple == عدم)
		منزل->vval.v_tuple = عدم;
	    وإلا إذا (copyID != 0 && from->vval.v_tuple->tv_copyID == copyID)
	    {
		// use the copy made earlier
		منزل->vval.v_tuple = from->vval.v_tuple->tv_copytuple;
		++منزل->vval.v_tuple->tv_refcount;
	    }
	    وإلا
	    {
		منزل->vval.v_tuple = tuple_copy(from->vval.v_tuple,
							    deep, چوٽي, copyID);
		إذا (منزل->vval.v_tuple == عدم)
		    ret = FAIL;
	    }
	    اكسر;
	حالة VAR_BLOB:
	    منزل->v_type = VAR_BLOB;
	    منزل->v_تالو = 0;
	    إذا (from->vval.v_blob == عدم)
		منزل->vval.v_blob = عدم;
	    وإلا
	    {
		منزل->vval.v_blob = blob_copy(from->vval.v_blob);
		إذا (منزل->vval.v_blob == عدم)
		    ret = FAIL;
	    }
	    اكسر;
	حالة VAR_DICT:
	    منزل->v_type = VAR_DICT;
	    منزل->v_تالو = 0;
	    إذا (from->vval.v_dict == عدم)
		منزل->vval.v_dict = عدم;
	    وإلا إذا (copyID != 0 && from->vval.v_dict->dv_copyID == copyID)
	    {
		// use the copy made earlier
		منزل->vval.v_dict = from->vval.v_dict->dv_copydict;
		++منزل->vval.v_dict->dv_refcount;
	    }
	    وإلا
	    {
		منزل->vval.v_dict = dict_copy(from->vval.v_dict,
							    deep, چوٽي, copyID);
		إذا (منزل->vval.v_dict == عدم)
		    ret = FAIL;
	    }
	    اكسر;
	حالة VAR_UNKNOWN:
	حالة VAR_ANY:
	حالة VAR_VOID:
	    اندروني_error_no_abort("item_copy(UNKNOWN)");
	    ret = FAIL;
    }
    --recurse;
    أرجع ret;
}

    فراغ
echo_one(typval_T *rettv, صحيح with_space, صحيح *atstart, صحيح *needclr)
{
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    char_u	*p = echo_string(rettv, &tofree, numbuf, get_copyID());

    إذا (*atstart)
    {
	*atstart = FALSE;
	// Call msg_start() after eval1(), evaluating the expression
	// may cause a message to appear.
	إذا (with_space)
	{
	    // Mark the saved text as finishing the line, so that what
	    // follows is displayed on a new line when scrolling back
	    // at the more prompt.
	    نياپو_sb_eol();
	    نياپو_شروعات();
	}
    }
    وإلا إذا (with_space)
	نياپو_puts_attr(" ", echo_attr);

    إذا (p != عدم)
	لكل ( ; *p != NUL && !got_int; ++p)
	{
	    إذا (*p == '\n' || *p == '\r' || *p == TAB)
	    {
		إذا (*p != TAB && *needclr)
		{
		    // remove any text still there from the command
		    نياپو_clr_eos();
		    *needclr = FALSE;
		}
		نياپو_putchar_attr(*p, echo_attr);
	    }
	    وإلا
	    {
		إذا (has_mbyte)
		{
		    صحيح i = (*mb_ptr2len)(p);

		    (فراغ)نياپو_outtrans_len_attr(p, i, echo_attr);
		    p += i - 1;
		}
		وإلا
		    (فراغ)نياپو_outtrans_len_attr(p, 1, echo_attr);
	    }
	}
    vim_free(tofree);
}

/*
 * ":echo expr1 ..."	print each argument separated with a space, add a
 *			newline at the end.
 * ":echon expr1 ..."	print each argument plain.
 */
    فراغ
ex_echo(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    typval_T	rettv;
    char_u	*arg_شروعات;
    صحيح		needclr = TRUE;
    صحيح		atstart = TRUE;
    صحيح		did_emsg_before = did_emsg;
    صحيح		called_emsg_before = called_emsg;
    evalarg_T	evalarg;

    fill_evalarg_from_eap(&evalarg, eap, eap->skip);

    إذا (eap->skip)
	++emsg_skip;
    طالما ((!پڇاڙيون_excmd2(eap->cmd, arg) || *arg == '"') && !got_int)
    {
	// If eval1() causes an error message the text from the command may
	// still need to be cleared. E.g., "echo 22,44".
	need_clr_eos = needclr;

	arg_شروعات = arg;
	إذا (eval1(&arg, &rettv, &evalarg) == FAIL)
	{
	    /*
	     * Report the invalid expression unless the expression evaluation
	     * has been cancelled due to an aborting error, an interrupt, or an
	     * exception.
	     */
	    إذا (!aborting() && did_emsg == did_emsg_before
					  && called_emsg == called_emsg_before)
		semsg(_(e_invalid_اظهار_str), arg_شروعات);
	    need_clr_eos = FALSE;
	    اكسر;
	}
	need_clr_eos = FALSE;

	إذا (!eap->skip)
	{
	    إذا (rettv.v_type == VAR_VOID)
	    {
		semsg(_(e_اظهار_does_not_نتيجو_اندريون_قدر_str), arg_شروعات);
		اكسر;
	    }
	    echo_one(&rettv, eap->cmdidx == CMD_echo, &atstart, &needclr);
	}

	clear_tv(&rettv);
	arg = skipwhite(arg);
    }
    سيٽ_nextcmd(eap, arg);
    clear_evalarg(&evalarg, eap);

    إذا (eap->skip)
	--emsg_skip;
    وإلا
    {
	// remove text that may still be there from the command
	إذا (needclr)
	    نياپو_clr_eos();
	إذا (eap->cmdidx == CMD_echo)
	    نياپو_پڇاڙي();
    }
}

/*
 * ":echohl {name}".
 */
    فراغ
ex_echohl(exarg_T *eap)
{
    echo_attr = syn_name2attr(eap->arg);
}

/*
 * Returns the :echo attribute
 */
    صحيح
get_echo_attr(فراغ)
{
    أرجع echo_attr;
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
    فراغ
ex_execute(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    typval_T	rettv;
    صحيح		ret = OK;
    char_u	*p;
    garray_T	ga;
    صحيح		len;
    ڊگهو	شروعات_lnum = SOURCING_LNUM;

    ga_init2(&ga, 1, 80);

    إذا (eap->skip)
	++emsg_skip;
    طالما (!پڇاڙيون_excmd2(eap->cmd, arg) || *arg == '"')
    {
	ret = eval1_emsg(&arg, &rettv, eap);
	إذا (ret == FAIL)
	    اكسر;

	إذا (!eap->skip)
	{
	    char_u   buf[NUMBUFLEN];

	    إذا (eap->cmdidx == CMD_execute)
	    {
		إذا (rettv.v_type == VAR_CHANNEL || rettv.v_type == VAR_ڪم)
		{
		    semsg(_(e_using_invalid_قدر_as_string_str),
						  vartype_name(rettv.v_type));
		    p = عدم;
		}
		وإلا
		    p = tv_get_string_buf(&rettv, buf);
	    }
	    وإلا
		p = tv_stringify(&rettv, buf);
	    إذا (p == عدم)
	    {
		clear_tv(&rettv);
		ret = FAIL;
		اكسر;
	    }
	    len = (صحيح)STRLEN(p);
	    إذا (ga_grow(&ga, len + 2) == FAIL)
	    {
		clear_tv(&rettv);
		ret = FAIL;
		اكسر;
	    }
	    إذا (ga.ga_len)
		((char_u *)(ga.ga_معلومات))[ga.ga_len++] = ' ';
	    STRCPY((char_u *)(ga.ga_معلومات) + ga.ga_len, p);
	    ga.ga_len += len;
	}

	clear_tv(&rettv);
	arg = skipwhite(arg);
    }

    إذا (ret != FAIL && ga.ga_معلومات != عدم)
    {
	// use the first line of continuation lines for messages
	SOURCING_LNUM = شروعات_lnum;

	إذا (eap->cmdidx == CMD_echomsg
		|| eap->cmdidx == CMD_echowindow
		|| eap->cmdidx == CMD_echoerr)
	{
	    // Mark the already saved text as finishing the line, so that what
	    // follows is displayed on a new line when scrolling back at the
	    // more prompt.
	    نياپو_sb_eol();
	}

	إذا (eap->cmdidx == CMD_echomsg)
	{
	    نياپو_attr(ga.ga_معلومات, echo_attr);
	    ٻاهريون_flush();
	}
	وإلا إذا (eap->cmdidx == CMD_echowindow)
	{
#جيڪڏهن_وصف_ٿيل HAS_پيغام_دري
	    شروعات_echowindow(eap->addr_ڳڻپ > 0 ? eap->line2 : 0);
#جيڪڏهن_پورو
	    نياپو_attr(ga.ga_معلومات, echo_attr);
#جيڪڏهن_وصف_ٿيل HAS_پيغام_دري
	    پڇاڙي_echowindow();
#جيڪڏهن_پورو
	}
	وإلا إذا (eap->cmdidx == CMD_echoconsole)
	{
	    ui_write(ga.ga_معلومات, (صحيح)STRLEN(ga.ga_معلومات), TRUE);
	    ui_write((char_u *)"\r\n", 2, TRUE);
	}
	وإلا إذا (eap->cmdidx == CMD_echoerr)
	{
	    صحيح		save_did_emsg = did_emsg;

	    // We don't want to abort following commands, restore did_emsg.
	    emsg(ga.ga_معلومات);
	    إذا (!force_abort)
		did_emsg = save_did_emsg;
	}
	وإلا إذا (eap->cmdidx == CMD_execute)
	{
	    صحيح save_sticky_cmdmod_flags = sticky_cmdmod_flags;

	    // "legacy exe cmd" and "vim9cmd exe cmd" applies to "cmd".
	    sticky_cmdmod_flags = cmdmod.cmod_flags
						& (CMOD_LEGACY | CMOD_VIM9CMD);
	    do_cmdline((char_u *)ga.ga_معلومات,
		       eap->ea_getline, eap->cookie, DOCMD_NOWAIT|DOCMD_VERBOSE);
	    sticky_cmdmod_flags = save_sticky_cmdmod_flags;
	}
    }

    ga_clear(&ga);

    إذا (eap->skip)
	--emsg_skip;
    سيٽ_nextcmd(eap, arg);
}

/*
 * Skip over the name of an option: "&option", "&g:option" or "&l:option".
 * "arg" points to the "&" or '+' when called, to "option" when returning.
 * Returns NULL when no option name found.  Otherwise pointer to the char
 * after the option name.
 */
    char_u *
ڳوليو_option_پڇاڙي(char_u **arg, صحيح *scope)
{
    char_u	*p = *arg;

    ++p;
    إذا (*p == 'g' && p[1] == ':')
    {
	*scope = OPT_GLOBAL;
	p += 2;
    }
    وإلا إذا (*p == 'l' && p[1] == ':')
    {
	*scope = OPT_LOCAL;
	p += 2;
    }
    وإلا
	*scope = 0;

    إذا (!ASCII_ISALPHA(*p))
	أرجع عدم;
    *arg = p;

    إذا (p[0] == 't' && p[1] == '_' && p[2] != NUL && p[3] != NUL)
	p += 4;	    // termcap option
    وإلا
	طالما (ASCII_ISALPHA(*p))
	    ++p;
    أرجع p;
}

/*
 * Display script name where an item was last set.
 * Should only be invoked when 'verbose' is non-zero.
 */
    فراغ
آخري_سيٽ_نياپو(sctx_T script_ctx)
{
    char_u *p;

    إذا (script_ctx.sc_sid == 0)
	أرجع;

    p = home_replace_save(عدم, get_scriptname(script_ctx.sc_sid));
    إذا (p == عدم)
	أرجع;

    verbose_enter();
    نياپو_puts(_("\n\tLast set from "));
    نياپو_puts((حرف *)p);
    إذا (script_ctx.sc_lnum > 0)
    {
	نياپو_puts(_(سٽ_نياپو));
	نياپو_outnum((ڊگهو)script_ctx.sc_lnum);
    }
    verbose_leave();
    vim_free(p);
}

#جيڪڏهن_پورو // FEAT_EVAL

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
    typval_T	*اظهار,
    char_u	*flags,
    size_t	*ret_len)		// length of returned buffer
{
    regmatch_T	regmatch;
    garray_T	ga;
    char_u	*ret;
    char_u	*save_cpo;

    // Make 'cpoptions' empty, so that the 'l' flag doesn't work here
    save_cpo = p_cpo;
    p_cpo = خالي_جايون_option;

    ga_init2(&ga, 1, 200);

    regmatch.rm_ic = p_ic;
    regmatch.regprog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
    إذا (regmatch.regprog != عدم)
    {
	char_u	*پڇ = str;
	char_u	*پڇاڙي = str + len;
	صحيح	do_all = (flags[0] == 'g');
	صحيح	sublen;
	صحيح	i;
	char_u	*zero_ويڪر = عدم;

	طالما (vim_regexec_nl(&regmatch, str, (colnr_T)(پڇ - str)))
	{
	    // Skip empty match except for first match.
	    إذا (regmatch.startp[0] == regmatch.endp[0])
	    {
		إذا (zero_ويڪر == regmatch.startp[0])
		{
		    // avoid getting stuck on a match with an empty string
		    i = mb_ptr2len(پڇ);
		    mch_memmove((char_u *)ga.ga_معلومات + ga.ga_len, پڇ,
								   (size_t)i);
		    ga.ga_len += i;
		    پڇ += i;
		    استمر;
		}
		zero_ويڪر = regmatch.startp[0];
	    }

	    /*
	     * Get some space for a temporary buffer to do the substitution
	     * into.  It will contain:
	     * - The text up to where the match is.
	     * - The substituted text.
	     * - The text after the match.
	     */
	    sublen = vim_regsub(&regmatch, sub, اظهار, پڇ, 0, REGSUB_MAGIC);
	    إذا (sublen <= 0)
	    {
		ga_clear(&ga);
		اكسر;
	    }
	    إذا (ga_grow(&ga, (صحيح)((پڇاڙي - پڇ) + sublen -
			    (regmatch.endp[0] - regmatch.startp[0]))) == FAIL)
	    {
		ga_clear(&ga);
		اكسر;
	    }

	    // copy the text up to where the match is
	    i = (صحيح)(regmatch.startp[0] - پڇ);
	    mch_memmove((char_u *)ga.ga_معلومات + ga.ga_len, پڇ, (size_t)i);
	    // add the substituted text
	    (فراغ)vim_regsub(&regmatch, sub, اظهار,
				  (char_u *)ga.ga_معلومات + ga.ga_len + i, sublen,
				  REGSUB_COPY | REGSUB_MAGIC);
	    ga.ga_len += i + sublen - 1;
	    پڇ = regmatch.endp[0];
	    إذا (*پڇ == NUL)
		اكسر;
	    إذا (!do_all)
		اكسر;
	}

	إذا (ga.ga_معلومات != عدم)
	{
	    STRCPY((حرف *)ga.ga_معلومات + ga.ga_len, پڇ);
	    ga.ga_len += (صحيح)(پڇاڙي - پڇ);
	}

	vim_regfree(regmatch.regprog);
    }

    إذا (ga.ga_معلومات != عدم)
    {
	str = (char_u *)ga.ga_معلومات;
	len = (size_t)ga.ga_len;
    }
    ret = vim_strnsave(str, len);
    ga_clear(&ga);
    إذا (p_cpo == خالي_جايون_option)
	p_cpo = save_cpo;
    وإلا
    {
	// Darn, evaluating {sub} expression or {expr} changed the value.
	// If it's still empty it was changed and restored, need to restore in
	// the complicated way.
	إذا (*p_cpo == NUL)
	    سيٽ_option_قدر_give_err((char_u *)"cpo", 0L, save_cpo, 0);
	free_string_option(save_cpo);
    }

    إذا (ret_len != عدم)
	*ret_len = len;

    أرجع ret;
}
