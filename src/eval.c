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

#അന്തര്ഭാവ "vim.h"

#എങ്കിൽ defined(FEAT_EVAL)

#ifdef VMS
# include <float.h>
#endif

#define NAMESPACE_CHAR	(char_u *)"abglstvw"

സ്ഥിതം പൂർണ്ണസംഖ്യ eval0_simple_funccal(char_u *arg, typval_T *rettv, exarg_T *eap, evalarg_T *evalarg);
സ്ഥിതം പൂർണ്ണസംഖ്യ eval2(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
സ്ഥിതം പൂർണ്ണസംഖ്യ eval3(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
സ്ഥിതം പൂർണ്ണസംഖ്യ eval4(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
സ്ഥിതം പൂർണ്ണസംഖ്യ eval5(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
സ്ഥിതം പൂർണ്ണസംഖ്യ eval6(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
സ്ഥിതം പൂർണ്ണസംഖ്യ eval7(char_u **arg, typval_T *rettv, evalarg_T *evalarg, പൂർണ്ണസംഖ്യ want_string);
സ്ഥിതം പൂർണ്ണസംഖ്യ eval8(char_u **arg, typval_T *rettv, evalarg_T *evalarg, പൂർണ്ണസംഖ്യ want_string);
സ്ഥിതം പൂർണ്ണസംഖ്യ eval9(char_u **arg, typval_T *rettv, evalarg_T *evalarg, പൂർണ്ണസംഖ്യ want_string);
സ്ഥിതം പൂർണ്ണസംഖ്യ eval9_leader(typval_T *rettv, പൂർണ്ണസംഖ്യ numeric_only, char_u *start_leader, char_u **end_leaderp);

സ്ഥിതം char_u *make_expanded_name(char_u *in_start, char_u *വ്യഞ്ജകം_start, char_u *വ്യഞ്ജകം_end, char_u *in_end);

/*
 * Return "n1" divided by "n2", taking care of dividing by zero.
 * If "failed" is not NULL set it to TRUE when dividing by zero fails.
 */
	varnumber_T
അക്കം_divide(varnumber_T n1, varnumber_T n2, പൂർണ്ണസംഖ്യ *failed)
{
    varnumber_T	result;

    എങ്കിൽ (n2 == 0)
    {
	എങ്കിൽ (in_vim9script())
	{
	    emsg(_(e_divide_by_zero));
	    എങ്കിൽ (failed != ശൂന്യ)
		*failed = TRUE;
	}
	എങ്കിൽ (n1 == 0)
	    result = VARNUM_MIN; // similar to NaN
	അല്ലെങ്കിൽ എങ്കിൽ (n1 < 0)
	    result = -VARNUM_MAX;
	അല്ലെങ്കിൽ
	    result = VARNUM_MAX;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (n1 == VARNUM_MIN && n2 == -1)
    {
	// specific case: trying to do VARNUM_MIN / -1 results in a positive
	// number that doesn't fit in varnumber_T and causes an FPE
	result = VARNUM_MAX;
    }
    അല്ലെങ്കിൽ
	result = n1 / n2;

    തിരികെ result;
}

/*
 * Return "n1" modulus "n2", taking care of dividing by zero.
 * If "failed" is not NULL set it to TRUE when dividing by zero fails.
 */
	varnumber_T
അക്കം_modulus(varnumber_T n1, varnumber_T n2, പൂർണ്ണസംഖ്യ *failed)
{
    എങ്കിൽ (n2 == 0 && in_vim9script())
    {
	emsg(_(e_divide_by_zero));
	എങ്കിൽ (failed != ശൂന്യ)
	    *failed = TRUE;
    }
    തിരികെ (n2 == 0) ? 0 : (n1 % n2);
}

/*
 * Initialize the global and v: variables.
 */
    ശൂന്യം
eval_init(ശൂന്യം)
{
    evalvars_init();
    func_init();
}

#എങ്കിൽ defined(EXITFREE)
    ശൂന്യം
eval_clear(ശൂന്യം)
{
    evalvars_clear();
    free_scriptnames();  // must come after evalvars_clear().
    free_locales();

    // autoloaded script names
    free_autoload_scriptnames();

    // unreferenced lists, tuples and dicts
    (ശൂന്യം)garbage_collect(FALSE);

    // functions not garbage collected
    free_all_functions();
}
#endif

    ശൂന്യം
fill_evalarg_from_eap(evalarg_T *evalarg, exarg_T *eap, പൂർണ്ണസംഖ്യ skip)
{
    init_evalarg(evalarg);
    evalarg->eval_flags = skip ? 0 : EVAL_EVALUATE;

    എങ്കിൽ (eap == ശൂന്യ)
	തിരികെ;

    evalarg->eval_cstack = eap->cstack;
    എങ്കിൽ (sourcing_a_script(eap) || eap->ea_getline == get_list_വരി)
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
    പൂർണ്ണസംഖ്യ
eval_to_bool(
    char_u	*arg,
    പൂർണ്ണസംഖ്യ		*error,
    exarg_T	*eap,
    പൂർണ്ണസംഖ്യ		skip,	    // only parse, don't execute
    പൂർണ്ണസംഖ്യ		use_simple_function)
{
    typval_T	tv;
    varnumber_T	retval = FALSE;
    evalarg_T	evalarg;
    പൂർണ്ണസംഖ്യ		r;

    fill_evalarg_from_eap(&evalarg, eap, skip);

    എങ്കിൽ (skip)
	++emsg_skip;
    എങ്കിൽ (use_simple_function)
	r = eval0_simple_funccal(arg, &tv, eap, &evalarg);
    അല്ലെങ്കിൽ
	r = eval0(arg, &tv, eap, &evalarg);
    എങ്കിൽ (r == FAIL)
	*error = TRUE;
    അല്ലെങ്കിൽ
    {
	*error = FALSE;
	എങ്കിൽ (!skip)
	{
	    എങ്കിൽ (in_vim9script())
		retval = tv_get_bool_chk(&tv, error);
	    അല്ലെങ്കിൽ
		retval = (tv_get_സംഖ്യ_chk(&tv, error) != 0);
	    clear_tv(&tv);
	}
    }
    എങ്കിൽ (skip)
	--emsg_skip;
    clear_evalarg(&evalarg, eap);

    തിരികെ (പൂർണ്ണസംഖ്യ)retval;
}

/*
 * Call eval1() and give an error message if not done at a lower level.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
eval1_emsg(char_u **arg, typval_T *rettv, exarg_T *eap)
{
    char_u	*start = *arg;
    പൂർണ്ണസംഖ്യ		ret;
    പൂർണ്ണസംഖ്യ		did_emsg_before = did_emsg;
    പൂർണ്ണസംഖ്യ		called_emsg_before = called_emsg;
    evalarg_T	evalarg;

    fill_evalarg_from_eap(&evalarg, eap, eap != ശൂന്യ && eap->skip);

    ret = eval1(arg, rettv, &evalarg);
    എങ്കിൽ (ret == FAIL)
    {
	// Report the invalid expression unless the expression evaluation has
	// been cancelled due to an aborting error, an interrupt, or an
	// exception, or we already gave a more specific error.
	// Also check called_emsg for when using assert_fails().
	എങ്കിൽ (!aborting() && did_emsg == did_emsg_before
					  && called_emsg == called_emsg_before)
	    semsg(_(e_invalid_expression_str), start);
    }
    clear_evalarg(&evalarg, eap);
    തിരികെ ret;
}

/*
 * Return whether a typval is a valid expression to pass to eval_expr_typval()
 * or eval_expr_to_bool().  An empty string returns FALSE;
 */
    പൂർണ്ണസംഖ്യ
eval_വ്യഞ്ജകം_valid_arg(typval_T *tv)
{
    തിരികെ tv->v_type != VAR_UNKNOWN
	    && (tv->v_type != VAR_STRING
		  || (tv->vval.v_string != ശൂന്യ && *tv->vval.v_string != NUL));
}

/*
 * When calling eval_expr_typval() many times we only need one funccall_T.
 * Returns NULL when no funccall_T is to be used.
 * When returning non-NULL remove_funccal() must be called later.
 */
    funccall_T *
eval_വ്യഞ്ജകം_get_funccal(typval_T *വ്യഞ്ജകം, typval_T *rettv)
{
    എങ്കിൽ (വ്യഞ്ജകം->v_type != VAR_PARTIAL)
	തിരികെ ശൂന്യ;

    partial_T *partial = വ്യഞ്ജകം->vval.v_partial;
    എങ്കിൽ (partial == ശൂന്യ)
	തിരികെ ശൂന്യ;
    എങ്കിൽ (partial->pt_func == ശൂന്യ
	    || partial->pt_func->uf_def_status == UF_NOT_COMPILED)
	തിരികെ ശൂന്യ;

    തിരികെ create_funccal(partial->pt_func, rettv);
}

/*
 * Evaluate a partial.
 * Pass arguments "argv[argc]".
 * "fc_arg" is from eval_expr_get_funccal() or NULL;
 * Return the result in "rettv" and OK or FAIL.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
eval_വ്യഞ്ജകം_partial(
    typval_T	*വ്യഞ്ജകം,
    typval_T	*argv,
    പൂർണ്ണസംഖ്യ		argc,
    funccall_T	*fc_arg,
    typval_T	*rettv)
{
    partial_T	*partial = വ്യഞ്ജകം->vval.v_partial;

    എങ്കിൽ (partial == ശൂന്യ)
	തിരികെ FAIL;

    എങ്കിൽ (partial->pt_func != ശൂന്യ
			&& partial->pt_func->uf_def_status != UF_NOT_COMPILED)
    {
	funccall_T	*fc = fc_arg != ശൂന്യ ? fc_arg
				: create_funccal(partial->pt_func, rettv);
	പൂർണ്ണസംഖ്യ		r;

	എങ്കിൽ (fc == ശൂന്യ)
	    തിരികെ FAIL;

	// Shortcut to call a compiled function with minimal overhead.
	എങ്കിൽ (partial->pt_obj != ശൂന്യ)
	    partial->pt_obj->obj_refcount++;
	r = call_def_function(partial->pt_func, argc, argv, DEF_USE_PT_ARGV,
					partial, partial->pt_obj, fc, rettv);
	എങ്കിൽ (fc_arg == ശൂന്യ)
	    remove_funccal();
	എങ്കിൽ (r == FAIL)
	    തിരികെ FAIL;
    }
    അല്ലെങ്കിൽ
    {
	char_u		*s = partial_name(partial);
	funcexe_T	funcexe;

	എങ്കിൽ (s == ശൂന്യ || *s == NUL)
	    തിരികെ FAIL;

	CLEAR_FIELD(funcexe);
	funcexe.fe_evaluate = TRUE;
	funcexe.fe_partial = partial;
	എങ്കിൽ (call_func(s, -1, rettv, argc, argv, &funcexe) == FAIL)
	    തിരികെ FAIL;
    }

    തിരികെ OK;
}

/*
 * Evaluate an expression which is a function.
 * Pass arguments "argv[argc]".
 * Return the result in "rettv" and OK or FAIL.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
eval_വ്യഞ്ജകം_func(
    typval_T	*വ്യഞ്ജകം,
    typval_T	*argv,
    പൂർണ്ണസംഖ്യ		argc,
    typval_T	*rettv)
{
    funcexe_T	funcexe;
    char_u	buf[NUMBUFLEN];
    char_u	*s;

    എങ്കിൽ (വ്യഞ്ജകം->v_type == VAR_FUNC)
	s = വ്യഞ്ജകം->vval.v_string;
    അല്ലെങ്കിൽ
	s = tv_get_string_buf_chk_strict(വ്യഞ്ജകം, buf, in_vim9script());
    എങ്കിൽ (s == ശൂന്യ || *s == NUL)
	തിരികെ FAIL;

    CLEAR_FIELD(funcexe);
    funcexe.fe_evaluate = TRUE;
    എങ്കിൽ (call_func(s, -1, rettv, argc, argv, &funcexe) == FAIL)
	തിരികെ FAIL;

    തിരികെ OK;
}

/*
 * Evaluate an expression, which is a string.
 * Return the result in "rettv" and OK or FAIL.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
eval_വ്യഞ്ജകം_string(
    typval_T	*വ്യഞ്ജകം,
    typval_T	*rettv)
{
    char_u	*s;
    char_u	buf[NUMBUFLEN];

    s = tv_get_string_buf_chk_strict(വ്യഞ്ജകം, buf, in_vim9script());
    എങ്കിൽ (s == ശൂന്യ)
	തിരികെ FAIL;

    s = skipwhite(s);
    എങ്കിൽ (eval1_emsg(&s, rettv, ശൂന്യ) == FAIL)
	തിരികെ FAIL;

    എങ്കിൽ (*skipwhite(s) != NUL)  // check for trailing chars after expr
    {
	clear_tv(rettv);
	semsg(_(e_invalid_expression_str), s);
	തിരികെ FAIL;
    }

    തിരികെ OK;
}

/*
 * Evaluate an expression, which can be a function, partial or string.
 * Pass arguments "argv[argc]".
 * If "want_func" is TRUE treat a string as a function name, not an expression.
 * "fc_arg" is from eval_expr_get_funccal() or NULL;
 * Return the result in "rettv" and OK or FAIL.
 */
    പൂർണ്ണസംഖ്യ
eval_വ്യഞ്ജകം_typval(
	typval_T    *വ്യഞ്ജകം,
	പൂർണ്ണസംഖ്യ	    want_func,
	typval_T    *argv,
	പൂർണ്ണസംഖ്യ	    argc,
	funccall_T  *fc_arg,
	typval_T    *rettv)
{
    എങ്കിൽ (വ്യഞ്ജകം->v_type == VAR_PARTIAL)
	തിരികെ eval_വ്യഞ്ജകം_partial(വ്യഞ്ജകം, argv, argc, fc_arg, rettv);
    എങ്കിൽ (വ്യഞ്ജകം->v_type == VAR_INSTR)
	തിരികെ exe_typval_instr(വ്യഞ്ജകം, rettv);
    എങ്കിൽ (വ്യഞ്ജകം->v_type == VAR_FUNC || want_func)
	തിരികെ eval_വ്യഞ്ജകം_func(വ്യഞ്ജകം, argv, argc, rettv);

    തിരികെ eval_വ്യഞ്ജകം_string(വ്യഞ്ജകം, rettv);
}

/*
 * Like eval_to_bool() but using a typval_T instead of a string.
 * Works for string, funcref and partial.
 */
    പൂർണ്ണസംഖ്യ
eval_വ്യഞ്ജകം_to_bool(typval_T *വ്യഞ്ജകം, പൂർണ്ണസംഖ്യ *error)
{
    typval_T	rettv;
    പൂർണ്ണസംഖ്യ		res;

    എങ്കിൽ (eval_വ്യഞ്ജകം_typval(വ്യഞ്ജകം, FALSE, ശൂന്യ, 0, ശൂന്യ, &rettv) == FAIL)
    {
	*error = TRUE;
	തിരികെ FALSE;
    }
    res = (tv_get_bool_chk(&rettv, error) != 0);
    clear_tv(&rettv);
    തിരികെ res;
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
    പൂർണ്ണസംഖ്യ		skip)	    // only parse, don't execute
{
    typval_T	tv;
    char_u	*retval;
    evalarg_T	evalarg;

    fill_evalarg_from_eap(&evalarg, eap, skip);
    എങ്കിൽ (skip)
	++emsg_skip;
    എങ്കിൽ (eval0(arg, &tv, eap, &evalarg) == FAIL || skip)
	retval = ശൂന്യ;
    അല്ലെങ്കിൽ
    {
	retval = vim_strsave(tv_get_string(&tv));
	clear_tv(&tv);
    }
    എങ്കിൽ (skip)
	--emsg_skip;
    clear_evalarg(&evalarg, eap);

    തിരികെ retval;
}

/*
 * Initialize "evalarg" for use.
 */
    ശൂന്യം
init_evalarg(evalarg_T *evalarg)
{
    CLEAR_POINTER(evalarg);
    ga_init2(&evalarg->eval_tofree_ga, വലുപ്പം(char_u *), 20);
}

/*
 * If "evalarg->eval_tofree" is not NULL free it later.
 * Caller is expected to overwrite "evalarg->eval_tofree" next.
 */
    സ്ഥിതം ശൂന്യം
free_eval_tofree_later(evalarg_T *evalarg)
{
    എങ്കിൽ (evalarg->eval_tofree == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ (ga_grow(&evalarg->eval_tofree_ga, 1) == OK)
	((char_u **)evalarg->eval_tofree_ga.ga_data)
	    [evalarg->eval_tofree_ga.ga_len++]
	    = evalarg->eval_tofree;
    അല്ലെങ്കിൽ
	vim_free(evalarg->eval_tofree);
}

/*
 * After using "evalarg" filled from "eap": free the memory.
 */
    ശൂന്യം
clear_evalarg(evalarg_T *evalarg, exarg_T *eap)
{
    എങ്കിൽ (evalarg == ശൂന്യ)
	തിരികെ;

    garray_T *etga = &evalarg->eval_tofree_ga;

    എങ്കിൽ (evalarg->eval_tofree != ശൂന്യ || evalarg->eval_using_cmdline)
    {
	എങ്കിൽ (eap != ശൂന്യ)
	{
	    // We may need to keep the original command line, e.g. for
	    // ":let" it has the variable names.  But we may also need
	    // the new one, "nextcmd" points into it.  Keep both.
	    vim_free(eap->cmdline_tofree);
	    eap->cmdline_tofree = *eap->cmdlinep;

	    എങ്കിൽ (evalarg->eval_using_cmdline && etga->ga_len > 0)
	    {
		// "nextcmd" points into the last line in eval_tofree_ga,
		// need to keep it around.
		--etga->ga_len;
		*eap->cmdlinep = ((char_u **)etga->ga_data)[etga->ga_len];
		vim_free(evalarg->eval_tofree);
	    }
	    അല്ലെങ്കിൽ
		*eap->cmdlinep = evalarg->eval_tofree;
	}
	അല്ലെങ്കിൽ
	    vim_free(evalarg->eval_tofree);
	evalarg->eval_tofree = ശൂന്യ;
    }

    ga_clear_strings(etga);
    VIM_CLEAR(evalarg->eval_tofree_lambda);
}

/*
 * Skip over an expression at "*pp".
 * Return FAIL for an error, OK otherwise.
 */
    പൂർണ്ണസംഖ്യ
skip_വ്യഞ്ജകം(char_u **pp, evalarg_T *evalarg)
{
    typval_T	rettv;

    *pp = skipwhite(*pp);
    തിരികെ eval1(pp, &rettv, evalarg);
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
    പൂർണ്ണസംഖ്യ
skip_വ്യഞ്ജകം_concatenate(
	char_u	    **arg,
	char_u	    **start,
	char_u	    **end,
	evalarg_T   *evalarg)
{
    typval_T	rettv;
    പൂർണ്ണസംഖ്യ		res;
    പൂർണ്ണസംഖ്യ		vim9script = in_vim9script();
    garray_T    *gap = evalarg == ശൂന്യ ? ശൂന്യ : &evalarg->eval_ga;
    garray_T    *freegap = evalarg == ശൂന്യ ? ശൂന്യ : &evalarg->eval_freega;
    പൂർണ്ണസംഖ്യ		save_flags = evalarg == ശൂന്യ ? 0 : evalarg->eval_flags;
    പൂർണ്ണസംഖ്യ		evaluate = evalarg == ശൂന്യ
			       ? FALSE : (evalarg->eval_flags & EVAL_EVALUATE);

    എങ്കിൽ (vim9script && evaluate
	       && (evalarg->eval_cookie != ശൂന്യ || evalarg->eval_cctx != ശൂന്യ))
    {
	ga_init2(gap, വലുപ്പം(char_u *), 10);
	// leave room for "start"
	എങ്കിൽ (ga_grow(gap, 1) == OK)
	    ++gap->ga_len;
	ga_init2(freegap, വലുപ്പം(char_u *), 10);
    }
    *start = *arg;

    // Don't evaluate the expression.
    എങ്കിൽ (evalarg != ശൂന്യ)
	evalarg->eval_flags &= ~EVAL_EVALUATE;
    *arg = skipwhite(*arg);
    res = eval1(arg, &rettv, evalarg);
    *end = *arg;
    എങ്കിൽ (evalarg != ശൂന്യ)
	evalarg->eval_flags = save_flags;

    എങ്കിൽ (vim9script && evaluate
	    && (evalarg->eval_cookie != ശൂന്യ || evalarg->eval_cctx != ശൂന്യ))
    {
	എങ്കിൽ (evalarg->eval_ga.ga_len == 1)
	{
	    // just the one line, no need to concatenate
	    ga_clear(gap);
	    gap->ga_itemsize = 0;
	}
	അല്ലെങ്കിൽ
	{
	    char_u	    *p;
	    size_t	    endoff = STRLEN(*arg);

	    // Line breaks encountered, concatenate all the lines.
	    *((char_u **)gap->ga_data) = *start;
	    p = ga_concat_strings(gap, " ");

	    // free the lines only when using getsourceline()
	    എങ്കിൽ (evalarg->eval_cookie != ശൂന്യ)
	    {
		// Do not free the first line, the caller can still use it.
		*((char_u **)gap->ga_data) = ശൂന്യ;
		// Do not free the last line, "arg" points into it, free it
		// later.  Also free "eval_tofree" later if needed.
		free_eval_tofree_later(evalarg);
		evalarg->eval_tofree =
				    ((char_u **)gap->ga_data)[gap->ga_len - 1];
		((char_u **)gap->ga_data)[gap->ga_len - 1] = ശൂന്യ;
		ga_clear_strings(gap);
		ga_clear(freegap);
	    }
	    അല്ലെങ്കിൽ
	    {
		ga_clear(gap);

		// free lines that were explicitly marked for freeing
		ga_clear_strings(freegap);
	    }

	    gap->ga_itemsize = 0;
	    എങ്കിൽ (p == ശൂന്യ)
		തിരികെ FAIL;
	    *start = p;
	    vim_free(evalarg->eval_tofree_lambda);
	    evalarg->eval_tofree_lambda = p;
	    // Compute "end" relative to the end.
	    *end = *start + STRLEN(*start) - endoff;
	}
    }

    തിരികെ res;
}

/*
 * Convert "tv" to a string.
 * When "join_list" is TRUE convert a List or a Tuple into a sequence of lines.
 * Returns an allocated string (NULL when out of memory).
 */
    char_u *
typval2string(typval_T *tv, പൂർണ്ണസംഖ്യ join_list)
{
    garray_T	ga;
    char_u	*retval = ശൂന്യ;

    എങ്കിൽ (join_list && (tv->v_type == VAR_LIST || tv->v_type == VAR_TUPLE))
    {
	എങ്കിൽ (tv->v_type == VAR_LIST)
	{
	    ga_init2(&ga, വലുപ്പം(അക്ഷരം), 80);
	    എങ്കിൽ (tv->vval.v_list != ശൂന്യ)
	    {
		list_join(&ga, tv->vval.v_list, (char_u *)"\n", TRUE, FALSE,
									0);
		എങ്കിൽ (tv->vval.v_list->lv_len > 0)
		    ga_append(&ga, NL);
	    }
	    ga_append(&ga, NUL);
	    retval = (char_u *)ga.ga_data;
	}
	അല്ലെങ്കിൽ
	{
	    // tuple
	    ga_init2(&ga, വലുപ്പം(അക്ഷരം), 80);
	    എങ്കിൽ (tv->vval.v_tuple != ശൂന്യ)
	    {
		tuple_join(&ga, tv->vval.v_tuple, (char_u *)"\n", TRUE, FALSE,
									0);
		എങ്കിൽ (TUPLE_LEN(tv->vval.v_tuple) > 0)
		    ga_append(&ga, NL);
	    }
	    ga_append(&ga, NUL);
	    retval = (char_u *)ga.ga_data;
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (tv->v_type == VAR_LIST
	    || tv->v_type == VAR_TUPLE
	    || tv->v_type == VAR_DICT)
    {
	char_u	*tofree;
	char_u	numbuf[NUMBUFLEN];

	retval = tv2string(tv, &tofree, numbuf, 0);
	// Make a copy if we have a value but it's not in allocated memory.
	എങ്കിൽ (retval != ശൂന്യ && tofree == ശൂന്യ)
	    retval = vim_strsave(retval);
    }
    അല്ലെങ്കിൽ
	retval = vim_strsave(tv_get_string(tv));
    തിരികെ retval;
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
    പൂർണ്ണസംഖ്യ		join_list,
    exarg_T	*eap,
    പൂർണ്ണസംഖ്യ		use_simple_function)
{
    typval_T	tv;
    char_u	*retval;
    evalarg_T	evalarg;
    പൂർണ്ണസംഖ്യ		r;

    fill_evalarg_from_eap(&evalarg, eap, eap != ശൂന്യ && eap->skip);
    എങ്കിൽ (use_simple_function)
	r = eval0_simple_funccal(arg, &tv, ശൂന്യ, &evalarg);
    അല്ലെങ്കിൽ
	r = eval0(arg, &tv, ശൂന്യ, &evalarg);
    എങ്കിൽ (r == FAIL)
	retval = ശൂന്യ;
    അല്ലെങ്കിൽ
    {
	retval = typval2string(&tv, join_list);
	clear_tv(&tv);
    }
    clear_evalarg(&evalarg, ശൂന്യ);

    തിരികെ retval;
}

    char_u *
eval_to_string(
    char_u	*arg,
    പൂർണ്ണസംഖ്യ		join_list,
    പൂർണ്ണസംഖ്യ		use_simple_function)
{
    തിരികെ eval_to_string_eap(arg, join_list, ശൂന്യ, use_simple_function);
}

/*
 * Call eval_to_string() without using current local variables and using
 * textlock.  When "use_sandbox" is TRUE use the sandbox.
 * Use legacy Vim script syntax.
 */
    char_u *
eval_to_string_safe(
    char_u	*arg,
    പൂർണ്ണസംഖ്യ		use_sandbox,
    പൂർണ്ണസംഖ്യ		keep_script_version,
    പൂർണ്ണസംഖ്യ		use_simple_function)
{
    char_u	*retval;
    funccal_entry_T funccal_entry;
    പൂർണ്ണസംഖ്യ		save_sc_version = current_sctx.sc_version;
    പൂർണ്ണസംഖ്യ		save_garbage = may_garbage_collect;

    എങ്കിൽ (!keep_script_version)
	current_sctx.sc_version = 1;
    save_funccal(&funccal_entry);
    എങ്കിൽ (use_sandbox)
	++sandbox;
    ++textlock;
    may_garbage_collect = FALSE;
    retval = eval_to_string(arg, FALSE, use_simple_function);
    എങ്കിൽ (use_sandbox)
	--sandbox;
    --textlock;
    may_garbage_collect = save_garbage;
    restore_funccal();
    current_sctx.sc_version = save_sc_version;
    തിരികെ retval;
}

/*
 * Top level evaluation function, returning a number.
 * Evaluates "expr" silently.
 * Returns -1 for an error.
 */
    varnumber_T
eval_to_സംഖ്യ(char_u *വ്യഞ്ജകം, പൂർണ്ണസംഖ്യ use_simple_function)
{
    typval_T	rettv;
    varnumber_T	retval;
    char_u	*p = skipwhite(വ്യഞ്ജകം);
    പൂർണ്ണസംഖ്യ		r = NOTDONE;

    ++emsg_off;

    എങ്കിൽ (use_simple_function)
	r = may_call_simple_func(വ്യഞ്ജകം, &rettv);
    എങ്കിൽ (r == NOTDONE)
	r = eval1(&p, &rettv, &EVALARG_EVALUATE);
    എങ്കിൽ (r == FAIL)
	retval = -1;
    അല്ലെങ്കിൽ
    {
	retval = tv_get_സംഖ്യ_chk(&rettv, ശൂന്യ);
	clear_tv(&rettv);
    }
    --emsg_off;

    തിരികെ retval;
}

/*
 * Top level evaluation function.
 * Returns an allocated typval_T with the result.
 * Returns NULL when there is an error.
 */
    typval_T *
eval_വ്യഞ്ജകം(char_u *arg, exarg_T *eap)
{
    തിരികെ eval_വ്യഞ്ജകം_ext(arg, eap, FALSE);
}

    typval_T *
eval_വ്യഞ്ജകം_ext(char_u *arg, exarg_T *eap, പൂർണ്ണസംഖ്യ use_simple_function)
{
    typval_T	*tv;
    evalarg_T	evalarg;

    fill_evalarg_from_eap(&evalarg, eap, eap != ശൂന്യ && eap->skip);

    tv = ALLOC_ONE(typval_T);
    എങ്കിൽ (tv != ശൂന്യ)
    {
	പൂർണ്ണസംഖ്യ	r = NOTDONE;

	എങ്കിൽ (use_simple_function)
	    r = eval0_simple_funccal(arg, tv, eap, &evalarg);
	എങ്കിൽ (r == NOTDONE)
	    r = eval0(arg, tv, eap, &evalarg);

	എങ്കിൽ (r == FAIL)
	    VIM_CLEAR(tv);
    }

    clear_evalarg(&evalarg, eap);
    തിരികെ tv;
}

/*
 * "*arg" points to what can be a function name in the form of "import.Name" or
 * "Funcref".  Return the name of the function.  Set "tofree" to something that
 * was allocated.
 * If "verbose" is FALSE no errors are given.
 * Return NULL for any failure.
 */
    സ്ഥിതം char_u *
deref_function_name(
	    char_u	**arg,
	    char_u	**tofree,
	    evalarg_T	*evalarg,
	    പൂർണ്ണസംഖ്യ		verbose)
{
    typval_T	ref;
    char_u	*name = *arg;
    പൂർണ്ണസംഖ്യ		save_flags = 0;
    പൂർണ്ണസംഖ്യ		evaluate = evalarg != ശൂന്യ
				      && (evalarg->eval_flags & EVAL_EVALUATE);

    ref.v_type = VAR_UNKNOWN;
    എങ്കിൽ (evalarg != ശൂന്യ)
    {
	// need to evaluate this to get an import, like in "a.Func"
	save_flags = evalarg->eval_flags;
	evalarg->eval_flags |= EVAL_EVALUATE;
    }
    എങ്കിൽ (eval9(arg, &ref, evalarg, FALSE) == FAIL)
    {
	dictitem_T	*v;

	// If <SID>VarName was used it would not be found, try another way.
	v = find_var_also_in_script(name, ശൂന്യ, FALSE);
	എങ്കിൽ (v == ശൂന്യ)
	{
	    name = ശൂന്യ;
	    പോകുക theend;
	}
	copy_tv(&v->di_tv, &ref);
    }
    എങ്കിൽ (*skipwhite(*arg) != NUL)
    {
	എങ്കിൽ (verbose)
	    semsg(_(e_trailing_characters_str), *arg);
	name = ശൂന്യ;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (ref.v_type == VAR_FUNC && ref.vval.v_string != ശൂന്യ)
    {
	name = ref.vval.v_string;
	ref.vval.v_string = ശൂന്യ;
	*tofree = name;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (ref.v_type == VAR_PARTIAL && ref.vval.v_partial != ശൂന്യ)
    {
	എങ്കിൽ (ref.vval.v_partial->pt_argc > 0
		|| ref.vval.v_partial->pt_dict != ശൂന്യ)
	{
	    എങ്കിൽ (verbose)
		emsg(_(e_cannot_use_partial_here));
	    name = ശൂന്യ;
	}
	അല്ലെങ്കിൽ
	{
	    name = vim_strsave(partial_name(ref.vval.v_partial));
	    *tofree = name;
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (evaluate)
    {
	എങ്കിൽ (verbose)
	    semsg(_(e_not_callable_type_str), name);
	name = ശൂന്യ;
    }

theend:
    clear_tv(&ref);
    എങ്കിൽ (evalarg != ശൂന്യ)
	evalarg->eval_flags = save_flags;
    തിരികെ name;
}

/*
 * Call some Vim script function and return the result in "*rettv".
 * Uses argv[0] to argv[argc - 1] for the function arguments.  argv[argc]
 * should have type VAR_UNKNOWN.
 * Returns OK or FAIL.
 */
    പൂർണ്ണസംഖ്യ
call_vim_function(
    char_u      *func,
    പൂർണ്ണസംഖ്യ		argc,
    typval_T	*argv,
    typval_T	*rettv)
{
    പൂർണ്ണസംഖ്യ		ret;
    funcexe_T	funcexe;
    char_u	*arg;
    char_u	*name;
    char_u	*tofree = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		ignore_errors;

    rettv->v_type = VAR_UNKNOWN;		// clear_tv() uses this
    CLEAR_FIELD(funcexe);
    funcexe.fe_firstline = curwin->w_cursor.lnum;
    funcexe.fe_lastline = curwin->w_cursor.lnum;
    funcexe.fe_evaluate = TRUE;

    // The name might be "import.Func" or "Funcref".  We don't know, we need to
    // ignore errors for an undefined name.  But we do want errors when an
    // autoload script has errors.  Guess that when there is a dot in the name
    // showing errors is the right choice.
    ignore_errors = vim_strchr(func, '.') == ശൂന്യ;
    arg = func;
    എങ്കിൽ (ignore_errors)
	++emsg_off;
    name = deref_function_name(&arg, &tofree, &EVALARG_EVALUATE, FALSE);
    എങ്കിൽ (ignore_errors)
	--emsg_off;
    എങ്കിൽ (name == ശൂന്യ)
	name = func;

    ret = call_func(name, -1, rettv, argc, argv, &funcexe);

    എങ്കിൽ (ret == FAIL)
	clear_tv(rettv);
    vim_free(tofree);

    തിരികെ ret;
}

/*
 * Call Vim script function "func" and return the result as a string.
 * Uses "argv[0]" to "argv[argc - 1]" for the function arguments. "argv[argc]"
 * should have type VAR_UNKNOWN.
 * Returns NULL when calling the function fails.
 */
    ശൂന്യം *
call_func_retstr(
    char_u      *func,
    പൂർണ്ണസംഖ്യ		argc,
    typval_T	*argv)
{
    typval_T	rettv;
    char_u	*retval;

    എങ്കിൽ (call_vim_function(func, argc, argv, &rettv) == FAIL)
	തിരികെ ശൂന്യ;

    retval = vim_strsave(tv_get_string(&rettv));
    clear_tv(&rettv);
    തിരികെ retval;
}

/*
 * Call Vim script function "func" and return the result as a List.
 * Uses "argv" and "argc" as call_func_retstr().
 * Returns NULL when there is something wrong.
 * Gives an error when the returned value is not a list.
 */
    ശൂന്യം *
call_func_retlist(
    char_u      *func,
    പൂർണ്ണസംഖ്യ		argc,
    typval_T	*argv)
{
    typval_T	rettv;

    എങ്കിൽ (call_vim_function(func, argc, argv, &rettv) == FAIL)
	തിരികെ ശൂന്യ;

    എങ്കിൽ (rettv.v_type != VAR_LIST)
    {
	semsg(_(e_custom_list_completion_function_does_not_return_list_but_str),
		vartype_name(rettv.v_type));
	clear_tv(&rettv);
	തിരികെ ശൂന്യ;
    }

    തിരികെ rettv.vval.v_list;
}

#എങ്കിൽ defined(FEAT_FOLDING)
/*
 * Evaluate "arg", which is 'foldexpr'.
 * Note: caller must set "curwin" to match "arg".
 * Returns the foldlevel, and any character preceding it in "*cp".  Doesn't
 * give error messages.
 */
    പൂർണ്ണസംഖ്യ
eval_foldexpr(win_T *wp, പൂർണ്ണസംഖ്യ *cp)
{
    char_u	*arg;
    typval_T	tv;
    varnumber_T	retval;
    char_u	*s;
    sctx_T	saved_sctx = current_sctx;
    പൂർണ്ണസംഖ്യ		use_sandbox = was_set_insecurely(wp, (char_u *)"foldexpr",
								    OPT_LOCAL);

    arg = skipwhite(wp->w_p_fde);
    current_sctx = wp->w_p_script_ctx[WV_FDE];

    ++emsg_off;
    എങ്കിൽ (use_sandbox)
	++sandbox;
    ++textlock;
    *cp = NUL;

    // Evaluate the expression.  If the expression is "FuncName()" call the
    // function directly.
    എങ്കിൽ (eval0_simple_funccal(arg, &tv, ശൂന്യ, &EVALARG_EVALUATE) == FAIL)
	retval = 0;
    അല്ലെങ്കിൽ
    {
	// If the result is a number, just return the number.
	എങ്കിൽ (tv.v_type == VAR_സംഖ്യ)
	    retval = tv.vval.v_സംഖ്യ;
	അല്ലെങ്കിൽ എങ്കിൽ (tv.v_type != VAR_STRING || tv.vval.v_string == ശൂന്യ)
	    retval = 0;
	അല്ലെങ്കിൽ
	{
	    // If the result is a string, check if there is a non-digit before
	    // the number.
	    s = tv.vval.v_string;
	    എങ്കിൽ (*s != NUL && !VIM_ISDIGIT(*s) && *s != '-')
		*cp = *s++;
	    retval = atol((അക്ഷരം *)s);
	}
	clear_tv(&tv);
    }
    --emsg_off;
    എങ്കിൽ (use_sandbox)
	--sandbox;
    --textlock;
    clear_evalarg(&EVALARG_EVALUATE, ശൂന്യ);
    current_sctx = saved_sctx;

    തിരികെ (പൂർണ്ണസംഖ്യ)retval;
}
#endif

#ifdef LOG_LOCKVAR
തരനിർവചനം ഘടന flag_string_S
{
    പൂർണ്ണസംഖ്യ	    flag;
    അക്ഷരം    *str;
} flag_string_T;

    സ്ഥിതം അക്ഷരം *
flags_tostring(പൂർണ്ണസംഖ്യ flags, flag_string_T *_fstring, അക്ഷരം *buf, size_t n)
{
    അക്ഷരം *p = buf;
    *p = NUL;
    വേണ്ടി (flag_string_T *fstring = _fstring; fstring->flag; ++fstring)
    {
	എങ്കിൽ ((fstring->flag & flags) != 0)
	{
	    size_t len = STRLEN(fstring->str);
	    എങ്കിൽ (n > p - buf + len + 7)
	    {
		STRCAT(p, fstring->str);
		p += len;
		STRCAT(p, " ");
		++p;
	    }
	    അല്ലെങ്കിൽ
	    {
		STRCAT(buf, "...");
		നിർത്തുക;
	    }
	}
    }
    തിരികെ buf;
}

flag_string_T glv_flag_strings[] = {
    { GLV_QUIET,		"QUIET" },
    { GLV_NO_AUTOLOAD,		"NO_AUTOLOAD" },
    { GLV_READ_ONLY,		"READ_ONLY" },
    { GLV_NO_DECL,		"NO_DECL" },
    { GLV_COMPILING,		"COMPILING" },
    { GLV_ASSIGN_WITH_OP,	"ASSIGN_WITH_OP" },
    { GLV_PREFER_FUNC,		"PREFER_FUNC" },
    { 0,			ശൂന്യ }
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
    സ്ഥിതം ശൂന്യം
fill_lval_from_lval_വേര്(lval_T *lp, lval_വേര്_T *lr)
{
#ifdef LOG_LOCKVAR
    ch_log(ശൂന്യ, "LKVAR: fill_lval_from_lval_root(): name %s, tv %p",
						lp->ll_name, (ശൂന്യം*)lr->lr_tv);
#endif
    എങ്കിൽ (lr->lr_tv == ശൂന്യ)
	തിരികെ;
    എങ്കിൽ (!lr->lr_is_arg && lr->lr_tv->v_type == VAR_CLASS)
    {
	എങ്കിൽ (lr->lr_tv->vval.v_class != ശൂന്യ)
	{
	    // Special special case. Look for a bare class variable reference.
	    class_T	*cl = lr->lr_tv->vval.v_class;
	    പൂർണ്ണസംഖ്യ		m_idx;
	    ocmember_T	*m = class_member_lookup(cl, lp->ll_name,
					lp->ll_name_end - lp->ll_name, &m_idx);
	    എങ്കിൽ (m != ശൂന്യ)
	    {
		// Assuming "inside class" since bare reference.
		lp->ll_class = lr->lr_tv->vval.v_class;
		lp->ll_oi = m_idx;
		lp->ll_valtype = m->ocm_type;
		lp->ll_tv = &lp->ll_class->class_members_tv[m_idx];
#ifdef LOG_LOCKVAR
		ch_log(ശൂന്യ, "LKVAR:    ... class member %s.%s",
					lp->ll_class->class_name.string, lp->ll_name);
#endif
		തിരികെ;
	    }
	}
    }

#ifdef LOG_LOCKVAR
    ch_log(ശൂന്യ, "LKVAR:    ... type: %s", vartype_name(lr->lr_tv->v_type));
#endif
    lp->ll_tv = lr->lr_tv;
    lp->ll_is_വേര് = TRUE;
}

/*
 * Check if the class has permission to access the member.
 * Returns OK or FAIL.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_lval_check_access(
    class_T	*cl_exec,   // executing class, NULL if :def or script level
    class_T	*cl,	    // class which contains the member
    ocmember_T	*om,	    // member being accessed
    char_u	*p,	    // char after member name
    പൂർണ്ണസംഖ്യ		flags)	    // GLV flags to check if writing to lval
{
#ifdef LOG_LOCKVAR
    ch_log(ശൂന്യ, "LKVAR: get_lval_check_access(), cl_exec %p, cl %p, %c",
						(ശൂന്യം*)cl_exec, (ശൂന്യം*)cl, *p);
#endif
    എങ്കിൽ (cl_exec != ശൂന്യ && cl_exec == cl)
	തിരികെ OK;

    അക്ഷരം *സന്ദേശം = ശൂന്യ;
    തിരഞ്ഞെടുപ്പ് (om->ocm_access)
    {
	അവസ്ഥ VIM_ACCESS_PRIVATE:
	    സന്ദേശം = e_cannot_access_protected_variable_str;
	    നിർത്തുക;
	അവസ്ഥ VIM_ACCESS_READ:
	    // If [idx] or .key following, read only OK.
	    എങ്കിൽ (*p == '[' || *p == '.')
		നിർത്തുക;
	    എങ്കിൽ ((flags & GLV_READ_ONLY) == 0)
	    {
		എങ്കിൽ (IS_ENUM(cl))
		{
		    എങ്കിൽ (om->ocm_type->tt_type == VAR_OBJECT)
			semsg(_(e_enumvalue_str_cannot_be_modified),
				cl->class_name.string, om->ocm_name.string);
		    അല്ലെങ്കിൽ
			സന്ദേശം = e_variable_is_not_writable_str;
		}
		അല്ലെങ്കിൽ
		    സന്ദേശം = e_variable_is_not_writable_str;
	    }
	    നിർത്തുക;
	അവസ്ഥ VIM_ACCESS_ALL:
	    നിർത്തുക;
    }
    എങ്കിൽ (സന്ദേശം != ശൂന്യ)
    {
	emsg_var_cl_define(സന്ദേശം, om->ocm_name.string, 0, cl);
	തിരികെ FAIL;
    }
    തിരികെ OK;
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
    സ്ഥിതം char_u *
get_lval_imported(
    lval_T	*lp,
    scid_T	imp_sid,
    char_u	*p,
    dictitem_T	**dip,
    പൂർണ്ണസംഖ്യ		fne_flags)
{
    ufunc_T	*ufunc;
    type_T	*type = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		cc;
    പൂർണ്ണസംഖ്യ		rc = FAIL;

    p = skipwhite(p);

    import_check_sourced_sid(&imp_sid);
    lp->ll_sid = imp_sid;
    lp->ll_name = p;
    p = find_name_end(lp->ll_name, ശൂന്യ, ശൂന്യ, fne_flags);
    lp->ll_name_end = p;

    // check the item is exported
    cc = *p;
    *p = NUL;
    എങ്കിൽ (find_exported(imp_sid, lp->ll_name, &ufunc, &type, ശൂന്യ, ശൂന്യ,
								TRUE) == -1)
	പോകുക failed;

    // Get the typval for the exported item
    hashtab_T *ht = &SCRIPT_VARS(imp_sid);
    എങ്കിൽ (ht == ശൂന്യ)
	പോകുക failed;

    dictitem_T *di = find_var_in_ht(ht, 0, lp->ll_name, TRUE);
    എങ്കിൽ (di == ശൂന്യ)
	// script is autoloaded.  So variable will be found later
	പോകുക success;

    *dip = di;

    // Check whether the variable is writable.
    svar_T *sv = find_typval_in_script(&di->di_tv, imp_sid, FALSE);
    എങ്കിൽ (sv != ശൂന്യ && sv->sv_const != 0)
    {
	semsg(_(e_cannot_change_readonly_variable_str), lp->ll_name);
	പോകുക failed;
    }

    // check whether variable is locked
    എങ്കിൽ (മൂല്യം_check_lock(di->di_tv.v_lock, lp->ll_name, FALSE))
	പോകുക failed;

    lp->ll_tv = &di->di_tv;
    lp->ll_valtype = type;

success:
    rc = OK;

failed:
    *p = cc;
    തിരികെ rc == OK ? p : ശൂന്യ;
}

തരനിർവചനം എണ്ണൽ {
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_lval_dict_item(
    lval_T	*lp,
    char_u	*name,
    char_u	*താക്കോൽ,
    പൂർണ്ണസംഖ്യ		len,
    char_u	**താക്കോൽ_end,
    typval_T	*var1,
    പൂർണ്ണസംഖ്യ		flags,
    പൂർണ്ണസംഖ്യ		unlet,
    typval_T	*rettv)
{
    പൂർണ്ണസംഖ്യ		quiet = flags & GLV_QUIET;
    char_u	*p = *താക്കോൽ_end;

    എങ്കിൽ (len == -1)
    {
	// "[key]": get key from "var1"
	താക്കോൽ = tv_get_string_chk(var1);	// is number or string
	എങ്കിൽ (താക്കോൽ == ശൂന്യ)
	    തിരികെ GLV_FAIL;
    }
    lp->ll_list = ശൂന്യ;
    lp->ll_list = ശൂന്യ;
    lp->ll_blob = ശൂന്യ;
    lp->ll_object = ശൂന്യ;
    lp->ll_class = ശൂന്യ;
    lp->ll_tuple = ശൂന്യ;

    // a NULL dict is equivalent with an empty dict
    എങ്കിൽ (lp->ll_tv->vval.v_dict == ശൂന്യ)
    {
	lp->ll_tv->vval.v_dict = dict_alloc();
	എങ്കിൽ (lp->ll_tv->vval.v_dict == ശൂന്യ)
	    തിരികെ GLV_FAIL;
	++lp->ll_tv->vval.v_dict->dv_refcount;
    }
    lp->ll_dict = lp->ll_tv->vval.v_dict;

    lp->ll_di = dict_find(lp->ll_dict, താക്കോൽ, len);

    // When assigning to a scope dictionary check that a function and
    // variable name is valid (only variable name unless it is l: or
    // g: dictionary). Disallow overwriting a builtin function.
    എങ്കിൽ (rettv != ശൂന്യ && lp->ll_dict->dv_scope != 0)
    {
	പൂർണ്ണസംഖ്യ prevval;

	എങ്കിൽ (len != -1)
	{
	    prevval = താക്കോൽ[len];
	    താക്കോൽ[len] = NUL;
	}
	അല്ലെങ്കിൽ
	    prevval = 0; // avoid compiler warning
	പൂർണ്ണസംഖ്യ wrong = (lp->ll_dict->dv_scope == VAR_DEF_SCOPE
		&& (rettv->v_type == VAR_FUNC
		    || rettv->v_type == VAR_PARTIAL)
		&& var_wrong_func_name(താക്കോൽ, lp->ll_di == ശൂന്യ))
	    || !valid_varname(താക്കോൽ, -1, TRUE);
	എങ്കിൽ (len != -1)
	    താക്കോൽ[len] = prevval;
	എങ്കിൽ (wrong)
	    തിരികെ GLV_FAIL;
    }

    എങ്കിൽ (lp->ll_valtype != ശൂന്യ)
	// use the type of the member
	lp->ll_valtype = lp->ll_valtype->tt_member;

    എങ്കിൽ (lp->ll_di == ശൂന്യ)
    {
	// Can't add "v:" or "a:" variable.
	എങ്കിൽ (lp->ll_dict == get_vimvar_dict()
		|| &lp->ll_dict->dv_hashtab == get_funccal_args_ht())
	{
	    semsg(_(e_illegal_variable_name_str), name);
	    തിരികെ GLV_FAIL;
	}

	// Key does not exist in dict: may need to add it.
	എങ്കിൽ (*p == '[' || *p == '.' || unlet)
	{
	    എങ്കിൽ (!quiet)
		semsg(_(e_താക്കോൽ_not_present_in_dictionary_str), താക്കോൽ);
	    തിരികെ GLV_FAIL;
	}
	എങ്കിൽ (len == -1)
	    lp->ll_newkey = vim_strsave(താക്കോൽ);
	അല്ലെങ്കിൽ
	    lp->ll_newkey = vim_strnsave(താക്കോൽ, len);
	എങ്കിൽ (lp->ll_newkey == ശൂന്യ)
	    p = ശൂന്യ;

	*താക്കോൽ_end = p;
	തിരികെ GLV_STOP;
    }
    // existing variable, need to check if it can be changed
    അല്ലെങ്കിൽ എങ്കിൽ ((flags & GLV_READ_ONLY) == 0
	    && (var_check_ro(lp->ll_di->di_flags, name, FALSE)
		|| var_check_lock(lp->ll_di->di_flags, name, FALSE)))
	തിരികെ GLV_FAIL;

    lp->ll_tv = &lp->ll_di->di_tv;

    തിരികെ GLV_OK;
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_lval_blob(
    lval_T	*lp,
    typval_T	*var1,
    typval_T	*var2,
    പൂർണ്ണസംഖ്യ		empty1,
    പൂർണ്ണസംഖ്യ		quiet)
{
    long	bloblen = blob_len(lp->ll_tv->vval.v_blob);

    lp->ll_list = ശൂന്യ;
    lp->ll_dict = ശൂന്യ;
    lp->ll_object = ശൂന്യ;
    lp->ll_class = ശൂന്യ;
    lp->ll_tuple = ശൂന്യ;

    // Get the number and item for the only or first index of a List or Tuple.
    എങ്കിൽ (empty1)
	lp->ll_n1 = 0;
    അല്ലെങ്കിൽ
	// is number or string
	lp->ll_n1 = (long)tv_get_സംഖ്യ(var1);

    എങ്കിൽ (check_blob_index(bloblen, lp->ll_n1, quiet) == FAIL)
	തിരികെ FAIL;
    എങ്കിൽ (lp->ll_range && !lp->ll_empty2)
    {
	lp->ll_n2 = (long)tv_get_സംഖ്യ(var2);
	എങ്കിൽ (check_blob_range(bloblen, lp->ll_n1, lp->ll_n2, quiet) == FAIL)
	    തിരികെ FAIL;
    }

    എങ്കിൽ (!lp->ll_range)
	// Indexing a single byte in a blob.  So the rhs type is a
	// number.
	lp->ll_valtype = &t_സംഖ്യ;

    lp->ll_blob = lp->ll_tv->vval.v_blob;
    lp->ll_tv = ശൂന്യ;

    തിരികെ OK;
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_lval_list(
    lval_T	*lp,
    typval_T	*var1,
    typval_T	*var2,
    പൂർണ്ണസംഖ്യ		empty1,
    പൂർണ്ണസംഖ്യ		flags,
    പൂർണ്ണസംഖ്യ		quiet)
{
    /*
     * Get the number and item for the only or first index of the List.
     */
    എങ്കിൽ (empty1)
	lp->ll_n1 = 0;
    അല്ലെങ്കിൽ
	// is number or string
	lp->ll_n1 = (long)tv_get_സംഖ്യ(var1);

    lp->ll_dict = ശൂന്യ;
    lp->ll_object = ശൂന്യ;
    lp->ll_class = ശൂന്യ;
    lp->ll_tuple = ശൂന്യ;
    lp->ll_list = lp->ll_tv->vval.v_list;
    lp->ll_li = check_range_index_one(lp->ll_list, &lp->ll_n1,
				(flags & GLV_ASSIGN_WITH_OP) == 0, quiet);
    എങ്കിൽ (lp->ll_li == ശൂന്യ)
	തിരികെ FAIL;

    എങ്കിൽ (lp->ll_valtype != ശൂന്യ && !lp->ll_range)
    {
	// use the type of the member
	എങ്കിൽ (lp->ll_valtype->tt_member != ശൂന്യ)
	    lp->ll_valtype = lp->ll_valtype->tt_member;
	അല്ലെങ്കിൽ
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
    എങ്കിൽ (lp->ll_range && !lp->ll_empty2)
    {
	lp->ll_n2 = (long)tv_get_സംഖ്യ(var2);
	// is number or string
	എങ്കിൽ (check_range_index_two(lp->ll_list,
			&lp->ll_n1, lp->ll_li, &lp->ll_n2, quiet) == FAIL)
	    തിരികെ FAIL;
    }

    lp->ll_tv = &lp->ll_li->li_tv;

    തിരികെ OK;
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_lval_tuple(
    lval_T	*lp,
    typval_T	*idx,
    പൂർണ്ണസംഖ്യ		quiet)
{
    // is number or string
    lp->ll_n1 = (long)tv_get_സംഖ്യ(idx);

    lp->ll_list = ശൂന്യ;
    lp->ll_dict = ശൂന്യ;
    lp->ll_blob = ശൂന്യ;
    lp->ll_object = ശൂന്യ;
    lp->ll_class = ശൂന്യ;

    lp->ll_tuple = lp->ll_tv->vval.v_tuple;
    lp->ll_tv = tuple_find(lp->ll_tuple, lp->ll_n1);
    എങ്കിൽ (lp->ll_tv == ശൂന്യ)
    {
	എങ്കിൽ (!quiet)
	    semsg(_(e_tuple_index_out_of_range_nr), lp->ll_n1);
	തിരികെ GLV_FAIL;
    }

    // use the type of the member
    എങ്കിൽ (lp->ll_valtype != ശൂന്യ)
    {
	എങ്കിൽ (lp->ll_valtype != ശൂന്യ
		&& lp->ll_valtype->tt_type == VAR_TUPLE
		&& lp->ll_valtype->tt_argcount == 1)
	{
	    // a variadic tuple or a single item tuple
	    എങ്കിൽ (lp->ll_valtype->tt_flags & TTFLAG_VARARGS)
		lp->ll_valtype = lp->ll_valtype->tt_args[0]->tt_member;
	    അല്ലെങ്കിൽ
		lp->ll_valtype = lp->ll_valtype->tt_args[0];
	}
	അല്ലെങ്കിൽ
	    // If the LHS member type is not known (VAR_ANY), then get it from
	    // the tuple item (after indexing)
	    lp->ll_valtype = typval2type(lp->ll_tv, get_copyID(),
					&lp->ll_type_list, TVTT_DO_MEMBER);
    }

    തിരികെ GLV_OK;
}

/*
 * Get a class or object lval method in class "cl".  The 'key' argument points
 * to the method name and 'key_end' points to the character after 'key'.
 * 'v_type' is VAR_CLASS or VAR_OBJECT.
 *
 * The method index, method function pointer and method type are returned in
 * "lp".
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_lval_oc_method(
    lval_T	*lp,
    class_T	*cl,
    char_u	*താക്കോൽ,
    char_u	**താക്കോൽ_end,
    vartype_T	v_type)
{
    // Look for a method with this name.
    // round 1: class functions (skipped for an object)
    // round 2: object methods
    വേണ്ടി (പൂർണ്ണസംഖ്യ round = v_type == VAR_OBJECT ? 2 : 1; round <= 2; ++round)
    {
	പൂർണ്ണസംഖ്യ	m_idx;
	ufunc_T	*fp;

	fp = method_lookup(cl, round == 1 ? VAR_CLASS : VAR_OBJECT,
						താക്കോൽ, *താക്കോൽ_end - താക്കോൽ, &m_idx);
	lp->ll_oi = m_idx;

	// process generic method (if present)
	എങ്കിൽ (fp && (fp = eval_generic_func(fp, താക്കോൽ, താക്കോൽ_end)) == ശൂന്യ)
	    തിരികെ FAIL;

	എങ്കിൽ (fp != ശൂന്യ)
	{
	    lp->ll_ufunc = fp;
	    lp->ll_valtype = fp->uf_func_type;
	    നിർത്തുക;
	}
    }

    തിരികെ OK;
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_lval_oc_variable(
    lval_T	*lp,
    class_T	*cl,
    char_u	*താക്കോൽ,
    char_u	*താക്കോൽ_end,
    vartype_T	v_type,
    class_T	*cl_exec,
    പൂർണ്ണസംഖ്യ		flags)
{
    പൂർണ്ണസംഖ്യ		m_idx;
    ocmember_T	*om;

    om = member_lookup(cl, v_type, താക്കോൽ, താക്കോൽ_end - താക്കോൽ, &m_idx);
    lp->ll_oi = m_idx;
    എങ്കിൽ (om == ശൂന്യ)
	തിരികെ OK;

    // Check variable is accessible
    എങ്കിൽ (get_lval_check_access(cl_exec, cl, om, താക്കോൽ_end, flags) == FAIL)
	തിരികെ FAIL;

    // When lhs is used to modify the variable, check it is not a read-only
    // variable.
    എങ്കിൽ ((flags & GLV_READ_ONLY) == 0 && (*താക്കോൽ_end != '.' && *താക്കോൽ_end != '[')
	    && oc_var_check_ro(cl, om))
	തിരികെ FAIL;

    lp->ll_valtype = om->ocm_type;

    എങ്കിൽ (v_type == VAR_OBJECT)
	lp->ll_tv = ((typval_T *)(lp->ll_tv->vval.v_object + 1)) + m_idx;
    അല്ലെങ്കിൽ
	lp->ll_tv = &cl->class_members_tv[m_idx];

    തിരികെ OK;
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_lval_class_or_obj(
    lval_T	*lp,
    char_u	*താക്കോൽ,
    char_u	**താക്കോൽ_end,
    vartype_T	v_type,
    class_T	*cl_exec,
    പൂർണ്ണസംഖ്യ		flags,
    പൂർണ്ണസംഖ്യ		quiet)
{
    lp->ll_dict = ശൂന്യ;
    lp->ll_list = ശൂന്യ;
    lp->ll_tuple = ശൂന്യ;

    class_T *cl;
    എങ്കിൽ (v_type == VAR_OBJECT)
    {
	എങ്കിൽ (lp->ll_tv->vval.v_object == ശൂന്യ)
	{
	    എങ്കിൽ (!quiet)
		emsg(_(e_using_null_object));
	    തിരികെ FAIL;
	}
	cl = lp->ll_tv->vval.v_object->obj_class;
	lp->ll_object = lp->ll_tv->vval.v_object;
    }
    അല്ലെങ്കിൽ
    {
	cl = lp->ll_tv->vval.v_class;
	lp->ll_object = ശൂന്യ;
    }
    lp->ll_class = cl;

    എങ്കിൽ (cl == ശൂന്യ)
	// TODO: what if class is NULL?
	തിരികെ OK;

    lp->ll_valtype = ശൂന്യ;

    എങ്കിൽ (flags & GLV_PREFER_FUNC)
	എങ്കിൽ (get_lval_oc_method(lp, cl, താക്കോൽ, താക്കോൽ_end, v_type) == FAIL)
	    തിരികെ FAIL;

    // Look for object/class member variable
    എങ്കിൽ (lp->ll_valtype == ശൂന്യ)
    {
	എങ്കിൽ (get_lval_oc_variable(lp, cl, താക്കോൽ, *താക്കോൽ_end, v_type, cl_exec, flags)
								== FAIL)
	    തിരികെ FAIL;
    }

    എങ്കിൽ (lp->ll_valtype == ശൂന്യ)
    {
	member_not_found_സന്ദേശം(cl, v_type, താക്കോൽ, *താക്കോൽ_end - താക്കോൽ);
	തിരികെ FAIL;
    }

    തിരികെ OK;
}

/*
 * Check whether dot (".") is allowed after the variable "name" with type
 * "v_type".  Only Dict, Class and Object types support a dot after the name.
 * Returns TRUE if dot is allowed after the name.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
dot_allowed_after_type(char_u *name, vartype_T v_type, പൂർണ്ണസംഖ്യ quiet)
{
    എങ്കിൽ (v_type != VAR_DICT && v_type != VAR_OBJECT && v_type != VAR_CLASS)
    {
	എങ്കിൽ (!quiet)
	    semsg(_(e_dot_not_allowed_after_str_str),
		    vartype_name(v_type), name);
	തിരികെ FALSE;
    }

    തിരികെ TRUE;
}

/*
 * Check whether left bracket ("[") is allowed after the variable "name" with
 * type "v_type".  Only Dict, List, Tuple and Blob types support a bracket
 * after the variable name.  Returns TRUE if bracket is allowed after the name.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
bracket_allowed_after_type(char_u *name, vartype_T v_type, പൂർണ്ണസംഖ്യ quiet)
{
    എങ്കിൽ (v_type == VAR_CLASS || v_type == VAR_OBJECT)
    {
	എങ്കിൽ (!quiet)
	    semsg(_(e_index_not_allowed_after_str_str),
		    vartype_name(v_type), name);
	തിരികെ FALSE;
    }

    തിരികെ TRUE;
}

/*
 * Check whether the variable "name" with type "v_type" can be followed by an
 * index.  Only Dict, List, Tuple, Blob, Object and Class types support
 * indexing.  Returns TRUE if indexing is allowed after the name.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
index_allowed_after_type(char_u *name, vartype_T v_type, പൂർണ്ണസംഖ്യ quiet)
{
    എങ്കിൽ (v_type != VAR_LIST
	    && v_type != VAR_TUPLE
	    && v_type != VAR_DICT
	    && v_type != VAR_BLOB
	    && v_type != VAR_OBJECT
	    && v_type != VAR_CLASS)
    {
	എങ്കിൽ (!quiet)
	    semsg(_(e_index_not_allowed_after_str_str),
		    vartype_name(v_type), name);
	തിരികെ FALSE;
    }

    തിരികെ TRUE;
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
    സ്ഥിതം char_u *
get_lval_subscript(
    lval_T	*lp,
    char_u	*p,
    char_u	*name,
    typval_T	*rettv,
    hashtab_T	*ht,
    dictitem_T	*v,
    പൂർണ്ണസംഖ്യ		unlet,
    പൂർണ്ണസംഖ്യ		flags,	    // GLV_ values
    class_T	*cl_exec)
{
    പൂർണ്ണസംഖ്യ		quiet = flags & GLV_QUIET;
    char_u	*താക്കോൽ = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		len;
    typval_T	var1;
    typval_T	var2;
    പൂർണ്ണസംഖ്യ		empty1 = FALSE;
    പൂർണ്ണസംഖ്യ		rc = FAIL;

    /*
     * Loop until no more [idx] or .key is following.
     */
    var1.v_type = VAR_UNKNOWN;
    var2.v_type = VAR_UNKNOWN;

    ആകുമ്പോൾ (*p == '[' || (*p == '.' && p[1] != '=' && p[1] != '.'))
    {
	vartype_T v_type = lp->ll_tv->v_type;

	എങ്കിൽ (*p == '.' && !dot_allowed_after_type(name, v_type, quiet))
	    പോകുക done;

	എങ്കിൽ (*p == '[' && !bracket_allowed_after_type(name, v_type, quiet))
	    പോകുക done;

	എങ്കിൽ (!index_allowed_after_type(name, v_type, quiet))
	    പോകുക done;

	// A NULL list/blob works like an empty list/blob, allocate one now.
	പൂർണ്ണസംഖ്യ r = OK;
	എങ്കിൽ (v_type == VAR_LIST && lp->ll_tv->vval.v_list == ശൂന്യ)
	    r = rettv_list_alloc(lp->ll_tv);
	അല്ലെങ്കിൽ എങ്കിൽ (v_type == VAR_BLOB && lp->ll_tv->vval.v_blob == ശൂന്യ)
	    r = rettv_blob_alloc(lp->ll_tv);
	എങ്കിൽ (r == FAIL)
	    പോകുക done;

	എങ്കിൽ (lp->ll_range)
	{
	    എങ്കിൽ (!quiet)
		emsg(_(e_slice_must_come_last));
	    പോകുക done;
	}
#ifdef LOG_LOCKVAR
	ch_log(ശൂന്യ, "LKVAR: get_lval() loop: p: %s, type: %s", p,
		vartype_name(v_type));
#endif

	എങ്കിൽ (current_script_is_vim9() && lp->ll_valtype == ശൂന്യ
		&& v != ശൂന്യ
		&& lp->ll_tv == &v->di_tv
		&& ht != ശൂന്യ && ht == get_script_local_ht())
	{
	    svar_T  *sv = find_typval_in_script(lp->ll_tv, 0, TRUE);

	    // Vim9 script local variable: get the type
	    എങ്കിൽ (sv != ശൂന്യ)
	    {
		lp->ll_valtype = sv->sv_type;
#ifdef LOG_LOCKVAR
		ch_log(ശൂന്യ, "LKVAR:    ... loop: vim9 assign type: %s",
			vartype_name(lp->ll_valtype->tt_type));
#endif
	    }
	}

	len = -1;
	എങ്കിൽ (*p == '.')
	{
	    താക്കോൽ = p + 1;

	    വേണ്ടി (len = 0; ASCII_ISALNUM(താക്കോൽ[len]) || താക്കോൽ[len] == '_'; ++len)
		;
	    എങ്കിൽ (len == 0)
	    {
		എങ്കിൽ (!quiet)
		    emsg(_(e_cannot_use_empty_താക്കോൽ_for_dictionary));
		പോകുക done;
	    }
	    p = താക്കോൽ + len;
	}
	അല്ലെങ്കിൽ
	{
	    // Get the index [expr] or the first index [expr: ].
	    p = skipwhite(p + 1);
	    എങ്കിൽ (*p == ':')
		empty1 = TRUE;
	    അല്ലെങ്കിൽ
	    {
		empty1 = FALSE;
		എങ്കിൽ (eval1(&p, &var1, &EVALARG_EVALUATE) == FAIL)  // recursive!
		    പോകുക done;
		എങ്കിൽ (tv_get_string_chk(&var1) == ശൂന്യ)
		    // not a number or string
		    പോകുക done;
		p = skipwhite(p);
	    }

	    // Optionally get the second index [ :expr].
	    എങ്കിൽ (*p == ':')
	    {
		എങ്കിൽ (v_type == VAR_DICT)
		{
		    എങ്കിൽ (!quiet)
			emsg(_(e_cannot_slice_dictionary));
		    പോകുക done;
		}
		എങ്കിൽ (v_type == VAR_TUPLE)
		{
		    എങ്കിൽ (!quiet)
			emsg(_(e_cannot_slice_tuple));
		    പോകുക done;
		}
		എങ്കിൽ (rettv != ശൂന്യ
			&& !(rettv->v_type == VAR_LIST
			    && rettv->vval.v_list != ശൂന്യ)
			&& !(rettv->v_type == VAR_BLOB
			    && rettv->vval.v_blob != ശൂന്യ))
		{
		    എങ്കിൽ (!quiet)
			emsg(_(e_slice_requires_list_or_blob_മൂല്യം));
		    പോകുക done;
		}
		p = skipwhite(p + 1);
		എങ്കിൽ (*p == ']')
		    lp->ll_empty2 = TRUE;
		അല്ലെങ്കിൽ
		{
		    lp->ll_empty2 = FALSE;
		    // recursive!
		    എങ്കിൽ (eval1(&p, &var2, &EVALARG_EVALUATE) == FAIL)
			പോകുക done;
		    എങ്കിൽ (tv_get_string_chk(&var2) == ശൂന്യ)
			// not a number or string
			പോകുക done;
		}
		lp->ll_range = TRUE;
	    }
	    അല്ലെങ്കിൽ
		lp->ll_range = FALSE;

	    എങ്കിൽ (*p != ']')
	    {
		എങ്കിൽ (!quiet)
		    emsg(_(e_missing_closing_square_brace));
		പോകുക done;
	    }

	    // Skip to past ']'.
	    ++p;
	}
#ifdef LOG_LOCKVAR
	എങ്കിൽ (len == -1)
	    ch_log(ശൂന്യ, "LKVAR:    ... loop: p: %s, '[' key: %s", p,
		    empty1 ? ":" : (അക്ഷരം*)tv_get_string(&var1));
	അല്ലെങ്കിൽ
	    ch_log(ശൂന്യ, "LKVAR:    ... loop: p: %s, '.' key: %s", p, താക്കോൽ);
#endif

	എങ്കിൽ (v_type == VAR_DICT)
	{
	    glv_status_T glv_status;

	    glv_status = get_lval_dict_item(lp, name, താക്കോൽ, len, &p, &var1,
							flags, unlet, rettv);
	    എങ്കിൽ (glv_status == GLV_FAIL)
		പോകുക done;
	    എങ്കിൽ (glv_status == GLV_STOP)
		നിർത്തുക;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (v_type == VAR_BLOB)
	{
	    എങ്കിൽ (get_lval_blob(lp, &var1, &var2, empty1, quiet) == FAIL)
		പോകുക done;

	    നിർത്തുക;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (v_type == VAR_LIST)
	{
	    എങ്കിൽ (get_lval_list(lp, &var1, &var2, empty1, flags, quiet) == FAIL)
		പോകുക done;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (v_type == VAR_TUPLE)
	{
	    എങ്കിൽ (get_lval_tuple(lp, &var1, quiet) == FAIL)
		പോകുക done;
	}
	അല്ലെങ്കിൽ  // v_type == VAR_CLASS || v_type == VAR_OBJECT
	{
	    എങ്കിൽ (get_lval_class_or_obj(lp, താക്കോൽ, &p, v_type, cl_exec, flags,
							quiet) == FAIL)
		പോകുക done;
	}

	clear_tv(&var1);
	clear_tv(&var2);
	var1.v_type = VAR_UNKNOWN;
	var2.v_type = VAR_UNKNOWN;
    }

    എങ്കിൽ (lp->ll_tuple != ശൂന്യ && (flags & GLV_READ_ONLY) == 0)
    {
	എങ്കിൽ (!quiet)
	    emsg(_(e_tuple_is_immutable));
	പോകുക done;
    }

    rc = OK;

done:
    clear_tv(&var1);
    clear_tv(&var2);
    തിരികെ rc == OK ? p : ശൂന്യ;
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
    പൂർണ്ണസംഖ്യ		unlet,
    പൂർണ്ണസംഖ്യ		skip,
    പൂർണ്ണസംഖ്യ		flags,	    // GLV_ values
    പൂർണ്ണസംഖ്യ		fne_flags)  // flags for find_name_end()
{
    char_u	*p;
    char_u	*വ്യഞ്ജകം_start, *വ്യഞ്ജകം_end;
    പൂർണ്ണസംഖ്യ		cc;
    dictitem_T	*v = ശൂന്യ;
    hashtab_T	*ht = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		quiet = flags & GLV_QUIET;
    പൂർണ്ണസംഖ്യ		writing = 0;
    പൂർണ്ണസംഖ്യ		vim9script = in_vim9script();
    class_T	*cl_exec = ശൂന്യ;    // class that is executing, or NULL.

#ifdef LOG_LOCKVAR
    എങ്കിൽ (lval_വേര് == ശൂന്യ)
	ch_log(ശൂന്യ, "LKVAR: get_lval(): name: %s, lval_root (nil)", name);
    അല്ലെങ്കിൽ
	ch_log(ശൂന്യ, "LKVAR: get_lval(): name: %s, lr_tv %p lr_is_arg %d",
			name, (ശൂന്യം*)lval_വേര്->lr_tv, lval_വേര്->lr_is_arg);
    അക്ഷരം buf[80];
    ch_log(ശൂന്യ, "LKVAR:    ...: GLV flags: %s",
		    flags_tostring(flags, glv_flag_strings, buf, വലുപ്പം(buf)));
#endif

    // Clear everything in "lp".
    CLEAR_POINTER(lp);
    ga_init2(&lp->ll_type_list, വലുപ്പം(type_T *), 10);

    എങ്കിൽ (skip || (flags & GLV_COMPILING))
    {
	// When skipping or compiling just find the end of the name.
	lp->ll_name = name;
	lp->ll_name_end = find_name_end(name, ശൂന്യ, ശൂന്യ,
						      FNE_INCL_BR | fne_flags);
	തിരികെ lp->ll_name_end;
    }

    // Cannot use "s:var" at the Vim9 script level.  "s: type" is OK.
    എങ്കിൽ (vim9script && at_script_level()
		  && name[0] == 's' && name[1] == ':' && !VIM_ISWHITE(name[2]))
    {
	semsg(_(e_cannot_use_s_colon_in_vim9_script_str), name);
	തിരികെ ശൂന്യ;
    }

    // Find the end of the name.
    p = find_name_end(name, &വ്യഞ്ജകം_start, &വ്യഞ്ജകം_end, fne_flags);
    lp->ll_name_end = p;
    എങ്കിൽ (വ്യഞ്ജകം_start != ശൂന്യ)
    {
	// Don't expand the name when we already know there is an error.
	എങ്കിൽ (unlet && !VIM_ISWHITE(*p) && !ends_excmd(*p)
						    && *p != '[' && *p != '.')
	{
	    semsg(_(e_trailing_characters_str), p);
	    തിരികെ ശൂന്യ;
	}

	lp->ll_exp_name = make_expanded_name(name, വ്യഞ്ജകം_start, വ്യഞ്ജകം_end, p);
	എങ്കിൽ (lp->ll_exp_name == ശൂന്യ)
	{
	    // Report an invalid expression in braces, unless the
	    // expression evaluation has been cancelled due to an
	    // aborting error, an interrupt, or an exception.
	    എങ്കിൽ (!aborting() && !quiet)
	    {
		emsg_severe = TRUE;
		semsg(_(e_invalid_argument_str), name);
		തിരികെ ശൂന്യ;
	    }
	}
	lp->ll_name = lp->ll_exp_name;
    }
    അല്ലെങ്കിൽ
    {
	lp->ll_name = name;

	എങ്കിൽ (vim9script)
	{
	    // "a: type" is declaring variable "a" with a type, not "a:".
	    // However, "g:[key]" is indexing a dictionary.
	    എങ്കിൽ (p == name + 2 && p[-1] == ':' && *p != '[')
	    {
		--p;
		lp->ll_name_end = p;
	    }
	    എങ്കിൽ (*skipwhite(p) == ':')
	    {
		char_u	    *tp = skipwhite(p + 1);

		എങ്കിൽ (is_scoped_variable(name))
		{
		    semsg(_(e_cannot_use_type_with_this_variable_str), name);
		    തിരികെ ശൂന്യ;
		}
		എങ്കിൽ (VIM_ISWHITE(*p))
		{
		    semsg(_(e_no_white_space_allowed_before_colon_str), p);
		    തിരികെ ശൂന്യ;
		}
		എങ്കിൽ (tp == p + 1 && !quiet)
		{
		    semsg(_(e_white_space_required_after_str_str), ":", p);
		    തിരികെ ശൂന്യ;
		}
		എങ്കിൽ (!SCRIPT_ID_VALID(current_sctx.sc_sid))
		{
		    semsg(_(e_using_type_not_in_script_context_str), p);
		    തിരികെ ശൂന്യ;
		}
		എങ്കിൽ (vim9script && (flags & GLV_NO_DECL) &&
			!(flags & GLV_FOR_LOOP))
		{
		    // Using a type and not in a "var" declaration.
		    semsg(_(e_trailing_characters_str), p);
		    തിരികെ ശൂന്യ;
		}


		// parse the type after the name
		lp->ll_type = parse_type(&tp,
			       &SCRIPT_ITEM(current_sctx.sc_sid)->sn_type_list,
			       ശൂന്യ, ശൂന്യ, !quiet);
		എങ്കിൽ (!quiet && (lp->ll_type == ശൂന്യ
			    || !valid_declaration_type(lp->ll_type)))
		    തിരികെ ശൂന്യ;
		lp->ll_name_end = tp;
	    }
	    // TODO: check inside class?
	}
    }
    എങ്കിൽ (lp->ll_name == ശൂന്യ)
	തിരികെ p;

    എങ്കിൽ (*p == '.')
    {
	// In legacy script, when a local variable and import exists with this name,
	// prioritize local variable over imports to avoid conflicts.
	പൂർണ്ണസംഖ്യ var_exists = FALSE;
	എങ്കിൽ (!vim9script)
	{
	    cc = *p;
	    *p = NUL;
	    hashtab_T *local_ht = get_funccal_local_ht();
	    എങ്കിൽ (local_ht != ശൂന്യ)
	    {
		hashitem_T *hi = hash_find(local_ht, lp->ll_name);
		എങ്കിൽ (!HASHITEM_EMPTY(hi))
		    var_exists = TRUE;
	    }
	    *p = cc;
	}

	എങ്കിൽ (!var_exists)
	{
	    imported_T *import = find_imported(lp->ll_name, p - lp->ll_name, TRUE);
	    എങ്കിൽ (import != ശൂന്യ)
	    {
		p++;	// skip '.'
		p = get_lval_imported(lp, import->imp_sid, p, &v, fne_flags);
		എങ്കിൽ (p == ശൂന്യ)
		    തിരികെ ശൂന്യ;
	    }
	}
    }

    // Without [idx] or .key we are done.
    എങ്കിൽ (*p != '[' && *p != '.')
    {
	എങ്കിൽ (lval_വേര് != ശൂന്യ)
	    fill_lval_from_lval_വേര്(lp, lval_വേര്);
	തിരികെ p;
    }

    എങ്കിൽ (vim9script && lval_വേര് != ശൂന്യ)
	cl_exec = lval_വേര്->lr_cl_exec;
    എങ്കിൽ (vim9script && lval_വേര് != ശൂന്യ && lval_വേര്->lr_tv != ശൂന്യ)
    {
	// using local variable
	lp->ll_tv = lval_വേര്->lr_tv;
	v = ശൂന്യ;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (lp->ll_tv == ശൂന്യ)
    {
	cc = *p;
	*p = NUL;
	// When we would write to the variable pass &ht and prevent autoload.
	writing = !(flags & GLV_READ_ONLY);
	v = find_var(lp->ll_name, writing ? &ht : ശൂന്യ,
					 (flags & GLV_NO_AUTOLOAD) || writing);
	എങ്കിൽ (v == ശൂന്യ && !quiet)
	    semsg(_(e_undefined_variable_str), lp->ll_name);
	*p = cc;
	എങ്കിൽ (v == ശൂന്യ)
	    തിരികെ ശൂന്യ;
	lp->ll_tv = &v->di_tv;
    }

    എങ്കിൽ (vim9script && (flags & GLV_NO_DECL) == 0)
    {
	എങ്കിൽ (!quiet)
	    semsg(_(e_variable_already_declared_str), lp->ll_name);
	തിരികെ ശൂന്യ;
    }

    // If the next character is a "." or a "[", then process the subitem.
    p = get_lval_subscript(lp, p, name, rettv, ht, v, unlet, flags, cl_exec);
    എങ്കിൽ (p == ശൂന്യ)
	തിരികെ ശൂന്യ;

    എങ്കിൽ (vim9script && lp->ll_valtype != ശൂന്യ && rettv != ശൂന്യ)
    {
	where_T	    where = WHERE_INIT;

	// In a Vim9 script, do type check and make sure the variable is
	// writable.
	എങ്കിൽ (check_typval_type(lp->ll_valtype, rettv, where) == FAIL)
	    തിരികെ ശൂന്യ;
    }

    lp->ll_name_end = p;
    തിരികെ p;
}

/*
 * Clear lval "lp" that was filled by get_lval().
 */
    ശൂന്യം
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
    ശൂന്യം
set_var_lval(
    lval_T	*lp,
    char_u	*endp,
    typval_T	*rettv,
    പൂർണ്ണസംഖ്യ		copy,
    പൂർണ്ണസംഖ്യ		flags,	    // ASSIGN_CONST, ASSIGN_NO_DECL
    char_u	*op,
    പൂർണ്ണസംഖ്യ		var_idx)    // index for "let [a, b] = list"
{
    പൂർണ്ണസംഖ്യ		cc;
    dictitem_T	*di;

    എങ്കിൽ (lp->ll_tv == ശൂന്യ)
    {
	cc = *endp;
	*endp = NUL;
	എങ്കിൽ (in_vim9script() && check_reserved_name(lp->ll_name, FALSE) == FAIL)
	    തിരികെ;

	എങ്കിൽ (lp->ll_blob != ശൂന്യ)
	{
	    പൂർണ്ണസംഖ്യ	    error = FALSE;

	    എങ്കിൽ (op != ശൂന്യ && *op != '=')
	    {
		semsg(_(e_wrong_variable_type_for_str_equal), op);
		തിരികെ;
	    }
	    എങ്കിൽ (മൂല്യം_check_lock(lp->ll_blob->bv_lock, lp->ll_name, FALSE))
		തിരികെ;

	    എങ്കിൽ (lp->ll_range && rettv->v_type == VAR_BLOB)
	    {
		എങ്കിൽ (lp->ll_empty2)
		    lp->ll_n2 = blob_len(lp->ll_blob) - 1;

		എങ്കിൽ (blob_set_range(lp->ll_blob, lp->ll_n1, lp->ll_n2,
								rettv) == FAIL)
		    തിരികെ;
	    }
	    അല്ലെങ്കിൽ
	    {
		varnumber_T	val = tv_get_സംഖ്യ_chk(rettv, &error);
		എങ്കിൽ (!error)
		{
		    എങ്കിൽ (val < 0 || val > 255)
			semsg(_(e_invalid_മൂല്യം_for_blob_nr), val);
		    അല്ലെങ്കിൽ
			blob_set_append(lp->ll_blob, lp->ll_n1, val);
		}
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (op != ശൂന്യ && *op != '=')
	{
	    typval_T tv;

	    എങ്കിൽ ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					     && (flags & ASSIGN_FOR_LOOP) == 0)
	    {
		emsg(_(e_cannot_modify_existing_variable));
		*endp = cc;
		തിരികെ;
	    }

	    // handle +=, -=, *=, /=, %= and .=
	    di = ശൂന്യ;
	    എങ്കിൽ (eval_variable(lp->ll_name, (പൂർണ്ണസംഖ്യ)STRLEN(lp->ll_name),
				 lp->ll_sid, &tv, &di, EVAL_VAR_VERBOSE) == OK)
	    {
		എങ്കിൽ (di != ശൂന്യ && check_typval_is_മൂല്യം(&di->di_tv) == FAIL)
		{
		    clear_tv(&tv);
		    തിരികെ;
		}

		// In Vim9 script a String v: variable only takes a String.
		എങ്കിൽ (in_vim9script() && STRNCMP(lp->ll_name, "v:", 2) == 0
			&& tv.v_type == VAR_STRING)
		{
		    where_T	where = WHERE_INIT;

		    എങ്കിൽ (check_typval_type(&t_string, rettv, where) == FAIL)
		    {
			clear_tv(&tv);
			*endp = cc;
			തിരികെ;
		    }
		}

		എങ്കിൽ ((di == ശൂന്യ
			 || (!var_check_ro(di->di_flags, lp->ll_name, FALSE)
			   && !tv_check_lock(&di->di_tv, lp->ll_name, FALSE)))
			&& tv_op(&tv, rettv, op) == OK)
		    set_var_const(lp->ll_name, lp->ll_sid, ശൂന്യ, &tv, FALSE,
				ASSIGN_NO_DECL | ASSIGN_COMPOUND_OP, 0);
		clear_tv(&tv);
	    }
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (lp->ll_type != ശൂന്യ && check_typval_arg_type(lp->ll_type, rettv,
							      ശൂന്യ, 0) == FAIL)
		തിരികെ;
	    set_var_const(lp->ll_name, lp->ll_sid, lp->ll_type, rettv, copy,
							       flags, var_idx);
	}
	*endp = cc;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (മൂല്യം_check_lock(lp->ll_newkey == ശൂന്യ
		? lp->ll_tv->v_lock
		: lp->ll_tv->vval.v_dict->dv_lock, lp->ll_name, FALSE))
	;
    അല്ലെങ്കിൽ എങ്കിൽ (lp->ll_range)
    {
	എങ്കിൽ ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					     && (flags & ASSIGN_FOR_LOOP) == 0)
	{
	    emsg(_(e_cannot_lock_range));
	    തിരികെ;
	}

	(ശൂന്യം)list_assign_range(lp->ll_list, rettv->vval.v_list,
			 lp->ll_n1, lp->ll_n2, lp->ll_empty2, op, lp->ll_name);
    }
    അല്ലെങ്കിൽ
    {
	/*
	 * Assign to a List, Dictionary or Object item.
	 */
	എങ്കിൽ ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					     && (flags & ASSIGN_FOR_LOOP) == 0)
	{
	    emsg(_(e_cannot_lock_list_or_dict));
	    തിരികെ;
	}

	എങ്കിൽ (lp->ll_valtype != ശൂന്യ
		    && check_typval_arg_type(lp->ll_valtype, rettv,
							      ശൂന്യ, 0) == FAIL)
	{
	    lp->ll_name_end = ശൂന്യ;
	    തിരികെ;
	}

	// If the lval is a List and the type of the list is not yet set,
	// then set the item type from the declared type of the variable.
	എങ്കിൽ (in_vim9script() && rettv->v_type == VAR_LIST
				&& rettv->vval.v_list != ശൂന്യ
				&& rettv->vval.v_list->lv_type == ശൂന്യ)
	{
	    എങ്കിൽ (lp->ll_tv->v_type == VAR_LIST
		    && lp->ll_tv->vval.v_list != ശൂന്യ
		    && lp->ll_tv->vval.v_list->lv_type != ശൂന്യ)
		set_tv_type(rettv, lp->ll_tv->vval.v_list->lv_type);
	}

	എങ്കിൽ (lp->ll_newkey != ശൂന്യ)
	{
	    എങ്കിൽ (op != ശൂന്യ && *op != '=')
	    {
		semsg(_(e_താക്കോൽ_not_present_in_dictionary_str), lp->ll_newkey);
		തിരികെ;
	    }
	    എങ്കിൽ (dict_wrong_func_name(lp->ll_tv->vval.v_dict, rettv,
								lp->ll_newkey))
		തിരികെ;

	    // Need to add an item to the Dictionary.
	    di = dictitem_alloc(lp->ll_newkey);
	    എങ്കിൽ (di == ശൂന്യ)
		തിരികെ;
	    എങ്കിൽ (dict_add(lp->ll_tv->vval.v_dict, di) == FAIL)
	    {
		vim_free(di);
		തിരികെ;
	    }
	    lp->ll_tv = &di->di_tv;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (op != ശൂന്യ && *op != '=')
	{
	    tv_op(lp->ll_tv, rettv, op);
	    തിരികെ;
	}
	അല്ലെങ്കിൽ
	    clear_tv(lp->ll_tv);

	/*
	 * Assign the value to the variable or list item.
	 */
	എങ്കിൽ (copy)
	    copy_tv(rettv, lp->ll_tv);
	അല്ലെങ്കിൽ
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
tv_op_blob(typval_T *tv1, typval_T *tv2, char_u *op)
{
    എങ്കിൽ (*op != '+' || tv2->v_type != VAR_BLOB)
	തിരികെ FAIL;

    // Blob += Blob
    എങ്കിൽ (tv2->vval.v_blob == ശൂന്യ)
	തിരികെ OK;

    എങ്കിൽ (tv1->vval.v_blob == ശൂന്യ)
    {
	tv1->vval.v_blob = tv2->vval.v_blob;
	++tv1->vval.v_blob->bv_refcount;
	തിരികെ OK;
    }

    blob_T	*b1 = tv1->vval.v_blob;
    blob_T	*b2 = tv2->vval.v_blob;
    പൂർണ്ണസംഖ്യ		len = blob_len(b2);

    എങ്കിൽ (len > 0 && ga_grow(&b1->bv_ga, len) == OK)
    {
	mch_memmove((char_u *)b1->bv_ga.ga_data + b1->bv_ga.ga_len,
		    (char_u *)b2->bv_ga.ga_data, (size_t)len);
	b1->bv_ga.ga_len += len;
    }
    അല്ലെങ്കിൽ
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < len; i++)
	    (ശൂന്യം)ga_append(&b1->bv_ga, blob_get(b2, i));

    തിരികെ OK;
}

/*
 * Handle "list1 += list2".
 * Returns OK or FAIL.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
tv_op_list(typval_T *tv1, typval_T *tv2, char_u *op)
{
    എങ്കിൽ (*op != '+' || tv2->v_type != VAR_LIST)
	തിരികെ FAIL;

    // List += List
    എങ്കിൽ (tv2->vval.v_list == ശൂന്യ)
	തിരികെ OK;

    എങ്കിൽ (tv1->vval.v_list == ശൂന്യ)
    {
	tv1->vval.v_list = tv2->vval.v_list;
	++tv1->vval.v_list->lv_refcount;
    }
    അല്ലെങ്കിൽ
	list_extend(tv1->vval.v_list, tv2->vval.v_list, ശൂന്യ);

    തിരികെ OK;
}

/*
 * Handle number operations:
 *	nr += nr , nr -= nr , nr *=nr , nr /= nr , nr %= nr
 *
 * Returns OK or FAIL.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
tv_op_സംഖ്യ(typval_T *tv1, typval_T *tv2, char_u *op)
{
    varnumber_T	n;
    പൂർണ്ണസംഖ്യ		failed = FALSE;

    n = tv_get_സംഖ്യ(tv1);
    എങ്കിൽ (tv2->v_type == VAR_FLOAT)
    {
	float_T f = n;

	എങ്കിൽ (*op == '%')
	    തിരികെ FAIL;
	തിരഞ്ഞെടുപ്പ് (*op)
	{
	    അവസ്ഥ '+': f += tv2->vval.v_float; നിർത്തുക;
	    അവസ്ഥ '-': f -= tv2->vval.v_float; നിർത്തുക;
	    അവസ്ഥ '*': f *= tv2->vval.v_float; നിർത്തുക;
	    അവസ്ഥ '/': f /= tv2->vval.v_float; നിർത്തുക;
	}
	clear_tv(tv1);
	tv1->v_type = VAR_FLOAT;
	tv1->vval.v_float = f;
    }
    അല്ലെങ്കിൽ
    {
	തിരഞ്ഞെടുപ്പ് (*op)
	{
	    അവസ്ഥ '+': n += tv_get_സംഖ്യ(tv2); നിർത്തുക;
	    അവസ്ഥ '-': n -= tv_get_സംഖ്യ(tv2); നിർത്തുക;
	    അവസ്ഥ '*': n *= tv_get_സംഖ്യ(tv2); നിർത്തുക;
	    അവസ്ഥ '/': n = അക്കം_divide(n, tv_get_സംഖ്യ(tv2), &failed); നിർത്തുക;
	    അവസ്ഥ '%': n = അക്കം_modulus(n, tv_get_സംഖ്യ(tv2), &failed); നിർത്തുക;
	}
	clear_tv(tv1);
	tv1->v_type = VAR_സംഖ്യ;
	tv1->vval.v_സംഖ്യ = n;
    }

    തിരികെ failed ? FAIL : OK;
}

/*
 * Append string "s2" to the string in "tv1".
 * Returns OK if "tv1" was grown in place, FAIL otherwise.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
grow_string_tv(typval_T *tv1, char_u *s2)
{
    എങ്കിൽ (tv1->v_type != VAR_STRING || tv1->vval.v_string == ശൂന്യ)
	തിരികെ FAIL;

    size_t len1 = STRLEN(tv1->vval.v_string);
    size_t len2 = STRLEN(s2);
    char_u *p = vim_realloc(tv1->vval.v_string, len1 + len2 + 1);

    എങ്കിൽ (p == ശൂന്യ)
	തിരികെ FAIL;

    mch_memmove(p + len1, s2, len2 + 1);
    tv1->vval.v_string = p;
    തിരികെ OK;
}

/*
 * Handle "str1 .= str2"
 * Returns OK or FAIL.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
tv_op_string(typval_T *tv1, typval_T *tv2, char_u *op UNUSED)
{
    char_u	numbuf[NUMBUFLEN];
    char_u	*s;
    char_u	*s2;

    എങ്കിൽ (tv2->v_type == VAR_FLOAT)
	തിരികെ FAIL;

    // str .= str
    s2 = tv_get_string_buf(tv2, numbuf);
    എങ്കിൽ (grow_string_tv(tv1, s2) == OK)
	തിരികെ OK;

    s = tv_get_string(tv1);
    s = concat_str(s, s2);
    clear_tv(tv1);
    tv1->v_type = VAR_STRING;
    tv1->vval.v_string = s;

    തിരികെ OK;
}

/*
 * Handle "tv1 += tv2", "tv1 -= tv2", "tv1 *= tv2", "tv1 /= tv2", "tv1 %= tv2"
 * and "tv1 .= tv2"
 * Returns OK or FAIL.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
tv_op_nr_or_string(typval_T *tv1, typval_T *tv2, char_u *op)
{
    എങ്കിൽ (tv2->v_type == VAR_LIST)
	തിരികെ FAIL;

    എങ്കിൽ (vim_strchr((char_u *)"+-*/%", *op) != ശൂന്യ)
	തിരികെ tv_op_സംഖ്യ(tv1, tv2, op);

    തിരികെ tv_op_string(tv1, tv2, op);
}

/*
 * Handle "f1 += f2", "f1 -= f2", "f1 *= f2", "f1 /= f2".
 * Returns OK or FAIL.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
tv_op_float(typval_T *tv1, typval_T *tv2, char_u *op)
{
    float_T f;

    എങ്കിൽ (*op == '%' || *op == '.'
	    || (tv2->v_type != VAR_FLOAT
		&& tv2->v_type != VAR_സംഖ്യ
		&& tv2->v_type != VAR_STRING))
	തിരികെ FAIL;

    എങ്കിൽ (tv2->v_type == VAR_FLOAT)
	f = tv2->vval.v_float;
    അല്ലെങ്കിൽ
	f = tv_get_സംഖ്യ(tv2);
    തിരഞ്ഞെടുപ്പ് (*op)
    {
	അവസ്ഥ '+': tv1->vval.v_float += f; നിർത്തുക;
	അവസ്ഥ '-': tv1->vval.v_float -= f; നിർത്തുക;
	അവസ്ഥ '*': tv1->vval.v_float *= f; നിർത്തുക;
	അവസ്ഥ '/': tv1->vval.v_float /= f; നിർത്തുക;
    }

    തിരികെ OK;
}

/*
 * Handle "tv1 += tv2", "tv1 -= tv2", "tv1 *= tv2", "tv1 /= tv2", "tv1 %= tv2"
 * and "tv1 .= tv2"
 * Returns OK or FAIL.
 */
    പൂർണ്ണസംഖ്യ
tv_op(typval_T *tv1, typval_T *tv2, char_u *op)
{
    // Can't do anything with a Funcref or Dict on the right.
    // v:true and friends only work with "..=".
    എങ്കിൽ (tv2->v_type == VAR_FUNC || tv2->v_type == VAR_DICT
		|| ((tv2->v_type == VAR_BOOL || tv2->v_type == VAR_SPECIAL)
							&& *op != '.'))
    {
	semsg(_(e_wrong_variable_type_for_str_equal), op);
	തിരികെ FAIL;
    }

    പൂർണ്ണസംഖ്യ retval = FAIL;
    തിരഞ്ഞെടുപ്പ് (tv1->v_type)
    {
	അവസ്ഥ VAR_UNKNOWN:
	അവസ്ഥ VAR_ANY:
	അവസ്ഥ VAR_VOID:
	അവസ്ഥ VAR_DICT:
	അവസ്ഥ VAR_FUNC:
	അവസ്ഥ VAR_PARTIAL:
	അവസ്ഥ VAR_BOOL:
	അവസ്ഥ VAR_SPECIAL:
	അവസ്ഥ VAR_JOB:
	അവസ്ഥ VAR_CHANNEL:
	അവസ്ഥ VAR_INSTR:
	അവസ്ഥ VAR_OBJECT:
	അവസ്ഥ VAR_CLASS:
	അവസ്ഥ VAR_TYPEALIAS:
	അവസ്ഥ VAR_TUPLE:
	    നിർത്തുക;

	അവസ്ഥ VAR_BLOB:
	    retval = tv_op_blob(tv1, tv2, op);
	    നിർത്തുക;

	അവസ്ഥ VAR_LIST:
	    retval = tv_op_list(tv1, tv2, op);
	    നിർത്തുക;

	അവസ്ഥ VAR_സംഖ്യ:
	അവസ്ഥ VAR_STRING:
	    retval = tv_op_nr_or_string(tv1, tv2, op);
	    നിർത്തുക;

	അവസ്ഥ VAR_FLOAT:
	    retval = tv_op_float(tv1, tv2, op);
	    നിർത്തുക;
    }

    എങ്കിൽ (retval != OK)
	semsg(_(e_wrong_variable_type_for_str_equal), op);

    തിരികെ retval;
}

/*
 * Evaluate the expression used in a ":for var in expr" command.
 * "arg" points to "var".
 * Set "*errp" to TRUE for an error, FALSE otherwise;
 * Return a pointer that holds the info.  Null when there is an error.
 */
    ശൂന്യം *
eval_for_വരി(
    char_u	*arg,
    പൂർണ്ണസംഖ്യ		*errp,
    exarg_T	*eap,
    evalarg_T	*evalarg)
{
    forinfo_T	*fi;
    char_u	*var_list_end;
    char_u	*വ്യഞ്ജകം;
    typval_T	tv;
    list_T	*l;
    tuple_T	*tuple;
    പൂർണ്ണസംഖ്യ		skip = !(evalarg->eval_flags & EVAL_EVALUATE);

    *errp = TRUE;	// default: there is an error

    fi = ALLOC_CLEAR_ONE(forinfo_T);
    എങ്കിൽ (fi == ശൂന്യ)
	തിരികെ ശൂന്യ;

    var_list_end = skip_var_list(arg, TRUE, &fi->fi_varcount,
						     &fi->fi_semicolon, FALSE);
    എങ്കിൽ (var_list_end == ശൂന്യ)
	തിരികെ fi;

    വ്യഞ്ജകം = skipwhite_and_linebreak(var_list_end, evalarg);
    എങ്കിൽ (വ്യഞ്ജകം[0] != 'i' || വ്യഞ്ജകം[1] != 'n'
				  || !(വ്യഞ്ജകം[2] == NUL || VIM_ISWHITE(വ്യഞ്ജകം[2])))
    {
	എങ്കിൽ (in_vim9script() && *വ്യഞ്ജകം == ':' && വ്യഞ്ജകം != var_list_end)
	    semsg(_(e_no_white_space_allowed_before_colon_str), വ്യഞ്ജകം);
	അല്ലെങ്കിൽ
	    emsg(_(e_missing_in_after_for));
	തിരികെ fi;
    }

    എങ്കിൽ (skip)
	++emsg_skip;
    വ്യഞ്ജകം = skipwhite_and_linebreak(വ്യഞ്ജകം + 2, evalarg);
    എങ്കിൽ (eval0(വ്യഞ്ജകം, &tv, eap, evalarg) == OK)
    {
	*errp = FALSE;
	എങ്കിൽ (!skip)
	{
	    എങ്കിൽ (tv.v_type == VAR_LIST)
	    {
		l = tv.vval.v_list;
		എങ്കിൽ (l == ശൂന്യ)
		{
		    // a null list is like an empty list: do nothing
		    clear_tv(&tv);
		}
		അല്ലെങ്കിൽ
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
	    അല്ലെങ്കിൽ എങ്കിൽ (tv.v_type == VAR_TUPLE)
	    {
		tuple = tv.vval.v_tuple;
		എങ്കിൽ (tuple == ശൂന്യ)
		{
		    // a null tuple is like an empty tuple: do nothing
		    clear_tv(&tv);
		}
		അല്ലെങ്കിൽ
		{
		    // No need to increment the refcount, it's already set for
		    // the tuple being used in "tv".
		    fi->fi_tuple = tuple;
		    fi->fi_tuple_idx = 0;
		}
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (tv.v_type == VAR_BLOB)
	    {
		fi->fi_bi = 0;
		എങ്കിൽ (tv.vval.v_blob != ശൂന്യ)
		{
		    // Make a copy, so that the iteration still works when the
		    // blob is changed.
		    fi->fi_blob = blob_copy(tv.vval.v_blob);
		}
		clear_tv(&tv);
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (tv.v_type == VAR_STRING)
	    {
		fi->fi_byte_idx = 0;
		fi->fi_string = tv.vval.v_string;
		tv.vval.v_string = ശൂന്യ;
		എങ്കിൽ (fi->fi_string == ശൂന്യ)
		    fi->fi_string = vim_strsave((char_u *)"");
	    }
	    അല്ലെങ്കിൽ
	    {
		emsg(_(e_string_list_tuple_or_blob_required));
		clear_tv(&tv);
	    }
	}
    }
    എങ്കിൽ (skip)
	--emsg_skip;
    fi->fi_break_എണ്ണം = evalarg->eval_break_എണ്ണം;

    തിരികെ fi;
}

/*
 * Used when looping over a :for line, skip the "in expr" part.
 */
    ശൂന്യം
skip_for_വരികൾ(ശൂന്യം *fi_void, evalarg_T *evalarg)
{
    forinfo_T	*fi = (forinfo_T *)fi_void;
    പൂർണ്ണസംഖ്യ		i;

    വേണ്ടി (i = 0; i < fi->fi_break_എണ്ണം; ++i)
	eval_next_വരി(ശൂന്യ, evalarg);
}

/*
 * Use the first item in a ":for" list.  Advance to the next.
 * Assign the values to the variable (list).  "arg" points to the first one.
 * Return TRUE when a valid item was found, FALSE when at end of list or
 * something wrong.
 */
    പൂർണ്ണസംഖ്യ
next_for_item(ശൂന്യം *fi_void, char_u *arg)
{
    forinfo_T	*fi = (forinfo_T *)fi_void;
    പൂർണ്ണസംഖ്യ		result;
    പൂർണ്ണസംഖ്യ		flag = ASSIGN_FOR_LOOP | (in_vim9script()
			 ? (ASSIGN_FINAL
			     // first round: error if variable exists
			     | (fi->fi_bi == 0 ? 0 : ASSIGN_DECL)
			     | ASSIGN_NO_MEMBER_TYPE
			     | ASSIGN_UPDATE_BLOCK_ID)
			 : 0);
    listitem_T	*item;
    പൂർണ്ണസംഖ്യ		skip_assign = in_vim9script() && arg[0] == '_'
						      && !eval_isnamec(arg[1]);

    എങ്കിൽ (fi->fi_blob != ശൂന്യ)
    {
	typval_T	tv;

	എങ്കിൽ (fi->fi_bi >= blob_len(fi->fi_blob))
	    തിരികെ FALSE;
	tv.v_type = VAR_സംഖ്യ;
	tv.v_lock = VAR_FIXED;
	tv.vval.v_സംഖ്യ = blob_get(fi->fi_blob, fi->fi_bi);
	++fi->fi_bi;
	എങ്കിൽ (skip_assign)
	    തിരികെ TRUE;
	തിരികെ ex_let_vars(arg, &tv, TRUE, fi->fi_semicolon,
					    fi->fi_varcount, flag, ശൂന്യ) == OK;
    }

    എങ്കിൽ (fi->fi_string != ശൂന്യ)
    {
	typval_T	tv;
	പൂർണ്ണസംഖ്യ		len;

	len = mb_ptr2len(fi->fi_string + fi->fi_byte_idx);
	എങ്കിൽ (len == 0)
	    തിരികെ FALSE;
	tv.v_type = VAR_STRING;
	tv.v_lock = VAR_FIXED;
	tv.vval.v_string = vim_strnsave(fi->fi_string + fi->fi_byte_idx, len);
	fi->fi_byte_idx += len;
	++fi->fi_bi;
	എങ്കിൽ (skip_assign)
	    result = TRUE;
	അല്ലെങ്കിൽ
	    result = ex_let_vars(arg, &tv, TRUE, fi->fi_semicolon,
					    fi->fi_varcount, flag, ശൂന്യ) == OK;
	vim_free(tv.vval.v_string);
	തിരികെ result;
    }

    എങ്കിൽ (fi->fi_tuple != ശൂന്യ)
    {
	typval_T	tv;

	എങ്കിൽ (fi->fi_tuple_idx >= TUPLE_LEN(fi->fi_tuple))
	    തിരികെ FALSE;

	copy_tv(TUPLE_ITEM(fi->fi_tuple, fi->fi_tuple_idx), &tv);
	++fi->fi_tuple_idx;
	++fi->fi_bi;
	എങ്കിൽ (skip_assign)
	    result = TRUE;
	അല്ലെങ്കിൽ
	    result = ex_let_vars(arg, &tv, TRUE, fi->fi_semicolon,
					    fi->fi_varcount, flag, ശൂന്യ) == OK;
	clear_tv(&tv);
	തിരികെ result;
    }

    item = fi->fi_lw.lw_item;
    എങ്കിൽ (item == ശൂന്യ)
	result = FALSE;
    അല്ലെങ്കിൽ
    {
	fi->fi_lw.lw_item = item->li_next;
	++fi->fi_bi;
	എങ്കിൽ (skip_assign)
	    result = TRUE;
	അല്ലെങ്കിൽ
	    result = (ex_let_vars(arg, &item->li_tv, TRUE, fi->fi_semicolon,
					   fi->fi_varcount, flag, ശൂന്യ) == OK);
    }
    തിരികെ result;
}

/*
 * Free the structure used to store info used by ":for".
 */
    ശൂന്യം
free_for_info(ശൂന്യം *fi_void)
{
    forinfo_T    *fi = (forinfo_T *)fi_void;

    എങ്കിൽ (fi == ശൂന്യ)
	തിരികെ;
    എങ്കിൽ (fi->fi_list != ശൂന്യ)
    {
	list_rem_watch(fi->fi_list, &fi->fi_lw);
	list_unref(fi->fi_list);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (fi->fi_blob != ശൂന്യ)
	blob_unref(fi->fi_blob);
    അല്ലെങ്കിൽ എങ്കിൽ (fi->fi_tuple != ശൂന്യ)
	tuple_unref(fi->fi_tuple);
    അല്ലെങ്കിൽ
	vim_free(fi->fi_string);
    vim_free(fi);
}

    ശൂന്യം
set_context_for_expression(
    expand_T	*xp,
    char_u	*arg,
    cmdidx_T	cmdidx)
{
    പൂർണ്ണസംഖ്യ		has_വ്യഞ്ജകം = cmdidx != CMD_let && cmdidx != CMD_var;
    പൂർണ്ണസംഖ്യ		c;
    char_u	*p;

    എങ്കിൽ (cmdidx == CMD_let || cmdidx == CMD_var
				 || cmdidx == CMD_const || cmdidx == CMD_final)
    {
	xp->xp_context = EXPAND_USER_VARS;
	എങ്കിൽ (vim_strpbrk(arg, (char_u *)"\"'+-*/%.=!?~|&$([<>,#") == ശൂന്യ)
	{
	    // ":let var1 var2 ...": find last space.
	    വേണ്ടി (p = arg + STRLEN(arg); p >= arg; )
	    {
		xp->xp_pattern = p;
		MB_PTR_BACK(arg, p);
		എങ്കിൽ (VIM_ISWHITE(*p))
		    നിർത്തുക;
	    }
	    തിരികെ;
	}
    }
    അല്ലെങ്കിൽ
	xp->xp_context = cmdidx == CMD_call ? EXPAND_FUNCTIONS
							  : EXPAND_EXPRESSION;
    ആകുമ്പോൾ ((xp->xp_pattern = vim_strpbrk(arg,
				  (char_u *)"\"'+-*/%.=!?~|&$([<>,#")) != ശൂന്യ)
    {
	c = *xp->xp_pattern;
	എങ്കിൽ (c == '&')
	{
	    c = xp->xp_pattern[1];
	    എങ്കിൽ (c == '&')
	    {
		++xp->xp_pattern;
		xp->xp_context = has_വ്യഞ്ജകം ? EXPAND_EXPRESSION : EXPAND_NOTHING;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (c != ' ')
	    {
		xp->xp_context = EXPAND_SETTINGS;
		എങ്കിൽ ((c == 'l' || c == 'g') && xp->xp_pattern[2] == ':')
		    xp->xp_pattern += 2;

	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (c == '$')
	{
	    // environment variable
	    xp->xp_context = EXPAND_ENV_VARS;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (c == '=')
	{
	    has_വ്യഞ്ജകം = TRUE;
	    xp->xp_context = EXPAND_EXPRESSION;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (c == '#'
		&& xp->xp_context == EXPAND_EXPRESSION)
	{
	    // Autoload function/variable contains '#'.
	    നിർത്തുക;
	}
	അല്ലെങ്കിൽ എങ്കിൽ ((c == '<' || c == '#')
		&& xp->xp_context == EXPAND_FUNCTIONS
		&& vim_strchr(xp->xp_pattern, '(') == ശൂന്യ)
	{
	    // Function name can start with "<SNR>" and contain '#'.
	    നിർത്തുക;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (has_വ്യഞ്ജകം)
	{
	    എങ്കിൽ (c == '"')	    // string
	    {
		ആകുമ്പോൾ ((c = *++xp->xp_pattern) != NUL && c != '"')
		    എങ്കിൽ (c == '\\' && xp->xp_pattern[1] != NUL)
			++xp->xp_pattern;
		xp->xp_context = EXPAND_NOTHING;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (c == '\'')	    // literal string
	    {
		// Trick: '' is like stopping and starting a literal string.
		ആകുമ്പോൾ ((c = *++xp->xp_pattern) != NUL && c != '\'')
		    /* skip */ ;
		xp->xp_context = EXPAND_NOTHING;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (c == '|')
	    {
		എങ്കിൽ (xp->xp_pattern[1] == '|')
		{
		    ++xp->xp_pattern;
		    xp->xp_context = EXPAND_EXPRESSION;
		}
		അല്ലെങ്കിൽ
		    xp->xp_context = EXPAND_COMMANDS;
	    }
	    അല്ലെങ്കിൽ
		xp->xp_context = EXPAND_EXPRESSION;
	}
	അല്ലെങ്കിൽ
	    // Doesn't look like something valid, expand as an expression
	    // anyway.
	    xp->xp_context = EXPAND_EXPRESSION;
	arg = xp->xp_pattern;
	എങ്കിൽ (*arg != NUL)
	    ആകുമ്പോൾ ((c = *++arg) != NUL && (c == ' ' || c == '\t'))
		/* skip */ ;
    }

    // ":exe one two" completes "two"
    എങ്കിൽ ((cmdidx == CMD_execute
		|| cmdidx == CMD_echo
		|| cmdidx == CMD_echon
		|| cmdidx == CMD_echomsg
		|| cmdidx == CMD_echoerr
		|| cmdidx == CMD_echoconsole
		|| cmdidx == CMD_echowindow)
	    && xp->xp_context == EXPAND_EXPRESSION)
    {
	വേണ്ടി (;;)
	{
	    char_u *n = skiptowhite(arg);

	    എങ്കിൽ (n == arg || IS_WHITE_OR_NUL(*skipwhite(n)))
		നിർത്തുക;
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
സ്ഥിതം regprog_T    *eval_prog_cache = ശൂന്യ;
സ്ഥിതം char_u	    *eval_prog_pat = ശൂന്യ;	// pattern it was compiled for
സ്ഥിതം char_u	    *eval_prog_enc = ശൂന്യ;	// 'encoding' when compiled
സ്ഥിതം long	    eval_prog_re;		// 'regexpengine' when compiled
// State sampled by eval_regcomp() for the eval_regfree() call that gives the
// program back: user code run in between, e.g. an object's string() method
// invoked by match() on a list, can change these options, and then the
// program must not be cached under the changed values.  eval_compile_enc is
// only ever compared against p_enc, never dereferenced.
സ്ഥിതം long	    eval_compile_re;
സ്ഥിതം char_u	    *eval_compile_enc;
സ്ഥിതം പൂർണ്ണസംഖ്യ	    eval_prog_busy = 0;	// programs eval_regcomp() handed out
					// that were not given back yet
സ്ഥിതം പൂർണ്ണസംഖ്യ	    eval_prog_reentered = FALSE;  // eval_regcomp() ran while
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
    സ്ഥിതം bool
eval_prog_volatile(char_u *pat)
{
    char_u *p;

    എങ്കിൽ (STRNCMP(pat, "\\%#=", 4) == 0)
	തിരികെ true;
    വേണ്ടി (p = pat; *p != NUL; ++p)
	എങ്കിൽ (*p == '~'
		|| (p[0] == '[' && (p[1] == ':' || p[1] == '=' || p[1] == '.'))
		|| (p[0] == '%' && (p[1] == '.'
			|| ((p[1] == '<' || p[1] == '>') && p[2] == '.'))))
	    തിരികെ true;
    തിരികെ false;
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
    എങ്കിൽ (eval_prog_busy > 0)
	// The samples above no longer describe the program already handed
	// out: its eval_regfree() must drop it instead of caching it.
	eval_prog_reentered = TRUE;

    എങ്കിൽ (eval_prog_cache != ശൂന്യ
	    && *p_cpo == NUL
	    && eval_prog_cache->re_flags == RE_MAGIC + RE_STRING
	    && eval_prog_re == p_re
	    && STRCMP(eval_prog_pat, pat) == 0
	    && STRCMP(eval_prog_enc, p_enc) == 0)
    {
	prog = eval_prog_cache;
	// The caller now owns the program, eval_regfree() adopts the
	// program the caller ends up with.
	eval_prog_cache = ശൂന്യ;
    }
    അല്ലെങ്കിൽ
	prog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
    എങ്കിൽ (prog != ശൂന്യ)
	++eval_prog_busy;	// paired with the eval_regfree() call
    തിരികെ prog;
}

/*
 * Free program "prog", obtained with eval_regcomp() for pattern "pat", by
 * keeping it in the cache for the next use.  "pat" must be the same string
 * the program was compiled for and must still be valid.
 */
    ശൂന്യം
eval_regfree(char_u *pat, regprog_T *prog)
{
    പൂർണ്ണസംഖ്യ	    reentered = eval_prog_reentered;

    എങ്കിൽ (eval_prog_busy > 0 && --eval_prog_busy == 0)
	eval_prog_reentered = FALSE;
    എങ്കിൽ (prog == ശൂന്യ)
	തിരികെ;
    എങ്കിൽ (reentered || *p_cpo != NUL
	    || eval_compile_re != p_re || eval_compile_enc != p_enc)
    {
	// 'cpoptions' is not empty, an option in the cache key changed, or
	// another program was handed out while the caller was using this
	// one: the samples may not describe this program, do not cache it.
	vim_regfree(prog);
	തിരികെ;
    }
    എങ്കിൽ (eval_prog_pat == ശൂന്യ || STRCMP(eval_prog_pat, pat) != 0
	    || STRCMP(eval_prog_enc, p_enc) != 0)
    {
	char_u	*pat_copy;
	char_u	*enc_copy;

	// A cached pattern is known not to be volatile, thus the scan is
	// only needed when installing a new key.
	എങ്കിൽ (eval_prog_volatile(pat))
	{
	    // compiling depends on state the cache key does not cover
	    vim_regfree(prog);
	    തിരികെ;
	}
	pat_copy = vim_strsave(pat);
	enc_copy = vim_strsave(p_enc);
	എങ്കിൽ (pat_copy == ശൂന്യ || enc_copy == ശൂന്യ)
	{
	    vim_free(pat_copy);
	    vim_free(enc_copy);
	    vim_regfree(prog);
	    തിരികെ;
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

#എങ്കിൽ defined(EXITFREE)
    ശൂന്യം
free_eval_regcomp_cache(ശൂന്യം)
{
    vim_regfree(eval_prog_cache);
    eval_prog_cache = ശൂന്യ;
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
    പൂർണ്ണസംഖ്യ
pattern_match(char_u *pat, char_u *text, പൂർണ്ണസംഖ്യ ic)
{
    പൂർണ്ണസംഖ്യ		matches = FALSE;
    char_u	*save_cpo;
    regmatch_T	regmatch;

    // avoid 'l' flag in 'cpoptions'
    save_cpo = p_cpo;
    p_cpo = empty_option;
    regmatch.regprog = eval_regcomp(pat);
    എങ്കിൽ (regmatch.regprog != ശൂന്യ)
    {
	regmatch.rm_ic = ic;
	matches = vim_regexec_nl(&regmatch, text, (colnr_T)0);
	eval_regfree(pat, regmatch.regprog);
    }
    p_cpo = save_cpo;
    തിരികെ matches;
}

/*
 * Handle a name followed by "(".  Both for just "name(arg)" and for
 * "expr->name(arg)".
 * Returns OK or FAIL.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
eval_func(
	char_u	    **arg,	// points to "(", will be advanced
	evalarg_T   *evalarg,
	char_u	    *name,
	പൂർണ്ണസംഖ്യ	    name_len,
	typval_T    *rettv,
	പൂർണ്ണസംഖ്യ	    flags,
	typval_T    *basetv)	// "expr" for "expr->name(arg)"
{
    പൂർണ്ണസംഖ്യ		evaluate = flags & EVAL_EVALUATE;
    char_u	*s = name;
    പൂർണ്ണസംഖ്യ		len = name_len;
    partial_T	*partial;
    പൂർണ്ണസംഖ്യ		ret = OK;
    type_T	*type = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		found_var = FALSE;

    എങ്കിൽ (!evaluate)
	check_vars(s, len);

    // If "s" is the name of a variable of type VAR_FUNC
    // use its contents.
    s = deref_func_name(s, &len, &partial,
		 in_vim9script() ? &type : ശൂന്യ, !evaluate, FALSE, &found_var);

    // Need to make a copy, in case evaluating the arguments makes
    // the name invalid.
    s = vim_strsave(s);
    എങ്കിൽ (s == ശൂന്യ || (evaluate && *s == NUL))
	ret = FAIL;
    അല്ലെങ്കിൽ
    {
	funcexe_T funcexe;

	// Invoke the function.
	CLEAR_FIELD(funcexe);
	funcexe.fe_firstline = curwin->w_cursor.lnum;
	funcexe.fe_lastline = curwin->w_cursor.lnum;
	funcexe.fe_evaluate = evaluate;
	funcexe.fe_partial = partial;
	എങ്കിൽ (partial != ശൂന്യ)
	{
	    funcexe.fe_object = partial->pt_obj;
	    എങ്കിൽ (funcexe.fe_object != ശൂന്യ)
		++funcexe.fe_object->obj_refcount;
	}
	funcexe.fe_basetv = basetv;
	funcexe.fe_check_type = type;
	funcexe.fe_found_var = found_var;
	എങ്കിൽ (evalarg != ശൂന്യ)
	    funcexe.fe_cctx = evalarg->eval_cctx;
	ret = get_func_tv(s, len, rettv, arg, evalarg, &funcexe);
    }
    vim_free(s);

    // If evaluate is FALSE rettv->v_type was not set in
    // get_func_tv, but it's needed in handle_subscript() to parse
    // what follows. So set it here.
    എങ്കിൽ (rettv->v_type == VAR_UNKNOWN && !evaluate && **arg == '(')
    {
	rettv->vval.v_string = ശൂന്യ;
	rettv->v_type = VAR_FUNC;
    }

    // Stop the expression evaluation when immediately
    // aborting on error, or when an interrupt occurred or
    // an exception was thrown but not caught.
    എങ്കിൽ (evaluate && aborting())
    {
	എങ്കിൽ (ret == OK)
	    clear_tv(rettv);
	ret = FAIL;
    }
    തിരികെ ret;
}

/*
 * After a NL, skip over empty lines and comment-only lines.
 */
    സ്ഥിതം char_u *
newline_skip_comments(char_u *arg)
{
    char_u *p = arg + 1;

    വേണ്ടി (;;)
    {
	p = skipwhite(p);

	എങ്കിൽ (*p == NUL)
	    നിർത്തുക;
	എങ്കിൽ (vim9_comment_start(p))
	{
	    char_u *nl = vim_strchr(p, NL);

	    എങ്കിൽ (nl == ശൂന്യ)
		നിർത്തുക;
	    p = nl;
	}
	എങ്കിൽ (*p != NL)
	    നിർത്തുക;
	++p;  // skip another NL
    }
    തിരികെ p;
}

/*
 * Get the next line source line without advancing.  But do skip over comment
 * lines.
 * Only called for Vim9 script.
 */
    സ്ഥിതം char_u *
getline_peek_skip_comments(evalarg_T *evalarg)
{
    വേണ്ടി (;;)
    {
	char_u *next = getline_peek(evalarg->eval_getline,
							 evalarg->eval_cookie);
	char_u *p;

	എങ്കിൽ (next == ശൂന്യ)
	    നിർത്തുക;
	p = skipwhite(next);
	എങ്കിൽ (*p != NUL && !vim9_comment_start(p))
	    തിരികെ next;
	എങ്കിൽ (eval_next_വരി(ശൂന്യ, evalarg) == ശൂന്യ)
	    നിർത്തുക;
    }
    തിരികെ ശൂന്യ;
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
eval_next_non_blank(char_u *arg, evalarg_T *evalarg, പൂർണ്ണസംഖ്യ *getnext)
{
    char_u *p = skipwhite(arg);

    *getnext = FALSE;
    എങ്കിൽ (in_vim9script()
	    && evalarg != ശൂന്യ
	    && (evalarg->eval_cookie != ശൂന്യ || evalarg->eval_cctx != ശൂന്യ
								   || *p == NL)
	    && (*p == NUL || *p == NL
			     || (vim9_comment_start(p) && VIM_ISWHITE(p[-1]))))
    {
	char_u *next;

	എങ്കിൽ (*p == NL)
	    next = newline_skip_comments(p);
	അല്ലെങ്കിൽ എങ്കിൽ (evalarg->eval_cookie != ശൂന്യ)
	    next = getline_peek_skip_comments(evalarg);
	അല്ലെങ്കിൽ
	    next = peek_next_വരി_from_context(evalarg->eval_cctx);

	എങ്കിൽ (next != ശൂന്യ)
	{
	    *getnext = *p != NL;
	    തിരികെ skipwhite(next);
	}
    }
    തിരികെ p;
}

/*
 * To be called after eval_next_non_blank() sets "getnext" to TRUE.
 * Only called for Vim9 script.
 *
 * If "arg" is not NULL, then the caller should assign the return value to
 * "arg".
 */
    char_u *
eval_next_വരി(char_u *arg, evalarg_T *evalarg)
{
    garray_T	*gap = &evalarg->eval_ga;
    char_u	*വരി;

    എങ്കിൽ (arg != ശൂന്യ)
    {
	എങ്കിൽ (*arg == NL)
	    തിരികെ newline_skip_comments(arg);
	// Truncate before a trailing comment, so that concatenating the lines
	// won't turn the rest into a comment.
	എങ്കിൽ (*skipwhite(arg) == '#')
	    *arg = NUL;
    }

    എങ്കിൽ (evalarg->eval_cookie != ശൂന്യ)
	വരി = evalarg->eval_getline(0, evalarg->eval_cookie, 0,
							   GETLINE_CONCAT_ALL);
    അല്ലെങ്കിൽ
	വരി = next_വരി_from_context(evalarg->eval_cctx, TRUE);
    എങ്കിൽ (വരി == ശൂന്യ)
	തിരികെ ശൂന്യ;

    ++evalarg->eval_break_എണ്ണം;
    എങ്കിൽ (gap->ga_itemsize > 0 && ga_grow(gap, 1) == OK)
    {
	char_u *p = skipwhite(വരി);

	// Going to concatenate the lines after parsing.  For an empty or
	// comment line use an empty string.
	എങ്കിൽ (*p == NUL || vim9_comment_start(p))
	{
	    vim_free(വരി);
	    വരി = vim_strsave((char_u *)"");
	}

	((char_u **)gap->ga_data)[gap->ga_len] = വരി;
	++gap->ga_len;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (evalarg->eval_cookie != ശൂന്യ)
    {
	free_eval_tofree_later(evalarg);
	evalarg->eval_tofree = വരി;
    }

    // Advanced to the next line, "arg" no longer points into the previous
    // line.  The caller assigns the return value to "arg".
    // If "arg" is NULL, then the return value is discarded.  In that case,
    // "arg" still points to the previous line.  So don't reset
    // "eval_using_cmdline".
    എങ്കിൽ (arg != ശൂന്യ)
	evalarg->eval_using_cmdline = FALSE;
    തിരികെ skipwhite(വരി);
}

/*
 * Call eval_next_non_blank() and get the next line if needed.
 */
    char_u *
skipwhite_and_linebreak(char_u *arg, evalarg_T *evalarg)
{
    പൂർണ്ണസംഖ്യ	    getnext;
    char_u  *p = skipwhite_and_nl(arg);

    എങ്കിൽ (evalarg == ശൂന്യ)
	തിരികെ skipwhite(arg);
    eval_next_non_blank(p, evalarg, &getnext);
    എങ്കിൽ (getnext)
	തിരികെ eval_next_വരി(arg, evalarg);
    തിരികെ p;
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
    പൂർണ്ണസംഖ്യ
eval0(
    char_u	*arg,
    typval_T	*rettv,
    exarg_T	*eap,
    evalarg_T	*evalarg)
{
    തിരികെ eval0_retarg(arg, rettv, eap, evalarg, ശൂന്യ);
}

/*
 * If "arg" is a simple function call without arguments then call it and return
 * the result.  Otherwise return NOTDONE.
 */
    പൂർണ്ണസംഖ്യ
may_call_simple_func(
    char_u	*arg,
    typval_T	*rettv)
{
    char_u  *parens = (char_u *)strstr((അക്ഷരം *)arg, "()");
    പൂർണ്ണസംഖ്യ	    r = NOTDONE;

    // If the expression is "FuncName()" then we can skip a lot of overhead.
    എങ്കിൽ (parens != ശൂന്യ && *skipwhite(parens + 2) == NUL)
    {
	char_u *p = STRNCMP(arg, "<SNR>", 5) == 0 ? skipdigits(arg + 5) : arg;

	എങ്കിൽ (to_name_end(p, TRUE) == parens)
	    r = call_simple_func(arg, (size_t)(parens - arg), rettv);
    }
    തിരികെ r;
}

/*
 * Handle zero level expression with optimization for a simple function call.
 * Same arguments and return value as eval0().
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
eval0_simple_funccal(
    char_u	*arg,
    typval_T	*rettv,
    exarg_T	*eap,
    evalarg_T	*evalarg)
{
    പൂർണ്ണസംഖ്യ	    r = may_call_simple_func(arg, rettv);

    എങ്കിൽ (r == NOTDONE)
	r = eval0_retarg(arg, rettv, eap, evalarg, ശൂന്യ);
    തിരികെ r;
}

/*
 * Like eval0() but when "retarg" is not NULL store the pointer to after the
 * expression and don't check what comes after the expression.
 */
    പൂർണ്ണസംഖ്യ
eval0_retarg(
    char_u	*arg,
    typval_T	*rettv,
    exarg_T	*eap,
    evalarg_T	*evalarg,
    char_u	**retarg)
{
    പൂർണ്ണസംഖ്യ		ret;
    char_u	*p;
    char_u	*വ്യഞ്ജകം_end;
    പൂർണ്ണസംഖ്യ		did_emsg_before = did_emsg;
    പൂർണ്ണസംഖ്യ		called_emsg_before = called_emsg;
    പൂർണ്ണസംഖ്യ		check_for_end = retarg == ശൂന്യ;
    പൂർണ്ണസംഖ്യ		end_error = FALSE;

    p = skipwhite(arg);
    ret = eval1(&p, rettv, evalarg);

    എങ്കിൽ (ret != FAIL)
    {
	വ്യഞ്ജകം_end = p;
	p = skipwhite(p);

	// In Vim9 script a command block is not split at NL characters for
	// commands using an expression argument.  Skip over a '#' comment to
	// check for a following NL.  Require white space before the '#'.
	എങ്കിൽ (in_vim9script() && p > വ്യഞ്ജകം_end && retarg == ശൂന്യ)
	    ആകുമ്പോൾ (*p == '#')
	    {
		char_u *nl = vim_strchr(p, NL);

		എങ്കിൽ (nl == ശൂന്യ)
		    നിർത്തുക;
		p = skipwhite(nl + 1);
		എങ്കിൽ (eap != ശൂന്യ && *p != NUL)
		    eap->nextcmd = p;
		check_for_end = FALSE;
	    }

	എങ്കിൽ (check_for_end)
	    end_error = !ends_excmd2(arg, p);
    }

    എങ്കിൽ (ret == FAIL || end_error)
    {
	എങ്കിൽ (ret != FAIL)
	    clear_tv(rettv);
	/*
	 * Report the invalid expression unless the expression evaluation has
	 * been cancelled due to an aborting error, an interrupt, or an
	 * exception, or we already gave a more specific error.
	 * Also check called_emsg for when using assert_fails().
	 */
	എങ്കിൽ (!aborting()
		&& did_emsg == did_emsg_before
		&& called_emsg == called_emsg_before
		&& (!in_vim9script() || !vim9_bad_comment(p)))
	{
	    എങ്കിൽ (end_error)
		semsg(_(e_trailing_characters_str), p);
	    അല്ലെങ്കിൽ
		semsg(_(e_invalid_expression_str), arg);
	}

	എങ്കിൽ (eap != ശൂന്യ && p != ശൂന്യ)
	{
	    // Some of the expression may not have been consumed.
	    // Only execute a next command if it cannot be a "||" operator.
	    // The next command may be "catch".
	    char_u *nextcmd = check_nextcmd(p);
	    എങ്കിൽ (nextcmd != ശൂന്യ && *nextcmd != '|')
		eap->nextcmd = nextcmd;
	}
	തിരികെ FAIL;
    }

    എങ്കിൽ (retarg != ശൂന്യ)
	*retarg = p;
    അല്ലെങ്കിൽ എങ്കിൽ (check_for_end && eap != ശൂന്യ)
	set_nextcmd(eap, p);

    തിരികെ ret;
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
    പൂർണ്ണസംഖ്യ
eval1(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    char_u  *p;
    പൂർണ്ണസംഖ്യ	    getnext;

    CLEAR_POINTER(rettv);

    /*
     * Get the first variable.
     */
    എങ്കിൽ (eval2(arg, rettv, evalarg) == FAIL)
	തിരികെ FAIL;

    p = eval_next_non_blank(*arg, evalarg, &getnext);
    എങ്കിൽ (*p == '?')
    {
	പൂർണ്ണസംഖ്യ		op_falsy = p[1] == '?';
	പൂർണ്ണസംഖ്യ		result;
	typval_T	var2;
	evalarg_T	*evalarg_used = evalarg;
	evalarg_T	local_evalarg;
	പൂർണ്ണസംഖ്യ		orig_flags;
	പൂർണ്ണസംഖ്യ		evaluate;
	പൂർണ്ണസംഖ്യ		vim9script = in_vim9script();

	എങ്കിൽ (evalarg == ശൂന്യ)
	{
	    init_evalarg(&local_evalarg);
	    evalarg_used = &local_evalarg;
	}
	orig_flags = evalarg_used->eval_flags;
	evaluate = evalarg_used->eval_flags & EVAL_EVALUATE;

	എങ്കിൽ (getnext)
	    *arg = eval_next_വരി(*arg, evalarg_used);
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (evaluate && vim9script && !VIM_ISWHITE(p[-1]))
	    {
		error_white_both(p, op_falsy ? 2 : 1);
		clear_tv(rettv);
		തിരികെ FAIL;
	    }
	    *arg = p;
	}

	result = FALSE;
	എങ്കിൽ (evaluate)
	{
	    പൂർണ്ണസംഖ്യ		error = FALSE;

	    എങ്കിൽ (op_falsy)
	    {
		// Is this typeval supported with the falsy operator?
		എങ്കിൽ (check_typval_is_മൂല്യം(rettv) == FAIL)
		{
		    clear_tv(rettv);
		    തിരികെ FAIL;
		}
		result = tv2bool(rettv);
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (vim9script)
		result = tv_get_bool_chk(rettv, &error);
	    അല്ലെങ്കിൽ എങ്കിൽ (tv_get_സംഖ്യ_chk(rettv, &error) != 0)
		result = TRUE;
	    എങ്കിൽ (error || !op_falsy || !result)
		clear_tv(rettv);
	    എങ്കിൽ (error)
		തിരികെ FAIL;
	}

	/*
	 * Get the second variable.  Recursive!
	 */
	എങ്കിൽ (op_falsy)
	    ++*arg;
	എങ്കിൽ (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[1]))
	{
	    error_white_both(*arg - (op_falsy ? 1 : 0), op_falsy ? 2 : 1);
	    clear_tv(rettv);
	    തിരികെ FAIL;
	}
	*arg = skipwhite_and_linebreak(*arg + 1, evalarg_used);
	evalarg_used->eval_flags = (op_falsy ? !result : result)
				  ? orig_flags : (orig_flags & ~EVAL_EVALUATE);
	എങ്കിൽ (eval1(arg, &var2, evalarg_used) == FAIL)
	{
	    evalarg_used->eval_flags = orig_flags;
	    തിരികെ FAIL;
	}
	എങ്കിൽ (!op_falsy || !result)
	    *rettv = var2;

	എങ്കിൽ (!op_falsy)
	{
	    /*
	     * Check for the ":".
	     */
	    p = eval_next_non_blank(*arg, evalarg_used, &getnext);
	    എങ്കിൽ (*p != ':')
	    {
		emsg(_(e_missing_colon_after_questionmark));
		എങ്കിൽ (evaluate && result)
		    clear_tv(rettv);
		evalarg_used->eval_flags = orig_flags;
		തിരികെ FAIL;
	    }
	    എങ്കിൽ (getnext)
		*arg = eval_next_വരി(*arg, evalarg_used);
	    അല്ലെങ്കിൽ
	    {
		എങ്കിൽ (evaluate && vim9script && !VIM_ISWHITE(p[-1]))
		{
		    error_white_both(p, 1);
		    clear_tv(rettv);
		    evalarg_used->eval_flags = orig_flags;
		    തിരികെ FAIL;
		}
		*arg = p;
	    }

	    /*
	     * Get the third variable.  Recursive!
	     */
	    എങ്കിൽ (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[1]))
	    {
		error_white_both(*arg, 1);
		clear_tv(rettv);
		evalarg_used->eval_flags = orig_flags;
		തിരികെ FAIL;
	    }
	    *arg = skipwhite_and_linebreak(*arg + 1, evalarg_used);
	    evalarg_used->eval_flags = !result ? orig_flags
					       : (orig_flags & ~EVAL_EVALUATE);
	    എങ്കിൽ (eval1(arg, &var2, evalarg_used) == FAIL)
	    {
		എങ്കിൽ (evaluate && result)
		    clear_tv(rettv);
		evalarg_used->eval_flags = orig_flags;
		തിരികെ FAIL;
	    }
	    എങ്കിൽ (evaluate && !result)
		*rettv = var2;
	}

	എങ്കിൽ (evalarg == ശൂന്യ)
	    clear_evalarg(&local_evalarg, ശൂന്യ);
	അല്ലെങ്കിൽ
	    evalarg->eval_flags = orig_flags;
    }

    തിരികെ OK;
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
eval2(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    char_u	*p;
    പൂർണ്ണസംഖ്യ		getnext;

    /*
     * Get the first expression.
     */
    എങ്കിൽ (eval3(arg, rettv, evalarg) == FAIL)
	തിരികെ FAIL;

    /*
     * Handle the  "||" operator.
     */
    p = eval_next_non_blank(*arg, evalarg, &getnext);
    എങ്കിൽ (p[0] == '|' && p[1] == '|')
    {
	evalarg_T   *evalarg_used = evalarg;
	evalarg_T   local_evalarg;
	പൂർണ്ണസംഖ്യ	    evaluate;
	പൂർണ്ണസംഖ്യ	    orig_flags;
	long	    result = FALSE;
	typval_T    var2;
	പൂർണ്ണസംഖ്യ	    error = FALSE;
	പൂർണ്ണസംഖ്യ	    vim9script = in_vim9script();

	എങ്കിൽ (evalarg == ശൂന്യ)
	{
	    init_evalarg(&local_evalarg);
	    evalarg_used = &local_evalarg;
	}
	orig_flags = evalarg_used->eval_flags;
	evaluate = orig_flags & EVAL_EVALUATE;
	എങ്കിൽ (evaluate)
	{
	    എങ്കിൽ (vim9script)
		result = tv_get_bool_chk(rettv, &error);
	    അല്ലെങ്കിൽ എങ്കിൽ (tv_get_സംഖ്യ_chk(rettv, &error) != 0)
		result = TRUE;
	    clear_tv(rettv);
	    എങ്കിൽ (error)
		തിരികെ FAIL;
	}

	/*
	 * Repeat until there is no following "||".
	 */
	ആകുമ്പോൾ (p[0] == '|' && p[1] == '|')
	{
	    എങ്കിൽ (getnext)
		*arg = eval_next_വരി(*arg, evalarg_used);
	    അല്ലെങ്കിൽ
	    {
		എങ്കിൽ (evaluate && vim9script && !VIM_ISWHITE(p[-1]))
		{
		    error_white_both(p, 2);
		    clear_tv(rettv);
		    തിരികെ FAIL;
		}
		*arg = p;
	    }

	    /*
	     * Get the second variable.
	     */
	    എങ്കിൽ (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[2]))
	    {
		error_white_both(*arg, 2);
		clear_tv(rettv);
		തിരികെ FAIL;
	    }
	    *arg = skipwhite_and_linebreak(*arg + 2, evalarg_used);
	    evalarg_used->eval_flags = !result ? orig_flags
					       : (orig_flags & ~EVAL_EVALUATE);
	    എങ്കിൽ (eval3(arg, &var2, evalarg_used) == FAIL)
		തിരികെ FAIL;

	    /*
	     * Compute the result.
	     */
	    എങ്കിൽ (evaluate && !result)
	    {
		എങ്കിൽ (vim9script)
		    result = tv_get_bool_chk(&var2, &error);
		അല്ലെങ്കിൽ എങ്കിൽ (tv_get_സംഖ്യ_chk(&var2, &error) != 0)
		    result = TRUE;
		clear_tv(&var2);
		എങ്കിൽ (error)
		    തിരികെ FAIL;
	    }
	    എങ്കിൽ (evaluate)
	    {
		എങ്കിൽ (vim9script)
		{
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_സംഖ്യ = result ? VVAL_TRUE : VVAL_FALSE;
		}
		അല്ലെങ്കിൽ
		{
		    rettv->v_type = VAR_സംഖ്യ;
		    rettv->vval.v_സംഖ്യ = result;
		}
	    }

	    p = eval_next_non_blank(*arg, evalarg_used, &getnext);
	}

	എങ്കിൽ (evalarg == ശൂന്യ)
	    clear_evalarg(&local_evalarg, ശൂന്യ);
	അല്ലെങ്കിൽ
	    evalarg->eval_flags = orig_flags;
    }

    തിരികെ OK;
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
eval3(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    char_u	*p;
    പൂർണ്ണസംഖ്യ		getnext;

    /*
     * Get the first expression.
     */
    എങ്കിൽ (eval4(arg, rettv, evalarg) == FAIL)
	തിരികെ FAIL;

    /*
     * Handle the "&&" operator.
     */
    p = eval_next_non_blank(*arg, evalarg, &getnext);
    എങ്കിൽ (p[0] == '&' && p[1] == '&')
    {
	evalarg_T   *evalarg_used = evalarg;
	evalarg_T   local_evalarg;
	പൂർണ്ണസംഖ്യ	    orig_flags;
	പൂർണ്ണസംഖ്യ	    evaluate;
	long	    result = TRUE;
	typval_T    var2;
	പൂർണ്ണസംഖ്യ	    error = FALSE;
	പൂർണ്ണസംഖ്യ	    vim9script = in_vim9script();

	എങ്കിൽ (evalarg == ശൂന്യ)
	{
	    init_evalarg(&local_evalarg);
	    evalarg_used = &local_evalarg;
	}
	orig_flags = evalarg_used->eval_flags;
	evaluate = orig_flags & EVAL_EVALUATE;
	എങ്കിൽ (evaluate)
	{
	    എങ്കിൽ (vim9script)
		result = tv_get_bool_chk(rettv, &error);
	    അല്ലെങ്കിൽ എങ്കിൽ (tv_get_സംഖ്യ_chk(rettv, &error) == 0)
		result = FALSE;
	    clear_tv(rettv);
	    എങ്കിൽ (error)
		തിരികെ FAIL;
	}

	/*
	 * Repeat until there is no following "&&".
	 */
	ആകുമ്പോൾ (p[0] == '&' && p[1] == '&')
	{
	    എങ്കിൽ (getnext)
		*arg = eval_next_വരി(*arg, evalarg_used);
	    അല്ലെങ്കിൽ
	    {
		എങ്കിൽ (evaluate && vim9script && !VIM_ISWHITE(p[-1]))
		{
		    error_white_both(p, 2);
		    clear_tv(rettv);
		    തിരികെ FAIL;
		}
		*arg = p;
	    }

	    /*
	     * Get the second variable.
	     */
	    എങ്കിൽ (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[2]))
	    {
		error_white_both(*arg, 2);
		clear_tv(rettv);
		തിരികെ FAIL;
	    }
	    *arg = skipwhite_and_linebreak(*arg + 2, evalarg_used);
	    evalarg_used->eval_flags = result ? orig_flags
					      : (orig_flags & ~EVAL_EVALUATE);
	    CLEAR_FIELD(var2);
	    എങ്കിൽ (eval4(arg, &var2, evalarg_used) == FAIL)
		തിരികെ FAIL;

	    /*
	     * Compute the result.
	     */
	    എങ്കിൽ (evaluate && result)
	    {
		എങ്കിൽ (vim9script)
		    result = tv_get_bool_chk(&var2, &error);
		അല്ലെങ്കിൽ എങ്കിൽ (tv_get_സംഖ്യ_chk(&var2, &error) == 0)
		    result = FALSE;
		clear_tv(&var2);
		എങ്കിൽ (error)
		    തിരികെ FAIL;
	    }
	    എങ്കിൽ (evaluate)
	    {
		എങ്കിൽ (vim9script)
		{
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_സംഖ്യ = result ? VVAL_TRUE : VVAL_FALSE;
		}
		അല്ലെങ്കിൽ
		{
		    rettv->v_type = VAR_സംഖ്യ;
		    rettv->vval.v_സംഖ്യ = result;
		}
	    }

	    p = eval_next_non_blank(*arg, evalarg_used, &getnext);
	}

	എങ്കിൽ (evalarg == ശൂന്യ)
	    clear_evalarg(&local_evalarg, ശൂന്യ);
	അല്ലെങ്കിൽ
	    evalarg->eval_flags = orig_flags;
    }

    തിരികെ OK;
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
eval4(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    char_u	*p;
    പൂർണ്ണസംഖ്യ		getnext;
    exprtype_T	type = വ്യഞ്ജകം_UNKNOWN;
    പൂർണ്ണസംഖ്യ		len = 2;
    പൂർണ്ണസംഖ്യ		type_is = FALSE;

    /*
     * Get the first expression.
     */
    എങ്കിൽ (eval5(arg, rettv, evalarg) == FAIL)
	തിരികെ FAIL;

    p = eval_next_non_blank(*arg, evalarg, &getnext);

    type = get_compare_type(p, &len, &type_is);

    /*
     * If there is a comparative operator, use it.
     */
    എങ്കിൽ (type != വ്യഞ്ജകം_UNKNOWN)
    {
	typval_T    var2;
	പൂർണ്ണസംഖ്യ	    ic;
	പൂർണ്ണസംഖ്യ	    vim9script = in_vim9script();
	പൂർണ്ണസംഖ്യ	    evaluate = evalarg == ശൂന്യ
				   ? 0 : (evalarg->eval_flags & EVAL_EVALUATE);
	long	    comp_lnum = SOURCING_LNUM;

	എങ്കിൽ (getnext)
	{
	    *arg = eval_next_വരി(*arg, evalarg);
	    p = *arg;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (evaluate && vim9script && !VIM_ISWHITE(**arg))
	{
	    error_white_both(*arg, len);
	    clear_tv(rettv);
	    തിരികെ FAIL;
	}

	എങ്കിൽ (vim9script && type_is && (p[len] == '?' || p[len] == '#'))
	{
	    semsg(_(e_invalid_expression_str), p);
	    clear_tv(rettv);
	    തിരികെ FAIL;
	}

	// extra question mark appended: ignore case
	എങ്കിൽ (p[len] == '?')
	{
	    ic = TRUE;
	    ++len;
	}
	// extra '#' appended: match case
	അല്ലെങ്കിൽ എങ്കിൽ (p[len] == '#')
	{
	    ic = FALSE;
	    ++len;
	}
	// nothing appended: use 'ignorecase' if not in Vim script
	അല്ലെങ്കിൽ
	    ic = vim9script ? FALSE : p_ic;

	/*
	 * Get the second variable.
	 */
	എങ്കിൽ (evaluate && vim9script && !IS_WHITE_OR_NUL(p[len]))
	{
	    error_white_both(p, len);
	    clear_tv(rettv);
	    തിരികെ FAIL;
	}
	*arg = skipwhite_and_linebreak(p + len, evalarg);
	എങ്കിൽ (eval5(arg, &var2, evalarg) == FAIL)
	{
	    clear_tv(rettv);
	    തിരികെ FAIL;
	}
	എങ്കിൽ (evaluate)
	{
	    പൂർണ്ണസംഖ്യ ret;

	    // use the line of the comparison for messages
	    SOURCING_LNUM = comp_lnum;
	    എങ്കിൽ (vim9script && check_compare_types(type, rettv, &var2) == FAIL)
	    {
		ret = FAIL;
		clear_tv(rettv);
	    }
	    അല്ലെങ്കിൽ
		ret = typval_compare(rettv, &var2, type, ic);
	    clear_tv(&var2);
	    തിരികെ ret;
	}
    }

    തിരികെ OK;
}

/*
 * Make a copy of blob "tv1" and append blob "tv2".
 */
    ശൂന്യം
eval_addblob(typval_T *tv1, typval_T *tv2)
{
    blob_T  *b1 = tv1->vval.v_blob;
    blob_T  *b2 = tv2->vval.v_blob;
    blob_T  *b = blob_alloc();
    long    len1, len2;
    long    totallen;

    എങ്കിൽ (b == ശൂന്യ)
	തിരികെ;

    len1 = blob_len(b1);
    len2 = blob_len(b2);
    totallen = len1 + len2;

    എങ്കിൽ (totallen > 0 && totallen <= INT_MAX
				    && ga_grow(&b->bv_ga, (പൂർണ്ണസംഖ്യ)totallen) == OK)
    {
	എങ്കിൽ (len1 > 0)
	    mch_memmove((char_u *)b->bv_ga.ga_data,
		    (char_u *)b1->bv_ga.ga_data, (size_t)len1);
	എങ്കിൽ (len2 > 0)
	    mch_memmove((char_u *)b->bv_ga.ga_data + len1,
		    (char_u *)b2->bv_ga.ga_data, (size_t)len2);
	b->bv_ga.ga_len = (പൂർണ്ണസംഖ്യ)totallen;
    }

    clear_tv(tv1);
    rettv_blob_set(tv1, b);
}

/*
 * Make a copy of list "tv1" and append list "tv2".
 */
    പൂർണ്ണസംഖ്യ
eval_addlist(typval_T *tv1, typval_T *tv2)
{
    typval_T var3;

    // concatenate Lists
    എങ്കിൽ (list_concat(tv1->vval.v_list, tv2->vval.v_list, &var3) == FAIL)
    {
	clear_tv(tv1);
	clear_tv(tv2);
	തിരികെ FAIL;
    }
    clear_tv(tv1);
    *tv1 = var3;
    തിരികെ OK;
}

/*
 * Make a copy of tuple "tv1" and append tuple "tv2".
 */
    പൂർണ്ണസംഖ്യ
eval_addtuple(typval_T *tv1, typval_T *tv2)
{
    പൂർണ്ണസംഖ്യ		vim9script = in_vim9script();
    typval_T	var3;

    എങ്കിൽ (vim9script && tv1->vval.v_tuple != ശൂന്യ && tv2->vval.v_tuple != ശൂന്യ
	    && tv1->vval.v_tuple->tv_type != ശൂന്യ
	    && tv2->vval.v_tuple->tv_type != ശൂന്യ)
    {
	എങ്കിൽ (!check_tuples_addable(tv1->vval.v_tuple->tv_type,
						tv2->vval.v_tuple->tv_type))
	    തിരികെ FAIL;
    }

    // concatenate tuples
    എങ്കിൽ (tuple_concat(tv1->vval.v_tuple, tv2->vval.v_tuple, &var3) == FAIL)
    {
	clear_tv(tv1);
	clear_tv(tv2);
	തിരികെ FAIL;
    }
    clear_tv(tv1);
    *tv1 = var3;
    തിരികെ OK;
}

/*
 * Left or right shift the number "tv1" by the number "tv2" and store the
 * result in "tv1".
 *
 * Return OK or FAIL.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
eval_shift_സംഖ്യ(typval_T *tv1, typval_T *tv2, പൂർണ്ണസംഖ്യ shift_type)
{
    എങ്കിൽ (tv2->v_type != VAR_സംഖ്യ || tv2->vval.v_സംഖ്യ < 0)
    {
	// right operand should be a positive number
	എങ്കിൽ (tv2->v_type != VAR_സംഖ്യ)
	    emsg(_(e_bitshift_ops_must_be_സംഖ്യ));
	അല്ലെങ്കിൽ
	    emsg(_(e_bitshift_ops_must_be_positive));
	clear_tv(tv1);
	clear_tv(tv2);
	തിരികെ FAIL;
    }

    എങ്കിൽ (tv2->vval.v_സംഖ്യ > MAX_LSHIFT_BITS)
	// shifting more bits than we have always results in zero
	tv1->vval.v_സംഖ്യ = 0;
    അല്ലെങ്കിൽ എങ്കിൽ (shift_type == വ്യഞ്ജകം_LSHIFT)
	tv1->vval.v_സംഖ്യ =
	    (uvarnumber_T)tv1->vval.v_സംഖ്യ << tv2->vval.v_സംഖ്യ;
    അല്ലെങ്കിൽ
	tv1->vval.v_സംഖ്യ =
	    (uvarnumber_T)tv1->vval.v_സംഖ്യ >> tv2->vval.v_സംഖ്യ;

    തിരികെ OK;
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
eval5(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    /*
     * Get the first expression.
     */
    എങ്കിൽ (eval6(arg, rettv, evalarg) == FAIL)
	തിരികെ FAIL;

    /*
     * Repeat computing, until no '<<' or '>>' is following.
     */
    വേണ്ടി (;;)
    {
	char_u		*p;
	പൂർണ്ണസംഖ്യ		getnext;
	exprtype_T	exprtype;
	പൂർണ്ണസംഖ്യ		evaluate;
	typval_T	var2;
	പൂർണ്ണസംഖ്യ		vim9script;

	p = eval_next_non_blank(*arg, evalarg, &getnext);
	എങ്കിൽ (p[0] == '<' && p[1] == '<')
	    exprtype = വ്യഞ്ജകം_LSHIFT;
	അല്ലെങ്കിൽ എങ്കിൽ (p[0] == '>' && p[1] == '>')
	    exprtype = വ്യഞ്ജകം_RSHIFT;
	അല്ലെങ്കിൽ
	    തിരികെ OK;

	// Handle a bitwise left or right shift operator
	evaluate = evalarg == ശൂന്യ ? 0 : (evalarg->eval_flags & EVAL_EVALUATE);
	എങ്കിൽ (evaluate && rettv->v_type != VAR_സംഖ്യ)
	{
	    // left operand should be a number
	    emsg(_(e_bitshift_ops_must_be_സംഖ്യ));
	    clear_tv(rettv);
	    തിരികെ FAIL;
	}

	vim9script = in_vim9script();
	എങ്കിൽ (getnext)
	{
	    *arg = eval_next_വരി(*arg, evalarg);
	    p = *arg;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (evaluate && vim9script && !VIM_ISWHITE(**arg))
	{
	    error_white_both(*arg, 2);
	    clear_tv(rettv);
	    തിരികെ FAIL;
	}

	/*
	 * Get the second variable.
	 */
	എങ്കിൽ (evaluate && vim9script && !IS_WHITE_OR_NUL(p[2]))
	{
	    error_white_both(p, 2);
	    clear_tv(rettv);
	    തിരികെ FAIL;
	}
	*arg = skipwhite_and_linebreak(p + 2, evalarg);
	എങ്കിൽ (eval6(arg, &var2, evalarg) == FAIL)
	{
	    clear_tv(rettv);
	    തിരികെ FAIL;
	}

	എങ്കിൽ (evaluate)
	{
	    എങ്കിൽ (eval_shift_സംഖ്യ(rettv, &var2, exprtype) == FAIL)
		തിരികെ FAIL;
	}

	clear_tv(&var2);
    }

    തിരികെ OK;
}

/*
 * Concatenate strings "tv1" and "tv2" and store the result in "tv1".
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
eval_concat_str(typval_T *tv1, typval_T *tv2)
{
    char_u	buf1[NUMBUFLEN], buf2[NUMBUFLEN];
    char_u	*s1 = tv_get_string_buf(tv1, buf1);
    char_u	*s2 = ശൂന്യ;
    char_u	*p;
    പൂർണ്ണസംഖ്യ		vim9script = in_vim9script();

    എങ്കിൽ (vim9script && (tv2->v_type == VAR_VOID
		|| tv2->v_type == VAR_CHANNEL
		|| tv2->v_type == VAR_JOB))
	semsg(_(e_using_invalid_മൂല്യം_as_string_str),
		vartype_name(tv2->v_type));
    അല്ലെങ്കിൽ എങ്കിൽ (vim9script && tv2->v_type == VAR_FLOAT)
    {
	vim_snprintf((അക്ഷരം *)buf2, NUMBUFLEN, "%g",
		tv2->vval.v_float);
	s2 = buf2;
    }
    അല്ലെങ്കിൽ
	s2 = tv_get_string_buf_chk(tv2, buf2);
    എങ്കിൽ (s2 == ശൂന്യ)		// type error ?
    {
	clear_tv(tv1);
	clear_tv(tv2);
	തിരികെ FAIL;
    }

    // When possible, grow the existing string in place to avoid alloc/free.
    എങ്കിൽ (grow_string_tv(tv1, s2) == OK)
	തിരികെ OK;

    p = concat_str(s1, s2);
    clear_tv(tv1);
    tv1->v_type = VAR_STRING;
    tv1->vval.v_string = p;

    തിരികെ OK;
}

/*
 * Add or subtract numbers "tv1" and "tv2" and store the result in "tv1".
 * The numbers can be whole numbers or floats.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
eval_addsub_സംഖ്യ(typval_T *tv1, typval_T *tv2, പൂർണ്ണസംഖ്യ op)
{
    പൂർണ്ണസംഖ്യ		error = FALSE;
    varnumber_T	n1, n2;
    float_T	f1 = 0, f2 = 0;

    എങ്കിൽ (tv1->v_type == VAR_FLOAT)
    {
	f1 = tv1->vval.v_float;
	n1 = 0;
    }
    അല്ലെങ്കിൽ
    {
	n1 = tv_get_സംഖ്യ_chk(tv1, &error);
	എങ്കിൽ (error)
	{
	    // This can only happen for "list + non-list" or
	    // "blob + non-blob".  For "non-list + ..." or
	    // "something - ...", we returned before evaluating the
	    // 2nd operand.
	    clear_tv(tv1);
	    clear_tv(tv2);
	    തിരികെ FAIL;
	}
	എങ്കിൽ (tv2->v_type == VAR_FLOAT)
	    f1 = n1;
    }
    എങ്കിൽ (tv2->v_type == VAR_FLOAT)
    {
	f2 = tv2->vval.v_float;
	n2 = 0;
    }
    അല്ലെങ്കിൽ
    {
	n2 = tv_get_സംഖ്യ_chk(tv2, &error);
	എങ്കിൽ (error)
	{
	    clear_tv(tv1);
	    clear_tv(tv2);
	    തിരികെ FAIL;
	}
	എങ്കിൽ (tv1->v_type == VAR_FLOAT)
	    f2 = n2;
    }
    clear_tv(tv1);

    // If there is a float on either side the result is a float.
    എങ്കിൽ (tv1->v_type == VAR_FLOAT || tv2->v_type == VAR_FLOAT)
    {
	എങ്കിൽ (op == '+')
	    f1 = f1 + f2;
	അല്ലെങ്കിൽ
	    f1 = f1 - f2;
	tv1->v_type = VAR_FLOAT;
	tv1->vval.v_float = f1;
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (op == '+')
	    n1 = n1 + n2;
	അല്ലെങ്കിൽ
	    n1 = n1 - n2;
	tv1->v_type = VAR_സംഖ്യ;
	tv1->vval.v_സംഖ്യ = n1;
    }

    തിരികെ OK;
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
eval6(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    /*
     * Get the first expression.
     */
    എങ്കിൽ (eval7(arg, rettv, evalarg, FALSE) == FAIL)
	തിരികെ FAIL;

    /*
     * Repeat computing, until no '+', '-' or '.' is following.
     */
    വേണ്ടി (;;)
    {
	പൂർണ്ണസംഖ്യ	    evaluate;
	പൂർണ്ണസംഖ്യ	    getnext;
	char_u	    *p;
	പൂർണ്ണസംഖ്യ	    op;
	പൂർണ്ണസംഖ്യ	    oplen;
	പൂർണ്ണസംഖ്യ	    concat;
	typval_T    var2;
	പൂർണ്ണസംഖ്യ	    vim9script = in_vim9script();
	long	    op_lnum = SOURCING_LNUM;

	// "." is only string concatenation when scriptversion is 1
	// "+=", "-=" and "..=" are assignments
	// "++" and "--" on the next line are a separate command.
	p = eval_next_non_blank(*arg, evalarg, &getnext);
	op = *p;
	concat = op == '.' && (*(p + 1) == '.' || in_old_script(2));
	എങ്കിൽ ((op != '+' && op != '-' && !concat) || p[1] == '='
					       || (p[1] == '.' && p[2] == '='))
	    നിർത്തുക;
	എങ്കിൽ (getnext && (op == '+' || op == '-') && p[0] == p[1])
	    നിർത്തുക;

	evaluate = evalarg == ശൂന്യ ? 0 : (evalarg->eval_flags & EVAL_EVALUATE);
	oplen = (concat && p[1] == '.') ? 2 : 1;
	എങ്കിൽ (getnext)
	    *arg = eval_next_വരി(*arg, evalarg);
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (evaluate && vim9script && !VIM_ISWHITE(**arg))
	    {
		error_white_both(*arg, oplen);
		clear_tv(rettv);
		തിരികെ FAIL;
	    }
	    *arg = p;
	}
	എങ്കിൽ ((op != '+' || (rettv->v_type != VAR_LIST
						&& rettv->v_type != VAR_TUPLE
						&& rettv->v_type != VAR_BLOB))
		&& (op == '.' || rettv->v_type != VAR_FLOAT)
		&& evaluate)
	{
	    പൂർണ്ണസംഖ്യ		error = FALSE;

	    // For "list + ...", an illegal use of the first operand as
	    // a number cannot be determined before evaluating the 2nd
	    // operand: if this is also a list, all is ok.
	    // For "something . ...", "something - ..." or "non-list + ...",
	    // we know that the first operand needs to be a string or number
	    // without evaluating the 2nd operand.  So check before to avoid
	    // side effects after an error.
	    എങ്കിൽ (op != '.')
		tv_get_സംഖ്യ_chk(rettv, &error);
	    എങ്കിൽ ((op == '.' && tv_get_string_chk(rettv) == ശൂന്യ) || error)
	    {
		clear_tv(rettv);
		തിരികെ FAIL;
	    }
	}

	/*
	 * Get the second variable.
	 */
	എങ്കിൽ (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[oplen]))
	{
	    error_white_both(*arg, oplen);
	    clear_tv(rettv);
	    തിരികെ FAIL;
	}
	*arg = skipwhite_and_linebreak(*arg + oplen, evalarg);
	എങ്കിൽ (eval7(arg, &var2, evalarg, !vim9script && op == '.') == FAIL)
	{
	    clear_tv(rettv);
	    തിരികെ FAIL;
	}

	എങ്കിൽ (evaluate)
	{
	    /*
	     * Compute the result.
	     */
	    // use the line of the operation for messages
	    SOURCING_LNUM = op_lnum;
	    എങ്കിൽ (op == '.')
	    {
		എങ്കിൽ (eval_concat_str(rettv, &var2) == FAIL)
		    തിരികെ FAIL;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (op == '+' && rettv->v_type == VAR_BLOB
						   && var2.v_type == VAR_BLOB)
		eval_addblob(rettv, &var2);
	    അല്ലെങ്കിൽ എങ്കിൽ (op == '+' && rettv->v_type == VAR_LIST
						   && var2.v_type == VAR_LIST)
	    {
		എങ്കിൽ (eval_addlist(rettv, &var2) == FAIL)
		    തിരികെ FAIL;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (op == '+' && rettv->v_type == VAR_TUPLE
					   && var2.v_type == VAR_TUPLE)
	    {
		എങ്കിൽ (eval_addtuple(rettv, &var2) == FAIL)
		    തിരികെ FAIL;
	    }
	    അല്ലെങ്കിൽ
	    {
		എങ്കിൽ (eval_addsub_സംഖ്യ(rettv, &var2, op) == FAIL)
		    തിരികെ FAIL;
	    }
	    clear_tv(&var2);
	}
    }
    തിരികെ OK;
}

/*
 * Multiply or divide or compute the modulo of numbers "tv1" and "tv2" and
 * store the result in "tv1".  The numbers can be whole numbers or floats.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
eval_multdiv_സംഖ്യ(typval_T *tv1, typval_T *tv2, പൂർണ്ണസംഖ്യ op)
{
    varnumber_T	n1, n2;
    float_T	f1, f2;
    പൂർണ്ണസംഖ്യ		error;
    പൂർണ്ണസംഖ്യ		use_float = FALSE;

    f1 = 0;
    f2 = 0;
    error = FALSE;
    എങ്കിൽ (tv1->v_type == VAR_FLOAT)
    {
	f1 = tv1->vval.v_float;
	use_float = TRUE;
	n1 = 0;
    }
    അല്ലെങ്കിൽ
	n1 = tv_get_സംഖ്യ_chk(tv1, &error);
    clear_tv(tv1);
    എങ്കിൽ (error)
    {
	clear_tv(tv2);
	തിരികെ FAIL;
    }

    എങ്കിൽ (tv2->v_type == VAR_FLOAT)
    {
	എങ്കിൽ (!use_float)
	{
	    f1 = n1;
	    use_float = TRUE;
	}
	f2 = tv2->vval.v_float;
	n2 = 0;
    }
    അല്ലെങ്കിൽ
    {
	n2 = tv_get_സംഖ്യ_chk(tv2, &error);
	clear_tv(tv2);
	എങ്കിൽ (error)
	    തിരികെ FAIL;
	എങ്കിൽ (use_float)
	    f2 = n2;
    }

    /*
     * Compute the result.
     * When either side is a float the result is a float.
     */
    എങ്കിൽ (use_float)
    {
	എങ്കിൽ (op == '*')
	    f1 = f1 * f2;
	അല്ലെങ്കിൽ എങ്കിൽ (op == '/')
	{
#ifdef VMS
	    // VMS crashes on divide by zero, work around it
	    എങ്കിൽ (f2 == 0.0)
	    {
		എങ്കിൽ (f1 == 0)
		    f1 = -1 * __F_FLT_MAX - 1L;   // similar to NaN
		അല്ലെങ്കിൽ എങ്കിൽ (f1 < 0)
		    f1 = -1 * __F_FLT_MAX;
		അല്ലെങ്കിൽ
		    f1 = __F_FLT_MAX;
	    }
	    അല്ലെങ്കിൽ
		f1 = f1 / f2;
#അല്ലെങ്കിൽ
	    // We rely on the floating point library to handle divide
	    // by zero to result in "inf" and not a crash.
	    f1 = f1 / f2;
#endif
	}
	അല്ലെങ്കിൽ
	{
	    emsg(_(e_cannot_use_percent_with_float));
	    തിരികെ FAIL;
	}
	tv1->v_type = VAR_FLOAT;
	tv1->vval.v_float = f1;
    }
    അല്ലെങ്കിൽ
    {
	പൂർണ്ണസംഖ്യ	    failed = FALSE;

	എങ്കിൽ (op == '*')
	    n1 = n1 * n2;
	അല്ലെങ്കിൽ എങ്കിൽ (op == '/')
	    n1 = അക്കം_divide(n1, n2, &failed);
	അല്ലെങ്കിൽ
	    n1 = അക്കം_modulus(n1, n2, &failed);
	എങ്കിൽ (failed)
	    തിരികെ FAIL;

	tv1->v_type = VAR_സംഖ്യ;
	tv1->vval.v_സംഖ്യ = n1;
    }

    തിരികെ OK;
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
eval7(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    പൂർണ്ണസംഖ്യ		want_string)  // after "." operator
{
    /*
     * Get the first expression.
     */
    എങ്കിൽ (eval8(arg, rettv, evalarg, want_string) == FAIL)
	തിരികെ FAIL;

    /*
     * Repeat computing, until no '*', '/' or '%' is following.
     */
    വേണ്ടി (;;)
    {
	പൂർണ്ണസംഖ്യ	    evaluate;
	പൂർണ്ണസംഖ്യ	    getnext;
	typval_T    var2;
	char_u	    *p;
	പൂർണ്ണസംഖ്യ	    op;

	// "*=", "/=" and "%=" are assignments
	p = eval_next_non_blank(*arg, evalarg, &getnext);
	op = *p;
	എങ്കിൽ ((op != '*' && op != '/' && op != '%') || p[1] == '=')
	    നിർത്തുക;

	evaluate = evalarg == ശൂന്യ ? 0 : (evalarg->eval_flags & EVAL_EVALUATE);
	എങ്കിൽ (getnext)
	    *arg = eval_next_വരി(*arg, evalarg);
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (evaluate && in_vim9script() && !VIM_ISWHITE(**arg))
	    {
		error_white_both(*arg, 1);
		clear_tv(rettv);
		തിരികെ FAIL;
	    }
	    *arg = p;
	}

	/*
	 * Get the second variable.
	 */
	എങ്കിൽ (evaluate && in_vim9script() && !IS_WHITE_OR_NUL((*arg)[1]))
	{
	    error_white_both(*arg, 1);
	    clear_tv(rettv);
	    തിരികെ FAIL;
	}
	*arg = skipwhite_and_linebreak(*arg + 1, evalarg);
	എങ്കിൽ (eval8(arg, &var2, evalarg, FALSE) == FAIL)
	    തിരികെ FAIL;

	എങ്കിൽ (evaluate)
	    // Compute the result.
	    എങ്കിൽ (eval_multdiv_സംഖ്യ(rettv, &var2, op) == FAIL)
		തിരികെ FAIL;
    }

    തിരികെ OK;
}

/*
 * Handle seventh level expression:
 *	a type cast before a base level expression.
 * "arg" must point to the first non-white of the expression.
 * "arg" is advanced to just after the recognized expression.
 * Return OK or FAIL.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
eval8(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    പൂർണ്ണസംഖ്യ		want_string)	// after "." operator
{
    type_T	*want_type = ശൂന്യ;
    garray_T	type_list;	    // list of pointers to allocated types
    പൂർണ്ണസംഖ്യ		res;
    പൂർണ്ണസംഖ്യ		evaluate = evalarg == ശൂന്യ ? 0
				       : (evalarg->eval_flags & EVAL_EVALUATE);

    // Recognize <type> in Vim9 script only.
    എങ്കിൽ (in_vim9script() && **arg == '<' && eval_isnamec1((*arg)[1])
					     && STRNCMP(*arg, "<SNR>", 5) != 0)
    {
	++*arg;
	ga_init2(&type_list, വലുപ്പം(type_T *), 10);
	എങ്കിൽ (!evaluate)
	    // Only skip over the type, it may use a type variable that is only
	    // known when the expression is compiled, e.g. in a lambda.
	    *arg = skip_type(*arg, FALSE);
	അല്ലെങ്കിൽ
	{
	    want_type = parse_type(arg, &type_list, ശൂന്യ, ശൂന്യ, TRUE);
	    എങ്കിൽ (want_type == ശൂന്യ)
	    {
		clear_type_list(&type_list);
		തിരികെ FAIL;
	    }
	}

	എങ്കിൽ (**arg != '>')
	{
	    എങ്കിൽ (*skipwhite(*arg) == '>')
		semsg(_(e_no_white_space_allowed_before_str_str), ">", *arg);
	    അല്ലെങ്കിൽ
		emsg(_(e_missing_gt));
	    clear_type_list(&type_list);
	    തിരികെ FAIL;
	}
	++*arg;
	*arg = skipwhite_and_linebreak(*arg, evalarg);
    }

    res = eval9(arg, rettv, evalarg, want_string);

    എങ്കിൽ (want_type != ശൂന്യ)
    {
	എങ്കിൽ (evaluate && res == OK)
	{
	    type_T *actual = typval2type(rettv, get_copyID(), &type_list,
							       TVTT_DO_MEMBER);

	    എങ്കിൽ (!equal_type(want_type, actual, 0))
	    {
		എങ്കിൽ (want_type->tt_type == VAR_BOOL
					&& actual->tt_type != VAR_BOOL
					&& (actual->tt_flags & TTFLAG_BOOL_OK))
		{
		    പൂർണ്ണസംഖ്യ n = tv2bool(rettv);

		    // can use "0" and "1" for boolean in some places
		    clear_tv(rettv);
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_സംഖ്യ = n ? VVAL_TRUE : VVAL_FALSE;
		}
		അല്ലെങ്കിൽ
		{
		    where_T where = WHERE_INIT;

		    res = check_type(want_type, actual, TRUE, where);
		}
	    }
	}
	clear_type_list(&type_list);
    }

    തിരികെ res;
}

    പൂർണ്ണസംഖ്യ
eval_leader(char_u **arg, പൂർണ്ണസംഖ്യ vim9)
{
    char_u	*s = *arg;
    char_u	*p = *arg;

    ആകുമ്പോൾ (*p == '!' || *p == '-' || *p == '+')
    {
	char_u *n = skipwhite(p + 1);

	// ++, --, -+ and +- are not accepted in Vim9 script
	എങ്കിൽ (vim9 && (*p == '-' || *p == '+') && (*n == '-' || *n == '+'))
	{
	    semsg(_(e_invalid_expression_str), s);
	    തിരികെ FAIL;
	}
	p = n;
    }
    *arg = p;
    തിരികെ OK;
}

/*
 * Check for a predefined value "true", "false" and "null.*".
 * Return OK when recognized.
 */
    പൂർണ്ണസംഖ്യ
handle_predefined(char_u *s, പൂർണ്ണസംഖ്യ len, typval_T *rettv)
{
    തിരഞ്ഞെടുപ്പ് (len)
    {
	അവസ്ഥ 4: എങ്കിൽ (STRNCMP(s, "true", 4) == 0)
		{
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_സംഖ്യ = VVAL_TRUE;
		    തിരികെ OK;
		}
		എങ്കിൽ (STRNCMP(s, "null", 4) == 0)
		{
		    rettv->v_type = VAR_SPECIAL;
		    rettv->vval.v_സംഖ്യ = VVAL_NULL;
		    തിരികെ OK;
		}
		നിർത്തുക;
	അവസ്ഥ 5: എങ്കിൽ (STRNCMP(s, "false", 5) == 0)
		{
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_സംഖ്യ = VVAL_FALSE;
		    തിരികെ OK;
		}
		നിർത്തുക;
	അവസ്ഥ 8: എങ്കിൽ (STRNCMP(s, "null_job", 8) == 0)
		{
#ifdef FEAT_JOB_CHANNEL
		    rettv->v_type = VAR_JOB;
		    rettv->vval.v_job = ശൂന്യ;
#അല്ലെങ്കിൽ
		    rettv->v_type = VAR_SPECIAL;
		    rettv->vval.v_സംഖ്യ = VVAL_NULL;
#endif
		    തിരികെ OK;
		}
		നിർത്തുക;
	അവസ്ഥ 9:
		എങ്കിൽ (STRNCMP(s, "null_", 5) != 0)
		    നിർത്തുക;
		// null_list
		എങ്കിൽ (STRNCMP(s + 5, "list", 4) == 0)
		{
		    rettv->v_type = VAR_LIST;
		    rettv->vval.v_list = ശൂന്യ;
		    തിരികെ OK;
		}
		// null_dict
		എങ്കിൽ (STRNCMP(s + 5, "dict", 4) == 0)
		{
		    rettv->v_type = VAR_DICT;
		    rettv->vval.v_dict = ശൂന്യ;
		    തിരികെ OK;
		}
		// null_blob
		എങ്കിൽ (STRNCMP(s + 5, "blob", 4) == 0)
		{
		    rettv->v_type = VAR_BLOB;
		    rettv->vval.v_blob = ശൂന്യ;
		    തിരികെ OK;
		}
		നിർത്തുക;
	അവസ്ഥ 10:
		എങ്കിൽ (STRNCMP(s, "null_", 5) != 0)
		    നിർത്തുക;
		// null_class
		എങ്കിൽ (STRNCMP(s + 5, "class", 5) == 0)
		{
		    rettv->v_type = VAR_CLASS;
		    rettv->vval.v_class = ശൂന്യ;
		    തിരികെ OK;
		}
		എങ്കിൽ (STRNCMP(s + 5, "tuple", 5) == 0)
		{
		    rettv->v_type = VAR_TUPLE;
		    rettv->vval.v_tuple = ശൂന്യ;
		    തിരികെ OK;
		}
		നിർത്തുക;
	അവസ്ഥ 11: എങ്കിൽ (STRNCMP(s, "null_string", 11) == 0)
		{
		    rettv->v_type = VAR_STRING;
		    rettv->vval.v_string = ശൂന്യ;
		    തിരികെ OK;
		}
		എങ്കിൽ (STRNCMP(s, "null_object", 11) == 0)
		{
		    rettv->v_type = VAR_OBJECT;
		    rettv->vval.v_object = ശൂന്യ;
		    തിരികെ OK;
		}
		നിർത്തുക;
	അവസ്ഥ 12:
		എങ്കിൽ (STRNCMP(s, "null_channel", 12) == 0)
		{
#ifdef FEAT_JOB_CHANNEL
		    rettv->v_type = VAR_CHANNEL;
		    rettv->vval.v_channel = ശൂന്യ;
#അല്ലെങ്കിൽ
		    rettv->v_type = VAR_SPECIAL;
		    rettv->vval.v_സംഖ്യ = VVAL_NULL;
#endif
		    തിരികെ OK;
		}
		എങ്കിൽ (STRNCMP(s, "null_partial", 12) == 0)
		{
		    rettv->v_type = VAR_PARTIAL;
		    rettv->vval.v_partial = ശൂന്യ;
		    തിരികെ OK;
		}
		നിർത്തുക;
	അവസ്ഥ 13: എങ്കിൽ (STRNCMP(s, "null_function", 13) == 0)
		{
		    rettv->v_type = VAR_FUNC;
		    rettv->vval.v_string = ശൂന്യ;
		    തിരികെ OK;
		}
		നിർത്തുക;
    }
    തിരികെ FAIL;
}

/*
 * Handle register contents: @r.
 */
    സ്ഥിതം ശൂന്യം
eval9_reg_contents(
    char_u	**arg,
    typval_T	*rettv,
    പൂർണ്ണസംഖ്യ		evaluate)
{
    പൂർണ്ണസംഖ്യ		vim9script = in_vim9script();

    ++*arg;	// skip '@'
    എങ്കിൽ (evaluate)
    {
	എങ്കിൽ (vim9script && IS_WHITE_OR_NUL(**arg))
	    semsg(_(e_syntax_error_at_str), *arg);
	അല്ലെങ്കിൽ എങ്കിൽ (vim9script && !valid_yank_reg(**arg, FALSE))
	    emsg_invreg(**arg);
	അല്ലെങ്കിൽ
	{
	    rettv->v_type = VAR_STRING;
	    rettv->vval.v_string = get_reg_contents(**arg,
		    GREG_വ്യഞ്ജകം_SRC);
	}
    }
    എങ്കിൽ (**arg != NUL)
	++*arg;
}

/*
 * Handle a nested expression: (expression) or lambda: (arg) => expr
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
eval9_nested_വ്യഞ്ജകം(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    പൂർണ്ണസംഖ്യ		evaluate)
{
    പൂർണ്ണസംഖ്യ		ret = NOTDONE;
    പൂർണ്ണസംഖ്യ		vim9script = in_vim9script();

    എങ്കിൽ (vim9script)
    {
	ret = get_lambda_tv(arg, rettv, TRUE, evalarg, ശൂന്യ);
	എങ്കിൽ (ret == OK && evaluate)
	{
	    ufunc_T *ufunc = rettv->vval.v_partial->pt_func;

	    // Compile it here to get the return type.  The return
	    // type is optional, when it's missing use t_unknown.
	    // This is recognized in compile_return().
	    എങ്കിൽ (ufunc->uf_ret_type->tt_type == VAR_VOID)
		ufunc->uf_ret_type = &t_unknown;
	    എങ്കിൽ (compile_def_function(ufunc, FALSE,
				get_compile_type(ufunc), ശൂന്യ) == FAIL)
	    {
		clear_tv(rettv);
		ret = FAIL;
	    }
	}
    }
    എങ്കിൽ (ret == NOTDONE)
    {
	*arg = skipwhite_and_linebreak(*arg + 1, evalarg);
	എങ്കിൽ (**arg == ')')
	    // empty tuple
	    ret = eval_tuple(arg, rettv, evalarg, TRUE);
	അല്ലെങ്കിൽ
	{
	    ret = eval1(arg, rettv, evalarg);	// recursive!
	    എങ്കിൽ (ret != OK)
		തിരികെ ret;

	    *arg = skipwhite_and_linebreak(*arg, evalarg);

	    എങ്കിൽ (**arg == ',')
		// tuple
		ret = eval_tuple(arg, rettv, evalarg, TRUE);
	    അല്ലെങ്കിൽ എങ്കിൽ (**arg == ')')
		++*arg;
	    അല്ലെങ്കിൽ എങ്കിൽ (ret == OK)
	    {
		emsg(_(e_missing_closing_paren));
		clear_tv(rettv);
		ret = FAIL;
	    }
	}
    }

    തിരികെ ret;
}

/*
* Handle be a variable or function name.
* Can also be a curly-braces kind of name: {expr}.
*/
    സ്ഥിതം പൂർണ്ണസംഖ്യ
eval9_var_func_name(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    പൂർണ്ണസംഖ്യ		evaluate,
    char_u	**name_start)
{
    char_u	*s;
    പൂർണ്ണസംഖ്യ		len;
    char_u	*alias;
    പൂർണ്ണസംഖ്യ		ret = OK;
    പൂർണ്ണസംഖ്യ		vim9script = in_vim9script();

    s = *arg;
    len = get_name_len(arg, &alias, evaluate, TRUE);
    എങ്കിൽ (alias != ശൂന്യ)
	s = alias;

    എങ്കിൽ (len <= 0)
	ret = FAIL;
    അല്ലെങ്കിൽ
    {
	പൂർണ്ണസംഖ്യ	flags = evalarg == ശൂന്യ ? 0 : evalarg->eval_flags;

	എങ്കിൽ (evaluate && vim9script && len == 1 && *s == '_')
	{
	    emsg(_(e_cannot_use_underscore_here));
	    ret = FAIL;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (evaluate && vim9script && len > 2
						&& s[0] == 's' && s[1] == ':')
	{
	    semsg(_(e_cannot_use_s_colon_in_vim9_script_str), s);
	    ret = FAIL;
	}
	അല്ലെങ്കിൽ എങ്കിൽ ((vim9script ? **arg : *skipwhite(*arg)) == '('
				|| (vim9script && generic_func_call(arg)))
	{
	    // "name(..."  recursive!
	    *arg = skipwhite(*arg);
	    ret = eval_func(arg, evalarg, s, len, rettv, flags, ശൂന്യ);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (evaluate)
	{
	    // get the value of "true", "false", etc. or a variable
	    ret = FAIL;
	    എങ്കിൽ (vim9script)
		ret = handle_predefined(s, len, rettv);
	    എങ്കിൽ (ret == FAIL)
	    {
		*name_start = s;
		ret = eval_variable(s, len, 0, rettv, ശൂന്യ,
					EVAL_VAR_VERBOSE + EVAL_VAR_IMPORT);

		// skip the generic function arguments (if present)
		// they are already processed by eval_variable
		എങ്കിൽ (ret == OK && vim9script && **arg == '<'
						&& rettv->v_type == VAR_FUNC)
		    ret = skip_generic_func_type_args(arg);
	    }
	}
	അല്ലെങ്കിൽ
	{
	    // skip the name
	    check_vars(s, len);

	    // Skip the type arguments of a generic function, its name starts
	    // with an uppercase letter.
	    എങ്കിൽ (vim9script && **arg == '<' && ASCII_ISUPPER(*s))
		skip_generic_func_type_args(arg);
	    ret = OK;
	}
    }
    vim_free(alias);

    തിരികെ ret;
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
eval9(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    പൂർണ്ണസംഖ്യ		want_string)	// after "." operator
{
    പൂർണ്ണസംഖ്യ		evaluate = evalarg != ശൂന്യ
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    char_u	*name_start = ശൂന്യ;
    char_u	*start_leader, *end_leader;
    പൂർണ്ണസംഖ്യ		ret = OK;
    സ്ഥിതം പൂർണ്ണസംഖ്യ	recurse = 0;
    പൂർണ്ണസംഖ്യ		vim9script = in_vim9script();

    /*
     * Initialise variable so that clear_tv() can't mistake this for a
     * string and free a string that isn't there.
     */
    rettv->v_type = VAR_UNKNOWN;

    /*
     * Skip '!', '-' and '+' characters.  They are handled later.
     */
    start_leader = *arg;
    എങ്കിൽ (eval_leader(arg, vim9script) == FAIL)
	തിരികെ FAIL;
    end_leader = *arg;

    എങ്കിൽ (**arg == '.' && (!SAFE_isdigit(*(*arg + 1)) || in_old_script(2)))
    {
	semsg(_(e_invalid_expression_str), *arg);
	++*arg;
	തിരികെ FAIL;
    }

    // Limit recursion to 1000 levels.  At least at 10000 we run out of stack
    // and crash.  With MSVC the stack is smaller.
    എങ്കിൽ (recurse ==
#ifdef _MSC_VER
		    300
#അല്ലെങ്കിൽ
		    1000
#endif
		    )
    {
	semsg(_(e_expression_too_recursive_str), *arg);
	തിരികെ FAIL;
    }
    ++recurse;

    തിരഞ്ഞെടുപ്പ് (**arg)
    {
    /*
     * Number constant.
     */
    അവസ്ഥ '0':
    അവസ്ഥ '1':
    അവസ്ഥ '2':
    അവസ്ഥ '3':
    അവസ്ഥ '4':
    അവസ്ഥ '5':
    അവസ്ഥ '6':
    അവസ്ഥ '7':
    അവസ്ഥ '8':
    അവസ്ഥ '9':
    അവസ്ഥ '.':	ret = eval_സംഖ്യ(arg, rettv, evaluate, want_string);

		// Apply prefixed "-" and "+" now.  Matters especially when
		// "->" follows.
		എങ്കിൽ (ret == OK && evaluate && end_leader > start_leader
						  && rettv->v_type != VAR_BLOB)
		    ret = eval9_leader(rettv, TRUE, start_leader, &end_leader);
		നിർത്തുക;

    /*
     * String constant: "string".
     */
    അവസ്ഥ '"':	ret = eval_string(arg, rettv, evaluate, FALSE);
		നിർത്തുക;

    /*
     * Literal string constant: 'str''ing'.
     */
    അവസ്ഥ '\'':	ret = eval_lit_string(arg, rettv, evaluate, FALSE);
		നിർത്തുക;

    /*
     * List: [expr, expr]
     */
    അവസ്ഥ '[':	ret = eval_list(arg, rettv, evalarg, TRUE);
		നിർത്തുക;

    /*
     * Literal Dictionary: #{key: val, key: val}
     */
    അവസ്ഥ '#':	ret = eval_lit_dict(arg, rettv, evalarg);
		നിർത്തുക;

    /*
     * Lambda: {arg, arg -> expr}
     * Dictionary: {'key': val, 'key': val}
     */
    അവസ്ഥ '{':	എങ്കിൽ (vim9script)
		    ret = NOTDONE;
		അല്ലെങ്കിൽ
		    ret = get_lambda_tv(arg, rettv, vim9script, evalarg, ശൂന്യ);
		എങ്കിൽ (ret == NOTDONE)
		    ret = eval_dict(arg, rettv, evalarg, FALSE);
		നിർത്തുക;

    /*
     * Option value: &name
     */
    അവസ്ഥ '&':	ret = eval_option(arg, rettv, evaluate);
		നിർത്തുക;

    /*
     * Environment variable: $VAR.
     * Interpolated string: $"string" or $'string'.
     */
    അവസ്ഥ '$':	എങ്കിൽ ((*arg)[1] == '"' || (*arg)[1] == '\'')
		    ret = eval_interp_string(arg, rettv, evaluate);
		അല്ലെങ്കിൽ
		    ret = eval_env_var(arg, rettv, evaluate);
		നിർത്തുക;

    /*
     * Register contents: @r.
     */
    അവസ്ഥ '@':	eval9_reg_contents(arg, rettv, evaluate);
		നിർത്തുക;

    /*
     * nested expression: (expression).
     * or lambda: (arg) => expr
     * or tuple
     */
    അവസ്ഥ '(':	ret = eval9_nested_വ്യഞ്ജകം(arg, rettv, evalarg, evaluate);
		നിർത്തുക;

    സ്ഥിരസ്ഥിതി:	ret = NOTDONE;
		നിർത്തുക;
    }

    എങ്കിൽ (ret == NOTDONE)
    {
	/*
	 * Must be a variable or function name.
	 * Can also be a curly-braces kind of name: {expr}.
	 */
	ret = eval9_var_func_name(arg, rettv, evalarg, evaluate, &name_start);
    }

    // Handle following '[', '(' and '.' for expr[expr], expr.name,
    // expr(expr), expr->name(expr)
    എങ്കിൽ (ret == OK)
	ret = handle_subscript(arg, name_start, rettv, evalarg, evaluate);

    /*
     * Apply logical NOT and unary '-', from right to left, ignore '+'.
     */
    എങ്കിൽ (ret == OK && evaluate && end_leader > start_leader)
	ret = eval9_leader(rettv, FALSE, start_leader, &end_leader);

    --recurse;
    തിരികെ ret;
}

/*
 * Apply the leading "!" and "-" before an eval9 expression to "rettv".
 * When "numeric_only" is TRUE only handle "+" and "-".
 * Adjusts "end_leaderp" until it is at "start_leader".
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
eval9_leader(
	typval_T    *rettv,
	പൂർണ്ണസംഖ്യ	    numeric_only,
	char_u	    *start_leader,
	char_u	    **end_leaderp)
{
    char_u	*end_leader = *end_leaderp;
    പൂർണ്ണസംഖ്യ		ret = OK;
    പൂർണ്ണസംഖ്യ		error = FALSE;
    varnumber_T val = 0;
    vartype_T	type = rettv->v_type;
    പൂർണ്ണസംഖ്യ		vim9script = in_vim9script();
    float_T	    f = 0.0;

    എങ്കിൽ (rettv->v_type == VAR_FLOAT)
	f = rettv->vval.v_float;
    അല്ലെങ്കിൽ
    {
	ആകുമ്പോൾ (VIM_ISWHITE(end_leader[-1]))
	    --end_leader;
	എങ്കിൽ (vim9script && end_leader[-1] == '!')
	{
	    // Is this typeval supported with the ! operator?
	    എങ്കിൽ (check_typval_is_മൂല്യം(rettv) == FAIL)
	    {
		clear_tv(rettv);
		തിരികെ FAIL;
	    }
	    val = tv2bool(rettv);
	}
	അല്ലെങ്കിൽ
	    val = tv_get_സംഖ്യ_chk(rettv, &error);
    }
    എങ്കിൽ (error)
    {
	clear_tv(rettv);
	ret = FAIL;
    }
    അല്ലെങ്കിൽ
    {
	ആകുമ്പോൾ (end_leader > start_leader)
	{
	    --end_leader;
	    എങ്കിൽ (*end_leader == '!')
	    {
		എങ്കിൽ (numeric_only)
		{
		    ++end_leader;
		    നിർത്തുക;
		}
		എങ്കിൽ (rettv->v_type == VAR_FLOAT)
		{
		    rettv->v_type = VAR_BOOL;
		    val = f == 0.0 ? VVAL_TRUE : VVAL_FALSE;
		    type = VAR_BOOL;
		}
		അല്ലെങ്കിൽ
		{
		    val = !val;
		    type = VAR_BOOL;
		}
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (*end_leader == '-')
	    {
		എങ്കിൽ (rettv->v_type == VAR_FLOAT)
		    f = -f;
		അല്ലെങ്കിൽ
		{
		    val = -val;
		    type = VAR_സംഖ്യ;
		}
	    }
	}
	എങ്കിൽ (rettv->v_type == VAR_FLOAT)
	{
	    clear_tv(rettv);
	    rettv->vval.v_float = f;
	}
	അല്ലെങ്കിൽ
	{
	    clear_tv(rettv);
	    എങ്കിൽ (vim9script)
		rettv->v_type = type;
	    അല്ലെങ്കിൽ
		rettv->v_type = VAR_സംഖ്യ;
	    rettv->vval.v_സംഖ്യ = val;
	}
    }
    *end_leaderp = end_leader;
    തിരികെ ret;
}

/*
 * Call the function referred to in "rettv".
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
call_func_rettv(
	char_u	    **arg,
	evalarg_T   *evalarg,
	typval_T    *rettv,
	പൂർണ്ണസംഖ്യ	    evaluate,
	dict_T	    *selfdict,
	typval_T    *basetv)
{
    partial_T	*pt = ശൂന്യ;
    funcexe_T	funcexe;
    typval_T	functv;
    char_u	*s;
    പൂർണ്ണസംഖ്യ		ret;

    // need to copy the funcref so that we can clear rettv
    എങ്കിൽ (evaluate)
    {
	functv = *rettv;
	rettv->v_type = VAR_UNKNOWN;

	// Invoke the function.  Recursive!
	എങ്കിൽ (functv.v_type == VAR_PARTIAL)
	{
	    pt = functv.vval.v_partial;
	    s = partial_name(pt);
	}
	അല്ലെങ്കിൽ
	{
	    s = functv.vval.v_string;
	    എങ്കിൽ (s == ശൂന്യ || *s == NUL)
	    {
		emsg(_(e_empty_function_name));
		ret = FAIL;
		പോകുക theend;
	    }
	}
    }
    അല്ലെങ്കിൽ
	s = (char_u *)"";

    CLEAR_FIELD(funcexe);
    funcexe.fe_firstline = curwin->w_cursor.lnum;
    funcexe.fe_lastline = curwin->w_cursor.lnum;
    funcexe.fe_evaluate = evaluate;
    funcexe.fe_partial = pt;
    എങ്കിൽ (pt != ശൂന്യ)
    {
	funcexe.fe_object = pt->pt_obj;
	എങ്കിൽ (funcexe.fe_object != ശൂന്യ)
	    ++funcexe.fe_object->obj_refcount;
    }
    funcexe.fe_selfdict = selfdict;
    funcexe.fe_basetv = basetv;
    എങ്കിൽ (evalarg != ശൂന്യ)
	funcexe.fe_cctx = evalarg->eval_cctx;
    ret = get_func_tv(s, -1, rettv, arg, evalarg, &funcexe);

theend:
    // Clear the funcref afterwards, so that deleting it while
    // evaluating the arguments is possible (see test55).
    എങ്കിൽ (evaluate)
	clear_tv(&functv);

    തിരികെ ret;
}

/*
 * Evaluate "->method()".
 * "*arg" points to "method".
 * Returns FAIL or OK. "*arg" is advanced to after the ')'.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
eval_lambda(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    പൂർണ്ണസംഖ്യ		verbose)	// give error messages
{
    പൂർണ്ണസംഖ്യ		evaluate = evalarg != ശൂന്യ
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    typval_T	base = *rettv;
    പൂർണ്ണസംഖ്യ		ret;

    rettv->v_type = VAR_UNKNOWN;

    എങ്കിൽ (**arg == '{')
    {
	// ->{lambda}()
	ret = get_lambda_tv(arg, rettv, FALSE, evalarg, ശൂന്യ);
    }
    അല്ലെങ്കിൽ
    {
	// ->(lambda)()
	++*arg;
	ret = eval1(arg, rettv, evalarg);
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	എങ്കിൽ (**arg != ')')
	{
	    emsg(_(e_missing_closing_paren));
	    തിരികെ FAIL;
	}
	എങ്കിൽ (rettv->v_type != VAR_STRING && rettv->v_type != VAR_FUNC
					       && rettv->v_type != VAR_PARTIAL)
	{
	    emsg(_(e_string_or_function_required_for_arrow_parens_വ്യഞ്ജകം));
	    തിരികെ FAIL;
	}
	++*arg;
    }
    എങ്കിൽ (ret != OK)
	തിരികെ FAIL;

    എങ്കിൽ (**arg != '(')
    {
	എങ്കിൽ (verbose)
	{
	    എങ്കിൽ (*skipwhite(*arg) == '(')
		emsg(_(e_no_white_space_allowed_before_parenthesis));
	    അല്ലെങ്കിൽ
		semsg(_(e_missing_parenthesis_str), "lambda");
	}
	clear_tv(rettv);
	ret = FAIL;
    }
    അല്ലെങ്കിൽ
	ret = call_func_rettv(arg, evalarg, rettv, evaluate, ശൂന്യ, &base);

    // Clear the funcref afterwards, so that deleting it while
    // evaluating the arguments is possible (see test55).
    എങ്കിൽ (evaluate)
	clear_tv(&base);

    തിരികെ ret;
}

/*
 * Evaluate "->method()".
 * "*arg" points to "method".
 * Returns FAIL or OK. "*arg" is advanced to after the ')'.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
eval_method(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    പൂർണ്ണസംഖ്യ		verbose)	// give error messages
{
    char_u	*name;
    long	len;
    char_u	*alias;
    char_u	*tofree = ശൂന്യ;
    typval_T	base = *rettv;
    പൂർണ്ണസംഖ്യ		ret = OK;
    പൂർണ്ണസംഖ്യ		evaluate = evalarg != ശൂന്യ
				      && (evalarg->eval_flags & EVAL_EVALUATE);

    rettv->v_type = VAR_UNKNOWN;

    name = *arg;
    len = get_name_len(arg, &alias, evaluate, evaluate);
    എങ്കിൽ (alias != ശൂന്യ)
	name = alias;

    എങ്കിൽ (len <= 0)
    {
	എങ്കിൽ (verbose)
	    emsg(_(e_missing_name_after_method));
	ret = FAIL;
    }
    അല്ലെങ്കിൽ
    {
	char_u *paren;

	// If there is no "(" immediately following, but there is further on,
	// it can be "import.Func()", "dict.Func()", "list[nr]", etc.
	// Does not handle anything where "(" is part of the expression.
	*arg = skipwhite(*arg);

	എങ്കിൽ (**arg != '(' && alias == ശൂന്യ
				    && (paren = vim_strchr(*arg, '(')) != ശൂന്യ)
	{
	    *arg = name;

	    // Truncate the name at the "(".  Avoid trying to get another line
	    // by making "getline" NULL.
	    *paren = NUL;
	    char_u	*(*getline)(പൂർണ്ണസംഖ്യ, ശൂന്യം *, പൂർണ്ണസംഖ്യ, getline_opt_T) = ശൂന്യ;
	    എങ്കിൽ (evalarg != ശൂന്യ)
	    {
		getline = evalarg->eval_getline;
		evalarg->eval_getline = ശൂന്യ;
	    }

	    char_u *deref = deref_function_name(arg, &tofree, evalarg, verbose);
	    എങ്കിൽ (deref == ശൂന്യ)
	    {
		*arg = name + len;
		ret = FAIL;
	    }
	    അല്ലെങ്കിൽ
	    {
		name = deref;
		len = (long)STRLEN(name);
	    }

	    *paren = '(';
	    എങ്കിൽ (getline != ശൂന്യ)
		evalarg->eval_getline = getline;
	}

	എങ്കിൽ (ret == OK)
	{
	    *arg = skipwhite(*arg);

	    എങ്കിൽ (**arg != '(')
	    {
		എങ്കിൽ (verbose)
		    semsg(_(e_missing_parenthesis_str), name);
		ret = FAIL;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (VIM_ISWHITE((*arg)[-1]))
	    {
		എങ്കിൽ (verbose)
		    emsg(_(e_no_white_space_allowed_before_parenthesis));
		ret = FAIL;
	    }
	    അല്ലെങ്കിൽ
		ret = eval_func(arg, evalarg, name, len, rettv,
					  evaluate ? EVAL_EVALUATE : 0, &base);
	}
    }

    // Clear the funcref afterwards, so that deleting it while
    // evaluating the arguments is possible (see test55).
    എങ്കിൽ (evaluate)
	clear_tv(&base);
    vim_free(tofree);

    എങ്കിൽ (alias != ശൂന്യ)
	vim_free(alias);

    തിരികെ ret;
}

/*
 * Evaluate an "[expr]" or "[expr:expr]" index.  Also "dict.key".
 * "*arg" points to the '[' or '.'.
 * Returns FAIL or OK. "*arg" is advanced to after the ']'.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
eval_index(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    പൂർണ്ണസംഖ്യ		verbose)	// give error messages
{
    പൂർണ്ണസംഖ്യ		evaluate = evalarg != ശൂന്യ
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    പൂർണ്ണസംഖ്യ		empty1 = FALSE, empty2 = FALSE;
    typval_T	var1, var2;
    പൂർണ്ണസംഖ്യ		range = FALSE;
    char_u	*താക്കോൽ = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		keylen = -1;
    പൂർണ്ണസംഖ്യ		vim9script = in_vim9script();

    എങ്കിൽ (check_can_index(rettv, evaluate, verbose) == FAIL)
	തിരികെ FAIL;

    init_tv(&var1);
    init_tv(&var2);
    എങ്കിൽ (**arg == '.')
    {
	/*
	 * dict.name
	 */
	താക്കോൽ = *arg + 1;
	വേണ്ടി (keylen = 0; eval_isdictc(താക്കോൽ[keylen]); ++keylen)
	    ;
	എങ്കിൽ (keylen == 0)
	    തിരികെ FAIL;
	എങ്കിൽ (vim9script && താക്കോൽ[keylen] == '<')
	{
	    // skip generic type arguments
	    char_u	*p = &താക്കോൽ[keylen];

	    എങ്കിൽ (skip_generic_func_type_args(&p) == FAIL)
		തിരികെ FAIL;
	    keylen = p - താക്കോൽ;
	}
	*arg = താക്കോൽ + keylen;
    }
    അല്ലെങ്കിൽ
    {
	/*
	 * something[idx]
	 *
	 * Get the (first) variable from inside the [].
	 */
	*arg = skipwhite_and_linebreak(*arg + 1, evalarg);
	എങ്കിൽ (**arg == ':')
	    empty1 = TRUE;
	അല്ലെങ്കിൽ എങ്കിൽ (eval1(arg, &var1, evalarg) == FAIL)	// recursive!
	    തിരികെ FAIL;
	അല്ലെങ്കിൽ എങ്കിൽ (vim9script && **arg == ':')
	{
	    semsg(_(e_white_space_required_before_and_after_str_at_str),
								    ":", *arg);
	    clear_tv(&var1);
	    തിരികെ FAIL;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (evaluate)
	{
	    പൂർണ്ണസംഖ്യ error = FALSE;

	    // allow for indexing with float
	    എങ്കിൽ (vim9script && rettv->v_type == VAR_DICT
						   && var1.v_type == VAR_FLOAT)
	    {
		var1.vval.v_string = typval_tostring(&var1, TRUE);
		var1.v_type = VAR_STRING;
	    }

	    എങ്കിൽ (vim9script && (rettv->v_type == VAR_LIST
						|| rettv->v_type == VAR_TUPLE))
		tv_get_സംഖ്യ_chk(&var1, &error);
	    അല്ലെങ്കിൽ
		error = tv_get_string_chk(&var1) == ശൂന്യ;
	    എങ്കിൽ (error)
	    {
		// not a number or string
		clear_tv(&var1);
		തിരികെ FAIL;
	    }
	}

	/*
	 * Get the second variable from inside the [:].
	 */
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	എങ്കിൽ (**arg == ':')
	{
	    range = TRUE;
	    ++*arg;
	    എങ്കിൽ (vim9script && !IS_WHITE_OR_NUL(**arg) && **arg != ']')
	    {
		semsg(_(e_white_space_required_before_and_after_str_at_str),
								":", *arg - 1);
		എങ്കിൽ (!empty1)
		    clear_tv(&var1);
		തിരികെ FAIL;
	    }
	    *arg = skipwhite_and_linebreak(*arg, evalarg);
	    എങ്കിൽ (**arg == ']')
		empty2 = TRUE;
	    അല്ലെങ്കിൽ എങ്കിൽ (eval1(arg, &var2, evalarg) == FAIL)	// recursive!
	    {
		എങ്കിൽ (!empty1)
		    clear_tv(&var1);
		തിരികെ FAIL;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (evaluate && tv_get_string_chk(&var2) == ശൂന്യ)
	    {
		// not a number or string
		എങ്കിൽ (!empty1)
		    clear_tv(&var1);
		clear_tv(&var2);
		തിരികെ FAIL;
	    }
	}

	// Check for the ']'.
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	എങ്കിൽ (**arg != ']')
	{
	    എങ്കിൽ (verbose)
		emsg(_(e_missing_closing_square_brace));
	    clear_tv(&var1);
	    എങ്കിൽ (range)
		clear_tv(&var2);
	    തിരികെ FAIL;
	}
	*arg = *arg + 1;	// skip over the ']'
    }

    എങ്കിൽ (evaluate)
    {
	പൂർണ്ണസംഖ്യ res = eval_index_inner(rettv, range,
		empty1 ? ശൂന്യ : &var1, empty2 ? ശൂന്യ : &var2, FALSE,
		താക്കോൽ, keylen, verbose);

	എങ്കിൽ (!empty1)
	    clear_tv(&var1);
	എങ്കിൽ (range)
	    clear_tv(&var2);
	തിരികെ res;
    }
    തിരികെ OK;
}

/*
 * Check if "rettv" can have an [index] or [sli:ce]
 */
    പൂർണ്ണസംഖ്യ
check_can_index(typval_T *rettv, പൂർണ്ണസംഖ്യ evaluate, പൂർണ്ണസംഖ്യ verbose)
{
    തിരഞ്ഞെടുപ്പ് (rettv->v_type)
    {
	അവസ്ഥ VAR_FUNC:
	അവസ്ഥ VAR_PARTIAL:
	    എങ്കിൽ (verbose)
		emsg(_(e_cannot_index_a_funcref));
	    തിരികെ FAIL;
	അവസ്ഥ VAR_FLOAT:
	    എങ്കിൽ (verbose)
		emsg(_(e_using_float_as_string));
	    തിരികെ FAIL;
	അവസ്ഥ VAR_BOOL:
	അവസ്ഥ VAR_SPECIAL:
	അവസ്ഥ VAR_JOB:
	അവസ്ഥ VAR_CHANNEL:
	അവസ്ഥ VAR_INSTR:
	അവസ്ഥ VAR_OBJECT:
	    എങ്കിൽ (verbose)
		emsg(_(e_cannot_index_special_variable));
	    തിരികെ FAIL;
	അവസ്ഥ VAR_CLASS:
	അവസ്ഥ VAR_TYPEALIAS:
	    എങ്കിൽ (verbose)
		check_typval_is_മൂല്യം(rettv);
	    തിരികെ FAIL;
	അവസ്ഥ VAR_UNKNOWN:
	അവസ്ഥ VAR_ANY:
	അവസ്ഥ VAR_VOID:
	    എങ്കിൽ (evaluate)
	    {
		emsg(_(e_cannot_index_special_variable));
		തിരികെ FAIL;
	    }
	    // FALLTHROUGH

	അവസ്ഥ VAR_STRING:
	അവസ്ഥ VAR_LIST:
	അവസ്ഥ VAR_TUPLE:
	അവസ്ഥ VAR_DICT:
	അവസ്ഥ VAR_BLOB:
	    നിർത്തുക;
	അവസ്ഥ VAR_സംഖ്യ:
	    എങ്കിൽ (in_vim9script())
		emsg(_(e_cannot_index_സംഖ്യ));
	    നിർത്തുക;
    }
    തിരികെ OK;
}

/*
 * Apply index or range to "rettv".
 * "var1" is the first index, NULL for [:expr].
 * "var2" is the second index, NULL for [expr] and [expr: ]
 * "exclusive" is TRUE for slice(): second index is exclusive, use character
 * index for string.
 * Alternatively, "key" is not NULL, then key[keylen] is the dict index.
 */
    പൂർണ്ണസംഖ്യ
eval_index_inner(
	typval_T    *rettv,
	പൂർണ്ണസംഖ്യ	    is_range,
	typval_T    *var1,
	typval_T    *var2,
	പൂർണ്ണസംഖ്യ	    exclusive,
	char_u	    *താക്കോൽ,
	പൂർണ്ണസംഖ്യ	    keylen,
	പൂർണ്ണസംഖ്യ	    verbose)
{
    varnumber_T	    n1, n2 = 0;
    long	    len;

    n1 = 0;
    എങ്കിൽ (var1 != ശൂന്യ && rettv->v_type != VAR_DICT)
	n1 = tv_get_സംഖ്യ(var1);

    എങ്കിൽ (is_range)
    {
	എങ്കിൽ (rettv->v_type == VAR_DICT)
	{
	    എങ്കിൽ (verbose)
		emsg(_(e_cannot_slice_dictionary));
	    തിരികെ FAIL;
	}
	എങ്കിൽ (var2 != ശൂന്യ)
	    n2 = tv_get_സംഖ്യ(var2);
	അല്ലെങ്കിൽ
	    n2 = VARNUM_MAX;
    }

    തിരഞ്ഞെടുപ്പ് (rettv->v_type)
    {
	അവസ്ഥ VAR_UNKNOWN:
	അവസ്ഥ VAR_ANY:
	അവസ്ഥ VAR_VOID:
	അവസ്ഥ VAR_FUNC:
	അവസ്ഥ VAR_PARTIAL:
	അവസ്ഥ VAR_FLOAT:
	അവസ്ഥ VAR_BOOL:
	അവസ്ഥ VAR_SPECIAL:
	അവസ്ഥ VAR_JOB:
	അവസ്ഥ VAR_CHANNEL:
	അവസ്ഥ VAR_INSTR:
	അവസ്ഥ VAR_CLASS:
	അവസ്ഥ VAR_OBJECT:
	അവസ്ഥ VAR_TYPEALIAS:
	    നിർത്തുക; // not evaluating, skipping over subscript

	അവസ്ഥ VAR_സംഖ്യ:
	അവസ്ഥ VAR_STRING:
	    {
		char_u	*s = tv_get_string(rettv);

		len = (long)STRLEN(s);
		എങ്കിൽ (in_vim9script() || exclusive)
		{
		    എങ്കിൽ (is_range)
			s = string_slice(s, n1, n2, exclusive);
		    അല്ലെങ്കിൽ
			s = char_from_string(s, n1);
		}
		അല്ലെങ്കിൽ എങ്കിൽ (is_range)
		{
		    // The resulting variable is a substring.  If the indexes
		    // are out of range the result is empty.
		    എങ്കിൽ (n1 < 0)
		    {
			n1 = len + n1;
			എങ്കിൽ (n1 < 0)
			    n1 = 0;
		    }
		    എങ്കിൽ (n2 < 0)
			n2 = len + n2;
		    അല്ലെങ്കിൽ എങ്കിൽ (n2 >= len)
			n2 = len;
		    എങ്കിൽ (n1 >= len || n2 < 0 || n1 > n2)
			s = ശൂന്യ;
		    അല്ലെങ്കിൽ
			s = vim_strnsave(s + n1, n2 - n1 + 1);
		}
		അല്ലെങ്കിൽ
		{
		    // The resulting variable is a string of a single
		    // character.  If the index is too big or negative the
		    // result is empty.
		    എങ്കിൽ (n1 >= len || n1 < 0)
			s = ശൂന്യ;
		    അല്ലെങ്കിൽ
			s = vim_strnsave(s + n1, 1);
		}
		clear_tv(rettv);
		rettv->v_type = VAR_STRING;
		rettv->vval.v_string = s;
	    }
	    നിർത്തുക;

	അവസ്ഥ VAR_BLOB:
	    blob_slice_or_index(rettv->vval.v_blob, is_range, n1, n2,
							     exclusive, rettv);
	    നിർത്തുക;

	അവസ്ഥ VAR_LIST:
	    എങ്കിൽ (var1 == ശൂന്യ)
		n1 = 0;
	    എങ്കിൽ (var2 == ശൂന്യ)
		n2 = VARNUM_MAX;
	    എങ്കിൽ (list_slice_or_index(rettv->vval.v_list,
			  is_range, n1, n2, exclusive, rettv, verbose) == FAIL)
		തിരികെ FAIL;
	    നിർത്തുക;

	അവസ്ഥ VAR_TUPLE:
	    എങ്കിൽ (var1 == ശൂന്യ)
		n1 = 0;
	    എങ്കിൽ (var2 == ശൂന്യ)
		n2 = VARNUM_MAX;
	    എങ്കിൽ (tuple_slice_or_index(rettv->vval.v_tuple,
			  is_range, n1, n2, exclusive, rettv, verbose) == FAIL)
		തിരികെ FAIL;
	    നിർത്തുക;

	അവസ്ഥ VAR_DICT:
	    {
		dictitem_T	*item;
		typval_T	tmp;

		എങ്കിൽ (താക്കോൽ == ശൂന്യ)
		{
		    താക്കോൽ = tv_get_string_chk(var1);
		    എങ്കിൽ (താക്കോൽ == ശൂന്യ)
			തിരികെ FAIL;
		}

		item = dict_find(rettv->vval.v_dict, താക്കോൽ, keylen);

		എങ്കിൽ (item == ശൂന്യ)
		{
		    എങ്കിൽ (verbose)
		    {
			എങ്കിൽ (keylen > 0)
			    താക്കോൽ[keylen] = NUL;
			semsg(_(e_താക്കോൽ_not_present_in_dictionary_str), താക്കോൽ);
		    }
		    തിരികെ FAIL;
		}

		copy_tv(&item->di_tv, &tmp);
		clear_tv(rettv);
		*rettv = tmp;
	    }
	    നിർത്തുക;
    }
    തിരികെ OK;
}

/*
 * Return the function name of partial "pt".
 */
    char_u *
partial_name(partial_T *pt)
{
    എങ്കിൽ (pt != ശൂന്യ)
    {
	എങ്കിൽ (pt->pt_name != ശൂന്യ)
	    തിരികെ pt->pt_name;
	എങ്കിൽ (pt->pt_func != ശൂന്യ)
	    തിരികെ pt->pt_func->uf_name;
    }
    തിരികെ (char_u *)"";
}

    സ്ഥിതം ശൂന്യം
partial_free(partial_T *pt)
{
    പൂർണ്ണസംഖ്യ i;

    വേണ്ടി (i = 0; i < pt->pt_argc; ++i)
	clear_tv(&pt->pt_argv[i]);
    vim_free(pt->pt_argv);
    dict_unref(pt->pt_dict);
    എങ്കിൽ (pt->pt_name != ശൂന്യ)
    {
	func_unref(pt->pt_name);
	vim_free(pt->pt_name);
    }
    അല്ലെങ്കിൽ
	func_ptr_unref(pt->pt_func);
    എങ്കിൽ (pt->pt_obj != ശൂന്യ)
	object_unref(pt->pt_obj);

    // "out_up" is no longer used, decrement refcount on partial that owns it.
    partial_unref(pt->pt_outer.out_up_partial);

    // Using pt_outer from another partial.
    partial_unref(pt->pt_outer_partial);

    // Decrease the reference count for the context of a closure.  If down
    // to the minimum it may be time to free it.
    എങ്കിൽ (pt->pt_funcstack != ശൂന്യ)
    {
	--pt->pt_funcstack->fs_refcount;
	funcstack_check_refcount(pt->pt_funcstack);
    }
    // Similarly for loop variables.
    വേണ്ടി (i = 0; i < MAX_LOOP_DEPTH; ++i)
	എങ്കിൽ (pt->pt_loopvars[i] != ശൂന്യ)
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
    ശൂന്യം
partial_unref(partial_T *pt)
{
    എങ്കിൽ (pt == ശൂന്യ)
	തിരികെ;

    പൂർണ്ണസംഖ്യ	done = FALSE;

    എങ്കിൽ (--pt->pt_refcount <= 0)
	partial_free(pt);

    // If the reference count goes down to one, the funcstack may be the
    // only reference and can be freed if no other partials reference it.
    അല്ലെങ്കിൽ എങ്കിൽ (pt->pt_refcount == 1)
    {
	// careful: if the funcstack is freed it may contain this partial
	// and it gets freed as well
	എങ്കിൽ (pt->pt_funcstack != ശൂന്യ)
	    done = funcstack_check_refcount(pt->pt_funcstack);

	എങ്കിൽ (!done)
	{
	    പൂർണ്ണസംഖ്യ	depth;

	    വേണ്ടി (depth = 0; depth < MAX_LOOP_DEPTH; ++depth)
		എങ്കിൽ (pt->pt_loopvars[depth] != ശൂന്യ
			&& loopvars_check_refcount(pt->pt_loopvars[depth]))
		    നിർത്തുക;
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
    സ്ഥിതം char_u *
string_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    പൂർണ്ണസംഖ്യ		echo_style,
    പൂർണ്ണസംഖ്യ		composite_val)
{
    char_u	*r = ശൂന്യ;

    എങ്കിൽ (echo_style && !composite_val)
    {
	*tofree = ശൂന്യ;
	r = tv->vval.v_string;
	എങ്കിൽ (r == ശൂന്യ)
	    r = (char_u *)"";
    }
    അല്ലെങ്കിൽ
    {
	*tofree = string_quote(tv->vval.v_string, FALSE);
	r = *tofree;
    }

    തിരികെ r;
}

/*
 * Return a textual representation of a function in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "echo_style" is FALSE, put quotes around the function name as
 * "function()", otherwise does not put quotes around function name.
 * May return NULL.
 */
    സ്ഥിതം char_u *
func_tv2string(typval_T *tv, char_u **tofree, പൂർണ്ണസംഖ്യ echo_style)
{
    char_u	*r = ശൂന്യ;
    char_u	buf[MAX_FUNC_NAME_LEN];

    എങ്കിൽ (echo_style)
    {
	*tofree = ശൂന്യ;

	എങ്കിൽ (tv->vval.v_string == ശൂന്യ)
	    r = (char_u *)"function()";
	അല്ലെങ്കിൽ
	{
	    r = make_ufunc_name_readable(tv->vval.v_string, buf,
							MAX_FUNC_NAME_LEN);
	    എങ്കിൽ (r == buf)
		r = *tofree = vim_strsave(buf);
	}
    }
    അല്ലെങ്കിൽ
    {
	char_u *s = ശൂന്യ;

	എങ്കിൽ (tv->vval.v_string != ശൂന്യ)
	    s = make_ufunc_name_readable(tv->vval.v_string, buf,
							MAX_FUNC_NAME_LEN);

	r = *tofree = string_quote(s, TRUE);
    }

    തിരികെ r;
}

/*
 * Return a textual representation of the object method in "tv", a VAR_PARTIAL.
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "echo_style" is FALSE, put quotes around the function name as
 * "function()", otherwise does not put quotes around function name.
 * May return NULL.
 */
    സ്ഥിതം char_u *
method_tv2string(typval_T *tv, char_u **tofree, പൂർണ്ണസംഖ്യ echo_style)
{
    char_u	buf[MAX_FUNC_NAME_LEN];
    partial_T	*pt = tv->vval.v_partial;

    size_t len = vim_snprintf((അക്ഷരം *)buf, വലുപ്പം(buf), "<SNR>%d_%s.%s",
			   pt->pt_func->uf_script_ctx.sc_sid,
			   pt->pt_func->uf_class->class_name.string,
			   pt->pt_func->uf_name);
    എങ്കിൽ (len >= വലുപ്പം(buf))
    {
	എങ്കിൽ (echo_style)
	{
	    *tofree = ശൂന്യ;
	    തിരികെ (char_u *)"function()";
	}
	അല്ലെങ്കിൽ
	    തിരികെ *tofree = string_quote((char_u*)"", TRUE);
    }

    തിരികെ *tofree = echo_style ? vim_strsave(buf) : string_quote(buf, TRUE);
}

/*
 * Return a textual representation of a partial in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * "numbuf" is used for a number.  May return NULL.
 */
    സ്ഥിതം char_u *
partial_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    char_u	*numbuf,
    പൂർണ്ണസംഖ്യ		copyID)
{
    char_u	*r = ശൂന്യ;
    partial_T	*pt;
    char_u	*fname;
    garray_T	ga;
    പൂർണ്ണസംഖ്യ		i;
    char_u	*tf;

    pt = tv->vval.v_partial;
    fname = string_quote(pt == ശൂന്യ ? ശൂന്യ : partial_name(pt), FALSE);

    ga_init2(&ga, 1, 100);
    GA_CONCAT_LITERAL(&ga, "function(");
    എങ്കിൽ (fname != ശൂന്യ)
    {
	// When using uf_name prepend "g:" for a global function.
	എങ്കിൽ (pt != ശൂന്യ && pt->pt_name == ശൂന്യ && fname[0] == '\''
						&& vim_isupper(fname[1]))
	{
	    GA_CONCAT_LITERAL(&ga, "'g:");
	    ga_concat(&ga, fname + 1);
	}
	അല്ലെങ്കിൽ
	    ga_concat(&ga, fname);
	vim_free(fname);
    }
    എങ്കിൽ (pt != ശൂന്യ && pt->pt_argc > 0)
    {
	GA_CONCAT_LITERAL(&ga, ", [");
	വേണ്ടി (i = 0; i < pt->pt_argc; ++i)
	{
	    എങ്കിൽ (i > 0)
		GA_CONCAT_LITERAL(&ga, ", ");
	    ga_concat(&ga, tv2string(&pt->pt_argv[i], &tf, numbuf, copyID));
	    vim_free(tf);
	}
	GA_CONCAT_LITERAL(&ga, "]");
    }
    എങ്കിൽ (pt != ശൂന്യ && pt->pt_dict != ശൂന്യ)
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

    തിരികെ r;
}

/*
 * Return a textual representation of a List in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "copyID" is not zero replace recursive lists with "...".  When
 * "restore_copyID" is FALSE, repeated items in lists are replaced with "...".
 * May return NULL.
 */
    സ്ഥിതം char_u *
list_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    പൂർണ്ണസംഖ്യ		copyID,
    പൂർണ്ണസംഖ്യ		restore_copyID)
{
    char_u	*r = ശൂന്യ;

    എങ്കിൽ (tv->vval.v_list == ശൂന്യ)
    {
	// NULL list is equivalent to empty list.
	*tofree = ശൂന്യ;
	r = (char_u *)"[]";
    }
    അല്ലെങ്കിൽ എങ്കിൽ (copyID != 0 && tv->vval.v_list->lv_copyID == copyID
	    && tv->vval.v_list->lv_len > 0)
    {
	*tofree = ശൂന്യ;
	r = (char_u *)"[...]";
    }
    അല്ലെങ്കിൽ
    {
	പൂർണ്ണസംഖ്യ old_copyID;
	എങ്കിൽ (restore_copyID)
	    old_copyID = tv->vval.v_list->lv_copyID;

	tv->vval.v_list->lv_copyID = copyID;
	*tofree = list2string(tv, copyID, restore_copyID);
	എങ്കിൽ (restore_copyID)
	    tv->vval.v_list->lv_copyID = old_copyID;
	r = *tofree;
    }

    തിരികെ r;
}

/*
 * Return a textual representation of a Tuple in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "copyID" is not zero replace recursive lists with "...".  When
 * "restore_copyID" is FALSE, repeated items in tuples are replaced with "...".
 * May return NULL.
 */
    സ്ഥിതം char_u *
tuple_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    പൂർണ്ണസംഖ്യ		copyID,
    പൂർണ്ണസംഖ്യ		restore_copyID)
{
    tuple_T	*tuple = tv->vval.v_tuple;
    char_u	*r = ശൂന്യ;

    എങ്കിൽ (tuple == ശൂന്യ)
    {
	// NULL tuple is equivalent to an empty tuple.
	*tofree = ശൂന്യ;
	r = (char_u *)"()";
    }
    അല്ലെങ്കിൽ എങ്കിൽ (copyID != 0 && tuple->tv_copyID == copyID
					&& tuple->tv_ഇനങ്ങൾ.ga_len > 0)
    {
	*tofree = ശൂന്യ;
	r = (char_u *)"(...)";
    }
    അല്ലെങ്കിൽ
    {
	പൂർണ്ണസംഖ്യ old_copyID;
	എങ്കിൽ (restore_copyID)
	    old_copyID = tuple->tv_copyID;

	tuple->tv_copyID = copyID;
	*tofree = tuple2string(tv, copyID, restore_copyID);
	എങ്കിൽ (restore_copyID)
	    tuple->tv_copyID = old_copyID;
	r = *tofree;
    }

    തിരികെ r;
}

/*
 * Return a textual representation of a Dict in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "copyID" is not zero replace recursive dicts with "...".
 * When "restore_copyID" is FALSE, repeated items in the dictionary are
 * replaced with "...".  May return NULL.
 */
    സ്ഥിതം char_u *
dict_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    പൂർണ്ണസംഖ്യ		copyID,
    പൂർണ്ണസംഖ്യ		restore_copyID)
{
    char_u	*r = ശൂന്യ;

    എങ്കിൽ (tv->vval.v_dict == ശൂന്യ)
    {
	// NULL dict is equivalent to empty dict.
	*tofree = ശൂന്യ;
	r = (char_u *)"{}";
    }
    അല്ലെങ്കിൽ എങ്കിൽ (copyID != 0 && tv->vval.v_dict->dv_copyID == copyID
	    && tv->vval.v_dict->dv_hashtab.ht_used != 0)
    {
	*tofree = ശൂന്യ;
	r = (char_u *)"{...}";
    }
    അല്ലെങ്കിൽ
    {
	പൂർണ്ണസംഖ്യ old_copyID;
	എങ്കിൽ (restore_copyID)
	    old_copyID = tv->vval.v_dict->dv_copyID;

	tv->vval.v_dict->dv_copyID = copyID;
	*tofree = dict2string(tv, copyID, restore_copyID);
	എങ്കിൽ (restore_copyID)
	    tv->vval.v_dict->dv_copyID = old_copyID;
	r = *tofree;
    }

    തിരികെ r;
}

/*
 * Return a textual representation of a job or a channel in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * "numbuf" is used for a number.
 * When "composite_val" is FALSE, put quotes around strings as "string()",
 * otherwise does not put quotes around strings.
 * May return NULL.
 */
    സ്ഥിതം char_u *
jobchan_tv2string(
    typval_T	*tv UNUSED,
    char_u	**tofree UNUSED,
    char_u	*numbuf UNUSED,
    പൂർണ്ണസംഖ്യ		composite_val UNUSED)
{
    char_u	*r = ശൂന്യ;

#ifdef FEAT_JOB_CHANNEL
    *tofree = ശൂന്യ;

    എങ്കിൽ (tv->v_type == VAR_JOB)
	r = job_to_string_buf(tv, numbuf);
    അല്ലെങ്കിൽ
	r = channel_to_string_buf(tv, numbuf);

    എങ്കിൽ (composite_val)
    {
	*tofree = string_quote(r, FALSE);
	r = *tofree;
    }
#endif

    തിരികെ r;
}

/*
 * Return a textual representation of a class in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * May return NULL.
 */
    സ്ഥിതം char_u *
class_tv2string(typval_T *tv, char_u **tofree)
{
    char_u	*r = ശൂന്യ;
    size_t	rsize;
    class_T	*cl = tv->vval.v_class;
    string_T	class_name = {(char_u *)"[unknown]", 9};
    string_T	s = {(char_u *)"class", 5};

    എങ്കിൽ (cl != ശൂന്യ)
    {
	class_name.string = cl->class_name.string;
	class_name.length = cl->class_name.length;
	എങ്കിൽ (IS_INTERFACE(cl))
	    STR_LITERAL_SET(s, "interface");
	അല്ലെങ്കിൽ എങ്കിൽ (IS_ENUM(cl))
	    STR_LITERAL_SET(s, "enum");
    }

    rsize = s.length + 1 + class_name.length + 1;
    r = *tofree = alloc(rsize);
    എങ്കിൽ (r != ശൂന്യ)
	vim_snprintf((അക്ഷരം *)r, rsize, "%s %s", s.string, (അക്ഷരം *)class_name.string);

    തിരികെ r;
}

/*
 * Return a textual representation of an Object in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "copyID" is not zero replace recursive object with "...".
 * When "restore_copyID" is FALSE, repeated items in the object are
 * replaced with "...".  May return NULL.
 */
    സ്ഥിതം char_u *
object_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    പൂർണ്ണസംഖ്യ		copyID,
    പൂർണ്ണസംഖ്യ		restore_copyID,
    char_u	*numbuf,
    പൂർണ്ണസംഖ്യ		echo_style,
    പൂർണ്ണസംഖ്യ		composite_val)
{
    char_u	*r = ശൂന്യ;

    object_T	*obj = tv->vval.v_object;
    എങ്കിൽ (obj == ശൂന്യ || obj->obj_class == ശൂന്യ)
    {
	*tofree = ശൂന്യ;
	r = (char_u *)"object of [unknown]";
    }
    അല്ലെങ്കിൽ എങ്കിൽ (copyID != 0 && obj->obj_copyID == copyID
	    && obj->obj_class->class_obj_member_എണ്ണം != 0)
    {
	size_t n = 25 + obj->obj_class->class_name.length;
	r = alloc(n);
	എങ്കിൽ (r != ശൂന്യ)
	    (ശൂന്യം)vim_snprintf((അക്ഷരം *)r, n, "object of %s {...}",
						obj->obj_class->class_name.string);
	*tofree = r;
    }
    അല്ലെങ്കിൽ
    {
	പൂർണ്ണസംഖ്യ old_copyID;
	എങ്കിൽ (restore_copyID)
	    old_copyID = obj->obj_copyID;

	obj->obj_copyID = copyID;
	*tofree = object2string(obj, numbuf, copyID, echo_style,
				restore_copyID, composite_val);
	എങ്കിൽ (restore_copyID)
	    obj->obj_copyID = old_copyID;
	r = *tofree;
    }

    തിരികെ r;
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
    പൂർണ്ണസംഖ്യ		copyID,
    പൂർണ്ണസംഖ്യ		echo_style,
    പൂർണ്ണസംഖ്യ		restore_copyID,
    പൂർണ്ണസംഖ്യ		composite_val)
{
    സ്ഥിതം പൂർണ്ണസംഖ്യ	recurse = 0;
    char_u	*r = ശൂന്യ;

    എങ്കിൽ (recurse >= DICT_MAXNEST)
    {
	എങ്കിൽ (!did_echo_string_emsg)
	{
	    // Only give this message once for a recursive call to avoid
	    // flooding the user with errors.  And stop iterating over lists
	    // and dicts and objects.
	    did_echo_string_emsg = TRUE;
	    emsg(_(e_variable_nested_too_deep_for_displaying));
	}
	*tofree = ശൂന്യ;
	തിരികെ (char_u *)"{E724}";
    }
    ++recurse;

    തിരഞ്ഞെടുപ്പ് (tv->v_type)
    {
	അവസ്ഥ VAR_STRING:
	    r = string_tv2string(tv, tofree, echo_style, composite_val);
	    നിർത്തുക;

	അവസ്ഥ VAR_FUNC:
	    r = func_tv2string(tv, tofree, echo_style);
	    നിർത്തുക;

	അവസ്ഥ VAR_PARTIAL:
	    എങ്കിൽ (tv->vval.v_partial == ശൂന്യ
		    || tv->vval.v_partial->pt_obj == ശൂന്യ)
		r = partial_tv2string(tv, tofree, numbuf, copyID);
	    അല്ലെങ്കിൽ
		r = method_tv2string(tv, tofree, echo_style);
	    നിർത്തുക;

	അവസ്ഥ VAR_BLOB:
	    r = blob2string(tv->vval.v_blob, tofree, numbuf);
	    നിർത്തുക;

	അവസ്ഥ VAR_LIST:
	    r = list_tv2string(tv, tofree, copyID, restore_copyID);
	    നിർത്തുക;

	അവസ്ഥ VAR_TUPLE:
	    r = tuple_tv2string(tv, tofree, copyID, restore_copyID);
	    നിർത്തുക;

	അവസ്ഥ VAR_DICT:
	    r = dict_tv2string(tv, tofree, copyID, restore_copyID);
	    നിർത്തുക;

	അവസ്ഥ VAR_സംഖ്യ:
	അവസ്ഥ VAR_UNKNOWN:
	അവസ്ഥ VAR_ANY:
	അവസ്ഥ VAR_VOID:
	    *tofree = ശൂന്യ;
	    r = tv_get_string_buf(tv, numbuf);
	    നിർത്തുക;

	അവസ്ഥ VAR_JOB:
	അവസ്ഥ VAR_CHANNEL:
	    r = jobchan_tv2string(tv, tofree, numbuf, composite_val);
	    നിർത്തുക;

	അവസ്ഥ VAR_INSTR:
	    *tofree = ശൂന്യ;
	    r = (char_u *)"instructions";
	    നിർത്തുക;

	അവസ്ഥ VAR_CLASS:
	    r = class_tv2string(tv, tofree);
	    നിർത്തുക;

	അവസ്ഥ VAR_OBJECT:
	    r = object_tv2string(tv, tofree, copyID, restore_copyID,
					 numbuf, echo_style, composite_val);
	    നിർത്തുക;

	അവസ്ഥ VAR_FLOAT:
	    *tofree = ശൂന്യ;
	    vim_snprintf((അക്ഷരം *)numbuf, NUMBUFLEN, "%g", tv->vval.v_float);
	    r = numbuf;
	    നിർത്തുക;

	അവസ്ഥ VAR_BOOL:
	അവസ്ഥ VAR_SPECIAL:
	    *tofree = ശൂന്യ;
	    r = (char_u *)get_var_special_name(tv->vval.v_സംഖ്യ);
	    നിർത്തുക;

	അവസ്ഥ VAR_TYPEALIAS:
	    *tofree = vim_strsave(tv->vval.v_typealias->ta_name);
	    r = *tofree;
	    എങ്കിൽ (r == ശൂന്യ)
		r = (char_u *)"";
	    നിർത്തുക;
    }

    എങ്കിൽ (--recurse == 0)
	did_echo_string_emsg = FALSE;
    തിരികെ r;
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
    പൂർണ്ണസംഖ്യ		copyID)
{
    തിരികെ echo_string_core(tv, tofree, numbuf, copyID, TRUE, FALSE, FALSE);
}

/*
 * Convert the specified byte index of line 'lnum' in buffer 'buf' to a
 * character index.  Works only for loaded buffers. Returns -1 on failure.
 * The index of the first byte and the first character is zero.
 */
    പൂർണ്ണസംഖ്യ
buf_byteidx_to_charidx(buf_T *buf, പൂർണ്ണസംഖ്യ lnum, പൂർണ്ണസംഖ്യ byteidx)
{
    char_u	*str;
    char_u	*t;
    പൂർണ്ണസംഖ്യ		എണ്ണം;

    എങ്കിൽ (buf == ശൂന്യ || buf->b_ml.ml_mfp == ശൂന്യ)
	തിരികെ -1;

    എങ്കിൽ (lnum > buf->b_ml.ml_വരി_എണ്ണം)
	lnum = buf->b_ml.ml_വരി_എണ്ണം;

    str = ml_get_buf(buf, lnum, FALSE);
    എങ്കിൽ (str == ശൂന്യ)
	തിരികെ -1;

    എങ്കിൽ (*str == NUL)
	തിരികെ 0;

    // count the number of characters
    t = str;
    വേണ്ടി (എണ്ണം = 0; *t != NUL && t <= str + byteidx; എണ്ണം++)
	t += mb_ptr2len(t);

    // In insert mode, when the cursor is at the end of a non-empty line,
    // byteidx points to the NUL character immediately past the end of the
    // string. In this case, add one to the character count.
    എങ്കിൽ (*t == NUL && byteidx != 0 && t == str + byteidx)
	എണ്ണം++;

    തിരികെ എണ്ണം - 1;
}

/*
 * Convert the specified character index of line 'lnum' in buffer 'buf' to a
 * byte index.  Works only for loaded buffers. Returns -1 on failure.
 * The index of the first byte and the first character is zero.
 */
    പൂർണ്ണസംഖ്യ
buf_charidx_to_byteidx(buf_T *buf, പൂർണ്ണസംഖ്യ lnum, പൂർണ്ണസംഖ്യ charidx)
{
    char_u	*str;
    char_u	*t;

    എങ്കിൽ (buf == ശൂന്യ || buf->b_ml.ml_mfp == ശൂന്യ)
	തിരികെ -1;

    എങ്കിൽ (lnum > buf->b_ml.ml_വരി_എണ്ണം)
	lnum = buf->b_ml.ml_വരി_എണ്ണം;

    str = ml_get_buf(buf, lnum, FALSE);
    എങ്കിൽ (str == ശൂന്യ)
	തിരികെ -1;

    // Convert the character offset to a byte offset
    t = str;
    ആകുമ്പോൾ (*t != NUL && --charidx > 0)
	t += mb_ptr2len(t);

    തിരികെ t - str;
}

/*
 * Translate a String variable into a position.
 * Returns NULL when there is an error.
 */
    pos_T *
var2fpos(
    typval_T	*varp,
    പൂർണ്ണസംഖ്യ		dollar_lnum,	// TRUE when $ is last line
    പൂർണ്ണസംഖ്യ		*fnum,		// set to fnum for '0, 'A, etc.
    പൂർണ്ണസംഖ്യ		charcol)	// return character column
{
    char_u		*name;
    സ്ഥിതം pos_T	pos;
    pos_T		*pp;
    പൂർണ്ണസംഖ്യ			error = FALSE;

    // Argument can be [lnum, col, coladd].
    എങ്കിൽ (varp->v_type == VAR_LIST)
    {
	list_T		*l;
	പൂർണ്ണസംഖ്യ		len;
	listitem_T	*li;

	l = varp->vval.v_list;
	എങ്കിൽ (l == ശൂന്യ)
	    തിരികെ ശൂന്യ;

	// Get the line number
	pos.lnum = list_find_nr(l, 0L, &error);
	എങ്കിൽ (error || pos.lnum <= 0 || pos.lnum > curbuf->b_ml.ml_വരി_എണ്ണം)
	    തിരികെ ശൂന്യ;	// invalid line number
	എങ്കിൽ (charcol)
	    len = (long)mb_charlen(ml_get(pos.lnum));
	അല്ലെങ്കിൽ
	    len = (long)ml_get_len(pos.lnum);

	// Get the column number
	// We accept "$" for the column number: last column.
	li = list_find(l, 1L);
	എങ്കിൽ (li != ശൂന്യ && li->li_tv.v_type == VAR_STRING
		&& li->li_tv.vval.v_string != ശൂന്യ
		&& STRCMP(li->li_tv.vval.v_string, "$") == 0)
	{
	    pos.col = len + 1;
	}
	അല്ലെങ്കിൽ
	{
	    pos.col = list_find_nr(l, 1L, &error);
	    എങ്കിൽ (error)
		തിരികെ ശൂന്യ;
	}

	// Accept a position up to the NUL after the line.
	എങ്കിൽ (pos.col == 0 || (പൂർണ്ണസംഖ്യ)pos.col > len + 1)
	    തിരികെ ശൂന്യ;	// invalid column number
	--pos.col;

	// Get the virtual offset.  Defaults to zero.
	pos.coladd = list_find_nr(l, 2L, &error);
	എങ്കിൽ (error)
	    pos.coladd = 0;

	തിരികെ &pos;
    }

    എങ്കിൽ (in_vim9script() && check_for_string_arg(varp, 0) == FAIL)
	തിരികെ ശൂന്യ;

    name = tv_get_string_chk(varp);
    എങ്കിൽ (name == ശൂന്യ)
	തിരികെ ശൂന്യ;

    error = TRUE;
    pos.lnum = 0;
    എങ്കിൽ (name[0] == '.')
    {
	എങ്കിൽ (!in_vim9script() || name[1] == NUL)
	{
	    error = FALSE;
	    // cursor
	    pos = curwin->w_cursor;
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (name[0] == 'v' && name[1] == NUL)
    {
	// Visual start
	എങ്കിൽ (VIsual_active)
	    pos = VIsual;
	അല്ലെങ്കിൽ
	    pos = curwin->w_cursor;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (name[0] == '\'')
    {
	എങ്കിൽ (!in_vim9script() || (name[1] != NUL && name[2] == NUL))
	{
	    error = FALSE;
	    // mark
	    pp = getmark_buf_fnum(curbuf, name[1], FALSE, fnum);
	    എങ്കിൽ (pp == ശൂന്യ || pp == (pos_T *)-1 || pp->lnum <= 0)
		തിരികെ ശൂന്യ;
	    pos = *pp;
	}
    }
    എങ്കിൽ (pos.lnum != 0)
    {
	എങ്കിൽ (charcol)
	    pos.col = buf_byteidx_to_charidx(curbuf, pos.lnum, pos.col);
	തിരികെ &pos;
    }

    pos.coladd = 0;

    എങ്കിൽ (name[0] == 'w' && dollar_lnum)
    {
	// the "w_valid" flags are not reset when moving the cursor, but they
	// do matter for update_topline() and validate_botline().
	check_cursor_moved(curwin);

	pos.col = 0;
	എങ്കിൽ (name[1] == '0')		// "w0": first visible line
	{
#ifdef FEAT_TERMINAL
	    എങ്കിൽ (bt_terminal(curwin->w_buffer)
				    && curwin->w_buffer->b_term != ശൂന്യ
				    && !term_in_normal_mode(curwin->w_buffer))
		may_move_terminal_to_buffer(curwin->w_buffer->b_term, TRUE);
#endif
	    update_topline();
	    // In silent Ex mode topline is zero, but that's not a valid line
	    // number; use one instead.
	    pos.lnum = curwin->w_topline > 0 ? curwin->w_topline : 1;
	    തിരികെ &pos;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (name[1] == '$')	// "w$": last visible line
	{
#ifdef FEAT_TERMINAL
	    എങ്കിൽ (bt_terminal(curwin->w_buffer)
				    && curwin->w_buffer->b_term != ശൂന്യ
				    && !term_in_normal_mode(curwin->w_buffer))
		may_move_terminal_to_buffer(curwin->w_buffer->b_term, TRUE);
#endif
	    validate_botline();
	    // In silent Ex mode botline is zero, return zero then.
	    pos.lnum = curwin->w_botline > 0 ? curwin->w_botline - 1 : 0;
	    തിരികെ &pos;
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (name[0] == '$')		// last column or line
    {
	എങ്കിൽ (dollar_lnum)
	{
	    pos.lnum = curbuf->b_ml.ml_വരി_എണ്ണം;
	    pos.col = 0;
	}
	അല്ലെങ്കിൽ
	{
	    pos.lnum = curwin->w_cursor.lnum;
	    എങ്കിൽ (charcol)
		pos.col = (colnr_T)mb_charlen(ml_get_curline());
	    അല്ലെങ്കിൽ
		pos.col = ml_get_curline_len();
	}
	തിരികെ &pos;
    }
    എങ്കിൽ (in_vim9script() && error)
	semsg(_(e_invalid_മൂല്യം_for_വരി_സംഖ്യ_str), name);
    തിരികെ ശൂന്യ;
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
    പൂർണ്ണസംഖ്യ
list2fpos(
    typval_T	*arg,
    pos_T	*posp,
    പൂർണ്ണസംഖ്യ		*fnump,
    colnr_T	*curswantp,
    പൂർണ്ണസംഖ്യ		charcol)
{
    list_T	*l = arg->vval.v_list;
    long	i = 0;
    long	n;

    // List must be: [fnum, lnum, col, coladd, curswant], where "fnum" is only
    // there when "fnump" isn't NULL; "coladd" and "curswant" are optional.
    എങ്കിൽ (arg->v_type != VAR_LIST
	    || l == ശൂന്യ
	    || l->lv_len < (fnump == ശൂന്യ ? 2 : 3)
	    || l->lv_len > (fnump == ശൂന്യ ? 4 : 5))
	തിരികെ FAIL;

    എങ്കിൽ (fnump != ശൂന്യ)
    {
	n = list_find_nr(l, i++, ശൂന്യ);	// fnum
	എങ്കിൽ (n < 0)
	    തിരികെ FAIL;
	എങ്കിൽ (n == 0)
	    n = curbuf->b_fnum;		// current buffer
	*fnump = n;
    }

    n = list_find_nr(l, i++, ശൂന്യ);	// lnum
    എങ്കിൽ (n < 0)
	തിരികെ FAIL;
    posp->lnum = n;

    n = list_find_nr(l, i++, ശൂന്യ);	// col
    എങ്കിൽ (n < 0)
	തിരികെ FAIL;
    // If character position is specified, then convert to byte position
    // If the line number is zero use the cursor line.
    എങ്കിൽ (charcol)
    {
	buf_T	*buf;

	// Get the text for the specified line in a loaded buffer
	buf = buflist_findnr(fnump == ശൂന്യ ? curbuf->b_fnum : *fnump);
	എങ്കിൽ (buf == ശൂന്യ || buf->b_ml.ml_mfp == ശൂന്യ)
	    തിരികെ FAIL;

	n = buf_charidx_to_byteidx(buf,
		  posp->lnum == 0 ? curwin->w_cursor.lnum : posp->lnum, n) + 1;
    }
    posp->col = n;

    n = list_find_nr(l, i, ശൂന്യ);	// off
    എങ്കിൽ (n < 0)
	posp->coladd = 0;
    അല്ലെങ്കിൽ
	posp->coladd = n;

    എങ്കിൽ (curswantp != ശൂന്യ)
	*curswantp = list_find_nr(l, i + 1, ശൂന്യ);  // curswant

    തിരികെ OK;
}

/*
 * Get the length of an environment variable name.
 * Advance "arg" to the first character after the name.
 * Return 0 for error.
 */
    പൂർണ്ണസംഖ്യ
get_env_len(char_u **arg)
{
    char_u	*p;
    പൂർണ്ണസംഖ്യ		len;

    വേണ്ടി (p = *arg; vim_isIDc(*p); ++p)
	;
    എങ്കിൽ (p == *arg)	    // no name found
	തിരികെ 0;

    len = (പൂർണ്ണസംഖ്യ)(p - *arg);
    *arg = p;
    തിരികെ len;
}

/*
 * Get the length of the name of a function or internal variable.
 * "arg" is advanced to after the name.
 * Return 0 if something is wrong.
 */
    പൂർണ്ണസംഖ്യ
get_id_len(char_u **arg)
{
    char_u	*p;
    പൂർണ്ണസംഖ്യ		len;

    // Find the end of the name.
    വേണ്ടി (p = *arg; eval_isnamec(*p); ++p)
    {
	എങ്കിൽ (*p == ':')
	{
	    // "s:" is start of "s:var", but "n:" is not and can be used in
	    // slice "[n:]".  Also "xx:" is not a namespace.
	    len = (പൂർണ്ണസംഖ്യ)(p - *arg);
	    എങ്കിൽ ((len == 1 && vim_strchr(NAMESPACE_CHAR, **arg) == ശൂന്യ)
		    || len > 1)
		നിർത്തുക;
	}
    }
    എങ്കിൽ (p == *arg)	    // no name found
	തിരികെ 0;

    len = (പൂർണ്ണസംഖ്യ)(p - *arg);
    *arg = p;

    തിരികെ len;
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
    പൂർണ്ണസംഖ്യ
get_name_len(
    char_u	**arg,
    char_u	**alias,
    പൂർണ്ണസംഖ്യ		evaluate,
    പൂർണ്ണസംഖ്യ		verbose)
{
    പൂർണ്ണസംഖ്യ		len;
    char_u	*p;
    char_u	*വ്യഞ്ജകം_start;
    char_u	*വ്യഞ്ജകം_end;

    *alias = ശൂന്യ;  // default to no alias

    എങ്കിൽ ((*arg)[0] == K_SPECIAL && (*arg)[1] == KS_EXTRA
						  && (*arg)[2] == (പൂർണ്ണസംഖ്യ)KE_SNR)
    {
	// hard coded <SNR>, already translated
	*arg += 3;
	തിരികെ get_id_len(arg) + 3;
    }
    len = eval_fname_script(*arg);
    എങ്കിൽ (len > 0)
    {
	// literal "<SID>", "s:" or "<SNR>"
	*arg += len;
    }

    /*
     * Find the end of the name; check for {} construction.
     */
    p = find_name_end(*arg, &വ്യഞ്ജകം_start, &വ്യഞ്ജകം_end,
					       len > 0 ? 0 : FNE_CHECK_START);
    എങ്കിൽ (വ്യഞ്ജകം_start != ശൂന്യ)
    {
	char_u	*temp_string;

	എങ്കിൽ (!evaluate)
	{
	    len += (പൂർണ്ണസംഖ്യ)(p - *arg);
	    *arg = skipwhite(p);
	    തിരികെ len;
	}

	/*
	 * Include any <SID> etc in the expanded string:
	 * Thus the -len here.
	 */
	temp_string = make_expanded_name(*arg - len, വ്യഞ്ജകം_start, വ്യഞ്ജകം_end, p);
	എങ്കിൽ (temp_string == ശൂന്യ)
	    തിരികെ -1;
	*alias = temp_string;
	*arg = skipwhite(p);
	തിരികെ (പൂർണ്ണസംഖ്യ)STRLEN(temp_string);
    }

    len += get_id_len(arg);
    // Only give an error when there is something, otherwise it will be
    // reported at a higher level.
    എങ്കിൽ (len == 0 && verbose && **arg != NUL)
	semsg(_(e_invalid_expression_str), *arg);

    തിരികെ len;
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
    char_u	**വ്യഞ്ജകം_start,
    char_u	**വ്യഞ്ജകം_end,
    പൂർണ്ണസംഖ്യ		flags)
{
    പൂർണ്ണസംഖ്യ		mb_nest = 0;
    പൂർണ്ണസംഖ്യ		br_nest = 0;
    char_u	*p;
    പൂർണ്ണസംഖ്യ		len;
    പൂർണ്ണസംഖ്യ		allow_curly = !in_vim9script();

    എങ്കിൽ (വ്യഞ്ജകം_start != ശൂന്യ)
    {
	*വ്യഞ്ജകം_start = ശൂന്യ;
	*വ്യഞ്ജകം_end = ശൂന്യ;
    }

    // Quick check for valid starting character.
    എങ്കിൽ ((flags & FNE_CHECK_START) && !eval_isnamec1(*arg)
					      && (*arg != '{' || !allow_curly))
	തിരികെ arg;

    വേണ്ടി (p = arg; *p != NUL
		    && (eval_isnamec(*p)
			|| (*p == '{' && allow_curly)
			|| ((flags & FNE_INCL_BR) && (*p == '['
					 || (*p == '.' && eval_isdictc(p[1]))))
			|| mb_nest != 0
			|| br_nest != 0); MB_PTR_ADV(p))
    {
	എങ്കിൽ (*p == '\'')
	{
	    // skip over 'string' to avoid counting [ and ] inside it.
	    വേണ്ടി (p = p + 1; *p != NUL && *p != '\''; MB_PTR_ADV(p))
		;
	    എങ്കിൽ (*p == NUL)
		നിർത്തുക;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (*p == '"')
	{
	    // skip over "str\"ing" to avoid counting [ and ] inside it.
	    വേണ്ടി (p = p + 1; *p != NUL && *p != '"'; MB_PTR_ADV(p))
		എങ്കിൽ (*p == '\\' && p[1] != NUL)
		    ++p;
	    എങ്കിൽ (*p == NUL)
		നിർത്തുക;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (br_nest == 0 && mb_nest == 0 && *p == ':')
	{
	    // "s:" is start of "s:var", but "n:" is not and can be used in
	    // slice "[n:]".  Also "xx:" is not a namespace. But {ns}: is.
	    len = (പൂർണ്ണസംഖ്യ)(p - arg);
	    എങ്കിൽ ((len == 1 && vim_strchr(NAMESPACE_CHAR, *arg) == ശൂന്യ)
		    || (len > 1 && p[-1] != '}'))
		നിർത്തുക;
	}

	എങ്കിൽ (mb_nest == 0)
	{
	    എങ്കിൽ (*p == '[')
		++br_nest;
	    അല്ലെങ്കിൽ എങ്കിൽ (*p == ']')
		--br_nest;
	}

	എങ്കിൽ (br_nest == 0 && allow_curly)
	{
	    എങ്കിൽ (*p == '{')
	    {
		mb_nest++;
		എങ്കിൽ (വ്യഞ്ജകം_start != ശൂന്യ && *വ്യഞ്ജകം_start == ശൂന്യ)
		    *വ്യഞ്ജകം_start = p;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (*p == '}')
	    {
		mb_nest--;
		എങ്കിൽ (വ്യഞ്ജകം_start != ശൂന്യ && mb_nest == 0 && *വ്യഞ്ജകം_end == ശൂന്യ)
		    *വ്യഞ്ജകം_end = p;
	    }
	}
    }

    തിരികെ p;
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
    സ്ഥിതം char_u *
make_expanded_name(
    char_u	*in_start,
    char_u	*വ്യഞ്ജകം_start,
    char_u	*വ്യഞ്ജകം_end,
    char_u	*in_end)
{
    char_u	c1;
    char_u	*retval = ശൂന്യ;
    char_u	*temp_result;

    എങ്കിൽ (വ്യഞ്ജകം_end == ശൂന്യ || in_end == ശൂന്യ)
	തിരികെ ശൂന്യ;
    *വ്യഞ്ജകം_start	= NUL;
    *വ്യഞ്ജകം_end = NUL;
    c1 = *in_end;
    *in_end = NUL;

    temp_result = eval_to_string(വ്യഞ്ജകം_start + 1, FALSE, FALSE);
    എങ്കിൽ (temp_result != ശൂന്യ)
    {
	size_t	retvalsize = (size_t)(വ്യഞ്ജകം_start - in_start)
				+ STRLEN(temp_result)
				+ (size_t)(in_end - വ്യഞ്ജകം_end) + 1;

	retval = alloc(retvalsize);
	എങ്കിൽ (retval != ശൂന്യ)
	    vim_snprintf((അക്ഷരം *)retval, retvalsize, "%s%s%s",
				in_start, temp_result, വ്യഞ്ജകം_end + 1);
    }
    vim_free(temp_result);

    *in_end = c1;		// put char back for error messages
    *വ്യഞ്ജകം_start = '{';
    *വ്യഞ്ജകം_end = '}';

    എങ്കിൽ (retval != ശൂന്യ)
    {
	temp_result = find_name_end(retval, &വ്യഞ്ജകം_start, &വ്യഞ്ജകം_end, 0);
	എങ്കിൽ (വ്യഞ്ജകം_start != ശൂന്യ)
	{
	    // Further expansion!
	    temp_result = make_expanded_name(retval, വ്യഞ്ജകം_start,
						       വ്യഞ്ജകം_end, temp_result);
	    vim_free(retval);
	    retval = temp_result;
	}
    }

    തിരികെ retval;
}

/*
 * Return TRUE if character "c" can be used in a variable or function name.
 * Does not include '{' or '}' for magic braces.
 */
    പൂർണ്ണസംഖ്യ
eval_isnamec(പൂർണ്ണസംഖ്യ c)
{
    തിരികെ ASCII_ISALNUM(c) || c == '_' || c == ':' || c == AUTOLOAD_CHAR;
}

/*
 * Return TRUE if character "c" can be used as the first character in a
 * variable or function name (excluding '{' and '}').
 */
    പൂർണ്ണസംഖ്യ
eval_isnamec1(പൂർണ്ണസംഖ്യ c)
{
    തിരികെ ASCII_ISALPHA(c) || c == '_';
}

/*
 * Return TRUE if character "c" can be used as the first character of a
 * dictionary key.
 */
    പൂർണ്ണസംഖ്യ
eval_isdictc(പൂർണ്ണസംഖ്യ c)
{
    തിരികെ ASCII_ISALNUM(c) || c == '_';
}

/*
 * Return true when "p" points to "->" followed by a method name or a lambda.
 * Only called for Vim9 script.
 */
    സ്ഥിതം bool
method_call_follows(char_u *p)
{
    എങ്കിൽ (p[0] != '-' || p[1] != '>')
	തിരികെ false;

    char_u	*after = skipwhite(p + 2);

    തിരികെ ASCII_ISALPHA(*after) || *after == '(';
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
    പൂർണ്ണസംഖ്യ
handle_subscript(
    char_u	**arg,
    char_u	*name_start,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    പൂർണ്ണസംഖ്യ		verbose)	// give error messages
{
    പൂർണ്ണസംഖ്യ		evaluate = evalarg != ശൂന്യ
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    പൂർണ്ണസംഖ്യ		ret = OK;
    dict_T	*selfdict = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		check_white = TRUE;
    പൂർണ്ണസംഖ്യ		getnext;
    char_u	*p;

    ആകുമ്പോൾ (ret == OK)
    {
	// When at the end of the line and ".name" or a method call follows in
	// the next line then consume the line break.
	p = eval_next_non_blank(*arg, evalarg, &getnext);
	എങ്കിൽ (getnext
	    && ((*p == '.'
		    && ((rettv->v_type == VAR_DICT && eval_isdictc(p[1]))
			|| rettv->v_type == VAR_CLASS
			|| rettv->v_type == VAR_OBJECT))
		|| method_call_follows(p)))
	{
	    *arg = eval_next_വരി(*arg, evalarg);
	    p = *arg;
	    check_white = FALSE;
	}

	എങ്കിൽ (rettv->v_type == VAR_ANY)
	{
	    char_u	*exp_name;
	    പൂർണ്ണസംഖ്യ		cc;
	    പൂർണ്ണസംഖ്യ		idx;
	    ufunc_T	*ufunc;
	    type_T	*type;

	    // Found script from "import {name} as name", script item name must
	    // follow.  "rettv->vval.v_number" has the script ID.
	    എങ്കിൽ (**arg != '.')
	    {
		എങ്കിൽ (verbose)
		    semsg(_(e_expected_dot_after_name_str),
					name_start != ശൂന്യ ? name_start: *arg);
		ret = FAIL;
		നിർത്തുക;
	    }
	    ++*arg;
	    എങ്കിൽ (IS_WHITE_OR_NUL(**arg))
	    {
		എങ്കിൽ (verbose)
		    emsg(_(e_no_white_space_allowed_after_dot));
		ret = FAIL;
		നിർത്തുക;
	    }

	    // isolate the name
	    exp_name = *arg;
	    ആകുമ്പോൾ (eval_isnamec(**arg))
		++*arg;
	    cc = **arg;
	    **arg = NUL;

	    idx = find_exported(rettv->vval.v_സംഖ്യ, exp_name, &ufunc, &type,
		       evalarg == ശൂന്യ ? ശൂന്യ : evalarg->eval_cctx,
		       evalarg == ശൂന്യ ? ശൂന്യ : evalarg->eval_cstack, verbose);
	    **arg = cc;

	    എങ്കിൽ (idx < 0 && ufunc == ശൂന്യ)
	    {
		ret = FAIL;
		നിർത്തുക;
	    }
	    എങ്കിൽ (idx >= 0)
	    {
		scriptitem_T    *si = SCRIPT_ITEM(rettv->vval.v_സംഖ്യ);
		svar_T		*sv = ((svar_T *)si->sn_var_vals.ga_data) + idx;

		copy_tv(sv->sv_tv, rettv);
	    }
	    അല്ലെങ്കിൽ
	    {
		rettv->v_type = VAR_FUNC;
		എങ്കിൽ (**arg == '<')
		{
		    char_u *s = get_generic_func_name(ufunc, arg);
		    എങ്കിൽ (s != ശൂന്യ)
			rettv->vval.v_string = s;
		    അല്ലെങ്കിൽ
			ret = FAIL;
		}
		അല്ലെങ്കിൽ
		{
		    rettv->vval.v_string =
			vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
		}
	    }
	    തുടരുക;
	}

	എങ്കിൽ ((**arg == '(' && (!evaluate || rettv->v_type == VAR_FUNC
			    || rettv->v_type == VAR_PARTIAL))
		    && (!check_white || !VIM_ISWHITE(*(*arg - 1))))
	{
	    ret = call_func_rettv(arg, evalarg, rettv, evaluate,
							       selfdict, ശൂന്യ);

	    // Stop the expression evaluation when immediately aborting on
	    // error, or when an interrupt occurred or an exception was thrown
	    // but not caught.
	    എങ്കിൽ (aborting())
	    {
		എങ്കിൽ (ret == OK)
		    clear_tv(rettv);
		ret = FAIL;
	    }
	    dict_unref(selfdict);
	    selfdict = ശൂന്യ;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (p[0] == '-' && p[1] == '>')
	{
	    എങ്കിൽ (in_vim9script())
		*arg = skipwhite(p + 2);
	    അല്ലെങ്കിൽ
		*arg = p + 2;
	    എങ്കിൽ (ret == OK && evaluate && rettv->v_type == VAR_VOID)
	    {
		എങ്കിൽ (verbose)
		    emsg(_(e_cannot_use_void_മൂല്യം));
		ret = FAIL;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (VIM_ISWHITE(**arg))
	    {
		emsg(_(e_no_white_space_allowed_before_parenthesis));
		ret = FAIL;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ ((**arg == '{' && !in_vim9script()) || **arg == '(')
		// expr->{lambda}() or expr->(lambda)()
		ret = eval_lambda(arg, rettv, evalarg, verbose);
	    അല്ലെങ്കിൽ
		// expr->name()
		ret = eval_method(arg, rettv, evalarg, verbose);
	}
	// "." is ".name" lookup when we found a dict or when evaluating and
	// scriptversion is at least 2, where string concatenation is "..".
	അല്ലെങ്കിൽ എങ്കിൽ (**arg == '['
		|| (**arg == '.' && (rettv->v_type == VAR_DICT
			|| (!evaluate
			    && (*arg)[1] != '.'
			    && !in_old_script(2)))))
	{
	    dict_unref(selfdict);
	    എങ്കിൽ (rettv->v_type == VAR_DICT)
	    {
		selfdict = rettv->vval.v_dict;
		എങ്കിൽ (selfdict != ശൂന്യ)
		    ++selfdict->dv_refcount;
	    }
	    അല്ലെങ്കിൽ
		selfdict = ശൂന്യ;
	    എങ്കിൽ (eval_index(arg, rettv, evalarg, verbose) == FAIL)
	    {
		clear_tv(rettv);
		ret = FAIL;
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (**arg == '.' && (rettv->v_type == VAR_CLASS
					       || rettv->v_type == VAR_OBJECT))
	{
	    // class member: SomeClass.varname
	    // class method: SomeClass.SomeMethod()
	    // class constructor: SomeClass.new()
	    // object member: someObject.varname
	    // object method: someObject.SomeMethod()
	    എങ്കിൽ (class_object_index(arg, rettv, evalarg, verbose) == FAIL)
	    {
		clear_tv(rettv);
		ret = FAIL;
	    }
	}
	അല്ലെങ്കിൽ
	    നിർത്തുക;
    }

    // Turn "dict.Func" into a partial for "Func" bound to "dict".
    // Don't do this when "Func" is already a partial that was bound
    // explicitly (pt_auto is FALSE).
    എങ്കിൽ (selfdict != ശൂന്യ
	    && (rettv->v_type == VAR_FUNC
		|| (rettv->v_type == VAR_PARTIAL
		    && (rettv->vval.v_partial->pt_auto
			|| rettv->vval.v_partial->pt_dict == ശൂന്യ))))
	selfdict = make_partial(selfdict, rettv);

    dict_unref(selfdict);
    തിരികെ ret;
}

/*
 * Make a copy of an item.
 * Lists and Dictionaries are also copied.  A deep copy if "deep" is set.
 * "top" is TRUE for the toplevel of copy().
 * For deepcopy() "copyID" is zero for a full copy or the ID for when a
 * reference to an already copied list/dict can be used.
 * Returns FAIL or OK.
 */
    പൂർണ്ണസംഖ്യ
item_copy(
    typval_T	*from,
    typval_T	*to,
    പൂർണ്ണസംഖ്യ		deep,
    പൂർണ്ണസംഖ്യ		മുകൾ,
    പൂർണ്ണസംഖ്യ		copyID)
{
    സ്ഥിതം പൂർണ്ണസംഖ്യ	recurse = 0;
    പൂർണ്ണസംഖ്യ		ret = OK;

    എങ്കിൽ (recurse >= DICT_MAXNEST)
    {
	emsg(_(e_variable_nested_too_deep_for_making_copy));
	തിരികെ FAIL;
    }
    ++recurse;

    തിരഞ്ഞെടുപ്പ് (from->v_type)
    {
	അവസ്ഥ VAR_സംഖ്യ:
	അവസ്ഥ VAR_FLOAT:
	അവസ്ഥ VAR_STRING:
	അവസ്ഥ VAR_FUNC:
	അവസ്ഥ VAR_PARTIAL:
	അവസ്ഥ VAR_BOOL:
	അവസ്ഥ VAR_SPECIAL:
	അവസ്ഥ VAR_JOB:
	അവസ്ഥ VAR_CHANNEL:
	അവസ്ഥ VAR_INSTR:
	അവസ്ഥ VAR_CLASS:
	അവസ്ഥ VAR_OBJECT:
	അവസ്ഥ VAR_TYPEALIAS:
	    copy_tv(from, to);
	    നിർത്തുക;
	അവസ്ഥ VAR_LIST:
	    to->v_type = VAR_LIST;
	    to->v_lock = 0;
	    എങ്കിൽ (from->vval.v_list == ശൂന്യ)
		to->vval.v_list = ശൂന്യ;
	    അല്ലെങ്കിൽ എങ്കിൽ (copyID != 0 && from->vval.v_list->lv_copyID == copyID)
	    {
		// use the copy made earlier
		to->vval.v_list = from->vval.v_list->lv_copylist;
		++to->vval.v_list->lv_refcount;
	    }
	    അല്ലെങ്കിൽ
	    {
		to->vval.v_list = list_copy(from->vval.v_list,
							    deep, മുകൾ, copyID);
		എങ്കിൽ (to->vval.v_list == ശൂന്യ)
		    ret = FAIL;
	    }
	    നിർത്തുക;
	അവസ്ഥ VAR_TUPLE:
	    to->v_type = VAR_TUPLE;
	    to->v_lock = 0;
	    എങ്കിൽ (from->vval.v_tuple == ശൂന്യ)
		to->vval.v_tuple = ശൂന്യ;
	    അല്ലെങ്കിൽ എങ്കിൽ (copyID != 0 && from->vval.v_tuple->tv_copyID == copyID)
	    {
		// use the copy made earlier
		to->vval.v_tuple = from->vval.v_tuple->tv_copytuple;
		++to->vval.v_tuple->tv_refcount;
	    }
	    അല്ലെങ്കിൽ
	    {
		to->vval.v_tuple = tuple_copy(from->vval.v_tuple,
							    deep, മുകൾ, copyID);
		എങ്കിൽ (to->vval.v_tuple == ശൂന്യ)
		    ret = FAIL;
	    }
	    നിർത്തുക;
	അവസ്ഥ VAR_BLOB:
	    to->v_type = VAR_BLOB;
	    to->v_lock = 0;
	    എങ്കിൽ (from->vval.v_blob == ശൂന്യ)
		to->vval.v_blob = ശൂന്യ;
	    അല്ലെങ്കിൽ
	    {
		to->vval.v_blob = blob_copy(from->vval.v_blob);
		എങ്കിൽ (to->vval.v_blob == ശൂന്യ)
		    ret = FAIL;
	    }
	    നിർത്തുക;
	അവസ്ഥ VAR_DICT:
	    to->v_type = VAR_DICT;
	    to->v_lock = 0;
	    എങ്കിൽ (from->vval.v_dict == ശൂന്യ)
		to->vval.v_dict = ശൂന്യ;
	    അല്ലെങ്കിൽ എങ്കിൽ (copyID != 0 && from->vval.v_dict->dv_copyID == copyID)
	    {
		// use the copy made earlier
		to->vval.v_dict = from->vval.v_dict->dv_copydict;
		++to->vval.v_dict->dv_refcount;
	    }
	    അല്ലെങ്കിൽ
	    {
		to->vval.v_dict = dict_copy(from->vval.v_dict,
							    deep, മുകൾ, copyID);
		എങ്കിൽ (to->vval.v_dict == ശൂന്യ)
		    ret = FAIL;
	    }
	    നിർത്തുക;
	അവസ്ഥ VAR_UNKNOWN:
	അവസ്ഥ VAR_ANY:
	അവസ്ഥ VAR_VOID:
	    internal_error_no_abort("item_copy(UNKNOWN)");
	    ret = FAIL;
    }
    --recurse;
    തിരികെ ret;
}

    ശൂന്യം
echo_one(typval_T *rettv, പൂർണ്ണസംഖ്യ with_space, പൂർണ്ണസംഖ്യ *atstart, പൂർണ്ണസംഖ്യ *needclr)
{
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    char_u	*p = echo_string(rettv, &tofree, numbuf, get_copyID());

    എങ്കിൽ (*atstart)
    {
	*atstart = FALSE;
	// Call msg_start() after eval1(), evaluating the expression
	// may cause a message to appear.
	എങ്കിൽ (with_space)
	{
	    // Mark the saved text as finishing the line, so that what
	    // follows is displayed on a new line when scrolling back
	    // at the more prompt.
	    സന്ദേശം_sb_eol();
	    സന്ദേശം_start();
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (with_space)
	സന്ദേശം_puts_attr(" ", echo_attr);

    എങ്കിൽ (p != ശൂന്യ)
	വേണ്ടി ( ; *p != NUL && !got_int; ++p)
	{
	    എങ്കിൽ (*p == '\n' || *p == '\r' || *p == TAB)
	    {
		എങ്കിൽ (*p != TAB && *needclr)
		{
		    // remove any text still there from the command
		    സന്ദേശം_clr_eos();
		    *needclr = FALSE;
		}
		സന്ദേശം_putchar_attr(*p, echo_attr);
	    }
	    അല്ലെങ്കിൽ
	    {
		എങ്കിൽ (has_mbyte)
		{
		    പൂർണ്ണസംഖ്യ i = (*mb_ptr2len)(p);

		    (ശൂന്യം)സന്ദേശം_outtrans_len_attr(p, i, echo_attr);
		    p += i - 1;
		}
		അല്ലെങ്കിൽ
		    (ശൂന്യം)സന്ദേശം_outtrans_len_attr(p, 1, echo_attr);
	    }
	}
    vim_free(tofree);
}

/*
 * ":echo expr1 ..."	print each argument separated with a space, add a
 *			newline at the end.
 * ":echon expr1 ..."	print each argument plain.
 */
    ശൂന്യം
ex_echo(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    typval_T	rettv;
    char_u	*arg_start;
    പൂർണ്ണസംഖ്യ		needclr = TRUE;
    പൂർണ്ണസംഖ്യ		atstart = TRUE;
    പൂർണ്ണസംഖ്യ		did_emsg_before = did_emsg;
    പൂർണ്ണസംഖ്യ		called_emsg_before = called_emsg;
    evalarg_T	evalarg;

    fill_evalarg_from_eap(&evalarg, eap, eap->skip);

    എങ്കിൽ (eap->skip)
	++emsg_skip;
    ആകുമ്പോൾ ((!ends_excmd2(eap->cmd, arg) || *arg == '"') && !got_int)
    {
	// If eval1() causes an error message the text from the command may
	// still need to be cleared. E.g., "echo 22,44".
	need_clr_eos = needclr;

	arg_start = arg;
	എങ്കിൽ (eval1(&arg, &rettv, &evalarg) == FAIL)
	{
	    /*
	     * Report the invalid expression unless the expression evaluation
	     * has been cancelled due to an aborting error, an interrupt, or an
	     * exception.
	     */
	    എങ്കിൽ (!aborting() && did_emsg == did_emsg_before
					  && called_emsg == called_emsg_before)
		semsg(_(e_invalid_expression_str), arg_start);
	    need_clr_eos = FALSE;
	    നിർത്തുക;
	}
	need_clr_eos = FALSE;

	എങ്കിൽ (!eap->skip)
	{
	    എങ്കിൽ (rettv.v_type == VAR_VOID)
	    {
		semsg(_(e_expression_does_not_result_in_മൂല്യം_str), arg_start);
		നിർത്തുക;
	    }
	    echo_one(&rettv, eap->cmdidx == CMD_echo, &atstart, &needclr);
	}

	clear_tv(&rettv);
	arg = skipwhite(arg);
    }
    set_nextcmd(eap, arg);
    clear_evalarg(&evalarg, eap);

    എങ്കിൽ (eap->skip)
	--emsg_skip;
    അല്ലെങ്കിൽ
    {
	// remove text that may still be there from the command
	എങ്കിൽ (needclr)
	    സന്ദേശം_clr_eos();
	എങ്കിൽ (eap->cmdidx == CMD_echo)
	    സന്ദേശം_end();
    }
}

/*
 * ":echohl {name}".
 */
    ശൂന്യം
ex_echohl(exarg_T *eap)
{
    echo_attr = syn_name2attr(eap->arg);
}

/*
 * Returns the :echo attribute
 */
    പൂർണ്ണസംഖ്യ
get_echo_attr(ശൂന്യം)
{
    തിരികെ echo_attr;
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
    ശൂന്യം
ex_execute(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    typval_T	rettv;
    പൂർണ്ണസംഖ്യ		ret = OK;
    char_u	*p;
    garray_T	ga;
    പൂർണ്ണസംഖ്യ		len;
    long	start_lnum = SOURCING_LNUM;

    ga_init2(&ga, 1, 80);

    എങ്കിൽ (eap->skip)
	++emsg_skip;
    ആകുമ്പോൾ (!ends_excmd2(eap->cmd, arg) || *arg == '"')
    {
	ret = eval1_emsg(&arg, &rettv, eap);
	എങ്കിൽ (ret == FAIL)
	    നിർത്തുക;

	എങ്കിൽ (!eap->skip)
	{
	    char_u   buf[NUMBUFLEN];

	    എങ്കിൽ (eap->cmdidx == CMD_execute)
	    {
		എങ്കിൽ (rettv.v_type == VAR_CHANNEL || rettv.v_type == VAR_JOB)
		{
		    semsg(_(e_using_invalid_മൂല്യം_as_string_str),
						  vartype_name(rettv.v_type));
		    p = ശൂന്യ;
		}
		അല്ലെങ്കിൽ
		    p = tv_get_string_buf(&rettv, buf);
	    }
	    അല്ലെങ്കിൽ
		p = tv_stringify(&rettv, buf);
	    എങ്കിൽ (p == ശൂന്യ)
	    {
		clear_tv(&rettv);
		ret = FAIL;
		നിർത്തുക;
	    }
	    len = (പൂർണ്ണസംഖ്യ)STRLEN(p);
	    എങ്കിൽ (ga_grow(&ga, len + 2) == FAIL)
	    {
		clear_tv(&rettv);
		ret = FAIL;
		നിർത്തുക;
	    }
	    എങ്കിൽ (ga.ga_len)
		((char_u *)(ga.ga_data))[ga.ga_len++] = ' ';
	    STRCPY((char_u *)(ga.ga_data) + ga.ga_len, p);
	    ga.ga_len += len;
	}

	clear_tv(&rettv);
	arg = skipwhite(arg);
    }

    എങ്കിൽ (ret != FAIL && ga.ga_data != ശൂന്യ)
    {
	// use the first line of continuation lines for messages
	SOURCING_LNUM = start_lnum;

	എങ്കിൽ (eap->cmdidx == CMD_echomsg
		|| eap->cmdidx == CMD_echowindow
		|| eap->cmdidx == CMD_echoerr)
	{
	    // Mark the already saved text as finishing the line, so that what
	    // follows is displayed on a new line when scrolling back at the
	    // more prompt.
	    സന്ദേശം_sb_eol();
	}

	എങ്കിൽ (eap->cmdidx == CMD_echomsg)
	{
	    സന്ദേശം_attr(ga.ga_data, echo_attr);
	    out_flush();
	}
	അല്ലെങ്കിൽ എങ്കിൽ (eap->cmdidx == CMD_echowindow)
	{
#ifdef HAS_MESSAGE_WINDOW
	    start_echowindow(eap->addr_എണ്ണം > 0 ? eap->line2 : 0);
#endif
	    സന്ദേശം_attr(ga.ga_data, echo_attr);
#ifdef HAS_MESSAGE_WINDOW
	    end_echowindow();
#endif
	}
	അല്ലെങ്കിൽ എങ്കിൽ (eap->cmdidx == CMD_echoconsole)
	{
	    ui_write(ga.ga_data, (പൂർണ്ണസംഖ്യ)STRLEN(ga.ga_data), TRUE);
	    ui_write((char_u *)"\r\n", 2, TRUE);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (eap->cmdidx == CMD_echoerr)
	{
	    പൂർണ്ണസംഖ്യ		save_did_emsg = did_emsg;

	    // We don't want to abort following commands, restore did_emsg.
	    emsg(ga.ga_data);
	    എങ്കിൽ (!force_abort)
		did_emsg = save_did_emsg;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (eap->cmdidx == CMD_execute)
	{
	    പൂർണ്ണസംഖ്യ save_sticky_cmdmod_flags = sticky_cmdmod_flags;

	    // "legacy exe cmd" and "vim9cmd exe cmd" applies to "cmd".
	    sticky_cmdmod_flags = cmdmod.cmod_flags
						& (CMOD_LEGACY | CMOD_VIM9CMD);
	    do_cmdline((char_u *)ga.ga_data,
		       eap->ea_getline, eap->cookie, DOCMD_NOWAIT|DOCMD_VERBOSE);
	    sticky_cmdmod_flags = save_sticky_cmdmod_flags;
	}
    }

    ga_clear(&ga);

    എങ്കിൽ (eap->skip)
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
find_option_end(char_u **arg, പൂർണ്ണസംഖ്യ *scope)
{
    char_u	*p = *arg;

    ++p;
    എങ്കിൽ (*p == 'g' && p[1] == ':')
    {
	*scope = OPT_GLOBAL;
	p += 2;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (*p == 'l' && p[1] == ':')
    {
	*scope = OPT_LOCAL;
	p += 2;
    }
    അല്ലെങ്കിൽ
	*scope = 0;

    എങ്കിൽ (!ASCII_ISALPHA(*p))
	തിരികെ ശൂന്യ;
    *arg = p;

    എങ്കിൽ (p[0] == 't' && p[1] == '_' && p[2] != NUL && p[3] != NUL)
	p += 4;	    // termcap option
    അല്ലെങ്കിൽ
	ആകുമ്പോൾ (ASCII_ISALPHA(*p))
	    ++p;
    തിരികെ p;
}

/*
 * Display script name where an item was last set.
 * Should only be invoked when 'verbose' is non-zero.
 */
    ശൂന്യം
last_set_സന്ദേശം(sctx_T script_ctx)
{
    char_u *p;

    എങ്കിൽ (script_ctx.sc_sid == 0)
	തിരികെ;

    p = home_replace_save(ശൂന്യ, get_scriptname(script_ctx.sc_sid));
    എങ്കിൽ (p == ശൂന്യ)
	തിരികെ;

    verbose_enter();
    സന്ദേശം_puts(_("\n\tLast set from "));
    സന്ദേശം_puts((അക്ഷരം *)p);
    എങ്കിൽ (script_ctx.sc_lnum > 0)
    {
	സന്ദേശം_puts(_(വരി_സന്ദേശം));
	സന്ദേശം_outnum((long)script_ctx.sc_lnum);
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
    typval_T	*വ്യഞ്ജകം,
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
    എങ്കിൽ (regmatch.regprog != ശൂന്യ)
    {
	char_u	*tail = str;
	char_u	*end = str + len;
	പൂർണ്ണസംഖ്യ	do_all = (flags[0] == 'g');
	പൂർണ്ണസംഖ്യ	sublen;
	പൂർണ്ണസംഖ്യ	i;
	char_u	*zero_width = ശൂന്യ;

	ആകുമ്പോൾ (vim_regexec_nl(&regmatch, str, (colnr_T)(tail - str)))
	{
	    // Skip empty match except for first match.
	    എങ്കിൽ (regmatch.startp[0] == regmatch.endp[0])
	    {
		എങ്കിൽ (zero_width == regmatch.startp[0])
		{
		    // avoid getting stuck on a match with an empty string
		    i = mb_ptr2len(tail);
		    mch_memmove((char_u *)ga.ga_data + ga.ga_len, tail,
								   (size_t)i);
		    ga.ga_len += i;
		    tail += i;
		    തുടരുക;
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
	    sublen = vim_regsub(&regmatch, sub, വ്യഞ്ജകം, tail, 0, REGSUB_MAGIC);
	    എങ്കിൽ (sublen <= 0)
	    {
		ga_clear(&ga);
		നിർത്തുക;
	    }
	    എങ്കിൽ (ga_grow(&ga, (പൂർണ്ണസംഖ്യ)((end - tail) + sublen -
			    (regmatch.endp[0] - regmatch.startp[0]))) == FAIL)
	    {
		ga_clear(&ga);
		നിർത്തുക;
	    }

	    // copy the text up to where the match is
	    i = (പൂർണ്ണസംഖ്യ)(regmatch.startp[0] - tail);
	    mch_memmove((char_u *)ga.ga_data + ga.ga_len, tail, (size_t)i);
	    // add the substituted text
	    (ശൂന്യം)vim_regsub(&regmatch, sub, വ്യഞ്ജകം,
				  (char_u *)ga.ga_data + ga.ga_len + i, sublen,
				  REGSUB_COPY | REGSUB_MAGIC);
	    ga.ga_len += i + sublen - 1;
	    tail = regmatch.endp[0];
	    എങ്കിൽ (*tail == NUL)
		നിർത്തുക;
	    എങ്കിൽ (!do_all)
		നിർത്തുക;
	}

	എങ്കിൽ (ga.ga_data != ശൂന്യ)
	{
	    STRCPY((അക്ഷരം *)ga.ga_data + ga.ga_len, tail);
	    ga.ga_len += (പൂർണ്ണസംഖ്യ)(end - tail);
	}

	vim_regfree(regmatch.regprog);
    }

    എങ്കിൽ (ga.ga_data != ശൂന്യ)
    {
	str = (char_u *)ga.ga_data;
	len = (size_t)ga.ga_len;
    }
    ret = vim_strnsave(str, len);
    ga_clear(&ga);
    എങ്കിൽ (p_cpo == empty_option)
	p_cpo = save_cpo;
    അല്ലെങ്കിൽ
    {
	// Darn, evaluating {sub} expression or {expr} changed the value.
	// If it's still empty it was changed and restored, need to restore in
	// the complicated way.
	എങ്കിൽ (*p_cpo == NUL)
	    set_option_മൂല്യം_give_err((char_u *)"cpo", 0L, save_cpo, 0);
	free_string_option(save_cpo);
    }

    എങ്കിൽ (ret_len != ശൂന്യ)
	*ret_len = len;

    തിരികെ ret;
}
