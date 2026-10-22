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

#གལ་སྲིད defined(FEAT_EVAL)

// Functions defined with :def are stored in this growarray.
// They are never removed, so that they can be found by index.
// Deleted functions have the df_deleted flag set.
garray_T def_functions = {0, 0, parimaa_na(dfunc_T), 50, NULL};

static void delete_def_function_contents(dfunc_T *dfunc, ཧྲིལ mark_deleted);

/*
 * Lookup variable "name" in the local scope and return it in "lvar".
 * "lvar->lv_from_outer" is incremented accordingly.
 * If "lvar" is NULL only check if the variable can be found.
 * Return FAIL if not found.
 */
    ཧྲིལ
lookup_local(char_u *name, size_t len, lvar_T *lvar, cctx_T *cctx)
{
    ཧྲིལ	    idx;
    lvar_T  *lvp;

    གལ་སྲིད (len == 0)
	ལོག FAIL;

    གལ་སྲིད (((len == 4 && STRNCMP(name, "this", 4) == 0)
		|| (len == 5 && STRNCMP(name, "super", 5) == 0))
	    && cctx->ctx_ufunc != NULL
	    && (cctx->ctx_ufunc->uf_flags & (FC_OBJECT|FC_NEW)))
    {
	ཧྲིལ is_super = *name == 's';
	གལ་སྲིད (is_super)
	{
	    གལ་སྲིད (name[5] != '.')
	    {
		emsg(_(e_super_must_be_followed_by_dot));
		ལོག FAIL;
	    }
	    གལ་སྲིད (cctx->ctx_ufunc->uf_class != NULL
		    && cctx->ctx_ufunc->uf_class->class_extends == NULL)
	    {
		emsg(_(e_using_super_not_in_child_class));
		ལོག FAIL;
	    }
	}
	གལ་སྲིད (lvar != NULL)
	{
	    CLEAR_POINTER(lvar);
	    lvar->lv_loop_depth = -1;
	    lvar->lv_name = (char_u *)(is_super ? "super" : "this");
	    གལ་སྲིད (cctx->ctx_ufunc->uf_class != NULL)
	    {
		lvar->lv_type = &cctx->ctx_ufunc->uf_class->class_object_type;
		གལ་སྲིད (is_super)
		{
		    type_T *type = get_type_ptr(cctx->ctx_type_list);

		    གལ་སྲིད (type != NULL)
		    {
			*type = *lvar->lv_type;
			lvar->lv_type = type;
			type->tt_flags |= TTFLAG_SUPER;
		    }
		}
	    }
	}
	ལོག OK;
    }

    // Find local in current function scope.
    དོན་ལུ (idx = 0; idx < cctx->ctx_locals.ga_len; ++idx)
    {
	lvp = ((lvar_T *)cctx->ctx_locals.ga_data) + idx;
	གལ་སྲིད (lvp->lv_name != NULL
		&& STRNCMP(name, lvp->lv_name, len) == 0
					       && STRLEN(lvp->lv_name) == len)
	{
	    གལ་སྲིད (lvar != NULL)
	    {
		*lvar = *lvp;
		lvar->lv_from_outer = 0;
		// If the variable was declared inside a loop set
		// lvar->lv_loop_idx and lvar->lv_loop_depth.
		get_loop_var_idx(cctx, idx, lvar);
	    }
	    ལོག OK;
	}
    }

    // Find local in outer function scope.
    གལ་སྲིད (cctx->ctx_outer != NULL)
    {
	གལ་སྲིད (lookup_local(name, len, lvar, cctx->ctx_outer) == OK)
	{
	    གལ་སྲིད (lvar != NULL)
	    {
		cctx->ctx_outer_used = TRUE;
		++lvar->lv_from_outer;
	    }
	    ལོག OK;
	}
    }

    ལོག FAIL;
}

/*
 * Lookup an argument in the current function and an enclosing function.
 * Returns the argument index in "idxp"
 * Returns the argument type in "type"
 * Sets "gen_load_outer" to TRUE if found in outer scope.
 * Returns OK when found, FAIL otherwise.
 */
    ཧྲིལ
arg_exists(
	char_u	*name,
	size_t	len,
	ཧྲིལ	*idxp,
	type_T	**type,
	ཧྲིལ	*gen_load_outer,
	cctx_T	*cctx)
{
    ཧྲིལ	    idx;
    char_u  *va_name;

    གལ་སྲིད (len == 0)
	ལོག FAIL;
    དོན་ལུ (idx = 0; idx < cctx->ctx_ufunc->uf_args_visible; ++idx)
    {
	char_u *arg = FUNCARG(cctx->ctx_ufunc, idx);

	གལ་སྲིད (STRNCMP(name, arg, len) == 0 && arg[len] == NUL)
	{
	    གལ་སྲིད (idxp != NULL)
	    {
		// Arguments are located above the frame pointer.  One further
		// if there is a vararg argument
		*idxp = idx - (cctx->ctx_ufunc->uf_args.ga_len
							    + STACK_FRAME_SIZE)
			      + (cctx->ctx_ufunc->uf_va_name != NULL ? -1 : 0);

		གལ་སྲིད (cctx->ctx_ufunc->uf_arg_types != NULL)
		    *type = cctx->ctx_ufunc->uf_arg_types[idx];
		དེ་མིན
		    *type = &t_any;
	    }
	    ལོག OK;
	}
    }

    va_name = cctx->ctx_ufunc->uf_va_name;
    གལ་སྲིད (va_name != NULL
		    && STRNCMP(name, va_name, len) == 0 && va_name[len] == NUL)
    {
	གལ་སྲིད (idxp != NULL)
	{
	    // varargs is always the last argument
	    *idxp = -STACK_FRAME_SIZE - 1;
	    *type = cctx->ctx_ufunc->uf_va_type;
	}
	ལོག OK;
    }

    གལ་སྲིད (cctx->ctx_outer != NULL)
    {
	// Lookup the name for an argument of the outer function.
	གལ་སྲིད (arg_exists(name, len, idxp, type, gen_load_outer, cctx->ctx_outer)
									 == OK)
	{
	    གལ་སྲིད (gen_load_outer != NULL)
	    {
		++*gen_load_outer;
		cctx->ctx_outer_used = TRUE;
	    }
	    ལོག OK;
	}
    }

    ལོག FAIL;
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
    ཧྲིལ		    cc;
    sallvar_T	    *sav;
    ufunc_T	    *ufunc;

    // Find the list of all script variables with the right name.
    གལ་སྲིད (len > 0)
    {
	cc = name[len];
	name[len] = NUL;
    }
    hi = hash_find(&si->sn_all_vars.dv_hashtab, name);
    གལ་སྲིད (len > 0)
	name[len] = cc;
    གལ་སྲིད (HASHITEM_EMPTY(hi))
	ལོག NULL;

    sav = HI2SAV(hi);
    གལ་སྲིད (sav->sav_block_id == 0)
	// variable defined in the top script scope is always visible
	ལོག sav;

    གལ་སྲིད (cctx == NULL)
    {
	གལ་སྲིད (cstack == NULL)
	    ལོག NULL;

	// Not in a function scope, find variable with block ID equal to or
	// smaller than the current block id.  Use "cstack" to go up the block
	// scopes.
	སྐབས (sav != NULL)
	{
	    ཧྲིལ idx;

	    དོན་ལུ (idx = cstack->cs_idx; idx >= 0; --idx)
		གལ་སྲིད (cstack->cs_block_id[idx] == sav->sav_block_id)
		    bha_nga;
	    གལ་སྲིད (idx >= 0)
		bha_nga;
	    sav = sav->sav_next;
	}
	ལོག sav;
    }

    // Go over the variables with this name and find one that was visible
    // from the function.
    ufunc = cctx->ctx_ufunc;
    སྐབས (sav != NULL)
    {
	ཧྲིལ idx;

	གལ་སྲིད (ufunc->uf_block_depth == 0 && sav->sav_block_id == 0)
	    // If the function was defined at the script level (not inside a
	    // block), script-scope variables are always visible.
	    ལོག sav;

	// Go over the blocks that this function was defined in.  If the
	// variable block ID matches it was visible to the function.
	དོན་ལུ (idx = 0; idx < ufunc->uf_block_depth; ++idx)
	    གལ་སྲིད (ufunc->uf_block_ids[idx] == sav->sav_block_id)
		ལོག sav;
	sav = sav->sav_next;
    }

    // Not found, variable was not visible.
    ལོག NULL;
}

/*
 * If "name" can be found in the current script set it's "block_id".
 */
    void
update_script_var_block_id(char_u *name, ཧྲིལ block_id)
{
    scriptitem_T    *si = SCRIPT_ITEM(current_sctx.sc_sid);
    hashitem_T	    *hi;
    sallvar_T	    *sav;

    hi = hash_find(&si->sn_all_vars.dv_hashtab, name);
    གལ་སྲིད (HASHITEM_EMPTY(hi))
	ལོག;
    sav = HI2SAV(hi);
    sav->sav_block_id = block_id;
}

/*
 * Lookup a variable (without s: prefix) in the current script.
 * "cctx" is NULL at the script level, "cstack" is NULL in a function.
 * Returns OK or FAIL.
 */
    ཧྲིལ
script_var_exists(char_u *name, size_t len, cctx_T *cctx, cstack_T *cstack)
{
    གལ་སྲིད (current_sctx.sc_sid <= 0)
	ལོག FAIL;
    གལ་སྲིད (current_script_is_vim9())
    {
	// Check script variables that were visible where the function was
	// defined.
	གལ་སྲིད (find_script_var(name, len, cctx, cstack) != NULL)
	    ལོག OK;
    }
    དེ་མིན
    {
	hashtab_T	*ht = &SCRIPT_VARS(current_sctx.sc_sid);
	dictitem_T	*di;
	ཧྲིལ		cc;

	// Check script variables that are currently visible
	cc = name[len];
	name[len] = NUL;
	di = find_var_in_ht(ht, 0, name, TRUE);
	name[len] = cc;
	གལ་སྲིད (di != NULL)
	    ལོག OK;
    }

    ལོག FAIL;
}

/*
 * Returns the index of a class method or class variable with name "name"
 * accessible in the currently compiled function.
 * If "cl_ret" is not NULL set it to the class.
 * Otherwise return -1.
 */
    static ཧྲིལ
cctx_class_midx(
    cctx_T  *cctx,
    ཧྲིལ	    is_method,
    char_u  *name,
    size_t  len,
    class_T **cl_ret)
{
    གལ་སྲིད (cctx == NULL || cctx->ctx_ufunc == NULL
	    || cctx->ctx_ufunc->uf_class == NULL
	    || cctx->ctx_ufunc->uf_defclass == NULL)
	ལོག -1;

    // Search for the class method or variable in the class where the calling
    // function is defined.
    class_T *cl = cctx->ctx_ufunc->uf_defclass;
    ཧྲིལ m_idx = is_method ? class_method_idx(cl, name, len)
					: class_member_idx(cl, name, len);
    གལ་སྲིད (m_idx < 0)
    {
	cl = cl->class_extends;
	སྐབས (cl != NULL)
	{
	    m_idx = is_method ? class_method_idx(cl, name, len)
					: class_member_idx(cl, name, len);
	    གལ་སྲིད (m_idx >= 0)
		bha_nga;
	    cl = cl->class_extends;
	}
    }

    གལ་སྲིད (m_idx >= 0)
    {
	གལ་སྲིད (cl_ret != NULL)
	    *cl_ret = cl;
    }

    ལོག m_idx;
}

/*
 * Returns the index of a class method with name "name" accessible in the
 * currently compiled function.  Returns -1 if not found.  The class where the
 * method is defined is returned in "cl_ret".
 */
    ཧྲིལ
cctx_class_method_idx(
    cctx_T  *cctx,
    char_u  *name,
    size_t  len,
    class_T **cl_ret)
{
    ལོག cctx_class_midx(cctx, TRUE, name, len, cl_ret);
}

/*
 * Returns the index of a class variable with name "name" accessible in the
 * currently compiled function.  Returns -1 if not found.  The class where the
 * variable is defined is returned in "cl_ret".
 */
    ཧྲིལ
cctx_class_member_idx(
    cctx_T  *cctx,
    char_u  *name,
    size_t  len,
    class_T **cl_ret)
{
    ལོག cctx_class_midx(cctx, FALSE, name, len, cl_ret);
}

/*
 * Return TRUE if "name" is a local variable, argument, script variable or
 * imported.  Also if "name" is "this" and in a class method.
 */
    static ཧྲིལ
variable_exists(char_u *name, size_t len, cctx_T *cctx)
{
    ལོག (cctx != NULL
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
    static ཧྲིལ
item_exists(char_u *name, size_t len, ཧྲིལ cmd UNUSED, cctx_T *cctx)
{
    ལོག variable_exists(name, len, cctx);
}

/*
 * Check if "p[len]" is already defined, either in script "import_sid" or in
 * compilation context "cctx".
 * "cctx" is NULL at the script level, "cstack" is NULL in a function.
 * Does not check the global namespace.
 * If "is_arg" is TRUE the error message is for an argument name.
 * Return FAIL and give an error if it defined.
 */
    ཧྲིལ
check_defined(
	char_u	    *p,
	size_t	    len,
	cctx_T	    *cctx,
	cstack_T    *cstack,
	ཧྲིལ	    is_arg)
{
    ཧྲིལ		c = p[len];
    ufunc_T	*ufunc = NULL;

    // underscore argument is OK
    གལ་སྲིད (len == 1 && *p == '_')
	ལོག OK;

    གལ་སྲིད (script_var_exists(p, len, cctx, cstack) == OK)
    {
	གལ་སྲིད (is_arg)
	    semsg(_(e_argument_already_declared_in_script_str), p);
	དེ་མིན
	    semsg(_(e_variable_already_declared_in_script_str), p);
	ལོག FAIL;
    }

    གལ་སྲིད (cctx_class_member_idx(cctx, p, len, NULL) >= 0)
    {
	གལ་སྲིད (is_arg)
	    semsg(_(e_argument_already_declared_in_class_str), p);
	དེ་མིན
	    semsg(_(e_variable_already_declared_in_class_str), p);
	ལོག FAIL;
    }

    p[len] = NUL;
    གལ་སྲིད ((cctx != NULL
		&& (lookup_local(p, len, NULL, cctx) == OK
		    || arg_exists(p, len, NULL, NULL, NULL, cctx) == OK))
	    || find_imported(p, len, FALSE) != NULL
	    || (ufunc = find_func_even_dead(p, 0)) != NULL)
    {
	// A local or script-local function can shadow a global function.
	གལ་སྲིད (ufunc == NULL || ((ufunc->uf_flags & FC_DEAD) == 0
		    && (!func_is_global(ufunc)
					     || (p[0] == 'g' && p[1] == ':'))))
	{
	    གལ་སྲིད (is_arg)
		semsg(_(e_argument_name_shadows_existing_variable_str), p);
	    དེ་མིན
		semsg(_(e_name_already_defined_str), p);
	    p[len] = c;
	    ལོག FAIL;
	}
    }
    p[len] = c;
    ལོག OK;
}


/*
 * Return TRUE if "actual" could be "expected" and a runtime typecheck is to be
 * used.  Return FALSE if the types will never match.
 */
    static ཧྲིལ
use_typecheck(type_T *actual, type_T *expected)
{
    གལ་སྲིད (actual->tt_type == VAR_ANY
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
	ལོག TRUE;
    གལ་སྲིད (actual->tt_type == VAR_OBJECT && expected->tt_type == VAR_OBJECT)
	ལོག TRUE;
    གལ་སྲིད ((actual->tt_type == VAR_LIST
		|| actual->tt_type == VAR_TUPLE
		|| actual->tt_type == VAR_DICT)
	    && actual->tt_type == expected->tt_type)
	// This takes care of a nested list or dict.
	ལོག use_typecheck(actual->tt_member, expected->tt_member);
    ལོག FALSE;
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
    ཧྲིལ
need_type_where(
	type_T	*actual,
	type_T	*expected,
	ཧྲིལ	typechk_flags,	// acceptable types (type check flags)
	ཧྲིལ	offset,
	where_T	where,
	cctx_T	*cctx,
	ཧྲིལ	silent,
	ཧྲིལ	actual_is_const)
{
    ཧྲིལ ret;

    གལ་སྲིད (expected->tt_type != VAR_CLASS && expected->tt_type != VAR_TYPEALIAS)
    {
	གལ་སྲིད (check_type_is_value(actual) == FAIL)
	    ལོག FAIL;
    }

    གལ་སྲིད (expected == &t_bool && actual != &t_bool
					&& (actual->tt_flags & TTFLAG_BOOL_OK))
    {
	// Using "0", "1" or the result of an expression with "&&" or "||" as a
	// boolean is OK but requires a conversion.
	generate_2BOOL(cctx, FALSE, offset);
	ལོག OK;
    }

    ret = check_type_maybe(expected, actual, FALSE, where);
    གལ་སྲིད (ret == OK)
	ལོག OK;

    // If actual a constant a runtime check makes no sense.  If it's
    // null_function it is OK.
    གལ་སྲིད (actual_is_const && ret == MAYBE && actual == &t_func_unknown)
	ལོག OK;

    // If the actual type can be the expected type add a runtime check.
    གལ་སྲིད (!actual_is_const && ret == MAYBE && use_typecheck(actual, expected))
    {
	generate_TYPECHECK(cctx, expected, typechk_flags, offset,
		where.wt_kind == WT_VARIABLE, where.wt_index);
	ལོག OK;
    }

    གལ་སྲིད (!silent)
	type_mismatch_where(expected, actual, where);
    ལོག FAIL;
}

    ཧྲིལ
need_type(
	type_T	*actual,
	type_T	*expected,
	ཧྲིལ	typechk_flags,	// acceptable types (type check flags)
	ཧྲིལ	offset,
	ཧྲིལ	arg_idx,
	cctx_T	*cctx,
	ཧྲིལ	silent,
	ཧྲིལ	actual_is_const)
{
    where_T where = WHERE_INIT;

    གལ་སྲིད (arg_idx > 0)
    {
	where.wt_index = arg_idx;
	where.wt_kind = WT_ARGUMENT;
    }
    ལོག need_type_where(actual, expected, typechk_flags, offset, where,
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

    གལ་སྲིད (lvar->lv_const == ASSIGN_CONST && (type->tt_flags & TTFLAG_CONST) == 0)
    {
	གལ་སྲིད (type->tt_flags & TTFLAG_STATIC)
	    // entry in static_types[] is followed by const type
	    type = type + 1;
	དེ་མིན
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
	ཧྲིལ	assign,
	type_T	*type)
{
    lvar_T  *lvar;
    dfunc_T *dfunc;

    གལ་སྲིད (arg_exists(name, len, NULL, NULL, NULL, cctx) == OK)
    {
	emsg_namelen(_(e_str_is_used_as_argument), name, (ཧྲིལ)len);
	ལོག NULL;
    }

    གལ་སྲིད (GA_GROW_FAILS(&cctx->ctx_locals, 1))
	ལོག NULL;
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
    གལ་སྲིད (type == &t_unknown || type == &t_any)
	// type not known yet, may be inferred from RHS
	lvar->lv_type = type;
    དེ་མིན
	// may use TTFLAG_CONST
	set_var_type(lvar, type, cctx);

    // Remember the name for debugging.
    གལ་སྲིད (GA_GROW_FAILS(&dfunc->df_var_names, 1))
	ལོག NULL;
    ((char_u **)dfunc->df_var_names.ga_data)[lvar->lv_idx] =
						    vim_strsave(lvar->lv_name);
    ++dfunc->df_var_names.ga_len;

    ལོག lvar;
}

/*
 * If "check_writable" is ASSIGN_CONST give an error if the variable was
 * defined with :final or :const, if "check_writable" is ASSIGN_FINAL give an
 * error if the variable was defined with :const.
 */
    static ཧྲིལ
check_item_writable(svar_T *sv, ཧྲིལ check_writable, char_u *name)
{
    གལ་སྲིད ((check_writable == ASSIGN_CONST && sv->sv_const != 0)
	    || (check_writable == ASSIGN_FINAL
					      && sv->sv_const == ASSIGN_CONST))
    {
	semsg(_(e_cannot_change_readonly_variable_str), name);
	ལོག FAIL;
    }
    ལོག OK;
}

/*
 * Find "name" in script-local items of script "sid".
 * Pass "check_writable" to check_item_writable().
 * "cctx" is NULL at the script level, "cstack" is NULL in a function.
 * Returns the index in "sn_var_vals" if found.
 * If found but not in "sn_var_vals" returns -1.
 * If not found or the variable is not writable returns -2.
 */
    ཧྲིལ
get_script_item_idx(
	ཧྲིལ	    sid,
	char_u	    *name,
	ཧྲིལ	    check_writable,
	cctx_T	    *cctx,
	cstack_T    *cstack)
{
    hashtab_T	    *ht;
    dictitem_T	    *di;
    scriptitem_T    *si = SCRIPT_ITEM(sid);
    svar_T	    *sv;
    ཧྲིལ		    idx;

    གལ་སྲིད (!SCRIPT_ID_VALID(sid))
	ལོག -1;
    གལ་སྲིད (sid == current_sctx.sc_sid)
    {
	sallvar_T *sav = find_script_var(name, 0, cctx, cstack);

	གལ་སྲིད (sav == NULL)
	    ལོག -2;
	idx = sav->sav_var_vals_idx;
	sv = ((svar_T *)si->sn_var_vals.ga_data) + idx;
	གལ་སྲིད (check_item_writable(sv, check_writable, name) == FAIL)
	    ལོག -2;
	ལོག idx;
    }

    // First look the name up in the hashtable.
    ht = &SCRIPT_VARS(sid);
    di = find_var_in_ht(ht, 0, name, TRUE);
    གལ་སྲིད (di == NULL)
    {
	གལ་སྲིད (si->sn_autoload_prefix != NULL)
	{
	    hashitem_T *hi;

	    // A variable exported from an autoload script is in the global
	    // variables, we can find it in the all_vars table.
	    hi = hash_find(&si->sn_all_vars.dv_hashtab, name);
	    གལ་སྲིད (!HASHITEM_EMPTY(hi))
		ལོག HI2SAV(hi)->sav_var_vals_idx;
	}
	ལོག -2;
    }

    // Now find the svar_T index in sn_var_vals.
    དོན་ལུ (idx = 0; idx < si->sn_var_vals.ga_len; ++idx)
    {
	sv = ((svar_T *)si->sn_var_vals.ga_data) + idx;
	གལ་སྲིད (sv->sv_tv == &di->di_tv)
	{
	    གལ་སྲིད (check_item_writable(sv, check_writable, name) == FAIL)
		ལོག -2;
	    ལོག idx;
	}
    }
    ལོག -1;
}

    static imported_T *
find_imported_in_script(char_u *name, size_t len, ཧྲིལ sid)
{
    scriptitem_T    *si;
    ཧྲིལ		    idx;

    གལ་སྲིད (!SCRIPT_ID_VALID(sid))
	ལོག NULL;
    si = SCRIPT_ITEM(sid);
    དོན་ལུ (idx = 0; idx < si->sn_imports.ga_len; ++idx)
    {
	imported_T *import = ((imported_T *)si->sn_imports.ga_data) + idx;

	གལ་སྲིད (len == 0 ? STRCMP(name, import->imp_name) == 0
		     : STRLEN(import->imp_name) == len
				  && STRNCMP(name, import->imp_name, len) == 0)
	    ལོག import;
    }
    ལོག NULL;
}

/*
 * Find "name" in imported items of the current script.
 * If "len" is 0 use any length that works.
 * If "load" is TRUE and the script was not loaded yet, load it now.
 */
    imported_T *
find_imported(char_u *name, size_t len, ཧྲིལ load)
{
    གལ་སྲིད (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	ལོག NULL;

    // Skip over "s:" before "s:something" to find the import name.
    ཧྲིལ off = name[0] == 's' && name[1] == ':' ? 2 : 0;

    imported_T *ret = find_imported_in_script(name + off, len - off,
							  current_sctx.sc_sid);
    གལ་སྲིད (ret != NULL && load && (ret->imp_flags & IMP_FLAGS_AUTOLOAD))
    {
	scid_T	actual_sid = 0;
	ཧྲིལ	save_emsg_off = emsg_off;

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
	གལ་སྲིད (actual_sid != 0)
	    ret->imp_sid = actual_sid;

	emsg_off = save_emsg_off;
    }
    ལོག ret;
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
    གལ་སྲིད (*p == NUL || (VIM_ISWHITE(*arg) && vim9_comment_start(p)))
    {
	*nextp = peek_next_line_from_context(cctx);
	གལ་སྲིད (*nextp != NULL)
	    ལོག *nextp;
    }
    ལོག p;
}

/*
 * Return a pointer to the next line that isn't empty or only contains a
 * comment. Skips over white space.
 * Returns NULL if there is none.
 */
    char_u *
peek_next_line_from_context(cctx_T *cctx)
{
    ཧྲིལ lnum = cctx->ctx_lnum;

    སྐབས (++lnum < cctx->ctx_ufunc->uf_lines.ga_len)
    {
	char_u *line = ((char_u **)cctx->ctx_ufunc->uf_lines.ga_data)[lnum];
	char_u *p;

	// ignore NULLs inserted for continuation lines
	གལ་སྲིད (line != NULL)
	{
	    p = skipwhite(line);
	    གལ་སྲིད (vim9_bad_comment(p))
		ལོག NULL;
	    གལ་སྲིད (*p != NUL && !vim9_comment_start(p))
		ལོག p;
	}
    }
    ལོག NULL;
}

/*
 * Get the next line of the function from "cctx".
 * Skips over empty lines.  Skips over comment lines if "skip_comment" is TRUE.
 * Returns NULL when at the end.
 */
    char_u *
next_line_from_context(cctx_T *cctx, ཧྲིལ skip_comment)
{
    char_u	*line;

    do
    {
	++cctx->ctx_lnum;
	གལ་སྲིད (cctx->ctx_lnum >= cctx->ctx_ufunc->uf_lines.ga_len)
	{
	    line = NULL;
	    bha_nga;
	}
	line = ((char_u **)cctx->ctx_ufunc->uf_lines.ga_data)[cctx->ctx_lnum];
	cctx->ctx_line_start = line;
	SOURCING_LNUM = cctx->ctx_lnum + 1;
    } སྐབས (line == NULL || *skipwhite(line) == NUL
		     || (skip_comment && vim9_comment_start(skipwhite(line))));
    ལོག line;
}

/*
 * Skip over white space at "whitep" and assign to "*arg".
 * If "*arg" is at the end of the line, advance to the next line.
 * Also when "whitep" points to white space and "*arg" is on a "#".
 * Return FAIL if beyond the last line, "*arg" is unmodified then.
 */
    ཧྲིལ
may_get_next_line(char_u *whitep, char_u **arg, cctx_T *cctx)
{
    *arg = skipwhite(whitep);
    གལ་སྲིད (vim9_bad_comment(*arg))
	ལོག FAIL;
    གལ་སྲིད (**arg == NUL || (VIM_ISWHITE(*whitep) && vim9_comment_start(*arg)))
    {
	char_u *next = next_line_from_context(cctx, TRUE);

	གལ་སྲིད (next == NULL)
	    ལོག FAIL;
	*arg = skipwhite(next);
    }
    ལོག OK;
}

/*
 * Idem, and give an error when failed.
 */
    ཧྲིལ
may_get_next_line_error(char_u *whitep, char_u **arg, cctx_T *cctx)
{
    གལ་སྲིད (may_get_next_line(whitep, arg, cctx) == FAIL)
    {
	SOURCING_LNUM = cctx->ctx_lnum + 1;
	emsg(_(e_line_incomplete));
	ལོག FAIL;
    }
    ལོག OK;
}

/*
 * Get a line from the compilation context, compatible with exarg_T getline().
 * Return a pointer to the line in allocated memory.
 * Return NULL for end-of-file or some error.
 */
    static char_u *
exarg_getline(
	ཧྲིལ c UNUSED,
	void *cookie,
	ཧྲིལ indent UNUSED,
	getline_opt_T options UNUSED)
{
    cctx_T  *cctx = (cctx_T *)cookie;
    char_u  *p;

    དོན་ལུ (;;)
    {
	གལ་སྲིད (cctx->ctx_lnum >= cctx->ctx_ufunc->uf_lines.ga_len - 1)
	    ལོག NULL;
	++cctx->ctx_lnum;
	p = ((char_u **)cctx->ctx_ufunc->uf_lines.ga_data)[cctx->ctx_lnum];
	// Comment lines result in NULL pointers, skip them.
	གལ་སྲིད (p != NULL)
	    ལོག vim_strsave(p);
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
    ཧྲིལ
func_needs_compiling(ufunc_T *ufunc, compiletype_T compile_type)
{
    switch (ufunc->uf_def_status)
    {
	pak_sa UF_TO_BE_COMPILED:
	    ལོག TRUE;

	pak_sa UF_COMPILED:
	{
	    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data)
							 + ufunc->uf_dfunc_idx;

	    switch (compile_type)
	    {
		pak_sa CT_PROFILE:
#ifdef FEAT_PROFILE
		    ལོག dfunc->df_instr_prof == NULL;
#endif
		pak_sa CT_NONE:
		    ལོག dfunc->df_instr == NULL;
		pak_sa CT_DEBUG:
		    ལོག dfunc->df_instr_debug == NULL;
	    }
	}

	pak_sa UF_NOT_COMPILED:
	pak_sa UF_COMPILE_ERROR:
	pak_sa UF_COMPILING:
	    bha_nga;
    }
    ལོག FALSE;
}

/*
 * Compile a nested :def command.
 */
    static char_u *
compile_nested_function(exarg_T *eap, cctx_T *cctx, garray_T *lines_to_free)
{
    ཧྲིལ		is_global = *eap->arg == 'g' && eap->arg[1] == ':';
    char_u	*name_start = eap->arg;
    char_u	*name_end = to_name_end(eap->arg, TRUE);
    ཧྲིལ		off;
    char_u	*func_name;
    string_T	lambda_name;
    ufunc_T	*ufunc;
    ཧྲིལ		r = FAIL;
    compiletype_T   compile_type;
    ཧྲིལ		funcref_isn_idx = -1;
    lvar_T	*lvar = NULL;
    char_u	*bracket_start = NULL;

    གལ་སྲིད (eap->forceit)
    {
	semsg(_(e_cannot_use_bang_with_nested_def_str),
		eap->cmdidx == CMD_def ? ":def" : ":function");
	ལོག NULL;
    }

    གལ་སྲིད (*name_start == '/')
    {
	name_end = skip_regexp(name_start + 1, '/', TRUE);
	གལ་སྲིད (*name_end == '/')
	    ++name_end;
	set_nextcmd(eap, name_end);
    }

    གལ་སྲིད (*name_end == '<')
    {
	bracket_start = name_end;
	གལ་སྲིད (skip_generic_func_type_args(&name_end) == FAIL)
	    ལོག NULL;
    }

    གལ་སྲིད (name_end == name_start || *skipwhite(name_end) != '(')
    {
	གལ་སྲིད (!ends_excmd2(name_start, name_end))
	{
	    གལ་སྲིད (*skipwhite(name_end) == '.')
		semsg(_(e_cannot_define_dict_func_in_vim9_script_str),
								     eap->cmd);
	    དེ་མིན
		semsg(_(e_invalid_command_str), eap->cmd);
	    ལོག NULL;
	}

	// "def" or "def Name": list functions
	གལ་སྲིད (generate_DEF(cctx, name_start, name_end - name_start) == FAIL)
	    ལོག NULL;
	ལོག eap->nextcmd == NULL ? (char_u *)"" : eap->nextcmd;
    }

    གལ་སྲིད (bracket_start != NULL)
	// generic function.  The function name ends before the list of types
	// (opening angle bracket).
	name_end = bracket_start;

    // Only g:Func() can use a namespace.
    གལ་སྲིད (name_start[1] == ':' && !is_global)
    {
	semsg(_(e_namespace_not_supported_str), name_start);
	ལོག NULL;
    }
    གལ་སྲིད (cctx->ctx_skip != SKIP_YES
	    && check_defined(name_start, name_end - name_start, cctx,
							  NULL, FALSE) == FAIL)
	ལོག NULL;
    གལ་སྲིད (!ASCII_ISUPPER(is_global ? name_start[2] : name_start[0]))
    {
	semsg(_(e_function_name_must_start_with_capital_str), name_start);
	ལོག NULL;
    }

    eap->arg = name_end;
    fill_exarg_from_cctx(eap, cctx);

    eap->forceit = FALSE;
    // We use the special <lambda>99 name, but it's not really a lambda.
    lambda_name = get_lambda_name();
    lambda_name.string = vim_strnsave(lambda_name.string, lambda_name.length);
    གལ་སྲིད (lambda_name.string == NULL)
	ལོག NULL;

    // This may free the current line, make a copy of the name.
    off = is_global ? 2 : 0;
    func_name = vim_strnsave(name_start + off, name_end - name_start - off);
    གལ་སྲིད (func_name == NULL)
    {
	r = FAIL;
	goto theend;
    }

    // A type variable cannot have the name of the function.  The function is
    // defined with the "<lambda>" name, check it here.
    གལ་སྲིད (bracket_start != NULL)
    {
	size_t	len = STRLEN(func_name);

	དོན་ལུ (char_u *t = bracket_start + 1; *t != '>' && *t != NUL; )
	{
	    char_u *te = to_name_end(t, FALSE);

	    གལ་སྲིད ((size_t)(te - t) == len && STRNCMP(t, func_name, len) == 0)
	    {
		semsg(_(e_redefining_script_item_str), func_name);
		r = FAIL;
		goto theend;
	    }
	    t = te == t ? t + 1 : te;
	}
    }

    // Make sure "KeyTyped" is not set, it may cause indent to be written.
    ཧྲིལ save_KeyTyped = KeyTyped;
    KeyTyped = FALSE;

    ufunc = define_function(eap, lambda_name.string, lines_to_free, 0, NULL, 0,
									cctx);

    KeyTyped = save_KeyTyped;

    གལ་སྲིད (ufunc == NULL)
    {
	r = eap->skip ? OK : FAIL;
	goto theend;
    }
    གལ་སྲིད (eap->nextcmd != NULL)
    {
	semsg(_(e_text_found_after_str_str),
	      eap->cmdidx == CMD_def ? "enddef" : "endfunction", eap->nextcmd);
	r = FAIL;
	func_ptr_unref(ufunc);
	goto theend;
    }

    // copy over the block scope IDs before compiling
    གལ་སྲིད (!is_global && cctx->ctx_ufunc->uf_block_depth > 0)
    {
	ཧྲིལ block_depth = cctx->ctx_ufunc->uf_block_depth;

	ufunc->uf_block_ids = ALLOC_MULT(ཧྲིལ, block_depth);
	གལ་སྲིད (ufunc->uf_block_ids != NULL)
	{
	    mch_memmove(ufunc->uf_block_ids, cctx->ctx_ufunc->uf_block_ids,
						    parimaa_na(ཧྲིལ) * block_depth);
	    ufunc->uf_block_depth = block_depth;
	}
    }

    // Define the funcref before compiling, so that it is found by any
    // recursive call.
    གལ་སྲིད (is_global)
    {
	r = generate_NEWFUNC(cctx, lambda_name.string, func_name);
	func_name = NULL;
	lambda_name.string = NULL;
	lambda_name.length = 0;
    }
    དེ་མིན
    {
	// Define a local variable for the function reference.
	lvar = reserve_local(cctx, func_name, name_end - name_start,
					    ASSIGN_CONST, ufunc->uf_func_type);
	གལ་སྲིད (lvar == NULL)
	{
	    func_ptr_unref(ufunc);
	    goto theend;
	}
	lvar->lv_ufunc = ufunc;
	གལ་སྲིད (generate_FUNCREF(cctx, ufunc, NULL, FALSE, 0, &funcref_isn_idx) == FAIL)
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
    གལ་སྲིད (cctx->ctx_compile_type == CT_PROFILE)
	compile_type = CT_PROFILE;
#endif
    གལ་སྲིད (func_needs_compiling(ufunc, compile_type)
	    && compile_def_function(ufunc, TRUE, compile_type, cctx) == FAIL)
    {
	func_ptr_unref(ufunc);
	གལ་སྲིད (lvar != NULL)
	    // Now the local variable can't be used.
	    *lvar->lv_name = '/';  // impossible value
	goto theend;
    }

#ifdef FEAT_PROFILE
    // When the outer function is compiled for profiling, the nested function
    // may be called without profiling.  Compile it here in the right context.
    གལ་སྲིད (compile_type == CT_PROFILE && func_needs_compiling(ufunc, CT_NONE))
	compile_def_function(ufunc, FALSE, CT_NONE, cctx);
#endif

    // If a FUNCREF instruction was generated, set the index after compiling.
    གལ་སྲིད (funcref_isn_idx != -1 && ufunc->uf_def_status == UF_COMPILED)
    {
	isn_T	*funcref_isn = ((isn_T *)cctx->ctx_instr.ga_data) +
							funcref_isn_idx;
	funcref_isn->isn_arg.funcref.fr_dfunc_idx = ufunc->uf_dfunc_idx;
    }

theend:
    vim_free(lambda_name.string);
    vim_free(func_name);
    ལོག r == FAIL ? NULL : (char_u *)"";
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
    གལ་སྲིད (*block_start != NUL && skip_expr(&block_end, NULL) == FAIL)
	ལོག NULL;
    block_end = skipwhite(block_end);
    // The block must be closed by a }.
    གལ་སྲིད (*block_end != '}')
    {
	semsg(_(e_missing_close_curly_str), p);
	ལོག NULL;
    }
    གལ་སྲིད (compile_expr0(&block_start, cctx) == FAIL)
	ལོག NULL;
    may_generate_2STRING(-1, TOSTRING_INTERPOLATE, cctx);

    ལོག block_end + 1;
}

/*
 * Compile a string "str" (either containing a literal string or a mix of
 * literal strings and Vim expressions of the form `{expr}`).  This is used
 * when compiling a heredoc assignment to a variable or an interpolated string
 * in a Vim9 def function.  Vim9 instructions are generated to push strings,
 * evaluate expressions, concatenate them and create a list of lines.  When
 * "evalstr" is TRUE, Vim expressions in "str" are evaluated.
 */
    ཧྲིལ
compile_all_expr_in_str(char_u *str, ཧྲིལ evalstr, cctx_T *cctx)
{
    char_u	*p = str;
    char_u	*val;
    ཧྲིལ		count = 0;

    གལ་སྲིད (cctx->ctx_skip == SKIP_YES)
	ལོག OK;

    གལ་སྲིད (!evalstr || *str == NUL)
    {
	// Literal string, possibly empty.
	val = *str != NUL ? vim_strsave(str) : NULL;
	ལོག generate_PUSHS(cctx, &val);
    }

    // Push all the string pieces to the stack, followed by a ISN_CONCAT.
    སྐབས (*p != NUL)
    {
	char_u	*lit_start;
	ཧྲིལ	escaped_brace = FALSE;

	// Look for a block start.
	lit_start = p;
	སྐབས (*p != '{' && *p != '}' && *p != NUL)
	    ++p;

	གལ་སྲིད (*p != NUL && *p == p[1])
	{
	    // Escaped brace, unescape and continue.
	    // Include the brace in the literal string.
	    ++p;
	    escaped_brace = TRUE;
	}
	དེ་མིན གལ་སྲིད (*p == '}')
	{
	    semsg(_(e_stray_closing_curly_str), str);
	    ལོག FAIL;
	}

	// Append the literal part.
	གལ་སྲིད (p != lit_start)
	{
	    val = vim_strnsave(lit_start, (size_t)(p - lit_start));
	    གལ་སྲིད (generate_PUSHS(cctx, &val) == FAIL)
		ལོག FAIL;
	    ++count;
	}

	གལ་སྲིད (*p == NUL)
	    bha_nga;

	གལ་སྲིད (escaped_brace)
	{
	    // Skip the second brace.
	    ++p;
	    continue;
	}

	p = compile_one_expr_in_str(p, cctx);
	གལ་སྲིད (p == NULL)
	    ལོག FAIL;
	++count;
    }

    // Small optimization, if there's only a single piece skip the ISN_CONCAT.
    གལ་སྲིད (count > 1)
	ལོག generate_CONCAT(cctx, count);

    ལོག OK;
}

/*
 * Return the length of an assignment operator, or zero if there isn't one.
 */
    ཧྲིལ
assignment_len(char_u *p, ཧྲིལ *heredoc)
{
    གལ་སྲིད (*p == '=')
    {
	གལ་སྲིད (p[1] == '<' && p[2] == '<')
	{
	    *heredoc = TRUE;
	    ལོག 3;
	}
	ལོག 1;
    }
    གལ་སྲིད (vim_strchr((char_u *)"+-*/%", *p) != NULL && p[1] == '=')
	ལོག 2;
    གལ་སྲིད (STRNCMP(p, "..=", 3) == 0)
	ལོག 3;
    ལོག 0;
}

/*
 * Generate the load instruction for "name".
 */
    static ཧྲིལ
generate_loadvar(cctx_T *cctx, lhs_T *lhs)
{
    char_u	*name = lhs->lhs_name;
    type_T	*type = lhs->lhs_type;
    ཧྲིལ		res = OK;

    switch (lhs->lhs_dest)
    {
	pak_sa dest_option:
	pak_sa dest_func_option:
	    generate_LOAD(cctx, ISN_LOADOPT, 0, name, type);
	    bha_nga;
	pak_sa dest_global:
	    གལ་སྲིད (vim_strchr(name, AUTOLOAD_CHAR) == NULL)
	    {
		གལ་སྲིད (name[2] == NUL)
		    generate_instr_type(cctx, ISN_LOADGDICT, &t_dict_any);
		དེ་མིན
		    generate_LOAD(cctx, ISN_LOADG, 0, name + 2, type);
	    }
	    དེ་མིན
		generate_LOAD(cctx, ISN_LOADAUTO, 0, name, type);
	    bha_nga;
	pak_sa dest_buffer:
	    generate_LOAD(cctx, ISN_LOADB, 0, name + 2, type);
	    bha_nga;
	pak_sa dest_window:
	    generate_LOAD(cctx, ISN_LOADW, 0, name + 2, type);
	    bha_nga;
	pak_sa dest_tab:
	    generate_LOAD(cctx, ISN_LOADT, 0, name + 2, type);
	    bha_nga;
	pak_sa dest_script:
	pak_sa dest_script_v9:
	    res = compile_load_scriptvar(cctx,
			    name + (name[1] == ':' ? 2 : 0), NULL, NULL, NULL);
	    bha_nga;
	pak_sa dest_env:
	    // Include $ in the name here
	    generate_LOAD(cctx, ISN_LOADENV, 0, name, type);
	    bha_nga;
	pak_sa dest_reg:
	    generate_LOAD(cctx, ISN_LOADREG, name[1], NULL, &t_string);
	    bha_nga;
	pak_sa dest_vimvar:
	    generate_LOADV(cctx, name + 2);
	    bha_nga;
	pak_sa dest_local:
	    གལ་སྲིད (cctx->ctx_skip != SKIP_YES)
	    {
		lvar_T	*lvar = lhs->lhs_lvar;
		གལ་སྲིད (lvar->lv_from_outer > 0)
		    generate_LOADOUTER(cctx, lvar->lv_idx, lvar->lv_from_outer,
				 lvar->lv_loop_depth, lvar->lv_loop_idx, type);
		དེ་མིན
		    generate_LOAD(cctx, ISN_LOAD, lvar->lv_idx, NULL, type);
	    }
	    bha_nga;
	pak_sa dest_class_member:
	    generate_CLASSMEMBER(cctx, TRUE, lhs->lhs_class,
						     lhs->lhs_classmember_idx);
	    bha_nga;
	pak_sa dest_expr:
	    // list or dict value should already be on the stack.
	    bha_nga;
    }

    ལོག res;
}

/*
 * Skip over "[expr]" or ".member".
 * Does not check for any errors.
 */
    static char_u *
skip_index(char_u *start)
{
    char_u *p = start;

    གལ་སྲིད (*p == '[')
    {
	p = skipwhite(p + 1);
	(void)skip_expr(&p, NULL);
	p = skipwhite(p);
	གལ་སྲིད (*p == ']')
	    ལོག p + 1;
	ལོག p;
    }
    // if (*p == '.')
    ལོག to_name_end(p + 1, TRUE);
}

    void
vim9_declare_error(char_u *name)
{
    var_na *scope = "";

    switch (*name)
    {
	pak_sa 'g': scope = _("global"); bha_nga;
	pak_sa 'b': scope = _("buffer"); bha_nga;
	pak_sa 'w': scope = _("window"); bha_nga;
	pak_sa 't': scope = _("tab"); bha_nga;
	pak_sa 'v': scope = "v:"; bha_nga;
	pak_sa '$': semsg(_(e_cannot_declare_an_environment_variable_str), name);
		  ལོག;
	pak_sa '&': semsg(_(e_cannot_declare_an_option_str), name);
		  ལོག;
	pak_sa '@': semsg(_(e_cannot_declare_a_register_str), name);
		  ལོག;
	default: ལོག;
    }
    semsg(_(e_cannot_declare_a_scope_variable_str), scope, name);
}

/*
 * Return TRUE if "name" is a valid register to use.
 * Return FALSE and give an error message if not.
 */
    static ཧྲིལ
valid_dest_reg(ཧྲིལ name)
{
    གལ་སྲིད (name == '@')
       name = '"';
    གལ་སྲིད (name == '/' || name == '=' || name == '#' || valid_yank_reg(name, TRUE))
	ལོག TRUE;
    emsg_invreg(name);
    ལོག FAIL;
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
    ཧྲིལ
get_var_dest(
	char_u		*name,
	assign_dest_T	*dest,
	cmdidx_T	cmdidx,
	ཧྲིལ		has_index,
	ཧྲིལ		*option_scope,
	ཧྲིལ		*vimvaridx,
	type_T		**type,
	cctx_T		*cctx)
{
    char_u *p;

    གལ་སྲིད (*name == '&')
    {
	ཧྲིལ		cc;
	long		numval;
	getoption_T	opt_type;
	ཧྲིལ		opt_p_flags;

	*dest = dest_option;
	གལ་སྲིད (cmdidx == CMD_final || cmdidx == CMD_const)
	{
	    emsg(_(e_cannot_lock_option));
	    ལོག FAIL;
	}
	p = name;
	p = find_option_end(&p, option_scope);
	གལ་སྲིད (p == NULL)
	{
	    // cannot happen?
	    emsg(_(e_unexpected_characters_in_assignment));
	    ལོག FAIL;
	}
	cc = *p;
	*p = NUL;
	opt_type = get_option_value(skip_option_env_lead(name),
				   &numval, NULL, &opt_p_flags, *option_scope);
	*p = cc;
	switch (opt_type)
	{
	    pak_sa gov_unknown:
		    semsg(_(e_unknown_option_str), name);
		    ལོག FAIL;
	    pak_sa gov_string:
	    pak_sa gov_hidden_string:
		    གལ་སྲིད (opt_p_flags & P_FUNC)
		    {
			// might be a Funcref, check the type later
			*type = &t_any;
			*dest = dest_func_option;
		    }
		    དེ་མིན
		    {
			*type = &t_string;
		    }
		    bha_nga;
	    pak_sa gov_bool:
	    pak_sa gov_hidden_bool:
		    *type = &t_bool;
		    bha_nga;
	    pak_sa gov_number:
	    pak_sa gov_hidden_number:
		    *type = &t_number;
		    bha_nga;
	}
    }
    དེ་མིན གལ་སྲིད (*name == '$')
    {
	*dest = dest_env;
	*type = &t_string;
    }
    དེ་མིན གལ་སྲིད (*name == '@')
    {
	གལ་སྲིད (!valid_dest_reg(name[1]))
	    ལོག FAIL;
	*dest = dest_reg;
	*type = name[1] == '#' ? &t_number_or_string : &t_string;
    }
    དེ་མིན གལ་སྲིད (STRNCMP(name, "g:", 2) == 0)
    {
	*dest = dest_global;
    }
    དེ་མིན གལ་སྲིད (STRNCMP(name, "b:", 2) == 0)
    {
	*dest = dest_buffer;
    }
    དེ་མིན གལ་སྲིད (STRNCMP(name, "w:", 2) == 0)
    {
	*dest = dest_window;
    }
    དེ་མིན གལ་སྲིད (STRNCMP(name, "t:", 2) == 0)
    {
	*dest = dest_tab;
    }
    དེ་མིན གལ་སྲིད (STRNCMP(name, "v:", 2) == 0)
    {
	typval_T	*vtv;
	ཧྲིལ		di_flags;

	*vimvaridx = find_vim_var(name + 2, &di_flags);
	གལ་སྲིད (*vimvaridx < 0)
	{
	    semsg(_(e_variable_not_found_str), name);
	    ལོག FAIL;
	}
	// An item of a read-only variable can be changed, as when not compiled.
	// We use the current value of "sandbox" here, is that OK?
	གལ་སྲིད (!has_index && var_check_ro(di_flags, name, FALSE))
	    ལོག FAIL;
	*dest = dest_vimvar;
	vtv = get_vim_var_tv(*vimvaridx);
	*type = typval2type_vimvar(vtv, cctx->ctx_type_list);
    }
    ལོག OK;
}

    static ཧྲིལ
is_decl_command(cmdidx_T cmdidx)
{
    ལོག cmdidx == CMD_let || cmdidx == CMD_var
				 || cmdidx == CMD_final || cmdidx == CMD_const;
}

/*
 * Returns TRUE if the class or object variable in "lhs" is modifiable.
 * "var_start" points to the start of the variable name and "lhs->lhs_varlen"
 * has the total length.  Note that the "lhs" can be nested an object reference
 * (e.g.  a.b.c.d.var).
 */
    static ཧྲིལ
lhs_class_member_modifiable(lhs_T *lhs, char_u	*var_start, cctx_T *cctx)
{
    size_t	varlen = lhs->lhs_varlen;
    class_T	*cl = lhs->lhs_type->tt_class;
    ཧྲིལ		is_object = lhs->lhs_type->tt_type == VAR_OBJECT;
    char_u	*name = var_start + varlen + 1;
    size_t	namelen = lhs->lhs_end - var_start - varlen - 1;
    ocmember_T	*m;

    m = member_lookup(cl, lhs->lhs_type->tt_type, name, namelen, NULL);
    གལ་སྲིད (m == NULL)
    {
	member_not_found_msg(cl, lhs->lhs_type->tt_type, name, namelen);
	ལོག FALSE;
    }

    གལ་སྲིད (IS_ENUM(cl))
    {
	semsg(_(e_enumvalue_str_cannot_be_modified), cl->class_name.string,
		m->ocm_name.string);
	ལོག FALSE;
    }

    // If it is private member variable, then accessing it outside the
    // class is not allowed.
    // If it is a read only class variable, then it can be modified
    // only inside the class where it is defined.
    གལ་སྲིད ((m->ocm_access != VIM_ACCESS_ALL) &&
	    ((is_object && !inside_class(cctx, cl))
	     || (!is_object && cctx->ctx_ufunc->uf_class != cl)))
    {
	var_na *msg = (m->ocm_access == VIM_ACCESS_PRIVATE)
				? e_cannot_access_protected_variable_str
				: e_variable_is_not_writable_str;
	emsg_var_cl_define(msg, m->ocm_name.string, 0, cl);
	ལོག FALSE;
    }

    ལོག TRUE;
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
    static ཧྲིལ
lhs_find_var_end(
    lhs_T	*lhs,
    char_u	*var_start,
    ཧྲིལ		is_decl,
    char_u	**var_endp)
{
    char_u  *var_end = *var_endp;

    // "lhs_dest_end" is the end of the destination, including "[expr]" or
    // ".name".
    // "var_end" is the end of the variable/option/etc. name.
    lhs->lhs_dest_end = skip_var_one(var_start, FALSE);
    གལ་སྲིད (*var_start == '@')
    {
	གལ་སྲིད (!valid_dest_reg(var_start[1]))
	    ལོག FAIL;
	var_end = var_start + 2;
    }
    དེ་མིན
    {
	// skip over the leading "&", "&l:", "&g:" and "$"
	var_end = skip_option_env_lead(var_start);
	var_end = to_name_end(var_end, TRUE);
    }

    // "a: type" is declaring variable "a" with a type, not dict "a:".
    གལ་སྲིད (is_decl && lhs->lhs_dest_end == var_start + 2
					&& lhs->lhs_dest_end[-1] == ':')
	--lhs->lhs_dest_end;
    གལ་སྲིད (is_decl && var_end == var_start + 2 && var_end[-1] == ':')
	--var_end;

    lhs->lhs_end = lhs->lhs_dest_end;
    *var_endp = var_end;

    ལོག OK;
}

/*
 * Set various fields in "lhs"
 */
    static ཧྲིལ
lhs_init(
    lhs_T	*lhs,
    char_u	*var_start,
    ཧྲིལ		is_decl,
    ཧྲིལ		heredoc,
    char_u	**var_endp)
{
    char_u *var_end = *var_endp;

    lhs_init_defaults(lhs);

    // Find the end of the variable and the destination
    གལ་སྲིད (lhs_find_var_end(lhs, var_start, is_decl, &var_end) == FAIL)
	ལོག FAIL;

    // compute the length of the destination without "[expr]" or ".name"
    lhs->lhs_varlen = var_end - var_start;
    lhs->lhs_varlen_total = lhs->lhs_varlen;
    lhs->lhs_name = vim_strnsave(var_start, lhs->lhs_varlen);
    གལ་སྲིད (lhs->lhs_name == NULL)
	ལོག FAIL;

    གལ་སྲིད (lhs->lhs_dest_end > var_start + lhs->lhs_varlen)
	// Something follows after the variable: "var[idx]" or "var.key".
	lhs->lhs_has_index = TRUE;

    lhs->lhs_type = heredoc ? &t_list_string : &t_any;

    *var_endp = var_end;

    ལོག OK;
}

/*
 * Compile a LHS class variable name.
 */
    static ཧྲིལ
compile_lhs_class_variable(
    cctx_T	*cctx,
    lhs_T	*lhs,
    class_T	*defcl,
    ཧྲིལ		is_decl)
{
    གལ་སྲིད (cctx->ctx_ufunc->uf_defclass != defcl)
    {
	// A class variable can be accessed without the class name
	// only inside a class.
	semsg(_(e_class_variable_str_accessible_only_inside_class_str),
		lhs->lhs_name, defcl->class_name.string);
	ལོག FAIL;
    }

    གལ་སྲིད (is_decl)
    {
	semsg(_(e_variable_already_declared_in_class_str), lhs->lhs_name);
	ལོག FAIL;
    }

    ocmember_T	*m = &defcl->class_class_members[lhs->lhs_classmember_idx];
    གལ་སྲིད (oc_var_check_ro(defcl, m))
	ལོག FAIL;

    lhs->lhs_dest = dest_class_member;
    // The class variable is defined either in the current class or
    // in one of the parent class in the hierarchy.
    lhs->lhs_class = defcl;
    lhs->lhs_type = oc_member_type_by_idx(defcl, FALSE,
						lhs->lhs_classmember_idx);

    ལོག OK;
}

/*
 * Compile an imported LHS variable
 */
    static ཧྲིལ
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
    གལ་སྲིད (dot == NULL)
    {
	semsg(_(e_no_dot_after_imported_name_str), var_start);
	ལོག FAIL;
    }

    p = skipwhite(dot + 1);
    var_end = to_name_end(p, TRUE);
    གལ་སྲིད (var_end == p)
    {
	semsg(_(e_missing_name_after_imported_name_str), var_start);
	ལོག FAIL;
    }

    vim_free(lhs->lhs_name);
    lhs->lhs_varlen = var_end - p;
    lhs->lhs_name = vim_strnsave(p, lhs->lhs_varlen);
    གལ་སྲིད (lhs->lhs_name == NULL)
	ལོག FAIL;
    *rawnamep = lhs->lhs_name;
    lhs->lhs_scriptvar_sid = import->imp_sid;

    // TODO: where do we check this name is exported?

    // Check if something follows: "exp.var[idx]" or
    // "exp.var.key".
    lhs->lhs_has_index = lhs->lhs_dest_end > skipwhite(var_end);

    *var_endp = var_end;

    ལོག OK;
}

/*
 * Process a script-local variable when compiling a LHS variable name.
 */
    static ཧྲིལ
compile_lhs_script_var(
    cctx_T	*cctx,
    lhs_T	*lhs,
    char_u	*var_start,
    char_u	*var_end,
    ཧྲིལ		is_decl)
{
    ཧྲིལ		script_namespace = FALSE;
    ཧྲིལ		script_var = FALSE;
    imported_T	*import;
    char_u	*var_name;
    size_t	var_name_len;

    གལ་སྲིད (lhs->lhs_varlen > 1 && STRNCMP(var_start, "s:", 2) == 0)
	script_namespace = TRUE;

    གལ་སྲིད (script_namespace)
    {
	var_name = var_start + 2;
	var_name_len = lhs->lhs_varlen - 2;
    }
    དེ་མིན
    {
	var_name = var_start;
	var_name_len = lhs->lhs_varlen;
    }

    གལ་སྲིད (script_var_exists(var_name, var_name_len, cctx, NULL) == OK)
	script_var = TRUE;

    import = find_imported(var_start, lhs->lhs_varlen, FALSE);

    གལ་སྲིད (script_namespace || script_var || import != NULL)
    {
	char_u *rawname = lhs->lhs_name + (lhs->lhs_name[1] == ':' ? 2 : 0);

	གལ་སྲིད (script_namespace && current_script_is_vim9())
	{
	    semsg(_(e_cannot_use_s_colon_in_vim9_script_str), var_start);
	    ལོག FAIL;
	}

	གལ་སྲིད (is_decl)
	{
	    གལ་སྲིད (script_namespace)
		semsg(_(e_cannot_declare_script_variable_in_function_str),
			lhs->lhs_name);
	    དེ་མིན
		semsg(_(e_variable_already_declared_in_script_str),
			lhs->lhs_name);
	    ལོག FAIL;
	}
	དེ་མིན གལ་སྲིད (cctx->ctx_ufunc->uf_script_ctx_version == SCRIPT_VERSION_VIM9
		&& script_namespace
		&& !script_var && import == NULL)
	{
	    semsg(_(e_unknown_variable_str), lhs->lhs_name);
	    ལོག FAIL;
	}

	lhs->lhs_dest = current_script_is_vim9() ? dest_script_v9 :
								dest_script;

	// existing script-local variables should have a type
	lhs->lhs_scriptvar_sid = current_sctx.sc_sid;
	གལ་སྲིད (import != NULL)
	{
	    གལ་སྲིད (compile_lhs_import_var(lhs, import, var_start, &var_end,
							&rawname) == FAIL)
		ལོག FAIL;
	}

	གལ་སྲིད (SCRIPT_ID_VALID(lhs->lhs_scriptvar_sid))
	{
	    // Check writable only when no index follows.
	    lhs->lhs_scriptvar_idx = get_script_item_idx(
					lhs->lhs_scriptvar_sid, rawname,
					lhs->lhs_has_index ?  ASSIGN_FINAL :
					ASSIGN_CONST, cctx, NULL);
	    གལ་སྲིད (lhs->lhs_scriptvar_idx >= 0)
	    {
		scriptitem_T *si = SCRIPT_ITEM(lhs->lhs_scriptvar_sid);
		svar_T	 *sv = ((svar_T *)si->sn_var_vals.ga_data)
						+ lhs->lhs_scriptvar_idx;

		lhs->lhs_type = sv->sv_type;
	    }
	}

	ལོག OK;
    }

    ལོག check_defined(var_start, lhs->lhs_varlen, cctx, NULL, FALSE);
}

/*
 * Compile the LHS destination.
 */
    static ཧྲིལ
compile_lhs_var_dest(
    cctx_T	*cctx,
    lhs_T	*lhs,
    ཧྲིལ		cmdidx,
    char_u	*var_start,
    char_u	*var_end,
    ཧྲིལ		is_decl,
    ཧྲིལ		has_cmd)	// "var" before "var_start"
{
    ཧྲིལ	    declare_error = FALSE;

    གལ་སྲིད (get_var_dest(lhs->lhs_name, &lhs->lhs_dest, cmdidx,
				lhs->lhs_has_index,
				&lhs->lhs_opt_flags, &lhs->lhs_vimvaridx,
				&lhs->lhs_type, cctx) == FAIL)
	ལོག FAIL;

    གལ་སྲིད (lhs->lhs_dest != dest_local && cmdidx != CMD_const
						&& cmdidx != CMD_final)
    {
	// Specific kind of variable recognized.
	declare_error = is_decl;
    }
    དེ་མིན
    {
	class_T	*defcl;

	// No specific kind of variable recognized, just a name.
	གལ་སྲིད (check_reserved_name(lhs->lhs_name, lhs->lhs_has_index
						&& *var_end == '.') == FAIL)
	    ལོག FAIL;

	གལ་སྲིད (lookup_local(var_start, lhs->lhs_varlen, &lhs->lhs_local_lvar,
								cctx) == OK)
	{
	    lhs->lhs_lvar = &lhs->lhs_local_lvar;
	}
	དེ་མིན
	{
	    CLEAR_FIELD(lhs->lhs_arg_lvar);
	    གལ་སྲིད (arg_exists(var_start, lhs->lhs_varlen,
			&lhs->lhs_arg_lvar.lv_idx, &lhs->lhs_arg_lvar.lv_type,
			&lhs->lhs_arg_lvar.lv_from_outer, cctx) == OK)
	    {
		གལ་སྲིད (is_decl)
		{
		    semsg(_(e_str_is_used_as_argument), lhs->lhs_name);
		    ལོག FAIL;
		}
		lhs->lhs_lvar = &lhs->lhs_arg_lvar;
	    }
	}

	གལ་སྲིད (lhs->lhs_lvar != NULL)
	{
	    གལ་སྲིད (is_decl)
	    {
		// if we come here with what looks like an assignment like
		// .= but which has been rejected by assignment_len() from
		// may_compile_assignment give a better error message
		char_u *p = skipwhite(lhs->lhs_end);
		གལ་སྲིད (p[0] == '.' && p[1] == '=')
		    emsg(_(e_dot_equal_not_supported_with_script_version_two));
		དེ་མིན གལ་སྲིད (p[0] == ':' && !has_cmd)
		    // type specified in an assignment without "var"
		    semsg(_(e_trailing_characters_str), p);
		དེ་མིན
		    semsg(_(e_variable_already_declared_str), lhs->lhs_name);
		ལོག FAIL;
	    }
	}
	དེ་མིན གལ་སྲིད ((lhs->lhs_classmember_idx = cctx_class_member_idx(
			cctx, var_start, lhs->lhs_varlen, &defcl)) >= 0)
	{
	    གལ་སྲིད (compile_lhs_class_variable(cctx, lhs, defcl, is_decl)
		    == FAIL)
		ལོག FAIL;
	}
	དེ་མིན
	{
	    གལ་སྲིད (compile_lhs_script_var(cctx, lhs, var_start, var_end,
			is_decl) == FAIL)
		ལོག FAIL;
	}
    }

    གལ་སྲིད (declare_error)
    {
	vim9_declare_error(lhs->lhs_name);
	ལོག FAIL;
    }

    ལོག OK;
}

/*
 * When compiling a LHS variable name, for a class or an object, set the LHS
 * member type.
 */
    static ཧྲིལ
compile_lhs_set_oc_member_type(
    cctx_T	*cctx,
    lhs_T	*lhs,
    char_u	*var_start)
{
    class_T	*cl = lhs->lhs_type->tt_class;
    ཧྲིལ		is_object = lhs->lhs_type->tt_type == VAR_OBJECT;
    char_u	*name = var_start + lhs->lhs_varlen + 1;
    size_t	namelen = lhs->lhs_end - var_start - lhs->lhs_varlen - 1;

    ocmember_T	*m = member_lookup(cl, lhs->lhs_type->tt_type,
	    name, namelen, &lhs->lhs_member_idx);
    གལ་སྲིད (m == NULL)
    {
	member_not_found_msg(cl, lhs->lhs_type->tt_type, name, namelen);
	ལོག FAIL;
    }

    གལ་སྲིད (IS_ENUM(cl))
    {
	གལ་སྲིད (!inside_class(cctx, cl))
	{
	    semsg(_(e_enumvalue_str_cannot_be_modified),
		    cl->class_name.string, m->ocm_name.string);
	    ལོག FAIL;
	}
	གལ་སྲིད (lhs->lhs_type->tt_type == VAR_OBJECT &&
		lhs->lhs_member_idx < 2)
	{
	    var_na *msg = lhs->lhs_member_idx == 0 ?
		e_enum_str_name_cannot_be_modified :
		e_enum_str_ordinal_cannot_be_modified;
	    semsg(_(msg), cl->class_name.string);
	    ལོག FAIL;
	}
    }

    // If it is private member variable, then accessing it outside the
    // class is not allowed.
    // If it is a read only class variable, then it can be modified
    // only inside the class where it is defined.
    གལ་སྲིད ((m->ocm_access != VIM_ACCESS_ALL) &&
	    ((is_object && !inside_class(cctx, cl))
	     || (!is_object && cctx->ctx_ufunc->uf_defclass != cl)))
    {
	var_na *msg = (m->ocm_access == VIM_ACCESS_PRIVATE)
	    ? e_cannot_access_protected_variable_str
	    : e_variable_is_not_writable_str;
	emsg_var_cl_define(msg, m->ocm_name.string, 0, cl);
	ལོག FAIL;
    }

    གལ་སྲིད (!IS_CONSTRUCTOR_METHOD(cctx->ctx_ufunc)
	    && oc_var_check_ro(cl, m))
	ལོག FAIL;

    lhs->lhs_member_type = m->ocm_type;

    ལོག OK;
}

/*
 * When compiling a LHS variable, set the LHS variable type.
 */
    static ཧྲིལ
compile_lhs_set_type(cctx_T *cctx, lhs_T *lhs, char_u *var_end, ཧྲིལ is_decl)
{
    གལ་སྲིད (is_decl && *skipwhite(var_end) == ':')
    {
	char_u *p;

	// parse optional type: "let var: type = expr"
	གལ་སྲིད (VIM_ISWHITE(*var_end))
	{
	    semsg(_(e_no_white_space_allowed_before_colon_str), var_end);
	    ལོག FAIL;
	}

	གལ་སྲིད (!VIM_ISWHITE(var_end[1]))
	{
	    semsg(_(e_white_space_required_after_str_str), ":", var_end);
	    ལོག FAIL;
	}

	p = skipwhite(var_end + 1);
	lhs->lhs_type = parse_type(&p, cctx->ctx_type_list, cctx->ctx_ufunc, cctx, TRUE);
	གལ་སྲིད (lhs->lhs_type == NULL
		|| !valid_declaration_type(lhs->lhs_type))
	    ལོག FAIL;

	lhs->lhs_has_type = TRUE;
	lhs->lhs_end = p;
    }
    དེ་མིན གལ་སྲིད (lhs->lhs_lvar != NULL)
	lhs->lhs_type = lhs->lhs_lvar->lv_type;

    ལོག OK;
}

/*
 * Returns TRUE if "lhs" is a concatenable string.
 */
    static ཧྲིལ
lhs_concatenable(lhs_T *lhs)
{
    ལོག lhs->lhs_dest == dest_global
		|| lhs->lhs_has_index
		|| lhs->lhs_type->tt_type == VAR_STRING
		|| lhs->lhs_type->tt_type == VAR_ANY;
}

/*
 * Create a new local variable when compiling a LHS variable.
 */
    static ཧྲིལ
compile_lhs_new_local_var(
    cctx_T	*cctx,
    lhs_T	*lhs,
    char_u	*var_start,
    ཧྲིལ		cmdidx,
    ཧྲིལ		oplen,
    ཧྲིལ		is_decl,
    ཧྲིལ		has_cmd,
    ཧྲིལ		heredoc)
{
    གལ་སྲིད (oplen > 1 && !heredoc)
    {
	// +=, /=, etc. require an existing variable
	semsg(_(e_cannot_use_operator_on_new_variable_str), lhs->lhs_name);
	ལོག FAIL;
    }

    གལ་སྲིད (!is_decl || (lhs->lhs_has_index && !has_cmd
					&& cctx->ctx_skip != SKIP_YES))
    {
	semsg(_(e_unknown_variable_str), lhs->lhs_name);
	ལོག FAIL;
    }

    // Check the name is valid for a funcref.
    གལ་སྲིད (lhs->lhs_type->tt_type == VAR_FUNC
				|| lhs->lhs_type->tt_type == VAR_PARTIAL)
    {
	གལ་སྲིད (var_wrong_func_name(lhs->lhs_name, TRUE))
	    ལོག FAIL;
    }

    // New local variable.
    ཧྲིལ assign;
    switch (cmdidx)
    {
	pak_sa CMD_final:
	    assign = ASSIGN_FINAL; bha_nga;
	pak_sa CMD_const:
	    assign = ASSIGN_CONST; bha_nga;
	default:
	    assign = ASSIGN_VAR; bha_nga;
    }

    lhs->lhs_lvar = reserve_local(cctx, var_start, lhs->lhs_varlen, assign,
							lhs->lhs_type);
    གལ་སྲིད (lhs->lhs_lvar == NULL)
	ལོག FAIL;

    lhs->lhs_new_local = TRUE;

    ལོག OK;
}

/*
 * When compiling a LHS variable name, set the LHS member type.
 */
    static ཧྲིལ
compile_lhs_set_member_type(
    cctx_T	*cctx,
    lhs_T	*lhs,
    char_u	*var_start,
    ཧྲིལ		is_decl,
    ཧྲིལ		has_cmd)
{
    lhs->lhs_member_type = lhs->lhs_type;

    གལ་སྲིད (!lhs->lhs_has_index)
	ལོག OK;

    char_u	*after = var_start + lhs->lhs_varlen;
    char_u	*p;

    // Something follows after the variable: "var[idx]" or "var.key".
    གལ་སྲིད (is_decl && cctx->ctx_skip != SKIP_YES)
    {
	གལ་སྲིད (has_cmd)
	    emsg(_(e_cannot_use_index_when_declaring_variable));
	དེ་མིན
	    semsg(_(e_unknown_variable_str), lhs->lhs_name);
	ལོག FAIL;
    }

    // Now: var_start[lhs->lhs_varlen] is '[' or '.'
    // Only the last index is used below, if there are others
    // before it generate code for the expression.  Thus for
    // "ll[1][2]" the expression is "ll[1]" and "[2]" is the index.
    དོན་ལུ (;;)
    {
	p = skip_index(after);
	གལ་སྲིད (*p != '[' && *p != '.')
	{
	    lhs->lhs_varlen_total = p - var_start;
	    bha_nga;
	}
	after = p;
    }
    གལ་སྲིད (after > var_start + lhs->lhs_varlen)
    {
	lhs->lhs_varlen = after - var_start;
	lhs->lhs_dest = dest_expr;
	// We don't know the type before evaluating the expression,
	// use "any" until then.  The member index is for the first name,
	// not for the last index.
	lhs->lhs_type = &t_any;
	lhs->lhs_member_idx = -1;
    }

    ཧྲིལ use_class = lhs->lhs_type != NULL
	&& (lhs->lhs_type->tt_type == VAR_CLASS
		|| lhs->lhs_type->tt_type == VAR_OBJECT);

    གལ་སྲིད (lhs->lhs_type == NULL
	    || (use_class ? lhs->lhs_type->tt_class == NULL
		: lhs->lhs_type->tt_member == NULL))
    {
	lhs->lhs_member_type = &t_any;
    }
    དེ་མིན གལ་སྲིད (use_class)
    {
	// for an object or class member get the type of the member
	གལ་སྲིད (compile_lhs_set_oc_member_type(cctx, lhs, var_start) == FAIL)
	    ལོག FAIL;
    }
    དེ་མིན
	lhs->lhs_member_type = lhs->lhs_type->tt_member;

    ལོག OK;
}

/*
 * Figure out the LHS type and other properties for an assignment or one item
 * of ":unlet" with an index.
 * Returns OK or FAIL.
 */
    ཧྲིལ
compile_lhs(
	char_u	    *var_start,
	lhs_T	    *lhs,
	cmdidx_T    cmdidx,
	ཧྲིལ	    heredoc,
	ཧྲིལ	    has_cmd,	    // "var" before "var_start"
	ཧྲིལ	    oplen,
	cctx_T	    *cctx)
{
    char_u	*var_end = NULL;
    ཧྲིལ		is_decl = is_decl_command(cmdidx);

    གལ་སྲིད (lhs_init(lhs, var_start, is_decl, heredoc, &var_end) == FAIL)
	ལོག FAIL;

    གལ་སྲིད (cctx->ctx_skip != SKIP_YES)
    {
	// compile the LHS destination
	གལ་སྲིད (compile_lhs_var_dest(cctx, lhs, cmdidx, var_start, var_end,
						is_decl, has_cmd) == FAIL)
	    ལོག FAIL;
    }

    // handle "a:name" as a name, not index "name" in "a"
    གལ་སྲིད (lhs->lhs_varlen > 1 || var_start[lhs->lhs_varlen] != ':')
	var_end = lhs->lhs_dest_end;

    གལ་སྲིད (lhs->lhs_dest != dest_option && lhs->lhs_dest != dest_func_option)
    {
	// set the LHS variable type
	གལ་སྲིད (compile_lhs_set_type(cctx, lhs, var_end, is_decl) == FAIL)
	    ལོག FAIL;
    }

    གལ་སྲིད (oplen == 3 && !heredoc && !lhs_concatenable(lhs))
    {
	emsg(_(e_can_only_concatenate_to_string));
	ལོག FAIL;
    }

    གལ་སྲིད (lhs->lhs_lvar == NULL && lhs->lhs_dest == dest_local
						&& cctx->ctx_skip != SKIP_YES)
    {
	གལ་སྲིད (compile_lhs_new_local_var(cctx, lhs, var_start, cmdidx, oplen,
					is_decl, has_cmd, heredoc) == FAIL)
	    ལོག FAIL;
    }

    གལ་སྲིད (compile_lhs_set_member_type(cctx, lhs, var_start, is_decl, has_cmd)
								== FAIL)
	ལོག FAIL;

    ལོག OK;
}

/*
 * Figure out the LHS and check a few errors.
 */
    ཧྲིལ
compile_assign_lhs(
	char_u	    *var_start,
	lhs_T	    *lhs,
	cmdidx_T    cmdidx,
	ཧྲིལ	    is_decl,
	ཧྲིལ	    heredoc,
	ཧྲིལ	    has_cmd,	    // "var" before "var_start"
	ཧྲིལ	    oplen,
	cctx_T	    *cctx)
{
    གལ་སྲིད (compile_lhs(var_start, lhs, cmdidx, heredoc, has_cmd, oplen, cctx)
								       == FAIL)
	ལོག FAIL;

    གལ་སྲིད (!lhs->lhs_has_index && lhs->lhs_lvar == &lhs->lhs_arg_lvar)
    {
	semsg(_(e_cannot_assign_to_argument_str), lhs->lhs_name);
	ལོག FAIL;
    }
    གལ་སྲིད (!is_decl && lhs->lhs_lvar != NULL
			   && lhs->lhs_lvar->lv_const != ASSIGN_VAR
			   && !lhs->lhs_has_index)
    {
	semsg(_(e_cannot_assign_to_constant_str), lhs->lhs_name);
	ལོག FAIL;
    }
    ལོག OK;
}

/*
 * Return TRUE if "lhs" has a range index: "[expr : expr]".
 */
    static ཧྲིལ
has_list_index(char_u *idx_start, cctx_T *cctx)
{
    char_u  *p = idx_start;
    ཧྲིལ	    save_skip;

    གལ་སྲིད (*p != '[')
	ལོག FALSE;

    p = skipwhite(p + 1);
    གལ་སྲིད (*p == ':')
	ལོག TRUE;

    save_skip = cctx->ctx_skip;
    cctx->ctx_skip = SKIP_YES;
    (void)compile_expr0(&p, cctx);
    cctx->ctx_skip = save_skip;
    ལོག *skipwhite(p) == ':';
}

/*
 * For an assignment with an index, compile the "idx" in "var[idx]" or "key" in
 * "var.key".
 */
    static ཧྲིལ
compile_assign_index(
	char_u	*var_start,
	lhs_T	*lhs,
	ཧྲིལ	*range,
	cctx_T	*cctx)
{
    size_t	varlen = lhs->lhs_varlen;
    char_u	*p;
    ཧྲིལ		r = OK;
    ཧྲིལ		need_white_before = TRUE;
    ཧྲིལ		empty_second;

    p = var_start + varlen;
    གལ་སྲིད (*p == '[')
    {
	p = skipwhite(p + 1);
	གལ་སྲིད (*p == ':')
	{
	    // empty first index, push zero
	    r = generate_PUSHNR(cctx, 0);
	    need_white_before = FALSE;
	}
	དེ་མིན
	    r = compile_expr0(&p, cctx);

	གལ་སྲིད (r == OK && *skipwhite(p) == ':')
	{
	    // unlet var[idx : idx]
	    // blob[idx : idx] = value
	    *range = TRUE;
	    p = skipwhite(p);
	    empty_second = *skipwhite(p + 1) == ']';
	    གལ་སྲིད ((need_white_before && !IS_WHITE_OR_NUL(p[-1]))
		    || (!empty_second && !IS_WHITE_OR_NUL(p[1])))
	    {
		semsg(_(e_white_space_required_before_and_after_str_at_str),
								      ":", p);
		ལོག FAIL;
	    }
	    p = skipwhite(p + 1);
	    གལ་སྲིད (*p == ']')
		// empty second index, push "none"
		r = generate_PUSHSPEC(cctx, VVAL_NONE);
	    དེ་མིན
		r = compile_expr0(&p, cctx);
	}

	གལ་སྲིད (r == OK && *skipwhite(p) != ']')
	{
	    // this should not happen
	    emsg(_(e_missing_closing_square_brace));
	    r = FAIL;
	}
    }
    དེ་མིན གལ་སྲིད (lhs->lhs_member_idx >= 0)
    {
	// object member index
	r = generate_PUSHNR(cctx, lhs->lhs_member_idx);
    }
    དེ་མིན // if (*p == '.')
    {
	char_u *key_end = to_name_end(p + 1, TRUE);
	char_u *key = vim_strnsave(p + 1, key_end - p - 1);

	r = generate_PUSHS(cctx, &key);
    }
    ལོག r;
}

/*
 * For a LHS with an index, load the variable to be indexed.
 */
    static ཧྲིལ
compile_load_lhs(
	lhs_T	*lhs,
	char_u	*var_start,
	type_T	*rhs_type,
	cctx_T	*cctx)
{
    གལ་སྲིད (lhs->lhs_dest == dest_expr)
    {
	size_t	    varlen = lhs->lhs_varlen;
	ཧྲིལ	    c = var_start[varlen];
	ཧྲིལ	    lines_len = cctx->ctx_ufunc->uf_lines.ga_len;
	ཧྲིལ	    res;

	// Evaluate "ll[expr]" of "ll[expr][idx]".  End the line with a NUL and
	// limit the lines array length to avoid skipping to a following line.
	var_start[varlen] = NUL;
	cctx->ctx_ufunc->uf_lines.ga_len = cctx->ctx_lnum + 1;
	char_u *p = var_start;
	res = compile_expr0(&p, cctx);
	var_start[varlen] = c;
	cctx->ctx_ufunc->uf_lines.ga_len = lines_len;
	གལ་སྲིད (res == FAIL || p != var_start + varlen)
	{
	    // this should not happen
	    གལ་སྲིད (res != FAIL)
		emsg(_(e_missing_closing_square_brace));
	    ལོག FAIL;
	}

	lhs->lhs_type = cctx->ctx_type_stack.ga_len == 0 ? &t_void
						  : get_type_on_stack(cctx, 0);

	གལ་སྲིད (lhs->lhs_type->tt_type == VAR_CLASS
		|| (lhs->lhs_type->tt_type == VAR_OBJECT
		    && lhs->lhs_type != &t_object_any))
	{
	    // Check whether the class or object variable is modifiable
	    གལ་སྲིད (!lhs_class_member_modifiable(lhs, var_start, cctx))
		ལོག FAIL;
	}

	// Now we can properly check the type.  The variable is indexed, thus
	// we need the member type.  For a class or object we don't know the
	// type yet, it depends on what member is used.
	// The top item in the stack is the Dict, followed by the key and then
	// the type of the value.
	vartype_T vartype = lhs->lhs_type->tt_type;
	type_T *member_type = lhs->lhs_type->tt_member;
	གལ་སྲིད (rhs_type != NULL && member_type != NULL
		&& vartype != VAR_OBJECT && vartype != VAR_CLASS
		&& rhs_type != &t_void
		&& need_type(rhs_type, member_type, 0,
					    -3, 0, cctx, FALSE, FALSE) == FAIL)
	    ལོག FAIL;

	ལོག OK;
    }

    ལོག generate_loadvar(cctx, lhs);
}

/*
 * Produce code for loading "lhs" and also take care of an index.
 * Return OK/FAIL.
 */
    ཧྲིལ
compile_load_lhs_with_index(lhs_T *lhs, char_u *var_start, cctx_T *cctx)
{
    གལ་སྲིད (lhs->lhs_type->tt_type == VAR_OBJECT)
    {
	// "this.value": load "this" object and get the value at index for an
	// object or class member get the type of the member.
	// Also for "obj.value".
	char_u *dot = vim_strchr(var_start, '.');
	གལ་སྲིད (dot == NULL)
	{
	    semsg(_(e_missing_dot_after_object_str), lhs->lhs_name);
	    ལོག FAIL;
	}

	class_T	*cl = lhs->lhs_type->tt_class;
	type_T	*type = oc_member_type(cl, TRUE, dot + 1,
					  lhs->lhs_end, &lhs->lhs_member_idx);
	གལ་སྲིད (lhs->lhs_member_idx < 0)
	    ལོག FAIL;

	གལ་སྲིད (dot - var_start == 4 && STRNCMP(var_start, "this", 4) == 0)
	{
	    // load "this"
	    lvar_T  *lvar = lhs->lhs_lvar;
	    ཧྲིལ	    rc;

	    གལ་སྲིད (lvar->lv_from_outer > 0)
		rc = generate_LOADOUTER(cctx, lvar->lv_idx,
			lvar->lv_from_outer, lvar->lv_loop_depth,
			lvar->lv_loop_idx, type);
	    དེ་མིན
		rc = generate_LOAD(cctx, ISN_LOAD, lvar->lv_idx, NULL, type);

	    གལ་སྲིད (rc == FAIL)
		ལོག FAIL;
	}
	དེ་མིན
	{
	    // load object variable or argument
	    གལ་སྲིད (compile_load_lhs(lhs, var_start, lhs->lhs_type, cctx) == FAIL)
		ལོག FAIL;
	}
	གལ་སྲིད (IS_INTERFACE(cl))
	    ལོག generate_GET_ITF_MEMBER(cctx, cl, lhs->lhs_member_idx, type);
	ལོག generate_GET_OBJ_MEMBER(cctx, lhs->lhs_member_idx, type);
    }
    དེ་མིན གལ་སྲིད (lhs->lhs_type->tt_type == VAR_CLASS)
    {
	// "<classname>.value": load class variable "classname.value"
	char_u *dot = vim_strchr(var_start, '.');
	གལ་སྲིད (dot == NULL)
	{
	    (void)check_type_is_value(lhs->lhs_type);
	    ལོག FAIL;
	}

	class_T	*cl = lhs->lhs_type->tt_class;
	ocmember_T *m = class_member_lookup(cl, dot + 1,
						lhs->lhs_end - dot - 1,
						&lhs->lhs_member_idx);
	གལ་སྲིད (m == NULL)
	    ལོག FAIL;

	ལོག generate_CLASSMEMBER(cctx, TRUE, cl, lhs->lhs_member_idx);
    }

    གལ་སྲིད (compile_load_lhs(lhs, var_start, NULL, cctx) == FAIL)
	ལོག FAIL;

    གལ་སྲིད (lhs->lhs_has_index)
    {
	ཧྲིལ	range = FALSE;
	type_T	*type = get_type_on_stack(cctx, 0);

	// A member of an object or class is not obtained by indexing it.
	གལ་སྲིད (type->tt_type == VAR_CLASS
		|| (type->tt_type == VAR_OBJECT && type != &t_object_any))
	{
	    char_u *p = var_start + lhs->lhs_varlen;

	    ལོག compile_class_object_index(cctx, &p, type);
	}

	// Get member from list or dict.  First compile the
	// index value.
	གལ་སྲིད (compile_assign_index(var_start, lhs, &range, cctx) == FAIL)
	    ལོག FAIL;
	གལ་སྲིད (range)
	{
	    semsg(_(e_cannot_use_range_with_assignment_operator_str),
								    var_start);
	    ལོག FAIL;
	}

	// Get the member.
	གལ་སྲིད (compile_member(FALSE, NULL, cctx) == FAIL)
	    ལོག FAIL;
    }
    ལོག OK;
}

/*
 * Assignment to a list or dict member, or ":unlet" for the item, using the
 * information in "lhs".
 * Returns OK or FAIL.
 */
    ཧྲིལ
compile_assign_unlet(
    char_u	*var_start,
    lhs_T	*lhs,
    ཧྲིལ		is_assign,
    type_T	*rhs_type,
    cctx_T	*cctx)
{
    vartype_T	dest_type;
    ཧྲིལ		range = FALSE;

    གལ་སྲིད (compile_assign_index(var_start, lhs, &range, cctx) == FAIL)
	ལོག FAIL;
    གལ་སྲིད (is_assign && range
	    && lhs->lhs_type->tt_type != VAR_LIST
	    && lhs->lhs_type != &t_blob
	    && lhs->lhs_type != &t_any)
    {
	གལ་སྲིད (lhs->lhs_type->tt_type == VAR_TUPLE)
	    emsg(_(e_cannot_slice_tuple));
	དེ་མིན
	    semsg(_(e_cannot_use_range_with_assignment_str), var_start);
	ལོག FAIL;
    }

    // For "expr[idx]" the index is compiled before the expression, thus the
    // resulting type cannot be used here.
    གལ་སྲིད (lhs->lhs_dest == dest_expr
	    || lhs->lhs_type == NULL || lhs->lhs_type == &t_any)
    {
	// Index on variable of unknown type: check at runtime.
	dest_type = VAR_ANY;
    }
    དེ་མིན
    {
	dest_type = lhs->lhs_type->tt_type;
	གལ་སྲིད (dest_type == VAR_DICT && range)
	{
	    emsg(_(e_cannot_use_range_with_dictionary));
	    ལོག FAIL;
	}
	གལ་སྲིད (dest_type == VAR_DICT
		&& may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
	    ལོག FAIL;
	གལ་སྲིད (dest_type == VAR_LIST || dest_type == VAR_BLOB)
	{
	    type_T *type;

	    གལ་སྲིད (range)
	    {
		type = get_type_on_stack(cctx, 1);
		གལ་སྲིད (need_type(type, &t_number, 0,
					    -2, 0, cctx, FALSE, FALSE) == FAIL)
		ལོག FAIL;
	    }
	    type = get_type_on_stack(cctx, 0);
	    གལ་སྲིད ((dest_type != VAR_BLOB && type->tt_type != VAR_SPECIAL)
		    && need_type(type, &t_number, 0,
					    -1, 0, cctx, FALSE, FALSE) == FAIL)
		ལོག FAIL;
	}
    }

    གལ་སྲིད (cctx->ctx_skip == SKIP_YES)
	ལོག OK;

    // Load the dict, list or object.  On the stack we then have:
    // - value (for assignment, not for :unlet)
    // - index
    // - for [a : b] second index
    // - variable
    གལ་སྲིད (compile_load_lhs(lhs, var_start, rhs_type, cctx) == FAIL)
	ལོག FAIL;

    གལ་སྲིད (dest_type == VAR_LIST
	    || dest_type == VAR_DICT
	    || dest_type == VAR_BLOB
	    || dest_type == VAR_CLASS
	    || dest_type == VAR_OBJECT
	    || dest_type == VAR_ANY)
    {
	གལ་སྲིད (is_assign)
	{
	    གལ་སྲིད (range)
	    {
		གལ་སྲིད (generate_instr_drop(cctx, ISN_STORERANGE, 4) == NULL)
		    ལོག FAIL;
	    }
	    དེ་མིན
	    {
		isn_T	*isn = generate_instr_drop(cctx, ISN_STOREINDEX, 3);

		གལ་སྲིད (isn == NULL)
		    ལོག FAIL;
		isn->isn_arg.storeindex.si_vartype = dest_type;
		isn->isn_arg.storeindex.si_class = NULL;

		གལ་སྲིད (dest_type == VAR_OBJECT)
		{
		    class_T *cl = lhs->lhs_type->tt_class;

		    གལ་སྲིད (IS_INTERFACE(cl))
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
	དེ་མིན གལ་སྲིད (range)
	{
	    གལ་སྲིད (generate_instr_drop(cctx, ISN_UNLETRANGE, 3) == NULL)
		ལོག FAIL;
	}
	དེ་མིན
	{
	    གལ་སྲིད (generate_instr_drop(cctx, ISN_UNLETINDEX, 2) == NULL)
		ལོག FAIL;
	}
    }
    དེ་མིན
    {
	གལ་སྲིད (dest_type == VAR_TUPLE)
	    emsg(_(e_tuple_is_immutable));
	དེ་མིན
	    emsg(_(e_indexable_type_required));
	ལོག FAIL;
    }

    ལོག OK;
}

/*
 * Generate an instruction to push the default value for "vartype".
 * if "dest_local" is TRUE then for some types no instruction is generated.
 * "skip_store" is set to TRUE if no PUSH instruction is generated.
 * Returns OK or FAIL.
 */
    static ཧྲིལ
push_default_value(
	cctx_T	    *cctx,
	vartype_T   vartype,
	ཧྲིལ	    dest_is_local,
	ཧྲིལ	    *skip_store)
{
    ཧྲིལ r = OK;

    switch (vartype)
    {
	pak_sa VAR_BOOL:
	    r = generate_PUSHBOOL(cctx, VVAL_FALSE);
	    bha_nga;
	pak_sa VAR_FLOAT:
	    r = generate_PUSHF(cctx, 0.0);
	    bha_nga;
	pak_sa VAR_STRING:
	    r = generate_PUSHS(cctx, NULL);
	    bha_nga;
	pak_sa VAR_BLOB:
	    r = generate_PUSHBLOB(cctx, blob_alloc());
	    bha_nga;
	pak_sa VAR_FUNC:
	    r = generate_PUSHFUNC(cctx, NULL, &t_func_void, TRUE);
	    bha_nga;
	pak_sa VAR_LIST:
	    r = generate_NEWLIST(cctx, 0, FALSE);
	    bha_nga;
	pak_sa VAR_TUPLE:
	    r = generate_NEWTUPLE(cctx, 0, FALSE);
	    bha_nga;
	pak_sa VAR_DICT:
	    r = generate_NEWDICT(cctx, 0, FALSE);
	    bha_nga;
	pak_sa VAR_JOB:
	    r = generate_PUSHJOB(cctx);
	    bha_nga;
	pak_sa VAR_CHANNEL:
	    r = generate_PUSHCHANNEL(cctx);
	    bha_nga;
	pak_sa VAR_OBJECT:
	    r = generate_PUSHOBJ(cctx);
	    bha_nga;
	pak_sa VAR_NUMBER:
	pak_sa VAR_UNKNOWN:
	pak_sa VAR_ANY:
	pak_sa VAR_PARTIAL:
	pak_sa VAR_VOID:
	pak_sa VAR_INSTR:
	pak_sa VAR_CLASS:
	pak_sa VAR_TYPEALIAS:
	pak_sa VAR_SPECIAL:  // cannot happen
	    // This is skipped for local variables, they are always
	    // initialized to zero.  But in a "for" or "while" loop
	    // the value may have been changed.
	    གལ་སྲིད (dest_is_local && !inside_loop_scope(cctx))
		*skip_store = TRUE;
	    དེ་མིན
		r = generate_PUSHNR(cctx, 0);
	    bha_nga;
    }
    ལོག r;
}

/*
 * Compile assignment context.  Used when compiling an assignment statement.
 */
typedef sa_nracanaa cac_S cac_T;
sa_nracanaa cac_S
{
    cmdidx_T	cac_cmdidx;		// assignment command
    char_u	*cac_nextc;		// next character to parse
    lhs_T	cac_lhs;		// lhs of the assignment
    type_T	*cac_rhs_type;		// rhs type of an assignment
    char_u	*cac_op;		// assignment operator
    ཧྲིལ		cac_oplen;		// assignment operator length
    char_u	*cac_var_start;		// start of the variable names
    char_u	*cac_var_end;		// end of the variable names
    ཧྲིལ		cac_var_count;		// number of variables in assignment
    ཧྲིལ		cac_var_idx;		// variable index in a list
    ཧྲིལ		cac_semicolon;		// semicolon in [var1, var2; var3]
    garray_T	*cac_instr;
    ཧྲིལ		cac_instr_count;
    ཧྲིལ		cac_incdec;
    ཧྲིལ		cac_did_generate_slice;
    ཧྲིལ		cac_is_decl;
    ཧྲིལ		cac_is_const;
    ཧྲིལ		cac_start_lnum;
    type_T	*cac_inferred_type;
    ཧྲིལ		cac_skip_store;
};

/*
 * Initialize the compile assignment context.
 */
    static void
compile_assign_context_init(cac_T *cac, cctx_T *cctx, ཧྲིལ cmdidx, char_u *arg)
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
    static ཧྲིལ
compile_assign_obj_new_arg(char_u **argp, cctx_T *cctx)
{
    char_u *arg = *argp;

    arg += 11;	    // skip "ifargisset"
    ཧྲིལ def_arg_idx = getdigits(&arg);
    arg = skipwhite(arg);

    // Use a JUMP_IF_ARG_NOT_SET instruction to skip if the value was not
    // given and the default value is "v:none".
    ཧྲིལ stack_offset = STACK_FRAME_SIZE +
				(cctx->ctx_ufunc->uf_va_name != NULL ? 1 : 0);
    ཧྲིལ def_arg_count = cctx->ctx_ufunc->uf_def_args.ga_len;
    ཧྲིལ arg_offset = def_arg_idx - def_arg_count - stack_offset;

    གལ་སྲིད (generate_JUMP_IF_ARG(cctx, ISN_JUMP_IF_ARG_NOT_SET,
			     arg_offset) == FAIL)
	ལོག FAIL;

    *argp = arg;
    ལོག OK;
}

/*
 * Translate the increment (++) and decrement (--) operators to the
 * corresponding compound operators (+= or -=).
 *
 * Returns OK on success and FAIL on syntax error.
 */
    static ཧྲིལ
translate_incdec_op(exarg_T *eap, cac_T *cac)
{
    གལ་སྲིད (VIM_ISWHITE(eap->cmd[2]))
    {
	semsg(_(e_no_white_space_allowed_after_str_str),
		eap->cmdidx == CMD_increment ? "++" : "--", eap->cmd);
	ལོག FAIL;
    }
    cac->cac_op = (char_u *)(eap->cmdidx == CMD_increment ? "+=" : "-=");
    cac->cac_oplen = 2;
    cac->cac_incdec = TRUE;

    ལོག OK;
}

/*
 * Process the operator in an assignment statement.
 */
    static ཧྲིལ
compile_assign_process_operator(
    exarg_T	*eap,
    char_u	*arg,
    cac_T	*cac,
    ཧྲིལ		*heredoc,
    char_u	**retstr)
{
    *retstr = NULL;

    གལ་སྲིད (eap->cmdidx == CMD_increment || eap->cmdidx == CMD_decrement)
	// Change an unary operator to a compound operator
	ལོག translate_incdec_op(eap, cac);

    char_u *sp = cac->cac_nextc;
    cac->cac_nextc = skipwhite(cac->cac_nextc);
    cac->cac_op = cac->cac_nextc;
    cac->cac_oplen = assignment_len(cac->cac_nextc, heredoc);

    གལ་སྲིད (cac->cac_var_count > 0 && cac->cac_oplen == 0)
    {
	// can be something like "[1, 2]->func()"
	*retstr = arg;
	ལོག FAIL;
    }

    // need white space before and after the operator
    གལ་སྲིད (cac->cac_oplen > 0 && (!VIM_ISWHITE(*sp)
		|| !IS_WHITE_OR_NUL(cac->cac_op[cac->cac_oplen])))
    {
	error_white_both(cac->cac_op, cac->cac_oplen);
	ལོག FAIL;
    }

    ལོག OK;
}

/*
 * Find the start of an assignment statement.
 */
    static char_u *
compile_assign_compute_start(char_u *arg, ཧྲིལ var_count)
{
    གལ་སྲིད (var_count > 0)
	// [var1, var2] = [val1, val2]
	// skip over the "["
	ལོག skipwhite(arg + 1);

    ལོག arg;
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
    གལ་སྲིད (l == NULL)
	ལོག NULL;

    list_free(l);
    cac->cac_nextc += STRLEN(cac->cac_nextc);

    ལོག cac->cac_nextc;
}

/*
 * Check the type of a RHS expression in a list assignment statement.
 * The RHS expression is already compiled.  So the type is on the stack.
 */
    static ཧྲིལ
compile_assign_list_check_rhs_type(cctx_T *cctx, cac_T *cac)
{
    type_T	*stacktype;

    stacktype = cctx->ctx_type_stack.ga_len == 0 ? &t_void
						: get_type_on_stack(cctx, 0);
    གལ་སྲིད (stacktype->tt_type == VAR_VOID)
    {
	emsg(_(e_cannot_use_void_value));
	ལོག FAIL;
    }

    གལ་སྲིད (stacktype->tt_type != VAR_LIST && stacktype->tt_type != VAR_TUPLE
					&& stacktype->tt_type != VAR_ANY)
    {
	emsg(_(e_list_or_tuple_required));
	ལོག FAIL;
    }

    གལ་སྲིད (need_type(stacktype,
		  stacktype->tt_type == VAR_TUPLE ? &t_tuple_any : &t_list_any,
		  TYPECHK_TUPLE_OK, -1, 0, cctx, FALSE, FALSE) == FAIL)
	ལོག FAIL;

    // The check accepts both a list and a tuple. Keep "any", making it a list
    // would reject a tuple later on.
    གལ་སྲིད (stacktype->tt_type == VAR_ANY)
	set_type_on_stack(cctx, &t_any, 0);

    གལ་སྲིད (stacktype->tt_type == VAR_TUPLE)
    {
	གལ་སྲིད (stacktype->tt_argcount != 1)
	    cac->cac_rhs_type = &t_any;
	དེ་མིན
	{
	    གལ་སྲིད (stacktype->tt_flags & TTFLAG_VARARGS)
		cac->cac_rhs_type = stacktype->tt_args[0]->tt_member;
	    དེ་མིན
		cac->cac_rhs_type = stacktype->tt_args[0];
	}
    }
    དེ་མིན གལ་སྲིད (stacktype->tt_member != NULL)
	cac->cac_rhs_type = stacktype->tt_member;

    ལོག OK;
}

/*
 * In a list assignment statement, if a constant list was used, check the
 * length.  Returns OK if the length check succeeds.  Returns FAIL otherwise.
 */
    static ཧྲིལ
compile_assign_list_check_length(cctx_T *cctx, cac_T *cac)
{
    ཧྲིལ	needed_list_len;
    ཧྲིལ	did_check = FALSE;

    needed_list_len = cac->cac_semicolon
				? cac->cac_var_count - 1
				: cac->cac_var_count;
    གལ་སྲིད (cac->cac_instr->ga_len > 0)
    {
	isn_T	*isn = ((isn_T *)cac->cac_instr->ga_data) +
						cac->cac_instr->ga_len - 1;

	གལ་སྲིད (isn->isn_type == ISN_NEWLIST || isn->isn_type == ISN_NEWTUPLE)
	{
	    did_check = TRUE;
	    གལ་སྲིད (cac->cac_semicolon ?
			isn->isn_arg.number < needed_list_len
			: isn->isn_arg.number != needed_list_len)
	    {
		semsg(_(e_expected_nr_items_but_got_nr),
			needed_list_len, (ཧྲིལ)isn->isn_arg.number);
		ལོག FAIL;
	    }
	}
    }

    གལ་སྲིད (!did_check)
	generate_CHECKLEN(cctx, needed_list_len, cac->cac_semicolon);

    ལོག OK;
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

    གལ་སྲིད (may_get_next_line_error(whitep, &cac->cac_nextc, cctx) == FAIL)
	ལོག NULL;

    // compile RHS expression
    གལ་སྲིད (compile_expr0(&cac->cac_nextc, cctx) == FAIL)
	ལོག NULL;

    གལ་སྲིད (cctx->ctx_skip == SKIP_YES)
	// no need to parse more when skipping
	ལོག cac->cac_nextc;

    གལ་སྲིད (compile_assign_list_check_rhs_type(cctx, cac) == FAIL)
	ལོག NULL;

    // If a constant list was used we can check the length right here.
    གལ་སྲིད (compile_assign_list_check_length(cctx, cac) == FAIL)
	ལོག FAIL;

    ལོག cac->cac_nextc;
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
    ཧྲིལ		heredoc)
{
    གལ་སྲིད (heredoc)
    {
	cac->cac_nextc = parse_heredoc_assignment(eap, cctx, cac);
	ལོག cac->cac_nextc;
    }

    གལ་སྲིད (cac->cac_var_count > 0)
    {
	// for "[var, var] = expr" evaluate the expression. The list of
	// variables are processed later.
	// A line break may follow the "=".
	cac->cac_nextc = compile_assign_list_expr(cctx, cac);
	ལོག cac->cac_nextc;
    }

    ལོག cac->cac_var_end;
}

/*
 * For "var = expr" evaluate the expression.
 */
    static ཧྲིལ
compile_assign_single_eval_expr(cctx_T *cctx, cac_T *cac)
{
    ཧྲིལ		ret = OK;
    char_u	*whitep;
    lhs_T	*lhs = &cac->cac_lhs;
    lvar_T	*lvp;
    lvar_T	save_lhs_lvar;

    // Compile the expression.
    གལ་སྲིད (cac->cac_incdec)
	ལོག generate_PUSHNR(cctx, 1);

    // Temporarily hide the new local variable here, it is
    // not available to this expression.
    གལ་སྲིད (lhs->lhs_new_local)
    {
	--cctx->ctx_locals.ga_len;

	// Save the local variable value (compiling the RHS expression may
	// create new local variables).
	lvp = ((lvar_T *)cctx->ctx_locals.ga_data) + cctx->ctx_locals.ga_len;
	save_lhs_lvar = *lvp;
    }
    whitep = cac->cac_op + cac->cac_oplen;

    གལ་སྲིད (may_get_next_line_error(whitep, &cac->cac_nextc, cctx) == FAIL)
    {
	གལ་སྲིད (lhs->lhs_new_local)
	    ++cctx->ctx_locals.ga_len;
	ལོག FAIL;
    }

    ret = compile_expr0_ext(&cac->cac_nextc, cctx, &cac->cac_is_const);
    གལ་སྲིད (lhs->lhs_new_local)
    {
	// Restore the local variable value.  Update lhs_lvar as the index of
	// the local variable might have changed.
	lvp = ((lvar_T *)cctx->ctx_locals.ga_data) + cctx->ctx_locals.ga_len;
	*lvp = save_lhs_lvar;
	lhs->lhs_lvar = lvp;

	++cctx->ctx_locals.ga_len;
    }

    ལོག ret;
}

/*
 * When compiling an assignment, set the LHS type to the RHS type.
 */
    static ཧྲིལ
compile_assign_set_lhs_type_from_rhs(
    cctx_T	*cctx,
    cac_T	*cac,
    lhs_T	*lhs,
    type_T	*rhs_type)
{
    གལ་སྲིད (rhs_type->tt_type == VAR_VOID)
    {
	emsg(_(e_cannot_use_void_value));
	ལོག FAIL;
    }

    type_T *type;

    // An empty list or dict has a &t_unknown member, for a variable that
    // implies &t_any.
    གལ་སྲིད (rhs_type == &t_list_empty)
	type = &t_list_any;
    དེ་མིན གལ་སྲིད (rhs_type == &t_dict_empty)
	type = &t_dict_any;
    དེ་མིན གལ་སྲིད (rhs_type == &t_unknown)
	type = &t_any;
    དེ་མིན
    {
	type = rhs_type;
	cac->cac_inferred_type = rhs_type;
    }

    set_var_type(lhs->lhs_lvar, type, cctx);

    ལོག OK;
}

/*
 * Returns TRUE if the "rhs_type" can be assigned to the "lhs" variable.
 * Used when compiling an assignment statement.
 */
    static ཧྲིལ
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
    གལ་སྲིད (cac->cac_var_count > 0)
    {
	where.wt_index = cac->cac_var_idx + 1;
	where.wt_kind = WT_VARIABLE;
    }

    // If assigning to a list or dict member, use the member type.
    // Not for "list[:] =".
    གལ་སྲིད (lhs->lhs_has_index &&
	    !has_list_index(cac->cac_var_start + lhs->lhs_varlen, cctx))
	use_type = lhs->lhs_member_type;

    གལ་སྲིད (need_type_where(rhs_type, use_type, 0, -1, where, cctx, FALSE,
						cac->cac_is_const) == FAIL)
	ལོག FALSE;

    ལོག TRUE;
}

/*
 * Compare the LHS type with the RHS type in an assignment.
 */
    static ཧྲིལ
compile_assign_check_type(cctx_T *cctx, cac_T *cac)
{
    lhs_T	*lhs = &cac->cac_lhs;
    type_T	*rhs_type;

    rhs_type = cctx->ctx_type_stack.ga_len == 0
					? &t_void
					: get_type_on_stack(cctx, 0);
    cac->cac_rhs_type = rhs_type;

    གལ་སྲིད (check_type_is_value(rhs_type) == FAIL)
	ལོག FAIL;

    གལ་སྲིད (lhs->lhs_lvar != NULL && (cac->cac_is_decl || !lhs->lhs_has_type))
    {
	གལ་སྲིད (rhs_type->tt_type == VAR_FUNC
					|| rhs_type->tt_type == VAR_PARTIAL)
	{
	    // Make sure the variable name can be used as a funcref
	    གལ་སྲིད (!lhs->lhs_has_index
				&& var_wrong_func_name(lhs->lhs_name, TRUE))
		ལོག FAIL;
	}

	གལ་སྲིད (lhs->lhs_new_local && !lhs->lhs_has_type)
	{
	    // The LHS variable doesn't have a type.  Set it to the RHS type.
	    གལ་སྲིད (compile_assign_set_lhs_type_from_rhs(cctx, cac, lhs, rhs_type)
								== FAIL)
		ལོག FAIL;
	}
	དེ་མིན གལ་སྲིད (*cac->cac_op == '=')
	{
	    གལ་སྲིད (!compile_assign_valid_rhs_type(cctx, cac, lhs, rhs_type))
		ལོག FAIL;
	}
    }
    དེ་མིན
    {
	// Assigning to a register using @r = "abc"

	type_T *lhs_type = lhs->lhs_member_type;

	// Special case: assigning to @# can use a number or a string.
	// Also: can assign a number to a float.
	གལ་སྲིད ((lhs_type == &t_number_or_string || lhs_type == &t_float)
					&& rhs_type->tt_type == VAR_NUMBER)
	    lhs_type = &t_number;

	གལ་སྲིད (*cac->cac_nextc != '=')
	{
	    གལ་སྲིད (need_type(rhs_type, lhs_type, 0, -1, 0, cctx, FALSE,
							FALSE) == FAIL)
		ལོག FAIL;
	}
    }

    ལོག OK;
}

/*
 * Compile the RHS expression in an assignment statement and generate the
 * instructions.
 */
    static ཧྲིལ
compile_assign_rhs_expr(cctx_T *cctx, cac_T *cac)
{
    cac->cac_is_const = FALSE;

    // for "+=", "*=", "..=" etc. first load the current value
    གལ་སྲིད (*cac->cac_op != '='
	    && compile_load_lhs_with_index(&cac->cac_lhs, cac->cac_var_start,
								cctx) == FAIL)
	ལོག FAIL;

    // For "var = expr" evaluate the expression.
    གལ་སྲིད (cac->cac_var_count == 0)
    {
	ཧྲིལ	ret;

	// Compile the expression.
	cac->cac_instr_count = cac->cac_instr->ga_len;
	ret = compile_assign_single_eval_expr(cctx, cac);
	གལ་སྲིད (ret == FAIL)
	    ལོག FAIL;
    }
    དེ་མིན གལ་སྲིད (cac->cac_semicolon && cac->cac_var_idx == cac->cac_var_count - 1)
    {
	// For "[var; var] = expr" get the rest of the list
	cac->cac_did_generate_slice = TRUE;
	གལ་སྲིད (generate_SLICE(cctx, cac->cac_var_count - 1) == FAIL)
	    ལོག FAIL;
    }
    དེ་མིན
    {
	// For "[var, var] = expr" get the "var_idx" item from the
	// list.
	ཧྲིལ with_op = *cac->cac_op != '=';
	གལ་སྲིད (generate_GETITEM(cctx, cac->cac_var_idx, with_op) == FAIL)
	    ལོག FAIL;
    }

    གལ་སྲིད (compile_assign_check_type(cctx, cac) == FAIL)
	ལོག FAIL;

    ལོག OK;
}

/*
 * Compile the RHS expression in an assignment
 */
    static ཧྲིལ
compile_assign_rhs(cctx_T *cctx, cac_T *cac)
{
    lhs_T	*lhs = &cac->cac_lhs;

    གལ་སྲིད (cctx->ctx_skip == SKIP_YES)
    {
	གལ་སྲིད (cac->cac_oplen > 0 && cac->cac_var_count == 0)
	{
	    // skip over the "=" and the expression
	    cac->cac_nextc = skipwhite(cac->cac_op + cac->cac_oplen);
	    (void)compile_expr0(&cac->cac_nextc, cctx);
	}
	ལོག OK;
    }

    // If RHS is specified, then generate instructions for RHS expression
    གལ་སྲིད (cac->cac_oplen > 0)
	ལོག compile_assign_rhs_expr(cctx, cac);

    གལ་སྲིད (cac->cac_cmdidx == CMD_final)
    {
	emsg(_(e_final_requires_a_value));
	ལོག FAIL;
    }

    གལ་སྲིད (cac->cac_cmdidx == CMD_const)
    {
	emsg(_(e_const_requires_a_value));
	ལོག FAIL;
    }

    གལ་སྲིད (!lhs->lhs_has_type || lhs->lhs_dest == dest_option
					|| lhs->lhs_dest == dest_func_option)
    {
	emsg(_(e_type_or_initialization_required));
	ལོག FAIL;
    }

    // variables are always initialized
    གལ་སྲིད (GA_GROW_FAILS(cac->cac_instr, 1))
	ལོག FAIL;

    cac->cac_instr_count = cac->cac_instr->ga_len;

    ལོག push_default_value(cctx, lhs->lhs_member_type->tt_type,
			      lhs->lhs_dest == dest_local,
			      &cac->cac_skip_store);
}

/*
 * Returns OK if "type" supports compound operator "op_arg" (e.g. +=, -=, %=,
 * etc.).  Compound operators are not supported with a tuple and a dict.
 * Returns FAIL if compound operator is not supported.
 */
    static ཧྲིལ
check_type_supports_compound_op(type_T *type, char_u op_arg)
{
    གལ་སྲིད (type->tt_type == VAR_TUPLE || type->tt_type == VAR_DICT)
    {
	char_u	op[2];

	op[0] = op_arg;
	op[1] = NUL;
	semsg(_(e_wrong_variable_type_for_str_equal), op);
	ལོག FAIL;
    }

    ལོག OK;
}

/*
 * Compile a compound op assignment statement (+=, -=, *=, %=, etc.)
 */
    static ཧྲིལ
compile_assign_compound_op(cctx_T *cctx, cac_T *cac)
{
    lhs_T	    *lhs = &cac->cac_lhs;
    type_T	    *expected;
    type_T	    *stacktype = NULL;

    གལ་སྲིད (cac->cac_lhs.lhs_type->tt_type == VAR_TUPLE
	    && check_type_supports_compound_op(cac->cac_lhs.lhs_type,
							*cac->cac_op) == FAIL)
	ལོག FAIL;

    གལ་སྲིད (*cac->cac_op == '.')
    {
	expected = lhs->lhs_member_type;
	stacktype = get_type_on_stack(cctx, 0);
	གལ་སྲིད (expected != &t_string
		&& need_type(stacktype, expected, 0, -1, 0, cctx,
					FALSE, FALSE) == FAIL)
	    ལོག FAIL;
	དེ་མིན གལ་སྲིད (may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
	    ལོག FAIL;
    }
    དེ་མིན
    {
	expected = lhs->lhs_member_type;
	stacktype = get_type_on_stack(cctx, 0);

	གལ་སྲིད (check_type_supports_compound_op(expected, *cac->cac_op) == FAIL)
	    ལོག FAIL;

	གལ་སྲིད (
		// If variable is float operation with number is OK.
		!(expected == &t_float && (stacktype == &t_number
					|| stacktype == &t_number_bool))
		&& need_type(stacktype, expected, TYPECHK_NUMBER_OK, -1, 0,
						cctx, FALSE, FALSE) == FAIL)
	    ལོག FAIL;
    }

    གལ་སྲིད (*cac->cac_op == '.')
    {
	གལ་སྲིད (generate_CONCAT(cctx, 2) == FAIL)
	    ལོག FAIL;
    }
    དེ་མིན གལ་སྲིད (*cac->cac_op == '+')
    {
	གལ་སྲིད (generate_add_instr(cctx,
		    operator_type(lhs->lhs_member_type, stacktype),
		    lhs->lhs_member_type, stacktype,
		    EXPR_APPEND) == FAIL)
	    ལོག FAIL;
    }
    དེ་མིན གལ་སྲིད (generate_two_op(cctx, cac->cac_op) == FAIL)
	ལོག FAIL;

    ལོག OK;
}

/*
 * Generate the STORE and SETTYPE instructions for an assignment statement.
 */
    static ཧྲིལ
compile_assign_generate_store(cctx_T *cctx, cac_T *cac)
{
    lhs_T	*lhs = &cac->cac_lhs;
    ཧྲིལ		save_lnum;

    // Use the line number of the assignment for store instruction.
    save_lnum = cctx->ctx_lnum;
    cctx->ctx_lnum = cac->cac_start_lnum - 1;

    གལ་སྲིད (lhs->lhs_has_index)
    {
	// Use the info in "lhs" to store the value at the index in the
	// list, dict or object.
	གལ་སྲིད (compile_assign_unlet(cac->cac_var_start, &cac->cac_lhs,
				 TRUE, cac->cac_rhs_type, cctx) == FAIL)
	{
	    cctx->ctx_lnum = save_lnum;
	    ལོག FAIL;
	}
    }
    དེ་མིན
    {
	གལ་སྲིད (cac->cac_is_decl && cac->cac_cmdidx == CMD_const &&
			(lhs->lhs_dest == dest_script
			 || lhs->lhs_dest == dest_script_v9
			 || lhs->lhs_dest == dest_global
			 || lhs->lhs_dest == dest_local))
	    // ":const var": lock the value, but not referenced variables
	    generate_LOCKCONST(cctx);

	type_T	*inferred_type = cac->cac_inferred_type;

	གལ་སྲིད ((lhs->lhs_type->tt_type == VAR_DICT
		    || lhs->lhs_type->tt_type == VAR_LIST)
		&& lhs->lhs_type->tt_member != NULL
		&& lhs->lhs_type->tt_member != &t_any
		&& lhs->lhs_type->tt_member != &t_unknown)
	    // Set the type in the list or dict, so that it can be
	    // checked, also in legacy script.
	    generate_SETTYPE(cctx, lhs->lhs_type);
	དེ་མིན གལ་སྲིད (lhs->lhs_type->tt_type == VAR_TUPLE
					&& lhs->lhs_type->tt_argcount != 0)
	    generate_SETTYPE(cctx, lhs->lhs_type);
	དེ་མིན གལ་སྲིད (inferred_type != NULL
		&& (inferred_type->tt_type == VAR_DICT
		    || inferred_type->tt_type == VAR_LIST)
		&& inferred_type->tt_member != NULL
		&& inferred_type->tt_member != &t_unknown
		&& inferred_type->tt_member != &t_any)
	    // Set the type in the list or dict, so that it can be
	    // checked, also in legacy script.
	    generate_SETTYPE(cctx, inferred_type);
	དེ་མིན གལ་སྲིད (inferred_type != NULL
				&& inferred_type->tt_type == VAR_TUPLE
				&& inferred_type->tt_argcount > 0)
	    generate_SETTYPE(cctx, inferred_type);

	གལ་སྲིད (!cac->cac_skip_store &&
		generate_store_lhs(cctx, &cac->cac_lhs,
				   cac->cac_instr_count,
				   cac->cac_is_decl) == FAIL)
	{
	    cctx->ctx_lnum = save_lnum;
	    ལོག FAIL;
	}
    }

    cctx->ctx_lnum = save_lnum;

    ལོག OK;
}

/*
 * Process the variable(s) in an assignment statement
 */
    static ཧྲིལ
compile_assign_process_variables(
    cctx_T	*cctx,
    cac_T	*cac,
    ཧྲིལ		cmdidx,
    ཧྲིལ		heredoc,
    ཧྲིལ		has_cmd,
    ཧྲིལ		has_argisset_prefix,
    ཧྲིལ		jump_instr_idx)
{
    /*
     * Loop over variables in "[var, var] = expr".
     * For "name = expr" and "var name: type" this is done only once.
     */
    དོན་ལུ (cac->cac_var_idx = 0; cac->cac_var_idx == 0 ||
	    cac->cac_var_idx < cac->cac_var_count; cac->cac_var_idx++)
    {
	གལ་སྲིད (cac->cac_var_start[0] == '_'
				&& !eval_isnamec(cac->cac_var_start[1]))
	{
	    // Ignore underscore in "[a, _, b] = list".
	    གལ་སྲིད (cac->cac_var_count > 0)
	    {
		cac->cac_var_start = skipwhite(cac->cac_var_start + 2);
		continue;
	    }
	    emsg(_(e_cannot_use_underscore_here));
	    ལོག FAIL;
	}
	vim_free(cac->cac_lhs.lhs_name);

	/*
	 * Figure out the LHS type and other properties.
	 */
	གལ་སྲིད (compile_assign_lhs(cac->cac_var_start, &cac->cac_lhs, cmdidx,
		    cac->cac_is_decl, heredoc, has_cmd,
		    cac->cac_oplen, cctx) == FAIL)
	    ལོག FAIL;

	// Compile the RHS expression
	གལ་སྲིད (heredoc)
	{
	    SOURCING_LNUM = cac->cac_start_lnum;
	    གལ་སྲིད (cac->cac_lhs.lhs_has_type
		    && need_type(&t_list_string, cac->cac_lhs.lhs_type,
				0, -1, 0, cctx, FALSE, FALSE) == FAIL)
		ལོག FAIL;
	}
	དེ་མིན
	{
	    གལ་སྲིད (compile_assign_rhs(cctx, cac) == FAIL)
		ལོག FAIL;
	    གལ་སྲིད (cac->cac_var_count == 0)
		cac->cac_var_end = cac->cac_nextc;
	}

	// no need to parse more when skipping
	གལ་སྲིད (cctx->ctx_skip == SKIP_YES)
	    bha_nga;

	གལ་སྲིད (cac->cac_oplen > 0 && *cac->cac_op != '=')
	{
	    གལ་སྲིད (compile_assign_compound_op(cctx, cac) == FAIL)
		ལོག FAIL;
	}

	// generate the store instructions
	གལ་སྲིད (compile_assign_generate_store(cctx, cac) == FAIL)
	    ལོག FAIL;

	གལ་སྲིད (cac->cac_var_idx + 1 < cac->cac_var_count)
	    cac->cac_var_start = skipwhite(cac->cac_lhs.lhs_end + 1);

	གལ་སྲིད (has_argisset_prefix)
	{
	    // set instruction index in JUMP_IF_ARG_SET to here
	    isn_T *isn = ((isn_T *)cac->cac_instr->ga_data) + jump_instr_idx;
	    isn->isn_arg.jumparg.jump_where = cac->cac_instr->ga_len;
	}
    }

    ལོག OK;
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
    ཧྲིལ		heredoc = FALSE;
    ཧྲིལ		jump_instr_idx;

    compile_assign_context_init(&cac, cctx, cmdidx, arg);

    jump_instr_idx = cac.cac_instr->ga_len;

    // process object variable initialization in a new() constructor method
    ཧྲིལ	has_argisset_prefix = STRNCMP(arg, "ifargisset ", 11) == 0;
    གལ་སྲིད (has_argisset_prefix &&
			compile_assign_obj_new_arg(&arg, cctx) == FAIL)
	goto theend;

    // Skip over the "varname" or "[varname, varname]" to get to any "=".
    cac.cac_nextc = skip_var_list(arg, TRUE, &cac.cac_var_count,
						&cac.cac_semicolon, TRUE);
    གལ་སྲིད (cac.cac_nextc == NULL)
	ལོག *arg == '[' ? arg : NULL;

    གལ་སྲིད (compile_assign_process_operator(eap, arg, &cac, &heredoc,
							&retstr) == FAIL)
	ལོག retstr;

    // Compute the start of the assignment
    cac.cac_var_start = compile_assign_compute_start(arg, cac.cac_var_count);

    // Compute the end of the assignment
    cac.cac_var_end = compile_assign_compute_end(eap, cctx, &cac, heredoc);
    གལ་སྲིད (cac.cac_var_end == NULL)
	ལོག NULL;

    ཧྲིལ has_cmd = cac.cac_var_start > eap->cmd;

    /* process the variable(s) */
    གལ་སྲིད (compile_assign_process_variables(cctx, &cac, cmdidx, heredoc,
					 has_cmd, has_argisset_prefix,
					 jump_instr_idx) == FAIL)
	goto theend;

    // For "[var, var] = expr" drop the "expr" value.
    // Also for "[var, var; _] = expr".
    གལ་སྲིད (cctx->ctx_skip != SKIP_YES && cac.cac_var_count > 0 &&
	    (!cac.cac_semicolon || !cac.cac_did_generate_slice))
    {
	གལ་སྲིད (generate_instr_drop(cctx, ISN_DROP, 1) == NULL)
	    goto theend;
    }

    retstr = skipwhite(cac.cac_var_end);

theend:
    vim_free(cac.cac_lhs.lhs_name);
    ལོག retstr;
}

/*
 * Check for an assignment at "eap->cmd", compile it if found.
 * Return NOTDONE if there is none, FAIL for failure, OK if done.
 */
    static ཧྲིལ
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
    གལ་སྲིད (p > eap->cmd && *p != NUL)
    {
	char_u *var_end;
	ཧྲིལ	oplen;
	ཧྲིལ	heredoc;

	གལ་སྲིད (eap->cmd[0] == '@')
	    var_end = eap->cmd + 2;
	དེ་མིན
	    var_end = find_name_end(pskip, NULL, NULL,
					FNE_CHECK_START | FNE_INCL_BR);
	oplen = assignment_len(skipwhite(var_end), &heredoc);
	གལ་སྲིད (oplen > 0)
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
	    གལ་སྲིད (*eap->cmd == '&'
		    || *eap->cmd == '$'
		    || *eap->cmd == '@'
		    || ((len) > 2 && eap->cmd[1] == ':')
		    || STRNCMP(eap->cmd, "g:[", 3) == 0
		    || variable_exists(eap->cmd, len, cctx))
	    {
		*line = compile_assignment(eap->cmd, eap, CMD_SIZE, cctx);
		གལ་སྲིད (*line == NULL || *line == eap->cmd)
		    ལོག FAIL;
		ལོག OK;
	    }
	}
    }

    // might be "[var, var] = expr" or "ifargisset this.member = expr"
    གལ་སྲིད (*eap->cmd == '[' || STRNCMP(eap->cmd, "ifargisset ", 11) == 0)
    {
	*line = compile_assignment(eap->cmd, eap, CMD_SIZE, cctx);
	གལ་སྲིད (*line == NULL)
	    ལོག FAIL;
	གལ་སྲིད (*line != eap->cmd)
	    ལོག OK;
    }
    ལོག NOTDONE;
}

/*
 * Check if arguments of "ufunc" shadow variables in "cctx".
 * Return OK or FAIL.
 */
    static ཧྲིལ
check_args_shadowing(ufunc_T *ufunc, cctx_T *cctx)
{
    ཧྲིལ	    i;
    char_u  *arg;
    ཧྲིལ	    r = OK;

    // Make sure arguments are not found when compiling a second time.
    ufunc->uf_args_visible = 0;

    // Check for arguments shadowing variables from the context.
    དོན་ལུ (i = 0; i < ufunc->uf_args.ga_len; ++i)
    {
	arg = ((char_u **)(ufunc->uf_args.ga_data))[i];
	གལ་སྲིད (check_defined(arg, STRLEN(arg), cctx, NULL, TRUE) == FAIL)
	{
	    r = FAIL;
	    bha_nga;
	}
    }
    ufunc->uf_args_visible = ufunc->uf_args.ga_len;
    ལོག r;
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
    དོན་ལུ (p = line; *p == ':' || VIM_ISWHITE(*p); ++p)
	;
    གལ་སྲིད (!SAFE_isdigit(*p))
    {
	// The command or modifiers must be following.  Assume a lower case
	// character means there is a modifier.
	གལ་སྲིད (p < eap->cmd && !vim_islower(*p))
	{
	    emsg(_(e_invalid_range));
	    ལོག -1;
	}
	ལོག 0;
    }
    ལོག atol((var_na *)p);
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

    གལ་སྲིད (debug_break_level > 0 || may_break_in_function(ufunc))
	ལོག CT_DEBUG;
#ifdef FEAT_PROFILE
    གལ་སྲིད (do_profiling == PROF_YES)
    {
	གལ་སྲིད (!ufunc->uf_profiling && has_profiling(FALSE, ufunc->uf_name, NULL,
							    &ufunc->uf_hash))
	    func_do_profile(ufunc);
	གལ་སྲིད (ufunc->uf_profiling)
	    ལོག CT_PROFILE;
    }
#endif
    ལོག CT_NONE;
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
	pak_sa CT_PROFILE:
#ifdef FEAT_PROFILE
	    instr_dest = dfunc->df_instr_prof; bha_nga;
#endif
	pak_sa CT_NONE:   instr_dest = dfunc->df_instr; bha_nga;
	pak_sa CT_DEBUG:  instr_dest = dfunc->df_instr_debug; bha_nga;
    }

    གལ་སྲིད (instr_dest != NULL)
	// Was compiled in this mode before: Free old instructions.
	delete_def_function_contents(dfunc, FALSE);

    ga_clear_strings(&dfunc->df_var_names);
    dfunc->df_defer_var_idx = 0;
}

/*
 * Add a function to the list of :def functions.
 * This sets "ufunc->uf_dfunc_idx" but the function isn't compiled yet.
 */
    static ཧྲིལ
add_def_function(ufunc_T *ufunc)
{
    dfunc_T *dfunc;

    གལ་སྲིད (def_functions.ga_len == 0)
    {
	// The first position is not used, so that a zero uf_dfunc_idx means it
	// wasn't set.
	གལ་སྲིད (GA_GROW_FAILS(&def_functions, 1))
	    ལོག FAIL;
	++def_functions.ga_len;
    }

    // Add the function to "def_functions".
    གལ་སྲིད (GA_GROW_FAILS(&def_functions, 1))
	ལོག FAIL;
    dfunc = ((dfunc_T *)def_functions.ga_data) + def_functions.ga_len;
    CLEAR_POINTER(dfunc);
    dfunc->df_idx = def_functions.ga_len;
    ufunc->uf_dfunc_idx = dfunc->df_idx;
    dfunc->df_ufunc = ufunc;
    dfunc->df_name = vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
    ga_init2(&dfunc->df_var_names, parimaa_na(char_u *), 10);
    ++dfunc->df_refcount;
    ++def_functions.ga_len;
    ལོག OK;
}

    static ཧྲིལ
compile_dfunc_ufunc_init(
    ufunc_T		*ufunc,
    cctx_T		*outer_cctx,
    compiletype_T	compile_type,
    ཧྲིལ			*new_def_function)
{
    // When using a function that was compiled before: Free old instructions.
    // The index is reused.  Otherwise add a new entry in "def_functions".
    གལ་སྲིད (ufunc->uf_dfunc_idx > 0)
	clear_def_function(ufunc, compile_type);
    དེ་མིན
    {
	གལ་སྲིད (add_def_function(ufunc) == FAIL)
	    ལོག FAIL;

	*new_def_function = TRUE;
    }

    གལ་སྲིད ((ufunc->uf_flags & FC_CLOSURE) && outer_cctx == NULL)
    {
	semsg(_(e_compiling_closure_without_context_str),
						printable_func_name(ufunc));
	ལོག FAIL;
    }

    ufunc->uf_def_status = UF_COMPILING;

    ལོག OK;
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
    ga_init2(&cctx->ctx_locals, parimaa_na(lvar_T), 10);
    // Each entry on the type stack consists of two type pointers.
    ga_init2(&cctx->ctx_type_stack, parimaa_na(type2_T), 50);
    cctx->ctx_type_list = &ufunc->uf_type_list;
    ga_init2(&cctx->ctx_instr, parimaa_na(isn_T), 50);
}

/*
 * For an object constructor, generate instruction to setup "this" (the first
 * local variable) and to initialize the object variables.
 */
    static ཧྲིལ
obj_constructor_prologue(ufunc_T *ufunc, cctx_T *cctx)
{
    generate_CONSTRUCT(cctx, ufunc->uf_class);

    དོན་ལུ (ཧྲིལ i = 0; i < ufunc->uf_class->class_obj_member_count; ++i)
    {
	ocmember_T *m = &ufunc->uf_class->class_obj_members[i];

	གལ་སྲིད (i < 2 && IS_ENUM(ufunc->uf_class))
	    // The first two object variables in an enum are the name
	    // and the ordinal.  These are set by the ISN_CONSTRUCT
	    // instruction.  So don't generate instructions to set
	    // these variables.
	    continue;

	གལ་སྲིད (m->ocm_init != NULL)
	{
	    char_u	*expr = m->ocm_init;
	    sctx_T	save_current_sctx;
	    ཧྲིལ		change_sctx = FALSE;

	    // If the member variable initialization script context is
	    // different from the current script context, then change it.
	    གལ་སྲིད (current_sctx.sc_sid != m->ocm_init_sctx.sc_sid)
		change_sctx = TRUE;

	    གལ་སྲིད (change_sctx)
	    {
		// generate an instruction to change the script context to the
		// member variable initialization script context.
		save_current_sctx = current_sctx;
		current_sctx = m->ocm_init_sctx;
		generate_SCRIPTCTX_SET(cctx, current_sctx);
	    }

	    ཧྲིལ r = compile_expr0(&expr, cctx);

	    གལ་སྲིད (change_sctx)
	    {
		// restore the previous script context
		current_sctx = save_current_sctx;
		generate_SCRIPTCTX_SET(cctx, current_sctx);
	    }

	    གལ་སྲིད (r == FAIL)
		ལོག FAIL;

	    གལ་སྲིད (!ends_excmd2(m->ocm_init, expr))
	    {
		semsg(_(e_trailing_characters_str), expr);
		ལོག FAIL;
	    }

	    type_T	*type = get_type_on_stack(cctx, 0);
	    གལ་སྲིད (m->ocm_type->tt_type == VAR_ANY
		    && !(m->ocm_flags & OCMFLAG_HAS_TYPE)
		    && type->tt_type != VAR_SPECIAL)
	    {
		// If the member variable type is not yet set, then use
		// the initialization expression type.
		m->ocm_type = type;
	    }
	    དེ་མིན
	    {
		// The type of the member initialization expression is
		// determined at run time.  Add a runtime type check.
		where_T	where = WHERE_INIT;
		where.wt_kind = WT_MEMBER;
		where.wt_func_name = (var_na *)m->ocm_name.string;
		གལ་སྲིད (need_type_where(type, m->ocm_type, 0, -1,
					where, cctx, FALSE, FALSE) == FAIL)
		    ལོག FAIL;
	    }
	}
	དེ་མིན
	    push_default_value(cctx, m->ocm_type->tt_type, FALSE, NULL);

	གལ་སྲིད (((m->ocm_type->tt_type == VAR_DICT
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

    ལོག OK;
}

/*
 * For an object method and an constructor, generate instruction to setup
 * "this" (the first local variable).  For a constructor, generate instructions
 * to initialize the object variables.
 */
    static ཧྲིལ
obj_method_prologue(ufunc_T *ufunc, cctx_T *cctx)
{
    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data) + ufunc->uf_dfunc_idx;

    གལ་སྲིད (GA_GROW_FAILS(&dfunc->df_var_names, 1))
	ལོག FAIL;

    ((char_u **)dfunc->df_var_names.ga_data)[0] =
						vim_strsave((char_u *)"this");
    ++dfunc->df_var_names.ga_len;

    // In the constructor allocate memory for the object and initialize the
    // object members.
    གལ་སྲིད (IS_CONSTRUCTOR_METHOD(ufunc))
	ལོག obj_constructor_prologue(ufunc, cctx);

    ལོག OK;
}

/*
 * Produce instructions for the default values of optional arguments.
 */
    static ཧྲིལ
compile_def_function_default_args(
    ufunc_T	*ufunc,
    garray_T	*instr,
    cctx_T	*cctx)
{
    ཧྲིལ	count = ufunc->uf_def_args.ga_len;
    ཧྲིལ	first_def_arg = ufunc->uf_args.ga_len - count;
    ཧྲིལ	i;
    ཧྲིལ	off = STACK_FRAME_SIZE + (ufunc->uf_va_name != NULL ? 1 : 0);
    ཧྲིལ	did_set_arg_type = FALSE;

    // Produce instructions for the default values of optional arguments.
    SOURCING_LNUM = 0;  // line number unknown
    དོན་ལུ (i = 0; i < count; ++i)
    {
	char_u *arg = ((char_u **)(ufunc->uf_def_args.ga_data))[i];
	གལ་སྲིད (STRCMP(arg, "v:none") == 0)
	    // "arg = v:none" means the argument is optional without
	    // setting a value when the argument is missing.
	    continue;

	type_T	*val_type;
	ཧྲིལ		arg_idx = first_def_arg + i;
	where_T	where = WHERE_INIT;
	ཧྲིལ		jump_instr_idx = instr->ga_len;
	isn_T	*isn;

	// Use a JUMP_IF_ARG_SET instruction to skip if the value was given.
	གལ་སྲིད (generate_JUMP_IF_ARG(cctx, ISN_JUMP_IF_ARG_SET,
						i - count - off) == FAIL)
	    ལོག FAIL;

	// Make sure later arguments are not found.
	ufunc->uf_args_visible = arg_idx;

	ཧྲིལ r = compile_expr0(&arg, cctx);
	གལ་སྲིད (r == FAIL)
	    ལོག FAIL;

	// If no type specified use the type of the default value.
	// Otherwise check that the default value type matches the
	// specified type.
	val_type = get_type_on_stack(cctx, 0);
	where.wt_index = arg_idx + 1;
	where.wt_kind = WT_ARGUMENT;
	གལ་སྲིད (ufunc->uf_arg_types[arg_idx] == &t_unknown)
	{
	    did_set_arg_type = TRUE;
	    ufunc->uf_arg_types[arg_idx] = val_type;
	}
	དེ་མིན གལ་སྲིད (need_type_where(val_type, ufunc->uf_arg_types[arg_idx],
		    0, -1, where, cctx, FALSE, FALSE) == FAIL)
	    ལོག FAIL;

	གལ་སྲིད (generate_STORE(cctx, ISN_STORE, i - count - off, NULL) == FAIL)
	    ལོག FAIL;

	// set instruction index in JUMP_IF_ARG_SET to here
	isn = ((isn_T *)instr->ga_data) + jump_instr_idx;
	isn->isn_arg.jumparg.jump_where = instr->ga_len;
    }

    གལ་སྲིད (did_set_arg_type)
	set_function_type(ufunc);

    ལོག OK;
}

/*
 * Compile def function body.  Loop over all the lines in the function and
 * generate instructions.
 */
    static ཧྲིལ
compile_def_function_body(
    ཧྲིལ		last_func_lnum,
    ཧྲིལ		check_return_type,
    garray_T	*lines_to_free,
    var_na	**errormsg,
    cctx_T	*cctx)
{
    char_u	*line = NULL;
    char_u	*p;
    ཧྲིལ		did_emsg_before = did_emsg;
#ifdef FEAT_PROFILE
    ཧྲིལ		prof_lnum = -1;
#endif
    ཧྲིལ		debug_lnum = -1;

    དོན་ལུ (;;)
    {
	exarg_T	    ea;
	ཧྲིལ	    starts_with_colon = FALSE;
	char_u	    *cmd;
	cmdmod_T    local_cmdmod;

	// Bail out on the first error to avoid a flood of errors and report
	// the right line number when inside try/catch.
	གལ་སྲིད (did_emsg_before != did_emsg)
	    ལོག FAIL;

	གལ་སྲིད (line != NULL && *line == '|')
	    // the line continues after a '|'
	    ++line;
	དེ་མིན གལ་སྲིད (line != NULL && *skipwhite(line) != NUL
		&& !(*line == '#' && (line == cctx->ctx_line_start
						    || VIM_ISWHITE(line[-1]))))
	{
	    semsg(_(e_trailing_characters_str), line);
	    ལོག FAIL;
	}
	དེ་མིན གལ་སྲིད (line != NULL && vim9_bad_comment(skipwhite(line)))
	    ལོག FAIL;
	དེ་མིན
	{
	    line = next_line_from_context(cctx, FALSE);
	    གལ་སྲིད (cctx->ctx_lnum >= last_func_lnum)
	    {
		// beyond the last line
#ifdef FEAT_PROFILE
		གལ་སྲིད (cctx->ctx_skip != SKIP_YES)
		    may_generate_prof_end(cctx, prof_lnum);
#endif
		bha_nga;
	    }
	    // Make a copy, splitting off nextcmd and removing trailing spaces
	    // may change it.
	    གལ་སྲིད (line != NULL)
	    {
		line = vim_strsave(line);
		གལ་སྲིད (ga_add_string(lines_to_free, line) == FAIL)
		{
		    vim_free(line);
		    ལོག FAIL;
		}
	    }
	}

	CLEAR_FIELD(ea);
	ea.cmdlinep = &line;
	ea.cmd = skipwhite(line);
	ea.skip = cctx->ctx_skip == SKIP_YES;

	གལ་སྲིད (*ea.cmd == '#')
	{
	    // "#" starts a comment, but "#{" is an error
	    གལ་སྲིད (vim9_bad_comment(ea.cmd))
		ལོག FAIL;
	    line = (char_u *)"";
	    continue;
	}

#ifdef FEAT_PROFILE
	གལ་སྲིད (cctx->ctx_compile_type == CT_PROFILE && cctx->ctx_lnum != prof_lnum
						  && cctx->ctx_skip != SKIP_YES)
	{
	    may_generate_prof_end(cctx, prof_lnum);

	    prof_lnum = cctx->ctx_lnum;
	    generate_instr(cctx, ISN_PROF_START);
	}
#endif
	གལ་སྲིད (cctx->ctx_compile_type == CT_DEBUG && cctx->ctx_lnum != debug_lnum
						  && cctx->ctx_skip != SKIP_YES)
	{
	    debug_lnum = cctx->ctx_lnum;
	    generate_instr_debug(cctx);
	}
	cctx->ctx_prev_lnum = cctx->ctx_lnum + 1;

	// Some things can be recognized by the first character.
	switch (*ea.cmd)
	{
	    pak_sa '}':
		{
		    // "}" ends a block scope
		    scopetype_T stype = cctx->ctx_scope == NULL
					  ? NO_SCOPE : cctx->ctx_scope->se_type;

		    གལ་སྲིད (stype == BLOCK_SCOPE)
		    {
			compile_endblock(cctx);
			line = ea.cmd;
		    }
		    དེ་མིན
		    {
			emsg(_(e_using_rcurly_outside_if_block_scope));
			ལོག FAIL;
		    }
		    གལ་སྲིད (line != NULL)
			line = skipwhite(ea.cmd + 1);
		    continue;
		}

	    pak_sa '{':
		// "{" starts a block scope
		// "{'a': 1}->func() is something else
		གལ་སྲིད (ends_excmd(*skipwhite(ea.cmd + 1)))
		{
		    line = compile_block(ea.cmd, cctx);
		    continue;
		}
		bha_nga;
	}

	/*
	 * COMMAND MODIFIERS
	 */
	cctx->ctx_has_cmdmod = FALSE;
	གལ་སྲིད (parse_command_modifiers(&ea, errormsg, &local_cmdmod, FALSE)
								       == FAIL)
	    ལོག FAIL;
	generate_cmdmods(cctx, &local_cmdmod);
	undo_cmdmod(&local_cmdmod);

	// Check if there was a colon after the last command modifier or before
	// the current position.
	དོན་ལུ (p = ea.cmd; p >= line; --p)
	{
	    གལ་སྲིད (*p == ':')
		starts_with_colon = TRUE;
	    གལ་སྲིད (p < ea.cmd && !VIM_ISWHITE(*p))
		bha_nga;
	}

	// Skip ":call" to get to the function name, unless using :legacy
	p = ea.cmd;
	གལ་སྲིད (!(local_cmdmod.cmod_flags & CMOD_LEGACY))
	{
	    གལ་སྲིད (checkforcmd(&ea.cmd, "call", 3))
	    {
		གལ་སྲིད (*ea.cmd == '(')
		    // not for "call()"
		    ea.cmd = p;
		དེ་མིན
		    ea.cmd = skipwhite(ea.cmd);
	    }

	    གལ་སྲིད (!starts_with_colon)
	    {
		ཧྲིལ	    assign;

		// Check for assignment after command modifiers.
		assign = may_compile_assignment(&ea, &line, cctx);
		གལ་སྲིད (assign == OK)
		    goto nextline;
		གལ་སྲིད (assign == FAIL)
		    ལོག FAIL;
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
	གལ་སྲིད ((*cmd != '$' || starts_with_colon)
		&& (starts_with_colon
		    || !(*cmd == '\''
			|| (cmd[0] == '0' && cmd[1] == 'z')
			|| (cmd[0] != NUL && cmd[0] == cmd[1]
					    && (*cmd == '+' || *cmd == '-'))
			|| number_method(cmd))))
	{
	    ea.cmd = skip_range(ea.cmd, TRUE, NULL);
	    གལ་སྲིད (ea.cmd > cmd)
	    {
		གལ་སྲིད (!starts_with_colon
				   && !(local_cmdmod.cmod_flags & CMOD_LEGACY))
		{
		    semsg(_(e_colon_required_before_range_str), cmd);
		    ལོག FAIL;
		}
		ea.addr_count = 1;
		གལ་སྲིད (ends_excmd2(line, ea.cmd))
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

	གལ་སྲིད (p == NULL)
	{
	    གལ་སྲིད (cctx->ctx_skip != SKIP_YES)
		semsg(_(e_ambiguous_use_of_user_defined_command_str), ea.cmd);
	    ལོག FAIL;
	}

	// When using ":legacy cmd" always use compile_exec().
	གལ་སྲིད (local_cmdmod.cmod_flags & CMOD_LEGACY)
	{
	    char_u *start = ea.cmd;

	    switch (ea.cmdidx)
	    {
		pak_sa CMD_if:
		pak_sa CMD_elseif:
		pak_sa CMD_else:
		pak_sa CMD_endif:
		pak_sa CMD_for:
		pak_sa CMD_endfor:
		pak_sa CMD_continue:
		pak_sa CMD_break:
		pak_sa CMD_while:
		pak_sa CMD_endwhile:
		pak_sa CMD_try:
		pak_sa CMD_catch:
		pak_sa CMD_finally:
		pak_sa CMD_endtry:
			semsg(_(e_cannot_use_legacy_with_command_str), ea.cmd);
			ལོག FAIL;
		default: bha_nga;
	    }

	    // ":legacy return expr" needs to be handled differently.
	    གལ་སྲིད (checkforcmd(&start, "return", 4))
		ea.cmdidx = CMD_return;
	    དེ་མིན
		ea.cmdidx = CMD_legacy;
	}

	གལ་སྲིད (p == ea.cmd && ea.cmdidx != CMD_SIZE)
	{
	    // "eval" is used for "val->func()" and "var" for "var = val", then
	    // "p" is equal to "ea.cmd" for a valid command.
	    གལ་སྲིད (ea.cmdidx == CMD_eval || ea.cmdidx == CMD_var)
		;
	    དེ་མིན གལ་སྲིད (cctx->ctx_skip == SKIP_YES)
	    {
		line += STRLEN(line);
		goto nextline;
	    }
	    དེ་མིན
	    {
		semsg(_(e_command_not_recognized_str), ea.cmd);
		ལོག FAIL;
	    }
	}

	གལ་སྲིད ((cctx->ctx_had_return || cctx->ctx_had_throw)
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
	    ལོག FAIL;
	}

	// When processing the end of an if-else or try-catch block, don't
	// clear the "ctx_had_throw" flag.  If such a block ends in a "throw"
	// statement, then it is considered to end in a "return" statement.
	// The "ctx_had_throw" is cleared immediately after processing the
	// block ending statement.
	// Otherwise, clear the "had_throw" flag.
	གལ་སྲིད (ea.cmdidx != CMD_else && ea.cmdidx != CMD_elseif
						&& ea.cmdidx != CMD_endif
						&& ea.cmdidx != CMD_catch
						&& ea.cmdidx != CMD_finally
						&& ea.cmdidx != CMD_endtry)
	    cctx->ctx_had_throw = FALSE;

	p = skipwhite(p);
	གལ་སྲིད (ea.cmdidx != CMD_SIZE
			    && ea.cmdidx != CMD_write && ea.cmdidx != CMD_read)
	{
	    གལ་སྲིད (ea.cmdidx >= 0)
		ea.argt = excmd_get_argt(ea.cmdidx);
	    གལ་སྲིད ((ea.argt & EX_BANG) && *p == '!')
	    {
		ea.forceit = TRUE;
		p = skipwhite(p + 1);
	    }
	    གལ་སྲིད ((ea.argt & EX_RANGE) == 0 && ea.addr_count > 0)
	    {
		emsg(_(e_no_range_allowed));
		ལོག FAIL;
	    }
	}

	switch (ea.cmdidx)
	{
	    pak_sa CMD_def:
	    pak_sa CMD_function:
		    ea.arg = p;
		    line = compile_nested_function(&ea, cctx, lines_to_free);
		    bha_nga;

	    pak_sa CMD_return:
		    line = compile_return(p, check_return_type,
				 local_cmdmod.cmod_flags & CMOD_LEGACY, cctx);
		    cctx->ctx_had_return = TRUE;
		    bha_nga;

	    pak_sa CMD_let:
		    emsg(_(e_cannot_use_let_in_vim9_script));
		    bha_nga;
	    pak_sa CMD_var:
	    pak_sa CMD_final:
	    pak_sa CMD_const:
	    pak_sa CMD_increment:
	    pak_sa CMD_decrement:
		    line = compile_assignment(p, &ea, ea.cmdidx, cctx);
		    གལ་སྲིད (line == p)
		    {
			emsg(_(e_invalid_assignment));
			line = NULL;
		    }
		    bha_nga;

	    pak_sa CMD_unlet:
	    pak_sa CMD_unlockvar:
	    pak_sa CMD_lockvar:
		    line = compile_unletlock(p, &ea, cctx);
		    bha_nga;

	    pak_sa CMD_import:
		    emsg(_(e_import_can_only_be_used_in_script));
		    line = NULL;
		    bha_nga;

	    pak_sa CMD_if:
		    line = compile_if(p, cctx);
		    bha_nga;
	    pak_sa CMD_elseif:
		    line = compile_elseif(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    cctx->ctx_had_throw = FALSE;
		    bha_nga;
	    pak_sa CMD_else:
		    line = compile_else(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    cctx->ctx_had_throw = FALSE;
		    bha_nga;
	    pak_sa CMD_endif:
		    line = compile_endif(p, cctx);
		    cctx->ctx_had_throw = FALSE;
		    bha_nga;

	    pak_sa CMD_while:
		    line = compile_while(p, cctx);
		    bha_nga;
	    pak_sa CMD_endwhile:
		    line = compile_endwhile(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    bha_nga;

	    pak_sa CMD_for:
		    line = compile_for(p, cctx);
		    bha_nga;
	    pak_sa CMD_endfor:
		    line = compile_endfor(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    bha_nga;
	    pak_sa CMD_continue:
		    line = compile_continue(p, cctx);
		    // "continue" unconditionally jumps back to the loop,
		    // like a "return".
		    cctx->ctx_had_return = TRUE;
		    bha_nga;
	    pak_sa CMD_break:
		    line = compile_break(p, cctx);
		    // "break" unconditionally exits the loop, like a "return".
		    cctx->ctx_had_return = TRUE;
		    bha_nga;

	    pak_sa CMD_try:
		    line = compile_try(p, cctx);
		    bha_nga;
	    pak_sa CMD_catch:
		    line = compile_catch(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    cctx->ctx_had_throw = FALSE;
		    bha_nga;
	    pak_sa CMD_finally:
		    line = compile_finally(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    cctx->ctx_had_throw = FALSE;
		    bha_nga;
	    pak_sa CMD_endtry:
		    line = compile_endtry(p, cctx);
		    cctx->ctx_had_throw = FALSE;
		    bha_nga;
	    pak_sa CMD_throw:
		    line = compile_throw(p, cctx);
		    cctx->ctx_had_throw = TRUE;
		    bha_nga;

	    pak_sa CMD_eval:
		    line = compile_eval(p, cctx);
		    bha_nga;

	    pak_sa CMD_defer:
		    line = compile_defer(p, cctx);
		    bha_nga;

#ifdef HAS_MESSAGE_WINDOW
	    pak_sa CMD_echowindow:
		    {
			long cmd_count = get_cmd_count(line, &ea);
			གལ་སྲིད (cmd_count < 0)
			    line = NULL;
			དེ་མིན
			    line = compile_mult_expr(p, ea.cmdidx,
							     cmd_count, cctx);
		    }
		    bha_nga;
#endif
	    pak_sa CMD_echo:
	    pak_sa CMD_echon:
	    pak_sa CMD_echoconsole:
	    pak_sa CMD_echoerr:
	    pak_sa CMD_echomsg:
	    pak_sa CMD_execute:
		    line = compile_mult_expr(p, ea.cmdidx, 0, cctx);
		    bha_nga;

	    pak_sa CMD_put:
		    ea.cmd = cmd;
		    line = compile_put(p, &ea, cctx, FALSE);
		    bha_nga;

	    pak_sa CMD_iput:
		    ea.cmd = cmd;
		    line = compile_put(p, &ea, cctx, TRUE);
		    bha_nga;

	    pak_sa CMD_substitute:
		    གལ་སྲིད (check_global_and_subst(ea.cmd, p) == FAIL)
			ལོག FAIL;
		    གལ་སྲིད (cctx->ctx_skip == SKIP_YES)
			line = (char_u *)"";
		    དེ་མིན
		    {
			ea.arg = p;
			line = compile_substitute(line, &ea, cctx);
		    }
		    bha_nga;

	    pak_sa CMD_redir:
		    ea.arg = p;
		    line = compile_redir(line, &ea, cctx);
		    bha_nga;

	    pak_sa CMD_cexpr:
	    pak_sa CMD_lexpr:
	    pak_sa CMD_caddexpr:
	    pak_sa CMD_laddexpr:
	    pak_sa CMD_cgetexpr:
	    pak_sa CMD_lgetexpr:
#ifdef FEAT_QUICKFIX
		    ea.arg = p;
		    line = compile_cexpr(line, &ea, cctx);
#དེ་མིན
		    ex_ni(&ea);
		    line = NULL;
#endif
		    bha_nga;

	    pak_sa CMD_append:
	    pak_sa CMD_change:
	    pak_sa CMD_insert:
	    pak_sa CMD_k:
	    pak_sa CMD_open:
	    pak_sa CMD_t:
	    pak_sa CMD_xit:
		    not_in_vim9(&ea, false);
		    ལོག FAIL;

	    pak_sa CMD_SIZE:
		    གལ་སྲིད (cctx->ctx_skip != SKIP_YES)
		    {
			semsg(_(e_invalid_command_str), ea.cmd);
			ལོག FAIL;
		    }
		    // We don't check for a next command here.
		    line = (char_u *)"";
		    bha_nga;

	    pak_sa CMD_lua:
	    pak_sa CMD_mzscheme:
	    pak_sa CMD_perl:
	    pak_sa CMD_py3:
	    pak_sa CMD_python3:
	    pak_sa CMD_python:
	    pak_sa CMD_pythonx:
	    pak_sa CMD_ruby:
	    pak_sa CMD_tcl:
		    ea.arg = p;
		    གལ་སྲིད (vim_strchr(line, '\n') == NULL)
			line = compile_exec(line, &ea, cctx);
		    དེ་མིན
			// heredoc lines have been concatenated with NL
			// characters in get_function_body()
			line = compile_script(line, cctx);
		    bha_nga;

	    pak_sa CMD_vim9script:
		    གལ་སྲིད (cctx->ctx_skip != SKIP_YES)
		    {
			emsg(_(e_vim9script_can_only_be_used_in_script));
			ལོག FAIL;
		    }
		    line = (char_u *)"";
		    bha_nga;

	    pak_sa CMD_class:
		    emsg(_(e_class_can_only_be_used_in_script));
		    ལོག FAIL;

	    pak_sa CMD_enum:
		    emsg(_(e_enum_can_only_be_used_in_script));
		    ལོག FAIL;

	    pak_sa CMD_interface:
		    emsg(_(e_interface_can_only_be_used_in_script));
		    ལོག FAIL;

	    pak_sa CMD_type:
		    emsg(_(e_type_can_only_be_used_in_script));
		    ལོག FAIL;

	    pak_sa CMD_global:
		    གལ་སྲིད (check_global_and_subst(ea.cmd, p) == FAIL)
			ལོག FAIL;
		    // FALLTHROUGH
	    default:
		    // Not recognized, execute with do_cmdline_cmd().
		    ea.arg = p;
		    line = compile_exec(line, &ea, cctx);
		    bha_nga;
	}
nextline:
	གལ་སྲིད (line == NULL)
	    ལོག FAIL;
	line = skipwhite(line);

	// Undo any command modifiers.
	generate_undo_cmdmods(cctx);

	གལ་སྲིད (cctx->ctx_type_stack.ga_len < 0)
	{
	    iemsg("Type stack underflow");
	    ལོག FAIL;
	}
    } // END of the loop over all the function body lines.

    ལོག OK;
}

/*
 * Returns TRUE if the end of a scope (if, while, for, block) is missing.
 * Called after compiling a def function body.
 */
    static ཧྲིལ
compile_dfunc_scope_end_missing(cctx_T *cctx)
{
    གལ་སྲིད (cctx->ctx_scope == NULL)
	ལོག FALSE;

    switch (cctx->ctx_scope->se_type)
    {
	pak_sa IF_SCOPE:
	    emsg(_(e_missing_endif));
	    bha_nga;
	pak_sa WHILE_SCOPE:
	    emsg(_(e_missing_endwhile));
	    bha_nga;
	pak_sa FOR_SCOPE:
	    emsg(_(e_missing_endfor));
	    bha_nga;
	pak_sa TRY_SCOPE:
	    emsg(_(e_missing_endtry));
	    bha_nga;
	pak_sa BLOCK_SCOPE:
	    // end block scope from :try (maybe)
	    compile_endblock(cctx);
	    གལ་སྲིད (cctx->ctx_scope != NULL
		    && cctx->ctx_scope->se_type == TRY_SCOPE)
		emsg(_(e_missing_endtry));
	    དེ་མིན
		emsg(_(e_missing_rcurly));
	    bha_nga;
	default:
	    emsg(_(e_missing_rcurly));
    }
    ལོག TRUE;
}

/*
 * When compiling a def function, if it doesn't have an explicit return
 * statement, then generate a default return instruction.  For an object
 * constructor, return the object.
 */
    static ཧྲིལ
compile_dfunc_generate_default_return(ufunc_T *ufunc, cctx_T *cctx)
{
    // TODO: if a function ends in "throw" but there was a return elsewhere we
    // should not assume the return type is "void".
    གལ་སྲིད (cctx->ctx_had_return || cctx->ctx_had_throw)
	ལོག OK;

    གལ་སྲིད (ufunc->uf_ret_type->tt_type == VAR_UNKNOWN)
	ufunc->uf_ret_type = &t_void;
    དེ་མིན གལ་སྲིད (ufunc->uf_ret_type->tt_type != VAR_VOID
					&& !IS_CONSTRUCTOR_METHOD(ufunc))
    {
	emsg(_(e_missing_return_statement));
	ལོག FAIL;
    }

    // Return void if there is no return at the end.
    // For a constructor return the object.
    གལ་སྲིད (IS_CONSTRUCTOR_METHOD(ufunc))
    {
	generate_instr(cctx, ISN_RETURN_OBJECT);
	ufunc->uf_ret_type = &ufunc->uf_class->class_object_type;
    }
    དེ་མིན
	generate_instr(cctx, ISN_RETURN_VOID);

    ལོག OK;
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
    གལ་སྲིད (cctx->ctx_compile_type == CT_PROFILE)
    {
	dfunc->df_instr_prof = instr->ga_data;
	dfunc->df_instr_prof_count = instr->ga_len;
    }
    དེ་མིན
#endif
	གལ་སྲིད (cctx->ctx_compile_type == CT_DEBUG)
	{
	    dfunc->df_instr_debug = instr->ga_data;
	    dfunc->df_instr_debug_count = instr->ga_len;
	}
	དེ་མིན
	{
	    dfunc->df_instr = instr->ga_data;
	    dfunc->df_instr_count = instr->ga_len;
	}
    dfunc->df_varcount = dfunc->df_var_names.ga_len;
    dfunc->df_has_closure = cctx->ctx_has_closure;

    གལ་སྲིད (cctx->ctx_outer_used)
    {
	ufunc->uf_flags |= FC_CLOSURE;
	གལ་སྲིད (outer_cctx != NULL)
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
    ཧྲིལ		new_def_function,
    var_na	*errormsg,
    ཧྲིལ		did_emsg_before,
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
    གལ་སྲིད (!dfunc->df_deleted && new_def_function
			&& ufunc->uf_dfunc_idx == def_functions.ga_len - 1)
    {
	--def_functions.ga_len;
	ufunc->uf_dfunc_idx = 0;
    }
    ufunc->uf_def_status = UF_COMPILE_ERROR;

    སྐབས (cctx->ctx_scope != NULL)
	drop_scope(cctx);

    གལ་སྲིད (errormsg != NULL)
	emsg(errormsg);
    དེ་མིན གལ་སྲིད (did_emsg == did_emsg_before)
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
    ཧྲིལ
compile_def_function(
	ufunc_T		*ufunc,
	ཧྲིལ		check_return_type,
	compiletype_T   compile_type,
	cctx_T		*outer_cctx)
{
    garray_T	lines_to_free;
    var_na	*errormsg = NULL;	// error message
    cctx_T	cctx;
    garray_T	*instr;
    ཧྲིལ		did_emsg_before = did_emsg;
    ཧྲིལ		did_emsg_silent_before = did_emsg_silent;
    ཧྲིལ		ret = FAIL;
    sctx_T	save_current_sctx = current_sctx;
    ཧྲིལ		save_estack_compiling = estack_compiling;
    ཧྲིལ		save_cmod_flags = cmdmod.cmod_flags;
    ཧྲིལ		do_estack_push;
    ཧྲིལ		new_def_function = FALSE;

    // allocated lines are freed at the end
    ga_init2(&lines_to_free, parimaa_na(char_u *), 50);

    // Initialize the ufunc and the compilation context
    གལ་སྲིད (compile_dfunc_ufunc_init(ufunc, outer_cctx, compile_type,
						&new_def_function) == FAIL)
	ལོག FAIL;

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
    གལ་སྲིད (do_estack_push)
	estack_push_ufunc(ufunc, 1);
    estack_compiling = TRUE;

    // Make sure arguments don't shadow variables in the context
    གལ་སྲིད (check_args_shadowing(ufunc, &cctx) == FAIL)
	goto erret;

    // For an object method and a constructor generate instructions to
    // initialize "this" and the object variables.
    གལ་སྲིད (ufunc->uf_flags & (FC_OBJECT|FC_NEW))
	གལ་སྲིད (obj_method_prologue(ufunc, &cctx) == FAIL)
	    goto erret;

    གལ་སྲིད (ufunc->uf_def_args.ga_len > 0)
	གལ་སྲིད (compile_def_function_default_args(ufunc, instr, &cctx) == FAIL)
	    goto erret;
    ufunc->uf_args_visible = ufunc->uf_args.ga_len;

    // Compiling an abstract method or a function in an interface is done to
    // get the function type.  No code is actually compiled.
    གལ་སྲིད (ufunc->uf_class != NULL && (IS_INTERFACE(ufunc->uf_class)
						|| IS_ABSTRACT_METHOD(ufunc)))
    {
	ufunc->uf_def_status = UF_NOT_COMPILED;
	ret = OK;
	goto erret;
    }

    // compile the function body
    གལ་སྲིད (compile_def_function_body(ufunc->uf_lines.ga_len, check_return_type,
				&lines_to_free, &errormsg, &cctx) == FAIL)
	goto erret;

    གལ་སྲིད (compile_dfunc_scope_end_missing(&cctx))
	goto erret;

    གལ་སྲིད (compile_dfunc_generate_default_return(ufunc, &cctx) == FAIL)
	goto erret;

    // When compiled with ":silent!" and there was an error don't consider the
    // function compiled.
    གལ་སྲིད (emsg_silent == 0 || did_emsg_silent == did_emsg_silent_before)
	compile_dfunc_epilogue(outer_cctx, ufunc, instr, &cctx);

    ret = OK;

erret:
    གལ་སྲིད (ufunc->uf_def_status == UF_COMPILING)
    {
	// compilation failed. do cleanup.
	compile_dfunc_ufunc_cleanup(ufunc, instr, new_def_function,
				    errormsg, did_emsg_before, &cctx);
    }

    གལ་སྲིད (cctx.ctx_redir_lhs.lhs_name != NULL)
    {
	གལ་སྲིད (ret == OK)
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
    གལ་སྲིད (do_estack_push)
	estack_pop();

    ga_clear_strings(&lines_to_free);
    free_locals(&cctx);
    ga_clear(&cctx.ctx_type_stack);
    ལོག ret;
}

    void
set_function_type(ufunc_T *ufunc)
{
    ཧྲིལ varargs = ufunc->uf_va_name != NULL;
    ཧྲིལ argcount = ufunc->uf_args.ga_len;

    // Create a type for the function, with the return type and any
    // argument types.
    // A vararg is included in uf_args.ga_len but not in uf_arg_types.
    // The type is included in "tt_args".
    གལ་སྲིད (argcount > 0 || varargs)
    {
	གལ་སྲིད (ufunc->uf_type_list.ga_itemsize == 0)
	    ga_init2(&ufunc->uf_type_list, parimaa_na(type_T *), 10);
	ufunc->uf_func_type = alloc_func_type(ufunc->uf_ret_type,
					   argcount, &ufunc->uf_type_list);
	// Add argument types to the function type.
	གལ་སྲིད (func_type_add_arg_types(ufunc->uf_func_type,
				    argcount + varargs,
				    &ufunc->uf_type_list) == FAIL)
	    ལོག;
	ufunc->uf_func_type->tt_argcount = argcount + varargs;
	ufunc->uf_func_type->tt_min_argcount =
				      argcount - ufunc->uf_def_args.ga_len;
	གལ་སྲིད (ufunc->uf_arg_types == NULL)
	{
	    ཧྲིལ i;

	    // lambda does not have argument types.
	    དོན་ལུ (i = 0; i < argcount; ++i)
		ufunc->uf_func_type->tt_args[i] = &t_any;
	}
	དེ་མིན
	    mch_memmove(ufunc->uf_func_type->tt_args,
			 ufunc->uf_arg_types, parimaa_na(type_T *) * argcount);
	གལ་སྲིད (varargs)
	{
	    ufunc->uf_func_type->tt_args[argcount] =
		   ufunc->uf_va_type == NULL ? &t_list_any : ufunc->uf_va_type;
	    ufunc->uf_func_type->tt_flags = TTFLAG_VARARGS;
	}
    }
    དེ་མིན
	// No arguments, can use a predefined type.
	ufunc->uf_func_type = get_func_type(ufunc->uf_ret_type,
					   argcount, &ufunc->uf_type_list);
}

/*
 * Free all instructions for "dfunc" except df_name.
 */
    static void
delete_def_function_contents(dfunc_T *dfunc, ཧྲིལ mark_deleted)
{
    ཧྲིལ idx;

    // In same cases the instructions may refer to a class in which the
    // function is defined and unreferencing the class may call back here
    // recursively.  Set the df_delete_busy to avoid problems.
    གལ་སྲིད (dfunc->df_delete_busy)
	ལོག;
    dfunc->df_delete_busy = TRUE;

    ga_clear(&dfunc->df_def_args_isn);
    ga_clear_strings(&dfunc->df_var_names);

    གལ་སྲིད (dfunc->df_instr != NULL)
    {
	དོན་ལུ (idx = 0; idx < dfunc->df_instr_count; ++idx)
	    delete_instr(dfunc->df_instr + idx);
	VIM_CLEAR(dfunc->df_instr);
    }
    གལ་སྲིད (dfunc->df_instr_debug != NULL)
    {
	དོན་ལུ (idx = 0; idx < dfunc->df_instr_debug_count; ++idx)
	    delete_instr(dfunc->df_instr_debug + idx);
	VIM_CLEAR(dfunc->df_instr_debug);
    }
#ifdef FEAT_PROFILE
    གལ་སྲིད (dfunc->df_instr_prof != NULL)
    {
	དོན་ལུ (idx = 0; idx < dfunc->df_instr_prof_count; ++idx)
	    delete_instr(dfunc->df_instr_prof + idx);
	VIM_CLEAR(dfunc->df_instr_prof);
    }
#endif

    གལ་སྲིད (mark_deleted)
	dfunc->df_deleted = TRUE;
    གལ་སྲིད (dfunc->df_ufunc != NULL)
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
    གལ་སྲིད (ufunc->uf_dfunc_idx <= 0 || def_functions.ga_data == NULL)
	ལོག;

    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data)
	+ ufunc->uf_dfunc_idx;

    གལ་སྲིད (--dfunc->df_refcount <= 0)
	delete_def_function_contents(dfunc, TRUE);
    ufunc->uf_def_status = UF_NOT_COMPILED;
    ufunc->uf_dfunc_idx = 0;
    གལ་སྲིད (dfunc->df_ufunc == ufunc)
	dfunc->df_ufunc = NULL;
}

/*
 * Used when a user function refers to an existing dfunc.
 */
    void
link_def_function(ufunc_T *ufunc)
{
    གལ་སྲིད (ufunc->uf_dfunc_idx <= 0)
	ལོག;

    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data)
	+ ufunc->uf_dfunc_idx;

    ++dfunc->df_refcount;
}

#གལ་སྲིད defined(EXITFREE)
/*
 * Free all functions defined with ":def".
 */
    void
free_def_functions(void)
{
    ཧྲིལ idx;

    དོན་ལུ (idx = 0; idx < def_functions.ga_len; ++idx)
    {
	dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data) + idx;

	delete_def_function_contents(dfunc, TRUE);
	vim_free(dfunc->df_name);
    }

    ga_clear(&def_functions);
}
#endif


#endif // FEAT_EVAL
