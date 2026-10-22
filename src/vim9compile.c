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
#अन्तर्भाव "vim.h"

#जइ defined(FEAT_EVAL)

// Functions defined with :def are stored in this growarray.
// They are never removed, so that they can be found by index.
// Deleted functions have the df_deleted flag set.
garray_T def_functions = {0, 0, परिमाण(dfunc_T), 50, रिक्त};

स्थिर शून्य delete_def_function_contents(dfunc_T *dfunc, पूर्णाङ्क mark_deleted);

/*
 * Lookup variable "name" in the local scope and return it in "lvar".
 * "lvar->lv_from_outer" is incremented accordingly.
 * If "lvar" is NULL only check if the variable can be found.
 * Return FAIL if not found.
 */
    पूर्णाङ्क
lookup_local(char_u *name, size_t len, lvar_T *lvar, cctx_T *cctx)
{
    पूर्णाङ्क	    idx;
    lvar_T  *lvp;

    जइ (len == 0)
	प्रत्यागम FAIL;

    जइ (((len == 4 && STRNCMP(name, "this", 4) == 0)
		|| (len == 5 && STRNCMP(name, "super", 5) == 0))
	    && cctx->ctx_ufunc != रिक्त
	    && (cctx->ctx_ufunc->uf_flags & (FC_OBJECT|FC_NEW)))
    {
	पूर्णाङ्क is_super = *name == 's';
	जइ (is_super)
	{
	    जइ (name[5] != '.')
	    {
		emsg(_(e_super_must_be_followed_by_dot));
		प्रत्यागम FAIL;
	    }
	    जइ (cctx->ctx_ufunc->uf_class != रिक्त
		    && cctx->ctx_ufunc->uf_class->class_extends == रिक्त)
	    {
		emsg(_(e_using_super_not_in_child_class));
		प्रत्यागम FAIL;
	    }
	}
	जइ (lvar != रिक्त)
	{
	    CLEAR_POINTER(lvar);
	    lvar->lv_loop_depth = -1;
	    lvar->lv_name = (char_u *)(is_super ? "super" : "this");
	    जइ (cctx->ctx_ufunc->uf_class != रिक्त)
	    {
		lvar->lv_type = &cctx->ctx_ufunc->uf_class->class_object_type;
		जइ (is_super)
		{
		    type_T *type = get_type_ptr(cctx->ctx_type_list);

		    जइ (type != रिक्त)
		    {
			*type = *lvar->lv_type;
			lvar->lv_type = type;
			type->tt_flags |= TTFLAG_SUPER;
		    }
		}
	    }
	}
	प्रत्यागम OK;
    }

    // Find local in current function scope.
    प्रति (idx = 0; idx < cctx->ctx_locals.ga_len; ++idx)
    {
	lvp = ((lvar_T *)cctx->ctx_locals.ga_data) + idx;
	जइ (lvp->lv_name != रिक्त
		&& STRNCMP(name, lvp->lv_name, len) == 0
					       && STRLEN(lvp->lv_name) == len)
	{
	    जइ (lvar != रिक्त)
	    {
		*lvar = *lvp;
		lvar->lv_from_outer = 0;
		// If the variable was declared inside a loop set
		// lvar->lv_loop_idx and lvar->lv_loop_depth.
		get_loop_var_idx(cctx, idx, lvar);
	    }
	    प्रत्यागम OK;
	}
    }

    // Find local in outer function scope.
    जइ (cctx->ctx_outer != रिक्त)
    {
	जइ (lookup_local(name, len, lvar, cctx->ctx_outer) == OK)
	{
	    जइ (lvar != रिक्त)
	    {
		cctx->ctx_outer_used = TRUE;
		++lvar->lv_from_outer;
	    }
	    प्रत्यागम OK;
	}
    }

    प्रत्यागम FAIL;
}

/*
 * Lookup an argument in the current function and an enclosing function.
 * Returns the argument index in "idxp"
 * Returns the argument type in "type"
 * Sets "gen_load_outer" to TRUE if found in outer scope.
 * Returns OK when found, FAIL otherwise.
 */
    पूर्णाङ्क
arg_exists(
	char_u	*name,
	size_t	len,
	पूर्णाङ्क	*idxp,
	type_T	**type,
	पूर्णाङ्क	*gen_load_outer,
	cctx_T	*cctx)
{
    पूर्णाङ्क	    idx;
    char_u  *va_name;

    जइ (len == 0)
	प्रत्यागम FAIL;
    प्रति (idx = 0; idx < cctx->ctx_ufunc->uf_args_visible; ++idx)
    {
	char_u *arg = FUNCARG(cctx->ctx_ufunc, idx);

	जइ (STRNCMP(name, arg, len) == 0 && arg[len] == NUL)
	{
	    जइ (idxp != रिक्त)
	    {
		// Arguments are located above the frame pointer.  One further
		// if there is a vararg argument
		*idxp = idx - (cctx->ctx_ufunc->uf_args.ga_len
							    + राशिः_FRAME_SIZE)
			      + (cctx->ctx_ufunc->uf_va_name != रिक्त ? -1 : 0);

		जइ (cctx->ctx_ufunc->uf_arg_types != रिक्त)
		    *type = cctx->ctx_ufunc->uf_arg_types[idx];
		अण्णहा
		    *type = &t_any;
	    }
	    प्रत्यागम OK;
	}
    }

    va_name = cctx->ctx_ufunc->uf_va_name;
    जइ (va_name != रिक्त
		    && STRNCMP(name, va_name, len) == 0 && va_name[len] == NUL)
    {
	जइ (idxp != रिक्त)
	{
	    // varargs is always the last argument
	    *idxp = -राशिः_FRAME_SIZE - 1;
	    *type = cctx->ctx_ufunc->uf_va_type;
	}
	प्रत्यागम OK;
    }

    जइ (cctx->ctx_outer != रिक्त)
    {
	// Lookup the name for an argument of the outer function.
	जइ (arg_exists(name, len, idxp, type, gen_load_outer, cctx->ctx_outer)
									 == OK)
	{
	    जइ (gen_load_outer != रिक्त)
	    {
		++*gen_load_outer;
		cctx->ctx_outer_used = TRUE;
	    }
	    प्रत्यागम OK;
	}
    }

    प्रत्यागम FAIL;
}

/*
 * Lookup a script-local variable in the current script, possibly defined in a
 * block that contains the function "cctx->ctx_ufunc".
 * "cctx" is NULL at the script level, "cstack" is NULL in a function.
 * If "len" is <= 0 "name" must be NUL terminated.
 * Return NULL when not found.
 */
    स्थिर sallvar_T *
find_script_var(char_u *name, size_t len, cctx_T *cctx, cstack_T *cstack)
{
    scriptitem_T    *si = SCRIPT_ITEM(current_sctx.sc_sid);
    hashitem_T	    *hi;
    पूर्णाङ्क		    cc;
    sallvar_T	    *sav;
    ufunc_T	    *ufunc;

    // Find the list of all script variables with the right name.
    जइ (len > 0)
    {
	cc = name[len];
	name[len] = NUL;
    }
    hi = hash_find(&si->sn_all_vars.dv_hashtab, name);
    जइ (len > 0)
	name[len] = cc;
    जइ (HASHITEM_EMPTY(hi))
	प्रत्यागम रिक्त;

    sav = HI2SAV(hi);
    जइ (sav->sav_block_id == 0)
	// variable defined in the top script scope is always visible
	प्रत्यागम sav;

    जइ (cctx == रिक्त)
    {
	जइ (cstack == रिक्त)
	    प्रत्यागम रिक्त;

	// Not in a function scope, find variable with block ID equal to or
	// smaller than the current block id.  Use "cstack" to go up the block
	// scopes.
	जाव (sav != रिक्त)
	{
	    पूर्णाङ्क idx;

	    प्रति (idx = cstack->cs_idx; idx >= 0; --idx)
		जइ (cstack->cs_block_id[idx] == sav->sav_block_id)
		    भङ्ग;
	    जइ (idx >= 0)
		भङ्ग;
	    sav = sav->sav_next;
	}
	प्रत्यागम sav;
    }

    // Go over the variables with this name and find one that was visible
    // from the function.
    ufunc = cctx->ctx_ufunc;
    जाव (sav != रिक्त)
    {
	पूर्णाङ्क idx;

	जइ (ufunc->uf_block_depth == 0 && sav->sav_block_id == 0)
	    // If the function was defined at the script level (not inside a
	    // block), script-scope variables are always visible.
	    प्रत्यागम sav;

	// Go over the blocks that this function was defined in.  If the
	// variable block ID matches it was visible to the function.
	प्रति (idx = 0; idx < ufunc->uf_block_depth; ++idx)
	    जइ (ufunc->uf_block_ids[idx] == sav->sav_block_id)
		प्रत्यागम sav;
	sav = sav->sav_next;
    }

    // Not found, variable was not visible.
    प्रत्यागम रिक्त;
}

/*
 * If "name" can be found in the current script set it's "block_id".
 */
    शून्य
update_script_var_block_id(char_u *name, पूर्णाङ्क block_id)
{
    scriptitem_T    *si = SCRIPT_ITEM(current_sctx.sc_sid);
    hashitem_T	    *hi;
    sallvar_T	    *sav;

    hi = hash_find(&si->sn_all_vars.dv_hashtab, name);
    जइ (HASHITEM_EMPTY(hi))
	प्रत्यागम;
    sav = HI2SAV(hi);
    sav->sav_block_id = block_id;
}

/*
 * Lookup a variable (without s: prefix) in the current script.
 * "cctx" is NULL at the script level, "cstack" is NULL in a function.
 * Returns OK or FAIL.
 */
    पूर्णाङ्क
script_var_exists(char_u *name, size_t len, cctx_T *cctx, cstack_T *cstack)
{
    जइ (current_sctx.sc_sid <= 0)
	प्रत्यागम FAIL;
    जइ (current_script_is_vim9())
    {
	// Check script variables that were visible where the function was
	// defined.
	जइ (find_script_var(name, len, cctx, cstack) != रिक्त)
	    प्रत्यागम OK;
    }
    अण्णहा
    {
	hashtab_T	*ht = &SCRIPT_VARS(current_sctx.sc_sid);
	dictitem_T	*di;
	पूर्णाङ्क		cc;

	// Check script variables that are currently visible
	cc = name[len];
	name[len] = NUL;
	di = find_var_in_ht(ht, 0, name, TRUE);
	name[len] = cc;
	जइ (di != रिक्त)
	    प्रत्यागम OK;
    }

    प्रत्यागम FAIL;
}

/*
 * Returns the index of a class method or class variable with name "name"
 * accessible in the currently compiled function.
 * If "cl_ret" is not NULL set it to the class.
 * Otherwise return -1.
 */
    स्थिर पूर्णाङ्क
cctx_class_midx(
    cctx_T  *cctx,
    पूर्णाङ्क	    is_method,
    char_u  *name,
    size_t  len,
    class_T **cl_ret)
{
    जइ (cctx == रिक्त || cctx->ctx_ufunc == रिक्त
	    || cctx->ctx_ufunc->uf_class == रिक्त
	    || cctx->ctx_ufunc->uf_defclass == रिक्त)
	प्रत्यागम -1;

    // Search for the class method or variable in the class where the calling
    // function is defined.
    class_T *cl = cctx->ctx_ufunc->uf_defclass;
    पूर्णाङ्क m_idx = is_method ? class_method_idx(cl, name, len)
					: class_member_idx(cl, name, len);
    जइ (m_idx < 0)
    {
	cl = cl->class_extends;
	जाव (cl != रिक्त)
	{
	    m_idx = is_method ? class_method_idx(cl, name, len)
					: class_member_idx(cl, name, len);
	    जइ (m_idx >= 0)
		भङ्ग;
	    cl = cl->class_extends;
	}
    }

    जइ (m_idx >= 0)
    {
	जइ (cl_ret != रिक्त)
	    *cl_ret = cl;
    }

    प्रत्यागम m_idx;
}

/*
 * Returns the index of a class method with name "name" accessible in the
 * currently compiled function.  Returns -1 if not found.  The class where the
 * method is defined is returned in "cl_ret".
 */
    पूर्णाङ्क
cctx_class_method_idx(
    cctx_T  *cctx,
    char_u  *name,
    size_t  len,
    class_T **cl_ret)
{
    प्रत्यागम cctx_class_midx(cctx, TRUE, name, len, cl_ret);
}

/*
 * Returns the index of a class variable with name "name" accessible in the
 * currently compiled function.  Returns -1 if not found.  The class where the
 * variable is defined is returned in "cl_ret".
 */
    पूर्णाङ्क
cctx_class_member_idx(
    cctx_T  *cctx,
    char_u  *name,
    size_t  len,
    class_T **cl_ret)
{
    प्रत्यागम cctx_class_midx(cctx, FALSE, name, len, cl_ret);
}

/*
 * Return TRUE if "name" is a local variable, argument, script variable or
 * imported.  Also if "name" is "this" and in a class method.
 */
    स्थिर पूर्णाङ्क
variable_exists(char_u *name, size_t len, cctx_T *cctx)
{
    प्रत्यागम (cctx != रिक्त
		&& (lookup_local(name, len, रिक्त, cctx) == OK
		    || arg_exists(name, len, रिक्त, रिक्त, रिक्त, cctx) == OK
		    || (len == 4
			&& cctx->ctx_ufunc != रिक्त
			&& (cctx->ctx_ufunc->uf_flags & (FC_OBJECT|FC_NEW))
			&& STRNCMP(name, "this", 4) == 0)))
	    || script_var_exists(name, len, cctx, रिक्त) == OK
	    || cctx_class_member_idx(cctx, name, len, रिक्त) >= 0
	    || find_imported(name, len, FALSE) != रिक्त;
}

/*
 * Return TRUE if "name" is a local variable, argument, script variable,
 * imported or function.  Or commands are being skipped, a declaration may have
 * been skipped then.
 */
    स्थिर पूर्णाङ्क
item_exists(char_u *name, size_t len, पूर्णाङ्क cmd UNUSED, cctx_T *cctx)
{
    प्रत्यागम variable_exists(name, len, cctx);
}

/*
 * Check if "p[len]" is already defined, either in script "import_sid" or in
 * compilation context "cctx".
 * "cctx" is NULL at the script level, "cstack" is NULL in a function.
 * Does not check the global namespace.
 * If "is_arg" is TRUE the error message is for an argument name.
 * Return FAIL and give an error if it defined.
 */
    पूर्णाङ्क
check_defined(
	char_u	    *p,
	size_t	    len,
	cctx_T	    *cctx,
	cstack_T    *cstack,
	पूर्णाङ्क	    is_arg)
{
    पूर्णाङ्क		c = p[len];
    ufunc_T	*ufunc = रिक्त;

    // underscore argument is OK
    जइ (len == 1 && *p == '_')
	प्रत्यागम OK;

    जइ (script_var_exists(p, len, cctx, cstack) == OK)
    {
	जइ (is_arg)
	    semsg(_(e_argument_already_declared_in_script_str), p);
	अण्णहा
	    semsg(_(e_variable_already_declared_in_script_str), p);
	प्रत्यागम FAIL;
    }

    जइ (cctx_class_member_idx(cctx, p, len, रिक्त) >= 0)
    {
	जइ (is_arg)
	    semsg(_(e_argument_already_declared_in_class_str), p);
	अण्णहा
	    semsg(_(e_variable_already_declared_in_class_str), p);
	प्रत्यागम FAIL;
    }

    p[len] = NUL;
    जइ ((cctx != रिक्त
		&& (lookup_local(p, len, रिक्त, cctx) == OK
		    || arg_exists(p, len, रिक्त, रिक्त, रिक्त, cctx) == OK))
	    || find_imported(p, len, FALSE) != रिक्त
	    || (ufunc = find_func_even_dead(p, 0)) != रिक्त)
    {
	// A local or script-local function can shadow a global function.
	जइ (ufunc == रिक्त || ((ufunc->uf_flags & FC_DEAD) == 0
		    && (!func_is_global(ufunc)
					     || (p[0] == 'g' && p[1] == ':'))))
	{
	    जइ (is_arg)
		semsg(_(e_argument_name_shadows_existing_variable_str), p);
	    अण्णहा
		semsg(_(e_name_already_defined_str), p);
	    p[len] = c;
	    प्रत्यागम FAIL;
	}
    }
    p[len] = c;
    प्रत्यागम OK;
}


/*
 * Return TRUE if "actual" could be "expected" and a runtime typecheck is to be
 * used.  Return FALSE if the types will never match.
 */
    स्थिर पूर्णाङ्क
use_typecheck(type_T *actual, type_T *expected)
{
    जइ (actual->tt_type == VAR_ANY
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
	प्रत्यागम TRUE;
    जइ (actual->tt_type == VAR_OBJECT && expected->tt_type == VAR_OBJECT)
	प्रत्यागम TRUE;
    जइ ((actual->tt_type == VAR_LIST
		|| actual->tt_type == VAR_TUPLE
		|| actual->tt_type == VAR_DICT)
	    && actual->tt_type == expected->tt_type)
	// This takes care of a nested list or dict.
	प्रत्यागम use_typecheck(actual->tt_member, expected->tt_member);
    प्रत्यागम FALSE;
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
    पूर्णाङ्क
need_type_where(
	type_T	*actual,
	type_T	*expected,
	पूर्णाङ्क	typechk_flags,	// acceptable types (type check flags)
	पूर्णाङ्क	offset,
	where_T	where,
	cctx_T	*cctx,
	पूर्णाङ्क	silent,
	पूर्णाङ्क	actual_is_const)
{
    पूर्णाङ्क ret;

    जइ (expected->tt_type != VAR_CLASS && expected->tt_type != VAR_TYPEALIAS)
    {
	जइ (check_type_is_मानम्(actual) == FAIL)
	    प्रत्यागम FAIL;
    }

    जइ (expected == &t_bool && actual != &t_bool
					&& (actual->tt_flags & TTFLAG_BOOL_OK))
    {
	// Using "0", "1" or the result of an expression with "&&" or "||" as a
	// boolean is OK but requires a conversion.
	generate_2BOOL(cctx, FALSE, offset);
	प्रत्यागम OK;
    }

    ret = check_type_maybe(expected, actual, FALSE, where);
    जइ (ret == OK)
	प्रत्यागम OK;

    // If actual a constant a runtime check makes no sense.  If it's
    // null_function it is OK.
    जइ (actual_is_const && ret == MAYBE && actual == &t_func_unknown)
	प्रत्यागम OK;

    // If the actual type can be the expected type add a runtime check.
    जइ (!actual_is_const && ret == MAYBE && use_typecheck(actual, expected))
    {
	generate_TYPECHECK(cctx, expected, typechk_flags, offset,
		where.wt_kind == WT_VARIABLE, where.wt_index);
	प्रत्यागम OK;
    }

    जइ (!silent)
	type_mismatch_where(expected, actual, where);
    प्रत्यागम FAIL;
}

    पूर्णाङ्क
need_type(
	type_T	*actual,
	type_T	*expected,
	पूर्णाङ्क	typechk_flags,	// acceptable types (type check flags)
	पूर्णाङ्क	offset,
	पूर्णाङ्क	arg_idx,
	cctx_T	*cctx,
	पूर्णाङ्क	silent,
	पूर्णाङ्क	actual_is_const)
{
    where_T where = WHERE_INIT;

    जइ (arg_idx > 0)
    {
	where.wt_index = arg_idx;
	where.wt_kind = WT_ARGUMENT;
    }
    प्रत्यागम need_type_where(actual, expected, typechk_flags, offset, where,
						cctx, silent, actual_is_const);
}

/*
 * Set type of variable "lvar" to "type".  If the variable is a constant then
 * the type gets TTFLAG_CONST.
 */
    स्थिर शून्य
set_var_type(lvar_T *lvar, type_T *type_arg, cctx_T *cctx)
{
    type_T	*type = type_arg;

    जइ (lvar->lv_const == ASSIGN_CONST && (type->tt_flags & TTFLAG_CONST) == 0)
    {
	जइ (type->tt_flags & TTFLAG_STATIC)
	    // entry in static_types[] is followed by const type
	    type = type + 1;
	अण्णहा
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
	पूर्णाङ्क	assign,
	type_T	*type)
{
    lvar_T  *lvar;
    dfunc_T *dfunc;

    जइ (arg_exists(name, len, रिक्त, रिक्त, रिक्त, cctx) == OK)
    {
	emsg_namelen(_(e_str_is_used_as_argument), name, (पूर्णाङ्क)len);
	प्रत्यागम रिक्त;
    }

    जइ (GA_GROW_FAILS(&cctx->ctx_locals, 1))
	प्रत्यागम रिक्त;
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
    जइ (type == &t_unknown || type == &t_any)
	// type not known yet, may be inferred from RHS
	lvar->lv_type = type;
    अण्णहा
	// may use TTFLAG_CONST
	set_var_type(lvar, type, cctx);

    // Remember the name for debugging.
    जइ (GA_GROW_FAILS(&dfunc->df_var_names, 1))
	प्रत्यागम रिक्त;
    ((char_u **)dfunc->df_var_names.ga_data)[lvar->lv_idx] =
						    vim_strsave(lvar->lv_name);
    ++dfunc->df_var_names.ga_len;

    प्रत्यागम lvar;
}

/*
 * If "check_writable" is ASSIGN_CONST give an error if the variable was
 * defined with :final or :const, if "check_writable" is ASSIGN_FINAL give an
 * error if the variable was defined with :const.
 */
    स्थिर पूर्णाङ्क
check_item_writable(svar_T *sv, पूर्णाङ्क check_writable, char_u *name)
{
    जइ ((check_writable == ASSIGN_CONST && sv->sv_const != 0)
	    || (check_writable == ASSIGN_FINAL
					      && sv->sv_const == ASSIGN_CONST))
    {
	semsg(_(e_cannot_change_readonly_variable_str), name);
	प्रत्यागम FAIL;
    }
    प्रत्यागम OK;
}

/*
 * Find "name" in script-local items of script "sid".
 * Pass "check_writable" to check_item_writable().
 * "cctx" is NULL at the script level, "cstack" is NULL in a function.
 * Returns the index in "sn_var_vals" if found.
 * If found but not in "sn_var_vals" returns -1.
 * If not found or the variable is not writable returns -2.
 */
    पूर्णाङ्क
get_script_item_idx(
	पूर्णाङ्क	    sid,
	char_u	    *name,
	पूर्णाङ्क	    check_writable,
	cctx_T	    *cctx,
	cstack_T    *cstack)
{
    hashtab_T	    *ht;
    dictitem_T	    *di;
    scriptitem_T    *si = SCRIPT_ITEM(sid);
    svar_T	    *sv;
    पूर्णाङ्क		    idx;

    जइ (!SCRIPT_ID_VALID(sid))
	प्रत्यागम -1;
    जइ (sid == current_sctx.sc_sid)
    {
	sallvar_T *sav = find_script_var(name, 0, cctx, cstack);

	जइ (sav == रिक्त)
	    प्रत्यागम -2;
	idx = sav->sav_var_vals_idx;
	sv = ((svar_T *)si->sn_var_vals.ga_data) + idx;
	जइ (check_item_writable(sv, check_writable, name) == FAIL)
	    प्रत्यागम -2;
	प्रत्यागम idx;
    }

    // First look the name up in the hashtable.
    ht = &SCRIPT_VARS(sid);
    di = find_var_in_ht(ht, 0, name, TRUE);
    जइ (di == रिक्त)
    {
	जइ (si->sn_autoload_prefix != रिक्त)
	{
	    hashitem_T *hi;

	    // A variable exported from an autoload script is in the global
	    // variables, we can find it in the all_vars table.
	    hi = hash_find(&si->sn_all_vars.dv_hashtab, name);
	    जइ (!HASHITEM_EMPTY(hi))
		प्रत्यागम HI2SAV(hi)->sav_var_vals_idx;
	}
	प्रत्यागम -2;
    }

    // Now find the svar_T index in sn_var_vals.
    प्रति (idx = 0; idx < si->sn_var_vals.ga_len; ++idx)
    {
	sv = ((svar_T *)si->sn_var_vals.ga_data) + idx;
	जइ (sv->sv_tv == &di->di_tv)
	{
	    जइ (check_item_writable(sv, check_writable, name) == FAIL)
		प्रत्यागम -2;
	    प्रत्यागम idx;
	}
    }
    प्रत्यागम -1;
}

    स्थिर imported_T *
find_imported_in_script(char_u *name, size_t len, पूर्णाङ्क sid)
{
    scriptitem_T    *si;
    पूर्णाङ्क		    idx;

    जइ (!SCRIPT_ID_VALID(sid))
	प्रत्यागम रिक्त;
    si = SCRIPT_ITEM(sid);
    प्रति (idx = 0; idx < si->sn_imports.ga_len; ++idx)
    {
	imported_T *import = ((imported_T *)si->sn_imports.ga_data) + idx;

	जइ (len == 0 ? STRCMP(name, import->imp_name) == 0
		     : STRLEN(import->imp_name) == len
				  && STRNCMP(name, import->imp_name, len) == 0)
	    प्रत्यागम import;
    }
    प्रत्यागम रिक्त;
}

/*
 * Find "name" in imported items of the current script.
 * If "len" is 0 use any length that works.
 * If "load" is TRUE and the script was not loaded yet, load it now.
 */
    imported_T *
find_imported(char_u *name, size_t len, पूर्णाङ्क load)
{
    जइ (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	प्रत्यागम रिक्त;

    // Skip over "s:" before "s:something" to find the import name.
    पूर्णाङ्क off = name[0] == 's' && name[1] == ':' ? 2 : 0;

    imported_T *ret = find_imported_in_script(name + off, len - off,
							  current_sctx.sc_sid);
    जइ (ret != रिक्त && load && (ret->imp_flags & IMP_FLAGS_AUTOLOAD))
    {
	scid_T	actual_sid = 0;
	पूर्णाङ्क	save_emsg_off = emsg_off;

	// "emsg_off" will be set when evaluating an expression silently, but
	// we do want to know about errors in a script.  Also because it then
	// aborts when an error is encountered.
	emsg_off = FALSE;

	// script found before but not loaded yet
	ret->imp_flags &= ~IMP_FLAGS_AUTOLOAD;
	(शून्य)do_source(SCRIPT_ITEM(ret->imp_sid)->sn_name, FALSE,
						       DOSO_NONE, &actual_sid);
	// If the script is a symlink it may be sourced with another name, may
	// need to adjust the script ID for that.
	जइ (actual_sid != 0)
	    ret->imp_sid = actual_sid;

	emsg_off = save_emsg_off;
    }
    प्रत्यागम ret;
}

/*
 * Called when checking for a following operator at "arg".  When the rest of
 * the line is empty or only a comment, peek the next line.  If there is a next
 * line return a pointer to it and set "nextp".
 * Otherwise skip over white space.
 */
    char_u *
may_peek_next_पङ्क्तिः(cctx_T *cctx, char_u *arg, char_u **nextp)
{
    char_u *p = skipwhite(arg);

    *nextp = रिक्त;
    जइ (*p == NUL || (VIM_ISWHITE(*arg) && vim9_comment_start(p)))
    {
	*nextp = peek_next_पङ्क्तिः_from_context(cctx);
	जइ (*nextp != रिक्त)
	    प्रत्यागम *nextp;
    }
    प्रत्यागम p;
}

/*
 * Return a pointer to the next line that isn't empty or only contains a
 * comment. Skips over white space.
 * Returns NULL if there is none.
 */
    char_u *
peek_next_पङ्क्तिः_from_context(cctx_T *cctx)
{
    पूर्णाङ्क lnum = cctx->ctx_lnum;

    जाव (++lnum < cctx->ctx_ufunc->uf_पङ्क्तयः.ga_len)
    {
	char_u *पङ्क्तिः = ((char_u **)cctx->ctx_ufunc->uf_पङ्क्तयः.ga_data)[lnum];
	char_u *p;

	// ignore NULLs inserted for continuation lines
	जइ (पङ्क्तिः != रिक्त)
	{
	    p = skipwhite(पङ्क्तिः);
	    जइ (vim9_bad_comment(p))
		प्रत्यागम रिक्त;
	    जइ (*p != NUL && !vim9_comment_start(p))
		प्रत्यागम p;
	}
    }
    प्रत्यागम रिक्त;
}

/*
 * Get the next line of the function from "cctx".
 * Skips over empty lines.  Skips over comment lines if "skip_comment" is TRUE.
 * Returns NULL when at the end.
 */
    char_u *
next_पङ्क्तिः_from_context(cctx_T *cctx, पूर्णाङ्क skip_comment)
{
    char_u	*पङ्क्तिः;

    करेहि
    {
	++cctx->ctx_lnum;
	जइ (cctx->ctx_lnum >= cctx->ctx_ufunc->uf_पङ्क्तयः.ga_len)
	{
	    पङ्क्तिः = रिक्त;
	    भङ्ग;
	}
	पङ्क्तिः = ((char_u **)cctx->ctx_ufunc->uf_पङ्क्तयः.ga_data)[cctx->ctx_lnum];
	cctx->ctx_पङ्क्तिः_start = पङ्क्तिः;
	SOURCING_LNUM = cctx->ctx_lnum + 1;
    } जाव (पङ्क्तिः == रिक्त || *skipwhite(पङ्क्तिः) == NUL
		     || (skip_comment && vim9_comment_start(skipwhite(पङ्क्तिः))));
    प्रत्यागम पङ्क्तिः;
}

/*
 * Skip over white space at "whitep" and assign to "*arg".
 * If "*arg" is at the end of the line, advance to the next line.
 * Also when "whitep" points to white space and "*arg" is on a "#".
 * Return FAIL if beyond the last line, "*arg" is unmodified then.
 */
    पूर्णाङ्क
may_get_next_पङ्क्तिः(char_u *whitep, char_u **arg, cctx_T *cctx)
{
    *arg = skipwhite(whitep);
    जइ (vim9_bad_comment(*arg))
	प्रत्यागम FAIL;
    जइ (**arg == NUL || (VIM_ISWHITE(*whitep) && vim9_comment_start(*arg)))
    {
	char_u *next = next_पङ्क्तिः_from_context(cctx, TRUE);

	जइ (next == रिक्त)
	    प्रत्यागम FAIL;
	*arg = skipwhite(next);
    }
    प्रत्यागम OK;
}

/*
 * Idem, and give an error when failed.
 */
    पूर्णाङ्क
may_get_next_पङ्क्तिः_error(char_u *whitep, char_u **arg, cctx_T *cctx)
{
    जइ (may_get_next_पङ्क्तिः(whitep, arg, cctx) == FAIL)
    {
	SOURCING_LNUM = cctx->ctx_lnum + 1;
	emsg(_(e_पङ्क्तिः_incomplete));
	प्रत्यागम FAIL;
    }
    प्रत्यागम OK;
}

/*
 * Get a line from the compilation context, compatible with exarg_T getline().
 * Return a pointer to the line in allocated memory.
 * Return NULL for end-of-file or some error.
 */
    स्थिर char_u *
exarg_getline(
	पूर्णाङ्क c UNUSED,
	शून्य *cookie,
	पूर्णाङ्क indent UNUSED,
	getline_opt_T options UNUSED)
{
    cctx_T  *cctx = (cctx_T *)cookie;
    char_u  *p;

    प्रति (;;)
    {
	जइ (cctx->ctx_lnum >= cctx->ctx_ufunc->uf_पङ्क्तयः.ga_len - 1)
	    प्रत्यागम रिक्त;
	++cctx->ctx_lnum;
	p = ((char_u **)cctx->ctx_ufunc->uf_पङ्क्तयः.ga_data)[cctx->ctx_lnum];
	// Comment lines result in NULL pointers, skip them.
	जइ (p != रिक्त)
	    प्रत्यागम vim_strsave(p);
    }
}

    शून्य
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
    पूर्णाङ्क
func_needs_compiling(ufunc_T *ufunc, compiletype_T compile_type)
{
    विकल्प (ufunc->uf_def_status)
    {
	पक्ष UF_TO_BE_COMPILED:
	    प्रत्यागम TRUE;

	पक्ष UF_COMPILED:
	{
	    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data)
							 + ufunc->uf_dfunc_idx;

	    विकल्प (compile_type)
	    {
		पक्ष CT_PROFILE:
#ifdef FEAT_PROFILE
		    प्रत्यागम dfunc->df_instr_prof == रिक्त;
#endif
		पक्ष CT_NONE:
		    प्रत्यागम dfunc->df_instr == रिक्त;
		पक्ष CT_DEBUG:
		    प्रत्यागम dfunc->df_instr_debug == रिक्त;
	    }
	}

	पक्ष UF_NOT_COMPILED:
	पक्ष UF_COMPILE_ERROR:
	पक्ष UF_COMPILING:
	    भङ्ग;
    }
    प्रत्यागम FALSE;
}

/*
 * Compile a nested :def command.
 */
    स्थिर char_u *
compile_nested_function(exarg_T *eap, cctx_T *cctx, garray_T *पङ्क्तयः_to_free)
{
    पूर्णाङ्क		is_global = *eap->arg == 'g' && eap->arg[1] == ':';
    char_u	*name_start = eap->arg;
    char_u	*name_end = to_name_end(eap->arg, TRUE);
    पूर्णाङ्क		off;
    char_u	*func_name;
    string_T	lambda_name;
    ufunc_T	*ufunc;
    पूर्णाङ्क		r = FAIL;
    compiletype_T   compile_type;
    पूर्णाङ्क		funcref_isn_idx = -1;
    lvar_T	*lvar = रिक्त;
    char_u	*bracket_start = रिक्त;

    जइ (eap->forceit)
    {
	semsg(_(e_cannot_use_bang_with_nested_def_str),
		eap->cmdidx == CMD_def ? ":def" : ":function");
	प्रत्यागम रिक्त;
    }

    जइ (*name_start == '/')
    {
	name_end = skip_regexp(name_start + 1, '/', TRUE);
	जइ (*name_end == '/')
	    ++name_end;
	set_nextcmd(eap, name_end);
    }

    जइ (*name_end == '<')
    {
	bracket_start = name_end;
	जइ (skip_generic_func_type_args(&name_end) == FAIL)
	    प्रत्यागम रिक्त;
    }

    जइ (name_end == name_start || *skipwhite(name_end) != '(')
    {
	जइ (!ends_excmd2(name_start, name_end))
	{
	    जइ (*skipwhite(name_end) == '.')
		semsg(_(e_cannot_define_dict_func_in_vim9_script_str),
								     eap->cmd);
	    अण्णहा
		semsg(_(e_invalid_command_str), eap->cmd);
	    प्रत्यागम रिक्त;
	}

	// "def" or "def Name": list functions
	जइ (generate_DEF(cctx, name_start, name_end - name_start) == FAIL)
	    प्रत्यागम रिक्त;
	प्रत्यागम eap->nextcmd == रिक्त ? (char_u *)"" : eap->nextcmd;
    }

    जइ (bracket_start != रिक्त)
	// generic function.  The function name ends before the list of types
	// (opening angle bracket).
	name_end = bracket_start;

    // Only g:Func() can use a namespace.
    जइ (name_start[1] == ':' && !is_global)
    {
	semsg(_(e_namespace_not_supported_str), name_start);
	प्रत्यागम रिक्त;
    }
    जइ (cctx->ctx_skip != SKIP_YES
	    && check_defined(name_start, name_end - name_start, cctx,
							  रिक्त, FALSE) == FAIL)
	प्रत्यागम रिक्त;
    जइ (!ASCII_ISUPPER(is_global ? name_start[2] : name_start[0]))
    {
	semsg(_(e_function_name_must_start_with_capital_str), name_start);
	प्रत्यागम रिक्त;
    }

    eap->arg = name_end;
    fill_exarg_from_cctx(eap, cctx);

    eap->forceit = FALSE;
    // We use the special <lambda>99 name, but it's not really a lambda.
    lambda_name = get_lambda_name();
    lambda_name.string = vim_strnsave(lambda_name.string, lambda_name.length);
    जइ (lambda_name.string == रिक्त)
	प्रत्यागम रिक्त;

    // This may free the current line, make a copy of the name.
    off = is_global ? 2 : 0;
    func_name = vim_strnsave(name_start + off, name_end - name_start - off);
    जइ (func_name == रिक्त)
    {
	r = FAIL;
	गच्छ theend;
    }

    // A type variable cannot have the name of the function.  The function is
    // defined with the "<lambda>" name, check it here.
    जइ (bracket_start != रिक्त)
    {
	size_t	len = STRLEN(func_name);

	प्रति (char_u *t = bracket_start + 1; *t != '>' && *t != NUL; )
	{
	    char_u *te = to_name_end(t, FALSE);

	    जइ ((size_t)(te - t) == len && STRNCMP(t, func_name, len) == 0)
	    {
		semsg(_(e_redefining_script_item_str), func_name);
		r = FAIL;
		गच्छ theend;
	    }
	    t = te == t ? t + 1 : te;
	}
    }

    // Make sure "KeyTyped" is not set, it may cause indent to be written.
    पूर्णाङ्क save_कुञ्चिका_Typed = कुञ्चिका_Typed;
    कुञ्चिका_Typed = FALSE;

    ufunc = define_function(eap, lambda_name.string, पङ्क्तयः_to_free, 0, रिक्त, 0,
									cctx);

    कुञ्चिका_Typed = save_कुञ्चिका_Typed;

    जइ (ufunc == रिक्त)
    {
	r = eap->skip ? OK : FAIL;
	गच्छ theend;
    }
    जइ (eap->nextcmd != रिक्त)
    {
	semsg(_(e_text_found_after_str_str),
	      eap->cmdidx == CMD_def ? "enddef" : "endfunction", eap->nextcmd);
	r = FAIL;
	func_ptr_unref(ufunc);
	गच्छ theend;
    }

    // copy over the block scope IDs before compiling
    जइ (!is_global && cctx->ctx_ufunc->uf_block_depth > 0)
    {
	पूर्णाङ्क block_depth = cctx->ctx_ufunc->uf_block_depth;

	ufunc->uf_block_ids = ALLOC_MULT(पूर्णाङ्क, block_depth);
	जइ (ufunc->uf_block_ids != रिक्त)
	{
	    mch_memmove(ufunc->uf_block_ids, cctx->ctx_ufunc->uf_block_ids,
						    परिमाण(पूर्णाङ्क) * block_depth);
	    ufunc->uf_block_depth = block_depth;
	}
    }

    // Define the funcref before compiling, so that it is found by any
    // recursive call.
    जइ (is_global)
    {
	r = generate_NEWFUNC(cctx, lambda_name.string, func_name);
	func_name = रिक्त;
	lambda_name.string = रिक्त;
	lambda_name.length = 0;
    }
    अण्णहा
    {
	// Define a local variable for the function reference.
	lvar = reserve_local(cctx, func_name, name_end - name_start,
					    ASSIGN_CONST, ufunc->uf_func_type);
	जइ (lvar == रिक्त)
	{
	    func_ptr_unref(ufunc);
	    गच्छ theend;
	}
	lvar->lv_ufunc = ufunc;
	जइ (generate_FUNCREF(cctx, ufunc, रिक्त, FALSE, 0, &funcref_isn_idx) == FAIL)
	{
	    func_ptr_unref(ufunc);
	    गच्छ theend;
	}
	r = generate_STORE(cctx, ISN_STORE, lvar->lv_idx, रिक्त);
    }

    compile_type = get_compile_type(ufunc);
#ifdef FEAT_PROFILE
    // If the outer function is profiled, also compile the nested function for
    // profiling.
    जइ (cctx->ctx_compile_type == CT_PROFILE)
	compile_type = CT_PROFILE;
#endif
    जइ (func_needs_compiling(ufunc, compile_type)
	    && compile_def_function(ufunc, TRUE, compile_type, cctx) == FAIL)
    {
	func_ptr_unref(ufunc);
	जइ (lvar != रिक्त)
	    // Now the local variable can't be used.
	    *lvar->lv_name = '/';  // impossible value
	गच्छ theend;
    }

#ifdef FEAT_PROFILE
    // When the outer function is compiled for profiling, the nested function
    // may be called without profiling.  Compile it here in the right context.
    जइ (compile_type == CT_PROFILE && func_needs_compiling(ufunc, CT_NONE))
	compile_def_function(ufunc, FALSE, CT_NONE, cctx);
#endif

    // If a FUNCREF instruction was generated, set the index after compiling.
    जइ (funcref_isn_idx != -1 && ufunc->uf_def_status == UF_COMPILED)
    {
	isn_T	*funcref_isn = ((isn_T *)cctx->ctx_instr.ga_data) +
							funcref_isn_idx;
	funcref_isn->isn_arg.funcref.fr_dfunc_idx = ufunc->uf_dfunc_idx;
    }

theend:
    vim_free(lambda_name.string);
    vim_free(func_name);
    प्रत्यागम r == FAIL ? रिक्त : (char_u *)"";
}

/*
 * Compile one Vim expression {expr} in string "p".
 * "p" points to the opening "{".
 * Return a pointer to the character after "}", NULL for an error.
 */
    char_u *
compile_one_व्यञ्जकम्_in_str(char_u *p, cctx_T *cctx)
{
    char_u	*block_start;
    char_u	*block_end;

    // Skip the opening {.
    block_start = skipwhite(p + 1);
    block_end = block_start;
    जइ (*block_start != NUL && skip_व्यञ्जकम्(&block_end, रिक्त) == FAIL)
	प्रत्यागम रिक्त;
    block_end = skipwhite(block_end);
    // The block must be closed by a }.
    जइ (*block_end != '}')
    {
	semsg(_(e_missing_close_curly_str), p);
	प्रत्यागम रिक्त;
    }
    जइ (compile_expr0(&block_start, cctx) == FAIL)
	प्रत्यागम रिक्त;
    may_generate_2STRING(-1, TOSTRING_INTERPOLATE, cctx);

    प्रत्यागम block_end + 1;
}

/*
 * Compile a string "str" (either containing a literal string or a mix of
 * literal strings and Vim expressions of the form `{expr}`).  This is used
 * when compiling a heredoc assignment to a variable or an interpolated string
 * in a Vim9 def function.  Vim9 instructions are generated to push strings,
 * evaluate expressions, concatenate them and create a list of lines.  When
 * "evalstr" is TRUE, Vim expressions in "str" are evaluated.
 */
    पूर्णाङ्क
compile_all_व्यञ्जकम्_in_str(char_u *str, पूर्णाङ्क evalstr, cctx_T *cctx)
{
    char_u	*p = str;
    char_u	*val;
    पूर्णाङ्क		गणनम् = 0;

    जइ (cctx->ctx_skip == SKIP_YES)
	प्रत्यागम OK;

    जइ (!evalstr || *str == NUL)
    {
	// Literal string, possibly empty.
	val = *str != NUL ? vim_strsave(str) : रिक्त;
	प्रत्यागम generate_PUSHS(cctx, &val);
    }

    // Push all the string pieces to the stack, followed by a ISN_CONCAT.
    जाव (*p != NUL)
    {
	char_u	*lit_start;
	पूर्णाङ्क	escaped_brace = FALSE;

	// Look for a block start.
	lit_start = p;
	जाव (*p != '{' && *p != '}' && *p != NUL)
	    ++p;

	जइ (*p != NUL && *p == p[1])
	{
	    // Escaped brace, unescape and continue.
	    // Include the brace in the literal string.
	    ++p;
	    escaped_brace = TRUE;
	}
	अण्णहा जइ (*p == '}')
	{
	    semsg(_(e_stray_closing_curly_str), str);
	    प्रत्यागम FAIL;
	}

	// Append the literal part.
	जइ (p != lit_start)
	{
	    val = vim_strnsave(lit_start, (size_t)(p - lit_start));
	    जइ (generate_PUSHS(cctx, &val) == FAIL)
		प्रत्यागम FAIL;
	    ++गणनम्;
	}

	जइ (*p == NUL)
	    भङ्ग;

	जइ (escaped_brace)
	{
	    // Skip the second brace.
	    ++p;
	    अनुवर्त;
	}

	p = compile_one_व्यञ्जकम्_in_str(p, cctx);
	जइ (p == रिक्त)
	    प्रत्यागम FAIL;
	++गणनम्;
    }

    // Small optimization, if there's only a single piece skip the ISN_CONCAT.
    जइ (गणनम् > 1)
	प्रत्यागम generate_CONCAT(cctx, गणनम्);

    प्रत्यागम OK;
}

/*
 * Return the length of an assignment operator, or zero if there isn't one.
 */
    पूर्णाङ्क
assignment_len(char_u *p, पूर्णाङ्क *heredoc)
{
    जइ (*p == '=')
    {
	जइ (p[1] == '<' && p[2] == '<')
	{
	    *heredoc = TRUE;
	    प्रत्यागम 3;
	}
	प्रत्यागम 1;
    }
    जइ (vim_strchr((char_u *)"+-*/%", *p) != रिक्त && p[1] == '=')
	प्रत्यागम 2;
    जइ (STRNCMP(p, "..=", 3) == 0)
	प्रत्यागम 3;
    प्रत्यागम 0;
}

/*
 * Generate the load instruction for "name".
 */
    स्थिर पूर्णाङ्क
generate_loadvar(cctx_T *cctx, lhs_T *lhs)
{
    char_u	*name = lhs->lhs_name;
    type_T	*type = lhs->lhs_type;
    पूर्णाङ्क		res = OK;

    विकल्प (lhs->lhs_dest)
    {
	पक्ष dest_option:
	पक्ष dest_func_option:
	    generate_LOAD(cctx, ISN_LOADOPT, 0, name, type);
	    भङ्ग;
	पक्ष dest_global:
	    जइ (vim_strchr(name, AUTOLOAD_CHAR) == रिक्त)
	    {
		जइ (name[2] == NUL)
		    generate_instr_type(cctx, ISN_LOADGDICT, &t_dict_any);
		अण्णहा
		    generate_LOAD(cctx, ISN_LOADG, 0, name + 2, type);
	    }
	    अण्णहा
		generate_LOAD(cctx, ISN_LOADAUTO, 0, name, type);
	    भङ्ग;
	पक्ष dest_buffer:
	    generate_LOAD(cctx, ISN_LOADB, 0, name + 2, type);
	    भङ्ग;
	पक्ष dest_window:
	    generate_LOAD(cctx, ISN_LOADW, 0, name + 2, type);
	    भङ्ग;
	पक्ष dest_tab:
	    generate_LOAD(cctx, ISN_LOADT, 0, name + 2, type);
	    भङ्ग;
	पक्ष dest_script:
	पक्ष dest_script_v9:
	    res = compile_load_scriptvar(cctx,
			    name + (name[1] == ':' ? 2 : 0), रिक्त, रिक्त, रिक्त);
	    भङ्ग;
	पक्ष dest_env:
	    // Include $ in the name here
	    generate_LOAD(cctx, ISN_LOADENV, 0, name, type);
	    भङ्ग;
	पक्ष dest_reg:
	    generate_LOAD(cctx, ISN_LOADREG, name[1], रिक्त, &t_string);
	    भङ्ग;
	पक्ष dest_vimvar:
	    generate_LOADV(cctx, name + 2);
	    भङ्ग;
	पक्ष dest_local:
	    जइ (cctx->ctx_skip != SKIP_YES)
	    {
		lvar_T	*lvar = lhs->lhs_lvar;
		जइ (lvar->lv_from_outer > 0)
		    generate_LOADOUTER(cctx, lvar->lv_idx, lvar->lv_from_outer,
				 lvar->lv_loop_depth, lvar->lv_loop_idx, type);
		अण्णहा
		    generate_LOAD(cctx, ISN_LOAD, lvar->lv_idx, रिक्त, type);
	    }
	    भङ्ग;
	पक्ष dest_class_member:
	    generate_CLASSMEMBER(cctx, TRUE, lhs->lhs_class,
						     lhs->lhs_classmember_idx);
	    भङ्ग;
	पक्ष dest_व्यञ्जकम्:
	    // list or dict value should already be on the stack.
	    भङ्ग;
    }

    प्रत्यागम res;
}

/*
 * Skip over "[expr]" or ".member".
 * Does not check for any errors.
 */
    स्थिर char_u *
skip_index(char_u *start)
{
    char_u *p = start;

    जइ (*p == '[')
    {
	p = skipwhite(p + 1);
	(शून्य)skip_व्यञ्जकम्(&p, रिक्त);
	p = skipwhite(p);
	जइ (*p == ']')
	    प्रत्यागम p + 1;
	प्रत्यागम p;
    }
    // if (*p == '.')
    प्रत्यागम to_name_end(p + 1, TRUE);
}

    शून्य
vim9_declare_error(char_u *name)
{
    वर्ण *scope = "";

    विकल्प (*name)
    {
	पक्ष 'g': scope = _("global"); भङ्ग;
	पक्ष 'b': scope = _("buffer"); भङ्ग;
	पक्ष 'w': scope = _("window"); भङ्ग;
	पक्ष 't': scope = _("tab"); भङ्ग;
	पक्ष 'v': scope = "v:"; भङ्ग;
	पक्ष '$': semsg(_(e_cannot_declare_an_environment_variable_str), name);
		  प्रत्यागम;
	पक्ष '&': semsg(_(e_cannot_declare_an_option_str), name);
		  प्रत्यागम;
	पक्ष '@': semsg(_(e_cannot_declare_a_register_str), name);
		  प्रत्यागम;
	स्वभाव: प्रत्यागम;
    }
    semsg(_(e_cannot_declare_a_scope_variable_str), scope, name);
}

/*
 * Return TRUE if "name" is a valid register to use.
 * Return FALSE and give an error message if not.
 */
    स्थिर पूर्णाङ्क
valid_dest_reg(पूर्णाङ्क name)
{
    जइ (name == '@')
       name = '"';
    जइ (name == '/' || name == '=' || name == '#' || valid_yank_reg(name, TRUE))
	प्रत्यागम TRUE;
    emsg_invreg(name);
    प्रत्यागम FAIL;
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
    पूर्णाङ्क
get_var_dest(
	char_u		*name,
	assign_dest_T	*dest,
	cmdidx_T	cmdidx,
	पूर्णाङ्क		has_index,
	पूर्णाङ्क		*option_scope,
	पूर्णाङ्क		*vimvaridx,
	type_T		**type,
	cctx_T		*cctx)
{
    char_u *p;

    जइ (*name == '&')
    {
	पूर्णाङ्क		cc;
	long		numval;
	getoption_T	opt_type;
	पूर्णाङ्क		opt_p_flags;

	*dest = dest_option;
	जइ (cmdidx == CMD_final || cmdidx == CMD_const)
	{
	    emsg(_(e_cannot_lock_option));
	    प्रत्यागम FAIL;
	}
	p = name;
	p = find_option_end(&p, option_scope);
	जइ (p == रिक्त)
	{
	    // cannot happen?
	    emsg(_(e_unexpected_characters_in_assignment));
	    प्रत्यागम FAIL;
	}
	cc = *p;
	*p = NUL;
	opt_type = get_option_मानम्(skip_option_env_lead(name),
				   &numval, रिक्त, &opt_p_flags, *option_scope);
	*p = cc;
	विकल्प (opt_type)
	{
	    पक्ष gov_unknown:
		    semsg(_(e_unknown_option_str), name);
		    प्रत्यागम FAIL;
	    पक्ष gov_string:
	    पक्ष gov_hidden_string:
		    जइ (opt_p_flags & P_FUNC)
		    {
			// might be a Funcref, check the type later
			*type = &t_any;
			*dest = dest_func_option;
		    }
		    अण्णहा
		    {
			*type = &t_string;
		    }
		    भङ्ग;
	    पक्ष gov_bool:
	    पक्ष gov_hidden_bool:
		    *type = &t_bool;
		    भङ्ग;
	    पक्ष gov_सङ्ख्या:
	    पक्ष gov_hidden_सङ्ख्या:
		    *type = &t_सङ्ख्या;
		    भङ्ग;
	}
    }
    अण्णहा जइ (*name == '$')
    {
	*dest = dest_env;
	*type = &t_string;
    }
    अण्णहा जइ (*name == '@')
    {
	जइ (!valid_dest_reg(name[1]))
	    प्रत्यागम FAIL;
	*dest = dest_reg;
	*type = name[1] == '#' ? &t_सङ्ख्या_or_string : &t_string;
    }
    अण्णहा जइ (STRNCMP(name, "g:", 2) == 0)
    {
	*dest = dest_global;
    }
    अण्णहा जइ (STRNCMP(name, "b:", 2) == 0)
    {
	*dest = dest_buffer;
    }
    अण्णहा जइ (STRNCMP(name, "w:", 2) == 0)
    {
	*dest = dest_window;
    }
    अण्णहा जइ (STRNCMP(name, "t:", 2) == 0)
    {
	*dest = dest_tab;
    }
    अण्णहा जइ (STRNCMP(name, "v:", 2) == 0)
    {
	typval_T	*vtv;
	पूर्णाङ्क		di_flags;

	*vimvaridx = find_vim_var(name + 2, &di_flags);
	जइ (*vimvaridx < 0)
	{
	    semsg(_(e_variable_not_found_str), name);
	    प्रत्यागम FAIL;
	}
	// An item of a read-only variable can be changed, as when not compiled.
	// We use the current value of "sandbox" here, is that OK?
	जइ (!has_index && var_check_ro(di_flags, name, FALSE))
	    प्रत्यागम FAIL;
	*dest = dest_vimvar;
	vtv = get_vim_var_tv(*vimvaridx);
	*type = typval2type_vimvar(vtv, cctx->ctx_type_list);
    }
    प्रत्यागम OK;
}

    स्थिर पूर्णाङ्क
is_decl_command(cmdidx_T cmdidx)
{
    प्रत्यागम cmdidx == CMD_let || cmdidx == CMD_var
				 || cmdidx == CMD_final || cmdidx == CMD_const;
}

/*
 * Returns TRUE if the class or object variable in "lhs" is modifiable.
 * "var_start" points to the start of the variable name and "lhs->lhs_varlen"
 * has the total length.  Note that the "lhs" can be nested an object reference
 * (e.g.  a.b.c.d.var).
 */
    स्थिर पूर्णाङ्क
lhs_class_member_modifiable(lhs_T *lhs, char_u	*var_start, cctx_T *cctx)
{
    size_t	varlen = lhs->lhs_varlen;
    class_T	*cl = lhs->lhs_type->tt_class;
    पूर्णाङ्क		is_object = lhs->lhs_type->tt_type == VAR_OBJECT;
    char_u	*name = var_start + varlen + 1;
    size_t	namelen = lhs->lhs_end - var_start - varlen - 1;
    ocmember_T	*m;

    m = member_lookup(cl, lhs->lhs_type->tt_type, name, namelen, रिक्त);
    जइ (m == रिक्त)
    {
	member_not_found_सन्देशः(cl, lhs->lhs_type->tt_type, name, namelen);
	प्रत्यागम FALSE;
    }

    जइ (IS_ENUM(cl))
    {
	semsg(_(e_enumvalue_str_cannot_be_modified), cl->class_name.string,
		m->ocm_name.string);
	प्रत्यागम FALSE;
    }

    // If it is private member variable, then accessing it outside the
    // class is not allowed.
    // If it is a read only class variable, then it can be modified
    // only inside the class where it is defined.
    जइ ((m->ocm_access != VIM_ACCESS_ALL) &&
	    ((is_object && !inside_class(cctx, cl))
	     || (!is_object && cctx->ctx_ufunc->uf_class != cl)))
    {
	वर्ण *सन्देशः = (m->ocm_access == VIM_ACCESS_PRIVATE)
				? e_cannot_access_protected_variable_str
				: e_variable_is_not_writable_str;
	emsg_var_cl_define(सन्देशः, m->ocm_name.string, 0, cl);
	प्रत्यागम FALSE;
    }

    प्रत्यागम TRUE;
}

/*
 * Initialize "lhs" with default values
 */
    स्थिर शून्य
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
    स्थिर पूर्णाङ्क
lhs_find_var_end(
    lhs_T	*lhs,
    char_u	*var_start,
    पूर्णाङ्क		is_decl,
    char_u	**var_endp)
{
    char_u  *var_end = *var_endp;

    // "lhs_dest_end" is the end of the destination, including "[expr]" or
    // ".name".
    // "var_end" is the end of the variable/option/etc. name.
    lhs->lhs_dest_end = skip_var_one(var_start, FALSE);
    जइ (*var_start == '@')
    {
	जइ (!valid_dest_reg(var_start[1]))
	    प्रत्यागम FAIL;
	var_end = var_start + 2;
    }
    अण्णहा
    {
	// skip over the leading "&", "&l:", "&g:" and "$"
	var_end = skip_option_env_lead(var_start);
	var_end = to_name_end(var_end, TRUE);
    }

    // "a: type" is declaring variable "a" with a type, not dict "a:".
    जइ (is_decl && lhs->lhs_dest_end == var_start + 2
					&& lhs->lhs_dest_end[-1] == ':')
	--lhs->lhs_dest_end;
    जइ (is_decl && var_end == var_start + 2 && var_end[-1] == ':')
	--var_end;

    lhs->lhs_end = lhs->lhs_dest_end;
    *var_endp = var_end;

    प्रत्यागम OK;
}

/*
 * Set various fields in "lhs"
 */
    स्थिर पूर्णाङ्क
lhs_init(
    lhs_T	*lhs,
    char_u	*var_start,
    पूर्णाङ्क		is_decl,
    पूर्णाङ्क		heredoc,
    char_u	**var_endp)
{
    char_u *var_end = *var_endp;

    lhs_init_defaults(lhs);

    // Find the end of the variable and the destination
    जइ (lhs_find_var_end(lhs, var_start, is_decl, &var_end) == FAIL)
	प्रत्यागम FAIL;

    // compute the length of the destination without "[expr]" or ".name"
    lhs->lhs_varlen = var_end - var_start;
    lhs->lhs_varlen_total = lhs->lhs_varlen;
    lhs->lhs_name = vim_strnsave(var_start, lhs->lhs_varlen);
    जइ (lhs->lhs_name == रिक्त)
	प्रत्यागम FAIL;

    जइ (lhs->lhs_dest_end > var_start + lhs->lhs_varlen)
	// Something follows after the variable: "var[idx]" or "var.key".
	lhs->lhs_has_index = TRUE;

    lhs->lhs_type = heredoc ? &t_list_string : &t_any;

    *var_endp = var_end;

    प्रत्यागम OK;
}

/*
 * Compile a LHS class variable name.
 */
    स्थिर पूर्णाङ्क
compile_lhs_class_variable(
    cctx_T	*cctx,
    lhs_T	*lhs,
    class_T	*defcl,
    पूर्णाङ्क		is_decl)
{
    जइ (cctx->ctx_ufunc->uf_defclass != defcl)
    {
	// A class variable can be accessed without the class name
	// only inside a class.
	semsg(_(e_class_variable_str_accessible_only_inside_class_str),
		lhs->lhs_name, defcl->class_name.string);
	प्रत्यागम FAIL;
    }

    जइ (is_decl)
    {
	semsg(_(e_variable_already_declared_in_class_str), lhs->lhs_name);
	प्रत्यागम FAIL;
    }

    ocmember_T	*m = &defcl->class_class_members[lhs->lhs_classmember_idx];
    जइ (oc_var_check_ro(defcl, m))
	प्रत्यागम FAIL;

    lhs->lhs_dest = dest_class_member;
    // The class variable is defined either in the current class or
    // in one of the parent class in the hierarchy.
    lhs->lhs_class = defcl;
    lhs->lhs_type = oc_member_type_by_idx(defcl, FALSE,
						lhs->lhs_classmember_idx);

    प्रत्यागम OK;
}

/*
 * Compile an imported LHS variable
 */
    स्थिर पूर्णाङ्क
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
    जइ (dot == रिक्त)
    {
	semsg(_(e_no_dot_after_imported_name_str), var_start);
	प्रत्यागम FAIL;
    }

    p = skipwhite(dot + 1);
    var_end = to_name_end(p, TRUE);
    जइ (var_end == p)
    {
	semsg(_(e_missing_name_after_imported_name_str), var_start);
	प्रत्यागम FAIL;
    }

    vim_free(lhs->lhs_name);
    lhs->lhs_varlen = var_end - p;
    lhs->lhs_name = vim_strnsave(p, lhs->lhs_varlen);
    जइ (lhs->lhs_name == रिक्त)
	प्रत्यागम FAIL;
    *rawnamep = lhs->lhs_name;
    lhs->lhs_scriptvar_sid = import->imp_sid;

    // TODO: where do we check this name is exported?

    // Check if something follows: "exp.var[idx]" or
    // "exp.var.key".
    lhs->lhs_has_index = lhs->lhs_dest_end > skipwhite(var_end);

    *var_endp = var_end;

    प्रत्यागम OK;
}

/*
 * Process a script-local variable when compiling a LHS variable name.
 */
    स्थिर पूर्णाङ्क
compile_lhs_script_var(
    cctx_T	*cctx,
    lhs_T	*lhs,
    char_u	*var_start,
    char_u	*var_end,
    पूर्णाङ्क		is_decl)
{
    पूर्णाङ्क		script_namespace = FALSE;
    पूर्णाङ्क		script_var = FALSE;
    imported_T	*import;
    char_u	*var_name;
    size_t	var_name_len;

    जइ (lhs->lhs_varlen > 1 && STRNCMP(var_start, "s:", 2) == 0)
	script_namespace = TRUE;

    जइ (script_namespace)
    {
	var_name = var_start + 2;
	var_name_len = lhs->lhs_varlen - 2;
    }
    अण्णहा
    {
	var_name = var_start;
	var_name_len = lhs->lhs_varlen;
    }

    जइ (script_var_exists(var_name, var_name_len, cctx, रिक्त) == OK)
	script_var = TRUE;

    import = find_imported(var_start, lhs->lhs_varlen, FALSE);

    जइ (script_namespace || script_var || import != रिक्त)
    {
	char_u *rawname = lhs->lhs_name + (lhs->lhs_name[1] == ':' ? 2 : 0);

	जइ (script_namespace && current_script_is_vim9())
	{
	    semsg(_(e_cannot_use_s_colon_in_vim9_script_str), var_start);
	    प्रत्यागम FAIL;
	}

	जइ (is_decl)
	{
	    जइ (script_namespace)
		semsg(_(e_cannot_declare_script_variable_in_function_str),
			lhs->lhs_name);
	    अण्णहा
		semsg(_(e_variable_already_declared_in_script_str),
			lhs->lhs_name);
	    प्रत्यागम FAIL;
	}
	अण्णहा जइ (cctx->ctx_ufunc->uf_script_ctx_version == SCRIPT_VERSION_VIM9
		&& script_namespace
		&& !script_var && import == रिक्त)
	{
	    semsg(_(e_unknown_variable_str), lhs->lhs_name);
	    प्रत्यागम FAIL;
	}

	lhs->lhs_dest = current_script_is_vim9() ? dest_script_v9 :
								dest_script;

	// existing script-local variables should have a type
	lhs->lhs_scriptvar_sid = current_sctx.sc_sid;
	जइ (import != रिक्त)
	{
	    जइ (compile_lhs_import_var(lhs, import, var_start, &var_end,
							&rawname) == FAIL)
		प्रत्यागम FAIL;
	}

	जइ (SCRIPT_ID_VALID(lhs->lhs_scriptvar_sid))
	{
	    // Check writable only when no index follows.
	    lhs->lhs_scriptvar_idx = get_script_item_idx(
					lhs->lhs_scriptvar_sid, rawname,
					lhs->lhs_has_index ?  ASSIGN_FINAL :
					ASSIGN_CONST, cctx, रिक्त);
	    जइ (lhs->lhs_scriptvar_idx >= 0)
	    {
		scriptitem_T *si = SCRIPT_ITEM(lhs->lhs_scriptvar_sid);
		svar_T	 *sv = ((svar_T *)si->sn_var_vals.ga_data)
						+ lhs->lhs_scriptvar_idx;

		lhs->lhs_type = sv->sv_type;
	    }
	}

	प्रत्यागम OK;
    }

    प्रत्यागम check_defined(var_start, lhs->lhs_varlen, cctx, रिक्त, FALSE);
}

/*
 * Compile the LHS destination.
 */
    स्थिर पूर्णाङ्क
compile_lhs_var_dest(
    cctx_T	*cctx,
    lhs_T	*lhs,
    पूर्णाङ्क		cmdidx,
    char_u	*var_start,
    char_u	*var_end,
    पूर्णाङ्क		is_decl,
    पूर्णाङ्क		has_cmd)	// "var" before "var_start"
{
    पूर्णाङ्क	    declare_error = FALSE;

    जइ (get_var_dest(lhs->lhs_name, &lhs->lhs_dest, cmdidx,
				lhs->lhs_has_index,
				&lhs->lhs_opt_flags, &lhs->lhs_vimvaridx,
				&lhs->lhs_type, cctx) == FAIL)
	प्रत्यागम FAIL;

    जइ (lhs->lhs_dest != dest_local && cmdidx != CMD_const
						&& cmdidx != CMD_final)
    {
	// Specific kind of variable recognized.
	declare_error = is_decl;
    }
    अण्णहा
    {
	class_T	*defcl;

	// No specific kind of variable recognized, just a name.
	जइ (check_reserved_name(lhs->lhs_name, lhs->lhs_has_index
						&& *var_end == '.') == FAIL)
	    प्रत्यागम FAIL;

	जइ (lookup_local(var_start, lhs->lhs_varlen, &lhs->lhs_local_lvar,
								cctx) == OK)
	{
	    lhs->lhs_lvar = &lhs->lhs_local_lvar;
	}
	अण्णहा
	{
	    CLEAR_FIELD(lhs->lhs_arg_lvar);
	    जइ (arg_exists(var_start, lhs->lhs_varlen,
			&lhs->lhs_arg_lvar.lv_idx, &lhs->lhs_arg_lvar.lv_type,
			&lhs->lhs_arg_lvar.lv_from_outer, cctx) == OK)
	    {
		जइ (is_decl)
		{
		    semsg(_(e_str_is_used_as_argument), lhs->lhs_name);
		    प्रत्यागम FAIL;
		}
		lhs->lhs_lvar = &lhs->lhs_arg_lvar;
	    }
	}

	जइ (lhs->lhs_lvar != रिक्त)
	{
	    जइ (is_decl)
	    {
		// if we come here with what looks like an assignment like
		// .= but which has been rejected by assignment_len() from
		// may_compile_assignment give a better error message
		char_u *p = skipwhite(lhs->lhs_end);
		जइ (p[0] == '.' && p[1] == '=')
		    emsg(_(e_dot_equal_not_supported_with_script_version_two));
		अण्णहा जइ (p[0] == ':' && !has_cmd)
		    // type specified in an assignment without "var"
		    semsg(_(e_trailing_characters_str), p);
		अण्णहा
		    semsg(_(e_variable_already_declared_str), lhs->lhs_name);
		प्रत्यागम FAIL;
	    }
	}
	अण्णहा जइ ((lhs->lhs_classmember_idx = cctx_class_member_idx(
			cctx, var_start, lhs->lhs_varlen, &defcl)) >= 0)
	{
	    जइ (compile_lhs_class_variable(cctx, lhs, defcl, is_decl)
		    == FAIL)
		प्रत्यागम FAIL;
	}
	अण्णहा
	{
	    जइ (compile_lhs_script_var(cctx, lhs, var_start, var_end,
			is_decl) == FAIL)
		प्रत्यागम FAIL;
	}
    }

    जइ (declare_error)
    {
	vim9_declare_error(lhs->lhs_name);
	प्रत्यागम FAIL;
    }

    प्रत्यागम OK;
}

/*
 * When compiling a LHS variable name, for a class or an object, set the LHS
 * member type.
 */
    स्थिर पूर्णाङ्क
compile_lhs_set_oc_member_type(
    cctx_T	*cctx,
    lhs_T	*lhs,
    char_u	*var_start)
{
    class_T	*cl = lhs->lhs_type->tt_class;
    पूर्णाङ्क		is_object = lhs->lhs_type->tt_type == VAR_OBJECT;
    char_u	*name = var_start + lhs->lhs_varlen + 1;
    size_t	namelen = lhs->lhs_end - var_start - lhs->lhs_varlen - 1;

    ocmember_T	*m = member_lookup(cl, lhs->lhs_type->tt_type,
	    name, namelen, &lhs->lhs_member_idx);
    जइ (m == रिक्त)
    {
	member_not_found_सन्देशः(cl, lhs->lhs_type->tt_type, name, namelen);
	प्रत्यागम FAIL;
    }

    जइ (IS_ENUM(cl))
    {
	जइ (!inside_class(cctx, cl))
	{
	    semsg(_(e_enumvalue_str_cannot_be_modified),
		    cl->class_name.string, m->ocm_name.string);
	    प्रत्यागम FAIL;
	}
	जइ (lhs->lhs_type->tt_type == VAR_OBJECT &&
		lhs->lhs_member_idx < 2)
	{
	    वर्ण *सन्देशः = lhs->lhs_member_idx == 0 ?
		e_enum_str_name_cannot_be_modified :
		e_enum_str_ordinal_cannot_be_modified;
	    semsg(_(सन्देशः), cl->class_name.string);
	    प्रत्यागम FAIL;
	}
    }

    // If it is private member variable, then accessing it outside the
    // class is not allowed.
    // If it is a read only class variable, then it can be modified
    // only inside the class where it is defined.
    जइ ((m->ocm_access != VIM_ACCESS_ALL) &&
	    ((is_object && !inside_class(cctx, cl))
	     || (!is_object && cctx->ctx_ufunc->uf_defclass != cl)))
    {
	वर्ण *सन्देशः = (m->ocm_access == VIM_ACCESS_PRIVATE)
	    ? e_cannot_access_protected_variable_str
	    : e_variable_is_not_writable_str;
	emsg_var_cl_define(सन्देशः, m->ocm_name.string, 0, cl);
	प्रत्यागम FAIL;
    }

    जइ (!IS_CONSTRUCTOR_METHOD(cctx->ctx_ufunc)
	    && oc_var_check_ro(cl, m))
	प्रत्यागम FAIL;

    lhs->lhs_member_type = m->ocm_type;

    प्रत्यागम OK;
}

/*
 * When compiling a LHS variable, set the LHS variable type.
 */
    स्थिर पूर्णाङ्क
compile_lhs_set_type(cctx_T *cctx, lhs_T *lhs, char_u *var_end, पूर्णाङ्क is_decl)
{
    जइ (is_decl && *skipwhite(var_end) == ':')
    {
	char_u *p;

	// parse optional type: "let var: type = expr"
	जइ (VIM_ISWHITE(*var_end))
	{
	    semsg(_(e_no_white_space_allowed_before_colon_str), var_end);
	    प्रत्यागम FAIL;
	}

	जइ (!VIM_ISWHITE(var_end[1]))
	{
	    semsg(_(e_white_space_required_after_str_str), ":", var_end);
	    प्रत्यागम FAIL;
	}

	p = skipwhite(var_end + 1);
	lhs->lhs_type = parse_type(&p, cctx->ctx_type_list, cctx->ctx_ufunc, cctx, TRUE);
	जइ (lhs->lhs_type == रिक्त
		|| !valid_declaration_type(lhs->lhs_type))
	    प्रत्यागम FAIL;

	lhs->lhs_has_type = TRUE;
	lhs->lhs_end = p;
    }
    अण्णहा जइ (lhs->lhs_lvar != रिक्त)
	lhs->lhs_type = lhs->lhs_lvar->lv_type;

    प्रत्यागम OK;
}

/*
 * Returns TRUE if "lhs" is a concatenable string.
 */
    स्थिर पूर्णाङ्क
lhs_concatenable(lhs_T *lhs)
{
    प्रत्यागम lhs->lhs_dest == dest_global
		|| lhs->lhs_has_index
		|| lhs->lhs_type->tt_type == VAR_STRING
		|| lhs->lhs_type->tt_type == VAR_ANY;
}

/*
 * Create a new local variable when compiling a LHS variable.
 */
    स्थिर पूर्णाङ्क
compile_lhs_new_local_var(
    cctx_T	*cctx,
    lhs_T	*lhs,
    char_u	*var_start,
    पूर्णाङ्क		cmdidx,
    पूर्णाङ्क		oplen,
    पूर्णाङ्क		is_decl,
    पूर्णाङ्क		has_cmd,
    पूर्णाङ्क		heredoc)
{
    जइ (oplen > 1 && !heredoc)
    {
	// +=, /=, etc. require an existing variable
	semsg(_(e_cannot_use_operator_on_new_variable_str), lhs->lhs_name);
	प्रत्यागम FAIL;
    }

    जइ (!is_decl || (lhs->lhs_has_index && !has_cmd
					&& cctx->ctx_skip != SKIP_YES))
    {
	semsg(_(e_unknown_variable_str), lhs->lhs_name);
	प्रत्यागम FAIL;
    }

    // Check the name is valid for a funcref.
    जइ (lhs->lhs_type->tt_type == VAR_FUNC
				|| lhs->lhs_type->tt_type == VAR_PARTIAL)
    {
	जइ (var_wrong_func_name(lhs->lhs_name, TRUE))
	    प्रत्यागम FAIL;
    }

    // New local variable.
    पूर्णाङ्क assign;
    विकल्प (cmdidx)
    {
	पक्ष CMD_final:
	    assign = ASSIGN_FINAL; भङ्ग;
	पक्ष CMD_const:
	    assign = ASSIGN_CONST; भङ्ग;
	स्वभाव:
	    assign = ASSIGN_VAR; भङ्ग;
    }

    lhs->lhs_lvar = reserve_local(cctx, var_start, lhs->lhs_varlen, assign,
							lhs->lhs_type);
    जइ (lhs->lhs_lvar == रिक्त)
	प्रत्यागम FAIL;

    lhs->lhs_new_local = TRUE;

    प्रत्यागम OK;
}

/*
 * When compiling a LHS variable name, set the LHS member type.
 */
    स्थिर पूर्णाङ्क
compile_lhs_set_member_type(
    cctx_T	*cctx,
    lhs_T	*lhs,
    char_u	*var_start,
    पूर्णाङ्क		is_decl,
    पूर्णाङ्क		has_cmd)
{
    lhs->lhs_member_type = lhs->lhs_type;

    जइ (!lhs->lhs_has_index)
	प्रत्यागम OK;

    char_u	*after = var_start + lhs->lhs_varlen;
    char_u	*p;

    // Something follows after the variable: "var[idx]" or "var.key".
    जइ (is_decl && cctx->ctx_skip != SKIP_YES)
    {
	जइ (has_cmd)
	    emsg(_(e_cannot_use_index_when_declaring_variable));
	अण्णहा
	    semsg(_(e_unknown_variable_str), lhs->lhs_name);
	प्रत्यागम FAIL;
    }

    // Now: var_start[lhs->lhs_varlen] is '[' or '.'
    // Only the last index is used below, if there are others
    // before it generate code for the expression.  Thus for
    // "ll[1][2]" the expression is "ll[1]" and "[2]" is the index.
    प्रति (;;)
    {
	p = skip_index(after);
	जइ (*p != '[' && *p != '.')
	{
	    lhs->lhs_varlen_total = p - var_start;
	    भङ्ग;
	}
	after = p;
    }
    जइ (after > var_start + lhs->lhs_varlen)
    {
	lhs->lhs_varlen = after - var_start;
	lhs->lhs_dest = dest_व्यञ्जकम्;
	// We don't know the type before evaluating the expression,
	// use "any" until then.  The member index is for the first name,
	// not for the last index.
	lhs->lhs_type = &t_any;
	lhs->lhs_member_idx = -1;
    }

    पूर्णाङ्क use_class = lhs->lhs_type != रिक्त
	&& (lhs->lhs_type->tt_type == VAR_CLASS
		|| lhs->lhs_type->tt_type == VAR_OBJECT);

    जइ (lhs->lhs_type == रिक्त
	    || (use_class ? lhs->lhs_type->tt_class == रिक्त
		: lhs->lhs_type->tt_member == रिक्त))
    {
	lhs->lhs_member_type = &t_any;
    }
    अण्णहा जइ (use_class)
    {
	// for an object or class member get the type of the member
	जइ (compile_lhs_set_oc_member_type(cctx, lhs, var_start) == FAIL)
	    प्रत्यागम FAIL;
    }
    अण्णहा
	lhs->lhs_member_type = lhs->lhs_type->tt_member;

    प्रत्यागम OK;
}

/*
 * Figure out the LHS type and other properties for an assignment or one item
 * of ":unlet" with an index.
 * Returns OK or FAIL.
 */
    पूर्णाङ्क
compile_lhs(
	char_u	    *var_start,
	lhs_T	    *lhs,
	cmdidx_T    cmdidx,
	पूर्णाङ्क	    heredoc,
	पूर्णाङ्क	    has_cmd,	    // "var" before "var_start"
	पूर्णाङ्क	    oplen,
	cctx_T	    *cctx)
{
    char_u	*var_end = रिक्त;
    पूर्णाङ्क		is_decl = is_decl_command(cmdidx);

    जइ (lhs_init(lhs, var_start, is_decl, heredoc, &var_end) == FAIL)
	प्रत्यागम FAIL;

    जइ (cctx->ctx_skip != SKIP_YES)
    {
	// compile the LHS destination
	जइ (compile_lhs_var_dest(cctx, lhs, cmdidx, var_start, var_end,
						is_decl, has_cmd) == FAIL)
	    प्रत्यागम FAIL;
    }

    // handle "a:name" as a name, not index "name" in "a"
    जइ (lhs->lhs_varlen > 1 || var_start[lhs->lhs_varlen] != ':')
	var_end = lhs->lhs_dest_end;

    जइ (lhs->lhs_dest != dest_option && lhs->lhs_dest != dest_func_option)
    {
	// set the LHS variable type
	जइ (compile_lhs_set_type(cctx, lhs, var_end, is_decl) == FAIL)
	    प्रत्यागम FAIL;
    }

    जइ (oplen == 3 && !heredoc && !lhs_concatenable(lhs))
    {
	emsg(_(e_can_only_concatenate_to_string));
	प्रत्यागम FAIL;
    }

    जइ (lhs->lhs_lvar == रिक्त && lhs->lhs_dest == dest_local
						&& cctx->ctx_skip != SKIP_YES)
    {
	जइ (compile_lhs_new_local_var(cctx, lhs, var_start, cmdidx, oplen,
					is_decl, has_cmd, heredoc) == FAIL)
	    प्रत्यागम FAIL;
    }

    जइ (compile_lhs_set_member_type(cctx, lhs, var_start, is_decl, has_cmd)
								== FAIL)
	प्रत्यागम FAIL;

    प्रत्यागम OK;
}

/*
 * Figure out the LHS and check a few errors.
 */
    पूर्णाङ्क
compile_assign_lhs(
	char_u	    *var_start,
	lhs_T	    *lhs,
	cmdidx_T    cmdidx,
	पूर्णाङ्क	    is_decl,
	पूर्णाङ्क	    heredoc,
	पूर्णाङ्क	    has_cmd,	    // "var" before "var_start"
	पूर्णाङ्क	    oplen,
	cctx_T	    *cctx)
{
    जइ (compile_lhs(var_start, lhs, cmdidx, heredoc, has_cmd, oplen, cctx)
								       == FAIL)
	प्रत्यागम FAIL;

    जइ (!lhs->lhs_has_index && lhs->lhs_lvar == &lhs->lhs_arg_lvar)
    {
	semsg(_(e_cannot_assign_to_argument_str), lhs->lhs_name);
	प्रत्यागम FAIL;
    }
    जइ (!is_decl && lhs->lhs_lvar != रिक्त
			   && lhs->lhs_lvar->lv_const != ASSIGN_VAR
			   && !lhs->lhs_has_index)
    {
	semsg(_(e_cannot_assign_to_constant_str), lhs->lhs_name);
	प्रत्यागम FAIL;
    }
    प्रत्यागम OK;
}

/*
 * Return TRUE if "lhs" has a range index: "[expr : expr]".
 */
    स्थिर पूर्णाङ्क
has_list_index(char_u *idx_start, cctx_T *cctx)
{
    char_u  *p = idx_start;
    पूर्णाङ्क	    save_skip;

    जइ (*p != '[')
	प्रत्यागम FALSE;

    p = skipwhite(p + 1);
    जइ (*p == ':')
	प्रत्यागम TRUE;

    save_skip = cctx->ctx_skip;
    cctx->ctx_skip = SKIP_YES;
    (शून्य)compile_expr0(&p, cctx);
    cctx->ctx_skip = save_skip;
    प्रत्यागम *skipwhite(p) == ':';
}

/*
 * For an assignment with an index, compile the "idx" in "var[idx]" or "key" in
 * "var.key".
 */
    स्थिर पूर्णाङ्क
compile_assign_index(
	char_u	*var_start,
	lhs_T	*lhs,
	पूर्णाङ्क	*range,
	cctx_T	*cctx)
{
    size_t	varlen = lhs->lhs_varlen;
    char_u	*p;
    पूर्णाङ्क		r = OK;
    पूर्णाङ्क		need_white_before = TRUE;
    पूर्णाङ्क		empty_second;

    p = var_start + varlen;
    जइ (*p == '[')
    {
	p = skipwhite(p + 1);
	जइ (*p == ':')
	{
	    // empty first index, push zero
	    r = generate_PUSHNR(cctx, 0);
	    need_white_before = FALSE;
	}
	अण्णहा
	    r = compile_expr0(&p, cctx);

	जइ (r == OK && *skipwhite(p) == ':')
	{
	    // unlet var[idx : idx]
	    // blob[idx : idx] = value
	    *range = TRUE;
	    p = skipwhite(p);
	    empty_second = *skipwhite(p + 1) == ']';
	    जइ ((need_white_before && !IS_WHITE_OR_NUL(p[-1]))
		    || (!empty_second && !IS_WHITE_OR_NUL(p[1])))
	    {
		semsg(_(e_white_space_required_before_and_after_str_at_str),
								      ":", p);
		प्रत्यागम FAIL;
	    }
	    p = skipwhite(p + 1);
	    जइ (*p == ']')
		// empty second index, push "none"
		r = generate_PUSHSPEC(cctx, VVAL_NONE);
	    अण्णहा
		r = compile_expr0(&p, cctx);
	}

	जइ (r == OK && *skipwhite(p) != ']')
	{
	    // this should not happen
	    emsg(_(e_missing_closing_square_brace));
	    r = FAIL;
	}
    }
    अण्णहा जइ (lhs->lhs_member_idx >= 0)
    {
	// object member index
	r = generate_PUSHNR(cctx, lhs->lhs_member_idx);
    }
    अण्णहा // if (*p == '.')
    {
	char_u *कुञ्चिका_end = to_name_end(p + 1, TRUE);
	char_u *कुञ्चिका = vim_strnsave(p + 1, कुञ्चिका_end - p - 1);

	r = generate_PUSHS(cctx, &कुञ्चिका);
    }
    प्रत्यागम r;
}

/*
 * For a LHS with an index, load the variable to be indexed.
 */
    स्थिर पूर्णाङ्क
compile_load_lhs(
	lhs_T	*lhs,
	char_u	*var_start,
	type_T	*rhs_type,
	cctx_T	*cctx)
{
    जइ (lhs->lhs_dest == dest_व्यञ्जकम्)
    {
	size_t	    varlen = lhs->lhs_varlen;
	पूर्णाङ्क	    c = var_start[varlen];
	पूर्णाङ्क	    पङ्क्तयः_len = cctx->ctx_ufunc->uf_पङ्क्तयः.ga_len;
	पूर्णाङ्क	    res;

	// Evaluate "ll[expr]" of "ll[expr][idx]".  End the line with a NUL and
	// limit the lines array length to avoid skipping to a following line.
	var_start[varlen] = NUL;
	cctx->ctx_ufunc->uf_पङ्क्तयः.ga_len = cctx->ctx_lnum + 1;
	char_u *p = var_start;
	res = compile_expr0(&p, cctx);
	var_start[varlen] = c;
	cctx->ctx_ufunc->uf_पङ्क्तयः.ga_len = पङ्क्तयः_len;
	जइ (res == FAIL || p != var_start + varlen)
	{
	    // this should not happen
	    जइ (res != FAIL)
		emsg(_(e_missing_closing_square_brace));
	    प्रत्यागम FAIL;
	}

	lhs->lhs_type = cctx->ctx_type_राशिः.ga_len == 0 ? &t_void
						  : get_type_on_राशिः(cctx, 0);

	जइ (lhs->lhs_type->tt_type == VAR_CLASS
		|| (lhs->lhs_type->tt_type == VAR_OBJECT
		    && lhs->lhs_type != &t_object_any))
	{
	    // Check whether the class or object variable is modifiable
	    जइ (!lhs_class_member_modifiable(lhs, var_start, cctx))
		प्रत्यागम FAIL;
	}

	// Now we can properly check the type.  The variable is indexed, thus
	// we need the member type.  For a class or object we don't know the
	// type yet, it depends on what member is used.
	// The top item in the stack is the Dict, followed by the key and then
	// the type of the value.
	vartype_T vartype = lhs->lhs_type->tt_type;
	type_T *member_type = lhs->lhs_type->tt_member;
	जइ (rhs_type != रिक्त && member_type != रिक्त
		&& vartype != VAR_OBJECT && vartype != VAR_CLASS
		&& rhs_type != &t_void
		&& need_type(rhs_type, member_type, 0,
					    -3, 0, cctx, FALSE, FALSE) == FAIL)
	    प्रत्यागम FAIL;

	प्रत्यागम OK;
    }

    प्रत्यागम generate_loadvar(cctx, lhs);
}

/*
 * Produce code for loading "lhs" and also take care of an index.
 * Return OK/FAIL.
 */
    पूर्णाङ्क
compile_load_lhs_with_index(lhs_T *lhs, char_u *var_start, cctx_T *cctx)
{
    जइ (lhs->lhs_type->tt_type == VAR_OBJECT)
    {
	// "this.value": load "this" object and get the value at index for an
	// object or class member get the type of the member.
	// Also for "obj.value".
	char_u *dot = vim_strchr(var_start, '.');
	जइ (dot == रिक्त)
	{
	    semsg(_(e_missing_dot_after_object_str), lhs->lhs_name);
	    प्रत्यागम FAIL;
	}

	class_T	*cl = lhs->lhs_type->tt_class;
	type_T	*type = oc_member_type(cl, TRUE, dot + 1,
					  lhs->lhs_end, &lhs->lhs_member_idx);
	जइ (lhs->lhs_member_idx < 0)
	    प्रत्यागम FAIL;

	जइ (dot - var_start == 4 && STRNCMP(var_start, "this", 4) == 0)
	{
	    // load "this"
	    lvar_T  *lvar = lhs->lhs_lvar;
	    पूर्णाङ्क	    rc;

	    जइ (lvar->lv_from_outer > 0)
		rc = generate_LOADOUTER(cctx, lvar->lv_idx,
			lvar->lv_from_outer, lvar->lv_loop_depth,
			lvar->lv_loop_idx, type);
	    अण्णहा
		rc = generate_LOAD(cctx, ISN_LOAD, lvar->lv_idx, रिक्त, type);

	    जइ (rc == FAIL)
		प्रत्यागम FAIL;
	}
	अण्णहा
	{
	    // load object variable or argument
	    जइ (compile_load_lhs(lhs, var_start, lhs->lhs_type, cctx) == FAIL)
		प्रत्यागम FAIL;
	}
	जइ (IS_INTERFACE(cl))
	    प्रत्यागम generate_GET_ITF_MEMBER(cctx, cl, lhs->lhs_member_idx, type);
	प्रत्यागम generate_GET_OBJ_MEMBER(cctx, lhs->lhs_member_idx, type);
    }
    अण्णहा जइ (lhs->lhs_type->tt_type == VAR_CLASS)
    {
	// "<classname>.value": load class variable "classname.value"
	char_u *dot = vim_strchr(var_start, '.');
	जइ (dot == रिक्त)
	{
	    (शून्य)check_type_is_मानम्(lhs->lhs_type);
	    प्रत्यागम FAIL;
	}

	class_T	*cl = lhs->lhs_type->tt_class;
	ocmember_T *m = class_member_lookup(cl, dot + 1,
						lhs->lhs_end - dot - 1,
						&lhs->lhs_member_idx);
	जइ (m == रिक्त)
	    प्रत्यागम FAIL;

	प्रत्यागम generate_CLASSMEMBER(cctx, TRUE, cl, lhs->lhs_member_idx);
    }

    जइ (compile_load_lhs(lhs, var_start, रिक्त, cctx) == FAIL)
	प्रत्यागम FAIL;

    जइ (lhs->lhs_has_index)
    {
	पूर्णाङ्क	range = FALSE;
	type_T	*type = get_type_on_राशिः(cctx, 0);

	// A member of an object or class is not obtained by indexing it.
	जइ (type->tt_type == VAR_CLASS
		|| (type->tt_type == VAR_OBJECT && type != &t_object_any))
	{
	    char_u *p = var_start + lhs->lhs_varlen;

	    प्रत्यागम compile_class_object_index(cctx, &p, type);
	}

	// Get member from list or dict.  First compile the
	// index value.
	जइ (compile_assign_index(var_start, lhs, &range, cctx) == FAIL)
	    प्रत्यागम FAIL;
	जइ (range)
	{
	    semsg(_(e_cannot_use_range_with_assignment_operator_str),
								    var_start);
	    प्रत्यागम FAIL;
	}

	// Get the member.
	जइ (compile_member(FALSE, रिक्त, cctx) == FAIL)
	    प्रत्यागम FAIL;
    }
    प्रत्यागम OK;
}

/*
 * Assignment to a list or dict member, or ":unlet" for the item, using the
 * information in "lhs".
 * Returns OK or FAIL.
 */
    पूर्णाङ्क
compile_assign_unlet(
    char_u	*var_start,
    lhs_T	*lhs,
    पूर्णाङ्क		is_assign,
    type_T	*rhs_type,
    cctx_T	*cctx)
{
    vartype_T	dest_type;
    पूर्णाङ्क		range = FALSE;

    जइ (compile_assign_index(var_start, lhs, &range, cctx) == FAIL)
	प्रत्यागम FAIL;
    जइ (is_assign && range
	    && lhs->lhs_type->tt_type != VAR_LIST
	    && lhs->lhs_type != &t_blob
	    && lhs->lhs_type != &t_any)
    {
	जइ (lhs->lhs_type->tt_type == VAR_TUPLE)
	    emsg(_(e_cannot_slice_tuple));
	अण्णहा
	    semsg(_(e_cannot_use_range_with_assignment_str), var_start);
	प्रत्यागम FAIL;
    }

    // For "expr[idx]" the index is compiled before the expression, thus the
    // resulting type cannot be used here.
    जइ (lhs->lhs_dest == dest_व्यञ्जकम्
	    || lhs->lhs_type == रिक्त || lhs->lhs_type == &t_any)
    {
	// Index on variable of unknown type: check at runtime.
	dest_type = VAR_ANY;
    }
    अण्णहा
    {
	dest_type = lhs->lhs_type->tt_type;
	जइ (dest_type == VAR_DICT && range)
	{
	    emsg(_(e_cannot_use_range_with_dictionary));
	    प्रत्यागम FAIL;
	}
	जइ (dest_type == VAR_DICT
		&& may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
	    प्रत्यागम FAIL;
	जइ (dest_type == VAR_LIST || dest_type == VAR_BLOB)
	{
	    type_T *type;

	    जइ (range)
	    {
		type = get_type_on_राशिः(cctx, 1);
		जइ (need_type(type, &t_सङ्ख्या, 0,
					    -2, 0, cctx, FALSE, FALSE) == FAIL)
		प्रत्यागम FAIL;
	    }
	    type = get_type_on_राशिः(cctx, 0);
	    जइ ((dest_type != VAR_BLOB && type->tt_type != VAR_SPECIAL)
		    && need_type(type, &t_सङ्ख्या, 0,
					    -1, 0, cctx, FALSE, FALSE) == FAIL)
		प्रत्यागम FAIL;
	}
    }

    जइ (cctx->ctx_skip == SKIP_YES)
	प्रत्यागम OK;

    // Load the dict, list or object.  On the stack we then have:
    // - value (for assignment, not for :unlet)
    // - index
    // - for [a : b] second index
    // - variable
    जइ (compile_load_lhs(lhs, var_start, rhs_type, cctx) == FAIL)
	प्रत्यागम FAIL;

    जइ (dest_type == VAR_LIST
	    || dest_type == VAR_DICT
	    || dest_type == VAR_BLOB
	    || dest_type == VAR_CLASS
	    || dest_type == VAR_OBJECT
	    || dest_type == VAR_ANY)
    {
	जइ (is_assign)
	{
	    जइ (range)
	    {
		जइ (generate_instr_drop(cctx, ISN_STORERANGE, 4) == रिक्त)
		    प्रत्यागम FAIL;
	    }
	    अण्णहा
	    {
		isn_T	*isn = generate_instr_drop(cctx, ISN_STOREINDEX, 3);

		जइ (isn == रिक्त)
		    प्रत्यागम FAIL;
		isn->isn_arg.storeindex.si_vartype = dest_type;
		isn->isn_arg.storeindex.si_class = रिक्त;

		जइ (dest_type == VAR_OBJECT)
		{
		    class_T *cl = lhs->lhs_type->tt_class;

		    जइ (IS_INTERFACE(cl))
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
	अण्णहा जइ (range)
	{
	    जइ (generate_instr_drop(cctx, ISN_UNLETRANGE, 3) == रिक्त)
		प्रत्यागम FAIL;
	}
	अण्णहा
	{
	    जइ (generate_instr_drop(cctx, ISN_UNLETINDEX, 2) == रिक्त)
		प्रत्यागम FAIL;
	}
    }
    अण्णहा
    {
	जइ (dest_type == VAR_TUPLE)
	    emsg(_(e_tuple_is_immutable));
	अण्णहा
	    emsg(_(e_indexable_type_required));
	प्रत्यागम FAIL;
    }

    प्रत्यागम OK;
}

/*
 * Generate an instruction to push the default value for "vartype".
 * if "dest_local" is TRUE then for some types no instruction is generated.
 * "skip_store" is set to TRUE if no PUSH instruction is generated.
 * Returns OK or FAIL.
 */
    स्थिर पूर्णाङ्क
प्रक्षिप_default_मानम्(
	cctx_T	    *cctx,
	vartype_T   vartype,
	पूर्णाङ्क	    dest_is_local,
	पूर्णाङ्क	    *skip_store)
{
    पूर्णाङ्क r = OK;

    विकल्प (vartype)
    {
	पक्ष VAR_BOOL:
	    r = generate_PUSHBOOL(cctx, VVAL_FALSE);
	    भङ्ग;
	पक्ष VAR_FLOAT:
	    r = generate_PUSHF(cctx, 0.0);
	    भङ्ग;
	पक्ष VAR_STRING:
	    r = generate_PUSHS(cctx, रिक्त);
	    भङ्ग;
	पक्ष VAR_BLOB:
	    r = generate_PUSHBLOB(cctx, blob_alloc());
	    भङ्ग;
	पक्ष VAR_FUNC:
	    r = generate_PUSHFUNC(cctx, रिक्त, &t_func_void, TRUE);
	    भङ्ग;
	पक्ष VAR_LIST:
	    r = generate_NEWLIST(cctx, 0, FALSE);
	    भङ्ग;
	पक्ष VAR_TUPLE:
	    r = generate_NEWTUPLE(cctx, 0, FALSE);
	    भङ्ग;
	पक्ष VAR_DICT:
	    r = generate_NEWDICT(cctx, 0, FALSE);
	    भङ्ग;
	पक्ष VAR_JOB:
	    r = generate_PUSHJOB(cctx);
	    भङ्ग;
	पक्ष VAR_CHANNEL:
	    r = generate_PUSHCHANNEL(cctx);
	    भङ्ग;
	पक्ष VAR_OBJECT:
	    r = generate_PUSHOBJ(cctx);
	    भङ्ग;
	पक्ष VAR_सङ्ख्या:
	पक्ष VAR_UNKNOWN:
	पक्ष VAR_ANY:
	पक्ष VAR_PARTIAL:
	पक्ष VAR_VOID:
	पक्ष VAR_INSTR:
	पक्ष VAR_CLASS:
	पक्ष VAR_TYPEALIAS:
	पक्ष VAR_SPECIAL:  // cannot happen
	    // This is skipped for local variables, they are always
	    // initialized to zero.  But in a "for" or "while" loop
	    // the value may have been changed.
	    जइ (dest_is_local && !inside_loop_scope(cctx))
		*skip_store = TRUE;
	    अण्णहा
		r = generate_PUSHNR(cctx, 0);
	    भङ्ग;
    }
    प्रत्यागम r;
}

/*
 * Compile assignment context.  Used when compiling an assignment statement.
 */
प्रकारनिर्देश संरचना cac_S cac_T;
संरचना cac_S
{
    cmdidx_T	cac_cmdidx;		// assignment command
    char_u	*cac_nextc;		// next character to parse
    lhs_T	cac_lhs;		// lhs of the assignment
    type_T	*cac_rhs_type;		// rhs type of an assignment
    char_u	*cac_op;		// assignment operator
    पूर्णाङ्क		cac_oplen;		// assignment operator length
    char_u	*cac_var_start;		// start of the variable names
    char_u	*cac_var_end;		// end of the variable names
    पूर्णाङ्क		cac_var_गणनम्;		// number of variables in assignment
    पूर्णाङ्क		cac_var_idx;		// variable index in a list
    पूर्णाङ्क		cac_semicolon;		// semicolon in [var1, var2; var3]
    garray_T	*cac_instr;
    पूर्णाङ्क		cac_instr_गणनम्;
    पूर्णाङ्क		cac_incdec;
    पूर्णाङ्क		cac_did_generate_slice;
    पूर्णाङ्क		cac_is_decl;
    पूर्णाङ्क		cac_is_const;
    पूर्णाङ्क		cac_start_lnum;
    type_T	*cac_inferred_type;
    पूर्णाङ्क		cac_skip_store;
};

/*
 * Initialize the compile assignment context.
 */
    स्थिर शून्य
compile_assign_context_init(cac_T *cac, cctx_T *cctx, पूर्णाङ्क cmdidx, char_u *arg)
{
    CLEAR_FIELD(*cac);
    cac->cac_cmdidx = cmdidx;
    cac->cac_instr = &cctx->ctx_instr;
    cac->cac_rhs_type = &t_any;
    cac->cac_is_decl = is_decl_command(cmdidx);
    cac->cac_start_lnum = SOURCING_LNUM;
    cac->cac_instr_गणनम् = -1;
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
    स्थिर पूर्णाङ्क
compile_assign_obj_new_arg(char_u **argp, cctx_T *cctx)
{
    char_u *arg = *argp;

    arg += 11;	    // skip "ifargisset"
    पूर्णाङ्क def_arg_idx = getdigits(&arg);
    arg = skipwhite(arg);

    // Use a JUMP_IF_ARG_NOT_SET instruction to skip if the value was not
    // given and the default value is "v:none".
    पूर्णाङ्क राशिः_offset = राशिः_FRAME_SIZE +
				(cctx->ctx_ufunc->uf_va_name != रिक्त ? 1 : 0);
    पूर्णाङ्क def_arg_गणनम् = cctx->ctx_ufunc->uf_def_args.ga_len;
    पूर्णाङ्क arg_offset = def_arg_idx - def_arg_गणनम् - राशिः_offset;

    जइ (generate_JUMP_IF_ARG(cctx, ISN_JUMP_IF_ARG_NOT_SET,
			     arg_offset) == FAIL)
	प्रत्यागम FAIL;

    *argp = arg;
    प्रत्यागम OK;
}

/*
 * Translate the increment (++) and decrement (--) operators to the
 * corresponding compound operators (+= or -=).
 *
 * Returns OK on success and FAIL on syntax error.
 */
    स्थिर पूर्णाङ्क
translate_incdec_op(exarg_T *eap, cac_T *cac)
{
    जइ (VIM_ISWHITE(eap->cmd[2]))
    {
	semsg(_(e_no_white_space_allowed_after_str_str),
		eap->cmdidx == CMD_increment ? "++" : "--", eap->cmd);
	प्रत्यागम FAIL;
    }
    cac->cac_op = (char_u *)(eap->cmdidx == CMD_increment ? "+=" : "-=");
    cac->cac_oplen = 2;
    cac->cac_incdec = TRUE;

    प्रत्यागम OK;
}

/*
 * Process the operator in an assignment statement.
 */
    स्थिर पूर्णाङ्क
compile_assign_process_operator(
    exarg_T	*eap,
    char_u	*arg,
    cac_T	*cac,
    पूर्णाङ्क		*heredoc,
    char_u	**retstr)
{
    *retstr = रिक्त;

    जइ (eap->cmdidx == CMD_increment || eap->cmdidx == CMD_decrement)
	// Change an unary operator to a compound operator
	प्रत्यागम translate_incdec_op(eap, cac);

    char_u *sp = cac->cac_nextc;
    cac->cac_nextc = skipwhite(cac->cac_nextc);
    cac->cac_op = cac->cac_nextc;
    cac->cac_oplen = assignment_len(cac->cac_nextc, heredoc);

    जइ (cac->cac_var_गणनम् > 0 && cac->cac_oplen == 0)
    {
	// can be something like "[1, 2]->func()"
	*retstr = arg;
	प्रत्यागम FAIL;
    }

    // need white space before and after the operator
    जइ (cac->cac_oplen > 0 && (!VIM_ISWHITE(*sp)
		|| !IS_WHITE_OR_NUL(cac->cac_op[cac->cac_oplen])))
    {
	error_white_both(cac->cac_op, cac->cac_oplen);
	प्रत्यागम FAIL;
    }

    प्रत्यागम OK;
}

/*
 * Find the start of an assignment statement.
 */
    स्थिर char_u *
compile_assign_compute_start(char_u *arg, पूर्णाङ्क var_गणनम्)
{
    जइ (var_गणनम् > 0)
	// [var1, var2] = [val1, val2]
	// skip over the "["
	प्रत्यागम skipwhite(arg + 1);

    प्रत्यागम arg;
}

/*
 * Parse a heredoc assignment starting at "p".  Returns a pointer to the
 * beginning of the heredoc content.
 */
    स्थिर char_u *
parse_heredoc_assignment(exarg_T *eap, cctx_T *cctx, cac_T *cac)
{
    // [let] varname =<< [trim] {end}
    eap->ea_getline = exarg_getline;
    eap->cookie = cctx;

    list_T *l = heredoc_get(eap, cac->cac_nextc + 3, FALSE, TRUE);
    जइ (l == रिक्त)
	प्रत्यागम रिक्त;

    list_free(l);
    cac->cac_nextc += STRLEN(cac->cac_nextc);

    प्रत्यागम cac->cac_nextc;
}

/*
 * Check the type of a RHS expression in a list assignment statement.
 * The RHS expression is already compiled.  So the type is on the stack.
 */
    स्थिर पूर्णाङ्क
compile_assign_list_check_rhs_type(cctx_T *cctx, cac_T *cac)
{
    type_T	*stacktype;

    stacktype = cctx->ctx_type_राशिः.ga_len == 0 ? &t_void
						: get_type_on_राशिः(cctx, 0);
    जइ (stacktype->tt_type == VAR_VOID)
    {
	emsg(_(e_cannot_use_void_मानम्));
	प्रत्यागम FAIL;
    }

    जइ (stacktype->tt_type != VAR_LIST && stacktype->tt_type != VAR_TUPLE
					&& stacktype->tt_type != VAR_ANY)
    {
	emsg(_(e_list_or_tuple_required));
	प्रत्यागम FAIL;
    }

    जइ (need_type(stacktype,
		  stacktype->tt_type == VAR_TUPLE ? &t_tuple_any : &t_list_any,
		  TYPECHK_TUPLE_OK, -1, 0, cctx, FALSE, FALSE) == FAIL)
	प्रत्यागम FAIL;

    // The check accepts both a list and a tuple. Keep "any", making it a list
    // would reject a tuple later on.
    जइ (stacktype->tt_type == VAR_ANY)
	set_type_on_राशिः(cctx, &t_any, 0);

    जइ (stacktype->tt_type == VAR_TUPLE)
    {
	जइ (stacktype->tt_argcount != 1)
	    cac->cac_rhs_type = &t_any;
	अण्णहा
	{
	    जइ (stacktype->tt_flags & TTFLAG_VARARGS)
		cac->cac_rhs_type = stacktype->tt_args[0]->tt_member;
	    अण्णहा
		cac->cac_rhs_type = stacktype->tt_args[0];
	}
    }
    अण्णहा जइ (stacktype->tt_member != रिक्त)
	cac->cac_rhs_type = stacktype->tt_member;

    प्रत्यागम OK;
}

/*
 * In a list assignment statement, if a constant list was used, check the
 * length.  Returns OK if the length check succeeds.  Returns FAIL otherwise.
 */
    स्थिर पूर्णाङ्क
compile_assign_list_check_length(cctx_T *cctx, cac_T *cac)
{
    पूर्णाङ्क	needed_list_len;
    पूर्णाङ्क	did_check = FALSE;

    needed_list_len = cac->cac_semicolon
				? cac->cac_var_गणनम् - 1
				: cac->cac_var_गणनम्;
    जइ (cac->cac_instr->ga_len > 0)
    {
	isn_T	*isn = ((isn_T *)cac->cac_instr->ga_data) +
						cac->cac_instr->ga_len - 1;

	जइ (isn->isn_type == ISN_NEWLIST || isn->isn_type == ISN_NEWTUPLE)
	{
	    did_check = TRUE;
	    जइ (cac->cac_semicolon ?
			isn->isn_arg.सङ्ख्या < needed_list_len
			: isn->isn_arg.सङ्ख्या != needed_list_len)
	    {
		semsg(_(e_expected_nr_वस्तूनि_but_got_nr),
			needed_list_len, (पूर्णाङ्क)isn->isn_arg.सङ्ख्या);
		प्रत्यागम FAIL;
	    }
	}
    }

    जइ (!did_check)
	generate_CHECKLEN(cctx, needed_list_len, cac->cac_semicolon);

    प्रत्यागम OK;
}

/*
 * Evaluate the expression for "[var, var] = expr" assignment.
 * A line break may follow the assignment operator "=".
 */
    स्थिर char_u *
compile_assign_list_व्यञ्जकम्(cctx_T *cctx, cac_T *cac)
{
    char_u *whitep;

    whitep = cac->cac_op + cac->cac_oplen;

    जइ (may_get_next_पङ्क्तिः_error(whitep, &cac->cac_nextc, cctx) == FAIL)
	प्रत्यागम रिक्त;

    // compile RHS expression
    जइ (compile_expr0(&cac->cac_nextc, cctx) == FAIL)
	प्रत्यागम रिक्त;

    जइ (cctx->ctx_skip == SKIP_YES)
	// no need to parse more when skipping
	प्रत्यागम cac->cac_nextc;

    जइ (compile_assign_list_check_rhs_type(cctx, cac) == FAIL)
	प्रत्यागम रिक्त;

    // If a constant list was used we can check the length right here.
    जइ (compile_assign_list_check_length(cctx, cac) == FAIL)
	प्रत्यागम FAIL;

    प्रत्यागम cac->cac_nextc;
}

/*
 * Find and return the end of a heredoc or a list of variables assignment
 * statement.  For a single variable assignment statement, returns the current
 * end.
 * Returns NULL on failure.
 */
    स्थिर char_u *
compile_assign_compute_end(
    exarg_T	*eap,
    cctx_T	*cctx,
    cac_T	*cac,
    पूर्णाङ्क		heredoc)
{
    जइ (heredoc)
    {
	cac->cac_nextc = parse_heredoc_assignment(eap, cctx, cac);
	प्रत्यागम cac->cac_nextc;
    }

    जइ (cac->cac_var_गणनम् > 0)
    {
	// for "[var, var] = expr" evaluate the expression. The list of
	// variables are processed later.
	// A line break may follow the "=".
	cac->cac_nextc = compile_assign_list_व्यञ्जकम्(cctx, cac);
	प्रत्यागम cac->cac_nextc;
    }

    प्रत्यागम cac->cac_var_end;
}

/*
 * For "var = expr" evaluate the expression.
 */
    स्थिर पूर्णाङ्क
compile_assign_single_eval_व्यञ्जकम्(cctx_T *cctx, cac_T *cac)
{
    पूर्णाङ्क		ret = OK;
    char_u	*whitep;
    lhs_T	*lhs = &cac->cac_lhs;
    lvar_T	*lvp;
    lvar_T	save_lhs_lvar;

    // Compile the expression.
    जइ (cac->cac_incdec)
	प्रत्यागम generate_PUSHNR(cctx, 1);

    // Temporarily hide the new local variable here, it is
    // not available to this expression.
    जइ (lhs->lhs_new_local)
    {
	--cctx->ctx_locals.ga_len;

	// Save the local variable value (compiling the RHS expression may
	// create new local variables).
	lvp = ((lvar_T *)cctx->ctx_locals.ga_data) + cctx->ctx_locals.ga_len;
	save_lhs_lvar = *lvp;
    }
    whitep = cac->cac_op + cac->cac_oplen;

    जइ (may_get_next_पङ्क्तिः_error(whitep, &cac->cac_nextc, cctx) == FAIL)
    {
	जइ (lhs->lhs_new_local)
	    ++cctx->ctx_locals.ga_len;
	प्रत्यागम FAIL;
    }

    ret = compile_expr0_ext(&cac->cac_nextc, cctx, &cac->cac_is_const);
    जइ (lhs->lhs_new_local)
    {
	// Restore the local variable value.  Update lhs_lvar as the index of
	// the local variable might have changed.
	lvp = ((lvar_T *)cctx->ctx_locals.ga_data) + cctx->ctx_locals.ga_len;
	*lvp = save_lhs_lvar;
	lhs->lhs_lvar = lvp;

	++cctx->ctx_locals.ga_len;
    }

    प्रत्यागम ret;
}

/*
 * When compiling an assignment, set the LHS type to the RHS type.
 */
    स्थिर पूर्णाङ्क
compile_assign_set_lhs_type_from_rhs(
    cctx_T	*cctx,
    cac_T	*cac,
    lhs_T	*lhs,
    type_T	*rhs_type)
{
    जइ (rhs_type->tt_type == VAR_VOID)
    {
	emsg(_(e_cannot_use_void_मानम्));
	प्रत्यागम FAIL;
    }

    type_T *type;

    // An empty list or dict has a &t_unknown member, for a variable that
    // implies &t_any.
    जइ (rhs_type == &t_list_empty)
	type = &t_list_any;
    अण्णहा जइ (rhs_type == &t_dict_empty)
	type = &t_dict_any;
    अण्णहा जइ (rhs_type == &t_unknown)
	type = &t_any;
    अण्णहा
    {
	type = rhs_type;
	cac->cac_inferred_type = rhs_type;
    }

    set_var_type(lhs->lhs_lvar, type, cctx);

    प्रत्यागम OK;
}

/*
 * Returns TRUE if the "rhs_type" can be assigned to the "lhs" variable.
 * Used when compiling an assignment statement.
 */
    स्थिर पूर्णाङ्क
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
    जइ (cac->cac_var_गणनम् > 0)
    {
	where.wt_index = cac->cac_var_idx + 1;
	where.wt_kind = WT_VARIABLE;
    }

    // If assigning to a list or dict member, use the member type.
    // Not for "list[:] =".
    जइ (lhs->lhs_has_index &&
	    !has_list_index(cac->cac_var_start + lhs->lhs_varlen, cctx))
	use_type = lhs->lhs_member_type;

    जइ (need_type_where(rhs_type, use_type, 0, -1, where, cctx, FALSE,
						cac->cac_is_const) == FAIL)
	प्रत्यागम FALSE;

    प्रत्यागम TRUE;
}

/*
 * Compare the LHS type with the RHS type in an assignment.
 */
    स्थिर पूर्णाङ्क
compile_assign_check_type(cctx_T *cctx, cac_T *cac)
{
    lhs_T	*lhs = &cac->cac_lhs;
    type_T	*rhs_type;

    rhs_type = cctx->ctx_type_राशिः.ga_len == 0
					? &t_void
					: get_type_on_राशिः(cctx, 0);
    cac->cac_rhs_type = rhs_type;

    जइ (check_type_is_मानम्(rhs_type) == FAIL)
	प्रत्यागम FAIL;

    जइ (lhs->lhs_lvar != रिक्त && (cac->cac_is_decl || !lhs->lhs_has_type))
    {
	जइ (rhs_type->tt_type == VAR_FUNC
					|| rhs_type->tt_type == VAR_PARTIAL)
	{
	    // Make sure the variable name can be used as a funcref
	    जइ (!lhs->lhs_has_index
				&& var_wrong_func_name(lhs->lhs_name, TRUE))
		प्रत्यागम FAIL;
	}

	जइ (lhs->lhs_new_local && !lhs->lhs_has_type)
	{
	    // The LHS variable doesn't have a type.  Set it to the RHS type.
	    जइ (compile_assign_set_lhs_type_from_rhs(cctx, cac, lhs, rhs_type)
								== FAIL)
		प्रत्यागम FAIL;
	}
	अण्णहा जइ (*cac->cac_op == '=')
	{
	    जइ (!compile_assign_valid_rhs_type(cctx, cac, lhs, rhs_type))
		प्रत्यागम FAIL;
	}
    }
    अण्णहा
    {
	// Assigning to a register using @r = "abc"

	type_T *lhs_type = lhs->lhs_member_type;

	// Special case: assigning to @# can use a number or a string.
	// Also: can assign a number to a float.
	जइ ((lhs_type == &t_सङ्ख्या_or_string || lhs_type == &t_float)
					&& rhs_type->tt_type == VAR_सङ्ख्या)
	    lhs_type = &t_सङ्ख्या;

	जइ (*cac->cac_nextc != '=')
	{
	    जइ (need_type(rhs_type, lhs_type, 0, -1, 0, cctx, FALSE,
							FALSE) == FAIL)
		प्रत्यागम FAIL;
	}
    }

    प्रत्यागम OK;
}

/*
 * Compile the RHS expression in an assignment statement and generate the
 * instructions.
 */
    स्थिर पूर्णाङ्क
compile_assign_rhs_व्यञ्जकम्(cctx_T *cctx, cac_T *cac)
{
    cac->cac_is_const = FALSE;

    // for "+=", "*=", "..=" etc. first load the current value
    जइ (*cac->cac_op != '='
	    && compile_load_lhs_with_index(&cac->cac_lhs, cac->cac_var_start,
								cctx) == FAIL)
	प्रत्यागम FAIL;

    // For "var = expr" evaluate the expression.
    जइ (cac->cac_var_गणनम् == 0)
    {
	पूर्णाङ्क	ret;

	// Compile the expression.
	cac->cac_instr_गणनम् = cac->cac_instr->ga_len;
	ret = compile_assign_single_eval_व्यञ्जकम्(cctx, cac);
	जइ (ret == FAIL)
	    प्रत्यागम FAIL;
    }
    अण्णहा जइ (cac->cac_semicolon && cac->cac_var_idx == cac->cac_var_गणनम् - 1)
    {
	// For "[var; var] = expr" get the rest of the list
	cac->cac_did_generate_slice = TRUE;
	जइ (generate_SLICE(cctx, cac->cac_var_गणनम् - 1) == FAIL)
	    प्रत्यागम FAIL;
    }
    अण्णहा
    {
	// For "[var, var] = expr" get the "var_idx" item from the
	// list.
	पूर्णाङ्क with_op = *cac->cac_op != '=';
	जइ (generate_GETITEM(cctx, cac->cac_var_idx, with_op) == FAIL)
	    प्रत्यागम FAIL;
    }

    जइ (compile_assign_check_type(cctx, cac) == FAIL)
	प्रत्यागम FAIL;

    प्रत्यागम OK;
}

/*
 * Compile the RHS expression in an assignment
 */
    स्थिर पूर्णाङ्क
compile_assign_rhs(cctx_T *cctx, cac_T *cac)
{
    lhs_T	*lhs = &cac->cac_lhs;

    जइ (cctx->ctx_skip == SKIP_YES)
    {
	जइ (cac->cac_oplen > 0 && cac->cac_var_गणनम् == 0)
	{
	    // skip over the "=" and the expression
	    cac->cac_nextc = skipwhite(cac->cac_op + cac->cac_oplen);
	    (शून्य)compile_expr0(&cac->cac_nextc, cctx);
	}
	प्रत्यागम OK;
    }

    // If RHS is specified, then generate instructions for RHS expression
    जइ (cac->cac_oplen > 0)
	प्रत्यागम compile_assign_rhs_व्यञ्जकम्(cctx, cac);

    जइ (cac->cac_cmdidx == CMD_final)
    {
	emsg(_(e_final_requires_a_मानम्));
	प्रत्यागम FAIL;
    }

    जइ (cac->cac_cmdidx == CMD_const)
    {
	emsg(_(e_const_requires_a_मानम्));
	प्रत्यागम FAIL;
    }

    जइ (!lhs->lhs_has_type || lhs->lhs_dest == dest_option
					|| lhs->lhs_dest == dest_func_option)
    {
	emsg(_(e_type_or_initialization_required));
	प्रत्यागम FAIL;
    }

    // variables are always initialized
    जइ (GA_GROW_FAILS(cac->cac_instr, 1))
	प्रत्यागम FAIL;

    cac->cac_instr_गणनम् = cac->cac_instr->ga_len;

    प्रत्यागम प्रक्षिप_default_मानम्(cctx, lhs->lhs_member_type->tt_type,
			      lhs->lhs_dest == dest_local,
			      &cac->cac_skip_store);
}

/*
 * Returns OK if "type" supports compound operator "op_arg" (e.g. +=, -=, %=,
 * etc.).  Compound operators are not supported with a tuple and a dict.
 * Returns FAIL if compound operator is not supported.
 */
    स्थिर पूर्णाङ्क
check_type_supports_compound_op(type_T *type, char_u op_arg)
{
    जइ (type->tt_type == VAR_TUPLE || type->tt_type == VAR_DICT)
    {
	char_u	op[2];

	op[0] = op_arg;
	op[1] = NUL;
	semsg(_(e_wrong_variable_type_for_str_equal), op);
	प्रत्यागम FAIL;
    }

    प्रत्यागम OK;
}

/*
 * Compile a compound op assignment statement (+=, -=, *=, %=, etc.)
 */
    स्थिर पूर्णाङ्क
compile_assign_compound_op(cctx_T *cctx, cac_T *cac)
{
    lhs_T	    *lhs = &cac->cac_lhs;
    type_T	    *expected;
    type_T	    *stacktype = रिक्त;

    जइ (cac->cac_lhs.lhs_type->tt_type == VAR_TUPLE
	    && check_type_supports_compound_op(cac->cac_lhs.lhs_type,
							*cac->cac_op) == FAIL)
	प्रत्यागम FAIL;

    जइ (*cac->cac_op == '.')
    {
	expected = lhs->lhs_member_type;
	stacktype = get_type_on_राशिः(cctx, 0);
	जइ (expected != &t_string
		&& need_type(stacktype, expected, 0, -1, 0, cctx,
					FALSE, FALSE) == FAIL)
	    प्रत्यागम FAIL;
	अण्णहा जइ (may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
	    प्रत्यागम FAIL;
    }
    अण्णहा
    {
	expected = lhs->lhs_member_type;
	stacktype = get_type_on_राशिः(cctx, 0);

	जइ (check_type_supports_compound_op(expected, *cac->cac_op) == FAIL)
	    प्रत्यागम FAIL;

	जइ (
		// If variable is float operation with number is OK.
		!(expected == &t_float && (stacktype == &t_सङ्ख्या
					|| stacktype == &t_सङ्ख्या_bool))
		&& need_type(stacktype, expected, TYPECHK_सङ्ख्या_OK, -1, 0,
						cctx, FALSE, FALSE) == FAIL)
	    प्रत्यागम FAIL;
    }

    जइ (*cac->cac_op == '.')
    {
	जइ (generate_CONCAT(cctx, 2) == FAIL)
	    प्रत्यागम FAIL;
    }
    अण्णहा जइ (*cac->cac_op == '+')
    {
	जइ (generate_add_instr(cctx,
		    operator_type(lhs->lhs_member_type, stacktype),
		    lhs->lhs_member_type, stacktype,
		    व्यञ्जकम्_APPEND) == FAIL)
	    प्रत्यागम FAIL;
    }
    अण्णहा जइ (generate_two_op(cctx, cac->cac_op) == FAIL)
	प्रत्यागम FAIL;

    प्रत्यागम OK;
}

/*
 * Generate the STORE and SETTYPE instructions for an assignment statement.
 */
    स्थिर पूर्णाङ्क
compile_assign_generate_store(cctx_T *cctx, cac_T *cac)
{
    lhs_T	*lhs = &cac->cac_lhs;
    पूर्णाङ्क		save_lnum;

    // Use the line number of the assignment for store instruction.
    save_lnum = cctx->ctx_lnum;
    cctx->ctx_lnum = cac->cac_start_lnum - 1;

    जइ (lhs->lhs_has_index)
    {
	// Use the info in "lhs" to store the value at the index in the
	// list, dict or object.
	जइ (compile_assign_unlet(cac->cac_var_start, &cac->cac_lhs,
				 TRUE, cac->cac_rhs_type, cctx) == FAIL)
	{
	    cctx->ctx_lnum = save_lnum;
	    प्रत्यागम FAIL;
	}
    }
    अण्णहा
    {
	जइ (cac->cac_is_decl && cac->cac_cmdidx == CMD_const &&
			(lhs->lhs_dest == dest_script
			 || lhs->lhs_dest == dest_script_v9
			 || lhs->lhs_dest == dest_global
			 || lhs->lhs_dest == dest_local))
	    // ":const var": lock the value, but not referenced variables
	    generate_LOCKCONST(cctx);

	type_T	*inferred_type = cac->cac_inferred_type;

	जइ ((lhs->lhs_type->tt_type == VAR_DICT
		    || lhs->lhs_type->tt_type == VAR_LIST)
		&& lhs->lhs_type->tt_member != रिक्त
		&& lhs->lhs_type->tt_member != &t_any
		&& lhs->lhs_type->tt_member != &t_unknown)
	    // Set the type in the list or dict, so that it can be
	    // checked, also in legacy script.
	    generate_SETTYPE(cctx, lhs->lhs_type);
	अण्णहा जइ (lhs->lhs_type->tt_type == VAR_TUPLE
					&& lhs->lhs_type->tt_argcount != 0)
	    generate_SETTYPE(cctx, lhs->lhs_type);
	अण्णहा जइ (inferred_type != रिक्त
		&& (inferred_type->tt_type == VAR_DICT
		    || inferred_type->tt_type == VAR_LIST)
		&& inferred_type->tt_member != रिक्त
		&& inferred_type->tt_member != &t_unknown
		&& inferred_type->tt_member != &t_any)
	    // Set the type in the list or dict, so that it can be
	    // checked, also in legacy script.
	    generate_SETTYPE(cctx, inferred_type);
	अण्णहा जइ (inferred_type != रिक्त
				&& inferred_type->tt_type == VAR_TUPLE
				&& inferred_type->tt_argcount > 0)
	    generate_SETTYPE(cctx, inferred_type);

	जइ (!cac->cac_skip_store &&
		generate_store_lhs(cctx, &cac->cac_lhs,
				   cac->cac_instr_गणनम्,
				   cac->cac_is_decl) == FAIL)
	{
	    cctx->ctx_lnum = save_lnum;
	    प्रत्यागम FAIL;
	}
    }

    cctx->ctx_lnum = save_lnum;

    प्रत्यागम OK;
}

/*
 * Process the variable(s) in an assignment statement
 */
    स्थिर पूर्णाङ्क
compile_assign_process_variables(
    cctx_T	*cctx,
    cac_T	*cac,
    पूर्णाङ्क		cmdidx,
    पूर्णाङ्क		heredoc,
    पूर्णाङ्क		has_cmd,
    पूर्णाङ्क		has_argisset_prefix,
    पूर्णाङ्क		jump_instr_idx)
{
    /*
     * Loop over variables in "[var, var] = expr".
     * For "name = expr" and "var name: type" this is done only once.
     */
    प्रति (cac->cac_var_idx = 0; cac->cac_var_idx == 0 ||
	    cac->cac_var_idx < cac->cac_var_गणनम्; cac->cac_var_idx++)
    {
	जइ (cac->cac_var_start[0] == '_'
				&& !eval_isnamec(cac->cac_var_start[1]))
	{
	    // Ignore underscore in "[a, _, b] = list".
	    जइ (cac->cac_var_गणनम् > 0)
	    {
		cac->cac_var_start = skipwhite(cac->cac_var_start + 2);
		अनुवर्त;
	    }
	    emsg(_(e_cannot_use_underscore_here));
	    प्रत्यागम FAIL;
	}
	vim_free(cac->cac_lhs.lhs_name);

	/*
	 * Figure out the LHS type and other properties.
	 */
	जइ (compile_assign_lhs(cac->cac_var_start, &cac->cac_lhs, cmdidx,
		    cac->cac_is_decl, heredoc, has_cmd,
		    cac->cac_oplen, cctx) == FAIL)
	    प्रत्यागम FAIL;

	// Compile the RHS expression
	जइ (heredoc)
	{
	    SOURCING_LNUM = cac->cac_start_lnum;
	    जइ (cac->cac_lhs.lhs_has_type
		    && need_type(&t_list_string, cac->cac_lhs.lhs_type,
				0, -1, 0, cctx, FALSE, FALSE) == FAIL)
		प्रत्यागम FAIL;
	}
	अण्णहा
	{
	    जइ (compile_assign_rhs(cctx, cac) == FAIL)
		प्रत्यागम FAIL;
	    जइ (cac->cac_var_गणनम् == 0)
		cac->cac_var_end = cac->cac_nextc;
	}

	// no need to parse more when skipping
	जइ (cctx->ctx_skip == SKIP_YES)
	    भङ्ग;

	जइ (cac->cac_oplen > 0 && *cac->cac_op != '=')
	{
	    जइ (compile_assign_compound_op(cctx, cac) == FAIL)
		प्रत्यागम FAIL;
	}

	// generate the store instructions
	जइ (compile_assign_generate_store(cctx, cac) == FAIL)
	    प्रत्यागम FAIL;

	जइ (cac->cac_var_idx + 1 < cac->cac_var_गणनम्)
	    cac->cac_var_start = skipwhite(cac->cac_lhs.lhs_end + 1);

	जइ (has_argisset_prefix)
	{
	    // set instruction index in JUMP_IF_ARG_SET to here
	    isn_T *isn = ((isn_T *)cac->cac_instr->ga_data) + jump_instr_idx;
	    isn->isn_arg.jumparg.jump_where = cac->cac_instr->ga_len;
	}
    }

    प्रत्यागम OK;
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
    स्थिर char_u *
compile_assignment(
    char_u	*arg_start,
    exarg_T	*eap,
    cmdidx_T	cmdidx,
    cctx_T	*cctx)
{
    cac_T	cac;
    char_u	*arg = arg_start;
    char_u	*retstr = रिक्त;
    पूर्णाङ्क		heredoc = FALSE;
    पूर्णाङ्क		jump_instr_idx;

    compile_assign_context_init(&cac, cctx, cmdidx, arg);

    jump_instr_idx = cac.cac_instr->ga_len;

    // process object variable initialization in a new() constructor method
    पूर्णाङ्क	has_argisset_prefix = STRNCMP(arg, "ifargisset ", 11) == 0;
    जइ (has_argisset_prefix &&
			compile_assign_obj_new_arg(&arg, cctx) == FAIL)
	गच्छ theend;

    // Skip over the "varname" or "[varname, varname]" to get to any "=".
    cac.cac_nextc = skip_var_list(arg, TRUE, &cac.cac_var_गणनम्,
						&cac.cac_semicolon, TRUE);
    जइ (cac.cac_nextc == रिक्त)
	प्रत्यागम *arg == '[' ? arg : रिक्त;

    जइ (compile_assign_process_operator(eap, arg, &cac, &heredoc,
							&retstr) == FAIL)
	प्रत्यागम retstr;

    // Compute the start of the assignment
    cac.cac_var_start = compile_assign_compute_start(arg, cac.cac_var_गणनम्);

    // Compute the end of the assignment
    cac.cac_var_end = compile_assign_compute_end(eap, cctx, &cac, heredoc);
    जइ (cac.cac_var_end == रिक्त)
	प्रत्यागम रिक्त;

    पूर्णाङ्क has_cmd = cac.cac_var_start > eap->cmd;

    /* process the variable(s) */
    जइ (compile_assign_process_variables(cctx, &cac, cmdidx, heredoc,
					 has_cmd, has_argisset_prefix,
					 jump_instr_idx) == FAIL)
	गच्छ theend;

    // For "[var, var] = expr" drop the "expr" value.
    // Also for "[var, var; _] = expr".
    जइ (cctx->ctx_skip != SKIP_YES && cac.cac_var_गणनम् > 0 &&
	    (!cac.cac_semicolon || !cac.cac_did_generate_slice))
    {
	जइ (generate_instr_drop(cctx, ISN_DROP, 1) == रिक्त)
	    गच्छ theend;
    }

    retstr = skipwhite(cac.cac_var_end);

theend:
    vim_free(cac.cac_lhs.lhs_name);
    प्रत्यागम retstr;
}

/*
 * Check for an assignment at "eap->cmd", compile it if found.
 * Return NOTDONE if there is none, FAIL for failure, OK if done.
 */
    स्थिर पूर्णाङ्क
may_compile_assignment(exarg_T *eap, char_u **पङ्क्तिः, cctx_T *cctx)
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
    जइ (p > eap->cmd && *p != NUL)
    {
	char_u *var_end;
	पूर्णाङ्क	oplen;
	पूर्णाङ्क	heredoc;

	जइ (eap->cmd[0] == '@')
	    var_end = eap->cmd + 2;
	अण्णहा
	    var_end = find_name_end(pskip, रिक्त, रिक्त,
					FNE_CHECK_START | FNE_INCL_BR);
	oplen = assignment_len(skipwhite(var_end), &heredoc);
	जइ (oplen > 0)
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
	    जइ (*eap->cmd == '&'
		    || *eap->cmd == '$'
		    || *eap->cmd == '@'
		    || ((len) > 2 && eap->cmd[1] == ':')
		    || STRNCMP(eap->cmd, "g:[", 3) == 0
		    || variable_exists(eap->cmd, len, cctx))
	    {
		*पङ्क्तिः = compile_assignment(eap->cmd, eap, CMD_SIZE, cctx);
		जइ (*पङ्क्तिः == रिक्त || *पङ्क्तिः == eap->cmd)
		    प्रत्यागम FAIL;
		प्रत्यागम OK;
	    }
	}
    }

    // might be "[var, var] = expr" or "ifargisset this.member = expr"
    जइ (*eap->cmd == '[' || STRNCMP(eap->cmd, "ifargisset ", 11) == 0)
    {
	*पङ्क्तिः = compile_assignment(eap->cmd, eap, CMD_SIZE, cctx);
	जइ (*पङ्क्तिः == रिक्त)
	    प्रत्यागम FAIL;
	जइ (*पङ्क्तिः != eap->cmd)
	    प्रत्यागम OK;
    }
    प्रत्यागम NOTDONE;
}

/*
 * Check if arguments of "ufunc" shadow variables in "cctx".
 * Return OK or FAIL.
 */
    स्थिर पूर्णाङ्क
check_args_shadowing(ufunc_T *ufunc, cctx_T *cctx)
{
    पूर्णाङ्क	    i;
    char_u  *arg;
    पूर्णाङ्क	    r = OK;

    // Make sure arguments are not found when compiling a second time.
    ufunc->uf_args_visible = 0;

    // Check for arguments shadowing variables from the context.
    प्रति (i = 0; i < ufunc->uf_args.ga_len; ++i)
    {
	arg = ((char_u **)(ufunc->uf_args.ga_data))[i];
	जइ (check_defined(arg, STRLEN(arg), cctx, रिक्त, TRUE) == FAIL)
	{
	    r = FAIL;
	    भङ्ग;
	}
    }
    ufunc->uf_args_visible = ufunc->uf_args.ga_len;
    प्रत्यागम r;
}

#ifdef HAS_MESSAGE_WINDOW
/*
 * Get a count before a command.  Can only be a number.
 * Returns zero if there is no count.
 * Returns -1 if there is something wrong.
 */
    स्थिर long
get_cmd_गणनम्(char_u *पङ्क्तिः, exarg_T *eap)
{
    char_u *p;

    // skip over colons and white space
    प्रति (p = पङ्क्तिः; *p == ':' || VIM_ISWHITE(*p); ++p)
	;
    जइ (!SAFE_isdigit(*p))
    {
	// The command or modifiers must be following.  Assume a lower case
	// character means there is a modifier.
	जइ (p < eap->cmd && !vim_islower(*p))
	{
	    emsg(_(e_invalid_range));
	    प्रत्यागम -1;
	}
	प्रत्यागम 0;
    }
    प्रत्यागम atol((वर्ण *)p);
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

    जइ (debug_break_level > 0 || may_break_in_function(ufunc))
	प्रत्यागम CT_DEBUG;
#ifdef FEAT_PROFILE
    जइ (do_profiling == PROF_YES)
    {
	जइ (!ufunc->uf_profiling && has_profiling(FALSE, ufunc->uf_name, रिक्त,
							    &ufunc->uf_hash))
	    func_do_profile(ufunc);
	जइ (ufunc->uf_profiling)
	    प्रत्यागम CT_PROFILE;
    }
#endif
    प्रत्यागम CT_NONE;
}

/*
 * Free the compiled instructions saved for a def function.  This is used when
 * compiling a def function and the function was compiled before.
 * The index is reused.
 */
    स्थिर शून्य
clear_def_function(ufunc_T *ufunc, compiletype_T compile_type)
{
    isn_T	*instr_dest = रिक्त;
    dfunc_T	*dfunc;

    dfunc = ((dfunc_T *)def_functions.ga_data) + ufunc->uf_dfunc_idx;

    विकल्प (compile_type)
    {
	पक्ष CT_PROFILE:
#ifdef FEAT_PROFILE
	    instr_dest = dfunc->df_instr_prof; भङ्ग;
#endif
	पक्ष CT_NONE:   instr_dest = dfunc->df_instr; भङ्ग;
	पक्ष CT_DEBUG:  instr_dest = dfunc->df_instr_debug; भङ्ग;
    }

    जइ (instr_dest != रिक्त)
	// Was compiled in this mode before: Free old instructions.
	delete_def_function_contents(dfunc, FALSE);

    ga_clear_strings(&dfunc->df_var_names);
    dfunc->df_defer_var_idx = 0;
}

/*
 * Add a function to the list of :def functions.
 * This sets "ufunc->uf_dfunc_idx" but the function isn't compiled yet.
 */
    स्थिर पूर्णाङ्क
add_def_function(ufunc_T *ufunc)
{
    dfunc_T *dfunc;

    जइ (def_functions.ga_len == 0)
    {
	// The first position is not used, so that a zero uf_dfunc_idx means it
	// wasn't set.
	जइ (GA_GROW_FAILS(&def_functions, 1))
	    प्रत्यागम FAIL;
	++def_functions.ga_len;
    }

    // Add the function to "def_functions".
    जइ (GA_GROW_FAILS(&def_functions, 1))
	प्रत्यागम FAIL;
    dfunc = ((dfunc_T *)def_functions.ga_data) + def_functions.ga_len;
    CLEAR_POINTER(dfunc);
    dfunc->df_idx = def_functions.ga_len;
    ufunc->uf_dfunc_idx = dfunc->df_idx;
    dfunc->df_ufunc = ufunc;
    dfunc->df_name = vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
    ga_init2(&dfunc->df_var_names, परिमाण(char_u *), 10);
    ++dfunc->df_refcount;
    ++def_functions.ga_len;
    प्रत्यागम OK;
}

    स्थिर पूर्णाङ्क
compile_dfunc_ufunc_init(
    ufunc_T		*ufunc,
    cctx_T		*outer_cctx,
    compiletype_T	compile_type,
    पूर्णाङ्क			*new_def_function)
{
    // When using a function that was compiled before: Free old instructions.
    // The index is reused.  Otherwise add a new entry in "def_functions".
    जइ (ufunc->uf_dfunc_idx > 0)
	clear_def_function(ufunc, compile_type);
    अण्णहा
    {
	जइ (add_def_function(ufunc) == FAIL)
	    प्रत्यागम FAIL;

	*new_def_function = TRUE;
    }

    जइ ((ufunc->uf_flags & FC_CLOSURE) && outer_cctx == रिक्त)
    {
	semsg(_(e_compiling_closure_without_context_str),
						printable_func_name(ufunc));
	प्रत्यागम FAIL;
    }

    ufunc->uf_def_status = UF_COMPILING;

    प्रत्यागम OK;
}

/*
 * Initialize the compilation context for compiling a def function.
 */
    स्थिर शून्य
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
    ga_init2(&cctx->ctx_locals, परिमाण(lvar_T), 10);
    // Each entry on the type stack consists of two type pointers.
    ga_init2(&cctx->ctx_type_राशिः, परिमाण(type2_T), 50);
    cctx->ctx_type_list = &ufunc->uf_type_list;
    ga_init2(&cctx->ctx_instr, परिमाण(isn_T), 50);
}

/*
 * For an object constructor, generate instruction to setup "this" (the first
 * local variable) and to initialize the object variables.
 */
    स्थिर पूर्णाङ्क
obj_constructor_prologue(ufunc_T *ufunc, cctx_T *cctx)
{
    generate_CONSTRUCT(cctx, ufunc->uf_class);

    प्रति (पूर्णाङ्क i = 0; i < ufunc->uf_class->class_obj_member_गणनम्; ++i)
    {
	ocmember_T *m = &ufunc->uf_class->class_obj_members[i];

	जइ (i < 2 && IS_ENUM(ufunc->uf_class))
	    // The first two object variables in an enum are the name
	    // and the ordinal.  These are set by the ISN_CONSTRUCT
	    // instruction.  So don't generate instructions to set
	    // these variables.
	    अनुवर्त;

	जइ (m->ocm_init != रिक्त)
	{
	    char_u	*व्यञ्जकम् = m->ocm_init;
	    sctx_T	save_current_sctx;
	    पूर्णाङ्क		change_sctx = FALSE;

	    // If the member variable initialization script context is
	    // different from the current script context, then change it.
	    जइ (current_sctx.sc_sid != m->ocm_init_sctx.sc_sid)
		change_sctx = TRUE;

	    जइ (change_sctx)
	    {
		// generate an instruction to change the script context to the
		// member variable initialization script context.
		save_current_sctx = current_sctx;
		current_sctx = m->ocm_init_sctx;
		generate_SCRIPTCTX_SET(cctx, current_sctx);
	    }

	    पूर्णाङ्क r = compile_expr0(&व्यञ्जकम्, cctx);

	    जइ (change_sctx)
	    {
		// restore the previous script context
		current_sctx = save_current_sctx;
		generate_SCRIPTCTX_SET(cctx, current_sctx);
	    }

	    जइ (r == FAIL)
		प्रत्यागम FAIL;

	    जइ (!ends_excmd2(m->ocm_init, व्यञ्जकम्))
	    {
		semsg(_(e_trailing_characters_str), व्यञ्जकम्);
		प्रत्यागम FAIL;
	    }

	    type_T	*type = get_type_on_राशिः(cctx, 0);
	    जइ (m->ocm_type->tt_type == VAR_ANY
		    && !(m->ocm_flags & OCMFLAG_HAS_TYPE)
		    && type->tt_type != VAR_SPECIAL)
	    {
		// If the member variable type is not yet set, then use
		// the initialization expression type.
		m->ocm_type = type;
	    }
	    अण्णहा
	    {
		// The type of the member initialization expression is
		// determined at run time.  Add a runtime type check.
		where_T	where = WHERE_INIT;
		where.wt_kind = WT_MEMBER;
		where.wt_func_name = (वर्ण *)m->ocm_name.string;
		जइ (need_type_where(type, m->ocm_type, 0, -1,
					where, cctx, FALSE, FALSE) == FAIL)
		    प्रत्यागम FAIL;
	    }
	}
	अण्णहा
	    प्रक्षिप_default_मानम्(cctx, m->ocm_type->tt_type, FALSE, रिक्त);

	जइ (((m->ocm_type->tt_type == VAR_DICT
			|| m->ocm_type->tt_type == VAR_LIST)
		    && m->ocm_type->tt_member != रिक्त
		    && m->ocm_type->tt_member != &t_any
		    && m->ocm_type->tt_member != &t_unknown)
		|| (m->ocm_type->tt_type == VAR_TUPLE
		    && m->ocm_type->tt_argcount > 0))
	    // Set the type in the list, tuple or dict, so that it can be
	    // checked, also in legacy script.
	    generate_SETTYPE(cctx, m->ocm_type);

	generate_STORE_THIS(cctx, i);
    }

    प्रत्यागम OK;
}

/*
 * For an object method and an constructor, generate instruction to setup
 * "this" (the first local variable).  For a constructor, generate instructions
 * to initialize the object variables.
 */
    स्थिर पूर्णाङ्क
obj_method_prologue(ufunc_T *ufunc, cctx_T *cctx)
{
    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data) + ufunc->uf_dfunc_idx;

    जइ (GA_GROW_FAILS(&dfunc->df_var_names, 1))
	प्रत्यागम FAIL;

    ((char_u **)dfunc->df_var_names.ga_data)[0] =
						vim_strsave((char_u *)"this");
    ++dfunc->df_var_names.ga_len;

    // In the constructor allocate memory for the object and initialize the
    // object members.
    जइ (IS_CONSTRUCTOR_METHOD(ufunc))
	प्रत्यागम obj_constructor_prologue(ufunc, cctx);

    प्रत्यागम OK;
}

/*
 * Produce instructions for the default values of optional arguments.
 */
    स्थिर पूर्णाङ्क
compile_def_function_default_args(
    ufunc_T	*ufunc,
    garray_T	*instr,
    cctx_T	*cctx)
{
    पूर्णाङ्क	गणनम् = ufunc->uf_def_args.ga_len;
    पूर्णाङ्क	first_def_arg = ufunc->uf_args.ga_len - गणनम्;
    पूर्णाङ्क	i;
    पूर्णाङ्क	off = राशिः_FRAME_SIZE + (ufunc->uf_va_name != रिक्त ? 1 : 0);
    पूर्णाङ्क	did_set_arg_type = FALSE;

    // Produce instructions for the default values of optional arguments.
    SOURCING_LNUM = 0;  // line number unknown
    प्रति (i = 0; i < गणनम्; ++i)
    {
	char_u *arg = ((char_u **)(ufunc->uf_def_args.ga_data))[i];
	जइ (STRCMP(arg, "v:none") == 0)
	    // "arg = v:none" means the argument is optional without
	    // setting a value when the argument is missing.
	    अनुवर्त;

	type_T	*val_type;
	पूर्णाङ्क		arg_idx = first_def_arg + i;
	where_T	where = WHERE_INIT;
	पूर्णाङ्क		jump_instr_idx = instr->ga_len;
	isn_T	*isn;

	// Use a JUMP_IF_ARG_SET instruction to skip if the value was given.
	जइ (generate_JUMP_IF_ARG(cctx, ISN_JUMP_IF_ARG_SET,
						i - गणनम् - off) == FAIL)
	    प्रत्यागम FAIL;

	// Make sure later arguments are not found.
	ufunc->uf_args_visible = arg_idx;

	पूर्णाङ्क r = compile_expr0(&arg, cctx);
	जइ (r == FAIL)
	    प्रत्यागम FAIL;

	// If no type specified use the type of the default value.
	// Otherwise check that the default value type matches the
	// specified type.
	val_type = get_type_on_राशिः(cctx, 0);
	where.wt_index = arg_idx + 1;
	where.wt_kind = WT_ARGUMENT;
	जइ (ufunc->uf_arg_types[arg_idx] == &t_unknown)
	{
	    did_set_arg_type = TRUE;
	    ufunc->uf_arg_types[arg_idx] = val_type;
	}
	अण्णहा जइ (need_type_where(val_type, ufunc->uf_arg_types[arg_idx],
		    0, -1, where, cctx, FALSE, FALSE) == FAIL)
	    प्रत्यागम FAIL;

	जइ (generate_STORE(cctx, ISN_STORE, i - गणनम् - off, रिक्त) == FAIL)
	    प्रत्यागम FAIL;

	// set instruction index in JUMP_IF_ARG_SET to here
	isn = ((isn_T *)instr->ga_data) + jump_instr_idx;
	isn->isn_arg.jumparg.jump_where = instr->ga_len;
    }

    जइ (did_set_arg_type)
	set_function_type(ufunc);

    प्रत्यागम OK;
}

/*
 * Compile def function body.  Loop over all the lines in the function and
 * generate instructions.
 */
    स्थिर पूर्णाङ्क
compile_def_function_body(
    पूर्णाङ्क		last_func_lnum,
    पूर्णाङ्क		check_return_type,
    garray_T	*पङ्क्तयः_to_free,
    वर्ण	**errormsg,
    cctx_T	*cctx)
{
    char_u	*पङ्क्तिः = रिक्त;
    char_u	*p;
    पूर्णाङ्क		did_emsg_before = did_emsg;
#ifdef FEAT_PROFILE
    पूर्णाङ्क		prof_lnum = -1;
#endif
    पूर्णाङ्क		debug_lnum = -1;

    प्रति (;;)
    {
	exarg_T	    ea;
	पूर्णाङ्क	    starts_with_colon = FALSE;
	char_u	    *cmd;
	cmdmod_T    local_cmdmod;

	// Bail out on the first error to avoid a flood of errors and report
	// the right line number when inside try/catch.
	जइ (did_emsg_before != did_emsg)
	    प्रत्यागम FAIL;

	जइ (पङ्क्तिः != रिक्त && *पङ्क्तिः == '|')
	    // the line continues after a '|'
	    ++पङ्क्तिः;
	अण्णहा जइ (पङ्क्तिः != रिक्त && *skipwhite(पङ्क्तिः) != NUL
		&& !(*पङ्क्तिः == '#' && (पङ्क्तिः == cctx->ctx_पङ्क्तिः_start
						    || VIM_ISWHITE(पङ्क्तिः[-1]))))
	{
	    semsg(_(e_trailing_characters_str), पङ्क्तिः);
	    प्रत्यागम FAIL;
	}
	अण्णहा जइ (पङ्क्तिः != रिक्त && vim9_bad_comment(skipwhite(पङ्क्तिः)))
	    प्रत्यागम FAIL;
	अण्णहा
	{
	    पङ्क्तिः = next_पङ्क्तिः_from_context(cctx, FALSE);
	    जइ (cctx->ctx_lnum >= last_func_lnum)
	    {
		// beyond the last line
#ifdef FEAT_PROFILE
		जइ (cctx->ctx_skip != SKIP_YES)
		    may_generate_prof_end(cctx, prof_lnum);
#endif
		भङ्ग;
	    }
	    // Make a copy, splitting off nextcmd and removing trailing spaces
	    // may change it.
	    जइ (पङ्क्तिः != रिक्त)
	    {
		पङ्क्तिः = vim_strsave(पङ्क्तिः);
		जइ (ga_add_string(पङ्क्तयः_to_free, पङ्क्तिः) == FAIL)
		{
		    vim_free(पङ्क्तिः);
		    प्रत्यागम FAIL;
		}
	    }
	}

	CLEAR_FIELD(ea);
	ea.cmdlinep = &पङ्क्तिः;
	ea.cmd = skipwhite(पङ्क्तिः);
	ea.skip = cctx->ctx_skip == SKIP_YES;

	जइ (*ea.cmd == '#')
	{
	    // "#" starts a comment, but "#{" is an error
	    जइ (vim9_bad_comment(ea.cmd))
		प्रत्यागम FAIL;
	    पङ्क्तिः = (char_u *)"";
	    अनुवर्त;
	}

#ifdef FEAT_PROFILE
	जइ (cctx->ctx_compile_type == CT_PROFILE && cctx->ctx_lnum != prof_lnum
						  && cctx->ctx_skip != SKIP_YES)
	{
	    may_generate_prof_end(cctx, prof_lnum);

	    prof_lnum = cctx->ctx_lnum;
	    generate_instr(cctx, ISN_PROF_START);
	}
#endif
	जइ (cctx->ctx_compile_type == CT_DEBUG && cctx->ctx_lnum != debug_lnum
						  && cctx->ctx_skip != SKIP_YES)
	{
	    debug_lnum = cctx->ctx_lnum;
	    generate_instr_debug(cctx);
	}
	cctx->ctx_prev_lnum = cctx->ctx_lnum + 1;

	// Some things can be recognized by the first character.
	विकल्प (*ea.cmd)
	{
	    पक्ष '}':
		{
		    // "}" ends a block scope
		    scopetype_T stype = cctx->ctx_scope == रिक्त
					  ? NO_SCOPE : cctx->ctx_scope->se_type;

		    जइ (stype == BLOCK_SCOPE)
		    {
			compile_endblock(cctx);
			पङ्क्तिः = ea.cmd;
		    }
		    अण्णहा
		    {
			emsg(_(e_using_rcurly_outside_if_block_scope));
			प्रत्यागम FAIL;
		    }
		    जइ (पङ्क्तिः != रिक्त)
			पङ्क्तिः = skipwhite(ea.cmd + 1);
		    अनुवर्त;
		}

	    पक्ष '{':
		// "{" starts a block scope
		// "{'a': 1}->func() is something else
		जइ (ends_excmd(*skipwhite(ea.cmd + 1)))
		{
		    पङ्क्तिः = compile_block(ea.cmd, cctx);
		    अनुवर्त;
		}
		भङ्ग;
	}

	/*
	 * COMMAND MODIFIERS
	 */
	cctx->ctx_has_cmdmod = FALSE;
	जइ (parse_command_modifiers(&ea, errormsg, &local_cmdmod, FALSE)
								       == FAIL)
	    प्रत्यागम FAIL;
	generate_cmdmods(cctx, &local_cmdmod);
	undo_cmdmod(&local_cmdmod);

	// Check if there was a colon after the last command modifier or before
	// the current position.
	प्रति (p = ea.cmd; p >= पङ्क्तिः; --p)
	{
	    जइ (*p == ':')
		starts_with_colon = TRUE;
	    जइ (p < ea.cmd && !VIM_ISWHITE(*p))
		भङ्ग;
	}

	// Skip ":call" to get to the function name, unless using :legacy
	p = ea.cmd;
	जइ (!(local_cmdmod.cmod_flags & CMOD_LEGACY))
	{
	    जइ (checkforcmd(&ea.cmd, "call", 3))
	    {
		जइ (*ea.cmd == '(')
		    // not for "call()"
		    ea.cmd = p;
		अण्णहा
		    ea.cmd = skipwhite(ea.cmd);
	    }

	    जइ (!starts_with_colon)
	    {
		पूर्णाङ्क	    assign;

		// Check for assignment after command modifiers.
		assign = may_compile_assignment(&ea, &पङ्क्तिः, cctx);
		जइ (assign == OK)
		    गच्छ nextline;
		जइ (assign == FAIL)
		    प्रत्यागम FAIL;
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
	जइ ((*cmd != '$' || starts_with_colon)
		&& (starts_with_colon
		    || !(*cmd == '\''
			|| (cmd[0] == '0' && cmd[1] == 'z')
			|| (cmd[0] != NUL && cmd[0] == cmd[1]
					    && (*cmd == '+' || *cmd == '-'))
			|| सङ्ख्या_method(cmd))))
	{
	    ea.cmd = skip_range(ea.cmd, TRUE, रिक्त);
	    जइ (ea.cmd > cmd)
	    {
		जइ (!starts_with_colon
				   && !(local_cmdmod.cmod_flags & CMOD_LEGACY))
		{
		    semsg(_(e_colon_required_before_range_str), cmd);
		    प्रत्यागम FAIL;
		}
		ea.addr_गणनम् = 1;
		जइ (ends_excmd2(पङ्क्तिः, ea.cmd))
		{
		    // A range without a command: jump to the line.
		    generate_EXEC(cctx, ISN_EXECRANGE,
					      vim_strnsave(cmd, ea.cmd - cmd));
		    पङ्क्तिः = ea.cmd;
		    गच्छ nextline;
		}
	    }
	}
	p = find_ex_command(&ea, रिक्त,
		starts_with_colon || (local_cmdmod.cmod_flags & CMOD_LEGACY)
						  ? रिक्त : item_exists, cctx);

	जइ (p == रिक्त)
	{
	    जइ (cctx->ctx_skip != SKIP_YES)
		semsg(_(e_ambiguous_use_of_user_defined_command_str), ea.cmd);
	    प्रत्यागम FAIL;
	}

	// When using ":legacy cmd" always use compile_exec().
	जइ (local_cmdmod.cmod_flags & CMOD_LEGACY)
	{
	    char_u *start = ea.cmd;

	    विकल्प (ea.cmdidx)
	    {
		पक्ष CMD_if:
		पक्ष CMD_elseif:
		पक्ष CMD_else:
		पक्ष CMD_endif:
		पक्ष CMD_for:
		पक्ष CMD_endfor:
		पक्ष CMD_continue:
		पक्ष CMD_break:
		पक्ष CMD_while:
		पक्ष CMD_endwhile:
		पक्ष CMD_try:
		पक्ष CMD_catch:
		पक्ष CMD_finally:
		पक्ष CMD_endtry:
			semsg(_(e_cannot_use_legacy_with_command_str), ea.cmd);
			प्रत्यागम FAIL;
		स्वभाव: भङ्ग;
	    }

	    // ":legacy return expr" needs to be handled differently.
	    जइ (checkforcmd(&start, "return", 4))
		ea.cmdidx = CMD_return;
	    अण्णहा
		ea.cmdidx = CMD_legacy;
	}

	जइ (p == ea.cmd && ea.cmdidx != CMD_SIZE)
	{
	    // "eval" is used for "val->func()" and "var" for "var = val", then
	    // "p" is equal to "ea.cmd" for a valid command.
	    जइ (ea.cmdidx == CMD_eval || ea.cmdidx == CMD_var)
		;
	    अण्णहा जइ (cctx->ctx_skip == SKIP_YES)
	    {
		पङ्क्तिः += STRLEN(पङ्क्तिः);
		गच्छ nextline;
	    }
	    अण्णहा
	    {
		semsg(_(e_command_not_recognized_str), ea.cmd);
		प्रत्यागम FAIL;
	    }
	}

	जइ ((cctx->ctx_had_return || cctx->ctx_had_throw)
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
	    प्रत्यागम FAIL;
	}

	// When processing the end of an if-else or try-catch block, don't
	// clear the "ctx_had_throw" flag.  If such a block ends in a "throw"
	// statement, then it is considered to end in a "return" statement.
	// The "ctx_had_throw" is cleared immediately after processing the
	// block ending statement.
	// Otherwise, clear the "had_throw" flag.
	जइ (ea.cmdidx != CMD_else && ea.cmdidx != CMD_elseif
						&& ea.cmdidx != CMD_endif
						&& ea.cmdidx != CMD_catch
						&& ea.cmdidx != CMD_finally
						&& ea.cmdidx != CMD_endtry)
	    cctx->ctx_had_throw = FALSE;

	p = skipwhite(p);
	जइ (ea.cmdidx != CMD_SIZE
			    && ea.cmdidx != CMD_write && ea.cmdidx != CMD_read)
	{
	    जइ (ea.cmdidx >= 0)
		ea.argt = excmd_get_argt(ea.cmdidx);
	    जइ ((ea.argt & EX_BANG) && *p == '!')
	    {
		ea.forceit = TRUE;
		p = skipwhite(p + 1);
	    }
	    जइ ((ea.argt & EX_RANGE) == 0 && ea.addr_गणनम् > 0)
	    {
		emsg(_(e_no_range_allowed));
		प्रत्यागम FAIL;
	    }
	}

	विकल्प (ea.cmdidx)
	{
	    पक्ष CMD_def:
	    पक्ष CMD_function:
		    ea.arg = p;
		    पङ्क्तिः = compile_nested_function(&ea, cctx, पङ्क्तयः_to_free);
		    भङ्ग;

	    पक्ष CMD_return:
		    पङ्क्तिः = compile_return(p, check_return_type,
				 local_cmdmod.cmod_flags & CMOD_LEGACY, cctx);
		    cctx->ctx_had_return = TRUE;
		    भङ्ग;

	    पक्ष CMD_let:
		    emsg(_(e_cannot_use_let_in_vim9_script));
		    भङ्ग;
	    पक्ष CMD_var:
	    पक्ष CMD_final:
	    पक्ष CMD_const:
	    पक्ष CMD_increment:
	    पक्ष CMD_decrement:
		    पङ्क्तिः = compile_assignment(p, &ea, ea.cmdidx, cctx);
		    जइ (पङ्क्तिः == p)
		    {
			emsg(_(e_invalid_assignment));
			पङ्क्तिः = रिक्त;
		    }
		    भङ्ग;

	    पक्ष CMD_unlet:
	    पक्ष CMD_unlockvar:
	    पक्ष CMD_lockvar:
		    पङ्क्तिः = compile_unletlock(p, &ea, cctx);
		    भङ्ग;

	    पक्ष CMD_import:
		    emsg(_(e_import_can_only_be_used_in_script));
		    पङ्क्तिः = रिक्त;
		    भङ्ग;

	    पक्ष CMD_if:
		    पङ्क्तिः = compile_if(p, cctx);
		    भङ्ग;
	    पक्ष CMD_elseif:
		    पङ्क्तिः = compile_elseif(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    cctx->ctx_had_throw = FALSE;
		    भङ्ग;
	    पक्ष CMD_else:
		    पङ्क्तिः = compile_else(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    cctx->ctx_had_throw = FALSE;
		    भङ्ग;
	    पक्ष CMD_endif:
		    पङ्क्तिः = compile_endif(p, cctx);
		    cctx->ctx_had_throw = FALSE;
		    भङ्ग;

	    पक्ष CMD_while:
		    पङ्क्तिः = compile_while(p, cctx);
		    भङ्ग;
	    पक्ष CMD_endwhile:
		    पङ्क्तिः = compile_endwhile(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    भङ्ग;

	    पक्ष CMD_for:
		    पङ्क्तिः = compile_for(p, cctx);
		    भङ्ग;
	    पक्ष CMD_endfor:
		    पङ्क्तिः = compile_endfor(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    भङ्ग;
	    पक्ष CMD_continue:
		    पङ्क्तिः = compile_continue(p, cctx);
		    // "continue" unconditionally jumps back to the loop,
		    // like a "return".
		    cctx->ctx_had_return = TRUE;
		    भङ्ग;
	    पक्ष CMD_break:
		    पङ्क्तिः = compile_break(p, cctx);
		    // "break" unconditionally exits the loop, like a "return".
		    cctx->ctx_had_return = TRUE;
		    भङ्ग;

	    पक्ष CMD_try:
		    पङ्क्तिः = compile_try(p, cctx);
		    भङ्ग;
	    पक्ष CMD_catch:
		    पङ्क्तिः = compile_catch(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    cctx->ctx_had_throw = FALSE;
		    भङ्ग;
	    पक्ष CMD_finally:
		    पङ्क्तिः = compile_finally(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    cctx->ctx_had_throw = FALSE;
		    भङ्ग;
	    पक्ष CMD_endtry:
		    पङ्क्तिः = compile_endtry(p, cctx);
		    cctx->ctx_had_throw = FALSE;
		    भङ्ग;
	    पक्ष CMD_throw:
		    पङ्क्तिः = compile_throw(p, cctx);
		    cctx->ctx_had_throw = TRUE;
		    भङ्ग;

	    पक्ष CMD_eval:
		    पङ्क्तिः = compile_eval(p, cctx);
		    भङ्ग;

	    पक्ष CMD_defer:
		    पङ्क्तिः = compile_defer(p, cctx);
		    भङ्ग;

#ifdef HAS_MESSAGE_WINDOW
	    पक्ष CMD_echowindow:
		    {
			long cmd_गणनम् = get_cmd_गणनम्(पङ्क्तिः, &ea);
			जइ (cmd_गणनम् < 0)
			    पङ्क्तिः = रिक्त;
			अण्णहा
			    पङ्क्तिः = compile_mult_व्यञ्जकम्(p, ea.cmdidx,
							     cmd_गणनम्, cctx);
		    }
		    भङ्ग;
#endif
	    पक्ष CMD_echo:
	    पक्ष CMD_echon:
	    पक्ष CMD_echoconsole:
	    पक्ष CMD_echoerr:
	    पक्ष CMD_echomsg:
	    पक्ष CMD_execute:
		    पङ्क्तिः = compile_mult_व्यञ्जकम्(p, ea.cmdidx, 0, cctx);
		    भङ्ग;

	    पक्ष CMD_put:
		    ea.cmd = cmd;
		    पङ्क्तिः = compile_put(p, &ea, cctx, FALSE);
		    भङ्ग;

	    पक्ष CMD_iput:
		    ea.cmd = cmd;
		    पङ्क्तिः = compile_put(p, &ea, cctx, TRUE);
		    भङ्ग;

	    पक्ष CMD_substitute:
		    जइ (check_global_and_subst(ea.cmd, p) == FAIL)
			प्रत्यागम FAIL;
		    जइ (cctx->ctx_skip == SKIP_YES)
			पङ्क्तिः = (char_u *)"";
		    अण्णहा
		    {
			ea.arg = p;
			पङ्क्तिः = compile_substitute(पङ्क्तिः, &ea, cctx);
		    }
		    भङ्ग;

	    पक्ष CMD_redir:
		    ea.arg = p;
		    पङ्क्तिः = compile_redir(पङ्क्तिः, &ea, cctx);
		    भङ्ग;

	    पक्ष CMD_cexpr:
	    पक्ष CMD_lexpr:
	    पक्ष CMD_caddexpr:
	    पक्ष CMD_laddexpr:
	    पक्ष CMD_cgetexpr:
	    पक्ष CMD_lgetexpr:
#ifdef FEAT_QUICKFIX
		    ea.arg = p;
		    पङ्क्तिः = compile_cexpr(पङ्क्तिः, &ea, cctx);
#अण्णहा
		    ex_ni(&ea);
		    पङ्क्तिः = रिक्त;
#endif
		    भङ्ग;

	    पक्ष CMD_append:
	    पक्ष CMD_change:
	    पक्ष CMD_निवेशय:
	    पक्ष CMD_k:
	    पक्ष CMD_open:
	    पक्ष CMD_t:
	    पक्ष CMD_xit:
		    not_in_vim9(&ea, false);
		    प्रत्यागम FAIL;

	    पक्ष CMD_SIZE:
		    जइ (cctx->ctx_skip != SKIP_YES)
		    {
			semsg(_(e_invalid_command_str), ea.cmd);
			प्रत्यागम FAIL;
		    }
		    // We don't check for a next command here.
		    पङ्क्तिः = (char_u *)"";
		    भङ्ग;

	    पक्ष CMD_lua:
	    पक्ष CMD_mzscheme:
	    पक्ष CMD_perl:
	    पक्ष CMD_py3:
	    पक्ष CMD_python3:
	    पक्ष CMD_python:
	    पक्ष CMD_pythonx:
	    पक्ष CMD_ruby:
	    पक्ष CMD_tcl:
		    ea.arg = p;
		    जइ (vim_strchr(पङ्क्तिः, '\n') == रिक्त)
			पङ्क्तिः = compile_exec(पङ्क्तिः, &ea, cctx);
		    अण्णहा
			// heredoc lines have been concatenated with NL
			// characters in get_function_body()
			पङ्क्तिः = compile_script(पङ्क्तिः, cctx);
		    भङ्ग;

	    पक्ष CMD_vim9script:
		    जइ (cctx->ctx_skip != SKIP_YES)
		    {
			emsg(_(e_vim9script_can_only_be_used_in_script));
			प्रत्यागम FAIL;
		    }
		    पङ्क्तिः = (char_u *)"";
		    भङ्ग;

	    पक्ष CMD_class:
		    emsg(_(e_class_can_only_be_used_in_script));
		    प्रत्यागम FAIL;

	    पक्ष CMD_enum:
		    emsg(_(e_enum_can_only_be_used_in_script));
		    प्रत्यागम FAIL;

	    पक्ष CMD_interface:
		    emsg(_(e_interface_can_only_be_used_in_script));
		    प्रत्यागम FAIL;

	    पक्ष CMD_type:
		    emsg(_(e_type_can_only_be_used_in_script));
		    प्रत्यागम FAIL;

	    पक्ष CMD_global:
		    जइ (check_global_and_subst(ea.cmd, p) == FAIL)
			प्रत्यागम FAIL;
		    // FALLTHROUGH
	    स्वभाव:
		    // Not recognized, execute with do_cmdline_cmd().
		    ea.arg = p;
		    पङ्क्तिः = compile_exec(पङ्क्तिः, &ea, cctx);
		    भङ्ग;
	}
nextline:
	जइ (पङ्क्तिः == रिक्त)
	    प्रत्यागम FAIL;
	पङ्क्तिः = skipwhite(पङ्क्तिः);

	// Undo any command modifiers.
	generate_undo_cmdmods(cctx);

	जइ (cctx->ctx_type_राशिः.ga_len < 0)
	{
	    iemsg("Type stack underflow");
	    प्रत्यागम FAIL;
	}
    } // END of the loop over all the function body lines.

    प्रत्यागम OK;
}

/*
 * Returns TRUE if the end of a scope (if, while, for, block) is missing.
 * Called after compiling a def function body.
 */
    स्थिर पूर्णाङ्क
compile_dfunc_scope_end_missing(cctx_T *cctx)
{
    जइ (cctx->ctx_scope == रिक्त)
	प्रत्यागम FALSE;

    विकल्प (cctx->ctx_scope->se_type)
    {
	पक्ष IF_SCOPE:
	    emsg(_(e_missing_endif));
	    भङ्ग;
	पक्ष WHILE_SCOPE:
	    emsg(_(e_missing_endwhile));
	    भङ्ग;
	पक्ष FOR_SCOPE:
	    emsg(_(e_missing_endfor));
	    भङ्ग;
	पक्ष TRY_SCOPE:
	    emsg(_(e_missing_endtry));
	    भङ्ग;
	पक्ष BLOCK_SCOPE:
	    // end block scope from :try (maybe)
	    compile_endblock(cctx);
	    जइ (cctx->ctx_scope != रिक्त
		    && cctx->ctx_scope->se_type == TRY_SCOPE)
		emsg(_(e_missing_endtry));
	    अण्णहा
		emsg(_(e_missing_rcurly));
	    भङ्ग;
	स्वभाव:
	    emsg(_(e_missing_rcurly));
    }
    प्रत्यागम TRUE;
}

/*
 * When compiling a def function, if it doesn't have an explicit return
 * statement, then generate a default return instruction.  For an object
 * constructor, return the object.
 */
    स्थिर पूर्णाङ्क
compile_dfunc_generate_default_return(ufunc_T *ufunc, cctx_T *cctx)
{
    // TODO: if a function ends in "throw" but there was a return elsewhere we
    // should not assume the return type is "void".
    जइ (cctx->ctx_had_return || cctx->ctx_had_throw)
	प्रत्यागम OK;

    जइ (ufunc->uf_ret_type->tt_type == VAR_UNKNOWN)
	ufunc->uf_ret_type = &t_void;
    अण्णहा जइ (ufunc->uf_ret_type->tt_type != VAR_VOID
					&& !IS_CONSTRUCTOR_METHOD(ufunc))
    {
	emsg(_(e_missing_return_statement));
	प्रत्यागम FAIL;
    }

    // Return void if there is no return at the end.
    // For a constructor return the object.
    जइ (IS_CONSTRUCTOR_METHOD(ufunc))
    {
	generate_instr(cctx, ISN_RETURN_OBJECT);
	ufunc->uf_ret_type = &ufunc->uf_class->class_object_type;
    }
    अण्णहा
	generate_instr(cctx, ISN_RETURN_VOID);

    प्रत्यागम OK;
}

/*
 * Perform the chores after successfully compiling a def function.
 */
    स्थिर शून्य
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
    जइ (cctx->ctx_compile_type == CT_PROFILE)
    {
	dfunc->df_instr_prof = instr->ga_data;
	dfunc->df_instr_prof_गणनम् = instr->ga_len;
    }
    अण्णहा
#endif
	जइ (cctx->ctx_compile_type == CT_DEBUG)
	{
	    dfunc->df_instr_debug = instr->ga_data;
	    dfunc->df_instr_debug_गणनम् = instr->ga_len;
	}
	अण्णहा
	{
	    dfunc->df_instr = instr->ga_data;
	    dfunc->df_instr_गणनम् = instr->ga_len;
	}
    dfunc->df_varcount = dfunc->df_var_names.ga_len;
    dfunc->df_has_closure = cctx->ctx_has_closure;

    जइ (cctx->ctx_outer_used)
    {
	ufunc->uf_flags |= FC_CLOSURE;
	जइ (outer_cctx != रिक्त)
	    ++outer_cctx->ctx_closure_गणनम्;
    }

    ufunc->uf_def_status = UF_COMPILED;
}

/*
 * Perform the cleanup when a def function compilation fails.
 */
    स्थिर शून्य
compile_dfunc_ufunc_cleanup(
    ufunc_T	*ufunc,
    garray_T	*instr,
    पूर्णाङ्क		new_def_function,
    वर्ण	*errormsg,
    पूर्णाङ्क		did_emsg_before,
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
    जइ (!dfunc->df_deleted && new_def_function
			&& ufunc->uf_dfunc_idx == def_functions.ga_len - 1)
    {
	--def_functions.ga_len;
	ufunc->uf_dfunc_idx = 0;
    }
    ufunc->uf_def_status = UF_COMPILE_ERROR;

    जाव (cctx->ctx_scope != रिक्त)
	drop_scope(cctx);

    जइ (errormsg != रिक्त)
	emsg(errormsg);
    अण्णहा जइ (did_emsg == did_emsg_before)
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
    पूर्णाङ्क
compile_def_function(
	ufunc_T		*ufunc,
	पूर्णाङ्क		check_return_type,
	compiletype_T   compile_type,
	cctx_T		*outer_cctx)
{
    garray_T	पङ्क्तयः_to_free;
    वर्ण	*errormsg = रिक्त;	// error message
    cctx_T	cctx;
    garray_T	*instr;
    पूर्णाङ्क		did_emsg_before = did_emsg;
    पूर्णाङ्क		did_emsg_silent_before = did_emsg_silent;
    पूर्णाङ्क		ret = FAIL;
    sctx_T	save_current_sctx = current_sctx;
    पूर्णाङ्क		save_estack_compiling = estack_compiling;
    पूर्णाङ्क		save_cmod_flags = cmdmod.cmod_flags;
    पूर्णाङ्क		do_estack_प्रक्षिप;
    पूर्णाङ्क		new_def_function = FALSE;

    // allocated lines are freed at the end
    ga_init2(&पङ्क्तयः_to_free, परिमाण(char_u *), 50);

    // Initialize the ufunc and the compilation context
    जइ (compile_dfunc_ufunc_init(ufunc, outer_cctx, compile_type,
						&new_def_function) == FAIL)
	प्रत्यागम FAIL;

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
    do_estack_प्रक्षिप = !estack_शिखरम्_is_ufunc(ufunc, 1);
    जइ (do_estack_प्रक्षिप)
	estack_प्रक्षिप_ufunc(ufunc, 1);
    estack_compiling = TRUE;

    // Make sure arguments don't shadow variables in the context
    जइ (check_args_shadowing(ufunc, &cctx) == FAIL)
	गच्छ erret;

    // For an object method and a constructor generate instructions to
    // initialize "this" and the object variables.
    जइ (ufunc->uf_flags & (FC_OBJECT|FC_NEW))
	जइ (obj_method_prologue(ufunc, &cctx) == FAIL)
	    गच्छ erret;

    जइ (ufunc->uf_def_args.ga_len > 0)
	जइ (compile_def_function_default_args(ufunc, instr, &cctx) == FAIL)
	    गच्छ erret;
    ufunc->uf_args_visible = ufunc->uf_args.ga_len;

    // Compiling an abstract method or a function in an interface is done to
    // get the function type.  No code is actually compiled.
    जइ (ufunc->uf_class != रिक्त && (IS_INTERFACE(ufunc->uf_class)
						|| IS_ABSTRACT_METHOD(ufunc)))
    {
	ufunc->uf_def_status = UF_NOT_COMPILED;
	ret = OK;
	गच्छ erret;
    }

    // compile the function body
    जइ (compile_def_function_body(ufunc->uf_पङ्क्तयः.ga_len, check_return_type,
				&पङ्क्तयः_to_free, &errormsg, &cctx) == FAIL)
	गच्छ erret;

    जइ (compile_dfunc_scope_end_missing(&cctx))
	गच्छ erret;

    जइ (compile_dfunc_generate_default_return(ufunc, &cctx) == FAIL)
	गच्छ erret;

    // When compiled with ":silent!" and there was an error don't consider the
    // function compiled.
    जइ (emsg_silent == 0 || did_emsg_silent == did_emsg_silent_before)
	compile_dfunc_epilogue(outer_cctx, ufunc, instr, &cctx);

    ret = OK;

erret:
    जइ (ufunc->uf_def_status == UF_COMPILING)
    {
	// compilation failed. do cleanup.
	compile_dfunc_ufunc_cleanup(ufunc, instr, new_def_function,
				    errormsg, did_emsg_before, &cctx);
    }

    जइ (cctx.ctx_redir_lhs.lhs_name != रिक्त)
    {
	जइ (ret == OK)
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
    जइ (do_estack_प्रक्षिप)
	estack_निष्कासय();

    ga_clear_strings(&पङ्क्तयः_to_free);
    free_locals(&cctx);
    ga_clear(&cctx.ctx_type_राशिः);
    प्रत्यागम ret;
}

    शून्य
set_function_type(ufunc_T *ufunc)
{
    पूर्णाङ्क varargs = ufunc->uf_va_name != रिक्त;
    पूर्णाङ्क argcount = ufunc->uf_args.ga_len;

    // Create a type for the function, with the return type and any
    // argument types.
    // A vararg is included in uf_args.ga_len but not in uf_arg_types.
    // The type is included in "tt_args".
    जइ (argcount > 0 || varargs)
    {
	जइ (ufunc->uf_type_list.ga_itemsize == 0)
	    ga_init2(&ufunc->uf_type_list, परिमाण(type_T *), 10);
	ufunc->uf_func_type = alloc_func_type(ufunc->uf_ret_type,
					   argcount, &ufunc->uf_type_list);
	// Add argument types to the function type.
	जइ (func_type_add_arg_types(ufunc->uf_func_type,
				    argcount + varargs,
				    &ufunc->uf_type_list) == FAIL)
	    प्रत्यागम;
	ufunc->uf_func_type->tt_argcount = argcount + varargs;
	ufunc->uf_func_type->tt_min_argcount =
				      argcount - ufunc->uf_def_args.ga_len;
	जइ (ufunc->uf_arg_types == रिक्त)
	{
	    पूर्णाङ्क i;

	    // lambda does not have argument types.
	    प्रति (i = 0; i < argcount; ++i)
		ufunc->uf_func_type->tt_args[i] = &t_any;
	}
	अण्णहा
	    mch_memmove(ufunc->uf_func_type->tt_args,
			 ufunc->uf_arg_types, परिमाण(type_T *) * argcount);
	जइ (varargs)
	{
	    ufunc->uf_func_type->tt_args[argcount] =
		   ufunc->uf_va_type == रिक्त ? &t_list_any : ufunc->uf_va_type;
	    ufunc->uf_func_type->tt_flags = TTFLAG_VARARGS;
	}
    }
    अण्णहा
	// No arguments, can use a predefined type.
	ufunc->uf_func_type = get_func_type(ufunc->uf_ret_type,
					   argcount, &ufunc->uf_type_list);
}

/*
 * Free all instructions for "dfunc" except df_name.
 */
    स्थिर शून्य
delete_def_function_contents(dfunc_T *dfunc, पूर्णाङ्क mark_deleted)
{
    पूर्णाङ्क idx;

    // In same cases the instructions may refer to a class in which the
    // function is defined and unreferencing the class may call back here
    // recursively.  Set the df_delete_busy to avoid problems.
    जइ (dfunc->df_delete_busy)
	प्रत्यागम;
    dfunc->df_delete_busy = TRUE;

    ga_clear(&dfunc->df_def_args_isn);
    ga_clear_strings(&dfunc->df_var_names);

    जइ (dfunc->df_instr != रिक्त)
    {
	प्रति (idx = 0; idx < dfunc->df_instr_गणनम्; ++idx)
	    delete_instr(dfunc->df_instr + idx);
	VIM_CLEAR(dfunc->df_instr);
    }
    जइ (dfunc->df_instr_debug != रिक्त)
    {
	प्रति (idx = 0; idx < dfunc->df_instr_debug_गणनम्; ++idx)
	    delete_instr(dfunc->df_instr_debug + idx);
	VIM_CLEAR(dfunc->df_instr_debug);
    }
#ifdef FEAT_PROFILE
    जइ (dfunc->df_instr_prof != रिक्त)
    {
	प्रति (idx = 0; idx < dfunc->df_instr_prof_गणनम्; ++idx)
	    delete_instr(dfunc->df_instr_prof + idx);
	VIM_CLEAR(dfunc->df_instr_prof);
    }
#endif

    जइ (mark_deleted)
	dfunc->df_deleted = TRUE;
    जइ (dfunc->df_ufunc != रिक्त)
	dfunc->df_ufunc->uf_def_status = UF_NOT_COMPILED;

    dfunc->df_delete_busy = FALSE;
}

/*
 * When a user function is deleted, clear the contents of any associated def
 * function, unless another user function still uses it.
 * The position in def_functions can be re-used.
 */
    शून्य
unlink_def_function(ufunc_T *ufunc)
{
    जइ (ufunc->uf_dfunc_idx <= 0 || def_functions.ga_data == रिक्त)
	प्रत्यागम;

    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data)
	+ ufunc->uf_dfunc_idx;

    जइ (--dfunc->df_refcount <= 0)
	delete_def_function_contents(dfunc, TRUE);
    ufunc->uf_def_status = UF_NOT_COMPILED;
    ufunc->uf_dfunc_idx = 0;
    जइ (dfunc->df_ufunc == ufunc)
	dfunc->df_ufunc = रिक्त;
}

/*
 * Used when a user function refers to an existing dfunc.
 */
    शून्य
link_def_function(ufunc_T *ufunc)
{
    जइ (ufunc->uf_dfunc_idx <= 0)
	प्रत्यागम;

    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data)
	+ ufunc->uf_dfunc_idx;

    ++dfunc->df_refcount;
}

#जइ defined(EXITFREE)
/*
 * Free all functions defined with ":def".
 */
    शून्य
free_def_functions(शून्य)
{
    पूर्णाङ्क idx;

    प्रति (idx = 0; idx < def_functions.ga_len; ++idx)
    {
	dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data) + idx;

	delete_def_function_contents(dfunc, TRUE);
	vim_free(dfunc->df_name);
    }

    ga_clear(&def_functions);
}
#endif


#endif // FEAT_EVAL
