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
#ଅନ୍ତର୍ଭାଵ "vim.h"

#ଯଦି defined(FEAT_EVAL)

// Functions defined with :def are stored in this growarray.
// They are never removed, so that they can be found by index.
// Deleted functions have the df_deleted flag set.
garray_T def_functions = {0, 0, ଆକାର(dfunc_T), 50, ଖାଲି};

ସ୍ଥିର ଶୂନ୍ୟ delete_def_function_contents(dfunc_T *dfunc, ପୂର୍ଣ୍ଣସଂଖ୍ୟା mark_deleted);

/*
 * Lookup variable "name" in the local scope and return it in "lvar".
 * "lvar->lv_from_outer" is incremented accordingly.
 * If "lvar" is NULL only check if the variable can be found.
 * Return FAIL if not found.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
lookup_local(char_u *name, size_t len, lvar_T *lvar, cctx_T *cctx)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    idx;
    lvar_T  *lvp;

    ଯଦି (len == 0)
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (((len == 4 && STRNCMP(name, "this", 4) == 0)
		|| (len == 5 && STRNCMP(name, "super", 5) == 0))
	    && cctx->ctx_ufunc != ଖାଲି
	    && (cctx->ctx_ufunc->uf_flags & (FC_OBJECT|FC_NEW)))
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_super = *name == 's';
	ଯଦି (is_super)
	{
	    ଯଦି (name[5] != '.')
	    {
		emsg(_(e_super_must_be_followed_by_dot));
		ଫେରନ୍ତୁ FAIL;
	    }
	    ଯଦି (cctx->ctx_ufunc->uf_class != ଖାଲି
		    && cctx->ctx_ufunc->uf_class->class_extends == ଖାଲି)
	    {
		emsg(_(e_using_super_not_in_child_class));
		ଫେରନ୍ତୁ FAIL;
	    }
	}
	ଯଦି (lvar != ଖାଲି)
	{
	    CLEAR_POINTER(lvar);
	    lvar->lv_loop_depth = -1;
	    lvar->lv_name = (char_u *)(is_super ? "super" : "this");
	    ଯଦି (cctx->ctx_ufunc->uf_class != ଖାଲି)
	    {
		lvar->lv_type = &cctx->ctx_ufunc->uf_class->class_object_type;
		ଯଦି (is_super)
		{
		    type_T *type = get_type_ptr(cctx->ctx_type_list);

		    ଯଦି (type != ଖାଲି)
		    {
			*type = *lvar->lv_type;
			lvar->lv_type = type;
			type->tt_flags |= TTFLAG_SUPER;
		    }
		}
	    }
	}
	ଫେରନ୍ତୁ OK;
    }

    // Find local in current function scope.
    ପାଇଁ (idx = 0; idx < cctx->ctx_locals.ga_len; ++idx)
    {
	lvp = ((lvar_T *)cctx->ctx_locals.ga_data) + idx;
	ଯଦି (lvp->lv_name != ଖାଲି
		&& STRNCMP(name, lvp->lv_name, len) == 0
					       && STRLEN(lvp->lv_name) == len)
	{
	    ଯଦି (lvar != ଖାଲି)
	    {
		*lvar = *lvp;
		lvar->lv_from_outer = 0;
		// If the variable was declared inside a loop set
		// lvar->lv_loop_idx and lvar->lv_loop_depth.
		get_loop_var_idx(cctx, idx, lvar);
	    }
	    ଫେରନ୍ତୁ OK;
	}
    }

    // Find local in outer function scope.
    ଯଦି (cctx->ctx_outer != ଖାଲି)
    {
	ଯଦି (lookup_local(name, len, lvar, cctx->ctx_outer) == OK)
	{
	    ଯଦି (lvar != ଖାଲି)
	    {
		cctx->ctx_outer_used = TRUE;
		++lvar->lv_from_outer;
	    }
	    ଫେରନ୍ତୁ OK;
	}
    }

    ଫେରନ୍ତୁ FAIL;
}

/*
 * Lookup an argument in the current function and an enclosing function.
 * Returns the argument index in "idxp"
 * Returns the argument type in "type"
 * Sets "gen_load_outer" to TRUE if found in outer scope.
 * Returns OK when found, FAIL otherwise.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
arg_exists(
	char_u	*name,
	size_t	len,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	*idxp,
	type_T	**type,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	*gen_load_outer,
	cctx_T	*cctx)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    idx;
    char_u  *va_name;

    ଯଦି (len == 0)
	ଫେରନ୍ତୁ FAIL;
    ପାଇଁ (idx = 0; idx < cctx->ctx_ufunc->uf_args_visible; ++idx)
    {
	char_u *arg = FUNCARG(cctx->ctx_ufunc, idx);

	ଯଦି (STRNCMP(name, arg, len) == 0 && arg[len] == NUL)
	{
	    ଯଦି (idxp != ଖାଲି)
	    {
		// Arguments are located above the frame pointer.  One further
		// if there is a vararg argument
		*idxp = idx - (cctx->ctx_ufunc->uf_args.ga_len
							    + ଗଦା_FRAME_SIZE)
			      + (cctx->ctx_ufunc->uf_va_name != ଖାଲି ? -1 : 0);

		ଯଦି (cctx->ctx_ufunc->uf_arg_types != ଖାଲି)
		    *type = cctx->ctx_ufunc->uf_arg_types[idx];
		ଅନ୍ୟଥା
		    *type = &t_any;
	    }
	    ଫେରନ୍ତୁ OK;
	}
    }

    va_name = cctx->ctx_ufunc->uf_va_name;
    ଯଦି (va_name != ଖାଲି
		    && STRNCMP(name, va_name, len) == 0 && va_name[len] == NUL)
    {
	ଯଦି (idxp != ଖାଲି)
	{
	    // varargs is always the last argument
	    *idxp = -ଗଦା_FRAME_SIZE - 1;
	    *type = cctx->ctx_ufunc->uf_va_type;
	}
	ଫେରନ୍ତୁ OK;
    }

    ଯଦି (cctx->ctx_outer != ଖାଲି)
    {
	// Lookup the name for an argument of the outer function.
	ଯଦି (arg_exists(name, len, idxp, type, gen_load_outer, cctx->ctx_outer)
									 == OK)
	{
	    ଯଦି (gen_load_outer != ଖାଲି)
	    {
		++*gen_load_outer;
		cctx->ctx_outer_used = TRUE;
	    }
	    ଫେରନ୍ତୁ OK;
	}
    }

    ଫେରନ୍ତୁ FAIL;
}

/*
 * Lookup a script-local variable in the current script, possibly defined in a
 * block that contains the function "cctx->ctx_ufunc".
 * "cctx" is NULL at the script level, "cstack" is NULL in a function.
 * If "len" is <= 0 "name" must be NUL terminated.
 * Return NULL when not found.
 */
    ସ୍ଥିର sallvar_T *
find_script_var(char_u *name, size_t len, cctx_T *cctx, cstack_T *cstack)
{
    scriptitem_T    *si = SCRIPT_ITEM(current_sctx.sc_sid);
    hashitem_T	    *hi;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    cc;
    sallvar_T	    *sav;
    ufunc_T	    *ufunc;

    // Find the list of all script variables with the right name.
    ଯଦି (len > 0)
    {
	cc = name[len];
	name[len] = NUL;
    }
    hi = hash_find(&si->sn_all_vars.dv_hashtab, name);
    ଯଦି (len > 0)
	name[len] = cc;
    ଯଦି (HASHITEM_EMPTY(hi))
	ଫେରନ୍ତୁ ଖାଲି;

    sav = HI2SAV(hi);
    ଯଦି (sav->sav_block_id == 0)
	// variable defined in the top script scope is always visible
	ଫେରନ୍ତୁ sav;

    ଯଦି (cctx == ଖାଲି)
    {
	ଯଦି (cstack == ଖାଲି)
	    ଫେରନ୍ତୁ ଖାଲି;

	// Not in a function scope, find variable with block ID equal to or
	// smaller than the current block id.  Use "cstack" to go up the block
	// scopes.
	ଯେତେବେଳେ (sav != ଖାଲି)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx;

	    ପାଇଁ (idx = cstack->cs_idx; idx >= 0; --idx)
		ଯଦି (cstack->cs_block_id[idx] == sav->sav_block_id)
		    ଭାଙ୍ଗ;
	    ଯଦି (idx >= 0)
		ଭାଙ୍ଗ;
	    sav = sav->sav_next;
	}
	ଫେରନ୍ତୁ sav;
    }

    // Go over the variables with this name and find one that was visible
    // from the function.
    ufunc = cctx->ctx_ufunc;
    ଯେତେବେଳେ (sav != ଖାଲି)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx;

	ଯଦି (ufunc->uf_block_depth == 0 && sav->sav_block_id == 0)
	    // If the function was defined at the script level (not inside a
	    // block), script-scope variables are always visible.
	    ଫେରନ୍ତୁ sav;

	// Go over the blocks that this function was defined in.  If the
	// variable block ID matches it was visible to the function.
	ପାଇଁ (idx = 0; idx < ufunc->uf_block_depth; ++idx)
	    ଯଦି (ufunc->uf_block_ids[idx] == sav->sav_block_id)
		ଫେରନ୍ତୁ sav;
	sav = sav->sav_next;
    }

    // Not found, variable was not visible.
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * If "name" can be found in the current script set it's "block_id".
 */
    ଶୂନ୍ୟ
update_script_var_block_id(char_u *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା block_id)
{
    scriptitem_T    *si = SCRIPT_ITEM(current_sctx.sc_sid);
    hashitem_T	    *hi;
    sallvar_T	    *sav;

    hi = hash_find(&si->sn_all_vars.dv_hashtab, name);
    ଯଦି (HASHITEM_EMPTY(hi))
	ଫେରନ୍ତୁ;
    sav = HI2SAV(hi);
    sav->sav_block_id = block_id;
}

/*
 * Lookup a variable (without s: prefix) in the current script.
 * "cctx" is NULL at the script level, "cstack" is NULL in a function.
 * Returns OK or FAIL.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
script_var_exists(char_u *name, size_t len, cctx_T *cctx, cstack_T *cstack)
{
    ଯଦି (current_sctx.sc_sid <= 0)
	ଫେରନ୍ତୁ FAIL;
    ଯଦି (current_script_is_vim9())
    {
	// Check script variables that were visible where the function was
	// defined.
	ଯଦି (find_script_var(name, len, cctx, cstack) != ଖାଲି)
	    ଫେରନ୍ତୁ OK;
    }
    ଅନ୍ୟଥା
    {
	hashtab_T	*ht = &SCRIPT_VARS(current_sctx.sc_sid);
	dictitem_T	*di;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cc;

	// Check script variables that are currently visible
	cc = name[len];
	name[len] = NUL;
	di = find_var_in_ht(ht, 0, name, TRUE);
	name[len] = cc;
	ଯଦି (di != ଖାଲି)
	    ଫେରନ୍ତୁ OK;
    }

    ଫେରନ୍ତୁ FAIL;
}

/*
 * Returns the index of a class method or class variable with name "name"
 * accessible in the currently compiled function.
 * If "cl_ret" is not NULL set it to the class.
 * Otherwise return -1.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cctx_class_midx(
    cctx_T  *cctx,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    is_method,
    char_u  *name,
    size_t  len,
    class_T **cl_ret)
{
    ଯଦି (cctx == ଖାଲି || cctx->ctx_ufunc == ଖାଲି
	    || cctx->ctx_ufunc->uf_class == ଖାଲି
	    || cctx->ctx_ufunc->uf_defclass == ଖାଲି)
	ଫେରନ୍ତୁ -1;

    // Search for the class method or variable in the class where the calling
    // function is defined.
    class_T *cl = cctx->ctx_ufunc->uf_defclass;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା m_idx = is_method ? class_method_idx(cl, name, len)
					: class_member_idx(cl, name, len);
    ଯଦି (m_idx < 0)
    {
	cl = cl->class_extends;
	ଯେତେବେଳେ (cl != ଖାଲି)
	{
	    m_idx = is_method ? class_method_idx(cl, name, len)
					: class_member_idx(cl, name, len);
	    ଯଦି (m_idx >= 0)
		ଭାଙ୍ଗ;
	    cl = cl->class_extends;
	}
    }

    ଯଦି (m_idx >= 0)
    {
	ଯଦି (cl_ret != ଖାଲି)
	    *cl_ret = cl;
    }

    ଫେରନ୍ତୁ m_idx;
}

/*
 * Returns the index of a class method with name "name" accessible in the
 * currently compiled function.  Returns -1 if not found.  The class where the
 * method is defined is returned in "cl_ret".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cctx_class_method_idx(
    cctx_T  *cctx,
    char_u  *name,
    size_t  len,
    class_T **cl_ret)
{
    ଫେରନ୍ତୁ cctx_class_midx(cctx, TRUE, name, len, cl_ret);
}

/*
 * Returns the index of a class variable with name "name" accessible in the
 * currently compiled function.  Returns -1 if not found.  The class where the
 * variable is defined is returned in "cl_ret".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cctx_class_member_idx(
    cctx_T  *cctx,
    char_u  *name,
    size_t  len,
    class_T **cl_ret)
{
    ଫେରନ୍ତୁ cctx_class_midx(cctx, FALSE, name, len, cl_ret);
}

/*
 * Return TRUE if "name" is a local variable, argument, script variable or
 * imported.  Also if "name" is "this" and in a class method.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
variable_exists(char_u *name, size_t len, cctx_T *cctx)
{
    ଫେରନ୍ତୁ (cctx != ଖାଲି
		&& (lookup_local(name, len, ଖାଲି, cctx) == OK
		    || arg_exists(name, len, ଖାଲି, ଖାଲି, ଖାଲି, cctx) == OK
		    || (len == 4
			&& cctx->ctx_ufunc != ଖାଲି
			&& (cctx->ctx_ufunc->uf_flags & (FC_OBJECT|FC_NEW))
			&& STRNCMP(name, "this", 4) == 0)))
	    || script_var_exists(name, len, cctx, ଖାଲି) == OK
	    || cctx_class_member_idx(cctx, name, len, ଖାଲି) >= 0
	    || find_imported(name, len, FALSE) != ଖାଲି;
}

/*
 * Return TRUE if "name" is a local variable, argument, script variable,
 * imported or function.  Or commands are being skipped, a declaration may have
 * been skipped then.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
item_exists(char_u *name, size_t len, ପୂର୍ଣ୍ଣସଂଖ୍ୟା cmd UNUSED, cctx_T *cctx)
{
    ଫେରନ୍ତୁ variable_exists(name, len, cctx);
}

/*
 * Check if "p[len]" is already defined, either in script "import_sid" or in
 * compilation context "cctx".
 * "cctx" is NULL at the script level, "cstack" is NULL in a function.
 * Does not check the global namespace.
 * If "is_arg" is TRUE the error message is for an argument name.
 * Return FAIL and give an error if it defined.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_defined(
	char_u	    *p,
	size_t	    len,
	cctx_T	    *cctx,
	cstack_T    *cstack,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    is_arg)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c = p[len];
    ufunc_T	*ufunc = ଖାଲି;

    // underscore argument is OK
    ଯଦି (len == 1 && *p == '_')
	ଫେରନ୍ତୁ OK;

    ଯଦି (script_var_exists(p, len, cctx, cstack) == OK)
    {
	ଯଦି (is_arg)
	    semsg(_(e_argument_already_declared_in_script_str), p);
	ଅନ୍ୟଥା
	    semsg(_(e_variable_already_declared_in_script_str), p);
	ଫେରନ୍ତୁ FAIL;
    }

    ଯଦି (cctx_class_member_idx(cctx, p, len, ଖାଲି) >= 0)
    {
	ଯଦି (is_arg)
	    semsg(_(e_argument_already_declared_in_class_str), p);
	ଅନ୍ୟଥା
	    semsg(_(e_variable_already_declared_in_class_str), p);
	ଫେରନ୍ତୁ FAIL;
    }

    p[len] = NUL;
    ଯଦି ((cctx != ଖାଲି
		&& (lookup_local(p, len, ଖାଲି, cctx) == OK
		    || arg_exists(p, len, ଖାଲି, ଖାଲି, ଖାଲି, cctx) == OK))
	    || find_imported(p, len, FALSE) != ଖାଲି
	    || (ufunc = find_func_even_dead(p, 0)) != ଖାଲି)
    {
	// A local or script-local function can shadow a global function.
	ଯଦି (ufunc == ଖାଲି || ((ufunc->uf_flags & FC_DEAD) == 0
		    && (!func_is_global(ufunc)
					     || (p[0] == 'g' && p[1] == ':'))))
	{
	    ଯଦି (is_arg)
		semsg(_(e_argument_name_shadows_existing_variable_str), p);
	    ଅନ୍ୟଥା
		semsg(_(e_name_already_defined_str), p);
	    p[len] = c;
	    ଫେରନ୍ତୁ FAIL;
	}
    }
    p[len] = c;
    ଫେରନ୍ତୁ OK;
}


/*
 * Return TRUE if "actual" could be "expected" and a runtime typecheck is to be
 * used.  Return FALSE if the types will never match.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
use_typecheck(type_T *actual, type_T *expected)
{
    ଯଦି (actual->tt_type == VAR_ANY
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
	ଫେରନ୍ତୁ TRUE;
    ଯଦି (actual->tt_type == VAR_OBJECT && expected->tt_type == VAR_OBJECT)
	ଫେରନ୍ତୁ TRUE;
    ଯଦି ((actual->tt_type == VAR_LIST
		|| actual->tt_type == VAR_TUPLE
		|| actual->tt_type == VAR_DICT)
	    && actual->tt_type == expected->tt_type)
	// This takes care of a nested list or dict.
	ଫେରନ୍ତୁ use_typecheck(actual->tt_member, expected->tt_member);
    ଫେରନ୍ତୁ FALSE;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
need_type_where(
	type_T	*actual,
	type_T	*expected,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	typechk_flags,	// acceptable types (type check flags)
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	offset,
	where_T	where,
	cctx_T	*cctx,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	silent,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	actual_is_const)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;

    ଯଦି (expected->tt_type != VAR_CLASS && expected->tt_type != VAR_TYPEALIAS)
    {
	ଯଦି (check_type_is_ମୂଲ୍ୟ(actual) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
    }

    ଯଦି (expected == &t_bool && actual != &t_bool
					&& (actual->tt_flags & TTFLAG_BOOL_OK))
    {
	// Using "0", "1" or the result of an expression with "&&" or "||" as a
	// boolean is OK but requires a conversion.
	generate_2BOOL(cctx, FALSE, offset);
	ଫେରନ୍ତୁ OK;
    }

    ret = check_type_maybe(expected, actual, FALSE, where);
    ଯଦି (ret == OK)
	ଫେରନ୍ତୁ OK;

    // If actual a constant a runtime check makes no sense.  If it's
    // null_function it is OK.
    ଯଦି (actual_is_const && ret == MAYBE && actual == &t_func_unknown)
	ଫେରନ୍ତୁ OK;

    // If the actual type can be the expected type add a runtime check.
    ଯଦି (!actual_is_const && ret == MAYBE && use_typecheck(actual, expected))
    {
	generate_TYPECHECK(cctx, expected, typechk_flags, offset,
		where.wt_kind == WT_VARIABLE, where.wt_index);
	ଫେରନ୍ତୁ OK;
    }

    ଯଦି (!silent)
	type_mismatch_where(expected, actual, where);
    ଫେରନ୍ତୁ FAIL;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
need_type(
	type_T	*actual,
	type_T	*expected,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	typechk_flags,	// acceptable types (type check flags)
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	offset,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	arg_idx,
	cctx_T	*cctx,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	silent,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	actual_is_const)
{
    where_T where = WHERE_INIT;

    ଯଦି (arg_idx > 0)
    {
	where.wt_index = arg_idx;
	where.wt_kind = WT_ARGUMENT;
    }
    ଫେରନ୍ତୁ need_type_where(actual, expected, typechk_flags, offset, where,
						cctx, silent, actual_is_const);
}

/*
 * Set type of variable "lvar" to "type".  If the variable is a constant then
 * the type gets TTFLAG_CONST.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
set_var_type(lvar_T *lvar, type_T *type_arg, cctx_T *cctx)
{
    type_T	*type = type_arg;

    ଯଦି (lvar->lv_const == ASSIGN_CONST && (type->tt_flags & TTFLAG_CONST) == 0)
    {
	ଯଦି (type->tt_flags & TTFLAG_STATIC)
	    // entry in static_types[] is followed by const type
	    type = type + 1;
	ଅନ୍ୟଥା
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
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	assign,
	type_T	*type)
{
    lvar_T  *lvar;
    dfunc_T *dfunc;

    ଯଦି (arg_exists(name, len, ଖାଲି, ଖାଲି, ଖାଲି, cctx) == OK)
    {
	emsg_namelen(_(e_str_is_used_as_argument), name, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)len);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    ଯଦି (GA_GROW_FAILS(&cctx->ctx_locals, 1))
	ଫେରନ୍ତୁ ଖାଲି;
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
    ଯଦି (type == &t_unknown || type == &t_any)
	// type not known yet, may be inferred from RHS
	lvar->lv_type = type;
    ଅନ୍ୟଥା
	// may use TTFLAG_CONST
	set_var_type(lvar, type, cctx);

    // Remember the name for debugging.
    ଯଦି (GA_GROW_FAILS(&dfunc->df_var_names, 1))
	ଫେରନ୍ତୁ ଖାଲି;
    ((char_u **)dfunc->df_var_names.ga_data)[lvar->lv_idx] =
						    vim_strsave(lvar->lv_name);
    ++dfunc->df_var_names.ga_len;

    ଫେରନ୍ତୁ lvar;
}

/*
 * If "check_writable" is ASSIGN_CONST give an error if the variable was
 * defined with :final or :const, if "check_writable" is ASSIGN_FINAL give an
 * error if the variable was defined with :const.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_item_writable(svar_T *sv, ପୂର୍ଣ୍ଣସଂଖ୍ୟା check_writable, char_u *name)
{
    ଯଦି ((check_writable == ASSIGN_CONST && sv->sv_const != 0)
	    || (check_writable == ASSIGN_FINAL
					      && sv->sv_const == ASSIGN_CONST))
    {
	semsg(_(e_cannot_change_readonly_variable_str), name);
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Find "name" in script-local items of script "sid".
 * Pass "check_writable" to check_item_writable().
 * "cctx" is NULL at the script level, "cstack" is NULL in a function.
 * Returns the index in "sn_var_vals" if found.
 * If found but not in "sn_var_vals" returns -1.
 * If not found or the variable is not writable returns -2.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_script_item_idx(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    sid,
	char_u	    *name,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    check_writable,
	cctx_T	    *cctx,
	cstack_T    *cstack)
{
    hashtab_T	    *ht;
    dictitem_T	    *di;
    scriptitem_T    *si = SCRIPT_ITEM(sid);
    svar_T	    *sv;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    idx;

    ଯଦି (!SCRIPT_ID_VALID(sid))
	ଫେରନ୍ତୁ -1;
    ଯଦି (sid == current_sctx.sc_sid)
    {
	sallvar_T *sav = find_script_var(name, 0, cctx, cstack);

	ଯଦି (sav == ଖାଲି)
	    ଫେରନ୍ତୁ -2;
	idx = sav->sav_var_vals_idx;
	sv = ((svar_T *)si->sn_var_vals.ga_data) + idx;
	ଯଦି (check_item_writable(sv, check_writable, name) == FAIL)
	    ଫେରନ୍ତୁ -2;
	ଫେରନ୍ତୁ idx;
    }

    // First look the name up in the hashtable.
    ht = &SCRIPT_VARS(sid);
    di = find_var_in_ht(ht, 0, name, TRUE);
    ଯଦି (di == ଖାଲି)
    {
	ଯଦି (si->sn_autoload_prefix != ଖାଲି)
	{
	    hashitem_T *hi;

	    // A variable exported from an autoload script is in the global
	    // variables, we can find it in the all_vars table.
	    hi = hash_find(&si->sn_all_vars.dv_hashtab, name);
	    ଯଦି (!HASHITEM_EMPTY(hi))
		ଫେରନ୍ତୁ HI2SAV(hi)->sav_var_vals_idx;
	}
	ଫେରନ୍ତୁ -2;
    }

    // Now find the svar_T index in sn_var_vals.
    ପାଇଁ (idx = 0; idx < si->sn_var_vals.ga_len; ++idx)
    {
	sv = ((svar_T *)si->sn_var_vals.ga_data) + idx;
	ଯଦି (sv->sv_tv == &di->di_tv)
	{
	    ଯଦି (check_item_writable(sv, check_writable, name) == FAIL)
		ଫେରନ୍ତୁ -2;
	    ଫେରନ୍ତୁ idx;
	}
    }
    ଫେରନ୍ତୁ -1;
}

    ସ୍ଥିର imported_T *
find_imported_in_script(char_u *name, size_t len, ପୂର୍ଣ୍ଣସଂଖ୍ୟା sid)
{
    scriptitem_T    *si;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    idx;

    ଯଦି (!SCRIPT_ID_VALID(sid))
	ଫେରନ୍ତୁ ଖାଲି;
    si = SCRIPT_ITEM(sid);
    ପାଇଁ (idx = 0; idx < si->sn_imports.ga_len; ++idx)
    {
	imported_T *import = ((imported_T *)si->sn_imports.ga_data) + idx;

	ଯଦି (len == 0 ? STRCMP(name, import->imp_name) == 0
		     : STRLEN(import->imp_name) == len
				  && STRNCMP(name, import->imp_name, len) == 0)
	    ଫେରନ୍ତୁ import;
    }
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Find "name" in imported items of the current script.
 * If "len" is 0 use any length that works.
 * If "load" is TRUE and the script was not loaded yet, load it now.
 */
    imported_T *
find_imported(char_u *name, size_t len, ପୂର୍ଣ୍ଣସଂଖ୍ୟା load)
{
    ଯଦି (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	ଫେରନ୍ତୁ ଖାଲି;

    // Skip over "s:" before "s:something" to find the import name.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା off = name[0] == 's' && name[1] == ':' ? 2 : 0;

    imported_T *ret = find_imported_in_script(name + off, len - off,
							  current_sctx.sc_sid);
    ଯଦି (ret != ଖାଲି && load && (ret->imp_flags & IMP_FLAGS_AUTOLOAD))
    {
	scid_T	actual_sid = 0;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	save_emsg_off = emsg_off;

	// "emsg_off" will be set when evaluating an expression silently, but
	// we do want to know about errors in a script.  Also because it then
	// aborts when an error is encountered.
	emsg_off = FALSE;

	// script found before but not loaded yet
	ret->imp_flags &= ~IMP_FLAGS_AUTOLOAD;
	(ଶୂନ୍ୟ)do_source(SCRIPT_ITEM(ret->imp_sid)->sn_name, FALSE,
						       DOSO_NONE, &actual_sid);
	// If the script is a symlink it may be sourced with another name, may
	// need to adjust the script ID for that.
	ଯଦି (actual_sid != 0)
	    ret->imp_sid = actual_sid;

	emsg_off = save_emsg_off;
    }
    ଫେରନ୍ତୁ ret;
}

/*
 * Called when checking for a following operator at "arg".  When the rest of
 * the line is empty or only a comment, peek the next line.  If there is a next
 * line return a pointer to it and set "nextp".
 * Otherwise skip over white space.
 */
    char_u *
may_peek_next_ଧାଡ଼ି(cctx_T *cctx, char_u *arg, char_u **nextp)
{
    char_u *p = skipwhite(arg);

    *nextp = ଖାଲି;
    ଯଦି (*p == NUL || (VIM_ISWHITE(*arg) && vim9_comment_start(p)))
    {
	*nextp = peek_next_ଧାଡ଼ି_from_context(cctx);
	ଯଦି (*nextp != ଖାଲି)
	    ଫେରନ୍ତୁ *nextp;
    }
    ଫେରନ୍ତୁ p;
}

/*
 * Return a pointer to the next line that isn't empty or only contains a
 * comment. Skips over white space.
 * Returns NULL if there is none.
 */
    char_u *
peek_next_ଧାଡ଼ି_from_context(cctx_T *cctx)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା lnum = cctx->ctx_lnum;

    ଯେତେବେଳେ (++lnum < cctx->ctx_ufunc->uf_ଧାଡ଼ିଗୁଡ଼ିକ.ga_len)
    {
	char_u *ଧାଡ଼ି = ((char_u **)cctx->ctx_ufunc->uf_ଧାଡ଼ିଗୁଡ଼ିକ.ga_data)[lnum];
	char_u *p;

	// ignore NULLs inserted for continuation lines
	ଯଦି (ଧାଡ଼ି != ଖାଲି)
	{
	    p = skipwhite(ଧାଡ଼ି);
	    ଯଦି (vim9_bad_comment(p))
		ଫେରନ୍ତୁ ଖାଲି;
	    ଯଦି (*p != NUL && !vim9_comment_start(p))
		ଫେରନ୍ତୁ p;
	}
    }
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Get the next line of the function from "cctx".
 * Skips over empty lines.  Skips over comment lines if "skip_comment" is TRUE.
 * Returns NULL when at the end.
 */
    char_u *
next_ଧାଡ଼ି_from_context(cctx_T *cctx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା skip_comment)
{
    char_u	*ଧାଡ଼ି;

    କର
    {
	++cctx->ctx_lnum;
	ଯଦି (cctx->ctx_lnum >= cctx->ctx_ufunc->uf_ଧାଡ଼ିଗୁଡ଼ିକ.ga_len)
	{
	    ଧାଡ଼ି = ଖାଲି;
	    ଭାଙ୍ଗ;
	}
	ଧାଡ଼ି = ((char_u **)cctx->ctx_ufunc->uf_ଧାଡ଼ିଗୁଡ଼ିକ.ga_data)[cctx->ctx_lnum];
	cctx->ctx_ଧାଡ଼ି_start = ଧାଡ଼ି;
	SOURCING_LNUM = cctx->ctx_lnum + 1;
    } ଯେତେବେଳେ (ଧାଡ଼ି == ଖାଲି || *skipwhite(ଧାଡ଼ି) == NUL
		     || (skip_comment && vim9_comment_start(skipwhite(ଧାଡ଼ି))));
    ଫେରନ୍ତୁ ଧାଡ଼ି;
}

/*
 * Skip over white space at "whitep" and assign to "*arg".
 * If "*arg" is at the end of the line, advance to the next line.
 * Also when "whitep" points to white space and "*arg" is on a "#".
 * Return FAIL if beyond the last line, "*arg" is unmodified then.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
may_get_next_ଧାଡ଼ି(char_u *whitep, char_u **arg, cctx_T *cctx)
{
    *arg = skipwhite(whitep);
    ଯଦି (vim9_bad_comment(*arg))
	ଫେରନ୍ତୁ FAIL;
    ଯଦି (**arg == NUL || (VIM_ISWHITE(*whitep) && vim9_comment_start(*arg)))
    {
	char_u *next = next_ଧାଡ଼ି_from_context(cctx, TRUE);

	ଯଦି (next == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;
	*arg = skipwhite(next);
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Idem, and give an error when failed.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
may_get_next_ଧାଡ଼ି_error(char_u *whitep, char_u **arg, cctx_T *cctx)
{
    ଯଦି (may_get_next_ଧାଡ଼ି(whitep, arg, cctx) == FAIL)
    {
	SOURCING_LNUM = cctx->ctx_lnum + 1;
	emsg(_(e_ଧାଡ଼ି_incomplete));
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Get a line from the compilation context, compatible with exarg_T getline().
 * Return a pointer to the line in allocated memory.
 * Return NULL for end-of-file or some error.
 */
    ସ୍ଥିର char_u *
exarg_getline(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା c UNUSED,
	ଶୂନ୍ୟ *cookie,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା indent UNUSED,
	getline_opt_T options UNUSED)
{
    cctx_T  *cctx = (cctx_T *)cookie;
    char_u  *p;

    ପାଇଁ (;;)
    {
	ଯଦି (cctx->ctx_lnum >= cctx->ctx_ufunc->uf_ଧାଡ଼ିଗୁଡ଼ିକ.ga_len - 1)
	    ଫେରନ୍ତୁ ଖାଲି;
	++cctx->ctx_lnum;
	p = ((char_u **)cctx->ctx_ufunc->uf_ଧାଡ଼ିଗୁଡ଼ିକ.ga_data)[cctx->ctx_lnum];
	// Comment lines result in NULL pointers, skip them.
	ଯଦି (p != ଖାଲି)
	    ଫେରନ୍ତୁ vim_strsave(p);
    }
}

    ଶୂନ୍ୟ
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
func_needs_compiling(ufunc_T *ufunc, compiletype_T compile_type)
{
    ବାଛ (ufunc->uf_def_status)
    {
	ସ୍ଥିତି UF_TO_BE_COMPILED:
	    ଫେରନ୍ତୁ TRUE;

	ସ୍ଥିତି UF_COMPILED:
	{
	    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data)
							 + ufunc->uf_dfunc_idx;

	    ବାଛ (compile_type)
	    {
		ସ୍ଥିତି CT_PROFILE:
#ifdef FEAT_PROFILE
		    ଫେରନ୍ତୁ dfunc->df_instr_prof == ଖାଲି;
#endif
		ସ୍ଥିତି CT_NONE:
		    ଫେରନ୍ତୁ dfunc->df_instr == ଖାଲି;
		ସ୍ଥିତି CT_DEBUG:
		    ଫେରନ୍ତୁ dfunc->df_instr_debug == ଖାଲି;
	    }
	}

	ସ୍ଥିତି UF_NOT_COMPILED:
	ସ୍ଥିତି UF_COMPILE_ERROR:
	ସ୍ଥିତି UF_COMPILING:
	    ଭାଙ୍ଗ;
    }
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Compile a nested :def command.
 */
    ସ୍ଥିର char_u *
compile_nested_function(exarg_T *eap, cctx_T *cctx, garray_T *ଧାଡ଼ିଗୁଡ଼ିକ_to_free)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_global = *eap->arg == 'g' && eap->arg[1] == ':';
    char_u	*name_start = eap->arg;
    char_u	*name_end = to_name_end(eap->arg, TRUE);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		off;
    char_u	*func_name;
    string_T	lambda_name;
    ufunc_T	*ufunc;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		r = FAIL;
    compiletype_T   compile_type;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		funcref_isn_idx = -1;
    lvar_T	*lvar = ଖାଲି;
    char_u	*bracket_start = ଖାଲି;

    ଯଦି (eap->forceit)
    {
	semsg(_(e_cannot_use_bang_with_nested_def_str),
		eap->cmdidx == CMD_def ? ":def" : ":function");
	ଫେରନ୍ତୁ ଖାଲି;
    }

    ଯଦି (*name_start == '/')
    {
	name_end = skip_regexp(name_start + 1, '/', TRUE);
	ଯଦି (*name_end == '/')
	    ++name_end;
	set_nextcmd(eap, name_end);
    }

    ଯଦି (*name_end == '<')
    {
	bracket_start = name_end;
	ଯଦି (skip_generic_func_type_args(&name_end) == FAIL)
	    ଫେରନ୍ତୁ ଖାଲି;
    }

    ଯଦି (name_end == name_start || *skipwhite(name_end) != '(')
    {
	ଯଦି (!ends_excmd2(name_start, name_end))
	{
	    ଯଦି (*skipwhite(name_end) == '.')
		semsg(_(e_cannot_define_dict_func_in_vim9_script_str),
								     eap->cmd);
	    ଅନ୍ୟଥା
		semsg(_(e_invalid_command_str), eap->cmd);
	    ଫେରନ୍ତୁ ଖାଲି;
	}

	// "def" or "def Name": list functions
	ଯଦି (generate_DEF(cctx, name_start, name_end - name_start) == FAIL)
	    ଫେରନ୍ତୁ ଖାଲି;
	ଫେରନ୍ତୁ eap->nextcmd == ଖାଲି ? (char_u *)"" : eap->nextcmd;
    }

    ଯଦି (bracket_start != ଖାଲି)
	// generic function.  The function name ends before the list of types
	// (opening angle bracket).
	name_end = bracket_start;

    // Only g:Func() can use a namespace.
    ଯଦି (name_start[1] == ':' && !is_global)
    {
	semsg(_(e_namespace_not_supported_str), name_start);
	ଫେରନ୍ତୁ ଖାଲି;
    }
    ଯଦି (cctx->ctx_skip != SKIP_YES
	    && check_defined(name_start, name_end - name_start, cctx,
							  ଖାଲି, FALSE) == FAIL)
	ଫେରନ୍ତୁ ଖାଲି;
    ଯଦି (!ASCII_ISUPPER(is_global ? name_start[2] : name_start[0]))
    {
	semsg(_(e_function_name_must_start_with_capital_str), name_start);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    eap->arg = name_end;
    fill_exarg_from_cctx(eap, cctx);

    eap->forceit = FALSE;
    // We use the special <lambda>99 name, but it's not really a lambda.
    lambda_name = get_lambda_name();
    lambda_name.string = vim_strnsave(lambda_name.string, lambda_name.length);
    ଯଦି (lambda_name.string == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    // This may free the current line, make a copy of the name.
    off = is_global ? 2 : 0;
    func_name = vim_strnsave(name_start + off, name_end - name_start - off);
    ଯଦି (func_name == ଖାଲି)
    {
	r = FAIL;
	ଯାଅ theend;
    }

    // A type variable cannot have the name of the function.  The function is
    // defined with the "<lambda>" name, check it here.
    ଯଦି (bracket_start != ଖାଲି)
    {
	size_t	len = STRLEN(func_name);

	ପାଇଁ (char_u *t = bracket_start + 1; *t != '>' && *t != NUL; )
	{
	    char_u *te = to_name_end(t, FALSE);

	    ଯଦି ((size_t)(te - t) == len && STRNCMP(t, func_name, len) == 0)
	    {
		semsg(_(e_redefining_script_item_str), func_name);
		r = FAIL;
		ଯାଅ theend;
	    }
	    t = te == t ? t + 1 : te;
	}
    }

    // Make sure "KeyTyped" is not set, it may cause indent to be written.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_ଚାବି_Typed = ଚାବି_Typed;
    ଚାବି_Typed = FALSE;

    ufunc = define_function(eap, lambda_name.string, ଧାଡ଼ିଗୁଡ଼ିକ_to_free, 0, ଖାଲି, 0,
									cctx);

    ଚାବି_Typed = save_ଚାବି_Typed;

    ଯଦି (ufunc == ଖାଲି)
    {
	r = eap->skip ? OK : FAIL;
	ଯାଅ theend;
    }
    ଯଦି (eap->nextcmd != ଖାଲି)
    {
	semsg(_(e_text_found_after_str_str),
	      eap->cmdidx == CMD_def ? "enddef" : "endfunction", eap->nextcmd);
	r = FAIL;
	func_ptr_unref(ufunc);
	ଯାଅ theend;
    }

    // copy over the block scope IDs before compiling
    ଯଦି (!is_global && cctx->ctx_ufunc->uf_block_depth > 0)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା block_depth = cctx->ctx_ufunc->uf_block_depth;

	ufunc->uf_block_ids = ALLOC_MULT(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, block_depth);
	ଯଦି (ufunc->uf_block_ids != ଖାଲି)
	{
	    mch_memmove(ufunc->uf_block_ids, cctx->ctx_ufunc->uf_block_ids,
						    ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା) * block_depth);
	    ufunc->uf_block_depth = block_depth;
	}
    }

    // Define the funcref before compiling, so that it is found by any
    // recursive call.
    ଯଦି (is_global)
    {
	r = generate_NEWFUNC(cctx, lambda_name.string, func_name);
	func_name = ଖାଲି;
	lambda_name.string = ଖାଲି;
	lambda_name.length = 0;
    }
    ଅନ୍ୟଥା
    {
	// Define a local variable for the function reference.
	lvar = reserve_local(cctx, func_name, name_end - name_start,
					    ASSIGN_CONST, ufunc->uf_func_type);
	ଯଦି (lvar == ଖାଲି)
	{
	    func_ptr_unref(ufunc);
	    ଯାଅ theend;
	}
	lvar->lv_ufunc = ufunc;
	ଯଦି (generate_FUNCREF(cctx, ufunc, ଖାଲି, FALSE, 0, &funcref_isn_idx) == FAIL)
	{
	    func_ptr_unref(ufunc);
	    ଯାଅ theend;
	}
	r = generate_STORE(cctx, ISN_STORE, lvar->lv_idx, ଖାଲି);
    }

    compile_type = get_compile_type(ufunc);
#ifdef FEAT_PROFILE
    // If the outer function is profiled, also compile the nested function for
    // profiling.
    ଯଦି (cctx->ctx_compile_type == CT_PROFILE)
	compile_type = CT_PROFILE;
#endif
    ଯଦି (func_needs_compiling(ufunc, compile_type)
	    && compile_def_function(ufunc, TRUE, compile_type, cctx) == FAIL)
    {
	func_ptr_unref(ufunc);
	ଯଦି (lvar != ଖାଲି)
	    // Now the local variable can't be used.
	    *lvar->lv_name = '/';  // impossible value
	ଯାଅ theend;
    }

#ifdef FEAT_PROFILE
    // When the outer function is compiled for profiling, the nested function
    // may be called without profiling.  Compile it here in the right context.
    ଯଦି (compile_type == CT_PROFILE && func_needs_compiling(ufunc, CT_NONE))
	compile_def_function(ufunc, FALSE, CT_NONE, cctx);
#endif

    // If a FUNCREF instruction was generated, set the index after compiling.
    ଯଦି (funcref_isn_idx != -1 && ufunc->uf_def_status == UF_COMPILED)
    {
	isn_T	*funcref_isn = ((isn_T *)cctx->ctx_instr.ga_data) +
							funcref_isn_idx;
	funcref_isn->isn_arg.funcref.fr_dfunc_idx = ufunc->uf_dfunc_idx;
    }

theend:
    vim_free(lambda_name.string);
    vim_free(func_name);
    ଫେରନ୍ତୁ r == FAIL ? ଖାଲି : (char_u *)"";
}

/*
 * Compile one Vim expression {expr} in string "p".
 * "p" points to the opening "{".
 * Return a pointer to the character after "}", NULL for an error.
 */
    char_u *
compile_one_ପଦସମଷ୍ଟି_in_str(char_u *p, cctx_T *cctx)
{
    char_u	*block_start;
    char_u	*block_end;

    // Skip the opening {.
    block_start = skipwhite(p + 1);
    block_end = block_start;
    ଯଦି (*block_start != NUL && skip_ପଦସମଷ୍ଟି(&block_end, ଖାଲି) == FAIL)
	ଫେରନ୍ତୁ ଖାଲି;
    block_end = skipwhite(block_end);
    // The block must be closed by a }.
    ଯଦି (*block_end != '}')
    {
	semsg(_(e_missing_close_curly_str), p);
	ଫେରନ୍ତୁ ଖାଲି;
    }
    ଯଦି (compile_expr0(&block_start, cctx) == FAIL)
	ଫେରନ୍ତୁ ଖାଲି;
    may_generate_2STRING(-1, TOSTRING_INTERPOLATE, cctx);

    ଫେରନ୍ତୁ block_end + 1;
}

/*
 * Compile a string "str" (either containing a literal string or a mix of
 * literal strings and Vim expressions of the form `{expr}`).  This is used
 * when compiling a heredoc assignment to a variable or an interpolated string
 * in a Vim9 def function.  Vim9 instructions are generated to push strings,
 * evaluate expressions, concatenate them and create a list of lines.  When
 * "evalstr" is TRUE, Vim expressions in "str" are evaluated.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_all_ପଦସମଷ୍ଟି_in_str(char_u *str, ପୂର୍ଣ୍ଣସଂଖ୍ୟା evalstr, cctx_T *cctx)
{
    char_u	*p = str;
    char_u	*val;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଗଣନା_2 = 0;

    ଯଦି (cctx->ctx_skip == SKIP_YES)
	ଫେରନ୍ତୁ OK;

    ଯଦି (!evalstr || *str == NUL)
    {
	// Literal string, possibly empty.
	val = *str != NUL ? vim_strsave(str) : ଖାଲି;
	ଫେରନ୍ତୁ generate_PUSHS(cctx, &val);
    }

    // Push all the string pieces to the stack, followed by a ISN_CONCAT.
    ଯେତେବେଳେ (*p != NUL)
    {
	char_u	*lit_start;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	escaped_brace = FALSE;

	// Look for a block start.
	lit_start = p;
	ଯେତେବେଳେ (*p != '{' && *p != '}' && *p != NUL)
	    ++p;

	ଯଦି (*p != NUL && *p == p[1])
	{
	    // Escaped brace, unescape and continue.
	    // Include the brace in the literal string.
	    ++p;
	    escaped_brace = TRUE;
	}
	ଅନ୍ୟଥା ଯଦି (*p == '}')
	{
	    semsg(_(e_stray_closing_curly_str), str);
	    ଫେରନ୍ତୁ FAIL;
	}

	// Append the literal part.
	ଯଦି (p != lit_start)
	{
	    val = vim_strnsave(lit_start, (size_t)(p - lit_start));
	    ଯଦି (generate_PUSHS(cctx, &val) == FAIL)
		ଫେରନ୍ତୁ FAIL;
	    ++ଗଣନା_2;
	}

	ଯଦି (*p == NUL)
	    ଭାଙ୍ଗ;

	ଯଦି (escaped_brace)
	{
	    // Skip the second brace.
	    ++p;
	    ଜାରି;
	}

	p = compile_one_ପଦସମଷ୍ଟି_in_str(p, cctx);
	ଯଦି (p == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;
	++ଗଣନା_2;
    }

    // Small optimization, if there's only a single piece skip the ISN_CONCAT.
    ଯଦି (ଗଣନା_2 > 1)
	ଫେରନ୍ତୁ generate_CONCAT(cctx, ଗଣନା_2);

    ଫେରନ୍ତୁ OK;
}

/*
 * Return the length of an assignment operator, or zero if there isn't one.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
assignment_len(char_u *p, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *heredoc)
{
    ଯଦି (*p == '=')
    {
	ଯଦି (p[1] == '<' && p[2] == '<')
	{
	    *heredoc = TRUE;
	    ଫେରନ୍ତୁ 3;
	}
	ଫେରନ୍ତୁ 1;
    }
    ଯଦି (vim_strchr((char_u *)"+-*/%", *p) != ଖାଲି && p[1] == '=')
	ଫେରନ୍ତୁ 2;
    ଯଦି (STRNCMP(p, "..=", 3) == 0)
	ଫେରନ୍ତୁ 3;
    ଫେରନ୍ତୁ 0;
}

/*
 * Generate the load instruction for "name".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_loadvar(cctx_T *cctx, lhs_T *lhs)
{
    char_u	*name = lhs->lhs_name;
    type_T	*type = lhs->lhs_type;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		res = OK;

    ବାଛ (lhs->lhs_dest)
    {
	ସ୍ଥିତି dest_option:
	ସ୍ଥିତି dest_func_option:
	    generate_LOAD(cctx, ISN_LOADOPT, 0, name, type);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି dest_global:
	    ଯଦି (vim_strchr(name, AUTOLOAD_CHAR) == ଖାଲି)
	    {
		ଯଦି (name[2] == NUL)
		    generate_instr_type(cctx, ISN_LOADGDICT, &t_dict_any);
		ଅନ୍ୟଥା
		    generate_LOAD(cctx, ISN_LOADG, 0, name + 2, type);
	    }
	    ଅନ୍ୟଥା
		generate_LOAD(cctx, ISN_LOADAUTO, 0, name, type);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି dest_buffer:
	    generate_LOAD(cctx, ISN_LOADB, 0, name + 2, type);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି dest_window:
	    generate_LOAD(cctx, ISN_LOADW, 0, name + 2, type);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି dest_tab:
	    generate_LOAD(cctx, ISN_LOADT, 0, name + 2, type);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି dest_script:
	ସ୍ଥିତି dest_script_v9:
	    res = compile_load_scriptvar(cctx,
			    name + (name[1] == ':' ? 2 : 0), ଖାଲି, ଖାଲି, ଖାଲି);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି dest_env:
	    // Include $ in the name here
	    generate_LOAD(cctx, ISN_LOADENV, 0, name, type);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି dest_reg:
	    generate_LOAD(cctx, ISN_LOADREG, name[1], ଖାଲି, &t_string);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି dest_vimvar:
	    generate_LOADV(cctx, name + 2);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି dest_local:
	    ଯଦି (cctx->ctx_skip != SKIP_YES)
	    {
		lvar_T	*lvar = lhs->lhs_lvar;
		ଯଦି (lvar->lv_from_outer > 0)
		    generate_LOADOUTER(cctx, lvar->lv_idx, lvar->lv_from_outer,
				 lvar->lv_loop_depth, lvar->lv_loop_idx, type);
		ଅନ୍ୟଥା
		    generate_LOAD(cctx, ISN_LOAD, lvar->lv_idx, ଖାଲି, type);
	    }
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି dest_class_member:
	    generate_CLASSMEMBER(cctx, TRUE, lhs->lhs_class,
						     lhs->lhs_classmember_idx);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି dest_ପଦସମଷ୍ଟି:
	    // list or dict value should already be on the stack.
	    ଭାଙ୍ଗ;
    }

    ଫେରନ୍ତୁ res;
}

/*
 * Skip over "[expr]" or ".member".
 * Does not check for any errors.
 */
    ସ୍ଥିର char_u *
skip_index(char_u *start)
{
    char_u *p = start;

    ଯଦି (*p == '[')
    {
	p = skipwhite(p + 1);
	(ଶୂନ୍ୟ)skip_ପଦସମଷ୍ଟି(&p, ଖାଲି);
	p = skipwhite(p);
	ଯଦି (*p == ']')
	    ଫେରନ୍ତୁ p + 1;
	ଫେରନ୍ତୁ p;
    }
    // if (*p == '.')
    ଫେରନ୍ତୁ to_name_end(p + 1, TRUE);
}

    ଶୂନ୍ୟ
vim9_declare_error(char_u *name)
{
    ଅକ୍ଷର *scope = "";

    ବାଛ (*name)
    {
	ସ୍ଥିତି 'g': scope = _("global"); ଭାଙ୍ଗ;
	ସ୍ଥିତି 'b': scope = _("buffer"); ଭାଙ୍ଗ;
	ସ୍ଥିତି 'w': scope = _("window"); ଭାଙ୍ଗ;
	ସ୍ଥିତି 't': scope = _("tab"); ଭାଙ୍ଗ;
	ସ୍ଥିତି 'v': scope = "v:"; ଭାଙ୍ଗ;
	ସ୍ଥିତି '$': semsg(_(e_cannot_declare_an_environment_variable_str), name);
		  ଫେରନ୍ତୁ;
	ସ୍ଥିତି '&': semsg(_(e_cannot_declare_an_option_str), name);
		  ଫେରନ୍ତୁ;
	ସ୍ଥିତି '@': semsg(_(e_cannot_declare_a_register_str), name);
		  ଫେରନ୍ତୁ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ: ଫେରନ୍ତୁ;
    }
    semsg(_(e_cannot_declare_a_scope_variable_str), scope, name);
}

/*
 * Return TRUE if "name" is a valid register to use.
 * Return FALSE and give an error message if not.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
valid_dest_reg(ପୂର୍ଣ୍ଣସଂଖ୍ୟା name)
{
    ଯଦି (name == '@')
       name = '"';
    ଯଦି (name == '/' || name == '=' || name == '#' || valid_yank_reg(name, TRUE))
	ଫେରନ୍ତୁ TRUE;
    emsg_invreg(name);
    ଫେରନ୍ତୁ FAIL;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_var_dest(
	char_u		*name,
	assign_dest_T	*dest,
	cmdidx_T	cmdidx,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		has_index,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*option_scope,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*vimvaridx,
	type_T		**type,
	cctx_T		*cctx)
{
    char_u *p;

    ଯଦି (*name == '&')
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cc;
	long		numval;
	getoption_T	opt_type;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_p_flags;

	*dest = dest_option;
	ଯଦି (cmdidx == CMD_final || cmdidx == CMD_const)
	{
	    emsg(_(e_cannot_lock_option));
	    ଫେରନ୍ତୁ FAIL;
	}
	p = name;
	p = find_option_end(&p, option_scope);
	ଯଦି (p == ଖାଲି)
	{
	    // cannot happen?
	    emsg(_(e_unexpected_characters_in_assignment));
	    ଫେରନ୍ତୁ FAIL;
	}
	cc = *p;
	*p = NUL;
	opt_type = get_option_ମୂଲ୍ୟ(skip_option_env_lead(name),
				   &numval, ଖାଲି, &opt_p_flags, *option_scope);
	*p = cc;
	ବାଛ (opt_type)
	{
	    ସ୍ଥିତି gov_unknown:
		    semsg(_(e_unknown_option_str), name);
		    ଫେରନ୍ତୁ FAIL;
	    ସ୍ଥିତି gov_string:
	    ସ୍ଥିତି gov_hidden_string:
		    ଯଦି (opt_p_flags & P_FUNC)
		    {
			// might be a Funcref, check the type later
			*type = &t_any;
			*dest = dest_func_option;
		    }
		    ଅନ୍ୟଥା
		    {
			*type = &t_string;
		    }
		    ଭାଙ୍ଗ;
	    ସ୍ଥିତି gov_bool:
	    ସ୍ଥିତି gov_hidden_bool:
		    *type = &t_bool;
		    ଭାଙ୍ଗ;
	    ସ୍ଥିତି gov_ସଂଖ୍ୟା:
	    ସ୍ଥିତି gov_hidden_ସଂଖ୍ୟା:
		    *type = &t_ସଂଖ୍ୟା;
		    ଭାଙ୍ଗ;
	}
    }
    ଅନ୍ୟଥା ଯଦି (*name == '$')
    {
	*dest = dest_env;
	*type = &t_string;
    }
    ଅନ୍ୟଥା ଯଦି (*name == '@')
    {
	ଯଦି (!valid_dest_reg(name[1]))
	    ଫେରନ୍ତୁ FAIL;
	*dest = dest_reg;
	*type = name[1] == '#' ? &t_ସଂଖ୍ୟା_or_string : &t_string;
    }
    ଅନ୍ୟଥା ଯଦି (STRNCMP(name, "g:", 2) == 0)
    {
	*dest = dest_global;
    }
    ଅନ୍ୟଥା ଯଦି (STRNCMP(name, "b:", 2) == 0)
    {
	*dest = dest_buffer;
    }
    ଅନ୍ୟଥା ଯଦି (STRNCMP(name, "w:", 2) == 0)
    {
	*dest = dest_window;
    }
    ଅନ୍ୟଥା ଯଦି (STRNCMP(name, "t:", 2) == 0)
    {
	*dest = dest_tab;
    }
    ଅନ୍ୟଥା ଯଦି (STRNCMP(name, "v:", 2) == 0)
    {
	typval_T	*vtv;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		di_flags;

	*vimvaridx = find_vim_var(name + 2, &di_flags);
	ଯଦି (*vimvaridx < 0)
	{
	    semsg(_(e_variable_not_found_str), name);
	    ଫେରନ୍ତୁ FAIL;
	}
	// An item of a read-only variable can be changed, as when not compiled.
	// We use the current value of "sandbox" here, is that OK?
	ଯଦି (!has_index && var_check_ro(di_flags, name, FALSE))
	    ଫେରନ୍ତୁ FAIL;
	*dest = dest_vimvar;
	vtv = get_vim_var_tv(*vimvaridx);
	*type = typval2type_vimvar(vtv, cctx->ctx_type_list);
    }
    ଫେରନ୍ତୁ OK;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
is_decl_command(cmdidx_T cmdidx)
{
    ଫେରନ୍ତୁ cmdidx == CMD_let || cmdidx == CMD_var
				 || cmdidx == CMD_final || cmdidx == CMD_const;
}

/*
 * Returns TRUE if the class or object variable in "lhs" is modifiable.
 * "var_start" points to the start of the variable name and "lhs->lhs_varlen"
 * has the total length.  Note that the "lhs" can be nested an object reference
 * (e.g.  a.b.c.d.var).
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
lhs_class_member_modifiable(lhs_T *lhs, char_u	*var_start, cctx_T *cctx)
{
    size_t	varlen = lhs->lhs_varlen;
    class_T	*cl = lhs->lhs_type->tt_class;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_object = lhs->lhs_type->tt_type == VAR_OBJECT;
    char_u	*name = var_start + varlen + 1;
    size_t	namelen = lhs->lhs_end - var_start - varlen - 1;
    ocmember_T	*m;

    m = member_lookup(cl, lhs->lhs_type->tt_type, name, namelen, ଖାଲି);
    ଯଦି (m == ଖାଲି)
    {
	member_not_found_ସନ୍ଦେଶ(cl, lhs->lhs_type->tt_type, name, namelen);
	ଫେରନ୍ତୁ FALSE;
    }

    ଯଦି (IS_ENUM(cl))
    {
	semsg(_(e_enumvalue_str_cannot_be_modified), cl->class_name.string,
		m->ocm_name.string);
	ଫେରନ୍ତୁ FALSE;
    }

    // If it is private member variable, then accessing it outside the
    // class is not allowed.
    // If it is a read only class variable, then it can be modified
    // only inside the class where it is defined.
    ଯଦି ((m->ocm_access != VIM_ACCESS_ALL) &&
	    ((is_object && !inside_class(cctx, cl))
	     || (!is_object && cctx->ctx_ufunc->uf_class != cl)))
    {
	ଅକ୍ଷର *ସନ୍ଦେଶ = (m->ocm_access == VIM_ACCESS_PRIVATE)
				? e_cannot_access_protected_variable_str
				: e_variable_is_not_writable_str;
	emsg_var_cl_define(ସନ୍ଦେଶ, m->ocm_name.string, 0, cl);
	ଫେରନ୍ତୁ FALSE;
    }

    ଫେରନ୍ତୁ TRUE;
}

/*
 * Initialize "lhs" with default values
 */
    ସ୍ଥିର ଶୂନ୍ୟ
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
lhs_find_var_end(
    lhs_T	*lhs,
    char_u	*var_start,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_decl,
    char_u	**var_endp)
{
    char_u  *var_end = *var_endp;

    // "lhs_dest_end" is the end of the destination, including "[expr]" or
    // ".name".
    // "var_end" is the end of the variable/option/etc. name.
    lhs->lhs_dest_end = skip_var_one(var_start, FALSE);
    ଯଦି (*var_start == '@')
    {
	ଯଦି (!valid_dest_reg(var_start[1]))
	    ଫେରନ୍ତୁ FAIL;
	var_end = var_start + 2;
    }
    ଅନ୍ୟଥା
    {
	// skip over the leading "&", "&l:", "&g:" and "$"
	var_end = skip_option_env_lead(var_start);
	var_end = to_name_end(var_end, TRUE);
    }

    // "a: type" is declaring variable "a" with a type, not dict "a:".
    ଯଦି (is_decl && lhs->lhs_dest_end == var_start + 2
					&& lhs->lhs_dest_end[-1] == ':')
	--lhs->lhs_dest_end;
    ଯଦି (is_decl && var_end == var_start + 2 && var_end[-1] == ':')
	--var_end;

    lhs->lhs_end = lhs->lhs_dest_end;
    *var_endp = var_end;

    ଫେରନ୍ତୁ OK;
}

/*
 * Set various fields in "lhs"
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
lhs_init(
    lhs_T	*lhs,
    char_u	*var_start,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_decl,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		heredoc,
    char_u	**var_endp)
{
    char_u *var_end = *var_endp;

    lhs_init_defaults(lhs);

    // Find the end of the variable and the destination
    ଯଦି (lhs_find_var_end(lhs, var_start, is_decl, &var_end) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    // compute the length of the destination without "[expr]" or ".name"
    lhs->lhs_varlen = var_end - var_start;
    lhs->lhs_varlen_total = lhs->lhs_varlen;
    lhs->lhs_name = vim_strnsave(var_start, lhs->lhs_varlen);
    ଯଦି (lhs->lhs_name == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (lhs->lhs_dest_end > var_start + lhs->lhs_varlen)
	// Something follows after the variable: "var[idx]" or "var.key".
	lhs->lhs_has_index = TRUE;

    lhs->lhs_type = heredoc ? &t_list_string : &t_any;

    *var_endp = var_end;

    ଫେରନ୍ତୁ OK;
}

/*
 * Compile a LHS class variable name.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_lhs_class_variable(
    cctx_T	*cctx,
    lhs_T	*lhs,
    class_T	*defcl,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_decl)
{
    ଯଦି (cctx->ctx_ufunc->uf_defclass != defcl)
    {
	// A class variable can be accessed without the class name
	// only inside a class.
	semsg(_(e_class_variable_str_accessible_only_inside_class_str),
		lhs->lhs_name, defcl->class_name.string);
	ଫେରନ୍ତୁ FAIL;
    }

    ଯଦି (is_decl)
    {
	semsg(_(e_variable_already_declared_in_class_str), lhs->lhs_name);
	ଫେରନ୍ତୁ FAIL;
    }

    ocmember_T	*m = &defcl->class_class_members[lhs->lhs_classmember_idx];
    ଯଦି (oc_var_check_ro(defcl, m))
	ଫେରନ୍ତୁ FAIL;

    lhs->lhs_dest = dest_class_member;
    // The class variable is defined either in the current class or
    // in one of the parent class in the hierarchy.
    lhs->lhs_class = defcl;
    lhs->lhs_type = oc_member_type_by_idx(defcl, FALSE,
						lhs->lhs_classmember_idx);

    ଫେରନ୍ତୁ OK;
}

/*
 * Compile an imported LHS variable
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
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
    ଯଦି (dot == ଖାଲି)
    {
	semsg(_(e_no_dot_after_imported_name_str), var_start);
	ଫେରନ୍ତୁ FAIL;
    }

    p = skipwhite(dot + 1);
    var_end = to_name_end(p, TRUE);
    ଯଦି (var_end == p)
    {
	semsg(_(e_missing_name_after_imported_name_str), var_start);
	ଫେରନ୍ତୁ FAIL;
    }

    vim_free(lhs->lhs_name);
    lhs->lhs_varlen = var_end - p;
    lhs->lhs_name = vim_strnsave(p, lhs->lhs_varlen);
    ଯଦି (lhs->lhs_name == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    *rawnamep = lhs->lhs_name;
    lhs->lhs_scriptvar_sid = import->imp_sid;

    // TODO: where do we check this name is exported?

    // Check if something follows: "exp.var[idx]" or
    // "exp.var.key".
    lhs->lhs_has_index = lhs->lhs_dest_end > skipwhite(var_end);

    *var_endp = var_end;

    ଫେରନ୍ତୁ OK;
}

/*
 * Process a script-local variable when compiling a LHS variable name.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_lhs_script_var(
    cctx_T	*cctx,
    lhs_T	*lhs,
    char_u	*var_start,
    char_u	*var_end,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_decl)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		script_namespace = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		script_var = FALSE;
    imported_T	*import;
    char_u	*var_name;
    size_t	var_name_len;

    ଯଦି (lhs->lhs_varlen > 1 && STRNCMP(var_start, "s:", 2) == 0)
	script_namespace = TRUE;

    ଯଦି (script_namespace)
    {
	var_name = var_start + 2;
	var_name_len = lhs->lhs_varlen - 2;
    }
    ଅନ୍ୟଥା
    {
	var_name = var_start;
	var_name_len = lhs->lhs_varlen;
    }

    ଯଦି (script_var_exists(var_name, var_name_len, cctx, ଖାଲି) == OK)
	script_var = TRUE;

    import = find_imported(var_start, lhs->lhs_varlen, FALSE);

    ଯଦି (script_namespace || script_var || import != ଖାଲି)
    {
	char_u *rawname = lhs->lhs_name + (lhs->lhs_name[1] == ':' ? 2 : 0);

	ଯଦି (script_namespace && current_script_is_vim9())
	{
	    semsg(_(e_cannot_use_s_colon_in_vim9_script_str), var_start);
	    ଫେରନ୍ତୁ FAIL;
	}

	ଯଦି (is_decl)
	{
	    ଯଦି (script_namespace)
		semsg(_(e_cannot_declare_script_variable_in_function_str),
			lhs->lhs_name);
	    ଅନ୍ୟଥା
		semsg(_(e_variable_already_declared_in_script_str),
			lhs->lhs_name);
	    ଫେରନ୍ତୁ FAIL;
	}
	ଅନ୍ୟଥା ଯଦି (cctx->ctx_ufunc->uf_script_ctx_version == SCRIPT_VERSION_VIM9
		&& script_namespace
		&& !script_var && import == ଖାଲି)
	{
	    semsg(_(e_unknown_variable_str), lhs->lhs_name);
	    ଫେରନ୍ତୁ FAIL;
	}

	lhs->lhs_dest = current_script_is_vim9() ? dest_script_v9 :
								dest_script;

	// existing script-local variables should have a type
	lhs->lhs_scriptvar_sid = current_sctx.sc_sid;
	ଯଦି (import != ଖାଲି)
	{
	    ଯଦି (compile_lhs_import_var(lhs, import, var_start, &var_end,
							&rawname) == FAIL)
		ଫେରନ୍ତୁ FAIL;
	}

	ଯଦି (SCRIPT_ID_VALID(lhs->lhs_scriptvar_sid))
	{
	    // Check writable only when no index follows.
	    lhs->lhs_scriptvar_idx = get_script_item_idx(
					lhs->lhs_scriptvar_sid, rawname,
					lhs->lhs_has_index ?  ASSIGN_FINAL :
					ASSIGN_CONST, cctx, ଖାଲି);
	    ଯଦି (lhs->lhs_scriptvar_idx >= 0)
	    {
		scriptitem_T *si = SCRIPT_ITEM(lhs->lhs_scriptvar_sid);
		svar_T	 *sv = ((svar_T *)si->sn_var_vals.ga_data)
						+ lhs->lhs_scriptvar_idx;

		lhs->lhs_type = sv->sv_type;
	    }
	}

	ଫେରନ୍ତୁ OK;
    }

    ଫେରନ୍ତୁ check_defined(var_start, lhs->lhs_varlen, cctx, ଖାଲି, FALSE);
}

/*
 * Compile the LHS destination.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_lhs_var_dest(
    cctx_T	*cctx,
    lhs_T	*lhs,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cmdidx,
    char_u	*var_start,
    char_u	*var_end,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_decl,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		has_cmd)	// "var" before "var_start"
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    declare_error = FALSE;

    ଯଦି (get_var_dest(lhs->lhs_name, &lhs->lhs_dest, cmdidx,
				lhs->lhs_has_index,
				&lhs->lhs_opt_flags, &lhs->lhs_vimvaridx,
				&lhs->lhs_type, cctx) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (lhs->lhs_dest != dest_local && cmdidx != CMD_const
						&& cmdidx != CMD_final)
    {
	// Specific kind of variable recognized.
	declare_error = is_decl;
    }
    ଅନ୍ୟଥା
    {
	class_T	*defcl;

	// No specific kind of variable recognized, just a name.
	ଯଦି (check_reserved_name(lhs->lhs_name, lhs->lhs_has_index
						&& *var_end == '.') == FAIL)
	    ଫେରନ୍ତୁ FAIL;

	ଯଦି (lookup_local(var_start, lhs->lhs_varlen, &lhs->lhs_local_lvar,
								cctx) == OK)
	{
	    lhs->lhs_lvar = &lhs->lhs_local_lvar;
	}
	ଅନ୍ୟଥା
	{
	    CLEAR_FIELD(lhs->lhs_arg_lvar);
	    ଯଦି (arg_exists(var_start, lhs->lhs_varlen,
			&lhs->lhs_arg_lvar.lv_idx, &lhs->lhs_arg_lvar.lv_type,
			&lhs->lhs_arg_lvar.lv_from_outer, cctx) == OK)
	    {
		ଯଦି (is_decl)
		{
		    semsg(_(e_str_is_used_as_argument), lhs->lhs_name);
		    ଫେରନ୍ତୁ FAIL;
		}
		lhs->lhs_lvar = &lhs->lhs_arg_lvar;
	    }
	}

	ଯଦି (lhs->lhs_lvar != ଖାଲି)
	{
	    ଯଦି (is_decl)
	    {
		// if we come here with what looks like an assignment like
		// .= but which has been rejected by assignment_len() from
		// may_compile_assignment give a better error message
		char_u *p = skipwhite(lhs->lhs_end);
		ଯଦି (p[0] == '.' && p[1] == '=')
		    emsg(_(e_dot_equal_not_supported_with_script_version_two));
		ଅନ୍ୟଥା ଯଦି (p[0] == ':' && !has_cmd)
		    // type specified in an assignment without "var"
		    semsg(_(e_trailing_characters_str), p);
		ଅନ୍ୟଥା
		    semsg(_(e_variable_already_declared_str), lhs->lhs_name);
		ଫେରନ୍ତୁ FAIL;
	    }
	}
	ଅନ୍ୟଥା ଯଦି ((lhs->lhs_classmember_idx = cctx_class_member_idx(
			cctx, var_start, lhs->lhs_varlen, &defcl)) >= 0)
	{
	    ଯଦି (compile_lhs_class_variable(cctx, lhs, defcl, is_decl)
		    == FAIL)
		ଫେରନ୍ତୁ FAIL;
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (compile_lhs_script_var(cctx, lhs, var_start, var_end,
			is_decl) == FAIL)
		ଫେରନ୍ତୁ FAIL;
	}
    }

    ଯଦି (declare_error)
    {
	vim9_declare_error(lhs->lhs_name);
	ଫେରନ୍ତୁ FAIL;
    }

    ଫେରନ୍ତୁ OK;
}

/*
 * When compiling a LHS variable name, for a class or an object, set the LHS
 * member type.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_lhs_set_oc_member_type(
    cctx_T	*cctx,
    lhs_T	*lhs,
    char_u	*var_start)
{
    class_T	*cl = lhs->lhs_type->tt_class;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_object = lhs->lhs_type->tt_type == VAR_OBJECT;
    char_u	*name = var_start + lhs->lhs_varlen + 1;
    size_t	namelen = lhs->lhs_end - var_start - lhs->lhs_varlen - 1;

    ocmember_T	*m = member_lookup(cl, lhs->lhs_type->tt_type,
	    name, namelen, &lhs->lhs_member_idx);
    ଯଦି (m == ଖାଲି)
    {
	member_not_found_ସନ୍ଦେଶ(cl, lhs->lhs_type->tt_type, name, namelen);
	ଫେରନ୍ତୁ FAIL;
    }

    ଯଦି (IS_ENUM(cl))
    {
	ଯଦି (!inside_class(cctx, cl))
	{
	    semsg(_(e_enumvalue_str_cannot_be_modified),
		    cl->class_name.string, m->ocm_name.string);
	    ଫେରନ୍ତୁ FAIL;
	}
	ଯଦି (lhs->lhs_type->tt_type == VAR_OBJECT &&
		lhs->lhs_member_idx < 2)
	{
	    ଅକ୍ଷର *ସନ୍ଦେଶ = lhs->lhs_member_idx == 0 ?
		e_enum_str_name_cannot_be_modified :
		e_enum_str_ordinal_cannot_be_modified;
	    semsg(_(ସନ୍ଦେଶ), cl->class_name.string);
	    ଫେରନ୍ତୁ FAIL;
	}
    }

    // If it is private member variable, then accessing it outside the
    // class is not allowed.
    // If it is a read only class variable, then it can be modified
    // only inside the class where it is defined.
    ଯଦି ((m->ocm_access != VIM_ACCESS_ALL) &&
	    ((is_object && !inside_class(cctx, cl))
	     || (!is_object && cctx->ctx_ufunc->uf_defclass != cl)))
    {
	ଅକ୍ଷର *ସନ୍ଦେଶ = (m->ocm_access == VIM_ACCESS_PRIVATE)
	    ? e_cannot_access_protected_variable_str
	    : e_variable_is_not_writable_str;
	emsg_var_cl_define(ସନ୍ଦେଶ, m->ocm_name.string, 0, cl);
	ଫେରନ୍ତୁ FAIL;
    }

    ଯଦି (!IS_CONSTRUCTOR_METHOD(cctx->ctx_ufunc)
	    && oc_var_check_ro(cl, m))
	ଫେରନ୍ତୁ FAIL;

    lhs->lhs_member_type = m->ocm_type;

    ଫେରନ୍ତୁ OK;
}

/*
 * When compiling a LHS variable, set the LHS variable type.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_lhs_set_type(cctx_T *cctx, lhs_T *lhs, char_u *var_end, ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_decl)
{
    ଯଦି (is_decl && *skipwhite(var_end) == ':')
    {
	char_u *p;

	// parse optional type: "let var: type = expr"
	ଯଦି (VIM_ISWHITE(*var_end))
	{
	    semsg(_(e_no_white_space_allowed_before_colon_str), var_end);
	    ଫେରନ୍ତୁ FAIL;
	}

	ଯଦି (!VIM_ISWHITE(var_end[1]))
	{
	    semsg(_(e_white_space_required_after_str_str), ":", var_end);
	    ଫେରନ୍ତୁ FAIL;
	}

	p = skipwhite(var_end + 1);
	lhs->lhs_type = parse_type(&p, cctx->ctx_type_list, cctx->ctx_ufunc, cctx, TRUE);
	ଯଦି (lhs->lhs_type == ଖାଲି
		|| !valid_declaration_type(lhs->lhs_type))
	    ଫେରନ୍ତୁ FAIL;

	lhs->lhs_has_type = TRUE;
	lhs->lhs_end = p;
    }
    ଅନ୍ୟଥା ଯଦି (lhs->lhs_lvar != ଖାଲି)
	lhs->lhs_type = lhs->lhs_lvar->lv_type;

    ଫେରନ୍ତୁ OK;
}

/*
 * Returns TRUE if "lhs" is a concatenable string.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
lhs_concatenable(lhs_T *lhs)
{
    ଫେରନ୍ତୁ lhs->lhs_dest == dest_global
		|| lhs->lhs_has_index
		|| lhs->lhs_type->tt_type == VAR_STRING
		|| lhs->lhs_type->tt_type == VAR_ANY;
}

/*
 * Create a new local variable when compiling a LHS variable.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_lhs_new_local_var(
    cctx_T	*cctx,
    lhs_T	*lhs,
    char_u	*var_start,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cmdidx,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		oplen,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_decl,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		has_cmd,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		heredoc)
{
    ଯଦି (oplen > 1 && !heredoc)
    {
	// +=, /=, etc. require an existing variable
	semsg(_(e_cannot_use_operator_on_new_variable_str), lhs->lhs_name);
	ଫେରନ୍ତୁ FAIL;
    }

    ଯଦି (!is_decl || (lhs->lhs_has_index && !has_cmd
					&& cctx->ctx_skip != SKIP_YES))
    {
	semsg(_(e_unknown_variable_str), lhs->lhs_name);
	ଫେରନ୍ତୁ FAIL;
    }

    // Check the name is valid for a funcref.
    ଯଦି (lhs->lhs_type->tt_type == VAR_FUNC
				|| lhs->lhs_type->tt_type == VAR_PARTIAL)
    {
	ଯଦି (var_wrong_func_name(lhs->lhs_name, TRUE))
	    ଫେରନ୍ତୁ FAIL;
    }

    // New local variable.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା assign;
    ବାଛ (cmdidx)
    {
	ସ୍ଥିତି CMD_final:
	    assign = ASSIGN_FINAL; ଭାଙ୍ଗ;
	ସ୍ଥିତି CMD_const:
	    assign = ASSIGN_CONST; ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    assign = ASSIGN_VAR; ଭାଙ୍ଗ;
    }

    lhs->lhs_lvar = reserve_local(cctx, var_start, lhs->lhs_varlen, assign,
							lhs->lhs_type);
    ଯଦି (lhs->lhs_lvar == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    lhs->lhs_new_local = TRUE;

    ଫେରନ୍ତୁ OK;
}

/*
 * When compiling a LHS variable name, set the LHS member type.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_lhs_set_member_type(
    cctx_T	*cctx,
    lhs_T	*lhs,
    char_u	*var_start,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_decl,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		has_cmd)
{
    lhs->lhs_member_type = lhs->lhs_type;

    ଯଦି (!lhs->lhs_has_index)
	ଫେରନ୍ତୁ OK;

    char_u	*after = var_start + lhs->lhs_varlen;
    char_u	*p;

    // Something follows after the variable: "var[idx]" or "var.key".
    ଯଦି (is_decl && cctx->ctx_skip != SKIP_YES)
    {
	ଯଦି (has_cmd)
	    emsg(_(e_cannot_use_index_when_declaring_variable));
	ଅନ୍ୟଥା
	    semsg(_(e_unknown_variable_str), lhs->lhs_name);
	ଫେରନ୍ତୁ FAIL;
    }

    // Now: var_start[lhs->lhs_varlen] is '[' or '.'
    // Only the last index is used below, if there are others
    // before it generate code for the expression.  Thus for
    // "ll[1][2]" the expression is "ll[1]" and "[2]" is the index.
    ପାଇଁ (;;)
    {
	p = skip_index(after);
	ଯଦି (*p != '[' && *p != '.')
	{
	    lhs->lhs_varlen_total = p - var_start;
	    ଭାଙ୍ଗ;
	}
	after = p;
    }
    ଯଦି (after > var_start + lhs->lhs_varlen)
    {
	lhs->lhs_varlen = after - var_start;
	lhs->lhs_dest = dest_ପଦସମଷ୍ଟି;
	// We don't know the type before evaluating the expression,
	// use "any" until then.  The member index is for the first name,
	// not for the last index.
	lhs->lhs_type = &t_any;
	lhs->lhs_member_idx = -1;
    }

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା use_class = lhs->lhs_type != ଖାଲି
	&& (lhs->lhs_type->tt_type == VAR_CLASS
		|| lhs->lhs_type->tt_type == VAR_OBJECT);

    ଯଦି (lhs->lhs_type == ଖାଲି
	    || (use_class ? lhs->lhs_type->tt_class == ଖାଲି
		: lhs->lhs_type->tt_member == ଖାଲି))
    {
	lhs->lhs_member_type = &t_any;
    }
    ଅନ୍ୟଥା ଯଦି (use_class)
    {
	// for an object or class member get the type of the member
	ଯଦି (compile_lhs_set_oc_member_type(cctx, lhs, var_start) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
    }
    ଅନ୍ୟଥା
	lhs->lhs_member_type = lhs->lhs_type->tt_member;

    ଫେରନ୍ତୁ OK;
}

/*
 * Figure out the LHS type and other properties for an assignment or one item
 * of ":unlet" with an index.
 * Returns OK or FAIL.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_lhs(
	char_u	    *var_start,
	lhs_T	    *lhs,
	cmdidx_T    cmdidx,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    heredoc,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    has_cmd,	    // "var" before "var_start"
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    oplen,
	cctx_T	    *cctx)
{
    char_u	*var_end = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_decl = is_decl_command(cmdidx);

    ଯଦି (lhs_init(lhs, var_start, is_decl, heredoc, &var_end) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (cctx->ctx_skip != SKIP_YES)
    {
	// compile the LHS destination
	ଯଦି (compile_lhs_var_dest(cctx, lhs, cmdidx, var_start, var_end,
						is_decl, has_cmd) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
    }

    // handle "a:name" as a name, not index "name" in "a"
    ଯଦି (lhs->lhs_varlen > 1 || var_start[lhs->lhs_varlen] != ':')
	var_end = lhs->lhs_dest_end;

    ଯଦି (lhs->lhs_dest != dest_option && lhs->lhs_dest != dest_func_option)
    {
	// set the LHS variable type
	ଯଦି (compile_lhs_set_type(cctx, lhs, var_end, is_decl) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
    }

    ଯଦି (oplen == 3 && !heredoc && !lhs_concatenable(lhs))
    {
	emsg(_(e_can_only_concatenate_to_string));
	ଫେରନ୍ତୁ FAIL;
    }

    ଯଦି (lhs->lhs_lvar == ଖାଲି && lhs->lhs_dest == dest_local
						&& cctx->ctx_skip != SKIP_YES)
    {
	ଯଦି (compile_lhs_new_local_var(cctx, lhs, var_start, cmdidx, oplen,
					is_decl, has_cmd, heredoc) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
    }

    ଯଦି (compile_lhs_set_member_type(cctx, lhs, var_start, is_decl, has_cmd)
								== FAIL)
	ଫେରନ୍ତୁ FAIL;

    ଫେରନ୍ତୁ OK;
}

/*
 * Figure out the LHS and check a few errors.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_assign_lhs(
	char_u	    *var_start,
	lhs_T	    *lhs,
	cmdidx_T    cmdidx,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    is_decl,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    heredoc,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    has_cmd,	    // "var" before "var_start"
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    oplen,
	cctx_T	    *cctx)
{
    ଯଦି (compile_lhs(var_start, lhs, cmdidx, heredoc, has_cmd, oplen, cctx)
								       == FAIL)
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (!lhs->lhs_has_index && lhs->lhs_lvar == &lhs->lhs_arg_lvar)
    {
	semsg(_(e_cannot_assign_to_argument_str), lhs->lhs_name);
	ଫେରନ୍ତୁ FAIL;
    }
    ଯଦି (!is_decl && lhs->lhs_lvar != ଖାଲି
			   && lhs->lhs_lvar->lv_const != ASSIGN_VAR
			   && !lhs->lhs_has_index)
    {
	semsg(_(e_cannot_assign_to_constant_str), lhs->lhs_name);
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Return TRUE if "lhs" has a range index: "[expr : expr]".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
has_list_index(char_u *idx_start, cctx_T *cctx)
{
    char_u  *p = idx_start;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    save_skip;

    ଯଦି (*p != '[')
	ଫେରନ୍ତୁ FALSE;

    p = skipwhite(p + 1);
    ଯଦି (*p == ':')
	ଫେରନ୍ତୁ TRUE;

    save_skip = cctx->ctx_skip;
    cctx->ctx_skip = SKIP_YES;
    (ଶୂନ୍ୟ)compile_expr0(&p, cctx);
    cctx->ctx_skip = save_skip;
    ଫେରନ୍ତୁ *skipwhite(p) == ':';
}

/*
 * For an assignment with an index, compile the "idx" in "var[idx]" or "key" in
 * "var.key".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_assign_index(
	char_u	*var_start,
	lhs_T	*lhs,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	*range,
	cctx_T	*cctx)
{
    size_t	varlen = lhs->lhs_varlen;
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		r = OK;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		need_white_before = TRUE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		empty_second;

    p = var_start + varlen;
    ଯଦି (*p == '[')
    {
	p = skipwhite(p + 1);
	ଯଦି (*p == ':')
	{
	    // empty first index, push zero
	    r = generate_PUSHNR(cctx, 0);
	    need_white_before = FALSE;
	}
	ଅନ୍ୟଥା
	    r = compile_expr0(&p, cctx);

	ଯଦି (r == OK && *skipwhite(p) == ':')
	{
	    // unlet var[idx : idx]
	    // blob[idx : idx] = value
	    *range = TRUE;
	    p = skipwhite(p);
	    empty_second = *skipwhite(p + 1) == ']';
	    ଯଦି ((need_white_before && !IS_WHITE_OR_NUL(p[-1]))
		    || (!empty_second && !IS_WHITE_OR_NUL(p[1])))
	    {
		semsg(_(e_white_space_required_before_and_after_str_at_str),
								      ":", p);
		ଫେରନ୍ତୁ FAIL;
	    }
	    p = skipwhite(p + 1);
	    ଯଦି (*p == ']')
		// empty second index, push "none"
		r = generate_PUSHSPEC(cctx, VVAL_NONE);
	    ଅନ୍ୟଥା
		r = compile_expr0(&p, cctx);
	}

	ଯଦି (r == OK && *skipwhite(p) != ']')
	{
	    // this should not happen
	    emsg(_(e_missing_closing_square_brace));
	    r = FAIL;
	}
    }
    ଅନ୍ୟଥା ଯଦି (lhs->lhs_member_idx >= 0)
    {
	// object member index
	r = generate_PUSHNR(cctx, lhs->lhs_member_idx);
    }
    ଅନ୍ୟଥା // if (*p == '.')
    {
	char_u *ଚାବି_end = to_name_end(p + 1, TRUE);
	char_u *ଚାବି = vim_strnsave(p + 1, ଚାବି_end - p - 1);

	r = generate_PUSHS(cctx, &ଚାବି);
    }
    ଫେରନ୍ତୁ r;
}

/*
 * For a LHS with an index, load the variable to be indexed.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_load_lhs(
	lhs_T	*lhs,
	char_u	*var_start,
	type_T	*rhs_type,
	cctx_T	*cctx)
{
    ଯଦି (lhs->lhs_dest == dest_ପଦସମଷ୍ଟି)
    {
	size_t	    varlen = lhs->lhs_varlen;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    c = var_start[varlen];
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    ଧାଡ଼ିଗୁଡ଼ିକ_len = cctx->ctx_ufunc->uf_ଧାଡ଼ିଗୁଡ଼ିକ.ga_len;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    res;

	// Evaluate "ll[expr]" of "ll[expr][idx]".  End the line with a NUL and
	// limit the lines array length to avoid skipping to a following line.
	var_start[varlen] = NUL;
	cctx->ctx_ufunc->uf_ଧାଡ଼ିଗୁଡ଼ିକ.ga_len = cctx->ctx_lnum + 1;
	char_u *p = var_start;
	res = compile_expr0(&p, cctx);
	var_start[varlen] = c;
	cctx->ctx_ufunc->uf_ଧାଡ଼ିଗୁଡ଼ିକ.ga_len = ଧାଡ଼ିଗୁଡ଼ିକ_len;
	ଯଦି (res == FAIL || p != var_start + varlen)
	{
	    // this should not happen
	    ଯଦି (res != FAIL)
		emsg(_(e_missing_closing_square_brace));
	    ଫେରନ୍ତୁ FAIL;
	}

	lhs->lhs_type = cctx->ctx_type_ଗଦା.ga_len == 0 ? &t_void
						  : get_type_on_ଗଦା(cctx, 0);

	ଯଦି (lhs->lhs_type->tt_type == VAR_CLASS
		|| (lhs->lhs_type->tt_type == VAR_OBJECT
		    && lhs->lhs_type != &t_object_any))
	{
	    // Check whether the class or object variable is modifiable
	    ଯଦି (!lhs_class_member_modifiable(lhs, var_start, cctx))
		ଫେରନ୍ତୁ FAIL;
	}

	// Now we can properly check the type.  The variable is indexed, thus
	// we need the member type.  For a class or object we don't know the
	// type yet, it depends on what member is used.
	// The top item in the stack is the Dict, followed by the key and then
	// the type of the value.
	vartype_T vartype = lhs->lhs_type->tt_type;
	type_T *member_type = lhs->lhs_type->tt_member;
	ଯଦି (rhs_type != ଖାଲି && member_type != ଖାଲି
		&& vartype != VAR_OBJECT && vartype != VAR_CLASS
		&& rhs_type != &t_void
		&& need_type(rhs_type, member_type, 0,
					    -3, 0, cctx, FALSE, FALSE) == FAIL)
	    ଫେରନ୍ତୁ FAIL;

	ଫେରନ୍ତୁ OK;
    }

    ଫେରନ୍ତୁ generate_loadvar(cctx, lhs);
}

/*
 * Produce code for loading "lhs" and also take care of an index.
 * Return OK/FAIL.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_load_lhs_with_index(lhs_T *lhs, char_u *var_start, cctx_T *cctx)
{
    ଯଦି (lhs->lhs_type->tt_type == VAR_OBJECT)
    {
	// "this.value": load "this" object and get the value at index for an
	// object or class member get the type of the member.
	// Also for "obj.value".
	char_u *dot = vim_strchr(var_start, '.');
	ଯଦି (dot == ଖାଲି)
	{
	    semsg(_(e_missing_dot_after_object_str), lhs->lhs_name);
	    ଫେରନ୍ତୁ FAIL;
	}

	class_T	*cl = lhs->lhs_type->tt_class;
	type_T	*type = oc_member_type(cl, TRUE, dot + 1,
					  lhs->lhs_end, &lhs->lhs_member_idx);
	ଯଦି (lhs->lhs_member_idx < 0)
	    ଫେରନ୍ତୁ FAIL;

	ଯଦି (dot - var_start == 4 && STRNCMP(var_start, "this", 4) == 0)
	{
	    // load "this"
	    lvar_T  *lvar = lhs->lhs_lvar;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    rc;

	    ଯଦି (lvar->lv_from_outer > 0)
		rc = generate_LOADOUTER(cctx, lvar->lv_idx,
			lvar->lv_from_outer, lvar->lv_loop_depth,
			lvar->lv_loop_idx, type);
	    ଅନ୍ୟଥା
		rc = generate_LOAD(cctx, ISN_LOAD, lvar->lv_idx, ଖାଲି, type);

	    ଯଦି (rc == FAIL)
		ଫେରନ୍ତୁ FAIL;
	}
	ଅନ୍ୟଥା
	{
	    // load object variable or argument
	    ଯଦି (compile_load_lhs(lhs, var_start, lhs->lhs_type, cctx) == FAIL)
		ଫେରନ୍ତୁ FAIL;
	}
	ଯଦି (IS_INTERFACE(cl))
	    ଫେରନ୍ତୁ generate_GET_ITF_MEMBER(cctx, cl, lhs->lhs_member_idx, type);
	ଫେରନ୍ତୁ generate_GET_OBJ_MEMBER(cctx, lhs->lhs_member_idx, type);
    }
    ଅନ୍ୟଥା ଯଦି (lhs->lhs_type->tt_type == VAR_CLASS)
    {
	// "<classname>.value": load class variable "classname.value"
	char_u *dot = vim_strchr(var_start, '.');
	ଯଦି (dot == ଖାଲି)
	{
	    (ଶୂନ୍ୟ)check_type_is_ମୂଲ୍ୟ(lhs->lhs_type);
	    ଫେରନ୍ତୁ FAIL;
	}

	class_T	*cl = lhs->lhs_type->tt_class;
	ocmember_T *m = class_member_lookup(cl, dot + 1,
						lhs->lhs_end - dot - 1,
						&lhs->lhs_member_idx);
	ଯଦି (m == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;

	ଫେରନ୍ତୁ generate_CLASSMEMBER(cctx, TRUE, cl, lhs->lhs_member_idx);
    }

    ଯଦି (compile_load_lhs(lhs, var_start, ଖାଲି, cctx) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (lhs->lhs_has_index)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	range = FALSE;
	type_T	*type = get_type_on_ଗଦା(cctx, 0);

	// A member of an object or class is not obtained by indexing it.
	ଯଦି (type->tt_type == VAR_CLASS
		|| (type->tt_type == VAR_OBJECT && type != &t_object_any))
	{
	    char_u *p = var_start + lhs->lhs_varlen;

	    ଫେରନ୍ତୁ compile_class_object_index(cctx, &p, type);
	}

	// Get member from list or dict.  First compile the
	// index value.
	ଯଦି (compile_assign_index(var_start, lhs, &range, cctx) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
	ଯଦି (range)
	{
	    semsg(_(e_cannot_use_range_with_assignment_operator_str),
								    var_start);
	    ଫେରନ୍ତୁ FAIL;
	}

	// Get the member.
	ଯଦି (compile_member(FALSE, ଖାଲି, cctx) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Assignment to a list or dict member, or ":unlet" for the item, using the
 * information in "lhs".
 * Returns OK or FAIL.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_assign_unlet(
    char_u	*var_start,
    lhs_T	*lhs,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_assign,
    type_T	*rhs_type,
    cctx_T	*cctx)
{
    vartype_T	dest_type;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		range = FALSE;

    ଯଦି (compile_assign_index(var_start, lhs, &range, cctx) == FAIL)
	ଫେରନ୍ତୁ FAIL;
    ଯଦି (is_assign && range
	    && lhs->lhs_type->tt_type != VAR_LIST
	    && lhs->lhs_type != &t_blob
	    && lhs->lhs_type != &t_any)
    {
	ଯଦି (lhs->lhs_type->tt_type == VAR_TUPLE)
	    emsg(_(e_cannot_slice_tuple));
	ଅନ୍ୟଥା
	    semsg(_(e_cannot_use_range_with_assignment_str), var_start);
	ଫେରନ୍ତୁ FAIL;
    }

    // For "expr[idx]" the index is compiled before the expression, thus the
    // resulting type cannot be used here.
    ଯଦି (lhs->lhs_dest == dest_ପଦସମଷ୍ଟି
	    || lhs->lhs_type == ଖାଲି || lhs->lhs_type == &t_any)
    {
	// Index on variable of unknown type: check at runtime.
	dest_type = VAR_ANY;
    }
    ଅନ୍ୟଥା
    {
	dest_type = lhs->lhs_type->tt_type;
	ଯଦି (dest_type == VAR_DICT && range)
	{
	    emsg(_(e_cannot_use_range_with_dictionary));
	    ଫେରନ୍ତୁ FAIL;
	}
	ଯଦି (dest_type == VAR_DICT
		&& may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
	ଯଦି (dest_type == VAR_LIST || dest_type == VAR_BLOB)
	{
	    type_T *type;

	    ଯଦି (range)
	    {
		type = get_type_on_ଗଦା(cctx, 1);
		ଯଦି (need_type(type, &t_ସଂଖ୍ୟା, 0,
					    -2, 0, cctx, FALSE, FALSE) == FAIL)
		ଫେରନ୍ତୁ FAIL;
	    }
	    type = get_type_on_ଗଦା(cctx, 0);
	    ଯଦି ((dest_type != VAR_BLOB && type->tt_type != VAR_SPECIAL)
		    && need_type(type, &t_ସଂଖ୍ୟା, 0,
					    -1, 0, cctx, FALSE, FALSE) == FAIL)
		ଫେରନ୍ତୁ FAIL;
	}
    }

    ଯଦି (cctx->ctx_skip == SKIP_YES)
	ଫେରନ୍ତୁ OK;

    // Load the dict, list or object.  On the stack we then have:
    // - value (for assignment, not for :unlet)
    // - index
    // - for [a : b] second index
    // - variable
    ଯଦି (compile_load_lhs(lhs, var_start, rhs_type, cctx) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (dest_type == VAR_LIST
	    || dest_type == VAR_DICT
	    || dest_type == VAR_BLOB
	    || dest_type == VAR_CLASS
	    || dest_type == VAR_OBJECT
	    || dest_type == VAR_ANY)
    {
	ଯଦି (is_assign)
	{
	    ଯଦି (range)
	    {
		ଯଦି (generate_instr_drop(cctx, ISN_STORERANGE, 4) == ଖାଲି)
		    ଫେରନ୍ତୁ FAIL;
	    }
	    ଅନ୍ୟଥା
	    {
		isn_T	*isn = generate_instr_drop(cctx, ISN_STOREINDEX, 3);

		ଯଦି (isn == ଖାଲି)
		    ଫେରନ୍ତୁ FAIL;
		isn->isn_arg.storeindex.si_vartype = dest_type;
		isn->isn_arg.storeindex.si_class = ଖାଲି;

		ଯଦି (dest_type == VAR_OBJECT)
		{
		    class_T *cl = lhs->lhs_type->tt_class;

		    ଯଦି (IS_INTERFACE(cl))
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
	ଅନ୍ୟଥା ଯଦି (range)
	{
	    ଯଦି (generate_instr_drop(cctx, ISN_UNLETRANGE, 3) == ଖାଲି)
		ଫେରନ୍ତୁ FAIL;
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (generate_instr_drop(cctx, ISN_UNLETINDEX, 2) == ଖାଲି)
		ଫେରନ୍ତୁ FAIL;
	}
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (dest_type == VAR_TUPLE)
	    emsg(_(e_tuple_is_immutable));
	ଅନ୍ୟଥା
	    emsg(_(e_indexable_type_required));
	ଫେରନ୍ତୁ FAIL;
    }

    ଫେରନ୍ତୁ OK;
}

/*
 * Generate an instruction to push the default value for "vartype".
 * if "dest_local" is TRUE then for some types no instruction is generated.
 * "skip_store" is set to TRUE if no PUSH instruction is generated.
 * Returns OK or FAIL.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ଠେଲ_default_ମୂଲ୍ୟ(
	cctx_T	    *cctx,
	vartype_T   vartype,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    dest_is_local,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    *skip_store)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା r = OK;

    ବାଛ (vartype)
    {
	ସ୍ଥିତି VAR_BOOL:
	    r = generate_PUSHBOOL(cctx, VVAL_FALSE);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_FLOAT:
	    r = generate_PUSHF(cctx, 0.0);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_STRING:
	    r = generate_PUSHS(cctx, ଖାଲି);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_BLOB:
	    r = generate_PUSHBLOB(cctx, blob_alloc());
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_FUNC:
	    r = generate_PUSHFUNC(cctx, ଖାଲି, &t_func_void, TRUE);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_LIST:
	    r = generate_NEWLIST(cctx, 0, FALSE);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_TUPLE:
	    r = generate_NEWTUPLE(cctx, 0, FALSE);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_DICT:
	    r = generate_NEWDICT(cctx, 0, FALSE);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_JOB:
	    r = generate_PUSHJOB(cctx);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_CHANNEL:
	    r = generate_PUSHCHANNEL(cctx);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_OBJECT:
	    r = generate_PUSHOBJ(cctx);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_ସଂଖ୍ୟା:
	ସ୍ଥିତି VAR_UNKNOWN:
	ସ୍ଥିତି VAR_ANY:
	ସ୍ଥିତି VAR_PARTIAL:
	ସ୍ଥିତି VAR_VOID:
	ସ୍ଥିତି VAR_INSTR:
	ସ୍ଥିତି VAR_CLASS:
	ସ୍ଥିତି VAR_TYPEALIAS:
	ସ୍ଥିତି VAR_SPECIAL:  // cannot happen
	    // This is skipped for local variables, they are always
	    // initialized to zero.  But in a "for" or "while" loop
	    // the value may have been changed.
	    ଯଦି (dest_is_local && !inside_loop_scope(cctx))
		*skip_store = TRUE;
	    ଅନ୍ୟଥା
		r = generate_PUSHNR(cctx, 0);
	    ଭାଙ୍ଗ;
    }
    ଫେରନ୍ତୁ r;
}

/*
 * Compile assignment context.  Used when compiling an assignment statement.
 */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା cac_S cac_T;
ସଂରଚନା cac_S
{
    cmdidx_T	cac_cmdidx;		// assignment command
    char_u	*cac_nextc;		// next character to parse
    lhs_T	cac_lhs;		// lhs of the assignment
    type_T	*cac_rhs_type;		// rhs type of an assignment
    char_u	*cac_op;		// assignment operator
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cac_oplen;		// assignment operator length
    char_u	*cac_var_start;		// start of the variable names
    char_u	*cac_var_end;		// end of the variable names
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cac_var_ଗଣନା;		// number of variables in assignment
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cac_var_idx;		// variable index in a list
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cac_semicolon;		// semicolon in [var1, var2; var3]
    garray_T	*cac_instr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cac_instr_ଗଣନା;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cac_incdec;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cac_did_generate_slice;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cac_is_decl;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cac_is_const;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cac_start_lnum;
    type_T	*cac_inferred_type;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cac_skip_store;
};

/*
 * Initialize the compile assignment context.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
compile_assign_context_init(cac_T *cac, cctx_T *cctx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା cmdidx, char_u *arg)
{
    CLEAR_FIELD(*cac);
    cac->cac_cmdidx = cmdidx;
    cac->cac_instr = &cctx->ctx_instr;
    cac->cac_rhs_type = &t_any;
    cac->cac_is_decl = is_decl_command(cmdidx);
    cac->cac_start_lnum = SOURCING_LNUM;
    cac->cac_instr_ଗଣନା = -1;
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_assign_obj_new_arg(char_u **argp, cctx_T *cctx)
{
    char_u *arg = *argp;

    arg += 11;	    // skip "ifargisset"
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା def_arg_idx = getdigits(&arg);
    arg = skipwhite(arg);

    // Use a JUMP_IF_ARG_NOT_SET instruction to skip if the value was not
    // given and the default value is "v:none".
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଦା_offset = ଗଦା_FRAME_SIZE +
				(cctx->ctx_ufunc->uf_va_name != ଖାଲି ? 1 : 0);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା def_arg_ଗଣନା = cctx->ctx_ufunc->uf_def_args.ga_len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା arg_offset = def_arg_idx - def_arg_ଗଣନା - ଗଦା_offset;

    ଯଦି (generate_JUMP_IF_ARG(cctx, ISN_JUMP_IF_ARG_NOT_SET,
			     arg_offset) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    *argp = arg;
    ଫେରନ୍ତୁ OK;
}

/*
 * Translate the increment (++) and decrement (--) operators to the
 * corresponding compound operators (+= or -=).
 *
 * Returns OK on success and FAIL on syntax error.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
translate_incdec_op(exarg_T *eap, cac_T *cac)
{
    ଯଦି (VIM_ISWHITE(eap->cmd[2]))
    {
	semsg(_(e_no_white_space_allowed_after_str_str),
		eap->cmdidx == CMD_increment ? "++" : "--", eap->cmd);
	ଫେରନ୍ତୁ FAIL;
    }
    cac->cac_op = (char_u *)(eap->cmdidx == CMD_increment ? "+=" : "-=");
    cac->cac_oplen = 2;
    cac->cac_incdec = TRUE;

    ଫେରନ୍ତୁ OK;
}

/*
 * Process the operator in an assignment statement.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_assign_process_operator(
    exarg_T	*eap,
    char_u	*arg,
    cac_T	*cac,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*heredoc,
    char_u	**retstr)
{
    *retstr = ଖାଲି;

    ଯଦି (eap->cmdidx == CMD_increment || eap->cmdidx == CMD_decrement)
	// Change an unary operator to a compound operator
	ଫେରନ୍ତୁ translate_incdec_op(eap, cac);

    char_u *sp = cac->cac_nextc;
    cac->cac_nextc = skipwhite(cac->cac_nextc);
    cac->cac_op = cac->cac_nextc;
    cac->cac_oplen = assignment_len(cac->cac_nextc, heredoc);

    ଯଦି (cac->cac_var_ଗଣନା > 0 && cac->cac_oplen == 0)
    {
	// can be something like "[1, 2]->func()"
	*retstr = arg;
	ଫେରନ୍ତୁ FAIL;
    }

    // need white space before and after the operator
    ଯଦି (cac->cac_oplen > 0 && (!VIM_ISWHITE(*sp)
		|| !IS_WHITE_OR_NUL(cac->cac_op[cac->cac_oplen])))
    {
	error_white_both(cac->cac_op, cac->cac_oplen);
	ଫେରନ୍ତୁ FAIL;
    }

    ଫେରନ୍ତୁ OK;
}

/*
 * Find the start of an assignment statement.
 */
    ସ୍ଥିର char_u *
compile_assign_compute_start(char_u *arg, ପୂର୍ଣ୍ଣସଂଖ୍ୟା var_ଗଣନା)
{
    ଯଦି (var_ଗଣନା > 0)
	// [var1, var2] = [val1, val2]
	// skip over the "["
	ଫେରନ୍ତୁ skipwhite(arg + 1);

    ଫେରନ୍ତୁ arg;
}

/*
 * Parse a heredoc assignment starting at "p".  Returns a pointer to the
 * beginning of the heredoc content.
 */
    ସ୍ଥିର char_u *
parse_heredoc_assignment(exarg_T *eap, cctx_T *cctx, cac_T *cac)
{
    // [let] varname =<< [trim] {end}
    eap->ea_getline = exarg_getline;
    eap->cookie = cctx;

    list_T *l = heredoc_get(eap, cac->cac_nextc + 3, FALSE, TRUE);
    ଯଦି (l == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    list_free(l);
    cac->cac_nextc += STRLEN(cac->cac_nextc);

    ଫେରନ୍ତୁ cac->cac_nextc;
}

/*
 * Check the type of a RHS expression in a list assignment statement.
 * The RHS expression is already compiled.  So the type is on the stack.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_assign_list_check_rhs_type(cctx_T *cctx, cac_T *cac)
{
    type_T	*stacktype;

    stacktype = cctx->ctx_type_ଗଦା.ga_len == 0 ? &t_void
						: get_type_on_ଗଦା(cctx, 0);
    ଯଦି (stacktype->tt_type == VAR_VOID)
    {
	emsg(_(e_cannot_use_void_ମୂଲ୍ୟ));
	ଫେରନ୍ତୁ FAIL;
    }

    ଯଦି (stacktype->tt_type != VAR_LIST && stacktype->tt_type != VAR_TUPLE
					&& stacktype->tt_type != VAR_ANY)
    {
	emsg(_(e_list_or_tuple_required));
	ଫେରନ୍ତୁ FAIL;
    }

    ଯଦି (need_type(stacktype,
		  stacktype->tt_type == VAR_TUPLE ? &t_tuple_any : &t_list_any,
		  TYPECHK_TUPLE_OK, -1, 0, cctx, FALSE, FALSE) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    // The check accepts both a list and a tuple. Keep "any", making it a list
    // would reject a tuple later on.
    ଯଦି (stacktype->tt_type == VAR_ANY)
	set_type_on_ଗଦା(cctx, &t_any, 0);

    ଯଦି (stacktype->tt_type == VAR_TUPLE)
    {
	ଯଦି (stacktype->tt_argcount != 1)
	    cac->cac_rhs_type = &t_any;
	ଅନ୍ୟଥା
	{
	    ଯଦି (stacktype->tt_flags & TTFLAG_VARARGS)
		cac->cac_rhs_type = stacktype->tt_args[0]->tt_member;
	    ଅନ୍ୟଥା
		cac->cac_rhs_type = stacktype->tt_args[0];
	}
    }
    ଅନ୍ୟଥା ଯଦି (stacktype->tt_member != ଖାଲି)
	cac->cac_rhs_type = stacktype->tt_member;

    ଫେରନ୍ତୁ OK;
}

/*
 * In a list assignment statement, if a constant list was used, check the
 * length.  Returns OK if the length check succeeds.  Returns FAIL otherwise.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_assign_list_check_length(cctx_T *cctx, cac_T *cac)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	needed_list_len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	did_check = FALSE;

    needed_list_len = cac->cac_semicolon
				? cac->cac_var_ଗଣନା - 1
				: cac->cac_var_ଗଣନା;
    ଯଦି (cac->cac_instr->ga_len > 0)
    {
	isn_T	*isn = ((isn_T *)cac->cac_instr->ga_data) +
						cac->cac_instr->ga_len - 1;

	ଯଦି (isn->isn_type == ISN_NEWLIST || isn->isn_type == ISN_NEWTUPLE)
	{
	    did_check = TRUE;
	    ଯଦି (cac->cac_semicolon ?
			isn->isn_arg.ସଂଖ୍ୟା < needed_list_len
			: isn->isn_arg.ସଂଖ୍ୟା != needed_list_len)
	    {
		semsg(_(e_expected_nr_ବସ୍ତୁଗୁଡ଼ିକ_but_got_nr),
			needed_list_len, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)isn->isn_arg.ସଂଖ୍ୟା);
		ଫେରନ୍ତୁ FAIL;
	    }
	}
    }

    ଯଦି (!did_check)
	generate_CHECKLEN(cctx, needed_list_len, cac->cac_semicolon);

    ଫେରନ୍ତୁ OK;
}

/*
 * Evaluate the expression for "[var, var] = expr" assignment.
 * A line break may follow the assignment operator "=".
 */
    ସ୍ଥିର char_u *
compile_assign_list_ପଦସମଷ୍ଟି(cctx_T *cctx, cac_T *cac)
{
    char_u *whitep;

    whitep = cac->cac_op + cac->cac_oplen;

    ଯଦି (may_get_next_ଧାଡ଼ି_error(whitep, &cac->cac_nextc, cctx) == FAIL)
	ଫେରନ୍ତୁ ଖାଲି;

    // compile RHS expression
    ଯଦି (compile_expr0(&cac->cac_nextc, cctx) == FAIL)
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (cctx->ctx_skip == SKIP_YES)
	// no need to parse more when skipping
	ଫେରନ୍ତୁ cac->cac_nextc;

    ଯଦି (compile_assign_list_check_rhs_type(cctx, cac) == FAIL)
	ଫେରନ୍ତୁ ଖାଲି;

    // If a constant list was used we can check the length right here.
    ଯଦି (compile_assign_list_check_length(cctx, cac) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    ଫେରନ୍ତୁ cac->cac_nextc;
}

/*
 * Find and return the end of a heredoc or a list of variables assignment
 * statement.  For a single variable assignment statement, returns the current
 * end.
 * Returns NULL on failure.
 */
    ସ୍ଥିର char_u *
compile_assign_compute_end(
    exarg_T	*eap,
    cctx_T	*cctx,
    cac_T	*cac,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		heredoc)
{
    ଯଦି (heredoc)
    {
	cac->cac_nextc = parse_heredoc_assignment(eap, cctx, cac);
	ଫେରନ୍ତୁ cac->cac_nextc;
    }

    ଯଦି (cac->cac_var_ଗଣନା > 0)
    {
	// for "[var, var] = expr" evaluate the expression. The list of
	// variables are processed later.
	// A line break may follow the "=".
	cac->cac_nextc = compile_assign_list_ପଦସମଷ୍ଟି(cctx, cac);
	ଫେରନ୍ତୁ cac->cac_nextc;
    }

    ଫେରନ୍ତୁ cac->cac_var_end;
}

/*
 * For "var = expr" evaluate the expression.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_assign_single_eval_ପଦସମଷ୍ଟି(cctx_T *cctx, cac_T *cac)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret = OK;
    char_u	*whitep;
    lhs_T	*lhs = &cac->cac_lhs;
    lvar_T	*lvp;
    lvar_T	save_lhs_lvar;

    // Compile the expression.
    ଯଦି (cac->cac_incdec)
	ଫେରନ୍ତୁ generate_PUSHNR(cctx, 1);

    // Temporarily hide the new local variable here, it is
    // not available to this expression.
    ଯଦି (lhs->lhs_new_local)
    {
	--cctx->ctx_locals.ga_len;

	// Save the local variable value (compiling the RHS expression may
	// create new local variables).
	lvp = ((lvar_T *)cctx->ctx_locals.ga_data) + cctx->ctx_locals.ga_len;
	save_lhs_lvar = *lvp;
    }
    whitep = cac->cac_op + cac->cac_oplen;

    ଯଦି (may_get_next_ଧାଡ଼ି_error(whitep, &cac->cac_nextc, cctx) == FAIL)
    {
	ଯଦି (lhs->lhs_new_local)
	    ++cctx->ctx_locals.ga_len;
	ଫେରନ୍ତୁ FAIL;
    }

    ret = compile_expr0_ext(&cac->cac_nextc, cctx, &cac->cac_is_const);
    ଯଦି (lhs->lhs_new_local)
    {
	// Restore the local variable value.  Update lhs_lvar as the index of
	// the local variable might have changed.
	lvp = ((lvar_T *)cctx->ctx_locals.ga_data) + cctx->ctx_locals.ga_len;
	*lvp = save_lhs_lvar;
	lhs->lhs_lvar = lvp;

	++cctx->ctx_locals.ga_len;
    }

    ଫେରନ୍ତୁ ret;
}

/*
 * When compiling an assignment, set the LHS type to the RHS type.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_assign_set_lhs_type_from_rhs(
    cctx_T	*cctx,
    cac_T	*cac,
    lhs_T	*lhs,
    type_T	*rhs_type)
{
    ଯଦି (rhs_type->tt_type == VAR_VOID)
    {
	emsg(_(e_cannot_use_void_ମୂଲ୍ୟ));
	ଫେରନ୍ତୁ FAIL;
    }

    type_T *type;

    // An empty list or dict has a &t_unknown member, for a variable that
    // implies &t_any.
    ଯଦି (rhs_type == &t_list_empty)
	type = &t_list_any;
    ଅନ୍ୟଥା ଯଦି (rhs_type == &t_dict_empty)
	type = &t_dict_any;
    ଅନ୍ୟଥା ଯଦି (rhs_type == &t_unknown)
	type = &t_any;
    ଅନ୍ୟଥା
    {
	type = rhs_type;
	cac->cac_inferred_type = rhs_type;
    }

    set_var_type(lhs->lhs_lvar, type, cctx);

    ଫେରନ୍ତୁ OK;
}

/*
 * Returns TRUE if the "rhs_type" can be assigned to the "lhs" variable.
 * Used when compiling an assignment statement.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
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
    ଯଦି (cac->cac_var_ଗଣନା > 0)
    {
	where.wt_index = cac->cac_var_idx + 1;
	where.wt_kind = WT_VARIABLE;
    }

    // If assigning to a list or dict member, use the member type.
    // Not for "list[:] =".
    ଯଦି (lhs->lhs_has_index &&
	    !has_list_index(cac->cac_var_start + lhs->lhs_varlen, cctx))
	use_type = lhs->lhs_member_type;

    ଯଦି (need_type_where(rhs_type, use_type, 0, -1, where, cctx, FALSE,
						cac->cac_is_const) == FAIL)
	ଫେରନ୍ତୁ FALSE;

    ଫେରନ୍ତୁ TRUE;
}

/*
 * Compare the LHS type with the RHS type in an assignment.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_assign_check_type(cctx_T *cctx, cac_T *cac)
{
    lhs_T	*lhs = &cac->cac_lhs;
    type_T	*rhs_type;

    rhs_type = cctx->ctx_type_ଗଦା.ga_len == 0
					? &t_void
					: get_type_on_ଗଦା(cctx, 0);
    cac->cac_rhs_type = rhs_type;

    ଯଦି (check_type_is_ମୂଲ୍ୟ(rhs_type) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (lhs->lhs_lvar != ଖାଲି && (cac->cac_is_decl || !lhs->lhs_has_type))
    {
	ଯଦି (rhs_type->tt_type == VAR_FUNC
					|| rhs_type->tt_type == VAR_PARTIAL)
	{
	    // Make sure the variable name can be used as a funcref
	    ଯଦି (!lhs->lhs_has_index
				&& var_wrong_func_name(lhs->lhs_name, TRUE))
		ଫେରନ୍ତୁ FAIL;
	}

	ଯଦି (lhs->lhs_new_local && !lhs->lhs_has_type)
	{
	    // The LHS variable doesn't have a type.  Set it to the RHS type.
	    ଯଦି (compile_assign_set_lhs_type_from_rhs(cctx, cac, lhs, rhs_type)
								== FAIL)
		ଫେରନ୍ତୁ FAIL;
	}
	ଅନ୍ୟଥା ଯଦି (*cac->cac_op == '=')
	{
	    ଯଦି (!compile_assign_valid_rhs_type(cctx, cac, lhs, rhs_type))
		ଫେରନ୍ତୁ FAIL;
	}
    }
    ଅନ୍ୟଥା
    {
	// Assigning to a register using @r = "abc"

	type_T *lhs_type = lhs->lhs_member_type;

	// Special case: assigning to @# can use a number or a string.
	// Also: can assign a number to a float.
	ଯଦି ((lhs_type == &t_ସଂଖ୍ୟା_or_string || lhs_type == &t_float)
					&& rhs_type->tt_type == VAR_ସଂଖ୍ୟା)
	    lhs_type = &t_ସଂଖ୍ୟା;

	ଯଦି (*cac->cac_nextc != '=')
	{
	    ଯଦି (need_type(rhs_type, lhs_type, 0, -1, 0, cctx, FALSE,
							FALSE) == FAIL)
		ଫେରନ୍ତୁ FAIL;
	}
    }

    ଫେରନ୍ତୁ OK;
}

/*
 * Compile the RHS expression in an assignment statement and generate the
 * instructions.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_assign_rhs_ପଦସମଷ୍ଟି(cctx_T *cctx, cac_T *cac)
{
    cac->cac_is_const = FALSE;

    // for "+=", "*=", "..=" etc. first load the current value
    ଯଦି (*cac->cac_op != '='
	    && compile_load_lhs_with_index(&cac->cac_lhs, cac->cac_var_start,
								cctx) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    // For "var = expr" evaluate the expression.
    ଯଦି (cac->cac_var_ଗଣନା == 0)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	ret;

	// Compile the expression.
	cac->cac_instr_ଗଣନା = cac->cac_instr->ga_len;
	ret = compile_assign_single_eval_ପଦସମଷ୍ଟି(cctx, cac);
	ଯଦି (ret == FAIL)
	    ଫେରନ୍ତୁ FAIL;
    }
    ଅନ୍ୟଥା ଯଦି (cac->cac_semicolon && cac->cac_var_idx == cac->cac_var_ଗଣନା - 1)
    {
	// For "[var; var] = expr" get the rest of the list
	cac->cac_did_generate_slice = TRUE;
	ଯଦି (generate_SLICE(cctx, cac->cac_var_ଗଣନା - 1) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
    }
    ଅନ୍ୟଥା
    {
	// For "[var, var] = expr" get the "var_idx" item from the
	// list.
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା with_op = *cac->cac_op != '=';
	ଯଦି (generate_GETITEM(cctx, cac->cac_var_idx, with_op) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
    }

    ଯଦି (compile_assign_check_type(cctx, cac) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    ଫେରନ୍ତୁ OK;
}

/*
 * Compile the RHS expression in an assignment
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_assign_rhs(cctx_T *cctx, cac_T *cac)
{
    lhs_T	*lhs = &cac->cac_lhs;

    ଯଦି (cctx->ctx_skip == SKIP_YES)
    {
	ଯଦି (cac->cac_oplen > 0 && cac->cac_var_ଗଣନା == 0)
	{
	    // skip over the "=" and the expression
	    cac->cac_nextc = skipwhite(cac->cac_op + cac->cac_oplen);
	    (ଶୂନ୍ୟ)compile_expr0(&cac->cac_nextc, cctx);
	}
	ଫେରନ୍ତୁ OK;
    }

    // If RHS is specified, then generate instructions for RHS expression
    ଯଦି (cac->cac_oplen > 0)
	ଫେରନ୍ତୁ compile_assign_rhs_ପଦସମଷ୍ଟି(cctx, cac);

    ଯଦି (cac->cac_cmdidx == CMD_final)
    {
	emsg(_(e_final_requires_a_ମୂଲ୍ୟ));
	ଫେରନ୍ତୁ FAIL;
    }

    ଯଦି (cac->cac_cmdidx == CMD_const)
    {
	emsg(_(e_const_requires_a_ମୂଲ୍ୟ));
	ଫେରନ୍ତୁ FAIL;
    }

    ଯଦି (!lhs->lhs_has_type || lhs->lhs_dest == dest_option
					|| lhs->lhs_dest == dest_func_option)
    {
	emsg(_(e_type_or_initialization_required));
	ଫେରନ୍ତୁ FAIL;
    }

    // variables are always initialized
    ଯଦି (GA_GROW_FAILS(cac->cac_instr, 1))
	ଫେରନ୍ତୁ FAIL;

    cac->cac_instr_ଗଣନା = cac->cac_instr->ga_len;

    ଫେରନ୍ତୁ ଠେଲ_default_ମୂଲ୍ୟ(cctx, lhs->lhs_member_type->tt_type,
			      lhs->lhs_dest == dest_local,
			      &cac->cac_skip_store);
}

/*
 * Returns OK if "type" supports compound operator "op_arg" (e.g. +=, -=, %=,
 * etc.).  Compound operators are not supported with a tuple and a dict.
 * Returns FAIL if compound operator is not supported.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_type_supports_compound_op(type_T *type, char_u op_arg)
{
    ଯଦି (type->tt_type == VAR_TUPLE || type->tt_type == VAR_DICT)
    {
	char_u	op[2];

	op[0] = op_arg;
	op[1] = NUL;
	semsg(_(e_wrong_variable_type_for_str_equal), op);
	ଫେରନ୍ତୁ FAIL;
    }

    ଫେରନ୍ତୁ OK;
}

/*
 * Compile a compound op assignment statement (+=, -=, *=, %=, etc.)
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_assign_compound_op(cctx_T *cctx, cac_T *cac)
{
    lhs_T	    *lhs = &cac->cac_lhs;
    type_T	    *expected;
    type_T	    *stacktype = ଖାଲି;

    ଯଦି (cac->cac_lhs.lhs_type->tt_type == VAR_TUPLE
	    && check_type_supports_compound_op(cac->cac_lhs.lhs_type,
							*cac->cac_op) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (*cac->cac_op == '.')
    {
	expected = lhs->lhs_member_type;
	stacktype = get_type_on_ଗଦା(cctx, 0);
	ଯଦି (expected != &t_string
		&& need_type(stacktype, expected, 0, -1, 0, cctx,
					FALSE, FALSE) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
	ଅନ୍ୟଥା ଯଦି (may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
    }
    ଅନ୍ୟଥା
    {
	expected = lhs->lhs_member_type;
	stacktype = get_type_on_ଗଦା(cctx, 0);

	ଯଦି (check_type_supports_compound_op(expected, *cac->cac_op) == FAIL)
	    ଫେରନ୍ତୁ FAIL;

	ଯଦି (
		// If variable is float operation with number is OK.
		!(expected == &t_float && (stacktype == &t_ସଂଖ୍ୟା
					|| stacktype == &t_ସଂଖ୍ୟା_bool))
		&& need_type(stacktype, expected, TYPECHK_ସଂଖ୍ୟା_OK, -1, 0,
						cctx, FALSE, FALSE) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
    }

    ଯଦି (*cac->cac_op == '.')
    {
	ଯଦି (generate_CONCAT(cctx, 2) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
    }
    ଅନ୍ୟଥା ଯଦି (*cac->cac_op == '+')
    {
	ଯଦି (generate_add_instr(cctx,
		    operator_type(lhs->lhs_member_type, stacktype),
		    lhs->lhs_member_type, stacktype,
		    ପଦସମଷ୍ଟି_APPEND) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
    }
    ଅନ୍ୟଥା ଯଦି (generate_two_op(cctx, cac->cac_op) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    ଫେରନ୍ତୁ OK;
}

/*
 * Generate the STORE and SETTYPE instructions for an assignment statement.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_assign_generate_store(cctx_T *cctx, cac_T *cac)
{
    lhs_T	*lhs = &cac->cac_lhs;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_lnum;

    // Use the line number of the assignment for store instruction.
    save_lnum = cctx->ctx_lnum;
    cctx->ctx_lnum = cac->cac_start_lnum - 1;

    ଯଦି (lhs->lhs_has_index)
    {
	// Use the info in "lhs" to store the value at the index in the
	// list, dict or object.
	ଯଦି (compile_assign_unlet(cac->cac_var_start, &cac->cac_lhs,
				 TRUE, cac->cac_rhs_type, cctx) == FAIL)
	{
	    cctx->ctx_lnum = save_lnum;
	    ଫେରନ୍ତୁ FAIL;
	}
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (cac->cac_is_decl && cac->cac_cmdidx == CMD_const &&
			(lhs->lhs_dest == dest_script
			 || lhs->lhs_dest == dest_script_v9
			 || lhs->lhs_dest == dest_global
			 || lhs->lhs_dest == dest_local))
	    // ":const var": lock the value, but not referenced variables
	    generate_LOCKCONST(cctx);

	type_T	*inferred_type = cac->cac_inferred_type;

	ଯଦି ((lhs->lhs_type->tt_type == VAR_DICT
		    || lhs->lhs_type->tt_type == VAR_LIST)
		&& lhs->lhs_type->tt_member != ଖାଲି
		&& lhs->lhs_type->tt_member != &t_any
		&& lhs->lhs_type->tt_member != &t_unknown)
	    // Set the type in the list or dict, so that it can be
	    // checked, also in legacy script.
	    generate_SETTYPE(cctx, lhs->lhs_type);
	ଅନ୍ୟଥା ଯଦି (lhs->lhs_type->tt_type == VAR_TUPLE
					&& lhs->lhs_type->tt_argcount != 0)
	    generate_SETTYPE(cctx, lhs->lhs_type);
	ଅନ୍ୟଥା ଯଦି (inferred_type != ଖାଲି
		&& (inferred_type->tt_type == VAR_DICT
		    || inferred_type->tt_type == VAR_LIST)
		&& inferred_type->tt_member != ଖାଲି
		&& inferred_type->tt_member != &t_unknown
		&& inferred_type->tt_member != &t_any)
	    // Set the type in the list or dict, so that it can be
	    // checked, also in legacy script.
	    generate_SETTYPE(cctx, inferred_type);
	ଅନ୍ୟଥା ଯଦି (inferred_type != ଖାଲି
				&& inferred_type->tt_type == VAR_TUPLE
				&& inferred_type->tt_argcount > 0)
	    generate_SETTYPE(cctx, inferred_type);

	ଯଦି (!cac->cac_skip_store &&
		generate_store_lhs(cctx, &cac->cac_lhs,
				   cac->cac_instr_ଗଣନା,
				   cac->cac_is_decl) == FAIL)
	{
	    cctx->ctx_lnum = save_lnum;
	    ଫେରନ୍ତୁ FAIL;
	}
    }

    cctx->ctx_lnum = save_lnum;

    ଫେରନ୍ତୁ OK;
}

/*
 * Process the variable(s) in an assignment statement
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_assign_process_variables(
    cctx_T	*cctx,
    cac_T	*cac,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cmdidx,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		heredoc,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		has_cmd,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		has_argisset_prefix,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		jump_instr_idx)
{
    /*
     * Loop over variables in "[var, var] = expr".
     * For "name = expr" and "var name: type" this is done only once.
     */
    ପାଇଁ (cac->cac_var_idx = 0; cac->cac_var_idx == 0 ||
	    cac->cac_var_idx < cac->cac_var_ଗଣନା; cac->cac_var_idx++)
    {
	ଯଦି (cac->cac_var_start[0] == '_'
				&& !eval_isnamec(cac->cac_var_start[1]))
	{
	    // Ignore underscore in "[a, _, b] = list".
	    ଯଦି (cac->cac_var_ଗଣନା > 0)
	    {
		cac->cac_var_start = skipwhite(cac->cac_var_start + 2);
		ଜାରି;
	    }
	    emsg(_(e_cannot_use_underscore_here));
	    ଫେରନ୍ତୁ FAIL;
	}
	vim_free(cac->cac_lhs.lhs_name);

	/*
	 * Figure out the LHS type and other properties.
	 */
	ଯଦି (compile_assign_lhs(cac->cac_var_start, &cac->cac_lhs, cmdidx,
		    cac->cac_is_decl, heredoc, has_cmd,
		    cac->cac_oplen, cctx) == FAIL)
	    ଫେରନ୍ତୁ FAIL;

	// Compile the RHS expression
	ଯଦି (heredoc)
	{
	    SOURCING_LNUM = cac->cac_start_lnum;
	    ଯଦି (cac->cac_lhs.lhs_has_type
		    && need_type(&t_list_string, cac->cac_lhs.lhs_type,
				0, -1, 0, cctx, FALSE, FALSE) == FAIL)
		ଫେରନ୍ତୁ FAIL;
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (compile_assign_rhs(cctx, cac) == FAIL)
		ଫେରନ୍ତୁ FAIL;
	    ଯଦି (cac->cac_var_ଗଣନା == 0)
		cac->cac_var_end = cac->cac_nextc;
	}

	// no need to parse more when skipping
	ଯଦି (cctx->ctx_skip == SKIP_YES)
	    ଭାଙ୍ଗ;

	ଯଦି (cac->cac_oplen > 0 && *cac->cac_op != '=')
	{
	    ଯଦି (compile_assign_compound_op(cctx, cac) == FAIL)
		ଫେରନ୍ତୁ FAIL;
	}

	// generate the store instructions
	ଯଦି (compile_assign_generate_store(cctx, cac) == FAIL)
	    ଫେରନ୍ତୁ FAIL;

	ଯଦି (cac->cac_var_idx + 1 < cac->cac_var_ଗଣନା)
	    cac->cac_var_start = skipwhite(cac->cac_lhs.lhs_end + 1);

	ଯଦି (has_argisset_prefix)
	{
	    // set instruction index in JUMP_IF_ARG_SET to here
	    isn_T *isn = ((isn_T *)cac->cac_instr->ga_data) + jump_instr_idx;
	    isn->isn_arg.jumparg.jump_where = cac->cac_instr->ga_len;
	}
    }

    ଫେରନ୍ତୁ OK;
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
    ସ୍ଥିର char_u *
compile_assignment(
    char_u	*arg_start,
    exarg_T	*eap,
    cmdidx_T	cmdidx,
    cctx_T	*cctx)
{
    cac_T	cac;
    char_u	*arg = arg_start;
    char_u	*retstr = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		heredoc = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		jump_instr_idx;

    compile_assign_context_init(&cac, cctx, cmdidx, arg);

    jump_instr_idx = cac.cac_instr->ga_len;

    // process object variable initialization in a new() constructor method
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	has_argisset_prefix = STRNCMP(arg, "ifargisset ", 11) == 0;
    ଯଦି (has_argisset_prefix &&
			compile_assign_obj_new_arg(&arg, cctx) == FAIL)
	ଯାଅ theend;

    // Skip over the "varname" or "[varname, varname]" to get to any "=".
    cac.cac_nextc = skip_var_list(arg, TRUE, &cac.cac_var_ଗଣନା,
						&cac.cac_semicolon, TRUE);
    ଯଦି (cac.cac_nextc == ଖାଲି)
	ଫେରନ୍ତୁ *arg == '[' ? arg : ଖାଲି;

    ଯଦି (compile_assign_process_operator(eap, arg, &cac, &heredoc,
							&retstr) == FAIL)
	ଫେରନ୍ତୁ retstr;

    // Compute the start of the assignment
    cac.cac_var_start = compile_assign_compute_start(arg, cac.cac_var_ଗଣନା);

    // Compute the end of the assignment
    cac.cac_var_end = compile_assign_compute_end(eap, cctx, &cac, heredoc);
    ଯଦି (cac.cac_var_end == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା has_cmd = cac.cac_var_start > eap->cmd;

    /* process the variable(s) */
    ଯଦି (compile_assign_process_variables(cctx, &cac, cmdidx, heredoc,
					 has_cmd, has_argisset_prefix,
					 jump_instr_idx) == FAIL)
	ଯାଅ theend;

    // For "[var, var] = expr" drop the "expr" value.
    // Also for "[var, var; _] = expr".
    ଯଦି (cctx->ctx_skip != SKIP_YES && cac.cac_var_ଗଣନା > 0 &&
	    (!cac.cac_semicolon || !cac.cac_did_generate_slice))
    {
	ଯଦି (generate_instr_drop(cctx, ISN_DROP, 1) == ଖାଲି)
	    ଯାଅ theend;
    }

    retstr = skipwhite(cac.cac_var_end);

theend:
    vim_free(cac.cac_lhs.lhs_name);
    ଫେରନ୍ତୁ retstr;
}

/*
 * Check for an assignment at "eap->cmd", compile it if found.
 * Return NOTDONE if there is none, FAIL for failure, OK if done.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
may_compile_assignment(exarg_T *eap, char_u **ଧାଡ଼ି, cctx_T *cctx)
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
    ଯଦି (p > eap->cmd && *p != NUL)
    {
	char_u *var_end;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	oplen;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	heredoc;

	ଯଦି (eap->cmd[0] == '@')
	    var_end = eap->cmd + 2;
	ଅନ୍ୟଥା
	    var_end = find_name_end(pskip, ଖାଲି, ଖାଲି,
					FNE_CHECK_START | FNE_INCL_BR);
	oplen = assignment_len(skipwhite(var_end), &heredoc);
	ଯଦି (oplen > 0)
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
	    ଯଦି (*eap->cmd == '&'
		    || *eap->cmd == '$'
		    || *eap->cmd == '@'
		    || ((len) > 2 && eap->cmd[1] == ':')
		    || STRNCMP(eap->cmd, "g:[", 3) == 0
		    || variable_exists(eap->cmd, len, cctx))
	    {
		*ଧାଡ଼ି = compile_assignment(eap->cmd, eap, CMD_SIZE, cctx);
		ଯଦି (*ଧାଡ଼ି == ଖାଲି || *ଧାଡ଼ି == eap->cmd)
		    ଫେରନ୍ତୁ FAIL;
		ଫେରନ୍ତୁ OK;
	    }
	}
    }

    // might be "[var, var] = expr" or "ifargisset this.member = expr"
    ଯଦି (*eap->cmd == '[' || STRNCMP(eap->cmd, "ifargisset ", 11) == 0)
    {
	*ଧାଡ଼ି = compile_assignment(eap->cmd, eap, CMD_SIZE, cctx);
	ଯଦି (*ଧାଡ଼ି == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;
	ଯଦି (*ଧାଡ଼ି != eap->cmd)
	    ଫେରନ୍ତୁ OK;
    }
    ଫେରନ୍ତୁ NOTDONE;
}

/*
 * Check if arguments of "ufunc" shadow variables in "cctx".
 * Return OK or FAIL.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_args_shadowing(ufunc_T *ufunc, cctx_T *cctx)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    i;
    char_u  *arg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    r = OK;

    // Make sure arguments are not found when compiling a second time.
    ufunc->uf_args_visible = 0;

    // Check for arguments shadowing variables from the context.
    ପାଇଁ (i = 0; i < ufunc->uf_args.ga_len; ++i)
    {
	arg = ((char_u **)(ufunc->uf_args.ga_data))[i];
	ଯଦି (check_defined(arg, STRLEN(arg), cctx, ଖାଲି, TRUE) == FAIL)
	{
	    r = FAIL;
	    ଭାଙ୍ଗ;
	}
    }
    ufunc->uf_args_visible = ufunc->uf_args.ga_len;
    ଫେରନ୍ତୁ r;
}

#ifdef HAS_MESSAGE_WINDOW
/*
 * Get a count before a command.  Can only be a number.
 * Returns zero if there is no count.
 * Returns -1 if there is something wrong.
 */
    ସ୍ଥିର long
get_cmd_ଗଣନା(char_u *ଧାଡ଼ି, exarg_T *eap)
{
    char_u *p;

    // skip over colons and white space
    ପାଇଁ (p = ଧାଡ଼ି; *p == ':' || VIM_ISWHITE(*p); ++p)
	;
    ଯଦି (!SAFE_isdigit(*p))
    {
	// The command or modifiers must be following.  Assume a lower case
	// character means there is a modifier.
	ଯଦି (p < eap->cmd && !vim_islower(*p))
	{
	    emsg(_(e_invalid_range));
	    ଫେରନ୍ତୁ -1;
	}
	ଫେରନ୍ତୁ 0;
    }
    ଫେରନ୍ତୁ atol((ଅକ୍ଷର *)p);
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

    ଯଦି (debug_break_level > 0 || may_break_in_function(ufunc))
	ଫେରନ୍ତୁ CT_DEBUG;
#ifdef FEAT_PROFILE
    ଯଦି (do_profiling == PROF_YES)
    {
	ଯଦି (!ufunc->uf_profiling && has_profiling(FALSE, ufunc->uf_name, ଖାଲି,
							    &ufunc->uf_hash))
	    func_do_profile(ufunc);
	ଯଦି (ufunc->uf_profiling)
	    ଫେରନ୍ତୁ CT_PROFILE;
    }
#endif
    ଫେରନ୍ତୁ CT_NONE;
}

/*
 * Free the compiled instructions saved for a def function.  This is used when
 * compiling a def function and the function was compiled before.
 * The index is reused.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
clear_def_function(ufunc_T *ufunc, compiletype_T compile_type)
{
    isn_T	*instr_dest = ଖାଲି;
    dfunc_T	*dfunc;

    dfunc = ((dfunc_T *)def_functions.ga_data) + ufunc->uf_dfunc_idx;

    ବାଛ (compile_type)
    {
	ସ୍ଥିତି CT_PROFILE:
#ifdef FEAT_PROFILE
	    instr_dest = dfunc->df_instr_prof; ଭାଙ୍ଗ;
#endif
	ସ୍ଥିତି CT_NONE:   instr_dest = dfunc->df_instr; ଭାଙ୍ଗ;
	ସ୍ଥିତି CT_DEBUG:  instr_dest = dfunc->df_instr_debug; ଭାଙ୍ଗ;
    }

    ଯଦି (instr_dest != ଖାଲି)
	// Was compiled in this mode before: Free old instructions.
	delete_def_function_contents(dfunc, FALSE);

    ga_clear_strings(&dfunc->df_var_names);
    dfunc->df_defer_var_idx = 0;
}

/*
 * Add a function to the list of :def functions.
 * This sets "ufunc->uf_dfunc_idx" but the function isn't compiled yet.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
add_def_function(ufunc_T *ufunc)
{
    dfunc_T *dfunc;

    ଯଦି (def_functions.ga_len == 0)
    {
	// The first position is not used, so that a zero uf_dfunc_idx means it
	// wasn't set.
	ଯଦି (GA_GROW_FAILS(&def_functions, 1))
	    ଫେରନ୍ତୁ FAIL;
	++def_functions.ga_len;
    }

    // Add the function to "def_functions".
    ଯଦି (GA_GROW_FAILS(&def_functions, 1))
	ଫେରନ୍ତୁ FAIL;
    dfunc = ((dfunc_T *)def_functions.ga_data) + def_functions.ga_len;
    CLEAR_POINTER(dfunc);
    dfunc->df_idx = def_functions.ga_len;
    ufunc->uf_dfunc_idx = dfunc->df_idx;
    dfunc->df_ufunc = ufunc;
    dfunc->df_name = vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
    ga_init2(&dfunc->df_var_names, ଆକାର(char_u *), 10);
    ++dfunc->df_refcount;
    ++def_functions.ga_len;
    ଫେରନ୍ତୁ OK;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_dfunc_ufunc_init(
    ufunc_T		*ufunc,
    cctx_T		*outer_cctx,
    compiletype_T	compile_type,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			*new_def_function)
{
    // When using a function that was compiled before: Free old instructions.
    // The index is reused.  Otherwise add a new entry in "def_functions".
    ଯଦି (ufunc->uf_dfunc_idx > 0)
	clear_def_function(ufunc, compile_type);
    ଅନ୍ୟଥା
    {
	ଯଦି (add_def_function(ufunc) == FAIL)
	    ଫେରନ୍ତୁ FAIL;

	*new_def_function = TRUE;
    }

    ଯଦି ((ufunc->uf_flags & FC_CLOSURE) && outer_cctx == ଖାଲି)
    {
	semsg(_(e_compiling_closure_without_context_str),
						printable_func_name(ufunc));
	ଫେରନ୍ତୁ FAIL;
    }

    ufunc->uf_def_status = UF_COMPILING;

    ଫେରନ୍ତୁ OK;
}

/*
 * Initialize the compilation context for compiling a def function.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
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
    ga_init2(&cctx->ctx_locals, ଆକାର(lvar_T), 10);
    // Each entry on the type stack consists of two type pointers.
    ga_init2(&cctx->ctx_type_ଗଦା, ଆକାର(type2_T), 50);
    cctx->ctx_type_list = &ufunc->uf_type_list;
    ga_init2(&cctx->ctx_instr, ଆକାର(isn_T), 50);
}

/*
 * For an object constructor, generate instruction to setup "this" (the first
 * local variable) and to initialize the object variables.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
obj_constructor_prologue(ufunc_T *ufunc, cctx_T *cctx)
{
    generate_CONSTRUCT(cctx, ufunc->uf_class);

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < ufunc->uf_class->class_obj_member_ଗଣନା; ++i)
    {
	ocmember_T *m = &ufunc->uf_class->class_obj_members[i];

	ଯଦି (i < 2 && IS_ENUM(ufunc->uf_class))
	    // The first two object variables in an enum are the name
	    // and the ordinal.  These are set by the ISN_CONSTRUCT
	    // instruction.  So don't generate instructions to set
	    // these variables.
	    ଜାରି;

	ଯଦି (m->ocm_init != ଖାଲି)
	{
	    char_u	*ପଦସମଷ୍ଟି = m->ocm_init;
	    sctx_T	save_current_sctx;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		change_sctx = FALSE;

	    // If the member variable initialization script context is
	    // different from the current script context, then change it.
	    ଯଦି (current_sctx.sc_sid != m->ocm_init_sctx.sc_sid)
		change_sctx = TRUE;

	    ଯଦି (change_sctx)
	    {
		// generate an instruction to change the script context to the
		// member variable initialization script context.
		save_current_sctx = current_sctx;
		current_sctx = m->ocm_init_sctx;
		generate_SCRIPTCTX_SET(cctx, current_sctx);
	    }

	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା r = compile_expr0(&ପଦସମଷ୍ଟି, cctx);

	    ଯଦି (change_sctx)
	    {
		// restore the previous script context
		current_sctx = save_current_sctx;
		generate_SCRIPTCTX_SET(cctx, current_sctx);
	    }

	    ଯଦି (r == FAIL)
		ଫେରନ୍ତୁ FAIL;

	    ଯଦି (!ends_excmd2(m->ocm_init, ପଦସମଷ୍ଟି))
	    {
		semsg(_(e_trailing_characters_str), ପଦସମଷ୍ଟି);
		ଫେରନ୍ତୁ FAIL;
	    }

	    type_T	*type = get_type_on_ଗଦା(cctx, 0);
	    ଯଦି (m->ocm_type->tt_type == VAR_ANY
		    && !(m->ocm_flags & OCMFLAG_HAS_TYPE)
		    && type->tt_type != VAR_SPECIAL)
	    {
		// If the member variable type is not yet set, then use
		// the initialization expression type.
		m->ocm_type = type;
	    }
	    ଅନ୍ୟଥା
	    {
		// The type of the member initialization expression is
		// determined at run time.  Add a runtime type check.
		where_T	where = WHERE_INIT;
		where.wt_kind = WT_MEMBER;
		where.wt_func_name = (ଅକ୍ଷର *)m->ocm_name.string;
		ଯଦି (need_type_where(type, m->ocm_type, 0, -1,
					where, cctx, FALSE, FALSE) == FAIL)
		    ଫେରନ୍ତୁ FAIL;
	    }
	}
	ଅନ୍ୟଥା
	    ଠେଲ_default_ମୂଲ୍ୟ(cctx, m->ocm_type->tt_type, FALSE, ଖାଲି);

	ଯଦି (((m->ocm_type->tt_type == VAR_DICT
			|| m->ocm_type->tt_type == VAR_LIST)
		    && m->ocm_type->tt_member != ଖାଲି
		    && m->ocm_type->tt_member != &t_any
		    && m->ocm_type->tt_member != &t_unknown)
		|| (m->ocm_type->tt_type == VAR_TUPLE
		    && m->ocm_type->tt_argcount > 0))
	    // Set the type in the list, tuple or dict, so that it can be
	    // checked, also in legacy script.
	    generate_SETTYPE(cctx, m->ocm_type);

	generate_STORE_THIS(cctx, i);
    }

    ଫେରନ୍ତୁ OK;
}

/*
 * For an object method and an constructor, generate instruction to setup
 * "this" (the first local variable).  For a constructor, generate instructions
 * to initialize the object variables.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
obj_method_prologue(ufunc_T *ufunc, cctx_T *cctx)
{
    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data) + ufunc->uf_dfunc_idx;

    ଯଦି (GA_GROW_FAILS(&dfunc->df_var_names, 1))
	ଫେରନ୍ତୁ FAIL;

    ((char_u **)dfunc->df_var_names.ga_data)[0] =
						vim_strsave((char_u *)"this");
    ++dfunc->df_var_names.ga_len;

    // In the constructor allocate memory for the object and initialize the
    // object members.
    ଯଦି (IS_CONSTRUCTOR_METHOD(ufunc))
	ଫେରନ୍ତୁ obj_constructor_prologue(ufunc, cctx);

    ଫେରନ୍ତୁ OK;
}

/*
 * Produce instructions for the default values of optional arguments.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_def_function_default_args(
    ufunc_T	*ufunc,
    garray_T	*instr,
    cctx_T	*cctx)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	ଗଣନା_2 = ufunc->uf_def_args.ga_len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	first_def_arg = ufunc->uf_args.ga_len - ଗଣନା_2;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	off = ଗଦା_FRAME_SIZE + (ufunc->uf_va_name != ଖାଲି ? 1 : 0);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	did_set_arg_type = FALSE;

    // Produce instructions for the default values of optional arguments.
    SOURCING_LNUM = 0;  // line number unknown
    ପାଇଁ (i = 0; i < ଗଣନା_2; ++i)
    {
	char_u *arg = ((char_u **)(ufunc->uf_def_args.ga_data))[i];
	ଯଦି (STRCMP(arg, "v:none") == 0)
	    // "arg = v:none" means the argument is optional without
	    // setting a value when the argument is missing.
	    ଜାରି;

	type_T	*val_type;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		arg_idx = first_def_arg + i;
	where_T	where = WHERE_INIT;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		jump_instr_idx = instr->ga_len;
	isn_T	*isn;

	// Use a JUMP_IF_ARG_SET instruction to skip if the value was given.
	ଯଦି (generate_JUMP_IF_ARG(cctx, ISN_JUMP_IF_ARG_SET,
						i - ଗଣନା_2 - off) == FAIL)
	    ଫେରନ୍ତୁ FAIL;

	// Make sure later arguments are not found.
	ufunc->uf_args_visible = arg_idx;

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା r = compile_expr0(&arg, cctx);
	ଯଦି (r == FAIL)
	    ଫେରନ୍ତୁ FAIL;

	// If no type specified use the type of the default value.
	// Otherwise check that the default value type matches the
	// specified type.
	val_type = get_type_on_ଗଦା(cctx, 0);
	where.wt_index = arg_idx + 1;
	where.wt_kind = WT_ARGUMENT;
	ଯଦି (ufunc->uf_arg_types[arg_idx] == &t_unknown)
	{
	    did_set_arg_type = TRUE;
	    ufunc->uf_arg_types[arg_idx] = val_type;
	}
	ଅନ୍ୟଥା ଯଦି (need_type_where(val_type, ufunc->uf_arg_types[arg_idx],
		    0, -1, where, cctx, FALSE, FALSE) == FAIL)
	    ଫେରନ୍ତୁ FAIL;

	ଯଦି (generate_STORE(cctx, ISN_STORE, i - ଗଣନା_2 - off, ଖାଲି) == FAIL)
	    ଫେରନ୍ତୁ FAIL;

	// set instruction index in JUMP_IF_ARG_SET to here
	isn = ((isn_T *)instr->ga_data) + jump_instr_idx;
	isn->isn_arg.jumparg.jump_where = instr->ga_len;
    }

    ଯଦି (did_set_arg_type)
	set_function_type(ufunc);

    ଫେରନ୍ତୁ OK;
}

/*
 * Compile def function body.  Loop over all the lines in the function and
 * generate instructions.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_def_function_body(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		last_func_lnum,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		check_return_type,
    garray_T	*ଧାଡ଼ିଗୁଡ଼ିକ_to_free,
    ଅକ୍ଷର	**errormsg,
    cctx_T	*cctx)
{
    char_u	*ଧାଡ଼ି = ଖାଲି;
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_emsg_before = did_emsg;
#ifdef FEAT_PROFILE
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prof_lnum = -1;
#endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		debug_lnum = -1;

    ପାଇଁ (;;)
    {
	exarg_T	    ea;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    starts_with_colon = FALSE;
	char_u	    *cmd;
	cmdmod_T    local_cmdmod;

	// Bail out on the first error to avoid a flood of errors and report
	// the right line number when inside try/catch.
	ଯଦି (did_emsg_before != did_emsg)
	    ଫେରନ୍ତୁ FAIL;

	ଯଦି (ଧାଡ଼ି != ଖାଲି && *ଧାଡ଼ି == '|')
	    // the line continues after a '|'
	    ++ଧାଡ଼ି;
	ଅନ୍ୟଥା ଯଦି (ଧାଡ଼ି != ଖାଲି && *skipwhite(ଧାଡ଼ି) != NUL
		&& !(*ଧାଡ଼ି == '#' && (ଧାଡ଼ି == cctx->ctx_ଧାଡ଼ି_start
						    || VIM_ISWHITE(ଧାଡ଼ି[-1]))))
	{
	    semsg(_(e_trailing_characters_str), ଧାଡ଼ି);
	    ଫେରନ୍ତୁ FAIL;
	}
	ଅନ୍ୟଥା ଯଦି (ଧାଡ଼ି != ଖାଲି && vim9_bad_comment(skipwhite(ଧାଡ଼ି)))
	    ଫେରନ୍ତୁ FAIL;
	ଅନ୍ୟଥା
	{
	    ଧାଡ଼ି = next_ଧାଡ଼ି_from_context(cctx, FALSE);
	    ଯଦି (cctx->ctx_lnum >= last_func_lnum)
	    {
		// beyond the last line
#ifdef FEAT_PROFILE
		ଯଦି (cctx->ctx_skip != SKIP_YES)
		    may_generate_prof_end(cctx, prof_lnum);
#endif
		ଭାଙ୍ଗ;
	    }
	    // Make a copy, splitting off nextcmd and removing trailing spaces
	    // may change it.
	    ଯଦି (ଧାଡ଼ି != ଖାଲି)
	    {
		ଧାଡ଼ି = vim_strsave(ଧାଡ଼ି);
		ଯଦି (ga_add_string(ଧାଡ଼ିଗୁଡ଼ିକ_to_free, ଧାଡ଼ି) == FAIL)
		{
		    vim_free(ଧାଡ଼ି);
		    ଫେରନ୍ତୁ FAIL;
		}
	    }
	}

	CLEAR_FIELD(ea);
	ea.cmdlinep = &ଧାଡ଼ି;
	ea.cmd = skipwhite(ଧାଡ଼ି);
	ea.skip = cctx->ctx_skip == SKIP_YES;

	ଯଦି (*ea.cmd == '#')
	{
	    // "#" starts a comment, but "#{" is an error
	    ଯଦି (vim9_bad_comment(ea.cmd))
		ଫେରନ୍ତୁ FAIL;
	    ଧାଡ଼ି = (char_u *)"";
	    ଜାରି;
	}

#ifdef FEAT_PROFILE
	ଯଦି (cctx->ctx_compile_type == CT_PROFILE && cctx->ctx_lnum != prof_lnum
						  && cctx->ctx_skip != SKIP_YES)
	{
	    may_generate_prof_end(cctx, prof_lnum);

	    prof_lnum = cctx->ctx_lnum;
	    generate_instr(cctx, ISN_PROF_START);
	}
#endif
	ଯଦି (cctx->ctx_compile_type == CT_DEBUG && cctx->ctx_lnum != debug_lnum
						  && cctx->ctx_skip != SKIP_YES)
	{
	    debug_lnum = cctx->ctx_lnum;
	    generate_instr_debug(cctx);
	}
	cctx->ctx_prev_lnum = cctx->ctx_lnum + 1;

	// Some things can be recognized by the first character.
	ବାଛ (*ea.cmd)
	{
	    ସ୍ଥିତି '}':
		{
		    // "}" ends a block scope
		    scopetype_T stype = cctx->ctx_scope == ଖାଲି
					  ? NO_SCOPE : cctx->ctx_scope->se_type;

		    ଯଦି (stype == BLOCK_SCOPE)
		    {
			compile_endblock(cctx);
			ଧାଡ଼ି = ea.cmd;
		    }
		    ଅନ୍ୟଥା
		    {
			emsg(_(e_using_rcurly_outside_if_block_scope));
			ଫେରନ୍ତୁ FAIL;
		    }
		    ଯଦି (ଧାଡ଼ି != ଖାଲି)
			ଧାଡ଼ି = skipwhite(ea.cmd + 1);
		    ଜାରି;
		}

	    ସ୍ଥିତି '{':
		// "{" starts a block scope
		// "{'a': 1}->func() is something else
		ଯଦି (ends_excmd(*skipwhite(ea.cmd + 1)))
		{
		    ଧାଡ଼ି = compile_block(ea.cmd, cctx);
		    ଜାରି;
		}
		ଭାଙ୍ଗ;
	}

	/*
	 * COMMAND MODIFIERS
	 */
	cctx->ctx_has_cmdmod = FALSE;
	ଯଦି (parse_command_modifiers(&ea, errormsg, &local_cmdmod, FALSE)
								       == FAIL)
	    ଫେରନ୍ତୁ FAIL;
	generate_cmdmods(cctx, &local_cmdmod);
	undo_cmdmod(&local_cmdmod);

	// Check if there was a colon after the last command modifier or before
	// the current position.
	ପାଇଁ (p = ea.cmd; p >= ଧାଡ଼ି; --p)
	{
	    ଯଦି (*p == ':')
		starts_with_colon = TRUE;
	    ଯଦି (p < ea.cmd && !VIM_ISWHITE(*p))
		ଭାଙ୍ଗ;
	}

	// Skip ":call" to get to the function name, unless using :legacy
	p = ea.cmd;
	ଯଦି (!(local_cmdmod.cmod_flags & CMOD_LEGACY))
	{
	    ଯଦି (checkforcmd(&ea.cmd, "call", 3))
	    {
		ଯଦି (*ea.cmd == '(')
		    // not for "call()"
		    ea.cmd = p;
		ଅନ୍ୟଥା
		    ea.cmd = skipwhite(ea.cmd);
	    }

	    ଯଦି (!starts_with_colon)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    assign;

		// Check for assignment after command modifiers.
		assign = may_compile_assignment(&ea, &ଧାଡ଼ି, cctx);
		ଯଦି (assign == OK)
		    ଯାଅ nextline;
		ଯଦି (assign == FAIL)
		    ଫେରନ୍ତୁ FAIL;
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
	ଯଦି ((*cmd != '$' || starts_with_colon)
		&& (starts_with_colon
		    || !(*cmd == '\''
			|| (cmd[0] == '0' && cmd[1] == 'z')
			|| (cmd[0] != NUL && cmd[0] == cmd[1]
					    && (*cmd == '+' || *cmd == '-'))
			|| ସଂଖ୍ୟା_method(cmd))))
	{
	    ea.cmd = skip_range(ea.cmd, TRUE, ଖାଲି);
	    ଯଦି (ea.cmd > cmd)
	    {
		ଯଦି (!starts_with_colon
				   && !(local_cmdmod.cmod_flags & CMOD_LEGACY))
		{
		    semsg(_(e_colon_required_before_range_str), cmd);
		    ଫେରନ୍ତୁ FAIL;
		}
		ea.addr_ଗଣନା = 1;
		ଯଦି (ends_excmd2(ଧାଡ଼ି, ea.cmd))
		{
		    // A range without a command: jump to the line.
		    generate_EXEC(cctx, ISN_EXECRANGE,
					      vim_strnsave(cmd, ea.cmd - cmd));
		    ଧାଡ଼ି = ea.cmd;
		    ଯାଅ nextline;
		}
	    }
	}
	p = find_ex_command(&ea, ଖାଲି,
		starts_with_colon || (local_cmdmod.cmod_flags & CMOD_LEGACY)
						  ? ଖାଲି : item_exists, cctx);

	ଯଦି (p == ଖାଲି)
	{
	    ଯଦି (cctx->ctx_skip != SKIP_YES)
		semsg(_(e_ambiguous_use_of_user_defined_command_str), ea.cmd);
	    ଫେରନ୍ତୁ FAIL;
	}

	// When using ":legacy cmd" always use compile_exec().
	ଯଦି (local_cmdmod.cmod_flags & CMOD_LEGACY)
	{
	    char_u *start = ea.cmd;

	    ବାଛ (ea.cmdidx)
	    {
		ସ୍ଥିତି CMD_if:
		ସ୍ଥିତି CMD_elseif:
		ସ୍ଥିତି CMD_else:
		ସ୍ଥିତି CMD_endif:
		ସ୍ଥିତି CMD_for:
		ସ୍ଥିତି CMD_endfor:
		ସ୍ଥିତି CMD_continue:
		ସ୍ଥିତି CMD_break:
		ସ୍ଥିତି CMD_while:
		ସ୍ଥିତି CMD_endwhile:
		ସ୍ଥିତି CMD_try:
		ସ୍ଥିତି CMD_catch:
		ସ୍ଥିତି CMD_finally:
		ସ୍ଥିତି CMD_endtry:
			semsg(_(e_cannot_use_legacy_with_command_str), ea.cmd);
			ଫେରନ୍ତୁ FAIL;
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ: ଭାଙ୍ଗ;
	    }

	    // ":legacy return expr" needs to be handled differently.
	    ଯଦି (checkforcmd(&start, "return", 4))
		ea.cmdidx = CMD_return;
	    ଅନ୍ୟଥା
		ea.cmdidx = CMD_legacy;
	}

	ଯଦି (p == ea.cmd && ea.cmdidx != CMD_SIZE)
	{
	    // "eval" is used for "val->func()" and "var" for "var = val", then
	    // "p" is equal to "ea.cmd" for a valid command.
	    ଯଦି (ea.cmdidx == CMD_eval || ea.cmdidx == CMD_var)
		;
	    ଅନ୍ୟଥା ଯଦି (cctx->ctx_skip == SKIP_YES)
	    {
		ଧାଡ଼ି += STRLEN(ଧାଡ଼ି);
		ଯାଅ nextline;
	    }
	    ଅନ୍ୟଥା
	    {
		semsg(_(e_command_not_recognized_str), ea.cmd);
		ଫେରନ୍ତୁ FAIL;
	    }
	}

	ଯଦି ((cctx->ctx_had_return || cctx->ctx_had_throw)
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
	    ଫେରନ୍ତୁ FAIL;
	}

	// When processing the end of an if-else or try-catch block, don't
	// clear the "ctx_had_throw" flag.  If such a block ends in a "throw"
	// statement, then it is considered to end in a "return" statement.
	// The "ctx_had_throw" is cleared immediately after processing the
	// block ending statement.
	// Otherwise, clear the "had_throw" flag.
	ଯଦି (ea.cmdidx != CMD_else && ea.cmdidx != CMD_elseif
						&& ea.cmdidx != CMD_endif
						&& ea.cmdidx != CMD_catch
						&& ea.cmdidx != CMD_finally
						&& ea.cmdidx != CMD_endtry)
	    cctx->ctx_had_throw = FALSE;

	p = skipwhite(p);
	ଯଦି (ea.cmdidx != CMD_SIZE
			    && ea.cmdidx != CMD_write && ea.cmdidx != CMD_read)
	{
	    ଯଦି (ea.cmdidx >= 0)
		ea.argt = excmd_get_argt(ea.cmdidx);
	    ଯଦି ((ea.argt & EX_BANG) && *p == '!')
	    {
		ea.forceit = TRUE;
		p = skipwhite(p + 1);
	    }
	    ଯଦି ((ea.argt & EX_RANGE) == 0 && ea.addr_ଗଣନା > 0)
	    {
		emsg(_(e_no_range_allowed));
		ଫେରନ୍ତୁ FAIL;
	    }
	}

	ବାଛ (ea.cmdidx)
	{
	    ସ୍ଥିତି CMD_def:
	    ସ୍ଥିତି CMD_function:
		    ea.arg = p;
		    ଧାଡ଼ି = compile_nested_function(&ea, cctx, ଧାଡ଼ିଗୁଡ଼ିକ_to_free);
		    ଭାଙ୍ଗ;

	    ସ୍ଥିତି CMD_return:
		    ଧାଡ଼ି = compile_return(p, check_return_type,
				 local_cmdmod.cmod_flags & CMOD_LEGACY, cctx);
		    cctx->ctx_had_return = TRUE;
		    ଭାଙ୍ଗ;

	    ସ୍ଥିତି CMD_let:
		    emsg(_(e_cannot_use_let_in_vim9_script));
		    ଭାଙ୍ଗ;
	    ସ୍ଥିତି CMD_var:
	    ସ୍ଥିତି CMD_final:
	    ସ୍ଥିତି CMD_const:
	    ସ୍ଥିତି CMD_increment:
	    ସ୍ଥିତି CMD_decrement:
		    ଧାଡ଼ି = compile_assignment(p, &ea, ea.cmdidx, cctx);
		    ଯଦି (ଧାଡ଼ି == p)
		    {
			emsg(_(e_invalid_assignment));
			ଧାଡ଼ି = ଖାଲି;
		    }
		    ଭାଙ୍ଗ;

	    ସ୍ଥିତି CMD_unlet:
	    ସ୍ଥିତି CMD_unlockvar:
	    ସ୍ଥିତି CMD_lockvar:
		    ଧାଡ଼ି = compile_unletlock(p, &ea, cctx);
		    ଭାଙ୍ଗ;

	    ସ୍ଥିତି CMD_import:
		    emsg(_(e_import_can_only_be_used_in_script));
		    ଧାଡ଼ି = ଖାଲି;
		    ଭାଙ୍ଗ;

	    ସ୍ଥିତି CMD_if:
		    ଧାଡ଼ି = compile_if(p, cctx);
		    ଭାଙ୍ଗ;
	    ସ୍ଥିତି CMD_elseif:
		    ଧାଡ଼ି = compile_elseif(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    cctx->ctx_had_throw = FALSE;
		    ଭାଙ୍ଗ;
	    ସ୍ଥିତି CMD_else:
		    ଧାଡ଼ି = compile_else(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    cctx->ctx_had_throw = FALSE;
		    ଭାଙ୍ଗ;
	    ସ୍ଥିତି CMD_endif:
		    ଧାଡ଼ି = compile_endif(p, cctx);
		    cctx->ctx_had_throw = FALSE;
		    ଭାଙ୍ଗ;

	    ସ୍ଥିତି CMD_while:
		    ଧାଡ଼ି = compile_while(p, cctx);
		    ଭାଙ୍ଗ;
	    ସ୍ଥିତି CMD_endwhile:
		    ଧାଡ଼ି = compile_endwhile(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    ଭାଙ୍ଗ;

	    ସ୍ଥିତି CMD_for:
		    ଧାଡ଼ି = compile_for(p, cctx);
		    ଭାଙ୍ଗ;
	    ସ୍ଥିତି CMD_endfor:
		    ଧାଡ଼ି = compile_endfor(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    ଭାଙ୍ଗ;
	    ସ୍ଥିତି CMD_continue:
		    ଧାଡ଼ି = compile_continue(p, cctx);
		    // "continue" unconditionally jumps back to the loop,
		    // like a "return".
		    cctx->ctx_had_return = TRUE;
		    ଭାଙ୍ଗ;
	    ସ୍ଥିତି CMD_break:
		    ଧାଡ଼ି = compile_break(p, cctx);
		    // "break" unconditionally exits the loop, like a "return".
		    cctx->ctx_had_return = TRUE;
		    ଭାଙ୍ଗ;

	    ସ୍ଥିତି CMD_try:
		    ଧାଡ଼ି = compile_try(p, cctx);
		    ଭାଙ୍ଗ;
	    ସ୍ଥିତି CMD_catch:
		    ଧାଡ଼ି = compile_catch(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    cctx->ctx_had_throw = FALSE;
		    ଭାଙ୍ଗ;
	    ସ୍ଥିତି CMD_finally:
		    ଧାଡ଼ି = compile_finally(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    cctx->ctx_had_throw = FALSE;
		    ଭାଙ୍ଗ;
	    ସ୍ଥିତି CMD_endtry:
		    ଧାଡ଼ି = compile_endtry(p, cctx);
		    cctx->ctx_had_throw = FALSE;
		    ଭାଙ୍ଗ;
	    ସ୍ଥିତି CMD_throw:
		    ଧାଡ଼ି = compile_throw(p, cctx);
		    cctx->ctx_had_throw = TRUE;
		    ଭାଙ୍ଗ;

	    ସ୍ଥିତି CMD_eval:
		    ଧାଡ଼ି = compile_eval(p, cctx);
		    ଭାଙ୍ଗ;

	    ସ୍ଥିତି CMD_defer:
		    ଧାଡ଼ି = compile_defer(p, cctx);
		    ଭାଙ୍ଗ;

#ifdef HAS_MESSAGE_WINDOW
	    ସ୍ଥିତି CMD_echowindow:
		    {
			long cmd_ଗଣନା = get_cmd_ଗଣନା(ଧାଡ଼ି, &ea);
			ଯଦି (cmd_ଗଣନା < 0)
			    ଧାଡ଼ି = ଖାଲି;
			ଅନ୍ୟଥା
			    ଧାଡ଼ି = compile_mult_ପଦସମଷ୍ଟି(p, ea.cmdidx,
							     cmd_ଗଣନା, cctx);
		    }
		    ଭାଙ୍ଗ;
#endif
	    ସ୍ଥିତି CMD_echo:
	    ସ୍ଥିତି CMD_echon:
	    ସ୍ଥିତି CMD_echoconsole:
	    ସ୍ଥିତି CMD_echoerr:
	    ସ୍ଥିତି CMD_echomsg:
	    ସ୍ଥିତି CMD_execute:
		    ଧାଡ଼ି = compile_mult_ପଦସମଷ୍ଟି(p, ea.cmdidx, 0, cctx);
		    ଭାଙ୍ଗ;

	    ସ୍ଥିତି CMD_put:
		    ea.cmd = cmd;
		    ଧାଡ଼ି = compile_put(p, &ea, cctx, FALSE);
		    ଭାଙ୍ଗ;

	    ସ୍ଥିତି CMD_iput:
		    ea.cmd = cmd;
		    ଧାଡ଼ି = compile_put(p, &ea, cctx, TRUE);
		    ଭାଙ୍ଗ;

	    ସ୍ଥିତି CMD_substitute:
		    ଯଦି (check_global_and_subst(ea.cmd, p) == FAIL)
			ଫେରନ୍ତୁ FAIL;
		    ଯଦି (cctx->ctx_skip == SKIP_YES)
			ଧାଡ଼ି = (char_u *)"";
		    ଅନ୍ୟଥା
		    {
			ea.arg = p;
			ଧାଡ଼ି = compile_substitute(ଧାଡ଼ି, &ea, cctx);
		    }
		    ଭାଙ୍ଗ;

	    ସ୍ଥିତି CMD_redir:
		    ea.arg = p;
		    ଧାଡ଼ି = compile_redir(ଧାଡ଼ି, &ea, cctx);
		    ଭାଙ୍ଗ;

	    ସ୍ଥିତି CMD_cexpr:
	    ସ୍ଥିତି CMD_lexpr:
	    ସ୍ଥିତି CMD_caddexpr:
	    ସ୍ଥିତି CMD_laddexpr:
	    ସ୍ଥିତି CMD_cgetexpr:
	    ସ୍ଥିତି CMD_lgetexpr:
#ifdef FEAT_QUICKFIX
		    ea.arg = p;
		    ଧାଡ଼ି = compile_cexpr(ଧାଡ଼ି, &ea, cctx);
#ଅନ୍ୟଥା
		    ex_ni(&ea);
		    ଧାଡ଼ି = ଖାଲି;
#endif
		    ଭାଙ୍ଗ;

	    ସ୍ଥିତି CMD_append:
	    ସ୍ଥିତି CMD_change:
	    ସ୍ଥିତି CMD_ଭର୍ତ୍ତି:
	    ସ୍ଥିତି CMD_k:
	    ସ୍ଥିତି CMD_open:
	    ସ୍ଥିତି CMD_t:
	    ସ୍ଥିତି CMD_xit:
		    not_in_vim9(&ea, false);
		    ଫେରନ୍ତୁ FAIL;

	    ସ୍ଥିତି CMD_SIZE:
		    ଯଦି (cctx->ctx_skip != SKIP_YES)
		    {
			semsg(_(e_invalid_command_str), ea.cmd);
			ଫେରନ୍ତୁ FAIL;
		    }
		    // We don't check for a next command here.
		    ଧାଡ଼ି = (char_u *)"";
		    ଭାଙ୍ଗ;

	    ସ୍ଥିତି CMD_lua:
	    ସ୍ଥିତି CMD_mzscheme:
	    ସ୍ଥିତି CMD_perl:
	    ସ୍ଥିତି CMD_py3:
	    ସ୍ଥିତି CMD_python3:
	    ସ୍ଥିତି CMD_python:
	    ସ୍ଥିତି CMD_pythonx:
	    ସ୍ଥିତି CMD_ruby:
	    ସ୍ଥିତି CMD_tcl:
		    ea.arg = p;
		    ଯଦି (vim_strchr(ଧାଡ଼ି, '\n') == ଖାଲି)
			ଧାଡ଼ି = compile_exec(ଧାଡ଼ି, &ea, cctx);
		    ଅନ୍ୟଥା
			// heredoc lines have been concatenated with NL
			// characters in get_function_body()
			ଧାଡ଼ି = compile_script(ଧାଡ଼ି, cctx);
		    ଭାଙ୍ଗ;

	    ସ୍ଥିତି CMD_vim9script:
		    ଯଦି (cctx->ctx_skip != SKIP_YES)
		    {
			emsg(_(e_vim9script_can_only_be_used_in_script));
			ଫେରନ୍ତୁ FAIL;
		    }
		    ଧାଡ଼ି = (char_u *)"";
		    ଭାଙ୍ଗ;

	    ସ୍ଥିତି CMD_class:
		    emsg(_(e_class_can_only_be_used_in_script));
		    ଫେରନ୍ତୁ FAIL;

	    ସ୍ଥିତି CMD_enum:
		    emsg(_(e_enum_can_only_be_used_in_script));
		    ଫେରନ୍ତୁ FAIL;

	    ସ୍ଥିତି CMD_interface:
		    emsg(_(e_interface_can_only_be_used_in_script));
		    ଫେରନ୍ତୁ FAIL;

	    ସ୍ଥିତି CMD_type:
		    emsg(_(e_type_can_only_be_used_in_script));
		    ଫେରନ୍ତୁ FAIL;

	    ସ୍ଥିତି CMD_global:
		    ଯଦି (check_global_and_subst(ea.cmd, p) == FAIL)
			ଫେରନ୍ତୁ FAIL;
		    // FALLTHROUGH
	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		    // Not recognized, execute with do_cmdline_cmd().
		    ea.arg = p;
		    ଧାଡ଼ି = compile_exec(ଧାଡ଼ି, &ea, cctx);
		    ଭାଙ୍ଗ;
	}
nextline:
	ଯଦି (ଧାଡ଼ି == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;
	ଧାଡ଼ି = skipwhite(ଧାଡ଼ି);

	// Undo any command modifiers.
	generate_undo_cmdmods(cctx);

	ଯଦି (cctx->ctx_type_ଗଦା.ga_len < 0)
	{
	    iemsg("Type stack underflow");
	    ଫେରନ୍ତୁ FAIL;
	}
    } // END of the loop over all the function body lines.

    ଫେରନ୍ତୁ OK;
}

/*
 * Returns TRUE if the end of a scope (if, while, for, block) is missing.
 * Called after compiling a def function body.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_dfunc_scope_end_missing(cctx_T *cctx)
{
    ଯଦି (cctx->ctx_scope == ଖାଲି)
	ଫେରନ୍ତୁ FALSE;

    ବାଛ (cctx->ctx_scope->se_type)
    {
	ସ୍ଥିତି IF_SCOPE:
	    emsg(_(e_missing_endif));
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି WHILE_SCOPE:
	    emsg(_(e_missing_endwhile));
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି FOR_SCOPE:
	    emsg(_(e_missing_endfor));
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି TRY_SCOPE:
	    emsg(_(e_missing_endtry));
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି BLOCK_SCOPE:
	    // end block scope from :try (maybe)
	    compile_endblock(cctx);
	    ଯଦି (cctx->ctx_scope != ଖାଲି
		    && cctx->ctx_scope->se_type == TRY_SCOPE)
		emsg(_(e_missing_endtry));
	    ଅନ୍ୟଥା
		emsg(_(e_missing_rcurly));
	    ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    emsg(_(e_missing_rcurly));
    }
    ଫେରନ୍ତୁ TRUE;
}

/*
 * When compiling a def function, if it doesn't have an explicit return
 * statement, then generate a default return instruction.  For an object
 * constructor, return the object.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_dfunc_generate_default_return(ufunc_T *ufunc, cctx_T *cctx)
{
    // TODO: if a function ends in "throw" but there was a return elsewhere we
    // should not assume the return type is "void".
    ଯଦି (cctx->ctx_had_return || cctx->ctx_had_throw)
	ଫେରନ୍ତୁ OK;

    ଯଦି (ufunc->uf_ret_type->tt_type == VAR_UNKNOWN)
	ufunc->uf_ret_type = &t_void;
    ଅନ୍ୟଥା ଯଦି (ufunc->uf_ret_type->tt_type != VAR_VOID
					&& !IS_CONSTRUCTOR_METHOD(ufunc))
    {
	emsg(_(e_missing_return_statement));
	ଫେରନ୍ତୁ FAIL;
    }

    // Return void if there is no return at the end.
    // For a constructor return the object.
    ଯଦି (IS_CONSTRUCTOR_METHOD(ufunc))
    {
	generate_instr(cctx, ISN_RETURN_OBJECT);
	ufunc->uf_ret_type = &ufunc->uf_class->class_object_type;
    }
    ଅନ୍ୟଥା
	generate_instr(cctx, ISN_RETURN_VOID);

    ଫେରନ୍ତୁ OK;
}

/*
 * Perform the chores after successfully compiling a def function.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
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
    ଯଦି (cctx->ctx_compile_type == CT_PROFILE)
    {
	dfunc->df_instr_prof = instr->ga_data;
	dfunc->df_instr_prof_ଗଣନା = instr->ga_len;
    }
    ଅନ୍ୟଥା
#endif
	ଯଦି (cctx->ctx_compile_type == CT_DEBUG)
	{
	    dfunc->df_instr_debug = instr->ga_data;
	    dfunc->df_instr_debug_ଗଣନା = instr->ga_len;
	}
	ଅନ୍ୟଥା
	{
	    dfunc->df_instr = instr->ga_data;
	    dfunc->df_instr_ଗଣନା = instr->ga_len;
	}
    dfunc->df_varcount = dfunc->df_var_names.ga_len;
    dfunc->df_has_closure = cctx->ctx_has_closure;

    ଯଦି (cctx->ctx_outer_used)
    {
	ufunc->uf_flags |= FC_CLOSURE;
	ଯଦି (outer_cctx != ଖାଲି)
	    ++outer_cctx->ctx_closure_ଗଣନା;
    }

    ufunc->uf_def_status = UF_COMPILED;
}

/*
 * Perform the cleanup when a def function compilation fails.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
compile_dfunc_ufunc_cleanup(
    ufunc_T	*ufunc,
    garray_T	*instr,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		new_def_function,
    ଅକ୍ଷର	*errormsg,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_emsg_before,
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
    ଯଦି (!dfunc->df_deleted && new_def_function
			&& ufunc->uf_dfunc_idx == def_functions.ga_len - 1)
    {
	--def_functions.ga_len;
	ufunc->uf_dfunc_idx = 0;
    }
    ufunc->uf_def_status = UF_COMPILE_ERROR;

    ଯେତେବେଳେ (cctx->ctx_scope != ଖାଲି)
	drop_scope(cctx);

    ଯଦି (errormsg != ଖାଲି)
	emsg(errormsg);
    ଅନ୍ୟଥା ଯଦି (did_emsg == did_emsg_before)
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_def_function(
	ufunc_T		*ufunc,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		check_return_type,
	compiletype_T   compile_type,
	cctx_T		*outer_cctx)
{
    garray_T	ଧାଡ଼ିଗୁଡ଼ିକ_to_free;
    ଅକ୍ଷର	*errormsg = ଖାଲି;	// error message
    cctx_T	cctx;
    garray_T	*instr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_emsg_before = did_emsg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_emsg_silent_before = did_emsg_silent;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret = FAIL;
    sctx_T	save_current_sctx = current_sctx;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_estack_compiling = estack_compiling;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_cmod_flags = cmdmod.cmod_flags;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		do_estack_ଠେଲ;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		new_def_function = FALSE;

    // allocated lines are freed at the end
    ga_init2(&ଧାଡ଼ିଗୁଡ଼ିକ_to_free, ଆକାର(char_u *), 50);

    // Initialize the ufunc and the compilation context
    ଯଦି (compile_dfunc_ufunc_init(ufunc, outer_cctx, compile_type,
						&new_def_function) == FAIL)
	ଫେରନ୍ତୁ FAIL;

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
    do_estack_ଠେଲ = !estack_ଶୀର୍ଷ_is_ufunc(ufunc, 1);
    ଯଦି (do_estack_ଠେଲ)
	estack_ଠେଲ_ufunc(ufunc, 1);
    estack_compiling = TRUE;

    // Make sure arguments don't shadow variables in the context
    ଯଦି (check_args_shadowing(ufunc, &cctx) == FAIL)
	ଯାଅ erret;

    // For an object method and a constructor generate instructions to
    // initialize "this" and the object variables.
    ଯଦି (ufunc->uf_flags & (FC_OBJECT|FC_NEW))
	ଯଦି (obj_method_prologue(ufunc, &cctx) == FAIL)
	    ଯାଅ erret;

    ଯଦି (ufunc->uf_def_args.ga_len > 0)
	ଯଦି (compile_def_function_default_args(ufunc, instr, &cctx) == FAIL)
	    ଯାଅ erret;
    ufunc->uf_args_visible = ufunc->uf_args.ga_len;

    // Compiling an abstract method or a function in an interface is done to
    // get the function type.  No code is actually compiled.
    ଯଦି (ufunc->uf_class != ଖାଲି && (IS_INTERFACE(ufunc->uf_class)
						|| IS_ABSTRACT_METHOD(ufunc)))
    {
	ufunc->uf_def_status = UF_NOT_COMPILED;
	ret = OK;
	ଯାଅ erret;
    }

    // compile the function body
    ଯଦି (compile_def_function_body(ufunc->uf_ଧାଡ଼ିଗୁଡ଼ିକ.ga_len, check_return_type,
				&ଧାଡ଼ିଗୁଡ଼ିକ_to_free, &errormsg, &cctx) == FAIL)
	ଯାଅ erret;

    ଯଦି (compile_dfunc_scope_end_missing(&cctx))
	ଯାଅ erret;

    ଯଦି (compile_dfunc_generate_default_return(ufunc, &cctx) == FAIL)
	ଯାଅ erret;

    // When compiled with ":silent!" and there was an error don't consider the
    // function compiled.
    ଯଦି (emsg_silent == 0 || did_emsg_silent == did_emsg_silent_before)
	compile_dfunc_epilogue(outer_cctx, ufunc, instr, &cctx);

    ret = OK;

erret:
    ଯଦି (ufunc->uf_def_status == UF_COMPILING)
    {
	// compilation failed. do cleanup.
	compile_dfunc_ufunc_cleanup(ufunc, instr, new_def_function,
				    errormsg, did_emsg_before, &cctx);
    }

    ଯଦି (cctx.ctx_redir_lhs.lhs_name != ଖାଲି)
    {
	ଯଦି (ret == OK)
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
    ଯଦି (do_estack_ଠେଲ)
	estack_କାଢ଼();

    ga_clear_strings(&ଧାଡ଼ିଗୁଡ଼ିକ_to_free);
    free_locals(&cctx);
    ga_clear(&cctx.ctx_type_ଗଦା);
    ଫେରନ୍ତୁ ret;
}

    ଶୂନ୍ୟ
set_function_type(ufunc_T *ufunc)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା varargs = ufunc->uf_va_name != ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount = ufunc->uf_args.ga_len;

    // Create a type for the function, with the return type and any
    // argument types.
    // A vararg is included in uf_args.ga_len but not in uf_arg_types.
    // The type is included in "tt_args".
    ଯଦି (argcount > 0 || varargs)
    {
	ଯଦି (ufunc->uf_type_list.ga_itemsize == 0)
	    ga_init2(&ufunc->uf_type_list, ଆକାର(type_T *), 10);
	ufunc->uf_func_type = alloc_func_type(ufunc->uf_ret_type,
					   argcount, &ufunc->uf_type_list);
	// Add argument types to the function type.
	ଯଦି (func_type_add_arg_types(ufunc->uf_func_type,
				    argcount + varargs,
				    &ufunc->uf_type_list) == FAIL)
	    ଫେରନ୍ତୁ;
	ufunc->uf_func_type->tt_argcount = argcount + varargs;
	ufunc->uf_func_type->tt_min_argcount =
				      argcount - ufunc->uf_def_args.ga_len;
	ଯଦି (ufunc->uf_arg_types == ଖାଲି)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

	    // lambda does not have argument types.
	    ପାଇଁ (i = 0; i < argcount; ++i)
		ufunc->uf_func_type->tt_args[i] = &t_any;
	}
	ଅନ୍ୟଥା
	    mch_memmove(ufunc->uf_func_type->tt_args,
			 ufunc->uf_arg_types, ଆକାର(type_T *) * argcount);
	ଯଦି (varargs)
	{
	    ufunc->uf_func_type->tt_args[argcount] =
		   ufunc->uf_va_type == ଖାଲି ? &t_list_any : ufunc->uf_va_type;
	    ufunc->uf_func_type->tt_flags = TTFLAG_VARARGS;
	}
    }
    ଅନ୍ୟଥା
	// No arguments, can use a predefined type.
	ufunc->uf_func_type = get_func_type(ufunc->uf_ret_type,
					   argcount, &ufunc->uf_type_list);
}

/*
 * Free all instructions for "dfunc" except df_name.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
delete_def_function_contents(dfunc_T *dfunc, ପୂର୍ଣ୍ଣସଂଖ୍ୟା mark_deleted)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx;

    // In same cases the instructions may refer to a class in which the
    // function is defined and unreferencing the class may call back here
    // recursively.  Set the df_delete_busy to avoid problems.
    ଯଦି (dfunc->df_delete_busy)
	ଫେରନ୍ତୁ;
    dfunc->df_delete_busy = TRUE;

    ga_clear(&dfunc->df_def_args_isn);
    ga_clear_strings(&dfunc->df_var_names);

    ଯଦି (dfunc->df_instr != ଖାଲି)
    {
	ପାଇଁ (idx = 0; idx < dfunc->df_instr_ଗଣନା; ++idx)
	    delete_instr(dfunc->df_instr + idx);
	VIM_CLEAR(dfunc->df_instr);
    }
    ଯଦି (dfunc->df_instr_debug != ଖାଲି)
    {
	ପାଇଁ (idx = 0; idx < dfunc->df_instr_debug_ଗଣନା; ++idx)
	    delete_instr(dfunc->df_instr_debug + idx);
	VIM_CLEAR(dfunc->df_instr_debug);
    }
#ifdef FEAT_PROFILE
    ଯଦି (dfunc->df_instr_prof != ଖାଲି)
    {
	ପାଇଁ (idx = 0; idx < dfunc->df_instr_prof_ଗଣନା; ++idx)
	    delete_instr(dfunc->df_instr_prof + idx);
	VIM_CLEAR(dfunc->df_instr_prof);
    }
#endif

    ଯଦି (mark_deleted)
	dfunc->df_deleted = TRUE;
    ଯଦି (dfunc->df_ufunc != ଖାଲି)
	dfunc->df_ufunc->uf_def_status = UF_NOT_COMPILED;

    dfunc->df_delete_busy = FALSE;
}

/*
 * When a user function is deleted, clear the contents of any associated def
 * function, unless another user function still uses it.
 * The position in def_functions can be re-used.
 */
    ଶୂନ୍ୟ
unlink_def_function(ufunc_T *ufunc)
{
    ଯଦି (ufunc->uf_dfunc_idx <= 0 || def_functions.ga_data == ଖାଲି)
	ଫେରନ୍ତୁ;

    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data)
	+ ufunc->uf_dfunc_idx;

    ଯଦି (--dfunc->df_refcount <= 0)
	delete_def_function_contents(dfunc, TRUE);
    ufunc->uf_def_status = UF_NOT_COMPILED;
    ufunc->uf_dfunc_idx = 0;
    ଯଦି (dfunc->df_ufunc == ufunc)
	dfunc->df_ufunc = ଖାଲି;
}

/*
 * Used when a user function refers to an existing dfunc.
 */
    ଶୂନ୍ୟ
link_def_function(ufunc_T *ufunc)
{
    ଯଦି (ufunc->uf_dfunc_idx <= 0)
	ଫେରନ୍ତୁ;

    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data)
	+ ufunc->uf_dfunc_idx;

    ++dfunc->df_refcount;
}

#ଯଦି defined(EXITFREE)
/*
 * Free all functions defined with ":def".
 */
    ଶୂନ୍ୟ
free_def_functions(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx;

    ପାଇଁ (idx = 0; idx < def_functions.ga_len; ++idx)
    {
	dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data) + idx;

	delete_def_function_contents(dfunc, TRUE);
	vim_free(dfunc->df_name);
    }

    ga_clear(&def_functions);
}
#endif


#endif // FEAT_EVAL
