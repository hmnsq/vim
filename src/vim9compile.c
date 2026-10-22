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
#અન્તર્ભાવ "vim.h"

#જો defined(FEAT_EVAL)

// Functions defined with :def are stored in this growarray.
// They are never removed, so that they can be found by index.
// Deleted functions have the df_deleted flag set.
garray_T def_functions = {0, 0, કદ(dfunc_T), 50, ખાલી};

સ્થિર શૂન્ય delete_def_function_contents(dfunc_T *dfunc, પૂર્ણાંક mark_deleted);

/*
 * Lookup variable "name" in the local scope and return it in "lvar".
 * "lvar->lv_from_outer" is incremented accordingly.
 * If "lvar" is NULL only check if the variable can be found.
 * Return FAIL if not found.
 */
    પૂર્ણાંક
lookup_local(char_u *name, size_t len, lvar_T *lvar, cctx_T *cctx)
{
    પૂર્ણાંક	    idx;
    lvar_T  *lvp;

    જો (len == 0)
	પાછા FAIL;

    જો (((len == 4 && STRNCMP(name, "this", 4) == 0)
		|| (len == 5 && STRNCMP(name, "super", 5) == 0))
	    && cctx->ctx_ufunc != ખાલી
	    && (cctx->ctx_ufunc->uf_flags & (FC_OBJECT|FC_NEW)))
    {
	પૂર્ણાંક is_super = *name == 's';
	જો (is_super)
	{
	    જો (name[5] != '.')
	    {
		emsg(_(e_super_must_be_followed_by_dot));
		પાછા FAIL;
	    }
	    જો (cctx->ctx_ufunc->uf_class != ખાલી
		    && cctx->ctx_ufunc->uf_class->class_extends == ખાલી)
	    {
		emsg(_(e_using_super_not_in_child_class));
		પાછા FAIL;
	    }
	}
	જો (lvar != ખાલી)
	{
	    CLEAR_POINTER(lvar);
	    lvar->lv_loop_depth = -1;
	    lvar->lv_name = (char_u *)(is_super ? "super" : "this");
	    જો (cctx->ctx_ufunc->uf_class != ખાલી)
	    {
		lvar->lv_type = &cctx->ctx_ufunc->uf_class->class_object_type;
		જો (is_super)
		{
		    type_T *type = get_type_ptr(cctx->ctx_type_list);

		    જો (type != ખાલી)
		    {
			*type = *lvar->lv_type;
			lvar->lv_type = type;
			type->tt_flags |= TTFLAG_SUPER;
		    }
		}
	    }
	}
	પાછા OK;
    }

    // Find local in current function scope.
    માટે (idx = 0; idx < cctx->ctx_locals.ga_len; ++idx)
    {
	lvp = ((lvar_T *)cctx->ctx_locals.ga_data) + idx;
	જો (lvp->lv_name != ખાલી
		&& STRNCMP(name, lvp->lv_name, len) == 0
					       && STRLEN(lvp->lv_name) == len)
	{
	    જો (lvar != ખાલી)
	    {
		*lvar = *lvp;
		lvar->lv_from_outer = 0;
		// If the variable was declared inside a loop set
		// lvar->lv_loop_idx and lvar->lv_loop_depth.
		get_loop_var_idx(cctx, idx, lvar);
	    }
	    પાછા OK;
	}
    }

    // Find local in outer function scope.
    જો (cctx->ctx_outer != ખાલી)
    {
	જો (lookup_local(name, len, lvar, cctx->ctx_outer) == OK)
	{
	    જો (lvar != ખાલી)
	    {
		cctx->ctx_outer_used = TRUE;
		++lvar->lv_from_outer;
	    }
	    પાછા OK;
	}
    }

    પાછા FAIL;
}

/*
 * Lookup an argument in the current function and an enclosing function.
 * Returns the argument index in "idxp"
 * Returns the argument type in "type"
 * Sets "gen_load_outer" to TRUE if found in outer scope.
 * Returns OK when found, FAIL otherwise.
 */
    પૂર્ણાંક
arg_exists(
	char_u	*name,
	size_t	len,
	પૂર્ણાંક	*idxp,
	type_T	**type,
	પૂર્ણાંક	*gen_load_outer,
	cctx_T	*cctx)
{
    પૂર્ણાંક	    idx;
    char_u  *va_name;

    જો (len == 0)
	પાછા FAIL;
    માટે (idx = 0; idx < cctx->ctx_ufunc->uf_args_visible; ++idx)
    {
	char_u *arg = FUNCARG(cctx->ctx_ufunc, idx);

	જો (STRNCMP(name, arg, len) == 0 && arg[len] == NUL)
	{
	    જો (idxp != ખાલી)
	    {
		// Arguments are located above the frame pointer.  One further
		// if there is a vararg argument
		*idxp = idx - (cctx->ctx_ufunc->uf_args.ga_len
							    + ઢગલો_FRAME_SIZE)
			      + (cctx->ctx_ufunc->uf_va_name != ખાલી ? -1 : 0);

		જો (cctx->ctx_ufunc->uf_arg_types != ખાલી)
		    *type = cctx->ctx_ufunc->uf_arg_types[idx];
		નહીંતર
		    *type = &t_any;
	    }
	    પાછા OK;
	}
    }

    va_name = cctx->ctx_ufunc->uf_va_name;
    જો (va_name != ખાલી
		    && STRNCMP(name, va_name, len) == 0 && va_name[len] == NUL)
    {
	જો (idxp != ખાલી)
	{
	    // varargs is always the last argument
	    *idxp = -ઢગલો_FRAME_SIZE - 1;
	    *type = cctx->ctx_ufunc->uf_va_type;
	}
	પાછા OK;
    }

    જો (cctx->ctx_outer != ખાલી)
    {
	// Lookup the name for an argument of the outer function.
	જો (arg_exists(name, len, idxp, type, gen_load_outer, cctx->ctx_outer)
									 == OK)
	{
	    જો (gen_load_outer != ખાલી)
	    {
		++*gen_load_outer;
		cctx->ctx_outer_used = TRUE;
	    }
	    પાછા OK;
	}
    }

    પાછા FAIL;
}

/*
 * Lookup a script-local variable in the current script, possibly defined in a
 * block that contains the function "cctx->ctx_ufunc".
 * "cctx" is NULL at the script level, "cstack" is NULL in a function.
 * If "len" is <= 0 "name" must be NUL terminated.
 * Return NULL when not found.
 */
    સ્થિર sallvar_T *
find_script_var(char_u *name, size_t len, cctx_T *cctx, cstack_T *cstack)
{
    scriptitem_T    *si = SCRIPT_ITEM(current_sctx.sc_sid);
    hashitem_T	    *hi;
    પૂર્ણાંક		    cc;
    sallvar_T	    *sav;
    ufunc_T	    *ufunc;

    // Find the list of all script variables with the right name.
    જો (len > 0)
    {
	cc = name[len];
	name[len] = NUL;
    }
    hi = hash_find(&si->sn_all_vars.dv_hashtab, name);
    જો (len > 0)
	name[len] = cc;
    જો (HASHITEM_EMPTY(hi))
	પાછા ખાલી;

    sav = HI2SAV(hi);
    જો (sav->sav_block_id == 0)
	// variable defined in the top script scope is always visible
	પાછા sav;

    જો (cctx == ખાલી)
    {
	જો (cstack == ખાલી)
	    પાછા ખાલી;

	// Not in a function scope, find variable with block ID equal to or
	// smaller than the current block id.  Use "cstack" to go up the block
	// scopes.
	જ્યારે (sav != ખાલી)
	{
	    પૂર્ણાંક idx;

	    માટે (idx = cstack->cs_idx; idx >= 0; --idx)
		જો (cstack->cs_block_id[idx] == sav->sav_block_id)
		    તોડો;
	    જો (idx >= 0)
		તોડો;
	    sav = sav->sav_next;
	}
	પાછા sav;
    }

    // Go over the variables with this name and find one that was visible
    // from the function.
    ufunc = cctx->ctx_ufunc;
    જ્યારે (sav != ખાલી)
    {
	પૂર્ણાંક idx;

	જો (ufunc->uf_block_depth == 0 && sav->sav_block_id == 0)
	    // If the function was defined at the script level (not inside a
	    // block), script-scope variables are always visible.
	    પાછા sav;

	// Go over the blocks that this function was defined in.  If the
	// variable block ID matches it was visible to the function.
	માટે (idx = 0; idx < ufunc->uf_block_depth; ++idx)
	    જો (ufunc->uf_block_ids[idx] == sav->sav_block_id)
		પાછા sav;
	sav = sav->sav_next;
    }

    // Not found, variable was not visible.
    પાછા ખાલી;
}

/*
 * If "name" can be found in the current script set it's "block_id".
 */
    શૂન્ય
update_script_var_block_id(char_u *name, પૂર્ણાંક block_id)
{
    scriptitem_T    *si = SCRIPT_ITEM(current_sctx.sc_sid);
    hashitem_T	    *hi;
    sallvar_T	    *sav;

    hi = hash_find(&si->sn_all_vars.dv_hashtab, name);
    જો (HASHITEM_EMPTY(hi))
	પાછા;
    sav = HI2SAV(hi);
    sav->sav_block_id = block_id;
}

/*
 * Lookup a variable (without s: prefix) in the current script.
 * "cctx" is NULL at the script level, "cstack" is NULL in a function.
 * Returns OK or FAIL.
 */
    પૂર્ણાંક
script_var_exists(char_u *name, size_t len, cctx_T *cctx, cstack_T *cstack)
{
    જો (current_sctx.sc_sid <= 0)
	પાછા FAIL;
    જો (current_script_is_vim9())
    {
	// Check script variables that were visible where the function was
	// defined.
	જો (find_script_var(name, len, cctx, cstack) != ખાલી)
	    પાછા OK;
    }
    નહીંતર
    {
	hashtab_T	*ht = &SCRIPT_VARS(current_sctx.sc_sid);
	dictitem_T	*di;
	પૂર્ણાંક		cc;

	// Check script variables that are currently visible
	cc = name[len];
	name[len] = NUL;
	di = find_var_in_ht(ht, 0, name, TRUE);
	name[len] = cc;
	જો (di != ખાલી)
	    પાછા OK;
    }

    પાછા FAIL;
}

/*
 * Returns the index of a class method or class variable with name "name"
 * accessible in the currently compiled function.
 * If "cl_ret" is not NULL set it to the class.
 * Otherwise return -1.
 */
    સ્થિર પૂર્ણાંક
cctx_class_midx(
    cctx_T  *cctx,
    પૂર્ણાંક	    is_method,
    char_u  *name,
    size_t  len,
    class_T **cl_ret)
{
    જો (cctx == ખાલી || cctx->ctx_ufunc == ખાલી
	    || cctx->ctx_ufunc->uf_class == ખાલી
	    || cctx->ctx_ufunc->uf_defclass == ખાલી)
	પાછા -1;

    // Search for the class method or variable in the class where the calling
    // function is defined.
    class_T *cl = cctx->ctx_ufunc->uf_defclass;
    પૂર્ણાંક m_idx = is_method ? class_method_idx(cl, name, len)
					: class_member_idx(cl, name, len);
    જો (m_idx < 0)
    {
	cl = cl->class_extends;
	જ્યારે (cl != ખાલી)
	{
	    m_idx = is_method ? class_method_idx(cl, name, len)
					: class_member_idx(cl, name, len);
	    જો (m_idx >= 0)
		તોડો;
	    cl = cl->class_extends;
	}
    }

    જો (m_idx >= 0)
    {
	જો (cl_ret != ખાલી)
	    *cl_ret = cl;
    }

    પાછા m_idx;
}

/*
 * Returns the index of a class method with name "name" accessible in the
 * currently compiled function.  Returns -1 if not found.  The class where the
 * method is defined is returned in "cl_ret".
 */
    પૂર્ણાંક
cctx_class_method_idx(
    cctx_T  *cctx,
    char_u  *name,
    size_t  len,
    class_T **cl_ret)
{
    પાછા cctx_class_midx(cctx, TRUE, name, len, cl_ret);
}

/*
 * Returns the index of a class variable with name "name" accessible in the
 * currently compiled function.  Returns -1 if not found.  The class where the
 * variable is defined is returned in "cl_ret".
 */
    પૂર્ણાંક
cctx_class_member_idx(
    cctx_T  *cctx,
    char_u  *name,
    size_t  len,
    class_T **cl_ret)
{
    પાછા cctx_class_midx(cctx, FALSE, name, len, cl_ret);
}

/*
 * Return TRUE if "name" is a local variable, argument, script variable or
 * imported.  Also if "name" is "this" and in a class method.
 */
    સ્થિર પૂર્ણાંક
variable_exists(char_u *name, size_t len, cctx_T *cctx)
{
    પાછા (cctx != ખાલી
		&& (lookup_local(name, len, ખાલી, cctx) == OK
		    || arg_exists(name, len, ખાલી, ખાલી, ખાલી, cctx) == OK
		    || (len == 4
			&& cctx->ctx_ufunc != ખાલી
			&& (cctx->ctx_ufunc->uf_flags & (FC_OBJECT|FC_NEW))
			&& STRNCMP(name, "this", 4) == 0)))
	    || script_var_exists(name, len, cctx, ખાલી) == OK
	    || cctx_class_member_idx(cctx, name, len, ખાલી) >= 0
	    || find_imported(name, len, FALSE) != ખાલી;
}

/*
 * Return TRUE if "name" is a local variable, argument, script variable,
 * imported or function.  Or commands are being skipped, a declaration may have
 * been skipped then.
 */
    સ્થિર પૂર્ણાંક
item_exists(char_u *name, size_t len, પૂર્ણાંક cmd UNUSED, cctx_T *cctx)
{
    પાછા variable_exists(name, len, cctx);
}

/*
 * Check if "p[len]" is already defined, either in script "import_sid" or in
 * compilation context "cctx".
 * "cctx" is NULL at the script level, "cstack" is NULL in a function.
 * Does not check the global namespace.
 * If "is_arg" is TRUE the error message is for an argument name.
 * Return FAIL and give an error if it defined.
 */
    પૂર્ણાંક
check_defined(
	char_u	    *p,
	size_t	    len,
	cctx_T	    *cctx,
	cstack_T    *cstack,
	પૂર્ણાંક	    is_arg)
{
    પૂર્ણાંક		c = p[len];
    ufunc_T	*ufunc = ખાલી;

    // underscore argument is OK
    જો (len == 1 && *p == '_')
	પાછા OK;

    જો (script_var_exists(p, len, cctx, cstack) == OK)
    {
	જો (is_arg)
	    semsg(_(e_argument_already_declared_in_script_str), p);
	નહીંતર
	    semsg(_(e_variable_already_declared_in_script_str), p);
	પાછા FAIL;
    }

    જો (cctx_class_member_idx(cctx, p, len, ખાલી) >= 0)
    {
	જો (is_arg)
	    semsg(_(e_argument_already_declared_in_class_str), p);
	નહીંતર
	    semsg(_(e_variable_already_declared_in_class_str), p);
	પાછા FAIL;
    }

    p[len] = NUL;
    જો ((cctx != ખાલી
		&& (lookup_local(p, len, ખાલી, cctx) == OK
		    || arg_exists(p, len, ખાલી, ખાલી, ખાલી, cctx) == OK))
	    || find_imported(p, len, FALSE) != ખાલી
	    || (ufunc = find_func_even_dead(p, 0)) != ખાલી)
    {
	// A local or script-local function can shadow a global function.
	જો (ufunc == ખાલી || ((ufunc->uf_flags & FC_DEAD) == 0
		    && (!func_is_global(ufunc)
					     || (p[0] == 'g' && p[1] == ':'))))
	{
	    જો (is_arg)
		semsg(_(e_argument_name_shadows_existing_variable_str), p);
	    નહીંતર
		semsg(_(e_name_already_defined_str), p);
	    p[len] = c;
	    પાછા FAIL;
	}
    }
    p[len] = c;
    પાછા OK;
}


/*
 * Return TRUE if "actual" could be "expected" and a runtime typecheck is to be
 * used.  Return FALSE if the types will never match.
 */
    સ્થિર પૂર્ણાંક
use_typecheck(type_T *actual, type_T *expected)
{
    જો (actual->tt_type == VAR_ANY
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
	પાછા TRUE;
    જો (actual->tt_type == VAR_OBJECT && expected->tt_type == VAR_OBJECT)
	પાછા TRUE;
    જો ((actual->tt_type == VAR_LIST
		|| actual->tt_type == VAR_TUPLE
		|| actual->tt_type == VAR_DICT)
	    && actual->tt_type == expected->tt_type)
	// This takes care of a nested list or dict.
	પાછા use_typecheck(actual->tt_member, expected->tt_member);
    પાછા FALSE;
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
    પૂર્ણાંક
need_type_where(
	type_T	*actual,
	type_T	*expected,
	પૂર્ણાંક	typechk_flags,	// acceptable types (type check flags)
	પૂર્ણાંક	offset,
	where_T	where,
	cctx_T	*cctx,
	પૂર્ણાંક	silent,
	પૂર્ણાંક	actual_is_const)
{
    પૂર્ણાંક ret;

    જો (expected->tt_type != VAR_CLASS && expected->tt_type != VAR_TYPEALIAS)
    {
	જો (check_type_is_કિંમત(actual) == FAIL)
	    પાછા FAIL;
    }

    જો (expected == &t_bool && actual != &t_bool
					&& (actual->tt_flags & TTFLAG_BOOL_OK))
    {
	// Using "0", "1" or the result of an expression with "&&" or "||" as a
	// boolean is OK but requires a conversion.
	generate_2BOOL(cctx, FALSE, offset);
	પાછા OK;
    }

    ret = check_type_maybe(expected, actual, FALSE, where);
    જો (ret == OK)
	પાછા OK;

    // If actual a constant a runtime check makes no sense.  If it's
    // null_function it is OK.
    જો (actual_is_const && ret == MAYBE && actual == &t_func_unknown)
	પાછા OK;

    // If the actual type can be the expected type add a runtime check.
    જો (!actual_is_const && ret == MAYBE && use_typecheck(actual, expected))
    {
	generate_TYPECHECK(cctx, expected, typechk_flags, offset,
		where.wt_kind == WT_VARIABLE, where.wt_index);
	પાછા OK;
    }

    જો (!silent)
	type_mismatch_where(expected, actual, where);
    પાછા FAIL;
}

    પૂર્ણાંક
need_type(
	type_T	*actual,
	type_T	*expected,
	પૂર્ણાંક	typechk_flags,	// acceptable types (type check flags)
	પૂર્ણાંક	offset,
	પૂર્ણાંક	arg_idx,
	cctx_T	*cctx,
	પૂર્ણાંક	silent,
	પૂર્ણાંક	actual_is_const)
{
    where_T where = WHERE_INIT;

    જો (arg_idx > 0)
    {
	where.wt_index = arg_idx;
	where.wt_kind = WT_ARGUMENT;
    }
    પાછા need_type_where(actual, expected, typechk_flags, offset, where,
						cctx, silent, actual_is_const);
}

/*
 * Set type of variable "lvar" to "type".  If the variable is a constant then
 * the type gets TTFLAG_CONST.
 */
    સ્થિર શૂન્ય
set_var_type(lvar_T *lvar, type_T *type_arg, cctx_T *cctx)
{
    type_T	*type = type_arg;

    જો (lvar->lv_const == ASSIGN_CONST && (type->tt_flags & TTFLAG_CONST) == 0)
    {
	જો (type->tt_flags & TTFLAG_STATIC)
	    // entry in static_types[] is followed by const type
	    type = type + 1;
	નહીંતર
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
	પૂર્ણાંક	assign,
	type_T	*type)
{
    lvar_T  *lvar;
    dfunc_T *dfunc;

    જો (arg_exists(name, len, ખાલી, ખાલી, ખાલી, cctx) == OK)
    {
	emsg_namelen(_(e_str_is_used_as_argument), name, (પૂર્ણાંક)len);
	પાછા ખાલી;
    }

    જો (GA_GROW_FAILS(&cctx->ctx_locals, 1))
	પાછા ખાલી;
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
    જો (type == &t_unknown || type == &t_any)
	// type not known yet, may be inferred from RHS
	lvar->lv_type = type;
    નહીંતર
	// may use TTFLAG_CONST
	set_var_type(lvar, type, cctx);

    // Remember the name for debugging.
    જો (GA_GROW_FAILS(&dfunc->df_var_names, 1))
	પાછા ખાલી;
    ((char_u **)dfunc->df_var_names.ga_data)[lvar->lv_idx] =
						    vim_strsave(lvar->lv_name);
    ++dfunc->df_var_names.ga_len;

    પાછા lvar;
}

/*
 * If "check_writable" is ASSIGN_CONST give an error if the variable was
 * defined with :final or :const, if "check_writable" is ASSIGN_FINAL give an
 * error if the variable was defined with :const.
 */
    સ્થિર પૂર્ણાંક
check_item_writable(svar_T *sv, પૂર્ણાંક check_writable, char_u *name)
{
    જો ((check_writable == ASSIGN_CONST && sv->sv_const != 0)
	    || (check_writable == ASSIGN_FINAL
					      && sv->sv_const == ASSIGN_CONST))
    {
	semsg(_(e_cannot_change_readonly_variable_str), name);
	પાછા FAIL;
    }
    પાછા OK;
}

/*
 * Find "name" in script-local items of script "sid".
 * Pass "check_writable" to check_item_writable().
 * "cctx" is NULL at the script level, "cstack" is NULL in a function.
 * Returns the index in "sn_var_vals" if found.
 * If found but not in "sn_var_vals" returns -1.
 * If not found or the variable is not writable returns -2.
 */
    પૂર્ણાંક
get_script_item_idx(
	પૂર્ણાંક	    sid,
	char_u	    *name,
	પૂર્ણાંક	    check_writable,
	cctx_T	    *cctx,
	cstack_T    *cstack)
{
    hashtab_T	    *ht;
    dictitem_T	    *di;
    scriptitem_T    *si = SCRIPT_ITEM(sid);
    svar_T	    *sv;
    પૂર્ણાંક		    idx;

    જો (!SCRIPT_ID_VALID(sid))
	પાછા -1;
    જો (sid == current_sctx.sc_sid)
    {
	sallvar_T *sav = find_script_var(name, 0, cctx, cstack);

	જો (sav == ખાલી)
	    પાછા -2;
	idx = sav->sav_var_vals_idx;
	sv = ((svar_T *)si->sn_var_vals.ga_data) + idx;
	જો (check_item_writable(sv, check_writable, name) == FAIL)
	    પાછા -2;
	પાછા idx;
    }

    // First look the name up in the hashtable.
    ht = &SCRIPT_VARS(sid);
    di = find_var_in_ht(ht, 0, name, TRUE);
    જો (di == ખાલી)
    {
	જો (si->sn_autoload_prefix != ખાલી)
	{
	    hashitem_T *hi;

	    // A variable exported from an autoload script is in the global
	    // variables, we can find it in the all_vars table.
	    hi = hash_find(&si->sn_all_vars.dv_hashtab, name);
	    જો (!HASHITEM_EMPTY(hi))
		પાછા HI2SAV(hi)->sav_var_vals_idx;
	}
	પાછા -2;
    }

    // Now find the svar_T index in sn_var_vals.
    માટે (idx = 0; idx < si->sn_var_vals.ga_len; ++idx)
    {
	sv = ((svar_T *)si->sn_var_vals.ga_data) + idx;
	જો (sv->sv_tv == &di->di_tv)
	{
	    જો (check_item_writable(sv, check_writable, name) == FAIL)
		પાછા -2;
	    પાછા idx;
	}
    }
    પાછા -1;
}

    સ્થિર imported_T *
find_imported_in_script(char_u *name, size_t len, પૂર્ણાંક sid)
{
    scriptitem_T    *si;
    પૂર્ણાંક		    idx;

    જો (!SCRIPT_ID_VALID(sid))
	પાછા ખાલી;
    si = SCRIPT_ITEM(sid);
    માટે (idx = 0; idx < si->sn_imports.ga_len; ++idx)
    {
	imported_T *import = ((imported_T *)si->sn_imports.ga_data) + idx;

	જો (len == 0 ? STRCMP(name, import->imp_name) == 0
		     : STRLEN(import->imp_name) == len
				  && STRNCMP(name, import->imp_name, len) == 0)
	    પાછા import;
    }
    પાછા ખાલી;
}

/*
 * Find "name" in imported items of the current script.
 * If "len" is 0 use any length that works.
 * If "load" is TRUE and the script was not loaded yet, load it now.
 */
    imported_T *
find_imported(char_u *name, size_t len, પૂર્ણાંક load)
{
    જો (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	પાછા ખાલી;

    // Skip over "s:" before "s:something" to find the import name.
    પૂર્ણાંક off = name[0] == 's' && name[1] == ':' ? 2 : 0;

    imported_T *ret = find_imported_in_script(name + off, len - off,
							  current_sctx.sc_sid);
    જો (ret != ખાલી && load && (ret->imp_flags & IMP_FLAGS_AUTOLOAD))
    {
	scid_T	actual_sid = 0;
	પૂર્ણાંક	save_emsg_off = emsg_off;

	// "emsg_off" will be set when evaluating an expression silently, but
	// we do want to know about errors in a script.  Also because it then
	// aborts when an error is encountered.
	emsg_off = FALSE;

	// script found before but not loaded yet
	ret->imp_flags &= ~IMP_FLAGS_AUTOLOAD;
	(શૂન્ય)do_source(SCRIPT_ITEM(ret->imp_sid)->sn_name, FALSE,
						       DOSO_NONE, &actual_sid);
	// If the script is a symlink it may be sourced with another name, may
	// need to adjust the script ID for that.
	જો (actual_sid != 0)
	    ret->imp_sid = actual_sid;

	emsg_off = save_emsg_off;
    }
    પાછા ret;
}

/*
 * Called when checking for a following operator at "arg".  When the rest of
 * the line is empty or only a comment, peek the next line.  If there is a next
 * line return a pointer to it and set "nextp".
 * Otherwise skip over white space.
 */
    char_u *
may_peek_next_લીટી(cctx_T *cctx, char_u *arg, char_u **nextp)
{
    char_u *p = skipwhite(arg);

    *nextp = ખાલી;
    જો (*p == NUL || (VIM_ISWHITE(*arg) && vim9_comment_start(p)))
    {
	*nextp = peek_next_લીટી_from_context(cctx);
	જો (*nextp != ખાલી)
	    પાછા *nextp;
    }
    પાછા p;
}

/*
 * Return a pointer to the next line that isn't empty or only contains a
 * comment. Skips over white space.
 * Returns NULL if there is none.
 */
    char_u *
peek_next_લીટી_from_context(cctx_T *cctx)
{
    પૂર્ણાંક lnum = cctx->ctx_lnum;

    જ્યારે (++lnum < cctx->ctx_ufunc->uf_લીટીઓ.ga_len)
    {
	char_u *લીટી = ((char_u **)cctx->ctx_ufunc->uf_લીટીઓ.ga_data)[lnum];
	char_u *p;

	// ignore NULLs inserted for continuation lines
	જો (લીટી != ખાલી)
	{
	    p = skipwhite(લીટી);
	    જો (vim9_bad_comment(p))
		પાછા ખાલી;
	    જો (*p != NUL && !vim9_comment_start(p))
		પાછા p;
	}
    }
    પાછા ખાલી;
}

/*
 * Get the next line of the function from "cctx".
 * Skips over empty lines.  Skips over comment lines if "skip_comment" is TRUE.
 * Returns NULL when at the end.
 */
    char_u *
next_લીટી_from_context(cctx_T *cctx, પૂર્ણાંક skip_comment)
{
    char_u	*લીટી;

    કરો
    {
	++cctx->ctx_lnum;
	જો (cctx->ctx_lnum >= cctx->ctx_ufunc->uf_લીટીઓ.ga_len)
	{
	    લીટી = ખાલી;
	    તોડો;
	}
	લીટી = ((char_u **)cctx->ctx_ufunc->uf_લીટીઓ.ga_data)[cctx->ctx_lnum];
	cctx->ctx_લીટી_start = લીટી;
	SOURCING_LNUM = cctx->ctx_lnum + 1;
    } જ્યારે (લીટી == ખાલી || *skipwhite(લીટી) == NUL
		     || (skip_comment && vim9_comment_start(skipwhite(લીટી))));
    પાછા લીટી;
}

/*
 * Skip over white space at "whitep" and assign to "*arg".
 * If "*arg" is at the end of the line, advance to the next line.
 * Also when "whitep" points to white space and "*arg" is on a "#".
 * Return FAIL if beyond the last line, "*arg" is unmodified then.
 */
    પૂર્ણાંક
may_get_next_લીટી(char_u *whitep, char_u **arg, cctx_T *cctx)
{
    *arg = skipwhite(whitep);
    જો (vim9_bad_comment(*arg))
	પાછા FAIL;
    જો (**arg == NUL || (VIM_ISWHITE(*whitep) && vim9_comment_start(*arg)))
    {
	char_u *next = next_લીટી_from_context(cctx, TRUE);

	જો (next == ખાલી)
	    પાછા FAIL;
	*arg = skipwhite(next);
    }
    પાછા OK;
}

/*
 * Idem, and give an error when failed.
 */
    પૂર્ણાંક
may_get_next_લીટી_error(char_u *whitep, char_u **arg, cctx_T *cctx)
{
    જો (may_get_next_લીટી(whitep, arg, cctx) == FAIL)
    {
	SOURCING_LNUM = cctx->ctx_lnum + 1;
	emsg(_(e_લીટી_incomplete));
	પાછા FAIL;
    }
    પાછા OK;
}

/*
 * Get a line from the compilation context, compatible with exarg_T getline().
 * Return a pointer to the line in allocated memory.
 * Return NULL for end-of-file or some error.
 */
    સ્થિર char_u *
exarg_getline(
	પૂર્ણાંક c UNUSED,
	શૂન્ય *cookie,
	પૂર્ણાંક indent UNUSED,
	getline_opt_T options UNUSED)
{
    cctx_T  *cctx = (cctx_T *)cookie;
    char_u  *p;

    માટે (;;)
    {
	જો (cctx->ctx_lnum >= cctx->ctx_ufunc->uf_લીટીઓ.ga_len - 1)
	    પાછા ખાલી;
	++cctx->ctx_lnum;
	p = ((char_u **)cctx->ctx_ufunc->uf_લીટીઓ.ga_data)[cctx->ctx_lnum];
	// Comment lines result in NULL pointers, skip them.
	જો (p != ખાલી)
	    પાછા vim_strsave(p);
    }
}

    શૂન્ય
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
    પૂર્ણાંક
func_needs_compiling(ufunc_T *ufunc, compiletype_T compile_type)
{
    પસંદગી (ufunc->uf_def_status)
    {
	સ્થિતિ UF_TO_BE_COMPILED:
	    પાછા TRUE;

	સ્થિતિ UF_COMPILED:
	{
	    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data)
							 + ufunc->uf_dfunc_idx;

	    પસંદગી (compile_type)
	    {
		સ્થિતિ CT_PROFILE:
#ifdef FEAT_PROFILE
		    પાછા dfunc->df_instr_prof == ખાલી;
#endif
		સ્થિતિ CT_NONE:
		    પાછા dfunc->df_instr == ખાલી;
		સ્થિતિ CT_DEBUG:
		    પાછા dfunc->df_instr_debug == ખાલી;
	    }
	}

	સ્થિતિ UF_NOT_COMPILED:
	સ્થિતિ UF_COMPILE_ERROR:
	સ્થિતિ UF_COMPILING:
	    તોડો;
    }
    પાછા FALSE;
}

/*
 * Compile a nested :def command.
 */
    સ્થિર char_u *
compile_nested_function(exarg_T *eap, cctx_T *cctx, garray_T *લીટીઓ_to_free)
{
    પૂર્ણાંક		is_global = *eap->arg == 'g' && eap->arg[1] == ':';
    char_u	*name_start = eap->arg;
    char_u	*name_end = to_name_end(eap->arg, TRUE);
    પૂર્ણાંક		off;
    char_u	*func_name;
    string_T	lambda_name;
    ufunc_T	*ufunc;
    પૂર્ણાંક		r = FAIL;
    compiletype_T   compile_type;
    પૂર્ણાંક		funcref_isn_idx = -1;
    lvar_T	*lvar = ખાલી;
    char_u	*bracket_start = ખાલી;

    જો (eap->forceit)
    {
	semsg(_(e_cannot_use_bang_with_nested_def_str),
		eap->cmdidx == CMD_def ? ":def" : ":function");
	પાછા ખાલી;
    }

    જો (*name_start == '/')
    {
	name_end = skip_regexp(name_start + 1, '/', TRUE);
	જો (*name_end == '/')
	    ++name_end;
	set_nextcmd(eap, name_end);
    }

    જો (*name_end == '<')
    {
	bracket_start = name_end;
	જો (skip_generic_func_type_args(&name_end) == FAIL)
	    પાછા ખાલી;
    }

    જો (name_end == name_start || *skipwhite(name_end) != '(')
    {
	જો (!ends_excmd2(name_start, name_end))
	{
	    જો (*skipwhite(name_end) == '.')
		semsg(_(e_cannot_define_dict_func_in_vim9_script_str),
								     eap->cmd);
	    નહીંતર
		semsg(_(e_invalid_command_str), eap->cmd);
	    પાછા ખાલી;
	}

	// "def" or "def Name": list functions
	જો (generate_DEF(cctx, name_start, name_end - name_start) == FAIL)
	    પાછા ખાલી;
	પાછા eap->nextcmd == ખાલી ? (char_u *)"" : eap->nextcmd;
    }

    જો (bracket_start != ખાલી)
	// generic function.  The function name ends before the list of types
	// (opening angle bracket).
	name_end = bracket_start;

    // Only g:Func() can use a namespace.
    જો (name_start[1] == ':' && !is_global)
    {
	semsg(_(e_namespace_not_supported_str), name_start);
	પાછા ખાલી;
    }
    જો (cctx->ctx_skip != SKIP_YES
	    && check_defined(name_start, name_end - name_start, cctx,
							  ખાલી, FALSE) == FAIL)
	પાછા ખાલી;
    જો (!ASCII_ISUPPER(is_global ? name_start[2] : name_start[0]))
    {
	semsg(_(e_function_name_must_start_with_capital_str), name_start);
	પાછા ખાલી;
    }

    eap->arg = name_end;
    fill_exarg_from_cctx(eap, cctx);

    eap->forceit = FALSE;
    // We use the special <lambda>99 name, but it's not really a lambda.
    lambda_name = get_lambda_name();
    lambda_name.string = vim_strnsave(lambda_name.string, lambda_name.length);
    જો (lambda_name.string == ખાલી)
	પાછા ખાલી;

    // This may free the current line, make a copy of the name.
    off = is_global ? 2 : 0;
    func_name = vim_strnsave(name_start + off, name_end - name_start - off);
    જો (func_name == ખાલી)
    {
	r = FAIL;
	જાઓ theend;
    }

    // A type variable cannot have the name of the function.  The function is
    // defined with the "<lambda>" name, check it here.
    જો (bracket_start != ખાલી)
    {
	size_t	len = STRLEN(func_name);

	માટે (char_u *t = bracket_start + 1; *t != '>' && *t != NUL; )
	{
	    char_u *te = to_name_end(t, FALSE);

	    જો ((size_t)(te - t) == len && STRNCMP(t, func_name, len) == 0)
	    {
		semsg(_(e_redefining_script_item_str), func_name);
		r = FAIL;
		જાઓ theend;
	    }
	    t = te == t ? t + 1 : te;
	}
    }

    // Make sure "KeyTyped" is not set, it may cause indent to be written.
    પૂર્ણાંક save_ચાવી_Typed = ચાવી_Typed;
    ચાવી_Typed = FALSE;

    ufunc = define_function(eap, lambda_name.string, લીટીઓ_to_free, 0, ખાલી, 0,
									cctx);

    ચાવી_Typed = save_ચાવી_Typed;

    જો (ufunc == ખાલી)
    {
	r = eap->skip ? OK : FAIL;
	જાઓ theend;
    }
    જો (eap->nextcmd != ખાલી)
    {
	semsg(_(e_text_found_after_str_str),
	      eap->cmdidx == CMD_def ? "enddef" : "endfunction", eap->nextcmd);
	r = FAIL;
	func_ptr_unref(ufunc);
	જાઓ theend;
    }

    // copy over the block scope IDs before compiling
    જો (!is_global && cctx->ctx_ufunc->uf_block_depth > 0)
    {
	પૂર્ણાંક block_depth = cctx->ctx_ufunc->uf_block_depth;

	ufunc->uf_block_ids = ALLOC_MULT(પૂર્ણાંક, block_depth);
	જો (ufunc->uf_block_ids != ખાલી)
	{
	    mch_memmove(ufunc->uf_block_ids, cctx->ctx_ufunc->uf_block_ids,
						    કદ(પૂર્ણાંક) * block_depth);
	    ufunc->uf_block_depth = block_depth;
	}
    }

    // Define the funcref before compiling, so that it is found by any
    // recursive call.
    જો (is_global)
    {
	r = generate_NEWFUNC(cctx, lambda_name.string, func_name);
	func_name = ખાલી;
	lambda_name.string = ખાલી;
	lambda_name.length = 0;
    }
    નહીંતર
    {
	// Define a local variable for the function reference.
	lvar = reserve_local(cctx, func_name, name_end - name_start,
					    ASSIGN_CONST, ufunc->uf_func_type);
	જો (lvar == ખાલી)
	{
	    func_ptr_unref(ufunc);
	    જાઓ theend;
	}
	lvar->lv_ufunc = ufunc;
	જો (generate_FUNCREF(cctx, ufunc, ખાલી, FALSE, 0, &funcref_isn_idx) == FAIL)
	{
	    func_ptr_unref(ufunc);
	    જાઓ theend;
	}
	r = generate_STORE(cctx, ISN_STORE, lvar->lv_idx, ખાલી);
    }

    compile_type = get_compile_type(ufunc);
#ifdef FEAT_PROFILE
    // If the outer function is profiled, also compile the nested function for
    // profiling.
    જો (cctx->ctx_compile_type == CT_PROFILE)
	compile_type = CT_PROFILE;
#endif
    જો (func_needs_compiling(ufunc, compile_type)
	    && compile_def_function(ufunc, TRUE, compile_type, cctx) == FAIL)
    {
	func_ptr_unref(ufunc);
	જો (lvar != ખાલી)
	    // Now the local variable can't be used.
	    *lvar->lv_name = '/';  // impossible value
	જાઓ theend;
    }

#ifdef FEAT_PROFILE
    // When the outer function is compiled for profiling, the nested function
    // may be called without profiling.  Compile it here in the right context.
    જો (compile_type == CT_PROFILE && func_needs_compiling(ufunc, CT_NONE))
	compile_def_function(ufunc, FALSE, CT_NONE, cctx);
#endif

    // If a FUNCREF instruction was generated, set the index after compiling.
    જો (funcref_isn_idx != -1 && ufunc->uf_def_status == UF_COMPILED)
    {
	isn_T	*funcref_isn = ((isn_T *)cctx->ctx_instr.ga_data) +
							funcref_isn_idx;
	funcref_isn->isn_arg.funcref.fr_dfunc_idx = ufunc->uf_dfunc_idx;
    }

theend:
    vim_free(lambda_name.string);
    vim_free(func_name);
    પાછા r == FAIL ? ખાલી : (char_u *)"";
}

/*
 * Compile one Vim expression {expr} in string "p".
 * "p" points to the opening "{".
 * Return a pointer to the character after "}", NULL for an error.
 */
    char_u *
compile_one_પદાવલિ_in_str(char_u *p, cctx_T *cctx)
{
    char_u	*block_start;
    char_u	*block_end;

    // Skip the opening {.
    block_start = skipwhite(p + 1);
    block_end = block_start;
    જો (*block_start != NUL && skip_પદાવલિ(&block_end, ખાલી) == FAIL)
	પાછા ખાલી;
    block_end = skipwhite(block_end);
    // The block must be closed by a }.
    જો (*block_end != '}')
    {
	semsg(_(e_missing_close_curly_str), p);
	પાછા ખાલી;
    }
    જો (compile_expr0(&block_start, cctx) == FAIL)
	પાછા ખાલી;
    may_generate_2STRING(-1, TOSTRING_INTERPOLATE, cctx);

    પાછા block_end + 1;
}

/*
 * Compile a string "str" (either containing a literal string or a mix of
 * literal strings and Vim expressions of the form `{expr}`).  This is used
 * when compiling a heredoc assignment to a variable or an interpolated string
 * in a Vim9 def function.  Vim9 instructions are generated to push strings,
 * evaluate expressions, concatenate them and create a list of lines.  When
 * "evalstr" is TRUE, Vim expressions in "str" are evaluated.
 */
    પૂર્ણાંક
compile_all_પદાવલિ_in_str(char_u *str, પૂર્ણાંક evalstr, cctx_T *cctx)
{
    char_u	*p = str;
    char_u	*val;
    પૂર્ણાંક		ગણતરી = 0;

    જો (cctx->ctx_skip == SKIP_YES)
	પાછા OK;

    જો (!evalstr || *str == NUL)
    {
	// Literal string, possibly empty.
	val = *str != NUL ? vim_strsave(str) : ખાલી;
	પાછા generate_PUSHS(cctx, &val);
    }

    // Push all the string pieces to the stack, followed by a ISN_CONCAT.
    જ્યારે (*p != NUL)
    {
	char_u	*lit_start;
	પૂર્ણાંક	escaped_brace = FALSE;

	// Look for a block start.
	lit_start = p;
	જ્યારે (*p != '{' && *p != '}' && *p != NUL)
	    ++p;

	જો (*p != NUL && *p == p[1])
	{
	    // Escaped brace, unescape and continue.
	    // Include the brace in the literal string.
	    ++p;
	    escaped_brace = TRUE;
	}
	નહીંતર જો (*p == '}')
	{
	    semsg(_(e_stray_closing_curly_str), str);
	    પાછા FAIL;
	}

	// Append the literal part.
	જો (p != lit_start)
	{
	    val = vim_strnsave(lit_start, (size_t)(p - lit_start));
	    જો (generate_PUSHS(cctx, &val) == FAIL)
		પાછા FAIL;
	    ++ગણતરી;
	}

	જો (*p == NUL)
	    તોડો;

	જો (escaped_brace)
	{
	    // Skip the second brace.
	    ++p;
	    ચાલુ;
	}

	p = compile_one_પદાવલિ_in_str(p, cctx);
	જો (p == ખાલી)
	    પાછા FAIL;
	++ગણતરી;
    }

    // Small optimization, if there's only a single piece skip the ISN_CONCAT.
    જો (ગણતરી > 1)
	પાછા generate_CONCAT(cctx, ગણતરી);

    પાછા OK;
}

/*
 * Return the length of an assignment operator, or zero if there isn't one.
 */
    પૂર્ણાંક
assignment_len(char_u *p, પૂર્ણાંક *heredoc)
{
    જો (*p == '=')
    {
	જો (p[1] == '<' && p[2] == '<')
	{
	    *heredoc = TRUE;
	    પાછા 3;
	}
	પાછા 1;
    }
    જો (vim_strchr((char_u *)"+-*/%", *p) != ખાલી && p[1] == '=')
	પાછા 2;
    જો (STRNCMP(p, "..=", 3) == 0)
	પાછા 3;
    પાછા 0;
}

/*
 * Generate the load instruction for "name".
 */
    સ્થિર પૂર્ણાંક
generate_loadvar(cctx_T *cctx, lhs_T *lhs)
{
    char_u	*name = lhs->lhs_name;
    type_T	*type = lhs->lhs_type;
    પૂર્ણાંક		res = OK;

    પસંદગી (lhs->lhs_dest)
    {
	સ્થિતિ dest_option:
	સ્થિતિ dest_func_option:
	    generate_LOAD(cctx, ISN_LOADOPT, 0, name, type);
	    તોડો;
	સ્થિતિ dest_global:
	    જો (vim_strchr(name, AUTOLOAD_CHAR) == ખાલી)
	    {
		જો (name[2] == NUL)
		    generate_instr_type(cctx, ISN_LOADGDICT, &t_dict_any);
		નહીંતર
		    generate_LOAD(cctx, ISN_LOADG, 0, name + 2, type);
	    }
	    નહીંતર
		generate_LOAD(cctx, ISN_LOADAUTO, 0, name, type);
	    તોડો;
	સ્થિતિ dest_buffer:
	    generate_LOAD(cctx, ISN_LOADB, 0, name + 2, type);
	    તોડો;
	સ્થિતિ dest_window:
	    generate_LOAD(cctx, ISN_LOADW, 0, name + 2, type);
	    તોડો;
	સ્થિતિ dest_tab:
	    generate_LOAD(cctx, ISN_LOADT, 0, name + 2, type);
	    તોડો;
	સ્થિતિ dest_script:
	સ્થિતિ dest_script_v9:
	    res = compile_load_scriptvar(cctx,
			    name + (name[1] == ':' ? 2 : 0), ખાલી, ખાલી, ખાલી);
	    તોડો;
	સ્થિતિ dest_env:
	    // Include $ in the name here
	    generate_LOAD(cctx, ISN_LOADENV, 0, name, type);
	    તોડો;
	સ્થિતિ dest_reg:
	    generate_LOAD(cctx, ISN_LOADREG, name[1], ખાલી, &t_string);
	    તોડો;
	સ્થિતિ dest_vimvar:
	    generate_LOADV(cctx, name + 2);
	    તોડો;
	સ્થિતિ dest_local:
	    જો (cctx->ctx_skip != SKIP_YES)
	    {
		lvar_T	*lvar = lhs->lhs_lvar;
		જો (lvar->lv_from_outer > 0)
		    generate_LOADOUTER(cctx, lvar->lv_idx, lvar->lv_from_outer,
				 lvar->lv_loop_depth, lvar->lv_loop_idx, type);
		નહીંતર
		    generate_LOAD(cctx, ISN_LOAD, lvar->lv_idx, ખાલી, type);
	    }
	    તોડો;
	સ્થિતિ dest_class_member:
	    generate_CLASSMEMBER(cctx, TRUE, lhs->lhs_class,
						     lhs->lhs_classmember_idx);
	    તોડો;
	સ્થિતિ dest_પદાવલિ:
	    // list or dict value should already be on the stack.
	    તોડો;
    }

    પાછા res;
}

/*
 * Skip over "[expr]" or ".member".
 * Does not check for any errors.
 */
    સ્થિર char_u *
skip_index(char_u *start)
{
    char_u *p = start;

    જો (*p == '[')
    {
	p = skipwhite(p + 1);
	(શૂન્ય)skip_પદાવલિ(&p, ખાલી);
	p = skipwhite(p);
	જો (*p == ']')
	    પાછા p + 1;
	પાછા p;
    }
    // if (*p == '.')
    પાછા to_name_end(p + 1, TRUE);
}

    શૂન્ય
vim9_declare_error(char_u *name)
{
    અક્ષર *scope = "";

    પસંદગી (*name)
    {
	સ્થિતિ 'g': scope = _("global"); તોડો;
	સ્થિતિ 'b': scope = _("buffer"); તોડો;
	સ્થિતિ 'w': scope = _("window"); તોડો;
	સ્થિતિ 't': scope = _("tab"); તોડો;
	સ્થિતિ 'v': scope = "v:"; તોડો;
	સ્થિતિ '$': semsg(_(e_cannot_declare_an_environment_variable_str), name);
		  પાછા;
	સ્થિતિ '&': semsg(_(e_cannot_declare_an_option_str), name);
		  પાછા;
	સ્થિતિ '@': semsg(_(e_cannot_declare_a_register_str), name);
		  પાછા;
	મૂળભૂત: પાછા;
    }
    semsg(_(e_cannot_declare_a_scope_variable_str), scope, name);
}

/*
 * Return TRUE if "name" is a valid register to use.
 * Return FALSE and give an error message if not.
 */
    સ્થિર પૂર્ણાંક
valid_dest_reg(પૂર્ણાંક name)
{
    જો (name == '@')
       name = '"';
    જો (name == '/' || name == '=' || name == '#' || valid_yank_reg(name, TRUE))
	પાછા TRUE;
    emsg_invreg(name);
    પાછા FAIL;
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
    પૂર્ણાંક
get_var_dest(
	char_u		*name,
	assign_dest_T	*dest,
	cmdidx_T	cmdidx,
	પૂર્ણાંક		has_index,
	પૂર્ણાંક		*option_scope,
	પૂર્ણાંક		*vimvaridx,
	type_T		**type,
	cctx_T		*cctx)
{
    char_u *p;

    જો (*name == '&')
    {
	પૂર્ણાંક		cc;
	long		numval;
	getoption_T	opt_type;
	પૂર્ણાંક		opt_p_flags;

	*dest = dest_option;
	જો (cmdidx == CMD_final || cmdidx == CMD_const)
	{
	    emsg(_(e_cannot_lock_option));
	    પાછા FAIL;
	}
	p = name;
	p = find_option_end(&p, option_scope);
	જો (p == ખાલી)
	{
	    // cannot happen?
	    emsg(_(e_unexpected_characters_in_assignment));
	    પાછા FAIL;
	}
	cc = *p;
	*p = NUL;
	opt_type = get_option_કિંમત(skip_option_env_lead(name),
				   &numval, ખાલી, &opt_p_flags, *option_scope);
	*p = cc;
	પસંદગી (opt_type)
	{
	    સ્થિતિ gov_unknown:
		    semsg(_(e_unknown_option_str), name);
		    પાછા FAIL;
	    સ્થિતિ gov_string:
	    સ્થિતિ gov_hidden_string:
		    જો (opt_p_flags & P_FUNC)
		    {
			// might be a Funcref, check the type later
			*type = &t_any;
			*dest = dest_func_option;
		    }
		    નહીંતર
		    {
			*type = &t_string;
		    }
		    તોડો;
	    સ્થિતિ gov_bool:
	    સ્થિતિ gov_hidden_bool:
		    *type = &t_bool;
		    તોડો;
	    સ્થિતિ gov_સંખ્યા:
	    સ્થિતિ gov_hidden_સંખ્યા:
		    *type = &t_સંખ્યા;
		    તોડો;
	}
    }
    નહીંતર જો (*name == '$')
    {
	*dest = dest_env;
	*type = &t_string;
    }
    નહીંતર જો (*name == '@')
    {
	જો (!valid_dest_reg(name[1]))
	    પાછા FAIL;
	*dest = dest_reg;
	*type = name[1] == '#' ? &t_સંખ્યા_or_string : &t_string;
    }
    નહીંતર જો (STRNCMP(name, "g:", 2) == 0)
    {
	*dest = dest_global;
    }
    નહીંતર જો (STRNCMP(name, "b:", 2) == 0)
    {
	*dest = dest_buffer;
    }
    નહીંતર જો (STRNCMP(name, "w:", 2) == 0)
    {
	*dest = dest_window;
    }
    નહીંતર જો (STRNCMP(name, "t:", 2) == 0)
    {
	*dest = dest_tab;
    }
    નહીંતર જો (STRNCMP(name, "v:", 2) == 0)
    {
	typval_T	*vtv;
	પૂર્ણાંક		di_flags;

	*vimvaridx = find_vim_var(name + 2, &di_flags);
	જો (*vimvaridx < 0)
	{
	    semsg(_(e_variable_not_found_str), name);
	    પાછા FAIL;
	}
	// An item of a read-only variable can be changed, as when not compiled.
	// We use the current value of "sandbox" here, is that OK?
	જો (!has_index && var_check_ro(di_flags, name, FALSE))
	    પાછા FAIL;
	*dest = dest_vimvar;
	vtv = get_vim_var_tv(*vimvaridx);
	*type = typval2type_vimvar(vtv, cctx->ctx_type_list);
    }
    પાછા OK;
}

    સ્થિર પૂર્ણાંક
is_decl_command(cmdidx_T cmdidx)
{
    પાછા cmdidx == CMD_let || cmdidx == CMD_var
				 || cmdidx == CMD_final || cmdidx == CMD_const;
}

/*
 * Returns TRUE if the class or object variable in "lhs" is modifiable.
 * "var_start" points to the start of the variable name and "lhs->lhs_varlen"
 * has the total length.  Note that the "lhs" can be nested an object reference
 * (e.g.  a.b.c.d.var).
 */
    સ્થિર પૂર્ણાંક
lhs_class_member_modifiable(lhs_T *lhs, char_u	*var_start, cctx_T *cctx)
{
    size_t	varlen = lhs->lhs_varlen;
    class_T	*cl = lhs->lhs_type->tt_class;
    પૂર્ણાંક		is_object = lhs->lhs_type->tt_type == VAR_OBJECT;
    char_u	*name = var_start + varlen + 1;
    size_t	namelen = lhs->lhs_end - var_start - varlen - 1;
    ocmember_T	*m;

    m = member_lookup(cl, lhs->lhs_type->tt_type, name, namelen, ખાલી);
    જો (m == ખાલી)
    {
	member_not_found_સંદેશ(cl, lhs->lhs_type->tt_type, name, namelen);
	પાછા FALSE;
    }

    જો (IS_ENUM(cl))
    {
	semsg(_(e_enumvalue_str_cannot_be_modified), cl->class_name.string,
		m->ocm_name.string);
	પાછા FALSE;
    }

    // If it is private member variable, then accessing it outside the
    // class is not allowed.
    // If it is a read only class variable, then it can be modified
    // only inside the class where it is defined.
    જો ((m->ocm_access != VIM_ACCESS_ALL) &&
	    ((is_object && !inside_class(cctx, cl))
	     || (!is_object && cctx->ctx_ufunc->uf_class != cl)))
    {
	અક્ષર *સંદેશ = (m->ocm_access == VIM_ACCESS_PRIVATE)
				? e_cannot_access_protected_variable_str
				: e_variable_is_not_writable_str;
	emsg_var_cl_define(સંદેશ, m->ocm_name.string, 0, cl);
	પાછા FALSE;
    }

    પાછા TRUE;
}

/*
 * Initialize "lhs" with default values
 */
    સ્થિર શૂન્ય
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
    સ્થિર પૂર્ણાંક
lhs_find_var_end(
    lhs_T	*lhs,
    char_u	*var_start,
    પૂર્ણાંક		is_decl,
    char_u	**var_endp)
{
    char_u  *var_end = *var_endp;

    // "lhs_dest_end" is the end of the destination, including "[expr]" or
    // ".name".
    // "var_end" is the end of the variable/option/etc. name.
    lhs->lhs_dest_end = skip_var_one(var_start, FALSE);
    જો (*var_start == '@')
    {
	જો (!valid_dest_reg(var_start[1]))
	    પાછા FAIL;
	var_end = var_start + 2;
    }
    નહીંતર
    {
	// skip over the leading "&", "&l:", "&g:" and "$"
	var_end = skip_option_env_lead(var_start);
	var_end = to_name_end(var_end, TRUE);
    }

    // "a: type" is declaring variable "a" with a type, not dict "a:".
    જો (is_decl && lhs->lhs_dest_end == var_start + 2
					&& lhs->lhs_dest_end[-1] == ':')
	--lhs->lhs_dest_end;
    જો (is_decl && var_end == var_start + 2 && var_end[-1] == ':')
	--var_end;

    lhs->lhs_end = lhs->lhs_dest_end;
    *var_endp = var_end;

    પાછા OK;
}

/*
 * Set various fields in "lhs"
 */
    સ્થિર પૂર્ણાંક
lhs_init(
    lhs_T	*lhs,
    char_u	*var_start,
    પૂર્ણાંક		is_decl,
    પૂર્ણાંક		heredoc,
    char_u	**var_endp)
{
    char_u *var_end = *var_endp;

    lhs_init_defaults(lhs);

    // Find the end of the variable and the destination
    જો (lhs_find_var_end(lhs, var_start, is_decl, &var_end) == FAIL)
	પાછા FAIL;

    // compute the length of the destination without "[expr]" or ".name"
    lhs->lhs_varlen = var_end - var_start;
    lhs->lhs_varlen_total = lhs->lhs_varlen;
    lhs->lhs_name = vim_strnsave(var_start, lhs->lhs_varlen);
    જો (lhs->lhs_name == ખાલી)
	પાછા FAIL;

    જો (lhs->lhs_dest_end > var_start + lhs->lhs_varlen)
	// Something follows after the variable: "var[idx]" or "var.key".
	lhs->lhs_has_index = TRUE;

    lhs->lhs_type = heredoc ? &t_list_string : &t_any;

    *var_endp = var_end;

    પાછા OK;
}

/*
 * Compile a LHS class variable name.
 */
    સ્થિર પૂર્ણાંક
compile_lhs_class_variable(
    cctx_T	*cctx,
    lhs_T	*lhs,
    class_T	*defcl,
    પૂર્ણાંક		is_decl)
{
    જો (cctx->ctx_ufunc->uf_defclass != defcl)
    {
	// A class variable can be accessed without the class name
	// only inside a class.
	semsg(_(e_class_variable_str_accessible_only_inside_class_str),
		lhs->lhs_name, defcl->class_name.string);
	પાછા FAIL;
    }

    જો (is_decl)
    {
	semsg(_(e_variable_already_declared_in_class_str), lhs->lhs_name);
	પાછા FAIL;
    }

    ocmember_T	*m = &defcl->class_class_members[lhs->lhs_classmember_idx];
    જો (oc_var_check_ro(defcl, m))
	પાછા FAIL;

    lhs->lhs_dest = dest_class_member;
    // The class variable is defined either in the current class or
    // in one of the parent class in the hierarchy.
    lhs->lhs_class = defcl;
    lhs->lhs_type = oc_member_type_by_idx(defcl, FALSE,
						lhs->lhs_classmember_idx);

    પાછા OK;
}

/*
 * Compile an imported LHS variable
 */
    સ્થિર પૂર્ણાંક
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
    જો (dot == ખાલી)
    {
	semsg(_(e_no_dot_after_imported_name_str), var_start);
	પાછા FAIL;
    }

    p = skipwhite(dot + 1);
    var_end = to_name_end(p, TRUE);
    જો (var_end == p)
    {
	semsg(_(e_missing_name_after_imported_name_str), var_start);
	પાછા FAIL;
    }

    vim_free(lhs->lhs_name);
    lhs->lhs_varlen = var_end - p;
    lhs->lhs_name = vim_strnsave(p, lhs->lhs_varlen);
    જો (lhs->lhs_name == ખાલી)
	પાછા FAIL;
    *rawnamep = lhs->lhs_name;
    lhs->lhs_scriptvar_sid = import->imp_sid;

    // TODO: where do we check this name is exported?

    // Check if something follows: "exp.var[idx]" or
    // "exp.var.key".
    lhs->lhs_has_index = lhs->lhs_dest_end > skipwhite(var_end);

    *var_endp = var_end;

    પાછા OK;
}

/*
 * Process a script-local variable when compiling a LHS variable name.
 */
    સ્થિર પૂર્ણાંક
compile_lhs_script_var(
    cctx_T	*cctx,
    lhs_T	*lhs,
    char_u	*var_start,
    char_u	*var_end,
    પૂર્ણાંક		is_decl)
{
    પૂર્ણાંક		script_namespace = FALSE;
    પૂર્ણાંક		script_var = FALSE;
    imported_T	*import;
    char_u	*var_name;
    size_t	var_name_len;

    જો (lhs->lhs_varlen > 1 && STRNCMP(var_start, "s:", 2) == 0)
	script_namespace = TRUE;

    જો (script_namespace)
    {
	var_name = var_start + 2;
	var_name_len = lhs->lhs_varlen - 2;
    }
    નહીંતર
    {
	var_name = var_start;
	var_name_len = lhs->lhs_varlen;
    }

    જો (script_var_exists(var_name, var_name_len, cctx, ખાલી) == OK)
	script_var = TRUE;

    import = find_imported(var_start, lhs->lhs_varlen, FALSE);

    જો (script_namespace || script_var || import != ખાલી)
    {
	char_u *rawname = lhs->lhs_name + (lhs->lhs_name[1] == ':' ? 2 : 0);

	જો (script_namespace && current_script_is_vim9())
	{
	    semsg(_(e_cannot_use_s_colon_in_vim9_script_str), var_start);
	    પાછા FAIL;
	}

	જો (is_decl)
	{
	    જો (script_namespace)
		semsg(_(e_cannot_declare_script_variable_in_function_str),
			lhs->lhs_name);
	    નહીંતર
		semsg(_(e_variable_already_declared_in_script_str),
			lhs->lhs_name);
	    પાછા FAIL;
	}
	નહીંતર જો (cctx->ctx_ufunc->uf_script_ctx_version == SCRIPT_VERSION_VIM9
		&& script_namespace
		&& !script_var && import == ખાલી)
	{
	    semsg(_(e_unknown_variable_str), lhs->lhs_name);
	    પાછા FAIL;
	}

	lhs->lhs_dest = current_script_is_vim9() ? dest_script_v9 :
								dest_script;

	// existing script-local variables should have a type
	lhs->lhs_scriptvar_sid = current_sctx.sc_sid;
	જો (import != ખાલી)
	{
	    જો (compile_lhs_import_var(lhs, import, var_start, &var_end,
							&rawname) == FAIL)
		પાછા FAIL;
	}

	જો (SCRIPT_ID_VALID(lhs->lhs_scriptvar_sid))
	{
	    // Check writable only when no index follows.
	    lhs->lhs_scriptvar_idx = get_script_item_idx(
					lhs->lhs_scriptvar_sid, rawname,
					lhs->lhs_has_index ?  ASSIGN_FINAL :
					ASSIGN_CONST, cctx, ખાલી);
	    જો (lhs->lhs_scriptvar_idx >= 0)
	    {
		scriptitem_T *si = SCRIPT_ITEM(lhs->lhs_scriptvar_sid);
		svar_T	 *sv = ((svar_T *)si->sn_var_vals.ga_data)
						+ lhs->lhs_scriptvar_idx;

		lhs->lhs_type = sv->sv_type;
	    }
	}

	પાછા OK;
    }

    પાછા check_defined(var_start, lhs->lhs_varlen, cctx, ખાલી, FALSE);
}

/*
 * Compile the LHS destination.
 */
    સ્થિર પૂર્ણાંક
compile_lhs_var_dest(
    cctx_T	*cctx,
    lhs_T	*lhs,
    પૂર્ણાંક		cmdidx,
    char_u	*var_start,
    char_u	*var_end,
    પૂર્ણાંક		is_decl,
    પૂર્ણાંક		has_cmd)	// "var" before "var_start"
{
    પૂર્ણાંક	    declare_error = FALSE;

    જો (get_var_dest(lhs->lhs_name, &lhs->lhs_dest, cmdidx,
				lhs->lhs_has_index,
				&lhs->lhs_opt_flags, &lhs->lhs_vimvaridx,
				&lhs->lhs_type, cctx) == FAIL)
	પાછા FAIL;

    જો (lhs->lhs_dest != dest_local && cmdidx != CMD_const
						&& cmdidx != CMD_final)
    {
	// Specific kind of variable recognized.
	declare_error = is_decl;
    }
    નહીંતર
    {
	class_T	*defcl;

	// No specific kind of variable recognized, just a name.
	જો (check_reserved_name(lhs->lhs_name, lhs->lhs_has_index
						&& *var_end == '.') == FAIL)
	    પાછા FAIL;

	જો (lookup_local(var_start, lhs->lhs_varlen, &lhs->lhs_local_lvar,
								cctx) == OK)
	{
	    lhs->lhs_lvar = &lhs->lhs_local_lvar;
	}
	નહીંતર
	{
	    CLEAR_FIELD(lhs->lhs_arg_lvar);
	    જો (arg_exists(var_start, lhs->lhs_varlen,
			&lhs->lhs_arg_lvar.lv_idx, &lhs->lhs_arg_lvar.lv_type,
			&lhs->lhs_arg_lvar.lv_from_outer, cctx) == OK)
	    {
		જો (is_decl)
		{
		    semsg(_(e_str_is_used_as_argument), lhs->lhs_name);
		    પાછા FAIL;
		}
		lhs->lhs_lvar = &lhs->lhs_arg_lvar;
	    }
	}

	જો (lhs->lhs_lvar != ખાલી)
	{
	    જો (is_decl)
	    {
		// if we come here with what looks like an assignment like
		// .= but which has been rejected by assignment_len() from
		// may_compile_assignment give a better error message
		char_u *p = skipwhite(lhs->lhs_end);
		જો (p[0] == '.' && p[1] == '=')
		    emsg(_(e_dot_equal_not_supported_with_script_version_two));
		નહીંતર જો (p[0] == ':' && !has_cmd)
		    // type specified in an assignment without "var"
		    semsg(_(e_trailing_characters_str), p);
		નહીંતર
		    semsg(_(e_variable_already_declared_str), lhs->lhs_name);
		પાછા FAIL;
	    }
	}
	નહીંતર જો ((lhs->lhs_classmember_idx = cctx_class_member_idx(
			cctx, var_start, lhs->lhs_varlen, &defcl)) >= 0)
	{
	    જો (compile_lhs_class_variable(cctx, lhs, defcl, is_decl)
		    == FAIL)
		પાછા FAIL;
	}
	નહીંતર
	{
	    જો (compile_lhs_script_var(cctx, lhs, var_start, var_end,
			is_decl) == FAIL)
		પાછા FAIL;
	}
    }

    જો (declare_error)
    {
	vim9_declare_error(lhs->lhs_name);
	પાછા FAIL;
    }

    પાછા OK;
}

/*
 * When compiling a LHS variable name, for a class or an object, set the LHS
 * member type.
 */
    સ્થિર પૂર્ણાંક
compile_lhs_set_oc_member_type(
    cctx_T	*cctx,
    lhs_T	*lhs,
    char_u	*var_start)
{
    class_T	*cl = lhs->lhs_type->tt_class;
    પૂર્ણાંક		is_object = lhs->lhs_type->tt_type == VAR_OBJECT;
    char_u	*name = var_start + lhs->lhs_varlen + 1;
    size_t	namelen = lhs->lhs_end - var_start - lhs->lhs_varlen - 1;

    ocmember_T	*m = member_lookup(cl, lhs->lhs_type->tt_type,
	    name, namelen, &lhs->lhs_member_idx);
    જો (m == ખાલી)
    {
	member_not_found_સંદેશ(cl, lhs->lhs_type->tt_type, name, namelen);
	પાછા FAIL;
    }

    જો (IS_ENUM(cl))
    {
	જો (!inside_class(cctx, cl))
	{
	    semsg(_(e_enumvalue_str_cannot_be_modified),
		    cl->class_name.string, m->ocm_name.string);
	    પાછા FAIL;
	}
	જો (lhs->lhs_type->tt_type == VAR_OBJECT &&
		lhs->lhs_member_idx < 2)
	{
	    અક્ષર *સંદેશ = lhs->lhs_member_idx == 0 ?
		e_enum_str_name_cannot_be_modified :
		e_enum_str_ordinal_cannot_be_modified;
	    semsg(_(સંદેશ), cl->class_name.string);
	    પાછા FAIL;
	}
    }

    // If it is private member variable, then accessing it outside the
    // class is not allowed.
    // If it is a read only class variable, then it can be modified
    // only inside the class where it is defined.
    જો ((m->ocm_access != VIM_ACCESS_ALL) &&
	    ((is_object && !inside_class(cctx, cl))
	     || (!is_object && cctx->ctx_ufunc->uf_defclass != cl)))
    {
	અક્ષર *સંદેશ = (m->ocm_access == VIM_ACCESS_PRIVATE)
	    ? e_cannot_access_protected_variable_str
	    : e_variable_is_not_writable_str;
	emsg_var_cl_define(સંદેશ, m->ocm_name.string, 0, cl);
	પાછા FAIL;
    }

    જો (!IS_CONSTRUCTOR_METHOD(cctx->ctx_ufunc)
	    && oc_var_check_ro(cl, m))
	પાછા FAIL;

    lhs->lhs_member_type = m->ocm_type;

    પાછા OK;
}

/*
 * When compiling a LHS variable, set the LHS variable type.
 */
    સ્થિર પૂર્ણાંક
compile_lhs_set_type(cctx_T *cctx, lhs_T *lhs, char_u *var_end, પૂર્ણાંક is_decl)
{
    જો (is_decl && *skipwhite(var_end) == ':')
    {
	char_u *p;

	// parse optional type: "let var: type = expr"
	જો (VIM_ISWHITE(*var_end))
	{
	    semsg(_(e_no_white_space_allowed_before_colon_str), var_end);
	    પાછા FAIL;
	}

	જો (!VIM_ISWHITE(var_end[1]))
	{
	    semsg(_(e_white_space_required_after_str_str), ":", var_end);
	    પાછા FAIL;
	}

	p = skipwhite(var_end + 1);
	lhs->lhs_type = parse_type(&p, cctx->ctx_type_list, cctx->ctx_ufunc, cctx, TRUE);
	જો (lhs->lhs_type == ખાલી
		|| !valid_declaration_type(lhs->lhs_type))
	    પાછા FAIL;

	lhs->lhs_has_type = TRUE;
	lhs->lhs_end = p;
    }
    નહીંતર જો (lhs->lhs_lvar != ખાલી)
	lhs->lhs_type = lhs->lhs_lvar->lv_type;

    પાછા OK;
}

/*
 * Returns TRUE if "lhs" is a concatenable string.
 */
    સ્થિર પૂર્ણાંક
lhs_concatenable(lhs_T *lhs)
{
    પાછા lhs->lhs_dest == dest_global
		|| lhs->lhs_has_index
		|| lhs->lhs_type->tt_type == VAR_STRING
		|| lhs->lhs_type->tt_type == VAR_ANY;
}

/*
 * Create a new local variable when compiling a LHS variable.
 */
    સ્થિર પૂર્ણાંક
compile_lhs_new_local_var(
    cctx_T	*cctx,
    lhs_T	*lhs,
    char_u	*var_start,
    પૂર્ણાંક		cmdidx,
    પૂર્ણાંક		oplen,
    પૂર્ણાંક		is_decl,
    પૂર્ણાંક		has_cmd,
    પૂર્ણાંક		heredoc)
{
    જો (oplen > 1 && !heredoc)
    {
	// +=, /=, etc. require an existing variable
	semsg(_(e_cannot_use_operator_on_new_variable_str), lhs->lhs_name);
	પાછા FAIL;
    }

    જો (!is_decl || (lhs->lhs_has_index && !has_cmd
					&& cctx->ctx_skip != SKIP_YES))
    {
	semsg(_(e_unknown_variable_str), lhs->lhs_name);
	પાછા FAIL;
    }

    // Check the name is valid for a funcref.
    જો (lhs->lhs_type->tt_type == VAR_FUNC
				|| lhs->lhs_type->tt_type == VAR_PARTIAL)
    {
	જો (var_wrong_func_name(lhs->lhs_name, TRUE))
	    પાછા FAIL;
    }

    // New local variable.
    પૂર્ણાંક assign;
    પસંદગી (cmdidx)
    {
	સ્થિતિ CMD_final:
	    assign = ASSIGN_FINAL; તોડો;
	સ્થિતિ CMD_const:
	    assign = ASSIGN_CONST; તોડો;
	મૂળભૂત:
	    assign = ASSIGN_VAR; તોડો;
    }

    lhs->lhs_lvar = reserve_local(cctx, var_start, lhs->lhs_varlen, assign,
							lhs->lhs_type);
    જો (lhs->lhs_lvar == ખાલી)
	પાછા FAIL;

    lhs->lhs_new_local = TRUE;

    પાછા OK;
}

/*
 * When compiling a LHS variable name, set the LHS member type.
 */
    સ્થિર પૂર્ણાંક
compile_lhs_set_member_type(
    cctx_T	*cctx,
    lhs_T	*lhs,
    char_u	*var_start,
    પૂર્ણાંક		is_decl,
    પૂર્ણાંક		has_cmd)
{
    lhs->lhs_member_type = lhs->lhs_type;

    જો (!lhs->lhs_has_index)
	પાછા OK;

    char_u	*after = var_start + lhs->lhs_varlen;
    char_u	*p;

    // Something follows after the variable: "var[idx]" or "var.key".
    જો (is_decl && cctx->ctx_skip != SKIP_YES)
    {
	જો (has_cmd)
	    emsg(_(e_cannot_use_index_when_declaring_variable));
	નહીંતર
	    semsg(_(e_unknown_variable_str), lhs->lhs_name);
	પાછા FAIL;
    }

    // Now: var_start[lhs->lhs_varlen] is '[' or '.'
    // Only the last index is used below, if there are others
    // before it generate code for the expression.  Thus for
    // "ll[1][2]" the expression is "ll[1]" and "[2]" is the index.
    માટે (;;)
    {
	p = skip_index(after);
	જો (*p != '[' && *p != '.')
	{
	    lhs->lhs_varlen_total = p - var_start;
	    તોડો;
	}
	after = p;
    }
    જો (after > var_start + lhs->lhs_varlen)
    {
	lhs->lhs_varlen = after - var_start;
	lhs->lhs_dest = dest_પદાવલિ;
	// We don't know the type before evaluating the expression,
	// use "any" until then.  The member index is for the first name,
	// not for the last index.
	lhs->lhs_type = &t_any;
	lhs->lhs_member_idx = -1;
    }

    પૂર્ણાંક use_class = lhs->lhs_type != ખાલી
	&& (lhs->lhs_type->tt_type == VAR_CLASS
		|| lhs->lhs_type->tt_type == VAR_OBJECT);

    જો (lhs->lhs_type == ખાલી
	    || (use_class ? lhs->lhs_type->tt_class == ખાલી
		: lhs->lhs_type->tt_member == ખાલી))
    {
	lhs->lhs_member_type = &t_any;
    }
    નહીંતર જો (use_class)
    {
	// for an object or class member get the type of the member
	જો (compile_lhs_set_oc_member_type(cctx, lhs, var_start) == FAIL)
	    પાછા FAIL;
    }
    નહીંતર
	lhs->lhs_member_type = lhs->lhs_type->tt_member;

    પાછા OK;
}

/*
 * Figure out the LHS type and other properties for an assignment or one item
 * of ":unlet" with an index.
 * Returns OK or FAIL.
 */
    પૂર્ણાંક
compile_lhs(
	char_u	    *var_start,
	lhs_T	    *lhs,
	cmdidx_T    cmdidx,
	પૂર્ણાંક	    heredoc,
	પૂર્ણાંક	    has_cmd,	    // "var" before "var_start"
	પૂર્ણાંક	    oplen,
	cctx_T	    *cctx)
{
    char_u	*var_end = ખાલી;
    પૂર્ણાંક		is_decl = is_decl_command(cmdidx);

    જો (lhs_init(lhs, var_start, is_decl, heredoc, &var_end) == FAIL)
	પાછા FAIL;

    જો (cctx->ctx_skip != SKIP_YES)
    {
	// compile the LHS destination
	જો (compile_lhs_var_dest(cctx, lhs, cmdidx, var_start, var_end,
						is_decl, has_cmd) == FAIL)
	    પાછા FAIL;
    }

    // handle "a:name" as a name, not index "name" in "a"
    જો (lhs->lhs_varlen > 1 || var_start[lhs->lhs_varlen] != ':')
	var_end = lhs->lhs_dest_end;

    જો (lhs->lhs_dest != dest_option && lhs->lhs_dest != dest_func_option)
    {
	// set the LHS variable type
	જો (compile_lhs_set_type(cctx, lhs, var_end, is_decl) == FAIL)
	    પાછા FAIL;
    }

    જો (oplen == 3 && !heredoc && !lhs_concatenable(lhs))
    {
	emsg(_(e_can_only_concatenate_to_string));
	પાછા FAIL;
    }

    જો (lhs->lhs_lvar == ખાલી && lhs->lhs_dest == dest_local
						&& cctx->ctx_skip != SKIP_YES)
    {
	જો (compile_lhs_new_local_var(cctx, lhs, var_start, cmdidx, oplen,
					is_decl, has_cmd, heredoc) == FAIL)
	    પાછા FAIL;
    }

    જો (compile_lhs_set_member_type(cctx, lhs, var_start, is_decl, has_cmd)
								== FAIL)
	પાછા FAIL;

    પાછા OK;
}

/*
 * Figure out the LHS and check a few errors.
 */
    પૂર્ણાંક
compile_assign_lhs(
	char_u	    *var_start,
	lhs_T	    *lhs,
	cmdidx_T    cmdidx,
	પૂર્ણાંક	    is_decl,
	પૂર્ણાંક	    heredoc,
	પૂર્ણાંક	    has_cmd,	    // "var" before "var_start"
	પૂર્ણાંક	    oplen,
	cctx_T	    *cctx)
{
    જો (compile_lhs(var_start, lhs, cmdidx, heredoc, has_cmd, oplen, cctx)
								       == FAIL)
	પાછા FAIL;

    જો (!lhs->lhs_has_index && lhs->lhs_lvar == &lhs->lhs_arg_lvar)
    {
	semsg(_(e_cannot_assign_to_argument_str), lhs->lhs_name);
	પાછા FAIL;
    }
    જો (!is_decl && lhs->lhs_lvar != ખાલી
			   && lhs->lhs_lvar->lv_const != ASSIGN_VAR
			   && !lhs->lhs_has_index)
    {
	semsg(_(e_cannot_assign_to_constant_str), lhs->lhs_name);
	પાછા FAIL;
    }
    પાછા OK;
}

/*
 * Return TRUE if "lhs" has a range index: "[expr : expr]".
 */
    સ્થિર પૂર્ણાંક
has_list_index(char_u *idx_start, cctx_T *cctx)
{
    char_u  *p = idx_start;
    પૂર્ણાંક	    save_skip;

    જો (*p != '[')
	પાછા FALSE;

    p = skipwhite(p + 1);
    જો (*p == ':')
	પાછા TRUE;

    save_skip = cctx->ctx_skip;
    cctx->ctx_skip = SKIP_YES;
    (શૂન્ય)compile_expr0(&p, cctx);
    cctx->ctx_skip = save_skip;
    પાછા *skipwhite(p) == ':';
}

/*
 * For an assignment with an index, compile the "idx" in "var[idx]" or "key" in
 * "var.key".
 */
    સ્થિર પૂર્ણાંક
compile_assign_index(
	char_u	*var_start,
	lhs_T	*lhs,
	પૂર્ણાંક	*range,
	cctx_T	*cctx)
{
    size_t	varlen = lhs->lhs_varlen;
    char_u	*p;
    પૂર્ણાંક		r = OK;
    પૂર્ણાંક		need_white_before = TRUE;
    પૂર્ણાંક		empty_second;

    p = var_start + varlen;
    જો (*p == '[')
    {
	p = skipwhite(p + 1);
	જો (*p == ':')
	{
	    // empty first index, push zero
	    r = generate_PUSHNR(cctx, 0);
	    need_white_before = FALSE;
	}
	નહીંતર
	    r = compile_expr0(&p, cctx);

	જો (r == OK && *skipwhite(p) == ':')
	{
	    // unlet var[idx : idx]
	    // blob[idx : idx] = value
	    *range = TRUE;
	    p = skipwhite(p);
	    empty_second = *skipwhite(p + 1) == ']';
	    જો ((need_white_before && !IS_WHITE_OR_NUL(p[-1]))
		    || (!empty_second && !IS_WHITE_OR_NUL(p[1])))
	    {
		semsg(_(e_white_space_required_before_and_after_str_at_str),
								      ":", p);
		પાછા FAIL;
	    }
	    p = skipwhite(p + 1);
	    જો (*p == ']')
		// empty second index, push "none"
		r = generate_PUSHSPEC(cctx, VVAL_NONE);
	    નહીંતર
		r = compile_expr0(&p, cctx);
	}

	જો (r == OK && *skipwhite(p) != ']')
	{
	    // this should not happen
	    emsg(_(e_missing_closing_square_brace));
	    r = FAIL;
	}
    }
    નહીંતર જો (lhs->lhs_member_idx >= 0)
    {
	// object member index
	r = generate_PUSHNR(cctx, lhs->lhs_member_idx);
    }
    નહીંતર // if (*p == '.')
    {
	char_u *ચાવી_end = to_name_end(p + 1, TRUE);
	char_u *ચાવી = vim_strnsave(p + 1, ચાવી_end - p - 1);

	r = generate_PUSHS(cctx, &ચાવી);
    }
    પાછા r;
}

/*
 * For a LHS with an index, load the variable to be indexed.
 */
    સ્થિર પૂર્ણાંક
compile_load_lhs(
	lhs_T	*lhs,
	char_u	*var_start,
	type_T	*rhs_type,
	cctx_T	*cctx)
{
    જો (lhs->lhs_dest == dest_પદાવલિ)
    {
	size_t	    varlen = lhs->lhs_varlen;
	પૂર્ણાંક	    c = var_start[varlen];
	પૂર્ણાંક	    લીટીઓ_len = cctx->ctx_ufunc->uf_લીટીઓ.ga_len;
	પૂર્ણાંક	    res;

	// Evaluate "ll[expr]" of "ll[expr][idx]".  End the line with a NUL and
	// limit the lines array length to avoid skipping to a following line.
	var_start[varlen] = NUL;
	cctx->ctx_ufunc->uf_લીટીઓ.ga_len = cctx->ctx_lnum + 1;
	char_u *p = var_start;
	res = compile_expr0(&p, cctx);
	var_start[varlen] = c;
	cctx->ctx_ufunc->uf_લીટીઓ.ga_len = લીટીઓ_len;
	જો (res == FAIL || p != var_start + varlen)
	{
	    // this should not happen
	    જો (res != FAIL)
		emsg(_(e_missing_closing_square_brace));
	    પાછા FAIL;
	}

	lhs->lhs_type = cctx->ctx_type_ઢગલો.ga_len == 0 ? &t_void
						  : get_type_on_ઢગલો(cctx, 0);

	જો (lhs->lhs_type->tt_type == VAR_CLASS
		|| (lhs->lhs_type->tt_type == VAR_OBJECT
		    && lhs->lhs_type != &t_object_any))
	{
	    // Check whether the class or object variable is modifiable
	    જો (!lhs_class_member_modifiable(lhs, var_start, cctx))
		પાછા FAIL;
	}

	// Now we can properly check the type.  The variable is indexed, thus
	// we need the member type.  For a class or object we don't know the
	// type yet, it depends on what member is used.
	// The top item in the stack is the Dict, followed by the key and then
	// the type of the value.
	vartype_T vartype = lhs->lhs_type->tt_type;
	type_T *member_type = lhs->lhs_type->tt_member;
	જો (rhs_type != ખાલી && member_type != ખાલી
		&& vartype != VAR_OBJECT && vartype != VAR_CLASS
		&& rhs_type != &t_void
		&& need_type(rhs_type, member_type, 0,
					    -3, 0, cctx, FALSE, FALSE) == FAIL)
	    પાછા FAIL;

	પાછા OK;
    }

    પાછા generate_loadvar(cctx, lhs);
}

/*
 * Produce code for loading "lhs" and also take care of an index.
 * Return OK/FAIL.
 */
    પૂર્ણાંક
compile_load_lhs_with_index(lhs_T *lhs, char_u *var_start, cctx_T *cctx)
{
    જો (lhs->lhs_type->tt_type == VAR_OBJECT)
    {
	// "this.value": load "this" object and get the value at index for an
	// object or class member get the type of the member.
	// Also for "obj.value".
	char_u *dot = vim_strchr(var_start, '.');
	જો (dot == ખાલી)
	{
	    semsg(_(e_missing_dot_after_object_str), lhs->lhs_name);
	    પાછા FAIL;
	}

	class_T	*cl = lhs->lhs_type->tt_class;
	type_T	*type = oc_member_type(cl, TRUE, dot + 1,
					  lhs->lhs_end, &lhs->lhs_member_idx);
	જો (lhs->lhs_member_idx < 0)
	    પાછા FAIL;

	જો (dot - var_start == 4 && STRNCMP(var_start, "this", 4) == 0)
	{
	    // load "this"
	    lvar_T  *lvar = lhs->lhs_lvar;
	    પૂર્ણાંક	    rc;

	    જો (lvar->lv_from_outer > 0)
		rc = generate_LOADOUTER(cctx, lvar->lv_idx,
			lvar->lv_from_outer, lvar->lv_loop_depth,
			lvar->lv_loop_idx, type);
	    નહીંતર
		rc = generate_LOAD(cctx, ISN_LOAD, lvar->lv_idx, ખાલી, type);

	    જો (rc == FAIL)
		પાછા FAIL;
	}
	નહીંતર
	{
	    // load object variable or argument
	    જો (compile_load_lhs(lhs, var_start, lhs->lhs_type, cctx) == FAIL)
		પાછા FAIL;
	}
	જો (IS_INTERFACE(cl))
	    પાછા generate_GET_ITF_MEMBER(cctx, cl, lhs->lhs_member_idx, type);
	પાછા generate_GET_OBJ_MEMBER(cctx, lhs->lhs_member_idx, type);
    }
    નહીંતર જો (lhs->lhs_type->tt_type == VAR_CLASS)
    {
	// "<classname>.value": load class variable "classname.value"
	char_u *dot = vim_strchr(var_start, '.');
	જો (dot == ખાલી)
	{
	    (શૂન્ય)check_type_is_કિંમત(lhs->lhs_type);
	    પાછા FAIL;
	}

	class_T	*cl = lhs->lhs_type->tt_class;
	ocmember_T *m = class_member_lookup(cl, dot + 1,
						lhs->lhs_end - dot - 1,
						&lhs->lhs_member_idx);
	જો (m == ખાલી)
	    પાછા FAIL;

	પાછા generate_CLASSMEMBER(cctx, TRUE, cl, lhs->lhs_member_idx);
    }

    જો (compile_load_lhs(lhs, var_start, ખાલી, cctx) == FAIL)
	પાછા FAIL;

    જો (lhs->lhs_has_index)
    {
	પૂર્ણાંક	range = FALSE;
	type_T	*type = get_type_on_ઢગલો(cctx, 0);

	// A member of an object or class is not obtained by indexing it.
	જો (type->tt_type == VAR_CLASS
		|| (type->tt_type == VAR_OBJECT && type != &t_object_any))
	{
	    char_u *p = var_start + lhs->lhs_varlen;

	    પાછા compile_class_object_index(cctx, &p, type);
	}

	// Get member from list or dict.  First compile the
	// index value.
	જો (compile_assign_index(var_start, lhs, &range, cctx) == FAIL)
	    પાછા FAIL;
	જો (range)
	{
	    semsg(_(e_cannot_use_range_with_assignment_operator_str),
								    var_start);
	    પાછા FAIL;
	}

	// Get the member.
	જો (compile_member(FALSE, ખાલી, cctx) == FAIL)
	    પાછા FAIL;
    }
    પાછા OK;
}

/*
 * Assignment to a list or dict member, or ":unlet" for the item, using the
 * information in "lhs".
 * Returns OK or FAIL.
 */
    પૂર્ણાંક
compile_assign_unlet(
    char_u	*var_start,
    lhs_T	*lhs,
    પૂર્ણાંક		is_assign,
    type_T	*rhs_type,
    cctx_T	*cctx)
{
    vartype_T	dest_type;
    પૂર્ણાંક		range = FALSE;

    જો (compile_assign_index(var_start, lhs, &range, cctx) == FAIL)
	પાછા FAIL;
    જો (is_assign && range
	    && lhs->lhs_type->tt_type != VAR_LIST
	    && lhs->lhs_type != &t_blob
	    && lhs->lhs_type != &t_any)
    {
	જો (lhs->lhs_type->tt_type == VAR_TUPLE)
	    emsg(_(e_cannot_slice_tuple));
	નહીંતર
	    semsg(_(e_cannot_use_range_with_assignment_str), var_start);
	પાછા FAIL;
    }

    // For "expr[idx]" the index is compiled before the expression, thus the
    // resulting type cannot be used here.
    જો (lhs->lhs_dest == dest_પદાવલિ
	    || lhs->lhs_type == ખાલી || lhs->lhs_type == &t_any)
    {
	// Index on variable of unknown type: check at runtime.
	dest_type = VAR_ANY;
    }
    નહીંતર
    {
	dest_type = lhs->lhs_type->tt_type;
	જો (dest_type == VAR_DICT && range)
	{
	    emsg(_(e_cannot_use_range_with_dictionary));
	    પાછા FAIL;
	}
	જો (dest_type == VAR_DICT
		&& may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
	    પાછા FAIL;
	જો (dest_type == VAR_LIST || dest_type == VAR_BLOB)
	{
	    type_T *type;

	    જો (range)
	    {
		type = get_type_on_ઢગલો(cctx, 1);
		જો (need_type(type, &t_સંખ્યા, 0,
					    -2, 0, cctx, FALSE, FALSE) == FAIL)
		પાછા FAIL;
	    }
	    type = get_type_on_ઢગલો(cctx, 0);
	    જો ((dest_type != VAR_BLOB && type->tt_type != VAR_SPECIAL)
		    && need_type(type, &t_સંખ્યા, 0,
					    -1, 0, cctx, FALSE, FALSE) == FAIL)
		પાછા FAIL;
	}
    }

    જો (cctx->ctx_skip == SKIP_YES)
	પાછા OK;

    // Load the dict, list or object.  On the stack we then have:
    // - value (for assignment, not for :unlet)
    // - index
    // - for [a : b] second index
    // - variable
    જો (compile_load_lhs(lhs, var_start, rhs_type, cctx) == FAIL)
	પાછા FAIL;

    જો (dest_type == VAR_LIST
	    || dest_type == VAR_DICT
	    || dest_type == VAR_BLOB
	    || dest_type == VAR_CLASS
	    || dest_type == VAR_OBJECT
	    || dest_type == VAR_ANY)
    {
	જો (is_assign)
	{
	    જો (range)
	    {
		જો (generate_instr_drop(cctx, ISN_STORERANGE, 4) == ખાલી)
		    પાછા FAIL;
	    }
	    નહીંતર
	    {
		isn_T	*isn = generate_instr_drop(cctx, ISN_STOREINDEX, 3);

		જો (isn == ખાલી)
		    પાછા FAIL;
		isn->isn_arg.storeindex.si_vartype = dest_type;
		isn->isn_arg.storeindex.si_class = ખાલી;

		જો (dest_type == VAR_OBJECT)
		{
		    class_T *cl = lhs->lhs_type->tt_class;

		    જો (IS_INTERFACE(cl))
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
	નહીંતર જો (range)
	{
	    જો (generate_instr_drop(cctx, ISN_UNLETRANGE, 3) == ખાલી)
		પાછા FAIL;
	}
	નહીંતર
	{
	    જો (generate_instr_drop(cctx, ISN_UNLETINDEX, 2) == ખાલી)
		પાછા FAIL;
	}
    }
    નહીંતર
    {
	જો (dest_type == VAR_TUPLE)
	    emsg(_(e_tuple_is_immutable));
	નહીંતર
	    emsg(_(e_indexable_type_required));
	પાછા FAIL;
    }

    પાછા OK;
}

/*
 * Generate an instruction to push the default value for "vartype".
 * if "dest_local" is TRUE then for some types no instruction is generated.
 * "skip_store" is set to TRUE if no PUSH instruction is generated.
 * Returns OK or FAIL.
 */
    સ્થિર પૂર્ણાંક
ધકેલો_default_કિંમત(
	cctx_T	    *cctx,
	vartype_T   vartype,
	પૂર્ણાંક	    dest_is_local,
	પૂર્ણાંક	    *skip_store)
{
    પૂર્ણાંક r = OK;

    પસંદગી (vartype)
    {
	સ્થિતિ VAR_BOOL:
	    r = generate_PUSHBOOL(cctx, VVAL_FALSE);
	    તોડો;
	સ્થિતિ VAR_FLOAT:
	    r = generate_PUSHF(cctx, 0.0);
	    તોડો;
	સ્થિતિ VAR_STRING:
	    r = generate_PUSHS(cctx, ખાલી);
	    તોડો;
	સ્થિતિ VAR_BLOB:
	    r = generate_PUSHBLOB(cctx, blob_alloc());
	    તોડો;
	સ્થિતિ VAR_FUNC:
	    r = generate_PUSHFUNC(cctx, ખાલી, &t_func_void, TRUE);
	    તોડો;
	સ્થિતિ VAR_LIST:
	    r = generate_NEWLIST(cctx, 0, FALSE);
	    તોડો;
	સ્થિતિ VAR_TUPLE:
	    r = generate_NEWTUPLE(cctx, 0, FALSE);
	    તોડો;
	સ્થિતિ VAR_DICT:
	    r = generate_NEWDICT(cctx, 0, FALSE);
	    તોડો;
	સ્થિતિ VAR_JOB:
	    r = generate_PUSHJOB(cctx);
	    તોડો;
	સ્થિતિ VAR_CHANNEL:
	    r = generate_PUSHCHANNEL(cctx);
	    તોડો;
	સ્થિતિ VAR_OBJECT:
	    r = generate_PUSHOBJ(cctx);
	    તોડો;
	સ્થિતિ VAR_સંખ્યા:
	સ્થિતિ VAR_UNKNOWN:
	સ્થિતિ VAR_ANY:
	સ્થિતિ VAR_PARTIAL:
	સ્થિતિ VAR_VOID:
	સ્થિતિ VAR_INSTR:
	સ્થિતિ VAR_CLASS:
	સ્થિતિ VAR_TYPEALIAS:
	સ્થિતિ VAR_SPECIAL:  // cannot happen
	    // This is skipped for local variables, they are always
	    // initialized to zero.  But in a "for" or "while" loop
	    // the value may have been changed.
	    જો (dest_is_local && !inside_loop_scope(cctx))
		*skip_store = TRUE;
	    નહીંતર
		r = generate_PUSHNR(cctx, 0);
	    તોડો;
    }
    પાછા r;
}

/*
 * Compile assignment context.  Used when compiling an assignment statement.
 */
પ્રકારવ્યાખ્યા સંરચના cac_S cac_T;
સંરચના cac_S
{
    cmdidx_T	cac_cmdidx;		// assignment command
    char_u	*cac_nextc;		// next character to parse
    lhs_T	cac_lhs;		// lhs of the assignment
    type_T	*cac_rhs_type;		// rhs type of an assignment
    char_u	*cac_op;		// assignment operator
    પૂર્ણાંક		cac_oplen;		// assignment operator length
    char_u	*cac_var_start;		// start of the variable names
    char_u	*cac_var_end;		// end of the variable names
    પૂર્ણાંક		cac_var_ગણતરી;		// number of variables in assignment
    પૂર્ણાંક		cac_var_idx;		// variable index in a list
    પૂર્ણાંક		cac_semicolon;		// semicolon in [var1, var2; var3]
    garray_T	*cac_instr;
    પૂર્ણાંક		cac_instr_ગણતરી;
    પૂર્ણાંક		cac_incdec;
    પૂર્ણાંક		cac_did_generate_slice;
    પૂર્ણાંક		cac_is_decl;
    પૂર્ણાંક		cac_is_const;
    પૂર્ણાંક		cac_start_lnum;
    type_T	*cac_inferred_type;
    પૂર્ણાંક		cac_skip_store;
};

/*
 * Initialize the compile assignment context.
 */
    સ્થિર શૂન્ય
compile_assign_context_init(cac_T *cac, cctx_T *cctx, પૂર્ણાંક cmdidx, char_u *arg)
{
    CLEAR_FIELD(*cac);
    cac->cac_cmdidx = cmdidx;
    cac->cac_instr = &cctx->ctx_instr;
    cac->cac_rhs_type = &t_any;
    cac->cac_is_decl = is_decl_command(cmdidx);
    cac->cac_start_lnum = SOURCING_LNUM;
    cac->cac_instr_ગણતરી = -1;
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
    સ્થિર પૂર્ણાંક
compile_assign_obj_new_arg(char_u **argp, cctx_T *cctx)
{
    char_u *arg = *argp;

    arg += 11;	    // skip "ifargisset"
    પૂર્ણાંક def_arg_idx = getdigits(&arg);
    arg = skipwhite(arg);

    // Use a JUMP_IF_ARG_NOT_SET instruction to skip if the value was not
    // given and the default value is "v:none".
    પૂર્ણાંક ઢગલો_offset = ઢગલો_FRAME_SIZE +
				(cctx->ctx_ufunc->uf_va_name != ખાલી ? 1 : 0);
    પૂર્ણાંક def_arg_ગણતરી = cctx->ctx_ufunc->uf_def_args.ga_len;
    પૂર્ણાંક arg_offset = def_arg_idx - def_arg_ગણતરી - ઢગલો_offset;

    જો (generate_JUMP_IF_ARG(cctx, ISN_JUMP_IF_ARG_NOT_SET,
			     arg_offset) == FAIL)
	પાછા FAIL;

    *argp = arg;
    પાછા OK;
}

/*
 * Translate the increment (++) and decrement (--) operators to the
 * corresponding compound operators (+= or -=).
 *
 * Returns OK on success and FAIL on syntax error.
 */
    સ્થિર પૂર્ણાંક
translate_incdec_op(exarg_T *eap, cac_T *cac)
{
    જો (VIM_ISWHITE(eap->cmd[2]))
    {
	semsg(_(e_no_white_space_allowed_after_str_str),
		eap->cmdidx == CMD_increment ? "++" : "--", eap->cmd);
	પાછા FAIL;
    }
    cac->cac_op = (char_u *)(eap->cmdidx == CMD_increment ? "+=" : "-=");
    cac->cac_oplen = 2;
    cac->cac_incdec = TRUE;

    પાછા OK;
}

/*
 * Process the operator in an assignment statement.
 */
    સ્થિર પૂર્ણાંક
compile_assign_process_operator(
    exarg_T	*eap,
    char_u	*arg,
    cac_T	*cac,
    પૂર્ણાંક		*heredoc,
    char_u	**retstr)
{
    *retstr = ખાલી;

    જો (eap->cmdidx == CMD_increment || eap->cmdidx == CMD_decrement)
	// Change an unary operator to a compound operator
	પાછા translate_incdec_op(eap, cac);

    char_u *sp = cac->cac_nextc;
    cac->cac_nextc = skipwhite(cac->cac_nextc);
    cac->cac_op = cac->cac_nextc;
    cac->cac_oplen = assignment_len(cac->cac_nextc, heredoc);

    જો (cac->cac_var_ગણતરી > 0 && cac->cac_oplen == 0)
    {
	// can be something like "[1, 2]->func()"
	*retstr = arg;
	પાછા FAIL;
    }

    // need white space before and after the operator
    જો (cac->cac_oplen > 0 && (!VIM_ISWHITE(*sp)
		|| !IS_WHITE_OR_NUL(cac->cac_op[cac->cac_oplen])))
    {
	error_white_both(cac->cac_op, cac->cac_oplen);
	પાછા FAIL;
    }

    પાછા OK;
}

/*
 * Find the start of an assignment statement.
 */
    સ્થિર char_u *
compile_assign_compute_start(char_u *arg, પૂર્ણાંક var_ગણતરી)
{
    જો (var_ગણતરી > 0)
	// [var1, var2] = [val1, val2]
	// skip over the "["
	પાછા skipwhite(arg + 1);

    પાછા arg;
}

/*
 * Parse a heredoc assignment starting at "p".  Returns a pointer to the
 * beginning of the heredoc content.
 */
    સ્થિર char_u *
parse_heredoc_assignment(exarg_T *eap, cctx_T *cctx, cac_T *cac)
{
    // [let] varname =<< [trim] {end}
    eap->ea_getline = exarg_getline;
    eap->cookie = cctx;

    list_T *l = heredoc_get(eap, cac->cac_nextc + 3, FALSE, TRUE);
    જો (l == ખાલી)
	પાછા ખાલી;

    list_free(l);
    cac->cac_nextc += STRLEN(cac->cac_nextc);

    પાછા cac->cac_nextc;
}

/*
 * Check the type of a RHS expression in a list assignment statement.
 * The RHS expression is already compiled.  So the type is on the stack.
 */
    સ્થિર પૂર્ણાંક
compile_assign_list_check_rhs_type(cctx_T *cctx, cac_T *cac)
{
    type_T	*stacktype;

    stacktype = cctx->ctx_type_ઢગલો.ga_len == 0 ? &t_void
						: get_type_on_ઢગલો(cctx, 0);
    જો (stacktype->tt_type == VAR_VOID)
    {
	emsg(_(e_cannot_use_void_કિંમત));
	પાછા FAIL;
    }

    જો (stacktype->tt_type != VAR_LIST && stacktype->tt_type != VAR_TUPLE
					&& stacktype->tt_type != VAR_ANY)
    {
	emsg(_(e_list_or_tuple_required));
	પાછા FAIL;
    }

    જો (need_type(stacktype,
		  stacktype->tt_type == VAR_TUPLE ? &t_tuple_any : &t_list_any,
		  TYPECHK_TUPLE_OK, -1, 0, cctx, FALSE, FALSE) == FAIL)
	પાછા FAIL;

    // The check accepts both a list and a tuple. Keep "any", making it a list
    // would reject a tuple later on.
    જો (stacktype->tt_type == VAR_ANY)
	set_type_on_ઢગલો(cctx, &t_any, 0);

    જો (stacktype->tt_type == VAR_TUPLE)
    {
	જો (stacktype->tt_argcount != 1)
	    cac->cac_rhs_type = &t_any;
	નહીંતર
	{
	    જો (stacktype->tt_flags & TTFLAG_VARARGS)
		cac->cac_rhs_type = stacktype->tt_args[0]->tt_member;
	    નહીંતર
		cac->cac_rhs_type = stacktype->tt_args[0];
	}
    }
    નહીંતર જો (stacktype->tt_member != ખાલી)
	cac->cac_rhs_type = stacktype->tt_member;

    પાછા OK;
}

/*
 * In a list assignment statement, if a constant list was used, check the
 * length.  Returns OK if the length check succeeds.  Returns FAIL otherwise.
 */
    સ્થિર પૂર્ણાંક
compile_assign_list_check_length(cctx_T *cctx, cac_T *cac)
{
    પૂર્ણાંક	needed_list_len;
    પૂર્ણાંક	did_check = FALSE;

    needed_list_len = cac->cac_semicolon
				? cac->cac_var_ગણતરી - 1
				: cac->cac_var_ગણતરી;
    જો (cac->cac_instr->ga_len > 0)
    {
	isn_T	*isn = ((isn_T *)cac->cac_instr->ga_data) +
						cac->cac_instr->ga_len - 1;

	જો (isn->isn_type == ISN_NEWLIST || isn->isn_type == ISN_NEWTUPLE)
	{
	    did_check = TRUE;
	    જો (cac->cac_semicolon ?
			isn->isn_arg.સંખ્યા < needed_list_len
			: isn->isn_arg.સંખ્યા != needed_list_len)
	    {
		semsg(_(e_expected_nr_વસ્તુઓ_but_got_nr),
			needed_list_len, (પૂર્ણાંક)isn->isn_arg.સંખ્યા);
		પાછા FAIL;
	    }
	}
    }

    જો (!did_check)
	generate_CHECKLEN(cctx, needed_list_len, cac->cac_semicolon);

    પાછા OK;
}

/*
 * Evaluate the expression for "[var, var] = expr" assignment.
 * A line break may follow the assignment operator "=".
 */
    સ્થિર char_u *
compile_assign_list_પદાવલિ(cctx_T *cctx, cac_T *cac)
{
    char_u *whitep;

    whitep = cac->cac_op + cac->cac_oplen;

    જો (may_get_next_લીટી_error(whitep, &cac->cac_nextc, cctx) == FAIL)
	પાછા ખાલી;

    // compile RHS expression
    જો (compile_expr0(&cac->cac_nextc, cctx) == FAIL)
	પાછા ખાલી;

    જો (cctx->ctx_skip == SKIP_YES)
	// no need to parse more when skipping
	પાછા cac->cac_nextc;

    જો (compile_assign_list_check_rhs_type(cctx, cac) == FAIL)
	પાછા ખાલી;

    // If a constant list was used we can check the length right here.
    જો (compile_assign_list_check_length(cctx, cac) == FAIL)
	પાછા FAIL;

    પાછા cac->cac_nextc;
}

/*
 * Find and return the end of a heredoc or a list of variables assignment
 * statement.  For a single variable assignment statement, returns the current
 * end.
 * Returns NULL on failure.
 */
    સ્થિર char_u *
compile_assign_compute_end(
    exarg_T	*eap,
    cctx_T	*cctx,
    cac_T	*cac,
    પૂર્ણાંક		heredoc)
{
    જો (heredoc)
    {
	cac->cac_nextc = parse_heredoc_assignment(eap, cctx, cac);
	પાછા cac->cac_nextc;
    }

    જો (cac->cac_var_ગણતરી > 0)
    {
	// for "[var, var] = expr" evaluate the expression. The list of
	// variables are processed later.
	// A line break may follow the "=".
	cac->cac_nextc = compile_assign_list_પદાવલિ(cctx, cac);
	પાછા cac->cac_nextc;
    }

    પાછા cac->cac_var_end;
}

/*
 * For "var = expr" evaluate the expression.
 */
    સ્થિર પૂર્ણાંક
compile_assign_single_eval_પદાવલિ(cctx_T *cctx, cac_T *cac)
{
    પૂર્ણાંક		ret = OK;
    char_u	*whitep;
    lhs_T	*lhs = &cac->cac_lhs;
    lvar_T	*lvp;
    lvar_T	save_lhs_lvar;

    // Compile the expression.
    જો (cac->cac_incdec)
	પાછા generate_PUSHNR(cctx, 1);

    // Temporarily hide the new local variable here, it is
    // not available to this expression.
    જો (lhs->lhs_new_local)
    {
	--cctx->ctx_locals.ga_len;

	// Save the local variable value (compiling the RHS expression may
	// create new local variables).
	lvp = ((lvar_T *)cctx->ctx_locals.ga_data) + cctx->ctx_locals.ga_len;
	save_lhs_lvar = *lvp;
    }
    whitep = cac->cac_op + cac->cac_oplen;

    જો (may_get_next_લીટી_error(whitep, &cac->cac_nextc, cctx) == FAIL)
    {
	જો (lhs->lhs_new_local)
	    ++cctx->ctx_locals.ga_len;
	પાછા FAIL;
    }

    ret = compile_expr0_ext(&cac->cac_nextc, cctx, &cac->cac_is_const);
    જો (lhs->lhs_new_local)
    {
	// Restore the local variable value.  Update lhs_lvar as the index of
	// the local variable might have changed.
	lvp = ((lvar_T *)cctx->ctx_locals.ga_data) + cctx->ctx_locals.ga_len;
	*lvp = save_lhs_lvar;
	lhs->lhs_lvar = lvp;

	++cctx->ctx_locals.ga_len;
    }

    પાછા ret;
}

/*
 * When compiling an assignment, set the LHS type to the RHS type.
 */
    સ્થિર પૂર્ણાંક
compile_assign_set_lhs_type_from_rhs(
    cctx_T	*cctx,
    cac_T	*cac,
    lhs_T	*lhs,
    type_T	*rhs_type)
{
    જો (rhs_type->tt_type == VAR_VOID)
    {
	emsg(_(e_cannot_use_void_કિંમત));
	પાછા FAIL;
    }

    type_T *type;

    // An empty list or dict has a &t_unknown member, for a variable that
    // implies &t_any.
    જો (rhs_type == &t_list_empty)
	type = &t_list_any;
    નહીંતર જો (rhs_type == &t_dict_empty)
	type = &t_dict_any;
    નહીંતર જો (rhs_type == &t_unknown)
	type = &t_any;
    નહીંતર
    {
	type = rhs_type;
	cac->cac_inferred_type = rhs_type;
    }

    set_var_type(lhs->lhs_lvar, type, cctx);

    પાછા OK;
}

/*
 * Returns TRUE if the "rhs_type" can be assigned to the "lhs" variable.
 * Used when compiling an assignment statement.
 */
    સ્થિર પૂર્ણાંક
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
    જો (cac->cac_var_ગણતરી > 0)
    {
	where.wt_index = cac->cac_var_idx + 1;
	where.wt_kind = WT_VARIABLE;
    }

    // If assigning to a list or dict member, use the member type.
    // Not for "list[:] =".
    જો (lhs->lhs_has_index &&
	    !has_list_index(cac->cac_var_start + lhs->lhs_varlen, cctx))
	use_type = lhs->lhs_member_type;

    જો (need_type_where(rhs_type, use_type, 0, -1, where, cctx, FALSE,
						cac->cac_is_const) == FAIL)
	પાછા FALSE;

    પાછા TRUE;
}

/*
 * Compare the LHS type with the RHS type in an assignment.
 */
    સ્થિર પૂર્ણાંક
compile_assign_check_type(cctx_T *cctx, cac_T *cac)
{
    lhs_T	*lhs = &cac->cac_lhs;
    type_T	*rhs_type;

    rhs_type = cctx->ctx_type_ઢગલો.ga_len == 0
					? &t_void
					: get_type_on_ઢગલો(cctx, 0);
    cac->cac_rhs_type = rhs_type;

    જો (check_type_is_કિંમત(rhs_type) == FAIL)
	પાછા FAIL;

    જો (lhs->lhs_lvar != ખાલી && (cac->cac_is_decl || !lhs->lhs_has_type))
    {
	જો (rhs_type->tt_type == VAR_FUNC
					|| rhs_type->tt_type == VAR_PARTIAL)
	{
	    // Make sure the variable name can be used as a funcref
	    જો (!lhs->lhs_has_index
				&& var_wrong_func_name(lhs->lhs_name, TRUE))
		પાછા FAIL;
	}

	જો (lhs->lhs_new_local && !lhs->lhs_has_type)
	{
	    // The LHS variable doesn't have a type.  Set it to the RHS type.
	    જો (compile_assign_set_lhs_type_from_rhs(cctx, cac, lhs, rhs_type)
								== FAIL)
		પાછા FAIL;
	}
	નહીંતર જો (*cac->cac_op == '=')
	{
	    જો (!compile_assign_valid_rhs_type(cctx, cac, lhs, rhs_type))
		પાછા FAIL;
	}
    }
    નહીંતર
    {
	// Assigning to a register using @r = "abc"

	type_T *lhs_type = lhs->lhs_member_type;

	// Special case: assigning to @# can use a number or a string.
	// Also: can assign a number to a float.
	જો ((lhs_type == &t_સંખ્યા_or_string || lhs_type == &t_float)
					&& rhs_type->tt_type == VAR_સંખ્યા)
	    lhs_type = &t_સંખ્યા;

	જો (*cac->cac_nextc != '=')
	{
	    જો (need_type(rhs_type, lhs_type, 0, -1, 0, cctx, FALSE,
							FALSE) == FAIL)
		પાછા FAIL;
	}
    }

    પાછા OK;
}

/*
 * Compile the RHS expression in an assignment statement and generate the
 * instructions.
 */
    સ્થિર પૂર્ણાંક
compile_assign_rhs_પદાવલિ(cctx_T *cctx, cac_T *cac)
{
    cac->cac_is_const = FALSE;

    // for "+=", "*=", "..=" etc. first load the current value
    જો (*cac->cac_op != '='
	    && compile_load_lhs_with_index(&cac->cac_lhs, cac->cac_var_start,
								cctx) == FAIL)
	પાછા FAIL;

    // For "var = expr" evaluate the expression.
    જો (cac->cac_var_ગણતરી == 0)
    {
	પૂર્ણાંક	ret;

	// Compile the expression.
	cac->cac_instr_ગણતરી = cac->cac_instr->ga_len;
	ret = compile_assign_single_eval_પદાવલિ(cctx, cac);
	જો (ret == FAIL)
	    પાછા FAIL;
    }
    નહીંતર જો (cac->cac_semicolon && cac->cac_var_idx == cac->cac_var_ગણતરી - 1)
    {
	// For "[var; var] = expr" get the rest of the list
	cac->cac_did_generate_slice = TRUE;
	જો (generate_SLICE(cctx, cac->cac_var_ગણતરી - 1) == FAIL)
	    પાછા FAIL;
    }
    નહીંતર
    {
	// For "[var, var] = expr" get the "var_idx" item from the
	// list.
	પૂર્ણાંક with_op = *cac->cac_op != '=';
	જો (generate_GETITEM(cctx, cac->cac_var_idx, with_op) == FAIL)
	    પાછા FAIL;
    }

    જો (compile_assign_check_type(cctx, cac) == FAIL)
	પાછા FAIL;

    પાછા OK;
}

/*
 * Compile the RHS expression in an assignment
 */
    સ્થિર પૂર્ણાંક
compile_assign_rhs(cctx_T *cctx, cac_T *cac)
{
    lhs_T	*lhs = &cac->cac_lhs;

    જો (cctx->ctx_skip == SKIP_YES)
    {
	જો (cac->cac_oplen > 0 && cac->cac_var_ગણતરી == 0)
	{
	    // skip over the "=" and the expression
	    cac->cac_nextc = skipwhite(cac->cac_op + cac->cac_oplen);
	    (શૂન્ય)compile_expr0(&cac->cac_nextc, cctx);
	}
	પાછા OK;
    }

    // If RHS is specified, then generate instructions for RHS expression
    જો (cac->cac_oplen > 0)
	પાછા compile_assign_rhs_પદાવલિ(cctx, cac);

    જો (cac->cac_cmdidx == CMD_final)
    {
	emsg(_(e_final_requires_a_કિંમત));
	પાછા FAIL;
    }

    જો (cac->cac_cmdidx == CMD_const)
    {
	emsg(_(e_const_requires_a_કિંમત));
	પાછા FAIL;
    }

    જો (!lhs->lhs_has_type || lhs->lhs_dest == dest_option
					|| lhs->lhs_dest == dest_func_option)
    {
	emsg(_(e_type_or_initialization_required));
	પાછા FAIL;
    }

    // variables are always initialized
    જો (GA_GROW_FAILS(cac->cac_instr, 1))
	પાછા FAIL;

    cac->cac_instr_ગણતરી = cac->cac_instr->ga_len;

    પાછા ધકેલો_default_કિંમત(cctx, lhs->lhs_member_type->tt_type,
			      lhs->lhs_dest == dest_local,
			      &cac->cac_skip_store);
}

/*
 * Returns OK if "type" supports compound operator "op_arg" (e.g. +=, -=, %=,
 * etc.).  Compound operators are not supported with a tuple and a dict.
 * Returns FAIL if compound operator is not supported.
 */
    સ્થિર પૂર્ણાંક
check_type_supports_compound_op(type_T *type, char_u op_arg)
{
    જો (type->tt_type == VAR_TUPLE || type->tt_type == VAR_DICT)
    {
	char_u	op[2];

	op[0] = op_arg;
	op[1] = NUL;
	semsg(_(e_wrong_variable_type_for_str_equal), op);
	પાછા FAIL;
    }

    પાછા OK;
}

/*
 * Compile a compound op assignment statement (+=, -=, *=, %=, etc.)
 */
    સ્થિર પૂર્ણાંક
compile_assign_compound_op(cctx_T *cctx, cac_T *cac)
{
    lhs_T	    *lhs = &cac->cac_lhs;
    type_T	    *expected;
    type_T	    *stacktype = ખાલી;

    જો (cac->cac_lhs.lhs_type->tt_type == VAR_TUPLE
	    && check_type_supports_compound_op(cac->cac_lhs.lhs_type,
							*cac->cac_op) == FAIL)
	પાછા FAIL;

    જો (*cac->cac_op == '.')
    {
	expected = lhs->lhs_member_type;
	stacktype = get_type_on_ઢગલો(cctx, 0);
	જો (expected != &t_string
		&& need_type(stacktype, expected, 0, -1, 0, cctx,
					FALSE, FALSE) == FAIL)
	    પાછા FAIL;
	નહીંતર જો (may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
	    પાછા FAIL;
    }
    નહીંતર
    {
	expected = lhs->lhs_member_type;
	stacktype = get_type_on_ઢગલો(cctx, 0);

	જો (check_type_supports_compound_op(expected, *cac->cac_op) == FAIL)
	    પાછા FAIL;

	જો (
		// If variable is float operation with number is OK.
		!(expected == &t_float && (stacktype == &t_સંખ્યા
					|| stacktype == &t_સંખ્યા_bool))
		&& need_type(stacktype, expected, TYPECHK_સંખ્યા_OK, -1, 0,
						cctx, FALSE, FALSE) == FAIL)
	    પાછા FAIL;
    }

    જો (*cac->cac_op == '.')
    {
	જો (generate_CONCAT(cctx, 2) == FAIL)
	    પાછા FAIL;
    }
    નહીંતર જો (*cac->cac_op == '+')
    {
	જો (generate_add_instr(cctx,
		    operator_type(lhs->lhs_member_type, stacktype),
		    lhs->lhs_member_type, stacktype,
		    પદાવલિ_APPEND) == FAIL)
	    પાછા FAIL;
    }
    નહીંતર જો (generate_two_op(cctx, cac->cac_op) == FAIL)
	પાછા FAIL;

    પાછા OK;
}

/*
 * Generate the STORE and SETTYPE instructions for an assignment statement.
 */
    સ્થિર પૂર્ણાંક
compile_assign_generate_store(cctx_T *cctx, cac_T *cac)
{
    lhs_T	*lhs = &cac->cac_lhs;
    પૂર્ણાંક		save_lnum;

    // Use the line number of the assignment for store instruction.
    save_lnum = cctx->ctx_lnum;
    cctx->ctx_lnum = cac->cac_start_lnum - 1;

    જો (lhs->lhs_has_index)
    {
	// Use the info in "lhs" to store the value at the index in the
	// list, dict or object.
	જો (compile_assign_unlet(cac->cac_var_start, &cac->cac_lhs,
				 TRUE, cac->cac_rhs_type, cctx) == FAIL)
	{
	    cctx->ctx_lnum = save_lnum;
	    પાછા FAIL;
	}
    }
    નહીંતર
    {
	જો (cac->cac_is_decl && cac->cac_cmdidx == CMD_const &&
			(lhs->lhs_dest == dest_script
			 || lhs->lhs_dest == dest_script_v9
			 || lhs->lhs_dest == dest_global
			 || lhs->lhs_dest == dest_local))
	    // ":const var": lock the value, but not referenced variables
	    generate_LOCKCONST(cctx);

	type_T	*inferred_type = cac->cac_inferred_type;

	જો ((lhs->lhs_type->tt_type == VAR_DICT
		    || lhs->lhs_type->tt_type == VAR_LIST)
		&& lhs->lhs_type->tt_member != ખાલી
		&& lhs->lhs_type->tt_member != &t_any
		&& lhs->lhs_type->tt_member != &t_unknown)
	    // Set the type in the list or dict, so that it can be
	    // checked, also in legacy script.
	    generate_SETTYPE(cctx, lhs->lhs_type);
	નહીંતર જો (lhs->lhs_type->tt_type == VAR_TUPLE
					&& lhs->lhs_type->tt_argcount != 0)
	    generate_SETTYPE(cctx, lhs->lhs_type);
	નહીંતર જો (inferred_type != ખાલી
		&& (inferred_type->tt_type == VAR_DICT
		    || inferred_type->tt_type == VAR_LIST)
		&& inferred_type->tt_member != ખાલી
		&& inferred_type->tt_member != &t_unknown
		&& inferred_type->tt_member != &t_any)
	    // Set the type in the list or dict, so that it can be
	    // checked, also in legacy script.
	    generate_SETTYPE(cctx, inferred_type);
	નહીંતર જો (inferred_type != ખાલી
				&& inferred_type->tt_type == VAR_TUPLE
				&& inferred_type->tt_argcount > 0)
	    generate_SETTYPE(cctx, inferred_type);

	જો (!cac->cac_skip_store &&
		generate_store_lhs(cctx, &cac->cac_lhs,
				   cac->cac_instr_ગણતરી,
				   cac->cac_is_decl) == FAIL)
	{
	    cctx->ctx_lnum = save_lnum;
	    પાછા FAIL;
	}
    }

    cctx->ctx_lnum = save_lnum;

    પાછા OK;
}

/*
 * Process the variable(s) in an assignment statement
 */
    સ્થિર પૂર્ણાંક
compile_assign_process_variables(
    cctx_T	*cctx,
    cac_T	*cac,
    પૂર્ણાંક		cmdidx,
    પૂર્ણાંક		heredoc,
    પૂર્ણાંક		has_cmd,
    પૂર્ણાંક		has_argisset_prefix,
    પૂર્ણાંક		jump_instr_idx)
{
    /*
     * Loop over variables in "[var, var] = expr".
     * For "name = expr" and "var name: type" this is done only once.
     */
    માટે (cac->cac_var_idx = 0; cac->cac_var_idx == 0 ||
	    cac->cac_var_idx < cac->cac_var_ગણતરી; cac->cac_var_idx++)
    {
	જો (cac->cac_var_start[0] == '_'
				&& !eval_isnamec(cac->cac_var_start[1]))
	{
	    // Ignore underscore in "[a, _, b] = list".
	    જો (cac->cac_var_ગણતરી > 0)
	    {
		cac->cac_var_start = skipwhite(cac->cac_var_start + 2);
		ચાલુ;
	    }
	    emsg(_(e_cannot_use_underscore_here));
	    પાછા FAIL;
	}
	vim_free(cac->cac_lhs.lhs_name);

	/*
	 * Figure out the LHS type and other properties.
	 */
	જો (compile_assign_lhs(cac->cac_var_start, &cac->cac_lhs, cmdidx,
		    cac->cac_is_decl, heredoc, has_cmd,
		    cac->cac_oplen, cctx) == FAIL)
	    પાછા FAIL;

	// Compile the RHS expression
	જો (heredoc)
	{
	    SOURCING_LNUM = cac->cac_start_lnum;
	    જો (cac->cac_lhs.lhs_has_type
		    && need_type(&t_list_string, cac->cac_lhs.lhs_type,
				0, -1, 0, cctx, FALSE, FALSE) == FAIL)
		પાછા FAIL;
	}
	નહીંતર
	{
	    જો (compile_assign_rhs(cctx, cac) == FAIL)
		પાછા FAIL;
	    જો (cac->cac_var_ગણતરી == 0)
		cac->cac_var_end = cac->cac_nextc;
	}

	// no need to parse more when skipping
	જો (cctx->ctx_skip == SKIP_YES)
	    તોડો;

	જો (cac->cac_oplen > 0 && *cac->cac_op != '=')
	{
	    જો (compile_assign_compound_op(cctx, cac) == FAIL)
		પાછા FAIL;
	}

	// generate the store instructions
	જો (compile_assign_generate_store(cctx, cac) == FAIL)
	    પાછા FAIL;

	જો (cac->cac_var_idx + 1 < cac->cac_var_ગણતરી)
	    cac->cac_var_start = skipwhite(cac->cac_lhs.lhs_end + 1);

	જો (has_argisset_prefix)
	{
	    // set instruction index in JUMP_IF_ARG_SET to here
	    isn_T *isn = ((isn_T *)cac->cac_instr->ga_data) + jump_instr_idx;
	    isn->isn_arg.jumparg.jump_where = cac->cac_instr->ga_len;
	}
    }

    પાછા OK;
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
    સ્થિર char_u *
compile_assignment(
    char_u	*arg_start,
    exarg_T	*eap,
    cmdidx_T	cmdidx,
    cctx_T	*cctx)
{
    cac_T	cac;
    char_u	*arg = arg_start;
    char_u	*retstr = ખાલી;
    પૂર્ણાંક		heredoc = FALSE;
    પૂર્ણાંક		jump_instr_idx;

    compile_assign_context_init(&cac, cctx, cmdidx, arg);

    jump_instr_idx = cac.cac_instr->ga_len;

    // process object variable initialization in a new() constructor method
    પૂર્ણાંક	has_argisset_prefix = STRNCMP(arg, "ifargisset ", 11) == 0;
    જો (has_argisset_prefix &&
			compile_assign_obj_new_arg(&arg, cctx) == FAIL)
	જાઓ theend;

    // Skip over the "varname" or "[varname, varname]" to get to any "=".
    cac.cac_nextc = skip_var_list(arg, TRUE, &cac.cac_var_ગણતરી,
						&cac.cac_semicolon, TRUE);
    જો (cac.cac_nextc == ખાલી)
	પાછા *arg == '[' ? arg : ખાલી;

    જો (compile_assign_process_operator(eap, arg, &cac, &heredoc,
							&retstr) == FAIL)
	પાછા retstr;

    // Compute the start of the assignment
    cac.cac_var_start = compile_assign_compute_start(arg, cac.cac_var_ગણતરી);

    // Compute the end of the assignment
    cac.cac_var_end = compile_assign_compute_end(eap, cctx, &cac, heredoc);
    જો (cac.cac_var_end == ખાલી)
	પાછા ખાલી;

    પૂર્ણાંક has_cmd = cac.cac_var_start > eap->cmd;

    /* process the variable(s) */
    જો (compile_assign_process_variables(cctx, &cac, cmdidx, heredoc,
					 has_cmd, has_argisset_prefix,
					 jump_instr_idx) == FAIL)
	જાઓ theend;

    // For "[var, var] = expr" drop the "expr" value.
    // Also for "[var, var; _] = expr".
    જો (cctx->ctx_skip != SKIP_YES && cac.cac_var_ગણતરી > 0 &&
	    (!cac.cac_semicolon || !cac.cac_did_generate_slice))
    {
	જો (generate_instr_drop(cctx, ISN_DROP, 1) == ખાલી)
	    જાઓ theend;
    }

    retstr = skipwhite(cac.cac_var_end);

theend:
    vim_free(cac.cac_lhs.lhs_name);
    પાછા retstr;
}

/*
 * Check for an assignment at "eap->cmd", compile it if found.
 * Return NOTDONE if there is none, FAIL for failure, OK if done.
 */
    સ્થિર પૂર્ણાંક
may_compile_assignment(exarg_T *eap, char_u **લીટી, cctx_T *cctx)
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
    જો (p > eap->cmd && *p != NUL)
    {
	char_u *var_end;
	પૂર્ણાંક	oplen;
	પૂર્ણાંક	heredoc;

	જો (eap->cmd[0] == '@')
	    var_end = eap->cmd + 2;
	નહીંતર
	    var_end = find_name_end(pskip, ખાલી, ખાલી,
					FNE_CHECK_START | FNE_INCL_BR);
	oplen = assignment_len(skipwhite(var_end), &heredoc);
	જો (oplen > 0)
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
	    જો (*eap->cmd == '&'
		    || *eap->cmd == '$'
		    || *eap->cmd == '@'
		    || ((len) > 2 && eap->cmd[1] == ':')
		    || STRNCMP(eap->cmd, "g:[", 3) == 0
		    || variable_exists(eap->cmd, len, cctx))
	    {
		*લીટી = compile_assignment(eap->cmd, eap, CMD_SIZE, cctx);
		જો (*લીટી == ખાલી || *લીટી == eap->cmd)
		    પાછા FAIL;
		પાછા OK;
	    }
	}
    }

    // might be "[var, var] = expr" or "ifargisset this.member = expr"
    જો (*eap->cmd == '[' || STRNCMP(eap->cmd, "ifargisset ", 11) == 0)
    {
	*લીટી = compile_assignment(eap->cmd, eap, CMD_SIZE, cctx);
	જો (*લીટી == ખાલી)
	    પાછા FAIL;
	જો (*લીટી != eap->cmd)
	    પાછા OK;
    }
    પાછા NOTDONE;
}

/*
 * Check if arguments of "ufunc" shadow variables in "cctx".
 * Return OK or FAIL.
 */
    સ્થિર પૂર્ણાંક
check_args_shadowing(ufunc_T *ufunc, cctx_T *cctx)
{
    પૂર્ણાંક	    i;
    char_u  *arg;
    પૂર્ણાંક	    r = OK;

    // Make sure arguments are not found when compiling a second time.
    ufunc->uf_args_visible = 0;

    // Check for arguments shadowing variables from the context.
    માટે (i = 0; i < ufunc->uf_args.ga_len; ++i)
    {
	arg = ((char_u **)(ufunc->uf_args.ga_data))[i];
	જો (check_defined(arg, STRLEN(arg), cctx, ખાલી, TRUE) == FAIL)
	{
	    r = FAIL;
	    તોડો;
	}
    }
    ufunc->uf_args_visible = ufunc->uf_args.ga_len;
    પાછા r;
}

#ifdef HAS_MESSAGE_WINDOW
/*
 * Get a count before a command.  Can only be a number.
 * Returns zero if there is no count.
 * Returns -1 if there is something wrong.
 */
    સ્થિર long
get_cmd_ગણતરી(char_u *લીટી, exarg_T *eap)
{
    char_u *p;

    // skip over colons and white space
    માટે (p = લીટી; *p == ':' || VIM_ISWHITE(*p); ++p)
	;
    જો (!SAFE_isdigit(*p))
    {
	// The command or modifiers must be following.  Assume a lower case
	// character means there is a modifier.
	જો (p < eap->cmd && !vim_islower(*p))
	{
	    emsg(_(e_invalid_range));
	    પાછા -1;
	}
	પાછા 0;
    }
    પાછા atol((અક્ષર *)p);
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

    જો (debug_break_level > 0 || may_break_in_function(ufunc))
	પાછા CT_DEBUG;
#ifdef FEAT_PROFILE
    જો (do_profiling == PROF_YES)
    {
	જો (!ufunc->uf_profiling && has_profiling(FALSE, ufunc->uf_name, ખાલી,
							    &ufunc->uf_hash))
	    func_do_profile(ufunc);
	જો (ufunc->uf_profiling)
	    પાછા CT_PROFILE;
    }
#endif
    પાછા CT_NONE;
}

/*
 * Free the compiled instructions saved for a def function.  This is used when
 * compiling a def function and the function was compiled before.
 * The index is reused.
 */
    સ્થિર શૂન્ય
clear_def_function(ufunc_T *ufunc, compiletype_T compile_type)
{
    isn_T	*instr_dest = ખાલી;
    dfunc_T	*dfunc;

    dfunc = ((dfunc_T *)def_functions.ga_data) + ufunc->uf_dfunc_idx;

    પસંદગી (compile_type)
    {
	સ્થિતિ CT_PROFILE:
#ifdef FEAT_PROFILE
	    instr_dest = dfunc->df_instr_prof; તોડો;
#endif
	સ્થિતિ CT_NONE:   instr_dest = dfunc->df_instr; તોડો;
	સ્થિતિ CT_DEBUG:  instr_dest = dfunc->df_instr_debug; તોડો;
    }

    જો (instr_dest != ખાલી)
	// Was compiled in this mode before: Free old instructions.
	delete_def_function_contents(dfunc, FALSE);

    ga_clear_strings(&dfunc->df_var_names);
    dfunc->df_defer_var_idx = 0;
}

/*
 * Add a function to the list of :def functions.
 * This sets "ufunc->uf_dfunc_idx" but the function isn't compiled yet.
 */
    સ્થિર પૂર્ણાંક
add_def_function(ufunc_T *ufunc)
{
    dfunc_T *dfunc;

    જો (def_functions.ga_len == 0)
    {
	// The first position is not used, so that a zero uf_dfunc_idx means it
	// wasn't set.
	જો (GA_GROW_FAILS(&def_functions, 1))
	    પાછા FAIL;
	++def_functions.ga_len;
    }

    // Add the function to "def_functions".
    જો (GA_GROW_FAILS(&def_functions, 1))
	પાછા FAIL;
    dfunc = ((dfunc_T *)def_functions.ga_data) + def_functions.ga_len;
    CLEAR_POINTER(dfunc);
    dfunc->df_idx = def_functions.ga_len;
    ufunc->uf_dfunc_idx = dfunc->df_idx;
    dfunc->df_ufunc = ufunc;
    dfunc->df_name = vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
    ga_init2(&dfunc->df_var_names, કદ(char_u *), 10);
    ++dfunc->df_refcount;
    ++def_functions.ga_len;
    પાછા OK;
}

    સ્થિર પૂર્ણાંક
compile_dfunc_ufunc_init(
    ufunc_T		*ufunc,
    cctx_T		*outer_cctx,
    compiletype_T	compile_type,
    પૂર્ણાંક			*new_def_function)
{
    // When using a function that was compiled before: Free old instructions.
    // The index is reused.  Otherwise add a new entry in "def_functions".
    જો (ufunc->uf_dfunc_idx > 0)
	clear_def_function(ufunc, compile_type);
    નહીંતર
    {
	જો (add_def_function(ufunc) == FAIL)
	    પાછા FAIL;

	*new_def_function = TRUE;
    }

    જો ((ufunc->uf_flags & FC_CLOSURE) && outer_cctx == ખાલી)
    {
	semsg(_(e_compiling_closure_without_context_str),
						printable_func_name(ufunc));
	પાછા FAIL;
    }

    ufunc->uf_def_status = UF_COMPILING;

    પાછા OK;
}

/*
 * Initialize the compilation context for compiling a def function.
 */
    સ્થિર શૂન્ય
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
    ga_init2(&cctx->ctx_locals, કદ(lvar_T), 10);
    // Each entry on the type stack consists of two type pointers.
    ga_init2(&cctx->ctx_type_ઢગલો, કદ(type2_T), 50);
    cctx->ctx_type_list = &ufunc->uf_type_list;
    ga_init2(&cctx->ctx_instr, કદ(isn_T), 50);
}

/*
 * For an object constructor, generate instruction to setup "this" (the first
 * local variable) and to initialize the object variables.
 */
    સ્થિર પૂર્ણાંક
obj_constructor_prologue(ufunc_T *ufunc, cctx_T *cctx)
{
    generate_CONSTRUCT(cctx, ufunc->uf_class);

    માટે (પૂર્ણાંક i = 0; i < ufunc->uf_class->class_obj_member_ગણતરી; ++i)
    {
	ocmember_T *m = &ufunc->uf_class->class_obj_members[i];

	જો (i < 2 && IS_ENUM(ufunc->uf_class))
	    // The first two object variables in an enum are the name
	    // and the ordinal.  These are set by the ISN_CONSTRUCT
	    // instruction.  So don't generate instructions to set
	    // these variables.
	    ચાલુ;

	જો (m->ocm_init != ખાલી)
	{
	    char_u	*પદાવલિ = m->ocm_init;
	    sctx_T	save_current_sctx;
	    પૂર્ણાંક		change_sctx = FALSE;

	    // If the member variable initialization script context is
	    // different from the current script context, then change it.
	    જો (current_sctx.sc_sid != m->ocm_init_sctx.sc_sid)
		change_sctx = TRUE;

	    જો (change_sctx)
	    {
		// generate an instruction to change the script context to the
		// member variable initialization script context.
		save_current_sctx = current_sctx;
		current_sctx = m->ocm_init_sctx;
		generate_SCRIPTCTX_SET(cctx, current_sctx);
	    }

	    પૂર્ણાંક r = compile_expr0(&પદાવલિ, cctx);

	    જો (change_sctx)
	    {
		// restore the previous script context
		current_sctx = save_current_sctx;
		generate_SCRIPTCTX_SET(cctx, current_sctx);
	    }

	    જો (r == FAIL)
		પાછા FAIL;

	    જો (!ends_excmd2(m->ocm_init, પદાવલિ))
	    {
		semsg(_(e_trailing_characters_str), પદાવલિ);
		પાછા FAIL;
	    }

	    type_T	*type = get_type_on_ઢગલો(cctx, 0);
	    જો (m->ocm_type->tt_type == VAR_ANY
		    && !(m->ocm_flags & OCMFLAG_HAS_TYPE)
		    && type->tt_type != VAR_SPECIAL)
	    {
		// If the member variable type is not yet set, then use
		// the initialization expression type.
		m->ocm_type = type;
	    }
	    નહીંતર
	    {
		// The type of the member initialization expression is
		// determined at run time.  Add a runtime type check.
		where_T	where = WHERE_INIT;
		where.wt_kind = WT_MEMBER;
		where.wt_func_name = (અક્ષર *)m->ocm_name.string;
		જો (need_type_where(type, m->ocm_type, 0, -1,
					where, cctx, FALSE, FALSE) == FAIL)
		    પાછા FAIL;
	    }
	}
	નહીંતર
	    ધકેલો_default_કિંમત(cctx, m->ocm_type->tt_type, FALSE, ખાલી);

	જો (((m->ocm_type->tt_type == VAR_DICT
			|| m->ocm_type->tt_type == VAR_LIST)
		    && m->ocm_type->tt_member != ખાલી
		    && m->ocm_type->tt_member != &t_any
		    && m->ocm_type->tt_member != &t_unknown)
		|| (m->ocm_type->tt_type == VAR_TUPLE
		    && m->ocm_type->tt_argcount > 0))
	    // Set the type in the list, tuple or dict, so that it can be
	    // checked, also in legacy script.
	    generate_SETTYPE(cctx, m->ocm_type);

	generate_STORE_THIS(cctx, i);
    }

    પાછા OK;
}

/*
 * For an object method and an constructor, generate instruction to setup
 * "this" (the first local variable).  For a constructor, generate instructions
 * to initialize the object variables.
 */
    સ્થિર પૂર્ણાંક
obj_method_prologue(ufunc_T *ufunc, cctx_T *cctx)
{
    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data) + ufunc->uf_dfunc_idx;

    જો (GA_GROW_FAILS(&dfunc->df_var_names, 1))
	પાછા FAIL;

    ((char_u **)dfunc->df_var_names.ga_data)[0] =
						vim_strsave((char_u *)"this");
    ++dfunc->df_var_names.ga_len;

    // In the constructor allocate memory for the object and initialize the
    // object members.
    જો (IS_CONSTRUCTOR_METHOD(ufunc))
	પાછા obj_constructor_prologue(ufunc, cctx);

    પાછા OK;
}

/*
 * Produce instructions for the default values of optional arguments.
 */
    સ્થિર પૂર્ણાંક
compile_def_function_default_args(
    ufunc_T	*ufunc,
    garray_T	*instr,
    cctx_T	*cctx)
{
    પૂર્ણાંક	ગણતરી = ufunc->uf_def_args.ga_len;
    પૂર્ણાંક	first_def_arg = ufunc->uf_args.ga_len - ગણતરી;
    પૂર્ણાંક	i;
    પૂર્ણાંક	off = ઢગલો_FRAME_SIZE + (ufunc->uf_va_name != ખાલી ? 1 : 0);
    પૂર્ણાંક	did_set_arg_type = FALSE;

    // Produce instructions for the default values of optional arguments.
    SOURCING_LNUM = 0;  // line number unknown
    માટે (i = 0; i < ગણતરી; ++i)
    {
	char_u *arg = ((char_u **)(ufunc->uf_def_args.ga_data))[i];
	જો (STRCMP(arg, "v:none") == 0)
	    // "arg = v:none" means the argument is optional without
	    // setting a value when the argument is missing.
	    ચાલુ;

	type_T	*val_type;
	પૂર્ણાંક		arg_idx = first_def_arg + i;
	where_T	where = WHERE_INIT;
	પૂર્ણાંક		jump_instr_idx = instr->ga_len;
	isn_T	*isn;

	// Use a JUMP_IF_ARG_SET instruction to skip if the value was given.
	જો (generate_JUMP_IF_ARG(cctx, ISN_JUMP_IF_ARG_SET,
						i - ગણતરી - off) == FAIL)
	    પાછા FAIL;

	// Make sure later arguments are not found.
	ufunc->uf_args_visible = arg_idx;

	પૂર્ણાંક r = compile_expr0(&arg, cctx);
	જો (r == FAIL)
	    પાછા FAIL;

	// If no type specified use the type of the default value.
	// Otherwise check that the default value type matches the
	// specified type.
	val_type = get_type_on_ઢગલો(cctx, 0);
	where.wt_index = arg_idx + 1;
	where.wt_kind = WT_ARGUMENT;
	જો (ufunc->uf_arg_types[arg_idx] == &t_unknown)
	{
	    did_set_arg_type = TRUE;
	    ufunc->uf_arg_types[arg_idx] = val_type;
	}
	નહીંતર જો (need_type_where(val_type, ufunc->uf_arg_types[arg_idx],
		    0, -1, where, cctx, FALSE, FALSE) == FAIL)
	    પાછા FAIL;

	જો (generate_STORE(cctx, ISN_STORE, i - ગણતરી - off, ખાલી) == FAIL)
	    પાછા FAIL;

	// set instruction index in JUMP_IF_ARG_SET to here
	isn = ((isn_T *)instr->ga_data) + jump_instr_idx;
	isn->isn_arg.jumparg.jump_where = instr->ga_len;
    }

    જો (did_set_arg_type)
	set_function_type(ufunc);

    પાછા OK;
}

/*
 * Compile def function body.  Loop over all the lines in the function and
 * generate instructions.
 */
    સ્થિર પૂર્ણાંક
compile_def_function_body(
    પૂર્ણાંક		last_func_lnum,
    પૂર્ણાંક		check_return_type,
    garray_T	*લીટીઓ_to_free,
    અક્ષર	**errormsg,
    cctx_T	*cctx)
{
    char_u	*લીટી = ખાલી;
    char_u	*p;
    પૂર્ણાંક		did_emsg_before = did_emsg;
#ifdef FEAT_PROFILE
    પૂર્ણાંક		prof_lnum = -1;
#endif
    પૂર્ણાંક		debug_lnum = -1;

    માટે (;;)
    {
	exarg_T	    ea;
	પૂર્ણાંક	    starts_with_colon = FALSE;
	char_u	    *cmd;
	cmdmod_T    local_cmdmod;

	// Bail out on the first error to avoid a flood of errors and report
	// the right line number when inside try/catch.
	જો (did_emsg_before != did_emsg)
	    પાછા FAIL;

	જો (લીટી != ખાલી && *લીટી == '|')
	    // the line continues after a '|'
	    ++લીટી;
	નહીંતર જો (લીટી != ખાલી && *skipwhite(લીટી) != NUL
		&& !(*લીટી == '#' && (લીટી == cctx->ctx_લીટી_start
						    || VIM_ISWHITE(લીટી[-1]))))
	{
	    semsg(_(e_trailing_characters_str), લીટી);
	    પાછા FAIL;
	}
	નહીંતર જો (લીટી != ખાલી && vim9_bad_comment(skipwhite(લીટી)))
	    પાછા FAIL;
	નહીંતર
	{
	    લીટી = next_લીટી_from_context(cctx, FALSE);
	    જો (cctx->ctx_lnum >= last_func_lnum)
	    {
		// beyond the last line
#ifdef FEAT_PROFILE
		જો (cctx->ctx_skip != SKIP_YES)
		    may_generate_prof_end(cctx, prof_lnum);
#endif
		તોડો;
	    }
	    // Make a copy, splitting off nextcmd and removing trailing spaces
	    // may change it.
	    જો (લીટી != ખાલી)
	    {
		લીટી = vim_strsave(લીટી);
		જો (ga_add_string(લીટીઓ_to_free, લીટી) == FAIL)
		{
		    vim_free(લીટી);
		    પાછા FAIL;
		}
	    }
	}

	CLEAR_FIELD(ea);
	ea.cmdlinep = &લીટી;
	ea.cmd = skipwhite(લીટી);
	ea.skip = cctx->ctx_skip == SKIP_YES;

	જો (*ea.cmd == '#')
	{
	    // "#" starts a comment, but "#{" is an error
	    જો (vim9_bad_comment(ea.cmd))
		પાછા FAIL;
	    લીટી = (char_u *)"";
	    ચાલુ;
	}

#ifdef FEAT_PROFILE
	જો (cctx->ctx_compile_type == CT_PROFILE && cctx->ctx_lnum != prof_lnum
						  && cctx->ctx_skip != SKIP_YES)
	{
	    may_generate_prof_end(cctx, prof_lnum);

	    prof_lnum = cctx->ctx_lnum;
	    generate_instr(cctx, ISN_PROF_START);
	}
#endif
	જો (cctx->ctx_compile_type == CT_DEBUG && cctx->ctx_lnum != debug_lnum
						  && cctx->ctx_skip != SKIP_YES)
	{
	    debug_lnum = cctx->ctx_lnum;
	    generate_instr_debug(cctx);
	}
	cctx->ctx_prev_lnum = cctx->ctx_lnum + 1;

	// Some things can be recognized by the first character.
	પસંદગી (*ea.cmd)
	{
	    સ્થિતિ '}':
		{
		    // "}" ends a block scope
		    scopetype_T stype = cctx->ctx_scope == ખાલી
					  ? NO_SCOPE : cctx->ctx_scope->se_type;

		    જો (stype == BLOCK_SCOPE)
		    {
			compile_endblock(cctx);
			લીટી = ea.cmd;
		    }
		    નહીંતર
		    {
			emsg(_(e_using_rcurly_outside_if_block_scope));
			પાછા FAIL;
		    }
		    જો (લીટી != ખાલી)
			લીટી = skipwhite(ea.cmd + 1);
		    ચાલુ;
		}

	    સ્થિતિ '{':
		// "{" starts a block scope
		// "{'a': 1}->func() is something else
		જો (ends_excmd(*skipwhite(ea.cmd + 1)))
		{
		    લીટી = compile_block(ea.cmd, cctx);
		    ચાલુ;
		}
		તોડો;
	}

	/*
	 * COMMAND MODIFIERS
	 */
	cctx->ctx_has_cmdmod = FALSE;
	જો (parse_command_modifiers(&ea, errormsg, &local_cmdmod, FALSE)
								       == FAIL)
	    પાછા FAIL;
	generate_cmdmods(cctx, &local_cmdmod);
	undo_cmdmod(&local_cmdmod);

	// Check if there was a colon after the last command modifier or before
	// the current position.
	માટે (p = ea.cmd; p >= લીટી; --p)
	{
	    જો (*p == ':')
		starts_with_colon = TRUE;
	    જો (p < ea.cmd && !VIM_ISWHITE(*p))
		તોડો;
	}

	// Skip ":call" to get to the function name, unless using :legacy
	p = ea.cmd;
	જો (!(local_cmdmod.cmod_flags & CMOD_LEGACY))
	{
	    જો (checkforcmd(&ea.cmd, "call", 3))
	    {
		જો (*ea.cmd == '(')
		    // not for "call()"
		    ea.cmd = p;
		નહીંતર
		    ea.cmd = skipwhite(ea.cmd);
	    }

	    જો (!starts_with_colon)
	    {
		પૂર્ણાંક	    assign;

		// Check for assignment after command modifiers.
		assign = may_compile_assignment(&ea, &લીટી, cctx);
		જો (assign == OK)
		    જાઓ nextline;
		જો (assign == FAIL)
		    પાછા FAIL;
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
	જો ((*cmd != '$' || starts_with_colon)
		&& (starts_with_colon
		    || !(*cmd == '\''
			|| (cmd[0] == '0' && cmd[1] == 'z')
			|| (cmd[0] != NUL && cmd[0] == cmd[1]
					    && (*cmd == '+' || *cmd == '-'))
			|| સંખ્યા_method(cmd))))
	{
	    ea.cmd = skip_range(ea.cmd, TRUE, ખાલી);
	    જો (ea.cmd > cmd)
	    {
		જો (!starts_with_colon
				   && !(local_cmdmod.cmod_flags & CMOD_LEGACY))
		{
		    semsg(_(e_colon_required_before_range_str), cmd);
		    પાછા FAIL;
		}
		ea.addr_ગણતરી = 1;
		જો (ends_excmd2(લીટી, ea.cmd))
		{
		    // A range without a command: jump to the line.
		    generate_EXEC(cctx, ISN_EXECRANGE,
					      vim_strnsave(cmd, ea.cmd - cmd));
		    લીટી = ea.cmd;
		    જાઓ nextline;
		}
	    }
	}
	p = find_ex_command(&ea, ખાલી,
		starts_with_colon || (local_cmdmod.cmod_flags & CMOD_LEGACY)
						  ? ખાલી : item_exists, cctx);

	જો (p == ખાલી)
	{
	    જો (cctx->ctx_skip != SKIP_YES)
		semsg(_(e_ambiguous_use_of_user_defined_command_str), ea.cmd);
	    પાછા FAIL;
	}

	// When using ":legacy cmd" always use compile_exec().
	જો (local_cmdmod.cmod_flags & CMOD_LEGACY)
	{
	    char_u *start = ea.cmd;

	    પસંદગી (ea.cmdidx)
	    {
		સ્થિતિ CMD_if:
		સ્થિતિ CMD_elseif:
		સ્થિતિ CMD_else:
		સ્થિતિ CMD_endif:
		સ્થિતિ CMD_for:
		સ્થિતિ CMD_endfor:
		સ્થિતિ CMD_continue:
		સ્થિતિ CMD_break:
		સ્થિતિ CMD_while:
		સ્થિતિ CMD_endwhile:
		સ્થિતિ CMD_try:
		સ્થિતિ CMD_catch:
		સ્થિતિ CMD_finally:
		સ્થિતિ CMD_endtry:
			semsg(_(e_cannot_use_legacy_with_command_str), ea.cmd);
			પાછા FAIL;
		મૂળભૂત: તોડો;
	    }

	    // ":legacy return expr" needs to be handled differently.
	    જો (checkforcmd(&start, "return", 4))
		ea.cmdidx = CMD_return;
	    નહીંતર
		ea.cmdidx = CMD_legacy;
	}

	જો (p == ea.cmd && ea.cmdidx != CMD_SIZE)
	{
	    // "eval" is used for "val->func()" and "var" for "var = val", then
	    // "p" is equal to "ea.cmd" for a valid command.
	    જો (ea.cmdidx == CMD_eval || ea.cmdidx == CMD_var)
		;
	    નહીંતર જો (cctx->ctx_skip == SKIP_YES)
	    {
		લીટી += STRLEN(લીટી);
		જાઓ nextline;
	    }
	    નહીંતર
	    {
		semsg(_(e_command_not_recognized_str), ea.cmd);
		પાછા FAIL;
	    }
	}

	જો ((cctx->ctx_had_return || cctx->ctx_had_throw)
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
	    પાછા FAIL;
	}

	// When processing the end of an if-else or try-catch block, don't
	// clear the "ctx_had_throw" flag.  If such a block ends in a "throw"
	// statement, then it is considered to end in a "return" statement.
	// The "ctx_had_throw" is cleared immediately after processing the
	// block ending statement.
	// Otherwise, clear the "had_throw" flag.
	જો (ea.cmdidx != CMD_else && ea.cmdidx != CMD_elseif
						&& ea.cmdidx != CMD_endif
						&& ea.cmdidx != CMD_catch
						&& ea.cmdidx != CMD_finally
						&& ea.cmdidx != CMD_endtry)
	    cctx->ctx_had_throw = FALSE;

	p = skipwhite(p);
	જો (ea.cmdidx != CMD_SIZE
			    && ea.cmdidx != CMD_write && ea.cmdidx != CMD_read)
	{
	    જો (ea.cmdidx >= 0)
		ea.argt = excmd_get_argt(ea.cmdidx);
	    જો ((ea.argt & EX_BANG) && *p == '!')
	    {
		ea.forceit = TRUE;
		p = skipwhite(p + 1);
	    }
	    જો ((ea.argt & EX_RANGE) == 0 && ea.addr_ગણતરી > 0)
	    {
		emsg(_(e_no_range_allowed));
		પાછા FAIL;
	    }
	}

	પસંદગી (ea.cmdidx)
	{
	    સ્થિતિ CMD_def:
	    સ્થિતિ CMD_function:
		    ea.arg = p;
		    લીટી = compile_nested_function(&ea, cctx, લીટીઓ_to_free);
		    તોડો;

	    સ્થિતિ CMD_return:
		    લીટી = compile_return(p, check_return_type,
				 local_cmdmod.cmod_flags & CMOD_LEGACY, cctx);
		    cctx->ctx_had_return = TRUE;
		    તોડો;

	    સ્થિતિ CMD_let:
		    emsg(_(e_cannot_use_let_in_vim9_script));
		    તોડો;
	    સ્થિતિ CMD_var:
	    સ્થિતિ CMD_final:
	    સ્થિતિ CMD_const:
	    સ્થિતિ CMD_increment:
	    સ્થિતિ CMD_decrement:
		    લીટી = compile_assignment(p, &ea, ea.cmdidx, cctx);
		    જો (લીટી == p)
		    {
			emsg(_(e_invalid_assignment));
			લીટી = ખાલી;
		    }
		    તોડો;

	    સ્થિતિ CMD_unlet:
	    સ્થિતિ CMD_unlockvar:
	    સ્થિતિ CMD_lockvar:
		    લીટી = compile_unletlock(p, &ea, cctx);
		    તોડો;

	    સ્થિતિ CMD_import:
		    emsg(_(e_import_can_only_be_used_in_script));
		    લીટી = ખાલી;
		    તોડો;

	    સ્થિતિ CMD_if:
		    લીટી = compile_if(p, cctx);
		    તોડો;
	    સ્થિતિ CMD_elseif:
		    લીટી = compile_elseif(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    cctx->ctx_had_throw = FALSE;
		    તોડો;
	    સ્થિતિ CMD_else:
		    લીટી = compile_else(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    cctx->ctx_had_throw = FALSE;
		    તોડો;
	    સ્થિતિ CMD_endif:
		    લીટી = compile_endif(p, cctx);
		    cctx->ctx_had_throw = FALSE;
		    તોડો;

	    સ્થિતિ CMD_while:
		    લીટી = compile_while(p, cctx);
		    તોડો;
	    સ્થિતિ CMD_endwhile:
		    લીટી = compile_endwhile(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    તોડો;

	    સ્થિતિ CMD_for:
		    લીટી = compile_for(p, cctx);
		    તોડો;
	    સ્થિતિ CMD_endfor:
		    લીટી = compile_endfor(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    તોડો;
	    સ્થિતિ CMD_continue:
		    લીટી = compile_continue(p, cctx);
		    // "continue" unconditionally jumps back to the loop,
		    // like a "return".
		    cctx->ctx_had_return = TRUE;
		    તોડો;
	    સ્થિતિ CMD_break:
		    લીટી = compile_break(p, cctx);
		    // "break" unconditionally exits the loop, like a "return".
		    cctx->ctx_had_return = TRUE;
		    તોડો;

	    સ્થિતિ CMD_try:
		    લીટી = compile_try(p, cctx);
		    તોડો;
	    સ્થિતિ CMD_catch:
		    લીટી = compile_catch(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    cctx->ctx_had_throw = FALSE;
		    તોડો;
	    સ્થિતિ CMD_finally:
		    લીટી = compile_finally(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    cctx->ctx_had_throw = FALSE;
		    તોડો;
	    સ્થિતિ CMD_endtry:
		    લીટી = compile_endtry(p, cctx);
		    cctx->ctx_had_throw = FALSE;
		    તોડો;
	    સ્થિતિ CMD_throw:
		    લીટી = compile_throw(p, cctx);
		    cctx->ctx_had_throw = TRUE;
		    તોડો;

	    સ્થિતિ CMD_eval:
		    લીટી = compile_eval(p, cctx);
		    તોડો;

	    સ્થિતિ CMD_defer:
		    લીટી = compile_defer(p, cctx);
		    તોડો;

#ifdef HAS_MESSAGE_WINDOW
	    સ્થિતિ CMD_echowindow:
		    {
			long cmd_ગણતરી = get_cmd_ગણતરી(લીટી, &ea);
			જો (cmd_ગણતરી < 0)
			    લીટી = ખાલી;
			નહીંતર
			    લીટી = compile_mult_પદાવલિ(p, ea.cmdidx,
							     cmd_ગણતરી, cctx);
		    }
		    તોડો;
#endif
	    સ્થિતિ CMD_echo:
	    સ્થિતિ CMD_echon:
	    સ્થિતિ CMD_echoconsole:
	    સ્થિતિ CMD_echoerr:
	    સ્થિતિ CMD_echomsg:
	    સ્થિતિ CMD_execute:
		    લીટી = compile_mult_પદાવલિ(p, ea.cmdidx, 0, cctx);
		    તોડો;

	    સ્થિતિ CMD_put:
		    ea.cmd = cmd;
		    લીટી = compile_put(p, &ea, cctx, FALSE);
		    તોડો;

	    સ્થિતિ CMD_iput:
		    ea.cmd = cmd;
		    લીટી = compile_put(p, &ea, cctx, TRUE);
		    તોડો;

	    સ્થિતિ CMD_substitute:
		    જો (check_global_and_subst(ea.cmd, p) == FAIL)
			પાછા FAIL;
		    જો (cctx->ctx_skip == SKIP_YES)
			લીટી = (char_u *)"";
		    નહીંતર
		    {
			ea.arg = p;
			લીટી = compile_substitute(લીટી, &ea, cctx);
		    }
		    તોડો;

	    સ્થિતિ CMD_redir:
		    ea.arg = p;
		    લીટી = compile_redir(લીટી, &ea, cctx);
		    તોડો;

	    સ્થિતિ CMD_cexpr:
	    સ્થિતિ CMD_lexpr:
	    સ્થિતિ CMD_caddexpr:
	    સ્થિતિ CMD_laddexpr:
	    સ્થિતિ CMD_cgetexpr:
	    સ્થિતિ CMD_lgetexpr:
#ifdef FEAT_QUICKFIX
		    ea.arg = p;
		    લીટી = compile_cexpr(લીટી, &ea, cctx);
#નહીંતર
		    ex_ni(&ea);
		    લીટી = ખાલી;
#endif
		    તોડો;

	    સ્થિતિ CMD_append:
	    સ્થિતિ CMD_change:
	    સ્થિતિ CMD_ઉમેરો:
	    સ્થિતિ CMD_k:
	    સ્થિતિ CMD_open:
	    સ્થિતિ CMD_t:
	    સ્થિતિ CMD_xit:
		    not_in_vim9(&ea, false);
		    પાછા FAIL;

	    સ્થિતિ CMD_SIZE:
		    જો (cctx->ctx_skip != SKIP_YES)
		    {
			semsg(_(e_invalid_command_str), ea.cmd);
			પાછા FAIL;
		    }
		    // We don't check for a next command here.
		    લીટી = (char_u *)"";
		    તોડો;

	    સ્થિતિ CMD_lua:
	    સ્થિતિ CMD_mzscheme:
	    સ્થિતિ CMD_perl:
	    સ્થિતિ CMD_py3:
	    સ્થિતિ CMD_python3:
	    સ્થિતિ CMD_python:
	    સ્થિતિ CMD_pythonx:
	    સ્થિતિ CMD_ruby:
	    સ્થિતિ CMD_tcl:
		    ea.arg = p;
		    જો (vim_strchr(લીટી, '\n') == ખાલી)
			લીટી = compile_exec(લીટી, &ea, cctx);
		    નહીંતર
			// heredoc lines have been concatenated with NL
			// characters in get_function_body()
			લીટી = compile_script(લીટી, cctx);
		    તોડો;

	    સ્થિતિ CMD_vim9script:
		    જો (cctx->ctx_skip != SKIP_YES)
		    {
			emsg(_(e_vim9script_can_only_be_used_in_script));
			પાછા FAIL;
		    }
		    લીટી = (char_u *)"";
		    તોડો;

	    સ્થિતિ CMD_class:
		    emsg(_(e_class_can_only_be_used_in_script));
		    પાછા FAIL;

	    સ્થિતિ CMD_enum:
		    emsg(_(e_enum_can_only_be_used_in_script));
		    પાછા FAIL;

	    સ્થિતિ CMD_interface:
		    emsg(_(e_interface_can_only_be_used_in_script));
		    પાછા FAIL;

	    સ્થિતિ CMD_type:
		    emsg(_(e_type_can_only_be_used_in_script));
		    પાછા FAIL;

	    સ્થિતિ CMD_global:
		    જો (check_global_and_subst(ea.cmd, p) == FAIL)
			પાછા FAIL;
		    // FALLTHROUGH
	    મૂળભૂત:
		    // Not recognized, execute with do_cmdline_cmd().
		    ea.arg = p;
		    લીટી = compile_exec(લીટી, &ea, cctx);
		    તોડો;
	}
nextline:
	જો (લીટી == ખાલી)
	    પાછા FAIL;
	લીટી = skipwhite(લીટી);

	// Undo any command modifiers.
	generate_undo_cmdmods(cctx);

	જો (cctx->ctx_type_ઢગલો.ga_len < 0)
	{
	    iemsg("Type stack underflow");
	    પાછા FAIL;
	}
    } // END of the loop over all the function body lines.

    પાછા OK;
}

/*
 * Returns TRUE if the end of a scope (if, while, for, block) is missing.
 * Called after compiling a def function body.
 */
    સ્થિર પૂર્ણાંક
compile_dfunc_scope_end_missing(cctx_T *cctx)
{
    જો (cctx->ctx_scope == ખાલી)
	પાછા FALSE;

    પસંદગી (cctx->ctx_scope->se_type)
    {
	સ્થિતિ IF_SCOPE:
	    emsg(_(e_missing_endif));
	    તોડો;
	સ્થિતિ WHILE_SCOPE:
	    emsg(_(e_missing_endwhile));
	    તોડો;
	સ્થિતિ FOR_SCOPE:
	    emsg(_(e_missing_endfor));
	    તોડો;
	સ્થિતિ TRY_SCOPE:
	    emsg(_(e_missing_endtry));
	    તોડો;
	સ્થિતિ BLOCK_SCOPE:
	    // end block scope from :try (maybe)
	    compile_endblock(cctx);
	    જો (cctx->ctx_scope != ખાલી
		    && cctx->ctx_scope->se_type == TRY_SCOPE)
		emsg(_(e_missing_endtry));
	    નહીંતર
		emsg(_(e_missing_rcurly));
	    તોડો;
	મૂળભૂત:
	    emsg(_(e_missing_rcurly));
    }
    પાછા TRUE;
}

/*
 * When compiling a def function, if it doesn't have an explicit return
 * statement, then generate a default return instruction.  For an object
 * constructor, return the object.
 */
    સ્થિર પૂર્ણાંક
compile_dfunc_generate_default_return(ufunc_T *ufunc, cctx_T *cctx)
{
    // TODO: if a function ends in "throw" but there was a return elsewhere we
    // should not assume the return type is "void".
    જો (cctx->ctx_had_return || cctx->ctx_had_throw)
	પાછા OK;

    જો (ufunc->uf_ret_type->tt_type == VAR_UNKNOWN)
	ufunc->uf_ret_type = &t_void;
    નહીંતર જો (ufunc->uf_ret_type->tt_type != VAR_VOID
					&& !IS_CONSTRUCTOR_METHOD(ufunc))
    {
	emsg(_(e_missing_return_statement));
	પાછા FAIL;
    }

    // Return void if there is no return at the end.
    // For a constructor return the object.
    જો (IS_CONSTRUCTOR_METHOD(ufunc))
    {
	generate_instr(cctx, ISN_RETURN_OBJECT);
	ufunc->uf_ret_type = &ufunc->uf_class->class_object_type;
    }
    નહીંતર
	generate_instr(cctx, ISN_RETURN_VOID);

    પાછા OK;
}

/*
 * Perform the chores after successfully compiling a def function.
 */
    સ્થિર શૂન્ય
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
    જો (cctx->ctx_compile_type == CT_PROFILE)
    {
	dfunc->df_instr_prof = instr->ga_data;
	dfunc->df_instr_prof_ગણતરી = instr->ga_len;
    }
    નહીંતર
#endif
	જો (cctx->ctx_compile_type == CT_DEBUG)
	{
	    dfunc->df_instr_debug = instr->ga_data;
	    dfunc->df_instr_debug_ગણતરી = instr->ga_len;
	}
	નહીંતર
	{
	    dfunc->df_instr = instr->ga_data;
	    dfunc->df_instr_ગણતરી = instr->ga_len;
	}
    dfunc->df_varcount = dfunc->df_var_names.ga_len;
    dfunc->df_has_closure = cctx->ctx_has_closure;

    જો (cctx->ctx_outer_used)
    {
	ufunc->uf_flags |= FC_CLOSURE;
	જો (outer_cctx != ખાલી)
	    ++outer_cctx->ctx_closure_ગણતરી;
    }

    ufunc->uf_def_status = UF_COMPILED;
}

/*
 * Perform the cleanup when a def function compilation fails.
 */
    સ્થિર શૂન્ય
compile_dfunc_ufunc_cleanup(
    ufunc_T	*ufunc,
    garray_T	*instr,
    પૂર્ણાંક		new_def_function,
    અક્ષર	*errormsg,
    પૂર્ણાંક		did_emsg_before,
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
    જો (!dfunc->df_deleted && new_def_function
			&& ufunc->uf_dfunc_idx == def_functions.ga_len - 1)
    {
	--def_functions.ga_len;
	ufunc->uf_dfunc_idx = 0;
    }
    ufunc->uf_def_status = UF_COMPILE_ERROR;

    જ્યારે (cctx->ctx_scope != ખાલી)
	drop_scope(cctx);

    જો (errormsg != ખાલી)
	emsg(errormsg);
    નહીંતર જો (did_emsg == did_emsg_before)
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
    પૂર્ણાંક
compile_def_function(
	ufunc_T		*ufunc,
	પૂર્ણાંક		check_return_type,
	compiletype_T   compile_type,
	cctx_T		*outer_cctx)
{
    garray_T	લીટીઓ_to_free;
    અક્ષર	*errormsg = ખાલી;	// error message
    cctx_T	cctx;
    garray_T	*instr;
    પૂર્ણાંક		did_emsg_before = did_emsg;
    પૂર્ણાંક		did_emsg_silent_before = did_emsg_silent;
    પૂર્ણાંક		ret = FAIL;
    sctx_T	save_current_sctx = current_sctx;
    પૂર્ણાંક		save_estack_compiling = estack_compiling;
    પૂર્ણાંક		save_cmod_flags = cmdmod.cmod_flags;
    પૂર્ણાંક		do_estack_ધકેલો;
    પૂર્ણાંક		new_def_function = FALSE;

    // allocated lines are freed at the end
    ga_init2(&લીટીઓ_to_free, કદ(char_u *), 50);

    // Initialize the ufunc and the compilation context
    જો (compile_dfunc_ufunc_init(ufunc, outer_cctx, compile_type,
						&new_def_function) == FAIL)
	પાછા FAIL;

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
    do_estack_ધકેલો = !estack_ટોચ_is_ufunc(ufunc, 1);
    જો (do_estack_ધકેલો)
	estack_ધકેલો_ufunc(ufunc, 1);
    estack_compiling = TRUE;

    // Make sure arguments don't shadow variables in the context
    જો (check_args_shadowing(ufunc, &cctx) == FAIL)
	જાઓ erret;

    // For an object method and a constructor generate instructions to
    // initialize "this" and the object variables.
    જો (ufunc->uf_flags & (FC_OBJECT|FC_NEW))
	જો (obj_method_prologue(ufunc, &cctx) == FAIL)
	    જાઓ erret;

    જો (ufunc->uf_def_args.ga_len > 0)
	જો (compile_def_function_default_args(ufunc, instr, &cctx) == FAIL)
	    જાઓ erret;
    ufunc->uf_args_visible = ufunc->uf_args.ga_len;

    // Compiling an abstract method or a function in an interface is done to
    // get the function type.  No code is actually compiled.
    જો (ufunc->uf_class != ખાલી && (IS_INTERFACE(ufunc->uf_class)
						|| IS_ABSTRACT_METHOD(ufunc)))
    {
	ufunc->uf_def_status = UF_NOT_COMPILED;
	ret = OK;
	જાઓ erret;
    }

    // compile the function body
    જો (compile_def_function_body(ufunc->uf_લીટીઓ.ga_len, check_return_type,
				&લીટીઓ_to_free, &errormsg, &cctx) == FAIL)
	જાઓ erret;

    જો (compile_dfunc_scope_end_missing(&cctx))
	જાઓ erret;

    જો (compile_dfunc_generate_default_return(ufunc, &cctx) == FAIL)
	જાઓ erret;

    // When compiled with ":silent!" and there was an error don't consider the
    // function compiled.
    જો (emsg_silent == 0 || did_emsg_silent == did_emsg_silent_before)
	compile_dfunc_epilogue(outer_cctx, ufunc, instr, &cctx);

    ret = OK;

erret:
    જો (ufunc->uf_def_status == UF_COMPILING)
    {
	// compilation failed. do cleanup.
	compile_dfunc_ufunc_cleanup(ufunc, instr, new_def_function,
				    errormsg, did_emsg_before, &cctx);
    }

    જો (cctx.ctx_redir_lhs.lhs_name != ખાલી)
    {
	જો (ret == OK)
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
    જો (do_estack_ધકેલો)
	estack_કાઢો();

    ga_clear_strings(&લીટીઓ_to_free);
    free_locals(&cctx);
    ga_clear(&cctx.ctx_type_ઢગલો);
    પાછા ret;
}

    શૂન્ય
set_function_type(ufunc_T *ufunc)
{
    પૂર્ણાંક varargs = ufunc->uf_va_name != ખાલી;
    પૂર્ણાંક argcount = ufunc->uf_args.ga_len;

    // Create a type for the function, with the return type and any
    // argument types.
    // A vararg is included in uf_args.ga_len but not in uf_arg_types.
    // The type is included in "tt_args".
    જો (argcount > 0 || varargs)
    {
	જો (ufunc->uf_type_list.ga_itemsize == 0)
	    ga_init2(&ufunc->uf_type_list, કદ(type_T *), 10);
	ufunc->uf_func_type = alloc_func_type(ufunc->uf_ret_type,
					   argcount, &ufunc->uf_type_list);
	// Add argument types to the function type.
	જો (func_type_add_arg_types(ufunc->uf_func_type,
				    argcount + varargs,
				    &ufunc->uf_type_list) == FAIL)
	    પાછા;
	ufunc->uf_func_type->tt_argcount = argcount + varargs;
	ufunc->uf_func_type->tt_min_argcount =
				      argcount - ufunc->uf_def_args.ga_len;
	જો (ufunc->uf_arg_types == ખાલી)
	{
	    પૂર્ણાંક i;

	    // lambda does not have argument types.
	    માટે (i = 0; i < argcount; ++i)
		ufunc->uf_func_type->tt_args[i] = &t_any;
	}
	નહીંતર
	    mch_memmove(ufunc->uf_func_type->tt_args,
			 ufunc->uf_arg_types, કદ(type_T *) * argcount);
	જો (varargs)
	{
	    ufunc->uf_func_type->tt_args[argcount] =
		   ufunc->uf_va_type == ખાલી ? &t_list_any : ufunc->uf_va_type;
	    ufunc->uf_func_type->tt_flags = TTFLAG_VARARGS;
	}
    }
    નહીંતર
	// No arguments, can use a predefined type.
	ufunc->uf_func_type = get_func_type(ufunc->uf_ret_type,
					   argcount, &ufunc->uf_type_list);
}

/*
 * Free all instructions for "dfunc" except df_name.
 */
    સ્થિર શૂન્ય
delete_def_function_contents(dfunc_T *dfunc, પૂર્ણાંક mark_deleted)
{
    પૂર્ણાંક idx;

    // In same cases the instructions may refer to a class in which the
    // function is defined and unreferencing the class may call back here
    // recursively.  Set the df_delete_busy to avoid problems.
    જો (dfunc->df_delete_busy)
	પાછા;
    dfunc->df_delete_busy = TRUE;

    ga_clear(&dfunc->df_def_args_isn);
    ga_clear_strings(&dfunc->df_var_names);

    જો (dfunc->df_instr != ખાલી)
    {
	માટે (idx = 0; idx < dfunc->df_instr_ગણતરી; ++idx)
	    delete_instr(dfunc->df_instr + idx);
	VIM_CLEAR(dfunc->df_instr);
    }
    જો (dfunc->df_instr_debug != ખાલી)
    {
	માટે (idx = 0; idx < dfunc->df_instr_debug_ગણતરી; ++idx)
	    delete_instr(dfunc->df_instr_debug + idx);
	VIM_CLEAR(dfunc->df_instr_debug);
    }
#ifdef FEAT_PROFILE
    જો (dfunc->df_instr_prof != ખાલી)
    {
	માટે (idx = 0; idx < dfunc->df_instr_prof_ગણતરી; ++idx)
	    delete_instr(dfunc->df_instr_prof + idx);
	VIM_CLEAR(dfunc->df_instr_prof);
    }
#endif

    જો (mark_deleted)
	dfunc->df_deleted = TRUE;
    જો (dfunc->df_ufunc != ખાલી)
	dfunc->df_ufunc->uf_def_status = UF_NOT_COMPILED;

    dfunc->df_delete_busy = FALSE;
}

/*
 * When a user function is deleted, clear the contents of any associated def
 * function, unless another user function still uses it.
 * The position in def_functions can be re-used.
 */
    શૂન્ય
unlink_def_function(ufunc_T *ufunc)
{
    જો (ufunc->uf_dfunc_idx <= 0 || def_functions.ga_data == ખાલી)
	પાછા;

    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data)
	+ ufunc->uf_dfunc_idx;

    જો (--dfunc->df_refcount <= 0)
	delete_def_function_contents(dfunc, TRUE);
    ufunc->uf_def_status = UF_NOT_COMPILED;
    ufunc->uf_dfunc_idx = 0;
    જો (dfunc->df_ufunc == ufunc)
	dfunc->df_ufunc = ખાલી;
}

/*
 * Used when a user function refers to an existing dfunc.
 */
    શૂન્ય
link_def_function(ufunc_T *ufunc)
{
    જો (ufunc->uf_dfunc_idx <= 0)
	પાછા;

    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data)
	+ ufunc->uf_dfunc_idx;

    ++dfunc->df_refcount;
}

#જો defined(EXITFREE)
/*
 * Free all functions defined with ":def".
 */
    શૂન્ય
free_def_functions(શૂન્ય)
{
    પૂર્ણાંક idx;

    માટે (idx = 0; idx < def_functions.ga_len; ++idx)
    {
	dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data) + idx;

	delete_def_function_contents(dfunc, TRUE);
	vim_free(dfunc->df_name);
    }

    ga_clear(&def_functions);
}
#endif


#endif // FEAT_EVAL
