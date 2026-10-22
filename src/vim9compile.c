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

#जर defined(FEAT_EVAL)

// Functions defined with :def are stored in this growarray.
// They are never removed, so that they can be found by index.
// Deleted functions have the df_deleted flag set.
garray_T def_functions = {0, 0, आकार(dfunc_T), 50, रिक्त};

स्थिर शून्य delete_def_function_contents(dfunc_T *dfunc, पूर्णांक mark_deleted);

/*
 * Lookup variable "name" in the local scope and return it in "lvar".
 * "lvar->lv_from_outer" is incremented accordingly.
 * If "lvar" is NULL only check if the variable can be found.
 * Return FAIL if not found.
 */
    पूर्णांक
lookup_local(char_u *name, माप_प्रकार len, lvar_T *lvar, cctx_T *cctx)
{
    पूर्णांक	    idx;
    lvar_T  *lvp;

    जर (len == 0)
	परत FAIL;

    जर (((len == 4 && STRNCMP(name, "this", 4) == 0)
		|| (len == 5 && STRNCMP(name, "super", 5) == 0))
	    && cctx->ctx_ufunc != रिक्त
	    && (cctx->ctx_ufunc->uf_flags & (FC_OBJECT|FC_NEW)))
    {
	पूर्णांक is_super = *name == 's';
	जर (is_super)
	{
	    जर (name[5] != '.')
	    {
		emsg(_(e_super_must_be_followed_by_dot));
		परत FAIL;
	    }
	    जर (cctx->ctx_ufunc->uf_class != रिक्त
		    && cctx->ctx_ufunc->uf_class->class_extends == रिक्त)
	    {
		emsg(_(e_using_super_not_in_child_class));
		परत FAIL;
	    }
	}
	जर (lvar != रिक्त)
	{
	    CLEAR_POINTER(lvar);
	    lvar->lv_loop_depth = -1;
	    lvar->lv_name = (char_u *)(is_super ? "super" : "this");
	    जर (cctx->ctx_ufunc->uf_class != रिक्त)
	    {
		lvar->lv_type = &cctx->ctx_ufunc->uf_class->class_object_type;
		जर (is_super)
		{
		    type_T *type = get_type_ptr(cctx->ctx_type_list);

		    जर (type != रिक्त)
		    {
			*type = *lvar->lv_type;
			lvar->lv_type = type;
			type->tt_flags |= TTFLAG_SUPER;
		    }
		}
	    }
	}
	परत OK;
    }

    // Find local in current function scope.
    साठी (idx = 0; idx < cctx->ctx_locals.ga_len; ++idx)
    {
	lvp = ((lvar_T *)cctx->ctx_locals.ga_data) + idx;
	जर (lvp->lv_name != रिक्त
		&& STRNCMP(name, lvp->lv_name, len) == 0
					       && STRLEN(lvp->lv_name) == len)
	{
	    जर (lvar != रिक्त)
	    {
		*lvar = *lvp;
		lvar->lv_from_outer = 0;
		// If the variable was declared inside a loop set
		// lvar->lv_loop_idx and lvar->lv_loop_depth.
		get_loop_var_idx(cctx, idx, lvar);
	    }
	    परत OK;
	}
    }

    // Find local in outer function scope.
    जर (cctx->ctx_outer != रिक्त)
    {
	जर (lookup_local(name, len, lvar, cctx->ctx_outer) == OK)
	{
	    जर (lvar != रिक्त)
	    {
		cctx->ctx_outer_used = TRUE;
		++lvar->lv_from_outer;
	    }
	    परत OK;
	}
    }

    परत FAIL;
}

/*
 * Lookup an argument in the current function and an enclosing function.
 * Returns the argument index in "idxp"
 * Returns the argument type in "type"
 * Sets "gen_load_outer" to TRUE if found in outer scope.
 * Returns OK when found, FAIL otherwise.
 */
    पूर्णांक
arg_exists(
	char_u	*name,
	माप_प्रकार	len,
	पूर्णांक	*idxp,
	type_T	**type,
	पूर्णांक	*gen_load_outer,
	cctx_T	*cctx)
{
    पूर्णांक	    idx;
    char_u  *va_name;

    जर (len == 0)
	परत FAIL;
    साठी (idx = 0; idx < cctx->ctx_ufunc->uf_args_visible; ++idx)
    {
	char_u *arg = FUNCARG(cctx->ctx_ufunc, idx);

	जर (STRNCMP(name, arg, len) == 0 && arg[len] == NUL)
	{
	    जर (idxp != रिक्त)
	    {
		// Arguments are located above the frame pointer.  One further
		// if there is a vararg argument
		*idxp = idx - (cctx->ctx_ufunc->uf_args.ga_len
							    + चळत_FRAME_SIZE)
			      + (cctx->ctx_ufunc->uf_va_name != रिक्त ? -1 : 0);

		जर (cctx->ctx_ufunc->uf_arg_types != रिक्त)
		    *type = cctx->ctx_ufunc->uf_arg_types[idx];
		अन्यथा
		    *type = &t_any;
	    }
	    परत OK;
	}
    }

    va_name = cctx->ctx_ufunc->uf_va_name;
    जर (va_name != रिक्त
		    && STRNCMP(name, va_name, len) == 0 && va_name[len] == NUL)
    {
	जर (idxp != रिक्त)
	{
	    // varargs is always the last argument
	    *idxp = -चळत_FRAME_SIZE - 1;
	    *type = cctx->ctx_ufunc->uf_va_type;
	}
	परत OK;
    }

    जर (cctx->ctx_outer != रिक्त)
    {
	// Lookup the name for an argument of the outer function.
	जर (arg_exists(name, len, idxp, type, gen_load_outer, cctx->ctx_outer)
									 == OK)
	{
	    जर (gen_load_outer != रिक्त)
	    {
		++*gen_load_outer;
		cctx->ctx_outer_used = TRUE;
	    }
	    परत OK;
	}
    }

    परत FAIL;
}

/*
 * Lookup a script-local variable in the current script, possibly defined in a
 * block that contains the function "cctx->ctx_ufunc".
 * "cctx" is NULL at the script level, "cstack" is NULL in a function.
 * If "len" is <= 0 "name" must be NUL terminated.
 * Return NULL when not found.
 */
    स्थिर sallvar_T *
find_script_var(char_u *name, माप_प्रकार len, cctx_T *cctx, cstack_T *cstack)
{
    scriptitem_T    *si = SCRIPT_ITEM(current_sctx.sc_sid);
    hashitem_T	    *hi;
    पूर्णांक		    cc;
    sallvar_T	    *sav;
    ufunc_T	    *ufunc;

    // Find the list of all script variables with the right name.
    जर (len > 0)
    {
	cc = name[len];
	name[len] = NUL;
    }
    hi = hash_find(&si->sn_all_vars.dv_hashtab, name);
    जर (len > 0)
	name[len] = cc;
    जर (HASHITEM_EMPTY(hi))
	परत रिक्त;

    sav = HI2SAV(hi);
    जर (sav->sav_block_id == 0)
	// variable defined in the top script scope is always visible
	परत sav;

    जर (cctx == रिक्त)
    {
	जर (cstack == रिक्त)
	    परत रिक्त;

	// Not in a function scope, find variable with block ID equal to or
	// smaller than the current block id.  Use "cstack" to go up the block
	// scopes.
	पर्यंत (sav != रिक्त)
	{
	    पूर्णांक idx;

	    साठी (idx = cstack->cs_idx; idx >= 0; --idx)
		जर (cstack->cs_block_id[idx] == sav->sav_block_id)
		    थांबा;
	    जर (idx >= 0)
		थांबा;
	    sav = sav->sav_next;
	}
	परत sav;
    }

    // Go over the variables with this name and find one that was visible
    // from the function.
    ufunc = cctx->ctx_ufunc;
    पर्यंत (sav != रिक्त)
    {
	पूर्णांक idx;

	जर (ufunc->uf_block_depth == 0 && sav->sav_block_id == 0)
	    // If the function was defined at the script level (not inside a
	    // block), script-scope variables are always visible.
	    परत sav;

	// Go over the blocks that this function was defined in.  If the
	// variable block ID matches it was visible to the function.
	साठी (idx = 0; idx < ufunc->uf_block_depth; ++idx)
	    जर (ufunc->uf_block_ids[idx] == sav->sav_block_id)
		परत sav;
	sav = sav->sav_next;
    }

    // Not found, variable was not visible.
    परत रिक्त;
}

/*
 * If "name" can be found in the current script set it's "block_id".
 */
    शून्य
update_script_var_block_id(char_u *name, पूर्णांक block_id)
{
    scriptitem_T    *si = SCRIPT_ITEM(current_sctx.sc_sid);
    hashitem_T	    *hi;
    sallvar_T	    *sav;

    hi = hash_find(&si->sn_all_vars.dv_hashtab, name);
    जर (HASHITEM_EMPTY(hi))
	परत;
    sav = HI2SAV(hi);
    sav->sav_block_id = block_id;
}

/*
 * Lookup a variable (without s: prefix) in the current script.
 * "cctx" is NULL at the script level, "cstack" is NULL in a function.
 * Returns OK or FAIL.
 */
    पूर्णांक
script_var_exists(char_u *name, माप_प्रकार len, cctx_T *cctx, cstack_T *cstack)
{
    जर (current_sctx.sc_sid <= 0)
	परत FAIL;
    जर (current_script_is_vim9())
    {
	// Check script variables that were visible where the function was
	// defined.
	जर (find_script_var(name, len, cctx, cstack) != रिक्त)
	    परत OK;
    }
    अन्यथा
    {
	hashtab_T	*ht = &SCRIPT_VARS(current_sctx.sc_sid);
	dictitem_T	*di;
	पूर्णांक		cc;

	// Check script variables that are currently visible
	cc = name[len];
	name[len] = NUL;
	di = find_var_in_ht(ht, 0, name, TRUE);
	name[len] = cc;
	जर (di != रिक्त)
	    परत OK;
    }

    परत FAIL;
}

/*
 * Returns the index of a class method or class variable with name "name"
 * accessible in the currently compiled function.
 * If "cl_ret" is not NULL set it to the class.
 * Otherwise return -1.
 */
    स्थिर पूर्णांक
cctx_class_midx(
    cctx_T  *cctx,
    पूर्णांक	    is_method,
    char_u  *name,
    माप_प्रकार  len,
    class_T **cl_ret)
{
    जर (cctx == रिक्त || cctx->ctx_ufunc == रिक्त
	    || cctx->ctx_ufunc->uf_class == रिक्त
	    || cctx->ctx_ufunc->uf_defclass == रिक्त)
	परत -1;

    // Search for the class method or variable in the class where the calling
    // function is defined.
    class_T *cl = cctx->ctx_ufunc->uf_defclass;
    पूर्णांक m_idx = is_method ? class_method_idx(cl, name, len)
					: class_member_idx(cl, name, len);
    जर (m_idx < 0)
    {
	cl = cl->class_extends;
	पर्यंत (cl != रिक्त)
	{
	    m_idx = is_method ? class_method_idx(cl, name, len)
					: class_member_idx(cl, name, len);
	    जर (m_idx >= 0)
		थांबा;
	    cl = cl->class_extends;
	}
    }

    जर (m_idx >= 0)
    {
	जर (cl_ret != रिक्त)
	    *cl_ret = cl;
    }

    परत m_idx;
}

/*
 * Returns the index of a class method with name "name" accessible in the
 * currently compiled function.  Returns -1 if not found.  The class where the
 * method is defined is returned in "cl_ret".
 */
    पूर्णांक
cctx_class_method_idx(
    cctx_T  *cctx,
    char_u  *name,
    माप_प्रकार  len,
    class_T **cl_ret)
{
    परत cctx_class_midx(cctx, TRUE, name, len, cl_ret);
}

/*
 * Returns the index of a class variable with name "name" accessible in the
 * currently compiled function.  Returns -1 if not found.  The class where the
 * variable is defined is returned in "cl_ret".
 */
    पूर्णांक
cctx_class_member_idx(
    cctx_T  *cctx,
    char_u  *name,
    माप_प्रकार  len,
    class_T **cl_ret)
{
    परत cctx_class_midx(cctx, FALSE, name, len, cl_ret);
}

/*
 * Return TRUE if "name" is a local variable, argument, script variable or
 * imported.  Also if "name" is "this" and in a class method.
 */
    स्थिर पूर्णांक
variable_exists(char_u *name, माप_प्रकार len, cctx_T *cctx)
{
    परत (cctx != रिक्त
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
    स्थिर पूर्णांक
item_exists(char_u *name, माप_प्रकार len, पूर्णांक cmd UNUSED, cctx_T *cctx)
{
    परत variable_exists(name, len, cctx);
}

/*
 * Check if "p[len]" is already defined, either in script "import_sid" or in
 * compilation context "cctx".
 * "cctx" is NULL at the script level, "cstack" is NULL in a function.
 * Does not check the global namespace.
 * If "is_arg" is TRUE the error message is for an argument name.
 * Return FAIL and give an error if it defined.
 */
    पूर्णांक
check_defined(
	char_u	    *p,
	माप_प्रकार	    len,
	cctx_T	    *cctx,
	cstack_T    *cstack,
	पूर्णांक	    is_arg)
{
    पूर्णांक		c = p[len];
    ufunc_T	*ufunc = रिक्त;

    // underscore argument is OK
    जर (len == 1 && *p == '_')
	परत OK;

    जर (script_var_exists(p, len, cctx, cstack) == OK)
    {
	जर (is_arg)
	    semsg(_(e_argument_already_declared_in_script_str), p);
	अन्यथा
	    semsg(_(e_variable_already_declared_in_script_str), p);
	परत FAIL;
    }

    जर (cctx_class_member_idx(cctx, p, len, रिक्त) >= 0)
    {
	जर (is_arg)
	    semsg(_(e_argument_already_declared_in_class_str), p);
	अन्यथा
	    semsg(_(e_variable_already_declared_in_class_str), p);
	परत FAIL;
    }

    p[len] = NUL;
    जर ((cctx != रिक्त
		&& (lookup_local(p, len, रिक्त, cctx) == OK
		    || arg_exists(p, len, रिक्त, रिक्त, रिक्त, cctx) == OK))
	    || find_imported(p, len, FALSE) != रिक्त
	    || (ufunc = find_func_even_dead(p, 0)) != रिक्त)
    {
	// A local or script-local function can shadow a global function.
	जर (ufunc == रिक्त || ((ufunc->uf_flags & FC_DEAD) == 0
		    && (!func_is_global(ufunc)
					     || (p[0] == 'g' && p[1] == ':'))))
	{
	    जर (is_arg)
		semsg(_(e_argument_name_shadows_existing_variable_str), p);
	    अन्यथा
		semsg(_(e_name_already_defined_str), p);
	    p[len] = c;
	    परत FAIL;
	}
    }
    p[len] = c;
    परत OK;
}


/*
 * Return TRUE if "actual" could be "expected" and a runtime typecheck is to be
 * used.  Return FALSE if the types will never match.
 */
    स्थिर पूर्णांक
use_typecheck(type_T *actual, type_T *expected)
{
    जर (actual->tt_type == VAR_ANY
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
	परत TRUE;
    जर (actual->tt_type == VAR_OBJECT && expected->tt_type == VAR_OBJECT)
	परत TRUE;
    जर ((actual->tt_type == VAR_LIST
		|| actual->tt_type == VAR_TUPLE
		|| actual->tt_type == VAR_DICT)
	    && actual->tt_type == expected->tt_type)
	// This takes care of a nested list or dict.
	परत use_typecheck(actual->tt_member, expected->tt_member);
    परत FALSE;
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
    पूर्णांक
need_type_where(
	type_T	*actual,
	type_T	*expected,
	पूर्णांक	typechk_flags,	// acceptable types (type check flags)
	पूर्णांक	offset,
	where_T	where,
	cctx_T	*cctx,
	पूर्णांक	silent,
	पूर्णांक	actual_is_const)
{
    पूर्णांक ret;

    जर (expected->tt_type != VAR_CLASS && expected->tt_type != VAR_TYPEALIAS)
    {
	जर (check_type_is_मूल्य(actual) == FAIL)
	    परत FAIL;
    }

    जर (expected == &t_bool && actual != &t_bool
					&& (actual->tt_flags & TTFLAG_BOOL_OK))
    {
	// Using "0", "1" or the result of an expression with "&&" or "||" as a
	// boolean is OK but requires a conversion.
	generate_2BOOL(cctx, FALSE, offset);
	परत OK;
    }

    ret = check_type_maybe(expected, actual, FALSE, where);
    जर (ret == OK)
	परत OK;

    // If actual a constant a runtime check makes no sense.  If it's
    // null_function it is OK.
    जर (actual_is_const && ret == MAYBE && actual == &t_func_unknown)
	परत OK;

    // If the actual type can be the expected type add a runtime check.
    जर (!actual_is_const && ret == MAYBE && use_typecheck(actual, expected))
    {
	generate_TYPECHECK(cctx, expected, typechk_flags, offset,
		where.wt_kind == WT_VARIABLE, where.wt_index);
	परत OK;
    }

    जर (!silent)
	type_mismatch_where(expected, actual, where);
    परत FAIL;
}

    पूर्णांक
need_type(
	type_T	*actual,
	type_T	*expected,
	पूर्णांक	typechk_flags,	// acceptable types (type check flags)
	पूर्णांक	offset,
	पूर्णांक	arg_idx,
	cctx_T	*cctx,
	पूर्णांक	silent,
	पूर्णांक	actual_is_const)
{
    where_T where = WHERE_INIT;

    जर (arg_idx > 0)
    {
	where.wt_index = arg_idx;
	where.wt_kind = WT_ARGUMENT;
    }
    परत need_type_where(actual, expected, typechk_flags, offset, where,
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

    जर (lvar->lv_const == ASSIGN_CONST && (type->tt_flags & TTFLAG_CONST) == 0)
    {
	जर (type->tt_flags & TTFLAG_STATIC)
	    // entry in static_types[] is followed by const type
	    type = type + 1;
	अन्यथा
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
	माप_प्रकार	len,
	पूर्णांक	assign,
	type_T	*type)
{
    lvar_T  *lvar;
    dfunc_T *dfunc;

    जर (arg_exists(name, len, रिक्त, रिक्त, रिक्त, cctx) == OK)
    {
	emsg_namelen(_(e_str_is_used_as_argument), name, (पूर्णांक)len);
	परत रिक्त;
    }

    जर (GA_GROW_FAILS(&cctx->ctx_locals, 1))
	परत रिक्त;
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
    जर (type == &t_unknown || type == &t_any)
	// type not known yet, may be inferred from RHS
	lvar->lv_type = type;
    अन्यथा
	// may use TTFLAG_CONST
	set_var_type(lvar, type, cctx);

    // Remember the name for debugging.
    जर (GA_GROW_FAILS(&dfunc->df_var_names, 1))
	परत रिक्त;
    ((char_u **)dfunc->df_var_names.ga_data)[lvar->lv_idx] =
						    vim_strsave(lvar->lv_name);
    ++dfunc->df_var_names.ga_len;

    परत lvar;
}

/*
 * If "check_writable" is ASSIGN_CONST give an error if the variable was
 * defined with :final or :const, if "check_writable" is ASSIGN_FINAL give an
 * error if the variable was defined with :const.
 */
    स्थिर पूर्णांक
check_item_writable(svar_T *sv, पूर्णांक check_writable, char_u *name)
{
    जर ((check_writable == ASSIGN_CONST && sv->sv_const != 0)
	    || (check_writable == ASSIGN_FINAL
					      && sv->sv_const == ASSIGN_CONST))
    {
	semsg(_(e_cannot_change_readonly_variable_str), name);
	परत FAIL;
    }
    परत OK;
}

/*
 * Find "name" in script-local items of script "sid".
 * Pass "check_writable" to check_item_writable().
 * "cctx" is NULL at the script level, "cstack" is NULL in a function.
 * Returns the index in "sn_var_vals" if found.
 * If found but not in "sn_var_vals" returns -1.
 * If not found or the variable is not writable returns -2.
 */
    पूर्णांक
get_script_item_idx(
	पूर्णांक	    sid,
	char_u	    *name,
	पूर्णांक	    check_writable,
	cctx_T	    *cctx,
	cstack_T    *cstack)
{
    hashtab_T	    *ht;
    dictitem_T	    *di;
    scriptitem_T    *si = SCRIPT_ITEM(sid);
    svar_T	    *sv;
    पूर्णांक		    idx;

    जर (!SCRIPT_ID_VALID(sid))
	परत -1;
    जर (sid == current_sctx.sc_sid)
    {
	sallvar_T *sav = find_script_var(name, 0, cctx, cstack);

	जर (sav == रिक्त)
	    परत -2;
	idx = sav->sav_var_vals_idx;
	sv = ((svar_T *)si->sn_var_vals.ga_data) + idx;
	जर (check_item_writable(sv, check_writable, name) == FAIL)
	    परत -2;
	परत idx;
    }

    // First look the name up in the hashtable.
    ht = &SCRIPT_VARS(sid);
    di = find_var_in_ht(ht, 0, name, TRUE);
    जर (di == रिक्त)
    {
	जर (si->sn_autoload_prefix != रिक्त)
	{
	    hashitem_T *hi;

	    // A variable exported from an autoload script is in the global
	    // variables, we can find it in the all_vars table.
	    hi = hash_find(&si->sn_all_vars.dv_hashtab, name);
	    जर (!HASHITEM_EMPTY(hi))
		परत HI2SAV(hi)->sav_var_vals_idx;
	}
	परत -2;
    }

    // Now find the svar_T index in sn_var_vals.
    साठी (idx = 0; idx < si->sn_var_vals.ga_len; ++idx)
    {
	sv = ((svar_T *)si->sn_var_vals.ga_data) + idx;
	जर (sv->sv_tv == &di->di_tv)
	{
	    जर (check_item_writable(sv, check_writable, name) == FAIL)
		परत -2;
	    परत idx;
	}
    }
    परत -1;
}

    स्थिर imported_T *
find_imported_in_script(char_u *name, माप_प्रकार len, पूर्णांक sid)
{
    scriptitem_T    *si;
    पूर्णांक		    idx;

    जर (!SCRIPT_ID_VALID(sid))
	परत रिक्त;
    si = SCRIPT_ITEM(sid);
    साठी (idx = 0; idx < si->sn_imports.ga_len; ++idx)
    {
	imported_T *import = ((imported_T *)si->sn_imports.ga_data) + idx;

	जर (len == 0 ? STRCMP(name, import->imp_name) == 0
		     : STRLEN(import->imp_name) == len
				  && STRNCMP(name, import->imp_name, len) == 0)
	    परत import;
    }
    परत रिक्त;
}

/*
 * Find "name" in imported items of the current script.
 * If "len" is 0 use any length that works.
 * If "load" is TRUE and the script was not loaded yet, load it now.
 */
    imported_T *
find_imported(char_u *name, माप_प्रकार len, पूर्णांक load)
{
    जर (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	परत रिक्त;

    // Skip over "s:" before "s:something" to find the import name.
    पूर्णांक off = name[0] == 's' && name[1] == ':' ? 2 : 0;

    imported_T *ret = find_imported_in_script(name + off, len - off,
							  current_sctx.sc_sid);
    जर (ret != रिक्त && load && (ret->imp_flags & IMP_FLAGS_AUTOLOAD))
    {
	scid_T	actual_sid = 0;
	पूर्णांक	save_emsg_off = emsg_off;

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
	जर (actual_sid != 0)
	    ret->imp_sid = actual_sid;

	emsg_off = save_emsg_off;
    }
    परत ret;
}

/*
 * Called when checking for a following operator at "arg".  When the rest of
 * the line is empty or only a comment, peek the next line.  If there is a next
 * line return a pointer to it and set "nextp".
 * Otherwise skip over white space.
 */
    char_u *
may_peek_next_ओळ(cctx_T *cctx, char_u *arg, char_u **nextp)
{
    char_u *p = skipwhite(arg);

    *nextp = रिक्त;
    जर (*p == NUL || (VIM_ISWHITE(*arg) && vim9_comment_start(p)))
    {
	*nextp = peek_next_ओळ_from_context(cctx);
	जर (*nextp != रिक्त)
	    परत *nextp;
    }
    परत p;
}

/*
 * Return a pointer to the next line that isn't empty or only contains a
 * comment. Skips over white space.
 * Returns NULL if there is none.
 */
    char_u *
peek_next_ओळ_from_context(cctx_T *cctx)
{
    पूर्णांक lnum = cctx->ctx_lnum;

    पर्यंत (++lnum < cctx->ctx_ufunc->uf_ओळी.ga_len)
    {
	char_u *ओळ = ((char_u **)cctx->ctx_ufunc->uf_ओळी.ga_data)[lnum];
	char_u *p;

	// ignore NULLs inserted for continuation lines
	जर (ओळ != रिक्त)
	{
	    p = skipwhite(ओळ);
	    जर (vim9_bad_comment(p))
		परत रिक्त;
	    जर (*p != NUL && !vim9_comment_start(p))
		परत p;
	}
    }
    परत रिक्त;
}

/*
 * Get the next line of the function from "cctx".
 * Skips over empty lines.  Skips over comment lines if "skip_comment" is TRUE.
 * Returns NULL when at the end.
 */
    char_u *
next_ओळ_from_context(cctx_T *cctx, पूर्णांक skip_comment)
{
    char_u	*ओळ;

    करा
    {
	++cctx->ctx_lnum;
	जर (cctx->ctx_lnum >= cctx->ctx_ufunc->uf_ओळी.ga_len)
	{
	    ओळ = रिक्त;
	    थांबा;
	}
	ओळ = ((char_u **)cctx->ctx_ufunc->uf_ओळी.ga_data)[cctx->ctx_lnum];
	cctx->ctx_ओळ_start = ओळ;
	SOURCING_LNUM = cctx->ctx_lnum + 1;
    } पर्यंत (ओळ == रिक्त || *skipwhite(ओळ) == NUL
		     || (skip_comment && vim9_comment_start(skipwhite(ओळ))));
    परत ओळ;
}

/*
 * Skip over white space at "whitep" and assign to "*arg".
 * If "*arg" is at the end of the line, advance to the next line.
 * Also when "whitep" points to white space and "*arg" is on a "#".
 * Return FAIL if beyond the last line, "*arg" is unmodified then.
 */
    पूर्णांक
may_get_next_ओळ(char_u *whitep, char_u **arg, cctx_T *cctx)
{
    *arg = skipwhite(whitep);
    जर (vim9_bad_comment(*arg))
	परत FAIL;
    जर (**arg == NUL || (VIM_ISWHITE(*whitep) && vim9_comment_start(*arg)))
    {
	char_u *next = next_ओळ_from_context(cctx, TRUE);

	जर (next == रिक्त)
	    परत FAIL;
	*arg = skipwhite(next);
    }
    परत OK;
}

/*
 * Idem, and give an error when failed.
 */
    पूर्णांक
may_get_next_ओळ_error(char_u *whitep, char_u **arg, cctx_T *cctx)
{
    जर (may_get_next_ओळ(whitep, arg, cctx) == FAIL)
    {
	SOURCING_LNUM = cctx->ctx_lnum + 1;
	emsg(_(e_ओळ_incomplete));
	परत FAIL;
    }
    परत OK;
}

/*
 * Get a line from the compilation context, compatible with exarg_T getline().
 * Return a pointer to the line in allocated memory.
 * Return NULL for end-of-file or some error.
 */
    स्थिर char_u *
exarg_getline(
	पूर्णांक c UNUSED,
	शून्य *cookie,
	पूर्णांक indent UNUSED,
	getline_opt_T options UNUSED)
{
    cctx_T  *cctx = (cctx_T *)cookie;
    char_u  *p;

    साठी (;;)
    {
	जर (cctx->ctx_lnum >= cctx->ctx_ufunc->uf_ओळी.ga_len - 1)
	    परत रिक्त;
	++cctx->ctx_lnum;
	p = ((char_u **)cctx->ctx_ufunc->uf_ओळी.ga_data)[cctx->ctx_lnum];
	// Comment lines result in NULL pointers, skip them.
	जर (p != रिक्त)
	    परत vim_strsave(p);
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
    पूर्णांक
func_needs_compiling(ufunc_T *ufunc, compiletype_T compile_type)
{
    निवड (ufunc->uf_def_status)
    {
	स्थिती UF_TO_BE_COMPILED:
	    परत TRUE;

	स्थिती UF_COMPILED:
	{
	    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data)
							 + ufunc->uf_dfunc_idx;

	    निवड (compile_type)
	    {
		स्थिती CT_PROFILE:
#ifdef FEAT_PROFILE
		    परत dfunc->df_instr_prof == रिक्त;
#endif
		स्थिती CT_NONE:
		    परत dfunc->df_instr == रिक्त;
		स्थिती CT_DEBUG:
		    परत dfunc->df_instr_debug == रिक्त;
	    }
	}

	स्थिती UF_NOT_COMPILED:
	स्थिती UF_COMPILE_ERROR:
	स्थिती UF_COMPILING:
	    थांबा;
    }
    परत FALSE;
}

/*
 * Compile a nested :def command.
 */
    स्थिर char_u *
compile_nested_function(exarg_T *eap, cctx_T *cctx, garray_T *ओळी_to_free)
{
    पूर्णांक		is_global = *eap->arg == 'g' && eap->arg[1] == ':';
    char_u	*name_start = eap->arg;
    char_u	*name_end = to_name_end(eap->arg, TRUE);
    पूर्णांक		off;
    char_u	*func_name;
    string_T	lambda_name;
    ufunc_T	*ufunc;
    पूर्णांक		r = FAIL;
    compiletype_T   compile_type;
    पूर्णांक		funcref_isn_idx = -1;
    lvar_T	*lvar = रिक्त;
    char_u	*bracket_start = रिक्त;

    जर (eap->forceit)
    {
	semsg(_(e_cannot_use_bang_with_nested_def_str),
		eap->cmdidx == CMD_def ? ":def" : ":function");
	परत रिक्त;
    }

    जर (*name_start == '/')
    {
	name_end = skip_regexp(name_start + 1, '/', TRUE);
	जर (*name_end == '/')
	    ++name_end;
	set_nextcmd(eap, name_end);
    }

    जर (*name_end == '<')
    {
	bracket_start = name_end;
	जर (skip_generic_func_type_args(&name_end) == FAIL)
	    परत रिक्त;
    }

    जर (name_end == name_start || *skipwhite(name_end) != '(')
    {
	जर (!ends_excmd2(name_start, name_end))
	{
	    जर (*skipwhite(name_end) == '.')
		semsg(_(e_cannot_define_dict_func_in_vim9_script_str),
								     eap->cmd);
	    अन्यथा
		semsg(_(e_invalid_command_str), eap->cmd);
	    परत रिक्त;
	}

	// "def" or "def Name": list functions
	जर (generate_DEF(cctx, name_start, name_end - name_start) == FAIL)
	    परत रिक्त;
	परत eap->nextcmd == रिक्त ? (char_u *)"" : eap->nextcmd;
    }

    जर (bracket_start != रिक्त)
	// generic function.  The function name ends before the list of types
	// (opening angle bracket).
	name_end = bracket_start;

    // Only g:Func() can use a namespace.
    जर (name_start[1] == ':' && !is_global)
    {
	semsg(_(e_namespace_not_supported_str), name_start);
	परत रिक्त;
    }
    जर (cctx->ctx_skip != SKIP_YES
	    && check_defined(name_start, name_end - name_start, cctx,
							  रिक्त, FALSE) == FAIL)
	परत रिक्त;
    जर (!ASCII_ISUPPER(is_global ? name_start[2] : name_start[0]))
    {
	semsg(_(e_function_name_must_start_with_capital_str), name_start);
	परत रिक्त;
    }

    eap->arg = name_end;
    fill_exarg_from_cctx(eap, cctx);

    eap->forceit = FALSE;
    // We use the special <lambda>99 name, but it's not really a lambda.
    lambda_name = get_lambda_name();
    lambda_name.string = vim_strnsave(lambda_name.string, lambda_name.length);
    जर (lambda_name.string == रिक्त)
	परत रिक्त;

    // This may free the current line, make a copy of the name.
    off = is_global ? 2 : 0;
    func_name = vim_strnsave(name_start + off, name_end - name_start - off);
    जर (func_name == रिक्त)
    {
	r = FAIL;
	जा theend;
    }

    // A type variable cannot have the name of the function.  The function is
    // defined with the "<lambda>" name, check it here.
    जर (bracket_start != रिक्त)
    {
	माप_प्रकार	len = STRLEN(func_name);

	साठी (char_u *t = bracket_start + 1; *t != '>' && *t != NUL; )
	{
	    char_u *te = to_name_end(t, FALSE);

	    जर ((माप_प्रकार)(te - t) == len && STRNCMP(t, func_name, len) == 0)
	    {
		semsg(_(e_redefining_script_item_str), func_name);
		r = FAIL;
		जा theend;
	    }
	    t = te == t ? t + 1 : te;
	}
    }

    // Make sure "KeyTyped" is not set, it may cause indent to be written.
    पूर्णांक save_किल्ली_Typed = किल्ली_Typed;
    किल्ली_Typed = FALSE;

    ufunc = define_function(eap, lambda_name.string, ओळी_to_free, 0, रिक्त, 0,
									cctx);

    किल्ली_Typed = save_किल्ली_Typed;

    जर (ufunc == रिक्त)
    {
	r = eap->skip ? OK : FAIL;
	जा theend;
    }
    जर (eap->nextcmd != रिक्त)
    {
	semsg(_(e_text_found_after_str_str),
	      eap->cmdidx == CMD_def ? "enddef" : "endfunction", eap->nextcmd);
	r = FAIL;
	func_ptr_unref(ufunc);
	जा theend;
    }

    // copy over the block scope IDs before compiling
    जर (!is_global && cctx->ctx_ufunc->uf_block_depth > 0)
    {
	पूर्णांक block_depth = cctx->ctx_ufunc->uf_block_depth;

	ufunc->uf_block_ids = ALLOC_MULT(पूर्णांक, block_depth);
	जर (ufunc->uf_block_ids != रिक्त)
	{
	    mch_memmove(ufunc->uf_block_ids, cctx->ctx_ufunc->uf_block_ids,
						    आकार(पूर्णांक) * block_depth);
	    ufunc->uf_block_depth = block_depth;
	}
    }

    // Define the funcref before compiling, so that it is found by any
    // recursive call.
    जर (is_global)
    {
	r = generate_NEWFUNC(cctx, lambda_name.string, func_name);
	func_name = रिक्त;
	lambda_name.string = रिक्त;
	lambda_name.length = 0;
    }
    अन्यथा
    {
	// Define a local variable for the function reference.
	lvar = reserve_local(cctx, func_name, name_end - name_start,
					    ASSIGN_CONST, ufunc->uf_func_type);
	जर (lvar == रिक्त)
	{
	    func_ptr_unref(ufunc);
	    जा theend;
	}
	lvar->lv_ufunc = ufunc;
	जर (generate_FUNCREF(cctx, ufunc, रिक्त, FALSE, 0, &funcref_isn_idx) == FAIL)
	{
	    func_ptr_unref(ufunc);
	    जा theend;
	}
	r = generate_STORE(cctx, ISN_STORE, lvar->lv_idx, रिक्त);
    }

    compile_type = get_compile_type(ufunc);
#ifdef FEAT_PROFILE
    // If the outer function is profiled, also compile the nested function for
    // profiling.
    जर (cctx->ctx_compile_type == CT_PROFILE)
	compile_type = CT_PROFILE;
#endif
    जर (func_needs_compiling(ufunc, compile_type)
	    && compile_def_function(ufunc, TRUE, compile_type, cctx) == FAIL)
    {
	func_ptr_unref(ufunc);
	जर (lvar != रिक्त)
	    // Now the local variable can't be used.
	    *lvar->lv_name = '/';  // impossible value
	जा theend;
    }

#ifdef FEAT_PROFILE
    // When the outer function is compiled for profiling, the nested function
    // may be called without profiling.  Compile it here in the right context.
    जर (compile_type == CT_PROFILE && func_needs_compiling(ufunc, CT_NONE))
	compile_def_function(ufunc, FALSE, CT_NONE, cctx);
#endif

    // If a FUNCREF instruction was generated, set the index after compiling.
    जर (funcref_isn_idx != -1 && ufunc->uf_def_status == UF_COMPILED)
    {
	isn_T	*funcref_isn = ((isn_T *)cctx->ctx_instr.ga_data) +
							funcref_isn_idx;
	funcref_isn->isn_arg.funcref.fr_dfunc_idx = ufunc->uf_dfunc_idx;
    }

theend:
    vim_free(lambda_name.string);
    vim_free(func_name);
    परत r == FAIL ? रिक्त : (char_u *)"";
}

/*
 * Compile one Vim expression {expr} in string "p".
 * "p" points to the opening "{".
 * Return a pointer to the character after "}", NULL for an error.
 */
    char_u *
compile_one_अभिव्यक्ती_in_str(char_u *p, cctx_T *cctx)
{
    char_u	*block_start;
    char_u	*block_end;

    // Skip the opening {.
    block_start = skipwhite(p + 1);
    block_end = block_start;
    जर (*block_start != NUL && skip_अभिव्यक्ती(&block_end, रिक्त) == FAIL)
	परत रिक्त;
    block_end = skipwhite(block_end);
    // The block must be closed by a }.
    जर (*block_end != '}')
    {
	semsg(_(e_missing_close_curly_str), p);
	परत रिक्त;
    }
    जर (compile_expr0(&block_start, cctx) == FAIL)
	परत रिक्त;
    may_generate_2STRING(-1, TOSTRING_INTERPOLATE, cctx);

    परत block_end + 1;
}

/*
 * Compile a string "str" (either containing a literal string or a mix of
 * literal strings and Vim expressions of the form `{expr}`).  This is used
 * when compiling a heredoc assignment to a variable or an interpolated string
 * in a Vim9 def function.  Vim9 instructions are generated to push strings,
 * evaluate expressions, concatenate them and create a list of lines.  When
 * "evalstr" is TRUE, Vim expressions in "str" are evaluated.
 */
    पूर्णांक
compile_all_अभिव्यक्ती_in_str(char_u *str, पूर्णांक evalstr, cctx_T *cctx)
{
    char_u	*p = str;
    char_u	*val;
    पूर्णांक		मोजणी = 0;

    जर (cctx->ctx_skip == SKIP_YES)
	परत OK;

    जर (!evalstr || *str == NUL)
    {
	// Literal string, possibly empty.
	val = *str != NUL ? vim_strsave(str) : रिक्त;
	परत generate_PUSHS(cctx, &val);
    }

    // Push all the string pieces to the stack, followed by a ISN_CONCAT.
    पर्यंत (*p != NUL)
    {
	char_u	*lit_start;
	पूर्णांक	escaped_brace = FALSE;

	// Look for a block start.
	lit_start = p;
	पर्यंत (*p != '{' && *p != '}' && *p != NUL)
	    ++p;

	जर (*p != NUL && *p == p[1])
	{
	    // Escaped brace, unescape and continue.
	    // Include the brace in the literal string.
	    ++p;
	    escaped_brace = TRUE;
	}
	अन्यथा जर (*p == '}')
	{
	    semsg(_(e_stray_closing_curly_str), str);
	    परत FAIL;
	}

	// Append the literal part.
	जर (p != lit_start)
	{
	    val = vim_strnsave(lit_start, (माप_प्रकार)(p - lit_start));
	    जर (generate_PUSHS(cctx, &val) == FAIL)
		परत FAIL;
	    ++मोजणी;
	}

	जर (*p == NUL)
	    थांबा;

	जर (escaped_brace)
	{
	    // Skip the second brace.
	    ++p;
	    सुरूठेवा;
	}

	p = compile_one_अभिव्यक्ती_in_str(p, cctx);
	जर (p == रिक्त)
	    परत FAIL;
	++मोजणी;
    }

    // Small optimization, if there's only a single piece skip the ISN_CONCAT.
    जर (मोजणी > 1)
	परत generate_CONCAT(cctx, मोजणी);

    परत OK;
}

/*
 * Return the length of an assignment operator, or zero if there isn't one.
 */
    पूर्णांक
assignment_len(char_u *p, पूर्णांक *heredoc)
{
    जर (*p == '=')
    {
	जर (p[1] == '<' && p[2] == '<')
	{
	    *heredoc = TRUE;
	    परत 3;
	}
	परत 1;
    }
    जर (vim_strchr((char_u *)"+-*/%", *p) != रिक्त && p[1] == '=')
	परत 2;
    जर (STRNCMP(p, "..=", 3) == 0)
	परत 3;
    परत 0;
}

/*
 * Generate the load instruction for "name".
 */
    स्थिर पूर्णांक
generate_loadvar(cctx_T *cctx, lhs_T *lhs)
{
    char_u	*name = lhs->lhs_name;
    type_T	*type = lhs->lhs_type;
    पूर्णांक		res = OK;

    निवड (lhs->lhs_dest)
    {
	स्थिती dest_option:
	स्थिती dest_func_option:
	    generate_LOAD(cctx, ISN_LOADOPT, 0, name, type);
	    थांबा;
	स्थिती dest_global:
	    जर (vim_strchr(name, AUTOLOAD_CHAR) == रिक्त)
	    {
		जर (name[2] == NUL)
		    generate_instr_type(cctx, ISN_LOADGDICT, &t_dict_any);
		अन्यथा
		    generate_LOAD(cctx, ISN_LOADG, 0, name + 2, type);
	    }
	    अन्यथा
		generate_LOAD(cctx, ISN_LOADAUTO, 0, name, type);
	    थांबा;
	स्थिती dest_buffer:
	    generate_LOAD(cctx, ISN_LOADB, 0, name + 2, type);
	    थांबा;
	स्थिती dest_window:
	    generate_LOAD(cctx, ISN_LOADW, 0, name + 2, type);
	    थांबा;
	स्थिती dest_tab:
	    generate_LOAD(cctx, ISN_LOADT, 0, name + 2, type);
	    थांबा;
	स्थिती dest_script:
	स्थिती dest_script_v9:
	    res = compile_load_scriptvar(cctx,
			    name + (name[1] == ':' ? 2 : 0), रिक्त, रिक्त, रिक्त);
	    थांबा;
	स्थिती dest_env:
	    // Include $ in the name here
	    generate_LOAD(cctx, ISN_LOADENV, 0, name, type);
	    थांबा;
	स्थिती dest_reg:
	    generate_LOAD(cctx, ISN_LOADREG, name[1], रिक्त, &t_string);
	    थांबा;
	स्थिती dest_vimvar:
	    generate_LOADV(cctx, name + 2);
	    थांबा;
	स्थिती dest_local:
	    जर (cctx->ctx_skip != SKIP_YES)
	    {
		lvar_T	*lvar = lhs->lhs_lvar;
		जर (lvar->lv_from_outer > 0)
		    generate_LOADOUTER(cctx, lvar->lv_idx, lvar->lv_from_outer,
				 lvar->lv_loop_depth, lvar->lv_loop_idx, type);
		अन्यथा
		    generate_LOAD(cctx, ISN_LOAD, lvar->lv_idx, रिक्त, type);
	    }
	    थांबा;
	स्थिती dest_class_member:
	    generate_CLASSMEMBER(cctx, TRUE, lhs->lhs_class,
						     lhs->lhs_classmember_idx);
	    थांबा;
	स्थिती dest_अभिव्यक्ती:
	    // list or dict value should already be on the stack.
	    थांबा;
    }

    परत res;
}

/*
 * Skip over "[expr]" or ".member".
 * Does not check for any errors.
 */
    स्थिर char_u *
skip_index(char_u *start)
{
    char_u *p = start;

    जर (*p == '[')
    {
	p = skipwhite(p + 1);
	(शून्य)skip_अभिव्यक्ती(&p, रिक्त);
	p = skipwhite(p);
	जर (*p == ']')
	    परत p + 1;
	परत p;
    }
    // if (*p == '.')
    परत to_name_end(p + 1, TRUE);
}

    शून्य
vim9_declare_error(char_u *name)
{
    अक्षर *scope = "";

    निवड (*name)
    {
	स्थिती 'g': scope = _("global"); थांबा;
	स्थिती 'b': scope = _("buffer"); थांबा;
	स्थिती 'w': scope = _("window"); थांबा;
	स्थिती 't': scope = _("tab"); थांबा;
	स्थिती 'v': scope = "v:"; थांबा;
	स्थिती '$': semsg(_(e_cannot_declare_an_environment_variable_str), name);
		  परत;
	स्थिती '&': semsg(_(e_cannot_declare_an_option_str), name);
		  परत;
	स्थिती '@': semsg(_(e_cannot_declare_a_register_str), name);
		  परत;
	पूर्वनिर्धारित: परत;
    }
    semsg(_(e_cannot_declare_a_scope_variable_str), scope, name);
}

/*
 * Return TRUE if "name" is a valid register to use.
 * Return FALSE and give an error message if not.
 */
    स्थिर पूर्णांक
valid_dest_reg(पूर्णांक name)
{
    जर (name == '@')
       name = '"';
    जर (name == '/' || name == '=' || name == '#' || valid_yank_reg(name, TRUE))
	परत TRUE;
    emsg_invreg(name);
    परत FAIL;
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
    पूर्णांक
get_var_dest(
	char_u		*name,
	assign_dest_T	*dest,
	cmdidx_T	cmdidx,
	पूर्णांक		has_index,
	पूर्णांक		*option_scope,
	पूर्णांक		*vimvaridx,
	type_T		**type,
	cctx_T		*cctx)
{
    char_u *p;

    जर (*name == '&')
    {
	पूर्णांक		cc;
	दीर्घ		numval;
	getoption_T	opt_type;
	पूर्णांक		opt_p_flags;

	*dest = dest_option;
	जर (cmdidx == CMD_final || cmdidx == CMD_const)
	{
	    emsg(_(e_cannot_lock_option));
	    परत FAIL;
	}
	p = name;
	p = find_option_end(&p, option_scope);
	जर (p == रिक्त)
	{
	    // cannot happen?
	    emsg(_(e_unexpected_characters_in_assignment));
	    परत FAIL;
	}
	cc = *p;
	*p = NUL;
	opt_type = get_option_मूल्य(skip_option_env_lead(name),
				   &numval, रिक्त, &opt_p_flags, *option_scope);
	*p = cc;
	निवड (opt_type)
	{
	    स्थिती gov_unknown:
		    semsg(_(e_unknown_option_str), name);
		    परत FAIL;
	    स्थिती gov_string:
	    स्थिती gov_hidden_string:
		    जर (opt_p_flags & P_FUNC)
		    {
			// might be a Funcref, check the type later
			*type = &t_any;
			*dest = dest_func_option;
		    }
		    अन्यथा
		    {
			*type = &t_string;
		    }
		    थांबा;
	    स्थिती gov_bool:
	    स्थिती gov_hidden_bool:
		    *type = &t_bool;
		    थांबा;
	    स्थिती gov_संख्या:
	    स्थिती gov_hidden_संख्या:
		    *type = &t_संख्या;
		    थांबा;
	}
    }
    अन्यथा जर (*name == '$')
    {
	*dest = dest_env;
	*type = &t_string;
    }
    अन्यथा जर (*name == '@')
    {
	जर (!valid_dest_reg(name[1]))
	    परत FAIL;
	*dest = dest_reg;
	*type = name[1] == '#' ? &t_संख्या_or_string : &t_string;
    }
    अन्यथा जर (STRNCMP(name, "g:", 2) == 0)
    {
	*dest = dest_global;
    }
    अन्यथा जर (STRNCMP(name, "b:", 2) == 0)
    {
	*dest = dest_buffer;
    }
    अन्यथा जर (STRNCMP(name, "w:", 2) == 0)
    {
	*dest = dest_window;
    }
    अन्यथा जर (STRNCMP(name, "t:", 2) == 0)
    {
	*dest = dest_tab;
    }
    अन्यथा जर (STRNCMP(name, "v:", 2) == 0)
    {
	typval_T	*vtv;
	पूर्णांक		di_flags;

	*vimvaridx = find_vim_var(name + 2, &di_flags);
	जर (*vimvaridx < 0)
	{
	    semsg(_(e_variable_not_found_str), name);
	    परत FAIL;
	}
	// An item of a read-only variable can be changed, as when not compiled.
	// We use the current value of "sandbox" here, is that OK?
	जर (!has_index && var_check_ro(di_flags, name, FALSE))
	    परत FAIL;
	*dest = dest_vimvar;
	vtv = get_vim_var_tv(*vimvaridx);
	*type = typval2type_vimvar(vtv, cctx->ctx_type_list);
    }
    परत OK;
}

    स्थिर पूर्णांक
is_decl_command(cmdidx_T cmdidx)
{
    परत cmdidx == CMD_let || cmdidx == CMD_var
				 || cmdidx == CMD_final || cmdidx == CMD_const;
}

/*
 * Returns TRUE if the class or object variable in "lhs" is modifiable.
 * "var_start" points to the start of the variable name and "lhs->lhs_varlen"
 * has the total length.  Note that the "lhs" can be nested an object reference
 * (e.g.  a.b.c.d.var).
 */
    स्थिर पूर्णांक
lhs_class_member_modifiable(lhs_T *lhs, char_u	*var_start, cctx_T *cctx)
{
    माप_प्रकार	varlen = lhs->lhs_varlen;
    class_T	*cl = lhs->lhs_type->tt_class;
    पूर्णांक		is_object = lhs->lhs_type->tt_type == VAR_OBJECT;
    char_u	*name = var_start + varlen + 1;
    माप_प्रकार	namelen = lhs->lhs_end - var_start - varlen - 1;
    ocmember_T	*m;

    m = member_lookup(cl, lhs->lhs_type->tt_type, name, namelen, रिक्त);
    जर (m == रिक्त)
    {
	member_not_found_संदेश(cl, lhs->lhs_type->tt_type, name, namelen);
	परत FALSE;
    }

    जर (IS_ENUM(cl))
    {
	semsg(_(e_enumvalue_str_cannot_be_modified), cl->class_name.string,
		m->ocm_name.string);
	परत FALSE;
    }

    // If it is private member variable, then accessing it outside the
    // class is not allowed.
    // If it is a read only class variable, then it can be modified
    // only inside the class where it is defined.
    जर ((m->ocm_access != VIM_ACCESS_ALL) &&
	    ((is_object && !inside_class(cctx, cl))
	     || (!is_object && cctx->ctx_ufunc->uf_class != cl)))
    {
	अक्षर *संदेश = (m->ocm_access == VIM_ACCESS_PRIVATE)
				? e_cannot_access_protected_variable_str
				: e_variable_is_not_writable_str;
	emsg_var_cl_define(संदेश, m->ocm_name.string, 0, cl);
	परत FALSE;
    }

    परत TRUE;
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
    स्थिर पूर्णांक
lhs_find_var_end(
    lhs_T	*lhs,
    char_u	*var_start,
    पूर्णांक		is_decl,
    char_u	**var_endp)
{
    char_u  *var_end = *var_endp;

    // "lhs_dest_end" is the end of the destination, including "[expr]" or
    // ".name".
    // "var_end" is the end of the variable/option/etc. name.
    lhs->lhs_dest_end = skip_var_one(var_start, FALSE);
    जर (*var_start == '@')
    {
	जर (!valid_dest_reg(var_start[1]))
	    परत FAIL;
	var_end = var_start + 2;
    }
    अन्यथा
    {
	// skip over the leading "&", "&l:", "&g:" and "$"
	var_end = skip_option_env_lead(var_start);
	var_end = to_name_end(var_end, TRUE);
    }

    // "a: type" is declaring variable "a" with a type, not dict "a:".
    जर (is_decl && lhs->lhs_dest_end == var_start + 2
					&& lhs->lhs_dest_end[-1] == ':')
	--lhs->lhs_dest_end;
    जर (is_decl && var_end == var_start + 2 && var_end[-1] == ':')
	--var_end;

    lhs->lhs_end = lhs->lhs_dest_end;
    *var_endp = var_end;

    परत OK;
}

/*
 * Set various fields in "lhs"
 */
    स्थिर पूर्णांक
lhs_init(
    lhs_T	*lhs,
    char_u	*var_start,
    पूर्णांक		is_decl,
    पूर्णांक		heredoc,
    char_u	**var_endp)
{
    char_u *var_end = *var_endp;

    lhs_init_defaults(lhs);

    // Find the end of the variable and the destination
    जर (lhs_find_var_end(lhs, var_start, is_decl, &var_end) == FAIL)
	परत FAIL;

    // compute the length of the destination without "[expr]" or ".name"
    lhs->lhs_varlen = var_end - var_start;
    lhs->lhs_varlen_total = lhs->lhs_varlen;
    lhs->lhs_name = vim_strnsave(var_start, lhs->lhs_varlen);
    जर (lhs->lhs_name == रिक्त)
	परत FAIL;

    जर (lhs->lhs_dest_end > var_start + lhs->lhs_varlen)
	// Something follows after the variable: "var[idx]" or "var.key".
	lhs->lhs_has_index = TRUE;

    lhs->lhs_type = heredoc ? &t_list_string : &t_any;

    *var_endp = var_end;

    परत OK;
}

/*
 * Compile a LHS class variable name.
 */
    स्थिर पूर्णांक
compile_lhs_class_variable(
    cctx_T	*cctx,
    lhs_T	*lhs,
    class_T	*defcl,
    पूर्णांक		is_decl)
{
    जर (cctx->ctx_ufunc->uf_defclass != defcl)
    {
	// A class variable can be accessed without the class name
	// only inside a class.
	semsg(_(e_class_variable_str_accessible_only_inside_class_str),
		lhs->lhs_name, defcl->class_name.string);
	परत FAIL;
    }

    जर (is_decl)
    {
	semsg(_(e_variable_already_declared_in_class_str), lhs->lhs_name);
	परत FAIL;
    }

    ocmember_T	*m = &defcl->class_class_members[lhs->lhs_classmember_idx];
    जर (oc_var_check_ro(defcl, m))
	परत FAIL;

    lhs->lhs_dest = dest_class_member;
    // The class variable is defined either in the current class or
    // in one of the parent class in the hierarchy.
    lhs->lhs_class = defcl;
    lhs->lhs_type = oc_member_type_by_idx(defcl, FALSE,
						lhs->lhs_classmember_idx);

    परत OK;
}

/*
 * Compile an imported LHS variable
 */
    स्थिर पूर्णांक
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
    जर (dot == रिक्त)
    {
	semsg(_(e_no_dot_after_imported_name_str), var_start);
	परत FAIL;
    }

    p = skipwhite(dot + 1);
    var_end = to_name_end(p, TRUE);
    जर (var_end == p)
    {
	semsg(_(e_missing_name_after_imported_name_str), var_start);
	परत FAIL;
    }

    vim_free(lhs->lhs_name);
    lhs->lhs_varlen = var_end - p;
    lhs->lhs_name = vim_strnsave(p, lhs->lhs_varlen);
    जर (lhs->lhs_name == रिक्त)
	परत FAIL;
    *rawnamep = lhs->lhs_name;
    lhs->lhs_scriptvar_sid = import->imp_sid;

    // TODO: where do we check this name is exported?

    // Check if something follows: "exp.var[idx]" or
    // "exp.var.key".
    lhs->lhs_has_index = lhs->lhs_dest_end > skipwhite(var_end);

    *var_endp = var_end;

    परत OK;
}

/*
 * Process a script-local variable when compiling a LHS variable name.
 */
    स्थिर पूर्णांक
compile_lhs_script_var(
    cctx_T	*cctx,
    lhs_T	*lhs,
    char_u	*var_start,
    char_u	*var_end,
    पूर्णांक		is_decl)
{
    पूर्णांक		script_namespace = FALSE;
    पूर्णांक		script_var = FALSE;
    imported_T	*import;
    char_u	*var_name;
    माप_प्रकार	var_name_len;

    जर (lhs->lhs_varlen > 1 && STRNCMP(var_start, "s:", 2) == 0)
	script_namespace = TRUE;

    जर (script_namespace)
    {
	var_name = var_start + 2;
	var_name_len = lhs->lhs_varlen - 2;
    }
    अन्यथा
    {
	var_name = var_start;
	var_name_len = lhs->lhs_varlen;
    }

    जर (script_var_exists(var_name, var_name_len, cctx, रिक्त) == OK)
	script_var = TRUE;

    import = find_imported(var_start, lhs->lhs_varlen, FALSE);

    जर (script_namespace || script_var || import != रिक्त)
    {
	char_u *rawname = lhs->lhs_name + (lhs->lhs_name[1] == ':' ? 2 : 0);

	जर (script_namespace && current_script_is_vim9())
	{
	    semsg(_(e_cannot_use_s_colon_in_vim9_script_str), var_start);
	    परत FAIL;
	}

	जर (is_decl)
	{
	    जर (script_namespace)
		semsg(_(e_cannot_declare_script_variable_in_function_str),
			lhs->lhs_name);
	    अन्यथा
		semsg(_(e_variable_already_declared_in_script_str),
			lhs->lhs_name);
	    परत FAIL;
	}
	अन्यथा जर (cctx->ctx_ufunc->uf_script_ctx_version == SCRIPT_VERSION_VIM9
		&& script_namespace
		&& !script_var && import == रिक्त)
	{
	    semsg(_(e_unknown_variable_str), lhs->lhs_name);
	    परत FAIL;
	}

	lhs->lhs_dest = current_script_is_vim9() ? dest_script_v9 :
								dest_script;

	// existing script-local variables should have a type
	lhs->lhs_scriptvar_sid = current_sctx.sc_sid;
	जर (import != रिक्त)
	{
	    जर (compile_lhs_import_var(lhs, import, var_start, &var_end,
							&rawname) == FAIL)
		परत FAIL;
	}

	जर (SCRIPT_ID_VALID(lhs->lhs_scriptvar_sid))
	{
	    // Check writable only when no index follows.
	    lhs->lhs_scriptvar_idx = get_script_item_idx(
					lhs->lhs_scriptvar_sid, rawname,
					lhs->lhs_has_index ?  ASSIGN_FINAL :
					ASSIGN_CONST, cctx, रिक्त);
	    जर (lhs->lhs_scriptvar_idx >= 0)
	    {
		scriptitem_T *si = SCRIPT_ITEM(lhs->lhs_scriptvar_sid);
		svar_T	 *sv = ((svar_T *)si->sn_var_vals.ga_data)
						+ lhs->lhs_scriptvar_idx;

		lhs->lhs_type = sv->sv_type;
	    }
	}

	परत OK;
    }

    परत check_defined(var_start, lhs->lhs_varlen, cctx, रिक्त, FALSE);
}

/*
 * Compile the LHS destination.
 */
    स्थिर पूर्णांक
compile_lhs_var_dest(
    cctx_T	*cctx,
    lhs_T	*lhs,
    पूर्णांक		cmdidx,
    char_u	*var_start,
    char_u	*var_end,
    पूर्णांक		is_decl,
    पूर्णांक		has_cmd)	// "var" before "var_start"
{
    पूर्णांक	    declare_error = FALSE;

    जर (get_var_dest(lhs->lhs_name, &lhs->lhs_dest, cmdidx,
				lhs->lhs_has_index,
				&lhs->lhs_opt_flags, &lhs->lhs_vimvaridx,
				&lhs->lhs_type, cctx) == FAIL)
	परत FAIL;

    जर (lhs->lhs_dest != dest_local && cmdidx != CMD_const
						&& cmdidx != CMD_final)
    {
	// Specific kind of variable recognized.
	declare_error = is_decl;
    }
    अन्यथा
    {
	class_T	*defcl;

	// No specific kind of variable recognized, just a name.
	जर (check_reserved_name(lhs->lhs_name, lhs->lhs_has_index
						&& *var_end == '.') == FAIL)
	    परत FAIL;

	जर (lookup_local(var_start, lhs->lhs_varlen, &lhs->lhs_local_lvar,
								cctx) == OK)
	{
	    lhs->lhs_lvar = &lhs->lhs_local_lvar;
	}
	अन्यथा
	{
	    CLEAR_FIELD(lhs->lhs_arg_lvar);
	    जर (arg_exists(var_start, lhs->lhs_varlen,
			&lhs->lhs_arg_lvar.lv_idx, &lhs->lhs_arg_lvar.lv_type,
			&lhs->lhs_arg_lvar.lv_from_outer, cctx) == OK)
	    {
		जर (is_decl)
		{
		    semsg(_(e_str_is_used_as_argument), lhs->lhs_name);
		    परत FAIL;
		}
		lhs->lhs_lvar = &lhs->lhs_arg_lvar;
	    }
	}

	जर (lhs->lhs_lvar != रिक्त)
	{
	    जर (is_decl)
	    {
		// if we come here with what looks like an assignment like
		// .= but which has been rejected by assignment_len() from
		// may_compile_assignment give a better error message
		char_u *p = skipwhite(lhs->lhs_end);
		जर (p[0] == '.' && p[1] == '=')
		    emsg(_(e_dot_equal_not_supported_with_script_version_two));
		अन्यथा जर (p[0] == ':' && !has_cmd)
		    // type specified in an assignment without "var"
		    semsg(_(e_trailing_characters_str), p);
		अन्यथा
		    semsg(_(e_variable_already_declared_str), lhs->lhs_name);
		परत FAIL;
	    }
	}
	अन्यथा जर ((lhs->lhs_classmember_idx = cctx_class_member_idx(
			cctx, var_start, lhs->lhs_varlen, &defcl)) >= 0)
	{
	    जर (compile_lhs_class_variable(cctx, lhs, defcl, is_decl)
		    == FAIL)
		परत FAIL;
	}
	अन्यथा
	{
	    जर (compile_lhs_script_var(cctx, lhs, var_start, var_end,
			is_decl) == FAIL)
		परत FAIL;
	}
    }

    जर (declare_error)
    {
	vim9_declare_error(lhs->lhs_name);
	परत FAIL;
    }

    परत OK;
}

/*
 * When compiling a LHS variable name, for a class or an object, set the LHS
 * member type.
 */
    स्थिर पूर्णांक
compile_lhs_set_oc_member_type(
    cctx_T	*cctx,
    lhs_T	*lhs,
    char_u	*var_start)
{
    class_T	*cl = lhs->lhs_type->tt_class;
    पूर्णांक		is_object = lhs->lhs_type->tt_type == VAR_OBJECT;
    char_u	*name = var_start + lhs->lhs_varlen + 1;
    माप_प्रकार	namelen = lhs->lhs_end - var_start - lhs->lhs_varlen - 1;

    ocmember_T	*m = member_lookup(cl, lhs->lhs_type->tt_type,
	    name, namelen, &lhs->lhs_member_idx);
    जर (m == रिक्त)
    {
	member_not_found_संदेश(cl, lhs->lhs_type->tt_type, name, namelen);
	परत FAIL;
    }

    जर (IS_ENUM(cl))
    {
	जर (!inside_class(cctx, cl))
	{
	    semsg(_(e_enumvalue_str_cannot_be_modified),
		    cl->class_name.string, m->ocm_name.string);
	    परत FAIL;
	}
	जर (lhs->lhs_type->tt_type == VAR_OBJECT &&
		lhs->lhs_member_idx < 2)
	{
	    अक्षर *संदेश = lhs->lhs_member_idx == 0 ?
		e_enum_str_name_cannot_be_modified :
		e_enum_str_ordinal_cannot_be_modified;
	    semsg(_(संदेश), cl->class_name.string);
	    परत FAIL;
	}
    }

    // If it is private member variable, then accessing it outside the
    // class is not allowed.
    // If it is a read only class variable, then it can be modified
    // only inside the class where it is defined.
    जर ((m->ocm_access != VIM_ACCESS_ALL) &&
	    ((is_object && !inside_class(cctx, cl))
	     || (!is_object && cctx->ctx_ufunc->uf_defclass != cl)))
    {
	अक्षर *संदेश = (m->ocm_access == VIM_ACCESS_PRIVATE)
	    ? e_cannot_access_protected_variable_str
	    : e_variable_is_not_writable_str;
	emsg_var_cl_define(संदेश, m->ocm_name.string, 0, cl);
	परत FAIL;
    }

    जर (!IS_CONSTRUCTOR_METHOD(cctx->ctx_ufunc)
	    && oc_var_check_ro(cl, m))
	परत FAIL;

    lhs->lhs_member_type = m->ocm_type;

    परत OK;
}

/*
 * When compiling a LHS variable, set the LHS variable type.
 */
    स्थिर पूर्णांक
compile_lhs_set_type(cctx_T *cctx, lhs_T *lhs, char_u *var_end, पूर्णांक is_decl)
{
    जर (is_decl && *skipwhite(var_end) == ':')
    {
	char_u *p;

	// parse optional type: "let var: type = expr"
	जर (VIM_ISWHITE(*var_end))
	{
	    semsg(_(e_no_white_space_allowed_before_colon_str), var_end);
	    परत FAIL;
	}

	जर (!VIM_ISWHITE(var_end[1]))
	{
	    semsg(_(e_white_space_required_after_str_str), ":", var_end);
	    परत FAIL;
	}

	p = skipwhite(var_end + 1);
	lhs->lhs_type = parse_type(&p, cctx->ctx_type_list, cctx->ctx_ufunc, cctx, TRUE);
	जर (lhs->lhs_type == रिक्त
		|| !valid_declaration_type(lhs->lhs_type))
	    परत FAIL;

	lhs->lhs_has_type = TRUE;
	lhs->lhs_end = p;
    }
    अन्यथा जर (lhs->lhs_lvar != रिक्त)
	lhs->lhs_type = lhs->lhs_lvar->lv_type;

    परत OK;
}

/*
 * Returns TRUE if "lhs" is a concatenable string.
 */
    स्थिर पूर्णांक
lhs_concatenable(lhs_T *lhs)
{
    परत lhs->lhs_dest == dest_global
		|| lhs->lhs_has_index
		|| lhs->lhs_type->tt_type == VAR_STRING
		|| lhs->lhs_type->tt_type == VAR_ANY;
}

/*
 * Create a new local variable when compiling a LHS variable.
 */
    स्थिर पूर्णांक
compile_lhs_new_local_var(
    cctx_T	*cctx,
    lhs_T	*lhs,
    char_u	*var_start,
    पूर्णांक		cmdidx,
    पूर्णांक		oplen,
    पूर्णांक		is_decl,
    पूर्णांक		has_cmd,
    पूर्णांक		heredoc)
{
    जर (oplen > 1 && !heredoc)
    {
	// +=, /=, etc. require an existing variable
	semsg(_(e_cannot_use_operator_on_new_variable_str), lhs->lhs_name);
	परत FAIL;
    }

    जर (!is_decl || (lhs->lhs_has_index && !has_cmd
					&& cctx->ctx_skip != SKIP_YES))
    {
	semsg(_(e_unknown_variable_str), lhs->lhs_name);
	परत FAIL;
    }

    // Check the name is valid for a funcref.
    जर (lhs->lhs_type->tt_type == VAR_FUNC
				|| lhs->lhs_type->tt_type == VAR_PARTIAL)
    {
	जर (var_wrong_func_name(lhs->lhs_name, TRUE))
	    परत FAIL;
    }

    // New local variable.
    पूर्णांक assign;
    निवड (cmdidx)
    {
	स्थिती CMD_final:
	    assign = ASSIGN_FINAL; थांबा;
	स्थिती CMD_const:
	    assign = ASSIGN_CONST; थांबा;
	पूर्वनिर्धारित:
	    assign = ASSIGN_VAR; थांबा;
    }

    lhs->lhs_lvar = reserve_local(cctx, var_start, lhs->lhs_varlen, assign,
							lhs->lhs_type);
    जर (lhs->lhs_lvar == रिक्त)
	परत FAIL;

    lhs->lhs_new_local = TRUE;

    परत OK;
}

/*
 * When compiling a LHS variable name, set the LHS member type.
 */
    स्थिर पूर्णांक
compile_lhs_set_member_type(
    cctx_T	*cctx,
    lhs_T	*lhs,
    char_u	*var_start,
    पूर्णांक		is_decl,
    पूर्णांक		has_cmd)
{
    lhs->lhs_member_type = lhs->lhs_type;

    जर (!lhs->lhs_has_index)
	परत OK;

    char_u	*after = var_start + lhs->lhs_varlen;
    char_u	*p;

    // Something follows after the variable: "var[idx]" or "var.key".
    जर (is_decl && cctx->ctx_skip != SKIP_YES)
    {
	जर (has_cmd)
	    emsg(_(e_cannot_use_index_when_declaring_variable));
	अन्यथा
	    semsg(_(e_unknown_variable_str), lhs->lhs_name);
	परत FAIL;
    }

    // Now: var_start[lhs->lhs_varlen] is '[' or '.'
    // Only the last index is used below, if there are others
    // before it generate code for the expression.  Thus for
    // "ll[1][2]" the expression is "ll[1]" and "[2]" is the index.
    साठी (;;)
    {
	p = skip_index(after);
	जर (*p != '[' && *p != '.')
	{
	    lhs->lhs_varlen_total = p - var_start;
	    थांबा;
	}
	after = p;
    }
    जर (after > var_start + lhs->lhs_varlen)
    {
	lhs->lhs_varlen = after - var_start;
	lhs->lhs_dest = dest_अभिव्यक्ती;
	// We don't know the type before evaluating the expression,
	// use "any" until then.  The member index is for the first name,
	// not for the last index.
	lhs->lhs_type = &t_any;
	lhs->lhs_member_idx = -1;
    }

    पूर्णांक use_class = lhs->lhs_type != रिक्त
	&& (lhs->lhs_type->tt_type == VAR_CLASS
		|| lhs->lhs_type->tt_type == VAR_OBJECT);

    जर (lhs->lhs_type == रिक्त
	    || (use_class ? lhs->lhs_type->tt_class == रिक्त
		: lhs->lhs_type->tt_member == रिक्त))
    {
	lhs->lhs_member_type = &t_any;
    }
    अन्यथा जर (use_class)
    {
	// for an object or class member get the type of the member
	जर (compile_lhs_set_oc_member_type(cctx, lhs, var_start) == FAIL)
	    परत FAIL;
    }
    अन्यथा
	lhs->lhs_member_type = lhs->lhs_type->tt_member;

    परत OK;
}

/*
 * Figure out the LHS type and other properties for an assignment or one item
 * of ":unlet" with an index.
 * Returns OK or FAIL.
 */
    पूर्णांक
compile_lhs(
	char_u	    *var_start,
	lhs_T	    *lhs,
	cmdidx_T    cmdidx,
	पूर्णांक	    heredoc,
	पूर्णांक	    has_cmd,	    // "var" before "var_start"
	पूर्णांक	    oplen,
	cctx_T	    *cctx)
{
    char_u	*var_end = रिक्त;
    पूर्णांक		is_decl = is_decl_command(cmdidx);

    जर (lhs_init(lhs, var_start, is_decl, heredoc, &var_end) == FAIL)
	परत FAIL;

    जर (cctx->ctx_skip != SKIP_YES)
    {
	// compile the LHS destination
	जर (compile_lhs_var_dest(cctx, lhs, cmdidx, var_start, var_end,
						is_decl, has_cmd) == FAIL)
	    परत FAIL;
    }

    // handle "a:name" as a name, not index "name" in "a"
    जर (lhs->lhs_varlen > 1 || var_start[lhs->lhs_varlen] != ':')
	var_end = lhs->lhs_dest_end;

    जर (lhs->lhs_dest != dest_option && lhs->lhs_dest != dest_func_option)
    {
	// set the LHS variable type
	जर (compile_lhs_set_type(cctx, lhs, var_end, is_decl) == FAIL)
	    परत FAIL;
    }

    जर (oplen == 3 && !heredoc && !lhs_concatenable(lhs))
    {
	emsg(_(e_can_only_concatenate_to_string));
	परत FAIL;
    }

    जर (lhs->lhs_lvar == रिक्त && lhs->lhs_dest == dest_local
						&& cctx->ctx_skip != SKIP_YES)
    {
	जर (compile_lhs_new_local_var(cctx, lhs, var_start, cmdidx, oplen,
					is_decl, has_cmd, heredoc) == FAIL)
	    परत FAIL;
    }

    जर (compile_lhs_set_member_type(cctx, lhs, var_start, is_decl, has_cmd)
								== FAIL)
	परत FAIL;

    परत OK;
}

/*
 * Figure out the LHS and check a few errors.
 */
    पूर्णांक
compile_assign_lhs(
	char_u	    *var_start,
	lhs_T	    *lhs,
	cmdidx_T    cmdidx,
	पूर्णांक	    is_decl,
	पूर्णांक	    heredoc,
	पूर्णांक	    has_cmd,	    // "var" before "var_start"
	पूर्णांक	    oplen,
	cctx_T	    *cctx)
{
    जर (compile_lhs(var_start, lhs, cmdidx, heredoc, has_cmd, oplen, cctx)
								       == FAIL)
	परत FAIL;

    जर (!lhs->lhs_has_index && lhs->lhs_lvar == &lhs->lhs_arg_lvar)
    {
	semsg(_(e_cannot_assign_to_argument_str), lhs->lhs_name);
	परत FAIL;
    }
    जर (!is_decl && lhs->lhs_lvar != रिक्त
			   && lhs->lhs_lvar->lv_const != ASSIGN_VAR
			   && !lhs->lhs_has_index)
    {
	semsg(_(e_cannot_assign_to_constant_str), lhs->lhs_name);
	परत FAIL;
    }
    परत OK;
}

/*
 * Return TRUE if "lhs" has a range index: "[expr : expr]".
 */
    स्थिर पूर्णांक
has_list_index(char_u *idx_start, cctx_T *cctx)
{
    char_u  *p = idx_start;
    पूर्णांक	    save_skip;

    जर (*p != '[')
	परत FALSE;

    p = skipwhite(p + 1);
    जर (*p == ':')
	परत TRUE;

    save_skip = cctx->ctx_skip;
    cctx->ctx_skip = SKIP_YES;
    (शून्य)compile_expr0(&p, cctx);
    cctx->ctx_skip = save_skip;
    परत *skipwhite(p) == ':';
}

/*
 * For an assignment with an index, compile the "idx" in "var[idx]" or "key" in
 * "var.key".
 */
    स्थिर पूर्णांक
compile_assign_index(
	char_u	*var_start,
	lhs_T	*lhs,
	पूर्णांक	*range,
	cctx_T	*cctx)
{
    माप_प्रकार	varlen = lhs->lhs_varlen;
    char_u	*p;
    पूर्णांक		r = OK;
    पूर्णांक		need_white_before = TRUE;
    पूर्णांक		empty_second;

    p = var_start + varlen;
    जर (*p == '[')
    {
	p = skipwhite(p + 1);
	जर (*p == ':')
	{
	    // empty first index, push zero
	    r = generate_PUSHNR(cctx, 0);
	    need_white_before = FALSE;
	}
	अन्यथा
	    r = compile_expr0(&p, cctx);

	जर (r == OK && *skipwhite(p) == ':')
	{
	    // unlet var[idx : idx]
	    // blob[idx : idx] = value
	    *range = TRUE;
	    p = skipwhite(p);
	    empty_second = *skipwhite(p + 1) == ']';
	    जर ((need_white_before && !IS_WHITE_OR_NUL(p[-1]))
		    || (!empty_second && !IS_WHITE_OR_NUL(p[1])))
	    {
		semsg(_(e_white_space_required_before_and_after_str_at_str),
								      ":", p);
		परत FAIL;
	    }
	    p = skipwhite(p + 1);
	    जर (*p == ']')
		// empty second index, push "none"
		r = generate_PUSHSPEC(cctx, VVAL_NONE);
	    अन्यथा
		r = compile_expr0(&p, cctx);
	}

	जर (r == OK && *skipwhite(p) != ']')
	{
	    // this should not happen
	    emsg(_(e_missing_closing_square_brace));
	    r = FAIL;
	}
    }
    अन्यथा जर (lhs->lhs_member_idx >= 0)
    {
	// object member index
	r = generate_PUSHNR(cctx, lhs->lhs_member_idx);
    }
    अन्यथा // if (*p == '.')
    {
	char_u *किल्ली_end = to_name_end(p + 1, TRUE);
	char_u *किल्ली = vim_strnsave(p + 1, किल्ली_end - p - 1);

	r = generate_PUSHS(cctx, &किल्ली);
    }
    परत r;
}

/*
 * For a LHS with an index, load the variable to be indexed.
 */
    स्थिर पूर्णांक
compile_load_lhs(
	lhs_T	*lhs,
	char_u	*var_start,
	type_T	*rhs_type,
	cctx_T	*cctx)
{
    जर (lhs->lhs_dest == dest_अभिव्यक्ती)
    {
	माप_प्रकार	    varlen = lhs->lhs_varlen;
	पूर्णांक	    c = var_start[varlen];
	पूर्णांक	    ओळी_len = cctx->ctx_ufunc->uf_ओळी.ga_len;
	पूर्णांक	    res;

	// Evaluate "ll[expr]" of "ll[expr][idx]".  End the line with a NUL and
	// limit the lines array length to avoid skipping to a following line.
	var_start[varlen] = NUL;
	cctx->ctx_ufunc->uf_ओळी.ga_len = cctx->ctx_lnum + 1;
	char_u *p = var_start;
	res = compile_expr0(&p, cctx);
	var_start[varlen] = c;
	cctx->ctx_ufunc->uf_ओळी.ga_len = ओळी_len;
	जर (res == FAIL || p != var_start + varlen)
	{
	    // this should not happen
	    जर (res != FAIL)
		emsg(_(e_missing_closing_square_brace));
	    परत FAIL;
	}

	lhs->lhs_type = cctx->ctx_type_चळत.ga_len == 0 ? &t_void
						  : get_type_on_चळत(cctx, 0);

	जर (lhs->lhs_type->tt_type == VAR_CLASS
		|| (lhs->lhs_type->tt_type == VAR_OBJECT
		    && lhs->lhs_type != &t_object_any))
	{
	    // Check whether the class or object variable is modifiable
	    जर (!lhs_class_member_modifiable(lhs, var_start, cctx))
		परत FAIL;
	}

	// Now we can properly check the type.  The variable is indexed, thus
	// we need the member type.  For a class or object we don't know the
	// type yet, it depends on what member is used.
	// The top item in the stack is the Dict, followed by the key and then
	// the type of the value.
	vartype_T vartype = lhs->lhs_type->tt_type;
	type_T *member_type = lhs->lhs_type->tt_member;
	जर (rhs_type != रिक्त && member_type != रिक्त
		&& vartype != VAR_OBJECT && vartype != VAR_CLASS
		&& rhs_type != &t_void
		&& need_type(rhs_type, member_type, 0,
					    -3, 0, cctx, FALSE, FALSE) == FAIL)
	    परत FAIL;

	परत OK;
    }

    परत generate_loadvar(cctx, lhs);
}

/*
 * Produce code for loading "lhs" and also take care of an index.
 * Return OK/FAIL.
 */
    पूर्णांक
compile_load_lhs_with_index(lhs_T *lhs, char_u *var_start, cctx_T *cctx)
{
    जर (lhs->lhs_type->tt_type == VAR_OBJECT)
    {
	// "this.value": load "this" object and get the value at index for an
	// object or class member get the type of the member.
	// Also for "obj.value".
	char_u *dot = vim_strchr(var_start, '.');
	जर (dot == रिक्त)
	{
	    semsg(_(e_missing_dot_after_object_str), lhs->lhs_name);
	    परत FAIL;
	}

	class_T	*cl = lhs->lhs_type->tt_class;
	type_T	*type = oc_member_type(cl, TRUE, dot + 1,
					  lhs->lhs_end, &lhs->lhs_member_idx);
	जर (lhs->lhs_member_idx < 0)
	    परत FAIL;

	जर (dot - var_start == 4 && STRNCMP(var_start, "this", 4) == 0)
	{
	    // load "this"
	    lvar_T  *lvar = lhs->lhs_lvar;
	    पूर्णांक	    rc;

	    जर (lvar->lv_from_outer > 0)
		rc = generate_LOADOUTER(cctx, lvar->lv_idx,
			lvar->lv_from_outer, lvar->lv_loop_depth,
			lvar->lv_loop_idx, type);
	    अन्यथा
		rc = generate_LOAD(cctx, ISN_LOAD, lvar->lv_idx, रिक्त, type);

	    जर (rc == FAIL)
		परत FAIL;
	}
	अन्यथा
	{
	    // load object variable or argument
	    जर (compile_load_lhs(lhs, var_start, lhs->lhs_type, cctx) == FAIL)
		परत FAIL;
	}
	जर (IS_INTERFACE(cl))
	    परत generate_GET_ITF_MEMBER(cctx, cl, lhs->lhs_member_idx, type);
	परत generate_GET_OBJ_MEMBER(cctx, lhs->lhs_member_idx, type);
    }
    अन्यथा जर (lhs->lhs_type->tt_type == VAR_CLASS)
    {
	// "<classname>.value": load class variable "classname.value"
	char_u *dot = vim_strchr(var_start, '.');
	जर (dot == रिक्त)
	{
	    (शून्य)check_type_is_मूल्य(lhs->lhs_type);
	    परत FAIL;
	}

	class_T	*cl = lhs->lhs_type->tt_class;
	ocmember_T *m = class_member_lookup(cl, dot + 1,
						lhs->lhs_end - dot - 1,
						&lhs->lhs_member_idx);
	जर (m == रिक्त)
	    परत FAIL;

	परत generate_CLASSMEMBER(cctx, TRUE, cl, lhs->lhs_member_idx);
    }

    जर (compile_load_lhs(lhs, var_start, रिक्त, cctx) == FAIL)
	परत FAIL;

    जर (lhs->lhs_has_index)
    {
	पूर्णांक	range = FALSE;
	type_T	*type = get_type_on_चळत(cctx, 0);

	// A member of an object or class is not obtained by indexing it.
	जर (type->tt_type == VAR_CLASS
		|| (type->tt_type == VAR_OBJECT && type != &t_object_any))
	{
	    char_u *p = var_start + lhs->lhs_varlen;

	    परत compile_class_object_index(cctx, &p, type);
	}

	// Get member from list or dict.  First compile the
	// index value.
	जर (compile_assign_index(var_start, lhs, &range, cctx) == FAIL)
	    परत FAIL;
	जर (range)
	{
	    semsg(_(e_cannot_use_range_with_assignment_operator_str),
								    var_start);
	    परत FAIL;
	}

	// Get the member.
	जर (compile_member(FALSE, रिक्त, cctx) == FAIL)
	    परत FAIL;
    }
    परत OK;
}

/*
 * Assignment to a list or dict member, or ":unlet" for the item, using the
 * information in "lhs".
 * Returns OK or FAIL.
 */
    पूर्णांक
compile_assign_unlet(
    char_u	*var_start,
    lhs_T	*lhs,
    पूर्णांक		is_assign,
    type_T	*rhs_type,
    cctx_T	*cctx)
{
    vartype_T	dest_type;
    पूर्णांक		range = FALSE;

    जर (compile_assign_index(var_start, lhs, &range, cctx) == FAIL)
	परत FAIL;
    जर (is_assign && range
	    && lhs->lhs_type->tt_type != VAR_LIST
	    && lhs->lhs_type != &t_blob
	    && lhs->lhs_type != &t_any)
    {
	जर (lhs->lhs_type->tt_type == VAR_TUPLE)
	    emsg(_(e_cannot_slice_tuple));
	अन्यथा
	    semsg(_(e_cannot_use_range_with_assignment_str), var_start);
	परत FAIL;
    }

    // For "expr[idx]" the index is compiled before the expression, thus the
    // resulting type cannot be used here.
    जर (lhs->lhs_dest == dest_अभिव्यक्ती
	    || lhs->lhs_type == रिक्त || lhs->lhs_type == &t_any)
    {
	// Index on variable of unknown type: check at runtime.
	dest_type = VAR_ANY;
    }
    अन्यथा
    {
	dest_type = lhs->lhs_type->tt_type;
	जर (dest_type == VAR_DICT && range)
	{
	    emsg(_(e_cannot_use_range_with_dictionary));
	    परत FAIL;
	}
	जर (dest_type == VAR_DICT
		&& may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
	    परत FAIL;
	जर (dest_type == VAR_LIST || dest_type == VAR_BLOB)
	{
	    type_T *type;

	    जर (range)
	    {
		type = get_type_on_चळत(cctx, 1);
		जर (need_type(type, &t_संख्या, 0,
					    -2, 0, cctx, FALSE, FALSE) == FAIL)
		परत FAIL;
	    }
	    type = get_type_on_चळत(cctx, 0);
	    जर ((dest_type != VAR_BLOB && type->tt_type != VAR_SPECIAL)
		    && need_type(type, &t_संख्या, 0,
					    -1, 0, cctx, FALSE, FALSE) == FAIL)
		परत FAIL;
	}
    }

    जर (cctx->ctx_skip == SKIP_YES)
	परत OK;

    // Load the dict, list or object.  On the stack we then have:
    // - value (for assignment, not for :unlet)
    // - index
    // - for [a : b] second index
    // - variable
    जर (compile_load_lhs(lhs, var_start, rhs_type, cctx) == FAIL)
	परत FAIL;

    जर (dest_type == VAR_LIST
	    || dest_type == VAR_DICT
	    || dest_type == VAR_BLOB
	    || dest_type == VAR_CLASS
	    || dest_type == VAR_OBJECT
	    || dest_type == VAR_ANY)
    {
	जर (is_assign)
	{
	    जर (range)
	    {
		जर (generate_instr_drop(cctx, ISN_STORERANGE, 4) == रिक्त)
		    परत FAIL;
	    }
	    अन्यथा
	    {
		isn_T	*isn = generate_instr_drop(cctx, ISN_STOREINDEX, 3);

		जर (isn == रिक्त)
		    परत FAIL;
		isn->isn_arg.storeindex.si_vartype = dest_type;
		isn->isn_arg.storeindex.si_class = रिक्त;

		जर (dest_type == VAR_OBJECT)
		{
		    class_T *cl = lhs->lhs_type->tt_class;

		    जर (IS_INTERFACE(cl))
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
	अन्यथा जर (range)
	{
	    जर (generate_instr_drop(cctx, ISN_UNLETRANGE, 3) == रिक्त)
		परत FAIL;
	}
	अन्यथा
	{
	    जर (generate_instr_drop(cctx, ISN_UNLETINDEX, 2) == रिक्त)
		परत FAIL;
	}
    }
    अन्यथा
    {
	जर (dest_type == VAR_TUPLE)
	    emsg(_(e_tuple_is_immutable));
	अन्यथा
	    emsg(_(e_indexable_type_required));
	परत FAIL;
    }

    परत OK;
}

/*
 * Generate an instruction to push the default value for "vartype".
 * if "dest_local" is TRUE then for some types no instruction is generated.
 * "skip_store" is set to TRUE if no PUSH instruction is generated.
 * Returns OK or FAIL.
 */
    स्थिर पूर्णांक
ढकला_default_मूल्य(
	cctx_T	    *cctx,
	vartype_T   vartype,
	पूर्णांक	    dest_is_local,
	पूर्णांक	    *skip_store)
{
    पूर्णांक r = OK;

    निवड (vartype)
    {
	स्थिती VAR_BOOL:
	    r = generate_PUSHBOOL(cctx, VVAL_FALSE);
	    थांबा;
	स्थिती VAR_FLOAT:
	    r = generate_PUSHF(cctx, 0.0);
	    थांबा;
	स्थिती VAR_STRING:
	    r = generate_PUSHS(cctx, रिक्त);
	    थांबा;
	स्थिती VAR_BLOB:
	    r = generate_PUSHBLOB(cctx, blob_alloc());
	    थांबा;
	स्थिती VAR_FUNC:
	    r = generate_PUSHFUNC(cctx, रिक्त, &t_func_void, TRUE);
	    थांबा;
	स्थिती VAR_LIST:
	    r = generate_NEWLIST(cctx, 0, FALSE);
	    थांबा;
	स्थिती VAR_TUPLE:
	    r = generate_NEWTUPLE(cctx, 0, FALSE);
	    थांबा;
	स्थिती VAR_DICT:
	    r = generate_NEWDICT(cctx, 0, FALSE);
	    थांबा;
	स्थिती VAR_JOB:
	    r = generate_PUSHJOB(cctx);
	    थांबा;
	स्थिती VAR_CHANNEL:
	    r = generate_PUSHCHANNEL(cctx);
	    थांबा;
	स्थिती VAR_OBJECT:
	    r = generate_PUSHOBJ(cctx);
	    थांबा;
	स्थिती VAR_संख्या:
	स्थिती VAR_UNKNOWN:
	स्थिती VAR_ANY:
	स्थिती VAR_PARTIAL:
	स्थिती VAR_VOID:
	स्थिती VAR_INSTR:
	स्थिती VAR_CLASS:
	स्थिती VAR_TYPEALIAS:
	स्थिती VAR_SPECIAL:  // cannot happen
	    // This is skipped for local variables, they are always
	    // initialized to zero.  But in a "for" or "while" loop
	    // the value may have been changed.
	    जर (dest_is_local && !inside_loop_scope(cctx))
		*skip_store = TRUE;
	    अन्यथा
		r = generate_PUSHNR(cctx, 0);
	    थांबा;
    }
    परत r;
}

/*
 * Compile assignment context.  Used when compiling an assignment statement.
 */
प्रकारव्याख्या संरचना cac_S cac_T;
संरचना cac_S
{
    cmdidx_T	cac_cmdidx;		// assignment command
    char_u	*cac_nextc;		// next character to parse
    lhs_T	cac_lhs;		// lhs of the assignment
    type_T	*cac_rhs_type;		// rhs type of an assignment
    char_u	*cac_op;		// assignment operator
    पूर्णांक		cac_oplen;		// assignment operator length
    char_u	*cac_var_start;		// start of the variable names
    char_u	*cac_var_end;		// end of the variable names
    पूर्णांक		cac_var_मोजणी;		// number of variables in assignment
    पूर्णांक		cac_var_idx;		// variable index in a list
    पूर्णांक		cac_semicolon;		// semicolon in [var1, var2; var3]
    garray_T	*cac_instr;
    पूर्णांक		cac_instr_मोजणी;
    पूर्णांक		cac_incdec;
    पूर्णांक		cac_did_generate_slice;
    पूर्णांक		cac_is_decl;
    पूर्णांक		cac_is_const;
    पूर्णांक		cac_start_lnum;
    type_T	*cac_inferred_type;
    पूर्णांक		cac_skip_store;
};

/*
 * Initialize the compile assignment context.
 */
    स्थिर शून्य
compile_assign_context_init(cac_T *cac, cctx_T *cctx, पूर्णांक cmdidx, char_u *arg)
{
    CLEAR_FIELD(*cac);
    cac->cac_cmdidx = cmdidx;
    cac->cac_instr = &cctx->ctx_instr;
    cac->cac_rhs_type = &t_any;
    cac->cac_is_decl = is_decl_command(cmdidx);
    cac->cac_start_lnum = SOURCING_LNUM;
    cac->cac_instr_मोजणी = -1;
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
    स्थिर पूर्णांक
compile_assign_obj_new_arg(char_u **argp, cctx_T *cctx)
{
    char_u *arg = *argp;

    arg += 11;	    // skip "ifargisset"
    पूर्णांक def_arg_idx = getdigits(&arg);
    arg = skipwhite(arg);

    // Use a JUMP_IF_ARG_NOT_SET instruction to skip if the value was not
    // given and the default value is "v:none".
    पूर्णांक चळत_offset = चळत_FRAME_SIZE +
				(cctx->ctx_ufunc->uf_va_name != रिक्त ? 1 : 0);
    पूर्णांक def_arg_मोजणी = cctx->ctx_ufunc->uf_def_args.ga_len;
    पूर्णांक arg_offset = def_arg_idx - def_arg_मोजणी - चळत_offset;

    जर (generate_JUMP_IF_ARG(cctx, ISN_JUMP_IF_ARG_NOT_SET,
			     arg_offset) == FAIL)
	परत FAIL;

    *argp = arg;
    परत OK;
}

/*
 * Translate the increment (++) and decrement (--) operators to the
 * corresponding compound operators (+= or -=).
 *
 * Returns OK on success and FAIL on syntax error.
 */
    स्थिर पूर्णांक
translate_incdec_op(exarg_T *eap, cac_T *cac)
{
    जर (VIM_ISWHITE(eap->cmd[2]))
    {
	semsg(_(e_no_white_space_allowed_after_str_str),
		eap->cmdidx == CMD_increment ? "++" : "--", eap->cmd);
	परत FAIL;
    }
    cac->cac_op = (char_u *)(eap->cmdidx == CMD_increment ? "+=" : "-=");
    cac->cac_oplen = 2;
    cac->cac_incdec = TRUE;

    परत OK;
}

/*
 * Process the operator in an assignment statement.
 */
    स्थिर पूर्णांक
compile_assign_process_operator(
    exarg_T	*eap,
    char_u	*arg,
    cac_T	*cac,
    पूर्णांक		*heredoc,
    char_u	**retstr)
{
    *retstr = रिक्त;

    जर (eap->cmdidx == CMD_increment || eap->cmdidx == CMD_decrement)
	// Change an unary operator to a compound operator
	परत translate_incdec_op(eap, cac);

    char_u *sp = cac->cac_nextc;
    cac->cac_nextc = skipwhite(cac->cac_nextc);
    cac->cac_op = cac->cac_nextc;
    cac->cac_oplen = assignment_len(cac->cac_nextc, heredoc);

    जर (cac->cac_var_मोजणी > 0 && cac->cac_oplen == 0)
    {
	// can be something like "[1, 2]->func()"
	*retstr = arg;
	परत FAIL;
    }

    // need white space before and after the operator
    जर (cac->cac_oplen > 0 && (!VIM_ISWHITE(*sp)
		|| !IS_WHITE_OR_NUL(cac->cac_op[cac->cac_oplen])))
    {
	error_white_both(cac->cac_op, cac->cac_oplen);
	परत FAIL;
    }

    परत OK;
}

/*
 * Find the start of an assignment statement.
 */
    स्थिर char_u *
compile_assign_compute_start(char_u *arg, पूर्णांक var_मोजणी)
{
    जर (var_मोजणी > 0)
	// [var1, var2] = [val1, val2]
	// skip over the "["
	परत skipwhite(arg + 1);

    परत arg;
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
    जर (l == रिक्त)
	परत रिक्त;

    list_free(l);
    cac->cac_nextc += STRLEN(cac->cac_nextc);

    परत cac->cac_nextc;
}

/*
 * Check the type of a RHS expression in a list assignment statement.
 * The RHS expression is already compiled.  So the type is on the stack.
 */
    स्थिर पूर्णांक
compile_assign_list_check_rhs_type(cctx_T *cctx, cac_T *cac)
{
    type_T	*stacktype;

    stacktype = cctx->ctx_type_चळत.ga_len == 0 ? &t_void
						: get_type_on_चळत(cctx, 0);
    जर (stacktype->tt_type == VAR_VOID)
    {
	emsg(_(e_cannot_use_void_मूल्य));
	परत FAIL;
    }

    जर (stacktype->tt_type != VAR_LIST && stacktype->tt_type != VAR_TUPLE
					&& stacktype->tt_type != VAR_ANY)
    {
	emsg(_(e_list_or_tuple_required));
	परत FAIL;
    }

    जर (need_type(stacktype,
		  stacktype->tt_type == VAR_TUPLE ? &t_tuple_any : &t_list_any,
		  TYPECHK_TUPLE_OK, -1, 0, cctx, FALSE, FALSE) == FAIL)
	परत FAIL;

    // The check accepts both a list and a tuple. Keep "any", making it a list
    // would reject a tuple later on.
    जर (stacktype->tt_type == VAR_ANY)
	set_type_on_चळत(cctx, &t_any, 0);

    जर (stacktype->tt_type == VAR_TUPLE)
    {
	जर (stacktype->tt_argcount != 1)
	    cac->cac_rhs_type = &t_any;
	अन्यथा
	{
	    जर (stacktype->tt_flags & TTFLAG_VARARGS)
		cac->cac_rhs_type = stacktype->tt_args[0]->tt_member;
	    अन्यथा
		cac->cac_rhs_type = stacktype->tt_args[0];
	}
    }
    अन्यथा जर (stacktype->tt_member != रिक्त)
	cac->cac_rhs_type = stacktype->tt_member;

    परत OK;
}

/*
 * In a list assignment statement, if a constant list was used, check the
 * length.  Returns OK if the length check succeeds.  Returns FAIL otherwise.
 */
    स्थिर पूर्णांक
compile_assign_list_check_length(cctx_T *cctx, cac_T *cac)
{
    पूर्णांक	needed_list_len;
    पूर्णांक	did_check = FALSE;

    needed_list_len = cac->cac_semicolon
				? cac->cac_var_मोजणी - 1
				: cac->cac_var_मोजणी;
    जर (cac->cac_instr->ga_len > 0)
    {
	isn_T	*isn = ((isn_T *)cac->cac_instr->ga_data) +
						cac->cac_instr->ga_len - 1;

	जर (isn->isn_type == ISN_NEWLIST || isn->isn_type == ISN_NEWTUPLE)
	{
	    did_check = TRUE;
	    जर (cac->cac_semicolon ?
			isn->isn_arg.संख्या < needed_list_len
			: isn->isn_arg.संख्या != needed_list_len)
	    {
		semsg(_(e_expected_nr_वस्तुसंच_but_got_nr),
			needed_list_len, (पूर्णांक)isn->isn_arg.संख्या);
		परत FAIL;
	    }
	}
    }

    जर (!did_check)
	generate_CHECKLEN(cctx, needed_list_len, cac->cac_semicolon);

    परत OK;
}

/*
 * Evaluate the expression for "[var, var] = expr" assignment.
 * A line break may follow the assignment operator "=".
 */
    स्थिर char_u *
compile_assign_list_अभिव्यक्ती(cctx_T *cctx, cac_T *cac)
{
    char_u *whitep;

    whitep = cac->cac_op + cac->cac_oplen;

    जर (may_get_next_ओळ_error(whitep, &cac->cac_nextc, cctx) == FAIL)
	परत रिक्त;

    // compile RHS expression
    जर (compile_expr0(&cac->cac_nextc, cctx) == FAIL)
	परत रिक्त;

    जर (cctx->ctx_skip == SKIP_YES)
	// no need to parse more when skipping
	परत cac->cac_nextc;

    जर (compile_assign_list_check_rhs_type(cctx, cac) == FAIL)
	परत रिक्त;

    // If a constant list was used we can check the length right here.
    जर (compile_assign_list_check_length(cctx, cac) == FAIL)
	परत FAIL;

    परत cac->cac_nextc;
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
    पूर्णांक		heredoc)
{
    जर (heredoc)
    {
	cac->cac_nextc = parse_heredoc_assignment(eap, cctx, cac);
	परत cac->cac_nextc;
    }

    जर (cac->cac_var_मोजणी > 0)
    {
	// for "[var, var] = expr" evaluate the expression. The list of
	// variables are processed later.
	// A line break may follow the "=".
	cac->cac_nextc = compile_assign_list_अभिव्यक्ती(cctx, cac);
	परत cac->cac_nextc;
    }

    परत cac->cac_var_end;
}

/*
 * For "var = expr" evaluate the expression.
 */
    स्थिर पूर्णांक
compile_assign_single_eval_अभिव्यक्ती(cctx_T *cctx, cac_T *cac)
{
    पूर्णांक		ret = OK;
    char_u	*whitep;
    lhs_T	*lhs = &cac->cac_lhs;
    lvar_T	*lvp;
    lvar_T	save_lhs_lvar;

    // Compile the expression.
    जर (cac->cac_incdec)
	परत generate_PUSHNR(cctx, 1);

    // Temporarily hide the new local variable here, it is
    // not available to this expression.
    जर (lhs->lhs_new_local)
    {
	--cctx->ctx_locals.ga_len;

	// Save the local variable value (compiling the RHS expression may
	// create new local variables).
	lvp = ((lvar_T *)cctx->ctx_locals.ga_data) + cctx->ctx_locals.ga_len;
	save_lhs_lvar = *lvp;
    }
    whitep = cac->cac_op + cac->cac_oplen;

    जर (may_get_next_ओळ_error(whitep, &cac->cac_nextc, cctx) == FAIL)
    {
	जर (lhs->lhs_new_local)
	    ++cctx->ctx_locals.ga_len;
	परत FAIL;
    }

    ret = compile_expr0_ext(&cac->cac_nextc, cctx, &cac->cac_is_const);
    जर (lhs->lhs_new_local)
    {
	// Restore the local variable value.  Update lhs_lvar as the index of
	// the local variable might have changed.
	lvp = ((lvar_T *)cctx->ctx_locals.ga_data) + cctx->ctx_locals.ga_len;
	*lvp = save_lhs_lvar;
	lhs->lhs_lvar = lvp;

	++cctx->ctx_locals.ga_len;
    }

    परत ret;
}

/*
 * When compiling an assignment, set the LHS type to the RHS type.
 */
    स्थिर पूर्णांक
compile_assign_set_lhs_type_from_rhs(
    cctx_T	*cctx,
    cac_T	*cac,
    lhs_T	*lhs,
    type_T	*rhs_type)
{
    जर (rhs_type->tt_type == VAR_VOID)
    {
	emsg(_(e_cannot_use_void_मूल्य));
	परत FAIL;
    }

    type_T *type;

    // An empty list or dict has a &t_unknown member, for a variable that
    // implies &t_any.
    जर (rhs_type == &t_list_empty)
	type = &t_list_any;
    अन्यथा जर (rhs_type == &t_dict_empty)
	type = &t_dict_any;
    अन्यथा जर (rhs_type == &t_unknown)
	type = &t_any;
    अन्यथा
    {
	type = rhs_type;
	cac->cac_inferred_type = rhs_type;
    }

    set_var_type(lhs->lhs_lvar, type, cctx);

    परत OK;
}

/*
 * Returns TRUE if the "rhs_type" can be assigned to the "lhs" variable.
 * Used when compiling an assignment statement.
 */
    स्थिर पूर्णांक
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
    जर (cac->cac_var_मोजणी > 0)
    {
	where.wt_index = cac->cac_var_idx + 1;
	where.wt_kind = WT_VARIABLE;
    }

    // If assigning to a list or dict member, use the member type.
    // Not for "list[:] =".
    जर (lhs->lhs_has_index &&
	    !has_list_index(cac->cac_var_start + lhs->lhs_varlen, cctx))
	use_type = lhs->lhs_member_type;

    जर (need_type_where(rhs_type, use_type, 0, -1, where, cctx, FALSE,
						cac->cac_is_const) == FAIL)
	परत FALSE;

    परत TRUE;
}

/*
 * Compare the LHS type with the RHS type in an assignment.
 */
    स्थिर पूर्णांक
compile_assign_check_type(cctx_T *cctx, cac_T *cac)
{
    lhs_T	*lhs = &cac->cac_lhs;
    type_T	*rhs_type;

    rhs_type = cctx->ctx_type_चळत.ga_len == 0
					? &t_void
					: get_type_on_चळत(cctx, 0);
    cac->cac_rhs_type = rhs_type;

    जर (check_type_is_मूल्य(rhs_type) == FAIL)
	परत FAIL;

    जर (lhs->lhs_lvar != रिक्त && (cac->cac_is_decl || !lhs->lhs_has_type))
    {
	जर (rhs_type->tt_type == VAR_FUNC
					|| rhs_type->tt_type == VAR_PARTIAL)
	{
	    // Make sure the variable name can be used as a funcref
	    जर (!lhs->lhs_has_index
				&& var_wrong_func_name(lhs->lhs_name, TRUE))
		परत FAIL;
	}

	जर (lhs->lhs_new_local && !lhs->lhs_has_type)
	{
	    // The LHS variable doesn't have a type.  Set it to the RHS type.
	    जर (compile_assign_set_lhs_type_from_rhs(cctx, cac, lhs, rhs_type)
								== FAIL)
		परत FAIL;
	}
	अन्यथा जर (*cac->cac_op == '=')
	{
	    जर (!compile_assign_valid_rhs_type(cctx, cac, lhs, rhs_type))
		परत FAIL;
	}
    }
    अन्यथा
    {
	// Assigning to a register using @r = "abc"

	type_T *lhs_type = lhs->lhs_member_type;

	// Special case: assigning to @# can use a number or a string.
	// Also: can assign a number to a float.
	जर ((lhs_type == &t_संख्या_or_string || lhs_type == &t_float)
					&& rhs_type->tt_type == VAR_संख्या)
	    lhs_type = &t_संख्या;

	जर (*cac->cac_nextc != '=')
	{
	    जर (need_type(rhs_type, lhs_type, 0, -1, 0, cctx, FALSE,
							FALSE) == FAIL)
		परत FAIL;
	}
    }

    परत OK;
}

/*
 * Compile the RHS expression in an assignment statement and generate the
 * instructions.
 */
    स्थिर पूर्णांक
compile_assign_rhs_अभिव्यक्ती(cctx_T *cctx, cac_T *cac)
{
    cac->cac_is_const = FALSE;

    // for "+=", "*=", "..=" etc. first load the current value
    जर (*cac->cac_op != '='
	    && compile_load_lhs_with_index(&cac->cac_lhs, cac->cac_var_start,
								cctx) == FAIL)
	परत FAIL;

    // For "var = expr" evaluate the expression.
    जर (cac->cac_var_मोजणी == 0)
    {
	पूर्णांक	ret;

	// Compile the expression.
	cac->cac_instr_मोजणी = cac->cac_instr->ga_len;
	ret = compile_assign_single_eval_अभिव्यक्ती(cctx, cac);
	जर (ret == FAIL)
	    परत FAIL;
    }
    अन्यथा जर (cac->cac_semicolon && cac->cac_var_idx == cac->cac_var_मोजणी - 1)
    {
	// For "[var; var] = expr" get the rest of the list
	cac->cac_did_generate_slice = TRUE;
	जर (generate_SLICE(cctx, cac->cac_var_मोजणी - 1) == FAIL)
	    परत FAIL;
    }
    अन्यथा
    {
	// For "[var, var] = expr" get the "var_idx" item from the
	// list.
	पूर्णांक with_op = *cac->cac_op != '=';
	जर (generate_GETITEM(cctx, cac->cac_var_idx, with_op) == FAIL)
	    परत FAIL;
    }

    जर (compile_assign_check_type(cctx, cac) == FAIL)
	परत FAIL;

    परत OK;
}

/*
 * Compile the RHS expression in an assignment
 */
    स्थिर पूर्णांक
compile_assign_rhs(cctx_T *cctx, cac_T *cac)
{
    lhs_T	*lhs = &cac->cac_lhs;

    जर (cctx->ctx_skip == SKIP_YES)
    {
	जर (cac->cac_oplen > 0 && cac->cac_var_मोजणी == 0)
	{
	    // skip over the "=" and the expression
	    cac->cac_nextc = skipwhite(cac->cac_op + cac->cac_oplen);
	    (शून्य)compile_expr0(&cac->cac_nextc, cctx);
	}
	परत OK;
    }

    // If RHS is specified, then generate instructions for RHS expression
    जर (cac->cac_oplen > 0)
	परत compile_assign_rhs_अभिव्यक्ती(cctx, cac);

    जर (cac->cac_cmdidx == CMD_final)
    {
	emsg(_(e_final_requires_a_मूल्य));
	परत FAIL;
    }

    जर (cac->cac_cmdidx == CMD_const)
    {
	emsg(_(e_const_requires_a_मूल्य));
	परत FAIL;
    }

    जर (!lhs->lhs_has_type || lhs->lhs_dest == dest_option
					|| lhs->lhs_dest == dest_func_option)
    {
	emsg(_(e_type_or_initialization_required));
	परत FAIL;
    }

    // variables are always initialized
    जर (GA_GROW_FAILS(cac->cac_instr, 1))
	परत FAIL;

    cac->cac_instr_मोजणी = cac->cac_instr->ga_len;

    परत ढकला_default_मूल्य(cctx, lhs->lhs_member_type->tt_type,
			      lhs->lhs_dest == dest_local,
			      &cac->cac_skip_store);
}

/*
 * Returns OK if "type" supports compound operator "op_arg" (e.g. +=, -=, %=,
 * etc.).  Compound operators are not supported with a tuple and a dict.
 * Returns FAIL if compound operator is not supported.
 */
    स्थिर पूर्णांक
check_type_supports_compound_op(type_T *type, char_u op_arg)
{
    जर (type->tt_type == VAR_TUPLE || type->tt_type == VAR_DICT)
    {
	char_u	op[2];

	op[0] = op_arg;
	op[1] = NUL;
	semsg(_(e_wrong_variable_type_for_str_equal), op);
	परत FAIL;
    }

    परत OK;
}

/*
 * Compile a compound op assignment statement (+=, -=, *=, %=, etc.)
 */
    स्थिर पूर्णांक
compile_assign_compound_op(cctx_T *cctx, cac_T *cac)
{
    lhs_T	    *lhs = &cac->cac_lhs;
    type_T	    *expected;
    type_T	    *stacktype = रिक्त;

    जर (cac->cac_lhs.lhs_type->tt_type == VAR_TUPLE
	    && check_type_supports_compound_op(cac->cac_lhs.lhs_type,
							*cac->cac_op) == FAIL)
	परत FAIL;

    जर (*cac->cac_op == '.')
    {
	expected = lhs->lhs_member_type;
	stacktype = get_type_on_चळत(cctx, 0);
	जर (expected != &t_string
		&& need_type(stacktype, expected, 0, -1, 0, cctx,
					FALSE, FALSE) == FAIL)
	    परत FAIL;
	अन्यथा जर (may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
	    परत FAIL;
    }
    अन्यथा
    {
	expected = lhs->lhs_member_type;
	stacktype = get_type_on_चळत(cctx, 0);

	जर (check_type_supports_compound_op(expected, *cac->cac_op) == FAIL)
	    परत FAIL;

	जर (
		// If variable is float operation with number is OK.
		!(expected == &t_float && (stacktype == &t_संख्या
					|| stacktype == &t_संख्या_bool))
		&& need_type(stacktype, expected, TYPECHK_संख्या_OK, -1, 0,
						cctx, FALSE, FALSE) == FAIL)
	    परत FAIL;
    }

    जर (*cac->cac_op == '.')
    {
	जर (generate_CONCAT(cctx, 2) == FAIL)
	    परत FAIL;
    }
    अन्यथा जर (*cac->cac_op == '+')
    {
	जर (generate_add_instr(cctx,
		    operator_type(lhs->lhs_member_type, stacktype),
		    lhs->lhs_member_type, stacktype,
		    अभिव्यक्ती_APPEND) == FAIL)
	    परत FAIL;
    }
    अन्यथा जर (generate_two_op(cctx, cac->cac_op) == FAIL)
	परत FAIL;

    परत OK;
}

/*
 * Generate the STORE and SETTYPE instructions for an assignment statement.
 */
    स्थिर पूर्णांक
compile_assign_generate_store(cctx_T *cctx, cac_T *cac)
{
    lhs_T	*lhs = &cac->cac_lhs;
    पूर्णांक		save_lnum;

    // Use the line number of the assignment for store instruction.
    save_lnum = cctx->ctx_lnum;
    cctx->ctx_lnum = cac->cac_start_lnum - 1;

    जर (lhs->lhs_has_index)
    {
	// Use the info in "lhs" to store the value at the index in the
	// list, dict or object.
	जर (compile_assign_unlet(cac->cac_var_start, &cac->cac_lhs,
				 TRUE, cac->cac_rhs_type, cctx) == FAIL)
	{
	    cctx->ctx_lnum = save_lnum;
	    परत FAIL;
	}
    }
    अन्यथा
    {
	जर (cac->cac_is_decl && cac->cac_cmdidx == CMD_const &&
			(lhs->lhs_dest == dest_script
			 || lhs->lhs_dest == dest_script_v9
			 || lhs->lhs_dest == dest_global
			 || lhs->lhs_dest == dest_local))
	    // ":const var": lock the value, but not referenced variables
	    generate_LOCKCONST(cctx);

	type_T	*inferred_type = cac->cac_inferred_type;

	जर ((lhs->lhs_type->tt_type == VAR_DICT
		    || lhs->lhs_type->tt_type == VAR_LIST)
		&& lhs->lhs_type->tt_member != रिक्त
		&& lhs->lhs_type->tt_member != &t_any
		&& lhs->lhs_type->tt_member != &t_unknown)
	    // Set the type in the list or dict, so that it can be
	    // checked, also in legacy script.
	    generate_SETTYPE(cctx, lhs->lhs_type);
	अन्यथा जर (lhs->lhs_type->tt_type == VAR_TUPLE
					&& lhs->lhs_type->tt_argcount != 0)
	    generate_SETTYPE(cctx, lhs->lhs_type);
	अन्यथा जर (inferred_type != रिक्त
		&& (inferred_type->tt_type == VAR_DICT
		    || inferred_type->tt_type == VAR_LIST)
		&& inferred_type->tt_member != रिक्त
		&& inferred_type->tt_member != &t_unknown
		&& inferred_type->tt_member != &t_any)
	    // Set the type in the list or dict, so that it can be
	    // checked, also in legacy script.
	    generate_SETTYPE(cctx, inferred_type);
	अन्यथा जर (inferred_type != रिक्त
				&& inferred_type->tt_type == VAR_TUPLE
				&& inferred_type->tt_argcount > 0)
	    generate_SETTYPE(cctx, inferred_type);

	जर (!cac->cac_skip_store &&
		generate_store_lhs(cctx, &cac->cac_lhs,
				   cac->cac_instr_मोजणी,
				   cac->cac_is_decl) == FAIL)
	{
	    cctx->ctx_lnum = save_lnum;
	    परत FAIL;
	}
    }

    cctx->ctx_lnum = save_lnum;

    परत OK;
}

/*
 * Process the variable(s) in an assignment statement
 */
    स्थिर पूर्णांक
compile_assign_process_variables(
    cctx_T	*cctx,
    cac_T	*cac,
    पूर्णांक		cmdidx,
    पूर्णांक		heredoc,
    पूर्णांक		has_cmd,
    पूर्णांक		has_argisset_prefix,
    पूर्णांक		jump_instr_idx)
{
    /*
     * Loop over variables in "[var, var] = expr".
     * For "name = expr" and "var name: type" this is done only once.
     */
    साठी (cac->cac_var_idx = 0; cac->cac_var_idx == 0 ||
	    cac->cac_var_idx < cac->cac_var_मोजणी; cac->cac_var_idx++)
    {
	जर (cac->cac_var_start[0] == '_'
				&& !eval_isnamec(cac->cac_var_start[1]))
	{
	    // Ignore underscore in "[a, _, b] = list".
	    जर (cac->cac_var_मोजणी > 0)
	    {
		cac->cac_var_start = skipwhite(cac->cac_var_start + 2);
		सुरूठेवा;
	    }
	    emsg(_(e_cannot_use_underscore_here));
	    परत FAIL;
	}
	vim_free(cac->cac_lhs.lhs_name);

	/*
	 * Figure out the LHS type and other properties.
	 */
	जर (compile_assign_lhs(cac->cac_var_start, &cac->cac_lhs, cmdidx,
		    cac->cac_is_decl, heredoc, has_cmd,
		    cac->cac_oplen, cctx) == FAIL)
	    परत FAIL;

	// Compile the RHS expression
	जर (heredoc)
	{
	    SOURCING_LNUM = cac->cac_start_lnum;
	    जर (cac->cac_lhs.lhs_has_type
		    && need_type(&t_list_string, cac->cac_lhs.lhs_type,
				0, -1, 0, cctx, FALSE, FALSE) == FAIL)
		परत FAIL;
	}
	अन्यथा
	{
	    जर (compile_assign_rhs(cctx, cac) == FAIL)
		परत FAIL;
	    जर (cac->cac_var_मोजणी == 0)
		cac->cac_var_end = cac->cac_nextc;
	}

	// no need to parse more when skipping
	जर (cctx->ctx_skip == SKIP_YES)
	    थांबा;

	जर (cac->cac_oplen > 0 && *cac->cac_op != '=')
	{
	    जर (compile_assign_compound_op(cctx, cac) == FAIL)
		परत FAIL;
	}

	// generate the store instructions
	जर (compile_assign_generate_store(cctx, cac) == FAIL)
	    परत FAIL;

	जर (cac->cac_var_idx + 1 < cac->cac_var_मोजणी)
	    cac->cac_var_start = skipwhite(cac->cac_lhs.lhs_end + 1);

	जर (has_argisset_prefix)
	{
	    // set instruction index in JUMP_IF_ARG_SET to here
	    isn_T *isn = ((isn_T *)cac->cac_instr->ga_data) + jump_instr_idx;
	    isn->isn_arg.jumparg.jump_where = cac->cac_instr->ga_len;
	}
    }

    परत OK;
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
    पूर्णांक		heredoc = FALSE;
    पूर्णांक		jump_instr_idx;

    compile_assign_context_init(&cac, cctx, cmdidx, arg);

    jump_instr_idx = cac.cac_instr->ga_len;

    // process object variable initialization in a new() constructor method
    पूर्णांक	has_argisset_prefix = STRNCMP(arg, "ifargisset ", 11) == 0;
    जर (has_argisset_prefix &&
			compile_assign_obj_new_arg(&arg, cctx) == FAIL)
	जा theend;

    // Skip over the "varname" or "[varname, varname]" to get to any "=".
    cac.cac_nextc = skip_var_list(arg, TRUE, &cac.cac_var_मोजणी,
						&cac.cac_semicolon, TRUE);
    जर (cac.cac_nextc == रिक्त)
	परत *arg == '[' ? arg : रिक्त;

    जर (compile_assign_process_operator(eap, arg, &cac, &heredoc,
							&retstr) == FAIL)
	परत retstr;

    // Compute the start of the assignment
    cac.cac_var_start = compile_assign_compute_start(arg, cac.cac_var_मोजणी);

    // Compute the end of the assignment
    cac.cac_var_end = compile_assign_compute_end(eap, cctx, &cac, heredoc);
    जर (cac.cac_var_end == रिक्त)
	परत रिक्त;

    पूर्णांक has_cmd = cac.cac_var_start > eap->cmd;

    /* process the variable(s) */
    जर (compile_assign_process_variables(cctx, &cac, cmdidx, heredoc,
					 has_cmd, has_argisset_prefix,
					 jump_instr_idx) == FAIL)
	जा theend;

    // For "[var, var] = expr" drop the "expr" value.
    // Also for "[var, var; _] = expr".
    जर (cctx->ctx_skip != SKIP_YES && cac.cac_var_मोजणी > 0 &&
	    (!cac.cac_semicolon || !cac.cac_did_generate_slice))
    {
	जर (generate_instr_drop(cctx, ISN_DROP, 1) == रिक्त)
	    जा theend;
    }

    retstr = skipwhite(cac.cac_var_end);

theend:
    vim_free(cac.cac_lhs.lhs_name);
    परत retstr;
}

/*
 * Check for an assignment at "eap->cmd", compile it if found.
 * Return NOTDONE if there is none, FAIL for failure, OK if done.
 */
    स्थिर पूर्णांक
may_compile_assignment(exarg_T *eap, char_u **ओळ, cctx_T *cctx)
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
    जर (p > eap->cmd && *p != NUL)
    {
	char_u *var_end;
	पूर्णांक	oplen;
	पूर्णांक	heredoc;

	जर (eap->cmd[0] == '@')
	    var_end = eap->cmd + 2;
	अन्यथा
	    var_end = find_name_end(pskip, रिक्त, रिक्त,
					FNE_CHECK_START | FNE_INCL_BR);
	oplen = assignment_len(skipwhite(var_end), &heredoc);
	जर (oplen > 0)
	{
	    माप_प्रकार len = p - eap->cmd;

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
	    जर (*eap->cmd == '&'
		    || *eap->cmd == '$'
		    || *eap->cmd == '@'
		    || ((len) > 2 && eap->cmd[1] == ':')
		    || STRNCMP(eap->cmd, "g:[", 3) == 0
		    || variable_exists(eap->cmd, len, cctx))
	    {
		*ओळ = compile_assignment(eap->cmd, eap, CMD_SIZE, cctx);
		जर (*ओळ == रिक्त || *ओळ == eap->cmd)
		    परत FAIL;
		परत OK;
	    }
	}
    }

    // might be "[var, var] = expr" or "ifargisset this.member = expr"
    जर (*eap->cmd == '[' || STRNCMP(eap->cmd, "ifargisset ", 11) == 0)
    {
	*ओळ = compile_assignment(eap->cmd, eap, CMD_SIZE, cctx);
	जर (*ओळ == रिक्त)
	    परत FAIL;
	जर (*ओळ != eap->cmd)
	    परत OK;
    }
    परत NOTDONE;
}

/*
 * Check if arguments of "ufunc" shadow variables in "cctx".
 * Return OK or FAIL.
 */
    स्थिर पूर्णांक
check_args_shadowing(ufunc_T *ufunc, cctx_T *cctx)
{
    पूर्णांक	    i;
    char_u  *arg;
    पूर्णांक	    r = OK;

    // Make sure arguments are not found when compiling a second time.
    ufunc->uf_args_visible = 0;

    // Check for arguments shadowing variables from the context.
    साठी (i = 0; i < ufunc->uf_args.ga_len; ++i)
    {
	arg = ((char_u **)(ufunc->uf_args.ga_data))[i];
	जर (check_defined(arg, STRLEN(arg), cctx, रिक्त, TRUE) == FAIL)
	{
	    r = FAIL;
	    थांबा;
	}
    }
    ufunc->uf_args_visible = ufunc->uf_args.ga_len;
    परत r;
}

#ifdef HAS_MESSAGE_WINDOW
/*
 * Get a count before a command.  Can only be a number.
 * Returns zero if there is no count.
 * Returns -1 if there is something wrong.
 */
    स्थिर दीर्घ
get_cmd_मोजणी(char_u *ओळ, exarg_T *eap)
{
    char_u *p;

    // skip over colons and white space
    साठी (p = ओळ; *p == ':' || VIM_ISWHITE(*p); ++p)
	;
    जर (!SAFE_isdigit(*p))
    {
	// The command or modifiers must be following.  Assume a lower case
	// character means there is a modifier.
	जर (p < eap->cmd && !vim_islower(*p))
	{
	    emsg(_(e_invalid_range));
	    परत -1;
	}
	परत 0;
    }
    परत म_से_द((अक्षर *)p);
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

    जर (debug_break_level > 0 || may_break_in_function(ufunc))
	परत CT_DEBUG;
#ifdef FEAT_PROFILE
    जर (do_profiling == PROF_YES)
    {
	जर (!ufunc->uf_profiling && has_profiling(FALSE, ufunc->uf_name, रिक्त,
							    &ufunc->uf_hash))
	    func_do_profile(ufunc);
	जर (ufunc->uf_profiling)
	    परत CT_PROFILE;
    }
#endif
    परत CT_NONE;
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

    निवड (compile_type)
    {
	स्थिती CT_PROFILE:
#ifdef FEAT_PROFILE
	    instr_dest = dfunc->df_instr_prof; थांबा;
#endif
	स्थिती CT_NONE:   instr_dest = dfunc->df_instr; थांबा;
	स्थिती CT_DEBUG:  instr_dest = dfunc->df_instr_debug; थांबा;
    }

    जर (instr_dest != रिक्त)
	// Was compiled in this mode before: Free old instructions.
	delete_def_function_contents(dfunc, FALSE);

    ga_clear_strings(&dfunc->df_var_names);
    dfunc->df_defer_var_idx = 0;
}

/*
 * Add a function to the list of :def functions.
 * This sets "ufunc->uf_dfunc_idx" but the function isn't compiled yet.
 */
    स्थिर पूर्णांक
add_def_function(ufunc_T *ufunc)
{
    dfunc_T *dfunc;

    जर (def_functions.ga_len == 0)
    {
	// The first position is not used, so that a zero uf_dfunc_idx means it
	// wasn't set.
	जर (GA_GROW_FAILS(&def_functions, 1))
	    परत FAIL;
	++def_functions.ga_len;
    }

    // Add the function to "def_functions".
    जर (GA_GROW_FAILS(&def_functions, 1))
	परत FAIL;
    dfunc = ((dfunc_T *)def_functions.ga_data) + def_functions.ga_len;
    CLEAR_POINTER(dfunc);
    dfunc->df_idx = def_functions.ga_len;
    ufunc->uf_dfunc_idx = dfunc->df_idx;
    dfunc->df_ufunc = ufunc;
    dfunc->df_name = vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
    ga_init2(&dfunc->df_var_names, आकार(char_u *), 10);
    ++dfunc->df_refcount;
    ++def_functions.ga_len;
    परत OK;
}

    स्थिर पूर्णांक
compile_dfunc_ufunc_init(
    ufunc_T		*ufunc,
    cctx_T		*outer_cctx,
    compiletype_T	compile_type,
    पूर्णांक			*new_def_function)
{
    // When using a function that was compiled before: Free old instructions.
    // The index is reused.  Otherwise add a new entry in "def_functions".
    जर (ufunc->uf_dfunc_idx > 0)
	clear_def_function(ufunc, compile_type);
    अन्यथा
    {
	जर (add_def_function(ufunc) == FAIL)
	    परत FAIL;

	*new_def_function = TRUE;
    }

    जर ((ufunc->uf_flags & FC_CLOSURE) && outer_cctx == रिक्त)
    {
	semsg(_(e_compiling_closure_without_context_str),
						printable_func_name(ufunc));
	परत FAIL;
    }

    ufunc->uf_def_status = UF_COMPILING;

    परत OK;
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
    ga_init2(&cctx->ctx_locals, आकार(lvar_T), 10);
    // Each entry on the type stack consists of two type pointers.
    ga_init2(&cctx->ctx_type_चळत, आकार(type2_T), 50);
    cctx->ctx_type_list = &ufunc->uf_type_list;
    ga_init2(&cctx->ctx_instr, आकार(isn_T), 50);
}

/*
 * For an object constructor, generate instruction to setup "this" (the first
 * local variable) and to initialize the object variables.
 */
    स्थिर पूर्णांक
obj_constructor_prologue(ufunc_T *ufunc, cctx_T *cctx)
{
    generate_CONSTRUCT(cctx, ufunc->uf_class);

    साठी (पूर्णांक i = 0; i < ufunc->uf_class->class_obj_member_मोजणी; ++i)
    {
	ocmember_T *m = &ufunc->uf_class->class_obj_members[i];

	जर (i < 2 && IS_ENUM(ufunc->uf_class))
	    // The first two object variables in an enum are the name
	    // and the ordinal.  These are set by the ISN_CONSTRUCT
	    // instruction.  So don't generate instructions to set
	    // these variables.
	    सुरूठेवा;

	जर (m->ocm_init != रिक्त)
	{
	    char_u	*अभिव्यक्ती = m->ocm_init;
	    sctx_T	save_current_sctx;
	    पूर्णांक		change_sctx = FALSE;

	    // If the member variable initialization script context is
	    // different from the current script context, then change it.
	    जर (current_sctx.sc_sid != m->ocm_init_sctx.sc_sid)
		change_sctx = TRUE;

	    जर (change_sctx)
	    {
		// generate an instruction to change the script context to the
		// member variable initialization script context.
		save_current_sctx = current_sctx;
		current_sctx = m->ocm_init_sctx;
		generate_SCRIPTCTX_SET(cctx, current_sctx);
	    }

	    पूर्णांक r = compile_expr0(&अभिव्यक्ती, cctx);

	    जर (change_sctx)
	    {
		// restore the previous script context
		current_sctx = save_current_sctx;
		generate_SCRIPTCTX_SET(cctx, current_sctx);
	    }

	    जर (r == FAIL)
		परत FAIL;

	    जर (!ends_excmd2(m->ocm_init, अभिव्यक्ती))
	    {
		semsg(_(e_trailing_characters_str), अभिव्यक्ती);
		परत FAIL;
	    }

	    type_T	*type = get_type_on_चळत(cctx, 0);
	    जर (m->ocm_type->tt_type == VAR_ANY
		    && !(m->ocm_flags & OCMFLAG_HAS_TYPE)
		    && type->tt_type != VAR_SPECIAL)
	    {
		// If the member variable type is not yet set, then use
		// the initialization expression type.
		m->ocm_type = type;
	    }
	    अन्यथा
	    {
		// The type of the member initialization expression is
		// determined at run time.  Add a runtime type check.
		where_T	where = WHERE_INIT;
		where.wt_kind = WT_MEMBER;
		where.wt_func_name = (अक्षर *)m->ocm_name.string;
		जर (need_type_where(type, m->ocm_type, 0, -1,
					where, cctx, FALSE, FALSE) == FAIL)
		    परत FAIL;
	    }
	}
	अन्यथा
	    ढकला_default_मूल्य(cctx, m->ocm_type->tt_type, FALSE, रिक्त);

	जर (((m->ocm_type->tt_type == VAR_DICT
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

    परत OK;
}

/*
 * For an object method and an constructor, generate instruction to setup
 * "this" (the first local variable).  For a constructor, generate instructions
 * to initialize the object variables.
 */
    स्थिर पूर्णांक
obj_method_prologue(ufunc_T *ufunc, cctx_T *cctx)
{
    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data) + ufunc->uf_dfunc_idx;

    जर (GA_GROW_FAILS(&dfunc->df_var_names, 1))
	परत FAIL;

    ((char_u **)dfunc->df_var_names.ga_data)[0] =
						vim_strsave((char_u *)"this");
    ++dfunc->df_var_names.ga_len;

    // In the constructor allocate memory for the object and initialize the
    // object members.
    जर (IS_CONSTRUCTOR_METHOD(ufunc))
	परत obj_constructor_prologue(ufunc, cctx);

    परत OK;
}

/*
 * Produce instructions for the default values of optional arguments.
 */
    स्थिर पूर्णांक
compile_def_function_default_args(
    ufunc_T	*ufunc,
    garray_T	*instr,
    cctx_T	*cctx)
{
    पूर्णांक	मोजणी = ufunc->uf_def_args.ga_len;
    पूर्णांक	first_def_arg = ufunc->uf_args.ga_len - मोजणी;
    पूर्णांक	i;
    पूर्णांक	off = चळत_FRAME_SIZE + (ufunc->uf_va_name != रिक्त ? 1 : 0);
    पूर्णांक	did_set_arg_type = FALSE;

    // Produce instructions for the default values of optional arguments.
    SOURCING_LNUM = 0;  // line number unknown
    साठी (i = 0; i < मोजणी; ++i)
    {
	char_u *arg = ((char_u **)(ufunc->uf_def_args.ga_data))[i];
	जर (STRCMP(arg, "v:none") == 0)
	    // "arg = v:none" means the argument is optional without
	    // setting a value when the argument is missing.
	    सुरूठेवा;

	type_T	*val_type;
	पूर्णांक		arg_idx = first_def_arg + i;
	where_T	where = WHERE_INIT;
	पूर्णांक		jump_instr_idx = instr->ga_len;
	isn_T	*isn;

	// Use a JUMP_IF_ARG_SET instruction to skip if the value was given.
	जर (generate_JUMP_IF_ARG(cctx, ISN_JUMP_IF_ARG_SET,
						i - मोजणी - off) == FAIL)
	    परत FAIL;

	// Make sure later arguments are not found.
	ufunc->uf_args_visible = arg_idx;

	पूर्णांक r = compile_expr0(&arg, cctx);
	जर (r == FAIL)
	    परत FAIL;

	// If no type specified use the type of the default value.
	// Otherwise check that the default value type matches the
	// specified type.
	val_type = get_type_on_चळत(cctx, 0);
	where.wt_index = arg_idx + 1;
	where.wt_kind = WT_ARGUMENT;
	जर (ufunc->uf_arg_types[arg_idx] == &t_unknown)
	{
	    did_set_arg_type = TRUE;
	    ufunc->uf_arg_types[arg_idx] = val_type;
	}
	अन्यथा जर (need_type_where(val_type, ufunc->uf_arg_types[arg_idx],
		    0, -1, where, cctx, FALSE, FALSE) == FAIL)
	    परत FAIL;

	जर (generate_STORE(cctx, ISN_STORE, i - मोजणी - off, रिक्त) == FAIL)
	    परत FAIL;

	// set instruction index in JUMP_IF_ARG_SET to here
	isn = ((isn_T *)instr->ga_data) + jump_instr_idx;
	isn->isn_arg.jumparg.jump_where = instr->ga_len;
    }

    जर (did_set_arg_type)
	set_function_type(ufunc);

    परत OK;
}

/*
 * Compile def function body.  Loop over all the lines in the function and
 * generate instructions.
 */
    स्थिर पूर्णांक
compile_def_function_body(
    पूर्णांक		last_func_lnum,
    पूर्णांक		check_return_type,
    garray_T	*ओळी_to_free,
    अक्षर	**errormsg,
    cctx_T	*cctx)
{
    char_u	*ओळ = रिक्त;
    char_u	*p;
    पूर्णांक		did_emsg_before = did_emsg;
#ifdef FEAT_PROFILE
    पूर्णांक		prof_lnum = -1;
#endif
    पूर्णांक		debug_lnum = -1;

    साठी (;;)
    {
	exarg_T	    ea;
	पूर्णांक	    starts_with_colon = FALSE;
	char_u	    *cmd;
	cmdmod_T    local_cmdmod;

	// Bail out on the first error to avoid a flood of errors and report
	// the right line number when inside try/catch.
	जर (did_emsg_before != did_emsg)
	    परत FAIL;

	जर (ओळ != रिक्त && *ओळ == '|')
	    // the line continues after a '|'
	    ++ओळ;
	अन्यथा जर (ओळ != रिक्त && *skipwhite(ओळ) != NUL
		&& !(*ओळ == '#' && (ओळ == cctx->ctx_ओळ_start
						    || VIM_ISWHITE(ओळ[-1]))))
	{
	    semsg(_(e_trailing_characters_str), ओळ);
	    परत FAIL;
	}
	अन्यथा जर (ओळ != रिक्त && vim9_bad_comment(skipwhite(ओळ)))
	    परत FAIL;
	अन्यथा
	{
	    ओळ = next_ओळ_from_context(cctx, FALSE);
	    जर (cctx->ctx_lnum >= last_func_lnum)
	    {
		// beyond the last line
#ifdef FEAT_PROFILE
		जर (cctx->ctx_skip != SKIP_YES)
		    may_generate_prof_end(cctx, prof_lnum);
#endif
		थांबा;
	    }
	    // Make a copy, splitting off nextcmd and removing trailing spaces
	    // may change it.
	    जर (ओळ != रिक्त)
	    {
		ओळ = vim_strsave(ओळ);
		जर (ga_add_string(ओळी_to_free, ओळ) == FAIL)
		{
		    vim_free(ओळ);
		    परत FAIL;
		}
	    }
	}

	CLEAR_FIELD(ea);
	ea.cmdlinep = &ओळ;
	ea.cmd = skipwhite(ओळ);
	ea.skip = cctx->ctx_skip == SKIP_YES;

	जर (*ea.cmd == '#')
	{
	    // "#" starts a comment, but "#{" is an error
	    जर (vim9_bad_comment(ea.cmd))
		परत FAIL;
	    ओळ = (char_u *)"";
	    सुरूठेवा;
	}

#ifdef FEAT_PROFILE
	जर (cctx->ctx_compile_type == CT_PROFILE && cctx->ctx_lnum != prof_lnum
						  && cctx->ctx_skip != SKIP_YES)
	{
	    may_generate_prof_end(cctx, prof_lnum);

	    prof_lnum = cctx->ctx_lnum;
	    generate_instr(cctx, ISN_PROF_START);
	}
#endif
	जर (cctx->ctx_compile_type == CT_DEBUG && cctx->ctx_lnum != debug_lnum
						  && cctx->ctx_skip != SKIP_YES)
	{
	    debug_lnum = cctx->ctx_lnum;
	    generate_instr_debug(cctx);
	}
	cctx->ctx_prev_lnum = cctx->ctx_lnum + 1;

	// Some things can be recognized by the first character.
	निवड (*ea.cmd)
	{
	    स्थिती '}':
		{
		    // "}" ends a block scope
		    scopetype_T stype = cctx->ctx_scope == रिक्त
					  ? NO_SCOPE : cctx->ctx_scope->se_type;

		    जर (stype == BLOCK_SCOPE)
		    {
			compile_endblock(cctx);
			ओळ = ea.cmd;
		    }
		    अन्यथा
		    {
			emsg(_(e_using_rcurly_outside_if_block_scope));
			परत FAIL;
		    }
		    जर (ओळ != रिक्त)
			ओळ = skipwhite(ea.cmd + 1);
		    सुरूठेवा;
		}

	    स्थिती '{':
		// "{" starts a block scope
		// "{'a': 1}->func() is something else
		जर (ends_excmd(*skipwhite(ea.cmd + 1)))
		{
		    ओळ = compile_block(ea.cmd, cctx);
		    सुरूठेवा;
		}
		थांबा;
	}

	/*
	 * COMMAND MODIFIERS
	 */
	cctx->ctx_has_cmdmod = FALSE;
	जर (parse_command_modifiers(&ea, errormsg, &local_cmdmod, FALSE)
								       == FAIL)
	    परत FAIL;
	generate_cmdmods(cctx, &local_cmdmod);
	undo_cmdmod(&local_cmdmod);

	// Check if there was a colon after the last command modifier or before
	// the current position.
	साठी (p = ea.cmd; p >= ओळ; --p)
	{
	    जर (*p == ':')
		starts_with_colon = TRUE;
	    जर (p < ea.cmd && !VIM_ISWHITE(*p))
		थांबा;
	}

	// Skip ":call" to get to the function name, unless using :legacy
	p = ea.cmd;
	जर (!(local_cmdmod.cmod_flags & CMOD_LEGACY))
	{
	    जर (checkforcmd(&ea.cmd, "call", 3))
	    {
		जर (*ea.cmd == '(')
		    // not for "call()"
		    ea.cmd = p;
		अन्यथा
		    ea.cmd = skipwhite(ea.cmd);
	    }

	    जर (!starts_with_colon)
	    {
		पूर्णांक	    assign;

		// Check for assignment after command modifiers.
		assign = may_compile_assignment(&ea, &ओळ, cctx);
		जर (assign == OK)
		    जा nextline;
		जर (assign == FAIL)
		    परत FAIL;
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
	जर ((*cmd != '$' || starts_with_colon)
		&& (starts_with_colon
		    || !(*cmd == '\''
			|| (cmd[0] == '0' && cmd[1] == 'z')
			|| (cmd[0] != NUL && cmd[0] == cmd[1]
					    && (*cmd == '+' || *cmd == '-'))
			|| संख्या_method(cmd))))
	{
	    ea.cmd = skip_range(ea.cmd, TRUE, रिक्त);
	    जर (ea.cmd > cmd)
	    {
		जर (!starts_with_colon
				   && !(local_cmdmod.cmod_flags & CMOD_LEGACY))
		{
		    semsg(_(e_colon_required_before_range_str), cmd);
		    परत FAIL;
		}
		ea.addr_मोजणी = 1;
		जर (ends_excmd2(ओळ, ea.cmd))
		{
		    // A range without a command: jump to the line.
		    generate_EXEC(cctx, ISN_EXECRANGE,
					      vim_strnsave(cmd, ea.cmd - cmd));
		    ओळ = ea.cmd;
		    जा nextline;
		}
	    }
	}
	p = find_ex_command(&ea, रिक्त,
		starts_with_colon || (local_cmdmod.cmod_flags & CMOD_LEGACY)
						  ? रिक्त : item_exists, cctx);

	जर (p == रिक्त)
	{
	    जर (cctx->ctx_skip != SKIP_YES)
		semsg(_(e_ambiguous_use_of_user_defined_command_str), ea.cmd);
	    परत FAIL;
	}

	// When using ":legacy cmd" always use compile_exec().
	जर (local_cmdmod.cmod_flags & CMOD_LEGACY)
	{
	    char_u *start = ea.cmd;

	    निवड (ea.cmdidx)
	    {
		स्थिती CMD_if:
		स्थिती CMD_elseif:
		स्थिती CMD_else:
		स्थिती CMD_endif:
		स्थिती CMD_for:
		स्थिती CMD_endfor:
		स्थिती CMD_continue:
		स्थिती CMD_break:
		स्थिती CMD_while:
		स्थिती CMD_endwhile:
		स्थिती CMD_try:
		स्थिती CMD_catch:
		स्थिती CMD_finally:
		स्थिती CMD_endtry:
			semsg(_(e_cannot_use_legacy_with_command_str), ea.cmd);
			परत FAIL;
		पूर्वनिर्धारित: थांबा;
	    }

	    // ":legacy return expr" needs to be handled differently.
	    जर (checkforcmd(&start, "return", 4))
		ea.cmdidx = CMD_return;
	    अन्यथा
		ea.cmdidx = CMD_legacy;
	}

	जर (p == ea.cmd && ea.cmdidx != CMD_SIZE)
	{
	    // "eval" is used for "val->func()" and "var" for "var = val", then
	    // "p" is equal to "ea.cmd" for a valid command.
	    जर (ea.cmdidx == CMD_eval || ea.cmdidx == CMD_var)
		;
	    अन्यथा जर (cctx->ctx_skip == SKIP_YES)
	    {
		ओळ += STRLEN(ओळ);
		जा nextline;
	    }
	    अन्यथा
	    {
		semsg(_(e_command_not_recognized_str), ea.cmd);
		परत FAIL;
	    }
	}

	जर ((cctx->ctx_had_return || cctx->ctx_had_throw)
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
	    परत FAIL;
	}

	// When processing the end of an if-else or try-catch block, don't
	// clear the "ctx_had_throw" flag.  If such a block ends in a "throw"
	// statement, then it is considered to end in a "return" statement.
	// The "ctx_had_throw" is cleared immediately after processing the
	// block ending statement.
	// Otherwise, clear the "had_throw" flag.
	जर (ea.cmdidx != CMD_else && ea.cmdidx != CMD_elseif
						&& ea.cmdidx != CMD_endif
						&& ea.cmdidx != CMD_catch
						&& ea.cmdidx != CMD_finally
						&& ea.cmdidx != CMD_endtry)
	    cctx->ctx_had_throw = FALSE;

	p = skipwhite(p);
	जर (ea.cmdidx != CMD_SIZE
			    && ea.cmdidx != CMD_write && ea.cmdidx != CMD_read)
	{
	    जर (ea.cmdidx >= 0)
		ea.argt = excmd_get_argt(ea.cmdidx);
	    जर ((ea.argt & EX_BANG) && *p == '!')
	    {
		ea.forceit = TRUE;
		p = skipwhite(p + 1);
	    }
	    जर ((ea.argt & EX_RANGE) == 0 && ea.addr_मोजणी > 0)
	    {
		emsg(_(e_no_range_allowed));
		परत FAIL;
	    }
	}

	निवड (ea.cmdidx)
	{
	    स्थिती CMD_def:
	    स्थिती CMD_function:
		    ea.arg = p;
		    ओळ = compile_nested_function(&ea, cctx, ओळी_to_free);
		    थांबा;

	    स्थिती CMD_return:
		    ओळ = compile_return(p, check_return_type,
				 local_cmdmod.cmod_flags & CMOD_LEGACY, cctx);
		    cctx->ctx_had_return = TRUE;
		    थांबा;

	    स्थिती CMD_let:
		    emsg(_(e_cannot_use_let_in_vim9_script));
		    थांबा;
	    स्थिती CMD_var:
	    स्थिती CMD_final:
	    स्थिती CMD_const:
	    स्थिती CMD_increment:
	    स्थिती CMD_decrement:
		    ओळ = compile_assignment(p, &ea, ea.cmdidx, cctx);
		    जर (ओळ == p)
		    {
			emsg(_(e_invalid_assignment));
			ओळ = रिक्त;
		    }
		    थांबा;

	    स्थिती CMD_unlet:
	    स्थिती CMD_unlockvar:
	    स्थिती CMD_lockvar:
		    ओळ = compile_unletlock(p, &ea, cctx);
		    थांबा;

	    स्थिती CMD_import:
		    emsg(_(e_import_can_only_be_used_in_script));
		    ओळ = रिक्त;
		    थांबा;

	    स्थिती CMD_if:
		    ओळ = compile_if(p, cctx);
		    थांबा;
	    स्थिती CMD_elseif:
		    ओळ = compile_elseif(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    cctx->ctx_had_throw = FALSE;
		    थांबा;
	    स्थिती CMD_else:
		    ओळ = compile_else(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    cctx->ctx_had_throw = FALSE;
		    थांबा;
	    स्थिती CMD_endif:
		    ओळ = compile_endif(p, cctx);
		    cctx->ctx_had_throw = FALSE;
		    थांबा;

	    स्थिती CMD_while:
		    ओळ = compile_while(p, cctx);
		    थांबा;
	    स्थिती CMD_endwhile:
		    ओळ = compile_endwhile(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    थांबा;

	    स्थिती CMD_for:
		    ओळ = compile_for(p, cctx);
		    थांबा;
	    स्थिती CMD_endfor:
		    ओळ = compile_endfor(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    थांबा;
	    स्थिती CMD_continue:
		    ओळ = compile_continue(p, cctx);
		    // "continue" unconditionally jumps back to the loop,
		    // like a "return".
		    cctx->ctx_had_return = TRUE;
		    थांबा;
	    स्थिती CMD_break:
		    ओळ = compile_break(p, cctx);
		    // "break" unconditionally exits the loop, like a "return".
		    cctx->ctx_had_return = TRUE;
		    थांबा;

	    स्थिती CMD_try:
		    ओळ = compile_try(p, cctx);
		    थांबा;
	    स्थिती CMD_catch:
		    ओळ = compile_catch(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    cctx->ctx_had_throw = FALSE;
		    थांबा;
	    स्थिती CMD_finally:
		    ओळ = compile_finally(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    cctx->ctx_had_throw = FALSE;
		    थांबा;
	    स्थिती CMD_endtry:
		    ओळ = compile_endtry(p, cctx);
		    cctx->ctx_had_throw = FALSE;
		    थांबा;
	    स्थिती CMD_throw:
		    ओळ = compile_throw(p, cctx);
		    cctx->ctx_had_throw = TRUE;
		    थांबा;

	    स्थिती CMD_eval:
		    ओळ = compile_eval(p, cctx);
		    थांबा;

	    स्थिती CMD_defer:
		    ओळ = compile_defer(p, cctx);
		    थांबा;

#ifdef HAS_MESSAGE_WINDOW
	    स्थिती CMD_echowindow:
		    {
			दीर्घ cmd_मोजणी = get_cmd_मोजणी(ओळ, &ea);
			जर (cmd_मोजणी < 0)
			    ओळ = रिक्त;
			अन्यथा
			    ओळ = compile_mult_अभिव्यक्ती(p, ea.cmdidx,
							     cmd_मोजणी, cctx);
		    }
		    थांबा;
#endif
	    स्थिती CMD_echo:
	    स्थिती CMD_echon:
	    स्थिती CMD_echoconsole:
	    स्थिती CMD_echoerr:
	    स्थिती CMD_echomsg:
	    स्थिती CMD_execute:
		    ओळ = compile_mult_अभिव्यक्ती(p, ea.cmdidx, 0, cctx);
		    थांबा;

	    स्थिती CMD_put:
		    ea.cmd = cmd;
		    ओळ = compile_put(p, &ea, cctx, FALSE);
		    थांबा;

	    स्थिती CMD_iput:
		    ea.cmd = cmd;
		    ओळ = compile_put(p, &ea, cctx, TRUE);
		    थांबा;

	    स्थिती CMD_substitute:
		    जर (check_global_and_subst(ea.cmd, p) == FAIL)
			परत FAIL;
		    जर (cctx->ctx_skip == SKIP_YES)
			ओळ = (char_u *)"";
		    अन्यथा
		    {
			ea.arg = p;
			ओळ = compile_substitute(ओळ, &ea, cctx);
		    }
		    थांबा;

	    स्थिती CMD_redir:
		    ea.arg = p;
		    ओळ = compile_redir(ओळ, &ea, cctx);
		    थांबा;

	    स्थिती CMD_cexpr:
	    स्थिती CMD_lexpr:
	    स्थिती CMD_caddexpr:
	    स्थिती CMD_laddexpr:
	    स्थिती CMD_cgetexpr:
	    स्थिती CMD_lgetexpr:
#ifdef FEAT_QUICKFIX
		    ea.arg = p;
		    ओळ = compile_cexpr(ओळ, &ea, cctx);
#अन्यथा
		    ex_ni(&ea);
		    ओळ = रिक्त;
#endif
		    थांबा;

	    स्थिती CMD_append:
	    स्थिती CMD_change:
	    स्थिती CMD_घाला:
	    स्थिती CMD_k:
	    स्थिती CMD_open:
	    स्थिती CMD_t:
	    स्थिती CMD_xit:
		    not_in_vim9(&ea, false);
		    परत FAIL;

	    स्थिती CMD_SIZE:
		    जर (cctx->ctx_skip != SKIP_YES)
		    {
			semsg(_(e_invalid_command_str), ea.cmd);
			परत FAIL;
		    }
		    // We don't check for a next command here.
		    ओळ = (char_u *)"";
		    थांबा;

	    स्थिती CMD_lua:
	    स्थिती CMD_mzscheme:
	    स्थिती CMD_perl:
	    स्थिती CMD_py3:
	    स्थिती CMD_python3:
	    स्थिती CMD_python:
	    स्थिती CMD_pythonx:
	    स्थिती CMD_ruby:
	    स्थिती CMD_tcl:
		    ea.arg = p;
		    जर (vim_strchr(ओळ, '\n') == रिक्त)
			ओळ = compile_exec(ओळ, &ea, cctx);
		    अन्यथा
			// heredoc lines have been concatenated with NL
			// characters in get_function_body()
			ओळ = compile_script(ओळ, cctx);
		    थांबा;

	    स्थिती CMD_vim9script:
		    जर (cctx->ctx_skip != SKIP_YES)
		    {
			emsg(_(e_vim9script_can_only_be_used_in_script));
			परत FAIL;
		    }
		    ओळ = (char_u *)"";
		    थांबा;

	    स्थिती CMD_class:
		    emsg(_(e_class_can_only_be_used_in_script));
		    परत FAIL;

	    स्थिती CMD_enum:
		    emsg(_(e_enum_can_only_be_used_in_script));
		    परत FAIL;

	    स्थिती CMD_interface:
		    emsg(_(e_interface_can_only_be_used_in_script));
		    परत FAIL;

	    स्थिती CMD_type:
		    emsg(_(e_type_can_only_be_used_in_script));
		    परत FAIL;

	    स्थिती CMD_global:
		    जर (check_global_and_subst(ea.cmd, p) == FAIL)
			परत FAIL;
		    // FALLTHROUGH
	    पूर्वनिर्धारित:
		    // Not recognized, execute with do_cmdline_cmd().
		    ea.arg = p;
		    ओळ = compile_exec(ओळ, &ea, cctx);
		    थांबा;
	}
nextline:
	जर (ओळ == रिक्त)
	    परत FAIL;
	ओळ = skipwhite(ओळ);

	// Undo any command modifiers.
	generate_undo_cmdmods(cctx);

	जर (cctx->ctx_type_चळत.ga_len < 0)
	{
	    iemsg("Type stack underflow");
	    परत FAIL;
	}
    } // END of the loop over all the function body lines.

    परत OK;
}

/*
 * Returns TRUE if the end of a scope (if, while, for, block) is missing.
 * Called after compiling a def function body.
 */
    स्थिर पूर्णांक
compile_dfunc_scope_end_missing(cctx_T *cctx)
{
    जर (cctx->ctx_scope == रिक्त)
	परत FALSE;

    निवड (cctx->ctx_scope->se_type)
    {
	स्थिती IF_SCOPE:
	    emsg(_(e_missing_endif));
	    थांबा;
	स्थिती WHILE_SCOPE:
	    emsg(_(e_missing_endwhile));
	    थांबा;
	स्थिती FOR_SCOPE:
	    emsg(_(e_missing_endfor));
	    थांबा;
	स्थिती TRY_SCOPE:
	    emsg(_(e_missing_endtry));
	    थांबा;
	स्थिती BLOCK_SCOPE:
	    // end block scope from :try (maybe)
	    compile_endblock(cctx);
	    जर (cctx->ctx_scope != रिक्त
		    && cctx->ctx_scope->se_type == TRY_SCOPE)
		emsg(_(e_missing_endtry));
	    अन्यथा
		emsg(_(e_missing_rcurly));
	    थांबा;
	पूर्वनिर्धारित:
	    emsg(_(e_missing_rcurly));
    }
    परत TRUE;
}

/*
 * When compiling a def function, if it doesn't have an explicit return
 * statement, then generate a default return instruction.  For an object
 * constructor, return the object.
 */
    स्थिर पूर्णांक
compile_dfunc_generate_default_return(ufunc_T *ufunc, cctx_T *cctx)
{
    // TODO: if a function ends in "throw" but there was a return elsewhere we
    // should not assume the return type is "void".
    जर (cctx->ctx_had_return || cctx->ctx_had_throw)
	परत OK;

    जर (ufunc->uf_ret_type->tt_type == VAR_UNKNOWN)
	ufunc->uf_ret_type = &t_void;
    अन्यथा जर (ufunc->uf_ret_type->tt_type != VAR_VOID
					&& !IS_CONSTRUCTOR_METHOD(ufunc))
    {
	emsg(_(e_missing_return_statement));
	परत FAIL;
    }

    // Return void if there is no return at the end.
    // For a constructor return the object.
    जर (IS_CONSTRUCTOR_METHOD(ufunc))
    {
	generate_instr(cctx, ISN_RETURN_OBJECT);
	ufunc->uf_ret_type = &ufunc->uf_class->class_object_type;
    }
    अन्यथा
	generate_instr(cctx, ISN_RETURN_VOID);

    परत OK;
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
    जर (cctx->ctx_compile_type == CT_PROFILE)
    {
	dfunc->df_instr_prof = instr->ga_data;
	dfunc->df_instr_prof_मोजणी = instr->ga_len;
    }
    अन्यथा
#endif
	जर (cctx->ctx_compile_type == CT_DEBUG)
	{
	    dfunc->df_instr_debug = instr->ga_data;
	    dfunc->df_instr_debug_मोजणी = instr->ga_len;
	}
	अन्यथा
	{
	    dfunc->df_instr = instr->ga_data;
	    dfunc->df_instr_मोजणी = instr->ga_len;
	}
    dfunc->df_varcount = dfunc->df_var_names.ga_len;
    dfunc->df_has_closure = cctx->ctx_has_closure;

    जर (cctx->ctx_outer_used)
    {
	ufunc->uf_flags |= FC_CLOSURE;
	जर (outer_cctx != रिक्त)
	    ++outer_cctx->ctx_closure_मोजणी;
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
    पूर्णांक		new_def_function,
    अक्षर	*errormsg,
    पूर्णांक		did_emsg_before,
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
    जर (!dfunc->df_deleted && new_def_function
			&& ufunc->uf_dfunc_idx == def_functions.ga_len - 1)
    {
	--def_functions.ga_len;
	ufunc->uf_dfunc_idx = 0;
    }
    ufunc->uf_def_status = UF_COMPILE_ERROR;

    पर्यंत (cctx->ctx_scope != रिक्त)
	drop_scope(cctx);

    जर (errormsg != रिक्त)
	emsg(errormsg);
    अन्यथा जर (did_emsg == did_emsg_before)
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
    पूर्णांक
compile_def_function(
	ufunc_T		*ufunc,
	पूर्णांक		check_return_type,
	compiletype_T   compile_type,
	cctx_T		*outer_cctx)
{
    garray_T	ओळी_to_free;
    अक्षर	*errormsg = रिक्त;	// error message
    cctx_T	cctx;
    garray_T	*instr;
    पूर्णांक		did_emsg_before = did_emsg;
    पूर्णांक		did_emsg_silent_before = did_emsg_silent;
    पूर्णांक		ret = FAIL;
    sctx_T	save_current_sctx = current_sctx;
    पूर्णांक		save_estack_compiling = estack_compiling;
    पूर्णांक		save_cmod_flags = cmdmod.cmod_flags;
    पूर्णांक		do_estack_ढकला;
    पूर्णांक		new_def_function = FALSE;

    // allocated lines are freed at the end
    ga_init2(&ओळी_to_free, आकार(char_u *), 50);

    // Initialize the ufunc and the compilation context
    जर (compile_dfunc_ufunc_init(ufunc, outer_cctx, compile_type,
						&new_def_function) == FAIL)
	परत FAIL;

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
    do_estack_ढकला = !estack_शीर्ष_is_ufunc(ufunc, 1);
    जर (do_estack_ढकला)
	estack_ढकला_ufunc(ufunc, 1);
    estack_compiling = TRUE;

    // Make sure arguments don't shadow variables in the context
    जर (check_args_shadowing(ufunc, &cctx) == FAIL)
	जा erret;

    // For an object method and a constructor generate instructions to
    // initialize "this" and the object variables.
    जर (ufunc->uf_flags & (FC_OBJECT|FC_NEW))
	जर (obj_method_prologue(ufunc, &cctx) == FAIL)
	    जा erret;

    जर (ufunc->uf_def_args.ga_len > 0)
	जर (compile_def_function_default_args(ufunc, instr, &cctx) == FAIL)
	    जा erret;
    ufunc->uf_args_visible = ufunc->uf_args.ga_len;

    // Compiling an abstract method or a function in an interface is done to
    // get the function type.  No code is actually compiled.
    जर (ufunc->uf_class != रिक्त && (IS_INTERFACE(ufunc->uf_class)
						|| IS_ABSTRACT_METHOD(ufunc)))
    {
	ufunc->uf_def_status = UF_NOT_COMPILED;
	ret = OK;
	जा erret;
    }

    // compile the function body
    जर (compile_def_function_body(ufunc->uf_ओळी.ga_len, check_return_type,
				&ओळी_to_free, &errormsg, &cctx) == FAIL)
	जा erret;

    जर (compile_dfunc_scope_end_missing(&cctx))
	जा erret;

    जर (compile_dfunc_generate_default_return(ufunc, &cctx) == FAIL)
	जा erret;

    // When compiled with ":silent!" and there was an error don't consider the
    // function compiled.
    जर (emsg_silent == 0 || did_emsg_silent == did_emsg_silent_before)
	compile_dfunc_epilogue(outer_cctx, ufunc, instr, &cctx);

    ret = OK;

erret:
    जर (ufunc->uf_def_status == UF_COMPILING)
    {
	// compilation failed. do cleanup.
	compile_dfunc_ufunc_cleanup(ufunc, instr, new_def_function,
				    errormsg, did_emsg_before, &cctx);
    }

    जर (cctx.ctx_redir_lhs.lhs_name != रिक्त)
    {
	जर (ret == OK)
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
    जर (do_estack_ढकला)
	estack_काढा();

    ga_clear_strings(&ओळी_to_free);
    free_locals(&cctx);
    ga_clear(&cctx.ctx_type_चळत);
    परत ret;
}

    शून्य
set_function_type(ufunc_T *ufunc)
{
    पूर्णांक varargs = ufunc->uf_va_name != रिक्त;
    पूर्णांक argcount = ufunc->uf_args.ga_len;

    // Create a type for the function, with the return type and any
    // argument types.
    // A vararg is included in uf_args.ga_len but not in uf_arg_types.
    // The type is included in "tt_args".
    जर (argcount > 0 || varargs)
    {
	जर (ufunc->uf_type_list.ga_itemsize == 0)
	    ga_init2(&ufunc->uf_type_list, आकार(type_T *), 10);
	ufunc->uf_func_type = alloc_func_type(ufunc->uf_ret_type,
					   argcount, &ufunc->uf_type_list);
	// Add argument types to the function type.
	जर (func_type_add_arg_types(ufunc->uf_func_type,
				    argcount + varargs,
				    &ufunc->uf_type_list) == FAIL)
	    परत;
	ufunc->uf_func_type->tt_argcount = argcount + varargs;
	ufunc->uf_func_type->tt_min_argcount =
				      argcount - ufunc->uf_def_args.ga_len;
	जर (ufunc->uf_arg_types == रिक्त)
	{
	    पूर्णांक i;

	    // lambda does not have argument types.
	    साठी (i = 0; i < argcount; ++i)
		ufunc->uf_func_type->tt_args[i] = &t_any;
	}
	अन्यथा
	    mch_memmove(ufunc->uf_func_type->tt_args,
			 ufunc->uf_arg_types, आकार(type_T *) * argcount);
	जर (varargs)
	{
	    ufunc->uf_func_type->tt_args[argcount] =
		   ufunc->uf_va_type == रिक्त ? &t_list_any : ufunc->uf_va_type;
	    ufunc->uf_func_type->tt_flags = TTFLAG_VARARGS;
	}
    }
    अन्यथा
	// No arguments, can use a predefined type.
	ufunc->uf_func_type = get_func_type(ufunc->uf_ret_type,
					   argcount, &ufunc->uf_type_list);
}

/*
 * Free all instructions for "dfunc" except df_name.
 */
    स्थिर शून्य
delete_def_function_contents(dfunc_T *dfunc, पूर्णांक mark_deleted)
{
    पूर्णांक idx;

    // In same cases the instructions may refer to a class in which the
    // function is defined and unreferencing the class may call back here
    // recursively.  Set the df_delete_busy to avoid problems.
    जर (dfunc->df_delete_busy)
	परत;
    dfunc->df_delete_busy = TRUE;

    ga_clear(&dfunc->df_def_args_isn);
    ga_clear_strings(&dfunc->df_var_names);

    जर (dfunc->df_instr != रिक्त)
    {
	साठी (idx = 0; idx < dfunc->df_instr_मोजणी; ++idx)
	    delete_instr(dfunc->df_instr + idx);
	VIM_CLEAR(dfunc->df_instr);
    }
    जर (dfunc->df_instr_debug != रिक्त)
    {
	साठी (idx = 0; idx < dfunc->df_instr_debug_मोजणी; ++idx)
	    delete_instr(dfunc->df_instr_debug + idx);
	VIM_CLEAR(dfunc->df_instr_debug);
    }
#ifdef FEAT_PROFILE
    जर (dfunc->df_instr_prof != रिक्त)
    {
	साठी (idx = 0; idx < dfunc->df_instr_prof_मोजणी; ++idx)
	    delete_instr(dfunc->df_instr_prof + idx);
	VIM_CLEAR(dfunc->df_instr_prof);
    }
#endif

    जर (mark_deleted)
	dfunc->df_deleted = TRUE;
    जर (dfunc->df_ufunc != रिक्त)
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
    जर (ufunc->uf_dfunc_idx <= 0 || def_functions.ga_data == रिक्त)
	परत;

    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data)
	+ ufunc->uf_dfunc_idx;

    जर (--dfunc->df_refcount <= 0)
	delete_def_function_contents(dfunc, TRUE);
    ufunc->uf_def_status = UF_NOT_COMPILED;
    ufunc->uf_dfunc_idx = 0;
    जर (dfunc->df_ufunc == ufunc)
	dfunc->df_ufunc = रिक्त;
}

/*
 * Used when a user function refers to an existing dfunc.
 */
    शून्य
link_def_function(ufunc_T *ufunc)
{
    जर (ufunc->uf_dfunc_idx <= 0)
	परत;

    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data)
	+ ufunc->uf_dfunc_idx;

    ++dfunc->df_refcount;
}

#जर defined(EXITFREE)
/*
 * Free all functions defined with ":def".
 */
    शून्य
free_def_functions(शून्य)
{
    पूर्णांक idx;

    साठी (idx = 0; idx < def_functions.ga_len; ++idx)
    {
	dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data) + idx;

	delete_def_function_contents(dfunc, TRUE);
	vim_free(dfunc->df_name);
    }

    ga_clear(&def_functions);
}
#endif


#endif // FEAT_EVAL
