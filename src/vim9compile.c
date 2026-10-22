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
#অন্তর্ভাব "vim.h"

#যদি defined(FEAT_EVAL)

// Functions defined with :def are stored in this growarray.
// They are never removed, so that they can be found by index.
// Deleted functions have the df_deleted flag set.
garray_T def_functions = {0, 0, আকার(dfunc_T), 50, ফাঁকা};

স্থির শূন্য delete_def_function_contents(dfunc_T *dfunc, পূর্ণসংখ্যা mark_deleted);

/*
 * Lookup variable "name" in the local scope and return it in "lvar".
 * "lvar->lv_from_outer" is incremented accordingly.
 * If "lvar" is NULL only check if the variable can be found.
 * Return FAIL if not found.
 */
    পূর্ণসংখ্যা
lookup_local(char_u *name, size_t len, lvar_T *lvar, cctx_T *cctx)
{
    পূর্ণসংখ্যা	    idx;
    lvar_T  *lvp;

    যদি (len == 0)
	ফেরত FAIL;

    যদি (((len == 4 && STRNCMP(name, "this", 4) == 0)
		|| (len == 5 && STRNCMP(name, "super", 5) == 0))
	    && cctx->ctx_ufunc != ফাঁকা
	    && (cctx->ctx_ufunc->uf_flags & (FC_OBJECT|FC_NEW)))
    {
	পূর্ণসংখ্যা is_super = *name == 's';
	যদি (is_super)
	{
	    যদি (name[5] != '.')
	    {
		emsg(_(e_super_must_be_followed_by_dot));
		ফেরত FAIL;
	    }
	    যদি (cctx->ctx_ufunc->uf_class != ফাঁকা
		    && cctx->ctx_ufunc->uf_class->class_extends == ফাঁকা)
	    {
		emsg(_(e_using_super_not_in_child_class));
		ফেরত FAIL;
	    }
	}
	যদি (lvar != ফাঁকা)
	{
	    CLEAR_POINTER(lvar);
	    lvar->lv_loop_depth = -1;
	    lvar->lv_name = (char_u *)(is_super ? "super" : "this");
	    যদি (cctx->ctx_ufunc->uf_class != ফাঁকা)
	    {
		lvar->lv_type = &cctx->ctx_ufunc->uf_class->class_object_type;
		যদি (is_super)
		{
		    type_T *type = get_type_ptr(cctx->ctx_type_list);

		    যদি (type != ফাঁকা)
		    {
			*type = *lvar->lv_type;
			lvar->lv_type = type;
			type->tt_flags |= TTFLAG_SUPER;
		    }
		}
	    }
	}
	ফেরত OK;
    }

    // Find local in current function scope.
    জন্য (idx = 0; idx < cctx->ctx_locals.ga_len; ++idx)
    {
	lvp = ((lvar_T *)cctx->ctx_locals.ga_তথ্য) + idx;
	যদি (lvp->lv_name != ফাঁকা
		&& STRNCMP(name, lvp->lv_name, len) == 0
					       && STRLEN(lvp->lv_name) == len)
	{
	    যদি (lvar != ফাঁকা)
	    {
		*lvar = *lvp;
		lvar->lv_from_outer = 0;
		// If the variable was declared inside a loop set
		// lvar->lv_loop_idx and lvar->lv_loop_depth.
		get_loop_var_idx(cctx, idx, lvar);
	    }
	    ফেরত OK;
	}
    }

    // Find local in outer function scope.
    যদি (cctx->ctx_outer != ফাঁকা)
    {
	যদি (lookup_local(name, len, lvar, cctx->ctx_outer) == OK)
	{
	    যদি (lvar != ফাঁকা)
	    {
		cctx->ctx_outer_used = TRUE;
		++lvar->lv_from_outer;
	    }
	    ফেরত OK;
	}
    }

    ফেরত FAIL;
}

/*
 * Lookup an argument in the current function and an enclosing function.
 * Returns the argument index in "idxp"
 * Returns the argument type in "type"
 * Sets "gen_load_outer" to TRUE if found in outer scope.
 * Returns OK when found, FAIL otherwise.
 */
    পূর্ণসংখ্যা
arg_exists(
	char_u	*name,
	size_t	len,
	পূর্ণসংখ্যা	*idxp,
	type_T	**type,
	পূর্ণসংখ্যা	*gen_load_outer,
	cctx_T	*cctx)
{
    পূর্ণসংখ্যা	    idx;
    char_u  *va_name;

    যদি (len == 0)
	ফেরত FAIL;
    জন্য (idx = 0; idx < cctx->ctx_ufunc->uf_args_visible; ++idx)
    {
	char_u *arg = FUNCARG(cctx->ctx_ufunc, idx);

	যদি (STRNCMP(name, arg, len) == 0 && arg[len] == NUL)
	{
	    যদি (idxp != ফাঁকা)
	    {
		// Arguments are located above the frame pointer.  One further
		// if there is a vararg argument
		*idxp = idx - (cctx->ctx_ufunc->uf_args.ga_len
							    + স্তূপ_FRAME_আকার)
			      + (cctx->ctx_ufunc->uf_va_name != ফাঁকা ? -1 : 0);

		যদি (cctx->ctx_ufunc->uf_arg_types != ফাঁকা)
		    *type = cctx->ctx_ufunc->uf_arg_types[idx];
		অন্যথা
		    *type = &t_any;
	    }
	    ফেরত OK;
	}
    }

    va_name = cctx->ctx_ufunc->uf_va_name;
    যদি (va_name != ফাঁকা
		    && STRNCMP(name, va_name, len) == 0 && va_name[len] == NUL)
    {
	যদি (idxp != ফাঁকা)
	{
	    // varargs is always the last argument
	    *idxp = -স্তূপ_FRAME_আকার - 1;
	    *type = cctx->ctx_ufunc->uf_va_type;
	}
	ফেরত OK;
    }

    যদি (cctx->ctx_outer != ফাঁকা)
    {
	// Lookup the name for an argument of the outer function.
	যদি (arg_exists(name, len, idxp, type, gen_load_outer, cctx->ctx_outer)
									 == OK)
	{
	    যদি (gen_load_outer != ফাঁকা)
	    {
		++*gen_load_outer;
		cctx->ctx_outer_used = TRUE;
	    }
	    ফেরত OK;
	}
    }

    ফেরত FAIL;
}

/*
 * Lookup a script-local variable in the current script, possibly defined in a
 * block that contains the function "cctx->ctx_ufunc".
 * "cctx" is NULL at the script level, "cstack" is NULL in a function.
 * If "len" is <= 0 "name" must be NUL terminated.
 * Return NULL when not found.
 */
    স্থির sallvar_T *
find_script_var(char_u *name, size_t len, cctx_T *cctx, cstack_T *cstack)
{
    scriptitem_T    *si = SCRIPT_ITEM(current_sctx.sc_sid);
    hashitem_T	    *hi;
    পূর্ণসংখ্যা		    cc;
    sallvar_T	    *sav;
    ufunc_T	    *ufunc;

    // Find the list of all script variables with the right name.
    যদি (len > 0)
    {
	cc = name[len];
	name[len] = NUL;
    }
    hi = hash_find(&si->sn_all_vars.dv_hashtab, name);
    যদি (len > 0)
	name[len] = cc;
    যদি (HASHITEM_EMPTY(hi))
	ফেরত ফাঁকা;

    sav = HI2SAV(hi);
    যদি (sav->sav_block_id == 0)
	// variable defined in the top script scope is always visible
	ফেরত sav;

    যদি (cctx == ফাঁকা)
    {
	যদি (cstack == ফাঁকা)
	    ফেরত ফাঁকা;

	// Not in a function scope, find variable with block ID equal to or
	// smaller than the current block id.  Use "cstack" to go up the block
	// scopes.
	যখন (sav != ফাঁকা)
	{
	    পূর্ণসংখ্যা idx;

	    জন্য (idx = cstack->cs_idx; idx >= 0; --idx)
		যদি (cstack->cs_block_id[idx] == sav->sav_block_id)
		    বিরতি;
	    যদি (idx >= 0)
		বিরতি;
	    sav = sav->sav_পরবর্তী;
	}
	ফেরত sav;
    }

    // Go over the variables with this name and find one that was visible
    // from the function.
    ufunc = cctx->ctx_ufunc;
    যখন (sav != ফাঁকা)
    {
	পূর্ণসংখ্যা idx;

	যদি (ufunc->uf_block_depth == 0 && sav->sav_block_id == 0)
	    // If the function was defined at the script level (not inside a
	    // block), script-scope variables are always visible.
	    ফেরত sav;

	// Go over the blocks that this function was defined in.  If the
	// variable block ID matches it was visible to the function.
	জন্য (idx = 0; idx < ufunc->uf_block_depth; ++idx)
	    যদি (ufunc->uf_block_ids[idx] == sav->sav_block_id)
		ফেরত sav;
	sav = sav->sav_পরবর্তী;
    }

    // Not found, variable was not visible.
    ফেরত ফাঁকা;
}

/*
 * If "name" can be found in the current script set it's "block_id".
 */
    শূন্য
update_script_var_block_id(char_u *name, পূর্ণসংখ্যা block_id)
{
    scriptitem_T    *si = SCRIPT_ITEM(current_sctx.sc_sid);
    hashitem_T	    *hi;
    sallvar_T	    *sav;

    hi = hash_find(&si->sn_all_vars.dv_hashtab, name);
    যদি (HASHITEM_EMPTY(hi))
	ফেরত;
    sav = HI2SAV(hi);
    sav->sav_block_id = block_id;
}

/*
 * Lookup a variable (without s: prefix) in the current script.
 * "cctx" is NULL at the script level, "cstack" is NULL in a function.
 * Returns OK or FAIL.
 */
    পূর্ণসংখ্যা
script_var_exists(char_u *name, size_t len, cctx_T *cctx, cstack_T *cstack)
{
    যদি (current_sctx.sc_sid <= 0)
	ফেরত FAIL;
    যদি (current_script_is_vim9())
    {
	// Check script variables that were visible where the function was
	// defined.
	যদি (find_script_var(name, len, cctx, cstack) != ফাঁকা)
	    ফেরত OK;
    }
    অন্যথা
    {
	hashtab_T	*ht = &SCRIPT_VARS(current_sctx.sc_sid);
	dictitem_T	*di;
	পূর্ণসংখ্যা		cc;

	// Check script variables that are currently visible
	cc = name[len];
	name[len] = NUL;
	di = find_var_in_ht(ht, 0, name, TRUE);
	name[len] = cc;
	যদি (di != ফাঁকা)
	    ফেরত OK;
    }

    ফেরত FAIL;
}

/*
 * Returns the index of a class method or class variable with name "name"
 * accessible in the currently compiled function.
 * If "cl_ret" is not NULL set it to the class.
 * Otherwise return -1.
 */
    স্থির পূর্ণসংখ্যা
cctx_class_midx(
    cctx_T  *cctx,
    পূর্ণসংখ্যা	    is_method,
    char_u  *name,
    size_t  len,
    class_T **cl_ret)
{
    যদি (cctx == ফাঁকা || cctx->ctx_ufunc == ফাঁকা
	    || cctx->ctx_ufunc->uf_class == ফাঁকা
	    || cctx->ctx_ufunc->uf_defclass == ফাঁকা)
	ফেরত -1;

    // Search for the class method or variable in the class where the calling
    // function is defined.
    class_T *cl = cctx->ctx_ufunc->uf_defclass;
    পূর্ণসংখ্যা m_idx = is_method ? class_method_idx(cl, name, len)
					: class_member_idx(cl, name, len);
    যদি (m_idx < 0)
    {
	cl = cl->class_extends;
	যখন (cl != ফাঁকা)
	{
	    m_idx = is_method ? class_method_idx(cl, name, len)
					: class_member_idx(cl, name, len);
	    যদি (m_idx >= 0)
		বিরতি;
	    cl = cl->class_extends;
	}
    }

    যদি (m_idx >= 0)
    {
	যদি (cl_ret != ফাঁকা)
	    *cl_ret = cl;
    }

    ফেরত m_idx;
}

/*
 * Returns the index of a class method with name "name" accessible in the
 * currently compiled function.  Returns -1 if not found.  The class where the
 * method is defined is returned in "cl_ret".
 */
    পূর্ণসংখ্যা
cctx_class_method_idx(
    cctx_T  *cctx,
    char_u  *name,
    size_t  len,
    class_T **cl_ret)
{
    ফেরত cctx_class_midx(cctx, TRUE, name, len, cl_ret);
}

/*
 * Returns the index of a class variable with name "name" accessible in the
 * currently compiled function.  Returns -1 if not found.  The class where the
 * variable is defined is returned in "cl_ret".
 */
    পূর্ণসংখ্যা
cctx_class_member_idx(
    cctx_T  *cctx,
    char_u  *name,
    size_t  len,
    class_T **cl_ret)
{
    ফেরত cctx_class_midx(cctx, FALSE, name, len, cl_ret);
}

/*
 * Return TRUE if "name" is a local variable, argument, script variable or
 * imported.  Also if "name" is "this" and in a class method.
 */
    স্থির পূর্ণসংখ্যা
variable_exists(char_u *name, size_t len, cctx_T *cctx)
{
    ফেরত (cctx != ফাঁকা
		&& (lookup_local(name, len, ফাঁকা, cctx) == OK
		    || arg_exists(name, len, ফাঁকা, ফাঁকা, ফাঁকা, cctx) == OK
		    || (len == 4
			&& cctx->ctx_ufunc != ফাঁকা
			&& (cctx->ctx_ufunc->uf_flags & (FC_OBJECT|FC_NEW))
			&& STRNCMP(name, "this", 4) == 0)))
	    || script_var_exists(name, len, cctx, ফাঁকা) == OK
	    || cctx_class_member_idx(cctx, name, len, ফাঁকা) >= 0
	    || find_imported(name, len, FALSE) != ফাঁকা;
}

/*
 * Return TRUE if "name" is a local variable, argument, script variable,
 * imported or function.  Or commands are being skipped, a declaration may have
 * been skipped then.
 */
    স্থির পূর্ণসংখ্যা
item_exists(char_u *name, size_t len, পূর্ণসংখ্যা cmd UNUSED, cctx_T *cctx)
{
    ফেরত variable_exists(name, len, cctx);
}

/*
 * Check if "p[len]" is already defined, either in script "import_sid" or in
 * compilation context "cctx".
 * "cctx" is NULL at the script level, "cstack" is NULL in a function.
 * Does not check the global namespace.
 * If "is_arg" is TRUE the error message is for an argument name.
 * Return FAIL and give an error if it defined.
 */
    পূর্ণসংখ্যা
check_defined(
	char_u	    *p,
	size_t	    len,
	cctx_T	    *cctx,
	cstack_T    *cstack,
	পূর্ণসংখ্যা	    is_arg)
{
    পূর্ণসংখ্যা		c = p[len];
    ufunc_T	*ufunc = ফাঁকা;

    // underscore argument is OK
    যদি (len == 1 && *p == '_')
	ফেরত OK;

    যদি (script_var_exists(p, len, cctx, cstack) == OK)
    {
	যদি (is_arg)
	    semsg(_(e_argument_already_declared_in_script_str), p);
	অন্যথা
	    semsg(_(e_variable_already_declared_in_script_str), p);
	ফেরত FAIL;
    }

    যদি (cctx_class_member_idx(cctx, p, len, ফাঁকা) >= 0)
    {
	যদি (is_arg)
	    semsg(_(e_argument_already_declared_in_class_str), p);
	অন্যথা
	    semsg(_(e_variable_already_declared_in_class_str), p);
	ফেরত FAIL;
    }

    p[len] = NUL;
    যদি ((cctx != ফাঁকা
		&& (lookup_local(p, len, ফাঁকা, cctx) == OK
		    || arg_exists(p, len, ফাঁকা, ফাঁকা, ফাঁকা, cctx) == OK))
	    || find_imported(p, len, FALSE) != ফাঁকা
	    || (ufunc = find_func_even_dead(p, 0)) != ফাঁকা)
    {
	// A local or script-local function can shadow a global function.
	যদি (ufunc == ফাঁকা || ((ufunc->uf_flags & FC_DEAD) == 0
		    && (!func_is_global(ufunc)
					     || (p[0] == 'g' && p[1] == ':'))))
	{
	    যদি (is_arg)
		semsg(_(e_argument_name_shadows_existing_variable_str), p);
	    অন্যথা
		semsg(_(e_name_already_defined_str), p);
	    p[len] = c;
	    ফেরত FAIL;
	}
    }
    p[len] = c;
    ফেরত OK;
}


/*
 * Return TRUE if "actual" could be "expected" and a runtime typecheck is to be
 * used.  Return FALSE if the types will never match.
 */
    স্থির পূর্ণসংখ্যা
use_typecheck(type_T *actual, type_T *expected)
{
    যদি (actual->tt_type == VAR_ANY
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
	ফেরত TRUE;
    যদি (actual->tt_type == VAR_OBJECT && expected->tt_type == VAR_OBJECT)
	ফেরত TRUE;
    যদি ((actual->tt_type == VAR_LIST
		|| actual->tt_type == VAR_TUPLE
		|| actual->tt_type == VAR_DICT)
	    && actual->tt_type == expected->tt_type)
	// This takes care of a nested list or dict.
	ফেরত use_typecheck(actual->tt_member, expected->tt_member);
    ফেরত FALSE;
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
    পূর্ণসংখ্যা
need_type_where(
	type_T	*actual,
	type_T	*expected,
	পূর্ণসংখ্যা	typechk_flags,	// acceptable types (type check flags)
	পূর্ণসংখ্যা	offset,
	where_T	where,
	cctx_T	*cctx,
	পূর্ণসংখ্যা	silent,
	পূর্ণসংখ্যা	actual_is_const)
{
    পূর্ণসংখ্যা ret;

    যদি (expected->tt_type != VAR_CLASS && expected->tt_type != VAR_TYPEALIAS)
    {
	যদি (check_type_is_মান(actual) == FAIL)
	    ফেরত FAIL;
    }

    যদি (expected == &t_bool && actual != &t_bool
					&& (actual->tt_flags & TTFLAG_BOOL_OK))
    {
	// Using "0", "1" or the result of an expression with "&&" or "||" as a
	// boolean is OK but requires a conversion.
	generate_2BOOL(cctx, FALSE, offset);
	ফেরত OK;
    }

    ret = check_type_maybe(expected, actual, FALSE, where);
    যদি (ret == OK)
	ফেরত OK;

    // If actual a constant a runtime check makes no sense.  If it's
    // null_function it is OK.
    যদি (actual_is_const && ret == MAYBE && actual == &t_func_unknown)
	ফেরত OK;

    // If the actual type can be the expected type add a runtime check.
    যদি (!actual_is_const && ret == MAYBE && use_typecheck(actual, expected))
    {
	generate_TYPECHECK(cctx, expected, typechk_flags, offset,
		where.wt_kind == WT_VARIABLE, where.wt_সূচক);
	ফেরত OK;
    }

    যদি (!silent)
	type_mismatch_where(expected, actual, where);
    ফেরত FAIL;
}

    পূর্ণসংখ্যা
need_type(
	type_T	*actual,
	type_T	*expected,
	পূর্ণসংখ্যা	typechk_flags,	// acceptable types (type check flags)
	পূর্ণসংখ্যা	offset,
	পূর্ণসংখ্যা	arg_idx,
	cctx_T	*cctx,
	পূর্ণসংখ্যা	silent,
	পূর্ণসংখ্যা	actual_is_const)
{
    where_T where = WHERE_INIT;

    যদি (arg_idx > 0)
    {
	where.wt_সূচক = arg_idx;
	where.wt_kind = WT_ARGUMENT;
    }
    ফেরত need_type_where(actual, expected, typechk_flags, offset, where,
						cctx, silent, actual_is_const);
}

/*
 * Set type of variable "lvar" to "type".  If the variable is a constant then
 * the type gets TTFLAG_CONST.
 */
    স্থির শূন্য
set_var_type(lvar_T *lvar, type_T *type_arg, cctx_T *cctx)
{
    type_T	*type = type_arg;

    যদি (lvar->lv_const == ASSIGN_CONST && (type->tt_flags & TTFLAG_CONST) == 0)
    {
	যদি (type->tt_flags & TTFLAG_STATIC)
	    // entry in static_types[] is followed by const type
	    type = type + 1;
	অন্যথা
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
	পূর্ণসংখ্যা	assign,
	type_T	*type)
{
    lvar_T  *lvar;
    dfunc_T *dfunc;

    যদি (arg_exists(name, len, ফাঁকা, ফাঁকা, ফাঁকা, cctx) == OK)
    {
	emsg_namelen(_(e_str_is_used_as_argument), name, (পূর্ণসংখ্যা)len);
	ফেরত ফাঁকা;
    }

    যদি (GA_GROW_FAILS(&cctx->ctx_locals, 1))
	ফেরত ফাঁকা;
    lvar = ((lvar_T *)cctx->ctx_locals.ga_তথ্য) + cctx->ctx_locals.ga_len++;
    CLEAR_POINTER(lvar);

    // Every local variable uses the next entry on the stack.  We could re-use
    // the last ones when leaving a scope, but then variables used in a closure
    // might get overwritten.  To keep things simple do not re-use stack
    // entries.  This is less efficient, but memory is cheap these days.
    dfunc = ((dfunc_T *)def_functions.ga_তথ্য) + cctx->ctx_ufunc->uf_dfunc_idx;
    lvar->lv_idx = dfunc->df_var_names.ga_len;

    lvar->lv_name = vim_strnsave(name, len == 0 ? STRLEN(name) : len);
    lvar->lv_const = assign;
    যদি (type == &t_unknown || type == &t_any)
	// type not known yet, may be inferred from RHS
	lvar->lv_type = type;
    অন্যথা
	// may use TTFLAG_CONST
	set_var_type(lvar, type, cctx);

    // Remember the name for debugging.
    যদি (GA_GROW_FAILS(&dfunc->df_var_names, 1))
	ফেরত ফাঁকা;
    ((char_u **)dfunc->df_var_names.ga_তথ্য)[lvar->lv_idx] =
						    vim_strsave(lvar->lv_name);
    ++dfunc->df_var_names.ga_len;

    ফেরত lvar;
}

/*
 * If "check_writable" is ASSIGN_CONST give an error if the variable was
 * defined with :final or :const, if "check_writable" is ASSIGN_FINAL give an
 * error if the variable was defined with :const.
 */
    স্থির পূর্ণসংখ্যা
check_item_writable(svar_T *sv, পূর্ণসংখ্যা check_writable, char_u *name)
{
    যদি ((check_writable == ASSIGN_CONST && sv->sv_const != 0)
	    || (check_writable == ASSIGN_FINAL
					      && sv->sv_const == ASSIGN_CONST))
    {
	semsg(_(e_cannot_change_readonly_variable_str), name);
	ফেরত FAIL;
    }
    ফেরত OK;
}

/*
 * Find "name" in script-local items of script "sid".
 * Pass "check_writable" to check_item_writable().
 * "cctx" is NULL at the script level, "cstack" is NULL in a function.
 * Returns the index in "sn_var_vals" if found.
 * If found but not in "sn_var_vals" returns -1.
 * If not found or the variable is not writable returns -2.
 */
    পূর্ণসংখ্যা
get_script_item_idx(
	পূর্ণসংখ্যা	    sid,
	char_u	    *name,
	পূর্ণসংখ্যা	    check_writable,
	cctx_T	    *cctx,
	cstack_T    *cstack)
{
    hashtab_T	    *ht;
    dictitem_T	    *di;
    scriptitem_T    *si = SCRIPT_ITEM(sid);
    svar_T	    *sv;
    পূর্ণসংখ্যা		    idx;

    যদি (!SCRIPT_ID_VALID(sid))
	ফেরত -1;
    যদি (sid == current_sctx.sc_sid)
    {
	sallvar_T *sav = find_script_var(name, 0, cctx, cstack);

	যদি (sav == ফাঁকা)
	    ফেরত -2;
	idx = sav->sav_var_vals_idx;
	sv = ((svar_T *)si->sn_var_vals.ga_তথ্য) + idx;
	যদি (check_item_writable(sv, check_writable, name) == FAIL)
	    ফেরত -2;
	ফেরত idx;
    }

    // First look the name up in the hashtable.
    ht = &SCRIPT_VARS(sid);
    di = find_var_in_ht(ht, 0, name, TRUE);
    যদি (di == ফাঁকা)
    {
	যদি (si->sn_autoload_prefix != ফাঁকা)
	{
	    hashitem_T *hi;

	    // A variable exported from an autoload script is in the global
	    // variables, we can find it in the all_vars table.
	    hi = hash_find(&si->sn_all_vars.dv_hashtab, name);
	    যদি (!HASHITEM_EMPTY(hi))
		ফেরত HI2SAV(hi)->sav_var_vals_idx;
	}
	ফেরত -2;
    }

    // Now find the svar_T index in sn_var_vals.
    জন্য (idx = 0; idx < si->sn_var_vals.ga_len; ++idx)
    {
	sv = ((svar_T *)si->sn_var_vals.ga_তথ্য) + idx;
	যদি (sv->sv_tv == &di->di_tv)
	{
	    যদি (check_item_writable(sv, check_writable, name) == FAIL)
		ফেরত -2;
	    ফেরত idx;
	}
    }
    ফেরত -1;
}

    স্থির imported_T *
find_imported_in_script(char_u *name, size_t len, পূর্ণসংখ্যা sid)
{
    scriptitem_T    *si;
    পূর্ণসংখ্যা		    idx;

    যদি (!SCRIPT_ID_VALID(sid))
	ফেরত ফাঁকা;
    si = SCRIPT_ITEM(sid);
    জন্য (idx = 0; idx < si->sn_imports.ga_len; ++idx)
    {
	imported_T *import = ((imported_T *)si->sn_imports.ga_তথ্য) + idx;

	যদি (len == 0 ? STRCMP(name, import->imp_name) == 0
		     : STRLEN(import->imp_name) == len
				  && STRNCMP(name, import->imp_name, len) == 0)
	    ফেরত import;
    }
    ফেরত ফাঁকা;
}

/*
 * Find "name" in imported items of the current script.
 * If "len" is 0 use any length that works.
 * If "load" is TRUE and the script was not loaded yet, load it now.
 */
    imported_T *
find_imported(char_u *name, size_t len, পূর্ণসংখ্যা load)
{
    যদি (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	ফেরত ফাঁকা;

    // Skip over "s:" before "s:something" to find the import name.
    পূর্ণসংখ্যা off = name[0] == 's' && name[1] == ':' ? 2 : 0;

    imported_T *ret = find_imported_in_script(name + off, len - off,
							  current_sctx.sc_sid);
    যদি (ret != ফাঁকা && load && (ret->imp_flags & IMP_FLAGS_AUTOLOAD))
    {
	scid_T	actual_sid = 0;
	পূর্ণসংখ্যা	save_emsg_off = emsg_off;

	// "emsg_off" will be set when evaluating an expression silently, but
	// we do want to know about errors in a script.  Also because it then
	// aborts when an error is encountered.
	emsg_off = FALSE;

	// script found before but not loaded yet
	ret->imp_flags &= ~IMP_FLAGS_AUTOLOAD;
	(শূন্য)do_source(SCRIPT_ITEM(ret->imp_sid)->sn_name, FALSE,
						       DOSO_NONE, &actual_sid);
	// If the script is a symlink it may be sourced with another name, may
	// need to adjust the script ID for that.
	যদি (actual_sid != 0)
	    ret->imp_sid = actual_sid;

	emsg_off = save_emsg_off;
    }
    ফেরত ret;
}

/*
 * Called when checking for a following operator at "arg".  When the rest of
 * the line is empty or only a comment, peek the next line.  If there is a next
 * line return a pointer to it and set "nextp".
 * Otherwise skip over white space.
 */
    char_u *
may_peek_পরবর্তী_সারি(cctx_T *cctx, char_u *arg, char_u **nextp)
{
    char_u *p = skipwhite(arg);

    *nextp = ফাঁকা;
    যদি (*p == NUL || (VIM_ISWHITE(*arg) && vim9_comment_start(p)))
    {
	*nextp = peek_পরবর্তী_সারি_from_context(cctx);
	যদি (*nextp != ফাঁকা)
	    ফেরত *nextp;
    }
    ফেরত p;
}

/*
 * Return a pointer to the next line that isn't empty or only contains a
 * comment. Skips over white space.
 * Returns NULL if there is none.
 */
    char_u *
peek_পরবর্তী_সারি_from_context(cctx_T *cctx)
{
    পূর্ণসংখ্যা lnum = cctx->ctx_lnum;

    যখন (++lnum < cctx->ctx_ufunc->uf_সারিগুলি.ga_len)
    {
	char_u *সারি = ((char_u **)cctx->ctx_ufunc->uf_সারিগুলি.ga_তথ্য)[lnum];
	char_u *p;

	// ignore NULLs inserted for continuation lines
	যদি (সারি != ফাঁকা)
	{
	    p = skipwhite(সারি);
	    যদি (vim9_bad_comment(p))
		ফেরত ফাঁকা;
	    যদি (*p != NUL && !vim9_comment_start(p))
		ফেরত p;
	}
    }
    ফেরত ফাঁকা;
}

/*
 * Get the next line of the function from "cctx".
 * Skips over empty lines.  Skips over comment lines if "skip_comment" is TRUE.
 * Returns NULL when at the end.
 */
    char_u *
পরবর্তী_সারি_from_context(cctx_T *cctx, পূর্ণসংখ্যা skip_comment)
{
    char_u	*সারি;

    করো
    {
	++cctx->ctx_lnum;
	যদি (cctx->ctx_lnum >= cctx->ctx_ufunc->uf_সারিগুলি.ga_len)
	{
	    সারি = ফাঁকা;
	    বিরতি;
	}
	সারি = ((char_u **)cctx->ctx_ufunc->uf_সারিগুলি.ga_তথ্য)[cctx->ctx_lnum];
	cctx->ctx_সারি_start = সারি;
	SOURCING_LNUM = cctx->ctx_lnum + 1;
    } যখন (সারি == ফাঁকা || *skipwhite(সারি) == NUL
		     || (skip_comment && vim9_comment_start(skipwhite(সারি))));
    ফেরত সারি;
}

/*
 * Skip over white space at "whitep" and assign to "*arg".
 * If "*arg" is at the end of the line, advance to the next line.
 * Also when "whitep" points to white space and "*arg" is on a "#".
 * Return FAIL if beyond the last line, "*arg" is unmodified then.
 */
    পূর্ণসংখ্যা
may_get_পরবর্তী_সারি(char_u *whitep, char_u **arg, cctx_T *cctx)
{
    *arg = skipwhite(whitep);
    যদি (vim9_bad_comment(*arg))
	ফেরত FAIL;
    যদি (**arg == NUL || (VIM_ISWHITE(*whitep) && vim9_comment_start(*arg)))
    {
	char_u *পরবর্তী = পরবর্তী_সারি_from_context(cctx, TRUE);

	যদি (পরবর্তী == ফাঁকা)
	    ফেরত FAIL;
	*arg = skipwhite(পরবর্তী);
    }
    ফেরত OK;
}

/*
 * Idem, and give an error when failed.
 */
    পূর্ণসংখ্যা
may_get_পরবর্তী_সারি_error(char_u *whitep, char_u **arg, cctx_T *cctx)
{
    যদি (may_get_পরবর্তী_সারি(whitep, arg, cctx) == FAIL)
    {
	SOURCING_LNUM = cctx->ctx_lnum + 1;
	emsg(_(e_সারি_incomplete));
	ফেরত FAIL;
    }
    ফেরত OK;
}

/*
 * Get a line from the compilation context, compatible with exarg_T getline().
 * Return a pointer to the line in allocated memory.
 * Return NULL for end-of-file or some error.
 */
    স্থির char_u *
exarg_getline(
	পূর্ণসংখ্যা c UNUSED,
	শূন্য *cookie,
	পূর্ণসংখ্যা indent UNUSED,
	getline_opt_T options UNUSED)
{
    cctx_T  *cctx = (cctx_T *)cookie;
    char_u  *p;

    জন্য (;;)
    {
	যদি (cctx->ctx_lnum >= cctx->ctx_ufunc->uf_সারিগুলি.ga_len - 1)
	    ফেরত ফাঁকা;
	++cctx->ctx_lnum;
	p = ((char_u **)cctx->ctx_ufunc->uf_সারিগুলি.ga_তথ্য)[cctx->ctx_lnum];
	// Comment lines result in NULL pointers, skip them.
	যদি (p != ফাঁকা)
	    ফেরত vim_strsave(p);
    }
}

    শূন্য
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
    পূর্ণসংখ্যা
func_needs_compiling(ufunc_T *ufunc, compiletype_T compile_type)
{
    নির্বাচন (ufunc->uf_def_status)
    {
	ক্ষেত্র UF_TO_BE_COMPILED:
	    ফেরত TRUE;

	ক্ষেত্র UF_COMPILED:
	{
	    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_তথ্য)
							 + ufunc->uf_dfunc_idx;

	    নির্বাচন (compile_type)
	    {
		ক্ষেত্র CT_PROFILE:
#ifdef FEAT_PROFILE
		    ফেরত dfunc->df_instr_prof == ফাঁকা;
#endif
		ক্ষেত্র CT_NONE:
		    ফেরত dfunc->df_instr == ফাঁকা;
		ক্ষেত্র CT_DEBUG:
		    ফেরত dfunc->df_instr_debug == ফাঁকা;
	    }
	}

	ক্ষেত্র UF_NOT_COMPILED:
	ক্ষেত্র UF_COMPILE_ERROR:
	ক্ষেত্র UF_COMPILING:
	    বিরতি;
    }
    ফেরত FALSE;
}

/*
 * Compile a nested :def command.
 */
    স্থির char_u *
compile_nested_function(exarg_T *eap, cctx_T *cctx, garray_T *সারিগুলি_to_free)
{
    পূর্ণসংখ্যা		is_global = *eap->arg == 'g' && eap->arg[1] == ':';
    char_u	*name_start = eap->arg;
    char_u	*name_end = to_name_end(eap->arg, TRUE);
    পূর্ণসংখ্যা		off;
    char_u	*func_name;
    string_T	lambda_name;
    ufunc_T	*ufunc;
    পূর্ণসংখ্যা		r = FAIL;
    compiletype_T   compile_type;
    পূর্ণসংখ্যা		funcref_isn_idx = -1;
    lvar_T	*lvar = ফাঁকা;
    char_u	*bracket_start = ফাঁকা;

    যদি (eap->forceit)
    {
	semsg(_(e_cannot_use_bang_with_nested_def_str),
		eap->cmdidx == CMD_def ? ":def" : ":function");
	ফেরত ফাঁকা;
    }

    যদি (*name_start == '/')
    {
	name_end = skip_regexp(name_start + 1, '/', TRUE);
	যদি (*name_end == '/')
	    ++name_end;
	set_nextcmd(eap, name_end);
    }

    যদি (*name_end == '<')
    {
	bracket_start = name_end;
	যদি (skip_generic_func_type_args(&name_end) == FAIL)
	    ফেরত ফাঁকা;
    }

    যদি (name_end == name_start || *skipwhite(name_end) != '(')
    {
	যদি (!ends_excmd2(name_start, name_end))
	{
	    যদি (*skipwhite(name_end) == '.')
		semsg(_(e_cannot_define_dict_func_in_vim9_script_str),
								     eap->cmd);
	    অন্যথা
		semsg(_(e_invalid_command_str), eap->cmd);
	    ফেরত ফাঁকা;
	}

	// "def" or "def Name": list functions
	যদি (generate_DEF(cctx, name_start, name_end - name_start) == FAIL)
	    ফেরত ফাঁকা;
	ফেরত eap->nextcmd == ফাঁকা ? (char_u *)"" : eap->nextcmd;
    }

    যদি (bracket_start != ফাঁকা)
	// generic function.  The function name ends before the list of types
	// (opening angle bracket).
	name_end = bracket_start;

    // Only g:Func() can use a namespace.
    যদি (name_start[1] == ':' && !is_global)
    {
	semsg(_(e_namespace_not_supported_str), name_start);
	ফেরত ফাঁকা;
    }
    যদি (cctx->ctx_skip != SKIP_YES
	    && check_defined(name_start, name_end - name_start, cctx,
							  ফাঁকা, FALSE) == FAIL)
	ফেরত ফাঁকা;
    যদি (!ASCII_ISUPPER(is_global ? name_start[2] : name_start[0]))
    {
	semsg(_(e_function_name_must_start_with_capital_str), name_start);
	ফেরত ফাঁকা;
    }

    eap->arg = name_end;
    fill_exarg_from_cctx(eap, cctx);

    eap->forceit = FALSE;
    // We use the special <lambda>99 name, but it's not really a lambda.
    lambda_name = get_lambda_name();
    lambda_name.string = vim_strnsave(lambda_name.string, lambda_name.দৈর্ঘ্য);
    যদি (lambda_name.string == ফাঁকা)
	ফেরত ফাঁকা;

    // This may free the current line, make a copy of the name.
    off = is_global ? 2 : 0;
    func_name = vim_strnsave(name_start + off, name_end - name_start - off);
    যদি (func_name == ফাঁকা)
    {
	r = FAIL;
	যাও theend;
    }

    // A type variable cannot have the name of the function.  The function is
    // defined with the "<lambda>" name, check it here.
    যদি (bracket_start != ফাঁকা)
    {
	size_t	len = STRLEN(func_name);

	জন্য (char_u *t = bracket_start + 1; *t != '>' && *t != NUL; )
	{
	    char_u *te = to_name_end(t, FALSE);

	    যদি ((size_t)(te - t) == len && STRNCMP(t, func_name, len) == 0)
	    {
		semsg(_(e_redefining_script_item_str), func_name);
		r = FAIL;
		যাও theend;
	    }
	    t = te == t ? t + 1 : te;
	}
    }

    // Make sure "KeyTyped" is not set, it may cause indent to be written.
    পূর্ণসংখ্যা save_চাবি_Typed = চাবি_Typed;
    চাবি_Typed = FALSE;

    ufunc = define_function(eap, lambda_name.string, সারিগুলি_to_free, 0, ফাঁকা, 0,
									cctx);

    চাবি_Typed = save_চাবি_Typed;

    যদি (ufunc == ফাঁকা)
    {
	r = eap->skip ? OK : FAIL;
	যাও theend;
    }
    যদি (eap->nextcmd != ফাঁকা)
    {
	semsg(_(e_text_found_after_str_str),
	      eap->cmdidx == CMD_def ? "enddef" : "endfunction", eap->nextcmd);
	r = FAIL;
	func_ptr_unref(ufunc);
	যাও theend;
    }

    // copy over the block scope IDs before compiling
    যদি (!is_global && cctx->ctx_ufunc->uf_block_depth > 0)
    {
	পূর্ণসংখ্যা block_depth = cctx->ctx_ufunc->uf_block_depth;

	ufunc->uf_block_ids = ALLOC_MULT(পূর্ণসংখ্যা, block_depth);
	যদি (ufunc->uf_block_ids != ফাঁকা)
	{
	    mch_memmove(ufunc->uf_block_ids, cctx->ctx_ufunc->uf_block_ids,
						    আকার(পূর্ণসংখ্যা) * block_depth);
	    ufunc->uf_block_depth = block_depth;
	}
    }

    // Define the funcref before compiling, so that it is found by any
    // recursive call.
    যদি (is_global)
    {
	r = generate_NEWFUNC(cctx, lambda_name.string, func_name);
	func_name = ফাঁকা;
	lambda_name.string = ফাঁকা;
	lambda_name.দৈর্ঘ্য = 0;
    }
    অন্যথা
    {
	// Define a local variable for the function reference.
	lvar = reserve_local(cctx, func_name, name_end - name_start,
					    ASSIGN_CONST, ufunc->uf_func_type);
	যদি (lvar == ফাঁকা)
	{
	    func_ptr_unref(ufunc);
	    যাও theend;
	}
	lvar->lv_ufunc = ufunc;
	যদি (generate_FUNCREF(cctx, ufunc, ফাঁকা, FALSE, 0, &funcref_isn_idx) == FAIL)
	{
	    func_ptr_unref(ufunc);
	    যাও theend;
	}
	r = generate_STORE(cctx, ISN_STORE, lvar->lv_idx, ফাঁকা);
    }

    compile_type = get_compile_type(ufunc);
#ifdef FEAT_PROFILE
    // If the outer function is profiled, also compile the nested function for
    // profiling.
    যদি (cctx->ctx_compile_type == CT_PROFILE)
	compile_type = CT_PROFILE;
#endif
    যদি (func_needs_compiling(ufunc, compile_type)
	    && compile_def_function(ufunc, TRUE, compile_type, cctx) == FAIL)
    {
	func_ptr_unref(ufunc);
	যদি (lvar != ফাঁকা)
	    // Now the local variable can't be used.
	    *lvar->lv_name = '/';  // impossible value
	যাও theend;
    }

#ifdef FEAT_PROFILE
    // When the outer function is compiled for profiling, the nested function
    // may be called without profiling.  Compile it here in the right context.
    যদি (compile_type == CT_PROFILE && func_needs_compiling(ufunc, CT_NONE))
	compile_def_function(ufunc, FALSE, CT_NONE, cctx);
#endif

    // If a FUNCREF instruction was generated, set the index after compiling.
    যদি (funcref_isn_idx != -1 && ufunc->uf_def_status == UF_COMPILED)
    {
	isn_T	*funcref_isn = ((isn_T *)cctx->ctx_instr.ga_তথ্য) +
							funcref_isn_idx;
	funcref_isn->isn_arg.funcref.fr_dfunc_idx = ufunc->uf_dfunc_idx;
    }

theend:
    vim_free(lambda_name.string);
    vim_free(func_name);
    ফেরত r == FAIL ? ফাঁকা : (char_u *)"";
}

/*
 * Compile one Vim expression {expr} in string "p".
 * "p" points to the opening "{".
 * Return a pointer to the character after "}", NULL for an error.
 */
    char_u *
compile_one_রাশিমালা_in_str(char_u *p, cctx_T *cctx)
{
    char_u	*block_start;
    char_u	*block_end;

    // Skip the opening {.
    block_start = skipwhite(p + 1);
    block_end = block_start;
    যদি (*block_start != NUL && skip_রাশিমালা(&block_end, ফাঁকা) == FAIL)
	ফেরত ফাঁকা;
    block_end = skipwhite(block_end);
    // The block must be closed by a }.
    যদি (*block_end != '}')
    {
	semsg(_(e_missing_close_curly_str), p);
	ফেরত ফাঁকা;
    }
    যদি (compile_expr0(&block_start, cctx) == FAIL)
	ফেরত ফাঁকা;
    may_generate_2STRING(-1, TOSTRING_INTERPOLATE, cctx);

    ফেরত block_end + 1;
}

/*
 * Compile a string "str" (either containing a literal string or a mix of
 * literal strings and Vim expressions of the form `{expr}`).  This is used
 * when compiling a heredoc assignment to a variable or an interpolated string
 * in a Vim9 def function.  Vim9 instructions are generated to push strings,
 * evaluate expressions, concatenate them and create a list of lines.  When
 * "evalstr" is TRUE, Vim expressions in "str" are evaluated.
 */
    পূর্ণসংখ্যা
compile_all_রাশিমালা_in_str(char_u *str, পূর্ণসংখ্যা evalstr, cctx_T *cctx)
{
    char_u	*p = str;
    char_u	*val;
    পূর্ণসংখ্যা		গুনতি = 0;

    যদি (cctx->ctx_skip == SKIP_YES)
	ফেরত OK;

    যদি (!evalstr || *str == NUL)
    {
	// Literal string, possibly empty.
	val = *str != NUL ? vim_strsave(str) : ফাঁকা;
	ফেরত generate_PUSHS(cctx, &val);
    }

    // Push all the string pieces to the stack, followed by a ISN_CONCAT.
    যখন (*p != NUL)
    {
	char_u	*lit_start;
	পূর্ণসংখ্যা	escaped_brace = FALSE;

	// Look for a block start.
	lit_start = p;
	যখন (*p != '{' && *p != '}' && *p != NUL)
	    ++p;

	যদি (*p != NUL && *p == p[1])
	{
	    // Escaped brace, unescape and continue.
	    // Include the brace in the literal string.
	    ++p;
	    escaped_brace = TRUE;
	}
	অন্যথা যদি (*p == '}')
	{
	    semsg(_(e_stray_closing_curly_str), str);
	    ফেরত FAIL;
	}

	// Append the literal part.
	যদি (p != lit_start)
	{
	    val = vim_strnsave(lit_start, (size_t)(p - lit_start));
	    যদি (generate_PUSHS(cctx, &val) == FAIL)
		ফেরত FAIL;
	    ++গুনতি;
	}

	যদি (*p == NUL)
	    বিরতি;

	যদি (escaped_brace)
	{
	    // Skip the second brace.
	    ++p;
	    চালু;
	}

	p = compile_one_রাশিমালা_in_str(p, cctx);
	যদি (p == ফাঁকা)
	    ফেরত FAIL;
	++গুনতি;
    }

    // Small optimization, if there's only a single piece skip the ISN_CONCAT.
    যদি (গুনতি > 1)
	ফেরত generate_CONCAT(cctx, গুনতি);

    ফেরত OK;
}

/*
 * Return the length of an assignment operator, or zero if there isn't one.
 */
    পূর্ণসংখ্যা
assignment_len(char_u *p, পূর্ণসংখ্যা *heredoc)
{
    যদি (*p == '=')
    {
	যদি (p[1] == '<' && p[2] == '<')
	{
	    *heredoc = TRUE;
	    ফেরত 3;
	}
	ফেরত 1;
    }
    যদি (vim_strchr((char_u *)"+-*/%", *p) != ফাঁকা && p[1] == '=')
	ফেরত 2;
    যদি (STRNCMP(p, "..=", 3) == 0)
	ফেরত 3;
    ফেরত 0;
}

/*
 * Generate the load instruction for "name".
 */
    স্থির পূর্ণসংখ্যা
generate_loadvar(cctx_T *cctx, lhs_T *lhs)
{
    char_u	*name = lhs->lhs_name;
    type_T	*type = lhs->lhs_type;
    পূর্ণসংখ্যা		res = OK;

    নির্বাচন (lhs->lhs_dest)
    {
	ক্ষেত্র dest_option:
	ক্ষেত্র dest_func_option:
	    generate_LOAD(cctx, ISN_LOADOPT, 0, name, type);
	    বিরতি;
	ক্ষেত্র dest_global:
	    যদি (vim_strchr(name, AUTOLOAD_CHAR) == ফাঁকা)
	    {
		যদি (name[2] == NUL)
		    generate_instr_type(cctx, ISN_LOADGDICT, &t_dict_any);
		অন্যথা
		    generate_LOAD(cctx, ISN_LOADG, 0, name + 2, type);
	    }
	    অন্যথা
		generate_LOAD(cctx, ISN_LOADAUTO, 0, name, type);
	    বিরতি;
	ক্ষেত্র dest_বাফার:
	    generate_LOAD(cctx, ISN_LOADB, 0, name + 2, type);
	    বিরতি;
	ক্ষেত্র dest_window:
	    generate_LOAD(cctx, ISN_LOADW, 0, name + 2, type);
	    বিরতি;
	ক্ষেত্র dest_tab:
	    generate_LOAD(cctx, ISN_LOADT, 0, name + 2, type);
	    বিরতি;
	ক্ষেত্র dest_script:
	ক্ষেত্র dest_script_v9:
	    res = compile_load_scriptvar(cctx,
			    name + (name[1] == ':' ? 2 : 0), ফাঁকা, ফাঁকা, ফাঁকা);
	    বিরতি;
	ক্ষেত্র dest_env:
	    // Include $ in the name here
	    generate_LOAD(cctx, ISN_LOADENV, 0, name, type);
	    বিরতি;
	ক্ষেত্র dest_reg:
	    generate_LOAD(cctx, ISN_LOADREG, name[1], ফাঁকা, &t_string);
	    বিরতি;
	ক্ষেত্র dest_vimvar:
	    generate_LOADV(cctx, name + 2);
	    বিরতি;
	ক্ষেত্র dest_local:
	    যদি (cctx->ctx_skip != SKIP_YES)
	    {
		lvar_T	*lvar = lhs->lhs_lvar;
		যদি (lvar->lv_from_outer > 0)
		    generate_LOADOUTER(cctx, lvar->lv_idx, lvar->lv_from_outer,
				 lvar->lv_loop_depth, lvar->lv_loop_idx, type);
		অন্যথা
		    generate_LOAD(cctx, ISN_LOAD, lvar->lv_idx, ফাঁকা, type);
	    }
	    বিরতি;
	ক্ষেত্র dest_class_member:
	    generate_CLASSMEMBER(cctx, TRUE, lhs->lhs_class,
						     lhs->lhs_classmember_idx);
	    বিরতি;
	ক্ষেত্র dest_রাশিমালা:
	    // list or dict value should already be on the stack.
	    বিরতি;
    }

    ফেরত res;
}

/*
 * Skip over "[expr]" or ".member".
 * Does not check for any errors.
 */
    স্থির char_u *
skip_সূচক(char_u *start)
{
    char_u *p = start;

    যদি (*p == '[')
    {
	p = skipwhite(p + 1);
	(শূন্য)skip_রাশিমালা(&p, ফাঁকা);
	p = skipwhite(p);
	যদি (*p == ']')
	    ফেরত p + 1;
	ফেরত p;
    }
    // if (*p == '.')
    ফেরত to_name_end(p + 1, TRUE);
}

    শূন্য
vim9_declare_error(char_u *name)
{
    অক্ষর *scope = "";

    নির্বাচন (*name)
    {
	ক্ষেত্র 'g': scope = _("global"); বিরতি;
	ক্ষেত্র 'b': scope = _("buffer"); বিরতি;
	ক্ষেত্র 'w': scope = _("window"); বিরতি;
	ক্ষেত্র 't': scope = _("tab"); বিরতি;
	ক্ষেত্র 'v': scope = "v:"; বিরতি;
	ক্ষেত্র '$': semsg(_(e_cannot_declare_an_environment_variable_str), name);
		  ফেরত;
	ক্ষেত্র '&': semsg(_(e_cannot_declare_an_option_str), name);
		  ফেরত;
	ক্ষেত্র '@': semsg(_(e_cannot_declare_a_register_str), name);
		  ফেরত;
	পূর্বনির্ধারিত: ফেরত;
    }
    semsg(_(e_cannot_declare_a_scope_variable_str), scope, name);
}

/*
 * Return TRUE if "name" is a valid register to use.
 * Return FALSE and give an error message if not.
 */
    স্থির পূর্ণসংখ্যা
valid_dest_reg(পূর্ণসংখ্যা name)
{
    যদি (name == '@')
       name = '"';
    যদি (name == '/' || name == '=' || name == '#' || valid_yank_reg(name, TRUE))
	ফেরত TRUE;
    emsg_invreg(name);
    ফেরত FAIL;
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
    পূর্ণসংখ্যা
get_var_dest(
	char_u		*name,
	assign_dest_T	*dest,
	cmdidx_T	cmdidx,
	পূর্ণসংখ্যা		has_সূচক,
	পূর্ণসংখ্যা		*option_scope,
	পূর্ণসংখ্যা		*vimvaridx,
	type_T		**type,
	cctx_T		*cctx)
{
    char_u *p;

    যদি (*name == '&')
    {
	পূর্ণসংখ্যা		cc;
	long		numval;
	getoption_T	opt_type;
	পূর্ণসংখ্যা		opt_p_flags;

	*dest = dest_option;
	যদি (cmdidx == CMD_final || cmdidx == CMD_const)
	{
	    emsg(_(e_cannot_lock_option));
	    ফেরত FAIL;
	}
	p = name;
	p = find_option_end(&p, option_scope);
	যদি (p == ফাঁকা)
	{
	    // cannot happen?
	    emsg(_(e_unexpected_characters_in_assignment));
	    ফেরত FAIL;
	}
	cc = *p;
	*p = NUL;
	opt_type = get_option_মান(skip_option_env_lead(name),
				   &numval, ফাঁকা, &opt_p_flags, *option_scope);
	*p = cc;
	নির্বাচন (opt_type)
	{
	    ক্ষেত্র gov_unknown:
		    semsg(_(e_unknown_option_str), name);
		    ফেরত FAIL;
	    ক্ষেত্র gov_string:
	    ক্ষেত্র gov_hidden_string:
		    যদি (opt_p_flags & P_FUNC)
		    {
			// might be a Funcref, check the type later
			*type = &t_any;
			*dest = dest_func_option;
		    }
		    অন্যথা
		    {
			*type = &t_string;
		    }
		    বিরতি;
	    ক্ষেত্র gov_bool:
	    ক্ষেত্র gov_hidden_bool:
		    *type = &t_bool;
		    বিরতি;
	    ক্ষেত্র gov_সংখ্যা:
	    ক্ষেত্র gov_hidden_সংখ্যা:
		    *type = &t_সংখ্যা;
		    বিরতি;
	}
    }
    অন্যথা যদি (*name == '$')
    {
	*dest = dest_env;
	*type = &t_string;
    }
    অন্যথা যদি (*name == '@')
    {
	যদি (!valid_dest_reg(name[1]))
	    ফেরত FAIL;
	*dest = dest_reg;
	*type = name[1] == '#' ? &t_সংখ্যা_or_string : &t_string;
    }
    অন্যথা যদি (STRNCMP(name, "g:", 2) == 0)
    {
	*dest = dest_global;
    }
    অন্যথা যদি (STRNCMP(name, "b:", 2) == 0)
    {
	*dest = dest_বাফার;
    }
    অন্যথা যদি (STRNCMP(name, "w:", 2) == 0)
    {
	*dest = dest_window;
    }
    অন্যথা যদি (STRNCMP(name, "t:", 2) == 0)
    {
	*dest = dest_tab;
    }
    অন্যথা যদি (STRNCMP(name, "v:", 2) == 0)
    {
	typval_T	*vtv;
	পূর্ণসংখ্যা		di_flags;

	*vimvaridx = find_vim_var(name + 2, &di_flags);
	যদি (*vimvaridx < 0)
	{
	    semsg(_(e_variable_not_found_str), name);
	    ফেরত FAIL;
	}
	// An item of a read-only variable can be changed, as when not compiled.
	// We use the current value of "sandbox" here, is that OK?
	যদি (!has_সূচক && var_check_ro(di_flags, name, FALSE))
	    ফেরত FAIL;
	*dest = dest_vimvar;
	vtv = get_vim_var_tv(*vimvaridx);
	*type = typval2type_vimvar(vtv, cctx->ctx_type_list);
    }
    ফেরত OK;
}

    স্থির পূর্ণসংখ্যা
is_decl_command(cmdidx_T cmdidx)
{
    ফেরত cmdidx == CMD_let || cmdidx == CMD_var
				 || cmdidx == CMD_final || cmdidx == CMD_const;
}

/*
 * Returns TRUE if the class or object variable in "lhs" is modifiable.
 * "var_start" points to the start of the variable name and "lhs->lhs_varlen"
 * has the total length.  Note that the "lhs" can be nested an object reference
 * (e.g.  a.b.c.d.var).
 */
    স্থির পূর্ণসংখ্যা
lhs_class_member_modifiable(lhs_T *lhs, char_u	*var_start, cctx_T *cctx)
{
    size_t	varlen = lhs->lhs_varlen;
    class_T	*cl = lhs->lhs_type->tt_class;
    পূর্ণসংখ্যা		is_object = lhs->lhs_type->tt_type == VAR_OBJECT;
    char_u	*name = var_start + varlen + 1;
    size_t	namelen = lhs->lhs_end - var_start - varlen - 1;
    ocmember_T	*m;

    m = member_lookup(cl, lhs->lhs_type->tt_type, name, namelen, ফাঁকা);
    যদি (m == ফাঁকা)
    {
	member_not_found_বার্তা(cl, lhs->lhs_type->tt_type, name, namelen);
	ফেরত FALSE;
    }

    যদি (IS_ENUM(cl))
    {
	semsg(_(e_enumvalue_str_cannot_be_modified), cl->class_name.string,
		m->ocm_name.string);
	ফেরত FALSE;
    }

    // If it is private member variable, then accessing it outside the
    // class is not allowed.
    // If it is a read only class variable, then it can be modified
    // only inside the class where it is defined.
    যদি ((m->ocm_access != VIM_ACCESS_ALL) &&
	    ((is_object && !inside_class(cctx, cl))
	     || (!is_object && cctx->ctx_ufunc->uf_class != cl)))
    {
	অক্ষর *বার্তা = (m->ocm_access == VIM_ACCESS_PRIVATE)
				? e_cannot_access_protected_variable_str
				: e_variable_is_not_writable_str;
	emsg_var_cl_define(বার্তা, m->ocm_name.string, 0, cl);
	ফেরত FALSE;
    }

    ফেরত TRUE;
}

/*
 * Initialize "lhs" with default values
 */
    স্থির শূন্য
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
    স্থির পূর্ণসংখ্যা
lhs_find_var_end(
    lhs_T	*lhs,
    char_u	*var_start,
    পূর্ণসংখ্যা		is_decl,
    char_u	**var_endp)
{
    char_u  *var_end = *var_endp;

    // "lhs_dest_end" is the end of the destination, including "[expr]" or
    // ".name".
    // "var_end" is the end of the variable/option/etc. name.
    lhs->lhs_dest_end = skip_var_one(var_start, FALSE);
    যদি (*var_start == '@')
    {
	যদি (!valid_dest_reg(var_start[1]))
	    ফেরত FAIL;
	var_end = var_start + 2;
    }
    অন্যথা
    {
	// skip over the leading "&", "&l:", "&g:" and "$"
	var_end = skip_option_env_lead(var_start);
	var_end = to_name_end(var_end, TRUE);
    }

    // "a: type" is declaring variable "a" with a type, not dict "a:".
    যদি (is_decl && lhs->lhs_dest_end == var_start + 2
					&& lhs->lhs_dest_end[-1] == ':')
	--lhs->lhs_dest_end;
    যদি (is_decl && var_end == var_start + 2 && var_end[-1] == ':')
	--var_end;

    lhs->lhs_end = lhs->lhs_dest_end;
    *var_endp = var_end;

    ফেরত OK;
}

/*
 * Set various fields in "lhs"
 */
    স্থির পূর্ণসংখ্যা
lhs_init(
    lhs_T	*lhs,
    char_u	*var_start,
    পূর্ণসংখ্যা		is_decl,
    পূর্ণসংখ্যা		heredoc,
    char_u	**var_endp)
{
    char_u *var_end = *var_endp;

    lhs_init_defaults(lhs);

    // Find the end of the variable and the destination
    যদি (lhs_find_var_end(lhs, var_start, is_decl, &var_end) == FAIL)
	ফেরত FAIL;

    // compute the length of the destination without "[expr]" or ".name"
    lhs->lhs_varlen = var_end - var_start;
    lhs->lhs_varlen_যোগ = lhs->lhs_varlen;
    lhs->lhs_name = vim_strnsave(var_start, lhs->lhs_varlen);
    যদি (lhs->lhs_name == ফাঁকা)
	ফেরত FAIL;

    যদি (lhs->lhs_dest_end > var_start + lhs->lhs_varlen)
	// Something follows after the variable: "var[idx]" or "var.key".
	lhs->lhs_has_সূচক = TRUE;

    lhs->lhs_type = heredoc ? &t_list_string : &t_any;

    *var_endp = var_end;

    ফেরত OK;
}

/*
 * Compile a LHS class variable name.
 */
    স্থির পূর্ণসংখ্যা
compile_lhs_class_variable(
    cctx_T	*cctx,
    lhs_T	*lhs,
    class_T	*defcl,
    পূর্ণসংখ্যা		is_decl)
{
    যদি (cctx->ctx_ufunc->uf_defclass != defcl)
    {
	// A class variable can be accessed without the class name
	// only inside a class.
	semsg(_(e_class_variable_str_accessible_only_inside_class_str),
		lhs->lhs_name, defcl->class_name.string);
	ফেরত FAIL;
    }

    যদি (is_decl)
    {
	semsg(_(e_variable_already_declared_in_class_str), lhs->lhs_name);
	ফেরত FAIL;
    }

    ocmember_T	*m = &defcl->class_class_members[lhs->lhs_classmember_idx];
    যদি (oc_var_check_ro(defcl, m))
	ফেরত FAIL;

    lhs->lhs_dest = dest_class_member;
    // The class variable is defined either in the current class or
    // in one of the parent class in the hierarchy.
    lhs->lhs_class = defcl;
    lhs->lhs_type = oc_member_type_by_idx(defcl, FALSE,
						lhs->lhs_classmember_idx);

    ফেরত OK;
}

/*
 * Compile an imported LHS variable
 */
    স্থির পূর্ণসংখ্যা
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
    যদি (dot == ফাঁকা)
    {
	semsg(_(e_no_dot_after_imported_name_str), var_start);
	ফেরত FAIL;
    }

    p = skipwhite(dot + 1);
    var_end = to_name_end(p, TRUE);
    যদি (var_end == p)
    {
	semsg(_(e_missing_name_after_imported_name_str), var_start);
	ফেরত FAIL;
    }

    vim_free(lhs->lhs_name);
    lhs->lhs_varlen = var_end - p;
    lhs->lhs_name = vim_strnsave(p, lhs->lhs_varlen);
    যদি (lhs->lhs_name == ফাঁকা)
	ফেরত FAIL;
    *rawnamep = lhs->lhs_name;
    lhs->lhs_scriptvar_sid = import->imp_sid;

    // TODO: where do we check this name is exported?

    // Check if something follows: "exp.var[idx]" or
    // "exp.var.key".
    lhs->lhs_has_সূচক = lhs->lhs_dest_end > skipwhite(var_end);

    *var_endp = var_end;

    ফেরত OK;
}

/*
 * Process a script-local variable when compiling a LHS variable name.
 */
    স্থির পূর্ণসংখ্যা
compile_lhs_script_var(
    cctx_T	*cctx,
    lhs_T	*lhs,
    char_u	*var_start,
    char_u	*var_end,
    পূর্ণসংখ্যা		is_decl)
{
    পূর্ণসংখ্যা		script_namespace = FALSE;
    পূর্ণসংখ্যা		script_var = FALSE;
    imported_T	*import;
    char_u	*var_name;
    size_t	var_name_len;

    যদি (lhs->lhs_varlen > 1 && STRNCMP(var_start, "s:", 2) == 0)
	script_namespace = TRUE;

    যদি (script_namespace)
    {
	var_name = var_start + 2;
	var_name_len = lhs->lhs_varlen - 2;
    }
    অন্যথা
    {
	var_name = var_start;
	var_name_len = lhs->lhs_varlen;
    }

    যদি (script_var_exists(var_name, var_name_len, cctx, ফাঁকা) == OK)
	script_var = TRUE;

    import = find_imported(var_start, lhs->lhs_varlen, FALSE);

    যদি (script_namespace || script_var || import != ফাঁকা)
    {
	char_u *rawname = lhs->lhs_name + (lhs->lhs_name[1] == ':' ? 2 : 0);

	যদি (script_namespace && current_script_is_vim9())
	{
	    semsg(_(e_cannot_use_s_colon_in_vim9_script_str), var_start);
	    ফেরত FAIL;
	}

	যদি (is_decl)
	{
	    যদি (script_namespace)
		semsg(_(e_cannot_declare_script_variable_in_function_str),
			lhs->lhs_name);
	    অন্যথা
		semsg(_(e_variable_already_declared_in_script_str),
			lhs->lhs_name);
	    ফেরত FAIL;
	}
	অন্যথা যদি (cctx->ctx_ufunc->uf_script_ctx_version == SCRIPT_VERSION_VIM9
		&& script_namespace
		&& !script_var && import == ফাঁকা)
	{
	    semsg(_(e_unknown_variable_str), lhs->lhs_name);
	    ফেরত FAIL;
	}

	lhs->lhs_dest = current_script_is_vim9() ? dest_script_v9 :
								dest_script;

	// existing script-local variables should have a type
	lhs->lhs_scriptvar_sid = current_sctx.sc_sid;
	যদি (import != ফাঁকা)
	{
	    যদি (compile_lhs_import_var(lhs, import, var_start, &var_end,
							&rawname) == FAIL)
		ফেরত FAIL;
	}

	যদি (SCRIPT_ID_VALID(lhs->lhs_scriptvar_sid))
	{
	    // Check writable only when no index follows.
	    lhs->lhs_scriptvar_idx = get_script_item_idx(
					lhs->lhs_scriptvar_sid, rawname,
					lhs->lhs_has_সূচক ?  ASSIGN_FINAL :
					ASSIGN_CONST, cctx, ফাঁকা);
	    যদি (lhs->lhs_scriptvar_idx >= 0)
	    {
		scriptitem_T *si = SCRIPT_ITEM(lhs->lhs_scriptvar_sid);
		svar_T	 *sv = ((svar_T *)si->sn_var_vals.ga_তথ্য)
						+ lhs->lhs_scriptvar_idx;

		lhs->lhs_type = sv->sv_type;
	    }
	}

	ফেরত OK;
    }

    ফেরত check_defined(var_start, lhs->lhs_varlen, cctx, ফাঁকা, FALSE);
}

/*
 * Compile the LHS destination.
 */
    স্থির পূর্ণসংখ্যা
compile_lhs_var_dest(
    cctx_T	*cctx,
    lhs_T	*lhs,
    পূর্ণসংখ্যা		cmdidx,
    char_u	*var_start,
    char_u	*var_end,
    পূর্ণসংখ্যা		is_decl,
    পূর্ণসংখ্যা		has_cmd)	// "var" before "var_start"
{
    পূর্ণসংখ্যা	    declare_error = FALSE;

    যদি (get_var_dest(lhs->lhs_name, &lhs->lhs_dest, cmdidx,
				lhs->lhs_has_সূচক,
				&lhs->lhs_opt_flags, &lhs->lhs_vimvaridx,
				&lhs->lhs_type, cctx) == FAIL)
	ফেরত FAIL;

    যদি (lhs->lhs_dest != dest_local && cmdidx != CMD_const
						&& cmdidx != CMD_final)
    {
	// Specific kind of variable recognized.
	declare_error = is_decl;
    }
    অন্যথা
    {
	class_T	*defcl;

	// No specific kind of variable recognized, just a name.
	যদি (check_reserved_name(lhs->lhs_name, lhs->lhs_has_সূচক
						&& *var_end == '.') == FAIL)
	    ফেরত FAIL;

	যদি (lookup_local(var_start, lhs->lhs_varlen, &lhs->lhs_local_lvar,
								cctx) == OK)
	{
	    lhs->lhs_lvar = &lhs->lhs_local_lvar;
	}
	অন্যথা
	{
	    CLEAR_FIELD(lhs->lhs_arg_lvar);
	    যদি (arg_exists(var_start, lhs->lhs_varlen,
			&lhs->lhs_arg_lvar.lv_idx, &lhs->lhs_arg_lvar.lv_type,
			&lhs->lhs_arg_lvar.lv_from_outer, cctx) == OK)
	    {
		যদি (is_decl)
		{
		    semsg(_(e_str_is_used_as_argument), lhs->lhs_name);
		    ফেরত FAIL;
		}
		lhs->lhs_lvar = &lhs->lhs_arg_lvar;
	    }
	}

	যদি (lhs->lhs_lvar != ফাঁকা)
	{
	    যদি (is_decl)
	    {
		// if we come here with what looks like an assignment like
		// .= but which has been rejected by assignment_len() from
		// may_compile_assignment give a better error message
		char_u *p = skipwhite(lhs->lhs_end);
		যদি (p[0] == '.' && p[1] == '=')
		    emsg(_(e_dot_equal_not_supported_with_script_version_two));
		অন্যথা যদি (p[0] == ':' && !has_cmd)
		    // type specified in an assignment without "var"
		    semsg(_(e_trailing_characters_str), p);
		অন্যথা
		    semsg(_(e_variable_already_declared_str), lhs->lhs_name);
		ফেরত FAIL;
	    }
	}
	অন্যথা যদি ((lhs->lhs_classmember_idx = cctx_class_member_idx(
			cctx, var_start, lhs->lhs_varlen, &defcl)) >= 0)
	{
	    যদি (compile_lhs_class_variable(cctx, lhs, defcl, is_decl)
		    == FAIL)
		ফেরত FAIL;
	}
	অন্যথা
	{
	    যদি (compile_lhs_script_var(cctx, lhs, var_start, var_end,
			is_decl) == FAIL)
		ফেরত FAIL;
	}
    }

    যদি (declare_error)
    {
	vim9_declare_error(lhs->lhs_name);
	ফেরত FAIL;
    }

    ফেরত OK;
}

/*
 * When compiling a LHS variable name, for a class or an object, set the LHS
 * member type.
 */
    স্থির পূর্ণসংখ্যা
compile_lhs_set_oc_member_type(
    cctx_T	*cctx,
    lhs_T	*lhs,
    char_u	*var_start)
{
    class_T	*cl = lhs->lhs_type->tt_class;
    পূর্ণসংখ্যা		is_object = lhs->lhs_type->tt_type == VAR_OBJECT;
    char_u	*name = var_start + lhs->lhs_varlen + 1;
    size_t	namelen = lhs->lhs_end - var_start - lhs->lhs_varlen - 1;

    ocmember_T	*m = member_lookup(cl, lhs->lhs_type->tt_type,
	    name, namelen, &lhs->lhs_member_idx);
    যদি (m == ফাঁকা)
    {
	member_not_found_বার্তা(cl, lhs->lhs_type->tt_type, name, namelen);
	ফেরত FAIL;
    }

    যদি (IS_ENUM(cl))
    {
	যদি (!inside_class(cctx, cl))
	{
	    semsg(_(e_enumvalue_str_cannot_be_modified),
		    cl->class_name.string, m->ocm_name.string);
	    ফেরত FAIL;
	}
	যদি (lhs->lhs_type->tt_type == VAR_OBJECT &&
		lhs->lhs_member_idx < 2)
	{
	    অক্ষর *বার্তা = lhs->lhs_member_idx == 0 ?
		e_enum_str_name_cannot_be_modified :
		e_enum_str_ordinal_cannot_be_modified;
	    semsg(_(বার্তা), cl->class_name.string);
	    ফেরত FAIL;
	}
    }

    // If it is private member variable, then accessing it outside the
    // class is not allowed.
    // If it is a read only class variable, then it can be modified
    // only inside the class where it is defined.
    যদি ((m->ocm_access != VIM_ACCESS_ALL) &&
	    ((is_object && !inside_class(cctx, cl))
	     || (!is_object && cctx->ctx_ufunc->uf_defclass != cl)))
    {
	অক্ষর *বার্তা = (m->ocm_access == VIM_ACCESS_PRIVATE)
	    ? e_cannot_access_protected_variable_str
	    : e_variable_is_not_writable_str;
	emsg_var_cl_define(বার্তা, m->ocm_name.string, 0, cl);
	ফেরত FAIL;
    }

    যদি (!IS_CONSTRUCTOR_METHOD(cctx->ctx_ufunc)
	    && oc_var_check_ro(cl, m))
	ফেরত FAIL;

    lhs->lhs_member_type = m->ocm_type;

    ফেরত OK;
}

/*
 * When compiling a LHS variable, set the LHS variable type.
 */
    স্থির পূর্ণসংখ্যা
compile_lhs_set_type(cctx_T *cctx, lhs_T *lhs, char_u *var_end, পূর্ণসংখ্যা is_decl)
{
    যদি (is_decl && *skipwhite(var_end) == ':')
    {
	char_u *p;

	// parse optional type: "let var: type = expr"
	যদি (VIM_ISWHITE(*var_end))
	{
	    semsg(_(e_no_white_space_allowed_before_colon_str), var_end);
	    ফেরত FAIL;
	}

	যদি (!VIM_ISWHITE(var_end[1]))
	{
	    semsg(_(e_white_space_required_after_str_str), ":", var_end);
	    ফেরত FAIL;
	}

	p = skipwhite(var_end + 1);
	lhs->lhs_type = parse_type(&p, cctx->ctx_type_list, cctx->ctx_ufunc, cctx, TRUE);
	যদি (lhs->lhs_type == ফাঁকা
		|| !valid_declaration_type(lhs->lhs_type))
	    ফেরত FAIL;

	lhs->lhs_has_type = TRUE;
	lhs->lhs_end = p;
    }
    অন্যথা যদি (lhs->lhs_lvar != ফাঁকা)
	lhs->lhs_type = lhs->lhs_lvar->lv_type;

    ফেরত OK;
}

/*
 * Returns TRUE if "lhs" is a concatenable string.
 */
    স্থির পূর্ণসংখ্যা
lhs_concatenable(lhs_T *lhs)
{
    ফেরত lhs->lhs_dest == dest_global
		|| lhs->lhs_has_সূচক
		|| lhs->lhs_type->tt_type == VAR_STRING
		|| lhs->lhs_type->tt_type == VAR_ANY;
}

/*
 * Create a new local variable when compiling a LHS variable.
 */
    স্থির পূর্ণসংখ্যা
compile_lhs_new_local_var(
    cctx_T	*cctx,
    lhs_T	*lhs,
    char_u	*var_start,
    পূর্ণসংখ্যা		cmdidx,
    পূর্ণসংখ্যা		oplen,
    পূর্ণসংখ্যা		is_decl,
    পূর্ণসংখ্যা		has_cmd,
    পূর্ণসংখ্যা		heredoc)
{
    যদি (oplen > 1 && !heredoc)
    {
	// +=, /=, etc. require an existing variable
	semsg(_(e_cannot_use_operator_on_new_variable_str), lhs->lhs_name);
	ফেরত FAIL;
    }

    যদি (!is_decl || (lhs->lhs_has_সূচক && !has_cmd
					&& cctx->ctx_skip != SKIP_YES))
    {
	semsg(_(e_unknown_variable_str), lhs->lhs_name);
	ফেরত FAIL;
    }

    // Check the name is valid for a funcref.
    যদি (lhs->lhs_type->tt_type == VAR_FUNC
				|| lhs->lhs_type->tt_type == VAR_PARTIAL)
    {
	যদি (var_wrong_func_name(lhs->lhs_name, TRUE))
	    ফেরত FAIL;
    }

    // New local variable.
    পূর্ণসংখ্যা assign;
    নির্বাচন (cmdidx)
    {
	ক্ষেত্র CMD_final:
	    assign = ASSIGN_FINAL; বিরতি;
	ক্ষেত্র CMD_const:
	    assign = ASSIGN_CONST; বিরতি;
	পূর্বনির্ধারিত:
	    assign = ASSIGN_VAR; বিরতি;
    }

    lhs->lhs_lvar = reserve_local(cctx, var_start, lhs->lhs_varlen, assign,
							lhs->lhs_type);
    যদি (lhs->lhs_lvar == ফাঁকা)
	ফেরত FAIL;

    lhs->lhs_new_local = TRUE;

    ফেরত OK;
}

/*
 * When compiling a LHS variable name, set the LHS member type.
 */
    স্থির পূর্ণসংখ্যা
compile_lhs_set_member_type(
    cctx_T	*cctx,
    lhs_T	*lhs,
    char_u	*var_start,
    পূর্ণসংখ্যা		is_decl,
    পূর্ণসংখ্যা		has_cmd)
{
    lhs->lhs_member_type = lhs->lhs_type;

    যদি (!lhs->lhs_has_সূচক)
	ফেরত OK;

    char_u	*after = var_start + lhs->lhs_varlen;
    char_u	*p;

    // Something follows after the variable: "var[idx]" or "var.key".
    যদি (is_decl && cctx->ctx_skip != SKIP_YES)
    {
	যদি (has_cmd)
	    emsg(_(e_cannot_use_সূচক_when_declaring_variable));
	অন্যথা
	    semsg(_(e_unknown_variable_str), lhs->lhs_name);
	ফেরত FAIL;
    }

    // Now: var_start[lhs->lhs_varlen] is '[' or '.'
    // Only the last index is used below, if there are others
    // before it generate code for the expression.  Thus for
    // "ll[1][2]" the expression is "ll[1]" and "[2]" is the index.
    জন্য (;;)
    {
	p = skip_সূচক(after);
	যদি (*p != '[' && *p != '.')
	{
	    lhs->lhs_varlen_যোগ = p - var_start;
	    বিরতি;
	}
	after = p;
    }
    যদি (after > var_start + lhs->lhs_varlen)
    {
	lhs->lhs_varlen = after - var_start;
	lhs->lhs_dest = dest_রাশিমালা;
	// We don't know the type before evaluating the expression,
	// use "any" until then.  The member index is for the first name,
	// not for the last index.
	lhs->lhs_type = &t_any;
	lhs->lhs_member_idx = -1;
    }

    পূর্ণসংখ্যা use_class = lhs->lhs_type != ফাঁকা
	&& (lhs->lhs_type->tt_type == VAR_CLASS
		|| lhs->lhs_type->tt_type == VAR_OBJECT);

    যদি (lhs->lhs_type == ফাঁকা
	    || (use_class ? lhs->lhs_type->tt_class == ফাঁকা
		: lhs->lhs_type->tt_member == ফাঁকা))
    {
	lhs->lhs_member_type = &t_any;
    }
    অন্যথা যদি (use_class)
    {
	// for an object or class member get the type of the member
	যদি (compile_lhs_set_oc_member_type(cctx, lhs, var_start) == FAIL)
	    ফেরত FAIL;
    }
    অন্যথা
	lhs->lhs_member_type = lhs->lhs_type->tt_member;

    ফেরত OK;
}

/*
 * Figure out the LHS type and other properties for an assignment or one item
 * of ":unlet" with an index.
 * Returns OK or FAIL.
 */
    পূর্ণসংখ্যা
compile_lhs(
	char_u	    *var_start,
	lhs_T	    *lhs,
	cmdidx_T    cmdidx,
	পূর্ণসংখ্যা	    heredoc,
	পূর্ণসংখ্যা	    has_cmd,	    // "var" before "var_start"
	পূর্ণসংখ্যা	    oplen,
	cctx_T	    *cctx)
{
    char_u	*var_end = ফাঁকা;
    পূর্ণসংখ্যা		is_decl = is_decl_command(cmdidx);

    যদি (lhs_init(lhs, var_start, is_decl, heredoc, &var_end) == FAIL)
	ফেরত FAIL;

    যদি (cctx->ctx_skip != SKIP_YES)
    {
	// compile the LHS destination
	যদি (compile_lhs_var_dest(cctx, lhs, cmdidx, var_start, var_end,
						is_decl, has_cmd) == FAIL)
	    ফেরত FAIL;
    }

    // handle "a:name" as a name, not index "name" in "a"
    যদি (lhs->lhs_varlen > 1 || var_start[lhs->lhs_varlen] != ':')
	var_end = lhs->lhs_dest_end;

    যদি (lhs->lhs_dest != dest_option && lhs->lhs_dest != dest_func_option)
    {
	// set the LHS variable type
	যদি (compile_lhs_set_type(cctx, lhs, var_end, is_decl) == FAIL)
	    ফেরত FAIL;
    }

    যদি (oplen == 3 && !heredoc && !lhs_concatenable(lhs))
    {
	emsg(_(e_can_only_concatenate_to_string));
	ফেরত FAIL;
    }

    যদি (lhs->lhs_lvar == ফাঁকা && lhs->lhs_dest == dest_local
						&& cctx->ctx_skip != SKIP_YES)
    {
	যদি (compile_lhs_new_local_var(cctx, lhs, var_start, cmdidx, oplen,
					is_decl, has_cmd, heredoc) == FAIL)
	    ফেরত FAIL;
    }

    যদি (compile_lhs_set_member_type(cctx, lhs, var_start, is_decl, has_cmd)
								== FAIL)
	ফেরত FAIL;

    ফেরত OK;
}

/*
 * Figure out the LHS and check a few errors.
 */
    পূর্ণসংখ্যা
compile_assign_lhs(
	char_u	    *var_start,
	lhs_T	    *lhs,
	cmdidx_T    cmdidx,
	পূর্ণসংখ্যা	    is_decl,
	পূর্ণসংখ্যা	    heredoc,
	পূর্ণসংখ্যা	    has_cmd,	    // "var" before "var_start"
	পূর্ণসংখ্যা	    oplen,
	cctx_T	    *cctx)
{
    যদি (compile_lhs(var_start, lhs, cmdidx, heredoc, has_cmd, oplen, cctx)
								       == FAIL)
	ফেরত FAIL;

    যদি (!lhs->lhs_has_সূচক && lhs->lhs_lvar == &lhs->lhs_arg_lvar)
    {
	semsg(_(e_cannot_assign_to_argument_str), lhs->lhs_name);
	ফেরত FAIL;
    }
    যদি (!is_decl && lhs->lhs_lvar != ফাঁকা
			   && lhs->lhs_lvar->lv_const != ASSIGN_VAR
			   && !lhs->lhs_has_সূচক)
    {
	semsg(_(e_cannot_assign_to_constant_str), lhs->lhs_name);
	ফেরত FAIL;
    }
    ফেরত OK;
}

/*
 * Return TRUE if "lhs" has a range index: "[expr : expr]".
 */
    স্থির পূর্ণসংখ্যা
has_list_সূচক(char_u *idx_start, cctx_T *cctx)
{
    char_u  *p = idx_start;
    পূর্ণসংখ্যা	    save_skip;

    যদি (*p != '[')
	ফেরত FALSE;

    p = skipwhite(p + 1);
    যদি (*p == ':')
	ফেরত TRUE;

    save_skip = cctx->ctx_skip;
    cctx->ctx_skip = SKIP_YES;
    (শূন্য)compile_expr0(&p, cctx);
    cctx->ctx_skip = save_skip;
    ফেরত *skipwhite(p) == ':';
}

/*
 * For an assignment with an index, compile the "idx" in "var[idx]" or "key" in
 * "var.key".
 */
    স্থির পূর্ণসংখ্যা
compile_assign_সূচক(
	char_u	*var_start,
	lhs_T	*lhs,
	পূর্ণসংখ্যা	*range,
	cctx_T	*cctx)
{
    size_t	varlen = lhs->lhs_varlen;
    char_u	*p;
    পূর্ণসংখ্যা		r = OK;
    পূর্ণসংখ্যা		need_white_before = TRUE;
    পূর্ণসংখ্যা		empty_second;

    p = var_start + varlen;
    যদি (*p == '[')
    {
	p = skipwhite(p + 1);
	যদি (*p == ':')
	{
	    // empty first index, push zero
	    r = generate_PUSHNR(cctx, 0);
	    need_white_before = FALSE;
	}
	অন্যথা
	    r = compile_expr0(&p, cctx);

	যদি (r == OK && *skipwhite(p) == ':')
	{
	    // unlet var[idx : idx]
	    // blob[idx : idx] = value
	    *range = TRUE;
	    p = skipwhite(p);
	    empty_second = *skipwhite(p + 1) == ']';
	    যদি ((need_white_before && !IS_WHITE_OR_NUL(p[-1]))
		    || (!empty_second && !IS_WHITE_OR_NUL(p[1])))
	    {
		semsg(_(e_white_space_required_before_and_after_str_at_str),
								      ":", p);
		ফেরত FAIL;
	    }
	    p = skipwhite(p + 1);
	    যদি (*p == ']')
		// empty second index, push "none"
		r = generate_PUSHSPEC(cctx, VVAL_NONE);
	    অন্যথা
		r = compile_expr0(&p, cctx);
	}

	যদি (r == OK && *skipwhite(p) != ']')
	{
	    // this should not happen
	    emsg(_(e_missing_closing_square_brace));
	    r = FAIL;
	}
    }
    অন্যথা যদি (lhs->lhs_member_idx >= 0)
    {
	// object member index
	r = generate_PUSHNR(cctx, lhs->lhs_member_idx);
    }
    অন্যথা // if (*p == '.')
    {
	char_u *চাবি_end = to_name_end(p + 1, TRUE);
	char_u *চাবি = vim_strnsave(p + 1, চাবি_end - p - 1);

	r = generate_PUSHS(cctx, &চাবি);
    }
    ফেরত r;
}

/*
 * For a LHS with an index, load the variable to be indexed.
 */
    স্থির পূর্ণসংখ্যা
compile_load_lhs(
	lhs_T	*lhs,
	char_u	*var_start,
	type_T	*rhs_type,
	cctx_T	*cctx)
{
    যদি (lhs->lhs_dest == dest_রাশিমালা)
    {
	size_t	    varlen = lhs->lhs_varlen;
	পূর্ণসংখ্যা	    c = var_start[varlen];
	পূর্ণসংখ্যা	    সারিগুলি_len = cctx->ctx_ufunc->uf_সারিগুলি.ga_len;
	পূর্ণসংখ্যা	    res;

	// Evaluate "ll[expr]" of "ll[expr][idx]".  End the line with a NUL and
	// limit the lines array length to avoid skipping to a following line.
	var_start[varlen] = NUL;
	cctx->ctx_ufunc->uf_সারিগুলি.ga_len = cctx->ctx_lnum + 1;
	char_u *p = var_start;
	res = compile_expr0(&p, cctx);
	var_start[varlen] = c;
	cctx->ctx_ufunc->uf_সারিগুলি.ga_len = সারিগুলি_len;
	যদি (res == FAIL || p != var_start + varlen)
	{
	    // this should not happen
	    যদি (res != FAIL)
		emsg(_(e_missing_closing_square_brace));
	    ফেরত FAIL;
	}

	lhs->lhs_type = cctx->ctx_type_স্তূপ.ga_len == 0 ? &t_void
						  : get_type_on_স্তূপ(cctx, 0);

	যদি (lhs->lhs_type->tt_type == VAR_CLASS
		|| (lhs->lhs_type->tt_type == VAR_OBJECT
		    && lhs->lhs_type != &t_object_any))
	{
	    // Check whether the class or object variable is modifiable
	    যদি (!lhs_class_member_modifiable(lhs, var_start, cctx))
		ফেরত FAIL;
	}

	// Now we can properly check the type.  The variable is indexed, thus
	// we need the member type.  For a class or object we don't know the
	// type yet, it depends on what member is used.
	// The top item in the stack is the Dict, followed by the key and then
	// the type of the value.
	vartype_T vartype = lhs->lhs_type->tt_type;
	type_T *member_type = lhs->lhs_type->tt_member;
	যদি (rhs_type != ফাঁকা && member_type != ফাঁকা
		&& vartype != VAR_OBJECT && vartype != VAR_CLASS
		&& rhs_type != &t_void
		&& need_type(rhs_type, member_type, 0,
					    -3, 0, cctx, FALSE, FALSE) == FAIL)
	    ফেরত FAIL;

	ফেরত OK;
    }

    ফেরত generate_loadvar(cctx, lhs);
}

/*
 * Produce code for loading "lhs" and also take care of an index.
 * Return OK/FAIL.
 */
    পূর্ণসংখ্যা
compile_load_lhs_with_সূচক(lhs_T *lhs, char_u *var_start, cctx_T *cctx)
{
    যদি (lhs->lhs_type->tt_type == VAR_OBJECT)
    {
	// "this.value": load "this" object and get the value at index for an
	// object or class member get the type of the member.
	// Also for "obj.value".
	char_u *dot = vim_strchr(var_start, '.');
	যদি (dot == ফাঁকা)
	{
	    semsg(_(e_missing_dot_after_object_str), lhs->lhs_name);
	    ফেরত FAIL;
	}

	class_T	*cl = lhs->lhs_type->tt_class;
	type_T	*type = oc_member_type(cl, TRUE, dot + 1,
					  lhs->lhs_end, &lhs->lhs_member_idx);
	যদি (lhs->lhs_member_idx < 0)
	    ফেরত FAIL;

	যদি (dot - var_start == 4 && STRNCMP(var_start, "this", 4) == 0)
	{
	    // load "this"
	    lvar_T  *lvar = lhs->lhs_lvar;
	    পূর্ণসংখ্যা	    rc;

	    যদি (lvar->lv_from_outer > 0)
		rc = generate_LOADOUTER(cctx, lvar->lv_idx,
			lvar->lv_from_outer, lvar->lv_loop_depth,
			lvar->lv_loop_idx, type);
	    অন্যথা
		rc = generate_LOAD(cctx, ISN_LOAD, lvar->lv_idx, ফাঁকা, type);

	    যদি (rc == FAIL)
		ফেরত FAIL;
	}
	অন্যথা
	{
	    // load object variable or argument
	    যদি (compile_load_lhs(lhs, var_start, lhs->lhs_type, cctx) == FAIL)
		ফেরত FAIL;
	}
	যদি (IS_INTERFACE(cl))
	    ফেরত generate_GET_ITF_MEMBER(cctx, cl, lhs->lhs_member_idx, type);
	ফেরত generate_GET_OBJ_MEMBER(cctx, lhs->lhs_member_idx, type);
    }
    অন্যথা যদি (lhs->lhs_type->tt_type == VAR_CLASS)
    {
	// "<classname>.value": load class variable "classname.value"
	char_u *dot = vim_strchr(var_start, '.');
	যদি (dot == ফাঁকা)
	{
	    (শূন্য)check_type_is_মান(lhs->lhs_type);
	    ফেরত FAIL;
	}

	class_T	*cl = lhs->lhs_type->tt_class;
	ocmember_T *m = class_member_lookup(cl, dot + 1,
						lhs->lhs_end - dot - 1,
						&lhs->lhs_member_idx);
	যদি (m == ফাঁকা)
	    ফেরত FAIL;

	ফেরত generate_CLASSMEMBER(cctx, TRUE, cl, lhs->lhs_member_idx);
    }

    যদি (compile_load_lhs(lhs, var_start, ফাঁকা, cctx) == FAIL)
	ফেরত FAIL;

    যদি (lhs->lhs_has_সূচক)
    {
	পূর্ণসংখ্যা	range = FALSE;
	type_T	*type = get_type_on_স্তূপ(cctx, 0);

	// A member of an object or class is not obtained by indexing it.
	যদি (type->tt_type == VAR_CLASS
		|| (type->tt_type == VAR_OBJECT && type != &t_object_any))
	{
	    char_u *p = var_start + lhs->lhs_varlen;

	    ফেরত compile_class_object_সূচক(cctx, &p, type);
	}

	// Get member from list or dict.  First compile the
	// index value.
	যদি (compile_assign_সূচক(var_start, lhs, &range, cctx) == FAIL)
	    ফেরত FAIL;
	যদি (range)
	{
	    semsg(_(e_cannot_use_range_with_assignment_operator_str),
								    var_start);
	    ফেরত FAIL;
	}

	// Get the member.
	যদি (compile_member(FALSE, ফাঁকা, cctx) == FAIL)
	    ফেরত FAIL;
    }
    ফেরত OK;
}

/*
 * Assignment to a list or dict member, or ":unlet" for the item, using the
 * information in "lhs".
 * Returns OK or FAIL.
 */
    পূর্ণসংখ্যা
compile_assign_unlet(
    char_u	*var_start,
    lhs_T	*lhs,
    পূর্ণসংখ্যা		is_assign,
    type_T	*rhs_type,
    cctx_T	*cctx)
{
    vartype_T	dest_type;
    পূর্ণসংখ্যা		range = FALSE;

    যদি (compile_assign_সূচক(var_start, lhs, &range, cctx) == FAIL)
	ফেরত FAIL;
    যদি (is_assign && range
	    && lhs->lhs_type->tt_type != VAR_LIST
	    && lhs->lhs_type != &t_blob
	    && lhs->lhs_type != &t_any)
    {
	যদি (lhs->lhs_type->tt_type == VAR_TUPLE)
	    emsg(_(e_cannot_slice_tuple));
	অন্যথা
	    semsg(_(e_cannot_use_range_with_assignment_str), var_start);
	ফেরত FAIL;
    }

    // For "expr[idx]" the index is compiled before the expression, thus the
    // resulting type cannot be used here.
    যদি (lhs->lhs_dest == dest_রাশিমালা
	    || lhs->lhs_type == ফাঁকা || lhs->lhs_type == &t_any)
    {
	// Index on variable of unknown type: check at runtime.
	dest_type = VAR_ANY;
    }
    অন্যথা
    {
	dest_type = lhs->lhs_type->tt_type;
	যদি (dest_type == VAR_DICT && range)
	{
	    emsg(_(e_cannot_use_range_with_dictionary));
	    ফেরত FAIL;
	}
	যদি (dest_type == VAR_DICT
		&& may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
	    ফেরত FAIL;
	যদি (dest_type == VAR_LIST || dest_type == VAR_BLOB)
	{
	    type_T *type;

	    যদি (range)
	    {
		type = get_type_on_স্তূপ(cctx, 1);
		যদি (need_type(type, &t_সংখ্যা, 0,
					    -2, 0, cctx, FALSE, FALSE) == FAIL)
		ফেরত FAIL;
	    }
	    type = get_type_on_স্তূপ(cctx, 0);
	    যদি ((dest_type != VAR_BLOB && type->tt_type != VAR_SPECIAL)
		    && need_type(type, &t_সংখ্যা, 0,
					    -1, 0, cctx, FALSE, FALSE) == FAIL)
		ফেরত FAIL;
	}
    }

    যদি (cctx->ctx_skip == SKIP_YES)
	ফেরত OK;

    // Load the dict, list or object.  On the stack we then have:
    // - value (for assignment, not for :unlet)
    // - index
    // - for [a : b] second index
    // - variable
    যদি (compile_load_lhs(lhs, var_start, rhs_type, cctx) == FAIL)
	ফেরত FAIL;

    যদি (dest_type == VAR_LIST
	    || dest_type == VAR_DICT
	    || dest_type == VAR_BLOB
	    || dest_type == VAR_CLASS
	    || dest_type == VAR_OBJECT
	    || dest_type == VAR_ANY)
    {
	যদি (is_assign)
	{
	    যদি (range)
	    {
		যদি (generate_instr_drop(cctx, ISN_STORERANGE, 4) == ফাঁকা)
		    ফেরত FAIL;
	    }
	    অন্যথা
	    {
		isn_T	*isn = generate_instr_drop(cctx, ISN_STOREINDEX, 3);

		যদি (isn == ফাঁকা)
		    ফেরত FAIL;
		isn->isn_arg.storeindex.si_vartype = dest_type;
		isn->isn_arg.storeindex.si_class = ফাঁকা;

		যদি (dest_type == VAR_OBJECT)
		{
		    class_T *cl = lhs->lhs_type->tt_class;

		    যদি (IS_INTERFACE(cl))
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
	অন্যথা যদি (range)
	{
	    যদি (generate_instr_drop(cctx, ISN_UNLETRANGE, 3) == ফাঁকা)
		ফেরত FAIL;
	}
	অন্যথা
	{
	    যদি (generate_instr_drop(cctx, ISN_UNLETINDEX, 2) == ফাঁকা)
		ফেরত FAIL;
	}
    }
    অন্যথা
    {
	যদি (dest_type == VAR_TUPLE)
	    emsg(_(e_tuple_is_immutable));
	অন্যথা
	    emsg(_(e_indexable_type_required));
	ফেরত FAIL;
    }

    ফেরত OK;
}

/*
 * Generate an instruction to push the default value for "vartype".
 * if "dest_local" is TRUE then for some types no instruction is generated.
 * "skip_store" is set to TRUE if no PUSH instruction is generated.
 * Returns OK or FAIL.
 */
    স্থির পূর্ণসংখ্যা
ঠেলো_default_মান(
	cctx_T	    *cctx,
	vartype_T   vartype,
	পূর্ণসংখ্যা	    dest_is_local,
	পূর্ণসংখ্যা	    *skip_store)
{
    পূর্ণসংখ্যা r = OK;

    নির্বাচন (vartype)
    {
	ক্ষেত্র VAR_BOOL:
	    r = generate_PUSHBOOL(cctx, VVAL_FALSE);
	    বিরতি;
	ক্ষেত্র VAR_FLOAT:
	    r = generate_PUSHF(cctx, 0.0);
	    বিরতি;
	ক্ষেত্র VAR_STRING:
	    r = generate_PUSHS(cctx, ফাঁকা);
	    বিরতি;
	ক্ষেত্র VAR_BLOB:
	    r = generate_PUSHBLOB(cctx, blob_alloc());
	    বিরতি;
	ক্ষেত্র VAR_FUNC:
	    r = generate_PUSHFUNC(cctx, ফাঁকা, &t_func_void, TRUE);
	    বিরতি;
	ক্ষেত্র VAR_LIST:
	    r = generate_NEWLIST(cctx, 0, FALSE);
	    বিরতি;
	ক্ষেত্র VAR_TUPLE:
	    r = generate_NEWTUPLE(cctx, 0, FALSE);
	    বিরতি;
	ক্ষেত্র VAR_DICT:
	    r = generate_NEWDICT(cctx, 0, FALSE);
	    বিরতি;
	ক্ষেত্র VAR_JOB:
	    r = generate_PUSHJOB(cctx);
	    বিরতি;
	ক্ষেত্র VAR_CHANNEL:
	    r = generate_PUSHCHANNEL(cctx);
	    বিরতি;
	ক্ষেত্র VAR_OBJECT:
	    r = generate_PUSHOBJ(cctx);
	    বিরতি;
	ক্ষেত্র VAR_সংখ্যা:
	ক্ষেত্র VAR_UNKNOWN:
	ক্ষেত্র VAR_ANY:
	ক্ষেত্র VAR_PARTIAL:
	ক্ষেত্র VAR_VOID:
	ক্ষেত্র VAR_INSTR:
	ক্ষেত্র VAR_CLASS:
	ক্ষেত্র VAR_TYPEALIAS:
	ক্ষেত্র VAR_SPECIAL:  // cannot happen
	    // This is skipped for local variables, they are always
	    // initialized to zero.  But in a "for" or "while" loop
	    // the value may have been changed.
	    যদি (dest_is_local && !inside_loop_scope(cctx))
		*skip_store = TRUE;
	    অন্যথা
		r = generate_PUSHNR(cctx, 0);
	    বিরতি;
    }
    ফেরত r;
}

/*
 * Compile assignment context.  Used when compiling an assignment statement.
 */
প্রকারলেখ কাঠামো cac_S cac_T;
কাঠামো cac_S
{
    cmdidx_T	cac_cmdidx;		// assignment command
    char_u	*cac_nextc;		// next character to parse
    lhs_T	cac_lhs;		// lhs of the assignment
    type_T	*cac_rhs_type;		// rhs type of an assignment
    char_u	*cac_op;		// assignment operator
    পূর্ণসংখ্যা		cac_oplen;		// assignment operator length
    char_u	*cac_var_start;		// start of the variable names
    char_u	*cac_var_end;		// end of the variable names
    পূর্ণসংখ্যা		cac_var_গুনতি;		// number of variables in assignment
    পূর্ণসংখ্যা		cac_var_idx;		// variable index in a list
    পূর্ণসংখ্যা		cac_semicolon;		// semicolon in [var1, var2; var3]
    garray_T	*cac_instr;
    পূর্ণসংখ্যা		cac_instr_গুনতি;
    পূর্ণসংখ্যা		cac_incdec;
    পূর্ণসংখ্যা		cac_did_generate_slice;
    পূর্ণসংখ্যা		cac_is_decl;
    পূর্ণসংখ্যা		cac_is_const;
    পূর্ণসংখ্যা		cac_start_lnum;
    type_T	*cac_inferred_type;
    পূর্ণসংখ্যা		cac_skip_store;
};

/*
 * Initialize the compile assignment context.
 */
    স্থির শূন্য
compile_assign_context_init(cac_T *cac, cctx_T *cctx, পূর্ণসংখ্যা cmdidx, char_u *arg)
{
    CLEAR_FIELD(*cac);
    cac->cac_cmdidx = cmdidx;
    cac->cac_instr = &cctx->ctx_instr;
    cac->cac_rhs_type = &t_any;
    cac->cac_is_decl = is_decl_command(cmdidx);
    cac->cac_start_lnum = SOURCING_LNUM;
    cac->cac_instr_গুনতি = -1;
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
    স্থির পূর্ণসংখ্যা
compile_assign_obj_new_arg(char_u **argp, cctx_T *cctx)
{
    char_u *arg = *argp;

    arg += 11;	    // skip "ifargisset"
    পূর্ণসংখ্যা def_arg_idx = getdigits(&arg);
    arg = skipwhite(arg);

    // Use a JUMP_IF_ARG_NOT_SET instruction to skip if the value was not
    // given and the default value is "v:none".
    পূর্ণসংখ্যা স্তূপ_offset = স্তূপ_FRAME_আকার +
				(cctx->ctx_ufunc->uf_va_name != ফাঁকা ? 1 : 0);
    পূর্ণসংখ্যা def_arg_গুনতি = cctx->ctx_ufunc->uf_def_args.ga_len;
    পূর্ণসংখ্যা arg_offset = def_arg_idx - def_arg_গুনতি - স্তূপ_offset;

    যদি (generate_JUMP_IF_ARG(cctx, ISN_JUMP_IF_ARG_NOT_SET,
			     arg_offset) == FAIL)
	ফেরত FAIL;

    *argp = arg;
    ফেরত OK;
}

/*
 * Translate the increment (++) and decrement (--) operators to the
 * corresponding compound operators (+= or -=).
 *
 * Returns OK on success and FAIL on syntax error.
 */
    স্থির পূর্ণসংখ্যা
translate_incdec_op(exarg_T *eap, cac_T *cac)
{
    যদি (VIM_ISWHITE(eap->cmd[2]))
    {
	semsg(_(e_no_white_space_allowed_after_str_str),
		eap->cmdidx == CMD_increment ? "++" : "--", eap->cmd);
	ফেরত FAIL;
    }
    cac->cac_op = (char_u *)(eap->cmdidx == CMD_increment ? "+=" : "-=");
    cac->cac_oplen = 2;
    cac->cac_incdec = TRUE;

    ফেরত OK;
}

/*
 * Process the operator in an assignment statement.
 */
    স্থির পূর্ণসংখ্যা
compile_assign_process_operator(
    exarg_T	*eap,
    char_u	*arg,
    cac_T	*cac,
    পূর্ণসংখ্যা		*heredoc,
    char_u	**retstr)
{
    *retstr = ফাঁকা;

    যদি (eap->cmdidx == CMD_increment || eap->cmdidx == CMD_decrement)
	// Change an unary operator to a compound operator
	ফেরত translate_incdec_op(eap, cac);

    char_u *sp = cac->cac_nextc;
    cac->cac_nextc = skipwhite(cac->cac_nextc);
    cac->cac_op = cac->cac_nextc;
    cac->cac_oplen = assignment_len(cac->cac_nextc, heredoc);

    যদি (cac->cac_var_গুনতি > 0 && cac->cac_oplen == 0)
    {
	// can be something like "[1, 2]->func()"
	*retstr = arg;
	ফেরত FAIL;
    }

    // need white space before and after the operator
    যদি (cac->cac_oplen > 0 && (!VIM_ISWHITE(*sp)
		|| !IS_WHITE_OR_NUL(cac->cac_op[cac->cac_oplen])))
    {
	error_white_both(cac->cac_op, cac->cac_oplen);
	ফেরত FAIL;
    }

    ফেরত OK;
}

/*
 * Find the start of an assignment statement.
 */
    স্থির char_u *
compile_assign_compute_start(char_u *arg, পূর্ণসংখ্যা var_গুনতি)
{
    যদি (var_গুনতি > 0)
	// [var1, var2] = [val1, val2]
	// skip over the "["
	ফেরত skipwhite(arg + 1);

    ফেরত arg;
}

/*
 * Parse a heredoc assignment starting at "p".  Returns a pointer to the
 * beginning of the heredoc content.
 */
    স্থির char_u *
parse_heredoc_assignment(exarg_T *eap, cctx_T *cctx, cac_T *cac)
{
    // [let] varname =<< [trim] {end}
    eap->ea_getline = exarg_getline;
    eap->cookie = cctx;

    list_T *l = heredoc_get(eap, cac->cac_nextc + 3, FALSE, TRUE);
    যদি (l == ফাঁকা)
	ফেরত ফাঁকা;

    list_free(l);
    cac->cac_nextc += STRLEN(cac->cac_nextc);

    ফেরত cac->cac_nextc;
}

/*
 * Check the type of a RHS expression in a list assignment statement.
 * The RHS expression is already compiled.  So the type is on the stack.
 */
    স্থির পূর্ণসংখ্যা
compile_assign_list_check_rhs_type(cctx_T *cctx, cac_T *cac)
{
    type_T	*stacktype;

    stacktype = cctx->ctx_type_স্তূপ.ga_len == 0 ? &t_void
						: get_type_on_স্তূপ(cctx, 0);
    যদি (stacktype->tt_type == VAR_VOID)
    {
	emsg(_(e_cannot_use_void_মান));
	ফেরত FAIL;
    }

    যদি (stacktype->tt_type != VAR_LIST && stacktype->tt_type != VAR_TUPLE
					&& stacktype->tt_type != VAR_ANY)
    {
	emsg(_(e_list_or_tuple_required));
	ফেরত FAIL;
    }

    যদি (need_type(stacktype,
		  stacktype->tt_type == VAR_TUPLE ? &t_tuple_any : &t_list_any,
		  TYPECHK_TUPLE_OK, -1, 0, cctx, FALSE, FALSE) == FAIL)
	ফেরত FAIL;

    // The check accepts both a list and a tuple. Keep "any", making it a list
    // would reject a tuple later on.
    যদি (stacktype->tt_type == VAR_ANY)
	set_type_on_স্তূপ(cctx, &t_any, 0);

    যদি (stacktype->tt_type == VAR_TUPLE)
    {
	যদি (stacktype->tt_argcount != 1)
	    cac->cac_rhs_type = &t_any;
	অন্যথা
	{
	    যদি (stacktype->tt_flags & TTFLAG_VARARGS)
		cac->cac_rhs_type = stacktype->tt_args[0]->tt_member;
	    অন্যথা
		cac->cac_rhs_type = stacktype->tt_args[0];
	}
    }
    অন্যথা যদি (stacktype->tt_member != ফাঁকা)
	cac->cac_rhs_type = stacktype->tt_member;

    ফেরত OK;
}

/*
 * In a list assignment statement, if a constant list was used, check the
 * length.  Returns OK if the length check succeeds.  Returns FAIL otherwise.
 */
    স্থির পূর্ণসংখ্যা
compile_assign_list_check_দৈর্ঘ্য(cctx_T *cctx, cac_T *cac)
{
    পূর্ণসংখ্যা	needed_list_len;
    পূর্ণসংখ্যা	did_check = FALSE;

    needed_list_len = cac->cac_semicolon
				? cac->cac_var_গুনতি - 1
				: cac->cac_var_গুনতি;
    যদি (cac->cac_instr->ga_len > 0)
    {
	isn_T	*isn = ((isn_T *)cac->cac_instr->ga_তথ্য) +
						cac->cac_instr->ga_len - 1;

	যদি (isn->isn_type == ISN_NEWLIST || isn->isn_type == ISN_NEWTUPLE)
	{
	    did_check = TRUE;
	    যদি (cac->cac_semicolon ?
			isn->isn_arg.সংখ্যা < needed_list_len
			: isn->isn_arg.সংখ্যা != needed_list_len)
	    {
		semsg(_(e_expected_nr_বস্তুসমূহ_but_got_nr),
			needed_list_len, (পূর্ণসংখ্যা)isn->isn_arg.সংখ্যা);
		ফেরত FAIL;
	    }
	}
    }

    যদি (!did_check)
	generate_CHECKLEN(cctx, needed_list_len, cac->cac_semicolon);

    ফেরত OK;
}

/*
 * Evaluate the expression for "[var, var] = expr" assignment.
 * A line break may follow the assignment operator "=".
 */
    স্থির char_u *
compile_assign_list_রাশিমালা(cctx_T *cctx, cac_T *cac)
{
    char_u *whitep;

    whitep = cac->cac_op + cac->cac_oplen;

    যদি (may_get_পরবর্তী_সারি_error(whitep, &cac->cac_nextc, cctx) == FAIL)
	ফেরত ফাঁকা;

    // compile RHS expression
    যদি (compile_expr0(&cac->cac_nextc, cctx) == FAIL)
	ফেরত ফাঁকা;

    যদি (cctx->ctx_skip == SKIP_YES)
	// no need to parse more when skipping
	ফেরত cac->cac_nextc;

    যদি (compile_assign_list_check_rhs_type(cctx, cac) == FAIL)
	ফেরত ফাঁকা;

    // If a constant list was used we can check the length right here.
    যদি (compile_assign_list_check_দৈর্ঘ্য(cctx, cac) == FAIL)
	ফেরত FAIL;

    ফেরত cac->cac_nextc;
}

/*
 * Find and return the end of a heredoc or a list of variables assignment
 * statement.  For a single variable assignment statement, returns the current
 * end.
 * Returns NULL on failure.
 */
    স্থির char_u *
compile_assign_compute_end(
    exarg_T	*eap,
    cctx_T	*cctx,
    cac_T	*cac,
    পূর্ণসংখ্যা		heredoc)
{
    যদি (heredoc)
    {
	cac->cac_nextc = parse_heredoc_assignment(eap, cctx, cac);
	ফেরত cac->cac_nextc;
    }

    যদি (cac->cac_var_গুনতি > 0)
    {
	// for "[var, var] = expr" evaluate the expression. The list of
	// variables are processed later.
	// A line break may follow the "=".
	cac->cac_nextc = compile_assign_list_রাশিমালা(cctx, cac);
	ফেরত cac->cac_nextc;
    }

    ফেরত cac->cac_var_end;
}

/*
 * For "var = expr" evaluate the expression.
 */
    স্থির পূর্ণসংখ্যা
compile_assign_single_eval_রাশিমালা(cctx_T *cctx, cac_T *cac)
{
    পূর্ণসংখ্যা		ret = OK;
    char_u	*whitep;
    lhs_T	*lhs = &cac->cac_lhs;
    lvar_T	*lvp;
    lvar_T	save_lhs_lvar;

    // Compile the expression.
    যদি (cac->cac_incdec)
	ফেরত generate_PUSHNR(cctx, 1);

    // Temporarily hide the new local variable here, it is
    // not available to this expression.
    যদি (lhs->lhs_new_local)
    {
	--cctx->ctx_locals.ga_len;

	// Save the local variable value (compiling the RHS expression may
	// create new local variables).
	lvp = ((lvar_T *)cctx->ctx_locals.ga_তথ্য) + cctx->ctx_locals.ga_len;
	save_lhs_lvar = *lvp;
    }
    whitep = cac->cac_op + cac->cac_oplen;

    যদি (may_get_পরবর্তী_সারি_error(whitep, &cac->cac_nextc, cctx) == FAIL)
    {
	যদি (lhs->lhs_new_local)
	    ++cctx->ctx_locals.ga_len;
	ফেরত FAIL;
    }

    ret = compile_expr0_ext(&cac->cac_nextc, cctx, &cac->cac_is_const);
    যদি (lhs->lhs_new_local)
    {
	// Restore the local variable value.  Update lhs_lvar as the index of
	// the local variable might have changed.
	lvp = ((lvar_T *)cctx->ctx_locals.ga_তথ্য) + cctx->ctx_locals.ga_len;
	*lvp = save_lhs_lvar;
	lhs->lhs_lvar = lvp;

	++cctx->ctx_locals.ga_len;
    }

    ফেরত ret;
}

/*
 * When compiling an assignment, set the LHS type to the RHS type.
 */
    স্থির পূর্ণসংখ্যা
compile_assign_set_lhs_type_from_rhs(
    cctx_T	*cctx,
    cac_T	*cac,
    lhs_T	*lhs,
    type_T	*rhs_type)
{
    যদি (rhs_type->tt_type == VAR_VOID)
    {
	emsg(_(e_cannot_use_void_মান));
	ফেরত FAIL;
    }

    type_T *type;

    // An empty list or dict has a &t_unknown member, for a variable that
    // implies &t_any.
    যদি (rhs_type == &t_list_empty)
	type = &t_list_any;
    অন্যথা যদি (rhs_type == &t_dict_empty)
	type = &t_dict_any;
    অন্যথা যদি (rhs_type == &t_unknown)
	type = &t_any;
    অন্যথা
    {
	type = rhs_type;
	cac->cac_inferred_type = rhs_type;
    }

    set_var_type(lhs->lhs_lvar, type, cctx);

    ফেরত OK;
}

/*
 * Returns TRUE if the "rhs_type" can be assigned to the "lhs" variable.
 * Used when compiling an assignment statement.
 */
    স্থির পূর্ণসংখ্যা
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
    যদি (cac->cac_var_গুনতি > 0)
    {
	where.wt_সূচক = cac->cac_var_idx + 1;
	where.wt_kind = WT_VARIABLE;
    }

    // If assigning to a list or dict member, use the member type.
    // Not for "list[:] =".
    যদি (lhs->lhs_has_সূচক &&
	    !has_list_সূচক(cac->cac_var_start + lhs->lhs_varlen, cctx))
	use_type = lhs->lhs_member_type;

    যদি (need_type_where(rhs_type, use_type, 0, -1, where, cctx, FALSE,
						cac->cac_is_const) == FAIL)
	ফেরত FALSE;

    ফেরত TRUE;
}

/*
 * Compare the LHS type with the RHS type in an assignment.
 */
    স্থির পূর্ণসংখ্যা
compile_assign_check_type(cctx_T *cctx, cac_T *cac)
{
    lhs_T	*lhs = &cac->cac_lhs;
    type_T	*rhs_type;

    rhs_type = cctx->ctx_type_স্তূপ.ga_len == 0
					? &t_void
					: get_type_on_স্তূপ(cctx, 0);
    cac->cac_rhs_type = rhs_type;

    যদি (check_type_is_মান(rhs_type) == FAIL)
	ফেরত FAIL;

    যদি (lhs->lhs_lvar != ফাঁকা && (cac->cac_is_decl || !lhs->lhs_has_type))
    {
	যদি (rhs_type->tt_type == VAR_FUNC
					|| rhs_type->tt_type == VAR_PARTIAL)
	{
	    // Make sure the variable name can be used as a funcref
	    যদি (!lhs->lhs_has_সূচক
				&& var_wrong_func_name(lhs->lhs_name, TRUE))
		ফেরত FAIL;
	}

	যদি (lhs->lhs_new_local && !lhs->lhs_has_type)
	{
	    // The LHS variable doesn't have a type.  Set it to the RHS type.
	    যদি (compile_assign_set_lhs_type_from_rhs(cctx, cac, lhs, rhs_type)
								== FAIL)
		ফেরত FAIL;
	}
	অন্যথা যদি (*cac->cac_op == '=')
	{
	    যদি (!compile_assign_valid_rhs_type(cctx, cac, lhs, rhs_type))
		ফেরত FAIL;
	}
    }
    অন্যথা
    {
	// Assigning to a register using @r = "abc"

	type_T *lhs_type = lhs->lhs_member_type;

	// Special case: assigning to @# can use a number or a string.
	// Also: can assign a number to a float.
	যদি ((lhs_type == &t_সংখ্যা_or_string || lhs_type == &t_float)
					&& rhs_type->tt_type == VAR_সংখ্যা)
	    lhs_type = &t_সংখ্যা;

	যদি (*cac->cac_nextc != '=')
	{
	    যদি (need_type(rhs_type, lhs_type, 0, -1, 0, cctx, FALSE,
							FALSE) == FAIL)
		ফেরত FAIL;
	}
    }

    ফেরত OK;
}

/*
 * Compile the RHS expression in an assignment statement and generate the
 * instructions.
 */
    স্থির পূর্ণসংখ্যা
compile_assign_rhs_রাশিমালা(cctx_T *cctx, cac_T *cac)
{
    cac->cac_is_const = FALSE;

    // for "+=", "*=", "..=" etc. first load the current value
    যদি (*cac->cac_op != '='
	    && compile_load_lhs_with_সূচক(&cac->cac_lhs, cac->cac_var_start,
								cctx) == FAIL)
	ফেরত FAIL;

    // For "var = expr" evaluate the expression.
    যদি (cac->cac_var_গুনতি == 0)
    {
	পূর্ণসংখ্যা	ret;

	// Compile the expression.
	cac->cac_instr_গুনতি = cac->cac_instr->ga_len;
	ret = compile_assign_single_eval_রাশিমালা(cctx, cac);
	যদি (ret == FAIL)
	    ফেরত FAIL;
    }
    অন্যথা যদি (cac->cac_semicolon && cac->cac_var_idx == cac->cac_var_গুনতি - 1)
    {
	// For "[var; var] = expr" get the rest of the list
	cac->cac_did_generate_slice = TRUE;
	যদি (generate_SLICE(cctx, cac->cac_var_গুনতি - 1) == FAIL)
	    ফেরত FAIL;
    }
    অন্যথা
    {
	// For "[var, var] = expr" get the "var_idx" item from the
	// list.
	পূর্ণসংখ্যা with_op = *cac->cac_op != '=';
	যদি (generate_GETITEM(cctx, cac->cac_var_idx, with_op) == FAIL)
	    ফেরত FAIL;
    }

    যদি (compile_assign_check_type(cctx, cac) == FAIL)
	ফেরত FAIL;

    ফেরত OK;
}

/*
 * Compile the RHS expression in an assignment
 */
    স্থির পূর্ণসংখ্যা
compile_assign_rhs(cctx_T *cctx, cac_T *cac)
{
    lhs_T	*lhs = &cac->cac_lhs;

    যদি (cctx->ctx_skip == SKIP_YES)
    {
	যদি (cac->cac_oplen > 0 && cac->cac_var_গুনতি == 0)
	{
	    // skip over the "=" and the expression
	    cac->cac_nextc = skipwhite(cac->cac_op + cac->cac_oplen);
	    (শূন্য)compile_expr0(&cac->cac_nextc, cctx);
	}
	ফেরত OK;
    }

    // If RHS is specified, then generate instructions for RHS expression
    যদি (cac->cac_oplen > 0)
	ফেরত compile_assign_rhs_রাশিমালা(cctx, cac);

    যদি (cac->cac_cmdidx == CMD_final)
    {
	emsg(_(e_final_requires_a_মান));
	ফেরত FAIL;
    }

    যদি (cac->cac_cmdidx == CMD_const)
    {
	emsg(_(e_const_requires_a_মান));
	ফেরত FAIL;
    }

    যদি (!lhs->lhs_has_type || lhs->lhs_dest == dest_option
					|| lhs->lhs_dest == dest_func_option)
    {
	emsg(_(e_type_or_initialization_required));
	ফেরত FAIL;
    }

    // variables are always initialized
    যদি (GA_GROW_FAILS(cac->cac_instr, 1))
	ফেরত FAIL;

    cac->cac_instr_গুনতি = cac->cac_instr->ga_len;

    ফেরত ঠেলো_default_মান(cctx, lhs->lhs_member_type->tt_type,
			      lhs->lhs_dest == dest_local,
			      &cac->cac_skip_store);
}

/*
 * Returns OK if "type" supports compound operator "op_arg" (e.g. +=, -=, %=,
 * etc.).  Compound operators are not supported with a tuple and a dict.
 * Returns FAIL if compound operator is not supported.
 */
    স্থির পূর্ণসংখ্যা
check_type_supports_compound_op(type_T *type, char_u op_arg)
{
    যদি (type->tt_type == VAR_TUPLE || type->tt_type == VAR_DICT)
    {
	char_u	op[2];

	op[0] = op_arg;
	op[1] = NUL;
	semsg(_(e_wrong_variable_type_for_str_equal), op);
	ফেরত FAIL;
    }

    ফেরত OK;
}

/*
 * Compile a compound op assignment statement (+=, -=, *=, %=, etc.)
 */
    স্থির পূর্ণসংখ্যা
compile_assign_compound_op(cctx_T *cctx, cac_T *cac)
{
    lhs_T	    *lhs = &cac->cac_lhs;
    type_T	    *expected;
    type_T	    *stacktype = ফাঁকা;

    যদি (cac->cac_lhs.lhs_type->tt_type == VAR_TUPLE
	    && check_type_supports_compound_op(cac->cac_lhs.lhs_type,
							*cac->cac_op) == FAIL)
	ফেরত FAIL;

    যদি (*cac->cac_op == '.')
    {
	expected = lhs->lhs_member_type;
	stacktype = get_type_on_স্তূপ(cctx, 0);
	যদি (expected != &t_string
		&& need_type(stacktype, expected, 0, -1, 0, cctx,
					FALSE, FALSE) == FAIL)
	    ফেরত FAIL;
	অন্যথা যদি (may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
	    ফেরত FAIL;
    }
    অন্যথা
    {
	expected = lhs->lhs_member_type;
	stacktype = get_type_on_স্তূপ(cctx, 0);

	যদি (check_type_supports_compound_op(expected, *cac->cac_op) == FAIL)
	    ফেরত FAIL;

	যদি (
		// If variable is float operation with number is OK.
		!(expected == &t_float && (stacktype == &t_সংখ্যা
					|| stacktype == &t_সংখ্যা_bool))
		&& need_type(stacktype, expected, TYPECHK_সংখ্যা_OK, -1, 0,
						cctx, FALSE, FALSE) == FAIL)
	    ফেরত FAIL;
    }

    যদি (*cac->cac_op == '.')
    {
	যদি (generate_CONCAT(cctx, 2) == FAIL)
	    ফেরত FAIL;
    }
    অন্যথা যদি (*cac->cac_op == '+')
    {
	যদি (generate_add_instr(cctx,
		    operator_type(lhs->lhs_member_type, stacktype),
		    lhs->lhs_member_type, stacktype,
		    রাশিমালা_APPEND) == FAIL)
	    ফেরত FAIL;
    }
    অন্যথা যদি (generate_two_op(cctx, cac->cac_op) == FAIL)
	ফেরত FAIL;

    ফেরত OK;
}

/*
 * Generate the STORE and SETTYPE instructions for an assignment statement.
 */
    স্থির পূর্ণসংখ্যা
compile_assign_generate_store(cctx_T *cctx, cac_T *cac)
{
    lhs_T	*lhs = &cac->cac_lhs;
    পূর্ণসংখ্যা		save_lnum;

    // Use the line number of the assignment for store instruction.
    save_lnum = cctx->ctx_lnum;
    cctx->ctx_lnum = cac->cac_start_lnum - 1;

    যদি (lhs->lhs_has_সূচক)
    {
	// Use the info in "lhs" to store the value at the index in the
	// list, dict or object.
	যদি (compile_assign_unlet(cac->cac_var_start, &cac->cac_lhs,
				 TRUE, cac->cac_rhs_type, cctx) == FAIL)
	{
	    cctx->ctx_lnum = save_lnum;
	    ফেরত FAIL;
	}
    }
    অন্যথা
    {
	যদি (cac->cac_is_decl && cac->cac_cmdidx == CMD_const &&
			(lhs->lhs_dest == dest_script
			 || lhs->lhs_dest == dest_script_v9
			 || lhs->lhs_dest == dest_global
			 || lhs->lhs_dest == dest_local))
	    // ":const var": lock the value, but not referenced variables
	    generate_LOCKCONST(cctx);

	type_T	*inferred_type = cac->cac_inferred_type;

	যদি ((lhs->lhs_type->tt_type == VAR_DICT
		    || lhs->lhs_type->tt_type == VAR_LIST)
		&& lhs->lhs_type->tt_member != ফাঁকা
		&& lhs->lhs_type->tt_member != &t_any
		&& lhs->lhs_type->tt_member != &t_unknown)
	    // Set the type in the list or dict, so that it can be
	    // checked, also in legacy script.
	    generate_SETTYPE(cctx, lhs->lhs_type);
	অন্যথা যদি (lhs->lhs_type->tt_type == VAR_TUPLE
					&& lhs->lhs_type->tt_argcount != 0)
	    generate_SETTYPE(cctx, lhs->lhs_type);
	অন্যথা যদি (inferred_type != ফাঁকা
		&& (inferred_type->tt_type == VAR_DICT
		    || inferred_type->tt_type == VAR_LIST)
		&& inferred_type->tt_member != ফাঁকা
		&& inferred_type->tt_member != &t_unknown
		&& inferred_type->tt_member != &t_any)
	    // Set the type in the list or dict, so that it can be
	    // checked, also in legacy script.
	    generate_SETTYPE(cctx, inferred_type);
	অন্যথা যদি (inferred_type != ফাঁকা
				&& inferred_type->tt_type == VAR_TUPLE
				&& inferred_type->tt_argcount > 0)
	    generate_SETTYPE(cctx, inferred_type);

	যদি (!cac->cac_skip_store &&
		generate_store_lhs(cctx, &cac->cac_lhs,
				   cac->cac_instr_গুনতি,
				   cac->cac_is_decl) == FAIL)
	{
	    cctx->ctx_lnum = save_lnum;
	    ফেরত FAIL;
	}
    }

    cctx->ctx_lnum = save_lnum;

    ফেরত OK;
}

/*
 * Process the variable(s) in an assignment statement
 */
    স্থির পূর্ণসংখ্যা
compile_assign_process_variables(
    cctx_T	*cctx,
    cac_T	*cac,
    পূর্ণসংখ্যা		cmdidx,
    পূর্ণসংখ্যা		heredoc,
    পূর্ণসংখ্যা		has_cmd,
    পূর্ণসংখ্যা		has_argisset_prefix,
    পূর্ণসংখ্যা		jump_instr_idx)
{
    /*
     * Loop over variables in "[var, var] = expr".
     * For "name = expr" and "var name: type" this is done only once.
     */
    জন্য (cac->cac_var_idx = 0; cac->cac_var_idx == 0 ||
	    cac->cac_var_idx < cac->cac_var_গুনতি; cac->cac_var_idx++)
    {
	যদি (cac->cac_var_start[0] == '_'
				&& !eval_isnamec(cac->cac_var_start[1]))
	{
	    // Ignore underscore in "[a, _, b] = list".
	    যদি (cac->cac_var_গুনতি > 0)
	    {
		cac->cac_var_start = skipwhite(cac->cac_var_start + 2);
		চালু;
	    }
	    emsg(_(e_cannot_use_underscore_here));
	    ফেরত FAIL;
	}
	vim_free(cac->cac_lhs.lhs_name);

	/*
	 * Figure out the LHS type and other properties.
	 */
	যদি (compile_assign_lhs(cac->cac_var_start, &cac->cac_lhs, cmdidx,
		    cac->cac_is_decl, heredoc, has_cmd,
		    cac->cac_oplen, cctx) == FAIL)
	    ফেরত FAIL;

	// Compile the RHS expression
	যদি (heredoc)
	{
	    SOURCING_LNUM = cac->cac_start_lnum;
	    যদি (cac->cac_lhs.lhs_has_type
		    && need_type(&t_list_string, cac->cac_lhs.lhs_type,
				0, -1, 0, cctx, FALSE, FALSE) == FAIL)
		ফেরত FAIL;
	}
	অন্যথা
	{
	    যদি (compile_assign_rhs(cctx, cac) == FAIL)
		ফেরত FAIL;
	    যদি (cac->cac_var_গুনতি == 0)
		cac->cac_var_end = cac->cac_nextc;
	}

	// no need to parse more when skipping
	যদি (cctx->ctx_skip == SKIP_YES)
	    বিরতি;

	যদি (cac->cac_oplen > 0 && *cac->cac_op != '=')
	{
	    যদি (compile_assign_compound_op(cctx, cac) == FAIL)
		ফেরত FAIL;
	}

	// generate the store instructions
	যদি (compile_assign_generate_store(cctx, cac) == FAIL)
	    ফেরত FAIL;

	যদি (cac->cac_var_idx + 1 < cac->cac_var_গুনতি)
	    cac->cac_var_start = skipwhite(cac->cac_lhs.lhs_end + 1);

	যদি (has_argisset_prefix)
	{
	    // set instruction index in JUMP_IF_ARG_SET to here
	    isn_T *isn = ((isn_T *)cac->cac_instr->ga_তথ্য) + jump_instr_idx;
	    isn->isn_arg.jumparg.jump_where = cac->cac_instr->ga_len;
	}
    }

    ফেরত OK;
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
    স্থির char_u *
compile_assignment(
    char_u	*arg_start,
    exarg_T	*eap,
    cmdidx_T	cmdidx,
    cctx_T	*cctx)
{
    cac_T	cac;
    char_u	*arg = arg_start;
    char_u	*retstr = ফাঁকা;
    পূর্ণসংখ্যা		heredoc = FALSE;
    পূর্ণসংখ্যা		jump_instr_idx;

    compile_assign_context_init(&cac, cctx, cmdidx, arg);

    jump_instr_idx = cac.cac_instr->ga_len;

    // process object variable initialization in a new() constructor method
    পূর্ণসংখ্যা	has_argisset_prefix = STRNCMP(arg, "ifargisset ", 11) == 0;
    যদি (has_argisset_prefix &&
			compile_assign_obj_new_arg(&arg, cctx) == FAIL)
	যাও theend;

    // Skip over the "varname" or "[varname, varname]" to get to any "=".
    cac.cac_nextc = skip_var_list(arg, TRUE, &cac.cac_var_গুনতি,
						&cac.cac_semicolon, TRUE);
    যদি (cac.cac_nextc == ফাঁকা)
	ফেরত *arg == '[' ? arg : ফাঁকা;

    যদি (compile_assign_process_operator(eap, arg, &cac, &heredoc,
							&retstr) == FAIL)
	ফেরত retstr;

    // Compute the start of the assignment
    cac.cac_var_start = compile_assign_compute_start(arg, cac.cac_var_গুনতি);

    // Compute the end of the assignment
    cac.cac_var_end = compile_assign_compute_end(eap, cctx, &cac, heredoc);
    যদি (cac.cac_var_end == ফাঁকা)
	ফেরত ফাঁকা;

    পূর্ণসংখ্যা has_cmd = cac.cac_var_start > eap->cmd;

    /* process the variable(s) */
    যদি (compile_assign_process_variables(cctx, &cac, cmdidx, heredoc,
					 has_cmd, has_argisset_prefix,
					 jump_instr_idx) == FAIL)
	যাও theend;

    // For "[var, var] = expr" drop the "expr" value.
    // Also for "[var, var; _] = expr".
    যদি (cctx->ctx_skip != SKIP_YES && cac.cac_var_গুনতি > 0 &&
	    (!cac.cac_semicolon || !cac.cac_did_generate_slice))
    {
	যদি (generate_instr_drop(cctx, ISN_DROP, 1) == ফাঁকা)
	    যাও theend;
    }

    retstr = skipwhite(cac.cac_var_end);

theend:
    vim_free(cac.cac_lhs.lhs_name);
    ফেরত retstr;
}

/*
 * Check for an assignment at "eap->cmd", compile it if found.
 * Return NOTDONE if there is none, FAIL for failure, OK if done.
 */
    স্থির পূর্ণসংখ্যা
may_compile_assignment(exarg_T *eap, char_u **সারি, cctx_T *cctx)
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
    যদি (p > eap->cmd && *p != NUL)
    {
	char_u *var_end;
	পূর্ণসংখ্যা	oplen;
	পূর্ণসংখ্যা	heredoc;

	যদি (eap->cmd[0] == '@')
	    var_end = eap->cmd + 2;
	অন্যথা
	    var_end = find_name_end(pskip, ফাঁকা, ফাঁকা,
					FNE_CHECK_START | FNE_INCL_BR);
	oplen = assignment_len(skipwhite(var_end), &heredoc);
	যদি (oplen > 0)
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
	    যদি (*eap->cmd == '&'
		    || *eap->cmd == '$'
		    || *eap->cmd == '@'
		    || ((len) > 2 && eap->cmd[1] == ':')
		    || STRNCMP(eap->cmd, "g:[", 3) == 0
		    || variable_exists(eap->cmd, len, cctx))
	    {
		*সারি = compile_assignment(eap->cmd, eap, CMD_আকার, cctx);
		যদি (*সারি == ফাঁকা || *সারি == eap->cmd)
		    ফেরত FAIL;
		ফেরত OK;
	    }
	}
    }

    // might be "[var, var] = expr" or "ifargisset this.member = expr"
    যদি (*eap->cmd == '[' || STRNCMP(eap->cmd, "ifargisset ", 11) == 0)
    {
	*সারি = compile_assignment(eap->cmd, eap, CMD_আকার, cctx);
	যদি (*সারি == ফাঁকা)
	    ফেরত FAIL;
	যদি (*সারি != eap->cmd)
	    ফেরত OK;
    }
    ফেরত NOTDONE;
}

/*
 * Check if arguments of "ufunc" shadow variables in "cctx".
 * Return OK or FAIL.
 */
    স্থির পূর্ণসংখ্যা
check_args_shadowing(ufunc_T *ufunc, cctx_T *cctx)
{
    পূর্ণসংখ্যা	    i;
    char_u  *arg;
    পূর্ণসংখ্যা	    r = OK;

    // Make sure arguments are not found when compiling a second time.
    ufunc->uf_args_visible = 0;

    // Check for arguments shadowing variables from the context.
    জন্য (i = 0; i < ufunc->uf_args.ga_len; ++i)
    {
	arg = ((char_u **)(ufunc->uf_args.ga_তথ্য))[i];
	যদি (check_defined(arg, STRLEN(arg), cctx, ফাঁকা, TRUE) == FAIL)
	{
	    r = FAIL;
	    বিরতি;
	}
    }
    ufunc->uf_args_visible = ufunc->uf_args.ga_len;
    ফেরত r;
}

#ifdef HAS_MESSAGE_WINDOW
/*
 * Get a count before a command.  Can only be a number.
 * Returns zero if there is no count.
 * Returns -1 if there is something wrong.
 */
    স্থির long
get_cmd_গুনতি(char_u *সারি, exarg_T *eap)
{
    char_u *p;

    // skip over colons and white space
    জন্য (p = সারি; *p == ':' || VIM_ISWHITE(*p); ++p)
	;
    যদি (!SAFE_isdigit(*p))
    {
	// The command or modifiers must be following.  Assume a lower case
	// character means there is a modifier.
	যদি (p < eap->cmd && !vim_islower(*p))
	{
	    emsg(_(e_invalid_range));
	    ফেরত -1;
	}
	ফেরত 0;
    }
    ফেরত atol((অক্ষর *)p);
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

    যদি (debug_break_level > 0 || may_break_in_function(ufunc))
	ফেরত CT_DEBUG;
#ifdef FEAT_PROFILE
    যদি (do_profiling == PROF_YES)
    {
	যদি (!ufunc->uf_profiling && has_profiling(FALSE, ufunc->uf_name, ফাঁকা,
							    &ufunc->uf_hash))
	    func_do_profile(ufunc);
	যদি (ufunc->uf_profiling)
	    ফেরত CT_PROFILE;
    }
#endif
    ফেরত CT_NONE;
}

/*
 * Free the compiled instructions saved for a def function.  This is used when
 * compiling a def function and the function was compiled before.
 * The index is reused.
 */
    স্থির শূন্য
clear_def_function(ufunc_T *ufunc, compiletype_T compile_type)
{
    isn_T	*instr_dest = ফাঁকা;
    dfunc_T	*dfunc;

    dfunc = ((dfunc_T *)def_functions.ga_তথ্য) + ufunc->uf_dfunc_idx;

    নির্বাচন (compile_type)
    {
	ক্ষেত্র CT_PROFILE:
#ifdef FEAT_PROFILE
	    instr_dest = dfunc->df_instr_prof; বিরতি;
#endif
	ক্ষেত্র CT_NONE:   instr_dest = dfunc->df_instr; বিরতি;
	ক্ষেত্র CT_DEBUG:  instr_dest = dfunc->df_instr_debug; বিরতি;
    }

    যদি (instr_dest != ফাঁকা)
	// Was compiled in this mode before: Free old instructions.
	delete_def_function_contents(dfunc, FALSE);

    ga_clear_strings(&dfunc->df_var_names);
    dfunc->df_defer_var_idx = 0;
}

/*
 * Add a function to the list of :def functions.
 * This sets "ufunc->uf_dfunc_idx" but the function isn't compiled yet.
 */
    স্থির পূর্ণসংখ্যা
add_def_function(ufunc_T *ufunc)
{
    dfunc_T *dfunc;

    যদি (def_functions.ga_len == 0)
    {
	// The first position is not used, so that a zero uf_dfunc_idx means it
	// wasn't set.
	যদি (GA_GROW_FAILS(&def_functions, 1))
	    ফেরত FAIL;
	++def_functions.ga_len;
    }

    // Add the function to "def_functions".
    যদি (GA_GROW_FAILS(&def_functions, 1))
	ফেরত FAIL;
    dfunc = ((dfunc_T *)def_functions.ga_তথ্য) + def_functions.ga_len;
    CLEAR_POINTER(dfunc);
    dfunc->df_idx = def_functions.ga_len;
    ufunc->uf_dfunc_idx = dfunc->df_idx;
    dfunc->df_ufunc = ufunc;
    dfunc->df_name = vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
    ga_init2(&dfunc->df_var_names, আকার(char_u *), 10);
    ++dfunc->df_refcount;
    ++def_functions.ga_len;
    ফেরত OK;
}

    স্থির পূর্ণসংখ্যা
compile_dfunc_ufunc_init(
    ufunc_T		*ufunc,
    cctx_T		*outer_cctx,
    compiletype_T	compile_type,
    পূর্ণসংখ্যা			*new_def_function)
{
    // When using a function that was compiled before: Free old instructions.
    // The index is reused.  Otherwise add a new entry in "def_functions".
    যদি (ufunc->uf_dfunc_idx > 0)
	clear_def_function(ufunc, compile_type);
    অন্যথা
    {
	যদি (add_def_function(ufunc) == FAIL)
	    ফেরত FAIL;

	*new_def_function = TRUE;
    }

    যদি ((ufunc->uf_flags & FC_CLOSURE) && outer_cctx == ফাঁকা)
    {
	semsg(_(e_compiling_closure_without_context_str),
						printable_func_name(ufunc));
	ফেরত FAIL;
    }

    ufunc->uf_def_status = UF_COMPILING;

    ফেরত OK;
}

/*
 * Initialize the compilation context for compiling a def function.
 */
    স্থির শূন্য
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
    ga_init2(&cctx->ctx_locals, আকার(lvar_T), 10);
    // Each entry on the type stack consists of two type pointers.
    ga_init2(&cctx->ctx_type_স্তূপ, আকার(type2_T), 50);
    cctx->ctx_type_list = &ufunc->uf_type_list;
    ga_init2(&cctx->ctx_instr, আকার(isn_T), 50);
}

/*
 * For an object constructor, generate instruction to setup "this" (the first
 * local variable) and to initialize the object variables.
 */
    স্থির পূর্ণসংখ্যা
obj_constructor_prologue(ufunc_T *ufunc, cctx_T *cctx)
{
    generate_CONSTRUCT(cctx, ufunc->uf_class);

    জন্য (পূর্ণসংখ্যা i = 0; i < ufunc->uf_class->class_obj_member_গুনতি; ++i)
    {
	ocmember_T *m = &ufunc->uf_class->class_obj_members[i];

	যদি (i < 2 && IS_ENUM(ufunc->uf_class))
	    // The first two object variables in an enum are the name
	    // and the ordinal.  These are set by the ISN_CONSTRUCT
	    // instruction.  So don't generate instructions to set
	    // these variables.
	    চালু;

	যদি (m->ocm_init != ফাঁকা)
	{
	    char_u	*রাশিমালা = m->ocm_init;
	    sctx_T	save_current_sctx;
	    পূর্ণসংখ্যা		change_sctx = FALSE;

	    // If the member variable initialization script context is
	    // different from the current script context, then change it.
	    যদি (current_sctx.sc_sid != m->ocm_init_sctx.sc_sid)
		change_sctx = TRUE;

	    যদি (change_sctx)
	    {
		// generate an instruction to change the script context to the
		// member variable initialization script context.
		save_current_sctx = current_sctx;
		current_sctx = m->ocm_init_sctx;
		generate_SCRIPTCTX_SET(cctx, current_sctx);
	    }

	    পূর্ণসংখ্যা r = compile_expr0(&রাশিমালা, cctx);

	    যদি (change_sctx)
	    {
		// restore the previous script context
		current_sctx = save_current_sctx;
		generate_SCRIPTCTX_SET(cctx, current_sctx);
	    }

	    যদি (r == FAIL)
		ফেরত FAIL;

	    যদি (!ends_excmd2(m->ocm_init, রাশিমালা))
	    {
		semsg(_(e_trailing_characters_str), রাশিমালা);
		ফেরত FAIL;
	    }

	    type_T	*type = get_type_on_স্তূপ(cctx, 0);
	    যদি (m->ocm_type->tt_type == VAR_ANY
		    && !(m->ocm_flags & OCMFLAG_HAS_TYPE)
		    && type->tt_type != VAR_SPECIAL)
	    {
		// If the member variable type is not yet set, then use
		// the initialization expression type.
		m->ocm_type = type;
	    }
	    অন্যথা
	    {
		// The type of the member initialization expression is
		// determined at run time.  Add a runtime type check.
		where_T	where = WHERE_INIT;
		where.wt_kind = WT_MEMBER;
		where.wt_func_name = (অক্ষর *)m->ocm_name.string;
		যদি (need_type_where(type, m->ocm_type, 0, -1,
					where, cctx, FALSE, FALSE) == FAIL)
		    ফেরত FAIL;
	    }
	}
	অন্যথা
	    ঠেলো_default_মান(cctx, m->ocm_type->tt_type, FALSE, ফাঁকা);

	যদি (((m->ocm_type->tt_type == VAR_DICT
			|| m->ocm_type->tt_type == VAR_LIST)
		    && m->ocm_type->tt_member != ফাঁকা
		    && m->ocm_type->tt_member != &t_any
		    && m->ocm_type->tt_member != &t_unknown)
		|| (m->ocm_type->tt_type == VAR_TUPLE
		    && m->ocm_type->tt_argcount > 0))
	    // Set the type in the list, tuple or dict, so that it can be
	    // checked, also in legacy script.
	    generate_SETTYPE(cctx, m->ocm_type);

	generate_STORE_THIS(cctx, i);
    }

    ফেরত OK;
}

/*
 * For an object method and an constructor, generate instruction to setup
 * "this" (the first local variable).  For a constructor, generate instructions
 * to initialize the object variables.
 */
    স্থির পূর্ণসংখ্যা
obj_method_prologue(ufunc_T *ufunc, cctx_T *cctx)
{
    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_তথ্য) + ufunc->uf_dfunc_idx;

    যদি (GA_GROW_FAILS(&dfunc->df_var_names, 1))
	ফেরত FAIL;

    ((char_u **)dfunc->df_var_names.ga_তথ্য)[0] =
						vim_strsave((char_u *)"this");
    ++dfunc->df_var_names.ga_len;

    // In the constructor allocate memory for the object and initialize the
    // object members.
    যদি (IS_CONSTRUCTOR_METHOD(ufunc))
	ফেরত obj_constructor_prologue(ufunc, cctx);

    ফেরত OK;
}

/*
 * Produce instructions for the default values of optional arguments.
 */
    স্থির পূর্ণসংখ্যা
compile_def_function_default_args(
    ufunc_T	*ufunc,
    garray_T	*instr,
    cctx_T	*cctx)
{
    পূর্ণসংখ্যা	গুনতি = ufunc->uf_def_args.ga_len;
    পূর্ণসংখ্যা	first_def_arg = ufunc->uf_args.ga_len - গুনতি;
    পূর্ণসংখ্যা	i;
    পূর্ণসংখ্যা	off = স্তূপ_FRAME_আকার + (ufunc->uf_va_name != ফাঁকা ? 1 : 0);
    পূর্ণসংখ্যা	did_set_arg_type = FALSE;

    // Produce instructions for the default values of optional arguments.
    SOURCING_LNUM = 0;  // line number unknown
    জন্য (i = 0; i < গুনতি; ++i)
    {
	char_u *arg = ((char_u **)(ufunc->uf_def_args.ga_তথ্য))[i];
	যদি (STRCMP(arg, "v:none") == 0)
	    // "arg = v:none" means the argument is optional without
	    // setting a value when the argument is missing.
	    চালু;

	type_T	*val_type;
	পূর্ণসংখ্যা		arg_idx = first_def_arg + i;
	where_T	where = WHERE_INIT;
	পূর্ণসংখ্যা		jump_instr_idx = instr->ga_len;
	isn_T	*isn;

	// Use a JUMP_IF_ARG_SET instruction to skip if the value was given.
	যদি (generate_JUMP_IF_ARG(cctx, ISN_JUMP_IF_ARG_SET,
						i - গুনতি - off) == FAIL)
	    ফেরত FAIL;

	// Make sure later arguments are not found.
	ufunc->uf_args_visible = arg_idx;

	পূর্ণসংখ্যা r = compile_expr0(&arg, cctx);
	যদি (r == FAIL)
	    ফেরত FAIL;

	// If no type specified use the type of the default value.
	// Otherwise check that the default value type matches the
	// specified type.
	val_type = get_type_on_স্তূপ(cctx, 0);
	where.wt_সূচক = arg_idx + 1;
	where.wt_kind = WT_ARGUMENT;
	যদি (ufunc->uf_arg_types[arg_idx] == &t_unknown)
	{
	    did_set_arg_type = TRUE;
	    ufunc->uf_arg_types[arg_idx] = val_type;
	}
	অন্যথা যদি (need_type_where(val_type, ufunc->uf_arg_types[arg_idx],
		    0, -1, where, cctx, FALSE, FALSE) == FAIL)
	    ফেরত FAIL;

	যদি (generate_STORE(cctx, ISN_STORE, i - গুনতি - off, ফাঁকা) == FAIL)
	    ফেরত FAIL;

	// set instruction index in JUMP_IF_ARG_SET to here
	isn = ((isn_T *)instr->ga_তথ্য) + jump_instr_idx;
	isn->isn_arg.jumparg.jump_where = instr->ga_len;
    }

    যদি (did_set_arg_type)
	set_function_type(ufunc);

    ফেরত OK;
}

/*
 * Compile def function body.  Loop over all the lines in the function and
 * generate instructions.
 */
    স্থির পূর্ণসংখ্যা
compile_def_function_body(
    পূর্ণসংখ্যা		last_func_lnum,
    পূর্ণসংখ্যা		check_return_type,
    garray_T	*সারিগুলি_to_free,
    অক্ষর	**errormsg,
    cctx_T	*cctx)
{
    char_u	*সারি = ফাঁকা;
    char_u	*p;
    পূর্ণসংখ্যা		did_emsg_before = did_emsg;
#ifdef FEAT_PROFILE
    পূর্ণসংখ্যা		prof_lnum = -1;
#endif
    পূর্ণসংখ্যা		debug_lnum = -1;

    জন্য (;;)
    {
	exarg_T	    ea;
	পূর্ণসংখ্যা	    starts_with_colon = FALSE;
	char_u	    *cmd;
	cmdmod_T    local_cmdmod;

	// Bail out on the first error to avoid a flood of errors and report
	// the right line number when inside try/catch.
	যদি (did_emsg_before != did_emsg)
	    ফেরত FAIL;

	যদি (সারি != ফাঁকা && *সারি == '|')
	    // the line continues after a '|'
	    ++সারি;
	অন্যথা যদি (সারি != ফাঁকা && *skipwhite(সারি) != NUL
		&& !(*সারি == '#' && (সারি == cctx->ctx_সারি_start
						    || VIM_ISWHITE(সারি[-1]))))
	{
	    semsg(_(e_trailing_characters_str), সারি);
	    ফেরত FAIL;
	}
	অন্যথা যদি (সারি != ফাঁকা && vim9_bad_comment(skipwhite(সারি)))
	    ফেরত FAIL;
	অন্যথা
	{
	    সারি = পরবর্তী_সারি_from_context(cctx, FALSE);
	    যদি (cctx->ctx_lnum >= last_func_lnum)
	    {
		// beyond the last line
#ifdef FEAT_PROFILE
		যদি (cctx->ctx_skip != SKIP_YES)
		    may_generate_prof_end(cctx, prof_lnum);
#endif
		বিরতি;
	    }
	    // Make a copy, splitting off nextcmd and removing trailing spaces
	    // may change it.
	    যদি (সারি != ফাঁকা)
	    {
		সারি = vim_strsave(সারি);
		যদি (ga_add_string(সারিগুলি_to_free, সারি) == FAIL)
		{
		    vim_free(সারি);
		    ফেরত FAIL;
		}
	    }
	}

	CLEAR_FIELD(ea);
	ea.cmdlinep = &সারি;
	ea.cmd = skipwhite(সারি);
	ea.skip = cctx->ctx_skip == SKIP_YES;

	যদি (*ea.cmd == '#')
	{
	    // "#" starts a comment, but "#{" is an error
	    যদি (vim9_bad_comment(ea.cmd))
		ফেরত FAIL;
	    সারি = (char_u *)"";
	    চালু;
	}

#ifdef FEAT_PROFILE
	যদি (cctx->ctx_compile_type == CT_PROFILE && cctx->ctx_lnum != prof_lnum
						  && cctx->ctx_skip != SKIP_YES)
	{
	    may_generate_prof_end(cctx, prof_lnum);

	    prof_lnum = cctx->ctx_lnum;
	    generate_instr(cctx, ISN_PROF_START);
	}
#endif
	যদি (cctx->ctx_compile_type == CT_DEBUG && cctx->ctx_lnum != debug_lnum
						  && cctx->ctx_skip != SKIP_YES)
	{
	    debug_lnum = cctx->ctx_lnum;
	    generate_instr_debug(cctx);
	}
	cctx->ctx_পূর্ব_lnum = cctx->ctx_lnum + 1;

	// Some things can be recognized by the first character.
	নির্বাচন (*ea.cmd)
	{
	    ক্ষেত্র '}':
		{
		    // "}" ends a block scope
		    scopetype_T stype = cctx->ctx_scope == ফাঁকা
					  ? NO_SCOPE : cctx->ctx_scope->se_type;

		    যদি (stype == BLOCK_SCOPE)
		    {
			compile_endblock(cctx);
			সারি = ea.cmd;
		    }
		    অন্যথা
		    {
			emsg(_(e_using_rcurly_outside_if_block_scope));
			ফেরত FAIL;
		    }
		    যদি (সারি != ফাঁকা)
			সারি = skipwhite(ea.cmd + 1);
		    চালু;
		}

	    ক্ষেত্র '{':
		// "{" starts a block scope
		// "{'a': 1}->func() is something else
		যদি (ends_excmd(*skipwhite(ea.cmd + 1)))
		{
		    সারি = compile_block(ea.cmd, cctx);
		    চালু;
		}
		বিরতি;
	}

	/*
	 * COMMAND MODIFIERS
	 */
	cctx->ctx_has_cmdmod = FALSE;
	যদি (parse_command_modifiers(&ea, errormsg, &local_cmdmod, FALSE)
								       == FAIL)
	    ফেরত FAIL;
	generate_cmdmods(cctx, &local_cmdmod);
	undo_cmdmod(&local_cmdmod);

	// Check if there was a colon after the last command modifier or before
	// the current position.
	জন্য (p = ea.cmd; p >= সারি; --p)
	{
	    যদি (*p == ':')
		starts_with_colon = TRUE;
	    যদি (p < ea.cmd && !VIM_ISWHITE(*p))
		বিরতি;
	}

	// Skip ":call" to get to the function name, unless using :legacy
	p = ea.cmd;
	যদি (!(local_cmdmod.cmod_flags & CMOD_LEGACY))
	{
	    যদি (checkforcmd(&ea.cmd, "call", 3))
	    {
		যদি (*ea.cmd == '(')
		    // not for "call()"
		    ea.cmd = p;
		অন্যথা
		    ea.cmd = skipwhite(ea.cmd);
	    }

	    যদি (!starts_with_colon)
	    {
		পূর্ণসংখ্যা	    assign;

		// Check for assignment after command modifiers.
		assign = may_compile_assignment(&ea, &সারি, cctx);
		যদি (assign == OK)
		    যাও nextline;
		যদি (assign == FAIL)
		    ফেরত FAIL;
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
	যদি ((*cmd != '$' || starts_with_colon)
		&& (starts_with_colon
		    || !(*cmd == '\''
			|| (cmd[0] == '0' && cmd[1] == 'z')
			|| (cmd[0] != NUL && cmd[0] == cmd[1]
					    && (*cmd == '+' || *cmd == '-'))
			|| সংখ্যা_method(cmd))))
	{
	    ea.cmd = skip_range(ea.cmd, TRUE, ফাঁকা);
	    যদি (ea.cmd > cmd)
	    {
		যদি (!starts_with_colon
				   && !(local_cmdmod.cmod_flags & CMOD_LEGACY))
		{
		    semsg(_(e_colon_required_before_range_str), cmd);
		    ফেরত FAIL;
		}
		ea.addr_গুনতি = 1;
		যদি (ends_excmd2(সারি, ea.cmd))
		{
		    // A range without a command: jump to the line.
		    generate_EXEC(cctx, ISN_EXECRANGE,
					      vim_strnsave(cmd, ea.cmd - cmd));
		    সারি = ea.cmd;
		    যাও nextline;
		}
	    }
	}
	p = find_ex_command(&ea, ফাঁকা,
		starts_with_colon || (local_cmdmod.cmod_flags & CMOD_LEGACY)
						  ? ফাঁকা : item_exists, cctx);

	যদি (p == ফাঁকা)
	{
	    যদি (cctx->ctx_skip != SKIP_YES)
		semsg(_(e_ambiguous_use_of_user_defined_command_str), ea.cmd);
	    ফেরত FAIL;
	}

	// When using ":legacy cmd" always use compile_exec().
	যদি (local_cmdmod.cmod_flags & CMOD_LEGACY)
	{
	    char_u *start = ea.cmd;

	    নির্বাচন (ea.cmdidx)
	    {
		ক্ষেত্র CMD_if:
		ক্ষেত্র CMD_elseif:
		ক্ষেত্র CMD_else:
		ক্ষেত্র CMD_endif:
		ক্ষেত্র CMD_for:
		ক্ষেত্র CMD_endfor:
		ক্ষেত্র CMD_continue:
		ক্ষেত্র CMD_break:
		ক্ষেত্র CMD_while:
		ক্ষেত্র CMD_endwhile:
		ক্ষেত্র CMD_try:
		ক্ষেত্র CMD_catch:
		ক্ষেত্র CMD_finally:
		ক্ষেত্র CMD_endtry:
			semsg(_(e_cannot_use_legacy_with_command_str), ea.cmd);
			ফেরত FAIL;
		পূর্বনির্ধারিত: বিরতি;
	    }

	    // ":legacy return expr" needs to be handled differently.
	    যদি (checkforcmd(&start, "return", 4))
		ea.cmdidx = CMD_return;
	    অন্যথা
		ea.cmdidx = CMD_legacy;
	}

	যদি (p == ea.cmd && ea.cmdidx != CMD_আকার)
	{
	    // "eval" is used for "val->func()" and "var" for "var = val", then
	    // "p" is equal to "ea.cmd" for a valid command.
	    যদি (ea.cmdidx == CMD_eval || ea.cmdidx == CMD_var)
		;
	    অন্যথা যদি (cctx->ctx_skip == SKIP_YES)
	    {
		সারি += STRLEN(সারি);
		যাও nextline;
	    }
	    অন্যথা
	    {
		semsg(_(e_command_not_recognized_str), ea.cmd);
		ফেরত FAIL;
	    }
	}

	যদি ((cctx->ctx_had_return || cctx->ctx_had_throw)
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
	    ফেরত FAIL;
	}

	// When processing the end of an if-else or try-catch block, don't
	// clear the "ctx_had_throw" flag.  If such a block ends in a "throw"
	// statement, then it is considered to end in a "return" statement.
	// The "ctx_had_throw" is cleared immediately after processing the
	// block ending statement.
	// Otherwise, clear the "had_throw" flag.
	যদি (ea.cmdidx != CMD_else && ea.cmdidx != CMD_elseif
						&& ea.cmdidx != CMD_endif
						&& ea.cmdidx != CMD_catch
						&& ea.cmdidx != CMD_finally
						&& ea.cmdidx != CMD_endtry)
	    cctx->ctx_had_throw = FALSE;

	p = skipwhite(p);
	যদি (ea.cmdidx != CMD_আকার
			    && ea.cmdidx != CMD_write && ea.cmdidx != CMD_read)
	{
	    যদি (ea.cmdidx >= 0)
		ea.argt = excmd_get_argt(ea.cmdidx);
	    যদি ((ea.argt & EX_BANG) && *p == '!')
	    {
		ea.forceit = TRUE;
		p = skipwhite(p + 1);
	    }
	    যদি ((ea.argt & EX_RANGE) == 0 && ea.addr_গুনতি > 0)
	    {
		emsg(_(e_no_range_allowed));
		ফেরত FAIL;
	    }
	}

	নির্বাচন (ea.cmdidx)
	{
	    ক্ষেত্র CMD_def:
	    ক্ষেত্র CMD_function:
		    ea.arg = p;
		    সারি = compile_nested_function(&ea, cctx, সারিগুলি_to_free);
		    বিরতি;

	    ক্ষেত্র CMD_return:
		    সারি = compile_return(p, check_return_type,
				 local_cmdmod.cmod_flags & CMOD_LEGACY, cctx);
		    cctx->ctx_had_return = TRUE;
		    বিরতি;

	    ক্ষেত্র CMD_let:
		    emsg(_(e_cannot_use_let_in_vim9_script));
		    বিরতি;
	    ক্ষেত্র CMD_var:
	    ক্ষেত্র CMD_final:
	    ক্ষেত্র CMD_const:
	    ক্ষেত্র CMD_increment:
	    ক্ষেত্র CMD_decrement:
		    সারি = compile_assignment(p, &ea, ea.cmdidx, cctx);
		    যদি (সারি == p)
		    {
			emsg(_(e_invalid_assignment));
			সারি = ফাঁকা;
		    }
		    বিরতি;

	    ক্ষেত্র CMD_unlet:
	    ক্ষেত্র CMD_unlockvar:
	    ক্ষেত্র CMD_lockvar:
		    সারি = compile_unletlock(p, &ea, cctx);
		    বিরতি;

	    ক্ষেত্র CMD_import:
		    emsg(_(e_import_can_only_be_used_in_script));
		    সারি = ফাঁকা;
		    বিরতি;

	    ক্ষেত্র CMD_if:
		    সারি = compile_if(p, cctx);
		    বিরতি;
	    ক্ষেত্র CMD_elseif:
		    সারি = compile_elseif(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    cctx->ctx_had_throw = FALSE;
		    বিরতি;
	    ক্ষেত্র CMD_else:
		    সারি = compile_else(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    cctx->ctx_had_throw = FALSE;
		    বিরতি;
	    ক্ষেত্র CMD_endif:
		    সারি = compile_endif(p, cctx);
		    cctx->ctx_had_throw = FALSE;
		    বিরতি;

	    ক্ষেত্র CMD_while:
		    সারি = compile_while(p, cctx);
		    বিরতি;
	    ক্ষেত্র CMD_endwhile:
		    সারি = compile_endwhile(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    বিরতি;

	    ক্ষেত্র CMD_for:
		    সারি = compile_for(p, cctx);
		    বিরতি;
	    ক্ষেত্র CMD_endfor:
		    সারি = compile_endfor(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    বিরতি;
	    ক্ষেত্র CMD_continue:
		    সারি = compile_continue(p, cctx);
		    // "continue" unconditionally jumps back to the loop,
		    // like a "return".
		    cctx->ctx_had_return = TRUE;
		    বিরতি;
	    ক্ষেত্র CMD_break:
		    সারি = compile_break(p, cctx);
		    // "break" unconditionally exits the loop, like a "return".
		    cctx->ctx_had_return = TRUE;
		    বিরতি;

	    ক্ষেত্র CMD_try:
		    সারি = compile_try(p, cctx);
		    বিরতি;
	    ক্ষেত্র CMD_catch:
		    সারি = compile_catch(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    cctx->ctx_had_throw = FALSE;
		    বিরতি;
	    ক্ষেত্র CMD_finally:
		    সারি = compile_finally(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    cctx->ctx_had_throw = FALSE;
		    বিরতি;
	    ক্ষেত্র CMD_endtry:
		    সারি = compile_endtry(p, cctx);
		    cctx->ctx_had_throw = FALSE;
		    বিরতি;
	    ক্ষেত্র CMD_throw:
		    সারি = compile_throw(p, cctx);
		    cctx->ctx_had_throw = TRUE;
		    বিরতি;

	    ক্ষেত্র CMD_eval:
		    সারি = compile_eval(p, cctx);
		    বিরতি;

	    ক্ষেত্র CMD_defer:
		    সারি = compile_defer(p, cctx);
		    বিরতি;

#ifdef HAS_MESSAGE_WINDOW
	    ক্ষেত্র CMD_echowindow:
		    {
			long cmd_গুনতি = get_cmd_গুনতি(সারি, &ea);
			যদি (cmd_গুনতি < 0)
			    সারি = ফাঁকা;
			অন্যথা
			    সারি = compile_mult_রাশিমালা(p, ea.cmdidx,
							     cmd_গুনতি, cctx);
		    }
		    বিরতি;
#endif
	    ক্ষেত্র CMD_echo:
	    ক্ষেত্র CMD_echon:
	    ক্ষেত্র CMD_echoconsole:
	    ক্ষেত্র CMD_echoerr:
	    ক্ষেত্র CMD_echomsg:
	    ক্ষেত্র CMD_execute:
		    সারি = compile_mult_রাশিমালা(p, ea.cmdidx, 0, cctx);
		    বিরতি;

	    ক্ষেত্র CMD_put:
		    ea.cmd = cmd;
		    সারি = compile_put(p, &ea, cctx, FALSE);
		    বিরতি;

	    ক্ষেত্র CMD_iput:
		    ea.cmd = cmd;
		    সারি = compile_put(p, &ea, cctx, TRUE);
		    বিরতি;

	    ক্ষেত্র CMD_substitute:
		    যদি (check_global_and_subst(ea.cmd, p) == FAIL)
			ফেরত FAIL;
		    যদি (cctx->ctx_skip == SKIP_YES)
			সারি = (char_u *)"";
		    অন্যথা
		    {
			ea.arg = p;
			সারি = compile_substitute(সারি, &ea, cctx);
		    }
		    বিরতি;

	    ক্ষেত্র CMD_redir:
		    ea.arg = p;
		    সারি = compile_redir(সারি, &ea, cctx);
		    বিরতি;

	    ক্ষেত্র CMD_cexpr:
	    ক্ষেত্র CMD_lexpr:
	    ক্ষেত্র CMD_caddexpr:
	    ক্ষেত্র CMD_laddexpr:
	    ক্ষেত্র CMD_cgetexpr:
	    ক্ষেত্র CMD_lgetexpr:
#ifdef FEAT_QUICKFIX
		    ea.arg = p;
		    সারি = compile_cexpr(সারি, &ea, cctx);
#অন্যথা
		    ex_ni(&ea);
		    সারি = ফাঁকা;
#endif
		    বিরতি;

	    ক্ষেত্র CMD_append:
	    ক্ষেত্র CMD_change:
	    ক্ষেত্র CMD_ঢোকাও:
	    ক্ষেত্র CMD_k:
	    ক্ষেত্র CMD_open:
	    ক্ষেত্র CMD_t:
	    ক্ষেত্র CMD_xit:
		    not_in_vim9(&ea, false);
		    ফেরত FAIL;

	    ক্ষেত্র CMD_আকার:
		    যদি (cctx->ctx_skip != SKIP_YES)
		    {
			semsg(_(e_invalid_command_str), ea.cmd);
			ফেরত FAIL;
		    }
		    // We don't check for a next command here.
		    সারি = (char_u *)"";
		    বিরতি;

	    ক্ষেত্র CMD_lua:
	    ক্ষেত্র CMD_mzscheme:
	    ক্ষেত্র CMD_perl:
	    ক্ষেত্র CMD_py3:
	    ক্ষেত্র CMD_python3:
	    ক্ষেত্র CMD_python:
	    ক্ষেত্র CMD_pythonx:
	    ক্ষেত্র CMD_ruby:
	    ক্ষেত্র CMD_tcl:
		    ea.arg = p;
		    যদি (vim_strchr(সারি, '\n') == ফাঁকা)
			সারি = compile_exec(সারি, &ea, cctx);
		    অন্যথা
			// heredoc lines have been concatenated with NL
			// characters in get_function_body()
			সারি = compile_script(সারি, cctx);
		    বিরতি;

	    ক্ষেত্র CMD_vim9script:
		    যদি (cctx->ctx_skip != SKIP_YES)
		    {
			emsg(_(e_vim9script_can_only_be_used_in_script));
			ফেরত FAIL;
		    }
		    সারি = (char_u *)"";
		    বিরতি;

	    ক্ষেত্র CMD_class:
		    emsg(_(e_class_can_only_be_used_in_script));
		    ফেরত FAIL;

	    ক্ষেত্র CMD_enum:
		    emsg(_(e_enum_can_only_be_used_in_script));
		    ফেরত FAIL;

	    ক্ষেত্র CMD_interface:
		    emsg(_(e_interface_can_only_be_used_in_script));
		    ফেরত FAIL;

	    ক্ষেত্র CMD_type:
		    emsg(_(e_type_can_only_be_used_in_script));
		    ফেরত FAIL;

	    ক্ষেত্র CMD_global:
		    যদি (check_global_and_subst(ea.cmd, p) == FAIL)
			ফেরত FAIL;
		    // FALLTHROUGH
	    পূর্বনির্ধারিত:
		    // Not recognized, execute with do_cmdline_cmd().
		    ea.arg = p;
		    সারি = compile_exec(সারি, &ea, cctx);
		    বিরতি;
	}
nextline:
	যদি (সারি == ফাঁকা)
	    ফেরত FAIL;
	সারি = skipwhite(সারি);

	// Undo any command modifiers.
	generate_undo_cmdmods(cctx);

	যদি (cctx->ctx_type_স্তূপ.ga_len < 0)
	{
	    iemsg("Type stack underflow");
	    ফেরত FAIL;
	}
    } // END of the loop over all the function body lines.

    ফেরত OK;
}

/*
 * Returns TRUE if the end of a scope (if, while, for, block) is missing.
 * Called after compiling a def function body.
 */
    স্থির পূর্ণসংখ্যা
compile_dfunc_scope_end_missing(cctx_T *cctx)
{
    যদি (cctx->ctx_scope == ফাঁকা)
	ফেরত FALSE;

    নির্বাচন (cctx->ctx_scope->se_type)
    {
	ক্ষেত্র IF_SCOPE:
	    emsg(_(e_missing_endif));
	    বিরতি;
	ক্ষেত্র WHILE_SCOPE:
	    emsg(_(e_missing_endwhile));
	    বিরতি;
	ক্ষেত্র FOR_SCOPE:
	    emsg(_(e_missing_endfor));
	    বিরতি;
	ক্ষেত্র TRY_SCOPE:
	    emsg(_(e_missing_endtry));
	    বিরতি;
	ক্ষেত্র BLOCK_SCOPE:
	    // end block scope from :try (maybe)
	    compile_endblock(cctx);
	    যদি (cctx->ctx_scope != ফাঁকা
		    && cctx->ctx_scope->se_type == TRY_SCOPE)
		emsg(_(e_missing_endtry));
	    অন্যথা
		emsg(_(e_missing_rcurly));
	    বিরতি;
	পূর্বনির্ধারিত:
	    emsg(_(e_missing_rcurly));
    }
    ফেরত TRUE;
}

/*
 * When compiling a def function, if it doesn't have an explicit return
 * statement, then generate a default return instruction.  For an object
 * constructor, return the object.
 */
    স্থির পূর্ণসংখ্যা
compile_dfunc_generate_default_return(ufunc_T *ufunc, cctx_T *cctx)
{
    // TODO: if a function ends in "throw" but there was a return elsewhere we
    // should not assume the return type is "void".
    যদি (cctx->ctx_had_return || cctx->ctx_had_throw)
	ফেরত OK;

    যদি (ufunc->uf_ret_type->tt_type == VAR_UNKNOWN)
	ufunc->uf_ret_type = &t_void;
    অন্যথা যদি (ufunc->uf_ret_type->tt_type != VAR_VOID
					&& !IS_CONSTRUCTOR_METHOD(ufunc))
    {
	emsg(_(e_missing_return_statement));
	ফেরত FAIL;
    }

    // Return void if there is no return at the end.
    // For a constructor return the object.
    যদি (IS_CONSTRUCTOR_METHOD(ufunc))
    {
	generate_instr(cctx, ISN_RETURN_OBJECT);
	ufunc->uf_ret_type = &ufunc->uf_class->class_object_type;
    }
    অন্যথা
	generate_instr(cctx, ISN_RETURN_VOID);

    ফেরত OK;
}

/*
 * Perform the chores after successfully compiling a def function.
 */
    স্থির শূন্য
compile_dfunc_epilogue(
    cctx_T	*outer_cctx,
    ufunc_T	*ufunc,
    garray_T	*instr,
    cctx_T	*cctx)
{
    dfunc_T	*dfunc;

    dfunc = ((dfunc_T *)def_functions.ga_তথ্য) + ufunc->uf_dfunc_idx;
    dfunc->df_deleted = FALSE;
    dfunc->df_script_seq = current_sctx.sc_seq;

#ifdef FEAT_PROFILE
    যদি (cctx->ctx_compile_type == CT_PROFILE)
    {
	dfunc->df_instr_prof = instr->ga_তথ্য;
	dfunc->df_instr_prof_গুনতি = instr->ga_len;
    }
    অন্যথা
#endif
	যদি (cctx->ctx_compile_type == CT_DEBUG)
	{
	    dfunc->df_instr_debug = instr->ga_তথ্য;
	    dfunc->df_instr_debug_গুনতি = instr->ga_len;
	}
	অন্যথা
	{
	    dfunc->df_instr = instr->ga_তথ্য;
	    dfunc->df_instr_গুনতি = instr->ga_len;
	}
    dfunc->df_varcount = dfunc->df_var_names.ga_len;
    dfunc->df_has_closure = cctx->ctx_has_closure;

    যদি (cctx->ctx_outer_used)
    {
	ufunc->uf_flags |= FC_CLOSURE;
	যদি (outer_cctx != ফাঁকা)
	    ++outer_cctx->ctx_closure_গুনতি;
    }

    ufunc->uf_def_status = UF_COMPILED;
}

/*
 * Perform the cleanup when a def function compilation fails.
 */
    স্থির শূন্য
compile_dfunc_ufunc_cleanup(
    ufunc_T	*ufunc,
    garray_T	*instr,
    পূর্ণসংখ্যা		new_def_function,
    অক্ষর	*errormsg,
    পূর্ণসংখ্যা		did_emsg_before,
    cctx_T	*cctx)
{
    dfunc_T	*dfunc;

    dfunc = ((dfunc_T *)def_functions.ga_তথ্য) + ufunc->uf_dfunc_idx;

    // Compiling aborted, free the generated instructions.
    clear_instr_ga(instr);
    VIM_CLEAR(dfunc->df_name);
    ga_clear_strings(&dfunc->df_var_names);

    // If using the last entry in the table and it was added above, we
    // might as well remove it.
    যদি (!dfunc->df_deleted && new_def_function
			&& ufunc->uf_dfunc_idx == def_functions.ga_len - 1)
    {
	--def_functions.ga_len;
	ufunc->uf_dfunc_idx = 0;
    }
    ufunc->uf_def_status = UF_COMPILE_ERROR;

    যখন (cctx->ctx_scope != ফাঁকা)
	drop_scope(cctx);

    যদি (errormsg != ফাঁকা)
	emsg(errormsg);
    অন্যথা যদি (did_emsg == did_emsg_before)
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
    পূর্ণসংখ্যা
compile_def_function(
	ufunc_T		*ufunc,
	পূর্ণসংখ্যা		check_return_type,
	compiletype_T   compile_type,
	cctx_T		*outer_cctx)
{
    garray_T	সারিগুলি_to_free;
    অক্ষর	*errormsg = ফাঁকা;	// error message
    cctx_T	cctx;
    garray_T	*instr;
    পূর্ণসংখ্যা		did_emsg_before = did_emsg;
    পূর্ণসংখ্যা		did_emsg_silent_before = did_emsg_silent;
    পূর্ণসংখ্যা		ret = FAIL;
    sctx_T	save_current_sctx = current_sctx;
    পূর্ণসংখ্যা		save_estack_compiling = estack_compiling;
    পূর্ণসংখ্যা		save_cmod_flags = cmdmod.cmod_flags;
    পূর্ণসংখ্যা		do_estack_ঠেলো;
    পূর্ণসংখ্যা		new_def_function = FALSE;

    // allocated lines are freed at the end
    ga_init2(&সারিগুলি_to_free, আকার(char_u *), 50);

    // Initialize the ufunc and the compilation context
    যদি (compile_dfunc_ufunc_init(ufunc, outer_cctx, compile_type,
						&new_def_function) == FAIL)
	ফেরত FAIL;

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
    do_estack_ঠেলো = !estack_শীর্ষ_is_ufunc(ufunc, 1);
    যদি (do_estack_ঠেলো)
	estack_ঠেলো_ufunc(ufunc, 1);
    estack_compiling = TRUE;

    // Make sure arguments don't shadow variables in the context
    যদি (check_args_shadowing(ufunc, &cctx) == FAIL)
	যাও erret;

    // For an object method and a constructor generate instructions to
    // initialize "this" and the object variables.
    যদি (ufunc->uf_flags & (FC_OBJECT|FC_NEW))
	যদি (obj_method_prologue(ufunc, &cctx) == FAIL)
	    যাও erret;

    যদি (ufunc->uf_def_args.ga_len > 0)
	যদি (compile_def_function_default_args(ufunc, instr, &cctx) == FAIL)
	    যাও erret;
    ufunc->uf_args_visible = ufunc->uf_args.ga_len;

    // Compiling an abstract method or a function in an interface is done to
    // get the function type.  No code is actually compiled.
    যদি (ufunc->uf_class != ফাঁকা && (IS_INTERFACE(ufunc->uf_class)
						|| IS_ABSTRACT_METHOD(ufunc)))
    {
	ufunc->uf_def_status = UF_NOT_COMPILED;
	ret = OK;
	যাও erret;
    }

    // compile the function body
    যদি (compile_def_function_body(ufunc->uf_সারিগুলি.ga_len, check_return_type,
				&সারিগুলি_to_free, &errormsg, &cctx) == FAIL)
	যাও erret;

    যদি (compile_dfunc_scope_end_missing(&cctx))
	যাও erret;

    যদি (compile_dfunc_generate_default_return(ufunc, &cctx) == FAIL)
	যাও erret;

    // When compiled with ":silent!" and there was an error don't consider the
    // function compiled.
    যদি (emsg_silent == 0 || did_emsg_silent == did_emsg_silent_before)
	compile_dfunc_epilogue(outer_cctx, ufunc, instr, &cctx);

    ret = OK;

erret:
    যদি (ufunc->uf_def_status == UF_COMPILING)
    {
	// compilation failed. do cleanup.
	compile_dfunc_ufunc_cleanup(ufunc, instr, new_def_function,
				    errormsg, did_emsg_before, &cctx);
    }

    যদি (cctx.ctx_redir_lhs.lhs_name != ফাঁকা)
    {
	যদি (ret == OK)
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
    যদি (do_estack_ঠেলো)
	estack_বের_করো();

    ga_clear_strings(&সারিগুলি_to_free);
    free_locals(&cctx);
    ga_clear(&cctx.ctx_type_স্তূপ);
    ফেরত ret;
}

    শূন্য
set_function_type(ufunc_T *ufunc)
{
    পূর্ণসংখ্যা varargs = ufunc->uf_va_name != ফাঁকা;
    পূর্ণসংখ্যা argcount = ufunc->uf_args.ga_len;

    // Create a type for the function, with the return type and any
    // argument types.
    // A vararg is included in uf_args.ga_len but not in uf_arg_types.
    // The type is included in "tt_args".
    যদি (argcount > 0 || varargs)
    {
	যদি (ufunc->uf_type_list.ga_itemsize == 0)
	    ga_init2(&ufunc->uf_type_list, আকার(type_T *), 10);
	ufunc->uf_func_type = alloc_func_type(ufunc->uf_ret_type,
					   argcount, &ufunc->uf_type_list);
	// Add argument types to the function type.
	যদি (func_type_add_arg_types(ufunc->uf_func_type,
				    argcount + varargs,
				    &ufunc->uf_type_list) == FAIL)
	    ফেরত;
	ufunc->uf_func_type->tt_argcount = argcount + varargs;
	ufunc->uf_func_type->tt_min_argcount =
				      argcount - ufunc->uf_def_args.ga_len;
	যদি (ufunc->uf_arg_types == ফাঁকা)
	{
	    পূর্ণসংখ্যা i;

	    // lambda does not have argument types.
	    জন্য (i = 0; i < argcount; ++i)
		ufunc->uf_func_type->tt_args[i] = &t_any;
	}
	অন্যথা
	    mch_memmove(ufunc->uf_func_type->tt_args,
			 ufunc->uf_arg_types, আকার(type_T *) * argcount);
	যদি (varargs)
	{
	    ufunc->uf_func_type->tt_args[argcount] =
		   ufunc->uf_va_type == ফাঁকা ? &t_list_any : ufunc->uf_va_type;
	    ufunc->uf_func_type->tt_flags = TTFLAG_VARARGS;
	}
    }
    অন্যথা
	// No arguments, can use a predefined type.
	ufunc->uf_func_type = get_func_type(ufunc->uf_ret_type,
					   argcount, &ufunc->uf_type_list);
}

/*
 * Free all instructions for "dfunc" except df_name.
 */
    স্থির শূন্য
delete_def_function_contents(dfunc_T *dfunc, পূর্ণসংখ্যা mark_deleted)
{
    পূর্ণসংখ্যা idx;

    // In same cases the instructions may refer to a class in which the
    // function is defined and unreferencing the class may call back here
    // recursively.  Set the df_delete_busy to avoid problems.
    যদি (dfunc->df_delete_busy)
	ফেরত;
    dfunc->df_delete_busy = TRUE;

    ga_clear(&dfunc->df_def_args_isn);
    ga_clear_strings(&dfunc->df_var_names);

    যদি (dfunc->df_instr != ফাঁকা)
    {
	জন্য (idx = 0; idx < dfunc->df_instr_গুনতি; ++idx)
	    delete_instr(dfunc->df_instr + idx);
	VIM_CLEAR(dfunc->df_instr);
    }
    যদি (dfunc->df_instr_debug != ফাঁকা)
    {
	জন্য (idx = 0; idx < dfunc->df_instr_debug_গুনতি; ++idx)
	    delete_instr(dfunc->df_instr_debug + idx);
	VIM_CLEAR(dfunc->df_instr_debug);
    }
#ifdef FEAT_PROFILE
    যদি (dfunc->df_instr_prof != ফাঁকা)
    {
	জন্য (idx = 0; idx < dfunc->df_instr_prof_গুনতি; ++idx)
	    delete_instr(dfunc->df_instr_prof + idx);
	VIM_CLEAR(dfunc->df_instr_prof);
    }
#endif

    যদি (mark_deleted)
	dfunc->df_deleted = TRUE;
    যদি (dfunc->df_ufunc != ফাঁকা)
	dfunc->df_ufunc->uf_def_status = UF_NOT_COMPILED;

    dfunc->df_delete_busy = FALSE;
}

/*
 * When a user function is deleted, clear the contents of any associated def
 * function, unless another user function still uses it.
 * The position in def_functions can be re-used.
 */
    শূন্য
unlink_def_function(ufunc_T *ufunc)
{
    যদি (ufunc->uf_dfunc_idx <= 0 || def_functions.ga_তথ্য == ফাঁকা)
	ফেরত;

    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_তথ্য)
	+ ufunc->uf_dfunc_idx;

    যদি (--dfunc->df_refcount <= 0)
	delete_def_function_contents(dfunc, TRUE);
    ufunc->uf_def_status = UF_NOT_COMPILED;
    ufunc->uf_dfunc_idx = 0;
    যদি (dfunc->df_ufunc == ufunc)
	dfunc->df_ufunc = ফাঁকা;
}

/*
 * Used when a user function refers to an existing dfunc.
 */
    শূন্য
link_def_function(ufunc_T *ufunc)
{
    যদি (ufunc->uf_dfunc_idx <= 0)
	ফেরত;

    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_তথ্য)
	+ ufunc->uf_dfunc_idx;

    ++dfunc->df_refcount;
}

#যদি defined(EXITFREE)
/*
 * Free all functions defined with ":def".
 */
    শূন্য
free_def_functions(শূন্য)
{
    পূর্ণসংখ্যা idx;

    জন্য (idx = 0; idx < def_functions.ga_len; ++idx)
    {
	dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_তথ্য) + idx;

	delete_def_function_contents(dfunc, TRUE);
	vim_free(dfunc->df_name);
    }

    ga_clear(&def_functions);
}
#endif


#endif // FEAT_EVAL
