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

#ଅନ୍ତର୍ଭାଵ "vim.h"

#ଯଦି defined(FEAT_EVAL)

#ifdef VMS
# include <float.h>
#endif

#define NAMESPACE_CHAR	(char_u *)"abglstvw"

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା eval0_simple_funccal(char_u *arg, typval_T *rettv, exarg_T *eap, evalarg_T *evalarg);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା eval2(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା eval3(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା eval4(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା eval5(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା eval6(char_u **arg, typval_T *rettv, evalarg_T *evalarg);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା eval7(char_u **arg, typval_T *rettv, evalarg_T *evalarg, ପୂର୍ଣ୍ଣସଂଖ୍ୟା want_string);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା eval8(char_u **arg, typval_T *rettv, evalarg_T *evalarg, ପୂର୍ଣ୍ଣସଂଖ୍ୟା want_string);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା eval9(char_u **arg, typval_T *rettv, evalarg_T *evalarg, ପୂର୍ଣ୍ଣସଂଖ୍ୟା want_string);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା eval9_leader(typval_T *rettv, ପୂର୍ଣ୍ଣସଂଖ୍ୟା numeric_only, char_u *start_leader, char_u **end_leaderp);

ସ୍ଥିର char_u *make_expanded_name(char_u *in_start, char_u *ପଦସମଷ୍ଟି_start, char_u *ପଦସମଷ୍ଟି_end, char_u *in_end);

/*
 * Return "n1" divided by "n2", taking care of dividing by zero.
 * If "failed" is not NULL set it to TRUE when dividing by zero fails.
 */
	varnumber_T
ଅଙ୍କ_divide(varnumber_T n1, varnumber_T n2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *failed)
{
    varnumber_T	result;

    ଯଦି (n2 == 0)
    {
	ଯଦି (in_vim9script())
	{
	    emsg(_(e_divide_by_zero));
	    ଯଦି (failed != ଖାଲି)
		*failed = TRUE;
	}
	ଯଦି (n1 == 0)
	    result = VARNUM_MIN; // similar to NaN
	ଅନ୍ୟଥା ଯଦି (n1 < 0)
	    result = -VARNUM_MAX;
	ଅନ୍ୟଥା
	    result = VARNUM_MAX;
    }
    ଅନ୍ୟଥା ଯଦି (n1 == VARNUM_MIN && n2 == -1)
    {
	// specific case: trying to do VARNUM_MIN / -1 results in a positive
	// number that doesn't fit in varnumber_T and causes an FPE
	result = VARNUM_MAX;
    }
    ଅନ୍ୟଥା
	result = n1 / n2;

    ଫେରନ୍ତୁ result;
}

/*
 * Return "n1" modulus "n2", taking care of dividing by zero.
 * If "failed" is not NULL set it to TRUE when dividing by zero fails.
 */
	varnumber_T
ଅଙ୍କ_modulus(varnumber_T n1, varnumber_T n2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *failed)
{
    ଯଦି (n2 == 0 && in_vim9script())
    {
	emsg(_(e_divide_by_zero));
	ଯଦି (failed != ଖାଲି)
	    *failed = TRUE;
    }
    ଫେରନ୍ତୁ (n2 == 0) ? 0 : (n1 % n2);
}

/*
 * Initialize the global and v: variables.
 */
    ଶୂନ୍ୟ
eval_init(ଶୂନ୍ୟ)
{
    evalvars_init();
    func_init();
}

#ଯଦି defined(EXITFREE)
    ଶୂନ୍ୟ
eval_clear(ଶୂନ୍ୟ)
{
    evalvars_clear();
    free_scriptnames();  // must come after evalvars_clear().
    free_locales();

    // autoloaded script names
    free_autoload_scriptnames();

    // unreferenced lists, tuples and dicts
    (ଶୂନ୍ୟ)garbage_collect(FALSE);

    // functions not garbage collected
    free_all_functions();
}
#endif

    ଶୂନ୍ୟ
fill_evalarg_from_eap(evalarg_T *evalarg, exarg_T *eap, ପୂର୍ଣ୍ଣସଂଖ୍ୟା skip)
{
    init_evalarg(evalarg);
    evalarg->eval_flags = skip ? 0 : EVAL_EVALUATE;

    ଯଦି (eap == ଖାଲି)
	ଫେରନ୍ତୁ;

    evalarg->eval_cstack = eap->cstack;
    ଯଦି (sourcing_a_script(eap) || eap->ea_getline == get_list_ଧାଡ଼ି)
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
eval_to_bool(
    char_u	*arg,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*error,
    exarg_T	*eap,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		skip,	    // only parse, don't execute
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		use_simple_function)
{
    typval_T	tv;
    varnumber_T	retval = FALSE;
    evalarg_T	evalarg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		r;

    fill_evalarg_from_eap(&evalarg, eap, skip);

    ଯଦି (skip)
	++emsg_skip;
    ଯଦି (use_simple_function)
	r = eval0_simple_funccal(arg, &tv, eap, &evalarg);
    ଅନ୍ୟଥା
	r = eval0(arg, &tv, eap, &evalarg);
    ଯଦି (r == FAIL)
	*error = TRUE;
    ଅନ୍ୟଥା
    {
	*error = FALSE;
	ଯଦି (!skip)
	{
	    ଯଦି (in_vim9script())
		retval = tv_get_bool_chk(&tv, error);
	    ଅନ୍ୟଥା
		retval = (tv_get_ସଂଖ୍ୟା_chk(&tv, error) != 0);
	    clear_tv(&tv);
	}
    }
    ଯଦି (skip)
	--emsg_skip;
    clear_evalarg(&evalarg, eap);

    ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)retval;
}

/*
 * Call eval1() and give an error message if not done at a lower level.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
eval1_emsg(char_u **arg, typval_T *rettv, exarg_T *eap)
{
    char_u	*start = *arg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_emsg_before = did_emsg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		called_emsg_before = called_emsg;
    evalarg_T	evalarg;

    fill_evalarg_from_eap(&evalarg, eap, eap != ଖାଲି && eap->skip);

    ret = eval1(arg, rettv, &evalarg);
    ଯଦି (ret == FAIL)
    {
	// Report the invalid expression unless the expression evaluation has
	// been cancelled due to an aborting error, an interrupt, or an
	// exception, or we already gave a more specific error.
	// Also check called_emsg for when using assert_fails().
	ଯଦି (!aborting() && did_emsg == did_emsg_before
					  && called_emsg == called_emsg_before)
	    semsg(_(e_invalid_expression_str), start);
    }
    clear_evalarg(&evalarg, eap);
    ଫେରନ୍ତୁ ret;
}

/*
 * Return whether a typval is a valid expression to pass to eval_expr_typval()
 * or eval_expr_to_bool().  An empty string returns FALSE;
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
eval_ପଦସମଷ୍ଟି_valid_arg(typval_T *tv)
{
    ଫେରନ୍ତୁ tv->v_type != VAR_UNKNOWN
	    && (tv->v_type != VAR_STRING
		  || (tv->vval.v_string != ଖାଲି && *tv->vval.v_string != NUL));
}

/*
 * When calling eval_expr_typval() many times we only need one funccall_T.
 * Returns NULL when no funccall_T is to be used.
 * When returning non-NULL remove_funccal() must be called later.
 */
    funccall_T *
eval_ପଦସମଷ୍ଟି_get_funccal(typval_T *ପଦସମଷ୍ଟି, typval_T *rettv)
{
    ଯଦି (ପଦସମଷ୍ଟି->v_type != VAR_PARTIAL)
	ଫେରନ୍ତୁ ଖାଲି;

    partial_T *partial = ପଦସମଷ୍ଟି->vval.v_partial;
    ଯଦି (partial == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    ଯଦି (partial->pt_func == ଖାଲି
	    || partial->pt_func->uf_def_status == UF_NOT_COMPILED)
	ଫେରନ୍ତୁ ଖାଲି;

    ଫେରନ୍ତୁ create_funccal(partial->pt_func, rettv);
}

/*
 * Evaluate a partial.
 * Pass arguments "argv[argc]".
 * "fc_arg" is from eval_expr_get_funccal() or NULL;
 * Return the result in "rettv" and OK or FAIL.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
eval_ପଦସମଷ୍ଟି_partial(
    typval_T	*ପଦସମଷ୍ଟି,
    typval_T	*argv,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		argc,
    funccall_T	*fc_arg,
    typval_T	*rettv)
{
    partial_T	*partial = ପଦସମଷ୍ଟି->vval.v_partial;

    ଯଦି (partial == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (partial->pt_func != ଖାଲି
			&& partial->pt_func->uf_def_status != UF_NOT_COMPILED)
    {
	funccall_T	*fc = fc_arg != ଖାଲି ? fc_arg
				: create_funccal(partial->pt_func, rettv);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		r;

	ଯଦି (fc == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;

	// Shortcut to call a compiled function with minimal overhead.
	ଯଦି (partial->pt_obj != ଖାଲି)
	    partial->pt_obj->obj_refcount++;
	r = call_def_function(partial->pt_func, argc, argv, DEF_USE_PT_ARGV,
					partial, partial->pt_obj, fc, rettv);
	ଯଦି (fc_arg == ଖାଲି)
	    remove_funccal();
	ଯଦି (r == FAIL)
	    ଫେରନ୍ତୁ FAIL;
    }
    ଅନ୍ୟଥା
    {
	char_u		*s = partial_name(partial);
	funcexe_T	funcexe;

	ଯଦି (s == ଖାଲି || *s == NUL)
	    ଫେରନ୍ତୁ FAIL;

	CLEAR_FIELD(funcexe);
	funcexe.fe_evaluate = TRUE;
	funcexe.fe_partial = partial;
	ଯଦି (call_func(s, -1, rettv, argc, argv, &funcexe) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
    }

    ଫେରନ୍ତୁ OK;
}

/*
 * Evaluate an expression which is a function.
 * Pass arguments "argv[argc]".
 * Return the result in "rettv" and OK or FAIL.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
eval_ପଦସମଷ୍ଟି_func(
    typval_T	*ପଦସମଷ୍ଟି,
    typval_T	*argv,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		argc,
    typval_T	*rettv)
{
    funcexe_T	funcexe;
    char_u	buf[NUMBUFLEN];
    char_u	*s;

    ଯଦି (ପଦସମଷ୍ଟି->v_type == VAR_FUNC)
	s = ପଦସମଷ୍ଟି->vval.v_string;
    ଅନ୍ୟଥା
	s = tv_get_string_buf_chk_strict(ପଦସମଷ୍ଟି, buf, in_vim9script());
    ଯଦି (s == ଖାଲି || *s == NUL)
	ଫେରନ୍ତୁ FAIL;

    CLEAR_FIELD(funcexe);
    funcexe.fe_evaluate = TRUE;
    ଯଦି (call_func(s, -1, rettv, argc, argv, &funcexe) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    ଫେରନ୍ତୁ OK;
}

/*
 * Evaluate an expression, which is a string.
 * Return the result in "rettv" and OK or FAIL.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
eval_ପଦସମଷ୍ଟି_string(
    typval_T	*ପଦସମଷ୍ଟି,
    typval_T	*rettv)
{
    char_u	*s;
    char_u	buf[NUMBUFLEN];

    s = tv_get_string_buf_chk_strict(ପଦସମଷ୍ଟି, buf, in_vim9script());
    ଯଦି (s == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    s = skipwhite(s);
    ଯଦି (eval1_emsg(&s, rettv, ଖାଲି) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (*skipwhite(s) != NUL)  // check for trailing chars after expr
    {
	clear_tv(rettv);
	semsg(_(e_invalid_expression_str), s);
	ଫେରନ୍ତୁ FAIL;
    }

    ଫେରନ୍ତୁ OK;
}

/*
 * Evaluate an expression, which can be a function, partial or string.
 * Pass arguments "argv[argc]".
 * If "want_func" is TRUE treat a string as a function name, not an expression.
 * "fc_arg" is from eval_expr_get_funccal() or NULL;
 * Return the result in "rettv" and OK or FAIL.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
eval_ପଦସମଷ୍ଟି_typval(
	typval_T    *ପଦସମଷ୍ଟି,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    want_func,
	typval_T    *argv,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    argc,
	funccall_T  *fc_arg,
	typval_T    *rettv)
{
    ଯଦି (ପଦସମଷ୍ଟି->v_type == VAR_PARTIAL)
	ଫେରନ୍ତୁ eval_ପଦସମଷ୍ଟି_partial(ପଦସମଷ୍ଟି, argv, argc, fc_arg, rettv);
    ଯଦି (ପଦସମଷ୍ଟି->v_type == VAR_INSTR)
	ଫେରନ୍ତୁ exe_typval_instr(ପଦସମଷ୍ଟି, rettv);
    ଯଦି (ପଦସମଷ୍ଟି->v_type == VAR_FUNC || want_func)
	ଫେରନ୍ତୁ eval_ପଦସମଷ୍ଟି_func(ପଦସମଷ୍ଟି, argv, argc, rettv);

    ଫେରନ୍ତୁ eval_ପଦସମଷ୍ଟି_string(ପଦସମଷ୍ଟି, rettv);
}

/*
 * Like eval_to_bool() but using a typval_T instead of a string.
 * Works for string, funcref and partial.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
eval_ପଦସମଷ୍ଟି_to_bool(typval_T *ପଦସମଷ୍ଟି, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *error)
{
    typval_T	rettv;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		res;

    ଯଦି (eval_ପଦସମଷ୍ଟି_typval(ପଦସମଷ୍ଟି, FALSE, ଖାଲି, 0, ଖାଲି, &rettv) == FAIL)
    {
	*error = TRUE;
	ଫେରନ୍ତୁ FALSE;
    }
    res = (tv_get_bool_chk(&rettv, error) != 0);
    clear_tv(&rettv);
    ଫେରନ୍ତୁ res;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		skip)	    // only parse, don't execute
{
    typval_T	tv;
    char_u	*retval;
    evalarg_T	evalarg;

    fill_evalarg_from_eap(&evalarg, eap, skip);
    ଯଦି (skip)
	++emsg_skip;
    ଯଦି (eval0(arg, &tv, eap, &evalarg) == FAIL || skip)
	retval = ଖାଲି;
    ଅନ୍ୟଥା
    {
	retval = vim_strsave(tv_get_string(&tv));
	clear_tv(&tv);
    }
    ଯଦି (skip)
	--emsg_skip;
    clear_evalarg(&evalarg, eap);

    ଫେରନ୍ତୁ retval;
}

/*
 * Initialize "evalarg" for use.
 */
    ଶୂନ୍ୟ
init_evalarg(evalarg_T *evalarg)
{
    CLEAR_POINTER(evalarg);
    ga_init2(&evalarg->eval_tofree_ga, ଆକାର(char_u *), 20);
}

/*
 * If "evalarg->eval_tofree" is not NULL free it later.
 * Caller is expected to overwrite "evalarg->eval_tofree" next.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
free_eval_tofree_later(evalarg_T *evalarg)
{
    ଯଦି (evalarg->eval_tofree == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (ga_grow(&evalarg->eval_tofree_ga, 1) == OK)
	((char_u **)evalarg->eval_tofree_ga.ga_data)
	    [evalarg->eval_tofree_ga.ga_len++]
	    = evalarg->eval_tofree;
    ଅନ୍ୟଥା
	vim_free(evalarg->eval_tofree);
}

/*
 * After using "evalarg" filled from "eap": free the memory.
 */
    ଶୂନ୍ୟ
clear_evalarg(evalarg_T *evalarg, exarg_T *eap)
{
    ଯଦି (evalarg == ଖାଲି)
	ଫେରନ୍ତୁ;

    garray_T *etga = &evalarg->eval_tofree_ga;

    ଯଦି (evalarg->eval_tofree != ଖାଲି || evalarg->eval_using_cmdline)
    {
	ଯଦି (eap != ଖାଲି)
	{
	    // We may need to keep the original command line, e.g. for
	    // ":let" it has the variable names.  But we may also need
	    // the new one, "nextcmd" points into it.  Keep both.
	    vim_free(eap->cmdline_tofree);
	    eap->cmdline_tofree = *eap->cmdlinep;

	    ଯଦି (evalarg->eval_using_cmdline && etga->ga_len > 0)
	    {
		// "nextcmd" points into the last line in eval_tofree_ga,
		// need to keep it around.
		--etga->ga_len;
		*eap->cmdlinep = ((char_u **)etga->ga_data)[etga->ga_len];
		vim_free(evalarg->eval_tofree);
	    }
	    ଅନ୍ୟଥା
		*eap->cmdlinep = evalarg->eval_tofree;
	}
	ଅନ୍ୟଥା
	    vim_free(evalarg->eval_tofree);
	evalarg->eval_tofree = ଖାଲି;
    }

    ga_clear_strings(etga);
    VIM_CLEAR(evalarg->eval_tofree_lambda);
}

/*
 * Skip over an expression at "*pp".
 * Return FAIL for an error, OK otherwise.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
skip_ପଦସମଷ୍ଟି(char_u **pp, evalarg_T *evalarg)
{
    typval_T	rettv;

    *pp = skipwhite(*pp);
    ଫେରନ୍ତୁ eval1(pp, &rettv, evalarg);
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
skip_ପଦସମଷ୍ଟି_concatenate(
	char_u	    **arg,
	char_u	    **start,
	char_u	    **end,
	evalarg_T   *evalarg)
{
    typval_T	rettv;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		res;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		vim9script = in_vim9script();
    garray_T    *gap = evalarg == ଖାଲି ? ଖାଲି : &evalarg->eval_ga;
    garray_T    *freegap = evalarg == ଖାଲି ? ଖାଲି : &evalarg->eval_freega;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_flags = evalarg == ଖାଲି ? 0 : evalarg->eval_flags;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		evaluate = evalarg == ଖାଲି
			       ? FALSE : (evalarg->eval_flags & EVAL_EVALUATE);

    ଯଦି (vim9script && evaluate
	       && (evalarg->eval_cookie != ଖାଲି || evalarg->eval_cctx != ଖାଲି))
    {
	ga_init2(gap, ଆକାର(char_u *), 10);
	// leave room for "start"
	ଯଦି (ga_grow(gap, 1) == OK)
	    ++gap->ga_len;
	ga_init2(freegap, ଆକାର(char_u *), 10);
    }
    *start = *arg;

    // Don't evaluate the expression.
    ଯଦି (evalarg != ଖାଲି)
	evalarg->eval_flags &= ~EVAL_EVALUATE;
    *arg = skipwhite(*arg);
    res = eval1(arg, &rettv, evalarg);
    *end = *arg;
    ଯଦି (evalarg != ଖାଲି)
	evalarg->eval_flags = save_flags;

    ଯଦି (vim9script && evaluate
	    && (evalarg->eval_cookie != ଖାଲି || evalarg->eval_cctx != ଖାଲି))
    {
	ଯଦି (evalarg->eval_ga.ga_len == 1)
	{
	    // just the one line, no need to concatenate
	    ga_clear(gap);
	    gap->ga_itemsize = 0;
	}
	ଅନ୍ୟଥା
	{
	    char_u	    *p;
	    size_t	    endoff = STRLEN(*arg);

	    // Line breaks encountered, concatenate all the lines.
	    *((char_u **)gap->ga_data) = *start;
	    p = ga_concat_strings(gap, " ");

	    // free the lines only when using getsourceline()
	    ଯଦି (evalarg->eval_cookie != ଖାଲି)
	    {
		// Do not free the first line, the caller can still use it.
		*((char_u **)gap->ga_data) = ଖାଲି;
		// Do not free the last line, "arg" points into it, free it
		// later.  Also free "eval_tofree" later if needed.
		free_eval_tofree_later(evalarg);
		evalarg->eval_tofree =
				    ((char_u **)gap->ga_data)[gap->ga_len - 1];
		((char_u **)gap->ga_data)[gap->ga_len - 1] = ଖାଲି;
		ga_clear_strings(gap);
		ga_clear(freegap);
	    }
	    ଅନ୍ୟଥା
	    {
		ga_clear(gap);

		// free lines that were explicitly marked for freeing
		ga_clear_strings(freegap);
	    }

	    gap->ga_itemsize = 0;
	    ଯଦି (p == ଖାଲି)
		ଫେରନ୍ତୁ FAIL;
	    *start = p;
	    vim_free(evalarg->eval_tofree_lambda);
	    evalarg->eval_tofree_lambda = p;
	    // Compute "end" relative to the end.
	    *end = *start + STRLEN(*start) - endoff;
	}
    }

    ଫେରନ୍ତୁ res;
}

/*
 * Convert "tv" to a string.
 * When "join_list" is TRUE convert a List or a Tuple into a sequence of lines.
 * Returns an allocated string (NULL when out of memory).
 */
    char_u *
typval2string(typval_T *tv, ପୂର୍ଣ୍ଣସଂଖ୍ୟା join_list)
{
    garray_T	ga;
    char_u	*retval = ଖାଲି;

    ଯଦି (join_list && (tv->v_type == VAR_LIST || tv->v_type == VAR_TUPLE))
    {
	ଯଦି (tv->v_type == VAR_LIST)
	{
	    ga_init2(&ga, ଆକାର(ଅକ୍ଷର), 80);
	    ଯଦି (tv->vval.v_list != ଖାଲି)
	    {
		list_join(&ga, tv->vval.v_list, (char_u *)"\n", TRUE, FALSE,
									0);
		ଯଦି (tv->vval.v_list->lv_len > 0)
		    ga_append(&ga, NL);
	    }
	    ga_append(&ga, NUL);
	    retval = (char_u *)ga.ga_data;
	}
	ଅନ୍ୟଥା
	{
	    // tuple
	    ga_init2(&ga, ଆକାର(ଅକ୍ଷର), 80);
	    ଯଦି (tv->vval.v_tuple != ଖାଲି)
	    {
		tuple_join(&ga, tv->vval.v_tuple, (char_u *)"\n", TRUE, FALSE,
									0);
		ଯଦି (TUPLE_LEN(tv->vval.v_tuple) > 0)
		    ga_append(&ga, NL);
	    }
	    ga_append(&ga, NUL);
	    retval = (char_u *)ga.ga_data;
	}
    }
    ଅନ୍ୟଥା ଯଦି (tv->v_type == VAR_LIST
	    || tv->v_type == VAR_TUPLE
	    || tv->v_type == VAR_DICT)
    {
	char_u	*tofree;
	char_u	numbuf[NUMBUFLEN];

	retval = tv2string(tv, &tofree, numbuf, 0);
	// Make a copy if we have a value but it's not in allocated memory.
	ଯଦି (retval != ଖାଲି && tofree == ଖାଲି)
	    retval = vim_strsave(retval);
    }
    ଅନ୍ୟଥା
	retval = vim_strsave(tv_get_string(tv));
    ଫେରନ୍ତୁ retval;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		join_list,
    exarg_T	*eap,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		use_simple_function)
{
    typval_T	tv;
    char_u	*retval;
    evalarg_T	evalarg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		r;

    fill_evalarg_from_eap(&evalarg, eap, eap != ଖାଲି && eap->skip);
    ଯଦି (use_simple_function)
	r = eval0_simple_funccal(arg, &tv, ଖାଲି, &evalarg);
    ଅନ୍ୟଥା
	r = eval0(arg, &tv, ଖାଲି, &evalarg);
    ଯଦି (r == FAIL)
	retval = ଖାଲି;
    ଅନ୍ୟଥା
    {
	retval = typval2string(&tv, join_list);
	clear_tv(&tv);
    }
    clear_evalarg(&evalarg, ଖାଲି);

    ଫେରନ୍ତୁ retval;
}

    char_u *
eval_to_string(
    char_u	*arg,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		join_list,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		use_simple_function)
{
    ଫେରନ୍ତୁ eval_to_string_eap(arg, join_list, ଖାଲି, use_simple_function);
}

/*
 * Call eval_to_string() without using current local variables and using
 * textlock.  When "use_sandbox" is TRUE use the sandbox.
 * Use legacy Vim script syntax.
 */
    char_u *
eval_to_string_safe(
    char_u	*arg,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		use_sandbox,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		keep_script_version,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		use_simple_function)
{
    char_u	*retval;
    funccal_entry_T funccal_entry;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_sc_version = current_sctx.sc_version;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_garbage = may_garbage_collect;

    ଯଦି (!keep_script_version)
	current_sctx.sc_version = 1;
    save_funccal(&funccal_entry);
    ଯଦି (use_sandbox)
	++sandbox;
    ++textlock;
    may_garbage_collect = FALSE;
    retval = eval_to_string(arg, FALSE, use_simple_function);
    ଯଦି (use_sandbox)
	--sandbox;
    --textlock;
    may_garbage_collect = save_garbage;
    restore_funccal();
    current_sctx.sc_version = save_sc_version;
    ଫେରନ୍ତୁ retval;
}

/*
 * Top level evaluation function, returning a number.
 * Evaluates "expr" silently.
 * Returns -1 for an error.
 */
    varnumber_T
eval_to_ସଂଖ୍ୟା(char_u *ପଦସମଷ୍ଟି, ପୂର୍ଣ୍ଣସଂଖ୍ୟା use_simple_function)
{
    typval_T	rettv;
    varnumber_T	retval;
    char_u	*p = skipwhite(ପଦସମଷ୍ଟି);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		r = NOTDONE;

    ++emsg_off;

    ଯଦି (use_simple_function)
	r = may_call_simple_func(ପଦସମଷ୍ଟି, &rettv);
    ଯଦି (r == NOTDONE)
	r = eval1(&p, &rettv, &EVALARG_EVALUATE);
    ଯଦି (r == FAIL)
	retval = -1;
    ଅନ୍ୟଥା
    {
	retval = tv_get_ସଂଖ୍ୟା_chk(&rettv, ଖାଲି);
	clear_tv(&rettv);
    }
    --emsg_off;

    ଫେରନ୍ତୁ retval;
}

/*
 * Top level evaluation function.
 * Returns an allocated typval_T with the result.
 * Returns NULL when there is an error.
 */
    typval_T *
eval_ପଦସମଷ୍ଟି(char_u *arg, exarg_T *eap)
{
    ଫେରନ୍ତୁ eval_ପଦସମଷ୍ଟି_ext(arg, eap, FALSE);
}

    typval_T *
eval_ପଦସମଷ୍ଟି_ext(char_u *arg, exarg_T *eap, ପୂର୍ଣ୍ଣସଂଖ୍ୟା use_simple_function)
{
    typval_T	*tv;
    evalarg_T	evalarg;

    fill_evalarg_from_eap(&evalarg, eap, eap != ଖାଲି && eap->skip);

    tv = ALLOC_ONE(typval_T);
    ଯଦି (tv != ଖାଲି)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	r = NOTDONE;

	ଯଦି (use_simple_function)
	    r = eval0_simple_funccal(arg, tv, eap, &evalarg);
	ଯଦି (r == NOTDONE)
	    r = eval0(arg, tv, eap, &evalarg);

	ଯଦି (r == FAIL)
	    VIM_CLEAR(tv);
    }

    clear_evalarg(&evalarg, eap);
    ଫେରନ୍ତୁ tv;
}

/*
 * "*arg" points to what can be a function name in the form of "import.Name" or
 * "Funcref".  Return the name of the function.  Set "tofree" to something that
 * was allocated.
 * If "verbose" is FALSE no errors are given.
 * Return NULL for any failure.
 */
    ସ୍ଥିର char_u *
deref_function_name(
	    char_u	**arg,
	    char_u	**tofree,
	    evalarg_T	*evalarg,
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		verbose)
{
    typval_T	ref;
    char_u	*name = *arg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_flags = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		evaluate = evalarg != ଖାଲି
				      && (evalarg->eval_flags & EVAL_EVALUATE);

    ref.v_type = VAR_UNKNOWN;
    ଯଦି (evalarg != ଖାଲି)
    {
	// need to evaluate this to get an import, like in "a.Func"
	save_flags = evalarg->eval_flags;
	evalarg->eval_flags |= EVAL_EVALUATE;
    }
    ଯଦି (eval9(arg, &ref, evalarg, FALSE) == FAIL)
    {
	dictitem_T	*v;

	// If <SID>VarName was used it would not be found, try another way.
	v = find_var_also_in_script(name, ଖାଲି, FALSE);
	ଯଦି (v == ଖାଲି)
	{
	    name = ଖାଲି;
	    ଯାଅ theend;
	}
	copy_tv(&v->di_tv, &ref);
    }
    ଯଦି (*skipwhite(*arg) != NUL)
    {
	ଯଦି (verbose)
	    semsg(_(e_trailing_characters_str), *arg);
	name = ଖାଲି;
    }
    ଅନ୍ୟଥା ଯଦି (ref.v_type == VAR_FUNC && ref.vval.v_string != ଖାଲି)
    {
	name = ref.vval.v_string;
	ref.vval.v_string = ଖାଲି;
	*tofree = name;
    }
    ଅନ୍ୟଥା ଯଦି (ref.v_type == VAR_PARTIAL && ref.vval.v_partial != ଖାଲି)
    {
	ଯଦି (ref.vval.v_partial->pt_argc > 0
		|| ref.vval.v_partial->pt_dict != ଖାଲି)
	{
	    ଯଦି (verbose)
		emsg(_(e_cannot_use_partial_here));
	    name = ଖାଲି;
	}
	ଅନ୍ୟଥା
	{
	    name = vim_strsave(partial_name(ref.vval.v_partial));
	    *tofree = name;
	}
    }
    ଅନ୍ୟଥା ଯଦି (evaluate)
    {
	ଯଦି (verbose)
	    semsg(_(e_not_callable_type_str), name);
	name = ଖାଲି;
    }

theend:
    clear_tv(&ref);
    ଯଦି (evalarg != ଖାଲି)
	evalarg->eval_flags = save_flags;
    ଫେରନ୍ତୁ name;
}

/*
 * Call some Vim script function and return the result in "*rettv".
 * Uses argv[0] to argv[argc - 1] for the function arguments.  argv[argc]
 * should have type VAR_UNKNOWN.
 * Returns OK or FAIL.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
call_vim_function(
    char_u      *func,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		argc,
    typval_T	*argv,
    typval_T	*rettv)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret;
    funcexe_T	funcexe;
    char_u	*arg;
    char_u	*name;
    char_u	*tofree = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ignore_errors;

    rettv->v_type = VAR_UNKNOWN;		// clear_tv() uses this
    CLEAR_FIELD(funcexe);
    funcexe.fe_firstline = curwin->w_cursor.lnum;
    funcexe.fe_lastline = curwin->w_cursor.lnum;
    funcexe.fe_evaluate = TRUE;

    // The name might be "import.Func" or "Funcref".  We don't know, we need to
    // ignore errors for an undefined name.  But we do want errors when an
    // autoload script has errors.  Guess that when there is a dot in the name
    // showing errors is the right choice.
    ignore_errors = vim_strchr(func, '.') == ଖାଲି;
    arg = func;
    ଯଦି (ignore_errors)
	++emsg_off;
    name = deref_function_name(&arg, &tofree, &EVALARG_EVALUATE, FALSE);
    ଯଦି (ignore_errors)
	--emsg_off;
    ଯଦି (name == ଖାଲି)
	name = func;

    ret = call_func(name, -1, rettv, argc, argv, &funcexe);

    ଯଦି (ret == FAIL)
	clear_tv(rettv);
    vim_free(tofree);

    ଫେରନ୍ତୁ ret;
}

/*
 * Call Vim script function "func" and return the result as a string.
 * Uses "argv[0]" to "argv[argc - 1]" for the function arguments. "argv[argc]"
 * should have type VAR_UNKNOWN.
 * Returns NULL when calling the function fails.
 */
    ଶୂନ୍ୟ *
call_func_retstr(
    char_u      *func,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		argc,
    typval_T	*argv)
{
    typval_T	rettv;
    char_u	*retval;

    ଯଦି (call_vim_function(func, argc, argv, &rettv) == FAIL)
	ଫେରନ୍ତୁ ଖାଲି;

    retval = vim_strsave(tv_get_string(&rettv));
    clear_tv(&rettv);
    ଫେରନ୍ତୁ retval;
}

/*
 * Call Vim script function "func" and return the result as a List.
 * Uses "argv" and "argc" as call_func_retstr().
 * Returns NULL when there is something wrong.
 * Gives an error when the returned value is not a list.
 */
    ଶୂନ୍ୟ *
call_func_retlist(
    char_u      *func,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		argc,
    typval_T	*argv)
{
    typval_T	rettv;

    ଯଦି (call_vim_function(func, argc, argv, &rettv) == FAIL)
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (rettv.v_type != VAR_LIST)
    {
	semsg(_(e_custom_list_completion_function_does_not_return_list_but_str),
		vartype_name(rettv.v_type));
	clear_tv(&rettv);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    ଫେରନ୍ତୁ rettv.vval.v_list;
}

#ଯଦି defined(FEAT_FOLDING)
/*
 * Evaluate "arg", which is 'foldexpr'.
 * Note: caller must set "curwin" to match "arg".
 * Returns the foldlevel, and any character preceding it in "*cp".  Doesn't
 * give error messages.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
eval_foldexpr(win_T *wp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *cp)
{
    char_u	*arg;
    typval_T	tv;
    varnumber_T	retval;
    char_u	*s;
    sctx_T	saved_sctx = current_sctx;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		use_sandbox = was_set_insecurely(wp, (char_u *)"foldexpr",
								    OPT_LOCAL);

    arg = skipwhite(wp->w_p_fde);
    current_sctx = wp->w_p_script_ctx[WV_FDE];

    ++emsg_off;
    ଯଦି (use_sandbox)
	++sandbox;
    ++textlock;
    *cp = NUL;

    // Evaluate the expression.  If the expression is "FuncName()" call the
    // function directly.
    ଯଦି (eval0_simple_funccal(arg, &tv, ଖାଲି, &EVALARG_EVALUATE) == FAIL)
	retval = 0;
    ଅନ୍ୟଥା
    {
	// If the result is a number, just return the number.
	ଯଦି (tv.v_type == VAR_ସଂଖ୍ୟା)
	    retval = tv.vval.v_ସଂଖ୍ୟା;
	ଅନ୍ୟଥା ଯଦି (tv.v_type != VAR_STRING || tv.vval.v_string == ଖାଲି)
	    retval = 0;
	ଅନ୍ୟଥା
	{
	    // If the result is a string, check if there is a non-digit before
	    // the number.
	    s = tv.vval.v_string;
	    ଯଦି (*s != NUL && !VIM_ISDIGIT(*s) && *s != '-')
		*cp = *s++;
	    retval = atol((ଅକ୍ଷର *)s);
	}
	clear_tv(&tv);
    }
    --emsg_off;
    ଯଦି (use_sandbox)
	--sandbox;
    --textlock;
    clear_evalarg(&EVALARG_EVALUATE, ଖାଲି);
    current_sctx = saved_sctx;

    ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)retval;
}
#endif

#ifdef LOG_LOCKVAR
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା flag_string_S
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    flag;
    ଅକ୍ଷର    *str;
} flag_string_T;

    ସ୍ଥିର ଅକ୍ଷର *
flags_tostring(ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags, flag_string_T *_fstring, ଅକ୍ଷର *buf, size_t n)
{
    ଅକ୍ଷର *p = buf;
    *p = NUL;
    ପାଇଁ (flag_string_T *fstring = _fstring; fstring->flag; ++fstring)
    {
	ଯଦି ((fstring->flag & flags) != 0)
	{
	    size_t len = STRLEN(fstring->str);
	    ଯଦି (n > p - buf + len + 7)
	    {
		STRCAT(p, fstring->str);
		p += len;
		STRCAT(p, " ");
		++p;
	    }
	    ଅନ୍ୟଥା
	    {
		STRCAT(buf, "...");
		ଭାଙ୍ଗ;
	    }
	}
    }
    ଫେରନ୍ତୁ buf;
}

flag_string_T glv_flag_strings[] = {
    { GLV_QUIET,		"QUIET" },
    { GLV_NO_AUTOLOAD,		"NO_AUTOLOAD" },
    { GLV_READ_ONLY,		"READ_ONLY" },
    { GLV_NO_DECL,		"NO_DECL" },
    { GLV_COMPILING,		"COMPILING" },
    { GLV_ASSIGN_WITH_OP,	"ASSIGN_WITH_OP" },
    { GLV_PREFER_FUNC,		"PREFER_FUNC" },
    { 0,			ଖାଲି }
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
    ସ୍ଥିର ଶୂନ୍ୟ
fill_lval_from_lval_ମୂଳ(lval_T *lp, lval_ମୂଳ_T *lr)
{
#ifdef LOG_LOCKVAR
    ch_log(ଖାଲି, "LKVAR: fill_lval_from_lval_root(): name %s, tv %p",
						lp->ll_name, (ଶୂନ୍ୟ*)lr->lr_tv);
#endif
    ଯଦି (lr->lr_tv == ଖାଲି)
	ଫେରନ୍ତୁ;
    ଯଦି (!lr->lr_is_arg && lr->lr_tv->v_type == VAR_CLASS)
    {
	ଯଦି (lr->lr_tv->vval.v_class != ଖାଲି)
	{
	    // Special special case. Look for a bare class variable reference.
	    class_T	*cl = lr->lr_tv->vval.v_class;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		m_idx;
	    ocmember_T	*m = class_member_lookup(cl, lp->ll_name,
					lp->ll_name_end - lp->ll_name, &m_idx);
	    ଯଦି (m != ଖାଲି)
	    {
		// Assuming "inside class" since bare reference.
		lp->ll_class = lr->lr_tv->vval.v_class;
		lp->ll_oi = m_idx;
		lp->ll_valtype = m->ocm_type;
		lp->ll_tv = &lp->ll_class->class_members_tv[m_idx];
#ifdef LOG_LOCKVAR
		ch_log(ଖାଲି, "LKVAR:    ... class member %s.%s",
					lp->ll_class->class_name.string, lp->ll_name);
#endif
		ଫେରନ୍ତୁ;
	    }
	}
    }

#ifdef LOG_LOCKVAR
    ch_log(ଖାଲି, "LKVAR:    ... type: %s", vartype_name(lr->lr_tv->v_type));
#endif
    lp->ll_tv = lr->lr_tv;
    lp->ll_is_ମୂଳ = TRUE;
}

/*
 * Check if the class has permission to access the member.
 * Returns OK or FAIL.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_lval_check_access(
    class_T	*cl_exec,   // executing class, NULL if :def or script level
    class_T	*cl,	    // class which contains the member
    ocmember_T	*om,	    // member being accessed
    char_u	*p,	    // char after member name
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags)	    // GLV flags to check if writing to lval
{
#ifdef LOG_LOCKVAR
    ch_log(ଖାଲି, "LKVAR: get_lval_check_access(), cl_exec %p, cl %p, %c",
						(ଶୂନ୍ୟ*)cl_exec, (ଶୂନ୍ୟ*)cl, *p);
#endif
    ଯଦି (cl_exec != ଖାଲି && cl_exec == cl)
	ଫେରନ୍ତୁ OK;

    ଅକ୍ଷର *ସନ୍ଦେଶ = ଖାଲି;
    ବାଛ (om->ocm_access)
    {
	ସ୍ଥିତି VIM_ACCESS_PRIVATE:
	    ସନ୍ଦେଶ = e_cannot_access_protected_variable_str;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VIM_ACCESS_READ:
	    // If [idx] or .key following, read only OK.
	    ଯଦି (*p == '[' || *p == '.')
		ଭାଙ୍ଗ;
	    ଯଦି ((flags & GLV_READ_ONLY) == 0)
	    {
		ଯଦି (IS_ENUM(cl))
		{
		    ଯଦି (om->ocm_type->tt_type == VAR_OBJECT)
			semsg(_(e_enumvalue_str_cannot_be_modified),
				cl->class_name.string, om->ocm_name.string);
		    ଅନ୍ୟଥା
			ସନ୍ଦେଶ = e_variable_is_not_writable_str;
		}
		ଅନ୍ୟଥା
		    ସନ୍ଦେଶ = e_variable_is_not_writable_str;
	    }
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VIM_ACCESS_ALL:
	    ଭାଙ୍ଗ;
    }
    ଯଦି (ସନ୍ଦେଶ != ଖାଲି)
    {
	emsg_var_cl_define(ସନ୍ଦେଶ, om->ocm_name.string, 0, cl);
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
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
    ସ୍ଥିର char_u *
get_lval_imported(
    lval_T	*lp,
    scid_T	imp_sid,
    char_u	*p,
    dictitem_T	**dip,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fne_flags)
{
    ufunc_T	*ufunc;
    type_T	*type = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cc;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		rc = FAIL;

    p = skipwhite(p);

    import_check_sourced_sid(&imp_sid);
    lp->ll_sid = imp_sid;
    lp->ll_name = p;
    p = find_name_end(lp->ll_name, ଖାଲି, ଖାଲି, fne_flags);
    lp->ll_name_end = p;

    // check the item is exported
    cc = *p;
    *p = NUL;
    ଯଦି (find_exported(imp_sid, lp->ll_name, &ufunc, &type, ଖାଲି, ଖାଲି,
								TRUE) == -1)
	ଯାଅ failed;

    // Get the typval for the exported item
    hashtab_T *ht = &SCRIPT_VARS(imp_sid);
    ଯଦି (ht == ଖାଲି)
	ଯାଅ failed;

    dictitem_T *di = find_var_in_ht(ht, 0, lp->ll_name, TRUE);
    ଯଦି (di == ଖାଲି)
	// script is autoloaded.  So variable will be found later
	ଯାଅ success;

    *dip = di;

    // Check whether the variable is writable.
    svar_T *sv = find_typval_in_script(&di->di_tv, imp_sid, FALSE);
    ଯଦି (sv != ଖାଲି && sv->sv_const != 0)
    {
	semsg(_(e_cannot_change_readonly_variable_str), lp->ll_name);
	ଯାଅ failed;
    }

    // check whether variable is locked
    ଯଦି (ମୂଲ୍ୟ_check_lock(di->di_tv.v_lock, lp->ll_name, FALSE))
	ଯାଅ failed;

    lp->ll_tv = &di->di_tv;
    lp->ll_valtype = type;

success:
    rc = OK;

failed:
    *p = cc;
    ଫେରନ୍ତୁ rc == OK ? p : ଖାଲି;
}

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_lval_dict_item(
    lval_T	*lp,
    char_u	*name,
    char_u	*ଚାବି,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len,
    char_u	**ଚାବି_end,
    typval_T	*var1,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		unlet,
    typval_T	*rettv)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		quiet = flags & GLV_QUIET;
    char_u	*p = *ଚାବି_end;

    ଯଦି (len == -1)
    {
	// "[key]": get key from "var1"
	ଚାବି = tv_get_string_chk(var1);	// is number or string
	ଯଦି (ଚାବି == ଖାଲି)
	    ଫେରନ୍ତୁ GLV_FAIL;
    }
    lp->ll_list = ଖାଲି;
    lp->ll_list = ଖାଲି;
    lp->ll_blob = ଖାଲି;
    lp->ll_object = ଖାଲି;
    lp->ll_class = ଖାଲି;
    lp->ll_tuple = ଖାଲି;

    // a NULL dict is equivalent with an empty dict
    ଯଦି (lp->ll_tv->vval.v_dict == ଖାଲି)
    {
	lp->ll_tv->vval.v_dict = dict_alloc();
	ଯଦି (lp->ll_tv->vval.v_dict == ଖାଲି)
	    ଫେରନ୍ତୁ GLV_FAIL;
	++lp->ll_tv->vval.v_dict->dv_refcount;
    }
    lp->ll_dict = lp->ll_tv->vval.v_dict;

    lp->ll_di = dict_find(lp->ll_dict, ଚାବି, len);

    // When assigning to a scope dictionary check that a function and
    // variable name is valid (only variable name unless it is l: or
    // g: dictionary). Disallow overwriting a builtin function.
    ଯଦି (rettv != ଖାଲି && lp->ll_dict->dv_scope != 0)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା prevval;

	ଯଦି (len != -1)
	{
	    prevval = ଚାବି[len];
	    ଚାବି[len] = NUL;
	}
	ଅନ୍ୟଥା
	    prevval = 0; // avoid compiler warning
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା wrong = (lp->ll_dict->dv_scope == VAR_DEF_SCOPE
		&& (rettv->v_type == VAR_FUNC
		    || rettv->v_type == VAR_PARTIAL)
		&& var_wrong_func_name(ଚାବି, lp->ll_di == ଖାଲି))
	    || !valid_varname(ଚାବି, -1, TRUE);
	ଯଦି (len != -1)
	    ଚାବି[len] = prevval;
	ଯଦି (wrong)
	    ଫେରନ୍ତୁ GLV_FAIL;
    }

    ଯଦି (lp->ll_valtype != ଖାଲି)
	// use the type of the member
	lp->ll_valtype = lp->ll_valtype->tt_member;

    ଯଦି (lp->ll_di == ଖାଲି)
    {
	// Can't add "v:" or "a:" variable.
	ଯଦି (lp->ll_dict == get_vimvar_dict()
		|| &lp->ll_dict->dv_hashtab == get_funccal_args_ht())
	{
	    semsg(_(e_illegal_variable_name_str), name);
	    ଫେରନ୍ତୁ GLV_FAIL;
	}

	// Key does not exist in dict: may need to add it.
	ଯଦି (*p == '[' || *p == '.' || unlet)
	{
	    ଯଦି (!quiet)
		semsg(_(e_ଚାବି_not_present_in_dictionary_str), ଚାବି);
	    ଫେରନ୍ତୁ GLV_FAIL;
	}
	ଯଦି (len == -1)
	    lp->ll_newkey = vim_strsave(ଚାବି);
	ଅନ୍ୟଥା
	    lp->ll_newkey = vim_strnsave(ଚାବି, len);
	ଯଦି (lp->ll_newkey == ଖାଲି)
	    p = ଖାଲି;

	*ଚାବି_end = p;
	ଫେରନ୍ତୁ GLV_STOP;
    }
    // existing variable, need to check if it can be changed
    ଅନ୍ୟଥା ଯଦି ((flags & GLV_READ_ONLY) == 0
	    && (var_check_ro(lp->ll_di->di_flags, name, FALSE)
		|| var_check_lock(lp->ll_di->di_flags, name, FALSE)))
	ଫେରନ୍ତୁ GLV_FAIL;

    lp->ll_tv = &lp->ll_di->di_tv;

    ଫେରନ୍ତୁ GLV_OK;
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_lval_blob(
    lval_T	*lp,
    typval_T	*var1,
    typval_T	*var2,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		empty1,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		quiet)
{
    long	bloblen = blob_len(lp->ll_tv->vval.v_blob);

    lp->ll_list = ଖାଲି;
    lp->ll_dict = ଖାଲି;
    lp->ll_object = ଖାଲି;
    lp->ll_class = ଖାଲି;
    lp->ll_tuple = ଖାଲି;

    // Get the number and item for the only or first index of a List or Tuple.
    ଯଦି (empty1)
	lp->ll_n1 = 0;
    ଅନ୍ୟଥା
	// is number or string
	lp->ll_n1 = (long)tv_get_ସଂଖ୍ୟା(var1);

    ଯଦି (check_blob_index(bloblen, lp->ll_n1, quiet) == FAIL)
	ଫେରନ୍ତୁ FAIL;
    ଯଦି (lp->ll_range && !lp->ll_empty2)
    {
	lp->ll_n2 = (long)tv_get_ସଂଖ୍ୟା(var2);
	ଯଦି (check_blob_range(bloblen, lp->ll_n1, lp->ll_n2, quiet) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
    }

    ଯଦି (!lp->ll_range)
	// Indexing a single byte in a blob.  So the rhs type is a
	// number.
	lp->ll_valtype = &t_ସଂଖ୍ୟା;

    lp->ll_blob = lp->ll_tv->vval.v_blob;
    lp->ll_tv = ଖାଲି;

    ଫେରନ୍ତୁ OK;
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_lval_list(
    lval_T	*lp,
    typval_T	*var1,
    typval_T	*var2,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		empty1,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		quiet)
{
    /*
     * Get the number and item for the only or first index of the List.
     */
    ଯଦି (empty1)
	lp->ll_n1 = 0;
    ଅନ୍ୟଥା
	// is number or string
	lp->ll_n1 = (long)tv_get_ସଂଖ୍ୟା(var1);

    lp->ll_dict = ଖାଲି;
    lp->ll_object = ଖାଲି;
    lp->ll_class = ଖାଲି;
    lp->ll_tuple = ଖାଲି;
    lp->ll_list = lp->ll_tv->vval.v_list;
    lp->ll_li = check_range_index_one(lp->ll_list, &lp->ll_n1,
				(flags & GLV_ASSIGN_WITH_OP) == 0, quiet);
    ଯଦି (lp->ll_li == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (lp->ll_valtype != ଖାଲି && !lp->ll_range)
    {
	// use the type of the member
	ଯଦି (lp->ll_valtype->tt_member != ଖାଲି)
	    lp->ll_valtype = lp->ll_valtype->tt_member;
	ଅନ୍ୟଥା
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
    ଯଦି (lp->ll_range && !lp->ll_empty2)
    {
	lp->ll_n2 = (long)tv_get_ସଂଖ୍ୟା(var2);
	// is number or string
	ଯଦି (check_range_index_two(lp->ll_list,
			&lp->ll_n1, lp->ll_li, &lp->ll_n2, quiet) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
    }

    lp->ll_tv = &lp->ll_li->li_tv;

    ଫେରନ୍ତୁ OK;
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_lval_tuple(
    lval_T	*lp,
    typval_T	*idx,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		quiet)
{
    // is number or string
    lp->ll_n1 = (long)tv_get_ସଂଖ୍ୟା(idx);

    lp->ll_list = ଖାଲି;
    lp->ll_dict = ଖାଲି;
    lp->ll_blob = ଖାଲି;
    lp->ll_object = ଖାଲି;
    lp->ll_class = ଖାଲି;

    lp->ll_tuple = lp->ll_tv->vval.v_tuple;
    lp->ll_tv = tuple_find(lp->ll_tuple, lp->ll_n1);
    ଯଦି (lp->ll_tv == ଖାଲି)
    {
	ଯଦି (!quiet)
	    semsg(_(e_tuple_index_out_of_range_nr), lp->ll_n1);
	ଫେରନ୍ତୁ GLV_FAIL;
    }

    // use the type of the member
    ଯଦି (lp->ll_valtype != ଖାଲି)
    {
	ଯଦି (lp->ll_valtype != ଖାଲି
		&& lp->ll_valtype->tt_type == VAR_TUPLE
		&& lp->ll_valtype->tt_argcount == 1)
	{
	    // a variadic tuple or a single item tuple
	    ଯଦି (lp->ll_valtype->tt_flags & TTFLAG_VARARGS)
		lp->ll_valtype = lp->ll_valtype->tt_args[0]->tt_member;
	    ଅନ୍ୟଥା
		lp->ll_valtype = lp->ll_valtype->tt_args[0];
	}
	ଅନ୍ୟଥା
	    // If the LHS member type is not known (VAR_ANY), then get it from
	    // the tuple item (after indexing)
	    lp->ll_valtype = typval2type(lp->ll_tv, get_copyID(),
					&lp->ll_type_list, TVTT_DO_MEMBER);
    }

    ଫେରନ୍ତୁ GLV_OK;
}

/*
 * Get a class or object lval method in class "cl".  The 'key' argument points
 * to the method name and 'key_end' points to the character after 'key'.
 * 'v_type' is VAR_CLASS or VAR_OBJECT.
 *
 * The method index, method function pointer and method type are returned in
 * "lp".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_lval_oc_method(
    lval_T	*lp,
    class_T	*cl,
    char_u	*ଚାବି,
    char_u	**ଚାବି_end,
    vartype_T	v_type)
{
    // Look for a method with this name.
    // round 1: class functions (skipped for an object)
    // round 2: object methods
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା round = v_type == VAR_OBJECT ? 2 : 1; round <= 2; ++round)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	m_idx;
	ufunc_T	*fp;

	fp = method_lookup(cl, round == 1 ? VAR_CLASS : VAR_OBJECT,
						ଚାବି, *ଚାବି_end - ଚାବି, &m_idx);
	lp->ll_oi = m_idx;

	// process generic method (if present)
	ଯଦି (fp && (fp = eval_generic_func(fp, ଚାବି, ଚାବି_end)) == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;

	ଯଦି (fp != ଖାଲି)
	{
	    lp->ll_ufunc = fp;
	    lp->ll_valtype = fp->uf_func_type;
	    ଭାଙ୍ଗ;
	}
    }

    ଫେରନ୍ତୁ OK;
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_lval_oc_variable(
    lval_T	*lp,
    class_T	*cl,
    char_u	*ଚାବି,
    char_u	*ଚାବି_end,
    vartype_T	v_type,
    class_T	*cl_exec,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		m_idx;
    ocmember_T	*om;

    om = member_lookup(cl, v_type, ଚାବି, ଚାବି_end - ଚାବି, &m_idx);
    lp->ll_oi = m_idx;
    ଯଦି (om == ଖାଲି)
	ଫେରନ୍ତୁ OK;

    // Check variable is accessible
    ଯଦି (get_lval_check_access(cl_exec, cl, om, ଚାବି_end, flags) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    // When lhs is used to modify the variable, check it is not a read-only
    // variable.
    ଯଦି ((flags & GLV_READ_ONLY) == 0 && (*ଚାବି_end != '.' && *ଚାବି_end != '[')
	    && oc_var_check_ro(cl, om))
	ଫେରନ୍ତୁ FAIL;

    lp->ll_valtype = om->ocm_type;

    ଯଦି (v_type == VAR_OBJECT)
	lp->ll_tv = ((typval_T *)(lp->ll_tv->vval.v_object + 1)) + m_idx;
    ଅନ୍ୟଥା
	lp->ll_tv = &cl->class_members_tv[m_idx];

    ଫେରନ୍ତୁ OK;
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_lval_class_or_obj(
    lval_T	*lp,
    char_u	*ଚାବି,
    char_u	**ଚାବି_end,
    vartype_T	v_type,
    class_T	*cl_exec,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		quiet)
{
    lp->ll_dict = ଖାଲି;
    lp->ll_list = ଖାଲି;
    lp->ll_tuple = ଖାଲି;

    class_T *cl;
    ଯଦି (v_type == VAR_OBJECT)
    {
	ଯଦି (lp->ll_tv->vval.v_object == ଖାଲି)
	{
	    ଯଦି (!quiet)
		emsg(_(e_using_null_object));
	    ଫେରନ୍ତୁ FAIL;
	}
	cl = lp->ll_tv->vval.v_object->obj_class;
	lp->ll_object = lp->ll_tv->vval.v_object;
    }
    ଅନ୍ୟଥା
    {
	cl = lp->ll_tv->vval.v_class;
	lp->ll_object = ଖାଲି;
    }
    lp->ll_class = cl;

    ଯଦି (cl == ଖାଲି)
	// TODO: what if class is NULL?
	ଫେରନ୍ତୁ OK;

    lp->ll_valtype = ଖାଲି;

    ଯଦି (flags & GLV_PREFER_FUNC)
	ଯଦି (get_lval_oc_method(lp, cl, ଚାବି, ଚାବି_end, v_type) == FAIL)
	    ଫେରନ୍ତୁ FAIL;

    // Look for object/class member variable
    ଯଦି (lp->ll_valtype == ଖାଲି)
    {
	ଯଦି (get_lval_oc_variable(lp, cl, ଚାବି, *ଚାବି_end, v_type, cl_exec, flags)
								== FAIL)
	    ଫେରନ୍ତୁ FAIL;
    }

    ଯଦି (lp->ll_valtype == ଖାଲି)
    {
	member_not_found_ସନ୍ଦେଶ(cl, v_type, ଚାବି, *ଚାବି_end - ଚାବି);
	ଫେରନ୍ତୁ FAIL;
    }

    ଫେରନ୍ତୁ OK;
}

/*
 * Check whether dot (".") is allowed after the variable "name" with type
 * "v_type".  Only Dict, Class and Object types support a dot after the name.
 * Returns TRUE if dot is allowed after the name.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
dot_allowed_after_type(char_u *name, vartype_T v_type, ପୂର୍ଣ୍ଣସଂଖ୍ୟା quiet)
{
    ଯଦି (v_type != VAR_DICT && v_type != VAR_OBJECT && v_type != VAR_CLASS)
    {
	ଯଦି (!quiet)
	    semsg(_(e_dot_not_allowed_after_str_str),
		    vartype_name(v_type), name);
	ଫେରନ୍ତୁ FALSE;
    }

    ଫେରନ୍ତୁ TRUE;
}

/*
 * Check whether left bracket ("[") is allowed after the variable "name" with
 * type "v_type".  Only Dict, List, Tuple and Blob types support a bracket
 * after the variable name.  Returns TRUE if bracket is allowed after the name.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
bracket_allowed_after_type(char_u *name, vartype_T v_type, ପୂର୍ଣ୍ଣସଂଖ୍ୟା quiet)
{
    ଯଦି (v_type == VAR_CLASS || v_type == VAR_OBJECT)
    {
	ଯଦି (!quiet)
	    semsg(_(e_index_not_allowed_after_str_str),
		    vartype_name(v_type), name);
	ଫେରନ୍ତୁ FALSE;
    }

    ଫେରନ୍ତୁ TRUE;
}

/*
 * Check whether the variable "name" with type "v_type" can be followed by an
 * index.  Only Dict, List, Tuple, Blob, Object and Class types support
 * indexing.  Returns TRUE if indexing is allowed after the name.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
index_allowed_after_type(char_u *name, vartype_T v_type, ପୂର୍ଣ୍ଣସଂଖ୍ୟା quiet)
{
    ଯଦି (v_type != VAR_LIST
	    && v_type != VAR_TUPLE
	    && v_type != VAR_DICT
	    && v_type != VAR_BLOB
	    && v_type != VAR_OBJECT
	    && v_type != VAR_CLASS)
    {
	ଯଦି (!quiet)
	    semsg(_(e_index_not_allowed_after_str_str),
		    vartype_name(v_type), name);
	ଫେରନ୍ତୁ FALSE;
    }

    ଫେରନ୍ତୁ TRUE;
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
    ସ୍ଥିର char_u *
get_lval_subscript(
    lval_T	*lp,
    char_u	*p,
    char_u	*name,
    typval_T	*rettv,
    hashtab_T	*ht,
    dictitem_T	*v,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		unlet,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags,	    // GLV_ values
    class_T	*cl_exec)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		quiet = flags & GLV_QUIET;
    char_u	*ଚାବି = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    typval_T	var1;
    typval_T	var2;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		empty1 = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		rc = FAIL;

    /*
     * Loop until no more [idx] or .key is following.
     */
    var1.v_type = VAR_UNKNOWN;
    var2.v_type = VAR_UNKNOWN;

    ଯେତେବେଳେ (*p == '[' || (*p == '.' && p[1] != '=' && p[1] != '.'))
    {
	vartype_T v_type = lp->ll_tv->v_type;

	ଯଦି (*p == '.' && !dot_allowed_after_type(name, v_type, quiet))
	    ଯାଅ done;

	ଯଦି (*p == '[' && !bracket_allowed_after_type(name, v_type, quiet))
	    ଯାଅ done;

	ଯଦି (!index_allowed_after_type(name, v_type, quiet))
	    ଯାଅ done;

	// A NULL list/blob works like an empty list/blob, allocate one now.
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା r = OK;
	ଯଦି (v_type == VAR_LIST && lp->ll_tv->vval.v_list == ଖାଲି)
	    r = rettv_list_alloc(lp->ll_tv);
	ଅନ୍ୟଥା ଯଦି (v_type == VAR_BLOB && lp->ll_tv->vval.v_blob == ଖାଲି)
	    r = rettv_blob_alloc(lp->ll_tv);
	ଯଦି (r == FAIL)
	    ଯାଅ done;

	ଯଦି (lp->ll_range)
	{
	    ଯଦି (!quiet)
		emsg(_(e_slice_must_come_last));
	    ଯାଅ done;
	}
#ifdef LOG_LOCKVAR
	ch_log(ଖାଲି, "LKVAR: get_lval() loop: p: %s, type: %s", p,
		vartype_name(v_type));
#endif

	ଯଦି (current_script_is_vim9() && lp->ll_valtype == ଖାଲି
		&& v != ଖାଲି
		&& lp->ll_tv == &v->di_tv
		&& ht != ଖାଲି && ht == get_script_local_ht())
	{
	    svar_T  *sv = find_typval_in_script(lp->ll_tv, 0, TRUE);

	    // Vim9 script local variable: get the type
	    ଯଦି (sv != ଖାଲି)
	    {
		lp->ll_valtype = sv->sv_type;
#ifdef LOG_LOCKVAR
		ch_log(ଖାଲି, "LKVAR:    ... loop: vim9 assign type: %s",
			vartype_name(lp->ll_valtype->tt_type));
#endif
	    }
	}

	len = -1;
	ଯଦି (*p == '.')
	{
	    ଚାବି = p + 1;

	    ପାଇଁ (len = 0; ASCII_ISALNUM(ଚାବି[len]) || ଚାବି[len] == '_'; ++len)
		;
	    ଯଦି (len == 0)
	    {
		ଯଦି (!quiet)
		    emsg(_(e_cannot_use_empty_ଚାବି_for_dictionary));
		ଯାଅ done;
	    }
	    p = ଚାବି + len;
	}
	ଅନ୍ୟଥା
	{
	    // Get the index [expr] or the first index [expr: ].
	    p = skipwhite(p + 1);
	    ଯଦି (*p == ':')
		empty1 = TRUE;
	    ଅନ୍ୟଥା
	    {
		empty1 = FALSE;
		ଯଦି (eval1(&p, &var1, &EVALARG_EVALUATE) == FAIL)  // recursive!
		    ଯାଅ done;
		ଯଦି (tv_get_string_chk(&var1) == ଖାଲି)
		    // not a number or string
		    ଯାଅ done;
		p = skipwhite(p);
	    }

	    // Optionally get the second index [ :expr].
	    ଯଦି (*p == ':')
	    {
		ଯଦି (v_type == VAR_DICT)
		{
		    ଯଦି (!quiet)
			emsg(_(e_cannot_slice_dictionary));
		    ଯାଅ done;
		}
		ଯଦି (v_type == VAR_TUPLE)
		{
		    ଯଦି (!quiet)
			emsg(_(e_cannot_slice_tuple));
		    ଯାଅ done;
		}
		ଯଦି (rettv != ଖାଲି
			&& !(rettv->v_type == VAR_LIST
			    && rettv->vval.v_list != ଖାଲି)
			&& !(rettv->v_type == VAR_BLOB
			    && rettv->vval.v_blob != ଖାଲି))
		{
		    ଯଦି (!quiet)
			emsg(_(e_slice_requires_list_or_blob_ମୂଲ୍ୟ));
		    ଯାଅ done;
		}
		p = skipwhite(p + 1);
		ଯଦି (*p == ']')
		    lp->ll_empty2 = TRUE;
		ଅନ୍ୟଥା
		{
		    lp->ll_empty2 = FALSE;
		    // recursive!
		    ଯଦି (eval1(&p, &var2, &EVALARG_EVALUATE) == FAIL)
			ଯାଅ done;
		    ଯଦି (tv_get_string_chk(&var2) == ଖାଲି)
			// not a number or string
			ଯାଅ done;
		}
		lp->ll_range = TRUE;
	    }
	    ଅନ୍ୟଥା
		lp->ll_range = FALSE;

	    ଯଦି (*p != ']')
	    {
		ଯଦି (!quiet)
		    emsg(_(e_missing_closing_square_brace));
		ଯାଅ done;
	    }

	    // Skip to past ']'.
	    ++p;
	}
#ifdef LOG_LOCKVAR
	ଯଦି (len == -1)
	    ch_log(ଖାଲି, "LKVAR:    ... loop: p: %s, '[' key: %s", p,
		    empty1 ? ":" : (ଅକ୍ଷର*)tv_get_string(&var1));
	ଅନ୍ୟଥା
	    ch_log(ଖାଲି, "LKVAR:    ... loop: p: %s, '.' key: %s", p, ଚାବି);
#endif

	ଯଦି (v_type == VAR_DICT)
	{
	    glv_status_T glv_status;

	    glv_status = get_lval_dict_item(lp, name, ଚାବି, len, &p, &var1,
							flags, unlet, rettv);
	    ଯଦି (glv_status == GLV_FAIL)
		ଯାଅ done;
	    ଯଦି (glv_status == GLV_STOP)
		ଭାଙ୍ଗ;
	}
	ଅନ୍ୟଥା ଯଦି (v_type == VAR_BLOB)
	{
	    ଯଦି (get_lval_blob(lp, &var1, &var2, empty1, quiet) == FAIL)
		ଯାଅ done;

	    ଭାଙ୍ଗ;
	}
	ଅନ୍ୟଥା ଯଦି (v_type == VAR_LIST)
	{
	    ଯଦି (get_lval_list(lp, &var1, &var2, empty1, flags, quiet) == FAIL)
		ଯାଅ done;
	}
	ଅନ୍ୟଥା ଯଦି (v_type == VAR_TUPLE)
	{
	    ଯଦି (get_lval_tuple(lp, &var1, quiet) == FAIL)
		ଯାଅ done;
	}
	ଅନ୍ୟଥା  // v_type == VAR_CLASS || v_type == VAR_OBJECT
	{
	    ଯଦି (get_lval_class_or_obj(lp, ଚାବି, &p, v_type, cl_exec, flags,
							quiet) == FAIL)
		ଯାଅ done;
	}

	clear_tv(&var1);
	clear_tv(&var2);
	var1.v_type = VAR_UNKNOWN;
	var2.v_type = VAR_UNKNOWN;
    }

    ଯଦି (lp->ll_tuple != ଖାଲି && (flags & GLV_READ_ONLY) == 0)
    {
	ଯଦି (!quiet)
	    emsg(_(e_tuple_is_immutable));
	ଯାଅ done;
    }

    rc = OK;

done:
    clear_tv(&var1);
    clear_tv(&var2);
    ଫେରନ୍ତୁ rc == OK ? p : ଖାଲି;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		unlet,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		skip,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags,	    // GLV_ values
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fne_flags)  // flags for find_name_end()
{
    char_u	*p;
    char_u	*ପଦସମଷ୍ଟି_start, *ପଦସମଷ୍ଟି_end;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cc;
    dictitem_T	*v = ଖାଲି;
    hashtab_T	*ht = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		quiet = flags & GLV_QUIET;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		writing = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		vim9script = in_vim9script();
    class_T	*cl_exec = ଖାଲି;    // class that is executing, or NULL.

#ifdef LOG_LOCKVAR
    ଯଦି (lval_ମୂଳ == ଖାଲି)
	ch_log(ଖାଲି, "LKVAR: get_lval(): name: %s, lval_root (nil)", name);
    ଅନ୍ୟଥା
	ch_log(ଖାଲି, "LKVAR: get_lval(): name: %s, lr_tv %p lr_is_arg %d",
			name, (ଶୂନ୍ୟ*)lval_ମୂଳ->lr_tv, lval_ମୂଳ->lr_is_arg);
    ଅକ୍ଷର buf[80];
    ch_log(ଖାଲି, "LKVAR:    ...: GLV flags: %s",
		    flags_tostring(flags, glv_flag_strings, buf, ଆକାର(buf)));
#endif

    // Clear everything in "lp".
    CLEAR_POINTER(lp);
    ga_init2(&lp->ll_type_list, ଆକାର(type_T *), 10);

    ଯଦି (skip || (flags & GLV_COMPILING))
    {
	// When skipping or compiling just find the end of the name.
	lp->ll_name = name;
	lp->ll_name_end = find_name_end(name, ଖାଲି, ଖାଲି,
						      FNE_INCL_BR | fne_flags);
	ଫେରନ୍ତୁ lp->ll_name_end;
    }

    // Cannot use "s:var" at the Vim9 script level.  "s: type" is OK.
    ଯଦି (vim9script && at_script_level()
		  && name[0] == 's' && name[1] == ':' && !VIM_ISWHITE(name[2]))
    {
	semsg(_(e_cannot_use_s_colon_in_vim9_script_str), name);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    // Find the end of the name.
    p = find_name_end(name, &ପଦସମଷ୍ଟି_start, &ପଦସମଷ୍ଟି_end, fne_flags);
    lp->ll_name_end = p;
    ଯଦି (ପଦସମଷ୍ଟି_start != ଖାଲି)
    {
	// Don't expand the name when we already know there is an error.
	ଯଦି (unlet && !VIM_ISWHITE(*p) && !ends_excmd(*p)
						    && *p != '[' && *p != '.')
	{
	    semsg(_(e_trailing_characters_str), p);
	    ଫେରନ୍ତୁ ଖାଲି;
	}

	lp->ll_exp_name = make_expanded_name(name, ପଦସମଷ୍ଟି_start, ପଦସମଷ୍ଟି_end, p);
	ଯଦି (lp->ll_exp_name == ଖାଲି)
	{
	    // Report an invalid expression in braces, unless the
	    // expression evaluation has been cancelled due to an
	    // aborting error, an interrupt, or an exception.
	    ଯଦି (!aborting() && !quiet)
	    {
		emsg_severe = TRUE;
		semsg(_(e_invalid_argument_str), name);
		ଫେରନ୍ତୁ ଖାଲି;
	    }
	}
	lp->ll_name = lp->ll_exp_name;
    }
    ଅନ୍ୟଥା
    {
	lp->ll_name = name;

	ଯଦି (vim9script)
	{
	    // "a: type" is declaring variable "a" with a type, not "a:".
	    // However, "g:[key]" is indexing a dictionary.
	    ଯଦି (p == name + 2 && p[-1] == ':' && *p != '[')
	    {
		--p;
		lp->ll_name_end = p;
	    }
	    ଯଦି (*skipwhite(p) == ':')
	    {
		char_u	    *tp = skipwhite(p + 1);

		ଯଦି (is_scoped_variable(name))
		{
		    semsg(_(e_cannot_use_type_with_this_variable_str), name);
		    ଫେରନ୍ତୁ ଖାଲି;
		}
		ଯଦି (VIM_ISWHITE(*p))
		{
		    semsg(_(e_no_white_space_allowed_before_colon_str), p);
		    ଫେରନ୍ତୁ ଖାଲି;
		}
		ଯଦି (tp == p + 1 && !quiet)
		{
		    semsg(_(e_white_space_required_after_str_str), ":", p);
		    ଫେରନ୍ତୁ ଖାଲି;
		}
		ଯଦି (!SCRIPT_ID_VALID(current_sctx.sc_sid))
		{
		    semsg(_(e_using_type_not_in_script_context_str), p);
		    ଫେରନ୍ତୁ ଖାଲି;
		}
		ଯଦି (vim9script && (flags & GLV_NO_DECL) &&
			!(flags & GLV_FOR_LOOP))
		{
		    // Using a type and not in a "var" declaration.
		    semsg(_(e_trailing_characters_str), p);
		    ଫେରନ୍ତୁ ଖାଲି;
		}


		// parse the type after the name
		lp->ll_type = parse_type(&tp,
			       &SCRIPT_ITEM(current_sctx.sc_sid)->sn_type_list,
			       ଖାଲି, ଖାଲି, !quiet);
		ଯଦି (!quiet && (lp->ll_type == ଖାଲି
			    || !valid_declaration_type(lp->ll_type)))
		    ଫେରନ୍ତୁ ଖାଲି;
		lp->ll_name_end = tp;
	    }
	    // TODO: check inside class?
	}
    }
    ଯଦି (lp->ll_name == ଖାଲି)
	ଫେରନ୍ତୁ p;

    ଯଦି (*p == '.')
    {
	// In legacy script, when a local variable and import exists with this name,
	// prioritize local variable over imports to avoid conflicts.
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା var_exists = FALSE;
	ଯଦି (!vim9script)
	{
	    cc = *p;
	    *p = NUL;
	    hashtab_T *local_ht = get_funccal_local_ht();
	    ଯଦି (local_ht != ଖାଲି)
	    {
		hashitem_T *hi = hash_find(local_ht, lp->ll_name);
		ଯଦି (!HASHITEM_EMPTY(hi))
		    var_exists = TRUE;
	    }
	    *p = cc;
	}

	ଯଦି (!var_exists)
	{
	    imported_T *import = find_imported(lp->ll_name, p - lp->ll_name, TRUE);
	    ଯଦି (import != ଖାଲି)
	    {
		p++;	// skip '.'
		p = get_lval_imported(lp, import->imp_sid, p, &v, fne_flags);
		ଯଦି (p == ଖାଲି)
		    ଫେରନ୍ତୁ ଖାଲି;
	    }
	}
    }

    // Without [idx] or .key we are done.
    ଯଦି (*p != '[' && *p != '.')
    {
	ଯଦି (lval_ମୂଳ != ଖାଲି)
	    fill_lval_from_lval_ମୂଳ(lp, lval_ମୂଳ);
	ଫେରନ୍ତୁ p;
    }

    ଯଦି (vim9script && lval_ମୂଳ != ଖାଲି)
	cl_exec = lval_ମୂଳ->lr_cl_exec;
    ଯଦି (vim9script && lval_ମୂଳ != ଖାଲି && lval_ମୂଳ->lr_tv != ଖାଲି)
    {
	// using local variable
	lp->ll_tv = lval_ମୂଳ->lr_tv;
	v = ଖାଲି;
    }
    ଅନ୍ୟଥା ଯଦି (lp->ll_tv == ଖାଲି)
    {
	cc = *p;
	*p = NUL;
	// When we would write to the variable pass &ht and prevent autoload.
	writing = !(flags & GLV_READ_ONLY);
	v = find_var(lp->ll_name, writing ? &ht : ଖାଲି,
					 (flags & GLV_NO_AUTOLOAD) || writing);
	ଯଦି (v == ଖାଲି && !quiet)
	    semsg(_(e_undefined_variable_str), lp->ll_name);
	*p = cc;
	ଯଦି (v == ଖାଲି)
	    ଫେରନ୍ତୁ ଖାଲି;
	lp->ll_tv = &v->di_tv;
    }

    ଯଦି (vim9script && (flags & GLV_NO_DECL) == 0)
    {
	ଯଦି (!quiet)
	    semsg(_(e_variable_already_declared_str), lp->ll_name);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    // If the next character is a "." or a "[", then process the subitem.
    p = get_lval_subscript(lp, p, name, rettv, ht, v, unlet, flags, cl_exec);
    ଯଦି (p == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (vim9script && lp->ll_valtype != ଖାଲି && rettv != ଖାଲି)
    {
	where_T	    where = WHERE_INIT;

	// In a Vim9 script, do type check and make sure the variable is
	// writable.
	ଯଦି (check_typval_type(lp->ll_valtype, rettv, where) == FAIL)
	    ଫେରନ୍ତୁ ଖାଲି;
    }

    lp->ll_name_end = p;
    ଫେରନ୍ତୁ p;
}

/*
 * Clear lval "lp" that was filled by get_lval().
 */
    ଶୂନ୍ୟ
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
    ଶୂନ୍ୟ
set_var_lval(
    lval_T	*lp,
    char_u	*endp,
    typval_T	*rettv,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		copy,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags,	    // ASSIGN_CONST, ASSIGN_NO_DECL
    char_u	*op,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		var_idx)    // index for "let [a, b] = list"
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cc;
    dictitem_T	*di;

    ଯଦି (lp->ll_tv == ଖାଲି)
    {
	cc = *endp;
	*endp = NUL;
	ଯଦି (in_vim9script() && check_reserved_name(lp->ll_name, FALSE) == FAIL)
	    ଫେରନ୍ତୁ;

	ଯଦି (lp->ll_blob != ଖାଲି)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    error = FALSE;

	    ଯଦି (op != ଖାଲି && *op != '=')
	    {
		semsg(_(e_wrong_variable_type_for_str_equal), op);
		ଫେରନ୍ତୁ;
	    }
	    ଯଦି (ମୂଲ୍ୟ_check_lock(lp->ll_blob->bv_lock, lp->ll_name, FALSE))
		ଫେରନ୍ତୁ;

	    ଯଦି (lp->ll_range && rettv->v_type == VAR_BLOB)
	    {
		ଯଦି (lp->ll_empty2)
		    lp->ll_n2 = blob_len(lp->ll_blob) - 1;

		ଯଦି (blob_set_range(lp->ll_blob, lp->ll_n1, lp->ll_n2,
								rettv) == FAIL)
		    ଫେରନ୍ତୁ;
	    }
	    ଅନ୍ୟଥା
	    {
		varnumber_T	val = tv_get_ସଂଖ୍ୟା_chk(rettv, &error);
		ଯଦି (!error)
		{
		    ଯଦି (val < 0 || val > 255)
			semsg(_(e_invalid_ମୂଲ୍ୟ_for_blob_nr), val);
		    ଅନ୍ୟଥା
			blob_set_append(lp->ll_blob, lp->ll_n1, val);
		}
	    }
	}
	ଅନ୍ୟଥା ଯଦି (op != ଖାଲି && *op != '=')
	{
	    typval_T tv;

	    ଯଦି ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					     && (flags & ASSIGN_FOR_LOOP) == 0)
	    {
		emsg(_(e_cannot_modify_existing_variable));
		*endp = cc;
		ଫେରନ୍ତୁ;
	    }

	    // handle +=, -=, *=, /=, %= and .=
	    di = ଖାଲି;
	    ଯଦି (eval_variable(lp->ll_name, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(lp->ll_name),
				 lp->ll_sid, &tv, &di, EVAL_VAR_VERBOSE) == OK)
	    {
		ଯଦି (di != ଖାଲି && check_typval_is_ମୂଲ୍ୟ(&di->di_tv) == FAIL)
		{
		    clear_tv(&tv);
		    ଫେରନ୍ତୁ;
		}

		// In Vim9 script a String v: variable only takes a String.
		ଯଦି (in_vim9script() && STRNCMP(lp->ll_name, "v:", 2) == 0
			&& tv.v_type == VAR_STRING)
		{
		    where_T	where = WHERE_INIT;

		    ଯଦି (check_typval_type(&t_string, rettv, where) == FAIL)
		    {
			clear_tv(&tv);
			*endp = cc;
			ଫେରନ୍ତୁ;
		    }
		}

		ଯଦି ((di == ଖାଲି
			 || (!var_check_ro(di->di_flags, lp->ll_name, FALSE)
			   && !tv_check_lock(&di->di_tv, lp->ll_name, FALSE)))
			&& tv_op(&tv, rettv, op) == OK)
		    set_var_const(lp->ll_name, lp->ll_sid, ଖାଲି, &tv, FALSE,
				ASSIGN_NO_DECL | ASSIGN_COMPOUND_OP, 0);
		clear_tv(&tv);
	    }
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (lp->ll_type != ଖାଲି && check_typval_arg_type(lp->ll_type, rettv,
							      ଖାଲି, 0) == FAIL)
		ଫେରନ୍ତୁ;
	    set_var_const(lp->ll_name, lp->ll_sid, lp->ll_type, rettv, copy,
							       flags, var_idx);
	}
	*endp = cc;
    }
    ଅନ୍ୟଥା ଯଦି (ମୂଲ୍ୟ_check_lock(lp->ll_newkey == ଖାଲି
		? lp->ll_tv->v_lock
		: lp->ll_tv->vval.v_dict->dv_lock, lp->ll_name, FALSE))
	;
    ଅନ୍ୟଥା ଯଦି (lp->ll_range)
    {
	ଯଦି ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					     && (flags & ASSIGN_FOR_LOOP) == 0)
	{
	    emsg(_(e_cannot_lock_range));
	    ଫେରନ୍ତୁ;
	}

	(ଶୂନ୍ୟ)list_assign_range(lp->ll_list, rettv->vval.v_list,
			 lp->ll_n1, lp->ll_n2, lp->ll_empty2, op, lp->ll_name);
    }
    ଅନ୍ୟଥା
    {
	/*
	 * Assign to a List, Dictionary or Object item.
	 */
	ଯଦି ((flags & (ASSIGN_CONST | ASSIGN_FINAL))
					     && (flags & ASSIGN_FOR_LOOP) == 0)
	{
	    emsg(_(e_cannot_lock_list_or_dict));
	    ଫେରନ୍ତୁ;
	}

	ଯଦି (lp->ll_valtype != ଖାଲି
		    && check_typval_arg_type(lp->ll_valtype, rettv,
							      ଖାଲି, 0) == FAIL)
	{
	    lp->ll_name_end = ଖାଲି;
	    ଫେରନ୍ତୁ;
	}

	// If the lval is a List and the type of the list is not yet set,
	// then set the item type from the declared type of the variable.
	ଯଦି (in_vim9script() && rettv->v_type == VAR_LIST
				&& rettv->vval.v_list != ଖାଲି
				&& rettv->vval.v_list->lv_type == ଖାଲି)
	{
	    ଯଦି (lp->ll_tv->v_type == VAR_LIST
		    && lp->ll_tv->vval.v_list != ଖାଲି
		    && lp->ll_tv->vval.v_list->lv_type != ଖାଲି)
		set_tv_type(rettv, lp->ll_tv->vval.v_list->lv_type);
	}

	ଯଦି (lp->ll_newkey != ଖାଲି)
	{
	    ଯଦି (op != ଖାଲି && *op != '=')
	    {
		semsg(_(e_ଚାବି_not_present_in_dictionary_str), lp->ll_newkey);
		ଫେରନ୍ତୁ;
	    }
	    ଯଦି (dict_wrong_func_name(lp->ll_tv->vval.v_dict, rettv,
								lp->ll_newkey))
		ଫେରନ୍ତୁ;

	    // Need to add an item to the Dictionary.
	    di = dictitem_alloc(lp->ll_newkey);
	    ଯଦି (di == ଖାଲି)
		ଫେରନ୍ତୁ;
	    ଯଦି (dict_add(lp->ll_tv->vval.v_dict, di) == FAIL)
	    {
		vim_free(di);
		ଫେରନ୍ତୁ;
	    }
	    lp->ll_tv = &di->di_tv;
	}
	ଅନ୍ୟଥା ଯଦି (op != ଖାଲି && *op != '=')
	{
	    tv_op(lp->ll_tv, rettv, op);
	    ଫେରନ୍ତୁ;
	}
	ଅନ୍ୟଥା
	    clear_tv(lp->ll_tv);

	/*
	 * Assign the value to the variable or list item.
	 */
	ଯଦି (copy)
	    copy_tv(rettv, lp->ll_tv);
	ଅନ୍ୟଥା
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
tv_op_blob(typval_T *tv1, typval_T *tv2, char_u *op)
{
    ଯଦି (*op != '+' || tv2->v_type != VAR_BLOB)
	ଫେରନ୍ତୁ FAIL;

    // Blob += Blob
    ଯଦି (tv2->vval.v_blob == ଖାଲି)
	ଫେରନ୍ତୁ OK;

    ଯଦି (tv1->vval.v_blob == ଖାଲି)
    {
	tv1->vval.v_blob = tv2->vval.v_blob;
	++tv1->vval.v_blob->bv_refcount;
	ଫେରନ୍ତୁ OK;
    }

    blob_T	*b1 = tv1->vval.v_blob;
    blob_T	*b2 = tv2->vval.v_blob;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len = blob_len(b2);

    ଯଦି (len > 0 && ga_grow(&b1->bv_ga, len) == OK)
    {
	mch_memmove((char_u *)b1->bv_ga.ga_data + b1->bv_ga.ga_len,
		    (char_u *)b2->bv_ga.ga_data, (size_t)len);
	b1->bv_ga.ga_len += len;
    }
    ଅନ୍ୟଥା
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < len; i++)
	    (ଶୂନ୍ୟ)ga_append(&b1->bv_ga, blob_get(b2, i));

    ଫେରନ୍ତୁ OK;
}

/*
 * Handle "list1 += list2".
 * Returns OK or FAIL.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
tv_op_list(typval_T *tv1, typval_T *tv2, char_u *op)
{
    ଯଦି (*op != '+' || tv2->v_type != VAR_LIST)
	ଫେରନ୍ତୁ FAIL;

    // List += List
    ଯଦି (tv2->vval.v_list == ଖାଲି)
	ଫେରନ୍ତୁ OK;

    ଯଦି (tv1->vval.v_list == ଖାଲି)
    {
	tv1->vval.v_list = tv2->vval.v_list;
	++tv1->vval.v_list->lv_refcount;
    }
    ଅନ୍ୟଥା
	list_extend(tv1->vval.v_list, tv2->vval.v_list, ଖାଲି);

    ଫେରନ୍ତୁ OK;
}

/*
 * Handle number operations:
 *	nr += nr , nr -= nr , nr *=nr , nr /= nr , nr %= nr
 *
 * Returns OK or FAIL.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
tv_op_ସଂଖ୍ୟା(typval_T *tv1, typval_T *tv2, char_u *op)
{
    varnumber_T	n;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		failed = FALSE;

    n = tv_get_ସଂଖ୍ୟା(tv1);
    ଯଦି (tv2->v_type == VAR_FLOAT)
    {
	float_T f = n;

	ଯଦି (*op == '%')
	    ଫେରନ୍ତୁ FAIL;
	ବାଛ (*op)
	{
	    ସ୍ଥିତି '+': f += tv2->vval.v_float; ଭାଙ୍ଗ;
	    ସ୍ଥିତି '-': f -= tv2->vval.v_float; ଭାଙ୍ଗ;
	    ସ୍ଥିତି '*': f *= tv2->vval.v_float; ଭାଙ୍ଗ;
	    ସ୍ଥିତି '/': f /= tv2->vval.v_float; ଭାଙ୍ଗ;
	}
	clear_tv(tv1);
	tv1->v_type = VAR_FLOAT;
	tv1->vval.v_float = f;
    }
    ଅନ୍ୟଥା
    {
	ବାଛ (*op)
	{
	    ସ୍ଥିତି '+': n += tv_get_ସଂଖ୍ୟା(tv2); ଭାଙ୍ଗ;
	    ସ୍ଥିତି '-': n -= tv_get_ସଂଖ୍ୟା(tv2); ଭାଙ୍ଗ;
	    ସ୍ଥିତି '*': n *= tv_get_ସଂଖ୍ୟା(tv2); ଭାଙ୍ଗ;
	    ସ୍ଥିତି '/': n = ଅଙ୍କ_divide(n, tv_get_ସଂଖ୍ୟା(tv2), &failed); ଭାଙ୍ଗ;
	    ସ୍ଥିତି '%': n = ଅଙ୍କ_modulus(n, tv_get_ସଂଖ୍ୟା(tv2), &failed); ଭାଙ୍ଗ;
	}
	clear_tv(tv1);
	tv1->v_type = VAR_ସଂଖ୍ୟା;
	tv1->vval.v_ସଂଖ୍ୟା = n;
    }

    ଫେରନ୍ତୁ failed ? FAIL : OK;
}

/*
 * Append string "s2" to the string in "tv1".
 * Returns OK if "tv1" was grown in place, FAIL otherwise.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
grow_string_tv(typval_T *tv1, char_u *s2)
{
    ଯଦି (tv1->v_type != VAR_STRING || tv1->vval.v_string == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    size_t len1 = STRLEN(tv1->vval.v_string);
    size_t len2 = STRLEN(s2);
    char_u *p = vim_realloc(tv1->vval.v_string, len1 + len2 + 1);

    ଯଦି (p == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    mch_memmove(p + len1, s2, len2 + 1);
    tv1->vval.v_string = p;
    ଫେରନ୍ତୁ OK;
}

/*
 * Handle "str1 .= str2"
 * Returns OK or FAIL.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
tv_op_string(typval_T *tv1, typval_T *tv2, char_u *op UNUSED)
{
    char_u	numbuf[NUMBUFLEN];
    char_u	*s;
    char_u	*s2;

    ଯଦି (tv2->v_type == VAR_FLOAT)
	ଫେରନ୍ତୁ FAIL;

    // str .= str
    s2 = tv_get_string_buf(tv2, numbuf);
    ଯଦି (grow_string_tv(tv1, s2) == OK)
	ଫେରନ୍ତୁ OK;

    s = tv_get_string(tv1);
    s = concat_str(s, s2);
    clear_tv(tv1);
    tv1->v_type = VAR_STRING;
    tv1->vval.v_string = s;

    ଫେରନ୍ତୁ OK;
}

/*
 * Handle "tv1 += tv2", "tv1 -= tv2", "tv1 *= tv2", "tv1 /= tv2", "tv1 %= tv2"
 * and "tv1 .= tv2"
 * Returns OK or FAIL.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
tv_op_nr_or_string(typval_T *tv1, typval_T *tv2, char_u *op)
{
    ଯଦି (tv2->v_type == VAR_LIST)
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (vim_strchr((char_u *)"+-*/%", *op) != ଖାଲି)
	ଫେରନ୍ତୁ tv_op_ସଂଖ୍ୟା(tv1, tv2, op);

    ଫେରନ୍ତୁ tv_op_string(tv1, tv2, op);
}

/*
 * Handle "f1 += f2", "f1 -= f2", "f1 *= f2", "f1 /= f2".
 * Returns OK or FAIL.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
tv_op_float(typval_T *tv1, typval_T *tv2, char_u *op)
{
    float_T f;

    ଯଦି (*op == '%' || *op == '.'
	    || (tv2->v_type != VAR_FLOAT
		&& tv2->v_type != VAR_ସଂଖ୍ୟା
		&& tv2->v_type != VAR_STRING))
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (tv2->v_type == VAR_FLOAT)
	f = tv2->vval.v_float;
    ଅନ୍ୟଥା
	f = tv_get_ସଂଖ୍ୟା(tv2);
    ବାଛ (*op)
    {
	ସ୍ଥିତି '+': tv1->vval.v_float += f; ଭାଙ୍ଗ;
	ସ୍ଥିତି '-': tv1->vval.v_float -= f; ଭାଙ୍ଗ;
	ସ୍ଥିତି '*': tv1->vval.v_float *= f; ଭାଙ୍ଗ;
	ସ୍ଥିତି '/': tv1->vval.v_float /= f; ଭାଙ୍ଗ;
    }

    ଫେରନ୍ତୁ OK;
}

/*
 * Handle "tv1 += tv2", "tv1 -= tv2", "tv1 *= tv2", "tv1 /= tv2", "tv1 %= tv2"
 * and "tv1 .= tv2"
 * Returns OK or FAIL.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
tv_op(typval_T *tv1, typval_T *tv2, char_u *op)
{
    // Can't do anything with a Funcref or Dict on the right.
    // v:true and friends only work with "..=".
    ଯଦି (tv2->v_type == VAR_FUNC || tv2->v_type == VAR_DICT
		|| ((tv2->v_type == VAR_BOOL || tv2->v_type == VAR_SPECIAL)
							&& *op != '.'))
    {
	semsg(_(e_wrong_variable_type_for_str_equal), op);
	ଫେରନ୍ତୁ FAIL;
    }

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା retval = FAIL;
    ବାଛ (tv1->v_type)
    {
	ସ୍ଥିତି VAR_UNKNOWN:
	ସ୍ଥିତି VAR_ANY:
	ସ୍ଥିତି VAR_VOID:
	ସ୍ଥିତି VAR_DICT:
	ସ୍ଥିତି VAR_FUNC:
	ସ୍ଥିତି VAR_PARTIAL:
	ସ୍ଥିତି VAR_BOOL:
	ସ୍ଥିତି VAR_SPECIAL:
	ସ୍ଥିତି VAR_JOB:
	ସ୍ଥିତି VAR_CHANNEL:
	ସ୍ଥିତି VAR_INSTR:
	ସ୍ଥିତି VAR_OBJECT:
	ସ୍ଥିତି VAR_CLASS:
	ସ୍ଥିତି VAR_TYPEALIAS:
	ସ୍ଥିତି VAR_TUPLE:
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି VAR_BLOB:
	    retval = tv_op_blob(tv1, tv2, op);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି VAR_LIST:
	    retval = tv_op_list(tv1, tv2, op);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି VAR_ସଂଖ୍ୟା:
	ସ୍ଥିତି VAR_STRING:
	    retval = tv_op_nr_or_string(tv1, tv2, op);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି VAR_FLOAT:
	    retval = tv_op_float(tv1, tv2, op);
	    ଭାଙ୍ଗ;
    }

    ଯଦି (retval != OK)
	semsg(_(e_wrong_variable_type_for_str_equal), op);

    ଫେରନ୍ତୁ retval;
}

/*
 * Evaluate the expression used in a ":for var in expr" command.
 * "arg" points to "var".
 * Set "*errp" to TRUE for an error, FALSE otherwise;
 * Return a pointer that holds the info.  Null when there is an error.
 */
    ଶୂନ୍ୟ *
eval_for_ଧାଡ଼ି(
    char_u	*arg,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*errp,
    exarg_T	*eap,
    evalarg_T	*evalarg)
{
    forinfo_T	*fi;
    char_u	*var_list_end;
    char_u	*ପଦସମଷ୍ଟି;
    typval_T	tv;
    list_T	*l;
    tuple_T	*tuple;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		skip = !(evalarg->eval_flags & EVAL_EVALUATE);

    *errp = TRUE;	// default: there is an error

    fi = ALLOC_CLEAR_ONE(forinfo_T);
    ଯଦି (fi == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    var_list_end = skip_var_list(arg, TRUE, &fi->fi_varcount,
						     &fi->fi_semicolon, FALSE);
    ଯଦି (var_list_end == ଖାଲି)
	ଫେରନ୍ତୁ fi;

    ପଦସମଷ୍ଟି = skipwhite_and_linebreak(var_list_end, evalarg);
    ଯଦି (ପଦସମଷ୍ଟି[0] != 'i' || ପଦସମଷ୍ଟି[1] != 'n'
				  || !(ପଦସମଷ୍ଟି[2] == NUL || VIM_ISWHITE(ପଦସମଷ୍ଟି[2])))
    {
	ଯଦି (in_vim9script() && *ପଦସମଷ୍ଟି == ':' && ପଦସମଷ୍ଟି != var_list_end)
	    semsg(_(e_no_white_space_allowed_before_colon_str), ପଦସମଷ୍ଟି);
	ଅନ୍ୟଥା
	    emsg(_(e_missing_in_after_for));
	ଫେରନ୍ତୁ fi;
    }

    ଯଦି (skip)
	++emsg_skip;
    ପଦସମଷ୍ଟି = skipwhite_and_linebreak(ପଦସମଷ୍ଟି + 2, evalarg);
    ଯଦି (eval0(ପଦସମଷ୍ଟି, &tv, eap, evalarg) == OK)
    {
	*errp = FALSE;
	ଯଦି (!skip)
	{
	    ଯଦି (tv.v_type == VAR_LIST)
	    {
		l = tv.vval.v_list;
		ଯଦି (l == ଖାଲି)
		{
		    // a null list is like an empty list: do nothing
		    clear_tv(&tv);
		}
		ଅନ୍ୟଥା
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
	    ଅନ୍ୟଥା ଯଦି (tv.v_type == VAR_TUPLE)
	    {
		tuple = tv.vval.v_tuple;
		ଯଦି (tuple == ଖାଲି)
		{
		    // a null tuple is like an empty tuple: do nothing
		    clear_tv(&tv);
		}
		ଅନ୍ୟଥା
		{
		    // No need to increment the refcount, it's already set for
		    // the tuple being used in "tv".
		    fi->fi_tuple = tuple;
		    fi->fi_tuple_idx = 0;
		}
	    }
	    ଅନ୍ୟଥା ଯଦି (tv.v_type == VAR_BLOB)
	    {
		fi->fi_bi = 0;
		ଯଦି (tv.vval.v_blob != ଖାଲି)
		{
		    // Make a copy, so that the iteration still works when the
		    // blob is changed.
		    fi->fi_blob = blob_copy(tv.vval.v_blob);
		}
		clear_tv(&tv);
	    }
	    ଅନ୍ୟଥା ଯଦି (tv.v_type == VAR_STRING)
	    {
		fi->fi_byte_idx = 0;
		fi->fi_string = tv.vval.v_string;
		tv.vval.v_string = ଖାଲି;
		ଯଦି (fi->fi_string == ଖାଲି)
		    fi->fi_string = vim_strsave((char_u *)"");
	    }
	    ଅନ୍ୟଥା
	    {
		emsg(_(e_string_list_tuple_or_blob_required));
		clear_tv(&tv);
	    }
	}
    }
    ଯଦି (skip)
	--emsg_skip;
    fi->fi_break_ଗଣନା = evalarg->eval_break_ଗଣନା;

    ଫେରନ୍ତୁ fi;
}

/*
 * Used when looping over a :for line, skip the "in expr" part.
 */
    ଶୂନ୍ୟ
skip_for_ଧାଡ଼ିଗୁଡ଼ିକ(ଶୂନ୍ୟ *fi_void, evalarg_T *evalarg)
{
    forinfo_T	*fi = (forinfo_T *)fi_void;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ପାଇଁ (i = 0; i < fi->fi_break_ଗଣନା; ++i)
	eval_next_ଧାଡ଼ି(ଖାଲି, evalarg);
}

/*
 * Use the first item in a ":for" list.  Advance to the next.
 * Assign the values to the variable (list).  "arg" points to the first one.
 * Return TRUE when a valid item was found, FALSE when at end of list or
 * something wrong.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
next_for_item(ଶୂନ୍ୟ *fi_void, char_u *arg)
{
    forinfo_T	*fi = (forinfo_T *)fi_void;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		result;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flag = ASSIGN_FOR_LOOP | (in_vim9script()
			 ? (ASSIGN_FINAL
			     // first round: error if variable exists
			     | (fi->fi_bi == 0 ? 0 : ASSIGN_DECL)
			     | ASSIGN_NO_MEMBER_TYPE
			     | ASSIGN_UPDATE_BLOCK_ID)
			 : 0);
    listitem_T	*item;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		skip_assign = in_vim9script() && arg[0] == '_'
						      && !eval_isnamec(arg[1]);

    ଯଦି (fi->fi_blob != ଖାଲି)
    {
	typval_T	tv;

	ଯଦି (fi->fi_bi >= blob_len(fi->fi_blob))
	    ଫେରନ୍ତୁ FALSE;
	tv.v_type = VAR_ସଂଖ୍ୟା;
	tv.v_lock = VAR_FIXED;
	tv.vval.v_ସଂଖ୍ୟା = blob_get(fi->fi_blob, fi->fi_bi);
	++fi->fi_bi;
	ଯଦି (skip_assign)
	    ଫେରନ୍ତୁ TRUE;
	ଫେରନ୍ତୁ ex_let_vars(arg, &tv, TRUE, fi->fi_semicolon,
					    fi->fi_varcount, flag, ଖାଲି) == OK;
    }

    ଯଦି (fi->fi_string != ଖାଲି)
    {
	typval_T	tv;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;

	len = mb_ptr2len(fi->fi_string + fi->fi_byte_idx);
	ଯଦି (len == 0)
	    ଫେରନ୍ତୁ FALSE;
	tv.v_type = VAR_STRING;
	tv.v_lock = VAR_FIXED;
	tv.vval.v_string = vim_strnsave(fi->fi_string + fi->fi_byte_idx, len);
	fi->fi_byte_idx += len;
	++fi->fi_bi;
	ଯଦି (skip_assign)
	    result = TRUE;
	ଅନ୍ୟଥା
	    result = ex_let_vars(arg, &tv, TRUE, fi->fi_semicolon,
					    fi->fi_varcount, flag, ଖାଲି) == OK;
	vim_free(tv.vval.v_string);
	ଫେରନ୍ତୁ result;
    }

    ଯଦି (fi->fi_tuple != ଖାଲି)
    {
	typval_T	tv;

	ଯଦି (fi->fi_tuple_idx >= TUPLE_LEN(fi->fi_tuple))
	    ଫେରନ୍ତୁ FALSE;

	copy_tv(TUPLE_ITEM(fi->fi_tuple, fi->fi_tuple_idx), &tv);
	++fi->fi_tuple_idx;
	++fi->fi_bi;
	ଯଦି (skip_assign)
	    result = TRUE;
	ଅନ୍ୟଥା
	    result = ex_let_vars(arg, &tv, TRUE, fi->fi_semicolon,
					    fi->fi_varcount, flag, ଖାଲି) == OK;
	clear_tv(&tv);
	ଫେରନ୍ତୁ result;
    }

    item = fi->fi_lw.lw_item;
    ଯଦି (item == ଖାଲି)
	result = FALSE;
    ଅନ୍ୟଥା
    {
	fi->fi_lw.lw_item = item->li_next;
	++fi->fi_bi;
	ଯଦି (skip_assign)
	    result = TRUE;
	ଅନ୍ୟଥା
	    result = (ex_let_vars(arg, &item->li_tv, TRUE, fi->fi_semicolon,
					   fi->fi_varcount, flag, ଖାଲି) == OK);
    }
    ଫେରନ୍ତୁ result;
}

/*
 * Free the structure used to store info used by ":for".
 */
    ଶୂନ୍ୟ
free_for_info(ଶୂନ୍ୟ *fi_void)
{
    forinfo_T    *fi = (forinfo_T *)fi_void;

    ଯଦି (fi == ଖାଲି)
	ଫେରନ୍ତୁ;
    ଯଦି (fi->fi_list != ଖାଲି)
    {
	list_rem_watch(fi->fi_list, &fi->fi_lw);
	list_unref(fi->fi_list);
    }
    ଅନ୍ୟଥା ଯଦି (fi->fi_blob != ଖାଲି)
	blob_unref(fi->fi_blob);
    ଅନ୍ୟଥା ଯଦି (fi->fi_tuple != ଖାଲି)
	tuple_unref(fi->fi_tuple);
    ଅନ୍ୟଥା
	vim_free(fi->fi_string);
    vim_free(fi);
}

    ଶୂନ୍ୟ
set_context_for_expression(
    expand_T	*xp,
    char_u	*arg,
    cmdidx_T	cmdidx)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		has_ପଦସମଷ୍ଟି = cmdidx != CMD_let && cmdidx != CMD_var;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    char_u	*p;

    ଯଦି (cmdidx == CMD_let || cmdidx == CMD_var
				 || cmdidx == CMD_const || cmdidx == CMD_final)
    {
	xp->xp_context = EXPAND_USER_VARS;
	ଯଦି (vim_strpbrk(arg, (char_u *)"\"'+-*/%.=!?~|&$([<>,#") == ଖାଲି)
	{
	    // ":let var1 var2 ...": find last space.
	    ପାଇଁ (p = arg + STRLEN(arg); p >= arg; )
	    {
		xp->xp_pattern = p;
		MB_PTR_BACK(arg, p);
		ଯଦି (VIM_ISWHITE(*p))
		    ଭାଙ୍ଗ;
	    }
	    ଫେରନ୍ତୁ;
	}
    }
    ଅନ୍ୟଥା
	xp->xp_context = cmdidx == CMD_call ? EXPAND_FUNCTIONS
							  : EXPAND_EXPRESSION;
    ଯେତେବେଳେ ((xp->xp_pattern = vim_strpbrk(arg,
				  (char_u *)"\"'+-*/%.=!?~|&$([<>,#")) != ଖାଲି)
    {
	c = *xp->xp_pattern;
	ଯଦି (c == '&')
	{
	    c = xp->xp_pattern[1];
	    ଯଦି (c == '&')
	    {
		++xp->xp_pattern;
		xp->xp_context = has_ପଦସମଷ୍ଟି ? EXPAND_EXPRESSION : EXPAND_NOTHING;
	    }
	    ଅନ୍ୟଥା ଯଦି (c != ' ')
	    {
		xp->xp_context = EXPAND_SETTINGS;
		ଯଦି ((c == 'l' || c == 'g') && xp->xp_pattern[2] == ':')
		    xp->xp_pattern += 2;

	    }
	}
	ଅନ୍ୟଥା ଯଦି (c == '$')
	{
	    // environment variable
	    xp->xp_context = EXPAND_ENV_VARS;
	}
	ଅନ୍ୟଥା ଯଦି (c == '=')
	{
	    has_ପଦସମଷ୍ଟି = TRUE;
	    xp->xp_context = EXPAND_EXPRESSION;
	}
	ଅନ୍ୟଥା ଯଦି (c == '#'
		&& xp->xp_context == EXPAND_EXPRESSION)
	{
	    // Autoload function/variable contains '#'.
	    ଭାଙ୍ଗ;
	}
	ଅନ୍ୟଥା ଯଦି ((c == '<' || c == '#')
		&& xp->xp_context == EXPAND_FUNCTIONS
		&& vim_strchr(xp->xp_pattern, '(') == ଖାଲି)
	{
	    // Function name can start with "<SNR>" and contain '#'.
	    ଭାଙ୍ଗ;
	}
	ଅନ୍ୟଥା ଯଦି (has_ପଦସମଷ୍ଟି)
	{
	    ଯଦି (c == '"')	    // string
	    {
		ଯେତେବେଳେ ((c = *++xp->xp_pattern) != NUL && c != '"')
		    ଯଦି (c == '\\' && xp->xp_pattern[1] != NUL)
			++xp->xp_pattern;
		xp->xp_context = EXPAND_NOTHING;
	    }
	    ଅନ୍ୟଥା ଯଦି (c == '\'')	    // literal string
	    {
		// Trick: '' is like stopping and starting a literal string.
		ଯେତେବେଳେ ((c = *++xp->xp_pattern) != NUL && c != '\'')
		    /* skip */ ;
		xp->xp_context = EXPAND_NOTHING;
	    }
	    ଅନ୍ୟଥା ଯଦି (c == '|')
	    {
		ଯଦି (xp->xp_pattern[1] == '|')
		{
		    ++xp->xp_pattern;
		    xp->xp_context = EXPAND_EXPRESSION;
		}
		ଅନ୍ୟଥା
		    xp->xp_context = EXPAND_COMMANDS;
	    }
	    ଅନ୍ୟଥା
		xp->xp_context = EXPAND_EXPRESSION;
	}
	ଅନ୍ୟଥା
	    // Doesn't look like something valid, expand as an expression
	    // anyway.
	    xp->xp_context = EXPAND_EXPRESSION;
	arg = xp->xp_pattern;
	ଯଦି (*arg != NUL)
	    ଯେତେବେଳେ ((c = *++arg) != NUL && (c == ' ' || c == '\t'))
		/* skip */ ;
    }

    // ":exe one two" completes "two"
    ଯଦି ((cmdidx == CMD_execute
		|| cmdidx == CMD_echo
		|| cmdidx == CMD_echon
		|| cmdidx == CMD_echomsg
		|| cmdidx == CMD_echoerr
		|| cmdidx == CMD_echoconsole
		|| cmdidx == CMD_echowindow)
	    && xp->xp_context == EXPAND_EXPRESSION)
    {
	ପାଇଁ (;;)
	{
	    char_u *n = skiptowhite(arg);

	    ଯଦି (n == arg || IS_WHITE_OR_NUL(*skipwhite(n)))
		ଭାଙ୍ଗ;
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
ସ୍ଥିର regprog_T    *eval_prog_cache = ଖାଲି;
ସ୍ଥିର char_u	    *eval_prog_pat = ଖାଲି;	// pattern it was compiled for
ସ୍ଥିର char_u	    *eval_prog_enc = ଖାଲି;	// 'encoding' when compiled
ସ୍ଥିର long	    eval_prog_re;		// 'regexpengine' when compiled
// State sampled by eval_regcomp() for the eval_regfree() call that gives the
// program back: user code run in between, e.g. an object's string() method
// invoked by match() on a list, can change these options, and then the
// program must not be cached under the changed values.  eval_compile_enc is
// only ever compared against p_enc, never dereferenced.
ସ୍ଥିର long	    eval_compile_re;
ସ୍ଥିର char_u	    *eval_compile_enc;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    eval_prog_busy = 0;	// programs eval_regcomp() handed out
					// that were not given back yet
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    eval_prog_reentered = FALSE;  // eval_regcomp() ran while
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
    ସ୍ଥିର bool
eval_prog_volatile(char_u *pat)
{
    char_u *p;

    ଯଦି (STRNCMP(pat, "\\%#=", 4) == 0)
	ଫେରନ୍ତୁ true;
    ପାଇଁ (p = pat; *p != NUL; ++p)
	ଯଦି (*p == '~'
		|| (p[0] == '[' && (p[1] == ':' || p[1] == '=' || p[1] == '.'))
		|| (p[0] == '%' && (p[1] == '.'
			|| ((p[1] == '<' || p[1] == '>') && p[2] == '.'))))
	    ଫେରନ୍ତୁ true;
    ଫେରନ୍ତୁ false;
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
    ଯଦି (eval_prog_busy > 0)
	// The samples above no longer describe the program already handed
	// out: its eval_regfree() must drop it instead of caching it.
	eval_prog_reentered = TRUE;

    ଯଦି (eval_prog_cache != ଖାଲି
	    && *p_cpo == NUL
	    && eval_prog_cache->re_flags == RE_MAGIC + RE_STRING
	    && eval_prog_re == p_re
	    && STRCMP(eval_prog_pat, pat) == 0
	    && STRCMP(eval_prog_enc, p_enc) == 0)
    {
	prog = eval_prog_cache;
	// The caller now owns the program, eval_regfree() adopts the
	// program the caller ends up with.
	eval_prog_cache = ଖାଲି;
    }
    ଅନ୍ୟଥା
	prog = vim_regcomp(pat, RE_MAGIC + RE_STRING);
    ଯଦି (prog != ଖାଲି)
	++eval_prog_busy;	// paired with the eval_regfree() call
    ଫେରନ୍ତୁ prog;
}

/*
 * Free program "prog", obtained with eval_regcomp() for pattern "pat", by
 * keeping it in the cache for the next use.  "pat" must be the same string
 * the program was compiled for and must still be valid.
 */
    ଶୂନ୍ୟ
eval_regfree(char_u *pat, regprog_T *prog)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    reentered = eval_prog_reentered;

    ଯଦି (eval_prog_busy > 0 && --eval_prog_busy == 0)
	eval_prog_reentered = FALSE;
    ଯଦି (prog == ଖାଲି)
	ଫେରନ୍ତୁ;
    ଯଦି (reentered || *p_cpo != NUL
	    || eval_compile_re != p_re || eval_compile_enc != p_enc)
    {
	// 'cpoptions' is not empty, an option in the cache key changed, or
	// another program was handed out while the caller was using this
	// one: the samples may not describe this program, do not cache it.
	vim_regfree(prog);
	ଫେରନ୍ତୁ;
    }
    ଯଦି (eval_prog_pat == ଖାଲି || STRCMP(eval_prog_pat, pat) != 0
	    || STRCMP(eval_prog_enc, p_enc) != 0)
    {
	char_u	*pat_copy;
	char_u	*enc_copy;

	// A cached pattern is known not to be volatile, thus the scan is
	// only needed when installing a new key.
	ଯଦି (eval_prog_volatile(pat))
	{
	    // compiling depends on state the cache key does not cover
	    vim_regfree(prog);
	    ଫେରନ୍ତୁ;
	}
	pat_copy = vim_strsave(pat);
	enc_copy = vim_strsave(p_enc);
	ଯଦି (pat_copy == ଖାଲି || enc_copy == ଖାଲି)
	{
	    vim_free(pat_copy);
	    vim_free(enc_copy);
	    vim_regfree(prog);
	    ଫେରନ୍ତୁ;
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

#ଯଦି defined(EXITFREE)
    ଶୂନ୍ୟ
free_eval_regcomp_cache(ଶୂନ୍ୟ)
{
    vim_regfree(eval_prog_cache);
    eval_prog_cache = ଖାଲି;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
pattern_match(char_u *pat, char_u *text, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ic)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		matches = FALSE;
    char_u	*save_cpo;
    regmatch_T	regmatch;

    // avoid 'l' flag in 'cpoptions'
    save_cpo = p_cpo;
    p_cpo = empty_option;
    regmatch.regprog = eval_regcomp(pat);
    ଯଦି (regmatch.regprog != ଖାଲି)
    {
	regmatch.rm_ic = ic;
	matches = vim_regexec_nl(&regmatch, text, (colnr_T)0);
	eval_regfree(pat, regmatch.regprog);
    }
    p_cpo = save_cpo;
    ଫେରନ୍ତୁ matches;
}

/*
 * Handle a name followed by "(".  Both for just "name(arg)" and for
 * "expr->name(arg)".
 * Returns OK or FAIL.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
eval_func(
	char_u	    **arg,	// points to "(", will be advanced
	evalarg_T   *evalarg,
	char_u	    *name,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    name_len,
	typval_T    *rettv,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    flags,
	typval_T    *basetv)	// "expr" for "expr->name(arg)"
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		evaluate = flags & EVAL_EVALUATE;
    char_u	*s = name;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len = name_len;
    partial_T	*partial;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret = OK;
    type_T	*type = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		found_var = FALSE;

    ଯଦି (!evaluate)
	check_vars(s, len);

    // If "s" is the name of a variable of type VAR_FUNC
    // use its contents.
    s = deref_func_name(s, &len, &partial,
		 in_vim9script() ? &type : ଖାଲି, !evaluate, FALSE, &found_var);

    // Need to make a copy, in case evaluating the arguments makes
    // the name invalid.
    s = vim_strsave(s);
    ଯଦି (s == ଖାଲି || (evaluate && *s == NUL))
	ret = FAIL;
    ଅନ୍ୟଥା
    {
	funcexe_T funcexe;

	// Invoke the function.
	CLEAR_FIELD(funcexe);
	funcexe.fe_firstline = curwin->w_cursor.lnum;
	funcexe.fe_lastline = curwin->w_cursor.lnum;
	funcexe.fe_evaluate = evaluate;
	funcexe.fe_partial = partial;
	ଯଦି (partial != ଖାଲି)
	{
	    funcexe.fe_object = partial->pt_obj;
	    ଯଦି (funcexe.fe_object != ଖାଲି)
		++funcexe.fe_object->obj_refcount;
	}
	funcexe.fe_basetv = basetv;
	funcexe.fe_check_type = type;
	funcexe.fe_found_var = found_var;
	ଯଦି (evalarg != ଖାଲି)
	    funcexe.fe_cctx = evalarg->eval_cctx;
	ret = get_func_tv(s, len, rettv, arg, evalarg, &funcexe);
    }
    vim_free(s);

    // If evaluate is FALSE rettv->v_type was not set in
    // get_func_tv, but it's needed in handle_subscript() to parse
    // what follows. So set it here.
    ଯଦି (rettv->v_type == VAR_UNKNOWN && !evaluate && **arg == '(')
    {
	rettv->vval.v_string = ଖାଲି;
	rettv->v_type = VAR_FUNC;
    }

    // Stop the expression evaluation when immediately
    // aborting on error, or when an interrupt occurred or
    // an exception was thrown but not caught.
    ଯଦି (evaluate && aborting())
    {
	ଯଦି (ret == OK)
	    clear_tv(rettv);
	ret = FAIL;
    }
    ଫେରନ୍ତୁ ret;
}

/*
 * After a NL, skip over empty lines and comment-only lines.
 */
    ସ୍ଥିର char_u *
newline_skip_comments(char_u *arg)
{
    char_u *p = arg + 1;

    ପାଇଁ (;;)
    {
	p = skipwhite(p);

	ଯଦି (*p == NUL)
	    ଭାଙ୍ଗ;
	ଯଦି (vim9_comment_start(p))
	{
	    char_u *nl = vim_strchr(p, NL);

	    ଯଦି (nl == ଖାଲି)
		ଭାଙ୍ଗ;
	    p = nl;
	}
	ଯଦି (*p != NL)
	    ଭାଙ୍ଗ;
	++p;  // skip another NL
    }
    ଫେରନ୍ତୁ p;
}

/*
 * Get the next line source line without advancing.  But do skip over comment
 * lines.
 * Only called for Vim9 script.
 */
    ସ୍ଥିର char_u *
getline_peek_skip_comments(evalarg_T *evalarg)
{
    ପାଇଁ (;;)
    {
	char_u *next = getline_peek(evalarg->eval_getline,
							 evalarg->eval_cookie);
	char_u *p;

	ଯଦି (next == ଖାଲି)
	    ଭାଙ୍ଗ;
	p = skipwhite(next);
	ଯଦି (*p != NUL && !vim9_comment_start(p))
	    ଫେରନ୍ତୁ next;
	ଯଦି (eval_next_ଧାଡ଼ି(ଖାଲି, evalarg) == ଖାଲି)
	    ଭାଙ୍ଗ;
    }
    ଫେରନ୍ତୁ ଖାଲି;
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
eval_next_non_blank(char_u *arg, evalarg_T *evalarg, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *getnext)
{
    char_u *p = skipwhite(arg);

    *getnext = FALSE;
    ଯଦି (in_vim9script()
	    && evalarg != ଖାଲି
	    && (evalarg->eval_cookie != ଖାଲି || evalarg->eval_cctx != ଖାଲି
								   || *p == NL)
	    && (*p == NUL || *p == NL
			     || (vim9_comment_start(p) && VIM_ISWHITE(p[-1]))))
    {
	char_u *next;

	ଯଦି (*p == NL)
	    next = newline_skip_comments(p);
	ଅନ୍ୟଥା ଯଦି (evalarg->eval_cookie != ଖାଲି)
	    next = getline_peek_skip_comments(evalarg);
	ଅନ୍ୟଥା
	    next = peek_next_ଧାଡ଼ି_from_context(evalarg->eval_cctx);

	ଯଦି (next != ଖାଲି)
	{
	    *getnext = *p != NL;
	    ଫେରନ୍ତୁ skipwhite(next);
	}
    }
    ଫେରନ୍ତୁ p;
}

/*
 * To be called after eval_next_non_blank() sets "getnext" to TRUE.
 * Only called for Vim9 script.
 *
 * If "arg" is not NULL, then the caller should assign the return value to
 * "arg".
 */
    char_u *
eval_next_ଧାଡ଼ି(char_u *arg, evalarg_T *evalarg)
{
    garray_T	*gap = &evalarg->eval_ga;
    char_u	*ଧାଡ଼ି;

    ଯଦି (arg != ଖାଲି)
    {
	ଯଦି (*arg == NL)
	    ଫେରନ୍ତୁ newline_skip_comments(arg);
	// Truncate before a trailing comment, so that concatenating the lines
	// won't turn the rest into a comment.
	ଯଦି (*skipwhite(arg) == '#')
	    *arg = NUL;
    }

    ଯଦି (evalarg->eval_cookie != ଖାଲି)
	ଧାଡ଼ି = evalarg->eval_getline(0, evalarg->eval_cookie, 0,
							   GETLINE_CONCAT_ALL);
    ଅନ୍ୟଥା
	ଧାଡ଼ି = next_ଧାଡ଼ି_from_context(evalarg->eval_cctx, TRUE);
    ଯଦି (ଧାଡ଼ି == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    ++evalarg->eval_break_ଗଣନା;
    ଯଦି (gap->ga_itemsize > 0 && ga_grow(gap, 1) == OK)
    {
	char_u *p = skipwhite(ଧାଡ଼ି);

	// Going to concatenate the lines after parsing.  For an empty or
	// comment line use an empty string.
	ଯଦି (*p == NUL || vim9_comment_start(p))
	{
	    vim_free(ଧାଡ଼ି);
	    ଧାଡ଼ି = vim_strsave((char_u *)"");
	}

	((char_u **)gap->ga_data)[gap->ga_len] = ଧାଡ଼ି;
	++gap->ga_len;
    }
    ଅନ୍ୟଥା ଯଦି (evalarg->eval_cookie != ଖାଲି)
    {
	free_eval_tofree_later(evalarg);
	evalarg->eval_tofree = ଧାଡ଼ି;
    }

    // Advanced to the next line, "arg" no longer points into the previous
    // line.  The caller assigns the return value to "arg".
    // If "arg" is NULL, then the return value is discarded.  In that case,
    // "arg" still points to the previous line.  So don't reset
    // "eval_using_cmdline".
    ଯଦି (arg != ଖାଲି)
	evalarg->eval_using_cmdline = FALSE;
    ଫେରନ୍ତୁ skipwhite(ଧାଡ଼ି);
}

/*
 * Call eval_next_non_blank() and get the next line if needed.
 */
    char_u *
skipwhite_and_linebreak(char_u *arg, evalarg_T *evalarg)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    getnext;
    char_u  *p = skipwhite_and_nl(arg);

    ଯଦି (evalarg == ଖାଲି)
	ଫେରନ୍ତୁ skipwhite(arg);
    eval_next_non_blank(p, evalarg, &getnext);
    ଯଦି (getnext)
	ଫେରନ୍ତୁ eval_next_ଧାଡ଼ି(arg, evalarg);
    ଫେରନ୍ତୁ p;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
eval0(
    char_u	*arg,
    typval_T	*rettv,
    exarg_T	*eap,
    evalarg_T	*evalarg)
{
    ଫେରନ୍ତୁ eval0_retarg(arg, rettv, eap, evalarg, ଖାଲି);
}

/*
 * If "arg" is a simple function call without arguments then call it and return
 * the result.  Otherwise return NOTDONE.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
may_call_simple_func(
    char_u	*arg,
    typval_T	*rettv)
{
    char_u  *parens = (char_u *)strstr((ଅକ୍ଷର *)arg, "()");
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    r = NOTDONE;

    // If the expression is "FuncName()" then we can skip a lot of overhead.
    ଯଦି (parens != ଖାଲି && *skipwhite(parens + 2) == NUL)
    {
	char_u *p = STRNCMP(arg, "<SNR>", 5) == 0 ? skipdigits(arg + 5) : arg;

	ଯଦି (to_name_end(p, TRUE) == parens)
	    r = call_simple_func(arg, (size_t)(parens - arg), rettv);
    }
    ଫେରନ୍ତୁ r;
}

/*
 * Handle zero level expression with optimization for a simple function call.
 * Same arguments and return value as eval0().
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
eval0_simple_funccal(
    char_u	*arg,
    typval_T	*rettv,
    exarg_T	*eap,
    evalarg_T	*evalarg)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    r = may_call_simple_func(arg, rettv);

    ଯଦି (r == NOTDONE)
	r = eval0_retarg(arg, rettv, eap, evalarg, ଖାଲି);
    ଫେରନ୍ତୁ r;
}

/*
 * Like eval0() but when "retarg" is not NULL store the pointer to after the
 * expression and don't check what comes after the expression.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
eval0_retarg(
    char_u	*arg,
    typval_T	*rettv,
    exarg_T	*eap,
    evalarg_T	*evalarg,
    char_u	**retarg)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret;
    char_u	*p;
    char_u	*ପଦସମଷ୍ଟି_end;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_emsg_before = did_emsg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		called_emsg_before = called_emsg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		check_for_end = retarg == ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		end_error = FALSE;

    p = skipwhite(arg);
    ret = eval1(&p, rettv, evalarg);

    ଯଦି (ret != FAIL)
    {
	ପଦସମଷ୍ଟି_end = p;
	p = skipwhite(p);

	// In Vim9 script a command block is not split at NL characters for
	// commands using an expression argument.  Skip over a '#' comment to
	// check for a following NL.  Require white space before the '#'.
	ଯଦି (in_vim9script() && p > ପଦସମଷ୍ଟି_end && retarg == ଖାଲି)
	    ଯେତେବେଳେ (*p == '#')
	    {
		char_u *nl = vim_strchr(p, NL);

		ଯଦି (nl == ଖାଲି)
		    ଭାଙ୍ଗ;
		p = skipwhite(nl + 1);
		ଯଦି (eap != ଖାଲି && *p != NUL)
		    eap->nextcmd = p;
		check_for_end = FALSE;
	    }

	ଯଦି (check_for_end)
	    end_error = !ends_excmd2(arg, p);
    }

    ଯଦି (ret == FAIL || end_error)
    {
	ଯଦି (ret != FAIL)
	    clear_tv(rettv);
	/*
	 * Report the invalid expression unless the expression evaluation has
	 * been cancelled due to an aborting error, an interrupt, or an
	 * exception, or we already gave a more specific error.
	 * Also check called_emsg for when using assert_fails().
	 */
	ଯଦି (!aborting()
		&& did_emsg == did_emsg_before
		&& called_emsg == called_emsg_before
		&& (!in_vim9script() || !vim9_bad_comment(p)))
	{
	    ଯଦି (end_error)
		semsg(_(e_trailing_characters_str), p);
	    ଅନ୍ୟଥା
		semsg(_(e_invalid_expression_str), arg);
	}

	ଯଦି (eap != ଖାଲି && p != ଖାଲି)
	{
	    // Some of the expression may not have been consumed.
	    // Only execute a next command if it cannot be a "||" operator.
	    // The next command may be "catch".
	    char_u *nextcmd = check_nextcmd(p);
	    ଯଦି (nextcmd != ଖାଲି && *nextcmd != '|')
		eap->nextcmd = nextcmd;
	}
	ଫେରନ୍ତୁ FAIL;
    }

    ଯଦି (retarg != ଖାଲି)
	*retarg = p;
    ଅନ୍ୟଥା ଯଦି (check_for_end && eap != ଖାଲି)
	set_nextcmd(eap, p);

    ଫେରନ୍ତୁ ret;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
eval1(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    char_u  *p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    getnext;

    CLEAR_POINTER(rettv);

    /*
     * Get the first variable.
     */
    ଯଦି (eval2(arg, rettv, evalarg) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    p = eval_next_non_blank(*arg, evalarg, &getnext);
    ଯଦି (*p == '?')
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		op_falsy = p[1] == '?';
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		result;
	typval_T	var2;
	evalarg_T	*evalarg_used = evalarg;
	evalarg_T	local_evalarg;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		orig_flags;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		evaluate;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		vim9script = in_vim9script();

	ଯଦି (evalarg == ଖାଲି)
	{
	    init_evalarg(&local_evalarg);
	    evalarg_used = &local_evalarg;
	}
	orig_flags = evalarg_used->eval_flags;
	evaluate = evalarg_used->eval_flags & EVAL_EVALUATE;

	ଯଦି (getnext)
	    *arg = eval_next_ଧାଡ଼ି(*arg, evalarg_used);
	ଅନ୍ୟଥା
	{
	    ଯଦି (evaluate && vim9script && !VIM_ISWHITE(p[-1]))
	    {
		error_white_both(p, op_falsy ? 2 : 1);
		clear_tv(rettv);
		ଫେରନ୍ତୁ FAIL;
	    }
	    *arg = p;
	}

	result = FALSE;
	ଯଦି (evaluate)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		error = FALSE;

	    ଯଦି (op_falsy)
	    {
		// Is this typeval supported with the falsy operator?
		ଯଦି (check_typval_is_ମୂଲ୍ୟ(rettv) == FAIL)
		{
		    clear_tv(rettv);
		    ଫେରନ୍ତୁ FAIL;
		}
		result = tv2bool(rettv);
	    }
	    ଅନ୍ୟଥା ଯଦି (vim9script)
		result = tv_get_bool_chk(rettv, &error);
	    ଅନ୍ୟଥା ଯଦି (tv_get_ସଂଖ୍ୟା_chk(rettv, &error) != 0)
		result = TRUE;
	    ଯଦି (error || !op_falsy || !result)
		clear_tv(rettv);
	    ଯଦି (error)
		ଫେରନ୍ତୁ FAIL;
	}

	/*
	 * Get the second variable.  Recursive!
	 */
	ଯଦି (op_falsy)
	    ++*arg;
	ଯଦି (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[1]))
	{
	    error_white_both(*arg - (op_falsy ? 1 : 0), op_falsy ? 2 : 1);
	    clear_tv(rettv);
	    ଫେରନ୍ତୁ FAIL;
	}
	*arg = skipwhite_and_linebreak(*arg + 1, evalarg_used);
	evalarg_used->eval_flags = (op_falsy ? !result : result)
				  ? orig_flags : (orig_flags & ~EVAL_EVALUATE);
	ଯଦି (eval1(arg, &var2, evalarg_used) == FAIL)
	{
	    evalarg_used->eval_flags = orig_flags;
	    ଫେରନ୍ତୁ FAIL;
	}
	ଯଦି (!op_falsy || !result)
	    *rettv = var2;

	ଯଦି (!op_falsy)
	{
	    /*
	     * Check for the ":".
	     */
	    p = eval_next_non_blank(*arg, evalarg_used, &getnext);
	    ଯଦି (*p != ':')
	    {
		emsg(_(e_missing_colon_after_questionmark));
		ଯଦି (evaluate && result)
		    clear_tv(rettv);
		evalarg_used->eval_flags = orig_flags;
		ଫେରନ୍ତୁ FAIL;
	    }
	    ଯଦି (getnext)
		*arg = eval_next_ଧାଡ଼ି(*arg, evalarg_used);
	    ଅନ୍ୟଥା
	    {
		ଯଦି (evaluate && vim9script && !VIM_ISWHITE(p[-1]))
		{
		    error_white_both(p, 1);
		    clear_tv(rettv);
		    evalarg_used->eval_flags = orig_flags;
		    ଫେରନ୍ତୁ FAIL;
		}
		*arg = p;
	    }

	    /*
	     * Get the third variable.  Recursive!
	     */
	    ଯଦି (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[1]))
	    {
		error_white_both(*arg, 1);
		clear_tv(rettv);
		evalarg_used->eval_flags = orig_flags;
		ଫେରନ୍ତୁ FAIL;
	    }
	    *arg = skipwhite_and_linebreak(*arg + 1, evalarg_used);
	    evalarg_used->eval_flags = !result ? orig_flags
					       : (orig_flags & ~EVAL_EVALUATE);
	    ଯଦି (eval1(arg, &var2, evalarg_used) == FAIL)
	    {
		ଯଦି (evaluate && result)
		    clear_tv(rettv);
		evalarg_used->eval_flags = orig_flags;
		ଫେରନ୍ତୁ FAIL;
	    }
	    ଯଦି (evaluate && !result)
		*rettv = var2;
	}

	ଯଦି (evalarg == ଖାଲି)
	    clear_evalarg(&local_evalarg, ଖାଲି);
	ଅନ୍ୟଥା
	    evalarg->eval_flags = orig_flags;
    }

    ଫେରନ୍ତୁ OK;
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
eval2(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		getnext;

    /*
     * Get the first expression.
     */
    ଯଦି (eval3(arg, rettv, evalarg) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    /*
     * Handle the  "||" operator.
     */
    p = eval_next_non_blank(*arg, evalarg, &getnext);
    ଯଦି (p[0] == '|' && p[1] == '|')
    {
	evalarg_T   *evalarg_used = evalarg;
	evalarg_T   local_evalarg;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    evaluate;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    orig_flags;
	long	    result = FALSE;
	typval_T    var2;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    error = FALSE;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    vim9script = in_vim9script();

	ଯଦି (evalarg == ଖାଲି)
	{
	    init_evalarg(&local_evalarg);
	    evalarg_used = &local_evalarg;
	}
	orig_flags = evalarg_used->eval_flags;
	evaluate = orig_flags & EVAL_EVALUATE;
	ଯଦି (evaluate)
	{
	    ଯଦି (vim9script)
		result = tv_get_bool_chk(rettv, &error);
	    ଅନ୍ୟଥା ଯଦି (tv_get_ସଂଖ୍ୟା_chk(rettv, &error) != 0)
		result = TRUE;
	    clear_tv(rettv);
	    ଯଦି (error)
		ଫେରନ୍ତୁ FAIL;
	}

	/*
	 * Repeat until there is no following "||".
	 */
	ଯେତେବେଳେ (p[0] == '|' && p[1] == '|')
	{
	    ଯଦି (getnext)
		*arg = eval_next_ଧାଡ଼ି(*arg, evalarg_used);
	    ଅନ୍ୟଥା
	    {
		ଯଦି (evaluate && vim9script && !VIM_ISWHITE(p[-1]))
		{
		    error_white_both(p, 2);
		    clear_tv(rettv);
		    ଫେରନ୍ତୁ FAIL;
		}
		*arg = p;
	    }

	    /*
	     * Get the second variable.
	     */
	    ଯଦି (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[2]))
	    {
		error_white_both(*arg, 2);
		clear_tv(rettv);
		ଫେରନ୍ତୁ FAIL;
	    }
	    *arg = skipwhite_and_linebreak(*arg + 2, evalarg_used);
	    evalarg_used->eval_flags = !result ? orig_flags
					       : (orig_flags & ~EVAL_EVALUATE);
	    ଯଦି (eval3(arg, &var2, evalarg_used) == FAIL)
		ଫେରନ୍ତୁ FAIL;

	    /*
	     * Compute the result.
	     */
	    ଯଦି (evaluate && !result)
	    {
		ଯଦି (vim9script)
		    result = tv_get_bool_chk(&var2, &error);
		ଅନ୍ୟଥା ଯଦି (tv_get_ସଂଖ୍ୟା_chk(&var2, &error) != 0)
		    result = TRUE;
		clear_tv(&var2);
		ଯଦି (error)
		    ଫେରନ୍ତୁ FAIL;
	    }
	    ଯଦି (evaluate)
	    {
		ଯଦି (vim9script)
		{
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_ସଂଖ୍ୟା = result ? VVAL_TRUE : VVAL_FALSE;
		}
		ଅନ୍ୟଥା
		{
		    rettv->v_type = VAR_ସଂଖ୍ୟା;
		    rettv->vval.v_ସଂଖ୍ୟା = result;
		}
	    }

	    p = eval_next_non_blank(*arg, evalarg_used, &getnext);
	}

	ଯଦି (evalarg == ଖାଲି)
	    clear_evalarg(&local_evalarg, ଖାଲି);
	ଅନ୍ୟଥା
	    evalarg->eval_flags = orig_flags;
    }

    ଫେରନ୍ତୁ OK;
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
eval3(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		getnext;

    /*
     * Get the first expression.
     */
    ଯଦି (eval4(arg, rettv, evalarg) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    /*
     * Handle the "&&" operator.
     */
    p = eval_next_non_blank(*arg, evalarg, &getnext);
    ଯଦି (p[0] == '&' && p[1] == '&')
    {
	evalarg_T   *evalarg_used = evalarg;
	evalarg_T   local_evalarg;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    orig_flags;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    evaluate;
	long	    result = TRUE;
	typval_T    var2;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    error = FALSE;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    vim9script = in_vim9script();

	ଯଦି (evalarg == ଖାଲି)
	{
	    init_evalarg(&local_evalarg);
	    evalarg_used = &local_evalarg;
	}
	orig_flags = evalarg_used->eval_flags;
	evaluate = orig_flags & EVAL_EVALUATE;
	ଯଦି (evaluate)
	{
	    ଯଦି (vim9script)
		result = tv_get_bool_chk(rettv, &error);
	    ଅନ୍ୟଥା ଯଦି (tv_get_ସଂଖ୍ୟା_chk(rettv, &error) == 0)
		result = FALSE;
	    clear_tv(rettv);
	    ଯଦି (error)
		ଫେରନ୍ତୁ FAIL;
	}

	/*
	 * Repeat until there is no following "&&".
	 */
	ଯେତେବେଳେ (p[0] == '&' && p[1] == '&')
	{
	    ଯଦି (getnext)
		*arg = eval_next_ଧାଡ଼ି(*arg, evalarg_used);
	    ଅନ୍ୟଥା
	    {
		ଯଦି (evaluate && vim9script && !VIM_ISWHITE(p[-1]))
		{
		    error_white_both(p, 2);
		    clear_tv(rettv);
		    ଫେରନ୍ତୁ FAIL;
		}
		*arg = p;
	    }

	    /*
	     * Get the second variable.
	     */
	    ଯଦି (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[2]))
	    {
		error_white_both(*arg, 2);
		clear_tv(rettv);
		ଫେରନ୍ତୁ FAIL;
	    }
	    *arg = skipwhite_and_linebreak(*arg + 2, evalarg_used);
	    evalarg_used->eval_flags = result ? orig_flags
					      : (orig_flags & ~EVAL_EVALUATE);
	    CLEAR_FIELD(var2);
	    ଯଦି (eval4(arg, &var2, evalarg_used) == FAIL)
		ଫେରନ୍ତୁ FAIL;

	    /*
	     * Compute the result.
	     */
	    ଯଦି (evaluate && result)
	    {
		ଯଦି (vim9script)
		    result = tv_get_bool_chk(&var2, &error);
		ଅନ୍ୟଥା ଯଦି (tv_get_ସଂଖ୍ୟା_chk(&var2, &error) == 0)
		    result = FALSE;
		clear_tv(&var2);
		ଯଦି (error)
		    ଫେରନ୍ତୁ FAIL;
	    }
	    ଯଦି (evaluate)
	    {
		ଯଦି (vim9script)
		{
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_ସଂଖ୍ୟା = result ? VVAL_TRUE : VVAL_FALSE;
		}
		ଅନ୍ୟଥା
		{
		    rettv->v_type = VAR_ସଂଖ୍ୟା;
		    rettv->vval.v_ସଂଖ୍ୟା = result;
		}
	    }

	    p = eval_next_non_blank(*arg, evalarg_used, &getnext);
	}

	ଯଦି (evalarg == ଖାଲି)
	    clear_evalarg(&local_evalarg, ଖାଲି);
	ଅନ୍ୟଥା
	    evalarg->eval_flags = orig_flags;
    }

    ଫେରନ୍ତୁ OK;
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
eval4(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		getnext;
    exprtype_T	type = ପଦସମଷ୍ଟି_UNKNOWN;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len = 2;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		type_is = FALSE;

    /*
     * Get the first expression.
     */
    ଯଦି (eval5(arg, rettv, evalarg) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    p = eval_next_non_blank(*arg, evalarg, &getnext);

    type = get_compare_type(p, &len, &type_is);

    /*
     * If there is a comparative operator, use it.
     */
    ଯଦି (type != ପଦସମଷ୍ଟି_UNKNOWN)
    {
	typval_T    var2;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    ic;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    vim9script = in_vim9script();
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    evaluate = evalarg == ଖାଲି
				   ? 0 : (evalarg->eval_flags & EVAL_EVALUATE);
	long	    comp_lnum = SOURCING_LNUM;

	ଯଦି (getnext)
	{
	    *arg = eval_next_ଧାଡ଼ି(*arg, evalarg);
	    p = *arg;
	}
	ଅନ୍ୟଥା ଯଦି (evaluate && vim9script && !VIM_ISWHITE(**arg))
	{
	    error_white_both(*arg, len);
	    clear_tv(rettv);
	    ଫେରନ୍ତୁ FAIL;
	}

	ଯଦି (vim9script && type_is && (p[len] == '?' || p[len] == '#'))
	{
	    semsg(_(e_invalid_expression_str), p);
	    clear_tv(rettv);
	    ଫେରନ୍ତୁ FAIL;
	}

	// extra question mark appended: ignore case
	ଯଦି (p[len] == '?')
	{
	    ic = TRUE;
	    ++len;
	}
	// extra '#' appended: match case
	ଅନ୍ୟଥା ଯଦି (p[len] == '#')
	{
	    ic = FALSE;
	    ++len;
	}
	// nothing appended: use 'ignorecase' if not in Vim script
	ଅନ୍ୟଥା
	    ic = vim9script ? FALSE : p_ic;

	/*
	 * Get the second variable.
	 */
	ଯଦି (evaluate && vim9script && !IS_WHITE_OR_NUL(p[len]))
	{
	    error_white_both(p, len);
	    clear_tv(rettv);
	    ଫେରନ୍ତୁ FAIL;
	}
	*arg = skipwhite_and_linebreak(p + len, evalarg);
	ଯଦି (eval5(arg, &var2, evalarg) == FAIL)
	{
	    clear_tv(rettv);
	    ଫେରନ୍ତୁ FAIL;
	}
	ଯଦି (evaluate)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;

	    // use the line of the comparison for messages
	    SOURCING_LNUM = comp_lnum;
	    ଯଦି (vim9script && check_compare_types(type, rettv, &var2) == FAIL)
	    {
		ret = FAIL;
		clear_tv(rettv);
	    }
	    ଅନ୍ୟଥା
		ret = typval_compare(rettv, &var2, type, ic);
	    clear_tv(&var2);
	    ଫେରନ୍ତୁ ret;
	}
    }

    ଫେରନ୍ତୁ OK;
}

/*
 * Make a copy of blob "tv1" and append blob "tv2".
 */
    ଶୂନ୍ୟ
eval_addblob(typval_T *tv1, typval_T *tv2)
{
    blob_T  *b1 = tv1->vval.v_blob;
    blob_T  *b2 = tv2->vval.v_blob;
    blob_T  *b = blob_alloc();
    long    len1, len2;
    long    totallen;

    ଯଦି (b == ଖାଲି)
	ଫେରନ୍ତୁ;

    len1 = blob_len(b1);
    len2 = blob_len(b2);
    totallen = len1 + len2;

    ଯଦି (totallen > 0 && totallen <= INT_MAX
				    && ga_grow(&b->bv_ga, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)totallen) == OK)
    {
	ଯଦି (len1 > 0)
	    mch_memmove((char_u *)b->bv_ga.ga_data,
		    (char_u *)b1->bv_ga.ga_data, (size_t)len1);
	ଯଦି (len2 > 0)
	    mch_memmove((char_u *)b->bv_ga.ga_data + len1,
		    (char_u *)b2->bv_ga.ga_data, (size_t)len2);
	b->bv_ga.ga_len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)totallen;
    }

    clear_tv(tv1);
    rettv_blob_set(tv1, b);
}

/*
 * Make a copy of list "tv1" and append list "tv2".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
eval_addlist(typval_T *tv1, typval_T *tv2)
{
    typval_T var3;

    // concatenate Lists
    ଯଦି (list_concat(tv1->vval.v_list, tv2->vval.v_list, &var3) == FAIL)
    {
	clear_tv(tv1);
	clear_tv(tv2);
	ଫେରନ୍ତୁ FAIL;
    }
    clear_tv(tv1);
    *tv1 = var3;
    ଫେରନ୍ତୁ OK;
}

/*
 * Make a copy of tuple "tv1" and append tuple "tv2".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
eval_addtuple(typval_T *tv1, typval_T *tv2)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		vim9script = in_vim9script();
    typval_T	var3;

    ଯଦି (vim9script && tv1->vval.v_tuple != ଖାଲି && tv2->vval.v_tuple != ଖାଲି
	    && tv1->vval.v_tuple->tv_type != ଖାଲି
	    && tv2->vval.v_tuple->tv_type != ଖାଲି)
    {
	ଯଦି (!check_tuples_addable(tv1->vval.v_tuple->tv_type,
						tv2->vval.v_tuple->tv_type))
	    ଫେରନ୍ତୁ FAIL;
    }

    // concatenate tuples
    ଯଦି (tuple_concat(tv1->vval.v_tuple, tv2->vval.v_tuple, &var3) == FAIL)
    {
	clear_tv(tv1);
	clear_tv(tv2);
	ଫେରନ୍ତୁ FAIL;
    }
    clear_tv(tv1);
    *tv1 = var3;
    ଫେରନ୍ତୁ OK;
}

/*
 * Left or right shift the number "tv1" by the number "tv2" and store the
 * result in "tv1".
 *
 * Return OK or FAIL.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
eval_shift_ସଂଖ୍ୟା(typval_T *tv1, typval_T *tv2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା shift_type)
{
    ଯଦି (tv2->v_type != VAR_ସଂଖ୍ୟା || tv2->vval.v_ସଂଖ୍ୟା < 0)
    {
	// right operand should be a positive number
	ଯଦି (tv2->v_type != VAR_ସଂଖ୍ୟା)
	    emsg(_(e_bitshift_ops_must_be_ସଂଖ୍ୟା));
	ଅନ୍ୟଥା
	    emsg(_(e_bitshift_ops_must_be_positive));
	clear_tv(tv1);
	clear_tv(tv2);
	ଫେରନ୍ତୁ FAIL;
    }

    ଯଦି (tv2->vval.v_ସଂଖ୍ୟା > MAX_LSHIFT_BITS)
	// shifting more bits than we have always results in zero
	tv1->vval.v_ସଂଖ୍ୟା = 0;
    ଅନ୍ୟଥା ଯଦି (shift_type == ପଦସମଷ୍ଟି_LSHIFT)
	tv1->vval.v_ସଂଖ୍ୟା =
	    (uvarnumber_T)tv1->vval.v_ସଂଖ୍ୟା << tv2->vval.v_ସଂଖ୍ୟା;
    ଅନ୍ୟଥା
	tv1->vval.v_ସଂଖ୍ୟା =
	    (uvarnumber_T)tv1->vval.v_ସଂଖ୍ୟା >> tv2->vval.v_ସଂଖ୍ୟା;

    ଫେରନ୍ତୁ OK;
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
eval5(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    /*
     * Get the first expression.
     */
    ଯଦି (eval6(arg, rettv, evalarg) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    /*
     * Repeat computing, until no '<<' or '>>' is following.
     */
    ପାଇଁ (;;)
    {
	char_u		*p;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		getnext;
	exprtype_T	exprtype;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		evaluate;
	typval_T	var2;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		vim9script;

	p = eval_next_non_blank(*arg, evalarg, &getnext);
	ଯଦି (p[0] == '<' && p[1] == '<')
	    exprtype = ପଦସମଷ୍ଟି_LSHIFT;
	ଅନ୍ୟଥା ଯଦି (p[0] == '>' && p[1] == '>')
	    exprtype = ପଦସମଷ୍ଟି_RSHIFT;
	ଅନ୍ୟଥା
	    ଫେରନ୍ତୁ OK;

	// Handle a bitwise left or right shift operator
	evaluate = evalarg == ଖାଲି ? 0 : (evalarg->eval_flags & EVAL_EVALUATE);
	ଯଦି (evaluate && rettv->v_type != VAR_ସଂଖ୍ୟା)
	{
	    // left operand should be a number
	    emsg(_(e_bitshift_ops_must_be_ସଂଖ୍ୟା));
	    clear_tv(rettv);
	    ଫେରନ୍ତୁ FAIL;
	}

	vim9script = in_vim9script();
	ଯଦି (getnext)
	{
	    *arg = eval_next_ଧାଡ଼ି(*arg, evalarg);
	    p = *arg;
	}
	ଅନ୍ୟଥା ଯଦି (evaluate && vim9script && !VIM_ISWHITE(**arg))
	{
	    error_white_both(*arg, 2);
	    clear_tv(rettv);
	    ଫେରନ୍ତୁ FAIL;
	}

	/*
	 * Get the second variable.
	 */
	ଯଦି (evaluate && vim9script && !IS_WHITE_OR_NUL(p[2]))
	{
	    error_white_both(p, 2);
	    clear_tv(rettv);
	    ଫେରନ୍ତୁ FAIL;
	}
	*arg = skipwhite_and_linebreak(p + 2, evalarg);
	ଯଦି (eval6(arg, &var2, evalarg) == FAIL)
	{
	    clear_tv(rettv);
	    ଫେରନ୍ତୁ FAIL;
	}

	ଯଦି (evaluate)
	{
	    ଯଦି (eval_shift_ସଂଖ୍ୟା(rettv, &var2, exprtype) == FAIL)
		ଫେରନ୍ତୁ FAIL;
	}

	clear_tv(&var2);
    }

    ଫେରନ୍ତୁ OK;
}

/*
 * Concatenate strings "tv1" and "tv2" and store the result in "tv1".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
eval_concat_str(typval_T *tv1, typval_T *tv2)
{
    char_u	buf1[NUMBUFLEN], buf2[NUMBUFLEN];
    char_u	*s1 = tv_get_string_buf(tv1, buf1);
    char_u	*s2 = ଖାଲି;
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		vim9script = in_vim9script();

    ଯଦି (vim9script && (tv2->v_type == VAR_VOID
		|| tv2->v_type == VAR_CHANNEL
		|| tv2->v_type == VAR_JOB))
	semsg(_(e_using_invalid_ମୂଲ୍ୟ_as_string_str),
		vartype_name(tv2->v_type));
    ଅନ୍ୟଥା ଯଦି (vim9script && tv2->v_type == VAR_FLOAT)
    {
	vim_snprintf((ଅକ୍ଷର *)buf2, NUMBUFLEN, "%g",
		tv2->vval.v_float);
	s2 = buf2;
    }
    ଅନ୍ୟଥା
	s2 = tv_get_string_buf_chk(tv2, buf2);
    ଯଦି (s2 == ଖାଲି)		// type error ?
    {
	clear_tv(tv1);
	clear_tv(tv2);
	ଫେରନ୍ତୁ FAIL;
    }

    // When possible, grow the existing string in place to avoid alloc/free.
    ଯଦି (grow_string_tv(tv1, s2) == OK)
	ଫେରନ୍ତୁ OK;

    p = concat_str(s1, s2);
    clear_tv(tv1);
    tv1->v_type = VAR_STRING;
    tv1->vval.v_string = p;

    ଫେରନ୍ତୁ OK;
}

/*
 * Add or subtract numbers "tv1" and "tv2" and store the result in "tv1".
 * The numbers can be whole numbers or floats.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
eval_addsub_ସଂଖ୍ୟା(typval_T *tv1, typval_T *tv2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା op)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		error = FALSE;
    varnumber_T	n1, n2;
    float_T	f1 = 0, f2 = 0;

    ଯଦି (tv1->v_type == VAR_FLOAT)
    {
	f1 = tv1->vval.v_float;
	n1 = 0;
    }
    ଅନ୍ୟଥା
    {
	n1 = tv_get_ସଂଖ୍ୟା_chk(tv1, &error);
	ଯଦି (error)
	{
	    // This can only happen for "list + non-list" or
	    // "blob + non-blob".  For "non-list + ..." or
	    // "something - ...", we returned before evaluating the
	    // 2nd operand.
	    clear_tv(tv1);
	    clear_tv(tv2);
	    ଫେରନ୍ତୁ FAIL;
	}
	ଯଦି (tv2->v_type == VAR_FLOAT)
	    f1 = n1;
    }
    ଯଦି (tv2->v_type == VAR_FLOAT)
    {
	f2 = tv2->vval.v_float;
	n2 = 0;
    }
    ଅନ୍ୟଥା
    {
	n2 = tv_get_ସଂଖ୍ୟା_chk(tv2, &error);
	ଯଦି (error)
	{
	    clear_tv(tv1);
	    clear_tv(tv2);
	    ଫେରନ୍ତୁ FAIL;
	}
	ଯଦି (tv1->v_type == VAR_FLOAT)
	    f2 = n2;
    }
    clear_tv(tv1);

    // If there is a float on either side the result is a float.
    ଯଦି (tv1->v_type == VAR_FLOAT || tv2->v_type == VAR_FLOAT)
    {
	ଯଦି (op == '+')
	    f1 = f1 + f2;
	ଅନ୍ୟଥା
	    f1 = f1 - f2;
	tv1->v_type = VAR_FLOAT;
	tv1->vval.v_float = f1;
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (op == '+')
	    n1 = n1 + n2;
	ଅନ୍ୟଥା
	    n1 = n1 - n2;
	tv1->v_type = VAR_ସଂଖ୍ୟା;
	tv1->vval.v_ସଂଖ୍ୟା = n1;
    }

    ଫେରନ୍ତୁ OK;
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
eval6(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    /*
     * Get the first expression.
     */
    ଯଦି (eval7(arg, rettv, evalarg, FALSE) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    /*
     * Repeat computing, until no '+', '-' or '.' is following.
     */
    ପାଇଁ (;;)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    evaluate;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    getnext;
	char_u	    *p;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    op;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    oplen;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    concat;
	typval_T    var2;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    vim9script = in_vim9script();
	long	    op_lnum = SOURCING_LNUM;

	// "." is only string concatenation when scriptversion is 1
	// "+=", "-=" and "..=" are assignments
	// "++" and "--" on the next line are a separate command.
	p = eval_next_non_blank(*arg, evalarg, &getnext);
	op = *p;
	concat = op == '.' && (*(p + 1) == '.' || in_old_script(2));
	ଯଦି ((op != '+' && op != '-' && !concat) || p[1] == '='
					       || (p[1] == '.' && p[2] == '='))
	    ଭାଙ୍ଗ;
	ଯଦି (getnext && (op == '+' || op == '-') && p[0] == p[1])
	    ଭାଙ୍ଗ;

	evaluate = evalarg == ଖାଲି ? 0 : (evalarg->eval_flags & EVAL_EVALUATE);
	oplen = (concat && p[1] == '.') ? 2 : 1;
	ଯଦି (getnext)
	    *arg = eval_next_ଧାଡ଼ି(*arg, evalarg);
	ଅନ୍ୟଥା
	{
	    ଯଦି (evaluate && vim9script && !VIM_ISWHITE(**arg))
	    {
		error_white_both(*arg, oplen);
		clear_tv(rettv);
		ଫେରନ୍ତୁ FAIL;
	    }
	    *arg = p;
	}
	ଯଦି ((op != '+' || (rettv->v_type != VAR_LIST
						&& rettv->v_type != VAR_TUPLE
						&& rettv->v_type != VAR_BLOB))
		&& (op == '.' || rettv->v_type != VAR_FLOAT)
		&& evaluate)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		error = FALSE;

	    // For "list + ...", an illegal use of the first operand as
	    // a number cannot be determined before evaluating the 2nd
	    // operand: if this is also a list, all is ok.
	    // For "something . ...", "something - ..." or "non-list + ...",
	    // we know that the first operand needs to be a string or number
	    // without evaluating the 2nd operand.  So check before to avoid
	    // side effects after an error.
	    ଯଦି (op != '.')
		tv_get_ସଂଖ୍ୟା_chk(rettv, &error);
	    ଯଦି ((op == '.' && tv_get_string_chk(rettv) == ଖାଲି) || error)
	    {
		clear_tv(rettv);
		ଫେରନ୍ତୁ FAIL;
	    }
	}

	/*
	 * Get the second variable.
	 */
	ଯଦି (evaluate && vim9script && !IS_WHITE_OR_NUL((*arg)[oplen]))
	{
	    error_white_both(*arg, oplen);
	    clear_tv(rettv);
	    ଫେରନ୍ତୁ FAIL;
	}
	*arg = skipwhite_and_linebreak(*arg + oplen, evalarg);
	ଯଦି (eval7(arg, &var2, evalarg, !vim9script && op == '.') == FAIL)
	{
	    clear_tv(rettv);
	    ଫେରନ୍ତୁ FAIL;
	}

	ଯଦି (evaluate)
	{
	    /*
	     * Compute the result.
	     */
	    // use the line of the operation for messages
	    SOURCING_LNUM = op_lnum;
	    ଯଦି (op == '.')
	    {
		ଯଦି (eval_concat_str(rettv, &var2) == FAIL)
		    ଫେରନ୍ତୁ FAIL;
	    }
	    ଅନ୍ୟଥା ଯଦି (op == '+' && rettv->v_type == VAR_BLOB
						   && var2.v_type == VAR_BLOB)
		eval_addblob(rettv, &var2);
	    ଅନ୍ୟଥା ଯଦି (op == '+' && rettv->v_type == VAR_LIST
						   && var2.v_type == VAR_LIST)
	    {
		ଯଦି (eval_addlist(rettv, &var2) == FAIL)
		    ଫେରନ୍ତୁ FAIL;
	    }
	    ଅନ୍ୟଥା ଯଦି (op == '+' && rettv->v_type == VAR_TUPLE
					   && var2.v_type == VAR_TUPLE)
	    {
		ଯଦି (eval_addtuple(rettv, &var2) == FAIL)
		    ଫେରନ୍ତୁ FAIL;
	    }
	    ଅନ୍ୟଥା
	    {
		ଯଦି (eval_addsub_ସଂଖ୍ୟା(rettv, &var2, op) == FAIL)
		    ଫେରନ୍ତୁ FAIL;
	    }
	    clear_tv(&var2);
	}
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Multiply or divide or compute the modulo of numbers "tv1" and "tv2" and
 * store the result in "tv1".  The numbers can be whole numbers or floats.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
eval_multdiv_ସଂଖ୍ୟା(typval_T *tv1, typval_T *tv2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା op)
{
    varnumber_T	n1, n2;
    float_T	f1, f2;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		error;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		use_float = FALSE;

    f1 = 0;
    f2 = 0;
    error = FALSE;
    ଯଦି (tv1->v_type == VAR_FLOAT)
    {
	f1 = tv1->vval.v_float;
	use_float = TRUE;
	n1 = 0;
    }
    ଅନ୍ୟଥା
	n1 = tv_get_ସଂଖ୍ୟା_chk(tv1, &error);
    clear_tv(tv1);
    ଯଦି (error)
    {
	clear_tv(tv2);
	ଫେରନ୍ତୁ FAIL;
    }

    ଯଦି (tv2->v_type == VAR_FLOAT)
    {
	ଯଦି (!use_float)
	{
	    f1 = n1;
	    use_float = TRUE;
	}
	f2 = tv2->vval.v_float;
	n2 = 0;
    }
    ଅନ୍ୟଥା
    {
	n2 = tv_get_ସଂଖ୍ୟା_chk(tv2, &error);
	clear_tv(tv2);
	ଯଦି (error)
	    ଫେରନ୍ତୁ FAIL;
	ଯଦି (use_float)
	    f2 = n2;
    }

    /*
     * Compute the result.
     * When either side is a float the result is a float.
     */
    ଯଦି (use_float)
    {
	ଯଦି (op == '*')
	    f1 = f1 * f2;
	ଅନ୍ୟଥା ଯଦି (op == '/')
	{
#ifdef VMS
	    // VMS crashes on divide by zero, work around it
	    ଯଦି (f2 == 0.0)
	    {
		ଯଦି (f1 == 0)
		    f1 = -1 * __F_FLT_MAX - 1L;   // similar to NaN
		ଅନ୍ୟଥା ଯଦି (f1 < 0)
		    f1 = -1 * __F_FLT_MAX;
		ଅନ୍ୟଥା
		    f1 = __F_FLT_MAX;
	    }
	    ଅନ୍ୟଥା
		f1 = f1 / f2;
#ଅନ୍ୟଥା
	    // We rely on the floating point library to handle divide
	    // by zero to result in "inf" and not a crash.
	    f1 = f1 / f2;
#endif
	}
	ଅନ୍ୟଥା
	{
	    emsg(_(e_cannot_use_percent_with_float));
	    ଫେରନ୍ତୁ FAIL;
	}
	tv1->v_type = VAR_FLOAT;
	tv1->vval.v_float = f1;
    }
    ଅନ୍ୟଥା
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    failed = FALSE;

	ଯଦି (op == '*')
	    n1 = n1 * n2;
	ଅନ୍ୟଥା ଯଦି (op == '/')
	    n1 = ଅଙ୍କ_divide(n1, n2, &failed);
	ଅନ୍ୟଥା
	    n1 = ଅଙ୍କ_modulus(n1, n2, &failed);
	ଯଦି (failed)
	    ଫେରନ୍ତୁ FAIL;

	tv1->v_type = VAR_ସଂଖ୍ୟା;
	tv1->vval.v_ସଂଖ୍ୟା = n1;
    }

    ଫେରନ୍ତୁ OK;
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
eval7(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		want_string)  // after "." operator
{
    /*
     * Get the first expression.
     */
    ଯଦି (eval8(arg, rettv, evalarg, want_string) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    /*
     * Repeat computing, until no '*', '/' or '%' is following.
     */
    ପାଇଁ (;;)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    evaluate;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    getnext;
	typval_T    var2;
	char_u	    *p;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    op;

	// "*=", "/=" and "%=" are assignments
	p = eval_next_non_blank(*arg, evalarg, &getnext);
	op = *p;
	ଯଦି ((op != '*' && op != '/' && op != '%') || p[1] == '=')
	    ଭାଙ୍ଗ;

	evaluate = evalarg == ଖାଲି ? 0 : (evalarg->eval_flags & EVAL_EVALUATE);
	ଯଦି (getnext)
	    *arg = eval_next_ଧାଡ଼ି(*arg, evalarg);
	ଅନ୍ୟଥା
	{
	    ଯଦି (evaluate && in_vim9script() && !VIM_ISWHITE(**arg))
	    {
		error_white_both(*arg, 1);
		clear_tv(rettv);
		ଫେରନ୍ତୁ FAIL;
	    }
	    *arg = p;
	}

	/*
	 * Get the second variable.
	 */
	ଯଦି (evaluate && in_vim9script() && !IS_WHITE_OR_NUL((*arg)[1]))
	{
	    error_white_both(*arg, 1);
	    clear_tv(rettv);
	    ଫେରନ୍ତୁ FAIL;
	}
	*arg = skipwhite_and_linebreak(*arg + 1, evalarg);
	ଯଦି (eval8(arg, &var2, evalarg, FALSE) == FAIL)
	    ଫେରନ୍ତୁ FAIL;

	ଯଦି (evaluate)
	    // Compute the result.
	    ଯଦି (eval_multdiv_ସଂଖ୍ୟା(rettv, &var2, op) == FAIL)
		ଫେରନ୍ତୁ FAIL;
    }

    ଫେରନ୍ତୁ OK;
}

/*
 * Handle seventh level expression:
 *	a type cast before a base level expression.
 * "arg" must point to the first non-white of the expression.
 * "arg" is advanced to just after the recognized expression.
 * Return OK or FAIL.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
eval8(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		want_string)	// after "." operator
{
    type_T	*want_type = ଖାଲି;
    garray_T	type_list;	    // list of pointers to allocated types
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		res;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		evaluate = evalarg == ଖାଲି ? 0
				       : (evalarg->eval_flags & EVAL_EVALUATE);

    // Recognize <type> in Vim9 script only.
    ଯଦି (in_vim9script() && **arg == '<' && eval_isnamec1((*arg)[1])
					     && STRNCMP(*arg, "<SNR>", 5) != 0)
    {
	++*arg;
	ga_init2(&type_list, ଆକାର(type_T *), 10);
	ଯଦି (!evaluate)
	    // Only skip over the type, it may use a type variable that is only
	    // known when the expression is compiled, e.g. in a lambda.
	    *arg = skip_type(*arg, FALSE);
	ଅନ୍ୟଥା
	{
	    want_type = parse_type(arg, &type_list, ଖାଲି, ଖାଲି, TRUE);
	    ଯଦି (want_type == ଖାଲି)
	    {
		clear_type_list(&type_list);
		ଫେରନ୍ତୁ FAIL;
	    }
	}

	ଯଦି (**arg != '>')
	{
	    ଯଦି (*skipwhite(*arg) == '>')
		semsg(_(e_no_white_space_allowed_before_str_str), ">", *arg);
	    ଅନ୍ୟଥା
		emsg(_(e_missing_gt));
	    clear_type_list(&type_list);
	    ଫେରନ୍ତୁ FAIL;
	}
	++*arg;
	*arg = skipwhite_and_linebreak(*arg, evalarg);
    }

    res = eval9(arg, rettv, evalarg, want_string);

    ଯଦି (want_type != ଖାଲି)
    {
	ଯଦି (evaluate && res == OK)
	{
	    type_T *actual = typval2type(rettv, get_copyID(), &type_list,
							       TVTT_DO_MEMBER);

	    ଯଦି (!equal_type(want_type, actual, 0))
	    {
		ଯଦି (want_type->tt_type == VAR_BOOL
					&& actual->tt_type != VAR_BOOL
					&& (actual->tt_flags & TTFLAG_BOOL_OK))
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା n = tv2bool(rettv);

		    // can use "0" and "1" for boolean in some places
		    clear_tv(rettv);
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_ସଂଖ୍ୟା = n ? VVAL_TRUE : VVAL_FALSE;
		}
		ଅନ୍ୟଥା
		{
		    where_T where = WHERE_INIT;

		    res = check_type(want_type, actual, TRUE, where);
		}
	    }
	}
	clear_type_list(&type_list);
    }

    ଫେରନ୍ତୁ res;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
eval_leader(char_u **arg, ପୂର୍ଣ୍ଣସଂଖ୍ୟା vim9)
{
    char_u	*s = *arg;
    char_u	*p = *arg;

    ଯେତେବେଳେ (*p == '!' || *p == '-' || *p == '+')
    {
	char_u *n = skipwhite(p + 1);

	// ++, --, -+ and +- are not accepted in Vim9 script
	ଯଦି (vim9 && (*p == '-' || *p == '+') && (*n == '-' || *n == '+'))
	{
	    semsg(_(e_invalid_expression_str), s);
	    ଫେରନ୍ତୁ FAIL;
	}
	p = n;
    }
    *arg = p;
    ଫେରନ୍ତୁ OK;
}

/*
 * Check for a predefined value "true", "false" and "null.*".
 * Return OK when recognized.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
handle_predefined(char_u *s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len, typval_T *rettv)
{
    ବାଛ (len)
    {
	ସ୍ଥିତି 4: ଯଦି (STRNCMP(s, "true", 4) == 0)
		{
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_ସଂଖ୍ୟା = VVAL_TRUE;
		    ଫେରନ୍ତୁ OK;
		}
		ଯଦି (STRNCMP(s, "null", 4) == 0)
		{
		    rettv->v_type = VAR_SPECIAL;
		    rettv->vval.v_ସଂଖ୍ୟା = VVAL_NULL;
		    ଫେରନ୍ତୁ OK;
		}
		ଭାଙ୍ଗ;
	ସ୍ଥିତି 5: ଯଦି (STRNCMP(s, "false", 5) == 0)
		{
		    rettv->v_type = VAR_BOOL;
		    rettv->vval.v_ସଂଖ୍ୟା = VVAL_FALSE;
		    ଫେରନ୍ତୁ OK;
		}
		ଭାଙ୍ଗ;
	ସ୍ଥିତି 8: ଯଦି (STRNCMP(s, "null_job", 8) == 0)
		{
#ifdef FEAT_JOB_CHANNEL
		    rettv->v_type = VAR_JOB;
		    rettv->vval.v_job = ଖାଲି;
#ଅନ୍ୟଥା
		    rettv->v_type = VAR_SPECIAL;
		    rettv->vval.v_ସଂଖ୍ୟା = VVAL_NULL;
#endif
		    ଫେରନ୍ତୁ OK;
		}
		ଭାଙ୍ଗ;
	ସ୍ଥିତି 9:
		ଯଦି (STRNCMP(s, "null_", 5) != 0)
		    ଭାଙ୍ଗ;
		// null_list
		ଯଦି (STRNCMP(s + 5, "list", 4) == 0)
		{
		    rettv->v_type = VAR_LIST;
		    rettv->vval.v_list = ଖାଲି;
		    ଫେରନ୍ତୁ OK;
		}
		// null_dict
		ଯଦି (STRNCMP(s + 5, "dict", 4) == 0)
		{
		    rettv->v_type = VAR_DICT;
		    rettv->vval.v_dict = ଖାଲି;
		    ଫେରନ୍ତୁ OK;
		}
		// null_blob
		ଯଦି (STRNCMP(s + 5, "blob", 4) == 0)
		{
		    rettv->v_type = VAR_BLOB;
		    rettv->vval.v_blob = ଖାଲି;
		    ଫେରନ୍ତୁ OK;
		}
		ଭାଙ୍ଗ;
	ସ୍ଥିତି 10:
		ଯଦି (STRNCMP(s, "null_", 5) != 0)
		    ଭାଙ୍ଗ;
		// null_class
		ଯଦି (STRNCMP(s + 5, "class", 5) == 0)
		{
		    rettv->v_type = VAR_CLASS;
		    rettv->vval.v_class = ଖାଲି;
		    ଫେରନ୍ତୁ OK;
		}
		ଯଦି (STRNCMP(s + 5, "tuple", 5) == 0)
		{
		    rettv->v_type = VAR_TUPLE;
		    rettv->vval.v_tuple = ଖାଲି;
		    ଫେରନ୍ତୁ OK;
		}
		ଭାଙ୍ଗ;
	ସ୍ଥିତି 11: ଯଦି (STRNCMP(s, "null_string", 11) == 0)
		{
		    rettv->v_type = VAR_STRING;
		    rettv->vval.v_string = ଖାଲି;
		    ଫେରନ୍ତୁ OK;
		}
		ଯଦି (STRNCMP(s, "null_object", 11) == 0)
		{
		    rettv->v_type = VAR_OBJECT;
		    rettv->vval.v_object = ଖାଲି;
		    ଫେରନ୍ତୁ OK;
		}
		ଭାଙ୍ଗ;
	ସ୍ଥିତି 12:
		ଯଦି (STRNCMP(s, "null_channel", 12) == 0)
		{
#ifdef FEAT_JOB_CHANNEL
		    rettv->v_type = VAR_CHANNEL;
		    rettv->vval.v_channel = ଖାଲି;
#ଅନ୍ୟଥା
		    rettv->v_type = VAR_SPECIAL;
		    rettv->vval.v_ସଂଖ୍ୟା = VVAL_NULL;
#endif
		    ଫେରନ୍ତୁ OK;
		}
		ଯଦି (STRNCMP(s, "null_partial", 12) == 0)
		{
		    rettv->v_type = VAR_PARTIAL;
		    rettv->vval.v_partial = ଖାଲି;
		    ଫେରନ୍ତୁ OK;
		}
		ଭାଙ୍ଗ;
	ସ୍ଥିତି 13: ଯଦି (STRNCMP(s, "null_function", 13) == 0)
		{
		    rettv->v_type = VAR_FUNC;
		    rettv->vval.v_string = ଖାଲି;
		    ଫେରନ୍ତୁ OK;
		}
		ଭାଙ୍ଗ;
    }
    ଫେରନ୍ତୁ FAIL;
}

/*
 * Handle register contents: @r.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
eval9_reg_contents(
    char_u	**arg,
    typval_T	*rettv,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		evaluate)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		vim9script = in_vim9script();

    ++*arg;	// skip '@'
    ଯଦି (evaluate)
    {
	ଯଦି (vim9script && IS_WHITE_OR_NUL(**arg))
	    semsg(_(e_syntax_error_at_str), *arg);
	ଅନ୍ୟଥା ଯଦି (vim9script && !valid_yank_reg(**arg, FALSE))
	    emsg_invreg(**arg);
	ଅନ୍ୟଥା
	{
	    rettv->v_type = VAR_STRING;
	    rettv->vval.v_string = get_reg_contents(**arg,
		    GREG_ପଦସମଷ୍ଟି_SRC);
	}
    }
    ଯଦି (**arg != NUL)
	++*arg;
}

/*
 * Handle a nested expression: (expression) or lambda: (arg) => expr
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
eval9_nested_ପଦସମଷ୍ଟି(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		evaluate)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret = NOTDONE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		vim9script = in_vim9script();

    ଯଦି (vim9script)
    {
	ret = get_lambda_tv(arg, rettv, TRUE, evalarg, ଖାଲି);
	ଯଦି (ret == OK && evaluate)
	{
	    ufunc_T *ufunc = rettv->vval.v_partial->pt_func;

	    // Compile it here to get the return type.  The return
	    // type is optional, when it's missing use t_unknown.
	    // This is recognized in compile_return().
	    ଯଦି (ufunc->uf_ret_type->tt_type == VAR_VOID)
		ufunc->uf_ret_type = &t_unknown;
	    ଯଦି (compile_def_function(ufunc, FALSE,
				get_compile_type(ufunc), ଖାଲି) == FAIL)
	    {
		clear_tv(rettv);
		ret = FAIL;
	    }
	}
    }
    ଯଦି (ret == NOTDONE)
    {
	*arg = skipwhite_and_linebreak(*arg + 1, evalarg);
	ଯଦି (**arg == ')')
	    // empty tuple
	    ret = eval_tuple(arg, rettv, evalarg, TRUE);
	ଅନ୍ୟଥା
	{
	    ret = eval1(arg, rettv, evalarg);	// recursive!
	    ଯଦି (ret != OK)
		ଫେରନ୍ତୁ ret;

	    *arg = skipwhite_and_linebreak(*arg, evalarg);

	    ଯଦି (**arg == ',')
		// tuple
		ret = eval_tuple(arg, rettv, evalarg, TRUE);
	    ଅନ୍ୟଥା ଯଦି (**arg == ')')
		++*arg;
	    ଅନ୍ୟଥା ଯଦି (ret == OK)
	    {
		emsg(_(e_missing_closing_paren));
		clear_tv(rettv);
		ret = FAIL;
	    }
	}
    }

    ଫେରନ୍ତୁ ret;
}

/*
* Handle be a variable or function name.
* Can also be a curly-braces kind of name: {expr}.
*/
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
eval9_var_func_name(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		evaluate,
    char_u	**name_start)
{
    char_u	*s;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    char_u	*alias;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret = OK;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		vim9script = in_vim9script();

    s = *arg;
    len = get_name_len(arg, &alias, evaluate, TRUE);
    ଯଦି (alias != ଖାଲି)
	s = alias;

    ଯଦି (len <= 0)
	ret = FAIL;
    ଅନ୍ୟଥା
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	flags = evalarg == ଖାଲି ? 0 : evalarg->eval_flags;

	ଯଦି (evaluate && vim9script && len == 1 && *s == '_')
	{
	    emsg(_(e_cannot_use_underscore_here));
	    ret = FAIL;
	}
	ଅନ୍ୟଥା ଯଦି (evaluate && vim9script && len > 2
						&& s[0] == 's' && s[1] == ':')
	{
	    semsg(_(e_cannot_use_s_colon_in_vim9_script_str), s);
	    ret = FAIL;
	}
	ଅନ୍ୟଥା ଯଦି ((vim9script ? **arg : *skipwhite(*arg)) == '('
				|| (vim9script && generic_func_call(arg)))
	{
	    // "name(..."  recursive!
	    *arg = skipwhite(*arg);
	    ret = eval_func(arg, evalarg, s, len, rettv, flags, ଖାଲି);
	}
	ଅନ୍ୟଥା ଯଦି (evaluate)
	{
	    // get the value of "true", "false", etc. or a variable
	    ret = FAIL;
	    ଯଦି (vim9script)
		ret = handle_predefined(s, len, rettv);
	    ଯଦି (ret == FAIL)
	    {
		*name_start = s;
		ret = eval_variable(s, len, 0, rettv, ଖାଲି,
					EVAL_VAR_VERBOSE + EVAL_VAR_IMPORT);

		// skip the generic function arguments (if present)
		// they are already processed by eval_variable
		ଯଦି (ret == OK && vim9script && **arg == '<'
						&& rettv->v_type == VAR_FUNC)
		    ret = skip_generic_func_type_args(arg);
	    }
	}
	ଅନ୍ୟଥା
	{
	    // skip the name
	    check_vars(s, len);

	    // Skip the type arguments of a generic function, its name starts
	    // with an uppercase letter.
	    ଯଦି (vim9script && **arg == '<' && ASCII_ISUPPER(*s))
		skip_generic_func_type_args(arg);
	    ret = OK;
	}
    }
    vim_free(alias);

    ଫେରନ୍ତୁ ret;
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
eval9(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		want_string)	// after "." operator
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		evaluate = evalarg != ଖାଲି
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    char_u	*name_start = ଖାଲି;
    char_u	*start_leader, *end_leader;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret = OK;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	recurse = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		vim9script = in_vim9script();

    /*
     * Initialise variable so that clear_tv() can't mistake this for a
     * string and free a string that isn't there.
     */
    rettv->v_type = VAR_UNKNOWN;

    /*
     * Skip '!', '-' and '+' characters.  They are handled later.
     */
    start_leader = *arg;
    ଯଦି (eval_leader(arg, vim9script) == FAIL)
	ଫେରନ୍ତୁ FAIL;
    end_leader = *arg;

    ଯଦି (**arg == '.' && (!SAFE_isdigit(*(*arg + 1)) || in_old_script(2)))
    {
	semsg(_(e_invalid_expression_str), *arg);
	++*arg;
	ଫେରନ୍ତୁ FAIL;
    }

    // Limit recursion to 1000 levels.  At least at 10000 we run out of stack
    // and crash.  With MSVC the stack is smaller.
    ଯଦି (recurse ==
#ifdef _MSC_VER
		    300
#ଅନ୍ୟଥା
		    1000
#endif
		    )
    {
	semsg(_(e_expression_too_recursive_str), *arg);
	ଫେରନ୍ତୁ FAIL;
    }
    ++recurse;

    ବାଛ (**arg)
    {
    /*
     * Number constant.
     */
    ସ୍ଥିତି '0':
    ସ୍ଥିତି '1':
    ସ୍ଥିତି '2':
    ସ୍ଥିତି '3':
    ସ୍ଥିତି '4':
    ସ୍ଥିତି '5':
    ସ୍ଥିତି '6':
    ସ୍ଥିତି '7':
    ସ୍ଥିତି '8':
    ସ୍ଥିତି '9':
    ସ୍ଥିତି '.':	ret = eval_ସଂଖ୍ୟା(arg, rettv, evaluate, want_string);

		// Apply prefixed "-" and "+" now.  Matters especially when
		// "->" follows.
		ଯଦି (ret == OK && evaluate && end_leader > start_leader
						  && rettv->v_type != VAR_BLOB)
		    ret = eval9_leader(rettv, TRUE, start_leader, &end_leader);
		ଭାଙ୍ଗ;

    /*
     * String constant: "string".
     */
    ସ୍ଥିତି '"':	ret = eval_string(arg, rettv, evaluate, FALSE);
		ଭାଙ୍ଗ;

    /*
     * Literal string constant: 'str''ing'.
     */
    ସ୍ଥିତି '\'':	ret = eval_lit_string(arg, rettv, evaluate, FALSE);
		ଭାଙ୍ଗ;

    /*
     * List: [expr, expr]
     */
    ସ୍ଥିତି '[':	ret = eval_list(arg, rettv, evalarg, TRUE);
		ଭାଙ୍ଗ;

    /*
     * Literal Dictionary: #{key: val, key: val}
     */
    ସ୍ଥିତି '#':	ret = eval_lit_dict(arg, rettv, evalarg);
		ଭାଙ୍ଗ;

    /*
     * Lambda: {arg, arg -> expr}
     * Dictionary: {'key': val, 'key': val}
     */
    ସ୍ଥିତି '{':	ଯଦି (vim9script)
		    ret = NOTDONE;
		ଅନ୍ୟଥା
		    ret = get_lambda_tv(arg, rettv, vim9script, evalarg, ଖାଲି);
		ଯଦି (ret == NOTDONE)
		    ret = eval_dict(arg, rettv, evalarg, FALSE);
		ଭାଙ୍ଗ;

    /*
     * Option value: &name
     */
    ସ୍ଥିତି '&':	ret = eval_option(arg, rettv, evaluate);
		ଭାଙ୍ଗ;

    /*
     * Environment variable: $VAR.
     * Interpolated string: $"string" or $'string'.
     */
    ସ୍ଥିତି '$':	ଯଦି ((*arg)[1] == '"' || (*arg)[1] == '\'')
		    ret = eval_interp_string(arg, rettv, evaluate);
		ଅନ୍ୟଥା
		    ret = eval_env_var(arg, rettv, evaluate);
		ଭାଙ୍ଗ;

    /*
     * Register contents: @r.
     */
    ସ୍ଥିତି '@':	eval9_reg_contents(arg, rettv, evaluate);
		ଭାଙ୍ଗ;

    /*
     * nested expression: (expression).
     * or lambda: (arg) => expr
     * or tuple
     */
    ସ୍ଥିତି '(':	ret = eval9_nested_ପଦସମଷ୍ଟି(arg, rettv, evalarg, evaluate);
		ଭାଙ୍ଗ;

    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:	ret = NOTDONE;
		ଭାଙ୍ଗ;
    }

    ଯଦି (ret == NOTDONE)
    {
	/*
	 * Must be a variable or function name.
	 * Can also be a curly-braces kind of name: {expr}.
	 */
	ret = eval9_var_func_name(arg, rettv, evalarg, evaluate, &name_start);
    }

    // Handle following '[', '(' and '.' for expr[expr], expr.name,
    // expr(expr), expr->name(expr)
    ଯଦି (ret == OK)
	ret = handle_subscript(arg, name_start, rettv, evalarg, evaluate);

    /*
     * Apply logical NOT and unary '-', from right to left, ignore '+'.
     */
    ଯଦି (ret == OK && evaluate && end_leader > start_leader)
	ret = eval9_leader(rettv, FALSE, start_leader, &end_leader);

    --recurse;
    ଫେରନ୍ତୁ ret;
}

/*
 * Apply the leading "!" and "-" before an eval9 expression to "rettv".
 * When "numeric_only" is TRUE only handle "+" and "-".
 * Adjusts "end_leaderp" until it is at "start_leader".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
eval9_leader(
	typval_T    *rettv,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    numeric_only,
	char_u	    *start_leader,
	char_u	    **end_leaderp)
{
    char_u	*end_leader = *end_leaderp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret = OK;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		error = FALSE;
    varnumber_T val = 0;
    vartype_T	type = rettv->v_type;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		vim9script = in_vim9script();
    float_T	    f = 0.0;

    ଯଦି (rettv->v_type == VAR_FLOAT)
	f = rettv->vval.v_float;
    ଅନ୍ୟଥା
    {
	ଯେତେବେଳେ (VIM_ISWHITE(end_leader[-1]))
	    --end_leader;
	ଯଦି (vim9script && end_leader[-1] == '!')
	{
	    // Is this typeval supported with the ! operator?
	    ଯଦି (check_typval_is_ମୂଲ୍ୟ(rettv) == FAIL)
	    {
		clear_tv(rettv);
		ଫେରନ୍ତୁ FAIL;
	    }
	    val = tv2bool(rettv);
	}
	ଅନ୍ୟଥା
	    val = tv_get_ସଂଖ୍ୟା_chk(rettv, &error);
    }
    ଯଦି (error)
    {
	clear_tv(rettv);
	ret = FAIL;
    }
    ଅନ୍ୟଥା
    {
	ଯେତେବେଳେ (end_leader > start_leader)
	{
	    --end_leader;
	    ଯଦି (*end_leader == '!')
	    {
		ଯଦି (numeric_only)
		{
		    ++end_leader;
		    ଭାଙ୍ଗ;
		}
		ଯଦି (rettv->v_type == VAR_FLOAT)
		{
		    rettv->v_type = VAR_BOOL;
		    val = f == 0.0 ? VVAL_TRUE : VVAL_FALSE;
		    type = VAR_BOOL;
		}
		ଅନ୍ୟଥା
		{
		    val = !val;
		    type = VAR_BOOL;
		}
	    }
	    ଅନ୍ୟଥା ଯଦି (*end_leader == '-')
	    {
		ଯଦି (rettv->v_type == VAR_FLOAT)
		    f = -f;
		ଅନ୍ୟଥା
		{
		    val = -val;
		    type = VAR_ସଂଖ୍ୟା;
		}
	    }
	}
	ଯଦି (rettv->v_type == VAR_FLOAT)
	{
	    clear_tv(rettv);
	    rettv->vval.v_float = f;
	}
	ଅନ୍ୟଥା
	{
	    clear_tv(rettv);
	    ଯଦି (vim9script)
		rettv->v_type = type;
	    ଅନ୍ୟଥା
		rettv->v_type = VAR_ସଂଖ୍ୟା;
	    rettv->vval.v_ସଂଖ୍ୟା = val;
	}
    }
    *end_leaderp = end_leader;
    ଫେରନ୍ତୁ ret;
}

/*
 * Call the function referred to in "rettv".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
call_func_rettv(
	char_u	    **arg,
	evalarg_T   *evalarg,
	typval_T    *rettv,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    evaluate,
	dict_T	    *selfdict,
	typval_T    *basetv)
{
    partial_T	*pt = ଖାଲି;
    funcexe_T	funcexe;
    typval_T	functv;
    char_u	*s;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret;

    // need to copy the funcref so that we can clear rettv
    ଯଦି (evaluate)
    {
	functv = *rettv;
	rettv->v_type = VAR_UNKNOWN;

	// Invoke the function.  Recursive!
	ଯଦି (functv.v_type == VAR_PARTIAL)
	{
	    pt = functv.vval.v_partial;
	    s = partial_name(pt);
	}
	ଅନ୍ୟଥା
	{
	    s = functv.vval.v_string;
	    ଯଦି (s == ଖାଲି || *s == NUL)
	    {
		emsg(_(e_empty_function_name));
		ret = FAIL;
		ଯାଅ theend;
	    }
	}
    }
    ଅନ୍ୟଥା
	s = (char_u *)"";

    CLEAR_FIELD(funcexe);
    funcexe.fe_firstline = curwin->w_cursor.lnum;
    funcexe.fe_lastline = curwin->w_cursor.lnum;
    funcexe.fe_evaluate = evaluate;
    funcexe.fe_partial = pt;
    ଯଦି (pt != ଖାଲି)
    {
	funcexe.fe_object = pt->pt_obj;
	ଯଦି (funcexe.fe_object != ଖାଲି)
	    ++funcexe.fe_object->obj_refcount;
    }
    funcexe.fe_selfdict = selfdict;
    funcexe.fe_basetv = basetv;
    ଯଦି (evalarg != ଖାଲି)
	funcexe.fe_cctx = evalarg->eval_cctx;
    ret = get_func_tv(s, -1, rettv, arg, evalarg, &funcexe);

theend:
    // Clear the funcref afterwards, so that deleting it while
    // evaluating the arguments is possible (see test55).
    ଯଦି (evaluate)
	clear_tv(&functv);

    ଫେରନ୍ତୁ ret;
}

/*
 * Evaluate "->method()".
 * "*arg" points to "method".
 * Returns FAIL or OK. "*arg" is advanced to after the ')'.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
eval_lambda(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		verbose)	// give error messages
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		evaluate = evalarg != ଖାଲି
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    typval_T	base = *rettv;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret;

    rettv->v_type = VAR_UNKNOWN;

    ଯଦି (**arg == '{')
    {
	// ->{lambda}()
	ret = get_lambda_tv(arg, rettv, FALSE, evalarg, ଖାଲି);
    }
    ଅନ୍ୟଥା
    {
	// ->(lambda)()
	++*arg;
	ret = eval1(arg, rettv, evalarg);
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	ଯଦି (**arg != ')')
	{
	    emsg(_(e_missing_closing_paren));
	    ଫେରନ୍ତୁ FAIL;
	}
	ଯଦି (rettv->v_type != VAR_STRING && rettv->v_type != VAR_FUNC
					       && rettv->v_type != VAR_PARTIAL)
	{
	    emsg(_(e_string_or_function_required_for_arrow_parens_ପଦସମଷ୍ଟି));
	    ଫେରନ୍ତୁ FAIL;
	}
	++*arg;
    }
    ଯଦି (ret != OK)
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (**arg != '(')
    {
	ଯଦି (verbose)
	{
	    ଯଦି (*skipwhite(*arg) == '(')
		emsg(_(e_no_white_space_allowed_before_parenthesis));
	    ଅନ୍ୟଥା
		semsg(_(e_missing_parenthesis_str), "lambda");
	}
	clear_tv(rettv);
	ret = FAIL;
    }
    ଅନ୍ୟଥା
	ret = call_func_rettv(arg, evalarg, rettv, evaluate, ଖାଲି, &base);

    // Clear the funcref afterwards, so that deleting it while
    // evaluating the arguments is possible (see test55).
    ଯଦି (evaluate)
	clear_tv(&base);

    ଫେରନ୍ତୁ ret;
}

/*
 * Evaluate "->method()".
 * "*arg" points to "method".
 * Returns FAIL or OK. "*arg" is advanced to after the ')'.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
eval_method(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		verbose)	// give error messages
{
    char_u	*name;
    long	len;
    char_u	*alias;
    char_u	*tofree = ଖାଲି;
    typval_T	base = *rettv;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret = OK;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		evaluate = evalarg != ଖାଲି
				      && (evalarg->eval_flags & EVAL_EVALUATE);

    rettv->v_type = VAR_UNKNOWN;

    name = *arg;
    len = get_name_len(arg, &alias, evaluate, evaluate);
    ଯଦି (alias != ଖାଲି)
	name = alias;

    ଯଦି (len <= 0)
    {
	ଯଦି (verbose)
	    emsg(_(e_missing_name_after_method));
	ret = FAIL;
    }
    ଅନ୍ୟଥା
    {
	char_u *paren;

	// If there is no "(" immediately following, but there is further on,
	// it can be "import.Func()", "dict.Func()", "list[nr]", etc.
	// Does not handle anything where "(" is part of the expression.
	*arg = skipwhite(*arg);

	ଯଦି (**arg != '(' && alias == ଖାଲି
				    && (paren = vim_strchr(*arg, '(')) != ଖାଲି)
	{
	    *arg = name;

	    // Truncate the name at the "(".  Avoid trying to get another line
	    // by making "getline" NULL.
	    *paren = NUL;
	    char_u	*(*getline)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ଶୂନ୍ୟ *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, getline_opt_T) = ଖାଲି;
	    ଯଦି (evalarg != ଖାଲି)
	    {
		getline = evalarg->eval_getline;
		evalarg->eval_getline = ଖାଲି;
	    }

	    char_u *deref = deref_function_name(arg, &tofree, evalarg, verbose);
	    ଯଦି (deref == ଖାଲି)
	    {
		*arg = name + len;
		ret = FAIL;
	    }
	    ଅନ୍ୟଥା
	    {
		name = deref;
		len = (long)STRLEN(name);
	    }

	    *paren = '(';
	    ଯଦି (getline != ଖାଲି)
		evalarg->eval_getline = getline;
	}

	ଯଦି (ret == OK)
	{
	    *arg = skipwhite(*arg);

	    ଯଦି (**arg != '(')
	    {
		ଯଦି (verbose)
		    semsg(_(e_missing_parenthesis_str), name);
		ret = FAIL;
	    }
	    ଅନ୍ୟଥା ଯଦି (VIM_ISWHITE((*arg)[-1]))
	    {
		ଯଦି (verbose)
		    emsg(_(e_no_white_space_allowed_before_parenthesis));
		ret = FAIL;
	    }
	    ଅନ୍ୟଥା
		ret = eval_func(arg, evalarg, name, len, rettv,
					  evaluate ? EVAL_EVALUATE : 0, &base);
	}
    }

    // Clear the funcref afterwards, so that deleting it while
    // evaluating the arguments is possible (see test55).
    ଯଦି (evaluate)
	clear_tv(&base);
    vim_free(tofree);

    ଯଦି (alias != ଖାଲି)
	vim_free(alias);

    ଫେରନ୍ତୁ ret;
}

/*
 * Evaluate an "[expr]" or "[expr:expr]" index.  Also "dict.key".
 * "*arg" points to the '[' or '.'.
 * Returns FAIL or OK. "*arg" is advanced to after the ']'.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
eval_index(
    char_u	**arg,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		verbose)	// give error messages
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		evaluate = evalarg != ଖାଲି
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		empty1 = FALSE, empty2 = FALSE;
    typval_T	var1, var2;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		range = FALSE;
    char_u	*ଚାବି = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		keylen = -1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		vim9script = in_vim9script();

    ଯଦି (check_can_index(rettv, evaluate, verbose) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    init_tv(&var1);
    init_tv(&var2);
    ଯଦି (**arg == '.')
    {
	/*
	 * dict.name
	 */
	ଚାବି = *arg + 1;
	ପାଇଁ (keylen = 0; eval_isdictc(ଚାବି[keylen]); ++keylen)
	    ;
	ଯଦି (keylen == 0)
	    ଫେରନ୍ତୁ FAIL;
	ଯଦି (vim9script && ଚାବି[keylen] == '<')
	{
	    // skip generic type arguments
	    char_u	*p = &ଚାବି[keylen];

	    ଯଦି (skip_generic_func_type_args(&p) == FAIL)
		ଫେରନ୍ତୁ FAIL;
	    keylen = p - ଚାବି;
	}
	*arg = ଚାବି + keylen;
    }
    ଅନ୍ୟଥା
    {
	/*
	 * something[idx]
	 *
	 * Get the (first) variable from inside the [].
	 */
	*arg = skipwhite_and_linebreak(*arg + 1, evalarg);
	ଯଦି (**arg == ':')
	    empty1 = TRUE;
	ଅନ୍ୟଥା ଯଦି (eval1(arg, &var1, evalarg) == FAIL)	// recursive!
	    ଫେରନ୍ତୁ FAIL;
	ଅନ୍ୟଥା ଯଦି (vim9script && **arg == ':')
	{
	    semsg(_(e_white_space_required_before_and_after_str_at_str),
								    ":", *arg);
	    clear_tv(&var1);
	    ଫେରନ୍ତୁ FAIL;
	}
	ଅନ୍ୟଥା ଯଦି (evaluate)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା error = FALSE;

	    // allow for indexing with float
	    ଯଦି (vim9script && rettv->v_type == VAR_DICT
						   && var1.v_type == VAR_FLOAT)
	    {
		var1.vval.v_string = typval_tostring(&var1, TRUE);
		var1.v_type = VAR_STRING;
	    }

	    ଯଦି (vim9script && (rettv->v_type == VAR_LIST
						|| rettv->v_type == VAR_TUPLE))
		tv_get_ସଂଖ୍ୟା_chk(&var1, &error);
	    ଅନ୍ୟଥା
		error = tv_get_string_chk(&var1) == ଖାଲି;
	    ଯଦି (error)
	    {
		// not a number or string
		clear_tv(&var1);
		ଫେରନ୍ତୁ FAIL;
	    }
	}

	/*
	 * Get the second variable from inside the [:].
	 */
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	ଯଦି (**arg == ':')
	{
	    range = TRUE;
	    ++*arg;
	    ଯଦି (vim9script && !IS_WHITE_OR_NUL(**arg) && **arg != ']')
	    {
		semsg(_(e_white_space_required_before_and_after_str_at_str),
								":", *arg - 1);
		ଯଦି (!empty1)
		    clear_tv(&var1);
		ଫେରନ୍ତୁ FAIL;
	    }
	    *arg = skipwhite_and_linebreak(*arg, evalarg);
	    ଯଦି (**arg == ']')
		empty2 = TRUE;
	    ଅନ୍ୟଥା ଯଦି (eval1(arg, &var2, evalarg) == FAIL)	// recursive!
	    {
		ଯଦି (!empty1)
		    clear_tv(&var1);
		ଫେରନ୍ତୁ FAIL;
	    }
	    ଅନ୍ୟଥା ଯଦି (evaluate && tv_get_string_chk(&var2) == ଖାଲି)
	    {
		// not a number or string
		ଯଦି (!empty1)
		    clear_tv(&var1);
		clear_tv(&var2);
		ଫେରନ୍ତୁ FAIL;
	    }
	}

	// Check for the ']'.
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	ଯଦି (**arg != ']')
	{
	    ଯଦି (verbose)
		emsg(_(e_missing_closing_square_brace));
	    clear_tv(&var1);
	    ଯଦି (range)
		clear_tv(&var2);
	    ଫେରନ୍ତୁ FAIL;
	}
	*arg = *arg + 1;	// skip over the ']'
    }

    ଯଦି (evaluate)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା res = eval_index_inner(rettv, range,
		empty1 ? ଖାଲି : &var1, empty2 ? ଖାଲି : &var2, FALSE,
		ଚାବି, keylen, verbose);

	ଯଦି (!empty1)
	    clear_tv(&var1);
	ଯଦି (range)
	    clear_tv(&var2);
	ଫେରନ୍ତୁ res;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Check if "rettv" can have an [index] or [sli:ce]
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_can_index(typval_T *rettv, ପୂର୍ଣ୍ଣସଂଖ୍ୟା evaluate, ପୂର୍ଣ୍ଣସଂଖ୍ୟା verbose)
{
    ବାଛ (rettv->v_type)
    {
	ସ୍ଥିତି VAR_FUNC:
	ସ୍ଥିତି VAR_PARTIAL:
	    ଯଦି (verbose)
		emsg(_(e_cannot_index_a_funcref));
	    ଫେରନ୍ତୁ FAIL;
	ସ୍ଥିତି VAR_FLOAT:
	    ଯଦି (verbose)
		emsg(_(e_using_float_as_string));
	    ଫେରନ୍ତୁ FAIL;
	ସ୍ଥିତି VAR_BOOL:
	ସ୍ଥିତି VAR_SPECIAL:
	ସ୍ଥିତି VAR_JOB:
	ସ୍ଥିତି VAR_CHANNEL:
	ସ୍ଥିତି VAR_INSTR:
	ସ୍ଥିତି VAR_OBJECT:
	    ଯଦି (verbose)
		emsg(_(e_cannot_index_special_variable));
	    ଫେରନ୍ତୁ FAIL;
	ସ୍ଥିତି VAR_CLASS:
	ସ୍ଥିତି VAR_TYPEALIAS:
	    ଯଦି (verbose)
		check_typval_is_ମୂଲ୍ୟ(rettv);
	    ଫେରନ୍ତୁ FAIL;
	ସ୍ଥିତି VAR_UNKNOWN:
	ସ୍ଥିତି VAR_ANY:
	ସ୍ଥିତି VAR_VOID:
	    ଯଦି (evaluate)
	    {
		emsg(_(e_cannot_index_special_variable));
		ଫେରନ୍ତୁ FAIL;
	    }
	    // FALLTHROUGH

	ସ୍ଥିତି VAR_STRING:
	ସ୍ଥିତି VAR_LIST:
	ସ୍ଥିତି VAR_TUPLE:
	ସ୍ଥିତି VAR_DICT:
	ସ୍ଥିତି VAR_BLOB:
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_ସଂଖ୍ୟା:
	    ଯଦି (in_vim9script())
		emsg(_(e_cannot_index_ସଂଖ୍ୟା));
	    ଭାଙ୍ଗ;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Apply index or range to "rettv".
 * "var1" is the first index, NULL for [:expr].
 * "var2" is the second index, NULL for [expr] and [expr: ]
 * "exclusive" is TRUE for slice(): second index is exclusive, use character
 * index for string.
 * Alternatively, "key" is not NULL, then key[keylen] is the dict index.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
eval_index_inner(
	typval_T    *rettv,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    is_range,
	typval_T    *var1,
	typval_T    *var2,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    exclusive,
	char_u	    *ଚାବି,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    keylen,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    verbose)
{
    varnumber_T	    n1, n2 = 0;
    long	    len;

    n1 = 0;
    ଯଦି (var1 != ଖାଲି && rettv->v_type != VAR_DICT)
	n1 = tv_get_ସଂଖ୍ୟା(var1);

    ଯଦି (is_range)
    {
	ଯଦି (rettv->v_type == VAR_DICT)
	{
	    ଯଦି (verbose)
		emsg(_(e_cannot_slice_dictionary));
	    ଫେରନ୍ତୁ FAIL;
	}
	ଯଦି (var2 != ଖାଲି)
	    n2 = tv_get_ସଂଖ୍ୟା(var2);
	ଅନ୍ୟଥା
	    n2 = VARNUM_MAX;
    }

    ବାଛ (rettv->v_type)
    {
	ସ୍ଥିତି VAR_UNKNOWN:
	ସ୍ଥିତି VAR_ANY:
	ସ୍ଥିତି VAR_VOID:
	ସ୍ଥିତି VAR_FUNC:
	ସ୍ଥିତି VAR_PARTIAL:
	ସ୍ଥିତି VAR_FLOAT:
	ସ୍ଥିତି VAR_BOOL:
	ସ୍ଥିତି VAR_SPECIAL:
	ସ୍ଥିତି VAR_JOB:
	ସ୍ଥିତି VAR_CHANNEL:
	ସ୍ଥିତି VAR_INSTR:
	ସ୍ଥିତି VAR_CLASS:
	ସ୍ଥିତି VAR_OBJECT:
	ସ୍ଥିତି VAR_TYPEALIAS:
	    ଭାଙ୍ଗ; // not evaluating, skipping over subscript

	ସ୍ଥିତି VAR_ସଂଖ୍ୟା:
	ସ୍ଥିତି VAR_STRING:
	    {
		char_u	*s = tv_get_string(rettv);

		len = (long)STRLEN(s);
		ଯଦି (in_vim9script() || exclusive)
		{
		    ଯଦି (is_range)
			s = string_slice(s, n1, n2, exclusive);
		    ଅନ୍ୟଥା
			s = char_from_string(s, n1);
		}
		ଅନ୍ୟଥା ଯଦି (is_range)
		{
		    // The resulting variable is a substring.  If the indexes
		    // are out of range the result is empty.
		    ଯଦି (n1 < 0)
		    {
			n1 = len + n1;
			ଯଦି (n1 < 0)
			    n1 = 0;
		    }
		    ଯଦି (n2 < 0)
			n2 = len + n2;
		    ଅନ୍ୟଥା ଯଦି (n2 >= len)
			n2 = len;
		    ଯଦି (n1 >= len || n2 < 0 || n1 > n2)
			s = ଖାଲି;
		    ଅନ୍ୟଥା
			s = vim_strnsave(s + n1, n2 - n1 + 1);
		}
		ଅନ୍ୟଥା
		{
		    // The resulting variable is a string of a single
		    // character.  If the index is too big or negative the
		    // result is empty.
		    ଯଦି (n1 >= len || n1 < 0)
			s = ଖାଲି;
		    ଅନ୍ୟଥା
			s = vim_strnsave(s + n1, 1);
		}
		clear_tv(rettv);
		rettv->v_type = VAR_STRING;
		rettv->vval.v_string = s;
	    }
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି VAR_BLOB:
	    blob_slice_or_index(rettv->vval.v_blob, is_range, n1, n2,
							     exclusive, rettv);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି VAR_LIST:
	    ଯଦି (var1 == ଖାଲି)
		n1 = 0;
	    ଯଦି (var2 == ଖାଲି)
		n2 = VARNUM_MAX;
	    ଯଦି (list_slice_or_index(rettv->vval.v_list,
			  is_range, n1, n2, exclusive, rettv, verbose) == FAIL)
		ଫେରନ୍ତୁ FAIL;
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି VAR_TUPLE:
	    ଯଦି (var1 == ଖାଲି)
		n1 = 0;
	    ଯଦି (var2 == ଖାଲି)
		n2 = VARNUM_MAX;
	    ଯଦି (tuple_slice_or_index(rettv->vval.v_tuple,
			  is_range, n1, n2, exclusive, rettv, verbose) == FAIL)
		ଫେରନ୍ତୁ FAIL;
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି VAR_DICT:
	    {
		dictitem_T	*item;
		typval_T	tmp;

		ଯଦି (ଚାବି == ଖାଲି)
		{
		    ଚାବି = tv_get_string_chk(var1);
		    ଯଦି (ଚାବି == ଖାଲି)
			ଫେରନ୍ତୁ FAIL;
		}

		item = dict_find(rettv->vval.v_dict, ଚାବି, keylen);

		ଯଦି (item == ଖାଲି)
		{
		    ଯଦି (verbose)
		    {
			ଯଦି (keylen > 0)
			    ଚାବି[keylen] = NUL;
			semsg(_(e_ଚାବି_not_present_in_dictionary_str), ଚାବି);
		    }
		    ଫେରନ୍ତୁ FAIL;
		}

		copy_tv(&item->di_tv, &tmp);
		clear_tv(rettv);
		*rettv = tmp;
	    }
	    ଭାଙ୍ଗ;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Return the function name of partial "pt".
 */
    char_u *
partial_name(partial_T *pt)
{
    ଯଦି (pt != ଖାଲି)
    {
	ଯଦି (pt->pt_name != ଖାଲି)
	    ଫେରନ୍ତୁ pt->pt_name;
	ଯଦି (pt->pt_func != ଖାଲି)
	    ଫେରନ୍ତୁ pt->pt_func->uf_name;
    }
    ଫେରନ୍ତୁ (char_u *)"";
}

    ସ୍ଥିର ଶୂନ୍ୟ
partial_free(partial_T *pt)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

    ପାଇଁ (i = 0; i < pt->pt_argc; ++i)
	clear_tv(&pt->pt_argv[i]);
    vim_free(pt->pt_argv);
    dict_unref(pt->pt_dict);
    ଯଦି (pt->pt_name != ଖାଲି)
    {
	func_unref(pt->pt_name);
	vim_free(pt->pt_name);
    }
    ଅନ୍ୟଥା
	func_ptr_unref(pt->pt_func);
    ଯଦି (pt->pt_obj != ଖାଲି)
	object_unref(pt->pt_obj);

    // "out_up" is no longer used, decrement refcount on partial that owns it.
    partial_unref(pt->pt_outer.out_up_partial);

    // Using pt_outer from another partial.
    partial_unref(pt->pt_outer_partial);

    // Decrease the reference count for the context of a closure.  If down
    // to the minimum it may be time to free it.
    ଯଦି (pt->pt_funcstack != ଖାଲି)
    {
	--pt->pt_funcstack->fs_refcount;
	funcstack_check_refcount(pt->pt_funcstack);
    }
    // Similarly for loop variables.
    ପାଇଁ (i = 0; i < MAX_LOOP_DEPTH; ++i)
	ଯଦି (pt->pt_loopvars[i] != ଖାଲି)
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
    ଶୂନ୍ୟ
partial_unref(partial_T *pt)
{
    ଯଦି (pt == ଖାଲି)
	ଫେରନ୍ତୁ;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	done = FALSE;

    ଯଦି (--pt->pt_refcount <= 0)
	partial_free(pt);

    // If the reference count goes down to one, the funcstack may be the
    // only reference and can be freed if no other partials reference it.
    ଅନ୍ୟଥା ଯଦି (pt->pt_refcount == 1)
    {
	// careful: if the funcstack is freed it may contain this partial
	// and it gets freed as well
	ଯଦି (pt->pt_funcstack != ଖାଲି)
	    done = funcstack_check_refcount(pt->pt_funcstack);

	ଯଦି (!done)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	depth;

	    ପାଇଁ (depth = 0; depth < MAX_LOOP_DEPTH; ++depth)
		ଯଦି (pt->pt_loopvars[depth] != ଖାଲି
			&& loopvars_check_refcount(pt->pt_loopvars[depth]))
		    ଭାଙ୍ଗ;
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
    ସ୍ଥିର char_u *
string_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		echo_style,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		composite_val)
{
    char_u	*r = ଖାଲି;

    ଯଦି (echo_style && !composite_val)
    {
	*tofree = ଖାଲି;
	r = tv->vval.v_string;
	ଯଦି (r == ଖାଲି)
	    r = (char_u *)"";
    }
    ଅନ୍ୟଥା
    {
	*tofree = string_quote(tv->vval.v_string, FALSE);
	r = *tofree;
    }

    ଫେରନ୍ତୁ r;
}

/*
 * Return a textual representation of a function in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "echo_style" is FALSE, put quotes around the function name as
 * "function()", otherwise does not put quotes around function name.
 * May return NULL.
 */
    ସ୍ଥିର char_u *
func_tv2string(typval_T *tv, char_u **tofree, ପୂର୍ଣ୍ଣସଂଖ୍ୟା echo_style)
{
    char_u	*r = ଖାଲି;
    char_u	buf[MAX_FUNC_NAME_LEN];

    ଯଦି (echo_style)
    {
	*tofree = ଖାଲି;

	ଯଦି (tv->vval.v_string == ଖାଲି)
	    r = (char_u *)"function()";
	ଅନ୍ୟଥା
	{
	    r = make_ufunc_name_readable(tv->vval.v_string, buf,
							MAX_FUNC_NAME_LEN);
	    ଯଦି (r == buf)
		r = *tofree = vim_strsave(buf);
	}
    }
    ଅନ୍ୟଥା
    {
	char_u *s = ଖାଲି;

	ଯଦି (tv->vval.v_string != ଖାଲି)
	    s = make_ufunc_name_readable(tv->vval.v_string, buf,
							MAX_FUNC_NAME_LEN);

	r = *tofree = string_quote(s, TRUE);
    }

    ଫେରନ୍ତୁ r;
}

/*
 * Return a textual representation of the object method in "tv", a VAR_PARTIAL.
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "echo_style" is FALSE, put quotes around the function name as
 * "function()", otherwise does not put quotes around function name.
 * May return NULL.
 */
    ସ୍ଥିର char_u *
method_tv2string(typval_T *tv, char_u **tofree, ପୂର୍ଣ୍ଣସଂଖ୍ୟା echo_style)
{
    char_u	buf[MAX_FUNC_NAME_LEN];
    partial_T	*pt = tv->vval.v_partial;

    size_t len = vim_snprintf((ଅକ୍ଷର *)buf, ଆକାର(buf), "<SNR>%d_%s.%s",
			   pt->pt_func->uf_script_ctx.sc_sid,
			   pt->pt_func->uf_class->class_name.string,
			   pt->pt_func->uf_name);
    ଯଦି (len >= ଆକାର(buf))
    {
	ଯଦି (echo_style)
	{
	    *tofree = ଖାଲି;
	    ଫେରନ୍ତୁ (char_u *)"function()";
	}
	ଅନ୍ୟଥା
	    ଫେରନ୍ତୁ *tofree = string_quote((char_u*)"", TRUE);
    }

    ଫେରନ୍ତୁ *tofree = echo_style ? vim_strsave(buf) : string_quote(buf, TRUE);
}

/*
 * Return a textual representation of a partial in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * "numbuf" is used for a number.  May return NULL.
 */
    ସ୍ଥିର char_u *
partial_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    char_u	*numbuf,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		copyID)
{
    char_u	*r = ଖାଲି;
    partial_T	*pt;
    char_u	*fname;
    garray_T	ga;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    char_u	*tf;

    pt = tv->vval.v_partial;
    fname = string_quote(pt == ଖାଲି ? ଖାଲି : partial_name(pt), FALSE);

    ga_init2(&ga, 1, 100);
    GA_CONCAT_LITERAL(&ga, "function(");
    ଯଦି (fname != ଖାଲି)
    {
	// When using uf_name prepend "g:" for a global function.
	ଯଦି (pt != ଖାଲି && pt->pt_name == ଖାଲି && fname[0] == '\''
						&& vim_isupper(fname[1]))
	{
	    GA_CONCAT_LITERAL(&ga, "'g:");
	    ga_concat(&ga, fname + 1);
	}
	ଅନ୍ୟଥା
	    ga_concat(&ga, fname);
	vim_free(fname);
    }
    ଯଦି (pt != ଖାଲି && pt->pt_argc > 0)
    {
	GA_CONCAT_LITERAL(&ga, ", [");
	ପାଇଁ (i = 0; i < pt->pt_argc; ++i)
	{
	    ଯଦି (i > 0)
		GA_CONCAT_LITERAL(&ga, ", ");
	    ga_concat(&ga, tv2string(&pt->pt_argv[i], &tf, numbuf, copyID));
	    vim_free(tf);
	}
	GA_CONCAT_LITERAL(&ga, "]");
    }
    ଯଦି (pt != ଖାଲି && pt->pt_dict != ଖାଲି)
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

    ଫେରନ୍ତୁ r;
}

/*
 * Return a textual representation of a List in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "copyID" is not zero replace recursive lists with "...".  When
 * "restore_copyID" is FALSE, repeated items in lists are replaced with "...".
 * May return NULL.
 */
    ସ୍ଥିର char_u *
list_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		copyID,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		restore_copyID)
{
    char_u	*r = ଖାଲି;

    ଯଦି (tv->vval.v_list == ଖାଲି)
    {
	// NULL list is equivalent to empty list.
	*tofree = ଖାଲି;
	r = (char_u *)"[]";
    }
    ଅନ୍ୟଥା ଯଦି (copyID != 0 && tv->vval.v_list->lv_copyID == copyID
	    && tv->vval.v_list->lv_len > 0)
    {
	*tofree = ଖାଲି;
	r = (char_u *)"[...]";
    }
    ଅନ୍ୟଥା
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା old_copyID;
	ଯଦି (restore_copyID)
	    old_copyID = tv->vval.v_list->lv_copyID;

	tv->vval.v_list->lv_copyID = copyID;
	*tofree = list2string(tv, copyID, restore_copyID);
	ଯଦି (restore_copyID)
	    tv->vval.v_list->lv_copyID = old_copyID;
	r = *tofree;
    }

    ଫେରନ୍ତୁ r;
}

/*
 * Return a textual representation of a Tuple in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "copyID" is not zero replace recursive lists with "...".  When
 * "restore_copyID" is FALSE, repeated items in tuples are replaced with "...".
 * May return NULL.
 */
    ସ୍ଥିର char_u *
tuple_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		copyID,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		restore_copyID)
{
    tuple_T	*tuple = tv->vval.v_tuple;
    char_u	*r = ଖାଲି;

    ଯଦି (tuple == ଖାଲି)
    {
	// NULL tuple is equivalent to an empty tuple.
	*tofree = ଖାଲି;
	r = (char_u *)"()";
    }
    ଅନ୍ୟଥା ଯଦି (copyID != 0 && tuple->tv_copyID == copyID
					&& tuple->tv_ବସ୍ତୁଗୁଡ଼ିକ.ga_len > 0)
    {
	*tofree = ଖାଲି;
	r = (char_u *)"(...)";
    }
    ଅନ୍ୟଥା
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା old_copyID;
	ଯଦି (restore_copyID)
	    old_copyID = tuple->tv_copyID;

	tuple->tv_copyID = copyID;
	*tofree = tuple2string(tv, copyID, restore_copyID);
	ଯଦି (restore_copyID)
	    tuple->tv_copyID = old_copyID;
	r = *tofree;
    }

    ଫେରନ୍ତୁ r;
}

/*
 * Return a textual representation of a Dict in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "copyID" is not zero replace recursive dicts with "...".
 * When "restore_copyID" is FALSE, repeated items in the dictionary are
 * replaced with "...".  May return NULL.
 */
    ସ୍ଥିର char_u *
dict_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		copyID,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		restore_copyID)
{
    char_u	*r = ଖାଲି;

    ଯଦି (tv->vval.v_dict == ଖାଲି)
    {
	// NULL dict is equivalent to empty dict.
	*tofree = ଖାଲି;
	r = (char_u *)"{}";
    }
    ଅନ୍ୟଥା ଯଦି (copyID != 0 && tv->vval.v_dict->dv_copyID == copyID
	    && tv->vval.v_dict->dv_hashtab.ht_used != 0)
    {
	*tofree = ଖାଲି;
	r = (char_u *)"{...}";
    }
    ଅନ୍ୟଥା
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା old_copyID;
	ଯଦି (restore_copyID)
	    old_copyID = tv->vval.v_dict->dv_copyID;

	tv->vval.v_dict->dv_copyID = copyID;
	*tofree = dict2string(tv, copyID, restore_copyID);
	ଯଦି (restore_copyID)
	    tv->vval.v_dict->dv_copyID = old_copyID;
	r = *tofree;
    }

    ଫେରନ୍ତୁ r;
}

/*
 * Return a textual representation of a job or a channel in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * "numbuf" is used for a number.
 * When "composite_val" is FALSE, put quotes around strings as "string()",
 * otherwise does not put quotes around strings.
 * May return NULL.
 */
    ସ୍ଥିର char_u *
jobchan_tv2string(
    typval_T	*tv UNUSED,
    char_u	**tofree UNUSED,
    char_u	*numbuf UNUSED,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		composite_val UNUSED)
{
    char_u	*r = ଖାଲି;

#ifdef FEAT_JOB_CHANNEL
    *tofree = ଖାଲି;

    ଯଦି (tv->v_type == VAR_JOB)
	r = job_to_string_buf(tv, numbuf);
    ଅନ୍ୟଥା
	r = channel_to_string_buf(tv, numbuf);

    ଯଦି (composite_val)
    {
	*tofree = string_quote(r, FALSE);
	r = *tofree;
    }
#endif

    ଫେରନ୍ତୁ r;
}

/*
 * Return a textual representation of a class in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * May return NULL.
 */
    ସ୍ଥିର char_u *
class_tv2string(typval_T *tv, char_u **tofree)
{
    char_u	*r = ଖାଲି;
    size_t	rsize;
    class_T	*cl = tv->vval.v_class;
    string_T	class_name = {(char_u *)"[unknown]", 9};
    string_T	s = {(char_u *)"class", 5};

    ଯଦି (cl != ଖାଲି)
    {
	class_name.string = cl->class_name.string;
	class_name.length = cl->class_name.length;
	ଯଦି (IS_INTERFACE(cl))
	    STR_LITERAL_SET(s, "interface");
	ଅନ୍ୟଥା ଯଦି (IS_ENUM(cl))
	    STR_LITERAL_SET(s, "enum");
    }

    rsize = s.length + 1 + class_name.length + 1;
    r = *tofree = alloc(rsize);
    ଯଦି (r != ଖାଲି)
	vim_snprintf((ଅକ୍ଷର *)r, rsize, "%s %s", s.string, (ଅକ୍ଷର *)class_name.string);

    ଫେରନ୍ତୁ r;
}

/*
 * Return a textual representation of an Object in "tv".
 * If the memory is allocated "tofree" is set to it, otherwise NULL.
 * When "copyID" is not zero replace recursive object with "...".
 * When "restore_copyID" is FALSE, repeated items in the object are
 * replaced with "...".  May return NULL.
 */
    ସ୍ଥିର char_u *
object_tv2string(
    typval_T	*tv,
    char_u	**tofree,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		copyID,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		restore_copyID,
    char_u	*numbuf,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		echo_style,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		composite_val)
{
    char_u	*r = ଖାଲି;

    object_T	*obj = tv->vval.v_object;
    ଯଦି (obj == ଖାଲି || obj->obj_class == ଖାଲି)
    {
	*tofree = ଖାଲି;
	r = (char_u *)"object of [unknown]";
    }
    ଅନ୍ୟଥା ଯଦି (copyID != 0 && obj->obj_copyID == copyID
	    && obj->obj_class->class_obj_member_ଗଣନା != 0)
    {
	size_t n = 25 + obj->obj_class->class_name.length;
	r = alloc(n);
	ଯଦି (r != ଖାଲି)
	    (ଶୂନ୍ୟ)vim_snprintf((ଅକ୍ଷର *)r, n, "object of %s {...}",
						obj->obj_class->class_name.string);
	*tofree = r;
    }
    ଅନ୍ୟଥା
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା old_copyID;
	ଯଦି (restore_copyID)
	    old_copyID = obj->obj_copyID;

	obj->obj_copyID = copyID;
	*tofree = object2string(obj, numbuf, copyID, echo_style,
				restore_copyID, composite_val);
	ଯଦି (restore_copyID)
	    obj->obj_copyID = old_copyID;
	r = *tofree;
    }

    ଫେରନ୍ତୁ r;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		copyID,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		echo_style,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		restore_copyID,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		composite_val)
{
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	recurse = 0;
    char_u	*r = ଖାଲି;

    ଯଦି (recurse >= DICT_MAXNEST)
    {
	ଯଦି (!did_echo_string_emsg)
	{
	    // Only give this message once for a recursive call to avoid
	    // flooding the user with errors.  And stop iterating over lists
	    // and dicts and objects.
	    did_echo_string_emsg = TRUE;
	    emsg(_(e_variable_nested_too_deep_for_displaying));
	}
	*tofree = ଖାଲି;
	ଫେରନ୍ତୁ (char_u *)"{E724}";
    }
    ++recurse;

    ବାଛ (tv->v_type)
    {
	ସ୍ଥିତି VAR_STRING:
	    r = string_tv2string(tv, tofree, echo_style, composite_val);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି VAR_FUNC:
	    r = func_tv2string(tv, tofree, echo_style);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି VAR_PARTIAL:
	    ଯଦି (tv->vval.v_partial == ଖାଲି
		    || tv->vval.v_partial->pt_obj == ଖାଲି)
		r = partial_tv2string(tv, tofree, numbuf, copyID);
	    ଅନ୍ୟଥା
		r = method_tv2string(tv, tofree, echo_style);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି VAR_BLOB:
	    r = blob2string(tv->vval.v_blob, tofree, numbuf);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି VAR_LIST:
	    r = list_tv2string(tv, tofree, copyID, restore_copyID);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି VAR_TUPLE:
	    r = tuple_tv2string(tv, tofree, copyID, restore_copyID);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି VAR_DICT:
	    r = dict_tv2string(tv, tofree, copyID, restore_copyID);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି VAR_ସଂଖ୍ୟା:
	ସ୍ଥିତି VAR_UNKNOWN:
	ସ୍ଥିତି VAR_ANY:
	ସ୍ଥିତି VAR_VOID:
	    *tofree = ଖାଲି;
	    r = tv_get_string_buf(tv, numbuf);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି VAR_JOB:
	ସ୍ଥିତି VAR_CHANNEL:
	    r = jobchan_tv2string(tv, tofree, numbuf, composite_val);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି VAR_INSTR:
	    *tofree = ଖାଲି;
	    r = (char_u *)"instructions";
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି VAR_CLASS:
	    r = class_tv2string(tv, tofree);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି VAR_OBJECT:
	    r = object_tv2string(tv, tofree, copyID, restore_copyID,
					 numbuf, echo_style, composite_val);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି VAR_FLOAT:
	    *tofree = ଖାଲି;
	    vim_snprintf((ଅକ୍ଷର *)numbuf, NUMBUFLEN, "%g", tv->vval.v_float);
	    r = numbuf;
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି VAR_BOOL:
	ସ୍ଥିତି VAR_SPECIAL:
	    *tofree = ଖାଲି;
	    r = (char_u *)get_var_special_name(tv->vval.v_ସଂଖ୍ୟା);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି VAR_TYPEALIAS:
	    *tofree = vim_strsave(tv->vval.v_typealias->ta_name);
	    r = *tofree;
	    ଯଦି (r == ଖାଲି)
		r = (char_u *)"";
	    ଭାଙ୍ଗ;
    }

    ଯଦି (--recurse == 0)
	did_echo_string_emsg = FALSE;
    ଫେରନ୍ତୁ r;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		copyID)
{
    ଫେରନ୍ତୁ echo_string_core(tv, tofree, numbuf, copyID, TRUE, FALSE, FALSE);
}

/*
 * Convert the specified byte index of line 'lnum' in buffer 'buf' to a
 * character index.  Works only for loaded buffers. Returns -1 on failure.
 * The index of the first byte and the first character is zero.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
buf_byteidx_to_charidx(buf_T *buf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା lnum, ପୂର୍ଣ୍ଣସଂଖ୍ୟା byteidx)
{
    char_u	*str;
    char_u	*t;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଗଣନା_2;

    ଯଦି (buf == ଖାଲି || buf->b_ml.ml_mfp == ଖାଲି)
	ଫେରନ୍ତୁ -1;

    ଯଦି (lnum > buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	lnum = buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;

    str = ml_get_buf(buf, lnum, FALSE);
    ଯଦି (str == ଖାଲି)
	ଫେରନ୍ତୁ -1;

    ଯଦି (*str == NUL)
	ଫେରନ୍ତୁ 0;

    // count the number of characters
    t = str;
    ପାଇଁ (ଗଣନା_2 = 0; *t != NUL && t <= str + byteidx; ଗଣନା_2++)
	t += mb_ptr2len(t);

    // In insert mode, when the cursor is at the end of a non-empty line,
    // byteidx points to the NUL character immediately past the end of the
    // string. In this case, add one to the character count.
    ଯଦି (*t == NUL && byteidx != 0 && t == str + byteidx)
	ଗଣନା_2++;

    ଫେରନ୍ତୁ ଗଣନା_2 - 1;
}

/*
 * Convert the specified character index of line 'lnum' in buffer 'buf' to a
 * byte index.  Works only for loaded buffers. Returns -1 on failure.
 * The index of the first byte and the first character is zero.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
buf_charidx_to_byteidx(buf_T *buf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା lnum, ପୂର୍ଣ୍ଣସଂଖ୍ୟା charidx)
{
    char_u	*str;
    char_u	*t;

    ଯଦି (buf == ଖାଲି || buf->b_ml.ml_mfp == ଖାଲି)
	ଫେରନ୍ତୁ -1;

    ଯଦି (lnum > buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	lnum = buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;

    str = ml_get_buf(buf, lnum, FALSE);
    ଯଦି (str == ଖାଲି)
	ଫେରନ୍ତୁ -1;

    // Convert the character offset to a byte offset
    t = str;
    ଯେତେବେଳେ (*t != NUL && --charidx > 0)
	t += mb_ptr2len(t);

    ଫେରନ୍ତୁ t - str;
}

/*
 * Translate a String variable into a position.
 * Returns NULL when there is an error.
 */
    pos_T *
var2fpos(
    typval_T	*varp,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dollar_lnum,	// TRUE when $ is last line
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*fnum,		// set to fnum for '0, 'A, etc.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		charcol)	// return character column
{
    char_u		*name;
    ସ୍ଥିର pos_T	pos;
    pos_T		*pp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			error = FALSE;

    // Argument can be [lnum, col, coladd].
    ଯଦି (varp->v_type == VAR_LIST)
    {
	list_T		*l;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
	listitem_T	*li;

	l = varp->vval.v_list;
	ଯଦି (l == ଖାଲି)
	    ଫେରନ୍ତୁ ଖାଲି;

	// Get the line number
	pos.lnum = list_find_nr(l, 0L, &error);
	ଯଦି (error || pos.lnum <= 0 || pos.lnum > curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	    ଫେରନ୍ତୁ ଖାଲି;	// invalid line number
	ଯଦି (charcol)
	    len = (long)mb_charlen(ml_get(pos.lnum));
	ଅନ୍ୟଥା
	    len = (long)ml_get_len(pos.lnum);

	// Get the column number
	// We accept "$" for the column number: last column.
	li = list_find(l, 1L);
	ଯଦି (li != ଖାଲି && li->li_tv.v_type == VAR_STRING
		&& li->li_tv.vval.v_string != ଖାଲି
		&& STRCMP(li->li_tv.vval.v_string, "$") == 0)
	{
	    pos.col = len + 1;
	}
	ଅନ୍ୟଥା
	{
	    pos.col = list_find_nr(l, 1L, &error);
	    ଯଦି (error)
		ଫେରନ୍ତୁ ଖାଲି;
	}

	// Accept a position up to the NUL after the line.
	ଯଦି (pos.col == 0 || (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)pos.col > len + 1)
	    ଫେରନ୍ତୁ ଖାଲି;	// invalid column number
	--pos.col;

	// Get the virtual offset.  Defaults to zero.
	pos.coladd = list_find_nr(l, 2L, &error);
	ଯଦି (error)
	    pos.coladd = 0;

	ଫେରନ୍ତୁ &pos;
    }

    ଯଦି (in_vim9script() && check_for_string_arg(varp, 0) == FAIL)
	ଫେରନ୍ତୁ ଖାଲି;

    name = tv_get_string_chk(varp);
    ଯଦି (name == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    error = TRUE;
    pos.lnum = 0;
    ଯଦି (name[0] == '.')
    {
	ଯଦି (!in_vim9script() || name[1] == NUL)
	{
	    error = FALSE;
	    // cursor
	    pos = curwin->w_cursor;
	}
    }
    ଅନ୍ୟଥା ଯଦି (name[0] == 'v' && name[1] == NUL)
    {
	// Visual start
	ଯଦି (VIsual_active)
	    pos = VIsual;
	ଅନ୍ୟଥା
	    pos = curwin->w_cursor;
    }
    ଅନ୍ୟଥା ଯଦି (name[0] == '\'')
    {
	ଯଦି (!in_vim9script() || (name[1] != NUL && name[2] == NUL))
	{
	    error = FALSE;
	    // mark
	    pp = getmark_buf_fnum(curbuf, name[1], FALSE, fnum);
	    ଯଦି (pp == ଖାଲି || pp == (pos_T *)-1 || pp->lnum <= 0)
		ଫେରନ୍ତୁ ଖାଲି;
	    pos = *pp;
	}
    }
    ଯଦି (pos.lnum != 0)
    {
	ଯଦି (charcol)
	    pos.col = buf_byteidx_to_charidx(curbuf, pos.lnum, pos.col);
	ଫେରନ୍ତୁ &pos;
    }

    pos.coladd = 0;

    ଯଦି (name[0] == 'w' && dollar_lnum)
    {
	// the "w_valid" flags are not reset when moving the cursor, but they
	// do matter for update_topline() and validate_botline().
	check_cursor_moved(curwin);

	pos.col = 0;
	ଯଦି (name[1] == '0')		// "w0": first visible line
	{
#ifdef FEAT_TERMINAL
	    ଯଦି (bt_terminal(curwin->w_buffer)
				    && curwin->w_buffer->b_term != ଖାଲି
				    && !term_in_normal_mode(curwin->w_buffer))
		may_move_terminal_to_buffer(curwin->w_buffer->b_term, TRUE);
#endif
	    update_topline();
	    // In silent Ex mode topline is zero, but that's not a valid line
	    // number; use one instead.
	    pos.lnum = curwin->w_topline > 0 ? curwin->w_topline : 1;
	    ଫେରନ୍ତୁ &pos;
	}
	ଅନ୍ୟଥା ଯଦି (name[1] == '$')	// "w$": last visible line
	{
#ifdef FEAT_TERMINAL
	    ଯଦି (bt_terminal(curwin->w_buffer)
				    && curwin->w_buffer->b_term != ଖାଲି
				    && !term_in_normal_mode(curwin->w_buffer))
		may_move_terminal_to_buffer(curwin->w_buffer->b_term, TRUE);
#endif
	    validate_botline();
	    // In silent Ex mode botline is zero, return zero then.
	    pos.lnum = curwin->w_botline > 0 ? curwin->w_botline - 1 : 0;
	    ଫେରନ୍ତୁ &pos;
	}
    }
    ଅନ୍ୟଥା ଯଦି (name[0] == '$')		// last column or line
    {
	ଯଦି (dollar_lnum)
	{
	    pos.lnum = curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
	    pos.col = 0;
	}
	ଅନ୍ୟଥା
	{
	    pos.lnum = curwin->w_cursor.lnum;
	    ଯଦି (charcol)
		pos.col = (colnr_T)mb_charlen(ml_get_curline());
	    ଅନ୍ୟଥା
		pos.col = ml_get_curline_len();
	}
	ଫେରନ୍ତୁ &pos;
    }
    ଯଦି (in_vim9script() && error)
	semsg(_(e_invalid_ମୂଲ୍ୟ_for_ଧାଡ଼ି_ସଂଖ୍ୟା_str), name);
    ଫେରନ୍ତୁ ଖାଲି;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
list2fpos(
    typval_T	*arg,
    pos_T	*posp,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*fnump,
    colnr_T	*curswantp,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		charcol)
{
    list_T	*l = arg->vval.v_list;
    long	i = 0;
    long	n;

    // List must be: [fnum, lnum, col, coladd, curswant], where "fnum" is only
    // there when "fnump" isn't NULL; "coladd" and "curswant" are optional.
    ଯଦି (arg->v_type != VAR_LIST
	    || l == ଖାଲି
	    || l->lv_len < (fnump == ଖାଲି ? 2 : 3)
	    || l->lv_len > (fnump == ଖାଲି ? 4 : 5))
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (fnump != ଖାଲି)
    {
	n = list_find_nr(l, i++, ଖାଲି);	// fnum
	ଯଦି (n < 0)
	    ଫେରନ୍ତୁ FAIL;
	ଯଦି (n == 0)
	    n = curbuf->b_fnum;		// current buffer
	*fnump = n;
    }

    n = list_find_nr(l, i++, ଖାଲି);	// lnum
    ଯଦି (n < 0)
	ଫେରନ୍ତୁ FAIL;
    posp->lnum = n;

    n = list_find_nr(l, i++, ଖାଲି);	// col
    ଯଦି (n < 0)
	ଫେରନ୍ତୁ FAIL;
    // If character position is specified, then convert to byte position
    // If the line number is zero use the cursor line.
    ଯଦି (charcol)
    {
	buf_T	*buf;

	// Get the text for the specified line in a loaded buffer
	buf = buflist_findnr(fnump == ଖାଲି ? curbuf->b_fnum : *fnump);
	ଯଦି (buf == ଖାଲି || buf->b_ml.ml_mfp == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;

	n = buf_charidx_to_byteidx(buf,
		  posp->lnum == 0 ? curwin->w_cursor.lnum : posp->lnum, n) + 1;
    }
    posp->col = n;

    n = list_find_nr(l, i, ଖାଲି);	// off
    ଯଦି (n < 0)
	posp->coladd = 0;
    ଅନ୍ୟଥା
	posp->coladd = n;

    ଯଦି (curswantp != ଖାଲି)
	*curswantp = list_find_nr(l, i + 1, ଖାଲି);  // curswant

    ଫେରନ୍ତୁ OK;
}

/*
 * Get the length of an environment variable name.
 * Advance "arg" to the first character after the name.
 * Return 0 for error.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_env_len(char_u **arg)
{
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;

    ପାଇଁ (p = *arg; vim_isIDc(*p); ++p)
	;
    ଯଦି (p == *arg)	    // no name found
	ଫେରନ୍ତୁ 0;

    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p - *arg);
    *arg = p;
    ଫେରନ୍ତୁ len;
}

/*
 * Get the length of the name of a function or internal variable.
 * "arg" is advanced to after the name.
 * Return 0 if something is wrong.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_id_len(char_u **arg)
{
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;

    // Find the end of the name.
    ପାଇଁ (p = *arg; eval_isnamec(*p); ++p)
    {
	ଯଦି (*p == ':')
	{
	    // "s:" is start of "s:var", but "n:" is not and can be used in
	    // slice "[n:]".  Also "xx:" is not a namespace.
	    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p - *arg);
	    ଯଦି ((len == 1 && vim_strchr(NAMESPACE_CHAR, **arg) == ଖାଲି)
		    || len > 1)
		ଭାଙ୍ଗ;
	}
    }
    ଯଦି (p == *arg)	    // no name found
	ଫେରନ୍ତୁ 0;

    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p - *arg);
    *arg = p;

    ଫେରନ୍ତୁ len;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_name_len(
    char_u	**arg,
    char_u	**alias,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		evaluate,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		verbose)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    char_u	*p;
    char_u	*ପଦସମଷ୍ଟି_start;
    char_u	*ପଦସମଷ୍ଟି_end;

    *alias = ଖାଲି;  // default to no alias

    ଯଦି ((*arg)[0] == K_SPECIAL && (*arg)[1] == KS_EXTRA
						  && (*arg)[2] == (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)KE_SNR)
    {
	// hard coded <SNR>, already translated
	*arg += 3;
	ଫେରନ୍ତୁ get_id_len(arg) + 3;
    }
    len = eval_fname_script(*arg);
    ଯଦି (len > 0)
    {
	// literal "<SID>", "s:" or "<SNR>"
	*arg += len;
    }

    /*
     * Find the end of the name; check for {} construction.
     */
    p = find_name_end(*arg, &ପଦସମଷ୍ଟି_start, &ପଦସମଷ୍ଟି_end,
					       len > 0 ? 0 : FNE_CHECK_START);
    ଯଦି (ପଦସମଷ୍ଟି_start != ଖାଲି)
    {
	char_u	*temp_string;

	ଯଦି (!evaluate)
	{
	    len += (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p - *arg);
	    *arg = skipwhite(p);
	    ଫେରନ୍ତୁ len;
	}

	/*
	 * Include any <SID> etc in the expanded string:
	 * Thus the -len here.
	 */
	temp_string = make_expanded_name(*arg - len, ପଦସମଷ୍ଟି_start, ପଦସମଷ୍ଟି_end, p);
	ଯଦି (temp_string == ଖାଲି)
	    ଫେରନ୍ତୁ -1;
	*alias = temp_string;
	*arg = skipwhite(p);
	ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(temp_string);
    }

    len += get_id_len(arg);
    // Only give an error when there is something, otherwise it will be
    // reported at a higher level.
    ଯଦି (len == 0 && verbose && **arg != NUL)
	semsg(_(e_invalid_expression_str), *arg);

    ଫେରନ୍ତୁ len;
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
    char_u	**ପଦସମଷ୍ଟି_start,
    char_u	**ପଦସମଷ୍ଟି_end,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mb_nest = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		br_nest = 0;
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		allow_curly = !in_vim9script();

    ଯଦି (ପଦସମଷ୍ଟି_start != ଖାଲି)
    {
	*ପଦସମଷ୍ଟି_start = ଖାଲି;
	*ପଦସମଷ୍ଟି_end = ଖାଲି;
    }

    // Quick check for valid starting character.
    ଯଦି ((flags & FNE_CHECK_START) && !eval_isnamec1(*arg)
					      && (*arg != '{' || !allow_curly))
	ଫେରନ୍ତୁ arg;

    ପାଇଁ (p = arg; *p != NUL
		    && (eval_isnamec(*p)
			|| (*p == '{' && allow_curly)
			|| ((flags & FNE_INCL_BR) && (*p == '['
					 || (*p == '.' && eval_isdictc(p[1]))))
			|| mb_nest != 0
			|| br_nest != 0); MB_PTR_ADV(p))
    {
	ଯଦି (*p == '\'')
	{
	    // skip over 'string' to avoid counting [ and ] inside it.
	    ପାଇଁ (p = p + 1; *p != NUL && *p != '\''; MB_PTR_ADV(p))
		;
	    ଯଦି (*p == NUL)
		ଭାଙ୍ଗ;
	}
	ଅନ୍ୟଥା ଯଦି (*p == '"')
	{
	    // skip over "str\"ing" to avoid counting [ and ] inside it.
	    ପାଇଁ (p = p + 1; *p != NUL && *p != '"'; MB_PTR_ADV(p))
		ଯଦି (*p == '\\' && p[1] != NUL)
		    ++p;
	    ଯଦି (*p == NUL)
		ଭାଙ୍ଗ;
	}
	ଅନ୍ୟଥା ଯଦି (br_nest == 0 && mb_nest == 0 && *p == ':')
	{
	    // "s:" is start of "s:var", but "n:" is not and can be used in
	    // slice "[n:]".  Also "xx:" is not a namespace. But {ns}: is.
	    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p - arg);
	    ଯଦି ((len == 1 && vim_strchr(NAMESPACE_CHAR, *arg) == ଖାଲି)
		    || (len > 1 && p[-1] != '}'))
		ଭାଙ୍ଗ;
	}

	ଯଦି (mb_nest == 0)
	{
	    ଯଦି (*p == '[')
		++br_nest;
	    ଅନ୍ୟଥା ଯଦି (*p == ']')
		--br_nest;
	}

	ଯଦି (br_nest == 0 && allow_curly)
	{
	    ଯଦି (*p == '{')
	    {
		mb_nest++;
		ଯଦି (ପଦସମଷ୍ଟି_start != ଖାଲି && *ପଦସମଷ୍ଟି_start == ଖାଲି)
		    *ପଦସମଷ୍ଟି_start = p;
	    }
	    ଅନ୍ୟଥା ଯଦି (*p == '}')
	    {
		mb_nest--;
		ଯଦି (ପଦସମଷ୍ଟି_start != ଖାଲି && mb_nest == 0 && *ପଦସମଷ୍ଟି_end == ଖାଲି)
		    *ପଦସମଷ୍ଟି_end = p;
	    }
	}
    }

    ଫେରନ୍ତୁ p;
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
    ସ୍ଥିର char_u *
make_expanded_name(
    char_u	*in_start,
    char_u	*ପଦସମଷ୍ଟି_start,
    char_u	*ପଦସମଷ୍ଟି_end,
    char_u	*in_end)
{
    char_u	c1;
    char_u	*retval = ଖାଲି;
    char_u	*temp_result;

    ଯଦି (ପଦସମଷ୍ଟି_end == ଖାଲି || in_end == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    *ପଦସମଷ୍ଟି_start	= NUL;
    *ପଦସମଷ୍ଟି_end = NUL;
    c1 = *in_end;
    *in_end = NUL;

    temp_result = eval_to_string(ପଦସମଷ୍ଟି_start + 1, FALSE, FALSE);
    ଯଦି (temp_result != ଖାଲି)
    {
	size_t	retvalsize = (size_t)(ପଦସମଷ୍ଟି_start - in_start)
				+ STRLEN(temp_result)
				+ (size_t)(in_end - ପଦସମଷ୍ଟି_end) + 1;

	retval = alloc(retvalsize);
	ଯଦି (retval != ଖାଲି)
	    vim_snprintf((ଅକ୍ଷର *)retval, retvalsize, "%s%s%s",
				in_start, temp_result, ପଦସମଷ୍ଟି_end + 1);
    }
    vim_free(temp_result);

    *in_end = c1;		// put char back for error messages
    *ପଦସମଷ୍ଟି_start = '{';
    *ପଦସମଷ୍ଟି_end = '}';

    ଯଦି (retval != ଖାଲି)
    {
	temp_result = find_name_end(retval, &ପଦସମଷ୍ଟି_start, &ପଦସମଷ୍ଟି_end, 0);
	ଯଦି (ପଦସମଷ୍ଟି_start != ଖାଲି)
	{
	    // Further expansion!
	    temp_result = make_expanded_name(retval, ପଦସମଷ୍ଟି_start,
						       ପଦସମଷ୍ଟି_end, temp_result);
	    vim_free(retval);
	    retval = temp_result;
	}
    }

    ଫେରନ୍ତୁ retval;
}

/*
 * Return TRUE if character "c" can be used in a variable or function name.
 * Does not include '{' or '}' for magic braces.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
eval_isnamec(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ଫେରନ୍ତୁ ASCII_ISALNUM(c) || c == '_' || c == ':' || c == AUTOLOAD_CHAR;
}

/*
 * Return TRUE if character "c" can be used as the first character in a
 * variable or function name (excluding '{' and '}').
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
eval_isnamec1(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ଫେରନ୍ତୁ ASCII_ISALPHA(c) || c == '_';
}

/*
 * Return TRUE if character "c" can be used as the first character of a
 * dictionary key.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
eval_isdictc(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
    ଫେରନ୍ତୁ ASCII_ISALNUM(c) || c == '_';
}

/*
 * Return true when "p" points to "->" followed by a method name or a lambda.
 * Only called for Vim9 script.
 */
    ସ୍ଥିର bool
method_call_follows(char_u *p)
{
    ଯଦି (p[0] != '-' || p[1] != '>')
	ଫେରନ୍ତୁ false;

    char_u	*after = skipwhite(p + 2);

    ଫେରନ୍ତୁ ASCII_ISALPHA(*after) || *after == '(';
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
handle_subscript(
    char_u	**arg,
    char_u	*name_start,
    typval_T	*rettv,
    evalarg_T	*evalarg,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		verbose)	// give error messages
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		evaluate = evalarg != ଖାଲି
				      && (evalarg->eval_flags & EVAL_EVALUATE);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret = OK;
    dict_T	*selfdict = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		check_white = TRUE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		getnext;
    char_u	*p;

    ଯେତେବେଳେ (ret == OK)
    {
	// When at the end of the line and ".name" or a method call follows in
	// the next line then consume the line break.
	p = eval_next_non_blank(*arg, evalarg, &getnext);
	ଯଦି (getnext
	    && ((*p == '.'
		    && ((rettv->v_type == VAR_DICT && eval_isdictc(p[1]))
			|| rettv->v_type == VAR_CLASS
			|| rettv->v_type == VAR_OBJECT))
		|| method_call_follows(p)))
	{
	    *arg = eval_next_ଧାଡ଼ି(*arg, evalarg);
	    p = *arg;
	    check_white = FALSE;
	}

	ଯଦି (rettv->v_type == VAR_ANY)
	{
	    char_u	*exp_name;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cc;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx;
	    ufunc_T	*ufunc;
	    type_T	*type;

	    // Found script from "import {name} as name", script item name must
	    // follow.  "rettv->vval.v_number" has the script ID.
	    ଯଦି (**arg != '.')
	    {
		ଯଦି (verbose)
		    semsg(_(e_expected_dot_after_name_str),
					name_start != ଖାଲି ? name_start: *arg);
		ret = FAIL;
		ଭାଙ୍ଗ;
	    }
	    ++*arg;
	    ଯଦି (IS_WHITE_OR_NUL(**arg))
	    {
		ଯଦି (verbose)
		    emsg(_(e_no_white_space_allowed_after_dot));
		ret = FAIL;
		ଭାଙ୍ଗ;
	    }

	    // isolate the name
	    exp_name = *arg;
	    ଯେତେବେଳେ (eval_isnamec(**arg))
		++*arg;
	    cc = **arg;
	    **arg = NUL;

	    idx = find_exported(rettv->vval.v_ସଂଖ୍ୟା, exp_name, &ufunc, &type,
		       evalarg == ଖାଲି ? ଖାଲି : evalarg->eval_cctx,
		       evalarg == ଖାଲି ? ଖାଲି : evalarg->eval_cstack, verbose);
	    **arg = cc;

	    ଯଦି (idx < 0 && ufunc == ଖାଲି)
	    {
		ret = FAIL;
		ଭାଙ୍ଗ;
	    }
	    ଯଦି (idx >= 0)
	    {
		scriptitem_T    *si = SCRIPT_ITEM(rettv->vval.v_ସଂଖ୍ୟା);
		svar_T		*sv = ((svar_T *)si->sn_var_vals.ga_data) + idx;

		copy_tv(sv->sv_tv, rettv);
	    }
	    ଅନ୍ୟଥା
	    {
		rettv->v_type = VAR_FUNC;
		ଯଦି (**arg == '<')
		{
		    char_u *s = get_generic_func_name(ufunc, arg);
		    ଯଦି (s != ଖାଲି)
			rettv->vval.v_string = s;
		    ଅନ୍ୟଥା
			ret = FAIL;
		}
		ଅନ୍ୟଥା
		{
		    rettv->vval.v_string =
			vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
		}
	    }
	    ଜାରି;
	}

	ଯଦି ((**arg == '(' && (!evaluate || rettv->v_type == VAR_FUNC
			    || rettv->v_type == VAR_PARTIAL))
		    && (!check_white || !VIM_ISWHITE(*(*arg - 1))))
	{
	    ret = call_func_rettv(arg, evalarg, rettv, evaluate,
							       selfdict, ଖାଲି);

	    // Stop the expression evaluation when immediately aborting on
	    // error, or when an interrupt occurred or an exception was thrown
	    // but not caught.
	    ଯଦି (aborting())
	    {
		ଯଦି (ret == OK)
		    clear_tv(rettv);
		ret = FAIL;
	    }
	    dict_unref(selfdict);
	    selfdict = ଖାଲି;
	}
	ଅନ୍ୟଥା ଯଦି (p[0] == '-' && p[1] == '>')
	{
	    ଯଦି (in_vim9script())
		*arg = skipwhite(p + 2);
	    ଅନ୍ୟଥା
		*arg = p + 2;
	    ଯଦି (ret == OK && evaluate && rettv->v_type == VAR_VOID)
	    {
		ଯଦି (verbose)
		    emsg(_(e_cannot_use_void_ମୂଲ୍ୟ));
		ret = FAIL;
	    }
	    ଅନ୍ୟଥା ଯଦି (VIM_ISWHITE(**arg))
	    {
		emsg(_(e_no_white_space_allowed_before_parenthesis));
		ret = FAIL;
	    }
	    ଅନ୍ୟଥା ଯଦି ((**arg == '{' && !in_vim9script()) || **arg == '(')
		// expr->{lambda}() or expr->(lambda)()
		ret = eval_lambda(arg, rettv, evalarg, verbose);
	    ଅନ୍ୟଥା
		// expr->name()
		ret = eval_method(arg, rettv, evalarg, verbose);
	}
	// "." is ".name" lookup when we found a dict or when evaluating and
	// scriptversion is at least 2, where string concatenation is "..".
	ଅନ୍ୟଥା ଯଦି (**arg == '['
		|| (**arg == '.' && (rettv->v_type == VAR_DICT
			|| (!evaluate
			    && (*arg)[1] != '.'
			    && !in_old_script(2)))))
	{
	    dict_unref(selfdict);
	    ଯଦି (rettv->v_type == VAR_DICT)
	    {
		selfdict = rettv->vval.v_dict;
		ଯଦି (selfdict != ଖାଲି)
		    ++selfdict->dv_refcount;
	    }
	    ଅନ୍ୟଥା
		selfdict = ଖାଲି;
	    ଯଦି (eval_index(arg, rettv, evalarg, verbose) == FAIL)
	    {
		clear_tv(rettv);
		ret = FAIL;
	    }
	}
	ଅନ୍ୟଥା ଯଦି (**arg == '.' && (rettv->v_type == VAR_CLASS
					       || rettv->v_type == VAR_OBJECT))
	{
	    // class member: SomeClass.varname
	    // class method: SomeClass.SomeMethod()
	    // class constructor: SomeClass.new()
	    // object member: someObject.varname
	    // object method: someObject.SomeMethod()
	    ଯଦି (class_object_index(arg, rettv, evalarg, verbose) == FAIL)
	    {
		clear_tv(rettv);
		ret = FAIL;
	    }
	}
	ଅନ୍ୟଥା
	    ଭାଙ୍ଗ;
    }

    // Turn "dict.Func" into a partial for "Func" bound to "dict".
    // Don't do this when "Func" is already a partial that was bound
    // explicitly (pt_auto is FALSE).
    ଯଦି (selfdict != ଖାଲି
	    && (rettv->v_type == VAR_FUNC
		|| (rettv->v_type == VAR_PARTIAL
		    && (rettv->vval.v_partial->pt_auto
			|| rettv->vval.v_partial->pt_dict == ଖାଲି))))
	selfdict = make_partial(selfdict, rettv);

    dict_unref(selfdict);
    ଫେରନ୍ତୁ ret;
}

/*
 * Make a copy of an item.
 * Lists and Dictionaries are also copied.  A deep copy if "deep" is set.
 * "top" is TRUE for the toplevel of copy().
 * For deepcopy() "copyID" is zero for a full copy or the ID for when a
 * reference to an already copied list/dict can be used.
 * Returns FAIL or OK.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
item_copy(
    typval_T	*from,
    typval_T	*to,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		deep,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଶୀର୍ଷ,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		copyID)
{
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	recurse = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret = OK;

    ଯଦି (recurse >= DICT_MAXNEST)
    {
	emsg(_(e_variable_nested_too_deep_for_making_copy));
	ଫେରନ୍ତୁ FAIL;
    }
    ++recurse;

    ବାଛ (from->v_type)
    {
	ସ୍ଥିତି VAR_ସଂଖ୍ୟା:
	ସ୍ଥିତି VAR_FLOAT:
	ସ୍ଥିତି VAR_STRING:
	ସ୍ଥିତି VAR_FUNC:
	ସ୍ଥିତି VAR_PARTIAL:
	ସ୍ଥିତି VAR_BOOL:
	ସ୍ଥିତି VAR_SPECIAL:
	ସ୍ଥିତି VAR_JOB:
	ସ୍ଥିତି VAR_CHANNEL:
	ସ୍ଥିତି VAR_INSTR:
	ସ୍ଥିତି VAR_CLASS:
	ସ୍ଥିତି VAR_OBJECT:
	ସ୍ଥିତି VAR_TYPEALIAS:
	    copy_tv(from, to);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_LIST:
	    to->v_type = VAR_LIST;
	    to->v_lock = 0;
	    ଯଦି (from->vval.v_list == ଖାଲି)
		to->vval.v_list = ଖାଲି;
	    ଅନ୍ୟଥା ଯଦି (copyID != 0 && from->vval.v_list->lv_copyID == copyID)
	    {
		// use the copy made earlier
		to->vval.v_list = from->vval.v_list->lv_copylist;
		++to->vval.v_list->lv_refcount;
	    }
	    ଅନ୍ୟଥା
	    {
		to->vval.v_list = list_copy(from->vval.v_list,
							    deep, ଶୀର୍ଷ, copyID);
		ଯଦି (to->vval.v_list == ଖାଲି)
		    ret = FAIL;
	    }
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_TUPLE:
	    to->v_type = VAR_TUPLE;
	    to->v_lock = 0;
	    ଯଦି (from->vval.v_tuple == ଖାଲି)
		to->vval.v_tuple = ଖାଲି;
	    ଅନ୍ୟଥା ଯଦି (copyID != 0 && from->vval.v_tuple->tv_copyID == copyID)
	    {
		// use the copy made earlier
		to->vval.v_tuple = from->vval.v_tuple->tv_copytuple;
		++to->vval.v_tuple->tv_refcount;
	    }
	    ଅନ୍ୟଥା
	    {
		to->vval.v_tuple = tuple_copy(from->vval.v_tuple,
							    deep, ଶୀର୍ଷ, copyID);
		ଯଦି (to->vval.v_tuple == ଖାଲି)
		    ret = FAIL;
	    }
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_BLOB:
	    to->v_type = VAR_BLOB;
	    to->v_lock = 0;
	    ଯଦି (from->vval.v_blob == ଖାଲି)
		to->vval.v_blob = ଖାଲି;
	    ଅନ୍ୟଥା
	    {
		to->vval.v_blob = blob_copy(from->vval.v_blob);
		ଯଦି (to->vval.v_blob == ଖାଲି)
		    ret = FAIL;
	    }
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_DICT:
	    to->v_type = VAR_DICT;
	    to->v_lock = 0;
	    ଯଦି (from->vval.v_dict == ଖାଲି)
		to->vval.v_dict = ଖାଲି;
	    ଅନ୍ୟଥା ଯଦି (copyID != 0 && from->vval.v_dict->dv_copyID == copyID)
	    {
		// use the copy made earlier
		to->vval.v_dict = from->vval.v_dict->dv_copydict;
		++to->vval.v_dict->dv_refcount;
	    }
	    ଅନ୍ୟଥା
	    {
		to->vval.v_dict = dict_copy(from->vval.v_dict,
							    deep, ଶୀର୍ଷ, copyID);
		ଯଦି (to->vval.v_dict == ଖାଲି)
		    ret = FAIL;
	    }
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_UNKNOWN:
	ସ୍ଥିତି VAR_ANY:
	ସ୍ଥିତି VAR_VOID:
	    internal_error_no_abort("item_copy(UNKNOWN)");
	    ret = FAIL;
    }
    --recurse;
    ଫେରନ୍ତୁ ret;
}

    ଶୂନ୍ୟ
echo_one(typval_T *rettv, ପୂର୍ଣ୍ଣସଂଖ୍ୟା with_space, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *atstart, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *needclr)
{
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    char_u	*p = echo_string(rettv, &tofree, numbuf, get_copyID());

    ଯଦି (*atstart)
    {
	*atstart = FALSE;
	// Call msg_start() after eval1(), evaluating the expression
	// may cause a message to appear.
	ଯଦି (with_space)
	{
	    // Mark the saved text as finishing the line, so that what
	    // follows is displayed on a new line when scrolling back
	    // at the more prompt.
	    ସନ୍ଦେଶ_sb_eol();
	    ସନ୍ଦେଶ_start();
	}
    }
    ଅନ୍ୟଥା ଯଦି (with_space)
	ସନ୍ଦେଶ_puts_attr(" ", echo_attr);

    ଯଦି (p != ଖାଲି)
	ପାଇଁ ( ; *p != NUL && !got_int; ++p)
	{
	    ଯଦି (*p == '\n' || *p == '\r' || *p == TAB)
	    {
		ଯଦି (*p != TAB && *needclr)
		{
		    // remove any text still there from the command
		    ସନ୍ଦେଶ_clr_eos();
		    *needclr = FALSE;
		}
		ସନ୍ଦେଶ_putchar_attr(*p, echo_attr);
	    }
	    ଅନ୍ୟଥା
	    {
		ଯଦି (has_mbyte)
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = (*mb_ptr2len)(p);

		    (ଶୂନ୍ୟ)ସନ୍ଦେଶ_outtrans_len_attr(p, i, echo_attr);
		    p += i - 1;
		}
		ଅନ୍ୟଥା
		    (ଶୂନ୍ୟ)ସନ୍ଦେଶ_outtrans_len_attr(p, 1, echo_attr);
	    }
	}
    vim_free(tofree);
}

/*
 * ":echo expr1 ..."	print each argument separated with a space, add a
 *			newline at the end.
 * ":echon expr1 ..."	print each argument plain.
 */
    ଶୂନ୍ୟ
ex_echo(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    typval_T	rettv;
    char_u	*arg_start;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		needclr = TRUE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		atstart = TRUE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_emsg_before = did_emsg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		called_emsg_before = called_emsg;
    evalarg_T	evalarg;

    fill_evalarg_from_eap(&evalarg, eap, eap->skip);

    ଯଦି (eap->skip)
	++emsg_skip;
    ଯେତେବେଳେ ((!ends_excmd2(eap->cmd, arg) || *arg == '"') && !got_int)
    {
	// If eval1() causes an error message the text from the command may
	// still need to be cleared. E.g., "echo 22,44".
	need_clr_eos = needclr;

	arg_start = arg;
	ଯଦି (eval1(&arg, &rettv, &evalarg) == FAIL)
	{
	    /*
	     * Report the invalid expression unless the expression evaluation
	     * has been cancelled due to an aborting error, an interrupt, or an
	     * exception.
	     */
	    ଯଦି (!aborting() && did_emsg == did_emsg_before
					  && called_emsg == called_emsg_before)
		semsg(_(e_invalid_expression_str), arg_start);
	    need_clr_eos = FALSE;
	    ଭାଙ୍ଗ;
	}
	need_clr_eos = FALSE;

	ଯଦି (!eap->skip)
	{
	    ଯଦି (rettv.v_type == VAR_VOID)
	    {
		semsg(_(e_expression_does_not_result_in_ମୂଲ୍ୟ_str), arg_start);
		ଭାଙ୍ଗ;
	    }
	    echo_one(&rettv, eap->cmdidx == CMD_echo, &atstart, &needclr);
	}

	clear_tv(&rettv);
	arg = skipwhite(arg);
    }
    set_nextcmd(eap, arg);
    clear_evalarg(&evalarg, eap);

    ଯଦି (eap->skip)
	--emsg_skip;
    ଅନ୍ୟଥା
    {
	// remove text that may still be there from the command
	ଯଦି (needclr)
	    ସନ୍ଦେଶ_clr_eos();
	ଯଦି (eap->cmdidx == CMD_echo)
	    ସନ୍ଦେଶ_end();
    }
}

/*
 * ":echohl {name}".
 */
    ଶୂନ୍ୟ
ex_echohl(exarg_T *eap)
{
    echo_attr = syn_name2attr(eap->arg);
}

/*
 * Returns the :echo attribute
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_echo_attr(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ echo_attr;
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
    ଶୂନ୍ୟ
ex_execute(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    typval_T	rettv;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret = OK;
    char_u	*p;
    garray_T	ga;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    long	start_lnum = SOURCING_LNUM;

    ga_init2(&ga, 1, 80);

    ଯଦି (eap->skip)
	++emsg_skip;
    ଯେତେବେଳେ (!ends_excmd2(eap->cmd, arg) || *arg == '"')
    {
	ret = eval1_emsg(&arg, &rettv, eap);
	ଯଦି (ret == FAIL)
	    ଭାଙ୍ଗ;

	ଯଦି (!eap->skip)
	{
	    char_u   buf[NUMBUFLEN];

	    ଯଦି (eap->cmdidx == CMD_execute)
	    {
		ଯଦି (rettv.v_type == VAR_CHANNEL || rettv.v_type == VAR_JOB)
		{
		    semsg(_(e_using_invalid_ମୂଲ୍ୟ_as_string_str),
						  vartype_name(rettv.v_type));
		    p = ଖାଲି;
		}
		ଅନ୍ୟଥା
		    p = tv_get_string_buf(&rettv, buf);
	    }
	    ଅନ୍ୟଥା
		p = tv_stringify(&rettv, buf);
	    ଯଦି (p == ଖାଲି)
	    {
		clear_tv(&rettv);
		ret = FAIL;
		ଭାଙ୍ଗ;
	    }
	    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(p);
	    ଯଦି (ga_grow(&ga, len + 2) == FAIL)
	    {
		clear_tv(&rettv);
		ret = FAIL;
		ଭାଙ୍ଗ;
	    }
	    ଯଦି (ga.ga_len)
		((char_u *)(ga.ga_data))[ga.ga_len++] = ' ';
	    STRCPY((char_u *)(ga.ga_data) + ga.ga_len, p);
	    ga.ga_len += len;
	}

	clear_tv(&rettv);
	arg = skipwhite(arg);
    }

    ଯଦି (ret != FAIL && ga.ga_data != ଖାଲି)
    {
	// use the first line of continuation lines for messages
	SOURCING_LNUM = start_lnum;

	ଯଦି (eap->cmdidx == CMD_echomsg
		|| eap->cmdidx == CMD_echowindow
		|| eap->cmdidx == CMD_echoerr)
	{
	    // Mark the already saved text as finishing the line, so that what
	    // follows is displayed on a new line when scrolling back at the
	    // more prompt.
	    ସନ୍ଦେଶ_sb_eol();
	}

	ଯଦି (eap->cmdidx == CMD_echomsg)
	{
	    ସନ୍ଦେଶ_attr(ga.ga_data, echo_attr);
	    out_flush();
	}
	ଅନ୍ୟଥା ଯଦି (eap->cmdidx == CMD_echowindow)
	{
#ifdef HAS_MESSAGE_WINDOW
	    start_echowindow(eap->addr_ଗଣନା > 0 ? eap->line2 : 0);
#endif
	    ସନ୍ଦେଶ_attr(ga.ga_data, echo_attr);
#ifdef HAS_MESSAGE_WINDOW
	    end_echowindow();
#endif
	}
	ଅନ୍ୟଥା ଯଦି (eap->cmdidx == CMD_echoconsole)
	{
	    ui_write(ga.ga_data, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(ga.ga_data), TRUE);
	    ui_write((char_u *)"\r\n", 2, TRUE);
	}
	ଅନ୍ୟଥା ଯଦି (eap->cmdidx == CMD_echoerr)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_did_emsg = did_emsg;

	    // We don't want to abort following commands, restore did_emsg.
	    emsg(ga.ga_data);
	    ଯଦି (!force_abort)
		did_emsg = save_did_emsg;
	}
	ଅନ୍ୟଥା ଯଦି (eap->cmdidx == CMD_execute)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_sticky_cmdmod_flags = sticky_cmdmod_flags;

	    // "legacy exe cmd" and "vim9cmd exe cmd" applies to "cmd".
	    sticky_cmdmod_flags = cmdmod.cmod_flags
						& (CMOD_LEGACY | CMOD_VIM9CMD);
	    do_cmdline((char_u *)ga.ga_data,
		       eap->ea_getline, eap->cookie, DOCMD_NOWAIT|DOCMD_VERBOSE);
	    sticky_cmdmod_flags = save_sticky_cmdmod_flags;
	}
    }

    ga_clear(&ga);

    ଯଦି (eap->skip)
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
find_option_end(char_u **arg, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *scope)
{
    char_u	*p = *arg;

    ++p;
    ଯଦି (*p == 'g' && p[1] == ':')
    {
	*scope = OPT_GLOBAL;
	p += 2;
    }
    ଅନ୍ୟଥା ଯଦି (*p == 'l' && p[1] == ':')
    {
	*scope = OPT_LOCAL;
	p += 2;
    }
    ଅନ୍ୟଥା
	*scope = 0;

    ଯଦି (!ASCII_ISALPHA(*p))
	ଫେରନ୍ତୁ ଖାଲି;
    *arg = p;

    ଯଦି (p[0] == 't' && p[1] == '_' && p[2] != NUL && p[3] != NUL)
	p += 4;	    // termcap option
    ଅନ୍ୟଥା
	ଯେତେବେଳେ (ASCII_ISALPHA(*p))
	    ++p;
    ଫେରନ୍ତୁ p;
}

/*
 * Display script name where an item was last set.
 * Should only be invoked when 'verbose' is non-zero.
 */
    ଶୂନ୍ୟ
last_set_ସନ୍ଦେଶ(sctx_T script_ctx)
{
    char_u *p;

    ଯଦି (script_ctx.sc_sid == 0)
	ଫେରନ୍ତୁ;

    p = home_replace_save(ଖାଲି, get_scriptname(script_ctx.sc_sid));
    ଯଦି (p == ଖାଲି)
	ଫେରନ୍ତୁ;

    verbose_enter();
    ସନ୍ଦେଶ_puts(_("\n\tLast set from "));
    ସନ୍ଦେଶ_puts((ଅକ୍ଷର *)p);
    ଯଦି (script_ctx.sc_lnum > 0)
    {
	ସନ୍ଦେଶ_puts(_(ଧାଡ଼ି_ସନ୍ଦେଶ));
	ସନ୍ଦେଶ_outnum((long)script_ctx.sc_lnum);
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
    typval_T	*ପଦସମଷ୍ଟି,
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
    ଯଦି (regmatch.regprog != ଖାଲି)
    {
	char_u	*tail = str;
	char_u	*end = str + len;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	do_all = (flags[0] == 'g');
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	sublen;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	i;
	char_u	*zero_width = ଖାଲି;

	ଯେତେବେଳେ (vim_regexec_nl(&regmatch, str, (colnr_T)(tail - str)))
	{
	    // Skip empty match except for first match.
	    ଯଦି (regmatch.startp[0] == regmatch.endp[0])
	    {
		ଯଦି (zero_width == regmatch.startp[0])
		{
		    // avoid getting stuck on a match with an empty string
		    i = mb_ptr2len(tail);
		    mch_memmove((char_u *)ga.ga_data + ga.ga_len, tail,
								   (size_t)i);
		    ga.ga_len += i;
		    tail += i;
		    ଜାରି;
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
	    sublen = vim_regsub(&regmatch, sub, ପଦସମଷ୍ଟି, tail, 0, REGSUB_MAGIC);
	    ଯଦି (sublen <= 0)
	    {
		ga_clear(&ga);
		ଭାଙ୍ଗ;
	    }
	    ଯଦି (ga_grow(&ga, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)((end - tail) + sublen -
			    (regmatch.endp[0] - regmatch.startp[0]))) == FAIL)
	    {
		ga_clear(&ga);
		ଭାଙ୍ଗ;
	    }

	    // copy the text up to where the match is
	    i = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(regmatch.startp[0] - tail);
	    mch_memmove((char_u *)ga.ga_data + ga.ga_len, tail, (size_t)i);
	    // add the substituted text
	    (ଶୂନ୍ୟ)vim_regsub(&regmatch, sub, ପଦସମଷ୍ଟି,
				  (char_u *)ga.ga_data + ga.ga_len + i, sublen,
				  REGSUB_COPY | REGSUB_MAGIC);
	    ga.ga_len += i + sublen - 1;
	    tail = regmatch.endp[0];
	    ଯଦି (*tail == NUL)
		ଭାଙ୍ଗ;
	    ଯଦି (!do_all)
		ଭାଙ୍ଗ;
	}

	ଯଦି (ga.ga_data != ଖାଲି)
	{
	    STRCPY((ଅକ୍ଷର *)ga.ga_data + ga.ga_len, tail);
	    ga.ga_len += (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(end - tail);
	}

	vim_regfree(regmatch.regprog);
    }

    ଯଦି (ga.ga_data != ଖାଲି)
    {
	str = (char_u *)ga.ga_data;
	len = (size_t)ga.ga_len;
    }
    ret = vim_strnsave(str, len);
    ga_clear(&ga);
    ଯଦି (p_cpo == empty_option)
	p_cpo = save_cpo;
    ଅନ୍ୟଥା
    {
	// Darn, evaluating {sub} expression or {expr} changed the value.
	// If it's still empty it was changed and restored, need to restore in
	// the complicated way.
	ଯଦି (*p_cpo == NUL)
	    set_option_ମୂଲ୍ୟ_give_err((char_u *)"cpo", 0L, save_cpo, 0);
	free_string_option(save_cpo);
    }

    ଯଦି (ret_len != ଖାଲି)
	*ret_len = len;

    ଫେରନ୍ତୁ ret;
}
