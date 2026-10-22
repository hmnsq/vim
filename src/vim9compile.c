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

#تعریف_کرو USING_FLOAT_STUFF
#شامل "vim.h"

#اگر defined(FEAT_EVAL)

// Functions defined with :def are stored in this growarray.
// They are never removed, so that they can be found by index.
// Deleted functions have the df_deleted flag set.
garray_T def_functions = {0, 0, حجم(dfunc_T), 50, تہی};

ساکن خالی delete_def_function_contents(dfunc_T *dfunc, صحیح mark_deleted);

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
	واپس FAIL;

    اگر (((len == 4 && STRNCMP(name, "this", 4) == 0)
		|| (len == 5 && STRNCMP(name, "super", 5) == 0))
	    && cctx->ctx_ufunc != تہی
	    && (cctx->ctx_ufunc->uf_flags & (FC_OBJECT|FC_NEW)))
    {
	صحیح is_super = *name == 's';
	اگر (is_super)
	{
	    اگر (name[5] != '.')
	    {
		emsg(_(e_super_must_be_followed_by_dot));
		واپس FAIL;
	    }
	    اگر (cctx->ctx_ufunc->uf_class != تہی
		    && cctx->ctx_ufunc->uf_class->class_extends == تہی)
	    {
		emsg(_(e_using_super_not_اندر_بچہ_class));
		واپس FAIL;
	    }
	}
	اگر (lvar != تہی)
	{
	    CLEAR_اشارہ_گر(lvar);
	    lvar->lv_loop_گہرائی = -1;
	    lvar->lv_name = (char_u *)(is_super ? "super" : "this");
	    اگر (cctx->ctx_ufunc->uf_class != تہی)
	    {
		lvar->lv_type = &cctx->ctx_ufunc->uf_class->class_object_type;
		اگر (is_super)
		{
		    type_T *type = get_type_ptr(cctx->ctx_type_list);

		    اگر (type != تہی)
		    {
			*type = *lvar->lv_type;
			lvar->lv_type = type;
			type->tt_flags |= TTFLAG_SUPER;
		    }
		}
	    }
	}
	واپس OK;
    }

    // Find local in current function scope.
    برائے (idx = 0; idx < cctx->ctx_locals.ga_len; ++idx)
    {
	lvp = ((lvar_T *)cctx->ctx_locals.ga_ڈیٹا) + idx;
	اگر (lvp->lv_name != تہی
		&& STRNCMP(name, lvp->lv_name, len) == 0
					       && STRLEN(lvp->lv_name) == len)
	{
	    اگر (lvar != تہی)
	    {
		*lvar = *lvp;
		lvar->lv_from_outer = 0;
		// If the variable was declared inside a loop set
		// lvar->lv_loop_idx and lvar->lv_loop_depth.
		get_loop_var_idx(cctx, idx, lvar);
	    }
	    واپس OK;
	}
    }

    // Find local in outer function scope.
    اگر (cctx->ctx_outer != تہی)
    {
	اگر (lookup_local(name, len, lvar, cctx->ctx_outer) == OK)
	{
	    اگر (lvar != تہی)
	    {
		cctx->ctx_outer_مستعمل = TRUE;
		++lvar->lv_from_outer;
	    }
	    واپس OK;
	}
    }

    واپس FAIL;
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
	صحیح	*gen_بھرو_outer,
	cctx_T	*cctx)
{
    صحیح	    idx;
    char_u  *va_name;

    اگر (len == 0)
	واپس FAIL;
    برائے (idx = 0; idx < cctx->ctx_ufunc->uf_دلائل_visible; ++idx)
    {
	char_u *arg = FUNCARG(cctx->ctx_ufunc, idx);

	اگر (STRNCMP(name, arg, len) == 0 && arg[len] == NUL)
	{
	    اگر (idxp != تہی)
	    {
		// Arguments are located above the frame pointer.  One further
		// if there is a vararg argument
		*idxp = idx - (cctx->ctx_ufunc->uf_دلائل.ga_len
							    + ڈھیر_چوکھٹا_سائز)
			      + (cctx->ctx_ufunc->uf_va_name != تہی ? -1 : 0);

		اگر (cctx->ctx_ufunc->uf_arg_types != تہی)
		    *type = cctx->ctx_ufunc->uf_arg_types[idx];
		ورنہ
		    *type = &t_any;
	    }
	    واپس OK;
	}
    }

    va_name = cctx->ctx_ufunc->uf_va_name;
    اگر (va_name != تہی
		    && STRNCMP(name, va_name, len) == 0 && va_name[len] == NUL)
    {
	اگر (idxp != تہی)
	{
	    // varargs is always the last argument
	    *idxp = -ڈھیر_چوکھٹا_سائز - 1;
	    *type = cctx->ctx_ufunc->uf_va_type;
	}
	واپس OK;
    }

    اگر (cctx->ctx_outer != تہی)
    {
	// Lookup the name for an argument of the outer function.
	اگر (arg_exists(name, len, idxp, type, gen_بھرو_outer, cctx->ctx_outer)
									 == OK)
	{
	    اگر (gen_بھرو_outer != تہی)
	    {
		++*gen_بھرو_outer;
		cctx->ctx_outer_مستعمل = TRUE;
	    }
	    واپس OK;
	}
    }

    واپس FAIL;
}

/*
 * Lookup a script-local variable in the current script, possibly defined in a
 * block that contains the function "cctx->ctx_ufunc".
 * "cctx" is NULL at the script level, "cstack" is NULL in a function.
 * If "len" is <= 0 "name" must be NUL terminated.
 * Return NULL when not found.
 */
    ساکن sallvar_T *
ڈھونڈو_script_var(char_u *name, size_t len, cctx_T *cctx, cstack_T *cstack)
{
    scriptitem_T    *si = SCRIPT_شے(current_sctx.sc_sid);
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
    hi = hash_ڈھونڈو(&si->sn_all_vars.dv_hashtab, name);
    اگر (len > 0)
	name[len] = cc;
    اگر (HASHITEM_خالی_خانے(hi))
	واپس تہی;

    sav = HI2SAV(hi);
    اگر (sav->sav_بلاک_id == 0)
	// variable defined in the top script scope is always visible
	واپس sav;

    اگر (cctx == تہی)
    {
	اگر (cstack == تہی)
	    واپس تہی;

	// Not in a function scope, find variable with block ID equal to or
	// smaller than the current block id.  Use "cstack" to go up the block
	// scopes.
	جب (sav != تہی)
	{
	    صحیح idx;

	    برائے (idx = cstack->cs_idx; idx >= 0; --idx)
		اگر (cstack->cs_بلاک_id[idx] == sav->sav_بلاک_id)
		    توڑو;
	    اگر (idx >= 0)
		توڑو;
	    sav = sav->sav_اگلا;
	}
	واپس sav;
    }

    // Go over the variables with this name and find one that was visible
    // from the function.
    ufunc = cctx->ctx_ufunc;
    جب (sav != تہی)
    {
	صحیح idx;

	اگر (ufunc->uf_بلاک_گہرائی == 0 && sav->sav_بلاک_id == 0)
	    // If the function was defined at the script level (not inside a
	    // block), script-scope variables are always visible.
	    واپس sav;

	// Go over the blocks that this function was defined in.  If the
	// variable block ID matches it was visible to the function.
	برائے (idx = 0; idx < ufunc->uf_بلاک_گہرائی; ++idx)
	    اگر (ufunc->uf_بلاک_ids[idx] == sav->sav_بلاک_id)
		واپس sav;
	sav = sav->sav_اگلا;
    }

    // Not found, variable was not visible.
    واپس تہی;
}

/*
 * If "name" can be found in the current script set it's "block_id".
 */
    خالی
update_script_var_بلاک_id(char_u *name, صحیح بلاک_id)
{
    scriptitem_T    *si = SCRIPT_شے(current_sctx.sc_sid);
    hashitem_T	    *hi;
    sallvar_T	    *sav;

    hi = hash_ڈھونڈو(&si->sn_all_vars.dv_hashtab, name);
    اگر (HASHITEM_خالی_خانے(hi))
	واپس;
    sav = HI2SAV(hi);
    sav->sav_بلاک_id = بلاک_id;
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
	واپس FAIL;
    اگر (current_script_is_vim9())
    {
	// Check script variables that were visible where the function was
	// defined.
	اگر (ڈھونڈو_script_var(name, len, cctx, cstack) != تہی)
	    واپس OK;
    }
    ورنہ
    {
	hashtab_T	*ht = &SCRIPT_VARS(current_sctx.sc_sid);
	dictitem_T	*di;
	صحیح		cc;

	// Check script variables that are currently visible
	cc = name[len];
	name[len] = NUL;
	di = ڈھونڈو_var_اندر_ht(ht, 0, name, TRUE);
	name[len] = cc;
	اگر (di != تہی)
	    واپس OK;
    }

    واپس FAIL;
}

/*
 * Returns the index of a class method or class variable with name "name"
 * accessible in the currently compiled function.
 * If "cl_ret" is not NULL set it to the class.
 * Otherwise return -1.
 */
    ساکن صحیح
cctx_class_midx(
    cctx_T  *cctx,
    صحیح	    is_طریقہ,
    char_u  *name,
    size_t  len,
    class_T **cl_ret)
{
    اگر (cctx == تہی || cctx->ctx_ufunc == تہی
	    || cctx->ctx_ufunc->uf_class == تہی
	    || cctx->ctx_ufunc->uf_defclass == تہی)
	واپس -1;

    // Search for the class method or variable in the class where the calling
    // function is defined.
    class_T *cl = cctx->ctx_ufunc->uf_defclass;
    صحیح m_idx = is_طریقہ ? class_طریقہ_idx(cl, name, len)
					: class_member_idx(cl, name, len);
    اگر (m_idx < 0)
    {
	cl = cl->class_extends;
	جب (cl != تہی)
	{
	    m_idx = is_طریقہ ? class_طریقہ_idx(cl, name, len)
					: class_member_idx(cl, name, len);
	    اگر (m_idx >= 0)
		توڑو;
	    cl = cl->class_extends;
	}
    }

    اگر (m_idx >= 0)
    {
	اگر (cl_ret != تہی)
	    *cl_ret = cl;
    }

    واپس m_idx;
}

/*
 * Returns the index of a class method with name "name" accessible in the
 * currently compiled function.  Returns -1 if not found.  The class where the
 * method is defined is returned in "cl_ret".
 */
    صحیح
cctx_class_طریقہ_idx(
    cctx_T  *cctx,
    char_u  *name,
    size_t  len,
    class_T **cl_ret)
{
    واپس cctx_class_midx(cctx, TRUE, name, len, cl_ret);
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
    واپس cctx_class_midx(cctx, FALSE, name, len, cl_ret);
}

/*
 * Return TRUE if "name" is a local variable, argument, script variable or
 * imported.  Also if "name" is "this" and in a class method.
 */
    ساکن صحیح
variable_exists(char_u *name, size_t len, cctx_T *cctx)
{
    واپس (cctx != تہی
		&& (lookup_local(name, len, تہی, cctx) == OK
		    || arg_exists(name, len, تہی, تہی, تہی, cctx) == OK
		    || (len == 4
			&& cctx->ctx_ufunc != تہی
			&& (cctx->ctx_ufunc->uf_flags & (FC_OBJECT|FC_NEW))
			&& STRNCMP(name, "this", 4) == 0)))
	    || script_var_exists(name, len, cctx, تہی) == OK
	    || cctx_class_member_idx(cctx, name, len, تہی) >= 0
	    || ڈھونڈو_imported(name, len, FALSE) != تہی;
}

/*
 * Return TRUE if "name" is a local variable, argument, script variable,
 * imported or function.  Or commands are being skipped, a declaration may have
 * been skipped then.
 */
    ساکن صحیح
شے_exists(char_u *name, size_t len, صحیح cmd غیر_مستعمل, cctx_T *cctx)
{
    واپس variable_exists(name, len, cctx);
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
    ufunc_T	*ufunc = تہی;

    // underscore argument is OK
    اگر (len == 1 && *p == '_')
	واپس OK;

    اگر (script_var_exists(p, len, cctx, cstack) == OK)
    {
	اگر (is_arg)
	    semsg(_(e_دلیل_already_declared_اندر_script_str), p);
	ورنہ
	    semsg(_(e_variable_already_declared_اندر_script_str), p);
	واپس FAIL;
    }

    اگر (cctx_class_member_idx(cctx, p, len, تہی) >= 0)
    {
	اگر (is_arg)
	    semsg(_(e_دلیل_already_declared_اندر_class_str), p);
	ورنہ
	    semsg(_(e_variable_already_declared_اندر_class_str), p);
	واپس FAIL;
    }

    p[len] = NUL;
    اگر ((cctx != تہی
		&& (lookup_local(p, len, تہی, cctx) == OK
		    || arg_exists(p, len, تہی, تہی, تہی, cctx) == OK))
	    || ڈھونڈو_imported(p, len, FALSE) != تہی
	    || (ufunc = ڈھونڈو_func_even_dead(p, 0)) != تہی)
    {
	// A local or script-local function can shadow a global function.
	اگر (ufunc == تہی || ((ufunc->uf_flags & FC_DEAD) == 0
		    && (!func_is_global(ufunc)
					     || (p[0] == 'g' && p[1] == ':'))))
	{
	    اگر (is_arg)
		semsg(_(e_دلیل_name_shadows_existing_variable_str), p);
	    ورنہ
		semsg(_(e_name_already_defined_str), p);
	    p[len] = c;
	    واپس FAIL;
	}
    }
    p[len] = c;
    واپس OK;
}


/*
 * Return TRUE if "actual" could be "expected" and a runtime typecheck is to be
 * used.  Return FALSE if the types will never match.
 */
    ساکن صحیح
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
	واپس TRUE;
    اگر (actual->tt_type == VAR_OBJECT && expected->tt_type == VAR_OBJECT)
	واپس TRUE;
    اگر ((actual->tt_type == VAR_LIST
		|| actual->tt_type == VAR_TUPLE
		|| actual->tt_type == VAR_DICT)
	    && actual->tt_type == expected->tt_type)
	// This takes care of a nested list or dict.
	واپس use_typecheck(actual->tt_member, expected->tt_member);
    واپس FALSE;
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
	صحیح	انحراف,
	where_T	where,
	cctx_T	*cctx,
	صحیح	silent,
	صحیح	actual_is_const)
{
    صحیح ret;

    اگر (expected->tt_type != VAR_CLASS && expected->tt_type != VAR_TYPEALIAS)
    {
	اگر (check_type_is_قدر(actual) == FAIL)
	    واپس FAIL;
    }

    اگر (expected == &t_bool && actual != &t_bool
					&& (actual->tt_flags & TTFLAG_BOOL_OK))
    {
	// Using "0", "1" or the result of an expression with "&&" or "||" as a
	// boolean is OK but requires a conversion.
	generate_2BOOL(cctx, FALSE, انحراف);
	واپس OK;
    }

    ret = check_type_maybe(expected, actual, FALSE, where);
    اگر (ret == OK)
	واپس OK;

    // If actual a constant a runtime check makes no sense.  If it's
    // null_function it is OK.
    اگر (actual_is_const && ret == MAYBE && actual == &t_func_unknown)
	واپس OK;

    // If the actual type can be the expected type add a runtime check.
    اگر (!actual_is_const && ret == MAYBE && use_typecheck(actual, expected))
    {
	generate_TYPECHECK(cctx, expected, typechk_flags, انحراف,
		where.wt_قسم == WT_VARIABLE, where.wt_اشاریہ);
	واپس OK;
    }

    اگر (!silent)
	type_mismatch_where(expected, actual, where);
    واپس FAIL;
}

    صحیح
need_type(
	type_T	*actual,
	type_T	*expected,
	صحیح	typechk_flags,	// acceptable types (type check flags)
	صحیح	انحراف,
	صحیح	arg_idx,
	cctx_T	*cctx,
	صحیح	silent,
	صحیح	actual_is_const)
{
    where_T where = WHERE_INIT;

    اگر (arg_idx > 0)
    {
	where.wt_اشاریہ = arg_idx;
	where.wt_قسم = WT_دلیل;
    }
    واپس need_type_where(actual, expected, typechk_flags, انحراف, where,
						cctx, silent, actual_is_const);
}

/*
 * Set type of variable "lvar" to "type".  If the variable is a constant then
 * the type gets TTFLAG_CONST.
 */
    ساکن خالی
سیٹ_var_type(lvar_T *lvar, type_T *type_arg, cctx_T *cctx)
{
    type_T	*type = type_arg;

    اگر (lvar->lv_const == تفویض_CONST && (type->tt_flags & TTFLAG_CONST) == 0)
    {
	اگر (type->tt_flags & TTFLAG_STATIC)
	    // entry in static_types[] is followed by const type
	    type = type + 1;
	ورنہ
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
	صحیح	تفویض,
	type_T	*type)
{
    lvar_T  *lvar;
    dfunc_T *dfunc;

    اگر (arg_exists(name, len, تہی, تہی, تہی, cctx) == OK)
    {
	emsg_namelen(_(e_str_is_مستعمل_as_دلیل), name, (صحیح)len);
	واپس تہی;
    }

    اگر (GA_GROW_FAILS(&cctx->ctx_locals, 1))
	واپس تہی;
    lvar = ((lvar_T *)cctx->ctx_locals.ga_ڈیٹا) + cctx->ctx_locals.ga_len++;
    CLEAR_اشارہ_گر(lvar);

    // Every local variable uses the next entry on the stack.  We could re-use
    // the last ones when leaving a scope, but then variables used in a closure
    // might get overwritten.  To keep things simple do not re-use stack
    // entries.  This is less efficient, but memory is cheap these days.
    dfunc = ((dfunc_T *)def_functions.ga_ڈیٹا) + cctx->ctx_ufunc->uf_dfunc_idx;
    lvar->lv_idx = dfunc->df_var_نام.ga_len;

    lvar->lv_name = vim_strnsave(name, len == 0 ? STRLEN(name) : len);
    lvar->lv_const = تفویض;
    اگر (type == &t_unknown || type == &t_any)
	// type not known yet, may be inferred from RHS
	lvar->lv_type = type;
    ورنہ
	// may use TTFLAG_CONST
	سیٹ_var_type(lvar, type, cctx);

    // Remember the name for debugging.
    اگر (GA_GROW_FAILS(&dfunc->df_var_نام, 1))
	واپس تہی;
    ((char_u **)dfunc->df_var_نام.ga_ڈیٹا)[lvar->lv_idx] =
						    vim_strsave(lvar->lv_name);
    ++dfunc->df_var_نام.ga_len;

    واپس lvar;
}

/*
 * If "check_writable" is ASSIGN_CONST give an error if the variable was
 * defined with :final or :const, if "check_writable" is ASSIGN_FINAL give an
 * error if the variable was defined with :const.
 */
    ساکن صحیح
check_شے_writable(svar_T *sv, صحیح check_writable, char_u *name)
{
    اگر ((check_writable == تفویض_CONST && sv->sv_const != 0)
	    || (check_writable == تفویض_FINAL
					      && sv->sv_const == تفویض_CONST))
    {
	semsg(_(e_cannot_change_readonly_variable_str), name);
	واپس FAIL;
    }
    واپس OK;
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
get_script_شے_idx(
	صحیح	    sid,
	char_u	    *name,
	صحیح	    check_writable,
	cctx_T	    *cctx,
	cstack_T    *cstack)
{
    hashtab_T	    *ht;
    dictitem_T	    *di;
    scriptitem_T    *si = SCRIPT_شے(sid);
    svar_T	    *sv;
    صحیح		    idx;

    اگر (!SCRIPT_ID_VALID(sid))
	واپس -1;
    اگر (sid == current_sctx.sc_sid)
    {
	sallvar_T *sav = ڈھونڈو_script_var(name, 0, cctx, cstack);

	اگر (sav == تہی)
	    واپس -2;
	idx = sav->sav_var_vals_idx;
	sv = ((svar_T *)si->sn_var_vals.ga_ڈیٹا) + idx;
	اگر (check_شے_writable(sv, check_writable, name) == FAIL)
	    واپس -2;
	واپس idx;
    }

    // First look the name up in the hashtable.
    ht = &SCRIPT_VARS(sid);
    di = ڈھونڈو_var_اندر_ht(ht, 0, name, TRUE);
    اگر (di == تہی)
    {
	اگر (si->sn_autoload_سابقہ != تہی)
	{
	    hashitem_T *hi;

	    // A variable exported from an autoload script is in the global
	    // variables, we can find it in the all_vars table.
	    hi = hash_ڈھونڈو(&si->sn_all_vars.dv_hashtab, name);
	    اگر (!HASHITEM_خالی_خانے(hi))
		واپس HI2SAV(hi)->sav_var_vals_idx;
	}
	واپس -2;
    }

    // Now find the svar_T index in sn_var_vals.
    برائے (idx = 0; idx < si->sn_var_vals.ga_len; ++idx)
    {
	sv = ((svar_T *)si->sn_var_vals.ga_ڈیٹا) + idx;
	اگر (sv->sv_tv == &di->di_tv)
	{
	    اگر (check_شے_writable(sv, check_writable, name) == FAIL)
		واپس -2;
	    واپس idx;
	}
    }
    واپس -1;
}

    ساکن imported_T *
ڈھونڈو_imported_اندر_script(char_u *name, size_t len, صحیح sid)
{
    scriptitem_T    *si;
    صحیح		    idx;

    اگر (!SCRIPT_ID_VALID(sid))
	واپس تہی;
    si = SCRIPT_شے(sid);
    برائے (idx = 0; idx < si->sn_imports.ga_len; ++idx)
    {
	imported_T *import = ((imported_T *)si->sn_imports.ga_ڈیٹا) + idx;

	اگر (len == 0 ? STRCMP(name, import->imp_name) == 0
		     : STRLEN(import->imp_name) == len
				  && STRNCMP(name, import->imp_name, len) == 0)
	    واپس import;
    }
    واپس تہی;
}

/*
 * Find "name" in imported items of the current script.
 * If "len" is 0 use any length that works.
 * If "load" is TRUE and the script was not loaded yet, load it now.
 */
    imported_T *
ڈھونڈو_imported(char_u *name, size_t len, صحیح بھرو)
{
    اگر (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	واپس تہی;

    // Skip over "s:" before "s:something" to find the import name.
    صحیح off = name[0] == 's' && name[1] == ':' ? 2 : 0;

    imported_T *ret = ڈھونڈو_imported_اندر_script(name + off, len - off,
							  current_sctx.sc_sid);
    اگر (ret != تہی && بھرو && (ret->imp_flags & IMP_FLAGS_AUTOLOAD))
    {
	scid_T	actual_sid = 0;
	صحیح	save_emsg_off = emsg_off;

	// "emsg_off" will be set when evaluating an expression silently, but
	// we do want to know about errors in a script.  Also because it then
	// aborts when an error is encountered.
	emsg_off = FALSE;

	// script found before but not loaded yet
	ret->imp_flags &= ~IMP_FLAGS_AUTOLOAD;
	(خالی)do_ماخذ(SCRIPT_شے(ret->imp_sid)->sn_name, FALSE,
						       DOSO_NONE, &actual_sid);
	// If the script is a symlink it may be sourced with another name, may
	// need to adjust the script ID for that.
	اگر (actual_sid != 0)
	    ret->imp_sid = actual_sid;

	emsg_off = save_emsg_off;
    }
    واپس ret;
}

/*
 * Called when checking for a following operator at "arg".  When the rest of
 * the line is empty or only a comment, peek the next line.  If there is a next
 * line return a pointer to it and set "nextp".
 * Otherwise skip over white space.
 */
    char_u *
may_peek_اگلا_سطر(cctx_T *cctx, char_u *arg, char_u **nextp)
{
    char_u *p = skipwhite(arg);

    *nextp = تہی;
    اگر (*p == NUL || (VIM_ISWHITE(*arg) && vim9_comment_آغاز(p)))
    {
	*nextp = peek_اگلا_سطر_from_context(cctx);
	اگر (*nextp != تہی)
	    واپس *nextp;
    }
    واپس p;
}

/*
 * Return a pointer to the next line that isn't empty or only contains a
 * comment. Skips over white space.
 * Returns NULL if there is none.
 */
    char_u *
peek_اگلا_سطر_from_context(cctx_T *cctx)
{
    صحیح lnum = cctx->ctx_lnum;

    جب (++lnum < cctx->ctx_ufunc->uf_سطریں.ga_len)
    {
	char_u *سطر = ((char_u **)cctx->ctx_ufunc->uf_سطریں.ga_ڈیٹا)[lnum];
	char_u *p;

	// ignore NULLs inserted for continuation lines
	اگر (سطر != تہی)
	{
	    p = skipwhite(سطر);
	    اگر (vim9_bad_comment(p))
		واپس تہی;
	    اگر (*p != NUL && !vim9_comment_آغاز(p))
		واپس p;
	}
    }
    واپس تہی;
}

/*
 * Get the next line of the function from "cctx".
 * Skips over empty lines.  Skips over comment lines if "skip_comment" is TRUE.
 * Returns NULL when at the end.
 */
    char_u *
اگلا_سطر_from_context(cctx_T *cctx, صحیح skip_comment)
{
    char_u	*سطر;

    کرو
    {
	++cctx->ctx_lnum;
	اگر (cctx->ctx_lnum >= cctx->ctx_ufunc->uf_سطریں.ga_len)
	{
	    سطر = تہی;
	    توڑو;
	}
	سطر = ((char_u **)cctx->ctx_ufunc->uf_سطریں.ga_ڈیٹا)[cctx->ctx_lnum];
	cctx->ctx_سطر_آغاز = سطر;
	SOURCING_LNUM = cctx->ctx_lnum + 1;
    } جب (سطر == تہی || *skipwhite(سطر) == NUL
		     || (skip_comment && vim9_comment_آغاز(skipwhite(سطر))));
    واپس سطر;
}

/*
 * Skip over white space at "whitep" and assign to "*arg".
 * If "*arg" is at the end of the line, advance to the next line.
 * Also when "whitep" points to white space and "*arg" is on a "#".
 * Return FAIL if beyond the last line, "*arg" is unmodified then.
 */
    صحیح
may_get_اگلا_سطر(char_u *whitep, char_u **arg, cctx_T *cctx)
{
    *arg = skipwhite(whitep);
    اگر (vim9_bad_comment(*arg))
	واپس FAIL;
    اگر (**arg == NUL || (VIM_ISWHITE(*whitep) && vim9_comment_آغاز(*arg)))
    {
	char_u *اگلا = اگلا_سطر_from_context(cctx, TRUE);

	اگر (اگلا == تہی)
	    واپس FAIL;
	*arg = skipwhite(اگلا);
    }
    واپس OK;
}

/*
 * Idem, and give an error when failed.
 */
    صحیح
may_get_اگلا_سطر_error(char_u *whitep, char_u **arg, cctx_T *cctx)
{
    اگر (may_get_اگلا_سطر(whitep, arg, cctx) == FAIL)
    {
	SOURCING_LNUM = cctx->ctx_lnum + 1;
	emsg(_(e_سطر_incomplete));
	واپس FAIL;
    }
    واپس OK;
}

/*
 * Get a line from the compilation context, compatible with exarg_T getline().
 * Return a pointer to the line in allocated memory.
 * Return NULL for end-of-file or some error.
 */
    ساکن char_u *
exarg_getline(
	صحیح c غیر_مستعمل,
	خالی *cookie,
	صحیح indent غیر_مستعمل,
	getline_opt_T options غیر_مستعمل)
{
    cctx_T  *cctx = (cctx_T *)cookie;
    char_u  *p;

    برائے (;;)
    {
	اگر (cctx->ctx_lnum >= cctx->ctx_ufunc->uf_سطریں.ga_len - 1)
	    واپس تہی;
	++cctx->ctx_lnum;
	p = ((char_u **)cctx->ctx_ufunc->uf_سطریں.ga_ڈیٹا)[cctx->ctx_lnum];
	// Comment lines result in NULL pointers, skip them.
	اگر (p != تہی)
	    واپس vim_strsave(p);
    }
}

    خالی
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
    انتخاب (ufunc->uf_def_حالت)
    {
	حالت UF_مقصد_BE_COMPILED:
	    واپس TRUE;

	حالت UF_COMPILED:
	{
	    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_ڈیٹا)
							 + ufunc->uf_dfunc_idx;

	    انتخاب (compile_type)
	    {
		حالت CT_PROFILE:
#اگر_تعریف_شدہ FEAT_PROFILE
		    واپس dfunc->df_instr_prof == تہی;
#اگر_ختم
		حالت CT_NONE:
		    واپس dfunc->df_instr == تہی;
		حالت CT_DEBUG:
		    واپس dfunc->df_instr_debug == تہی;
	    }
	}

	حالت UF_NOT_COMPILED:
	حالت UF_COMPILE_ERROR:
	حالت UF_COMPILING:
	    توڑو;
    }
    واپس FALSE;
}

/*
 * Compile a nested :def command.
 */
    ساکن char_u *
compile_nested_function(exarg_T *eap, cctx_T *cctx, garray_T *سطریں_مقصد_free)
{
    صحیح		is_global = *eap->arg == 'g' && eap->arg[1] == ':';
    char_u	*name_آغاز = eap->arg;
    char_u	*name_انتہا = مقصد_name_انتہا(eap->arg, TRUE);
    صحیح		off;
    char_u	*func_name;
    string_T	lambda_name;
    ufunc_T	*ufunc;
    صحیح		r = FAIL;
    compiletype_T   compile_type;
    صحیح		funcref_isn_idx = -1;
    lvar_T	*lvar = تہی;
    char_u	*bracket_آغاز = تہی;

    اگر (eap->forceit)
    {
	semsg(_(e_cannot_use_bang_with_nested_def_str),
		eap->cmdidx == CMD_def ? ":def" : ":function");
	واپس تہی;
    }

    اگر (*name_آغاز == '/')
    {
	name_انتہا = skip_regexp(name_آغاز + 1, '/', TRUE);
	اگر (*name_انتہا == '/')
	    ++name_انتہا;
	سیٹ_nextcmd(eap, name_انتہا);
    }

    اگر (*name_انتہا == '<')
    {
	bracket_آغاز = name_انتہا;
	اگر (skip_generic_func_type_دلائل(&name_انتہا) == FAIL)
	    واپس تہی;
    }

    اگر (name_انتہا == name_آغاز || *skipwhite(name_انتہا) != '(')
    {
	اگر (!سرے_excmd2(name_آغاز, name_انتہا))
	{
	    اگر (*skipwhite(name_انتہا) == '.')
		semsg(_(e_cannot_define_dict_func_اندر_vim9_script_str),
								     eap->cmd);
	    ورنہ
		semsg(_(e_invalid_command_str), eap->cmd);
	    واپس تہی;
	}

	// "def" or "def Name": list functions
	اگر (generate_DEF(cctx, name_آغاز, name_انتہا - name_آغاز) == FAIL)
	    واپس تہی;
	واپس eap->nextcmd == تہی ? (char_u *)"" : eap->nextcmd;
    }

    اگر (bracket_آغاز != تہی)
	// generic function.  The function name ends before the list of types
	// (opening angle bracket).
	name_انتہا = bracket_آغاز;

    // Only g:Func() can use a namespace.
    اگر (name_آغاز[1] == ':' && !is_global)
    {
	semsg(_(e_namespace_not_supported_str), name_آغاز);
	واپس تہی;
    }
    اگر (cctx->ctx_skip != SKIP_YES
	    && check_defined(name_آغاز, name_انتہا - name_آغاز, cctx,
							  تہی, FALSE) == FAIL)
	واپس تہی;
    اگر (!ASCII_ISUPPER(is_global ? name_آغاز[2] : name_آغاز[0]))
    {
	semsg(_(e_function_name_must_آغاز_with_capital_str), name_آغاز);
	واپس تہی;
    }

    eap->arg = name_انتہا;
    fill_exarg_from_cctx(eap, cctx);

    eap->forceit = FALSE;
    // We use the special <lambda>99 name, but it's not really a lambda.
    lambda_name = get_lambda_name();
    lambda_name.string = vim_strnsave(lambda_name.string, lambda_name.لمبائی);
    اگر (lambda_name.string == تہی)
	واپس تہی;

    // This may free the current line, make a copy of the name.
    off = is_global ? 2 : 0;
    func_name = vim_strnsave(name_آغاز + off, name_انتہا - name_آغاز - off);
    اگر (func_name == تہی)
    {
	r = FAIL;
	جاؤ theend;
    }

    // A type variable cannot have the name of the function.  The function is
    // defined with the "<lambda>" name, check it here.
    اگر (bracket_آغاز != تہی)
    {
	size_t	len = STRLEN(func_name);

	برائے (char_u *t = bracket_آغاز + 1; *t != '>' && *t != NUL; )
	{
	    char_u *te = مقصد_name_انتہا(t, FALSE);

	    اگر ((size_t)(te - t) == len && STRNCMP(t, func_name, len) == 0)
	    {
		semsg(_(e_redefining_script_شے_str), func_name);
		r = FAIL;
		جاؤ theend;
	    }
	    t = te == t ? t + 1 : te;
	}
    }

    // Make sure "KeyTyped" is not set, it may cause indent to be written.
    صحیح save_کلید_Typed = کلید_Typed;
    کلید_Typed = FALSE;

    ufunc = define_function(eap, lambda_name.string, سطریں_مقصد_free, 0, تہی, 0,
									cctx);

    کلید_Typed = save_کلید_Typed;

    اگر (ufunc == تہی)
    {
	r = eap->skip ? OK : FAIL;
	جاؤ theend;
    }
    اگر (eap->nextcmd != تہی)
    {
	semsg(_(e_متن_ملا_after_str_str),
	      eap->cmdidx == CMD_def ? "enddef" : "endfunction", eap->nextcmd);
	r = FAIL;
	func_ptr_unref(ufunc);
	جاؤ theend;
    }

    // copy over the block scope IDs before compiling
    اگر (!is_global && cctx->ctx_ufunc->uf_بلاک_گہرائی > 0)
    {
	صحیح بلاک_گہرائی = cctx->ctx_ufunc->uf_بلاک_گہرائی;

	ufunc->uf_بلاک_ids = ALLOC_MULT(صحیح, بلاک_گہرائی);
	اگر (ufunc->uf_بلاک_ids != تہی)
	{
	    mch_memmove(ufunc->uf_بلاک_ids, cctx->ctx_ufunc->uf_بلاک_ids,
						    حجم(صحیح) * بلاک_گہرائی);
	    ufunc->uf_بلاک_گہرائی = بلاک_گہرائی;
	}
    }

    // Define the funcref before compiling, so that it is found by any
    // recursive call.
    اگر (is_global)
    {
	r = generate_NEWFUNC(cctx, lambda_name.string, func_name);
	func_name = تہی;
	lambda_name.string = تہی;
	lambda_name.لمبائی = 0;
    }
    ورنہ
    {
	// Define a local variable for the function reference.
	lvar = reserve_local(cctx, func_name, name_انتہا - name_آغاز,
					    تفویض_CONST, ufunc->uf_func_type);
	اگر (lvar == تہی)
	{
	    func_ptr_unref(ufunc);
	    جاؤ theend;
	}
	lvar->lv_ufunc = ufunc;
	اگر (generate_FUNCREF(cctx, ufunc, تہی, FALSE, 0, &funcref_isn_idx) == FAIL)
	{
	    func_ptr_unref(ufunc);
	    جاؤ theend;
	}
	r = generate_STORE(cctx, ISN_STORE, lvar->lv_idx, تہی);
    }

    compile_type = get_compile_type(ufunc);
#اگر_تعریف_شدہ FEAT_PROFILE
    // If the outer function is profiled, also compile the nested function for
    // profiling.
    اگر (cctx->ctx_compile_type == CT_PROFILE)
	compile_type = CT_PROFILE;
#اگر_ختم
    اگر (func_needs_compiling(ufunc, compile_type)
	    && compile_def_function(ufunc, TRUE, compile_type, cctx) == FAIL)
    {
	func_ptr_unref(ufunc);
	اگر (lvar != تہی)
	    // Now the local variable can't be used.
	    *lvar->lv_name = '/';  // impossible value
	جاؤ theend;
    }

#اگر_تعریف_شدہ FEAT_PROFILE
    // When the outer function is compiled for profiling, the nested function
    // may be called without profiling.  Compile it here in the right context.
    اگر (compile_type == CT_PROFILE && func_needs_compiling(ufunc, CT_NONE))
	compile_def_function(ufunc, FALSE, CT_NONE, cctx);
#اگر_ختم

    // If a FUNCREF instruction was generated, set the index after compiling.
    اگر (funcref_isn_idx != -1 && ufunc->uf_def_حالت == UF_COMPILED)
    {
	isn_T	*funcref_isn = ((isn_T *)cctx->ctx_instr.ga_ڈیٹا) +
							funcref_isn_idx;
	funcref_isn->isn_arg.funcref.fr_dfunc_idx = ufunc->uf_dfunc_idx;
    }

theend:
    vim_free(lambda_name.string);
    vim_free(func_name);
    واپس r == FAIL ? تہی : (char_u *)"";
}

/*
 * Compile one Vim expression {expr} in string "p".
 * "p" points to the opening "{".
 * Return a pointer to the character after "}", NULL for an error.
 */
    char_u *
compile_one_اظہاریہ_اندر_str(char_u *p, cctx_T *cctx)
{
    char_u	*بلاک_آغاز;
    char_u	*بلاک_انتہا;

    // Skip the opening {.
    بلاک_آغاز = skipwhite(p + 1);
    بلاک_انتہا = بلاک_آغاز;
    اگر (*بلاک_آغاز != NUL && skip_اظہاریہ(&بلاک_انتہا, تہی) == FAIL)
	واپس تہی;
    بلاک_انتہا = skipwhite(بلاک_انتہا);
    // The block must be closed by a }.
    اگر (*بلاک_انتہا != '}')
    {
	semsg(_(e_missing_close_curly_str), p);
	واپس تہی;
    }
    اگر (compile_expr0(&بلاک_آغاز, cctx) == FAIL)
	واپس تہی;
    may_generate_2STRING(-1, TOSTRING_INTERPOLATE, cctx);

    واپس بلاک_انتہا + 1;
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
compile_all_اظہاریہ_اندر_str(char_u *str, صحیح evalstr, cctx_T *cctx)
{
    char_u	*p = str;
    char_u	*val;
    صحیح		گنتی = 0;

    اگر (cctx->ctx_skip == SKIP_YES)
	واپس OK;

    اگر (!evalstr || *str == NUL)
    {
	// Literal string, possibly empty.
	val = *str != NUL ? vim_strsave(str) : تہی;
	واپس generate_PUSHS(cctx, &val);
    }

    // Push all the string pieces to the stack, followed by a ISN_CONCAT.
    جب (*p != NUL)
    {
	char_u	*lit_آغاز;
	صحیح	escaped_brace = FALSE;

	// Look for a block start.
	lit_آغاز = p;
	جب (*p != '{' && *p != '}' && *p != NUL)
	    ++p;

	اگر (*p != NUL && *p == p[1])
	{
	    // Escaped brace, unescape and continue.
	    // Include the brace in the literal string.
	    ++p;
	    escaped_brace = TRUE;
	}
	ورنہ اگر (*p == '}')
	{
	    semsg(_(e_stray_closing_curly_str), str);
	    واپس FAIL;
	}

	// Append the literal part.
	اگر (p != lit_آغاز)
	{
	    val = vim_strnsave(lit_آغاز, (size_t)(p - lit_آغاز));
	    اگر (generate_PUSHS(cctx, &val) == FAIL)
		واپس FAIL;
	    ++گنتی;
	}

	اگر (*p == NUL)
	    توڑو;

	اگر (escaped_brace)
	{
	    // Skip the second brace.
	    ++p;
	    جاری;
	}

	p = compile_one_اظہاریہ_اندر_str(p, cctx);
	اگر (p == تہی)
	    واپس FAIL;
	++گنتی;
    }

    // Small optimization, if there's only a single piece skip the ISN_CONCAT.
    اگر (گنتی > 1)
	واپس generate_CONCAT(cctx, گنتی);

    واپس OK;
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
	    واپس 3;
	}
	واپس 1;
    }
    اگر (vim_strchr((char_u *)"+-*/%", *p) != تہی && p[1] == '=')
	واپس 2;
    اگر (STRNCMP(p, "..=", 3) == 0)
	واپس 3;
    واپس 0;
}

/*
 * Generate the load instruction for "name".
 */
    ساکن صحیح
generate_loadvar(cctx_T *cctx, lhs_T *lhs)
{
    char_u	*name = lhs->lhs_name;
    type_T	*type = lhs->lhs_type;
    صحیح		res = OK;

    انتخاب (lhs->lhs_dest)
    {
	حالت dest_option:
	حالت dest_func_option:
	    generate_بھرو(cctx, ISN_LOADOPT, 0, name, type);
	    توڑو;
	حالت dest_global:
	    اگر (vim_strchr(name, AUTOLOAD_CHAR) == تہی)
	    {
		اگر (name[2] == NUL)
		    generate_instr_type(cctx, ISN_LOADGDICT, &t_dict_any);
		ورنہ
		    generate_بھرو(cctx, ISN_LOADG, 0, name + 2, type);
	    }
	    ورنہ
		generate_بھرو(cctx, ISN_LOADAUTO, 0, name, type);
	    توڑو;
	حالت dest_بفر:
	    generate_بھرو(cctx, ISN_LOADB, 0, name + 2, type);
	    توڑو;
	حالت dest_کھڑکی:
	    generate_بھرو(cctx, ISN_LOADW, 0, name + 2, type);
	    توڑو;
	حالت dest_tab:
	    generate_بھرو(cctx, ISN_LOADT, 0, name + 2, type);
	    توڑو;
	حالت dest_script:
	حالت dest_script_v9:
	    res = compile_بھرو_scriptvar(cctx,
			    name + (name[1] == ':' ? 2 : 0), تہی, تہی, تہی);
	    توڑو;
	حالت dest_env:
	    // Include $ in the name here
	    generate_بھرو(cctx, ISN_LOADENV, 0, name, type);
	    توڑو;
	حالت dest_reg:
	    generate_بھرو(cctx, ISN_LOADREG, name[1], تہی, &t_string);
	    توڑو;
	حالت dest_vimvar:
	    generate_LOADV(cctx, name + 2);
	    توڑو;
	حالت dest_local:
	    اگر (cctx->ctx_skip != SKIP_YES)
	    {
		lvar_T	*lvar = lhs->lhs_lvar;
		اگر (lvar->lv_from_outer > 0)
		    generate_LOADOUTER(cctx, lvar->lv_idx, lvar->lv_from_outer,
				 lvar->lv_loop_گہرائی, lvar->lv_loop_idx, type);
		ورنہ
		    generate_بھرو(cctx, ISN_بھرو, lvar->lv_idx, تہی, type);
	    }
	    توڑو;
	حالت dest_class_member:
	    generate_CLASSMEMBER(cctx, TRUE, lhs->lhs_class,
						     lhs->lhs_classmember_idx);
	    توڑو;
	حالت dest_اظہاریہ:
	    // list or dict value should already be on the stack.
	    توڑو;
    }

    واپس res;
}

/*
 * Skip over "[expr]" or ".member".
 * Does not check for any errors.
 */
    ساکن char_u *
skip_اشاریہ(char_u *آغاز)
{
    char_u *p = آغاز;

    اگر (*p == '[')
    {
	p = skipwhite(p + 1);
	(خالی)skip_اظہاریہ(&p, تہی);
	p = skipwhite(p);
	اگر (*p == ']')
	    واپس p + 1;
	واپس p;
    }
    // if (*p == '.')
    واپس مقصد_name_انتہا(p + 1, TRUE);
}

    خالی
vim9_declare_error(char_u *name)
{
    حرف *scope = "";

    انتخاب (*name)
    {
	حالت 'g': scope = _("global"); توڑو;
	حالت 'b': scope = _("buffer"); توڑو;
	حالت 'w': scope = _("window"); توڑو;
	حالت 't': scope = _("tab"); توڑو;
	حالت 'v': scope = "v:"; توڑو;
	حالت '$': semsg(_(e_cannot_declare_an_environment_variable_str), name);
		  واپس;
	حالت '&': semsg(_(e_cannot_declare_an_option_str), name);
		  واپس;
	حالت '@': semsg(_(e_cannot_declare_a_رجسٹر_str), name);
		  واپس;
	طےشدہ: واپس;
    }
    semsg(_(e_cannot_declare_a_scope_variable_str), scope, name);
}

/*
 * Return TRUE if "name" is a valid register to use.
 * Return FALSE and give an error message if not.
 */
    ساکن صحیح
valid_dest_reg(صحیح name)
{
    اگر (name == '@')
       name = '"';
    اگر (name == '/' || name == '=' || name == '#' || valid_yank_reg(name, TRUE))
	واپس TRUE;
    emsg_invreg(name);
    واپس FAIL;
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
	تفویض_dest_T	*dest,
	cmdidx_T	cmdidx,
	صحیح		has_اشاریہ,
	صحیح		*option_scope,
	صحیح		*vimvaridx,
	type_T		**type,
	cctx_T		*cctx)
{
    char_u *p;

    اگر (*name == '&')
    {
	صحیح		cc;
	طویل		numval;
	getoption_T	opt_type;
	صحیح		opt_p_flags;

	*dest = dest_option;
	اگر (cmdidx == CMD_final || cmdidx == CMD_const)
	{
	    emsg(_(e_cannot_تالا_option));
	    واپس FAIL;
	}
	p = name;
	p = ڈھونڈو_option_انتہا(&p, option_scope);
	اگر (p == تہی)
	{
	    // cannot happen?
	    emsg(_(e_unexpected_characters_اندر_assignment));
	    واپس FAIL;
	}
	cc = *p;
	*p = NUL;
	opt_type = get_option_قدر(skip_option_env_lead(name),
				   &numval, تہی, &opt_p_flags, *option_scope);
	*p = cc;
	انتخاب (opt_type)
	{
	    حالت gov_unknown:
		    semsg(_(e_unknown_option_str), name);
		    واپس FAIL;
	    حالت gov_string:
	    حالت gov_hidden_string:
		    اگر (opt_p_flags & P_FUNC)
		    {
			// might be a Funcref, check the type later
			*type = &t_any;
			*dest = dest_func_option;
		    }
		    ورنہ
		    {
			*type = &t_string;
		    }
		    توڑو;
	    حالت gov_bool:
	    حالت gov_hidden_bool:
		    *type = &t_bool;
		    توڑو;
	    حالت gov_نمبر:
	    حالت gov_hidden_نمبر:
		    *type = &t_نمبر;
		    توڑو;
	}
    }
    ورنہ اگر (*name == '$')
    {
	*dest = dest_env;
	*type = &t_string;
    }
    ورنہ اگر (*name == '@')
    {
	اگر (!valid_dest_reg(name[1]))
	    واپس FAIL;
	*dest = dest_reg;
	*type = name[1] == '#' ? &t_نمبر_or_string : &t_string;
    }
    ورنہ اگر (STRNCMP(name, "g:", 2) == 0)
    {
	*dest = dest_global;
    }
    ورنہ اگر (STRNCMP(name, "b:", 2) == 0)
    {
	*dest = dest_بفر;
    }
    ورنہ اگر (STRNCMP(name, "w:", 2) == 0)
    {
	*dest = dest_کھڑکی;
    }
    ورنہ اگر (STRNCMP(name, "t:", 2) == 0)
    {
	*dest = dest_tab;
    }
    ورنہ اگر (STRNCMP(name, "v:", 2) == 0)
    {
	typval_T	*vtv;
	صحیح		di_flags;

	*vimvaridx = ڈھونڈو_vim_var(name + 2, &di_flags);
	اگر (*vimvaridx < 0)
	{
	    semsg(_(e_variable_not_ملا_str), name);
	    واپس FAIL;
	}
	// An item of a read-only variable can be changed, as when not compiled.
	// We use the current value of "sandbox" here, is that OK?
	اگر (!has_اشاریہ && var_check_ro(di_flags, name, FALSE))
	    واپس FAIL;
	*dest = dest_vimvar;
	vtv = get_vim_var_tv(*vimvaridx);
	*type = typval2type_vimvar(vtv, cctx->ctx_type_list);
    }
    واپس OK;
}

    ساکن صحیح
is_decl_command(cmdidx_T cmdidx)
{
    واپس cmdidx == CMD_let || cmdidx == CMD_var
				 || cmdidx == CMD_final || cmdidx == CMD_const;
}

/*
 * Returns TRUE if the class or object variable in "lhs" is modifiable.
 * "var_start" points to the start of the variable name and "lhs->lhs_varlen"
 * has the total length.  Note that the "lhs" can be nested an object reference
 * (e.g.  a.b.c.d.var).
 */
    ساکن صحیح
lhs_class_member_modifiable(lhs_T *lhs, char_u	*var_آغاز, cctx_T *cctx)
{
    size_t	varlen = lhs->lhs_varlen;
    class_T	*cl = lhs->lhs_type->tt_class;
    صحیح		is_object = lhs->lhs_type->tt_type == VAR_OBJECT;
    char_u	*name = var_آغاز + varlen + 1;
    size_t	namelen = lhs->lhs_انتہا - var_آغاز - varlen - 1;
    ocmember_T	*m;

    m = member_lookup(cl, lhs->lhs_type->tt_type, name, namelen, تہی);
    اگر (m == تہی)
    {
	member_not_ملا_پیغام(cl, lhs->lhs_type->tt_type, name, namelen);
	واپس FALSE;
    }

    اگر (IS_ENUM(cl))
    {
	semsg(_(e_enumvalue_str_cannot_be_modified), cl->class_name.string,
		m->ocm_name.string);
	واپس FALSE;
    }

    // If it is private member variable, then accessing it outside the
    // class is not allowed.
    // If it is a read only class variable, then it can be modified
    // only inside the class where it is defined.
    اگر ((m->ocm_access != VIM_ACCESS_ALL) &&
	    ((is_object && !inside_class(cctx, cl))
	     || (!is_object && cctx->ctx_ufunc->uf_class != cl)))
    {
	حرف *پیغام = (m->ocm_access == VIM_ACCESS_نجی)
				? e_cannot_access_protected_variable_str
				: e_variable_is_not_writable_str;
	emsg_var_cl_define(پیغام, m->ocm_name.string, 0, cl);
	واپس FALSE;
    }

    واپس TRUE;
}

/*
 * Initialize "lhs" with default values
 */
    ساکن خالی
lhs_init_defaults(lhs_T *lhs)
{
    CLEAR_اشارہ_گر(lhs);
    lhs->lhs_dest = dest_local;
    lhs->lhs_vimvaridx = -1;
    lhs->lhs_scriptvar_idx = -1;
    lhs->lhs_member_idx = -1;
}

/*
 * When compiling a LHS variable name, find the end of the destination and the
 * end of the variable name.
 */
    ساکن صحیح
lhs_ڈھونڈو_var_انتہا(
    lhs_T	*lhs,
    char_u	*var_آغاز,
    صحیح		is_decl,
    char_u	**var_endp)
{
    char_u  *var_انتہا = *var_endp;

    // "lhs_dest_end" is the end of the destination, including "[expr]" or
    // ".name".
    // "var_end" is the end of the variable/option/etc. name.
    lhs->lhs_dest_انتہا = skip_var_one(var_آغاز, FALSE);
    اگر (*var_آغاز == '@')
    {
	اگر (!valid_dest_reg(var_آغاز[1]))
	    واپس FAIL;
	var_انتہا = var_آغاز + 2;
    }
    ورنہ
    {
	// skip over the leading "&", "&l:", "&g:" and "$"
	var_انتہا = skip_option_env_lead(var_آغاز);
	var_انتہا = مقصد_name_انتہا(var_انتہا, TRUE);
    }

    // "a: type" is declaring variable "a" with a type, not dict "a:".
    اگر (is_decl && lhs->lhs_dest_انتہا == var_آغاز + 2
					&& lhs->lhs_dest_انتہا[-1] == ':')
	--lhs->lhs_dest_انتہا;
    اگر (is_decl && var_انتہا == var_آغاز + 2 && var_انتہا[-1] == ':')
	--var_انتہا;

    lhs->lhs_انتہا = lhs->lhs_dest_انتہا;
    *var_endp = var_انتہا;

    واپس OK;
}

/*
 * Set various fields in "lhs"
 */
    ساکن صحیح
lhs_init(
    lhs_T	*lhs,
    char_u	*var_آغاز,
    صحیح		is_decl,
    صحیح		heredoc,
    char_u	**var_endp)
{
    char_u *var_انتہا = *var_endp;

    lhs_init_defaults(lhs);

    // Find the end of the variable and the destination
    اگر (lhs_ڈھونڈو_var_انتہا(lhs, var_آغاز, is_decl, &var_انتہا) == FAIL)
	واپس FAIL;

    // compute the length of the destination without "[expr]" or ".name"
    lhs->lhs_varlen = var_انتہا - var_آغاز;
    lhs->lhs_varlen_کل = lhs->lhs_varlen;
    lhs->lhs_name = vim_strnsave(var_آغاز, lhs->lhs_varlen);
    اگر (lhs->lhs_name == تہی)
	واپس FAIL;

    اگر (lhs->lhs_dest_انتہا > var_آغاز + lhs->lhs_varlen)
	// Something follows after the variable: "var[idx]" or "var.key".
	lhs->lhs_has_اشاریہ = TRUE;

    lhs->lhs_type = heredoc ? &t_list_string : &t_any;

    *var_endp = var_انتہا;

    واپس OK;
}

/*
 * Compile a LHS class variable name.
 */
    ساکن صحیح
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
	واپس FAIL;
    }

    اگر (is_decl)
    {
	semsg(_(e_variable_already_declared_اندر_class_str), lhs->lhs_name);
	واپس FAIL;
    }

    ocmember_T	*m = &defcl->class_class_members[lhs->lhs_classmember_idx];
    اگر (oc_var_check_ro(defcl, m))
	واپس FAIL;

    lhs->lhs_dest = dest_class_member;
    // The class variable is defined either in the current class or
    // in one of the parent class in the hierarchy.
    lhs->lhs_class = defcl;
    lhs->lhs_type = oc_member_type_by_idx(defcl, FALSE,
						lhs->lhs_classmember_idx);

    واپس OK;
}

/*
 * Compile an imported LHS variable
 */
    ساکن صحیح
compile_lhs_import_var(
    lhs_T	*lhs,
    imported_T	*import,
    char_u	*var_آغاز,
    char_u	**var_endp,
    char_u	**rawnamep)
{
    char_u	*var_انتہا = *var_endp;
    char_u	*dot = vim_strchr(var_آغاز, '.');
    char_u	*p;

    // for an import the name is what comes after the dot
    اگر (dot == تہی)
    {
	semsg(_(e_no_dot_after_imported_name_str), var_آغاز);
	واپس FAIL;
    }

    p = skipwhite(dot + 1);
    var_انتہا = مقصد_name_انتہا(p, TRUE);
    اگر (var_انتہا == p)
    {
	semsg(_(e_missing_name_after_imported_name_str), var_آغاز);
	واپس FAIL;
    }

    vim_free(lhs->lhs_name);
    lhs->lhs_varlen = var_انتہا - p;
    lhs->lhs_name = vim_strnsave(p, lhs->lhs_varlen);
    اگر (lhs->lhs_name == تہی)
	واپس FAIL;
    *rawnamep = lhs->lhs_name;
    lhs->lhs_scriptvar_sid = import->imp_sid;

    // TODO: where do we check this name is exported?

    // Check if something follows: "exp.var[idx]" or
    // "exp.var.key".
    lhs->lhs_has_اشاریہ = lhs->lhs_dest_انتہا > skipwhite(var_انتہا);

    *var_endp = var_انتہا;

    واپس OK;
}

/*
 * Process a script-local variable when compiling a LHS variable name.
 */
    ساکن صحیح
compile_lhs_script_var(
    cctx_T	*cctx,
    lhs_T	*lhs,
    char_u	*var_آغاز,
    char_u	*var_انتہا,
    صحیح		is_decl)
{
    صحیح		script_namespace = FALSE;
    صحیح		script_var = FALSE;
    imported_T	*import;
    char_u	*var_name;
    size_t	var_name_len;

    اگر (lhs->lhs_varlen > 1 && STRNCMP(var_آغاز, "s:", 2) == 0)
	script_namespace = TRUE;

    اگر (script_namespace)
    {
	var_name = var_آغاز + 2;
	var_name_len = lhs->lhs_varlen - 2;
    }
    ورنہ
    {
	var_name = var_آغاز;
	var_name_len = lhs->lhs_varlen;
    }

    اگر (script_var_exists(var_name, var_name_len, cctx, تہی) == OK)
	script_var = TRUE;

    import = ڈھونڈو_imported(var_آغاز, lhs->lhs_varlen, FALSE);

    اگر (script_namespace || script_var || import != تہی)
    {
	char_u *rawname = lhs->lhs_name + (lhs->lhs_name[1] == ':' ? 2 : 0);

	اگر (script_namespace && current_script_is_vim9())
	{
	    semsg(_(e_cannot_use_s_colon_اندر_vim9_script_str), var_آغاز);
	    واپس FAIL;
	}

	اگر (is_decl)
	{
	    اگر (script_namespace)
		semsg(_(e_cannot_declare_script_variable_اندر_function_str),
			lhs->lhs_name);
	    ورنہ
		semsg(_(e_variable_already_declared_اندر_script_str),
			lhs->lhs_name);
	    واپس FAIL;
	}
	ورنہ اگر (cctx->ctx_ufunc->uf_script_ctx_نسخہ == SCRIPT_نسخہ_VIM9
		&& script_namespace
		&& !script_var && import == تہی)
	{
	    semsg(_(e_unknown_variable_str), lhs->lhs_name);
	    واپس FAIL;
	}

	lhs->lhs_dest = current_script_is_vim9() ? dest_script_v9 :
								dest_script;

	// existing script-local variables should have a type
	lhs->lhs_scriptvar_sid = current_sctx.sc_sid;
	اگر (import != تہی)
	{
	    اگر (compile_lhs_import_var(lhs, import, var_آغاز, &var_انتہا,
							&rawname) == FAIL)
		واپس FAIL;
	}

	اگر (SCRIPT_ID_VALID(lhs->lhs_scriptvar_sid))
	{
	    // Check writable only when no index follows.
	    lhs->lhs_scriptvar_idx = get_script_شے_idx(
					lhs->lhs_scriptvar_sid, rawname,
					lhs->lhs_has_اشاریہ ?  تفویض_FINAL :
					تفویض_CONST, cctx, تہی);
	    اگر (lhs->lhs_scriptvar_idx >= 0)
	    {
		scriptitem_T *si = SCRIPT_شے(lhs->lhs_scriptvar_sid);
		svar_T	 *sv = ((svar_T *)si->sn_var_vals.ga_ڈیٹا)
						+ lhs->lhs_scriptvar_idx;

		lhs->lhs_type = sv->sv_type;
	    }
	}

	واپس OK;
    }

    واپس check_defined(var_آغاز, lhs->lhs_varlen, cctx, تہی, FALSE);
}

/*
 * Compile the LHS destination.
 */
    ساکن صحیح
compile_lhs_var_dest(
    cctx_T	*cctx,
    lhs_T	*lhs,
    صحیح		cmdidx,
    char_u	*var_آغاز,
    char_u	*var_انتہا,
    صحیح		is_decl,
    صحیح		has_cmd)	// "var" before "var_start"
{
    صحیح	    declare_error = FALSE;

    اگر (get_var_dest(lhs->lhs_name, &lhs->lhs_dest, cmdidx,
				lhs->lhs_has_اشاریہ,
				&lhs->lhs_opt_flags, &lhs->lhs_vimvaridx,
				&lhs->lhs_type, cctx) == FAIL)
	واپس FAIL;

    اگر (lhs->lhs_dest != dest_local && cmdidx != CMD_const
						&& cmdidx != CMD_final)
    {
	// Specific kind of variable recognized.
	declare_error = is_decl;
    }
    ورنہ
    {
	class_T	*defcl;

	// No specific kind of variable recognized, just a name.
	اگر (check_reserved_name(lhs->lhs_name, lhs->lhs_has_اشاریہ
						&& *var_انتہا == '.') == FAIL)
	    واپس FAIL;

	اگر (lookup_local(var_آغاز, lhs->lhs_varlen, &lhs->lhs_local_lvar,
								cctx) == OK)
	{
	    lhs->lhs_lvar = &lhs->lhs_local_lvar;
	}
	ورنہ
	{
	    CLEAR_میدان(lhs->lhs_arg_lvar);
	    اگر (arg_exists(var_آغاز, lhs->lhs_varlen,
			&lhs->lhs_arg_lvar.lv_idx, &lhs->lhs_arg_lvar.lv_type,
			&lhs->lhs_arg_lvar.lv_from_outer, cctx) == OK)
	    {
		اگر (is_decl)
		{
		    semsg(_(e_str_is_مستعمل_as_دلیل), lhs->lhs_name);
		    واپس FAIL;
		}
		lhs->lhs_lvar = &lhs->lhs_arg_lvar;
	    }
	}

	اگر (lhs->lhs_lvar != تہی)
	{
	    اگر (is_decl)
	    {
		// if we come here with what looks like an assignment like
		// .= but which has been rejected by assignment_len() from
		// may_compile_assignment give a better error message
		char_u *p = skipwhite(lhs->lhs_انتہا);
		اگر (p[0] == '.' && p[1] == '=')
		    emsg(_(e_dot_equal_not_supported_with_script_نسخہ_two));
		ورنہ اگر (p[0] == ':' && !has_cmd)
		    // type specified in an assignment without "var"
		    semsg(_(e_trailing_characters_str), p);
		ورنہ
		    semsg(_(e_variable_already_declared_str), lhs->lhs_name);
		واپس FAIL;
	    }
	}
	ورنہ اگر ((lhs->lhs_classmember_idx = cctx_class_member_idx(
			cctx, var_آغاز, lhs->lhs_varlen, &defcl)) >= 0)
	{
	    اگر (compile_lhs_class_variable(cctx, lhs, defcl, is_decl)
		    == FAIL)
		واپس FAIL;
	}
	ورنہ
	{
	    اگر (compile_lhs_script_var(cctx, lhs, var_آغاز, var_انتہا,
			is_decl) == FAIL)
		واپس FAIL;
	}
    }

    اگر (declare_error)
    {
	vim9_declare_error(lhs->lhs_name);
	واپس FAIL;
    }

    واپس OK;
}

/*
 * When compiling a LHS variable name, for a class or an object, set the LHS
 * member type.
 */
    ساکن صحیح
compile_lhs_سیٹ_oc_member_type(
    cctx_T	*cctx,
    lhs_T	*lhs,
    char_u	*var_آغاز)
{
    class_T	*cl = lhs->lhs_type->tt_class;
    صحیح		is_object = lhs->lhs_type->tt_type == VAR_OBJECT;
    char_u	*name = var_آغاز + lhs->lhs_varlen + 1;
    size_t	namelen = lhs->lhs_انتہا - var_آغاز - lhs->lhs_varlen - 1;

    ocmember_T	*m = member_lookup(cl, lhs->lhs_type->tt_type,
	    name, namelen, &lhs->lhs_member_idx);
    اگر (m == تہی)
    {
	member_not_ملا_پیغام(cl, lhs->lhs_type->tt_type, name, namelen);
	واپس FAIL;
    }

    اگر (IS_ENUM(cl))
    {
	اگر (!inside_class(cctx, cl))
	{
	    semsg(_(e_enumvalue_str_cannot_be_modified),
		    cl->class_name.string, m->ocm_name.string);
	    واپس FAIL;
	}
	اگر (lhs->lhs_type->tt_type == VAR_OBJECT &&
		lhs->lhs_member_idx < 2)
	{
	    حرف *پیغام = lhs->lhs_member_idx == 0 ?
		e_enum_str_name_cannot_be_modified :
		e_enum_str_ordinal_cannot_be_modified;
	    semsg(_(پیغام), cl->class_name.string);
	    واپس FAIL;
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
	حرف *پیغام = (m->ocm_access == VIM_ACCESS_نجی)
	    ? e_cannot_access_protected_variable_str
	    : e_variable_is_not_writable_str;
	emsg_var_cl_define(پیغام, m->ocm_name.string, 0, cl);
	واپس FAIL;
    }

    اگر (!IS_CONSTRUCTOR_طریقہ(cctx->ctx_ufunc)
	    && oc_var_check_ro(cl, m))
	واپس FAIL;

    lhs->lhs_member_type = m->ocm_type;

    واپس OK;
}

/*
 * When compiling a LHS variable, set the LHS variable type.
 */
    ساکن صحیح
compile_lhs_سیٹ_type(cctx_T *cctx, lhs_T *lhs, char_u *var_انتہا, صحیح is_decl)
{
    اگر (is_decl && *skipwhite(var_انتہا) == ':')
    {
	char_u *p;

	// parse optional type: "let var: type = expr"
	اگر (VIM_ISWHITE(*var_انتہا))
	{
	    semsg(_(e_no_white_space_allowed_before_colon_str), var_انتہا);
	    واپس FAIL;
	}

	اگر (!VIM_ISWHITE(var_انتہا[1]))
	{
	    semsg(_(e_white_space_required_after_str_str), ":", var_انتہا);
	    واپس FAIL;
	}

	p = skipwhite(var_انتہا + 1);
	lhs->lhs_type = parse_type(&p, cctx->ctx_type_list, cctx->ctx_ufunc, cctx, TRUE);
	اگر (lhs->lhs_type == تہی
		|| !valid_declaration_type(lhs->lhs_type))
	    واپس FAIL;

	lhs->lhs_has_type = TRUE;
	lhs->lhs_انتہا = p;
    }
    ورنہ اگر (lhs->lhs_lvar != تہی)
	lhs->lhs_type = lhs->lhs_lvar->lv_type;

    واپس OK;
}

/*
 * Returns TRUE if "lhs" is a concatenable string.
 */
    ساکن صحیح
lhs_concatenable(lhs_T *lhs)
{
    واپس lhs->lhs_dest == dest_global
		|| lhs->lhs_has_اشاریہ
		|| lhs->lhs_type->tt_type == VAR_STRING
		|| lhs->lhs_type->tt_type == VAR_ANY;
}

/*
 * Create a new local variable when compiling a LHS variable.
 */
    ساکن صحیح
compile_lhs_new_local_var(
    cctx_T	*cctx,
    lhs_T	*lhs,
    char_u	*var_آغاز,
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
	واپس FAIL;
    }

    اگر (!is_decl || (lhs->lhs_has_اشاریہ && !has_cmd
					&& cctx->ctx_skip != SKIP_YES))
    {
	semsg(_(e_unknown_variable_str), lhs->lhs_name);
	واپس FAIL;
    }

    // Check the name is valid for a funcref.
    اگر (lhs->lhs_type->tt_type == VAR_FUNC
				|| lhs->lhs_type->tt_type == VAR_PARTIAL)
    {
	اگر (var_wrong_func_name(lhs->lhs_name, TRUE))
	    واپس FAIL;
    }

    // New local variable.
    صحیح تفویض;
    انتخاب (cmdidx)
    {
	حالت CMD_final:
	    تفویض = تفویض_FINAL; توڑو;
	حالت CMD_const:
	    تفویض = تفویض_CONST; توڑو;
	طےشدہ:
	    تفویض = تفویض_VAR; توڑو;
    }

    lhs->lhs_lvar = reserve_local(cctx, var_آغاز, lhs->lhs_varlen, تفویض,
							lhs->lhs_type);
    اگر (lhs->lhs_lvar == تہی)
	واپس FAIL;

    lhs->lhs_new_local = TRUE;

    واپس OK;
}

/*
 * When compiling a LHS variable name, set the LHS member type.
 */
    ساکن صحیح
compile_lhs_سیٹ_member_type(
    cctx_T	*cctx,
    lhs_T	*lhs,
    char_u	*var_آغاز,
    صحیح		is_decl,
    صحیح		has_cmd)
{
    lhs->lhs_member_type = lhs->lhs_type;

    اگر (!lhs->lhs_has_اشاریہ)
	واپس OK;

    char_u	*after = var_آغاز + lhs->lhs_varlen;
    char_u	*p;

    // Something follows after the variable: "var[idx]" or "var.key".
    اگر (is_decl && cctx->ctx_skip != SKIP_YES)
    {
	اگر (has_cmd)
	    emsg(_(e_cannot_use_اشاریہ_when_declaring_variable));
	ورنہ
	    semsg(_(e_unknown_variable_str), lhs->lhs_name);
	واپس FAIL;
    }

    // Now: var_start[lhs->lhs_varlen] is '[' or '.'
    // Only the last index is used below, if there are others
    // before it generate code for the expression.  Thus for
    // "ll[1][2]" the expression is "ll[1]" and "[2]" is the index.
    برائے (;;)
    {
	p = skip_اشاریہ(after);
	اگر (*p != '[' && *p != '.')
	{
	    lhs->lhs_varlen_کل = p - var_آغاز;
	    توڑو;
	}
	after = p;
    }
    اگر (after > var_آغاز + lhs->lhs_varlen)
    {
	lhs->lhs_varlen = after - var_آغاز;
	lhs->lhs_dest = dest_اظہاریہ;
	// We don't know the type before evaluating the expression,
	// use "any" until then.  The member index is for the first name,
	// not for the last index.
	lhs->lhs_type = &t_any;
	lhs->lhs_member_idx = -1;
    }

    صحیح use_class = lhs->lhs_type != تہی
	&& (lhs->lhs_type->tt_type == VAR_CLASS
		|| lhs->lhs_type->tt_type == VAR_OBJECT);

    اگر (lhs->lhs_type == تہی
	    || (use_class ? lhs->lhs_type->tt_class == تہی
		: lhs->lhs_type->tt_member == تہی))
    {
	lhs->lhs_member_type = &t_any;
    }
    ورنہ اگر (use_class)
    {
	// for an object or class member get the type of the member
	اگر (compile_lhs_سیٹ_oc_member_type(cctx, lhs, var_آغاز) == FAIL)
	    واپس FAIL;
    }
    ورنہ
	lhs->lhs_member_type = lhs->lhs_type->tt_member;

    واپس OK;
}

/*
 * Figure out the LHS type and other properties for an assignment or one item
 * of ":unlet" with an index.
 * Returns OK or FAIL.
 */
    صحیح
compile_lhs(
	char_u	    *var_آغاز,
	lhs_T	    *lhs,
	cmdidx_T    cmdidx,
	صحیح	    heredoc,
	صحیح	    has_cmd,	    // "var" before "var_start"
	صحیح	    oplen,
	cctx_T	    *cctx)
{
    char_u	*var_انتہا = تہی;
    صحیح		is_decl = is_decl_command(cmdidx);

    اگر (lhs_init(lhs, var_آغاز, is_decl, heredoc, &var_انتہا) == FAIL)
	واپس FAIL;

    اگر (cctx->ctx_skip != SKIP_YES)
    {
	// compile the LHS destination
	اگر (compile_lhs_var_dest(cctx, lhs, cmdidx, var_آغاز, var_انتہا,
						is_decl, has_cmd) == FAIL)
	    واپس FAIL;
    }

    // handle "a:name" as a name, not index "name" in "a"
    اگر (lhs->lhs_varlen > 1 || var_آغاز[lhs->lhs_varlen] != ':')
	var_انتہا = lhs->lhs_dest_انتہا;

    اگر (lhs->lhs_dest != dest_option && lhs->lhs_dest != dest_func_option)
    {
	// set the LHS variable type
	اگر (compile_lhs_سیٹ_type(cctx, lhs, var_انتہا, is_decl) == FAIL)
	    واپس FAIL;
    }

    اگر (oplen == 3 && !heredoc && !lhs_concatenable(lhs))
    {
	emsg(_(e_can_only_concatenate_مقصد_string));
	واپس FAIL;
    }

    اگر (lhs->lhs_lvar == تہی && lhs->lhs_dest == dest_local
						&& cctx->ctx_skip != SKIP_YES)
    {
	اگر (compile_lhs_new_local_var(cctx, lhs, var_آغاز, cmdidx, oplen,
					is_decl, has_cmd, heredoc) == FAIL)
	    واپس FAIL;
    }

    اگر (compile_lhs_سیٹ_member_type(cctx, lhs, var_آغاز, is_decl, has_cmd)
								== FAIL)
	واپس FAIL;

    واپس OK;
}

/*
 * Figure out the LHS and check a few errors.
 */
    صحیح
compile_تفویض_lhs(
	char_u	    *var_آغاز,
	lhs_T	    *lhs,
	cmdidx_T    cmdidx,
	صحیح	    is_decl,
	صحیح	    heredoc,
	صحیح	    has_cmd,	    // "var" before "var_start"
	صحیح	    oplen,
	cctx_T	    *cctx)
{
    اگر (compile_lhs(var_آغاز, lhs, cmdidx, heredoc, has_cmd, oplen, cctx)
								       == FAIL)
	واپس FAIL;

    اگر (!lhs->lhs_has_اشاریہ && lhs->lhs_lvar == &lhs->lhs_arg_lvar)
    {
	semsg(_(e_cannot_تفویض_مقصد_دلیل_str), lhs->lhs_name);
	واپس FAIL;
    }
    اگر (!is_decl && lhs->lhs_lvar != تہی
			   && lhs->lhs_lvar->lv_const != تفویض_VAR
			   && !lhs->lhs_has_اشاریہ)
    {
	semsg(_(e_cannot_تفویض_مقصد_constant_str), lhs->lhs_name);
	واپس FAIL;
    }
    واپس OK;
}

/*
 * Return TRUE if "lhs" has a range index: "[expr : expr]".
 */
    ساکن صحیح
has_list_اشاریہ(char_u *idx_آغاز, cctx_T *cctx)
{
    char_u  *p = idx_آغاز;
    صحیح	    save_skip;

    اگر (*p != '[')
	واپس FALSE;

    p = skipwhite(p + 1);
    اگر (*p == ':')
	واپس TRUE;

    save_skip = cctx->ctx_skip;
    cctx->ctx_skip = SKIP_YES;
    (خالی)compile_expr0(&p, cctx);
    cctx->ctx_skip = save_skip;
    واپس *skipwhite(p) == ':';
}

/*
 * For an assignment with an index, compile the "idx" in "var[idx]" or "key" in
 * "var.key".
 */
    ساکن صحیح
compile_تفویض_اشاریہ(
	char_u	*var_آغاز,
	lhs_T	*lhs,
	صحیح	*range,
	cctx_T	*cctx)
{
    size_t	varlen = lhs->lhs_varlen;
    char_u	*p;
    صحیح		r = OK;
    صحیح		need_white_before = TRUE;
    صحیح		خالی_خانے_دوسرا;

    p = var_آغاز + varlen;
    اگر (*p == '[')
    {
	p = skipwhite(p + 1);
	اگر (*p == ':')
	{
	    // empty first index, push zero
	    r = generate_PUSHNR(cctx, 0);
	    need_white_before = FALSE;
	}
	ورنہ
	    r = compile_expr0(&p, cctx);

	اگر (r == OK && *skipwhite(p) == ':')
	{
	    // unlet var[idx : idx]
	    // blob[idx : idx] = value
	    *range = TRUE;
	    p = skipwhite(p);
	    خالی_خانے_دوسرا = *skipwhite(p + 1) == ']';
	    اگر ((need_white_before && !IS_WHITE_OR_NUL(p[-1]))
		    || (!خالی_خانے_دوسرا && !IS_WHITE_OR_NUL(p[1])))
	    {
		semsg(_(e_white_space_required_before_and_after_str_at_str),
								      ":", p);
		واپس FAIL;
	    }
	    p = skipwhite(p + 1);
	    اگر (*p == ']')
		// empty second index, push "none"
		r = generate_PUSHSPEC(cctx, VVAL_NONE);
	    ورنہ
		r = compile_expr0(&p, cctx);
	}

	اگر (r == OK && *skipwhite(p) != ']')
	{
	    // this should not happen
	    emsg(_(e_missing_closing_مربع_brace));
	    r = FAIL;
	}
    }
    ورنہ اگر (lhs->lhs_member_idx >= 0)
    {
	// object member index
	r = generate_PUSHNR(cctx, lhs->lhs_member_idx);
    }
    ورنہ // if (*p == '.')
    {
	char_u *کلید_انتہا = مقصد_name_انتہا(p + 1, TRUE);
	char_u *کلید = vim_strnsave(p + 1, کلید_انتہا - p - 1);

	r = generate_PUSHS(cctx, &کلید);
    }
    واپس r;
}

/*
 * For a LHS with an index, load the variable to be indexed.
 */
    ساکن صحیح
compile_بھرو_lhs(
	lhs_T	*lhs,
	char_u	*var_آغاز,
	type_T	*rhs_type,
	cctx_T	*cctx)
{
    اگر (lhs->lhs_dest == dest_اظہاریہ)
    {
	size_t	    varlen = lhs->lhs_varlen;
	صحیح	    c = var_آغاز[varlen];
	صحیح	    سطریں_len = cctx->ctx_ufunc->uf_سطریں.ga_len;
	صحیح	    res;

	// Evaluate "ll[expr]" of "ll[expr][idx]".  End the line with a NUL and
	// limit the lines array length to avoid skipping to a following line.
	var_آغاز[varlen] = NUL;
	cctx->ctx_ufunc->uf_سطریں.ga_len = cctx->ctx_lnum + 1;
	char_u *p = var_آغاز;
	res = compile_expr0(&p, cctx);
	var_آغاز[varlen] = c;
	cctx->ctx_ufunc->uf_سطریں.ga_len = سطریں_len;
	اگر (res == FAIL || p != var_آغاز + varlen)
	{
	    // this should not happen
	    اگر (res != FAIL)
		emsg(_(e_missing_closing_مربع_brace));
	    واپس FAIL;
	}

	lhs->lhs_type = cctx->ctx_type_ڈھیر.ga_len == 0 ? &t_void
						  : get_type_on_ڈھیر(cctx, 0);

	اگر (lhs->lhs_type->tt_type == VAR_CLASS
		|| (lhs->lhs_type->tt_type == VAR_OBJECT
		    && lhs->lhs_type != &t_object_any))
	{
	    // Check whether the class or object variable is modifiable
	    اگر (!lhs_class_member_modifiable(lhs, var_آغاز, cctx))
		واپس FAIL;
	}

	// Now we can properly check the type.  The variable is indexed, thus
	// we need the member type.  For a class or object we don't know the
	// type yet, it depends on what member is used.
	// The top item in the stack is the Dict, followed by the key and then
	// the type of the value.
	vartype_T vartype = lhs->lhs_type->tt_type;
	type_T *member_type = lhs->lhs_type->tt_member;
	اگر (rhs_type != تہی && member_type != تہی
		&& vartype != VAR_OBJECT && vartype != VAR_CLASS
		&& rhs_type != &t_void
		&& need_type(rhs_type, member_type, 0,
					    -3, 0, cctx, FALSE, FALSE) == FAIL)
	    واپس FAIL;

	واپس OK;
    }

    واپس generate_loadvar(cctx, lhs);
}

/*
 * Produce code for loading "lhs" and also take care of an index.
 * Return OK/FAIL.
 */
    صحیح
compile_بھرو_lhs_with_اشاریہ(lhs_T *lhs, char_u *var_آغاز, cctx_T *cctx)
{
    اگر (lhs->lhs_type->tt_type == VAR_OBJECT)
    {
	// "this.value": load "this" object and get the value at index for an
	// object or class member get the type of the member.
	// Also for "obj.value".
	char_u *dot = vim_strchr(var_آغاز, '.');
	اگر (dot == تہی)
	{
	    semsg(_(e_missing_dot_after_object_str), lhs->lhs_name);
	    واپس FAIL;
	}

	class_T	*cl = lhs->lhs_type->tt_class;
	type_T	*type = oc_member_type(cl, TRUE, dot + 1,
					  lhs->lhs_انتہا, &lhs->lhs_member_idx);
	اگر (lhs->lhs_member_idx < 0)
	    واپس FAIL;

	اگر (dot - var_آغاز == 4 && STRNCMP(var_آغاز, "this", 4) == 0)
	{
	    // load "this"
	    lvar_T  *lvar = lhs->lhs_lvar;
	    صحیح	    rc;

	    اگر (lvar->lv_from_outer > 0)
		rc = generate_LOADOUTER(cctx, lvar->lv_idx,
			lvar->lv_from_outer, lvar->lv_loop_گہرائی,
			lvar->lv_loop_idx, type);
	    ورنہ
		rc = generate_بھرو(cctx, ISN_بھرو, lvar->lv_idx, تہی, type);

	    اگر (rc == FAIL)
		واپس FAIL;
	}
	ورنہ
	{
	    // load object variable or argument
	    اگر (compile_بھرو_lhs(lhs, var_آغاز, lhs->lhs_type, cctx) == FAIL)
		واپس FAIL;
	}
	اگر (IS_INTERFACE(cl))
	    واپس generate_GET_ITF_MEMBER(cctx, cl, lhs->lhs_member_idx, type);
	واپس generate_GET_OBJ_MEMBER(cctx, lhs->lhs_member_idx, type);
    }
    ورنہ اگر (lhs->lhs_type->tt_type == VAR_CLASS)
    {
	// "<classname>.value": load class variable "classname.value"
	char_u *dot = vim_strchr(var_آغاز, '.');
	اگر (dot == تہی)
	{
	    (خالی)check_type_is_قدر(lhs->lhs_type);
	    واپس FAIL;
	}

	class_T	*cl = lhs->lhs_type->tt_class;
	ocmember_T *m = class_member_lookup(cl, dot + 1,
						lhs->lhs_انتہا - dot - 1,
						&lhs->lhs_member_idx);
	اگر (m == تہی)
	    واپس FAIL;

	واپس generate_CLASSMEMBER(cctx, TRUE, cl, lhs->lhs_member_idx);
    }

    اگر (compile_بھرو_lhs(lhs, var_آغاز, تہی, cctx) == FAIL)
	واپس FAIL;

    اگر (lhs->lhs_has_اشاریہ)
    {
	صحیح	range = FALSE;
	type_T	*type = get_type_on_ڈھیر(cctx, 0);

	// A member of an object or class is not obtained by indexing it.
	اگر (type->tt_type == VAR_CLASS
		|| (type->tt_type == VAR_OBJECT && type != &t_object_any))
	{
	    char_u *p = var_آغاز + lhs->lhs_varlen;

	    واپس compile_class_object_اشاریہ(cctx, &p, type);
	}

	// Get member from list or dict.  First compile the
	// index value.
	اگر (compile_تفویض_اشاریہ(var_آغاز, lhs, &range, cctx) == FAIL)
	    واپس FAIL;
	اگر (range)
	{
	    semsg(_(e_cannot_use_range_with_assignment_operator_str),
								    var_آغاز);
	    واپس FAIL;
	}

	// Get the member.
	اگر (compile_member(FALSE, تہی, cctx) == FAIL)
	    واپس FAIL;
    }
    واپس OK;
}

/*
 * Assignment to a list or dict member, or ":unlet" for the item, using the
 * information in "lhs".
 * Returns OK or FAIL.
 */
    صحیح
compile_تفویض_unlet(
    char_u	*var_آغاز,
    lhs_T	*lhs,
    صحیح		is_تفویض,
    type_T	*rhs_type,
    cctx_T	*cctx)
{
    vartype_T	dest_type;
    صحیح		range = FALSE;

    اگر (compile_تفویض_اشاریہ(var_آغاز, lhs, &range, cctx) == FAIL)
	واپس FAIL;
    اگر (is_تفویض && range
	    && lhs->lhs_type->tt_type != VAR_LIST
	    && lhs->lhs_type != &t_blob
	    && lhs->lhs_type != &t_any)
    {
	اگر (lhs->lhs_type->tt_type == VAR_TUPLE)
	    emsg(_(e_cannot_قاش_tuple));
	ورنہ
	    semsg(_(e_cannot_use_range_with_assignment_str), var_آغاز);
	واپس FAIL;
    }

    // For "expr[idx]" the index is compiled before the expression, thus the
    // resulting type cannot be used here.
    اگر (lhs->lhs_dest == dest_اظہاریہ
	    || lhs->lhs_type == تہی || lhs->lhs_type == &t_any)
    {
	// Index on variable of unknown type: check at runtime.
	dest_type = VAR_ANY;
    }
    ورنہ
    {
	dest_type = lhs->lhs_type->tt_type;
	اگر (dest_type == VAR_DICT && range)
	{
	    emsg(_(e_cannot_use_range_with_dictionary));
	    واپس FAIL;
	}
	اگر (dest_type == VAR_DICT
		&& may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
	    واپس FAIL;
	اگر (dest_type == VAR_LIST || dest_type == VAR_BLOB)
	{
	    type_T *type;

	    اگر (range)
	    {
		type = get_type_on_ڈھیر(cctx, 1);
		اگر (need_type(type, &t_نمبر, 0,
					    -2, 0, cctx, FALSE, FALSE) == FAIL)
		واپس FAIL;
	    }
	    type = get_type_on_ڈھیر(cctx, 0);
	    اگر ((dest_type != VAR_BLOB && type->tt_type != VAR_SPECIAL)
		    && need_type(type, &t_نمبر, 0,
					    -1, 0, cctx, FALSE, FALSE) == FAIL)
		واپس FAIL;
	}
    }

    اگر (cctx->ctx_skip == SKIP_YES)
	واپس OK;

    // Load the dict, list or object.  On the stack we then have:
    // - value (for assignment, not for :unlet)
    // - index
    // - for [a : b] second index
    // - variable
    اگر (compile_بھرو_lhs(lhs, var_آغاز, rhs_type, cctx) == FAIL)
	واپس FAIL;

    اگر (dest_type == VAR_LIST
	    || dest_type == VAR_DICT
	    || dest_type == VAR_BLOB
	    || dest_type == VAR_CLASS
	    || dest_type == VAR_OBJECT
	    || dest_type == VAR_ANY)
    {
	اگر (is_تفویض)
	{
	    اگر (range)
	    {
		اگر (generate_instr_drop(cctx, ISN_STORERANGE, 4) == تہی)
		    واپس FAIL;
	    }
	    ورنہ
	    {
		isn_T	*isn = generate_instr_drop(cctx, ISN_STOREINDEX, 3);

		اگر (isn == تہی)
		    واپس FAIL;
		isn->isn_arg.storeindex.si_vartype = dest_type;
		isn->isn_arg.storeindex.si_class = تہی;

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
	ورنہ اگر (range)
	{
	    اگر (generate_instr_drop(cctx, ISN_UNLETRANGE, 3) == تہی)
		واپس FAIL;
	}
	ورنہ
	{
	    اگر (generate_instr_drop(cctx, ISN_UNLETINDEX, 2) == تہی)
		واپس FAIL;
	}
    }
    ورنہ
    {
	اگر (dest_type == VAR_TUPLE)
	    emsg(_(e_tuple_is_immutable));
	ورنہ
	    emsg(_(e_indexable_type_required));
	واپس FAIL;
    }

    واپس OK;
}

/*
 * Generate an instruction to push the default value for "vartype".
 * if "dest_local" is TRUE then for some types no instruction is generated.
 * "skip_store" is set to TRUE if no PUSH instruction is generated.
 * Returns OK or FAIL.
 */
    ساکن صحیح
ڈالو_default_قدر(
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
	    توڑو;
	حالت VAR_FLOAT:
	    r = generate_PUSHF(cctx, 0.0);
	    توڑو;
	حالت VAR_STRING:
	    r = generate_PUSHS(cctx, تہی);
	    توڑو;
	حالت VAR_BLOB:
	    r = generate_PUSHBLOB(cctx, blob_alloc());
	    توڑو;
	حالت VAR_FUNC:
	    r = generate_PUSHFUNC(cctx, تہی, &t_func_void, TRUE);
	    توڑو;
	حالت VAR_LIST:
	    r = generate_NEWLIST(cctx, 0, FALSE);
	    توڑو;
	حالت VAR_TUPLE:
	    r = generate_NEWTUPLE(cctx, 0, FALSE);
	    توڑو;
	حالت VAR_DICT:
	    r = generate_NEWDICT(cctx, 0, FALSE);
	    توڑو;
	حالت VAR_کام:
	    r = generate_PUSHJOB(cctx);
	    توڑو;
	حالت VAR_CHANNEL:
	    r = generate_PUSHCHANNEL(cctx);
	    توڑو;
	حالت VAR_OBJECT:
	    r = generate_PUSHOBJ(cctx);
	    توڑو;
	حالت VAR_نمبر:
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
	    ورنہ
		r = generate_PUSHNR(cctx, 0);
	    توڑو;
    }
    واپس r;
}

/*
 * Compile assignment context.  Used when compiling an assignment statement.
 */
قسمتعریف ساخت cac_S cac_T;
ساخت cac_S
{
    cmdidx_T	cac_cmdidx;		// assignment command
    char_u	*cac_nextc;		// next character to parse
    lhs_T	cac_lhs;		// lhs of the assignment
    type_T	*cac_rhs_type;		// rhs type of an assignment
    char_u	*cac_op;		// assignment operator
    صحیح		cac_oplen;		// assignment operator length
    char_u	*cac_var_آغاز;		// start of the variable names
    char_u	*cac_var_انتہا;		// end of the variable names
    صحیح		cac_var_گنتی;		// number of variables in assignment
    صحیح		cac_var_idx;		// variable index in a list
    صحیح		cac_semicolon;		// semicolon in [var1, var2; var3]
    garray_T	*cac_instr;
    صحیح		cac_instr_گنتی;
    صحیح		cac_incdec;
    صحیح		cac_did_generate_قاش;
    صحیح		cac_is_decl;
    صحیح		cac_is_const;
    صحیح		cac_آغاز_lnum;
    type_T	*cac_inferred_type;
    صحیح		cac_skip_store;
};

/*
 * Initialize the compile assignment context.
 */
    ساکن خالی
compile_تفویض_context_init(cac_T *cac, cctx_T *cctx, صحیح cmdidx, char_u *arg)
{
    CLEAR_میدان(*cac);
    cac->cac_cmdidx = cmdidx;
    cac->cac_instr = &cctx->ctx_instr;
    cac->cac_rhs_type = &t_any;
    cac->cac_is_decl = is_decl_command(cmdidx);
    cac->cac_آغاز_lnum = SOURCING_LNUM;
    cac->cac_instr_گنتی = -1;
    cac->cac_var_انتہا = arg;
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
    ساکن صحیح
compile_تفویض_obj_new_arg(char_u **argp, cctx_T *cctx)
{
    char_u *arg = *argp;

    arg += 11;	    // skip "ifargisset"
    صحیح def_arg_idx = getdigits(&arg);
    arg = skipwhite(arg);

    // Use a JUMP_IF_ARG_NOT_SET instruction to skip if the value was not
    // given and the default value is "v:none".
    صحیح ڈھیر_انحراف = ڈھیر_چوکھٹا_سائز +
				(cctx->ctx_ufunc->uf_va_name != تہی ? 1 : 0);
    صحیح def_arg_گنتی = cctx->ctx_ufunc->uf_def_دلائل.ga_len;
    صحیح arg_انحراف = def_arg_idx - def_arg_گنتی - ڈھیر_انحراف;

    اگر (generate_JUMP_IF_ARG(cctx, ISN_JUMP_IF_ARG_NOT_سیٹ,
			     arg_انحراف) == FAIL)
	واپس FAIL;

    *argp = arg;
    واپس OK;
}

/*
 * Translate the increment (++) and decrement (--) operators to the
 * corresponding compound operators (+= or -=).
 *
 * Returns OK on success and FAIL on syntax error.
 */
    ساکن صحیح
translate_incdec_op(exarg_T *eap, cac_T *cac)
{
    اگر (VIM_ISWHITE(eap->cmd[2]))
    {
	semsg(_(e_no_white_space_allowed_after_str_str),
		eap->cmdidx == CMD_increment ? "++" : "--", eap->cmd);
	واپس FAIL;
    }
    cac->cac_op = (char_u *)(eap->cmdidx == CMD_increment ? "+=" : "-=");
    cac->cac_oplen = 2;
    cac->cac_incdec = TRUE;

    واپس OK;
}

/*
 * Process the operator in an assignment statement.
 */
    ساکن صحیح
compile_تفویض_process_operator(
    exarg_T	*eap,
    char_u	*arg,
    cac_T	*cac,
    صحیح		*heredoc,
    char_u	**retstr)
{
    *retstr = تہی;

    اگر (eap->cmdidx == CMD_increment || eap->cmdidx == CMD_decrement)
	// Change an unary operator to a compound operator
	واپس translate_incdec_op(eap, cac);

    char_u *sp = cac->cac_nextc;
    cac->cac_nextc = skipwhite(cac->cac_nextc);
    cac->cac_op = cac->cac_nextc;
    cac->cac_oplen = assignment_len(cac->cac_nextc, heredoc);

    اگر (cac->cac_var_گنتی > 0 && cac->cac_oplen == 0)
    {
	// can be something like "[1, 2]->func()"
	*retstr = arg;
	واپس FAIL;
    }

    // need white space before and after the operator
    اگر (cac->cac_oplen > 0 && (!VIM_ISWHITE(*sp)
		|| !IS_WHITE_OR_NUL(cac->cac_op[cac->cac_oplen])))
    {
	error_white_both(cac->cac_op, cac->cac_oplen);
	واپس FAIL;
    }

    واپس OK;
}

/*
 * Find the start of an assignment statement.
 */
    ساکن char_u *
compile_تفویض_compute_آغاز(char_u *arg, صحیح var_گنتی)
{
    اگر (var_گنتی > 0)
	// [var1, var2] = [val1, val2]
	// skip over the "["
	واپس skipwhite(arg + 1);

    واپس arg;
}

/*
 * Parse a heredoc assignment starting at "p".  Returns a pointer to the
 * beginning of the heredoc content.
 */
    ساکن char_u *
parse_heredoc_assignment(exarg_T *eap, cctx_T *cctx, cac_T *cac)
{
    // [let] varname =<< [trim] {end}
    eap->ea_getline = exarg_getline;
    eap->cookie = cctx;

    list_T *l = heredoc_get(eap, cac->cac_nextc + 3, FALSE, TRUE);
    اگر (l == تہی)
	واپس تہی;

    list_free(l);
    cac->cac_nextc += STRLEN(cac->cac_nextc);

    واپس cac->cac_nextc;
}

/*
 * Check the type of a RHS expression in a list assignment statement.
 * The RHS expression is already compiled.  So the type is on the stack.
 */
    ساکن صحیح
compile_تفویض_list_check_rhs_type(cctx_T *cctx, cac_T *cac)
{
    type_T	*stacktype;

    stacktype = cctx->ctx_type_ڈھیر.ga_len == 0 ? &t_void
						: get_type_on_ڈھیر(cctx, 0);
    اگر (stacktype->tt_type == VAR_VOID)
    {
	emsg(_(e_cannot_use_void_قدر));
	واپس FAIL;
    }

    اگر (stacktype->tt_type != VAR_LIST && stacktype->tt_type != VAR_TUPLE
					&& stacktype->tt_type != VAR_ANY)
    {
	emsg(_(e_list_or_tuple_required));
	واپس FAIL;
    }

    اگر (need_type(stacktype,
		  stacktype->tt_type == VAR_TUPLE ? &t_tuple_any : &t_list_any,
		  TYPECHK_TUPLE_OK, -1, 0, cctx, FALSE, FALSE) == FAIL)
	واپس FAIL;

    // The check accepts both a list and a tuple. Keep "any", making it a list
    // would reject a tuple later on.
    اگر (stacktype->tt_type == VAR_ANY)
	سیٹ_type_on_ڈھیر(cctx, &t_any, 0);

    اگر (stacktype->tt_type == VAR_TUPLE)
    {
	اگر (stacktype->tt_argcount != 1)
	    cac->cac_rhs_type = &t_any;
	ورنہ
	{
	    اگر (stacktype->tt_flags & TTFLAG_VARARGS)
		cac->cac_rhs_type = stacktype->tt_دلائل[0]->tt_member;
	    ورنہ
		cac->cac_rhs_type = stacktype->tt_دلائل[0];
	}
    }
    ورنہ اگر (stacktype->tt_member != تہی)
	cac->cac_rhs_type = stacktype->tt_member;

    واپس OK;
}

/*
 * In a list assignment statement, if a constant list was used, check the
 * length.  Returns OK if the length check succeeds.  Returns FAIL otherwise.
 */
    ساکن صحیح
compile_تفویض_list_check_لمبائی(cctx_T *cctx, cac_T *cac)
{
    صحیح	needed_list_len;
    صحیح	did_check = FALSE;

    needed_list_len = cac->cac_semicolon
				? cac->cac_var_گنتی - 1
				: cac->cac_var_گنتی;
    اگر (cac->cac_instr->ga_len > 0)
    {
	isn_T	*isn = ((isn_T *)cac->cac_instr->ga_ڈیٹا) +
						cac->cac_instr->ga_len - 1;

	اگر (isn->isn_type == ISN_NEWLIST || isn->isn_type == ISN_NEWTUPLE)
	{
	    did_check = TRUE;
	    اگر (cac->cac_semicolon ?
			isn->isn_arg.نمبر < needed_list_len
			: isn->isn_arg.نمبر != needed_list_len)
	    {
		semsg(_(e_expected_nr_اشیاء_but_got_nr),
			needed_list_len, (صحیح)isn->isn_arg.نمبر);
		واپس FAIL;
	    }
	}
    }

    اگر (!did_check)
	generate_CHECKLEN(cctx, needed_list_len, cac->cac_semicolon);

    واپس OK;
}

/*
 * Evaluate the expression for "[var, var] = expr" assignment.
 * A line break may follow the assignment operator "=".
 */
    ساکن char_u *
compile_تفویض_list_اظہاریہ(cctx_T *cctx, cac_T *cac)
{
    char_u *whitep;

    whitep = cac->cac_op + cac->cac_oplen;

    اگر (may_get_اگلا_سطر_error(whitep, &cac->cac_nextc, cctx) == FAIL)
	واپس تہی;

    // compile RHS expression
    اگر (compile_expr0(&cac->cac_nextc, cctx) == FAIL)
	واپس تہی;

    اگر (cctx->ctx_skip == SKIP_YES)
	// no need to parse more when skipping
	واپس cac->cac_nextc;

    اگر (compile_تفویض_list_check_rhs_type(cctx, cac) == FAIL)
	واپس تہی;

    // If a constant list was used we can check the length right here.
    اگر (compile_تفویض_list_check_لمبائی(cctx, cac) == FAIL)
	واپس FAIL;

    واپس cac->cac_nextc;
}

/*
 * Find and return the end of a heredoc or a list of variables assignment
 * statement.  For a single variable assignment statement, returns the current
 * end.
 * Returns NULL on failure.
 */
    ساکن char_u *
compile_تفویض_compute_انتہا(
    exarg_T	*eap,
    cctx_T	*cctx,
    cac_T	*cac,
    صحیح		heredoc)
{
    اگر (heredoc)
    {
	cac->cac_nextc = parse_heredoc_assignment(eap, cctx, cac);
	واپس cac->cac_nextc;
    }

    اگر (cac->cac_var_گنتی > 0)
    {
	// for "[var, var] = expr" evaluate the expression. The list of
	// variables are processed later.
	// A line break may follow the "=".
	cac->cac_nextc = compile_تفویض_list_اظہاریہ(cctx, cac);
	واپس cac->cac_nextc;
    }

    واپس cac->cac_var_انتہا;
}

/*
 * For "var = expr" evaluate the expression.
 */
    ساکن صحیح
compile_تفویض_single_eval_اظہاریہ(cctx_T *cctx, cac_T *cac)
{
    صحیح		ret = OK;
    char_u	*whitep;
    lhs_T	*lhs = &cac->cac_lhs;
    lvar_T	*lvp;
    lvar_T	save_lhs_lvar;

    // Compile the expression.
    اگر (cac->cac_incdec)
	واپس generate_PUSHNR(cctx, 1);

    // Temporarily hide the new local variable here, it is
    // not available to this expression.
    اگر (lhs->lhs_new_local)
    {
	--cctx->ctx_locals.ga_len;

	// Save the local variable value (compiling the RHS expression may
	// create new local variables).
	lvp = ((lvar_T *)cctx->ctx_locals.ga_ڈیٹا) + cctx->ctx_locals.ga_len;
	save_lhs_lvar = *lvp;
    }
    whitep = cac->cac_op + cac->cac_oplen;

    اگر (may_get_اگلا_سطر_error(whitep, &cac->cac_nextc, cctx) == FAIL)
    {
	اگر (lhs->lhs_new_local)
	    ++cctx->ctx_locals.ga_len;
	واپس FAIL;
    }

    ret = compile_expr0_ext(&cac->cac_nextc, cctx, &cac->cac_is_const);
    اگر (lhs->lhs_new_local)
    {
	// Restore the local variable value.  Update lhs_lvar as the index of
	// the local variable might have changed.
	lvp = ((lvar_T *)cctx->ctx_locals.ga_ڈیٹا) + cctx->ctx_locals.ga_len;
	*lvp = save_lhs_lvar;
	lhs->lhs_lvar = lvp;

	++cctx->ctx_locals.ga_len;
    }

    واپس ret;
}

/*
 * When compiling an assignment, set the LHS type to the RHS type.
 */
    ساکن صحیح
compile_تفویض_سیٹ_lhs_type_from_rhs(
    cctx_T	*cctx,
    cac_T	*cac,
    lhs_T	*lhs,
    type_T	*rhs_type)
{
    اگر (rhs_type->tt_type == VAR_VOID)
    {
	emsg(_(e_cannot_use_void_قدر));
	واپس FAIL;
    }

    type_T *type;

    // An empty list or dict has a &t_unknown member, for a variable that
    // implies &t_any.
    اگر (rhs_type == &t_list_خالی_خانے)
	type = &t_list_any;
    ورنہ اگر (rhs_type == &t_dict_خالی_خانے)
	type = &t_dict_any;
    ورنہ اگر (rhs_type == &t_unknown)
	type = &t_any;
    ورنہ
    {
	type = rhs_type;
	cac->cac_inferred_type = rhs_type;
    }

    سیٹ_var_type(lhs->lhs_lvar, type, cctx);

    واپس OK;
}

/*
 * Returns TRUE if the "rhs_type" can be assigned to the "lhs" variable.
 * Used when compiling an assignment statement.
 */
    ساکن صحیح
compile_تفویض_valid_rhs_type(
    cctx_T	*cctx,
    cac_T	*cac,
    lhs_T	*lhs,
    type_T	*rhs_type)
{
    type_T	*use_type = lhs->lhs_lvar->lv_type;
    where_T	where = WHERE_INIT;

    // Without operator check type here, otherwise below.
    // Use the line number of the assignment.
    SOURCING_LNUM = cac->cac_آغاز_lnum;
    اگر (cac->cac_var_گنتی > 0)
    {
	where.wt_اشاریہ = cac->cac_var_idx + 1;
	where.wt_قسم = WT_VARIABLE;
    }

    // If assigning to a list or dict member, use the member type.
    // Not for "list[:] =".
    اگر (lhs->lhs_has_اشاریہ &&
	    !has_list_اشاریہ(cac->cac_var_آغاز + lhs->lhs_varlen, cctx))
	use_type = lhs->lhs_member_type;

    اگر (need_type_where(rhs_type, use_type, 0, -1, where, cctx, FALSE,
						cac->cac_is_const) == FAIL)
	واپس FALSE;

    واپس TRUE;
}

/*
 * Compare the LHS type with the RHS type in an assignment.
 */
    ساکن صحیح
compile_تفویض_check_type(cctx_T *cctx, cac_T *cac)
{
    lhs_T	*lhs = &cac->cac_lhs;
    type_T	*rhs_type;

    rhs_type = cctx->ctx_type_ڈھیر.ga_len == 0
					? &t_void
					: get_type_on_ڈھیر(cctx, 0);
    cac->cac_rhs_type = rhs_type;

    اگر (check_type_is_قدر(rhs_type) == FAIL)
	واپس FAIL;

    اگر (lhs->lhs_lvar != تہی && (cac->cac_is_decl || !lhs->lhs_has_type))
    {
	اگر (rhs_type->tt_type == VAR_FUNC
					|| rhs_type->tt_type == VAR_PARTIAL)
	{
	    // Make sure the variable name can be used as a funcref
	    اگر (!lhs->lhs_has_اشاریہ
				&& var_wrong_func_name(lhs->lhs_name, TRUE))
		واپس FAIL;
	}

	اگر (lhs->lhs_new_local && !lhs->lhs_has_type)
	{
	    // The LHS variable doesn't have a type.  Set it to the RHS type.
	    اگر (compile_تفویض_سیٹ_lhs_type_from_rhs(cctx, cac, lhs, rhs_type)
								== FAIL)
		واپس FAIL;
	}
	ورنہ اگر (*cac->cac_op == '=')
	{
	    اگر (!compile_تفویض_valid_rhs_type(cctx, cac, lhs, rhs_type))
		واپس FAIL;
	}
    }
    ورنہ
    {
	// Assigning to a register using @r = "abc"

	type_T *lhs_type = lhs->lhs_member_type;

	// Special case: assigning to @# can use a number or a string.
	// Also: can assign a number to a float.
	اگر ((lhs_type == &t_نمبر_or_string || lhs_type == &t_float)
					&& rhs_type->tt_type == VAR_نمبر)
	    lhs_type = &t_نمبر;

	اگر (*cac->cac_nextc != '=')
	{
	    اگر (need_type(rhs_type, lhs_type, 0, -1, 0, cctx, FALSE,
							FALSE) == FAIL)
		واپس FAIL;
	}
    }

    واپس OK;
}

/*
 * Compile the RHS expression in an assignment statement and generate the
 * instructions.
 */
    ساکن صحیح
compile_تفویض_rhs_اظہاریہ(cctx_T *cctx, cac_T *cac)
{
    cac->cac_is_const = FALSE;

    // for "+=", "*=", "..=" etc. first load the current value
    اگر (*cac->cac_op != '='
	    && compile_بھرو_lhs_with_اشاریہ(&cac->cac_lhs, cac->cac_var_آغاز,
								cctx) == FAIL)
	واپس FAIL;

    // For "var = expr" evaluate the expression.
    اگر (cac->cac_var_گنتی == 0)
    {
	صحیح	ret;

	// Compile the expression.
	cac->cac_instr_گنتی = cac->cac_instr->ga_len;
	ret = compile_تفویض_single_eval_اظہاریہ(cctx, cac);
	اگر (ret == FAIL)
	    واپس FAIL;
    }
    ورنہ اگر (cac->cac_semicolon && cac->cac_var_idx == cac->cac_var_گنتی - 1)
    {
	// For "[var; var] = expr" get the rest of the list
	cac->cac_did_generate_قاش = TRUE;
	اگر (generate_قاش(cctx, cac->cac_var_گنتی - 1) == FAIL)
	    واپس FAIL;
    }
    ورنہ
    {
	// For "[var, var] = expr" get the "var_idx" item from the
	// list.
	صحیح with_op = *cac->cac_op != '=';
	اگر (generate_GETITEM(cctx, cac->cac_var_idx, with_op) == FAIL)
	    واپس FAIL;
    }

    اگر (compile_تفویض_check_type(cctx, cac) == FAIL)
	واپس FAIL;

    واپس OK;
}

/*
 * Compile the RHS expression in an assignment
 */
    ساکن صحیح
compile_تفویض_rhs(cctx_T *cctx, cac_T *cac)
{
    lhs_T	*lhs = &cac->cac_lhs;

    اگر (cctx->ctx_skip == SKIP_YES)
    {
	اگر (cac->cac_oplen > 0 && cac->cac_var_گنتی == 0)
	{
	    // skip over the "=" and the expression
	    cac->cac_nextc = skipwhite(cac->cac_op + cac->cac_oplen);
	    (خالی)compile_expr0(&cac->cac_nextc, cctx);
	}
	واپس OK;
    }

    // If RHS is specified, then generate instructions for RHS expression
    اگر (cac->cac_oplen > 0)
	واپس compile_تفویض_rhs_اظہاریہ(cctx, cac);

    اگر (cac->cac_cmdidx == CMD_final)
    {
	emsg(_(e_final_requires_a_قدر));
	واپس FAIL;
    }

    اگر (cac->cac_cmdidx == CMD_const)
    {
	emsg(_(e_const_requires_a_قدر));
	واپس FAIL;
    }

    اگر (!lhs->lhs_has_type || lhs->lhs_dest == dest_option
					|| lhs->lhs_dest == dest_func_option)
    {
	emsg(_(e_type_or_initialization_required));
	واپس FAIL;
    }

    // variables are always initialized
    اگر (GA_GROW_FAILS(cac->cac_instr, 1))
	واپس FAIL;

    cac->cac_instr_گنتی = cac->cac_instr->ga_len;

    واپس ڈالو_default_قدر(cctx, lhs->lhs_member_type->tt_type,
			      lhs->lhs_dest == dest_local,
			      &cac->cac_skip_store);
}

/*
 * Returns OK if "type" supports compound operator "op_arg" (e.g. +=, -=, %=,
 * etc.).  Compound operators are not supported with a tuple and a dict.
 * Returns FAIL if compound operator is not supported.
 */
    ساکن صحیح
check_type_supports_compound_op(type_T *type, char_u op_arg)
{
    اگر (type->tt_type == VAR_TUPLE || type->tt_type == VAR_DICT)
    {
	char_u	op[2];

	op[0] = op_arg;
	op[1] = NUL;
	semsg(_(e_wrong_variable_type_for_str_equal), op);
	واپس FAIL;
    }

    واپس OK;
}

/*
 * Compile a compound op assignment statement (+=, -=, *=, %=, etc.)
 */
    ساکن صحیح
compile_تفویض_compound_op(cctx_T *cctx, cac_T *cac)
{
    lhs_T	    *lhs = &cac->cac_lhs;
    type_T	    *expected;
    type_T	    *stacktype = تہی;

    اگر (cac->cac_lhs.lhs_type->tt_type == VAR_TUPLE
	    && check_type_supports_compound_op(cac->cac_lhs.lhs_type,
							*cac->cac_op) == FAIL)
	واپس FAIL;

    اگر (*cac->cac_op == '.')
    {
	expected = lhs->lhs_member_type;
	stacktype = get_type_on_ڈھیر(cctx, 0);
	اگر (expected != &t_string
		&& need_type(stacktype, expected, 0, -1, 0, cctx,
					FALSE, FALSE) == FAIL)
	    واپس FAIL;
	ورنہ اگر (may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
	    واپس FAIL;
    }
    ورنہ
    {
	expected = lhs->lhs_member_type;
	stacktype = get_type_on_ڈھیر(cctx, 0);

	اگر (check_type_supports_compound_op(expected, *cac->cac_op) == FAIL)
	    واپس FAIL;

	اگر (
		// If variable is float operation with number is OK.
		!(expected == &t_float && (stacktype == &t_نمبر
					|| stacktype == &t_نمبر_bool))
		&& need_type(stacktype, expected, TYPECHK_نمبر_OK, -1, 0,
						cctx, FALSE, FALSE) == FAIL)
	    واپس FAIL;
    }

    اگر (*cac->cac_op == '.')
    {
	اگر (generate_CONCAT(cctx, 2) == FAIL)
	    واپس FAIL;
    }
    ورنہ اگر (*cac->cac_op == '+')
    {
	اگر (generate_جوڑو_instr(cctx,
		    operator_type(lhs->lhs_member_type, stacktype),
		    lhs->lhs_member_type, stacktype,
		    اظہاریہ_APPEND) == FAIL)
	    واپس FAIL;
    }
    ورنہ اگر (generate_two_op(cctx, cac->cac_op) == FAIL)
	واپس FAIL;

    واپس OK;
}

/*
 * Generate the STORE and SETTYPE instructions for an assignment statement.
 */
    ساکن صحیح
compile_تفویض_generate_store(cctx_T *cctx, cac_T *cac)
{
    lhs_T	*lhs = &cac->cac_lhs;
    صحیح		save_lnum;

    // Use the line number of the assignment for store instruction.
    save_lnum = cctx->ctx_lnum;
    cctx->ctx_lnum = cac->cac_آغاز_lnum - 1;

    اگر (lhs->lhs_has_اشاریہ)
    {
	// Use the info in "lhs" to store the value at the index in the
	// list, dict or object.
	اگر (compile_تفویض_unlet(cac->cac_var_آغاز, &cac->cac_lhs,
				 TRUE, cac->cac_rhs_type, cctx) == FAIL)
	{
	    cctx->ctx_lnum = save_lnum;
	    واپس FAIL;
	}
    }
    ورنہ
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
		&& lhs->lhs_type->tt_member != تہی
		&& lhs->lhs_type->tt_member != &t_any
		&& lhs->lhs_type->tt_member != &t_unknown)
	    // Set the type in the list or dict, so that it can be
	    // checked, also in legacy script.
	    generate_SETTYPE(cctx, lhs->lhs_type);
	ورنہ اگر (lhs->lhs_type->tt_type == VAR_TUPLE
					&& lhs->lhs_type->tt_argcount != 0)
	    generate_SETTYPE(cctx, lhs->lhs_type);
	ورنہ اگر (inferred_type != تہی
		&& (inferred_type->tt_type == VAR_DICT
		    || inferred_type->tt_type == VAR_LIST)
		&& inferred_type->tt_member != تہی
		&& inferred_type->tt_member != &t_unknown
		&& inferred_type->tt_member != &t_any)
	    // Set the type in the list or dict, so that it can be
	    // checked, also in legacy script.
	    generate_SETTYPE(cctx, inferred_type);
	ورنہ اگر (inferred_type != تہی
				&& inferred_type->tt_type == VAR_TUPLE
				&& inferred_type->tt_argcount > 0)
	    generate_SETTYPE(cctx, inferred_type);

	اگر (!cac->cac_skip_store &&
		generate_store_lhs(cctx, &cac->cac_lhs,
				   cac->cac_instr_گنتی,
				   cac->cac_is_decl) == FAIL)
	{
	    cctx->ctx_lnum = save_lnum;
	    واپس FAIL;
	}
    }

    cctx->ctx_lnum = save_lnum;

    واپس OK;
}

/*
 * Process the variable(s) in an assignment statement
 */
    ساکن صحیح
compile_تفویض_process_variables(
    cctx_T	*cctx,
    cac_T	*cac,
    صحیح		cmdidx,
    صحیح		heredoc,
    صحیح		has_cmd,
    صحیح		has_argisset_سابقہ,
    صحیح		jump_instr_idx)
{
    /*
     * Loop over variables in "[var, var] = expr".
     * For "name = expr" and "var name: type" this is done only once.
     */
    برائے (cac->cac_var_idx = 0; cac->cac_var_idx == 0 ||
	    cac->cac_var_idx < cac->cac_var_گنتی; cac->cac_var_idx++)
    {
	اگر (cac->cac_var_آغاز[0] == '_'
				&& !eval_isnamec(cac->cac_var_آغاز[1]))
	{
	    // Ignore underscore in "[a, _, b] = list".
	    اگر (cac->cac_var_گنتی > 0)
	    {
		cac->cac_var_آغاز = skipwhite(cac->cac_var_آغاز + 2);
		جاری;
	    }
	    emsg(_(e_cannot_use_underscore_یہاں));
	    واپس FAIL;
	}
	vim_free(cac->cac_lhs.lhs_name);

	/*
	 * Figure out the LHS type and other properties.
	 */
	اگر (compile_تفویض_lhs(cac->cac_var_آغاز, &cac->cac_lhs, cmdidx,
		    cac->cac_is_decl, heredoc, has_cmd,
		    cac->cac_oplen, cctx) == FAIL)
	    واپس FAIL;

	// Compile the RHS expression
	اگر (heredoc)
	{
	    SOURCING_LNUM = cac->cac_آغاز_lnum;
	    اگر (cac->cac_lhs.lhs_has_type
		    && need_type(&t_list_string, cac->cac_lhs.lhs_type,
				0, -1, 0, cctx, FALSE, FALSE) == FAIL)
		واپس FAIL;
	}
	ورنہ
	{
	    اگر (compile_تفویض_rhs(cctx, cac) == FAIL)
		واپس FAIL;
	    اگر (cac->cac_var_گنتی == 0)
		cac->cac_var_انتہا = cac->cac_nextc;
	}

	// no need to parse more when skipping
	اگر (cctx->ctx_skip == SKIP_YES)
	    توڑو;

	اگر (cac->cac_oplen > 0 && *cac->cac_op != '=')
	{
	    اگر (compile_تفویض_compound_op(cctx, cac) == FAIL)
		واپس FAIL;
	}

	// generate the store instructions
	اگر (compile_تفویض_generate_store(cctx, cac) == FAIL)
	    واپس FAIL;

	اگر (cac->cac_var_idx + 1 < cac->cac_var_گنتی)
	    cac->cac_var_آغاز = skipwhite(cac->cac_lhs.lhs_انتہا + 1);

	اگر (has_argisset_سابقہ)
	{
	    // set instruction index in JUMP_IF_ARG_SET to here
	    isn_T *isn = ((isn_T *)cac->cac_instr->ga_ڈیٹا) + jump_instr_idx;
	    isn->isn_arg.jumparg.jump_where = cac->cac_instr->ga_len;
	}
    }

    واپس OK;
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
    ساکن char_u *
compile_assignment(
    char_u	*arg_آغاز,
    exarg_T	*eap,
    cmdidx_T	cmdidx,
    cctx_T	*cctx)
{
    cac_T	cac;
    char_u	*arg = arg_آغاز;
    char_u	*retstr = تہی;
    صحیح		heredoc = FALSE;
    صحیح		jump_instr_idx;

    compile_تفویض_context_init(&cac, cctx, cmdidx, arg);

    jump_instr_idx = cac.cac_instr->ga_len;

    // process object variable initialization in a new() constructor method
    صحیح	has_argisset_سابقہ = STRNCMP(arg, "ifargisset ", 11) == 0;
    اگر (has_argisset_سابقہ &&
			compile_تفویض_obj_new_arg(&arg, cctx) == FAIL)
	جاؤ theend;

    // Skip over the "varname" or "[varname, varname]" to get to any "=".
    cac.cac_nextc = skip_var_list(arg, TRUE, &cac.cac_var_گنتی,
						&cac.cac_semicolon, TRUE);
    اگر (cac.cac_nextc == تہی)
	واپس *arg == '[' ? arg : تہی;

    اگر (compile_تفویض_process_operator(eap, arg, &cac, &heredoc,
							&retstr) == FAIL)
	واپس retstr;

    // Compute the start of the assignment
    cac.cac_var_آغاز = compile_تفویض_compute_آغاز(arg, cac.cac_var_گنتی);

    // Compute the end of the assignment
    cac.cac_var_انتہا = compile_تفویض_compute_انتہا(eap, cctx, &cac, heredoc);
    اگر (cac.cac_var_انتہا == تہی)
	واپس تہی;

    صحیح has_cmd = cac.cac_var_آغاز > eap->cmd;

    /* process the variable(s) */
    اگر (compile_تفویض_process_variables(cctx, &cac, cmdidx, heredoc,
					 has_cmd, has_argisset_سابقہ,
					 jump_instr_idx) == FAIL)
	جاؤ theend;

    // For "[var, var] = expr" drop the "expr" value.
    // Also for "[var, var; _] = expr".
    اگر (cctx->ctx_skip != SKIP_YES && cac.cac_var_گنتی > 0 &&
	    (!cac.cac_semicolon || !cac.cac_did_generate_قاش))
    {
	اگر (generate_instr_drop(cctx, ISN_DROP, 1) == تہی)
	    جاؤ theend;
    }

    retstr = skipwhite(cac.cac_var_انتہا);

theend:
    vim_free(cac.cac_lhs.lhs_name);
    واپس retstr;
}

/*
 * Check for an assignment at "eap->cmd", compile it if found.
 * Return NOTDONE if there is none, FAIL for failure, OK if done.
 */
    ساکن صحیح
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
    p = مقصد_name_انتہا(pskip, TRUE);
    اگر (p > eap->cmd && *p != NUL)
    {
	char_u *var_انتہا;
	صحیح	oplen;
	صحیح	heredoc;

	اگر (eap->cmd[0] == '@')
	    var_انتہا = eap->cmd + 2;
	ورنہ
	    var_انتہا = ڈھونڈو_name_انتہا(pskip, تہی, تہی,
					FNE_CHECK_آغاز | FNE_INCL_BR);
	oplen = assignment_len(skipwhite(var_انتہا), &heredoc);
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
		*سطر = compile_assignment(eap->cmd, eap, CMD_سائز, cctx);
		اگر (*سطر == تہی || *سطر == eap->cmd)
		    واپس FAIL;
		واپس OK;
	    }
	}
    }

    // might be "[var, var] = expr" or "ifargisset this.member = expr"
    اگر (*eap->cmd == '[' || STRNCMP(eap->cmd, "ifargisset ", 11) == 0)
    {
	*سطر = compile_assignment(eap->cmd, eap, CMD_سائز, cctx);
	اگر (*سطر == تہی)
	    واپس FAIL;
	اگر (*سطر != eap->cmd)
	    واپس OK;
    }
    واپس NOTDONE;
}

/*
 * Check if arguments of "ufunc" shadow variables in "cctx".
 * Return OK or FAIL.
 */
    ساکن صحیح
check_دلائل_shadowing(ufunc_T *ufunc, cctx_T *cctx)
{
    صحیح	    i;
    char_u  *arg;
    صحیح	    r = OK;

    // Make sure arguments are not found when compiling a second time.
    ufunc->uf_دلائل_visible = 0;

    // Check for arguments shadowing variables from the context.
    برائے (i = 0; i < ufunc->uf_دلائل.ga_len; ++i)
    {
	arg = ((char_u **)(ufunc->uf_دلائل.ga_ڈیٹا))[i];
	اگر (check_defined(arg, STRLEN(arg), cctx, تہی, TRUE) == FAIL)
	{
	    r = FAIL;
	    توڑو;
	}
    }
    ufunc->uf_دلائل_visible = ufunc->uf_دلائل.ga_len;
    واپس r;
}

#اگر_تعریف_شدہ HAS_پیغام_کھڑکی
/*
 * Get a count before a command.  Can only be a number.
 * Returns zero if there is no count.
 * Returns -1 if there is something wrong.
 */
    ساکن طویل
get_cmd_گنتی(char_u *سطر, exarg_T *eap)
{
    char_u *p;

    // skip over colons and white space
    برائے (p = سطر; *p == ':' || VIM_ISWHITE(*p); ++p)
	;
    اگر (!محفوظ_isdigit(*p))
    {
	// The command or modifiers must be following.  Assume a lower case
	// character means there is a modifier.
	اگر (p < eap->cmd && !vim_islower(*p))
	{
	    emsg(_(e_invalid_range));
	    واپس -1;
	}
	واپس 0;
    }
    واپس atol((حرف *)p);
}
#اگر_ختم

/*
 * Get the compilation type that should be used for "ufunc".
 * Keep in sync with INSTRUCTIONS().
 */
    compiletype_T
get_compile_type(ufunc_T *ufunc)
{
    // Update uf_has_breakpoint if needed.
    update_has_breakpoint(ufunc);

    اگر (debug_break_level > 0 || may_break_اندر_function(ufunc))
	واپس CT_DEBUG;
#اگر_تعریف_شدہ FEAT_PROFILE
    اگر (do_profiling == PROF_YES)
    {
	اگر (!ufunc->uf_profiling && has_profiling(FALSE, ufunc->uf_name, تہی,
							    &ufunc->uf_hash))
	    func_do_profile(ufunc);
	اگر (ufunc->uf_profiling)
	    واپس CT_PROFILE;
    }
#اگر_ختم
    واپس CT_NONE;
}

/*
 * Free the compiled instructions saved for a def function.  This is used when
 * compiling a def function and the function was compiled before.
 * The index is reused.
 */
    ساکن خالی
clear_def_function(ufunc_T *ufunc, compiletype_T compile_type)
{
    isn_T	*instr_dest = تہی;
    dfunc_T	*dfunc;

    dfunc = ((dfunc_T *)def_functions.ga_ڈیٹا) + ufunc->uf_dfunc_idx;

    انتخاب (compile_type)
    {
	حالت CT_PROFILE:
#اگر_تعریف_شدہ FEAT_PROFILE
	    instr_dest = dfunc->df_instr_prof; توڑو;
#اگر_ختم
	حالت CT_NONE:   instr_dest = dfunc->df_instr; توڑو;
	حالت CT_DEBUG:  instr_dest = dfunc->df_instr_debug; توڑو;
    }

    اگر (instr_dest != تہی)
	// Was compiled in this mode before: Free old instructions.
	delete_def_function_contents(dfunc, FALSE);

    ga_clear_لڑیاں(&dfunc->df_var_نام);
    dfunc->df_defer_var_idx = 0;
}

/*
 * Add a function to the list of :def functions.
 * This sets "ufunc->uf_dfunc_idx" but the function isn't compiled yet.
 */
    ساکن صحیح
جوڑو_def_function(ufunc_T *ufunc)
{
    dfunc_T *dfunc;

    اگر (def_functions.ga_len == 0)
    {
	// The first position is not used, so that a zero uf_dfunc_idx means it
	// wasn't set.
	اگر (GA_GROW_FAILS(&def_functions, 1))
	    واپس FAIL;
	++def_functions.ga_len;
    }

    // Add the function to "def_functions".
    اگر (GA_GROW_FAILS(&def_functions, 1))
	واپس FAIL;
    dfunc = ((dfunc_T *)def_functions.ga_ڈیٹا) + def_functions.ga_len;
    CLEAR_اشارہ_گر(dfunc);
    dfunc->df_idx = def_functions.ga_len;
    ufunc->uf_dfunc_idx = dfunc->df_idx;
    dfunc->df_ufunc = ufunc;
    dfunc->df_name = vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
    ga_init2(&dfunc->df_var_نام, حجم(char_u *), 10);
    ++dfunc->df_refcount;
    ++def_functions.ga_len;
    واپس OK;
}

    ساکن صحیح
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
    ورنہ
    {
	اگر (جوڑو_def_function(ufunc) == FAIL)
	    واپس FAIL;

	*new_def_function = TRUE;
    }

    اگر ((ufunc->uf_flags & FC_احاطہ) && outer_cctx == تہی)
    {
	semsg(_(e_compiling_احاطہ_without_context_str),
						printable_func_name(ufunc));
	واپس FAIL;
    }

    ufunc->uf_def_حالت = UF_COMPILING;

    واپس OK;
}

/*
 * Initialize the compilation context for compiling a def function.
 */
    ساکن خالی
compile_dfunc_cctx_init(
    cctx_T		*cctx,
    cctx_T		*outer_cctx,
    ufunc_T		*ufunc,
    compiletype_T	compile_type)
{
    CLEAR_میدان(*cctx);

    cctx->ctx_compile_type = compile_type;
    cctx->ctx_ufunc = ufunc;
    cctx->ctx_lnum = -1;
    cctx->ctx_outer = outer_cctx;
    ga_init2(&cctx->ctx_locals, حجم(lvar_T), 10);
    // Each entry on the type stack consists of two type pointers.
    ga_init2(&cctx->ctx_type_ڈھیر, حجم(type2_T), 50);
    cctx->ctx_type_list = &ufunc->uf_type_list;
    ga_init2(&cctx->ctx_instr, حجم(isn_T), 50);
}

/*
 * For an object constructor, generate instruction to setup "this" (the first
 * local variable) and to initialize the object variables.
 */
    ساکن صحیح
obj_constructor_prologue(ufunc_T *ufunc, cctx_T *cctx)
{
    generate_CONSTRUCT(cctx, ufunc->uf_class);

    برائے (صحیح i = 0; i < ufunc->uf_class->class_obj_member_گنتی; ++i)
    {
	ocmember_T *m = &ufunc->uf_class->class_obj_members[i];

	اگر (i < 2 && IS_ENUM(ufunc->uf_class))
	    // The first two object variables in an enum are the name
	    // and the ordinal.  These are set by the ISN_CONSTRUCT
	    // instruction.  So don't generate instructions to set
	    // these variables.
	    جاری;

	اگر (m->ocm_init != تہی)
	{
	    char_u	*اظہاریہ = m->ocm_init;
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
		generate_SCRIPTCTX_سیٹ(cctx, current_sctx);
	    }

	    صحیح r = compile_expr0(&اظہاریہ, cctx);

	    اگر (change_sctx)
	    {
		// restore the previous script context
		current_sctx = save_current_sctx;
		generate_SCRIPTCTX_سیٹ(cctx, current_sctx);
	    }

	    اگر (r == FAIL)
		واپس FAIL;

	    اگر (!سرے_excmd2(m->ocm_init, اظہاریہ))
	    {
		semsg(_(e_trailing_characters_str), اظہاریہ);
		واپس FAIL;
	    }

	    type_T	*type = get_type_on_ڈھیر(cctx, 0);
	    اگر (m->ocm_type->tt_type == VAR_ANY
		    && !(m->ocm_flags & OCMFLAG_HAS_TYPE)
		    && type->tt_type != VAR_SPECIAL)
	    {
		// If the member variable type is not yet set, then use
		// the initialization expression type.
		m->ocm_type = type;
	    }
	    ورنہ
	    {
		// The type of the member initialization expression is
		// determined at run time.  Add a runtime type check.
		where_T	where = WHERE_INIT;
		where.wt_قسم = WT_MEMBER;
		where.wt_func_name = (حرف *)m->ocm_name.string;
		اگر (need_type_where(type, m->ocm_type, 0, -1,
					where, cctx, FALSE, FALSE) == FAIL)
		    واپس FAIL;
	    }
	}
	ورنہ
	    ڈالو_default_قدر(cctx, m->ocm_type->tt_type, FALSE, تہی);

	اگر (((m->ocm_type->tt_type == VAR_DICT
			|| m->ocm_type->tt_type == VAR_LIST)
		    && m->ocm_type->tt_member != تہی
		    && m->ocm_type->tt_member != &t_any
		    && m->ocm_type->tt_member != &t_unknown)
		|| (m->ocm_type->tt_type == VAR_TUPLE
		    && m->ocm_type->tt_argcount > 0))
	    // Set the type in the list, tuple or dict, so that it can be
	    // checked, also in legacy script.
	    generate_SETTYPE(cctx, m->ocm_type);

	generate_STORE_THIS(cctx, i);
    }

    واپس OK;
}

/*
 * For an object method and an constructor, generate instruction to setup
 * "this" (the first local variable).  For a constructor, generate instructions
 * to initialize the object variables.
 */
    ساکن صحیح
obj_طریقہ_prologue(ufunc_T *ufunc, cctx_T *cctx)
{
    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_ڈیٹا) + ufunc->uf_dfunc_idx;

    اگر (GA_GROW_FAILS(&dfunc->df_var_نام, 1))
	واپس FAIL;

    ((char_u **)dfunc->df_var_نام.ga_ڈیٹا)[0] =
						vim_strsave((char_u *)"this");
    ++dfunc->df_var_نام.ga_len;

    // In the constructor allocate memory for the object and initialize the
    // object members.
    اگر (IS_CONSTRUCTOR_طریقہ(ufunc))
	واپس obj_constructor_prologue(ufunc, cctx);

    واپس OK;
}

/*
 * Produce instructions for the default values of optional arguments.
 */
    ساکن صحیح
compile_def_function_default_دلائل(
    ufunc_T	*ufunc,
    garray_T	*instr,
    cctx_T	*cctx)
{
    صحیح	گنتی = ufunc->uf_def_دلائل.ga_len;
    صحیح	پہلا_def_arg = ufunc->uf_دلائل.ga_len - گنتی;
    صحیح	i;
    صحیح	off = ڈھیر_چوکھٹا_سائز + (ufunc->uf_va_name != تہی ? 1 : 0);
    صحیح	did_سیٹ_arg_type = FALSE;

    // Produce instructions for the default values of optional arguments.
    SOURCING_LNUM = 0;  // line number unknown
    برائے (i = 0; i < گنتی; ++i)
    {
	char_u *arg = ((char_u **)(ufunc->uf_def_دلائل.ga_ڈیٹا))[i];
	اگر (STRCMP(arg, "v:none") == 0)
	    // "arg = v:none" means the argument is optional without
	    // setting a value when the argument is missing.
	    جاری;

	type_T	*val_type;
	صحیح		arg_idx = پہلا_def_arg + i;
	where_T	where = WHERE_INIT;
	صحیح		jump_instr_idx = instr->ga_len;
	isn_T	*isn;

	// Use a JUMP_IF_ARG_SET instruction to skip if the value was given.
	اگر (generate_JUMP_IF_ARG(cctx, ISN_JUMP_IF_ARG_سیٹ,
						i - گنتی - off) == FAIL)
	    واپس FAIL;

	// Make sure later arguments are not found.
	ufunc->uf_دلائل_visible = arg_idx;

	صحیح r = compile_expr0(&arg, cctx);
	اگر (r == FAIL)
	    واپس FAIL;

	// If no type specified use the type of the default value.
	// Otherwise check that the default value type matches the
	// specified type.
	val_type = get_type_on_ڈھیر(cctx, 0);
	where.wt_اشاریہ = arg_idx + 1;
	where.wt_قسم = WT_دلیل;
	اگر (ufunc->uf_arg_types[arg_idx] == &t_unknown)
	{
	    did_سیٹ_arg_type = TRUE;
	    ufunc->uf_arg_types[arg_idx] = val_type;
	}
	ورنہ اگر (need_type_where(val_type, ufunc->uf_arg_types[arg_idx],
		    0, -1, where, cctx, FALSE, FALSE) == FAIL)
	    واپس FAIL;

	اگر (generate_STORE(cctx, ISN_STORE, i - گنتی - off, تہی) == FAIL)
	    واپس FAIL;

	// set instruction index in JUMP_IF_ARG_SET to here
	isn = ((isn_T *)instr->ga_ڈیٹا) + jump_instr_idx;
	isn->isn_arg.jumparg.jump_where = instr->ga_len;
    }

    اگر (did_سیٹ_arg_type)
	سیٹ_function_type(ufunc);

    واپس OK;
}

/*
 * Compile def function body.  Loop over all the lines in the function and
 * generate instructions.
 */
    ساکن صحیح
compile_def_function_body(
    صحیح		آخری_func_lnum,
    صحیح		check_return_type,
    garray_T	*سطریں_مقصد_free,
    حرف	**errormsg,
    cctx_T	*cctx)
{
    char_u	*سطر = تہی;
    char_u	*p;
    صحیح		did_emsg_before = did_emsg;
#اگر_تعریف_شدہ FEAT_PROFILE
    صحیح		prof_lnum = -1;
#اگر_ختم
    صحیح		debug_lnum = -1;

    برائے (;;)
    {
	exarg_T	    ea;
	صحیح	    starts_with_colon = FALSE;
	char_u	    *cmd;
	cmdmod_T    local_cmdmod;

	// Bail out on the first error to avoid a flood of errors and report
	// the right line number when inside try/catch.
	اگر (did_emsg_before != did_emsg)
	    واپس FAIL;

	اگر (سطر != تہی && *سطر == '|')
	    // the line continues after a '|'
	    ++سطر;
	ورنہ اگر (سطر != تہی && *skipwhite(سطر) != NUL
		&& !(*سطر == '#' && (سطر == cctx->ctx_سطر_آغاز
						    || VIM_ISWHITE(سطر[-1]))))
	{
	    semsg(_(e_trailing_characters_str), سطر);
	    واپس FAIL;
	}
	ورنہ اگر (سطر != تہی && vim9_bad_comment(skipwhite(سطر)))
	    واپس FAIL;
	ورنہ
	{
	    سطر = اگلا_سطر_from_context(cctx, FALSE);
	    اگر (cctx->ctx_lnum >= آخری_func_lnum)
	    {
		// beyond the last line
#اگر_تعریف_شدہ FEAT_PROFILE
		اگر (cctx->ctx_skip != SKIP_YES)
		    may_generate_prof_انتہا(cctx, prof_lnum);
#اگر_ختم
		توڑو;
	    }
	    // Make a copy, splitting off nextcmd and removing trailing spaces
	    // may change it.
	    اگر (سطر != تہی)
	    {
		سطر = vim_strsave(سطر);
		اگر (ga_جوڑو_string(سطریں_مقصد_free, سطر) == FAIL)
		{
		    vim_free(سطر);
		    واپس FAIL;
		}
	    }
	}

	CLEAR_میدان(ea);
	ea.cmdlinep = &سطر;
	ea.cmd = skipwhite(سطر);
	ea.skip = cctx->ctx_skip == SKIP_YES;

	اگر (*ea.cmd == '#')
	{
	    // "#" starts a comment, but "#{" is an error
	    اگر (vim9_bad_comment(ea.cmd))
		واپس FAIL;
	    سطر = (char_u *)"";
	    جاری;
	}

#اگر_تعریف_شدہ FEAT_PROFILE
	اگر (cctx->ctx_compile_type == CT_PROFILE && cctx->ctx_lnum != prof_lnum
						  && cctx->ctx_skip != SKIP_YES)
	{
	    may_generate_prof_انتہا(cctx, prof_lnum);

	    prof_lnum = cctx->ctx_lnum;
	    generate_instr(cctx, ISN_PROF_آغاز);
	}
#اگر_ختم
	اگر (cctx->ctx_compile_type == CT_DEBUG && cctx->ctx_lnum != debug_lnum
						  && cctx->ctx_skip != SKIP_YES)
	{
	    debug_lnum = cctx->ctx_lnum;
	    generate_instr_debug(cctx);
	}
	cctx->ctx_پچھلا_lnum = cctx->ctx_lnum + 1;

	// Some things can be recognized by the first character.
	انتخاب (*ea.cmd)
	{
	    حالت '}':
		{
		    // "}" ends a block scope
		    scopetype_T stype = cctx->ctx_scope == تہی
					  ? NO_SCOPE : cctx->ctx_scope->se_type;

		    اگر (stype == بلاک_SCOPE)
		    {
			compile_endblock(cctx);
			سطر = ea.cmd;
		    }
		    ورنہ
		    {
			emsg(_(e_using_rcurly_outside_if_بلاک_scope));
			واپس FAIL;
		    }
		    اگر (سطر != تہی)
			سطر = skipwhite(ea.cmd + 1);
		    جاری;
		}

	    حالت '{':
		// "{" starts a block scope
		// "{'a': 1}->func() is something else
		اگر (سرے_excmd(*skipwhite(ea.cmd + 1)))
		{
		    سطر = compile_بلاک(ea.cmd, cctx);
		    جاری;
		}
		توڑو;
	}

	/*
	 * COMMAND MODIFIERS
	 */
	cctx->ctx_has_cmdmod = FALSE;
	اگر (parse_command_modifiers(&ea, errormsg, &local_cmdmod, FALSE)
								       == FAIL)
	    واپس FAIL;
	generate_cmdmods(cctx, &local_cmdmod);
	undo_cmdmod(&local_cmdmod);

	// Check if there was a colon after the last command modifier or before
	// the current position.
	برائے (p = ea.cmd; p >= سطر; --p)
	{
	    اگر (*p == ':')
		starts_with_colon = TRUE;
	    اگر (p < ea.cmd && !VIM_ISWHITE(*p))
		توڑو;
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
		ورنہ
		    ea.cmd = skipwhite(ea.cmd);
	    }

	    اگر (!starts_with_colon)
	    {
		صحیح	    تفویض;

		// Check for assignment after command modifiers.
		تفویض = may_compile_assignment(&ea, &سطر, cctx);
		اگر (تفویض == OK)
		    جاؤ nextline;
		اگر (تفویض == FAIL)
		    واپس FAIL;
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
			|| نمبر_طریقہ(cmd))))
	{
	    ea.cmd = skip_range(ea.cmd, TRUE, تہی);
	    اگر (ea.cmd > cmd)
	    {
		اگر (!starts_with_colon
				   && !(local_cmdmod.cmod_flags & CMOD_LEGACY))
		{
		    semsg(_(e_colon_required_before_range_str), cmd);
		    واپس FAIL;
		}
		ea.addr_گنتی = 1;
		اگر (سرے_excmd2(سطر, ea.cmd))
		{
		    // A range without a command: jump to the line.
		    generate_EXEC(cctx, ISN_EXECRANGE,
					      vim_strnsave(cmd, ea.cmd - cmd));
		    سطر = ea.cmd;
		    جاؤ nextline;
		}
	    }
	}
	p = ڈھونڈو_ex_command(&ea, تہی,
		starts_with_colon || (local_cmdmod.cmod_flags & CMOD_LEGACY)
						  ? تہی : شے_exists, cctx);

	اگر (p == تہی)
	{
	    اگر (cctx->ctx_skip != SKIP_YES)
		semsg(_(e_ambiguous_use_کا_user_defined_command_str), ea.cmd);
	    واپس FAIL;
	}

	// When using ":legacy cmd" always use compile_exec().
	اگر (local_cmdmod.cmod_flags & CMOD_LEGACY)
	{
	    char_u *آغاز = ea.cmd;

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
			واپس FAIL;
		طےشدہ: توڑو;
	    }

	    // ":legacy return expr" needs to be handled differently.
	    اگر (checkforcmd(&آغاز, "return", 4))
		ea.cmdidx = CMD_return;
	    ورنہ
		ea.cmdidx = CMD_legacy;
	}

	اگر (p == ea.cmd && ea.cmdidx != CMD_سائز)
	{
	    // "eval" is used for "val->func()" and "var" for "var = val", then
	    // "p" is equal to "ea.cmd" for a valid command.
	    اگر (ea.cmdidx == CMD_eval || ea.cmdidx == CMD_var)
		;
	    ورنہ اگر (cctx->ctx_skip == SKIP_YES)
	    {
		سطر += STRLEN(سطر);
		جاؤ nextline;
	    }
	    ورنہ
	    {
		semsg(_(e_command_not_recognized_str), ea.cmd);
		واپس FAIL;
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
		&& !ignore_unreachable_رمز_for_testing)
	{
	    semsg(_(e_unreachable_رمز_after_str),
				     cctx->ctx_had_return ? "return" : "throw");
	    واپس FAIL;
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
	اگر (ea.cmdidx != CMD_سائز
			    && ea.cmdidx != CMD_write && ea.cmdidx != CMD_read)
	{
	    اگر (ea.cmdidx >= 0)
		ea.argt = excmd_get_argt(ea.cmdidx);
	    اگر ((ea.argt & EX_BANG) && *p == '!')
	    {
		ea.forceit = TRUE;
		p = skipwhite(p + 1);
	    }
	    اگر ((ea.argt & EX_RANGE) == 0 && ea.addr_گنتی > 0)
	    {
		emsg(_(e_no_range_allowed));
		واپس FAIL;
	    }
	}

	انتخاب (ea.cmdidx)
	{
	    حالت CMD_def:
	    حالت CMD_function:
		    ea.arg = p;
		    سطر = compile_nested_function(&ea, cctx, سطریں_مقصد_free);
		    توڑو;

	    حالت CMD_return:
		    سطر = compile_return(p, check_return_type,
				 local_cmdmod.cmod_flags & CMOD_LEGACY, cctx);
		    cctx->ctx_had_return = TRUE;
		    توڑو;

	    حالت CMD_let:
		    emsg(_(e_cannot_use_let_اندر_vim9_script));
		    توڑو;
	    حالت CMD_var:
	    حالت CMD_final:
	    حالت CMD_const:
	    حالت CMD_increment:
	    حالت CMD_decrement:
		    سطر = compile_assignment(p, &ea, ea.cmdidx, cctx);
		    اگر (سطر == p)
		    {
			emsg(_(e_invalid_assignment));
			سطر = تہی;
		    }
		    توڑو;

	    حالت CMD_unlet:
	    حالت CMD_unlockvar:
	    حالت CMD_lockvar:
		    سطر = compile_unletlock(p, &ea, cctx);
		    توڑو;

	    حالت CMD_import:
		    emsg(_(e_import_can_only_be_مستعمل_اندر_script));
		    سطر = تہی;
		    توڑو;

	    حالت CMD_if:
		    سطر = compile_if(p, cctx);
		    توڑو;
	    حالت CMD_elseif:
		    سطر = compile_elseif(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    cctx->ctx_had_throw = FALSE;
		    توڑو;
	    حالت CMD_else:
		    سطر = compile_else(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    cctx->ctx_had_throw = FALSE;
		    توڑو;
	    حالت CMD_endif:
		    سطر = compile_endif(p, cctx);
		    cctx->ctx_had_throw = FALSE;
		    توڑو;

	    حالت CMD_while:
		    سطر = compile_while(p, cctx);
		    توڑو;
	    حالت CMD_endwhile:
		    سطر = compile_endwhile(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    توڑو;

	    حالت CMD_for:
		    سطر = compile_for(p, cctx);
		    توڑو;
	    حالت CMD_endfor:
		    سطر = compile_endfor(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    توڑو;
	    حالت CMD_continue:
		    سطر = compile_continue(p, cctx);
		    // "continue" unconditionally jumps back to the loop,
		    // like a "return".
		    cctx->ctx_had_return = TRUE;
		    توڑو;
	    حالت CMD_break:
		    سطر = compile_break(p, cctx);
		    // "break" unconditionally exits the loop, like a "return".
		    cctx->ctx_had_return = TRUE;
		    توڑو;

	    حالت CMD_try:
		    سطر = compile_try(p, cctx);
		    توڑو;
	    حالت CMD_catch:
		    سطر = compile_catch(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    cctx->ctx_had_throw = FALSE;
		    توڑو;
	    حالت CMD_finally:
		    سطر = compile_finally(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    cctx->ctx_had_throw = FALSE;
		    توڑو;
	    حالت CMD_endtry:
		    سطر = compile_endtry(p, cctx);
		    cctx->ctx_had_throw = FALSE;
		    توڑو;
	    حالت CMD_throw:
		    سطر = compile_throw(p, cctx);
		    cctx->ctx_had_throw = TRUE;
		    توڑو;

	    حالت CMD_eval:
		    سطر = compile_eval(p, cctx);
		    توڑو;

	    حالت CMD_defer:
		    سطر = compile_defer(p, cctx);
		    توڑو;

#اگر_تعریف_شدہ HAS_پیغام_کھڑکی
	    حالت CMD_echowindow:
		    {
			طویل cmd_گنتی = get_cmd_گنتی(سطر, &ea);
			اگر (cmd_گنتی < 0)
			    سطر = تہی;
			ورنہ
			    سطر = compile_mult_اظہاریہ(p, ea.cmdidx,
							     cmd_گنتی, cctx);
		    }
		    توڑو;
#اگر_ختم
	    حالت CMD_echo:
	    حالت CMD_echon:
	    حالت CMD_echoconsole:
	    حالت CMD_echoerr:
	    حالت CMD_echomsg:
	    حالت CMD_execute:
		    سطر = compile_mult_اظہاریہ(p, ea.cmdidx, 0, cctx);
		    توڑو;

	    حالت CMD_put:
		    ea.cmd = cmd;
		    سطر = compile_put(p, &ea, cctx, FALSE);
		    توڑو;

	    حالت CMD_iput:
		    ea.cmd = cmd;
		    سطر = compile_put(p, &ea, cctx, TRUE);
		    توڑو;

	    حالت CMD_substitute:
		    اگر (check_global_and_subst(ea.cmd, p) == FAIL)
			واپس FAIL;
		    اگر (cctx->ctx_skip == SKIP_YES)
			سطر = (char_u *)"";
		    ورنہ
		    {
			ea.arg = p;
			سطر = compile_substitute(سطر, &ea, cctx);
		    }
		    توڑو;

	    حالت CMD_redir:
		    ea.arg = p;
		    سطر = compile_redir(سطر, &ea, cctx);
		    توڑو;

	    حالت CMD_cexpr:
	    حالت CMD_lexpr:
	    حالت CMD_caddexpr:
	    حالت CMD_laddexpr:
	    حالت CMD_cgetexpr:
	    حالت CMD_lgetexpr:
#اگر_تعریف_شدہ FEAT_QUICKFIX
		    ea.arg = p;
		    سطر = compile_cexpr(سطر, &ea, cctx);
#ورنہ
		    ex_ni(&ea);
		    سطر = تہی;
#اگر_ختم
		    توڑو;

	    حالت CMD_append:
	    حالت CMD_change:
	    حالت CMD_داخل_کرو:
	    حالت CMD_k:
	    حالت CMD_open:
	    حالت CMD_t:
	    حالت CMD_xit:
		    not_اندر_vim9(&ea, false);
		    واپس FAIL;

	    حالت CMD_سائز:
		    اگر (cctx->ctx_skip != SKIP_YES)
		    {
			semsg(_(e_invalid_command_str), ea.cmd);
			واپس FAIL;
		    }
		    // We don't check for a next command here.
		    سطر = (char_u *)"";
		    توڑو;

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
		    اگر (vim_strchr(سطر, '\n') == تہی)
			سطر = compile_exec(سطر, &ea, cctx);
		    ورنہ
			// heredoc lines have been concatenated with NL
			// characters in get_function_body()
			سطر = compile_script(سطر, cctx);
		    توڑو;

	    حالت CMD_vim9script:
		    اگر (cctx->ctx_skip != SKIP_YES)
		    {
			emsg(_(e_vim9script_can_only_be_مستعمل_اندر_script));
			واپس FAIL;
		    }
		    سطر = (char_u *)"";
		    توڑو;

	    حالت CMD_class:
		    emsg(_(e_class_can_only_be_مستعمل_اندر_script));
		    واپس FAIL;

	    حالت CMD_enum:
		    emsg(_(e_enum_can_only_be_مستعمل_اندر_script));
		    واپس FAIL;

	    حالت CMD_interface:
		    emsg(_(e_interface_can_only_be_مستعمل_اندر_script));
		    واپس FAIL;

	    حالت CMD_type:
		    emsg(_(e_type_can_only_be_مستعمل_اندر_script));
		    واپس FAIL;

	    حالت CMD_global:
		    اگر (check_global_and_subst(ea.cmd, p) == FAIL)
			واپس FAIL;
		    // FALLTHROUGH
	    طےشدہ:
		    // Not recognized, execute with do_cmdline_cmd().
		    ea.arg = p;
		    سطر = compile_exec(سطر, &ea, cctx);
		    توڑو;
	}
nextline:
	اگر (سطر == تہی)
	    واپس FAIL;
	سطر = skipwhite(سطر);

	// Undo any command modifiers.
	generate_undo_cmdmods(cctx);

	اگر (cctx->ctx_type_ڈھیر.ga_len < 0)
	{
	    iemsg("Type stack underflow");
	    واپس FAIL;
	}
    } // END of the loop over all the function body lines.

    واپس OK;
}

/*
 * Returns TRUE if the end of a scope (if, while, for, block) is missing.
 * Called after compiling a def function body.
 */
    ساکن صحیح
compile_dfunc_scope_انتہا_missing(cctx_T *cctx)
{
    اگر (cctx->ctx_scope == تہی)
	واپس FALSE;

    انتخاب (cctx->ctx_scope->se_type)
    {
	حالت IF_SCOPE:
	    emsg(_(e_missing_endif));
	    توڑو;
	حالت WHILE_SCOPE:
	    emsg(_(e_missing_endwhile));
	    توڑو;
	حالت FOR_SCOPE:
	    emsg(_(e_missing_endfor));
	    توڑو;
	حالت TRY_SCOPE:
	    emsg(_(e_missing_endtry));
	    توڑو;
	حالت بلاک_SCOPE:
	    // end block scope from :try (maybe)
	    compile_endblock(cctx);
	    اگر (cctx->ctx_scope != تہی
		    && cctx->ctx_scope->se_type == TRY_SCOPE)
		emsg(_(e_missing_endtry));
	    ورنہ
		emsg(_(e_missing_rcurly));
	    توڑو;
	طےشدہ:
	    emsg(_(e_missing_rcurly));
    }
    واپس TRUE;
}

/*
 * When compiling a def function, if it doesn't have an explicit return
 * statement, then generate a default return instruction.  For an object
 * constructor, return the object.
 */
    ساکن صحیح
compile_dfunc_generate_default_return(ufunc_T *ufunc, cctx_T *cctx)
{
    // TODO: if a function ends in "throw" but there was a return elsewhere we
    // should not assume the return type is "void".
    اگر (cctx->ctx_had_return || cctx->ctx_had_throw)
	واپس OK;

    اگر (ufunc->uf_ret_type->tt_type == VAR_UNKNOWN)
	ufunc->uf_ret_type = &t_void;
    ورنہ اگر (ufunc->uf_ret_type->tt_type != VAR_VOID
					&& !IS_CONSTRUCTOR_طریقہ(ufunc))
    {
	emsg(_(e_missing_return_statement));
	واپس FAIL;
    }

    // Return void if there is no return at the end.
    // For a constructor return the object.
    اگر (IS_CONSTRUCTOR_طریقہ(ufunc))
    {
	generate_instr(cctx, ISN_RETURN_OBJECT);
	ufunc->uf_ret_type = &ufunc->uf_class->class_object_type;
    }
    ورنہ
	generate_instr(cctx, ISN_RETURN_VOID);

    واپس OK;
}

/*
 * Perform the chores after successfully compiling a def function.
 */
    ساکن خالی
compile_dfunc_epilogue(
    cctx_T	*outer_cctx,
    ufunc_T	*ufunc,
    garray_T	*instr,
    cctx_T	*cctx)
{
    dfunc_T	*dfunc;

    dfunc = ((dfunc_T *)def_functions.ga_ڈیٹا) + ufunc->uf_dfunc_idx;
    dfunc->df_deleted = FALSE;
    dfunc->df_script_seq = current_sctx.sc_seq;

#اگر_تعریف_شدہ FEAT_PROFILE
    اگر (cctx->ctx_compile_type == CT_PROFILE)
    {
	dfunc->df_instr_prof = instr->ga_ڈیٹا;
	dfunc->df_instr_prof_گنتی = instr->ga_len;
    }
    ورنہ
#اگر_ختم
	اگر (cctx->ctx_compile_type == CT_DEBUG)
	{
	    dfunc->df_instr_debug = instr->ga_ڈیٹا;
	    dfunc->df_instr_debug_گنتی = instr->ga_len;
	}
	ورنہ
	{
	    dfunc->df_instr = instr->ga_ڈیٹا;
	    dfunc->df_instr_گنتی = instr->ga_len;
	}
    dfunc->df_varcount = dfunc->df_var_نام.ga_len;
    dfunc->df_has_احاطہ = cctx->ctx_has_احاطہ;

    اگر (cctx->ctx_outer_مستعمل)
    {
	ufunc->uf_flags |= FC_احاطہ;
	اگر (outer_cctx != تہی)
	    ++outer_cctx->ctx_احاطہ_گنتی;
    }

    ufunc->uf_def_حالت = UF_COMPILED;
}

/*
 * Perform the cleanup when a def function compilation fails.
 */
    ساکن خالی
compile_dfunc_ufunc_cleanup(
    ufunc_T	*ufunc,
    garray_T	*instr,
    صحیح		new_def_function,
    حرف	*errormsg,
    صحیح		did_emsg_before,
    cctx_T	*cctx)
{
    dfunc_T	*dfunc;

    dfunc = ((dfunc_T *)def_functions.ga_ڈیٹا) + ufunc->uf_dfunc_idx;

    // Compiling aborted, free the generated instructions.
    clear_instr_ga(instr);
    VIM_CLEAR(dfunc->df_name);
    ga_clear_لڑیاں(&dfunc->df_var_نام);

    // If using the last entry in the table and it was added above, we
    // might as well remove it.
    اگر (!dfunc->df_deleted && new_def_function
			&& ufunc->uf_dfunc_idx == def_functions.ga_len - 1)
    {
	--def_functions.ga_len;
	ufunc->uf_dfunc_idx = 0;
    }
    ufunc->uf_def_حالت = UF_COMPILE_ERROR;

    جب (cctx->ctx_scope != تہی)
	drop_scope(cctx);

    اگر (errormsg != تہی)
	emsg(errormsg);
    ورنہ اگر (did_emsg == did_emsg_before)
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
    garray_T	سطریں_مقصد_free;
    حرف	*errormsg = تہی;	// error message
    cctx_T	cctx;
    garray_T	*instr;
    صحیح		did_emsg_before = did_emsg;
    صحیح		did_emsg_silent_before = did_emsg_silent;
    صحیح		ret = FAIL;
    sctx_T	save_current_sctx = current_sctx;
    صحیح		save_estack_compiling = estack_compiling;
    صحیح		save_cmod_flags = cmdmod.cmod_flags;
    صحیح		do_estack_ڈالو;
    صحیح		new_def_function = FALSE;

    // allocated lines are freed at the end
    ga_init2(&سطریں_مقصد_free, حجم(char_u *), 50);

    // Initialize the ufunc and the compilation context
    اگر (compile_dfunc_ufunc_init(ufunc, outer_cctx, compile_type,
						&new_def_function) == FAIL)
	واپس FAIL;

    compile_dfunc_cctx_init(&cctx, outer_cctx, ufunc, compile_type);

    instr = &cctx.ctx_instr;

    // Set the context to the function, it may be compiled when called from
    // another script.  Set the script version to the most modern one.
    // The line number will be set in next_line_from_context().
    current_sctx = ufunc->uf_script_ctx;
    current_sctx.sc_نسخہ = SCRIPT_نسخہ_VIM9;

    // Don't use the flag from ":legacy" here.
    cmdmod.cmod_flags &= ~CMOD_LEGACY;

    // Make sure error messages are OK.
    do_estack_ڈالو = !estack_چوٹی_is_ufunc(ufunc, 1);
    اگر (do_estack_ڈالو)
	estack_ڈالو_ufunc(ufunc, 1);
    estack_compiling = TRUE;

    // Make sure arguments don't shadow variables in the context
    اگر (check_دلائل_shadowing(ufunc, &cctx) == FAIL)
	جاؤ erret;

    // For an object method and a constructor generate instructions to
    // initialize "this" and the object variables.
    اگر (ufunc->uf_flags & (FC_OBJECT|FC_NEW))
	اگر (obj_طریقہ_prologue(ufunc, &cctx) == FAIL)
	    جاؤ erret;

    اگر (ufunc->uf_def_دلائل.ga_len > 0)
	اگر (compile_def_function_default_دلائل(ufunc, instr, &cctx) == FAIL)
	    جاؤ erret;
    ufunc->uf_دلائل_visible = ufunc->uf_دلائل.ga_len;

    // Compiling an abstract method or a function in an interface is done to
    // get the function type.  No code is actually compiled.
    اگر (ufunc->uf_class != تہی && (IS_INTERFACE(ufunc->uf_class)
						|| IS_ABSTRACT_طریقہ(ufunc)))
    {
	ufunc->uf_def_حالت = UF_NOT_COMPILED;
	ret = OK;
	جاؤ erret;
    }

    // compile the function body
    اگر (compile_def_function_body(ufunc->uf_سطریں.ga_len, check_return_type,
				&سطریں_مقصد_free, &errormsg, &cctx) == FAIL)
	جاؤ erret;

    اگر (compile_dfunc_scope_انتہا_missing(&cctx))
	جاؤ erret;

    اگر (compile_dfunc_generate_default_return(ufunc, &cctx) == FAIL)
	جاؤ erret;

    // When compiled with ":silent!" and there was an error don't consider the
    // function compiled.
    اگر (emsg_silent == 0 || did_emsg_silent == did_emsg_silent_before)
	compile_dfunc_epilogue(outer_cctx, ufunc, instr, &cctx);

    ret = OK;

erret:
    اگر (ufunc->uf_def_حالت == UF_COMPILING)
    {
	// compilation failed. do cleanup.
	compile_dfunc_ufunc_cleanup(ufunc, instr, new_def_function,
				    errormsg, did_emsg_before, &cctx);
    }

    اگر (cctx.ctx_redir_lhs.lhs_name != تہی)
    {
	اگر (ret == OK)
	{
	    emsg(_(e_missing_redir_انتہا));
	    ret = FAIL;
	}
	vim_free(cctx.ctx_redir_lhs.lhs_name);
	vim_free(cctx.ctx_redir_lhs.lhs_پورا);
    }

    current_sctx = save_current_sctx;
    estack_compiling = save_estack_compiling;
    cmdmod.cmod_flags =	save_cmod_flags;
    اگر (do_estack_ڈالو)
	estack_نکالو();

    ga_clear_لڑیاں(&سطریں_مقصد_free);
    free_locals(&cctx);
    ga_clear(&cctx.ctx_type_ڈھیر);
    واپس ret;
}

    خالی
سیٹ_function_type(ufunc_T *ufunc)
{
    صحیح varargs = ufunc->uf_va_name != تہی;
    صحیح argcount = ufunc->uf_دلائل.ga_len;

    // Create a type for the function, with the return type and any
    // argument types.
    // A vararg is included in uf_args.ga_len but not in uf_arg_types.
    // The type is included in "tt_args".
    اگر (argcount > 0 || varargs)
    {
	اگر (ufunc->uf_type_list.ga_itemsize == 0)
	    ga_init2(&ufunc->uf_type_list, حجم(type_T *), 10);
	ufunc->uf_func_type = alloc_func_type(ufunc->uf_ret_type,
					   argcount, &ufunc->uf_type_list);
	// Add argument types to the function type.
	اگر (func_type_جوڑو_arg_types(ufunc->uf_func_type,
				    argcount + varargs,
				    &ufunc->uf_type_list) == FAIL)
	    واپس;
	ufunc->uf_func_type->tt_argcount = argcount + varargs;
	ufunc->uf_func_type->tt_min_argcount =
				      argcount - ufunc->uf_def_دلائل.ga_len;
	اگر (ufunc->uf_arg_types == تہی)
	{
	    صحیح i;

	    // lambda does not have argument types.
	    برائے (i = 0; i < argcount; ++i)
		ufunc->uf_func_type->tt_دلائل[i] = &t_any;
	}
	ورنہ
	    mch_memmove(ufunc->uf_func_type->tt_دلائل,
			 ufunc->uf_arg_types, حجم(type_T *) * argcount);
	اگر (varargs)
	{
	    ufunc->uf_func_type->tt_دلائل[argcount] =
		   ufunc->uf_va_type == تہی ? &t_list_any : ufunc->uf_va_type;
	    ufunc->uf_func_type->tt_flags = TTFLAG_VARARGS;
	}
    }
    ورنہ
	// No arguments, can use a predefined type.
	ufunc->uf_func_type = get_func_type(ufunc->uf_ret_type,
					   argcount, &ufunc->uf_type_list);
}

/*
 * Free all instructions for "dfunc" except df_name.
 */
    ساکن خالی
delete_def_function_contents(dfunc_T *dfunc, صحیح mark_deleted)
{
    صحیح idx;

    // In same cases the instructions may refer to a class in which the
    // function is defined and unreferencing the class may call back here
    // recursively.  Set the df_delete_busy to avoid problems.
    اگر (dfunc->df_delete_busy)
	واپس;
    dfunc->df_delete_busy = TRUE;

    ga_clear(&dfunc->df_def_دلائل_isn);
    ga_clear_لڑیاں(&dfunc->df_var_نام);

    اگر (dfunc->df_instr != تہی)
    {
	برائے (idx = 0; idx < dfunc->df_instr_گنتی; ++idx)
	    delete_instr(dfunc->df_instr + idx);
	VIM_CLEAR(dfunc->df_instr);
    }
    اگر (dfunc->df_instr_debug != تہی)
    {
	برائے (idx = 0; idx < dfunc->df_instr_debug_گنتی; ++idx)
	    delete_instr(dfunc->df_instr_debug + idx);
	VIM_CLEAR(dfunc->df_instr_debug);
    }
#اگر_تعریف_شدہ FEAT_PROFILE
    اگر (dfunc->df_instr_prof != تہی)
    {
	برائے (idx = 0; idx < dfunc->df_instr_prof_گنتی; ++idx)
	    delete_instr(dfunc->df_instr_prof + idx);
	VIM_CLEAR(dfunc->df_instr_prof);
    }
#اگر_ختم

    اگر (mark_deleted)
	dfunc->df_deleted = TRUE;
    اگر (dfunc->df_ufunc != تہی)
	dfunc->df_ufunc->uf_def_حالت = UF_NOT_COMPILED;

    dfunc->df_delete_busy = FALSE;
}

/*
 * When a user function is deleted, clear the contents of any associated def
 * function, unless another user function still uses it.
 * The position in def_functions can be re-used.
 */
    خالی
unlink_def_function(ufunc_T *ufunc)
{
    اگر (ufunc->uf_dfunc_idx <= 0 || def_functions.ga_ڈیٹا == تہی)
	واپس;

    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_ڈیٹا)
	+ ufunc->uf_dfunc_idx;

    اگر (--dfunc->df_refcount <= 0)
	delete_def_function_contents(dfunc, TRUE);
    ufunc->uf_def_حالت = UF_NOT_COMPILED;
    ufunc->uf_dfunc_idx = 0;
    اگر (dfunc->df_ufunc == ufunc)
	dfunc->df_ufunc = تہی;
}

/*
 * Used when a user function refers to an existing dfunc.
 */
    خالی
link_def_function(ufunc_T *ufunc)
{
    اگر (ufunc->uf_dfunc_idx <= 0)
	واپس;

    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_ڈیٹا)
	+ ufunc->uf_dfunc_idx;

    ++dfunc->df_refcount;
}

#اگر defined(EXITFREE)
/*
 * Free all functions defined with ":def".
 */
    خالی
free_def_functions(خالی)
{
    صحیح idx;

    برائے (idx = 0; idx < def_functions.ga_len; ++idx)
    {
	dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_ڈیٹا) + idx;

	delete_def_function_contents(dfunc, TRUE);
	vim_free(dfunc->df_name);
    }

    ga_clear(&def_functions);
}
#اگر_ختم


#اگر_ختم // FEAT_EVAL
