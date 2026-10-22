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

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USING_FLOAT_STUFF
#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ defined(FEAT_EVAL)

// Functions defined with :def are stored in this growarray.
// They are never removed, so that they can be found by index.
// Deleted functions have the df_deleted flag set.
garray_T def_functions = {0, 0, ਆਕਾਰ(dfunc_T), 50, ਨਿਰਾ};

ਸਥਿਰ ਖਾਲੀ delete_def_function_contents(dfunc_T *dfunc, ਪੂਰਨ mark_deleted);

/*
 * Lookup variable "name" in the local scope and return it in "lvar".
 * "lvar->lv_from_outer" is incremented accordingly.
 * If "lvar" is NULL only check if the variable can be found.
 * Return FAIL if not found.
 */
    ਪੂਰਨ
lookup_local(char_u *name, size_t len, lvar_T *lvar, cctx_T *cctx)
{
    ਪੂਰਨ	    idx;
    lvar_T  *lvp;

    ਜੇ (len == 0)
	ਵਾਪਸ FAIL;

    ਜੇ (((len == 4 && STRNCMP(name, "this", 4) == 0)
		|| (len == 5 && STRNCMP(name, "super", 5) == 0))
	    && cctx->ctx_ufunc != ਨਿਰਾ
	    && (cctx->ctx_ufunc->uf_flags & (FC_OBJECT|FC_NEW)))
    {
	ਪੂਰਨ is_super = *name == 's';
	ਜੇ (is_super)
	{
	    ਜੇ (name[5] != '.')
	    {
		emsg(_(e_super_must_be_followed_by_dot));
		ਵਾਪਸ FAIL;
	    }
	    ਜੇ (cctx->ctx_ufunc->uf_class != ਨਿਰਾ
		    && cctx->ctx_ufunc->uf_class->class_extends == ਨਿਰਾ)
	    {
		emsg(_(e_using_super_not_ਵਿੱਚ_ਬੱਚਾ_class));
		ਵਾਪਸ FAIL;
	    }
	}
	ਜੇ (lvar != ਨਿਰਾ)
	{
	    CLEAR_ਸੂਚਕ(lvar);
	    lvar->lv_loop_ਡੂੰਘਾਈ = -1;
	    lvar->lv_name = (char_u *)(is_super ? "super" : "this");
	    ਜੇ (cctx->ctx_ufunc->uf_class != ਨਿਰਾ)
	    {
		lvar->lv_type = &cctx->ctx_ufunc->uf_class->class_object_type;
		ਜੇ (is_super)
		{
		    type_T *type = get_type_ptr(cctx->ctx_type_list);

		    ਜੇ (type != ਨਿਰਾ)
		    {
			*type = *lvar->lv_type;
			lvar->lv_type = type;
			type->tt_flags |= TTFLAG_SUPER;
		    }
		}
	    }
	}
	ਵਾਪਸ OK;
    }

    // Find local in current function scope.
    ਲਈ (idx = 0; idx < cctx->ctx_locals.ga_len; ++idx)
    {
	lvp = ((lvar_T *)cctx->ctx_locals.ga_ਅੰਕੜੇ) + idx;
	ਜੇ (lvp->lv_name != ਨਿਰਾ
		&& STRNCMP(name, lvp->lv_name, len) == 0
					       && STRLEN(lvp->lv_name) == len)
	{
	    ਜੇ (lvar != ਨਿਰਾ)
	    {
		*lvar = *lvp;
		lvar->lv_from_outer = 0;
		// If the variable was declared inside a loop set
		// lvar->lv_loop_idx and lvar->lv_loop_depth.
		get_loop_var_idx(cctx, idx, lvar);
	    }
	    ਵਾਪਸ OK;
	}
    }

    // Find local in outer function scope.
    ਜੇ (cctx->ctx_outer != ਨਿਰਾ)
    {
	ਜੇ (lookup_local(name, len, lvar, cctx->ctx_outer) == OK)
	{
	    ਜੇ (lvar != ਨਿਰਾ)
	    {
		cctx->ctx_outer_ਵਰਤਿਆ = TRUE;
		++lvar->lv_from_outer;
	    }
	    ਵਾਪਸ OK;
	}
    }

    ਵਾਪਸ FAIL;
}

/*
 * Lookup an argument in the current function and an enclosing function.
 * Returns the argument index in "idxp"
 * Returns the argument type in "type"
 * Sets "gen_load_outer" to TRUE if found in outer scope.
 * Returns OK when found, FAIL otherwise.
 */
    ਪੂਰਨ
arg_exists(
	char_u	*name,
	size_t	len,
	ਪੂਰਨ	*idxp,
	type_T	**type,
	ਪੂਰਨ	*gen_ਲੱਦੋ_outer,
	cctx_T	*cctx)
{
    ਪੂਰਨ	    idx;
    char_u  *va_name;

    ਜੇ (len == 0)
	ਵਾਪਸ FAIL;
    ਲਈ (idx = 0; idx < cctx->ctx_ufunc->uf_ਦਲੀਲਾਂ_visible; ++idx)
    {
	char_u *arg = FUNCARG(cctx->ctx_ufunc, idx);

	ਜੇ (STRNCMP(name, arg, len) == 0 && arg[len] == NUL)
	{
	    ਜੇ (idxp != ਨਿਰਾ)
	    {
		// Arguments are located above the frame pointer.  One further
		// if there is a vararg argument
		*idxp = idx - (cctx->ctx_ufunc->uf_ਦਲੀਲਾਂ.ga_len
							    + ਢੇਰ_ਚੌਖਟਾ_ਮਾਪ)
			      + (cctx->ctx_ufunc->uf_va_name != ਨਿਰਾ ? -1 : 0);

		ਜੇ (cctx->ctx_ufunc->uf_arg_types != ਨਿਰਾ)
		    *type = cctx->ctx_ufunc->uf_arg_types[idx];
		ਨਹੀਂਤਾਂ
		    *type = &t_any;
	    }
	    ਵਾਪਸ OK;
	}
    }

    va_name = cctx->ctx_ufunc->uf_va_name;
    ਜੇ (va_name != ਨਿਰਾ
		    && STRNCMP(name, va_name, len) == 0 && va_name[len] == NUL)
    {
	ਜੇ (idxp != ਨਿਰਾ)
	{
	    // varargs is always the last argument
	    *idxp = -ਢੇਰ_ਚੌਖਟਾ_ਮਾਪ - 1;
	    *type = cctx->ctx_ufunc->uf_va_type;
	}
	ਵਾਪਸ OK;
    }

    ਜੇ (cctx->ctx_outer != ਨਿਰਾ)
    {
	// Lookup the name for an argument of the outer function.
	ਜੇ (arg_exists(name, len, idxp, type, gen_ਲੱਦੋ_outer, cctx->ctx_outer)
									 == OK)
	{
	    ਜੇ (gen_ਲੱਦੋ_outer != ਨਿਰਾ)
	    {
		++*gen_ਲੱਦੋ_outer;
		cctx->ctx_outer_ਵਰਤਿਆ = TRUE;
	    }
	    ਵਾਪਸ OK;
	}
    }

    ਵਾਪਸ FAIL;
}

/*
 * Lookup a script-local variable in the current script, possibly defined in a
 * block that contains the function "cctx->ctx_ufunc".
 * "cctx" is NULL at the script level, "cstack" is NULL in a function.
 * If "len" is <= 0 "name" must be NUL terminated.
 * Return NULL when not found.
 */
    ਸਥਿਰ sallvar_T *
ਲੱਭੋ_script_var(char_u *name, size_t len, cctx_T *cctx, cstack_T *cstack)
{
    scriptitem_T    *si = SCRIPT_ਵਸਤੂ(current_sctx.sc_sid);
    hashitem_T	    *hi;
    ਪੂਰਨ		    cc;
    sallvar_T	    *sav;
    ufunc_T	    *ufunc;

    // Find the list of all script variables with the right name.
    ਜੇ (len > 0)
    {
	cc = name[len];
	name[len] = NUL;
    }
    hi = hash_ਲੱਭੋ(&si->sn_all_vars.dv_hashtab, name);
    ਜੇ (len > 0)
	name[len] = cc;
    ਜੇ (HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	ਵਾਪਸ ਨਿਰਾ;

    sav = HI2SAV(hi);
    ਜੇ (sav->sav_ਖੰਡ_id == 0)
	// variable defined in the top script scope is always visible
	ਵਾਪਸ sav;

    ਜੇ (cctx == ਨਿਰਾ)
    {
	ਜੇ (cstack == ਨਿਰਾ)
	    ਵਾਪਸ ਨਿਰਾ;

	// Not in a function scope, find variable with block ID equal to or
	// smaller than the current block id.  Use "cstack" to go up the block
	// scopes.
	ਜਦੋਂ (sav != ਨਿਰਾ)
	{
	    ਪੂਰਨ idx;

	    ਲਈ (idx = cstack->cs_idx; idx >= 0; --idx)
		ਜੇ (cstack->cs_ਖੰਡ_id[idx] == sav->sav_ਖੰਡ_id)
		    ਰੋਕੋ;
	    ਜੇ (idx >= 0)
		ਰੋਕੋ;
	    sav = sav->sav_ਅਗਲਾ;
	}
	ਵਾਪਸ sav;
    }

    // Go over the variables with this name and find one that was visible
    // from the function.
    ufunc = cctx->ctx_ufunc;
    ਜਦੋਂ (sav != ਨਿਰਾ)
    {
	ਪੂਰਨ idx;

	ਜੇ (ufunc->uf_ਖੰਡ_ਡੂੰਘਾਈ == 0 && sav->sav_ਖੰਡ_id == 0)
	    // If the function was defined at the script level (not inside a
	    // block), script-scope variables are always visible.
	    ਵਾਪਸ sav;

	// Go over the blocks that this function was defined in.  If the
	// variable block ID matches it was visible to the function.
	ਲਈ (idx = 0; idx < ufunc->uf_ਖੰਡ_ਡੂੰਘਾਈ; ++idx)
	    ਜੇ (ufunc->uf_ਖੰਡ_ids[idx] == sav->sav_ਖੰਡ_id)
		ਵਾਪਸ sav;
	sav = sav->sav_ਅਗਲਾ;
    }

    // Not found, variable was not visible.
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * If "name" can be found in the current script set it's "block_id".
 */
    ਖਾਲੀ
update_script_var_ਖੰਡ_id(char_u *name, ਪੂਰਨ ਖੰਡ_id)
{
    scriptitem_T    *si = SCRIPT_ਵਸਤੂ(current_sctx.sc_sid);
    hashitem_T	    *hi;
    sallvar_T	    *sav;

    hi = hash_ਲੱਭੋ(&si->sn_all_vars.dv_hashtab, name);
    ਜੇ (HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	ਵਾਪਸ;
    sav = HI2SAV(hi);
    sav->sav_ਖੰਡ_id = ਖੰਡ_id;
}

/*
 * Lookup a variable (without s: prefix) in the current script.
 * "cctx" is NULL at the script level, "cstack" is NULL in a function.
 * Returns OK or FAIL.
 */
    ਪੂਰਨ
script_var_exists(char_u *name, size_t len, cctx_T *cctx, cstack_T *cstack)
{
    ਜੇ (current_sctx.sc_sid <= 0)
	ਵਾਪਸ FAIL;
    ਜੇ (current_script_is_vim9())
    {
	// Check script variables that were visible where the function was
	// defined.
	ਜੇ (ਲੱਭੋ_script_var(name, len, cctx, cstack) != ਨਿਰਾ)
	    ਵਾਪਸ OK;
    }
    ਨਹੀਂਤਾਂ
    {
	hashtab_T	*ht = &SCRIPT_VARS(current_sctx.sc_sid);
	dictitem_T	*di;
	ਪੂਰਨ		cc;

	// Check script variables that are currently visible
	cc = name[len];
	name[len] = NUL;
	di = ਲੱਭੋ_var_ਵਿੱਚ_ht(ht, 0, name, TRUE);
	name[len] = cc;
	ਜੇ (di != ਨਿਰਾ)
	    ਵਾਪਸ OK;
    }

    ਵਾਪਸ FAIL;
}

/*
 * Returns the index of a class method or class variable with name "name"
 * accessible in the currently compiled function.
 * If "cl_ret" is not NULL set it to the class.
 * Otherwise return -1.
 */
    ਸਥਿਰ ਪੂਰਨ
cctx_class_midx(
    cctx_T  *cctx,
    ਪੂਰਨ	    is_ਵਿਧੀ,
    char_u  *name,
    size_t  len,
    class_T **cl_ret)
{
    ਜੇ (cctx == ਨਿਰਾ || cctx->ctx_ufunc == ਨਿਰਾ
	    || cctx->ctx_ufunc->uf_class == ਨਿਰਾ
	    || cctx->ctx_ufunc->uf_defclass == ਨਿਰਾ)
	ਵਾਪਸ -1;

    // Search for the class method or variable in the class where the calling
    // function is defined.
    class_T *cl = cctx->ctx_ufunc->uf_defclass;
    ਪੂਰਨ m_idx = is_ਵਿਧੀ ? class_ਵਿਧੀ_idx(cl, name, len)
					: class_member_idx(cl, name, len);
    ਜੇ (m_idx < 0)
    {
	cl = cl->class_extends;
	ਜਦੋਂ (cl != ਨਿਰਾ)
	{
	    m_idx = is_ਵਿਧੀ ? class_ਵਿਧੀ_idx(cl, name, len)
					: class_member_idx(cl, name, len);
	    ਜੇ (m_idx >= 0)
		ਰੋਕੋ;
	    cl = cl->class_extends;
	}
    }

    ਜੇ (m_idx >= 0)
    {
	ਜੇ (cl_ret != ਨਿਰਾ)
	    *cl_ret = cl;
    }

    ਵਾਪਸ m_idx;
}

/*
 * Returns the index of a class method with name "name" accessible in the
 * currently compiled function.  Returns -1 if not found.  The class where the
 * method is defined is returned in "cl_ret".
 */
    ਪੂਰਨ
cctx_class_ਵਿਧੀ_idx(
    cctx_T  *cctx,
    char_u  *name,
    size_t  len,
    class_T **cl_ret)
{
    ਵਾਪਸ cctx_class_midx(cctx, TRUE, name, len, cl_ret);
}

/*
 * Returns the index of a class variable with name "name" accessible in the
 * currently compiled function.  Returns -1 if not found.  The class where the
 * variable is defined is returned in "cl_ret".
 */
    ਪੂਰਨ
cctx_class_member_idx(
    cctx_T  *cctx,
    char_u  *name,
    size_t  len,
    class_T **cl_ret)
{
    ਵਾਪਸ cctx_class_midx(cctx, FALSE, name, len, cl_ret);
}

/*
 * Return TRUE if "name" is a local variable, argument, script variable or
 * imported.  Also if "name" is "this" and in a class method.
 */
    ਸਥਿਰ ਪੂਰਨ
variable_exists(char_u *name, size_t len, cctx_T *cctx)
{
    ਵਾਪਸ (cctx != ਨਿਰਾ
		&& (lookup_local(name, len, ਨਿਰਾ, cctx) == OK
		    || arg_exists(name, len, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, cctx) == OK
		    || (len == 4
			&& cctx->ctx_ufunc != ਨਿਰਾ
			&& (cctx->ctx_ufunc->uf_flags & (FC_OBJECT|FC_NEW))
			&& STRNCMP(name, "this", 4) == 0)))
	    || script_var_exists(name, len, cctx, ਨਿਰਾ) == OK
	    || cctx_class_member_idx(cctx, name, len, ਨਿਰਾ) >= 0
	    || ਲੱਭੋ_imported(name, len, FALSE) != ਨਿਰਾ;
}

/*
 * Return TRUE if "name" is a local variable, argument, script variable,
 * imported or function.  Or commands are being skipped, a declaration may have
 * been skipped then.
 */
    ਸਥਿਰ ਪੂਰਨ
ਵਸਤੂ_exists(char_u *name, size_t len, ਪੂਰਨ cmd ਅਣਵਰਤਿਆ, cctx_T *cctx)
{
    ਵਾਪਸ variable_exists(name, len, cctx);
}

/*
 * Check if "p[len]" is already defined, either in script "import_sid" or in
 * compilation context "cctx".
 * "cctx" is NULL at the script level, "cstack" is NULL in a function.
 * Does not check the global namespace.
 * If "is_arg" is TRUE the error message is for an argument name.
 * Return FAIL and give an error if it defined.
 */
    ਪੂਰਨ
check_defined(
	char_u	    *p,
	size_t	    len,
	cctx_T	    *cctx,
	cstack_T    *cstack,
	ਪੂਰਨ	    is_arg)
{
    ਪੂਰਨ		c = p[len];
    ufunc_T	*ufunc = ਨਿਰਾ;

    // underscore argument is OK
    ਜੇ (len == 1 && *p == '_')
	ਵਾਪਸ OK;

    ਜੇ (script_var_exists(p, len, cctx, cstack) == OK)
    {
	ਜੇ (is_arg)
	    semsg(_(e_ਦਲੀਲ_already_declared_ਵਿੱਚ_script_str), p);
	ਨਹੀਂਤਾਂ
	    semsg(_(e_variable_already_declared_ਵਿੱਚ_script_str), p);
	ਵਾਪਸ FAIL;
    }

    ਜੇ (cctx_class_member_idx(cctx, p, len, ਨਿਰਾ) >= 0)
    {
	ਜੇ (is_arg)
	    semsg(_(e_ਦਲੀਲ_already_declared_ਵਿੱਚ_class_str), p);
	ਨਹੀਂਤਾਂ
	    semsg(_(e_variable_already_declared_ਵਿੱਚ_class_str), p);
	ਵਾਪਸ FAIL;
    }

    p[len] = NUL;
    ਜੇ ((cctx != ਨਿਰਾ
		&& (lookup_local(p, len, ਨਿਰਾ, cctx) == OK
		    || arg_exists(p, len, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, cctx) == OK))
	    || ਲੱਭੋ_imported(p, len, FALSE) != ਨਿਰਾ
	    || (ufunc = ਲੱਭੋ_func_even_dead(p, 0)) != ਨਿਰਾ)
    {
	// A local or script-local function can shadow a global function.
	ਜੇ (ufunc == ਨਿਰਾ || ((ufunc->uf_flags & FC_DEAD) == 0
		    && (!func_is_global(ufunc)
					     || (p[0] == 'g' && p[1] == ':'))))
	{
	    ਜੇ (is_arg)
		semsg(_(e_ਦਲੀਲ_name_shadows_existing_variable_str), p);
	    ਨਹੀਂਤਾਂ
		semsg(_(e_name_already_defined_str), p);
	    p[len] = c;
	    ਵਾਪਸ FAIL;
	}
    }
    p[len] = c;
    ਵਾਪਸ OK;
}


/*
 * Return TRUE if "actual" could be "expected" and a runtime typecheck is to be
 * used.  Return FALSE if the types will never match.
 */
    ਸਥਿਰ ਪੂਰਨ
use_typecheck(type_T *actual, type_T *expected)
{
    ਜੇ (actual->tt_type == VAR_ANY
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
	ਵਾਪਸ TRUE;
    ਜੇ (actual->tt_type == VAR_OBJECT && expected->tt_type == VAR_OBJECT)
	ਵਾਪਸ TRUE;
    ਜੇ ((actual->tt_type == VAR_LIST
		|| actual->tt_type == VAR_TUPLE
		|| actual->tt_type == VAR_DICT)
	    && actual->tt_type == expected->tt_type)
	// This takes care of a nested list or dict.
	ਵਾਪਸ use_typecheck(actual->tt_member, expected->tt_member);
    ਵਾਪਸ FALSE;
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
    ਪੂਰਨ
need_type_where(
	type_T	*actual,
	type_T	*expected,
	ਪੂਰਨ	typechk_flags,	// acceptable types (type check flags)
	ਪੂਰਨ	ਵਿੱਥ,
	where_T	where,
	cctx_T	*cctx,
	ਪੂਰਨ	silent,
	ਪੂਰਨ	actual_is_const)
{
    ਪੂਰਨ ret;

    ਜੇ (expected->tt_type != VAR_CLASS && expected->tt_type != VAR_TYPEALIAS)
    {
	ਜੇ (check_type_is_ਮੁੱਲ(actual) == FAIL)
	    ਵਾਪਸ FAIL;
    }

    ਜੇ (expected == &t_bool && actual != &t_bool
					&& (actual->tt_flags & TTFLAG_BOOL_OK))
    {
	// Using "0", "1" or the result of an expression with "&&" or "||" as a
	// boolean is OK but requires a conversion.
	generate_2BOOL(cctx, FALSE, ਵਿੱਥ);
	ਵਾਪਸ OK;
    }

    ret = check_type_maybe(expected, actual, FALSE, where);
    ਜੇ (ret == OK)
	ਵਾਪਸ OK;

    // If actual a constant a runtime check makes no sense.  If it's
    // null_function it is OK.
    ਜੇ (actual_is_const && ret == MAYBE && actual == &t_func_unknown)
	ਵਾਪਸ OK;

    // If the actual type can be the expected type add a runtime check.
    ਜੇ (!actual_is_const && ret == MAYBE && use_typecheck(actual, expected))
    {
	generate_TYPECHECK(cctx, expected, typechk_flags, ਵਿੱਥ,
		where.wt_ਕਿਸਮ == WT_VARIABLE, where.wt_ਸੂਚਕਾਂਕ);
	ਵਾਪਸ OK;
    }

    ਜੇ (!silent)
	type_mismatch_where(expected, actual, where);
    ਵਾਪਸ FAIL;
}

    ਪੂਰਨ
need_type(
	type_T	*actual,
	type_T	*expected,
	ਪੂਰਨ	typechk_flags,	// acceptable types (type check flags)
	ਪੂਰਨ	ਵਿੱਥ,
	ਪੂਰਨ	arg_idx,
	cctx_T	*cctx,
	ਪੂਰਨ	silent,
	ਪੂਰਨ	actual_is_const)
{
    where_T where = WHERE_INIT;

    ਜੇ (arg_idx > 0)
    {
	where.wt_ਸੂਚਕਾਂਕ = arg_idx;
	where.wt_ਕਿਸਮ = WT_ਦਲੀਲ;
    }
    ਵਾਪਸ need_type_where(actual, expected, typechk_flags, ਵਿੱਥ, where,
						cctx, silent, actual_is_const);
}

/*
 * Set type of variable "lvar" to "type".  If the variable is a constant then
 * the type gets TTFLAG_CONST.
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_var_type(lvar_T *lvar, type_T *type_arg, cctx_T *cctx)
{
    type_T	*type = type_arg;

    ਜੇ (lvar->lv_const == ਸੌਂਪੋ_CONST && (type->tt_flags & TTFLAG_CONST) == 0)
    {
	ਜੇ (type->tt_flags & TTFLAG_STATIC)
	    // entry in static_types[] is followed by const type
	    type = type + 1;
	ਨਹੀਂਤਾਂ
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
	ਪੂਰਨ	ਸੌਂਪੋ,
	type_T	*type)
{
    lvar_T  *lvar;
    dfunc_T *dfunc;

    ਜੇ (arg_exists(name, len, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, cctx) == OK)
    {
	emsg_namelen(_(e_str_is_ਵਰਤਿਆ_as_ਦਲੀਲ), name, (ਪੂਰਨ)len);
	ਵਾਪਸ ਨਿਰਾ;
    }

    ਜੇ (GA_GROW_FAILS(&cctx->ctx_locals, 1))
	ਵਾਪਸ ਨਿਰਾ;
    lvar = ((lvar_T *)cctx->ctx_locals.ga_ਅੰਕੜੇ) + cctx->ctx_locals.ga_len++;
    CLEAR_ਸੂਚਕ(lvar);

    // Every local variable uses the next entry on the stack.  We could re-use
    // the last ones when leaving a scope, but then variables used in a closure
    // might get overwritten.  To keep things simple do not re-use stack
    // entries.  This is less efficient, but memory is cheap these days.
    dfunc = ((dfunc_T *)def_functions.ga_ਅੰਕੜੇ) + cctx->ctx_ufunc->uf_dfunc_idx;
    lvar->lv_idx = dfunc->df_var_ਨਾਂ.ga_len;

    lvar->lv_name = vim_strnsave(name, len == 0 ? STRLEN(name) : len);
    lvar->lv_const = ਸੌਂਪੋ;
    ਜੇ (type == &t_unknown || type == &t_any)
	// type not known yet, may be inferred from RHS
	lvar->lv_type = type;
    ਨਹੀਂਤਾਂ
	// may use TTFLAG_CONST
	ਸੈੱਟ_var_type(lvar, type, cctx);

    // Remember the name for debugging.
    ਜੇ (GA_GROW_FAILS(&dfunc->df_var_ਨਾਂ, 1))
	ਵਾਪਸ ਨਿਰਾ;
    ((char_u **)dfunc->df_var_ਨਾਂ.ga_ਅੰਕੜੇ)[lvar->lv_idx] =
						    vim_strsave(lvar->lv_name);
    ++dfunc->df_var_ਨਾਂ.ga_len;

    ਵਾਪਸ lvar;
}

/*
 * If "check_writable" is ASSIGN_CONST give an error if the variable was
 * defined with :final or :const, if "check_writable" is ASSIGN_FINAL give an
 * error if the variable was defined with :const.
 */
    ਸਥਿਰ ਪੂਰਨ
check_ਵਸਤੂ_writable(svar_T *sv, ਪੂਰਨ check_writable, char_u *name)
{
    ਜੇ ((check_writable == ਸੌਂਪੋ_CONST && sv->sv_const != 0)
	    || (check_writable == ਸੌਂਪੋ_FINAL
					      && sv->sv_const == ਸੌਂਪੋ_CONST))
    {
	semsg(_(e_cannot_change_readonly_variable_str), name);
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * Find "name" in script-local items of script "sid".
 * Pass "check_writable" to check_item_writable().
 * "cctx" is NULL at the script level, "cstack" is NULL in a function.
 * Returns the index in "sn_var_vals" if found.
 * If found but not in "sn_var_vals" returns -1.
 * If not found or the variable is not writable returns -2.
 */
    ਪੂਰਨ
get_script_ਵਸਤੂ_idx(
	ਪੂਰਨ	    sid,
	char_u	    *name,
	ਪੂਰਨ	    check_writable,
	cctx_T	    *cctx,
	cstack_T    *cstack)
{
    hashtab_T	    *ht;
    dictitem_T	    *di;
    scriptitem_T    *si = SCRIPT_ਵਸਤੂ(sid);
    svar_T	    *sv;
    ਪੂਰਨ		    idx;

    ਜੇ (!SCRIPT_ID_VALID(sid))
	ਵਾਪਸ -1;
    ਜੇ (sid == current_sctx.sc_sid)
    {
	sallvar_T *sav = ਲੱਭੋ_script_var(name, 0, cctx, cstack);

	ਜੇ (sav == ਨਿਰਾ)
	    ਵਾਪਸ -2;
	idx = sav->sav_var_vals_idx;
	sv = ((svar_T *)si->sn_var_vals.ga_ਅੰਕੜੇ) + idx;
	ਜੇ (check_ਵਸਤੂ_writable(sv, check_writable, name) == FAIL)
	    ਵਾਪਸ -2;
	ਵਾਪਸ idx;
    }

    // First look the name up in the hashtable.
    ht = &SCRIPT_VARS(sid);
    di = ਲੱਭੋ_var_ਵਿੱਚ_ht(ht, 0, name, TRUE);
    ਜੇ (di == ਨਿਰਾ)
    {
	ਜੇ (si->sn_autoload_ਅਗੇਤਰ != ਨਿਰਾ)
	{
	    hashitem_T *hi;

	    // A variable exported from an autoload script is in the global
	    // variables, we can find it in the all_vars table.
	    hi = hash_ਲੱਭੋ(&si->sn_all_vars.dv_hashtab, name);
	    ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
		ਵਾਪਸ HI2SAV(hi)->sav_var_vals_idx;
	}
	ਵਾਪਸ -2;
    }

    // Now find the svar_T index in sn_var_vals.
    ਲਈ (idx = 0; idx < si->sn_var_vals.ga_len; ++idx)
    {
	sv = ((svar_T *)si->sn_var_vals.ga_ਅੰਕੜੇ) + idx;
	ਜੇ (sv->sv_tv == &di->di_tv)
	{
	    ਜੇ (check_ਵਸਤੂ_writable(sv, check_writable, name) == FAIL)
		ਵਾਪਸ -2;
	    ਵਾਪਸ idx;
	}
    }
    ਵਾਪਸ -1;
}

    ਸਥਿਰ imported_T *
ਲੱਭੋ_imported_ਵਿੱਚ_script(char_u *name, size_t len, ਪੂਰਨ sid)
{
    scriptitem_T    *si;
    ਪੂਰਨ		    idx;

    ਜੇ (!SCRIPT_ID_VALID(sid))
	ਵਾਪਸ ਨਿਰਾ;
    si = SCRIPT_ਵਸਤੂ(sid);
    ਲਈ (idx = 0; idx < si->sn_imports.ga_len; ++idx)
    {
	imported_T *import = ((imported_T *)si->sn_imports.ga_ਅੰਕੜੇ) + idx;

	ਜੇ (len == 0 ? STRCMP(name, import->imp_name) == 0
		     : STRLEN(import->imp_name) == len
				  && STRNCMP(name, import->imp_name, len) == 0)
	    ਵਾਪਸ import;
    }
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Find "name" in imported items of the current script.
 * If "len" is 0 use any length that works.
 * If "load" is TRUE and the script was not loaded yet, load it now.
 */
    imported_T *
ਲੱਭੋ_imported(char_u *name, size_t len, ਪੂਰਨ ਲੱਦੋ)
{
    ਜੇ (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	ਵਾਪਸ ਨਿਰਾ;

    // Skip over "s:" before "s:something" to find the import name.
    ਪੂਰਨ off = name[0] == 's' && name[1] == ':' ? 2 : 0;

    imported_T *ret = ਲੱਭੋ_imported_ਵਿੱਚ_script(name + off, len - off,
							  current_sctx.sc_sid);
    ਜੇ (ret != ਨਿਰਾ && ਲੱਦੋ && (ret->imp_flags & IMP_FLAGS_AUTOLOAD))
    {
	scid_T	actual_sid = 0;
	ਪੂਰਨ	save_emsg_off = emsg_off;

	// "emsg_off" will be set when evaluating an expression silently, but
	// we do want to know about errors in a script.  Also because it then
	// aborts when an error is encountered.
	emsg_off = FALSE;

	// script found before but not loaded yet
	ret->imp_flags &= ~IMP_FLAGS_AUTOLOAD;
	(ਖਾਲੀ)do_ਸਰੋਤ(SCRIPT_ਵਸਤੂ(ret->imp_sid)->sn_name, FALSE,
						       DOSO_NONE, &actual_sid);
	// If the script is a symlink it may be sourced with another name, may
	// need to adjust the script ID for that.
	ਜੇ (actual_sid != 0)
	    ret->imp_sid = actual_sid;

	emsg_off = save_emsg_off;
    }
    ਵਾਪਸ ret;
}

/*
 * Called when checking for a following operator at "arg".  When the rest of
 * the line is empty or only a comment, peek the next line.  If there is a next
 * line return a pointer to it and set "nextp".
 * Otherwise skip over white space.
 */
    char_u *
may_peek_ਅਗਲਾ_ਸਤਰ(cctx_T *cctx, char_u *arg, char_u **nextp)
{
    char_u *p = skipwhite(arg);

    *nextp = ਨਿਰਾ;
    ਜੇ (*p == NUL || (VIM_ISWHITE(*arg) && vim9_comment_ਸ਼ੁਰੂ(p)))
    {
	*nextp = peek_ਅਗਲਾ_ਸਤਰ_from_context(cctx);
	ਜੇ (*nextp != ਨਿਰਾ)
	    ਵਾਪਸ *nextp;
    }
    ਵਾਪਸ p;
}

/*
 * Return a pointer to the next line that isn't empty or only contains a
 * comment. Skips over white space.
 * Returns NULL if there is none.
 */
    char_u *
peek_ਅਗਲਾ_ਸਤਰ_from_context(cctx_T *cctx)
{
    ਪੂਰਨ lnum = cctx->ctx_lnum;

    ਜਦੋਂ (++lnum < cctx->ctx_ufunc->uf_ਸਤਰਾਂ.ga_len)
    {
	char_u *ਸਤਰ = ((char_u **)cctx->ctx_ufunc->uf_ਸਤਰਾਂ.ga_ਅੰਕੜੇ)[lnum];
	char_u *p;

	// ignore NULLs inserted for continuation lines
	ਜੇ (ਸਤਰ != ਨਿਰਾ)
	{
	    p = skipwhite(ਸਤਰ);
	    ਜੇ (vim9_bad_comment(p))
		ਵਾਪਸ ਨਿਰਾ;
	    ਜੇ (*p != NUL && !vim9_comment_ਸ਼ੁਰੂ(p))
		ਵਾਪਸ p;
	}
    }
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Get the next line of the function from "cctx".
 * Skips over empty lines.  Skips over comment lines if "skip_comment" is TRUE.
 * Returns NULL when at the end.
 */
    char_u *
ਅਗਲਾ_ਸਤਰ_from_context(cctx_T *cctx, ਪੂਰਨ skip_comment)
{
    char_u	*ਸਤਰ;

    ਕਰੋ
    {
	++cctx->ctx_lnum;
	ਜੇ (cctx->ctx_lnum >= cctx->ctx_ufunc->uf_ਸਤਰਾਂ.ga_len)
	{
	    ਸਤਰ = ਨਿਰਾ;
	    ਰੋਕੋ;
	}
	ਸਤਰ = ((char_u **)cctx->ctx_ufunc->uf_ਸਤਰਾਂ.ga_ਅੰਕੜੇ)[cctx->ctx_lnum];
	cctx->ctx_ਸਤਰ_ਸ਼ੁਰੂ = ਸਤਰ;
	SOURCING_LNUM = cctx->ctx_lnum + 1;
    } ਜਦੋਂ (ਸਤਰ == ਨਿਰਾ || *skipwhite(ਸਤਰ) == NUL
		     || (skip_comment && vim9_comment_ਸ਼ੁਰੂ(skipwhite(ਸਤਰ))));
    ਵਾਪਸ ਸਤਰ;
}

/*
 * Skip over white space at "whitep" and assign to "*arg".
 * If "*arg" is at the end of the line, advance to the next line.
 * Also when "whitep" points to white space and "*arg" is on a "#".
 * Return FAIL if beyond the last line, "*arg" is unmodified then.
 */
    ਪੂਰਨ
may_get_ਅਗਲਾ_ਸਤਰ(char_u *whitep, char_u **arg, cctx_T *cctx)
{
    *arg = skipwhite(whitep);
    ਜੇ (vim9_bad_comment(*arg))
	ਵਾਪਸ FAIL;
    ਜੇ (**arg == NUL || (VIM_ISWHITE(*whitep) && vim9_comment_ਸ਼ੁਰੂ(*arg)))
    {
	char_u *ਅਗਲਾ = ਅਗਲਾ_ਸਤਰ_from_context(cctx, TRUE);

	ਜੇ (ਅਗਲਾ == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;
	*arg = skipwhite(ਅਗਲਾ);
    }
    ਵਾਪਸ OK;
}

/*
 * Idem, and give an error when failed.
 */
    ਪੂਰਨ
may_get_ਅਗਲਾ_ਸਤਰ_error(char_u *whitep, char_u **arg, cctx_T *cctx)
{
    ਜੇ (may_get_ਅਗਲਾ_ਸਤਰ(whitep, arg, cctx) == FAIL)
    {
	SOURCING_LNUM = cctx->ctx_lnum + 1;
	emsg(_(e_ਸਤਰ_incomplete));
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * Get a line from the compilation context, compatible with exarg_T getline().
 * Return a pointer to the line in allocated memory.
 * Return NULL for end-of-file or some error.
 */
    ਸਥਿਰ char_u *
exarg_getline(
	ਪੂਰਨ c ਅਣਵਰਤਿਆ,
	ਖਾਲੀ *cookie,
	ਪੂਰਨ indent ਅਣਵਰਤਿਆ,
	getline_opt_T options ਅਣਵਰਤਿਆ)
{
    cctx_T  *cctx = (cctx_T *)cookie;
    char_u  *p;

    ਲਈ (;;)
    {
	ਜੇ (cctx->ctx_lnum >= cctx->ctx_ufunc->uf_ਸਤਰਾਂ.ga_len - 1)
	    ਵਾਪਸ ਨਿਰਾ;
	++cctx->ctx_lnum;
	p = ((char_u **)cctx->ctx_ufunc->uf_ਸਤਰਾਂ.ga_ਅੰਕੜੇ)[cctx->ctx_lnum];
	// Comment lines result in NULL pointers, skip them.
	ਜੇ (p != ਨਿਰਾ)
	    ਵਾਪਸ vim_strsave(p);
    }
}

    ਖਾਲੀ
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
    ਪੂਰਨ
func_needs_compiling(ufunc_T *ufunc, compiletype_T compile_type)
{
    ਚੋਣ (ufunc->uf_def_ਸਥਿਤੀ)
    {
	ਹਾਲਤ UF_ਮੰਜ਼ਿਲ_BE_COMPILED:
	    ਵਾਪਸ TRUE;

	ਹਾਲਤ UF_COMPILED:
	{
	    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_ਅੰਕੜੇ)
							 + ufunc->uf_dfunc_idx;

	    ਚੋਣ (compile_type)
	    {
		ਹਾਲਤ CT_PROFILE:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROFILE
		    ਵਾਪਸ dfunc->df_instr_prof == ਨਿਰਾ;
#ਜੇ_ਅੰਤ
		ਹਾਲਤ CT_NONE:
		    ਵਾਪਸ dfunc->df_instr == ਨਿਰਾ;
		ਹਾਲਤ CT_DEBUG:
		    ਵਾਪਸ dfunc->df_instr_debug == ਨਿਰਾ;
	    }
	}

	ਹਾਲਤ UF_NOT_COMPILED:
	ਹਾਲਤ UF_COMPILE_ERROR:
	ਹਾਲਤ UF_COMPILING:
	    ਰੋਕੋ;
    }
    ਵਾਪਸ FALSE;
}

/*
 * Compile a nested :def command.
 */
    ਸਥਿਰ char_u *
compile_nested_function(exarg_T *eap, cctx_T *cctx, garray_T *ਸਤਰਾਂ_ਮੰਜ਼ਿਲ_free)
{
    ਪੂਰਨ		is_global = *eap->arg == 'g' && eap->arg[1] == ':';
    char_u	*name_ਸ਼ੁਰੂ = eap->arg;
    char_u	*name_ਸਿਰਾ = ਮੰਜ਼ਿਲ_name_ਸਿਰਾ(eap->arg, TRUE);
    ਪੂਰਨ		off;
    char_u	*func_name;
    string_T	lambda_name;
    ufunc_T	*ufunc;
    ਪੂਰਨ		r = FAIL;
    compiletype_T   compile_type;
    ਪੂਰਨ		funcref_isn_idx = -1;
    lvar_T	*lvar = ਨਿਰਾ;
    char_u	*bracket_ਸ਼ੁਰੂ = ਨਿਰਾ;

    ਜੇ (eap->forceit)
    {
	semsg(_(e_cannot_use_bang_with_nested_def_str),
		eap->cmdidx == CMD_def ? ":def" : ":function");
	ਵਾਪਸ ਨਿਰਾ;
    }

    ਜੇ (*name_ਸ਼ੁਰੂ == '/')
    {
	name_ਸਿਰਾ = skip_regexp(name_ਸ਼ੁਰੂ + 1, '/', TRUE);
	ਜੇ (*name_ਸਿਰਾ == '/')
	    ++name_ਸਿਰਾ;
	ਸੈੱਟ_nextcmd(eap, name_ਸਿਰਾ);
    }

    ਜੇ (*name_ਸਿਰਾ == '<')
    {
	bracket_ਸ਼ੁਰੂ = name_ਸਿਰਾ;
	ਜੇ (skip_generic_func_type_ਦਲੀਲਾਂ(&name_ਸਿਰਾ) == FAIL)
	    ਵਾਪਸ ਨਿਰਾ;
    }

    ਜੇ (name_ਸਿਰਾ == name_ਸ਼ੁਰੂ || *skipwhite(name_ਸਿਰਾ) != '(')
    {
	ਜੇ (!ਸਿਰੇ_excmd2(name_ਸ਼ੁਰੂ, name_ਸਿਰਾ))
	{
	    ਜੇ (*skipwhite(name_ਸਿਰਾ) == '.')
		semsg(_(e_cannot_define_dict_func_ਵਿੱਚ_vim9_script_str),
								     eap->cmd);
	    ਨਹੀਂਤਾਂ
		semsg(_(e_invalid_command_str), eap->cmd);
	    ਵਾਪਸ ਨਿਰਾ;
	}

	// "def" or "def Name": list functions
	ਜੇ (generate_DEF(cctx, name_ਸ਼ੁਰੂ, name_ਸਿਰਾ - name_ਸ਼ੁਰੂ) == FAIL)
	    ਵਾਪਸ ਨਿਰਾ;
	ਵਾਪਸ eap->nextcmd == ਨਿਰਾ ? (char_u *)"" : eap->nextcmd;
    }

    ਜੇ (bracket_ਸ਼ੁਰੂ != ਨਿਰਾ)
	// generic function.  The function name ends before the list of types
	// (opening angle bracket).
	name_ਸਿਰਾ = bracket_ਸ਼ੁਰੂ;

    // Only g:Func() can use a namespace.
    ਜੇ (name_ਸ਼ੁਰੂ[1] == ':' && !is_global)
    {
	semsg(_(e_namespace_not_supported_str), name_ਸ਼ੁਰੂ);
	ਵਾਪਸ ਨਿਰਾ;
    }
    ਜੇ (cctx->ctx_skip != SKIP_YES
	    && check_defined(name_ਸ਼ੁਰੂ, name_ਸਿਰਾ - name_ਸ਼ੁਰੂ, cctx,
							  ਨਿਰਾ, FALSE) == FAIL)
	ਵਾਪਸ ਨਿਰਾ;
    ਜੇ (!ASCII_ISUPPER(is_global ? name_ਸ਼ੁਰੂ[2] : name_ਸ਼ੁਰੂ[0]))
    {
	semsg(_(e_function_name_must_ਸ਼ੁਰੂ_with_capital_str), name_ਸ਼ੁਰੂ);
	ਵਾਪਸ ਨਿਰਾ;
    }

    eap->arg = name_ਸਿਰਾ;
    fill_exarg_from_cctx(eap, cctx);

    eap->forceit = FALSE;
    // We use the special <lambda>99 name, but it's not really a lambda.
    lambda_name = get_lambda_name();
    lambda_name.string = vim_strnsave(lambda_name.string, lambda_name.ਲੰਮਾਈ);
    ਜੇ (lambda_name.string == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    // This may free the current line, make a copy of the name.
    off = is_global ? 2 : 0;
    func_name = vim_strnsave(name_ਸ਼ੁਰੂ + off, name_ਸਿਰਾ - name_ਸ਼ੁਰੂ - off);
    ਜੇ (func_name == ਨਿਰਾ)
    {
	r = FAIL;
	ਜਾਓ theend;
    }

    // A type variable cannot have the name of the function.  The function is
    // defined with the "<lambda>" name, check it here.
    ਜੇ (bracket_ਸ਼ੁਰੂ != ਨਿਰਾ)
    {
	size_t	len = STRLEN(func_name);

	ਲਈ (char_u *t = bracket_ਸ਼ੁਰੂ + 1; *t != '>' && *t != NUL; )
	{
	    char_u *te = ਮੰਜ਼ਿਲ_name_ਸਿਰਾ(t, FALSE);

	    ਜੇ ((size_t)(te - t) == len && STRNCMP(t, func_name, len) == 0)
	    {
		semsg(_(e_redefining_script_ਵਸਤੂ_str), func_name);
		r = FAIL;
		ਜਾਓ theend;
	    }
	    t = te == t ? t + 1 : te;
	}
    }

    // Make sure "KeyTyped" is not set, it may cause indent to be written.
    ਪੂਰਨ save_ਕੁੰਜੀ_Typed = ਕੁੰਜੀ_Typed;
    ਕੁੰਜੀ_Typed = FALSE;

    ufunc = define_function(eap, lambda_name.string, ਸਤਰਾਂ_ਮੰਜ਼ਿਲ_free, 0, ਨਿਰਾ, 0,
									cctx);

    ਕੁੰਜੀ_Typed = save_ਕੁੰਜੀ_Typed;

    ਜੇ (ufunc == ਨਿਰਾ)
    {
	r = eap->skip ? OK : FAIL;
	ਜਾਓ theend;
    }
    ਜੇ (eap->nextcmd != ਨਿਰਾ)
    {
	semsg(_(e_ਪਾਠ_ਮਿਲਿਆ_after_str_str),
	      eap->cmdidx == CMD_def ? "enddef" : "endfunction", eap->nextcmd);
	r = FAIL;
	func_ptr_unref(ufunc);
	ਜਾਓ theend;
    }

    // copy over the block scope IDs before compiling
    ਜੇ (!is_global && cctx->ctx_ufunc->uf_ਖੰਡ_ਡੂੰਘਾਈ > 0)
    {
	ਪੂਰਨ ਖੰਡ_ਡੂੰਘਾਈ = cctx->ctx_ufunc->uf_ਖੰਡ_ਡੂੰਘਾਈ;

	ufunc->uf_ਖੰਡ_ids = ALLOC_MULT(ਪੂਰਨ, ਖੰਡ_ਡੂੰਘਾਈ);
	ਜੇ (ufunc->uf_ਖੰਡ_ids != ਨਿਰਾ)
	{
	    mch_memmove(ufunc->uf_ਖੰਡ_ids, cctx->ctx_ufunc->uf_ਖੰਡ_ids,
						    ਆਕਾਰ(ਪੂਰਨ) * ਖੰਡ_ਡੂੰਘਾਈ);
	    ufunc->uf_ਖੰਡ_ਡੂੰਘਾਈ = ਖੰਡ_ਡੂੰਘਾਈ;
	}
    }

    // Define the funcref before compiling, so that it is found by any
    // recursive call.
    ਜੇ (is_global)
    {
	r = generate_NEWFUNC(cctx, lambda_name.string, func_name);
	func_name = ਨਿਰਾ;
	lambda_name.string = ਨਿਰਾ;
	lambda_name.ਲੰਮਾਈ = 0;
    }
    ਨਹੀਂਤਾਂ
    {
	// Define a local variable for the function reference.
	lvar = reserve_local(cctx, func_name, name_ਸਿਰਾ - name_ਸ਼ੁਰੂ,
					    ਸੌਂਪੋ_CONST, ufunc->uf_func_type);
	ਜੇ (lvar == ਨਿਰਾ)
	{
	    func_ptr_unref(ufunc);
	    ਜਾਓ theend;
	}
	lvar->lv_ufunc = ufunc;
	ਜੇ (generate_FUNCREF(cctx, ufunc, ਨਿਰਾ, FALSE, 0, &funcref_isn_idx) == FAIL)
	{
	    func_ptr_unref(ufunc);
	    ਜਾਓ theend;
	}
	r = generate_STORE(cctx, ISN_STORE, lvar->lv_idx, ਨਿਰਾ);
    }

    compile_type = get_compile_type(ufunc);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROFILE
    // If the outer function is profiled, also compile the nested function for
    // profiling.
    ਜੇ (cctx->ctx_compile_type == CT_PROFILE)
	compile_type = CT_PROFILE;
#ਜੇ_ਅੰਤ
    ਜੇ (func_needs_compiling(ufunc, compile_type)
	    && compile_def_function(ufunc, TRUE, compile_type, cctx) == FAIL)
    {
	func_ptr_unref(ufunc);
	ਜੇ (lvar != ਨਿਰਾ)
	    // Now the local variable can't be used.
	    *lvar->lv_name = '/';  // impossible value
	ਜਾਓ theend;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROFILE
    // When the outer function is compiled for profiling, the nested function
    // may be called without profiling.  Compile it here in the right context.
    ਜੇ (compile_type == CT_PROFILE && func_needs_compiling(ufunc, CT_NONE))
	compile_def_function(ufunc, FALSE, CT_NONE, cctx);
#ਜੇ_ਅੰਤ

    // If a FUNCREF instruction was generated, set the index after compiling.
    ਜੇ (funcref_isn_idx != -1 && ufunc->uf_def_ਸਥਿਤੀ == UF_COMPILED)
    {
	isn_T	*funcref_isn = ((isn_T *)cctx->ctx_instr.ga_ਅੰਕੜੇ) +
							funcref_isn_idx;
	funcref_isn->isn_arg.funcref.fr_dfunc_idx = ufunc->uf_dfunc_idx;
    }

theend:
    vim_free(lambda_name.string);
    vim_free(func_name);
    ਵਾਪਸ r == FAIL ? ਨਿਰਾ : (char_u *)"";
}

/*
 * Compile one Vim expression {expr} in string "p".
 * "p" points to the opening "{".
 * Return a pointer to the character after "}", NULL for an error.
 */
    char_u *
compile_one_ਵਿਅੰਜਕ_ਵਿੱਚ_str(char_u *p, cctx_T *cctx)
{
    char_u	*ਖੰਡ_ਸ਼ੁਰੂ;
    char_u	*ਖੰਡ_ਸਿਰਾ;

    // Skip the opening {.
    ਖੰਡ_ਸ਼ੁਰੂ = skipwhite(p + 1);
    ਖੰਡ_ਸਿਰਾ = ਖੰਡ_ਸ਼ੁਰੂ;
    ਜੇ (*ਖੰਡ_ਸ਼ੁਰੂ != NUL && skip_ਵਿਅੰਜਕ(&ਖੰਡ_ਸਿਰਾ, ਨਿਰਾ) == FAIL)
	ਵਾਪਸ ਨਿਰਾ;
    ਖੰਡ_ਸਿਰਾ = skipwhite(ਖੰਡ_ਸਿਰਾ);
    // The block must be closed by a }.
    ਜੇ (*ਖੰਡ_ਸਿਰਾ != '}')
    {
	semsg(_(e_missing_close_curly_str), p);
	ਵਾਪਸ ਨਿਰਾ;
    }
    ਜੇ (compile_expr0(&ਖੰਡ_ਸ਼ੁਰੂ, cctx) == FAIL)
	ਵਾਪਸ ਨਿਰਾ;
    may_generate_2STRING(-1, TOSTRING_INTERPOLATE, cctx);

    ਵਾਪਸ ਖੰਡ_ਸਿਰਾ + 1;
}

/*
 * Compile a string "str" (either containing a literal string or a mix of
 * literal strings and Vim expressions of the form `{expr}`).  This is used
 * when compiling a heredoc assignment to a variable or an interpolated string
 * in a Vim9 def function.  Vim9 instructions are generated to push strings,
 * evaluate expressions, concatenate them and create a list of lines.  When
 * "evalstr" is TRUE, Vim expressions in "str" are evaluated.
 */
    ਪੂਰਨ
compile_all_ਵਿਅੰਜਕ_ਵਿੱਚ_str(char_u *str, ਪੂਰਨ evalstr, cctx_T *cctx)
{
    char_u	*p = str;
    char_u	*val;
    ਪੂਰਨ		ਗਿਣਤੀ_2 = 0;

    ਜੇ (cctx->ctx_skip == SKIP_YES)
	ਵਾਪਸ OK;

    ਜੇ (!evalstr || *str == NUL)
    {
	// Literal string, possibly empty.
	val = *str != NUL ? vim_strsave(str) : ਨਿਰਾ;
	ਵਾਪਸ generate_PUSHS(cctx, &val);
    }

    // Push all the string pieces to the stack, followed by a ISN_CONCAT.
    ਜਦੋਂ (*p != NUL)
    {
	char_u	*lit_ਸ਼ੁਰੂ;
	ਪੂਰਨ	escaped_brace = FALSE;

	// Look for a block start.
	lit_ਸ਼ੁਰੂ = p;
	ਜਦੋਂ (*p != '{' && *p != '}' && *p != NUL)
	    ++p;

	ਜੇ (*p != NUL && *p == p[1])
	{
	    // Escaped brace, unescape and continue.
	    // Include the brace in the literal string.
	    ++p;
	    escaped_brace = TRUE;
	}
	ਨਹੀਂਤਾਂ ਜੇ (*p == '}')
	{
	    semsg(_(e_stray_closing_curly_str), str);
	    ਵਾਪਸ FAIL;
	}

	// Append the literal part.
	ਜੇ (p != lit_ਸ਼ੁਰੂ)
	{
	    val = vim_strnsave(lit_ਸ਼ੁਰੂ, (size_t)(p - lit_ਸ਼ੁਰੂ));
	    ਜੇ (generate_PUSHS(cctx, &val) == FAIL)
		ਵਾਪਸ FAIL;
	    ++ਗਿਣਤੀ_2;
	}

	ਜੇ (*p == NUL)
	    ਰੋਕੋ;

	ਜੇ (escaped_brace)
	{
	    // Skip the second brace.
	    ++p;
	    ਜਾਰੀ;
	}

	p = compile_one_ਵਿਅੰਜਕ_ਵਿੱਚ_str(p, cctx);
	ਜੇ (p == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;
	++ਗਿਣਤੀ_2;
    }

    // Small optimization, if there's only a single piece skip the ISN_CONCAT.
    ਜੇ (ਗਿਣਤੀ_2 > 1)
	ਵਾਪਸ generate_CONCAT(cctx, ਗਿਣਤੀ_2);

    ਵਾਪਸ OK;
}

/*
 * Return the length of an assignment operator, or zero if there isn't one.
 */
    ਪੂਰਨ
assignment_len(char_u *p, ਪੂਰਨ *heredoc)
{
    ਜੇ (*p == '=')
    {
	ਜੇ (p[1] == '<' && p[2] == '<')
	{
	    *heredoc = TRUE;
	    ਵਾਪਸ 3;
	}
	ਵਾਪਸ 1;
    }
    ਜੇ (vim_strchr((char_u *)"+-*/%", *p) != ਨਿਰਾ && p[1] == '=')
	ਵਾਪਸ 2;
    ਜੇ (STRNCMP(p, "..=", 3) == 0)
	ਵਾਪਸ 3;
    ਵਾਪਸ 0;
}

/*
 * Generate the load instruction for "name".
 */
    ਸਥਿਰ ਪੂਰਨ
generate_loadvar(cctx_T *cctx, lhs_T *lhs)
{
    char_u	*name = lhs->lhs_name;
    type_T	*type = lhs->lhs_type;
    ਪੂਰਨ		res = OK;

    ਚੋਣ (lhs->lhs_dest)
    {
	ਹਾਲਤ dest_option:
	ਹਾਲਤ dest_func_option:
	    generate_ਲੱਦੋ(cctx, ISN_LOADOPT, 0, name, type);
	    ਰੋਕੋ;
	ਹਾਲਤ dest_global:
	    ਜੇ (vim_strchr(name, AUTOLOAD_CHAR) == ਨਿਰਾ)
	    {
		ਜੇ (name[2] == NUL)
		    generate_instr_type(cctx, ISN_LOADGDICT, &t_dict_any);
		ਨਹੀਂਤਾਂ
		    generate_ਲੱਦੋ(cctx, ISN_LOADG, 0, name + 2, type);
	    }
	    ਨਹੀਂਤਾਂ
		generate_ਲੱਦੋ(cctx, ISN_LOADAUTO, 0, name, type);
	    ਰੋਕੋ;
	ਹਾਲਤ dest_ਬਫਰ:
	    generate_ਲੱਦੋ(cctx, ISN_LOADB, 0, name + 2, type);
	    ਰੋਕੋ;
	ਹਾਲਤ dest_ਖਿੜਕੀ:
	    generate_ਲੱਦੋ(cctx, ISN_LOADW, 0, name + 2, type);
	    ਰੋਕੋ;
	ਹਾਲਤ dest_tab:
	    generate_ਲੱਦੋ(cctx, ISN_LOADT, 0, name + 2, type);
	    ਰੋਕੋ;
	ਹਾਲਤ dest_script:
	ਹਾਲਤ dest_script_v9:
	    res = compile_ਲੱਦੋ_scriptvar(cctx,
			    name + (name[1] == ':' ? 2 : 0), ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ);
	    ਰੋਕੋ;
	ਹਾਲਤ dest_env:
	    // Include $ in the name here
	    generate_ਲੱਦੋ(cctx, ISN_LOADENV, 0, name, type);
	    ਰੋਕੋ;
	ਹਾਲਤ dest_reg:
	    generate_ਲੱਦੋ(cctx, ISN_LOADREG, name[1], ਨਿਰਾ, &t_string);
	    ਰੋਕੋ;
	ਹਾਲਤ dest_vimvar:
	    generate_LOADV(cctx, name + 2);
	    ਰੋਕੋ;
	ਹਾਲਤ dest_local:
	    ਜੇ (cctx->ctx_skip != SKIP_YES)
	    {
		lvar_T	*lvar = lhs->lhs_lvar;
		ਜੇ (lvar->lv_from_outer > 0)
		    generate_LOADOUTER(cctx, lvar->lv_idx, lvar->lv_from_outer,
				 lvar->lv_loop_ਡੂੰਘਾਈ, lvar->lv_loop_idx, type);
		ਨਹੀਂਤਾਂ
		    generate_ਲੱਦੋ(cctx, ISN_ਲੱਦੋ, lvar->lv_idx, ਨਿਰਾ, type);
	    }
	    ਰੋਕੋ;
	ਹਾਲਤ dest_class_member:
	    generate_CLASSMEMBER(cctx, TRUE, lhs->lhs_class,
						     lhs->lhs_classmember_idx);
	    ਰੋਕੋ;
	ਹਾਲਤ dest_ਵਿਅੰਜਕ:
	    // list or dict value should already be on the stack.
	    ਰੋਕੋ;
    }

    ਵਾਪਸ res;
}

/*
 * Skip over "[expr]" or ".member".
 * Does not check for any errors.
 */
    ਸਥਿਰ char_u *
skip_ਸੂਚਕਾਂਕ(char_u *ਸ਼ੁਰੂ)
{
    char_u *p = ਸ਼ੁਰੂ;

    ਜੇ (*p == '[')
    {
	p = skipwhite(p + 1);
	(ਖਾਲੀ)skip_ਵਿਅੰਜਕ(&p, ਨਿਰਾ);
	p = skipwhite(p);
	ਜੇ (*p == ']')
	    ਵਾਪਸ p + 1;
	ਵਾਪਸ p;
    }
    // if (*p == '.')
    ਵਾਪਸ ਮੰਜ਼ਿਲ_name_ਸਿਰਾ(p + 1, TRUE);
}

    ਖਾਲੀ
vim9_declare_error(char_u *name)
{
    ਅੱਖਰ *scope = "";

    ਚੋਣ (*name)
    {
	ਹਾਲਤ 'g': scope = _("global"); ਰੋਕੋ;
	ਹਾਲਤ 'b': scope = _("buffer"); ਰੋਕੋ;
	ਹਾਲਤ 'w': scope = _("window"); ਰੋਕੋ;
	ਹਾਲਤ 't': scope = _("tab"); ਰੋਕੋ;
	ਹਾਲਤ 'v': scope = "v:"; ਰੋਕੋ;
	ਹਾਲਤ '$': semsg(_(e_cannot_declare_an_environment_variable_str), name);
		  ਵਾਪਸ;
	ਹਾਲਤ '&': semsg(_(e_cannot_declare_an_option_str), name);
		  ਵਾਪਸ;
	ਹਾਲਤ '@': semsg(_(e_cannot_declare_a_ਰਜਿਸਟਰ_str), name);
		  ਵਾਪਸ;
	ਮੂਲ: ਵਾਪਸ;
    }
    semsg(_(e_cannot_declare_a_scope_variable_str), scope, name);
}

/*
 * Return TRUE if "name" is a valid register to use.
 * Return FALSE and give an error message if not.
 */
    ਸਥਿਰ ਪੂਰਨ
valid_dest_reg(ਪੂਰਨ name)
{
    ਜੇ (name == '@')
       name = '"';
    ਜੇ (name == '/' || name == '=' || name == '#' || valid_yank_reg(name, TRUE))
	ਵਾਪਸ TRUE;
    emsg_invreg(name);
    ਵਾਪਸ FAIL;
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
    ਪੂਰਨ
get_var_dest(
	char_u		*name,
	ਸੌਂਪੋ_dest_T	*dest,
	cmdidx_T	cmdidx,
	ਪੂਰਨ		has_ਸੂਚਕਾਂਕ,
	ਪੂਰਨ		*option_scope,
	ਪੂਰਨ		*vimvaridx,
	type_T		**type,
	cctx_T		*cctx)
{
    char_u *p;

    ਜੇ (*name == '&')
    {
	ਪੂਰਨ		cc;
	ਲੰਮਾ		numval;
	getoption_T	opt_type;
	ਪੂਰਨ		opt_p_flags;

	*dest = dest_option;
	ਜੇ (cmdidx == CMD_final || cmdidx == CMD_const)
	{
	    emsg(_(e_cannot_ਤਾਲਾ_option));
	    ਵਾਪਸ FAIL;
	}
	p = name;
	p = ਲੱਭੋ_option_ਸਿਰਾ(&p, option_scope);
	ਜੇ (p == ਨਿਰਾ)
	{
	    // cannot happen?
	    emsg(_(e_unexpected_characters_ਵਿੱਚ_assignment));
	    ਵਾਪਸ FAIL;
	}
	cc = *p;
	*p = NUL;
	opt_type = get_option_ਮੁੱਲ(skip_option_env_lead(name),
				   &numval, ਨਿਰਾ, &opt_p_flags, *option_scope);
	*p = cc;
	ਚੋਣ (opt_type)
	{
	    ਹਾਲਤ gov_unknown:
		    semsg(_(e_unknown_option_str), name);
		    ਵਾਪਸ FAIL;
	    ਹਾਲਤ gov_string:
	    ਹਾਲਤ gov_hidden_string:
		    ਜੇ (opt_p_flags & P_FUNC)
		    {
			// might be a Funcref, check the type later
			*type = &t_any;
			*dest = dest_func_option;
		    }
		    ਨਹੀਂਤਾਂ
		    {
			*type = &t_string;
		    }
		    ਰੋਕੋ;
	    ਹਾਲਤ gov_bool:
	    ਹਾਲਤ gov_hidden_bool:
		    *type = &t_bool;
		    ਰੋਕੋ;
	    ਹਾਲਤ gov_ਸੰਖਿਆ:
	    ਹਾਲਤ gov_hidden_ਸੰਖਿਆ:
		    *type = &t_ਸੰਖਿਆ;
		    ਰੋਕੋ;
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (*name == '$')
    {
	*dest = dest_env;
	*type = &t_string;
    }
    ਨਹੀਂਤਾਂ ਜੇ (*name == '@')
    {
	ਜੇ (!valid_dest_reg(name[1]))
	    ਵਾਪਸ FAIL;
	*dest = dest_reg;
	*type = name[1] == '#' ? &t_ਸੰਖਿਆ_or_string : &t_string;
    }
    ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(name, "g:", 2) == 0)
    {
	*dest = dest_global;
    }
    ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(name, "b:", 2) == 0)
    {
	*dest = dest_ਬਫਰ;
    }
    ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(name, "w:", 2) == 0)
    {
	*dest = dest_ਖਿੜਕੀ;
    }
    ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(name, "t:", 2) == 0)
    {
	*dest = dest_tab;
    }
    ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(name, "v:", 2) == 0)
    {
	typval_T	*vtv;
	ਪੂਰਨ		di_flags;

	*vimvaridx = ਲੱਭੋ_vim_var(name + 2, &di_flags);
	ਜੇ (*vimvaridx < 0)
	{
	    semsg(_(e_variable_not_ਮਿਲਿਆ_str), name);
	    ਵਾਪਸ FAIL;
	}
	// An item of a read-only variable can be changed, as when not compiled.
	// We use the current value of "sandbox" here, is that OK?
	ਜੇ (!has_ਸੂਚਕਾਂਕ && var_check_ro(di_flags, name, FALSE))
	    ਵਾਪਸ FAIL;
	*dest = dest_vimvar;
	vtv = get_vim_var_tv(*vimvaridx);
	*type = typval2type_vimvar(vtv, cctx->ctx_type_list);
    }
    ਵਾਪਸ OK;
}

    ਸਥਿਰ ਪੂਰਨ
is_decl_command(cmdidx_T cmdidx)
{
    ਵਾਪਸ cmdidx == CMD_let || cmdidx == CMD_var
				 || cmdidx == CMD_final || cmdidx == CMD_const;
}

/*
 * Returns TRUE if the class or object variable in "lhs" is modifiable.
 * "var_start" points to the start of the variable name and "lhs->lhs_varlen"
 * has the total length.  Note that the "lhs" can be nested an object reference
 * (e.g.  a.b.c.d.var).
 */
    ਸਥਿਰ ਪੂਰਨ
lhs_class_member_modifiable(lhs_T *lhs, char_u	*var_ਸ਼ੁਰੂ, cctx_T *cctx)
{
    size_t	varlen = lhs->lhs_varlen;
    class_T	*cl = lhs->lhs_type->tt_class;
    ਪੂਰਨ		is_object = lhs->lhs_type->tt_type == VAR_OBJECT;
    char_u	*name = var_ਸ਼ੁਰੂ + varlen + 1;
    size_t	namelen = lhs->lhs_ਸਿਰਾ - var_ਸ਼ੁਰੂ - varlen - 1;
    ocmember_T	*m;

    m = member_lookup(cl, lhs->lhs_type->tt_type, name, namelen, ਨਿਰਾ);
    ਜੇ (m == ਨਿਰਾ)
    {
	member_not_ਮਿਲਿਆ_ਸੁਨੇਹਾ(cl, lhs->lhs_type->tt_type, name, namelen);
	ਵਾਪਸ FALSE;
    }

    ਜੇ (IS_ENUM(cl))
    {
	semsg(_(e_enumvalue_str_cannot_be_modified), cl->class_name.string,
		m->ocm_name.string);
	ਵਾਪਸ FALSE;
    }

    // If it is private member variable, then accessing it outside the
    // class is not allowed.
    // If it is a read only class variable, then it can be modified
    // only inside the class where it is defined.
    ਜੇ ((m->ocm_access != VIM_ACCESS_ALL) &&
	    ((is_object && !inside_class(cctx, cl))
	     || (!is_object && cctx->ctx_ufunc->uf_class != cl)))
    {
	ਅੱਖਰ *ਸੁਨੇਹਾ = (m->ocm_access == VIM_ACCESS_ਨਿੱਜੀ)
				? e_cannot_access_protected_variable_str
				: e_variable_is_not_writable_str;
	emsg_var_cl_define(ਸੁਨੇਹਾ, m->ocm_name.string, 0, cl);
	ਵਾਪਸ FALSE;
    }

    ਵਾਪਸ TRUE;
}

/*
 * Initialize "lhs" with default values
 */
    ਸਥਿਰ ਖਾਲੀ
lhs_init_defaults(lhs_T *lhs)
{
    CLEAR_ਸੂਚਕ(lhs);
    lhs->lhs_dest = dest_local;
    lhs->lhs_vimvaridx = -1;
    lhs->lhs_scriptvar_idx = -1;
    lhs->lhs_member_idx = -1;
}

/*
 * When compiling a LHS variable name, find the end of the destination and the
 * end of the variable name.
 */
    ਸਥਿਰ ਪੂਰਨ
lhs_ਲੱਭੋ_var_ਸਿਰਾ(
    lhs_T	*lhs,
    char_u	*var_ਸ਼ੁਰੂ,
    ਪੂਰਨ		is_decl,
    char_u	**var_endp)
{
    char_u  *var_ਸਿਰਾ = *var_endp;

    // "lhs_dest_end" is the end of the destination, including "[expr]" or
    // ".name".
    // "var_end" is the end of the variable/option/etc. name.
    lhs->lhs_dest_ਸਿਰਾ = skip_var_one(var_ਸ਼ੁਰੂ, FALSE);
    ਜੇ (*var_ਸ਼ੁਰੂ == '@')
    {
	ਜੇ (!valid_dest_reg(var_ਸ਼ੁਰੂ[1]))
	    ਵਾਪਸ FAIL;
	var_ਸਿਰਾ = var_ਸ਼ੁਰੂ + 2;
    }
    ਨਹੀਂਤਾਂ
    {
	// skip over the leading "&", "&l:", "&g:" and "$"
	var_ਸਿਰਾ = skip_option_env_lead(var_ਸ਼ੁਰੂ);
	var_ਸਿਰਾ = ਮੰਜ਼ਿਲ_name_ਸਿਰਾ(var_ਸਿਰਾ, TRUE);
    }

    // "a: type" is declaring variable "a" with a type, not dict "a:".
    ਜੇ (is_decl && lhs->lhs_dest_ਸਿਰਾ == var_ਸ਼ੁਰੂ + 2
					&& lhs->lhs_dest_ਸਿਰਾ[-1] == ':')
	--lhs->lhs_dest_ਸਿਰਾ;
    ਜੇ (is_decl && var_ਸਿਰਾ == var_ਸ਼ੁਰੂ + 2 && var_ਸਿਰਾ[-1] == ':')
	--var_ਸਿਰਾ;

    lhs->lhs_ਸਿਰਾ = lhs->lhs_dest_ਸਿਰਾ;
    *var_endp = var_ਸਿਰਾ;

    ਵਾਪਸ OK;
}

/*
 * Set various fields in "lhs"
 */
    ਸਥਿਰ ਪੂਰਨ
lhs_init(
    lhs_T	*lhs,
    char_u	*var_ਸ਼ੁਰੂ,
    ਪੂਰਨ		is_decl,
    ਪੂਰਨ		heredoc,
    char_u	**var_endp)
{
    char_u *var_ਸਿਰਾ = *var_endp;

    lhs_init_defaults(lhs);

    // Find the end of the variable and the destination
    ਜੇ (lhs_ਲੱਭੋ_var_ਸਿਰਾ(lhs, var_ਸ਼ੁਰੂ, is_decl, &var_ਸਿਰਾ) == FAIL)
	ਵਾਪਸ FAIL;

    // compute the length of the destination without "[expr]" or ".name"
    lhs->lhs_varlen = var_ਸਿਰਾ - var_ਸ਼ੁਰੂ;
    lhs->lhs_varlen_ਕੁੱਲ = lhs->lhs_varlen;
    lhs->lhs_name = vim_strnsave(var_ਸ਼ੁਰੂ, lhs->lhs_varlen);
    ਜੇ (lhs->lhs_name == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    ਜੇ (lhs->lhs_dest_ਸਿਰਾ > var_ਸ਼ੁਰੂ + lhs->lhs_varlen)
	// Something follows after the variable: "var[idx]" or "var.key".
	lhs->lhs_has_ਸੂਚਕਾਂਕ = TRUE;

    lhs->lhs_type = heredoc ? &t_list_string : &t_any;

    *var_endp = var_ਸਿਰਾ;

    ਵਾਪਸ OK;
}

/*
 * Compile a LHS class variable name.
 */
    ਸਥਿਰ ਪੂਰਨ
compile_lhs_class_variable(
    cctx_T	*cctx,
    lhs_T	*lhs,
    class_T	*defcl,
    ਪੂਰਨ		is_decl)
{
    ਜੇ (cctx->ctx_ufunc->uf_defclass != defcl)
    {
	// A class variable can be accessed without the class name
	// only inside a class.
	semsg(_(e_class_variable_str_accessible_only_inside_class_str),
		lhs->lhs_name, defcl->class_name.string);
	ਵਾਪਸ FAIL;
    }

    ਜੇ (is_decl)
    {
	semsg(_(e_variable_already_declared_ਵਿੱਚ_class_str), lhs->lhs_name);
	ਵਾਪਸ FAIL;
    }

    ocmember_T	*m = &defcl->class_class_members[lhs->lhs_classmember_idx];
    ਜੇ (oc_var_check_ro(defcl, m))
	ਵਾਪਸ FAIL;

    lhs->lhs_dest = dest_class_member;
    // The class variable is defined either in the current class or
    // in one of the parent class in the hierarchy.
    lhs->lhs_class = defcl;
    lhs->lhs_type = oc_member_type_by_idx(defcl, FALSE,
						lhs->lhs_classmember_idx);

    ਵਾਪਸ OK;
}

/*
 * Compile an imported LHS variable
 */
    ਸਥਿਰ ਪੂਰਨ
compile_lhs_import_var(
    lhs_T	*lhs,
    imported_T	*import,
    char_u	*var_ਸ਼ੁਰੂ,
    char_u	**var_endp,
    char_u	**rawnamep)
{
    char_u	*var_ਸਿਰਾ = *var_endp;
    char_u	*dot = vim_strchr(var_ਸ਼ੁਰੂ, '.');
    char_u	*p;

    // for an import the name is what comes after the dot
    ਜੇ (dot == ਨਿਰਾ)
    {
	semsg(_(e_no_dot_after_imported_name_str), var_ਸ਼ੁਰੂ);
	ਵਾਪਸ FAIL;
    }

    p = skipwhite(dot + 1);
    var_ਸਿਰਾ = ਮੰਜ਼ਿਲ_name_ਸਿਰਾ(p, TRUE);
    ਜੇ (var_ਸਿਰਾ == p)
    {
	semsg(_(e_missing_name_after_imported_name_str), var_ਸ਼ੁਰੂ);
	ਵਾਪਸ FAIL;
    }

    vim_free(lhs->lhs_name);
    lhs->lhs_varlen = var_ਸਿਰਾ - p;
    lhs->lhs_name = vim_strnsave(p, lhs->lhs_varlen);
    ਜੇ (lhs->lhs_name == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    *rawnamep = lhs->lhs_name;
    lhs->lhs_scriptvar_sid = import->imp_sid;

    // TODO: where do we check this name is exported?

    // Check if something follows: "exp.var[idx]" or
    // "exp.var.key".
    lhs->lhs_has_ਸੂਚਕਾਂਕ = lhs->lhs_dest_ਸਿਰਾ > skipwhite(var_ਸਿਰਾ);

    *var_endp = var_ਸਿਰਾ;

    ਵਾਪਸ OK;
}

/*
 * Process a script-local variable when compiling a LHS variable name.
 */
    ਸਥਿਰ ਪੂਰਨ
compile_lhs_script_var(
    cctx_T	*cctx,
    lhs_T	*lhs,
    char_u	*var_ਸ਼ੁਰੂ,
    char_u	*var_ਸਿਰਾ,
    ਪੂਰਨ		is_decl)
{
    ਪੂਰਨ		script_namespace = FALSE;
    ਪੂਰਨ		script_var = FALSE;
    imported_T	*import;
    char_u	*var_name;
    size_t	var_name_len;

    ਜੇ (lhs->lhs_varlen > 1 && STRNCMP(var_ਸ਼ੁਰੂ, "s:", 2) == 0)
	script_namespace = TRUE;

    ਜੇ (script_namespace)
    {
	var_name = var_ਸ਼ੁਰੂ + 2;
	var_name_len = lhs->lhs_varlen - 2;
    }
    ਨਹੀਂਤਾਂ
    {
	var_name = var_ਸ਼ੁਰੂ;
	var_name_len = lhs->lhs_varlen;
    }

    ਜੇ (script_var_exists(var_name, var_name_len, cctx, ਨਿਰਾ) == OK)
	script_var = TRUE;

    import = ਲੱਭੋ_imported(var_ਸ਼ੁਰੂ, lhs->lhs_varlen, FALSE);

    ਜੇ (script_namespace || script_var || import != ਨਿਰਾ)
    {
	char_u *rawname = lhs->lhs_name + (lhs->lhs_name[1] == ':' ? 2 : 0);

	ਜੇ (script_namespace && current_script_is_vim9())
	{
	    semsg(_(e_cannot_use_s_colon_ਵਿੱਚ_vim9_script_str), var_ਸ਼ੁਰੂ);
	    ਵਾਪਸ FAIL;
	}

	ਜੇ (is_decl)
	{
	    ਜੇ (script_namespace)
		semsg(_(e_cannot_declare_script_variable_ਵਿੱਚ_function_str),
			lhs->lhs_name);
	    ਨਹੀਂਤਾਂ
		semsg(_(e_variable_already_declared_ਵਿੱਚ_script_str),
			lhs->lhs_name);
	    ਵਾਪਸ FAIL;
	}
	ਨਹੀਂਤਾਂ ਜੇ (cctx->ctx_ufunc->uf_script_ctx_ਸੰਸਕਰਣ == SCRIPT_ਸੰਸਕਰਣ_VIM9
		&& script_namespace
		&& !script_var && import == ਨਿਰਾ)
	{
	    semsg(_(e_unknown_variable_str), lhs->lhs_name);
	    ਵਾਪਸ FAIL;
	}

	lhs->lhs_dest = current_script_is_vim9() ? dest_script_v9 :
								dest_script;

	// existing script-local variables should have a type
	lhs->lhs_scriptvar_sid = current_sctx.sc_sid;
	ਜੇ (import != ਨਿਰਾ)
	{
	    ਜੇ (compile_lhs_import_var(lhs, import, var_ਸ਼ੁਰੂ, &var_ਸਿਰਾ,
							&rawname) == FAIL)
		ਵਾਪਸ FAIL;
	}

	ਜੇ (SCRIPT_ID_VALID(lhs->lhs_scriptvar_sid))
	{
	    // Check writable only when no index follows.
	    lhs->lhs_scriptvar_idx = get_script_ਵਸਤੂ_idx(
					lhs->lhs_scriptvar_sid, rawname,
					lhs->lhs_has_ਸੂਚਕਾਂਕ ?  ਸੌਂਪੋ_FINAL :
					ਸੌਂਪੋ_CONST, cctx, ਨਿਰਾ);
	    ਜੇ (lhs->lhs_scriptvar_idx >= 0)
	    {
		scriptitem_T *si = SCRIPT_ਵਸਤੂ(lhs->lhs_scriptvar_sid);
		svar_T	 *sv = ((svar_T *)si->sn_var_vals.ga_ਅੰਕੜੇ)
						+ lhs->lhs_scriptvar_idx;

		lhs->lhs_type = sv->sv_type;
	    }
	}

	ਵਾਪਸ OK;
    }

    ਵਾਪਸ check_defined(var_ਸ਼ੁਰੂ, lhs->lhs_varlen, cctx, ਨਿਰਾ, FALSE);
}

/*
 * Compile the LHS destination.
 */
    ਸਥਿਰ ਪੂਰਨ
compile_lhs_var_dest(
    cctx_T	*cctx,
    lhs_T	*lhs,
    ਪੂਰਨ		cmdidx,
    char_u	*var_ਸ਼ੁਰੂ,
    char_u	*var_ਸਿਰਾ,
    ਪੂਰਨ		is_decl,
    ਪੂਰਨ		has_cmd)	// "var" before "var_start"
{
    ਪੂਰਨ	    declare_error = FALSE;

    ਜੇ (get_var_dest(lhs->lhs_name, &lhs->lhs_dest, cmdidx,
				lhs->lhs_has_ਸੂਚਕਾਂਕ,
				&lhs->lhs_opt_flags, &lhs->lhs_vimvaridx,
				&lhs->lhs_type, cctx) == FAIL)
	ਵਾਪਸ FAIL;

    ਜੇ (lhs->lhs_dest != dest_local && cmdidx != CMD_const
						&& cmdidx != CMD_final)
    {
	// Specific kind of variable recognized.
	declare_error = is_decl;
    }
    ਨਹੀਂਤਾਂ
    {
	class_T	*defcl;

	// No specific kind of variable recognized, just a name.
	ਜੇ (check_reserved_name(lhs->lhs_name, lhs->lhs_has_ਸੂਚਕਾਂਕ
						&& *var_ਸਿਰਾ == '.') == FAIL)
	    ਵਾਪਸ FAIL;

	ਜੇ (lookup_local(var_ਸ਼ੁਰੂ, lhs->lhs_varlen, &lhs->lhs_local_lvar,
								cctx) == OK)
	{
	    lhs->lhs_lvar = &lhs->lhs_local_lvar;
	}
	ਨਹੀਂਤਾਂ
	{
	    CLEAR_ਖੇਤਰ(lhs->lhs_arg_lvar);
	    ਜੇ (arg_exists(var_ਸ਼ੁਰੂ, lhs->lhs_varlen,
			&lhs->lhs_arg_lvar.lv_idx, &lhs->lhs_arg_lvar.lv_type,
			&lhs->lhs_arg_lvar.lv_from_outer, cctx) == OK)
	    {
		ਜੇ (is_decl)
		{
		    semsg(_(e_str_is_ਵਰਤਿਆ_as_ਦਲੀਲ), lhs->lhs_name);
		    ਵਾਪਸ FAIL;
		}
		lhs->lhs_lvar = &lhs->lhs_arg_lvar;
	    }
	}

	ਜੇ (lhs->lhs_lvar != ਨਿਰਾ)
	{
	    ਜੇ (is_decl)
	    {
		// if we come here with what looks like an assignment like
		// .= but which has been rejected by assignment_len() from
		// may_compile_assignment give a better error message
		char_u *p = skipwhite(lhs->lhs_ਸਿਰਾ);
		ਜੇ (p[0] == '.' && p[1] == '=')
		    emsg(_(e_dot_equal_not_supported_with_script_ਸੰਸਕਰਣ_two));
		ਨਹੀਂਤਾਂ ਜੇ (p[0] == ':' && !has_cmd)
		    // type specified in an assignment without "var"
		    semsg(_(e_trailing_characters_str), p);
		ਨਹੀਂਤਾਂ
		    semsg(_(e_variable_already_declared_str), lhs->lhs_name);
		ਵਾਪਸ FAIL;
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ ((lhs->lhs_classmember_idx = cctx_class_member_idx(
			cctx, var_ਸ਼ੁਰੂ, lhs->lhs_varlen, &defcl)) >= 0)
	{
	    ਜੇ (compile_lhs_class_variable(cctx, lhs, defcl, is_decl)
		    == FAIL)
		ਵਾਪਸ FAIL;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (compile_lhs_script_var(cctx, lhs, var_ਸ਼ੁਰੂ, var_ਸਿਰਾ,
			is_decl) == FAIL)
		ਵਾਪਸ FAIL;
	}
    }

    ਜੇ (declare_error)
    {
	vim9_declare_error(lhs->lhs_name);
	ਵਾਪਸ FAIL;
    }

    ਵਾਪਸ OK;
}

/*
 * When compiling a LHS variable name, for a class or an object, set the LHS
 * member type.
 */
    ਸਥਿਰ ਪੂਰਨ
compile_lhs_ਸੈੱਟ_oc_member_type(
    cctx_T	*cctx,
    lhs_T	*lhs,
    char_u	*var_ਸ਼ੁਰੂ)
{
    class_T	*cl = lhs->lhs_type->tt_class;
    ਪੂਰਨ		is_object = lhs->lhs_type->tt_type == VAR_OBJECT;
    char_u	*name = var_ਸ਼ੁਰੂ + lhs->lhs_varlen + 1;
    size_t	namelen = lhs->lhs_ਸਿਰਾ - var_ਸ਼ੁਰੂ - lhs->lhs_varlen - 1;

    ocmember_T	*m = member_lookup(cl, lhs->lhs_type->tt_type,
	    name, namelen, &lhs->lhs_member_idx);
    ਜੇ (m == ਨਿਰਾ)
    {
	member_not_ਮਿਲਿਆ_ਸੁਨੇਹਾ(cl, lhs->lhs_type->tt_type, name, namelen);
	ਵਾਪਸ FAIL;
    }

    ਜੇ (IS_ENUM(cl))
    {
	ਜੇ (!inside_class(cctx, cl))
	{
	    semsg(_(e_enumvalue_str_cannot_be_modified),
		    cl->class_name.string, m->ocm_name.string);
	    ਵਾਪਸ FAIL;
	}
	ਜੇ (lhs->lhs_type->tt_type == VAR_OBJECT &&
		lhs->lhs_member_idx < 2)
	{
	    ਅੱਖਰ *ਸੁਨੇਹਾ = lhs->lhs_member_idx == 0 ?
		e_enum_str_name_cannot_be_modified :
		e_enum_str_ordinal_cannot_be_modified;
	    semsg(_(ਸੁਨੇਹਾ), cl->class_name.string);
	    ਵਾਪਸ FAIL;
	}
    }

    // If it is private member variable, then accessing it outside the
    // class is not allowed.
    // If it is a read only class variable, then it can be modified
    // only inside the class where it is defined.
    ਜੇ ((m->ocm_access != VIM_ACCESS_ALL) &&
	    ((is_object && !inside_class(cctx, cl))
	     || (!is_object && cctx->ctx_ufunc->uf_defclass != cl)))
    {
	ਅੱਖਰ *ਸੁਨੇਹਾ = (m->ocm_access == VIM_ACCESS_ਨਿੱਜੀ)
	    ? e_cannot_access_protected_variable_str
	    : e_variable_is_not_writable_str;
	emsg_var_cl_define(ਸੁਨੇਹਾ, m->ocm_name.string, 0, cl);
	ਵਾਪਸ FAIL;
    }

    ਜੇ (!IS_CONSTRUCTOR_ਵਿਧੀ(cctx->ctx_ufunc)
	    && oc_var_check_ro(cl, m))
	ਵਾਪਸ FAIL;

    lhs->lhs_member_type = m->ocm_type;

    ਵਾਪਸ OK;
}

/*
 * When compiling a LHS variable, set the LHS variable type.
 */
    ਸਥਿਰ ਪੂਰਨ
compile_lhs_ਸੈੱਟ_type(cctx_T *cctx, lhs_T *lhs, char_u *var_ਸਿਰਾ, ਪੂਰਨ is_decl)
{
    ਜੇ (is_decl && *skipwhite(var_ਸਿਰਾ) == ':')
    {
	char_u *p;

	// parse optional type: "let var: type = expr"
	ਜੇ (VIM_ISWHITE(*var_ਸਿਰਾ))
	{
	    semsg(_(e_no_white_space_allowed_before_colon_str), var_ਸਿਰਾ);
	    ਵਾਪਸ FAIL;
	}

	ਜੇ (!VIM_ISWHITE(var_ਸਿਰਾ[1]))
	{
	    semsg(_(e_white_space_required_after_str_str), ":", var_ਸਿਰਾ);
	    ਵਾਪਸ FAIL;
	}

	p = skipwhite(var_ਸਿਰਾ + 1);
	lhs->lhs_type = parse_type(&p, cctx->ctx_type_list, cctx->ctx_ufunc, cctx, TRUE);
	ਜੇ (lhs->lhs_type == ਨਿਰਾ
		|| !valid_declaration_type(lhs->lhs_type))
	    ਵਾਪਸ FAIL;

	lhs->lhs_has_type = TRUE;
	lhs->lhs_ਸਿਰਾ = p;
    }
    ਨਹੀਂਤਾਂ ਜੇ (lhs->lhs_lvar != ਨਿਰਾ)
	lhs->lhs_type = lhs->lhs_lvar->lv_type;

    ਵਾਪਸ OK;
}

/*
 * Returns TRUE if "lhs" is a concatenable string.
 */
    ਸਥਿਰ ਪੂਰਨ
lhs_concatenable(lhs_T *lhs)
{
    ਵਾਪਸ lhs->lhs_dest == dest_global
		|| lhs->lhs_has_ਸੂਚਕਾਂਕ
		|| lhs->lhs_type->tt_type == VAR_STRING
		|| lhs->lhs_type->tt_type == VAR_ANY;
}

/*
 * Create a new local variable when compiling a LHS variable.
 */
    ਸਥਿਰ ਪੂਰਨ
compile_lhs_new_local_var(
    cctx_T	*cctx,
    lhs_T	*lhs,
    char_u	*var_ਸ਼ੁਰੂ,
    ਪੂਰਨ		cmdidx,
    ਪੂਰਨ		oplen,
    ਪੂਰਨ		is_decl,
    ਪੂਰਨ		has_cmd,
    ਪੂਰਨ		heredoc)
{
    ਜੇ (oplen > 1 && !heredoc)
    {
	// +=, /=, etc. require an existing variable
	semsg(_(e_cannot_use_operator_on_new_variable_str), lhs->lhs_name);
	ਵਾਪਸ FAIL;
    }

    ਜੇ (!is_decl || (lhs->lhs_has_ਸੂਚਕਾਂਕ && !has_cmd
					&& cctx->ctx_skip != SKIP_YES))
    {
	semsg(_(e_unknown_variable_str), lhs->lhs_name);
	ਵਾਪਸ FAIL;
    }

    // Check the name is valid for a funcref.
    ਜੇ (lhs->lhs_type->tt_type == VAR_FUNC
				|| lhs->lhs_type->tt_type == VAR_PARTIAL)
    {
	ਜੇ (var_wrong_func_name(lhs->lhs_name, TRUE))
	    ਵਾਪਸ FAIL;
    }

    // New local variable.
    ਪੂਰਨ ਸੌਂਪੋ;
    ਚੋਣ (cmdidx)
    {
	ਹਾਲਤ CMD_final:
	    ਸੌਂਪੋ = ਸੌਂਪੋ_FINAL; ਰੋਕੋ;
	ਹਾਲਤ CMD_const:
	    ਸੌਂਪੋ = ਸੌਂਪੋ_CONST; ਰੋਕੋ;
	ਮੂਲ:
	    ਸੌਂਪੋ = ਸੌਂਪੋ_VAR; ਰੋਕੋ;
    }

    lhs->lhs_lvar = reserve_local(cctx, var_ਸ਼ੁਰੂ, lhs->lhs_varlen, ਸੌਂਪੋ,
							lhs->lhs_type);
    ਜੇ (lhs->lhs_lvar == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    lhs->lhs_new_local = TRUE;

    ਵਾਪਸ OK;
}

/*
 * When compiling a LHS variable name, set the LHS member type.
 */
    ਸਥਿਰ ਪੂਰਨ
compile_lhs_ਸੈੱਟ_member_type(
    cctx_T	*cctx,
    lhs_T	*lhs,
    char_u	*var_ਸ਼ੁਰੂ,
    ਪੂਰਨ		is_decl,
    ਪੂਰਨ		has_cmd)
{
    lhs->lhs_member_type = lhs->lhs_type;

    ਜੇ (!lhs->lhs_has_ਸੂਚਕਾਂਕ)
	ਵਾਪਸ OK;

    char_u	*after = var_ਸ਼ੁਰੂ + lhs->lhs_varlen;
    char_u	*p;

    // Something follows after the variable: "var[idx]" or "var.key".
    ਜੇ (is_decl && cctx->ctx_skip != SKIP_YES)
    {
	ਜੇ (has_cmd)
	    emsg(_(e_cannot_use_ਸੂਚਕਾਂਕ_when_declaring_variable));
	ਨਹੀਂਤਾਂ
	    semsg(_(e_unknown_variable_str), lhs->lhs_name);
	ਵਾਪਸ FAIL;
    }

    // Now: var_start[lhs->lhs_varlen] is '[' or '.'
    // Only the last index is used below, if there are others
    // before it generate code for the expression.  Thus for
    // "ll[1][2]" the expression is "ll[1]" and "[2]" is the index.
    ਲਈ (;;)
    {
	p = skip_ਸੂਚਕਾਂਕ(after);
	ਜੇ (*p != '[' && *p != '.')
	{
	    lhs->lhs_varlen_ਕੁੱਲ = p - var_ਸ਼ੁਰੂ;
	    ਰੋਕੋ;
	}
	after = p;
    }
    ਜੇ (after > var_ਸ਼ੁਰੂ + lhs->lhs_varlen)
    {
	lhs->lhs_varlen = after - var_ਸ਼ੁਰੂ;
	lhs->lhs_dest = dest_ਵਿਅੰਜਕ;
	// We don't know the type before evaluating the expression,
	// use "any" until then.  The member index is for the first name,
	// not for the last index.
	lhs->lhs_type = &t_any;
	lhs->lhs_member_idx = -1;
    }

    ਪੂਰਨ use_class = lhs->lhs_type != ਨਿਰਾ
	&& (lhs->lhs_type->tt_type == VAR_CLASS
		|| lhs->lhs_type->tt_type == VAR_OBJECT);

    ਜੇ (lhs->lhs_type == ਨਿਰਾ
	    || (use_class ? lhs->lhs_type->tt_class == ਨਿਰਾ
		: lhs->lhs_type->tt_member == ਨਿਰਾ))
    {
	lhs->lhs_member_type = &t_any;
    }
    ਨਹੀਂਤਾਂ ਜੇ (use_class)
    {
	// for an object or class member get the type of the member
	ਜੇ (compile_lhs_ਸੈੱਟ_oc_member_type(cctx, lhs, var_ਸ਼ੁਰੂ) == FAIL)
	    ਵਾਪਸ FAIL;
    }
    ਨਹੀਂਤਾਂ
	lhs->lhs_member_type = lhs->lhs_type->tt_member;

    ਵਾਪਸ OK;
}

/*
 * Figure out the LHS type and other properties for an assignment or one item
 * of ":unlet" with an index.
 * Returns OK or FAIL.
 */
    ਪੂਰਨ
compile_lhs(
	char_u	    *var_ਸ਼ੁਰੂ,
	lhs_T	    *lhs,
	cmdidx_T    cmdidx,
	ਪੂਰਨ	    heredoc,
	ਪੂਰਨ	    has_cmd,	    // "var" before "var_start"
	ਪੂਰਨ	    oplen,
	cctx_T	    *cctx)
{
    char_u	*var_ਸਿਰਾ = ਨਿਰਾ;
    ਪੂਰਨ		is_decl = is_decl_command(cmdidx);

    ਜੇ (lhs_init(lhs, var_ਸ਼ੁਰੂ, is_decl, heredoc, &var_ਸਿਰਾ) == FAIL)
	ਵਾਪਸ FAIL;

    ਜੇ (cctx->ctx_skip != SKIP_YES)
    {
	// compile the LHS destination
	ਜੇ (compile_lhs_var_dest(cctx, lhs, cmdidx, var_ਸ਼ੁਰੂ, var_ਸਿਰਾ,
						is_decl, has_cmd) == FAIL)
	    ਵਾਪਸ FAIL;
    }

    // handle "a:name" as a name, not index "name" in "a"
    ਜੇ (lhs->lhs_varlen > 1 || var_ਸ਼ੁਰੂ[lhs->lhs_varlen] != ':')
	var_ਸਿਰਾ = lhs->lhs_dest_ਸਿਰਾ;

    ਜੇ (lhs->lhs_dest != dest_option && lhs->lhs_dest != dest_func_option)
    {
	// set the LHS variable type
	ਜੇ (compile_lhs_ਸੈੱਟ_type(cctx, lhs, var_ਸਿਰਾ, is_decl) == FAIL)
	    ਵਾਪਸ FAIL;
    }

    ਜੇ (oplen == 3 && !heredoc && !lhs_concatenable(lhs))
    {
	emsg(_(e_can_only_concatenate_ਮੰਜ਼ਿਲ_string));
	ਵਾਪਸ FAIL;
    }

    ਜੇ (lhs->lhs_lvar == ਨਿਰਾ && lhs->lhs_dest == dest_local
						&& cctx->ctx_skip != SKIP_YES)
    {
	ਜੇ (compile_lhs_new_local_var(cctx, lhs, var_ਸ਼ੁਰੂ, cmdidx, oplen,
					is_decl, has_cmd, heredoc) == FAIL)
	    ਵਾਪਸ FAIL;
    }

    ਜੇ (compile_lhs_ਸੈੱਟ_member_type(cctx, lhs, var_ਸ਼ੁਰੂ, is_decl, has_cmd)
								== FAIL)
	ਵਾਪਸ FAIL;

    ਵਾਪਸ OK;
}

/*
 * Figure out the LHS and check a few errors.
 */
    ਪੂਰਨ
compile_ਸੌਂਪੋ_lhs(
	char_u	    *var_ਸ਼ੁਰੂ,
	lhs_T	    *lhs,
	cmdidx_T    cmdidx,
	ਪੂਰਨ	    is_decl,
	ਪੂਰਨ	    heredoc,
	ਪੂਰਨ	    has_cmd,	    // "var" before "var_start"
	ਪੂਰਨ	    oplen,
	cctx_T	    *cctx)
{
    ਜੇ (compile_lhs(var_ਸ਼ੁਰੂ, lhs, cmdidx, heredoc, has_cmd, oplen, cctx)
								       == FAIL)
	ਵਾਪਸ FAIL;

    ਜੇ (!lhs->lhs_has_ਸੂਚਕਾਂਕ && lhs->lhs_lvar == &lhs->lhs_arg_lvar)
    {
	semsg(_(e_cannot_ਸੌਂਪੋ_ਮੰਜ਼ਿਲ_ਦਲੀਲ_str), lhs->lhs_name);
	ਵਾਪਸ FAIL;
    }
    ਜੇ (!is_decl && lhs->lhs_lvar != ਨਿਰਾ
			   && lhs->lhs_lvar->lv_const != ਸੌਂਪੋ_VAR
			   && !lhs->lhs_has_ਸੂਚਕਾਂਕ)
    {
	semsg(_(e_cannot_ਸੌਂਪੋ_ਮੰਜ਼ਿਲ_constant_str), lhs->lhs_name);
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * Return TRUE if "lhs" has a range index: "[expr : expr]".
 */
    ਸਥਿਰ ਪੂਰਨ
has_list_ਸੂਚਕਾਂਕ(char_u *idx_ਸ਼ੁਰੂ, cctx_T *cctx)
{
    char_u  *p = idx_ਸ਼ੁਰੂ;
    ਪੂਰਨ	    save_skip;

    ਜੇ (*p != '[')
	ਵਾਪਸ FALSE;

    p = skipwhite(p + 1);
    ਜੇ (*p == ':')
	ਵਾਪਸ TRUE;

    save_skip = cctx->ctx_skip;
    cctx->ctx_skip = SKIP_YES;
    (ਖਾਲੀ)compile_expr0(&p, cctx);
    cctx->ctx_skip = save_skip;
    ਵਾਪਸ *skipwhite(p) == ':';
}

/*
 * For an assignment with an index, compile the "idx" in "var[idx]" or "key" in
 * "var.key".
 */
    ਸਥਿਰ ਪੂਰਨ
compile_ਸੌਂਪੋ_ਸੂਚਕਾਂਕ(
	char_u	*var_ਸ਼ੁਰੂ,
	lhs_T	*lhs,
	ਪੂਰਨ	*range,
	cctx_T	*cctx)
{
    size_t	varlen = lhs->lhs_varlen;
    char_u	*p;
    ਪੂਰਨ		r = OK;
    ਪੂਰਨ		need_white_before = TRUE;
    ਪੂਰਨ		ਖਾਲੀ_ਥਾਵਾਂ_ਦੂਜਾ;

    p = var_ਸ਼ੁਰੂ + varlen;
    ਜੇ (*p == '[')
    {
	p = skipwhite(p + 1);
	ਜੇ (*p == ':')
	{
	    // empty first index, push zero
	    r = generate_PUSHNR(cctx, 0);
	    need_white_before = FALSE;
	}
	ਨਹੀਂਤਾਂ
	    r = compile_expr0(&p, cctx);

	ਜੇ (r == OK && *skipwhite(p) == ':')
	{
	    // unlet var[idx : idx]
	    // blob[idx : idx] = value
	    *range = TRUE;
	    p = skipwhite(p);
	    ਖਾਲੀ_ਥਾਵਾਂ_ਦੂਜਾ = *skipwhite(p + 1) == ']';
	    ਜੇ ((need_white_before && !IS_WHITE_OR_NUL(p[-1]))
		    || (!ਖਾਲੀ_ਥਾਵਾਂ_ਦੂਜਾ && !IS_WHITE_OR_NUL(p[1])))
	    {
		semsg(_(e_white_space_required_before_and_after_str_at_str),
								      ":", p);
		ਵਾਪਸ FAIL;
	    }
	    p = skipwhite(p + 1);
	    ਜੇ (*p == ']')
		// empty second index, push "none"
		r = generate_PUSHSPEC(cctx, VVAL_NONE);
	    ਨਹੀਂਤਾਂ
		r = compile_expr0(&p, cctx);
	}

	ਜੇ (r == OK && *skipwhite(p) != ']')
	{
	    // this should not happen
	    emsg(_(e_missing_closing_ਵਰਗਫਲ_brace));
	    r = FAIL;
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (lhs->lhs_member_idx >= 0)
    {
	// object member index
	r = generate_PUSHNR(cctx, lhs->lhs_member_idx);
    }
    ਨਹੀਂਤਾਂ // if (*p == '.')
    {
	char_u *ਕੁੰਜੀ_ਸਿਰਾ = ਮੰਜ਼ਿਲ_name_ਸਿਰਾ(p + 1, TRUE);
	char_u *ਕੁੰਜੀ = vim_strnsave(p + 1, ਕੁੰਜੀ_ਸਿਰਾ - p - 1);

	r = generate_PUSHS(cctx, &ਕੁੰਜੀ);
    }
    ਵਾਪਸ r;
}

/*
 * For a LHS with an index, load the variable to be indexed.
 */
    ਸਥਿਰ ਪੂਰਨ
compile_ਲੱਦੋ_lhs(
	lhs_T	*lhs,
	char_u	*var_ਸ਼ੁਰੂ,
	type_T	*rhs_type,
	cctx_T	*cctx)
{
    ਜੇ (lhs->lhs_dest == dest_ਵਿਅੰਜਕ)
    {
	size_t	    varlen = lhs->lhs_varlen;
	ਪੂਰਨ	    c = var_ਸ਼ੁਰੂ[varlen];
	ਪੂਰਨ	    ਸਤਰਾਂ_len = cctx->ctx_ufunc->uf_ਸਤਰਾਂ.ga_len;
	ਪੂਰਨ	    res;

	// Evaluate "ll[expr]" of "ll[expr][idx]".  End the line with a NUL and
	// limit the lines array length to avoid skipping to a following line.
	var_ਸ਼ੁਰੂ[varlen] = NUL;
	cctx->ctx_ufunc->uf_ਸਤਰਾਂ.ga_len = cctx->ctx_lnum + 1;
	char_u *p = var_ਸ਼ੁਰੂ;
	res = compile_expr0(&p, cctx);
	var_ਸ਼ੁਰੂ[varlen] = c;
	cctx->ctx_ufunc->uf_ਸਤਰਾਂ.ga_len = ਸਤਰਾਂ_len;
	ਜੇ (res == FAIL || p != var_ਸ਼ੁਰੂ + varlen)
	{
	    // this should not happen
	    ਜੇ (res != FAIL)
		emsg(_(e_missing_closing_ਵਰਗਫਲ_brace));
	    ਵਾਪਸ FAIL;
	}

	lhs->lhs_type = cctx->ctx_type_ਢੇਰ.ga_len == 0 ? &t_void
						  : get_type_on_ਢੇਰ(cctx, 0);

	ਜੇ (lhs->lhs_type->tt_type == VAR_CLASS
		|| (lhs->lhs_type->tt_type == VAR_OBJECT
		    && lhs->lhs_type != &t_object_any))
	{
	    // Check whether the class or object variable is modifiable
	    ਜੇ (!lhs_class_member_modifiable(lhs, var_ਸ਼ੁਰੂ, cctx))
		ਵਾਪਸ FAIL;
	}

	// Now we can properly check the type.  The variable is indexed, thus
	// we need the member type.  For a class or object we don't know the
	// type yet, it depends on what member is used.
	// The top item in the stack is the Dict, followed by the key and then
	// the type of the value.
	vartype_T vartype = lhs->lhs_type->tt_type;
	type_T *member_type = lhs->lhs_type->tt_member;
	ਜੇ (rhs_type != ਨਿਰਾ && member_type != ਨਿਰਾ
		&& vartype != VAR_OBJECT && vartype != VAR_CLASS
		&& rhs_type != &t_void
		&& need_type(rhs_type, member_type, 0,
					    -3, 0, cctx, FALSE, FALSE) == FAIL)
	    ਵਾਪਸ FAIL;

	ਵਾਪਸ OK;
    }

    ਵਾਪਸ generate_loadvar(cctx, lhs);
}

/*
 * Produce code for loading "lhs" and also take care of an index.
 * Return OK/FAIL.
 */
    ਪੂਰਨ
compile_ਲੱਦੋ_lhs_with_ਸੂਚਕਾਂਕ(lhs_T *lhs, char_u *var_ਸ਼ੁਰੂ, cctx_T *cctx)
{
    ਜੇ (lhs->lhs_type->tt_type == VAR_OBJECT)
    {
	// "this.value": load "this" object and get the value at index for an
	// object or class member get the type of the member.
	// Also for "obj.value".
	char_u *dot = vim_strchr(var_ਸ਼ੁਰੂ, '.');
	ਜੇ (dot == ਨਿਰਾ)
	{
	    semsg(_(e_missing_dot_after_object_str), lhs->lhs_name);
	    ਵਾਪਸ FAIL;
	}

	class_T	*cl = lhs->lhs_type->tt_class;
	type_T	*type = oc_member_type(cl, TRUE, dot + 1,
					  lhs->lhs_ਸਿਰਾ, &lhs->lhs_member_idx);
	ਜੇ (lhs->lhs_member_idx < 0)
	    ਵਾਪਸ FAIL;

	ਜੇ (dot - var_ਸ਼ੁਰੂ == 4 && STRNCMP(var_ਸ਼ੁਰੂ, "this", 4) == 0)
	{
	    // load "this"
	    lvar_T  *lvar = lhs->lhs_lvar;
	    ਪੂਰਨ	    rc;

	    ਜੇ (lvar->lv_from_outer > 0)
		rc = generate_LOADOUTER(cctx, lvar->lv_idx,
			lvar->lv_from_outer, lvar->lv_loop_ਡੂੰਘਾਈ,
			lvar->lv_loop_idx, type);
	    ਨਹੀਂਤਾਂ
		rc = generate_ਲੱਦੋ(cctx, ISN_ਲੱਦੋ, lvar->lv_idx, ਨਿਰਾ, type);

	    ਜੇ (rc == FAIL)
		ਵਾਪਸ FAIL;
	}
	ਨਹੀਂਤਾਂ
	{
	    // load object variable or argument
	    ਜੇ (compile_ਲੱਦੋ_lhs(lhs, var_ਸ਼ੁਰੂ, lhs->lhs_type, cctx) == FAIL)
		ਵਾਪਸ FAIL;
	}
	ਜੇ (IS_INTERFACE(cl))
	    ਵਾਪਸ generate_GET_ITF_MEMBER(cctx, cl, lhs->lhs_member_idx, type);
	ਵਾਪਸ generate_GET_OBJ_MEMBER(cctx, lhs->lhs_member_idx, type);
    }
    ਨਹੀਂਤਾਂ ਜੇ (lhs->lhs_type->tt_type == VAR_CLASS)
    {
	// "<classname>.value": load class variable "classname.value"
	char_u *dot = vim_strchr(var_ਸ਼ੁਰੂ, '.');
	ਜੇ (dot == ਨਿਰਾ)
	{
	    (ਖਾਲੀ)check_type_is_ਮੁੱਲ(lhs->lhs_type);
	    ਵਾਪਸ FAIL;
	}

	class_T	*cl = lhs->lhs_type->tt_class;
	ocmember_T *m = class_member_lookup(cl, dot + 1,
						lhs->lhs_ਸਿਰਾ - dot - 1,
						&lhs->lhs_member_idx);
	ਜੇ (m == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;

	ਵਾਪਸ generate_CLASSMEMBER(cctx, TRUE, cl, lhs->lhs_member_idx);
    }

    ਜੇ (compile_ਲੱਦੋ_lhs(lhs, var_ਸ਼ੁਰੂ, ਨਿਰਾ, cctx) == FAIL)
	ਵਾਪਸ FAIL;

    ਜੇ (lhs->lhs_has_ਸੂਚਕਾਂਕ)
    {
	ਪੂਰਨ	range = FALSE;
	type_T	*type = get_type_on_ਢੇਰ(cctx, 0);

	// A member of an object or class is not obtained by indexing it.
	ਜੇ (type->tt_type == VAR_CLASS
		|| (type->tt_type == VAR_OBJECT && type != &t_object_any))
	{
	    char_u *p = var_ਸ਼ੁਰੂ + lhs->lhs_varlen;

	    ਵਾਪਸ compile_class_object_ਸੂਚਕਾਂਕ(cctx, &p, type);
	}

	// Get member from list or dict.  First compile the
	// index value.
	ਜੇ (compile_ਸੌਂਪੋ_ਸੂਚਕਾਂਕ(var_ਸ਼ੁਰੂ, lhs, &range, cctx) == FAIL)
	    ਵਾਪਸ FAIL;
	ਜੇ (range)
	{
	    semsg(_(e_cannot_use_range_with_assignment_operator_str),
								    var_ਸ਼ੁਰੂ);
	    ਵਾਪਸ FAIL;
	}

	// Get the member.
	ਜੇ (compile_member(FALSE, ਨਿਰਾ, cctx) == FAIL)
	    ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * Assignment to a list or dict member, or ":unlet" for the item, using the
 * information in "lhs".
 * Returns OK or FAIL.
 */
    ਪੂਰਨ
compile_ਸੌਂਪੋ_unlet(
    char_u	*var_ਸ਼ੁਰੂ,
    lhs_T	*lhs,
    ਪੂਰਨ		is_ਸੌਂਪੋ,
    type_T	*rhs_type,
    cctx_T	*cctx)
{
    vartype_T	dest_type;
    ਪੂਰਨ		range = FALSE;

    ਜੇ (compile_ਸੌਂਪੋ_ਸੂਚਕਾਂਕ(var_ਸ਼ੁਰੂ, lhs, &range, cctx) == FAIL)
	ਵਾਪਸ FAIL;
    ਜੇ (is_ਸੌਂਪੋ && range
	    && lhs->lhs_type->tt_type != VAR_LIST
	    && lhs->lhs_type != &t_blob
	    && lhs->lhs_type != &t_any)
    {
	ਜੇ (lhs->lhs_type->tt_type == VAR_TUPLE)
	    emsg(_(e_cannot_ਟੁਕੜਾ_tuple));
	ਨਹੀਂਤਾਂ
	    semsg(_(e_cannot_use_range_with_assignment_str), var_ਸ਼ੁਰੂ);
	ਵਾਪਸ FAIL;
    }

    // For "expr[idx]" the index is compiled before the expression, thus the
    // resulting type cannot be used here.
    ਜੇ (lhs->lhs_dest == dest_ਵਿਅੰਜਕ
	    || lhs->lhs_type == ਨਿਰਾ || lhs->lhs_type == &t_any)
    {
	// Index on variable of unknown type: check at runtime.
	dest_type = VAR_ANY;
    }
    ਨਹੀਂਤਾਂ
    {
	dest_type = lhs->lhs_type->tt_type;
	ਜੇ (dest_type == VAR_DICT && range)
	{
	    emsg(_(e_cannot_use_range_with_dictionary));
	    ਵਾਪਸ FAIL;
	}
	ਜੇ (dest_type == VAR_DICT
		&& may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
	    ਵਾਪਸ FAIL;
	ਜੇ (dest_type == VAR_LIST || dest_type == VAR_BLOB)
	{
	    type_T *type;

	    ਜੇ (range)
	    {
		type = get_type_on_ਢੇਰ(cctx, 1);
		ਜੇ (need_type(type, &t_ਸੰਖਿਆ, 0,
					    -2, 0, cctx, FALSE, FALSE) == FAIL)
		ਵਾਪਸ FAIL;
	    }
	    type = get_type_on_ਢੇਰ(cctx, 0);
	    ਜੇ ((dest_type != VAR_BLOB && type->tt_type != VAR_SPECIAL)
		    && need_type(type, &t_ਸੰਖਿਆ, 0,
					    -1, 0, cctx, FALSE, FALSE) == FAIL)
		ਵਾਪਸ FAIL;
	}
    }

    ਜੇ (cctx->ctx_skip == SKIP_YES)
	ਵਾਪਸ OK;

    // Load the dict, list or object.  On the stack we then have:
    // - value (for assignment, not for :unlet)
    // - index
    // - for [a : b] second index
    // - variable
    ਜੇ (compile_ਲੱਦੋ_lhs(lhs, var_ਸ਼ੁਰੂ, rhs_type, cctx) == FAIL)
	ਵਾਪਸ FAIL;

    ਜੇ (dest_type == VAR_LIST
	    || dest_type == VAR_DICT
	    || dest_type == VAR_BLOB
	    || dest_type == VAR_CLASS
	    || dest_type == VAR_OBJECT
	    || dest_type == VAR_ANY)
    {
	ਜੇ (is_ਸੌਂਪੋ)
	{
	    ਜੇ (range)
	    {
		ਜੇ (generate_instr_drop(cctx, ISN_STORERANGE, 4) == ਨਿਰਾ)
		    ਵਾਪਸ FAIL;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		isn_T	*isn = generate_instr_drop(cctx, ISN_STOREINDEX, 3);

		ਜੇ (isn == ਨਿਰਾ)
		    ਵਾਪਸ FAIL;
		isn->isn_arg.storeindex.si_vartype = dest_type;
		isn->isn_arg.storeindex.si_class = ਨਿਰਾ;

		ਜੇ (dest_type == VAR_OBJECT)
		{
		    class_T *cl = lhs->lhs_type->tt_class;

		    ਜੇ (IS_INTERFACE(cl))
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
	ਨਹੀਂਤਾਂ ਜੇ (range)
	{
	    ਜੇ (generate_instr_drop(cctx, ISN_UNLETRANGE, 3) == ਨਿਰਾ)
		ਵਾਪਸ FAIL;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (generate_instr_drop(cctx, ISN_UNLETINDEX, 2) == ਨਿਰਾ)
		ਵਾਪਸ FAIL;
	}
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (dest_type == VAR_TUPLE)
	    emsg(_(e_tuple_is_immutable));
	ਨਹੀਂਤਾਂ
	    emsg(_(e_indexable_type_required));
	ਵਾਪਸ FAIL;
    }

    ਵਾਪਸ OK;
}

/*
 * Generate an instruction to push the default value for "vartype".
 * if "dest_local" is TRUE then for some types no instruction is generated.
 * "skip_store" is set to TRUE if no PUSH instruction is generated.
 * Returns OK or FAIL.
 */
    ਸਥਿਰ ਪੂਰਨ
ਧੱਕੋ_default_ਮੁੱਲ(
	cctx_T	    *cctx,
	vartype_T   vartype,
	ਪੂਰਨ	    dest_is_local,
	ਪੂਰਨ	    *skip_store)
{
    ਪੂਰਨ r = OK;

    ਚੋਣ (vartype)
    {
	ਹਾਲਤ VAR_BOOL:
	    r = generate_PUSHBOOL(cctx, VVAL_FALSE);
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_FLOAT:
	    r = generate_PUSHF(cctx, 0.0);
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_STRING:
	    r = generate_PUSHS(cctx, ਨਿਰਾ);
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_BLOB:
	    r = generate_PUSHBLOB(cctx, blob_alloc());
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_FUNC:
	    r = generate_PUSHFUNC(cctx, ਨਿਰਾ, &t_func_void, TRUE);
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_LIST:
	    r = generate_NEWLIST(cctx, 0, FALSE);
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_TUPLE:
	    r = generate_NEWTUPLE(cctx, 0, FALSE);
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_DICT:
	    r = generate_NEWDICT(cctx, 0, FALSE);
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_ਕੰਮ:
	    r = generate_PUSHJOB(cctx);
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_CHANNEL:
	    r = generate_PUSHCHANNEL(cctx);
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_OBJECT:
	    r = generate_PUSHOBJ(cctx);
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_ਸੰਖਿਆ:
	ਹਾਲਤ VAR_UNKNOWN:
	ਹਾਲਤ VAR_ANY:
	ਹਾਲਤ VAR_PARTIAL:
	ਹਾਲਤ VAR_VOID:
	ਹਾਲਤ VAR_INSTR:
	ਹਾਲਤ VAR_CLASS:
	ਹਾਲਤ VAR_TYPEALIAS:
	ਹਾਲਤ VAR_SPECIAL:  // cannot happen
	    // This is skipped for local variables, they are always
	    // initialized to zero.  But in a "for" or "while" loop
	    // the value may have been changed.
	    ਜੇ (dest_is_local && !inside_loop_scope(cctx))
		*skip_store = TRUE;
	    ਨਹੀਂਤਾਂ
		r = generate_PUSHNR(cctx, 0);
	    ਰੋਕੋ;
    }
    ਵਾਪਸ r;
}

/*
 * Compile assignment context.  Used when compiling an assignment statement.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ cac_S cac_T;
ਬਣਤਰ cac_S
{
    cmdidx_T	cac_cmdidx;		// assignment command
    char_u	*cac_nextc;		// next character to parse
    lhs_T	cac_lhs;		// lhs of the assignment
    type_T	*cac_rhs_type;		// rhs type of an assignment
    char_u	*cac_op;		// assignment operator
    ਪੂਰਨ		cac_oplen;		// assignment operator length
    char_u	*cac_var_ਸ਼ੁਰੂ;		// start of the variable names
    char_u	*cac_var_ਸਿਰਾ;		// end of the variable names
    ਪੂਰਨ		cac_var_ਗਿਣਤੀ;		// number of variables in assignment
    ਪੂਰਨ		cac_var_idx;		// variable index in a list
    ਪੂਰਨ		cac_semicolon;		// semicolon in [var1, var2; var3]
    garray_T	*cac_instr;
    ਪੂਰਨ		cac_instr_ਗਿਣਤੀ;
    ਪੂਰਨ		cac_incdec;
    ਪੂਰਨ		cac_did_generate_ਟੁਕੜਾ;
    ਪੂਰਨ		cac_is_decl;
    ਪੂਰਨ		cac_is_const;
    ਪੂਰਨ		cac_ਸ਼ੁਰੂ_lnum;
    type_T	*cac_inferred_type;
    ਪੂਰਨ		cac_skip_store;
};

/*
 * Initialize the compile assignment context.
 */
    ਸਥਿਰ ਖਾਲੀ
compile_ਸੌਂਪੋ_context_init(cac_T *cac, cctx_T *cctx, ਪੂਰਨ cmdidx, char_u *arg)
{
    CLEAR_ਖੇਤਰ(*cac);
    cac->cac_cmdidx = cmdidx;
    cac->cac_instr = &cctx->ctx_instr;
    cac->cac_rhs_type = &t_any;
    cac->cac_is_decl = is_decl_command(cmdidx);
    cac->cac_ਸ਼ੁਰੂ_lnum = SOURCING_LNUM;
    cac->cac_instr_ਗਿਣਤੀ = -1;
    cac->cac_var_ਸਿਰਾ = arg;
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
    ਸਥਿਰ ਪੂਰਨ
compile_ਸੌਂਪੋ_obj_new_arg(char_u **argp, cctx_T *cctx)
{
    char_u *arg = *argp;

    arg += 11;	    // skip "ifargisset"
    ਪੂਰਨ def_arg_idx = getdigits(&arg);
    arg = skipwhite(arg);

    // Use a JUMP_IF_ARG_NOT_SET instruction to skip if the value was not
    // given and the default value is "v:none".
    ਪੂਰਨ ਢੇਰ_ਵਿੱਥ = ਢੇਰ_ਚੌਖਟਾ_ਮਾਪ +
				(cctx->ctx_ufunc->uf_va_name != ਨਿਰਾ ? 1 : 0);
    ਪੂਰਨ def_arg_ਗਿਣਤੀ = cctx->ctx_ufunc->uf_def_ਦਲੀਲਾਂ.ga_len;
    ਪੂਰਨ arg_ਵਿੱਥ = def_arg_idx - def_arg_ਗਿਣਤੀ - ਢੇਰ_ਵਿੱਥ;

    ਜੇ (generate_JUMP_IF_ARG(cctx, ISN_JUMP_IF_ARG_NOT_ਸੈੱਟ,
			     arg_ਵਿੱਥ) == FAIL)
	ਵਾਪਸ FAIL;

    *argp = arg;
    ਵਾਪਸ OK;
}

/*
 * Translate the increment (++) and decrement (--) operators to the
 * corresponding compound operators (+= or -=).
 *
 * Returns OK on success and FAIL on syntax error.
 */
    ਸਥਿਰ ਪੂਰਨ
translate_incdec_op(exarg_T *eap, cac_T *cac)
{
    ਜੇ (VIM_ISWHITE(eap->cmd[2]))
    {
	semsg(_(e_no_white_space_allowed_after_str_str),
		eap->cmdidx == CMD_increment ? "++" : "--", eap->cmd);
	ਵਾਪਸ FAIL;
    }
    cac->cac_op = (char_u *)(eap->cmdidx == CMD_increment ? "+=" : "-=");
    cac->cac_oplen = 2;
    cac->cac_incdec = TRUE;

    ਵਾਪਸ OK;
}

/*
 * Process the operator in an assignment statement.
 */
    ਸਥਿਰ ਪੂਰਨ
compile_ਸੌਂਪੋ_process_operator(
    exarg_T	*eap,
    char_u	*arg,
    cac_T	*cac,
    ਪੂਰਨ		*heredoc,
    char_u	**retstr)
{
    *retstr = ਨਿਰਾ;

    ਜੇ (eap->cmdidx == CMD_increment || eap->cmdidx == CMD_decrement)
	// Change an unary operator to a compound operator
	ਵਾਪਸ translate_incdec_op(eap, cac);

    char_u *sp = cac->cac_nextc;
    cac->cac_nextc = skipwhite(cac->cac_nextc);
    cac->cac_op = cac->cac_nextc;
    cac->cac_oplen = assignment_len(cac->cac_nextc, heredoc);

    ਜੇ (cac->cac_var_ਗਿਣਤੀ > 0 && cac->cac_oplen == 0)
    {
	// can be something like "[1, 2]->func()"
	*retstr = arg;
	ਵਾਪਸ FAIL;
    }

    // need white space before and after the operator
    ਜੇ (cac->cac_oplen > 0 && (!VIM_ISWHITE(*sp)
		|| !IS_WHITE_OR_NUL(cac->cac_op[cac->cac_oplen])))
    {
	error_white_both(cac->cac_op, cac->cac_oplen);
	ਵਾਪਸ FAIL;
    }

    ਵਾਪਸ OK;
}

/*
 * Find the start of an assignment statement.
 */
    ਸਥਿਰ char_u *
compile_ਸੌਂਪੋ_compute_ਸ਼ੁਰੂ(char_u *arg, ਪੂਰਨ var_ਗਿਣਤੀ)
{
    ਜੇ (var_ਗਿਣਤੀ > 0)
	// [var1, var2] = [val1, val2]
	// skip over the "["
	ਵਾਪਸ skipwhite(arg + 1);

    ਵਾਪਸ arg;
}

/*
 * Parse a heredoc assignment starting at "p".  Returns a pointer to the
 * beginning of the heredoc content.
 */
    ਸਥਿਰ char_u *
parse_heredoc_assignment(exarg_T *eap, cctx_T *cctx, cac_T *cac)
{
    // [let] varname =<< [trim] {end}
    eap->ea_getline = exarg_getline;
    eap->cookie = cctx;

    list_T *l = heredoc_get(eap, cac->cac_nextc + 3, FALSE, TRUE);
    ਜੇ (l == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    list_free(l);
    cac->cac_nextc += STRLEN(cac->cac_nextc);

    ਵਾਪਸ cac->cac_nextc;
}

/*
 * Check the type of a RHS expression in a list assignment statement.
 * The RHS expression is already compiled.  So the type is on the stack.
 */
    ਸਥਿਰ ਪੂਰਨ
compile_ਸੌਂਪੋ_list_check_rhs_type(cctx_T *cctx, cac_T *cac)
{
    type_T	*stacktype;

    stacktype = cctx->ctx_type_ਢੇਰ.ga_len == 0 ? &t_void
						: get_type_on_ਢੇਰ(cctx, 0);
    ਜੇ (stacktype->tt_type == VAR_VOID)
    {
	emsg(_(e_cannot_use_void_ਮੁੱਲ));
	ਵਾਪਸ FAIL;
    }

    ਜੇ (stacktype->tt_type != VAR_LIST && stacktype->tt_type != VAR_TUPLE
					&& stacktype->tt_type != VAR_ANY)
    {
	emsg(_(e_list_or_tuple_required));
	ਵਾਪਸ FAIL;
    }

    ਜੇ (need_type(stacktype,
		  stacktype->tt_type == VAR_TUPLE ? &t_tuple_any : &t_list_any,
		  TYPECHK_TUPLE_OK, -1, 0, cctx, FALSE, FALSE) == FAIL)
	ਵਾਪਸ FAIL;

    // The check accepts both a list and a tuple. Keep "any", making it a list
    // would reject a tuple later on.
    ਜੇ (stacktype->tt_type == VAR_ANY)
	ਸੈੱਟ_type_on_ਢੇਰ(cctx, &t_any, 0);

    ਜੇ (stacktype->tt_type == VAR_TUPLE)
    {
	ਜੇ (stacktype->tt_argcount != 1)
	    cac->cac_rhs_type = &t_any;
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (stacktype->tt_flags & TTFLAG_VARARGS)
		cac->cac_rhs_type = stacktype->tt_ਦਲੀਲਾਂ[0]->tt_member;
	    ਨਹੀਂਤਾਂ
		cac->cac_rhs_type = stacktype->tt_ਦਲੀਲਾਂ[0];
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (stacktype->tt_member != ਨਿਰਾ)
	cac->cac_rhs_type = stacktype->tt_member;

    ਵਾਪਸ OK;
}

/*
 * In a list assignment statement, if a constant list was used, check the
 * length.  Returns OK if the length check succeeds.  Returns FAIL otherwise.
 */
    ਸਥਿਰ ਪੂਰਨ
compile_ਸੌਂਪੋ_list_check_ਲੰਮਾਈ(cctx_T *cctx, cac_T *cac)
{
    ਪੂਰਨ	needed_list_len;
    ਪੂਰਨ	did_check = FALSE;

    needed_list_len = cac->cac_semicolon
				? cac->cac_var_ਗਿਣਤੀ - 1
				: cac->cac_var_ਗਿਣਤੀ;
    ਜੇ (cac->cac_instr->ga_len > 0)
    {
	isn_T	*isn = ((isn_T *)cac->cac_instr->ga_ਅੰਕੜੇ) +
						cac->cac_instr->ga_len - 1;

	ਜੇ (isn->isn_type == ISN_NEWLIST || isn->isn_type == ISN_NEWTUPLE)
	{
	    did_check = TRUE;
	    ਜੇ (cac->cac_semicolon ?
			isn->isn_arg.ਸੰਖਿਆ < needed_list_len
			: isn->isn_arg.ਸੰਖਿਆ != needed_list_len)
	    {
		semsg(_(e_expected_nr_ਚੀਜ਼ਾਂ_but_got_nr),
			needed_list_len, (ਪੂਰਨ)isn->isn_arg.ਸੰਖਿਆ);
		ਵਾਪਸ FAIL;
	    }
	}
    }

    ਜੇ (!did_check)
	generate_CHECKLEN(cctx, needed_list_len, cac->cac_semicolon);

    ਵਾਪਸ OK;
}

/*
 * Evaluate the expression for "[var, var] = expr" assignment.
 * A line break may follow the assignment operator "=".
 */
    ਸਥਿਰ char_u *
compile_ਸੌਂਪੋ_list_ਵਿਅੰਜਕ(cctx_T *cctx, cac_T *cac)
{
    char_u *whitep;

    whitep = cac->cac_op + cac->cac_oplen;

    ਜੇ (may_get_ਅਗਲਾ_ਸਤਰ_error(whitep, &cac->cac_nextc, cctx) == FAIL)
	ਵਾਪਸ ਨਿਰਾ;

    // compile RHS expression
    ਜੇ (compile_expr0(&cac->cac_nextc, cctx) == FAIL)
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (cctx->ctx_skip == SKIP_YES)
	// no need to parse more when skipping
	ਵਾਪਸ cac->cac_nextc;

    ਜੇ (compile_ਸੌਂਪੋ_list_check_rhs_type(cctx, cac) == FAIL)
	ਵਾਪਸ ਨਿਰਾ;

    // If a constant list was used we can check the length right here.
    ਜੇ (compile_ਸੌਂਪੋ_list_check_ਲੰਮਾਈ(cctx, cac) == FAIL)
	ਵਾਪਸ FAIL;

    ਵਾਪਸ cac->cac_nextc;
}

/*
 * Find and return the end of a heredoc or a list of variables assignment
 * statement.  For a single variable assignment statement, returns the current
 * end.
 * Returns NULL on failure.
 */
    ਸਥਿਰ char_u *
compile_ਸੌਂਪੋ_compute_ਸਿਰਾ(
    exarg_T	*eap,
    cctx_T	*cctx,
    cac_T	*cac,
    ਪੂਰਨ		heredoc)
{
    ਜੇ (heredoc)
    {
	cac->cac_nextc = parse_heredoc_assignment(eap, cctx, cac);
	ਵਾਪਸ cac->cac_nextc;
    }

    ਜੇ (cac->cac_var_ਗਿਣਤੀ > 0)
    {
	// for "[var, var] = expr" evaluate the expression. The list of
	// variables are processed later.
	// A line break may follow the "=".
	cac->cac_nextc = compile_ਸੌਂਪੋ_list_ਵਿਅੰਜਕ(cctx, cac);
	ਵਾਪਸ cac->cac_nextc;
    }

    ਵਾਪਸ cac->cac_var_ਸਿਰਾ;
}

/*
 * For "var = expr" evaluate the expression.
 */
    ਸਥਿਰ ਪੂਰਨ
compile_ਸੌਂਪੋ_single_eval_ਵਿਅੰਜਕ(cctx_T *cctx, cac_T *cac)
{
    ਪੂਰਨ		ret = OK;
    char_u	*whitep;
    lhs_T	*lhs = &cac->cac_lhs;
    lvar_T	*lvp;
    lvar_T	save_lhs_lvar;

    // Compile the expression.
    ਜੇ (cac->cac_incdec)
	ਵਾਪਸ generate_PUSHNR(cctx, 1);

    // Temporarily hide the new local variable here, it is
    // not available to this expression.
    ਜੇ (lhs->lhs_new_local)
    {
	--cctx->ctx_locals.ga_len;

	// Save the local variable value (compiling the RHS expression may
	// create new local variables).
	lvp = ((lvar_T *)cctx->ctx_locals.ga_ਅੰਕੜੇ) + cctx->ctx_locals.ga_len;
	save_lhs_lvar = *lvp;
    }
    whitep = cac->cac_op + cac->cac_oplen;

    ਜੇ (may_get_ਅਗਲਾ_ਸਤਰ_error(whitep, &cac->cac_nextc, cctx) == FAIL)
    {
	ਜੇ (lhs->lhs_new_local)
	    ++cctx->ctx_locals.ga_len;
	ਵਾਪਸ FAIL;
    }

    ret = compile_expr0_ext(&cac->cac_nextc, cctx, &cac->cac_is_const);
    ਜੇ (lhs->lhs_new_local)
    {
	// Restore the local variable value.  Update lhs_lvar as the index of
	// the local variable might have changed.
	lvp = ((lvar_T *)cctx->ctx_locals.ga_ਅੰਕੜੇ) + cctx->ctx_locals.ga_len;
	*lvp = save_lhs_lvar;
	lhs->lhs_lvar = lvp;

	++cctx->ctx_locals.ga_len;
    }

    ਵਾਪਸ ret;
}

/*
 * When compiling an assignment, set the LHS type to the RHS type.
 */
    ਸਥਿਰ ਪੂਰਨ
compile_ਸੌਂਪੋ_ਸੈੱਟ_lhs_type_from_rhs(
    cctx_T	*cctx,
    cac_T	*cac,
    lhs_T	*lhs,
    type_T	*rhs_type)
{
    ਜੇ (rhs_type->tt_type == VAR_VOID)
    {
	emsg(_(e_cannot_use_void_ਮੁੱਲ));
	ਵਾਪਸ FAIL;
    }

    type_T *type;

    // An empty list or dict has a &t_unknown member, for a variable that
    // implies &t_any.
    ਜੇ (rhs_type == &t_list_ਖਾਲੀ_ਥਾਵਾਂ)
	type = &t_list_any;
    ਨਹੀਂਤਾਂ ਜੇ (rhs_type == &t_dict_ਖਾਲੀ_ਥਾਵਾਂ)
	type = &t_dict_any;
    ਨਹੀਂਤਾਂ ਜੇ (rhs_type == &t_unknown)
	type = &t_any;
    ਨਹੀਂਤਾਂ
    {
	type = rhs_type;
	cac->cac_inferred_type = rhs_type;
    }

    ਸੈੱਟ_var_type(lhs->lhs_lvar, type, cctx);

    ਵਾਪਸ OK;
}

/*
 * Returns TRUE if the "rhs_type" can be assigned to the "lhs" variable.
 * Used when compiling an assignment statement.
 */
    ਸਥਿਰ ਪੂਰਨ
compile_ਸੌਂਪੋ_valid_rhs_type(
    cctx_T	*cctx,
    cac_T	*cac,
    lhs_T	*lhs,
    type_T	*rhs_type)
{
    type_T	*use_type = lhs->lhs_lvar->lv_type;
    where_T	where = WHERE_INIT;

    // Without operator check type here, otherwise below.
    // Use the line number of the assignment.
    SOURCING_LNUM = cac->cac_ਸ਼ੁਰੂ_lnum;
    ਜੇ (cac->cac_var_ਗਿਣਤੀ > 0)
    {
	where.wt_ਸੂਚਕਾਂਕ = cac->cac_var_idx + 1;
	where.wt_ਕਿਸਮ = WT_VARIABLE;
    }

    // If assigning to a list or dict member, use the member type.
    // Not for "list[:] =".
    ਜੇ (lhs->lhs_has_ਸੂਚਕਾਂਕ &&
	    !has_list_ਸੂਚਕਾਂਕ(cac->cac_var_ਸ਼ੁਰੂ + lhs->lhs_varlen, cctx))
	use_type = lhs->lhs_member_type;

    ਜੇ (need_type_where(rhs_type, use_type, 0, -1, where, cctx, FALSE,
						cac->cac_is_const) == FAIL)
	ਵਾਪਸ FALSE;

    ਵਾਪਸ TRUE;
}

/*
 * Compare the LHS type with the RHS type in an assignment.
 */
    ਸਥਿਰ ਪੂਰਨ
compile_ਸੌਂਪੋ_check_type(cctx_T *cctx, cac_T *cac)
{
    lhs_T	*lhs = &cac->cac_lhs;
    type_T	*rhs_type;

    rhs_type = cctx->ctx_type_ਢੇਰ.ga_len == 0
					? &t_void
					: get_type_on_ਢੇਰ(cctx, 0);
    cac->cac_rhs_type = rhs_type;

    ਜੇ (check_type_is_ਮੁੱਲ(rhs_type) == FAIL)
	ਵਾਪਸ FAIL;

    ਜੇ (lhs->lhs_lvar != ਨਿਰਾ && (cac->cac_is_decl || !lhs->lhs_has_type))
    {
	ਜੇ (rhs_type->tt_type == VAR_FUNC
					|| rhs_type->tt_type == VAR_PARTIAL)
	{
	    // Make sure the variable name can be used as a funcref
	    ਜੇ (!lhs->lhs_has_ਸੂਚਕਾਂਕ
				&& var_wrong_func_name(lhs->lhs_name, TRUE))
		ਵਾਪਸ FAIL;
	}

	ਜੇ (lhs->lhs_new_local && !lhs->lhs_has_type)
	{
	    // The LHS variable doesn't have a type.  Set it to the RHS type.
	    ਜੇ (compile_ਸੌਂਪੋ_ਸੈੱਟ_lhs_type_from_rhs(cctx, cac, lhs, rhs_type)
								== FAIL)
		ਵਾਪਸ FAIL;
	}
	ਨਹੀਂਤਾਂ ਜੇ (*cac->cac_op == '=')
	{
	    ਜੇ (!compile_ਸੌਂਪੋ_valid_rhs_type(cctx, cac, lhs, rhs_type))
		ਵਾਪਸ FAIL;
	}
    }
    ਨਹੀਂਤਾਂ
    {
	// Assigning to a register using @r = "abc"

	type_T *lhs_type = lhs->lhs_member_type;

	// Special case: assigning to @# can use a number or a string.
	// Also: can assign a number to a float.
	ਜੇ ((lhs_type == &t_ਸੰਖਿਆ_or_string || lhs_type == &t_float)
					&& rhs_type->tt_type == VAR_ਸੰਖਿਆ)
	    lhs_type = &t_ਸੰਖਿਆ;

	ਜੇ (*cac->cac_nextc != '=')
	{
	    ਜੇ (need_type(rhs_type, lhs_type, 0, -1, 0, cctx, FALSE,
							FALSE) == FAIL)
		ਵਾਪਸ FAIL;
	}
    }

    ਵਾਪਸ OK;
}

/*
 * Compile the RHS expression in an assignment statement and generate the
 * instructions.
 */
    ਸਥਿਰ ਪੂਰਨ
compile_ਸੌਂਪੋ_rhs_ਵਿਅੰਜਕ(cctx_T *cctx, cac_T *cac)
{
    cac->cac_is_const = FALSE;

    // for "+=", "*=", "..=" etc. first load the current value
    ਜੇ (*cac->cac_op != '='
	    && compile_ਲੱਦੋ_lhs_with_ਸੂਚਕਾਂਕ(&cac->cac_lhs, cac->cac_var_ਸ਼ੁਰੂ,
								cctx) == FAIL)
	ਵਾਪਸ FAIL;

    // For "var = expr" evaluate the expression.
    ਜੇ (cac->cac_var_ਗਿਣਤੀ == 0)
    {
	ਪੂਰਨ	ret;

	// Compile the expression.
	cac->cac_instr_ਗਿਣਤੀ = cac->cac_instr->ga_len;
	ret = compile_ਸੌਂਪੋ_single_eval_ਵਿਅੰਜਕ(cctx, cac);
	ਜੇ (ret == FAIL)
	    ਵਾਪਸ FAIL;
    }
    ਨਹੀਂਤਾਂ ਜੇ (cac->cac_semicolon && cac->cac_var_idx == cac->cac_var_ਗਿਣਤੀ - 1)
    {
	// For "[var; var] = expr" get the rest of the list
	cac->cac_did_generate_ਟੁਕੜਾ = TRUE;
	ਜੇ (generate_ਟੁਕੜਾ(cctx, cac->cac_var_ਗਿਣਤੀ - 1) == FAIL)
	    ਵਾਪਸ FAIL;
    }
    ਨਹੀਂਤਾਂ
    {
	// For "[var, var] = expr" get the "var_idx" item from the
	// list.
	ਪੂਰਨ with_op = *cac->cac_op != '=';
	ਜੇ (generate_GETITEM(cctx, cac->cac_var_idx, with_op) == FAIL)
	    ਵਾਪਸ FAIL;
    }

    ਜੇ (compile_ਸੌਂਪੋ_check_type(cctx, cac) == FAIL)
	ਵਾਪਸ FAIL;

    ਵਾਪਸ OK;
}

/*
 * Compile the RHS expression in an assignment
 */
    ਸਥਿਰ ਪੂਰਨ
compile_ਸੌਂਪੋ_rhs(cctx_T *cctx, cac_T *cac)
{
    lhs_T	*lhs = &cac->cac_lhs;

    ਜੇ (cctx->ctx_skip == SKIP_YES)
    {
	ਜੇ (cac->cac_oplen > 0 && cac->cac_var_ਗਿਣਤੀ == 0)
	{
	    // skip over the "=" and the expression
	    cac->cac_nextc = skipwhite(cac->cac_op + cac->cac_oplen);
	    (ਖਾਲੀ)compile_expr0(&cac->cac_nextc, cctx);
	}
	ਵਾਪਸ OK;
    }

    // If RHS is specified, then generate instructions for RHS expression
    ਜੇ (cac->cac_oplen > 0)
	ਵਾਪਸ compile_ਸੌਂਪੋ_rhs_ਵਿਅੰਜਕ(cctx, cac);

    ਜੇ (cac->cac_cmdidx == CMD_final)
    {
	emsg(_(e_final_requires_a_ਮੁੱਲ));
	ਵਾਪਸ FAIL;
    }

    ਜੇ (cac->cac_cmdidx == CMD_const)
    {
	emsg(_(e_const_requires_a_ਮੁੱਲ));
	ਵਾਪਸ FAIL;
    }

    ਜੇ (!lhs->lhs_has_type || lhs->lhs_dest == dest_option
					|| lhs->lhs_dest == dest_func_option)
    {
	emsg(_(e_type_or_initialization_required));
	ਵਾਪਸ FAIL;
    }

    // variables are always initialized
    ਜੇ (GA_GROW_FAILS(cac->cac_instr, 1))
	ਵਾਪਸ FAIL;

    cac->cac_instr_ਗਿਣਤੀ = cac->cac_instr->ga_len;

    ਵਾਪਸ ਧੱਕੋ_default_ਮੁੱਲ(cctx, lhs->lhs_member_type->tt_type,
			      lhs->lhs_dest == dest_local,
			      &cac->cac_skip_store);
}

/*
 * Returns OK if "type" supports compound operator "op_arg" (e.g. +=, -=, %=,
 * etc.).  Compound operators are not supported with a tuple and a dict.
 * Returns FAIL if compound operator is not supported.
 */
    ਸਥਿਰ ਪੂਰਨ
check_type_supports_compound_op(type_T *type, char_u op_arg)
{
    ਜੇ (type->tt_type == VAR_TUPLE || type->tt_type == VAR_DICT)
    {
	char_u	op[2];

	op[0] = op_arg;
	op[1] = NUL;
	semsg(_(e_wrong_variable_type_for_str_equal), op);
	ਵਾਪਸ FAIL;
    }

    ਵਾਪਸ OK;
}

/*
 * Compile a compound op assignment statement (+=, -=, *=, %=, etc.)
 */
    ਸਥਿਰ ਪੂਰਨ
compile_ਸੌਂਪੋ_compound_op(cctx_T *cctx, cac_T *cac)
{
    lhs_T	    *lhs = &cac->cac_lhs;
    type_T	    *expected;
    type_T	    *stacktype = ਨਿਰਾ;

    ਜੇ (cac->cac_lhs.lhs_type->tt_type == VAR_TUPLE
	    && check_type_supports_compound_op(cac->cac_lhs.lhs_type,
							*cac->cac_op) == FAIL)
	ਵਾਪਸ FAIL;

    ਜੇ (*cac->cac_op == '.')
    {
	expected = lhs->lhs_member_type;
	stacktype = get_type_on_ਢੇਰ(cctx, 0);
	ਜੇ (expected != &t_string
		&& need_type(stacktype, expected, 0, -1, 0, cctx,
					FALSE, FALSE) == FAIL)
	    ਵਾਪਸ FAIL;
	ਨਹੀਂਤਾਂ ਜੇ (may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
	    ਵਾਪਸ FAIL;
    }
    ਨਹੀਂਤਾਂ
    {
	expected = lhs->lhs_member_type;
	stacktype = get_type_on_ਢੇਰ(cctx, 0);

	ਜੇ (check_type_supports_compound_op(expected, *cac->cac_op) == FAIL)
	    ਵਾਪਸ FAIL;

	ਜੇ (
		// If variable is float operation with number is OK.
		!(expected == &t_float && (stacktype == &t_ਸੰਖਿਆ
					|| stacktype == &t_ਸੰਖਿਆ_bool))
		&& need_type(stacktype, expected, TYPECHK_ਸੰਖਿਆ_OK, -1, 0,
						cctx, FALSE, FALSE) == FAIL)
	    ਵਾਪਸ FAIL;
    }

    ਜੇ (*cac->cac_op == '.')
    {
	ਜੇ (generate_CONCAT(cctx, 2) == FAIL)
	    ਵਾਪਸ FAIL;
    }
    ਨਹੀਂਤਾਂ ਜੇ (*cac->cac_op == '+')
    {
	ਜੇ (generate_ਜੋੜੋ_instr(cctx,
		    operator_type(lhs->lhs_member_type, stacktype),
		    lhs->lhs_member_type, stacktype,
		    ਵਿਅੰਜਕ_APPEND) == FAIL)
	    ਵਾਪਸ FAIL;
    }
    ਨਹੀਂਤਾਂ ਜੇ (generate_two_op(cctx, cac->cac_op) == FAIL)
	ਵਾਪਸ FAIL;

    ਵਾਪਸ OK;
}

/*
 * Generate the STORE and SETTYPE instructions for an assignment statement.
 */
    ਸਥਿਰ ਪੂਰਨ
compile_ਸੌਂਪੋ_generate_store(cctx_T *cctx, cac_T *cac)
{
    lhs_T	*lhs = &cac->cac_lhs;
    ਪੂਰਨ		save_lnum;

    // Use the line number of the assignment for store instruction.
    save_lnum = cctx->ctx_lnum;
    cctx->ctx_lnum = cac->cac_ਸ਼ੁਰੂ_lnum - 1;

    ਜੇ (lhs->lhs_has_ਸੂਚਕਾਂਕ)
    {
	// Use the info in "lhs" to store the value at the index in the
	// list, dict or object.
	ਜੇ (compile_ਸੌਂਪੋ_unlet(cac->cac_var_ਸ਼ੁਰੂ, &cac->cac_lhs,
				 TRUE, cac->cac_rhs_type, cctx) == FAIL)
	{
	    cctx->ctx_lnum = save_lnum;
	    ਵਾਪਸ FAIL;
	}
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (cac->cac_is_decl && cac->cac_cmdidx == CMD_const &&
			(lhs->lhs_dest == dest_script
			 || lhs->lhs_dest == dest_script_v9
			 || lhs->lhs_dest == dest_global
			 || lhs->lhs_dest == dest_local))
	    // ":const var": lock the value, but not referenced variables
	    generate_LOCKCONST(cctx);

	type_T	*inferred_type = cac->cac_inferred_type;

	ਜੇ ((lhs->lhs_type->tt_type == VAR_DICT
		    || lhs->lhs_type->tt_type == VAR_LIST)
		&& lhs->lhs_type->tt_member != ਨਿਰਾ
		&& lhs->lhs_type->tt_member != &t_any
		&& lhs->lhs_type->tt_member != &t_unknown)
	    // Set the type in the list or dict, so that it can be
	    // checked, also in legacy script.
	    generate_SETTYPE(cctx, lhs->lhs_type);
	ਨਹੀਂਤਾਂ ਜੇ (lhs->lhs_type->tt_type == VAR_TUPLE
					&& lhs->lhs_type->tt_argcount != 0)
	    generate_SETTYPE(cctx, lhs->lhs_type);
	ਨਹੀਂਤਾਂ ਜੇ (inferred_type != ਨਿਰਾ
		&& (inferred_type->tt_type == VAR_DICT
		    || inferred_type->tt_type == VAR_LIST)
		&& inferred_type->tt_member != ਨਿਰਾ
		&& inferred_type->tt_member != &t_unknown
		&& inferred_type->tt_member != &t_any)
	    // Set the type in the list or dict, so that it can be
	    // checked, also in legacy script.
	    generate_SETTYPE(cctx, inferred_type);
	ਨਹੀਂਤਾਂ ਜੇ (inferred_type != ਨਿਰਾ
				&& inferred_type->tt_type == VAR_TUPLE
				&& inferred_type->tt_argcount > 0)
	    generate_SETTYPE(cctx, inferred_type);

	ਜੇ (!cac->cac_skip_store &&
		generate_store_lhs(cctx, &cac->cac_lhs,
				   cac->cac_instr_ਗਿਣਤੀ,
				   cac->cac_is_decl) == FAIL)
	{
	    cctx->ctx_lnum = save_lnum;
	    ਵਾਪਸ FAIL;
	}
    }

    cctx->ctx_lnum = save_lnum;

    ਵਾਪਸ OK;
}

/*
 * Process the variable(s) in an assignment statement
 */
    ਸਥਿਰ ਪੂਰਨ
compile_ਸੌਂਪੋ_process_variables(
    cctx_T	*cctx,
    cac_T	*cac,
    ਪੂਰਨ		cmdidx,
    ਪੂਰਨ		heredoc,
    ਪੂਰਨ		has_cmd,
    ਪੂਰਨ		has_argisset_ਅਗੇਤਰ,
    ਪੂਰਨ		jump_instr_idx)
{
    /*
     * Loop over variables in "[var, var] = expr".
     * For "name = expr" and "var name: type" this is done only once.
     */
    ਲਈ (cac->cac_var_idx = 0; cac->cac_var_idx == 0 ||
	    cac->cac_var_idx < cac->cac_var_ਗਿਣਤੀ; cac->cac_var_idx++)
    {
	ਜੇ (cac->cac_var_ਸ਼ੁਰੂ[0] == '_'
				&& !eval_isnamec(cac->cac_var_ਸ਼ੁਰੂ[1]))
	{
	    // Ignore underscore in "[a, _, b] = list".
	    ਜੇ (cac->cac_var_ਗਿਣਤੀ > 0)
	    {
		cac->cac_var_ਸ਼ੁਰੂ = skipwhite(cac->cac_var_ਸ਼ੁਰੂ + 2);
		ਜਾਰੀ;
	    }
	    emsg(_(e_cannot_use_underscore_ਇੱਥੇ));
	    ਵਾਪਸ FAIL;
	}
	vim_free(cac->cac_lhs.lhs_name);

	/*
	 * Figure out the LHS type and other properties.
	 */
	ਜੇ (compile_ਸੌਂਪੋ_lhs(cac->cac_var_ਸ਼ੁਰੂ, &cac->cac_lhs, cmdidx,
		    cac->cac_is_decl, heredoc, has_cmd,
		    cac->cac_oplen, cctx) == FAIL)
	    ਵਾਪਸ FAIL;

	// Compile the RHS expression
	ਜੇ (heredoc)
	{
	    SOURCING_LNUM = cac->cac_ਸ਼ੁਰੂ_lnum;
	    ਜੇ (cac->cac_lhs.lhs_has_type
		    && need_type(&t_list_string, cac->cac_lhs.lhs_type,
				0, -1, 0, cctx, FALSE, FALSE) == FAIL)
		ਵਾਪਸ FAIL;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (compile_ਸੌਂਪੋ_rhs(cctx, cac) == FAIL)
		ਵਾਪਸ FAIL;
	    ਜੇ (cac->cac_var_ਗਿਣਤੀ == 0)
		cac->cac_var_ਸਿਰਾ = cac->cac_nextc;
	}

	// no need to parse more when skipping
	ਜੇ (cctx->ctx_skip == SKIP_YES)
	    ਰੋਕੋ;

	ਜੇ (cac->cac_oplen > 0 && *cac->cac_op != '=')
	{
	    ਜੇ (compile_ਸੌਂਪੋ_compound_op(cctx, cac) == FAIL)
		ਵਾਪਸ FAIL;
	}

	// generate the store instructions
	ਜੇ (compile_ਸੌਂਪੋ_generate_store(cctx, cac) == FAIL)
	    ਵਾਪਸ FAIL;

	ਜੇ (cac->cac_var_idx + 1 < cac->cac_var_ਗਿਣਤੀ)
	    cac->cac_var_ਸ਼ੁਰੂ = skipwhite(cac->cac_lhs.lhs_ਸਿਰਾ + 1);

	ਜੇ (has_argisset_ਅਗੇਤਰ)
	{
	    // set instruction index in JUMP_IF_ARG_SET to here
	    isn_T *isn = ((isn_T *)cac->cac_instr->ga_ਅੰਕੜੇ) + jump_instr_idx;
	    isn->isn_arg.jumparg.jump_where = cac->cac_instr->ga_len;
	}
    }

    ਵਾਪਸ OK;
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
    ਸਥਿਰ char_u *
compile_assignment(
    char_u	*arg_ਸ਼ੁਰੂ,
    exarg_T	*eap,
    cmdidx_T	cmdidx,
    cctx_T	*cctx)
{
    cac_T	cac;
    char_u	*arg = arg_ਸ਼ੁਰੂ;
    char_u	*retstr = ਨਿਰਾ;
    ਪੂਰਨ		heredoc = FALSE;
    ਪੂਰਨ		jump_instr_idx;

    compile_ਸੌਂਪੋ_context_init(&cac, cctx, cmdidx, arg);

    jump_instr_idx = cac.cac_instr->ga_len;

    // process object variable initialization in a new() constructor method
    ਪੂਰਨ	has_argisset_ਅਗੇਤਰ = STRNCMP(arg, "ifargisset ", 11) == 0;
    ਜੇ (has_argisset_ਅਗੇਤਰ &&
			compile_ਸੌਂਪੋ_obj_new_arg(&arg, cctx) == FAIL)
	ਜਾਓ theend;

    // Skip over the "varname" or "[varname, varname]" to get to any "=".
    cac.cac_nextc = skip_var_list(arg, TRUE, &cac.cac_var_ਗਿਣਤੀ,
						&cac.cac_semicolon, TRUE);
    ਜੇ (cac.cac_nextc == ਨਿਰਾ)
	ਵਾਪਸ *arg == '[' ? arg : ਨਿਰਾ;

    ਜੇ (compile_ਸੌਂਪੋ_process_operator(eap, arg, &cac, &heredoc,
							&retstr) == FAIL)
	ਵਾਪਸ retstr;

    // Compute the start of the assignment
    cac.cac_var_ਸ਼ੁਰੂ = compile_ਸੌਂਪੋ_compute_ਸ਼ੁਰੂ(arg, cac.cac_var_ਗਿਣਤੀ);

    // Compute the end of the assignment
    cac.cac_var_ਸਿਰਾ = compile_ਸੌਂਪੋ_compute_ਸਿਰਾ(eap, cctx, &cac, heredoc);
    ਜੇ (cac.cac_var_ਸਿਰਾ == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    ਪੂਰਨ has_cmd = cac.cac_var_ਸ਼ੁਰੂ > eap->cmd;

    /* process the variable(s) */
    ਜੇ (compile_ਸੌਂਪੋ_process_variables(cctx, &cac, cmdidx, heredoc,
					 has_cmd, has_argisset_ਅਗੇਤਰ,
					 jump_instr_idx) == FAIL)
	ਜਾਓ theend;

    // For "[var, var] = expr" drop the "expr" value.
    // Also for "[var, var; _] = expr".
    ਜੇ (cctx->ctx_skip != SKIP_YES && cac.cac_var_ਗਿਣਤੀ > 0 &&
	    (!cac.cac_semicolon || !cac.cac_did_generate_ਟੁਕੜਾ))
    {
	ਜੇ (generate_instr_drop(cctx, ISN_DROP, 1) == ਨਿਰਾ)
	    ਜਾਓ theend;
    }

    retstr = skipwhite(cac.cac_var_ਸਿਰਾ);

theend:
    vim_free(cac.cac_lhs.lhs_name);
    ਵਾਪਸ retstr;
}

/*
 * Check for an assignment at "eap->cmd", compile it if found.
 * Return NOTDONE if there is none, FAIL for failure, OK if done.
 */
    ਸਥਿਰ ਪੂਰਨ
may_compile_assignment(exarg_T *eap, char_u **ਸਤਰ, cctx_T *cctx)
{
    char_u  *pskip;
    char_u  *p;

    // Assuming the command starts with a variable or function name,
    // find what follows.
    // Skip over "var.member", "var[idx]" and the like.
    // Also "&opt = val", "$ENV = val" and "@r = val".
    pskip = (*eap->cmd == '&' || *eap->cmd == '$' || *eap->cmd == '@')
						 ? eap->cmd + 1 : eap->cmd;
    p = ਮੰਜ਼ਿਲ_name_ਸਿਰਾ(pskip, TRUE);
    ਜੇ (p > eap->cmd && *p != NUL)
    {
	char_u *var_ਸਿਰਾ;
	ਪੂਰਨ	oplen;
	ਪੂਰਨ	heredoc;

	ਜੇ (eap->cmd[0] == '@')
	    var_ਸਿਰਾ = eap->cmd + 2;
	ਨਹੀਂਤਾਂ
	    var_ਸਿਰਾ = ਲੱਭੋ_name_ਸਿਰਾ(pskip, ਨਿਰਾ, ਨਿਰਾ,
					FNE_CHECK_ਸ਼ੁਰੂ | FNE_INCL_BR);
	oplen = assignment_len(skipwhite(var_ਸਿਰਾ), &heredoc);
	ਜੇ (oplen > 0)
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
	    ਜੇ (*eap->cmd == '&'
		    || *eap->cmd == '$'
		    || *eap->cmd == '@'
		    || ((len) > 2 && eap->cmd[1] == ':')
		    || STRNCMP(eap->cmd, "g:[", 3) == 0
		    || variable_exists(eap->cmd, len, cctx))
	    {
		*ਸਤਰ = compile_assignment(eap->cmd, eap, CMD_ਮਾਪ, cctx);
		ਜੇ (*ਸਤਰ == ਨਿਰਾ || *ਸਤਰ == eap->cmd)
		    ਵਾਪਸ FAIL;
		ਵਾਪਸ OK;
	    }
	}
    }

    // might be "[var, var] = expr" or "ifargisset this.member = expr"
    ਜੇ (*eap->cmd == '[' || STRNCMP(eap->cmd, "ifargisset ", 11) == 0)
    {
	*ਸਤਰ = compile_assignment(eap->cmd, eap, CMD_ਮਾਪ, cctx);
	ਜੇ (*ਸਤਰ == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;
	ਜੇ (*ਸਤਰ != eap->cmd)
	    ਵਾਪਸ OK;
    }
    ਵਾਪਸ NOTDONE;
}

/*
 * Check if arguments of "ufunc" shadow variables in "cctx".
 * Return OK or FAIL.
 */
    ਸਥਿਰ ਪੂਰਨ
check_ਦਲੀਲਾਂ_shadowing(ufunc_T *ufunc, cctx_T *cctx)
{
    ਪੂਰਨ	    i;
    char_u  *arg;
    ਪੂਰਨ	    r = OK;

    // Make sure arguments are not found when compiling a second time.
    ufunc->uf_ਦਲੀਲਾਂ_visible = 0;

    // Check for arguments shadowing variables from the context.
    ਲਈ (i = 0; i < ufunc->uf_ਦਲੀਲਾਂ.ga_len; ++i)
    {
	arg = ((char_u **)(ufunc->uf_ਦਲੀਲਾਂ.ga_ਅੰਕੜੇ))[i];
	ਜੇ (check_defined(arg, STRLEN(arg), cctx, ਨਿਰਾ, TRUE) == FAIL)
	{
	    r = FAIL;
	    ਰੋਕੋ;
	}
    }
    ufunc->uf_ਦਲੀਲਾਂ_visible = ufunc->uf_ਦਲੀਲਾਂ.ga_len;
    ਵਾਪਸ r;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAS_ਸੁਨੇਹਾ_ਖਿੜਕੀ
/*
 * Get a count before a command.  Can only be a number.
 * Returns zero if there is no count.
 * Returns -1 if there is something wrong.
 */
    ਸਥਿਰ ਲੰਮਾ
get_cmd_ਗਿਣਤੀ(char_u *ਸਤਰ, exarg_T *eap)
{
    char_u *p;

    // skip over colons and white space
    ਲਈ (p = ਸਤਰ; *p == ':' || VIM_ISWHITE(*p); ++p)
	;
    ਜੇ (!ਸੁਰੱਖਿਅਤ_isdigit(*p))
    {
	// The command or modifiers must be following.  Assume a lower case
	// character means there is a modifier.
	ਜੇ (p < eap->cmd && !vim_islower(*p))
	{
	    emsg(_(e_invalid_range));
	    ਵਾਪਸ -1;
	}
	ਵਾਪਸ 0;
    }
    ਵਾਪਸ atol((ਅੱਖਰ *)p);
}
#ਜੇ_ਅੰਤ

/*
 * Get the compilation type that should be used for "ufunc".
 * Keep in sync with INSTRUCTIONS().
 */
    compiletype_T
get_compile_type(ufunc_T *ufunc)
{
    // Update uf_has_breakpoint if needed.
    update_has_breakpoint(ufunc);

    ਜੇ (debug_break_level > 0 || may_break_ਵਿੱਚ_function(ufunc))
	ਵਾਪਸ CT_DEBUG;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROFILE
    ਜੇ (do_profiling == PROF_YES)
    {
	ਜੇ (!ufunc->uf_profiling && has_profiling(FALSE, ufunc->uf_name, ਨਿਰਾ,
							    &ufunc->uf_hash))
	    func_do_profile(ufunc);
	ਜੇ (ufunc->uf_profiling)
	    ਵਾਪਸ CT_PROFILE;
    }
#ਜੇ_ਅੰਤ
    ਵਾਪਸ CT_NONE;
}

/*
 * Free the compiled instructions saved for a def function.  This is used when
 * compiling a def function and the function was compiled before.
 * The index is reused.
 */
    ਸਥਿਰ ਖਾਲੀ
clear_def_function(ufunc_T *ufunc, compiletype_T compile_type)
{
    isn_T	*instr_dest = ਨਿਰਾ;
    dfunc_T	*dfunc;

    dfunc = ((dfunc_T *)def_functions.ga_ਅੰਕੜੇ) + ufunc->uf_dfunc_idx;

    ਚੋਣ (compile_type)
    {
	ਹਾਲਤ CT_PROFILE:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROFILE
	    instr_dest = dfunc->df_instr_prof; ਰੋਕੋ;
#ਜੇ_ਅੰਤ
	ਹਾਲਤ CT_NONE:   instr_dest = dfunc->df_instr; ਰੋਕੋ;
	ਹਾਲਤ CT_DEBUG:  instr_dest = dfunc->df_instr_debug; ਰੋਕੋ;
    }

    ਜੇ (instr_dest != ਨਿਰਾ)
	// Was compiled in this mode before: Free old instructions.
	delete_def_function_contents(dfunc, FALSE);

    ga_clear_ਲੜੀਆਂ(&dfunc->df_var_ਨਾਂ);
    dfunc->df_defer_var_idx = 0;
}

/*
 * Add a function to the list of :def functions.
 * This sets "ufunc->uf_dfunc_idx" but the function isn't compiled yet.
 */
    ਸਥਿਰ ਪੂਰਨ
ਜੋੜੋ_def_function(ufunc_T *ufunc)
{
    dfunc_T *dfunc;

    ਜੇ (def_functions.ga_len == 0)
    {
	// The first position is not used, so that a zero uf_dfunc_idx means it
	// wasn't set.
	ਜੇ (GA_GROW_FAILS(&def_functions, 1))
	    ਵਾਪਸ FAIL;
	++def_functions.ga_len;
    }

    // Add the function to "def_functions".
    ਜੇ (GA_GROW_FAILS(&def_functions, 1))
	ਵਾਪਸ FAIL;
    dfunc = ((dfunc_T *)def_functions.ga_ਅੰਕੜੇ) + def_functions.ga_len;
    CLEAR_ਸੂਚਕ(dfunc);
    dfunc->df_idx = def_functions.ga_len;
    ufunc->uf_dfunc_idx = dfunc->df_idx;
    dfunc->df_ufunc = ufunc;
    dfunc->df_name = vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
    ga_init2(&dfunc->df_var_ਨਾਂ, ਆਕਾਰ(char_u *), 10);
    ++dfunc->df_refcount;
    ++def_functions.ga_len;
    ਵਾਪਸ OK;
}

    ਸਥਿਰ ਪੂਰਨ
compile_dfunc_ufunc_init(
    ufunc_T		*ufunc,
    cctx_T		*outer_cctx,
    compiletype_T	compile_type,
    ਪੂਰਨ			*new_def_function)
{
    // When using a function that was compiled before: Free old instructions.
    // The index is reused.  Otherwise add a new entry in "def_functions".
    ਜੇ (ufunc->uf_dfunc_idx > 0)
	clear_def_function(ufunc, compile_type);
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (ਜੋੜੋ_def_function(ufunc) == FAIL)
	    ਵਾਪਸ FAIL;

	*new_def_function = TRUE;
    }

    ਜੇ ((ufunc->uf_flags & FC_ਘੇਰਾ) && outer_cctx == ਨਿਰਾ)
    {
	semsg(_(e_compiling_ਘੇਰਾ_without_context_str),
						printable_func_name(ufunc));
	ਵਾਪਸ FAIL;
    }

    ufunc->uf_def_ਸਥਿਤੀ = UF_COMPILING;

    ਵਾਪਸ OK;
}

/*
 * Initialize the compilation context for compiling a def function.
 */
    ਸਥਿਰ ਖਾਲੀ
compile_dfunc_cctx_init(
    cctx_T		*cctx,
    cctx_T		*outer_cctx,
    ufunc_T		*ufunc,
    compiletype_T	compile_type)
{
    CLEAR_ਖੇਤਰ(*cctx);

    cctx->ctx_compile_type = compile_type;
    cctx->ctx_ufunc = ufunc;
    cctx->ctx_lnum = -1;
    cctx->ctx_outer = outer_cctx;
    ga_init2(&cctx->ctx_locals, ਆਕਾਰ(lvar_T), 10);
    // Each entry on the type stack consists of two type pointers.
    ga_init2(&cctx->ctx_type_ਢੇਰ, ਆਕਾਰ(type2_T), 50);
    cctx->ctx_type_list = &ufunc->uf_type_list;
    ga_init2(&cctx->ctx_instr, ਆਕਾਰ(isn_T), 50);
}

/*
 * For an object constructor, generate instruction to setup "this" (the first
 * local variable) and to initialize the object variables.
 */
    ਸਥਿਰ ਪੂਰਨ
obj_constructor_prologue(ufunc_T *ufunc, cctx_T *cctx)
{
    generate_CONSTRUCT(cctx, ufunc->uf_class);

    ਲਈ (ਪੂਰਨ i = 0; i < ufunc->uf_class->class_obj_member_ਗਿਣਤੀ; ++i)
    {
	ocmember_T *m = &ufunc->uf_class->class_obj_members[i];

	ਜੇ (i < 2 && IS_ENUM(ufunc->uf_class))
	    // The first two object variables in an enum are the name
	    // and the ordinal.  These are set by the ISN_CONSTRUCT
	    // instruction.  So don't generate instructions to set
	    // these variables.
	    ਜਾਰੀ;

	ਜੇ (m->ocm_init != ਨਿਰਾ)
	{
	    char_u	*ਵਿਅੰਜਕ = m->ocm_init;
	    sctx_T	save_current_sctx;
	    ਪੂਰਨ		change_sctx = FALSE;

	    // If the member variable initialization script context is
	    // different from the current script context, then change it.
	    ਜੇ (current_sctx.sc_sid != m->ocm_init_sctx.sc_sid)
		change_sctx = TRUE;

	    ਜੇ (change_sctx)
	    {
		// generate an instruction to change the script context to the
		// member variable initialization script context.
		save_current_sctx = current_sctx;
		current_sctx = m->ocm_init_sctx;
		generate_SCRIPTCTX_ਸੈੱਟ(cctx, current_sctx);
	    }

	    ਪੂਰਨ r = compile_expr0(&ਵਿਅੰਜਕ, cctx);

	    ਜੇ (change_sctx)
	    {
		// restore the previous script context
		current_sctx = save_current_sctx;
		generate_SCRIPTCTX_ਸੈੱਟ(cctx, current_sctx);
	    }

	    ਜੇ (r == FAIL)
		ਵਾਪਸ FAIL;

	    ਜੇ (!ਸਿਰੇ_excmd2(m->ocm_init, ਵਿਅੰਜਕ))
	    {
		semsg(_(e_trailing_characters_str), ਵਿਅੰਜਕ);
		ਵਾਪਸ FAIL;
	    }

	    type_T	*type = get_type_on_ਢੇਰ(cctx, 0);
	    ਜੇ (m->ocm_type->tt_type == VAR_ANY
		    && !(m->ocm_flags & OCMFLAG_HAS_TYPE)
		    && type->tt_type != VAR_SPECIAL)
	    {
		// If the member variable type is not yet set, then use
		// the initialization expression type.
		m->ocm_type = type;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		// The type of the member initialization expression is
		// determined at run time.  Add a runtime type check.
		where_T	where = WHERE_INIT;
		where.wt_ਕਿਸਮ = WT_MEMBER;
		where.wt_func_name = (ਅੱਖਰ *)m->ocm_name.string;
		ਜੇ (need_type_where(type, m->ocm_type, 0, -1,
					where, cctx, FALSE, FALSE) == FAIL)
		    ਵਾਪਸ FAIL;
	    }
	}
	ਨਹੀਂਤਾਂ
	    ਧੱਕੋ_default_ਮੁੱਲ(cctx, m->ocm_type->tt_type, FALSE, ਨਿਰਾ);

	ਜੇ (((m->ocm_type->tt_type == VAR_DICT
			|| m->ocm_type->tt_type == VAR_LIST)
		    && m->ocm_type->tt_member != ਨਿਰਾ
		    && m->ocm_type->tt_member != &t_any
		    && m->ocm_type->tt_member != &t_unknown)
		|| (m->ocm_type->tt_type == VAR_TUPLE
		    && m->ocm_type->tt_argcount > 0))
	    // Set the type in the list, tuple or dict, so that it can be
	    // checked, also in legacy script.
	    generate_SETTYPE(cctx, m->ocm_type);

	generate_STORE_THIS(cctx, i);
    }

    ਵਾਪਸ OK;
}

/*
 * For an object method and an constructor, generate instruction to setup
 * "this" (the first local variable).  For a constructor, generate instructions
 * to initialize the object variables.
 */
    ਸਥਿਰ ਪੂਰਨ
obj_ਵਿਧੀ_prologue(ufunc_T *ufunc, cctx_T *cctx)
{
    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_ਅੰਕੜੇ) + ufunc->uf_dfunc_idx;

    ਜੇ (GA_GROW_FAILS(&dfunc->df_var_ਨਾਂ, 1))
	ਵਾਪਸ FAIL;

    ((char_u **)dfunc->df_var_ਨਾਂ.ga_ਅੰਕੜੇ)[0] =
						vim_strsave((char_u *)"this");
    ++dfunc->df_var_ਨਾਂ.ga_len;

    // In the constructor allocate memory for the object and initialize the
    // object members.
    ਜੇ (IS_CONSTRUCTOR_ਵਿਧੀ(ufunc))
	ਵਾਪਸ obj_constructor_prologue(ufunc, cctx);

    ਵਾਪਸ OK;
}

/*
 * Produce instructions for the default values of optional arguments.
 */
    ਸਥਿਰ ਪੂਰਨ
compile_def_function_default_ਦਲੀਲਾਂ(
    ufunc_T	*ufunc,
    garray_T	*instr,
    cctx_T	*cctx)
{
    ਪੂਰਨ	ਗਿਣਤੀ_2 = ufunc->uf_def_ਦਲੀਲਾਂ.ga_len;
    ਪੂਰਨ	ਪਹਿਲਾ_def_arg = ufunc->uf_ਦਲੀਲਾਂ.ga_len - ਗਿਣਤੀ_2;
    ਪੂਰਨ	i;
    ਪੂਰਨ	off = ਢੇਰ_ਚੌਖਟਾ_ਮਾਪ + (ufunc->uf_va_name != ਨਿਰਾ ? 1 : 0);
    ਪੂਰਨ	did_ਸੈੱਟ_arg_type = FALSE;

    // Produce instructions for the default values of optional arguments.
    SOURCING_LNUM = 0;  // line number unknown
    ਲਈ (i = 0; i < ਗਿਣਤੀ_2; ++i)
    {
	char_u *arg = ((char_u **)(ufunc->uf_def_ਦਲੀਲਾਂ.ga_ਅੰਕੜੇ))[i];
	ਜੇ (STRCMP(arg, "v:none") == 0)
	    // "arg = v:none" means the argument is optional without
	    // setting a value when the argument is missing.
	    ਜਾਰੀ;

	type_T	*val_type;
	ਪੂਰਨ		arg_idx = ਪਹਿਲਾ_def_arg + i;
	where_T	where = WHERE_INIT;
	ਪੂਰਨ		jump_instr_idx = instr->ga_len;
	isn_T	*isn;

	// Use a JUMP_IF_ARG_SET instruction to skip if the value was given.
	ਜੇ (generate_JUMP_IF_ARG(cctx, ISN_JUMP_IF_ARG_ਸੈੱਟ,
						i - ਗਿਣਤੀ_2 - off) == FAIL)
	    ਵਾਪਸ FAIL;

	// Make sure later arguments are not found.
	ufunc->uf_ਦਲੀਲਾਂ_visible = arg_idx;

	ਪੂਰਨ r = compile_expr0(&arg, cctx);
	ਜੇ (r == FAIL)
	    ਵਾਪਸ FAIL;

	// If no type specified use the type of the default value.
	// Otherwise check that the default value type matches the
	// specified type.
	val_type = get_type_on_ਢੇਰ(cctx, 0);
	where.wt_ਸੂਚਕਾਂਕ = arg_idx + 1;
	where.wt_ਕਿਸਮ = WT_ਦਲੀਲ;
	ਜੇ (ufunc->uf_arg_types[arg_idx] == &t_unknown)
	{
	    did_ਸੈੱਟ_arg_type = TRUE;
	    ufunc->uf_arg_types[arg_idx] = val_type;
	}
	ਨਹੀਂਤਾਂ ਜੇ (need_type_where(val_type, ufunc->uf_arg_types[arg_idx],
		    0, -1, where, cctx, FALSE, FALSE) == FAIL)
	    ਵਾਪਸ FAIL;

	ਜੇ (generate_STORE(cctx, ISN_STORE, i - ਗਿਣਤੀ_2 - off, ਨਿਰਾ) == FAIL)
	    ਵਾਪਸ FAIL;

	// set instruction index in JUMP_IF_ARG_SET to here
	isn = ((isn_T *)instr->ga_ਅੰਕੜੇ) + jump_instr_idx;
	isn->isn_arg.jumparg.jump_where = instr->ga_len;
    }

    ਜੇ (did_ਸੈੱਟ_arg_type)
	ਸੈੱਟ_function_type(ufunc);

    ਵਾਪਸ OK;
}

/*
 * Compile def function body.  Loop over all the lines in the function and
 * generate instructions.
 */
    ਸਥਿਰ ਪੂਰਨ
compile_def_function_body(
    ਪੂਰਨ		ਆਖਰੀ_func_lnum,
    ਪੂਰਨ		check_return_type,
    garray_T	*ਸਤਰਾਂ_ਮੰਜ਼ਿਲ_free,
    ਅੱਖਰ	**errormsg,
    cctx_T	*cctx)
{
    char_u	*ਸਤਰ = ਨਿਰਾ;
    char_u	*p;
    ਪੂਰਨ		did_emsg_before = did_emsg;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROFILE
    ਪੂਰਨ		prof_lnum = -1;
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		debug_lnum = -1;

    ਲਈ (;;)
    {
	exarg_T	    ea;
	ਪੂਰਨ	    starts_with_colon = FALSE;
	char_u	    *cmd;
	cmdmod_T    local_cmdmod;

	// Bail out on the first error to avoid a flood of errors and report
	// the right line number when inside try/catch.
	ਜੇ (did_emsg_before != did_emsg)
	    ਵਾਪਸ FAIL;

	ਜੇ (ਸਤਰ != ਨਿਰਾ && *ਸਤਰ == '|')
	    // the line continues after a '|'
	    ++ਸਤਰ;
	ਨਹੀਂਤਾਂ ਜੇ (ਸਤਰ != ਨਿਰਾ && *skipwhite(ਸਤਰ) != NUL
		&& !(*ਸਤਰ == '#' && (ਸਤਰ == cctx->ctx_ਸਤਰ_ਸ਼ੁਰੂ
						    || VIM_ISWHITE(ਸਤਰ[-1]))))
	{
	    semsg(_(e_trailing_characters_str), ਸਤਰ);
	    ਵਾਪਸ FAIL;
	}
	ਨਹੀਂਤਾਂ ਜੇ (ਸਤਰ != ਨਿਰਾ && vim9_bad_comment(skipwhite(ਸਤਰ)))
	    ਵਾਪਸ FAIL;
	ਨਹੀਂਤਾਂ
	{
	    ਸਤਰ = ਅਗਲਾ_ਸਤਰ_from_context(cctx, FALSE);
	    ਜੇ (cctx->ctx_lnum >= ਆਖਰੀ_func_lnum)
	    {
		// beyond the last line
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROFILE
		ਜੇ (cctx->ctx_skip != SKIP_YES)
		    may_generate_prof_ਸਿਰਾ(cctx, prof_lnum);
#ਜੇ_ਅੰਤ
		ਰੋਕੋ;
	    }
	    // Make a copy, splitting off nextcmd and removing trailing spaces
	    // may change it.
	    ਜੇ (ਸਤਰ != ਨਿਰਾ)
	    {
		ਸਤਰ = vim_strsave(ਸਤਰ);
		ਜੇ (ga_ਜੋੜੋ_string(ਸਤਰਾਂ_ਮੰਜ਼ਿਲ_free, ਸਤਰ) == FAIL)
		{
		    vim_free(ਸਤਰ);
		    ਵਾਪਸ FAIL;
		}
	    }
	}

	CLEAR_ਖੇਤਰ(ea);
	ea.cmdlinep = &ਸਤਰ;
	ea.cmd = skipwhite(ਸਤਰ);
	ea.skip = cctx->ctx_skip == SKIP_YES;

	ਜੇ (*ea.cmd == '#')
	{
	    // "#" starts a comment, but "#{" is an error
	    ਜੇ (vim9_bad_comment(ea.cmd))
		ਵਾਪਸ FAIL;
	    ਸਤਰ = (char_u *)"";
	    ਜਾਰੀ;
	}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROFILE
	ਜੇ (cctx->ctx_compile_type == CT_PROFILE && cctx->ctx_lnum != prof_lnum
						  && cctx->ctx_skip != SKIP_YES)
	{
	    may_generate_prof_ਸਿਰਾ(cctx, prof_lnum);

	    prof_lnum = cctx->ctx_lnum;
	    generate_instr(cctx, ISN_PROF_ਸ਼ੁਰੂ);
	}
#ਜੇ_ਅੰਤ
	ਜੇ (cctx->ctx_compile_type == CT_DEBUG && cctx->ctx_lnum != debug_lnum
						  && cctx->ctx_skip != SKIP_YES)
	{
	    debug_lnum = cctx->ctx_lnum;
	    generate_instr_debug(cctx);
	}
	cctx->ctx_prev_lnum = cctx->ctx_lnum + 1;

	// Some things can be recognized by the first character.
	ਚੋਣ (*ea.cmd)
	{
	    ਹਾਲਤ '}':
		{
		    // "}" ends a block scope
		    scopetype_T stype = cctx->ctx_scope == ਨਿਰਾ
					  ? NO_SCOPE : cctx->ctx_scope->se_type;

		    ਜੇ (stype == ਖੰਡ_SCOPE)
		    {
			compile_endblock(cctx);
			ਸਤਰ = ea.cmd;
		    }
		    ਨਹੀਂਤਾਂ
		    {
			emsg(_(e_using_rcurly_outside_if_ਖੰਡ_scope));
			ਵਾਪਸ FAIL;
		    }
		    ਜੇ (ਸਤਰ != ਨਿਰਾ)
			ਸਤਰ = skipwhite(ea.cmd + 1);
		    ਜਾਰੀ;
		}

	    ਹਾਲਤ '{':
		// "{" starts a block scope
		// "{'a': 1}->func() is something else
		ਜੇ (ਸਿਰੇ_excmd(*skipwhite(ea.cmd + 1)))
		{
		    ਸਤਰ = compile_ਖੰਡ(ea.cmd, cctx);
		    ਜਾਰੀ;
		}
		ਰੋਕੋ;
	}

	/*
	 * COMMAND MODIFIERS
	 */
	cctx->ctx_has_cmdmod = FALSE;
	ਜੇ (parse_command_modifiers(&ea, errormsg, &local_cmdmod, FALSE)
								       == FAIL)
	    ਵਾਪਸ FAIL;
	generate_cmdmods(cctx, &local_cmdmod);
	undo_cmdmod(&local_cmdmod);

	// Check if there was a colon after the last command modifier or before
	// the current position.
	ਲਈ (p = ea.cmd; p >= ਸਤਰ; --p)
	{
	    ਜੇ (*p == ':')
		starts_with_colon = TRUE;
	    ਜੇ (p < ea.cmd && !VIM_ISWHITE(*p))
		ਰੋਕੋ;
	}

	// Skip ":call" to get to the function name, unless using :legacy
	p = ea.cmd;
	ਜੇ (!(local_cmdmod.cmod_flags & CMOD_LEGACY))
	{
	    ਜੇ (checkforcmd(&ea.cmd, "call", 3))
	    {
		ਜੇ (*ea.cmd == '(')
		    // not for "call()"
		    ea.cmd = p;
		ਨਹੀਂਤਾਂ
		    ea.cmd = skipwhite(ea.cmd);
	    }

	    ਜੇ (!starts_with_colon)
	    {
		ਪੂਰਨ	    ਸੌਂਪੋ;

		// Check for assignment after command modifiers.
		ਸੌਂਪੋ = may_compile_assignment(&ea, &ਸਤਰ, cctx);
		ਜੇ (ਸੌਂਪੋ == OK)
		    ਜਾਓ nextline;
		ਜੇ (ਸੌਂਪੋ == FAIL)
		    ਵਾਪਸ FAIL;
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
	ਜੇ ((*cmd != '$' || starts_with_colon)
		&& (starts_with_colon
		    || !(*cmd == '\''
			|| (cmd[0] == '0' && cmd[1] == 'z')
			|| (cmd[0] != NUL && cmd[0] == cmd[1]
					    && (*cmd == '+' || *cmd == '-'))
			|| ਸੰਖਿਆ_ਵਿਧੀ(cmd))))
	{
	    ea.cmd = skip_range(ea.cmd, TRUE, ਨਿਰਾ);
	    ਜੇ (ea.cmd > cmd)
	    {
		ਜੇ (!starts_with_colon
				   && !(local_cmdmod.cmod_flags & CMOD_LEGACY))
		{
		    semsg(_(e_colon_required_before_range_str), cmd);
		    ਵਾਪਸ FAIL;
		}
		ea.addr_ਗਿਣਤੀ = 1;
		ਜੇ (ਸਿਰੇ_excmd2(ਸਤਰ, ea.cmd))
		{
		    // A range without a command: jump to the line.
		    generate_EXEC(cctx, ISN_EXECRANGE,
					      vim_strnsave(cmd, ea.cmd - cmd));
		    ਸਤਰ = ea.cmd;
		    ਜਾਓ nextline;
		}
	    }
	}
	p = ਲੱਭੋ_ex_command(&ea, ਨਿਰਾ,
		starts_with_colon || (local_cmdmod.cmod_flags & CMOD_LEGACY)
						  ? ਨਿਰਾ : ਵਸਤੂ_exists, cctx);

	ਜੇ (p == ਨਿਰਾ)
	{
	    ਜੇ (cctx->ctx_skip != SKIP_YES)
		semsg(_(e_ambiguous_use_ਦਾ_user_defined_command_str), ea.cmd);
	    ਵਾਪਸ FAIL;
	}

	// When using ":legacy cmd" always use compile_exec().
	ਜੇ (local_cmdmod.cmod_flags & CMOD_LEGACY)
	{
	    char_u *ਸ਼ੁਰੂ = ea.cmd;

	    ਚੋਣ (ea.cmdidx)
	    {
		ਹਾਲਤ CMD_if:
		ਹਾਲਤ CMD_elseif:
		ਹਾਲਤ CMD_else:
		ਹਾਲਤ CMD_endif:
		ਹਾਲਤ CMD_for:
		ਹਾਲਤ CMD_endfor:
		ਹਾਲਤ CMD_continue:
		ਹਾਲਤ CMD_break:
		ਹਾਲਤ CMD_while:
		ਹਾਲਤ CMD_endwhile:
		ਹਾਲਤ CMD_try:
		ਹਾਲਤ CMD_catch:
		ਹਾਲਤ CMD_finally:
		ਹਾਲਤ CMD_endtry:
			semsg(_(e_cannot_use_legacy_with_command_str), ea.cmd);
			ਵਾਪਸ FAIL;
		ਮੂਲ: ਰੋਕੋ;
	    }

	    // ":legacy return expr" needs to be handled differently.
	    ਜੇ (checkforcmd(&ਸ਼ੁਰੂ, "return", 4))
		ea.cmdidx = CMD_return;
	    ਨਹੀਂਤਾਂ
		ea.cmdidx = CMD_legacy;
	}

	ਜੇ (p == ea.cmd && ea.cmdidx != CMD_ਮਾਪ)
	{
	    // "eval" is used for "val->func()" and "var" for "var = val", then
	    // "p" is equal to "ea.cmd" for a valid command.
	    ਜੇ (ea.cmdidx == CMD_eval || ea.cmdidx == CMD_var)
		;
	    ਨਹੀਂਤਾਂ ਜੇ (cctx->ctx_skip == SKIP_YES)
	    {
		ਸਤਰ += STRLEN(ਸਤਰ);
		ਜਾਓ nextline;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		semsg(_(e_command_not_recognized_str), ea.cmd);
		ਵਾਪਸ FAIL;
	    }
	}

	ਜੇ ((cctx->ctx_had_return || cctx->ctx_had_throw)
		&& ea.cmdidx != CMD_elseif
		&& ea.cmdidx != CMD_else
		&& ea.cmdidx != CMD_endif
		&& ea.cmdidx != CMD_endfor
		&& ea.cmdidx != CMD_endwhile
		&& ea.cmdidx != CMD_catch
		&& ea.cmdidx != CMD_finally
		&& ea.cmdidx != CMD_endtry
		&& !ignore_unreachable_ਕੋਡ_for_testing)
	{
	    semsg(_(e_unreachable_ਕੋਡ_after_str),
				     cctx->ctx_had_return ? "return" : "throw");
	    ਵਾਪਸ FAIL;
	}

	// When processing the end of an if-else or try-catch block, don't
	// clear the "ctx_had_throw" flag.  If such a block ends in a "throw"
	// statement, then it is considered to end in a "return" statement.
	// The "ctx_had_throw" is cleared immediately after processing the
	// block ending statement.
	// Otherwise, clear the "had_throw" flag.
	ਜੇ (ea.cmdidx != CMD_else && ea.cmdidx != CMD_elseif
						&& ea.cmdidx != CMD_endif
						&& ea.cmdidx != CMD_catch
						&& ea.cmdidx != CMD_finally
						&& ea.cmdidx != CMD_endtry)
	    cctx->ctx_had_throw = FALSE;

	p = skipwhite(p);
	ਜੇ (ea.cmdidx != CMD_ਮਾਪ
			    && ea.cmdidx != CMD_write && ea.cmdidx != CMD_read)
	{
	    ਜੇ (ea.cmdidx >= 0)
		ea.argt = excmd_get_argt(ea.cmdidx);
	    ਜੇ ((ea.argt & EX_BANG) && *p == '!')
	    {
		ea.forceit = TRUE;
		p = skipwhite(p + 1);
	    }
	    ਜੇ ((ea.argt & EX_RANGE) == 0 && ea.addr_ਗਿਣਤੀ > 0)
	    {
		emsg(_(e_no_range_allowed));
		ਵਾਪਸ FAIL;
	    }
	}

	ਚੋਣ (ea.cmdidx)
	{
	    ਹਾਲਤ CMD_def:
	    ਹਾਲਤ CMD_function:
		    ea.arg = p;
		    ਸਤਰ = compile_nested_function(&ea, cctx, ਸਤਰਾਂ_ਮੰਜ਼ਿਲ_free);
		    ਰੋਕੋ;

	    ਹਾਲਤ CMD_return:
		    ਸਤਰ = compile_return(p, check_return_type,
				 local_cmdmod.cmod_flags & CMOD_LEGACY, cctx);
		    cctx->ctx_had_return = TRUE;
		    ਰੋਕੋ;

	    ਹਾਲਤ CMD_let:
		    emsg(_(e_cannot_use_let_ਵਿੱਚ_vim9_script));
		    ਰੋਕੋ;
	    ਹਾਲਤ CMD_var:
	    ਹਾਲਤ CMD_final:
	    ਹਾਲਤ CMD_const:
	    ਹਾਲਤ CMD_increment:
	    ਹਾਲਤ CMD_decrement:
		    ਸਤਰ = compile_assignment(p, &ea, ea.cmdidx, cctx);
		    ਜੇ (ਸਤਰ == p)
		    {
			emsg(_(e_invalid_assignment));
			ਸਤਰ = ਨਿਰਾ;
		    }
		    ਰੋਕੋ;

	    ਹਾਲਤ CMD_unlet:
	    ਹਾਲਤ CMD_unlockvar:
	    ਹਾਲਤ CMD_lockvar:
		    ਸਤਰ = compile_unletlock(p, &ea, cctx);
		    ਰੋਕੋ;

	    ਹਾਲਤ CMD_import:
		    emsg(_(e_import_can_only_be_ਵਰਤਿਆ_ਵਿੱਚ_script));
		    ਸਤਰ = ਨਿਰਾ;
		    ਰੋਕੋ;

	    ਹਾਲਤ CMD_if:
		    ਸਤਰ = compile_if(p, cctx);
		    ਰੋਕੋ;
	    ਹਾਲਤ CMD_elseif:
		    ਸਤਰ = compile_elseif(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    cctx->ctx_had_throw = FALSE;
		    ਰੋਕੋ;
	    ਹਾਲਤ CMD_else:
		    ਸਤਰ = compile_else(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    cctx->ctx_had_throw = FALSE;
		    ਰੋਕੋ;
	    ਹਾਲਤ CMD_endif:
		    ਸਤਰ = compile_endif(p, cctx);
		    cctx->ctx_had_throw = FALSE;
		    ਰੋਕੋ;

	    ਹਾਲਤ CMD_while:
		    ਸਤਰ = compile_while(p, cctx);
		    ਰੋਕੋ;
	    ਹਾਲਤ CMD_endwhile:
		    ਸਤਰ = compile_endwhile(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    ਰੋਕੋ;

	    ਹਾਲਤ CMD_for:
		    ਸਤਰ = compile_for(p, cctx);
		    ਰੋਕੋ;
	    ਹਾਲਤ CMD_endfor:
		    ਸਤਰ = compile_endfor(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    ਰੋਕੋ;
	    ਹਾਲਤ CMD_continue:
		    ਸਤਰ = compile_continue(p, cctx);
		    // "continue" unconditionally jumps back to the loop,
		    // like a "return".
		    cctx->ctx_had_return = TRUE;
		    ਰੋਕੋ;
	    ਹਾਲਤ CMD_break:
		    ਸਤਰ = compile_break(p, cctx);
		    // "break" unconditionally exits the loop, like a "return".
		    cctx->ctx_had_return = TRUE;
		    ਰੋਕੋ;

	    ਹਾਲਤ CMD_try:
		    ਸਤਰ = compile_try(p, cctx);
		    ਰੋਕੋ;
	    ਹਾਲਤ CMD_catch:
		    ਸਤਰ = compile_catch(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    cctx->ctx_had_throw = FALSE;
		    ਰੋਕੋ;
	    ਹਾਲਤ CMD_finally:
		    ਸਤਰ = compile_finally(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    cctx->ctx_had_throw = FALSE;
		    ਰੋਕੋ;
	    ਹਾਲਤ CMD_endtry:
		    ਸਤਰ = compile_endtry(p, cctx);
		    cctx->ctx_had_throw = FALSE;
		    ਰੋਕੋ;
	    ਹਾਲਤ CMD_throw:
		    ਸਤਰ = compile_throw(p, cctx);
		    cctx->ctx_had_throw = TRUE;
		    ਰੋਕੋ;

	    ਹਾਲਤ CMD_eval:
		    ਸਤਰ = compile_eval(p, cctx);
		    ਰੋਕੋ;

	    ਹਾਲਤ CMD_defer:
		    ਸਤਰ = compile_defer(p, cctx);
		    ਰੋਕੋ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAS_ਸੁਨੇਹਾ_ਖਿੜਕੀ
	    ਹਾਲਤ CMD_echowindow:
		    {
			ਲੰਮਾ cmd_ਗਿਣਤੀ = get_cmd_ਗਿਣਤੀ(ਸਤਰ, &ea);
			ਜੇ (cmd_ਗਿਣਤੀ < 0)
			    ਸਤਰ = ਨਿਰਾ;
			ਨਹੀਂਤਾਂ
			    ਸਤਰ = compile_mult_ਵਿਅੰਜਕ(p, ea.cmdidx,
							     cmd_ਗਿਣਤੀ, cctx);
		    }
		    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
	    ਹਾਲਤ CMD_echo:
	    ਹਾਲਤ CMD_echon:
	    ਹਾਲਤ CMD_echoconsole:
	    ਹਾਲਤ CMD_echoerr:
	    ਹਾਲਤ CMD_echomsg:
	    ਹਾਲਤ CMD_execute:
		    ਸਤਰ = compile_mult_ਵਿਅੰਜਕ(p, ea.cmdidx, 0, cctx);
		    ਰੋਕੋ;

	    ਹਾਲਤ CMD_put:
		    ea.cmd = cmd;
		    ਸਤਰ = compile_put(p, &ea, cctx, FALSE);
		    ਰੋਕੋ;

	    ਹਾਲਤ CMD_iput:
		    ea.cmd = cmd;
		    ਸਤਰ = compile_put(p, &ea, cctx, TRUE);
		    ਰੋਕੋ;

	    ਹਾਲਤ CMD_substitute:
		    ਜੇ (check_global_and_subst(ea.cmd, p) == FAIL)
			ਵਾਪਸ FAIL;
		    ਜੇ (cctx->ctx_skip == SKIP_YES)
			ਸਤਰ = (char_u *)"";
		    ਨਹੀਂਤਾਂ
		    {
			ea.arg = p;
			ਸਤਰ = compile_substitute(ਸਤਰ, &ea, cctx);
		    }
		    ਰੋਕੋ;

	    ਹਾਲਤ CMD_redir:
		    ea.arg = p;
		    ਸਤਰ = compile_redir(ਸਤਰ, &ea, cctx);
		    ਰੋਕੋ;

	    ਹਾਲਤ CMD_cexpr:
	    ਹਾਲਤ CMD_lexpr:
	    ਹਾਲਤ CMD_caddexpr:
	    ਹਾਲਤ CMD_laddexpr:
	    ਹਾਲਤ CMD_cgetexpr:
	    ਹਾਲਤ CMD_lgetexpr:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
		    ea.arg = p;
		    ਸਤਰ = compile_cexpr(ਸਤਰ, &ea, cctx);
#ਨਹੀਂਤਾਂ
		    ex_ni(&ea);
		    ਸਤਰ = ਨਿਰਾ;
#ਜੇ_ਅੰਤ
		    ਰੋਕੋ;

	    ਹਾਲਤ CMD_append:
	    ਹਾਲਤ CMD_change:
	    ਹਾਲਤ CMD_ਪਾਓ:
	    ਹਾਲਤ CMD_k:
	    ਹਾਲਤ CMD_open:
	    ਹਾਲਤ CMD_t:
	    ਹਾਲਤ CMD_xit:
		    not_ਵਿੱਚ_vim9(&ea, false);
		    ਵਾਪਸ FAIL;

	    ਹਾਲਤ CMD_ਮਾਪ:
		    ਜੇ (cctx->ctx_skip != SKIP_YES)
		    {
			semsg(_(e_invalid_command_str), ea.cmd);
			ਵਾਪਸ FAIL;
		    }
		    // We don't check for a next command here.
		    ਸਤਰ = (char_u *)"";
		    ਰੋਕੋ;

	    ਹਾਲਤ CMD_lua:
	    ਹਾਲਤ CMD_mzscheme:
	    ਹਾਲਤ CMD_perl:
	    ਹਾਲਤ CMD_py3:
	    ਹਾਲਤ CMD_python3:
	    ਹਾਲਤ CMD_python:
	    ਹਾਲਤ CMD_pythonx:
	    ਹਾਲਤ CMD_ruby:
	    ਹਾਲਤ CMD_tcl:
		    ea.arg = p;
		    ਜੇ (vim_strchr(ਸਤਰ, '\n') == ਨਿਰਾ)
			ਸਤਰ = compile_exec(ਸਤਰ, &ea, cctx);
		    ਨਹੀਂਤਾਂ
			// heredoc lines have been concatenated with NL
			// characters in get_function_body()
			ਸਤਰ = compile_script(ਸਤਰ, cctx);
		    ਰੋਕੋ;

	    ਹਾਲਤ CMD_vim9script:
		    ਜੇ (cctx->ctx_skip != SKIP_YES)
		    {
			emsg(_(e_vim9script_can_only_be_ਵਰਤਿਆ_ਵਿੱਚ_script));
			ਵਾਪਸ FAIL;
		    }
		    ਸਤਰ = (char_u *)"";
		    ਰੋਕੋ;

	    ਹਾਲਤ CMD_class:
		    emsg(_(e_class_can_only_be_ਵਰਤਿਆ_ਵਿੱਚ_script));
		    ਵਾਪਸ FAIL;

	    ਹਾਲਤ CMD_enum:
		    emsg(_(e_enum_can_only_be_ਵਰਤਿਆ_ਵਿੱਚ_script));
		    ਵਾਪਸ FAIL;

	    ਹਾਲਤ CMD_interface:
		    emsg(_(e_interface_can_only_be_ਵਰਤਿਆ_ਵਿੱਚ_script));
		    ਵਾਪਸ FAIL;

	    ਹਾਲਤ CMD_type:
		    emsg(_(e_type_can_only_be_ਵਰਤਿਆ_ਵਿੱਚ_script));
		    ਵਾਪਸ FAIL;

	    ਹਾਲਤ CMD_global:
		    ਜੇ (check_global_and_subst(ea.cmd, p) == FAIL)
			ਵਾਪਸ FAIL;
		    // FALLTHROUGH
	    ਮੂਲ:
		    // Not recognized, execute with do_cmdline_cmd().
		    ea.arg = p;
		    ਸਤਰ = compile_exec(ਸਤਰ, &ea, cctx);
		    ਰੋਕੋ;
	}
nextline:
	ਜੇ (ਸਤਰ == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;
	ਸਤਰ = skipwhite(ਸਤਰ);

	// Undo any command modifiers.
	generate_undo_cmdmods(cctx);

	ਜੇ (cctx->ctx_type_ਢੇਰ.ga_len < 0)
	{
	    iemsg("Type stack underflow");
	    ਵਾਪਸ FAIL;
	}
    } // END of the loop over all the function body lines.

    ਵਾਪਸ OK;
}

/*
 * Returns TRUE if the end of a scope (if, while, for, block) is missing.
 * Called after compiling a def function body.
 */
    ਸਥਿਰ ਪੂਰਨ
compile_dfunc_scope_ਸਿਰਾ_missing(cctx_T *cctx)
{
    ਜੇ (cctx->ctx_scope == ਨਿਰਾ)
	ਵਾਪਸ FALSE;

    ਚੋਣ (cctx->ctx_scope->se_type)
    {
	ਹਾਲਤ IF_SCOPE:
	    emsg(_(e_missing_endif));
	    ਰੋਕੋ;
	ਹਾਲਤ WHILE_SCOPE:
	    emsg(_(e_missing_endwhile));
	    ਰੋਕੋ;
	ਹਾਲਤ FOR_SCOPE:
	    emsg(_(e_missing_endfor));
	    ਰੋਕੋ;
	ਹਾਲਤ TRY_SCOPE:
	    emsg(_(e_missing_endtry));
	    ਰੋਕੋ;
	ਹਾਲਤ ਖੰਡ_SCOPE:
	    // end block scope from :try (maybe)
	    compile_endblock(cctx);
	    ਜੇ (cctx->ctx_scope != ਨਿਰਾ
		    && cctx->ctx_scope->se_type == TRY_SCOPE)
		emsg(_(e_missing_endtry));
	    ਨਹੀਂਤਾਂ
		emsg(_(e_missing_rcurly));
	    ਰੋਕੋ;
	ਮੂਲ:
	    emsg(_(e_missing_rcurly));
    }
    ਵਾਪਸ TRUE;
}

/*
 * When compiling a def function, if it doesn't have an explicit return
 * statement, then generate a default return instruction.  For an object
 * constructor, return the object.
 */
    ਸਥਿਰ ਪੂਰਨ
compile_dfunc_generate_default_return(ufunc_T *ufunc, cctx_T *cctx)
{
    // TODO: if a function ends in "throw" but there was a return elsewhere we
    // should not assume the return type is "void".
    ਜੇ (cctx->ctx_had_return || cctx->ctx_had_throw)
	ਵਾਪਸ OK;

    ਜੇ (ufunc->uf_ret_type->tt_type == VAR_UNKNOWN)
	ufunc->uf_ret_type = &t_void;
    ਨਹੀਂਤਾਂ ਜੇ (ufunc->uf_ret_type->tt_type != VAR_VOID
					&& !IS_CONSTRUCTOR_ਵਿਧੀ(ufunc))
    {
	emsg(_(e_missing_return_statement));
	ਵਾਪਸ FAIL;
    }

    // Return void if there is no return at the end.
    // For a constructor return the object.
    ਜੇ (IS_CONSTRUCTOR_ਵਿਧੀ(ufunc))
    {
	generate_instr(cctx, ISN_RETURN_OBJECT);
	ufunc->uf_ret_type = &ufunc->uf_class->class_object_type;
    }
    ਨਹੀਂਤਾਂ
	generate_instr(cctx, ISN_RETURN_VOID);

    ਵਾਪਸ OK;
}

/*
 * Perform the chores after successfully compiling a def function.
 */
    ਸਥਿਰ ਖਾਲੀ
compile_dfunc_epilogue(
    cctx_T	*outer_cctx,
    ufunc_T	*ufunc,
    garray_T	*instr,
    cctx_T	*cctx)
{
    dfunc_T	*dfunc;

    dfunc = ((dfunc_T *)def_functions.ga_ਅੰਕੜੇ) + ufunc->uf_dfunc_idx;
    dfunc->df_deleted = FALSE;
    dfunc->df_script_seq = current_sctx.sc_seq;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROFILE
    ਜੇ (cctx->ctx_compile_type == CT_PROFILE)
    {
	dfunc->df_instr_prof = instr->ga_ਅੰਕੜੇ;
	dfunc->df_instr_prof_ਗਿਣਤੀ = instr->ga_len;
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	ਜੇ (cctx->ctx_compile_type == CT_DEBUG)
	{
	    dfunc->df_instr_debug = instr->ga_ਅੰਕੜੇ;
	    dfunc->df_instr_debug_ਗਿਣਤੀ = instr->ga_len;
	}
	ਨਹੀਂਤਾਂ
	{
	    dfunc->df_instr = instr->ga_ਅੰਕੜੇ;
	    dfunc->df_instr_ਗਿਣਤੀ = instr->ga_len;
	}
    dfunc->df_varcount = dfunc->df_var_ਨਾਂ.ga_len;
    dfunc->df_has_ਘੇਰਾ = cctx->ctx_has_ਘੇਰਾ;

    ਜੇ (cctx->ctx_outer_ਵਰਤਿਆ)
    {
	ufunc->uf_flags |= FC_ਘੇਰਾ;
	ਜੇ (outer_cctx != ਨਿਰਾ)
	    ++outer_cctx->ctx_ਘੇਰਾ_ਗਿਣਤੀ;
    }

    ufunc->uf_def_ਸਥਿਤੀ = UF_COMPILED;
}

/*
 * Perform the cleanup when a def function compilation fails.
 */
    ਸਥਿਰ ਖਾਲੀ
compile_dfunc_ufunc_cleanup(
    ufunc_T	*ufunc,
    garray_T	*instr,
    ਪੂਰਨ		new_def_function,
    ਅੱਖਰ	*errormsg,
    ਪੂਰਨ		did_emsg_before,
    cctx_T	*cctx)
{
    dfunc_T	*dfunc;

    dfunc = ((dfunc_T *)def_functions.ga_ਅੰਕੜੇ) + ufunc->uf_dfunc_idx;

    // Compiling aborted, free the generated instructions.
    clear_instr_ga(instr);
    VIM_CLEAR(dfunc->df_name);
    ga_clear_ਲੜੀਆਂ(&dfunc->df_var_ਨਾਂ);

    // If using the last entry in the table and it was added above, we
    // might as well remove it.
    ਜੇ (!dfunc->df_deleted && new_def_function
			&& ufunc->uf_dfunc_idx == def_functions.ga_len - 1)
    {
	--def_functions.ga_len;
	ufunc->uf_dfunc_idx = 0;
    }
    ufunc->uf_def_ਸਥਿਤੀ = UF_COMPILE_ERROR;

    ਜਦੋਂ (cctx->ctx_scope != ਨਿਰਾ)
	drop_scope(cctx);

    ਜੇ (errormsg != ਨਿਰਾ)
	emsg(errormsg);
    ਨਹੀਂਤਾਂ ਜੇ (did_emsg == did_emsg_before)
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
    ਪੂਰਨ
compile_def_function(
	ufunc_T		*ufunc,
	ਪੂਰਨ		check_return_type,
	compiletype_T   compile_type,
	cctx_T		*outer_cctx)
{
    garray_T	ਸਤਰਾਂ_ਮੰਜ਼ਿਲ_free;
    ਅੱਖਰ	*errormsg = ਨਿਰਾ;	// error message
    cctx_T	cctx;
    garray_T	*instr;
    ਪੂਰਨ		did_emsg_before = did_emsg;
    ਪੂਰਨ		did_emsg_silent_before = did_emsg_silent;
    ਪੂਰਨ		ret = FAIL;
    sctx_T	save_current_sctx = current_sctx;
    ਪੂਰਨ		save_estack_compiling = estack_compiling;
    ਪੂਰਨ		save_cmod_flags = cmdmod.cmod_flags;
    ਪੂਰਨ		do_estack_ਧੱਕੋ;
    ਪੂਰਨ		new_def_function = FALSE;

    // allocated lines are freed at the end
    ga_init2(&ਸਤਰਾਂ_ਮੰਜ਼ਿਲ_free, ਆਕਾਰ(char_u *), 50);

    // Initialize the ufunc and the compilation context
    ਜੇ (compile_dfunc_ufunc_init(ufunc, outer_cctx, compile_type,
						&new_def_function) == FAIL)
	ਵਾਪਸ FAIL;

    compile_dfunc_cctx_init(&cctx, outer_cctx, ufunc, compile_type);

    instr = &cctx.ctx_instr;

    // Set the context to the function, it may be compiled when called from
    // another script.  Set the script version to the most modern one.
    // The line number will be set in next_line_from_context().
    current_sctx = ufunc->uf_script_ctx;
    current_sctx.sc_ਸੰਸਕਰਣ = SCRIPT_ਸੰਸਕਰਣ_VIM9;

    // Don't use the flag from ":legacy" here.
    cmdmod.cmod_flags &= ~CMOD_LEGACY;

    // Make sure error messages are OK.
    do_estack_ਧੱਕੋ = !estack_ਸਿਖਰ_is_ufunc(ufunc, 1);
    ਜੇ (do_estack_ਧੱਕੋ)
	estack_ਧੱਕੋ_ufunc(ufunc, 1);
    estack_compiling = TRUE;

    // Make sure arguments don't shadow variables in the context
    ਜੇ (check_ਦਲੀਲਾਂ_shadowing(ufunc, &cctx) == FAIL)
	ਜਾਓ erret;

    // For an object method and a constructor generate instructions to
    // initialize "this" and the object variables.
    ਜੇ (ufunc->uf_flags & (FC_OBJECT|FC_NEW))
	ਜੇ (obj_ਵਿਧੀ_prologue(ufunc, &cctx) == FAIL)
	    ਜਾਓ erret;

    ਜੇ (ufunc->uf_def_ਦਲੀਲਾਂ.ga_len > 0)
	ਜੇ (compile_def_function_default_ਦਲੀਲਾਂ(ufunc, instr, &cctx) == FAIL)
	    ਜਾਓ erret;
    ufunc->uf_ਦਲੀਲਾਂ_visible = ufunc->uf_ਦਲੀਲਾਂ.ga_len;

    // Compiling an abstract method or a function in an interface is done to
    // get the function type.  No code is actually compiled.
    ਜੇ (ufunc->uf_class != ਨਿਰਾ && (IS_INTERFACE(ufunc->uf_class)
						|| IS_ABSTRACT_ਵਿਧੀ(ufunc)))
    {
	ufunc->uf_def_ਸਥਿਤੀ = UF_NOT_COMPILED;
	ret = OK;
	ਜਾਓ erret;
    }

    // compile the function body
    ਜੇ (compile_def_function_body(ufunc->uf_ਸਤਰਾਂ.ga_len, check_return_type,
				&ਸਤਰਾਂ_ਮੰਜ਼ਿਲ_free, &errormsg, &cctx) == FAIL)
	ਜਾਓ erret;

    ਜੇ (compile_dfunc_scope_ਸਿਰਾ_missing(&cctx))
	ਜਾਓ erret;

    ਜੇ (compile_dfunc_generate_default_return(ufunc, &cctx) == FAIL)
	ਜਾਓ erret;

    // When compiled with ":silent!" and there was an error don't consider the
    // function compiled.
    ਜੇ (emsg_silent == 0 || did_emsg_silent == did_emsg_silent_before)
	compile_dfunc_epilogue(outer_cctx, ufunc, instr, &cctx);

    ret = OK;

erret:
    ਜੇ (ufunc->uf_def_ਸਥਿਤੀ == UF_COMPILING)
    {
	// compilation failed. do cleanup.
	compile_dfunc_ufunc_cleanup(ufunc, instr, new_def_function,
				    errormsg, did_emsg_before, &cctx);
    }

    ਜੇ (cctx.ctx_redir_lhs.lhs_name != ਨਿਰਾ)
    {
	ਜੇ (ret == OK)
	{
	    emsg(_(e_missing_redir_ਸਿਰਾ));
	    ret = FAIL;
	}
	vim_free(cctx.ctx_redir_lhs.lhs_name);
	vim_free(cctx.ctx_redir_lhs.lhs_ਸਾਰਾ);
    }

    current_sctx = save_current_sctx;
    estack_compiling = save_estack_compiling;
    cmdmod.cmod_flags =	save_cmod_flags;
    ਜੇ (do_estack_ਧੱਕੋ)
	estack_ਕੱਢੋ();

    ga_clear_ਲੜੀਆਂ(&ਸਤਰਾਂ_ਮੰਜ਼ਿਲ_free);
    free_locals(&cctx);
    ga_clear(&cctx.ctx_type_ਢੇਰ);
    ਵਾਪਸ ret;
}

    ਖਾਲੀ
ਸੈੱਟ_function_type(ufunc_T *ufunc)
{
    ਪੂਰਨ varargs = ufunc->uf_va_name != ਨਿਰਾ;
    ਪੂਰਨ argcount = ufunc->uf_ਦਲੀਲਾਂ.ga_len;

    // Create a type for the function, with the return type and any
    // argument types.
    // A vararg is included in uf_args.ga_len but not in uf_arg_types.
    // The type is included in "tt_args".
    ਜੇ (argcount > 0 || varargs)
    {
	ਜੇ (ufunc->uf_type_list.ga_itemsize == 0)
	    ga_init2(&ufunc->uf_type_list, ਆਕਾਰ(type_T *), 10);
	ufunc->uf_func_type = alloc_func_type(ufunc->uf_ret_type,
					   argcount, &ufunc->uf_type_list);
	// Add argument types to the function type.
	ਜੇ (func_type_ਜੋੜੋ_arg_types(ufunc->uf_func_type,
				    argcount + varargs,
				    &ufunc->uf_type_list) == FAIL)
	    ਵਾਪਸ;
	ufunc->uf_func_type->tt_argcount = argcount + varargs;
	ufunc->uf_func_type->tt_min_argcount =
				      argcount - ufunc->uf_def_ਦਲੀਲਾਂ.ga_len;
	ਜੇ (ufunc->uf_arg_types == ਨਿਰਾ)
	{
	    ਪੂਰਨ i;

	    // lambda does not have argument types.
	    ਲਈ (i = 0; i < argcount; ++i)
		ufunc->uf_func_type->tt_ਦਲੀਲਾਂ[i] = &t_any;
	}
	ਨਹੀਂਤਾਂ
	    mch_memmove(ufunc->uf_func_type->tt_ਦਲੀਲਾਂ,
			 ufunc->uf_arg_types, ਆਕਾਰ(type_T *) * argcount);
	ਜੇ (varargs)
	{
	    ufunc->uf_func_type->tt_ਦਲੀਲਾਂ[argcount] =
		   ufunc->uf_va_type == ਨਿਰਾ ? &t_list_any : ufunc->uf_va_type;
	    ufunc->uf_func_type->tt_flags = TTFLAG_VARARGS;
	}
    }
    ਨਹੀਂਤਾਂ
	// No arguments, can use a predefined type.
	ufunc->uf_func_type = get_func_type(ufunc->uf_ret_type,
					   argcount, &ufunc->uf_type_list);
}

/*
 * Free all instructions for "dfunc" except df_name.
 */
    ਸਥਿਰ ਖਾਲੀ
delete_def_function_contents(dfunc_T *dfunc, ਪੂਰਨ mark_deleted)
{
    ਪੂਰਨ idx;

    // In same cases the instructions may refer to a class in which the
    // function is defined and unreferencing the class may call back here
    // recursively.  Set the df_delete_busy to avoid problems.
    ਜੇ (dfunc->df_delete_busy)
	ਵਾਪਸ;
    dfunc->df_delete_busy = TRUE;

    ga_clear(&dfunc->df_def_ਦਲੀਲਾਂ_isn);
    ga_clear_ਲੜੀਆਂ(&dfunc->df_var_ਨਾਂ);

    ਜੇ (dfunc->df_instr != ਨਿਰਾ)
    {
	ਲਈ (idx = 0; idx < dfunc->df_instr_ਗਿਣਤੀ; ++idx)
	    delete_instr(dfunc->df_instr + idx);
	VIM_CLEAR(dfunc->df_instr);
    }
    ਜੇ (dfunc->df_instr_debug != ਨਿਰਾ)
    {
	ਲਈ (idx = 0; idx < dfunc->df_instr_debug_ਗਿਣਤੀ; ++idx)
	    delete_instr(dfunc->df_instr_debug + idx);
	VIM_CLEAR(dfunc->df_instr_debug);
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROFILE
    ਜੇ (dfunc->df_instr_prof != ਨਿਰਾ)
    {
	ਲਈ (idx = 0; idx < dfunc->df_instr_prof_ਗਿਣਤੀ; ++idx)
	    delete_instr(dfunc->df_instr_prof + idx);
	VIM_CLEAR(dfunc->df_instr_prof);
    }
#ਜੇ_ਅੰਤ

    ਜੇ (mark_deleted)
	dfunc->df_deleted = TRUE;
    ਜੇ (dfunc->df_ufunc != ਨਿਰਾ)
	dfunc->df_ufunc->uf_def_ਸਥਿਤੀ = UF_NOT_COMPILED;

    dfunc->df_delete_busy = FALSE;
}

/*
 * When a user function is deleted, clear the contents of any associated def
 * function, unless another user function still uses it.
 * The position in def_functions can be re-used.
 */
    ਖਾਲੀ
unlink_def_function(ufunc_T *ufunc)
{
    ਜੇ (ufunc->uf_dfunc_idx <= 0 || def_functions.ga_ਅੰਕੜੇ == ਨਿਰਾ)
	ਵਾਪਸ;

    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_ਅੰਕੜੇ)
	+ ufunc->uf_dfunc_idx;

    ਜੇ (--dfunc->df_refcount <= 0)
	delete_def_function_contents(dfunc, TRUE);
    ufunc->uf_def_ਸਥਿਤੀ = UF_NOT_COMPILED;
    ufunc->uf_dfunc_idx = 0;
    ਜੇ (dfunc->df_ufunc == ufunc)
	dfunc->df_ufunc = ਨਿਰਾ;
}

/*
 * Used when a user function refers to an existing dfunc.
 */
    ਖਾਲੀ
link_def_function(ufunc_T *ufunc)
{
    ਜੇ (ufunc->uf_dfunc_idx <= 0)
	ਵਾਪਸ;

    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_ਅੰਕੜੇ)
	+ ufunc->uf_dfunc_idx;

    ++dfunc->df_refcount;
}

#ਜੇ defined(EXITFREE)
/*
 * Free all functions defined with ":def".
 */
    ਖਾਲੀ
free_def_functions(ਖਾਲੀ)
{
    ਪੂਰਨ idx;

    ਲਈ (idx = 0; idx < def_functions.ga_len; ++idx)
    {
	dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_ਅੰਕੜੇ) + idx;

	delete_def_function_contents(dfunc, TRUE);
	vim_free(dfunc->df_name);
    }

    ga_clear(&def_functions);
}
#ਜੇ_ਅੰਤ


#ਜੇ_ਅੰਤ // FEAT_EVAL
