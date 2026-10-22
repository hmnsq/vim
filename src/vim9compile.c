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

#define USING_FLOAT_STUFF
#അന്തര്ഭാവ "vim.h"

#എങ്കിൽ defined(FEAT_EVAL)

// Functions defined with :def are stored in this growarray.
// They are never removed, so that they can be found by index.
// Deleted functions have the df_deleted flag set.
garray_T def_functions = {0, 0, വലുപ്പം(dfunc_T), 50, ശൂന്യ};

സ്ഥിതം ശൂന്യം delete_def_function_contents(dfunc_T *dfunc, പൂർണ്ണസംഖ്യ mark_deleted);

/*
 * Lookup variable "name" in the local scope and return it in "lvar".
 * "lvar->lv_from_outer" is incremented accordingly.
 * If "lvar" is NULL only check if the variable can be found.
 * Return FAIL if not found.
 */
    പൂർണ്ണസംഖ്യ
lookup_local(char_u *name, size_t len, lvar_T *lvar, cctx_T *cctx)
{
    പൂർണ്ണസംഖ്യ	    idx;
    lvar_T  *lvp;

    എങ്കിൽ (len == 0)
	തിരികെ FAIL;

    എങ്കിൽ (((len == 4 && STRNCMP(name, "this", 4) == 0)
		|| (len == 5 && STRNCMP(name, "super", 5) == 0))
	    && cctx->ctx_ufunc != ശൂന്യ
	    && (cctx->ctx_ufunc->uf_flags & (FC_OBJECT|FC_NEW)))
    {
	പൂർണ്ണസംഖ്യ is_super = *name == 's';
	എങ്കിൽ (is_super)
	{
	    എങ്കിൽ (name[5] != '.')
	    {
		emsg(_(e_super_must_be_followed_by_dot));
		തിരികെ FAIL;
	    }
	    എങ്കിൽ (cctx->ctx_ufunc->uf_class != ശൂന്യ
		    && cctx->ctx_ufunc->uf_class->class_extends == ശൂന്യ)
	    {
		emsg(_(e_using_super_not_in_child_class));
		തിരികെ FAIL;
	    }
	}
	എങ്കിൽ (lvar != ശൂന്യ)
	{
	    CLEAR_POINTER(lvar);
	    lvar->lv_loop_depth = -1;
	    lvar->lv_name = (char_u *)(is_super ? "super" : "this");
	    എങ്കിൽ (cctx->ctx_ufunc->uf_class != ശൂന്യ)
	    {
		lvar->lv_type = &cctx->ctx_ufunc->uf_class->class_object_type;
		എങ്കിൽ (is_super)
		{
		    type_T *type = get_type_ptr(cctx->ctx_type_list);

		    എങ്കിൽ (type != ശൂന്യ)
		    {
			*type = *lvar->lv_type;
			lvar->lv_type = type;
			type->tt_flags |= TTFLAG_SUPER;
		    }
		}
	    }
	}
	തിരികെ OK;
    }

    // Find local in current function scope.
    വേണ്ടി (idx = 0; idx < cctx->ctx_locals.ga_len; ++idx)
    {
	lvp = ((lvar_T *)cctx->ctx_locals.ga_data) + idx;
	എങ്കിൽ (lvp->lv_name != ശൂന്യ
		&& STRNCMP(name, lvp->lv_name, len) == 0
					       && STRLEN(lvp->lv_name) == len)
	{
	    എങ്കിൽ (lvar != ശൂന്യ)
	    {
		*lvar = *lvp;
		lvar->lv_from_outer = 0;
		// If the variable was declared inside a loop set
		// lvar->lv_loop_idx and lvar->lv_loop_depth.
		get_loop_var_idx(cctx, idx, lvar);
	    }
	    തിരികെ OK;
	}
    }

    // Find local in outer function scope.
    എങ്കിൽ (cctx->ctx_outer != ശൂന്യ)
    {
	എങ്കിൽ (lookup_local(name, len, lvar, cctx->ctx_outer) == OK)
	{
	    എങ്കിൽ (lvar != ശൂന്യ)
	    {
		cctx->ctx_outer_used = TRUE;
		++lvar->lv_from_outer;
	    }
	    തിരികെ OK;
	}
    }

    തിരികെ FAIL;
}

/*
 * Lookup an argument in the current function and an enclosing function.
 * Returns the argument index in "idxp"
 * Returns the argument type in "type"
 * Sets "gen_load_outer" to TRUE if found in outer scope.
 * Returns OK when found, FAIL otherwise.
 */
    പൂർണ്ണസംഖ്യ
arg_exists(
	char_u	*name,
	size_t	len,
	പൂർണ്ണസംഖ്യ	*idxp,
	type_T	**type,
	പൂർണ്ണസംഖ്യ	*gen_load_outer,
	cctx_T	*cctx)
{
    പൂർണ്ണസംഖ്യ	    idx;
    char_u  *va_name;

    എങ്കിൽ (len == 0)
	തിരികെ FAIL;
    വേണ്ടി (idx = 0; idx < cctx->ctx_ufunc->uf_args_visible; ++idx)
    {
	char_u *arg = FUNCARG(cctx->ctx_ufunc, idx);

	എങ്കിൽ (STRNCMP(name, arg, len) == 0 && arg[len] == NUL)
	{
	    എങ്കിൽ (idxp != ശൂന്യ)
	    {
		// Arguments are located above the frame pointer.  One further
		// if there is a vararg argument
		*idxp = idx - (cctx->ctx_ufunc->uf_args.ga_len
							    + അട്ടി_FRAME_SIZE)
			      + (cctx->ctx_ufunc->uf_va_name != ശൂന്യ ? -1 : 0);

		എങ്കിൽ (cctx->ctx_ufunc->uf_arg_types != ശൂന്യ)
		    *type = cctx->ctx_ufunc->uf_arg_types[idx];
		അല്ലെങ്കിൽ
		    *type = &t_any;
	    }
	    തിരികെ OK;
	}
    }

    va_name = cctx->ctx_ufunc->uf_va_name;
    എങ്കിൽ (va_name != ശൂന്യ
		    && STRNCMP(name, va_name, len) == 0 && va_name[len] == NUL)
    {
	എങ്കിൽ (idxp != ശൂന്യ)
	{
	    // varargs is always the last argument
	    *idxp = -അട്ടി_FRAME_SIZE - 1;
	    *type = cctx->ctx_ufunc->uf_va_type;
	}
	തിരികെ OK;
    }

    എങ്കിൽ (cctx->ctx_outer != ശൂന്യ)
    {
	// Lookup the name for an argument of the outer function.
	എങ്കിൽ (arg_exists(name, len, idxp, type, gen_load_outer, cctx->ctx_outer)
									 == OK)
	{
	    എങ്കിൽ (gen_load_outer != ശൂന്യ)
	    {
		++*gen_load_outer;
		cctx->ctx_outer_used = TRUE;
	    }
	    തിരികെ OK;
	}
    }

    തിരികെ FAIL;
}

/*
 * Lookup a script-local variable in the current script, possibly defined in a
 * block that contains the function "cctx->ctx_ufunc".
 * "cctx" is NULL at the script level, "cstack" is NULL in a function.
 * If "len" is <= 0 "name" must be NUL terminated.
 * Return NULL when not found.
 */
    സ്ഥിതം sallvar_T *
find_script_var(char_u *name, size_t len, cctx_T *cctx, cstack_T *cstack)
{
    scriptitem_T    *si = SCRIPT_ITEM(current_sctx.sc_sid);
    hashitem_T	    *hi;
    പൂർണ്ണസംഖ്യ		    cc;
    sallvar_T	    *sav;
    ufunc_T	    *ufunc;

    // Find the list of all script variables with the right name.
    എങ്കിൽ (len > 0)
    {
	cc = name[len];
	name[len] = NUL;
    }
    hi = hash_find(&si->sn_all_vars.dv_hashtab, name);
    എങ്കിൽ (len > 0)
	name[len] = cc;
    എങ്കിൽ (HASHITEM_EMPTY(hi))
	തിരികെ ശൂന്യ;

    sav = HI2SAV(hi);
    എങ്കിൽ (sav->sav_block_id == 0)
	// variable defined in the top script scope is always visible
	തിരികെ sav;

    എങ്കിൽ (cctx == ശൂന്യ)
    {
	എങ്കിൽ (cstack == ശൂന്യ)
	    തിരികെ ശൂന്യ;

	// Not in a function scope, find variable with block ID equal to or
	// smaller than the current block id.  Use "cstack" to go up the block
	// scopes.
	ആകുമ്പോൾ (sav != ശൂന്യ)
	{
	    പൂർണ്ണസംഖ്യ idx;

	    വേണ്ടി (idx = cstack->cs_idx; idx >= 0; --idx)
		എങ്കിൽ (cstack->cs_block_id[idx] == sav->sav_block_id)
		    നിർത്തുക;
	    എങ്കിൽ (idx >= 0)
		നിർത്തുക;
	    sav = sav->sav_next;
	}
	തിരികെ sav;
    }

    // Go over the variables with this name and find one that was visible
    // from the function.
    ufunc = cctx->ctx_ufunc;
    ആകുമ്പോൾ (sav != ശൂന്യ)
    {
	പൂർണ്ണസംഖ്യ idx;

	എങ്കിൽ (ufunc->uf_block_depth == 0 && sav->sav_block_id == 0)
	    // If the function was defined at the script level (not inside a
	    // block), script-scope variables are always visible.
	    തിരികെ sav;

	// Go over the blocks that this function was defined in.  If the
	// variable block ID matches it was visible to the function.
	വേണ്ടി (idx = 0; idx < ufunc->uf_block_depth; ++idx)
	    എങ്കിൽ (ufunc->uf_block_ids[idx] == sav->sav_block_id)
		തിരികെ sav;
	sav = sav->sav_next;
    }

    // Not found, variable was not visible.
    തിരികെ ശൂന്യ;
}

/*
 * If "name" can be found in the current script set it's "block_id".
 */
    ശൂന്യം
update_script_var_block_id(char_u *name, പൂർണ്ണസംഖ്യ block_id)
{
    scriptitem_T    *si = SCRIPT_ITEM(current_sctx.sc_sid);
    hashitem_T	    *hi;
    sallvar_T	    *sav;

    hi = hash_find(&si->sn_all_vars.dv_hashtab, name);
    എങ്കിൽ (HASHITEM_EMPTY(hi))
	തിരികെ;
    sav = HI2SAV(hi);
    sav->sav_block_id = block_id;
}

/*
 * Lookup a variable (without s: prefix) in the current script.
 * "cctx" is NULL at the script level, "cstack" is NULL in a function.
 * Returns OK or FAIL.
 */
    പൂർണ്ണസംഖ്യ
script_var_exists(char_u *name, size_t len, cctx_T *cctx, cstack_T *cstack)
{
    എങ്കിൽ (current_sctx.sc_sid <= 0)
	തിരികെ FAIL;
    എങ്കിൽ (current_script_is_vim9())
    {
	// Check script variables that were visible where the function was
	// defined.
	എങ്കിൽ (find_script_var(name, len, cctx, cstack) != ശൂന്യ)
	    തിരികെ OK;
    }
    അല്ലെങ്കിൽ
    {
	hashtab_T	*ht = &SCRIPT_VARS(current_sctx.sc_sid);
	dictitem_T	*di;
	പൂർണ്ണസംഖ്യ		cc;

	// Check script variables that are currently visible
	cc = name[len];
	name[len] = NUL;
	di = find_var_in_ht(ht, 0, name, TRUE);
	name[len] = cc;
	എങ്കിൽ (di != ശൂന്യ)
	    തിരികെ OK;
    }

    തിരികെ FAIL;
}

/*
 * Returns the index of a class method or class variable with name "name"
 * accessible in the currently compiled function.
 * If "cl_ret" is not NULL set it to the class.
 * Otherwise return -1.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
cctx_class_midx(
    cctx_T  *cctx,
    പൂർണ്ണസംഖ്യ	    is_method,
    char_u  *name,
    size_t  len,
    class_T **cl_ret)
{
    എങ്കിൽ (cctx == ശൂന്യ || cctx->ctx_ufunc == ശൂന്യ
	    || cctx->ctx_ufunc->uf_class == ശൂന്യ
	    || cctx->ctx_ufunc->uf_defclass == ശൂന്യ)
	തിരികെ -1;

    // Search for the class method or variable in the class where the calling
    // function is defined.
    class_T *cl = cctx->ctx_ufunc->uf_defclass;
    പൂർണ്ണസംഖ്യ m_idx = is_method ? class_method_idx(cl, name, len)
					: class_member_idx(cl, name, len);
    എങ്കിൽ (m_idx < 0)
    {
	cl = cl->class_extends;
	ആകുമ്പോൾ (cl != ശൂന്യ)
	{
	    m_idx = is_method ? class_method_idx(cl, name, len)
					: class_member_idx(cl, name, len);
	    എങ്കിൽ (m_idx >= 0)
		നിർത്തുക;
	    cl = cl->class_extends;
	}
    }

    എങ്കിൽ (m_idx >= 0)
    {
	എങ്കിൽ (cl_ret != ശൂന്യ)
	    *cl_ret = cl;
    }

    തിരികെ m_idx;
}

/*
 * Returns the index of a class method with name "name" accessible in the
 * currently compiled function.  Returns -1 if not found.  The class where the
 * method is defined is returned in "cl_ret".
 */
    പൂർണ്ണസംഖ്യ
cctx_class_method_idx(
    cctx_T  *cctx,
    char_u  *name,
    size_t  len,
    class_T **cl_ret)
{
    തിരികെ cctx_class_midx(cctx, TRUE, name, len, cl_ret);
}

/*
 * Returns the index of a class variable with name "name" accessible in the
 * currently compiled function.  Returns -1 if not found.  The class where the
 * variable is defined is returned in "cl_ret".
 */
    പൂർണ്ണസംഖ്യ
cctx_class_member_idx(
    cctx_T  *cctx,
    char_u  *name,
    size_t  len,
    class_T **cl_ret)
{
    തിരികെ cctx_class_midx(cctx, FALSE, name, len, cl_ret);
}

/*
 * Return TRUE if "name" is a local variable, argument, script variable or
 * imported.  Also if "name" is "this" and in a class method.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
variable_exists(char_u *name, size_t len, cctx_T *cctx)
{
    തിരികെ (cctx != ശൂന്യ
		&& (lookup_local(name, len, ശൂന്യ, cctx) == OK
		    || arg_exists(name, len, ശൂന്യ, ശൂന്യ, ശൂന്യ, cctx) == OK
		    || (len == 4
			&& cctx->ctx_ufunc != ശൂന്യ
			&& (cctx->ctx_ufunc->uf_flags & (FC_OBJECT|FC_NEW))
			&& STRNCMP(name, "this", 4) == 0)))
	    || script_var_exists(name, len, cctx, ശൂന്യ) == OK
	    || cctx_class_member_idx(cctx, name, len, ശൂന്യ) >= 0
	    || find_imported(name, len, FALSE) != ശൂന്യ;
}

/*
 * Return TRUE if "name" is a local variable, argument, script variable,
 * imported or function.  Or commands are being skipped, a declaration may have
 * been skipped then.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
item_exists(char_u *name, size_t len, പൂർണ്ണസംഖ്യ cmd UNUSED, cctx_T *cctx)
{
    തിരികെ variable_exists(name, len, cctx);
}

/*
 * Check if "p[len]" is already defined, either in script "import_sid" or in
 * compilation context "cctx".
 * "cctx" is NULL at the script level, "cstack" is NULL in a function.
 * Does not check the global namespace.
 * If "is_arg" is TRUE the error message is for an argument name.
 * Return FAIL and give an error if it defined.
 */
    പൂർണ്ണസംഖ്യ
check_defined(
	char_u	    *p,
	size_t	    len,
	cctx_T	    *cctx,
	cstack_T    *cstack,
	പൂർണ്ണസംഖ്യ	    is_arg)
{
    പൂർണ്ണസംഖ്യ		c = p[len];
    ufunc_T	*ufunc = ശൂന്യ;

    // underscore argument is OK
    എങ്കിൽ (len == 1 && *p == '_')
	തിരികെ OK;

    എങ്കിൽ (script_var_exists(p, len, cctx, cstack) == OK)
    {
	എങ്കിൽ (is_arg)
	    semsg(_(e_argument_already_declared_in_script_str), p);
	അല്ലെങ്കിൽ
	    semsg(_(e_variable_already_declared_in_script_str), p);
	തിരികെ FAIL;
    }

    എങ്കിൽ (cctx_class_member_idx(cctx, p, len, ശൂന്യ) >= 0)
    {
	എങ്കിൽ (is_arg)
	    semsg(_(e_argument_already_declared_in_class_str), p);
	അല്ലെങ്കിൽ
	    semsg(_(e_variable_already_declared_in_class_str), p);
	തിരികെ FAIL;
    }

    p[len] = NUL;
    എങ്കിൽ ((cctx != ശൂന്യ
		&& (lookup_local(p, len, ശൂന്യ, cctx) == OK
		    || arg_exists(p, len, ശൂന്യ, ശൂന്യ, ശൂന്യ, cctx) == OK))
	    || find_imported(p, len, FALSE) != ശൂന്യ
	    || (ufunc = find_func_even_dead(p, 0)) != ശൂന്യ)
    {
	// A local or script-local function can shadow a global function.
	എങ്കിൽ (ufunc == ശൂന്യ || ((ufunc->uf_flags & FC_DEAD) == 0
		    && (!func_is_global(ufunc)
					     || (p[0] == 'g' && p[1] == ':'))))
	{
	    എങ്കിൽ (is_arg)
		semsg(_(e_argument_name_shadows_existing_variable_str), p);
	    അല്ലെങ്കിൽ
		semsg(_(e_name_already_defined_str), p);
	    p[len] = c;
	    തിരികെ FAIL;
	}
    }
    p[len] = c;
    തിരികെ OK;
}


/*
 * Return TRUE if "actual" could be "expected" and a runtime typecheck is to be
 * used.  Return FALSE if the types will never match.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
use_typecheck(type_T *actual, type_T *expected)
{
    എങ്കിൽ (actual->tt_type == VAR_ANY
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
	തിരികെ TRUE;
    എങ്കിൽ (actual->tt_type == VAR_OBJECT && expected->tt_type == VAR_OBJECT)
	തിരികെ TRUE;
    എങ്കിൽ ((actual->tt_type == VAR_LIST
		|| actual->tt_type == VAR_TUPLE
		|| actual->tt_type == VAR_DICT)
	    && actual->tt_type == expected->tt_type)
	// This takes care of a nested list or dict.
	തിരികെ use_typecheck(actual->tt_member, expected->tt_member);
    തിരികെ FALSE;
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
    പൂർണ്ണസംഖ്യ
need_type_where(
	type_T	*actual,
	type_T	*expected,
	പൂർണ്ണസംഖ്യ	typechk_flags,	// acceptable types (type check flags)
	പൂർണ്ണസംഖ്യ	offset,
	where_T	where,
	cctx_T	*cctx,
	പൂർണ്ണസംഖ്യ	silent,
	പൂർണ്ണസംഖ്യ	actual_is_const)
{
    പൂർണ്ണസംഖ്യ ret;

    എങ്കിൽ (expected->tt_type != VAR_CLASS && expected->tt_type != VAR_TYPEALIAS)
    {
	എങ്കിൽ (check_type_is_മൂല്യം(actual) == FAIL)
	    തിരികെ FAIL;
    }

    എങ്കിൽ (expected == &t_bool && actual != &t_bool
					&& (actual->tt_flags & TTFLAG_BOOL_OK))
    {
	// Using "0", "1" or the result of an expression with "&&" or "||" as a
	// boolean is OK but requires a conversion.
	generate_2BOOL(cctx, FALSE, offset);
	തിരികെ OK;
    }

    ret = check_type_maybe(expected, actual, FALSE, where);
    എങ്കിൽ (ret == OK)
	തിരികെ OK;

    // If actual a constant a runtime check makes no sense.  If it's
    // null_function it is OK.
    എങ്കിൽ (actual_is_const && ret == MAYBE && actual == &t_func_unknown)
	തിരികെ OK;

    // If the actual type can be the expected type add a runtime check.
    എങ്കിൽ (!actual_is_const && ret == MAYBE && use_typecheck(actual, expected))
    {
	generate_TYPECHECK(cctx, expected, typechk_flags, offset,
		where.wt_kind == WT_VARIABLE, where.wt_index);
	തിരികെ OK;
    }

    എങ്കിൽ (!silent)
	type_mismatch_where(expected, actual, where);
    തിരികെ FAIL;
}

    പൂർണ്ണസംഖ്യ
need_type(
	type_T	*actual,
	type_T	*expected,
	പൂർണ്ണസംഖ്യ	typechk_flags,	// acceptable types (type check flags)
	പൂർണ്ണസംഖ്യ	offset,
	പൂർണ്ണസംഖ്യ	arg_idx,
	cctx_T	*cctx,
	പൂർണ്ണസംഖ്യ	silent,
	പൂർണ്ണസംഖ്യ	actual_is_const)
{
    where_T where = WHERE_INIT;

    എങ്കിൽ (arg_idx > 0)
    {
	where.wt_index = arg_idx;
	where.wt_kind = WT_ARGUMENT;
    }
    തിരികെ need_type_where(actual, expected, typechk_flags, offset, where,
						cctx, silent, actual_is_const);
}

/*
 * Set type of variable "lvar" to "type".  If the variable is a constant then
 * the type gets TTFLAG_CONST.
 */
    സ്ഥിതം ശൂന്യം
set_var_type(lvar_T *lvar, type_T *type_arg, cctx_T *cctx)
{
    type_T	*type = type_arg;

    എങ്കിൽ (lvar->lv_const == ASSIGN_CONST && (type->tt_flags & TTFLAG_CONST) == 0)
    {
	എങ്കിൽ (type->tt_flags & TTFLAG_STATIC)
	    // entry in static_types[] is followed by const type
	    type = type + 1;
	അല്ലെങ്കിൽ
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
	പൂർണ്ണസംഖ്യ	assign,
	type_T	*type)
{
    lvar_T  *lvar;
    dfunc_T *dfunc;

    എങ്കിൽ (arg_exists(name, len, ശൂന്യ, ശൂന്യ, ശൂന്യ, cctx) == OK)
    {
	emsg_namelen(_(e_str_is_used_as_argument), name, (പൂർണ്ണസംഖ്യ)len);
	തിരികെ ശൂന്യ;
    }

    എങ്കിൽ (GA_GROW_FAILS(&cctx->ctx_locals, 1))
	തിരികെ ശൂന്യ;
    lvar = ((lvar_T *)cctx->ctx_locals.ga_data) + cctx->ctx_locals.ga_len++;
    CLEAR_POINTER(lvar);

    // Every local variable uses the next entry on the stack.  We could re-use
    // the last ones when leaving a scope, but then variables used in a closure
    // might get overwritten.  To keep things simple do not re-use stack
    // entries.  This is less efficient, but memory is cheap these days.
    dfunc = ((dfunc_T *)def_functions.ga_data) + cctx->ctx_ufunc->uf_dfunc_idx;
    lvar->lv_idx = dfunc->df_var_names.ga_len;

    lvar->lv_name = vim_strnsave(name, len == 0 ? STRLEN(name) : len);
    lvar->lv_const = assign;
    എങ്കിൽ (type == &t_unknown || type == &t_any)
	// type not known yet, may be inferred from RHS
	lvar->lv_type = type;
    അല്ലെങ്കിൽ
	// may use TTFLAG_CONST
	set_var_type(lvar, type, cctx);

    // Remember the name for debugging.
    എങ്കിൽ (GA_GROW_FAILS(&dfunc->df_var_names, 1))
	തിരികെ ശൂന്യ;
    ((char_u **)dfunc->df_var_names.ga_data)[lvar->lv_idx] =
						    vim_strsave(lvar->lv_name);
    ++dfunc->df_var_names.ga_len;

    തിരികെ lvar;
}

/*
 * If "check_writable" is ASSIGN_CONST give an error if the variable was
 * defined with :final or :const, if "check_writable" is ASSIGN_FINAL give an
 * error if the variable was defined with :const.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
check_item_writable(svar_T *sv, പൂർണ്ണസംഖ്യ check_writable, char_u *name)
{
    എങ്കിൽ ((check_writable == ASSIGN_CONST && sv->sv_const != 0)
	    || (check_writable == ASSIGN_FINAL
					      && sv->sv_const == ASSIGN_CONST))
    {
	semsg(_(e_cannot_change_readonly_variable_str), name);
	തിരികെ FAIL;
    }
    തിരികെ OK;
}

/*
 * Find "name" in script-local items of script "sid".
 * Pass "check_writable" to check_item_writable().
 * "cctx" is NULL at the script level, "cstack" is NULL in a function.
 * Returns the index in "sn_var_vals" if found.
 * If found but not in "sn_var_vals" returns -1.
 * If not found or the variable is not writable returns -2.
 */
    പൂർണ്ണസംഖ്യ
get_script_item_idx(
	പൂർണ്ണസംഖ്യ	    sid,
	char_u	    *name,
	പൂർണ്ണസംഖ്യ	    check_writable,
	cctx_T	    *cctx,
	cstack_T    *cstack)
{
    hashtab_T	    *ht;
    dictitem_T	    *di;
    scriptitem_T    *si = SCRIPT_ITEM(sid);
    svar_T	    *sv;
    പൂർണ്ണസംഖ്യ		    idx;

    എങ്കിൽ (!SCRIPT_ID_VALID(sid))
	തിരികെ -1;
    എങ്കിൽ (sid == current_sctx.sc_sid)
    {
	sallvar_T *sav = find_script_var(name, 0, cctx, cstack);

	എങ്കിൽ (sav == ശൂന്യ)
	    തിരികെ -2;
	idx = sav->sav_var_vals_idx;
	sv = ((svar_T *)si->sn_var_vals.ga_data) + idx;
	എങ്കിൽ (check_item_writable(sv, check_writable, name) == FAIL)
	    തിരികെ -2;
	തിരികെ idx;
    }

    // First look the name up in the hashtable.
    ht = &SCRIPT_VARS(sid);
    di = find_var_in_ht(ht, 0, name, TRUE);
    എങ്കിൽ (di == ശൂന്യ)
    {
	എങ്കിൽ (si->sn_autoload_prefix != ശൂന്യ)
	{
	    hashitem_T *hi;

	    // A variable exported from an autoload script is in the global
	    // variables, we can find it in the all_vars table.
	    hi = hash_find(&si->sn_all_vars.dv_hashtab, name);
	    എങ്കിൽ (!HASHITEM_EMPTY(hi))
		തിരികെ HI2SAV(hi)->sav_var_vals_idx;
	}
	തിരികെ -2;
    }

    // Now find the svar_T index in sn_var_vals.
    വേണ്ടി (idx = 0; idx < si->sn_var_vals.ga_len; ++idx)
    {
	sv = ((svar_T *)si->sn_var_vals.ga_data) + idx;
	എങ്കിൽ (sv->sv_tv == &di->di_tv)
	{
	    എങ്കിൽ (check_item_writable(sv, check_writable, name) == FAIL)
		തിരികെ -2;
	    തിരികെ idx;
	}
    }
    തിരികെ -1;
}

    സ്ഥിതം imported_T *
find_imported_in_script(char_u *name, size_t len, പൂർണ്ണസംഖ്യ sid)
{
    scriptitem_T    *si;
    പൂർണ്ണസംഖ്യ		    idx;

    എങ്കിൽ (!SCRIPT_ID_VALID(sid))
	തിരികെ ശൂന്യ;
    si = SCRIPT_ITEM(sid);
    വേണ്ടി (idx = 0; idx < si->sn_imports.ga_len; ++idx)
    {
	imported_T *import = ((imported_T *)si->sn_imports.ga_data) + idx;

	എങ്കിൽ (len == 0 ? STRCMP(name, import->imp_name) == 0
		     : STRLEN(import->imp_name) == len
				  && STRNCMP(name, import->imp_name, len) == 0)
	    തിരികെ import;
    }
    തിരികെ ശൂന്യ;
}

/*
 * Find "name" in imported items of the current script.
 * If "len" is 0 use any length that works.
 * If "load" is TRUE and the script was not loaded yet, load it now.
 */
    imported_T *
find_imported(char_u *name, size_t len, പൂർണ്ണസംഖ്യ load)
{
    എങ്കിൽ (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	തിരികെ ശൂന്യ;

    // Skip over "s:" before "s:something" to find the import name.
    പൂർണ്ണസംഖ്യ off = name[0] == 's' && name[1] == ':' ? 2 : 0;

    imported_T *ret = find_imported_in_script(name + off, len - off,
							  current_sctx.sc_sid);
    എങ്കിൽ (ret != ശൂന്യ && load && (ret->imp_flags & IMP_FLAGS_AUTOLOAD))
    {
	scid_T	actual_sid = 0;
	പൂർണ്ണസംഖ്യ	save_emsg_off = emsg_off;

	// "emsg_off" will be set when evaluating an expression silently, but
	// we do want to know about errors in a script.  Also because it then
	// aborts when an error is encountered.
	emsg_off = FALSE;

	// script found before but not loaded yet
	ret->imp_flags &= ~IMP_FLAGS_AUTOLOAD;
	(ശൂന്യം)do_source(SCRIPT_ITEM(ret->imp_sid)->sn_name, FALSE,
						       DOSO_NONE, &actual_sid);
	// If the script is a symlink it may be sourced with another name, may
	// need to adjust the script ID for that.
	എങ്കിൽ (actual_sid != 0)
	    ret->imp_sid = actual_sid;

	emsg_off = save_emsg_off;
    }
    തിരികെ ret;
}

/*
 * Called when checking for a following operator at "arg".  When the rest of
 * the line is empty or only a comment, peek the next line.  If there is a next
 * line return a pointer to it and set "nextp".
 * Otherwise skip over white space.
 */
    char_u *
may_peek_next_വരി(cctx_T *cctx, char_u *arg, char_u **nextp)
{
    char_u *p = skipwhite(arg);

    *nextp = ശൂന്യ;
    എങ്കിൽ (*p == NUL || (VIM_ISWHITE(*arg) && vim9_comment_start(p)))
    {
	*nextp = peek_next_വരി_from_context(cctx);
	എങ്കിൽ (*nextp != ശൂന്യ)
	    തിരികെ *nextp;
    }
    തിരികെ p;
}

/*
 * Return a pointer to the next line that isn't empty or only contains a
 * comment. Skips over white space.
 * Returns NULL if there is none.
 */
    char_u *
peek_next_വരി_from_context(cctx_T *cctx)
{
    പൂർണ്ണസംഖ്യ lnum = cctx->ctx_lnum;

    ആകുമ്പോൾ (++lnum < cctx->ctx_ufunc->uf_വരികൾ.ga_len)
    {
	char_u *വരി = ((char_u **)cctx->ctx_ufunc->uf_വരികൾ.ga_data)[lnum];
	char_u *p;

	// ignore NULLs inserted for continuation lines
	എങ്കിൽ (വരി != ശൂന്യ)
	{
	    p = skipwhite(വരി);
	    എങ്കിൽ (vim9_bad_comment(p))
		തിരികെ ശൂന്യ;
	    എങ്കിൽ (*p != NUL && !vim9_comment_start(p))
		തിരികെ p;
	}
    }
    തിരികെ ശൂന്യ;
}

/*
 * Get the next line of the function from "cctx".
 * Skips over empty lines.  Skips over comment lines if "skip_comment" is TRUE.
 * Returns NULL when at the end.
 */
    char_u *
next_വരി_from_context(cctx_T *cctx, പൂർണ്ണസംഖ്യ skip_comment)
{
    char_u	*വരി;

    ചെയ്യുക
    {
	++cctx->ctx_lnum;
	എങ്കിൽ (cctx->ctx_lnum >= cctx->ctx_ufunc->uf_വരികൾ.ga_len)
	{
	    വരി = ശൂന്യ;
	    നിർത്തുക;
	}
	വരി = ((char_u **)cctx->ctx_ufunc->uf_വരികൾ.ga_data)[cctx->ctx_lnum];
	cctx->ctx_വരി_start = വരി;
	SOURCING_LNUM = cctx->ctx_lnum + 1;
    } ആകുമ്പോൾ (വരി == ശൂന്യ || *skipwhite(വരി) == NUL
		     || (skip_comment && vim9_comment_start(skipwhite(വരി))));
    തിരികെ വരി;
}

/*
 * Skip over white space at "whitep" and assign to "*arg".
 * If "*arg" is at the end of the line, advance to the next line.
 * Also when "whitep" points to white space and "*arg" is on a "#".
 * Return FAIL if beyond the last line, "*arg" is unmodified then.
 */
    പൂർണ്ണസംഖ്യ
may_get_next_വരി(char_u *whitep, char_u **arg, cctx_T *cctx)
{
    *arg = skipwhite(whitep);
    എങ്കിൽ (vim9_bad_comment(*arg))
	തിരികെ FAIL;
    എങ്കിൽ (**arg == NUL || (VIM_ISWHITE(*whitep) && vim9_comment_start(*arg)))
    {
	char_u *next = next_വരി_from_context(cctx, TRUE);

	എങ്കിൽ (next == ശൂന്യ)
	    തിരികെ FAIL;
	*arg = skipwhite(next);
    }
    തിരികെ OK;
}

/*
 * Idem, and give an error when failed.
 */
    പൂർണ്ണസംഖ്യ
may_get_next_വരി_error(char_u *whitep, char_u **arg, cctx_T *cctx)
{
    എങ്കിൽ (may_get_next_വരി(whitep, arg, cctx) == FAIL)
    {
	SOURCING_LNUM = cctx->ctx_lnum + 1;
	emsg(_(e_വരി_incomplete));
	തിരികെ FAIL;
    }
    തിരികെ OK;
}

/*
 * Get a line from the compilation context, compatible with exarg_T getline().
 * Return a pointer to the line in allocated memory.
 * Return NULL for end-of-file or some error.
 */
    സ്ഥിതം char_u *
exarg_getline(
	പൂർണ്ണസംഖ്യ c UNUSED,
	ശൂന്യം *cookie,
	പൂർണ്ണസംഖ്യ indent UNUSED,
	getline_opt_T options UNUSED)
{
    cctx_T  *cctx = (cctx_T *)cookie;
    char_u  *p;

    വേണ്ടി (;;)
    {
	എങ്കിൽ (cctx->ctx_lnum >= cctx->ctx_ufunc->uf_വരികൾ.ga_len - 1)
	    തിരികെ ശൂന്യ;
	++cctx->ctx_lnum;
	p = ((char_u **)cctx->ctx_ufunc->uf_വരികൾ.ga_data)[cctx->ctx_lnum];
	// Comment lines result in NULL pointers, skip them.
	എങ്കിൽ (p != ശൂന്യ)
	    തിരികെ vim_strsave(p);
    }
}

    ശൂന്യം
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
    പൂർണ്ണസംഖ്യ
func_needs_compiling(ufunc_T *ufunc, compiletype_T compile_type)
{
    തിരഞ്ഞെടുപ്പ് (ufunc->uf_def_status)
    {
	അവസ്ഥ UF_TO_BE_COMPILED:
	    തിരികെ TRUE;

	അവസ്ഥ UF_COMPILED:
	{
	    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data)
							 + ufunc->uf_dfunc_idx;

	    തിരഞ്ഞെടുപ്പ് (compile_type)
	    {
		അവസ്ഥ CT_PROFILE:
#ifdef FEAT_PROFILE
		    തിരികെ dfunc->df_instr_prof == ശൂന്യ;
#endif
		അവസ്ഥ CT_NONE:
		    തിരികെ dfunc->df_instr == ശൂന്യ;
		അവസ്ഥ CT_DEBUG:
		    തിരികെ dfunc->df_instr_debug == ശൂന്യ;
	    }
	}

	അവസ്ഥ UF_NOT_COMPILED:
	അവസ്ഥ UF_COMPILE_ERROR:
	അവസ്ഥ UF_COMPILING:
	    നിർത്തുക;
    }
    തിരികെ FALSE;
}

/*
 * Compile a nested :def command.
 */
    സ്ഥിതം char_u *
compile_nested_function(exarg_T *eap, cctx_T *cctx, garray_T *വരികൾ_to_free)
{
    പൂർണ്ണസംഖ്യ		is_global = *eap->arg == 'g' && eap->arg[1] == ':';
    char_u	*name_start = eap->arg;
    char_u	*name_end = to_name_end(eap->arg, TRUE);
    പൂർണ്ണസംഖ്യ		off;
    char_u	*func_name;
    string_T	lambda_name;
    ufunc_T	*ufunc;
    പൂർണ്ണസംഖ്യ		r = FAIL;
    compiletype_T   compile_type;
    പൂർണ്ണസംഖ്യ		funcref_isn_idx = -1;
    lvar_T	*lvar = ശൂന്യ;
    char_u	*bracket_start = ശൂന്യ;

    എങ്കിൽ (eap->forceit)
    {
	semsg(_(e_cannot_use_bang_with_nested_def_str),
		eap->cmdidx == CMD_def ? ":def" : ":function");
	തിരികെ ശൂന്യ;
    }

    എങ്കിൽ (*name_start == '/')
    {
	name_end = skip_regexp(name_start + 1, '/', TRUE);
	എങ്കിൽ (*name_end == '/')
	    ++name_end;
	set_nextcmd(eap, name_end);
    }

    എങ്കിൽ (*name_end == '<')
    {
	bracket_start = name_end;
	എങ്കിൽ (skip_generic_func_type_args(&name_end) == FAIL)
	    തിരികെ ശൂന്യ;
    }

    എങ്കിൽ (name_end == name_start || *skipwhite(name_end) != '(')
    {
	എങ്കിൽ (!ends_excmd2(name_start, name_end))
	{
	    എങ്കിൽ (*skipwhite(name_end) == '.')
		semsg(_(e_cannot_define_dict_func_in_vim9_script_str),
								     eap->cmd);
	    അല്ലെങ്കിൽ
		semsg(_(e_invalid_command_str), eap->cmd);
	    തിരികെ ശൂന്യ;
	}

	// "def" or "def Name": list functions
	എങ്കിൽ (generate_DEF(cctx, name_start, name_end - name_start) == FAIL)
	    തിരികെ ശൂന്യ;
	തിരികെ eap->nextcmd == ശൂന്യ ? (char_u *)"" : eap->nextcmd;
    }

    എങ്കിൽ (bracket_start != ശൂന്യ)
	// generic function.  The function name ends before the list of types
	// (opening angle bracket).
	name_end = bracket_start;

    // Only g:Func() can use a namespace.
    എങ്കിൽ (name_start[1] == ':' && !is_global)
    {
	semsg(_(e_namespace_not_supported_str), name_start);
	തിരികെ ശൂന്യ;
    }
    എങ്കിൽ (cctx->ctx_skip != SKIP_YES
	    && check_defined(name_start, name_end - name_start, cctx,
							  ശൂന്യ, FALSE) == FAIL)
	തിരികെ ശൂന്യ;
    എങ്കിൽ (!ASCII_ISUPPER(is_global ? name_start[2] : name_start[0]))
    {
	semsg(_(e_function_name_must_start_with_capital_str), name_start);
	തിരികെ ശൂന്യ;
    }

    eap->arg = name_end;
    fill_exarg_from_cctx(eap, cctx);

    eap->forceit = FALSE;
    // We use the special <lambda>99 name, but it's not really a lambda.
    lambda_name = get_lambda_name();
    lambda_name.string = vim_strnsave(lambda_name.string, lambda_name.length);
    എങ്കിൽ (lambda_name.string == ശൂന്യ)
	തിരികെ ശൂന്യ;

    // This may free the current line, make a copy of the name.
    off = is_global ? 2 : 0;
    func_name = vim_strnsave(name_start + off, name_end - name_start - off);
    എങ്കിൽ (func_name == ശൂന്യ)
    {
	r = FAIL;
	പോകുക theend;
    }

    // A type variable cannot have the name of the function.  The function is
    // defined with the "<lambda>" name, check it here.
    എങ്കിൽ (bracket_start != ശൂന്യ)
    {
	size_t	len = STRLEN(func_name);

	വേണ്ടി (char_u *t = bracket_start + 1; *t != '>' && *t != NUL; )
	{
	    char_u *te = to_name_end(t, FALSE);

	    എങ്കിൽ ((size_t)(te - t) == len && STRNCMP(t, func_name, len) == 0)
	    {
		semsg(_(e_redefining_script_item_str), func_name);
		r = FAIL;
		പോകുക theend;
	    }
	    t = te == t ? t + 1 : te;
	}
    }

    // Make sure "KeyTyped" is not set, it may cause indent to be written.
    പൂർണ്ണസംഖ്യ save_താക്കോൽ_Typed = താക്കോൽ_Typed;
    താക്കോൽ_Typed = FALSE;

    ufunc = define_function(eap, lambda_name.string, വരികൾ_to_free, 0, ശൂന്യ, 0,
									cctx);

    താക്കോൽ_Typed = save_താക്കോൽ_Typed;

    എങ്കിൽ (ufunc == ശൂന്യ)
    {
	r = eap->skip ? OK : FAIL;
	പോകുക theend;
    }
    എങ്കിൽ (eap->nextcmd != ശൂന്യ)
    {
	semsg(_(e_text_found_after_str_str),
	      eap->cmdidx == CMD_def ? "enddef" : "endfunction", eap->nextcmd);
	r = FAIL;
	func_ptr_unref(ufunc);
	പോകുക theend;
    }

    // copy over the block scope IDs before compiling
    എങ്കിൽ (!is_global && cctx->ctx_ufunc->uf_block_depth > 0)
    {
	പൂർണ്ണസംഖ്യ block_depth = cctx->ctx_ufunc->uf_block_depth;

	ufunc->uf_block_ids = ALLOC_MULT(പൂർണ്ണസംഖ്യ, block_depth);
	എങ്കിൽ (ufunc->uf_block_ids != ശൂന്യ)
	{
	    mch_memmove(ufunc->uf_block_ids, cctx->ctx_ufunc->uf_block_ids,
						    വലുപ്പം(പൂർണ്ണസംഖ്യ) * block_depth);
	    ufunc->uf_block_depth = block_depth;
	}
    }

    // Define the funcref before compiling, so that it is found by any
    // recursive call.
    എങ്കിൽ (is_global)
    {
	r = generate_NEWFUNC(cctx, lambda_name.string, func_name);
	func_name = ശൂന്യ;
	lambda_name.string = ശൂന്യ;
	lambda_name.length = 0;
    }
    അല്ലെങ്കിൽ
    {
	// Define a local variable for the function reference.
	lvar = reserve_local(cctx, func_name, name_end - name_start,
					    ASSIGN_CONST, ufunc->uf_func_type);
	എങ്കിൽ (lvar == ശൂന്യ)
	{
	    func_ptr_unref(ufunc);
	    പോകുക theend;
	}
	lvar->lv_ufunc = ufunc;
	എങ്കിൽ (generate_FUNCREF(cctx, ufunc, ശൂന്യ, FALSE, 0, &funcref_isn_idx) == FAIL)
	{
	    func_ptr_unref(ufunc);
	    പോകുക theend;
	}
	r = generate_STORE(cctx, ISN_STORE, lvar->lv_idx, ശൂന്യ);
    }

    compile_type = get_compile_type(ufunc);
#ifdef FEAT_PROFILE
    // If the outer function is profiled, also compile the nested function for
    // profiling.
    എങ്കിൽ (cctx->ctx_compile_type == CT_PROFILE)
	compile_type = CT_PROFILE;
#endif
    എങ്കിൽ (func_needs_compiling(ufunc, compile_type)
	    && compile_def_function(ufunc, TRUE, compile_type, cctx) == FAIL)
    {
	func_ptr_unref(ufunc);
	എങ്കിൽ (lvar != ശൂന്യ)
	    // Now the local variable can't be used.
	    *lvar->lv_name = '/';  // impossible value
	പോകുക theend;
    }

#ifdef FEAT_PROFILE
    // When the outer function is compiled for profiling, the nested function
    // may be called without profiling.  Compile it here in the right context.
    എങ്കിൽ (compile_type == CT_PROFILE && func_needs_compiling(ufunc, CT_NONE))
	compile_def_function(ufunc, FALSE, CT_NONE, cctx);
#endif

    // If a FUNCREF instruction was generated, set the index after compiling.
    എങ്കിൽ (funcref_isn_idx != -1 && ufunc->uf_def_status == UF_COMPILED)
    {
	isn_T	*funcref_isn = ((isn_T *)cctx->ctx_instr.ga_data) +
							funcref_isn_idx;
	funcref_isn->isn_arg.funcref.fr_dfunc_idx = ufunc->uf_dfunc_idx;
    }

theend:
    vim_free(lambda_name.string);
    vim_free(func_name);
    തിരികെ r == FAIL ? ശൂന്യ : (char_u *)"";
}

/*
 * Compile one Vim expression {expr} in string "p".
 * "p" points to the opening "{".
 * Return a pointer to the character after "}", NULL for an error.
 */
    char_u *
compile_one_വ്യഞ്ജകം_in_str(char_u *p, cctx_T *cctx)
{
    char_u	*block_start;
    char_u	*block_end;

    // Skip the opening {.
    block_start = skipwhite(p + 1);
    block_end = block_start;
    എങ്കിൽ (*block_start != NUL && skip_വ്യഞ്ജകം(&block_end, ശൂന്യ) == FAIL)
	തിരികെ ശൂന്യ;
    block_end = skipwhite(block_end);
    // The block must be closed by a }.
    എങ്കിൽ (*block_end != '}')
    {
	semsg(_(e_missing_close_curly_str), p);
	തിരികെ ശൂന്യ;
    }
    എങ്കിൽ (compile_expr0(&block_start, cctx) == FAIL)
	തിരികെ ശൂന്യ;
    may_generate_2STRING(-1, TOSTRING_INTERPOLATE, cctx);

    തിരികെ block_end + 1;
}

/*
 * Compile a string "str" (either containing a literal string or a mix of
 * literal strings and Vim expressions of the form `{expr}`).  This is used
 * when compiling a heredoc assignment to a variable or an interpolated string
 * in a Vim9 def function.  Vim9 instructions are generated to push strings,
 * evaluate expressions, concatenate them and create a list of lines.  When
 * "evalstr" is TRUE, Vim expressions in "str" are evaluated.
 */
    പൂർണ്ണസംഖ്യ
compile_all_വ്യഞ്ജകം_in_str(char_u *str, പൂർണ്ണസംഖ്യ evalstr, cctx_T *cctx)
{
    char_u	*p = str;
    char_u	*val;
    പൂർണ്ണസംഖ്യ		എണ്ണം = 0;

    എങ്കിൽ (cctx->ctx_skip == SKIP_YES)
	തിരികെ OK;

    എങ്കിൽ (!evalstr || *str == NUL)
    {
	// Literal string, possibly empty.
	val = *str != NUL ? vim_strsave(str) : ശൂന്യ;
	തിരികെ generate_PUSHS(cctx, &val);
    }

    // Push all the string pieces to the stack, followed by a ISN_CONCAT.
    ആകുമ്പോൾ (*p != NUL)
    {
	char_u	*lit_start;
	പൂർണ്ണസംഖ്യ	escaped_brace = FALSE;

	// Look for a block start.
	lit_start = p;
	ആകുമ്പോൾ (*p != '{' && *p != '}' && *p != NUL)
	    ++p;

	എങ്കിൽ (*p != NUL && *p == p[1])
	{
	    // Escaped brace, unescape and continue.
	    // Include the brace in the literal string.
	    ++p;
	    escaped_brace = TRUE;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (*p == '}')
	{
	    semsg(_(e_stray_closing_curly_str), str);
	    തിരികെ FAIL;
	}

	// Append the literal part.
	എങ്കിൽ (p != lit_start)
	{
	    val = vim_strnsave(lit_start, (size_t)(p - lit_start));
	    എങ്കിൽ (generate_PUSHS(cctx, &val) == FAIL)
		തിരികെ FAIL;
	    ++എണ്ണം;
	}

	എങ്കിൽ (*p == NUL)
	    നിർത്തുക;

	എങ്കിൽ (escaped_brace)
	{
	    // Skip the second brace.
	    ++p;
	    തുടരുക;
	}

	p = compile_one_വ്യഞ്ജകം_in_str(p, cctx);
	എങ്കിൽ (p == ശൂന്യ)
	    തിരികെ FAIL;
	++എണ്ണം;
    }

    // Small optimization, if there's only a single piece skip the ISN_CONCAT.
    എങ്കിൽ (എണ്ണം > 1)
	തിരികെ generate_CONCAT(cctx, എണ്ണം);

    തിരികെ OK;
}

/*
 * Return the length of an assignment operator, or zero if there isn't one.
 */
    പൂർണ്ണസംഖ്യ
assignment_len(char_u *p, പൂർണ്ണസംഖ്യ *heredoc)
{
    എങ്കിൽ (*p == '=')
    {
	എങ്കിൽ (p[1] == '<' && p[2] == '<')
	{
	    *heredoc = TRUE;
	    തിരികെ 3;
	}
	തിരികെ 1;
    }
    എങ്കിൽ (vim_strchr((char_u *)"+-*/%", *p) != ശൂന്യ && p[1] == '=')
	തിരികെ 2;
    എങ്കിൽ (STRNCMP(p, "..=", 3) == 0)
	തിരികെ 3;
    തിരികെ 0;
}

/*
 * Generate the load instruction for "name".
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
generate_loadvar(cctx_T *cctx, lhs_T *lhs)
{
    char_u	*name = lhs->lhs_name;
    type_T	*type = lhs->lhs_type;
    പൂർണ്ണസംഖ്യ		res = OK;

    തിരഞ്ഞെടുപ്പ് (lhs->lhs_dest)
    {
	അവസ്ഥ dest_option:
	അവസ്ഥ dest_func_option:
	    generate_LOAD(cctx, ISN_LOADOPT, 0, name, type);
	    നിർത്തുക;
	അവസ്ഥ dest_global:
	    എങ്കിൽ (vim_strchr(name, AUTOLOAD_CHAR) == ശൂന്യ)
	    {
		എങ്കിൽ (name[2] == NUL)
		    generate_instr_type(cctx, ISN_LOADGDICT, &t_dict_any);
		അല്ലെങ്കിൽ
		    generate_LOAD(cctx, ISN_LOADG, 0, name + 2, type);
	    }
	    അല്ലെങ്കിൽ
		generate_LOAD(cctx, ISN_LOADAUTO, 0, name, type);
	    നിർത്തുക;
	അവസ്ഥ dest_buffer:
	    generate_LOAD(cctx, ISN_LOADB, 0, name + 2, type);
	    നിർത്തുക;
	അവസ്ഥ dest_window:
	    generate_LOAD(cctx, ISN_LOADW, 0, name + 2, type);
	    നിർത്തുക;
	അവസ്ഥ dest_tab:
	    generate_LOAD(cctx, ISN_LOADT, 0, name + 2, type);
	    നിർത്തുക;
	അവസ്ഥ dest_script:
	അവസ്ഥ dest_script_v9:
	    res = compile_load_scriptvar(cctx,
			    name + (name[1] == ':' ? 2 : 0), ശൂന്യ, ശൂന്യ, ശൂന്യ);
	    നിർത്തുക;
	അവസ്ഥ dest_env:
	    // Include $ in the name here
	    generate_LOAD(cctx, ISN_LOADENV, 0, name, type);
	    നിർത്തുക;
	അവസ്ഥ dest_reg:
	    generate_LOAD(cctx, ISN_LOADREG, name[1], ശൂന്യ, &t_string);
	    നിർത്തുക;
	അവസ്ഥ dest_vimvar:
	    generate_LOADV(cctx, name + 2);
	    നിർത്തുക;
	അവസ്ഥ dest_local:
	    എങ്കിൽ (cctx->ctx_skip != SKIP_YES)
	    {
		lvar_T	*lvar = lhs->lhs_lvar;
		എങ്കിൽ (lvar->lv_from_outer > 0)
		    generate_LOADOUTER(cctx, lvar->lv_idx, lvar->lv_from_outer,
				 lvar->lv_loop_depth, lvar->lv_loop_idx, type);
		അല്ലെങ്കിൽ
		    generate_LOAD(cctx, ISN_LOAD, lvar->lv_idx, ശൂന്യ, type);
	    }
	    നിർത്തുക;
	അവസ്ഥ dest_class_member:
	    generate_CLASSMEMBER(cctx, TRUE, lhs->lhs_class,
						     lhs->lhs_classmember_idx);
	    നിർത്തുക;
	അവസ്ഥ dest_വ്യഞ്ജകം:
	    // list or dict value should already be on the stack.
	    നിർത്തുക;
    }

    തിരികെ res;
}

/*
 * Skip over "[expr]" or ".member".
 * Does not check for any errors.
 */
    സ്ഥിതം char_u *
skip_index(char_u *start)
{
    char_u *p = start;

    എങ്കിൽ (*p == '[')
    {
	p = skipwhite(p + 1);
	(ശൂന്യം)skip_വ്യഞ്ജകം(&p, ശൂന്യ);
	p = skipwhite(p);
	എങ്കിൽ (*p == ']')
	    തിരികെ p + 1;
	തിരികെ p;
    }
    // if (*p == '.')
    തിരികെ to_name_end(p + 1, TRUE);
}

    ശൂന്യം
vim9_declare_error(char_u *name)
{
    അക്ഷരം *scope = "";

    തിരഞ്ഞെടുപ്പ് (*name)
    {
	അവസ്ഥ 'g': scope = _("global"); നിർത്തുക;
	അവസ്ഥ 'b': scope = _("buffer"); നിർത്തുക;
	അവസ്ഥ 'w': scope = _("window"); നിർത്തുക;
	അവസ്ഥ 't': scope = _("tab"); നിർത്തുക;
	അവസ്ഥ 'v': scope = "v:"; നിർത്തുക;
	അവസ്ഥ '$': semsg(_(e_cannot_declare_an_environment_variable_str), name);
		  തിരികെ;
	അവസ്ഥ '&': semsg(_(e_cannot_declare_an_option_str), name);
		  തിരികെ;
	അവസ്ഥ '@': semsg(_(e_cannot_declare_a_register_str), name);
		  തിരികെ;
	സ്ഥിരസ്ഥിതി: തിരികെ;
    }
    semsg(_(e_cannot_declare_a_scope_variable_str), scope, name);
}

/*
 * Return TRUE if "name" is a valid register to use.
 * Return FALSE and give an error message if not.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
valid_dest_reg(പൂർണ്ണസംഖ്യ name)
{
    എങ്കിൽ (name == '@')
       name = '"';
    എങ്കിൽ (name == '/' || name == '=' || name == '#' || valid_yank_reg(name, TRUE))
	തിരികെ TRUE;
    emsg_invreg(name);
    തിരികെ FAIL;
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
    പൂർണ്ണസംഖ്യ
get_var_dest(
	char_u		*name,
	assign_dest_T	*dest,
	cmdidx_T	cmdidx,
	പൂർണ്ണസംഖ്യ		has_index,
	പൂർണ്ണസംഖ്യ		*option_scope,
	പൂർണ്ണസംഖ്യ		*vimvaridx,
	type_T		**type,
	cctx_T		*cctx)
{
    char_u *p;

    എങ്കിൽ (*name == '&')
    {
	പൂർണ്ണസംഖ്യ		cc;
	long		numval;
	getoption_T	opt_type;
	പൂർണ്ണസംഖ്യ		opt_p_flags;

	*dest = dest_option;
	എങ്കിൽ (cmdidx == CMD_final || cmdidx == CMD_const)
	{
	    emsg(_(e_cannot_lock_option));
	    തിരികെ FAIL;
	}
	p = name;
	p = find_option_end(&p, option_scope);
	എങ്കിൽ (p == ശൂന്യ)
	{
	    // cannot happen?
	    emsg(_(e_unexpected_characters_in_assignment));
	    തിരികെ FAIL;
	}
	cc = *p;
	*p = NUL;
	opt_type = get_option_മൂല്യം(skip_option_env_lead(name),
				   &numval, ശൂന്യ, &opt_p_flags, *option_scope);
	*p = cc;
	തിരഞ്ഞെടുപ്പ് (opt_type)
	{
	    അവസ്ഥ gov_unknown:
		    semsg(_(e_unknown_option_str), name);
		    തിരികെ FAIL;
	    അവസ്ഥ gov_string:
	    അവസ്ഥ gov_hidden_string:
		    എങ്കിൽ (opt_p_flags & P_FUNC)
		    {
			// might be a Funcref, check the type later
			*type = &t_any;
			*dest = dest_func_option;
		    }
		    അല്ലെങ്കിൽ
		    {
			*type = &t_string;
		    }
		    നിർത്തുക;
	    അവസ്ഥ gov_bool:
	    അവസ്ഥ gov_hidden_bool:
		    *type = &t_bool;
		    നിർത്തുക;
	    അവസ്ഥ gov_സംഖ്യ:
	    അവസ്ഥ gov_hidden_സംഖ്യ:
		    *type = &t_സംഖ്യ;
		    നിർത്തുക;
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (*name == '$')
    {
	*dest = dest_env;
	*type = &t_string;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (*name == '@')
    {
	എങ്കിൽ (!valid_dest_reg(name[1]))
	    തിരികെ FAIL;
	*dest = dest_reg;
	*type = name[1] == '#' ? &t_സംഖ്യ_or_string : &t_string;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(name, "g:", 2) == 0)
    {
	*dest = dest_global;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(name, "b:", 2) == 0)
    {
	*dest = dest_buffer;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(name, "w:", 2) == 0)
    {
	*dest = dest_window;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(name, "t:", 2) == 0)
    {
	*dest = dest_tab;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(name, "v:", 2) == 0)
    {
	typval_T	*vtv;
	പൂർണ്ണസംഖ്യ		di_flags;

	*vimvaridx = find_vim_var(name + 2, &di_flags);
	എങ്കിൽ (*vimvaridx < 0)
	{
	    semsg(_(e_variable_not_found_str), name);
	    തിരികെ FAIL;
	}
	// An item of a read-only variable can be changed, as when not compiled.
	// We use the current value of "sandbox" here, is that OK?
	എങ്കിൽ (!has_index && var_check_ro(di_flags, name, FALSE))
	    തിരികെ FAIL;
	*dest = dest_vimvar;
	vtv = get_vim_var_tv(*vimvaridx);
	*type = typval2type_vimvar(vtv, cctx->ctx_type_list);
    }
    തിരികെ OK;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
is_decl_command(cmdidx_T cmdidx)
{
    തിരികെ cmdidx == CMD_let || cmdidx == CMD_var
				 || cmdidx == CMD_final || cmdidx == CMD_const;
}

/*
 * Returns TRUE if the class or object variable in "lhs" is modifiable.
 * "var_start" points to the start of the variable name and "lhs->lhs_varlen"
 * has the total length.  Note that the "lhs" can be nested an object reference
 * (e.g.  a.b.c.d.var).
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
lhs_class_member_modifiable(lhs_T *lhs, char_u	*var_start, cctx_T *cctx)
{
    size_t	varlen = lhs->lhs_varlen;
    class_T	*cl = lhs->lhs_type->tt_class;
    പൂർണ്ണസംഖ്യ		is_object = lhs->lhs_type->tt_type == VAR_OBJECT;
    char_u	*name = var_start + varlen + 1;
    size_t	namelen = lhs->lhs_end - var_start - varlen - 1;
    ocmember_T	*m;

    m = member_lookup(cl, lhs->lhs_type->tt_type, name, namelen, ശൂന്യ);
    എങ്കിൽ (m == ശൂന്യ)
    {
	member_not_found_സന്ദേശം(cl, lhs->lhs_type->tt_type, name, namelen);
	തിരികെ FALSE;
    }

    എങ്കിൽ (IS_ENUM(cl))
    {
	semsg(_(e_enumvalue_str_cannot_be_modified), cl->class_name.string,
		m->ocm_name.string);
	തിരികെ FALSE;
    }

    // If it is private member variable, then accessing it outside the
    // class is not allowed.
    // If it is a read only class variable, then it can be modified
    // only inside the class where it is defined.
    എങ്കിൽ ((m->ocm_access != VIM_ACCESS_ALL) &&
	    ((is_object && !inside_class(cctx, cl))
	     || (!is_object && cctx->ctx_ufunc->uf_class != cl)))
    {
	അക്ഷരം *സന്ദേശം = (m->ocm_access == VIM_ACCESS_PRIVATE)
				? e_cannot_access_protected_variable_str
				: e_variable_is_not_writable_str;
	emsg_var_cl_define(സന്ദേശം, m->ocm_name.string, 0, cl);
	തിരികെ FALSE;
    }

    തിരികെ TRUE;
}

/*
 * Initialize "lhs" with default values
 */
    സ്ഥിതം ശൂന്യം
lhs_init_defaults(lhs_T *lhs)
{
    CLEAR_POINTER(lhs);
    lhs->lhs_dest = dest_local;
    lhs->lhs_vimvaridx = -1;
    lhs->lhs_scriptvar_idx = -1;
    lhs->lhs_member_idx = -1;
}

/*
 * When compiling a LHS variable name, find the end of the destination and the
 * end of the variable name.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
lhs_find_var_end(
    lhs_T	*lhs,
    char_u	*var_start,
    പൂർണ്ണസംഖ്യ		is_decl,
    char_u	**var_endp)
{
    char_u  *var_end = *var_endp;

    // "lhs_dest_end" is the end of the destination, including "[expr]" or
    // ".name".
    // "var_end" is the end of the variable/option/etc. name.
    lhs->lhs_dest_end = skip_var_one(var_start, FALSE);
    എങ്കിൽ (*var_start == '@')
    {
	എങ്കിൽ (!valid_dest_reg(var_start[1]))
	    തിരികെ FAIL;
	var_end = var_start + 2;
    }
    അല്ലെങ്കിൽ
    {
	// skip over the leading "&", "&l:", "&g:" and "$"
	var_end = skip_option_env_lead(var_start);
	var_end = to_name_end(var_end, TRUE);
    }

    // "a: type" is declaring variable "a" with a type, not dict "a:".
    എങ്കിൽ (is_decl && lhs->lhs_dest_end == var_start + 2
					&& lhs->lhs_dest_end[-1] == ':')
	--lhs->lhs_dest_end;
    എങ്കിൽ (is_decl && var_end == var_start + 2 && var_end[-1] == ':')
	--var_end;

    lhs->lhs_end = lhs->lhs_dest_end;
    *var_endp = var_end;

    തിരികെ OK;
}

/*
 * Set various fields in "lhs"
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
lhs_init(
    lhs_T	*lhs,
    char_u	*var_start,
    പൂർണ്ണസംഖ്യ		is_decl,
    പൂർണ്ണസംഖ്യ		heredoc,
    char_u	**var_endp)
{
    char_u *var_end = *var_endp;

    lhs_init_defaults(lhs);

    // Find the end of the variable and the destination
    എങ്കിൽ (lhs_find_var_end(lhs, var_start, is_decl, &var_end) == FAIL)
	തിരികെ FAIL;

    // compute the length of the destination without "[expr]" or ".name"
    lhs->lhs_varlen = var_end - var_start;
    lhs->lhs_varlen_total = lhs->lhs_varlen;
    lhs->lhs_name = vim_strnsave(var_start, lhs->lhs_varlen);
    എങ്കിൽ (lhs->lhs_name == ശൂന്യ)
	തിരികെ FAIL;

    എങ്കിൽ (lhs->lhs_dest_end > var_start + lhs->lhs_varlen)
	// Something follows after the variable: "var[idx]" or "var.key".
	lhs->lhs_has_index = TRUE;

    lhs->lhs_type = heredoc ? &t_list_string : &t_any;

    *var_endp = var_end;

    തിരികെ OK;
}

/*
 * Compile a LHS class variable name.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
compile_lhs_class_variable(
    cctx_T	*cctx,
    lhs_T	*lhs,
    class_T	*defcl,
    പൂർണ്ണസംഖ്യ		is_decl)
{
    എങ്കിൽ (cctx->ctx_ufunc->uf_defclass != defcl)
    {
	// A class variable can be accessed without the class name
	// only inside a class.
	semsg(_(e_class_variable_str_accessible_only_inside_class_str),
		lhs->lhs_name, defcl->class_name.string);
	തിരികെ FAIL;
    }

    എങ്കിൽ (is_decl)
    {
	semsg(_(e_variable_already_declared_in_class_str), lhs->lhs_name);
	തിരികെ FAIL;
    }

    ocmember_T	*m = &defcl->class_class_members[lhs->lhs_classmember_idx];
    എങ്കിൽ (oc_var_check_ro(defcl, m))
	തിരികെ FAIL;

    lhs->lhs_dest = dest_class_member;
    // The class variable is defined either in the current class or
    // in one of the parent class in the hierarchy.
    lhs->lhs_class = defcl;
    lhs->lhs_type = oc_member_type_by_idx(defcl, FALSE,
						lhs->lhs_classmember_idx);

    തിരികെ OK;
}

/*
 * Compile an imported LHS variable
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
compile_lhs_import_var(
    lhs_T	*lhs,
    imported_T	*import,
    char_u	*var_start,
    char_u	**var_endp,
    char_u	**rawnamep)
{
    char_u	*var_end = *var_endp;
    char_u	*dot = vim_strchr(var_start, '.');
    char_u	*p;

    // for an import the name is what comes after the dot
    എങ്കിൽ (dot == ശൂന്യ)
    {
	semsg(_(e_no_dot_after_imported_name_str), var_start);
	തിരികെ FAIL;
    }

    p = skipwhite(dot + 1);
    var_end = to_name_end(p, TRUE);
    എങ്കിൽ (var_end == p)
    {
	semsg(_(e_missing_name_after_imported_name_str), var_start);
	തിരികെ FAIL;
    }

    vim_free(lhs->lhs_name);
    lhs->lhs_varlen = var_end - p;
    lhs->lhs_name = vim_strnsave(p, lhs->lhs_varlen);
    എങ്കിൽ (lhs->lhs_name == ശൂന്യ)
	തിരികെ FAIL;
    *rawnamep = lhs->lhs_name;
    lhs->lhs_scriptvar_sid = import->imp_sid;

    // TODO: where do we check this name is exported?

    // Check if something follows: "exp.var[idx]" or
    // "exp.var.key".
    lhs->lhs_has_index = lhs->lhs_dest_end > skipwhite(var_end);

    *var_endp = var_end;

    തിരികെ OK;
}

/*
 * Process a script-local variable when compiling a LHS variable name.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
compile_lhs_script_var(
    cctx_T	*cctx,
    lhs_T	*lhs,
    char_u	*var_start,
    char_u	*var_end,
    പൂർണ്ണസംഖ്യ		is_decl)
{
    പൂർണ്ണസംഖ്യ		script_namespace = FALSE;
    പൂർണ്ണസംഖ്യ		script_var = FALSE;
    imported_T	*import;
    char_u	*var_name;
    size_t	var_name_len;

    എങ്കിൽ (lhs->lhs_varlen > 1 && STRNCMP(var_start, "s:", 2) == 0)
	script_namespace = TRUE;

    എങ്കിൽ (script_namespace)
    {
	var_name = var_start + 2;
	var_name_len = lhs->lhs_varlen - 2;
    }
    അല്ലെങ്കിൽ
    {
	var_name = var_start;
	var_name_len = lhs->lhs_varlen;
    }

    എങ്കിൽ (script_var_exists(var_name, var_name_len, cctx, ശൂന്യ) == OK)
	script_var = TRUE;

    import = find_imported(var_start, lhs->lhs_varlen, FALSE);

    എങ്കിൽ (script_namespace || script_var || import != ശൂന്യ)
    {
	char_u *rawname = lhs->lhs_name + (lhs->lhs_name[1] == ':' ? 2 : 0);

	എങ്കിൽ (script_namespace && current_script_is_vim9())
	{
	    semsg(_(e_cannot_use_s_colon_in_vim9_script_str), var_start);
	    തിരികെ FAIL;
	}

	എങ്കിൽ (is_decl)
	{
	    എങ്കിൽ (script_namespace)
		semsg(_(e_cannot_declare_script_variable_in_function_str),
			lhs->lhs_name);
	    അല്ലെങ്കിൽ
		semsg(_(e_variable_already_declared_in_script_str),
			lhs->lhs_name);
	    തിരികെ FAIL;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (cctx->ctx_ufunc->uf_script_ctx_version == SCRIPT_VERSION_VIM9
		&& script_namespace
		&& !script_var && import == ശൂന്യ)
	{
	    semsg(_(e_unknown_variable_str), lhs->lhs_name);
	    തിരികെ FAIL;
	}

	lhs->lhs_dest = current_script_is_vim9() ? dest_script_v9 :
								dest_script;

	// existing script-local variables should have a type
	lhs->lhs_scriptvar_sid = current_sctx.sc_sid;
	എങ്കിൽ (import != ശൂന്യ)
	{
	    എങ്കിൽ (compile_lhs_import_var(lhs, import, var_start, &var_end,
							&rawname) == FAIL)
		തിരികെ FAIL;
	}

	എങ്കിൽ (SCRIPT_ID_VALID(lhs->lhs_scriptvar_sid))
	{
	    // Check writable only when no index follows.
	    lhs->lhs_scriptvar_idx = get_script_item_idx(
					lhs->lhs_scriptvar_sid, rawname,
					lhs->lhs_has_index ?  ASSIGN_FINAL :
					ASSIGN_CONST, cctx, ശൂന്യ);
	    എങ്കിൽ (lhs->lhs_scriptvar_idx >= 0)
	    {
		scriptitem_T *si = SCRIPT_ITEM(lhs->lhs_scriptvar_sid);
		svar_T	 *sv = ((svar_T *)si->sn_var_vals.ga_data)
						+ lhs->lhs_scriptvar_idx;

		lhs->lhs_type = sv->sv_type;
	    }
	}

	തിരികെ OK;
    }

    തിരികെ check_defined(var_start, lhs->lhs_varlen, cctx, ശൂന്യ, FALSE);
}

/*
 * Compile the LHS destination.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
compile_lhs_var_dest(
    cctx_T	*cctx,
    lhs_T	*lhs,
    പൂർണ്ണസംഖ്യ		cmdidx,
    char_u	*var_start,
    char_u	*var_end,
    പൂർണ്ണസംഖ്യ		is_decl,
    പൂർണ്ണസംഖ്യ		has_cmd)	// "var" before "var_start"
{
    പൂർണ്ണസംഖ്യ	    declare_error = FALSE;

    എങ്കിൽ (get_var_dest(lhs->lhs_name, &lhs->lhs_dest, cmdidx,
				lhs->lhs_has_index,
				&lhs->lhs_opt_flags, &lhs->lhs_vimvaridx,
				&lhs->lhs_type, cctx) == FAIL)
	തിരികെ FAIL;

    എങ്കിൽ (lhs->lhs_dest != dest_local && cmdidx != CMD_const
						&& cmdidx != CMD_final)
    {
	// Specific kind of variable recognized.
	declare_error = is_decl;
    }
    അല്ലെങ്കിൽ
    {
	class_T	*defcl;

	// No specific kind of variable recognized, just a name.
	എങ്കിൽ (check_reserved_name(lhs->lhs_name, lhs->lhs_has_index
						&& *var_end == '.') == FAIL)
	    തിരികെ FAIL;

	എങ്കിൽ (lookup_local(var_start, lhs->lhs_varlen, &lhs->lhs_local_lvar,
								cctx) == OK)
	{
	    lhs->lhs_lvar = &lhs->lhs_local_lvar;
	}
	അല്ലെങ്കിൽ
	{
	    CLEAR_FIELD(lhs->lhs_arg_lvar);
	    എങ്കിൽ (arg_exists(var_start, lhs->lhs_varlen,
			&lhs->lhs_arg_lvar.lv_idx, &lhs->lhs_arg_lvar.lv_type,
			&lhs->lhs_arg_lvar.lv_from_outer, cctx) == OK)
	    {
		എങ്കിൽ (is_decl)
		{
		    semsg(_(e_str_is_used_as_argument), lhs->lhs_name);
		    തിരികെ FAIL;
		}
		lhs->lhs_lvar = &lhs->lhs_arg_lvar;
	    }
	}

	എങ്കിൽ (lhs->lhs_lvar != ശൂന്യ)
	{
	    എങ്കിൽ (is_decl)
	    {
		// if we come here with what looks like an assignment like
		// .= but which has been rejected by assignment_len() from
		// may_compile_assignment give a better error message
		char_u *p = skipwhite(lhs->lhs_end);
		എങ്കിൽ (p[0] == '.' && p[1] == '=')
		    emsg(_(e_dot_equal_not_supported_with_script_version_two));
		അല്ലെങ്കിൽ എങ്കിൽ (p[0] == ':' && !has_cmd)
		    // type specified in an assignment without "var"
		    semsg(_(e_trailing_characters_str), p);
		അല്ലെങ്കിൽ
		    semsg(_(e_variable_already_declared_str), lhs->lhs_name);
		തിരികെ FAIL;
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ ((lhs->lhs_classmember_idx = cctx_class_member_idx(
			cctx, var_start, lhs->lhs_varlen, &defcl)) >= 0)
	{
	    എങ്കിൽ (compile_lhs_class_variable(cctx, lhs, defcl, is_decl)
		    == FAIL)
		തിരികെ FAIL;
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (compile_lhs_script_var(cctx, lhs, var_start, var_end,
			is_decl) == FAIL)
		തിരികെ FAIL;
	}
    }

    എങ്കിൽ (declare_error)
    {
	vim9_declare_error(lhs->lhs_name);
	തിരികെ FAIL;
    }

    തിരികെ OK;
}

/*
 * When compiling a LHS variable name, for a class or an object, set the LHS
 * member type.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
compile_lhs_set_oc_member_type(
    cctx_T	*cctx,
    lhs_T	*lhs,
    char_u	*var_start)
{
    class_T	*cl = lhs->lhs_type->tt_class;
    പൂർണ്ണസംഖ്യ		is_object = lhs->lhs_type->tt_type == VAR_OBJECT;
    char_u	*name = var_start + lhs->lhs_varlen + 1;
    size_t	namelen = lhs->lhs_end - var_start - lhs->lhs_varlen - 1;

    ocmember_T	*m = member_lookup(cl, lhs->lhs_type->tt_type,
	    name, namelen, &lhs->lhs_member_idx);
    എങ്കിൽ (m == ശൂന്യ)
    {
	member_not_found_സന്ദേശം(cl, lhs->lhs_type->tt_type, name, namelen);
	തിരികെ FAIL;
    }

    എങ്കിൽ (IS_ENUM(cl))
    {
	എങ്കിൽ (!inside_class(cctx, cl))
	{
	    semsg(_(e_enumvalue_str_cannot_be_modified),
		    cl->class_name.string, m->ocm_name.string);
	    തിരികെ FAIL;
	}
	എങ്കിൽ (lhs->lhs_type->tt_type == VAR_OBJECT &&
		lhs->lhs_member_idx < 2)
	{
	    അക്ഷരം *സന്ദേശം = lhs->lhs_member_idx == 0 ?
		e_enum_str_name_cannot_be_modified :
		e_enum_str_ordinal_cannot_be_modified;
	    semsg(_(സന്ദേശം), cl->class_name.string);
	    തിരികെ FAIL;
	}
    }

    // If it is private member variable, then accessing it outside the
    // class is not allowed.
    // If it is a read only class variable, then it can be modified
    // only inside the class where it is defined.
    എങ്കിൽ ((m->ocm_access != VIM_ACCESS_ALL) &&
	    ((is_object && !inside_class(cctx, cl))
	     || (!is_object && cctx->ctx_ufunc->uf_defclass != cl)))
    {
	അക്ഷരം *സന്ദേശം = (m->ocm_access == VIM_ACCESS_PRIVATE)
	    ? e_cannot_access_protected_variable_str
	    : e_variable_is_not_writable_str;
	emsg_var_cl_define(സന്ദേശം, m->ocm_name.string, 0, cl);
	തിരികെ FAIL;
    }

    എങ്കിൽ (!IS_CONSTRUCTOR_METHOD(cctx->ctx_ufunc)
	    && oc_var_check_ro(cl, m))
	തിരികെ FAIL;

    lhs->lhs_member_type = m->ocm_type;

    തിരികെ OK;
}

/*
 * When compiling a LHS variable, set the LHS variable type.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
compile_lhs_set_type(cctx_T *cctx, lhs_T *lhs, char_u *var_end, പൂർണ്ണസംഖ്യ is_decl)
{
    എങ്കിൽ (is_decl && *skipwhite(var_end) == ':')
    {
	char_u *p;

	// parse optional type: "let var: type = expr"
	എങ്കിൽ (VIM_ISWHITE(*var_end))
	{
	    semsg(_(e_no_white_space_allowed_before_colon_str), var_end);
	    തിരികെ FAIL;
	}

	എങ്കിൽ (!VIM_ISWHITE(var_end[1]))
	{
	    semsg(_(e_white_space_required_after_str_str), ":", var_end);
	    തിരികെ FAIL;
	}

	p = skipwhite(var_end + 1);
	lhs->lhs_type = parse_type(&p, cctx->ctx_type_list, cctx->ctx_ufunc, cctx, TRUE);
	എങ്കിൽ (lhs->lhs_type == ശൂന്യ
		|| !valid_declaration_type(lhs->lhs_type))
	    തിരികെ FAIL;

	lhs->lhs_has_type = TRUE;
	lhs->lhs_end = p;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (lhs->lhs_lvar != ശൂന്യ)
	lhs->lhs_type = lhs->lhs_lvar->lv_type;

    തിരികെ OK;
}

/*
 * Returns TRUE if "lhs" is a concatenable string.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
lhs_concatenable(lhs_T *lhs)
{
    തിരികെ lhs->lhs_dest == dest_global
		|| lhs->lhs_has_index
		|| lhs->lhs_type->tt_type == VAR_STRING
		|| lhs->lhs_type->tt_type == VAR_ANY;
}

/*
 * Create a new local variable when compiling a LHS variable.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
compile_lhs_new_local_var(
    cctx_T	*cctx,
    lhs_T	*lhs,
    char_u	*var_start,
    പൂർണ്ണസംഖ്യ		cmdidx,
    പൂർണ്ണസംഖ്യ		oplen,
    പൂർണ്ണസംഖ്യ		is_decl,
    പൂർണ്ണസംഖ്യ		has_cmd,
    പൂർണ്ണസംഖ്യ		heredoc)
{
    എങ്കിൽ (oplen > 1 && !heredoc)
    {
	// +=, /=, etc. require an existing variable
	semsg(_(e_cannot_use_operator_on_new_variable_str), lhs->lhs_name);
	തിരികെ FAIL;
    }

    എങ്കിൽ (!is_decl || (lhs->lhs_has_index && !has_cmd
					&& cctx->ctx_skip != SKIP_YES))
    {
	semsg(_(e_unknown_variable_str), lhs->lhs_name);
	തിരികെ FAIL;
    }

    // Check the name is valid for a funcref.
    എങ്കിൽ (lhs->lhs_type->tt_type == VAR_FUNC
				|| lhs->lhs_type->tt_type == VAR_PARTIAL)
    {
	എങ്കിൽ (var_wrong_func_name(lhs->lhs_name, TRUE))
	    തിരികെ FAIL;
    }

    // New local variable.
    പൂർണ്ണസംഖ്യ assign;
    തിരഞ്ഞെടുപ്പ് (cmdidx)
    {
	അവസ്ഥ CMD_final:
	    assign = ASSIGN_FINAL; നിർത്തുക;
	അവസ്ഥ CMD_const:
	    assign = ASSIGN_CONST; നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
	    assign = ASSIGN_VAR; നിർത്തുക;
    }

    lhs->lhs_lvar = reserve_local(cctx, var_start, lhs->lhs_varlen, assign,
							lhs->lhs_type);
    എങ്കിൽ (lhs->lhs_lvar == ശൂന്യ)
	തിരികെ FAIL;

    lhs->lhs_new_local = TRUE;

    തിരികെ OK;
}

/*
 * When compiling a LHS variable name, set the LHS member type.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
compile_lhs_set_member_type(
    cctx_T	*cctx,
    lhs_T	*lhs,
    char_u	*var_start,
    പൂർണ്ണസംഖ്യ		is_decl,
    പൂർണ്ണസംഖ്യ		has_cmd)
{
    lhs->lhs_member_type = lhs->lhs_type;

    എങ്കിൽ (!lhs->lhs_has_index)
	തിരികെ OK;

    char_u	*after = var_start + lhs->lhs_varlen;
    char_u	*p;

    // Something follows after the variable: "var[idx]" or "var.key".
    എങ്കിൽ (is_decl && cctx->ctx_skip != SKIP_YES)
    {
	എങ്കിൽ (has_cmd)
	    emsg(_(e_cannot_use_index_when_declaring_variable));
	അല്ലെങ്കിൽ
	    semsg(_(e_unknown_variable_str), lhs->lhs_name);
	തിരികെ FAIL;
    }

    // Now: var_start[lhs->lhs_varlen] is '[' or '.'
    // Only the last index is used below, if there are others
    // before it generate code for the expression.  Thus for
    // "ll[1][2]" the expression is "ll[1]" and "[2]" is the index.
    വേണ്ടി (;;)
    {
	p = skip_index(after);
	എങ്കിൽ (*p != '[' && *p != '.')
	{
	    lhs->lhs_varlen_total = p - var_start;
	    നിർത്തുക;
	}
	after = p;
    }
    എങ്കിൽ (after > var_start + lhs->lhs_varlen)
    {
	lhs->lhs_varlen = after - var_start;
	lhs->lhs_dest = dest_വ്യഞ്ജകം;
	// We don't know the type before evaluating the expression,
	// use "any" until then.  The member index is for the first name,
	// not for the last index.
	lhs->lhs_type = &t_any;
	lhs->lhs_member_idx = -1;
    }

    പൂർണ്ണസംഖ്യ use_class = lhs->lhs_type != ശൂന്യ
	&& (lhs->lhs_type->tt_type == VAR_CLASS
		|| lhs->lhs_type->tt_type == VAR_OBJECT);

    എങ്കിൽ (lhs->lhs_type == ശൂന്യ
	    || (use_class ? lhs->lhs_type->tt_class == ശൂന്യ
		: lhs->lhs_type->tt_member == ശൂന്യ))
    {
	lhs->lhs_member_type = &t_any;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (use_class)
    {
	// for an object or class member get the type of the member
	എങ്കിൽ (compile_lhs_set_oc_member_type(cctx, lhs, var_start) == FAIL)
	    തിരികെ FAIL;
    }
    അല്ലെങ്കിൽ
	lhs->lhs_member_type = lhs->lhs_type->tt_member;

    തിരികെ OK;
}

/*
 * Figure out the LHS type and other properties for an assignment or one item
 * of ":unlet" with an index.
 * Returns OK or FAIL.
 */
    പൂർണ്ണസംഖ്യ
compile_lhs(
	char_u	    *var_start,
	lhs_T	    *lhs,
	cmdidx_T    cmdidx,
	പൂർണ്ണസംഖ്യ	    heredoc,
	പൂർണ്ണസംഖ്യ	    has_cmd,	    // "var" before "var_start"
	പൂർണ്ണസംഖ്യ	    oplen,
	cctx_T	    *cctx)
{
    char_u	*var_end = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		is_decl = is_decl_command(cmdidx);

    എങ്കിൽ (lhs_init(lhs, var_start, is_decl, heredoc, &var_end) == FAIL)
	തിരികെ FAIL;

    എങ്കിൽ (cctx->ctx_skip != SKIP_YES)
    {
	// compile the LHS destination
	എങ്കിൽ (compile_lhs_var_dest(cctx, lhs, cmdidx, var_start, var_end,
						is_decl, has_cmd) == FAIL)
	    തിരികെ FAIL;
    }

    // handle "a:name" as a name, not index "name" in "a"
    എങ്കിൽ (lhs->lhs_varlen > 1 || var_start[lhs->lhs_varlen] != ':')
	var_end = lhs->lhs_dest_end;

    എങ്കിൽ (lhs->lhs_dest != dest_option && lhs->lhs_dest != dest_func_option)
    {
	// set the LHS variable type
	എങ്കിൽ (compile_lhs_set_type(cctx, lhs, var_end, is_decl) == FAIL)
	    തിരികെ FAIL;
    }

    എങ്കിൽ (oplen == 3 && !heredoc && !lhs_concatenable(lhs))
    {
	emsg(_(e_can_only_concatenate_to_string));
	തിരികെ FAIL;
    }

    എങ്കിൽ (lhs->lhs_lvar == ശൂന്യ && lhs->lhs_dest == dest_local
						&& cctx->ctx_skip != SKIP_YES)
    {
	എങ്കിൽ (compile_lhs_new_local_var(cctx, lhs, var_start, cmdidx, oplen,
					is_decl, has_cmd, heredoc) == FAIL)
	    തിരികെ FAIL;
    }

    എങ്കിൽ (compile_lhs_set_member_type(cctx, lhs, var_start, is_decl, has_cmd)
								== FAIL)
	തിരികെ FAIL;

    തിരികെ OK;
}

/*
 * Figure out the LHS and check a few errors.
 */
    പൂർണ്ണസംഖ്യ
compile_assign_lhs(
	char_u	    *var_start,
	lhs_T	    *lhs,
	cmdidx_T    cmdidx,
	പൂർണ്ണസംഖ്യ	    is_decl,
	പൂർണ്ണസംഖ്യ	    heredoc,
	പൂർണ്ണസംഖ്യ	    has_cmd,	    // "var" before "var_start"
	പൂർണ്ണസംഖ്യ	    oplen,
	cctx_T	    *cctx)
{
    എങ്കിൽ (compile_lhs(var_start, lhs, cmdidx, heredoc, has_cmd, oplen, cctx)
								       == FAIL)
	തിരികെ FAIL;

    എങ്കിൽ (!lhs->lhs_has_index && lhs->lhs_lvar == &lhs->lhs_arg_lvar)
    {
	semsg(_(e_cannot_assign_to_argument_str), lhs->lhs_name);
	തിരികെ FAIL;
    }
    എങ്കിൽ (!is_decl && lhs->lhs_lvar != ശൂന്യ
			   && lhs->lhs_lvar->lv_const != ASSIGN_VAR
			   && !lhs->lhs_has_index)
    {
	semsg(_(e_cannot_assign_to_constant_str), lhs->lhs_name);
	തിരികെ FAIL;
    }
    തിരികെ OK;
}

/*
 * Return TRUE if "lhs" has a range index: "[expr : expr]".
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
has_list_index(char_u *idx_start, cctx_T *cctx)
{
    char_u  *p = idx_start;
    പൂർണ്ണസംഖ്യ	    save_skip;

    എങ്കിൽ (*p != '[')
	തിരികെ FALSE;

    p = skipwhite(p + 1);
    എങ്കിൽ (*p == ':')
	തിരികെ TRUE;

    save_skip = cctx->ctx_skip;
    cctx->ctx_skip = SKIP_YES;
    (ശൂന്യം)compile_expr0(&p, cctx);
    cctx->ctx_skip = save_skip;
    തിരികെ *skipwhite(p) == ':';
}

/*
 * For an assignment with an index, compile the "idx" in "var[idx]" or "key" in
 * "var.key".
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
compile_assign_index(
	char_u	*var_start,
	lhs_T	*lhs,
	പൂർണ്ണസംഖ്യ	*range,
	cctx_T	*cctx)
{
    size_t	varlen = lhs->lhs_varlen;
    char_u	*p;
    പൂർണ്ണസംഖ്യ		r = OK;
    പൂർണ്ണസംഖ്യ		need_white_before = TRUE;
    പൂർണ്ണസംഖ്യ		empty_second;

    p = var_start + varlen;
    എങ്കിൽ (*p == '[')
    {
	p = skipwhite(p + 1);
	എങ്കിൽ (*p == ':')
	{
	    // empty first index, push zero
	    r = generate_PUSHNR(cctx, 0);
	    need_white_before = FALSE;
	}
	അല്ലെങ്കിൽ
	    r = compile_expr0(&p, cctx);

	എങ്കിൽ (r == OK && *skipwhite(p) == ':')
	{
	    // unlet var[idx : idx]
	    // blob[idx : idx] = value
	    *range = TRUE;
	    p = skipwhite(p);
	    empty_second = *skipwhite(p + 1) == ']';
	    എങ്കിൽ ((need_white_before && !IS_WHITE_OR_NUL(p[-1]))
		    || (!empty_second && !IS_WHITE_OR_NUL(p[1])))
	    {
		semsg(_(e_white_space_required_before_and_after_str_at_str),
								      ":", p);
		തിരികെ FAIL;
	    }
	    p = skipwhite(p + 1);
	    എങ്കിൽ (*p == ']')
		// empty second index, push "none"
		r = generate_PUSHSPEC(cctx, VVAL_NONE);
	    അല്ലെങ്കിൽ
		r = compile_expr0(&p, cctx);
	}

	എങ്കിൽ (r == OK && *skipwhite(p) != ']')
	{
	    // this should not happen
	    emsg(_(e_missing_closing_square_brace));
	    r = FAIL;
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (lhs->lhs_member_idx >= 0)
    {
	// object member index
	r = generate_PUSHNR(cctx, lhs->lhs_member_idx);
    }
    അല്ലെങ്കിൽ // if (*p == '.')
    {
	char_u *താക്കോൽ_end = to_name_end(p + 1, TRUE);
	char_u *താക്കോൽ = vim_strnsave(p + 1, താക്കോൽ_end - p - 1);

	r = generate_PUSHS(cctx, &താക്കോൽ);
    }
    തിരികെ r;
}

/*
 * For a LHS with an index, load the variable to be indexed.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
compile_load_lhs(
	lhs_T	*lhs,
	char_u	*var_start,
	type_T	*rhs_type,
	cctx_T	*cctx)
{
    എങ്കിൽ (lhs->lhs_dest == dest_വ്യഞ്ജകം)
    {
	size_t	    varlen = lhs->lhs_varlen;
	പൂർണ്ണസംഖ്യ	    c = var_start[varlen];
	പൂർണ്ണസംഖ്യ	    വരികൾ_len = cctx->ctx_ufunc->uf_വരികൾ.ga_len;
	പൂർണ്ണസംഖ്യ	    res;

	// Evaluate "ll[expr]" of "ll[expr][idx]".  End the line with a NUL and
	// limit the lines array length to avoid skipping to a following line.
	var_start[varlen] = NUL;
	cctx->ctx_ufunc->uf_വരികൾ.ga_len = cctx->ctx_lnum + 1;
	char_u *p = var_start;
	res = compile_expr0(&p, cctx);
	var_start[varlen] = c;
	cctx->ctx_ufunc->uf_വരികൾ.ga_len = വരികൾ_len;
	എങ്കിൽ (res == FAIL || p != var_start + varlen)
	{
	    // this should not happen
	    എങ്കിൽ (res != FAIL)
		emsg(_(e_missing_closing_square_brace));
	    തിരികെ FAIL;
	}

	lhs->lhs_type = cctx->ctx_type_അട്ടി.ga_len == 0 ? &t_void
						  : get_type_on_അട്ടി(cctx, 0);

	എങ്കിൽ (lhs->lhs_type->tt_type == VAR_CLASS
		|| (lhs->lhs_type->tt_type == VAR_OBJECT
		    && lhs->lhs_type != &t_object_any))
	{
	    // Check whether the class or object variable is modifiable
	    എങ്കിൽ (!lhs_class_member_modifiable(lhs, var_start, cctx))
		തിരികെ FAIL;
	}

	// Now we can properly check the type.  The variable is indexed, thus
	// we need the member type.  For a class or object we don't know the
	// type yet, it depends on what member is used.
	// The top item in the stack is the Dict, followed by the key and then
	// the type of the value.
	vartype_T vartype = lhs->lhs_type->tt_type;
	type_T *member_type = lhs->lhs_type->tt_member;
	എങ്കിൽ (rhs_type != ശൂന്യ && member_type != ശൂന്യ
		&& vartype != VAR_OBJECT && vartype != VAR_CLASS
		&& rhs_type != &t_void
		&& need_type(rhs_type, member_type, 0,
					    -3, 0, cctx, FALSE, FALSE) == FAIL)
	    തിരികെ FAIL;

	തിരികെ OK;
    }

    തിരികെ generate_loadvar(cctx, lhs);
}

/*
 * Produce code for loading "lhs" and also take care of an index.
 * Return OK/FAIL.
 */
    പൂർണ്ണസംഖ്യ
compile_load_lhs_with_index(lhs_T *lhs, char_u *var_start, cctx_T *cctx)
{
    എങ്കിൽ (lhs->lhs_type->tt_type == VAR_OBJECT)
    {
	// "this.value": load "this" object and get the value at index for an
	// object or class member get the type of the member.
	// Also for "obj.value".
	char_u *dot = vim_strchr(var_start, '.');
	എങ്കിൽ (dot == ശൂന്യ)
	{
	    semsg(_(e_missing_dot_after_object_str), lhs->lhs_name);
	    തിരികെ FAIL;
	}

	class_T	*cl = lhs->lhs_type->tt_class;
	type_T	*type = oc_member_type(cl, TRUE, dot + 1,
					  lhs->lhs_end, &lhs->lhs_member_idx);
	എങ്കിൽ (lhs->lhs_member_idx < 0)
	    തിരികെ FAIL;

	എങ്കിൽ (dot - var_start == 4 && STRNCMP(var_start, "this", 4) == 0)
	{
	    // load "this"
	    lvar_T  *lvar = lhs->lhs_lvar;
	    പൂർണ്ണസംഖ്യ	    rc;

	    എങ്കിൽ (lvar->lv_from_outer > 0)
		rc = generate_LOADOUTER(cctx, lvar->lv_idx,
			lvar->lv_from_outer, lvar->lv_loop_depth,
			lvar->lv_loop_idx, type);
	    അല്ലെങ്കിൽ
		rc = generate_LOAD(cctx, ISN_LOAD, lvar->lv_idx, ശൂന്യ, type);

	    എങ്കിൽ (rc == FAIL)
		തിരികെ FAIL;
	}
	അല്ലെങ്കിൽ
	{
	    // load object variable or argument
	    എങ്കിൽ (compile_load_lhs(lhs, var_start, lhs->lhs_type, cctx) == FAIL)
		തിരികെ FAIL;
	}
	എങ്കിൽ (IS_INTERFACE(cl))
	    തിരികെ generate_GET_ITF_MEMBER(cctx, cl, lhs->lhs_member_idx, type);
	തിരികെ generate_GET_OBJ_MEMBER(cctx, lhs->lhs_member_idx, type);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (lhs->lhs_type->tt_type == VAR_CLASS)
    {
	// "<classname>.value": load class variable "classname.value"
	char_u *dot = vim_strchr(var_start, '.');
	എങ്കിൽ (dot == ശൂന്യ)
	{
	    (ശൂന്യം)check_type_is_മൂല്യം(lhs->lhs_type);
	    തിരികെ FAIL;
	}

	class_T	*cl = lhs->lhs_type->tt_class;
	ocmember_T *m = class_member_lookup(cl, dot + 1,
						lhs->lhs_end - dot - 1,
						&lhs->lhs_member_idx);
	എങ്കിൽ (m == ശൂന്യ)
	    തിരികെ FAIL;

	തിരികെ generate_CLASSMEMBER(cctx, TRUE, cl, lhs->lhs_member_idx);
    }

    എങ്കിൽ (compile_load_lhs(lhs, var_start, ശൂന്യ, cctx) == FAIL)
	തിരികെ FAIL;

    എങ്കിൽ (lhs->lhs_has_index)
    {
	പൂർണ്ണസംഖ്യ	range = FALSE;
	type_T	*type = get_type_on_അട്ടി(cctx, 0);

	// A member of an object or class is not obtained by indexing it.
	എങ്കിൽ (type->tt_type == VAR_CLASS
		|| (type->tt_type == VAR_OBJECT && type != &t_object_any))
	{
	    char_u *p = var_start + lhs->lhs_varlen;

	    തിരികെ compile_class_object_index(cctx, &p, type);
	}

	// Get member from list or dict.  First compile the
	// index value.
	എങ്കിൽ (compile_assign_index(var_start, lhs, &range, cctx) == FAIL)
	    തിരികെ FAIL;
	എങ്കിൽ (range)
	{
	    semsg(_(e_cannot_use_range_with_assignment_operator_str),
								    var_start);
	    തിരികെ FAIL;
	}

	// Get the member.
	എങ്കിൽ (compile_member(FALSE, ശൂന്യ, cctx) == FAIL)
	    തിരികെ FAIL;
    }
    തിരികെ OK;
}

/*
 * Assignment to a list or dict member, or ":unlet" for the item, using the
 * information in "lhs".
 * Returns OK or FAIL.
 */
    പൂർണ്ണസംഖ്യ
compile_assign_unlet(
    char_u	*var_start,
    lhs_T	*lhs,
    പൂർണ്ണസംഖ്യ		is_assign,
    type_T	*rhs_type,
    cctx_T	*cctx)
{
    vartype_T	dest_type;
    പൂർണ്ണസംഖ്യ		range = FALSE;

    എങ്കിൽ (compile_assign_index(var_start, lhs, &range, cctx) == FAIL)
	തിരികെ FAIL;
    എങ്കിൽ (is_assign && range
	    && lhs->lhs_type->tt_type != VAR_LIST
	    && lhs->lhs_type != &t_blob
	    && lhs->lhs_type != &t_any)
    {
	എങ്കിൽ (lhs->lhs_type->tt_type == VAR_TUPLE)
	    emsg(_(e_cannot_slice_tuple));
	അല്ലെങ്കിൽ
	    semsg(_(e_cannot_use_range_with_assignment_str), var_start);
	തിരികെ FAIL;
    }

    // For "expr[idx]" the index is compiled before the expression, thus the
    // resulting type cannot be used here.
    എങ്കിൽ (lhs->lhs_dest == dest_വ്യഞ്ജകം
	    || lhs->lhs_type == ശൂന്യ || lhs->lhs_type == &t_any)
    {
	// Index on variable of unknown type: check at runtime.
	dest_type = VAR_ANY;
    }
    അല്ലെങ്കിൽ
    {
	dest_type = lhs->lhs_type->tt_type;
	എങ്കിൽ (dest_type == VAR_DICT && range)
	{
	    emsg(_(e_cannot_use_range_with_dictionary));
	    തിരികെ FAIL;
	}
	എങ്കിൽ (dest_type == VAR_DICT
		&& may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
	    തിരികെ FAIL;
	എങ്കിൽ (dest_type == VAR_LIST || dest_type == VAR_BLOB)
	{
	    type_T *type;

	    എങ്കിൽ (range)
	    {
		type = get_type_on_അട്ടി(cctx, 1);
		എങ്കിൽ (need_type(type, &t_സംഖ്യ, 0,
					    -2, 0, cctx, FALSE, FALSE) == FAIL)
		തിരികെ FAIL;
	    }
	    type = get_type_on_അട്ടി(cctx, 0);
	    എങ്കിൽ ((dest_type != VAR_BLOB && type->tt_type != VAR_SPECIAL)
		    && need_type(type, &t_സംഖ്യ, 0,
					    -1, 0, cctx, FALSE, FALSE) == FAIL)
		തിരികെ FAIL;
	}
    }

    എങ്കിൽ (cctx->ctx_skip == SKIP_YES)
	തിരികെ OK;

    // Load the dict, list or object.  On the stack we then have:
    // - value (for assignment, not for :unlet)
    // - index
    // - for [a : b] second index
    // - variable
    എങ്കിൽ (compile_load_lhs(lhs, var_start, rhs_type, cctx) == FAIL)
	തിരികെ FAIL;

    എങ്കിൽ (dest_type == VAR_LIST
	    || dest_type == VAR_DICT
	    || dest_type == VAR_BLOB
	    || dest_type == VAR_CLASS
	    || dest_type == VAR_OBJECT
	    || dest_type == VAR_ANY)
    {
	എങ്കിൽ (is_assign)
	{
	    എങ്കിൽ (range)
	    {
		എങ്കിൽ (generate_instr_drop(cctx, ISN_STORERANGE, 4) == ശൂന്യ)
		    തിരികെ FAIL;
	    }
	    അല്ലെങ്കിൽ
	    {
		isn_T	*isn = generate_instr_drop(cctx, ISN_STOREINDEX, 3);

		എങ്കിൽ (isn == ശൂന്യ)
		    തിരികെ FAIL;
		isn->isn_arg.storeindex.si_vartype = dest_type;
		isn->isn_arg.storeindex.si_class = ശൂന്യ;

		എങ്കിൽ (dest_type == VAR_OBJECT)
		{
		    class_T *cl = lhs->lhs_type->tt_class;

		    എങ്കിൽ (IS_INTERFACE(cl))
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
	അല്ലെങ്കിൽ എങ്കിൽ (range)
	{
	    എങ്കിൽ (generate_instr_drop(cctx, ISN_UNLETRANGE, 3) == ശൂന്യ)
		തിരികെ FAIL;
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (generate_instr_drop(cctx, ISN_UNLETINDEX, 2) == ശൂന്യ)
		തിരികെ FAIL;
	}
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (dest_type == VAR_TUPLE)
	    emsg(_(e_tuple_is_immutable));
	അല്ലെങ്കിൽ
	    emsg(_(e_indexable_type_required));
	തിരികെ FAIL;
    }

    തിരികെ OK;
}

/*
 * Generate an instruction to push the default value for "vartype".
 * if "dest_local" is TRUE then for some types no instruction is generated.
 * "skip_store" is set to TRUE if no PUSH instruction is generated.
 * Returns OK or FAIL.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
തള്ളുക_default_മൂല്യം(
	cctx_T	    *cctx,
	vartype_T   vartype,
	പൂർണ്ണസംഖ്യ	    dest_is_local,
	പൂർണ്ണസംഖ്യ	    *skip_store)
{
    പൂർണ്ണസംഖ്യ r = OK;

    തിരഞ്ഞെടുപ്പ് (vartype)
    {
	അവസ്ഥ VAR_BOOL:
	    r = generate_PUSHBOOL(cctx, VVAL_FALSE);
	    നിർത്തുക;
	അവസ്ഥ VAR_FLOAT:
	    r = generate_PUSHF(cctx, 0.0);
	    നിർത്തുക;
	അവസ്ഥ VAR_STRING:
	    r = generate_PUSHS(cctx, ശൂന്യ);
	    നിർത്തുക;
	അവസ്ഥ VAR_BLOB:
	    r = generate_PUSHBLOB(cctx, blob_alloc());
	    നിർത്തുക;
	അവസ്ഥ VAR_FUNC:
	    r = generate_PUSHFUNC(cctx, ശൂന്യ, &t_func_void, TRUE);
	    നിർത്തുക;
	അവസ്ഥ VAR_LIST:
	    r = generate_NEWLIST(cctx, 0, FALSE);
	    നിർത്തുക;
	അവസ്ഥ VAR_TUPLE:
	    r = generate_NEWTUPLE(cctx, 0, FALSE);
	    നിർത്തുക;
	അവസ്ഥ VAR_DICT:
	    r = generate_NEWDICT(cctx, 0, FALSE);
	    നിർത്തുക;
	അവസ്ഥ VAR_JOB:
	    r = generate_PUSHJOB(cctx);
	    നിർത്തുക;
	അവസ്ഥ VAR_CHANNEL:
	    r = generate_PUSHCHANNEL(cctx);
	    നിർത്തുക;
	അവസ്ഥ VAR_OBJECT:
	    r = generate_PUSHOBJ(cctx);
	    നിർത്തുക;
	അവസ്ഥ VAR_സംഖ്യ:
	അവസ്ഥ VAR_UNKNOWN:
	അവസ്ഥ VAR_ANY:
	അവസ്ഥ VAR_PARTIAL:
	അവസ്ഥ VAR_VOID:
	അവസ്ഥ VAR_INSTR:
	അവസ്ഥ VAR_CLASS:
	അവസ്ഥ VAR_TYPEALIAS:
	അവസ്ഥ VAR_SPECIAL:  // cannot happen
	    // This is skipped for local variables, they are always
	    // initialized to zero.  But in a "for" or "while" loop
	    // the value may have been changed.
	    എങ്കിൽ (dest_is_local && !inside_loop_scope(cctx))
		*skip_store = TRUE;
	    അല്ലെങ്കിൽ
		r = generate_PUSHNR(cctx, 0);
	    നിർത്തുക;
    }
    തിരികെ r;
}

/*
 * Compile assignment context.  Used when compiling an assignment statement.
 */
തരനിർവചനം ഘടന cac_S cac_T;
ഘടന cac_S
{
    cmdidx_T	cac_cmdidx;		// assignment command
    char_u	*cac_nextc;		// next character to parse
    lhs_T	cac_lhs;		// lhs of the assignment
    type_T	*cac_rhs_type;		// rhs type of an assignment
    char_u	*cac_op;		// assignment operator
    പൂർണ്ണസംഖ്യ		cac_oplen;		// assignment operator length
    char_u	*cac_var_start;		// start of the variable names
    char_u	*cac_var_end;		// end of the variable names
    പൂർണ്ണസംഖ്യ		cac_var_എണ്ണം;		// number of variables in assignment
    പൂർണ്ണസംഖ്യ		cac_var_idx;		// variable index in a list
    പൂർണ്ണസംഖ്യ		cac_semicolon;		// semicolon in [var1, var2; var3]
    garray_T	*cac_instr;
    പൂർണ്ണസംഖ്യ		cac_instr_എണ്ണം;
    പൂർണ്ണസംഖ്യ		cac_incdec;
    പൂർണ്ണസംഖ്യ		cac_did_generate_slice;
    പൂർണ്ണസംഖ്യ		cac_is_decl;
    പൂർണ്ണസംഖ്യ		cac_is_const;
    പൂർണ്ണസംഖ്യ		cac_start_lnum;
    type_T	*cac_inferred_type;
    പൂർണ്ണസംഖ്യ		cac_skip_store;
};

/*
 * Initialize the compile assignment context.
 */
    സ്ഥിതം ശൂന്യം
compile_assign_context_init(cac_T *cac, cctx_T *cctx, പൂർണ്ണസംഖ്യ cmdidx, char_u *arg)
{
    CLEAR_FIELD(*cac);
    cac->cac_cmdidx = cmdidx;
    cac->cac_instr = &cctx->ctx_instr;
    cac->cac_rhs_type = &t_any;
    cac->cac_is_decl = is_decl_command(cmdidx);
    cac->cac_start_lnum = SOURCING_LNUM;
    cac->cac_instr_എണ്ണം = -1;
    cac->cac_var_end = arg;
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
compile_assign_obj_new_arg(char_u **argp, cctx_T *cctx)
{
    char_u *arg = *argp;

    arg += 11;	    // skip "ifargisset"
    പൂർണ്ണസംഖ്യ def_arg_idx = getdigits(&arg);
    arg = skipwhite(arg);

    // Use a JUMP_IF_ARG_NOT_SET instruction to skip if the value was not
    // given and the default value is "v:none".
    പൂർണ്ണസംഖ്യ അട്ടി_offset = അട്ടി_FRAME_SIZE +
				(cctx->ctx_ufunc->uf_va_name != ശൂന്യ ? 1 : 0);
    പൂർണ്ണസംഖ്യ def_arg_എണ്ണം = cctx->ctx_ufunc->uf_def_args.ga_len;
    പൂർണ്ണസംഖ്യ arg_offset = def_arg_idx - def_arg_എണ്ണം - അട്ടി_offset;

    എങ്കിൽ (generate_JUMP_IF_ARG(cctx, ISN_JUMP_IF_ARG_NOT_SET,
			     arg_offset) == FAIL)
	തിരികെ FAIL;

    *argp = arg;
    തിരികെ OK;
}

/*
 * Translate the increment (++) and decrement (--) operators to the
 * corresponding compound operators (+= or -=).
 *
 * Returns OK on success and FAIL on syntax error.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
translate_incdec_op(exarg_T *eap, cac_T *cac)
{
    എങ്കിൽ (VIM_ISWHITE(eap->cmd[2]))
    {
	semsg(_(e_no_white_space_allowed_after_str_str),
		eap->cmdidx == CMD_increment ? "++" : "--", eap->cmd);
	തിരികെ FAIL;
    }
    cac->cac_op = (char_u *)(eap->cmdidx == CMD_increment ? "+=" : "-=");
    cac->cac_oplen = 2;
    cac->cac_incdec = TRUE;

    തിരികെ OK;
}

/*
 * Process the operator in an assignment statement.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
compile_assign_process_operator(
    exarg_T	*eap,
    char_u	*arg,
    cac_T	*cac,
    പൂർണ്ണസംഖ്യ		*heredoc,
    char_u	**retstr)
{
    *retstr = ശൂന്യ;

    എങ്കിൽ (eap->cmdidx == CMD_increment || eap->cmdidx == CMD_decrement)
	// Change an unary operator to a compound operator
	തിരികെ translate_incdec_op(eap, cac);

    char_u *sp = cac->cac_nextc;
    cac->cac_nextc = skipwhite(cac->cac_nextc);
    cac->cac_op = cac->cac_nextc;
    cac->cac_oplen = assignment_len(cac->cac_nextc, heredoc);

    എങ്കിൽ (cac->cac_var_എണ്ണം > 0 && cac->cac_oplen == 0)
    {
	// can be something like "[1, 2]->func()"
	*retstr = arg;
	തിരികെ FAIL;
    }

    // need white space before and after the operator
    എങ്കിൽ (cac->cac_oplen > 0 && (!VIM_ISWHITE(*sp)
		|| !IS_WHITE_OR_NUL(cac->cac_op[cac->cac_oplen])))
    {
	error_white_both(cac->cac_op, cac->cac_oplen);
	തിരികെ FAIL;
    }

    തിരികെ OK;
}

/*
 * Find the start of an assignment statement.
 */
    സ്ഥിതം char_u *
compile_assign_compute_start(char_u *arg, പൂർണ്ണസംഖ്യ var_എണ്ണം)
{
    എങ്കിൽ (var_എണ്ണം > 0)
	// [var1, var2] = [val1, val2]
	// skip over the "["
	തിരികെ skipwhite(arg + 1);

    തിരികെ arg;
}

/*
 * Parse a heredoc assignment starting at "p".  Returns a pointer to the
 * beginning of the heredoc content.
 */
    സ്ഥിതം char_u *
parse_heredoc_assignment(exarg_T *eap, cctx_T *cctx, cac_T *cac)
{
    // [let] varname =<< [trim] {end}
    eap->ea_getline = exarg_getline;
    eap->cookie = cctx;

    list_T *l = heredoc_get(eap, cac->cac_nextc + 3, FALSE, TRUE);
    എങ്കിൽ (l == ശൂന്യ)
	തിരികെ ശൂന്യ;

    list_free(l);
    cac->cac_nextc += STRLEN(cac->cac_nextc);

    തിരികെ cac->cac_nextc;
}

/*
 * Check the type of a RHS expression in a list assignment statement.
 * The RHS expression is already compiled.  So the type is on the stack.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
compile_assign_list_check_rhs_type(cctx_T *cctx, cac_T *cac)
{
    type_T	*stacktype;

    stacktype = cctx->ctx_type_അട്ടി.ga_len == 0 ? &t_void
						: get_type_on_അട്ടി(cctx, 0);
    എങ്കിൽ (stacktype->tt_type == VAR_VOID)
    {
	emsg(_(e_cannot_use_void_മൂല്യം));
	തിരികെ FAIL;
    }

    എങ്കിൽ (stacktype->tt_type != VAR_LIST && stacktype->tt_type != VAR_TUPLE
					&& stacktype->tt_type != VAR_ANY)
    {
	emsg(_(e_list_or_tuple_required));
	തിരികെ FAIL;
    }

    എങ്കിൽ (need_type(stacktype,
		  stacktype->tt_type == VAR_TUPLE ? &t_tuple_any : &t_list_any,
		  TYPECHK_TUPLE_OK, -1, 0, cctx, FALSE, FALSE) == FAIL)
	തിരികെ FAIL;

    // The check accepts both a list and a tuple. Keep "any", making it a list
    // would reject a tuple later on.
    എങ്കിൽ (stacktype->tt_type == VAR_ANY)
	set_type_on_അട്ടി(cctx, &t_any, 0);

    എങ്കിൽ (stacktype->tt_type == VAR_TUPLE)
    {
	എങ്കിൽ (stacktype->tt_argcount != 1)
	    cac->cac_rhs_type = &t_any;
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (stacktype->tt_flags & TTFLAG_VARARGS)
		cac->cac_rhs_type = stacktype->tt_args[0]->tt_member;
	    അല്ലെങ്കിൽ
		cac->cac_rhs_type = stacktype->tt_args[0];
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (stacktype->tt_member != ശൂന്യ)
	cac->cac_rhs_type = stacktype->tt_member;

    തിരികെ OK;
}

/*
 * In a list assignment statement, if a constant list was used, check the
 * length.  Returns OK if the length check succeeds.  Returns FAIL otherwise.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
compile_assign_list_check_length(cctx_T *cctx, cac_T *cac)
{
    പൂർണ്ണസംഖ്യ	needed_list_len;
    പൂർണ്ണസംഖ്യ	did_check = FALSE;

    needed_list_len = cac->cac_semicolon
				? cac->cac_var_എണ്ണം - 1
				: cac->cac_var_എണ്ണം;
    എങ്കിൽ (cac->cac_instr->ga_len > 0)
    {
	isn_T	*isn = ((isn_T *)cac->cac_instr->ga_data) +
						cac->cac_instr->ga_len - 1;

	എങ്കിൽ (isn->isn_type == ISN_NEWLIST || isn->isn_type == ISN_NEWTUPLE)
	{
	    did_check = TRUE;
	    എങ്കിൽ (cac->cac_semicolon ?
			isn->isn_arg.സംഖ്യ < needed_list_len
			: isn->isn_arg.സംഖ്യ != needed_list_len)
	    {
		semsg(_(e_expected_nr_ഇനങ്ങൾ_but_got_nr),
			needed_list_len, (പൂർണ്ണസംഖ്യ)isn->isn_arg.സംഖ്യ);
		തിരികെ FAIL;
	    }
	}
    }

    എങ്കിൽ (!did_check)
	generate_CHECKLEN(cctx, needed_list_len, cac->cac_semicolon);

    തിരികെ OK;
}

/*
 * Evaluate the expression for "[var, var] = expr" assignment.
 * A line break may follow the assignment operator "=".
 */
    സ്ഥിതം char_u *
compile_assign_list_വ്യഞ്ജകം(cctx_T *cctx, cac_T *cac)
{
    char_u *whitep;

    whitep = cac->cac_op + cac->cac_oplen;

    എങ്കിൽ (may_get_next_വരി_error(whitep, &cac->cac_nextc, cctx) == FAIL)
	തിരികെ ശൂന്യ;

    // compile RHS expression
    എങ്കിൽ (compile_expr0(&cac->cac_nextc, cctx) == FAIL)
	തിരികെ ശൂന്യ;

    എങ്കിൽ (cctx->ctx_skip == SKIP_YES)
	// no need to parse more when skipping
	തിരികെ cac->cac_nextc;

    എങ്കിൽ (compile_assign_list_check_rhs_type(cctx, cac) == FAIL)
	തിരികെ ശൂന്യ;

    // If a constant list was used we can check the length right here.
    എങ്കിൽ (compile_assign_list_check_length(cctx, cac) == FAIL)
	തിരികെ FAIL;

    തിരികെ cac->cac_nextc;
}

/*
 * Find and return the end of a heredoc or a list of variables assignment
 * statement.  For a single variable assignment statement, returns the current
 * end.
 * Returns NULL on failure.
 */
    സ്ഥിതം char_u *
compile_assign_compute_end(
    exarg_T	*eap,
    cctx_T	*cctx,
    cac_T	*cac,
    പൂർണ്ണസംഖ്യ		heredoc)
{
    എങ്കിൽ (heredoc)
    {
	cac->cac_nextc = parse_heredoc_assignment(eap, cctx, cac);
	തിരികെ cac->cac_nextc;
    }

    എങ്കിൽ (cac->cac_var_എണ്ണം > 0)
    {
	// for "[var, var] = expr" evaluate the expression. The list of
	// variables are processed later.
	// A line break may follow the "=".
	cac->cac_nextc = compile_assign_list_വ്യഞ്ജകം(cctx, cac);
	തിരികെ cac->cac_nextc;
    }

    തിരികെ cac->cac_var_end;
}

/*
 * For "var = expr" evaluate the expression.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
compile_assign_single_eval_വ്യഞ്ജകം(cctx_T *cctx, cac_T *cac)
{
    പൂർണ്ണസംഖ്യ		ret = OK;
    char_u	*whitep;
    lhs_T	*lhs = &cac->cac_lhs;
    lvar_T	*lvp;
    lvar_T	save_lhs_lvar;

    // Compile the expression.
    എങ്കിൽ (cac->cac_incdec)
	തിരികെ generate_PUSHNR(cctx, 1);

    // Temporarily hide the new local variable here, it is
    // not available to this expression.
    എങ്കിൽ (lhs->lhs_new_local)
    {
	--cctx->ctx_locals.ga_len;

	// Save the local variable value (compiling the RHS expression may
	// create new local variables).
	lvp = ((lvar_T *)cctx->ctx_locals.ga_data) + cctx->ctx_locals.ga_len;
	save_lhs_lvar = *lvp;
    }
    whitep = cac->cac_op + cac->cac_oplen;

    എങ്കിൽ (may_get_next_വരി_error(whitep, &cac->cac_nextc, cctx) == FAIL)
    {
	എങ്കിൽ (lhs->lhs_new_local)
	    ++cctx->ctx_locals.ga_len;
	തിരികെ FAIL;
    }

    ret = compile_expr0_ext(&cac->cac_nextc, cctx, &cac->cac_is_const);
    എങ്കിൽ (lhs->lhs_new_local)
    {
	// Restore the local variable value.  Update lhs_lvar as the index of
	// the local variable might have changed.
	lvp = ((lvar_T *)cctx->ctx_locals.ga_data) + cctx->ctx_locals.ga_len;
	*lvp = save_lhs_lvar;
	lhs->lhs_lvar = lvp;

	++cctx->ctx_locals.ga_len;
    }

    തിരികെ ret;
}

/*
 * When compiling an assignment, set the LHS type to the RHS type.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
compile_assign_set_lhs_type_from_rhs(
    cctx_T	*cctx,
    cac_T	*cac,
    lhs_T	*lhs,
    type_T	*rhs_type)
{
    എങ്കിൽ (rhs_type->tt_type == VAR_VOID)
    {
	emsg(_(e_cannot_use_void_മൂല്യം));
	തിരികെ FAIL;
    }

    type_T *type;

    // An empty list or dict has a &t_unknown member, for a variable that
    // implies &t_any.
    എങ്കിൽ (rhs_type == &t_list_empty)
	type = &t_list_any;
    അല്ലെങ്കിൽ എങ്കിൽ (rhs_type == &t_dict_empty)
	type = &t_dict_any;
    അല്ലെങ്കിൽ എങ്കിൽ (rhs_type == &t_unknown)
	type = &t_any;
    അല്ലെങ്കിൽ
    {
	type = rhs_type;
	cac->cac_inferred_type = rhs_type;
    }

    set_var_type(lhs->lhs_lvar, type, cctx);

    തിരികെ OK;
}

/*
 * Returns TRUE if the "rhs_type" can be assigned to the "lhs" variable.
 * Used when compiling an assignment statement.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
compile_assign_valid_rhs_type(
    cctx_T	*cctx,
    cac_T	*cac,
    lhs_T	*lhs,
    type_T	*rhs_type)
{
    type_T	*use_type = lhs->lhs_lvar->lv_type;
    where_T	where = WHERE_INIT;

    // Without operator check type here, otherwise below.
    // Use the line number of the assignment.
    SOURCING_LNUM = cac->cac_start_lnum;
    എങ്കിൽ (cac->cac_var_എണ്ണം > 0)
    {
	where.wt_index = cac->cac_var_idx + 1;
	where.wt_kind = WT_VARIABLE;
    }

    // If assigning to a list or dict member, use the member type.
    // Not for "list[:] =".
    എങ്കിൽ (lhs->lhs_has_index &&
	    !has_list_index(cac->cac_var_start + lhs->lhs_varlen, cctx))
	use_type = lhs->lhs_member_type;

    എങ്കിൽ (need_type_where(rhs_type, use_type, 0, -1, where, cctx, FALSE,
						cac->cac_is_const) == FAIL)
	തിരികെ FALSE;

    തിരികെ TRUE;
}

/*
 * Compare the LHS type with the RHS type in an assignment.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
compile_assign_check_type(cctx_T *cctx, cac_T *cac)
{
    lhs_T	*lhs = &cac->cac_lhs;
    type_T	*rhs_type;

    rhs_type = cctx->ctx_type_അട്ടി.ga_len == 0
					? &t_void
					: get_type_on_അട്ടി(cctx, 0);
    cac->cac_rhs_type = rhs_type;

    എങ്കിൽ (check_type_is_മൂല്യം(rhs_type) == FAIL)
	തിരികെ FAIL;

    എങ്കിൽ (lhs->lhs_lvar != ശൂന്യ && (cac->cac_is_decl || !lhs->lhs_has_type))
    {
	എങ്കിൽ (rhs_type->tt_type == VAR_FUNC
					|| rhs_type->tt_type == VAR_PARTIAL)
	{
	    // Make sure the variable name can be used as a funcref
	    എങ്കിൽ (!lhs->lhs_has_index
				&& var_wrong_func_name(lhs->lhs_name, TRUE))
		തിരികെ FAIL;
	}

	എങ്കിൽ (lhs->lhs_new_local && !lhs->lhs_has_type)
	{
	    // The LHS variable doesn't have a type.  Set it to the RHS type.
	    എങ്കിൽ (compile_assign_set_lhs_type_from_rhs(cctx, cac, lhs, rhs_type)
								== FAIL)
		തിരികെ FAIL;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (*cac->cac_op == '=')
	{
	    എങ്കിൽ (!compile_assign_valid_rhs_type(cctx, cac, lhs, rhs_type))
		തിരികെ FAIL;
	}
    }
    അല്ലെങ്കിൽ
    {
	// Assigning to a register using @r = "abc"

	type_T *lhs_type = lhs->lhs_member_type;

	// Special case: assigning to @# can use a number or a string.
	// Also: can assign a number to a float.
	എങ്കിൽ ((lhs_type == &t_സംഖ്യ_or_string || lhs_type == &t_float)
					&& rhs_type->tt_type == VAR_സംഖ്യ)
	    lhs_type = &t_സംഖ്യ;

	എങ്കിൽ (*cac->cac_nextc != '=')
	{
	    എങ്കിൽ (need_type(rhs_type, lhs_type, 0, -1, 0, cctx, FALSE,
							FALSE) == FAIL)
		തിരികെ FAIL;
	}
    }

    തിരികെ OK;
}

/*
 * Compile the RHS expression in an assignment statement and generate the
 * instructions.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
compile_assign_rhs_വ്യഞ്ജകം(cctx_T *cctx, cac_T *cac)
{
    cac->cac_is_const = FALSE;

    // for "+=", "*=", "..=" etc. first load the current value
    എങ്കിൽ (*cac->cac_op != '='
	    && compile_load_lhs_with_index(&cac->cac_lhs, cac->cac_var_start,
								cctx) == FAIL)
	തിരികെ FAIL;

    // For "var = expr" evaluate the expression.
    എങ്കിൽ (cac->cac_var_എണ്ണം == 0)
    {
	പൂർണ്ണസംഖ്യ	ret;

	// Compile the expression.
	cac->cac_instr_എണ്ണം = cac->cac_instr->ga_len;
	ret = compile_assign_single_eval_വ്യഞ്ജകം(cctx, cac);
	എങ്കിൽ (ret == FAIL)
	    തിരികെ FAIL;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (cac->cac_semicolon && cac->cac_var_idx == cac->cac_var_എണ്ണം - 1)
    {
	// For "[var; var] = expr" get the rest of the list
	cac->cac_did_generate_slice = TRUE;
	എങ്കിൽ (generate_SLICE(cctx, cac->cac_var_എണ്ണം - 1) == FAIL)
	    തിരികെ FAIL;
    }
    അല്ലെങ്കിൽ
    {
	// For "[var, var] = expr" get the "var_idx" item from the
	// list.
	പൂർണ്ണസംഖ്യ with_op = *cac->cac_op != '=';
	എങ്കിൽ (generate_GETITEM(cctx, cac->cac_var_idx, with_op) == FAIL)
	    തിരികെ FAIL;
    }

    എങ്കിൽ (compile_assign_check_type(cctx, cac) == FAIL)
	തിരികെ FAIL;

    തിരികെ OK;
}

/*
 * Compile the RHS expression in an assignment
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
compile_assign_rhs(cctx_T *cctx, cac_T *cac)
{
    lhs_T	*lhs = &cac->cac_lhs;

    എങ്കിൽ (cctx->ctx_skip == SKIP_YES)
    {
	എങ്കിൽ (cac->cac_oplen > 0 && cac->cac_var_എണ്ണം == 0)
	{
	    // skip over the "=" and the expression
	    cac->cac_nextc = skipwhite(cac->cac_op + cac->cac_oplen);
	    (ശൂന്യം)compile_expr0(&cac->cac_nextc, cctx);
	}
	തിരികെ OK;
    }

    // If RHS is specified, then generate instructions for RHS expression
    എങ്കിൽ (cac->cac_oplen > 0)
	തിരികെ compile_assign_rhs_വ്യഞ്ജകം(cctx, cac);

    എങ്കിൽ (cac->cac_cmdidx == CMD_final)
    {
	emsg(_(e_final_requires_a_മൂല്യം));
	തിരികെ FAIL;
    }

    എങ്കിൽ (cac->cac_cmdidx == CMD_const)
    {
	emsg(_(e_const_requires_a_മൂല്യം));
	തിരികെ FAIL;
    }

    എങ്കിൽ (!lhs->lhs_has_type || lhs->lhs_dest == dest_option
					|| lhs->lhs_dest == dest_func_option)
    {
	emsg(_(e_type_or_initialization_required));
	തിരികെ FAIL;
    }

    // variables are always initialized
    എങ്കിൽ (GA_GROW_FAILS(cac->cac_instr, 1))
	തിരികെ FAIL;

    cac->cac_instr_എണ്ണം = cac->cac_instr->ga_len;

    തിരികെ തള്ളുക_default_മൂല്യം(cctx, lhs->lhs_member_type->tt_type,
			      lhs->lhs_dest == dest_local,
			      &cac->cac_skip_store);
}

/*
 * Returns OK if "type" supports compound operator "op_arg" (e.g. +=, -=, %=,
 * etc.).  Compound operators are not supported with a tuple and a dict.
 * Returns FAIL if compound operator is not supported.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
check_type_supports_compound_op(type_T *type, char_u op_arg)
{
    എങ്കിൽ (type->tt_type == VAR_TUPLE || type->tt_type == VAR_DICT)
    {
	char_u	op[2];

	op[0] = op_arg;
	op[1] = NUL;
	semsg(_(e_wrong_variable_type_for_str_equal), op);
	തിരികെ FAIL;
    }

    തിരികെ OK;
}

/*
 * Compile a compound op assignment statement (+=, -=, *=, %=, etc.)
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
compile_assign_compound_op(cctx_T *cctx, cac_T *cac)
{
    lhs_T	    *lhs = &cac->cac_lhs;
    type_T	    *expected;
    type_T	    *stacktype = ശൂന്യ;

    എങ്കിൽ (cac->cac_lhs.lhs_type->tt_type == VAR_TUPLE
	    && check_type_supports_compound_op(cac->cac_lhs.lhs_type,
							*cac->cac_op) == FAIL)
	തിരികെ FAIL;

    എങ്കിൽ (*cac->cac_op == '.')
    {
	expected = lhs->lhs_member_type;
	stacktype = get_type_on_അട്ടി(cctx, 0);
	എങ്കിൽ (expected != &t_string
		&& need_type(stacktype, expected, 0, -1, 0, cctx,
					FALSE, FALSE) == FAIL)
	    തിരികെ FAIL;
	അല്ലെങ്കിൽ എങ്കിൽ (may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
	    തിരികെ FAIL;
    }
    അല്ലെങ്കിൽ
    {
	expected = lhs->lhs_member_type;
	stacktype = get_type_on_അട്ടി(cctx, 0);

	എങ്കിൽ (check_type_supports_compound_op(expected, *cac->cac_op) == FAIL)
	    തിരികെ FAIL;

	എങ്കിൽ (
		// If variable is float operation with number is OK.
		!(expected == &t_float && (stacktype == &t_സംഖ്യ
					|| stacktype == &t_സംഖ്യ_bool))
		&& need_type(stacktype, expected, TYPECHK_സംഖ്യ_OK, -1, 0,
						cctx, FALSE, FALSE) == FAIL)
	    തിരികെ FAIL;
    }

    എങ്കിൽ (*cac->cac_op == '.')
    {
	എങ്കിൽ (generate_CONCAT(cctx, 2) == FAIL)
	    തിരികെ FAIL;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (*cac->cac_op == '+')
    {
	എങ്കിൽ (generate_add_instr(cctx,
		    operator_type(lhs->lhs_member_type, stacktype),
		    lhs->lhs_member_type, stacktype,
		    വ്യഞ്ജകം_APPEND) == FAIL)
	    തിരികെ FAIL;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (generate_two_op(cctx, cac->cac_op) == FAIL)
	തിരികെ FAIL;

    തിരികെ OK;
}

/*
 * Generate the STORE and SETTYPE instructions for an assignment statement.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
compile_assign_generate_store(cctx_T *cctx, cac_T *cac)
{
    lhs_T	*lhs = &cac->cac_lhs;
    പൂർണ്ണസംഖ്യ		save_lnum;

    // Use the line number of the assignment for store instruction.
    save_lnum = cctx->ctx_lnum;
    cctx->ctx_lnum = cac->cac_start_lnum - 1;

    എങ്കിൽ (lhs->lhs_has_index)
    {
	// Use the info in "lhs" to store the value at the index in the
	// list, dict or object.
	എങ്കിൽ (compile_assign_unlet(cac->cac_var_start, &cac->cac_lhs,
				 TRUE, cac->cac_rhs_type, cctx) == FAIL)
	{
	    cctx->ctx_lnum = save_lnum;
	    തിരികെ FAIL;
	}
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (cac->cac_is_decl && cac->cac_cmdidx == CMD_const &&
			(lhs->lhs_dest == dest_script
			 || lhs->lhs_dest == dest_script_v9
			 || lhs->lhs_dest == dest_global
			 || lhs->lhs_dest == dest_local))
	    // ":const var": lock the value, but not referenced variables
	    generate_LOCKCONST(cctx);

	type_T	*inferred_type = cac->cac_inferred_type;

	എങ്കിൽ ((lhs->lhs_type->tt_type == VAR_DICT
		    || lhs->lhs_type->tt_type == VAR_LIST)
		&& lhs->lhs_type->tt_member != ശൂന്യ
		&& lhs->lhs_type->tt_member != &t_any
		&& lhs->lhs_type->tt_member != &t_unknown)
	    // Set the type in the list or dict, so that it can be
	    // checked, also in legacy script.
	    generate_SETTYPE(cctx, lhs->lhs_type);
	അല്ലെങ്കിൽ എങ്കിൽ (lhs->lhs_type->tt_type == VAR_TUPLE
					&& lhs->lhs_type->tt_argcount != 0)
	    generate_SETTYPE(cctx, lhs->lhs_type);
	അല്ലെങ്കിൽ എങ്കിൽ (inferred_type != ശൂന്യ
		&& (inferred_type->tt_type == VAR_DICT
		    || inferred_type->tt_type == VAR_LIST)
		&& inferred_type->tt_member != ശൂന്യ
		&& inferred_type->tt_member != &t_unknown
		&& inferred_type->tt_member != &t_any)
	    // Set the type in the list or dict, so that it can be
	    // checked, also in legacy script.
	    generate_SETTYPE(cctx, inferred_type);
	അല്ലെങ്കിൽ എങ്കിൽ (inferred_type != ശൂന്യ
				&& inferred_type->tt_type == VAR_TUPLE
				&& inferred_type->tt_argcount > 0)
	    generate_SETTYPE(cctx, inferred_type);

	എങ്കിൽ (!cac->cac_skip_store &&
		generate_store_lhs(cctx, &cac->cac_lhs,
				   cac->cac_instr_എണ്ണം,
				   cac->cac_is_decl) == FAIL)
	{
	    cctx->ctx_lnum = save_lnum;
	    തിരികെ FAIL;
	}
    }

    cctx->ctx_lnum = save_lnum;

    തിരികെ OK;
}

/*
 * Process the variable(s) in an assignment statement
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
compile_assign_process_variables(
    cctx_T	*cctx,
    cac_T	*cac,
    പൂർണ്ണസംഖ്യ		cmdidx,
    പൂർണ്ണസംഖ്യ		heredoc,
    പൂർണ്ണസംഖ്യ		has_cmd,
    പൂർണ്ണസംഖ്യ		has_argisset_prefix,
    പൂർണ്ണസംഖ്യ		jump_instr_idx)
{
    /*
     * Loop over variables in "[var, var] = expr".
     * For "name = expr" and "var name: type" this is done only once.
     */
    വേണ്ടി (cac->cac_var_idx = 0; cac->cac_var_idx == 0 ||
	    cac->cac_var_idx < cac->cac_var_എണ്ണം; cac->cac_var_idx++)
    {
	എങ്കിൽ (cac->cac_var_start[0] == '_'
				&& !eval_isnamec(cac->cac_var_start[1]))
	{
	    // Ignore underscore in "[a, _, b] = list".
	    എങ്കിൽ (cac->cac_var_എണ്ണം > 0)
	    {
		cac->cac_var_start = skipwhite(cac->cac_var_start + 2);
		തുടരുക;
	    }
	    emsg(_(e_cannot_use_underscore_here));
	    തിരികെ FAIL;
	}
	vim_free(cac->cac_lhs.lhs_name);

	/*
	 * Figure out the LHS type and other properties.
	 */
	എങ്കിൽ (compile_assign_lhs(cac->cac_var_start, &cac->cac_lhs, cmdidx,
		    cac->cac_is_decl, heredoc, has_cmd,
		    cac->cac_oplen, cctx) == FAIL)
	    തിരികെ FAIL;

	// Compile the RHS expression
	എങ്കിൽ (heredoc)
	{
	    SOURCING_LNUM = cac->cac_start_lnum;
	    എങ്കിൽ (cac->cac_lhs.lhs_has_type
		    && need_type(&t_list_string, cac->cac_lhs.lhs_type,
				0, -1, 0, cctx, FALSE, FALSE) == FAIL)
		തിരികെ FAIL;
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (compile_assign_rhs(cctx, cac) == FAIL)
		തിരികെ FAIL;
	    എങ്കിൽ (cac->cac_var_എണ്ണം == 0)
		cac->cac_var_end = cac->cac_nextc;
	}

	// no need to parse more when skipping
	എങ്കിൽ (cctx->ctx_skip == SKIP_YES)
	    നിർത്തുക;

	എങ്കിൽ (cac->cac_oplen > 0 && *cac->cac_op != '=')
	{
	    എങ്കിൽ (compile_assign_compound_op(cctx, cac) == FAIL)
		തിരികെ FAIL;
	}

	// generate the store instructions
	എങ്കിൽ (compile_assign_generate_store(cctx, cac) == FAIL)
	    തിരികെ FAIL;

	എങ്കിൽ (cac->cac_var_idx + 1 < cac->cac_var_എണ്ണം)
	    cac->cac_var_start = skipwhite(cac->cac_lhs.lhs_end + 1);

	എങ്കിൽ (has_argisset_prefix)
	{
	    // set instruction index in JUMP_IF_ARG_SET to here
	    isn_T *isn = ((isn_T *)cac->cac_instr->ga_data) + jump_instr_idx;
	    isn->isn_arg.jumparg.jump_where = cac->cac_instr->ga_len;
	}
    }

    തിരികെ OK;
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
    സ്ഥിതം char_u *
compile_assignment(
    char_u	*arg_start,
    exarg_T	*eap,
    cmdidx_T	cmdidx,
    cctx_T	*cctx)
{
    cac_T	cac;
    char_u	*arg = arg_start;
    char_u	*retstr = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		heredoc = FALSE;
    പൂർണ്ണസംഖ്യ		jump_instr_idx;

    compile_assign_context_init(&cac, cctx, cmdidx, arg);

    jump_instr_idx = cac.cac_instr->ga_len;

    // process object variable initialization in a new() constructor method
    പൂർണ്ണസംഖ്യ	has_argisset_prefix = STRNCMP(arg, "ifargisset ", 11) == 0;
    എങ്കിൽ (has_argisset_prefix &&
			compile_assign_obj_new_arg(&arg, cctx) == FAIL)
	പോകുക theend;

    // Skip over the "varname" or "[varname, varname]" to get to any "=".
    cac.cac_nextc = skip_var_list(arg, TRUE, &cac.cac_var_എണ്ണം,
						&cac.cac_semicolon, TRUE);
    എങ്കിൽ (cac.cac_nextc == ശൂന്യ)
	തിരികെ *arg == '[' ? arg : ശൂന്യ;

    എങ്കിൽ (compile_assign_process_operator(eap, arg, &cac, &heredoc,
							&retstr) == FAIL)
	തിരികെ retstr;

    // Compute the start of the assignment
    cac.cac_var_start = compile_assign_compute_start(arg, cac.cac_var_എണ്ണം);

    // Compute the end of the assignment
    cac.cac_var_end = compile_assign_compute_end(eap, cctx, &cac, heredoc);
    എങ്കിൽ (cac.cac_var_end == ശൂന്യ)
	തിരികെ ശൂന്യ;

    പൂർണ്ണസംഖ്യ has_cmd = cac.cac_var_start > eap->cmd;

    /* process the variable(s) */
    എങ്കിൽ (compile_assign_process_variables(cctx, &cac, cmdidx, heredoc,
					 has_cmd, has_argisset_prefix,
					 jump_instr_idx) == FAIL)
	പോകുക theend;

    // For "[var, var] = expr" drop the "expr" value.
    // Also for "[var, var; _] = expr".
    എങ്കിൽ (cctx->ctx_skip != SKIP_YES && cac.cac_var_എണ്ണം > 0 &&
	    (!cac.cac_semicolon || !cac.cac_did_generate_slice))
    {
	എങ്കിൽ (generate_instr_drop(cctx, ISN_DROP, 1) == ശൂന്യ)
	    പോകുക theend;
    }

    retstr = skipwhite(cac.cac_var_end);

theend:
    vim_free(cac.cac_lhs.lhs_name);
    തിരികെ retstr;
}

/*
 * Check for an assignment at "eap->cmd", compile it if found.
 * Return NOTDONE if there is none, FAIL for failure, OK if done.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
may_compile_assignment(exarg_T *eap, char_u **വരി, cctx_T *cctx)
{
    char_u  *pskip;
    char_u  *p;

    // Assuming the command starts with a variable or function name,
    // find what follows.
    // Skip over "var.member", "var[idx]" and the like.
    // Also "&opt = val", "$ENV = val" and "@r = val".
    pskip = (*eap->cmd == '&' || *eap->cmd == '$' || *eap->cmd == '@')
						 ? eap->cmd + 1 : eap->cmd;
    p = to_name_end(pskip, TRUE);
    എങ്കിൽ (p > eap->cmd && *p != NUL)
    {
	char_u *var_end;
	പൂർണ്ണസംഖ്യ	oplen;
	പൂർണ്ണസംഖ്യ	heredoc;

	എങ്കിൽ (eap->cmd[0] == '@')
	    var_end = eap->cmd + 2;
	അല്ലെങ്കിൽ
	    var_end = find_name_end(pskip, ശൂന്യ, ശൂന്യ,
					FNE_CHECK_START | FNE_INCL_BR);
	oplen = assignment_len(skipwhite(var_end), &heredoc);
	എങ്കിൽ (oplen > 0)
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
	    എങ്കിൽ (*eap->cmd == '&'
		    || *eap->cmd == '$'
		    || *eap->cmd == '@'
		    || ((len) > 2 && eap->cmd[1] == ':')
		    || STRNCMP(eap->cmd, "g:[", 3) == 0
		    || variable_exists(eap->cmd, len, cctx))
	    {
		*വരി = compile_assignment(eap->cmd, eap, CMD_SIZE, cctx);
		എങ്കിൽ (*വരി == ശൂന്യ || *വരി == eap->cmd)
		    തിരികെ FAIL;
		തിരികെ OK;
	    }
	}
    }

    // might be "[var, var] = expr" or "ifargisset this.member = expr"
    എങ്കിൽ (*eap->cmd == '[' || STRNCMP(eap->cmd, "ifargisset ", 11) == 0)
    {
	*വരി = compile_assignment(eap->cmd, eap, CMD_SIZE, cctx);
	എങ്കിൽ (*വരി == ശൂന്യ)
	    തിരികെ FAIL;
	എങ്കിൽ (*വരി != eap->cmd)
	    തിരികെ OK;
    }
    തിരികെ NOTDONE;
}

/*
 * Check if arguments of "ufunc" shadow variables in "cctx".
 * Return OK or FAIL.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
check_args_shadowing(ufunc_T *ufunc, cctx_T *cctx)
{
    പൂർണ്ണസംഖ്യ	    i;
    char_u  *arg;
    പൂർണ്ണസംഖ്യ	    r = OK;

    // Make sure arguments are not found when compiling a second time.
    ufunc->uf_args_visible = 0;

    // Check for arguments shadowing variables from the context.
    വേണ്ടി (i = 0; i < ufunc->uf_args.ga_len; ++i)
    {
	arg = ((char_u **)(ufunc->uf_args.ga_data))[i];
	എങ്കിൽ (check_defined(arg, STRLEN(arg), cctx, ശൂന്യ, TRUE) == FAIL)
	{
	    r = FAIL;
	    നിർത്തുക;
	}
    }
    ufunc->uf_args_visible = ufunc->uf_args.ga_len;
    തിരികെ r;
}

#ifdef HAS_MESSAGE_WINDOW
/*
 * Get a count before a command.  Can only be a number.
 * Returns zero if there is no count.
 * Returns -1 if there is something wrong.
 */
    സ്ഥിതം long
get_cmd_എണ്ണം(char_u *വരി, exarg_T *eap)
{
    char_u *p;

    // skip over colons and white space
    വേണ്ടി (p = വരി; *p == ':' || VIM_ISWHITE(*p); ++p)
	;
    എങ്കിൽ (!SAFE_isdigit(*p))
    {
	// The command or modifiers must be following.  Assume a lower case
	// character means there is a modifier.
	എങ്കിൽ (p < eap->cmd && !vim_islower(*p))
	{
	    emsg(_(e_invalid_range));
	    തിരികെ -1;
	}
	തിരികെ 0;
    }
    തിരികെ atol((അക്ഷരം *)p);
}
#endif

/*
 * Get the compilation type that should be used for "ufunc".
 * Keep in sync with INSTRUCTIONS().
 */
    compiletype_T
get_compile_type(ufunc_T *ufunc)
{
    // Update uf_has_breakpoint if needed.
    update_has_breakpoint(ufunc);

    എങ്കിൽ (debug_break_level > 0 || may_break_in_function(ufunc))
	തിരികെ CT_DEBUG;
#ifdef FEAT_PROFILE
    എങ്കിൽ (do_profiling == PROF_YES)
    {
	എങ്കിൽ (!ufunc->uf_profiling && has_profiling(FALSE, ufunc->uf_name, ശൂന്യ,
							    &ufunc->uf_hash))
	    func_do_profile(ufunc);
	എങ്കിൽ (ufunc->uf_profiling)
	    തിരികെ CT_PROFILE;
    }
#endif
    തിരികെ CT_NONE;
}

/*
 * Free the compiled instructions saved for a def function.  This is used when
 * compiling a def function and the function was compiled before.
 * The index is reused.
 */
    സ്ഥിതം ശൂന്യം
clear_def_function(ufunc_T *ufunc, compiletype_T compile_type)
{
    isn_T	*instr_dest = ശൂന്യ;
    dfunc_T	*dfunc;

    dfunc = ((dfunc_T *)def_functions.ga_data) + ufunc->uf_dfunc_idx;

    തിരഞ്ഞെടുപ്പ് (compile_type)
    {
	അവസ്ഥ CT_PROFILE:
#ifdef FEAT_PROFILE
	    instr_dest = dfunc->df_instr_prof; നിർത്തുക;
#endif
	അവസ്ഥ CT_NONE:   instr_dest = dfunc->df_instr; നിർത്തുക;
	അവസ്ഥ CT_DEBUG:  instr_dest = dfunc->df_instr_debug; നിർത്തുക;
    }

    എങ്കിൽ (instr_dest != ശൂന്യ)
	// Was compiled in this mode before: Free old instructions.
	delete_def_function_contents(dfunc, FALSE);

    ga_clear_strings(&dfunc->df_var_names);
    dfunc->df_defer_var_idx = 0;
}

/*
 * Add a function to the list of :def functions.
 * This sets "ufunc->uf_dfunc_idx" but the function isn't compiled yet.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
add_def_function(ufunc_T *ufunc)
{
    dfunc_T *dfunc;

    എങ്കിൽ (def_functions.ga_len == 0)
    {
	// The first position is not used, so that a zero uf_dfunc_idx means it
	// wasn't set.
	എങ്കിൽ (GA_GROW_FAILS(&def_functions, 1))
	    തിരികെ FAIL;
	++def_functions.ga_len;
    }

    // Add the function to "def_functions".
    എങ്കിൽ (GA_GROW_FAILS(&def_functions, 1))
	തിരികെ FAIL;
    dfunc = ((dfunc_T *)def_functions.ga_data) + def_functions.ga_len;
    CLEAR_POINTER(dfunc);
    dfunc->df_idx = def_functions.ga_len;
    ufunc->uf_dfunc_idx = dfunc->df_idx;
    dfunc->df_ufunc = ufunc;
    dfunc->df_name = vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
    ga_init2(&dfunc->df_var_names, വലുപ്പം(char_u *), 10);
    ++dfunc->df_refcount;
    ++def_functions.ga_len;
    തിരികെ OK;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
compile_dfunc_ufunc_init(
    ufunc_T		*ufunc,
    cctx_T		*outer_cctx,
    compiletype_T	compile_type,
    പൂർണ്ണസംഖ്യ			*new_def_function)
{
    // When using a function that was compiled before: Free old instructions.
    // The index is reused.  Otherwise add a new entry in "def_functions".
    എങ്കിൽ (ufunc->uf_dfunc_idx > 0)
	clear_def_function(ufunc, compile_type);
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (add_def_function(ufunc) == FAIL)
	    തിരികെ FAIL;

	*new_def_function = TRUE;
    }

    എങ്കിൽ ((ufunc->uf_flags & FC_CLOSURE) && outer_cctx == ശൂന്യ)
    {
	semsg(_(e_compiling_closure_without_context_str),
						printable_func_name(ufunc));
	തിരികെ FAIL;
    }

    ufunc->uf_def_status = UF_COMPILING;

    തിരികെ OK;
}

/*
 * Initialize the compilation context for compiling a def function.
 */
    സ്ഥിതം ശൂന്യം
compile_dfunc_cctx_init(
    cctx_T		*cctx,
    cctx_T		*outer_cctx,
    ufunc_T		*ufunc,
    compiletype_T	compile_type)
{
    CLEAR_FIELD(*cctx);

    cctx->ctx_compile_type = compile_type;
    cctx->ctx_ufunc = ufunc;
    cctx->ctx_lnum = -1;
    cctx->ctx_outer = outer_cctx;
    ga_init2(&cctx->ctx_locals, വലുപ്പം(lvar_T), 10);
    // Each entry on the type stack consists of two type pointers.
    ga_init2(&cctx->ctx_type_അട്ടി, വലുപ്പം(type2_T), 50);
    cctx->ctx_type_list = &ufunc->uf_type_list;
    ga_init2(&cctx->ctx_instr, വലുപ്പം(isn_T), 50);
}

/*
 * For an object constructor, generate instruction to setup "this" (the first
 * local variable) and to initialize the object variables.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
obj_constructor_prologue(ufunc_T *ufunc, cctx_T *cctx)
{
    generate_CONSTRUCT(cctx, ufunc->uf_class);

    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < ufunc->uf_class->class_obj_member_എണ്ണം; ++i)
    {
	ocmember_T *m = &ufunc->uf_class->class_obj_members[i];

	എങ്കിൽ (i < 2 && IS_ENUM(ufunc->uf_class))
	    // The first two object variables in an enum are the name
	    // and the ordinal.  These are set by the ISN_CONSTRUCT
	    // instruction.  So don't generate instructions to set
	    // these variables.
	    തുടരുക;

	എങ്കിൽ (m->ocm_init != ശൂന്യ)
	{
	    char_u	*വ്യഞ്ജകം = m->ocm_init;
	    sctx_T	save_current_sctx;
	    പൂർണ്ണസംഖ്യ		change_sctx = FALSE;

	    // If the member variable initialization script context is
	    // different from the current script context, then change it.
	    എങ്കിൽ (current_sctx.sc_sid != m->ocm_init_sctx.sc_sid)
		change_sctx = TRUE;

	    എങ്കിൽ (change_sctx)
	    {
		// generate an instruction to change the script context to the
		// member variable initialization script context.
		save_current_sctx = current_sctx;
		current_sctx = m->ocm_init_sctx;
		generate_SCRIPTCTX_SET(cctx, current_sctx);
	    }

	    പൂർണ്ണസംഖ്യ r = compile_expr0(&വ്യഞ്ജകം, cctx);

	    എങ്കിൽ (change_sctx)
	    {
		// restore the previous script context
		current_sctx = save_current_sctx;
		generate_SCRIPTCTX_SET(cctx, current_sctx);
	    }

	    എങ്കിൽ (r == FAIL)
		തിരികെ FAIL;

	    എങ്കിൽ (!ends_excmd2(m->ocm_init, വ്യഞ്ജകം))
	    {
		semsg(_(e_trailing_characters_str), വ്യഞ്ജകം);
		തിരികെ FAIL;
	    }

	    type_T	*type = get_type_on_അട്ടി(cctx, 0);
	    എങ്കിൽ (m->ocm_type->tt_type == VAR_ANY
		    && !(m->ocm_flags & OCMFLAG_HAS_TYPE)
		    && type->tt_type != VAR_SPECIAL)
	    {
		// If the member variable type is not yet set, then use
		// the initialization expression type.
		m->ocm_type = type;
	    }
	    അല്ലെങ്കിൽ
	    {
		// The type of the member initialization expression is
		// determined at run time.  Add a runtime type check.
		where_T	where = WHERE_INIT;
		where.wt_kind = WT_MEMBER;
		where.wt_func_name = (അക്ഷരം *)m->ocm_name.string;
		എങ്കിൽ (need_type_where(type, m->ocm_type, 0, -1,
					where, cctx, FALSE, FALSE) == FAIL)
		    തിരികെ FAIL;
	    }
	}
	അല്ലെങ്കിൽ
	    തള്ളുക_default_മൂല്യം(cctx, m->ocm_type->tt_type, FALSE, ശൂന്യ);

	എങ്കിൽ (((m->ocm_type->tt_type == VAR_DICT
			|| m->ocm_type->tt_type == VAR_LIST)
		    && m->ocm_type->tt_member != ശൂന്യ
		    && m->ocm_type->tt_member != &t_any
		    && m->ocm_type->tt_member != &t_unknown)
		|| (m->ocm_type->tt_type == VAR_TUPLE
		    && m->ocm_type->tt_argcount > 0))
	    // Set the type in the list, tuple or dict, so that it can be
	    // checked, also in legacy script.
	    generate_SETTYPE(cctx, m->ocm_type);

	generate_STORE_THIS(cctx, i);
    }

    തിരികെ OK;
}

/*
 * For an object method and an constructor, generate instruction to setup
 * "this" (the first local variable).  For a constructor, generate instructions
 * to initialize the object variables.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
obj_method_prologue(ufunc_T *ufunc, cctx_T *cctx)
{
    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data) + ufunc->uf_dfunc_idx;

    എങ്കിൽ (GA_GROW_FAILS(&dfunc->df_var_names, 1))
	തിരികെ FAIL;

    ((char_u **)dfunc->df_var_names.ga_data)[0] =
						vim_strsave((char_u *)"this");
    ++dfunc->df_var_names.ga_len;

    // In the constructor allocate memory for the object and initialize the
    // object members.
    എങ്കിൽ (IS_CONSTRUCTOR_METHOD(ufunc))
	തിരികെ obj_constructor_prologue(ufunc, cctx);

    തിരികെ OK;
}

/*
 * Produce instructions for the default values of optional arguments.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
compile_def_function_default_args(
    ufunc_T	*ufunc,
    garray_T	*instr,
    cctx_T	*cctx)
{
    പൂർണ്ണസംഖ്യ	എണ്ണം = ufunc->uf_def_args.ga_len;
    പൂർണ്ണസംഖ്യ	first_def_arg = ufunc->uf_args.ga_len - എണ്ണം;
    പൂർണ്ണസംഖ്യ	i;
    പൂർണ്ണസംഖ്യ	off = അട്ടി_FRAME_SIZE + (ufunc->uf_va_name != ശൂന്യ ? 1 : 0);
    പൂർണ്ണസംഖ്യ	did_set_arg_type = FALSE;

    // Produce instructions for the default values of optional arguments.
    SOURCING_LNUM = 0;  // line number unknown
    വേണ്ടി (i = 0; i < എണ്ണം; ++i)
    {
	char_u *arg = ((char_u **)(ufunc->uf_def_args.ga_data))[i];
	എങ്കിൽ (STRCMP(arg, "v:none") == 0)
	    // "arg = v:none" means the argument is optional without
	    // setting a value when the argument is missing.
	    തുടരുക;

	type_T	*val_type;
	പൂർണ്ണസംഖ്യ		arg_idx = first_def_arg + i;
	where_T	where = WHERE_INIT;
	പൂർണ്ണസംഖ്യ		jump_instr_idx = instr->ga_len;
	isn_T	*isn;

	// Use a JUMP_IF_ARG_SET instruction to skip if the value was given.
	എങ്കിൽ (generate_JUMP_IF_ARG(cctx, ISN_JUMP_IF_ARG_SET,
						i - എണ്ണം - off) == FAIL)
	    തിരികെ FAIL;

	// Make sure later arguments are not found.
	ufunc->uf_args_visible = arg_idx;

	പൂർണ്ണസംഖ്യ r = compile_expr0(&arg, cctx);
	എങ്കിൽ (r == FAIL)
	    തിരികെ FAIL;

	// If no type specified use the type of the default value.
	// Otherwise check that the default value type matches the
	// specified type.
	val_type = get_type_on_അട്ടി(cctx, 0);
	where.wt_index = arg_idx + 1;
	where.wt_kind = WT_ARGUMENT;
	എങ്കിൽ (ufunc->uf_arg_types[arg_idx] == &t_unknown)
	{
	    did_set_arg_type = TRUE;
	    ufunc->uf_arg_types[arg_idx] = val_type;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (need_type_where(val_type, ufunc->uf_arg_types[arg_idx],
		    0, -1, where, cctx, FALSE, FALSE) == FAIL)
	    തിരികെ FAIL;

	എങ്കിൽ (generate_STORE(cctx, ISN_STORE, i - എണ്ണം - off, ശൂന്യ) == FAIL)
	    തിരികെ FAIL;

	// set instruction index in JUMP_IF_ARG_SET to here
	isn = ((isn_T *)instr->ga_data) + jump_instr_idx;
	isn->isn_arg.jumparg.jump_where = instr->ga_len;
    }

    എങ്കിൽ (did_set_arg_type)
	set_function_type(ufunc);

    തിരികെ OK;
}

/*
 * Compile def function body.  Loop over all the lines in the function and
 * generate instructions.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
compile_def_function_body(
    പൂർണ്ണസംഖ്യ		last_func_lnum,
    പൂർണ്ണസംഖ്യ		check_return_type,
    garray_T	*വരികൾ_to_free,
    അക്ഷരം	**errormsg,
    cctx_T	*cctx)
{
    char_u	*വരി = ശൂന്യ;
    char_u	*p;
    പൂർണ്ണസംഖ്യ		did_emsg_before = did_emsg;
#ifdef FEAT_PROFILE
    പൂർണ്ണസംഖ്യ		prof_lnum = -1;
#endif
    പൂർണ്ണസംഖ്യ		debug_lnum = -1;

    വേണ്ടി (;;)
    {
	exarg_T	    ea;
	പൂർണ്ണസംഖ്യ	    starts_with_colon = FALSE;
	char_u	    *cmd;
	cmdmod_T    local_cmdmod;

	// Bail out on the first error to avoid a flood of errors and report
	// the right line number when inside try/catch.
	എങ്കിൽ (did_emsg_before != did_emsg)
	    തിരികെ FAIL;

	എങ്കിൽ (വരി != ശൂന്യ && *വരി == '|')
	    // the line continues after a '|'
	    ++വരി;
	അല്ലെങ്കിൽ എങ്കിൽ (വരി != ശൂന്യ && *skipwhite(വരി) != NUL
		&& !(*വരി == '#' && (വരി == cctx->ctx_വരി_start
						    || VIM_ISWHITE(വരി[-1]))))
	{
	    semsg(_(e_trailing_characters_str), വരി);
	    തിരികെ FAIL;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (വരി != ശൂന്യ && vim9_bad_comment(skipwhite(വരി)))
	    തിരികെ FAIL;
	അല്ലെങ്കിൽ
	{
	    വരി = next_വരി_from_context(cctx, FALSE);
	    എങ്കിൽ (cctx->ctx_lnum >= last_func_lnum)
	    {
		// beyond the last line
#ifdef FEAT_PROFILE
		എങ്കിൽ (cctx->ctx_skip != SKIP_YES)
		    may_generate_prof_end(cctx, prof_lnum);
#endif
		നിർത്തുക;
	    }
	    // Make a copy, splitting off nextcmd and removing trailing spaces
	    // may change it.
	    എങ്കിൽ (വരി != ശൂന്യ)
	    {
		വരി = vim_strsave(വരി);
		എങ്കിൽ (ga_add_string(വരികൾ_to_free, വരി) == FAIL)
		{
		    vim_free(വരി);
		    തിരികെ FAIL;
		}
	    }
	}

	CLEAR_FIELD(ea);
	ea.cmdlinep = &വരി;
	ea.cmd = skipwhite(വരി);
	ea.skip = cctx->ctx_skip == SKIP_YES;

	എങ്കിൽ (*ea.cmd == '#')
	{
	    // "#" starts a comment, but "#{" is an error
	    എങ്കിൽ (vim9_bad_comment(ea.cmd))
		തിരികെ FAIL;
	    വരി = (char_u *)"";
	    തുടരുക;
	}

#ifdef FEAT_PROFILE
	എങ്കിൽ (cctx->ctx_compile_type == CT_PROFILE && cctx->ctx_lnum != prof_lnum
						  && cctx->ctx_skip != SKIP_YES)
	{
	    may_generate_prof_end(cctx, prof_lnum);

	    prof_lnum = cctx->ctx_lnum;
	    generate_instr(cctx, ISN_PROF_START);
	}
#endif
	എങ്കിൽ (cctx->ctx_compile_type == CT_DEBUG && cctx->ctx_lnum != debug_lnum
						  && cctx->ctx_skip != SKIP_YES)
	{
	    debug_lnum = cctx->ctx_lnum;
	    generate_instr_debug(cctx);
	}
	cctx->ctx_prev_lnum = cctx->ctx_lnum + 1;

	// Some things can be recognized by the first character.
	തിരഞ്ഞെടുപ്പ് (*ea.cmd)
	{
	    അവസ്ഥ '}':
		{
		    // "}" ends a block scope
		    scopetype_T stype = cctx->ctx_scope == ശൂന്യ
					  ? NO_SCOPE : cctx->ctx_scope->se_type;

		    എങ്കിൽ (stype == BLOCK_SCOPE)
		    {
			compile_endblock(cctx);
			വരി = ea.cmd;
		    }
		    അല്ലെങ്കിൽ
		    {
			emsg(_(e_using_rcurly_outside_if_block_scope));
			തിരികെ FAIL;
		    }
		    എങ്കിൽ (വരി != ശൂന്യ)
			വരി = skipwhite(ea.cmd + 1);
		    തുടരുക;
		}

	    അവസ്ഥ '{':
		// "{" starts a block scope
		// "{'a': 1}->func() is something else
		എങ്കിൽ (ends_excmd(*skipwhite(ea.cmd + 1)))
		{
		    വരി = compile_block(ea.cmd, cctx);
		    തുടരുക;
		}
		നിർത്തുക;
	}

	/*
	 * COMMAND MODIFIERS
	 */
	cctx->ctx_has_cmdmod = FALSE;
	എങ്കിൽ (parse_command_modifiers(&ea, errormsg, &local_cmdmod, FALSE)
								       == FAIL)
	    തിരികെ FAIL;
	generate_cmdmods(cctx, &local_cmdmod);
	undo_cmdmod(&local_cmdmod);

	// Check if there was a colon after the last command modifier or before
	// the current position.
	വേണ്ടി (p = ea.cmd; p >= വരി; --p)
	{
	    എങ്കിൽ (*p == ':')
		starts_with_colon = TRUE;
	    എങ്കിൽ (p < ea.cmd && !VIM_ISWHITE(*p))
		നിർത്തുക;
	}

	// Skip ":call" to get to the function name, unless using :legacy
	p = ea.cmd;
	എങ്കിൽ (!(local_cmdmod.cmod_flags & CMOD_LEGACY))
	{
	    എങ്കിൽ (checkforcmd(&ea.cmd, "call", 3))
	    {
		എങ്കിൽ (*ea.cmd == '(')
		    // not for "call()"
		    ea.cmd = p;
		അല്ലെങ്കിൽ
		    ea.cmd = skipwhite(ea.cmd);
	    }

	    എങ്കിൽ (!starts_with_colon)
	    {
		പൂർണ്ണസംഖ്യ	    assign;

		// Check for assignment after command modifiers.
		assign = may_compile_assignment(&ea, &വരി, cctx);
		എങ്കിൽ (assign == OK)
		    പോകുക nextline;
		എങ്കിൽ (assign == FAIL)
		    തിരികെ FAIL;
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
	എങ്കിൽ ((*cmd != '$' || starts_with_colon)
		&& (starts_with_colon
		    || !(*cmd == '\''
			|| (cmd[0] == '0' && cmd[1] == 'z')
			|| (cmd[0] != NUL && cmd[0] == cmd[1]
					    && (*cmd == '+' || *cmd == '-'))
			|| സംഖ്യ_method(cmd))))
	{
	    ea.cmd = skip_range(ea.cmd, TRUE, ശൂന്യ);
	    എങ്കിൽ (ea.cmd > cmd)
	    {
		എങ്കിൽ (!starts_with_colon
				   && !(local_cmdmod.cmod_flags & CMOD_LEGACY))
		{
		    semsg(_(e_colon_required_before_range_str), cmd);
		    തിരികെ FAIL;
		}
		ea.addr_എണ്ണം = 1;
		എങ്കിൽ (ends_excmd2(വരി, ea.cmd))
		{
		    // A range without a command: jump to the line.
		    generate_EXEC(cctx, ISN_EXECRANGE,
					      vim_strnsave(cmd, ea.cmd - cmd));
		    വരി = ea.cmd;
		    പോകുക nextline;
		}
	    }
	}
	p = find_ex_command(&ea, ശൂന്യ,
		starts_with_colon || (local_cmdmod.cmod_flags & CMOD_LEGACY)
						  ? ശൂന്യ : item_exists, cctx);

	എങ്കിൽ (p == ശൂന്യ)
	{
	    എങ്കിൽ (cctx->ctx_skip != SKIP_YES)
		semsg(_(e_ambiguous_use_of_user_defined_command_str), ea.cmd);
	    തിരികെ FAIL;
	}

	// When using ":legacy cmd" always use compile_exec().
	എങ്കിൽ (local_cmdmod.cmod_flags & CMOD_LEGACY)
	{
	    char_u *start = ea.cmd;

	    തിരഞ്ഞെടുപ്പ് (ea.cmdidx)
	    {
		അവസ്ഥ CMD_if:
		അവസ്ഥ CMD_elseif:
		അവസ്ഥ CMD_else:
		അവസ്ഥ CMD_endif:
		അവസ്ഥ CMD_for:
		അവസ്ഥ CMD_endfor:
		അവസ്ഥ CMD_continue:
		അവസ്ഥ CMD_break:
		അവസ്ഥ CMD_while:
		അവസ്ഥ CMD_endwhile:
		അവസ്ഥ CMD_try:
		അവസ്ഥ CMD_catch:
		അവസ്ഥ CMD_finally:
		അവസ്ഥ CMD_endtry:
			semsg(_(e_cannot_use_legacy_with_command_str), ea.cmd);
			തിരികെ FAIL;
		സ്ഥിരസ്ഥിതി: നിർത്തുക;
	    }

	    // ":legacy return expr" needs to be handled differently.
	    എങ്കിൽ (checkforcmd(&start, "return", 4))
		ea.cmdidx = CMD_return;
	    അല്ലെങ്കിൽ
		ea.cmdidx = CMD_legacy;
	}

	എങ്കിൽ (p == ea.cmd && ea.cmdidx != CMD_SIZE)
	{
	    // "eval" is used for "val->func()" and "var" for "var = val", then
	    // "p" is equal to "ea.cmd" for a valid command.
	    എങ്കിൽ (ea.cmdidx == CMD_eval || ea.cmdidx == CMD_var)
		;
	    അല്ലെങ്കിൽ എങ്കിൽ (cctx->ctx_skip == SKIP_YES)
	    {
		വരി += STRLEN(വരി);
		പോകുക nextline;
	    }
	    അല്ലെങ്കിൽ
	    {
		semsg(_(e_command_not_recognized_str), ea.cmd);
		തിരികെ FAIL;
	    }
	}

	എങ്കിൽ ((cctx->ctx_had_return || cctx->ctx_had_throw)
		&& ea.cmdidx != CMD_elseif
		&& ea.cmdidx != CMD_else
		&& ea.cmdidx != CMD_endif
		&& ea.cmdidx != CMD_endfor
		&& ea.cmdidx != CMD_endwhile
		&& ea.cmdidx != CMD_catch
		&& ea.cmdidx != CMD_finally
		&& ea.cmdidx != CMD_endtry
		&& !ignore_unreachable_code_for_testing)
	{
	    semsg(_(e_unreachable_code_after_str),
				     cctx->ctx_had_return ? "return" : "throw");
	    തിരികെ FAIL;
	}

	// When processing the end of an if-else or try-catch block, don't
	// clear the "ctx_had_throw" flag.  If such a block ends in a "throw"
	// statement, then it is considered to end in a "return" statement.
	// The "ctx_had_throw" is cleared immediately after processing the
	// block ending statement.
	// Otherwise, clear the "had_throw" flag.
	എങ്കിൽ (ea.cmdidx != CMD_else && ea.cmdidx != CMD_elseif
						&& ea.cmdidx != CMD_endif
						&& ea.cmdidx != CMD_catch
						&& ea.cmdidx != CMD_finally
						&& ea.cmdidx != CMD_endtry)
	    cctx->ctx_had_throw = FALSE;

	p = skipwhite(p);
	എങ്കിൽ (ea.cmdidx != CMD_SIZE
			    && ea.cmdidx != CMD_write && ea.cmdidx != CMD_read)
	{
	    എങ്കിൽ (ea.cmdidx >= 0)
		ea.argt = excmd_get_argt(ea.cmdidx);
	    എങ്കിൽ ((ea.argt & EX_BANG) && *p == '!')
	    {
		ea.forceit = TRUE;
		p = skipwhite(p + 1);
	    }
	    എങ്കിൽ ((ea.argt & EX_RANGE) == 0 && ea.addr_എണ്ണം > 0)
	    {
		emsg(_(e_no_range_allowed));
		തിരികെ FAIL;
	    }
	}

	തിരഞ്ഞെടുപ്പ് (ea.cmdidx)
	{
	    അവസ്ഥ CMD_def:
	    അവസ്ഥ CMD_function:
		    ea.arg = p;
		    വരി = compile_nested_function(&ea, cctx, വരികൾ_to_free);
		    നിർത്തുക;

	    അവസ്ഥ CMD_return:
		    വരി = compile_return(p, check_return_type,
				 local_cmdmod.cmod_flags & CMOD_LEGACY, cctx);
		    cctx->ctx_had_return = TRUE;
		    നിർത്തുക;

	    അവസ്ഥ CMD_let:
		    emsg(_(e_cannot_use_let_in_vim9_script));
		    നിർത്തുക;
	    അവസ്ഥ CMD_var:
	    അവസ്ഥ CMD_final:
	    അവസ്ഥ CMD_const:
	    അവസ്ഥ CMD_increment:
	    അവസ്ഥ CMD_decrement:
		    വരി = compile_assignment(p, &ea, ea.cmdidx, cctx);
		    എങ്കിൽ (വരി == p)
		    {
			emsg(_(e_invalid_assignment));
			വരി = ശൂന്യ;
		    }
		    നിർത്തുക;

	    അവസ്ഥ CMD_unlet:
	    അവസ്ഥ CMD_unlockvar:
	    അവസ്ഥ CMD_lockvar:
		    വരി = compile_unletlock(p, &ea, cctx);
		    നിർത്തുക;

	    അവസ്ഥ CMD_import:
		    emsg(_(e_import_can_only_be_used_in_script));
		    വരി = ശൂന്യ;
		    നിർത്തുക;

	    അവസ്ഥ CMD_if:
		    വരി = compile_if(p, cctx);
		    നിർത്തുക;
	    അവസ്ഥ CMD_elseif:
		    വരി = compile_elseif(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    cctx->ctx_had_throw = FALSE;
		    നിർത്തുക;
	    അവസ്ഥ CMD_else:
		    വരി = compile_else(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    cctx->ctx_had_throw = FALSE;
		    നിർത്തുക;
	    അവസ്ഥ CMD_endif:
		    വരി = compile_endif(p, cctx);
		    cctx->ctx_had_throw = FALSE;
		    നിർത്തുക;

	    അവസ്ഥ CMD_while:
		    വരി = compile_while(p, cctx);
		    നിർത്തുക;
	    അവസ്ഥ CMD_endwhile:
		    വരി = compile_endwhile(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    നിർത്തുക;

	    അവസ്ഥ CMD_for:
		    വരി = compile_for(p, cctx);
		    നിർത്തുക;
	    അവസ്ഥ CMD_endfor:
		    വരി = compile_endfor(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    നിർത്തുക;
	    അവസ്ഥ CMD_continue:
		    വരി = compile_continue(p, cctx);
		    // "continue" unconditionally jumps back to the loop,
		    // like a "return".
		    cctx->ctx_had_return = TRUE;
		    നിർത്തുക;
	    അവസ്ഥ CMD_break:
		    വരി = compile_break(p, cctx);
		    // "break" unconditionally exits the loop, like a "return".
		    cctx->ctx_had_return = TRUE;
		    നിർത്തുക;

	    അവസ്ഥ CMD_try:
		    വരി = compile_try(p, cctx);
		    നിർത്തുക;
	    അവസ്ഥ CMD_catch:
		    വരി = compile_catch(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    cctx->ctx_had_throw = FALSE;
		    നിർത്തുക;
	    അവസ്ഥ CMD_finally:
		    വരി = compile_finally(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    cctx->ctx_had_throw = FALSE;
		    നിർത്തുക;
	    അവസ്ഥ CMD_endtry:
		    വരി = compile_endtry(p, cctx);
		    cctx->ctx_had_throw = FALSE;
		    നിർത്തുക;
	    അവസ്ഥ CMD_throw:
		    വരി = compile_throw(p, cctx);
		    cctx->ctx_had_throw = TRUE;
		    നിർത്തുക;

	    അവസ്ഥ CMD_eval:
		    വരി = compile_eval(p, cctx);
		    നിർത്തുക;

	    അവസ്ഥ CMD_defer:
		    വരി = compile_defer(p, cctx);
		    നിർത്തുക;

#ifdef HAS_MESSAGE_WINDOW
	    അവസ്ഥ CMD_echowindow:
		    {
			long cmd_എണ്ണം = get_cmd_എണ്ണം(വരി, &ea);
			എങ്കിൽ (cmd_എണ്ണം < 0)
			    വരി = ശൂന്യ;
			അല്ലെങ്കിൽ
			    വരി = compile_mult_വ്യഞ്ജകം(p, ea.cmdidx,
							     cmd_എണ്ണം, cctx);
		    }
		    നിർത്തുക;
#endif
	    അവസ്ഥ CMD_echo:
	    അവസ്ഥ CMD_echon:
	    അവസ്ഥ CMD_echoconsole:
	    അവസ്ഥ CMD_echoerr:
	    അവസ്ഥ CMD_echomsg:
	    അവസ്ഥ CMD_execute:
		    വരി = compile_mult_വ്യഞ്ജകം(p, ea.cmdidx, 0, cctx);
		    നിർത്തുക;

	    അവസ്ഥ CMD_put:
		    ea.cmd = cmd;
		    വരി = compile_put(p, &ea, cctx, FALSE);
		    നിർത്തുക;

	    അവസ്ഥ CMD_iput:
		    ea.cmd = cmd;
		    വരി = compile_put(p, &ea, cctx, TRUE);
		    നിർത്തുക;

	    അവസ്ഥ CMD_substitute:
		    എങ്കിൽ (check_global_and_subst(ea.cmd, p) == FAIL)
			തിരികെ FAIL;
		    എങ്കിൽ (cctx->ctx_skip == SKIP_YES)
			വരി = (char_u *)"";
		    അല്ലെങ്കിൽ
		    {
			ea.arg = p;
			വരി = compile_substitute(വരി, &ea, cctx);
		    }
		    നിർത്തുക;

	    അവസ്ഥ CMD_redir:
		    ea.arg = p;
		    വരി = compile_redir(വരി, &ea, cctx);
		    നിർത്തുക;

	    അവസ്ഥ CMD_cexpr:
	    അവസ്ഥ CMD_lexpr:
	    അവസ്ഥ CMD_caddexpr:
	    അവസ്ഥ CMD_laddexpr:
	    അവസ്ഥ CMD_cgetexpr:
	    അവസ്ഥ CMD_lgetexpr:
#ifdef FEAT_QUICKFIX
		    ea.arg = p;
		    വരി = compile_cexpr(വരി, &ea, cctx);
#അല്ലെങ്കിൽ
		    ex_ni(&ea);
		    വരി = ശൂന്യ;
#endif
		    നിർത്തുക;

	    അവസ്ഥ CMD_append:
	    അവസ്ഥ CMD_change:
	    അവസ്ഥ CMD_തിരുകുക:
	    അവസ്ഥ CMD_k:
	    അവസ്ഥ CMD_open:
	    അവസ്ഥ CMD_t:
	    അവസ്ഥ CMD_xit:
		    not_in_vim9(&ea, false);
		    തിരികെ FAIL;

	    അവസ്ഥ CMD_SIZE:
		    എങ്കിൽ (cctx->ctx_skip != SKIP_YES)
		    {
			semsg(_(e_invalid_command_str), ea.cmd);
			തിരികെ FAIL;
		    }
		    // We don't check for a next command here.
		    വരി = (char_u *)"";
		    നിർത്തുക;

	    അവസ്ഥ CMD_lua:
	    അവസ്ഥ CMD_mzscheme:
	    അവസ്ഥ CMD_perl:
	    അവസ്ഥ CMD_py3:
	    അവസ്ഥ CMD_python3:
	    അവസ്ഥ CMD_python:
	    അവസ്ഥ CMD_pythonx:
	    അവസ്ഥ CMD_ruby:
	    അവസ്ഥ CMD_tcl:
		    ea.arg = p;
		    എങ്കിൽ (vim_strchr(വരി, '\n') == ശൂന്യ)
			വരി = compile_exec(വരി, &ea, cctx);
		    അല്ലെങ്കിൽ
			// heredoc lines have been concatenated with NL
			// characters in get_function_body()
			വരി = compile_script(വരി, cctx);
		    നിർത്തുക;

	    അവസ്ഥ CMD_vim9script:
		    എങ്കിൽ (cctx->ctx_skip != SKIP_YES)
		    {
			emsg(_(e_vim9script_can_only_be_used_in_script));
			തിരികെ FAIL;
		    }
		    വരി = (char_u *)"";
		    നിർത്തുക;

	    അവസ്ഥ CMD_class:
		    emsg(_(e_class_can_only_be_used_in_script));
		    തിരികെ FAIL;

	    അവസ്ഥ CMD_enum:
		    emsg(_(e_enum_can_only_be_used_in_script));
		    തിരികെ FAIL;

	    അവസ്ഥ CMD_interface:
		    emsg(_(e_interface_can_only_be_used_in_script));
		    തിരികെ FAIL;

	    അവസ്ഥ CMD_type:
		    emsg(_(e_type_can_only_be_used_in_script));
		    തിരികെ FAIL;

	    അവസ്ഥ CMD_global:
		    എങ്കിൽ (check_global_and_subst(ea.cmd, p) == FAIL)
			തിരികെ FAIL;
		    // FALLTHROUGH
	    സ്ഥിരസ്ഥിതി:
		    // Not recognized, execute with do_cmdline_cmd().
		    ea.arg = p;
		    വരി = compile_exec(വരി, &ea, cctx);
		    നിർത്തുക;
	}
nextline:
	എങ്കിൽ (വരി == ശൂന്യ)
	    തിരികെ FAIL;
	വരി = skipwhite(വരി);

	// Undo any command modifiers.
	generate_undo_cmdmods(cctx);

	എങ്കിൽ (cctx->ctx_type_അട്ടി.ga_len < 0)
	{
	    iemsg("Type stack underflow");
	    തിരികെ FAIL;
	}
    } // END of the loop over all the function body lines.

    തിരികെ OK;
}

/*
 * Returns TRUE if the end of a scope (if, while, for, block) is missing.
 * Called after compiling a def function body.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
compile_dfunc_scope_end_missing(cctx_T *cctx)
{
    എങ്കിൽ (cctx->ctx_scope == ശൂന്യ)
	തിരികെ FALSE;

    തിരഞ്ഞെടുപ്പ് (cctx->ctx_scope->se_type)
    {
	അവസ്ഥ IF_SCOPE:
	    emsg(_(e_missing_endif));
	    നിർത്തുക;
	അവസ്ഥ WHILE_SCOPE:
	    emsg(_(e_missing_endwhile));
	    നിർത്തുക;
	അവസ്ഥ FOR_SCOPE:
	    emsg(_(e_missing_endfor));
	    നിർത്തുക;
	അവസ്ഥ TRY_SCOPE:
	    emsg(_(e_missing_endtry));
	    നിർത്തുക;
	അവസ്ഥ BLOCK_SCOPE:
	    // end block scope from :try (maybe)
	    compile_endblock(cctx);
	    എങ്കിൽ (cctx->ctx_scope != ശൂന്യ
		    && cctx->ctx_scope->se_type == TRY_SCOPE)
		emsg(_(e_missing_endtry));
	    അല്ലെങ്കിൽ
		emsg(_(e_missing_rcurly));
	    നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
	    emsg(_(e_missing_rcurly));
    }
    തിരികെ TRUE;
}

/*
 * When compiling a def function, if it doesn't have an explicit return
 * statement, then generate a default return instruction.  For an object
 * constructor, return the object.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
compile_dfunc_generate_default_return(ufunc_T *ufunc, cctx_T *cctx)
{
    // TODO: if a function ends in "throw" but there was a return elsewhere we
    // should not assume the return type is "void".
    എങ്കിൽ (cctx->ctx_had_return || cctx->ctx_had_throw)
	തിരികെ OK;

    എങ്കിൽ (ufunc->uf_ret_type->tt_type == VAR_UNKNOWN)
	ufunc->uf_ret_type = &t_void;
    അല്ലെങ്കിൽ എങ്കിൽ (ufunc->uf_ret_type->tt_type != VAR_VOID
					&& !IS_CONSTRUCTOR_METHOD(ufunc))
    {
	emsg(_(e_missing_return_statement));
	തിരികെ FAIL;
    }

    // Return void if there is no return at the end.
    // For a constructor return the object.
    എങ്കിൽ (IS_CONSTRUCTOR_METHOD(ufunc))
    {
	generate_instr(cctx, ISN_RETURN_OBJECT);
	ufunc->uf_ret_type = &ufunc->uf_class->class_object_type;
    }
    അല്ലെങ്കിൽ
	generate_instr(cctx, ISN_RETURN_VOID);

    തിരികെ OK;
}

/*
 * Perform the chores after successfully compiling a def function.
 */
    സ്ഥിതം ശൂന്യം
compile_dfunc_epilogue(
    cctx_T	*outer_cctx,
    ufunc_T	*ufunc,
    garray_T	*instr,
    cctx_T	*cctx)
{
    dfunc_T	*dfunc;

    dfunc = ((dfunc_T *)def_functions.ga_data) + ufunc->uf_dfunc_idx;
    dfunc->df_deleted = FALSE;
    dfunc->df_script_seq = current_sctx.sc_seq;

#ifdef FEAT_PROFILE
    എങ്കിൽ (cctx->ctx_compile_type == CT_PROFILE)
    {
	dfunc->df_instr_prof = instr->ga_data;
	dfunc->df_instr_prof_എണ്ണം = instr->ga_len;
    }
    അല്ലെങ്കിൽ
#endif
	എങ്കിൽ (cctx->ctx_compile_type == CT_DEBUG)
	{
	    dfunc->df_instr_debug = instr->ga_data;
	    dfunc->df_instr_debug_എണ്ണം = instr->ga_len;
	}
	അല്ലെങ്കിൽ
	{
	    dfunc->df_instr = instr->ga_data;
	    dfunc->df_instr_എണ്ണം = instr->ga_len;
	}
    dfunc->df_varcount = dfunc->df_var_names.ga_len;
    dfunc->df_has_closure = cctx->ctx_has_closure;

    എങ്കിൽ (cctx->ctx_outer_used)
    {
	ufunc->uf_flags |= FC_CLOSURE;
	എങ്കിൽ (outer_cctx != ശൂന്യ)
	    ++outer_cctx->ctx_closure_എണ്ണം;
    }

    ufunc->uf_def_status = UF_COMPILED;
}

/*
 * Perform the cleanup when a def function compilation fails.
 */
    സ്ഥിതം ശൂന്യം
compile_dfunc_ufunc_cleanup(
    ufunc_T	*ufunc,
    garray_T	*instr,
    പൂർണ്ണസംഖ്യ		new_def_function,
    അക്ഷരം	*errormsg,
    പൂർണ്ണസംഖ്യ		did_emsg_before,
    cctx_T	*cctx)
{
    dfunc_T	*dfunc;

    dfunc = ((dfunc_T *)def_functions.ga_data) + ufunc->uf_dfunc_idx;

    // Compiling aborted, free the generated instructions.
    clear_instr_ga(instr);
    VIM_CLEAR(dfunc->df_name);
    ga_clear_strings(&dfunc->df_var_names);

    // If using the last entry in the table and it was added above, we
    // might as well remove it.
    എങ്കിൽ (!dfunc->df_deleted && new_def_function
			&& ufunc->uf_dfunc_idx == def_functions.ga_len - 1)
    {
	--def_functions.ga_len;
	ufunc->uf_dfunc_idx = 0;
    }
    ufunc->uf_def_status = UF_COMPILE_ERROR;

    ആകുമ്പോൾ (cctx->ctx_scope != ശൂന്യ)
	drop_scope(cctx);

    എങ്കിൽ (errormsg != ശൂന്യ)
	emsg(errormsg);
    അല്ലെങ്കിൽ എങ്കിൽ (did_emsg == did_emsg_before)
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
    പൂർണ്ണസംഖ്യ
compile_def_function(
	ufunc_T		*ufunc,
	പൂർണ്ണസംഖ്യ		check_return_type,
	compiletype_T   compile_type,
	cctx_T		*outer_cctx)
{
    garray_T	വരികൾ_to_free;
    അക്ഷരം	*errormsg = ശൂന്യ;	// error message
    cctx_T	cctx;
    garray_T	*instr;
    പൂർണ്ണസംഖ്യ		did_emsg_before = did_emsg;
    പൂർണ്ണസംഖ്യ		did_emsg_silent_before = did_emsg_silent;
    പൂർണ്ണസംഖ്യ		ret = FAIL;
    sctx_T	save_current_sctx = current_sctx;
    പൂർണ്ണസംഖ്യ		save_estack_compiling = estack_compiling;
    പൂർണ്ണസംഖ്യ		save_cmod_flags = cmdmod.cmod_flags;
    പൂർണ്ണസംഖ്യ		do_estack_തള്ളുക;
    പൂർണ്ണസംഖ്യ		new_def_function = FALSE;

    // allocated lines are freed at the end
    ga_init2(&വരികൾ_to_free, വലുപ്പം(char_u *), 50);

    // Initialize the ufunc and the compilation context
    എങ്കിൽ (compile_dfunc_ufunc_init(ufunc, outer_cctx, compile_type,
						&new_def_function) == FAIL)
	തിരികെ FAIL;

    compile_dfunc_cctx_init(&cctx, outer_cctx, ufunc, compile_type);

    instr = &cctx.ctx_instr;

    // Set the context to the function, it may be compiled when called from
    // another script.  Set the script version to the most modern one.
    // The line number will be set in next_line_from_context().
    current_sctx = ufunc->uf_script_ctx;
    current_sctx.sc_version = SCRIPT_VERSION_VIM9;

    // Don't use the flag from ":legacy" here.
    cmdmod.cmod_flags &= ~CMOD_LEGACY;

    // Make sure error messages are OK.
    do_estack_തള്ളുക = !estack_മുകൾ_is_ufunc(ufunc, 1);
    എങ്കിൽ (do_estack_തള്ളുക)
	estack_തള്ളുക_ufunc(ufunc, 1);
    estack_compiling = TRUE;

    // Make sure arguments don't shadow variables in the context
    എങ്കിൽ (check_args_shadowing(ufunc, &cctx) == FAIL)
	പോകുക erret;

    // For an object method and a constructor generate instructions to
    // initialize "this" and the object variables.
    എങ്കിൽ (ufunc->uf_flags & (FC_OBJECT|FC_NEW))
	എങ്കിൽ (obj_method_prologue(ufunc, &cctx) == FAIL)
	    പോകുക erret;

    എങ്കിൽ (ufunc->uf_def_args.ga_len > 0)
	എങ്കിൽ (compile_def_function_default_args(ufunc, instr, &cctx) == FAIL)
	    പോകുക erret;
    ufunc->uf_args_visible = ufunc->uf_args.ga_len;

    // Compiling an abstract method or a function in an interface is done to
    // get the function type.  No code is actually compiled.
    എങ്കിൽ (ufunc->uf_class != ശൂന്യ && (IS_INTERFACE(ufunc->uf_class)
						|| IS_ABSTRACT_METHOD(ufunc)))
    {
	ufunc->uf_def_status = UF_NOT_COMPILED;
	ret = OK;
	പോകുക erret;
    }

    // compile the function body
    എങ്കിൽ (compile_def_function_body(ufunc->uf_വരികൾ.ga_len, check_return_type,
				&വരികൾ_to_free, &errormsg, &cctx) == FAIL)
	പോകുക erret;

    എങ്കിൽ (compile_dfunc_scope_end_missing(&cctx))
	പോകുക erret;

    എങ്കിൽ (compile_dfunc_generate_default_return(ufunc, &cctx) == FAIL)
	പോകുക erret;

    // When compiled with ":silent!" and there was an error don't consider the
    // function compiled.
    എങ്കിൽ (emsg_silent == 0 || did_emsg_silent == did_emsg_silent_before)
	compile_dfunc_epilogue(outer_cctx, ufunc, instr, &cctx);

    ret = OK;

erret:
    എങ്കിൽ (ufunc->uf_def_status == UF_COMPILING)
    {
	// compilation failed. do cleanup.
	compile_dfunc_ufunc_cleanup(ufunc, instr, new_def_function,
				    errormsg, did_emsg_before, &cctx);
    }

    എങ്കിൽ (cctx.ctx_redir_lhs.lhs_name != ശൂന്യ)
    {
	എങ്കിൽ (ret == OK)
	{
	    emsg(_(e_missing_redir_end));
	    ret = FAIL;
	}
	vim_free(cctx.ctx_redir_lhs.lhs_name);
	vim_free(cctx.ctx_redir_lhs.lhs_whole);
    }

    current_sctx = save_current_sctx;
    estack_compiling = save_estack_compiling;
    cmdmod.cmod_flags =	save_cmod_flags;
    എങ്കിൽ (do_estack_തള്ളുക)
	estack_എടുക്കുക();

    ga_clear_strings(&വരികൾ_to_free);
    free_locals(&cctx);
    ga_clear(&cctx.ctx_type_അട്ടി);
    തിരികെ ret;
}

    ശൂന്യം
set_function_type(ufunc_T *ufunc)
{
    പൂർണ്ണസംഖ്യ varargs = ufunc->uf_va_name != ശൂന്യ;
    പൂർണ്ണസംഖ്യ argcount = ufunc->uf_args.ga_len;

    // Create a type for the function, with the return type and any
    // argument types.
    // A vararg is included in uf_args.ga_len but not in uf_arg_types.
    // The type is included in "tt_args".
    എങ്കിൽ (argcount > 0 || varargs)
    {
	എങ്കിൽ (ufunc->uf_type_list.ga_itemsize == 0)
	    ga_init2(&ufunc->uf_type_list, വലുപ്പം(type_T *), 10);
	ufunc->uf_func_type = alloc_func_type(ufunc->uf_ret_type,
					   argcount, &ufunc->uf_type_list);
	// Add argument types to the function type.
	എങ്കിൽ (func_type_add_arg_types(ufunc->uf_func_type,
				    argcount + varargs,
				    &ufunc->uf_type_list) == FAIL)
	    തിരികെ;
	ufunc->uf_func_type->tt_argcount = argcount + varargs;
	ufunc->uf_func_type->tt_min_argcount =
				      argcount - ufunc->uf_def_args.ga_len;
	എങ്കിൽ (ufunc->uf_arg_types == ശൂന്യ)
	{
	    പൂർണ്ണസംഖ്യ i;

	    // lambda does not have argument types.
	    വേണ്ടി (i = 0; i < argcount; ++i)
		ufunc->uf_func_type->tt_args[i] = &t_any;
	}
	അല്ലെങ്കിൽ
	    mch_memmove(ufunc->uf_func_type->tt_args,
			 ufunc->uf_arg_types, വലുപ്പം(type_T *) * argcount);
	എങ്കിൽ (varargs)
	{
	    ufunc->uf_func_type->tt_args[argcount] =
		   ufunc->uf_va_type == ശൂന്യ ? &t_list_any : ufunc->uf_va_type;
	    ufunc->uf_func_type->tt_flags = TTFLAG_VARARGS;
	}
    }
    അല്ലെങ്കിൽ
	// No arguments, can use a predefined type.
	ufunc->uf_func_type = get_func_type(ufunc->uf_ret_type,
					   argcount, &ufunc->uf_type_list);
}

/*
 * Free all instructions for "dfunc" except df_name.
 */
    സ്ഥിതം ശൂന്യം
delete_def_function_contents(dfunc_T *dfunc, പൂർണ്ണസംഖ്യ mark_deleted)
{
    പൂർണ്ണസംഖ്യ idx;

    // In same cases the instructions may refer to a class in which the
    // function is defined and unreferencing the class may call back here
    // recursively.  Set the df_delete_busy to avoid problems.
    എങ്കിൽ (dfunc->df_delete_busy)
	തിരികെ;
    dfunc->df_delete_busy = TRUE;

    ga_clear(&dfunc->df_def_args_isn);
    ga_clear_strings(&dfunc->df_var_names);

    എങ്കിൽ (dfunc->df_instr != ശൂന്യ)
    {
	വേണ്ടി (idx = 0; idx < dfunc->df_instr_എണ്ണം; ++idx)
	    delete_instr(dfunc->df_instr + idx);
	VIM_CLEAR(dfunc->df_instr);
    }
    എങ്കിൽ (dfunc->df_instr_debug != ശൂന്യ)
    {
	വേണ്ടി (idx = 0; idx < dfunc->df_instr_debug_എണ്ണം; ++idx)
	    delete_instr(dfunc->df_instr_debug + idx);
	VIM_CLEAR(dfunc->df_instr_debug);
    }
#ifdef FEAT_PROFILE
    എങ്കിൽ (dfunc->df_instr_prof != ശൂന്യ)
    {
	വേണ്ടി (idx = 0; idx < dfunc->df_instr_prof_എണ്ണം; ++idx)
	    delete_instr(dfunc->df_instr_prof + idx);
	VIM_CLEAR(dfunc->df_instr_prof);
    }
#endif

    എങ്കിൽ (mark_deleted)
	dfunc->df_deleted = TRUE;
    എങ്കിൽ (dfunc->df_ufunc != ശൂന്യ)
	dfunc->df_ufunc->uf_def_status = UF_NOT_COMPILED;

    dfunc->df_delete_busy = FALSE;
}

/*
 * When a user function is deleted, clear the contents of any associated def
 * function, unless another user function still uses it.
 * The position in def_functions can be re-used.
 */
    ശൂന്യം
unlink_def_function(ufunc_T *ufunc)
{
    എങ്കിൽ (ufunc->uf_dfunc_idx <= 0 || def_functions.ga_data == ശൂന്യ)
	തിരികെ;

    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data)
	+ ufunc->uf_dfunc_idx;

    എങ്കിൽ (--dfunc->df_refcount <= 0)
	delete_def_function_contents(dfunc, TRUE);
    ufunc->uf_def_status = UF_NOT_COMPILED;
    ufunc->uf_dfunc_idx = 0;
    എങ്കിൽ (dfunc->df_ufunc == ufunc)
	dfunc->df_ufunc = ശൂന്യ;
}

/*
 * Used when a user function refers to an existing dfunc.
 */
    ശൂന്യം
link_def_function(ufunc_T *ufunc)
{
    എങ്കിൽ (ufunc->uf_dfunc_idx <= 0)
	തിരികെ;

    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data)
	+ ufunc->uf_dfunc_idx;

    ++dfunc->df_refcount;
}

#എങ്കിൽ defined(EXITFREE)
/*
 * Free all functions defined with ":def".
 */
    ശൂന്യം
free_def_functions(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ idx;

    വേണ്ടി (idx = 0; idx < def_functions.ga_len; ++idx)
    {
	dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data) + idx;

	delete_def_function_contents(dfunc, TRUE);
	vim_free(dfunc->df_name);
    }

    ga_clear(&def_functions);
}
#endif


#endif // FEAT_EVAL
