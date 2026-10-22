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
#شامل "vim.h"

#اگر defined(FEAT_EVAL)

// Functions defined with :def are stored in this growarray.
// They are never removed, so that they can be found by index.
// Deleted functions have the df_deleted flag set.
garray_T def_functions = {0, 0, اندازه(dfunc_T), 50, پوچ};

ایستا تهی delete_def_function_contents(dfunc_T *dfunc, صحیح mark_deleted);

/*
 * Lookup variable "name" in the local scope and return it in "lvar".
 * "lvar->lv_from_outer" is incremented accordingly.
 * If "lvar" is NULL only check if the variable can be found.
 * Return FAIL if not found.
 */
    صحیح
lookup_local(char_u *name, size_t len, lvar_T *lvar, cctx_T *cctx)
{
    صحیح	    idx;
    lvar_T  *lvp;

    اگر (len == 0)
	بازگشت FAIL;

    اگر (((len == 4 && STRNCMP(name, "this", 4) == 0)
		|| (len == 5 && STRNCMP(name, "super", 5) == 0))
	    && cctx->ctx_ufunc != پوچ
	    && (cctx->ctx_ufunc->uf_flags & (FC_OBJECT|FC_NEW)))
    {
	صحیح is_super = *name == 's';
	اگر (is_super)
	{
	    اگر (name[5] != '.')
	    {
		emsg(_(e_super_must_be_followed_by_dot));
		بازگشت FAIL;
	    }
	    اگر (cctx->ctx_ufunc->uf_class != پوچ
		    && cctx->ctx_ufunc->uf_class->class_extends == پوچ)
	    {
		emsg(_(e_using_super_not_in_child_class));
		بازگشت FAIL;
	    }
	}
	اگر (lvar != پوچ)
	{
	    CLEAR_POINTER(lvar);
	    lvar->lv_loop_depth = -1;
	    lvar->lv_name = (char_u *)(is_super ? "super" : "this");
	    اگر (cctx->ctx_ufunc->uf_class != پوچ)
	    {
		lvar->lv_type = &cctx->ctx_ufunc->uf_class->class_object_type;
		اگر (is_super)
		{
		    type_T *type = get_type_ptr(cctx->ctx_type_list);

		    اگر (type != پوچ)
		    {
			*type = *lvar->lv_type;
			lvar->lv_type = type;
			type->tt_flags |= TTFLAG_SUPER;
		    }
		}
	    }
	}
	بازگشت OK;
    }

    // Find local in current function scope.
    برای (idx = 0; idx < cctx->ctx_locals.ga_len; ++idx)
    {
	lvp = ((lvar_T *)cctx->ctx_locals.ga_data) + idx;
	اگر (lvp->lv_name != پوچ
		&& STRNCMP(name, lvp->lv_name, len) == 0
					       && STRLEN(lvp->lv_name) == len)
	{
	    اگر (lvar != پوچ)
	    {
		*lvar = *lvp;
		lvar->lv_from_outer = 0;
		// If the variable was declared inside a loop set
		// lvar->lv_loop_idx and lvar->lv_loop_depth.
		get_loop_var_idx(cctx, idx, lvar);
	    }
	    بازگشت OK;
	}
    }

    // Find local in outer function scope.
    اگر (cctx->ctx_outer != پوچ)
    {
	اگر (lookup_local(name, len, lvar, cctx->ctx_outer) == OK)
	{
	    اگر (lvar != پوچ)
	    {
		cctx->ctx_outer_used = TRUE;
		++lvar->lv_from_outer;
	    }
	    بازگشت OK;
	}
    }

    بازگشت FAIL;
}

/*
 * Lookup an argument in the current function and an enclosing function.
 * Returns the argument index in "idxp"
 * Returns the argument type in "type"
 * Sets "gen_load_outer" to TRUE if found in outer scope.
 * Returns OK when found, FAIL otherwise.
 */
    صحیح
arg_exists(
	char_u	*name,
	size_t	len,
	صحیح	*idxp,
	type_T	**type,
	صحیح	*gen_load_outer,
	cctx_T	*cctx)
{
    صحیح	    idx;
    char_u  *va_name;

    اگر (len == 0)
	بازگشت FAIL;
    برای (idx = 0; idx < cctx->ctx_ufunc->uf_args_visible; ++idx)
    {
	char_u *arg = FUNCARG(cctx->ctx_ufunc, idx);

	اگر (STRNCMP(name, arg, len) == 0 && arg[len] == NUL)
	{
	    اگر (idxp != پوچ)
	    {
		// Arguments are located above the frame pointer.  One further
		// if there is a vararg argument
		*idxp = idx - (cctx->ctx_ufunc->uf_args.ga_len
							    + پشته_FRAME_SIZE)
			      + (cctx->ctx_ufunc->uf_va_name != پوچ ? -1 : 0);

		اگر (cctx->ctx_ufunc->uf_arg_types != پوچ)
		    *type = cctx->ctx_ufunc->uf_arg_types[idx];
		وگرنه
		    *type = &t_any;
	    }
	    بازگشت OK;
	}
    }

    va_name = cctx->ctx_ufunc->uf_va_name;
    اگر (va_name != پوچ
		    && STRNCMP(name, va_name, len) == 0 && va_name[len] == NUL)
    {
	اگر (idxp != پوچ)
	{
	    // varargs is always the last argument
	    *idxp = -پشته_FRAME_SIZE - 1;
	    *type = cctx->ctx_ufunc->uf_va_type;
	}
	بازگشت OK;
    }

    اگر (cctx->ctx_outer != پوچ)
    {
	// Lookup the name for an argument of the outer function.
	اگر (arg_exists(name, len, idxp, type, gen_load_outer, cctx->ctx_outer)
									 == OK)
	{
	    اگر (gen_load_outer != پوچ)
	    {
		++*gen_load_outer;
		cctx->ctx_outer_used = TRUE;
	    }
	    بازگشت OK;
	}
    }

    بازگشت FAIL;
}

/*
 * Lookup a script-local variable in the current script, possibly defined in a
 * block that contains the function "cctx->ctx_ufunc".
 * "cctx" is NULL at the script level, "cstack" is NULL in a function.
 * If "len" is <= 0 "name" must be NUL terminated.
 * Return NULL when not found.
 */
    ایستا sallvar_T *
find_script_var(char_u *name, size_t len, cctx_T *cctx, cstack_T *cstack)
{
    scriptitem_T    *si = SCRIPT_ITEM(current_sctx.sc_sid);
    hashitem_T	    *hi;
    صحیح		    cc;
    sallvar_T	    *sav;
    ufunc_T	    *ufunc;

    // Find the list of all script variables with the right name.
    اگر (len > 0)
    {
	cc = name[len];
	name[len] = NUL;
    }
    hi = hash_find(&si->sn_all_vars.dv_hashtab, name);
    اگر (len > 0)
	name[len] = cc;
    اگر (HASHITEM_EMPTY(hi))
	بازگشت پوچ;

    sav = HI2SAV(hi);
    اگر (sav->sav_block_id == 0)
	// variable defined in the top script scope is always visible
	بازگشت sav;

    اگر (cctx == پوچ)
    {
	اگر (cstack == پوچ)
	    بازگشت پوچ;

	// Not in a function scope, find variable with block ID equal to or
	// smaller than the current block id.  Use "cstack" to go up the block
	// scopes.
	وقتی (sav != پوچ)
	{
	    صحیح idx;

	    برای (idx = cstack->cs_idx; idx >= 0; --idx)
		اگر (cstack->cs_block_id[idx] == sav->sav_block_id)
		    بشکن;
	    اگر (idx >= 0)
		بشکن;
	    sav = sav->sav_next;
	}
	بازگشت sav;
    }

    // Go over the variables with this name and find one that was visible
    // from the function.
    ufunc = cctx->ctx_ufunc;
    وقتی (sav != پوچ)
    {
	صحیح idx;

	اگر (ufunc->uf_block_depth == 0 && sav->sav_block_id == 0)
	    // If the function was defined at the script level (not inside a
	    // block), script-scope variables are always visible.
	    بازگشت sav;

	// Go over the blocks that this function was defined in.  If the
	// variable block ID matches it was visible to the function.
	برای (idx = 0; idx < ufunc->uf_block_depth; ++idx)
	    اگر (ufunc->uf_block_ids[idx] == sav->sav_block_id)
		بازگشت sav;
	sav = sav->sav_next;
    }

    // Not found, variable was not visible.
    بازگشت پوچ;
}

/*
 * If "name" can be found in the current script set it's "block_id".
 */
    تهی
update_script_var_block_id(char_u *name, صحیح block_id)
{
    scriptitem_T    *si = SCRIPT_ITEM(current_sctx.sc_sid);
    hashitem_T	    *hi;
    sallvar_T	    *sav;

    hi = hash_find(&si->sn_all_vars.dv_hashtab, name);
    اگر (HASHITEM_EMPTY(hi))
	بازگشت;
    sav = HI2SAV(hi);
    sav->sav_block_id = block_id;
}

/*
 * Lookup a variable (without s: prefix) in the current script.
 * "cctx" is NULL at the script level, "cstack" is NULL in a function.
 * Returns OK or FAIL.
 */
    صحیح
script_var_exists(char_u *name, size_t len, cctx_T *cctx, cstack_T *cstack)
{
    اگر (current_sctx.sc_sid <= 0)
	بازگشت FAIL;
    اگر (current_script_is_vim9())
    {
	// Check script variables that were visible where the function was
	// defined.
	اگر (find_script_var(name, len, cctx, cstack) != پوچ)
	    بازگشت OK;
    }
    وگرنه
    {
	hashtab_T	*ht = &SCRIPT_VARS(current_sctx.sc_sid);
	dictitem_T	*di;
	صحیح		cc;

	// Check script variables that are currently visible
	cc = name[len];
	name[len] = NUL;
	di = find_var_in_ht(ht, 0, name, TRUE);
	name[len] = cc;
	اگر (di != پوچ)
	    بازگشت OK;
    }

    بازگشت FAIL;
}

/*
 * Returns the index of a class method or class variable with name "name"
 * accessible in the currently compiled function.
 * If "cl_ret" is not NULL set it to the class.
 * Otherwise return -1.
 */
    ایستا صحیح
cctx_class_midx(
    cctx_T  *cctx,
    صحیح	    is_method,
    char_u  *name,
    size_t  len,
    class_T **cl_ret)
{
    اگر (cctx == پوچ || cctx->ctx_ufunc == پوچ
	    || cctx->ctx_ufunc->uf_class == پوچ
	    || cctx->ctx_ufunc->uf_defclass == پوچ)
	بازگشت -1;

    // Search for the class method or variable in the class where the calling
    // function is defined.
    class_T *cl = cctx->ctx_ufunc->uf_defclass;
    صحیح m_idx = is_method ? class_method_idx(cl, name, len)
					: class_member_idx(cl, name, len);
    اگر (m_idx < 0)
    {
	cl = cl->class_extends;
	وقتی (cl != پوچ)
	{
	    m_idx = is_method ? class_method_idx(cl, name, len)
					: class_member_idx(cl, name, len);
	    اگر (m_idx >= 0)
		بشکن;
	    cl = cl->class_extends;
	}
    }

    اگر (m_idx >= 0)
    {
	اگر (cl_ret != پوچ)
	    *cl_ret = cl;
    }

    بازگشت m_idx;
}

/*
 * Returns the index of a class method with name "name" accessible in the
 * currently compiled function.  Returns -1 if not found.  The class where the
 * method is defined is returned in "cl_ret".
 */
    صحیح
cctx_class_method_idx(
    cctx_T  *cctx,
    char_u  *name,
    size_t  len,
    class_T **cl_ret)
{
    بازگشت cctx_class_midx(cctx, TRUE, name, len, cl_ret);
}

/*
 * Returns the index of a class variable with name "name" accessible in the
 * currently compiled function.  Returns -1 if not found.  The class where the
 * variable is defined is returned in "cl_ret".
 */
    صحیح
cctx_class_member_idx(
    cctx_T  *cctx,
    char_u  *name,
    size_t  len,
    class_T **cl_ret)
{
    بازگشت cctx_class_midx(cctx, FALSE, name, len, cl_ret);
}

/*
 * Return TRUE if "name" is a local variable, argument, script variable or
 * imported.  Also if "name" is "this" and in a class method.
 */
    ایستا صحیح
variable_exists(char_u *name, size_t len, cctx_T *cctx)
{
    بازگشت (cctx != پوچ
		&& (lookup_local(name, len, پوچ, cctx) == OK
		    || arg_exists(name, len, پوچ, پوچ, پوچ, cctx) == OK
		    || (len == 4
			&& cctx->ctx_ufunc != پوچ
			&& (cctx->ctx_ufunc->uf_flags & (FC_OBJECT|FC_NEW))
			&& STRNCMP(name, "this", 4) == 0)))
	    || script_var_exists(name, len, cctx, پوچ) == OK
	    || cctx_class_member_idx(cctx, name, len, پوچ) >= 0
	    || find_imported(name, len, FALSE) != پوچ;
}

/*
 * Return TRUE if "name" is a local variable, argument, script variable,
 * imported or function.  Or commands are being skipped, a declaration may have
 * been skipped then.
 */
    ایستا صحیح
item_exists(char_u *name, size_t len, صحیح cmd UNUSED, cctx_T *cctx)
{
    بازگشت variable_exists(name, len, cctx);
}

/*
 * Check if "p[len]" is already defined, either in script "import_sid" or in
 * compilation context "cctx".
 * "cctx" is NULL at the script level, "cstack" is NULL in a function.
 * Does not check the global namespace.
 * If "is_arg" is TRUE the error message is for an argument name.
 * Return FAIL and give an error if it defined.
 */
    صحیح
check_defined(
	char_u	    *p,
	size_t	    len,
	cctx_T	    *cctx,
	cstack_T    *cstack,
	صحیح	    is_arg)
{
    صحیح		c = p[len];
    ufunc_T	*ufunc = پوچ;

    // underscore argument is OK
    اگر (len == 1 && *p == '_')
	بازگشت OK;

    اگر (script_var_exists(p, len, cctx, cstack) == OK)
    {
	اگر (is_arg)
	    semsg(_(e_argument_already_declared_in_script_str), p);
	وگرنه
	    semsg(_(e_variable_already_declared_in_script_str), p);
	بازگشت FAIL;
    }

    اگر (cctx_class_member_idx(cctx, p, len, پوچ) >= 0)
    {
	اگر (is_arg)
	    semsg(_(e_argument_already_declared_in_class_str), p);
	وگرنه
	    semsg(_(e_variable_already_declared_in_class_str), p);
	بازگشت FAIL;
    }

    p[len] = NUL;
    اگر ((cctx != پوچ
		&& (lookup_local(p, len, پوچ, cctx) == OK
		    || arg_exists(p, len, پوچ, پوچ, پوچ, cctx) == OK))
	    || find_imported(p, len, FALSE) != پوچ
	    || (ufunc = find_func_even_dead(p, 0)) != پوچ)
    {
	// A local or script-local function can shadow a global function.
	اگر (ufunc == پوچ || ((ufunc->uf_flags & FC_DEAD) == 0
		    && (!func_is_global(ufunc)
					     || (p[0] == 'g' && p[1] == ':'))))
	{
	    اگر (is_arg)
		semsg(_(e_argument_name_shadows_existing_variable_str), p);
	    وگرنه
		semsg(_(e_name_already_defined_str), p);
	    p[len] = c;
	    بازگشت FAIL;
	}
    }
    p[len] = c;
    بازگشت OK;
}


/*
 * Return TRUE if "actual" could be "expected" and a runtime typecheck is to be
 * used.  Return FALSE if the types will never match.
 */
    ایستا صحیح
use_typecheck(type_T *actual, type_T *expected)
{
    اگر (actual->tt_type == VAR_ANY
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
	بازگشت TRUE;
    اگر (actual->tt_type == VAR_OBJECT && expected->tt_type == VAR_OBJECT)
	بازگشت TRUE;
    اگر ((actual->tt_type == VAR_LIST
		|| actual->tt_type == VAR_TUPLE
		|| actual->tt_type == VAR_DICT)
	    && actual->tt_type == expected->tt_type)
	// This takes care of a nested list or dict.
	بازگشت use_typecheck(actual->tt_member, expected->tt_member);
    بازگشت FALSE;
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
    صحیح
need_type_where(
	type_T	*actual,
	type_T	*expected,
	صحیح	typechk_flags,	// acceptable types (type check flags)
	صحیح	offset,
	where_T	where,
	cctx_T	*cctx,
	صحیح	silent,
	صحیح	actual_is_const)
{
    صحیح ret;

    اگر (expected->tt_type != VAR_CLASS && expected->tt_type != VAR_TYPEALIAS)
    {
	اگر (check_type_is_مقدار(actual) == FAIL)
	    بازگشت FAIL;
    }

    اگر (expected == &t_bool && actual != &t_bool
					&& (actual->tt_flags & TTFLAG_BOOL_OK))
    {
	// Using "0", "1" or the result of an expression with "&&" or "||" as a
	// boolean is OK but requires a conversion.
	generate_2BOOL(cctx, FALSE, offset);
	بازگشت OK;
    }

    ret = check_type_maybe(expected, actual, FALSE, where);
    اگر (ret == OK)
	بازگشت OK;

    // If actual a constant a runtime check makes no sense.  If it's
    // null_function it is OK.
    اگر (actual_is_const && ret == MAYBE && actual == &t_func_unknown)
	بازگشت OK;

    // If the actual type can be the expected type add a runtime check.
    اگر (!actual_is_const && ret == MAYBE && use_typecheck(actual, expected))
    {
	generate_TYPECHECK(cctx, expected, typechk_flags, offset,
		where.wt_kind == WT_VARIABLE, where.wt_index);
	بازگشت OK;
    }

    اگر (!silent)
	type_mismatch_where(expected, actual, where);
    بازگشت FAIL;
}

    صحیح
need_type(
	type_T	*actual,
	type_T	*expected,
	صحیح	typechk_flags,	// acceptable types (type check flags)
	صحیح	offset,
	صحیح	arg_idx,
	cctx_T	*cctx,
	صحیح	silent,
	صحیح	actual_is_const)
{
    where_T where = WHERE_INIT;

    اگر (arg_idx > 0)
    {
	where.wt_index = arg_idx;
	where.wt_kind = WT_ARGUMENT;
    }
    بازگشت need_type_where(actual, expected, typechk_flags, offset, where,
						cctx, silent, actual_is_const);
}

/*
 * Set type of variable "lvar" to "type".  If the variable is a constant then
 * the type gets TTFLAG_CONST.
 */
    ایستا تهی
set_var_type(lvar_T *lvar, type_T *type_arg, cctx_T *cctx)
{
    type_T	*type = type_arg;

    اگر (lvar->lv_const == ASSIGN_CONST && (type->tt_flags & TTFLAG_CONST) == 0)
    {
	اگر (type->tt_flags & TTFLAG_STATIC)
	    // entry in static_types[] is followed by const type
	    type = type + 1;
	وگرنه
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
	صحیح	assign,
	type_T	*type)
{
    lvar_T  *lvar;
    dfunc_T *dfunc;

    اگر (arg_exists(name, len, پوچ, پوچ, پوچ, cctx) == OK)
    {
	emsg_namelen(_(e_str_is_used_as_argument), name, (صحیح)len);
	بازگشت پوچ;
    }

    اگر (GA_GROW_FAILS(&cctx->ctx_locals, 1))
	بازگشت پوچ;
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
    اگر (type == &t_unknown || type == &t_any)
	// type not known yet, may be inferred from RHS
	lvar->lv_type = type;
    وگرنه
	// may use TTFLAG_CONST
	set_var_type(lvar, type, cctx);

    // Remember the name for debugging.
    اگر (GA_GROW_FAILS(&dfunc->df_var_names, 1))
	بازگشت پوچ;
    ((char_u **)dfunc->df_var_names.ga_data)[lvar->lv_idx] =
						    vim_strsave(lvar->lv_name);
    ++dfunc->df_var_names.ga_len;

    بازگشت lvar;
}

/*
 * If "check_writable" is ASSIGN_CONST give an error if the variable was
 * defined with :final or :const, if "check_writable" is ASSIGN_FINAL give an
 * error if the variable was defined with :const.
 */
    ایستا صحیح
check_item_writable(svar_T *sv, صحیح check_writable, char_u *name)
{
    اگر ((check_writable == ASSIGN_CONST && sv->sv_const != 0)
	    || (check_writable == ASSIGN_FINAL
					      && sv->sv_const == ASSIGN_CONST))
    {
	semsg(_(e_cannot_change_readonly_variable_str), name);
	بازگشت FAIL;
    }
    بازگشت OK;
}

/*
 * Find "name" in script-local items of script "sid".
 * Pass "check_writable" to check_item_writable().
 * "cctx" is NULL at the script level, "cstack" is NULL in a function.
 * Returns the index in "sn_var_vals" if found.
 * If found but not in "sn_var_vals" returns -1.
 * If not found or the variable is not writable returns -2.
 */
    صحیح
get_script_item_idx(
	صحیح	    sid,
	char_u	    *name,
	صحیح	    check_writable,
	cctx_T	    *cctx,
	cstack_T    *cstack)
{
    hashtab_T	    *ht;
    dictitem_T	    *di;
    scriptitem_T    *si = SCRIPT_ITEM(sid);
    svar_T	    *sv;
    صحیح		    idx;

    اگر (!SCRIPT_ID_VALID(sid))
	بازگشت -1;
    اگر (sid == current_sctx.sc_sid)
    {
	sallvar_T *sav = find_script_var(name, 0, cctx, cstack);

	اگر (sav == پوچ)
	    بازگشت -2;
	idx = sav->sav_var_vals_idx;
	sv = ((svar_T *)si->sn_var_vals.ga_data) + idx;
	اگر (check_item_writable(sv, check_writable, name) == FAIL)
	    بازگشت -2;
	بازگشت idx;
    }

    // First look the name up in the hashtable.
    ht = &SCRIPT_VARS(sid);
    di = find_var_in_ht(ht, 0, name, TRUE);
    اگر (di == پوچ)
    {
	اگر (si->sn_autoload_prefix != پوچ)
	{
	    hashitem_T *hi;

	    // A variable exported from an autoload script is in the global
	    // variables, we can find it in the all_vars table.
	    hi = hash_find(&si->sn_all_vars.dv_hashtab, name);
	    اگر (!HASHITEM_EMPTY(hi))
		بازگشت HI2SAV(hi)->sav_var_vals_idx;
	}
	بازگشت -2;
    }

    // Now find the svar_T index in sn_var_vals.
    برای (idx = 0; idx < si->sn_var_vals.ga_len; ++idx)
    {
	sv = ((svar_T *)si->sn_var_vals.ga_data) + idx;
	اگر (sv->sv_tv == &di->di_tv)
	{
	    اگر (check_item_writable(sv, check_writable, name) == FAIL)
		بازگشت -2;
	    بازگشت idx;
	}
    }
    بازگشت -1;
}

    ایستا imported_T *
find_imported_in_script(char_u *name, size_t len, صحیح sid)
{
    scriptitem_T    *si;
    صحیح		    idx;

    اگر (!SCRIPT_ID_VALID(sid))
	بازگشت پوچ;
    si = SCRIPT_ITEM(sid);
    برای (idx = 0; idx < si->sn_imports.ga_len; ++idx)
    {
	imported_T *import = ((imported_T *)si->sn_imports.ga_data) + idx;

	اگر (len == 0 ? STRCMP(name, import->imp_name) == 0
		     : STRLEN(import->imp_name) == len
				  && STRNCMP(name, import->imp_name, len) == 0)
	    بازگشت import;
    }
    بازگشت پوچ;
}

/*
 * Find "name" in imported items of the current script.
 * If "len" is 0 use any length that works.
 * If "load" is TRUE and the script was not loaded yet, load it now.
 */
    imported_T *
find_imported(char_u *name, size_t len, صحیح load)
{
    اگر (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	بازگشت پوچ;

    // Skip over "s:" before "s:something" to find the import name.
    صحیح off = name[0] == 's' && name[1] == ':' ? 2 : 0;

    imported_T *ret = find_imported_in_script(name + off, len - off,
							  current_sctx.sc_sid);
    اگر (ret != پوچ && load && (ret->imp_flags & IMP_FLAGS_AUTOLOAD))
    {
	scid_T	actual_sid = 0;
	صحیح	save_emsg_off = emsg_off;

	// "emsg_off" will be set when evaluating an expression silently, but
	// we do want to know about errors in a script.  Also because it then
	// aborts when an error is encountered.
	emsg_off = FALSE;

	// script found before but not loaded yet
	ret->imp_flags &= ~IMP_FLAGS_AUTOLOAD;
	(تهی)do_source(SCRIPT_ITEM(ret->imp_sid)->sn_name, FALSE,
						       DOSO_NONE, &actual_sid);
	// If the script is a symlink it may be sourced with another name, may
	// need to adjust the script ID for that.
	اگر (actual_sid != 0)
	    ret->imp_sid = actual_sid;

	emsg_off = save_emsg_off;
    }
    بازگشت ret;
}

/*
 * Called when checking for a following operator at "arg".  When the rest of
 * the line is empty or only a comment, peek the next line.  If there is a next
 * line return a pointer to it and set "nextp".
 * Otherwise skip over white space.
 */
    char_u *
may_peek_next_سطر(cctx_T *cctx, char_u *arg, char_u **nextp)
{
    char_u *p = skipwhite(arg);

    *nextp = پوچ;
    اگر (*p == NUL || (VIM_ISWHITE(*arg) && vim9_comment_start(p)))
    {
	*nextp = peek_next_سطر_from_context(cctx);
	اگر (*nextp != پوچ)
	    بازگشت *nextp;
    }
    بازگشت p;
}

/*
 * Return a pointer to the next line that isn't empty or only contains a
 * comment. Skips over white space.
 * Returns NULL if there is none.
 */
    char_u *
peek_next_سطر_from_context(cctx_T *cctx)
{
    صحیح lnum = cctx->ctx_lnum;

    وقتی (++lnum < cctx->ctx_ufunc->uf_سطرها.ga_len)
    {
	char_u *سطر = ((char_u **)cctx->ctx_ufunc->uf_سطرها.ga_data)[lnum];
	char_u *p;

	// ignore NULLs inserted for continuation lines
	اگر (سطر != پوچ)
	{
	    p = skipwhite(سطر);
	    اگر (vim9_bad_comment(p))
		بازگشت پوچ;
	    اگر (*p != NUL && !vim9_comment_start(p))
		بازگشت p;
	}
    }
    بازگشت پوچ;
}

/*
 * Get the next line of the function from "cctx".
 * Skips over empty lines.  Skips over comment lines if "skip_comment" is TRUE.
 * Returns NULL when at the end.
 */
    char_u *
next_سطر_from_context(cctx_T *cctx, صحیح skip_comment)
{
    char_u	*سطر;

    انجامبده
    {
	++cctx->ctx_lnum;
	اگر (cctx->ctx_lnum >= cctx->ctx_ufunc->uf_سطرها.ga_len)
	{
	    سطر = پوچ;
	    بشکن;
	}
	سطر = ((char_u **)cctx->ctx_ufunc->uf_سطرها.ga_data)[cctx->ctx_lnum];
	cctx->ctx_سطر_start = سطر;
	SOURCING_LNUM = cctx->ctx_lnum + 1;
    } وقتی (سطر == پوچ || *skipwhite(سطر) == NUL
		     || (skip_comment && vim9_comment_start(skipwhite(سطر))));
    بازگشت سطر;
}

/*
 * Skip over white space at "whitep" and assign to "*arg".
 * If "*arg" is at the end of the line, advance to the next line.
 * Also when "whitep" points to white space and "*arg" is on a "#".
 * Return FAIL if beyond the last line, "*arg" is unmodified then.
 */
    صحیح
may_get_next_سطر(char_u *whitep, char_u **arg, cctx_T *cctx)
{
    *arg = skipwhite(whitep);
    اگر (vim9_bad_comment(*arg))
	بازگشت FAIL;
    اگر (**arg == NUL || (VIM_ISWHITE(*whitep) && vim9_comment_start(*arg)))
    {
	char_u *next = next_سطر_from_context(cctx, TRUE);

	اگر (next == پوچ)
	    بازگشت FAIL;
	*arg = skipwhite(next);
    }
    بازگشت OK;
}

/*
 * Idem, and give an error when failed.
 */
    صحیح
may_get_next_سطر_error(char_u *whitep, char_u **arg, cctx_T *cctx)
{
    اگر (may_get_next_سطر(whitep, arg, cctx) == FAIL)
    {
	SOURCING_LNUM = cctx->ctx_lnum + 1;
	emsg(_(e_سطر_incomplete));
	بازگشت FAIL;
    }
    بازگشت OK;
}

/*
 * Get a line from the compilation context, compatible with exarg_T getline().
 * Return a pointer to the line in allocated memory.
 * Return NULL for end-of-file or some error.
 */
    ایستا char_u *
exarg_getline(
	صحیح c UNUSED,
	تهی *cookie,
	صحیح indent UNUSED,
	getline_opt_T options UNUSED)
{
    cctx_T  *cctx = (cctx_T *)cookie;
    char_u  *p;

    برای (;;)
    {
	اگر (cctx->ctx_lnum >= cctx->ctx_ufunc->uf_سطرها.ga_len - 1)
	    بازگشت پوچ;
	++cctx->ctx_lnum;
	p = ((char_u **)cctx->ctx_ufunc->uf_سطرها.ga_data)[cctx->ctx_lnum];
	// Comment lines result in NULL pointers, skip them.
	اگر (p != پوچ)
	    بازگشت vim_strsave(p);
    }
}

    تهی
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
    صحیح
func_needs_compiling(ufunc_T *ufunc, compiletype_T compile_type)
{
    انتخاب (ufunc->uf_def_status)
    {
	حالت UF_TO_BE_COMPILED:
	    بازگشت TRUE;

	حالت UF_COMPILED:
	{
	    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data)
							 + ufunc->uf_dfunc_idx;

	    انتخاب (compile_type)
	    {
		حالت CT_PROFILE:
#ifdef FEAT_PROFILE
		    بازگشت dfunc->df_instr_prof == پوچ;
#endif
		حالت CT_NONE:
		    بازگشت dfunc->df_instr == پوچ;
		حالت CT_DEBUG:
		    بازگشت dfunc->df_instr_debug == پوچ;
	    }
	}

	حالت UF_NOT_COMPILED:
	حالت UF_COMPILE_ERROR:
	حالت UF_COMPILING:
	    بشکن;
    }
    بازگشت FALSE;
}

/*
 * Compile a nested :def command.
 */
    ایستا char_u *
compile_nested_function(exarg_T *eap, cctx_T *cctx, garray_T *سطرها_to_free)
{
    صحیح		is_global = *eap->arg == 'g' && eap->arg[1] == ':';
    char_u	*name_start = eap->arg;
    char_u	*name_end = to_name_end(eap->arg, TRUE);
    صحیح		off;
    char_u	*func_name;
    string_T	lambda_name;
    ufunc_T	*ufunc;
    صحیح		r = FAIL;
    compiletype_T   compile_type;
    صحیح		funcref_isn_idx = -1;
    lvar_T	*lvar = پوچ;
    char_u	*bracket_start = پوچ;

    اگر (eap->forceit)
    {
	semsg(_(e_cannot_use_bang_with_nested_def_str),
		eap->cmdidx == CMD_def ? ":def" : ":function");
	بازگشت پوچ;
    }

    اگر (*name_start == '/')
    {
	name_end = skip_regexp(name_start + 1, '/', TRUE);
	اگر (*name_end == '/')
	    ++name_end;
	set_nextcmd(eap, name_end);
    }

    اگر (*name_end == '<')
    {
	bracket_start = name_end;
	اگر (skip_generic_func_type_args(&name_end) == FAIL)
	    بازگشت پوچ;
    }

    اگر (name_end == name_start || *skipwhite(name_end) != '(')
    {
	اگر (!ends_excmd2(name_start, name_end))
	{
	    اگر (*skipwhite(name_end) == '.')
		semsg(_(e_cannot_define_dict_func_in_vim9_script_str),
								     eap->cmd);
	    وگرنه
		semsg(_(e_invalid_command_str), eap->cmd);
	    بازگشت پوچ;
	}

	// "def" or "def Name": list functions
	اگر (generate_DEF(cctx, name_start, name_end - name_start) == FAIL)
	    بازگشت پوچ;
	بازگشت eap->nextcmd == پوچ ? (char_u *)"" : eap->nextcmd;
    }

    اگر (bracket_start != پوچ)
	// generic function.  The function name ends before the list of types
	// (opening angle bracket).
	name_end = bracket_start;

    // Only g:Func() can use a namespace.
    اگر (name_start[1] == ':' && !is_global)
    {
	semsg(_(e_namespace_not_supported_str), name_start);
	بازگشت پوچ;
    }
    اگر (cctx->ctx_skip != SKIP_YES
	    && check_defined(name_start, name_end - name_start, cctx,
							  پوچ, FALSE) == FAIL)
	بازگشت پوچ;
    اگر (!ASCII_ISUPPER(is_global ? name_start[2] : name_start[0]))
    {
	semsg(_(e_function_name_must_start_with_capital_str), name_start);
	بازگشت پوچ;
    }

    eap->arg = name_end;
    fill_exarg_from_cctx(eap, cctx);

    eap->forceit = FALSE;
    // We use the special <lambda>99 name, but it's not really a lambda.
    lambda_name = get_lambda_name();
    lambda_name.string = vim_strnsave(lambda_name.string, lambda_name.length);
    اگر (lambda_name.string == پوچ)
	بازگشت پوچ;

    // This may free the current line, make a copy of the name.
    off = is_global ? 2 : 0;
    func_name = vim_strnsave(name_start + off, name_end - name_start - off);
    اگر (func_name == پوچ)
    {
	r = FAIL;
	برو theend;
    }

    // A type variable cannot have the name of the function.  The function is
    // defined with the "<lambda>" name, check it here.
    اگر (bracket_start != پوچ)
    {
	size_t	len = STRLEN(func_name);

	برای (char_u *t = bracket_start + 1; *t != '>' && *t != NUL; )
	{
	    char_u *te = to_name_end(t, FALSE);

	    اگر ((size_t)(te - t) == len && STRNCMP(t, func_name, len) == 0)
	    {
		semsg(_(e_redefining_script_item_str), func_name);
		r = FAIL;
		برو theend;
	    }
	    t = te == t ? t + 1 : te;
	}
    }

    // Make sure "KeyTyped" is not set, it may cause indent to be written.
    صحیح save_کلید_Typed = کلید_Typed;
    کلید_Typed = FALSE;

    ufunc = define_function(eap, lambda_name.string, سطرها_to_free, 0, پوچ, 0,
									cctx);

    کلید_Typed = save_کلید_Typed;

    اگر (ufunc == پوچ)
    {
	r = eap->skip ? OK : FAIL;
	برو theend;
    }
    اگر (eap->nextcmd != پوچ)
    {
	semsg(_(e_text_found_after_str_str),
	      eap->cmdidx == CMD_def ? "enddef" : "endfunction", eap->nextcmd);
	r = FAIL;
	func_ptr_unref(ufunc);
	برو theend;
    }

    // copy over the block scope IDs before compiling
    اگر (!is_global && cctx->ctx_ufunc->uf_block_depth > 0)
    {
	صحیح block_depth = cctx->ctx_ufunc->uf_block_depth;

	ufunc->uf_block_ids = ALLOC_MULT(صحیح, block_depth);
	اگر (ufunc->uf_block_ids != پوچ)
	{
	    mch_memmove(ufunc->uf_block_ids, cctx->ctx_ufunc->uf_block_ids,
						    اندازه(صحیح) * block_depth);
	    ufunc->uf_block_depth = block_depth;
	}
    }

    // Define the funcref before compiling, so that it is found by any
    // recursive call.
    اگر (is_global)
    {
	r = generate_NEWFUNC(cctx, lambda_name.string, func_name);
	func_name = پوچ;
	lambda_name.string = پوچ;
	lambda_name.length = 0;
    }
    وگرنه
    {
	// Define a local variable for the function reference.
	lvar = reserve_local(cctx, func_name, name_end - name_start,
					    ASSIGN_CONST, ufunc->uf_func_type);
	اگر (lvar == پوچ)
	{
	    func_ptr_unref(ufunc);
	    برو theend;
	}
	lvar->lv_ufunc = ufunc;
	اگر (generate_FUNCREF(cctx, ufunc, پوچ, FALSE, 0, &funcref_isn_idx) == FAIL)
	{
	    func_ptr_unref(ufunc);
	    برو theend;
	}
	r = generate_STORE(cctx, ISN_STORE, lvar->lv_idx, پوچ);
    }

    compile_type = get_compile_type(ufunc);
#ifdef FEAT_PROFILE
    // If the outer function is profiled, also compile the nested function for
    // profiling.
    اگر (cctx->ctx_compile_type == CT_PROFILE)
	compile_type = CT_PROFILE;
#endif
    اگر (func_needs_compiling(ufunc, compile_type)
	    && compile_def_function(ufunc, TRUE, compile_type, cctx) == FAIL)
    {
	func_ptr_unref(ufunc);
	اگر (lvar != پوچ)
	    // Now the local variable can't be used.
	    *lvar->lv_name = '/';  // impossible value
	برو theend;
    }

#ifdef FEAT_PROFILE
    // When the outer function is compiled for profiling, the nested function
    // may be called without profiling.  Compile it here in the right context.
    اگر (compile_type == CT_PROFILE && func_needs_compiling(ufunc, CT_NONE))
	compile_def_function(ufunc, FALSE, CT_NONE, cctx);
#endif

    // If a FUNCREF instruction was generated, set the index after compiling.
    اگر (funcref_isn_idx != -1 && ufunc->uf_def_status == UF_COMPILED)
    {
	isn_T	*funcref_isn = ((isn_T *)cctx->ctx_instr.ga_data) +
							funcref_isn_idx;
	funcref_isn->isn_arg.funcref.fr_dfunc_idx = ufunc->uf_dfunc_idx;
    }

theend:
    vim_free(lambda_name.string);
    vim_free(func_name);
    بازگشت r == FAIL ? پوچ : (char_u *)"";
}

/*
 * Compile one Vim expression {expr} in string "p".
 * "p" points to the opening "{".
 * Return a pointer to the character after "}", NULL for an error.
 */
    char_u *
compile_one_عبارت_in_str(char_u *p, cctx_T *cctx)
{
    char_u	*block_start;
    char_u	*block_end;

    // Skip the opening {.
    block_start = skipwhite(p + 1);
    block_end = block_start;
    اگر (*block_start != NUL && skip_عبارت(&block_end, پوچ) == FAIL)
	بازگشت پوچ;
    block_end = skipwhite(block_end);
    // The block must be closed by a }.
    اگر (*block_end != '}')
    {
	semsg(_(e_missing_close_curly_str), p);
	بازگشت پوچ;
    }
    اگر (compile_expr0(&block_start, cctx) == FAIL)
	بازگشت پوچ;
    may_generate_2STRING(-1, TOSTRING_INTERPOLATE, cctx);

    بازگشت block_end + 1;
}

/*
 * Compile a string "str" (either containing a literal string or a mix of
 * literal strings and Vim expressions of the form `{expr}`).  This is used
 * when compiling a heredoc assignment to a variable or an interpolated string
 * in a Vim9 def function.  Vim9 instructions are generated to push strings,
 * evaluate expressions, concatenate them and create a list of lines.  When
 * "evalstr" is TRUE, Vim expressions in "str" are evaluated.
 */
    صحیح
compile_all_عبارت_in_str(char_u *str, صحیح evalstr, cctx_T *cctx)
{
    char_u	*p = str;
    char_u	*val;
    صحیح		شمار = 0;

    اگر (cctx->ctx_skip == SKIP_YES)
	بازگشت OK;

    اگر (!evalstr || *str == NUL)
    {
	// Literal string, possibly empty.
	val = *str != NUL ? vim_strsave(str) : پوچ;
	بازگشت generate_PUSHS(cctx, &val);
    }

    // Push all the string pieces to the stack, followed by a ISN_CONCAT.
    وقتی (*p != NUL)
    {
	char_u	*lit_start;
	صحیح	escaped_brace = FALSE;

	// Look for a block start.
	lit_start = p;
	وقتی (*p != '{' && *p != '}' && *p != NUL)
	    ++p;

	اگر (*p != NUL && *p == p[1])
	{
	    // Escaped brace, unescape and continue.
	    // Include the brace in the literal string.
	    ++p;
	    escaped_brace = TRUE;
	}
	وگرنه اگر (*p == '}')
	{
	    semsg(_(e_stray_closing_curly_str), str);
	    بازگشت FAIL;
	}

	// Append the literal part.
	اگر (p != lit_start)
	{
	    val = vim_strnsave(lit_start, (size_t)(p - lit_start));
	    اگر (generate_PUSHS(cctx, &val) == FAIL)
		بازگشت FAIL;
	    ++شمار;
	}

	اگر (*p == NUL)
	    بشکن;

	اگر (escaped_brace)
	{
	    // Skip the second brace.
	    ++p;
	    ادامه;
	}

	p = compile_one_عبارت_in_str(p, cctx);
	اگر (p == پوچ)
	    بازگشت FAIL;
	++شمار;
    }

    // Small optimization, if there's only a single piece skip the ISN_CONCAT.
    اگر (شمار > 1)
	بازگشت generate_CONCAT(cctx, شمار);

    بازگشت OK;
}

/*
 * Return the length of an assignment operator, or zero if there isn't one.
 */
    صحیح
assignment_len(char_u *p, صحیح *heredoc)
{
    اگر (*p == '=')
    {
	اگر (p[1] == '<' && p[2] == '<')
	{
	    *heredoc = TRUE;
	    بازگشت 3;
	}
	بازگشت 1;
    }
    اگر (vim_strchr((char_u *)"+-*/%", *p) != پوچ && p[1] == '=')
	بازگشت 2;
    اگر (STRNCMP(p, "..=", 3) == 0)
	بازگشت 3;
    بازگشت 0;
}

/*
 * Generate the load instruction for "name".
 */
    ایستا صحیح
generate_loadvar(cctx_T *cctx, lhs_T *lhs)
{
    char_u	*name = lhs->lhs_name;
    type_T	*type = lhs->lhs_type;
    صحیح		res = OK;

    انتخاب (lhs->lhs_dest)
    {
	حالت dest_option:
	حالت dest_func_option:
	    generate_LOAD(cctx, ISN_LOADOPT, 0, name, type);
	    بشکن;
	حالت dest_global:
	    اگر (vim_strchr(name, AUTOLOAD_CHAR) == پوچ)
	    {
		اگر (name[2] == NUL)
		    generate_instr_type(cctx, ISN_LOADGDICT, &t_dict_any);
		وگرنه
		    generate_LOAD(cctx, ISN_LOADG, 0, name + 2, type);
	    }
	    وگرنه
		generate_LOAD(cctx, ISN_LOADAUTO, 0, name, type);
	    بشکن;
	حالت dest_buffer:
	    generate_LOAD(cctx, ISN_LOADB, 0, name + 2, type);
	    بشکن;
	حالت dest_window:
	    generate_LOAD(cctx, ISN_LOADW, 0, name + 2, type);
	    بشکن;
	حالت dest_tab:
	    generate_LOAD(cctx, ISN_LOADT, 0, name + 2, type);
	    بشکن;
	حالت dest_script:
	حالت dest_script_v9:
	    res = compile_load_scriptvar(cctx,
			    name + (name[1] == ':' ? 2 : 0), پوچ, پوچ, پوچ);
	    بشکن;
	حالت dest_env:
	    // Include $ in the name here
	    generate_LOAD(cctx, ISN_LOADENV, 0, name, type);
	    بشکن;
	حالت dest_reg:
	    generate_LOAD(cctx, ISN_LOADREG, name[1], پوچ, &t_string);
	    بشکن;
	حالت dest_vimvar:
	    generate_LOADV(cctx, name + 2);
	    بشکن;
	حالت dest_local:
	    اگر (cctx->ctx_skip != SKIP_YES)
	    {
		lvar_T	*lvar = lhs->lhs_lvar;
		اگر (lvar->lv_from_outer > 0)
		    generate_LOADOUTER(cctx, lvar->lv_idx, lvar->lv_from_outer,
				 lvar->lv_loop_depth, lvar->lv_loop_idx, type);
		وگرنه
		    generate_LOAD(cctx, ISN_LOAD, lvar->lv_idx, پوچ, type);
	    }
	    بشکن;
	حالت dest_class_member:
	    generate_CLASSMEMBER(cctx, TRUE, lhs->lhs_class,
						     lhs->lhs_classmember_idx);
	    بشکن;
	حالت dest_عبارت:
	    // list or dict value should already be on the stack.
	    بشکن;
    }

    بازگشت res;
}

/*
 * Skip over "[expr]" or ".member".
 * Does not check for any errors.
 */
    ایستا char_u *
skip_index(char_u *start)
{
    char_u *p = start;

    اگر (*p == '[')
    {
	p = skipwhite(p + 1);
	(تهی)skip_عبارت(&p, پوچ);
	p = skipwhite(p);
	اگر (*p == ']')
	    بازگشت p + 1;
	بازگشت p;
    }
    // if (*p == '.')
    بازگشت to_name_end(p + 1, TRUE);
}

    تهی
vim9_declare_error(char_u *name)
{
    نویسه *scope = "";

    انتخاب (*name)
    {
	حالت 'g': scope = _("global"); بشکن;
	حالت 'b': scope = _("buffer"); بشکن;
	حالت 'w': scope = _("window"); بشکن;
	حالت 't': scope = _("tab"); بشکن;
	حالت 'v': scope = "v:"; بشکن;
	حالت '$': semsg(_(e_cannot_declare_an_environment_variable_str), name);
		  بازگشت;
	حالت '&': semsg(_(e_cannot_declare_an_option_str), name);
		  بازگشت;
	حالت '@': semsg(_(e_cannot_declare_a_register_str), name);
		  بازگشت;
	پیشفرض: بازگشت;
    }
    semsg(_(e_cannot_declare_a_scope_variable_str), scope, name);
}

/*
 * Return TRUE if "name" is a valid register to use.
 * Return FALSE and give an error message if not.
 */
    ایستا صحیح
valid_dest_reg(صحیح name)
{
    اگر (name == '@')
       name = '"';
    اگر (name == '/' || name == '=' || name == '#' || valid_yank_reg(name, TRUE))
	بازگشت TRUE;
    emsg_invreg(name);
    بازگشت FAIL;
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
    صحیح
get_var_dest(
	char_u		*name,
	assign_dest_T	*dest,
	cmdidx_T	cmdidx,
	صحیح		has_index,
	صحیح		*option_scope,
	صحیح		*vimvaridx,
	type_T		**type,
	cctx_T		*cctx)
{
    char_u *p;

    اگر (*name == '&')
    {
	صحیح		cc;
	long		numval;
	getoption_T	opt_type;
	صحیح		opt_p_flags;

	*dest = dest_option;
	اگر (cmdidx == CMD_final || cmdidx == CMD_const)
	{
	    emsg(_(e_cannot_lock_option));
	    بازگشت FAIL;
	}
	p = name;
	p = find_option_end(&p, option_scope);
	اگر (p == پوچ)
	{
	    // cannot happen?
	    emsg(_(e_unexpected_characters_in_assignment));
	    بازگشت FAIL;
	}
	cc = *p;
	*p = NUL;
	opt_type = get_option_مقدار(skip_option_env_lead(name),
				   &numval, پوچ, &opt_p_flags, *option_scope);
	*p = cc;
	انتخاب (opt_type)
	{
	    حالت gov_unknown:
		    semsg(_(e_unknown_option_str), name);
		    بازگشت FAIL;
	    حالت gov_string:
	    حالت gov_hidden_string:
		    اگر (opt_p_flags & P_FUNC)
		    {
			// might be a Funcref, check the type later
			*type = &t_any;
			*dest = dest_func_option;
		    }
		    وگرنه
		    {
			*type = &t_string;
		    }
		    بشکن;
	    حالت gov_bool:
	    حالت gov_hidden_bool:
		    *type = &t_bool;
		    بشکن;
	    حالت gov_عدد:
	    حالت gov_hidden_عدد:
		    *type = &t_عدد;
		    بشکن;
	}
    }
    وگرنه اگر (*name == '$')
    {
	*dest = dest_env;
	*type = &t_string;
    }
    وگرنه اگر (*name == '@')
    {
	اگر (!valid_dest_reg(name[1]))
	    بازگشت FAIL;
	*dest = dest_reg;
	*type = name[1] == '#' ? &t_عدد_or_string : &t_string;
    }
    وگرنه اگر (STRNCMP(name, "g:", 2) == 0)
    {
	*dest = dest_global;
    }
    وگرنه اگر (STRNCMP(name, "b:", 2) == 0)
    {
	*dest = dest_buffer;
    }
    وگرنه اگر (STRNCMP(name, "w:", 2) == 0)
    {
	*dest = dest_window;
    }
    وگرنه اگر (STRNCMP(name, "t:", 2) == 0)
    {
	*dest = dest_tab;
    }
    وگرنه اگر (STRNCMP(name, "v:", 2) == 0)
    {
	typval_T	*vtv;
	صحیح		di_flags;

	*vimvaridx = find_vim_var(name + 2, &di_flags);
	اگر (*vimvaridx < 0)
	{
	    semsg(_(e_variable_not_found_str), name);
	    بازگشت FAIL;
	}
	// An item of a read-only variable can be changed, as when not compiled.
	// We use the current value of "sandbox" here, is that OK?
	اگر (!has_index && var_check_ro(di_flags, name, FALSE))
	    بازگشت FAIL;
	*dest = dest_vimvar;
	vtv = get_vim_var_tv(*vimvaridx);
	*type = typval2type_vimvar(vtv, cctx->ctx_type_list);
    }
    بازگشت OK;
}

    ایستا صحیح
is_decl_command(cmdidx_T cmdidx)
{
    بازگشت cmdidx == CMD_let || cmdidx == CMD_var
				 || cmdidx == CMD_final || cmdidx == CMD_const;
}

/*
 * Returns TRUE if the class or object variable in "lhs" is modifiable.
 * "var_start" points to the start of the variable name and "lhs->lhs_varlen"
 * has the total length.  Note that the "lhs" can be nested an object reference
 * (e.g.  a.b.c.d.var).
 */
    ایستا صحیح
lhs_class_member_modifiable(lhs_T *lhs, char_u	*var_start, cctx_T *cctx)
{
    size_t	varlen = lhs->lhs_varlen;
    class_T	*cl = lhs->lhs_type->tt_class;
    صحیح		is_object = lhs->lhs_type->tt_type == VAR_OBJECT;
    char_u	*name = var_start + varlen + 1;
    size_t	namelen = lhs->lhs_end - var_start - varlen - 1;
    ocmember_T	*m;

    m = member_lookup(cl, lhs->lhs_type->tt_type, name, namelen, پوچ);
    اگر (m == پوچ)
    {
	member_not_found_پیام(cl, lhs->lhs_type->tt_type, name, namelen);
	بازگشت FALSE;
    }

    اگر (IS_ENUM(cl))
    {
	semsg(_(e_enumvalue_str_cannot_be_modified), cl->class_name.string,
		m->ocm_name.string);
	بازگشت FALSE;
    }

    // If it is private member variable, then accessing it outside the
    // class is not allowed.
    // If it is a read only class variable, then it can be modified
    // only inside the class where it is defined.
    اگر ((m->ocm_access != VIM_ACCESS_ALL) &&
	    ((is_object && !inside_class(cctx, cl))
	     || (!is_object && cctx->ctx_ufunc->uf_class != cl)))
    {
	نویسه *پیام = (m->ocm_access == VIM_ACCESS_PRIVATE)
				? e_cannot_access_protected_variable_str
				: e_variable_is_not_writable_str;
	emsg_var_cl_define(پیام, m->ocm_name.string, 0, cl);
	بازگشت FALSE;
    }

    بازگشت TRUE;
}

/*
 * Initialize "lhs" with default values
 */
    ایستا تهی
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
    ایستا صحیح
lhs_find_var_end(
    lhs_T	*lhs,
    char_u	*var_start,
    صحیح		is_decl,
    char_u	**var_endp)
{
    char_u  *var_end = *var_endp;

    // "lhs_dest_end" is the end of the destination, including "[expr]" or
    // ".name".
    // "var_end" is the end of the variable/option/etc. name.
    lhs->lhs_dest_end = skip_var_one(var_start, FALSE);
    اگر (*var_start == '@')
    {
	اگر (!valid_dest_reg(var_start[1]))
	    بازگشت FAIL;
	var_end = var_start + 2;
    }
    وگرنه
    {
	// skip over the leading "&", "&l:", "&g:" and "$"
	var_end = skip_option_env_lead(var_start);
	var_end = to_name_end(var_end, TRUE);
    }

    // "a: type" is declaring variable "a" with a type, not dict "a:".
    اگر (is_decl && lhs->lhs_dest_end == var_start + 2
					&& lhs->lhs_dest_end[-1] == ':')
	--lhs->lhs_dest_end;
    اگر (is_decl && var_end == var_start + 2 && var_end[-1] == ':')
	--var_end;

    lhs->lhs_end = lhs->lhs_dest_end;
    *var_endp = var_end;

    بازگشت OK;
}

/*
 * Set various fields in "lhs"
 */
    ایستا صحیح
lhs_init(
    lhs_T	*lhs,
    char_u	*var_start,
    صحیح		is_decl,
    صحیح		heredoc,
    char_u	**var_endp)
{
    char_u *var_end = *var_endp;

    lhs_init_defaults(lhs);

    // Find the end of the variable and the destination
    اگر (lhs_find_var_end(lhs, var_start, is_decl, &var_end) == FAIL)
	بازگشت FAIL;

    // compute the length of the destination without "[expr]" or ".name"
    lhs->lhs_varlen = var_end - var_start;
    lhs->lhs_varlen_total = lhs->lhs_varlen;
    lhs->lhs_name = vim_strnsave(var_start, lhs->lhs_varlen);
    اگر (lhs->lhs_name == پوچ)
	بازگشت FAIL;

    اگر (lhs->lhs_dest_end > var_start + lhs->lhs_varlen)
	// Something follows after the variable: "var[idx]" or "var.key".
	lhs->lhs_has_index = TRUE;

    lhs->lhs_type = heredoc ? &t_list_string : &t_any;

    *var_endp = var_end;

    بازگشت OK;
}

/*
 * Compile a LHS class variable name.
 */
    ایستا صحیح
compile_lhs_class_variable(
    cctx_T	*cctx,
    lhs_T	*lhs,
    class_T	*defcl,
    صحیح		is_decl)
{
    اگر (cctx->ctx_ufunc->uf_defclass != defcl)
    {
	// A class variable can be accessed without the class name
	// only inside a class.
	semsg(_(e_class_variable_str_accessible_only_inside_class_str),
		lhs->lhs_name, defcl->class_name.string);
	بازگشت FAIL;
    }

    اگر (is_decl)
    {
	semsg(_(e_variable_already_declared_in_class_str), lhs->lhs_name);
	بازگشت FAIL;
    }

    ocmember_T	*m = &defcl->class_class_members[lhs->lhs_classmember_idx];
    اگر (oc_var_check_ro(defcl, m))
	بازگشت FAIL;

    lhs->lhs_dest = dest_class_member;
    // The class variable is defined either in the current class or
    // in one of the parent class in the hierarchy.
    lhs->lhs_class = defcl;
    lhs->lhs_type = oc_member_type_by_idx(defcl, FALSE,
						lhs->lhs_classmember_idx);

    بازگشت OK;
}

/*
 * Compile an imported LHS variable
 */
    ایستا صحیح
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
    اگر (dot == پوچ)
    {
	semsg(_(e_no_dot_after_imported_name_str), var_start);
	بازگشت FAIL;
    }

    p = skipwhite(dot + 1);
    var_end = to_name_end(p, TRUE);
    اگر (var_end == p)
    {
	semsg(_(e_missing_name_after_imported_name_str), var_start);
	بازگشت FAIL;
    }

    vim_free(lhs->lhs_name);
    lhs->lhs_varlen = var_end - p;
    lhs->lhs_name = vim_strnsave(p, lhs->lhs_varlen);
    اگر (lhs->lhs_name == پوچ)
	بازگشت FAIL;
    *rawnamep = lhs->lhs_name;
    lhs->lhs_scriptvar_sid = import->imp_sid;

    // TODO: where do we check this name is exported?

    // Check if something follows: "exp.var[idx]" or
    // "exp.var.key".
    lhs->lhs_has_index = lhs->lhs_dest_end > skipwhite(var_end);

    *var_endp = var_end;

    بازگشت OK;
}

/*
 * Process a script-local variable when compiling a LHS variable name.
 */
    ایستا صحیح
compile_lhs_script_var(
    cctx_T	*cctx,
    lhs_T	*lhs,
    char_u	*var_start,
    char_u	*var_end,
    صحیح		is_decl)
{
    صحیح		script_namespace = FALSE;
    صحیح		script_var = FALSE;
    imported_T	*import;
    char_u	*var_name;
    size_t	var_name_len;

    اگر (lhs->lhs_varlen > 1 && STRNCMP(var_start, "s:", 2) == 0)
	script_namespace = TRUE;

    اگر (script_namespace)
    {
	var_name = var_start + 2;
	var_name_len = lhs->lhs_varlen - 2;
    }
    وگرنه
    {
	var_name = var_start;
	var_name_len = lhs->lhs_varlen;
    }

    اگر (script_var_exists(var_name, var_name_len, cctx, پوچ) == OK)
	script_var = TRUE;

    import = find_imported(var_start, lhs->lhs_varlen, FALSE);

    اگر (script_namespace || script_var || import != پوچ)
    {
	char_u *rawname = lhs->lhs_name + (lhs->lhs_name[1] == ':' ? 2 : 0);

	اگر (script_namespace && current_script_is_vim9())
	{
	    semsg(_(e_cannot_use_s_colon_in_vim9_script_str), var_start);
	    بازگشت FAIL;
	}

	اگر (is_decl)
	{
	    اگر (script_namespace)
		semsg(_(e_cannot_declare_script_variable_in_function_str),
			lhs->lhs_name);
	    وگرنه
		semsg(_(e_variable_already_declared_in_script_str),
			lhs->lhs_name);
	    بازگشت FAIL;
	}
	وگرنه اگر (cctx->ctx_ufunc->uf_script_ctx_version == SCRIPT_VERSION_VIM9
		&& script_namespace
		&& !script_var && import == پوچ)
	{
	    semsg(_(e_unknown_variable_str), lhs->lhs_name);
	    بازگشت FAIL;
	}

	lhs->lhs_dest = current_script_is_vim9() ? dest_script_v9 :
								dest_script;

	// existing script-local variables should have a type
	lhs->lhs_scriptvar_sid = current_sctx.sc_sid;
	اگر (import != پوچ)
	{
	    اگر (compile_lhs_import_var(lhs, import, var_start, &var_end,
							&rawname) == FAIL)
		بازگشت FAIL;
	}

	اگر (SCRIPT_ID_VALID(lhs->lhs_scriptvar_sid))
	{
	    // Check writable only when no index follows.
	    lhs->lhs_scriptvar_idx = get_script_item_idx(
					lhs->lhs_scriptvar_sid, rawname,
					lhs->lhs_has_index ?  ASSIGN_FINAL :
					ASSIGN_CONST, cctx, پوچ);
	    اگر (lhs->lhs_scriptvar_idx >= 0)
	    {
		scriptitem_T *si = SCRIPT_ITEM(lhs->lhs_scriptvar_sid);
		svar_T	 *sv = ((svar_T *)si->sn_var_vals.ga_data)
						+ lhs->lhs_scriptvar_idx;

		lhs->lhs_type = sv->sv_type;
	    }
	}

	بازگشت OK;
    }

    بازگشت check_defined(var_start, lhs->lhs_varlen, cctx, پوچ, FALSE);
}

/*
 * Compile the LHS destination.
 */
    ایستا صحیح
compile_lhs_var_dest(
    cctx_T	*cctx,
    lhs_T	*lhs,
    صحیح		cmdidx,
    char_u	*var_start,
    char_u	*var_end,
    صحیح		is_decl,
    صحیح		has_cmd)	// "var" before "var_start"
{
    صحیح	    declare_error = FALSE;

    اگر (get_var_dest(lhs->lhs_name, &lhs->lhs_dest, cmdidx,
				lhs->lhs_has_index,
				&lhs->lhs_opt_flags, &lhs->lhs_vimvaridx,
				&lhs->lhs_type, cctx) == FAIL)
	بازگشت FAIL;

    اگر (lhs->lhs_dest != dest_local && cmdidx != CMD_const
						&& cmdidx != CMD_final)
    {
	// Specific kind of variable recognized.
	declare_error = is_decl;
    }
    وگرنه
    {
	class_T	*defcl;

	// No specific kind of variable recognized, just a name.
	اگر (check_reserved_name(lhs->lhs_name, lhs->lhs_has_index
						&& *var_end == '.') == FAIL)
	    بازگشت FAIL;

	اگر (lookup_local(var_start, lhs->lhs_varlen, &lhs->lhs_local_lvar,
								cctx) == OK)
	{
	    lhs->lhs_lvar = &lhs->lhs_local_lvar;
	}
	وگرنه
	{
	    CLEAR_FIELD(lhs->lhs_arg_lvar);
	    اگر (arg_exists(var_start, lhs->lhs_varlen,
			&lhs->lhs_arg_lvar.lv_idx, &lhs->lhs_arg_lvar.lv_type,
			&lhs->lhs_arg_lvar.lv_from_outer, cctx) == OK)
	    {
		اگر (is_decl)
		{
		    semsg(_(e_str_is_used_as_argument), lhs->lhs_name);
		    بازگشت FAIL;
		}
		lhs->lhs_lvar = &lhs->lhs_arg_lvar;
	    }
	}

	اگر (lhs->lhs_lvar != پوچ)
	{
	    اگر (is_decl)
	    {
		// if we come here with what looks like an assignment like
		// .= but which has been rejected by assignment_len() from
		// may_compile_assignment give a better error message
		char_u *p = skipwhite(lhs->lhs_end);
		اگر (p[0] == '.' && p[1] == '=')
		    emsg(_(e_dot_equal_not_supported_with_script_version_two));
		وگرنه اگر (p[0] == ':' && !has_cmd)
		    // type specified in an assignment without "var"
		    semsg(_(e_trailing_characters_str), p);
		وگرنه
		    semsg(_(e_variable_already_declared_str), lhs->lhs_name);
		بازگشت FAIL;
	    }
	}
	وگرنه اگر ((lhs->lhs_classmember_idx = cctx_class_member_idx(
			cctx, var_start, lhs->lhs_varlen, &defcl)) >= 0)
	{
	    اگر (compile_lhs_class_variable(cctx, lhs, defcl, is_decl)
		    == FAIL)
		بازگشت FAIL;
	}
	وگرنه
	{
	    اگر (compile_lhs_script_var(cctx, lhs, var_start, var_end,
			is_decl) == FAIL)
		بازگشت FAIL;
	}
    }

    اگر (declare_error)
    {
	vim9_declare_error(lhs->lhs_name);
	بازگشت FAIL;
    }

    بازگشت OK;
}

/*
 * When compiling a LHS variable name, for a class or an object, set the LHS
 * member type.
 */
    ایستا صحیح
compile_lhs_set_oc_member_type(
    cctx_T	*cctx,
    lhs_T	*lhs,
    char_u	*var_start)
{
    class_T	*cl = lhs->lhs_type->tt_class;
    صحیح		is_object = lhs->lhs_type->tt_type == VAR_OBJECT;
    char_u	*name = var_start + lhs->lhs_varlen + 1;
    size_t	namelen = lhs->lhs_end - var_start - lhs->lhs_varlen - 1;

    ocmember_T	*m = member_lookup(cl, lhs->lhs_type->tt_type,
	    name, namelen, &lhs->lhs_member_idx);
    اگر (m == پوچ)
    {
	member_not_found_پیام(cl, lhs->lhs_type->tt_type, name, namelen);
	بازگشت FAIL;
    }

    اگر (IS_ENUM(cl))
    {
	اگر (!inside_class(cctx, cl))
	{
	    semsg(_(e_enumvalue_str_cannot_be_modified),
		    cl->class_name.string, m->ocm_name.string);
	    بازگشت FAIL;
	}
	اگر (lhs->lhs_type->tt_type == VAR_OBJECT &&
		lhs->lhs_member_idx < 2)
	{
	    نویسه *پیام = lhs->lhs_member_idx == 0 ?
		e_enum_str_name_cannot_be_modified :
		e_enum_str_ordinal_cannot_be_modified;
	    semsg(_(پیام), cl->class_name.string);
	    بازگشت FAIL;
	}
    }

    // If it is private member variable, then accessing it outside the
    // class is not allowed.
    // If it is a read only class variable, then it can be modified
    // only inside the class where it is defined.
    اگر ((m->ocm_access != VIM_ACCESS_ALL) &&
	    ((is_object && !inside_class(cctx, cl))
	     || (!is_object && cctx->ctx_ufunc->uf_defclass != cl)))
    {
	نویسه *پیام = (m->ocm_access == VIM_ACCESS_PRIVATE)
	    ? e_cannot_access_protected_variable_str
	    : e_variable_is_not_writable_str;
	emsg_var_cl_define(پیام, m->ocm_name.string, 0, cl);
	بازگشت FAIL;
    }

    اگر (!IS_CONSTRUCTOR_METHOD(cctx->ctx_ufunc)
	    && oc_var_check_ro(cl, m))
	بازگشت FAIL;

    lhs->lhs_member_type = m->ocm_type;

    بازگشت OK;
}

/*
 * When compiling a LHS variable, set the LHS variable type.
 */
    ایستا صحیح
compile_lhs_set_type(cctx_T *cctx, lhs_T *lhs, char_u *var_end, صحیح is_decl)
{
    اگر (is_decl && *skipwhite(var_end) == ':')
    {
	char_u *p;

	// parse optional type: "let var: type = expr"
	اگر (VIM_ISWHITE(*var_end))
	{
	    semsg(_(e_no_white_space_allowed_before_colon_str), var_end);
	    بازگشت FAIL;
	}

	اگر (!VIM_ISWHITE(var_end[1]))
	{
	    semsg(_(e_white_space_required_after_str_str), ":", var_end);
	    بازگشت FAIL;
	}

	p = skipwhite(var_end + 1);
	lhs->lhs_type = parse_type(&p, cctx->ctx_type_list, cctx->ctx_ufunc, cctx, TRUE);
	اگر (lhs->lhs_type == پوچ
		|| !valid_declaration_type(lhs->lhs_type))
	    بازگشت FAIL;

	lhs->lhs_has_type = TRUE;
	lhs->lhs_end = p;
    }
    وگرنه اگر (lhs->lhs_lvar != پوچ)
	lhs->lhs_type = lhs->lhs_lvar->lv_type;

    بازگشت OK;
}

/*
 * Returns TRUE if "lhs" is a concatenable string.
 */
    ایستا صحیح
lhs_concatenable(lhs_T *lhs)
{
    بازگشت lhs->lhs_dest == dest_global
		|| lhs->lhs_has_index
		|| lhs->lhs_type->tt_type == VAR_STRING
		|| lhs->lhs_type->tt_type == VAR_ANY;
}

/*
 * Create a new local variable when compiling a LHS variable.
 */
    ایستا صحیح
compile_lhs_new_local_var(
    cctx_T	*cctx,
    lhs_T	*lhs,
    char_u	*var_start,
    صحیح		cmdidx,
    صحیح		oplen,
    صحیح		is_decl,
    صحیح		has_cmd,
    صحیح		heredoc)
{
    اگر (oplen > 1 && !heredoc)
    {
	// +=, /=, etc. require an existing variable
	semsg(_(e_cannot_use_operator_on_new_variable_str), lhs->lhs_name);
	بازگشت FAIL;
    }

    اگر (!is_decl || (lhs->lhs_has_index && !has_cmd
					&& cctx->ctx_skip != SKIP_YES))
    {
	semsg(_(e_unknown_variable_str), lhs->lhs_name);
	بازگشت FAIL;
    }

    // Check the name is valid for a funcref.
    اگر (lhs->lhs_type->tt_type == VAR_FUNC
				|| lhs->lhs_type->tt_type == VAR_PARTIAL)
    {
	اگر (var_wrong_func_name(lhs->lhs_name, TRUE))
	    بازگشت FAIL;
    }

    // New local variable.
    صحیح assign;
    انتخاب (cmdidx)
    {
	حالت CMD_final:
	    assign = ASSIGN_FINAL; بشکن;
	حالت CMD_const:
	    assign = ASSIGN_CONST; بشکن;
	پیشفرض:
	    assign = ASSIGN_VAR; بشکن;
    }

    lhs->lhs_lvar = reserve_local(cctx, var_start, lhs->lhs_varlen, assign,
							lhs->lhs_type);
    اگر (lhs->lhs_lvar == پوچ)
	بازگشت FAIL;

    lhs->lhs_new_local = TRUE;

    بازگشت OK;
}

/*
 * When compiling a LHS variable name, set the LHS member type.
 */
    ایستا صحیح
compile_lhs_set_member_type(
    cctx_T	*cctx,
    lhs_T	*lhs,
    char_u	*var_start,
    صحیح		is_decl,
    صحیح		has_cmd)
{
    lhs->lhs_member_type = lhs->lhs_type;

    اگر (!lhs->lhs_has_index)
	بازگشت OK;

    char_u	*after = var_start + lhs->lhs_varlen;
    char_u	*p;

    // Something follows after the variable: "var[idx]" or "var.key".
    اگر (is_decl && cctx->ctx_skip != SKIP_YES)
    {
	اگر (has_cmd)
	    emsg(_(e_cannot_use_index_when_declaring_variable));
	وگرنه
	    semsg(_(e_unknown_variable_str), lhs->lhs_name);
	بازگشت FAIL;
    }

    // Now: var_start[lhs->lhs_varlen] is '[' or '.'
    // Only the last index is used below, if there are others
    // before it generate code for the expression.  Thus for
    // "ll[1][2]" the expression is "ll[1]" and "[2]" is the index.
    برای (;;)
    {
	p = skip_index(after);
	اگر (*p != '[' && *p != '.')
	{
	    lhs->lhs_varlen_total = p - var_start;
	    بشکن;
	}
	after = p;
    }
    اگر (after > var_start + lhs->lhs_varlen)
    {
	lhs->lhs_varlen = after - var_start;
	lhs->lhs_dest = dest_عبارت;
	// We don't know the type before evaluating the expression,
	// use "any" until then.  The member index is for the first name,
	// not for the last index.
	lhs->lhs_type = &t_any;
	lhs->lhs_member_idx = -1;
    }

    صحیح use_class = lhs->lhs_type != پوچ
	&& (lhs->lhs_type->tt_type == VAR_CLASS
		|| lhs->lhs_type->tt_type == VAR_OBJECT);

    اگر (lhs->lhs_type == پوچ
	    || (use_class ? lhs->lhs_type->tt_class == پوچ
		: lhs->lhs_type->tt_member == پوچ))
    {
	lhs->lhs_member_type = &t_any;
    }
    وگرنه اگر (use_class)
    {
	// for an object or class member get the type of the member
	اگر (compile_lhs_set_oc_member_type(cctx, lhs, var_start) == FAIL)
	    بازگشت FAIL;
    }
    وگرنه
	lhs->lhs_member_type = lhs->lhs_type->tt_member;

    بازگشت OK;
}

/*
 * Figure out the LHS type and other properties for an assignment or one item
 * of ":unlet" with an index.
 * Returns OK or FAIL.
 */
    صحیح
compile_lhs(
	char_u	    *var_start,
	lhs_T	    *lhs,
	cmdidx_T    cmdidx,
	صحیح	    heredoc,
	صحیح	    has_cmd,	    // "var" before "var_start"
	صحیح	    oplen,
	cctx_T	    *cctx)
{
    char_u	*var_end = پوچ;
    صحیح		is_decl = is_decl_command(cmdidx);

    اگر (lhs_init(lhs, var_start, is_decl, heredoc, &var_end) == FAIL)
	بازگشت FAIL;

    اگر (cctx->ctx_skip != SKIP_YES)
    {
	// compile the LHS destination
	اگر (compile_lhs_var_dest(cctx, lhs, cmdidx, var_start, var_end,
						is_decl, has_cmd) == FAIL)
	    بازگشت FAIL;
    }

    // handle "a:name" as a name, not index "name" in "a"
    اگر (lhs->lhs_varlen > 1 || var_start[lhs->lhs_varlen] != ':')
	var_end = lhs->lhs_dest_end;

    اگر (lhs->lhs_dest != dest_option && lhs->lhs_dest != dest_func_option)
    {
	// set the LHS variable type
	اگر (compile_lhs_set_type(cctx, lhs, var_end, is_decl) == FAIL)
	    بازگشت FAIL;
    }

    اگر (oplen == 3 && !heredoc && !lhs_concatenable(lhs))
    {
	emsg(_(e_can_only_concatenate_to_string));
	بازگشت FAIL;
    }

    اگر (lhs->lhs_lvar == پوچ && lhs->lhs_dest == dest_local
						&& cctx->ctx_skip != SKIP_YES)
    {
	اگر (compile_lhs_new_local_var(cctx, lhs, var_start, cmdidx, oplen,
					is_decl, has_cmd, heredoc) == FAIL)
	    بازگشت FAIL;
    }

    اگر (compile_lhs_set_member_type(cctx, lhs, var_start, is_decl, has_cmd)
								== FAIL)
	بازگشت FAIL;

    بازگشت OK;
}

/*
 * Figure out the LHS and check a few errors.
 */
    صحیح
compile_assign_lhs(
	char_u	    *var_start,
	lhs_T	    *lhs,
	cmdidx_T    cmdidx,
	صحیح	    is_decl,
	صحیح	    heredoc,
	صحیح	    has_cmd,	    // "var" before "var_start"
	صحیح	    oplen,
	cctx_T	    *cctx)
{
    اگر (compile_lhs(var_start, lhs, cmdidx, heredoc, has_cmd, oplen, cctx)
								       == FAIL)
	بازگشت FAIL;

    اگر (!lhs->lhs_has_index && lhs->lhs_lvar == &lhs->lhs_arg_lvar)
    {
	semsg(_(e_cannot_assign_to_argument_str), lhs->lhs_name);
	بازگشت FAIL;
    }
    اگر (!is_decl && lhs->lhs_lvar != پوچ
			   && lhs->lhs_lvar->lv_const != ASSIGN_VAR
			   && !lhs->lhs_has_index)
    {
	semsg(_(e_cannot_assign_to_constant_str), lhs->lhs_name);
	بازگشت FAIL;
    }
    بازگشت OK;
}

/*
 * Return TRUE if "lhs" has a range index: "[expr : expr]".
 */
    ایستا صحیح
has_list_index(char_u *idx_start, cctx_T *cctx)
{
    char_u  *p = idx_start;
    صحیح	    save_skip;

    اگر (*p != '[')
	بازگشت FALSE;

    p = skipwhite(p + 1);
    اگر (*p == ':')
	بازگشت TRUE;

    save_skip = cctx->ctx_skip;
    cctx->ctx_skip = SKIP_YES;
    (تهی)compile_expr0(&p, cctx);
    cctx->ctx_skip = save_skip;
    بازگشت *skipwhite(p) == ':';
}

/*
 * For an assignment with an index, compile the "idx" in "var[idx]" or "key" in
 * "var.key".
 */
    ایستا صحیح
compile_assign_index(
	char_u	*var_start,
	lhs_T	*lhs,
	صحیح	*range,
	cctx_T	*cctx)
{
    size_t	varlen = lhs->lhs_varlen;
    char_u	*p;
    صحیح		r = OK;
    صحیح		need_white_before = TRUE;
    صحیح		empty_second;

    p = var_start + varlen;
    اگر (*p == '[')
    {
	p = skipwhite(p + 1);
	اگر (*p == ':')
	{
	    // empty first index, push zero
	    r = generate_PUSHNR(cctx, 0);
	    need_white_before = FALSE;
	}
	وگرنه
	    r = compile_expr0(&p, cctx);

	اگر (r == OK && *skipwhite(p) == ':')
	{
	    // unlet var[idx : idx]
	    // blob[idx : idx] = value
	    *range = TRUE;
	    p = skipwhite(p);
	    empty_second = *skipwhite(p + 1) == ']';
	    اگر ((need_white_before && !IS_WHITE_OR_NUL(p[-1]))
		    || (!empty_second && !IS_WHITE_OR_NUL(p[1])))
	    {
		semsg(_(e_white_space_required_before_and_after_str_at_str),
								      ":", p);
		بازگشت FAIL;
	    }
	    p = skipwhite(p + 1);
	    اگر (*p == ']')
		// empty second index, push "none"
		r = generate_PUSHSPEC(cctx, VVAL_NONE);
	    وگرنه
		r = compile_expr0(&p, cctx);
	}

	اگر (r == OK && *skipwhite(p) != ']')
	{
	    // this should not happen
	    emsg(_(e_missing_closing_square_brace));
	    r = FAIL;
	}
    }
    وگرنه اگر (lhs->lhs_member_idx >= 0)
    {
	// object member index
	r = generate_PUSHNR(cctx, lhs->lhs_member_idx);
    }
    وگرنه // if (*p == '.')
    {
	char_u *کلید_end = to_name_end(p + 1, TRUE);
	char_u *کلید = vim_strnsave(p + 1, کلید_end - p - 1);

	r = generate_PUSHS(cctx, &کلید);
    }
    بازگشت r;
}

/*
 * For a LHS with an index, load the variable to be indexed.
 */
    ایستا صحیح
compile_load_lhs(
	lhs_T	*lhs,
	char_u	*var_start,
	type_T	*rhs_type,
	cctx_T	*cctx)
{
    اگر (lhs->lhs_dest == dest_عبارت)
    {
	size_t	    varlen = lhs->lhs_varlen;
	صحیح	    c = var_start[varlen];
	صحیح	    سطرها_len = cctx->ctx_ufunc->uf_سطرها.ga_len;
	صحیح	    res;

	// Evaluate "ll[expr]" of "ll[expr][idx]".  End the line with a NUL and
	// limit the lines array length to avoid skipping to a following line.
	var_start[varlen] = NUL;
	cctx->ctx_ufunc->uf_سطرها.ga_len = cctx->ctx_lnum + 1;
	char_u *p = var_start;
	res = compile_expr0(&p, cctx);
	var_start[varlen] = c;
	cctx->ctx_ufunc->uf_سطرها.ga_len = سطرها_len;
	اگر (res == FAIL || p != var_start + varlen)
	{
	    // this should not happen
	    اگر (res != FAIL)
		emsg(_(e_missing_closing_square_brace));
	    بازگشت FAIL;
	}

	lhs->lhs_type = cctx->ctx_type_پشته.ga_len == 0 ? &t_void
						  : get_type_on_پشته(cctx, 0);

	اگر (lhs->lhs_type->tt_type == VAR_CLASS
		|| (lhs->lhs_type->tt_type == VAR_OBJECT
		    && lhs->lhs_type != &t_object_any))
	{
	    // Check whether the class or object variable is modifiable
	    اگر (!lhs_class_member_modifiable(lhs, var_start, cctx))
		بازگشت FAIL;
	}

	// Now we can properly check the type.  The variable is indexed, thus
	// we need the member type.  For a class or object we don't know the
	// type yet, it depends on what member is used.
	// The top item in the stack is the Dict, followed by the key and then
	// the type of the value.
	vartype_T vartype = lhs->lhs_type->tt_type;
	type_T *member_type = lhs->lhs_type->tt_member;
	اگر (rhs_type != پوچ && member_type != پوچ
		&& vartype != VAR_OBJECT && vartype != VAR_CLASS
		&& rhs_type != &t_void
		&& need_type(rhs_type, member_type, 0,
					    -3, 0, cctx, FALSE, FALSE) == FAIL)
	    بازگشت FAIL;

	بازگشت OK;
    }

    بازگشت generate_loadvar(cctx, lhs);
}

/*
 * Produce code for loading "lhs" and also take care of an index.
 * Return OK/FAIL.
 */
    صحیح
compile_load_lhs_with_index(lhs_T *lhs, char_u *var_start, cctx_T *cctx)
{
    اگر (lhs->lhs_type->tt_type == VAR_OBJECT)
    {
	// "this.value": load "this" object and get the value at index for an
	// object or class member get the type of the member.
	// Also for "obj.value".
	char_u *dot = vim_strchr(var_start, '.');
	اگر (dot == پوچ)
	{
	    semsg(_(e_missing_dot_after_object_str), lhs->lhs_name);
	    بازگشت FAIL;
	}

	class_T	*cl = lhs->lhs_type->tt_class;
	type_T	*type = oc_member_type(cl, TRUE, dot + 1,
					  lhs->lhs_end, &lhs->lhs_member_idx);
	اگر (lhs->lhs_member_idx < 0)
	    بازگشت FAIL;

	اگر (dot - var_start == 4 && STRNCMP(var_start, "this", 4) == 0)
	{
	    // load "this"
	    lvar_T  *lvar = lhs->lhs_lvar;
	    صحیح	    rc;

	    اگر (lvar->lv_from_outer > 0)
		rc = generate_LOADOUTER(cctx, lvar->lv_idx,
			lvar->lv_from_outer, lvar->lv_loop_depth,
			lvar->lv_loop_idx, type);
	    وگرنه
		rc = generate_LOAD(cctx, ISN_LOAD, lvar->lv_idx, پوچ, type);

	    اگر (rc == FAIL)
		بازگشت FAIL;
	}
	وگرنه
	{
	    // load object variable or argument
	    اگر (compile_load_lhs(lhs, var_start, lhs->lhs_type, cctx) == FAIL)
		بازگشت FAIL;
	}
	اگر (IS_INTERFACE(cl))
	    بازگشت generate_GET_ITF_MEMBER(cctx, cl, lhs->lhs_member_idx, type);
	بازگشت generate_GET_OBJ_MEMBER(cctx, lhs->lhs_member_idx, type);
    }
    وگرنه اگر (lhs->lhs_type->tt_type == VAR_CLASS)
    {
	// "<classname>.value": load class variable "classname.value"
	char_u *dot = vim_strchr(var_start, '.');
	اگر (dot == پوچ)
	{
	    (تهی)check_type_is_مقدار(lhs->lhs_type);
	    بازگشت FAIL;
	}

	class_T	*cl = lhs->lhs_type->tt_class;
	ocmember_T *m = class_member_lookup(cl, dot + 1,
						lhs->lhs_end - dot - 1,
						&lhs->lhs_member_idx);
	اگر (m == پوچ)
	    بازگشت FAIL;

	بازگشت generate_CLASSMEMBER(cctx, TRUE, cl, lhs->lhs_member_idx);
    }

    اگر (compile_load_lhs(lhs, var_start, پوچ, cctx) == FAIL)
	بازگشت FAIL;

    اگر (lhs->lhs_has_index)
    {
	صحیح	range = FALSE;
	type_T	*type = get_type_on_پشته(cctx, 0);

	// A member of an object or class is not obtained by indexing it.
	اگر (type->tt_type == VAR_CLASS
		|| (type->tt_type == VAR_OBJECT && type != &t_object_any))
	{
	    char_u *p = var_start + lhs->lhs_varlen;

	    بازگشت compile_class_object_index(cctx, &p, type);
	}

	// Get member from list or dict.  First compile the
	// index value.
	اگر (compile_assign_index(var_start, lhs, &range, cctx) == FAIL)
	    بازگشت FAIL;
	اگر (range)
	{
	    semsg(_(e_cannot_use_range_with_assignment_operator_str),
								    var_start);
	    بازگشت FAIL;
	}

	// Get the member.
	اگر (compile_member(FALSE, پوچ, cctx) == FAIL)
	    بازگشت FAIL;
    }
    بازگشت OK;
}

/*
 * Assignment to a list or dict member, or ":unlet" for the item, using the
 * information in "lhs".
 * Returns OK or FAIL.
 */
    صحیح
compile_assign_unlet(
    char_u	*var_start,
    lhs_T	*lhs,
    صحیح		is_assign,
    type_T	*rhs_type,
    cctx_T	*cctx)
{
    vartype_T	dest_type;
    صحیح		range = FALSE;

    اگر (compile_assign_index(var_start, lhs, &range, cctx) == FAIL)
	بازگشت FAIL;
    اگر (is_assign && range
	    && lhs->lhs_type->tt_type != VAR_LIST
	    && lhs->lhs_type != &t_blob
	    && lhs->lhs_type != &t_any)
    {
	اگر (lhs->lhs_type->tt_type == VAR_TUPLE)
	    emsg(_(e_cannot_slice_tuple));
	وگرنه
	    semsg(_(e_cannot_use_range_with_assignment_str), var_start);
	بازگشت FAIL;
    }

    // For "expr[idx]" the index is compiled before the expression, thus the
    // resulting type cannot be used here.
    اگر (lhs->lhs_dest == dest_عبارت
	    || lhs->lhs_type == پوچ || lhs->lhs_type == &t_any)
    {
	// Index on variable of unknown type: check at runtime.
	dest_type = VAR_ANY;
    }
    وگرنه
    {
	dest_type = lhs->lhs_type->tt_type;
	اگر (dest_type == VAR_DICT && range)
	{
	    emsg(_(e_cannot_use_range_with_dictionary));
	    بازگشت FAIL;
	}
	اگر (dest_type == VAR_DICT
		&& may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
	    بازگشت FAIL;
	اگر (dest_type == VAR_LIST || dest_type == VAR_BLOB)
	{
	    type_T *type;

	    اگر (range)
	    {
		type = get_type_on_پشته(cctx, 1);
		اگر (need_type(type, &t_عدد, 0,
					    -2, 0, cctx, FALSE, FALSE) == FAIL)
		بازگشت FAIL;
	    }
	    type = get_type_on_پشته(cctx, 0);
	    اگر ((dest_type != VAR_BLOB && type->tt_type != VAR_SPECIAL)
		    && need_type(type, &t_عدد, 0,
					    -1, 0, cctx, FALSE, FALSE) == FAIL)
		بازگشت FAIL;
	}
    }

    اگر (cctx->ctx_skip == SKIP_YES)
	بازگشت OK;

    // Load the dict, list or object.  On the stack we then have:
    // - value (for assignment, not for :unlet)
    // - index
    // - for [a : b] second index
    // - variable
    اگر (compile_load_lhs(lhs, var_start, rhs_type, cctx) == FAIL)
	بازگشت FAIL;

    اگر (dest_type == VAR_LIST
	    || dest_type == VAR_DICT
	    || dest_type == VAR_BLOB
	    || dest_type == VAR_CLASS
	    || dest_type == VAR_OBJECT
	    || dest_type == VAR_ANY)
    {
	اگر (is_assign)
	{
	    اگر (range)
	    {
		اگر (generate_instr_drop(cctx, ISN_STORERANGE, 4) == پوچ)
		    بازگشت FAIL;
	    }
	    وگرنه
	    {
		isn_T	*isn = generate_instr_drop(cctx, ISN_STOREINDEX, 3);

		اگر (isn == پوچ)
		    بازگشت FAIL;
		isn->isn_arg.storeindex.si_vartype = dest_type;
		isn->isn_arg.storeindex.si_class = پوچ;

		اگر (dest_type == VAR_OBJECT)
		{
		    class_T *cl = lhs->lhs_type->tt_class;

		    اگر (IS_INTERFACE(cl))
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
	وگرنه اگر (range)
	{
	    اگر (generate_instr_drop(cctx, ISN_UNLETRANGE, 3) == پوچ)
		بازگشت FAIL;
	}
	وگرنه
	{
	    اگر (generate_instr_drop(cctx, ISN_UNLETINDEX, 2) == پوچ)
		بازگشت FAIL;
	}
    }
    وگرنه
    {
	اگر (dest_type == VAR_TUPLE)
	    emsg(_(e_tuple_is_immutable));
	وگرنه
	    emsg(_(e_indexable_type_required));
	بازگشت FAIL;
    }

    بازگشت OK;
}

/*
 * Generate an instruction to push the default value for "vartype".
 * if "dest_local" is TRUE then for some types no instruction is generated.
 * "skip_store" is set to TRUE if no PUSH instruction is generated.
 * Returns OK or FAIL.
 */
    ایستا صحیح
افزودن_default_مقدار(
	cctx_T	    *cctx,
	vartype_T   vartype,
	صحیح	    dest_is_local,
	صحیح	    *skip_store)
{
    صحیح r = OK;

    انتخاب (vartype)
    {
	حالت VAR_BOOL:
	    r = generate_PUSHBOOL(cctx, VVAL_FALSE);
	    بشکن;
	حالت VAR_FLOAT:
	    r = generate_PUSHF(cctx, 0.0);
	    بشکن;
	حالت VAR_STRING:
	    r = generate_PUSHS(cctx, پوچ);
	    بشکن;
	حالت VAR_BLOB:
	    r = generate_PUSHBLOB(cctx, blob_alloc());
	    بشکن;
	حالت VAR_FUNC:
	    r = generate_PUSHFUNC(cctx, پوچ, &t_func_void, TRUE);
	    بشکن;
	حالت VAR_LIST:
	    r = generate_NEWLIST(cctx, 0, FALSE);
	    بشکن;
	حالت VAR_TUPLE:
	    r = generate_NEWTUPLE(cctx, 0, FALSE);
	    بشکن;
	حالت VAR_DICT:
	    r = generate_NEWDICT(cctx, 0, FALSE);
	    بشکن;
	حالت VAR_JOB:
	    r = generate_PUSHJOB(cctx);
	    بشکن;
	حالت VAR_CHANNEL:
	    r = generate_PUSHCHANNEL(cctx);
	    بشکن;
	حالت VAR_OBJECT:
	    r = generate_PUSHOBJ(cctx);
	    بشکن;
	حالت VAR_عدد:
	حالت VAR_UNKNOWN:
	حالت VAR_ANY:
	حالت VAR_PARTIAL:
	حالت VAR_VOID:
	حالت VAR_INSTR:
	حالت VAR_CLASS:
	حالت VAR_TYPEALIAS:
	حالت VAR_SPECIAL:  // cannot happen
	    // This is skipped for local variables, they are always
	    // initialized to zero.  But in a "for" or "while" loop
	    // the value may have been changed.
	    اگر (dest_is_local && !inside_loop_scope(cctx))
		*skip_store = TRUE;
	    وگرنه
		r = generate_PUSHNR(cctx, 0);
	    بشکن;
    }
    بازگشت r;
}

/*
 * Compile assignment context.  Used when compiling an assignment statement.
 */
تعریفنوع ساختار cac_S cac_T;
ساختار cac_S
{
    cmdidx_T	cac_cmdidx;		// assignment command
    char_u	*cac_nextc;		// next character to parse
    lhs_T	cac_lhs;		// lhs of the assignment
    type_T	*cac_rhs_type;		// rhs type of an assignment
    char_u	*cac_op;		// assignment operator
    صحیح		cac_oplen;		// assignment operator length
    char_u	*cac_var_start;		// start of the variable names
    char_u	*cac_var_end;		// end of the variable names
    صحیح		cac_var_شمار;		// number of variables in assignment
    صحیح		cac_var_idx;		// variable index in a list
    صحیح		cac_semicolon;		// semicolon in [var1, var2; var3]
    garray_T	*cac_instr;
    صحیح		cac_instr_شمار;
    صحیح		cac_incdec;
    صحیح		cac_did_generate_slice;
    صحیح		cac_is_decl;
    صحیح		cac_is_const;
    صحیح		cac_start_lnum;
    type_T	*cac_inferred_type;
    صحیح		cac_skip_store;
};

/*
 * Initialize the compile assignment context.
 */
    ایستا تهی
compile_assign_context_init(cac_T *cac, cctx_T *cctx, صحیح cmdidx, char_u *arg)
{
    CLEAR_FIELD(*cac);
    cac->cac_cmdidx = cmdidx;
    cac->cac_instr = &cctx->ctx_instr;
    cac->cac_rhs_type = &t_any;
    cac->cac_is_decl = is_decl_command(cmdidx);
    cac->cac_start_lnum = SOURCING_LNUM;
    cac->cac_instr_شمار = -1;
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
    ایستا صحیح
compile_assign_obj_new_arg(char_u **argp, cctx_T *cctx)
{
    char_u *arg = *argp;

    arg += 11;	    // skip "ifargisset"
    صحیح def_arg_idx = getdigits(&arg);
    arg = skipwhite(arg);

    // Use a JUMP_IF_ARG_NOT_SET instruction to skip if the value was not
    // given and the default value is "v:none".
    صحیح پشته_offset = پشته_FRAME_SIZE +
				(cctx->ctx_ufunc->uf_va_name != پوچ ? 1 : 0);
    صحیح def_arg_شمار = cctx->ctx_ufunc->uf_def_args.ga_len;
    صحیح arg_offset = def_arg_idx - def_arg_شمار - پشته_offset;

    اگر (generate_JUMP_IF_ARG(cctx, ISN_JUMP_IF_ARG_NOT_SET,
			     arg_offset) == FAIL)
	بازگشت FAIL;

    *argp = arg;
    بازگشت OK;
}

/*
 * Translate the increment (++) and decrement (--) operators to the
 * corresponding compound operators (+= or -=).
 *
 * Returns OK on success and FAIL on syntax error.
 */
    ایستا صحیح
translate_incdec_op(exarg_T *eap, cac_T *cac)
{
    اگر (VIM_ISWHITE(eap->cmd[2]))
    {
	semsg(_(e_no_white_space_allowed_after_str_str),
		eap->cmdidx == CMD_increment ? "++" : "--", eap->cmd);
	بازگشت FAIL;
    }
    cac->cac_op = (char_u *)(eap->cmdidx == CMD_increment ? "+=" : "-=");
    cac->cac_oplen = 2;
    cac->cac_incdec = TRUE;

    بازگشت OK;
}

/*
 * Process the operator in an assignment statement.
 */
    ایستا صحیح
compile_assign_process_operator(
    exarg_T	*eap,
    char_u	*arg,
    cac_T	*cac,
    صحیح		*heredoc,
    char_u	**retstr)
{
    *retstr = پوچ;

    اگر (eap->cmdidx == CMD_increment || eap->cmdidx == CMD_decrement)
	// Change an unary operator to a compound operator
	بازگشت translate_incdec_op(eap, cac);

    char_u *sp = cac->cac_nextc;
    cac->cac_nextc = skipwhite(cac->cac_nextc);
    cac->cac_op = cac->cac_nextc;
    cac->cac_oplen = assignment_len(cac->cac_nextc, heredoc);

    اگر (cac->cac_var_شمار > 0 && cac->cac_oplen == 0)
    {
	// can be something like "[1, 2]->func()"
	*retstr = arg;
	بازگشت FAIL;
    }

    // need white space before and after the operator
    اگر (cac->cac_oplen > 0 && (!VIM_ISWHITE(*sp)
		|| !IS_WHITE_OR_NUL(cac->cac_op[cac->cac_oplen])))
    {
	error_white_both(cac->cac_op, cac->cac_oplen);
	بازگشت FAIL;
    }

    بازگشت OK;
}

/*
 * Find the start of an assignment statement.
 */
    ایستا char_u *
compile_assign_compute_start(char_u *arg, صحیح var_شمار)
{
    اگر (var_شمار > 0)
	// [var1, var2] = [val1, val2]
	// skip over the "["
	بازگشت skipwhite(arg + 1);

    بازگشت arg;
}

/*
 * Parse a heredoc assignment starting at "p".  Returns a pointer to the
 * beginning of the heredoc content.
 */
    ایستا char_u *
parse_heredoc_assignment(exarg_T *eap, cctx_T *cctx, cac_T *cac)
{
    // [let] varname =<< [trim] {end}
    eap->ea_getline = exarg_getline;
    eap->cookie = cctx;

    list_T *l = heredoc_get(eap, cac->cac_nextc + 3, FALSE, TRUE);
    اگر (l == پوچ)
	بازگشت پوچ;

    list_free(l);
    cac->cac_nextc += STRLEN(cac->cac_nextc);

    بازگشت cac->cac_nextc;
}

/*
 * Check the type of a RHS expression in a list assignment statement.
 * The RHS expression is already compiled.  So the type is on the stack.
 */
    ایستا صحیح
compile_assign_list_check_rhs_type(cctx_T *cctx, cac_T *cac)
{
    type_T	*stacktype;

    stacktype = cctx->ctx_type_پشته.ga_len == 0 ? &t_void
						: get_type_on_پشته(cctx, 0);
    اگر (stacktype->tt_type == VAR_VOID)
    {
	emsg(_(e_cannot_use_void_مقدار));
	بازگشت FAIL;
    }

    اگر (stacktype->tt_type != VAR_LIST && stacktype->tt_type != VAR_TUPLE
					&& stacktype->tt_type != VAR_ANY)
    {
	emsg(_(e_list_or_tuple_required));
	بازگشت FAIL;
    }

    اگر (need_type(stacktype,
		  stacktype->tt_type == VAR_TUPLE ? &t_tuple_any : &t_list_any,
		  TYPECHK_TUPLE_OK, -1, 0, cctx, FALSE, FALSE) == FAIL)
	بازگشت FAIL;

    // The check accepts both a list and a tuple. Keep "any", making it a list
    // would reject a tuple later on.
    اگر (stacktype->tt_type == VAR_ANY)
	set_type_on_پشته(cctx, &t_any, 0);

    اگر (stacktype->tt_type == VAR_TUPLE)
    {
	اگر (stacktype->tt_argcount != 1)
	    cac->cac_rhs_type = &t_any;
	وگرنه
	{
	    اگر (stacktype->tt_flags & TTFLAG_VARARGS)
		cac->cac_rhs_type = stacktype->tt_args[0]->tt_member;
	    وگرنه
		cac->cac_rhs_type = stacktype->tt_args[0];
	}
    }
    وگرنه اگر (stacktype->tt_member != پوچ)
	cac->cac_rhs_type = stacktype->tt_member;

    بازگشت OK;
}

/*
 * In a list assignment statement, if a constant list was used, check the
 * length.  Returns OK if the length check succeeds.  Returns FAIL otherwise.
 */
    ایستا صحیح
compile_assign_list_check_length(cctx_T *cctx, cac_T *cac)
{
    صحیح	needed_list_len;
    صحیح	did_check = FALSE;

    needed_list_len = cac->cac_semicolon
				? cac->cac_var_شمار - 1
				: cac->cac_var_شمار;
    اگر (cac->cac_instr->ga_len > 0)
    {
	isn_T	*isn = ((isn_T *)cac->cac_instr->ga_data) +
						cac->cac_instr->ga_len - 1;

	اگر (isn->isn_type == ISN_NEWLIST || isn->isn_type == ISN_NEWTUPLE)
	{
	    did_check = TRUE;
	    اگر (cac->cac_semicolon ?
			isn->isn_arg.عدد < needed_list_len
			: isn->isn_arg.عدد != needed_list_len)
	    {
		semsg(_(e_expected_nr_اقلام_but_got_nr),
			needed_list_len, (صحیح)isn->isn_arg.عدد);
		بازگشت FAIL;
	    }
	}
    }

    اگر (!did_check)
	generate_CHECKLEN(cctx, needed_list_len, cac->cac_semicolon);

    بازگشت OK;
}

/*
 * Evaluate the expression for "[var, var] = expr" assignment.
 * A line break may follow the assignment operator "=".
 */
    ایستا char_u *
compile_assign_list_عبارت(cctx_T *cctx, cac_T *cac)
{
    char_u *whitep;

    whitep = cac->cac_op + cac->cac_oplen;

    اگر (may_get_next_سطر_error(whitep, &cac->cac_nextc, cctx) == FAIL)
	بازگشت پوچ;

    // compile RHS expression
    اگر (compile_expr0(&cac->cac_nextc, cctx) == FAIL)
	بازگشت پوچ;

    اگر (cctx->ctx_skip == SKIP_YES)
	// no need to parse more when skipping
	بازگشت cac->cac_nextc;

    اگر (compile_assign_list_check_rhs_type(cctx, cac) == FAIL)
	بازگشت پوچ;

    // If a constant list was used we can check the length right here.
    اگر (compile_assign_list_check_length(cctx, cac) == FAIL)
	بازگشت FAIL;

    بازگشت cac->cac_nextc;
}

/*
 * Find and return the end of a heredoc or a list of variables assignment
 * statement.  For a single variable assignment statement, returns the current
 * end.
 * Returns NULL on failure.
 */
    ایستا char_u *
compile_assign_compute_end(
    exarg_T	*eap,
    cctx_T	*cctx,
    cac_T	*cac,
    صحیح		heredoc)
{
    اگر (heredoc)
    {
	cac->cac_nextc = parse_heredoc_assignment(eap, cctx, cac);
	بازگشت cac->cac_nextc;
    }

    اگر (cac->cac_var_شمار > 0)
    {
	// for "[var, var] = expr" evaluate the expression. The list of
	// variables are processed later.
	// A line break may follow the "=".
	cac->cac_nextc = compile_assign_list_عبارت(cctx, cac);
	بازگشت cac->cac_nextc;
    }

    بازگشت cac->cac_var_end;
}

/*
 * For "var = expr" evaluate the expression.
 */
    ایستا صحیح
compile_assign_single_eval_عبارت(cctx_T *cctx, cac_T *cac)
{
    صحیح		ret = OK;
    char_u	*whitep;
    lhs_T	*lhs = &cac->cac_lhs;
    lvar_T	*lvp;
    lvar_T	save_lhs_lvar;

    // Compile the expression.
    اگر (cac->cac_incdec)
	بازگشت generate_PUSHNR(cctx, 1);

    // Temporarily hide the new local variable here, it is
    // not available to this expression.
    اگر (lhs->lhs_new_local)
    {
	--cctx->ctx_locals.ga_len;

	// Save the local variable value (compiling the RHS expression may
	// create new local variables).
	lvp = ((lvar_T *)cctx->ctx_locals.ga_data) + cctx->ctx_locals.ga_len;
	save_lhs_lvar = *lvp;
    }
    whitep = cac->cac_op + cac->cac_oplen;

    اگر (may_get_next_سطر_error(whitep, &cac->cac_nextc, cctx) == FAIL)
    {
	اگر (lhs->lhs_new_local)
	    ++cctx->ctx_locals.ga_len;
	بازگشت FAIL;
    }

    ret = compile_expr0_ext(&cac->cac_nextc, cctx, &cac->cac_is_const);
    اگر (lhs->lhs_new_local)
    {
	// Restore the local variable value.  Update lhs_lvar as the index of
	// the local variable might have changed.
	lvp = ((lvar_T *)cctx->ctx_locals.ga_data) + cctx->ctx_locals.ga_len;
	*lvp = save_lhs_lvar;
	lhs->lhs_lvar = lvp;

	++cctx->ctx_locals.ga_len;
    }

    بازگشت ret;
}

/*
 * When compiling an assignment, set the LHS type to the RHS type.
 */
    ایستا صحیح
compile_assign_set_lhs_type_from_rhs(
    cctx_T	*cctx,
    cac_T	*cac,
    lhs_T	*lhs,
    type_T	*rhs_type)
{
    اگر (rhs_type->tt_type == VAR_VOID)
    {
	emsg(_(e_cannot_use_void_مقدار));
	بازگشت FAIL;
    }

    type_T *type;

    // An empty list or dict has a &t_unknown member, for a variable that
    // implies &t_any.
    اگر (rhs_type == &t_list_empty)
	type = &t_list_any;
    وگرنه اگر (rhs_type == &t_dict_empty)
	type = &t_dict_any;
    وگرنه اگر (rhs_type == &t_unknown)
	type = &t_any;
    وگرنه
    {
	type = rhs_type;
	cac->cac_inferred_type = rhs_type;
    }

    set_var_type(lhs->lhs_lvar, type, cctx);

    بازگشت OK;
}

/*
 * Returns TRUE if the "rhs_type" can be assigned to the "lhs" variable.
 * Used when compiling an assignment statement.
 */
    ایستا صحیح
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
    اگر (cac->cac_var_شمار > 0)
    {
	where.wt_index = cac->cac_var_idx + 1;
	where.wt_kind = WT_VARIABLE;
    }

    // If assigning to a list or dict member, use the member type.
    // Not for "list[:] =".
    اگر (lhs->lhs_has_index &&
	    !has_list_index(cac->cac_var_start + lhs->lhs_varlen, cctx))
	use_type = lhs->lhs_member_type;

    اگر (need_type_where(rhs_type, use_type, 0, -1, where, cctx, FALSE,
						cac->cac_is_const) == FAIL)
	بازگشت FALSE;

    بازگشت TRUE;
}

/*
 * Compare the LHS type with the RHS type in an assignment.
 */
    ایستا صحیح
compile_assign_check_type(cctx_T *cctx, cac_T *cac)
{
    lhs_T	*lhs = &cac->cac_lhs;
    type_T	*rhs_type;

    rhs_type = cctx->ctx_type_پشته.ga_len == 0
					? &t_void
					: get_type_on_پشته(cctx, 0);
    cac->cac_rhs_type = rhs_type;

    اگر (check_type_is_مقدار(rhs_type) == FAIL)
	بازگشت FAIL;

    اگر (lhs->lhs_lvar != پوچ && (cac->cac_is_decl || !lhs->lhs_has_type))
    {
	اگر (rhs_type->tt_type == VAR_FUNC
					|| rhs_type->tt_type == VAR_PARTIAL)
	{
	    // Make sure the variable name can be used as a funcref
	    اگر (!lhs->lhs_has_index
				&& var_wrong_func_name(lhs->lhs_name, TRUE))
		بازگشت FAIL;
	}

	اگر (lhs->lhs_new_local && !lhs->lhs_has_type)
	{
	    // The LHS variable doesn't have a type.  Set it to the RHS type.
	    اگر (compile_assign_set_lhs_type_from_rhs(cctx, cac, lhs, rhs_type)
								== FAIL)
		بازگشت FAIL;
	}
	وگرنه اگر (*cac->cac_op == '=')
	{
	    اگر (!compile_assign_valid_rhs_type(cctx, cac, lhs, rhs_type))
		بازگشت FAIL;
	}
    }
    وگرنه
    {
	// Assigning to a register using @r = "abc"

	type_T *lhs_type = lhs->lhs_member_type;

	// Special case: assigning to @# can use a number or a string.
	// Also: can assign a number to a float.
	اگر ((lhs_type == &t_عدد_or_string || lhs_type == &t_float)
					&& rhs_type->tt_type == VAR_عدد)
	    lhs_type = &t_عدد;

	اگر (*cac->cac_nextc != '=')
	{
	    اگر (need_type(rhs_type, lhs_type, 0, -1, 0, cctx, FALSE,
							FALSE) == FAIL)
		بازگشت FAIL;
	}
    }

    بازگشت OK;
}

/*
 * Compile the RHS expression in an assignment statement and generate the
 * instructions.
 */
    ایستا صحیح
compile_assign_rhs_عبارت(cctx_T *cctx, cac_T *cac)
{
    cac->cac_is_const = FALSE;

    // for "+=", "*=", "..=" etc. first load the current value
    اگر (*cac->cac_op != '='
	    && compile_load_lhs_with_index(&cac->cac_lhs, cac->cac_var_start,
								cctx) == FAIL)
	بازگشت FAIL;

    // For "var = expr" evaluate the expression.
    اگر (cac->cac_var_شمار == 0)
    {
	صحیح	ret;

	// Compile the expression.
	cac->cac_instr_شمار = cac->cac_instr->ga_len;
	ret = compile_assign_single_eval_عبارت(cctx, cac);
	اگر (ret == FAIL)
	    بازگشت FAIL;
    }
    وگرنه اگر (cac->cac_semicolon && cac->cac_var_idx == cac->cac_var_شمار - 1)
    {
	// For "[var; var] = expr" get the rest of the list
	cac->cac_did_generate_slice = TRUE;
	اگر (generate_SLICE(cctx, cac->cac_var_شمار - 1) == FAIL)
	    بازگشت FAIL;
    }
    وگرنه
    {
	// For "[var, var] = expr" get the "var_idx" item from the
	// list.
	صحیح with_op = *cac->cac_op != '=';
	اگر (generate_GETITEM(cctx, cac->cac_var_idx, with_op) == FAIL)
	    بازگشت FAIL;
    }

    اگر (compile_assign_check_type(cctx, cac) == FAIL)
	بازگشت FAIL;

    بازگشت OK;
}

/*
 * Compile the RHS expression in an assignment
 */
    ایستا صحیح
compile_assign_rhs(cctx_T *cctx, cac_T *cac)
{
    lhs_T	*lhs = &cac->cac_lhs;

    اگر (cctx->ctx_skip == SKIP_YES)
    {
	اگر (cac->cac_oplen > 0 && cac->cac_var_شمار == 0)
	{
	    // skip over the "=" and the expression
	    cac->cac_nextc = skipwhite(cac->cac_op + cac->cac_oplen);
	    (تهی)compile_expr0(&cac->cac_nextc, cctx);
	}
	بازگشت OK;
    }

    // If RHS is specified, then generate instructions for RHS expression
    اگر (cac->cac_oplen > 0)
	بازگشت compile_assign_rhs_عبارت(cctx, cac);

    اگر (cac->cac_cmdidx == CMD_final)
    {
	emsg(_(e_final_requires_a_مقدار));
	بازگشت FAIL;
    }

    اگر (cac->cac_cmdidx == CMD_const)
    {
	emsg(_(e_const_requires_a_مقدار));
	بازگشت FAIL;
    }

    اگر (!lhs->lhs_has_type || lhs->lhs_dest == dest_option
					|| lhs->lhs_dest == dest_func_option)
    {
	emsg(_(e_type_or_initialization_required));
	بازگشت FAIL;
    }

    // variables are always initialized
    اگر (GA_GROW_FAILS(cac->cac_instr, 1))
	بازگشت FAIL;

    cac->cac_instr_شمار = cac->cac_instr->ga_len;

    بازگشت افزودن_default_مقدار(cctx, lhs->lhs_member_type->tt_type,
			      lhs->lhs_dest == dest_local,
			      &cac->cac_skip_store);
}

/*
 * Returns OK if "type" supports compound operator "op_arg" (e.g. +=, -=, %=,
 * etc.).  Compound operators are not supported with a tuple and a dict.
 * Returns FAIL if compound operator is not supported.
 */
    ایستا صحیح
check_type_supports_compound_op(type_T *type, char_u op_arg)
{
    اگر (type->tt_type == VAR_TUPLE || type->tt_type == VAR_DICT)
    {
	char_u	op[2];

	op[0] = op_arg;
	op[1] = NUL;
	semsg(_(e_wrong_variable_type_for_str_equal), op);
	بازگشت FAIL;
    }

    بازگشت OK;
}

/*
 * Compile a compound op assignment statement (+=, -=, *=, %=, etc.)
 */
    ایستا صحیح
compile_assign_compound_op(cctx_T *cctx, cac_T *cac)
{
    lhs_T	    *lhs = &cac->cac_lhs;
    type_T	    *expected;
    type_T	    *stacktype = پوچ;

    اگر (cac->cac_lhs.lhs_type->tt_type == VAR_TUPLE
	    && check_type_supports_compound_op(cac->cac_lhs.lhs_type,
							*cac->cac_op) == FAIL)
	بازگشت FAIL;

    اگر (*cac->cac_op == '.')
    {
	expected = lhs->lhs_member_type;
	stacktype = get_type_on_پشته(cctx, 0);
	اگر (expected != &t_string
		&& need_type(stacktype, expected, 0, -1, 0, cctx,
					FALSE, FALSE) == FAIL)
	    بازگشت FAIL;
	وگرنه اگر (may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
	    بازگشت FAIL;
    }
    وگرنه
    {
	expected = lhs->lhs_member_type;
	stacktype = get_type_on_پشته(cctx, 0);

	اگر (check_type_supports_compound_op(expected, *cac->cac_op) == FAIL)
	    بازگشت FAIL;

	اگر (
		// If variable is float operation with number is OK.
		!(expected == &t_float && (stacktype == &t_عدد
					|| stacktype == &t_عدد_bool))
		&& need_type(stacktype, expected, TYPECHK_عدد_OK, -1, 0,
						cctx, FALSE, FALSE) == FAIL)
	    بازگشت FAIL;
    }

    اگر (*cac->cac_op == '.')
    {
	اگر (generate_CONCAT(cctx, 2) == FAIL)
	    بازگشت FAIL;
    }
    وگرنه اگر (*cac->cac_op == '+')
    {
	اگر (generate_add_instr(cctx,
		    operator_type(lhs->lhs_member_type, stacktype),
		    lhs->lhs_member_type, stacktype,
		    عبارت_APPEND) == FAIL)
	    بازگشت FAIL;
    }
    وگرنه اگر (generate_two_op(cctx, cac->cac_op) == FAIL)
	بازگشت FAIL;

    بازگشت OK;
}

/*
 * Generate the STORE and SETTYPE instructions for an assignment statement.
 */
    ایستا صحیح
compile_assign_generate_store(cctx_T *cctx, cac_T *cac)
{
    lhs_T	*lhs = &cac->cac_lhs;
    صحیح		save_lnum;

    // Use the line number of the assignment for store instruction.
    save_lnum = cctx->ctx_lnum;
    cctx->ctx_lnum = cac->cac_start_lnum - 1;

    اگر (lhs->lhs_has_index)
    {
	// Use the info in "lhs" to store the value at the index in the
	// list, dict or object.
	اگر (compile_assign_unlet(cac->cac_var_start, &cac->cac_lhs,
				 TRUE, cac->cac_rhs_type, cctx) == FAIL)
	{
	    cctx->ctx_lnum = save_lnum;
	    بازگشت FAIL;
	}
    }
    وگرنه
    {
	اگر (cac->cac_is_decl && cac->cac_cmdidx == CMD_const &&
			(lhs->lhs_dest == dest_script
			 || lhs->lhs_dest == dest_script_v9
			 || lhs->lhs_dest == dest_global
			 || lhs->lhs_dest == dest_local))
	    // ":const var": lock the value, but not referenced variables
	    generate_LOCKCONST(cctx);

	type_T	*inferred_type = cac->cac_inferred_type;

	اگر ((lhs->lhs_type->tt_type == VAR_DICT
		    || lhs->lhs_type->tt_type == VAR_LIST)
		&& lhs->lhs_type->tt_member != پوچ
		&& lhs->lhs_type->tt_member != &t_any
		&& lhs->lhs_type->tt_member != &t_unknown)
	    // Set the type in the list or dict, so that it can be
	    // checked, also in legacy script.
	    generate_SETTYPE(cctx, lhs->lhs_type);
	وگرنه اگر (lhs->lhs_type->tt_type == VAR_TUPLE
					&& lhs->lhs_type->tt_argcount != 0)
	    generate_SETTYPE(cctx, lhs->lhs_type);
	وگرنه اگر (inferred_type != پوچ
		&& (inferred_type->tt_type == VAR_DICT
		    || inferred_type->tt_type == VAR_LIST)
		&& inferred_type->tt_member != پوچ
		&& inferred_type->tt_member != &t_unknown
		&& inferred_type->tt_member != &t_any)
	    // Set the type in the list or dict, so that it can be
	    // checked, also in legacy script.
	    generate_SETTYPE(cctx, inferred_type);
	وگرنه اگر (inferred_type != پوچ
				&& inferred_type->tt_type == VAR_TUPLE
				&& inferred_type->tt_argcount > 0)
	    generate_SETTYPE(cctx, inferred_type);

	اگر (!cac->cac_skip_store &&
		generate_store_lhs(cctx, &cac->cac_lhs,
				   cac->cac_instr_شمار,
				   cac->cac_is_decl) == FAIL)
	{
	    cctx->ctx_lnum = save_lnum;
	    بازگشت FAIL;
	}
    }

    cctx->ctx_lnum = save_lnum;

    بازگشت OK;
}

/*
 * Process the variable(s) in an assignment statement
 */
    ایستا صحیح
compile_assign_process_variables(
    cctx_T	*cctx,
    cac_T	*cac,
    صحیح		cmdidx,
    صحیح		heredoc,
    صحیح		has_cmd,
    صحیح		has_argisset_prefix,
    صحیح		jump_instr_idx)
{
    /*
     * Loop over variables in "[var, var] = expr".
     * For "name = expr" and "var name: type" this is done only once.
     */
    برای (cac->cac_var_idx = 0; cac->cac_var_idx == 0 ||
	    cac->cac_var_idx < cac->cac_var_شمار; cac->cac_var_idx++)
    {
	اگر (cac->cac_var_start[0] == '_'
				&& !eval_isnamec(cac->cac_var_start[1]))
	{
	    // Ignore underscore in "[a, _, b] = list".
	    اگر (cac->cac_var_شمار > 0)
	    {
		cac->cac_var_start = skipwhite(cac->cac_var_start + 2);
		ادامه;
	    }
	    emsg(_(e_cannot_use_underscore_here));
	    بازگشت FAIL;
	}
	vim_free(cac->cac_lhs.lhs_name);

	/*
	 * Figure out the LHS type and other properties.
	 */
	اگر (compile_assign_lhs(cac->cac_var_start, &cac->cac_lhs, cmdidx,
		    cac->cac_is_decl, heredoc, has_cmd,
		    cac->cac_oplen, cctx) == FAIL)
	    بازگشت FAIL;

	// Compile the RHS expression
	اگر (heredoc)
	{
	    SOURCING_LNUM = cac->cac_start_lnum;
	    اگر (cac->cac_lhs.lhs_has_type
		    && need_type(&t_list_string, cac->cac_lhs.lhs_type,
				0, -1, 0, cctx, FALSE, FALSE) == FAIL)
		بازگشت FAIL;
	}
	وگرنه
	{
	    اگر (compile_assign_rhs(cctx, cac) == FAIL)
		بازگشت FAIL;
	    اگر (cac->cac_var_شمار == 0)
		cac->cac_var_end = cac->cac_nextc;
	}

	// no need to parse more when skipping
	اگر (cctx->ctx_skip == SKIP_YES)
	    بشکن;

	اگر (cac->cac_oplen > 0 && *cac->cac_op != '=')
	{
	    اگر (compile_assign_compound_op(cctx, cac) == FAIL)
		بازگشت FAIL;
	}

	// generate the store instructions
	اگر (compile_assign_generate_store(cctx, cac) == FAIL)
	    بازگشت FAIL;

	اگر (cac->cac_var_idx + 1 < cac->cac_var_شمار)
	    cac->cac_var_start = skipwhite(cac->cac_lhs.lhs_end + 1);

	اگر (has_argisset_prefix)
	{
	    // set instruction index in JUMP_IF_ARG_SET to here
	    isn_T *isn = ((isn_T *)cac->cac_instr->ga_data) + jump_instr_idx;
	    isn->isn_arg.jumparg.jump_where = cac->cac_instr->ga_len;
	}
    }

    بازگشت OK;
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
    ایستا char_u *
compile_assignment(
    char_u	*arg_start,
    exarg_T	*eap,
    cmdidx_T	cmdidx,
    cctx_T	*cctx)
{
    cac_T	cac;
    char_u	*arg = arg_start;
    char_u	*retstr = پوچ;
    صحیح		heredoc = FALSE;
    صحیح		jump_instr_idx;

    compile_assign_context_init(&cac, cctx, cmdidx, arg);

    jump_instr_idx = cac.cac_instr->ga_len;

    // process object variable initialization in a new() constructor method
    صحیح	has_argisset_prefix = STRNCMP(arg, "ifargisset ", 11) == 0;
    اگر (has_argisset_prefix &&
			compile_assign_obj_new_arg(&arg, cctx) == FAIL)
	برو theend;

    // Skip over the "varname" or "[varname, varname]" to get to any "=".
    cac.cac_nextc = skip_var_list(arg, TRUE, &cac.cac_var_شمار,
						&cac.cac_semicolon, TRUE);
    اگر (cac.cac_nextc == پوچ)
	بازگشت *arg == '[' ? arg : پوچ;

    اگر (compile_assign_process_operator(eap, arg, &cac, &heredoc,
							&retstr) == FAIL)
	بازگشت retstr;

    // Compute the start of the assignment
    cac.cac_var_start = compile_assign_compute_start(arg, cac.cac_var_شمار);

    // Compute the end of the assignment
    cac.cac_var_end = compile_assign_compute_end(eap, cctx, &cac, heredoc);
    اگر (cac.cac_var_end == پوچ)
	بازگشت پوچ;

    صحیح has_cmd = cac.cac_var_start > eap->cmd;

    /* process the variable(s) */
    اگر (compile_assign_process_variables(cctx, &cac, cmdidx, heredoc,
					 has_cmd, has_argisset_prefix,
					 jump_instr_idx) == FAIL)
	برو theend;

    // For "[var, var] = expr" drop the "expr" value.
    // Also for "[var, var; _] = expr".
    اگر (cctx->ctx_skip != SKIP_YES && cac.cac_var_شمار > 0 &&
	    (!cac.cac_semicolon || !cac.cac_did_generate_slice))
    {
	اگر (generate_instr_drop(cctx, ISN_DROP, 1) == پوچ)
	    برو theend;
    }

    retstr = skipwhite(cac.cac_var_end);

theend:
    vim_free(cac.cac_lhs.lhs_name);
    بازگشت retstr;
}

/*
 * Check for an assignment at "eap->cmd", compile it if found.
 * Return NOTDONE if there is none, FAIL for failure, OK if done.
 */
    ایستا صحیح
may_compile_assignment(exarg_T *eap, char_u **سطر, cctx_T *cctx)
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
    اگر (p > eap->cmd && *p != NUL)
    {
	char_u *var_end;
	صحیح	oplen;
	صحیح	heredoc;

	اگر (eap->cmd[0] == '@')
	    var_end = eap->cmd + 2;
	وگرنه
	    var_end = find_name_end(pskip, پوچ, پوچ,
					FNE_CHECK_START | FNE_INCL_BR);
	oplen = assignment_len(skipwhite(var_end), &heredoc);
	اگر (oplen > 0)
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
	    اگر (*eap->cmd == '&'
		    || *eap->cmd == '$'
		    || *eap->cmd == '@'
		    || ((len) > 2 && eap->cmd[1] == ':')
		    || STRNCMP(eap->cmd, "g:[", 3) == 0
		    || variable_exists(eap->cmd, len, cctx))
	    {
		*سطر = compile_assignment(eap->cmd, eap, CMD_SIZE, cctx);
		اگر (*سطر == پوچ || *سطر == eap->cmd)
		    بازگشت FAIL;
		بازگشت OK;
	    }
	}
    }

    // might be "[var, var] = expr" or "ifargisset this.member = expr"
    اگر (*eap->cmd == '[' || STRNCMP(eap->cmd, "ifargisset ", 11) == 0)
    {
	*سطر = compile_assignment(eap->cmd, eap, CMD_SIZE, cctx);
	اگر (*سطر == پوچ)
	    بازگشت FAIL;
	اگر (*سطر != eap->cmd)
	    بازگشت OK;
    }
    بازگشت NOTDONE;
}

/*
 * Check if arguments of "ufunc" shadow variables in "cctx".
 * Return OK or FAIL.
 */
    ایستا صحیح
check_args_shadowing(ufunc_T *ufunc, cctx_T *cctx)
{
    صحیح	    i;
    char_u  *arg;
    صحیح	    r = OK;

    // Make sure arguments are not found when compiling a second time.
    ufunc->uf_args_visible = 0;

    // Check for arguments shadowing variables from the context.
    برای (i = 0; i < ufunc->uf_args.ga_len; ++i)
    {
	arg = ((char_u **)(ufunc->uf_args.ga_data))[i];
	اگر (check_defined(arg, STRLEN(arg), cctx, پوچ, TRUE) == FAIL)
	{
	    r = FAIL;
	    بشکن;
	}
    }
    ufunc->uf_args_visible = ufunc->uf_args.ga_len;
    بازگشت r;
}

#ifdef HAS_MESSAGE_WINDOW
/*
 * Get a count before a command.  Can only be a number.
 * Returns zero if there is no count.
 * Returns -1 if there is something wrong.
 */
    ایستا long
get_cmd_شمار(char_u *سطر, exarg_T *eap)
{
    char_u *p;

    // skip over colons and white space
    برای (p = سطر; *p == ':' || VIM_ISWHITE(*p); ++p)
	;
    اگر (!SAFE_isdigit(*p))
    {
	// The command or modifiers must be following.  Assume a lower case
	// character means there is a modifier.
	اگر (p < eap->cmd && !vim_islower(*p))
	{
	    emsg(_(e_invalid_range));
	    بازگشت -1;
	}
	بازگشت 0;
    }
    بازگشت atol((نویسه *)p);
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

    اگر (debug_break_level > 0 || may_break_in_function(ufunc))
	بازگشت CT_DEBUG;
#ifdef FEAT_PROFILE
    اگر (do_profiling == PROF_YES)
    {
	اگر (!ufunc->uf_profiling && has_profiling(FALSE, ufunc->uf_name, پوچ,
							    &ufunc->uf_hash))
	    func_do_profile(ufunc);
	اگر (ufunc->uf_profiling)
	    بازگشت CT_PROFILE;
    }
#endif
    بازگشت CT_NONE;
}

/*
 * Free the compiled instructions saved for a def function.  This is used when
 * compiling a def function and the function was compiled before.
 * The index is reused.
 */
    ایستا تهی
clear_def_function(ufunc_T *ufunc, compiletype_T compile_type)
{
    isn_T	*instr_dest = پوچ;
    dfunc_T	*dfunc;

    dfunc = ((dfunc_T *)def_functions.ga_data) + ufunc->uf_dfunc_idx;

    انتخاب (compile_type)
    {
	حالت CT_PROFILE:
#ifdef FEAT_PROFILE
	    instr_dest = dfunc->df_instr_prof; بشکن;
#endif
	حالت CT_NONE:   instr_dest = dfunc->df_instr; بشکن;
	حالت CT_DEBUG:  instr_dest = dfunc->df_instr_debug; بشکن;
    }

    اگر (instr_dest != پوچ)
	// Was compiled in this mode before: Free old instructions.
	delete_def_function_contents(dfunc, FALSE);

    ga_clear_strings(&dfunc->df_var_names);
    dfunc->df_defer_var_idx = 0;
}

/*
 * Add a function to the list of :def functions.
 * This sets "ufunc->uf_dfunc_idx" but the function isn't compiled yet.
 */
    ایستا صحیح
add_def_function(ufunc_T *ufunc)
{
    dfunc_T *dfunc;

    اگر (def_functions.ga_len == 0)
    {
	// The first position is not used, so that a zero uf_dfunc_idx means it
	// wasn't set.
	اگر (GA_GROW_FAILS(&def_functions, 1))
	    بازگشت FAIL;
	++def_functions.ga_len;
    }

    // Add the function to "def_functions".
    اگر (GA_GROW_FAILS(&def_functions, 1))
	بازگشت FAIL;
    dfunc = ((dfunc_T *)def_functions.ga_data) + def_functions.ga_len;
    CLEAR_POINTER(dfunc);
    dfunc->df_idx = def_functions.ga_len;
    ufunc->uf_dfunc_idx = dfunc->df_idx;
    dfunc->df_ufunc = ufunc;
    dfunc->df_name = vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
    ga_init2(&dfunc->df_var_names, اندازه(char_u *), 10);
    ++dfunc->df_refcount;
    ++def_functions.ga_len;
    بازگشت OK;
}

    ایستا صحیح
compile_dfunc_ufunc_init(
    ufunc_T		*ufunc,
    cctx_T		*outer_cctx,
    compiletype_T	compile_type,
    صحیح			*new_def_function)
{
    // When using a function that was compiled before: Free old instructions.
    // The index is reused.  Otherwise add a new entry in "def_functions".
    اگر (ufunc->uf_dfunc_idx > 0)
	clear_def_function(ufunc, compile_type);
    وگرنه
    {
	اگر (add_def_function(ufunc) == FAIL)
	    بازگشت FAIL;

	*new_def_function = TRUE;
    }

    اگر ((ufunc->uf_flags & FC_CLOSURE) && outer_cctx == پوچ)
    {
	semsg(_(e_compiling_closure_without_context_str),
						printable_func_name(ufunc));
	بازگشت FAIL;
    }

    ufunc->uf_def_status = UF_COMPILING;

    بازگشت OK;
}

/*
 * Initialize the compilation context for compiling a def function.
 */
    ایستا تهی
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
    ga_init2(&cctx->ctx_locals, اندازه(lvar_T), 10);
    // Each entry on the type stack consists of two type pointers.
    ga_init2(&cctx->ctx_type_پشته, اندازه(type2_T), 50);
    cctx->ctx_type_list = &ufunc->uf_type_list;
    ga_init2(&cctx->ctx_instr, اندازه(isn_T), 50);
}

/*
 * For an object constructor, generate instruction to setup "this" (the first
 * local variable) and to initialize the object variables.
 */
    ایستا صحیح
obj_constructor_prologue(ufunc_T *ufunc, cctx_T *cctx)
{
    generate_CONSTRUCT(cctx, ufunc->uf_class);

    برای (صحیح i = 0; i < ufunc->uf_class->class_obj_member_شمار; ++i)
    {
	ocmember_T *m = &ufunc->uf_class->class_obj_members[i];

	اگر (i < 2 && IS_ENUM(ufunc->uf_class))
	    // The first two object variables in an enum are the name
	    // and the ordinal.  These are set by the ISN_CONSTRUCT
	    // instruction.  So don't generate instructions to set
	    // these variables.
	    ادامه;

	اگر (m->ocm_init != پوچ)
	{
	    char_u	*عبارت = m->ocm_init;
	    sctx_T	save_current_sctx;
	    صحیح		change_sctx = FALSE;

	    // If the member variable initialization script context is
	    // different from the current script context, then change it.
	    اگر (current_sctx.sc_sid != m->ocm_init_sctx.sc_sid)
		change_sctx = TRUE;

	    اگر (change_sctx)
	    {
		// generate an instruction to change the script context to the
		// member variable initialization script context.
		save_current_sctx = current_sctx;
		current_sctx = m->ocm_init_sctx;
		generate_SCRIPTCTX_SET(cctx, current_sctx);
	    }

	    صحیح r = compile_expr0(&عبارت, cctx);

	    اگر (change_sctx)
	    {
		// restore the previous script context
		current_sctx = save_current_sctx;
		generate_SCRIPTCTX_SET(cctx, current_sctx);
	    }

	    اگر (r == FAIL)
		بازگشت FAIL;

	    اگر (!ends_excmd2(m->ocm_init, عبارت))
	    {
		semsg(_(e_trailing_characters_str), عبارت);
		بازگشت FAIL;
	    }

	    type_T	*type = get_type_on_پشته(cctx, 0);
	    اگر (m->ocm_type->tt_type == VAR_ANY
		    && !(m->ocm_flags & OCMFLAG_HAS_TYPE)
		    && type->tt_type != VAR_SPECIAL)
	    {
		// If the member variable type is not yet set, then use
		// the initialization expression type.
		m->ocm_type = type;
	    }
	    وگرنه
	    {
		// The type of the member initialization expression is
		// determined at run time.  Add a runtime type check.
		where_T	where = WHERE_INIT;
		where.wt_kind = WT_MEMBER;
		where.wt_func_name = (نویسه *)m->ocm_name.string;
		اگر (need_type_where(type, m->ocm_type, 0, -1,
					where, cctx, FALSE, FALSE) == FAIL)
		    بازگشت FAIL;
	    }
	}
	وگرنه
	    افزودن_default_مقدار(cctx, m->ocm_type->tt_type, FALSE, پوچ);

	اگر (((m->ocm_type->tt_type == VAR_DICT
			|| m->ocm_type->tt_type == VAR_LIST)
		    && m->ocm_type->tt_member != پوچ
		    && m->ocm_type->tt_member != &t_any
		    && m->ocm_type->tt_member != &t_unknown)
		|| (m->ocm_type->tt_type == VAR_TUPLE
		    && m->ocm_type->tt_argcount > 0))
	    // Set the type in the list, tuple or dict, so that it can be
	    // checked, also in legacy script.
	    generate_SETTYPE(cctx, m->ocm_type);

	generate_STORE_THIS(cctx, i);
    }

    بازگشت OK;
}

/*
 * For an object method and an constructor, generate instruction to setup
 * "this" (the first local variable).  For a constructor, generate instructions
 * to initialize the object variables.
 */
    ایستا صحیح
obj_method_prologue(ufunc_T *ufunc, cctx_T *cctx)
{
    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data) + ufunc->uf_dfunc_idx;

    اگر (GA_GROW_FAILS(&dfunc->df_var_names, 1))
	بازگشت FAIL;

    ((char_u **)dfunc->df_var_names.ga_data)[0] =
						vim_strsave((char_u *)"this");
    ++dfunc->df_var_names.ga_len;

    // In the constructor allocate memory for the object and initialize the
    // object members.
    اگر (IS_CONSTRUCTOR_METHOD(ufunc))
	بازگشت obj_constructor_prologue(ufunc, cctx);

    بازگشت OK;
}

/*
 * Produce instructions for the default values of optional arguments.
 */
    ایستا صحیح
compile_def_function_default_args(
    ufunc_T	*ufunc,
    garray_T	*instr,
    cctx_T	*cctx)
{
    صحیح	شمار = ufunc->uf_def_args.ga_len;
    صحیح	first_def_arg = ufunc->uf_args.ga_len - شمار;
    صحیح	i;
    صحیح	off = پشته_FRAME_SIZE + (ufunc->uf_va_name != پوچ ? 1 : 0);
    صحیح	did_set_arg_type = FALSE;

    // Produce instructions for the default values of optional arguments.
    SOURCING_LNUM = 0;  // line number unknown
    برای (i = 0; i < شمار; ++i)
    {
	char_u *arg = ((char_u **)(ufunc->uf_def_args.ga_data))[i];
	اگر (STRCMP(arg, "v:none") == 0)
	    // "arg = v:none" means the argument is optional without
	    // setting a value when the argument is missing.
	    ادامه;

	type_T	*val_type;
	صحیح		arg_idx = first_def_arg + i;
	where_T	where = WHERE_INIT;
	صحیح		jump_instr_idx = instr->ga_len;
	isn_T	*isn;

	// Use a JUMP_IF_ARG_SET instruction to skip if the value was given.
	اگر (generate_JUMP_IF_ARG(cctx, ISN_JUMP_IF_ARG_SET,
						i - شمار - off) == FAIL)
	    بازگشت FAIL;

	// Make sure later arguments are not found.
	ufunc->uf_args_visible = arg_idx;

	صحیح r = compile_expr0(&arg, cctx);
	اگر (r == FAIL)
	    بازگشت FAIL;

	// If no type specified use the type of the default value.
	// Otherwise check that the default value type matches the
	// specified type.
	val_type = get_type_on_پشته(cctx, 0);
	where.wt_index = arg_idx + 1;
	where.wt_kind = WT_ARGUMENT;
	اگر (ufunc->uf_arg_types[arg_idx] == &t_unknown)
	{
	    did_set_arg_type = TRUE;
	    ufunc->uf_arg_types[arg_idx] = val_type;
	}
	وگرنه اگر (need_type_where(val_type, ufunc->uf_arg_types[arg_idx],
		    0, -1, where, cctx, FALSE, FALSE) == FAIL)
	    بازگشت FAIL;

	اگر (generate_STORE(cctx, ISN_STORE, i - شمار - off, پوچ) == FAIL)
	    بازگشت FAIL;

	// set instruction index in JUMP_IF_ARG_SET to here
	isn = ((isn_T *)instr->ga_data) + jump_instr_idx;
	isn->isn_arg.jumparg.jump_where = instr->ga_len;
    }

    اگر (did_set_arg_type)
	set_function_type(ufunc);

    بازگشت OK;
}

/*
 * Compile def function body.  Loop over all the lines in the function and
 * generate instructions.
 */
    ایستا صحیح
compile_def_function_body(
    صحیح		last_func_lnum,
    صحیح		check_return_type,
    garray_T	*سطرها_to_free,
    نویسه	**errormsg,
    cctx_T	*cctx)
{
    char_u	*سطر = پوچ;
    char_u	*p;
    صحیح		did_emsg_before = did_emsg;
#ifdef FEAT_PROFILE
    صحیح		prof_lnum = -1;
#endif
    صحیح		debug_lnum = -1;

    برای (;;)
    {
	exarg_T	    ea;
	صحیح	    starts_with_colon = FALSE;
	char_u	    *cmd;
	cmdmod_T    local_cmdmod;

	// Bail out on the first error to avoid a flood of errors and report
	// the right line number when inside try/catch.
	اگر (did_emsg_before != did_emsg)
	    بازگشت FAIL;

	اگر (سطر != پوچ && *سطر == '|')
	    // the line continues after a '|'
	    ++سطر;
	وگرنه اگر (سطر != پوچ && *skipwhite(سطر) != NUL
		&& !(*سطر == '#' && (سطر == cctx->ctx_سطر_start
						    || VIM_ISWHITE(سطر[-1]))))
	{
	    semsg(_(e_trailing_characters_str), سطر);
	    بازگشت FAIL;
	}
	وگرنه اگر (سطر != پوچ && vim9_bad_comment(skipwhite(سطر)))
	    بازگشت FAIL;
	وگرنه
	{
	    سطر = next_سطر_from_context(cctx, FALSE);
	    اگر (cctx->ctx_lnum >= last_func_lnum)
	    {
		// beyond the last line
#ifdef FEAT_PROFILE
		اگر (cctx->ctx_skip != SKIP_YES)
		    may_generate_prof_end(cctx, prof_lnum);
#endif
		بشکن;
	    }
	    // Make a copy, splitting off nextcmd and removing trailing spaces
	    // may change it.
	    اگر (سطر != پوچ)
	    {
		سطر = vim_strsave(سطر);
		اگر (ga_add_string(سطرها_to_free, سطر) == FAIL)
		{
		    vim_free(سطر);
		    بازگشت FAIL;
		}
	    }
	}

	CLEAR_FIELD(ea);
	ea.cmdlinep = &سطر;
	ea.cmd = skipwhite(سطر);
	ea.skip = cctx->ctx_skip == SKIP_YES;

	اگر (*ea.cmd == '#')
	{
	    // "#" starts a comment, but "#{" is an error
	    اگر (vim9_bad_comment(ea.cmd))
		بازگشت FAIL;
	    سطر = (char_u *)"";
	    ادامه;
	}

#ifdef FEAT_PROFILE
	اگر (cctx->ctx_compile_type == CT_PROFILE && cctx->ctx_lnum != prof_lnum
						  && cctx->ctx_skip != SKIP_YES)
	{
	    may_generate_prof_end(cctx, prof_lnum);

	    prof_lnum = cctx->ctx_lnum;
	    generate_instr(cctx, ISN_PROF_START);
	}
#endif
	اگر (cctx->ctx_compile_type == CT_DEBUG && cctx->ctx_lnum != debug_lnum
						  && cctx->ctx_skip != SKIP_YES)
	{
	    debug_lnum = cctx->ctx_lnum;
	    generate_instr_debug(cctx);
	}
	cctx->ctx_prev_lnum = cctx->ctx_lnum + 1;

	// Some things can be recognized by the first character.
	انتخاب (*ea.cmd)
	{
	    حالت '}':
		{
		    // "}" ends a block scope
		    scopetype_T stype = cctx->ctx_scope == پوچ
					  ? NO_SCOPE : cctx->ctx_scope->se_type;

		    اگر (stype == BLOCK_SCOPE)
		    {
			compile_endblock(cctx);
			سطر = ea.cmd;
		    }
		    وگرنه
		    {
			emsg(_(e_using_rcurly_outside_if_block_scope));
			بازگشت FAIL;
		    }
		    اگر (سطر != پوچ)
			سطر = skipwhite(ea.cmd + 1);
		    ادامه;
		}

	    حالت '{':
		// "{" starts a block scope
		// "{'a': 1}->func() is something else
		اگر (ends_excmd(*skipwhite(ea.cmd + 1)))
		{
		    سطر = compile_block(ea.cmd, cctx);
		    ادامه;
		}
		بشکن;
	}

	/*
	 * COMMAND MODIFIERS
	 */
	cctx->ctx_has_cmdmod = FALSE;
	اگر (parse_command_modifiers(&ea, errormsg, &local_cmdmod, FALSE)
								       == FAIL)
	    بازگشت FAIL;
	generate_cmdmods(cctx, &local_cmdmod);
	undo_cmdmod(&local_cmdmod);

	// Check if there was a colon after the last command modifier or before
	// the current position.
	برای (p = ea.cmd; p >= سطر; --p)
	{
	    اگر (*p == ':')
		starts_with_colon = TRUE;
	    اگر (p < ea.cmd && !VIM_ISWHITE(*p))
		بشکن;
	}

	// Skip ":call" to get to the function name, unless using :legacy
	p = ea.cmd;
	اگر (!(local_cmdmod.cmod_flags & CMOD_LEGACY))
	{
	    اگر (checkforcmd(&ea.cmd, "call", 3))
	    {
		اگر (*ea.cmd == '(')
		    // not for "call()"
		    ea.cmd = p;
		وگرنه
		    ea.cmd = skipwhite(ea.cmd);
	    }

	    اگر (!starts_with_colon)
	    {
		صحیح	    assign;

		// Check for assignment after command modifiers.
		assign = may_compile_assignment(&ea, &سطر, cctx);
		اگر (assign == OK)
		    برو nextline;
		اگر (assign == FAIL)
		    بازگشت FAIL;
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
	اگر ((*cmd != '$' || starts_with_colon)
		&& (starts_with_colon
		    || !(*cmd == '\''
			|| (cmd[0] == '0' && cmd[1] == 'z')
			|| (cmd[0] != NUL && cmd[0] == cmd[1]
					    && (*cmd == '+' || *cmd == '-'))
			|| عدد_method(cmd))))
	{
	    ea.cmd = skip_range(ea.cmd, TRUE, پوچ);
	    اگر (ea.cmd > cmd)
	    {
		اگر (!starts_with_colon
				   && !(local_cmdmod.cmod_flags & CMOD_LEGACY))
		{
		    semsg(_(e_colon_required_before_range_str), cmd);
		    بازگشت FAIL;
		}
		ea.addr_شمار = 1;
		اگر (ends_excmd2(سطر, ea.cmd))
		{
		    // A range without a command: jump to the line.
		    generate_EXEC(cctx, ISN_EXECRANGE,
					      vim_strnsave(cmd, ea.cmd - cmd));
		    سطر = ea.cmd;
		    برو nextline;
		}
	    }
	}
	p = find_ex_command(&ea, پوچ,
		starts_with_colon || (local_cmdmod.cmod_flags & CMOD_LEGACY)
						  ? پوچ : item_exists, cctx);

	اگر (p == پوچ)
	{
	    اگر (cctx->ctx_skip != SKIP_YES)
		semsg(_(e_ambiguous_use_of_user_defined_command_str), ea.cmd);
	    بازگشت FAIL;
	}

	// When using ":legacy cmd" always use compile_exec().
	اگر (local_cmdmod.cmod_flags & CMOD_LEGACY)
	{
	    char_u *start = ea.cmd;

	    انتخاب (ea.cmdidx)
	    {
		حالت CMD_if:
		حالت CMD_elseif:
		حالت CMD_else:
		حالت CMD_endif:
		حالت CMD_for:
		حالت CMD_endfor:
		حالت CMD_continue:
		حالت CMD_break:
		حالت CMD_while:
		حالت CMD_endwhile:
		حالت CMD_try:
		حالت CMD_catch:
		حالت CMD_finally:
		حالت CMD_endtry:
			semsg(_(e_cannot_use_legacy_with_command_str), ea.cmd);
			بازگشت FAIL;
		پیشفرض: بشکن;
	    }

	    // ":legacy return expr" needs to be handled differently.
	    اگر (checkforcmd(&start, "return", 4))
		ea.cmdidx = CMD_return;
	    وگرنه
		ea.cmdidx = CMD_legacy;
	}

	اگر (p == ea.cmd && ea.cmdidx != CMD_SIZE)
	{
	    // "eval" is used for "val->func()" and "var" for "var = val", then
	    // "p" is equal to "ea.cmd" for a valid command.
	    اگر (ea.cmdidx == CMD_eval || ea.cmdidx == CMD_var)
		;
	    وگرنه اگر (cctx->ctx_skip == SKIP_YES)
	    {
		سطر += STRLEN(سطر);
		برو nextline;
	    }
	    وگرنه
	    {
		semsg(_(e_command_not_recognized_str), ea.cmd);
		بازگشت FAIL;
	    }
	}

	اگر ((cctx->ctx_had_return || cctx->ctx_had_throw)
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
	    بازگشت FAIL;
	}

	// When processing the end of an if-else or try-catch block, don't
	// clear the "ctx_had_throw" flag.  If such a block ends in a "throw"
	// statement, then it is considered to end in a "return" statement.
	// The "ctx_had_throw" is cleared immediately after processing the
	// block ending statement.
	// Otherwise, clear the "had_throw" flag.
	اگر (ea.cmdidx != CMD_else && ea.cmdidx != CMD_elseif
						&& ea.cmdidx != CMD_endif
						&& ea.cmdidx != CMD_catch
						&& ea.cmdidx != CMD_finally
						&& ea.cmdidx != CMD_endtry)
	    cctx->ctx_had_throw = FALSE;

	p = skipwhite(p);
	اگر (ea.cmdidx != CMD_SIZE
			    && ea.cmdidx != CMD_write && ea.cmdidx != CMD_read)
	{
	    اگر (ea.cmdidx >= 0)
		ea.argt = excmd_get_argt(ea.cmdidx);
	    اگر ((ea.argt & EX_BANG) && *p == '!')
	    {
		ea.forceit = TRUE;
		p = skipwhite(p + 1);
	    }
	    اگر ((ea.argt & EX_RANGE) == 0 && ea.addr_شمار > 0)
	    {
		emsg(_(e_no_range_allowed));
		بازگشت FAIL;
	    }
	}

	انتخاب (ea.cmdidx)
	{
	    حالت CMD_def:
	    حالت CMD_function:
		    ea.arg = p;
		    سطر = compile_nested_function(&ea, cctx, سطرها_to_free);
		    بشکن;

	    حالت CMD_return:
		    سطر = compile_return(p, check_return_type,
				 local_cmdmod.cmod_flags & CMOD_LEGACY, cctx);
		    cctx->ctx_had_return = TRUE;
		    بشکن;

	    حالت CMD_let:
		    emsg(_(e_cannot_use_let_in_vim9_script));
		    بشکن;
	    حالت CMD_var:
	    حالت CMD_final:
	    حالت CMD_const:
	    حالت CMD_increment:
	    حالت CMD_decrement:
		    سطر = compile_assignment(p, &ea, ea.cmdidx, cctx);
		    اگر (سطر == p)
		    {
			emsg(_(e_invalid_assignment));
			سطر = پوچ;
		    }
		    بشکن;

	    حالت CMD_unlet:
	    حالت CMD_unlockvar:
	    حالت CMD_lockvar:
		    سطر = compile_unletlock(p, &ea, cctx);
		    بشکن;

	    حالت CMD_import:
		    emsg(_(e_import_can_only_be_used_in_script));
		    سطر = پوچ;
		    بشکن;

	    حالت CMD_if:
		    سطر = compile_if(p, cctx);
		    بشکن;
	    حالت CMD_elseif:
		    سطر = compile_elseif(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    cctx->ctx_had_throw = FALSE;
		    بشکن;
	    حالت CMD_else:
		    سطر = compile_else(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    cctx->ctx_had_throw = FALSE;
		    بشکن;
	    حالت CMD_endif:
		    سطر = compile_endif(p, cctx);
		    cctx->ctx_had_throw = FALSE;
		    بشکن;

	    حالت CMD_while:
		    سطر = compile_while(p, cctx);
		    بشکن;
	    حالت CMD_endwhile:
		    سطر = compile_endwhile(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    بشکن;

	    حالت CMD_for:
		    سطر = compile_for(p, cctx);
		    بشکن;
	    حالت CMD_endfor:
		    سطر = compile_endfor(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    بشکن;
	    حالت CMD_continue:
		    سطر = compile_continue(p, cctx);
		    // "continue" unconditionally jumps back to the loop,
		    // like a "return".
		    cctx->ctx_had_return = TRUE;
		    بشکن;
	    حالت CMD_break:
		    سطر = compile_break(p, cctx);
		    // "break" unconditionally exits the loop, like a "return".
		    cctx->ctx_had_return = TRUE;
		    بشکن;

	    حالت CMD_try:
		    سطر = compile_try(p, cctx);
		    بشکن;
	    حالت CMD_catch:
		    سطر = compile_catch(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    cctx->ctx_had_throw = FALSE;
		    بشکن;
	    حالت CMD_finally:
		    سطر = compile_finally(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    cctx->ctx_had_throw = FALSE;
		    بشکن;
	    حالت CMD_endtry:
		    سطر = compile_endtry(p, cctx);
		    cctx->ctx_had_throw = FALSE;
		    بشکن;
	    حالت CMD_throw:
		    سطر = compile_throw(p, cctx);
		    cctx->ctx_had_throw = TRUE;
		    بشکن;

	    حالت CMD_eval:
		    سطر = compile_eval(p, cctx);
		    بشکن;

	    حالت CMD_defer:
		    سطر = compile_defer(p, cctx);
		    بشکن;

#ifdef HAS_MESSAGE_WINDOW
	    حالت CMD_echowindow:
		    {
			long cmd_شمار = get_cmd_شمار(سطر, &ea);
			اگر (cmd_شمار < 0)
			    سطر = پوچ;
			وگرنه
			    سطر = compile_mult_عبارت(p, ea.cmdidx,
							     cmd_شمار, cctx);
		    }
		    بشکن;
#endif
	    حالت CMD_echo:
	    حالت CMD_echon:
	    حالت CMD_echoconsole:
	    حالت CMD_echoerr:
	    حالت CMD_echomsg:
	    حالت CMD_execute:
		    سطر = compile_mult_عبارت(p, ea.cmdidx, 0, cctx);
		    بشکن;

	    حالت CMD_put:
		    ea.cmd = cmd;
		    سطر = compile_put(p, &ea, cctx, FALSE);
		    بشکن;

	    حالت CMD_iput:
		    ea.cmd = cmd;
		    سطر = compile_put(p, &ea, cctx, TRUE);
		    بشکن;

	    حالت CMD_substitute:
		    اگر (check_global_and_subst(ea.cmd, p) == FAIL)
			بازگشت FAIL;
		    اگر (cctx->ctx_skip == SKIP_YES)
			سطر = (char_u *)"";
		    وگرنه
		    {
			ea.arg = p;
			سطر = compile_substitute(سطر, &ea, cctx);
		    }
		    بشکن;

	    حالت CMD_redir:
		    ea.arg = p;
		    سطر = compile_redir(سطر, &ea, cctx);
		    بشکن;

	    حالت CMD_cexpr:
	    حالت CMD_lexpr:
	    حالت CMD_caddexpr:
	    حالت CMD_laddexpr:
	    حالت CMD_cgetexpr:
	    حالت CMD_lgetexpr:
#ifdef FEAT_QUICKFIX
		    ea.arg = p;
		    سطر = compile_cexpr(سطر, &ea, cctx);
#وگرنه
		    ex_ni(&ea);
		    سطر = پوچ;
#endif
		    بشکن;

	    حالت CMD_append:
	    حالت CMD_change:
	    حالت CMD_درج:
	    حالت CMD_k:
	    حالت CMD_open:
	    حالت CMD_t:
	    حالت CMD_xit:
		    not_in_vim9(&ea, false);
		    بازگشت FAIL;

	    حالت CMD_SIZE:
		    اگر (cctx->ctx_skip != SKIP_YES)
		    {
			semsg(_(e_invalid_command_str), ea.cmd);
			بازگشت FAIL;
		    }
		    // We don't check for a next command here.
		    سطر = (char_u *)"";
		    بشکن;

	    حالت CMD_lua:
	    حالت CMD_mzscheme:
	    حالت CMD_perl:
	    حالت CMD_py3:
	    حالت CMD_python3:
	    حالت CMD_python:
	    حالت CMD_pythonx:
	    حالت CMD_ruby:
	    حالت CMD_tcl:
		    ea.arg = p;
		    اگر (vim_strchr(سطر, '\n') == پوچ)
			سطر = compile_exec(سطر, &ea, cctx);
		    وگرنه
			// heredoc lines have been concatenated with NL
			// characters in get_function_body()
			سطر = compile_script(سطر, cctx);
		    بشکن;

	    حالت CMD_vim9script:
		    اگر (cctx->ctx_skip != SKIP_YES)
		    {
			emsg(_(e_vim9script_can_only_be_used_in_script));
			بازگشت FAIL;
		    }
		    سطر = (char_u *)"";
		    بشکن;

	    حالت CMD_class:
		    emsg(_(e_class_can_only_be_used_in_script));
		    بازگشت FAIL;

	    حالت CMD_enum:
		    emsg(_(e_enum_can_only_be_used_in_script));
		    بازگشت FAIL;

	    حالت CMD_interface:
		    emsg(_(e_interface_can_only_be_used_in_script));
		    بازگشت FAIL;

	    حالت CMD_type:
		    emsg(_(e_type_can_only_be_used_in_script));
		    بازگشت FAIL;

	    حالت CMD_global:
		    اگر (check_global_and_subst(ea.cmd, p) == FAIL)
			بازگشت FAIL;
		    // FALLTHROUGH
	    پیشفرض:
		    // Not recognized, execute with do_cmdline_cmd().
		    ea.arg = p;
		    سطر = compile_exec(سطر, &ea, cctx);
		    بشکن;
	}
nextline:
	اگر (سطر == پوچ)
	    بازگشت FAIL;
	سطر = skipwhite(سطر);

	// Undo any command modifiers.
	generate_undo_cmdmods(cctx);

	اگر (cctx->ctx_type_پشته.ga_len < 0)
	{
	    iemsg("Type stack underflow");
	    بازگشت FAIL;
	}
    } // END of the loop over all the function body lines.

    بازگشت OK;
}

/*
 * Returns TRUE if the end of a scope (if, while, for, block) is missing.
 * Called after compiling a def function body.
 */
    ایستا صحیح
compile_dfunc_scope_end_missing(cctx_T *cctx)
{
    اگر (cctx->ctx_scope == پوچ)
	بازگشت FALSE;

    انتخاب (cctx->ctx_scope->se_type)
    {
	حالت IF_SCOPE:
	    emsg(_(e_missing_endif));
	    بشکن;
	حالت WHILE_SCOPE:
	    emsg(_(e_missing_endwhile));
	    بشکن;
	حالت FOR_SCOPE:
	    emsg(_(e_missing_endfor));
	    بشکن;
	حالت TRY_SCOPE:
	    emsg(_(e_missing_endtry));
	    بشکن;
	حالت BLOCK_SCOPE:
	    // end block scope from :try (maybe)
	    compile_endblock(cctx);
	    اگر (cctx->ctx_scope != پوچ
		    && cctx->ctx_scope->se_type == TRY_SCOPE)
		emsg(_(e_missing_endtry));
	    وگرنه
		emsg(_(e_missing_rcurly));
	    بشکن;
	پیشفرض:
	    emsg(_(e_missing_rcurly));
    }
    بازگشت TRUE;
}

/*
 * When compiling a def function, if it doesn't have an explicit return
 * statement, then generate a default return instruction.  For an object
 * constructor, return the object.
 */
    ایستا صحیح
compile_dfunc_generate_default_return(ufunc_T *ufunc, cctx_T *cctx)
{
    // TODO: if a function ends in "throw" but there was a return elsewhere we
    // should not assume the return type is "void".
    اگر (cctx->ctx_had_return || cctx->ctx_had_throw)
	بازگشت OK;

    اگر (ufunc->uf_ret_type->tt_type == VAR_UNKNOWN)
	ufunc->uf_ret_type = &t_void;
    وگرنه اگر (ufunc->uf_ret_type->tt_type != VAR_VOID
					&& !IS_CONSTRUCTOR_METHOD(ufunc))
    {
	emsg(_(e_missing_return_statement));
	بازگشت FAIL;
    }

    // Return void if there is no return at the end.
    // For a constructor return the object.
    اگر (IS_CONSTRUCTOR_METHOD(ufunc))
    {
	generate_instr(cctx, ISN_RETURN_OBJECT);
	ufunc->uf_ret_type = &ufunc->uf_class->class_object_type;
    }
    وگرنه
	generate_instr(cctx, ISN_RETURN_VOID);

    بازگشت OK;
}

/*
 * Perform the chores after successfully compiling a def function.
 */
    ایستا تهی
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
    اگر (cctx->ctx_compile_type == CT_PROFILE)
    {
	dfunc->df_instr_prof = instr->ga_data;
	dfunc->df_instr_prof_شمار = instr->ga_len;
    }
    وگرنه
#endif
	اگر (cctx->ctx_compile_type == CT_DEBUG)
	{
	    dfunc->df_instr_debug = instr->ga_data;
	    dfunc->df_instr_debug_شمار = instr->ga_len;
	}
	وگرنه
	{
	    dfunc->df_instr = instr->ga_data;
	    dfunc->df_instr_شمار = instr->ga_len;
	}
    dfunc->df_varcount = dfunc->df_var_names.ga_len;
    dfunc->df_has_closure = cctx->ctx_has_closure;

    اگر (cctx->ctx_outer_used)
    {
	ufunc->uf_flags |= FC_CLOSURE;
	اگر (outer_cctx != پوچ)
	    ++outer_cctx->ctx_closure_شمار;
    }

    ufunc->uf_def_status = UF_COMPILED;
}

/*
 * Perform the cleanup when a def function compilation fails.
 */
    ایستا تهی
compile_dfunc_ufunc_cleanup(
    ufunc_T	*ufunc,
    garray_T	*instr,
    صحیح		new_def_function,
    نویسه	*errormsg,
    صحیح		did_emsg_before,
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
    اگر (!dfunc->df_deleted && new_def_function
			&& ufunc->uf_dfunc_idx == def_functions.ga_len - 1)
    {
	--def_functions.ga_len;
	ufunc->uf_dfunc_idx = 0;
    }
    ufunc->uf_def_status = UF_COMPILE_ERROR;

    وقتی (cctx->ctx_scope != پوچ)
	drop_scope(cctx);

    اگر (errormsg != پوچ)
	emsg(errormsg);
    وگرنه اگر (did_emsg == did_emsg_before)
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
    صحیح
compile_def_function(
	ufunc_T		*ufunc,
	صحیح		check_return_type,
	compiletype_T   compile_type,
	cctx_T		*outer_cctx)
{
    garray_T	سطرها_to_free;
    نویسه	*errormsg = پوچ;	// error message
    cctx_T	cctx;
    garray_T	*instr;
    صحیح		did_emsg_before = did_emsg;
    صحیح		did_emsg_silent_before = did_emsg_silent;
    صحیح		ret = FAIL;
    sctx_T	save_current_sctx = current_sctx;
    صحیح		save_estack_compiling = estack_compiling;
    صحیح		save_cmod_flags = cmdmod.cmod_flags;
    صحیح		do_estack_افزودن;
    صحیح		new_def_function = FALSE;

    // allocated lines are freed at the end
    ga_init2(&سطرها_to_free, اندازه(char_u *), 50);

    // Initialize the ufunc and the compilation context
    اگر (compile_dfunc_ufunc_init(ufunc, outer_cctx, compile_type,
						&new_def_function) == FAIL)
	بازگشت FAIL;

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
    do_estack_افزودن = !estack_بالاترین_is_ufunc(ufunc, 1);
    اگر (do_estack_افزودن)
	estack_افزودن_ufunc(ufunc, 1);
    estack_compiling = TRUE;

    // Make sure arguments don't shadow variables in the context
    اگر (check_args_shadowing(ufunc, &cctx) == FAIL)
	برو erret;

    // For an object method and a constructor generate instructions to
    // initialize "this" and the object variables.
    اگر (ufunc->uf_flags & (FC_OBJECT|FC_NEW))
	اگر (obj_method_prologue(ufunc, &cctx) == FAIL)
	    برو erret;

    اگر (ufunc->uf_def_args.ga_len > 0)
	اگر (compile_def_function_default_args(ufunc, instr, &cctx) == FAIL)
	    برو erret;
    ufunc->uf_args_visible = ufunc->uf_args.ga_len;

    // Compiling an abstract method or a function in an interface is done to
    // get the function type.  No code is actually compiled.
    اگر (ufunc->uf_class != پوچ && (IS_INTERFACE(ufunc->uf_class)
						|| IS_ABSTRACT_METHOD(ufunc)))
    {
	ufunc->uf_def_status = UF_NOT_COMPILED;
	ret = OK;
	برو erret;
    }

    // compile the function body
    اگر (compile_def_function_body(ufunc->uf_سطرها.ga_len, check_return_type,
				&سطرها_to_free, &errormsg, &cctx) == FAIL)
	برو erret;

    اگر (compile_dfunc_scope_end_missing(&cctx))
	برو erret;

    اگر (compile_dfunc_generate_default_return(ufunc, &cctx) == FAIL)
	برو erret;

    // When compiled with ":silent!" and there was an error don't consider the
    // function compiled.
    اگر (emsg_silent == 0 || did_emsg_silent == did_emsg_silent_before)
	compile_dfunc_epilogue(outer_cctx, ufunc, instr, &cctx);

    ret = OK;

erret:
    اگر (ufunc->uf_def_status == UF_COMPILING)
    {
	// compilation failed. do cleanup.
	compile_dfunc_ufunc_cleanup(ufunc, instr, new_def_function,
				    errormsg, did_emsg_before, &cctx);
    }

    اگر (cctx.ctx_redir_lhs.lhs_name != پوچ)
    {
	اگر (ret == OK)
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
    اگر (do_estack_افزودن)
	estack_برداشت();

    ga_clear_strings(&سطرها_to_free);
    free_locals(&cctx);
    ga_clear(&cctx.ctx_type_پشته);
    بازگشت ret;
}

    تهی
set_function_type(ufunc_T *ufunc)
{
    صحیح varargs = ufunc->uf_va_name != پوچ;
    صحیح argcount = ufunc->uf_args.ga_len;

    // Create a type for the function, with the return type and any
    // argument types.
    // A vararg is included in uf_args.ga_len but not in uf_arg_types.
    // The type is included in "tt_args".
    اگر (argcount > 0 || varargs)
    {
	اگر (ufunc->uf_type_list.ga_itemsize == 0)
	    ga_init2(&ufunc->uf_type_list, اندازه(type_T *), 10);
	ufunc->uf_func_type = alloc_func_type(ufunc->uf_ret_type,
					   argcount, &ufunc->uf_type_list);
	// Add argument types to the function type.
	اگر (func_type_add_arg_types(ufunc->uf_func_type,
				    argcount + varargs,
				    &ufunc->uf_type_list) == FAIL)
	    بازگشت;
	ufunc->uf_func_type->tt_argcount = argcount + varargs;
	ufunc->uf_func_type->tt_min_argcount =
				      argcount - ufunc->uf_def_args.ga_len;
	اگر (ufunc->uf_arg_types == پوچ)
	{
	    صحیح i;

	    // lambda does not have argument types.
	    برای (i = 0; i < argcount; ++i)
		ufunc->uf_func_type->tt_args[i] = &t_any;
	}
	وگرنه
	    mch_memmove(ufunc->uf_func_type->tt_args,
			 ufunc->uf_arg_types, اندازه(type_T *) * argcount);
	اگر (varargs)
	{
	    ufunc->uf_func_type->tt_args[argcount] =
		   ufunc->uf_va_type == پوچ ? &t_list_any : ufunc->uf_va_type;
	    ufunc->uf_func_type->tt_flags = TTFLAG_VARARGS;
	}
    }
    وگرنه
	// No arguments, can use a predefined type.
	ufunc->uf_func_type = get_func_type(ufunc->uf_ret_type,
					   argcount, &ufunc->uf_type_list);
}

/*
 * Free all instructions for "dfunc" except df_name.
 */
    ایستا تهی
delete_def_function_contents(dfunc_T *dfunc, صحیح mark_deleted)
{
    صحیح idx;

    // In same cases the instructions may refer to a class in which the
    // function is defined and unreferencing the class may call back here
    // recursively.  Set the df_delete_busy to avoid problems.
    اگر (dfunc->df_delete_busy)
	بازگشت;
    dfunc->df_delete_busy = TRUE;

    ga_clear(&dfunc->df_def_args_isn);
    ga_clear_strings(&dfunc->df_var_names);

    اگر (dfunc->df_instr != پوچ)
    {
	برای (idx = 0; idx < dfunc->df_instr_شمار; ++idx)
	    delete_instr(dfunc->df_instr + idx);
	VIM_CLEAR(dfunc->df_instr);
    }
    اگر (dfunc->df_instr_debug != پوچ)
    {
	برای (idx = 0; idx < dfunc->df_instr_debug_شمار; ++idx)
	    delete_instr(dfunc->df_instr_debug + idx);
	VIM_CLEAR(dfunc->df_instr_debug);
    }
#ifdef FEAT_PROFILE
    اگر (dfunc->df_instr_prof != پوچ)
    {
	برای (idx = 0; idx < dfunc->df_instr_prof_شمار; ++idx)
	    delete_instr(dfunc->df_instr_prof + idx);
	VIM_CLEAR(dfunc->df_instr_prof);
    }
#endif

    اگر (mark_deleted)
	dfunc->df_deleted = TRUE;
    اگر (dfunc->df_ufunc != پوچ)
	dfunc->df_ufunc->uf_def_status = UF_NOT_COMPILED;

    dfunc->df_delete_busy = FALSE;
}

/*
 * When a user function is deleted, clear the contents of any associated def
 * function, unless another user function still uses it.
 * The position in def_functions can be re-used.
 */
    تهی
unlink_def_function(ufunc_T *ufunc)
{
    اگر (ufunc->uf_dfunc_idx <= 0 || def_functions.ga_data == پوچ)
	بازگشت;

    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data)
	+ ufunc->uf_dfunc_idx;

    اگر (--dfunc->df_refcount <= 0)
	delete_def_function_contents(dfunc, TRUE);
    ufunc->uf_def_status = UF_NOT_COMPILED;
    ufunc->uf_dfunc_idx = 0;
    اگر (dfunc->df_ufunc == ufunc)
	dfunc->df_ufunc = پوچ;
}

/*
 * Used when a user function refers to an existing dfunc.
 */
    تهی
link_def_function(ufunc_T *ufunc)
{
    اگر (ufunc->uf_dfunc_idx <= 0)
	بازگشت;

    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data)
	+ ufunc->uf_dfunc_idx;

    ++dfunc->df_refcount;
}

#اگر defined(EXITFREE)
/*
 * Free all functions defined with ":def".
 */
    تهی
free_def_functions(تهی)
{
    صحیح idx;

    برای (idx = 0; idx < def_functions.ga_len; ++idx)
    {
	dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data) + idx;

	delete_def_function_contents(dfunc, TRUE);
	vim_free(dfunc->df_name);
    }

    ga_clear(&def_functions);
}
#endif


#endif // FEAT_EVAL
