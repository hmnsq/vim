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

#અન્તર્ભાવ "vim.h"

#જો defined(FEAT_EVAL)

#ifdef VMS
# include <float.h>
#endif

#define NAMESPACE_CHAR	(char_u *)"abglstvw"

સ્થિર પૂર્ણાંક eval0_simple_funccal(char_u *arg, typval_T *rettv, exarg_T *eap, evalarg_T *evalarg);
સ્થિર પૂર્ણાંક eval2(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
સ્થિર પૂર્ણાંક eval3(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
સ્થિર પૂર્ણાંક eval4(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
સ્થિર પૂર્ણાંક eval5(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
સ્થિર પૂર્ણાંક eval6(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
સ્થિર પૂર્ણાંક eval7(char_u **arg, typval_T *rettv, evalarg_T *evalarg, પૂર્ણાંક want_string);
સ્થિર પૂર્ણાંક eval8(char_u **arg, typval_T *rettv, evalarg_T *evalarg, પૂર્ણાંક want_string);
સ્થિર પૂર્ણાંક eval9(char_u **arg, typval_T *rettv, evalarg_T *evalarg, પૂર્ણાંક want_string);
સ્થિર પૂર્ણાંક eval9_leader(typval_T *rettv, પૂર્ણાંક numeric_only, char_u *start_leader, char_u **end_leaderp);

સ્થિર char_u *make_expanded_name(char_u *in_start, char_u *પદાવલિ_start, char_u *પદાવલિ_end, char_u *in_end);

/*
 * Return "n1" divided by "n2", taking care of dividing by zero.
 * If "failed" is not NULL set it to TRUE when dividing by zero fails.
 */
	varnumber_T
અંક_divide(varnumber_T n1, varnumber_T n2, પૂર્ણાંક *failed)
{
    varnumber_T	result;

    જો (n2 == 0)
    {
	જો (in_vim9script())
	{
	    emsg(_(e_divide_by_zero));
	    જો (failed != ખાલી)
		*failed = TRUE;
	}
	જો (n1 == 0)
	    result = VARNUM_MIN; // similar to NaN
	નહીંતર જો (n1 < 0)
	    result = -VARNUM_MAX;
	નહીંતર
	    result = VARNUM_MAX;
    }
    નહીંતર જો (n1 == VARNUM_MIN && n2 == -1)
    {
	// specific case: trying to do VARNUM_MIN / -1 results in a positive
	// number that doesn't fit in varnumber_T and causes an FPE
	result = VARNUM_MAX;
    }
    નહીંતર
	result = n1 / n2;

    પાછા result;
}

/*
 * Return "n1" modulus "n2", taking care of dividing by zero.
 * If "failed" is not NULL set it to TRUE when dividing by zero fails.
 */
	varnumber_T
અંક_modulus(varnumber_T n1, varnumber_T n2, પૂર્ણાંક *failed)
{
    જો (n2 == 0 && in_vim9script())
    {
	emsg(_(e_divide_by_zero));
	જો (failed != ખાલી)
	    *failed = TRUE;
    }
    પાછા (n2 == 0) ? 0 : (n1 % n2);
}

/*
 * Initialize the global and v: variables.
 */
    શૂન્ય
eval_init(શૂન્ય)
{
    evalvars_init();
    func_init();
}

#જો defined(EXITFREE)
    શૂન્ય
eval_clear(શૂન્ય)
{
    evalvars_clear();
    free_scriptnames();  // must come after evalvars_clear().
    free_locales();

    // autoloaded script names
    free_autoload_scriptnames();

    // unreferenced lists, tuples and dicts
    (શૂન્ય)garbage_collect(FALSE);

    // functions not garbage collected
    free_all_functions();
}
#endif

    શૂન્ય
fill_evalarg_from_eap(evalarg_T *evalarg, exarg_T *eap, પૂર્ણાંક skip)
{
    init_evalarg(evalarg);
    evalarg->eval_flags = skip ? 0 : EVAL_EVALUATE;

    જો (eap == ખાલી)
	પાછા;

    evalarg->eval_cstack = eap->cstack;
    જો (sourcing_a_script(eap) || eap->ea_getline == get_list_લીટી)
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
    પૂર્ણાંક
eval_to_bool(
    char_u	*arg,
    પૂર્ણાંક		*error,
    exarg_T	*eap,
    પૂર્ણાંક		skip,	    // only parse, don't execute
    પૂર્ણાંક		use_simple_function)
{
    typval_T	tv;
    varnumber_T	retval = FALSE;
    evalarg_T	evalarg;
    પૂર્ણાંક		r;

    fill_evalarg_from_eap(&evalarg, eap, skip);

    જો (skip)
	++emsg_skip;
    જો (use_simple_function)
	r = eval0_simple_funccal(arg, &tv, eap, &evalarg);
    નહીંતર
	r = eval0(arg, &tv, eap, &evalarg);
    જો (r == FAIL)
	*error = TRUE;
    નહીંતર
    {
	*error = FALSE;
	જો (!skip)
	{
	    જો (in_vim9script())
		retval = tv_get_bool_chk(&tv, error);
	    નહીંતર
		retval = (tv_get_સંખ્યા_chk(&tv, error) != 0);
	    clear_tv(&tv);
	}
    }
    જો (skip)
	--emsg_skip;
    clear_evalarg(&evalarg, eap);

    પાછા (પૂર્ણાંક)retval;
}

/*
 * Call eval1() and give an error message if not done at a lower level.
 */
    સ્થિર પૂર્ણાંક
eval1_emsg(char_u **arg, typval_T *rettv, exarg_T *eap)
{
    char_u	*start = *arg;
    પૂર્ણાંક		ret;
    પૂર્ણાંક		did_emsg_before = did_emsg;
    પૂર્ણાંક		called_emsg_before = called_emsg;
    evalarg_T	evalarg;

    fill_evalarg_from_eap(&evalarg, eap, eap != ખાલી && eap->skip);

    ret = eval1(arg, rettv, &evalarg);
    જો (ret == FAIL)
    {
	// Report the invalid expression unless the expression evaluation has
	// been cancelled due to an aborting error, an interrupt, or an
	// exception, or we already gave a more specific error.
	// Also check called_emsg for when using assert_fails().
	જો (!aborting() && did_emsg == did_emsg_before
					  && called_emsg == called_emsg_before)
	    semsg(_(e_invalid_expression_str), start);
    }
    clear_evalarg(&evalarg, eap);
    પાછા ret;
}

/*
 * Return whether a typval is a valid expression to pass to eval_expr_typval()
 * or eval_expr_to_bool().  An empty string returns FALSE;
 */
    પૂર્ણાંક
eval_પદાવલિ_valid_arg(typval_T *tv)
{
    પાછા tv->v_type != VAR_UNKNOWN
	    && (tv->v_type != VAR_STRING
		  || (tv->vval.v_string != ખાલી && *tv->vval.v_string != NUL));
}

/*
 * When calling eval_expr_typval() many times we only need one funccall_T.
 * Returns NULL when no funccall_T is to be used.
 * When returning non-NULL remove_funccal() must be called later.
 */
    funccall_T *
eval_પદાવલિ_get_funccal(typval_T *પદાવલિ, typval_T *rettv)
{
    જો (પદાવલિ->v_type != VAR_PARTIAL)
	પાછા ખાલી;

    partial_T *partial = પદાવલિ->vval.v_partial;
    જો (partial == ખાલી)
	પાછા ખાલી;
    જો (partial->pt_func == ખાલી
	    || partial->pt_func->uf_def_status == UF_NOT_COMPILED)
	પાછા ખાલી;

    પાછા create_funccal(partial->pt_func, rettv);
}

/*
 * Evaluate a partial.
 * Pass arguments "argv[argc]".
 * "fc_arg" is from eval_expr_get_funccal() or NULL;
 * Return the result in "rettv" and OK or FAIL.
 */
    સ્થિર પૂર્ણાંક
eval_પદાવલિ_partial(
    typval_T	*પદાવલિ,
    typval_T	*argv,
    પૂર્ણાંક		argc,
    funccall_T	*fc_arg,
    typval_T	*rettv)
{
    partial_T	*partial = પદાવલિ->vval.v_partial;

    જો (partial == ખાલી)
	પાછા FAIL;

    જો (partial->pt_func != ખાલી
			&& partial->pt_func->uf_def_status != UF_NOT_COMPILED)
    {
	funccall_T	*fc = fc_arg != ખાલી ? fc_arg
				: create_funccal(partial->pt_func, rettv);
	પૂર્ણાંક		r;

	જો (fc == ખાલી)
	    પાછા FAIL;

	// Shortcut to call a compiled function with minimal overhead.
	જો (partial->pt_obj != ખાલી)
	    partial->pt_obj->obj_refcount++;
	r = call_def_function(partial->pt_func, argc, argv, DEF_USE_PT_ARGV,
					partial, partial->pt_obj, fc, rettv);
	જો (fc_arg == ખાલી)
	    remove_funccal();
	જો (r == FAIL)
	    પાછા FAIL;
    }
    નહીંતર
    {
	char_u		*s = partial_name(partial);
	funcexe_T	funcexe;

	જો (s == ખાલી || *s == NUL)
	    પાછા FAIL;

	CLEAR_FIELD(funcexe);
	funcexe.fe_evaluate = TRUE;
	funcexe.fe_partial = partial;
	જો (call_func(s, -1, rettv, argc, argv, &funcexe) == FAIL)
	    પાછા FAIL;
    }

    પાછા OK;
}

/*
 * Evaluate an expression which is a function.
 * Pass arguments "argv[argc]".
 * Return the result in "rettv" and OK or FAIL.
 */
    સ્થિર પૂર્ણાંક
eval_પદાવલિ_func(
    typval_T	*પદાવલિ,
    typval_T	*argv,
    પૂર્ણાંક		argc,
    typval_T	*rettv)
{
    funcexe_T	funcexe;
    char_u	buf[NUMBUFLEN];
    char_u	*s;

    જો (પદાવલિ->v_type == VAR_FUNC)
	s = પદાવલિ->vval.v_string;
    નહીંતર
	s = tv_get_string_buf_chk_strict(પદાવલિ, buf, in_vim9script());
    જો (s == ખાલી || *s == NUL)
	પાછા FAIL;

    CLEAR_FIELD(funcexe);
    funcexe.fe_evaluate = TRUE;
    જો (call_func(s, -1, rettv, argc, argv, &funcexe) == FAIL)
	પાછા FAIL;

    પાછા OK;
}

/*
 * Evaluate an expression, which is a string.
 * Return the result in "rettv" and OK or FAIL.
 */
    સ્થિર પૂર્ણાંક
eval_પદાવલિ_string(
    typval_T	*પદાવલિ,
    typval_T	*rettv)
{
    char_u	*s;
    char_u	buf[NUMBUFLEN];

    s = tv_get_string_buf_chk_strict(પદાવલિ, buf, in_vim9script());
    જો (s == ખાલી)
	પાછા FAIL;

    s = skipwhite(s);
    જો (eval1_emsg(&s, rettv, ખાલી) == FAIL)
	પાછા FAIL;

    જો (*skipwhite(s) != NUL)  // check for trailing chars after expr
    {
	clear_tv(rettv);
	semsg(_(e_invalid_expression_str), s);
	પાછા FAIL;
    }

    પાછા OK;
}

/*
 * Evaluate an expression, which can be a function, partial or string.
 * Pass arguments "argv[argc]".
 * If "want_func" is TRUE treat a string as a function name, not an expression.
 * "fc_arg" is from eval_expr_get_funccal() or NULL;
 * Return the result in "rettv" and OK or FAIL.
 */
    પૂર્ણાંક
eval_પદાવલિ_typval(
	typval_T    *પદાવલિ,
	પૂર્ણાંક	    want_func,
	typval_T    *argv,
	પૂર્ણાંક	    argc,
	funccall_T  *fc_arg,
	typval_T    *rettv)
{
    જો (પદાવલિ->v_type == VAR_PARTIAL)
	પાછા eval_પદાવલિ_partial(પદાવલિ, argv, argc, fc_arg, rettv);
    જો (પદાવલિ->v_type == VAR_INSTR)
	પાછા exe_typval_instr(પદાવલિ, rettv);
    જો (પદાવલિ->v_type == VAR_FUNC || want_func)
	પાછા eval_પદાવલિ_func(પદાવલિ, argv, argc, rettv);

    પાછા eval_પદાવલિ_string(પદાવલિ, rettv);
}

/*
 * Like eval_to_bool() but using a typval_T instead of a string.
 * Works for string, funcref and partial.
 */
    પૂર્ણાંક
eval_પદાવલિ_to_bool(typval_T *પદાવલિ, પૂર્ણાંક *error)
{
    typval_T	rettv;
    પૂર્ણાંક		res;

    જો (eval_પદાવલિ_typval(પદાવલિ, FALSE, ખાલી, 0, ખાલી, &rettv) == FAIL)
    {
	*error = TRUE;
	પાછા FALSE;
    }
    res = (tv_get_bool_chk(&rettv, error) != 0);
    clear_tv(&rettv);
    પાછા res;
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
    પૂર્ણાંક		skip)	    // only parse, don't execute
{
    typval_T	tv;
    char_u	*retval;
    evalarg_T	evalarg;

    fill_evalarg_from_eap(&evalarg, eap, skip);
    જો (skip)
	++emsg_skip;
    જો (eval0(arg, &tv, eap, &evalarg) == FAIL || skip)
	retval = ખાલી;
    નહીંતર
    {
	retval = vim_strsave(tv_get_string(&tv));
	clear_tv(&tv);
    }
    જો (skip)
	--emsg_skip;
    clear_evalarg(&evalarg, eap);

    પાછા retval;
}

/*
 * Initialize "evalarg" for use.
 */
    શૂન્ય
init_evalarg(evalarg_T *evalarg)
{
    CLEAR_POINTER(evalarg);
    ga_init2(&evalarg->eval_tofree_ga, કદ(char_u *), 20);
}

/*
 * If "evalarg->eval_tofree" is not NULL free it later.
 * Caller is expected to overwrite "evalarg->eval_tofree" next.
 */
    સ્થિર શૂન્ય
free_eval_tofree_later(evalarg_T *evalarg)
{
    જો (evalarg->eval_tofree == ખાલી)
	પાછા;

    જો (ga_grow(&evalarg->eval_tofree_ga, 1) == OK)
	((char_u **)evalarg->eval_tofree_ga.ga_data)
	    [evalarg->eval_tofree_ga.ga_len++]
	    = evalarg->eval_tofree;
    નહીંતર
	vim_free(evalarg->eval_tofree);
}

/*
 * After using "evalarg" filled from "eap": free the memory.
 */
    શૂન્ય
clear_evalarg(evalarg_T *evalarg, exarg_T *eap)
{
    જો (evalarg == ખાલી)
	પાછા;

    garray_T *etga = &evalarg->eval_tofree_ga;

    જો (evalarg->eval_tofree != ખાલી || evalarg->eval_using_cmdline)
    {
	જો (eap != ખાલી)
	{
	    // We may need to keep the original command line, e.g. for
	    // ":let" it has the variable names.  But we may also need
	    // the new one, "nextcmd" points into it.  Keep both.
	    vim_free(eap->cmdline_tofree);
	    eap->cmdline_tofree = *eap->cmdlinep;

	    જો (evalarg->eval_using_cmdline && etga->ga_len > 0)
	    {
		// "nextcmd" points into the last line in eval_tofree_ga,
		// need to keep it around.
		--etga->ga_len;
		*eap->cmdlinep = ((char_u **)etga->ga_data)[etga->ga_len];
		vim_free(evalarg->eval_tofree);
	    }
	    નહીંતર
		*eap->cmdlinep = evalarg->eval_tofree;
	}
	નહીંતર
	    vim_free(evalarg->eval_tofree);
	evalarg->eval_tofree = ખાલી;
    }

    ga_clear_strings(etga);
    VIM_CLEAR(evalarg->eval_tofree_lambda);
}

/*
 * Skip over an expression at "*pp".
 * Return FAIL for an error, OK otherwise.
 */
    પૂર્ણાંક
skip_પદાવલિ(char_u **pp, evalarg_T *evalarg)
{
    typval_T	rettv;

    *pp = skipwhite(*pp);
    પાછા eval1(pp, &rettv, evalarg);
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
    પૂર્ણાંક
skip_પદાવલિ_concatenate(
	char_u	    **arg,
	char_u	    **start,
	char_u	    **end,
	evalarg_T   *evalarg)
{
    typval_T	rettv;
    પૂર્ણાંક		res;
    પૂર્ણાંક		vim9script = in_vim9script();
    garray_T    *gap = evalarg == ખાલી ? ખાલી : &evalarg->eval_ga;
    garray_T    *freegap = evalarg == ખાલી ? ખાલી : &evalarg->eval_freega;
    પૂર્ણાંક		save_flags = evalarg == ખાલી ? 0 : evalarg->eval_flags;
    પૂર્ણાંક		evaluate = evalarg == ખાલી
			       ? FALSE : (evalarg->eval_flags & EVAL_EVALUATE);

    જો (vim9script && evaluate
	       && (evalarg->eval_cookie != ખાલી || evalarg->eval_cctx != ખાલી))
    {
	ga_init2(gap, કદ(char_u *), 10);
	// leave room for "start"
	જો (ga_grow(gap, 1) == OK)
	    ++gap->ga_len;
	ga_init2(freegap, કદ(char_u *), 10);
    }
    *start = *arg;

    // Don't evaluate the expression.
    જો (evalarg != ખાલી)
	evalarg->eval_flags &= ~EVAL_EVALUATE;
    *arg = skipwhite(*arg);
    res = eval1(arg, &rettv, evalarg);
    *end = *arg;
    જો (evalarg != ખાલી)
	evalarg->eval_flags = save_flags;

    જો (vim9script && evaluate
	    && (evalarg->eval_cookie != ખાલી || evalarg->eval_cctx != ખાલી))
    {
	જો (evalarg->eval_ga.ga_len == 1)
	{
	    // just the one line, no need to concatenate
	    ga_clear(gap);
	    gap->ga_itemsize = 0;
	}
	નહીંતર
	{
	    char_u	    *p;
	    size_t	    endoff = STRLEN(*arg);

	    // Line breaks encountered, concatenate all the lines.
	    *((char_u **)gap->ga_data) = *start;
	    p = ga_concat_strings(gap, " ");

	    // free the lines only when using getsourceline()
	    જો (evalarg->eval_cookie != ખાલી)
	    {
		// Do not free the first line, the caller can still use it.
		*((char_u **)gap->ga_data) = ખાલી;
		// Do not free the last line, "arg" points into it, free it
		// later.  Also free "eval_tofree" later if needed.
		free_eval_tofree_later(evalarg);
		evalarg->eval_tofree =
				    ((char_u **)gap->ga_data)[gap->ga_len - 1];
		((char_u **)gap->ga_data)[gap->ga_len - 1] = ખાલી;
		ga_clear_strings(gap);
		ga_clear(freegap);
	    }
	    નહીંતર
	    {
		ga_clear(gap);

		// free lines that were explicitly marked for freeing
		ga_clear_strings(freegap);
	    }

	    gap->ga_itemsize = 0;
	    જો (p == ખાલી)
		પાછા FAIL;
	    *start = p;
	    vim_free(evalarg->eval_tofree_lambda);
	    evalarg->eval_tofree_lambda = p;
	    // Compute "end" relative to the end.
	    *end = *start + STRLEN(*start) - endoff;
	}
    }

    પાછા res;
}

/*
 * Convert "tv" to a string.
 * When "join_list" is TRUE convert a List or a Tuple into a sequence of lines.
 * Returns an allocated string (NULL when out of memory).
 */
    char_u *
typval2string(typval_T *tv, પૂર્ણાંક join_list)
{
    garray_T	ga;
    char_u	*retval = ખાલી;

    જો (join_list && (tv->v_type == VAR_LIST || tv->v_type == VAR_TUPLE))
    {
	જો (tv->v_type == VAR_LIST)
	{
	    ga_init2(&ga, કદ(અક્ષર), 80);
	    જો (tv->vval.v_list != ખાલી)
	    {
		list_join(&ga, tv->vval.v_list, (char_u *)"\n", TRUE, FALSE,
									0);
		જો (tv->vval.v_list->lv_len > 0)
		    ga_append(&ga, NL);
	    }
	    ga_append(&ga, NUL);
	    retval = (char_u *)ga.ga_data;
	}
	નહીંતર
	{
	    // tuple
	    ga_init2(&ga, કદ(અક્ષર), 80);
	    જો (tv->vval.v_tuple != ખાલી)
	    {
		tuple_join(&ga, tv->vval.v_tuple, (char_u *)"\n", TRUE, FALSE,
									0);
		જો (TUPLE_LEN(tv->vval.v_tuple) > 0)
		    ga_append(&ga, NL);
	    }
	    ga_append(&ga, NUL);
	    retval = (char_u *)ga.ga_data;
	}
    }
    નહીંતર જો (tv->v_type == VAR_LIST
	    || tv->v_type == VAR_TUPLE
	    || tv->v_type == VAR_DICT)
    {
	char_u	*tofree;
	char_u	numbuf[NUMBUFLEN];

	retval = tv2string(tv, &tofree, numbuf, 0);
	// Make a copy if we have a value but it's not in allocated memory.
	જો (retval != ખાલી && tofree == ખાલી)
	    retval = vim_strsave(retval);
    }
    નહીંતર
	retval = vim_strsave(tv_get_string(tv));
    પાછા retval;
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
    પૂર્ણાંક		join_list,
    exarg_T	*eap,
    પૂર્ણાંક		use_simple_function)
{
    typval_T	tv;
    char_u	*retval;
    evalarg_T	evalarg;
    પૂર્ણાંક		r;

    fill_evalarg_from_eap(&evalarg, eap, eap != ખાલી && eap->skip);
    જો (use_simple_function)
	r = eval0_simple_funccal(arg, &tv, ખાલી, &evalarg);
    નહીંતર
	r = eval0(arg, &tv, ખાલી, &evalarg);
    જો (r == FAIL)
	retval = ખાલી;
    નહીંતર
    {
	retval = typval2string(&tv, join_list);
	clear_tv(&tv);
    }
    clear_evalarg(&evalarg, ખાલી);

    પાછા retval;
}

    char_u *
eval_to_string(
    char_u	*arg,
    પૂર્ણાંક		join_list,
    પૂર્ણાંક		use_simple_function)
{
    પાછા eval_to_string_eap(arg, join_list, ખાલી, use_simple_function);
}

/*
 * Call eval_to_string() without using current local variables and using
 * textlock.  When "use_sandbox" is TRUE use the sandbox.
 * Use legacy Vim script syntax.
 */
    char_u *
eval_to_string_safe(
    char_u	*arg,
    પૂર્ણાંક		use_sandbox,
    પૂર્ણાંક		keep_script_version,
    પૂર્ણાંક		use_simple_function)
{
    char_u	*retval;
    funccal_entry_T funccal_entry;
    પૂર્ણાંક		save_sc_version = current_sctx.sc_version;
    પૂર્ણાંક		save_garbage = may_garbage_collect;

    જો (!keep_script_version)
	current_sctx.sc_version = 1;
    save_funccal(&funccal_entry);
    જો (use_sandbox)
	++sandbox;
    ++textlock;
    may_garbage_collect = FALSE;
    retval = eval_to_string(arg, FALSE, use_simple_function);
    જો (use_sandbox)
	--sandbox;
    --textlock;
    may_garbage_collect = save_garbage;
    restore_funccal();
    current_sctx.sc_version = save_sc_version;
    પાછા retval;
}

/*
 * Top level evaluation function, returning a number.
 * Evaluates "expr" silently.
 * Returns -1 for an error.
 */
    varnumber_T
eval_to_સંખ્યા(char_u *પદાવલિ, પૂર્ણાંક use_simple_function)
{
    typval_T	rettv;
    varnumber_T	retval;
    char_u	*p = skipwhite(પદાવલિ);
    પૂર્ણાંક		r = NOTDONE;

    ++emsg_off;

    જો (use_simple_function)
	r = may_call_simple_func(પદાવલિ, &rettv);
    જો (r == NOTDONE)
	r = eval1(&p, &rettv, &EVALARG_EVALUATE);
    જો (r == FAIL)
	retval = -1;
    નહીંતર
    {
	retval = tv_get_સંખ્યા_chk(&rettv, ખાલી);
	clear_tv(&rettv);
    }
    --emsg_off;

    પાછા retval;
}

/*
 * Top level evaluation function.
 * Returns an allocated typval_T with the result.
 * Returns NULL when there is an error.
 */
    typval_T *
eval_પદાવલિ(char_u *arg, exarg_T *eap)
{
    પાછા eval_પદાવલિ_ext(arg, eap, FALSE);
}

    typval_T *
eval_પદાવલિ_ext(char_u *arg, exarg_T *eap, પૂર્ણાંક use_simple_function)
{
    typval_T	*tv;
    evalarg_T	evalarg;

    fill_evalarg_from_eap(&evalarg, eap, eap != ખાલી && eap->skip);

    tv = ALLOC_ONE(typval_T);
    જો (tv != ખાલી)
    {
	પૂર્ણાંક	r = NOTDONE;

	જો (use_simple_function)
	    r = eval0_simple_funccal(arg, tv, eap, &evalarg);
	જો (r == NOTDONE)
	    r = eval0(arg, tv, eap, &evalarg);

	જો (r == FAIL)
	    VIM_CLEAR(tv);
    }

    clear_evalarg(&evalarg, eap);
    પાછા tv;
}

/*
 * "*arg" points to what can be a function name in the form of "import.Name" or
 * "Funcref".  Return the name of the function.  Set "tofree" to something that
 * was allocated.
 * If "verbose" is FALSE no errors are given.
 * Return NULL for any failure.
 */
    સ્થિર char_u *
deref_function_name(
	    char_u	**arg,
	    char_u	**tofree,
	    evalarg_T	*evalarg,
	    પૂર્ણાંક		verbose)
{
    typval_T	ref;
    char_u	*name = *arg;
    પૂર્ણાંક		save_flags = 0;
    પૂર્ણાંક		evaluate = evalarg != ખાલી
				      && (evalarg->eval_flags & EVAL_EVALUATE);

    ref.v_type = VAR_UNKNOWN;
    જો (evalarg != ખાલી)
    {
	// need to evaluate this to get an import, like in "a.Func"
	save_flags = evalarg->eval_flags;
	evalarg->eval_flags |= EVAL_EVALUATE;
    }
    જો (eval9(arg, &ref, evalarg, FALSE) == FAIL)
    {
	dictitem_T	*v;

	// If <SID>VarName was used it would not be found, try another way.
	v = find_var_also_in_script(name, ખાલી, FALSE);
	જો (v == ખાલી)
	{
	    name = ખાલી;
	    જાઓ theend;
	}
	copy_tv(&v->di_tv, &ref);
    }
    જો (*skipwhite(*arg) != NUL)
    {
	જો (verbose)
	    semsg(_(e_trailing_characters_str), *arg);
	name = ખાલી;
    }
    નહીંતર જો (ref.v_type == VAR_FUNC && ref.vval.v_string != ખાલી)
    {
	name = ref.vval.v_string;
	ref.vval.v_string = ખાલી;
	*tofree = name;
    }
    નહીંતર જો (ref.v_type == VAR_PARTIAL && ref.vval.v_partial != ખાલી)
    {
	જો (ref.vval.v_partial->pt_argc > 0
		|| ref.vval.v_partial->pt_dict != ખાલી)
	{
	    જો (verbose)
		emsg(_(e_cannot_use_partial_here));
	    name = ખાલી;
	}
	નહીંતર
	{
	    name = vim_strsave(partial_name(ref.vval.v_partial));
	    *tofree = name;
	}
    }
    નહીંતર જો (evaluate)
    {
	જો (verbose)
	    semsg(_(e_not_callable_type_str), name);
	name = ખાલી;
    }

theend:
    clear_tv(&ref);
    જો (evalarg != ખાલી)
	evalarg->eval_flags = save_flags;
    પાછા name;
}

/*
 * Call some Vim script function and return the result in "*rettv".
 * Uses argv[0] to argv[argc - 1] for the function arguments.  argv[argc]
 * should have type VAR_UNKNOWN.
 * Returns OK or FAIL.
 */
    પૂર્ણાંક
call_vim_function(
    char_u      *func,
    પૂર્ણાંક		argc,
    typval_T	*argv,
    typval_T	*rettv)
{
    પૂર્ણાંક		ret;
    funcexe_T	funcexe;
    char_u	*arg;
    char_u	*name;
    char_u	*tofree = ખાલી;
    પૂર્ણાંક		ignore_errors;

    rettv->v_type = VAR_UNKNOWN;		// clear_tv() uses this
    CLEAR_FIELD(funcexe);
    funcexe.fe_firstline = curwin->w_cursor.lnum;
    funcexe.fe_lastline = curwin->w_cursor.lnum;
    funcexe.fe_evaluate = TRUE;

    // The name might be "import.Func" or "Funcref".  We don't know, we need to
    // ignore errors for an undefined name.  But we do want errors when an
    // autoload script has errors.  Guess that when there is a dot in the name
    // showing errors is the right choice.
    ignore_errors = vim_strchr(func, '.') == ખાલી;
    arg = func;
    જો (ignore_errors)
	++emsg_off;
    name = deref_function_name(&arg, &tofree, &EVALARG_EVALUATE, FALSE);
    જો (ignore_errors)
	--emsg_off;
    જો (name == ખાલી)
	name = func;

    ret = call_func(name, -1, rettv, argc, argv, &funcexe);

    જો (ret == FAIL)
	clear_tv(rettv);
    vim_free(tofree);

    પાછા ret;
}

/*
 * Call Vim script function "func" and return the result as a string.
 * Uses "argv[0]" to "argv[argc - 1]" for the function arguments. "argv[argc]"
 * should have type VAR_UNKNOWN.
 * Returns NULL when calling the function fails.
 */
    શૂન્ય *
call_func_retstr(
    char_u      *func,
    પૂર્ણાંક		argc,
    typval_T	*argv)
{
    typval_T	rettv;
    char_u	*retval;

    જો (call_vim_function(func, argc, argv, &rettv) == FAIL)
	પાછા ખાલી;

    retval = vim_strsave(tv_get_string(&rettv));
    clear_tv(&rettv);
    પાછા retval;
}

/*
 * Call Vim script function "func" and return the result as a List.
 * Uses "argv" and "argc" as call_func_retstr().
 * Returns NULL when there is something wrong.
 * Gives an error when the returned value is not a list.
 */
    શૂન્ય *
call_func_retlist(
    char_u      *func,
    પૂર્ણાંક		argc,
    typval_T	*argv)
{
    typval_T	rettv;

    જો (call_vim_function(func, argc, argv, &rettv) == FAIL)
	પાછા ખાલી;

    જો (rettv.v_type != VAR_LIST)
    {
	semsg(_(e_custom_list_completion_function_does_not_return_list_but_str),
		vartype_name(rettv.v_type));
	clear_tv(&rettv);
	પાછા ખાલી;
    }

    પાછા rettv.vval.v_list;
}

#જો defined(FEAT_FOLDING)
/*
 * Evaluate "arg", which is 'foldexpr'.
 * Note: caller must set "curwin" to match "arg".
 * Returns the foldlevel, and any character preceding it in "*cp".  Doesn't
 * give error messages.
 */
    પૂર્ણાંક
eval_foldexpr(win_T *wp, પૂર્ણાંક *cp)
{
    char_u	*arg;
    typval_T	tv;
    varnumber_T	retval;
    char_u	*s;
    sctx_T	saved_sctx = current_sctx;
    પૂર્ણાંક		use_sandbox = was_set_insecurely(wp, (char_u *)"foldexpr",
								    OPT_LOCAL);

    arg = skipwhite(wp->w_p_fde);
    current_sctx = wp->w_p_script_ctx[WV_FDE];

    ++emsg_off;
    જો (use_sandbox)
	++sandbox;
    ++textlock;
    *cp = NUL;

    // Evaluate the expression.  If the expression is "FuncName()" call the
    // function directly.
    જો (eval0_simple_funccal(arg, &tv, ખાલી, &EVALARG_EVALUATE) == FAIL)
	retval = 0;
    નહીંતર
    {
	// If the result is a number, just return the number.
	જો (tv.v_type == VAR_સંખ્યા)
	    retval = tv.vval.v_સંખ્યા;
	નહીંતર જો (tv.v_type != VAR_STRING || tv.vval.v_string == ખાલી)
	    retval = 0;
	નહીંતર
	{
	    // If the result is a string, check if there is a non-digit before
	    // the number.
	    s = tv.vval.v_string;
	    જો (*s != NUL && !VIM_ISDIGIT(*s) && *s != '-')
		*cp = *s++;
	    retval = atol((અક્ષર *)s);
	}
	clear_tv(&tv);
    }
    --emsg_off;
    જો (use_sandbox)
	--sandbox;
    --textlock;
    clear_evalarg(&EVALARG_EVALUATE, ખાલી);
    current_sctx = saved_sctx;

    પાછા (પૂર્ણાંક)retval;
}
#endif

#ifdef LOG_LOCKVAR
પ્રકારવ્યાખ્યા સંરચના flag_string_S
{
    પૂર્ણાંક	    flag;
    અક્ષર    *str;
} flag_string_T;

    સ્થિર અક્ષર *
flags_tostring(પૂર્ણાંક flags, flag_string_T *_fstring, અક્ષર *buf, size_t n)
{
    અક્ષર *p = buf;
    *p = NUL;
    માટે (flag_string_T *fstring = _fstring; fstring->flag; ++fstring)
    {
	જો ((fstring->flag & flags) != 0)
	{
	    size_t len = STRLEN(fstring->str);
	    જો (n > p - buf + len + 7)
	    {
		STRCAT(p, fstring->str);
		p += len;
		STRCAT(p, " ");
		++p;
	    }
	    નહીંતર
	    {
		STRCAT(buf, "...");
		તોડો;
	    }
	}
    }
    પાછા buf;
}

flag_string_T glv_flag_strings[] = {
    { GLV_QUIET,		"QUIET" },
    { GLV_NO_AUTOLOAD,		"NO_AUTOLOAD" },
    { GLV_READ_ONLY,		"READ_ONLY" },
    { GLV_NO_DECL,		"NO_DECL" },
    { GLV_COMPILING,		"COMPILING" },
    { GLV_ASSIGN_WITH_OP,	"ASSIGN_WITH_OP" },
    { GLV_PREFER_FUNC,		"PREFER_FUNC" },
    { 0,			ખાલી }
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
    સ્થિર શૂન્ય
fill_lval_from_lval_મૂળ(lval_T *lp, lval_મૂળ_T *lr)
{
#ifdef LOG_LOCKVAR
    ch_log(ખાલી, "LKVAR: fill_lval_from_lval_root(): name %s, tv %p",
						lp->ll_name, (શૂન્ય*)lr->lr_tv);
#endif
    જો (lr->lr_tv == ખાલી)
	પાછા;
    જો (!lr->lr_is_arg && lr->lr_tv->v_type == VAR_CLASS)
    {
	જો (lr->lr_tv->vval.v_class != ખાલી)
	{
	    // Special special case. Look for a bare class variable reference.
	    class_T	*cl = lr->lr_tv->vval.v_class;
	    પૂર્ણાંક		m_idx;
	    ocmember_T	*m = class_member_lookup(cl, lp->ll_name,
					lp->ll_name_end - lp->ll_name, &m_idx);
	    જો (m != ખાલી)
	    {
		// Assuming "inside class" since bare reference.
		lp->ll_class = lr->lr_tv->vval.v_class;
		lp->ll_oi = m_idx;
		lp->ll_valtype = m->ocm_type;
		lp->ll_tv = &lp->ll_class->class_members_tv[m_idx];
#ifdef LOG_LOCKVAR
		ch_log(ખાલી, "LKVAR:    ... class member %s.%s",
					lp->ll_class->class_name.string, lp->ll_name);
#endif
		પાછા;
	    }
	}
    }

#ifdef LOG_LOCKVAR
    ch_log(ખાલી, "LKVAR:    ... type: %s", vartype_name(lr->lr_tv->v_type));
#endif
    lp->ll_tv = lr->lr_tv;
    lp->ll_is_મૂળ = TRUE;
}

/*
 * Check if the class has permission to access the member.
 * Returns OK or FAIL.
 */
    સ્થિર પૂર્ણાંક
get_lval_check_access(
    class_T	*cl_exec,   // executing class, NULL if :def or script level
    class_T	*cl,	    // class which contains the member
    ocmember_T	*om,	    // member being accessed
    char_u	*p,	    // char after member name
    પૂર્ણાંક		flags)	    // GLV flags to check if writing to lval
{
#ifdef LOG_LOCKVAR
    ch_log(ખાલી, "LKVAR: get_lval_check_access(), cl_exec %p, cl %p, %c",
						(શૂન્ય*)cl_exec, (શૂન્ય*)cl, *p);
#endif
    જો (cl_exec != ખાલી && cl_exec == cl)
	પાછા OK;

    અક્ષર *સંદેશ = ખાલી;
    પસંદગી (om->ocm_access)
    {
	સ્થિતિ VIM_ACCESS_PRIVATE:
	    સંદેશ = e_cannot_access_protected_variable_str;
	    તોડો;
	સ્થિતિ VIM_ACCESS_READ:
	    // If [idx] or .key following, read only OK.
	    જો (*p == '[' || *p == '.')
		તોડો;
	    જો ((flags & GLV_READ_ONLY) == 0)
	    {
		જો (IS_ENUM(cl))
		{
		    જો (om->ocm_type->tt_type == VAR_OBJECT)
			semsg(_(e_enumvalue_str_cannot_be_modified),
				cl->class_name.string, om->ocm_name.string);
		    નહીંતર
			સંદેશ = e_variable_is_not_writable_str;
		}
		નહીંતર
		    સંદેશ = e_variable_is_not_writable_str;
	    }
	    તોડો;
	સ્થિતિ VIM_ACCESS_ALL:
	    તોડો;
    }
    જો (સંદેશ != ખાલી)
    {
	emsg_var_cl_define(સંદેશ, om->ocm_name.string, 0, cl);
	પાછા FAIL;
    }
    પાછા OK;
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
    સ્થિર char_u *
get_lval_imported(
    lval_T	*lp,
    scid_T	imp_sid,
    char_u	*p,
    dictitem_T	**dip,
    પૂર્ણાંક		fne_flags)
{
    ufunc_T	*ufunc;
    type_T	*type = ખાલી;
    પૂર્ણાંક		cc;
    પૂર્ણાંક		rc = FAIL;

    p = skipwhite(p);

    import_check_sourced_sid(&imp_sid);
    lp->ll_sid = imp_sid;
    lp->ll_name = p;
    p = find_name_end(lp->ll_name, ખાલી, ખાલી, fne_flags);
    lp->ll_name_end = p;

    // check the item is exported
    cc = *p;
    *p = NUL;
    જો (find_exported(imp_sid, lp->ll_name, &ufunc, &type, ખાલી, ખાલી,
								TRUE) == -1)
	જાઓ failed;

    // Get the typval for the exported item
    hashtab_T *ht = &SCRIPT_VARS(imp_sid);
    જો (ht == ખાલી)
	જાઓ failed;

    dictitem_T *di = find_var_in_ht(ht, 0, lp->ll_name, TRUE);
    જો (di == ખાલી)
	// script is autoloaded.  So variable will be found later
	જાઓ success;

    *dip = di;

    // Check whether the variable is writable.
    svar_T *sv = find_typval_in_script(&di->di_tv, imp_sid, FALSE);
    જો (sv != ખાલી && sv->sv_const != 0)
    {
	semsg(_(e_cannot_change_readonly_variable_str), lp->ll_name);
	જાઓ failed;
    }

    // check whether variable is locked
    જો (કિંમત_check_lock(di->di_tv.v_lock, lp->ll_name, FALSE))
	જાઓ failed;

    lp->ll_tv = &di->di_tv;
    lp->ll_valtype = type;

success:
    rc = OK;

failed:
    *p = cc;
    પાછા rc == OK ? p : ખાલી;
}

પ્રકારવ્યાખ્યા ગણના {
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
    સ્થિર પૂર્ણાંક
get_lval_dict_item(
    lval_T	*lp,
    char_u	*name,
    char_u	*ચાવી,
    પૂર્ણાંક		len,
    char_u	**ચાવી_end,
    typval_T	*var1,
    પૂર્ણાંક		flags,
    પૂર્ણાંક		unlet,
    typval_T	*rettv)
{
    પૂર્ણાંક		quiet = flags & GLV_QUIET;
    char_u	*p = *ચાવી_end;

    જો (len == -1)
    {
	// "[key]": get key from "var1"
	ચાવી = tv_get_string_chk(var1);	// is number or string
	જો (ચાવી == ખાલી)
	    પાછા GLV_FAIL;
    }
    lp->ll_list = ખાલી;
    lp->ll_list = ખાલી;
    lp->ll_blob = ખાલી;
    lp->ll_object = ખાલી;
    lp->ll_class = ખાલી;
    lp->ll_tuple = ખાલી;

    // a NULL dict is equivalent with an empty dict
    જો (lp->ll_tv->vval.v_dict == ખાલી)
    {
	lp->ll_tv->vval.v_dict = dict_alloc();
	જો (lp->ll_tv->vval.v_dict == ખાલી)
	    પાછા GLV_FAIL;
	++lp->ll_tv->vval.v_dict->dv_refcount;
    }
    lp->ll_dict = lp->ll_tv->vval.v_dict;

    lp->ll_di = dict_find(lp->ll_dict, ચાવી, len);

    // When assigning to a scope dictionary check that a function and
    // variable name is valid (only variable name unless it is l: or
    // g: dictionary). Disallow overwriting a builtin function.
    જો (rettv != ખાલી && lp->ll_dict->dv_scope != 0)
    {
	પૂર્ણાંક prevval;

	જો (len != -1)
	{
	    prevval = ચાવી[len];
	    ચાવી[len] = NUL;
	}
	નહીંતર
	    prevval = 0; // avoid compiler warning
	પૂર્ણાંક wrong = (lp->ll_dict->dv_scope == VAR_DEF_SCOPE
		&& (rettv->v_type == VAR_FUNC
		    || rettv->v_type == VAR_PARTIAL)
		&& var_wrong_func_name(ચાવી, lp->ll_di == ખાલી))
	    || !valid_varname(ચાવી, -1, TRUE);
	જો (len != -1)
	    ચાવી[len] = prevval;
	જો (wrong)
	    પાછા GLV_FAIL;
    }

    જો (lp->ll_valtype != ખાલી)
	// use the type of the member
	lp->ll_valtype = lp->ll_valtype->tt_member;

    જો (lp->ll_di == ખાલી)
    {
	// Can't add "v:" or "a:" variable.
	જો (lp->ll_dict == get_vimvar_dict()
		|| &lp->ll_dict->dv_hashtab == get_funccal_args_ht())
	{
	    semsg(_(e_illegal_variable_name_str), name);
	    પાછા GLV_FAIL;
	}

	// Key does not exist in dict: may need to add it.
	જો (*p == '[' || *p == '.' || unlet)
	{
	    જો (!quiet)
		semsg(_(e_ચાવી_not_present_in_dictionary_str), ચાવી);
	    પાછા GLV_FAIL;
	}
	જો (len == -1)
	    lp->ll_newkey = vim_strsave(ચાવી);
	નહીંતર
	    lp->ll_newkey = vim_strnsave(ચાવી, len);
	જો (lp->ll_newkey == ખાલી)
	    p = ખાલી;

	*ચાવી_end = p;
	પાછા GLV_STOP;
    }
    // existing variable, need to check if it can be changed
    નહીંતર જો ((flags & GLV_READ_ONLY) == 0
	    && (var_check_ro(lp->ll_di->di_flags, name, FALSE)
		|| var_check_lock(lp->ll_di->di_flags, name, FALSE)))
	પાછા GLV_FAIL;

    lp->ll_tv = &lp->ll_di->di_tv;

    પાછા GLV_OK;
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
    સ્થિર પૂર્ણાંક
get_lval_blob(
    lval_T	*lp,
    typval_T	*var1,
    typval_T	*var2,
    પૂર્ણાંક		empty1,
    પૂર્ણાંક		quiet)
{
    long	bloblen = blob_len(lp->ll_tv->vval.v_blob);

    lp->ll_list = ખાલી;
    lp->ll_dict = ખાલી;
    lp->ll_object = ખાલી;
    lp->ll_class = ખાલી;
    lp->ll_tuple = ખાલી;

    // Get the number and item for the only or first index of a List or Tuple.
    જો (empty1)
	lp->ll_n1 = 0;
    નહીંતર
	// is number or string
	lp->ll_n1 = (long)tv_get_સંખ્યા(var1);

    જો (check_blob_index(bloblen, lp->ll_n1, quiet) == FAIL)
	પાછા FAIL;
    જો (lp->ll_range && !lp->ll_empty2)
    {
	lp->ll_n2 = (long)tv_get_સંખ્યા(var2);
	જો (check_blob_range(bloblen, lp->ll_n1, lp->ll_n2, quiet) == FAIL)
	    પાછા FAIL;
    }

    જો (!lp->ll_range)
	// Indexing a single byte in a blob.  So the rhs type is a
	// number.
	lp->ll_valtype = &t_સંખ્યા;

    lp->ll_blob = lp->ll_tv->vval.v_blob;
    lp->ll_tv = ખાલી;

    પાછા OK;
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
    સ્થિર પૂર્ણાંક
get_lval_list(
    lval_T	*lp,
    typval_T	*var1,
    typval_T	*var2,
    પૂર્ણાંક		empty1,
    પૂર્ણાંક		flags,
    પૂર્ણાંક		quiet)
{
    /*
     * Get the number and item for the only or first index of the List.
     */
    જો (empty1)
	lp->ll_n1 = 0;
    નહીંતર
	// is number or string
	lp->ll_n1 = (long)tv_get_સંખ્યા(var1);

    lp->ll_dict = ખાલી;
    lp->ll_object = ખાલી;
    lp->ll_class = ખાલી;
    lp->ll_tuple = ખાલી;
    lp->ll_list = lp->ll_tv->vval.v_list;
    lp->ll_li = check_range_index_one(lp->ll_list, &lp->ll_n1,
				(flags & GLV_ASSIGN_WITH_OP) == 0, quiet);
    જો (lp->ll_li == ખાલી)
	પાછા FAIL;

    જો (lp->ll_valtype != ખાલી && !lp->ll_range)
    {
	// use the type of the member
	જો (lp->ll_valtype->tt_member != ખાલી)
	    lp->ll_valtype = lp->ll_valtype->tt_member;
	નહીંતર
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
    જો (lp->ll_range && !lp->ll_empty2)
    {
	lp->ll_n2 = (long)tv_get_સંખ્યા(var2);
	// is number or string
	જો (check_range_index_two(lp->ll_list,
			&lp->ll_n1, lp->ll_li, &lp->ll_n2, quiet) == FAIL)
	    પાછા FAIL;
    }

    lp->ll_tv = &lp->ll_li->li_tv;

    પાછા OK;
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
    સ્થિર પૂર્ણાંક
get_lval_tuple(
    lval_T	*lp,
    typval_T	*idx,
    પૂર્ણાંક		quiet)
{
    // is number or string
    lp->ll_n1 = (long)tv_get_સંખ્યા(idx);

    lp->ll_list = ખાલી;
    lp->ll_dict = ખાલી;
    lp->ll_blob = ખાલી;
    lp->ll_object = ખાલી;
    lp->ll_class = ખાલી;

    lp->ll_tuple = lp->ll_tv->vval.v_tuple;
    lp->ll_tv = tuple_find(lp->ll_tuple, lp->ll_n1);
    જો (lp->ll_tv == ખાલી)
    {
	જો (!quiet)
	    semsg(_(e_tuple_index_out_of_range_nr), lp->ll_n1);
	પાછા GLV_FAIL;
    }

    // use the type of the member
    જો (lp->ll_valtype != ખાલી)
    {
	જો (lp->ll_valtype != ખાલી
		&& lp->ll_valtype->tt_type == VAR_TUPLE
		&& lp->ll_valtype->tt_argcount == 1)
	{
	    // a variadic tuple or a single item tuple
	    જો (lp->ll_valtype->tt_flags & TTFLAG_VARARGS)
		lp->ll_valtype = lp->ll_valtype->tt_args[0]->tt_member;
	    નહીંતર
		lp->ll_valtype = lp->ll_valtype->tt_args[0];
	}
	નહીંતર
	    // If the LHS member type is not known (VAR_ANY), then get it from
	    // the tuple item (after indexing)
	    lp->ll_valtype = typval2type(lp->ll_tv, get_copyID(),
					&lp->ll_type_list, TVTT_DO_MEMBER);
    }

    પાછા GLV_OK;
}

/*
 * Get a class or object lval method in class "cl".  The 'key' argument points
 * to the method name and 'key_end' points to the character after 'key'.
 * 'v_type' is VAR_CLASS or VAR_OBJECT.
 *
 * The method index, method function pointer and method type are returned in
 * "lp".
 */
    સ્થિર પૂર્ણાંક
get_lval_oc_method(
    lval_T	*lp,
    class_T	*cl,
    char_u	*ચાવી,
    char_u	**ચાવી_end,
    vartype_T	v_type)
{
    // Look for a method with this name.
    // round 1: class functions (skipped for an object)
    // round 2: object methods
    માટે (પૂર્ણાંક round = v_type == VAR_OBJECT ? 2 : 1; round <= 2; ++round)
    {
	પૂર્ણાંક	m_idx;
	ufunc_T	*fp;

	fp = method_lookup(cl, round == 1 ? VAR_CLASS : VAR_OBJECT,
						ચાવી, *ચાવી_end - ચાવી, &m_idx);
	lp->ll_oi = m_idx;

	// process generic method (if present)
	જો (fp && (fp = eval_generic_func(fp, ચાવી, ચાવી_end)) == ખાલી)
	    પાછા FAIL;

	જો (fp != ખાલી)
	{
	    lp->ll_ufunc = fp;
	    lp->ll_valtype = fp->uf_func_type;
	    તોડો;
	}
    }

    પાછા OK;
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
    સ્થિર પૂર્ણાંક
get_lval_oc_variable(
    lval_T	*lp,
    class_T	*cl,
    char_u	*ચાવી,
    char_u	*ચાવી_end,
    vartype_T	v_type,
    class_T	*cl_exec,
    પૂર્ણાંક		flags)
{
    પૂર્ણાંક		m_idx;
    ocmember_T	*om;

    om = member_lookup(cl, v_type, ચાવી, ચાવી_end - ચાવી, &m_idx);
    lp->ll_oi = m_idx;
    જો (om == ખાલી)
	પાછા OK;

    // Check variable is accessible
    જો (get_lval_check_access(cl_exec, cl, om, ચાવી_end, flags) == FAIL)
	પાછા FAIL;

    // When lhs is used to modify the variable, check it is not a read-only
    // variable.
    જો ((flags & GLV_READ_ONLY) == 0 && (*ચાવી_end != '.' && *ચાવી_end != '[')
	    && oc_var_check_ro(cl, om))
	પાછા FAIL;

    lp->ll_valtype = om->ocm_type;

    જો (v_type == VAR_OBJECT)
	lp->ll_tv = ((typval_T *)(lp->ll_tv->vval.v_object + 1)) + m_idx;
    નહીંતર
	lp->ll_tv = &cl->class_members_tv[m_idx];

    પાછા OK;
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
    સ્થિર પૂર્ણાંક
get_lval_class_or_obj(
    lval_T	*lp,
    char_u	*ચાવી,
    char_u	**ચાવી_end,
    vartype_T	v_type,
    class_T	*cl_exec,
    પૂર્ણાંક		flags,
    પૂર્ણાંક		quiet)
{
    lp->ll_dict = ખાલી;
    lp->ll_list = ખાલી;
    lp->ll_tuple = ખાલી;

    class_T *cl;
    જો (v_type == VAR_OBJECT)
    {
	જો (lp->ll_tv->vval.v_object == ખાલી)
	{
	    જો (!quiet)
		emsg(_(e_using_null_object));
	    પાછા FAIL;
	}
	cl = lp->ll_tv->vval.v_object->obj_class;
	lp->ll_object = lp->ll_tv->vval.v_object;
    }
    નહીંતર
    {
	cl = lp->ll_tv->vval.v_class;
	lp->ll_object = ખાલી;
    }
    lp->ll_class = cl;

    જો (cl == ખાલી)
	// TODO: what if class is NULL?
	પાછા OK;

    lp->ll_valtype = ખાલી;

    જો (flags & GLV_PREFER_FUNC)
	જો (get_lval_oc_method(lp, cl, ચાવી, ચાવી_end, v_type) == FAIL)
	    પાછા FAIL;

    // Look for object/class member variable
    જો (lp->ll_valtype == ખાલી)
    {
	જો (get_lval_oc_variable(lp, cl, ચાવી, *ચાવી_end, v_type, cl_exec, flags)
								== FAIL)
	    પાછા FAIL;
    }

    જો (lp->ll_valtype == ખાલી)
    {
	member_not_found_સંદેશ(cl, v_type, ચાવી, *ચાવી_end - ચાવી);
	પાછા FAIL;
    }

    પાછા OK;
}

/*
 * Check whether dot (".") is allowed after the variable "name" with type
 * "v_type".  Only Dict, Class and Object types support a dot after the name.
 * Returns TRUE if dot is allowed after the name.
 */
    સ્થિર પૂર્ણાંક
dot_allowed_after_type(char_u *name, vartype_T v_type, પૂર્ણાંક quiet)
{
    જો (v_type != VAR_DICT && v_type != VAR_OBJECT && v_type != VAR_CLASS)
    {
	જો (!quiet)
	    semsg(_(e_dot_not_allowed_after_str_str),
		    vartype_name(v_type), name);
	પાછા FALSE;
    }

    પાછા TRUE;
}

/*
 * Check whether left bracket ("[") is allowed after the variable "name" with
 * type "v_type".  Only Dict, List, Tuple and Blob types support a bracket
 * after the variable name.  Returns TRUE if bracket is allowed after the name.
 */
    સ્થિર પૂર્ણાંક
bracket_allowed_after_type(char_u *name, vartype_T v_type, પૂર્ણાંક quiet)
{
    જો (v_type == VAR_CLASS || v_type == VAR_OBJECT)
    {
	જો (!quiet)
	    semsg(_(e_index_not_allowed_after_str_str),
		    vartype_name(v_type), name);
	પાછા FALSE;
    }

    પાછા TRUE;
}

/*
 * Check whether the variable "name" with type "v_type" can be followed by an
 * index.  Only Dict, List, Tuple, Blob, Object and Class types support
 * indexing.  Returns TRUE if indexing is allowed after the name.
 */
    સ્થિર પૂર્ણાંક
index_allowed_after_type(char_u *name, vartype_T v_type, પૂર્ણાંક quiet)
{
    જો (v_type != VAR_LIST
	    && v_type != VAR_TUPLE
	    && v_type != VAR_DICT
	    && v_type != VAR_BLOB
	    && v_type != VAR_OBJECT
	    && v_type != VAR_CLASS)
    {
	જો (!quiet)
	    semsg(_(e_index_not_allowed_after_str_str),
		    vartype_name(v_type), name);
	પાછા FALSE;
    }

    પાછા TRUE;
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
    સ્થિર char_u *
get_lval_subscript(
    lval_T	*lp,
    char_u	*p,
    char_u	*name,
    typval_T	*rettv,
    hashtab_T	*ht,
    dictitem_T	*v,
    પૂર્ણાંક		unlet,
    પૂર્ણાંક		flags,	    // GLV_ values
    class_T	*cl_exec)
{
    પૂર્ણાંક		quiet = flags & GLV_QUIET;
    char_u	*ચાવી = ખાલી;
    પૂર્ણાંક		len;
    typval_T	var1;
    typval_T	var2;
    પૂર્ણાંક		empty1 = FALSE;
    પૂર્ણાંક		rc = FAIL;

    /*
     * Loop until no more [idx] or .key is following.
     */
    var1.v_type = VAR_UNKNOWN;
    var2.v_type = VAR_UNKNOWN;

    જ્યારે (*p == '[' || (*p == '.' && p[1] != '=' && p[1] != '.'))
    {
	vartype_T v_type = lp->ll_tv->v_type;

	જો (*p == '.' && !dot_allowed_after_type(name, v_type, quiet))
	    જાઓ done;

	જો (*p == '[' && !bracket_allowed_after_type(name, v_type, quiet))
	    જાઓ done;

	જો (!index_allowed_after_type(name, v_type, quiet))
	    જાઓ done;

	// A NULL list/blob works like an empty list/blob, allocate one now.
	પૂર્ણાંક r = OK;
	જો (v_type == VAR_LIST && lp->ll_tv->vval.v_list == ખાલી)
	    r = rettv_list_alloc(lp->ll_tv);
	નહીંતર જો (v_type == VAR_BLOB && lp->ll_tv->vval.v_blob == ખાલી)
	    r = rettv_blob_alloc(lp->ll_tv);
	જો (r == FAIL)
	    જાઓ done;

	જો (lp->ll_range)
	{
	    જો (!quiet)
		emsg(_(e_slice_must_come_last));
	    જાઓ done;
	}
#ifdef LOG_LOCKVAR
	ch_log(ખાલી, "LKVAR: get_lval() loop: p: %s, type: %s", p,
		vartype_name(v_type));
#endif

	જો (current_script_is_vim9() && lp->ll_valtype == ખાલી
		&& v != ખાલી
		&& lp->ll_tv == &v->di_tv
		&& ht != ખાલી && ht == get_script_local_ht())
	{
	    svar_T  *sv = find_typval_in_script(lp->ll_tv, 0, TRUE);

	    // Vim9 script local variable: get the type
	    જો (sv != ખાલી)
	    {
		lp->ll_valtype = sv->sv_type;
#ifdef LOG_LOCKVAR
		ch_log(ખાલી, "LKVAR:    ... loop: vim9 assign type: %s",
			vartype_name(lp->ll_valtype->tt_type));
#endif
	    }
	}

	len = -1;
	જો (*p == '.')
	{
	    ચાવી = p + 1;

	    માટે (len = 0; ASCII_ISALNUM(ચાવી[len]) || ચાવી[len] == '_'; ++len)
		;
	    જો (len == 0)
	    {
		જો (!quiet)
		    emsg(_(e_cannot_use_empty_ચાવી_for_dictionary));
		જાઓ done;
	    }
	    p = ચાવી + len;
	}
	નહીંતર
	{
	    // Get the index [expr] or the first index [expr: ].
	    p = skipwhite(p + 1);
	    જો (*p == ':')
		empty1 = TRUE;
	    નહીંતર
	    {
		empty1 = FALSE;
		જો (eval1(&p, &var1, &EVALARG_EVALUATE) == FAIL)  // recursive!
		    જાઓ done;
		જો (tv_get_string_chk(&var1) == ખાલી)
		    // not a number or string
		    જાઓ done;
		p = skipwhite(p);
	    }

	    // Optionally get the second index [ :expr].
	    જો (*p == ':')
	    {
		જો (v_type == VAR_DICT)
		{
		    જો (!quiet)
			emsg(_(e_cannot_slice_dictionary));
		    જાઓ done;
		}
		જો (v_type == VAR_TUPLE)
		{
		    જો (!quiet)
			emsg(_(e_cannot_slice_tuple));
		    જાઓ done;
		}
		જો (rettv != ખાલી
			&& !(rettv->v_type == VAR_LIST
			    && rettv->vval.v_list != ખાલી)
			&& !(rettv->v_type == VAR_BLOB
			    && rettv->vval.v_blob != ખાલી))
		{
		    જો (!quiet)
			emsg(_(e_slice_requires_list_or_blob_કિંમત));
		    જાઓ done;
		}
		p = skipwhite(p + 1);
		જો (*p == ']')
		    lp->ll_empty2 = TRUE;
		નહીંતર
		{
		    lp->ll_empty2 = FALSE;
		    // recursive!
		    જો (eval1(&p, &var2, &EVALARG_EVALUATE) == FAIL)
			જાઓ done;
		    જો (tv_get_string_chk(&var2) == ખાલી)
			// not a number or string
			જાઓ done;
		}
		lp->ll_range = TRUE;
	    }
	    નહીંતર
		lp->ll_range = FALSE;

	    જો (*p != ']')
	    {
		જો (!quiet)
		    emsg(_(e_missing_closing_square_brace));
		જાઓ done;
	    }

	    // Skip to past ']'.
	    ++p;
	}
#ifdef LOG_LOCKVAR
	જો (len == -1)
	    ch_log(ખાલી, "LKVAR:    ... loop: p: %s, '[' key: %s", p,
		    empty1 ? ":" : (અક્ષર*)tv_get_string(&var1));
	નહીંતર
	    ch_log(ખાલી, "LKVAR:    ... loop: p: %s, '.' key: %s", p, ચાવી);
#endif

	જો (v_type == VAR_DICT)
	{
	    glv_status_T glv_status;

	    glv_status = get_lval_dict_item(lp, name, ચાવી, len, &p, &var1,
							flags, unlet, rettv);
	    જો (glv_status == GLV_FAIL)
		જાઓ done;
	    જો (glv_status == GLV_STOP)
		તોડો;
	}
	નહીંતર જો (v_type == VAR_BLOB)
	{
	    જો (get_lval_blob(lp, &var1, &var2, empty1, quiet) == FAIL)
		જાઓ done;

	    તોડો;
	}
	નહીંતર જો (v_type == VAR_LIST)
	{
	    જો (get_lval_list(lp, &var1, &var2, empty1, flags, quiet) == FAIL)
		જાઓ done;
	}
	નહીંતર જો (v_type == VAR_TUPLE)
	{
	    જો (get_lval_tuple(lp, &var1, quiet) == FAIL)
		જાઓ done;
	}
	નહીંતર  // v_type == VAR_CLASS || v_type == VAR_OBJECT
	{
	    જો (get_lval_class_or_obj(lp, ચાવી, &p, v_type, cl_exec, flags,
							quiet) == FAIL)
		જાઓ done;
	}

	clear_tv(&var1);
	clear_tv(&var2);
	var1.v_type = VAR_UNKNOWN;
	var2.v_type = VAR_UNKNOWN;
    }

    જો (lp->ll_tuple != ખાલી && (flags & GLV_READ_ONLY) == 0)
    {
	જો (!quiet)
	    emsg(_(e_tuple_is_immutable));
	જાઓ done;
    }

    rc = OK;

done:
    clear_tv(&var1);
    clear_tv(&var2);
    પાછા rc == OK ? p : ખાલી;
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
    પૂર્ણાંક		unlet,
    પૂર્ણાંક		skip,
    પૂર્ણાંક		flags,	    // GLV_ values
    પૂર્ણાંક		fne_flags)  // flags for find_name_end()
{
    char_u	*p;
    char_u	*પદાવલિ_start, *પદાવલિ_end;
    પૂર્ણાંક		cc;
    dictitem_T	*v = ખાલી;
    hashtab_T	*ht = ખાલી;
    પૂર્ણાંક		quiet = flags & GLV_QUIET;
    પૂર્ણાંક		writing = 0;
    પૂર્ણાંક		vim9script = in_vim9script();
    class_T	*cl_exec = ખાલી;    // class that is executing, or NULL.

#ifdef LOG_LOCKVAR
    જો (lval_મૂળ == ખાલી)
	ch_log(ખાલી, "LKVAR: get_lval(): name: %s, lval_root (nil)", name);
    નહીંતર
	ch_log(ખાલી, "LKVAR: get_lval(): name: %s, lr_tv %p lr_is_arg %d",
			name, (શૂન્ય*)lval_મૂળ->lr_tv, lval_મૂળ->lr_is_arg);
    અક્ષર buf[80];
    ch_log(ખાલી, "LKVAR:    ...: GLV flags: %s",
		    flags_tostring(flags, glv_flag_strings, buf, કદ(buf)));
#endif

    // Clear everything in "lp".
    CLEAR_POINTER(lp);
    ga_init2(&lp->ll_type_list, કદ(type_T *), 10);

    જો (skip || (flags & GLV_COMPILING))
    {
	// When skipping or compiling just find the end of the name.
	lp->ll_name = name;
	lp->ll_name_end = find_name_end(name, ખાલી, ખાલી,
						      FNE_INCL_BR | fne_flags);
	પાછા lp->ll_name_end;
    }

    // Cannot use "s:var" at the Vim9 script level.  "s: type" is OK.
    જો (vim9script && at_script_level()
		  && name[0] == 's' && name[1] == ':' && !VIM_ISWHITE(name[2]))
    {
	semsg(_(e_cannot_use_s_colon_in_vim9_script_str), name);
	પાછા ખાલી;
    }

    // Find the end of the name.
    p = find_name_end(name, &પદાવલિ_start, &પદાવલિ_end, fne_flags);
    lp->ll_name_end = p;
    જો (પદાવલિ_start != ખાલી)
    {
	// Don't expand the name when we already know there is an error.
	જો (unlet && !VIM_ISWHITE(*p) && !ends_excmd(*p)
						    && *p != '[' && *p != '.')
	{
	    semsg(_(e_trailing_characters_str), p);
	    પાછા ખાલી;
	}

	lp->ll_exp_name = make_expanded_name(name, પદાવલિ_start, પદાવલિ_end, p);
	જો (lp->ll_exp_name == ખાલી)
	{
	    // Report an invalid expression in braces, unless the
	    // expression evaluation has been cancelled due to an
	    // aborting error, an interrupt, or an exception.
	    જો (!aborting() && !quiet)
	    {
		emsg_severe = TRUE;
		semsg(_(e_invalid_argument_str), name);
		પાછા ખાલી;
	    }
	}
	lp->ll_name = lp->ll_exp_name;
    }
    નહીંતર
    {
	lp->ll_name = name;

	જો (vim9script)
	{
	    // "a: type" is declaring variable "a" with a type, not "a:".
	    // However, "g:[key]" is indexing a dictionary.
	    જો (p == name + 2 && p[-1] == ':' && *p != '[')
	    {
		--p;
		lp->ll_name_end = p;
	    }
	    જો (*skipwhite(p) == ':')
	    {
		char_u	    *tp = skipwhite(p + 1);

		જો (is_scoped_variable(name))
		{
		    semsg(_(e_cannot_use_type_with_this_variable_str), name);
		    પાછા ખાલી;
		}
		જો (VIM_ISWHITE(*p))
		{
		    semsg(_(e_no_white_space_allowed_before_colon_str), p);
		    પાછા ખાલી;
		}
		જો (tp == p + 1 && !quiet)
		{
		    semsg(_(e_white_space_required_after_str_str), ":", p);
		    પાછા ખાલી;
		}
		જો (!SCRIPT_ID_VALID(current_sctx.sc_sid))
		{
		    semsg(_(e_using_type_not_in_script_context_str), p);
		    પાછા ખાલી;
		}
		જો (vim9script && (flags & GLV_NO_DECL) &&
			!(flags & GLV_FOR_LOOP))
		{
		    // Using a type and not in a "var" declaration.
		    semsg(_(e_trailing_characters_str), p);
		    પાછા ખાલી;
		}


		// parse the type after the name
		lp->ll_type = parse_type(&tp,
			       &SCRIPT_ITEM(current_sctx.sc_sid)->sn_type_list,
			       ખાલી, ખાલી, !quiet);
		જો (!quiet && (lp->ll_type == ખાલી
			    || !valid_declaration_type(lp->ll_type)))
		    પાછા ખાલી;
		lp->ll_name_end = tp;
	    }
	    // TODO: check inside class?
	}
    }
    જો (lp->ll_name == ખાલી)
	પાછા p;

    જો (*p == '.')
    {
	// In legacy script, when a local variable and import exists with this name,
	// prioritize local variable over imports to avoid conflicts.
	પૂર્ણાંક var_exists = FALSE;
	જો (!vim9script)
	{
	    cc = *p;
	    *p = NUL;
	    hashtab_T *local_ht = get_funccal_local_ht();
	    જો (local_ht != ખાલી)
	    {
		hashitem_T *hi = hash_find(local_ht, lp->ll_name);
		જો (!HASHITEM_EMPTY(hi))
		    var_exists = TRUE;
	    }
	    *p = cc;
	}

	જો (!var_exists)
	{
	    imported_T *import = find_imported(lp->ll_name, p - lp->ll_name, TRUE);
	    જો (import != ખાલી)
	    {
		p++;	// skip '.'
		p = get_lval_imported(lp, import->imp_sid, p, &v, fne_flags);
		જો (p == ખાલી)
		    પાછા ખાલી;
	    }
	}
    }

    // Without [idx] or .key we are done.
    જો (*p != '[' && *p != '.')
    {
	જો (lval_મૂળ != ખાલી)
	    fill_lval_from_lval_મૂળ(lp, lval_મૂળ);
	પાછા p;
    }

    જો (vim9script && lval_મૂળ != ખાલી)
	cl_exec = lval_મૂળ->lr_cl_exec;
    જો (vim9script && lval_મૂળ != ખાલી && lval_મૂળ->lr_tv != ખાલી)
    {
	// using local variable
	lp->ll_tv = lval_મૂળ->lr_tv;
	v = ખાલી;
    }
    નહીંતર જો (lp->ll_tv == ખાલી)
    {
	cc = *p;
	*p = NUL;
	// When we would write to the variable pass &ht and prevent autoload.
	writing = !(flags & GLV_READ_ONLY);
	v = find_var(lp->ll_name, writing ? &ht : ખાલી,
					 (flags & GLV_NO_AUTOLOAD) || writing);
	જો (v == ખાલી && !quiet)
	    semsg(_(e_undefined_variable_str), lp->ll_name);
	*p = cc;
	જો (v == ખાલી)
	    પાછા ખાલી;
	lp->ll_tv = &v->di_tv;
    }

    જો (vim9script && (flags & GLV_NO_DECL) == 0)
    {
	જો (!quiet)
	    semsg(_(e_variable_already_declared_str), lp->ll_name);
	પાછા ખાલી;
    }

    // If the next character is a "." or a "[", then process the subitem.
    p = get_lval_subscript(lp, p, name, rettv, ht, v, unlet, flags, cl_exec);
    જો (p == ખાલી)
	પાછા ખાલી;

    જો (vim9script && lp->ll_valtype != ખાલી && rettv != ખાલી)
    {
	where_T	    where = WHERE_INIT;

	// In a Vim9 script, do type check and make sure the variable is
	// writable.
	જો (check_typval_type(lp->ll_valtype, rettv, where) == FAIL)
	    પાછા ખાલી;
    }

    lp->ll_name_end = p;
    પાછા p;
}

/*
 * Clear lval "lp" that was filled by get_lval().
 */
    શૂન્ય
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
    શૂન્ય
set_var_lval(
    lval_T	*lp,
    char_u	*endp,
    typval_T	*rettv,
    પૂર્ણાંક		copy,
    પૂર્ણાંક		flags,	    // ASSIGN_CONST, ASSIGN_NO_DECL
    char_u	*op,
    પૂર્ણાંક		var_idx)    // index for "let [a, b] = list"
{
    પૂર્ણાંક		cc;
    dictitem_T	*di;

    જો (lp->ll_tv == ખાલી)
    {
	cc = *endp;
	*endp = NUL;
	જો (in_vim9script() && check_reserved_name(lp->ll_name, FALSE) == FAIL)
	    પાછા;

	જો (lp->ll_blob != ખાલી)
	{
	    પૂર્ણાંક	    error = FALSE;

	    જો (op != ખાલી && *op != '=')
	    {
		semsg(_(e_wrong_variable_type_for_str_equal), op);
		પાછા;
	    }
	    જો (કિંમત_check_lock(lp->ll_blob->bv_lock, lp->ll_name, FALSE))
		પાછા;

	    જો (lp->ll_range && rettv->v_type == VAR_BLOB)
	    {
		જો (lp->ll_empty2)
		    lp->ll_n2 = blob_len(lp->ll_blob) - 1;

		જો (blob_set_range(lp->ll_blob, lp->ll_n1, lp->ll_n2,
								rettv) == FAIL)
		    પાછા;
	    }
	    નહીંતર
	    {
		varnumber_T	val = tv_get_સંખ્યા_chk(rettv, &error);
		જો (!error)
		{
		    જો (val < 0 || val > 255)
			semsg(_(e_invalid_કિંમત_for_blob_nr), val);
		    નહીંતર
			blob_set_append(lp->ll_blob, lp->ll_n1, val);
		}
	    }
	}
	નહીંતર જો (op != ખાલી && *op != '=')
	{
	    typval_T tv;

	    જો ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					     && (flags & ASSIGN_FOR_LOOP) == 0)
	    {
		emsg(_(e_cannot_modify_existing_variable));
		*endp = cc;
		પાછા;
	    }

	    // handle +=, -=, *=, /=, %= and .=
	    di = ખાલી;
	    જો (eval_variable(lp->ll_name, (પૂર્ણાંક)STRLEN(lp->ll_name),
				 lp->ll_sid, &tv, &di, EVAL_VAR_VERBOSE) == OK)
	    {
		જો (di != ખાલી && check_typval_is_કિંમત(&di->di_tv) == FAIL)
		{
		    clear_tv(&tv);
		    પાછા;
		}

		// In Vim9 script a String v: variable only takes a String.
		જો (in_vim9script() && STRNCMP(lp->ll_name, "v:", 2) == 0
			&& tv.v_type == VAR_STRING)
		{
		    where_T	where = WHERE_INIT;

		    જો (check_typval_type(&t_string, rettv, where) == FAIL)
		    {
			clear_tv(&tv);
			*endp = cc;
			પાછા;
		    }
		}

		જો ((di == ખાલી
			 || (!var_check_ro(di->di_flags, lp->ll_name, FALSE)
			   && !tv_check_lock(&di->di_tv, lp->ll_name, FALSE)))
			&& tv_op(&tv, rettv, op) == OK)
		    set_var_const(lp->ll_name, lp->ll_sid, ખાલી, &tv, FALSE,
				ASSIGN_NO_DECL | ASSIGN_COMPOUND_OP, 0);
		clear_tv(&tv);
	    }
	}
	નહીંતર
	{
	    જો (lp->ll_type != ખાલી && check_typval_arg_type(lp->ll_type, rettv,
							      ખાલી, 0) == FAIL)
		પાછા;
	    set_var_const(lp->ll_name, lp->ll_sid, lp->ll_type, rettv, copy,
							       flags, var_idx);
	}
	*endp = cc;
    }
    નહીંતર જો (કિંમત_check_lock(lp->ll_newkey == ખાલી
		? lp->ll_tv->v_lock
		: lp->ll_tv->vval.v_dict->dv_lock, lp->ll_name, FALSE))
	;
    નહીંતર જો (lp->ll_range)
    {
	જો ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					     && (flags & ASSIGN_FOR_LOOP) == 0)
	{
	    emsg(_(e_cannot_lock_range));
	    પાછા;
	}

	(શૂન્ય)list_assign_range(lp->ll_list, rettv->vval.v_list,
			 lp->ll_n1, lp->ll_n2, lp->ll_empty2, op, lp->ll_name);
    }
    નહીંતર
    {
	/*
	 * Assign to a List, Dictionary or Object item.
	 */
	જો ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					     && (flags & ASSIGN_FOR_LOOP) == 0)
	{
	    emsg(_(e_cannot_lock_list_or_dict));
	    પાછા;
	}

	જો (lp->ll_valtype != ખાલી
		    && check_typval_arg_type(lp->ll_valtype, rettv,
							      ખાલી, 0) == FAIL)
	{
	    lp->ll_name_end = ખાલી;
	    પાછા;
	}

	// If the lval is a List and the type of the list is not yet set,
	// then set the item type from the declared type of the variable.
	જો (in_vim9script() && rettv->v_type == VAR_LIST
				&& rettv->vval.v_list != ખાલી
				&& rettv->vval.v_list->lv_type == ખાલી)
	{
	    જો (lp->ll_tv->v_type == VAR_LIST
		    && lp->ll_tv->vval.v_list != ખાલી
		    && lp->ll_tv->vval.v_list->lv_type != ખાલી)
		set_tv_type(rettv, lp->ll_tv->vval.v_list->lv_type);
	}

	જો (lp->ll_newkey != ખાલી)
	{
	    જો (op != ખાલી && *op != '=')
	    {
		semsg(_(e_ચાવી_not_present_in_dictionary_str), lp->ll_newkey);
		પાછા;
	    }
	    જો (dict_wrong_func_name(lp->ll_tv->vval.v_dict, rettv,
								lp->ll_newkey))
		પાછા;

	    // Need to add an item to the Dictionary.
	    di = dictitem_alloc(lp->ll_newkey);
	    જો (di == ખાલી)
		પાછા;
	    જો (dict_add(lp->ll_tv->vval.v_dict, di) == FAIL)
	    {
		vim_free(di);
		પાછા;
	    }
	    lp->ll_tv = &di->di_tv;
	}
	નહીંતર જો (op != ખાલી && *op != '=')
	{
	    tv_op(lp->ll_tv, rettv, op);
	    પાછા;
	}
	નહીંતર
	    clear_tv(lp->ll_tv);

	/*
	 * Assign the value to the variable or list item.
	 */
	જો (copy)
	    copy_tv(rettv, lp->ll_tv);
	નહીંતર
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
    સ્થિર પૂર્ણાંક
tv_op_blob(typval_T *tv1, typval_T *tv2, char_u *op)
{
    જો (*op != '+' || tv2->v_type != VAR_BLOB)
	પાછા FAIL;

    // Blob += Blob
    જો (tv2->vval.v_blob == ખાલી)
	પાછા OK;

    જો (tv1->vval.v_blob == ખાલી)
    {
	tv1->vval.v_blob = tv2->vval.v_blob;
	++tv1->vval.v_blob->bv_refcount;
	પાછા OK;
    }

    blob_T	*b1 = tv1->vval.v_blob;
    blob_T	*b2 = tv2->vval.v_blob;
    પૂર્ણાંક		len = blob_len(b2);

    જો (len > 0 && ga_grow(&b1->bv_ga, len) == OK)
    {
	mch_memmove((char_u *)b1->bv_ga.ga_data + b1->bv_ga.ga_len,
		    (char_u *)b2->bv_ga.ga_data, (size_t)len);
	b1->bv_ga.ga_len += len;
    }
    નહીંતર
	માટે (પૂર્ણાંક i = 0; i < len; i++)
	    (શૂન્ય)ga_append(&b1->bv_ga, blob_get(b2, i));

    પાછા OK;
}

/*
 * Handle "list1 += list2".
 * Returns OK or FAIL.
 */
    સ્થિર પૂર્ણાંક
tv_op_list(typval_T *tv1, typval_T *tv2, char_u *op)
{
    જો (*op != '+' || tv2->v_type != VAR_LIST)
	પાછા FAIL;

    // List += List
    જો (tv2->vval.v_list == ખાલી)
	પાછા OK;

    જો (tv1->vval.v_list == ખાલી)
    {
	tv1->vval.v_list = tv2->vval.v_list;
	++tv1->vval.v_list->lv_refcount;
    }
    નહીંતર
	list_extend(tv1->vval.v_list, tv2->vval.v_list, ખાલી);

    પાછા OK;
}

/*
 * Handle number operations:
 *	nr += nr , nr -= nr , nr *=nr , nr /= nr , nr %= nr
 *
 * Returns OK or FAIL.
 */
    સ્થિર પૂર્ણાંક
tv_op_સંખ્યા(typval_T *tv1, typval_T *tv2, char_u *op)
{
    varnumber_T	n;
    પૂર્ણાંક		failed = FALSE;

    n = tv_get_સંખ્યા(tv1);
    જો (tv2->v_type == VAR_FLOAT)
    {
	float_T f = n;

	જો (*op == '%')
	    પાછા FAIL;
	પસંદગી (*op)
	{
	    સ્થિતિ '+': f += tv2->vval.v_float; તોડો;
	    સ્થિતિ '-': f -= tv2->vval.v_float; તોડો;
	    સ્થિતિ '*': f *= tv2->vval.v_float; તોડો;
	    સ્થિતિ '/': f /= tv2->vval.v_float; તોડો;
	}
	clear_tv(tv1);
	tv1->v_type = VAR_FLOAT;
	tv1->vval.v_float = f;
    }
    નહીંતર
    {
	પસંદગી (*op)
	{
	    સ્થિતિ '+': n += tv_get_સંખ્યા(tv2); તોડો;
	    સ્થિતિ '-': n -= tv_get_સંખ્યા(tv2); તોડો;
	    સ્થિતિ '*': n *= tv_get_સંખ્યા(tv2); તોડો;
	    સ્થિતિ '/': n = અંક_divide(n, tv_get_સંખ્યા(tv2), &failed); તોડો;
	    સ્થિતિ '%': n = અંક_modulus(n, tv_get_સંખ્યા(tv2), &failed); તોડો;
	}
	clear_tv(tv1);
	tv1->v_type = VAR_સંખ્યા;
	tv1->vval.v_સંખ્યા = n;
    }

    પાછા failed ? FAIL : OK;
}

/*
 * Append string "s2" to the string in "tv1".
 * Returns OK if "tv1" was grown in place, FAIL otherwise.
 */
    સ્થિર પૂર્ણાંક
grow_string_tv(typval_T *tv1, char_u *s2)
{
    જો (tv1->v_type != VAR_STRING || tv1->vval.v_string == ખાલી)
	પાછા FAIL;

    size_t len1 = STRLEN(tv1->vval.v_string);
    size_t len2 = STRLEN(s2);
    char_u *p = vim_realloc(tv1->vval.v_string, len1 + len2 + 1);

    જો (p == ખાલી)
	પાછા FAIL;

    mch_memmove(p + len1, s2, len2 + 1);
    tv1->vval.v_string = p;
    પાછા OK;
}

/*
 * Handle "str1 .= str2"
 * Returns OK or FAIL.
 */
    સ્થિર પૂર્ણાંક
tv_op_string(typval_T *tv1, typval_T *tv2, char_u *op UNUSED)
{
    char_u	numbuf[NUMBUFLEN];
    char_u	*s;
    char_u	*s2;

    જો (tv2->v_type == VAR_FLOAT)
	પાછા FAIL;

    // str .= str
    s2 = tv_get_string_buf(tv2, numbuf);
    જો (grow_string_tv(tv1, s2) == OK)
	પાછા OK;

    s = tv_get_string(tv1);
    s = concat_str(s, s2);
    clear_tv(tv1);
    tv1->v_type = VAR_STRING;
    tv1->vval.v_string = s;

    પાછા OK;
}

/*
 * Handle "tv1 += tv2", "tv1 -= tv2", "tv1 *= tv2", "tv1 /= tv2", "tv1 %= tv2"
 * and "tv1 .= tv2"
 * Returns OK or FAIL.
 */
    સ્થિર પૂર્ણાંક
tv_op_nr_or_string(typval_T *tv1, typval_T *tv2, char_u *op)
{
    જો (tv2->v_type == VAR_LIST)
	પાછા FAIL;

    જો (vim_strchr((char_u *)"+-*/%", *op) != ખાલી)
	પાછા tv_op_સંખ્યા(tv1, tv2, op);

    પાછા tv_op_string(tv1, tv2, op);
}

/*
 * Handle "f1 += f2", "f1 -= f2", "f1 *= f2", "f1 /= f2".
 * Returns OK or FAIL.
 */
    સ્થિર પૂર્ણાંક
tv_op_float(typval_T *tv1, typval_T *tv2, char_u *op)
{
    float_T f;

    જો (*op == '%' || *op == '.'
	    || (tv2->v_type != VAR_FLOAT
		&& tv2->v_type != VAR_સંખ્યા
		&& tv2->v_type != VAR_STRING))
	પાછા FAIL;

    જો (tv2->v_type == VAR_FLOAT)
	f = tv2->vval.v_float;
    નહીંતર
	f = tv_get_સંખ્યા(tv2);
    પસંદગી (*op)
    {
	સ્થિતિ '+': tv1->vval.v_float += f; તોડો;
	સ્થિતિ '-': tv1->vval.v_float -= f; તોડો;
	સ્થિતિ '*': tv1->vval.v_float *= f; તોડો;
	સ્થિતિ '/': tv1->vval.v_float /= f; તોડો;
    }

    પાછા OK;
}

/*
 * Handle "tv1 += tv2", "tv1 -= tv2", "tv1 *= tv2", "tv1 /= tv2", "tv1 %= tv2"
 * and "tv1 .= tv2"
 * Returns OK or FAIL.
 */
    પૂર્ણાંક
tv_op(typval_T *tv1, typval_T *tv2, char_u *op)
{
    // Can't do anything with a Funcref or Dict on the right.
    // v:true and friends only work with "..=".
    જો (tv2->v_type == VAR_FUNC || tv2->v_type == VAR_DICT
		|| ((tv2->v_type == VAR_BOOL || tv2->v_type == VAR_SPECIAL)
							&& *op != '.'))
    {
	semsg(_(e_wrong_variable_type_for_str_equal), op);
	પાછા FAIL;
    }

    પૂર્ણાંક retval = FAIL;
    પસંદગી (tv1->v_type)
    {
	સ્થિતિ VAR_UNKNOWN:
	સ્થિતિ VAR_ANY:
	સ્થિતિ VAR_VOID:
	સ્થિતિ VAR_DICT:
	સ્થિતિ VAR_FUNC:
	સ્થિતિ VAR_PARTIAL:
	સ્થિતિ VAR_BOOL:
	સ્થિતિ VAR_SPECIAL:
	સ્થિતિ VAR_JOB:
	સ્થિતિ VAR_CHANNEL:
	સ્થિતિ VAR_INSTR:
	સ્થિતિ VAR_OBJECT:
	સ્થિતિ VAR_CLASS:
	સ્થિતિ VAR_TYPEALIAS:
	સ્થિતિ VAR_TUPLE:
	    તોડો;

	સ્થિતિ VAR_BLOB:
	    retval = tv_op_blob(tv1, tv2, op);
	    તોડો;

	સ્થિતિ VAR_LIST:
	    retval = tv_op_list(tv1, tv2, op);
	    તોડો;

	સ્થિતિ VAR_સંખ્યા:
	સ્થિતિ VAR_STRING:
	    retval = tv_op_nr_or_string(tv1, tv2, op);
	    તોડો;

	સ્થિતિ VAR_FLOAT:
	    retval = tv_op_float(tv1, tv2, op);
	    તોડો;
    }

    જો (retval != OK)
	semsg(_(e_wrong_variable_type_for_str_equal), op);

    પાછા retval;
}

/*
 * Evaluate the expression used in a ":for var in expr" command.
 * "arg" points to "var".
 * Set "*errp" to TRUE for an error, FALSE otherwise;
 * Return a pointer that holds the info.  Null when there is an error.
 */
    શૂન્ય *
eval_for_લીટી(
    char_u	*arg,
    પૂર્ણાંક		*errp,
    exarg_T	*eap,
    evalarg_T	*evalarg)
{
    forinfo_T	*fi;
    char_u	*var_list_end;
    char_u	*પદાવલિ;
    typval_T	tv;
    list_T	*l;
    tuple_T	*tuple;
    પૂર્ણાંક		skip = !(evalarg->eval_flags & EVAL_EVALUATE);

    *errp = TRUE;	// default: there is an error

    fi = ALLOC_CLEAR_ONE(forinfo_T);
    જો (fi == ખાલી)
	પાછા ખાલી;

    var_list_end = skip_var_list(arg, TRUE, &fi->fi_varcount,
						     &fi->fi_semicolon, FALSE);
    જો (var_list_end == ખાલી)
	પાછા fi;

    પદાવલિ = skipwhite_and_linebreak(var_list_end, evalarg);
    જો (પદાવલિ[0] != 'i' || પદાવલિ[1] != 'n'
				  || !(પદાવલિ[2] == NUL || VIM_ISWHITE(પદાવલિ[2])))
    {
	જો (in_vim9script() && *પદાવલિ == ':' && પદાવલિ != var_list_end)
	    semsg(_(e_no_white_space_allowed_before_colon_str), પદાવલિ);
	નહીંતર
	    emsg(_(e_missing_in_after_for));
	પાછા fi;
    }

    જો (skip)
	++emsg_skip;
    પદાવલિ = skipwhite_and_linebreak(પદાવલિ + 2, evalarg);
    જો (eval0(પદાવલિ, &tv, eap, evalarg) == OK)
    {
	*errp = FALSE;
	જો (!skip)
	{
	    જો (tv.v_type == VAR_LIST)
	    {
		l = tv.vval.v_list;
		જો (l == ખાલી)
		{
		    // a null list is like an empty list: do nothing
		    clear_tv(&tv);
		}
		નહીંતર
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
	    નહીંતર જો (tv.v_type == VAR_TUPLE)
	    {
		tuple = tv.vval.v_tuple;
		જો (tuple == ખાલી)
		{
		    // a null tuple is like an empty tuple: do nothing
		    clear_tv(&tv);
		}
		નહીંતર
		{
		    // No need to increment the refcount, it's already set for
		    // the tuple being used in "tv".
		    fi->fi_tuple = tuple;
		    fi->fi_tuple_idx = 0;
		}
	    }
	    નહીંતર જો (tv.v_type == VAR_BLOB)
	    {
		fi->fi_bi = 0;
		જો (tv.vval.v_blob != ખાલી)
		{
		    // Make a copy, so that the iteration still works when the
		    // blob is changed.
		    fi->fi_blob = blob_copy(tv.vval.v_blob);
		}
		clear_tv(&tv);
	    }
	    નહીંતર જો (tv.v_type == VAR_STRING)
	    {
		fi->fi_byte_idx = 0;
		fi->fi_string = tv.vval.v_string;
		tv.vval.v_string = ખાલી;
		જો (fi->fi_string == ખાલી)
		    fi->fi_string = vim_strsave((char_u *)"");
	    }
	    નહીંતર
	    {
		emsg(_(e_string_list_tuple_or_blob_required));
		clear_tv(&tv);
	    }
	}
    }
    જો (skip)
	--emsg_skip;
    fi->fi_break_ગણતરી = evalarg->eval_break_ગણતરી;

    પાછા fi;
}

/*
 * Used when looping over a :for line, skip the "in expr" part.
 */
    શૂન્ય
skip_for_લીટીઓ(શૂન્ય *fi_void, evalarg_T *evalarg)
{
    forinfo_T	*fi = (forinfo_T *)fi_void;
    પૂર્ણાંક		i;

    માટે (i = 0; i < fi->fi_break_ગણતરી; ++i)
	eval_next_લીટી(ખાલી, evalarg);
}

/*
 * Use the first item in a ":for" list.  Advance to the next.
 * Assign the values to the variable (list).  "arg" points to the first one.
 * Return TRUE when a valid item was found, FALSE when at end of list or
 * something wrong.
 */
    પૂર્ણાંક
next_for_item(શૂન્ય *fi_void, char_u *arg)
{
    forinfo_T	*fi = (forinfo_T *)fi_void;
    પૂર્ણાંક		result;
    પૂર્ણાંક		flag = ASSIGN_FOR_LOOP | (in_vim9script()
			 ? (ASSIGN_FINAL
			     // first round: error if variable exists
			     | (fi->fi_bi == 0 ? 0 : ASSIGN_DECL)
			     | ASSIGN_NO_MEMBER_TYPE
			     | ASSIGN_UPDATE_BLOCK_ID)
			 : 0);
    listitem_T	*item;
    પૂર્ણાંક		skip_assign = in_vim9script() && arg[0] == '_'
						      && !eval_isnamec(arg[1]);

    જો (fi->fi_blob != ખાલી)
    {
	typval_T	tv;

	જો (fi->fi_bi >= blob_len(fi->fi_blob))
	    પાછા FALSE;
	tv.v_type = VAR_સંખ્યા;
	tv.v_lock = VAR_FIXED;
	tv.vval.v_સંખ્યા = blob_get(fi->fi_blob, fi->fi_bi);
	++fi->fi_bi;
	જો (skip_assign)
	    પાછા TRUE;
	પાછા ex_let_vars(arg, &tv, TRUE, fi->fi_semicolon,
					    fi->fi_varcount, flag, ખાલી) == OK;
    }

    જો (fi->fi_string != ખાલી)
    {
	typval_T	tv;
	પૂર્ણાંક		len;

	len = mb_ptr2len(fi->fi_string + fi->fi_byte_idx);
	જો (len == 0)
	    પાછા FALSE;
	tv.v_type = VAR_STRING;
	tv.v_lock = VAR_FIXED;
	tv.vval.v_string = vim_strnsave(fi->fi_string + fi->fi_byte_idx, len);
	fi->fi_byte_idx += len;
	++fi->fi_bi;
	જો (skip_assign)
	    result = TRUE;
	નહીંતર
	    result = ex_let_vars(arg, &tv, TRUE, fi->fi_semicolon,
					    fi->fi_varcount, flag, ખાલી) == OK;
	vim_free(tv.vval.v_string);
	પાછા result;
    }

    જો (fi->fi_tuple != ખાલી)
    {
	typval_T	tv;

	જો (fi->fi_tuple_idx >= TUPLE_LEN(fi->fi_tuple))
	    પાછા FALSE;

	copy_tv(TUPLE_ITEM(fi->fi_tuple, fi->fi_tuple_idx), &tv);
	++fi->fi_tuple_idx;
	++fi->fi_bi;
	જો (skip_assign)
	    result = TRUE;
	નહીંતર
	    result = ex_let_vars(arg, &tv, TRUE, fi->fi_semicolon,
					    fi->fi_varcount, flag, ખાલી) == OK;
	clear_tv(&tv);
	પાછા result;
    }

    item = fi->fi_lw.lw_item;
    જો (item == ખાલી)
	result = FALSE;
    નહીંતર
    {
	fi->fi_lw.lw_item = item->li_next;
	++fi->fi_bi;
	જો (skip_assign)
	    result = TRUE;
	નહીંતર
	    result = (ex_let_vars(arg, &item->li_tv, TRUE, fi->fi_semicolon,
					   fi->fi_varcount, flag, ખાલી) == OK);
    }
    પાછા result;
}

/*
 * Free the structure used to store info used by ":for".
 */
    શૂન્ય
free_for_info(શૂન્ય *fi_void)
{
    forinfo_T    *fi = (forinfo_T *)fi_void;

    જો (fi == ખાલી)
	પાછા;
    જો (fi->fi_list != ખાલી)
    {
	list_rem_watch(fi->fi_list, &fi->fi_lw);
	list_unref(fi->fi_list);
    }
    નહીંતર જો (fi->fi_blob != ખાલી)
	blob_unref(fi->fi_blob);
    નહીંતર જો (fi->fi_tuple != ખાલી)
	tuple_unref(fi->fi_tuple);
    નહીંતર
	vim_free(fi->fi_string);
    vim_free(fi);
}

    શૂન્ય
set_context_for_expression(
    expand_T	*xp,
    char_u	*arg,
    cmdidx_T	cmdidx)
{
    પૂર્ણાંક		has_પદાવલિ = cmdidx != CMD_let && cmdidx != CMD_var;
    પૂર્ણાંક		c;
    char_u	*p;

    જો (cmdidx == CMD_let || cmdidx == CMD_var
				 || cmdidx == CMD_const || cmdidx == CMD_final)
    {
	xp->xp_context = EXPAND_USER_VARS;
	જો (vim_strpbrk(arg, (char_u *)"\"'+-*/%.=!?~|&$([<>,#") == ખાલી)
	{
	    // ":let var1 var2 ...": find last space.
	    માટે (p = arg + STRLEN(arg); p >= arg; )
	    {
		xp->xp_pattern = p;
		MB_PTR_BACK(arg, p);
		જો (VIM_ISWHITE(*p))
		    તોડો;
	    }
	    પાછા;
	}
    }
    નહીંતર
	xp->xp_context = cmdidx == CMD_call ? EXPAND_FUNCTIONS
							  : EXPAND_EXPRESSION;
    જ્યારે ((xp->xp_pattern = vim_strpbrk(arg,
				  (char_u *)"\"'+-*/%.=!?~|&$([<>,#")) != ખાલી)
    {
	c = *xp->xp_pattern;
	જો (c == '&')
	{
	    c = xp->xp_pattern[1];
	    જો (c == '&')
	    {
		++xp->xp_pattern;
		xp->xp_context = has_પદાવલિ ? EXPAND_EXPRESSION : EXPAND_NOTHING;
	    }
	    નહીંતર જો (c != ' ')
	    {
		xp->xp_context = EXPAND_SETTINGS;
		જો ((c == 'l' || c == 'g') && xp->xp_pattern[2] == ':')
		    xp->xp_pattern += 2;

	    }
	}
	નહીંતર જો (c == '$')
	{
	    // environment variable
	    xp->xp_context = EXPAND_ENV_VARS;
	}
	નહીંતર જો (c == '=')
	{
	    has_પદાવલિ = TRUE;
	    xp->xp_context = EXPAND_EXPRESSION;
	}
	નહીંતર જો (c == '#'
		&& xp->xp_context == EXPAND_EXPRESSION)
	{
	    // Autoload function/variable contains '#'.
	    તોડો;
	}
	નહીંતર જો ((c == '<' || c == '#')
		&& xp->xp_context == EXPAND_FUNCTIONS
		&& vim_strchr(xp->xp_pattern, '(') == ખાલી)
	{
	    // Function name can start with "<SNR>" and contain '#'.
	    તોડો;
	}
	નહીંતર જો (has_પદાવલિ)
	{
	    જો (c == '"')	    // string
	    {
		જ્યારે ((c = *++xp->xp_pattern) != NUL && c != '"')
		    જો (c == '\\' && xp->xp_pattern[1] != NUL)
			++xp->xp_pattern;
		xp->xp_context = EXPAND_NOTHING;
	    }
	    નહીંતર જો (c == '\'')	    // literal string
	    {
		// Trick: '' is like stopping and starting a literal string.
		જ્યારે ((c = *++xp->xp_pattern) != NUL && c != '\'')
		    /* skip */ ;
		xp->xp_context = EXPAND_NOTHING;
	    }
	    નહીંતર જો (c == '|')
	    {
		જો (xp->xp_pattern[1] == '|')
		{
		    ++xp->xp_pattern;
		    xp->xp_context = EXPAND_EXPRESSION;
		}
		નહીંતર
		    xp->xp_context = EXPAND_COMMANDS;
	    }
	    નહીંતર
		xp->xp_context = EXPAND_EXPRESSION;
	}
	નહીંતર
	    // Doesn't look like something valid, expand as an expression
	    // anyway.
	    xp->xp_context = EXPAND_EXPRESSION;
	arg = xp->xp_pattern;
	જો (*arg != NUL)
	    જ્યારે ((c = *++arg) != NUL && (c == ' ' || c == '\t'))
		/* skip */ ;
    }

    // ":exe one two" completes "two"
    જો ((cmdidx == CMD_execute
		|| cmdidx == CMD_echo
		|| cmdidx == CMD_echon
		|| cmdidx == CMD_echomsg
		|| cmdidx == CMD_echoerr
		|| cmdidx == CMD_echoconsole
		|| cmdidx == CMD_echowindow)
	    && xp->xp_context == EXPAND_EXPRESSION)
    {
	માટે (;;)
	{
	    char_u *n = skiptowhite(arg);

	    જો (n == arg || IS_WHITE_OR_NUL(*skipwhite(n)))
		તોડો;
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
સ્થિર regprog_T    *eval_prog_cache = ખાલી;
સ્થિર char_u	    *eval_prog_pat = ખાલી;	// pattern it was compiled for
સ્થિર char_u	    *eval_prog_enc = ખાલી;	// 'encoding' when compiled
સ્થિર long	    eval_prog_re;		// 'regexpengine' when compiled
// State sampled by eval_regcomp() for the eval_regfree() call that gives the
// program back: user code run in between, e.g. an object's string() method
// invoked by match() on a list, can change these options, and then the
// program must not be cached under the changed values.  eval_compile_enc is
// only ever compared against p_enc, never dereferenced.
સ્થિર long	    eval_compile_re;
સ્થિર char_u	    *eval_compile_enc;
સ્થિર પૂર્ણાંક	    eval_prog_busy = 0;	// programs eval_regcomp() handed out
					// that were not given back yet
સ્થિર પૂર્ણાંક	    eval_prog_reentered = FALSE;  // eval_regcomp() ran while
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
    સ્થિર bool
eval_prog_volatile(char_u *pat)
{
    char_u *p;

    જો (STRNCMP(pat, "\\%#=", 4) == 0)
	પાછા true;
    માટે (p = pat; *p != NUL; ++p)
	જો (*p == '~'
		|| (p[0] == '[' && (p[1] == ':' || p[1] == '=' || p[1] == '.'))
		|| (p[0] == '%' && (p[1] == '.'
			|| ((p[1] == '<' || p[1] == '>') && p[2] == '.'))))
	    પાછા true;
    પાછા false;
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
    જો (eval_prog_busy > 0)
	// The samples above no longer describe the program already handed
	// out: its eval_regfree() must drop it instead of caching it.
	eval_prog_reentered = TRUE;

    જો (eval_prog_cache != ખાલી
	    && *p_cpo == NUL
	    && eval_prog_cache->re_flags == RE_MAGIC + RE_STRING
	    && eval_prog_re == p_re
	    && STRCMP(eval_prog_pat, pat) == 0
	    && STRCMP(eval_prog_enc, p_enc) == 0)
    {
	prog = eval_prog_cache;
	// The caller now owns the program, eval_regfree() adopts the
	// program the caller ends up with.
	eval_prog_cache = ખાલી;
    }
    નહીંતર
	prog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
    જો (prog != ખાલી)
	++eval_prog_busy;	// paired with the eval_regfree() call
    પાછા prog;
}

/*
 * Free program "prog", obtained with eval_regcomp() for pattern "pat", by
 * keeping it in the cache for the next use.  "pat" must be the same string
 * the program was compiled for and must still be valid.
 */
    શૂન્ય
eval_regfree(char_u *pat, regprog_T *prog)
{
    પૂર્ણાંક	    reentered = eval_prog_reentered;

    જો (eval_prog_busy > 0 && --eval_prog_busy == 0)
	eval_prog_reentered = FALSE;
    જો (prog == ખાલી)
	પાછા;
    જો (reentered || *p_cpo != NUL
	    || eval_compile_re != p_re || eval_compile_enc != p_enc)
    {
	// 'cpoptions' is not empty, an option in the cache key changed, or
	// another program was handed out while the caller was using this
	// one: the samples may not describe this program, do not cache it.
	vim_regfree(prog);
	પાછા;
    }
    જો (eval_prog_pat == ખાલી || STRCMP(eval_prog_pat, pat) != 0
	    || STRCMP(eval_prog_enc, p_enc) != 0)
    {
	char_u	*pat_copy;
	char_u	*enc_copy;

	// A cached pattern is known not to be volatile, thus the scan is
	// only needed when installing a new key.
	જો (eval_prog_volatile(pat))
	{
	    // compiling depends on state the cache key does not cover
	    vim_regfree(prog);
	    પાછા;
	}
	pat_copy = vim_strsave(pat);
	enc_copy = vim_strsave(p_enc);
	જો (pat_copy == ખાલી || enc_copy == ખાલી)
	{
	    vim_free(pat_copy);
	    vim_free(enc_copy);
	    vim_regfree(prog);
	    પાછા;
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

#જો defined(EXITFREE)
    શૂન્ય
free_eval_regcomp_cache(શૂન્ય)
{
    vim_regfree(eval_prog_cache);
    eval_prog_cache = ખાલી;
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
    પૂર્ણાંક
pattern_match(char_u *pat, char_u *text, પૂર્ણાંક ic)
{
    પૂર્ણાંક		matches = FALSE;
    char_u	*save_cpo;
    regmatch_T	regmatch;

    // avoid 'l' flag in 'cpoptions'
    save_cpo = p_cpo;
    p_cpo = empty_option;
    regmatch.regprog = eval_regcomp(pat);
    જો (regmatch.regprog != ખાલી)
    {
	regmatch.rm_ic = ic;
	matches = vim_regexec_nl(&regmatch, text, (colnr_T)0);
	eval_regfree(pat, regmatch.regprog);
    }
    p_cpo = save_cpo;
    પાછા matches;
}

/*
 * Handle a name followed by "(".  Both for just "name(arg)" and for
 * "expr->name(arg)".
 * Returns OK or FAIL.
 */
    સ્થિર પૂર્ણાંક
eval_func(
	char_u	    **arg,	// points to "(", will be advanced
	evalarg_T   *evalarg,
	char_u	    *name,
	પૂર્ણાંક	    name_len,
	typval_T    *rettv,
	પૂર્ણાંક	    flags,
	typval_T    *basetv)	// "expr" for "expr->name(arg)"
{
    પૂર્ણાંક		evaluate = flags & EVAL_EVALUATE;
    char_u	*s = name;
    પૂર્ણાંક		len = name_len;
    partial_T	*partial;
    પૂર્ણાંક		ret = OK;
    type_T	*type = ખાલી;
    પૂર્ણાંક		found_var = FALSE;

    જો (!evaluate)
	check_vars(s, len);

    // If "s" is the name of a variable of type VAR_FUNC
    // use its contents.
    s = deref_func_name(s, &len, &partial,
		 in_vim9script() ? &type : ખાલી, !evaluate, FALSE, &found_var);

    // Need to make a copy, in case evaluating the arguments makes
    // the name invalid.
    s = vim_strsave(s);
    જો (s == ખાલી || (evaluate && *s == NUL))
	ret = FAIL;
    નહીંતર
    {
	funcexe_T funcexe;

	// Invoke the function.
	CLEAR_FIELD(funcexe);
	funcexe.fe_firstline = curwin->w_cursor.lnum;
	funcexe.fe_lastline = curwin->w_cursor.lnum;
	funcexe.fe_evaluate = evaluate;
	funcexe.fe_partial = partial;
	જો (partial != ખાલી)
	{
	    funcexe.fe_object = partial->pt_obj;
	    જો (funcexe.fe_object != ખાલી)
		++funcexe.fe_object->obj_refcount;
	}
	funcexe.fe_basetv = basetv;
	funcexe.fe_check_type = type;
	funcexe.fe_found_var = found_var;
	જો (evalarg != ખાલી)
	    funcexe.fe_cctx = evalarg->eval_cctx;
	ret = get_func_tv(s, len, rettv, arg, evalarg, &funcexe);
    }
    vim_free(s);

    // If evaluate is FALSE rettv->v_type was not set in
    // get_func_tv, but it's needed in handle_subscript() to parse
    // what follows. So set it here.
    જો (rettv->v_type == VAR_UNKNOWN && !evaluate && **arg == '(')
    {
	rettv->vval.v_string = ખાલી;
	rettv->v_type = VAR_FUNC;
    }

    // Stop the expression evaluation when immediately
    // aborting on error, or when an interrupt occurred or
    // an exception was thrown but not caught.
    જો (evaluate && aborting())
    {
	જો (ret == OK)
	    clear_tv(rettv);
	ret = FAIL;
    }
    પાછા ret;
}

/*
 * After a NL, skip over empty lines and comment-only lines.
 */
    સ્થિર char_u *
newline_skip_comments(char_u *arg)
{
    char_u *p = arg + 1;

    માટે (;;)
    {
	p = skipwhite(p);

	જો (*p == NUL)
	    તોડો;
	જો (vim9_comment_start(p))
	{
	    char_u *nl = vim_strchr(p, NL);

	    જો (nl == ખાલી)
		તોડો;
	    p = nl;
	}
	જો (*p != NL)
	    તોડો;
	++p;  // skip another NL
    }
    પાછા p;
}

/*
 * Get the next line source line without advancing.  But do skip over comment
 * lines.
 * Only called for Vim9 script.
 */
    સ્થિર char_u *
getline_peek_skip_comments(evalarg_T *evalarg)
{
    માટે (;;)
    {
	char_u *next = getline_peek(evalarg->eval_getline,
							 evalarg->eval_cookie);
	char_u *p;

	જો (next == ખાલી)
	    તોડો;
	p = skipwhite(next);
	જો (*p != NUL && !vim9_comment_start(p))
	    પાછા next;
	જો (eval_next_લીટી(ખાલી, evalarg) == ખાલી)
	    તોડો;
    }
    પાછા ખાલી;
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
eval_next_non_blank(char_u *arg, evalarg_T *evalarg, પૂર્ણાંક *getnext)
{
    char_u *p = skipwhite(arg);

    *getnext = FALSE;
    જો (in_vim9script()
	    && evalarg != ખાલી
	    && (evalarg->eval_cookie != ખાલી || evalarg->eval_cctx != ખાલી
								   || *p == NL)
	    && (*p == NUL || *p == NL
			     || (vim9_comment_start(p) && VIM_ISWHITE(p[-1]))))
    {
	char_u *next;

	જો (*p == NL)
	    next = newline_skip_comments(p);
	નહીંતર જો (evalarg->eval_cookie != ખાલી)
	    next = getline_peek_skip_comments(evalarg);
	નહીંતર
	    next = peek_next_લીટી_from_context(evalarg->eval_cctx);

	જો (next != ખાલી)
	{
	    *getnext = *p != NL;
	    પાછા skipwhite(next);
	}
    }
    પાછા p;
}

/*
 * To be called after eval_next_non_blank() sets "getnext" to TRUE.
 * Only called for Vim9 script.
 *
 * If "arg" is not NULL, then the caller should assign the return value to
 * "arg".
 */
    char_u *
eval_next_લીટી(char_u *arg, evalarg_T *evalarg)
{
    garray_T	*gap = &evalarg->eval_ga;
    char_u	*લીટી;

    જો (arg != ખાલી)
    {
	જો (*arg == NL)
	    પાછા newline_skip_comments(arg);
	// Truncate before a trailing comment, so that concatenating the lines
	// won't turn the rest into a comment.
	જો (*skipwhite(arg) == '#')
	    *arg = NUL;
    }

    જો (evalarg->eval_cookie != ખાલી)
	લીટી = evalarg->eval_getline(0, evalarg->eval_cookie, 0,
							   GETLINE_CONCAT_ALL);
    નહીંતર
	લીટી = next_લીટી_from_context(evalarg->eval_cctx, TRUE);
    જો (લીટી == ખાલી)
	પાછા ખાલી;

    ++evalarg->eval_break_ગણતરી;
    જો (gap->ga_itemsize > 0 && ga_grow(gap, 1) == OK)
    {
	char_u *p = skipwhite(લીટી);

	// Going to concatenate the lines after parsing.  For an empty or
	// comment line use an empty string.
	જો (*p == NUL || vim9_comment_start(p))
	{
	    vim_free(લીટી);
	    લીટી = vim_strsave((char_u *)"");
	}

	((char_u **)gap->ga_data)[gap->ga_len] = લીટી;
	++gap->ga_len;
    }
    નહીંતર જો (evalarg->eval_cookie != ખાલી)
    {
	free_eval_tofree_later(evalarg);
	evalarg->eval_tofree = લીટી;
    }

    // Advanced to the next line, "arg" no longer points into the previous
    // line.  The caller assigns the return value to "arg".
    // If "arg" is NULL, then the return value is discarded.  In that case,
    // "arg" still points to the previous line.  So don't reset
    // "eval_using_cmdline".
    જો (arg != ખાલી)
	evalarg->eval_using_cmdline = FALSE;
    પાછા skipwhite(લીટી);
}

/*
 * Call eval_next_non_blank() and get the next line if needed.
 */
    char_u *
skipwhite_and_linebreak(char_u *arg, evalarg_T *evalarg)
{
    પૂર્ણાંક	    getnext;
    char_u  *p = skipwhite_and_nl(arg);

    જો (evalarg == ખાલી)
	પાછા skipwhite(arg);
    eval_next_non_blank(p, evalarg, &getnext);
    જો (getnext)
	પાછા eval_next_લીટી(arg, evalarg);
    પાછા p;
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
    પૂર્ણાંક
eval0(
    char_u	*arg,
    typval_T	*rettv,
    exarg_T	*eap,
    evalarg_T	*evalarg)
{
    પાછા eval0_retarg(arg, rettv, eap, evalarg, ખાલી);
}

/*
 * If "arg" is a simple function call without arguments then call it and return
 * the result.  Otherwise return NOTDONE.
 */
    પૂર્ણાંક
may_call_simple_func(
    char_u	*arg,
    typval_T	*rettv)
{
    char_u  *parens = (char_u *)strstr((અક્ષર *)arg, "()");
    પૂર્ણાંક	    r = NOTDONE;

    // If the expression is "FuncName()" then we can skip a lot of overhead.
    જો (parens != ખાલી && *skipwhite(parens + 2) == NUL)
    {
	char_u *p = STRNCMP(arg, "<SNR>", 5) == 0 ? skipdigits(arg + 5) : arg;

	જો (to_name_end(p, TRUE) == parens)
	    r = call_simple_func(arg, (size_t)(parens - arg), rettv);
    }
    પાછા r;
}

/*
 * Handle zero level expression with optimization for a simple function call.
 * Same arguments and return value as eval0().
 */
    સ્થિર પૂર્ણાંક
eval0_simple_funccal(
    char_u	*arg,
    typval_T	*rettv,
    exarg_T	*eap,
    evalarg_T	*evalarg)
{
    પૂર્ણાંક	    r = may_call_simple_func(arg, rettv);

    જો (r == NOTDONE)
	r = eval0_retarg(arg, rettv, eap, evalarg, ખાલી);
    પાછા r;
}

/*
 * Like eval0() but when "retarg" is not NULL store the pointer to after the
 * expression and don't check what comes after the expression.
 */
    પૂર્ણાંક
eval0_retarg(
    char_u	*arg,
    typval_T	*rettv,
    exarg_T	*eap,
    evalarg_T	*evalarg,
    char_u	**retarg)
{
    પૂર્ણાંક		ret;
    char_u	*p;
    char_u	*પદાવલિ_end;
    પૂર્ણાંક		did_emsg_before = did_emsg;
    પૂર્ણાંક		called_emsg_before = called_emsg;
    પૂર્ણાંક		check_for_end = retarg == ખાલી;
    પૂર્ણાંક		end_error = FALSE;

    p = skipwhite(arg);
    ret = eval1(&p, rettv, evalarg);

    જો (ret != FAIL)
    {
	પદાવલિ_end = p;
	p = skipwhite(p);

	// In Vim9 script a command block is not split at NL characters for
	// commands using an expression argument.  Skip over a '#' comment to
	// check for a following NL.  Require white space before the '#'.
	જો (in_vim9script() && p > પદાવલિ_end && retarg == ખાલી)
	    જ્યારે (*p == '#')
	    {
		char_u *nl = vim_strchr(p, NL);

		જો (nl == ખાલી)
		    તોડો;
		p = skipwhite(nl + 1);
		જો (eap != ખાલી && *p != NUL)
		    eap->nextcmd = p;
		check_for_end = FALSE;
	    }

	જો (check_for_end)
	    end_error = !ends_excmd2(arg, p);
    }

    જો (ret == FAIL || end_error)
    {
	જો (ret != FAIL)
	    clear_tv(rettv);
	/*
	 * Report the invalid expression unless the expression evaluation has
	 * been cancelled due to an aborting error, an interrupt, or an
	 * exception, or we already gave a more specific error.
	 * Also check called_emsg for when using assert_fails().
	 */
	જો (!aborting()
		&& did_emsg == did_emsg_before
		&& called_emsg == called_emsg_before
		&& (!in_vim9script() || !vim9_bad_comment(p)))
	{
	    જો (end_error)
		semsg(_(e_trailing_characters_str), p);
	    નહીંતર
		semsg(_(e_invalid_expression_str), arg);
	}

	જો (eap != ખાલી && p != ખાલી)
	{
	    // Some of the expression may not have been consumed.
	    // Only execute a next command if it cannot be a "||" operator.
	    // The next command may be "catch".
	    char_u *nextcmd = check_nextcmd(p);
	    જો (nextcmd != ખાલી && *nextcmd != '|')
		eap->nextcmd = nextcmd;
	}
	પાછા FAIL;
    }

    જો (retarg != ખાલી)
	*retarg = p;
    નહીંતર જો (check_for_end && eap != ખાલી)
	set_nextcmd(eap, p);

    પાછા ret;
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
    પૂર્ણાંક
eval1(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    char_u  *p;
    પૂર્ણાંક	    getnext;

    CLEAR_POINTER(rettv);

    /*
     * Get the first variable.
     */
    જો (eval2(arg, rettv, evalarg) == FAIL)
	પાછા FAIL;

    p = eval_next_non_blank(*arg, evalarg, &getnext);
    જો (*p == '?')
    {
	પૂર્ણાંક		op_falsy = p[1] == '?';
	પૂર્ણાંક		result;
	typval_T	var2;
	evalarg_T	*evalarg_used = evalarg;
	evalarg_T	local_evalarg;
	પૂર્ણાંક		orig_flags;
	પૂર્ણાંક		evaluate;
	પૂર્ણાંક		vim9script = in_vim9script();

	જો (evalarg == ખાલી)
	{
	    init_evalarg(&local_evalarg);
	    evalarg_used = &local_evalarg;
	}
	orig_flags = evalarg_used->eval_flags;
	evaluate = evalarg_used->eval_flags & EVAL_EVALUATE;

	જો (getnext)
	    *arg = eval_next_લીટી(*arg, evalarg_used);
	નહીંતર
	{
	    જો (evaluate && vim9script && !VIM_ISWHITE(p[-1]))
	    {
		error_white_both(p, op_falsy ? 2 : 1);
		clear_tv(rettv);
		પાછા FAIL;
	    }
	    *arg = p;
	}

	result = FALSE;
	જો (evaluate)
	{
	    પૂર્ણાંક		error = FALSE;

	    જો (op_falsy)
	    {
		// Is this typeval supported with the falsy operator?
		જો (check_typval_is_કિંમત(rettv) == FAIL)
		{
		    clear_tv(rettv);
		    પાછા FAIL;
		}
		result = tv2bool(rettv);
	    }
	    નહીંતર જો (vim9script)
		result = tv_get_bool_chk(rettv, &error);
	    નહીંતર જો (tv_get_સંખ્યા_chk(rettv, &error) != 0)
		result = TRUE;
	    જો (error || !op_falsy || !result)
		clear_tv(rettv);
	    જો (error)
		પાછા FAIL;
	}

	/*
	 * Get the second variable.  Recursive!
	 */
	જો (op_falsy)
	    ++*arg;
	જો (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[1]))
	{
	    error_white_both(*arg - (op_falsy ? 1 : 0), op_falsy ? 2 : 1);
	    clear_tv(rettv);
	    પાછા FAIL;
	}
	*arg = skipwhite_and_linebreak(*arg + 1, evalarg_used);
	evalarg_used->eval_flags = (op_falsy ? !result : result)
				  ? orig_flags : (orig_flags & ~EVAL_EVALUATE);
	જો (eval1(arg, &var2, evalarg_used) == FAIL)
	{
	    evalarg_used->eval_flags = orig_flags;
	    પાછા FAIL;
	}
	જો (!op_falsy || !result)
	    *rettv = var2;

	જો (!op_falsy)
	{
	    /*
	     * Check for the ":".
	     */
	    p = eval_next_non_blank(*arg, evalarg_used, &getnext);
	    જો (*p != ':')
	    {
		emsg(_(e_missing_colon_after_questionmark));
		જો (evaluate && result)
		    clear_tv(rettv);
		evalarg_used->eval_flags = orig_flags;
		પાછા FAIL;
	    }
	    જો (getnext)
		*arg = eval_next_લીટી(*arg, evalarg_used);
	    નહીંતર
	    {
		જો (evaluate && vim9script && !VIM_ISWHITE(p[-1]))
		{
		    error_white_both(p, 1);
		    clear_tv(rettv);
		    evalarg_used->eval_flags = orig_flags;
		    પાછા FAIL;
		}
		*arg = p;
	    }

	    /*
	     * Get the third variable.  Recursive!
	     */
	    જો (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[1]))
	    {
		error_white_both(*arg, 1);
		clear_tv(rettv);
		evalarg_used->eval_flags = orig_flags;
		પાછા FAIL;
	    }
	    *arg = skipwhite_and_linebreak(*arg + 1, evalarg_used);
	    evalarg_used->eval_flags = !result ? orig_flags
					       : (orig_flags & ~EVAL_EVALUATE);
	    જો (eval1(arg, &var2, evalarg_used) == FAIL)
	    {
		જો (evaluate && result)
		    clear_tv(rettv);
		evalarg_used->eval_flags = orig_flags;
		પાછા FAIL;
	    }
	    જો (evaluate && !result)
		*rettv = var2;
	}

	જો (evalarg == ખાલી)
	    clear_evalarg(&local_evalarg, ખાલી);
	નહીંતર
	    evalarg->eval_flags = orig_flags;
    }

    પાછા OK;
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
    સ્થિર પૂર્ણાંક
eval2(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    char_u	*p;
    પૂર્ણાંક		getnext;

    /*
     * Get the first expression.
     */
    જો (eval3(arg, rettv, evalarg) == FAIL)
	પાછા FAIL;

    /*
     * Handle the  "||" operator.
     */
    p = eval_next_non_blank(*arg, evalarg, &getnext);
    જો (p[0] == '|' && p[1] == '|')
    {
	evalarg_T   *evalarg_used = evalarg;
	evalarg_T   local_evalarg;
	પૂર્ણાંક	    evaluate;
	પૂર્ણાંક	    orig_flags;
	long	    result = FALSE;
	typval_T    var2;
	પૂર્ણાંક	    error = FALSE;
	પૂર્ણાંક	    vim9script = in_vim9script();

	જો (evalarg == ખાલી)
	{
	    init_evalarg(&local_evalarg);
	    evalarg_used = &local_evalarg;
	}
	orig_flags = evalarg_used->eval_flags;
	evaluate = orig_flags & EVAL_EVALUATE;
	જો (evaluate)
	{
	    જો (vim9script)
		result = tv_get_bool_chk(rettv, &error);
	    નહીંતર જો (tv_get_સંખ્યા_chk(rettv, &error) != 0)
		result = TRUE;
	    clear_tv(rettv);
	    જો (error)
		પાછા FAIL;
	}

	/*
	 * Repeat until there is no following "||".
	 */
	જ્યારે (p[0] == '|' && p[1] == '|')
	{
	    જો (getnext)
		*arg = eval_next_લીટી(*arg, evalarg_used);
	    નહીંતર
	    {
		જો (evaluate && vim9script && !VIM_ISWHITE(p[-1]))
		{
		    error_white_both(p, 2);
		    clear_tv(rettv);
		    પાછા FAIL;
		}
		*arg = p;
	    }

	    /*
	     * Get the second variable.
	     */
	    જો (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[2]))
	    {
		error_white_both(*arg, 2);
		clear_tv(rettv);
		પાછા FAIL;
	    }
	    *arg = skipwhite_and_linebreak(*arg + 2, evalarg_used);
	    evalarg_used->eval_flags = !result ? orig_flags
					       : (orig_flags & ~EVAL_EVALUATE);
	    જો (eval3(arg, &var2, evalarg_used) == FAIL)
		પાછા FAIL;

	    /*
	     * Compute the result.
	     */
	    જો (evaluate && !result)
	    {
		જો (vim9script)
		    result = tv_get_bool_chk(&var2, &error);
		નહીંતર જો (tv_get_સંખ્યા_chk(&var2, &error) != 0)
		    result = TRUE;
		clear_tv(&var2);
		જો (error)
		    પાછા FAIL;
	    }
	    જો (evaluate)
	    {
		જો (vim9script)
		{
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_સંખ્યા = result ? VVAL_TRUE : VVAL_FALSE;
		}
		નહીંતર
		{
		    rettv->v_type = VAR_સંખ્યા;
		    rettv->vval.v_સંખ્યા = result;
		}
	    }

	    p = eval_next_non_blank(*arg, evalarg_used, &getnext);
	}

	જો (evalarg == ખાલી)
	    clear_evalarg(&local_evalarg, ખાલી);
	નહીંતર
	    evalarg->eval_flags = orig_flags;
    }

    પાછા OK;
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
    સ્થિર પૂર્ણાંક
eval3(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    char_u	*p;
    પૂર્ણાંક		getnext;

    /*
     * Get the first expression.
     */
    જો (eval4(arg, rettv, evalarg) == FAIL)
	પાછા FAIL;

    /*
     * Handle the "&&" operator.
     */
    p = eval_next_non_blank(*arg, evalarg, &getnext);
    જો (p[0] == '&' && p[1] == '&')
    {
	evalarg_T   *evalarg_used = evalarg;
	evalarg_T   local_evalarg;
	પૂર્ણાંક	    orig_flags;
	પૂર્ણાંક	    evaluate;
	long	    result = TRUE;
	typval_T    var2;
	પૂર્ણાંક	    error = FALSE;
	પૂર્ણાંક	    vim9script = in_vim9script();

	જો (evalarg == ખાલી)
	{
	    init_evalarg(&local_evalarg);
	    evalarg_used = &local_evalarg;
	}
	orig_flags = evalarg_used->eval_flags;
	evaluate = orig_flags & EVAL_EVALUATE;
	જો (evaluate)
	{
	    જો (vim9script)
		result = tv_get_bool_chk(rettv, &error);
	    નહીંતર જો (tv_get_સંખ્યા_chk(rettv, &error) == 0)
		result = FALSE;
	    clear_tv(rettv);
	    જો (error)
		પાછા FAIL;
	}

	/*
	 * Repeat until there is no following "&&".
	 */
	જ્યારે (p[0] == '&' && p[1] == '&')
	{
	    જો (getnext)
		*arg = eval_next_લીટી(*arg, evalarg_used);
	    નહીંતર
	    {
		જો (evaluate && vim9script && !VIM_ISWHITE(p[-1]))
		{
		    error_white_both(p, 2);
		    clear_tv(rettv);
		    પાછા FAIL;
		}
		*arg = p;
	    }

	    /*
	     * Get the second variable.
	     */
	    જો (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[2]))
	    {
		error_white_both(*arg, 2);
		clear_tv(rettv);
		પાછા FAIL;
	    }
	    *arg = skipwhite_and_linebreak(*arg + 2, evalarg_used);
	    evalarg_used->eval_flags = result ? orig_flags
					      : (orig_flags & ~EVAL_EVALUATE);
	    CLEAR_FIELD(var2);
	    જો (eval4(arg, &var2, evalarg_used) == FAIL)
		પાછા FAIL;

	    /*
	     * Compute the result.
	     */
	    જો (evaluate && result)
	    {
		જો (vim9script)
		    result = tv_get_bool_chk(&var2, &error);
		નહીંતર જો (tv_get_સંખ્યા_chk(&var2, &error) == 0)
		    result = FALSE;
		clear_tv(&var2);
		જો (error)
		    પાછા FAIL;
	    }
	    જો (evaluate)
	    {
		જો (vim9script)
		{
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_સંખ્યા = result ? VVAL_TRUE : VVAL_FALSE;
		}
		નહીંતર
		{
		    rettv->v_type = VAR_સંખ્યા;
		    rettv->vval.v_સંખ્યા = result;
		}
	    }

	    p = eval_next_non_blank(*arg, evalarg_used, &getnext);
	}

	જો (evalarg == ખાલી)
	    clear_evalarg(&local_evalarg, ખાલી);
	નહીંતર
	    evalarg->eval_flags = orig_flags;
    }

    પાછા OK;
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
    સ્થિર પૂર્ણાંક
eval4(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    char_u	*p;
    પૂર્ણાંક		getnext;
    exprtype_T	type = પદાવલિ_UNKNOWN;
    પૂર્ણાંક		len = 2;
    પૂર્ણાંક		type_is = FALSE;

    /*
     * Get the first expression.
     */
    જો (eval5(arg, rettv, evalarg) == FAIL)
	પાછા FAIL;

    p = eval_next_non_blank(*arg, evalarg, &getnext);

    type = get_compare_type(p, &len, &type_is);

    /*
     * If there is a comparative operator, use it.
     */
    જો (type != પદાવલિ_UNKNOWN)
    {
	typval_T    var2;
	પૂર્ણાંક	    ic;
	પૂર્ણાંક	    vim9script = in_vim9script();
	પૂર્ણાંક	    evaluate = evalarg == ખાલી
				   ? 0 : (evalarg->eval_flags & EVAL_EVALUATE);
	long	    comp_lnum = SOURCING_LNUM;

	જો (getnext)
	{
	    *arg = eval_next_લીટી(*arg, evalarg);
	    p = *arg;
	}
	નહીંતર જો (evaluate && vim9script && !VIM_ISWHITE(**arg))
	{
	    error_white_both(*arg, len);
	    clear_tv(rettv);
	    પાછા FAIL;
	}

	જો (vim9script && type_is && (p[len] == '?' || p[len] == '#'))
	{
	    semsg(_(e_invalid_expression_str), p);
	    clear_tv(rettv);
	    પાછા FAIL;
	}

	// extra question mark appended: ignore case
	જો (p[len] == '?')
	{
	    ic = TRUE;
	    ++len;
	}
	// extra '#' appended: match case
	નહીંતર જો (p[len] == '#')
	{
	    ic = FALSE;
	    ++len;
	}
	// nothing appended: use 'ignorecase' if not in Vim script
	નહીંતર
	    ic = vim9script ? FALSE : p_ic;

	/*
	 * Get the second variable.
	 */
	જો (evaluate && vim9script && !IS_WHITE_OR_NUL(p[len]))
	{
	    error_white_both(p, len);
	    clear_tv(rettv);
	    પાછા FAIL;
	}
	*arg = skipwhite_and_linebreak(p + len, evalarg);
	જો (eval5(arg, &var2, evalarg) == FAIL)
	{
	    clear_tv(rettv);
	    પાછા FAIL;
	}
	જો (evaluate)
	{
	    પૂર્ણાંક ret;

	    // use the line of the comparison for messages
	    SOURCING_LNUM = comp_lnum;
	    જો (vim9script && check_compare_types(type, rettv, &var2) == FAIL)
	    {
		ret = FAIL;
		clear_tv(rettv);
	    }
	    નહીંતર
		ret = typval_compare(rettv, &var2, type, ic);
	    clear_tv(&var2);
	    પાછા ret;
	}
    }

    પાછા OK;
}

/*
 * Make a copy of blob "tv1" and append blob "tv2".
 */
    શૂન્ય
eval_addblob(typval_T *tv1, typval_T *tv2)
{
    blob_T  *b1 = tv1->vval.v_blob;
    blob_T  *b2 = tv2->vval.v_blob;
    blob_T  *b = blob_alloc();
    long    len1, len2;
    long    totallen;

    જો (b == ખાલી)
	પાછા;

    len1 = blob_len(b1);
    len2 = blob_len(b2);
    totallen = len1 + len2;

    જો (totallen > 0 && totallen <= INT_MAX
				    && ga_grow(&b->bv_ga, (પૂર્ણાંક)totallen) == OK)
    {
	જો (len1 > 0)
	    mch_memmove((char_u *)b->bv_ga.ga_data,
		    (char_u *)b1->bv_ga.ga_data, (size_t)len1);
	જો (len2 > 0)
	    mch_memmove((char_u *)b->bv_ga.ga_data + len1,
		    (char_u *)b2->bv_ga.ga_data, (size_t)len2);
	b->bv_ga.ga_len = (પૂર્ણાંક)totallen;
    }

    clear_tv(tv1);
    rettv_blob_set(tv1, b);
}

/*
 * Make a copy of list "tv1" and append list "tv2".
 */
    પૂર્ણાંક
eval_addlist(typval_T *tv1, typval_T *tv2)
{
    typval_T var3;

    // concatenate Lists
    જો (list_concat(tv1->vval.v_list, tv2->vval.v_list, &var3) == FAIL)
    {
	clear_tv(tv1);
	clear_tv(tv2);
	પાછા FAIL;
    }
    clear_tv(tv1);
    *tv1 = var3;
    પાછા OK;
}

/*
 * Make a copy of tuple "tv1" and append tuple "tv2".
 */
    પૂર્ણાંક
eval_addtuple(typval_T *tv1, typval_T *tv2)
{
    પૂર્ણાંક		vim9script = in_vim9script();
    typval_T	var3;

    જો (vim9script && tv1->vval.v_tuple != ખાલી && tv2->vval.v_tuple != ખાલી
	    && tv1->vval.v_tuple->tv_type != ખાલી
	    && tv2->vval.v_tuple->tv_type != ખાલી)
    {
	જો (!check_tuples_addable(tv1->vval.v_tuple->tv_type,
						tv2->vval.v_tuple->tv_type))
	    પાછા FAIL;
    }

    // concatenate tuples
    જો (tuple_concat(tv1->vval.v_tuple, tv2->vval.v_tuple, &var3) == FAIL)
    {
	clear_tv(tv1);
	clear_tv(tv2);
	પાછા FAIL;
    }
    clear_tv(tv1);
    *tv1 = var3;
    પાછા OK;
}

/*
 * Left or right shift the number "tv1" by the number "tv2" and store the
 * result in "tv1".
 *
 * Return OK or FAIL.
 */
    સ્થિર પૂર્ણાંક
eval_shift_સંખ્યા(typval_T *tv1, typval_T *tv2, પૂર્ણાંક shift_type)
{
    જો (tv2->v_type != VAR_સંખ્યા || tv2->vval.v_સંખ્યા < 0)
    {
	// right operand should be a positive number
	જો (tv2->v_type != VAR_સંખ્યા)
	    emsg(_(e_bitshift_ops_must_be_સંખ્યા));
	નહીંતર
	    emsg(_(e_bitshift_ops_must_be_positive));
	clear_tv(tv1);
	clear_tv(tv2);
	પાછા FAIL;
    }

    જો (tv2->vval.v_સંખ્યા > MAX_LSHIFT_BITS)
	// shifting more bits than we have always results in zero
	tv1->vval.v_સંખ્યા = 0;
    નહીંતર જો (shift_type == પદાવલિ_LSHIFT)
	tv1->vval.v_સંખ્યા =
	    (uvarnumber_T)tv1->vval.v_સંખ્યા << tv2->vval.v_સંખ્યા;
    નહીંતર
	tv1->vval.v_સંખ્યા =
	    (uvarnumber_T)tv1->vval.v_સંખ્યા >> tv2->vval.v_સંખ્યા;

    પાછા OK;
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
    સ્થિર પૂર્ણાંક
eval5(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    /*
     * Get the first expression.
     */
    જો (eval6(arg, rettv, evalarg) == FAIL)
	પાછા FAIL;

    /*
     * Repeat computing, until no '<<' or '>>' is following.
     */
    માટે (;;)
    {
	char_u		*p;
	પૂર્ણાંક		getnext;
	exprtype_T	exprtype;
	પૂર્ણાંક		evaluate;
	typval_T	var2;
	પૂર્ણાંક		vim9script;

	p = eval_next_non_blank(*arg, evalarg, &getnext);
	જો (p[0] == '<' && p[1] == '<')
	    exprtype = પદાવલિ_LSHIFT;
	નહીંતર જો (p[0] == '>' && p[1] == '>')
	    exprtype = પદાવલિ_RSHIFT;
	નહીંતર
	    પાછા OK;

	// Handle a bitwise left or right shift operator
	evaluate = evalarg == ખાલી ? 0 : (evalarg->eval_flags & EVAL_EVALUATE);
	જો (evaluate && rettv->v_type != VAR_સંખ્યા)
	{
	    // left operand should be a number
	    emsg(_(e_bitshift_ops_must_be_સંખ્યા));
	    clear_tv(rettv);
	    પાછા FAIL;
	}

	vim9script = in_vim9script();
	જો (getnext)
	{
	    *arg = eval_next_લીટી(*arg, evalarg);
	    p = *arg;
	}
	નહીંતર જો (evaluate && vim9script && !VIM_ISWHITE(**arg))
	{
	    error_white_both(*arg, 2);
	    clear_tv(rettv);
	    પાછા FAIL;
	}

	/*
	 * Get the second variable.
	 */
	જો (evaluate && vim9script && !IS_WHITE_OR_NUL(p[2]))
	{
	    error_white_both(p, 2);
	    clear_tv(rettv);
	    પાછા FAIL;
	}
	*arg = skipwhite_and_linebreak(p + 2, evalarg);
	જો (eval6(arg, &var2, evalarg) == FAIL)
	{
	    clear_tv(rettv);
	    પાછા FAIL;
	}

	જો (evaluate)
	{
	    જો (eval_shift_સંખ્યા(rettv, &var2, exprtype) == FAIL)
		પાછા FAIL;
	}

	clear_tv(&var2);
    }

    પાછા OK;
}

/*
 * Concatenate strings "tv1" and "tv2" and store the result in "tv1".
 */
    સ્થિર પૂર્ણાંક
eval_concat_str(typval_T *tv1, typval_T *tv2)
{
    char_u	buf1[NUMBUFLEN], buf2[NUMBUFLEN];
    char_u	*s1 = tv_get_string_buf(tv1, buf1);
    char_u	*s2 = ખાલી;
    char_u	*p;
    પૂર્ણાંક		vim9script = in_vim9script();

    જો (vim9script && (tv2->v_type == VAR_VOID
		|| tv2->v_type == VAR_CHANNEL
		|| tv2->v_type == VAR_JOB))
	semsg(_(e_using_invalid_કિંમત_as_string_str),
		vartype_name(tv2->v_type));
    નહીંતર જો (vim9script && tv2->v_type == VAR_FLOAT)
    {
	vim_snprintf((અક્ષર *)buf2, NUMBUFLEN, "%g",
		tv2->vval.v_float);
	s2 = buf2;
    }
    નહીંતર
	s2 = tv_get_string_buf_chk(tv2, buf2);
    જો (s2 == ખાલી)		// type error ?
    {
	clear_tv(tv1);
	clear_tv(tv2);
	પાછા FAIL;
    }

    // When possible, grow the existing string in place to avoid alloc/free.
    જો (grow_string_tv(tv1, s2) == OK)
	પાછા OK;

    p = concat_str(s1, s2);
    clear_tv(tv1);
    tv1->v_type = VAR_STRING;
    tv1->vval.v_string = p;

    પાછા OK;
}

/*
 * Add or subtract numbers "tv1" and "tv2" and store the result in "tv1".
 * The numbers can be whole numbers or floats.
 */
    સ્થિર પૂર્ણાંક
eval_addsub_સંખ્યા(typval_T *tv1, typval_T *tv2, પૂર્ણાંક op)
{
    પૂર્ણાંક		error = FALSE;
    varnumber_T	n1, n2;
    float_T	f1 = 0, f2 = 0;

    જો (tv1->v_type == VAR_FLOAT)
    {
	f1 = tv1->vval.v_float;
	n1 = 0;
    }
    નહીંતર
    {
	n1 = tv_get_સંખ્યા_chk(tv1, &error);
	જો (error)
	{
	    // This can only happen for "list + non-list" or
	    // "blob + non-blob".  For "non-list + ..." or
	    // "something - ...", we returned before evaluating the
	    // 2nd operand.
	    clear_tv(tv1);
	    clear_tv(tv2);
	    પાછા FAIL;
	}
	જો (tv2->v_type == VAR_FLOAT)
	    f1 = n1;
    }
    જો (tv2->v_type == VAR_FLOAT)
    {
	f2 = tv2->vval.v_float;
	n2 = 0;
    }
    નહીંતર
    {
	n2 = tv_get_સંખ્યા_chk(tv2, &error);
	જો (error)
	{
	    clear_tv(tv1);
	    clear_tv(tv2);
	    પાછા FAIL;
	}
	જો (tv1->v_type == VAR_FLOAT)
	    f2 = n2;
    }
    clear_tv(tv1);

    // If there is a float on either side the result is a float.
    જો (tv1->v_type == VAR_FLOAT || tv2->v_type == VAR_FLOAT)
    {
	જો (op == '+')
	    f1 = f1 + f2;
	નહીંતર
	    f1 = f1 - f2;
	tv1->v_type = VAR_FLOAT;
	tv1->vval.v_float = f1;
    }
    નહીંતર
    {
	જો (op == '+')
	    n1 = n1 + n2;
	નહીંતર
	    n1 = n1 - n2;
	tv1->v_type = VAR_સંખ્યા;
	tv1->vval.v_સંખ્યા = n1;
    }

    પાછા OK;
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
    સ્થિર પૂર્ણાંક
eval6(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    /*
     * Get the first expression.
     */
    જો (eval7(arg, rettv, evalarg, FALSE) == FAIL)
	પાછા FAIL;

    /*
     * Repeat computing, until no '+', '-' or '.' is following.
     */
    માટે (;;)
    {
	પૂર્ણાંક	    evaluate;
	પૂર્ણાંક	    getnext;
	char_u	    *p;
	પૂર્ણાંક	    op;
	પૂર્ણાંક	    oplen;
	પૂર્ણાંક	    concat;
	typval_T    var2;
	પૂર્ણાંક	    vim9script = in_vim9script();
	long	    op_lnum = SOURCING_LNUM;

	// "." is only string concatenation when scriptversion is 1
	// "+=", "-=" and "..=" are assignments
	// "++" and "--" on the next line are a separate command.
	p = eval_next_non_blank(*arg, evalarg, &getnext);
	op = *p;
	concat = op == '.' && (*(p + 1) == '.' || in_old_script(2));
	જો ((op != '+' && op != '-' && !concat) || p[1] == '='
					       || (p[1] == '.' && p[2] == '='))
	    તોડો;
	જો (getnext && (op == '+' || op == '-') && p[0] == p[1])
	    તોડો;

	evaluate = evalarg == ખાલી ? 0 : (evalarg->eval_flags & EVAL_EVALUATE);
	oplen = (concat && p[1] == '.') ? 2 : 1;
	જો (getnext)
	    *arg = eval_next_લીટી(*arg, evalarg);
	નહીંતર
	{
	    જો (evaluate && vim9script && !VIM_ISWHITE(**arg))
	    {
		error_white_both(*arg, oplen);
		clear_tv(rettv);
		પાછા FAIL;
	    }
	    *arg = p;
	}
	જો ((op != '+' || (rettv->v_type != VAR_LIST
						&& rettv->v_type != VAR_TUPLE
						&& rettv->v_type != VAR_BLOB))
		&& (op == '.' || rettv->v_type != VAR_FLOAT)
		&& evaluate)
	{
	    પૂર્ણાંક		error = FALSE;

	    // For "list + ...", an illegal use of the first operand as
	    // a number cannot be determined before evaluating the 2nd
	    // operand: if this is also a list, all is ok.
	    // For "something . ...", "something - ..." or "non-list + ...",
	    // we know that the first operand needs to be a string or number
	    // without evaluating the 2nd operand.  So check before to avoid
	    // side effects after an error.
	    જો (op != '.')
		tv_get_સંખ્યા_chk(rettv, &error);
	    જો ((op == '.' && tv_get_string_chk(rettv) == ખાલી) || error)
	    {
		clear_tv(rettv);
		પાછા FAIL;
	    }
	}

	/*
	 * Get the second variable.
	 */
	જો (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[oplen]))
	{
	    error_white_both(*arg, oplen);
	    clear_tv(rettv);
	    પાછા FAIL;
	}
	*arg = skipwhite_and_linebreak(*arg + oplen, evalarg);
	જો (eval7(arg, &var2, evalarg, !vim9script && op == '.') == FAIL)
	{
	    clear_tv(rettv);
	    પાછા FAIL;
	}

	જો (evaluate)
	{
	    /*
	     * Compute the result.
	     */
	    // use the line of the operation for messages
	    SOURCING_LNUM = op_lnum;
	    જો (op == '.')
	    {
		જો (eval_concat_str(rettv, &var2) == FAIL)
		    પાછા FAIL;
	    }
	    નહીંતર જો (op == '+' && rettv->v_type == VAR_BLOB
						   && var2.v_type == VAR_BLOB)
		eval_addblob(rettv, &var2);
	    નહીંતર જો (op == '+' && rettv->v_type == VAR_LIST
						   && var2.v_type == VAR_LIST)
	    {
		જો (eval_addlist(rettv, &var2) == FAIL)
		    પાછા FAIL;
	    }
	    નહીંતર જો (op == '+' && rettv->v_type == VAR_TUPLE
					   && var2.v_type == VAR_TUPLE)
	    {
		જો (eval_addtuple(rettv, &var2) == FAIL)
		    પાછા FAIL;
	    }
	    નહીંતર
	    {
		જો (eval_addsub_સંખ્યા(rettv, &var2, op) == FAIL)
		    પાછા FAIL;
	    }
	    clear_tv(&var2);
	}
    }
    પાછા OK;
}

/*
 * Multiply or divide or compute the modulo of numbers "tv1" and "tv2" and
 * store the result in "tv1".  The numbers can be whole numbers or floats.
 */
    સ્થિર પૂર્ણાંક
eval_multdiv_સંખ્યા(typval_T *tv1, typval_T *tv2, પૂર્ણાંક op)
{
    varnumber_T	n1, n2;
    float_T	f1, f2;
    પૂર્ણાંક		error;
    પૂર્ણાંક		use_float = FALSE;

    f1 = 0;
    f2 = 0;
    error = FALSE;
    જો (tv1->v_type == VAR_FLOAT)
    {
	f1 = tv1->vval.v_float;
	use_float = TRUE;
	n1 = 0;
    }
    નહીંતર
	n1 = tv_get_સંખ્યા_chk(tv1, &error);
    clear_tv(tv1);
    જો (error)
    {
	clear_tv(tv2);
	પાછા FAIL;
    }

    જો (tv2->v_type == VAR_FLOAT)
    {
	જો (!use_float)
	{
	    f1 = n1;
	    use_float = TRUE;
	}
	f2 = tv2->vval.v_float;
	n2 = 0;
    }
    નહીંતર
    {
	n2 = tv_get_સંખ્યા_chk(tv2, &error);
	clear_tv(tv2);
	જો (error)
	    પાછા FAIL;
	જો (use_float)
	    f2 = n2;
    }

    /*
     * Compute the result.
     * When either side is a float the result is a float.
     */
    જો (use_float)
    {
	જો (op == '*')
	    f1 = f1 * f2;
	નહીંતર જો (op == '/')
	{
#ifdef VMS
	    // VMS crashes on divide by zero, work around it
	    જો (f2 == 0.0)
	    {
		જો (f1 == 0)
		    f1 = -1 * __F_FLT_MAX - 1L;   // similar to NaN
		નહીંતર જો (f1 < 0)
		    f1 = -1 * __F_FLT_MAX;
		નહીંતર
		    f1 = __F_FLT_MAX;
	    }
	    નહીંતર
		f1 = f1 / f2;
#નહીંતર
	    // We rely on the floating point library to handle divide
	    // by zero to result in "inf" and not a crash.
	    f1 = f1 / f2;
#endif
	}
	નહીંતર
	{
	    emsg(_(e_cannot_use_percent_with_float));
	    પાછા FAIL;
	}
	tv1->v_type = VAR_FLOAT;
	tv1->vval.v_float = f1;
    }
    નહીંતર
    {
	પૂર્ણાંક	    failed = FALSE;

	જો (op == '*')
	    n1 = n1 * n2;
	નહીંતર જો (op == '/')
	    n1 = અંક_divide(n1, n2, &failed);
	નહીંતર
	    n1 = અંક_modulus(n1, n2, &failed);
	જો (failed)
	    પાછા FAIL;

	tv1->v_type = VAR_સંખ્યા;
	tv1->vval.v_સંખ્યા = n1;
    }

    પાછા OK;
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
    સ્થિર પૂર્ણાંક
eval7(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    પૂર્ણાંક		want_string)  // after "." operator
{
    /*
     * Get the first expression.
     */
    જો (eval8(arg, rettv, evalarg, want_string) == FAIL)
	પાછા FAIL;

    /*
     * Repeat computing, until no '*', '/' or '%' is following.
     */
    માટે (;;)
    {
	પૂર્ણાંક	    evaluate;
	પૂર્ણાંક	    getnext;
	typval_T    var2;
	char_u	    *p;
	પૂર્ણાંક	    op;

	// "*=", "/=" and "%=" are assignments
	p = eval_next_non_blank(*arg, evalarg, &getnext);
	op = *p;
	જો ((op != '*' && op != '/' && op != '%') || p[1] == '=')
	    તોડો;

	evaluate = evalarg == ખાલી ? 0 : (evalarg->eval_flags & EVAL_EVALUATE);
	જો (getnext)
	    *arg = eval_next_લીટી(*arg, evalarg);
	નહીંતર
	{
	    જો (evaluate && in_vim9script() && !VIM_ISWHITE(**arg))
	    {
		error_white_both(*arg, 1);
		clear_tv(rettv);
		પાછા FAIL;
	    }
	    *arg = p;
	}

	/*
	 * Get the second variable.
	 */
	જો (evaluate && in_vim9script() && !IS_WHITE_OR_NUL((*arg)[1]))
	{
	    error_white_both(*arg, 1);
	    clear_tv(rettv);
	    પાછા FAIL;
	}
	*arg = skipwhite_and_linebreak(*arg + 1, evalarg);
	જો (eval8(arg, &var2, evalarg, FALSE) == FAIL)
	    પાછા FAIL;

	જો (evaluate)
	    // Compute the result.
	    જો (eval_multdiv_સંખ્યા(rettv, &var2, op) == FAIL)
		પાછા FAIL;
    }

    પાછા OK;
}

/*
 * Handle seventh level expression:
 *	a type cast before a base level expression.
 * "arg" must point to the first non-white of the expression.
 * "arg" is advanced to just after the recognized expression.
 * Return OK or FAIL.
 */
    સ્થિર પૂર્ણાંક
eval8(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    પૂર્ણાંક		want_string)	// after "." operator
{
    type_T	*want_type = ખાલી;
    garray_T	type_list;	    // list of pointers to allocated types
    પૂર્ણાંક		res;
    પૂર્ણાંક		evaluate = evalarg == ખાલી ? 0
				       : (evalarg->eval_flags & EVAL_EVALUATE);

    // Recognize <type> in Vim9 script only.
    જો (in_vim9script() && **arg == '<' && eval_isnamec1((*arg)[1])
					     && STRNCMP(*arg, "<SNR>", 5) != 0)
    {
	++*arg;
	ga_init2(&type_list, કદ(type_T *), 10);
	જો (!evaluate)
	    // Only skip over the type, it may use a type variable that is only
	    // known when the expression is compiled, e.g. in a lambda.
	    *arg = skip_type(*arg, FALSE);
	નહીંતર
	{
	    want_type = parse_type(arg, &type_list, ખાલી, ખાલી, TRUE);
	    જો (want_type == ખાલી)
	    {
		clear_type_list(&type_list);
		પાછા FAIL;
	    }
	}

	જો (**arg != '>')
	{
	    જો (*skipwhite(*arg) == '>')
		semsg(_(e_no_white_space_allowed_before_str_str), ">", *arg);
	    નહીંતર
		emsg(_(e_missing_gt));
	    clear_type_list(&type_list);
	    પાછા FAIL;
	}
	++*arg;
	*arg = skipwhite_and_linebreak(*arg, evalarg);
    }

    res = eval9(arg, rettv, evalarg, want_string);

    જો (want_type != ખાલી)
    {
	જો (evaluate && res == OK)
	{
	    type_T *actual = typval2type(rettv, get_copyID(), &type_list,
							       TVTT_DO_MEMBER);

	    જો (!equal_type(want_type, actual, 0))
	    {
		જો (want_type->tt_type == VAR_BOOL
					&& actual->tt_type != VAR_BOOL
					&& (actual->tt_flags & TTFLAG_BOOL_OK))
		{
		    પૂર્ણાંક n = tv2bool(rettv);

		    // can use "0" and "1" for boolean in some places
		    clear_tv(rettv);
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_સંખ્યા = n ? VVAL_TRUE : VVAL_FALSE;
		}
		નહીંતર
		{
		    where_T where = WHERE_INIT;

		    res = check_type(want_type, actual, TRUE, where);
		}
	    }
	}
	clear_type_list(&type_list);
    }

    પાછા res;
}

    પૂર્ણાંક
eval_leader(char_u **arg, પૂર્ણાંક vim9)
{
    char_u	*s = *arg;
    char_u	*p = *arg;

    જ્યારે (*p == '!' || *p == '-' || *p == '+')
    {
	char_u *n = skipwhite(p + 1);

	// ++, --, -+ and +- are not accepted in Vim9 script
	જો (vim9 && (*p == '-' || *p == '+') && (*n == '-' || *n == '+'))
	{
	    semsg(_(e_invalid_expression_str), s);
	    પાછા FAIL;
	}
	p = n;
    }
    *arg = p;
    પાછા OK;
}

/*
 * Check for a predefined value "true", "false" and "null.*".
 * Return OK when recognized.
 */
    પૂર્ણાંક
handle_predefined(char_u *s, પૂર્ણાંક len, typval_T *rettv)
{
    પસંદગી (len)
    {
	સ્થિતિ 4: જો (STRNCMP(s, "true", 4) == 0)
		{
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_સંખ્યા = VVAL_TRUE;
		    પાછા OK;
		}
		જો (STRNCMP(s, "null", 4) == 0)
		{
		    rettv->v_type = VAR_SPECIAL;
		    rettv->vval.v_સંખ્યા = VVAL_NULL;
		    પાછા OK;
		}
		તોડો;
	સ્થિતિ 5: જો (STRNCMP(s, "false", 5) == 0)
		{
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_સંખ્યા = VVAL_FALSE;
		    પાછા OK;
		}
		તોડો;
	સ્થિતિ 8: જો (STRNCMP(s, "null_job", 8) == 0)
		{
#ifdef FEAT_JOB_CHANNEL
		    rettv->v_type = VAR_JOB;
		    rettv->vval.v_job = ખાલી;
#નહીંતર
		    rettv->v_type = VAR_SPECIAL;
		    rettv->vval.v_સંખ્યા = VVAL_NULL;
#endif
		    પાછા OK;
		}
		તોડો;
	સ્થિતિ 9:
		જો (STRNCMP(s, "null_", 5) != 0)
		    તોડો;
		// null_list
		જો (STRNCMP(s + 5, "list", 4) == 0)
		{
		    rettv->v_type = VAR_LIST;
		    rettv->vval.v_list = ખાલી;
		    પાછા OK;
		}
		// null_dict
		જો (STRNCMP(s + 5, "dict", 4) == 0)
		{
		    rettv->v_type = VAR_DICT;
		    rettv->vval.v_dict = ખાલી;
		    પાછા OK;
		}
		// null_blob
		જો (STRNCMP(s + 5, "blob", 4) == 0)
		{
		    rettv->v_type = VAR_BLOB;
		    rettv->vval.v_blob = ખાલી;
		    પાછા OK;
		}
		તોડો;
	સ્થિતિ 10:
		જો (STRNCMP(s, "null_", 5) != 0)
		    તોડો;
		// null_class
		જો (STRNCMP(s + 5, "class", 5) == 0)
		{
		    rettv->v_type = VAR_CLASS;
		    rettv->vval.v_class = ખાલી;
		    પાછા OK;
		}
		જો (STRNCMP(s + 5, "tuple", 5) == 0)
		{
		    rettv->v_type = VAR_TUPLE;
		    rettv->vval.v_tuple = ખાલી;
		    પાછા OK;
		}
		તોડો;
	સ્થિતિ 11: જો (STRNCMP(s, "null_string", 11) == 0)
		{
		    rettv->v_type = VAR_STRING;
		    rettv->vval.v_string = ખાલી;
		    પાછા OK;
		}
		જો (STRNCMP(s, "null_object", 11) == 0)
		{
		    rettv->v_type = VAR_OBJECT;
		    rettv->vval.v_object = ખાલી;
		    પાછા OK;
		}
		તોડો;
	સ્થિતિ 12:
		જો (STRNCMP(s, "null_channel", 12) == 0)
		{
#ifdef FEAT_JOB_CHANNEL
		    rettv->v_type = VAR_CHANNEL;
		    rettv->vval.v_channel = ખાલી;
#નહીંતર
		    rettv->v_type = VAR_SPECIAL;
		    rettv->vval.v_સંખ્યા = VVAL_NULL;
#endif
		    પાછા OK;
		}
		જો (STRNCMP(s, "null_partial", 12) == 0)
		{
		    rettv->v_type = VAR_PARTIAL;
		    rettv->vval.v_partial = ખાલી;
		    પાછા OK;
		}
		તોડો;
	સ્થિતિ 13: જો (STRNCMP(s, "null_function", 13) == 0)
		{
		    rettv->v_type = VAR_FUNC;
		    rettv->vval.v_string = ખાલી;
		    પાછા OK;
		}
		તોડો;
    }
    પાછા FAIL;
}

/*
 * Handle register contents: @r.
 */
    સ્થિર શૂન્ય
eval9_reg_contents(
    char_u	**arg,
    typval_T	*rettv,
    પૂર્ણાંક		evaluate)
{
    પૂર્ણાંક		vim9script = in_vim9script();

    ++*arg;	// skip '@'
    જો (evaluate)
    {
	જો (vim9script && IS_WHITE_OR_NUL(**arg))
	    semsg(_(e_syntax_error_at_str), *arg);
	નહીંતર જો (vim9script && !valid_yank_reg(**arg, FALSE))
	    emsg_invreg(**arg);
	નહીંતર
	{
	    rettv->v_type = VAR_STRING;
	    rettv->vval.v_string = get_reg_contents(**arg,
		    GREG_પદાવલિ_SRC);
	}
    }
    જો (**arg != NUL)
	++*arg;
}

/*
 * Handle a nested expression: (expression) or lambda: (arg) => expr
 */
    સ્થિર પૂર્ણાંક
eval9_nested_પદાવલિ(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    પૂર્ણાંક		evaluate)
{
    પૂર્ણાંક		ret = NOTDONE;
    પૂર્ણાંક		vim9script = in_vim9script();

    જો (vim9script)
    {
	ret = get_lambda_tv(arg, rettv, TRUE, evalarg, ખાલી);
	જો (ret == OK && evaluate)
	{
	    ufunc_T *ufunc = rettv->vval.v_partial->pt_func;

	    // Compile it here to get the return type.  The return
	    // type is optional, when it's missing use t_unknown.
	    // This is recognized in compile_return().
	    જો (ufunc->uf_ret_type->tt_type == VAR_VOID)
		ufunc->uf_ret_type = &t_unknown;
	    જો (compile_def_function(ufunc, FALSE,
				get_compile_type(ufunc), ખાલી) == FAIL)
	    {
		clear_tv(rettv);
		ret = FAIL;
	    }
	}
    }
    જો (ret == NOTDONE)
    {
	*arg = skipwhite_and_linebreak(*arg + 1, evalarg);
	જો (**arg == ')')
	    // empty tuple
	    ret = eval_tuple(arg, rettv, evalarg, TRUE);
	નહીંતર
	{
	    ret = eval1(arg, rettv, evalarg);	// recursive!
	    જો (ret != OK)
		પાછા ret;

	    *arg = skipwhite_and_linebreak(*arg, evalarg);

	    જો (**arg == ',')
		// tuple
		ret = eval_tuple(arg, rettv, evalarg, TRUE);
	    નહીંતર જો (**arg == ')')
		++*arg;
	    નહીંતર જો (ret == OK)
	    {
		emsg(_(e_missing_closing_paren));
		clear_tv(rettv);
		ret = FAIL;
	    }
	}
    }

    પાછા ret;
}

/*
* Handle be a variable or function name.
* Can also be a curly-braces kind of name: {expr}.
*/
    સ્થિર પૂર્ણાંક
eval9_var_func_name(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    પૂર્ણાંક		evaluate,
    char_u	**name_start)
{
    char_u	*s;
    પૂર્ણાંક		len;
    char_u	*alias;
    પૂર્ણાંક		ret = OK;
    પૂર્ણાંક		vim9script = in_vim9script();

    s = *arg;
    len = get_name_len(arg, &alias, evaluate, TRUE);
    જો (alias != ખાલી)
	s = alias;

    જો (len <= 0)
	ret = FAIL;
    નહીંતર
    {
	પૂર્ણાંક	flags = evalarg == ખાલી ? 0 : evalarg->eval_flags;

	જો (evaluate && vim9script && len == 1 && *s == '_')
	{
	    emsg(_(e_cannot_use_underscore_here));
	    ret = FAIL;
	}
	નહીંતર જો (evaluate && vim9script && len > 2
						&& s[0] == 's' && s[1] == ':')
	{
	    semsg(_(e_cannot_use_s_colon_in_vim9_script_str), s);
	    ret = FAIL;
	}
	નહીંતર જો ((vim9script ? **arg : *skipwhite(*arg)) == '('
				|| (vim9script && generic_func_call(arg)))
	{
	    // "name(..."  recursive!
	    *arg = skipwhite(*arg);
	    ret = eval_func(arg, evalarg, s, len, rettv, flags, ખાલી);
	}
	નહીંતર જો (evaluate)
	{
	    // get the value of "true", "false", etc. or a variable
	    ret = FAIL;
	    જો (vim9script)
		ret = handle_predefined(s, len, rettv);
	    જો (ret == FAIL)
	    {
		*name_start = s;
		ret = eval_variable(s, len, 0, rettv, ખાલી,
					EVAL_VAR_VERBOSE + EVAL_VAR_IMPORT);

		// skip the generic function arguments (if present)
		// they are already processed by eval_variable
		જો (ret == OK && vim9script && **arg == '<'
						&& rettv->v_type == VAR_FUNC)
		    ret = skip_generic_func_type_args(arg);
	    }
	}
	નહીંતર
	{
	    // skip the name
	    check_vars(s, len);

	    // Skip the type arguments of a generic function, its name starts
	    // with an uppercase letter.
	    જો (vim9script && **arg == '<' && ASCII_ISUPPER(*s))
		skip_generic_func_type_args(arg);
	    ret = OK;
	}
    }
    vim_free(alias);

    પાછા ret;
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
    સ્થિર પૂર્ણાંક
eval9(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    પૂર્ણાંક		want_string)	// after "." operator
{
    પૂર્ણાંક		evaluate = evalarg != ખાલી
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    char_u	*name_start = ખાલી;
    char_u	*start_leader, *end_leader;
    પૂર્ણાંક		ret = OK;
    સ્થિર પૂર્ણાંક	recurse = 0;
    પૂર્ણાંક		vim9script = in_vim9script();

    /*
     * Initialise variable so that clear_tv() can't mistake this for a
     * string and free a string that isn't there.
     */
    rettv->v_type = VAR_UNKNOWN;

    /*
     * Skip '!', '-' and '+' characters.  They are handled later.
     */
    start_leader = *arg;
    જો (eval_leader(arg, vim9script) == FAIL)
	પાછા FAIL;
    end_leader = *arg;

    જો (**arg == '.' && (!SAFE_isdigit(*(*arg + 1)) || in_old_script(2)))
    {
	semsg(_(e_invalid_expression_str), *arg);
	++*arg;
	પાછા FAIL;
    }

    // Limit recursion to 1000 levels.  At least at 10000 we run out of stack
    // and crash.  With MSVC the stack is smaller.
    જો (recurse ==
#ifdef _MSC_VER
		    300
#નહીંતર
		    1000
#endif
		    )
    {
	semsg(_(e_expression_too_recursive_str), *arg);
	પાછા FAIL;
    }
    ++recurse;

    પસંદગી (**arg)
    {
    /*
     * Number constant.
     */
    સ્થિતિ '0':
    સ્થિતિ '1':
    સ્થિતિ '2':
    સ્થિતિ '3':
    સ્થિતિ '4':
    સ્થિતિ '5':
    સ્થિતિ '6':
    સ્થિતિ '7':
    સ્થિતિ '8':
    સ્થિતિ '9':
    સ્થિતિ '.':	ret = eval_સંખ્યા(arg, rettv, evaluate, want_string);

		// Apply prefixed "-" and "+" now.  Matters especially when
		// "->" follows.
		જો (ret == OK && evaluate && end_leader > start_leader
						  && rettv->v_type != VAR_BLOB)
		    ret = eval9_leader(rettv, TRUE, start_leader, &end_leader);
		તોડો;

    /*
     * String constant: "string".
     */
    સ્થિતિ '"':	ret = eval_string(arg, rettv, evaluate, FALSE);
		તોડો;

    /*
     * Literal string constant: 'str''ing'.
     */
    સ્થિતિ '\'':	ret = eval_lit_string(arg, rettv, evaluate, FALSE);
		તોડો;

    /*
     * List: [expr, expr]
     */
    સ્થિતિ '[':	ret = eval_list(arg, rettv, evalarg, TRUE);
		તોડો;

    /*
     * Literal Dictionary: #{key: val, key: val}
     */
    સ્થિતિ '#':	ret = eval_lit_dict(arg, rettv, evalarg);
		તોડો;

    /*
     * Lambda: {arg, arg -> expr}
     * Dictionary: {'key': val, 'key': val}
     */
    સ્થિતિ '{':	જો (vim9script)
		    ret = NOTDONE;
		નહીંતર
		    ret = get_lambda_tv(arg, rettv, vim9script, evalarg, ખાલી);
		જો (ret == NOTDONE)
		    ret = eval_dict(arg, rettv, evalarg, FALSE);
		તોડો;

    /*
     * Option value: &name
     */
    સ્થિતિ '&':	ret = eval_option(arg, rettv, evaluate);
		તોડો;

    /*
     * Environment variable: $VAR.
     * Interpolated string: $"string" or $'string'.
     */
    સ્થિતિ '$':	જો ((*arg)[1] == '"' || (*arg)[1] == '\'')
		    ret = eval_interp_string(arg, rettv, evaluate);
		નહીંતર
		    ret = eval_env_var(arg, rettv, evaluate);
		તોડો;

    /*
     * Register contents: @r.
     */
    સ્થિતિ '@':	eval9_reg_contents(arg, rettv, evaluate);
		તોડો;

    /*
     * nested expression: (expression).
     * or lambda: (arg) => expr
     * or tuple
     */
    સ્થિતિ '(':	ret = eval9_nested_પદાવલિ(arg, rettv, evalarg, evaluate);
		તોડો;

    મૂળભૂત:	ret = NOTDONE;
		તોડો;
    }

    જો (ret == NOTDONE)
    {
	/*
	 * Must be a variable or function name.
	 * Can also be a curly-braces kind of name: {expr}.
	 */
	ret = eval9_var_func_name(arg, rettv, evalarg, evaluate, &name_start);
    }

    // Handle following '[', '(' and '.' for expr[expr], expr.name,
    // expr(expr), expr->name(expr)
    જો (ret == OK)
	ret = handle_subscript(arg, name_start, rettv, evalarg, evaluate);

    /*
     * Apply logical NOT and unary '-', from right to left, ignore '+'.
     */
    જો (ret == OK && evaluate && end_leader > start_leader)
	ret = eval9_leader(rettv, FALSE, start_leader, &end_leader);

    --recurse;
    પાછા ret;
}

/*
 * Apply the leading "!" and "-" before an eval9 expression to "rettv".
 * When "numeric_only" is TRUE only handle "+" and "-".
 * Adjusts "end_leaderp" until it is at "start_leader".
 */
    સ્થિર પૂર્ણાંક
eval9_leader(
	typval_T    *rettv,
	પૂર્ણાંક	    numeric_only,
	char_u	    *start_leader,
	char_u	    **end_leaderp)
{
    char_u	*end_leader = *end_leaderp;
    પૂર્ણાંક		ret = OK;
    પૂર્ણાંક		error = FALSE;
    varnumber_T val = 0;
    vartype_T	type = rettv->v_type;
    પૂર્ણાંક		vim9script = in_vim9script();
    float_T	    f = 0.0;

    જો (rettv->v_type == VAR_FLOAT)
	f = rettv->vval.v_float;
    નહીંતર
    {
	જ્યારે (VIM_ISWHITE(end_leader[-1]))
	    --end_leader;
	જો (vim9script && end_leader[-1] == '!')
	{
	    // Is this typeval supported with the ! operator?
	    જો (check_typval_is_કિંમત(rettv) == FAIL)
	    {
		clear_tv(rettv);
		પાછા FAIL;
	    }
	    val = tv2bool(rettv);
	}
	નહીંતર
	    val = tv_get_સંખ્યા_chk(rettv, &error);
    }
    જો (error)
    {
	clear_tv(rettv);
	ret = FAIL;
    }
    નહીંતર
    {
	જ્યારે (end_leader > start_leader)
	{
	    --end_leader;
	    જો (*end_leader == '!')
	    {
		જો (numeric_only)
		{
		    ++end_leader;
		    તોડો;
		}
		જો (rettv->v_type == VAR_FLOAT)
		{
		    rettv->v_type = VAR_BOOL;
		    val = f == 0.0 ? VVAL_TRUE : VVAL_FALSE;
		    type = VAR_BOOL;
		}
		નહીંતર
		{
		    val = !val;
		    type = VAR_BOOL;
		}
	    }
	    નહીંતર જો (*end_leader == '-')
	    {
		જો (rettv->v_type == VAR_FLOAT)
		    f = -f;
		નહીંતર
		{
		    val = -val;
		    type = VAR_સંખ્યા;
		}
	    }
	}
	જો (rettv->v_type == VAR_FLOAT)
	{
	    clear_tv(rettv);
	    rettv->vval.v_float = f;
	}
	નહીંતર
	{
	    clear_tv(rettv);
	    જો (vim9script)
		rettv->v_type = type;
	    નહીંતર
		rettv->v_type = VAR_સંખ્યા;
	    rettv->vval.v_સંખ્યા = val;
	}
    }
    *end_leaderp = end_leader;
    પાછા ret;
}

/*
 * Call the function referred to in "rettv".
 */
    સ્થિર પૂર્ણાંક
call_func_rettv(
	char_u	    **arg,
	evalarg_T   *evalarg,
	typval_T    *rettv,
	પૂર્ણાંક	    evaluate,
	dict_T	    *selfdict,
	typval_T    *basetv)
{
    partial_T	*pt = ખાલી;
    funcexe_T	funcexe;
    typval_T	functv;
    char_u	*s;
    પૂર્ણાંક		ret;

    // need to copy the funcref so that we can clear rettv
    જો (evaluate)
    {
	functv = *rettv;
	rettv->v_type = VAR_UNKNOWN;

	// Invoke the function.  Recursive!
	જો (functv.v_type == VAR_PARTIAL)
	{
	    pt = functv.vval.v_partial;
	    s = partial_name(pt);
	}
	નહીંતર
	{
	    s = functv.vval.v_string;
	    જો (s == ખાલી || *s == NUL)
	    {
		emsg(_(e_empty_function_name));
		ret = FAIL;
		જાઓ theend;
	    }
	}
    }
    નહીંતર
	s = (char_u *)"";

    CLEAR_FIELD(funcexe);
    funcexe.fe_firstline = curwin->w_cursor.lnum;
    funcexe.fe_lastline = curwin->w_cursor.lnum;
    funcexe.fe_evaluate = evaluate;
    funcexe.fe_partial = pt;
    જો (pt != ખાલી)
    {
	funcexe.fe_object = pt->pt_obj;
	જો (funcexe.fe_object != ખાલી)
	    ++funcexe.fe_object->obj_refcount;
    }
    funcexe.fe_selfdict = selfdict;
    funcexe.fe_basetv = basetv;
    જો (evalarg != ખાલી)
	funcexe.fe_cctx = evalarg->eval_cctx;
    ret = get_func_tv(s, -1, rettv, arg, evalarg, &funcexe);

theend:
    // Clear the funcref afterwards, so that deleting it while
    // evaluating the arguments is possible (see test55).
    જો (evaluate)
	clear_tv(&functv);

    પાછા ret;
}

/*
 * Evaluate "->method()".
 * "*arg" points to "method".
 * Returns FAIL or OK. "*arg" is advanced to after the ')'.
 */
    સ્થિર પૂર્ણાંક
eval_lambda(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    પૂર્ણાંક		verbose)	// give error messages
{
    પૂર્ણાંક		evaluate = evalarg != ખાલી
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    typval_T	base = *rettv;
    પૂર્ણાંક		ret;

    rettv->v_type = VAR_UNKNOWN;

    જો (**arg == '{')
    {
	// ->{lambda}()
	ret = get_lambda_tv(arg, rettv, FALSE, evalarg, ખાલી);
    }
    નહીંતર
    {
	// ->(lambda)()
	++*arg;
	ret = eval1(arg, rettv, evalarg);
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	જો (**arg != ')')
	{
	    emsg(_(e_missing_closing_paren));
	    પાછા FAIL;
	}
	જો (rettv->v_type != VAR_STRING && rettv->v_type != VAR_FUNC
					       && rettv->v_type != VAR_PARTIAL)
	{
	    emsg(_(e_string_or_function_required_for_arrow_parens_પદાવલિ));
	    પાછા FAIL;
	}
	++*arg;
    }
    જો (ret != OK)
	પાછા FAIL;

    જો (**arg != '(')
    {
	જો (verbose)
	{
	    જો (*skipwhite(*arg) == '(')
		emsg(_(e_no_white_space_allowed_before_parenthesis));
	    નહીંતર
		semsg(_(e_missing_parenthesis_str), "lambda");
	}
	clear_tv(rettv);
	ret = FAIL;
    }
    નહીંતર
	ret = call_func_rettv(arg, evalarg, rettv, evaluate, ખાલી, &base);

    // Clear the funcref afterwards, so that deleting it while
    // evaluating the arguments is possible (see test55).
    જો (evaluate)
	clear_tv(&base);

    પાછા ret;
}

/*
 * Evaluate "->method()".
 * "*arg" points to "method".
 * Returns FAIL or OK. "*arg" is advanced to after the ')'.
 */
    સ્થિર પૂર્ણાંક
eval_method(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    પૂર્ણાંક		verbose)	// give error messages
{
    char_u	*name;
    long	len;
    char_u	*alias;
    char_u	*tofree = ખાલી;
    typval_T	base = *rettv;
    પૂર્ણાંક		ret = OK;
    પૂર્ણાંક		evaluate = evalarg != ખાલી
				      && (evalarg->eval_flags & EVAL_EVALUATE);

    rettv->v_type = VAR_UNKNOWN;

    name = *arg;
    len = get_name_len(arg, &alias, evaluate, evaluate);
    જો (alias != ખાલી)
	name = alias;

    જો (len <= 0)
    {
	જો (verbose)
	    emsg(_(e_missing_name_after_method));
	ret = FAIL;
    }
    નહીંતર
    {
	char_u *paren;

	// If there is no "(" immediately following, but there is further on,
	// it can be "import.Func()", "dict.Func()", "list[nr]", etc.
	// Does not handle anything where "(" is part of the expression.
	*arg = skipwhite(*arg);

	જો (**arg != '(' && alias == ખાલી
				    && (paren = vim_strchr(*arg, '(')) != ખાલી)
	{
	    *arg = name;

	    // Truncate the name at the "(".  Avoid trying to get another line
	    // by making "getline" NULL.
	    *paren = NUL;
	    char_u	*(*getline)(પૂર્ણાંક, શૂન્ય *, પૂર્ણાંક, getline_opt_T) = ખાલી;
	    જો (evalarg != ખાલી)
	    {
		getline = evalarg->eval_getline;
		evalarg->eval_getline = ખાલી;
	    }

	    char_u *deref = deref_function_name(arg, &tofree, evalarg, verbose);
	    જો (deref == ખાલી)
	    {
		*arg = name + len;
		ret = FAIL;
	    }
	    નહીંતર
	    {
		name = deref;
		len = (long)STRLEN(name);
	    }

	    *paren = '(';
	    જો (getline != ખાલી)
		evalarg->eval_getline = getline;
	}

	જો (ret == OK)
	{
	    *arg = skipwhite(*arg);

	    જો (**arg != '(')
	    {
		જો (verbose)
		    semsg(_(e_missing_parenthesis_str), name);
		ret = FAIL;
	    }
	    નહીંતર જો (VIM_ISWHITE((*arg)[-1]))
	    {
		જો (verbose)
		    emsg(_(e_no_white_space_allowed_before_parenthesis));
		ret = FAIL;
	    }
	    નહીંતર
		ret = eval_func(arg, evalarg, name, len, rettv,
					  evaluate ? EVAL_EVALUATE : 0, &base);
	}
    }

    // Clear the funcref afterwards, so that deleting it while
    // evaluating the arguments is possible (see test55).
    જો (evaluate)
	clear_tv(&base);
    vim_free(tofree);

    જો (alias != ખાલી)
	vim_free(alias);

    પાછા ret;
}

/*
 * Evaluate an "[expr]" or "[expr:expr]" index.  Also "dict.key".
 * "*arg" points to the '[' or '.'.
 * Returns FAIL or OK. "*arg" is advanced to after the ']'.
 */
    સ્થિર પૂર્ણાંક
eval_index(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    પૂર્ણાંક		verbose)	// give error messages
{
    પૂર્ણાંક		evaluate = evalarg != ખાલી
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    પૂર્ણાંક		empty1 = FALSE, empty2 = FALSE;
    typval_T	var1, var2;
    પૂર્ણાંક		range = FALSE;
    char_u	*ચાવી = ખાલી;
    પૂર્ણાંક		keylen = -1;
    પૂર્ણાંક		vim9script = in_vim9script();

    જો (check_can_index(rettv, evaluate, verbose) == FAIL)
	પાછા FAIL;

    init_tv(&var1);
    init_tv(&var2);
    જો (**arg == '.')
    {
	/*
	 * dict.name
	 */
	ચાવી = *arg + 1;
	માટે (keylen = 0; eval_isdictc(ચાવી[keylen]); ++keylen)
	    ;
	જો (keylen == 0)
	    પાછા FAIL;
	જો (vim9script && ચાવી[keylen] == '<')
	{
	    // skip generic type arguments
	    char_u	*p = &ચાવી[keylen];

	    જો (skip_generic_func_type_args(&p) == FAIL)
		પાછા FAIL;
	    keylen = p - ચાવી;
	}
	*arg = ચાવી + keylen;
    }
    નહીંતર
    {
	/*
	 * something[idx]
	 *
	 * Get the (first) variable from inside the [].
	 */
	*arg = skipwhite_and_linebreak(*arg + 1, evalarg);
	જો (**arg == ':')
	    empty1 = TRUE;
	નહીંતર જો (eval1(arg, &var1, evalarg) == FAIL)	// recursive!
	    પાછા FAIL;
	નહીંતર જો (vim9script && **arg == ':')
	{
	    semsg(_(e_white_space_required_before_and_after_str_at_str),
								    ":", *arg);
	    clear_tv(&var1);
	    પાછા FAIL;
	}
	નહીંતર જો (evaluate)
	{
	    પૂર્ણાંક error = FALSE;

	    // allow for indexing with float
	    જો (vim9script && rettv->v_type == VAR_DICT
						   && var1.v_type == VAR_FLOAT)
	    {
		var1.vval.v_string = typval_tostring(&var1, TRUE);
		var1.v_type = VAR_STRING;
	    }

	    જો (vim9script && (rettv->v_type == VAR_LIST
						|| rettv->v_type == VAR_TUPLE))
		tv_get_સંખ્યા_chk(&var1, &error);
	    નહીંતર
		error = tv_get_string_chk(&var1) == ખાલી;
	    જો (error)
	    {
		// not a number or string
		clear_tv(&var1);
		પાછા FAIL;
	    }
	}

	/*
	 * Get the second variable from inside the [:].
	 */
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	જો (**arg == ':')
	{
	    range = TRUE;
	    ++*arg;
	    જો (vim9script && !IS_WHITE_OR_NUL(**arg) && **arg != ']')
	    {
		semsg(_(e_white_space_required_before_and_after_str_at_str),
								":", *arg - 1);
		જો (!empty1)
		    clear_tv(&var1);
		પાછા FAIL;
	    }
	    *arg = skipwhite_and_linebreak(*arg, evalarg);
	    જો (**arg == ']')
		empty2 = TRUE;
	    નહીંતર જો (eval1(arg, &var2, evalarg) == FAIL)	// recursive!
	    {
		જો (!empty1)
		    clear_tv(&var1);
		પાછા FAIL;
	    }
	    નહીંતર જો (evaluate && tv_get_string_chk(&var2) == ખાલી)
	    {
		// not a number or string
		જો (!empty1)
		    clear_tv(&var1);
		clear_tv(&var2);
		પાછા FAIL;
	    }
	}

	// Check for the ']'.
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	જો (**arg != ']')
	{
	    જો (verbose)
		emsg(_(e_missing_closing_square_brace));
	    clear_tv(&var1);
	    જો (range)
		clear_tv(&var2);
	    પાછા FAIL;
	}
	*arg = *arg + 1;	// skip over the ']'
    }

    જો (evaluate)
    {
	પૂર્ણાંક res = eval_index_inner(rettv, range,
		empty1 ? ખાલી : &var1, empty2 ? ખાલી : &var2, FALSE,
		ચાવી, keylen, verbose);

	જો (!empty1)
	    clear_tv(&var1);
	જો (range)
	    clear_tv(&var2);
	પાછા res;
    }
    પાછા OK;
}

/*
 * Check if "rettv" can have an [index] or [sli:ce]
 */
    પૂર્ણાંક
check_can_index(typval_T *rettv, પૂર્ણાંક evaluate, પૂર્ણાંક verbose)
{
    પસંદગી (rettv->v_type)
    {
	સ્થિતિ VAR_FUNC:
	સ્થિતિ VAR_PARTIAL:
	    જો (verbose)
		emsg(_(e_cannot_index_a_funcref));
	    પાછા FAIL;
	સ્થિતિ VAR_FLOAT:
	    જો (verbose)
		emsg(_(e_using_float_as_string));
	    પાછા FAIL;
	સ્થિતિ VAR_BOOL:
	સ્થિતિ VAR_SPECIAL:
	સ્થિતિ VAR_JOB:
	સ્થિતિ VAR_CHANNEL:
	સ્થિતિ VAR_INSTR:
	સ્થિતિ VAR_OBJECT:
	    જો (verbose)
		emsg(_(e_cannot_index_special_variable));
	    પાછા FAIL;
	સ્થિતિ VAR_CLASS:
	સ્થિતિ VAR_TYPEALIAS:
	    જો (verbose)
		check_typval_is_કિંમત(rettv);
	    પાછા FAIL;
	સ્થિતિ VAR_UNKNOWN:
	સ્થિતિ VAR_ANY:
	સ્થિતિ VAR_VOID:
	    જો (evaluate)
	    {
		emsg(_(e_cannot_index_special_variable));
		પાછા FAIL;
	    }
	    // FALLTHROUGH

	સ્થિતિ VAR_STRING:
	સ્થિતિ VAR_LIST:
	સ્થિતિ VAR_TUPLE:
	સ્થિતિ VAR_DICT:
	સ્થિતિ VAR_BLOB:
	    તોડો;
	સ્થિતિ VAR_સંખ્યા:
	    જો (in_vim9script())
		emsg(_(e_cannot_index_સંખ્યા));
	    તોડો;
    }
    પાછા OK;
}

/*
 * Apply index or range to "rettv".
 * "var1" is the first index, NULL for [:expr].
 * "var2" is the second index, NULL for [expr] and [expr: ]
 * "exclusive" is TRUE for slice(): second index is exclusive, use character
 * index for string.
 * Alternatively, "key" is not NULL, then key[keylen] is the dict index.
 */
    પૂર્ણાંક
eval_index_inner(
	typval_T    *rettv,
	પૂર્ણાંક	    is_range,
	typval_T    *var1,
	typval_T    *var2,
	પૂર્ણાંક	    exclusive,
	char_u	    *ચાવી,
	પૂર્ણાંક	    keylen,
	પૂર્ણાંક	    verbose)
{
    varnumber_T	    n1, n2 = 0;
    long	    len;

    n1 = 0;
    જો (var1 != ખાલી && rettv->v_type != VAR_DICT)
	n1 = tv_get_સંખ્યા(var1);

    જો (is_range)
    {
	જો (rettv->v_type == VAR_DICT)
	{
	    જો (verbose)
		emsg(_(e_cannot_slice_dictionary));
	    પાછા FAIL;
	}
	જો (var2 != ખાલી)
	    n2 = tv_get_સંખ્યા(var2);
	નહીંતર
	    n2 = VARNUM_MAX;
    }

    પસંદગી (rettv->v_type)
    {
	સ્થિતિ VAR_UNKNOWN:
	સ્થિતિ VAR_ANY:
	સ્થિતિ VAR_VOID:
	સ્થિતિ VAR_FUNC:
	સ્થિતિ VAR_PARTIAL:
	સ્થિતિ VAR_FLOAT:
	સ્થિતિ VAR_BOOL:
	સ્થિતિ VAR_SPECIAL:
	સ્થિતિ VAR_JOB:
	સ્થિતિ VAR_CHANNEL:
	સ્થિતિ VAR_INSTR:
	સ્થિતિ VAR_CLASS:
	સ્થિતિ VAR_OBJECT:
	સ્થિતિ VAR_TYPEALIAS:
	    તોડો; // not evaluating, skipping over subscript

	સ્થિતિ VAR_સંખ્યા:
	સ્થિતિ VAR_STRING:
	    {
		char_u	*s = tv_get_string(rettv);

		len = (long)STRLEN(s);
		જો (in_vim9script() || exclusive)
		{
		    જો (is_range)
			s = string_slice(s, n1, n2, exclusive);
		    નહીંતર
			s = char_from_string(s, n1);
		}
		નહીંતર જો (is_range)
		{
		    // The resulting variable is a substring.  If the indexes
		    // are out of range the result is empty.
		    જો (n1 < 0)
		    {
			n1 = len + n1;
			જો (n1 < 0)
			    n1 = 0;
		    }
		    જો (n2 < 0)
			n2 = len + n2;
		    નહીંતર જો (n2 >= len)
			n2 = len;
		    જો (n1 >= len || n2 < 0 || n1 > n2)
			s = ખાલી;
		    નહીંતર
			s = vim_strnsave(s + n1, n2 - n1 + 1);
		}
		નહીંતર
		{
		    // The resulting variable is a string of a single
		    // character.  If the index is too big or negative the
		    // result is empty.
		    જો (n1 >= len || n1 < 0)
			s = ખાલી;
		    નહીંતર
			s = vim_strnsave(s + n1, 1);
		}
		clear_tv(rettv);
		rettv->v_type = VAR_STRING;
		rettv->vval.v_string = s;
	    }
	    તોડો;

	સ્થિતિ VAR_BLOB:
	    blob_slice_or_index(rettv->vval.v_blob, is_range, n1, n2,
							     exclusive, rettv);
	    તોડો;

	સ્થિતિ VAR_LIST:
	    જો (var1 == ખાલી)
		n1 = 0;
	    જો (var2 == ખાલી)
		n2 = VARNUM_MAX;
	    જો (list_slice_or_index(rettv->vval.v_list,
			  is_range, n1, n2, exclusive, rettv, verbose) == FAIL)
		પાછા FAIL;
	    તોડો;

	સ્થિતિ VAR_TUPLE:
	    જો (var1 == ખાલી)
		n1 = 0;
	    જો (var2 == ખાલી)
		n2 = VARNUM_MAX;
	    જો (tuple_slice_or_index(rettv->vval.v_tuple,
			  is_range, n1, n2, exclusive, rettv, verbose) == FAIL)
		પાછા FAIL;
	    તોડો;

	સ્થિતિ VAR_DICT:
	    {
		dictitem_T	*item;
		typval_T	tmp;

		જો (ચાવી == ખાલી)
		{
		    ચાવી = tv_get_string_chk(var1);
		    જો (ચાવી == ખાલી)
			પાછા FAIL;
		}

		item = dict_find(rettv->vval.v_dict, ચાવી, keylen);

		જો (item == ખાલી)
		{
		    જો (verbose)
		    {
			જો (keylen > 0)
			    ચાવી[keylen] = NUL;
			semsg(_(e_ચાવી_not_present_in_dictionary_str), ચાવી);
		    }
		    પાછા FAIL;
		}

		copy_tv(&item->di_tv, &tmp);
		clear_tv(rettv);
		*rettv = tmp;
	    }
	    તોડો;
    }
    પાછા OK;
}

/*
 * Return the function name of partial "pt".
 */
    char_u *
partial_name(partial_T *pt)
{
    જો (pt != ખાલી)
    {
	જો (pt->pt_name != ખાલી)
	    પાછા pt->pt_name;
	જો (pt->pt_func != ખાલી)
	    પાછા pt->pt_func->uf_name;
    }
    પાછા (char_u *)"";
}

    સ્થિર શૂન્ય
partial_free(partial_T *pt)
{
    પૂર્ણાંક i;

    માટે (i = 0; i < pt->pt_argc; ++i)
	clear_tv(&pt->pt_argv[i]);
    vim_free(pt->pt_argv);
    dict_unref(pt->pt_dict);
    જો (pt->pt_name != ખાલી)
    {
	func_unref(pt->pt_name);
	vim_free(pt->pt_name);
    }
    નહીંતર
	func_ptr_unref(pt->pt_func);
    જો (pt->pt_obj != ખાલી)
	object_unref(pt->pt_obj);

    // "out_up" is no longer used, decrement refcount on partial that owns it.
    partial_unref(pt->pt_outer.out_up_partial);

    // Using pt_outer from another partial.
    partial_unref(pt->pt_outer_partial);

    // Decrease the reference count for the context of a closure.  If down
    // to the minimum it may be time to free it.
    જો (pt->pt_funcstack != ખાલી)
    {
	--pt->pt_funcstack->fs_refcount;
	funcstack_check_refcount(pt->pt_funcstack);
    }
    // Similarly for loop variables.
    માટે (i = 0; i < MAX_LOOP_DEPTH; ++i)
	જો (pt->pt_loopvars[i] != ખાલી)
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
    શૂન્ય
partial_unref(partial_T *pt)
{
    જો (pt == ખાલી)
	પાછા;

    પૂર્ણાંક	done = FALSE;

    જો (--pt->pt_refcount <= 0)
	partial_free(pt);

    // If the reference count goes down to one, the funcstack may be the
    // only reference and can be freed if no other partials reference it.
    નહીંતર જો (pt->pt_refcount == 1)
    {
	// careful: if the funcstack is freed it may contain this partial
	// and it gets freed as well
	જો (pt->pt_funcstack != ખાલી)
	    done = funcstack_check_refcount(pt->pt_funcstack);

	જો (!done)
	{
	    પૂર્ણાંક	depth;

	    માટે (depth = 0; depth < MAX_LOOP_DEPTH; ++depth)
		જો (pt->pt_loopvars[depth] != ખાલી
			&& loopvars_check_refcount(pt->pt_loopvars[depth]))
		    તોડો;
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
    સ્થિર char_u *
string_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    પૂર્ણાંક		echo_style,
    પૂર્ણાંક		composite_val)
{
    char_u	*r = ખાલી;

    જો (echo_style && !composite_val)
    {
	*tofree = ખાલી;
	r = tv->vval.v_string;
	જો (r == ખાલી)
	    r = (char_u *)"";
    }
    નહીંતર
    {
	*tofree = string_quote(tv->vval.v_string, FALSE);
	r = *tofree;
    }

    પાછા r;
}

/*
 * Return a textual representation of a function in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "echo_style" is FALSE, put quotes around the function name as
 * "function()", otherwise does not put quotes around function name.
 * May return NULL.
 */
    સ્થિર char_u *
func_tv2string(typval_T *tv, char_u **tofree, પૂર્ણાંક echo_style)
{
    char_u	*r = ખાલી;
    char_u	buf[MAX_FUNC_NAME_LEN];

    જો (echo_style)
    {
	*tofree = ખાલી;

	જો (tv->vval.v_string == ખાલી)
	    r = (char_u *)"function()";
	નહીંતર
	{
	    r = make_ufunc_name_readable(tv->vval.v_string, buf,
							MAX_FUNC_NAME_LEN);
	    જો (r == buf)
		r = *tofree = vim_strsave(buf);
	}
    }
    નહીંતર
    {
	char_u *s = ખાલી;

	જો (tv->vval.v_string != ખાલી)
	    s = make_ufunc_name_readable(tv->vval.v_string, buf,
							MAX_FUNC_NAME_LEN);

	r = *tofree = string_quote(s, TRUE);
    }

    પાછા r;
}

/*
 * Return a textual representation of the object method in "tv", a VAR_PARTIAL.
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "echo_style" is FALSE, put quotes around the function name as
 * "function()", otherwise does not put quotes around function name.
 * May return NULL.
 */
    સ્થિર char_u *
method_tv2string(typval_T *tv, char_u **tofree, પૂર્ણાંક echo_style)
{
    char_u	buf[MAX_FUNC_NAME_LEN];
    partial_T	*pt = tv->vval.v_partial;

    size_t len = vim_snprintf((અક્ષર *)buf, કદ(buf), "<SNR>%d_%s.%s",
			   pt->pt_func->uf_script_ctx.sc_sid,
			   pt->pt_func->uf_class->class_name.string,
			   pt->pt_func->uf_name);
    જો (len >= કદ(buf))
    {
	જો (echo_style)
	{
	    *tofree = ખાલી;
	    પાછા (char_u *)"function()";
	}
	નહીંતર
	    પાછા *tofree = string_quote((char_u*)"", TRUE);
    }

    પાછા *tofree = echo_style ? vim_strsave(buf) : string_quote(buf, TRUE);
}

/*
 * Return a textual representation of a partial in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * "numbuf" is used for a number.  May return NULL.
 */
    સ્થિર char_u *
partial_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    char_u	*numbuf,
    પૂર્ણાંક		copyID)
{
    char_u	*r = ખાલી;
    partial_T	*pt;
    char_u	*fname;
    garray_T	ga;
    પૂર્ણાંક		i;
    char_u	*tf;

    pt = tv->vval.v_partial;
    fname = string_quote(pt == ખાલી ? ખાલી : partial_name(pt), FALSE);

    ga_init2(&ga, 1, 100);
    GA_CONCAT_LITERAL(&ga, "function(");
    જો (fname != ખાલી)
    {
	// When using uf_name prepend "g:" for a global function.
	જો (pt != ખાલી && pt->pt_name == ખાલી && fname[0] == '\''
						&& vim_isupper(fname[1]))
	{
	    GA_CONCAT_LITERAL(&ga, "'g:");
	    ga_concat(&ga, fname + 1);
	}
	નહીંતર
	    ga_concat(&ga, fname);
	vim_free(fname);
    }
    જો (pt != ખાલી && pt->pt_argc > 0)
    {
	GA_CONCAT_LITERAL(&ga, ", [");
	માટે (i = 0; i < pt->pt_argc; ++i)
	{
	    જો (i > 0)
		GA_CONCAT_LITERAL(&ga, ", ");
	    ga_concat(&ga, tv2string(&pt->pt_argv[i], &tf, numbuf, copyID));
	    vim_free(tf);
	}
	GA_CONCAT_LITERAL(&ga, "]");
    }
    જો (pt != ખાલી && pt->pt_dict != ખાલી)
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

    પાછા r;
}

/*
 * Return a textual representation of a List in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "copyID" is not zero replace recursive lists with "...".  When
 * "restore_copyID" is FALSE, repeated items in lists are replaced with "...".
 * May return NULL.
 */
    સ્થિર char_u *
list_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    પૂર્ણાંક		copyID,
    પૂર્ણાંક		restore_copyID)
{
    char_u	*r = ખાલી;

    જો (tv->vval.v_list == ખાલી)
    {
	// NULL list is equivalent to empty list.
	*tofree = ખાલી;
	r = (char_u *)"[]";
    }
    નહીંતર જો (copyID != 0 && tv->vval.v_list->lv_copyID == copyID
	    && tv->vval.v_list->lv_len > 0)
    {
	*tofree = ખાલી;
	r = (char_u *)"[...]";
    }
    નહીંતર
    {
	પૂર્ણાંક old_copyID;
	જો (restore_copyID)
	    old_copyID = tv->vval.v_list->lv_copyID;

	tv->vval.v_list->lv_copyID = copyID;
	*tofree = list2string(tv, copyID, restore_copyID);
	જો (restore_copyID)
	    tv->vval.v_list->lv_copyID = old_copyID;
	r = *tofree;
    }

    પાછા r;
}

/*
 * Return a textual representation of a Tuple in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "copyID" is not zero replace recursive lists with "...".  When
 * "restore_copyID" is FALSE, repeated items in tuples are replaced with "...".
 * May return NULL.
 */
    સ્થિર char_u *
tuple_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    પૂર્ણાંક		copyID,
    પૂર્ણાંક		restore_copyID)
{
    tuple_T	*tuple = tv->vval.v_tuple;
    char_u	*r = ખાલી;

    જો (tuple == ખાલી)
    {
	// NULL tuple is equivalent to an empty tuple.
	*tofree = ખાલી;
	r = (char_u *)"()";
    }
    નહીંતર જો (copyID != 0 && tuple->tv_copyID == copyID
					&& tuple->tv_વસ્તુઓ.ga_len > 0)
    {
	*tofree = ખાલી;
	r = (char_u *)"(...)";
    }
    નહીંતર
    {
	પૂર્ણાંક old_copyID;
	જો (restore_copyID)
	    old_copyID = tuple->tv_copyID;

	tuple->tv_copyID = copyID;
	*tofree = tuple2string(tv, copyID, restore_copyID);
	જો (restore_copyID)
	    tuple->tv_copyID = old_copyID;
	r = *tofree;
    }

    પાછા r;
}

/*
 * Return a textual representation of a Dict in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "copyID" is not zero replace recursive dicts with "...".
 * When "restore_copyID" is FALSE, repeated items in the dictionary are
 * replaced with "...".  May return NULL.
 */
    સ્થિર char_u *
dict_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    પૂર્ણાંક		copyID,
    પૂર્ણાંક		restore_copyID)
{
    char_u	*r = ખાલી;

    જો (tv->vval.v_dict == ખાલી)
    {
	// NULL dict is equivalent to empty dict.
	*tofree = ખાલી;
	r = (char_u *)"{}";
    }
    નહીંતર જો (copyID != 0 && tv->vval.v_dict->dv_copyID == copyID
	    && tv->vval.v_dict->dv_hashtab.ht_used != 0)
    {
	*tofree = ખાલી;
	r = (char_u *)"{...}";
    }
    નહીંતર
    {
	પૂર્ણાંક old_copyID;
	જો (restore_copyID)
	    old_copyID = tv->vval.v_dict->dv_copyID;

	tv->vval.v_dict->dv_copyID = copyID;
	*tofree = dict2string(tv, copyID, restore_copyID);
	જો (restore_copyID)
	    tv->vval.v_dict->dv_copyID = old_copyID;
	r = *tofree;
    }

    પાછા r;
}

/*
 * Return a textual representation of a job or a channel in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * "numbuf" is used for a number.
 * When "composite_val" is FALSE, put quotes around strings as "string()",
 * otherwise does not put quotes around strings.
 * May return NULL.
 */
    સ્થિર char_u *
jobchan_tv2string(
    typval_T	*tv UNUSED,
    char_u	**tofree UNUSED,
    char_u	*numbuf UNUSED,
    પૂર્ણાંક		composite_val UNUSED)
{
    char_u	*r = ખાલી;

#ifdef FEAT_JOB_CHANNEL
    *tofree = ખાલી;

    જો (tv->v_type == VAR_JOB)
	r = job_to_string_buf(tv, numbuf);
    નહીંતર
	r = channel_to_string_buf(tv, numbuf);

    જો (composite_val)
    {
	*tofree = string_quote(r, FALSE);
	r = *tofree;
    }
#endif

    પાછા r;
}

/*
 * Return a textual representation of a class in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * May return NULL.
 */
    સ્થિર char_u *
class_tv2string(typval_T *tv, char_u **tofree)
{
    char_u	*r = ખાલી;
    size_t	rsize;
    class_T	*cl = tv->vval.v_class;
    string_T	class_name = {(char_u *)"[unknown]", 9};
    string_T	s = {(char_u *)"class", 5};

    જો (cl != ખાલી)
    {
	class_name.string = cl->class_name.string;
	class_name.length = cl->class_name.length;
	જો (IS_INTERFACE(cl))
	    STR_LITERAL_SET(s, "interface");
	નહીંતર જો (IS_ENUM(cl))
	    STR_LITERAL_SET(s, "enum");
    }

    rsize = s.length + 1 + class_name.length + 1;
    r = *tofree = alloc(rsize);
    જો (r != ખાલી)
	vim_snprintf((અક્ષર *)r, rsize, "%s %s", s.string, (અક્ષર *)class_name.string);

    પાછા r;
}

/*
 * Return a textual representation of an Object in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "copyID" is not zero replace recursive object with "...".
 * When "restore_copyID" is FALSE, repeated items in the object are
 * replaced with "...".  May return NULL.
 */
    સ્થિર char_u *
object_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    પૂર્ણાંક		copyID,
    પૂર્ણાંક		restore_copyID,
    char_u	*numbuf,
    પૂર્ણાંક		echo_style,
    પૂર્ણાંક		composite_val)
{
    char_u	*r = ખાલી;

    object_T	*obj = tv->vval.v_object;
    જો (obj == ખાલી || obj->obj_class == ખાલી)
    {
	*tofree = ખાલી;
	r = (char_u *)"object of [unknown]";
    }
    નહીંતર જો (copyID != 0 && obj->obj_copyID == copyID
	    && obj->obj_class->class_obj_member_ગણતરી != 0)
    {
	size_t n = 25 + obj->obj_class->class_name.length;
	r = alloc(n);
	જો (r != ખાલી)
	    (શૂન્ય)vim_snprintf((અક્ષર *)r, n, "object of %s {...}",
						obj->obj_class->class_name.string);
	*tofree = r;
    }
    નહીંતર
    {
	પૂર્ણાંક old_copyID;
	જો (restore_copyID)
	    old_copyID = obj->obj_copyID;

	obj->obj_copyID = copyID;
	*tofree = object2string(obj, numbuf, copyID, echo_style,
				restore_copyID, composite_val);
	જો (restore_copyID)
	    obj->obj_copyID = old_copyID;
	r = *tofree;
    }

    પાછા r;
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
    પૂર્ણાંક		copyID,
    પૂર્ણાંક		echo_style,
    પૂર્ણાંક		restore_copyID,
    પૂર્ણાંક		composite_val)
{
    સ્થિર પૂર્ણાંક	recurse = 0;
    char_u	*r = ખાલી;

    જો (recurse >= DICT_MAXNEST)
    {
	જો (!did_echo_string_emsg)
	{
	    // Only give this message once for a recursive call to avoid
	    // flooding the user with errors.  And stop iterating over lists
	    // and dicts and objects.
	    did_echo_string_emsg = TRUE;
	    emsg(_(e_variable_nested_too_deep_for_displaying));
	}
	*tofree = ખાલી;
	પાછા (char_u *)"{E724}";
    }
    ++recurse;

    પસંદગી (tv->v_type)
    {
	સ્થિતિ VAR_STRING:
	    r = string_tv2string(tv, tofree, echo_style, composite_val);
	    તોડો;

	સ્થિતિ VAR_FUNC:
	    r = func_tv2string(tv, tofree, echo_style);
	    તોડો;

	સ્થિતિ VAR_PARTIAL:
	    જો (tv->vval.v_partial == ખાલી
		    || tv->vval.v_partial->pt_obj == ખાલી)
		r = partial_tv2string(tv, tofree, numbuf, copyID);
	    નહીંતર
		r = method_tv2string(tv, tofree, echo_style);
	    તોડો;

	સ્થિતિ VAR_BLOB:
	    r = blob2string(tv->vval.v_blob, tofree, numbuf);
	    તોડો;

	સ્થિતિ VAR_LIST:
	    r = list_tv2string(tv, tofree, copyID, restore_copyID);
	    તોડો;

	સ્થિતિ VAR_TUPLE:
	    r = tuple_tv2string(tv, tofree, copyID, restore_copyID);
	    તોડો;

	સ્થિતિ VAR_DICT:
	    r = dict_tv2string(tv, tofree, copyID, restore_copyID);
	    તોડો;

	સ્થિતિ VAR_સંખ્યા:
	સ્થિતિ VAR_UNKNOWN:
	સ્થિતિ VAR_ANY:
	સ્થિતિ VAR_VOID:
	    *tofree = ખાલી;
	    r = tv_get_string_buf(tv, numbuf);
	    તોડો;

	સ્થિતિ VAR_JOB:
	સ્થિતિ VAR_CHANNEL:
	    r = jobchan_tv2string(tv, tofree, numbuf, composite_val);
	    તોડો;

	સ્થિતિ VAR_INSTR:
	    *tofree = ખાલી;
	    r = (char_u *)"instructions";
	    તોડો;

	સ્થિતિ VAR_CLASS:
	    r = class_tv2string(tv, tofree);
	    તોડો;

	સ્થિતિ VAR_OBJECT:
	    r = object_tv2string(tv, tofree, copyID, restore_copyID,
					 numbuf, echo_style, composite_val);
	    તોડો;

	સ્થિતિ VAR_FLOAT:
	    *tofree = ખાલી;
	    vim_snprintf((અક્ષર *)numbuf, NUMBUFLEN, "%g", tv->vval.v_float);
	    r = numbuf;
	    તોડો;

	સ્થિતિ VAR_BOOL:
	સ્થિતિ VAR_SPECIAL:
	    *tofree = ખાલી;
	    r = (char_u *)get_var_special_name(tv->vval.v_સંખ્યા);
	    તોડો;

	સ્થિતિ VAR_TYPEALIAS:
	    *tofree = vim_strsave(tv->vval.v_typealias->ta_name);
	    r = *tofree;
	    જો (r == ખાલી)
		r = (char_u *)"";
	    તોડો;
    }

    જો (--recurse == 0)
	did_echo_string_emsg = FALSE;
    પાછા r;
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
    પૂર્ણાંક		copyID)
{
    પાછા echo_string_core(tv, tofree, numbuf, copyID, TRUE, FALSE, FALSE);
}

/*
 * Convert the specified byte index of line 'lnum' in buffer 'buf' to a
 * character index.  Works only for loaded buffers. Returns -1 on failure.
 * The index of the first byte and the first character is zero.
 */
    પૂર્ણાંક
buf_byteidx_to_charidx(buf_T *buf, પૂર્ણાંક lnum, પૂર્ણાંક byteidx)
{
    char_u	*str;
    char_u	*t;
    પૂર્ણાંક		ગણતરી;

    જો (buf == ખાલી || buf->b_ml.ml_mfp == ખાલી)
	પાછા -1;

    જો (lnum > buf->b_ml.ml_લીટી_ગણતરી)
	lnum = buf->b_ml.ml_લીટી_ગણતરી;

    str = ml_get_buf(buf, lnum, FALSE);
    જો (str == ખાલી)
	પાછા -1;

    જો (*str == NUL)
	પાછા 0;

    // count the number of characters
    t = str;
    માટે (ગણતરી = 0; *t != NUL && t <= str + byteidx; ગણતરી++)
	t += mb_ptr2len(t);

    // In insert mode, when the cursor is at the end of a non-empty line,
    // byteidx points to the NUL character immediately past the end of the
    // string. In this case, add one to the character count.
    જો (*t == NUL && byteidx != 0 && t == str + byteidx)
	ગણતરી++;

    પાછા ગણતરી - 1;
}

/*
 * Convert the specified character index of line 'lnum' in buffer 'buf' to a
 * byte index.  Works only for loaded buffers. Returns -1 on failure.
 * The index of the first byte and the first character is zero.
 */
    પૂર્ણાંક
buf_charidx_to_byteidx(buf_T *buf, પૂર્ણાંક lnum, પૂર્ણાંક charidx)
{
    char_u	*str;
    char_u	*t;

    જો (buf == ખાલી || buf->b_ml.ml_mfp == ખાલી)
	પાછા -1;

    જો (lnum > buf->b_ml.ml_લીટી_ગણતરી)
	lnum = buf->b_ml.ml_લીટી_ગણતરી;

    str = ml_get_buf(buf, lnum, FALSE);
    જો (str == ખાલી)
	પાછા -1;

    // Convert the character offset to a byte offset
    t = str;
    જ્યારે (*t != NUL && --charidx > 0)
	t += mb_ptr2len(t);

    પાછા t - str;
}

/*
 * Translate a String variable into a position.
 * Returns NULL when there is an error.
 */
    pos_T *
var2fpos(
    typval_T	*varp,
    પૂર્ણાંક		dollar_lnum,	// TRUE when $ is last line
    પૂર્ણાંક		*fnum,		// set to fnum for '0, 'A, etc.
    પૂર્ણાંક		charcol)	// return character column
{
    char_u		*name;
    સ્થિર pos_T	pos;
    pos_T		*pp;
    પૂર્ણાંક			error = FALSE;

    // Argument can be [lnum, col, coladd].
    જો (varp->v_type == VAR_LIST)
    {
	list_T		*l;
	પૂર્ણાંક		len;
	listitem_T	*li;

	l = varp->vval.v_list;
	જો (l == ખાલી)
	    પાછા ખાલી;

	// Get the line number
	pos.lnum = list_find_nr(l, 0L, &error);
	જો (error || pos.lnum <= 0 || pos.lnum > curbuf->b_ml.ml_લીટી_ગણતરી)
	    પાછા ખાલી;	// invalid line number
	જો (charcol)
	    len = (long)mb_charlen(ml_get(pos.lnum));
	નહીંતર
	    len = (long)ml_get_len(pos.lnum);

	// Get the column number
	// We accept "$" for the column number: last column.
	li = list_find(l, 1L);
	જો (li != ખાલી && li->li_tv.v_type == VAR_STRING
		&& li->li_tv.vval.v_string != ખાલી
		&& STRCMP(li->li_tv.vval.v_string, "$") == 0)
	{
	    pos.col = len + 1;
	}
	નહીંતર
	{
	    pos.col = list_find_nr(l, 1L, &error);
	    જો (error)
		પાછા ખાલી;
	}

	// Accept a position up to the NUL after the line.
	જો (pos.col == 0 || (પૂર્ણાંક)pos.col > len + 1)
	    પાછા ખાલી;	// invalid column number
	--pos.col;

	// Get the virtual offset.  Defaults to zero.
	pos.coladd = list_find_nr(l, 2L, &error);
	જો (error)
	    pos.coladd = 0;

	પાછા &pos;
    }

    જો (in_vim9script() && check_for_string_arg(varp, 0) == FAIL)
	પાછા ખાલી;

    name = tv_get_string_chk(varp);
    જો (name == ખાલી)
	પાછા ખાલી;

    error = TRUE;
    pos.lnum = 0;
    જો (name[0] == '.')
    {
	જો (!in_vim9script() || name[1] == NUL)
	{
	    error = FALSE;
	    // cursor
	    pos = curwin->w_cursor;
	}
    }
    નહીંતર જો (name[0] == 'v' && name[1] == NUL)
    {
	// Visual start
	જો (VIsual_active)
	    pos = VIsual;
	નહીંતર
	    pos = curwin->w_cursor;
    }
    નહીંતર જો (name[0] == '\'')
    {
	જો (!in_vim9script() || (name[1] != NUL && name[2] == NUL))
	{
	    error = FALSE;
	    // mark
	    pp = getmark_buf_fnum(curbuf, name[1], FALSE, fnum);
	    જો (pp == ખાલી || pp == (pos_T *)-1 || pp->lnum <= 0)
		પાછા ખાલી;
	    pos = *pp;
	}
    }
    જો (pos.lnum != 0)
    {
	જો (charcol)
	    pos.col = buf_byteidx_to_charidx(curbuf, pos.lnum, pos.col);
	પાછા &pos;
    }

    pos.coladd = 0;

    જો (name[0] == 'w' && dollar_lnum)
    {
	// the "w_valid" flags are not reset when moving the cursor, but they
	// do matter for update_topline() and validate_botline().
	check_cursor_moved(curwin);

	pos.col = 0;
	જો (name[1] == '0')		// "w0": first visible line
	{
#ifdef FEAT_TERMINAL
	    જો (bt_terminal(curwin->w_buffer)
				    && curwin->w_buffer->b_term != ખાલી
				    && !term_in_normal_mode(curwin->w_buffer))
		may_move_terminal_to_buffer(curwin->w_buffer->b_term, TRUE);
#endif
	    update_topline();
	    // In silent Ex mode topline is zero, but that's not a valid line
	    // number; use one instead.
	    pos.lnum = curwin->w_topline > 0 ? curwin->w_topline : 1;
	    પાછા &pos;
	}
	નહીંતર જો (name[1] == '$')	// "w$": last visible line
	{
#ifdef FEAT_TERMINAL
	    જો (bt_terminal(curwin->w_buffer)
				    && curwin->w_buffer->b_term != ખાલી
				    && !term_in_normal_mode(curwin->w_buffer))
		may_move_terminal_to_buffer(curwin->w_buffer->b_term, TRUE);
#endif
	    validate_botline();
	    // In silent Ex mode botline is zero, return zero then.
	    pos.lnum = curwin->w_botline > 0 ? curwin->w_botline - 1 : 0;
	    પાછા &pos;
	}
    }
    નહીંતર જો (name[0] == '$')		// last column or line
    {
	જો (dollar_lnum)
	{
	    pos.lnum = curbuf->b_ml.ml_લીટી_ગણતરી;
	    pos.col = 0;
	}
	નહીંતર
	{
	    pos.lnum = curwin->w_cursor.lnum;
	    જો (charcol)
		pos.col = (colnr_T)mb_charlen(ml_get_curline());
	    નહીંતર
		pos.col = ml_get_curline_len();
	}
	પાછા &pos;
    }
    જો (in_vim9script() && error)
	semsg(_(e_invalid_કિંમત_for_લીટી_સંખ્યા_str), name);
    પાછા ખાલી;
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
    પૂર્ણાંક
list2fpos(
    typval_T	*arg,
    pos_T	*posp,
    પૂર્ણાંક		*fnump,
    colnr_T	*curswantp,
    પૂર્ણાંક		charcol)
{
    list_T	*l = arg->vval.v_list;
    long	i = 0;
    long	n;

    // List must be: [fnum, lnum, col, coladd, curswant], where "fnum" is only
    // there when "fnump" isn't NULL; "coladd" and "curswant" are optional.
    જો (arg->v_type != VAR_LIST
	    || l == ખાલી
	    || l->lv_len < (fnump == ખાલી ? 2 : 3)
	    || l->lv_len > (fnump == ખાલી ? 4 : 5))
	પાછા FAIL;

    જો (fnump != ખાલી)
    {
	n = list_find_nr(l, i++, ખાલી);	// fnum
	જો (n < 0)
	    પાછા FAIL;
	જો (n == 0)
	    n = curbuf->b_fnum;		// current buffer
	*fnump = n;
    }

    n = list_find_nr(l, i++, ખાલી);	// lnum
    જો (n < 0)
	પાછા FAIL;
    posp->lnum = n;

    n = list_find_nr(l, i++, ખાલી);	// col
    જો (n < 0)
	પાછા FAIL;
    // If character position is specified, then convert to byte position
    // If the line number is zero use the cursor line.
    જો (charcol)
    {
	buf_T	*buf;

	// Get the text for the specified line in a loaded buffer
	buf = buflist_findnr(fnump == ખાલી ? curbuf->b_fnum : *fnump);
	જો (buf == ખાલી || buf->b_ml.ml_mfp == ખાલી)
	    પાછા FAIL;

	n = buf_charidx_to_byteidx(buf,
		  posp->lnum == 0 ? curwin->w_cursor.lnum : posp->lnum, n) + 1;
    }
    posp->col = n;

    n = list_find_nr(l, i, ખાલી);	// off
    જો (n < 0)
	posp->coladd = 0;
    નહીંતર
	posp->coladd = n;

    જો (curswantp != ખાલી)
	*curswantp = list_find_nr(l, i + 1, ખાલી);  // curswant

    પાછા OK;
}

/*
 * Get the length of an environment variable name.
 * Advance "arg" to the first character after the name.
 * Return 0 for error.
 */
    પૂર્ણાંક
get_env_len(char_u **arg)
{
    char_u	*p;
    પૂર્ણાંક		len;

    માટે (p = *arg; vim_isIDc(*p); ++p)
	;
    જો (p == *arg)	    // no name found
	પાછા 0;

    len = (પૂર્ણાંક)(p - *arg);
    *arg = p;
    પાછા len;
}

/*
 * Get the length of the name of a function or internal variable.
 * "arg" is advanced to after the name.
 * Return 0 if something is wrong.
 */
    પૂર્ણાંક
get_id_len(char_u **arg)
{
    char_u	*p;
    પૂર્ણાંક		len;

    // Find the end of the name.
    માટે (p = *arg; eval_isnamec(*p); ++p)
    {
	જો (*p == ':')
	{
	    // "s:" is start of "s:var", but "n:" is not and can be used in
	    // slice "[n:]".  Also "xx:" is not a namespace.
	    len = (પૂર્ણાંક)(p - *arg);
	    જો ((len == 1 && vim_strchr(NAMESPACE_CHAR, **arg) == ખાલી)
		    || len > 1)
		તોડો;
	}
    }
    જો (p == *arg)	    // no name found
	પાછા 0;

    len = (પૂર્ણાંક)(p - *arg);
    *arg = p;

    પાછા len;
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
    પૂર્ણાંક
get_name_len(
    char_u	**arg,
    char_u	**alias,
    પૂર્ણાંક		evaluate,
    પૂર્ણાંક		verbose)
{
    પૂર્ણાંક		len;
    char_u	*p;
    char_u	*પદાવલિ_start;
    char_u	*પદાવલિ_end;

    *alias = ખાલી;  // default to no alias

    જો ((*arg)[0] == K_SPECIAL && (*arg)[1] == KS_EXTRA
						  && (*arg)[2] == (પૂર્ણાંક)KE_SNR)
    {
	// hard coded <SNR>, already translated
	*arg += 3;
	પાછા get_id_len(arg) + 3;
    }
    len = eval_fname_script(*arg);
    જો (len > 0)
    {
	// literal "<SID>", "s:" or "<SNR>"
	*arg += len;
    }

    /*
     * Find the end of the name; check for {} construction.
     */
    p = find_name_end(*arg, &પદાવલિ_start, &પદાવલિ_end,
					       len > 0 ? 0 : FNE_CHECK_START);
    જો (પદાવલિ_start != ખાલી)
    {
	char_u	*temp_string;

	જો (!evaluate)
	{
	    len += (પૂર્ણાંક)(p - *arg);
	    *arg = skipwhite(p);
	    પાછા len;
	}

	/*
	 * Include any <SID> etc in the expanded string:
	 * Thus the -len here.
	 */
	temp_string = make_expanded_name(*arg - len, પદાવલિ_start, પદાવલિ_end, p);
	જો (temp_string == ખાલી)
	    પાછા -1;
	*alias = temp_string;
	*arg = skipwhite(p);
	પાછા (પૂર્ણાંક)STRLEN(temp_string);
    }

    len += get_id_len(arg);
    // Only give an error when there is something, otherwise it will be
    // reported at a higher level.
    જો (len == 0 && verbose && **arg != NUL)
	semsg(_(e_invalid_expression_str), *arg);

    પાછા len;
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
    char_u	**પદાવલિ_start,
    char_u	**પદાવલિ_end,
    પૂર્ણાંક		flags)
{
    પૂર્ણાંક		mb_nest = 0;
    પૂર્ણાંક		br_nest = 0;
    char_u	*p;
    પૂર્ણાંક		len;
    પૂર્ણાંક		allow_curly = !in_vim9script();

    જો (પદાવલિ_start != ખાલી)
    {
	*પદાવલિ_start = ખાલી;
	*પદાવલિ_end = ખાલી;
    }

    // Quick check for valid starting character.
    જો ((flags & FNE_CHECK_START) && !eval_isnamec1(*arg)
					      && (*arg != '{' || !allow_curly))
	પાછા arg;

    માટે (p = arg; *p != NUL
		    && (eval_isnamec(*p)
			|| (*p == '{' && allow_curly)
			|| ((flags & FNE_INCL_BR) && (*p == '['
					 || (*p == '.' && eval_isdictc(p[1]))))
			|| mb_nest != 0
			|| br_nest != 0); MB_PTR_ADV(p))
    {
	જો (*p == '\'')
	{
	    // skip over 'string' to avoid counting [ and ] inside it.
	    માટે (p = p + 1; *p != NUL && *p != '\''; MB_PTR_ADV(p))
		;
	    જો (*p == NUL)
		તોડો;
	}
	નહીંતર જો (*p == '"')
	{
	    // skip over "str\"ing" to avoid counting [ and ] inside it.
	    માટે (p = p + 1; *p != NUL && *p != '"'; MB_PTR_ADV(p))
		જો (*p == '\\' && p[1] != NUL)
		    ++p;
	    જો (*p == NUL)
		તોડો;
	}
	નહીંતર જો (br_nest == 0 && mb_nest == 0 && *p == ':')
	{
	    // "s:" is start of "s:var", but "n:" is not and can be used in
	    // slice "[n:]".  Also "xx:" is not a namespace. But {ns}: is.
	    len = (પૂર્ણાંક)(p - arg);
	    જો ((len == 1 && vim_strchr(NAMESPACE_CHAR, *arg) == ખાલી)
		    || (len > 1 && p[-1] != '}'))
		તોડો;
	}

	જો (mb_nest == 0)
	{
	    જો (*p == '[')
		++br_nest;
	    નહીંતર જો (*p == ']')
		--br_nest;
	}

	જો (br_nest == 0 && allow_curly)
	{
	    જો (*p == '{')
	    {
		mb_nest++;
		જો (પદાવલિ_start != ખાલી && *પદાવલિ_start == ખાલી)
		    *પદાવલિ_start = p;
	    }
	    નહીંતર જો (*p == '}')
	    {
		mb_nest--;
		જો (પદાવલિ_start != ખાલી && mb_nest == 0 && *પદાવલિ_end == ખાલી)
		    *પદાવલિ_end = p;
	    }
	}
    }

    પાછા p;
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
    સ્થિર char_u *
make_expanded_name(
    char_u	*in_start,
    char_u	*પદાવલિ_start,
    char_u	*પદાવલિ_end,
    char_u	*in_end)
{
    char_u	c1;
    char_u	*retval = ખાલી;
    char_u	*temp_result;

    જો (પદાવલિ_end == ખાલી || in_end == ખાલી)
	પાછા ખાલી;
    *પદાવલિ_start	= NUL;
    *પદાવલિ_end = NUL;
    c1 = *in_end;
    *in_end = NUL;

    temp_result = eval_to_string(પદાવલિ_start + 1, FALSE, FALSE);
    જો (temp_result != ખાલી)
    {
	size_t	retvalsize = (size_t)(પદાવલિ_start - in_start)
				+ STRLEN(temp_result)
				+ (size_t)(in_end - પદાવલિ_end) + 1;

	retval = alloc(retvalsize);
	જો (retval != ખાલી)
	    vim_snprintf((અક્ષર *)retval, retvalsize, "%s%s%s",
				in_start, temp_result, પદાવલિ_end + 1);
    }
    vim_free(temp_result);

    *in_end = c1;		// put char back for error messages
    *પદાવલિ_start = '{';
    *પદાવલિ_end = '}';

    જો (retval != ખાલી)
    {
	temp_result = find_name_end(retval, &પદાવલિ_start, &પદાવલિ_end, 0);
	જો (પદાવલિ_start != ખાલી)
	{
	    // Further expansion!
	    temp_result = make_expanded_name(retval, પદાવલિ_start,
						       પદાવલિ_end, temp_result);
	    vim_free(retval);
	    retval = temp_result;
	}
    }

    પાછા retval;
}

/*
 * Return TRUE if character "c" can be used in a variable or function name.
 * Does not include '{' or '}' for magic braces.
 */
    પૂર્ણાંક
eval_isnamec(પૂર્ણાંક c)
{
    પાછા ASCII_ISALNUM(c) || c == '_' || c == ':' || c == AUTOLOAD_CHAR;
}

/*
 * Return TRUE if character "c" can be used as the first character in a
 * variable or function name (excluding '{' and '}').
 */
    પૂર્ણાંક
eval_isnamec1(પૂર્ણાંક c)
{
    પાછા ASCII_ISALPHA(c) || c == '_';
}

/*
 * Return TRUE if character "c" can be used as the first character of a
 * dictionary key.
 */
    પૂર્ણાંક
eval_isdictc(પૂર્ણાંક c)
{
    પાછા ASCII_ISALNUM(c) || c == '_';
}

/*
 * Return true when "p" points to "->" followed by a method name or a lambda.
 * Only called for Vim9 script.
 */
    સ્થિર bool
method_call_follows(char_u *p)
{
    જો (p[0] != '-' || p[1] != '>')
	પાછા false;

    char_u	*after = skipwhite(p + 2);

    પાછા ASCII_ISALPHA(*after) || *after == '(';
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
    પૂર્ણાંક
handle_subscript(
    char_u	**arg,
    char_u	*name_start,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    પૂર્ણાંક		verbose)	// give error messages
{
    પૂર્ણાંક		evaluate = evalarg != ખાલી
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    પૂર્ણાંક		ret = OK;
    dict_T	*selfdict = ખાલી;
    પૂર્ણાંક		check_white = TRUE;
    પૂર્ણાંક		getnext;
    char_u	*p;

    જ્યારે (ret == OK)
    {
	// When at the end of the line and ".name" or a method call follows in
	// the next line then consume the line break.
	p = eval_next_non_blank(*arg, evalarg, &getnext);
	જો (getnext
	    && ((*p == '.'
		    && ((rettv->v_type == VAR_DICT && eval_isdictc(p[1]))
			|| rettv->v_type == VAR_CLASS
			|| rettv->v_type == VAR_OBJECT))
		|| method_call_follows(p)))
	{
	    *arg = eval_next_લીટી(*arg, evalarg);
	    p = *arg;
	    check_white = FALSE;
	}

	જો (rettv->v_type == VAR_ANY)
	{
	    char_u	*exp_name;
	    પૂર્ણાંક		cc;
	    પૂર્ણાંક		idx;
	    ufunc_T	*ufunc;
	    type_T	*type;

	    // Found script from "import {name} as name", script item name must
	    // follow.  "rettv->vval.v_number" has the script ID.
	    જો (**arg != '.')
	    {
		જો (verbose)
		    semsg(_(e_expected_dot_after_name_str),
					name_start != ખાલી ? name_start: *arg);
		ret = FAIL;
		તોડો;
	    }
	    ++*arg;
	    જો (IS_WHITE_OR_NUL(**arg))
	    {
		જો (verbose)
		    emsg(_(e_no_white_space_allowed_after_dot));
		ret = FAIL;
		તોડો;
	    }

	    // isolate the name
	    exp_name = *arg;
	    જ્યારે (eval_isnamec(**arg))
		++*arg;
	    cc = **arg;
	    **arg = NUL;

	    idx = find_exported(rettv->vval.v_સંખ્યા, exp_name, &ufunc, &type,
		       evalarg == ખાલી ? ખાલી : evalarg->eval_cctx,
		       evalarg == ખાલી ? ખાલી : evalarg->eval_cstack, verbose);
	    **arg = cc;

	    જો (idx < 0 && ufunc == ખાલી)
	    {
		ret = FAIL;
		તોડો;
	    }
	    જો (idx >= 0)
	    {
		scriptitem_T    *si = SCRIPT_ITEM(rettv->vval.v_સંખ્યા);
		svar_T		*sv = ((svar_T *)si->sn_var_vals.ga_data) + idx;

		copy_tv(sv->sv_tv, rettv);
	    }
	    નહીંતર
	    {
		rettv->v_type = VAR_FUNC;
		જો (**arg == '<')
		{
		    char_u *s = get_generic_func_name(ufunc, arg);
		    જો (s != ખાલી)
			rettv->vval.v_string = s;
		    નહીંતર
			ret = FAIL;
		}
		નહીંતર
		{
		    rettv->vval.v_string =
			vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
		}
	    }
	    ચાલુ;
	}

	જો ((**arg == '(' && (!evaluate || rettv->v_type == VAR_FUNC
			    || rettv->v_type == VAR_PARTIAL))
		    && (!check_white || !VIM_ISWHITE(*(*arg - 1))))
	{
	    ret = call_func_rettv(arg, evalarg, rettv, evaluate,
							       selfdict, ખાલી);

	    // Stop the expression evaluation when immediately aborting on
	    // error, or when an interrupt occurred or an exception was thrown
	    // but not caught.
	    જો (aborting())
	    {
		જો (ret == OK)
		    clear_tv(rettv);
		ret = FAIL;
	    }
	    dict_unref(selfdict);
	    selfdict = ખાલી;
	}
	નહીંતર જો (p[0] == '-' && p[1] == '>')
	{
	    જો (in_vim9script())
		*arg = skipwhite(p + 2);
	    નહીંતર
		*arg = p + 2;
	    જો (ret == OK && evaluate && rettv->v_type == VAR_VOID)
	    {
		જો (verbose)
		    emsg(_(e_cannot_use_void_કિંમત));
		ret = FAIL;
	    }
	    નહીંતર જો (VIM_ISWHITE(**arg))
	    {
		emsg(_(e_no_white_space_allowed_before_parenthesis));
		ret = FAIL;
	    }
	    નહીંતર જો ((**arg == '{' && !in_vim9script()) || **arg == '(')
		// expr->{lambda}() or expr->(lambda)()
		ret = eval_lambda(arg, rettv, evalarg, verbose);
	    નહીંતર
		// expr->name()
		ret = eval_method(arg, rettv, evalarg, verbose);
	}
	// "." is ".name" lookup when we found a dict or when evaluating and
	// scriptversion is at least 2, where string concatenation is "..".
	નહીંતર જો (**arg == '['
		|| (**arg == '.' && (rettv->v_type == VAR_DICT
			|| (!evaluate
			    && (*arg)[1] != '.'
			    && !in_old_script(2)))))
	{
	    dict_unref(selfdict);
	    જો (rettv->v_type == VAR_DICT)
	    {
		selfdict = rettv->vval.v_dict;
		જો (selfdict != ખાલી)
		    ++selfdict->dv_refcount;
	    }
	    નહીંતર
		selfdict = ખાલી;
	    જો (eval_index(arg, rettv, evalarg, verbose) == FAIL)
	    {
		clear_tv(rettv);
		ret = FAIL;
	    }
	}
	નહીંતર જો (**arg == '.' && (rettv->v_type == VAR_CLASS
					       || rettv->v_type == VAR_OBJECT))
	{
	    // class member: SomeClass.varname
	    // class method: SomeClass.SomeMethod()
	    // class constructor: SomeClass.new()
	    // object member: someObject.varname
	    // object method: someObject.SomeMethod()
	    જો (class_object_index(arg, rettv, evalarg, verbose) == FAIL)
	    {
		clear_tv(rettv);
		ret = FAIL;
	    }
	}
	નહીંતર
	    તોડો;
    }

    // Turn "dict.Func" into a partial for "Func" bound to "dict".
    // Don't do this when "Func" is already a partial that was bound
    // explicitly (pt_auto is FALSE).
    જો (selfdict != ખાલી
	    && (rettv->v_type == VAR_FUNC
		|| (rettv->v_type == VAR_PARTIAL
		    && (rettv->vval.v_partial->pt_auto
			|| rettv->vval.v_partial->pt_dict == ખાલી))))
	selfdict = make_partial(selfdict, rettv);

    dict_unref(selfdict);
    પાછા ret;
}

/*
 * Make a copy of an item.
 * Lists and Dictionaries are also copied.  A deep copy if "deep" is set.
 * "top" is TRUE for the toplevel of copy().
 * For deepcopy() "copyID" is zero for a full copy or the ID for when a
 * reference to an already copied list/dict can be used.
 * Returns FAIL or OK.
 */
    પૂર્ણાંક
item_copy(
    typval_T	*from,
    typval_T	*to,
    પૂર્ણાંક		deep,
    પૂર્ણાંક		ટોચ,
    પૂર્ણાંક		copyID)
{
    સ્થિર પૂર્ણાંક	recurse = 0;
    પૂર્ણાંક		ret = OK;

    જો (recurse >= DICT_MAXNEST)
    {
	emsg(_(e_variable_nested_too_deep_for_making_copy));
	પાછા FAIL;
    }
    ++recurse;

    પસંદગી (from->v_type)
    {
	સ્થિતિ VAR_સંખ્યા:
	સ્થિતિ VAR_FLOAT:
	સ્થિતિ VAR_STRING:
	સ્થિતિ VAR_FUNC:
	સ્થિતિ VAR_PARTIAL:
	સ્થિતિ VAR_BOOL:
	સ્થિતિ VAR_SPECIAL:
	સ્થિતિ VAR_JOB:
	સ્થિતિ VAR_CHANNEL:
	સ્થિતિ VAR_INSTR:
	સ્થિતિ VAR_CLASS:
	સ્થિતિ VAR_OBJECT:
	સ્થિતિ VAR_TYPEALIAS:
	    copy_tv(from, to);
	    તોડો;
	સ્થિતિ VAR_LIST:
	    to->v_type = VAR_LIST;
	    to->v_lock = 0;
	    જો (from->vval.v_list == ખાલી)
		to->vval.v_list = ખાલી;
	    નહીંતર જો (copyID != 0 && from->vval.v_list->lv_copyID == copyID)
	    {
		// use the copy made earlier
		to->vval.v_list = from->vval.v_list->lv_copylist;
		++to->vval.v_list->lv_refcount;
	    }
	    નહીંતર
	    {
		to->vval.v_list = list_copy(from->vval.v_list,
							    deep, ટોચ, copyID);
		જો (to->vval.v_list == ખાલી)
		    ret = FAIL;
	    }
	    તોડો;
	સ્થિતિ VAR_TUPLE:
	    to->v_type = VAR_TUPLE;
	    to->v_lock = 0;
	    જો (from->vval.v_tuple == ખાલી)
		to->vval.v_tuple = ખાલી;
	    નહીંતર જો (copyID != 0 && from->vval.v_tuple->tv_copyID == copyID)
	    {
		// use the copy made earlier
		to->vval.v_tuple = from->vval.v_tuple->tv_copytuple;
		++to->vval.v_tuple->tv_refcount;
	    }
	    નહીંતર
	    {
		to->vval.v_tuple = tuple_copy(from->vval.v_tuple,
							    deep, ટોચ, copyID);
		જો (to->vval.v_tuple == ખાલી)
		    ret = FAIL;
	    }
	    તોડો;
	સ્થિતિ VAR_BLOB:
	    to->v_type = VAR_BLOB;
	    to->v_lock = 0;
	    જો (from->vval.v_blob == ખાલી)
		to->vval.v_blob = ખાલી;
	    નહીંતર
	    {
		to->vval.v_blob = blob_copy(from->vval.v_blob);
		જો (to->vval.v_blob == ખાલી)
		    ret = FAIL;
	    }
	    તોડો;
	સ્થિતિ VAR_DICT:
	    to->v_type = VAR_DICT;
	    to->v_lock = 0;
	    જો (from->vval.v_dict == ખાલી)
		to->vval.v_dict = ખાલી;
	    નહીંતર જો (copyID != 0 && from->vval.v_dict->dv_copyID == copyID)
	    {
		// use the copy made earlier
		to->vval.v_dict = from->vval.v_dict->dv_copydict;
		++to->vval.v_dict->dv_refcount;
	    }
	    નહીંતર
	    {
		to->vval.v_dict = dict_copy(from->vval.v_dict,
							    deep, ટોચ, copyID);
		જો (to->vval.v_dict == ખાલી)
		    ret = FAIL;
	    }
	    તોડો;
	સ્થિતિ VAR_UNKNOWN:
	સ્થિતિ VAR_ANY:
	સ્થિતિ VAR_VOID:
	    internal_error_no_abort("item_copy(UNKNOWN)");
	    ret = FAIL;
    }
    --recurse;
    પાછા ret;
}

    શૂન્ય
echo_one(typval_T *rettv, પૂર્ણાંક with_space, પૂર્ણાંક *atstart, પૂર્ણાંક *needclr)
{
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    char_u	*p = echo_string(rettv, &tofree, numbuf, get_copyID());

    જો (*atstart)
    {
	*atstart = FALSE;
	// Call msg_start() after eval1(), evaluating the expression
	// may cause a message to appear.
	જો (with_space)
	{
	    // Mark the saved text as finishing the line, so that what
	    // follows is displayed on a new line when scrolling back
	    // at the more prompt.
	    સંદેશ_sb_eol();
	    સંદેશ_start();
	}
    }
    નહીંતર જો (with_space)
	સંદેશ_puts_attr(" ", echo_attr);

    જો (p != ખાલી)
	માટે ( ; *p != NUL && !got_int; ++p)
	{
	    જો (*p == '\n' || *p == '\r' || *p == TAB)
	    {
		જો (*p != TAB && *needclr)
		{
		    // remove any text still there from the command
		    સંદેશ_clr_eos();
		    *needclr = FALSE;
		}
		સંદેશ_putchar_attr(*p, echo_attr);
	    }
	    નહીંતર
	    {
		જો (has_mbyte)
		{
		    પૂર્ણાંક i = (*mb_ptr2len)(p);

		    (શૂન્ય)સંદેશ_outtrans_len_attr(p, i, echo_attr);
		    p += i - 1;
		}
		નહીંતર
		    (શૂન્ય)સંદેશ_outtrans_len_attr(p, 1, echo_attr);
	    }
	}
    vim_free(tofree);
}

/*
 * ":echo expr1 ..."	print each argument separated with a space, add a
 *			newline at the end.
 * ":echon expr1 ..."	print each argument plain.
 */
    શૂન્ય
ex_echo(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    typval_T	rettv;
    char_u	*arg_start;
    પૂર્ણાંક		needclr = TRUE;
    પૂર્ણાંક		atstart = TRUE;
    પૂર્ણાંક		did_emsg_before = did_emsg;
    પૂર્ણાંક		called_emsg_before = called_emsg;
    evalarg_T	evalarg;

    fill_evalarg_from_eap(&evalarg, eap, eap->skip);

    જો (eap->skip)
	++emsg_skip;
    જ્યારે ((!ends_excmd2(eap->cmd, arg) || *arg == '"') && !got_int)
    {
	// If eval1() causes an error message the text from the command may
	// still need to be cleared. E.g., "echo 22,44".
	need_clr_eos = needclr;

	arg_start = arg;
	જો (eval1(&arg, &rettv, &evalarg) == FAIL)
	{
	    /*
	     * Report the invalid expression unless the expression evaluation
	     * has been cancelled due to an aborting error, an interrupt, or an
	     * exception.
	     */
	    જો (!aborting() && did_emsg == did_emsg_before
					  && called_emsg == called_emsg_before)
		semsg(_(e_invalid_expression_str), arg_start);
	    need_clr_eos = FALSE;
	    તોડો;
	}
	need_clr_eos = FALSE;

	જો (!eap->skip)
	{
	    જો (rettv.v_type == VAR_VOID)
	    {
		semsg(_(e_expression_does_not_result_in_કિંમત_str), arg_start);
		તોડો;
	    }
	    echo_one(&rettv, eap->cmdidx == CMD_echo, &atstart, &needclr);
	}

	clear_tv(&rettv);
	arg = skipwhite(arg);
    }
    set_nextcmd(eap, arg);
    clear_evalarg(&evalarg, eap);

    જો (eap->skip)
	--emsg_skip;
    નહીંતર
    {
	// remove text that may still be there from the command
	જો (needclr)
	    સંદેશ_clr_eos();
	જો (eap->cmdidx == CMD_echo)
	    સંદેશ_end();
    }
}

/*
 * ":echohl {name}".
 */
    શૂન્ય
ex_echohl(exarg_T *eap)
{
    echo_attr = syn_name2attr(eap->arg);
}

/*
 * Returns the :echo attribute
 */
    પૂર્ણાંક
get_echo_attr(શૂન્ય)
{
    પાછા echo_attr;
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
    શૂન્ય
ex_execute(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    typval_T	rettv;
    પૂર્ણાંક		ret = OK;
    char_u	*p;
    garray_T	ga;
    પૂર્ણાંક		len;
    long	start_lnum = SOURCING_LNUM;

    ga_init2(&ga, 1, 80);

    જો (eap->skip)
	++emsg_skip;
    જ્યારે (!ends_excmd2(eap->cmd, arg) || *arg == '"')
    {
	ret = eval1_emsg(&arg, &rettv, eap);
	જો (ret == FAIL)
	    તોડો;

	જો (!eap->skip)
	{
	    char_u   buf[NUMBUFLEN];

	    જો (eap->cmdidx == CMD_execute)
	    {
		જો (rettv.v_type == VAR_CHANNEL || rettv.v_type == VAR_JOB)
		{
		    semsg(_(e_using_invalid_કિંમત_as_string_str),
						  vartype_name(rettv.v_type));
		    p = ખાલી;
		}
		નહીંતર
		    p = tv_get_string_buf(&rettv, buf);
	    }
	    નહીંતર
		p = tv_stringify(&rettv, buf);
	    જો (p == ખાલી)
	    {
		clear_tv(&rettv);
		ret = FAIL;
		તોડો;
	    }
	    len = (પૂર્ણાંક)STRLEN(p);
	    જો (ga_grow(&ga, len + 2) == FAIL)
	    {
		clear_tv(&rettv);
		ret = FAIL;
		તોડો;
	    }
	    જો (ga.ga_len)
		((char_u *)(ga.ga_data))[ga.ga_len++] = ' ';
	    STRCPY((char_u *)(ga.ga_data) + ga.ga_len, p);
	    ga.ga_len += len;
	}

	clear_tv(&rettv);
	arg = skipwhite(arg);
    }

    જો (ret != FAIL && ga.ga_data != ખાલી)
    {
	// use the first line of continuation lines for messages
	SOURCING_LNUM = start_lnum;

	જો (eap->cmdidx == CMD_echomsg
		|| eap->cmdidx == CMD_echowindow
		|| eap->cmdidx == CMD_echoerr)
	{
	    // Mark the already saved text as finishing the line, so that what
	    // follows is displayed on a new line when scrolling back at the
	    // more prompt.
	    સંદેશ_sb_eol();
	}

	જો (eap->cmdidx == CMD_echomsg)
	{
	    સંદેશ_attr(ga.ga_data, echo_attr);
	    out_flush();
	}
	નહીંતર જો (eap->cmdidx == CMD_echowindow)
	{
#ifdef HAS_MESSAGE_WINDOW
	    start_echowindow(eap->addr_ગણતરી > 0 ? eap->line2 : 0);
#endif
	    સંદેશ_attr(ga.ga_data, echo_attr);
#ifdef HAS_MESSAGE_WINDOW
	    end_echowindow();
#endif
	}
	નહીંતર જો (eap->cmdidx == CMD_echoconsole)
	{
	    ui_write(ga.ga_data, (પૂર્ણાંક)STRLEN(ga.ga_data), TRUE);
	    ui_write((char_u *)"\r\n", 2, TRUE);
	}
	નહીંતર જો (eap->cmdidx == CMD_echoerr)
	{
	    પૂર્ણાંક		save_did_emsg = did_emsg;

	    // We don't want to abort following commands, restore did_emsg.
	    emsg(ga.ga_data);
	    જો (!force_abort)
		did_emsg = save_did_emsg;
	}
	નહીંતર જો (eap->cmdidx == CMD_execute)
	{
	    પૂર્ણાંક save_sticky_cmdmod_flags = sticky_cmdmod_flags;

	    // "legacy exe cmd" and "vim9cmd exe cmd" applies to "cmd".
	    sticky_cmdmod_flags = cmdmod.cmod_flags
						& (CMOD_LEGACY | CMOD_VIM9CMD);
	    do_cmdline((char_u *)ga.ga_data,
		       eap->ea_getline, eap->cookie, DOCMD_NOWAIT|DOCMD_VERBOSE);
	    sticky_cmdmod_flags = save_sticky_cmdmod_flags;
	}
    }

    ga_clear(&ga);

    જો (eap->skip)
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
find_option_end(char_u **arg, પૂર્ણાંક *scope)
{
    char_u	*p = *arg;

    ++p;
    જો (*p == 'g' && p[1] == ':')
    {
	*scope = OPT_GLOBAL;
	p += 2;
    }
    નહીંતર જો (*p == 'l' && p[1] == ':')
    {
	*scope = OPT_LOCAL;
	p += 2;
    }
    નહીંતર
	*scope = 0;

    જો (!ASCII_ISALPHA(*p))
	પાછા ખાલી;
    *arg = p;

    જો (p[0] == 't' && p[1] == '_' && p[2] != NUL && p[3] != NUL)
	p += 4;	    // termcap option
    નહીંતર
	જ્યારે (ASCII_ISALPHA(*p))
	    ++p;
    પાછા p;
}

/*
 * Display script name where an item was last set.
 * Should only be invoked when 'verbose' is non-zero.
 */
    શૂન્ય
last_set_સંદેશ(sctx_T script_ctx)
{
    char_u *p;

    જો (script_ctx.sc_sid == 0)
	પાછા;

    p = home_replace_save(ખાલી, get_scriptname(script_ctx.sc_sid));
    જો (p == ખાલી)
	પાછા;

    verbose_enter();
    સંદેશ_puts(_("\n\tLast set from "));
    સંદેશ_puts((અક્ષર *)p);
    જો (script_ctx.sc_lnum > 0)
    {
	સંદેશ_puts(_(લીટી_સંદેશ));
	સંદેશ_outnum((long)script_ctx.sc_lnum);
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
    typval_T	*પદાવલિ,
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
    જો (regmatch.regprog != ખાલી)
    {
	char_u	*tail = str;
	char_u	*end = str + len;
	પૂર્ણાંક	do_all = (flags[0] == 'g');
	પૂર્ણાંક	sublen;
	પૂર્ણાંક	i;
	char_u	*zero_width = ખાલી;

	જ્યારે (vim_regexec_nl(&regmatch, str, (colnr_T)(tail - str)))
	{
	    // Skip empty match except for first match.
	    જો (regmatch.startp[0] == regmatch.endp[0])
	    {
		જો (zero_width == regmatch.startp[0])
		{
		    // avoid getting stuck on a match with an empty string
		    i = mb_ptr2len(tail);
		    mch_memmove((char_u *)ga.ga_data + ga.ga_len, tail,
								   (size_t)i);
		    ga.ga_len += i;
		    tail += i;
		    ચાલુ;
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
	    sublen = vim_regsub(&regmatch, sub, પદાવલિ, tail, 0, REGSUB_MAGIC);
	    જો (sublen <= 0)
	    {
		ga_clear(&ga);
		તોડો;
	    }
	    જો (ga_grow(&ga, (પૂર્ણાંક)((end - tail) + sublen -
			    (regmatch.endp[0] - regmatch.startp[0]))) == FAIL)
	    {
		ga_clear(&ga);
		તોડો;
	    }

	    // copy the text up to where the match is
	    i = (પૂર્ણાંક)(regmatch.startp[0] - tail);
	    mch_memmove((char_u *)ga.ga_data + ga.ga_len, tail, (size_t)i);
	    // add the substituted text
	    (શૂન્ય)vim_regsub(&regmatch, sub, પદાવલિ,
				  (char_u *)ga.ga_data + ga.ga_len + i, sublen,
				  REGSUB_COPY | REGSUB_MAGIC);
	    ga.ga_len += i + sublen - 1;
	    tail = regmatch.endp[0];
	    જો (*tail == NUL)
		તોડો;
	    જો (!do_all)
		તોડો;
	}

	જો (ga.ga_data != ખાલી)
	{
	    STRCPY((અક્ષર *)ga.ga_data + ga.ga_len, tail);
	    ga.ga_len += (પૂર્ણાંક)(end - tail);
	}

	vim_regfree(regmatch.regprog);
    }

    જો (ga.ga_data != ખાલી)
    {
	str = (char_u *)ga.ga_data;
	len = (size_t)ga.ga_len;
    }
    ret = vim_strnsave(str, len);
    ga_clear(&ga);
    જો (p_cpo == empty_option)
	p_cpo = save_cpo;
    નહીંતર
    {
	// Darn, evaluating {sub} expression or {expr} changed the value.
	// If it's still empty it was changed and restored, need to restore in
	// the complicated way.
	જો (*p_cpo == NUL)
	    set_option_કિંમત_give_err((char_u *)"cpo", 0L, save_cpo, 0);
	free_string_option(save_cpo);
    }

    જો (ret_len != ખાલી)
	*ret_len = len;

    પાછા ret;
}
