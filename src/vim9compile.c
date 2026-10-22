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

#وصف_ڪريو USING_FLOAT_STUFF
#ضمن "vim.h"

#إذا defined(FEAT_EVAL)

// Functions defined with :def are stored in this growarray.
// They are never removed, so that they can be found by index.
// Deleted functions have the df_deleted flag set.
garray_T def_functions = {0, 0, حجم(dfunc_T), 50, عدم};

ساكن فراغ delete_def_function_contents(dfunc_T *dfunc, صحيح mark_deleted);

/*
 * Lookup variable "name" in the local scope and return it in "lvar".
 * "lvar->lv_from_outer" is incremented accordingly.
 * If "lvar" is NULL only check if the variable can be found.
 * Return FAIL if not found.
 */
    صحيح
lookup_local(char_u *name, size_t len, lvar_T *lvar, cctx_T *cctx)
{
    صحيح	    idx;
    lvar_T  *lvp;

    إذا (len == 0)
	أرجع FAIL;

    إذا (((len == 4 && STRNCMP(name, "this", 4) == 0)
		|| (len == 5 && STRNCMP(name, "super", 5) == 0))
	    && cctx->ctx_ufunc != عدم
	    && (cctx->ctx_ufunc->uf_flags & (FC_OBJECT|FC_NEW)))
    {
	صحيح is_super = *name == 's';
	إذا (is_super)
	{
	    إذا (name[5] != '.')
	    {
		emsg(_(e_super_must_be_followed_by_dot));
		أرجع FAIL;
	    }
	    إذا (cctx->ctx_ufunc->uf_class != عدم
		    && cctx->ctx_ufunc->uf_class->class_extends == عدم)
	    {
		emsg(_(e_using_super_not_اندريون_ٻار_class));
		أرجع FAIL;
	    }
	}
	إذا (lvar != عدم)
	{
	    CLEAR_اشارو(lvar);
	    lvar->lv_loop_اونهائي = -1;
	    lvar->lv_name = (char_u *)(is_super ? "super" : "this");
	    إذا (cctx->ctx_ufunc->uf_class != عدم)
	    {
		lvar->lv_type = &cctx->ctx_ufunc->uf_class->class_object_type;
		إذا (is_super)
		{
		    type_T *type = get_type_ptr(cctx->ctx_type_list);

		    إذا (type != عدم)
		    {
			*type = *lvar->lv_type;
			lvar->lv_type = type;
			type->tt_flags |= TTFLAG_SUPER;
		    }
		}
	    }
	}
	أرجع OK;
    }

    // Find local in current function scope.
    لكل (idx = 0; idx < cctx->ctx_locals.ga_len; ++idx)
    {
	lvp = ((lvar_T *)cctx->ctx_locals.ga_معلومات) + idx;
	إذا (lvp->lv_name != عدم
		&& STRNCMP(name, lvp->lv_name, len) == 0
					       && STRLEN(lvp->lv_name) == len)
	{
	    إذا (lvar != عدم)
	    {
		*lvar = *lvp;
		lvar->lv_from_outer = 0;
		// If the variable was declared inside a loop set
		// lvar->lv_loop_idx and lvar->lv_loop_depth.
		get_loop_var_idx(cctx, idx, lvar);
	    }
	    أرجع OK;
	}
    }

    // Find local in outer function scope.
    إذا (cctx->ctx_outer != عدم)
    {
	إذا (lookup_local(name, len, lvar, cctx->ctx_outer) == OK)
	{
	    إذا (lvar != عدم)
	    {
		cctx->ctx_outer_استعمال_ٿيل = TRUE;
		++lvar->lv_from_outer;
	    }
	    أرجع OK;
	}
    }

    أرجع FAIL;
}

/*
 * Lookup an argument in the current function and an enclosing function.
 * Returns the argument index in "idxp"
 * Returns the argument type in "type"
 * Sets "gen_load_outer" to TRUE if found in outer scope.
 * Returns OK when found, FAIL otherwise.
 */
    صحيح
arg_exists(
	char_u	*name,
	size_t	len,
	صحيح	*idxp,
	type_T	**type,
	صحيح	*gen_ڀريو_outer,
	cctx_T	*cctx)
{
    صحيح	    idx;
    char_u  *va_name;

    إذا (len == 0)
	أرجع FAIL;
    لكل (idx = 0; idx < cctx->ctx_ufunc->uf_دليلون_visible; ++idx)
    {
	char_u *arg = FUNCARG(cctx->ctx_ufunc, idx);

	إذا (STRNCMP(name, arg, len) == 0 && arg[len] == NUL)
	{
	    إذا (idxp != عدم)
	    {
		// Arguments are located above the frame pointer.  One further
		// if there is a vararg argument
		*idxp = idx - (cctx->ctx_ufunc->uf_دليلون.ga_len
							    + ڍير_چوکٺو_سائيز)
			      + (cctx->ctx_ufunc->uf_va_name != عدم ? -1 : 0);

		إذا (cctx->ctx_ufunc->uf_arg_types != عدم)
		    *type = cctx->ctx_ufunc->uf_arg_types[idx];
		وإلا
		    *type = &t_any;
	    }
	    أرجع OK;
	}
    }

    va_name = cctx->ctx_ufunc->uf_va_name;
    إذا (va_name != عدم
		    && STRNCMP(name, va_name, len) == 0 && va_name[len] == NUL)
    {
	إذا (idxp != عدم)
	{
	    // varargs is always the last argument
	    *idxp = -ڍير_چوکٺو_سائيز - 1;
	    *type = cctx->ctx_ufunc->uf_va_type;
	}
	أرجع OK;
    }

    إذا (cctx->ctx_outer != عدم)
    {
	// Lookup the name for an argument of the outer function.
	إذا (arg_exists(name, len, idxp, type, gen_ڀريو_outer, cctx->ctx_outer)
									 == OK)
	{
	    إذا (gen_ڀريو_outer != عدم)
	    {
		++*gen_ڀريو_outer;
		cctx->ctx_outer_استعمال_ٿيل = TRUE;
	    }
	    أرجع OK;
	}
    }

    أرجع FAIL;
}

/*
 * Lookup a script-local variable in the current script, possibly defined in a
 * block that contains the function "cctx->ctx_ufunc".
 * "cctx" is NULL at the script level, "cstack" is NULL in a function.
 * If "len" is <= 0 "name" must be NUL terminated.
 * Return NULL when not found.
 */
    ساكن sallvar_T *
ڳوليو_script_var(char_u *name, size_t len, cctx_T *cctx, cstack_T *cstack)
{
    scriptitem_T    *si = SCRIPT_شي(current_sctx.sc_sid);
    hashitem_T	    *hi;
    صحيح		    cc;
    sallvar_T	    *sav;
    ufunc_T	    *ufunc;

    // Find the list of all script variables with the right name.
    إذا (len > 0)
    {
	cc = name[len];
	name[len] = NUL;
    }
    hi = hash_ڳوليو(&si->sn_all_vars.dv_hashtab, name);
    إذا (len > 0)
	name[len] = cc;
    إذا (HASHITEM_خالي_جايون(hi))
	أرجع عدم;

    sav = HI2SAV(hi);
    إذا (sav->sav_بلاڪ_id == 0)
	// variable defined in the top script scope is always visible
	أرجع sav;

    إذا (cctx == عدم)
    {
	إذا (cstack == عدم)
	    أرجع عدم;

	// Not in a function scope, find variable with block ID equal to or
	// smaller than the current block id.  Use "cstack" to go up the block
	// scopes.
	طالما (sav != عدم)
	{
	    صحيح idx;

	    لكل (idx = cstack->cs_idx; idx >= 0; --idx)
		إذا (cstack->cs_بلاڪ_id[idx] == sav->sav_بلاڪ_id)
		    اكسر;
	    إذا (idx >= 0)
		اكسر;
	    sav = sav->sav_ايندڙ;
	}
	أرجع sav;
    }

    // Go over the variables with this name and find one that was visible
    // from the function.
    ufunc = cctx->ctx_ufunc;
    طالما (sav != عدم)
    {
	صحيح idx;

	إذا (ufunc->uf_بلاڪ_اونهائي == 0 && sav->sav_بلاڪ_id == 0)
	    // If the function was defined at the script level (not inside a
	    // block), script-scope variables are always visible.
	    أرجع sav;

	// Go over the blocks that this function was defined in.  If the
	// variable block ID matches it was visible to the function.
	لكل (idx = 0; idx < ufunc->uf_بلاڪ_اونهائي; ++idx)
	    إذا (ufunc->uf_بلاڪ_ids[idx] == sav->sav_بلاڪ_id)
		أرجع sav;
	sav = sav->sav_ايندڙ;
    }

    // Not found, variable was not visible.
    أرجع عدم;
}

/*
 * If "name" can be found in the current script set it's "block_id".
 */
    فراغ
update_script_var_بلاڪ_id(char_u *name, صحيح بلاڪ_id)
{
    scriptitem_T    *si = SCRIPT_شي(current_sctx.sc_sid);
    hashitem_T	    *hi;
    sallvar_T	    *sav;

    hi = hash_ڳوليو(&si->sn_all_vars.dv_hashtab, name);
    إذا (HASHITEM_خالي_جايون(hi))
	أرجع;
    sav = HI2SAV(hi);
    sav->sav_بلاڪ_id = بلاڪ_id;
}

/*
 * Lookup a variable (without s: prefix) in the current script.
 * "cctx" is NULL at the script level, "cstack" is NULL in a function.
 * Returns OK or FAIL.
 */
    صحيح
script_var_exists(char_u *name, size_t len, cctx_T *cctx, cstack_T *cstack)
{
    إذا (current_sctx.sc_sid <= 0)
	أرجع FAIL;
    إذا (current_script_is_vim9())
    {
	// Check script variables that were visible where the function was
	// defined.
	إذا (ڳوليو_script_var(name, len, cctx, cstack) != عدم)
	    أرجع OK;
    }
    وإلا
    {
	hashtab_T	*ht = &SCRIPT_VARS(current_sctx.sc_sid);
	dictitem_T	*di;
	صحيح		cc;

	// Check script variables that are currently visible
	cc = name[len];
	name[len] = NUL;
	di = ڳوليو_var_اندريون_ht(ht, 0, name, TRUE);
	name[len] = cc;
	إذا (di != عدم)
	    أرجع OK;
    }

    أرجع FAIL;
}

/*
 * Returns the index of a class method or class variable with name "name"
 * accessible in the currently compiled function.
 * If "cl_ret" is not NULL set it to the class.
 * Otherwise return -1.
 */
    ساكن صحيح
cctx_class_midx(
    cctx_T  *cctx,
    صحيح	    is_طريقو,
    char_u  *name,
    size_t  len,
    class_T **cl_ret)
{
    إذا (cctx == عدم || cctx->ctx_ufunc == عدم
	    || cctx->ctx_ufunc->uf_class == عدم
	    || cctx->ctx_ufunc->uf_defclass == عدم)
	أرجع -1;

    // Search for the class method or variable in the class where the calling
    // function is defined.
    class_T *cl = cctx->ctx_ufunc->uf_defclass;
    صحيح m_idx = is_طريقو ? class_طريقو_idx(cl, name, len)
					: class_member_idx(cl, name, len);
    إذا (m_idx < 0)
    {
	cl = cl->class_extends;
	طالما (cl != عدم)
	{
	    m_idx = is_طريقو ? class_طريقو_idx(cl, name, len)
					: class_member_idx(cl, name, len);
	    إذا (m_idx >= 0)
		اكسر;
	    cl = cl->class_extends;
	}
    }

    إذا (m_idx >= 0)
    {
	إذا (cl_ret != عدم)
	    *cl_ret = cl;
    }

    أرجع m_idx;
}

/*
 * Returns the index of a class method with name "name" accessible in the
 * currently compiled function.  Returns -1 if not found.  The class where the
 * method is defined is returned in "cl_ret".
 */
    صحيح
cctx_class_طريقو_idx(
    cctx_T  *cctx,
    char_u  *name,
    size_t  len,
    class_T **cl_ret)
{
    أرجع cctx_class_midx(cctx, TRUE, name, len, cl_ret);
}

/*
 * Returns the index of a class variable with name "name" accessible in the
 * currently compiled function.  Returns -1 if not found.  The class where the
 * variable is defined is returned in "cl_ret".
 */
    صحيح
cctx_class_member_idx(
    cctx_T  *cctx,
    char_u  *name,
    size_t  len,
    class_T **cl_ret)
{
    أرجع cctx_class_midx(cctx, FALSE, name, len, cl_ret);
}

/*
 * Return TRUE if "name" is a local variable, argument, script variable or
 * imported.  Also if "name" is "this" and in a class method.
 */
    ساكن صحيح
variable_exists(char_u *name, size_t len, cctx_T *cctx)
{
    أرجع (cctx != عدم
		&& (lookup_local(name, len, عدم, cctx) == OK
		    || arg_exists(name, len, عدم, عدم, عدم, cctx) == OK
		    || (len == 4
			&& cctx->ctx_ufunc != عدم
			&& (cctx->ctx_ufunc->uf_flags & (FC_OBJECT|FC_NEW))
			&& STRNCMP(name, "this", 4) == 0)))
	    || script_var_exists(name, len, cctx, عدم) == OK
	    || cctx_class_member_idx(cctx, name, len, عدم) >= 0
	    || ڳوليو_imported(name, len, FALSE) != عدم;
}

/*
 * Return TRUE if "name" is a local variable, argument, script variable,
 * imported or function.  Or commands are being skipped, a declaration may have
 * been skipped then.
 */
    ساكن صحيح
شي_exists(char_u *name, size_t len, صحيح cmd اڻ_استعمال_ٿيل, cctx_T *cctx)
{
    أرجع variable_exists(name, len, cctx);
}

/*
 * Check if "p[len]" is already defined, either in script "import_sid" or in
 * compilation context "cctx".
 * "cctx" is NULL at the script level, "cstack" is NULL in a function.
 * Does not check the global namespace.
 * If "is_arg" is TRUE the error message is for an argument name.
 * Return FAIL and give an error if it defined.
 */
    صحيح
check_defined(
	char_u	    *p,
	size_t	    len,
	cctx_T	    *cctx,
	cstack_T    *cstack,
	صحيح	    is_arg)
{
    صحيح		c = p[len];
    ufunc_T	*ufunc = عدم;

    // underscore argument is OK
    إذا (len == 1 && *p == '_')
	أرجع OK;

    إذا (script_var_exists(p, len, cctx, cstack) == OK)
    {
	إذا (is_arg)
	    semsg(_(e_دليل_already_declared_اندريون_script_str), p);
	وإلا
	    semsg(_(e_variable_already_declared_اندريون_script_str), p);
	أرجع FAIL;
    }

    إذا (cctx_class_member_idx(cctx, p, len, عدم) >= 0)
    {
	إذا (is_arg)
	    semsg(_(e_دليل_already_declared_اندريون_class_str), p);
	وإلا
	    semsg(_(e_variable_already_declared_اندريون_class_str), p);
	أرجع FAIL;
    }

    p[len] = NUL;
    إذا ((cctx != عدم
		&& (lookup_local(p, len, عدم, cctx) == OK
		    || arg_exists(p, len, عدم, عدم, عدم, cctx) == OK))
	    || ڳوليو_imported(p, len, FALSE) != عدم
	    || (ufunc = ڳوليو_func_even_dead(p, 0)) != عدم)
    {
	// A local or script-local function can shadow a global function.
	إذا (ufunc == عدم || ((ufunc->uf_flags & FC_DEAD) == 0
		    && (!func_is_global(ufunc)
					     || (p[0] == 'g' && p[1] == ':'))))
	{
	    إذا (is_arg)
		semsg(_(e_دليل_name_shadows_existing_variable_str), p);
	    وإلا
		semsg(_(e_name_already_defined_str), p);
	    p[len] = c;
	    أرجع FAIL;
	}
    }
    p[len] = c;
    أرجع OK;
}


/*
 * Return TRUE if "actual" could be "expected" and a runtime typecheck is to be
 * used.  Return FALSE if the types will never match.
 */
    ساكن صحيح
use_typecheck(type_T *actual, type_T *expected)
{
    إذا (actual->tt_type == VAR_ANY
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
	أرجع TRUE;
    إذا (actual->tt_type == VAR_OBJECT && expected->tt_type == VAR_OBJECT)
	أرجع TRUE;
    إذا ((actual->tt_type == VAR_LIST
		|| actual->tt_type == VAR_TUPLE
		|| actual->tt_type == VAR_DICT)
	    && actual->tt_type == expected->tt_type)
	// This takes care of a nested list or dict.
	أرجع use_typecheck(actual->tt_member, expected->tt_member);
    أرجع FALSE;
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
    صحيح
need_type_where(
	type_T	*actual,
	type_T	*expected,
	صحيح	typechk_flags,	// acceptable types (type check flags)
	صحيح	وٿي,
	where_T	where,
	cctx_T	*cctx,
	صحيح	silent,
	صحيح	actual_is_const)
{
    صحيح ret;

    إذا (expected->tt_type != VAR_CLASS && expected->tt_type != VAR_TYPEALIAS)
    {
	إذا (check_type_is_قدر(actual) == FAIL)
	    أرجع FAIL;
    }

    إذا (expected == &t_bool && actual != &t_bool
					&& (actual->tt_flags & TTFLAG_BOOL_OK))
    {
	// Using "0", "1" or the result of an expression with "&&" or "||" as a
	// boolean is OK but requires a conversion.
	generate_2BOOL(cctx, FALSE, وٿي);
	أرجع OK;
    }

    ret = check_type_maybe(expected, actual, FALSE, where);
    إذا (ret == OK)
	أرجع OK;

    // If actual a constant a runtime check makes no sense.  If it's
    // null_function it is OK.
    إذا (actual_is_const && ret == MAYBE && actual == &t_func_unknown)
	أرجع OK;

    // If the actual type can be the expected type add a runtime check.
    إذا (!actual_is_const && ret == MAYBE && use_typecheck(actual, expected))
    {
	generate_TYPECHECK(cctx, expected, typechk_flags, وٿي,
		where.wt_قسم == WT_VARIABLE, where.wt_اشاريو);
	أرجع OK;
    }

    إذا (!silent)
	type_mismatch_where(expected, actual, where);
    أرجع FAIL;
}

    صحيح
need_type(
	type_T	*actual,
	type_T	*expected,
	صحيح	typechk_flags,	// acceptable types (type check flags)
	صحيح	وٿي,
	صحيح	arg_idx,
	cctx_T	*cctx,
	صحيح	silent,
	صحيح	actual_is_const)
{
    where_T where = WHERE_INIT;

    إذا (arg_idx > 0)
    {
	where.wt_اشاريو = arg_idx;
	where.wt_قسم = WT_دليل;
    }
    أرجع need_type_where(actual, expected, typechk_flags, وٿي, where,
						cctx, silent, actual_is_const);
}

/*
 * Set type of variable "lvar" to "type".  If the variable is a constant then
 * the type gets TTFLAG_CONST.
 */
    ساكن فراغ
سيٽ_var_type(lvar_T *lvar, type_T *type_arg, cctx_T *cctx)
{
    type_T	*type = type_arg;

    إذا (lvar->lv_const == سونپيو_CONST && (type->tt_flags & TTFLAG_CONST) == 0)
    {
	إذا (type->tt_flags & TTFLAG_STATIC)
	    // entry in static_types[] is followed by const type
	    type = type + 1;
	وإلا
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
	صحيح	سونپيو,
	type_T	*type)
{
    lvar_T  *lvar;
    dfunc_T *dfunc;

    إذا (arg_exists(name, len, عدم, عدم, عدم, cctx) == OK)
    {
	emsg_namelen(_(e_str_is_استعمال_ٿيل_as_دليل), name, (صحيح)len);
	أرجع عدم;
    }

    إذا (GA_GROW_FAILS(&cctx->ctx_locals, 1))
	أرجع عدم;
    lvar = ((lvar_T *)cctx->ctx_locals.ga_معلومات) + cctx->ctx_locals.ga_len++;
    CLEAR_اشارو(lvar);

    // Every local variable uses the next entry on the stack.  We could re-use
    // the last ones when leaving a scope, but then variables used in a closure
    // might get overwritten.  To keep things simple do not re-use stack
    // entries.  This is less efficient, but memory is cheap these days.
    dfunc = ((dfunc_T *)def_functions.ga_معلومات) + cctx->ctx_ufunc->uf_dfunc_idx;
    lvar->lv_idx = dfunc->df_var_نالا.ga_len;

    lvar->lv_name = vim_strnsave(name, len == 0 ? STRLEN(name) : len);
    lvar->lv_const = سونپيو;
    إذا (type == &t_unknown || type == &t_any)
	// type not known yet, may be inferred from RHS
	lvar->lv_type = type;
    وإلا
	// may use TTFLAG_CONST
	سيٽ_var_type(lvar, type, cctx);

    // Remember the name for debugging.
    إذا (GA_GROW_FAILS(&dfunc->df_var_نالا, 1))
	أرجع عدم;
    ((char_u **)dfunc->df_var_نالا.ga_معلومات)[lvar->lv_idx] =
						    vim_strsave(lvar->lv_name);
    ++dfunc->df_var_نالا.ga_len;

    أرجع lvar;
}

/*
 * If "check_writable" is ASSIGN_CONST give an error if the variable was
 * defined with :final or :const, if "check_writable" is ASSIGN_FINAL give an
 * error if the variable was defined with :const.
 */
    ساكن صحيح
check_شي_writable(svar_T *sv, صحيح check_writable, char_u *name)
{
    إذا ((check_writable == سونپيو_CONST && sv->sv_const != 0)
	    || (check_writable == سونپيو_FINAL
					      && sv->sv_const == سونپيو_CONST))
    {
	semsg(_(e_cannot_change_readonly_variable_str), name);
	أرجع FAIL;
    }
    أرجع OK;
}

/*
 * Find "name" in script-local items of script "sid".
 * Pass "check_writable" to check_item_writable().
 * "cctx" is NULL at the script level, "cstack" is NULL in a function.
 * Returns the index in "sn_var_vals" if found.
 * If found but not in "sn_var_vals" returns -1.
 * If not found or the variable is not writable returns -2.
 */
    صحيح
get_script_شي_idx(
	صحيح	    sid,
	char_u	    *name,
	صحيح	    check_writable,
	cctx_T	    *cctx,
	cstack_T    *cstack)
{
    hashtab_T	    *ht;
    dictitem_T	    *di;
    scriptitem_T    *si = SCRIPT_شي(sid);
    svar_T	    *sv;
    صحيح		    idx;

    إذا (!SCRIPT_ID_VALID(sid))
	أرجع -1;
    إذا (sid == current_sctx.sc_sid)
    {
	sallvar_T *sav = ڳوليو_script_var(name, 0, cctx, cstack);

	إذا (sav == عدم)
	    أرجع -2;
	idx = sav->sav_var_vals_idx;
	sv = ((svar_T *)si->sn_var_vals.ga_معلومات) + idx;
	إذا (check_شي_writable(sv, check_writable, name) == FAIL)
	    أرجع -2;
	أرجع idx;
    }

    // First look the name up in the hashtable.
    ht = &SCRIPT_VARS(sid);
    di = ڳوليو_var_اندريون_ht(ht, 0, name, TRUE);
    إذا (di == عدم)
    {
	إذا (si->sn_autoload_اڳياڙي != عدم)
	{
	    hashitem_T *hi;

	    // A variable exported from an autoload script is in the global
	    // variables, we can find it in the all_vars table.
	    hi = hash_ڳوليو(&si->sn_all_vars.dv_hashtab, name);
	    إذا (!HASHITEM_خالي_جايون(hi))
		أرجع HI2SAV(hi)->sav_var_vals_idx;
	}
	أرجع -2;
    }

    // Now find the svar_T index in sn_var_vals.
    لكل (idx = 0; idx < si->sn_var_vals.ga_len; ++idx)
    {
	sv = ((svar_T *)si->sn_var_vals.ga_معلومات) + idx;
	إذا (sv->sv_tv == &di->di_tv)
	{
	    إذا (check_شي_writable(sv, check_writable, name) == FAIL)
		أرجع -2;
	    أرجع idx;
	}
    }
    أرجع -1;
}

    ساكن imported_T *
ڳوليو_imported_اندريون_script(char_u *name, size_t len, صحيح sid)
{
    scriptitem_T    *si;
    صحيح		    idx;

    إذا (!SCRIPT_ID_VALID(sid))
	أرجع عدم;
    si = SCRIPT_شي(sid);
    لكل (idx = 0; idx < si->sn_imports.ga_len; ++idx)
    {
	imported_T *import = ((imported_T *)si->sn_imports.ga_معلومات) + idx;

	إذا (len == 0 ? STRCMP(name, import->imp_name) == 0
		     : STRLEN(import->imp_name) == len
				  && STRNCMP(name, import->imp_name, len) == 0)
	    أرجع import;
    }
    أرجع عدم;
}

/*
 * Find "name" in imported items of the current script.
 * If "len" is 0 use any length that works.
 * If "load" is TRUE and the script was not loaded yet, load it now.
 */
    imported_T *
ڳوليو_imported(char_u *name, size_t len, صحيح ڀريو)
{
    إذا (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	أرجع عدم;

    // Skip over "s:" before "s:something" to find the import name.
    صحيح off = name[0] == 's' && name[1] == ':' ? 2 : 0;

    imported_T *ret = ڳوليو_imported_اندريون_script(name + off, len - off,
							  current_sctx.sc_sid);
    إذا (ret != عدم && ڀريو && (ret->imp_flags & IMP_FLAGS_AUTOLOAD))
    {
	scid_T	actual_sid = 0;
	صحيح	save_emsg_off = emsg_off;

	// "emsg_off" will be set when evaluating an expression silently, but
	// we do want to know about errors in a script.  Also because it then
	// aborts when an error is encountered.
	emsg_off = FALSE;

	// script found before but not loaded yet
	ret->imp_flags &= ~IMP_FLAGS_AUTOLOAD;
	(فراغ)do_ذريعو(SCRIPT_شي(ret->imp_sid)->sn_name, FALSE,
						       DOSO_NONE, &actual_sid);
	// If the script is a symlink it may be sourced with another name, may
	// need to adjust the script ID for that.
	إذا (actual_sid != 0)
	    ret->imp_sid = actual_sid;

	emsg_off = save_emsg_off;
    }
    أرجع ret;
}

/*
 * Called when checking for a following operator at "arg".  When the rest of
 * the line is empty or only a comment, peek the next line.  If there is a next
 * line return a pointer to it and set "nextp".
 * Otherwise skip over white space.
 */
    char_u *
may_peek_ايندڙ_سٽ(cctx_T *cctx, char_u *arg, char_u **nextp)
{
    char_u *p = skipwhite(arg);

    *nextp = عدم;
    إذا (*p == NUL || (VIM_ISWHITE(*arg) && vim9_comment_شروعات(p)))
    {
	*nextp = peek_ايندڙ_سٽ_from_context(cctx);
	إذا (*nextp != عدم)
	    أرجع *nextp;
    }
    أرجع p;
}

/*
 * Return a pointer to the next line that isn't empty or only contains a
 * comment. Skips over white space.
 * Returns NULL if there is none.
 */
    char_u *
peek_ايندڙ_سٽ_from_context(cctx_T *cctx)
{
    صحيح lnum = cctx->ctx_lnum;

    طالما (++lnum < cctx->ctx_ufunc->uf_سٽون.ga_len)
    {
	char_u *سٽ = ((char_u **)cctx->ctx_ufunc->uf_سٽون.ga_معلومات)[lnum];
	char_u *p;

	// ignore NULLs inserted for continuation lines
	إذا (سٽ != عدم)
	{
	    p = skipwhite(سٽ);
	    إذا (vim9_bad_comment(p))
		أرجع عدم;
	    إذا (*p != NUL && !vim9_comment_شروعات(p))
		أرجع p;
	}
    }
    أرجع عدم;
}

/*
 * Get the next line of the function from "cctx".
 * Skips over empty lines.  Skips over comment lines if "skip_comment" is TRUE.
 * Returns NULL when at the end.
 */
    char_u *
ايندڙ_سٽ_from_context(cctx_T *cctx, صحيح skip_comment)
{
    char_u	*سٽ;

    افعل
    {
	++cctx->ctx_lnum;
	إذا (cctx->ctx_lnum >= cctx->ctx_ufunc->uf_سٽون.ga_len)
	{
	    سٽ = عدم;
	    اكسر;
	}
	سٽ = ((char_u **)cctx->ctx_ufunc->uf_سٽون.ga_معلومات)[cctx->ctx_lnum];
	cctx->ctx_سٽ_شروعات = سٽ;
	SOURCING_LNUM = cctx->ctx_lnum + 1;
    } طالما (سٽ == عدم || *skipwhite(سٽ) == NUL
		     || (skip_comment && vim9_comment_شروعات(skipwhite(سٽ))));
    أرجع سٽ;
}

/*
 * Skip over white space at "whitep" and assign to "*arg".
 * If "*arg" is at the end of the line, advance to the next line.
 * Also when "whitep" points to white space and "*arg" is on a "#".
 * Return FAIL if beyond the last line, "*arg" is unmodified then.
 */
    صحيح
may_get_ايندڙ_سٽ(char_u *whitep, char_u **arg, cctx_T *cctx)
{
    *arg = skipwhite(whitep);
    إذا (vim9_bad_comment(*arg))
	أرجع FAIL;
    إذا (**arg == NUL || (VIM_ISWHITE(*whitep) && vim9_comment_شروعات(*arg)))
    {
	char_u *ايندڙ = ايندڙ_سٽ_from_context(cctx, TRUE);

	إذا (ايندڙ == عدم)
	    أرجع FAIL;
	*arg = skipwhite(ايندڙ);
    }
    أرجع OK;
}

/*
 * Idem, and give an error when failed.
 */
    صحيح
may_get_ايندڙ_سٽ_error(char_u *whitep, char_u **arg, cctx_T *cctx)
{
    إذا (may_get_ايندڙ_سٽ(whitep, arg, cctx) == FAIL)
    {
	SOURCING_LNUM = cctx->ctx_lnum + 1;
	emsg(_(e_سٽ_incomplete));
	أرجع FAIL;
    }
    أرجع OK;
}

/*
 * Get a line from the compilation context, compatible with exarg_T getline().
 * Return a pointer to the line in allocated memory.
 * Return NULL for end-of-file or some error.
 */
    ساكن char_u *
exarg_getline(
	صحيح c اڻ_استعمال_ٿيل,
	فراغ *cookie,
	صحيح indent اڻ_استعمال_ٿيل,
	getline_opt_T options اڻ_استعمال_ٿيل)
{
    cctx_T  *cctx = (cctx_T *)cookie;
    char_u  *p;

    لكل (;;)
    {
	إذا (cctx->ctx_lnum >= cctx->ctx_ufunc->uf_سٽون.ga_len - 1)
	    أرجع عدم;
	++cctx->ctx_lnum;
	p = ((char_u **)cctx->ctx_ufunc->uf_سٽون.ga_معلومات)[cctx->ctx_lnum];
	// Comment lines result in NULL pointers, skip them.
	إذا (p != عدم)
	    أرجع vim_strsave(p);
    }
}

    فراغ
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
    صحيح
func_needs_compiling(ufunc_T *ufunc, compiletype_T compile_type)
{
    بدل (ufunc->uf_def_حالت)
    {
	حالة UF_منزل_BE_COMPILED:
	    أرجع TRUE;

	حالة UF_COMPILED:
	{
	    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_معلومات)
							 + ufunc->uf_dfunc_idx;

	    بدل (compile_type)
	    {
		حالة CT_PROFILE:
#جيڪڏهن_وصف_ٿيل FEAT_PROFILE
		    أرجع dfunc->df_instr_prof == عدم;
#جيڪڏهن_پورو
		حالة CT_NONE:
		    أرجع dfunc->df_instr == عدم;
		حالة CT_DEBUG:
		    أرجع dfunc->df_instr_debug == عدم;
	    }
	}

	حالة UF_NOT_COMPILED:
	حالة UF_COMPILE_ERROR:
	حالة UF_COMPILING:
	    اكسر;
    }
    أرجع FALSE;
}

/*
 * Compile a nested :def command.
 */
    ساكن char_u *
compile_nested_function(exarg_T *eap, cctx_T *cctx, garray_T *سٽون_منزل_free)
{
    صحيح		is_global = *eap->arg == 'g' && eap->arg[1] == ':';
    char_u	*name_شروعات = eap->arg;
    char_u	*name_پڇاڙي = منزل_name_پڇاڙي(eap->arg, TRUE);
    صحيح		off;
    char_u	*func_name;
    string_T	lambda_name;
    ufunc_T	*ufunc;
    صحيح		r = FAIL;
    compiletype_T   compile_type;
    صحيح		funcref_isn_idx = -1;
    lvar_T	*lvar = عدم;
    char_u	*bracket_شروعات = عدم;

    إذا (eap->forceit)
    {
	semsg(_(e_cannot_use_bang_with_nested_def_str),
		eap->cmdidx == CMD_def ? ":def" : ":function");
	أرجع عدم;
    }

    إذا (*name_شروعات == '/')
    {
	name_پڇاڙي = skip_regexp(name_شروعات + 1, '/', TRUE);
	إذا (*name_پڇاڙي == '/')
	    ++name_پڇاڙي;
	سيٽ_nextcmd(eap, name_پڇاڙي);
    }

    إذا (*name_پڇاڙي == '<')
    {
	bracket_شروعات = name_پڇاڙي;
	إذا (skip_generic_func_type_دليلون(&name_پڇاڙي) == FAIL)
	    أرجع عدم;
    }

    إذا (name_پڇاڙي == name_شروعات || *skipwhite(name_پڇاڙي) != '(')
    {
	إذا (!پڇاڙيون_excmd2(name_شروعات, name_پڇاڙي))
	{
	    إذا (*skipwhite(name_پڇاڙي) == '.')
		semsg(_(e_cannot_define_dict_func_اندريون_vim9_script_str),
								     eap->cmd);
	    وإلا
		semsg(_(e_invalid_command_str), eap->cmd);
	    أرجع عدم;
	}

	// "def" or "def Name": list functions
	إذا (generate_DEF(cctx, name_شروعات, name_پڇاڙي - name_شروعات) == FAIL)
	    أرجع عدم;
	أرجع eap->nextcmd == عدم ? (char_u *)"" : eap->nextcmd;
    }

    إذا (bracket_شروعات != عدم)
	// generic function.  The function name ends before the list of types
	// (opening angle bracket).
	name_پڇاڙي = bracket_شروعات;

    // Only g:Func() can use a namespace.
    إذا (name_شروعات[1] == ':' && !is_global)
    {
	semsg(_(e_namespace_not_supported_str), name_شروعات);
	أرجع عدم;
    }
    إذا (cctx->ctx_skip != SKIP_YES
	    && check_defined(name_شروعات, name_پڇاڙي - name_شروعات, cctx,
							  عدم, FALSE) == FAIL)
	أرجع عدم;
    إذا (!ASCII_ISUPPER(is_global ? name_شروعات[2] : name_شروعات[0]))
    {
	semsg(_(e_function_name_must_شروعات_with_capital_str), name_شروعات);
	أرجع عدم;
    }

    eap->arg = name_پڇاڙي;
    fill_exarg_from_cctx(eap, cctx);

    eap->forceit = FALSE;
    // We use the special <lambda>99 name, but it's not really a lambda.
    lambda_name = get_lambda_name();
    lambda_name.string = vim_strnsave(lambda_name.string, lambda_name.ڊيگهائي);
    إذا (lambda_name.string == عدم)
	أرجع عدم;

    // This may free the current line, make a copy of the name.
    off = is_global ? 2 : 0;
    func_name = vim_strnsave(name_شروعات + off, name_پڇاڙي - name_شروعات - off);
    إذا (func_name == عدم)
    {
	r = FAIL;
	اذهب theend;
    }

    // A type variable cannot have the name of the function.  The function is
    // defined with the "<lambda>" name, check it here.
    إذا (bracket_شروعات != عدم)
    {
	size_t	len = STRLEN(func_name);

	لكل (char_u *t = bracket_شروعات + 1; *t != '>' && *t != NUL; )
	{
	    char_u *te = منزل_name_پڇاڙي(t, FALSE);

	    إذا ((size_t)(te - t) == len && STRNCMP(t, func_name, len) == 0)
	    {
		semsg(_(e_redefining_script_شي_str), func_name);
		r = FAIL;
		اذهب theend;
	    }
	    t = te == t ? t + 1 : te;
	}
    }

    // Make sure "KeyTyped" is not set, it may cause indent to be written.
    صحيح save_ڪنجي_Typed = ڪنجي_Typed;
    ڪنجي_Typed = FALSE;

    ufunc = define_function(eap, lambda_name.string, سٽون_منزل_free, 0, عدم, 0,
									cctx);

    ڪنجي_Typed = save_ڪنجي_Typed;

    إذا (ufunc == عدم)
    {
	r = eap->skip ? OK : FAIL;
	اذهب theend;
    }
    إذا (eap->nextcmd != عدم)
    {
	semsg(_(e_متن_مليو_after_str_str),
	      eap->cmdidx == CMD_def ? "enddef" : "endfunction", eap->nextcmd);
	r = FAIL;
	func_ptr_unref(ufunc);
	اذهب theend;
    }

    // copy over the block scope IDs before compiling
    إذا (!is_global && cctx->ctx_ufunc->uf_بلاڪ_اونهائي > 0)
    {
	صحيح بلاڪ_اونهائي = cctx->ctx_ufunc->uf_بلاڪ_اونهائي;

	ufunc->uf_بلاڪ_ids = ALLOC_MULT(صحيح, بلاڪ_اونهائي);
	إذا (ufunc->uf_بلاڪ_ids != عدم)
	{
	    mch_memmove(ufunc->uf_بلاڪ_ids, cctx->ctx_ufunc->uf_بلاڪ_ids,
						    حجم(صحيح) * بلاڪ_اونهائي);
	    ufunc->uf_بلاڪ_اونهائي = بلاڪ_اونهائي;
	}
    }

    // Define the funcref before compiling, so that it is found by any
    // recursive call.
    إذا (is_global)
    {
	r = generate_NEWFUNC(cctx, lambda_name.string, func_name);
	func_name = عدم;
	lambda_name.string = عدم;
	lambda_name.ڊيگهائي = 0;
    }
    وإلا
    {
	// Define a local variable for the function reference.
	lvar = reserve_local(cctx, func_name, name_پڇاڙي - name_شروعات,
					    سونپيو_CONST, ufunc->uf_func_type);
	إذا (lvar == عدم)
	{
	    func_ptr_unref(ufunc);
	    اذهب theend;
	}
	lvar->lv_ufunc = ufunc;
	إذا (generate_FUNCREF(cctx, ufunc, عدم, FALSE, 0, &funcref_isn_idx) == FAIL)
	{
	    func_ptr_unref(ufunc);
	    اذهب theend;
	}
	r = generate_STORE(cctx, ISN_STORE, lvar->lv_idx, عدم);
    }

    compile_type = get_compile_type(ufunc);
#جيڪڏهن_وصف_ٿيل FEAT_PROFILE
    // If the outer function is profiled, also compile the nested function for
    // profiling.
    إذا (cctx->ctx_compile_type == CT_PROFILE)
	compile_type = CT_PROFILE;
#جيڪڏهن_پورو
    إذا (func_needs_compiling(ufunc, compile_type)
	    && compile_def_function(ufunc, TRUE, compile_type, cctx) == FAIL)
    {
	func_ptr_unref(ufunc);
	إذا (lvar != عدم)
	    // Now the local variable can't be used.
	    *lvar->lv_name = '/';  // impossible value
	اذهب theend;
    }

#جيڪڏهن_وصف_ٿيل FEAT_PROFILE
    // When the outer function is compiled for profiling, the nested function
    // may be called without profiling.  Compile it here in the right context.
    إذا (compile_type == CT_PROFILE && func_needs_compiling(ufunc, CT_NONE))
	compile_def_function(ufunc, FALSE, CT_NONE, cctx);
#جيڪڏهن_پورو

    // If a FUNCREF instruction was generated, set the index after compiling.
    إذا (funcref_isn_idx != -1 && ufunc->uf_def_حالت == UF_COMPILED)
    {
	isn_T	*funcref_isn = ((isn_T *)cctx->ctx_instr.ga_معلومات) +
							funcref_isn_idx;
	funcref_isn->isn_arg.funcref.fr_dfunc_idx = ufunc->uf_dfunc_idx;
    }

theend:
    vim_free(lambda_name.string);
    vim_free(func_name);
    أرجع r == FAIL ? عدم : (char_u *)"";
}

/*
 * Compile one Vim expression {expr} in string "p".
 * "p" points to the opening "{".
 * Return a pointer to the character after "}", NULL for an error.
 */
    char_u *
compile_one_اظهار_اندريون_str(char_u *p, cctx_T *cctx)
{
    char_u	*بلاڪ_شروعات;
    char_u	*بلاڪ_پڇاڙي;

    // Skip the opening {.
    بلاڪ_شروعات = skipwhite(p + 1);
    بلاڪ_پڇاڙي = بلاڪ_شروعات;
    إذا (*بلاڪ_شروعات != NUL && skip_اظهار(&بلاڪ_پڇاڙي, عدم) == FAIL)
	أرجع عدم;
    بلاڪ_پڇاڙي = skipwhite(بلاڪ_پڇاڙي);
    // The block must be closed by a }.
    إذا (*بلاڪ_پڇاڙي != '}')
    {
	semsg(_(e_missing_close_curly_str), p);
	أرجع عدم;
    }
    إذا (compile_expr0(&بلاڪ_شروعات, cctx) == FAIL)
	أرجع عدم;
    may_generate_2STRING(-1, TOSTRING_INTERPOLATE, cctx);

    أرجع بلاڪ_پڇاڙي + 1;
}

/*
 * Compile a string "str" (either containing a literal string or a mix of
 * literal strings and Vim expressions of the form `{expr}`).  This is used
 * when compiling a heredoc assignment to a variable or an interpolated string
 * in a Vim9 def function.  Vim9 instructions are generated to push strings,
 * evaluate expressions, concatenate them and create a list of lines.  When
 * "evalstr" is TRUE, Vim expressions in "str" are evaluated.
 */
    صحيح
compile_all_اظهار_اندريون_str(char_u *str, صحيح evalstr, cctx_T *cctx)
{
    char_u	*p = str;
    char_u	*val;
    صحيح		ڳڻپ = 0;

    إذا (cctx->ctx_skip == SKIP_YES)
	أرجع OK;

    إذا (!evalstr || *str == NUL)
    {
	// Literal string, possibly empty.
	val = *str != NUL ? vim_strsave(str) : عدم;
	أرجع generate_PUSHS(cctx, &val);
    }

    // Push all the string pieces to the stack, followed by a ISN_CONCAT.
    طالما (*p != NUL)
    {
	char_u	*lit_شروعات;
	صحيح	escaped_brace = FALSE;

	// Look for a block start.
	lit_شروعات = p;
	طالما (*p != '{' && *p != '}' && *p != NUL)
	    ++p;

	إذا (*p != NUL && *p == p[1])
	{
	    // Escaped brace, unescape and continue.
	    // Include the brace in the literal string.
	    ++p;
	    escaped_brace = TRUE;
	}
	وإلا إذا (*p == '}')
	{
	    semsg(_(e_stray_closing_curly_str), str);
	    أرجع FAIL;
	}

	// Append the literal part.
	إذا (p != lit_شروعات)
	{
	    val = vim_strnsave(lit_شروعات, (size_t)(p - lit_شروعات));
	    إذا (generate_PUSHS(cctx, &val) == FAIL)
		أرجع FAIL;
	    ++ڳڻپ;
	}

	إذا (*p == NUL)
	    اكسر;

	إذا (escaped_brace)
	{
	    // Skip the second brace.
	    ++p;
	    استمر;
	}

	p = compile_one_اظهار_اندريون_str(p, cctx);
	إذا (p == عدم)
	    أرجع FAIL;
	++ڳڻپ;
    }

    // Small optimization, if there's only a single piece skip the ISN_CONCAT.
    إذا (ڳڻپ > 1)
	أرجع generate_CONCAT(cctx, ڳڻپ);

    أرجع OK;
}

/*
 * Return the length of an assignment operator, or zero if there isn't one.
 */
    صحيح
assignment_len(char_u *p, صحيح *heredoc)
{
    إذا (*p == '=')
    {
	إذا (p[1] == '<' && p[2] == '<')
	{
	    *heredoc = TRUE;
	    أرجع 3;
	}
	أرجع 1;
    }
    إذا (vim_strchr((char_u *)"+-*/%", *p) != عدم && p[1] == '=')
	أرجع 2;
    إذا (STRNCMP(p, "..=", 3) == 0)
	أرجع 3;
    أرجع 0;
}

/*
 * Generate the load instruction for "name".
 */
    ساكن صحيح
generate_loadvar(cctx_T *cctx, lhs_T *lhs)
{
    char_u	*name = lhs->lhs_name;
    type_T	*type = lhs->lhs_type;
    صحيح		res = OK;

    بدل (lhs->lhs_dest)
    {
	حالة dest_option:
	حالة dest_func_option:
	    generate_ڀريو(cctx, ISN_LOADOPT, 0, name, type);
	    اكسر;
	حالة dest_global:
	    إذا (vim_strchr(name, AUTOLOAD_CHAR) == عدم)
	    {
		إذا (name[2] == NUL)
		    generate_instr_type(cctx, ISN_LOADGDICT, &t_dict_any);
		وإلا
		    generate_ڀريو(cctx, ISN_LOADG, 0, name + 2, type);
	    }
	    وإلا
		generate_ڀريو(cctx, ISN_LOADAUTO, 0, name, type);
	    اكسر;
	حالة dest_بفر:
	    generate_ڀريو(cctx, ISN_LOADB, 0, name + 2, type);
	    اكسر;
	حالة dest_دري:
	    generate_ڀريو(cctx, ISN_LOADW, 0, name + 2, type);
	    اكسر;
	حالة dest_tab:
	    generate_ڀريو(cctx, ISN_LOADT, 0, name + 2, type);
	    اكسر;
	حالة dest_script:
	حالة dest_script_v9:
	    res = compile_ڀريو_scriptvar(cctx,
			    name + (name[1] == ':' ? 2 : 0), عدم, عدم, عدم);
	    اكسر;
	حالة dest_env:
	    // Include $ in the name here
	    generate_ڀريو(cctx, ISN_LOADENV, 0, name, type);
	    اكسر;
	حالة dest_reg:
	    generate_ڀريو(cctx, ISN_LOADREG, name[1], عدم, &t_string);
	    اكسر;
	حالة dest_vimvar:
	    generate_LOADV(cctx, name + 2);
	    اكسر;
	حالة dest_local:
	    إذا (cctx->ctx_skip != SKIP_YES)
	    {
		lvar_T	*lvar = lhs->lhs_lvar;
		إذا (lvar->lv_from_outer > 0)
		    generate_LOADOUTER(cctx, lvar->lv_idx, lvar->lv_from_outer,
				 lvar->lv_loop_اونهائي, lvar->lv_loop_idx, type);
		وإلا
		    generate_ڀريو(cctx, ISN_ڀريو, lvar->lv_idx, عدم, type);
	    }
	    اكسر;
	حالة dest_class_member:
	    generate_CLASSMEMBER(cctx, TRUE, lhs->lhs_class,
						     lhs->lhs_classmember_idx);
	    اكسر;
	حالة dest_اظهار:
	    // list or dict value should already be on the stack.
	    اكسر;
    }

    أرجع res;
}

/*
 * Skip over "[expr]" or ".member".
 * Does not check for any errors.
 */
    ساكن char_u *
skip_اشاريو(char_u *شروعات)
{
    char_u *p = شروعات;

    إذا (*p == '[')
    {
	p = skipwhite(p + 1);
	(فراغ)skip_اظهار(&p, عدم);
	p = skipwhite(p);
	إذا (*p == ']')
	    أرجع p + 1;
	أرجع p;
    }
    // if (*p == '.')
    أرجع منزل_name_پڇاڙي(p + 1, TRUE);
}

    فراغ
vim9_declare_error(char_u *name)
{
    حرف *scope = "";

    بدل (*name)
    {
	حالة 'g': scope = _("global"); اكسر;
	حالة 'b': scope = _("buffer"); اكسر;
	حالة 'w': scope = _("window"); اكسر;
	حالة 't': scope = _("tab"); اكسر;
	حالة 'v': scope = "v:"; اكسر;
	حالة '$': semsg(_(e_cannot_declare_an_environment_variable_str), name);
		  أرجع;
	حالة '&': semsg(_(e_cannot_declare_an_option_str), name);
		  أرجع;
	حالة '@': semsg(_(e_cannot_declare_a_رجسٽر_str), name);
		  أرجع;
	افتراضي: أرجع;
    }
    semsg(_(e_cannot_declare_a_scope_variable_str), scope, name);
}

/*
 * Return TRUE if "name" is a valid register to use.
 * Return FALSE and give an error message if not.
 */
    ساكن صحيح
valid_dest_reg(صحيح name)
{
    إذا (name == '@')
       name = '"';
    إذا (name == '/' || name == '=' || name == '#' || valid_yank_reg(name, TRUE))
	أرجع TRUE;
    emsg_invreg(name);
    أرجع FAIL;
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
    صحيح
get_var_dest(
	char_u		*name,
	سونپيو_dest_T	*dest,
	cmdidx_T	cmdidx,
	صحيح		has_اشاريو,
	صحيح		*option_scope,
	صحيح		*vimvaridx,
	type_T		**type,
	cctx_T		*cctx)
{
    char_u *p;

    إذا (*name == '&')
    {
	صحيح		cc;
	ڊگهو		numval;
	getoption_T	opt_type;
	صحيح		opt_p_flags;

	*dest = dest_option;
	إذا (cmdidx == CMD_final || cmdidx == CMD_const)
	{
	    emsg(_(e_cannot_تالو_option));
	    أرجع FAIL;
	}
	p = name;
	p = ڳوليو_option_پڇاڙي(&p, option_scope);
	إذا (p == عدم)
	{
	    // cannot happen?
	    emsg(_(e_unexpected_characters_اندريون_assignment));
	    أرجع FAIL;
	}
	cc = *p;
	*p = NUL;
	opt_type = get_option_قدر(skip_option_env_lead(name),
				   &numval, عدم, &opt_p_flags, *option_scope);
	*p = cc;
	بدل (opt_type)
	{
	    حالة gov_unknown:
		    semsg(_(e_unknown_option_str), name);
		    أرجع FAIL;
	    حالة gov_string:
	    حالة gov_hidden_string:
		    إذا (opt_p_flags & P_FUNC)
		    {
			// might be a Funcref, check the type later
			*type = &t_any;
			*dest = dest_func_option;
		    }
		    وإلا
		    {
			*type = &t_string;
		    }
		    اكسر;
	    حالة gov_bool:
	    حالة gov_hidden_bool:
		    *type = &t_bool;
		    اكسر;
	    حالة gov_عدد:
	    حالة gov_hidden_عدد:
		    *type = &t_عدد;
		    اكسر;
	}
    }
    وإلا إذا (*name == '$')
    {
	*dest = dest_env;
	*type = &t_string;
    }
    وإلا إذا (*name == '@')
    {
	إذا (!valid_dest_reg(name[1]))
	    أرجع FAIL;
	*dest = dest_reg;
	*type = name[1] == '#' ? &t_عدد_or_string : &t_string;
    }
    وإلا إذا (STRNCMP(name, "g:", 2) == 0)
    {
	*dest = dest_global;
    }
    وإلا إذا (STRNCMP(name, "b:", 2) == 0)
    {
	*dest = dest_بفر;
    }
    وإلا إذا (STRNCMP(name, "w:", 2) == 0)
    {
	*dest = dest_دري;
    }
    وإلا إذا (STRNCMP(name, "t:", 2) == 0)
    {
	*dest = dest_tab;
    }
    وإلا إذا (STRNCMP(name, "v:", 2) == 0)
    {
	typval_T	*vtv;
	صحيح		di_flags;

	*vimvaridx = ڳوليو_vim_var(name + 2, &di_flags);
	إذا (*vimvaridx < 0)
	{
	    semsg(_(e_variable_not_مليو_str), name);
	    أرجع FAIL;
	}
	// An item of a read-only variable can be changed, as when not compiled.
	// We use the current value of "sandbox" here, is that OK?
	إذا (!has_اشاريو && var_check_ro(di_flags, name, FALSE))
	    أرجع FAIL;
	*dest = dest_vimvar;
	vtv = get_vim_var_tv(*vimvaridx);
	*type = typval2type_vimvar(vtv, cctx->ctx_type_list);
    }
    أرجع OK;
}

    ساكن صحيح
is_decl_command(cmdidx_T cmdidx)
{
    أرجع cmdidx == CMD_let || cmdidx == CMD_var
				 || cmdidx == CMD_final || cmdidx == CMD_const;
}

/*
 * Returns TRUE if the class or object variable in "lhs" is modifiable.
 * "var_start" points to the start of the variable name and "lhs->lhs_varlen"
 * has the total length.  Note that the "lhs" can be nested an object reference
 * (e.g.  a.b.c.d.var).
 */
    ساكن صحيح
lhs_class_member_modifiable(lhs_T *lhs, char_u	*var_شروعات, cctx_T *cctx)
{
    size_t	varlen = lhs->lhs_varlen;
    class_T	*cl = lhs->lhs_type->tt_class;
    صحيح		is_object = lhs->lhs_type->tt_type == VAR_OBJECT;
    char_u	*name = var_شروعات + varlen + 1;
    size_t	namelen = lhs->lhs_پڇاڙي - var_شروعات - varlen - 1;
    ocmember_T	*m;

    m = member_lookup(cl, lhs->lhs_type->tt_type, name, namelen, عدم);
    إذا (m == عدم)
    {
	member_not_مليو_نياپو(cl, lhs->lhs_type->tt_type, name, namelen);
	أرجع FALSE;
    }

    إذا (IS_ENUM(cl))
    {
	semsg(_(e_enumvalue_str_cannot_be_modified), cl->class_name.string,
		m->ocm_name.string);
	أرجع FALSE;
    }

    // If it is private member variable, then accessing it outside the
    // class is not allowed.
    // If it is a read only class variable, then it can be modified
    // only inside the class where it is defined.
    إذا ((m->ocm_access != VIM_ACCESS_ALL) &&
	    ((is_object && !inside_class(cctx, cl))
	     || (!is_object && cctx->ctx_ufunc->uf_class != cl)))
    {
	حرف *نياپو = (m->ocm_access == VIM_ACCESS_ذاتي)
				? e_cannot_access_protected_variable_str
				: e_variable_is_not_writable_str;
	emsg_var_cl_define(نياپو, m->ocm_name.string, 0, cl);
	أرجع FALSE;
    }

    أرجع TRUE;
}

/*
 * Initialize "lhs" with default values
 */
    ساكن فراغ
lhs_init_defaults(lhs_T *lhs)
{
    CLEAR_اشارو(lhs);
    lhs->lhs_dest = dest_local;
    lhs->lhs_vimvaridx = -1;
    lhs->lhs_scriptvar_idx = -1;
    lhs->lhs_member_idx = -1;
}

/*
 * When compiling a LHS variable name, find the end of the destination and the
 * end of the variable name.
 */
    ساكن صحيح
lhs_ڳوليو_var_پڇاڙي(
    lhs_T	*lhs,
    char_u	*var_شروعات,
    صحيح		is_decl,
    char_u	**var_endp)
{
    char_u  *var_پڇاڙي = *var_endp;

    // "lhs_dest_end" is the end of the destination, including "[expr]" or
    // ".name".
    // "var_end" is the end of the variable/option/etc. name.
    lhs->lhs_dest_پڇاڙي = skip_var_one(var_شروعات, FALSE);
    إذا (*var_شروعات == '@')
    {
	إذا (!valid_dest_reg(var_شروعات[1]))
	    أرجع FAIL;
	var_پڇاڙي = var_شروعات + 2;
    }
    وإلا
    {
	// skip over the leading "&", "&l:", "&g:" and "$"
	var_پڇاڙي = skip_option_env_lead(var_شروعات);
	var_پڇاڙي = منزل_name_پڇاڙي(var_پڇاڙي, TRUE);
    }

    // "a: type" is declaring variable "a" with a type, not dict "a:".
    إذا (is_decl && lhs->lhs_dest_پڇاڙي == var_شروعات + 2
					&& lhs->lhs_dest_پڇاڙي[-1] == ':')
	--lhs->lhs_dest_پڇاڙي;
    إذا (is_decl && var_پڇاڙي == var_شروعات + 2 && var_پڇاڙي[-1] == ':')
	--var_پڇاڙي;

    lhs->lhs_پڇاڙي = lhs->lhs_dest_پڇاڙي;
    *var_endp = var_پڇاڙي;

    أرجع OK;
}

/*
 * Set various fields in "lhs"
 */
    ساكن صحيح
lhs_init(
    lhs_T	*lhs,
    char_u	*var_شروعات,
    صحيح		is_decl,
    صحيح		heredoc,
    char_u	**var_endp)
{
    char_u *var_پڇاڙي = *var_endp;

    lhs_init_defaults(lhs);

    // Find the end of the variable and the destination
    إذا (lhs_ڳوليو_var_پڇاڙي(lhs, var_شروعات, is_decl, &var_پڇاڙي) == FAIL)
	أرجع FAIL;

    // compute the length of the destination without "[expr]" or ".name"
    lhs->lhs_varlen = var_پڇاڙي - var_شروعات;
    lhs->lhs_varlen_ڪل = lhs->lhs_varlen;
    lhs->lhs_name = vim_strnsave(var_شروعات, lhs->lhs_varlen);
    إذا (lhs->lhs_name == عدم)
	أرجع FAIL;

    إذا (lhs->lhs_dest_پڇاڙي > var_شروعات + lhs->lhs_varlen)
	// Something follows after the variable: "var[idx]" or "var.key".
	lhs->lhs_has_اشاريو = TRUE;

    lhs->lhs_type = heredoc ? &t_list_string : &t_any;

    *var_endp = var_پڇاڙي;

    أرجع OK;
}

/*
 * Compile a LHS class variable name.
 */
    ساكن صحيح
compile_lhs_class_variable(
    cctx_T	*cctx,
    lhs_T	*lhs,
    class_T	*defcl,
    صحيح		is_decl)
{
    إذا (cctx->ctx_ufunc->uf_defclass != defcl)
    {
	// A class variable can be accessed without the class name
	// only inside a class.
	semsg(_(e_class_variable_str_accessible_only_inside_class_str),
		lhs->lhs_name, defcl->class_name.string);
	أرجع FAIL;
    }

    إذا (is_decl)
    {
	semsg(_(e_variable_already_declared_اندريون_class_str), lhs->lhs_name);
	أرجع FAIL;
    }

    ocmember_T	*m = &defcl->class_class_members[lhs->lhs_classmember_idx];
    إذا (oc_var_check_ro(defcl, m))
	أرجع FAIL;

    lhs->lhs_dest = dest_class_member;
    // The class variable is defined either in the current class or
    // in one of the parent class in the hierarchy.
    lhs->lhs_class = defcl;
    lhs->lhs_type = oc_member_type_by_idx(defcl, FALSE,
						lhs->lhs_classmember_idx);

    أرجع OK;
}

/*
 * Compile an imported LHS variable
 */
    ساكن صحيح
compile_lhs_import_var(
    lhs_T	*lhs,
    imported_T	*import,
    char_u	*var_شروعات,
    char_u	**var_endp,
    char_u	**rawnamep)
{
    char_u	*var_پڇاڙي = *var_endp;
    char_u	*dot = vim_strchr(var_شروعات, '.');
    char_u	*p;

    // for an import the name is what comes after the dot
    إذا (dot == عدم)
    {
	semsg(_(e_no_dot_after_imported_name_str), var_شروعات);
	أرجع FAIL;
    }

    p = skipwhite(dot + 1);
    var_پڇاڙي = منزل_name_پڇاڙي(p, TRUE);
    إذا (var_پڇاڙي == p)
    {
	semsg(_(e_missing_name_after_imported_name_str), var_شروعات);
	أرجع FAIL;
    }

    vim_free(lhs->lhs_name);
    lhs->lhs_varlen = var_پڇاڙي - p;
    lhs->lhs_name = vim_strnsave(p, lhs->lhs_varlen);
    إذا (lhs->lhs_name == عدم)
	أرجع FAIL;
    *rawnamep = lhs->lhs_name;
    lhs->lhs_scriptvar_sid = import->imp_sid;

    // TODO: where do we check this name is exported?

    // Check if something follows: "exp.var[idx]" or
    // "exp.var.key".
    lhs->lhs_has_اشاريو = lhs->lhs_dest_پڇاڙي > skipwhite(var_پڇاڙي);

    *var_endp = var_پڇاڙي;

    أرجع OK;
}

/*
 * Process a script-local variable when compiling a LHS variable name.
 */
    ساكن صحيح
compile_lhs_script_var(
    cctx_T	*cctx,
    lhs_T	*lhs,
    char_u	*var_شروعات,
    char_u	*var_پڇاڙي,
    صحيح		is_decl)
{
    صحيح		script_namespace = FALSE;
    صحيح		script_var = FALSE;
    imported_T	*import;
    char_u	*var_name;
    size_t	var_name_len;

    إذا (lhs->lhs_varlen > 1 && STRNCMP(var_شروعات, "s:", 2) == 0)
	script_namespace = TRUE;

    إذا (script_namespace)
    {
	var_name = var_شروعات + 2;
	var_name_len = lhs->lhs_varlen - 2;
    }
    وإلا
    {
	var_name = var_شروعات;
	var_name_len = lhs->lhs_varlen;
    }

    إذا (script_var_exists(var_name, var_name_len, cctx, عدم) == OK)
	script_var = TRUE;

    import = ڳوليو_imported(var_شروعات, lhs->lhs_varlen, FALSE);

    إذا (script_namespace || script_var || import != عدم)
    {
	char_u *rawname = lhs->lhs_name + (lhs->lhs_name[1] == ':' ? 2 : 0);

	إذا (script_namespace && current_script_is_vim9())
	{
	    semsg(_(e_cannot_use_s_colon_اندريون_vim9_script_str), var_شروعات);
	    أرجع FAIL;
	}

	إذا (is_decl)
	{
	    إذا (script_namespace)
		semsg(_(e_cannot_declare_script_variable_اندريون_function_str),
			lhs->lhs_name);
	    وإلا
		semsg(_(e_variable_already_declared_اندريون_script_str),
			lhs->lhs_name);
	    أرجع FAIL;
	}
	وإلا إذا (cctx->ctx_ufunc->uf_script_ctx_نسخو == SCRIPT_نسخو_VIM9
		&& script_namespace
		&& !script_var && import == عدم)
	{
	    semsg(_(e_unknown_variable_str), lhs->lhs_name);
	    أرجع FAIL;
	}

	lhs->lhs_dest = current_script_is_vim9() ? dest_script_v9 :
								dest_script;

	// existing script-local variables should have a type
	lhs->lhs_scriptvar_sid = current_sctx.sc_sid;
	إذا (import != عدم)
	{
	    إذا (compile_lhs_import_var(lhs, import, var_شروعات, &var_پڇاڙي,
							&rawname) == FAIL)
		أرجع FAIL;
	}

	إذا (SCRIPT_ID_VALID(lhs->lhs_scriptvar_sid))
	{
	    // Check writable only when no index follows.
	    lhs->lhs_scriptvar_idx = get_script_شي_idx(
					lhs->lhs_scriptvar_sid, rawname,
					lhs->lhs_has_اشاريو ?  سونپيو_FINAL :
					سونپيو_CONST, cctx, عدم);
	    إذا (lhs->lhs_scriptvar_idx >= 0)
	    {
		scriptitem_T *si = SCRIPT_شي(lhs->lhs_scriptvar_sid);
		svar_T	 *sv = ((svar_T *)si->sn_var_vals.ga_معلومات)
						+ lhs->lhs_scriptvar_idx;

		lhs->lhs_type = sv->sv_type;
	    }
	}

	أرجع OK;
    }

    أرجع check_defined(var_شروعات, lhs->lhs_varlen, cctx, عدم, FALSE);
}

/*
 * Compile the LHS destination.
 */
    ساكن صحيح
compile_lhs_var_dest(
    cctx_T	*cctx,
    lhs_T	*lhs,
    صحيح		cmdidx,
    char_u	*var_شروعات,
    char_u	*var_پڇاڙي,
    صحيح		is_decl,
    صحيح		has_cmd)	// "var" before "var_start"
{
    صحيح	    declare_error = FALSE;

    إذا (get_var_dest(lhs->lhs_name, &lhs->lhs_dest, cmdidx,
				lhs->lhs_has_اشاريو,
				&lhs->lhs_opt_flags, &lhs->lhs_vimvaridx,
				&lhs->lhs_type, cctx) == FAIL)
	أرجع FAIL;

    إذا (lhs->lhs_dest != dest_local && cmdidx != CMD_const
						&& cmdidx != CMD_final)
    {
	// Specific kind of variable recognized.
	declare_error = is_decl;
    }
    وإلا
    {
	class_T	*defcl;

	// No specific kind of variable recognized, just a name.
	إذا (check_reserved_name(lhs->lhs_name, lhs->lhs_has_اشاريو
						&& *var_پڇاڙي == '.') == FAIL)
	    أرجع FAIL;

	إذا (lookup_local(var_شروعات, lhs->lhs_varlen, &lhs->lhs_local_lvar,
								cctx) == OK)
	{
	    lhs->lhs_lvar = &lhs->lhs_local_lvar;
	}
	وإلا
	{
	    CLEAR_ميدان(lhs->lhs_arg_lvar);
	    إذا (arg_exists(var_شروعات, lhs->lhs_varlen,
			&lhs->lhs_arg_lvar.lv_idx, &lhs->lhs_arg_lvar.lv_type,
			&lhs->lhs_arg_lvar.lv_from_outer, cctx) == OK)
	    {
		إذا (is_decl)
		{
		    semsg(_(e_str_is_استعمال_ٿيل_as_دليل), lhs->lhs_name);
		    أرجع FAIL;
		}
		lhs->lhs_lvar = &lhs->lhs_arg_lvar;
	    }
	}

	إذا (lhs->lhs_lvar != عدم)
	{
	    إذا (is_decl)
	    {
		// if we come here with what looks like an assignment like
		// .= but which has been rejected by assignment_len() from
		// may_compile_assignment give a better error message
		char_u *p = skipwhite(lhs->lhs_پڇاڙي);
		إذا (p[0] == '.' && p[1] == '=')
		    emsg(_(e_dot_equal_not_supported_with_script_نسخو_two));
		وإلا إذا (p[0] == ':' && !has_cmd)
		    // type specified in an assignment without "var"
		    semsg(_(e_trailing_characters_str), p);
		وإلا
		    semsg(_(e_variable_already_declared_str), lhs->lhs_name);
		أرجع FAIL;
	    }
	}
	وإلا إذا ((lhs->lhs_classmember_idx = cctx_class_member_idx(
			cctx, var_شروعات, lhs->lhs_varlen, &defcl)) >= 0)
	{
	    إذا (compile_lhs_class_variable(cctx, lhs, defcl, is_decl)
		    == FAIL)
		أرجع FAIL;
	}
	وإلا
	{
	    إذا (compile_lhs_script_var(cctx, lhs, var_شروعات, var_پڇاڙي,
			is_decl) == FAIL)
		أرجع FAIL;
	}
    }

    إذا (declare_error)
    {
	vim9_declare_error(lhs->lhs_name);
	أرجع FAIL;
    }

    أرجع OK;
}

/*
 * When compiling a LHS variable name, for a class or an object, set the LHS
 * member type.
 */
    ساكن صحيح
compile_lhs_سيٽ_oc_member_type(
    cctx_T	*cctx,
    lhs_T	*lhs,
    char_u	*var_شروعات)
{
    class_T	*cl = lhs->lhs_type->tt_class;
    صحيح		is_object = lhs->lhs_type->tt_type == VAR_OBJECT;
    char_u	*name = var_شروعات + lhs->lhs_varlen + 1;
    size_t	namelen = lhs->lhs_پڇاڙي - var_شروعات - lhs->lhs_varlen - 1;

    ocmember_T	*m = member_lookup(cl, lhs->lhs_type->tt_type,
	    name, namelen, &lhs->lhs_member_idx);
    إذا (m == عدم)
    {
	member_not_مليو_نياپو(cl, lhs->lhs_type->tt_type, name, namelen);
	أرجع FAIL;
    }

    إذا (IS_ENUM(cl))
    {
	إذا (!inside_class(cctx, cl))
	{
	    semsg(_(e_enumvalue_str_cannot_be_modified),
		    cl->class_name.string, m->ocm_name.string);
	    أرجع FAIL;
	}
	إذا (lhs->lhs_type->tt_type == VAR_OBJECT &&
		lhs->lhs_member_idx < 2)
	{
	    حرف *نياپو = lhs->lhs_member_idx == 0 ?
		e_enum_str_name_cannot_be_modified :
		e_enum_str_ordinal_cannot_be_modified;
	    semsg(_(نياپو), cl->class_name.string);
	    أرجع FAIL;
	}
    }

    // If it is private member variable, then accessing it outside the
    // class is not allowed.
    // If it is a read only class variable, then it can be modified
    // only inside the class where it is defined.
    إذا ((m->ocm_access != VIM_ACCESS_ALL) &&
	    ((is_object && !inside_class(cctx, cl))
	     || (!is_object && cctx->ctx_ufunc->uf_defclass != cl)))
    {
	حرف *نياپو = (m->ocm_access == VIM_ACCESS_ذاتي)
	    ? e_cannot_access_protected_variable_str
	    : e_variable_is_not_writable_str;
	emsg_var_cl_define(نياپو, m->ocm_name.string, 0, cl);
	أرجع FAIL;
    }

    إذا (!IS_CONSTRUCTOR_طريقو(cctx->ctx_ufunc)
	    && oc_var_check_ro(cl, m))
	أرجع FAIL;

    lhs->lhs_member_type = m->ocm_type;

    أرجع OK;
}

/*
 * When compiling a LHS variable, set the LHS variable type.
 */
    ساكن صحيح
compile_lhs_سيٽ_type(cctx_T *cctx, lhs_T *lhs, char_u *var_پڇاڙي, صحيح is_decl)
{
    إذا (is_decl && *skipwhite(var_پڇاڙي) == ':')
    {
	char_u *p;

	// parse optional type: "let var: type = expr"
	إذا (VIM_ISWHITE(*var_پڇاڙي))
	{
	    semsg(_(e_no_white_space_allowed_before_colon_str), var_پڇاڙي);
	    أرجع FAIL;
	}

	إذا (!VIM_ISWHITE(var_پڇاڙي[1]))
	{
	    semsg(_(e_white_space_required_after_str_str), ":", var_پڇاڙي);
	    أرجع FAIL;
	}

	p = skipwhite(var_پڇاڙي + 1);
	lhs->lhs_type = parse_type(&p, cctx->ctx_type_list, cctx->ctx_ufunc, cctx, TRUE);
	إذا (lhs->lhs_type == عدم
		|| !valid_declaration_type(lhs->lhs_type))
	    أرجع FAIL;

	lhs->lhs_has_type = TRUE;
	lhs->lhs_پڇاڙي = p;
    }
    وإلا إذا (lhs->lhs_lvar != عدم)
	lhs->lhs_type = lhs->lhs_lvar->lv_type;

    أرجع OK;
}

/*
 * Returns TRUE if "lhs" is a concatenable string.
 */
    ساكن صحيح
lhs_concatenable(lhs_T *lhs)
{
    أرجع lhs->lhs_dest == dest_global
		|| lhs->lhs_has_اشاريو
		|| lhs->lhs_type->tt_type == VAR_STRING
		|| lhs->lhs_type->tt_type == VAR_ANY;
}

/*
 * Create a new local variable when compiling a LHS variable.
 */
    ساكن صحيح
compile_lhs_new_local_var(
    cctx_T	*cctx,
    lhs_T	*lhs,
    char_u	*var_شروعات,
    صحيح		cmdidx,
    صحيح		oplen,
    صحيح		is_decl,
    صحيح		has_cmd,
    صحيح		heredoc)
{
    إذا (oplen > 1 && !heredoc)
    {
	// +=, /=, etc. require an existing variable
	semsg(_(e_cannot_use_operator_on_new_variable_str), lhs->lhs_name);
	أرجع FAIL;
    }

    إذا (!is_decl || (lhs->lhs_has_اشاريو && !has_cmd
					&& cctx->ctx_skip != SKIP_YES))
    {
	semsg(_(e_unknown_variable_str), lhs->lhs_name);
	أرجع FAIL;
    }

    // Check the name is valid for a funcref.
    إذا (lhs->lhs_type->tt_type == VAR_FUNC
				|| lhs->lhs_type->tt_type == VAR_PARTIAL)
    {
	إذا (var_wrong_func_name(lhs->lhs_name, TRUE))
	    أرجع FAIL;
    }

    // New local variable.
    صحيح سونپيو;
    بدل (cmdidx)
    {
	حالة CMD_final:
	    سونپيو = سونپيو_FINAL; اكسر;
	حالة CMD_const:
	    سونپيو = سونپيو_CONST; اكسر;
	افتراضي:
	    سونپيو = سونپيو_VAR; اكسر;
    }

    lhs->lhs_lvar = reserve_local(cctx, var_شروعات, lhs->lhs_varlen, سونپيو,
							lhs->lhs_type);
    إذا (lhs->lhs_lvar == عدم)
	أرجع FAIL;

    lhs->lhs_new_local = TRUE;

    أرجع OK;
}

/*
 * When compiling a LHS variable name, set the LHS member type.
 */
    ساكن صحيح
compile_lhs_سيٽ_member_type(
    cctx_T	*cctx,
    lhs_T	*lhs,
    char_u	*var_شروعات,
    صحيح		is_decl,
    صحيح		has_cmd)
{
    lhs->lhs_member_type = lhs->lhs_type;

    إذا (!lhs->lhs_has_اشاريو)
	أرجع OK;

    char_u	*after = var_شروعات + lhs->lhs_varlen;
    char_u	*p;

    // Something follows after the variable: "var[idx]" or "var.key".
    إذا (is_decl && cctx->ctx_skip != SKIP_YES)
    {
	إذا (has_cmd)
	    emsg(_(e_cannot_use_اشاريو_when_declaring_variable));
	وإلا
	    semsg(_(e_unknown_variable_str), lhs->lhs_name);
	أرجع FAIL;
    }

    // Now: var_start[lhs->lhs_varlen] is '[' or '.'
    // Only the last index is used below, if there are others
    // before it generate code for the expression.  Thus for
    // "ll[1][2]" the expression is "ll[1]" and "[2]" is the index.
    لكل (;;)
    {
	p = skip_اشاريو(after);
	إذا (*p != '[' && *p != '.')
	{
	    lhs->lhs_varlen_ڪل = p - var_شروعات;
	    اكسر;
	}
	after = p;
    }
    إذا (after > var_شروعات + lhs->lhs_varlen)
    {
	lhs->lhs_varlen = after - var_شروعات;
	lhs->lhs_dest = dest_اظهار;
	// We don't know the type before evaluating the expression,
	// use "any" until then.  The member index is for the first name,
	// not for the last index.
	lhs->lhs_type = &t_any;
	lhs->lhs_member_idx = -1;
    }

    صحيح use_class = lhs->lhs_type != عدم
	&& (lhs->lhs_type->tt_type == VAR_CLASS
		|| lhs->lhs_type->tt_type == VAR_OBJECT);

    إذا (lhs->lhs_type == عدم
	    || (use_class ? lhs->lhs_type->tt_class == عدم
		: lhs->lhs_type->tt_member == عدم))
    {
	lhs->lhs_member_type = &t_any;
    }
    وإلا إذا (use_class)
    {
	// for an object or class member get the type of the member
	إذا (compile_lhs_سيٽ_oc_member_type(cctx, lhs, var_شروعات) == FAIL)
	    أرجع FAIL;
    }
    وإلا
	lhs->lhs_member_type = lhs->lhs_type->tt_member;

    أرجع OK;
}

/*
 * Figure out the LHS type and other properties for an assignment or one item
 * of ":unlet" with an index.
 * Returns OK or FAIL.
 */
    صحيح
compile_lhs(
	char_u	    *var_شروعات,
	lhs_T	    *lhs,
	cmdidx_T    cmdidx,
	صحيح	    heredoc,
	صحيح	    has_cmd,	    // "var" before "var_start"
	صحيح	    oplen,
	cctx_T	    *cctx)
{
    char_u	*var_پڇاڙي = عدم;
    صحيح		is_decl = is_decl_command(cmdidx);

    إذا (lhs_init(lhs, var_شروعات, is_decl, heredoc, &var_پڇاڙي) == FAIL)
	أرجع FAIL;

    إذا (cctx->ctx_skip != SKIP_YES)
    {
	// compile the LHS destination
	إذا (compile_lhs_var_dest(cctx, lhs, cmdidx, var_شروعات, var_پڇاڙي,
						is_decl, has_cmd) == FAIL)
	    أرجع FAIL;
    }

    // handle "a:name" as a name, not index "name" in "a"
    إذا (lhs->lhs_varlen > 1 || var_شروعات[lhs->lhs_varlen] != ':')
	var_پڇاڙي = lhs->lhs_dest_پڇاڙي;

    إذا (lhs->lhs_dest != dest_option && lhs->lhs_dest != dest_func_option)
    {
	// set the LHS variable type
	إذا (compile_lhs_سيٽ_type(cctx, lhs, var_پڇاڙي, is_decl) == FAIL)
	    أرجع FAIL;
    }

    إذا (oplen == 3 && !heredoc && !lhs_concatenable(lhs))
    {
	emsg(_(e_can_only_concatenate_منزل_string));
	أرجع FAIL;
    }

    إذا (lhs->lhs_lvar == عدم && lhs->lhs_dest == dest_local
						&& cctx->ctx_skip != SKIP_YES)
    {
	إذا (compile_lhs_new_local_var(cctx, lhs, var_شروعات, cmdidx, oplen,
					is_decl, has_cmd, heredoc) == FAIL)
	    أرجع FAIL;
    }

    إذا (compile_lhs_سيٽ_member_type(cctx, lhs, var_شروعات, is_decl, has_cmd)
								== FAIL)
	أرجع FAIL;

    أرجع OK;
}

/*
 * Figure out the LHS and check a few errors.
 */
    صحيح
compile_سونپيو_lhs(
	char_u	    *var_شروعات,
	lhs_T	    *lhs,
	cmdidx_T    cmdidx,
	صحيح	    is_decl,
	صحيح	    heredoc,
	صحيح	    has_cmd,	    // "var" before "var_start"
	صحيح	    oplen,
	cctx_T	    *cctx)
{
    إذا (compile_lhs(var_شروعات, lhs, cmdidx, heredoc, has_cmd, oplen, cctx)
								       == FAIL)
	أرجع FAIL;

    إذا (!lhs->lhs_has_اشاريو && lhs->lhs_lvar == &lhs->lhs_arg_lvar)
    {
	semsg(_(e_cannot_سونپيو_منزل_دليل_str), lhs->lhs_name);
	أرجع FAIL;
    }
    إذا (!is_decl && lhs->lhs_lvar != عدم
			   && lhs->lhs_lvar->lv_const != سونپيو_VAR
			   && !lhs->lhs_has_اشاريو)
    {
	semsg(_(e_cannot_سونپيو_منزل_constant_str), lhs->lhs_name);
	أرجع FAIL;
    }
    أرجع OK;
}

/*
 * Return TRUE if "lhs" has a range index: "[expr : expr]".
 */
    ساكن صحيح
has_list_اشاريو(char_u *idx_شروعات, cctx_T *cctx)
{
    char_u  *p = idx_شروعات;
    صحيح	    save_skip;

    إذا (*p != '[')
	أرجع FALSE;

    p = skipwhite(p + 1);
    إذا (*p == ':')
	أرجع TRUE;

    save_skip = cctx->ctx_skip;
    cctx->ctx_skip = SKIP_YES;
    (فراغ)compile_expr0(&p, cctx);
    cctx->ctx_skip = save_skip;
    أرجع *skipwhite(p) == ':';
}

/*
 * For an assignment with an index, compile the "idx" in "var[idx]" or "key" in
 * "var.key".
 */
    ساكن صحيح
compile_سونپيو_اشاريو(
	char_u	*var_شروعات,
	lhs_T	*lhs,
	صحيح	*range,
	cctx_T	*cctx)
{
    size_t	varlen = lhs->lhs_varlen;
    char_u	*p;
    صحيح		r = OK;
    صحيح		need_white_before = TRUE;
    صحيح		خالي_جايون_ٻيو;

    p = var_شروعات + varlen;
    إذا (*p == '[')
    {
	p = skipwhite(p + 1);
	إذا (*p == ':')
	{
	    // empty first index, push zero
	    r = generate_PUSHNR(cctx, 0);
	    need_white_before = FALSE;
	}
	وإلا
	    r = compile_expr0(&p, cctx);

	إذا (r == OK && *skipwhite(p) == ':')
	{
	    // unlet var[idx : idx]
	    // blob[idx : idx] = value
	    *range = TRUE;
	    p = skipwhite(p);
	    خالي_جايون_ٻيو = *skipwhite(p + 1) == ']';
	    إذا ((need_white_before && !IS_WHITE_OR_NUL(p[-1]))
		    || (!خالي_جايون_ٻيو && !IS_WHITE_OR_NUL(p[1])))
	    {
		semsg(_(e_white_space_required_before_and_after_str_at_str),
								      ":", p);
		أرجع FAIL;
	    }
	    p = skipwhite(p + 1);
	    إذا (*p == ']')
		// empty second index, push "none"
		r = generate_PUSHSPEC(cctx, VVAL_NONE);
	    وإلا
		r = compile_expr0(&p, cctx);
	}

	إذا (r == OK && *skipwhite(p) != ']')
	{
	    // this should not happen
	    emsg(_(e_missing_closing_چورس_brace));
	    r = FAIL;
	}
    }
    وإلا إذا (lhs->lhs_member_idx >= 0)
    {
	// object member index
	r = generate_PUSHNR(cctx, lhs->lhs_member_idx);
    }
    وإلا // if (*p == '.')
    {
	char_u *ڪنجي_پڇاڙي = منزل_name_پڇاڙي(p + 1, TRUE);
	char_u *ڪنجي = vim_strnsave(p + 1, ڪنجي_پڇاڙي - p - 1);

	r = generate_PUSHS(cctx, &ڪنجي);
    }
    أرجع r;
}

/*
 * For a LHS with an index, load the variable to be indexed.
 */
    ساكن صحيح
compile_ڀريو_lhs(
	lhs_T	*lhs,
	char_u	*var_شروعات,
	type_T	*rhs_type,
	cctx_T	*cctx)
{
    إذا (lhs->lhs_dest == dest_اظهار)
    {
	size_t	    varlen = lhs->lhs_varlen;
	صحيح	    c = var_شروعات[varlen];
	صحيح	    سٽون_len = cctx->ctx_ufunc->uf_سٽون.ga_len;
	صحيح	    res;

	// Evaluate "ll[expr]" of "ll[expr][idx]".  End the line with a NUL and
	// limit the lines array length to avoid skipping to a following line.
	var_شروعات[varlen] = NUL;
	cctx->ctx_ufunc->uf_سٽون.ga_len = cctx->ctx_lnum + 1;
	char_u *p = var_شروعات;
	res = compile_expr0(&p, cctx);
	var_شروعات[varlen] = c;
	cctx->ctx_ufunc->uf_سٽون.ga_len = سٽون_len;
	إذا (res == FAIL || p != var_شروعات + varlen)
	{
	    // this should not happen
	    إذا (res != FAIL)
		emsg(_(e_missing_closing_چورس_brace));
	    أرجع FAIL;
	}

	lhs->lhs_type = cctx->ctx_type_ڍير.ga_len == 0 ? &t_void
						  : get_type_on_ڍير(cctx, 0);

	إذا (lhs->lhs_type->tt_type == VAR_CLASS
		|| (lhs->lhs_type->tt_type == VAR_OBJECT
		    && lhs->lhs_type != &t_object_any))
	{
	    // Check whether the class or object variable is modifiable
	    إذا (!lhs_class_member_modifiable(lhs, var_شروعات, cctx))
		أرجع FAIL;
	}

	// Now we can properly check the type.  The variable is indexed, thus
	// we need the member type.  For a class or object we don't know the
	// type yet, it depends on what member is used.
	// The top item in the stack is the Dict, followed by the key and then
	// the type of the value.
	vartype_T vartype = lhs->lhs_type->tt_type;
	type_T *member_type = lhs->lhs_type->tt_member;
	إذا (rhs_type != عدم && member_type != عدم
		&& vartype != VAR_OBJECT && vartype != VAR_CLASS
		&& rhs_type != &t_void
		&& need_type(rhs_type, member_type, 0,
					    -3, 0, cctx, FALSE, FALSE) == FAIL)
	    أرجع FAIL;

	أرجع OK;
    }

    أرجع generate_loadvar(cctx, lhs);
}

/*
 * Produce code for loading "lhs" and also take care of an index.
 * Return OK/FAIL.
 */
    صحيح
compile_ڀريو_lhs_with_اشاريو(lhs_T *lhs, char_u *var_شروعات, cctx_T *cctx)
{
    إذا (lhs->lhs_type->tt_type == VAR_OBJECT)
    {
	// "this.value": load "this" object and get the value at index for an
	// object or class member get the type of the member.
	// Also for "obj.value".
	char_u *dot = vim_strchr(var_شروعات, '.');
	إذا (dot == عدم)
	{
	    semsg(_(e_missing_dot_after_object_str), lhs->lhs_name);
	    أرجع FAIL;
	}

	class_T	*cl = lhs->lhs_type->tt_class;
	type_T	*type = oc_member_type(cl, TRUE, dot + 1,
					  lhs->lhs_پڇاڙي, &lhs->lhs_member_idx);
	إذا (lhs->lhs_member_idx < 0)
	    أرجع FAIL;

	إذا (dot - var_شروعات == 4 && STRNCMP(var_شروعات, "this", 4) == 0)
	{
	    // load "this"
	    lvar_T  *lvar = lhs->lhs_lvar;
	    صحيح	    rc;

	    إذا (lvar->lv_from_outer > 0)
		rc = generate_LOADOUTER(cctx, lvar->lv_idx,
			lvar->lv_from_outer, lvar->lv_loop_اونهائي,
			lvar->lv_loop_idx, type);
	    وإلا
		rc = generate_ڀريو(cctx, ISN_ڀريو, lvar->lv_idx, عدم, type);

	    إذا (rc == FAIL)
		أرجع FAIL;
	}
	وإلا
	{
	    // load object variable or argument
	    إذا (compile_ڀريو_lhs(lhs, var_شروعات, lhs->lhs_type, cctx) == FAIL)
		أرجع FAIL;
	}
	إذا (IS_INTERFACE(cl))
	    أرجع generate_GET_ITF_MEMBER(cctx, cl, lhs->lhs_member_idx, type);
	أرجع generate_GET_OBJ_MEMBER(cctx, lhs->lhs_member_idx, type);
    }
    وإلا إذا (lhs->lhs_type->tt_type == VAR_CLASS)
    {
	// "<classname>.value": load class variable "classname.value"
	char_u *dot = vim_strchr(var_شروعات, '.');
	إذا (dot == عدم)
	{
	    (فراغ)check_type_is_قدر(lhs->lhs_type);
	    أرجع FAIL;
	}

	class_T	*cl = lhs->lhs_type->tt_class;
	ocmember_T *m = class_member_lookup(cl, dot + 1,
						lhs->lhs_پڇاڙي - dot - 1,
						&lhs->lhs_member_idx);
	إذا (m == عدم)
	    أرجع FAIL;

	أرجع generate_CLASSMEMBER(cctx, TRUE, cl, lhs->lhs_member_idx);
    }

    إذا (compile_ڀريو_lhs(lhs, var_شروعات, عدم, cctx) == FAIL)
	أرجع FAIL;

    إذا (lhs->lhs_has_اشاريو)
    {
	صحيح	range = FALSE;
	type_T	*type = get_type_on_ڍير(cctx, 0);

	// A member of an object or class is not obtained by indexing it.
	إذا (type->tt_type == VAR_CLASS
		|| (type->tt_type == VAR_OBJECT && type != &t_object_any))
	{
	    char_u *p = var_شروعات + lhs->lhs_varlen;

	    أرجع compile_class_object_اشاريو(cctx, &p, type);
	}

	// Get member from list or dict.  First compile the
	// index value.
	إذا (compile_سونپيو_اشاريو(var_شروعات, lhs, &range, cctx) == FAIL)
	    أرجع FAIL;
	إذا (range)
	{
	    semsg(_(e_cannot_use_range_with_assignment_operator_str),
								    var_شروعات);
	    أرجع FAIL;
	}

	// Get the member.
	إذا (compile_member(FALSE, عدم, cctx) == FAIL)
	    أرجع FAIL;
    }
    أرجع OK;
}

/*
 * Assignment to a list or dict member, or ":unlet" for the item, using the
 * information in "lhs".
 * Returns OK or FAIL.
 */
    صحيح
compile_سونپيو_unlet(
    char_u	*var_شروعات,
    lhs_T	*lhs,
    صحيح		is_سونپيو,
    type_T	*rhs_type,
    cctx_T	*cctx)
{
    vartype_T	dest_type;
    صحيح		range = FALSE;

    إذا (compile_سونپيو_اشاريو(var_شروعات, lhs, &range, cctx) == FAIL)
	أرجع FAIL;
    إذا (is_سونپيو && range
	    && lhs->lhs_type->tt_type != VAR_LIST
	    && lhs->lhs_type != &t_blob
	    && lhs->lhs_type != &t_any)
    {
	إذا (lhs->lhs_type->tt_type == VAR_TUPLE)
	    emsg(_(e_cannot_ٽڪرو_tuple));
	وإلا
	    semsg(_(e_cannot_use_range_with_assignment_str), var_شروعات);
	أرجع FAIL;
    }

    // For "expr[idx]" the index is compiled before the expression, thus the
    // resulting type cannot be used here.
    إذا (lhs->lhs_dest == dest_اظهار
	    || lhs->lhs_type == عدم || lhs->lhs_type == &t_any)
    {
	// Index on variable of unknown type: check at runtime.
	dest_type = VAR_ANY;
    }
    وإلا
    {
	dest_type = lhs->lhs_type->tt_type;
	إذا (dest_type == VAR_DICT && range)
	{
	    emsg(_(e_cannot_use_range_with_dictionary));
	    أرجع FAIL;
	}
	إذا (dest_type == VAR_DICT
		&& may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
	    أرجع FAIL;
	إذا (dest_type == VAR_LIST || dest_type == VAR_BLOB)
	{
	    type_T *type;

	    إذا (range)
	    {
		type = get_type_on_ڍير(cctx, 1);
		إذا (need_type(type, &t_عدد, 0,
					    -2, 0, cctx, FALSE, FALSE) == FAIL)
		أرجع FAIL;
	    }
	    type = get_type_on_ڍير(cctx, 0);
	    إذا ((dest_type != VAR_BLOB && type->tt_type != VAR_SPECIAL)
		    && need_type(type, &t_عدد, 0,
					    -1, 0, cctx, FALSE, FALSE) == FAIL)
		أرجع FAIL;
	}
    }

    إذا (cctx->ctx_skip == SKIP_YES)
	أرجع OK;

    // Load the dict, list or object.  On the stack we then have:
    // - value (for assignment, not for :unlet)
    // - index
    // - for [a : b] second index
    // - variable
    إذا (compile_ڀريو_lhs(lhs, var_شروعات, rhs_type, cctx) == FAIL)
	أرجع FAIL;

    إذا (dest_type == VAR_LIST
	    || dest_type == VAR_DICT
	    || dest_type == VAR_BLOB
	    || dest_type == VAR_CLASS
	    || dest_type == VAR_OBJECT
	    || dest_type == VAR_ANY)
    {
	إذا (is_سونپيو)
	{
	    إذا (range)
	    {
		إذا (generate_instr_drop(cctx, ISN_STORERANGE, 4) == عدم)
		    أرجع FAIL;
	    }
	    وإلا
	    {
		isn_T	*isn = generate_instr_drop(cctx, ISN_STOREINDEX, 3);

		إذا (isn == عدم)
		    أرجع FAIL;
		isn->isn_arg.storeindex.si_vartype = dest_type;
		isn->isn_arg.storeindex.si_class = عدم;

		إذا (dest_type == VAR_OBJECT)
		{
		    class_T *cl = lhs->lhs_type->tt_class;

		    إذا (IS_INTERFACE(cl))
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
	وإلا إذا (range)
	{
	    إذا (generate_instr_drop(cctx, ISN_UNLETRANGE, 3) == عدم)
		أرجع FAIL;
	}
	وإلا
	{
	    إذا (generate_instr_drop(cctx, ISN_UNLETINDEX, 2) == عدم)
		أرجع FAIL;
	}
    }
    وإلا
    {
	إذا (dest_type == VAR_TUPLE)
	    emsg(_(e_tuple_is_immutable));
	وإلا
	    emsg(_(e_indexable_type_required));
	أرجع FAIL;
    }

    أرجع OK;
}

/*
 * Generate an instruction to push the default value for "vartype".
 * if "dest_local" is TRUE then for some types no instruction is generated.
 * "skip_store" is set to TRUE if no PUSH instruction is generated.
 * Returns OK or FAIL.
 */
    ساكن صحيح
ڌڪو_default_قدر(
	cctx_T	    *cctx,
	vartype_T   vartype,
	صحيح	    dest_is_local,
	صحيح	    *skip_store)
{
    صحيح r = OK;

    بدل (vartype)
    {
	حالة VAR_BOOL:
	    r = generate_PUSHBOOL(cctx, VVAL_FALSE);
	    اكسر;
	حالة VAR_FLOAT:
	    r = generate_PUSHF(cctx, 0.0);
	    اكسر;
	حالة VAR_STRING:
	    r = generate_PUSHS(cctx, عدم);
	    اكسر;
	حالة VAR_BLOB:
	    r = generate_PUSHBLOB(cctx, blob_alloc());
	    اكسر;
	حالة VAR_FUNC:
	    r = generate_PUSHFUNC(cctx, عدم, &t_func_void, TRUE);
	    اكسر;
	حالة VAR_LIST:
	    r = generate_NEWLIST(cctx, 0, FALSE);
	    اكسر;
	حالة VAR_TUPLE:
	    r = generate_NEWTUPLE(cctx, 0, FALSE);
	    اكسر;
	حالة VAR_DICT:
	    r = generate_NEWDICT(cctx, 0, FALSE);
	    اكسر;
	حالة VAR_ڪم:
	    r = generate_PUSHJOB(cctx);
	    اكسر;
	حالة VAR_CHANNEL:
	    r = generate_PUSHCHANNEL(cctx);
	    اكسر;
	حالة VAR_OBJECT:
	    r = generate_PUSHOBJ(cctx);
	    اكسر;
	حالة VAR_عدد:
	حالة VAR_UNKNOWN:
	حالة VAR_ANY:
	حالة VAR_PARTIAL:
	حالة VAR_VOID:
	حالة VAR_INSTR:
	حالة VAR_CLASS:
	حالة VAR_TYPEALIAS:
	حالة VAR_SPECIAL:  // cannot happen
	    // This is skipped for local variables, they are always
	    // initialized to zero.  But in a "for" or "while" loop
	    // the value may have been changed.
	    إذا (dest_is_local && !inside_loop_scope(cctx))
		*skip_store = TRUE;
	    وإلا
		r = generate_PUSHNR(cctx, 0);
	    اكسر;
    }
    أرجع r;
}

/*
 * Compile assignment context.  Used when compiling an assignment statement.
 */
تعريفنوع بنية cac_S cac_T;
بنية cac_S
{
    cmdidx_T	cac_cmdidx;		// assignment command
    char_u	*cac_nextc;		// next character to parse
    lhs_T	cac_lhs;		// lhs of the assignment
    type_T	*cac_rhs_type;		// rhs type of an assignment
    char_u	*cac_op;		// assignment operator
    صحيح		cac_oplen;		// assignment operator length
    char_u	*cac_var_شروعات;		// start of the variable names
    char_u	*cac_var_پڇاڙي;		// end of the variable names
    صحيح		cac_var_ڳڻپ;		// number of variables in assignment
    صحيح		cac_var_idx;		// variable index in a list
    صحيح		cac_semicolon;		// semicolon in [var1, var2; var3]
    garray_T	*cac_instr;
    صحيح		cac_instr_ڳڻپ;
    صحيح		cac_incdec;
    صحيح		cac_did_generate_ٽڪرو;
    صحيح		cac_is_decl;
    صحيح		cac_is_const;
    صحيح		cac_شروعات_lnum;
    type_T	*cac_inferred_type;
    صحيح		cac_skip_store;
};

/*
 * Initialize the compile assignment context.
 */
    ساكن فراغ
compile_سونپيو_context_init(cac_T *cac, cctx_T *cctx, صحيح cmdidx, char_u *arg)
{
    CLEAR_ميدان(*cac);
    cac->cac_cmdidx = cmdidx;
    cac->cac_instr = &cctx->ctx_instr;
    cac->cac_rhs_type = &t_any;
    cac->cac_is_decl = is_decl_command(cmdidx);
    cac->cac_شروعات_lnum = SOURCING_LNUM;
    cac->cac_instr_ڳڻپ = -1;
    cac->cac_var_پڇاڙي = arg;
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
    ساكن صحيح
compile_سونپيو_obj_new_arg(char_u **argp, cctx_T *cctx)
{
    char_u *arg = *argp;

    arg += 11;	    // skip "ifargisset"
    صحيح def_arg_idx = getdigits(&arg);
    arg = skipwhite(arg);

    // Use a JUMP_IF_ARG_NOT_SET instruction to skip if the value was not
    // given and the default value is "v:none".
    صحيح ڍير_وٿي = ڍير_چوکٺو_سائيز +
				(cctx->ctx_ufunc->uf_va_name != عدم ? 1 : 0);
    صحيح def_arg_ڳڻپ = cctx->ctx_ufunc->uf_def_دليلون.ga_len;
    صحيح arg_وٿي = def_arg_idx - def_arg_ڳڻپ - ڍير_وٿي;

    إذا (generate_JUMP_IF_ARG(cctx, ISN_JUMP_IF_ARG_NOT_سيٽ,
			     arg_وٿي) == FAIL)
	أرجع FAIL;

    *argp = arg;
    أرجع OK;
}

/*
 * Translate the increment (++) and decrement (--) operators to the
 * corresponding compound operators (+= or -=).
 *
 * Returns OK on success and FAIL on syntax error.
 */
    ساكن صحيح
translate_incdec_op(exarg_T *eap, cac_T *cac)
{
    إذا (VIM_ISWHITE(eap->cmd[2]))
    {
	semsg(_(e_no_white_space_allowed_after_str_str),
		eap->cmdidx == CMD_increment ? "++" : "--", eap->cmd);
	أرجع FAIL;
    }
    cac->cac_op = (char_u *)(eap->cmdidx == CMD_increment ? "+=" : "-=");
    cac->cac_oplen = 2;
    cac->cac_incdec = TRUE;

    أرجع OK;
}

/*
 * Process the operator in an assignment statement.
 */
    ساكن صحيح
compile_سونپيو_process_operator(
    exarg_T	*eap,
    char_u	*arg,
    cac_T	*cac,
    صحيح		*heredoc,
    char_u	**retstr)
{
    *retstr = عدم;

    إذا (eap->cmdidx == CMD_increment || eap->cmdidx == CMD_decrement)
	// Change an unary operator to a compound operator
	أرجع translate_incdec_op(eap, cac);

    char_u *sp = cac->cac_nextc;
    cac->cac_nextc = skipwhite(cac->cac_nextc);
    cac->cac_op = cac->cac_nextc;
    cac->cac_oplen = assignment_len(cac->cac_nextc, heredoc);

    إذا (cac->cac_var_ڳڻپ > 0 && cac->cac_oplen == 0)
    {
	// can be something like "[1, 2]->func()"
	*retstr = arg;
	أرجع FAIL;
    }

    // need white space before and after the operator
    إذا (cac->cac_oplen > 0 && (!VIM_ISWHITE(*sp)
		|| !IS_WHITE_OR_NUL(cac->cac_op[cac->cac_oplen])))
    {
	error_white_both(cac->cac_op, cac->cac_oplen);
	أرجع FAIL;
    }

    أرجع OK;
}

/*
 * Find the start of an assignment statement.
 */
    ساكن char_u *
compile_سونپيو_compute_شروعات(char_u *arg, صحيح var_ڳڻپ)
{
    إذا (var_ڳڻپ > 0)
	// [var1, var2] = [val1, val2]
	// skip over the "["
	أرجع skipwhite(arg + 1);

    أرجع arg;
}

/*
 * Parse a heredoc assignment starting at "p".  Returns a pointer to the
 * beginning of the heredoc content.
 */
    ساكن char_u *
parse_heredoc_assignment(exarg_T *eap, cctx_T *cctx, cac_T *cac)
{
    // [let] varname =<< [trim] {end}
    eap->ea_getline = exarg_getline;
    eap->cookie = cctx;

    list_T *l = heredoc_get(eap, cac->cac_nextc + 3, FALSE, TRUE);
    إذا (l == عدم)
	أرجع عدم;

    list_free(l);
    cac->cac_nextc += STRLEN(cac->cac_nextc);

    أرجع cac->cac_nextc;
}

/*
 * Check the type of a RHS expression in a list assignment statement.
 * The RHS expression is already compiled.  So the type is on the stack.
 */
    ساكن صحيح
compile_سونپيو_list_check_rhs_type(cctx_T *cctx, cac_T *cac)
{
    type_T	*stacktype;

    stacktype = cctx->ctx_type_ڍير.ga_len == 0 ? &t_void
						: get_type_on_ڍير(cctx, 0);
    إذا (stacktype->tt_type == VAR_VOID)
    {
	emsg(_(e_cannot_use_void_قدر));
	أرجع FAIL;
    }

    إذا (stacktype->tt_type != VAR_LIST && stacktype->tt_type != VAR_TUPLE
					&& stacktype->tt_type != VAR_ANY)
    {
	emsg(_(e_list_or_tuple_required));
	أرجع FAIL;
    }

    إذا (need_type(stacktype,
		  stacktype->tt_type == VAR_TUPLE ? &t_tuple_any : &t_list_any,
		  TYPECHK_TUPLE_OK, -1, 0, cctx, FALSE, FALSE) == FAIL)
	أرجع FAIL;

    // The check accepts both a list and a tuple. Keep "any", making it a list
    // would reject a tuple later on.
    إذا (stacktype->tt_type == VAR_ANY)
	سيٽ_type_on_ڍير(cctx, &t_any, 0);

    إذا (stacktype->tt_type == VAR_TUPLE)
    {
	إذا (stacktype->tt_argcount != 1)
	    cac->cac_rhs_type = &t_any;
	وإلا
	{
	    إذا (stacktype->tt_flags & TTFLAG_VARARGS)
		cac->cac_rhs_type = stacktype->tt_دليلون[0]->tt_member;
	    وإلا
		cac->cac_rhs_type = stacktype->tt_دليلون[0];
	}
    }
    وإلا إذا (stacktype->tt_member != عدم)
	cac->cac_rhs_type = stacktype->tt_member;

    أرجع OK;
}

/*
 * In a list assignment statement, if a constant list was used, check the
 * length.  Returns OK if the length check succeeds.  Returns FAIL otherwise.
 */
    ساكن صحيح
compile_سونپيو_list_check_ڊيگهائي(cctx_T *cctx, cac_T *cac)
{
    صحيح	needed_list_len;
    صحيح	did_check = FALSE;

    needed_list_len = cac->cac_semicolon
				? cac->cac_var_ڳڻپ - 1
				: cac->cac_var_ڳڻپ;
    إذا (cac->cac_instr->ga_len > 0)
    {
	isn_T	*isn = ((isn_T *)cac->cac_instr->ga_معلومات) +
						cac->cac_instr->ga_len - 1;

	إذا (isn->isn_type == ISN_NEWLIST || isn->isn_type == ISN_NEWTUPLE)
	{
	    did_check = TRUE;
	    إذا (cac->cac_semicolon ?
			isn->isn_arg.عدد < needed_list_len
			: isn->isn_arg.عدد != needed_list_len)
	    {
		semsg(_(e_expected_nr_شيون_but_got_nr),
			needed_list_len, (صحيح)isn->isn_arg.عدد);
		أرجع FAIL;
	    }
	}
    }

    إذا (!did_check)
	generate_CHECKLEN(cctx, needed_list_len, cac->cac_semicolon);

    أرجع OK;
}

/*
 * Evaluate the expression for "[var, var] = expr" assignment.
 * A line break may follow the assignment operator "=".
 */
    ساكن char_u *
compile_سونپيو_list_اظهار(cctx_T *cctx, cac_T *cac)
{
    char_u *whitep;

    whitep = cac->cac_op + cac->cac_oplen;

    إذا (may_get_ايندڙ_سٽ_error(whitep, &cac->cac_nextc, cctx) == FAIL)
	أرجع عدم;

    // compile RHS expression
    إذا (compile_expr0(&cac->cac_nextc, cctx) == FAIL)
	أرجع عدم;

    إذا (cctx->ctx_skip == SKIP_YES)
	// no need to parse more when skipping
	أرجع cac->cac_nextc;

    إذا (compile_سونپيو_list_check_rhs_type(cctx, cac) == FAIL)
	أرجع عدم;

    // If a constant list was used we can check the length right here.
    إذا (compile_سونپيو_list_check_ڊيگهائي(cctx, cac) == FAIL)
	أرجع FAIL;

    أرجع cac->cac_nextc;
}

/*
 * Find and return the end of a heredoc or a list of variables assignment
 * statement.  For a single variable assignment statement, returns the current
 * end.
 * Returns NULL on failure.
 */
    ساكن char_u *
compile_سونپيو_compute_پڇاڙي(
    exarg_T	*eap,
    cctx_T	*cctx,
    cac_T	*cac,
    صحيح		heredoc)
{
    إذا (heredoc)
    {
	cac->cac_nextc = parse_heredoc_assignment(eap, cctx, cac);
	أرجع cac->cac_nextc;
    }

    إذا (cac->cac_var_ڳڻپ > 0)
    {
	// for "[var, var] = expr" evaluate the expression. The list of
	// variables are processed later.
	// A line break may follow the "=".
	cac->cac_nextc = compile_سونپيو_list_اظهار(cctx, cac);
	أرجع cac->cac_nextc;
    }

    أرجع cac->cac_var_پڇاڙي;
}

/*
 * For "var = expr" evaluate the expression.
 */
    ساكن صحيح
compile_سونپيو_single_eval_اظهار(cctx_T *cctx, cac_T *cac)
{
    صحيح		ret = OK;
    char_u	*whitep;
    lhs_T	*lhs = &cac->cac_lhs;
    lvar_T	*lvp;
    lvar_T	save_lhs_lvar;

    // Compile the expression.
    إذا (cac->cac_incdec)
	أرجع generate_PUSHNR(cctx, 1);

    // Temporarily hide the new local variable here, it is
    // not available to this expression.
    إذا (lhs->lhs_new_local)
    {
	--cctx->ctx_locals.ga_len;

	// Save the local variable value (compiling the RHS expression may
	// create new local variables).
	lvp = ((lvar_T *)cctx->ctx_locals.ga_معلومات) + cctx->ctx_locals.ga_len;
	save_lhs_lvar = *lvp;
    }
    whitep = cac->cac_op + cac->cac_oplen;

    إذا (may_get_ايندڙ_سٽ_error(whitep, &cac->cac_nextc, cctx) == FAIL)
    {
	إذا (lhs->lhs_new_local)
	    ++cctx->ctx_locals.ga_len;
	أرجع FAIL;
    }

    ret = compile_expr0_ext(&cac->cac_nextc, cctx, &cac->cac_is_const);
    إذا (lhs->lhs_new_local)
    {
	// Restore the local variable value.  Update lhs_lvar as the index of
	// the local variable might have changed.
	lvp = ((lvar_T *)cctx->ctx_locals.ga_معلومات) + cctx->ctx_locals.ga_len;
	*lvp = save_lhs_lvar;
	lhs->lhs_lvar = lvp;

	++cctx->ctx_locals.ga_len;
    }

    أرجع ret;
}

/*
 * When compiling an assignment, set the LHS type to the RHS type.
 */
    ساكن صحيح
compile_سونپيو_سيٽ_lhs_type_from_rhs(
    cctx_T	*cctx,
    cac_T	*cac,
    lhs_T	*lhs,
    type_T	*rhs_type)
{
    إذا (rhs_type->tt_type == VAR_VOID)
    {
	emsg(_(e_cannot_use_void_قدر));
	أرجع FAIL;
    }

    type_T *type;

    // An empty list or dict has a &t_unknown member, for a variable that
    // implies &t_any.
    إذا (rhs_type == &t_list_خالي_جايون)
	type = &t_list_any;
    وإلا إذا (rhs_type == &t_dict_خالي_جايون)
	type = &t_dict_any;
    وإلا إذا (rhs_type == &t_unknown)
	type = &t_any;
    وإلا
    {
	type = rhs_type;
	cac->cac_inferred_type = rhs_type;
    }

    سيٽ_var_type(lhs->lhs_lvar, type, cctx);

    أرجع OK;
}

/*
 * Returns TRUE if the "rhs_type" can be assigned to the "lhs" variable.
 * Used when compiling an assignment statement.
 */
    ساكن صحيح
compile_سونپيو_valid_rhs_type(
    cctx_T	*cctx,
    cac_T	*cac,
    lhs_T	*lhs,
    type_T	*rhs_type)
{
    type_T	*use_type = lhs->lhs_lvar->lv_type;
    where_T	where = WHERE_INIT;

    // Without operator check type here, otherwise below.
    // Use the line number of the assignment.
    SOURCING_LNUM = cac->cac_شروعات_lnum;
    إذا (cac->cac_var_ڳڻپ > 0)
    {
	where.wt_اشاريو = cac->cac_var_idx + 1;
	where.wt_قسم = WT_VARIABLE;
    }

    // If assigning to a list or dict member, use the member type.
    // Not for "list[:] =".
    إذا (lhs->lhs_has_اشاريو &&
	    !has_list_اشاريو(cac->cac_var_شروعات + lhs->lhs_varlen, cctx))
	use_type = lhs->lhs_member_type;

    إذا (need_type_where(rhs_type, use_type, 0, -1, where, cctx, FALSE,
						cac->cac_is_const) == FAIL)
	أرجع FALSE;

    أرجع TRUE;
}

/*
 * Compare the LHS type with the RHS type in an assignment.
 */
    ساكن صحيح
compile_سونپيو_check_type(cctx_T *cctx, cac_T *cac)
{
    lhs_T	*lhs = &cac->cac_lhs;
    type_T	*rhs_type;

    rhs_type = cctx->ctx_type_ڍير.ga_len == 0
					? &t_void
					: get_type_on_ڍير(cctx, 0);
    cac->cac_rhs_type = rhs_type;

    إذا (check_type_is_قدر(rhs_type) == FAIL)
	أرجع FAIL;

    إذا (lhs->lhs_lvar != عدم && (cac->cac_is_decl || !lhs->lhs_has_type))
    {
	إذا (rhs_type->tt_type == VAR_FUNC
					|| rhs_type->tt_type == VAR_PARTIAL)
	{
	    // Make sure the variable name can be used as a funcref
	    إذا (!lhs->lhs_has_اشاريو
				&& var_wrong_func_name(lhs->lhs_name, TRUE))
		أرجع FAIL;
	}

	إذا (lhs->lhs_new_local && !lhs->lhs_has_type)
	{
	    // The LHS variable doesn't have a type.  Set it to the RHS type.
	    إذا (compile_سونپيو_سيٽ_lhs_type_from_rhs(cctx, cac, lhs, rhs_type)
								== FAIL)
		أرجع FAIL;
	}
	وإلا إذا (*cac->cac_op == '=')
	{
	    إذا (!compile_سونپيو_valid_rhs_type(cctx, cac, lhs, rhs_type))
		أرجع FAIL;
	}
    }
    وإلا
    {
	// Assigning to a register using @r = "abc"

	type_T *lhs_type = lhs->lhs_member_type;

	// Special case: assigning to @# can use a number or a string.
	// Also: can assign a number to a float.
	إذا ((lhs_type == &t_عدد_or_string || lhs_type == &t_float)
					&& rhs_type->tt_type == VAR_عدد)
	    lhs_type = &t_عدد;

	إذا (*cac->cac_nextc != '=')
	{
	    إذا (need_type(rhs_type, lhs_type, 0, -1, 0, cctx, FALSE,
							FALSE) == FAIL)
		أرجع FAIL;
	}
    }

    أرجع OK;
}

/*
 * Compile the RHS expression in an assignment statement and generate the
 * instructions.
 */
    ساكن صحيح
compile_سونپيو_rhs_اظهار(cctx_T *cctx, cac_T *cac)
{
    cac->cac_is_const = FALSE;

    // for "+=", "*=", "..=" etc. first load the current value
    إذا (*cac->cac_op != '='
	    && compile_ڀريو_lhs_with_اشاريو(&cac->cac_lhs, cac->cac_var_شروعات,
								cctx) == FAIL)
	أرجع FAIL;

    // For "var = expr" evaluate the expression.
    إذا (cac->cac_var_ڳڻپ == 0)
    {
	صحيح	ret;

	// Compile the expression.
	cac->cac_instr_ڳڻپ = cac->cac_instr->ga_len;
	ret = compile_سونپيو_single_eval_اظهار(cctx, cac);
	إذا (ret == FAIL)
	    أرجع FAIL;
    }
    وإلا إذا (cac->cac_semicolon && cac->cac_var_idx == cac->cac_var_ڳڻپ - 1)
    {
	// For "[var; var] = expr" get the rest of the list
	cac->cac_did_generate_ٽڪرو = TRUE;
	إذا (generate_ٽڪرو(cctx, cac->cac_var_ڳڻپ - 1) == FAIL)
	    أرجع FAIL;
    }
    وإلا
    {
	// For "[var, var] = expr" get the "var_idx" item from the
	// list.
	صحيح with_op = *cac->cac_op != '=';
	إذا (generate_GETITEM(cctx, cac->cac_var_idx, with_op) == FAIL)
	    أرجع FAIL;
    }

    إذا (compile_سونپيو_check_type(cctx, cac) == FAIL)
	أرجع FAIL;

    أرجع OK;
}

/*
 * Compile the RHS expression in an assignment
 */
    ساكن صحيح
compile_سونپيو_rhs(cctx_T *cctx, cac_T *cac)
{
    lhs_T	*lhs = &cac->cac_lhs;

    إذا (cctx->ctx_skip == SKIP_YES)
    {
	إذا (cac->cac_oplen > 0 && cac->cac_var_ڳڻپ == 0)
	{
	    // skip over the "=" and the expression
	    cac->cac_nextc = skipwhite(cac->cac_op + cac->cac_oplen);
	    (فراغ)compile_expr0(&cac->cac_nextc, cctx);
	}
	أرجع OK;
    }

    // If RHS is specified, then generate instructions for RHS expression
    إذا (cac->cac_oplen > 0)
	أرجع compile_سونپيو_rhs_اظهار(cctx, cac);

    إذا (cac->cac_cmdidx == CMD_final)
    {
	emsg(_(e_final_requires_a_قدر));
	أرجع FAIL;
    }

    إذا (cac->cac_cmdidx == CMD_const)
    {
	emsg(_(e_const_requires_a_قدر));
	أرجع FAIL;
    }

    إذا (!lhs->lhs_has_type || lhs->lhs_dest == dest_option
					|| lhs->lhs_dest == dest_func_option)
    {
	emsg(_(e_type_or_initialization_required));
	أرجع FAIL;
    }

    // variables are always initialized
    إذا (GA_GROW_FAILS(cac->cac_instr, 1))
	أرجع FAIL;

    cac->cac_instr_ڳڻپ = cac->cac_instr->ga_len;

    أرجع ڌڪو_default_قدر(cctx, lhs->lhs_member_type->tt_type,
			      lhs->lhs_dest == dest_local,
			      &cac->cac_skip_store);
}

/*
 * Returns OK if "type" supports compound operator "op_arg" (e.g. +=, -=, %=,
 * etc.).  Compound operators are not supported with a tuple and a dict.
 * Returns FAIL if compound operator is not supported.
 */
    ساكن صحيح
check_type_supports_compound_op(type_T *type, char_u op_arg)
{
    إذا (type->tt_type == VAR_TUPLE || type->tt_type == VAR_DICT)
    {
	char_u	op[2];

	op[0] = op_arg;
	op[1] = NUL;
	semsg(_(e_wrong_variable_type_for_str_equal), op);
	أرجع FAIL;
    }

    أرجع OK;
}

/*
 * Compile a compound op assignment statement (+=, -=, *=, %=, etc.)
 */
    ساكن صحيح
compile_سونپيو_compound_op(cctx_T *cctx, cac_T *cac)
{
    lhs_T	    *lhs = &cac->cac_lhs;
    type_T	    *expected;
    type_T	    *stacktype = عدم;

    إذا (cac->cac_lhs.lhs_type->tt_type == VAR_TUPLE
	    && check_type_supports_compound_op(cac->cac_lhs.lhs_type,
							*cac->cac_op) == FAIL)
	أرجع FAIL;

    إذا (*cac->cac_op == '.')
    {
	expected = lhs->lhs_member_type;
	stacktype = get_type_on_ڍير(cctx, 0);
	إذا (expected != &t_string
		&& need_type(stacktype, expected, 0, -1, 0, cctx,
					FALSE, FALSE) == FAIL)
	    أرجع FAIL;
	وإلا إذا (may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
	    أرجع FAIL;
    }
    وإلا
    {
	expected = lhs->lhs_member_type;
	stacktype = get_type_on_ڍير(cctx, 0);

	إذا (check_type_supports_compound_op(expected, *cac->cac_op) == FAIL)
	    أرجع FAIL;

	إذا (
		// If variable is float operation with number is OK.
		!(expected == &t_float && (stacktype == &t_عدد
					|| stacktype == &t_عدد_bool))
		&& need_type(stacktype, expected, TYPECHK_عدد_OK, -1, 0,
						cctx, FALSE, FALSE) == FAIL)
	    أرجع FAIL;
    }

    إذا (*cac->cac_op == '.')
    {
	إذا (generate_CONCAT(cctx, 2) == FAIL)
	    أرجع FAIL;
    }
    وإلا إذا (*cac->cac_op == '+')
    {
	إذا (generate_جوڙيو_instr(cctx,
		    operator_type(lhs->lhs_member_type, stacktype),
		    lhs->lhs_member_type, stacktype,
		    اظهار_APPEND) == FAIL)
	    أرجع FAIL;
    }
    وإلا إذا (generate_two_op(cctx, cac->cac_op) == FAIL)
	أرجع FAIL;

    أرجع OK;
}

/*
 * Generate the STORE and SETTYPE instructions for an assignment statement.
 */
    ساكن صحيح
compile_سونپيو_generate_store(cctx_T *cctx, cac_T *cac)
{
    lhs_T	*lhs = &cac->cac_lhs;
    صحيح		save_lnum;

    // Use the line number of the assignment for store instruction.
    save_lnum = cctx->ctx_lnum;
    cctx->ctx_lnum = cac->cac_شروعات_lnum - 1;

    إذا (lhs->lhs_has_اشاريو)
    {
	// Use the info in "lhs" to store the value at the index in the
	// list, dict or object.
	إذا (compile_سونپيو_unlet(cac->cac_var_شروعات, &cac->cac_lhs,
				 TRUE, cac->cac_rhs_type, cctx) == FAIL)
	{
	    cctx->ctx_lnum = save_lnum;
	    أرجع FAIL;
	}
    }
    وإلا
    {
	إذا (cac->cac_is_decl && cac->cac_cmdidx == CMD_const &&
			(lhs->lhs_dest == dest_script
			 || lhs->lhs_dest == dest_script_v9
			 || lhs->lhs_dest == dest_global
			 || lhs->lhs_dest == dest_local))
	    // ":const var": lock the value, but not referenced variables
	    generate_LOCKCONST(cctx);

	type_T	*inferred_type = cac->cac_inferred_type;

	إذا ((lhs->lhs_type->tt_type == VAR_DICT
		    || lhs->lhs_type->tt_type == VAR_LIST)
		&& lhs->lhs_type->tt_member != عدم
		&& lhs->lhs_type->tt_member != &t_any
		&& lhs->lhs_type->tt_member != &t_unknown)
	    // Set the type in the list or dict, so that it can be
	    // checked, also in legacy script.
	    generate_SETTYPE(cctx, lhs->lhs_type);
	وإلا إذا (lhs->lhs_type->tt_type == VAR_TUPLE
					&& lhs->lhs_type->tt_argcount != 0)
	    generate_SETTYPE(cctx, lhs->lhs_type);
	وإلا إذا (inferred_type != عدم
		&& (inferred_type->tt_type == VAR_DICT
		    || inferred_type->tt_type == VAR_LIST)
		&& inferred_type->tt_member != عدم
		&& inferred_type->tt_member != &t_unknown
		&& inferred_type->tt_member != &t_any)
	    // Set the type in the list or dict, so that it can be
	    // checked, also in legacy script.
	    generate_SETTYPE(cctx, inferred_type);
	وإلا إذا (inferred_type != عدم
				&& inferred_type->tt_type == VAR_TUPLE
				&& inferred_type->tt_argcount > 0)
	    generate_SETTYPE(cctx, inferred_type);

	إذا (!cac->cac_skip_store &&
		generate_store_lhs(cctx, &cac->cac_lhs,
				   cac->cac_instr_ڳڻپ,
				   cac->cac_is_decl) == FAIL)
	{
	    cctx->ctx_lnum = save_lnum;
	    أرجع FAIL;
	}
    }

    cctx->ctx_lnum = save_lnum;

    أرجع OK;
}

/*
 * Process the variable(s) in an assignment statement
 */
    ساكن صحيح
compile_سونپيو_process_variables(
    cctx_T	*cctx,
    cac_T	*cac,
    صحيح		cmdidx,
    صحيح		heredoc,
    صحيح		has_cmd,
    صحيح		has_argisset_اڳياڙي,
    صحيح		jump_instr_idx)
{
    /*
     * Loop over variables in "[var, var] = expr".
     * For "name = expr" and "var name: type" this is done only once.
     */
    لكل (cac->cac_var_idx = 0; cac->cac_var_idx == 0 ||
	    cac->cac_var_idx < cac->cac_var_ڳڻپ; cac->cac_var_idx++)
    {
	إذا (cac->cac_var_شروعات[0] == '_'
				&& !eval_isnamec(cac->cac_var_شروعات[1]))
	{
	    // Ignore underscore in "[a, _, b] = list".
	    إذا (cac->cac_var_ڳڻپ > 0)
	    {
		cac->cac_var_شروعات = skipwhite(cac->cac_var_شروعات + 2);
		استمر;
	    }
	    emsg(_(e_cannot_use_underscore_هتي));
	    أرجع FAIL;
	}
	vim_free(cac->cac_lhs.lhs_name);

	/*
	 * Figure out the LHS type and other properties.
	 */
	إذا (compile_سونپيو_lhs(cac->cac_var_شروعات, &cac->cac_lhs, cmdidx,
		    cac->cac_is_decl, heredoc, has_cmd,
		    cac->cac_oplen, cctx) == FAIL)
	    أرجع FAIL;

	// Compile the RHS expression
	إذا (heredoc)
	{
	    SOURCING_LNUM = cac->cac_شروعات_lnum;
	    إذا (cac->cac_lhs.lhs_has_type
		    && need_type(&t_list_string, cac->cac_lhs.lhs_type,
				0, -1, 0, cctx, FALSE, FALSE) == FAIL)
		أرجع FAIL;
	}
	وإلا
	{
	    إذا (compile_سونپيو_rhs(cctx, cac) == FAIL)
		أرجع FAIL;
	    إذا (cac->cac_var_ڳڻپ == 0)
		cac->cac_var_پڇاڙي = cac->cac_nextc;
	}

	// no need to parse more when skipping
	إذا (cctx->ctx_skip == SKIP_YES)
	    اكسر;

	إذا (cac->cac_oplen > 0 && *cac->cac_op != '=')
	{
	    إذا (compile_سونپيو_compound_op(cctx, cac) == FAIL)
		أرجع FAIL;
	}

	// generate the store instructions
	إذا (compile_سونپيو_generate_store(cctx, cac) == FAIL)
	    أرجع FAIL;

	إذا (cac->cac_var_idx + 1 < cac->cac_var_ڳڻپ)
	    cac->cac_var_شروعات = skipwhite(cac->cac_lhs.lhs_پڇاڙي + 1);

	إذا (has_argisset_اڳياڙي)
	{
	    // set instruction index in JUMP_IF_ARG_SET to here
	    isn_T *isn = ((isn_T *)cac->cac_instr->ga_معلومات) + jump_instr_idx;
	    isn->isn_arg.jumparg.jump_where = cac->cac_instr->ga_len;
	}
    }

    أرجع OK;
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
    ساكن char_u *
compile_assignment(
    char_u	*arg_شروعات,
    exarg_T	*eap,
    cmdidx_T	cmdidx,
    cctx_T	*cctx)
{
    cac_T	cac;
    char_u	*arg = arg_شروعات;
    char_u	*retstr = عدم;
    صحيح		heredoc = FALSE;
    صحيح		jump_instr_idx;

    compile_سونپيو_context_init(&cac, cctx, cmdidx, arg);

    jump_instr_idx = cac.cac_instr->ga_len;

    // process object variable initialization in a new() constructor method
    صحيح	has_argisset_اڳياڙي = STRNCMP(arg, "ifargisset ", 11) == 0;
    إذا (has_argisset_اڳياڙي &&
			compile_سونپيو_obj_new_arg(&arg, cctx) == FAIL)
	اذهب theend;

    // Skip over the "varname" or "[varname, varname]" to get to any "=".
    cac.cac_nextc = skip_var_list(arg, TRUE, &cac.cac_var_ڳڻپ,
						&cac.cac_semicolon, TRUE);
    إذا (cac.cac_nextc == عدم)
	أرجع *arg == '[' ? arg : عدم;

    إذا (compile_سونپيو_process_operator(eap, arg, &cac, &heredoc,
							&retstr) == FAIL)
	أرجع retstr;

    // Compute the start of the assignment
    cac.cac_var_شروعات = compile_سونپيو_compute_شروعات(arg, cac.cac_var_ڳڻپ);

    // Compute the end of the assignment
    cac.cac_var_پڇاڙي = compile_سونپيو_compute_پڇاڙي(eap, cctx, &cac, heredoc);
    إذا (cac.cac_var_پڇاڙي == عدم)
	أرجع عدم;

    صحيح has_cmd = cac.cac_var_شروعات > eap->cmd;

    /* process the variable(s) */
    إذا (compile_سونپيو_process_variables(cctx, &cac, cmdidx, heredoc,
					 has_cmd, has_argisset_اڳياڙي,
					 jump_instr_idx) == FAIL)
	اذهب theend;

    // For "[var, var] = expr" drop the "expr" value.
    // Also for "[var, var; _] = expr".
    إذا (cctx->ctx_skip != SKIP_YES && cac.cac_var_ڳڻپ > 0 &&
	    (!cac.cac_semicolon || !cac.cac_did_generate_ٽڪرو))
    {
	إذا (generate_instr_drop(cctx, ISN_DROP, 1) == عدم)
	    اذهب theend;
    }

    retstr = skipwhite(cac.cac_var_پڇاڙي);

theend:
    vim_free(cac.cac_lhs.lhs_name);
    أرجع retstr;
}

/*
 * Check for an assignment at "eap->cmd", compile it if found.
 * Return NOTDONE if there is none, FAIL for failure, OK if done.
 */
    ساكن صحيح
may_compile_assignment(exarg_T *eap, char_u **سٽ, cctx_T *cctx)
{
    char_u  *pskip;
    char_u  *p;

    // Assuming the command starts with a variable or function name,
    // find what follows.
    // Skip over "var.member", "var[idx]" and the like.
    // Also "&opt = val", "$ENV = val" and "@r = val".
    pskip = (*eap->cmd == '&' || *eap->cmd == '$' || *eap->cmd == '@')
						 ? eap->cmd + 1 : eap->cmd;
    p = منزل_name_پڇاڙي(pskip, TRUE);
    إذا (p > eap->cmd && *p != NUL)
    {
	char_u *var_پڇاڙي;
	صحيح	oplen;
	صحيح	heredoc;

	إذا (eap->cmd[0] == '@')
	    var_پڇاڙي = eap->cmd + 2;
	وإلا
	    var_پڇاڙي = ڳوليو_name_پڇاڙي(pskip, عدم, عدم,
					FNE_CHECK_شروعات | FNE_INCL_BR);
	oplen = assignment_len(skipwhite(var_پڇاڙي), &heredoc);
	إذا (oplen > 0)
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
	    إذا (*eap->cmd == '&'
		    || *eap->cmd == '$'
		    || *eap->cmd == '@'
		    || ((len) > 2 && eap->cmd[1] == ':')
		    || STRNCMP(eap->cmd, "g:[", 3) == 0
		    || variable_exists(eap->cmd, len, cctx))
	    {
		*سٽ = compile_assignment(eap->cmd, eap, CMD_سائيز, cctx);
		إذا (*سٽ == عدم || *سٽ == eap->cmd)
		    أرجع FAIL;
		أرجع OK;
	    }
	}
    }

    // might be "[var, var] = expr" or "ifargisset this.member = expr"
    إذا (*eap->cmd == '[' || STRNCMP(eap->cmd, "ifargisset ", 11) == 0)
    {
	*سٽ = compile_assignment(eap->cmd, eap, CMD_سائيز, cctx);
	إذا (*سٽ == عدم)
	    أرجع FAIL;
	إذا (*سٽ != eap->cmd)
	    أرجع OK;
    }
    أرجع NOTDONE;
}

/*
 * Check if arguments of "ufunc" shadow variables in "cctx".
 * Return OK or FAIL.
 */
    ساكن صحيح
check_دليلون_shadowing(ufunc_T *ufunc, cctx_T *cctx)
{
    صحيح	    i;
    char_u  *arg;
    صحيح	    r = OK;

    // Make sure arguments are not found when compiling a second time.
    ufunc->uf_دليلون_visible = 0;

    // Check for arguments shadowing variables from the context.
    لكل (i = 0; i < ufunc->uf_دليلون.ga_len; ++i)
    {
	arg = ((char_u **)(ufunc->uf_دليلون.ga_معلومات))[i];
	إذا (check_defined(arg, STRLEN(arg), cctx, عدم, TRUE) == FAIL)
	{
	    r = FAIL;
	    اكسر;
	}
    }
    ufunc->uf_دليلون_visible = ufunc->uf_دليلون.ga_len;
    أرجع r;
}

#جيڪڏهن_وصف_ٿيل HAS_پيغام_دري
/*
 * Get a count before a command.  Can only be a number.
 * Returns zero if there is no count.
 * Returns -1 if there is something wrong.
 */
    ساكن ڊگهو
get_cmd_ڳڻپ(char_u *سٽ, exarg_T *eap)
{
    char_u *p;

    // skip over colons and white space
    لكل (p = سٽ; *p == ':' || VIM_ISWHITE(*p); ++p)
	;
    إذا (!محفوظ_isdigit(*p))
    {
	// The command or modifiers must be following.  Assume a lower case
	// character means there is a modifier.
	إذا (p < eap->cmd && !vim_islower(*p))
	{
	    emsg(_(e_invalid_range));
	    أرجع -1;
	}
	أرجع 0;
    }
    أرجع atol((حرف *)p);
}
#جيڪڏهن_پورو

/*
 * Get the compilation type that should be used for "ufunc".
 * Keep in sync with INSTRUCTIONS().
 */
    compiletype_T
get_compile_type(ufunc_T *ufunc)
{
    // Update uf_has_breakpoint if needed.
    update_has_breakpoint(ufunc);

    إذا (debug_break_level > 0 || may_break_اندريون_function(ufunc))
	أرجع CT_DEBUG;
#جيڪڏهن_وصف_ٿيل FEAT_PROFILE
    إذا (do_profiling == PROF_YES)
    {
	إذا (!ufunc->uf_profiling && has_profiling(FALSE, ufunc->uf_name, عدم,
							    &ufunc->uf_hash))
	    func_do_profile(ufunc);
	إذا (ufunc->uf_profiling)
	    أرجع CT_PROFILE;
    }
#جيڪڏهن_پورو
    أرجع CT_NONE;
}

/*
 * Free the compiled instructions saved for a def function.  This is used when
 * compiling a def function and the function was compiled before.
 * The index is reused.
 */
    ساكن فراغ
clear_def_function(ufunc_T *ufunc, compiletype_T compile_type)
{
    isn_T	*instr_dest = عدم;
    dfunc_T	*dfunc;

    dfunc = ((dfunc_T *)def_functions.ga_معلومات) + ufunc->uf_dfunc_idx;

    بدل (compile_type)
    {
	حالة CT_PROFILE:
#جيڪڏهن_وصف_ٿيل FEAT_PROFILE
	    instr_dest = dfunc->df_instr_prof; اكسر;
#جيڪڏهن_پورو
	حالة CT_NONE:   instr_dest = dfunc->df_instr; اكسر;
	حالة CT_DEBUG:  instr_dest = dfunc->df_instr_debug; اكسر;
    }

    إذا (instr_dest != عدم)
	// Was compiled in this mode before: Free old instructions.
	delete_def_function_contents(dfunc, FALSE);

    ga_clear_لڙيون(&dfunc->df_var_نالا);
    dfunc->df_defer_var_idx = 0;
}

/*
 * Add a function to the list of :def functions.
 * This sets "ufunc->uf_dfunc_idx" but the function isn't compiled yet.
 */
    ساكن صحيح
جوڙيو_def_function(ufunc_T *ufunc)
{
    dfunc_T *dfunc;

    إذا (def_functions.ga_len == 0)
    {
	// The first position is not used, so that a zero uf_dfunc_idx means it
	// wasn't set.
	إذا (GA_GROW_FAILS(&def_functions, 1))
	    أرجع FAIL;
	++def_functions.ga_len;
    }

    // Add the function to "def_functions".
    إذا (GA_GROW_FAILS(&def_functions, 1))
	أرجع FAIL;
    dfunc = ((dfunc_T *)def_functions.ga_معلومات) + def_functions.ga_len;
    CLEAR_اشارو(dfunc);
    dfunc->df_idx = def_functions.ga_len;
    ufunc->uf_dfunc_idx = dfunc->df_idx;
    dfunc->df_ufunc = ufunc;
    dfunc->df_name = vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
    ga_init2(&dfunc->df_var_نالا, حجم(char_u *), 10);
    ++dfunc->df_refcount;
    ++def_functions.ga_len;
    أرجع OK;
}

    ساكن صحيح
compile_dfunc_ufunc_init(
    ufunc_T		*ufunc,
    cctx_T		*outer_cctx,
    compiletype_T	compile_type,
    صحيح			*new_def_function)
{
    // When using a function that was compiled before: Free old instructions.
    // The index is reused.  Otherwise add a new entry in "def_functions".
    إذا (ufunc->uf_dfunc_idx > 0)
	clear_def_function(ufunc, compile_type);
    وإلا
    {
	إذا (جوڙيو_def_function(ufunc) == FAIL)
	    أرجع FAIL;

	*new_def_function = TRUE;
    }

    إذا ((ufunc->uf_flags & FC_گهيرو) && outer_cctx == عدم)
    {
	semsg(_(e_compiling_گهيرو_without_context_str),
						printable_func_name(ufunc));
	أرجع FAIL;
    }

    ufunc->uf_def_حالت = UF_COMPILING;

    أرجع OK;
}

/*
 * Initialize the compilation context for compiling a def function.
 */
    ساكن فراغ
compile_dfunc_cctx_init(
    cctx_T		*cctx,
    cctx_T		*outer_cctx,
    ufunc_T		*ufunc,
    compiletype_T	compile_type)
{
    CLEAR_ميدان(*cctx);

    cctx->ctx_compile_type = compile_type;
    cctx->ctx_ufunc = ufunc;
    cctx->ctx_lnum = -1;
    cctx->ctx_outer = outer_cctx;
    ga_init2(&cctx->ctx_locals, حجم(lvar_T), 10);
    // Each entry on the type stack consists of two type pointers.
    ga_init2(&cctx->ctx_type_ڍير, حجم(type2_T), 50);
    cctx->ctx_type_list = &ufunc->uf_type_list;
    ga_init2(&cctx->ctx_instr, حجم(isn_T), 50);
}

/*
 * For an object constructor, generate instruction to setup "this" (the first
 * local variable) and to initialize the object variables.
 */
    ساكن صحيح
obj_constructor_prologue(ufunc_T *ufunc, cctx_T *cctx)
{
    generate_CONSTRUCT(cctx, ufunc->uf_class);

    لكل (صحيح i = 0; i < ufunc->uf_class->class_obj_member_ڳڻپ; ++i)
    {
	ocmember_T *m = &ufunc->uf_class->class_obj_members[i];

	إذا (i < 2 && IS_ENUM(ufunc->uf_class))
	    // The first two object variables in an enum are the name
	    // and the ordinal.  These are set by the ISN_CONSTRUCT
	    // instruction.  So don't generate instructions to set
	    // these variables.
	    استمر;

	إذا (m->ocm_init != عدم)
	{
	    char_u	*اظهار = m->ocm_init;
	    sctx_T	save_current_sctx;
	    صحيح		change_sctx = FALSE;

	    // If the member variable initialization script context is
	    // different from the current script context, then change it.
	    إذا (current_sctx.sc_sid != m->ocm_init_sctx.sc_sid)
		change_sctx = TRUE;

	    إذا (change_sctx)
	    {
		// generate an instruction to change the script context to the
		// member variable initialization script context.
		save_current_sctx = current_sctx;
		current_sctx = m->ocm_init_sctx;
		generate_SCRIPTCTX_سيٽ(cctx, current_sctx);
	    }

	    صحيح r = compile_expr0(&اظهار, cctx);

	    إذا (change_sctx)
	    {
		// restore the previous script context
		current_sctx = save_current_sctx;
		generate_SCRIPTCTX_سيٽ(cctx, current_sctx);
	    }

	    إذا (r == FAIL)
		أرجع FAIL;

	    إذا (!پڇاڙيون_excmd2(m->ocm_init, اظهار))
	    {
		semsg(_(e_trailing_characters_str), اظهار);
		أرجع FAIL;
	    }

	    type_T	*type = get_type_on_ڍير(cctx, 0);
	    إذا (m->ocm_type->tt_type == VAR_ANY
		    && !(m->ocm_flags & OCMFLAG_HAS_TYPE)
		    && type->tt_type != VAR_SPECIAL)
	    {
		// If the member variable type is not yet set, then use
		// the initialization expression type.
		m->ocm_type = type;
	    }
	    وإلا
	    {
		// The type of the member initialization expression is
		// determined at run time.  Add a runtime type check.
		where_T	where = WHERE_INIT;
		where.wt_قسم = WT_MEMBER;
		where.wt_func_name = (حرف *)m->ocm_name.string;
		إذا (need_type_where(type, m->ocm_type, 0, -1,
					where, cctx, FALSE, FALSE) == FAIL)
		    أرجع FAIL;
	    }
	}
	وإلا
	    ڌڪو_default_قدر(cctx, m->ocm_type->tt_type, FALSE, عدم);

	إذا (((m->ocm_type->tt_type == VAR_DICT
			|| m->ocm_type->tt_type == VAR_LIST)
		    && m->ocm_type->tt_member != عدم
		    && m->ocm_type->tt_member != &t_any
		    && m->ocm_type->tt_member != &t_unknown)
		|| (m->ocm_type->tt_type == VAR_TUPLE
		    && m->ocm_type->tt_argcount > 0))
	    // Set the type in the list, tuple or dict, so that it can be
	    // checked, also in legacy script.
	    generate_SETTYPE(cctx, m->ocm_type);

	generate_STORE_THIS(cctx, i);
    }

    أرجع OK;
}

/*
 * For an object method and an constructor, generate instruction to setup
 * "this" (the first local variable).  For a constructor, generate instructions
 * to initialize the object variables.
 */
    ساكن صحيح
obj_طريقو_prologue(ufunc_T *ufunc, cctx_T *cctx)
{
    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_معلومات) + ufunc->uf_dfunc_idx;

    إذا (GA_GROW_FAILS(&dfunc->df_var_نالا, 1))
	أرجع FAIL;

    ((char_u **)dfunc->df_var_نالا.ga_معلومات)[0] =
						vim_strsave((char_u *)"this");
    ++dfunc->df_var_نالا.ga_len;

    // In the constructor allocate memory for the object and initialize the
    // object members.
    إذا (IS_CONSTRUCTOR_طريقو(ufunc))
	أرجع obj_constructor_prologue(ufunc, cctx);

    أرجع OK;
}

/*
 * Produce instructions for the default values of optional arguments.
 */
    ساكن صحيح
compile_def_function_default_دليلون(
    ufunc_T	*ufunc,
    garray_T	*instr,
    cctx_T	*cctx)
{
    صحيح	ڳڻپ = ufunc->uf_def_دليلون.ga_len;
    صحيح	پهريون_def_arg = ufunc->uf_دليلون.ga_len - ڳڻپ;
    صحيح	i;
    صحيح	off = ڍير_چوکٺو_سائيز + (ufunc->uf_va_name != عدم ? 1 : 0);
    صحيح	did_سيٽ_arg_type = FALSE;

    // Produce instructions for the default values of optional arguments.
    SOURCING_LNUM = 0;  // line number unknown
    لكل (i = 0; i < ڳڻپ; ++i)
    {
	char_u *arg = ((char_u **)(ufunc->uf_def_دليلون.ga_معلومات))[i];
	إذا (STRCMP(arg, "v:none") == 0)
	    // "arg = v:none" means the argument is optional without
	    // setting a value when the argument is missing.
	    استمر;

	type_T	*val_type;
	صحيح		arg_idx = پهريون_def_arg + i;
	where_T	where = WHERE_INIT;
	صحيح		jump_instr_idx = instr->ga_len;
	isn_T	*isn;

	// Use a JUMP_IF_ARG_SET instruction to skip if the value was given.
	إذا (generate_JUMP_IF_ARG(cctx, ISN_JUMP_IF_ARG_سيٽ,
						i - ڳڻپ - off) == FAIL)
	    أرجع FAIL;

	// Make sure later arguments are not found.
	ufunc->uf_دليلون_visible = arg_idx;

	صحيح r = compile_expr0(&arg, cctx);
	إذا (r == FAIL)
	    أرجع FAIL;

	// If no type specified use the type of the default value.
	// Otherwise check that the default value type matches the
	// specified type.
	val_type = get_type_on_ڍير(cctx, 0);
	where.wt_اشاريو = arg_idx + 1;
	where.wt_قسم = WT_دليل;
	إذا (ufunc->uf_arg_types[arg_idx] == &t_unknown)
	{
	    did_سيٽ_arg_type = TRUE;
	    ufunc->uf_arg_types[arg_idx] = val_type;
	}
	وإلا إذا (need_type_where(val_type, ufunc->uf_arg_types[arg_idx],
		    0, -1, where, cctx, FALSE, FALSE) == FAIL)
	    أرجع FAIL;

	إذا (generate_STORE(cctx, ISN_STORE, i - ڳڻپ - off, عدم) == FAIL)
	    أرجع FAIL;

	// set instruction index in JUMP_IF_ARG_SET to here
	isn = ((isn_T *)instr->ga_معلومات) + jump_instr_idx;
	isn->isn_arg.jumparg.jump_where = instr->ga_len;
    }

    إذا (did_سيٽ_arg_type)
	سيٽ_function_type(ufunc);

    أرجع OK;
}

/*
 * Compile def function body.  Loop over all the lines in the function and
 * generate instructions.
 */
    ساكن صحيح
compile_def_function_body(
    صحيح		آخري_func_lnum,
    صحيح		check_return_type,
    garray_T	*سٽون_منزل_free,
    حرف	**errormsg,
    cctx_T	*cctx)
{
    char_u	*سٽ = عدم;
    char_u	*p;
    صحيح		did_emsg_before = did_emsg;
#جيڪڏهن_وصف_ٿيل FEAT_PROFILE
    صحيح		prof_lnum = -1;
#جيڪڏهن_پورو
    صحيح		debug_lnum = -1;

    لكل (;;)
    {
	exarg_T	    ea;
	صحيح	    starts_with_colon = FALSE;
	char_u	    *cmd;
	cmdmod_T    local_cmdmod;

	// Bail out on the first error to avoid a flood of errors and report
	// the right line number when inside try/catch.
	إذا (did_emsg_before != did_emsg)
	    أرجع FAIL;

	إذا (سٽ != عدم && *سٽ == '|')
	    // the line continues after a '|'
	    ++سٽ;
	وإلا إذا (سٽ != عدم && *skipwhite(سٽ) != NUL
		&& !(*سٽ == '#' && (سٽ == cctx->ctx_سٽ_شروعات
						    || VIM_ISWHITE(سٽ[-1]))))
	{
	    semsg(_(e_trailing_characters_str), سٽ);
	    أرجع FAIL;
	}
	وإلا إذا (سٽ != عدم && vim9_bad_comment(skipwhite(سٽ)))
	    أرجع FAIL;
	وإلا
	{
	    سٽ = ايندڙ_سٽ_from_context(cctx, FALSE);
	    إذا (cctx->ctx_lnum >= آخري_func_lnum)
	    {
		// beyond the last line
#جيڪڏهن_وصف_ٿيل FEAT_PROFILE
		إذا (cctx->ctx_skip != SKIP_YES)
		    may_generate_prof_پڇاڙي(cctx, prof_lnum);
#جيڪڏهن_پورو
		اكسر;
	    }
	    // Make a copy, splitting off nextcmd and removing trailing spaces
	    // may change it.
	    إذا (سٽ != عدم)
	    {
		سٽ = vim_strsave(سٽ);
		إذا (ga_جوڙيو_string(سٽون_منزل_free, سٽ) == FAIL)
		{
		    vim_free(سٽ);
		    أرجع FAIL;
		}
	    }
	}

	CLEAR_ميدان(ea);
	ea.cmdlinep = &سٽ;
	ea.cmd = skipwhite(سٽ);
	ea.skip = cctx->ctx_skip == SKIP_YES;

	إذا (*ea.cmd == '#')
	{
	    // "#" starts a comment, but "#{" is an error
	    إذا (vim9_bad_comment(ea.cmd))
		أرجع FAIL;
	    سٽ = (char_u *)"";
	    استمر;
	}

#جيڪڏهن_وصف_ٿيل FEAT_PROFILE
	إذا (cctx->ctx_compile_type == CT_PROFILE && cctx->ctx_lnum != prof_lnum
						  && cctx->ctx_skip != SKIP_YES)
	{
	    may_generate_prof_پڇاڙي(cctx, prof_lnum);

	    prof_lnum = cctx->ctx_lnum;
	    generate_instr(cctx, ISN_PROF_شروعات);
	}
#جيڪڏهن_پورو
	إذا (cctx->ctx_compile_type == CT_DEBUG && cctx->ctx_lnum != debug_lnum
						  && cctx->ctx_skip != SKIP_YES)
	{
	    debug_lnum = cctx->ctx_lnum;
	    generate_instr_debug(cctx);
	}
	cctx->ctx_prev_lnum = cctx->ctx_lnum + 1;

	// Some things can be recognized by the first character.
	بدل (*ea.cmd)
	{
	    حالة '}':
		{
		    // "}" ends a block scope
		    scopetype_T stype = cctx->ctx_scope == عدم
					  ? NO_SCOPE : cctx->ctx_scope->se_type;

		    إذا (stype == بلاڪ_SCOPE)
		    {
			compile_endblock(cctx);
			سٽ = ea.cmd;
		    }
		    وإلا
		    {
			emsg(_(e_using_rcurly_outside_if_بلاڪ_scope));
			أرجع FAIL;
		    }
		    إذا (سٽ != عدم)
			سٽ = skipwhite(ea.cmd + 1);
		    استمر;
		}

	    حالة '{':
		// "{" starts a block scope
		// "{'a': 1}->func() is something else
		إذا (پڇاڙيون_excmd(*skipwhite(ea.cmd + 1)))
		{
		    سٽ = compile_بلاڪ(ea.cmd, cctx);
		    استمر;
		}
		اكسر;
	}

	/*
	 * COMMAND MODIFIERS
	 */
	cctx->ctx_has_cmdmod = FALSE;
	إذا (parse_command_modifiers(&ea, errormsg, &local_cmdmod, FALSE)
								       == FAIL)
	    أرجع FAIL;
	generate_cmdmods(cctx, &local_cmdmod);
	undo_cmdmod(&local_cmdmod);

	// Check if there was a colon after the last command modifier or before
	// the current position.
	لكل (p = ea.cmd; p >= سٽ; --p)
	{
	    إذا (*p == ':')
		starts_with_colon = TRUE;
	    إذا (p < ea.cmd && !VIM_ISWHITE(*p))
		اكسر;
	}

	// Skip ":call" to get to the function name, unless using :legacy
	p = ea.cmd;
	إذا (!(local_cmdmod.cmod_flags & CMOD_LEGACY))
	{
	    إذا (checkforcmd(&ea.cmd, "call", 3))
	    {
		إذا (*ea.cmd == '(')
		    // not for "call()"
		    ea.cmd = p;
		وإلا
		    ea.cmd = skipwhite(ea.cmd);
	    }

	    إذا (!starts_with_colon)
	    {
		صحيح	    سونپيو;

		// Check for assignment after command modifiers.
		سونپيو = may_compile_assignment(&ea, &سٽ, cctx);
		إذا (سونپيو == OK)
		    اذهب nextline;
		إذا (سونپيو == FAIL)
		    أرجع FAIL;
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
	إذا ((*cmd != '$' || starts_with_colon)
		&& (starts_with_colon
		    || !(*cmd == '\''
			|| (cmd[0] == '0' && cmd[1] == 'z')
			|| (cmd[0] != NUL && cmd[0] == cmd[1]
					    && (*cmd == '+' || *cmd == '-'))
			|| عدد_طريقو(cmd))))
	{
	    ea.cmd = skip_range(ea.cmd, TRUE, عدم);
	    إذا (ea.cmd > cmd)
	    {
		إذا (!starts_with_colon
				   && !(local_cmdmod.cmod_flags & CMOD_LEGACY))
		{
		    semsg(_(e_colon_required_before_range_str), cmd);
		    أرجع FAIL;
		}
		ea.addr_ڳڻپ = 1;
		إذا (پڇاڙيون_excmd2(سٽ, ea.cmd))
		{
		    // A range without a command: jump to the line.
		    generate_EXEC(cctx, ISN_EXECRANGE,
					      vim_strnsave(cmd, ea.cmd - cmd));
		    سٽ = ea.cmd;
		    اذهب nextline;
		}
	    }
	}
	p = ڳوليو_ex_command(&ea, عدم,
		starts_with_colon || (local_cmdmod.cmod_flags & CMOD_LEGACY)
						  ? عدم : شي_exists, cctx);

	إذا (p == عدم)
	{
	    إذا (cctx->ctx_skip != SKIP_YES)
		semsg(_(e_ambiguous_use_جو_user_defined_command_str), ea.cmd);
	    أرجع FAIL;
	}

	// When using ":legacy cmd" always use compile_exec().
	إذا (local_cmdmod.cmod_flags & CMOD_LEGACY)
	{
	    char_u *شروعات = ea.cmd;

	    بدل (ea.cmdidx)
	    {
		حالة CMD_if:
		حالة CMD_elseif:
		حالة CMD_else:
		حالة CMD_endif:
		حالة CMD_for:
		حالة CMD_endfor:
		حالة CMD_continue:
		حالة CMD_break:
		حالة CMD_while:
		حالة CMD_endwhile:
		حالة CMD_try:
		حالة CMD_catch:
		حالة CMD_finally:
		حالة CMD_endtry:
			semsg(_(e_cannot_use_legacy_with_command_str), ea.cmd);
			أرجع FAIL;
		افتراضي: اكسر;
	    }

	    // ":legacy return expr" needs to be handled differently.
	    إذا (checkforcmd(&شروعات, "return", 4))
		ea.cmdidx = CMD_return;
	    وإلا
		ea.cmdidx = CMD_legacy;
	}

	إذا (p == ea.cmd && ea.cmdidx != CMD_سائيز)
	{
	    // "eval" is used for "val->func()" and "var" for "var = val", then
	    // "p" is equal to "ea.cmd" for a valid command.
	    إذا (ea.cmdidx == CMD_eval || ea.cmdidx == CMD_var)
		;
	    وإلا إذا (cctx->ctx_skip == SKIP_YES)
	    {
		سٽ += STRLEN(سٽ);
		اذهب nextline;
	    }
	    وإلا
	    {
		semsg(_(e_command_not_recognized_str), ea.cmd);
		أرجع FAIL;
	    }
	}

	إذا ((cctx->ctx_had_return || cctx->ctx_had_throw)
		&& ea.cmdidx != CMD_elseif
		&& ea.cmdidx != CMD_else
		&& ea.cmdidx != CMD_endif
		&& ea.cmdidx != CMD_endfor
		&& ea.cmdidx != CMD_endwhile
		&& ea.cmdidx != CMD_catch
		&& ea.cmdidx != CMD_finally
		&& ea.cmdidx != CMD_endtry
		&& !ignore_unreachable_ڪوڊ_for_testing)
	{
	    semsg(_(e_unreachable_ڪوڊ_after_str),
				     cctx->ctx_had_return ? "return" : "throw");
	    أرجع FAIL;
	}

	// When processing the end of an if-else or try-catch block, don't
	// clear the "ctx_had_throw" flag.  If such a block ends in a "throw"
	// statement, then it is considered to end in a "return" statement.
	// The "ctx_had_throw" is cleared immediately after processing the
	// block ending statement.
	// Otherwise, clear the "had_throw" flag.
	إذا (ea.cmdidx != CMD_else && ea.cmdidx != CMD_elseif
						&& ea.cmdidx != CMD_endif
						&& ea.cmdidx != CMD_catch
						&& ea.cmdidx != CMD_finally
						&& ea.cmdidx != CMD_endtry)
	    cctx->ctx_had_throw = FALSE;

	p = skipwhite(p);
	إذا (ea.cmdidx != CMD_سائيز
			    && ea.cmdidx != CMD_write && ea.cmdidx != CMD_read)
	{
	    إذا (ea.cmdidx >= 0)
		ea.argt = excmd_get_argt(ea.cmdidx);
	    إذا ((ea.argt & EX_BANG) && *p == '!')
	    {
		ea.forceit = TRUE;
		p = skipwhite(p + 1);
	    }
	    إذا ((ea.argt & EX_RANGE) == 0 && ea.addr_ڳڻپ > 0)
	    {
		emsg(_(e_no_range_allowed));
		أرجع FAIL;
	    }
	}

	بدل (ea.cmdidx)
	{
	    حالة CMD_def:
	    حالة CMD_function:
		    ea.arg = p;
		    سٽ = compile_nested_function(&ea, cctx, سٽون_منزل_free);
		    اكسر;

	    حالة CMD_return:
		    سٽ = compile_return(p, check_return_type,
				 local_cmdmod.cmod_flags & CMOD_LEGACY, cctx);
		    cctx->ctx_had_return = TRUE;
		    اكسر;

	    حالة CMD_let:
		    emsg(_(e_cannot_use_let_اندريون_vim9_script));
		    اكسر;
	    حالة CMD_var:
	    حالة CMD_final:
	    حالة CMD_const:
	    حالة CMD_increment:
	    حالة CMD_decrement:
		    سٽ = compile_assignment(p, &ea, ea.cmdidx, cctx);
		    إذا (سٽ == p)
		    {
			emsg(_(e_invalid_assignment));
			سٽ = عدم;
		    }
		    اكسر;

	    حالة CMD_unlet:
	    حالة CMD_unlockvar:
	    حالة CMD_lockvar:
		    سٽ = compile_unletlock(p, &ea, cctx);
		    اكسر;

	    حالة CMD_import:
		    emsg(_(e_import_can_only_be_استعمال_ٿيل_اندريون_script));
		    سٽ = عدم;
		    اكسر;

	    حالة CMD_if:
		    سٽ = compile_if(p, cctx);
		    اكسر;
	    حالة CMD_elseif:
		    سٽ = compile_elseif(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    cctx->ctx_had_throw = FALSE;
		    اكسر;
	    حالة CMD_else:
		    سٽ = compile_else(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    cctx->ctx_had_throw = FALSE;
		    اكسر;
	    حالة CMD_endif:
		    سٽ = compile_endif(p, cctx);
		    cctx->ctx_had_throw = FALSE;
		    اكسر;

	    حالة CMD_while:
		    سٽ = compile_while(p, cctx);
		    اكسر;
	    حالة CMD_endwhile:
		    سٽ = compile_endwhile(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    اكسر;

	    حالة CMD_for:
		    سٽ = compile_for(p, cctx);
		    اكسر;
	    حالة CMD_endfor:
		    سٽ = compile_endfor(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    اكسر;
	    حالة CMD_continue:
		    سٽ = compile_continue(p, cctx);
		    // "continue" unconditionally jumps back to the loop,
		    // like a "return".
		    cctx->ctx_had_return = TRUE;
		    اكسر;
	    حالة CMD_break:
		    سٽ = compile_break(p, cctx);
		    // "break" unconditionally exits the loop, like a "return".
		    cctx->ctx_had_return = TRUE;
		    اكسر;

	    حالة CMD_try:
		    سٽ = compile_try(p, cctx);
		    اكسر;
	    حالة CMD_catch:
		    سٽ = compile_catch(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    cctx->ctx_had_throw = FALSE;
		    اكسر;
	    حالة CMD_finally:
		    سٽ = compile_finally(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    cctx->ctx_had_throw = FALSE;
		    اكسر;
	    حالة CMD_endtry:
		    سٽ = compile_endtry(p, cctx);
		    cctx->ctx_had_throw = FALSE;
		    اكسر;
	    حالة CMD_throw:
		    سٽ = compile_throw(p, cctx);
		    cctx->ctx_had_throw = TRUE;
		    اكسر;

	    حالة CMD_eval:
		    سٽ = compile_eval(p, cctx);
		    اكسر;

	    حالة CMD_defer:
		    سٽ = compile_defer(p, cctx);
		    اكسر;

#جيڪڏهن_وصف_ٿيل HAS_پيغام_دري
	    حالة CMD_echowindow:
		    {
			ڊگهو cmd_ڳڻپ = get_cmd_ڳڻپ(سٽ, &ea);
			إذا (cmd_ڳڻپ < 0)
			    سٽ = عدم;
			وإلا
			    سٽ = compile_mult_اظهار(p, ea.cmdidx,
							     cmd_ڳڻپ, cctx);
		    }
		    اكسر;
#جيڪڏهن_پورو
	    حالة CMD_echo:
	    حالة CMD_echon:
	    حالة CMD_echoconsole:
	    حالة CMD_echoerr:
	    حالة CMD_echomsg:
	    حالة CMD_execute:
		    سٽ = compile_mult_اظهار(p, ea.cmdidx, 0, cctx);
		    اكسر;

	    حالة CMD_put:
		    ea.cmd = cmd;
		    سٽ = compile_put(p, &ea, cctx, FALSE);
		    اكسر;

	    حالة CMD_iput:
		    ea.cmd = cmd;
		    سٽ = compile_put(p, &ea, cctx, TRUE);
		    اكسر;

	    حالة CMD_substitute:
		    إذا (check_global_and_subst(ea.cmd, p) == FAIL)
			أرجع FAIL;
		    إذا (cctx->ctx_skip == SKIP_YES)
			سٽ = (char_u *)"";
		    وإلا
		    {
			ea.arg = p;
			سٽ = compile_substitute(سٽ, &ea, cctx);
		    }
		    اكسر;

	    حالة CMD_redir:
		    ea.arg = p;
		    سٽ = compile_redir(سٽ, &ea, cctx);
		    اكسر;

	    حالة CMD_cexpr:
	    حالة CMD_lexpr:
	    حالة CMD_caddexpr:
	    حالة CMD_laddexpr:
	    حالة CMD_cgetexpr:
	    حالة CMD_lgetexpr:
#جيڪڏهن_وصف_ٿيل FEAT_QUICKFIX
		    ea.arg = p;
		    سٽ = compile_cexpr(سٽ, &ea, cctx);
#وإلا
		    ex_ni(&ea);
		    سٽ = عدم;
#جيڪڏهن_پورو
		    اكسر;

	    حالة CMD_append:
	    حالة CMD_change:
	    حالة CMD_وجهو:
	    حالة CMD_k:
	    حالة CMD_open:
	    حالة CMD_t:
	    حالة CMD_xit:
		    not_اندريون_vim9(&ea, false);
		    أرجع FAIL;

	    حالة CMD_سائيز:
		    إذا (cctx->ctx_skip != SKIP_YES)
		    {
			semsg(_(e_invalid_command_str), ea.cmd);
			أرجع FAIL;
		    }
		    // We don't check for a next command here.
		    سٽ = (char_u *)"";
		    اكسر;

	    حالة CMD_lua:
	    حالة CMD_mzscheme:
	    حالة CMD_perl:
	    حالة CMD_py3:
	    حالة CMD_python3:
	    حالة CMD_python:
	    حالة CMD_pythonx:
	    حالة CMD_ruby:
	    حالة CMD_tcl:
		    ea.arg = p;
		    إذا (vim_strchr(سٽ, '\n') == عدم)
			سٽ = compile_exec(سٽ, &ea, cctx);
		    وإلا
			// heredoc lines have been concatenated with NL
			// characters in get_function_body()
			سٽ = compile_script(سٽ, cctx);
		    اكسر;

	    حالة CMD_vim9script:
		    إذا (cctx->ctx_skip != SKIP_YES)
		    {
			emsg(_(e_vim9script_can_only_be_استعمال_ٿيل_اندريون_script));
			أرجع FAIL;
		    }
		    سٽ = (char_u *)"";
		    اكسر;

	    حالة CMD_class:
		    emsg(_(e_class_can_only_be_استعمال_ٿيل_اندريون_script));
		    أرجع FAIL;

	    حالة CMD_enum:
		    emsg(_(e_enum_can_only_be_استعمال_ٿيل_اندريون_script));
		    أرجع FAIL;

	    حالة CMD_interface:
		    emsg(_(e_interface_can_only_be_استعمال_ٿيل_اندريون_script));
		    أرجع FAIL;

	    حالة CMD_type:
		    emsg(_(e_type_can_only_be_استعمال_ٿيل_اندريون_script));
		    أرجع FAIL;

	    حالة CMD_global:
		    إذا (check_global_and_subst(ea.cmd, p) == FAIL)
			أرجع FAIL;
		    // FALLTHROUGH
	    افتراضي:
		    // Not recognized, execute with do_cmdline_cmd().
		    ea.arg = p;
		    سٽ = compile_exec(سٽ, &ea, cctx);
		    اكسر;
	}
nextline:
	إذا (سٽ == عدم)
	    أرجع FAIL;
	سٽ = skipwhite(سٽ);

	// Undo any command modifiers.
	generate_undo_cmdmods(cctx);

	إذا (cctx->ctx_type_ڍير.ga_len < 0)
	{
	    iemsg("Type stack underflow");
	    أرجع FAIL;
	}
    } // END of the loop over all the function body lines.

    أرجع OK;
}

/*
 * Returns TRUE if the end of a scope (if, while, for, block) is missing.
 * Called after compiling a def function body.
 */
    ساكن صحيح
compile_dfunc_scope_پڇاڙي_missing(cctx_T *cctx)
{
    إذا (cctx->ctx_scope == عدم)
	أرجع FALSE;

    بدل (cctx->ctx_scope->se_type)
    {
	حالة IF_SCOPE:
	    emsg(_(e_missing_endif));
	    اكسر;
	حالة WHILE_SCOPE:
	    emsg(_(e_missing_endwhile));
	    اكسر;
	حالة FOR_SCOPE:
	    emsg(_(e_missing_endfor));
	    اكسر;
	حالة TRY_SCOPE:
	    emsg(_(e_missing_endtry));
	    اكسر;
	حالة بلاڪ_SCOPE:
	    // end block scope from :try (maybe)
	    compile_endblock(cctx);
	    إذا (cctx->ctx_scope != عدم
		    && cctx->ctx_scope->se_type == TRY_SCOPE)
		emsg(_(e_missing_endtry));
	    وإلا
		emsg(_(e_missing_rcurly));
	    اكسر;
	افتراضي:
	    emsg(_(e_missing_rcurly));
    }
    أرجع TRUE;
}

/*
 * When compiling a def function, if it doesn't have an explicit return
 * statement, then generate a default return instruction.  For an object
 * constructor, return the object.
 */
    ساكن صحيح
compile_dfunc_generate_default_return(ufunc_T *ufunc, cctx_T *cctx)
{
    // TODO: if a function ends in "throw" but there was a return elsewhere we
    // should not assume the return type is "void".
    إذا (cctx->ctx_had_return || cctx->ctx_had_throw)
	أرجع OK;

    إذا (ufunc->uf_ret_type->tt_type == VAR_UNKNOWN)
	ufunc->uf_ret_type = &t_void;
    وإلا إذا (ufunc->uf_ret_type->tt_type != VAR_VOID
					&& !IS_CONSTRUCTOR_طريقو(ufunc))
    {
	emsg(_(e_missing_return_statement));
	أرجع FAIL;
    }

    // Return void if there is no return at the end.
    // For a constructor return the object.
    إذا (IS_CONSTRUCTOR_طريقو(ufunc))
    {
	generate_instr(cctx, ISN_RETURN_OBJECT);
	ufunc->uf_ret_type = &ufunc->uf_class->class_object_type;
    }
    وإلا
	generate_instr(cctx, ISN_RETURN_VOID);

    أرجع OK;
}

/*
 * Perform the chores after successfully compiling a def function.
 */
    ساكن فراغ
compile_dfunc_epilogue(
    cctx_T	*outer_cctx,
    ufunc_T	*ufunc,
    garray_T	*instr,
    cctx_T	*cctx)
{
    dfunc_T	*dfunc;

    dfunc = ((dfunc_T *)def_functions.ga_معلومات) + ufunc->uf_dfunc_idx;
    dfunc->df_deleted = FALSE;
    dfunc->df_script_seq = current_sctx.sc_seq;

#جيڪڏهن_وصف_ٿيل FEAT_PROFILE
    إذا (cctx->ctx_compile_type == CT_PROFILE)
    {
	dfunc->df_instr_prof = instr->ga_معلومات;
	dfunc->df_instr_prof_ڳڻپ = instr->ga_len;
    }
    وإلا
#جيڪڏهن_پورو
	إذا (cctx->ctx_compile_type == CT_DEBUG)
	{
	    dfunc->df_instr_debug = instr->ga_معلومات;
	    dfunc->df_instr_debug_ڳڻپ = instr->ga_len;
	}
	وإلا
	{
	    dfunc->df_instr = instr->ga_معلومات;
	    dfunc->df_instr_ڳڻپ = instr->ga_len;
	}
    dfunc->df_varcount = dfunc->df_var_نالا.ga_len;
    dfunc->df_has_گهيرو = cctx->ctx_has_گهيرو;

    إذا (cctx->ctx_outer_استعمال_ٿيل)
    {
	ufunc->uf_flags |= FC_گهيرو;
	إذا (outer_cctx != عدم)
	    ++outer_cctx->ctx_گهيرو_ڳڻپ;
    }

    ufunc->uf_def_حالت = UF_COMPILED;
}

/*
 * Perform the cleanup when a def function compilation fails.
 */
    ساكن فراغ
compile_dfunc_ufunc_cleanup(
    ufunc_T	*ufunc,
    garray_T	*instr,
    صحيح		new_def_function,
    حرف	*errormsg,
    صحيح		did_emsg_before,
    cctx_T	*cctx)
{
    dfunc_T	*dfunc;

    dfunc = ((dfunc_T *)def_functions.ga_معلومات) + ufunc->uf_dfunc_idx;

    // Compiling aborted, free the generated instructions.
    clear_instr_ga(instr);
    VIM_CLEAR(dfunc->df_name);
    ga_clear_لڙيون(&dfunc->df_var_نالا);

    // If using the last entry in the table and it was added above, we
    // might as well remove it.
    إذا (!dfunc->df_deleted && new_def_function
			&& ufunc->uf_dfunc_idx == def_functions.ga_len - 1)
    {
	--def_functions.ga_len;
	ufunc->uf_dfunc_idx = 0;
    }
    ufunc->uf_def_حالت = UF_COMPILE_ERROR;

    طالما (cctx->ctx_scope != عدم)
	drop_scope(cctx);

    إذا (errormsg != عدم)
	emsg(errormsg);
    وإلا إذا (did_emsg == did_emsg_before)
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
    صحيح
compile_def_function(
	ufunc_T		*ufunc,
	صحيح		check_return_type,
	compiletype_T   compile_type,
	cctx_T		*outer_cctx)
{
    garray_T	سٽون_منزل_free;
    حرف	*errormsg = عدم;	// error message
    cctx_T	cctx;
    garray_T	*instr;
    صحيح		did_emsg_before = did_emsg;
    صحيح		did_emsg_silent_before = did_emsg_silent;
    صحيح		ret = FAIL;
    sctx_T	save_current_sctx = current_sctx;
    صحيح		save_estack_compiling = estack_compiling;
    صحيح		save_cmod_flags = cmdmod.cmod_flags;
    صحيح		do_estack_ڌڪو;
    صحيح		new_def_function = FALSE;

    // allocated lines are freed at the end
    ga_init2(&سٽون_منزل_free, حجم(char_u *), 50);

    // Initialize the ufunc and the compilation context
    إذا (compile_dfunc_ufunc_init(ufunc, outer_cctx, compile_type,
						&new_def_function) == FAIL)
	أرجع FAIL;

    compile_dfunc_cctx_init(&cctx, outer_cctx, ufunc, compile_type);

    instr = &cctx.ctx_instr;

    // Set the context to the function, it may be compiled when called from
    // another script.  Set the script version to the most modern one.
    // The line number will be set in next_line_from_context().
    current_sctx = ufunc->uf_script_ctx;
    current_sctx.sc_نسخو = SCRIPT_نسخو_VIM9;

    // Don't use the flag from ":legacy" here.
    cmdmod.cmod_flags &= ~CMOD_LEGACY;

    // Make sure error messages are OK.
    do_estack_ڌڪو = !estack_چوٽي_is_ufunc(ufunc, 1);
    إذا (do_estack_ڌڪو)
	estack_ڌڪو_ufunc(ufunc, 1);
    estack_compiling = TRUE;

    // Make sure arguments don't shadow variables in the context
    إذا (check_دليلون_shadowing(ufunc, &cctx) == FAIL)
	اذهب erret;

    // For an object method and a constructor generate instructions to
    // initialize "this" and the object variables.
    إذا (ufunc->uf_flags & (FC_OBJECT|FC_NEW))
	إذا (obj_طريقو_prologue(ufunc, &cctx) == FAIL)
	    اذهب erret;

    إذا (ufunc->uf_def_دليلون.ga_len > 0)
	إذا (compile_def_function_default_دليلون(ufunc, instr, &cctx) == FAIL)
	    اذهب erret;
    ufunc->uf_دليلون_visible = ufunc->uf_دليلون.ga_len;

    // Compiling an abstract method or a function in an interface is done to
    // get the function type.  No code is actually compiled.
    إذا (ufunc->uf_class != عدم && (IS_INTERFACE(ufunc->uf_class)
						|| IS_ABSTRACT_طريقو(ufunc)))
    {
	ufunc->uf_def_حالت = UF_NOT_COMPILED;
	ret = OK;
	اذهب erret;
    }

    // compile the function body
    إذا (compile_def_function_body(ufunc->uf_سٽون.ga_len, check_return_type,
				&سٽون_منزل_free, &errormsg, &cctx) == FAIL)
	اذهب erret;

    إذا (compile_dfunc_scope_پڇاڙي_missing(&cctx))
	اذهب erret;

    إذا (compile_dfunc_generate_default_return(ufunc, &cctx) == FAIL)
	اذهب erret;

    // When compiled with ":silent!" and there was an error don't consider the
    // function compiled.
    إذا (emsg_silent == 0 || did_emsg_silent == did_emsg_silent_before)
	compile_dfunc_epilogue(outer_cctx, ufunc, instr, &cctx);

    ret = OK;

erret:
    إذا (ufunc->uf_def_حالت == UF_COMPILING)
    {
	// compilation failed. do cleanup.
	compile_dfunc_ufunc_cleanup(ufunc, instr, new_def_function,
				    errormsg, did_emsg_before, &cctx);
    }

    إذا (cctx.ctx_redir_lhs.lhs_name != عدم)
    {
	إذا (ret == OK)
	{
	    emsg(_(e_missing_redir_پڇاڙي));
	    ret = FAIL;
	}
	vim_free(cctx.ctx_redir_lhs.lhs_name);
	vim_free(cctx.ctx_redir_lhs.lhs_سڄو);
    }

    current_sctx = save_current_sctx;
    estack_compiling = save_estack_compiling;
    cmdmod.cmod_flags =	save_cmod_flags;
    إذا (do_estack_ڌڪو)
	estack_ڪڍو();

    ga_clear_لڙيون(&سٽون_منزل_free);
    free_locals(&cctx);
    ga_clear(&cctx.ctx_type_ڍير);
    أرجع ret;
}

    فراغ
سيٽ_function_type(ufunc_T *ufunc)
{
    صحيح varargs = ufunc->uf_va_name != عدم;
    صحيح argcount = ufunc->uf_دليلون.ga_len;

    // Create a type for the function, with the return type and any
    // argument types.
    // A vararg is included in uf_args.ga_len but not in uf_arg_types.
    // The type is included in "tt_args".
    إذا (argcount > 0 || varargs)
    {
	إذا (ufunc->uf_type_list.ga_itemsize == 0)
	    ga_init2(&ufunc->uf_type_list, حجم(type_T *), 10);
	ufunc->uf_func_type = alloc_func_type(ufunc->uf_ret_type,
					   argcount, &ufunc->uf_type_list);
	// Add argument types to the function type.
	إذا (func_type_جوڙيو_arg_types(ufunc->uf_func_type,
				    argcount + varargs,
				    &ufunc->uf_type_list) == FAIL)
	    أرجع;
	ufunc->uf_func_type->tt_argcount = argcount + varargs;
	ufunc->uf_func_type->tt_min_argcount =
				      argcount - ufunc->uf_def_دليلون.ga_len;
	إذا (ufunc->uf_arg_types == عدم)
	{
	    صحيح i;

	    // lambda does not have argument types.
	    لكل (i = 0; i < argcount; ++i)
		ufunc->uf_func_type->tt_دليلون[i] = &t_any;
	}
	وإلا
	    mch_memmove(ufunc->uf_func_type->tt_دليلون,
			 ufunc->uf_arg_types, حجم(type_T *) * argcount);
	إذا (varargs)
	{
	    ufunc->uf_func_type->tt_دليلون[argcount] =
		   ufunc->uf_va_type == عدم ? &t_list_any : ufunc->uf_va_type;
	    ufunc->uf_func_type->tt_flags = TTFLAG_VARARGS;
	}
    }
    وإلا
	// No arguments, can use a predefined type.
	ufunc->uf_func_type = get_func_type(ufunc->uf_ret_type,
					   argcount, &ufunc->uf_type_list);
}

/*
 * Free all instructions for "dfunc" except df_name.
 */
    ساكن فراغ
delete_def_function_contents(dfunc_T *dfunc, صحيح mark_deleted)
{
    صحيح idx;

    // In same cases the instructions may refer to a class in which the
    // function is defined and unreferencing the class may call back here
    // recursively.  Set the df_delete_busy to avoid problems.
    إذا (dfunc->df_delete_busy)
	أرجع;
    dfunc->df_delete_busy = TRUE;

    ga_clear(&dfunc->df_def_دليلون_isn);
    ga_clear_لڙيون(&dfunc->df_var_نالا);

    إذا (dfunc->df_instr != عدم)
    {
	لكل (idx = 0; idx < dfunc->df_instr_ڳڻپ; ++idx)
	    delete_instr(dfunc->df_instr + idx);
	VIM_CLEAR(dfunc->df_instr);
    }
    إذا (dfunc->df_instr_debug != عدم)
    {
	لكل (idx = 0; idx < dfunc->df_instr_debug_ڳڻپ; ++idx)
	    delete_instr(dfunc->df_instr_debug + idx);
	VIM_CLEAR(dfunc->df_instr_debug);
    }
#جيڪڏهن_وصف_ٿيل FEAT_PROFILE
    إذا (dfunc->df_instr_prof != عدم)
    {
	لكل (idx = 0; idx < dfunc->df_instr_prof_ڳڻپ; ++idx)
	    delete_instr(dfunc->df_instr_prof + idx);
	VIM_CLEAR(dfunc->df_instr_prof);
    }
#جيڪڏهن_پورو

    إذا (mark_deleted)
	dfunc->df_deleted = TRUE;
    إذا (dfunc->df_ufunc != عدم)
	dfunc->df_ufunc->uf_def_حالت = UF_NOT_COMPILED;

    dfunc->df_delete_busy = FALSE;
}

/*
 * When a user function is deleted, clear the contents of any associated def
 * function, unless another user function still uses it.
 * The position in def_functions can be re-used.
 */
    فراغ
unlink_def_function(ufunc_T *ufunc)
{
    إذا (ufunc->uf_dfunc_idx <= 0 || def_functions.ga_معلومات == عدم)
	أرجع;

    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_معلومات)
	+ ufunc->uf_dfunc_idx;

    إذا (--dfunc->df_refcount <= 0)
	delete_def_function_contents(dfunc, TRUE);
    ufunc->uf_def_حالت = UF_NOT_COMPILED;
    ufunc->uf_dfunc_idx = 0;
    إذا (dfunc->df_ufunc == ufunc)
	dfunc->df_ufunc = عدم;
}

/*
 * Used when a user function refers to an existing dfunc.
 */
    فراغ
link_def_function(ufunc_T *ufunc)
{
    إذا (ufunc->uf_dfunc_idx <= 0)
	أرجع;

    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_معلومات)
	+ ufunc->uf_dfunc_idx;

    ++dfunc->df_refcount;
}

#إذا defined(EXITFREE)
/*
 * Free all functions defined with ":def".
 */
    فراغ
free_def_functions(فراغ)
{
    صحيح idx;

    لكل (idx = 0; idx < def_functions.ga_len; ++idx)
    {
	dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_معلومات) + idx;

	delete_def_function_contents(dfunc, TRUE);
	vim_free(dfunc->df_name);
    }

    ga_clear(&def_functions);
}
#جيڪڏهن_پورو


#جيڪڏهن_پورو // FEAT_EVAL
