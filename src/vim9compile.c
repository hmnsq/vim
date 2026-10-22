/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * vim9compile.c: compiling a :def function
 */

#నిర్వచించు USING_FLOAT_STUFF
#అన్తర్భావ "vim.h"

#ఒకవేళ defined(FEAT_EVAL)

// Functions defined with :def are stored in this growarray.
// They are never removed, so that they can be found by index.
// Deleted functions have the df_deleted flag set.
garray_T def_functions = {0, 0, పరిమాణం(dfunc_T), 50, ఖాళీ};

స్థిర శూన్య delete_def_function_contents(dfunc_T *dfunc, పూర్ణాంకం mark_deleted);

/*
 * Lookup variable "name" in the local scope and return it in "lvar".
 * "lvar->lv_from_outer" is incremented accordingly.
 * If "lvar" is NULL only check if the variable can be found.
 * Return FAIL if not found.
 */
    పూర్ణాంకం
lookup_local(char_u *name, size_t len, lvar_T *lvar, cctx_T *cctx)
{
    పూర్ణాంకం	    idx;
    lvar_T  *lvp;

    ఒకవేళ (len == 0)
	తిరిగి FAIL;

    ఒకవేళ (((len == 4 && STRNCMP(name, "this", 4) == 0)
		|| (len == 5 && STRNCMP(name, "super", 5) == 0))
	    && cctx->ctx_ufunc != ఖాళీ
	    && (cctx->ctx_ufunc->uf_flags & (FC_OBJECT|FC_NEW)))
    {
	పూర్ణాంకం is_super = *name == 's';
	ఒకవేళ (is_super)
	{
	    ఒకవేళ (name[5] != '.')
	    {
		emsg(_(e_super_must_be_followed_by_dot));
		తిరిగి FAIL;
	    }
	    ఒకవేళ (cctx->ctx_ufunc->uf_class != ఖాళీ
		    && cctx->ctx_ufunc->uf_class->class_extends == ఖాళీ)
	    {
		emsg(_(e_using_super_not_లోపల_బిడ్డ_class));
		తిరిగి FAIL;
	    }
	}
	ఒకవేళ (lvar != ఖాళీ)
	{
	    CLEAR_సూచిక(lvar);
	    lvar->lv_loop_లోతు = -1;
	    lvar->lv_name = (char_u *)(is_super ? "super" : "this");
	    ఒకవేళ (cctx->ctx_ufunc->uf_class != ఖాళీ)
	    {
		lvar->lv_type = &cctx->ctx_ufunc->uf_class->class_object_type;
		ఒకవేళ (is_super)
		{
		    type_T *type = get_type_ptr(cctx->ctx_type_list);

		    ఒకవేళ (type != ఖాళీ)
		    {
			*type = *lvar->lv_type;
			lvar->lv_type = type;
			type->tt_flags |= TTFLAG_SUPER;
		    }
		}
	    }
	}
	తిరిగి OK;
    }

    // Find local in current function scope.
    కొరకు (idx = 0; idx < cctx->ctx_locals.ga_len; ++idx)
    {
	lvp = ((lvar_T *)cctx->ctx_locals.ga_దత్తాంశం) + idx;
	ఒకవేళ (lvp->lv_name != ఖాళీ
		&& STRNCMP(name, lvp->lv_name, len) == 0
					       && STRLEN(lvp->lv_name) == len)
	{
	    ఒకవేళ (lvar != ఖాళీ)
	    {
		*lvar = *lvp;
		lvar->lv_from_outer = 0;
		// If the variable was declared inside a loop set
		// lvar->lv_loop_idx and lvar->lv_loop_depth.
		get_loop_var_idx(cctx, idx, lvar);
	    }
	    తిరిగి OK;
	}
    }

    // Find local in outer function scope.
    ఒకవేళ (cctx->ctx_outer != ఖాళీ)
    {
	ఒకవేళ (lookup_local(name, len, lvar, cctx->ctx_outer) == OK)
	{
	    ఒకవేళ (lvar != ఖాళీ)
	    {
		cctx->ctx_outer_వాడిన = TRUE;
		++lvar->lv_from_outer;
	    }
	    తిరిగి OK;
	}
    }

    తిరిగి FAIL;
}

/*
 * Lookup an argument in the current function and an enclosing function.
 * Returns the argument index in "idxp"
 * Returns the argument type in "type"
 * Sets "gen_load_outer" to TRUE if found in outer scope.
 * Returns OK when found, FAIL otherwise.
 */
    పూర్ణాంకం
arg_exists(
	char_u	*name,
	size_t	len,
	పూర్ణాంకం	*idxp,
	type_T	**type,
	పూర్ణాంకం	*gen_ఎక్కించు_outer,
	cctx_T	*cctx)
{
    పూర్ణాంకం	    idx;
    char_u  *va_name;

    ఒకవేళ (len == 0)
	తిరిగి FAIL;
    కొరకు (idx = 0; idx < cctx->ctx_ufunc->uf_వాదనలు_visible; ++idx)
    {
	char_u *arg = FUNCARG(cctx->ctx_ufunc, idx);

	ఒకవేళ (STRNCMP(name, arg, len) == 0 && arg[len] == NUL)
	{
	    ఒకవేళ (idxp != ఖాళీ)
	    {
		// Arguments are located above the frame pointer.  One further
		// if there is a vararg argument
		*idxp = idx - (cctx->ctx_ufunc->uf_వాదనలు.ga_len
							    + దొంతర_చట్రం_కొలత)
			      + (cctx->ctx_ufunc->uf_va_name != ఖాళీ ? -1 : 0);

		ఒకవేళ (cctx->ctx_ufunc->uf_arg_types != ఖాళీ)
		    *type = cctx->ctx_ufunc->uf_arg_types[idx];
		లేకపోతే
		    *type = &t_any;
	    }
	    తిరిగి OK;
	}
    }

    va_name = cctx->ctx_ufunc->uf_va_name;
    ఒకవేళ (va_name != ఖాళీ
		    && STRNCMP(name, va_name, len) == 0 && va_name[len] == NUL)
    {
	ఒకవేళ (idxp != ఖాళీ)
	{
	    // varargs is always the last argument
	    *idxp = -దొంతర_చట్రం_కొలత - 1;
	    *type = cctx->ctx_ufunc->uf_va_type;
	}
	తిరిగి OK;
    }

    ఒకవేళ (cctx->ctx_outer != ఖాళీ)
    {
	// Lookup the name for an argument of the outer function.
	ఒకవేళ (arg_exists(name, len, idxp, type, gen_ఎక్కించు_outer, cctx->ctx_outer)
									 == OK)
	{
	    ఒకవేళ (gen_ఎక్కించు_outer != ఖాళీ)
	    {
		++*gen_ఎక్కించు_outer;
		cctx->ctx_outer_వాడిన = TRUE;
	    }
	    తిరిగి OK;
	}
    }

    తిరిగి FAIL;
}

/*
 * Lookup a script-local variable in the current script, possibly defined in a
 * block that contains the function "cctx->ctx_ufunc".
 * "cctx" is NULL at the script level, "cstack" is NULL in a function.
 * If "len" is <= 0 "name" must be NUL terminated.
 * Return NULL when not found.
 */
    స్థిర sallvar_T *
కనుగొను_script_var(char_u *name, size_t len, cctx_T *cctx, cstack_T *cstack)
{
    scriptitem_T    *si = SCRIPT_అంశం(current_sctx.sc_sid);
    hashitem_T	    *hi;
    పూర్ణాంకం		    cc;
    sallvar_T	    *sav;
    ufunc_T	    *ufunc;

    // Find the list of all script variables with the right name.
    ఒకవేళ (len > 0)
    {
	cc = name[len];
	name[len] = NUL;
    }
    hi = hash_కనుగొను(&si->sn_all_vars.dv_hashtab, name);
    ఒకవేళ (len > 0)
	name[len] = cc;
    ఒకవేళ (HASHITEM_ఖాళీలు(hi))
	తిరిగి ఖాళీ;

    sav = HI2SAV(hi);
    ఒకవేళ (sav->sav_ఖండం_id == 0)
	// variable defined in the top script scope is always visible
	తిరిగి sav;

    ఒకవేళ (cctx == ఖాళీ)
    {
	ఒకవేళ (cstack == ఖాళీ)
	    తిరిగి ఖాళీ;

	// Not in a function scope, find variable with block ID equal to or
	// smaller than the current block id.  Use "cstack" to go up the block
	// scopes.
	అయితే (sav != ఖాళీ)
	{
	    పూర్ణాంకం idx;

	    కొరకు (idx = cstack->cs_idx; idx >= 0; --idx)
		ఒకవేళ (cstack->cs_ఖండం_id[idx] == sav->sav_ఖండం_id)
		    ఆపు;
	    ఒకవేళ (idx >= 0)
		ఆపు;
	    sav = sav->sav_తదుపరి;
	}
	తిరిగి sav;
    }

    // Go over the variables with this name and find one that was visible
    // from the function.
    ufunc = cctx->ctx_ufunc;
    అయితే (sav != ఖాళీ)
    {
	పూర్ణాంకం idx;

	ఒకవేళ (ufunc->uf_ఖండం_లోతు == 0 && sav->sav_ఖండం_id == 0)
	    // If the function was defined at the script level (not inside a
	    // block), script-scope variables are always visible.
	    తిరిగి sav;

	// Go over the blocks that this function was defined in.  If the
	// variable block ID matches it was visible to the function.
	కొరకు (idx = 0; idx < ufunc->uf_ఖండం_లోతు; ++idx)
	    ఒకవేళ (ufunc->uf_ఖండం_ids[idx] == sav->sav_ఖండం_id)
		తిరిగి sav;
	sav = sav->sav_తదుపరి;
    }

    // Not found, variable was not visible.
    తిరిగి ఖాళీ;
}

/*
 * If "name" can be found in the current script set it's "block_id".
 */
    శూన్య
update_script_var_ఖండం_id(char_u *name, పూర్ణాంకం ఖండం_id)
{
    scriptitem_T    *si = SCRIPT_అంశం(current_sctx.sc_sid);
    hashitem_T	    *hi;
    sallvar_T	    *sav;

    hi = hash_కనుగొను(&si->sn_all_vars.dv_hashtab, name);
    ఒకవేళ (HASHITEM_ఖాళీలు(hi))
	తిరిగి;
    sav = HI2SAV(hi);
    sav->sav_ఖండం_id = ఖండం_id;
}

/*
 * Lookup a variable (without s: prefix) in the current script.
 * "cctx" is NULL at the script level, "cstack" is NULL in a function.
 * Returns OK or FAIL.
 */
    పూర్ణాంకం
script_var_exists(char_u *name, size_t len, cctx_T *cctx, cstack_T *cstack)
{
    ఒకవేళ (current_sctx.sc_sid <= 0)
	తిరిగి FAIL;
    ఒకవేళ (current_script_is_vim9())
    {
	// Check script variables that were visible where the function was
	// defined.
	ఒకవేళ (కనుగొను_script_var(name, len, cctx, cstack) != ఖాళీ)
	    తిరిగి OK;
    }
    లేకపోతే
    {
	hashtab_T	*ht = &SCRIPT_VARS(current_sctx.sc_sid);
	dictitem_T	*di;
	పూర్ణాంకం		cc;

	// Check script variables that are currently visible
	cc = name[len];
	name[len] = NUL;
	di = కనుగొను_var_లోపల_ht(ht, 0, name, TRUE);
	name[len] = cc;
	ఒకవేళ (di != ఖాళీ)
	    తిరిగి OK;
    }

    తిరిగి FAIL;
}

/*
 * Returns the index of a class method or class variable with name "name"
 * accessible in the currently compiled function.
 * If "cl_ret" is not NULL set it to the class.
 * Otherwise return -1.
 */
    స్థిర పూర్ణాంకం
cctx_class_midx(
    cctx_T  *cctx,
    పూర్ణాంకం	    is_పద్ధతి,
    char_u  *name,
    size_t  len,
    class_T **cl_ret)
{
    ఒకవేళ (cctx == ఖాళీ || cctx->ctx_ufunc == ఖాళీ
	    || cctx->ctx_ufunc->uf_class == ఖాళీ
	    || cctx->ctx_ufunc->uf_defclass == ఖాళీ)
	తిరిగి -1;

    // Search for the class method or variable in the class where the calling
    // function is defined.
    class_T *cl = cctx->ctx_ufunc->uf_defclass;
    పూర్ణాంకం m_idx = is_పద్ధతి ? class_పద్ధతి_idx(cl, name, len)
					: class_member_idx(cl, name, len);
    ఒకవేళ (m_idx < 0)
    {
	cl = cl->class_extends;
	అయితే (cl != ఖాళీ)
	{
	    m_idx = is_పద్ధతి ? class_పద్ధతి_idx(cl, name, len)
					: class_member_idx(cl, name, len);
	    ఒకవేళ (m_idx >= 0)
		ఆపు;
	    cl = cl->class_extends;
	}
    }

    ఒకవేళ (m_idx >= 0)
    {
	ఒకవేళ (cl_ret != ఖాళీ)
	    *cl_ret = cl;
    }

    తిరిగి m_idx;
}

/*
 * Returns the index of a class method with name "name" accessible in the
 * currently compiled function.  Returns -1 if not found.  The class where the
 * method is defined is returned in "cl_ret".
 */
    పూర్ణాంకం
cctx_class_పద్ధతి_idx(
    cctx_T  *cctx,
    char_u  *name,
    size_t  len,
    class_T **cl_ret)
{
    తిరిగి cctx_class_midx(cctx, TRUE, name, len, cl_ret);
}

/*
 * Returns the index of a class variable with name "name" accessible in the
 * currently compiled function.  Returns -1 if not found.  The class where the
 * variable is defined is returned in "cl_ret".
 */
    పూర్ణాంకం
cctx_class_member_idx(
    cctx_T  *cctx,
    char_u  *name,
    size_t  len,
    class_T **cl_ret)
{
    తిరిగి cctx_class_midx(cctx, FALSE, name, len, cl_ret);
}

/*
 * Return TRUE if "name" is a local variable, argument, script variable or
 * imported.  Also if "name" is "this" and in a class method.
 */
    స్థిర పూర్ణాంకం
variable_exists(char_u *name, size_t len, cctx_T *cctx)
{
    తిరిగి (cctx != ఖాళీ
		&& (lookup_local(name, len, ఖాళీ, cctx) == OK
		    || arg_exists(name, len, ఖాళీ, ఖాళీ, ఖాళీ, cctx) == OK
		    || (len == 4
			&& cctx->ctx_ufunc != ఖాళీ
			&& (cctx->ctx_ufunc->uf_flags & (FC_OBJECT|FC_NEW))
			&& STRNCMP(name, "this", 4) == 0)))
	    || script_var_exists(name, len, cctx, ఖాళీ) == OK
	    || cctx_class_member_idx(cctx, name, len, ఖాళీ) >= 0
	    || కనుగొను_imported(name, len, FALSE) != ఖాళీ;
}

/*
 * Return TRUE if "name" is a local variable, argument, script variable,
 * imported or function.  Or commands are being skipped, a declaration may have
 * been skipped then.
 */
    స్థిర పూర్ణాంకం
అంశం_exists(char_u *name, size_t len, పూర్ణాంకం cmd వాడని, cctx_T *cctx)
{
    తిరిగి variable_exists(name, len, cctx);
}

/*
 * Check if "p[len]" is already defined, either in script "import_sid" or in
 * compilation context "cctx".
 * "cctx" is NULL at the script level, "cstack" is NULL in a function.
 * Does not check the global namespace.
 * If "is_arg" is TRUE the error message is for an argument name.
 * Return FAIL and give an error if it defined.
 */
    పూర్ణాంకం
check_defined(
	char_u	    *p,
	size_t	    len,
	cctx_T	    *cctx,
	cstack_T    *cstack,
	పూర్ణాంకం	    is_arg)
{
    పూర్ణాంకం		c = p[len];
    ufunc_T	*ufunc = ఖాళీ;

    // underscore argument is OK
    ఒకవేళ (len == 1 && *p == '_')
	తిరిగి OK;

    ఒకవేళ (script_var_exists(p, len, cctx, cstack) == OK)
    {
	ఒకవేళ (is_arg)
	    semsg(_(e_వాదం_already_declared_లోపల_script_str), p);
	లేకపోతే
	    semsg(_(e_variable_already_declared_లోపల_script_str), p);
	తిరిగి FAIL;
    }

    ఒకవేళ (cctx_class_member_idx(cctx, p, len, ఖాళీ) >= 0)
    {
	ఒకవేళ (is_arg)
	    semsg(_(e_వాదం_already_declared_లోపల_class_str), p);
	లేకపోతే
	    semsg(_(e_variable_already_declared_లోపల_class_str), p);
	తిరిగి FAIL;
    }

    p[len] = NUL;
    ఒకవేళ ((cctx != ఖాళీ
		&& (lookup_local(p, len, ఖాళీ, cctx) == OK
		    || arg_exists(p, len, ఖాళీ, ఖాళీ, ఖాళీ, cctx) == OK))
	    || కనుగొను_imported(p, len, FALSE) != ఖాళీ
	    || (ufunc = కనుగొను_func_even_dead(p, 0)) != ఖాళీ)
    {
	// A local or script-local function can shadow a global function.
	ఒకవేళ (ufunc == ఖాళీ || ((ufunc->uf_flags & FC_DEAD) == 0
		    && (!func_is_global(ufunc)
					     || (p[0] == 'g' && p[1] == ':'))))
	{
	    ఒకవేళ (is_arg)
		semsg(_(e_వాదం_name_shadows_existing_variable_str), p);
	    లేకపోతే
		semsg(_(e_name_already_defined_str), p);
	    p[len] = c;
	    తిరిగి FAIL;
	}
    }
    p[len] = c;
    తిరిగి OK;
}


/*
 * Return TRUE if "actual" could be "expected" and a runtime typecheck is to be
 * used.  Return FALSE if the types will never match.
 */
    స్థిర పూర్ణాంకం
use_typecheck(type_T *actual, type_T *expected)
{
    ఒకవేళ (actual->tt_type == VAR_ANY
	    || actual->tt_type == VAR_UNKNOWN
	    || (actual->tt_type == VAR_FUNC
		&& (expected->tt_type == VAR_FUNC
					   || expected->tt_type == VAR_PARTIAL)
		&& (actual->tt_member == &t_any
		    || actual->tt_member == &t_unknown
		    || actual->tt_argcount < 0)
		&& (actual->tt_member == &t_unknown ||
		    (actual->tt_member == &t_void)
					 == (expected->tt_member == &t_void))))
	తిరిగి TRUE;
    ఒకవేళ (actual->tt_type == VAR_OBJECT && expected->tt_type == VAR_OBJECT)
	తిరిగి TRUE;
    ఒకవేళ ((actual->tt_type == VAR_LIST
		|| actual->tt_type == VAR_TUPLE
		|| actual->tt_type == VAR_DICT)
	    && actual->tt_type == expected->tt_type)
	// This takes care of a nested list or dict.
	తిరిగి use_typecheck(actual->tt_member, expected->tt_member);
    తిరిగి FALSE;
}

/*
 * Check that
 * - "actual" matches "expected" type or
 * - "actual" is a type that can be "expected" type: add a runtime check; or
 * - return FAIL.
 * If "actual_is_const" is TRUE then the type won't change at runtime, do not
 * generate a TYPECHECK.
 * If "typechk_flags" has TYPECHK_NUMBER_OK, then a number is accepted for
 * a float.
 * If "typechk_flags" has TYPECHK_TUPLE_OK, then a tuple is accepted for a
 * list.
 */
    పూర్ణాంకం
need_type_where(
	type_T	*actual,
	type_T	*expected,
	పూర్ణాంకం	typechk_flags,	// acceptable types (type check flags)
	పూర్ణాంకం	స్థానభ్రంశం,
	where_T	where,
	cctx_T	*cctx,
	పూర్ణాంకం	silent,
	పూర్ణాంకం	actual_is_const)
{
    పూర్ణాంకం ret;

    ఒకవేళ (expected->tt_type != VAR_CLASS && expected->tt_type != VAR_TYPEALIAS)
    {
	ఒకవేళ (check_type_is_విలువ(actual) == FAIL)
	    తిరిగి FAIL;
    }

    ఒకవేళ (expected == &t_bool && actual != &t_bool
					&& (actual->tt_flags & TTFLAG_BOOL_OK))
    {
	// Using "0", "1" or the result of an expression with "&&" or "||" as a
	// boolean is OK but requires a conversion.
	generate_2BOOL(cctx, FALSE, స్థానభ్రంశం);
	తిరిగి OK;
    }

    ret = check_type_maybe(expected, actual, FALSE, where);
    ఒకవేళ (ret == OK)
	తిరిగి OK;

    // If actual a constant a runtime check makes no sense.  If it's
    // null_function it is OK.
    ఒకవేళ (actual_is_const && ret == MAYBE && actual == &t_func_unknown)
	తిరిగి OK;

    // If the actual type can be the expected type add a runtime check.
    ఒకవేళ (!actual_is_const && ret == MAYBE && use_typecheck(actual, expected))
    {
	generate_TYPECHECK(cctx, expected, typechk_flags, స్థానభ్రంశం,
		where.wt_రకం == WT_VARIABLE, where.wt_సూచీ);
	తిరిగి OK;
    }

    ఒకవేళ (!silent)
	type_mismatch_where(expected, actual, where);
    తిరిగి FAIL;
}

    పూర్ణాంకం
need_type(
	type_T	*actual,
	type_T	*expected,
	పూర్ణాంకం	typechk_flags,	// acceptable types (type check flags)
	పూర్ణాంకం	స్థానభ్రంశం,
	పూర్ణాంకం	arg_idx,
	cctx_T	*cctx,
	పూర్ణాంకం	silent,
	పూర్ణాంకం	actual_is_const)
{
    where_T where = WHERE_INIT;

    ఒకవేళ (arg_idx > 0)
    {
	where.wt_సూచీ = arg_idx;
	where.wt_రకం = WT_వాదం;
    }
    తిరిగి need_type_where(actual, expected, typechk_flags, స్థానభ్రంశం, where,
						cctx, silent, actual_is_const);
}

/*
 * Set type of variable "lvar" to "type".  If the variable is a constant then
 * the type gets TTFLAG_CONST.
 */
    స్థిర శూన్య
సమితి_var_type(lvar_T *lvar, type_T *type_arg, cctx_T *cctx)
{
    type_T	*type = type_arg;

    ఒకవేళ (lvar->lv_const == కేటాయించు_CONST && (type->tt_flags & TTFLAG_CONST) == 0)
    {
	ఒకవేళ (type->tt_flags & TTFLAG_STATIC)
	    // entry in static_types[] is followed by const type
	    type = type + 1;
	లేకపోతే
	{
	    type = copy_type(type, cctx->ctx_type_list);
	    type->tt_flags |= TTFLAG_CONST;
	}
    }
    lvar->lv_type = type;
}

/*
 * Reserve space for a local variable.
 * "assign" can be ASSIGN_VAR for :var, ASSIGN_CONST for :const and
 * ASSIGN_FINAL for :final.
 * Return the variable or NULL if it failed.
 */
    lvar_T *
reserve_local(
	cctx_T	*cctx,
	char_u	*name,
	size_t	len,
	పూర్ణాంకం	కేటాయించు,
	type_T	*type)
{
    lvar_T  *lvar;
    dfunc_T *dfunc;

    ఒకవేళ (arg_exists(name, len, ఖాళీ, ఖాళీ, ఖాళీ, cctx) == OK)
    {
	emsg_namelen(_(e_str_is_వాడిన_as_వాదం), name, (పూర్ణాంకం)len);
	తిరిగి ఖాళీ;
    }

    ఒకవేళ (GA_GROW_FAILS(&cctx->ctx_locals, 1))
	తిరిగి ఖాళీ;
    lvar = ((lvar_T *)cctx->ctx_locals.ga_దత్తాంశం) + cctx->ctx_locals.ga_len++;
    CLEAR_సూచిక(lvar);

    // Every local variable uses the next entry on the stack.  We could re-use
    // the last ones when leaving a scope, but then variables used in a closure
    // might get overwritten.  To keep things simple do not re-use stack
    // entries.  This is less efficient, but memory is cheap these days.
    dfunc = ((dfunc_T *)def_functions.ga_దత్తాంశం) + cctx->ctx_ufunc->uf_dfunc_idx;
    lvar->lv_idx = dfunc->df_var_పేర్లు.ga_len;

    lvar->lv_name = vim_strnsave(name, len == 0 ? STRLEN(name) : len);
    lvar->lv_const = కేటాయించు;
    ఒకవేళ (type == &t_unknown || type == &t_any)
	// type not known yet, may be inferred from RHS
	lvar->lv_type = type;
    లేకపోతే
	// may use TTFLAG_CONST
	సమితి_var_type(lvar, type, cctx);

    // Remember the name for debugging.
    ఒకవేళ (GA_GROW_FAILS(&dfunc->df_var_పేర్లు, 1))
	తిరిగి ఖాళీ;
    ((char_u **)dfunc->df_var_పేర్లు.ga_దత్తాంశం)[lvar->lv_idx] =
						    vim_strsave(lvar->lv_name);
    ++dfunc->df_var_పేర్లు.ga_len;

    తిరిగి lvar;
}

/*
 * If "check_writable" is ASSIGN_CONST give an error if the variable was
 * defined with :final or :const, if "check_writable" is ASSIGN_FINAL give an
 * error if the variable was defined with :const.
 */
    స్థిర పూర్ణాంకం
check_అంశం_writable(svar_T *sv, పూర్ణాంకం check_writable, char_u *name)
{
    ఒకవేళ ((check_writable == కేటాయించు_CONST && sv->sv_const != 0)
	    || (check_writable == కేటాయించు_FINAL
					      && sv->sv_const == కేటాయించు_CONST))
    {
	semsg(_(e_cannot_change_readonly_variable_str), name);
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * Find "name" in script-local items of script "sid".
 * Pass "check_writable" to check_item_writable().
 * "cctx" is NULL at the script level, "cstack" is NULL in a function.
 * Returns the index in "sn_var_vals" if found.
 * If found but not in "sn_var_vals" returns -1.
 * If not found or the variable is not writable returns -2.
 */
    పూర్ణాంకం
get_script_అంశం_idx(
	పూర్ణాంకం	    sid,
	char_u	    *name,
	పూర్ణాంకం	    check_writable,
	cctx_T	    *cctx,
	cstack_T    *cstack)
{
    hashtab_T	    *ht;
    dictitem_T	    *di;
    scriptitem_T    *si = SCRIPT_అంశం(sid);
    svar_T	    *sv;
    పూర్ణాంకం		    idx;

    ఒకవేళ (!SCRIPT_ID_VALID(sid))
	తిరిగి -1;
    ఒకవేళ (sid == current_sctx.sc_sid)
    {
	sallvar_T *sav = కనుగొను_script_var(name, 0, cctx, cstack);

	ఒకవేళ (sav == ఖాళీ)
	    తిరిగి -2;
	idx = sav->sav_var_vals_idx;
	sv = ((svar_T *)si->sn_var_vals.ga_దత్తాంశం) + idx;
	ఒకవేళ (check_అంశం_writable(sv, check_writable, name) == FAIL)
	    తిరిగి -2;
	తిరిగి idx;
    }

    // First look the name up in the hashtable.
    ht = &SCRIPT_VARS(sid);
    di = కనుగొను_var_లోపల_ht(ht, 0, name, TRUE);
    ఒకవేళ (di == ఖాళీ)
    {
	ఒకవేళ (si->sn_autoload_ఉపసర్గ != ఖాళీ)
	{
	    hashitem_T *hi;

	    // A variable exported from an autoload script is in the global
	    // variables, we can find it in the all_vars table.
	    hi = hash_కనుగొను(&si->sn_all_vars.dv_hashtab, name);
	    ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
		తిరిగి HI2SAV(hi)->sav_var_vals_idx;
	}
	తిరిగి -2;
    }

    // Now find the svar_T index in sn_var_vals.
    కొరకు (idx = 0; idx < si->sn_var_vals.ga_len; ++idx)
    {
	sv = ((svar_T *)si->sn_var_vals.ga_దత్తాంశం) + idx;
	ఒకవేళ (sv->sv_tv == &di->di_tv)
	{
	    ఒకవేళ (check_అంశం_writable(sv, check_writable, name) == FAIL)
		తిరిగి -2;
	    తిరిగి idx;
	}
    }
    తిరిగి -1;
}

    స్థిర imported_T *
కనుగొను_imported_లోపల_script(char_u *name, size_t len, పూర్ణాంకం sid)
{
    scriptitem_T    *si;
    పూర్ణాంకం		    idx;

    ఒకవేళ (!SCRIPT_ID_VALID(sid))
	తిరిగి ఖాళీ;
    si = SCRIPT_అంశం(sid);
    కొరకు (idx = 0; idx < si->sn_imports.ga_len; ++idx)
    {
	imported_T *import = ((imported_T *)si->sn_imports.ga_దత్తాంశం) + idx;

	ఒకవేళ (len == 0 ? STRCMP(name, import->imp_name) == 0
		     : STRLEN(import->imp_name) == len
				  && STRNCMP(name, import->imp_name, len) == 0)
	    తిరిగి import;
    }
    తిరిగి ఖాళీ;
}

/*
 * Find "name" in imported items of the current script.
 * If "len" is 0 use any length that works.
 * If "load" is TRUE and the script was not loaded yet, load it now.
 */
    imported_T *
కనుగొను_imported(char_u *name, size_t len, పూర్ణాంకం ఎక్కించు)
{
    ఒకవేళ (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	తిరిగి ఖాళీ;

    // Skip over "s:" before "s:something" to find the import name.
    పూర్ణాంకం off = name[0] == 's' && name[1] == ':' ? 2 : 0;

    imported_T *ret = కనుగొను_imported_లోపల_script(name + off, len - off,
							  current_sctx.sc_sid);
    ఒకవేళ (ret != ఖాళీ && ఎక్కించు && (ret->imp_flags & IMP_FLAGS_AUTOLOAD))
    {
	scid_T	actual_sid = 0;
	పూర్ణాంకం	save_emsg_off = emsg_off;

	// "emsg_off" will be set when evaluating an expression silently, but
	// we do want to know about errors in a script.  Also because it then
	// aborts when an error is encountered.
	emsg_off = FALSE;

	// script found before but not loaded yet
	ret->imp_flags &= ~IMP_FLAGS_AUTOLOAD;
	(శూన్య)do_ఆధారం(SCRIPT_అంశం(ret->imp_sid)->sn_name, FALSE,
						       DOSO_NONE, &actual_sid);
	// If the script is a symlink it may be sourced with another name, may
	// need to adjust the script ID for that.
	ఒకవేళ (actual_sid != 0)
	    ret->imp_sid = actual_sid;

	emsg_off = save_emsg_off;
    }
    తిరిగి ret;
}

/*
 * Called when checking for a following operator at "arg".  When the rest of
 * the line is empty or only a comment, peek the next line.  If there is a next
 * line return a pointer to it and set "nextp".
 * Otherwise skip over white space.
 */
    char_u *
may_peek_తదుపరి_పంక్తి(cctx_T *cctx, char_u *arg, char_u **nextp)
{
    char_u *p = skipwhite(arg);

    *nextp = ఖాళీ;
    ఒకవేళ (*p == NUL || (VIM_ISWHITE(*arg) && vim9_comment_ప్రారంభం(p)))
    {
	*nextp = peek_తదుపరి_పంక్తి_from_context(cctx);
	ఒకవేళ (*nextp != ఖాళీ)
	    తిరిగి *nextp;
    }
    తిరిగి p;
}

/*
 * Return a pointer to the next line that isn't empty or only contains a
 * comment. Skips over white space.
 * Returns NULL if there is none.
 */
    char_u *
peek_తదుపరి_పంక్తి_from_context(cctx_T *cctx)
{
    పూర్ణాంకం lnum = cctx->ctx_lnum;

    అయితే (++lnum < cctx->ctx_ufunc->uf_పంక్తులు.ga_len)
    {
	char_u *పంక్తి = ((char_u **)cctx->ctx_ufunc->uf_పంక్తులు.ga_దత్తాంశం)[lnum];
	char_u *p;

	// ignore NULLs inserted for continuation lines
	ఒకవేళ (పంక్తి != ఖాళీ)
	{
	    p = skipwhite(పంక్తి);
	    ఒకవేళ (vim9_bad_comment(p))
		తిరిగి ఖాళీ;
	    ఒకవేళ (*p != NUL && !vim9_comment_ప్రారంభం(p))
		తిరిగి p;
	}
    }
    తిరిగి ఖాళీ;
}

/*
 * Get the next line of the function from "cctx".
 * Skips over empty lines.  Skips over comment lines if "skip_comment" is TRUE.
 * Returns NULL when at the end.
 */
    char_u *
తదుపరి_పంక్తి_from_context(cctx_T *cctx, పూర్ణాంకం skip_comment)
{
    char_u	*పంక్తి;

    చేయి
    {
	++cctx->ctx_lnum;
	ఒకవేళ (cctx->ctx_lnum >= cctx->ctx_ufunc->uf_పంక్తులు.ga_len)
	{
	    పంక్తి = ఖాళీ;
	    ఆపు;
	}
	పంక్తి = ((char_u **)cctx->ctx_ufunc->uf_పంక్తులు.ga_దత్తాంశం)[cctx->ctx_lnum];
	cctx->ctx_పంక్తి_ప్రారంభం = పంక్తి;
	SOURCING_LNUM = cctx->ctx_lnum + 1;
    } అయితే (పంక్తి == ఖాళీ || *skipwhite(పంక్తి) == NUL
		     || (skip_comment && vim9_comment_ప్రారంభం(skipwhite(పంక్తి))));
    తిరిగి పంక్తి;
}

/*
 * Skip over white space at "whitep" and assign to "*arg".
 * If "*arg" is at the end of the line, advance to the next line.
 * Also when "whitep" points to white space and "*arg" is on a "#".
 * Return FAIL if beyond the last line, "*arg" is unmodified then.
 */
    పూర్ణాంకం
may_get_తదుపరి_పంక్తి(char_u *whitep, char_u **arg, cctx_T *cctx)
{
    *arg = skipwhite(whitep);
    ఒకవేళ (vim9_bad_comment(*arg))
	తిరిగి FAIL;
    ఒకవేళ (**arg == NUL || (VIM_ISWHITE(*whitep) && vim9_comment_ప్రారంభం(*arg)))
    {
	char_u *తదుపరి = తదుపరి_పంక్తి_from_context(cctx, TRUE);

	ఒకవేళ (తదుపరి == ఖాళీ)
	    తిరిగి FAIL;
	*arg = skipwhite(తదుపరి);
    }
    తిరిగి OK;
}

/*
 * Idem, and give an error when failed.
 */
    పూర్ణాంకం
may_get_తదుపరి_పంక్తి_error(char_u *whitep, char_u **arg, cctx_T *cctx)
{
    ఒకవేళ (may_get_తదుపరి_పంక్తి(whitep, arg, cctx) == FAIL)
    {
	SOURCING_LNUM = cctx->ctx_lnum + 1;
	emsg(_(e_పంక్తి_incomplete));
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * Get a line from the compilation context, compatible with exarg_T getline().
 * Return a pointer to the line in allocated memory.
 * Return NULL for end-of-file or some error.
 */
    స్థిర char_u *
exarg_getline(
	పూర్ణాంకం c వాడని,
	శూన్య *cookie,
	పూర్ణాంకం indent వాడని,
	getline_opt_T options వాడని)
{
    cctx_T  *cctx = (cctx_T *)cookie;
    char_u  *p;

    కొరకు (;;)
    {
	ఒకవేళ (cctx->ctx_lnum >= cctx->ctx_ufunc->uf_పంక్తులు.ga_len - 1)
	    తిరిగి ఖాళీ;
	++cctx->ctx_lnum;
	p = ((char_u **)cctx->ctx_ufunc->uf_పంక్తులు.ga_దత్తాంశం)[cctx->ctx_lnum];
	// Comment lines result in NULL pointers, skip them.
	ఒకవేళ (p != ఖాళీ)
	    తిరిగి vim_strsave(p);
    }
}

    శూన్య
fill_exarg_from_cctx(exarg_T *eap, cctx_T *cctx)
{
    eap->ea_getline = exarg_getline;
    eap->cookie = cctx;
    eap->skip = cctx->ctx_skip == SKIP_YES;
}

/*
 * Return TRUE if "ufunc" should be compiled, taking into account whether
 * "profile" indicates profiling is to be done.
 */
    పూర్ణాంకం
func_needs_compiling(ufunc_T *ufunc, compiletype_T compile_type)
{
    ఎంపిక (ufunc->uf_def_స్థితి)
    {
	స్థితి UF_గమ్యం_BE_COMPILED:
	    తిరిగి TRUE;

	స్థితి UF_COMPILED:
	{
	    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_దత్తాంశం)
							 + ufunc->uf_dfunc_idx;

	    ఎంపిక (compile_type)
	    {
		స్థితి CT_PROFILE:
#నిర్వచితమైతే FEAT_PROFILE
		    తిరిగి dfunc->df_instr_prof == ఖాళీ;
#అయితే_ముగింపు
		స్థితి CT_NONE:
		    తిరిగి dfunc->df_instr == ఖాళీ;
		స్థితి CT_DEBUG:
		    తిరిగి dfunc->df_instr_debug == ఖాళీ;
	    }
	}

	స్థితి UF_NOT_COMPILED:
	స్థితి UF_COMPILE_ERROR:
	స్థితి UF_COMPILING:
	    ఆపు;
    }
    తిరిగి FALSE;
}

/*
 * Compile a nested :def command.
 */
    స్థిర char_u *
compile_nested_function(exarg_T *eap, cctx_T *cctx, garray_T *పంక్తులు_గమ్యం_free)
{
    పూర్ణాంకం		is_global = *eap->arg == 'g' && eap->arg[1] == ':';
    char_u	*name_ప్రారంభం = eap->arg;
    char_u	*name_చివర = గమ్యం_name_చివర(eap->arg, TRUE);
    పూర్ణాంకం		off;
    char_u	*func_name;
    string_T	lambda_name;
    ufunc_T	*ufunc;
    పూర్ణాంకం		r = FAIL;
    compiletype_T   compile_type;
    పూర్ణాంకం		funcref_isn_idx = -1;
    lvar_T	*lvar = ఖాళీ;
    char_u	*bracket_ప్రారంభం = ఖాళీ;

    ఒకవేళ (eap->forceit)
    {
	semsg(_(e_cannot_use_bang_with_nested_def_str),
		eap->cmdidx == CMD_def ? ":def" : ":function");
	తిరిగి ఖాళీ;
    }

    ఒకవేళ (*name_ప్రారంభం == '/')
    {
	name_చివర = skip_regexp(name_ప్రారంభం + 1, '/', TRUE);
	ఒకవేళ (*name_చివర == '/')
	    ++name_చివర;
	సమితి_nextcmd(eap, name_చివర);
    }

    ఒకవేళ (*name_చివర == '<')
    {
	bracket_ప్రారంభం = name_చివర;
	ఒకవేళ (skip_generic_func_type_వాదనలు(&name_చివర) == FAIL)
	    తిరిగి ఖాళీ;
    }

    ఒకవేళ (name_చివర == name_ప్రారంభం || *skipwhite(name_చివర) != '(')
    {
	ఒకవేళ (!చివర్లు_excmd2(name_ప్రారంభం, name_చివర))
	{
	    ఒకవేళ (*skipwhite(name_చివర) == '.')
		semsg(_(e_cannot_define_dict_func_లోపల_vim9_script_str),
								     eap->cmd);
	    లేకపోతే
		semsg(_(e_invalid_command_str), eap->cmd);
	    తిరిగి ఖాళీ;
	}

	// "def" or "def Name": list functions
	ఒకవేళ (generate_DEF(cctx, name_ప్రారంభం, name_చివర - name_ప్రారంభం) == FAIL)
	    తిరిగి ఖాళీ;
	తిరిగి eap->nextcmd == ఖాళీ ? (char_u *)"" : eap->nextcmd;
    }

    ఒకవేళ (bracket_ప్రారంభం != ఖాళీ)
	// generic function.  The function name ends before the list of types
	// (opening angle bracket).
	name_చివర = bracket_ప్రారంభం;

    // Only g:Func() can use a namespace.
    ఒకవేళ (name_ప్రారంభం[1] == ':' && !is_global)
    {
	semsg(_(e_namespace_not_supported_str), name_ప్రారంభం);
	తిరిగి ఖాళీ;
    }
    ఒకవేళ (cctx->ctx_skip != SKIP_YES
	    && check_defined(name_ప్రారంభం, name_చివర - name_ప్రారంభం, cctx,
							  ఖాళీ, FALSE) == FAIL)
	తిరిగి ఖాళీ;
    ఒకవేళ (!ASCII_ISUPPER(is_global ? name_ప్రారంభం[2] : name_ప్రారంభం[0]))
    {
	semsg(_(e_function_name_must_ప్రారంభం_with_capital_str), name_ప్రారంభం);
	తిరిగి ఖాళీ;
    }

    eap->arg = name_చివర;
    fill_exarg_from_cctx(eap, cctx);

    eap->forceit = FALSE;
    // We use the special <lambda>99 name, but it's not really a lambda.
    lambda_name = get_lambda_name();
    lambda_name.string = vim_strnsave(lambda_name.string, lambda_name.నిడివి);
    ఒకవేళ (lambda_name.string == ఖాళీ)
	తిరిగి ఖాళీ;

    // This may free the current line, make a copy of the name.
    off = is_global ? 2 : 0;
    func_name = vim_strnsave(name_ప్రారంభం + off, name_చివర - name_ప్రారంభం - off);
    ఒకవేళ (func_name == ఖాళీ)
    {
	r = FAIL;
	వెళ్ళు theend;
    }

    // A type variable cannot have the name of the function.  The function is
    // defined with the "<lambda>" name, check it here.
    ఒకవేళ (bracket_ప్రారంభం != ఖాళీ)
    {
	size_t	len = STRLEN(func_name);

	కొరకు (char_u *t = bracket_ప్రారంభం + 1; *t != '>' && *t != NUL; )
	{
	    char_u *te = గమ్యం_name_చివర(t, FALSE);

	    ఒకవేళ ((size_t)(te - t) == len && STRNCMP(t, func_name, len) == 0)
	    {
		semsg(_(e_redefining_script_అంశం_str), func_name);
		r = FAIL;
		వెళ్ళు theend;
	    }
	    t = te == t ? t + 1 : te;
	}
    }

    // Make sure "KeyTyped" is not set, it may cause indent to be written.
    పూర్ణాంకం save_కీలకం_Typed = కీలకం_Typed;
    కీలకం_Typed = FALSE;

    ufunc = define_function(eap, lambda_name.string, పంక్తులు_గమ్యం_free, 0, ఖాళీ, 0,
									cctx);

    కీలకం_Typed = save_కీలకం_Typed;

    ఒకవేళ (ufunc == ఖాళీ)
    {
	r = eap->skip ? OK : FAIL;
	వెళ్ళు theend;
    }
    ఒకవేళ (eap->nextcmd != ఖాళీ)
    {
	semsg(_(e_పాఠ్యం_దొరికింది_after_str_str),
	      eap->cmdidx == CMD_def ? "enddef" : "endfunction", eap->nextcmd);
	r = FAIL;
	func_ptr_unref(ufunc);
	వెళ్ళు theend;
    }

    // copy over the block scope IDs before compiling
    ఒకవేళ (!is_global && cctx->ctx_ufunc->uf_ఖండం_లోతు > 0)
    {
	పూర్ణాంకం ఖండం_లోతు = cctx->ctx_ufunc->uf_ఖండం_లోతు;

	ufunc->uf_ఖండం_ids = ALLOC_MULT(పూర్ణాంకం, ఖండం_లోతు);
	ఒకవేళ (ufunc->uf_ఖండం_ids != ఖాళీ)
	{
	    mch_memmove(ufunc->uf_ఖండం_ids, cctx->ctx_ufunc->uf_ఖండం_ids,
						    పరిమాణం(పూర్ణాంకం) * ఖండం_లోతు);
	    ufunc->uf_ఖండం_లోతు = ఖండం_లోతు;
	}
    }

    // Define the funcref before compiling, so that it is found by any
    // recursive call.
    ఒకవేళ (is_global)
    {
	r = generate_NEWFUNC(cctx, lambda_name.string, func_name);
	func_name = ఖాళీ;
	lambda_name.string = ఖాళీ;
	lambda_name.నిడివి = 0;
    }
    లేకపోతే
    {
	// Define a local variable for the function reference.
	lvar = reserve_local(cctx, func_name, name_చివర - name_ప్రారంభం,
					    కేటాయించు_CONST, ufunc->uf_func_type);
	ఒకవేళ (lvar == ఖాళీ)
	{
	    func_ptr_unref(ufunc);
	    వెళ్ళు theend;
	}
	lvar->lv_ufunc = ufunc;
	ఒకవేళ (generate_FUNCREF(cctx, ufunc, ఖాళీ, FALSE, 0, &funcref_isn_idx) == FAIL)
	{
	    func_ptr_unref(ufunc);
	    వెళ్ళు theend;
	}
	r = generate_STORE(cctx, ISN_STORE, lvar->lv_idx, ఖాళీ);
    }

    compile_type = get_compile_type(ufunc);
#నిర్వచితమైతే FEAT_PROFILE
    // If the outer function is profiled, also compile the nested function for
    // profiling.
    ఒకవేళ (cctx->ctx_compile_type == CT_PROFILE)
	compile_type = CT_PROFILE;
#అయితే_ముగింపు
    ఒకవేళ (func_needs_compiling(ufunc, compile_type)
	    && compile_def_function(ufunc, TRUE, compile_type, cctx) == FAIL)
    {
	func_ptr_unref(ufunc);
	ఒకవేళ (lvar != ఖాళీ)
	    // Now the local variable can't be used.
	    *lvar->lv_name = '/';  // impossible value
	వెళ్ళు theend;
    }

#నిర్వచితమైతే FEAT_PROFILE
    // When the outer function is compiled for profiling, the nested function
    // may be called without profiling.  Compile it here in the right context.
    ఒకవేళ (compile_type == CT_PROFILE && func_needs_compiling(ufunc, CT_NONE))
	compile_def_function(ufunc, FALSE, CT_NONE, cctx);
#అయితే_ముగింపు

    // If a FUNCREF instruction was generated, set the index after compiling.
    ఒకవేళ (funcref_isn_idx != -1 && ufunc->uf_def_స్థితి == UF_COMPILED)
    {
	isn_T	*funcref_isn = ((isn_T *)cctx->ctx_instr.ga_దత్తాంశం) +
							funcref_isn_idx;
	funcref_isn->isn_arg.funcref.fr_dfunc_idx = ufunc->uf_dfunc_idx;
    }

theend:
    vim_free(lambda_name.string);
    vim_free(func_name);
    తిరిగి r == FAIL ? ఖాళీ : (char_u *)"";
}

/*
 * Compile one Vim expression {expr} in string "p".
 * "p" points to the opening "{".
 * Return a pointer to the character after "}", NULL for an error.
 */
    char_u *
compile_one_సమాసం_లోపల_str(char_u *p, cctx_T *cctx)
{
    char_u	*ఖండం_ప్రారంభం;
    char_u	*ఖండం_చివర;

    // Skip the opening {.
    ఖండం_ప్రారంభం = skipwhite(p + 1);
    ఖండం_చివర = ఖండం_ప్రారంభం;
    ఒకవేళ (*ఖండం_ప్రారంభం != NUL && skip_సమాసం(&ఖండం_చివర, ఖాళీ) == FAIL)
	తిరిగి ఖాళీ;
    ఖండం_చివర = skipwhite(ఖండం_చివర);
    // The block must be closed by a }.
    ఒకవేళ (*ఖండం_చివర != '}')
    {
	semsg(_(e_missing_close_curly_str), p);
	తిరిగి ఖాళీ;
    }
    ఒకవేళ (compile_expr0(&ఖండం_ప్రారంభం, cctx) == FAIL)
	తిరిగి ఖాళీ;
    may_generate_2STRING(-1, TOSTRING_INTERPOLATE, cctx);

    తిరిగి ఖండం_చివర + 1;
}

/*
 * Compile a string "str" (either containing a literal string or a mix of
 * literal strings and Vim expressions of the form `{expr}`).  This is used
 * when compiling a heredoc assignment to a variable or an interpolated string
 * in a Vim9 def function.  Vim9 instructions are generated to push strings,
 * evaluate expressions, concatenate them and create a list of lines.  When
 * "evalstr" is TRUE, Vim expressions in "str" are evaluated.
 */
    పూర్ణాంకం
compile_all_సమాసం_లోపల_str(char_u *str, పూర్ణాంకం evalstr, cctx_T *cctx)
{
    char_u	*p = str;
    char_u	*val;
    పూర్ణాంకం		లెక్క = 0;

    ఒకవేళ (cctx->ctx_skip == SKIP_YES)
	తిరిగి OK;

    ఒకవేళ (!evalstr || *str == NUL)
    {
	// Literal string, possibly empty.
	val = *str != NUL ? vim_strsave(str) : ఖాళీ;
	తిరిగి generate_PUSHS(cctx, &val);
    }

    // Push all the string pieces to the stack, followed by a ISN_CONCAT.
    అయితే (*p != NUL)
    {
	char_u	*lit_ప్రారంభం;
	పూర్ణాంకం	escaped_brace = FALSE;

	// Look for a block start.
	lit_ప్రారంభం = p;
	అయితే (*p != '{' && *p != '}' && *p != NUL)
	    ++p;

	ఒకవేళ (*p != NUL && *p == p[1])
	{
	    // Escaped brace, unescape and continue.
	    // Include the brace in the literal string.
	    ++p;
	    escaped_brace = TRUE;
	}
	లేకపోతే ఒకవేళ (*p == '}')
	{
	    semsg(_(e_stray_closing_curly_str), str);
	    తిరిగి FAIL;
	}

	// Append the literal part.
	ఒకవేళ (p != lit_ప్రారంభం)
	{
	    val = vim_strnsave(lit_ప్రారంభం, (size_t)(p - lit_ప్రారంభం));
	    ఒకవేళ (generate_PUSHS(cctx, &val) == FAIL)
		తిరిగి FAIL;
	    ++లెక్క;
	}

	ఒకవేళ (*p == NUL)
	    ఆపు;

	ఒకవేళ (escaped_brace)
	{
	    // Skip the second brace.
	    ++p;
	    కొనసాగు;
	}

	p = compile_one_సమాసం_లోపల_str(p, cctx);
	ఒకవేళ (p == ఖాళీ)
	    తిరిగి FAIL;
	++లెక్క;
    }

    // Small optimization, if there's only a single piece skip the ISN_CONCAT.
    ఒకవేళ (లెక్క > 1)
	తిరిగి generate_CONCAT(cctx, లెక్క);

    తిరిగి OK;
}

/*
 * Return the length of an assignment operator, or zero if there isn't one.
 */
    పూర్ణాంకం
assignment_len(char_u *p, పూర్ణాంకం *heredoc)
{
    ఒకవేళ (*p == '=')
    {
	ఒకవేళ (p[1] == '<' && p[2] == '<')
	{
	    *heredoc = TRUE;
	    తిరిగి 3;
	}
	తిరిగి 1;
    }
    ఒకవేళ (vim_strchr((char_u *)"+-*/%", *p) != ఖాళీ && p[1] == '=')
	తిరిగి 2;
    ఒకవేళ (STRNCMP(p, "..=", 3) == 0)
	తిరిగి 3;
    తిరిగి 0;
}

/*
 * Generate the load instruction for "name".
 */
    స్థిర పూర్ణాంకం
generate_loadvar(cctx_T *cctx, lhs_T *lhs)
{
    char_u	*name = lhs->lhs_name;
    type_T	*type = lhs->lhs_type;
    పూర్ణాంకం		res = OK;

    ఎంపిక (lhs->lhs_dest)
    {
	స్థితి dest_option:
	స్థితి dest_func_option:
	    generate_ఎక్కించు(cctx, ISN_LOADOPT, 0, name, type);
	    ఆపు;
	స్థితి dest_global:
	    ఒకవేళ (vim_strchr(name, AUTOLOAD_CHAR) == ఖాళీ)
	    {
		ఒకవేళ (name[2] == NUL)
		    generate_instr_type(cctx, ISN_LOADGDICT, &t_dict_any);
		లేకపోతే
		    generate_ఎక్కించు(cctx, ISN_LOADG, 0, name + 2, type);
	    }
	    లేకపోతే
		generate_ఎక్కించు(cctx, ISN_LOADAUTO, 0, name, type);
	    ఆపు;
	స్థితి dest_బఫరు:
	    generate_ఎక్కించు(cctx, ISN_LOADB, 0, name + 2, type);
	    ఆపు;
	స్థితి dest_కిటికీ:
	    generate_ఎక్కించు(cctx, ISN_LOADW, 0, name + 2, type);
	    ఆపు;
	స్థితి dest_tab:
	    generate_ఎక్కించు(cctx, ISN_LOADT, 0, name + 2, type);
	    ఆపు;
	స్థితి dest_script:
	స్థితి dest_script_v9:
	    res = compile_ఎక్కించు_scriptvar(cctx,
			    name + (name[1] == ':' ? 2 : 0), ఖాళీ, ఖాళీ, ఖాళీ);
	    ఆపు;
	స్థితి dest_env:
	    // Include $ in the name here
	    generate_ఎక్కించు(cctx, ISN_LOADENV, 0, name, type);
	    ఆపు;
	స్థితి dest_reg:
	    generate_ఎక్కించు(cctx, ISN_LOADREG, name[1], ఖాళీ, &t_string);
	    ఆపు;
	స్థితి dest_vimvar:
	    generate_LOADV(cctx, name + 2);
	    ఆపు;
	స్థితి dest_local:
	    ఒకవేళ (cctx->ctx_skip != SKIP_YES)
	    {
		lvar_T	*lvar = lhs->lhs_lvar;
		ఒకవేళ (lvar->lv_from_outer > 0)
		    generate_LOADOUTER(cctx, lvar->lv_idx, lvar->lv_from_outer,
				 lvar->lv_loop_లోతు, lvar->lv_loop_idx, type);
		లేకపోతే
		    generate_ఎక్కించు(cctx, ISN_ఎక్కించు, lvar->lv_idx, ఖాళీ, type);
	    }
	    ఆపు;
	స్థితి dest_class_member:
	    generate_CLASSMEMBER(cctx, TRUE, lhs->lhs_class,
						     lhs->lhs_classmember_idx);
	    ఆపు;
	స్థితి dest_సమాసం:
	    // list or dict value should already be on the stack.
	    ఆపు;
    }

    తిరిగి res;
}

/*
 * Skip over "[expr]" or ".member".
 * Does not check for any errors.
 */
    స్థిర char_u *
skip_సూచీ(char_u *ప్రారంభం)
{
    char_u *p = ప్రారంభం;

    ఒకవేళ (*p == '[')
    {
	p = skipwhite(p + 1);
	(శూన్య)skip_సమాసం(&p, ఖాళీ);
	p = skipwhite(p);
	ఒకవేళ (*p == ']')
	    తిరిగి p + 1;
	తిరిగి p;
    }
    // if (*p == '.')
    తిరిగి గమ్యం_name_చివర(p + 1, TRUE);
}

    శూన్య
vim9_declare_error(char_u *name)
{
    అక్షరం *scope = "";

    ఎంపిక (*name)
    {
	స్థితి 'g': scope = _("global"); ఆపు;
	స్థితి 'b': scope = _("buffer"); ఆపు;
	స్థితి 'w': scope = _("window"); ఆపు;
	స్థితి 't': scope = _("tab"); ఆపు;
	స్థితి 'v': scope = "v:"; ఆపు;
	స్థితి '$': semsg(_(e_cannot_declare_an_environment_variable_str), name);
		  తిరిగి;
	స్థితి '&': semsg(_(e_cannot_declare_an_option_str), name);
		  తిరిగి;
	స్థితి '@': semsg(_(e_cannot_declare_a_రిజిస్టరు_str), name);
		  తిరిగి;
	అప్రమేయ: తిరిగి;
    }
    semsg(_(e_cannot_declare_a_scope_variable_str), scope, name);
}

/*
 * Return TRUE if "name" is a valid register to use.
 * Return FALSE and give an error message if not.
 */
    స్థిర పూర్ణాంకం
valid_dest_reg(పూర్ణాంకం name)
{
    ఒకవేళ (name == '@')
       name = '"';
    ఒకవేళ (name == '/' || name == '=' || name == '#' || valid_yank_reg(name, TRUE))
	తిరిగి TRUE;
    emsg_invreg(name);
    తిరిగి FAIL;
}

/*
 * For one assignment figure out the type of destination.  Return it in "dest".
 * When not recognized "dest" is not set.
 * For an option "option_scope" is set.
 * For a v:var "vimvaridx" is set.
 * "type" is set to the destination type if known, unchanted otherwise.
 * When "has_index" is TRUE an item of the variable is assigned to.
 * Return FAIL if an error message was given.
 */
    పూర్ణాంకం
get_var_dest(
	char_u		*name,
	కేటాయించు_dest_T	*dest,
	cmdidx_T	cmdidx,
	పూర్ణాంకం		has_సూచీ,
	పూర్ణాంకం		*option_scope,
	పూర్ణాంకం		*vimvaridx,
	type_T		**type,
	cctx_T		*cctx)
{
    char_u *p;

    ఒకవేళ (*name == '&')
    {
	పూర్ణాంకం		cc;
	దీర్ఘ		numval;
	getoption_T	opt_type;
	పూర్ణాంకం		opt_p_flags;

	*dest = dest_option;
	ఒకవేళ (cmdidx == CMD_final || cmdidx == CMD_const)
	{
	    emsg(_(e_cannot_తాళం_option));
	    తిరిగి FAIL;
	}
	p = name;
	p = కనుగొను_option_చివర(&p, option_scope);
	ఒకవేళ (p == ఖాళీ)
	{
	    // cannot happen?
	    emsg(_(e_unexpected_characters_లోపల_assignment));
	    తిరిగి FAIL;
	}
	cc = *p;
	*p = NUL;
	opt_type = get_option_విలువ(skip_option_env_lead(name),
				   &numval, ఖాళీ, &opt_p_flags, *option_scope);
	*p = cc;
	ఎంపిక (opt_type)
	{
	    స్థితి gov_unknown:
		    semsg(_(e_unknown_option_str), name);
		    తిరిగి FAIL;
	    స్థితి gov_string:
	    స్థితి gov_hidden_string:
		    ఒకవేళ (opt_p_flags & P_FUNC)
		    {
			// might be a Funcref, check the type later
			*type = &t_any;
			*dest = dest_func_option;
		    }
		    లేకపోతే
		    {
			*type = &t_string;
		    }
		    ఆపు;
	    స్థితి gov_bool:
	    స్థితి gov_hidden_bool:
		    *type = &t_bool;
		    ఆపు;
	    స్థితి gov_సంఖ్య:
	    స్థితి gov_hidden_సంఖ్య:
		    *type = &t_సంఖ్య;
		    ఆపు;
	}
    }
    లేకపోతే ఒకవేళ (*name == '$')
    {
	*dest = dest_env;
	*type = &t_string;
    }
    లేకపోతే ఒకవేళ (*name == '@')
    {
	ఒకవేళ (!valid_dest_reg(name[1]))
	    తిరిగి FAIL;
	*dest = dest_reg;
	*type = name[1] == '#' ? &t_సంఖ్య_or_string : &t_string;
    }
    లేకపోతే ఒకవేళ (STRNCMP(name, "g:", 2) == 0)
    {
	*dest = dest_global;
    }
    లేకపోతే ఒకవేళ (STRNCMP(name, "b:", 2) == 0)
    {
	*dest = dest_బఫరు;
    }
    లేకపోతే ఒకవేళ (STRNCMP(name, "w:", 2) == 0)
    {
	*dest = dest_కిటికీ;
    }
    లేకపోతే ఒకవేళ (STRNCMP(name, "t:", 2) == 0)
    {
	*dest = dest_tab;
    }
    లేకపోతే ఒకవేళ (STRNCMP(name, "v:", 2) == 0)
    {
	typval_T	*vtv;
	పూర్ణాంకం		di_flags;

	*vimvaridx = కనుగొను_vim_var(name + 2, &di_flags);
	ఒకవేళ (*vimvaridx < 0)
	{
	    semsg(_(e_variable_not_దొరికింది_str), name);
	    తిరిగి FAIL;
	}
	// An item of a read-only variable can be changed, as when not compiled.
	// We use the current value of "sandbox" here, is that OK?
	ఒకవేళ (!has_సూచీ && var_check_ro(di_flags, name, FALSE))
	    తిరిగి FAIL;
	*dest = dest_vimvar;
	vtv = get_vim_var_tv(*vimvaridx);
	*type = typval2type_vimvar(vtv, cctx->ctx_type_list);
    }
    తిరిగి OK;
}

    స్థిర పూర్ణాంకం
is_decl_command(cmdidx_T cmdidx)
{
    తిరిగి cmdidx == CMD_let || cmdidx == CMD_var
				 || cmdidx == CMD_final || cmdidx == CMD_const;
}

/*
 * Returns TRUE if the class or object variable in "lhs" is modifiable.
 * "var_start" points to the start of the variable name and "lhs->lhs_varlen"
 * has the total length.  Note that the "lhs" can be nested an object reference
 * (e.g.  a.b.c.d.var).
 */
    స్థిర పూర్ణాంకం
lhs_class_member_modifiable(lhs_T *lhs, char_u	*var_ప్రారంభం, cctx_T *cctx)
{
    size_t	varlen = lhs->lhs_varlen;
    class_T	*cl = lhs->lhs_type->tt_class;
    పూర్ణాంకం		is_object = lhs->lhs_type->tt_type == VAR_OBJECT;
    char_u	*name = var_ప్రారంభం + varlen + 1;
    size_t	namelen = lhs->lhs_చివర - var_ప్రారంభం - varlen - 1;
    ocmember_T	*m;

    m = member_lookup(cl, lhs->lhs_type->tt_type, name, namelen, ఖాళీ);
    ఒకవేళ (m == ఖాళీ)
    {
	member_not_దొరికింది_సందేశం(cl, lhs->lhs_type->tt_type, name, namelen);
	తిరిగి FALSE;
    }

    ఒకవేళ (IS_ENUM(cl))
    {
	semsg(_(e_enumvalue_str_cannot_be_modified), cl->class_name.string,
		m->ocm_name.string);
	తిరిగి FALSE;
    }

    // If it is private member variable, then accessing it outside the
    // class is not allowed.
    // If it is a read only class variable, then it can be modified
    // only inside the class where it is defined.
    ఒకవేళ ((m->ocm_access != VIM_ACCESS_ALL) &&
	    ((is_object && !inside_class(cctx, cl))
	     || (!is_object && cctx->ctx_ufunc->uf_class != cl)))
    {
	అక్షరం *సందేశం = (m->ocm_access == VIM_ACCESS_వ్యక్తిగత)
				? e_cannot_access_protected_variable_str
				: e_variable_is_not_writable_str;
	emsg_var_cl_define(సందేశం, m->ocm_name.string, 0, cl);
	తిరిగి FALSE;
    }

    తిరిగి TRUE;
}

/*
 * Initialize "lhs" with default values
 */
    స్థిర శూన్య
lhs_init_defaults(lhs_T *lhs)
{
    CLEAR_సూచిక(lhs);
    lhs->lhs_dest = dest_local;
    lhs->lhs_vimvaridx = -1;
    lhs->lhs_scriptvar_idx = -1;
    lhs->lhs_member_idx = -1;
}

/*
 * When compiling a LHS variable name, find the end of the destination and the
 * end of the variable name.
 */
    స్థిర పూర్ణాంకం
lhs_కనుగొను_var_చివర(
    lhs_T	*lhs,
    char_u	*var_ప్రారంభం,
    పూర్ణాంకం		is_decl,
    char_u	**var_endp)
{
    char_u  *var_చివర = *var_endp;

    // "lhs_dest_end" is the end of the destination, including "[expr]" or
    // ".name".
    // "var_end" is the end of the variable/option/etc. name.
    lhs->lhs_dest_చివర = skip_var_one(var_ప్రారంభం, FALSE);
    ఒకవేళ (*var_ప్రారంభం == '@')
    {
	ఒకవేళ (!valid_dest_reg(var_ప్రారంభం[1]))
	    తిరిగి FAIL;
	var_చివర = var_ప్రారంభం + 2;
    }
    లేకపోతే
    {
	// skip over the leading "&", "&l:", "&g:" and "$"
	var_చివర = skip_option_env_lead(var_ప్రారంభం);
	var_చివర = గమ్యం_name_చివర(var_చివర, TRUE);
    }

    // "a: type" is declaring variable "a" with a type, not dict "a:".
    ఒకవేళ (is_decl && lhs->lhs_dest_చివర == var_ప్రారంభం + 2
					&& lhs->lhs_dest_చివర[-1] == ':')
	--lhs->lhs_dest_చివర;
    ఒకవేళ (is_decl && var_చివర == var_ప్రారంభం + 2 && var_చివర[-1] == ':')
	--var_చివర;

    lhs->lhs_చివర = lhs->lhs_dest_చివర;
    *var_endp = var_చివర;

    తిరిగి OK;
}

/*
 * Set various fields in "lhs"
 */
    స్థిర పూర్ణాంకం
lhs_init(
    lhs_T	*lhs,
    char_u	*var_ప్రారంభం,
    పూర్ణాంకం		is_decl,
    పూర్ణాంకం		heredoc,
    char_u	**var_endp)
{
    char_u *var_చివర = *var_endp;

    lhs_init_defaults(lhs);

    // Find the end of the variable and the destination
    ఒకవేళ (lhs_కనుగొను_var_చివర(lhs, var_ప్రారంభం, is_decl, &var_చివర) == FAIL)
	తిరిగి FAIL;

    // compute the length of the destination without "[expr]" or ".name"
    lhs->lhs_varlen = var_చివర - var_ప్రారంభం;
    lhs->lhs_varlen_మొత్తం = lhs->lhs_varlen;
    lhs->lhs_name = vim_strnsave(var_ప్రారంభం, lhs->lhs_varlen);
    ఒకవేళ (lhs->lhs_name == ఖాళీ)
	తిరిగి FAIL;

    ఒకవేళ (lhs->lhs_dest_చివర > var_ప్రారంభం + lhs->lhs_varlen)
	// Something follows after the variable: "var[idx]" or "var.key".
	lhs->lhs_has_సూచీ = TRUE;

    lhs->lhs_type = heredoc ? &t_list_string : &t_any;

    *var_endp = var_చివర;

    తిరిగి OK;
}

/*
 * Compile a LHS class variable name.
 */
    స్థిర పూర్ణాంకం
compile_lhs_class_variable(
    cctx_T	*cctx,
    lhs_T	*lhs,
    class_T	*defcl,
    పూర్ణాంకం		is_decl)
{
    ఒకవేళ (cctx->ctx_ufunc->uf_defclass != defcl)
    {
	// A class variable can be accessed without the class name
	// only inside a class.
	semsg(_(e_class_variable_str_accessible_only_inside_class_str),
		lhs->lhs_name, defcl->class_name.string);
	తిరిగి FAIL;
    }

    ఒకవేళ (is_decl)
    {
	semsg(_(e_variable_already_declared_లోపల_class_str), lhs->lhs_name);
	తిరిగి FAIL;
    }

    ocmember_T	*m = &defcl->class_class_members[lhs->lhs_classmember_idx];
    ఒకవేళ (oc_var_check_ro(defcl, m))
	తిరిగి FAIL;

    lhs->lhs_dest = dest_class_member;
    // The class variable is defined either in the current class or
    // in one of the parent class in the hierarchy.
    lhs->lhs_class = defcl;
    lhs->lhs_type = oc_member_type_by_idx(defcl, FALSE,
						lhs->lhs_classmember_idx);

    తిరిగి OK;
}

/*
 * Compile an imported LHS variable
 */
    స్థిర పూర్ణాంకం
compile_lhs_import_var(
    lhs_T	*lhs,
    imported_T	*import,
    char_u	*var_ప్రారంభం,
    char_u	**var_endp,
    char_u	**rawnamep)
{
    char_u	*var_చివర = *var_endp;
    char_u	*dot = vim_strchr(var_ప్రారంభం, '.');
    char_u	*p;

    // for an import the name is what comes after the dot
    ఒకవేళ (dot == ఖాళీ)
    {
	semsg(_(e_no_dot_after_imported_name_str), var_ప్రారంభం);
	తిరిగి FAIL;
    }

    p = skipwhite(dot + 1);
    var_చివర = గమ్యం_name_చివర(p, TRUE);
    ఒకవేళ (var_చివర == p)
    {
	semsg(_(e_missing_name_after_imported_name_str), var_ప్రారంభం);
	తిరిగి FAIL;
    }

    vim_free(lhs->lhs_name);
    lhs->lhs_varlen = var_చివర - p;
    lhs->lhs_name = vim_strnsave(p, lhs->lhs_varlen);
    ఒకవేళ (lhs->lhs_name == ఖాళీ)
	తిరిగి FAIL;
    *rawnamep = lhs->lhs_name;
    lhs->lhs_scriptvar_sid = import->imp_sid;

    // TODO: where do we check this name is exported?

    // Check if something follows: "exp.var[idx]" or
    // "exp.var.key".
    lhs->lhs_has_సూచీ = lhs->lhs_dest_చివర > skipwhite(var_చివర);

    *var_endp = var_చివర;

    తిరిగి OK;
}

/*
 * Process a script-local variable when compiling a LHS variable name.
 */
    స్థిర పూర్ణాంకం
compile_lhs_script_var(
    cctx_T	*cctx,
    lhs_T	*lhs,
    char_u	*var_ప్రారంభం,
    char_u	*var_చివర,
    పూర్ణాంకం		is_decl)
{
    పూర్ణాంకం		script_namespace = FALSE;
    పూర్ణాంకం		script_var = FALSE;
    imported_T	*import;
    char_u	*var_name;
    size_t	var_name_len;

    ఒకవేళ (lhs->lhs_varlen > 1 && STRNCMP(var_ప్రారంభం, "s:", 2) == 0)
	script_namespace = TRUE;

    ఒకవేళ (script_namespace)
    {
	var_name = var_ప్రారంభం + 2;
	var_name_len = lhs->lhs_varlen - 2;
    }
    లేకపోతే
    {
	var_name = var_ప్రారంభం;
	var_name_len = lhs->lhs_varlen;
    }

    ఒకవేళ (script_var_exists(var_name, var_name_len, cctx, ఖాళీ) == OK)
	script_var = TRUE;

    import = కనుగొను_imported(var_ప్రారంభం, lhs->lhs_varlen, FALSE);

    ఒకవేళ (script_namespace || script_var || import != ఖాళీ)
    {
	char_u *rawname = lhs->lhs_name + (lhs->lhs_name[1] == ':' ? 2 : 0);

	ఒకవేళ (script_namespace && current_script_is_vim9())
	{
	    semsg(_(e_cannot_use_s_colon_లోపల_vim9_script_str), var_ప్రారంభం);
	    తిరిగి FAIL;
	}

	ఒకవేళ (is_decl)
	{
	    ఒకవేళ (script_namespace)
		semsg(_(e_cannot_declare_script_variable_లోపల_function_str),
			lhs->lhs_name);
	    లేకపోతే
		semsg(_(e_variable_already_declared_లోపల_script_str),
			lhs->lhs_name);
	    తిరిగి FAIL;
	}
	లేకపోతే ఒకవేళ (cctx->ctx_ufunc->uf_script_ctx_సంస్కరణ == SCRIPT_సంస్కరణ_VIM9
		&& script_namespace
		&& !script_var && import == ఖాళీ)
	{
	    semsg(_(e_unknown_variable_str), lhs->lhs_name);
	    తిరిగి FAIL;
	}

	lhs->lhs_dest = current_script_is_vim9() ? dest_script_v9 :
								dest_script;

	// existing script-local variables should have a type
	lhs->lhs_scriptvar_sid = current_sctx.sc_sid;
	ఒకవేళ (import != ఖాళీ)
	{
	    ఒకవేళ (compile_lhs_import_var(lhs, import, var_ప్రారంభం, &var_చివర,
							&rawname) == FAIL)
		తిరిగి FAIL;
	}

	ఒకవేళ (SCRIPT_ID_VALID(lhs->lhs_scriptvar_sid))
	{
	    // Check writable only when no index follows.
	    lhs->lhs_scriptvar_idx = get_script_అంశం_idx(
					lhs->lhs_scriptvar_sid, rawname,
					lhs->lhs_has_సూచీ ?  కేటాయించు_FINAL :
					కేటాయించు_CONST, cctx, ఖాళీ);
	    ఒకవేళ (lhs->lhs_scriptvar_idx >= 0)
	    {
		scriptitem_T *si = SCRIPT_అంశం(lhs->lhs_scriptvar_sid);
		svar_T	 *sv = ((svar_T *)si->sn_var_vals.ga_దత్తాంశం)
						+ lhs->lhs_scriptvar_idx;

		lhs->lhs_type = sv->sv_type;
	    }
	}

	తిరిగి OK;
    }

    తిరిగి check_defined(var_ప్రారంభం, lhs->lhs_varlen, cctx, ఖాళీ, FALSE);
}

/*
 * Compile the LHS destination.
 */
    స్థిర పూర్ణాంకం
compile_lhs_var_dest(
    cctx_T	*cctx,
    lhs_T	*lhs,
    పూర్ణాంకం		cmdidx,
    char_u	*var_ప్రారంభం,
    char_u	*var_చివర,
    పూర్ణాంకం		is_decl,
    పూర్ణాంకం		has_cmd)	// "var" before "var_start"
{
    పూర్ణాంకం	    declare_error = FALSE;

    ఒకవేళ (get_var_dest(lhs->lhs_name, &lhs->lhs_dest, cmdidx,
				lhs->lhs_has_సూచీ,
				&lhs->lhs_opt_flags, &lhs->lhs_vimvaridx,
				&lhs->lhs_type, cctx) == FAIL)
	తిరిగి FAIL;

    ఒకవేళ (lhs->lhs_dest != dest_local && cmdidx != CMD_const
						&& cmdidx != CMD_final)
    {
	// Specific kind of variable recognized.
	declare_error = is_decl;
    }
    లేకపోతే
    {
	class_T	*defcl;

	// No specific kind of variable recognized, just a name.
	ఒకవేళ (check_reserved_name(lhs->lhs_name, lhs->lhs_has_సూచీ
						&& *var_చివర == '.') == FAIL)
	    తిరిగి FAIL;

	ఒకవేళ (lookup_local(var_ప్రారంభం, lhs->lhs_varlen, &lhs->lhs_local_lvar,
								cctx) == OK)
	{
	    lhs->lhs_lvar = &lhs->lhs_local_lvar;
	}
	లేకపోతే
	{
	    CLEAR_క్షేత్రం(lhs->lhs_arg_lvar);
	    ఒకవేళ (arg_exists(var_ప్రారంభం, lhs->lhs_varlen,
			&lhs->lhs_arg_lvar.lv_idx, &lhs->lhs_arg_lvar.lv_type,
			&lhs->lhs_arg_lvar.lv_from_outer, cctx) == OK)
	    {
		ఒకవేళ (is_decl)
		{
		    semsg(_(e_str_is_వాడిన_as_వాదం), lhs->lhs_name);
		    తిరిగి FAIL;
		}
		lhs->lhs_lvar = &lhs->lhs_arg_lvar;
	    }
	}

	ఒకవేళ (lhs->lhs_lvar != ఖాళీ)
	{
	    ఒకవేళ (is_decl)
	    {
		// if we come here with what looks like an assignment like
		// .= but which has been rejected by assignment_len() from
		// may_compile_assignment give a better error message
		char_u *p = skipwhite(lhs->lhs_చివర);
		ఒకవేళ (p[0] == '.' && p[1] == '=')
		    emsg(_(e_dot_equal_not_supported_with_script_సంస్కరణ_two));
		లేకపోతే ఒకవేళ (p[0] == ':' && !has_cmd)
		    // type specified in an assignment without "var"
		    semsg(_(e_trailing_characters_str), p);
		లేకపోతే
		    semsg(_(e_variable_already_declared_str), lhs->lhs_name);
		తిరిగి FAIL;
	    }
	}
	లేకపోతే ఒకవేళ ((lhs->lhs_classmember_idx = cctx_class_member_idx(
			cctx, var_ప్రారంభం, lhs->lhs_varlen, &defcl)) >= 0)
	{
	    ఒకవేళ (compile_lhs_class_variable(cctx, lhs, defcl, is_decl)
		    == FAIL)
		తిరిగి FAIL;
	}
	లేకపోతే
	{
	    ఒకవేళ (compile_lhs_script_var(cctx, lhs, var_ప్రారంభం, var_చివర,
			is_decl) == FAIL)
		తిరిగి FAIL;
	}
    }

    ఒకవేళ (declare_error)
    {
	vim9_declare_error(lhs->lhs_name);
	తిరిగి FAIL;
    }

    తిరిగి OK;
}

/*
 * When compiling a LHS variable name, for a class or an object, set the LHS
 * member type.
 */
    స్థిర పూర్ణాంకం
compile_lhs_సమితి_oc_member_type(
    cctx_T	*cctx,
    lhs_T	*lhs,
    char_u	*var_ప్రారంభం)
{
    class_T	*cl = lhs->lhs_type->tt_class;
    పూర్ణాంకం		is_object = lhs->lhs_type->tt_type == VAR_OBJECT;
    char_u	*name = var_ప్రారంభం + lhs->lhs_varlen + 1;
    size_t	namelen = lhs->lhs_చివర - var_ప్రారంభం - lhs->lhs_varlen - 1;

    ocmember_T	*m = member_lookup(cl, lhs->lhs_type->tt_type,
	    name, namelen, &lhs->lhs_member_idx);
    ఒకవేళ (m == ఖాళీ)
    {
	member_not_దొరికింది_సందేశం(cl, lhs->lhs_type->tt_type, name, namelen);
	తిరిగి FAIL;
    }

    ఒకవేళ (IS_ENUM(cl))
    {
	ఒకవేళ (!inside_class(cctx, cl))
	{
	    semsg(_(e_enumvalue_str_cannot_be_modified),
		    cl->class_name.string, m->ocm_name.string);
	    తిరిగి FAIL;
	}
	ఒకవేళ (lhs->lhs_type->tt_type == VAR_OBJECT &&
		lhs->lhs_member_idx < 2)
	{
	    అక్షరం *సందేశం = lhs->lhs_member_idx == 0 ?
		e_enum_str_name_cannot_be_modified :
		e_enum_str_ordinal_cannot_be_modified;
	    semsg(_(సందేశం), cl->class_name.string);
	    తిరిగి FAIL;
	}
    }

    // If it is private member variable, then accessing it outside the
    // class is not allowed.
    // If it is a read only class variable, then it can be modified
    // only inside the class where it is defined.
    ఒకవేళ ((m->ocm_access != VIM_ACCESS_ALL) &&
	    ((is_object && !inside_class(cctx, cl))
	     || (!is_object && cctx->ctx_ufunc->uf_defclass != cl)))
    {
	అక్షరం *సందేశం = (m->ocm_access == VIM_ACCESS_వ్యక్తిగత)
	    ? e_cannot_access_protected_variable_str
	    : e_variable_is_not_writable_str;
	emsg_var_cl_define(సందేశం, m->ocm_name.string, 0, cl);
	తిరిగి FAIL;
    }

    ఒకవేళ (!IS_CONSTRUCTOR_పద్ధతి(cctx->ctx_ufunc)
	    && oc_var_check_ro(cl, m))
	తిరిగి FAIL;

    lhs->lhs_member_type = m->ocm_type;

    తిరిగి OK;
}

/*
 * When compiling a LHS variable, set the LHS variable type.
 */
    స్థిర పూర్ణాంకం
compile_lhs_సమితి_type(cctx_T *cctx, lhs_T *lhs, char_u *var_చివర, పూర్ణాంకం is_decl)
{
    ఒకవేళ (is_decl && *skipwhite(var_చివర) == ':')
    {
	char_u *p;

	// parse optional type: "let var: type = expr"
	ఒకవేళ (VIM_ISWHITE(*var_చివర))
	{
	    semsg(_(e_no_white_space_allowed_before_colon_str), var_చివర);
	    తిరిగి FAIL;
	}

	ఒకవేళ (!VIM_ISWHITE(var_చివర[1]))
	{
	    semsg(_(e_white_space_required_after_str_str), ":", var_చివర);
	    తిరిగి FAIL;
	}

	p = skipwhite(var_చివర + 1);
	lhs->lhs_type = parse_type(&p, cctx->ctx_type_list, cctx->ctx_ufunc, cctx, TRUE);
	ఒకవేళ (lhs->lhs_type == ఖాళీ
		|| !valid_declaration_type(lhs->lhs_type))
	    తిరిగి FAIL;

	lhs->lhs_has_type = TRUE;
	lhs->lhs_చివర = p;
    }
    లేకపోతే ఒకవేళ (lhs->lhs_lvar != ఖాళీ)
	lhs->lhs_type = lhs->lhs_lvar->lv_type;

    తిరిగి OK;
}

/*
 * Returns TRUE if "lhs" is a concatenable string.
 */
    స్థిర పూర్ణాంకం
lhs_concatenable(lhs_T *lhs)
{
    తిరిగి lhs->lhs_dest == dest_global
		|| lhs->lhs_has_సూచీ
		|| lhs->lhs_type->tt_type == VAR_STRING
		|| lhs->lhs_type->tt_type == VAR_ANY;
}

/*
 * Create a new local variable when compiling a LHS variable.
 */
    స్థిర పూర్ణాంకం
compile_lhs_new_local_var(
    cctx_T	*cctx,
    lhs_T	*lhs,
    char_u	*var_ప్రారంభం,
    పూర్ణాంకం		cmdidx,
    పూర్ణాంకం		oplen,
    పూర్ణాంకం		is_decl,
    పూర్ణాంకం		has_cmd,
    పూర్ణాంకం		heredoc)
{
    ఒకవేళ (oplen > 1 && !heredoc)
    {
	// +=, /=, etc. require an existing variable
	semsg(_(e_cannot_use_operator_on_new_variable_str), lhs->lhs_name);
	తిరిగి FAIL;
    }

    ఒకవేళ (!is_decl || (lhs->lhs_has_సూచీ && !has_cmd
					&& cctx->ctx_skip != SKIP_YES))
    {
	semsg(_(e_unknown_variable_str), lhs->lhs_name);
	తిరిగి FAIL;
    }

    // Check the name is valid for a funcref.
    ఒకవేళ (lhs->lhs_type->tt_type == VAR_FUNC
				|| lhs->lhs_type->tt_type == VAR_PARTIAL)
    {
	ఒకవేళ (var_wrong_func_name(lhs->lhs_name, TRUE))
	    తిరిగి FAIL;
    }

    // New local variable.
    పూర్ణాంకం కేటాయించు;
    ఎంపిక (cmdidx)
    {
	స్థితి CMD_final:
	    కేటాయించు = కేటాయించు_FINAL; ఆపు;
	స్థితి CMD_const:
	    కేటాయించు = కేటాయించు_CONST; ఆపు;
	అప్రమేయ:
	    కేటాయించు = కేటాయించు_VAR; ఆపు;
    }

    lhs->lhs_lvar = reserve_local(cctx, var_ప్రారంభం, lhs->lhs_varlen, కేటాయించు,
							lhs->lhs_type);
    ఒకవేళ (lhs->lhs_lvar == ఖాళీ)
	తిరిగి FAIL;

    lhs->lhs_new_local = TRUE;

    తిరిగి OK;
}

/*
 * When compiling a LHS variable name, set the LHS member type.
 */
    స్థిర పూర్ణాంకం
compile_lhs_సమితి_member_type(
    cctx_T	*cctx,
    lhs_T	*lhs,
    char_u	*var_ప్రారంభం,
    పూర్ణాంకం		is_decl,
    పూర్ణాంకం		has_cmd)
{
    lhs->lhs_member_type = lhs->lhs_type;

    ఒకవేళ (!lhs->lhs_has_సూచీ)
	తిరిగి OK;

    char_u	*after = var_ప్రారంభం + lhs->lhs_varlen;
    char_u	*p;

    // Something follows after the variable: "var[idx]" or "var.key".
    ఒకవేళ (is_decl && cctx->ctx_skip != SKIP_YES)
    {
	ఒకవేళ (has_cmd)
	    emsg(_(e_cannot_use_సూచీ_when_declaring_variable));
	లేకపోతే
	    semsg(_(e_unknown_variable_str), lhs->lhs_name);
	తిరిగి FAIL;
    }

    // Now: var_start[lhs->lhs_varlen] is '[' or '.'
    // Only the last index is used below, if there are others
    // before it generate code for the expression.  Thus for
    // "ll[1][2]" the expression is "ll[1]" and "[2]" is the index.
    కొరకు (;;)
    {
	p = skip_సూచీ(after);
	ఒకవేళ (*p != '[' && *p != '.')
	{
	    lhs->lhs_varlen_మొత్తం = p - var_ప్రారంభం;
	    ఆపు;
	}
	after = p;
    }
    ఒకవేళ (after > var_ప్రారంభం + lhs->lhs_varlen)
    {
	lhs->lhs_varlen = after - var_ప్రారంభం;
	lhs->lhs_dest = dest_సమాసం;
	// We don't know the type before evaluating the expression,
	// use "any" until then.  The member index is for the first name,
	// not for the last index.
	lhs->lhs_type = &t_any;
	lhs->lhs_member_idx = -1;
    }

    పూర్ణాంకం use_class = lhs->lhs_type != ఖాళీ
	&& (lhs->lhs_type->tt_type == VAR_CLASS
		|| lhs->lhs_type->tt_type == VAR_OBJECT);

    ఒకవేళ (lhs->lhs_type == ఖాళీ
	    || (use_class ? lhs->lhs_type->tt_class == ఖాళీ
		: lhs->lhs_type->tt_member == ఖాళీ))
    {
	lhs->lhs_member_type = &t_any;
    }
    లేకపోతే ఒకవేళ (use_class)
    {
	// for an object or class member get the type of the member
	ఒకవేళ (compile_lhs_సమితి_oc_member_type(cctx, lhs, var_ప్రారంభం) == FAIL)
	    తిరిగి FAIL;
    }
    లేకపోతే
	lhs->lhs_member_type = lhs->lhs_type->tt_member;

    తిరిగి OK;
}

/*
 * Figure out the LHS type and other properties for an assignment or one item
 * of ":unlet" with an index.
 * Returns OK or FAIL.
 */
    పూర్ణాంకం
compile_lhs(
	char_u	    *var_ప్రారంభం,
	lhs_T	    *lhs,
	cmdidx_T    cmdidx,
	పూర్ణాంకం	    heredoc,
	పూర్ణాంకం	    has_cmd,	    // "var" before "var_start"
	పూర్ణాంకం	    oplen,
	cctx_T	    *cctx)
{
    char_u	*var_చివర = ఖాళీ;
    పూర్ణాంకం		is_decl = is_decl_command(cmdidx);

    ఒకవేళ (lhs_init(lhs, var_ప్రారంభం, is_decl, heredoc, &var_చివర) == FAIL)
	తిరిగి FAIL;

    ఒకవేళ (cctx->ctx_skip != SKIP_YES)
    {
	// compile the LHS destination
	ఒకవేళ (compile_lhs_var_dest(cctx, lhs, cmdidx, var_ప్రారంభం, var_చివర,
						is_decl, has_cmd) == FAIL)
	    తిరిగి FAIL;
    }

    // handle "a:name" as a name, not index "name" in "a"
    ఒకవేళ (lhs->lhs_varlen > 1 || var_ప్రారంభం[lhs->lhs_varlen] != ':')
	var_చివర = lhs->lhs_dest_చివర;

    ఒకవేళ (lhs->lhs_dest != dest_option && lhs->lhs_dest != dest_func_option)
    {
	// set the LHS variable type
	ఒకవేళ (compile_lhs_సమితి_type(cctx, lhs, var_చివర, is_decl) == FAIL)
	    తిరిగి FAIL;
    }

    ఒకవేళ (oplen == 3 && !heredoc && !lhs_concatenable(lhs))
    {
	emsg(_(e_can_only_concatenate_గమ్యం_string));
	తిరిగి FAIL;
    }

    ఒకవేళ (lhs->lhs_lvar == ఖాళీ && lhs->lhs_dest == dest_local
						&& cctx->ctx_skip != SKIP_YES)
    {
	ఒకవేళ (compile_lhs_new_local_var(cctx, lhs, var_ప్రారంభం, cmdidx, oplen,
					is_decl, has_cmd, heredoc) == FAIL)
	    తిరిగి FAIL;
    }

    ఒకవేళ (compile_lhs_సమితి_member_type(cctx, lhs, var_ప్రారంభం, is_decl, has_cmd)
								== FAIL)
	తిరిగి FAIL;

    తిరిగి OK;
}

/*
 * Figure out the LHS and check a few errors.
 */
    పూర్ణాంకం
compile_కేటాయించు_lhs(
	char_u	    *var_ప్రారంభం,
	lhs_T	    *lhs,
	cmdidx_T    cmdidx,
	పూర్ణాంకం	    is_decl,
	పూర్ణాంకం	    heredoc,
	పూర్ణాంకం	    has_cmd,	    // "var" before "var_start"
	పూర్ణాంకం	    oplen,
	cctx_T	    *cctx)
{
    ఒకవేళ (compile_lhs(var_ప్రారంభం, lhs, cmdidx, heredoc, has_cmd, oplen, cctx)
								       == FAIL)
	తిరిగి FAIL;

    ఒకవేళ (!lhs->lhs_has_సూచీ && lhs->lhs_lvar == &lhs->lhs_arg_lvar)
    {
	semsg(_(e_cannot_కేటాయించు_గమ్యం_వాదం_str), lhs->lhs_name);
	తిరిగి FAIL;
    }
    ఒకవేళ (!is_decl && lhs->lhs_lvar != ఖాళీ
			   && lhs->lhs_lvar->lv_const != కేటాయించు_VAR
			   && !lhs->lhs_has_సూచీ)
    {
	semsg(_(e_cannot_కేటాయించు_గమ్యం_constant_str), lhs->lhs_name);
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * Return TRUE if "lhs" has a range index: "[expr : expr]".
 */
    స్థిర పూర్ణాంకం
has_list_సూచీ(char_u *idx_ప్రారంభం, cctx_T *cctx)
{
    char_u  *p = idx_ప్రారంభం;
    పూర్ణాంకం	    save_skip;

    ఒకవేళ (*p != '[')
	తిరిగి FALSE;

    p = skipwhite(p + 1);
    ఒకవేళ (*p == ':')
	తిరిగి TRUE;

    save_skip = cctx->ctx_skip;
    cctx->ctx_skip = SKIP_YES;
    (శూన్య)compile_expr0(&p, cctx);
    cctx->ctx_skip = save_skip;
    తిరిగి *skipwhite(p) == ':';
}

/*
 * For an assignment with an index, compile the "idx" in "var[idx]" or "key" in
 * "var.key".
 */
    స్థిర పూర్ణాంకం
compile_కేటాయించు_సూచీ(
	char_u	*var_ప్రారంభం,
	lhs_T	*lhs,
	పూర్ణాంకం	*range,
	cctx_T	*cctx)
{
    size_t	varlen = lhs->lhs_varlen;
    char_u	*p;
    పూర్ణాంకం		r = OK;
    పూర్ణాంకం		need_white_before = TRUE;
    పూర్ణాంకం		ఖాళీలు_రెండవ;

    p = var_ప్రారంభం + varlen;
    ఒకవేళ (*p == '[')
    {
	p = skipwhite(p + 1);
	ఒకవేళ (*p == ':')
	{
	    // empty first index, push zero
	    r = generate_PUSHNR(cctx, 0);
	    need_white_before = FALSE;
	}
	లేకపోతే
	    r = compile_expr0(&p, cctx);

	ఒకవేళ (r == OK && *skipwhite(p) == ':')
	{
	    // unlet var[idx : idx]
	    // blob[idx : idx] = value
	    *range = TRUE;
	    p = skipwhite(p);
	    ఖాళీలు_రెండవ = *skipwhite(p + 1) == ']';
	    ఒకవేళ ((need_white_before && !IS_WHITE_OR_NUL(p[-1]))
		    || (!ఖాళీలు_రెండవ && !IS_WHITE_OR_NUL(p[1])))
	    {
		semsg(_(e_white_space_required_before_and_after_str_at_str),
								      ":", p);
		తిరిగి FAIL;
	    }
	    p = skipwhite(p + 1);
	    ఒకవేళ (*p == ']')
		// empty second index, push "none"
		r = generate_PUSHSPEC(cctx, VVAL_NONE);
	    లేకపోతే
		r = compile_expr0(&p, cctx);
	}

	ఒకవేళ (r == OK && *skipwhite(p) != ']')
	{
	    // this should not happen
	    emsg(_(e_missing_closing_వర్గం_brace));
	    r = FAIL;
	}
    }
    లేకపోతే ఒకవేళ (lhs->lhs_member_idx >= 0)
    {
	// object member index
	r = generate_PUSHNR(cctx, lhs->lhs_member_idx);
    }
    లేకపోతే // if (*p == '.')
    {
	char_u *కీలకం_చివర = గమ్యం_name_చివర(p + 1, TRUE);
	char_u *కీలకం = vim_strnsave(p + 1, కీలకం_చివర - p - 1);

	r = generate_PUSHS(cctx, &కీలకం);
    }
    తిరిగి r;
}

/*
 * For a LHS with an index, load the variable to be indexed.
 */
    స్థిర పూర్ణాంకం
compile_ఎక్కించు_lhs(
	lhs_T	*lhs,
	char_u	*var_ప్రారంభం,
	type_T	*rhs_type,
	cctx_T	*cctx)
{
    ఒకవేళ (lhs->lhs_dest == dest_సమాసం)
    {
	size_t	    varlen = lhs->lhs_varlen;
	పూర్ణాంకం	    c = var_ప్రారంభం[varlen];
	పూర్ణాంకం	    పంక్తులు_len = cctx->ctx_ufunc->uf_పంక్తులు.ga_len;
	పూర్ణాంకం	    res;

	// Evaluate "ll[expr]" of "ll[expr][idx]".  End the line with a NUL and
	// limit the lines array length to avoid skipping to a following line.
	var_ప్రారంభం[varlen] = NUL;
	cctx->ctx_ufunc->uf_పంక్తులు.ga_len = cctx->ctx_lnum + 1;
	char_u *p = var_ప్రారంభం;
	res = compile_expr0(&p, cctx);
	var_ప్రారంభం[varlen] = c;
	cctx->ctx_ufunc->uf_పంక్తులు.ga_len = పంక్తులు_len;
	ఒకవేళ (res == FAIL || p != var_ప్రారంభం + varlen)
	{
	    // this should not happen
	    ఒకవేళ (res != FAIL)
		emsg(_(e_missing_closing_వర్గం_brace));
	    తిరిగి FAIL;
	}

	lhs->lhs_type = cctx->ctx_type_దొంతర.ga_len == 0 ? &t_void
						  : get_type_on_దొంతర(cctx, 0);

	ఒకవేళ (lhs->lhs_type->tt_type == VAR_CLASS
		|| (lhs->lhs_type->tt_type == VAR_OBJECT
		    && lhs->lhs_type != &t_object_any))
	{
	    // Check whether the class or object variable is modifiable
	    ఒకవేళ (!lhs_class_member_modifiable(lhs, var_ప్రారంభం, cctx))
		తిరిగి FAIL;
	}

	// Now we can properly check the type.  The variable is indexed, thus
	// we need the member type.  For a class or object we don't know the
	// type yet, it depends on what member is used.
	// The top item in the stack is the Dict, followed by the key and then
	// the type of the value.
	vartype_T vartype = lhs->lhs_type->tt_type;
	type_T *member_type = lhs->lhs_type->tt_member;
	ఒకవేళ (rhs_type != ఖాళీ && member_type != ఖాళీ
		&& vartype != VAR_OBJECT && vartype != VAR_CLASS
		&& rhs_type != &t_void
		&& need_type(rhs_type, member_type, 0,
					    -3, 0, cctx, FALSE, FALSE) == FAIL)
	    తిరిగి FAIL;

	తిరిగి OK;
    }

    తిరిగి generate_loadvar(cctx, lhs);
}

/*
 * Produce code for loading "lhs" and also take care of an index.
 * Return OK/FAIL.
 */
    పూర్ణాంకం
compile_ఎక్కించు_lhs_with_సూచీ(lhs_T *lhs, char_u *var_ప్రారంభం, cctx_T *cctx)
{
    ఒకవేళ (lhs->lhs_type->tt_type == VAR_OBJECT)
    {
	// "this.value": load "this" object and get the value at index for an
	// object or class member get the type of the member.
	// Also for "obj.value".
	char_u *dot = vim_strchr(var_ప్రారంభం, '.');
	ఒకవేళ (dot == ఖాళీ)
	{
	    semsg(_(e_missing_dot_after_object_str), lhs->lhs_name);
	    తిరిగి FAIL;
	}

	class_T	*cl = lhs->lhs_type->tt_class;
	type_T	*type = oc_member_type(cl, TRUE, dot + 1,
					  lhs->lhs_చివర, &lhs->lhs_member_idx);
	ఒకవేళ (lhs->lhs_member_idx < 0)
	    తిరిగి FAIL;

	ఒకవేళ (dot - var_ప్రారంభం == 4 && STRNCMP(var_ప్రారంభం, "this", 4) == 0)
	{
	    // load "this"
	    lvar_T  *lvar = lhs->lhs_lvar;
	    పూర్ణాంకం	    rc;

	    ఒకవేళ (lvar->lv_from_outer > 0)
		rc = generate_LOADOUTER(cctx, lvar->lv_idx,
			lvar->lv_from_outer, lvar->lv_loop_లోతు,
			lvar->lv_loop_idx, type);
	    లేకపోతే
		rc = generate_ఎక్కించు(cctx, ISN_ఎక్కించు, lvar->lv_idx, ఖాళీ, type);

	    ఒకవేళ (rc == FAIL)
		తిరిగి FAIL;
	}
	లేకపోతే
	{
	    // load object variable or argument
	    ఒకవేళ (compile_ఎక్కించు_lhs(lhs, var_ప్రారంభం, lhs->lhs_type, cctx) == FAIL)
		తిరిగి FAIL;
	}
	ఒకవేళ (IS_INTERFACE(cl))
	    తిరిగి generate_GET_ITF_MEMBER(cctx, cl, lhs->lhs_member_idx, type);
	తిరిగి generate_GET_OBJ_MEMBER(cctx, lhs->lhs_member_idx, type);
    }
    లేకపోతే ఒకవేళ (lhs->lhs_type->tt_type == VAR_CLASS)
    {
	// "<classname>.value": load class variable "classname.value"
	char_u *dot = vim_strchr(var_ప్రారంభం, '.');
	ఒకవేళ (dot == ఖాళీ)
	{
	    (శూన్య)check_type_is_విలువ(lhs->lhs_type);
	    తిరిగి FAIL;
	}

	class_T	*cl = lhs->lhs_type->tt_class;
	ocmember_T *m = class_member_lookup(cl, dot + 1,
						lhs->lhs_చివర - dot - 1,
						&lhs->lhs_member_idx);
	ఒకవేళ (m == ఖాళీ)
	    తిరిగి FAIL;

	తిరిగి generate_CLASSMEMBER(cctx, TRUE, cl, lhs->lhs_member_idx);
    }

    ఒకవేళ (compile_ఎక్కించు_lhs(lhs, var_ప్రారంభం, ఖాళీ, cctx) == FAIL)
	తిరిగి FAIL;

    ఒకవేళ (lhs->lhs_has_సూచీ)
    {
	పూర్ణాంకం	range = FALSE;
	type_T	*type = get_type_on_దొంతర(cctx, 0);

	// A member of an object or class is not obtained by indexing it.
	ఒకవేళ (type->tt_type == VAR_CLASS
		|| (type->tt_type == VAR_OBJECT && type != &t_object_any))
	{
	    char_u *p = var_ప్రారంభం + lhs->lhs_varlen;

	    తిరిగి compile_class_object_సూచీ(cctx, &p, type);
	}

	// Get member from list or dict.  First compile the
	// index value.
	ఒకవేళ (compile_కేటాయించు_సూచీ(var_ప్రారంభం, lhs, &range, cctx) == FAIL)
	    తిరిగి FAIL;
	ఒకవేళ (range)
	{
	    semsg(_(e_cannot_use_range_with_assignment_operator_str),
								    var_ప్రారంభం);
	    తిరిగి FAIL;
	}

	// Get the member.
	ఒకవేళ (compile_member(FALSE, ఖాళీ, cctx) == FAIL)
	    తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * Assignment to a list or dict member, or ":unlet" for the item, using the
 * information in "lhs".
 * Returns OK or FAIL.
 */
    పూర్ణాంకం
compile_కేటాయించు_unlet(
    char_u	*var_ప్రారంభం,
    lhs_T	*lhs,
    పూర్ణాంకం		is_కేటాయించు,
    type_T	*rhs_type,
    cctx_T	*cctx)
{
    vartype_T	dest_type;
    పూర్ణాంకం		range = FALSE;

    ఒకవేళ (compile_కేటాయించు_సూచీ(var_ప్రారంభం, lhs, &range, cctx) == FAIL)
	తిరిగి FAIL;
    ఒకవేళ (is_కేటాయించు && range
	    && lhs->lhs_type->tt_type != VAR_LIST
	    && lhs->lhs_type != &t_blob
	    && lhs->lhs_type != &t_any)
    {
	ఒకవేళ (lhs->lhs_type->tt_type == VAR_TUPLE)
	    emsg(_(e_cannot_ముక్క_tuple));
	లేకపోతే
	    semsg(_(e_cannot_use_range_with_assignment_str), var_ప్రారంభం);
	తిరిగి FAIL;
    }

    // For "expr[idx]" the index is compiled before the expression, thus the
    // resulting type cannot be used here.
    ఒకవేళ (lhs->lhs_dest == dest_సమాసం
	    || lhs->lhs_type == ఖాళీ || lhs->lhs_type == &t_any)
    {
	// Index on variable of unknown type: check at runtime.
	dest_type = VAR_ANY;
    }
    లేకపోతే
    {
	dest_type = lhs->lhs_type->tt_type;
	ఒకవేళ (dest_type == VAR_DICT && range)
	{
	    emsg(_(e_cannot_use_range_with_dictionary));
	    తిరిగి FAIL;
	}
	ఒకవేళ (dest_type == VAR_DICT
		&& may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
	    తిరిగి FAIL;
	ఒకవేళ (dest_type == VAR_LIST || dest_type == VAR_BLOB)
	{
	    type_T *type;

	    ఒకవేళ (range)
	    {
		type = get_type_on_దొంతర(cctx, 1);
		ఒకవేళ (need_type(type, &t_సంఖ్య, 0,
					    -2, 0, cctx, FALSE, FALSE) == FAIL)
		తిరిగి FAIL;
	    }
	    type = get_type_on_దొంతర(cctx, 0);
	    ఒకవేళ ((dest_type != VAR_BLOB && type->tt_type != VAR_SPECIAL)
		    && need_type(type, &t_సంఖ్య, 0,
					    -1, 0, cctx, FALSE, FALSE) == FAIL)
		తిరిగి FAIL;
	}
    }

    ఒకవేళ (cctx->ctx_skip == SKIP_YES)
	తిరిగి OK;

    // Load the dict, list or object.  On the stack we then have:
    // - value (for assignment, not for :unlet)
    // - index
    // - for [a : b] second index
    // - variable
    ఒకవేళ (compile_ఎక్కించు_lhs(lhs, var_ప్రారంభం, rhs_type, cctx) == FAIL)
	తిరిగి FAIL;

    ఒకవేళ (dest_type == VAR_LIST
	    || dest_type == VAR_DICT
	    || dest_type == VAR_BLOB
	    || dest_type == VAR_CLASS
	    || dest_type == VAR_OBJECT
	    || dest_type == VAR_ANY)
    {
	ఒకవేళ (is_కేటాయించు)
	{
	    ఒకవేళ (range)
	    {
		ఒకవేళ (generate_instr_drop(cctx, ISN_STORERANGE, 4) == ఖాళీ)
		    తిరిగి FAIL;
	    }
	    లేకపోతే
	    {
		isn_T	*isn = generate_instr_drop(cctx, ISN_STOREINDEX, 3);

		ఒకవేళ (isn == ఖాళీ)
		    తిరిగి FAIL;
		isn->isn_arg.storeindex.si_vartype = dest_type;
		isn->isn_arg.storeindex.si_class = ఖాళీ;

		ఒకవేళ (dest_type == VAR_OBJECT)
		{
		    class_T *cl = lhs->lhs_type->tt_class;

		    ఒకవేళ (IS_INTERFACE(cl))
		    {
			// "this.value": load "this" object and get the value
			// at index for an object or class member get the type
			// of the member
			isn->isn_arg.storeindex.si_class = cl;
			++cl->class_refcount;
		    }
		}
	    }
	}
	లేకపోతే ఒకవేళ (range)
	{
	    ఒకవేళ (generate_instr_drop(cctx, ISN_UNLETRANGE, 3) == ఖాళీ)
		తిరిగి FAIL;
	}
	లేకపోతే
	{
	    ఒకవేళ (generate_instr_drop(cctx, ISN_UNLETINDEX, 2) == ఖాళీ)
		తిరిగి FAIL;
	}
    }
    లేకపోతే
    {
	ఒకవేళ (dest_type == VAR_TUPLE)
	    emsg(_(e_tuple_is_immutable));
	లేకపోతే
	    emsg(_(e_indexable_type_required));
	తిరిగి FAIL;
    }

    తిరిగి OK;
}

/*
 * Generate an instruction to push the default value for "vartype".
 * if "dest_local" is TRUE then for some types no instruction is generated.
 * "skip_store" is set to TRUE if no PUSH instruction is generated.
 * Returns OK or FAIL.
 */
    స్థిర పూర్ణాంకం
నెట్టు_default_విలువ(
	cctx_T	    *cctx,
	vartype_T   vartype,
	పూర్ణాంకం	    dest_is_local,
	పూర్ణాంకం	    *skip_store)
{
    పూర్ణాంకం r = OK;

    ఎంపిక (vartype)
    {
	స్థితి VAR_BOOL:
	    r = generate_PUSHBOOL(cctx, VVAL_FALSE);
	    ఆపు;
	స్థితి VAR_FLOAT:
	    r = generate_PUSHF(cctx, 0.0);
	    ఆపు;
	స్థితి VAR_STRING:
	    r = generate_PUSHS(cctx, ఖాళీ);
	    ఆపు;
	స్థితి VAR_BLOB:
	    r = generate_PUSHBLOB(cctx, blob_alloc());
	    ఆపు;
	స్థితి VAR_FUNC:
	    r = generate_PUSHFUNC(cctx, ఖాళీ, &t_func_void, TRUE);
	    ఆపు;
	స్థితి VAR_LIST:
	    r = generate_NEWLIST(cctx, 0, FALSE);
	    ఆపు;
	స్థితి VAR_TUPLE:
	    r = generate_NEWTUPLE(cctx, 0, FALSE);
	    ఆపు;
	స్థితి VAR_DICT:
	    r = generate_NEWDICT(cctx, 0, FALSE);
	    ఆపు;
	స్థితి VAR_పని:
	    r = generate_PUSHJOB(cctx);
	    ఆపు;
	స్థితి VAR_CHANNEL:
	    r = generate_PUSHCHANNEL(cctx);
	    ఆపు;
	స్థితి VAR_OBJECT:
	    r = generate_PUSHOBJ(cctx);
	    ఆపు;
	స్థితి VAR_సంఖ్య:
	స్థితి VAR_UNKNOWN:
	స్థితి VAR_ANY:
	స్థితి VAR_PARTIAL:
	స్థితి VAR_VOID:
	స్థితి VAR_INSTR:
	స్థితి VAR_CLASS:
	స్థితి VAR_TYPEALIAS:
	స్థితి VAR_SPECIAL:  // cannot happen
	    // This is skipped for local variables, they are always
	    // initialized to zero.  But in a "for" or "while" loop
	    // the value may have been changed.
	    ఒకవేళ (dest_is_local && !inside_loop_scope(cctx))
		*skip_store = TRUE;
	    లేకపోతే
		r = generate_PUSHNR(cctx, 0);
	    ఆపు;
    }
    తిరిగి r;
}

/*
 * Compile assignment context.  Used when compiling an assignment statement.
 */
రకనిర్వచనం నిర్మాణం cac_S cac_T;
నిర్మాణం cac_S
{
    cmdidx_T	cac_cmdidx;		// assignment command
    char_u	*cac_nextc;		// next character to parse
    lhs_T	cac_lhs;		// lhs of the assignment
    type_T	*cac_rhs_type;		// rhs type of an assignment
    char_u	*cac_op;		// assignment operator
    పూర్ణాంకం		cac_oplen;		// assignment operator length
    char_u	*cac_var_ప్రారంభం;		// start of the variable names
    char_u	*cac_var_చివర;		// end of the variable names
    పూర్ణాంకం		cac_var_లెక్క;		// number of variables in assignment
    పూర్ణాంకం		cac_var_idx;		// variable index in a list
    పూర్ణాంకం		cac_semicolon;		// semicolon in [var1, var2; var3]
    garray_T	*cac_instr;
    పూర్ణాంకం		cac_instr_లెక్క;
    పూర్ణాంకం		cac_incdec;
    పూర్ణాంకం		cac_did_generate_ముక్క;
    పూర్ణాంకం		cac_is_decl;
    పూర్ణాంకం		cac_is_const;
    పూర్ణాంకం		cac_ప్రారంభం_lnum;
    type_T	*cac_inferred_type;
    పూర్ణాంకం		cac_skip_store;
};

/*
 * Initialize the compile assignment context.
 */
    స్థిర శూన్య
compile_కేటాయించు_context_init(cac_T *cac, cctx_T *cctx, పూర్ణాంకం cmdidx, char_u *arg)
{
    CLEAR_క్షేత్రం(*cac);
    cac->cac_cmdidx = cmdidx;
    cac->cac_instr = &cctx->ctx_instr;
    cac->cac_rhs_type = &t_any;
    cac->cac_is_decl = is_decl_command(cmdidx);
    cac->cac_ప్రారంభం_lnum = SOURCING_LNUM;
    cac->cac_instr_లెక్క = -1;
    cac->cac_var_చివర = arg;
}

/*
 * Compile an object member variable assignment in the arguments passed to a
 * class new() method.
 *
 * Instruction format:
 *
 *	ifargisset <n> this.<varname> = <value>
 *
 * where <n> is the index of the default argument.
 *
 * Generates the ISN_JUMP_IF_ARG_NOT_SET instruction to skip the assignment if
 * the value is passed as an argument to the new() method call.
 *
 * Returns OK on success.
 */
    స్థిర పూర్ణాంకం
compile_కేటాయించు_obj_new_arg(char_u **argp, cctx_T *cctx)
{
    char_u *arg = *argp;

    arg += 11;	    // skip "ifargisset"
    పూర్ణాంకం def_arg_idx = getdigits(&arg);
    arg = skipwhite(arg);

    // Use a JUMP_IF_ARG_NOT_SET instruction to skip if the value was not
    // given and the default value is "v:none".
    పూర్ణాంకం దొంతర_స్థానభ్రంశం = దొంతర_చట్రం_కొలత +
				(cctx->ctx_ufunc->uf_va_name != ఖాళీ ? 1 : 0);
    పూర్ణాంకం def_arg_లెక్క = cctx->ctx_ufunc->uf_def_వాదనలు.ga_len;
    పూర్ణాంకం arg_స్థానభ్రంశం = def_arg_idx - def_arg_లెక్క - దొంతర_స్థానభ్రంశం;

    ఒకవేళ (generate_JUMP_IF_ARG(cctx, ISN_JUMP_IF_ARG_NOT_సమితి,
			     arg_స్థానభ్రంశం) == FAIL)
	తిరిగి FAIL;

    *argp = arg;
    తిరిగి OK;
}

/*
 * Translate the increment (++) and decrement (--) operators to the
 * corresponding compound operators (+= or -=).
 *
 * Returns OK on success and FAIL on syntax error.
 */
    స్థిర పూర్ణాంకం
translate_incdec_op(exarg_T *eap, cac_T *cac)
{
    ఒకవేళ (VIM_ISWHITE(eap->cmd[2]))
    {
	semsg(_(e_no_white_space_allowed_after_str_str),
		eap->cmdidx == CMD_increment ? "++" : "--", eap->cmd);
	తిరిగి FAIL;
    }
    cac->cac_op = (char_u *)(eap->cmdidx == CMD_increment ? "+=" : "-=");
    cac->cac_oplen = 2;
    cac->cac_incdec = TRUE;

    తిరిగి OK;
}

/*
 * Process the operator in an assignment statement.
 */
    స్థిర పూర్ణాంకం
compile_కేటాయించు_process_operator(
    exarg_T	*eap,
    char_u	*arg,
    cac_T	*cac,
    పూర్ణాంకం		*heredoc,
    char_u	**retstr)
{
    *retstr = ఖాళీ;

    ఒకవేళ (eap->cmdidx == CMD_increment || eap->cmdidx == CMD_decrement)
	// Change an unary operator to a compound operator
	తిరిగి translate_incdec_op(eap, cac);

    char_u *sp = cac->cac_nextc;
    cac->cac_nextc = skipwhite(cac->cac_nextc);
    cac->cac_op = cac->cac_nextc;
    cac->cac_oplen = assignment_len(cac->cac_nextc, heredoc);

    ఒకవేళ (cac->cac_var_లెక్క > 0 && cac->cac_oplen == 0)
    {
	// can be something like "[1, 2]->func()"
	*retstr = arg;
	తిరిగి FAIL;
    }

    // need white space before and after the operator
    ఒకవేళ (cac->cac_oplen > 0 && (!VIM_ISWHITE(*sp)
		|| !IS_WHITE_OR_NUL(cac->cac_op[cac->cac_oplen])))
    {
	error_white_both(cac->cac_op, cac->cac_oplen);
	తిరిగి FAIL;
    }

    తిరిగి OK;
}

/*
 * Find the start of an assignment statement.
 */
    స్థిర char_u *
compile_కేటాయించు_compute_ప్రారంభం(char_u *arg, పూర్ణాంకం var_లెక్క)
{
    ఒకవేళ (var_లెక్క > 0)
	// [var1, var2] = [val1, val2]
	// skip over the "["
	తిరిగి skipwhite(arg + 1);

    తిరిగి arg;
}

/*
 * Parse a heredoc assignment starting at "p".  Returns a pointer to the
 * beginning of the heredoc content.
 */
    స్థిర char_u *
parse_heredoc_assignment(exarg_T *eap, cctx_T *cctx, cac_T *cac)
{
    // [let] varname =<< [trim] {end}
    eap->ea_getline = exarg_getline;
    eap->cookie = cctx;

    list_T *l = heredoc_get(eap, cac->cac_nextc + 3, FALSE, TRUE);
    ఒకవేళ (l == ఖాళీ)
	తిరిగి ఖాళీ;

    list_free(l);
    cac->cac_nextc += STRLEN(cac->cac_nextc);

    తిరిగి cac->cac_nextc;
}

/*
 * Check the type of a RHS expression in a list assignment statement.
 * The RHS expression is already compiled.  So the type is on the stack.
 */
    స్థిర పూర్ణాంకం
compile_కేటాయించు_list_check_rhs_type(cctx_T *cctx, cac_T *cac)
{
    type_T	*stacktype;

    stacktype = cctx->ctx_type_దొంతర.ga_len == 0 ? &t_void
						: get_type_on_దొంతర(cctx, 0);
    ఒకవేళ (stacktype->tt_type == VAR_VOID)
    {
	emsg(_(e_cannot_use_void_విలువ));
	తిరిగి FAIL;
    }

    ఒకవేళ (stacktype->tt_type != VAR_LIST && stacktype->tt_type != VAR_TUPLE
					&& stacktype->tt_type != VAR_ANY)
    {
	emsg(_(e_list_or_tuple_required));
	తిరిగి FAIL;
    }

    ఒకవేళ (need_type(stacktype,
		  stacktype->tt_type == VAR_TUPLE ? &t_tuple_any : &t_list_any,
		  TYPECHK_TUPLE_OK, -1, 0, cctx, FALSE, FALSE) == FAIL)
	తిరిగి FAIL;

    // The check accepts both a list and a tuple. Keep "any", making it a list
    // would reject a tuple later on.
    ఒకవేళ (stacktype->tt_type == VAR_ANY)
	సమితి_type_on_దొంతర(cctx, &t_any, 0);

    ఒకవేళ (stacktype->tt_type == VAR_TUPLE)
    {
	ఒకవేళ (stacktype->tt_argcount != 1)
	    cac->cac_rhs_type = &t_any;
	లేకపోతే
	{
	    ఒకవేళ (stacktype->tt_flags & TTFLAG_VARARGS)
		cac->cac_rhs_type = stacktype->tt_వాదనలు[0]->tt_member;
	    లేకపోతే
		cac->cac_rhs_type = stacktype->tt_వాదనలు[0];
	}
    }
    లేకపోతే ఒకవేళ (stacktype->tt_member != ఖాళీ)
	cac->cac_rhs_type = stacktype->tt_member;

    తిరిగి OK;
}

/*
 * In a list assignment statement, if a constant list was used, check the
 * length.  Returns OK if the length check succeeds.  Returns FAIL otherwise.
 */
    స్థిర పూర్ణాంకం
compile_కేటాయించు_list_check_నిడివి(cctx_T *cctx, cac_T *cac)
{
    పూర్ణాంకం	needed_list_len;
    పూర్ణాంకం	did_check = FALSE;

    needed_list_len = cac->cac_semicolon
				? cac->cac_var_లెక్క - 1
				: cac->cac_var_లెక్క;
    ఒకవేళ (cac->cac_instr->ga_len > 0)
    {
	isn_T	*isn = ((isn_T *)cac->cac_instr->ga_దత్తాంశం) +
						cac->cac_instr->ga_len - 1;

	ఒకవేళ (isn->isn_type == ISN_NEWLIST || isn->isn_type == ISN_NEWTUPLE)
	{
	    did_check = TRUE;
	    ఒకవేళ (cac->cac_semicolon ?
			isn->isn_arg.సంఖ్య < needed_list_len
			: isn->isn_arg.సంఖ్య != needed_list_len)
	    {
		semsg(_(e_expected_nr_వస్తువులు_but_got_nr),
			needed_list_len, (పూర్ణాంకం)isn->isn_arg.సంఖ్య);
		తిరిగి FAIL;
	    }
	}
    }

    ఒకవేళ (!did_check)
	generate_CHECKLEN(cctx, needed_list_len, cac->cac_semicolon);

    తిరిగి OK;
}

/*
 * Evaluate the expression for "[var, var] = expr" assignment.
 * A line break may follow the assignment operator "=".
 */
    స్థిర char_u *
compile_కేటాయించు_list_సమాసం(cctx_T *cctx, cac_T *cac)
{
    char_u *whitep;

    whitep = cac->cac_op + cac->cac_oplen;

    ఒకవేళ (may_get_తదుపరి_పంక్తి_error(whitep, &cac->cac_nextc, cctx) == FAIL)
	తిరిగి ఖాళీ;

    // compile RHS expression
    ఒకవేళ (compile_expr0(&cac->cac_nextc, cctx) == FAIL)
	తిరిగి ఖాళీ;

    ఒకవేళ (cctx->ctx_skip == SKIP_YES)
	// no need to parse more when skipping
	తిరిగి cac->cac_nextc;

    ఒకవేళ (compile_కేటాయించు_list_check_rhs_type(cctx, cac) == FAIL)
	తిరిగి ఖాళీ;

    // If a constant list was used we can check the length right here.
    ఒకవేళ (compile_కేటాయించు_list_check_నిడివి(cctx, cac) == FAIL)
	తిరిగి FAIL;

    తిరిగి cac->cac_nextc;
}

/*
 * Find and return the end of a heredoc or a list of variables assignment
 * statement.  For a single variable assignment statement, returns the current
 * end.
 * Returns NULL on failure.
 */
    స్థిర char_u *
compile_కేటాయించు_compute_చివర(
    exarg_T	*eap,
    cctx_T	*cctx,
    cac_T	*cac,
    పూర్ణాంకం		heredoc)
{
    ఒకవేళ (heredoc)
    {
	cac->cac_nextc = parse_heredoc_assignment(eap, cctx, cac);
	తిరిగి cac->cac_nextc;
    }

    ఒకవేళ (cac->cac_var_లెక్క > 0)
    {
	// for "[var, var] = expr" evaluate the expression. The list of
	// variables are processed later.
	// A line break may follow the "=".
	cac->cac_nextc = compile_కేటాయించు_list_సమాసం(cctx, cac);
	తిరిగి cac->cac_nextc;
    }

    తిరిగి cac->cac_var_చివర;
}

/*
 * For "var = expr" evaluate the expression.
 */
    స్థిర పూర్ణాంకం
compile_కేటాయించు_single_eval_సమాసం(cctx_T *cctx, cac_T *cac)
{
    పూర్ణాంకం		ret = OK;
    char_u	*whitep;
    lhs_T	*lhs = &cac->cac_lhs;
    lvar_T	*lvp;
    lvar_T	save_lhs_lvar;

    // Compile the expression.
    ఒకవేళ (cac->cac_incdec)
	తిరిగి generate_PUSHNR(cctx, 1);

    // Temporarily hide the new local variable here, it is
    // not available to this expression.
    ఒకవేళ (lhs->lhs_new_local)
    {
	--cctx->ctx_locals.ga_len;

	// Save the local variable value (compiling the RHS expression may
	// create new local variables).
	lvp = ((lvar_T *)cctx->ctx_locals.ga_దత్తాంశం) + cctx->ctx_locals.ga_len;
	save_lhs_lvar = *lvp;
    }
    whitep = cac->cac_op + cac->cac_oplen;

    ఒకవేళ (may_get_తదుపరి_పంక్తి_error(whitep, &cac->cac_nextc, cctx) == FAIL)
    {
	ఒకవేళ (lhs->lhs_new_local)
	    ++cctx->ctx_locals.ga_len;
	తిరిగి FAIL;
    }

    ret = compile_expr0_ext(&cac->cac_nextc, cctx, &cac->cac_is_const);
    ఒకవేళ (lhs->lhs_new_local)
    {
	// Restore the local variable value.  Update lhs_lvar as the index of
	// the local variable might have changed.
	lvp = ((lvar_T *)cctx->ctx_locals.ga_దత్తాంశం) + cctx->ctx_locals.ga_len;
	*lvp = save_lhs_lvar;
	lhs->lhs_lvar = lvp;

	++cctx->ctx_locals.ga_len;
    }

    తిరిగి ret;
}

/*
 * When compiling an assignment, set the LHS type to the RHS type.
 */
    స్థిర పూర్ణాంకం
compile_కేటాయించు_సమితి_lhs_type_from_rhs(
    cctx_T	*cctx,
    cac_T	*cac,
    lhs_T	*lhs,
    type_T	*rhs_type)
{
    ఒకవేళ (rhs_type->tt_type == VAR_VOID)
    {
	emsg(_(e_cannot_use_void_విలువ));
	తిరిగి FAIL;
    }

    type_T *type;

    // An empty list or dict has a &t_unknown member, for a variable that
    // implies &t_any.
    ఒకవేళ (rhs_type == &t_list_ఖాళీలు)
	type = &t_list_any;
    లేకపోతే ఒకవేళ (rhs_type == &t_dict_ఖాళీలు)
	type = &t_dict_any;
    లేకపోతే ఒకవేళ (rhs_type == &t_unknown)
	type = &t_any;
    లేకపోతే
    {
	type = rhs_type;
	cac->cac_inferred_type = rhs_type;
    }

    సమితి_var_type(lhs->lhs_lvar, type, cctx);

    తిరిగి OK;
}

/*
 * Returns TRUE if the "rhs_type" can be assigned to the "lhs" variable.
 * Used when compiling an assignment statement.
 */
    స్థిర పూర్ణాంకం
compile_కేటాయించు_valid_rhs_type(
    cctx_T	*cctx,
    cac_T	*cac,
    lhs_T	*lhs,
    type_T	*rhs_type)
{
    type_T	*use_type = lhs->lhs_lvar->lv_type;
    where_T	where = WHERE_INIT;

    // Without operator check type here, otherwise below.
    // Use the line number of the assignment.
    SOURCING_LNUM = cac->cac_ప్రారంభం_lnum;
    ఒకవేళ (cac->cac_var_లెక్క > 0)
    {
	where.wt_సూచీ = cac->cac_var_idx + 1;
	where.wt_రకం = WT_VARIABLE;
    }

    // If assigning to a list or dict member, use the member type.
    // Not for "list[:] =".
    ఒకవేళ (lhs->lhs_has_సూచీ &&
	    !has_list_సూచీ(cac->cac_var_ప్రారంభం + lhs->lhs_varlen, cctx))
	use_type = lhs->lhs_member_type;

    ఒకవేళ (need_type_where(rhs_type, use_type, 0, -1, where, cctx, FALSE,
						cac->cac_is_const) == FAIL)
	తిరిగి FALSE;

    తిరిగి TRUE;
}

/*
 * Compare the LHS type with the RHS type in an assignment.
 */
    స్థిర పూర్ణాంకం
compile_కేటాయించు_check_type(cctx_T *cctx, cac_T *cac)
{
    lhs_T	*lhs = &cac->cac_lhs;
    type_T	*rhs_type;

    rhs_type = cctx->ctx_type_దొంతర.ga_len == 0
					? &t_void
					: get_type_on_దొంతర(cctx, 0);
    cac->cac_rhs_type = rhs_type;

    ఒకవేళ (check_type_is_విలువ(rhs_type) == FAIL)
	తిరిగి FAIL;

    ఒకవేళ (lhs->lhs_lvar != ఖాళీ && (cac->cac_is_decl || !lhs->lhs_has_type))
    {
	ఒకవేళ (rhs_type->tt_type == VAR_FUNC
					|| rhs_type->tt_type == VAR_PARTIAL)
	{
	    // Make sure the variable name can be used as a funcref
	    ఒకవేళ (!lhs->lhs_has_సూచీ
				&& var_wrong_func_name(lhs->lhs_name, TRUE))
		తిరిగి FAIL;
	}

	ఒకవేళ (lhs->lhs_new_local && !lhs->lhs_has_type)
	{
	    // The LHS variable doesn't have a type.  Set it to the RHS type.
	    ఒకవేళ (compile_కేటాయించు_సమితి_lhs_type_from_rhs(cctx, cac, lhs, rhs_type)
								== FAIL)
		తిరిగి FAIL;
	}
	లేకపోతే ఒకవేళ (*cac->cac_op == '=')
	{
	    ఒకవేళ (!compile_కేటాయించు_valid_rhs_type(cctx, cac, lhs, rhs_type))
		తిరిగి FAIL;
	}
    }
    లేకపోతే
    {
	// Assigning to a register using @r = "abc"

	type_T *lhs_type = lhs->lhs_member_type;

	// Special case: assigning to @# can use a number or a string.
	// Also: can assign a number to a float.
	ఒకవేళ ((lhs_type == &t_సంఖ్య_or_string || lhs_type == &t_float)
					&& rhs_type->tt_type == VAR_సంఖ్య)
	    lhs_type = &t_సంఖ్య;

	ఒకవేళ (*cac->cac_nextc != '=')
	{
	    ఒకవేళ (need_type(rhs_type, lhs_type, 0, -1, 0, cctx, FALSE,
							FALSE) == FAIL)
		తిరిగి FAIL;
	}
    }

    తిరిగి OK;
}

/*
 * Compile the RHS expression in an assignment statement and generate the
 * instructions.
 */
    స్థిర పూర్ణాంకం
compile_కేటాయించు_rhs_సమాసం(cctx_T *cctx, cac_T *cac)
{
    cac->cac_is_const = FALSE;

    // for "+=", "*=", "..=" etc. first load the current value
    ఒకవేళ (*cac->cac_op != '='
	    && compile_ఎక్కించు_lhs_with_సూచీ(&cac->cac_lhs, cac->cac_var_ప్రారంభం,
								cctx) == FAIL)
	తిరిగి FAIL;

    // For "var = expr" evaluate the expression.
    ఒకవేళ (cac->cac_var_లెక్క == 0)
    {
	పూర్ణాంకం	ret;

	// Compile the expression.
	cac->cac_instr_లెక్క = cac->cac_instr->ga_len;
	ret = compile_కేటాయించు_single_eval_సమాసం(cctx, cac);
	ఒకవేళ (ret == FAIL)
	    తిరిగి FAIL;
    }
    లేకపోతే ఒకవేళ (cac->cac_semicolon && cac->cac_var_idx == cac->cac_var_లెక్క - 1)
    {
	// For "[var; var] = expr" get the rest of the list
	cac->cac_did_generate_ముక్క = TRUE;
	ఒకవేళ (generate_ముక్క(cctx, cac->cac_var_లెక్క - 1) == FAIL)
	    తిరిగి FAIL;
    }
    లేకపోతే
    {
	// For "[var, var] = expr" get the "var_idx" item from the
	// list.
	పూర్ణాంకం with_op = *cac->cac_op != '=';
	ఒకవేళ (generate_GETITEM(cctx, cac->cac_var_idx, with_op) == FAIL)
	    తిరిగి FAIL;
    }

    ఒకవేళ (compile_కేటాయించు_check_type(cctx, cac) == FAIL)
	తిరిగి FAIL;

    తిరిగి OK;
}

/*
 * Compile the RHS expression in an assignment
 */
    స్థిర పూర్ణాంకం
compile_కేటాయించు_rhs(cctx_T *cctx, cac_T *cac)
{
    lhs_T	*lhs = &cac->cac_lhs;

    ఒకవేళ (cctx->ctx_skip == SKIP_YES)
    {
	ఒకవేళ (cac->cac_oplen > 0 && cac->cac_var_లెక్క == 0)
	{
	    // skip over the "=" and the expression
	    cac->cac_nextc = skipwhite(cac->cac_op + cac->cac_oplen);
	    (శూన్య)compile_expr0(&cac->cac_nextc, cctx);
	}
	తిరిగి OK;
    }

    // If RHS is specified, then generate instructions for RHS expression
    ఒకవేళ (cac->cac_oplen > 0)
	తిరిగి compile_కేటాయించు_rhs_సమాసం(cctx, cac);

    ఒకవేళ (cac->cac_cmdidx == CMD_final)
    {
	emsg(_(e_final_requires_a_విలువ));
	తిరిగి FAIL;
    }

    ఒకవేళ (cac->cac_cmdidx == CMD_const)
    {
	emsg(_(e_const_requires_a_విలువ));
	తిరిగి FAIL;
    }

    ఒకవేళ (!lhs->lhs_has_type || lhs->lhs_dest == dest_option
					|| lhs->lhs_dest == dest_func_option)
    {
	emsg(_(e_type_or_initialization_required));
	తిరిగి FAIL;
    }

    // variables are always initialized
    ఒకవేళ (GA_GROW_FAILS(cac->cac_instr, 1))
	తిరిగి FAIL;

    cac->cac_instr_లెక్క = cac->cac_instr->ga_len;

    తిరిగి నెట్టు_default_విలువ(cctx, lhs->lhs_member_type->tt_type,
			      lhs->lhs_dest == dest_local,
			      &cac->cac_skip_store);
}

/*
 * Returns OK if "type" supports compound operator "op_arg" (e.g. +=, -=, %=,
 * etc.).  Compound operators are not supported with a tuple and a dict.
 * Returns FAIL if compound operator is not supported.
 */
    స్థిర పూర్ణాంకం
check_type_supports_compound_op(type_T *type, char_u op_arg)
{
    ఒకవేళ (type->tt_type == VAR_TUPLE || type->tt_type == VAR_DICT)
    {
	char_u	op[2];

	op[0] = op_arg;
	op[1] = NUL;
	semsg(_(e_wrong_variable_type_for_str_equal), op);
	తిరిగి FAIL;
    }

    తిరిగి OK;
}

/*
 * Compile a compound op assignment statement (+=, -=, *=, %=, etc.)
 */
    స్థిర పూర్ణాంకం
compile_కేటాయించు_compound_op(cctx_T *cctx, cac_T *cac)
{
    lhs_T	    *lhs = &cac->cac_lhs;
    type_T	    *expected;
    type_T	    *stacktype = ఖాళీ;

    ఒకవేళ (cac->cac_lhs.lhs_type->tt_type == VAR_TUPLE
	    && check_type_supports_compound_op(cac->cac_lhs.lhs_type,
							*cac->cac_op) == FAIL)
	తిరిగి FAIL;

    ఒకవేళ (*cac->cac_op == '.')
    {
	expected = lhs->lhs_member_type;
	stacktype = get_type_on_దొంతర(cctx, 0);
	ఒకవేళ (expected != &t_string
		&& need_type(stacktype, expected, 0, -1, 0, cctx,
					FALSE, FALSE) == FAIL)
	    తిరిగి FAIL;
	లేకపోతే ఒకవేళ (may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
	    తిరిగి FAIL;
    }
    లేకపోతే
    {
	expected = lhs->lhs_member_type;
	stacktype = get_type_on_దొంతర(cctx, 0);

	ఒకవేళ (check_type_supports_compound_op(expected, *cac->cac_op) == FAIL)
	    తిరిగి FAIL;

	ఒకవేళ (
		// If variable is float operation with number is OK.
		!(expected == &t_float && (stacktype == &t_సంఖ్య
					|| stacktype == &t_సంఖ్య_bool))
		&& need_type(stacktype, expected, TYPECHK_సంఖ్య_OK, -1, 0,
						cctx, FALSE, FALSE) == FAIL)
	    తిరిగి FAIL;
    }

    ఒకవేళ (*cac->cac_op == '.')
    {
	ఒకవేళ (generate_CONCAT(cctx, 2) == FAIL)
	    తిరిగి FAIL;
    }
    లేకపోతే ఒకవేళ (*cac->cac_op == '+')
    {
	ఒకవేళ (generate_కూడు_instr(cctx,
		    operator_type(lhs->lhs_member_type, stacktype),
		    lhs->lhs_member_type, stacktype,
		    సమాసం_APPEND) == FAIL)
	    తిరిగి FAIL;
    }
    లేకపోతే ఒకవేళ (generate_two_op(cctx, cac->cac_op) == FAIL)
	తిరిగి FAIL;

    తిరిగి OK;
}

/*
 * Generate the STORE and SETTYPE instructions for an assignment statement.
 */
    స్థిర పూర్ణాంకం
compile_కేటాయించు_generate_store(cctx_T *cctx, cac_T *cac)
{
    lhs_T	*lhs = &cac->cac_lhs;
    పూర్ణాంకం		save_lnum;

    // Use the line number of the assignment for store instruction.
    save_lnum = cctx->ctx_lnum;
    cctx->ctx_lnum = cac->cac_ప్రారంభం_lnum - 1;

    ఒకవేళ (lhs->lhs_has_సూచీ)
    {
	// Use the info in "lhs" to store the value at the index in the
	// list, dict or object.
	ఒకవేళ (compile_కేటాయించు_unlet(cac->cac_var_ప్రారంభం, &cac->cac_lhs,
				 TRUE, cac->cac_rhs_type, cctx) == FAIL)
	{
	    cctx->ctx_lnum = save_lnum;
	    తిరిగి FAIL;
	}
    }
    లేకపోతే
    {
	ఒకవేళ (cac->cac_is_decl && cac->cac_cmdidx == CMD_const &&
			(lhs->lhs_dest == dest_script
			 || lhs->lhs_dest == dest_script_v9
			 || lhs->lhs_dest == dest_global
			 || lhs->lhs_dest == dest_local))
	    // ":const var": lock the value, but not referenced variables
	    generate_LOCKCONST(cctx);

	type_T	*inferred_type = cac->cac_inferred_type;

	ఒకవేళ ((lhs->lhs_type->tt_type == VAR_DICT
		    || lhs->lhs_type->tt_type == VAR_LIST)
		&& lhs->lhs_type->tt_member != ఖాళీ
		&& lhs->lhs_type->tt_member != &t_any
		&& lhs->lhs_type->tt_member != &t_unknown)
	    // Set the type in the list or dict, so that it can be
	    // checked, also in legacy script.
	    generate_SETTYPE(cctx, lhs->lhs_type);
	లేకపోతే ఒకవేళ (lhs->lhs_type->tt_type == VAR_TUPLE
					&& lhs->lhs_type->tt_argcount != 0)
	    generate_SETTYPE(cctx, lhs->lhs_type);
	లేకపోతే ఒకవేళ (inferred_type != ఖాళీ
		&& (inferred_type->tt_type == VAR_DICT
		    || inferred_type->tt_type == VAR_LIST)
		&& inferred_type->tt_member != ఖాళీ
		&& inferred_type->tt_member != &t_unknown
		&& inferred_type->tt_member != &t_any)
	    // Set the type in the list or dict, so that it can be
	    // checked, also in legacy script.
	    generate_SETTYPE(cctx, inferred_type);
	లేకపోతే ఒకవేళ (inferred_type != ఖాళీ
				&& inferred_type->tt_type == VAR_TUPLE
				&& inferred_type->tt_argcount > 0)
	    generate_SETTYPE(cctx, inferred_type);

	ఒకవేళ (!cac->cac_skip_store &&
		generate_store_lhs(cctx, &cac->cac_lhs,
				   cac->cac_instr_లెక్క,
				   cac->cac_is_decl) == FAIL)
	{
	    cctx->ctx_lnum = save_lnum;
	    తిరిగి FAIL;
	}
    }

    cctx->ctx_lnum = save_lnum;

    తిరిగి OK;
}

/*
 * Process the variable(s) in an assignment statement
 */
    స్థిర పూర్ణాంకం
compile_కేటాయించు_process_variables(
    cctx_T	*cctx,
    cac_T	*cac,
    పూర్ణాంకం		cmdidx,
    పూర్ణాంకం		heredoc,
    పూర్ణాంకం		has_cmd,
    పూర్ణాంకం		has_argisset_ఉపసర్గ,
    పూర్ణాంకం		jump_instr_idx)
{
    /*
     * Loop over variables in "[var, var] = expr".
     * For "name = expr" and "var name: type" this is done only once.
     */
    కొరకు (cac->cac_var_idx = 0; cac->cac_var_idx == 0 ||
	    cac->cac_var_idx < cac->cac_var_లెక్క; cac->cac_var_idx++)
    {
	ఒకవేళ (cac->cac_var_ప్రారంభం[0] == '_'
				&& !eval_isnamec(cac->cac_var_ప్రారంభం[1]))
	{
	    // Ignore underscore in "[a, _, b] = list".
	    ఒకవేళ (cac->cac_var_లెక్క > 0)
	    {
		cac->cac_var_ప్రారంభం = skipwhite(cac->cac_var_ప్రారంభం + 2);
		కొనసాగు;
	    }
	    emsg(_(e_cannot_use_underscore_ఇక్కడ));
	    తిరిగి FAIL;
	}
	vim_free(cac->cac_lhs.lhs_name);

	/*
	 * Figure out the LHS type and other properties.
	 */
	ఒకవేళ (compile_కేటాయించు_lhs(cac->cac_var_ప్రారంభం, &cac->cac_lhs, cmdidx,
		    cac->cac_is_decl, heredoc, has_cmd,
		    cac->cac_oplen, cctx) == FAIL)
	    తిరిగి FAIL;

	// Compile the RHS expression
	ఒకవేళ (heredoc)
	{
	    SOURCING_LNUM = cac->cac_ప్రారంభం_lnum;
	    ఒకవేళ (cac->cac_lhs.lhs_has_type
		    && need_type(&t_list_string, cac->cac_lhs.lhs_type,
				0, -1, 0, cctx, FALSE, FALSE) == FAIL)
		తిరిగి FAIL;
	}
	లేకపోతే
	{
	    ఒకవేళ (compile_కేటాయించు_rhs(cctx, cac) == FAIL)
		తిరిగి FAIL;
	    ఒకవేళ (cac->cac_var_లెక్క == 0)
		cac->cac_var_చివర = cac->cac_nextc;
	}

	// no need to parse more when skipping
	ఒకవేళ (cctx->ctx_skip == SKIP_YES)
	    ఆపు;

	ఒకవేళ (cac->cac_oplen > 0 && *cac->cac_op != '=')
	{
	    ఒకవేళ (compile_కేటాయించు_compound_op(cctx, cac) == FAIL)
		తిరిగి FAIL;
	}

	// generate the store instructions
	ఒకవేళ (compile_కేటాయించు_generate_store(cctx, cac) == FAIL)
	    తిరిగి FAIL;

	ఒకవేళ (cac->cac_var_idx + 1 < cac->cac_var_లెక్క)
	    cac->cac_var_ప్రారంభం = skipwhite(cac->cac_lhs.lhs_చివర + 1);

	ఒకవేళ (has_argisset_ఉపసర్గ)
	{
	    // set instruction index in JUMP_IF_ARG_SET to here
	    isn_T *isn = ((isn_T *)cac->cac_instr->ga_దత్తాంశం) + jump_instr_idx;
	    isn->isn_arg.jumparg.jump_where = cac->cac_instr->ga_len;
	}
    }

    తిరిగి OK;
}

/*
 * Compile declaration and assignment:
 * "let name"
 * "var name = expr"
 * "final name = expr"
 * "const name = expr"
 * "name = expr"
 * "arg" points to "name".
 * "++arg" and "--arg"
 * Return NULL for an error.
 * Return "arg" if it does not look like a variable list.
 */
    స్థిర char_u *
compile_assignment(
    char_u	*arg_ప్రారంభం,
    exarg_T	*eap,
    cmdidx_T	cmdidx,
    cctx_T	*cctx)
{
    cac_T	cac;
    char_u	*arg = arg_ప్రారంభం;
    char_u	*retstr = ఖాళీ;
    పూర్ణాంకం		heredoc = FALSE;
    పూర్ణాంకం		jump_instr_idx;

    compile_కేటాయించు_context_init(&cac, cctx, cmdidx, arg);

    jump_instr_idx = cac.cac_instr->ga_len;

    // process object variable initialization in a new() constructor method
    పూర్ణాంకం	has_argisset_ఉపసర్గ = STRNCMP(arg, "ifargisset ", 11) == 0;
    ఒకవేళ (has_argisset_ఉపసర్గ &&
			compile_కేటాయించు_obj_new_arg(&arg, cctx) == FAIL)
	వెళ్ళు theend;

    // Skip over the "varname" or "[varname, varname]" to get to any "=".
    cac.cac_nextc = skip_var_list(arg, TRUE, &cac.cac_var_లెక్క,
						&cac.cac_semicolon, TRUE);
    ఒకవేళ (cac.cac_nextc == ఖాళీ)
	తిరిగి *arg == '[' ? arg : ఖాళీ;

    ఒకవేళ (compile_కేటాయించు_process_operator(eap, arg, &cac, &heredoc,
							&retstr) == FAIL)
	తిరిగి retstr;

    // Compute the start of the assignment
    cac.cac_var_ప్రారంభం = compile_కేటాయించు_compute_ప్రారంభం(arg, cac.cac_var_లెక్క);

    // Compute the end of the assignment
    cac.cac_var_చివర = compile_కేటాయించు_compute_చివర(eap, cctx, &cac, heredoc);
    ఒకవేళ (cac.cac_var_చివర == ఖాళీ)
	తిరిగి ఖాళీ;

    పూర్ణాంకం has_cmd = cac.cac_var_ప్రారంభం > eap->cmd;

    /* process the variable(s) */
    ఒకవేళ (compile_కేటాయించు_process_variables(cctx, &cac, cmdidx, heredoc,
					 has_cmd, has_argisset_ఉపసర్గ,
					 jump_instr_idx) == FAIL)
	వెళ్ళు theend;

    // For "[var, var] = expr" drop the "expr" value.
    // Also for "[var, var; _] = expr".
    ఒకవేళ (cctx->ctx_skip != SKIP_YES && cac.cac_var_లెక్క > 0 &&
	    (!cac.cac_semicolon || !cac.cac_did_generate_ముక్క))
    {
	ఒకవేళ (generate_instr_drop(cctx, ISN_DROP, 1) == ఖాళీ)
	    వెళ్ళు theend;
    }

    retstr = skipwhite(cac.cac_var_చివర);

theend:
    vim_free(cac.cac_lhs.lhs_name);
    తిరిగి retstr;
}

/*
 * Check for an assignment at "eap->cmd", compile it if found.
 * Return NOTDONE if there is none, FAIL for failure, OK if done.
 */
    స్థిర పూర్ణాంకం
may_compile_assignment(exarg_T *eap, char_u **పంక్తి, cctx_T *cctx)
{
    char_u  *pskip;
    char_u  *p;

    // Assuming the command starts with a variable or function name,
    // find what follows.
    // Skip over "var.member", "var[idx]" and the like.
    // Also "&opt = val", "$ENV = val" and "@r = val".
    pskip = (*eap->cmd == '&' || *eap->cmd == '$' || *eap->cmd == '@')
						 ? eap->cmd + 1 : eap->cmd;
    p = గమ్యం_name_చివర(pskip, TRUE);
    ఒకవేళ (p > eap->cmd && *p != NUL)
    {
	char_u *var_చివర;
	పూర్ణాంకం	oplen;
	పూర్ణాంకం	heredoc;

	ఒకవేళ (eap->cmd[0] == '@')
	    var_చివర = eap->cmd + 2;
	లేకపోతే
	    var_చివర = కనుగొను_name_చివర(pskip, ఖాళీ, ఖాళీ,
					FNE_CHECK_ప్రారంభం | FNE_INCL_BR);
	oplen = assignment_len(skipwhite(var_చివర), &heredoc);
	ఒకవేళ (oplen > 0)
	{
	    size_t len = p - eap->cmd;

	    // Recognize an assignment if we recognize the variable
	    // name:
	    // "&opt = expr"
	    // "$ENV = expr"
	    // "@r = expr"
	    // "g:var = expr"
	    // "g:[key] = expr"
	    // "local = expr"  where "local" is a local var.
	    // "script = expr"  where "script" is a script-local var.
	    // "import = expr"  where "import" is an imported var
	    ఒకవేళ (*eap->cmd == '&'
		    || *eap->cmd == '$'
		    || *eap->cmd == '@'
		    || ((len) > 2 && eap->cmd[1] == ':')
		    || STRNCMP(eap->cmd, "g:[", 3) == 0
		    || variable_exists(eap->cmd, len, cctx))
	    {
		*పంక్తి = compile_assignment(eap->cmd, eap, CMD_కొలత, cctx);
		ఒకవేళ (*పంక్తి == ఖాళీ || *పంక్తి == eap->cmd)
		    తిరిగి FAIL;
		తిరిగి OK;
	    }
	}
    }

    // might be "[var, var] = expr" or "ifargisset this.member = expr"
    ఒకవేళ (*eap->cmd == '[' || STRNCMP(eap->cmd, "ifargisset ", 11) == 0)
    {
	*పంక్తి = compile_assignment(eap->cmd, eap, CMD_కొలత, cctx);
	ఒకవేళ (*పంక్తి == ఖాళీ)
	    తిరిగి FAIL;
	ఒకవేళ (*పంక్తి != eap->cmd)
	    తిరిగి OK;
    }
    తిరిగి NOTDONE;
}

/*
 * Check if arguments of "ufunc" shadow variables in "cctx".
 * Return OK or FAIL.
 */
    స్థిర పూర్ణాంకం
check_వాదనలు_shadowing(ufunc_T *ufunc, cctx_T *cctx)
{
    పూర్ణాంకం	    i;
    char_u  *arg;
    పూర్ణాంకం	    r = OK;

    // Make sure arguments are not found when compiling a second time.
    ufunc->uf_వాదనలు_visible = 0;

    // Check for arguments shadowing variables from the context.
    కొరకు (i = 0; i < ufunc->uf_వాదనలు.ga_len; ++i)
    {
	arg = ((char_u **)(ufunc->uf_వాదనలు.ga_దత్తాంశం))[i];
	ఒకవేళ (check_defined(arg, STRLEN(arg), cctx, ఖాళీ, TRUE) == FAIL)
	{
	    r = FAIL;
	    ఆపు;
	}
    }
    ufunc->uf_వాదనలు_visible = ufunc->uf_వాదనలు.ga_len;
    తిరిగి r;
}

#నిర్వచితమైతే HAS_సందేశం_కిటికీ
/*
 * Get a count before a command.  Can only be a number.
 * Returns zero if there is no count.
 * Returns -1 if there is something wrong.
 */
    స్థిర దీర్ఘ
get_cmd_లెక్క(char_u *పంక్తి, exarg_T *eap)
{
    char_u *p;

    // skip over colons and white space
    కొరకు (p = పంక్తి; *p == ':' || VIM_ISWHITE(*p); ++p)
	;
    ఒకవేళ (!సురక్షిత_isdigit(*p))
    {
	// The command or modifiers must be following.  Assume a lower case
	// character means there is a modifier.
	ఒకవేళ (p < eap->cmd && !vim_islower(*p))
	{
	    emsg(_(e_invalid_range));
	    తిరిగి -1;
	}
	తిరిగి 0;
    }
    తిరిగి atol((అక్షరం *)p);
}
#అయితే_ముగింపు

/*
 * Get the compilation type that should be used for "ufunc".
 * Keep in sync with INSTRUCTIONS().
 */
    compiletype_T
get_compile_type(ufunc_T *ufunc)
{
    // Update uf_has_breakpoint if needed.
    update_has_breakpoint(ufunc);

    ఒకవేళ (debug_break_level > 0 || may_break_లోపల_function(ufunc))
	తిరిగి CT_DEBUG;
#నిర్వచితమైతే FEAT_PROFILE
    ఒకవేళ (do_profiling == PROF_YES)
    {
	ఒకవేళ (!ufunc->uf_profiling && has_profiling(FALSE, ufunc->uf_name, ఖాళీ,
							    &ufunc->uf_hash))
	    func_do_profile(ufunc);
	ఒకవేళ (ufunc->uf_profiling)
	    తిరిగి CT_PROFILE;
    }
#అయితే_ముగింపు
    తిరిగి CT_NONE;
}

/*
 * Free the compiled instructions saved for a def function.  This is used when
 * compiling a def function and the function was compiled before.
 * The index is reused.
 */
    స్థిర శూన్య
clear_def_function(ufunc_T *ufunc, compiletype_T compile_type)
{
    isn_T	*instr_dest = ఖాళీ;
    dfunc_T	*dfunc;

    dfunc = ((dfunc_T *)def_functions.ga_దత్తాంశం) + ufunc->uf_dfunc_idx;

    ఎంపిక (compile_type)
    {
	స్థితి CT_PROFILE:
#నిర్వచితమైతే FEAT_PROFILE
	    instr_dest = dfunc->df_instr_prof; ఆపు;
#అయితే_ముగింపు
	స్థితి CT_NONE:   instr_dest = dfunc->df_instr; ఆపు;
	స్థితి CT_DEBUG:  instr_dest = dfunc->df_instr_debug; ఆపు;
    }

    ఒకవేళ (instr_dest != ఖాళీ)
	// Was compiled in this mode before: Free old instructions.
	delete_def_function_contents(dfunc, FALSE);

    ga_clear_అక్షరశ్రేణులు(&dfunc->df_var_పేర్లు);
    dfunc->df_defer_var_idx = 0;
}

/*
 * Add a function to the list of :def functions.
 * This sets "ufunc->uf_dfunc_idx" but the function isn't compiled yet.
 */
    స్థిర పూర్ణాంకం
కూడు_def_function(ufunc_T *ufunc)
{
    dfunc_T *dfunc;

    ఒకవేళ (def_functions.ga_len == 0)
    {
	// The first position is not used, so that a zero uf_dfunc_idx means it
	// wasn't set.
	ఒకవేళ (GA_GROW_FAILS(&def_functions, 1))
	    తిరిగి FAIL;
	++def_functions.ga_len;
    }

    // Add the function to "def_functions".
    ఒకవేళ (GA_GROW_FAILS(&def_functions, 1))
	తిరిగి FAIL;
    dfunc = ((dfunc_T *)def_functions.ga_దత్తాంశం) + def_functions.ga_len;
    CLEAR_సూచిక(dfunc);
    dfunc->df_idx = def_functions.ga_len;
    ufunc->uf_dfunc_idx = dfunc->df_idx;
    dfunc->df_ufunc = ufunc;
    dfunc->df_name = vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
    ga_init2(&dfunc->df_var_పేర్లు, పరిమాణం(char_u *), 10);
    ++dfunc->df_refcount;
    ++def_functions.ga_len;
    తిరిగి OK;
}

    స్థిర పూర్ణాంకం
compile_dfunc_ufunc_init(
    ufunc_T		*ufunc,
    cctx_T		*outer_cctx,
    compiletype_T	compile_type,
    పూర్ణాంకం			*new_def_function)
{
    // When using a function that was compiled before: Free old instructions.
    // The index is reused.  Otherwise add a new entry in "def_functions".
    ఒకవేళ (ufunc->uf_dfunc_idx > 0)
	clear_def_function(ufunc, compile_type);
    లేకపోతే
    {
	ఒకవేళ (కూడు_def_function(ufunc) == FAIL)
	    తిరిగి FAIL;

	*new_def_function = TRUE;
    }

    ఒకవేళ ((ufunc->uf_flags & FC_సంవృతం) && outer_cctx == ఖాళీ)
    {
	semsg(_(e_compiling_సంవృతం_without_context_str),
						printable_func_name(ufunc));
	తిరిగి FAIL;
    }

    ufunc->uf_def_స్థితి = UF_COMPILING;

    తిరిగి OK;
}

/*
 * Initialize the compilation context for compiling a def function.
 */
    స్థిర శూన్య
compile_dfunc_cctx_init(
    cctx_T		*cctx,
    cctx_T		*outer_cctx,
    ufunc_T		*ufunc,
    compiletype_T	compile_type)
{
    CLEAR_క్షేత్రం(*cctx);

    cctx->ctx_compile_type = compile_type;
    cctx->ctx_ufunc = ufunc;
    cctx->ctx_lnum = -1;
    cctx->ctx_outer = outer_cctx;
    ga_init2(&cctx->ctx_locals, పరిమాణం(lvar_T), 10);
    // Each entry on the type stack consists of two type pointers.
    ga_init2(&cctx->ctx_type_దొంతర, పరిమాణం(type2_T), 50);
    cctx->ctx_type_list = &ufunc->uf_type_list;
    ga_init2(&cctx->ctx_instr, పరిమాణం(isn_T), 50);
}

/*
 * For an object constructor, generate instruction to setup "this" (the first
 * local variable) and to initialize the object variables.
 */
    స్థిర పూర్ణాంకం
obj_constructor_prologue(ufunc_T *ufunc, cctx_T *cctx)
{
    generate_CONSTRUCT(cctx, ufunc->uf_class);

    కొరకు (పూర్ణాంకం i = 0; i < ufunc->uf_class->class_obj_member_లెక్క; ++i)
    {
	ocmember_T *m = &ufunc->uf_class->class_obj_members[i];

	ఒకవేళ (i < 2 && IS_ENUM(ufunc->uf_class))
	    // The first two object variables in an enum are the name
	    // and the ordinal.  These are set by the ISN_CONSTRUCT
	    // instruction.  So don't generate instructions to set
	    // these variables.
	    కొనసాగు;

	ఒకవేళ (m->ocm_init != ఖాళీ)
	{
	    char_u	*సమాసం = m->ocm_init;
	    sctx_T	save_current_sctx;
	    పూర్ణాంకం		change_sctx = FALSE;

	    // If the member variable initialization script context is
	    // different from the current script context, then change it.
	    ఒకవేళ (current_sctx.sc_sid != m->ocm_init_sctx.sc_sid)
		change_sctx = TRUE;

	    ఒకవేళ (change_sctx)
	    {
		// generate an instruction to change the script context to the
		// member variable initialization script context.
		save_current_sctx = current_sctx;
		current_sctx = m->ocm_init_sctx;
		generate_SCRIPTCTX_సమితి(cctx, current_sctx);
	    }

	    పూర్ణాంకం r = compile_expr0(&సమాసం, cctx);

	    ఒకవేళ (change_sctx)
	    {
		// restore the previous script context
		current_sctx = save_current_sctx;
		generate_SCRIPTCTX_సమితి(cctx, current_sctx);
	    }

	    ఒకవేళ (r == FAIL)
		తిరిగి FAIL;

	    ఒకవేళ (!చివర్లు_excmd2(m->ocm_init, సమాసం))
	    {
		semsg(_(e_trailing_characters_str), సమాసం);
		తిరిగి FAIL;
	    }

	    type_T	*type = get_type_on_దొంతర(cctx, 0);
	    ఒకవేళ (m->ocm_type->tt_type == VAR_ANY
		    && !(m->ocm_flags & OCMFLAG_HAS_TYPE)
		    && type->tt_type != VAR_SPECIAL)
	    {
		// If the member variable type is not yet set, then use
		// the initialization expression type.
		m->ocm_type = type;
	    }
	    లేకపోతే
	    {
		// The type of the member initialization expression is
		// determined at run time.  Add a runtime type check.
		where_T	where = WHERE_INIT;
		where.wt_రకం = WT_MEMBER;
		where.wt_func_name = (అక్షరం *)m->ocm_name.string;
		ఒకవేళ (need_type_where(type, m->ocm_type, 0, -1,
					where, cctx, FALSE, FALSE) == FAIL)
		    తిరిగి FAIL;
	    }
	}
	లేకపోతే
	    నెట్టు_default_విలువ(cctx, m->ocm_type->tt_type, FALSE, ఖాళీ);

	ఒకవేళ (((m->ocm_type->tt_type == VAR_DICT
			|| m->ocm_type->tt_type == VAR_LIST)
		    && m->ocm_type->tt_member != ఖాళీ
		    && m->ocm_type->tt_member != &t_any
		    && m->ocm_type->tt_member != &t_unknown)
		|| (m->ocm_type->tt_type == VAR_TUPLE
		    && m->ocm_type->tt_argcount > 0))
	    // Set the type in the list, tuple or dict, so that it can be
	    // checked, also in legacy script.
	    generate_SETTYPE(cctx, m->ocm_type);

	generate_STORE_THIS(cctx, i);
    }

    తిరిగి OK;
}

/*
 * For an object method and an constructor, generate instruction to setup
 * "this" (the first local variable).  For a constructor, generate instructions
 * to initialize the object variables.
 */
    స్థిర పూర్ణాంకం
obj_పద్ధతి_prologue(ufunc_T *ufunc, cctx_T *cctx)
{
    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_దత్తాంశం) + ufunc->uf_dfunc_idx;

    ఒకవేళ (GA_GROW_FAILS(&dfunc->df_var_పేర్లు, 1))
	తిరిగి FAIL;

    ((char_u **)dfunc->df_var_పేర్లు.ga_దత్తాంశం)[0] =
						vim_strsave((char_u *)"this");
    ++dfunc->df_var_పేర్లు.ga_len;

    // In the constructor allocate memory for the object and initialize the
    // object members.
    ఒకవేళ (IS_CONSTRUCTOR_పద్ధతి(ufunc))
	తిరిగి obj_constructor_prologue(ufunc, cctx);

    తిరిగి OK;
}

/*
 * Produce instructions for the default values of optional arguments.
 */
    స్థిర పూర్ణాంకం
compile_def_function_default_వాదనలు(
    ufunc_T	*ufunc,
    garray_T	*instr,
    cctx_T	*cctx)
{
    పూర్ణాంకం	లెక్క = ufunc->uf_def_వాదనలు.ga_len;
    పూర్ణాంకం	మొదటి_def_arg = ufunc->uf_వాదనలు.ga_len - లెక్క;
    పూర్ణాంకం	i;
    పూర్ణాంకం	off = దొంతర_చట్రం_కొలత + (ufunc->uf_va_name != ఖాళీ ? 1 : 0);
    పూర్ణాంకం	did_సమితి_arg_type = FALSE;

    // Produce instructions for the default values of optional arguments.
    SOURCING_LNUM = 0;  // line number unknown
    కొరకు (i = 0; i < లెక్క; ++i)
    {
	char_u *arg = ((char_u **)(ufunc->uf_def_వాదనలు.ga_దత్తాంశం))[i];
	ఒకవేళ (STRCMP(arg, "v:none") == 0)
	    // "arg = v:none" means the argument is optional without
	    // setting a value when the argument is missing.
	    కొనసాగు;

	type_T	*val_type;
	పూర్ణాంకం		arg_idx = మొదటి_def_arg + i;
	where_T	where = WHERE_INIT;
	పూర్ణాంకం		jump_instr_idx = instr->ga_len;
	isn_T	*isn;

	// Use a JUMP_IF_ARG_SET instruction to skip if the value was given.
	ఒకవేళ (generate_JUMP_IF_ARG(cctx, ISN_JUMP_IF_ARG_సమితి,
						i - లెక్క - off) == FAIL)
	    తిరిగి FAIL;

	// Make sure later arguments are not found.
	ufunc->uf_వాదనలు_visible = arg_idx;

	పూర్ణాంకం r = compile_expr0(&arg, cctx);
	ఒకవేళ (r == FAIL)
	    తిరిగి FAIL;

	// If no type specified use the type of the default value.
	// Otherwise check that the default value type matches the
	// specified type.
	val_type = get_type_on_దొంతర(cctx, 0);
	where.wt_సూచీ = arg_idx + 1;
	where.wt_రకం = WT_వాదం;
	ఒకవేళ (ufunc->uf_arg_types[arg_idx] == &t_unknown)
	{
	    did_సమితి_arg_type = TRUE;
	    ufunc->uf_arg_types[arg_idx] = val_type;
	}
	లేకపోతే ఒకవేళ (need_type_where(val_type, ufunc->uf_arg_types[arg_idx],
		    0, -1, where, cctx, FALSE, FALSE) == FAIL)
	    తిరిగి FAIL;

	ఒకవేళ (generate_STORE(cctx, ISN_STORE, i - లెక్క - off, ఖాళీ) == FAIL)
	    తిరిగి FAIL;

	// set instruction index in JUMP_IF_ARG_SET to here
	isn = ((isn_T *)instr->ga_దత్తాంశం) + jump_instr_idx;
	isn->isn_arg.jumparg.jump_where = instr->ga_len;
    }

    ఒకవేళ (did_సమితి_arg_type)
	సమితి_function_type(ufunc);

    తిరిగి OK;
}

/*
 * Compile def function body.  Loop over all the lines in the function and
 * generate instructions.
 */
    స్థిర పూర్ణాంకం
compile_def_function_body(
    పూర్ణాంకం		చివరి_func_lnum,
    పూర్ణాంకం		check_return_type,
    garray_T	*పంక్తులు_గమ్యం_free,
    అక్షరం	**errormsg,
    cctx_T	*cctx)
{
    char_u	*పంక్తి = ఖాళీ;
    char_u	*p;
    పూర్ణాంకం		did_emsg_before = did_emsg;
#నిర్వచితమైతే FEAT_PROFILE
    పూర్ణాంకం		prof_lnum = -1;
#అయితే_ముగింపు
    పూర్ణాంకం		debug_lnum = -1;

    కొరకు (;;)
    {
	exarg_T	    ea;
	పూర్ణాంకం	    starts_with_colon = FALSE;
	char_u	    *cmd;
	cmdmod_T    local_cmdmod;

	// Bail out on the first error to avoid a flood of errors and report
	// the right line number when inside try/catch.
	ఒకవేళ (did_emsg_before != did_emsg)
	    తిరిగి FAIL;

	ఒకవేళ (పంక్తి != ఖాళీ && *పంక్తి == '|')
	    // the line continues after a '|'
	    ++పంక్తి;
	లేకపోతే ఒకవేళ (పంక్తి != ఖాళీ && *skipwhite(పంక్తి) != NUL
		&& !(*పంక్తి == '#' && (పంక్తి == cctx->ctx_పంక్తి_ప్రారంభం
						    || VIM_ISWHITE(పంక్తి[-1]))))
	{
	    semsg(_(e_trailing_characters_str), పంక్తి);
	    తిరిగి FAIL;
	}
	లేకపోతే ఒకవేళ (పంక్తి != ఖాళీ && vim9_bad_comment(skipwhite(పంక్తి)))
	    తిరిగి FAIL;
	లేకపోతే
	{
	    పంక్తి = తదుపరి_పంక్తి_from_context(cctx, FALSE);
	    ఒకవేళ (cctx->ctx_lnum >= చివరి_func_lnum)
	    {
		// beyond the last line
#నిర్వచితమైతే FEAT_PROFILE
		ఒకవేళ (cctx->ctx_skip != SKIP_YES)
		    may_generate_prof_చివర(cctx, prof_lnum);
#అయితే_ముగింపు
		ఆపు;
	    }
	    // Make a copy, splitting off nextcmd and removing trailing spaces
	    // may change it.
	    ఒకవేళ (పంక్తి != ఖాళీ)
	    {
		పంక్తి = vim_strsave(పంక్తి);
		ఒకవేళ (ga_కూడు_string(పంక్తులు_గమ్యం_free, పంక్తి) == FAIL)
		{
		    vim_free(పంక్తి);
		    తిరిగి FAIL;
		}
	    }
	}

	CLEAR_క్షేత్రం(ea);
	ea.cmdlinep = &పంక్తి;
	ea.cmd = skipwhite(పంక్తి);
	ea.skip = cctx->ctx_skip == SKIP_YES;

	ఒకవేళ (*ea.cmd == '#')
	{
	    // "#" starts a comment, but "#{" is an error
	    ఒకవేళ (vim9_bad_comment(ea.cmd))
		తిరిగి FAIL;
	    పంక్తి = (char_u *)"";
	    కొనసాగు;
	}

#నిర్వచితమైతే FEAT_PROFILE
	ఒకవేళ (cctx->ctx_compile_type == CT_PROFILE && cctx->ctx_lnum != prof_lnum
						  && cctx->ctx_skip != SKIP_YES)
	{
	    may_generate_prof_చివర(cctx, prof_lnum);

	    prof_lnum = cctx->ctx_lnum;
	    generate_instr(cctx, ISN_PROF_ప్రారంభం);
	}
#అయితే_ముగింపు
	ఒకవేళ (cctx->ctx_compile_type == CT_DEBUG && cctx->ctx_lnum != debug_lnum
						  && cctx->ctx_skip != SKIP_YES)
	{
	    debug_lnum = cctx->ctx_lnum;
	    generate_instr_debug(cctx);
	}
	cctx->ctx_prev_lnum = cctx->ctx_lnum + 1;

	// Some things can be recognized by the first character.
	ఎంపిక (*ea.cmd)
	{
	    స్థితి '}':
		{
		    // "}" ends a block scope
		    scopetype_T stype = cctx->ctx_scope == ఖాళీ
					  ? NO_SCOPE : cctx->ctx_scope->se_type;

		    ఒకవేళ (stype == ఖండం_SCOPE)
		    {
			compile_endblock(cctx);
			పంక్తి = ea.cmd;
		    }
		    లేకపోతే
		    {
			emsg(_(e_using_rcurly_outside_if_ఖండం_scope));
			తిరిగి FAIL;
		    }
		    ఒకవేళ (పంక్తి != ఖాళీ)
			పంక్తి = skipwhite(ea.cmd + 1);
		    కొనసాగు;
		}

	    స్థితి '{':
		// "{" starts a block scope
		// "{'a': 1}->func() is something else
		ఒకవేళ (చివర్లు_excmd(*skipwhite(ea.cmd + 1)))
		{
		    పంక్తి = compile_ఖండం(ea.cmd, cctx);
		    కొనసాగు;
		}
		ఆపు;
	}

	/*
	 * COMMAND MODIFIERS
	 */
	cctx->ctx_has_cmdmod = FALSE;
	ఒకవేళ (parse_command_modifiers(&ea, errormsg, &local_cmdmod, FALSE)
								       == FAIL)
	    తిరిగి FAIL;
	generate_cmdmods(cctx, &local_cmdmod);
	undo_cmdmod(&local_cmdmod);

	// Check if there was a colon after the last command modifier or before
	// the current position.
	కొరకు (p = ea.cmd; p >= పంక్తి; --p)
	{
	    ఒకవేళ (*p == ':')
		starts_with_colon = TRUE;
	    ఒకవేళ (p < ea.cmd && !VIM_ISWHITE(*p))
		ఆపు;
	}

	// Skip ":call" to get to the function name, unless using :legacy
	p = ea.cmd;
	ఒకవేళ (!(local_cmdmod.cmod_flags & CMOD_LEGACY))
	{
	    ఒకవేళ (checkforcmd(&ea.cmd, "call", 3))
	    {
		ఒకవేళ (*ea.cmd == '(')
		    // not for "call()"
		    ea.cmd = p;
		లేకపోతే
		    ea.cmd = skipwhite(ea.cmd);
	    }

	    ఒకవేళ (!starts_with_colon)
	    {
		పూర్ణాంకం	    కేటాయించు;

		// Check for assignment after command modifiers.
		కేటాయించు = may_compile_assignment(&ea, &పంక్తి, cctx);
		ఒకవేళ (కేటాయించు == OK)
		    వెళ్ళు nextline;
		ఒకవేళ (కేటాయించు == FAIL)
		    తిరిగి FAIL;
	    }
	}

	/*
	 * COMMAND after range
	 * 'text'->func() should not be confused with 'a mark
	 * 0z1234->func() should not be confused with a zero line number
	 * "++nr" and "--nr" are eval commands
	 * in "$ENV->func()" the "$" is not a range
	 * "123->func()" is a method call
	 */
	cmd = ea.cmd;
	ఒకవేళ ((*cmd != '$' || starts_with_colon)
		&& (starts_with_colon
		    || !(*cmd == '\''
			|| (cmd[0] == '0' && cmd[1] == 'z')
			|| (cmd[0] != NUL && cmd[0] == cmd[1]
					    && (*cmd == '+' || *cmd == '-'))
			|| సంఖ్య_పద్ధతి(cmd))))
	{
	    ea.cmd = skip_range(ea.cmd, TRUE, ఖాళీ);
	    ఒకవేళ (ea.cmd > cmd)
	    {
		ఒకవేళ (!starts_with_colon
				   && !(local_cmdmod.cmod_flags & CMOD_LEGACY))
		{
		    semsg(_(e_colon_required_before_range_str), cmd);
		    తిరిగి FAIL;
		}
		ea.addr_లెక్క = 1;
		ఒకవేళ (చివర్లు_excmd2(పంక్తి, ea.cmd))
		{
		    // A range without a command: jump to the line.
		    generate_EXEC(cctx, ISN_EXECRANGE,
					      vim_strnsave(cmd, ea.cmd - cmd));
		    పంక్తి = ea.cmd;
		    వెళ్ళు nextline;
		}
	    }
	}
	p = కనుగొను_ex_command(&ea, ఖాళీ,
		starts_with_colon || (local_cmdmod.cmod_flags & CMOD_LEGACY)
						  ? ఖాళీ : అంశం_exists, cctx);

	ఒకవేళ (p == ఖాళీ)
	{
	    ఒకవేళ (cctx->ctx_skip != SKIP_YES)
		semsg(_(e_ambiguous_use_యొక్క_user_defined_command_str), ea.cmd);
	    తిరిగి FAIL;
	}

	// When using ":legacy cmd" always use compile_exec().
	ఒకవేళ (local_cmdmod.cmod_flags & CMOD_LEGACY)
	{
	    char_u *ప్రారంభం = ea.cmd;

	    ఎంపిక (ea.cmdidx)
	    {
		స్థితి CMD_if:
		స్థితి CMD_elseif:
		స్థితి CMD_else:
		స్థితి CMD_endif:
		స్థితి CMD_for:
		స్థితి CMD_endfor:
		స్థితి CMD_continue:
		స్థితి CMD_break:
		స్థితి CMD_while:
		స్థితి CMD_endwhile:
		స్థితి CMD_try:
		స్థితి CMD_catch:
		స్థితి CMD_finally:
		స్థితి CMD_endtry:
			semsg(_(e_cannot_use_legacy_with_command_str), ea.cmd);
			తిరిగి FAIL;
		అప్రమేయ: ఆపు;
	    }

	    // ":legacy return expr" needs to be handled differently.
	    ఒకవేళ (checkforcmd(&ప్రారంభం, "return", 4))
		ea.cmdidx = CMD_return;
	    లేకపోతే
		ea.cmdidx = CMD_legacy;
	}

	ఒకవేళ (p == ea.cmd && ea.cmdidx != CMD_కొలత)
	{
	    // "eval" is used for "val->func()" and "var" for "var = val", then
	    // "p" is equal to "ea.cmd" for a valid command.
	    ఒకవేళ (ea.cmdidx == CMD_eval || ea.cmdidx == CMD_var)
		;
	    లేకపోతే ఒకవేళ (cctx->ctx_skip == SKIP_YES)
	    {
		పంక్తి += STRLEN(పంక్తి);
		వెళ్ళు nextline;
	    }
	    లేకపోతే
	    {
		semsg(_(e_command_not_recognized_str), ea.cmd);
		తిరిగి FAIL;
	    }
	}

	ఒకవేళ ((cctx->ctx_had_return || cctx->ctx_had_throw)
		&& ea.cmdidx != CMD_elseif
		&& ea.cmdidx != CMD_else
		&& ea.cmdidx != CMD_endif
		&& ea.cmdidx != CMD_endfor
		&& ea.cmdidx != CMD_endwhile
		&& ea.cmdidx != CMD_catch
		&& ea.cmdidx != CMD_finally
		&& ea.cmdidx != CMD_endtry
		&& !ignore_unreachable_సంకేతం_for_testing)
	{
	    semsg(_(e_unreachable_సంకేతం_after_str),
				     cctx->ctx_had_return ? "return" : "throw");
	    తిరిగి FAIL;
	}

	// When processing the end of an if-else or try-catch block, don't
	// clear the "ctx_had_throw" flag.  If such a block ends in a "throw"
	// statement, then it is considered to end in a "return" statement.
	// The "ctx_had_throw" is cleared immediately after processing the
	// block ending statement.
	// Otherwise, clear the "had_throw" flag.
	ఒకవేళ (ea.cmdidx != CMD_else && ea.cmdidx != CMD_elseif
						&& ea.cmdidx != CMD_endif
						&& ea.cmdidx != CMD_catch
						&& ea.cmdidx != CMD_finally
						&& ea.cmdidx != CMD_endtry)
	    cctx->ctx_had_throw = FALSE;

	p = skipwhite(p);
	ఒకవేళ (ea.cmdidx != CMD_కొలత
			    && ea.cmdidx != CMD_write && ea.cmdidx != CMD_read)
	{
	    ఒకవేళ (ea.cmdidx >= 0)
		ea.argt = excmd_get_argt(ea.cmdidx);
	    ఒకవేళ ((ea.argt & EX_BANG) && *p == '!')
	    {
		ea.forceit = TRUE;
		p = skipwhite(p + 1);
	    }
	    ఒకవేళ ((ea.argt & EX_RANGE) == 0 && ea.addr_లెక్క > 0)
	    {
		emsg(_(e_no_range_allowed));
		తిరిగి FAIL;
	    }
	}

	ఎంపిక (ea.cmdidx)
	{
	    స్థితి CMD_def:
	    స్థితి CMD_function:
		    ea.arg = p;
		    పంక్తి = compile_nested_function(&ea, cctx, పంక్తులు_గమ్యం_free);
		    ఆపు;

	    స్థితి CMD_return:
		    పంక్తి = compile_return(p, check_return_type,
				 local_cmdmod.cmod_flags & CMOD_LEGACY, cctx);
		    cctx->ctx_had_return = TRUE;
		    ఆపు;

	    స్థితి CMD_let:
		    emsg(_(e_cannot_use_let_లోపల_vim9_script));
		    ఆపు;
	    స్థితి CMD_var:
	    స్థితి CMD_final:
	    స్థితి CMD_const:
	    స్థితి CMD_increment:
	    స్థితి CMD_decrement:
		    పంక్తి = compile_assignment(p, &ea, ea.cmdidx, cctx);
		    ఒకవేళ (పంక్తి == p)
		    {
			emsg(_(e_invalid_assignment));
			పంక్తి = ఖాళీ;
		    }
		    ఆపు;

	    స్థితి CMD_unlet:
	    స్థితి CMD_unlockvar:
	    స్థితి CMD_lockvar:
		    పంక్తి = compile_unletlock(p, &ea, cctx);
		    ఆపు;

	    స్థితి CMD_import:
		    emsg(_(e_import_can_only_be_వాడిన_లోపల_script));
		    పంక్తి = ఖాళీ;
		    ఆపు;

	    స్థితి CMD_if:
		    పంక్తి = compile_if(p, cctx);
		    ఆపు;
	    స్థితి CMD_elseif:
		    పంక్తి = compile_elseif(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    cctx->ctx_had_throw = FALSE;
		    ఆపు;
	    స్థితి CMD_else:
		    పంక్తి = compile_else(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    cctx->ctx_had_throw = FALSE;
		    ఆపు;
	    స్థితి CMD_endif:
		    పంక్తి = compile_endif(p, cctx);
		    cctx->ctx_had_throw = FALSE;
		    ఆపు;

	    స్థితి CMD_while:
		    పంక్తి = compile_while(p, cctx);
		    ఆపు;
	    స్థితి CMD_endwhile:
		    పంక్తి = compile_endwhile(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    ఆపు;

	    స్థితి CMD_for:
		    పంక్తి = compile_for(p, cctx);
		    ఆపు;
	    స్థితి CMD_endfor:
		    పంక్తి = compile_endfor(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    ఆపు;
	    స్థితి CMD_continue:
		    పంక్తి = compile_continue(p, cctx);
		    // "continue" unconditionally jumps back to the loop,
		    // like a "return".
		    cctx->ctx_had_return = TRUE;
		    ఆపు;
	    స్థితి CMD_break:
		    పంక్తి = compile_break(p, cctx);
		    // "break" unconditionally exits the loop, like a "return".
		    cctx->ctx_had_return = TRUE;
		    ఆపు;

	    స్థితి CMD_try:
		    పంక్తి = compile_try(p, cctx);
		    ఆపు;
	    స్థితి CMD_catch:
		    పంక్తి = compile_catch(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    cctx->ctx_had_throw = FALSE;
		    ఆపు;
	    స్థితి CMD_finally:
		    పంక్తి = compile_finally(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    cctx->ctx_had_throw = FALSE;
		    ఆపు;
	    స్థితి CMD_endtry:
		    పంక్తి = compile_endtry(p, cctx);
		    cctx->ctx_had_throw = FALSE;
		    ఆపు;
	    స్థితి CMD_throw:
		    పంక్తి = compile_throw(p, cctx);
		    cctx->ctx_had_throw = TRUE;
		    ఆపు;

	    స్థితి CMD_eval:
		    పంక్తి = compile_eval(p, cctx);
		    ఆపు;

	    స్థితి CMD_defer:
		    పంక్తి = compile_defer(p, cctx);
		    ఆపు;

#నిర్వచితమైతే HAS_సందేశం_కిటికీ
	    స్థితి CMD_echowindow:
		    {
			దీర్ఘ cmd_లెక్క = get_cmd_లెక్క(పంక్తి, &ea);
			ఒకవేళ (cmd_లెక్క < 0)
			    పంక్తి = ఖాళీ;
			లేకపోతే
			    పంక్తి = compile_mult_సమాసం(p, ea.cmdidx,
							     cmd_లెక్క, cctx);
		    }
		    ఆపు;
#అయితే_ముగింపు
	    స్థితి CMD_echo:
	    స్థితి CMD_echon:
	    స్థితి CMD_echoconsole:
	    స్థితి CMD_echoerr:
	    స్థితి CMD_echomsg:
	    స్థితి CMD_execute:
		    పంక్తి = compile_mult_సమాసం(p, ea.cmdidx, 0, cctx);
		    ఆపు;

	    స్థితి CMD_put:
		    ea.cmd = cmd;
		    పంక్తి = compile_put(p, &ea, cctx, FALSE);
		    ఆపు;

	    స్థితి CMD_iput:
		    ea.cmd = cmd;
		    పంక్తి = compile_put(p, &ea, cctx, TRUE);
		    ఆపు;

	    స్థితి CMD_substitute:
		    ఒకవేళ (check_global_and_subst(ea.cmd, p) == FAIL)
			తిరిగి FAIL;
		    ఒకవేళ (cctx->ctx_skip == SKIP_YES)
			పంక్తి = (char_u *)"";
		    లేకపోతే
		    {
			ea.arg = p;
			పంక్తి = compile_substitute(పంక్తి, &ea, cctx);
		    }
		    ఆపు;

	    స్థితి CMD_redir:
		    ea.arg = p;
		    పంక్తి = compile_redir(పంక్తి, &ea, cctx);
		    ఆపు;

	    స్థితి CMD_cexpr:
	    స్థితి CMD_lexpr:
	    స్థితి CMD_caddexpr:
	    స్థితి CMD_laddexpr:
	    స్థితి CMD_cgetexpr:
	    స్థితి CMD_lgetexpr:
#నిర్వచితమైతే FEAT_QUICKFIX
		    ea.arg = p;
		    పంక్తి = compile_cexpr(పంక్తి, &ea, cctx);
#లేకపోతే
		    ex_ni(&ea);
		    పంక్తి = ఖాళీ;
#అయితే_ముగింపు
		    ఆపు;

	    స్థితి CMD_append:
	    స్థితి CMD_change:
	    స్థితి CMD_చొప్పించు:
	    స్థితి CMD_k:
	    స్థితి CMD_open:
	    స్థితి CMD_t:
	    స్థితి CMD_xit:
		    not_లోపల_vim9(&ea, false);
		    తిరిగి FAIL;

	    స్థితి CMD_కొలత:
		    ఒకవేళ (cctx->ctx_skip != SKIP_YES)
		    {
			semsg(_(e_invalid_command_str), ea.cmd);
			తిరిగి FAIL;
		    }
		    // We don't check for a next command here.
		    పంక్తి = (char_u *)"";
		    ఆపు;

	    స్థితి CMD_lua:
	    స్థితి CMD_mzscheme:
	    స్థితి CMD_perl:
	    స్థితి CMD_py3:
	    స్థితి CMD_python3:
	    స్థితి CMD_python:
	    స్థితి CMD_pythonx:
	    స్థితి CMD_ruby:
	    స్థితి CMD_tcl:
		    ea.arg = p;
		    ఒకవేళ (vim_strchr(పంక్తి, '\n') == ఖాళీ)
			పంక్తి = compile_exec(పంక్తి, &ea, cctx);
		    లేకపోతే
			// heredoc lines have been concatenated with NL
			// characters in get_function_body()
			పంక్తి = compile_script(పంక్తి, cctx);
		    ఆపు;

	    స్థితి CMD_vim9script:
		    ఒకవేళ (cctx->ctx_skip != SKIP_YES)
		    {
			emsg(_(e_vim9script_can_only_be_వాడిన_లోపల_script));
			తిరిగి FAIL;
		    }
		    పంక్తి = (char_u *)"";
		    ఆపు;

	    స్థితి CMD_class:
		    emsg(_(e_class_can_only_be_వాడిన_లోపల_script));
		    తిరిగి FAIL;

	    స్థితి CMD_enum:
		    emsg(_(e_enum_can_only_be_వాడిన_లోపల_script));
		    తిరిగి FAIL;

	    స్థితి CMD_interface:
		    emsg(_(e_interface_can_only_be_వాడిన_లోపల_script));
		    తిరిగి FAIL;

	    స్థితి CMD_type:
		    emsg(_(e_type_can_only_be_వాడిన_లోపల_script));
		    తిరిగి FAIL;

	    స్థితి CMD_global:
		    ఒకవేళ (check_global_and_subst(ea.cmd, p) == FAIL)
			తిరిగి FAIL;
		    // FALLTHROUGH
	    అప్రమేయ:
		    // Not recognized, execute with do_cmdline_cmd().
		    ea.arg = p;
		    పంక్తి = compile_exec(పంక్తి, &ea, cctx);
		    ఆపు;
	}
nextline:
	ఒకవేళ (పంక్తి == ఖాళీ)
	    తిరిగి FAIL;
	పంక్తి = skipwhite(పంక్తి);

	// Undo any command modifiers.
	generate_undo_cmdmods(cctx);

	ఒకవేళ (cctx->ctx_type_దొంతర.ga_len < 0)
	{
	    iemsg("Type stack underflow");
	    తిరిగి FAIL;
	}
    } // END of the loop over all the function body lines.

    తిరిగి OK;
}

/*
 * Returns TRUE if the end of a scope (if, while, for, block) is missing.
 * Called after compiling a def function body.
 */
    స్థిర పూర్ణాంకం
compile_dfunc_scope_చివర_missing(cctx_T *cctx)
{
    ఒకవేళ (cctx->ctx_scope == ఖాళీ)
	తిరిగి FALSE;

    ఎంపిక (cctx->ctx_scope->se_type)
    {
	స్థితి IF_SCOPE:
	    emsg(_(e_missing_endif));
	    ఆపు;
	స్థితి WHILE_SCOPE:
	    emsg(_(e_missing_endwhile));
	    ఆపు;
	స్థితి FOR_SCOPE:
	    emsg(_(e_missing_endfor));
	    ఆపు;
	స్థితి TRY_SCOPE:
	    emsg(_(e_missing_endtry));
	    ఆపు;
	స్థితి ఖండం_SCOPE:
	    // end block scope from :try (maybe)
	    compile_endblock(cctx);
	    ఒకవేళ (cctx->ctx_scope != ఖాళీ
		    && cctx->ctx_scope->se_type == TRY_SCOPE)
		emsg(_(e_missing_endtry));
	    లేకపోతే
		emsg(_(e_missing_rcurly));
	    ఆపు;
	అప్రమేయ:
	    emsg(_(e_missing_rcurly));
    }
    తిరిగి TRUE;
}

/*
 * When compiling a def function, if it doesn't have an explicit return
 * statement, then generate a default return instruction.  For an object
 * constructor, return the object.
 */
    స్థిర పూర్ణాంకం
compile_dfunc_generate_default_return(ufunc_T *ufunc, cctx_T *cctx)
{
    // TODO: if a function ends in "throw" but there was a return elsewhere we
    // should not assume the return type is "void".
    ఒకవేళ (cctx->ctx_had_return || cctx->ctx_had_throw)
	తిరిగి OK;

    ఒకవేళ (ufunc->uf_ret_type->tt_type == VAR_UNKNOWN)
	ufunc->uf_ret_type = &t_void;
    లేకపోతే ఒకవేళ (ufunc->uf_ret_type->tt_type != VAR_VOID
					&& !IS_CONSTRUCTOR_పద్ధతి(ufunc))
    {
	emsg(_(e_missing_return_statement));
	తిరిగి FAIL;
    }

    // Return void if there is no return at the end.
    // For a constructor return the object.
    ఒకవేళ (IS_CONSTRUCTOR_పద్ధతి(ufunc))
    {
	generate_instr(cctx, ISN_RETURN_OBJECT);
	ufunc->uf_ret_type = &ufunc->uf_class->class_object_type;
    }
    లేకపోతే
	generate_instr(cctx, ISN_RETURN_VOID);

    తిరిగి OK;
}

/*
 * Perform the chores after successfully compiling a def function.
 */
    స్థిర శూన్య
compile_dfunc_epilogue(
    cctx_T	*outer_cctx,
    ufunc_T	*ufunc,
    garray_T	*instr,
    cctx_T	*cctx)
{
    dfunc_T	*dfunc;

    dfunc = ((dfunc_T *)def_functions.ga_దత్తాంశం) + ufunc->uf_dfunc_idx;
    dfunc->df_deleted = FALSE;
    dfunc->df_script_seq = current_sctx.sc_seq;

#నిర్వచితమైతే FEAT_PROFILE
    ఒకవేళ (cctx->ctx_compile_type == CT_PROFILE)
    {
	dfunc->df_instr_prof = instr->ga_దత్తాంశం;
	dfunc->df_instr_prof_లెక్క = instr->ga_len;
    }
    లేకపోతే
#అయితే_ముగింపు
	ఒకవేళ (cctx->ctx_compile_type == CT_DEBUG)
	{
	    dfunc->df_instr_debug = instr->ga_దత్తాంశం;
	    dfunc->df_instr_debug_లెక్క = instr->ga_len;
	}
	లేకపోతే
	{
	    dfunc->df_instr = instr->ga_దత్తాంశం;
	    dfunc->df_instr_లెక్క = instr->ga_len;
	}
    dfunc->df_varcount = dfunc->df_var_పేర్లు.ga_len;
    dfunc->df_has_సంవృతం = cctx->ctx_has_సంవృతం;

    ఒకవేళ (cctx->ctx_outer_వాడిన)
    {
	ufunc->uf_flags |= FC_సంవృతం;
	ఒకవేళ (outer_cctx != ఖాళీ)
	    ++outer_cctx->ctx_సంవృతం_లెక్క;
    }

    ufunc->uf_def_స్థితి = UF_COMPILED;
}

/*
 * Perform the cleanup when a def function compilation fails.
 */
    స్థిర శూన్య
compile_dfunc_ufunc_cleanup(
    ufunc_T	*ufunc,
    garray_T	*instr,
    పూర్ణాంకం		new_def_function,
    అక్షరం	*errormsg,
    పూర్ణాంకం		did_emsg_before,
    cctx_T	*cctx)
{
    dfunc_T	*dfunc;

    dfunc = ((dfunc_T *)def_functions.ga_దత్తాంశం) + ufunc->uf_dfunc_idx;

    // Compiling aborted, free the generated instructions.
    clear_instr_ga(instr);
    VIM_CLEAR(dfunc->df_name);
    ga_clear_అక్షరశ్రేణులు(&dfunc->df_var_పేర్లు);

    // If using the last entry in the table and it was added above, we
    // might as well remove it.
    ఒకవేళ (!dfunc->df_deleted && new_def_function
			&& ufunc->uf_dfunc_idx == def_functions.ga_len - 1)
    {
	--def_functions.ga_len;
	ufunc->uf_dfunc_idx = 0;
    }
    ufunc->uf_def_స్థితి = UF_COMPILE_ERROR;

    అయితే (cctx->ctx_scope != ఖాళీ)
	drop_scope(cctx);

    ఒకవేళ (errormsg != ఖాళీ)
	emsg(errormsg);
    లేకపోతే ఒకవేళ (did_emsg == did_emsg_before)
	emsg(_(e_compiling_def_function_failed));
}

/*
 * After ex_function() has collected all the function lines: parse and compile
 * the lines into instructions.
 * Adds the function to "def_functions".
 * When "check_return_type" is set then set ufunc->uf_ret_type to the type of
 * the return statement (used for lambda).  When uf_ret_type is already set
 * then check that it matches.
 * When "profiling" is true add ISN_PROF_START instructions.
 * "outer_cctx" is set for a nested function.
 * This can be used recursively through compile_lambda(), which may reallocate
 * "def_functions".
 * Returns OK or FAIL.
 */
    పూర్ణాంకం
compile_def_function(
	ufunc_T		*ufunc,
	పూర్ణాంకం		check_return_type,
	compiletype_T   compile_type,
	cctx_T		*outer_cctx)
{
    garray_T	పంక్తులు_గమ్యం_free;
    అక్షరం	*errormsg = ఖాళీ;	// error message
    cctx_T	cctx;
    garray_T	*instr;
    పూర్ణాంకం		did_emsg_before = did_emsg;
    పూర్ణాంకం		did_emsg_silent_before = did_emsg_silent;
    పూర్ణాంకం		ret = FAIL;
    sctx_T	save_current_sctx = current_sctx;
    పూర్ణాంకం		save_estack_compiling = estack_compiling;
    పూర్ణాంకం		save_cmod_flags = cmdmod.cmod_flags;
    పూర్ణాంకం		do_estack_నెట్టు;
    పూర్ణాంకం		new_def_function = FALSE;

    // allocated lines are freed at the end
    ga_init2(&పంక్తులు_గమ్యం_free, పరిమాణం(char_u *), 50);

    // Initialize the ufunc and the compilation context
    ఒకవేళ (compile_dfunc_ufunc_init(ufunc, outer_cctx, compile_type,
						&new_def_function) == FAIL)
	తిరిగి FAIL;

    compile_dfunc_cctx_init(&cctx, outer_cctx, ufunc, compile_type);

    instr = &cctx.ctx_instr;

    // Set the context to the function, it may be compiled when called from
    // another script.  Set the script version to the most modern one.
    // The line number will be set in next_line_from_context().
    current_sctx = ufunc->uf_script_ctx;
    current_sctx.sc_సంస్కరణ = SCRIPT_సంస్కరణ_VIM9;

    // Don't use the flag from ":legacy" here.
    cmdmod.cmod_flags &= ~CMOD_LEGACY;

    // Make sure error messages are OK.
    do_estack_నెట్టు = !estack_శిఖరం_is_ufunc(ufunc, 1);
    ఒకవేళ (do_estack_నెట్టు)
	estack_నెట్టు_ufunc(ufunc, 1);
    estack_compiling = TRUE;

    // Make sure arguments don't shadow variables in the context
    ఒకవేళ (check_వాదనలు_shadowing(ufunc, &cctx) == FAIL)
	వెళ్ళు erret;

    // For an object method and a constructor generate instructions to
    // initialize "this" and the object variables.
    ఒకవేళ (ufunc->uf_flags & (FC_OBJECT|FC_NEW))
	ఒకవేళ (obj_పద్ధతి_prologue(ufunc, &cctx) == FAIL)
	    వెళ్ళు erret;

    ఒకవేళ (ufunc->uf_def_వాదనలు.ga_len > 0)
	ఒకవేళ (compile_def_function_default_వాదనలు(ufunc, instr, &cctx) == FAIL)
	    వెళ్ళు erret;
    ufunc->uf_వాదనలు_visible = ufunc->uf_వాదనలు.ga_len;

    // Compiling an abstract method or a function in an interface is done to
    // get the function type.  No code is actually compiled.
    ఒకవేళ (ufunc->uf_class != ఖాళీ && (IS_INTERFACE(ufunc->uf_class)
						|| IS_ABSTRACT_పద్ధతి(ufunc)))
    {
	ufunc->uf_def_స్థితి = UF_NOT_COMPILED;
	ret = OK;
	వెళ్ళు erret;
    }

    // compile the function body
    ఒకవేళ (compile_def_function_body(ufunc->uf_పంక్తులు.ga_len, check_return_type,
				&పంక్తులు_గమ్యం_free, &errormsg, &cctx) == FAIL)
	వెళ్ళు erret;

    ఒకవేళ (compile_dfunc_scope_చివర_missing(&cctx))
	వెళ్ళు erret;

    ఒకవేళ (compile_dfunc_generate_default_return(ufunc, &cctx) == FAIL)
	వెళ్ళు erret;

    // When compiled with ":silent!" and there was an error don't consider the
    // function compiled.
    ఒకవేళ (emsg_silent == 0 || did_emsg_silent == did_emsg_silent_before)
	compile_dfunc_epilogue(outer_cctx, ufunc, instr, &cctx);

    ret = OK;

erret:
    ఒకవేళ (ufunc->uf_def_స్థితి == UF_COMPILING)
    {
	// compilation failed. do cleanup.
	compile_dfunc_ufunc_cleanup(ufunc, instr, new_def_function,
				    errormsg, did_emsg_before, &cctx);
    }

    ఒకవేళ (cctx.ctx_redir_lhs.lhs_name != ఖాళీ)
    {
	ఒకవేళ (ret == OK)
	{
	    emsg(_(e_missing_redir_చివర));
	    ret = FAIL;
	}
	vim_free(cctx.ctx_redir_lhs.lhs_name);
	vim_free(cctx.ctx_redir_lhs.lhs_సంపూర్ణ);
    }

    current_sctx = save_current_sctx;
    estack_compiling = save_estack_compiling;
    cmdmod.cmod_flags =	save_cmod_flags;
    ఒకవేళ (do_estack_నెట్టు)
	estack_తీయి();

    ga_clear_అక్షరశ్రేణులు(&పంక్తులు_గమ్యం_free);
    free_locals(&cctx);
    ga_clear(&cctx.ctx_type_దొంతర);
    తిరిగి ret;
}

    శూన్య
సమితి_function_type(ufunc_T *ufunc)
{
    పూర్ణాంకం varargs = ufunc->uf_va_name != ఖాళీ;
    పూర్ణాంకం argcount = ufunc->uf_వాదనలు.ga_len;

    // Create a type for the function, with the return type and any
    // argument types.
    // A vararg is included in uf_args.ga_len but not in uf_arg_types.
    // The type is included in "tt_args".
    ఒకవేళ (argcount > 0 || varargs)
    {
	ఒకవేళ (ufunc->uf_type_list.ga_itemsize == 0)
	    ga_init2(&ufunc->uf_type_list, పరిమాణం(type_T *), 10);
	ufunc->uf_func_type = alloc_func_type(ufunc->uf_ret_type,
					   argcount, &ufunc->uf_type_list);
	// Add argument types to the function type.
	ఒకవేళ (func_type_కూడు_arg_types(ufunc->uf_func_type,
				    argcount + varargs,
				    &ufunc->uf_type_list) == FAIL)
	    తిరిగి;
	ufunc->uf_func_type->tt_argcount = argcount + varargs;
	ufunc->uf_func_type->tt_min_argcount =
				      argcount - ufunc->uf_def_వాదనలు.ga_len;
	ఒకవేళ (ufunc->uf_arg_types == ఖాళీ)
	{
	    పూర్ణాంకం i;

	    // lambda does not have argument types.
	    కొరకు (i = 0; i < argcount; ++i)
		ufunc->uf_func_type->tt_వాదనలు[i] = &t_any;
	}
	లేకపోతే
	    mch_memmove(ufunc->uf_func_type->tt_వాదనలు,
			 ufunc->uf_arg_types, పరిమాణం(type_T *) * argcount);
	ఒకవేళ (varargs)
	{
	    ufunc->uf_func_type->tt_వాదనలు[argcount] =
		   ufunc->uf_va_type == ఖాళీ ? &t_list_any : ufunc->uf_va_type;
	    ufunc->uf_func_type->tt_flags = TTFLAG_VARARGS;
	}
    }
    లేకపోతే
	// No arguments, can use a predefined type.
	ufunc->uf_func_type = get_func_type(ufunc->uf_ret_type,
					   argcount, &ufunc->uf_type_list);
}

/*
 * Free all instructions for "dfunc" except df_name.
 */
    స్థిర శూన్య
delete_def_function_contents(dfunc_T *dfunc, పూర్ణాంకం mark_deleted)
{
    పూర్ణాంకం idx;

    // In same cases the instructions may refer to a class in which the
    // function is defined and unreferencing the class may call back here
    // recursively.  Set the df_delete_busy to avoid problems.
    ఒకవేళ (dfunc->df_delete_busy)
	తిరిగి;
    dfunc->df_delete_busy = TRUE;

    ga_clear(&dfunc->df_def_వాదనలు_isn);
    ga_clear_అక్షరశ్రేణులు(&dfunc->df_var_పేర్లు);

    ఒకవేళ (dfunc->df_instr != ఖాళీ)
    {
	కొరకు (idx = 0; idx < dfunc->df_instr_లెక్క; ++idx)
	    delete_instr(dfunc->df_instr + idx);
	VIM_CLEAR(dfunc->df_instr);
    }
    ఒకవేళ (dfunc->df_instr_debug != ఖాళీ)
    {
	కొరకు (idx = 0; idx < dfunc->df_instr_debug_లెక్క; ++idx)
	    delete_instr(dfunc->df_instr_debug + idx);
	VIM_CLEAR(dfunc->df_instr_debug);
    }
#నిర్వచితమైతే FEAT_PROFILE
    ఒకవేళ (dfunc->df_instr_prof != ఖాళీ)
    {
	కొరకు (idx = 0; idx < dfunc->df_instr_prof_లెక్క; ++idx)
	    delete_instr(dfunc->df_instr_prof + idx);
	VIM_CLEAR(dfunc->df_instr_prof);
    }
#అయితే_ముగింపు

    ఒకవేళ (mark_deleted)
	dfunc->df_deleted = TRUE;
    ఒకవేళ (dfunc->df_ufunc != ఖాళీ)
	dfunc->df_ufunc->uf_def_స్థితి = UF_NOT_COMPILED;

    dfunc->df_delete_busy = FALSE;
}

/*
 * When a user function is deleted, clear the contents of any associated def
 * function, unless another user function still uses it.
 * The position in def_functions can be re-used.
 */
    శూన్య
unlink_def_function(ufunc_T *ufunc)
{
    ఒకవేళ (ufunc->uf_dfunc_idx <= 0 || def_functions.ga_దత్తాంశం == ఖాళీ)
	తిరిగి;

    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_దత్తాంశం)
	+ ufunc->uf_dfunc_idx;

    ఒకవేళ (--dfunc->df_refcount <= 0)
	delete_def_function_contents(dfunc, TRUE);
    ufunc->uf_def_స్థితి = UF_NOT_COMPILED;
    ufunc->uf_dfunc_idx = 0;
    ఒకవేళ (dfunc->df_ufunc == ufunc)
	dfunc->df_ufunc = ఖాళీ;
}

/*
 * Used when a user function refers to an existing dfunc.
 */
    శూన్య
link_def_function(ufunc_T *ufunc)
{
    ఒకవేళ (ufunc->uf_dfunc_idx <= 0)
	తిరిగి;

    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_దత్తాంశం)
	+ ufunc->uf_dfunc_idx;

    ++dfunc->df_refcount;
}

#ఒకవేళ defined(EXITFREE)
/*
 * Free all functions defined with ":def".
 */
    శూన్య
free_def_functions(శూన్య)
{
    పూర్ణాంకం idx;

    కొరకు (idx = 0; idx < def_functions.ga_len; ++idx)
    {
	dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_దత్తాంశం) + idx;

	delete_def_function_contents(dfunc, TRUE);
	vim_free(dfunc->df_name);
    }

    ga_clear(&def_functions);
}
#అయితే_ముగింపు


#అయితే_ముగింపు // FEAT_EVAL
