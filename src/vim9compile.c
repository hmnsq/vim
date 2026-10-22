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
#include "vim.h"

#если defined(FEAT_EVAL)

// Functions defined with :def are stored in this growarray.
// They are never removed, so that they can be found by index.
// Deleted functions have the df_deleted flag set.
garray_T def_functions = {0, 0, sizeof(dfunc_T), 50, NULL};

static void delete_def_function_contents(dfunc_T *dfunc, целое mark_deleted);

/*
 * Lookup variable "name" in the local scope and return it in "lvar".
 * "lvar->lv_from_outer" is incremented accordingly.
 * If "lvar" is NULL only check if the variable can be found.
 * Return FAIL if not found.
 */
    целое
lookup_local(char_u *name, size_t len, lvar_T *lvar, cctx_T *cctx)
{
    целое	    idx;
    lvar_T  *lvp;

    если (len == 0)
	вернуть FAIL;

    если (((len == 4 && STRNCMP(name, "this", 4) == 0)
		|| (len == 5 && STRNCMP(name, "super", 5) == 0))
	    && cctx->ctx_ufunc != NULL
	    && (cctx->ctx_ufunc->uf_flags & (FC_OBJECT|FC_NEW)))
    {
	целое is_super = *name == 's';
	если (is_super)
	{
	    если (name[5] != '.')
	    {
		emsg(_(e_super_must_be_followed_by_dot));
		вернуть FAIL;
	    }
	    если (cctx->ctx_ufunc->uf_class != NULL
		    && cctx->ctx_ufunc->uf_class->class_extends == NULL)
	    {
		emsg(_(e_using_super_not_in_child_class));
		вернуть FAIL;
	    }
	}
	если (lvar != NULL)
	{
	    CLEAR_POINTER(lvar);
	    lvar->lv_loop_depth = -1;
	    lvar->lv_name = (char_u *)(is_super ? "super" : "this");
	    если (cctx->ctx_ufunc->uf_class != NULL)
	    {
		lvar->lv_type = &cctx->ctx_ufunc->uf_class->class_object_type;
		если (is_super)
		{
		    type_T *type = get_type_ptr(cctx->ctx_type_list);

		    если (type != NULL)
		    {
			*type = *lvar->lv_type;
			lvar->lv_type = type;
			type->tt_flags |= TTFLAG_SUPER;
		    }
		}
	    }
	}
	вернуть OK;
    }

    // Find local in current function scope.
    для (idx = 0; idx < cctx->ctx_locals.ga_len; ++idx)
    {
	lvp = ((lvar_T *)cctx->ctx_locals.ga_data) + idx;
	если (lvp->lv_name != NULL
		&& STRNCMP(name, lvp->lv_name, len) == 0
					       && STRLEN(lvp->lv_name) == len)
	{
	    если (lvar != NULL)
	    {
		*lvar = *lvp;
		lvar->lv_from_outer = 0;
		// If the variable was declared inside a loop set
		// lvar->lv_loop_idx and lvar->lv_loop_depth.
		get_loop_var_idx(cctx, idx, lvar);
	    }
	    вернуть OK;
	}
    }

    // Find local in outer function scope.
    если (cctx->ctx_outer != NULL)
    {
	если (lookup_local(name, len, lvar, cctx->ctx_outer) == OK)
	{
	    если (lvar != NULL)
	    {
		cctx->ctx_outer_used = TRUE;
		++lvar->lv_from_outer;
	    }
	    вернуть OK;
	}
    }

    вернуть FAIL;
}

/*
 * Lookup an argument in the current function and an enclosing function.
 * Returns the argument index in "idxp"
 * Returns the argument type in "type"
 * Sets "gen_load_outer" to TRUE if found in outer scope.
 * Returns OK when found, FAIL otherwise.
 */
    целое
arg_exists(
	char_u	*name,
	size_t	len,
	целое	*idxp,
	type_T	**type,
	целое	*gen_load_outer,
	cctx_T	*cctx)
{
    целое	    idx;
    char_u  *va_name;

    если (len == 0)
	вернуть FAIL;
    для (idx = 0; idx < cctx->ctx_ufunc->uf_args_visible; ++idx)
    {
	char_u *arg = FUNCARG(cctx->ctx_ufunc, idx);

	если (STRNCMP(name, arg, len) == 0 && arg[len] == NUL)
	{
	    если (idxp != NULL)
	    {
		// Arguments are located above the frame pointer.  One further
		// if there is a vararg argument
		*idxp = idx - (cctx->ctx_ufunc->uf_args.ga_len
							    + STACK_FRAME_SIZE)
			      + (cctx->ctx_ufunc->uf_va_name != NULL ? -1 : 0);

		если (cctx->ctx_ufunc->uf_arg_types != NULL)
		    *type = cctx->ctx_ufunc->uf_arg_types[idx];
		иначе
		    *type = &t_any;
	    }
	    вернуть OK;
	}
    }

    va_name = cctx->ctx_ufunc->uf_va_name;
    если (va_name != NULL
		    && STRNCMP(name, va_name, len) == 0 && va_name[len] == NUL)
    {
	если (idxp != NULL)
	{
	    // varargs is always the last argument
	    *idxp = -STACK_FRAME_SIZE - 1;
	    *type = cctx->ctx_ufunc->uf_va_type;
	}
	вернуть OK;
    }

    если (cctx->ctx_outer != NULL)
    {
	// Lookup the name for an argument of the outer function.
	если (arg_exists(name, len, idxp, type, gen_load_outer, cctx->ctx_outer)
									 == OK)
	{
	    если (gen_load_outer != NULL)
	    {
		++*gen_load_outer;
		cctx->ctx_outer_used = TRUE;
	    }
	    вернуть OK;
	}
    }

    вернуть FAIL;
}

/*
 * Lookup a script-local variable in the current script, possibly defined in a
 * block that contains the function "cctx->ctx_ufunc".
 * "cctx" is NULL at the script level, "cstack" is NULL in a function.
 * If "len" is <= 0 "name" must be NUL terminated.
 * Return NULL when not found.
 */
    static sallvar_T *
find_script_var(char_u *name, size_t len, cctx_T *cctx, cstack_T *cstack)
{
    scriptitem_T    *si = SCRIPT_ITEM(current_sctx.sc_sid);
    hashitem_T	    *hi;
    целое		    cc;
    sallvar_T	    *sav;
    ufunc_T	    *ufunc;

    // Find the list of all script variables with the right name.
    если (len > 0)
    {
	cc = name[len];
	name[len] = NUL;
    }
    hi = hash_find(&si->sn_all_vars.dv_hashtab, name);
    если (len > 0)
	name[len] = cc;
    если (HASHITEM_EMPTY(hi))
	вернуть NULL;

    sav = HI2SAV(hi);
    если (sav->sav_block_id == 0)
	// variable defined in the top script scope is always visible
	вернуть sav;

    если (cctx == NULL)
    {
	если (cstack == NULL)
	    вернуть NULL;

	// Not in a function scope, find variable with block ID equal to or
	// smaller than the current block id.  Use "cstack" to go up the block
	// scopes.
	пока (sav != NULL)
	{
	    целое idx;

	    для (idx = cstack->cs_idx; idx >= 0; --idx)
		если (cstack->cs_block_id[idx] == sav->sav_block_id)
		    break;
	    если (idx >= 0)
		break;
	    sav = sav->sav_next;
	}
	вернуть sav;
    }

    // Go over the variables with this name and find one that was visible
    // from the function.
    ufunc = cctx->ctx_ufunc;
    пока (sav != NULL)
    {
	целое idx;

	если (ufunc->uf_block_depth == 0 && sav->sav_block_id == 0)
	    // If the function was defined at the script level (not inside a
	    // block), script-scope variables are always visible.
	    вернуть sav;

	// Go over the blocks that this function was defined in.  If the
	// variable block ID matches it was visible to the function.
	для (idx = 0; idx < ufunc->uf_block_depth; ++idx)
	    если (ufunc->uf_block_ids[idx] == sav->sav_block_id)
		вернуть sav;
	sav = sav->sav_next;
    }

    // Not found, variable was not visible.
    вернуть NULL;
}

/*
 * If "name" can be found in the current script set it's "block_id".
 */
    void
update_script_var_block_id(char_u *name, целое block_id)
{
    scriptitem_T    *si = SCRIPT_ITEM(current_sctx.sc_sid);
    hashitem_T	    *hi;
    sallvar_T	    *sav;

    hi = hash_find(&si->sn_all_vars.dv_hashtab, name);
    если (HASHITEM_EMPTY(hi))
	вернуть;
    sav = HI2SAV(hi);
    sav->sav_block_id = block_id;
}

/*
 * Lookup a variable (without s: prefix) in the current script.
 * "cctx" is NULL at the script level, "cstack" is NULL in a function.
 * Returns OK or FAIL.
 */
    целое
script_var_exists(char_u *name, size_t len, cctx_T *cctx, cstack_T *cstack)
{
    если (current_sctx.sc_sid <= 0)
	вернуть FAIL;
    если (current_script_is_vim9())
    {
	// Check script variables that were visible where the function was
	// defined.
	если (find_script_var(name, len, cctx, cstack) != NULL)
	    вернуть OK;
    }
    иначе
    {
	hashtab_T	*ht = &SCRIPT_VARS(current_sctx.sc_sid);
	dictitem_T	*di;
	целое		cc;

	// Check script variables that are currently visible
	cc = name[len];
	name[len] = NUL;
	di = find_var_in_ht(ht, 0, name, TRUE);
	name[len] = cc;
	если (di != NULL)
	    вернуть OK;
    }

    вернуть FAIL;
}

/*
 * Returns the index of a class method or class variable with name "name"
 * accessible in the currently compiled function.
 * If "cl_ret" is not NULL set it to the class.
 * Otherwise return -1.
 */
    static целое
cctx_class_midx(
    cctx_T  *cctx,
    целое	    is_method,
    char_u  *name,
    size_t  len,
    class_T **cl_ret)
{
    если (cctx == NULL || cctx->ctx_ufunc == NULL
	    || cctx->ctx_ufunc->uf_class == NULL
	    || cctx->ctx_ufunc->uf_defclass == NULL)
	вернуть -1;

    // Search for the class method or variable in the class where the calling
    // function is defined.
    class_T *cl = cctx->ctx_ufunc->uf_defclass;
    целое m_idx = is_method ? class_method_idx(cl, name, len)
					: class_member_idx(cl, name, len);
    если (m_idx < 0)
    {
	cl = cl->class_extends;
	пока (cl != NULL)
	{
	    m_idx = is_method ? class_method_idx(cl, name, len)
					: class_member_idx(cl, name, len);
	    если (m_idx >= 0)
		break;
	    cl = cl->class_extends;
	}
    }

    если (m_idx >= 0)
    {
	если (cl_ret != NULL)
	    *cl_ret = cl;
    }

    вернуть m_idx;
}

/*
 * Returns the index of a class method with name "name" accessible in the
 * currently compiled function.  Returns -1 if not found.  The class where the
 * method is defined is returned in "cl_ret".
 */
    целое
cctx_class_method_idx(
    cctx_T  *cctx,
    char_u  *name,
    size_t  len,
    class_T **cl_ret)
{
    вернуть cctx_class_midx(cctx, TRUE, name, len, cl_ret);
}

/*
 * Returns the index of a class variable with name "name" accessible in the
 * currently compiled function.  Returns -1 if not found.  The class where the
 * variable is defined is returned in "cl_ret".
 */
    целое
cctx_class_member_idx(
    cctx_T  *cctx,
    char_u  *name,
    size_t  len,
    class_T **cl_ret)
{
    вернуть cctx_class_midx(cctx, FALSE, name, len, cl_ret);
}

/*
 * Return TRUE if "name" is a local variable, argument, script variable or
 * imported.  Also if "name" is "this" and in a class method.
 */
    static целое
variable_exists(char_u *name, size_t len, cctx_T *cctx)
{
    вернуть (cctx != NULL
		&& (lookup_local(name, len, NULL, cctx) == OK
		    || arg_exists(name, len, NULL, NULL, NULL, cctx) == OK
		    || (len == 4
			&& cctx->ctx_ufunc != NULL
			&& (cctx->ctx_ufunc->uf_flags & (FC_OBJECT|FC_NEW))
			&& STRNCMP(name, "this", 4) == 0)))
	    || script_var_exists(name, len, cctx, NULL) == OK
	    || cctx_class_member_idx(cctx, name, len, NULL) >= 0
	    || find_imported(name, len, FALSE) != NULL;
}

/*
 * Return TRUE if "name" is a local variable, argument, script variable,
 * imported or function.  Or commands are being skipped, a declaration may have
 * been skipped then.
 */
    static целое
item_exists(char_u *name, size_t len, целое cmd UNUSED, cctx_T *cctx)
{
    вернуть variable_exists(name, len, cctx);
}

/*
 * Check if "p[len]" is already defined, either in script "import_sid" or in
 * compilation context "cctx".
 * "cctx" is NULL at the script level, "cstack" is NULL in a function.
 * Does not check the global namespace.
 * If "is_arg" is TRUE the error message is for an argument name.
 * Return FAIL and give an error if it defined.
 */
    целое
check_defined(
	char_u	    *p,
	size_t	    len,
	cctx_T	    *cctx,
	cstack_T    *cstack,
	целое	    is_arg)
{
    целое		c = p[len];
    ufunc_T	*ufunc = NULL;

    // underscore argument is OK
    если (len == 1 && *p == '_')
	вернуть OK;

    если (script_var_exists(p, len, cctx, cstack) == OK)
    {
	если (is_arg)
	    semsg(_(e_argument_already_declared_in_script_str), p);
	иначе
	    semsg(_(e_variable_already_declared_in_script_str), p);
	вернуть FAIL;
    }

    если (cctx_class_member_idx(cctx, p, len, NULL) >= 0)
    {
	если (is_arg)
	    semsg(_(e_argument_already_declared_in_class_str), p);
	иначе
	    semsg(_(e_variable_already_declared_in_class_str), p);
	вернуть FAIL;
    }

    p[len] = NUL;
    если ((cctx != NULL
		&& (lookup_local(p, len, NULL, cctx) == OK
		    || arg_exists(p, len, NULL, NULL, NULL, cctx) == OK))
	    || find_imported(p, len, FALSE) != NULL
	    || (ufunc = find_func_even_dead(p, 0)) != NULL)
    {
	// A local or script-local function can shadow a global function.
	если (ufunc == NULL || ((ufunc->uf_flags & FC_DEAD) == 0
		    && (!func_is_global(ufunc)
					     || (p[0] == 'g' && p[1] == ':'))))
	{
	    если (is_arg)
		semsg(_(e_argument_name_shadows_existing_variable_str), p);
	    иначе
		semsg(_(e_name_already_defined_str), p);
	    p[len] = c;
	    вернуть FAIL;
	}
    }
    p[len] = c;
    вернуть OK;
}


/*
 * Return TRUE if "actual" could be "expected" and a runtime typecheck is to be
 * used.  Return FALSE if the types will never match.
 */
    static целое
use_typecheck(type_T *actual, type_T *expected)
{
    если (actual->tt_type == VAR_ANY
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
	вернуть TRUE;
    если (actual->tt_type == VAR_OBJECT && expected->tt_type == VAR_OBJECT)
	вернуть TRUE;
    если ((actual->tt_type == VAR_LIST
		|| actual->tt_type == VAR_TUPLE
		|| actual->tt_type == VAR_DICT)
	    && actual->tt_type == expected->tt_type)
	// This takes care of a nested list or dict.
	вернуть use_typecheck(actual->tt_member, expected->tt_member);
    вернуть FALSE;
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
    целое
need_type_where(
	type_T	*actual,
	type_T	*expected,
	целое	typechk_flags,	// acceptable types (type check flags)
	целое	offset,
	where_T	where,
	cctx_T	*cctx,
	целое	silent,
	целое	actual_is_const)
{
    целое ret;

    если (expected->tt_type != VAR_CLASS && expected->tt_type != VAR_TYPEALIAS)
    {
	если (check_type_is_value(actual) == FAIL)
	    вернуть FAIL;
    }

    если (expected == &t_bool && actual != &t_bool
					&& (actual->tt_flags & TTFLAG_BOOL_OK))
    {
	// Using "0", "1" or the result of an expression with "&&" or "||" as a
	// boolean is OK but requires a conversion.
	generate_2BOOL(cctx, FALSE, offset);
	вернуть OK;
    }

    ret = check_type_maybe(expected, actual, FALSE, where);
    если (ret == OK)
	вернуть OK;

    // If actual a constant a runtime check makes no sense.  If it's
    // null_function it is OK.
    если (actual_is_const && ret == MAYBE && actual == &t_func_unknown)
	вернуть OK;

    // If the actual type can be the expected type add a runtime check.
    если (!actual_is_const && ret == MAYBE && use_typecheck(actual, expected))
    {
	generate_TYPECHECK(cctx, expected, typechk_flags, offset,
		where.wt_kind == WT_VARIABLE, where.wt_index);
	вернуть OK;
    }

    если (!silent)
	type_mismatch_where(expected, actual, where);
    вернуть FAIL;
}

    целое
need_type(
	type_T	*actual,
	type_T	*expected,
	целое	typechk_flags,	// acceptable types (type check flags)
	целое	offset,
	целое	arg_idx,
	cctx_T	*cctx,
	целое	silent,
	целое	actual_is_const)
{
    where_T where = WHERE_INIT;

    если (arg_idx > 0)
    {
	where.wt_index = arg_idx;
	where.wt_kind = WT_ARGUMENT;
    }
    вернуть need_type_where(actual, expected, typechk_flags, offset, where,
						cctx, silent, actual_is_const);
}

/*
 * Set type of variable "lvar" to "type".  If the variable is a constant then
 * the type gets TTFLAG_CONST.
 */
    static void
set_var_type(lvar_T *lvar, type_T *type_arg, cctx_T *cctx)
{
    type_T	*type = type_arg;

    если (lvar->lv_const == ASSIGN_CONST && (type->tt_flags & TTFLAG_CONST) == 0)
    {
	если (type->tt_flags & TTFLAG_STATIC)
	    // entry in static_types[] is followed by const type
	    type = type + 1;
	иначе
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
	целое	assign,
	type_T	*type)
{
    lvar_T  *lvar;
    dfunc_T *dfunc;

    если (arg_exists(name, len, NULL, NULL, NULL, cctx) == OK)
    {
	emsg_namelen(_(e_str_is_used_as_argument), name, (целое)len);
	вернуть NULL;
    }

    если (GA_GROW_FAILS(&cctx->ctx_locals, 1))
	вернуть NULL;
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
    если (type == &t_unknown || type == &t_any)
	// type not known yet, may be inferred from RHS
	lvar->lv_type = type;
    иначе
	// may use TTFLAG_CONST
	set_var_type(lvar, type, cctx);

    // Remember the name for debugging.
    если (GA_GROW_FAILS(&dfunc->df_var_names, 1))
	вернуть NULL;
    ((char_u **)dfunc->df_var_names.ga_data)[lvar->lv_idx] =
						    vim_strsave(lvar->lv_name);
    ++dfunc->df_var_names.ga_len;

    вернуть lvar;
}

/*
 * If "check_writable" is ASSIGN_CONST give an error if the variable was
 * defined with :final or :const, if "check_writable" is ASSIGN_FINAL give an
 * error if the variable was defined with :const.
 */
    static целое
check_item_writable(svar_T *sv, целое check_writable, char_u *name)
{
    если ((check_writable == ASSIGN_CONST && sv->sv_const != 0)
	    || (check_writable == ASSIGN_FINAL
					      && sv->sv_const == ASSIGN_CONST))
    {
	semsg(_(e_cannot_change_readonly_variable_str), name);
	вернуть FAIL;
    }
    вернуть OK;
}

/*
 * Find "name" in script-local items of script "sid".
 * Pass "check_writable" to check_item_writable().
 * "cctx" is NULL at the script level, "cstack" is NULL in a function.
 * Returns the index in "sn_var_vals" if found.
 * If found but not in "sn_var_vals" returns -1.
 * If not found or the variable is not writable returns -2.
 */
    целое
get_script_item_idx(
	целое	    sid,
	char_u	    *name,
	целое	    check_writable,
	cctx_T	    *cctx,
	cstack_T    *cstack)
{
    hashtab_T	    *ht;
    dictitem_T	    *di;
    scriptitem_T    *si = SCRIPT_ITEM(sid);
    svar_T	    *sv;
    целое		    idx;

    если (!SCRIPT_ID_VALID(sid))
	вернуть -1;
    если (sid == current_sctx.sc_sid)
    {
	sallvar_T *sav = find_script_var(name, 0, cctx, cstack);

	если (sav == NULL)
	    вернуть -2;
	idx = sav->sav_var_vals_idx;
	sv = ((svar_T *)si->sn_var_vals.ga_data) + idx;
	если (check_item_writable(sv, check_writable, name) == FAIL)
	    вернуть -2;
	вернуть idx;
    }

    // First look the name up in the hashtable.
    ht = &SCRIPT_VARS(sid);
    di = find_var_in_ht(ht, 0, name, TRUE);
    если (di == NULL)
    {
	если (si->sn_autoload_prefix != NULL)
	{
	    hashitem_T *hi;

	    // A variable exported from an autoload script is in the global
	    // variables, we can find it in the all_vars table.
	    hi = hash_find(&si->sn_all_vars.dv_hashtab, name);
	    если (!HASHITEM_EMPTY(hi))
		вернуть HI2SAV(hi)->sav_var_vals_idx;
	}
	вернуть -2;
    }

    // Now find the svar_T index in sn_var_vals.
    для (idx = 0; idx < si->sn_var_vals.ga_len; ++idx)
    {
	sv = ((svar_T *)si->sn_var_vals.ga_data) + idx;
	если (sv->sv_tv == &di->di_tv)
	{
	    если (check_item_writable(sv, check_writable, name) == FAIL)
		вернуть -2;
	    вернуть idx;
	}
    }
    вернуть -1;
}

    static imported_T *
find_imported_in_script(char_u *name, size_t len, целое sid)
{
    scriptitem_T    *si;
    целое		    idx;

    если (!SCRIPT_ID_VALID(sid))
	вернуть NULL;
    si = SCRIPT_ITEM(sid);
    для (idx = 0; idx < si->sn_imports.ga_len; ++idx)
    {
	imported_T *import = ((imported_T *)si->sn_imports.ga_data) + idx;

	если (len == 0 ? STRCMP(name, import->imp_name) == 0
		     : STRLEN(import->imp_name) == len
				  && STRNCMP(name, import->imp_name, len) == 0)
	    вернуть import;
    }
    вернуть NULL;
}

/*
 * Find "name" in imported items of the current script.
 * If "len" is 0 use any length that works.
 * If "load" is TRUE and the script was not loaded yet, load it now.
 */
    imported_T *
find_imported(char_u *name, size_t len, целое load)
{
    если (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	вернуть NULL;

    // Skip over "s:" before "s:something" to find the import name.
    целое off = name[0] == 's' && name[1] == ':' ? 2 : 0;

    imported_T *ret = find_imported_in_script(name + off, len - off,
							  current_sctx.sc_sid);
    если (ret != NULL && load && (ret->imp_flags & IMP_FLAGS_AUTOLOAD))
    {
	scid_T	actual_sid = 0;
	целое	save_emsg_off = emsg_off;

	// "emsg_off" will be set when evaluating an expression silently, but
	// we do want to know about errors in a script.  Also because it then
	// aborts when an error is encountered.
	emsg_off = FALSE;

	// script found before but not loaded yet
	ret->imp_flags &= ~IMP_FLAGS_AUTOLOAD;
	(void)do_source(SCRIPT_ITEM(ret->imp_sid)->sn_name, FALSE,
						       DOSO_NONE, &actual_sid);
	// If the script is a symlink it may be sourced with another name, may
	// need to adjust the script ID for that.
	если (actual_sid != 0)
	    ret->imp_sid = actual_sid;

	emsg_off = save_emsg_off;
    }
    вернуть ret;
}

/*
 * Called when checking for a following operator at "arg".  When the rest of
 * the line is empty or only a comment, peek the next line.  If there is a next
 * line return a pointer to it and set "nextp".
 * Otherwise skip over white space.
 */
    char_u *
may_peek_next_line(cctx_T *cctx, char_u *arg, char_u **nextp)
{
    char_u *p = skipwhite(arg);

    *nextp = NULL;
    если (*p == NUL || (VIM_ISWHITE(*arg) && vim9_comment_start(p)))
    {
	*nextp = peek_next_line_from_context(cctx);
	если (*nextp != NULL)
	    вернуть *nextp;
    }
    вернуть p;
}

/*
 * Return a pointer to the next line that isn't empty or only contains a
 * comment. Skips over white space.
 * Returns NULL if there is none.
 */
    char_u *
peek_next_line_from_context(cctx_T *cctx)
{
    целое lnum = cctx->ctx_lnum;

    пока (++lnum < cctx->ctx_ufunc->uf_lines.ga_len)
    {
	char_u *line = ((char_u **)cctx->ctx_ufunc->uf_lines.ga_data)[lnum];
	char_u *p;

	// ignore NULLs inserted for continuation lines
	если (line != NULL)
	{
	    p = skipwhite(line);
	    если (vim9_bad_comment(p))
		вернуть NULL;
	    если (*p != NUL && !vim9_comment_start(p))
		вернуть p;
	}
    }
    вернуть NULL;
}

/*
 * Get the next line of the function from "cctx".
 * Skips over empty lines.  Skips over comment lines if "skip_comment" is TRUE.
 * Returns NULL when at the end.
 */
    char_u *
next_line_from_context(cctx_T *cctx, целое skip_comment)
{
    char_u	*line;

    do
    {
	++cctx->ctx_lnum;
	если (cctx->ctx_lnum >= cctx->ctx_ufunc->uf_lines.ga_len)
	{
	    line = NULL;
	    break;
	}
	line = ((char_u **)cctx->ctx_ufunc->uf_lines.ga_data)[cctx->ctx_lnum];
	cctx->ctx_line_start = line;
	SOURCING_LNUM = cctx->ctx_lnum + 1;
    } пока (line == NULL || *skipwhite(line) == NUL
		     || (skip_comment && vim9_comment_start(skipwhite(line))));
    вернуть line;
}

/*
 * Skip over white space at "whitep" and assign to "*arg".
 * If "*arg" is at the end of the line, advance to the next line.
 * Also when "whitep" points to white space and "*arg" is on a "#".
 * Return FAIL if beyond the last line, "*arg" is unmodified then.
 */
    целое
may_get_next_line(char_u *whitep, char_u **arg, cctx_T *cctx)
{
    *arg = skipwhite(whitep);
    если (vim9_bad_comment(*arg))
	вернуть FAIL;
    если (**arg == NUL || (VIM_ISWHITE(*whitep) && vim9_comment_start(*arg)))
    {
	char_u *next = next_line_from_context(cctx, TRUE);

	если (next == NULL)
	    вернуть FAIL;
	*arg = skipwhite(next);
    }
    вернуть OK;
}

/*
 * Idem, and give an error when failed.
 */
    целое
may_get_next_line_error(char_u *whitep, char_u **arg, cctx_T *cctx)
{
    если (may_get_next_line(whitep, arg, cctx) == FAIL)
    {
	SOURCING_LNUM = cctx->ctx_lnum + 1;
	emsg(_(e_line_incomplete));
	вернуть FAIL;
    }
    вернуть OK;
}

/*
 * Get a line from the compilation context, compatible with exarg_T getline().
 * Return a pointer to the line in allocated memory.
 * Return NULL for end-of-file or some error.
 */
    static char_u *
exarg_getline(
	целое c UNUSED,
	void *cookie,
	целое indent UNUSED,
	getline_opt_T options UNUSED)
{
    cctx_T  *cctx = (cctx_T *)cookie;
    char_u  *p;

    для (;;)
    {
	если (cctx->ctx_lnum >= cctx->ctx_ufunc->uf_lines.ga_len - 1)
	    вернуть NULL;
	++cctx->ctx_lnum;
	p = ((char_u **)cctx->ctx_ufunc->uf_lines.ga_data)[cctx->ctx_lnum];
	// Comment lines result in NULL pointers, skip them.
	если (p != NULL)
	    вернуть vim_strsave(p);
    }
}

    void
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
    целое
func_needs_compiling(ufunc_T *ufunc, compiletype_T compile_type)
{
    switch (ufunc->uf_def_status)
    {
	case UF_TO_BE_COMPILED:
	    вернуть TRUE;

	case UF_COMPILED:
	{
	    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data)
							 + ufunc->uf_dfunc_idx;

	    switch (compile_type)
	    {
		case CT_PROFILE:
#ifdef FEAT_PROFILE
		    вернуть dfunc->df_instr_prof == NULL;
#endif
		case CT_NONE:
		    вернуть dfunc->df_instr == NULL;
		case CT_DEBUG:
		    вернуть dfunc->df_instr_debug == NULL;
	    }
	}

	case UF_NOT_COMPILED:
	case UF_COMPILE_ERROR:
	case UF_COMPILING:
	    break;
    }
    вернуть FALSE;
}

/*
 * Compile a nested :def command.
 */
    static char_u *
compile_nested_function(exarg_T *eap, cctx_T *cctx, garray_T *lines_to_free)
{
    целое		is_global = *eap->arg == 'g' && eap->arg[1] == ':';
    char_u	*name_start = eap->arg;
    char_u	*name_end = to_name_end(eap->arg, TRUE);
    целое		off;
    char_u	*func_name;
    string_T	lambda_name;
    ufunc_T	*ufunc;
    целое		r = FAIL;
    compiletype_T   compile_type;
    целое		funcref_isn_idx = -1;
    lvar_T	*lvar = NULL;
    char_u	*bracket_start = NULL;

    если (eap->forceit)
    {
	semsg(_(e_cannot_use_bang_with_nested_def_str),
		eap->cmdidx == CMD_def ? ":def" : ":function");
	вернуть NULL;
    }

    если (*name_start == '/')
    {
	name_end = skip_regexp(name_start + 1, '/', TRUE);
	если (*name_end == '/')
	    ++name_end;
	set_nextcmd(eap, name_end);
    }

    если (*name_end == '<')
    {
	bracket_start = name_end;
	если (skip_generic_func_type_args(&name_end) == FAIL)
	    вернуть NULL;
    }

    если (name_end == name_start || *skipwhite(name_end) != '(')
    {
	если (!ends_excmd2(name_start, name_end))
	{
	    если (*skipwhite(name_end) == '.')
		semsg(_(e_cannot_define_dict_func_in_vim9_script_str),
								     eap->cmd);
	    иначе
		semsg(_(e_invalid_command_str), eap->cmd);
	    вернуть NULL;
	}

	// "def" or "def Name": list functions
	если (generate_DEF(cctx, name_start, name_end - name_start) == FAIL)
	    вернуть NULL;
	вернуть eap->nextcmd == NULL ? (char_u *)"" : eap->nextcmd;
    }

    если (bracket_start != NULL)
	// generic function.  The function name ends before the list of types
	// (opening angle bracket).
	name_end = bracket_start;

    // Only g:Func() can use a namespace.
    если (name_start[1] == ':' && !is_global)
    {
	semsg(_(e_namespace_not_supported_str), name_start);
	вернуть NULL;
    }
    если (cctx->ctx_skip != SKIP_YES
	    && check_defined(name_start, name_end - name_start, cctx,
							  NULL, FALSE) == FAIL)
	вернуть NULL;
    если (!ASCII_ISUPPER(is_global ? name_start[2] : name_start[0]))
    {
	semsg(_(e_function_name_must_start_with_capital_str), name_start);
	вернуть NULL;
    }

    eap->arg = name_end;
    fill_exarg_from_cctx(eap, cctx);

    eap->forceit = FALSE;
    // We use the special <lambda>99 name, but it's not really a lambda.
    lambda_name = get_lambda_name();
    lambda_name.string = vim_strnsave(lambda_name.string, lambda_name.length);
    если (lambda_name.string == NULL)
	вернуть NULL;

    // This may free the current line, make a copy of the name.
    off = is_global ? 2 : 0;
    func_name = vim_strnsave(name_start + off, name_end - name_start - off);
    если (func_name == NULL)
    {
	r = FAIL;
	goto theend;
    }

    // A type variable cannot have the name of the function.  The function is
    // defined with the "<lambda>" name, check it here.
    если (bracket_start != NULL)
    {
	size_t	len = STRLEN(func_name);

	для (char_u *t = bracket_start + 1; *t != '>' && *t != NUL; )
	{
	    char_u *te = to_name_end(t, FALSE);

	    если ((size_t)(te - t) == len && STRNCMP(t, func_name, len) == 0)
	    {
		semsg(_(e_redefining_script_item_str), func_name);
		r = FAIL;
		goto theend;
	    }
	    t = te == t ? t + 1 : te;
	}
    }

    // Make sure "KeyTyped" is not set, it may cause indent to be written.
    целое save_KeyTyped = KeyTyped;
    KeyTyped = FALSE;

    ufunc = define_function(eap, lambda_name.string, lines_to_free, 0, NULL, 0,
									cctx);

    KeyTyped = save_KeyTyped;

    если (ufunc == NULL)
    {
	r = eap->skip ? OK : FAIL;
	goto theend;
    }
    если (eap->nextcmd != NULL)
    {
	semsg(_(e_text_found_after_str_str),
	      eap->cmdidx == CMD_def ? "enddef" : "endfunction", eap->nextcmd);
	r = FAIL;
	func_ptr_unref(ufunc);
	goto theend;
    }

    // copy over the block scope IDs before compiling
    если (!is_global && cctx->ctx_ufunc->uf_block_depth > 0)
    {
	целое block_depth = cctx->ctx_ufunc->uf_block_depth;

	ufunc->uf_block_ids = ALLOC_MULT(целое, block_depth);
	если (ufunc->uf_block_ids != NULL)
	{
	    mch_memmove(ufunc->uf_block_ids, cctx->ctx_ufunc->uf_block_ids,
						    sizeof(целое) * block_depth);
	    ufunc->uf_block_depth = block_depth;
	}
    }

    // Define the funcref before compiling, so that it is found by any
    // recursive call.
    если (is_global)
    {
	r = generate_NEWFUNC(cctx, lambda_name.string, func_name);
	func_name = NULL;
	lambda_name.string = NULL;
	lambda_name.length = 0;
    }
    иначе
    {
	// Define a local variable for the function reference.
	lvar = reserve_local(cctx, func_name, name_end - name_start,
					    ASSIGN_CONST, ufunc->uf_func_type);
	если (lvar == NULL)
	{
	    func_ptr_unref(ufunc);
	    goto theend;
	}
	lvar->lv_ufunc = ufunc;
	если (generate_FUNCREF(cctx, ufunc, NULL, FALSE, 0, &funcref_isn_idx) == FAIL)
	{
	    func_ptr_unref(ufunc);
	    goto theend;
	}
	r = generate_STORE(cctx, ISN_STORE, lvar->lv_idx, NULL);
    }

    compile_type = get_compile_type(ufunc);
#ifdef FEAT_PROFILE
    // If the outer function is profiled, also compile the nested function for
    // profiling.
    если (cctx->ctx_compile_type == CT_PROFILE)
	compile_type = CT_PROFILE;
#endif
    если (func_needs_compiling(ufunc, compile_type)
	    && compile_def_function(ufunc, TRUE, compile_type, cctx) == FAIL)
    {
	func_ptr_unref(ufunc);
	если (lvar != NULL)
	    // Now the local variable can't be used.
	    *lvar->lv_name = '/';  // impossible value
	goto theend;
    }

#ifdef FEAT_PROFILE
    // When the outer function is compiled for profiling, the nested function
    // may be called without profiling.  Compile it here in the right context.
    если (compile_type == CT_PROFILE && func_needs_compiling(ufunc, CT_NONE))
	compile_def_function(ufunc, FALSE, CT_NONE, cctx);
#endif

    // If a FUNCREF instruction was generated, set the index after compiling.
    если (funcref_isn_idx != -1 && ufunc->uf_def_status == UF_COMPILED)
    {
	isn_T	*funcref_isn = ((isn_T *)cctx->ctx_instr.ga_data) +
							funcref_isn_idx;
	funcref_isn->isn_arg.funcref.fr_dfunc_idx = ufunc->uf_dfunc_idx;
    }

theend:
    vim_free(lambda_name.string);
    vim_free(func_name);
    вернуть r == FAIL ? NULL : (char_u *)"";
}

/*
 * Compile one Vim expression {expr} in string "p".
 * "p" points to the opening "{".
 * Return a pointer to the character after "}", NULL for an error.
 */
    char_u *
compile_one_expr_in_str(char_u *p, cctx_T *cctx)
{
    char_u	*block_start;
    char_u	*block_end;

    // Skip the opening {.
    block_start = skipwhite(p + 1);
    block_end = block_start;
    если (*block_start != NUL && skip_expr(&block_end, NULL) == FAIL)
	вернуть NULL;
    block_end = skipwhite(block_end);
    // The block must be closed by a }.
    если (*block_end != '}')
    {
	semsg(_(e_missing_close_curly_str), p);
	вернуть NULL;
    }
    если (compile_expr0(&block_start, cctx) == FAIL)
	вернуть NULL;
    may_generate_2STRING(-1, TOSTRING_INTERPOLATE, cctx);

    вернуть block_end + 1;
}

/*
 * Compile a string "str" (either containing a literal string or a mix of
 * literal strings and Vim expressions of the form `{expr}`).  This is used
 * when compiling a heredoc assignment to a variable or an interpolated string
 * in a Vim9 def function.  Vim9 instructions are generated to push strings,
 * evaluate expressions, concatenate them and create a list of lines.  When
 * "evalstr" is TRUE, Vim expressions in "str" are evaluated.
 */
    целое
compile_all_expr_in_str(char_u *str, целое evalstr, cctx_T *cctx)
{
    char_u	*p = str;
    char_u	*val;
    целое		count = 0;

    если (cctx->ctx_skip == SKIP_YES)
	вернуть OK;

    если (!evalstr || *str == NUL)
    {
	// Literal string, possibly empty.
	val = *str != NUL ? vim_strsave(str) : NULL;
	вернуть generate_PUSHS(cctx, &val);
    }

    // Push all the string pieces to the stack, followed by a ISN_CONCAT.
    пока (*p != NUL)
    {
	char_u	*lit_start;
	целое	escaped_brace = FALSE;

	// Look for a block start.
	lit_start = p;
	пока (*p != '{' && *p != '}' && *p != NUL)
	    ++p;

	если (*p != NUL && *p == p[1])
	{
	    // Escaped brace, unescape and continue.
	    // Include the brace in the literal string.
	    ++p;
	    escaped_brace = TRUE;
	}
	иначе если (*p == '}')
	{
	    semsg(_(e_stray_closing_curly_str), str);
	    вернуть FAIL;
	}

	// Append the literal part.
	если (p != lit_start)
	{
	    val = vim_strnsave(lit_start, (size_t)(p - lit_start));
	    если (generate_PUSHS(cctx, &val) == FAIL)
		вернуть FAIL;
	    ++count;
	}

	если (*p == NUL)
	    break;

	если (escaped_brace)
	{
	    // Skip the second brace.
	    ++p;
	    continue;
	}

	p = compile_one_expr_in_str(p, cctx);
	если (p == NULL)
	    вернуть FAIL;
	++count;
    }

    // Small optimization, if there's only a single piece skip the ISN_CONCAT.
    если (count > 1)
	вернуть generate_CONCAT(cctx, count);

    вернуть OK;
}

/*
 * Return the length of an assignment operator, or zero if there isn't one.
 */
    целое
assignment_len(char_u *p, целое *heredoc)
{
    если (*p == '=')
    {
	если (p[1] == '<' && p[2] == '<')
	{
	    *heredoc = TRUE;
	    вернуть 3;
	}
	вернуть 1;
    }
    если (vim_strchr((char_u *)"+-*/%", *p) != NULL && p[1] == '=')
	вернуть 2;
    если (STRNCMP(p, "..=", 3) == 0)
	вернуть 3;
    вернуть 0;
}

/*
 * Generate the load instruction for "name".
 */
    static целое
generate_loadvar(cctx_T *cctx, lhs_T *lhs)
{
    char_u	*name = lhs->lhs_name;
    type_T	*type = lhs->lhs_type;
    целое		res = OK;

    switch (lhs->lhs_dest)
    {
	case dest_option:
	case dest_func_option:
	    generate_LOAD(cctx, ISN_LOADOPT, 0, name, type);
	    break;
	case dest_global:
	    если (vim_strchr(name, AUTOLOAD_CHAR) == NULL)
	    {
		если (name[2] == NUL)
		    generate_instr_type(cctx, ISN_LOADGDICT, &t_dict_any);
		иначе
		    generate_LOAD(cctx, ISN_LOADG, 0, name + 2, type);
	    }
	    иначе
		generate_LOAD(cctx, ISN_LOADAUTO, 0, name, type);
	    break;
	case dest_buffer:
	    generate_LOAD(cctx, ISN_LOADB, 0, name + 2, type);
	    break;
	case dest_window:
	    generate_LOAD(cctx, ISN_LOADW, 0, name + 2, type);
	    break;
	case dest_tab:
	    generate_LOAD(cctx, ISN_LOADT, 0, name + 2, type);
	    break;
	case dest_script:
	case dest_script_v9:
	    res = compile_load_scriptvar(cctx,
			    name + (name[1] == ':' ? 2 : 0), NULL, NULL, NULL);
	    break;
	case dest_env:
	    // Include $ in the name here
	    generate_LOAD(cctx, ISN_LOADENV, 0, name, type);
	    break;
	case dest_reg:
	    generate_LOAD(cctx, ISN_LOADREG, name[1], NULL, &t_string);
	    break;
	case dest_vimvar:
	    generate_LOADV(cctx, name + 2);
	    break;
	case dest_local:
	    если (cctx->ctx_skip != SKIP_YES)
	    {
		lvar_T	*lvar = lhs->lhs_lvar;
		если (lvar->lv_from_outer > 0)
		    generate_LOADOUTER(cctx, lvar->lv_idx, lvar->lv_from_outer,
				 lvar->lv_loop_depth, lvar->lv_loop_idx, type);
		иначе
		    generate_LOAD(cctx, ISN_LOAD, lvar->lv_idx, NULL, type);
	    }
	    break;
	case dest_class_member:
	    generate_CLASSMEMBER(cctx, TRUE, lhs->lhs_class,
						     lhs->lhs_classmember_idx);
	    break;
	case dest_expr:
	    // list or dict value should already be on the stack.
	    break;
    }

    вернуть res;
}

/*
 * Skip over "[expr]" or ".member".
 * Does not check for any errors.
 */
    static char_u *
skip_index(char_u *start)
{
    char_u *p = start;

    если (*p == '[')
    {
	p = skipwhite(p + 1);
	(void)skip_expr(&p, NULL);
	p = skipwhite(p);
	если (*p == ']')
	    вернуть p + 1;
	вернуть p;
    }
    // if (*p == '.')
    вернуть to_name_end(p + 1, TRUE);
}

    void
vim9_declare_error(char_u *name)
{
    char *scope = "";

    switch (*name)
    {
	case 'g': scope = _("global"); break;
	case 'b': scope = _("buffer"); break;
	case 'w': scope = _("window"); break;
	case 't': scope = _("tab"); break;
	case 'v': scope = "v:"; break;
	case '$': semsg(_(e_cannot_declare_an_environment_variable_str), name);
		  вернуть;
	case '&': semsg(_(e_cannot_declare_an_option_str), name);
		  вернуть;
	case '@': semsg(_(e_cannot_declare_a_register_str), name);
		  вернуть;
	default: вернуть;
    }
    semsg(_(e_cannot_declare_a_scope_variable_str), scope, name);
}

/*
 * Return TRUE if "name" is a valid register to use.
 * Return FALSE and give an error message if not.
 */
    static целое
valid_dest_reg(целое name)
{
    если (name == '@')
       name = '"';
    если (name == '/' || name == '=' || name == '#' || valid_yank_reg(name, TRUE))
	вернуть TRUE;
    emsg_invreg(name);
    вернуть FAIL;
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
    целое
get_var_dest(
	char_u		*name,
	assign_dest_T	*dest,
	cmdidx_T	cmdidx,
	целое		has_index,
	целое		*option_scope,
	целое		*vimvaridx,
	type_T		**type,
	cctx_T		*cctx)
{
    char_u *p;

    если (*name == '&')
    {
	целое		cc;
	long		numval;
	getoption_T	opt_type;
	целое		opt_p_flags;

	*dest = dest_option;
	если (cmdidx == CMD_final || cmdidx == CMD_const)
	{
	    emsg(_(e_cannot_lock_option));
	    вернуть FAIL;
	}
	p = name;
	p = find_option_end(&p, option_scope);
	если (p == NULL)
	{
	    // cannot happen?
	    emsg(_(e_unexpected_characters_in_assignment));
	    вернуть FAIL;
	}
	cc = *p;
	*p = NUL;
	opt_type = get_option_value(skip_option_env_lead(name),
				   &numval, NULL, &opt_p_flags, *option_scope);
	*p = cc;
	switch (opt_type)
	{
	    case gov_unknown:
		    semsg(_(e_unknown_option_str), name);
		    вернуть FAIL;
	    case gov_string:
	    case gov_hidden_string:
		    если (opt_p_flags & P_FUNC)
		    {
			// might be a Funcref, check the type later
			*type = &t_any;
			*dest = dest_func_option;
		    }
		    иначе
		    {
			*type = &t_string;
		    }
		    break;
	    case gov_bool:
	    case gov_hidden_bool:
		    *type = &t_bool;
		    break;
	    case gov_number:
	    case gov_hidden_number:
		    *type = &t_number;
		    break;
	}
    }
    иначе если (*name == '$')
    {
	*dest = dest_env;
	*type = &t_string;
    }
    иначе если (*name == '@')
    {
	если (!valid_dest_reg(name[1]))
	    вернуть FAIL;
	*dest = dest_reg;
	*type = name[1] == '#' ? &t_number_or_string : &t_string;
    }
    иначе если (STRNCMP(name, "g:", 2) == 0)
    {
	*dest = dest_global;
    }
    иначе если (STRNCMP(name, "b:", 2) == 0)
    {
	*dest = dest_buffer;
    }
    иначе если (STRNCMP(name, "w:", 2) == 0)
    {
	*dest = dest_window;
    }
    иначе если (STRNCMP(name, "t:", 2) == 0)
    {
	*dest = dest_tab;
    }
    иначе если (STRNCMP(name, "v:", 2) == 0)
    {
	typval_T	*vtv;
	целое		di_flags;

	*vimvaridx = find_vim_var(name + 2, &di_flags);
	если (*vimvaridx < 0)
	{
	    semsg(_(e_variable_not_found_str), name);
	    вернуть FAIL;
	}
	// An item of a read-only variable can be changed, as when not compiled.
	// We use the current value of "sandbox" here, is that OK?
	если (!has_index && var_check_ro(di_flags, name, FALSE))
	    вернуть FAIL;
	*dest = dest_vimvar;
	vtv = get_vim_var_tv(*vimvaridx);
	*type = typval2type_vimvar(vtv, cctx->ctx_type_list);
    }
    вернуть OK;
}

    static целое
is_decl_command(cmdidx_T cmdidx)
{
    вернуть cmdidx == CMD_let || cmdidx == CMD_var
				 || cmdidx == CMD_final || cmdidx == CMD_const;
}

/*
 * Returns TRUE if the class or object variable in "lhs" is modifiable.
 * "var_start" points to the start of the variable name and "lhs->lhs_varlen"
 * has the total length.  Note that the "lhs" can be nested an object reference
 * (e.g.  a.b.c.d.var).
 */
    static целое
lhs_class_member_modifiable(lhs_T *lhs, char_u	*var_start, cctx_T *cctx)
{
    size_t	varlen = lhs->lhs_varlen;
    class_T	*cl = lhs->lhs_type->tt_class;
    целое		is_object = lhs->lhs_type->tt_type == VAR_OBJECT;
    char_u	*name = var_start + varlen + 1;
    size_t	namelen = lhs->lhs_end - var_start - varlen - 1;
    ocmember_T	*m;

    m = member_lookup(cl, lhs->lhs_type->tt_type, name, namelen, NULL);
    если (m == NULL)
    {
	member_not_found_msg(cl, lhs->lhs_type->tt_type, name, namelen);
	вернуть FALSE;
    }

    если (IS_ENUM(cl))
    {
	semsg(_(e_enumvalue_str_cannot_be_modified), cl->class_name.string,
		m->ocm_name.string);
	вернуть FALSE;
    }

    // If it is private member variable, then accessing it outside the
    // class is not allowed.
    // If it is a read only class variable, then it can be modified
    // only inside the class where it is defined.
    если ((m->ocm_access != VIM_ACCESS_ALL) &&
	    ((is_object && !inside_class(cctx, cl))
	     || (!is_object && cctx->ctx_ufunc->uf_class != cl)))
    {
	char *msg = (m->ocm_access == VIM_ACCESS_PRIVATE)
				? e_cannot_access_protected_variable_str
				: e_variable_is_not_writable_str;
	emsg_var_cl_define(msg, m->ocm_name.string, 0, cl);
	вернуть FALSE;
    }

    вернуть TRUE;
}

/*
 * Initialize "lhs" with default values
 */
    static void
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
    static целое
lhs_find_var_end(
    lhs_T	*lhs,
    char_u	*var_start,
    целое		is_decl,
    char_u	**var_endp)
{
    char_u  *var_end = *var_endp;

    // "lhs_dest_end" is the end of the destination, including "[expr]" or
    // ".name".
    // "var_end" is the end of the variable/option/etc. name.
    lhs->lhs_dest_end = skip_var_one(var_start, FALSE);
    если (*var_start == '@')
    {
	если (!valid_dest_reg(var_start[1]))
	    вернуть FAIL;
	var_end = var_start + 2;
    }
    иначе
    {
	// skip over the leading "&", "&l:", "&g:" and "$"
	var_end = skip_option_env_lead(var_start);
	var_end = to_name_end(var_end, TRUE);
    }

    // "a: type" is declaring variable "a" with a type, not dict "a:".
    если (is_decl && lhs->lhs_dest_end == var_start + 2
					&& lhs->lhs_dest_end[-1] == ':')
	--lhs->lhs_dest_end;
    если (is_decl && var_end == var_start + 2 && var_end[-1] == ':')
	--var_end;

    lhs->lhs_end = lhs->lhs_dest_end;
    *var_endp = var_end;

    вернуть OK;
}

/*
 * Set various fields in "lhs"
 */
    static целое
lhs_init(
    lhs_T	*lhs,
    char_u	*var_start,
    целое		is_decl,
    целое		heredoc,
    char_u	**var_endp)
{
    char_u *var_end = *var_endp;

    lhs_init_defaults(lhs);

    // Find the end of the variable and the destination
    если (lhs_find_var_end(lhs, var_start, is_decl, &var_end) == FAIL)
	вернуть FAIL;

    // compute the length of the destination without "[expr]" or ".name"
    lhs->lhs_varlen = var_end - var_start;
    lhs->lhs_varlen_total = lhs->lhs_varlen;
    lhs->lhs_name = vim_strnsave(var_start, lhs->lhs_varlen);
    если (lhs->lhs_name == NULL)
	вернуть FAIL;

    если (lhs->lhs_dest_end > var_start + lhs->lhs_varlen)
	// Something follows after the variable: "var[idx]" or "var.key".
	lhs->lhs_has_index = TRUE;

    lhs->lhs_type = heredoc ? &t_list_string : &t_any;

    *var_endp = var_end;

    вернуть OK;
}

/*
 * Compile a LHS class variable name.
 */
    static целое
compile_lhs_class_variable(
    cctx_T	*cctx,
    lhs_T	*lhs,
    class_T	*defcl,
    целое		is_decl)
{
    если (cctx->ctx_ufunc->uf_defclass != defcl)
    {
	// A class variable can be accessed without the class name
	// only inside a class.
	semsg(_(e_class_variable_str_accessible_only_inside_class_str),
		lhs->lhs_name, defcl->class_name.string);
	вернуть FAIL;
    }

    если (is_decl)
    {
	semsg(_(e_variable_already_declared_in_class_str), lhs->lhs_name);
	вернуть FAIL;
    }

    ocmember_T	*m = &defcl->class_class_members[lhs->lhs_classmember_idx];
    если (oc_var_check_ro(defcl, m))
	вернуть FAIL;

    lhs->lhs_dest = dest_class_member;
    // The class variable is defined either in the current class or
    // in one of the parent class in the hierarchy.
    lhs->lhs_class = defcl;
    lhs->lhs_type = oc_member_type_by_idx(defcl, FALSE,
						lhs->lhs_classmember_idx);

    вернуть OK;
}

/*
 * Compile an imported LHS variable
 */
    static целое
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
    если (dot == NULL)
    {
	semsg(_(e_no_dot_after_imported_name_str), var_start);
	вернуть FAIL;
    }

    p = skipwhite(dot + 1);
    var_end = to_name_end(p, TRUE);
    если (var_end == p)
    {
	semsg(_(e_missing_name_after_imported_name_str), var_start);
	вернуть FAIL;
    }

    vim_free(lhs->lhs_name);
    lhs->lhs_varlen = var_end - p;
    lhs->lhs_name = vim_strnsave(p, lhs->lhs_varlen);
    если (lhs->lhs_name == NULL)
	вернуть FAIL;
    *rawnamep = lhs->lhs_name;
    lhs->lhs_scriptvar_sid = import->imp_sid;

    // TODO: where do we check this name is exported?

    // Check if something follows: "exp.var[idx]" or
    // "exp.var.key".
    lhs->lhs_has_index = lhs->lhs_dest_end > skipwhite(var_end);

    *var_endp = var_end;

    вернуть OK;
}

/*
 * Process a script-local variable when compiling a LHS variable name.
 */
    static целое
compile_lhs_script_var(
    cctx_T	*cctx,
    lhs_T	*lhs,
    char_u	*var_start,
    char_u	*var_end,
    целое		is_decl)
{
    целое		script_namespace = FALSE;
    целое		script_var = FALSE;
    imported_T	*import;
    char_u	*var_name;
    size_t	var_name_len;

    если (lhs->lhs_varlen > 1 && STRNCMP(var_start, "s:", 2) == 0)
	script_namespace = TRUE;

    если (script_namespace)
    {
	var_name = var_start + 2;
	var_name_len = lhs->lhs_varlen - 2;
    }
    иначе
    {
	var_name = var_start;
	var_name_len = lhs->lhs_varlen;
    }

    если (script_var_exists(var_name, var_name_len, cctx, NULL) == OK)
	script_var = TRUE;

    import = find_imported(var_start, lhs->lhs_varlen, FALSE);

    если (script_namespace || script_var || import != NULL)
    {
	char_u *rawname = lhs->lhs_name + (lhs->lhs_name[1] == ':' ? 2 : 0);

	если (script_namespace && current_script_is_vim9())
	{
	    semsg(_(e_cannot_use_s_colon_in_vim9_script_str), var_start);
	    вернуть FAIL;
	}

	если (is_decl)
	{
	    если (script_namespace)
		semsg(_(e_cannot_declare_script_variable_in_function_str),
			lhs->lhs_name);
	    иначе
		semsg(_(e_variable_already_declared_in_script_str),
			lhs->lhs_name);
	    вернуть FAIL;
	}
	иначе если (cctx->ctx_ufunc->uf_script_ctx_version == SCRIPT_VERSION_VIM9
		&& script_namespace
		&& !script_var && import == NULL)
	{
	    semsg(_(e_unknown_variable_str), lhs->lhs_name);
	    вернуть FAIL;
	}

	lhs->lhs_dest = current_script_is_vim9() ? dest_script_v9 :
								dest_script;

	// existing script-local variables should have a type
	lhs->lhs_scriptvar_sid = current_sctx.sc_sid;
	если (import != NULL)
	{
	    если (compile_lhs_import_var(lhs, import, var_start, &var_end,
							&rawname) == FAIL)
		вернуть FAIL;
	}

	если (SCRIPT_ID_VALID(lhs->lhs_scriptvar_sid))
	{
	    // Check writable only when no index follows.
	    lhs->lhs_scriptvar_idx = get_script_item_idx(
					lhs->lhs_scriptvar_sid, rawname,
					lhs->lhs_has_index ?  ASSIGN_FINAL :
					ASSIGN_CONST, cctx, NULL);
	    если (lhs->lhs_scriptvar_idx >= 0)
	    {
		scriptitem_T *si = SCRIPT_ITEM(lhs->lhs_scriptvar_sid);
		svar_T	 *sv = ((svar_T *)si->sn_var_vals.ga_data)
						+ lhs->lhs_scriptvar_idx;

		lhs->lhs_type = sv->sv_type;
	    }
	}

	вернуть OK;
    }

    вернуть check_defined(var_start, lhs->lhs_varlen, cctx, NULL, FALSE);
}

/*
 * Compile the LHS destination.
 */
    static целое
compile_lhs_var_dest(
    cctx_T	*cctx,
    lhs_T	*lhs,
    целое		cmdidx,
    char_u	*var_start,
    char_u	*var_end,
    целое		is_decl,
    целое		has_cmd)	// "var" before "var_start"
{
    целое	    declare_error = FALSE;

    если (get_var_dest(lhs->lhs_name, &lhs->lhs_dest, cmdidx,
				lhs->lhs_has_index,
				&lhs->lhs_opt_flags, &lhs->lhs_vimvaridx,
				&lhs->lhs_type, cctx) == FAIL)
	вернуть FAIL;

    если (lhs->lhs_dest != dest_local && cmdidx != CMD_const
						&& cmdidx != CMD_final)
    {
	// Specific kind of variable recognized.
	declare_error = is_decl;
    }
    иначе
    {
	class_T	*defcl;

	// No specific kind of variable recognized, just a name.
	если (check_reserved_name(lhs->lhs_name, lhs->lhs_has_index
						&& *var_end == '.') == FAIL)
	    вернуть FAIL;

	если (lookup_local(var_start, lhs->lhs_varlen, &lhs->lhs_local_lvar,
								cctx) == OK)
	{
	    lhs->lhs_lvar = &lhs->lhs_local_lvar;
	}
	иначе
	{
	    CLEAR_FIELD(lhs->lhs_arg_lvar);
	    если (arg_exists(var_start, lhs->lhs_varlen,
			&lhs->lhs_arg_lvar.lv_idx, &lhs->lhs_arg_lvar.lv_type,
			&lhs->lhs_arg_lvar.lv_from_outer, cctx) == OK)
	    {
		если (is_decl)
		{
		    semsg(_(e_str_is_used_as_argument), lhs->lhs_name);
		    вернуть FAIL;
		}
		lhs->lhs_lvar = &lhs->lhs_arg_lvar;
	    }
	}

	если (lhs->lhs_lvar != NULL)
	{
	    если (is_decl)
	    {
		// if we come here with what looks like an assignment like
		// .= but which has been rejected by assignment_len() from
		// may_compile_assignment give a better error message
		char_u *p = skipwhite(lhs->lhs_end);
		если (p[0] == '.' && p[1] == '=')
		    emsg(_(e_dot_equal_not_supported_with_script_version_two));
		иначе если (p[0] == ':' && !has_cmd)
		    // type specified in an assignment without "var"
		    semsg(_(e_trailing_characters_str), p);
		иначе
		    semsg(_(e_variable_already_declared_str), lhs->lhs_name);
		вернуть FAIL;
	    }
	}
	иначе если ((lhs->lhs_classmember_idx = cctx_class_member_idx(
			cctx, var_start, lhs->lhs_varlen, &defcl)) >= 0)
	{
	    если (compile_lhs_class_variable(cctx, lhs, defcl, is_decl)
		    == FAIL)
		вернуть FAIL;
	}
	иначе
	{
	    если (compile_lhs_script_var(cctx, lhs, var_start, var_end,
			is_decl) == FAIL)
		вернуть FAIL;
	}
    }

    если (declare_error)
    {
	vim9_declare_error(lhs->lhs_name);
	вернуть FAIL;
    }

    вернуть OK;
}

/*
 * When compiling a LHS variable name, for a class or an object, set the LHS
 * member type.
 */
    static целое
compile_lhs_set_oc_member_type(
    cctx_T	*cctx,
    lhs_T	*lhs,
    char_u	*var_start)
{
    class_T	*cl = lhs->lhs_type->tt_class;
    целое		is_object = lhs->lhs_type->tt_type == VAR_OBJECT;
    char_u	*name = var_start + lhs->lhs_varlen + 1;
    size_t	namelen = lhs->lhs_end - var_start - lhs->lhs_varlen - 1;

    ocmember_T	*m = member_lookup(cl, lhs->lhs_type->tt_type,
	    name, namelen, &lhs->lhs_member_idx);
    если (m == NULL)
    {
	member_not_found_msg(cl, lhs->lhs_type->tt_type, name, namelen);
	вернуть FAIL;
    }

    если (IS_ENUM(cl))
    {
	если (!inside_class(cctx, cl))
	{
	    semsg(_(e_enumvalue_str_cannot_be_modified),
		    cl->class_name.string, m->ocm_name.string);
	    вернуть FAIL;
	}
	если (lhs->lhs_type->tt_type == VAR_OBJECT &&
		lhs->lhs_member_idx < 2)
	{
	    char *msg = lhs->lhs_member_idx == 0 ?
		e_enum_str_name_cannot_be_modified :
		e_enum_str_ordinal_cannot_be_modified;
	    semsg(_(msg), cl->class_name.string);
	    вернуть FAIL;
	}
    }

    // If it is private member variable, then accessing it outside the
    // class is not allowed.
    // If it is a read only class variable, then it can be modified
    // only inside the class where it is defined.
    если ((m->ocm_access != VIM_ACCESS_ALL) &&
	    ((is_object && !inside_class(cctx, cl))
	     || (!is_object && cctx->ctx_ufunc->uf_defclass != cl)))
    {
	char *msg = (m->ocm_access == VIM_ACCESS_PRIVATE)
	    ? e_cannot_access_protected_variable_str
	    : e_variable_is_not_writable_str;
	emsg_var_cl_define(msg, m->ocm_name.string, 0, cl);
	вернуть FAIL;
    }

    если (!IS_CONSTRUCTOR_METHOD(cctx->ctx_ufunc)
	    && oc_var_check_ro(cl, m))
	вернуть FAIL;

    lhs->lhs_member_type = m->ocm_type;

    вернуть OK;
}

/*
 * When compiling a LHS variable, set the LHS variable type.
 */
    static целое
compile_lhs_set_type(cctx_T *cctx, lhs_T *lhs, char_u *var_end, целое is_decl)
{
    если (is_decl && *skipwhite(var_end) == ':')
    {
	char_u *p;

	// parse optional type: "let var: type = expr"
	если (VIM_ISWHITE(*var_end))
	{
	    semsg(_(e_no_white_space_allowed_before_colon_str), var_end);
	    вернуть FAIL;
	}

	если (!VIM_ISWHITE(var_end[1]))
	{
	    semsg(_(e_white_space_required_after_str_str), ":", var_end);
	    вернуть FAIL;
	}

	p = skipwhite(var_end + 1);
	lhs->lhs_type = parse_type(&p, cctx->ctx_type_list, cctx->ctx_ufunc, cctx, TRUE);
	если (lhs->lhs_type == NULL
		|| !valid_declaration_type(lhs->lhs_type))
	    вернуть FAIL;

	lhs->lhs_has_type = TRUE;
	lhs->lhs_end = p;
    }
    иначе если (lhs->lhs_lvar != NULL)
	lhs->lhs_type = lhs->lhs_lvar->lv_type;

    вернуть OK;
}

/*
 * Returns TRUE if "lhs" is a concatenable string.
 */
    static целое
lhs_concatenable(lhs_T *lhs)
{
    вернуть lhs->lhs_dest == dest_global
		|| lhs->lhs_has_index
		|| lhs->lhs_type->tt_type == VAR_STRING
		|| lhs->lhs_type->tt_type == VAR_ANY;
}

/*
 * Create a new local variable when compiling a LHS variable.
 */
    static целое
compile_lhs_new_local_var(
    cctx_T	*cctx,
    lhs_T	*lhs,
    char_u	*var_start,
    целое		cmdidx,
    целое		oplen,
    целое		is_decl,
    целое		has_cmd,
    целое		heredoc)
{
    если (oplen > 1 && !heredoc)
    {
	// +=, /=, etc. require an existing variable
	semsg(_(e_cannot_use_operator_on_new_variable_str), lhs->lhs_name);
	вернуть FAIL;
    }

    если (!is_decl || (lhs->lhs_has_index && !has_cmd
					&& cctx->ctx_skip != SKIP_YES))
    {
	semsg(_(e_unknown_variable_str), lhs->lhs_name);
	вернуть FAIL;
    }

    // Check the name is valid for a funcref.
    если (lhs->lhs_type->tt_type == VAR_FUNC
				|| lhs->lhs_type->tt_type == VAR_PARTIAL)
    {
	если (var_wrong_func_name(lhs->lhs_name, TRUE))
	    вернуть FAIL;
    }

    // New local variable.
    целое assign;
    switch (cmdidx)
    {
	case CMD_final:
	    assign = ASSIGN_FINAL; break;
	case CMD_const:
	    assign = ASSIGN_CONST; break;
	default:
	    assign = ASSIGN_VAR; break;
    }

    lhs->lhs_lvar = reserve_local(cctx, var_start, lhs->lhs_varlen, assign,
							lhs->lhs_type);
    если (lhs->lhs_lvar == NULL)
	вернуть FAIL;

    lhs->lhs_new_local = TRUE;

    вернуть OK;
}

/*
 * When compiling a LHS variable name, set the LHS member type.
 */
    static целое
compile_lhs_set_member_type(
    cctx_T	*cctx,
    lhs_T	*lhs,
    char_u	*var_start,
    целое		is_decl,
    целое		has_cmd)
{
    lhs->lhs_member_type = lhs->lhs_type;

    если (!lhs->lhs_has_index)
	вернуть OK;

    char_u	*after = var_start + lhs->lhs_varlen;
    char_u	*p;

    // Something follows after the variable: "var[idx]" or "var.key".
    если (is_decl && cctx->ctx_skip != SKIP_YES)
    {
	если (has_cmd)
	    emsg(_(e_cannot_use_index_when_declaring_variable));
	иначе
	    semsg(_(e_unknown_variable_str), lhs->lhs_name);
	вернуть FAIL;
    }

    // Now: var_start[lhs->lhs_varlen] is '[' or '.'
    // Only the last index is used below, if there are others
    // before it generate code for the expression.  Thus for
    // "ll[1][2]" the expression is "ll[1]" and "[2]" is the index.
    для (;;)
    {
	p = skip_index(after);
	если (*p != '[' && *p != '.')
	{
	    lhs->lhs_varlen_total = p - var_start;
	    break;
	}
	after = p;
    }
    если (after > var_start + lhs->lhs_varlen)
    {
	lhs->lhs_varlen = after - var_start;
	lhs->lhs_dest = dest_expr;
	// We don't know the type before evaluating the expression,
	// use "any" until then.  The member index is for the first name,
	// not for the last index.
	lhs->lhs_type = &t_any;
	lhs->lhs_member_idx = -1;
    }

    целое use_class = lhs->lhs_type != NULL
	&& (lhs->lhs_type->tt_type == VAR_CLASS
		|| lhs->lhs_type->tt_type == VAR_OBJECT);

    если (lhs->lhs_type == NULL
	    || (use_class ? lhs->lhs_type->tt_class == NULL
		: lhs->lhs_type->tt_member == NULL))
    {
	lhs->lhs_member_type = &t_any;
    }
    иначе если (use_class)
    {
	// for an object or class member get the type of the member
	если (compile_lhs_set_oc_member_type(cctx, lhs, var_start) == FAIL)
	    вернуть FAIL;
    }
    иначе
	lhs->lhs_member_type = lhs->lhs_type->tt_member;

    вернуть OK;
}

/*
 * Figure out the LHS type and other properties for an assignment or one item
 * of ":unlet" with an index.
 * Returns OK or FAIL.
 */
    целое
compile_lhs(
	char_u	    *var_start,
	lhs_T	    *lhs,
	cmdidx_T    cmdidx,
	целое	    heredoc,
	целое	    has_cmd,	    // "var" before "var_start"
	целое	    oplen,
	cctx_T	    *cctx)
{
    char_u	*var_end = NULL;
    целое		is_decl = is_decl_command(cmdidx);

    если (lhs_init(lhs, var_start, is_decl, heredoc, &var_end) == FAIL)
	вернуть FAIL;

    если (cctx->ctx_skip != SKIP_YES)
    {
	// compile the LHS destination
	если (compile_lhs_var_dest(cctx, lhs, cmdidx, var_start, var_end,
						is_decl, has_cmd) == FAIL)
	    вернуть FAIL;
    }

    // handle "a:name" as a name, not index "name" in "a"
    если (lhs->lhs_varlen > 1 || var_start[lhs->lhs_varlen] != ':')
	var_end = lhs->lhs_dest_end;

    если (lhs->lhs_dest != dest_option && lhs->lhs_dest != dest_func_option)
    {
	// set the LHS variable type
	если (compile_lhs_set_type(cctx, lhs, var_end, is_decl) == FAIL)
	    вернуть FAIL;
    }

    если (oplen == 3 && !heredoc && !lhs_concatenable(lhs))
    {
	emsg(_(e_can_only_concatenate_to_string));
	вернуть FAIL;
    }

    если (lhs->lhs_lvar == NULL && lhs->lhs_dest == dest_local
						&& cctx->ctx_skip != SKIP_YES)
    {
	если (compile_lhs_new_local_var(cctx, lhs, var_start, cmdidx, oplen,
					is_decl, has_cmd, heredoc) == FAIL)
	    вернуть FAIL;
    }

    если (compile_lhs_set_member_type(cctx, lhs, var_start, is_decl, has_cmd)
								== FAIL)
	вернуть FAIL;

    вернуть OK;
}

/*
 * Figure out the LHS and check a few errors.
 */
    целое
compile_assign_lhs(
	char_u	    *var_start,
	lhs_T	    *lhs,
	cmdidx_T    cmdidx,
	целое	    is_decl,
	целое	    heredoc,
	целое	    has_cmd,	    // "var" before "var_start"
	целое	    oplen,
	cctx_T	    *cctx)
{
    если (compile_lhs(var_start, lhs, cmdidx, heredoc, has_cmd, oplen, cctx)
								       == FAIL)
	вернуть FAIL;

    если (!lhs->lhs_has_index && lhs->lhs_lvar == &lhs->lhs_arg_lvar)
    {
	semsg(_(e_cannot_assign_to_argument_str), lhs->lhs_name);
	вернуть FAIL;
    }
    если (!is_decl && lhs->lhs_lvar != NULL
			   && lhs->lhs_lvar->lv_const != ASSIGN_VAR
			   && !lhs->lhs_has_index)
    {
	semsg(_(e_cannot_assign_to_constant_str), lhs->lhs_name);
	вернуть FAIL;
    }
    вернуть OK;
}

/*
 * Return TRUE if "lhs" has a range index: "[expr : expr]".
 */
    static целое
has_list_index(char_u *idx_start, cctx_T *cctx)
{
    char_u  *p = idx_start;
    целое	    save_skip;

    если (*p != '[')
	вернуть FALSE;

    p = skipwhite(p + 1);
    если (*p == ':')
	вернуть TRUE;

    save_skip = cctx->ctx_skip;
    cctx->ctx_skip = SKIP_YES;
    (void)compile_expr0(&p, cctx);
    cctx->ctx_skip = save_skip;
    вернуть *skipwhite(p) == ':';
}

/*
 * For an assignment with an index, compile the "idx" in "var[idx]" or "key" in
 * "var.key".
 */
    static целое
compile_assign_index(
	char_u	*var_start,
	lhs_T	*lhs,
	целое	*range,
	cctx_T	*cctx)
{
    size_t	varlen = lhs->lhs_varlen;
    char_u	*p;
    целое		r = OK;
    целое		need_white_before = TRUE;
    целое		empty_second;

    p = var_start + varlen;
    если (*p == '[')
    {
	p = skipwhite(p + 1);
	если (*p == ':')
	{
	    // empty first index, push zero
	    r = generate_PUSHNR(cctx, 0);
	    need_white_before = FALSE;
	}
	иначе
	    r = compile_expr0(&p, cctx);

	если (r == OK && *skipwhite(p) == ':')
	{
	    // unlet var[idx : idx]
	    // blob[idx : idx] = value
	    *range = TRUE;
	    p = skipwhite(p);
	    empty_second = *skipwhite(p + 1) == ']';
	    если ((need_white_before && !IS_WHITE_OR_NUL(p[-1]))
		    || (!empty_second && !IS_WHITE_OR_NUL(p[1])))
	    {
		semsg(_(e_white_space_required_before_and_after_str_at_str),
								      ":", p);
		вернуть FAIL;
	    }
	    p = skipwhite(p + 1);
	    если (*p == ']')
		// empty second index, push "none"
		r = generate_PUSHSPEC(cctx, VVAL_NONE);
	    иначе
		r = compile_expr0(&p, cctx);
	}

	если (r == OK && *skipwhite(p) != ']')
	{
	    // this should not happen
	    emsg(_(e_missing_closing_square_brace));
	    r = FAIL;
	}
    }
    иначе если (lhs->lhs_member_idx >= 0)
    {
	// object member index
	r = generate_PUSHNR(cctx, lhs->lhs_member_idx);
    }
    иначе // if (*p == '.')
    {
	char_u *key_end = to_name_end(p + 1, TRUE);
	char_u *key = vim_strnsave(p + 1, key_end - p - 1);

	r = generate_PUSHS(cctx, &key);
    }
    вернуть r;
}

/*
 * For a LHS with an index, load the variable to be indexed.
 */
    static целое
compile_load_lhs(
	lhs_T	*lhs,
	char_u	*var_start,
	type_T	*rhs_type,
	cctx_T	*cctx)
{
    если (lhs->lhs_dest == dest_expr)
    {
	size_t	    varlen = lhs->lhs_varlen;
	целое	    c = var_start[varlen];
	целое	    lines_len = cctx->ctx_ufunc->uf_lines.ga_len;
	целое	    res;

	// Evaluate "ll[expr]" of "ll[expr][idx]".  End the line with a NUL and
	// limit the lines array length to avoid skipping to a following line.
	var_start[varlen] = NUL;
	cctx->ctx_ufunc->uf_lines.ga_len = cctx->ctx_lnum + 1;
	char_u *p = var_start;
	res = compile_expr0(&p, cctx);
	var_start[varlen] = c;
	cctx->ctx_ufunc->uf_lines.ga_len = lines_len;
	если (res == FAIL || p != var_start + varlen)
	{
	    // this should not happen
	    если (res != FAIL)
		emsg(_(e_missing_closing_square_brace));
	    вернуть FAIL;
	}

	lhs->lhs_type = cctx->ctx_type_stack.ga_len == 0 ? &t_void
						  : get_type_on_stack(cctx, 0);

	если (lhs->lhs_type->tt_type == VAR_CLASS
		|| (lhs->lhs_type->tt_type == VAR_OBJECT
		    && lhs->lhs_type != &t_object_any))
	{
	    // Check whether the class or object variable is modifiable
	    если (!lhs_class_member_modifiable(lhs, var_start, cctx))
		вернуть FAIL;
	}

	// Now we can properly check the type.  The variable is indexed, thus
	// we need the member type.  For a class or object we don't know the
	// type yet, it depends on what member is used.
	// The top item in the stack is the Dict, followed by the key and then
	// the type of the value.
	vartype_T vartype = lhs->lhs_type->tt_type;
	type_T *member_type = lhs->lhs_type->tt_member;
	если (rhs_type != NULL && member_type != NULL
		&& vartype != VAR_OBJECT && vartype != VAR_CLASS
		&& rhs_type != &t_void
		&& need_type(rhs_type, member_type, 0,
					    -3, 0, cctx, FALSE, FALSE) == FAIL)
	    вернуть FAIL;

	вернуть OK;
    }

    вернуть generate_loadvar(cctx, lhs);
}

/*
 * Produce code for loading "lhs" and also take care of an index.
 * Return OK/FAIL.
 */
    целое
compile_load_lhs_with_index(lhs_T *lhs, char_u *var_start, cctx_T *cctx)
{
    если (lhs->lhs_type->tt_type == VAR_OBJECT)
    {
	// "this.value": load "this" object and get the value at index for an
	// object or class member get the type of the member.
	// Also for "obj.value".
	char_u *dot = vim_strchr(var_start, '.');
	если (dot == NULL)
	{
	    semsg(_(e_missing_dot_after_object_str), lhs->lhs_name);
	    вернуть FAIL;
	}

	class_T	*cl = lhs->lhs_type->tt_class;
	type_T	*type = oc_member_type(cl, TRUE, dot + 1,
					  lhs->lhs_end, &lhs->lhs_member_idx);
	если (lhs->lhs_member_idx < 0)
	    вернуть FAIL;

	если (dot - var_start == 4 && STRNCMP(var_start, "this", 4) == 0)
	{
	    // load "this"
	    lvar_T  *lvar = lhs->lhs_lvar;
	    целое	    rc;

	    если (lvar->lv_from_outer > 0)
		rc = generate_LOADOUTER(cctx, lvar->lv_idx,
			lvar->lv_from_outer, lvar->lv_loop_depth,
			lvar->lv_loop_idx, type);
	    иначе
		rc = generate_LOAD(cctx, ISN_LOAD, lvar->lv_idx, NULL, type);

	    если (rc == FAIL)
		вернуть FAIL;
	}
	иначе
	{
	    // load object variable or argument
	    если (compile_load_lhs(lhs, var_start, lhs->lhs_type, cctx) == FAIL)
		вернуть FAIL;
	}
	если (IS_INTERFACE(cl))
	    вернуть generate_GET_ITF_MEMBER(cctx, cl, lhs->lhs_member_idx, type);
	вернуть generate_GET_OBJ_MEMBER(cctx, lhs->lhs_member_idx, type);
    }
    иначе если (lhs->lhs_type->tt_type == VAR_CLASS)
    {
	// "<classname>.value": load class variable "classname.value"
	char_u *dot = vim_strchr(var_start, '.');
	если (dot == NULL)
	{
	    (void)check_type_is_value(lhs->lhs_type);
	    вернуть FAIL;
	}

	class_T	*cl = lhs->lhs_type->tt_class;
	ocmember_T *m = class_member_lookup(cl, dot + 1,
						lhs->lhs_end - dot - 1,
						&lhs->lhs_member_idx);
	если (m == NULL)
	    вернуть FAIL;

	вернуть generate_CLASSMEMBER(cctx, TRUE, cl, lhs->lhs_member_idx);
    }

    если (compile_load_lhs(lhs, var_start, NULL, cctx) == FAIL)
	вернуть FAIL;

    если (lhs->lhs_has_index)
    {
	целое	range = FALSE;
	type_T	*type = get_type_on_stack(cctx, 0);

	// A member of an object or class is not obtained by indexing it.
	если (type->tt_type == VAR_CLASS
		|| (type->tt_type == VAR_OBJECT && type != &t_object_any))
	{
	    char_u *p = var_start + lhs->lhs_varlen;

	    вернуть compile_class_object_index(cctx, &p, type);
	}

	// Get member from list or dict.  First compile the
	// index value.
	если (compile_assign_index(var_start, lhs, &range, cctx) == FAIL)
	    вернуть FAIL;
	если (range)
	{
	    semsg(_(e_cannot_use_range_with_assignment_operator_str),
								    var_start);
	    вернуть FAIL;
	}

	// Get the member.
	если (compile_member(FALSE, NULL, cctx) == FAIL)
	    вернуть FAIL;
    }
    вернуть OK;
}

/*
 * Assignment to a list or dict member, or ":unlet" for the item, using the
 * information in "lhs".
 * Returns OK or FAIL.
 */
    целое
compile_assign_unlet(
    char_u	*var_start,
    lhs_T	*lhs,
    целое		is_assign,
    type_T	*rhs_type,
    cctx_T	*cctx)
{
    vartype_T	dest_type;
    целое		range = FALSE;

    если (compile_assign_index(var_start, lhs, &range, cctx) == FAIL)
	вернуть FAIL;
    если (is_assign && range
	    && lhs->lhs_type->tt_type != VAR_LIST
	    && lhs->lhs_type != &t_blob
	    && lhs->lhs_type != &t_any)
    {
	если (lhs->lhs_type->tt_type == VAR_TUPLE)
	    emsg(_(e_cannot_slice_tuple));
	иначе
	    semsg(_(e_cannot_use_range_with_assignment_str), var_start);
	вернуть FAIL;
    }

    // For "expr[idx]" the index is compiled before the expression, thus the
    // resulting type cannot be used here.
    если (lhs->lhs_dest == dest_expr
	    || lhs->lhs_type == NULL || lhs->lhs_type == &t_any)
    {
	// Index on variable of unknown type: check at runtime.
	dest_type = VAR_ANY;
    }
    иначе
    {
	dest_type = lhs->lhs_type->tt_type;
	если (dest_type == VAR_DICT && range)
	{
	    emsg(_(e_cannot_use_range_with_dictionary));
	    вернуть FAIL;
	}
	если (dest_type == VAR_DICT
		&& may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
	    вернуть FAIL;
	если (dest_type == VAR_LIST || dest_type == VAR_BLOB)
	{
	    type_T *type;

	    если (range)
	    {
		type = get_type_on_stack(cctx, 1);
		если (need_type(type, &t_number, 0,
					    -2, 0, cctx, FALSE, FALSE) == FAIL)
		вернуть FAIL;
	    }
	    type = get_type_on_stack(cctx, 0);
	    если ((dest_type != VAR_BLOB && type->tt_type != VAR_SPECIAL)
		    && need_type(type, &t_number, 0,
					    -1, 0, cctx, FALSE, FALSE) == FAIL)
		вернуть FAIL;
	}
    }

    если (cctx->ctx_skip == SKIP_YES)
	вернуть OK;

    // Load the dict, list or object.  On the stack we then have:
    // - value (for assignment, not for :unlet)
    // - index
    // - for [a : b] second index
    // - variable
    если (compile_load_lhs(lhs, var_start, rhs_type, cctx) == FAIL)
	вернуть FAIL;

    если (dest_type == VAR_LIST
	    || dest_type == VAR_DICT
	    || dest_type == VAR_BLOB
	    || dest_type == VAR_CLASS
	    || dest_type == VAR_OBJECT
	    || dest_type == VAR_ANY)
    {
	если (is_assign)
	{
	    если (range)
	    {
		если (generate_instr_drop(cctx, ISN_STORERANGE, 4) == NULL)
		    вернуть FAIL;
	    }
	    иначе
	    {
		isn_T	*isn = generate_instr_drop(cctx, ISN_STOREINDEX, 3);

		если (isn == NULL)
		    вернуть FAIL;
		isn->isn_arg.storeindex.si_vartype = dest_type;
		isn->isn_arg.storeindex.si_class = NULL;

		если (dest_type == VAR_OBJECT)
		{
		    class_T *cl = lhs->lhs_type->tt_class;

		    если (IS_INTERFACE(cl))
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
	иначе если (range)
	{
	    если (generate_instr_drop(cctx, ISN_UNLETRANGE, 3) == NULL)
		вернуть FAIL;
	}
	иначе
	{
	    если (generate_instr_drop(cctx, ISN_UNLETINDEX, 2) == NULL)
		вернуть FAIL;
	}
    }
    иначе
    {
	если (dest_type == VAR_TUPLE)
	    emsg(_(e_tuple_is_immutable));
	иначе
	    emsg(_(e_indexable_type_required));
	вернуть FAIL;
    }

    вернуть OK;
}

/*
 * Generate an instruction to push the default value for "vartype".
 * if "dest_local" is TRUE then for some types no instruction is generated.
 * "skip_store" is set to TRUE if no PUSH instruction is generated.
 * Returns OK or FAIL.
 */
    static целое
push_default_value(
	cctx_T	    *cctx,
	vartype_T   vartype,
	целое	    dest_is_local,
	целое	    *skip_store)
{
    целое r = OK;

    switch (vartype)
    {
	case VAR_BOOL:
	    r = generate_PUSHBOOL(cctx, VVAL_FALSE);
	    break;
	case VAR_FLOAT:
	    r = generate_PUSHF(cctx, 0.0);
	    break;
	case VAR_STRING:
	    r = generate_PUSHS(cctx, NULL);
	    break;
	case VAR_BLOB:
	    r = generate_PUSHBLOB(cctx, blob_alloc());
	    break;
	case VAR_FUNC:
	    r = generate_PUSHFUNC(cctx, NULL, &t_func_void, TRUE);
	    break;
	case VAR_LIST:
	    r = generate_NEWLIST(cctx, 0, FALSE);
	    break;
	case VAR_TUPLE:
	    r = generate_NEWTUPLE(cctx, 0, FALSE);
	    break;
	case VAR_DICT:
	    r = generate_NEWDICT(cctx, 0, FALSE);
	    break;
	case VAR_JOB:
	    r = generate_PUSHJOB(cctx);
	    break;
	case VAR_CHANNEL:
	    r = generate_PUSHCHANNEL(cctx);
	    break;
	case VAR_OBJECT:
	    r = generate_PUSHOBJ(cctx);
	    break;
	case VAR_NUMBER:
	case VAR_UNKNOWN:
	case VAR_ANY:
	case VAR_PARTIAL:
	case VAR_VOID:
	case VAR_INSTR:
	case VAR_CLASS:
	case VAR_TYPEALIAS:
	case VAR_SPECIAL:  // cannot happen
	    // This is skipped for local variables, they are always
	    // initialized to zero.  But in a "for" or "while" loop
	    // the value may have been changed.
	    если (dest_is_local && !inside_loop_scope(cctx))
		*skip_store = TRUE;
	    иначе
		r = generate_PUSHNR(cctx, 0);
	    break;
    }
    вернуть r;
}

/*
 * Compile assignment context.  Used when compiling an assignment statement.
 */
typedef struct cac_S cac_T;
struct cac_S
{
    cmdidx_T	cac_cmdidx;		// assignment command
    char_u	*cac_nextc;		// next character to parse
    lhs_T	cac_lhs;		// lhs of the assignment
    type_T	*cac_rhs_type;		// rhs type of an assignment
    char_u	*cac_op;		// assignment operator
    целое		cac_oplen;		// assignment operator length
    char_u	*cac_var_start;		// start of the variable names
    char_u	*cac_var_end;		// end of the variable names
    целое		cac_var_count;		// number of variables in assignment
    целое		cac_var_idx;		// variable index in a list
    целое		cac_semicolon;		// semicolon in [var1, var2; var3]
    garray_T	*cac_instr;
    целое		cac_instr_count;
    целое		cac_incdec;
    целое		cac_did_generate_slice;
    целое		cac_is_decl;
    целое		cac_is_const;
    целое		cac_start_lnum;
    type_T	*cac_inferred_type;
    целое		cac_skip_store;
};

/*
 * Initialize the compile assignment context.
 */
    static void
compile_assign_context_init(cac_T *cac, cctx_T *cctx, целое cmdidx, char_u *arg)
{
    CLEAR_FIELD(*cac);
    cac->cac_cmdidx = cmdidx;
    cac->cac_instr = &cctx->ctx_instr;
    cac->cac_rhs_type = &t_any;
    cac->cac_is_decl = is_decl_command(cmdidx);
    cac->cac_start_lnum = SOURCING_LNUM;
    cac->cac_instr_count = -1;
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
    static целое
compile_assign_obj_new_arg(char_u **argp, cctx_T *cctx)
{
    char_u *arg = *argp;

    arg += 11;	    // skip "ifargisset"
    целое def_arg_idx = getdigits(&arg);
    arg = skipwhite(arg);

    // Use a JUMP_IF_ARG_NOT_SET instruction to skip if the value was not
    // given and the default value is "v:none".
    целое stack_offset = STACK_FRAME_SIZE +
				(cctx->ctx_ufunc->uf_va_name != NULL ? 1 : 0);
    целое def_arg_count = cctx->ctx_ufunc->uf_def_args.ga_len;
    целое arg_offset = def_arg_idx - def_arg_count - stack_offset;

    если (generate_JUMP_IF_ARG(cctx, ISN_JUMP_IF_ARG_NOT_SET,
			     arg_offset) == FAIL)
	вернуть FAIL;

    *argp = arg;
    вернуть OK;
}

/*
 * Translate the increment (++) and decrement (--) operators to the
 * corresponding compound operators (+= or -=).
 *
 * Returns OK on success and FAIL on syntax error.
 */
    static целое
translate_incdec_op(exarg_T *eap, cac_T *cac)
{
    если (VIM_ISWHITE(eap->cmd[2]))
    {
	semsg(_(e_no_white_space_allowed_after_str_str),
		eap->cmdidx == CMD_increment ? "++" : "--", eap->cmd);
	вернуть FAIL;
    }
    cac->cac_op = (char_u *)(eap->cmdidx == CMD_increment ? "+=" : "-=");
    cac->cac_oplen = 2;
    cac->cac_incdec = TRUE;

    вернуть OK;
}

/*
 * Process the operator in an assignment statement.
 */
    static целое
compile_assign_process_operator(
    exarg_T	*eap,
    char_u	*arg,
    cac_T	*cac,
    целое		*heredoc,
    char_u	**retstr)
{
    *retstr = NULL;

    если (eap->cmdidx == CMD_increment || eap->cmdidx == CMD_decrement)
	// Change an unary operator to a compound operator
	вернуть translate_incdec_op(eap, cac);

    char_u *sp = cac->cac_nextc;
    cac->cac_nextc = skipwhite(cac->cac_nextc);
    cac->cac_op = cac->cac_nextc;
    cac->cac_oplen = assignment_len(cac->cac_nextc, heredoc);

    если (cac->cac_var_count > 0 && cac->cac_oplen == 0)
    {
	// can be something like "[1, 2]->func()"
	*retstr = arg;
	вернуть FAIL;
    }

    // need white space before and after the operator
    если (cac->cac_oplen > 0 && (!VIM_ISWHITE(*sp)
		|| !IS_WHITE_OR_NUL(cac->cac_op[cac->cac_oplen])))
    {
	error_white_both(cac->cac_op, cac->cac_oplen);
	вернуть FAIL;
    }

    вернуть OK;
}

/*
 * Find the start of an assignment statement.
 */
    static char_u *
compile_assign_compute_start(char_u *arg, целое var_count)
{
    если (var_count > 0)
	// [var1, var2] = [val1, val2]
	// skip over the "["
	вернуть skipwhite(arg + 1);

    вернуть arg;
}

/*
 * Parse a heredoc assignment starting at "p".  Returns a pointer to the
 * beginning of the heredoc content.
 */
    static char_u *
parse_heredoc_assignment(exarg_T *eap, cctx_T *cctx, cac_T *cac)
{
    // [let] varname =<< [trim] {end}
    eap->ea_getline = exarg_getline;
    eap->cookie = cctx;

    list_T *l = heredoc_get(eap, cac->cac_nextc + 3, FALSE, TRUE);
    если (l == NULL)
	вернуть NULL;

    list_free(l);
    cac->cac_nextc += STRLEN(cac->cac_nextc);

    вернуть cac->cac_nextc;
}

/*
 * Check the type of a RHS expression in a list assignment statement.
 * The RHS expression is already compiled.  So the type is on the stack.
 */
    static целое
compile_assign_list_check_rhs_type(cctx_T *cctx, cac_T *cac)
{
    type_T	*stacktype;

    stacktype = cctx->ctx_type_stack.ga_len == 0 ? &t_void
						: get_type_on_stack(cctx, 0);
    если (stacktype->tt_type == VAR_VOID)
    {
	emsg(_(e_cannot_use_void_value));
	вернуть FAIL;
    }

    если (stacktype->tt_type != VAR_LIST && stacktype->tt_type != VAR_TUPLE
					&& stacktype->tt_type != VAR_ANY)
    {
	emsg(_(e_list_or_tuple_required));
	вернуть FAIL;
    }

    если (need_type(stacktype,
		  stacktype->tt_type == VAR_TUPLE ? &t_tuple_any : &t_list_any,
		  TYPECHK_TUPLE_OK, -1, 0, cctx, FALSE, FALSE) == FAIL)
	вернуть FAIL;

    // The check accepts both a list and a tuple. Keep "any", making it a list
    // would reject a tuple later on.
    если (stacktype->tt_type == VAR_ANY)
	set_type_on_stack(cctx, &t_any, 0);

    если (stacktype->tt_type == VAR_TUPLE)
    {
	если (stacktype->tt_argcount != 1)
	    cac->cac_rhs_type = &t_any;
	иначе
	{
	    если (stacktype->tt_flags & TTFLAG_VARARGS)
		cac->cac_rhs_type = stacktype->tt_args[0]->tt_member;
	    иначе
		cac->cac_rhs_type = stacktype->tt_args[0];
	}
    }
    иначе если (stacktype->tt_member != NULL)
	cac->cac_rhs_type = stacktype->tt_member;

    вернуть OK;
}

/*
 * In a list assignment statement, if a constant list was used, check the
 * length.  Returns OK if the length check succeeds.  Returns FAIL otherwise.
 */
    static целое
compile_assign_list_check_length(cctx_T *cctx, cac_T *cac)
{
    целое	needed_list_len;
    целое	did_check = FALSE;

    needed_list_len = cac->cac_semicolon
				? cac->cac_var_count - 1
				: cac->cac_var_count;
    если (cac->cac_instr->ga_len > 0)
    {
	isn_T	*isn = ((isn_T *)cac->cac_instr->ga_data) +
						cac->cac_instr->ga_len - 1;

	если (isn->isn_type == ISN_NEWLIST || isn->isn_type == ISN_NEWTUPLE)
	{
	    did_check = TRUE;
	    если (cac->cac_semicolon ?
			isn->isn_arg.number < needed_list_len
			: isn->isn_arg.number != needed_list_len)
	    {
		semsg(_(e_expected_nr_items_but_got_nr),
			needed_list_len, (целое)isn->isn_arg.number);
		вернуть FAIL;
	    }
	}
    }

    если (!did_check)
	generate_CHECKLEN(cctx, needed_list_len, cac->cac_semicolon);

    вернуть OK;
}

/*
 * Evaluate the expression for "[var, var] = expr" assignment.
 * A line break may follow the assignment operator "=".
 */
    static char_u *
compile_assign_list_expr(cctx_T *cctx, cac_T *cac)
{
    char_u *whitep;

    whitep = cac->cac_op + cac->cac_oplen;

    если (may_get_next_line_error(whitep, &cac->cac_nextc, cctx) == FAIL)
	вернуть NULL;

    // compile RHS expression
    если (compile_expr0(&cac->cac_nextc, cctx) == FAIL)
	вернуть NULL;

    если (cctx->ctx_skip == SKIP_YES)
	// no need to parse more when skipping
	вернуть cac->cac_nextc;

    если (compile_assign_list_check_rhs_type(cctx, cac) == FAIL)
	вернуть NULL;

    // If a constant list was used we can check the length right here.
    если (compile_assign_list_check_length(cctx, cac) == FAIL)
	вернуть FAIL;

    вернуть cac->cac_nextc;
}

/*
 * Find and return the end of a heredoc or a list of variables assignment
 * statement.  For a single variable assignment statement, returns the current
 * end.
 * Returns NULL on failure.
 */
    static char_u *
compile_assign_compute_end(
    exarg_T	*eap,
    cctx_T	*cctx,
    cac_T	*cac,
    целое		heredoc)
{
    если (heredoc)
    {
	cac->cac_nextc = parse_heredoc_assignment(eap, cctx, cac);
	вернуть cac->cac_nextc;
    }

    если (cac->cac_var_count > 0)
    {
	// for "[var, var] = expr" evaluate the expression. The list of
	// variables are processed later.
	// A line break may follow the "=".
	cac->cac_nextc = compile_assign_list_expr(cctx, cac);
	вернуть cac->cac_nextc;
    }

    вернуть cac->cac_var_end;
}

/*
 * For "var = expr" evaluate the expression.
 */
    static целое
compile_assign_single_eval_expr(cctx_T *cctx, cac_T *cac)
{
    целое		ret = OK;
    char_u	*whitep;
    lhs_T	*lhs = &cac->cac_lhs;
    lvar_T	*lvp;
    lvar_T	save_lhs_lvar;

    // Compile the expression.
    если (cac->cac_incdec)
	вернуть generate_PUSHNR(cctx, 1);

    // Temporarily hide the new local variable here, it is
    // not available to this expression.
    если (lhs->lhs_new_local)
    {
	--cctx->ctx_locals.ga_len;

	// Save the local variable value (compiling the RHS expression may
	// create new local variables).
	lvp = ((lvar_T *)cctx->ctx_locals.ga_data) + cctx->ctx_locals.ga_len;
	save_lhs_lvar = *lvp;
    }
    whitep = cac->cac_op + cac->cac_oplen;

    если (may_get_next_line_error(whitep, &cac->cac_nextc, cctx) == FAIL)
    {
	если (lhs->lhs_new_local)
	    ++cctx->ctx_locals.ga_len;
	вернуть FAIL;
    }

    ret = compile_expr0_ext(&cac->cac_nextc, cctx, &cac->cac_is_const);
    если (lhs->lhs_new_local)
    {
	// Restore the local variable value.  Update lhs_lvar as the index of
	// the local variable might have changed.
	lvp = ((lvar_T *)cctx->ctx_locals.ga_data) + cctx->ctx_locals.ga_len;
	*lvp = save_lhs_lvar;
	lhs->lhs_lvar = lvp;

	++cctx->ctx_locals.ga_len;
    }

    вернуть ret;
}

/*
 * When compiling an assignment, set the LHS type to the RHS type.
 */
    static целое
compile_assign_set_lhs_type_from_rhs(
    cctx_T	*cctx,
    cac_T	*cac,
    lhs_T	*lhs,
    type_T	*rhs_type)
{
    если (rhs_type->tt_type == VAR_VOID)
    {
	emsg(_(e_cannot_use_void_value));
	вернуть FAIL;
    }

    type_T *type;

    // An empty list or dict has a &t_unknown member, for a variable that
    // implies &t_any.
    если (rhs_type == &t_list_empty)
	type = &t_list_any;
    иначе если (rhs_type == &t_dict_empty)
	type = &t_dict_any;
    иначе если (rhs_type == &t_unknown)
	type = &t_any;
    иначе
    {
	type = rhs_type;
	cac->cac_inferred_type = rhs_type;
    }

    set_var_type(lhs->lhs_lvar, type, cctx);

    вернуть OK;
}

/*
 * Returns TRUE if the "rhs_type" can be assigned to the "lhs" variable.
 * Used when compiling an assignment statement.
 */
    static целое
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
    если (cac->cac_var_count > 0)
    {
	where.wt_index = cac->cac_var_idx + 1;
	where.wt_kind = WT_VARIABLE;
    }

    // If assigning to a list or dict member, use the member type.
    // Not for "list[:] =".
    если (lhs->lhs_has_index &&
	    !has_list_index(cac->cac_var_start + lhs->lhs_varlen, cctx))
	use_type = lhs->lhs_member_type;

    если (need_type_where(rhs_type, use_type, 0, -1, where, cctx, FALSE,
						cac->cac_is_const) == FAIL)
	вернуть FALSE;

    вернуть TRUE;
}

/*
 * Compare the LHS type with the RHS type in an assignment.
 */
    static целое
compile_assign_check_type(cctx_T *cctx, cac_T *cac)
{
    lhs_T	*lhs = &cac->cac_lhs;
    type_T	*rhs_type;

    rhs_type = cctx->ctx_type_stack.ga_len == 0
					? &t_void
					: get_type_on_stack(cctx, 0);
    cac->cac_rhs_type = rhs_type;

    если (check_type_is_value(rhs_type) == FAIL)
	вернуть FAIL;

    если (lhs->lhs_lvar != NULL && (cac->cac_is_decl || !lhs->lhs_has_type))
    {
	если (rhs_type->tt_type == VAR_FUNC
					|| rhs_type->tt_type == VAR_PARTIAL)
	{
	    // Make sure the variable name can be used as a funcref
	    если (!lhs->lhs_has_index
				&& var_wrong_func_name(lhs->lhs_name, TRUE))
		вернуть FAIL;
	}

	если (lhs->lhs_new_local && !lhs->lhs_has_type)
	{
	    // The LHS variable doesn't have a type.  Set it to the RHS type.
	    если (compile_assign_set_lhs_type_from_rhs(cctx, cac, lhs, rhs_type)
								== FAIL)
		вернуть FAIL;
	}
	иначе если (*cac->cac_op == '=')
	{
	    если (!compile_assign_valid_rhs_type(cctx, cac, lhs, rhs_type))
		вернуть FAIL;
	}
    }
    иначе
    {
	// Assigning to a register using @r = "abc"

	type_T *lhs_type = lhs->lhs_member_type;

	// Special case: assigning to @# can use a number or a string.
	// Also: can assign a number to a float.
	если ((lhs_type == &t_number_or_string || lhs_type == &t_float)
					&& rhs_type->tt_type == VAR_NUMBER)
	    lhs_type = &t_number;

	если (*cac->cac_nextc != '=')
	{
	    если (need_type(rhs_type, lhs_type, 0, -1, 0, cctx, FALSE,
							FALSE) == FAIL)
		вернуть FAIL;
	}
    }

    вернуть OK;
}

/*
 * Compile the RHS expression in an assignment statement and generate the
 * instructions.
 */
    static целое
compile_assign_rhs_expr(cctx_T *cctx, cac_T *cac)
{
    cac->cac_is_const = FALSE;

    // for "+=", "*=", "..=" etc. first load the current value
    если (*cac->cac_op != '='
	    && compile_load_lhs_with_index(&cac->cac_lhs, cac->cac_var_start,
								cctx) == FAIL)
	вернуть FAIL;

    // For "var = expr" evaluate the expression.
    если (cac->cac_var_count == 0)
    {
	целое	ret;

	// Compile the expression.
	cac->cac_instr_count = cac->cac_instr->ga_len;
	ret = compile_assign_single_eval_expr(cctx, cac);
	если (ret == FAIL)
	    вернуть FAIL;
    }
    иначе если (cac->cac_semicolon && cac->cac_var_idx == cac->cac_var_count - 1)
    {
	// For "[var; var] = expr" get the rest of the list
	cac->cac_did_generate_slice = TRUE;
	если (generate_SLICE(cctx, cac->cac_var_count - 1) == FAIL)
	    вернуть FAIL;
    }
    иначе
    {
	// For "[var, var] = expr" get the "var_idx" item from the
	// list.
	целое with_op = *cac->cac_op != '=';
	если (generate_GETITEM(cctx, cac->cac_var_idx, with_op) == FAIL)
	    вернуть FAIL;
    }

    если (compile_assign_check_type(cctx, cac) == FAIL)
	вернуть FAIL;

    вернуть OK;
}

/*
 * Compile the RHS expression in an assignment
 */
    static целое
compile_assign_rhs(cctx_T *cctx, cac_T *cac)
{
    lhs_T	*lhs = &cac->cac_lhs;

    если (cctx->ctx_skip == SKIP_YES)
    {
	если (cac->cac_oplen > 0 && cac->cac_var_count == 0)
	{
	    // skip over the "=" and the expression
	    cac->cac_nextc = skipwhite(cac->cac_op + cac->cac_oplen);
	    (void)compile_expr0(&cac->cac_nextc, cctx);
	}
	вернуть OK;
    }

    // If RHS is specified, then generate instructions for RHS expression
    если (cac->cac_oplen > 0)
	вернуть compile_assign_rhs_expr(cctx, cac);

    если (cac->cac_cmdidx == CMD_final)
    {
	emsg(_(e_final_requires_a_value));
	вернуть FAIL;
    }

    если (cac->cac_cmdidx == CMD_const)
    {
	emsg(_(e_const_requires_a_value));
	вернуть FAIL;
    }

    если (!lhs->lhs_has_type || lhs->lhs_dest == dest_option
					|| lhs->lhs_dest == dest_func_option)
    {
	emsg(_(e_type_or_initialization_required));
	вернуть FAIL;
    }

    // variables are always initialized
    если (GA_GROW_FAILS(cac->cac_instr, 1))
	вернуть FAIL;

    cac->cac_instr_count = cac->cac_instr->ga_len;

    вернуть push_default_value(cctx, lhs->lhs_member_type->tt_type,
			      lhs->lhs_dest == dest_local,
			      &cac->cac_skip_store);
}

/*
 * Returns OK if "type" supports compound operator "op_arg" (e.g. +=, -=, %=,
 * etc.).  Compound operators are not supported with a tuple and a dict.
 * Returns FAIL if compound operator is not supported.
 */
    static целое
check_type_supports_compound_op(type_T *type, char_u op_arg)
{
    если (type->tt_type == VAR_TUPLE || type->tt_type == VAR_DICT)
    {
	char_u	op[2];

	op[0] = op_arg;
	op[1] = NUL;
	semsg(_(e_wrong_variable_type_for_str_equal), op);
	вернуть FAIL;
    }

    вернуть OK;
}

/*
 * Compile a compound op assignment statement (+=, -=, *=, %=, etc.)
 */
    static целое
compile_assign_compound_op(cctx_T *cctx, cac_T *cac)
{
    lhs_T	    *lhs = &cac->cac_lhs;
    type_T	    *expected;
    type_T	    *stacktype = NULL;

    если (cac->cac_lhs.lhs_type->tt_type == VAR_TUPLE
	    && check_type_supports_compound_op(cac->cac_lhs.lhs_type,
							*cac->cac_op) == FAIL)
	вернуть FAIL;

    если (*cac->cac_op == '.')
    {
	expected = lhs->lhs_member_type;
	stacktype = get_type_on_stack(cctx, 0);
	если (expected != &t_string
		&& need_type(stacktype, expected, 0, -1, 0, cctx,
					FALSE, FALSE) == FAIL)
	    вернуть FAIL;
	иначе если (may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
	    вернуть FAIL;
    }
    иначе
    {
	expected = lhs->lhs_member_type;
	stacktype = get_type_on_stack(cctx, 0);

	если (check_type_supports_compound_op(expected, *cac->cac_op) == FAIL)
	    вернуть FAIL;

	если (
		// If variable is float operation with number is OK.
		!(expected == &t_float && (stacktype == &t_number
					|| stacktype == &t_number_bool))
		&& need_type(stacktype, expected, TYPECHK_NUMBER_OK, -1, 0,
						cctx, FALSE, FALSE) == FAIL)
	    вернуть FAIL;
    }

    если (*cac->cac_op == '.')
    {
	если (generate_CONCAT(cctx, 2) == FAIL)
	    вернуть FAIL;
    }
    иначе если (*cac->cac_op == '+')
    {
	если (generate_add_instr(cctx,
		    operator_type(lhs->lhs_member_type, stacktype),
		    lhs->lhs_member_type, stacktype,
		    EXPR_APPEND) == FAIL)
	    вернуть FAIL;
    }
    иначе если (generate_two_op(cctx, cac->cac_op) == FAIL)
	вернуть FAIL;

    вернуть OK;
}

/*
 * Generate the STORE and SETTYPE instructions for an assignment statement.
 */
    static целое
compile_assign_generate_store(cctx_T *cctx, cac_T *cac)
{
    lhs_T	*lhs = &cac->cac_lhs;
    целое		save_lnum;

    // Use the line number of the assignment for store instruction.
    save_lnum = cctx->ctx_lnum;
    cctx->ctx_lnum = cac->cac_start_lnum - 1;

    если (lhs->lhs_has_index)
    {
	// Use the info in "lhs" to store the value at the index in the
	// list, dict or object.
	если (compile_assign_unlet(cac->cac_var_start, &cac->cac_lhs,
				 TRUE, cac->cac_rhs_type, cctx) == FAIL)
	{
	    cctx->ctx_lnum = save_lnum;
	    вернуть FAIL;
	}
    }
    иначе
    {
	если (cac->cac_is_decl && cac->cac_cmdidx == CMD_const &&
			(lhs->lhs_dest == dest_script
			 || lhs->lhs_dest == dest_script_v9
			 || lhs->lhs_dest == dest_global
			 || lhs->lhs_dest == dest_local))
	    // ":const var": lock the value, but not referenced variables
	    generate_LOCKCONST(cctx);

	type_T	*inferred_type = cac->cac_inferred_type;

	если ((lhs->lhs_type->tt_type == VAR_DICT
		    || lhs->lhs_type->tt_type == VAR_LIST)
		&& lhs->lhs_type->tt_member != NULL
		&& lhs->lhs_type->tt_member != &t_any
		&& lhs->lhs_type->tt_member != &t_unknown)
	    // Set the type in the list or dict, so that it can be
	    // checked, also in legacy script.
	    generate_SETTYPE(cctx, lhs->lhs_type);
	иначе если (lhs->lhs_type->tt_type == VAR_TUPLE
					&& lhs->lhs_type->tt_argcount != 0)
	    generate_SETTYPE(cctx, lhs->lhs_type);
	иначе если (inferred_type != NULL
		&& (inferred_type->tt_type == VAR_DICT
		    || inferred_type->tt_type == VAR_LIST)
		&& inferred_type->tt_member != NULL
		&& inferred_type->tt_member != &t_unknown
		&& inferred_type->tt_member != &t_any)
	    // Set the type in the list or dict, so that it can be
	    // checked, also in legacy script.
	    generate_SETTYPE(cctx, inferred_type);
	иначе если (inferred_type != NULL
				&& inferred_type->tt_type == VAR_TUPLE
				&& inferred_type->tt_argcount > 0)
	    generate_SETTYPE(cctx, inferred_type);

	если (!cac->cac_skip_store &&
		generate_store_lhs(cctx, &cac->cac_lhs,
				   cac->cac_instr_count,
				   cac->cac_is_decl) == FAIL)
	{
	    cctx->ctx_lnum = save_lnum;
	    вернуть FAIL;
	}
    }

    cctx->ctx_lnum = save_lnum;

    вернуть OK;
}

/*
 * Process the variable(s) in an assignment statement
 */
    static целое
compile_assign_process_variables(
    cctx_T	*cctx,
    cac_T	*cac,
    целое		cmdidx,
    целое		heredoc,
    целое		has_cmd,
    целое		has_argisset_prefix,
    целое		jump_instr_idx)
{
    /*
     * Loop over variables in "[var, var] = expr".
     * For "name = expr" and "var name: type" this is done only once.
     */
    для (cac->cac_var_idx = 0; cac->cac_var_idx == 0 ||
	    cac->cac_var_idx < cac->cac_var_count; cac->cac_var_idx++)
    {
	если (cac->cac_var_start[0] == '_'
				&& !eval_isnamec(cac->cac_var_start[1]))
	{
	    // Ignore underscore in "[a, _, b] = list".
	    если (cac->cac_var_count > 0)
	    {
		cac->cac_var_start = skipwhite(cac->cac_var_start + 2);
		continue;
	    }
	    emsg(_(e_cannot_use_underscore_here));
	    вернуть FAIL;
	}
	vim_free(cac->cac_lhs.lhs_name);

	/*
	 * Figure out the LHS type and other properties.
	 */
	если (compile_assign_lhs(cac->cac_var_start, &cac->cac_lhs, cmdidx,
		    cac->cac_is_decl, heredoc, has_cmd,
		    cac->cac_oplen, cctx) == FAIL)
	    вернуть FAIL;

	// Compile the RHS expression
	если (heredoc)
	{
	    SOURCING_LNUM = cac->cac_start_lnum;
	    если (cac->cac_lhs.lhs_has_type
		    && need_type(&t_list_string, cac->cac_lhs.lhs_type,
				0, -1, 0, cctx, FALSE, FALSE) == FAIL)
		вернуть FAIL;
	}
	иначе
	{
	    если (compile_assign_rhs(cctx, cac) == FAIL)
		вернуть FAIL;
	    если (cac->cac_var_count == 0)
		cac->cac_var_end = cac->cac_nextc;
	}

	// no need to parse more when skipping
	если (cctx->ctx_skip == SKIP_YES)
	    break;

	если (cac->cac_oplen > 0 && *cac->cac_op != '=')
	{
	    если (compile_assign_compound_op(cctx, cac) == FAIL)
		вернуть FAIL;
	}

	// generate the store instructions
	если (compile_assign_generate_store(cctx, cac) == FAIL)
	    вернуть FAIL;

	если (cac->cac_var_idx + 1 < cac->cac_var_count)
	    cac->cac_var_start = skipwhite(cac->cac_lhs.lhs_end + 1);

	если (has_argisset_prefix)
	{
	    // set instruction index in JUMP_IF_ARG_SET to here
	    isn_T *isn = ((isn_T *)cac->cac_instr->ga_data) + jump_instr_idx;
	    isn->isn_arg.jumparg.jump_where = cac->cac_instr->ga_len;
	}
    }

    вернуть OK;
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
    static char_u *
compile_assignment(
    char_u	*arg_start,
    exarg_T	*eap,
    cmdidx_T	cmdidx,
    cctx_T	*cctx)
{
    cac_T	cac;
    char_u	*arg = arg_start;
    char_u	*retstr = NULL;
    целое		heredoc = FALSE;
    целое		jump_instr_idx;

    compile_assign_context_init(&cac, cctx, cmdidx, arg);

    jump_instr_idx = cac.cac_instr->ga_len;

    // process object variable initialization in a new() constructor method
    целое	has_argisset_prefix = STRNCMP(arg, "ifargisset ", 11) == 0;
    если (has_argisset_prefix &&
			compile_assign_obj_new_arg(&arg, cctx) == FAIL)
	goto theend;

    // Skip over the "varname" or "[varname, varname]" to get to any "=".
    cac.cac_nextc = skip_var_list(arg, TRUE, &cac.cac_var_count,
						&cac.cac_semicolon, TRUE);
    если (cac.cac_nextc == NULL)
	вернуть *arg == '[' ? arg : NULL;

    если (compile_assign_process_operator(eap, arg, &cac, &heredoc,
							&retstr) == FAIL)
	вернуть retstr;

    // Compute the start of the assignment
    cac.cac_var_start = compile_assign_compute_start(arg, cac.cac_var_count);

    // Compute the end of the assignment
    cac.cac_var_end = compile_assign_compute_end(eap, cctx, &cac, heredoc);
    если (cac.cac_var_end == NULL)
	вернуть NULL;

    целое has_cmd = cac.cac_var_start > eap->cmd;

    /* process the variable(s) */
    если (compile_assign_process_variables(cctx, &cac, cmdidx, heredoc,
					 has_cmd, has_argisset_prefix,
					 jump_instr_idx) == FAIL)
	goto theend;

    // For "[var, var] = expr" drop the "expr" value.
    // Also for "[var, var; _] = expr".
    если (cctx->ctx_skip != SKIP_YES && cac.cac_var_count > 0 &&
	    (!cac.cac_semicolon || !cac.cac_did_generate_slice))
    {
	если (generate_instr_drop(cctx, ISN_DROP, 1) == NULL)
	    goto theend;
    }

    retstr = skipwhite(cac.cac_var_end);

theend:
    vim_free(cac.cac_lhs.lhs_name);
    вернуть retstr;
}

/*
 * Check for an assignment at "eap->cmd", compile it if found.
 * Return NOTDONE if there is none, FAIL for failure, OK if done.
 */
    static целое
may_compile_assignment(exarg_T *eap, char_u **line, cctx_T *cctx)
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
    если (p > eap->cmd && *p != NUL)
    {
	char_u *var_end;
	целое	oplen;
	целое	heredoc;

	если (eap->cmd[0] == '@')
	    var_end = eap->cmd + 2;
	иначе
	    var_end = find_name_end(pskip, NULL, NULL,
					FNE_CHECK_START | FNE_INCL_BR);
	oplen = assignment_len(skipwhite(var_end), &heredoc);
	если (oplen > 0)
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
	    если (*eap->cmd == '&'
		    || *eap->cmd == '$'
		    || *eap->cmd == '@'
		    || ((len) > 2 && eap->cmd[1] == ':')
		    || STRNCMP(eap->cmd, "g:[", 3) == 0
		    || variable_exists(eap->cmd, len, cctx))
	    {
		*line = compile_assignment(eap->cmd, eap, CMD_SIZE, cctx);
		если (*line == NULL || *line == eap->cmd)
		    вернуть FAIL;
		вернуть OK;
	    }
	}
    }

    // might be "[var, var] = expr" or "ifargisset this.member = expr"
    если (*eap->cmd == '[' || STRNCMP(eap->cmd, "ifargisset ", 11) == 0)
    {
	*line = compile_assignment(eap->cmd, eap, CMD_SIZE, cctx);
	если (*line == NULL)
	    вернуть FAIL;
	если (*line != eap->cmd)
	    вернуть OK;
    }
    вернуть NOTDONE;
}

/*
 * Check if arguments of "ufunc" shadow variables in "cctx".
 * Return OK or FAIL.
 */
    static целое
check_args_shadowing(ufunc_T *ufunc, cctx_T *cctx)
{
    целое	    i;
    char_u  *arg;
    целое	    r = OK;

    // Make sure arguments are not found when compiling a second time.
    ufunc->uf_args_visible = 0;

    // Check for arguments shadowing variables from the context.
    для (i = 0; i < ufunc->uf_args.ga_len; ++i)
    {
	arg = ((char_u **)(ufunc->uf_args.ga_data))[i];
	если (check_defined(arg, STRLEN(arg), cctx, NULL, TRUE) == FAIL)
	{
	    r = FAIL;
	    break;
	}
    }
    ufunc->uf_args_visible = ufunc->uf_args.ga_len;
    вернуть r;
}

#ifdef HAS_MESSAGE_WINDOW
/*
 * Get a count before a command.  Can only be a number.
 * Returns zero if there is no count.
 * Returns -1 if there is something wrong.
 */
    static long
get_cmd_count(char_u *line, exarg_T *eap)
{
    char_u *p;

    // skip over colons and white space
    для (p = line; *p == ':' || VIM_ISWHITE(*p); ++p)
	;
    если (!SAFE_isdigit(*p))
    {
	// The command or modifiers must be following.  Assume a lower case
	// character means there is a modifier.
	если (p < eap->cmd && !vim_islower(*p))
	{
	    emsg(_(e_invalid_range));
	    вернуть -1;
	}
	вернуть 0;
    }
    вернуть atol((char *)p);
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

    если (debug_break_level > 0 || may_break_in_function(ufunc))
	вернуть CT_DEBUG;
#ifdef FEAT_PROFILE
    если (do_profiling == PROF_YES)
    {
	если (!ufunc->uf_profiling && has_profiling(FALSE, ufunc->uf_name, NULL,
							    &ufunc->uf_hash))
	    func_do_profile(ufunc);
	если (ufunc->uf_profiling)
	    вернуть CT_PROFILE;
    }
#endif
    вернуть CT_NONE;
}

/*
 * Free the compiled instructions saved for a def function.  This is used when
 * compiling a def function and the function was compiled before.
 * The index is reused.
 */
    static void
clear_def_function(ufunc_T *ufunc, compiletype_T compile_type)
{
    isn_T	*instr_dest = NULL;
    dfunc_T	*dfunc;

    dfunc = ((dfunc_T *)def_functions.ga_data) + ufunc->uf_dfunc_idx;

    switch (compile_type)
    {
	case CT_PROFILE:
#ifdef FEAT_PROFILE
	    instr_dest = dfunc->df_instr_prof; break;
#endif
	case CT_NONE:   instr_dest = dfunc->df_instr; break;
	case CT_DEBUG:  instr_dest = dfunc->df_instr_debug; break;
    }

    если (instr_dest != NULL)
	// Was compiled in this mode before: Free old instructions.
	delete_def_function_contents(dfunc, FALSE);

    ga_clear_strings(&dfunc->df_var_names);
    dfunc->df_defer_var_idx = 0;
}

/*
 * Add a function to the list of :def functions.
 * This sets "ufunc->uf_dfunc_idx" but the function isn't compiled yet.
 */
    static целое
add_def_function(ufunc_T *ufunc)
{
    dfunc_T *dfunc;

    если (def_functions.ga_len == 0)
    {
	// The first position is not used, so that a zero uf_dfunc_idx means it
	// wasn't set.
	если (GA_GROW_FAILS(&def_functions, 1))
	    вернуть FAIL;
	++def_functions.ga_len;
    }

    // Add the function to "def_functions".
    если (GA_GROW_FAILS(&def_functions, 1))
	вернуть FAIL;
    dfunc = ((dfunc_T *)def_functions.ga_data) + def_functions.ga_len;
    CLEAR_POINTER(dfunc);
    dfunc->df_idx = def_functions.ga_len;
    ufunc->uf_dfunc_idx = dfunc->df_idx;
    dfunc->df_ufunc = ufunc;
    dfunc->df_name = vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
    ga_init2(&dfunc->df_var_names, sizeof(char_u *), 10);
    ++dfunc->df_refcount;
    ++def_functions.ga_len;
    вернуть OK;
}

    static целое
compile_dfunc_ufunc_init(
    ufunc_T		*ufunc,
    cctx_T		*outer_cctx,
    compiletype_T	compile_type,
    целое			*new_def_function)
{
    // When using a function that was compiled before: Free old instructions.
    // The index is reused.  Otherwise add a new entry in "def_functions".
    если (ufunc->uf_dfunc_idx > 0)
	clear_def_function(ufunc, compile_type);
    иначе
    {
	если (add_def_function(ufunc) == FAIL)
	    вернуть FAIL;

	*new_def_function = TRUE;
    }

    если ((ufunc->uf_flags & FC_CLOSURE) && outer_cctx == NULL)
    {
	semsg(_(e_compiling_closure_without_context_str),
						printable_func_name(ufunc));
	вернуть FAIL;
    }

    ufunc->uf_def_status = UF_COMPILING;

    вернуть OK;
}

/*
 * Initialize the compilation context for compiling a def function.
 */
    static void
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
    ga_init2(&cctx->ctx_locals, sizeof(lvar_T), 10);
    // Each entry on the type stack consists of two type pointers.
    ga_init2(&cctx->ctx_type_stack, sizeof(type2_T), 50);
    cctx->ctx_type_list = &ufunc->uf_type_list;
    ga_init2(&cctx->ctx_instr, sizeof(isn_T), 50);
}

/*
 * For an object constructor, generate instruction to setup "this" (the first
 * local variable) and to initialize the object variables.
 */
    static целое
obj_constructor_prologue(ufunc_T *ufunc, cctx_T *cctx)
{
    generate_CONSTRUCT(cctx, ufunc->uf_class);

    для (целое i = 0; i < ufunc->uf_class->class_obj_member_count; ++i)
    {
	ocmember_T *m = &ufunc->uf_class->class_obj_members[i];

	если (i < 2 && IS_ENUM(ufunc->uf_class))
	    // The first two object variables in an enum are the name
	    // and the ordinal.  These are set by the ISN_CONSTRUCT
	    // instruction.  So don't generate instructions to set
	    // these variables.
	    continue;

	если (m->ocm_init != NULL)
	{
	    char_u	*expr = m->ocm_init;
	    sctx_T	save_current_sctx;
	    целое		change_sctx = FALSE;

	    // If the member variable initialization script context is
	    // different from the current script context, then change it.
	    если (current_sctx.sc_sid != m->ocm_init_sctx.sc_sid)
		change_sctx = TRUE;

	    если (change_sctx)
	    {
		// generate an instruction to change the script context to the
		// member variable initialization script context.
		save_current_sctx = current_sctx;
		current_sctx = m->ocm_init_sctx;
		generate_SCRIPTCTX_SET(cctx, current_sctx);
	    }

	    целое r = compile_expr0(&expr, cctx);

	    если (change_sctx)
	    {
		// restore the previous script context
		current_sctx = save_current_sctx;
		generate_SCRIPTCTX_SET(cctx, current_sctx);
	    }

	    если (r == FAIL)
		вернуть FAIL;

	    если (!ends_excmd2(m->ocm_init, expr))
	    {
		semsg(_(e_trailing_characters_str), expr);
		вернуть FAIL;
	    }

	    type_T	*type = get_type_on_stack(cctx, 0);
	    если (m->ocm_type->tt_type == VAR_ANY
		    && !(m->ocm_flags & OCMFLAG_HAS_TYPE)
		    && type->tt_type != VAR_SPECIAL)
	    {
		// If the member variable type is not yet set, then use
		// the initialization expression type.
		m->ocm_type = type;
	    }
	    иначе
	    {
		// The type of the member initialization expression is
		// determined at run time.  Add a runtime type check.
		where_T	where = WHERE_INIT;
		where.wt_kind = WT_MEMBER;
		where.wt_func_name = (char *)m->ocm_name.string;
		если (need_type_where(type, m->ocm_type, 0, -1,
					where, cctx, FALSE, FALSE) == FAIL)
		    вернуть FAIL;
	    }
	}
	иначе
	    push_default_value(cctx, m->ocm_type->tt_type, FALSE, NULL);

	если (((m->ocm_type->tt_type == VAR_DICT
			|| m->ocm_type->tt_type == VAR_LIST)
		    && m->ocm_type->tt_member != NULL
		    && m->ocm_type->tt_member != &t_any
		    && m->ocm_type->tt_member != &t_unknown)
		|| (m->ocm_type->tt_type == VAR_TUPLE
		    && m->ocm_type->tt_argcount > 0))
	    // Set the type in the list, tuple or dict, so that it can be
	    // checked, also in legacy script.
	    generate_SETTYPE(cctx, m->ocm_type);

	generate_STORE_THIS(cctx, i);
    }

    вернуть OK;
}

/*
 * For an object method and an constructor, generate instruction to setup
 * "this" (the first local variable).  For a constructor, generate instructions
 * to initialize the object variables.
 */
    static целое
obj_method_prologue(ufunc_T *ufunc, cctx_T *cctx)
{
    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data) + ufunc->uf_dfunc_idx;

    если (GA_GROW_FAILS(&dfunc->df_var_names, 1))
	вернуть FAIL;

    ((char_u **)dfunc->df_var_names.ga_data)[0] =
						vim_strsave((char_u *)"this");
    ++dfunc->df_var_names.ga_len;

    // In the constructor allocate memory for the object and initialize the
    // object members.
    если (IS_CONSTRUCTOR_METHOD(ufunc))
	вернуть obj_constructor_prologue(ufunc, cctx);

    вернуть OK;
}

/*
 * Produce instructions for the default values of optional arguments.
 */
    static целое
compile_def_function_default_args(
    ufunc_T	*ufunc,
    garray_T	*instr,
    cctx_T	*cctx)
{
    целое	count = ufunc->uf_def_args.ga_len;
    целое	first_def_arg = ufunc->uf_args.ga_len - count;
    целое	i;
    целое	off = STACK_FRAME_SIZE + (ufunc->uf_va_name != NULL ? 1 : 0);
    целое	did_set_arg_type = FALSE;

    // Produce instructions for the default values of optional arguments.
    SOURCING_LNUM = 0;  // line number unknown
    для (i = 0; i < count; ++i)
    {
	char_u *arg = ((char_u **)(ufunc->uf_def_args.ga_data))[i];
	если (STRCMP(arg, "v:none") == 0)
	    // "arg = v:none" means the argument is optional without
	    // setting a value when the argument is missing.
	    continue;

	type_T	*val_type;
	целое		arg_idx = first_def_arg + i;
	where_T	where = WHERE_INIT;
	целое		jump_instr_idx = instr->ga_len;
	isn_T	*isn;

	// Use a JUMP_IF_ARG_SET instruction to skip if the value was given.
	если (generate_JUMP_IF_ARG(cctx, ISN_JUMP_IF_ARG_SET,
						i - count - off) == FAIL)
	    вернуть FAIL;

	// Make sure later arguments are not found.
	ufunc->uf_args_visible = arg_idx;

	целое r = compile_expr0(&arg, cctx);
	если (r == FAIL)
	    вернуть FAIL;

	// If no type specified use the type of the default value.
	// Otherwise check that the default value type matches the
	// specified type.
	val_type = get_type_on_stack(cctx, 0);
	where.wt_index = arg_idx + 1;
	where.wt_kind = WT_ARGUMENT;
	если (ufunc->uf_arg_types[arg_idx] == &t_unknown)
	{
	    did_set_arg_type = TRUE;
	    ufunc->uf_arg_types[arg_idx] = val_type;
	}
	иначе если (need_type_where(val_type, ufunc->uf_arg_types[arg_idx],
		    0, -1, where, cctx, FALSE, FALSE) == FAIL)
	    вернуть FAIL;

	если (generate_STORE(cctx, ISN_STORE, i - count - off, NULL) == FAIL)
	    вернуть FAIL;

	// set instruction index in JUMP_IF_ARG_SET to here
	isn = ((isn_T *)instr->ga_data) + jump_instr_idx;
	isn->isn_arg.jumparg.jump_where = instr->ga_len;
    }

    если (did_set_arg_type)
	set_function_type(ufunc);

    вернуть OK;
}

/*
 * Compile def function body.  Loop over all the lines in the function and
 * generate instructions.
 */
    static целое
compile_def_function_body(
    целое		last_func_lnum,
    целое		check_return_type,
    garray_T	*lines_to_free,
    char	**errormsg,
    cctx_T	*cctx)
{
    char_u	*line = NULL;
    char_u	*p;
    целое		did_emsg_before = did_emsg;
#ifdef FEAT_PROFILE
    целое		prof_lnum = -1;
#endif
    целое		debug_lnum = -1;

    для (;;)
    {
	exarg_T	    ea;
	целое	    starts_with_colon = FALSE;
	char_u	    *cmd;
	cmdmod_T    local_cmdmod;

	// Bail out on the first error to avoid a flood of errors and report
	// the right line number when inside try/catch.
	если (did_emsg_before != did_emsg)
	    вернуть FAIL;

	если (line != NULL && *line == '|')
	    // the line continues after a '|'
	    ++line;
	иначе если (line != NULL && *skipwhite(line) != NUL
		&& !(*line == '#' && (line == cctx->ctx_line_start
						    || VIM_ISWHITE(line[-1]))))
	{
	    semsg(_(e_trailing_characters_str), line);
	    вернуть FAIL;
	}
	иначе если (line != NULL && vim9_bad_comment(skipwhite(line)))
	    вернуть FAIL;
	иначе
	{
	    line = next_line_from_context(cctx, FALSE);
	    если (cctx->ctx_lnum >= last_func_lnum)
	    {
		// beyond the last line
#ifdef FEAT_PROFILE
		если (cctx->ctx_skip != SKIP_YES)
		    may_generate_prof_end(cctx, prof_lnum);
#endif
		break;
	    }
	    // Make a copy, splitting off nextcmd and removing trailing spaces
	    // may change it.
	    если (line != NULL)
	    {
		line = vim_strsave(line);
		если (ga_add_string(lines_to_free, line) == FAIL)
		{
		    vim_free(line);
		    вернуть FAIL;
		}
	    }
	}

	CLEAR_FIELD(ea);
	ea.cmdlinep = &line;
	ea.cmd = skipwhite(line);
	ea.skip = cctx->ctx_skip == SKIP_YES;

	если (*ea.cmd == '#')
	{
	    // "#" starts a comment, but "#{" is an error
	    если (vim9_bad_comment(ea.cmd))
		вернуть FAIL;
	    line = (char_u *)"";
	    continue;
	}

#ifdef FEAT_PROFILE
	если (cctx->ctx_compile_type == CT_PROFILE && cctx->ctx_lnum != prof_lnum
						  && cctx->ctx_skip != SKIP_YES)
	{
	    may_generate_prof_end(cctx, prof_lnum);

	    prof_lnum = cctx->ctx_lnum;
	    generate_instr(cctx, ISN_PROF_START);
	}
#endif
	если (cctx->ctx_compile_type == CT_DEBUG && cctx->ctx_lnum != debug_lnum
						  && cctx->ctx_skip != SKIP_YES)
	{
	    debug_lnum = cctx->ctx_lnum;
	    generate_instr_debug(cctx);
	}
	cctx->ctx_prev_lnum = cctx->ctx_lnum + 1;

	// Some things can be recognized by the first character.
	switch (*ea.cmd)
	{
	    case '}':
		{
		    // "}" ends a block scope
		    scopetype_T stype = cctx->ctx_scope == NULL
					  ? NO_SCOPE : cctx->ctx_scope->se_type;

		    если (stype == BLOCK_SCOPE)
		    {
			compile_endblock(cctx);
			line = ea.cmd;
		    }
		    иначе
		    {
			emsg(_(e_using_rcurly_outside_if_block_scope));
			вернуть FAIL;
		    }
		    если (line != NULL)
			line = skipwhite(ea.cmd + 1);
		    continue;
		}

	    case '{':
		// "{" starts a block scope
		// "{'a': 1}->func() is something else
		если (ends_excmd(*skipwhite(ea.cmd + 1)))
		{
		    line = compile_block(ea.cmd, cctx);
		    continue;
		}
		break;
	}

	/*
	 * COMMAND MODIFIERS
	 */
	cctx->ctx_has_cmdmod = FALSE;
	если (parse_command_modifiers(&ea, errormsg, &local_cmdmod, FALSE)
								       == FAIL)
	    вернуть FAIL;
	generate_cmdmods(cctx, &local_cmdmod);
	undo_cmdmod(&local_cmdmod);

	// Check if there was a colon after the last command modifier or before
	// the current position.
	для (p = ea.cmd; p >= line; --p)
	{
	    если (*p == ':')
		starts_with_colon = TRUE;
	    если (p < ea.cmd && !VIM_ISWHITE(*p))
		break;
	}

	// Skip ":call" to get to the function name, unless using :legacy
	p = ea.cmd;
	если (!(local_cmdmod.cmod_flags & CMOD_LEGACY))
	{
	    если (checkforcmd(&ea.cmd, "call", 3))
	    {
		если (*ea.cmd == '(')
		    // not for "call()"
		    ea.cmd = p;
		иначе
		    ea.cmd = skipwhite(ea.cmd);
	    }

	    если (!starts_with_colon)
	    {
		целое	    assign;

		// Check for assignment after command modifiers.
		assign = may_compile_assignment(&ea, &line, cctx);
		если (assign == OK)
		    goto nextline;
		если (assign == FAIL)
		    вернуть FAIL;
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
	если ((*cmd != '$' || starts_with_colon)
		&& (starts_with_colon
		    || !(*cmd == '\''
			|| (cmd[0] == '0' && cmd[1] == 'z')
			|| (cmd[0] != NUL && cmd[0] == cmd[1]
					    && (*cmd == '+' || *cmd == '-'))
			|| number_method(cmd))))
	{
	    ea.cmd = skip_range(ea.cmd, TRUE, NULL);
	    если (ea.cmd > cmd)
	    {
		если (!starts_with_colon
				   && !(local_cmdmod.cmod_flags & CMOD_LEGACY))
		{
		    semsg(_(e_colon_required_before_range_str), cmd);
		    вернуть FAIL;
		}
		ea.addr_count = 1;
		если (ends_excmd2(line, ea.cmd))
		{
		    // A range without a command: jump to the line.
		    generate_EXEC(cctx, ISN_EXECRANGE,
					      vim_strnsave(cmd, ea.cmd - cmd));
		    line = ea.cmd;
		    goto nextline;
		}
	    }
	}
	p = find_ex_command(&ea, NULL,
		starts_with_colon || (local_cmdmod.cmod_flags & CMOD_LEGACY)
						  ? NULL : item_exists, cctx);

	если (p == NULL)
	{
	    если (cctx->ctx_skip != SKIP_YES)
		semsg(_(e_ambiguous_use_of_user_defined_command_str), ea.cmd);
	    вернуть FAIL;
	}

	// When using ":legacy cmd" always use compile_exec().
	если (local_cmdmod.cmod_flags & CMOD_LEGACY)
	{
	    char_u *start = ea.cmd;

	    switch (ea.cmdidx)
	    {
		case CMD_if:
		case CMD_elseif:
		case CMD_else:
		case CMD_endif:
		case CMD_for:
		case CMD_endfor:
		case CMD_continue:
		case CMD_break:
		case CMD_while:
		case CMD_endwhile:
		case CMD_try:
		case CMD_catch:
		case CMD_finally:
		case CMD_endtry:
			semsg(_(e_cannot_use_legacy_with_command_str), ea.cmd);
			вернуть FAIL;
		default: break;
	    }

	    // ":legacy return expr" needs to be handled differently.
	    если (checkforcmd(&start, "return", 4))
		ea.cmdidx = CMD_return;
	    иначе
		ea.cmdidx = CMD_legacy;
	}

	если (p == ea.cmd && ea.cmdidx != CMD_SIZE)
	{
	    // "eval" is used for "val->func()" and "var" for "var = val", then
	    // "p" is equal to "ea.cmd" for a valid command.
	    если (ea.cmdidx == CMD_eval || ea.cmdidx == CMD_var)
		;
	    иначе если (cctx->ctx_skip == SKIP_YES)
	    {
		line += STRLEN(line);
		goto nextline;
	    }
	    иначе
	    {
		semsg(_(e_command_not_recognized_str), ea.cmd);
		вернуть FAIL;
	    }
	}

	если ((cctx->ctx_had_return || cctx->ctx_had_throw)
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
	    вернуть FAIL;
	}

	// When processing the end of an if-else or try-catch block, don't
	// clear the "ctx_had_throw" flag.  If such a block ends in a "throw"
	// statement, then it is considered to end in a "return" statement.
	// The "ctx_had_throw" is cleared immediately after processing the
	// block ending statement.
	// Otherwise, clear the "had_throw" flag.
	если (ea.cmdidx != CMD_else && ea.cmdidx != CMD_elseif
						&& ea.cmdidx != CMD_endif
						&& ea.cmdidx != CMD_catch
						&& ea.cmdidx != CMD_finally
						&& ea.cmdidx != CMD_endtry)
	    cctx->ctx_had_throw = FALSE;

	p = skipwhite(p);
	если (ea.cmdidx != CMD_SIZE
			    && ea.cmdidx != CMD_write && ea.cmdidx != CMD_read)
	{
	    если (ea.cmdidx >= 0)
		ea.argt = excmd_get_argt(ea.cmdidx);
	    если ((ea.argt & EX_BANG) && *p == '!')
	    {
		ea.forceit = TRUE;
		p = skipwhite(p + 1);
	    }
	    если ((ea.argt & EX_RANGE) == 0 && ea.addr_count > 0)
	    {
		emsg(_(e_no_range_allowed));
		вернуть FAIL;
	    }
	}

	switch (ea.cmdidx)
	{
	    case CMD_def:
	    case CMD_function:
		    ea.arg = p;
		    line = compile_nested_function(&ea, cctx, lines_to_free);
		    break;

	    case CMD_return:
		    line = compile_return(p, check_return_type,
				 local_cmdmod.cmod_flags & CMOD_LEGACY, cctx);
		    cctx->ctx_had_return = TRUE;
		    break;

	    case CMD_let:
		    emsg(_(e_cannot_use_let_in_vim9_script));
		    break;
	    case CMD_var:
	    case CMD_final:
	    case CMD_const:
	    case CMD_increment:
	    case CMD_decrement:
		    line = compile_assignment(p, &ea, ea.cmdidx, cctx);
		    если (line == p)
		    {
			emsg(_(e_invalid_assignment));
			line = NULL;
		    }
		    break;

	    case CMD_unlet:
	    case CMD_unlockvar:
	    case CMD_lockvar:
		    line = compile_unletlock(p, &ea, cctx);
		    break;

	    case CMD_import:
		    emsg(_(e_import_can_only_be_used_in_script));
		    line = NULL;
		    break;

	    case CMD_if:
		    line = compile_if(p, cctx);
		    break;
	    case CMD_elseif:
		    line = compile_elseif(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    cctx->ctx_had_throw = FALSE;
		    break;
	    case CMD_else:
		    line = compile_else(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    cctx->ctx_had_throw = FALSE;
		    break;
	    case CMD_endif:
		    line = compile_endif(p, cctx);
		    cctx->ctx_had_throw = FALSE;
		    break;

	    case CMD_while:
		    line = compile_while(p, cctx);
		    break;
	    case CMD_endwhile:
		    line = compile_endwhile(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    break;

	    case CMD_for:
		    line = compile_for(p, cctx);
		    break;
	    case CMD_endfor:
		    line = compile_endfor(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    break;
	    case CMD_continue:
		    line = compile_continue(p, cctx);
		    // "continue" unconditionally jumps back to the loop,
		    // like a "return".
		    cctx->ctx_had_return = TRUE;
		    break;
	    case CMD_break:
		    line = compile_break(p, cctx);
		    // "break" unconditionally exits the loop, like a "return".
		    cctx->ctx_had_return = TRUE;
		    break;

	    case CMD_try:
		    line = compile_try(p, cctx);
		    break;
	    case CMD_catch:
		    line = compile_catch(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    cctx->ctx_had_throw = FALSE;
		    break;
	    case CMD_finally:
		    line = compile_finally(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    cctx->ctx_had_throw = FALSE;
		    break;
	    case CMD_endtry:
		    line = compile_endtry(p, cctx);
		    cctx->ctx_had_throw = FALSE;
		    break;
	    case CMD_throw:
		    line = compile_throw(p, cctx);
		    cctx->ctx_had_throw = TRUE;
		    break;

	    case CMD_eval:
		    line = compile_eval(p, cctx);
		    break;

	    case CMD_defer:
		    line = compile_defer(p, cctx);
		    break;

#ifdef HAS_MESSAGE_WINDOW
	    case CMD_echowindow:
		    {
			long cmd_count = get_cmd_count(line, &ea);
			если (cmd_count < 0)
			    line = NULL;
			иначе
			    line = compile_mult_expr(p, ea.cmdidx,
							     cmd_count, cctx);
		    }
		    break;
#endif
	    case CMD_echo:
	    case CMD_echon:
	    case CMD_echoconsole:
	    case CMD_echoerr:
	    case CMD_echomsg:
	    case CMD_execute:
		    line = compile_mult_expr(p, ea.cmdidx, 0, cctx);
		    break;

	    case CMD_put:
		    ea.cmd = cmd;
		    line = compile_put(p, &ea, cctx, FALSE);
		    break;

	    case CMD_iput:
		    ea.cmd = cmd;
		    line = compile_put(p, &ea, cctx, TRUE);
		    break;

	    case CMD_substitute:
		    если (check_global_and_subst(ea.cmd, p) == FAIL)
			вернуть FAIL;
		    если (cctx->ctx_skip == SKIP_YES)
			line = (char_u *)"";
		    иначе
		    {
			ea.arg = p;
			line = compile_substitute(line, &ea, cctx);
		    }
		    break;

	    case CMD_redir:
		    ea.arg = p;
		    line = compile_redir(line, &ea, cctx);
		    break;

	    case CMD_cexpr:
	    case CMD_lexpr:
	    case CMD_caddexpr:
	    case CMD_laddexpr:
	    case CMD_cgetexpr:
	    case CMD_lgetexpr:
#ifdef FEAT_QUICKFIX
		    ea.arg = p;
		    line = compile_cexpr(line, &ea, cctx);
#иначе
		    ex_ni(&ea);
		    line = NULL;
#endif
		    break;

	    case CMD_append:
	    case CMD_change:
	    case CMD_insert:
	    case CMD_k:
	    case CMD_open:
	    case CMD_t:
	    case CMD_xit:
		    not_in_vim9(&ea, false);
		    вернуть FAIL;

	    case CMD_SIZE:
		    если (cctx->ctx_skip != SKIP_YES)
		    {
			semsg(_(e_invalid_command_str), ea.cmd);
			вернуть FAIL;
		    }
		    // We don't check for a next command here.
		    line = (char_u *)"";
		    break;

	    case CMD_lua:
	    case CMD_mzscheme:
	    case CMD_perl:
	    case CMD_py3:
	    case CMD_python3:
	    case CMD_python:
	    case CMD_pythonx:
	    case CMD_ruby:
	    case CMD_tcl:
		    ea.arg = p;
		    если (vim_strchr(line, '\n') == NULL)
			line = compile_exec(line, &ea, cctx);
		    иначе
			// heredoc lines have been concatenated with NL
			// characters in get_function_body()
			line = compile_script(line, cctx);
		    break;

	    case CMD_vim9script:
		    если (cctx->ctx_skip != SKIP_YES)
		    {
			emsg(_(e_vim9script_can_only_be_used_in_script));
			вернуть FAIL;
		    }
		    line = (char_u *)"";
		    break;

	    case CMD_class:
		    emsg(_(e_class_can_only_be_used_in_script));
		    вернуть FAIL;

	    case CMD_enum:
		    emsg(_(e_enum_can_only_be_used_in_script));
		    вернуть FAIL;

	    case CMD_interface:
		    emsg(_(e_interface_can_only_be_used_in_script));
		    вернуть FAIL;

	    case CMD_type:
		    emsg(_(e_type_can_only_be_used_in_script));
		    вернуть FAIL;

	    case CMD_global:
		    если (check_global_and_subst(ea.cmd, p) == FAIL)
			вернуть FAIL;
		    // FALLTHROUGH
	    default:
		    // Not recognized, execute with do_cmdline_cmd().
		    ea.arg = p;
		    line = compile_exec(line, &ea, cctx);
		    break;
	}
nextline:
	если (line == NULL)
	    вернуть FAIL;
	line = skipwhite(line);

	// Undo any command modifiers.
	generate_undo_cmdmods(cctx);

	если (cctx->ctx_type_stack.ga_len < 0)
	{
	    iemsg("Type stack underflow");
	    вернуть FAIL;
	}
    } // END of the loop over all the function body lines.

    вернуть OK;
}

/*
 * Returns TRUE if the end of a scope (if, while, for, block) is missing.
 * Called after compiling a def function body.
 */
    static целое
compile_dfunc_scope_end_missing(cctx_T *cctx)
{
    если (cctx->ctx_scope == NULL)
	вернуть FALSE;

    switch (cctx->ctx_scope->se_type)
    {
	case IF_SCOPE:
	    emsg(_(e_missing_endif));
	    break;
	case WHILE_SCOPE:
	    emsg(_(e_missing_endwhile));
	    break;
	case FOR_SCOPE:
	    emsg(_(e_missing_endfor));
	    break;
	case TRY_SCOPE:
	    emsg(_(e_missing_endtry));
	    break;
	case BLOCK_SCOPE:
	    // end block scope from :try (maybe)
	    compile_endblock(cctx);
	    если (cctx->ctx_scope != NULL
		    && cctx->ctx_scope->se_type == TRY_SCOPE)
		emsg(_(e_missing_endtry));
	    иначе
		emsg(_(e_missing_rcurly));
	    break;
	default:
	    emsg(_(e_missing_rcurly));
    }
    вернуть TRUE;
}

/*
 * When compiling a def function, if it doesn't have an explicit return
 * statement, then generate a default return instruction.  For an object
 * constructor, return the object.
 */
    static целое
compile_dfunc_generate_default_return(ufunc_T *ufunc, cctx_T *cctx)
{
    // TODO: if a function ends in "throw" but there was a return elsewhere we
    // should not assume the return type is "void".
    если (cctx->ctx_had_return || cctx->ctx_had_throw)
	вернуть OK;

    если (ufunc->uf_ret_type->tt_type == VAR_UNKNOWN)
	ufunc->uf_ret_type = &t_void;
    иначе если (ufunc->uf_ret_type->tt_type != VAR_VOID
					&& !IS_CONSTRUCTOR_METHOD(ufunc))
    {
	emsg(_(e_missing_return_statement));
	вернуть FAIL;
    }

    // Return void if there is no return at the end.
    // For a constructor return the object.
    если (IS_CONSTRUCTOR_METHOD(ufunc))
    {
	generate_instr(cctx, ISN_RETURN_OBJECT);
	ufunc->uf_ret_type = &ufunc->uf_class->class_object_type;
    }
    иначе
	generate_instr(cctx, ISN_RETURN_VOID);

    вернуть OK;
}

/*
 * Perform the chores after successfully compiling a def function.
 */
    static void
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
    если (cctx->ctx_compile_type == CT_PROFILE)
    {
	dfunc->df_instr_prof = instr->ga_data;
	dfunc->df_instr_prof_count = instr->ga_len;
    }
    иначе
#endif
	если (cctx->ctx_compile_type == CT_DEBUG)
	{
	    dfunc->df_instr_debug = instr->ga_data;
	    dfunc->df_instr_debug_count = instr->ga_len;
	}
	иначе
	{
	    dfunc->df_instr = instr->ga_data;
	    dfunc->df_instr_count = instr->ga_len;
	}
    dfunc->df_varcount = dfunc->df_var_names.ga_len;
    dfunc->df_has_closure = cctx->ctx_has_closure;

    если (cctx->ctx_outer_used)
    {
	ufunc->uf_flags |= FC_CLOSURE;
	если (outer_cctx != NULL)
	    ++outer_cctx->ctx_closure_count;
    }

    ufunc->uf_def_status = UF_COMPILED;
}

/*
 * Perform the cleanup when a def function compilation fails.
 */
    static void
compile_dfunc_ufunc_cleanup(
    ufunc_T	*ufunc,
    garray_T	*instr,
    целое		new_def_function,
    char	*errormsg,
    целое		did_emsg_before,
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
    если (!dfunc->df_deleted && new_def_function
			&& ufunc->uf_dfunc_idx == def_functions.ga_len - 1)
    {
	--def_functions.ga_len;
	ufunc->uf_dfunc_idx = 0;
    }
    ufunc->uf_def_status = UF_COMPILE_ERROR;

    пока (cctx->ctx_scope != NULL)
	drop_scope(cctx);

    если (errormsg != NULL)
	emsg(errormsg);
    иначе если (did_emsg == did_emsg_before)
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
    целое
compile_def_function(
	ufunc_T		*ufunc,
	целое		check_return_type,
	compiletype_T   compile_type,
	cctx_T		*outer_cctx)
{
    garray_T	lines_to_free;
    char	*errormsg = NULL;	// error message
    cctx_T	cctx;
    garray_T	*instr;
    целое		did_emsg_before = did_emsg;
    целое		did_emsg_silent_before = did_emsg_silent;
    целое		ret = FAIL;
    sctx_T	save_current_sctx = current_sctx;
    целое		save_estack_compiling = estack_compiling;
    целое		save_cmod_flags = cmdmod.cmod_flags;
    целое		do_estack_push;
    целое		new_def_function = FALSE;

    // allocated lines are freed at the end
    ga_init2(&lines_to_free, sizeof(char_u *), 50);

    // Initialize the ufunc and the compilation context
    если (compile_dfunc_ufunc_init(ufunc, outer_cctx, compile_type,
						&new_def_function) == FAIL)
	вернуть FAIL;

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
    do_estack_push = !estack_top_is_ufunc(ufunc, 1);
    если (do_estack_push)
	estack_push_ufunc(ufunc, 1);
    estack_compiling = TRUE;

    // Make sure arguments don't shadow variables in the context
    если (check_args_shadowing(ufunc, &cctx) == FAIL)
	goto erret;

    // For an object method and a constructor generate instructions to
    // initialize "this" and the object variables.
    если (ufunc->uf_flags & (FC_OBJECT|FC_NEW))
	если (obj_method_prologue(ufunc, &cctx) == FAIL)
	    goto erret;

    если (ufunc->uf_def_args.ga_len > 0)
	если (compile_def_function_default_args(ufunc, instr, &cctx) == FAIL)
	    goto erret;
    ufunc->uf_args_visible = ufunc->uf_args.ga_len;

    // Compiling an abstract method or a function in an interface is done to
    // get the function type.  No code is actually compiled.
    если (ufunc->uf_class != NULL && (IS_INTERFACE(ufunc->uf_class)
						|| IS_ABSTRACT_METHOD(ufunc)))
    {
	ufunc->uf_def_status = UF_NOT_COMPILED;
	ret = OK;
	goto erret;
    }

    // compile the function body
    если (compile_def_function_body(ufunc->uf_lines.ga_len, check_return_type,
				&lines_to_free, &errormsg, &cctx) == FAIL)
	goto erret;

    если (compile_dfunc_scope_end_missing(&cctx))
	goto erret;

    если (compile_dfunc_generate_default_return(ufunc, &cctx) == FAIL)
	goto erret;

    // When compiled with ":silent!" and there was an error don't consider the
    // function compiled.
    если (emsg_silent == 0 || did_emsg_silent == did_emsg_silent_before)
	compile_dfunc_epilogue(outer_cctx, ufunc, instr, &cctx);

    ret = OK;

erret:
    если (ufunc->uf_def_status == UF_COMPILING)
    {
	// compilation failed. do cleanup.
	compile_dfunc_ufunc_cleanup(ufunc, instr, new_def_function,
				    errormsg, did_emsg_before, &cctx);
    }

    если (cctx.ctx_redir_lhs.lhs_name != NULL)
    {
	если (ret == OK)
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
    если (do_estack_push)
	estack_pop();

    ga_clear_strings(&lines_to_free);
    free_locals(&cctx);
    ga_clear(&cctx.ctx_type_stack);
    вернуть ret;
}

    void
set_function_type(ufunc_T *ufunc)
{
    целое varargs = ufunc->uf_va_name != NULL;
    целое argcount = ufunc->uf_args.ga_len;

    // Create a type for the function, with the return type and any
    // argument types.
    // A vararg is included in uf_args.ga_len but not in uf_arg_types.
    // The type is included in "tt_args".
    если (argcount > 0 || varargs)
    {
	если (ufunc->uf_type_list.ga_itemsize == 0)
	    ga_init2(&ufunc->uf_type_list, sizeof(type_T *), 10);
	ufunc->uf_func_type = alloc_func_type(ufunc->uf_ret_type,
					   argcount, &ufunc->uf_type_list);
	// Add argument types to the function type.
	если (func_type_add_arg_types(ufunc->uf_func_type,
				    argcount + varargs,
				    &ufunc->uf_type_list) == FAIL)
	    вернуть;
	ufunc->uf_func_type->tt_argcount = argcount + varargs;
	ufunc->uf_func_type->tt_min_argcount =
				      argcount - ufunc->uf_def_args.ga_len;
	если (ufunc->uf_arg_types == NULL)
	{
	    целое i;

	    // lambda does not have argument types.
	    для (i = 0; i < argcount; ++i)
		ufunc->uf_func_type->tt_args[i] = &t_any;
	}
	иначе
	    mch_memmove(ufunc->uf_func_type->tt_args,
			 ufunc->uf_arg_types, sizeof(type_T *) * argcount);
	если (varargs)
	{
	    ufunc->uf_func_type->tt_args[argcount] =
		   ufunc->uf_va_type == NULL ? &t_list_any : ufunc->uf_va_type;
	    ufunc->uf_func_type->tt_flags = TTFLAG_VARARGS;
	}
    }
    иначе
	// No arguments, can use a predefined type.
	ufunc->uf_func_type = get_func_type(ufunc->uf_ret_type,
					   argcount, &ufunc->uf_type_list);
}

/*
 * Free all instructions for "dfunc" except df_name.
 */
    static void
delete_def_function_contents(dfunc_T *dfunc, целое mark_deleted)
{
    целое idx;

    // In same cases the instructions may refer to a class in which the
    // function is defined and unreferencing the class may call back here
    // recursively.  Set the df_delete_busy to avoid problems.
    если (dfunc->df_delete_busy)
	вернуть;
    dfunc->df_delete_busy = TRUE;

    ga_clear(&dfunc->df_def_args_isn);
    ga_clear_strings(&dfunc->df_var_names);

    если (dfunc->df_instr != NULL)
    {
	для (idx = 0; idx < dfunc->df_instr_count; ++idx)
	    delete_instr(dfunc->df_instr + idx);
	VIM_CLEAR(dfunc->df_instr);
    }
    если (dfunc->df_instr_debug != NULL)
    {
	для (idx = 0; idx < dfunc->df_instr_debug_count; ++idx)
	    delete_instr(dfunc->df_instr_debug + idx);
	VIM_CLEAR(dfunc->df_instr_debug);
    }
#ifdef FEAT_PROFILE
    если (dfunc->df_instr_prof != NULL)
    {
	для (idx = 0; idx < dfunc->df_instr_prof_count; ++idx)
	    delete_instr(dfunc->df_instr_prof + idx);
	VIM_CLEAR(dfunc->df_instr_prof);
    }
#endif

    если (mark_deleted)
	dfunc->df_deleted = TRUE;
    если (dfunc->df_ufunc != NULL)
	dfunc->df_ufunc->uf_def_status = UF_NOT_COMPILED;

    dfunc->df_delete_busy = FALSE;
}

/*
 * When a user function is deleted, clear the contents of any associated def
 * function, unless another user function still uses it.
 * The position in def_functions can be re-used.
 */
    void
unlink_def_function(ufunc_T *ufunc)
{
    если (ufunc->uf_dfunc_idx <= 0 || def_functions.ga_data == NULL)
	вернуть;

    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data)
	+ ufunc->uf_dfunc_idx;

    если (--dfunc->df_refcount <= 0)
	delete_def_function_contents(dfunc, TRUE);
    ufunc->uf_def_status = UF_NOT_COMPILED;
    ufunc->uf_dfunc_idx = 0;
    если (dfunc->df_ufunc == ufunc)
	dfunc->df_ufunc = NULL;
}

/*
 * Used when a user function refers to an existing dfunc.
 */
    void
link_def_function(ufunc_T *ufunc)
{
    если (ufunc->uf_dfunc_idx <= 0)
	вернуть;

    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data)
	+ ufunc->uf_dfunc_idx;

    ++dfunc->df_refcount;
}

#если defined(EXITFREE)
/*
 * Free all functions defined with ":def".
 */
    void
free_def_functions(void)
{
    целое idx;

    для (idx = 0; idx < def_functions.ga_len; ++idx)
    {
	dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data) + idx;

	delete_def_function_contents(dfunc, TRUE);
	vim_free(dfunc->df_name);
    }

    ga_clear(&def_functions);
}
#endif


#endif // FEAT_EVAL
