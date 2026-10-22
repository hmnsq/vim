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
#அந்தர்பாவ "vim.h"

#எனில் defined(FEAT_EVAL)

// Functions defined with :def are stored in this growarray.
// They are never removed, so that they can be found by index.
// Deleted functions have the df_deleted flag set.
garray_T def_functions = {0, 0, அளவு(dfunc_T), 50, வெற்று};

நிலையான வெறுமை delete_def_function_contents(dfunc_T *dfunc, முழு mark_deleted);

/*
 * Lookup variable "name" in the local scope and return it in "lvar".
 * "lvar->lv_from_outer" is incremented accordingly.
 * If "lvar" is NULL only check if the variable can be found.
 * Return FAIL if not found.
 */
    முழு
lookup_local(char_u *name, size_t len, lvar_T *lvar, cctx_T *cctx)
{
    முழு	    idx;
    lvar_T  *lvp;

    எனில் (len == 0)
	திரும்பு FAIL;

    எனில் (((len == 4 && STRNCMP(name, "this", 4) == 0)
		|| (len == 5 && STRNCMP(name, "super", 5) == 0))
	    && cctx->ctx_ufunc != வெற்று
	    && (cctx->ctx_ufunc->uf_flags & (FC_OBJECT|FC_NEW)))
    {
	முழு is_super = *name == 's';
	எனில் (is_super)
	{
	    எனில் (name[5] != '.')
	    {
		emsg(_(e_super_must_be_followed_by_dot));
		திரும்பு FAIL;
	    }
	    எனில் (cctx->ctx_ufunc->uf_class != வெற்று
		    && cctx->ctx_ufunc->uf_class->class_extends == வெற்று)
	    {
		emsg(_(e_using_super_not_in_child_class));
		திரும்பு FAIL;
	    }
	}
	எனில் (lvar != வெற்று)
	{
	    CLEAR_POINTER(lvar);
	    lvar->lv_loop_depth = -1;
	    lvar->lv_name = (char_u *)(is_super ? "super" : "this");
	    எனில் (cctx->ctx_ufunc->uf_class != வெற்று)
	    {
		lvar->lv_type = &cctx->ctx_ufunc->uf_class->class_object_type;
		எனில் (is_super)
		{
		    type_T *type = get_type_ptr(cctx->ctx_type_list);

		    எனில் (type != வெற்று)
		    {
			*type = *lvar->lv_type;
			lvar->lv_type = type;
			type->tt_flags |= TTFLAG_SUPER;
		    }
		}
	    }
	}
	திரும்பு OK;
    }

    // Find local in current function scope.
    க்கு (idx = 0; idx < cctx->ctx_locals.ga_len; ++idx)
    {
	lvp = ((lvar_T *)cctx->ctx_locals.ga_data) + idx;
	எனில் (lvp->lv_name != வெற்று
		&& STRNCMP(name, lvp->lv_name, len) == 0
					       && STRLEN(lvp->lv_name) == len)
	{
	    எனில் (lvar != வெற்று)
	    {
		*lvar = *lvp;
		lvar->lv_from_outer = 0;
		// If the variable was declared inside a loop set
		// lvar->lv_loop_idx and lvar->lv_loop_depth.
		get_loop_var_idx(cctx, idx, lvar);
	    }
	    திரும்பு OK;
	}
    }

    // Find local in outer function scope.
    எனில் (cctx->ctx_outer != வெற்று)
    {
	எனில் (lookup_local(name, len, lvar, cctx->ctx_outer) == OK)
	{
	    எனில் (lvar != வெற்று)
	    {
		cctx->ctx_outer_used = TRUE;
		++lvar->lv_from_outer;
	    }
	    திரும்பு OK;
	}
    }

    திரும்பு FAIL;
}

/*
 * Lookup an argument in the current function and an enclosing function.
 * Returns the argument index in "idxp"
 * Returns the argument type in "type"
 * Sets "gen_load_outer" to TRUE if found in outer scope.
 * Returns OK when found, FAIL otherwise.
 */
    முழு
arg_exists(
	char_u	*name,
	size_t	len,
	முழு	*idxp,
	type_T	**type,
	முழு	*gen_load_outer,
	cctx_T	*cctx)
{
    முழு	    idx;
    char_u  *va_name;

    எனில் (len == 0)
	திரும்பு FAIL;
    க்கு (idx = 0; idx < cctx->ctx_ufunc->uf_args_visible; ++idx)
    {
	char_u *arg = FUNCARG(cctx->ctx_ufunc, idx);

	எனில் (STRNCMP(name, arg, len) == 0 && arg[len] == NUL)
	{
	    எனில் (idxp != வெற்று)
	    {
		// Arguments are located above the frame pointer.  One further
		// if there is a vararg argument
		*idxp = idx - (cctx->ctx_ufunc->uf_args.ga_len
							    + அடுக்கு_FRAME_SIZE)
			      + (cctx->ctx_ufunc->uf_va_name != வெற்று ? -1 : 0);

		எனில் (cctx->ctx_ufunc->uf_arg_types != வெற்று)
		    *type = cctx->ctx_ufunc->uf_arg_types[idx];
		இல்லையெனில்
		    *type = &t_any;
	    }
	    திரும்பு OK;
	}
    }

    va_name = cctx->ctx_ufunc->uf_va_name;
    எனில் (va_name != வெற்று
		    && STRNCMP(name, va_name, len) == 0 && va_name[len] == NUL)
    {
	எனில் (idxp != வெற்று)
	{
	    // varargs is always the last argument
	    *idxp = -அடுக்கு_FRAME_SIZE - 1;
	    *type = cctx->ctx_ufunc->uf_va_type;
	}
	திரும்பு OK;
    }

    எனில் (cctx->ctx_outer != வெற்று)
    {
	// Lookup the name for an argument of the outer function.
	எனில் (arg_exists(name, len, idxp, type, gen_load_outer, cctx->ctx_outer)
									 == OK)
	{
	    எனில் (gen_load_outer != வெற்று)
	    {
		++*gen_load_outer;
		cctx->ctx_outer_used = TRUE;
	    }
	    திரும்பு OK;
	}
    }

    திரும்பு FAIL;
}

/*
 * Lookup a script-local variable in the current script, possibly defined in a
 * block that contains the function "cctx->ctx_ufunc".
 * "cctx" is NULL at the script level, "cstack" is NULL in a function.
 * If "len" is <= 0 "name" must be NUL terminated.
 * Return NULL when not found.
 */
    நிலையான sallvar_T *
find_script_var(char_u *name, size_t len, cctx_T *cctx, cstack_T *cstack)
{
    scriptitem_T    *si = SCRIPT_ITEM(current_sctx.sc_sid);
    hashitem_T	    *hi;
    முழு		    cc;
    sallvar_T	    *sav;
    ufunc_T	    *ufunc;

    // Find the list of all script variables with the right name.
    எனில் (len > 0)
    {
	cc = name[len];
	name[len] = NUL;
    }
    hi = hash_find(&si->sn_all_vars.dv_hashtab, name);
    எனில் (len > 0)
	name[len] = cc;
    எனில் (HASHITEM_EMPTY(hi))
	திரும்பு வெற்று;

    sav = HI2SAV(hi);
    எனில் (sav->sav_block_id == 0)
	// variable defined in the top script scope is always visible
	திரும்பு sav;

    எனில் (cctx == வெற்று)
    {
	எனில் (cstack == வெற்று)
	    திரும்பு வெற்று;

	// Not in a function scope, find variable with block ID equal to or
	// smaller than the current block id.  Use "cstack" to go up the block
	// scopes.
	போது (sav != வெற்று)
	{
	    முழு idx;

	    க்கு (idx = cstack->cs_idx; idx >= 0; --idx)
		எனில் (cstack->cs_block_id[idx] == sav->sav_block_id)
		    நிறுத்து;
	    எனில் (idx >= 0)
		நிறுத்து;
	    sav = sav->sav_next;
	}
	திரும்பு sav;
    }

    // Go over the variables with this name and find one that was visible
    // from the function.
    ufunc = cctx->ctx_ufunc;
    போது (sav != வெற்று)
    {
	முழு idx;

	எனில் (ufunc->uf_block_depth == 0 && sav->sav_block_id == 0)
	    // If the function was defined at the script level (not inside a
	    // block), script-scope variables are always visible.
	    திரும்பு sav;

	// Go over the blocks that this function was defined in.  If the
	// variable block ID matches it was visible to the function.
	க்கு (idx = 0; idx < ufunc->uf_block_depth; ++idx)
	    எனில் (ufunc->uf_block_ids[idx] == sav->sav_block_id)
		திரும்பு sav;
	sav = sav->sav_next;
    }

    // Not found, variable was not visible.
    திரும்பு வெற்று;
}

/*
 * If "name" can be found in the current script set it's "block_id".
 */
    வெறுமை
update_script_var_block_id(char_u *name, முழு block_id)
{
    scriptitem_T    *si = SCRIPT_ITEM(current_sctx.sc_sid);
    hashitem_T	    *hi;
    sallvar_T	    *sav;

    hi = hash_find(&si->sn_all_vars.dv_hashtab, name);
    எனில் (HASHITEM_EMPTY(hi))
	திரும்பு;
    sav = HI2SAV(hi);
    sav->sav_block_id = block_id;
}

/*
 * Lookup a variable (without s: prefix) in the current script.
 * "cctx" is NULL at the script level, "cstack" is NULL in a function.
 * Returns OK or FAIL.
 */
    முழு
script_var_exists(char_u *name, size_t len, cctx_T *cctx, cstack_T *cstack)
{
    எனில் (current_sctx.sc_sid <= 0)
	திரும்பு FAIL;
    எனில் (current_script_is_vim9())
    {
	// Check script variables that were visible where the function was
	// defined.
	எனில் (find_script_var(name, len, cctx, cstack) != வெற்று)
	    திரும்பு OK;
    }
    இல்லையெனில்
    {
	hashtab_T	*ht = &SCRIPT_VARS(current_sctx.sc_sid);
	dictitem_T	*di;
	முழு		cc;

	// Check script variables that are currently visible
	cc = name[len];
	name[len] = NUL;
	di = find_var_in_ht(ht, 0, name, TRUE);
	name[len] = cc;
	எனில் (di != வெற்று)
	    திரும்பு OK;
    }

    திரும்பு FAIL;
}

/*
 * Returns the index of a class method or class variable with name "name"
 * accessible in the currently compiled function.
 * If "cl_ret" is not NULL set it to the class.
 * Otherwise return -1.
 */
    நிலையான முழு
cctx_class_midx(
    cctx_T  *cctx,
    முழு	    is_method,
    char_u  *name,
    size_t  len,
    class_T **cl_ret)
{
    எனில் (cctx == வெற்று || cctx->ctx_ufunc == வெற்று
	    || cctx->ctx_ufunc->uf_class == வெற்று
	    || cctx->ctx_ufunc->uf_defclass == வெற்று)
	திரும்பு -1;

    // Search for the class method or variable in the class where the calling
    // function is defined.
    class_T *cl = cctx->ctx_ufunc->uf_defclass;
    முழு m_idx = is_method ? class_method_idx(cl, name, len)
					: class_member_idx(cl, name, len);
    எனில் (m_idx < 0)
    {
	cl = cl->class_extends;
	போது (cl != வெற்று)
	{
	    m_idx = is_method ? class_method_idx(cl, name, len)
					: class_member_idx(cl, name, len);
	    எனில் (m_idx >= 0)
		நிறுத்து;
	    cl = cl->class_extends;
	}
    }

    எனில் (m_idx >= 0)
    {
	எனில் (cl_ret != வெற்று)
	    *cl_ret = cl;
    }

    திரும்பு m_idx;
}

/*
 * Returns the index of a class method with name "name" accessible in the
 * currently compiled function.  Returns -1 if not found.  The class where the
 * method is defined is returned in "cl_ret".
 */
    முழு
cctx_class_method_idx(
    cctx_T  *cctx,
    char_u  *name,
    size_t  len,
    class_T **cl_ret)
{
    திரும்பு cctx_class_midx(cctx, TRUE, name, len, cl_ret);
}

/*
 * Returns the index of a class variable with name "name" accessible in the
 * currently compiled function.  Returns -1 if not found.  The class where the
 * variable is defined is returned in "cl_ret".
 */
    முழு
cctx_class_member_idx(
    cctx_T  *cctx,
    char_u  *name,
    size_t  len,
    class_T **cl_ret)
{
    திரும்பு cctx_class_midx(cctx, FALSE, name, len, cl_ret);
}

/*
 * Return TRUE if "name" is a local variable, argument, script variable or
 * imported.  Also if "name" is "this" and in a class method.
 */
    நிலையான முழு
variable_exists(char_u *name, size_t len, cctx_T *cctx)
{
    திரும்பு (cctx != வெற்று
		&& (lookup_local(name, len, வெற்று, cctx) == OK
		    || arg_exists(name, len, வெற்று, வெற்று, வெற்று, cctx) == OK
		    || (len == 4
			&& cctx->ctx_ufunc != வெற்று
			&& (cctx->ctx_ufunc->uf_flags & (FC_OBJECT|FC_NEW))
			&& STRNCMP(name, "this", 4) == 0)))
	    || script_var_exists(name, len, cctx, வெற்று) == OK
	    || cctx_class_member_idx(cctx, name, len, வெற்று) >= 0
	    || find_imported(name, len, FALSE) != வெற்று;
}

/*
 * Return TRUE if "name" is a local variable, argument, script variable,
 * imported or function.  Or commands are being skipped, a declaration may have
 * been skipped then.
 */
    நிலையான முழு
item_exists(char_u *name, size_t len, முழு cmd UNUSED, cctx_T *cctx)
{
    திரும்பு variable_exists(name, len, cctx);
}

/*
 * Check if "p[len]" is already defined, either in script "import_sid" or in
 * compilation context "cctx".
 * "cctx" is NULL at the script level, "cstack" is NULL in a function.
 * Does not check the global namespace.
 * If "is_arg" is TRUE the error message is for an argument name.
 * Return FAIL and give an error if it defined.
 */
    முழு
check_defined(
	char_u	    *p,
	size_t	    len,
	cctx_T	    *cctx,
	cstack_T    *cstack,
	முழு	    is_arg)
{
    முழு		c = p[len];
    ufunc_T	*ufunc = வெற்று;

    // underscore argument is OK
    எனில் (len == 1 && *p == '_')
	திரும்பு OK;

    எனில் (script_var_exists(p, len, cctx, cstack) == OK)
    {
	எனில் (is_arg)
	    semsg(_(e_argument_already_declared_in_script_str), p);
	இல்லையெனில்
	    semsg(_(e_variable_already_declared_in_script_str), p);
	திரும்பு FAIL;
    }

    எனில் (cctx_class_member_idx(cctx, p, len, வெற்று) >= 0)
    {
	எனில் (is_arg)
	    semsg(_(e_argument_already_declared_in_class_str), p);
	இல்லையெனில்
	    semsg(_(e_variable_already_declared_in_class_str), p);
	திரும்பு FAIL;
    }

    p[len] = NUL;
    எனில் ((cctx != வெற்று
		&& (lookup_local(p, len, வெற்று, cctx) == OK
		    || arg_exists(p, len, வெற்று, வெற்று, வெற்று, cctx) == OK))
	    || find_imported(p, len, FALSE) != வெற்று
	    || (ufunc = find_func_even_dead(p, 0)) != வெற்று)
    {
	// A local or script-local function can shadow a global function.
	எனில் (ufunc == வெற்று || ((ufunc->uf_flags & FC_DEAD) == 0
		    && (!func_is_global(ufunc)
					     || (p[0] == 'g' && p[1] == ':'))))
	{
	    எனில் (is_arg)
		semsg(_(e_argument_name_shadows_existing_variable_str), p);
	    இல்லையெனில்
		semsg(_(e_name_already_defined_str), p);
	    p[len] = c;
	    திரும்பு FAIL;
	}
    }
    p[len] = c;
    திரும்பு OK;
}


/*
 * Return TRUE if "actual" could be "expected" and a runtime typecheck is to be
 * used.  Return FALSE if the types will never match.
 */
    நிலையான முழு
use_typecheck(type_T *actual, type_T *expected)
{
    எனில் (actual->tt_type == VAR_ANY
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
	திரும்பு TRUE;
    எனில் (actual->tt_type == VAR_OBJECT && expected->tt_type == VAR_OBJECT)
	திரும்பு TRUE;
    எனில் ((actual->tt_type == VAR_LIST
		|| actual->tt_type == VAR_TUPLE
		|| actual->tt_type == VAR_DICT)
	    && actual->tt_type == expected->tt_type)
	// This takes care of a nested list or dict.
	திரும்பு use_typecheck(actual->tt_member, expected->tt_member);
    திரும்பு FALSE;
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
    முழு
need_type_where(
	type_T	*actual,
	type_T	*expected,
	முழு	typechk_flags,	// acceptable types (type check flags)
	முழு	offset,
	where_T	where,
	cctx_T	*cctx,
	முழு	silent,
	முழு	actual_is_const)
{
    முழு ret;

    எனில் (expected->tt_type != VAR_CLASS && expected->tt_type != VAR_TYPEALIAS)
    {
	எனில் (check_type_is_மதிப்பு(actual) == FAIL)
	    திரும்பு FAIL;
    }

    எனில் (expected == &t_bool && actual != &t_bool
					&& (actual->tt_flags & TTFLAG_BOOL_OK))
    {
	// Using "0", "1" or the result of an expression with "&&" or "||" as a
	// boolean is OK but requires a conversion.
	generate_2BOOL(cctx, FALSE, offset);
	திரும்பு OK;
    }

    ret = check_type_maybe(expected, actual, FALSE, where);
    எனில் (ret == OK)
	திரும்பு OK;

    // If actual a constant a runtime check makes no sense.  If it's
    // null_function it is OK.
    எனில் (actual_is_const && ret == MAYBE && actual == &t_func_unknown)
	திரும்பு OK;

    // If the actual type can be the expected type add a runtime check.
    எனில் (!actual_is_const && ret == MAYBE && use_typecheck(actual, expected))
    {
	generate_TYPECHECK(cctx, expected, typechk_flags, offset,
		where.wt_kind == WT_VARIABLE, where.wt_index);
	திரும்பு OK;
    }

    எனில் (!silent)
	type_mismatch_where(expected, actual, where);
    திரும்பு FAIL;
}

    முழு
need_type(
	type_T	*actual,
	type_T	*expected,
	முழு	typechk_flags,	// acceptable types (type check flags)
	முழு	offset,
	முழு	arg_idx,
	cctx_T	*cctx,
	முழு	silent,
	முழு	actual_is_const)
{
    where_T where = WHERE_INIT;

    எனில் (arg_idx > 0)
    {
	where.wt_index = arg_idx;
	where.wt_kind = WT_ARGUMENT;
    }
    திரும்பு need_type_where(actual, expected, typechk_flags, offset, where,
						cctx, silent, actual_is_const);
}

/*
 * Set type of variable "lvar" to "type".  If the variable is a constant then
 * the type gets TTFLAG_CONST.
 */
    நிலையான வெறுமை
set_var_type(lvar_T *lvar, type_T *type_arg, cctx_T *cctx)
{
    type_T	*type = type_arg;

    எனில் (lvar->lv_const == ASSIGN_CONST && (type->tt_flags & TTFLAG_CONST) == 0)
    {
	எனில் (type->tt_flags & TTFLAG_STATIC)
	    // entry in static_types[] is followed by const type
	    type = type + 1;
	இல்லையெனில்
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
	முழு	assign,
	type_T	*type)
{
    lvar_T  *lvar;
    dfunc_T *dfunc;

    எனில் (arg_exists(name, len, வெற்று, வெற்று, வெற்று, cctx) == OK)
    {
	emsg_namelen(_(e_str_is_used_as_argument), name, (முழு)len);
	திரும்பு வெற்று;
    }

    எனில் (GA_GROW_FAILS(&cctx->ctx_locals, 1))
	திரும்பு வெற்று;
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
    எனில் (type == &t_unknown || type == &t_any)
	// type not known yet, may be inferred from RHS
	lvar->lv_type = type;
    இல்லையெனில்
	// may use TTFLAG_CONST
	set_var_type(lvar, type, cctx);

    // Remember the name for debugging.
    எனில் (GA_GROW_FAILS(&dfunc->df_var_names, 1))
	திரும்பு வெற்று;
    ((char_u **)dfunc->df_var_names.ga_data)[lvar->lv_idx] =
						    vim_strsave(lvar->lv_name);
    ++dfunc->df_var_names.ga_len;

    திரும்பு lvar;
}

/*
 * If "check_writable" is ASSIGN_CONST give an error if the variable was
 * defined with :final or :const, if "check_writable" is ASSIGN_FINAL give an
 * error if the variable was defined with :const.
 */
    நிலையான முழு
check_item_writable(svar_T *sv, முழு check_writable, char_u *name)
{
    எனில் ((check_writable == ASSIGN_CONST && sv->sv_const != 0)
	    || (check_writable == ASSIGN_FINAL
					      && sv->sv_const == ASSIGN_CONST))
    {
	semsg(_(e_cannot_change_readonly_variable_str), name);
	திரும்பு FAIL;
    }
    திரும்பு OK;
}

/*
 * Find "name" in script-local items of script "sid".
 * Pass "check_writable" to check_item_writable().
 * "cctx" is NULL at the script level, "cstack" is NULL in a function.
 * Returns the index in "sn_var_vals" if found.
 * If found but not in "sn_var_vals" returns -1.
 * If not found or the variable is not writable returns -2.
 */
    முழு
get_script_item_idx(
	முழு	    sid,
	char_u	    *name,
	முழு	    check_writable,
	cctx_T	    *cctx,
	cstack_T    *cstack)
{
    hashtab_T	    *ht;
    dictitem_T	    *di;
    scriptitem_T    *si = SCRIPT_ITEM(sid);
    svar_T	    *sv;
    முழு		    idx;

    எனில் (!SCRIPT_ID_VALID(sid))
	திரும்பு -1;
    எனில் (sid == current_sctx.sc_sid)
    {
	sallvar_T *sav = find_script_var(name, 0, cctx, cstack);

	எனில் (sav == வெற்று)
	    திரும்பு -2;
	idx = sav->sav_var_vals_idx;
	sv = ((svar_T *)si->sn_var_vals.ga_data) + idx;
	எனில் (check_item_writable(sv, check_writable, name) == FAIL)
	    திரும்பு -2;
	திரும்பு idx;
    }

    // First look the name up in the hashtable.
    ht = &SCRIPT_VARS(sid);
    di = find_var_in_ht(ht, 0, name, TRUE);
    எனில் (di == வெற்று)
    {
	எனில் (si->sn_autoload_prefix != வெற்று)
	{
	    hashitem_T *hi;

	    // A variable exported from an autoload script is in the global
	    // variables, we can find it in the all_vars table.
	    hi = hash_find(&si->sn_all_vars.dv_hashtab, name);
	    எனில் (!HASHITEM_EMPTY(hi))
		திரும்பு HI2SAV(hi)->sav_var_vals_idx;
	}
	திரும்பு -2;
    }

    // Now find the svar_T index in sn_var_vals.
    க்கு (idx = 0; idx < si->sn_var_vals.ga_len; ++idx)
    {
	sv = ((svar_T *)si->sn_var_vals.ga_data) + idx;
	எனில் (sv->sv_tv == &di->di_tv)
	{
	    எனில் (check_item_writable(sv, check_writable, name) == FAIL)
		திரும்பு -2;
	    திரும்பு idx;
	}
    }
    திரும்பு -1;
}

    நிலையான imported_T *
find_imported_in_script(char_u *name, size_t len, முழு sid)
{
    scriptitem_T    *si;
    முழு		    idx;

    எனில் (!SCRIPT_ID_VALID(sid))
	திரும்பு வெற்று;
    si = SCRIPT_ITEM(sid);
    க்கு (idx = 0; idx < si->sn_imports.ga_len; ++idx)
    {
	imported_T *import = ((imported_T *)si->sn_imports.ga_data) + idx;

	எனில் (len == 0 ? STRCMP(name, import->imp_name) == 0
		     : STRLEN(import->imp_name) == len
				  && STRNCMP(name, import->imp_name, len) == 0)
	    திரும்பு import;
    }
    திரும்பு வெற்று;
}

/*
 * Find "name" in imported items of the current script.
 * If "len" is 0 use any length that works.
 * If "load" is TRUE and the script was not loaded yet, load it now.
 */
    imported_T *
find_imported(char_u *name, size_t len, முழு load)
{
    எனில் (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	திரும்பு வெற்று;

    // Skip over "s:" before "s:something" to find the import name.
    முழு off = name[0] == 's' && name[1] == ':' ? 2 : 0;

    imported_T *ret = find_imported_in_script(name + off, len - off,
							  current_sctx.sc_sid);
    எனில் (ret != வெற்று && load && (ret->imp_flags & IMP_FLAGS_AUTOLOAD))
    {
	scid_T	actual_sid = 0;
	முழு	save_emsg_off = emsg_off;

	// "emsg_off" will be set when evaluating an expression silently, but
	// we do want to know about errors in a script.  Also because it then
	// aborts when an error is encountered.
	emsg_off = FALSE;

	// script found before but not loaded yet
	ret->imp_flags &= ~IMP_FLAGS_AUTOLOAD;
	(வெறுமை)do_source(SCRIPT_ITEM(ret->imp_sid)->sn_name, FALSE,
						       DOSO_NONE, &actual_sid);
	// If the script is a symlink it may be sourced with another name, may
	// need to adjust the script ID for that.
	எனில் (actual_sid != 0)
	    ret->imp_sid = actual_sid;

	emsg_off = save_emsg_off;
    }
    திரும்பு ret;
}

/*
 * Called when checking for a following operator at "arg".  When the rest of
 * the line is empty or only a comment, peek the next line.  If there is a next
 * line return a pointer to it and set "nextp".
 * Otherwise skip over white space.
 */
    char_u *
may_peek_next_வரி(cctx_T *cctx, char_u *arg, char_u **nextp)
{
    char_u *p = skipwhite(arg);

    *nextp = வெற்று;
    எனில் (*p == NUL || (VIM_ISWHITE(*arg) && vim9_comment_start(p)))
    {
	*nextp = peek_next_வரி_from_context(cctx);
	எனில் (*nextp != வெற்று)
	    திரும்பு *nextp;
    }
    திரும்பு p;
}

/*
 * Return a pointer to the next line that isn't empty or only contains a
 * comment. Skips over white space.
 * Returns NULL if there is none.
 */
    char_u *
peek_next_வரி_from_context(cctx_T *cctx)
{
    முழு lnum = cctx->ctx_lnum;

    போது (++lnum < cctx->ctx_ufunc->uf_வரிகள்.ga_len)
    {
	char_u *வரி = ((char_u **)cctx->ctx_ufunc->uf_வரிகள்.ga_data)[lnum];
	char_u *p;

	// ignore NULLs inserted for continuation lines
	எனில் (வரி != வெற்று)
	{
	    p = skipwhite(வரி);
	    எனில் (vim9_bad_comment(p))
		திரும்பு வெற்று;
	    எனில் (*p != NUL && !vim9_comment_start(p))
		திரும்பு p;
	}
    }
    திரும்பு வெற்று;
}

/*
 * Get the next line of the function from "cctx".
 * Skips over empty lines.  Skips over comment lines if "skip_comment" is TRUE.
 * Returns NULL when at the end.
 */
    char_u *
next_வரி_from_context(cctx_T *cctx, முழு skip_comment)
{
    char_u	*வரி;

    செய்
    {
	++cctx->ctx_lnum;
	எனில் (cctx->ctx_lnum >= cctx->ctx_ufunc->uf_வரிகள்.ga_len)
	{
	    வரி = வெற்று;
	    நிறுத்து;
	}
	வரி = ((char_u **)cctx->ctx_ufunc->uf_வரிகள்.ga_data)[cctx->ctx_lnum];
	cctx->ctx_வரி_start = வரி;
	SOURCING_LNUM = cctx->ctx_lnum + 1;
    } போது (வரி == வெற்று || *skipwhite(வரி) == NUL
		     || (skip_comment && vim9_comment_start(skipwhite(வரி))));
    திரும்பு வரி;
}

/*
 * Skip over white space at "whitep" and assign to "*arg".
 * If "*arg" is at the end of the line, advance to the next line.
 * Also when "whitep" points to white space and "*arg" is on a "#".
 * Return FAIL if beyond the last line, "*arg" is unmodified then.
 */
    முழு
may_get_next_வரி(char_u *whitep, char_u **arg, cctx_T *cctx)
{
    *arg = skipwhite(whitep);
    எனில் (vim9_bad_comment(*arg))
	திரும்பு FAIL;
    எனில் (**arg == NUL || (VIM_ISWHITE(*whitep) && vim9_comment_start(*arg)))
    {
	char_u *next = next_வரி_from_context(cctx, TRUE);

	எனில் (next == வெற்று)
	    திரும்பு FAIL;
	*arg = skipwhite(next);
    }
    திரும்பு OK;
}

/*
 * Idem, and give an error when failed.
 */
    முழு
may_get_next_வரி_error(char_u *whitep, char_u **arg, cctx_T *cctx)
{
    எனில் (may_get_next_வரி(whitep, arg, cctx) == FAIL)
    {
	SOURCING_LNUM = cctx->ctx_lnum + 1;
	emsg(_(e_வரி_incomplete));
	திரும்பு FAIL;
    }
    திரும்பு OK;
}

/*
 * Get a line from the compilation context, compatible with exarg_T getline().
 * Return a pointer to the line in allocated memory.
 * Return NULL for end-of-file or some error.
 */
    நிலையான char_u *
exarg_getline(
	முழு c UNUSED,
	வெறுமை *cookie,
	முழு indent UNUSED,
	getline_opt_T options UNUSED)
{
    cctx_T  *cctx = (cctx_T *)cookie;
    char_u  *p;

    க்கு (;;)
    {
	எனில் (cctx->ctx_lnum >= cctx->ctx_ufunc->uf_வரிகள்.ga_len - 1)
	    திரும்பு வெற்று;
	++cctx->ctx_lnum;
	p = ((char_u **)cctx->ctx_ufunc->uf_வரிகள்.ga_data)[cctx->ctx_lnum];
	// Comment lines result in NULL pointers, skip them.
	எனில் (p != வெற்று)
	    திரும்பு vim_strsave(p);
    }
}

    வெறுமை
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
    முழு
func_needs_compiling(ufunc_T *ufunc, compiletype_T compile_type)
{
    தேர்வு (ufunc->uf_def_status)
    {
	நிலை UF_TO_BE_COMPILED:
	    திரும்பு TRUE;

	நிலை UF_COMPILED:
	{
	    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data)
							 + ufunc->uf_dfunc_idx;

	    தேர்வு (compile_type)
	    {
		நிலை CT_PROFILE:
#ifdef FEAT_PROFILE
		    திரும்பு dfunc->df_instr_prof == வெற்று;
#endif
		நிலை CT_NONE:
		    திரும்பு dfunc->df_instr == வெற்று;
		நிலை CT_DEBUG:
		    திரும்பு dfunc->df_instr_debug == வெற்று;
	    }
	}

	நிலை UF_NOT_COMPILED:
	நிலை UF_COMPILE_ERROR:
	நிலை UF_COMPILING:
	    நிறுத்து;
    }
    திரும்பு FALSE;
}

/*
 * Compile a nested :def command.
 */
    நிலையான char_u *
compile_nested_function(exarg_T *eap, cctx_T *cctx, garray_T *வரிகள்_to_free)
{
    முழு		is_global = *eap->arg == 'g' && eap->arg[1] == ':';
    char_u	*name_start = eap->arg;
    char_u	*name_end = to_name_end(eap->arg, TRUE);
    முழு		off;
    char_u	*func_name;
    string_T	lambda_name;
    ufunc_T	*ufunc;
    முழு		r = FAIL;
    compiletype_T   compile_type;
    முழு		funcref_isn_idx = -1;
    lvar_T	*lvar = வெற்று;
    char_u	*bracket_start = வெற்று;

    எனில் (eap->forceit)
    {
	semsg(_(e_cannot_use_bang_with_nested_def_str),
		eap->cmdidx == CMD_def ? ":def" : ":function");
	திரும்பு வெற்று;
    }

    எனில் (*name_start == '/')
    {
	name_end = skip_regexp(name_start + 1, '/', TRUE);
	எனில் (*name_end == '/')
	    ++name_end;
	set_nextcmd(eap, name_end);
    }

    எனில் (*name_end == '<')
    {
	bracket_start = name_end;
	எனில் (skip_generic_func_type_args(&name_end) == FAIL)
	    திரும்பு வெற்று;
    }

    எனில் (name_end == name_start || *skipwhite(name_end) != '(')
    {
	எனில் (!ends_excmd2(name_start, name_end))
	{
	    எனில் (*skipwhite(name_end) == '.')
		semsg(_(e_cannot_define_dict_func_in_vim9_script_str),
								     eap->cmd);
	    இல்லையெனில்
		semsg(_(e_invalid_command_str), eap->cmd);
	    திரும்பு வெற்று;
	}

	// "def" or "def Name": list functions
	எனில் (generate_DEF(cctx, name_start, name_end - name_start) == FAIL)
	    திரும்பு வெற்று;
	திரும்பு eap->nextcmd == வெற்று ? (char_u *)"" : eap->nextcmd;
    }

    எனில் (bracket_start != வெற்று)
	// generic function.  The function name ends before the list of types
	// (opening angle bracket).
	name_end = bracket_start;

    // Only g:Func() can use a namespace.
    எனில் (name_start[1] == ':' && !is_global)
    {
	semsg(_(e_namespace_not_supported_str), name_start);
	திரும்பு வெற்று;
    }
    எனில் (cctx->ctx_skip != SKIP_YES
	    && check_defined(name_start, name_end - name_start, cctx,
							  வெற்று, FALSE) == FAIL)
	திரும்பு வெற்று;
    எனில் (!ASCII_ISUPPER(is_global ? name_start[2] : name_start[0]))
    {
	semsg(_(e_function_name_must_start_with_capital_str), name_start);
	திரும்பு வெற்று;
    }

    eap->arg = name_end;
    fill_exarg_from_cctx(eap, cctx);

    eap->forceit = FALSE;
    // We use the special <lambda>99 name, but it's not really a lambda.
    lambda_name = get_lambda_name();
    lambda_name.string = vim_strnsave(lambda_name.string, lambda_name.length);
    எனில் (lambda_name.string == வெற்று)
	திரும்பு வெற்று;

    // This may free the current line, make a copy of the name.
    off = is_global ? 2 : 0;
    func_name = vim_strnsave(name_start + off, name_end - name_start - off);
    எனில் (func_name == வெற்று)
    {
	r = FAIL;
	செல் theend;
    }

    // A type variable cannot have the name of the function.  The function is
    // defined with the "<lambda>" name, check it here.
    எனில் (bracket_start != வெற்று)
    {
	size_t	len = STRLEN(func_name);

	க்கு (char_u *t = bracket_start + 1; *t != '>' && *t != NUL; )
	{
	    char_u *te = to_name_end(t, FALSE);

	    எனில் ((size_t)(te - t) == len && STRNCMP(t, func_name, len) == 0)
	    {
		semsg(_(e_redefining_script_item_str), func_name);
		r = FAIL;
		செல் theend;
	    }
	    t = te == t ? t + 1 : te;
	}
    }

    // Make sure "KeyTyped" is not set, it may cause indent to be written.
    முழு save_சாவி_Typed = சாவி_Typed;
    சாவி_Typed = FALSE;

    ufunc = define_function(eap, lambda_name.string, வரிகள்_to_free, 0, வெற்று, 0,
									cctx);

    சாவி_Typed = save_சாவி_Typed;

    எனில் (ufunc == வெற்று)
    {
	r = eap->skip ? OK : FAIL;
	செல் theend;
    }
    எனில் (eap->nextcmd != வெற்று)
    {
	semsg(_(e_text_found_after_str_str),
	      eap->cmdidx == CMD_def ? "enddef" : "endfunction", eap->nextcmd);
	r = FAIL;
	func_ptr_unref(ufunc);
	செல் theend;
    }

    // copy over the block scope IDs before compiling
    எனில் (!is_global && cctx->ctx_ufunc->uf_block_depth > 0)
    {
	முழு block_depth = cctx->ctx_ufunc->uf_block_depth;

	ufunc->uf_block_ids = ALLOC_MULT(முழு, block_depth);
	எனில் (ufunc->uf_block_ids != வெற்று)
	{
	    mch_memmove(ufunc->uf_block_ids, cctx->ctx_ufunc->uf_block_ids,
						    அளவு(முழு) * block_depth);
	    ufunc->uf_block_depth = block_depth;
	}
    }

    // Define the funcref before compiling, so that it is found by any
    // recursive call.
    எனில் (is_global)
    {
	r = generate_NEWFUNC(cctx, lambda_name.string, func_name);
	func_name = வெற்று;
	lambda_name.string = வெற்று;
	lambda_name.length = 0;
    }
    இல்லையெனில்
    {
	// Define a local variable for the function reference.
	lvar = reserve_local(cctx, func_name, name_end - name_start,
					    ASSIGN_CONST, ufunc->uf_func_type);
	எனில் (lvar == வெற்று)
	{
	    func_ptr_unref(ufunc);
	    செல் theend;
	}
	lvar->lv_ufunc = ufunc;
	எனில் (generate_FUNCREF(cctx, ufunc, வெற்று, FALSE, 0, &funcref_isn_idx) == FAIL)
	{
	    func_ptr_unref(ufunc);
	    செல் theend;
	}
	r = generate_STORE(cctx, ISN_STORE, lvar->lv_idx, வெற்று);
    }

    compile_type = get_compile_type(ufunc);
#ifdef FEAT_PROFILE
    // If the outer function is profiled, also compile the nested function for
    // profiling.
    எனில் (cctx->ctx_compile_type == CT_PROFILE)
	compile_type = CT_PROFILE;
#endif
    எனில் (func_needs_compiling(ufunc, compile_type)
	    && compile_def_function(ufunc, TRUE, compile_type, cctx) == FAIL)
    {
	func_ptr_unref(ufunc);
	எனில் (lvar != வெற்று)
	    // Now the local variable can't be used.
	    *lvar->lv_name = '/';  // impossible value
	செல் theend;
    }

#ifdef FEAT_PROFILE
    // When the outer function is compiled for profiling, the nested function
    // may be called without profiling.  Compile it here in the right context.
    எனில் (compile_type == CT_PROFILE && func_needs_compiling(ufunc, CT_NONE))
	compile_def_function(ufunc, FALSE, CT_NONE, cctx);
#endif

    // If a FUNCREF instruction was generated, set the index after compiling.
    எனில் (funcref_isn_idx != -1 && ufunc->uf_def_status == UF_COMPILED)
    {
	isn_T	*funcref_isn = ((isn_T *)cctx->ctx_instr.ga_data) +
							funcref_isn_idx;
	funcref_isn->isn_arg.funcref.fr_dfunc_idx = ufunc->uf_dfunc_idx;
    }

theend:
    vim_free(lambda_name.string);
    vim_free(func_name);
    திரும்பு r == FAIL ? வெற்று : (char_u *)"";
}

/*
 * Compile one Vim expression {expr} in string "p".
 * "p" points to the opening "{".
 * Return a pointer to the character after "}", NULL for an error.
 */
    char_u *
compile_one_கோவை_in_str(char_u *p, cctx_T *cctx)
{
    char_u	*block_start;
    char_u	*block_end;

    // Skip the opening {.
    block_start = skipwhite(p + 1);
    block_end = block_start;
    எனில் (*block_start != NUL && skip_கோவை(&block_end, வெற்று) == FAIL)
	திரும்பு வெற்று;
    block_end = skipwhite(block_end);
    // The block must be closed by a }.
    எனில் (*block_end != '}')
    {
	semsg(_(e_missing_close_curly_str), p);
	திரும்பு வெற்று;
    }
    எனில் (compile_expr0(&block_start, cctx) == FAIL)
	திரும்பு வெற்று;
    may_generate_2STRING(-1, TOSTRING_INTERPOLATE, cctx);

    திரும்பு block_end + 1;
}

/*
 * Compile a string "str" (either containing a literal string or a mix of
 * literal strings and Vim expressions of the form `{expr}`).  This is used
 * when compiling a heredoc assignment to a variable or an interpolated string
 * in a Vim9 def function.  Vim9 instructions are generated to push strings,
 * evaluate expressions, concatenate them and create a list of lines.  When
 * "evalstr" is TRUE, Vim expressions in "str" are evaluated.
 */
    முழு
compile_all_கோவை_in_str(char_u *str, முழு evalstr, cctx_T *cctx)
{
    char_u	*p = str;
    char_u	*val;
    முழு		மொத்தம் = 0;

    எனில் (cctx->ctx_skip == SKIP_YES)
	திரும்பு OK;

    எனில் (!evalstr || *str == NUL)
    {
	// Literal string, possibly empty.
	val = *str != NUL ? vim_strsave(str) : வெற்று;
	திரும்பு generate_PUSHS(cctx, &val);
    }

    // Push all the string pieces to the stack, followed by a ISN_CONCAT.
    போது (*p != NUL)
    {
	char_u	*lit_start;
	முழு	escaped_brace = FALSE;

	// Look for a block start.
	lit_start = p;
	போது (*p != '{' && *p != '}' && *p != NUL)
	    ++p;

	எனில் (*p != NUL && *p == p[1])
	{
	    // Escaped brace, unescape and continue.
	    // Include the brace in the literal string.
	    ++p;
	    escaped_brace = TRUE;
	}
	இல்லையெனில் எனில் (*p == '}')
	{
	    semsg(_(e_stray_closing_curly_str), str);
	    திரும்பு FAIL;
	}

	// Append the literal part.
	எனில் (p != lit_start)
	{
	    val = vim_strnsave(lit_start, (size_t)(p - lit_start));
	    எனில் (generate_PUSHS(cctx, &val) == FAIL)
		திரும்பு FAIL;
	    ++மொத்தம்;
	}

	எனில் (*p == NUL)
	    நிறுத்து;

	எனில் (escaped_brace)
	{
	    // Skip the second brace.
	    ++p;
	    தொடர்;
	}

	p = compile_one_கோவை_in_str(p, cctx);
	எனில் (p == வெற்று)
	    திரும்பு FAIL;
	++மொத்தம்;
    }

    // Small optimization, if there's only a single piece skip the ISN_CONCAT.
    எனில் (மொத்தம் > 1)
	திரும்பு generate_CONCAT(cctx, மொத்தம்);

    திரும்பு OK;
}

/*
 * Return the length of an assignment operator, or zero if there isn't one.
 */
    முழு
assignment_len(char_u *p, முழு *heredoc)
{
    எனில் (*p == '=')
    {
	எனில் (p[1] == '<' && p[2] == '<')
	{
	    *heredoc = TRUE;
	    திரும்பு 3;
	}
	திரும்பு 1;
    }
    எனில் (vim_strchr((char_u *)"+-*/%", *p) != வெற்று && p[1] == '=')
	திரும்பு 2;
    எனில் (STRNCMP(p, "..=", 3) == 0)
	திரும்பு 3;
    திரும்பு 0;
}

/*
 * Generate the load instruction for "name".
 */
    நிலையான முழு
generate_loadvar(cctx_T *cctx, lhs_T *lhs)
{
    char_u	*name = lhs->lhs_name;
    type_T	*type = lhs->lhs_type;
    முழு		res = OK;

    தேர்வு (lhs->lhs_dest)
    {
	நிலை dest_option:
	நிலை dest_func_option:
	    generate_LOAD(cctx, ISN_LOADOPT, 0, name, type);
	    நிறுத்து;
	நிலை dest_global:
	    எனில் (vim_strchr(name, AUTOLOAD_CHAR) == வெற்று)
	    {
		எனில் (name[2] == NUL)
		    generate_instr_type(cctx, ISN_LOADGDICT, &t_dict_any);
		இல்லையெனில்
		    generate_LOAD(cctx, ISN_LOADG, 0, name + 2, type);
	    }
	    இல்லையெனில்
		generate_LOAD(cctx, ISN_LOADAUTO, 0, name, type);
	    நிறுத்து;
	நிலை dest_buffer:
	    generate_LOAD(cctx, ISN_LOADB, 0, name + 2, type);
	    நிறுத்து;
	நிலை dest_window:
	    generate_LOAD(cctx, ISN_LOADW, 0, name + 2, type);
	    நிறுத்து;
	நிலை dest_tab:
	    generate_LOAD(cctx, ISN_LOADT, 0, name + 2, type);
	    நிறுத்து;
	நிலை dest_script:
	நிலை dest_script_v9:
	    res = compile_load_scriptvar(cctx,
			    name + (name[1] == ':' ? 2 : 0), வெற்று, வெற்று, வெற்று);
	    நிறுத்து;
	நிலை dest_env:
	    // Include $ in the name here
	    generate_LOAD(cctx, ISN_LOADENV, 0, name, type);
	    நிறுத்து;
	நிலை dest_reg:
	    generate_LOAD(cctx, ISN_LOADREG, name[1], வெற்று, &t_string);
	    நிறுத்து;
	நிலை dest_vimvar:
	    generate_LOADV(cctx, name + 2);
	    நிறுத்து;
	நிலை dest_local:
	    எனில் (cctx->ctx_skip != SKIP_YES)
	    {
		lvar_T	*lvar = lhs->lhs_lvar;
		எனில் (lvar->lv_from_outer > 0)
		    generate_LOADOUTER(cctx, lvar->lv_idx, lvar->lv_from_outer,
				 lvar->lv_loop_depth, lvar->lv_loop_idx, type);
		இல்லையெனில்
		    generate_LOAD(cctx, ISN_LOAD, lvar->lv_idx, வெற்று, type);
	    }
	    நிறுத்து;
	நிலை dest_class_member:
	    generate_CLASSMEMBER(cctx, TRUE, lhs->lhs_class,
						     lhs->lhs_classmember_idx);
	    நிறுத்து;
	நிலை dest_கோவை:
	    // list or dict value should already be on the stack.
	    நிறுத்து;
    }

    திரும்பு res;
}

/*
 * Skip over "[expr]" or ".member".
 * Does not check for any errors.
 */
    நிலையான char_u *
skip_index(char_u *start)
{
    char_u *p = start;

    எனில் (*p == '[')
    {
	p = skipwhite(p + 1);
	(வெறுமை)skip_கோவை(&p, வெற்று);
	p = skipwhite(p);
	எனில் (*p == ']')
	    திரும்பு p + 1;
	திரும்பு p;
    }
    // if (*p == '.')
    திரும்பு to_name_end(p + 1, TRUE);
}

    வெறுமை
vim9_declare_error(char_u *name)
{
    எழுத்து *scope = "";

    தேர்வு (*name)
    {
	நிலை 'g': scope = _("global"); நிறுத்து;
	நிலை 'b': scope = _("buffer"); நிறுத்து;
	நிலை 'w': scope = _("window"); நிறுத்து;
	நிலை 't': scope = _("tab"); நிறுத்து;
	நிலை 'v': scope = "v:"; நிறுத்து;
	நிலை '$': semsg(_(e_cannot_declare_an_environment_variable_str), name);
		  திரும்பு;
	நிலை '&': semsg(_(e_cannot_declare_an_option_str), name);
		  திரும்பு;
	நிலை '@': semsg(_(e_cannot_declare_a_register_str), name);
		  திரும்பு;
	இயல்பு: திரும்பு;
    }
    semsg(_(e_cannot_declare_a_scope_variable_str), scope, name);
}

/*
 * Return TRUE if "name" is a valid register to use.
 * Return FALSE and give an error message if not.
 */
    நிலையான முழு
valid_dest_reg(முழு name)
{
    எனில் (name == '@')
       name = '"';
    எனில் (name == '/' || name == '=' || name == '#' || valid_yank_reg(name, TRUE))
	திரும்பு TRUE;
    emsg_invreg(name);
    திரும்பு FAIL;
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
    முழு
get_var_dest(
	char_u		*name,
	assign_dest_T	*dest,
	cmdidx_T	cmdidx,
	முழு		has_index,
	முழு		*option_scope,
	முழு		*vimvaridx,
	type_T		**type,
	cctx_T		*cctx)
{
    char_u *p;

    எனில் (*name == '&')
    {
	முழு		cc;
	long		numval;
	getoption_T	opt_type;
	முழு		opt_p_flags;

	*dest = dest_option;
	எனில் (cmdidx == CMD_final || cmdidx == CMD_const)
	{
	    emsg(_(e_cannot_lock_option));
	    திரும்பு FAIL;
	}
	p = name;
	p = find_option_end(&p, option_scope);
	எனில் (p == வெற்று)
	{
	    // cannot happen?
	    emsg(_(e_unexpected_characters_in_assignment));
	    திரும்பு FAIL;
	}
	cc = *p;
	*p = NUL;
	opt_type = get_option_மதிப்பு(skip_option_env_lead(name),
				   &numval, வெற்று, &opt_p_flags, *option_scope);
	*p = cc;
	தேர்வு (opt_type)
	{
	    நிலை gov_unknown:
		    semsg(_(e_unknown_option_str), name);
		    திரும்பு FAIL;
	    நிலை gov_string:
	    நிலை gov_hidden_string:
		    எனில் (opt_p_flags & P_FUNC)
		    {
			// might be a Funcref, check the type later
			*type = &t_any;
			*dest = dest_func_option;
		    }
		    இல்லையெனில்
		    {
			*type = &t_string;
		    }
		    நிறுத்து;
	    நிலை gov_bool:
	    நிலை gov_hidden_bool:
		    *type = &t_bool;
		    நிறுத்து;
	    நிலை gov_எண்:
	    நிலை gov_hidden_எண்:
		    *type = &t_எண்;
		    நிறுத்து;
	}
    }
    இல்லையெனில் எனில் (*name == '$')
    {
	*dest = dest_env;
	*type = &t_string;
    }
    இல்லையெனில் எனில் (*name == '@')
    {
	எனில் (!valid_dest_reg(name[1]))
	    திரும்பு FAIL;
	*dest = dest_reg;
	*type = name[1] == '#' ? &t_எண்_or_string : &t_string;
    }
    இல்லையெனில் எனில் (STRNCMP(name, "g:", 2) == 0)
    {
	*dest = dest_global;
    }
    இல்லையெனில் எனில் (STRNCMP(name, "b:", 2) == 0)
    {
	*dest = dest_buffer;
    }
    இல்லையெனில் எனில் (STRNCMP(name, "w:", 2) == 0)
    {
	*dest = dest_window;
    }
    இல்லையெனில் எனில் (STRNCMP(name, "t:", 2) == 0)
    {
	*dest = dest_tab;
    }
    இல்லையெனில் எனில் (STRNCMP(name, "v:", 2) == 0)
    {
	typval_T	*vtv;
	முழு		di_flags;

	*vimvaridx = find_vim_var(name + 2, &di_flags);
	எனில் (*vimvaridx < 0)
	{
	    semsg(_(e_variable_not_found_str), name);
	    திரும்பு FAIL;
	}
	// An item of a read-only variable can be changed, as when not compiled.
	// We use the current value of "sandbox" here, is that OK?
	எனில் (!has_index && var_check_ro(di_flags, name, FALSE))
	    திரும்பு FAIL;
	*dest = dest_vimvar;
	vtv = get_vim_var_tv(*vimvaridx);
	*type = typval2type_vimvar(vtv, cctx->ctx_type_list);
    }
    திரும்பு OK;
}

    நிலையான முழு
is_decl_command(cmdidx_T cmdidx)
{
    திரும்பு cmdidx == CMD_let || cmdidx == CMD_var
				 || cmdidx == CMD_final || cmdidx == CMD_const;
}

/*
 * Returns TRUE if the class or object variable in "lhs" is modifiable.
 * "var_start" points to the start of the variable name and "lhs->lhs_varlen"
 * has the total length.  Note that the "lhs" can be nested an object reference
 * (e.g.  a.b.c.d.var).
 */
    நிலையான முழு
lhs_class_member_modifiable(lhs_T *lhs, char_u	*var_start, cctx_T *cctx)
{
    size_t	varlen = lhs->lhs_varlen;
    class_T	*cl = lhs->lhs_type->tt_class;
    முழு		is_object = lhs->lhs_type->tt_type == VAR_OBJECT;
    char_u	*name = var_start + varlen + 1;
    size_t	namelen = lhs->lhs_end - var_start - varlen - 1;
    ocmember_T	*m;

    m = member_lookup(cl, lhs->lhs_type->tt_type, name, namelen, வெற்று);
    எனில் (m == வெற்று)
    {
	member_not_found_செய்தி(cl, lhs->lhs_type->tt_type, name, namelen);
	திரும்பு FALSE;
    }

    எனில் (IS_ENUM(cl))
    {
	semsg(_(e_enumvalue_str_cannot_be_modified), cl->class_name.string,
		m->ocm_name.string);
	திரும்பு FALSE;
    }

    // If it is private member variable, then accessing it outside the
    // class is not allowed.
    // If it is a read only class variable, then it can be modified
    // only inside the class where it is defined.
    எனில் ((m->ocm_access != VIM_ACCESS_ALL) &&
	    ((is_object && !inside_class(cctx, cl))
	     || (!is_object && cctx->ctx_ufunc->uf_class != cl)))
    {
	எழுத்து *செய்தி = (m->ocm_access == VIM_ACCESS_PRIVATE)
				? e_cannot_access_protected_variable_str
				: e_variable_is_not_writable_str;
	emsg_var_cl_define(செய்தி, m->ocm_name.string, 0, cl);
	திரும்பு FALSE;
    }

    திரும்பு TRUE;
}

/*
 * Initialize "lhs" with default values
 */
    நிலையான வெறுமை
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
    நிலையான முழு
lhs_find_var_end(
    lhs_T	*lhs,
    char_u	*var_start,
    முழு		is_decl,
    char_u	**var_endp)
{
    char_u  *var_end = *var_endp;

    // "lhs_dest_end" is the end of the destination, including "[expr]" or
    // ".name".
    // "var_end" is the end of the variable/option/etc. name.
    lhs->lhs_dest_end = skip_var_one(var_start, FALSE);
    எனில் (*var_start == '@')
    {
	எனில் (!valid_dest_reg(var_start[1]))
	    திரும்பு FAIL;
	var_end = var_start + 2;
    }
    இல்லையெனில்
    {
	// skip over the leading "&", "&l:", "&g:" and "$"
	var_end = skip_option_env_lead(var_start);
	var_end = to_name_end(var_end, TRUE);
    }

    // "a: type" is declaring variable "a" with a type, not dict "a:".
    எனில் (is_decl && lhs->lhs_dest_end == var_start + 2
					&& lhs->lhs_dest_end[-1] == ':')
	--lhs->lhs_dest_end;
    எனில் (is_decl && var_end == var_start + 2 && var_end[-1] == ':')
	--var_end;

    lhs->lhs_end = lhs->lhs_dest_end;
    *var_endp = var_end;

    திரும்பு OK;
}

/*
 * Set various fields in "lhs"
 */
    நிலையான முழு
lhs_init(
    lhs_T	*lhs,
    char_u	*var_start,
    முழு		is_decl,
    முழு		heredoc,
    char_u	**var_endp)
{
    char_u *var_end = *var_endp;

    lhs_init_defaults(lhs);

    // Find the end of the variable and the destination
    எனில் (lhs_find_var_end(lhs, var_start, is_decl, &var_end) == FAIL)
	திரும்பு FAIL;

    // compute the length of the destination without "[expr]" or ".name"
    lhs->lhs_varlen = var_end - var_start;
    lhs->lhs_varlen_total = lhs->lhs_varlen;
    lhs->lhs_name = vim_strnsave(var_start, lhs->lhs_varlen);
    எனில் (lhs->lhs_name == வெற்று)
	திரும்பு FAIL;

    எனில் (lhs->lhs_dest_end > var_start + lhs->lhs_varlen)
	// Something follows after the variable: "var[idx]" or "var.key".
	lhs->lhs_has_index = TRUE;

    lhs->lhs_type = heredoc ? &t_list_string : &t_any;

    *var_endp = var_end;

    திரும்பு OK;
}

/*
 * Compile a LHS class variable name.
 */
    நிலையான முழு
compile_lhs_class_variable(
    cctx_T	*cctx,
    lhs_T	*lhs,
    class_T	*defcl,
    முழு		is_decl)
{
    எனில் (cctx->ctx_ufunc->uf_defclass != defcl)
    {
	// A class variable can be accessed without the class name
	// only inside a class.
	semsg(_(e_class_variable_str_accessible_only_inside_class_str),
		lhs->lhs_name, defcl->class_name.string);
	திரும்பு FAIL;
    }

    எனில் (is_decl)
    {
	semsg(_(e_variable_already_declared_in_class_str), lhs->lhs_name);
	திரும்பு FAIL;
    }

    ocmember_T	*m = &defcl->class_class_members[lhs->lhs_classmember_idx];
    எனில் (oc_var_check_ro(defcl, m))
	திரும்பு FAIL;

    lhs->lhs_dest = dest_class_member;
    // The class variable is defined either in the current class or
    // in one of the parent class in the hierarchy.
    lhs->lhs_class = defcl;
    lhs->lhs_type = oc_member_type_by_idx(defcl, FALSE,
						lhs->lhs_classmember_idx);

    திரும்பு OK;
}

/*
 * Compile an imported LHS variable
 */
    நிலையான முழு
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
    எனில் (dot == வெற்று)
    {
	semsg(_(e_no_dot_after_imported_name_str), var_start);
	திரும்பு FAIL;
    }

    p = skipwhite(dot + 1);
    var_end = to_name_end(p, TRUE);
    எனில் (var_end == p)
    {
	semsg(_(e_missing_name_after_imported_name_str), var_start);
	திரும்பு FAIL;
    }

    vim_free(lhs->lhs_name);
    lhs->lhs_varlen = var_end - p;
    lhs->lhs_name = vim_strnsave(p, lhs->lhs_varlen);
    எனில் (lhs->lhs_name == வெற்று)
	திரும்பு FAIL;
    *rawnamep = lhs->lhs_name;
    lhs->lhs_scriptvar_sid = import->imp_sid;

    // TODO: where do we check this name is exported?

    // Check if something follows: "exp.var[idx]" or
    // "exp.var.key".
    lhs->lhs_has_index = lhs->lhs_dest_end > skipwhite(var_end);

    *var_endp = var_end;

    திரும்பு OK;
}

/*
 * Process a script-local variable when compiling a LHS variable name.
 */
    நிலையான முழு
compile_lhs_script_var(
    cctx_T	*cctx,
    lhs_T	*lhs,
    char_u	*var_start,
    char_u	*var_end,
    முழு		is_decl)
{
    முழு		script_namespace = FALSE;
    முழு		script_var = FALSE;
    imported_T	*import;
    char_u	*var_name;
    size_t	var_name_len;

    எனில் (lhs->lhs_varlen > 1 && STRNCMP(var_start, "s:", 2) == 0)
	script_namespace = TRUE;

    எனில் (script_namespace)
    {
	var_name = var_start + 2;
	var_name_len = lhs->lhs_varlen - 2;
    }
    இல்லையெனில்
    {
	var_name = var_start;
	var_name_len = lhs->lhs_varlen;
    }

    எனில் (script_var_exists(var_name, var_name_len, cctx, வெற்று) == OK)
	script_var = TRUE;

    import = find_imported(var_start, lhs->lhs_varlen, FALSE);

    எனில் (script_namespace || script_var || import != வெற்று)
    {
	char_u *rawname = lhs->lhs_name + (lhs->lhs_name[1] == ':' ? 2 : 0);

	எனில் (script_namespace && current_script_is_vim9())
	{
	    semsg(_(e_cannot_use_s_colon_in_vim9_script_str), var_start);
	    திரும்பு FAIL;
	}

	எனில் (is_decl)
	{
	    எனில் (script_namespace)
		semsg(_(e_cannot_declare_script_variable_in_function_str),
			lhs->lhs_name);
	    இல்லையெனில்
		semsg(_(e_variable_already_declared_in_script_str),
			lhs->lhs_name);
	    திரும்பு FAIL;
	}
	இல்லையெனில் எனில் (cctx->ctx_ufunc->uf_script_ctx_version == SCRIPT_VERSION_VIM9
		&& script_namespace
		&& !script_var && import == வெற்று)
	{
	    semsg(_(e_unknown_variable_str), lhs->lhs_name);
	    திரும்பு FAIL;
	}

	lhs->lhs_dest = current_script_is_vim9() ? dest_script_v9 :
								dest_script;

	// existing script-local variables should have a type
	lhs->lhs_scriptvar_sid = current_sctx.sc_sid;
	எனில் (import != வெற்று)
	{
	    எனில் (compile_lhs_import_var(lhs, import, var_start, &var_end,
							&rawname) == FAIL)
		திரும்பு FAIL;
	}

	எனில் (SCRIPT_ID_VALID(lhs->lhs_scriptvar_sid))
	{
	    // Check writable only when no index follows.
	    lhs->lhs_scriptvar_idx = get_script_item_idx(
					lhs->lhs_scriptvar_sid, rawname,
					lhs->lhs_has_index ?  ASSIGN_FINAL :
					ASSIGN_CONST, cctx, வெற்று);
	    எனில் (lhs->lhs_scriptvar_idx >= 0)
	    {
		scriptitem_T *si = SCRIPT_ITEM(lhs->lhs_scriptvar_sid);
		svar_T	 *sv = ((svar_T *)si->sn_var_vals.ga_data)
						+ lhs->lhs_scriptvar_idx;

		lhs->lhs_type = sv->sv_type;
	    }
	}

	திரும்பு OK;
    }

    திரும்பு check_defined(var_start, lhs->lhs_varlen, cctx, வெற்று, FALSE);
}

/*
 * Compile the LHS destination.
 */
    நிலையான முழு
compile_lhs_var_dest(
    cctx_T	*cctx,
    lhs_T	*lhs,
    முழு		cmdidx,
    char_u	*var_start,
    char_u	*var_end,
    முழு		is_decl,
    முழு		has_cmd)	// "var" before "var_start"
{
    முழு	    declare_error = FALSE;

    எனில் (get_var_dest(lhs->lhs_name, &lhs->lhs_dest, cmdidx,
				lhs->lhs_has_index,
				&lhs->lhs_opt_flags, &lhs->lhs_vimvaridx,
				&lhs->lhs_type, cctx) == FAIL)
	திரும்பு FAIL;

    எனில் (lhs->lhs_dest != dest_local && cmdidx != CMD_const
						&& cmdidx != CMD_final)
    {
	// Specific kind of variable recognized.
	declare_error = is_decl;
    }
    இல்லையெனில்
    {
	class_T	*defcl;

	// No specific kind of variable recognized, just a name.
	எனில் (check_reserved_name(lhs->lhs_name, lhs->lhs_has_index
						&& *var_end == '.') == FAIL)
	    திரும்பு FAIL;

	எனில் (lookup_local(var_start, lhs->lhs_varlen, &lhs->lhs_local_lvar,
								cctx) == OK)
	{
	    lhs->lhs_lvar = &lhs->lhs_local_lvar;
	}
	இல்லையெனில்
	{
	    CLEAR_FIELD(lhs->lhs_arg_lvar);
	    எனில் (arg_exists(var_start, lhs->lhs_varlen,
			&lhs->lhs_arg_lvar.lv_idx, &lhs->lhs_arg_lvar.lv_type,
			&lhs->lhs_arg_lvar.lv_from_outer, cctx) == OK)
	    {
		எனில் (is_decl)
		{
		    semsg(_(e_str_is_used_as_argument), lhs->lhs_name);
		    திரும்பு FAIL;
		}
		lhs->lhs_lvar = &lhs->lhs_arg_lvar;
	    }
	}

	எனில் (lhs->lhs_lvar != வெற்று)
	{
	    எனில் (is_decl)
	    {
		// if we come here with what looks like an assignment like
		// .= but which has been rejected by assignment_len() from
		// may_compile_assignment give a better error message
		char_u *p = skipwhite(lhs->lhs_end);
		எனில் (p[0] == '.' && p[1] == '=')
		    emsg(_(e_dot_equal_not_supported_with_script_version_two));
		இல்லையெனில் எனில் (p[0] == ':' && !has_cmd)
		    // type specified in an assignment without "var"
		    semsg(_(e_trailing_characters_str), p);
		இல்லையெனில்
		    semsg(_(e_variable_already_declared_str), lhs->lhs_name);
		திரும்பு FAIL;
	    }
	}
	இல்லையெனில் எனில் ((lhs->lhs_classmember_idx = cctx_class_member_idx(
			cctx, var_start, lhs->lhs_varlen, &defcl)) >= 0)
	{
	    எனில் (compile_lhs_class_variable(cctx, lhs, defcl, is_decl)
		    == FAIL)
		திரும்பு FAIL;
	}
	இல்லையெனில்
	{
	    எனில் (compile_lhs_script_var(cctx, lhs, var_start, var_end,
			is_decl) == FAIL)
		திரும்பு FAIL;
	}
    }

    எனில் (declare_error)
    {
	vim9_declare_error(lhs->lhs_name);
	திரும்பு FAIL;
    }

    திரும்பு OK;
}

/*
 * When compiling a LHS variable name, for a class or an object, set the LHS
 * member type.
 */
    நிலையான முழு
compile_lhs_set_oc_member_type(
    cctx_T	*cctx,
    lhs_T	*lhs,
    char_u	*var_start)
{
    class_T	*cl = lhs->lhs_type->tt_class;
    முழு		is_object = lhs->lhs_type->tt_type == VAR_OBJECT;
    char_u	*name = var_start + lhs->lhs_varlen + 1;
    size_t	namelen = lhs->lhs_end - var_start - lhs->lhs_varlen - 1;

    ocmember_T	*m = member_lookup(cl, lhs->lhs_type->tt_type,
	    name, namelen, &lhs->lhs_member_idx);
    எனில் (m == வெற்று)
    {
	member_not_found_செய்தி(cl, lhs->lhs_type->tt_type, name, namelen);
	திரும்பு FAIL;
    }

    எனில் (IS_ENUM(cl))
    {
	எனில் (!inside_class(cctx, cl))
	{
	    semsg(_(e_enumvalue_str_cannot_be_modified),
		    cl->class_name.string, m->ocm_name.string);
	    திரும்பு FAIL;
	}
	எனில் (lhs->lhs_type->tt_type == VAR_OBJECT &&
		lhs->lhs_member_idx < 2)
	{
	    எழுத்து *செய்தி = lhs->lhs_member_idx == 0 ?
		e_enum_str_name_cannot_be_modified :
		e_enum_str_ordinal_cannot_be_modified;
	    semsg(_(செய்தி), cl->class_name.string);
	    திரும்பு FAIL;
	}
    }

    // If it is private member variable, then accessing it outside the
    // class is not allowed.
    // If it is a read only class variable, then it can be modified
    // only inside the class where it is defined.
    எனில் ((m->ocm_access != VIM_ACCESS_ALL) &&
	    ((is_object && !inside_class(cctx, cl))
	     || (!is_object && cctx->ctx_ufunc->uf_defclass != cl)))
    {
	எழுத்து *செய்தி = (m->ocm_access == VIM_ACCESS_PRIVATE)
	    ? e_cannot_access_protected_variable_str
	    : e_variable_is_not_writable_str;
	emsg_var_cl_define(செய்தி, m->ocm_name.string, 0, cl);
	திரும்பு FAIL;
    }

    எனில் (!IS_CONSTRUCTOR_METHOD(cctx->ctx_ufunc)
	    && oc_var_check_ro(cl, m))
	திரும்பு FAIL;

    lhs->lhs_member_type = m->ocm_type;

    திரும்பு OK;
}

/*
 * When compiling a LHS variable, set the LHS variable type.
 */
    நிலையான முழு
compile_lhs_set_type(cctx_T *cctx, lhs_T *lhs, char_u *var_end, முழு is_decl)
{
    எனில் (is_decl && *skipwhite(var_end) == ':')
    {
	char_u *p;

	// parse optional type: "let var: type = expr"
	எனில் (VIM_ISWHITE(*var_end))
	{
	    semsg(_(e_no_white_space_allowed_before_colon_str), var_end);
	    திரும்பு FAIL;
	}

	எனில் (!VIM_ISWHITE(var_end[1]))
	{
	    semsg(_(e_white_space_required_after_str_str), ":", var_end);
	    திரும்பு FAIL;
	}

	p = skipwhite(var_end + 1);
	lhs->lhs_type = parse_type(&p, cctx->ctx_type_list, cctx->ctx_ufunc, cctx, TRUE);
	எனில் (lhs->lhs_type == வெற்று
		|| !valid_declaration_type(lhs->lhs_type))
	    திரும்பு FAIL;

	lhs->lhs_has_type = TRUE;
	lhs->lhs_end = p;
    }
    இல்லையெனில் எனில் (lhs->lhs_lvar != வெற்று)
	lhs->lhs_type = lhs->lhs_lvar->lv_type;

    திரும்பு OK;
}

/*
 * Returns TRUE if "lhs" is a concatenable string.
 */
    நிலையான முழு
lhs_concatenable(lhs_T *lhs)
{
    திரும்பு lhs->lhs_dest == dest_global
		|| lhs->lhs_has_index
		|| lhs->lhs_type->tt_type == VAR_STRING
		|| lhs->lhs_type->tt_type == VAR_ANY;
}

/*
 * Create a new local variable when compiling a LHS variable.
 */
    நிலையான முழு
compile_lhs_new_local_var(
    cctx_T	*cctx,
    lhs_T	*lhs,
    char_u	*var_start,
    முழு		cmdidx,
    முழு		oplen,
    முழு		is_decl,
    முழு		has_cmd,
    முழு		heredoc)
{
    எனில் (oplen > 1 && !heredoc)
    {
	// +=, /=, etc. require an existing variable
	semsg(_(e_cannot_use_operator_on_new_variable_str), lhs->lhs_name);
	திரும்பு FAIL;
    }

    எனில் (!is_decl || (lhs->lhs_has_index && !has_cmd
					&& cctx->ctx_skip != SKIP_YES))
    {
	semsg(_(e_unknown_variable_str), lhs->lhs_name);
	திரும்பு FAIL;
    }

    // Check the name is valid for a funcref.
    எனில் (lhs->lhs_type->tt_type == VAR_FUNC
				|| lhs->lhs_type->tt_type == VAR_PARTIAL)
    {
	எனில் (var_wrong_func_name(lhs->lhs_name, TRUE))
	    திரும்பு FAIL;
    }

    // New local variable.
    முழு assign;
    தேர்வு (cmdidx)
    {
	நிலை CMD_final:
	    assign = ASSIGN_FINAL; நிறுத்து;
	நிலை CMD_const:
	    assign = ASSIGN_CONST; நிறுத்து;
	இயல்பு:
	    assign = ASSIGN_VAR; நிறுத்து;
    }

    lhs->lhs_lvar = reserve_local(cctx, var_start, lhs->lhs_varlen, assign,
							lhs->lhs_type);
    எனில் (lhs->lhs_lvar == வெற்று)
	திரும்பு FAIL;

    lhs->lhs_new_local = TRUE;

    திரும்பு OK;
}

/*
 * When compiling a LHS variable name, set the LHS member type.
 */
    நிலையான முழு
compile_lhs_set_member_type(
    cctx_T	*cctx,
    lhs_T	*lhs,
    char_u	*var_start,
    முழு		is_decl,
    முழு		has_cmd)
{
    lhs->lhs_member_type = lhs->lhs_type;

    எனில் (!lhs->lhs_has_index)
	திரும்பு OK;

    char_u	*after = var_start + lhs->lhs_varlen;
    char_u	*p;

    // Something follows after the variable: "var[idx]" or "var.key".
    எனில் (is_decl && cctx->ctx_skip != SKIP_YES)
    {
	எனில் (has_cmd)
	    emsg(_(e_cannot_use_index_when_declaring_variable));
	இல்லையெனில்
	    semsg(_(e_unknown_variable_str), lhs->lhs_name);
	திரும்பு FAIL;
    }

    // Now: var_start[lhs->lhs_varlen] is '[' or '.'
    // Only the last index is used below, if there are others
    // before it generate code for the expression.  Thus for
    // "ll[1][2]" the expression is "ll[1]" and "[2]" is the index.
    க்கு (;;)
    {
	p = skip_index(after);
	எனில் (*p != '[' && *p != '.')
	{
	    lhs->lhs_varlen_total = p - var_start;
	    நிறுத்து;
	}
	after = p;
    }
    எனில் (after > var_start + lhs->lhs_varlen)
    {
	lhs->lhs_varlen = after - var_start;
	lhs->lhs_dest = dest_கோவை;
	// We don't know the type before evaluating the expression,
	// use "any" until then.  The member index is for the first name,
	// not for the last index.
	lhs->lhs_type = &t_any;
	lhs->lhs_member_idx = -1;
    }

    முழு use_class = lhs->lhs_type != வெற்று
	&& (lhs->lhs_type->tt_type == VAR_CLASS
		|| lhs->lhs_type->tt_type == VAR_OBJECT);

    எனில் (lhs->lhs_type == வெற்று
	    || (use_class ? lhs->lhs_type->tt_class == வெற்று
		: lhs->lhs_type->tt_member == வெற்று))
    {
	lhs->lhs_member_type = &t_any;
    }
    இல்லையெனில் எனில் (use_class)
    {
	// for an object or class member get the type of the member
	எனில் (compile_lhs_set_oc_member_type(cctx, lhs, var_start) == FAIL)
	    திரும்பு FAIL;
    }
    இல்லையெனில்
	lhs->lhs_member_type = lhs->lhs_type->tt_member;

    திரும்பு OK;
}

/*
 * Figure out the LHS type and other properties for an assignment or one item
 * of ":unlet" with an index.
 * Returns OK or FAIL.
 */
    முழு
compile_lhs(
	char_u	    *var_start,
	lhs_T	    *lhs,
	cmdidx_T    cmdidx,
	முழு	    heredoc,
	முழு	    has_cmd,	    // "var" before "var_start"
	முழு	    oplen,
	cctx_T	    *cctx)
{
    char_u	*var_end = வெற்று;
    முழு		is_decl = is_decl_command(cmdidx);

    எனில் (lhs_init(lhs, var_start, is_decl, heredoc, &var_end) == FAIL)
	திரும்பு FAIL;

    எனில் (cctx->ctx_skip != SKIP_YES)
    {
	// compile the LHS destination
	எனில் (compile_lhs_var_dest(cctx, lhs, cmdidx, var_start, var_end,
						is_decl, has_cmd) == FAIL)
	    திரும்பு FAIL;
    }

    // handle "a:name" as a name, not index "name" in "a"
    எனில் (lhs->lhs_varlen > 1 || var_start[lhs->lhs_varlen] != ':')
	var_end = lhs->lhs_dest_end;

    எனில் (lhs->lhs_dest != dest_option && lhs->lhs_dest != dest_func_option)
    {
	// set the LHS variable type
	எனில் (compile_lhs_set_type(cctx, lhs, var_end, is_decl) == FAIL)
	    திரும்பு FAIL;
    }

    எனில் (oplen == 3 && !heredoc && !lhs_concatenable(lhs))
    {
	emsg(_(e_can_only_concatenate_to_string));
	திரும்பு FAIL;
    }

    எனில் (lhs->lhs_lvar == வெற்று && lhs->lhs_dest == dest_local
						&& cctx->ctx_skip != SKIP_YES)
    {
	எனில் (compile_lhs_new_local_var(cctx, lhs, var_start, cmdidx, oplen,
					is_decl, has_cmd, heredoc) == FAIL)
	    திரும்பு FAIL;
    }

    எனில் (compile_lhs_set_member_type(cctx, lhs, var_start, is_decl, has_cmd)
								== FAIL)
	திரும்பு FAIL;

    திரும்பு OK;
}

/*
 * Figure out the LHS and check a few errors.
 */
    முழு
compile_assign_lhs(
	char_u	    *var_start,
	lhs_T	    *lhs,
	cmdidx_T    cmdidx,
	முழு	    is_decl,
	முழு	    heredoc,
	முழு	    has_cmd,	    // "var" before "var_start"
	முழு	    oplen,
	cctx_T	    *cctx)
{
    எனில் (compile_lhs(var_start, lhs, cmdidx, heredoc, has_cmd, oplen, cctx)
								       == FAIL)
	திரும்பு FAIL;

    எனில் (!lhs->lhs_has_index && lhs->lhs_lvar == &lhs->lhs_arg_lvar)
    {
	semsg(_(e_cannot_assign_to_argument_str), lhs->lhs_name);
	திரும்பு FAIL;
    }
    எனில் (!is_decl && lhs->lhs_lvar != வெற்று
			   && lhs->lhs_lvar->lv_const != ASSIGN_VAR
			   && !lhs->lhs_has_index)
    {
	semsg(_(e_cannot_assign_to_constant_str), lhs->lhs_name);
	திரும்பு FAIL;
    }
    திரும்பு OK;
}

/*
 * Return TRUE if "lhs" has a range index: "[expr : expr]".
 */
    நிலையான முழு
has_list_index(char_u *idx_start, cctx_T *cctx)
{
    char_u  *p = idx_start;
    முழு	    save_skip;

    எனில் (*p != '[')
	திரும்பு FALSE;

    p = skipwhite(p + 1);
    எனில் (*p == ':')
	திரும்பு TRUE;

    save_skip = cctx->ctx_skip;
    cctx->ctx_skip = SKIP_YES;
    (வெறுமை)compile_expr0(&p, cctx);
    cctx->ctx_skip = save_skip;
    திரும்பு *skipwhite(p) == ':';
}

/*
 * For an assignment with an index, compile the "idx" in "var[idx]" or "key" in
 * "var.key".
 */
    நிலையான முழு
compile_assign_index(
	char_u	*var_start,
	lhs_T	*lhs,
	முழு	*range,
	cctx_T	*cctx)
{
    size_t	varlen = lhs->lhs_varlen;
    char_u	*p;
    முழு		r = OK;
    முழு		need_white_before = TRUE;
    முழு		empty_second;

    p = var_start + varlen;
    எனில் (*p == '[')
    {
	p = skipwhite(p + 1);
	எனில் (*p == ':')
	{
	    // empty first index, push zero
	    r = generate_PUSHNR(cctx, 0);
	    need_white_before = FALSE;
	}
	இல்லையெனில்
	    r = compile_expr0(&p, cctx);

	எனில் (r == OK && *skipwhite(p) == ':')
	{
	    // unlet var[idx : idx]
	    // blob[idx : idx] = value
	    *range = TRUE;
	    p = skipwhite(p);
	    empty_second = *skipwhite(p + 1) == ']';
	    எனில் ((need_white_before && !IS_WHITE_OR_NUL(p[-1]))
		    || (!empty_second && !IS_WHITE_OR_NUL(p[1])))
	    {
		semsg(_(e_white_space_required_before_and_after_str_at_str),
								      ":", p);
		திரும்பு FAIL;
	    }
	    p = skipwhite(p + 1);
	    எனில் (*p == ']')
		// empty second index, push "none"
		r = generate_PUSHSPEC(cctx, VVAL_NONE);
	    இல்லையெனில்
		r = compile_expr0(&p, cctx);
	}

	எனில் (r == OK && *skipwhite(p) != ']')
	{
	    // this should not happen
	    emsg(_(e_missing_closing_square_brace));
	    r = FAIL;
	}
    }
    இல்லையெனில் எனில் (lhs->lhs_member_idx >= 0)
    {
	// object member index
	r = generate_PUSHNR(cctx, lhs->lhs_member_idx);
    }
    இல்லையெனில் // if (*p == '.')
    {
	char_u *சாவி_end = to_name_end(p + 1, TRUE);
	char_u *சாவி = vim_strnsave(p + 1, சாவி_end - p - 1);

	r = generate_PUSHS(cctx, &சாவி);
    }
    திரும்பு r;
}

/*
 * For a LHS with an index, load the variable to be indexed.
 */
    நிலையான முழு
compile_load_lhs(
	lhs_T	*lhs,
	char_u	*var_start,
	type_T	*rhs_type,
	cctx_T	*cctx)
{
    எனில் (lhs->lhs_dest == dest_கோவை)
    {
	size_t	    varlen = lhs->lhs_varlen;
	முழு	    c = var_start[varlen];
	முழு	    வரிகள்_len = cctx->ctx_ufunc->uf_வரிகள்.ga_len;
	முழு	    res;

	// Evaluate "ll[expr]" of "ll[expr][idx]".  End the line with a NUL and
	// limit the lines array length to avoid skipping to a following line.
	var_start[varlen] = NUL;
	cctx->ctx_ufunc->uf_வரிகள்.ga_len = cctx->ctx_lnum + 1;
	char_u *p = var_start;
	res = compile_expr0(&p, cctx);
	var_start[varlen] = c;
	cctx->ctx_ufunc->uf_வரிகள்.ga_len = வரிகள்_len;
	எனில் (res == FAIL || p != var_start + varlen)
	{
	    // this should not happen
	    எனில் (res != FAIL)
		emsg(_(e_missing_closing_square_brace));
	    திரும்பு FAIL;
	}

	lhs->lhs_type = cctx->ctx_type_அடுக்கு.ga_len == 0 ? &t_void
						  : get_type_on_அடுக்கு(cctx, 0);

	எனில் (lhs->lhs_type->tt_type == VAR_CLASS
		|| (lhs->lhs_type->tt_type == VAR_OBJECT
		    && lhs->lhs_type != &t_object_any))
	{
	    // Check whether the class or object variable is modifiable
	    எனில் (!lhs_class_member_modifiable(lhs, var_start, cctx))
		திரும்பு FAIL;
	}

	// Now we can properly check the type.  The variable is indexed, thus
	// we need the member type.  For a class or object we don't know the
	// type yet, it depends on what member is used.
	// The top item in the stack is the Dict, followed by the key and then
	// the type of the value.
	vartype_T vartype = lhs->lhs_type->tt_type;
	type_T *member_type = lhs->lhs_type->tt_member;
	எனில் (rhs_type != வெற்று && member_type != வெற்று
		&& vartype != VAR_OBJECT && vartype != VAR_CLASS
		&& rhs_type != &t_void
		&& need_type(rhs_type, member_type, 0,
					    -3, 0, cctx, FALSE, FALSE) == FAIL)
	    திரும்பு FAIL;

	திரும்பு OK;
    }

    திரும்பு generate_loadvar(cctx, lhs);
}

/*
 * Produce code for loading "lhs" and also take care of an index.
 * Return OK/FAIL.
 */
    முழு
compile_load_lhs_with_index(lhs_T *lhs, char_u *var_start, cctx_T *cctx)
{
    எனில் (lhs->lhs_type->tt_type == VAR_OBJECT)
    {
	// "this.value": load "this" object and get the value at index for an
	// object or class member get the type of the member.
	// Also for "obj.value".
	char_u *dot = vim_strchr(var_start, '.');
	எனில் (dot == வெற்று)
	{
	    semsg(_(e_missing_dot_after_object_str), lhs->lhs_name);
	    திரும்பு FAIL;
	}

	class_T	*cl = lhs->lhs_type->tt_class;
	type_T	*type = oc_member_type(cl, TRUE, dot + 1,
					  lhs->lhs_end, &lhs->lhs_member_idx);
	எனில் (lhs->lhs_member_idx < 0)
	    திரும்பு FAIL;

	எனில் (dot - var_start == 4 && STRNCMP(var_start, "this", 4) == 0)
	{
	    // load "this"
	    lvar_T  *lvar = lhs->lhs_lvar;
	    முழு	    rc;

	    எனில் (lvar->lv_from_outer > 0)
		rc = generate_LOADOUTER(cctx, lvar->lv_idx,
			lvar->lv_from_outer, lvar->lv_loop_depth,
			lvar->lv_loop_idx, type);
	    இல்லையெனில்
		rc = generate_LOAD(cctx, ISN_LOAD, lvar->lv_idx, வெற்று, type);

	    எனில் (rc == FAIL)
		திரும்பு FAIL;
	}
	இல்லையெனில்
	{
	    // load object variable or argument
	    எனில் (compile_load_lhs(lhs, var_start, lhs->lhs_type, cctx) == FAIL)
		திரும்பு FAIL;
	}
	எனில் (IS_INTERFACE(cl))
	    திரும்பு generate_GET_ITF_MEMBER(cctx, cl, lhs->lhs_member_idx, type);
	திரும்பு generate_GET_OBJ_MEMBER(cctx, lhs->lhs_member_idx, type);
    }
    இல்லையெனில் எனில் (lhs->lhs_type->tt_type == VAR_CLASS)
    {
	// "<classname>.value": load class variable "classname.value"
	char_u *dot = vim_strchr(var_start, '.');
	எனில் (dot == வெற்று)
	{
	    (வெறுமை)check_type_is_மதிப்பு(lhs->lhs_type);
	    திரும்பு FAIL;
	}

	class_T	*cl = lhs->lhs_type->tt_class;
	ocmember_T *m = class_member_lookup(cl, dot + 1,
						lhs->lhs_end - dot - 1,
						&lhs->lhs_member_idx);
	எனில் (m == வெற்று)
	    திரும்பு FAIL;

	திரும்பு generate_CLASSMEMBER(cctx, TRUE, cl, lhs->lhs_member_idx);
    }

    எனில் (compile_load_lhs(lhs, var_start, வெற்று, cctx) == FAIL)
	திரும்பு FAIL;

    எனில் (lhs->lhs_has_index)
    {
	முழு	range = FALSE;
	type_T	*type = get_type_on_அடுக்கு(cctx, 0);

	// A member of an object or class is not obtained by indexing it.
	எனில் (type->tt_type == VAR_CLASS
		|| (type->tt_type == VAR_OBJECT && type != &t_object_any))
	{
	    char_u *p = var_start + lhs->lhs_varlen;

	    திரும்பு compile_class_object_index(cctx, &p, type);
	}

	// Get member from list or dict.  First compile the
	// index value.
	எனில் (compile_assign_index(var_start, lhs, &range, cctx) == FAIL)
	    திரும்பு FAIL;
	எனில் (range)
	{
	    semsg(_(e_cannot_use_range_with_assignment_operator_str),
								    var_start);
	    திரும்பு FAIL;
	}

	// Get the member.
	எனில் (compile_member(FALSE, வெற்று, cctx) == FAIL)
	    திரும்பு FAIL;
    }
    திரும்பு OK;
}

/*
 * Assignment to a list or dict member, or ":unlet" for the item, using the
 * information in "lhs".
 * Returns OK or FAIL.
 */
    முழு
compile_assign_unlet(
    char_u	*var_start,
    lhs_T	*lhs,
    முழு		is_assign,
    type_T	*rhs_type,
    cctx_T	*cctx)
{
    vartype_T	dest_type;
    முழு		range = FALSE;

    எனில் (compile_assign_index(var_start, lhs, &range, cctx) == FAIL)
	திரும்பு FAIL;
    எனில் (is_assign && range
	    && lhs->lhs_type->tt_type != VAR_LIST
	    && lhs->lhs_type != &t_blob
	    && lhs->lhs_type != &t_any)
    {
	எனில் (lhs->lhs_type->tt_type == VAR_TUPLE)
	    emsg(_(e_cannot_slice_tuple));
	இல்லையெனில்
	    semsg(_(e_cannot_use_range_with_assignment_str), var_start);
	திரும்பு FAIL;
    }

    // For "expr[idx]" the index is compiled before the expression, thus the
    // resulting type cannot be used here.
    எனில் (lhs->lhs_dest == dest_கோவை
	    || lhs->lhs_type == வெற்று || lhs->lhs_type == &t_any)
    {
	// Index on variable of unknown type: check at runtime.
	dest_type = VAR_ANY;
    }
    இல்லையெனில்
    {
	dest_type = lhs->lhs_type->tt_type;
	எனில் (dest_type == VAR_DICT && range)
	{
	    emsg(_(e_cannot_use_range_with_dictionary));
	    திரும்பு FAIL;
	}
	எனில் (dest_type == VAR_DICT
		&& may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
	    திரும்பு FAIL;
	எனில் (dest_type == VAR_LIST || dest_type == VAR_BLOB)
	{
	    type_T *type;

	    எனில் (range)
	    {
		type = get_type_on_அடுக்கு(cctx, 1);
		எனில் (need_type(type, &t_எண், 0,
					    -2, 0, cctx, FALSE, FALSE) == FAIL)
		திரும்பு FAIL;
	    }
	    type = get_type_on_அடுக்கு(cctx, 0);
	    எனில் ((dest_type != VAR_BLOB && type->tt_type != VAR_SPECIAL)
		    && need_type(type, &t_எண், 0,
					    -1, 0, cctx, FALSE, FALSE) == FAIL)
		திரும்பு FAIL;
	}
    }

    எனில் (cctx->ctx_skip == SKIP_YES)
	திரும்பு OK;

    // Load the dict, list or object.  On the stack we then have:
    // - value (for assignment, not for :unlet)
    // - index
    // - for [a : b] second index
    // - variable
    எனில் (compile_load_lhs(lhs, var_start, rhs_type, cctx) == FAIL)
	திரும்பு FAIL;

    எனில் (dest_type == VAR_LIST
	    || dest_type == VAR_DICT
	    || dest_type == VAR_BLOB
	    || dest_type == VAR_CLASS
	    || dest_type == VAR_OBJECT
	    || dest_type == VAR_ANY)
    {
	எனில் (is_assign)
	{
	    எனில் (range)
	    {
		எனில் (generate_instr_drop(cctx, ISN_STORERANGE, 4) == வெற்று)
		    திரும்பு FAIL;
	    }
	    இல்லையெனில்
	    {
		isn_T	*isn = generate_instr_drop(cctx, ISN_STOREINDEX, 3);

		எனில் (isn == வெற்று)
		    திரும்பு FAIL;
		isn->isn_arg.storeindex.si_vartype = dest_type;
		isn->isn_arg.storeindex.si_class = வெற்று;

		எனில் (dest_type == VAR_OBJECT)
		{
		    class_T *cl = lhs->lhs_type->tt_class;

		    எனில் (IS_INTERFACE(cl))
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
	இல்லையெனில் எனில் (range)
	{
	    எனில் (generate_instr_drop(cctx, ISN_UNLETRANGE, 3) == வெற்று)
		திரும்பு FAIL;
	}
	இல்லையெனில்
	{
	    எனில் (generate_instr_drop(cctx, ISN_UNLETINDEX, 2) == வெற்று)
		திரும்பு FAIL;
	}
    }
    இல்லையெனில்
    {
	எனில் (dest_type == VAR_TUPLE)
	    emsg(_(e_tuple_is_immutable));
	இல்லையெனில்
	    emsg(_(e_indexable_type_required));
	திரும்பு FAIL;
    }

    திரும்பு OK;
}

/*
 * Generate an instruction to push the default value for "vartype".
 * if "dest_local" is TRUE then for some types no instruction is generated.
 * "skip_store" is set to TRUE if no PUSH instruction is generated.
 * Returns OK or FAIL.
 */
    நிலையான முழு
தள்ளு_default_மதிப்பு(
	cctx_T	    *cctx,
	vartype_T   vartype,
	முழு	    dest_is_local,
	முழு	    *skip_store)
{
    முழு r = OK;

    தேர்வு (vartype)
    {
	நிலை VAR_BOOL:
	    r = generate_PUSHBOOL(cctx, VVAL_FALSE);
	    நிறுத்து;
	நிலை VAR_FLOAT:
	    r = generate_PUSHF(cctx, 0.0);
	    நிறுத்து;
	நிலை VAR_STRING:
	    r = generate_PUSHS(cctx, வெற்று);
	    நிறுத்து;
	நிலை VAR_BLOB:
	    r = generate_PUSHBLOB(cctx, blob_alloc());
	    நிறுத்து;
	நிலை VAR_FUNC:
	    r = generate_PUSHFUNC(cctx, வெற்று, &t_func_void, TRUE);
	    நிறுத்து;
	நிலை VAR_LIST:
	    r = generate_NEWLIST(cctx, 0, FALSE);
	    நிறுத்து;
	நிலை VAR_TUPLE:
	    r = generate_NEWTUPLE(cctx, 0, FALSE);
	    நிறுத்து;
	நிலை VAR_DICT:
	    r = generate_NEWDICT(cctx, 0, FALSE);
	    நிறுத்து;
	நிலை VAR_JOB:
	    r = generate_PUSHJOB(cctx);
	    நிறுத்து;
	நிலை VAR_CHANNEL:
	    r = generate_PUSHCHANNEL(cctx);
	    நிறுத்து;
	நிலை VAR_OBJECT:
	    r = generate_PUSHOBJ(cctx);
	    நிறுத்து;
	நிலை VAR_எண்:
	நிலை VAR_UNKNOWN:
	நிலை VAR_ANY:
	நிலை VAR_PARTIAL:
	நிலை VAR_VOID:
	நிலை VAR_INSTR:
	நிலை VAR_CLASS:
	நிலை VAR_TYPEALIAS:
	நிலை VAR_SPECIAL:  // cannot happen
	    // This is skipped for local variables, they are always
	    // initialized to zero.  But in a "for" or "while" loop
	    // the value may have been changed.
	    எனில் (dest_is_local && !inside_loop_scope(cctx))
		*skip_store = TRUE;
	    இல்லையெனில்
		r = generate_PUSHNR(cctx, 0);
	    நிறுத்து;
    }
    திரும்பு r;
}

/*
 * Compile assignment context.  Used when compiling an assignment statement.
 */
வகைவரை கட்டமைப்பு cac_S cac_T;
கட்டமைப்பு cac_S
{
    cmdidx_T	cac_cmdidx;		// assignment command
    char_u	*cac_nextc;		// next character to parse
    lhs_T	cac_lhs;		// lhs of the assignment
    type_T	*cac_rhs_type;		// rhs type of an assignment
    char_u	*cac_op;		// assignment operator
    முழு		cac_oplen;		// assignment operator length
    char_u	*cac_var_start;		// start of the variable names
    char_u	*cac_var_end;		// end of the variable names
    முழு		cac_var_மொத்தம்;		// number of variables in assignment
    முழு		cac_var_idx;		// variable index in a list
    முழு		cac_semicolon;		// semicolon in [var1, var2; var3]
    garray_T	*cac_instr;
    முழு		cac_instr_மொத்தம்;
    முழு		cac_incdec;
    முழு		cac_did_generate_slice;
    முழு		cac_is_decl;
    முழு		cac_is_const;
    முழு		cac_start_lnum;
    type_T	*cac_inferred_type;
    முழு		cac_skip_store;
};

/*
 * Initialize the compile assignment context.
 */
    நிலையான வெறுமை
compile_assign_context_init(cac_T *cac, cctx_T *cctx, முழு cmdidx, char_u *arg)
{
    CLEAR_FIELD(*cac);
    cac->cac_cmdidx = cmdidx;
    cac->cac_instr = &cctx->ctx_instr;
    cac->cac_rhs_type = &t_any;
    cac->cac_is_decl = is_decl_command(cmdidx);
    cac->cac_start_lnum = SOURCING_LNUM;
    cac->cac_instr_மொத்தம் = -1;
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
    நிலையான முழு
compile_assign_obj_new_arg(char_u **argp, cctx_T *cctx)
{
    char_u *arg = *argp;

    arg += 11;	    // skip "ifargisset"
    முழு def_arg_idx = getdigits(&arg);
    arg = skipwhite(arg);

    // Use a JUMP_IF_ARG_NOT_SET instruction to skip if the value was not
    // given and the default value is "v:none".
    முழு அடுக்கு_offset = அடுக்கு_FRAME_SIZE +
				(cctx->ctx_ufunc->uf_va_name != வெற்று ? 1 : 0);
    முழு def_arg_மொத்தம் = cctx->ctx_ufunc->uf_def_args.ga_len;
    முழு arg_offset = def_arg_idx - def_arg_மொத்தம் - அடுக்கு_offset;

    எனில் (generate_JUMP_IF_ARG(cctx, ISN_JUMP_IF_ARG_NOT_SET,
			     arg_offset) == FAIL)
	திரும்பு FAIL;

    *argp = arg;
    திரும்பு OK;
}

/*
 * Translate the increment (++) and decrement (--) operators to the
 * corresponding compound operators (+= or -=).
 *
 * Returns OK on success and FAIL on syntax error.
 */
    நிலையான முழு
translate_incdec_op(exarg_T *eap, cac_T *cac)
{
    எனில் (VIM_ISWHITE(eap->cmd[2]))
    {
	semsg(_(e_no_white_space_allowed_after_str_str),
		eap->cmdidx == CMD_increment ? "++" : "--", eap->cmd);
	திரும்பு FAIL;
    }
    cac->cac_op = (char_u *)(eap->cmdidx == CMD_increment ? "+=" : "-=");
    cac->cac_oplen = 2;
    cac->cac_incdec = TRUE;

    திரும்பு OK;
}

/*
 * Process the operator in an assignment statement.
 */
    நிலையான முழு
compile_assign_process_operator(
    exarg_T	*eap,
    char_u	*arg,
    cac_T	*cac,
    முழு		*heredoc,
    char_u	**retstr)
{
    *retstr = வெற்று;

    எனில் (eap->cmdidx == CMD_increment || eap->cmdidx == CMD_decrement)
	// Change an unary operator to a compound operator
	திரும்பு translate_incdec_op(eap, cac);

    char_u *sp = cac->cac_nextc;
    cac->cac_nextc = skipwhite(cac->cac_nextc);
    cac->cac_op = cac->cac_nextc;
    cac->cac_oplen = assignment_len(cac->cac_nextc, heredoc);

    எனில் (cac->cac_var_மொத்தம் > 0 && cac->cac_oplen == 0)
    {
	// can be something like "[1, 2]->func()"
	*retstr = arg;
	திரும்பு FAIL;
    }

    // need white space before and after the operator
    எனில் (cac->cac_oplen > 0 && (!VIM_ISWHITE(*sp)
		|| !IS_WHITE_OR_NUL(cac->cac_op[cac->cac_oplen])))
    {
	error_white_both(cac->cac_op, cac->cac_oplen);
	திரும்பு FAIL;
    }

    திரும்பு OK;
}

/*
 * Find the start of an assignment statement.
 */
    நிலையான char_u *
compile_assign_compute_start(char_u *arg, முழு var_மொத்தம்)
{
    எனில் (var_மொத்தம் > 0)
	// [var1, var2] = [val1, val2]
	// skip over the "["
	திரும்பு skipwhite(arg + 1);

    திரும்பு arg;
}

/*
 * Parse a heredoc assignment starting at "p".  Returns a pointer to the
 * beginning of the heredoc content.
 */
    நிலையான char_u *
parse_heredoc_assignment(exarg_T *eap, cctx_T *cctx, cac_T *cac)
{
    // [let] varname =<< [trim] {end}
    eap->ea_getline = exarg_getline;
    eap->cookie = cctx;

    list_T *l = heredoc_get(eap, cac->cac_nextc + 3, FALSE, TRUE);
    எனில் (l == வெற்று)
	திரும்பு வெற்று;

    list_free(l);
    cac->cac_nextc += STRLEN(cac->cac_nextc);

    திரும்பு cac->cac_nextc;
}

/*
 * Check the type of a RHS expression in a list assignment statement.
 * The RHS expression is already compiled.  So the type is on the stack.
 */
    நிலையான முழு
compile_assign_list_check_rhs_type(cctx_T *cctx, cac_T *cac)
{
    type_T	*stacktype;

    stacktype = cctx->ctx_type_அடுக்கு.ga_len == 0 ? &t_void
						: get_type_on_அடுக்கு(cctx, 0);
    எனில் (stacktype->tt_type == VAR_VOID)
    {
	emsg(_(e_cannot_use_void_மதிப்பு));
	திரும்பு FAIL;
    }

    எனில் (stacktype->tt_type != VAR_LIST && stacktype->tt_type != VAR_TUPLE
					&& stacktype->tt_type != VAR_ANY)
    {
	emsg(_(e_list_or_tuple_required));
	திரும்பு FAIL;
    }

    எனில் (need_type(stacktype,
		  stacktype->tt_type == VAR_TUPLE ? &t_tuple_any : &t_list_any,
		  TYPECHK_TUPLE_OK, -1, 0, cctx, FALSE, FALSE) == FAIL)
	திரும்பு FAIL;

    // The check accepts both a list and a tuple. Keep "any", making it a list
    // would reject a tuple later on.
    எனில் (stacktype->tt_type == VAR_ANY)
	set_type_on_அடுக்கு(cctx, &t_any, 0);

    எனில் (stacktype->tt_type == VAR_TUPLE)
    {
	எனில் (stacktype->tt_argcount != 1)
	    cac->cac_rhs_type = &t_any;
	இல்லையெனில்
	{
	    எனில் (stacktype->tt_flags & TTFLAG_VARARGS)
		cac->cac_rhs_type = stacktype->tt_args[0]->tt_member;
	    இல்லையெனில்
		cac->cac_rhs_type = stacktype->tt_args[0];
	}
    }
    இல்லையெனில் எனில் (stacktype->tt_member != வெற்று)
	cac->cac_rhs_type = stacktype->tt_member;

    திரும்பு OK;
}

/*
 * In a list assignment statement, if a constant list was used, check the
 * length.  Returns OK if the length check succeeds.  Returns FAIL otherwise.
 */
    நிலையான முழு
compile_assign_list_check_length(cctx_T *cctx, cac_T *cac)
{
    முழு	needed_list_len;
    முழு	did_check = FALSE;

    needed_list_len = cac->cac_semicolon
				? cac->cac_var_மொத்தம் - 1
				: cac->cac_var_மொத்தம்;
    எனில் (cac->cac_instr->ga_len > 0)
    {
	isn_T	*isn = ((isn_T *)cac->cac_instr->ga_data) +
						cac->cac_instr->ga_len - 1;

	எனில் (isn->isn_type == ISN_NEWLIST || isn->isn_type == ISN_NEWTUPLE)
	{
	    did_check = TRUE;
	    எனில் (cac->cac_semicolon ?
			isn->isn_arg.எண் < needed_list_len
			: isn->isn_arg.எண் != needed_list_len)
	    {
		semsg(_(e_expected_nr_பொருட்கள்_but_got_nr),
			needed_list_len, (முழு)isn->isn_arg.எண்);
		திரும்பு FAIL;
	    }
	}
    }

    எனில் (!did_check)
	generate_CHECKLEN(cctx, needed_list_len, cac->cac_semicolon);

    திரும்பு OK;
}

/*
 * Evaluate the expression for "[var, var] = expr" assignment.
 * A line break may follow the assignment operator "=".
 */
    நிலையான char_u *
compile_assign_list_கோவை(cctx_T *cctx, cac_T *cac)
{
    char_u *whitep;

    whitep = cac->cac_op + cac->cac_oplen;

    எனில் (may_get_next_வரி_error(whitep, &cac->cac_nextc, cctx) == FAIL)
	திரும்பு வெற்று;

    // compile RHS expression
    எனில் (compile_expr0(&cac->cac_nextc, cctx) == FAIL)
	திரும்பு வெற்று;

    எனில் (cctx->ctx_skip == SKIP_YES)
	// no need to parse more when skipping
	திரும்பு cac->cac_nextc;

    எனில் (compile_assign_list_check_rhs_type(cctx, cac) == FAIL)
	திரும்பு வெற்று;

    // If a constant list was used we can check the length right here.
    எனில் (compile_assign_list_check_length(cctx, cac) == FAIL)
	திரும்பு FAIL;

    திரும்பு cac->cac_nextc;
}

/*
 * Find and return the end of a heredoc or a list of variables assignment
 * statement.  For a single variable assignment statement, returns the current
 * end.
 * Returns NULL on failure.
 */
    நிலையான char_u *
compile_assign_compute_end(
    exarg_T	*eap,
    cctx_T	*cctx,
    cac_T	*cac,
    முழு		heredoc)
{
    எனில் (heredoc)
    {
	cac->cac_nextc = parse_heredoc_assignment(eap, cctx, cac);
	திரும்பு cac->cac_nextc;
    }

    எனில் (cac->cac_var_மொத்தம் > 0)
    {
	// for "[var, var] = expr" evaluate the expression. The list of
	// variables are processed later.
	// A line break may follow the "=".
	cac->cac_nextc = compile_assign_list_கோவை(cctx, cac);
	திரும்பு cac->cac_nextc;
    }

    திரும்பு cac->cac_var_end;
}

/*
 * For "var = expr" evaluate the expression.
 */
    நிலையான முழு
compile_assign_single_eval_கோவை(cctx_T *cctx, cac_T *cac)
{
    முழு		ret = OK;
    char_u	*whitep;
    lhs_T	*lhs = &cac->cac_lhs;
    lvar_T	*lvp;
    lvar_T	save_lhs_lvar;

    // Compile the expression.
    எனில் (cac->cac_incdec)
	திரும்பு generate_PUSHNR(cctx, 1);

    // Temporarily hide the new local variable here, it is
    // not available to this expression.
    எனில் (lhs->lhs_new_local)
    {
	--cctx->ctx_locals.ga_len;

	// Save the local variable value (compiling the RHS expression may
	// create new local variables).
	lvp = ((lvar_T *)cctx->ctx_locals.ga_data) + cctx->ctx_locals.ga_len;
	save_lhs_lvar = *lvp;
    }
    whitep = cac->cac_op + cac->cac_oplen;

    எனில் (may_get_next_வரி_error(whitep, &cac->cac_nextc, cctx) == FAIL)
    {
	எனில் (lhs->lhs_new_local)
	    ++cctx->ctx_locals.ga_len;
	திரும்பு FAIL;
    }

    ret = compile_expr0_ext(&cac->cac_nextc, cctx, &cac->cac_is_const);
    எனில் (lhs->lhs_new_local)
    {
	// Restore the local variable value.  Update lhs_lvar as the index of
	// the local variable might have changed.
	lvp = ((lvar_T *)cctx->ctx_locals.ga_data) + cctx->ctx_locals.ga_len;
	*lvp = save_lhs_lvar;
	lhs->lhs_lvar = lvp;

	++cctx->ctx_locals.ga_len;
    }

    திரும்பு ret;
}

/*
 * When compiling an assignment, set the LHS type to the RHS type.
 */
    நிலையான முழு
compile_assign_set_lhs_type_from_rhs(
    cctx_T	*cctx,
    cac_T	*cac,
    lhs_T	*lhs,
    type_T	*rhs_type)
{
    எனில் (rhs_type->tt_type == VAR_VOID)
    {
	emsg(_(e_cannot_use_void_மதிப்பு));
	திரும்பு FAIL;
    }

    type_T *type;

    // An empty list or dict has a &t_unknown member, for a variable that
    // implies &t_any.
    எனில் (rhs_type == &t_list_empty)
	type = &t_list_any;
    இல்லையெனில் எனில் (rhs_type == &t_dict_empty)
	type = &t_dict_any;
    இல்லையெனில் எனில் (rhs_type == &t_unknown)
	type = &t_any;
    இல்லையெனில்
    {
	type = rhs_type;
	cac->cac_inferred_type = rhs_type;
    }

    set_var_type(lhs->lhs_lvar, type, cctx);

    திரும்பு OK;
}

/*
 * Returns TRUE if the "rhs_type" can be assigned to the "lhs" variable.
 * Used when compiling an assignment statement.
 */
    நிலையான முழு
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
    எனில் (cac->cac_var_மொத்தம் > 0)
    {
	where.wt_index = cac->cac_var_idx + 1;
	where.wt_kind = WT_VARIABLE;
    }

    // If assigning to a list or dict member, use the member type.
    // Not for "list[:] =".
    எனில் (lhs->lhs_has_index &&
	    !has_list_index(cac->cac_var_start + lhs->lhs_varlen, cctx))
	use_type = lhs->lhs_member_type;

    எனில் (need_type_where(rhs_type, use_type, 0, -1, where, cctx, FALSE,
						cac->cac_is_const) == FAIL)
	திரும்பு FALSE;

    திரும்பு TRUE;
}

/*
 * Compare the LHS type with the RHS type in an assignment.
 */
    நிலையான முழு
compile_assign_check_type(cctx_T *cctx, cac_T *cac)
{
    lhs_T	*lhs = &cac->cac_lhs;
    type_T	*rhs_type;

    rhs_type = cctx->ctx_type_அடுக்கு.ga_len == 0
					? &t_void
					: get_type_on_அடுக்கு(cctx, 0);
    cac->cac_rhs_type = rhs_type;

    எனில் (check_type_is_மதிப்பு(rhs_type) == FAIL)
	திரும்பு FAIL;

    எனில் (lhs->lhs_lvar != வெற்று && (cac->cac_is_decl || !lhs->lhs_has_type))
    {
	எனில் (rhs_type->tt_type == VAR_FUNC
					|| rhs_type->tt_type == VAR_PARTIAL)
	{
	    // Make sure the variable name can be used as a funcref
	    எனில் (!lhs->lhs_has_index
				&& var_wrong_func_name(lhs->lhs_name, TRUE))
		திரும்பு FAIL;
	}

	எனில் (lhs->lhs_new_local && !lhs->lhs_has_type)
	{
	    // The LHS variable doesn't have a type.  Set it to the RHS type.
	    எனில் (compile_assign_set_lhs_type_from_rhs(cctx, cac, lhs, rhs_type)
								== FAIL)
		திரும்பு FAIL;
	}
	இல்லையெனில் எனில் (*cac->cac_op == '=')
	{
	    எனில் (!compile_assign_valid_rhs_type(cctx, cac, lhs, rhs_type))
		திரும்பு FAIL;
	}
    }
    இல்லையெனில்
    {
	// Assigning to a register using @r = "abc"

	type_T *lhs_type = lhs->lhs_member_type;

	// Special case: assigning to @# can use a number or a string.
	// Also: can assign a number to a float.
	எனில் ((lhs_type == &t_எண்_or_string || lhs_type == &t_float)
					&& rhs_type->tt_type == VAR_எண்)
	    lhs_type = &t_எண்;

	எனில் (*cac->cac_nextc != '=')
	{
	    எனில் (need_type(rhs_type, lhs_type, 0, -1, 0, cctx, FALSE,
							FALSE) == FAIL)
		திரும்பு FAIL;
	}
    }

    திரும்பு OK;
}

/*
 * Compile the RHS expression in an assignment statement and generate the
 * instructions.
 */
    நிலையான முழு
compile_assign_rhs_கோவை(cctx_T *cctx, cac_T *cac)
{
    cac->cac_is_const = FALSE;

    // for "+=", "*=", "..=" etc. first load the current value
    எனில் (*cac->cac_op != '='
	    && compile_load_lhs_with_index(&cac->cac_lhs, cac->cac_var_start,
								cctx) == FAIL)
	திரும்பு FAIL;

    // For "var = expr" evaluate the expression.
    எனில் (cac->cac_var_மொத்தம் == 0)
    {
	முழு	ret;

	// Compile the expression.
	cac->cac_instr_மொத்தம் = cac->cac_instr->ga_len;
	ret = compile_assign_single_eval_கோவை(cctx, cac);
	எனில் (ret == FAIL)
	    திரும்பு FAIL;
    }
    இல்லையெனில் எனில் (cac->cac_semicolon && cac->cac_var_idx == cac->cac_var_மொத்தம் - 1)
    {
	// For "[var; var] = expr" get the rest of the list
	cac->cac_did_generate_slice = TRUE;
	எனில் (generate_SLICE(cctx, cac->cac_var_மொத்தம் - 1) == FAIL)
	    திரும்பு FAIL;
    }
    இல்லையெனில்
    {
	// For "[var, var] = expr" get the "var_idx" item from the
	// list.
	முழு with_op = *cac->cac_op != '=';
	எனில் (generate_GETITEM(cctx, cac->cac_var_idx, with_op) == FAIL)
	    திரும்பு FAIL;
    }

    எனில் (compile_assign_check_type(cctx, cac) == FAIL)
	திரும்பு FAIL;

    திரும்பு OK;
}

/*
 * Compile the RHS expression in an assignment
 */
    நிலையான முழு
compile_assign_rhs(cctx_T *cctx, cac_T *cac)
{
    lhs_T	*lhs = &cac->cac_lhs;

    எனில் (cctx->ctx_skip == SKIP_YES)
    {
	எனில் (cac->cac_oplen > 0 && cac->cac_var_மொத்தம் == 0)
	{
	    // skip over the "=" and the expression
	    cac->cac_nextc = skipwhite(cac->cac_op + cac->cac_oplen);
	    (வெறுமை)compile_expr0(&cac->cac_nextc, cctx);
	}
	திரும்பு OK;
    }

    // If RHS is specified, then generate instructions for RHS expression
    எனில் (cac->cac_oplen > 0)
	திரும்பு compile_assign_rhs_கோவை(cctx, cac);

    எனில் (cac->cac_cmdidx == CMD_final)
    {
	emsg(_(e_final_requires_a_மதிப்பு));
	திரும்பு FAIL;
    }

    எனில் (cac->cac_cmdidx == CMD_const)
    {
	emsg(_(e_const_requires_a_மதிப்பு));
	திரும்பு FAIL;
    }

    எனில் (!lhs->lhs_has_type || lhs->lhs_dest == dest_option
					|| lhs->lhs_dest == dest_func_option)
    {
	emsg(_(e_type_or_initialization_required));
	திரும்பு FAIL;
    }

    // variables are always initialized
    எனில் (GA_GROW_FAILS(cac->cac_instr, 1))
	திரும்பு FAIL;

    cac->cac_instr_மொத்தம் = cac->cac_instr->ga_len;

    திரும்பு தள்ளு_default_மதிப்பு(cctx, lhs->lhs_member_type->tt_type,
			      lhs->lhs_dest == dest_local,
			      &cac->cac_skip_store);
}

/*
 * Returns OK if "type" supports compound operator "op_arg" (e.g. +=, -=, %=,
 * etc.).  Compound operators are not supported with a tuple and a dict.
 * Returns FAIL if compound operator is not supported.
 */
    நிலையான முழு
check_type_supports_compound_op(type_T *type, char_u op_arg)
{
    எனில் (type->tt_type == VAR_TUPLE || type->tt_type == VAR_DICT)
    {
	char_u	op[2];

	op[0] = op_arg;
	op[1] = NUL;
	semsg(_(e_wrong_variable_type_for_str_equal), op);
	திரும்பு FAIL;
    }

    திரும்பு OK;
}

/*
 * Compile a compound op assignment statement (+=, -=, *=, %=, etc.)
 */
    நிலையான முழு
compile_assign_compound_op(cctx_T *cctx, cac_T *cac)
{
    lhs_T	    *lhs = &cac->cac_lhs;
    type_T	    *expected;
    type_T	    *stacktype = வெற்று;

    எனில் (cac->cac_lhs.lhs_type->tt_type == VAR_TUPLE
	    && check_type_supports_compound_op(cac->cac_lhs.lhs_type,
							*cac->cac_op) == FAIL)
	திரும்பு FAIL;

    எனில் (*cac->cac_op == '.')
    {
	expected = lhs->lhs_member_type;
	stacktype = get_type_on_அடுக்கு(cctx, 0);
	எனில் (expected != &t_string
		&& need_type(stacktype, expected, 0, -1, 0, cctx,
					FALSE, FALSE) == FAIL)
	    திரும்பு FAIL;
	இல்லையெனில் எனில் (may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
	    திரும்பு FAIL;
    }
    இல்லையெனில்
    {
	expected = lhs->lhs_member_type;
	stacktype = get_type_on_அடுக்கு(cctx, 0);

	எனில் (check_type_supports_compound_op(expected, *cac->cac_op) == FAIL)
	    திரும்பு FAIL;

	எனில் (
		// If variable is float operation with number is OK.
		!(expected == &t_float && (stacktype == &t_எண்
					|| stacktype == &t_எண்_bool))
		&& need_type(stacktype, expected, TYPECHK_எண்_OK, -1, 0,
						cctx, FALSE, FALSE) == FAIL)
	    திரும்பு FAIL;
    }

    எனில் (*cac->cac_op == '.')
    {
	எனில் (generate_CONCAT(cctx, 2) == FAIL)
	    திரும்பு FAIL;
    }
    இல்லையெனில் எனில் (*cac->cac_op == '+')
    {
	எனில் (generate_add_instr(cctx,
		    operator_type(lhs->lhs_member_type, stacktype),
		    lhs->lhs_member_type, stacktype,
		    கோவை_APPEND) == FAIL)
	    திரும்பு FAIL;
    }
    இல்லையெனில் எனில் (generate_two_op(cctx, cac->cac_op) == FAIL)
	திரும்பு FAIL;

    திரும்பு OK;
}

/*
 * Generate the STORE and SETTYPE instructions for an assignment statement.
 */
    நிலையான முழு
compile_assign_generate_store(cctx_T *cctx, cac_T *cac)
{
    lhs_T	*lhs = &cac->cac_lhs;
    முழு		save_lnum;

    // Use the line number of the assignment for store instruction.
    save_lnum = cctx->ctx_lnum;
    cctx->ctx_lnum = cac->cac_start_lnum - 1;

    எனில் (lhs->lhs_has_index)
    {
	// Use the info in "lhs" to store the value at the index in the
	// list, dict or object.
	எனில் (compile_assign_unlet(cac->cac_var_start, &cac->cac_lhs,
				 TRUE, cac->cac_rhs_type, cctx) == FAIL)
	{
	    cctx->ctx_lnum = save_lnum;
	    திரும்பு FAIL;
	}
    }
    இல்லையெனில்
    {
	எனில் (cac->cac_is_decl && cac->cac_cmdidx == CMD_const &&
			(lhs->lhs_dest == dest_script
			 || lhs->lhs_dest == dest_script_v9
			 || lhs->lhs_dest == dest_global
			 || lhs->lhs_dest == dest_local))
	    // ":const var": lock the value, but not referenced variables
	    generate_LOCKCONST(cctx);

	type_T	*inferred_type = cac->cac_inferred_type;

	எனில் ((lhs->lhs_type->tt_type == VAR_DICT
		    || lhs->lhs_type->tt_type == VAR_LIST)
		&& lhs->lhs_type->tt_member != வெற்று
		&& lhs->lhs_type->tt_member != &t_any
		&& lhs->lhs_type->tt_member != &t_unknown)
	    // Set the type in the list or dict, so that it can be
	    // checked, also in legacy script.
	    generate_SETTYPE(cctx, lhs->lhs_type);
	இல்லையெனில் எனில் (lhs->lhs_type->tt_type == VAR_TUPLE
					&& lhs->lhs_type->tt_argcount != 0)
	    generate_SETTYPE(cctx, lhs->lhs_type);
	இல்லையெனில் எனில் (inferred_type != வெற்று
		&& (inferred_type->tt_type == VAR_DICT
		    || inferred_type->tt_type == VAR_LIST)
		&& inferred_type->tt_member != வெற்று
		&& inferred_type->tt_member != &t_unknown
		&& inferred_type->tt_member != &t_any)
	    // Set the type in the list or dict, so that it can be
	    // checked, also in legacy script.
	    generate_SETTYPE(cctx, inferred_type);
	இல்லையெனில் எனில் (inferred_type != வெற்று
				&& inferred_type->tt_type == VAR_TUPLE
				&& inferred_type->tt_argcount > 0)
	    generate_SETTYPE(cctx, inferred_type);

	எனில் (!cac->cac_skip_store &&
		generate_store_lhs(cctx, &cac->cac_lhs,
				   cac->cac_instr_மொத்தம்,
				   cac->cac_is_decl) == FAIL)
	{
	    cctx->ctx_lnum = save_lnum;
	    திரும்பு FAIL;
	}
    }

    cctx->ctx_lnum = save_lnum;

    திரும்பு OK;
}

/*
 * Process the variable(s) in an assignment statement
 */
    நிலையான முழு
compile_assign_process_variables(
    cctx_T	*cctx,
    cac_T	*cac,
    முழு		cmdidx,
    முழு		heredoc,
    முழு		has_cmd,
    முழு		has_argisset_prefix,
    முழு		jump_instr_idx)
{
    /*
     * Loop over variables in "[var, var] = expr".
     * For "name = expr" and "var name: type" this is done only once.
     */
    க்கு (cac->cac_var_idx = 0; cac->cac_var_idx == 0 ||
	    cac->cac_var_idx < cac->cac_var_மொத்தம்; cac->cac_var_idx++)
    {
	எனில் (cac->cac_var_start[0] == '_'
				&& !eval_isnamec(cac->cac_var_start[1]))
	{
	    // Ignore underscore in "[a, _, b] = list".
	    எனில் (cac->cac_var_மொத்தம் > 0)
	    {
		cac->cac_var_start = skipwhite(cac->cac_var_start + 2);
		தொடர்;
	    }
	    emsg(_(e_cannot_use_underscore_here));
	    திரும்பு FAIL;
	}
	vim_free(cac->cac_lhs.lhs_name);

	/*
	 * Figure out the LHS type and other properties.
	 */
	எனில் (compile_assign_lhs(cac->cac_var_start, &cac->cac_lhs, cmdidx,
		    cac->cac_is_decl, heredoc, has_cmd,
		    cac->cac_oplen, cctx) == FAIL)
	    திரும்பு FAIL;

	// Compile the RHS expression
	எனில் (heredoc)
	{
	    SOURCING_LNUM = cac->cac_start_lnum;
	    எனில் (cac->cac_lhs.lhs_has_type
		    && need_type(&t_list_string, cac->cac_lhs.lhs_type,
				0, -1, 0, cctx, FALSE, FALSE) == FAIL)
		திரும்பு FAIL;
	}
	இல்லையெனில்
	{
	    எனில் (compile_assign_rhs(cctx, cac) == FAIL)
		திரும்பு FAIL;
	    எனில் (cac->cac_var_மொத்தம் == 0)
		cac->cac_var_end = cac->cac_nextc;
	}

	// no need to parse more when skipping
	எனில் (cctx->ctx_skip == SKIP_YES)
	    நிறுத்து;

	எனில் (cac->cac_oplen > 0 && *cac->cac_op != '=')
	{
	    எனில் (compile_assign_compound_op(cctx, cac) == FAIL)
		திரும்பு FAIL;
	}

	// generate the store instructions
	எனில் (compile_assign_generate_store(cctx, cac) == FAIL)
	    திரும்பு FAIL;

	எனில் (cac->cac_var_idx + 1 < cac->cac_var_மொத்தம்)
	    cac->cac_var_start = skipwhite(cac->cac_lhs.lhs_end + 1);

	எனில் (has_argisset_prefix)
	{
	    // set instruction index in JUMP_IF_ARG_SET to here
	    isn_T *isn = ((isn_T *)cac->cac_instr->ga_data) + jump_instr_idx;
	    isn->isn_arg.jumparg.jump_where = cac->cac_instr->ga_len;
	}
    }

    திரும்பு OK;
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
    நிலையான char_u *
compile_assignment(
    char_u	*arg_start,
    exarg_T	*eap,
    cmdidx_T	cmdidx,
    cctx_T	*cctx)
{
    cac_T	cac;
    char_u	*arg = arg_start;
    char_u	*retstr = வெற்று;
    முழு		heredoc = FALSE;
    முழு		jump_instr_idx;

    compile_assign_context_init(&cac, cctx, cmdidx, arg);

    jump_instr_idx = cac.cac_instr->ga_len;

    // process object variable initialization in a new() constructor method
    முழு	has_argisset_prefix = STRNCMP(arg, "ifargisset ", 11) == 0;
    எனில் (has_argisset_prefix &&
			compile_assign_obj_new_arg(&arg, cctx) == FAIL)
	செல் theend;

    // Skip over the "varname" or "[varname, varname]" to get to any "=".
    cac.cac_nextc = skip_var_list(arg, TRUE, &cac.cac_var_மொத்தம்,
						&cac.cac_semicolon, TRUE);
    எனில் (cac.cac_nextc == வெற்று)
	திரும்பு *arg == '[' ? arg : வெற்று;

    எனில் (compile_assign_process_operator(eap, arg, &cac, &heredoc,
							&retstr) == FAIL)
	திரும்பு retstr;

    // Compute the start of the assignment
    cac.cac_var_start = compile_assign_compute_start(arg, cac.cac_var_மொத்தம்);

    // Compute the end of the assignment
    cac.cac_var_end = compile_assign_compute_end(eap, cctx, &cac, heredoc);
    எனில் (cac.cac_var_end == வெற்று)
	திரும்பு வெற்று;

    முழு has_cmd = cac.cac_var_start > eap->cmd;

    /* process the variable(s) */
    எனில் (compile_assign_process_variables(cctx, &cac, cmdidx, heredoc,
					 has_cmd, has_argisset_prefix,
					 jump_instr_idx) == FAIL)
	செல் theend;

    // For "[var, var] = expr" drop the "expr" value.
    // Also for "[var, var; _] = expr".
    எனில் (cctx->ctx_skip != SKIP_YES && cac.cac_var_மொத்தம் > 0 &&
	    (!cac.cac_semicolon || !cac.cac_did_generate_slice))
    {
	எனில் (generate_instr_drop(cctx, ISN_DROP, 1) == வெற்று)
	    செல் theend;
    }

    retstr = skipwhite(cac.cac_var_end);

theend:
    vim_free(cac.cac_lhs.lhs_name);
    திரும்பு retstr;
}

/*
 * Check for an assignment at "eap->cmd", compile it if found.
 * Return NOTDONE if there is none, FAIL for failure, OK if done.
 */
    நிலையான முழு
may_compile_assignment(exarg_T *eap, char_u **வரி, cctx_T *cctx)
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
    எனில் (p > eap->cmd && *p != NUL)
    {
	char_u *var_end;
	முழு	oplen;
	முழு	heredoc;

	எனில் (eap->cmd[0] == '@')
	    var_end = eap->cmd + 2;
	இல்லையெனில்
	    var_end = find_name_end(pskip, வெற்று, வெற்று,
					FNE_CHECK_START | FNE_INCL_BR);
	oplen = assignment_len(skipwhite(var_end), &heredoc);
	எனில் (oplen > 0)
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
	    எனில் (*eap->cmd == '&'
		    || *eap->cmd == '$'
		    || *eap->cmd == '@'
		    || ((len) > 2 && eap->cmd[1] == ':')
		    || STRNCMP(eap->cmd, "g:[", 3) == 0
		    || variable_exists(eap->cmd, len, cctx))
	    {
		*வரி = compile_assignment(eap->cmd, eap, CMD_SIZE, cctx);
		எனில் (*வரி == வெற்று || *வரி == eap->cmd)
		    திரும்பு FAIL;
		திரும்பு OK;
	    }
	}
    }

    // might be "[var, var] = expr" or "ifargisset this.member = expr"
    எனில் (*eap->cmd == '[' || STRNCMP(eap->cmd, "ifargisset ", 11) == 0)
    {
	*வரி = compile_assignment(eap->cmd, eap, CMD_SIZE, cctx);
	எனில் (*வரி == வெற்று)
	    திரும்பு FAIL;
	எனில் (*வரி != eap->cmd)
	    திரும்பு OK;
    }
    திரும்பு NOTDONE;
}

/*
 * Check if arguments of "ufunc" shadow variables in "cctx".
 * Return OK or FAIL.
 */
    நிலையான முழு
check_args_shadowing(ufunc_T *ufunc, cctx_T *cctx)
{
    முழு	    i;
    char_u  *arg;
    முழு	    r = OK;

    // Make sure arguments are not found when compiling a second time.
    ufunc->uf_args_visible = 0;

    // Check for arguments shadowing variables from the context.
    க்கு (i = 0; i < ufunc->uf_args.ga_len; ++i)
    {
	arg = ((char_u **)(ufunc->uf_args.ga_data))[i];
	எனில் (check_defined(arg, STRLEN(arg), cctx, வெற்று, TRUE) == FAIL)
	{
	    r = FAIL;
	    நிறுத்து;
	}
    }
    ufunc->uf_args_visible = ufunc->uf_args.ga_len;
    திரும்பு r;
}

#ifdef HAS_MESSAGE_WINDOW
/*
 * Get a count before a command.  Can only be a number.
 * Returns zero if there is no count.
 * Returns -1 if there is something wrong.
 */
    நிலையான long
get_cmd_மொத்தம்(char_u *வரி, exarg_T *eap)
{
    char_u *p;

    // skip over colons and white space
    க்கு (p = வரி; *p == ':' || VIM_ISWHITE(*p); ++p)
	;
    எனில் (!SAFE_isdigit(*p))
    {
	// The command or modifiers must be following.  Assume a lower case
	// character means there is a modifier.
	எனில் (p < eap->cmd && !vim_islower(*p))
	{
	    emsg(_(e_invalid_range));
	    திரும்பு -1;
	}
	திரும்பு 0;
    }
    திரும்பு atol((எழுத்து *)p);
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

    எனில் (debug_break_level > 0 || may_break_in_function(ufunc))
	திரும்பு CT_DEBUG;
#ifdef FEAT_PROFILE
    எனில் (do_profiling == PROF_YES)
    {
	எனில் (!ufunc->uf_profiling && has_profiling(FALSE, ufunc->uf_name, வெற்று,
							    &ufunc->uf_hash))
	    func_do_profile(ufunc);
	எனில் (ufunc->uf_profiling)
	    திரும்பு CT_PROFILE;
    }
#endif
    திரும்பு CT_NONE;
}

/*
 * Free the compiled instructions saved for a def function.  This is used when
 * compiling a def function and the function was compiled before.
 * The index is reused.
 */
    நிலையான வெறுமை
clear_def_function(ufunc_T *ufunc, compiletype_T compile_type)
{
    isn_T	*instr_dest = வெற்று;
    dfunc_T	*dfunc;

    dfunc = ((dfunc_T *)def_functions.ga_data) + ufunc->uf_dfunc_idx;

    தேர்வு (compile_type)
    {
	நிலை CT_PROFILE:
#ifdef FEAT_PROFILE
	    instr_dest = dfunc->df_instr_prof; நிறுத்து;
#endif
	நிலை CT_NONE:   instr_dest = dfunc->df_instr; நிறுத்து;
	நிலை CT_DEBUG:  instr_dest = dfunc->df_instr_debug; நிறுத்து;
    }

    எனில் (instr_dest != வெற்று)
	// Was compiled in this mode before: Free old instructions.
	delete_def_function_contents(dfunc, FALSE);

    ga_clear_strings(&dfunc->df_var_names);
    dfunc->df_defer_var_idx = 0;
}

/*
 * Add a function to the list of :def functions.
 * This sets "ufunc->uf_dfunc_idx" but the function isn't compiled yet.
 */
    நிலையான முழு
add_def_function(ufunc_T *ufunc)
{
    dfunc_T *dfunc;

    எனில் (def_functions.ga_len == 0)
    {
	// The first position is not used, so that a zero uf_dfunc_idx means it
	// wasn't set.
	எனில் (GA_GROW_FAILS(&def_functions, 1))
	    திரும்பு FAIL;
	++def_functions.ga_len;
    }

    // Add the function to "def_functions".
    எனில் (GA_GROW_FAILS(&def_functions, 1))
	திரும்பு FAIL;
    dfunc = ((dfunc_T *)def_functions.ga_data) + def_functions.ga_len;
    CLEAR_POINTER(dfunc);
    dfunc->df_idx = def_functions.ga_len;
    ufunc->uf_dfunc_idx = dfunc->df_idx;
    dfunc->df_ufunc = ufunc;
    dfunc->df_name = vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
    ga_init2(&dfunc->df_var_names, அளவு(char_u *), 10);
    ++dfunc->df_refcount;
    ++def_functions.ga_len;
    திரும்பு OK;
}

    நிலையான முழு
compile_dfunc_ufunc_init(
    ufunc_T		*ufunc,
    cctx_T		*outer_cctx,
    compiletype_T	compile_type,
    முழு			*new_def_function)
{
    // When using a function that was compiled before: Free old instructions.
    // The index is reused.  Otherwise add a new entry in "def_functions".
    எனில் (ufunc->uf_dfunc_idx > 0)
	clear_def_function(ufunc, compile_type);
    இல்லையெனில்
    {
	எனில் (add_def_function(ufunc) == FAIL)
	    திரும்பு FAIL;

	*new_def_function = TRUE;
    }

    எனில் ((ufunc->uf_flags & FC_CLOSURE) && outer_cctx == வெற்று)
    {
	semsg(_(e_compiling_closure_without_context_str),
						printable_func_name(ufunc));
	திரும்பு FAIL;
    }

    ufunc->uf_def_status = UF_COMPILING;

    திரும்பு OK;
}

/*
 * Initialize the compilation context for compiling a def function.
 */
    நிலையான வெறுமை
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
    ga_init2(&cctx->ctx_locals, அளவு(lvar_T), 10);
    // Each entry on the type stack consists of two type pointers.
    ga_init2(&cctx->ctx_type_அடுக்கு, அளவு(type2_T), 50);
    cctx->ctx_type_list = &ufunc->uf_type_list;
    ga_init2(&cctx->ctx_instr, அளவு(isn_T), 50);
}

/*
 * For an object constructor, generate instruction to setup "this" (the first
 * local variable) and to initialize the object variables.
 */
    நிலையான முழு
obj_constructor_prologue(ufunc_T *ufunc, cctx_T *cctx)
{
    generate_CONSTRUCT(cctx, ufunc->uf_class);

    க்கு (முழு i = 0; i < ufunc->uf_class->class_obj_member_மொத்தம்; ++i)
    {
	ocmember_T *m = &ufunc->uf_class->class_obj_members[i];

	எனில் (i < 2 && IS_ENUM(ufunc->uf_class))
	    // The first two object variables in an enum are the name
	    // and the ordinal.  These are set by the ISN_CONSTRUCT
	    // instruction.  So don't generate instructions to set
	    // these variables.
	    தொடர்;

	எனில் (m->ocm_init != வெற்று)
	{
	    char_u	*கோவை = m->ocm_init;
	    sctx_T	save_current_sctx;
	    முழு		change_sctx = FALSE;

	    // If the member variable initialization script context is
	    // different from the current script context, then change it.
	    எனில் (current_sctx.sc_sid != m->ocm_init_sctx.sc_sid)
		change_sctx = TRUE;

	    எனில் (change_sctx)
	    {
		// generate an instruction to change the script context to the
		// member variable initialization script context.
		save_current_sctx = current_sctx;
		current_sctx = m->ocm_init_sctx;
		generate_SCRIPTCTX_SET(cctx, current_sctx);
	    }

	    முழு r = compile_expr0(&கோவை, cctx);

	    எனில் (change_sctx)
	    {
		// restore the previous script context
		current_sctx = save_current_sctx;
		generate_SCRIPTCTX_SET(cctx, current_sctx);
	    }

	    எனில் (r == FAIL)
		திரும்பு FAIL;

	    எனில் (!ends_excmd2(m->ocm_init, கோவை))
	    {
		semsg(_(e_trailing_characters_str), கோவை);
		திரும்பு FAIL;
	    }

	    type_T	*type = get_type_on_அடுக்கு(cctx, 0);
	    எனில் (m->ocm_type->tt_type == VAR_ANY
		    && !(m->ocm_flags & OCMFLAG_HAS_TYPE)
		    && type->tt_type != VAR_SPECIAL)
	    {
		// If the member variable type is not yet set, then use
		// the initialization expression type.
		m->ocm_type = type;
	    }
	    இல்லையெனில்
	    {
		// The type of the member initialization expression is
		// determined at run time.  Add a runtime type check.
		where_T	where = WHERE_INIT;
		where.wt_kind = WT_MEMBER;
		where.wt_func_name = (எழுத்து *)m->ocm_name.string;
		எனில் (need_type_where(type, m->ocm_type, 0, -1,
					where, cctx, FALSE, FALSE) == FAIL)
		    திரும்பு FAIL;
	    }
	}
	இல்லையெனில்
	    தள்ளு_default_மதிப்பு(cctx, m->ocm_type->tt_type, FALSE, வெற்று);

	எனில் (((m->ocm_type->tt_type == VAR_DICT
			|| m->ocm_type->tt_type == VAR_LIST)
		    && m->ocm_type->tt_member != வெற்று
		    && m->ocm_type->tt_member != &t_any
		    && m->ocm_type->tt_member != &t_unknown)
		|| (m->ocm_type->tt_type == VAR_TUPLE
		    && m->ocm_type->tt_argcount > 0))
	    // Set the type in the list, tuple or dict, so that it can be
	    // checked, also in legacy script.
	    generate_SETTYPE(cctx, m->ocm_type);

	generate_STORE_THIS(cctx, i);
    }

    திரும்பு OK;
}

/*
 * For an object method and an constructor, generate instruction to setup
 * "this" (the first local variable).  For a constructor, generate instructions
 * to initialize the object variables.
 */
    நிலையான முழு
obj_method_prologue(ufunc_T *ufunc, cctx_T *cctx)
{
    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data) + ufunc->uf_dfunc_idx;

    எனில் (GA_GROW_FAILS(&dfunc->df_var_names, 1))
	திரும்பு FAIL;

    ((char_u **)dfunc->df_var_names.ga_data)[0] =
						vim_strsave((char_u *)"this");
    ++dfunc->df_var_names.ga_len;

    // In the constructor allocate memory for the object and initialize the
    // object members.
    எனில் (IS_CONSTRUCTOR_METHOD(ufunc))
	திரும்பு obj_constructor_prologue(ufunc, cctx);

    திரும்பு OK;
}

/*
 * Produce instructions for the default values of optional arguments.
 */
    நிலையான முழு
compile_def_function_default_args(
    ufunc_T	*ufunc,
    garray_T	*instr,
    cctx_T	*cctx)
{
    முழு	மொத்தம் = ufunc->uf_def_args.ga_len;
    முழு	first_def_arg = ufunc->uf_args.ga_len - மொத்தம்;
    முழு	i;
    முழு	off = அடுக்கு_FRAME_SIZE + (ufunc->uf_va_name != வெற்று ? 1 : 0);
    முழு	did_set_arg_type = FALSE;

    // Produce instructions for the default values of optional arguments.
    SOURCING_LNUM = 0;  // line number unknown
    க்கு (i = 0; i < மொத்தம்; ++i)
    {
	char_u *arg = ((char_u **)(ufunc->uf_def_args.ga_data))[i];
	எனில் (STRCMP(arg, "v:none") == 0)
	    // "arg = v:none" means the argument is optional without
	    // setting a value when the argument is missing.
	    தொடர்;

	type_T	*val_type;
	முழு		arg_idx = first_def_arg + i;
	where_T	where = WHERE_INIT;
	முழு		jump_instr_idx = instr->ga_len;
	isn_T	*isn;

	// Use a JUMP_IF_ARG_SET instruction to skip if the value was given.
	எனில் (generate_JUMP_IF_ARG(cctx, ISN_JUMP_IF_ARG_SET,
						i - மொத்தம் - off) == FAIL)
	    திரும்பு FAIL;

	// Make sure later arguments are not found.
	ufunc->uf_args_visible = arg_idx;

	முழு r = compile_expr0(&arg, cctx);
	எனில் (r == FAIL)
	    திரும்பு FAIL;

	// If no type specified use the type of the default value.
	// Otherwise check that the default value type matches the
	// specified type.
	val_type = get_type_on_அடுக்கு(cctx, 0);
	where.wt_index = arg_idx + 1;
	where.wt_kind = WT_ARGUMENT;
	எனில் (ufunc->uf_arg_types[arg_idx] == &t_unknown)
	{
	    did_set_arg_type = TRUE;
	    ufunc->uf_arg_types[arg_idx] = val_type;
	}
	இல்லையெனில் எனில் (need_type_where(val_type, ufunc->uf_arg_types[arg_idx],
		    0, -1, where, cctx, FALSE, FALSE) == FAIL)
	    திரும்பு FAIL;

	எனில் (generate_STORE(cctx, ISN_STORE, i - மொத்தம் - off, வெற்று) == FAIL)
	    திரும்பு FAIL;

	// set instruction index in JUMP_IF_ARG_SET to here
	isn = ((isn_T *)instr->ga_data) + jump_instr_idx;
	isn->isn_arg.jumparg.jump_where = instr->ga_len;
    }

    எனில் (did_set_arg_type)
	set_function_type(ufunc);

    திரும்பு OK;
}

/*
 * Compile def function body.  Loop over all the lines in the function and
 * generate instructions.
 */
    நிலையான முழு
compile_def_function_body(
    முழு		last_func_lnum,
    முழு		check_return_type,
    garray_T	*வரிகள்_to_free,
    எழுத்து	**errormsg,
    cctx_T	*cctx)
{
    char_u	*வரி = வெற்று;
    char_u	*p;
    முழு		did_emsg_before = did_emsg;
#ifdef FEAT_PROFILE
    முழு		prof_lnum = -1;
#endif
    முழு		debug_lnum = -1;

    க்கு (;;)
    {
	exarg_T	    ea;
	முழு	    starts_with_colon = FALSE;
	char_u	    *cmd;
	cmdmod_T    local_cmdmod;

	// Bail out on the first error to avoid a flood of errors and report
	// the right line number when inside try/catch.
	எனில் (did_emsg_before != did_emsg)
	    திரும்பு FAIL;

	எனில் (வரி != வெற்று && *வரி == '|')
	    // the line continues after a '|'
	    ++வரி;
	இல்லையெனில் எனில் (வரி != வெற்று && *skipwhite(வரி) != NUL
		&& !(*வரி == '#' && (வரி == cctx->ctx_வரி_start
						    || VIM_ISWHITE(வரி[-1]))))
	{
	    semsg(_(e_trailing_characters_str), வரி);
	    திரும்பு FAIL;
	}
	இல்லையெனில் எனில் (வரி != வெற்று && vim9_bad_comment(skipwhite(வரி)))
	    திரும்பு FAIL;
	இல்லையெனில்
	{
	    வரி = next_வரி_from_context(cctx, FALSE);
	    எனில் (cctx->ctx_lnum >= last_func_lnum)
	    {
		// beyond the last line
#ifdef FEAT_PROFILE
		எனில் (cctx->ctx_skip != SKIP_YES)
		    may_generate_prof_end(cctx, prof_lnum);
#endif
		நிறுத்து;
	    }
	    // Make a copy, splitting off nextcmd and removing trailing spaces
	    // may change it.
	    எனில் (வரி != வெற்று)
	    {
		வரி = vim_strsave(வரி);
		எனில் (ga_add_string(வரிகள்_to_free, வரி) == FAIL)
		{
		    vim_free(வரி);
		    திரும்பு FAIL;
		}
	    }
	}

	CLEAR_FIELD(ea);
	ea.cmdlinep = &வரி;
	ea.cmd = skipwhite(வரி);
	ea.skip = cctx->ctx_skip == SKIP_YES;

	எனில் (*ea.cmd == '#')
	{
	    // "#" starts a comment, but "#{" is an error
	    எனில் (vim9_bad_comment(ea.cmd))
		திரும்பு FAIL;
	    வரி = (char_u *)"";
	    தொடர்;
	}

#ifdef FEAT_PROFILE
	எனில் (cctx->ctx_compile_type == CT_PROFILE && cctx->ctx_lnum != prof_lnum
						  && cctx->ctx_skip != SKIP_YES)
	{
	    may_generate_prof_end(cctx, prof_lnum);

	    prof_lnum = cctx->ctx_lnum;
	    generate_instr(cctx, ISN_PROF_START);
	}
#endif
	எனில் (cctx->ctx_compile_type == CT_DEBUG && cctx->ctx_lnum != debug_lnum
						  && cctx->ctx_skip != SKIP_YES)
	{
	    debug_lnum = cctx->ctx_lnum;
	    generate_instr_debug(cctx);
	}
	cctx->ctx_prev_lnum = cctx->ctx_lnum + 1;

	// Some things can be recognized by the first character.
	தேர்வு (*ea.cmd)
	{
	    நிலை '}':
		{
		    // "}" ends a block scope
		    scopetype_T stype = cctx->ctx_scope == வெற்று
					  ? NO_SCOPE : cctx->ctx_scope->se_type;

		    எனில் (stype == BLOCK_SCOPE)
		    {
			compile_endblock(cctx);
			வரி = ea.cmd;
		    }
		    இல்லையெனில்
		    {
			emsg(_(e_using_rcurly_outside_if_block_scope));
			திரும்பு FAIL;
		    }
		    எனில் (வரி != வெற்று)
			வரி = skipwhite(ea.cmd + 1);
		    தொடர்;
		}

	    நிலை '{':
		// "{" starts a block scope
		// "{'a': 1}->func() is something else
		எனில் (ends_excmd(*skipwhite(ea.cmd + 1)))
		{
		    வரி = compile_block(ea.cmd, cctx);
		    தொடர்;
		}
		நிறுத்து;
	}

	/*
	 * COMMAND MODIFIERS
	 */
	cctx->ctx_has_cmdmod = FALSE;
	எனில் (parse_command_modifiers(&ea, errormsg, &local_cmdmod, FALSE)
								       == FAIL)
	    திரும்பு FAIL;
	generate_cmdmods(cctx, &local_cmdmod);
	undo_cmdmod(&local_cmdmod);

	// Check if there was a colon after the last command modifier or before
	// the current position.
	க்கு (p = ea.cmd; p >= வரி; --p)
	{
	    எனில் (*p == ':')
		starts_with_colon = TRUE;
	    எனில் (p < ea.cmd && !VIM_ISWHITE(*p))
		நிறுத்து;
	}

	// Skip ":call" to get to the function name, unless using :legacy
	p = ea.cmd;
	எனில் (!(local_cmdmod.cmod_flags & CMOD_LEGACY))
	{
	    எனில் (checkforcmd(&ea.cmd, "call", 3))
	    {
		எனில் (*ea.cmd == '(')
		    // not for "call()"
		    ea.cmd = p;
		இல்லையெனில்
		    ea.cmd = skipwhite(ea.cmd);
	    }

	    எனில் (!starts_with_colon)
	    {
		முழு	    assign;

		// Check for assignment after command modifiers.
		assign = may_compile_assignment(&ea, &வரி, cctx);
		எனில் (assign == OK)
		    செல் nextline;
		எனில் (assign == FAIL)
		    திரும்பு FAIL;
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
	எனில் ((*cmd != '$' || starts_with_colon)
		&& (starts_with_colon
		    || !(*cmd == '\''
			|| (cmd[0] == '0' && cmd[1] == 'z')
			|| (cmd[0] != NUL && cmd[0] == cmd[1]
					    && (*cmd == '+' || *cmd == '-'))
			|| எண்_method(cmd))))
	{
	    ea.cmd = skip_range(ea.cmd, TRUE, வெற்று);
	    எனில் (ea.cmd > cmd)
	    {
		எனில் (!starts_with_colon
				   && !(local_cmdmod.cmod_flags & CMOD_LEGACY))
		{
		    semsg(_(e_colon_required_before_range_str), cmd);
		    திரும்பு FAIL;
		}
		ea.addr_மொத்தம் = 1;
		எனில் (ends_excmd2(வரி, ea.cmd))
		{
		    // A range without a command: jump to the line.
		    generate_EXEC(cctx, ISN_EXECRANGE,
					      vim_strnsave(cmd, ea.cmd - cmd));
		    வரி = ea.cmd;
		    செல் nextline;
		}
	    }
	}
	p = find_ex_command(&ea, வெற்று,
		starts_with_colon || (local_cmdmod.cmod_flags & CMOD_LEGACY)
						  ? வெற்று : item_exists, cctx);

	எனில் (p == வெற்று)
	{
	    எனில் (cctx->ctx_skip != SKIP_YES)
		semsg(_(e_ambiguous_use_of_user_defined_command_str), ea.cmd);
	    திரும்பு FAIL;
	}

	// When using ":legacy cmd" always use compile_exec().
	எனில் (local_cmdmod.cmod_flags & CMOD_LEGACY)
	{
	    char_u *start = ea.cmd;

	    தேர்வு (ea.cmdidx)
	    {
		நிலை CMD_if:
		நிலை CMD_elseif:
		நிலை CMD_else:
		நிலை CMD_endif:
		நிலை CMD_for:
		நிலை CMD_endfor:
		நிலை CMD_continue:
		நிலை CMD_break:
		நிலை CMD_while:
		நிலை CMD_endwhile:
		நிலை CMD_try:
		நிலை CMD_catch:
		நிலை CMD_finally:
		நிலை CMD_endtry:
			semsg(_(e_cannot_use_legacy_with_command_str), ea.cmd);
			திரும்பு FAIL;
		இயல்பு: நிறுத்து;
	    }

	    // ":legacy return expr" needs to be handled differently.
	    எனில் (checkforcmd(&start, "return", 4))
		ea.cmdidx = CMD_return;
	    இல்லையெனில்
		ea.cmdidx = CMD_legacy;
	}

	எனில் (p == ea.cmd && ea.cmdidx != CMD_SIZE)
	{
	    // "eval" is used for "val->func()" and "var" for "var = val", then
	    // "p" is equal to "ea.cmd" for a valid command.
	    எனில் (ea.cmdidx == CMD_eval || ea.cmdidx == CMD_var)
		;
	    இல்லையெனில் எனில் (cctx->ctx_skip == SKIP_YES)
	    {
		வரி += STRLEN(வரி);
		செல் nextline;
	    }
	    இல்லையெனில்
	    {
		semsg(_(e_command_not_recognized_str), ea.cmd);
		திரும்பு FAIL;
	    }
	}

	எனில் ((cctx->ctx_had_return || cctx->ctx_had_throw)
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
	    திரும்பு FAIL;
	}

	// When processing the end of an if-else or try-catch block, don't
	// clear the "ctx_had_throw" flag.  If such a block ends in a "throw"
	// statement, then it is considered to end in a "return" statement.
	// The "ctx_had_throw" is cleared immediately after processing the
	// block ending statement.
	// Otherwise, clear the "had_throw" flag.
	எனில் (ea.cmdidx != CMD_else && ea.cmdidx != CMD_elseif
						&& ea.cmdidx != CMD_endif
						&& ea.cmdidx != CMD_catch
						&& ea.cmdidx != CMD_finally
						&& ea.cmdidx != CMD_endtry)
	    cctx->ctx_had_throw = FALSE;

	p = skipwhite(p);
	எனில் (ea.cmdidx != CMD_SIZE
			    && ea.cmdidx != CMD_write && ea.cmdidx != CMD_read)
	{
	    எனில் (ea.cmdidx >= 0)
		ea.argt = excmd_get_argt(ea.cmdidx);
	    எனில் ((ea.argt & EX_BANG) && *p == '!')
	    {
		ea.forceit = TRUE;
		p = skipwhite(p + 1);
	    }
	    எனில் ((ea.argt & EX_RANGE) == 0 && ea.addr_மொத்தம் > 0)
	    {
		emsg(_(e_no_range_allowed));
		திரும்பு FAIL;
	    }
	}

	தேர்வு (ea.cmdidx)
	{
	    நிலை CMD_def:
	    நிலை CMD_function:
		    ea.arg = p;
		    வரி = compile_nested_function(&ea, cctx, வரிகள்_to_free);
		    நிறுத்து;

	    நிலை CMD_return:
		    வரி = compile_return(p, check_return_type,
				 local_cmdmod.cmod_flags & CMOD_LEGACY, cctx);
		    cctx->ctx_had_return = TRUE;
		    நிறுத்து;

	    நிலை CMD_let:
		    emsg(_(e_cannot_use_let_in_vim9_script));
		    நிறுத்து;
	    நிலை CMD_var:
	    நிலை CMD_final:
	    நிலை CMD_const:
	    நிலை CMD_increment:
	    நிலை CMD_decrement:
		    வரி = compile_assignment(p, &ea, ea.cmdidx, cctx);
		    எனில் (வரி == p)
		    {
			emsg(_(e_invalid_assignment));
			வரி = வெற்று;
		    }
		    நிறுத்து;

	    நிலை CMD_unlet:
	    நிலை CMD_unlockvar:
	    நிலை CMD_lockvar:
		    வரி = compile_unletlock(p, &ea, cctx);
		    நிறுத்து;

	    நிலை CMD_import:
		    emsg(_(e_import_can_only_be_used_in_script));
		    வரி = வெற்று;
		    நிறுத்து;

	    நிலை CMD_if:
		    வரி = compile_if(p, cctx);
		    நிறுத்து;
	    நிலை CMD_elseif:
		    வரி = compile_elseif(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    cctx->ctx_had_throw = FALSE;
		    நிறுத்து;
	    நிலை CMD_else:
		    வரி = compile_else(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    cctx->ctx_had_throw = FALSE;
		    நிறுத்து;
	    நிலை CMD_endif:
		    வரி = compile_endif(p, cctx);
		    cctx->ctx_had_throw = FALSE;
		    நிறுத்து;

	    நிலை CMD_while:
		    வரி = compile_while(p, cctx);
		    நிறுத்து;
	    நிலை CMD_endwhile:
		    வரி = compile_endwhile(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    நிறுத்து;

	    நிலை CMD_for:
		    வரி = compile_for(p, cctx);
		    நிறுத்து;
	    நிலை CMD_endfor:
		    வரி = compile_endfor(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    நிறுத்து;
	    நிலை CMD_continue:
		    வரி = compile_continue(p, cctx);
		    // "continue" unconditionally jumps back to the loop,
		    // like a "return".
		    cctx->ctx_had_return = TRUE;
		    நிறுத்து;
	    நிலை CMD_break:
		    வரி = compile_break(p, cctx);
		    // "break" unconditionally exits the loop, like a "return".
		    cctx->ctx_had_return = TRUE;
		    நிறுத்து;

	    நிலை CMD_try:
		    வரி = compile_try(p, cctx);
		    நிறுத்து;
	    நிலை CMD_catch:
		    வரி = compile_catch(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    cctx->ctx_had_throw = FALSE;
		    நிறுத்து;
	    நிலை CMD_finally:
		    வரி = compile_finally(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    cctx->ctx_had_throw = FALSE;
		    நிறுத்து;
	    நிலை CMD_endtry:
		    வரி = compile_endtry(p, cctx);
		    cctx->ctx_had_throw = FALSE;
		    நிறுத்து;
	    நிலை CMD_throw:
		    வரி = compile_throw(p, cctx);
		    cctx->ctx_had_throw = TRUE;
		    நிறுத்து;

	    நிலை CMD_eval:
		    வரி = compile_eval(p, cctx);
		    நிறுத்து;

	    நிலை CMD_defer:
		    வரி = compile_defer(p, cctx);
		    நிறுத்து;

#ifdef HAS_MESSAGE_WINDOW
	    நிலை CMD_echowindow:
		    {
			long cmd_மொத்தம் = get_cmd_மொத்தம்(வரி, &ea);
			எனில் (cmd_மொத்தம் < 0)
			    வரி = வெற்று;
			இல்லையெனில்
			    வரி = compile_mult_கோவை(p, ea.cmdidx,
							     cmd_மொத்தம், cctx);
		    }
		    நிறுத்து;
#endif
	    நிலை CMD_echo:
	    நிலை CMD_echon:
	    நிலை CMD_echoconsole:
	    நிலை CMD_echoerr:
	    நிலை CMD_echomsg:
	    நிலை CMD_execute:
		    வரி = compile_mult_கோவை(p, ea.cmdidx, 0, cctx);
		    நிறுத்து;

	    நிலை CMD_put:
		    ea.cmd = cmd;
		    வரி = compile_put(p, &ea, cctx, FALSE);
		    நிறுத்து;

	    நிலை CMD_iput:
		    ea.cmd = cmd;
		    வரி = compile_put(p, &ea, cctx, TRUE);
		    நிறுத்து;

	    நிலை CMD_substitute:
		    எனில் (check_global_and_subst(ea.cmd, p) == FAIL)
			திரும்பு FAIL;
		    எனில் (cctx->ctx_skip == SKIP_YES)
			வரி = (char_u *)"";
		    இல்லையெனில்
		    {
			ea.arg = p;
			வரி = compile_substitute(வரி, &ea, cctx);
		    }
		    நிறுத்து;

	    நிலை CMD_redir:
		    ea.arg = p;
		    வரி = compile_redir(வரி, &ea, cctx);
		    நிறுத்து;

	    நிலை CMD_cexpr:
	    நிலை CMD_lexpr:
	    நிலை CMD_caddexpr:
	    நிலை CMD_laddexpr:
	    நிலை CMD_cgetexpr:
	    நிலை CMD_lgetexpr:
#ifdef FEAT_QUICKFIX
		    ea.arg = p;
		    வரி = compile_cexpr(வரி, &ea, cctx);
#இல்லையெனில்
		    ex_ni(&ea);
		    வரி = வெற்று;
#endif
		    நிறுத்து;

	    நிலை CMD_append:
	    நிலை CMD_change:
	    நிலை CMD_செருகு:
	    நிலை CMD_k:
	    நிலை CMD_open:
	    நிலை CMD_t:
	    நிலை CMD_xit:
		    not_in_vim9(&ea, false);
		    திரும்பு FAIL;

	    நிலை CMD_SIZE:
		    எனில் (cctx->ctx_skip != SKIP_YES)
		    {
			semsg(_(e_invalid_command_str), ea.cmd);
			திரும்பு FAIL;
		    }
		    // We don't check for a next command here.
		    வரி = (char_u *)"";
		    நிறுத்து;

	    நிலை CMD_lua:
	    நிலை CMD_mzscheme:
	    நிலை CMD_perl:
	    நிலை CMD_py3:
	    நிலை CMD_python3:
	    நிலை CMD_python:
	    நிலை CMD_pythonx:
	    நிலை CMD_ruby:
	    நிலை CMD_tcl:
		    ea.arg = p;
		    எனில் (vim_strchr(வரி, '\n') == வெற்று)
			வரி = compile_exec(வரி, &ea, cctx);
		    இல்லையெனில்
			// heredoc lines have been concatenated with NL
			// characters in get_function_body()
			வரி = compile_script(வரி, cctx);
		    நிறுத்து;

	    நிலை CMD_vim9script:
		    எனில் (cctx->ctx_skip != SKIP_YES)
		    {
			emsg(_(e_vim9script_can_only_be_used_in_script));
			திரும்பு FAIL;
		    }
		    வரி = (char_u *)"";
		    நிறுத்து;

	    நிலை CMD_class:
		    emsg(_(e_class_can_only_be_used_in_script));
		    திரும்பு FAIL;

	    நிலை CMD_enum:
		    emsg(_(e_enum_can_only_be_used_in_script));
		    திரும்பு FAIL;

	    நிலை CMD_interface:
		    emsg(_(e_interface_can_only_be_used_in_script));
		    திரும்பு FAIL;

	    நிலை CMD_type:
		    emsg(_(e_type_can_only_be_used_in_script));
		    திரும்பு FAIL;

	    நிலை CMD_global:
		    எனில் (check_global_and_subst(ea.cmd, p) == FAIL)
			திரும்பு FAIL;
		    // FALLTHROUGH
	    இயல்பு:
		    // Not recognized, execute with do_cmdline_cmd().
		    ea.arg = p;
		    வரி = compile_exec(வரி, &ea, cctx);
		    நிறுத்து;
	}
nextline:
	எனில் (வரி == வெற்று)
	    திரும்பு FAIL;
	வரி = skipwhite(வரி);

	// Undo any command modifiers.
	generate_undo_cmdmods(cctx);

	எனில் (cctx->ctx_type_அடுக்கு.ga_len < 0)
	{
	    iemsg("Type stack underflow");
	    திரும்பு FAIL;
	}
    } // END of the loop over all the function body lines.

    திரும்பு OK;
}

/*
 * Returns TRUE if the end of a scope (if, while, for, block) is missing.
 * Called after compiling a def function body.
 */
    நிலையான முழு
compile_dfunc_scope_end_missing(cctx_T *cctx)
{
    எனில் (cctx->ctx_scope == வெற்று)
	திரும்பு FALSE;

    தேர்வு (cctx->ctx_scope->se_type)
    {
	நிலை IF_SCOPE:
	    emsg(_(e_missing_endif));
	    நிறுத்து;
	நிலை WHILE_SCOPE:
	    emsg(_(e_missing_endwhile));
	    நிறுத்து;
	நிலை FOR_SCOPE:
	    emsg(_(e_missing_endfor));
	    நிறுத்து;
	நிலை TRY_SCOPE:
	    emsg(_(e_missing_endtry));
	    நிறுத்து;
	நிலை BLOCK_SCOPE:
	    // end block scope from :try (maybe)
	    compile_endblock(cctx);
	    எனில் (cctx->ctx_scope != வெற்று
		    && cctx->ctx_scope->se_type == TRY_SCOPE)
		emsg(_(e_missing_endtry));
	    இல்லையெனில்
		emsg(_(e_missing_rcurly));
	    நிறுத்து;
	இயல்பு:
	    emsg(_(e_missing_rcurly));
    }
    திரும்பு TRUE;
}

/*
 * When compiling a def function, if it doesn't have an explicit return
 * statement, then generate a default return instruction.  For an object
 * constructor, return the object.
 */
    நிலையான முழு
compile_dfunc_generate_default_return(ufunc_T *ufunc, cctx_T *cctx)
{
    // TODO: if a function ends in "throw" but there was a return elsewhere we
    // should not assume the return type is "void".
    எனில் (cctx->ctx_had_return || cctx->ctx_had_throw)
	திரும்பு OK;

    எனில் (ufunc->uf_ret_type->tt_type == VAR_UNKNOWN)
	ufunc->uf_ret_type = &t_void;
    இல்லையெனில் எனில் (ufunc->uf_ret_type->tt_type != VAR_VOID
					&& !IS_CONSTRUCTOR_METHOD(ufunc))
    {
	emsg(_(e_missing_return_statement));
	திரும்பு FAIL;
    }

    // Return void if there is no return at the end.
    // For a constructor return the object.
    எனில் (IS_CONSTRUCTOR_METHOD(ufunc))
    {
	generate_instr(cctx, ISN_RETURN_OBJECT);
	ufunc->uf_ret_type = &ufunc->uf_class->class_object_type;
    }
    இல்லையெனில்
	generate_instr(cctx, ISN_RETURN_VOID);

    திரும்பு OK;
}

/*
 * Perform the chores after successfully compiling a def function.
 */
    நிலையான வெறுமை
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
    எனில் (cctx->ctx_compile_type == CT_PROFILE)
    {
	dfunc->df_instr_prof = instr->ga_data;
	dfunc->df_instr_prof_மொத்தம் = instr->ga_len;
    }
    இல்லையெனில்
#endif
	எனில் (cctx->ctx_compile_type == CT_DEBUG)
	{
	    dfunc->df_instr_debug = instr->ga_data;
	    dfunc->df_instr_debug_மொத்தம் = instr->ga_len;
	}
	இல்லையெனில்
	{
	    dfunc->df_instr = instr->ga_data;
	    dfunc->df_instr_மொத்தம் = instr->ga_len;
	}
    dfunc->df_varcount = dfunc->df_var_names.ga_len;
    dfunc->df_has_closure = cctx->ctx_has_closure;

    எனில் (cctx->ctx_outer_used)
    {
	ufunc->uf_flags |= FC_CLOSURE;
	எனில் (outer_cctx != வெற்று)
	    ++outer_cctx->ctx_closure_மொத்தம்;
    }

    ufunc->uf_def_status = UF_COMPILED;
}

/*
 * Perform the cleanup when a def function compilation fails.
 */
    நிலையான வெறுமை
compile_dfunc_ufunc_cleanup(
    ufunc_T	*ufunc,
    garray_T	*instr,
    முழு		new_def_function,
    எழுத்து	*errormsg,
    முழு		did_emsg_before,
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
    எனில் (!dfunc->df_deleted && new_def_function
			&& ufunc->uf_dfunc_idx == def_functions.ga_len - 1)
    {
	--def_functions.ga_len;
	ufunc->uf_dfunc_idx = 0;
    }
    ufunc->uf_def_status = UF_COMPILE_ERROR;

    போது (cctx->ctx_scope != வெற்று)
	drop_scope(cctx);

    எனில் (errormsg != வெற்று)
	emsg(errormsg);
    இல்லையெனில் எனில் (did_emsg == did_emsg_before)
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
    முழு
compile_def_function(
	ufunc_T		*ufunc,
	முழு		check_return_type,
	compiletype_T   compile_type,
	cctx_T		*outer_cctx)
{
    garray_T	வரிகள்_to_free;
    எழுத்து	*errormsg = வெற்று;	// error message
    cctx_T	cctx;
    garray_T	*instr;
    முழு		did_emsg_before = did_emsg;
    முழு		did_emsg_silent_before = did_emsg_silent;
    முழு		ret = FAIL;
    sctx_T	save_current_sctx = current_sctx;
    முழு		save_estack_compiling = estack_compiling;
    முழு		save_cmod_flags = cmdmod.cmod_flags;
    முழு		do_estack_தள்ளு;
    முழு		new_def_function = FALSE;

    // allocated lines are freed at the end
    ga_init2(&வரிகள்_to_free, அளவு(char_u *), 50);

    // Initialize the ufunc and the compilation context
    எனில் (compile_dfunc_ufunc_init(ufunc, outer_cctx, compile_type,
						&new_def_function) == FAIL)
	திரும்பு FAIL;

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
    do_estack_தள்ளு = !estack_உச்சி_is_ufunc(ufunc, 1);
    எனில் (do_estack_தள்ளு)
	estack_தள்ளு_ufunc(ufunc, 1);
    estack_compiling = TRUE;

    // Make sure arguments don't shadow variables in the context
    எனில் (check_args_shadowing(ufunc, &cctx) == FAIL)
	செல் erret;

    // For an object method and a constructor generate instructions to
    // initialize "this" and the object variables.
    எனில் (ufunc->uf_flags & (FC_OBJECT|FC_NEW))
	எனில் (obj_method_prologue(ufunc, &cctx) == FAIL)
	    செல் erret;

    எனில் (ufunc->uf_def_args.ga_len > 0)
	எனில் (compile_def_function_default_args(ufunc, instr, &cctx) == FAIL)
	    செல் erret;
    ufunc->uf_args_visible = ufunc->uf_args.ga_len;

    // Compiling an abstract method or a function in an interface is done to
    // get the function type.  No code is actually compiled.
    எனில் (ufunc->uf_class != வெற்று && (IS_INTERFACE(ufunc->uf_class)
						|| IS_ABSTRACT_METHOD(ufunc)))
    {
	ufunc->uf_def_status = UF_NOT_COMPILED;
	ret = OK;
	செல் erret;
    }

    // compile the function body
    எனில் (compile_def_function_body(ufunc->uf_வரிகள்.ga_len, check_return_type,
				&வரிகள்_to_free, &errormsg, &cctx) == FAIL)
	செல் erret;

    எனில் (compile_dfunc_scope_end_missing(&cctx))
	செல் erret;

    எனில் (compile_dfunc_generate_default_return(ufunc, &cctx) == FAIL)
	செல் erret;

    // When compiled with ":silent!" and there was an error don't consider the
    // function compiled.
    எனில் (emsg_silent == 0 || did_emsg_silent == did_emsg_silent_before)
	compile_dfunc_epilogue(outer_cctx, ufunc, instr, &cctx);

    ret = OK;

erret:
    எனில் (ufunc->uf_def_status == UF_COMPILING)
    {
	// compilation failed. do cleanup.
	compile_dfunc_ufunc_cleanup(ufunc, instr, new_def_function,
				    errormsg, did_emsg_before, &cctx);
    }

    எனில் (cctx.ctx_redir_lhs.lhs_name != வெற்று)
    {
	எனில் (ret == OK)
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
    எனில் (do_estack_தள்ளு)
	estack_எடு();

    ga_clear_strings(&வரிகள்_to_free);
    free_locals(&cctx);
    ga_clear(&cctx.ctx_type_அடுக்கு);
    திரும்பு ret;
}

    வெறுமை
set_function_type(ufunc_T *ufunc)
{
    முழு varargs = ufunc->uf_va_name != வெற்று;
    முழு argcount = ufunc->uf_args.ga_len;

    // Create a type for the function, with the return type and any
    // argument types.
    // A vararg is included in uf_args.ga_len but not in uf_arg_types.
    // The type is included in "tt_args".
    எனில் (argcount > 0 || varargs)
    {
	எனில் (ufunc->uf_type_list.ga_itemsize == 0)
	    ga_init2(&ufunc->uf_type_list, அளவு(type_T *), 10);
	ufunc->uf_func_type = alloc_func_type(ufunc->uf_ret_type,
					   argcount, &ufunc->uf_type_list);
	// Add argument types to the function type.
	எனில் (func_type_add_arg_types(ufunc->uf_func_type,
				    argcount + varargs,
				    &ufunc->uf_type_list) == FAIL)
	    திரும்பு;
	ufunc->uf_func_type->tt_argcount = argcount + varargs;
	ufunc->uf_func_type->tt_min_argcount =
				      argcount - ufunc->uf_def_args.ga_len;
	எனில் (ufunc->uf_arg_types == வெற்று)
	{
	    முழு i;

	    // lambda does not have argument types.
	    க்கு (i = 0; i < argcount; ++i)
		ufunc->uf_func_type->tt_args[i] = &t_any;
	}
	இல்லையெனில்
	    mch_memmove(ufunc->uf_func_type->tt_args,
			 ufunc->uf_arg_types, அளவு(type_T *) * argcount);
	எனில் (varargs)
	{
	    ufunc->uf_func_type->tt_args[argcount] =
		   ufunc->uf_va_type == வெற்று ? &t_list_any : ufunc->uf_va_type;
	    ufunc->uf_func_type->tt_flags = TTFLAG_VARARGS;
	}
    }
    இல்லையெனில்
	// No arguments, can use a predefined type.
	ufunc->uf_func_type = get_func_type(ufunc->uf_ret_type,
					   argcount, &ufunc->uf_type_list);
}

/*
 * Free all instructions for "dfunc" except df_name.
 */
    நிலையான வெறுமை
delete_def_function_contents(dfunc_T *dfunc, முழு mark_deleted)
{
    முழு idx;

    // In same cases the instructions may refer to a class in which the
    // function is defined and unreferencing the class may call back here
    // recursively.  Set the df_delete_busy to avoid problems.
    எனில் (dfunc->df_delete_busy)
	திரும்பு;
    dfunc->df_delete_busy = TRUE;

    ga_clear(&dfunc->df_def_args_isn);
    ga_clear_strings(&dfunc->df_var_names);

    எனில் (dfunc->df_instr != வெற்று)
    {
	க்கு (idx = 0; idx < dfunc->df_instr_மொத்தம்; ++idx)
	    delete_instr(dfunc->df_instr + idx);
	VIM_CLEAR(dfunc->df_instr);
    }
    எனில் (dfunc->df_instr_debug != வெற்று)
    {
	க்கு (idx = 0; idx < dfunc->df_instr_debug_மொத்தம்; ++idx)
	    delete_instr(dfunc->df_instr_debug + idx);
	VIM_CLEAR(dfunc->df_instr_debug);
    }
#ifdef FEAT_PROFILE
    எனில் (dfunc->df_instr_prof != வெற்று)
    {
	க்கு (idx = 0; idx < dfunc->df_instr_prof_மொத்தம்; ++idx)
	    delete_instr(dfunc->df_instr_prof + idx);
	VIM_CLEAR(dfunc->df_instr_prof);
    }
#endif

    எனில் (mark_deleted)
	dfunc->df_deleted = TRUE;
    எனில் (dfunc->df_ufunc != வெற்று)
	dfunc->df_ufunc->uf_def_status = UF_NOT_COMPILED;

    dfunc->df_delete_busy = FALSE;
}

/*
 * When a user function is deleted, clear the contents of any associated def
 * function, unless another user function still uses it.
 * The position in def_functions can be re-used.
 */
    வெறுமை
unlink_def_function(ufunc_T *ufunc)
{
    எனில் (ufunc->uf_dfunc_idx <= 0 || def_functions.ga_data == வெற்று)
	திரும்பு;

    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data)
	+ ufunc->uf_dfunc_idx;

    எனில் (--dfunc->df_refcount <= 0)
	delete_def_function_contents(dfunc, TRUE);
    ufunc->uf_def_status = UF_NOT_COMPILED;
    ufunc->uf_dfunc_idx = 0;
    எனில் (dfunc->df_ufunc == ufunc)
	dfunc->df_ufunc = வெற்று;
}

/*
 * Used when a user function refers to an existing dfunc.
 */
    வெறுமை
link_def_function(ufunc_T *ufunc)
{
    எனில் (ufunc->uf_dfunc_idx <= 0)
	திரும்பு;

    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data)
	+ ufunc->uf_dfunc_idx;

    ++dfunc->df_refcount;
}

#எனில் defined(EXITFREE)
/*
 * Free all functions defined with ":def".
 */
    வெறுமை
free_def_functions(வெறுமை)
{
    முழு idx;

    க்கு (idx = 0; idx < def_functions.ga_len; ++idx)
    {
	dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data) + idx;

	delete_def_function_contents(dfunc, TRUE);
	vim_free(dfunc->df_name);
    }

    ga_clear(&def_functions);
}
#endif


#endif // FEAT_EVAL
